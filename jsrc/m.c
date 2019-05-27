/* Copyright 1990-2010, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Memory Management                                                       */

#ifdef _WIN32
#include <windows.h>
#else
#define __cdecl
#endif
#ifdef _MSC_VER
#pragma warning(disable: 4334)
#endif

#include "j.h"

#define LEAKSNIFF 0

#define ALIGNTOCACHE 0   // set to 1 to align each block to cache-line boundary.  Will reduce cache usage for headers

#define MEMJMASK 0xf   // these bits of j contain subpool #; higher bits used for computation for subpool entries
#define SBFREEBLG (14+PMINL)   // lg2(SBFREEB)
#define SBFREEB (1L<<SBFREEBLG)   // number of bytes that need to be freed before we rescan
#define MFREEBCOUNTING 1   // When this bit is set in mfreeb[], we keep track of max space usage

// Format of h, the 16-bit workarea for free and allocated blocks in main memory.  This is used for in-memory headers for NJS blocks, but not for NJA blocks all on disk
#define AFHRH(a) ((a)->h)    // the workarea
#define AFCHAIN(a) ((a)->kchain.chain)  // the chain field, when the block is not allocated
#define AFPROXYCHAIN(a) ((a)->tproxy.proxychain)  // chain field for base proxies during garbage collection
#define FHRHROOTX 15
#define FHRHROOT (((I)1)<<FHRHROOTX)  // set if the current block is the root (the first of the consecutive blocks making up its allocation)
#define FHRHROOTFREE ((I)2<<(PSIZEL-PMINL))   // If this bit is set at the end of garbage-collection, the whole allocation can be freed.  LSB (precisely, lowest 1-bit) is the size indicator
//
// the lower bits encode the size of the block, by the position of the lowest 1 bit, and in the upper bits either (1) the full size of the block for large allocations
// (2) the offset of the block from the root, for pool allocations.  The following macros define the field
#define FHRHPOOLBIN(h) CTTZ(h)     // pool bin# for free (0 means allo of size PMIN, etc).  If this gives PLIML-PMINL+1, the allocation is a system allo
#define FHRHBINISPOOL(b) ((b)<=(PLIML-PMINL))      // true is this is a pool allo, false if system (b is pool bin #)
#define ALLOJISPOOL(j) ((j)<=PLIML)     // true if pool allo, false if system (j is lg2(requested size))
#define ALLOJBIN(j) ((j)-PMINL)   // convert j (=lg2(size)) to pool bin#
#define FHRHPOOLBINSIZE(b) (PMIN<<(b))        // convert bin# to size for pool bin#
#define FHRHSYSSIZE(h) (((I)1)<<((h)>>(PLIML-PMINL+2)))        // convert h to size for system alloc
#define FHRHSIZE(h) ((FHRHBINISPOOL(FHRHPOOLBIN(h)) ? FHRHPOOLBINSIZE(FHRHPOOLBIN(h)) : FHRHSYSSIZE(h)))
#define FHRHSYSJHDR(j) ((2*j+1)<<(PLIML-PMINL+1))        // convert j (=lg(size)) to h format for a system allo
#define FHRHBININCR(b) ((I)2<<(b))      // when garbage-collecting bin b, add this much to the root for each free block encountered.  This is also the amount by which the h values of successive blocks in an allocation differ
#define FHRHBLOCKOFFSETMASK(b) (FHRHROOTFREE - FHRHBININCR(b))  // for blocks in pool b, mask to use to extract offset to root
#define FHRHBLOCKOFFSET(h,m) ((((h)>>(FHRHROOTX-0))-1) & (((h)&(m))<<(PMINL-1)))     // how far the pool block with h is offset from its root.  0 if FHRHROOT is set.  m is FHRHBLOCKOFFSETMASK
#define FHRHISROOTALLOFREE(h) ((h)&FHRHROOTFREE)   // given the root's h after garbage collection, is the entire allocation free?
#define FHRHROOTADDR(a,m) ((A)((C*)(a) - FHRHBLOCKOFFSET(AFHRH(a),m)))   // address of root for block a.  m is FHRHBLOCKOFFSETMASK
#define FHRHISALLOFREE(a,m) FHRHISROOTALLOFREE(AFHRH(FHRHROOTADDR(a,m)))      // is the given block a free after garbage collection? m is FHRHBLOCKOFFSETMASK
#define FHRHRESETROOT(b) (FHRHROOT + (((I)1)<<(b)))     // value to set root to after garbage-collection if the allocation was NOT freed
#define FHRHENDVALUE(b) (FHRHROOTFREE + (((I)1)<<(b)))     // value representing last+1 block in allo.  Subtract FHRHBININCR to get to previous

// the size of the total allocation of the block for w, always a power of 2
#define alloroundsize(w)  FHRHSIZE(AFHRH(w))

#if (MEMAUDIT==0 || !_WIN32)
#define FREECHK(x) FREE(x)
#else
#define FREECHK(x) if(!FREE(x))SEGFAULT  // crash on error
#endif

static void jttraverse(J,A,AF);

#if LEAKSNIFF
static I leakcode;
static A leakblock;
static I leaknbufs;
#endif

// Return the total length of the data area of y, i. e. the number of bytes from start-of-data to end-of-allocation
// The allocation size depends on the type of allocation
I allosize(A y) {
 if(AFLAG(y)&AFVIRTUAL)R 0;  // if this block is virtual, you can't append to the data, so don't ask about the length
 if(!(AFLAG(y)&(AFNJA))) {
  // normal block, or SMM.  Get the size from the power-of-2 used to allocate it
  R alloroundsize(y) + (C*)y - CAV(y);  // allocated size
 }
 // Must be NJA
 R AM(y);
}


// msize[k]=2^k, for sizes up to the size of an I.  Not used in this file any more
B jtmeminit(J jt){I k,m=MLEN;
 if(jt->tstack==0){  // meminit gets called twice.  Alloc the block only once
  jt->tstack=(A*)MALLOC(NTSTACK);  // bias is 0, because nextpushx starts at 0
  jt->tnextpushx = SZI;  // start storing at position 1 (the chain field in entry 0 is unused)
 }
 jt->mmax =(I)1<<(m-1);
 for(k=PMINL;k<=PLIML;++k){jt->mfree[-PMINL+k].ballo=SBFREEB;jt->mfree[-PMINL+k].pool=0;}  // init so we garbage-collect after SBFREEB frees
 jt->mfreegenallo=-SBFREEB*(PLIML+1-PMINL);   // balance that with negative general allocation
#if LEAKSNIFF
 leakblock = 0;
 leaknbufs = 0;
 leakcode = 0;
#endif
 R 1;
}

// Audit all memory chains to detect overrun
void jtauditmemchains(J jt){
#if MEMAUDIT&16
I Wi,Wj;A Wx; if(jt->peekdata){for(Wi=PMINL;Wi<=PLIML;++Wi){Wj=0; Wx=(jt->mfree[-PMINL+Wi].pool); while(Wx){if(FHRHPOOLBIN(AFHRH(Wx))!=(Wi-PMINL)||(UI4)AFHRH(Wx)!=Wx->fill)SEGFAULT Wx=AFCHAIN(Wx); ++Wj;}}}
#endif
}


F1(jtspcount){A z;I c=0,i,j,*v;A x;
 ASSERTMTV(w);
 GATV0(z,INT,2*(-PMINL+PLIML+1),2); v=AV(z);
 for(i=PMINL;i<=PLIML;++i){j=0; x=(jt->mfree[-PMINL+i].pool); while(x){x=AFCHAIN(x); ++j;} if(j){++c; *v++=(I)1<<i; *v++=j;}}
 v=AS(z); v[0]=c; v[1]=2; AN(z)=2*c;
 RETF(z);
}    /* 7!:3 count of unused blocks */

// Garbage collector.  Called when free has decided a call is needed
B jtspfree(J jt){I i;A p;
 for(i = 0;i<=PLIML-PMINL;++i) {
  // Check each chain to see if it is ready to coalesce
  if(jt->mfree[i].ballo<=0) {
   // garbage collector: coalesce blocks in chain i
   // pass through the chain, incrementing the j field in the base allo for each
   // Also create a 'proxy chain' - one element for each base block processed, not necessarily the base block (because the base block may not be free)
   US incr = FHRHBININCR(i);  // offset to add into root
   US virginbase = FHRHRESETROOT(i);  // value the root has when it is unincremented
   US offsetmask = FHRHBLOCKOFFSETMASK(i);  // mask to use for extracting offset to root
   A baseblockproxyroot = 0;  // init to empty proxy chain
   US freereqd = 0;  // indicate if any fully-freed block is found
   for(p=jt->mfree[i].pool;p;p=AFCHAIN(p)){
#if MEMAUDIT&1
    if(FHRHPOOLBIN(AFHRH(p))!=i)SEGFAULT  // make sure chains are valid
#endif
    A base = FHRHROOTADDR(p,offsetmask);   // address of base
    US baseh = AFHRH(base);  // fetch header for base
    if(baseh==virginbase) {AFPROXYCHAIN(p) = baseblockproxyroot; baseblockproxyroot = p;}  // on first encounter of base block, chain the proxy for it
    AFHRH(base) = baseh += incr;  // increment header in base & restore
    freereqd |= baseh;  // accumulate indication of freed base
   }
   // if any blocks can be freed, pass through the chain to remove them.
   if(FHRHISROOTALLOFREE(freereqd)) {   // if any of the base blocks were freed...
    A survivetail = (A)&jt->mfree[i].pool;  // running pointer to last block in chain of blocks that are NOT dropped off.  Chain is rooted in jt->mfree[i].pool, i. e. it replaces the previous chain there
      // NOTE PUN: AFCHAIN(a) must be offset 0 of a
    for(p=jt->mfree[i].pool;p;p=AFCHAIN(p)){   // for each free block
     if(!FHRHISALLOFREE(p,offsetmask)) {  // if the whole allocation containing this block is NOT deleted...
      AFCHAIN(survivetail)=p;survivetail=p;  // ...add it as tail of survival chain
     }
    }
    AFCHAIN(survivetail)=0;  // terminate the chain of surviving buffers.  We leave the [].pool entry pointing to the free list
   }
   // We have kept the surviving buffers in order because the head of the free list is the most-recently-freed buffer
   // and therefore most likely to be in cache.  This would work better if we could avoid trashing the caches while we chase the chain

   // Traverse the list of base-block proxies.  There is one per base block.  If all blocks were freed, free the whole allocation;
   // otherwise clear the count
   for(p=baseblockproxyroot;p;){A np = AFPROXYCHAIN(p);  // next-in-chain
    A baseblock = FHRHROOTADDR(p,offsetmask);  // get address of corresponding base block
    if(FHRHISROOTALLOFREE(AFHRH(baseblock))){ // Free fully-unused base blocks;
#if 1 || ALIGNTOCACHE   // with short headers, always align to cache bdy
     FREECHK(((I**)baseblock)[-1]);  // If aligned, the word before the block points to the original block address
#else
     FREECHK(baseblock);
#endif
    }else{AFHRH(baseblock) = virginbase;}   // restore the count to 0 in the rest
    p=np;   //  step to next base block
   } 

   // set up for next spfree: set mfreeb to a value such that when SPFREEB bytes have been freed,
   // mfreeb will hit 0, causing a rescan.
   // Account for the buffers that were freed during the coalescing by reducing the number of PSIZEL bytes allocated
   // coalescing doesn't change the allocation, but it does change the accounting.  The change to mfreeb[] must be
   // compensated for by a change to mfreegenallo
   // This elides the step of subtracting coalesced buffers from the number of allocated buffers of size i, followed by
   // adding the bytes for those blocks to mfreebgenallo
   jt->mfreegenallo -= SBFREEB - (jt->mfree[i].ballo & ~MFREEBCOUNTING);  // subtract diff between current mfreeb[] and what it will be set to
   jt->mfree[i].ballo = SBFREEB + (jt->mfree[i].ballo & MFREEBCOUNTING);  // set so we trigger rescan when we have allocated another SBFREEB bytes
  }
 }
 jt->uflags.us.uq.uq_c.spfreeneeded = 0;  // indicate no check needed yet
 R 1;
}    /* free unused blocks */

static F1(jtspfor1){
 RZ(w);
 if(BOX&AT(w)){A*wv=AAV(w); DO(AN(w), if(wv[i])spfor1(wv[i]););}
 else if(AT(w)&TRAVERSIBLE)traverse(w,jtspfor1); 
 if(!ACISPERM(AC(w))) {
#if 0 // wrong?
  if(AFNJA&AFLAG(w)){I j,m,n,p;
   m=SZI*WP(AT(w),AN(w),AR(w)); 
   n=p=m+mhb; 
   j=PMINL; n>>=j; 
   while(n){n>>=1; ++j;} 
   if(p==(I)1<<(j-1))--j;
   jt->spfor+=(I)1<<j;
  }else jt->spfor+=(I)1<<(((MS*)w-1)->j);
#else
  // for NJA allocations with contiguous header, the size is the header size (7+64 words) plus the data size
  // for NJA allocations with separate header, the size is the data size plus the size of the base block
  if(AFNJA&AFLAG(w)) {
   if(AK(w)>0&&AK(w)<=AM(w))jt->spfor += SZI*WP(AT(w),AN(w),64);  // fixed rank of 64 in NJA memory
   else{
    jt->spfor += SZI*((1&&AT(w)&LAST0)+(((AT(w)&NAME?sizeof(NM):0)+(AN(w)<<bplg(AT(w)))+SZI-1)>>LGSZI));  // data size only
    jt->spfor += alloroundsize(w);  // add in the header
   }
  } else {
  // for non-NJA allocations, just take the full size of the block
   jt->spfor += alloroundsize(w);
  }
#endif
 }
 R mtm;
}

F1(jtspfor){A*wv,x,y,z;C*s;D*v,*zv;I i,m,n;
 RZ(w);
 n=AN(w); wv=AAV(w);  v=&jt->spfor;
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 GATV(z,FL,n,AR(w),AS(w)); zv=DAV(z); 
 for(i=0;i<n;++i){
  x=wv[i]; m=AN(x); s=CAV(x);
  ASSERT(LIT&AT(x),EVDOMAIN);
  ASSERT(1>=AR(x),EVRANK);
  ASSERT(vnm(m,s),EVILNAME);
  RZ(y=symbrd(nfs(m,s))); 
  *v=0.0; spfor1(y); zv[i]=*v;
 }
 RETF(z);
}    /* 7!:5 space for named object; w is <'name' */

F1(jtspforloc){A*wv,x,y,z;C*s;D*v,*zv;I i,j,m,n;L*u;LX *yv,c;
 RZ(w);
 n=AN(w); wv=AAV(w);  v=&jt->spfor;
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 GATV(z,FL,n,AR(w),AS(w)); zv=DAV(z);   // zv-> results
 for(i=0;i<n;++i){   // loop over each name given...
  x=wv[i];  // x is the name/number
  I bucketx;  // will be hash/number for the locale
  if(!AR(x)&&AT(x)&INT){
   m=-1; bucketx=IAV(x)[0];   // signal numeric-atom locale; fetch number
  }else{
   m=AN(x); s=CAV(x);
   if(!m){m=sizeof(jt->baselocale); s=jt->baselocale;}
   ASSERT(LIT&AT(x),EVDOMAIN);
   ASSERT(1>=AR(x),EVRANK);
   ASSERT(vlocnm(m,s),EVILNAME);
   bucketx=BUCKETXLOC(m,s);
  }
  y=stfind(m,s,bucketx);   // y is the block for the locale
  ASSERT(y,EVLOCALE);
  *v=(D)(FHRHSIZE(AFHRH(y)));  // start with the size of the locale block (always a normal block)
  spfor1(LOCPATH(y)); spfor1(LOCNAME(y));  // add in the size of the path and name
  m=AN(y); yv=LXAV0(y); 
  for(j=SYMLINFOSIZE;j<m;++j){  // for each name in the locale
   c=yv[j];
   while(c){*v+=sizeof(L); u=c+jt->sympv; spfor1(u->name); spfor1(u->val); c=u->next;}  // add in the size of the name itself and the value, and the L block for the name
  }
  zv[i]=*v;
 }
 RETF(z);
}    /* 7!:6 space for a locale */


F1(jtmmaxq){ASSERTMTV(w); RETF(sc(jt->mmax));}
     /* 9!:20 space limit query */

F1(jtmmaxs){I j,m=MLEN,n;
 RE(n=i0(vib(w)));
 ASSERT(1E5<=n,EVLIMIT);
 j=m-1; DO(m, if(n<=(I)1<<i){j=i; break;});
 jt->mmax=(I)1<<j;
 RETF(mtm);
}    /* 9!:21 space limit set */


// Get total # bytes in use.  That's total allocated so far, minus the bytes in the free lists.
// mfreeb[] is a negative count of blocks in the free list, and biased so the value goes negative
// when garbage-collection is required.  All non-pool allocations are accounted for in
// mfreegenallo
// At init, each mfreeb indicates SBFREEB bytes. mfreegenallo is negative to match that total,
// indicating nothing has really been allocated; that's (PLIML-PMINL+1)*SBFREEB to begin with.  When a block
// is alocated, mfreeb[] increases; when a big block is allocated, mfreegenallo increases by the
// amount of the alllocation, and mfree[-PMINL+] decreases by the amount in all the blocks that are now
// on the free list.
// At coalescing,
// mfreeb is set back to indicate SBFREEB bytes, and mfreegenallo is decreased by the amount of the setback.
I jtspbytesinuse(J jt){I i,totalallo = jt->mfreegenallo&~MFREEBCOUNTING;  // start with bias value
for(i=PMINL;i<=PLIML;++i){totalallo+=jt->mfree[-PMINL+i].ballo&~MFREEBCOUNTING;}  // add all the allocations
R totalallo;
}

// Start tracking jt->bytes and jt->bytesmax.  We indicate this by setting the LSB of EVERY entry of mfreeb
// Also count current space, and set that into jt->bytes and the result of this function
I jtspstarttracking(J jt){I i;
 for(i=PMINL;i<=PLIML;++i){jt->mfree[-PMINL+i].ballo |= MFREEBCOUNTING;}
 jt->mfreegenallo |= MFREEBCOUNTING;  // same for non-pool alloc
 R jt->bytes = spbytesinuse();
}

// Turn off tracking.
void jtspendtracking(J jt){I i;
 for(i=PMINL;i<=PLIML;++i){jt->mfree[-PMINL+i].ballo &= ~MFREEBCOUNTING;}
 R;
}

#if BW==64 && MEMAUDIT&2
// Make sure all deletecounts start at 0
static void auditsimverify0(A w){
 if(!w)R;
 if(AFLAG(w)>>AFAUDITUCX)SEGFAULT   // hang if nonzero count
 if(AFLAG(w)&AFVIRTUAL)auditsimverify0(ABACK(w));  // check backer
 if(AT(w)&(RAT|XNUM)) {A* v=AAV(w);  DO(AT(w)&RAT?2*AN(w):AN(w), if(*v)auditsimverify0(*v); ++v;)}
 if(UCISRECUR(w)){  // process children
  if(AT(w)&BOX){
   I n=AN(w); I af=AFLAG(w);
   A* RESTRICT wv=AAV(w);  // pointer to box pointers
   I wrel = af&AFNJA?(I)w:0;  // If relative, add wv[] to wd; otherwise wv[] is a direct pointer
   if((af&AFNJA)||n==0)R;  // no processing if not J-managed memory (rare)
   DO(n, auditsimverify0((A)(intptr_t)((I)wv[i]+(I)wrel)););
  }else if(AT(w)&FUNC) {V* RESTRICT v=VAV(w);
   auditsimverify0(v->fgh[0]); auditsimverify0(v->fgh[1]); auditsimverify0(v->fgh[2]);
  }else if(AT(w)&RAT|XNUM) {
  }else SEGFAULT  // inadmissible type for recursive usecount
 }
 R;
}

// Simulate tpop on the input block.  If that produces a delete count that equals the usecount,
// recur on children if any.  If it produces a delete count higher than the use count in the block, abort
static void auditsimdelete(A w){I delct;
 if(!w)R;
 if(AN(w)==0xdeadbeefdeadbeef||AN(w)==0xfeeefeeefeeefeee)SEGFAULT
 if((delct = ((AFLAG(w)+=AFAUDITUC)>>AFAUDITUCX))>ACUC(w))SEGFAULT   // hang if too many deletes
 if(AFLAG(w)&AFVIRTUAL && AFLAG(w)&RECURSIBLE)SEGFAULT
 if(delct==ACUC(w)&&AFLAG(w)&AFVIRTUAL){A wb = ABACK(w);
  // we fa() the backer, while we mf() the block itself.  So if the backer is NOT recursive, we have to
  // handle nonrecursive children.  All recursible types will be recursive
  if(AFLAG(w)&AFVIRTUAL && (AT(wb)^AFLAG(wb))&RECURSIBLE)SEGFAULT
  auditsimdelete(wb);  // delete backer of virtual block, recursibly
// obsolete   if(AT(wb)&(RAT|XNUM)) {A* v=AAV(wb);  DO(AT(wb)&RAT?2*AN(wb):AN(wb), if(*v)auditsimdelete(*v); ++v;)}  // finish fa() if nonrecursive
 }
 if(delct==ACUC(w)&&(UCISRECUR(w))){  // we deleted down to 0.  process children
  if(AT(w)&BOX){
   I n=AN(w); I af=AFLAG(w);
   A* RESTRICT wv=AAV(w);  // pointer to box pointers
   I wrel = af&AFNJA?(I)w:0;  // If relative, add wv[] to wd; othewrwise wv[] is a direct pointer
   if((af&AFNJA)||n==0)R;  // no processing if not J-managed memory (rare)
   DO(n, auditsimdelete((A)(intptr_t)((I)wv[i]+(I)wrel)););
  }else if(AT(w)&FUNC) {V* RESTRICT v=VAV(w);
   auditsimdelete(v->fgh[0]); auditsimdelete(v->fgh[1]); auditsimdelete(v->fgh[2]);
  }else if(AT(w)&RAT|XNUM) {A* v=AAV(w);  DO(AT(w)&RAT?2*AN(w):AN(w), if(*v)auditsimdelete(*v); ++v;)
  }else SEGFAULT  // inadmissible type for recursive usecount
 }
 R;
}
// clear delete counts back to 0 for next run
static void auditsimreset(A w){I delct;
 if(!w)R;
 delct = AFLAG(w)>>AFAUDITUCX;   // did this recur?
 AFLAG(w) &= AFAUDITUC-1;   // clear count for next time
 if(AFLAG(w)&AFVIRTUAL){A wb = ABACK(w);
  auditsimreset(wb);  // reset backer of virtual block
  if(AT(wb)&(RAT|XNUM)) {A* v=AAV(wb);  DO(AT(wb)&RAT?2*AN(wb):AN(wb), if(*v)auditsimreset(*v); ++v;)}  // reset children
 }
 if(delct==ACUC(w)&&(UCISRECUR(w))){  // if so, recursive reset
  if(AT(w)&BOX){
   I n=AN(w); I af=AFLAG(w);
   A* RESTRICT wv=AAV(w);  // pointer to box pointers
   I wrel = af&AFNJA?(I)w:0;  // If relative, add wv[] to wd; othewrwise wv[] is a direct pointer
   if((af&AFNJA)||n==0)R;  // no processing if not J-managed memory (rare)
   DO(n, auditsimreset((A)(intptr_t)((I)wv[i]+(I)wrel)););
  }else if(AT(w)&FUNC) {V* RESTRICT v=VAV(w);
   auditsimreset(v->fgh[0]); auditsimreset(v->fgh[1]); auditsimreset(v->fgh[2]);
  }else if(AT(w)&RAT|XNUM) {A* v=AAV(w);  DO(AT(w)&RAT?2*AN(w):AN(w), if(*v)auditsimreset(*v); ++v;)
  }else SEGFAULT  // inadmissible type for recursive usecount
 }
 R;
}

#endif

// Register the value to insert into leak-sniff records
void jtsetleakcode(J jt, I code) {
#if LEAKSNIFF
 if(!leakblock)GAT0(leakblock,INT,10000,1); ras(leakblock);
 leakcode = code;
#endif
}

F1(jtleakblockread){
#if LEAKSNIFF
if(!leakblock)R num[0];
R vec(INT,2*leaknbufs,IAV(leakblock));
#else
R num[0];
#endif
}
F1(jtleakblockreset){
#if LEAKSNIFF
leakcode = 0;
leaknbufs = 0;
R num[0];
#else
R num[0];
#endif
}

// Verify that block w does not appear on tstack more than lim times
void audittstack(J jt){
#if BW==64 && MEMAUDIT&2
 if(jt->audittstackdisabled&1)R;
 A* tstack; I ttop;
 // verify counts start clear
 for(tstack=jt->tstack,ttop=jt->tnextpushx;ttop>0;){I j;
  // loop through each entry, skipping the first which is a chain
  for(j=(ttop-SZI);j&(NTSTACK-1);j-=SZI){
   A stkent = *(A*)((I)tstack+j);
   auditsimverify0(stkent);
  }
  // back up to previous block
  ttop = (ttop-SZI)&-NTSTACK;  // decrement to start of block, will roll over boundary above
  tstack=(A*)*(I*)((I)tstack+j); // back up to data for previous field
 }
 // loop through each block of stack
 for(tstack=jt->tstack,ttop=jt->tnextpushx;ttop>0;){I j;
  // loop through each entry, skipping the first which is a chain
  for(j=(ttop-SZI);j&(NTSTACK-1);j-=SZI){
   A stkent = *(A*)((I)tstack+j);
   auditsimdelete(stkent);
  }
  // back up to previous block
  ttop = (ttop-SZI)&-NTSTACK;  // decrement to start of block, will roll over boundary above
  tstack=(A*)*(I*)((I)tstack+j); // back up to data for previous field
 }
 // again to clear the counts
 for(tstack=jt->tstack,ttop=jt->tnextpushx;ttop>0;){I j;
  // loop through each entry, skipping the first which is a chain
  for(j=(ttop-SZI);j&(NTSTACK-1);j-=SZI){
   A stkent = *(A*)((I)tstack+j);
   auditsimreset(stkent);
  }
  // back up to previous block
  ttop = (ttop-SZI)&-NTSTACK;  // decrement to start of block, will roll over boundary above
  tstack=(A*)*(I*)((I)tstack+j); // back up to data for previous field
 }
#endif
}

// Free all symbols pointed to by the SYMB block w.
static void freesymb(J jt, A w){I j,wn=AN(w); LX k,kt,* RESTRICT wv=LXAV0(w);
 L *jtsympv=jt->sympv;  // Move base of symbol block to a register.  Block 0 is the base of the free chain.  MUST NOT move the base of the free queue to a register,
  // because when we free a locale it frees its symbols here, and one of them might be a verb that contains a nested SYMB, giving recursion.  It is safe to move sympv to a register because
  // we know there will be no allocations during the free process.
 // First, free the path and name (in the SYMLINFO block), and then free the SYMLINFO block itself
 if(k=wv[SYMLINFO]){  // The LINFO block might not have been allocated (for local symbol tables)
  fr(LOCPATH(w));
  fr(LOCNAME(w));
  // clear the data fields   kludge but this is how it was done (should be done in symnew)
  jtsympv[k].name=0;jtsympv[k].val=0;jtsympv[k].sn=0;jtsympv[k].flag=0;
  jtsympv[k].next=jtsympv[0].next;jtsympv[0].next=k;  // jt->sympv[0] is the base of the free chain
 }
 // loop through each hash chain, clearing the blocks in the chain
 for(j=SYMLINFOSIZE;j<wn;++j){
  // free the chain; kt->last block freed
  if(k=wv[j]){
   do{kt=k;fr(jtsympv[k].name);fa(jtsympv[k].val);jtsympv[k].name=0;jtsympv[k].val=0;jtsympv[k].sn=0;jtsympv[k].flag=0;k=jtsympv[k].next;}while(k);  // prev for 18!:31
   // if the chain is not empty, make it the base of the free pool & chain previous pool from it
   jtsympv[kt].next=jtsympv[0].next;jtsympv[0].next=wv[j];
  }
 }
}

static void jttraverse(J jt,A wd,AF f){
 switch(CTTZ(AT(wd))){
  case XDX:
   {DX*v=(DX*)AV(wd); DO(AN(wd), if(v->x)CALL1(f,v->x,0L); ++v;);} break;
  case RATX:  
   {A*v=AAV(wd); DO(2*AN(wd), if(*v)CALL1(f,*v++,0L););} break;
  case XNUMX: case BOXX:
   if(!(AFLAG(wd)&AFNJA)){A*wv=AAV(wd);
   {DO(AN(wd), if(wv[i])CALL1(f,wv[i],0L););}
   }
   break;
  case VERBX: case ADVX:  case CONJX: 
   {V*v=FAV(wd); if(v->fgh[0])CALL1(f,v->fgh[0],0L); if(v->fgh[1])CALL1(f,v->fgh[1],0L); if(v->fgh[2])CALL1(f,v->fgh[2],0L);} break;
  case SB01X: case SINTX: case SFLX: case SCMPXX: case SLITX: case SBOXX:
   {P*v=PAV(wd); if(SPA(v,a))CALL1(f,SPA(v,a),0L); if(SPA(v,e))CALL1(f,SPA(v,e),0L); if(SPA(v,i))CALL1(f,SPA(v,i),0L); if(SPA(v,x))CALL1(f,SPA(v,x),0L);} break;
 }
}

void jtfh(J jt,A w){fr(w);}

// overview of the usecount routines
//
// gc() protects a result, and pops the stack.  It preserves inplacing and virtuality if possible.  It cannot be used on blocks
//   that contain contents younger than the block
// gc3() is a simple-minded gc() that works on all blocks, and can handle up to 3 at a time.
// virtual() creates a virtual block that refers to a part of another block.  It looks at the inplacing flags to see if it can get away with modifying the
//    block given rather than creating a new one
// realize() creates a real block that has the contents referred to by a virtual block
// realizeifvirtual() does what its name implies.
// ra(x) raises the usecount of a block and its descendants.  It traverses, stopping a path when it becomes recursible.  It marks its result recursible.  x may not be 0, and may be modified.
// ras() does realizeifvirtual() followed by ra().  x may be 0, and may be modified
// rat() does ras() followed by tpush().  It is used to protect a result over some operation other than tpop()
// fa() lowers the usecount of a block and its descendants.  It traverses and stops a path that is recursible and has usecount going in > 1.  If the usecount is reduced to 0, the block is freed with mf()
// tpush() puts a block and its descendants onto the stack.  In effect this is a call for a later fa().  It traverses, stopping a path when it becomes recursible.  Every block allocated
//   by ga*() starts out with a tpush already performed, which is how blocks are normally freed.
// tpush1() puts the block onto the stack, but does not recur to descendants.  Used for virtual blocks only
// tpop() processes the stack up to a given point.  The usecount is decremented; if it goes to 0 the block is freed by mf() if not recursible, or by fa() if recursible.
//       The fa() will free the descendants.
// ga*() allocates a block and does an initial tpush()
// mf() frees a block.  If what if freed is a symbol table, all the symbols are freed first.

// mark w incorporated, reassigning if necessary.  Return the address of the block.  Used when w is an rvalue
A jtincorp(J jt, A w) {RZ(w); INCORP(w); R w;}

// allocate a virtual block, given the backing block
// offset is offset in atoms from start of w; r is rank
// result block is never inplaceable, never recursible, virtual.  Can return 0 if allocation error
// This is inplaceable, and we inplace the w block.  'Inplaceable' here includes being the target of jt->assignsym
// We fill in everything but AN and AS, which are done in the caller
// You should be wary of making an NJA block virtual, because with a usecount of 1 it might be inplaced by the code for x,y or x u}y
RESTRICTF A jtvirtual(J jtip, AD *RESTRICT w, I offset, I r){AD* RESTRICT z;
 J jt=(J)(intptr_t)((I)jtip&~JTFLAGMSK);  // get flag-free pointer to J block
 ASSERT(RMAX>=r,EVLIMIT);
 I t=AT(w);  // type of input
 offset<<=bplg(t);  // length of an atom of t
 I c=AC(w);  // count of input
 I wf=AFLAG(w);  // flags in input
  // If this is an inplaceable request for an inplaceable DIRECT block, we don't need to create a new virtual block: just modify the offset in the old block.  Make sure the shape fits
  // if the block is UNINCORPABLE, we don't modify it, because then we would have to check everywhere to see if a parameter block had changed
 if(((I)jtip&JTINPLACEW) && t&DIRECT && AR(w)>=r && ASGNINPLACE(w) && !(wf&AFUNINCORPABLE)){
  // virtual-in-place.  There's nothing to do but change the pointer and fill in the new rank.  AN and AS are handled in the caller
  AK(w)+=offset; AR(w)=(RANKT)r;
  R w;
 }else{
  // not self-virtual block: allocate a new one
  RZ(z=gafv(SZI*(NORMAH+r)));  // allocate the block
  AFLAG(z)=AFVIRTUAL;  // flags: not recursive, not UNINCORPABLE
  AC(z)=ACUC1; AT(z)=t; AK(z)=(CAV(w)-(C*)z)+offset; AR(z)=(RANKT)r;  // virtual, not inplaceable
  // If w is inplaceable and inplacing is enabled, we could transfer the inplaceability to the new virtual block.  We choose not to, because we have already picked up
  // virtual-in-place cases above.  The main case would be an inplaceable UNINCORPABLE block, which might be worth the trouble.
  if(AFLAG(w)&AFVIRTUAL){
   // If w is virtual, me must disallow inplacing for it, since it may be at large in the execution and we are creating an alias to it
   ACIPNO(w);  // turn off inplacing
   w=ABACK(w);  // if w is itself virtual, use its original backer.  Otherwise we would have trouble knowing when the backer for z is freed.  Backer is never virtual
  }
  ABACK(z)=w;   // set the pointer to the base: w or its base
  ra(w);   // ensure that the backer is not deleted while it is a backer.  This means that all backers are RECURSIBLE
  R z;
 }
}  


// realize a virtual block (error if not virtual)
// allocate a new block, copy the data to it.  result is address of new block; can be 0 if allocation failure
// only non-sparse nouns can be virtual
// If the input is not virtual, just return it
A jtrealize(J jt, A w){A z; I t;
// allocate a block of the correct type and size.  Copy the shape
 RZ(w);
 t=AT(w);
 GA(z,t,AN(w),AR(w),AS(w));
 // new block is not VIRTUAL, not RECURSIBLE
// copy the contents.
 MC(AV(z),AV(w),AN(w)<<bplg(t));
 R z;
}

// Free temporary buffers, while preventing the result from being freed
//
// Here w is a result that needs to be protected against being deleted.  We increment its usecount,
// pop all the blocks we have allocated, then put w back on that stack to be deleted later.  After
// this, w has the same status as a block allocated in the program that called the one that called gc.
//
// Additional subtlety is needed to get the most out of inplacing.  If w is inplaceable, it
// should remain inplaceable after we finish, because by definition we are through with it.  So
// we need to revert the usecount to inplaceable in that case.  But there's more: if the block
// was inplaced by the program calling gc, it will have been allocated back up the stack.  In that case,
// the tpop will not free it and it will be left with a usecount of 2, preventing further inplacing.
//
// To solve both problems, we check to see if w is inplaceable.  If it is, we restore it to inplaceable
// after the tpop.  But if its usecount after tpop was 2, we do not do the tpush.
//
// If w is a virtual block, we avoid realizing it unless its backing block is deleted
//
// result is the address of the block, which may have changed if it had to be realized.  result can be 0
// if the block could not be realized


A jtgc (J jt,A w,I old){
 RZ(w);  // return if no input (could be error or unfilled box)
 I c=AC(w);  // remember original usecount/inplaceability
 // We want to avoid realizing w if possible, so we handle virtual w separately
 if(AFLAG(w)&AFVIRTUAL){
  if(!(AFLAG(w)&AFUNINCORPABLE)){
   A b=ABACK(w);  // backing block for w.  It is known to be direct or recursible, and had its usecount incremented by w
   // Raise the count of w to protect it.  Since w raised the count of b when w was created, this protects b also.  Afterwards, if
   // b need not be deleted, w can survive as is; but if b is to be deleted, we must realize w.  We don't keep b around because it may be huge
   // (could look at relative size to make this decision).  Because virtual blocks are never assigned or ra()d, we know that the usecount of w
   // coming in is 1
   AC(w)=2;  // protect w from being freed
   tpop(old);  // delete everything allocated on the stack, except for w and b which were protected
   // if the block backing w must be deleted, we must realize w to protect it; and we must also ra() w to protect its contents.  When this is
   // finished, we have a brand-new w with usecount necessarily 1, so we can make it in-placeable.  Setting the usecount to inplaceable will undo the ra() for the top block only
   if(AC(b)<=1){A origw = w; RZ(w=realize(w)); ra(w); AC(w)=ACUC1|ACINPLACE; fa(b); mf(origw); }  // if b is about to be deleted, get w out of the way.  Since we
                                      // raised the usecount of w only, we use mf rather than fa to free just the virtual block
                                      // fa the backer to undo the ra when the virtual block was created
   else{
    // if the backing block survives, w can continue as virtual; we must undo the increment above.  If the usecount was changed by the tpop, we must replace
    // the stack entry.  Otherwise we can keep the stack entry we have, wherever it is, but we must restore the usecount to its original value.  In case we ever want
    // to have virtual inplaceable blocks (imaginable for rank)
    if(AC(w)<2)tpush1(w);  // if the stack entry for w was removed, restore it
    AC(w)=c;  // restore initial usecount and inplaceability
   }
  } else {
   // w was UNINCORPABLE.  That happens only if it is returned from a function called by the function in which it was created.  Therefore, w must not be on the stack
   // and we don't have to go through the trouble of protecting it.  And good thing, too, because if w is a faux block its backer has not had its usecount incremented, and
   // we would end up trying to free the faux block in the code above.  All we need to do is free the stack.
   tpop(old);
  }
  R w;  // if realize() failed, this could be returning 0
 }
 // non-VIRTUAL path
 ra(w);  // protect w and its descendants from tpop; also converts w to recursive usecount.
  // if we are turning w to recursive, this is the last pass through all of w incrementing usecounts.  All currently-on-stack pointers to blocks are compatible with the increment
 tpop(old);  // delete everything allocated on the stack, except for w which was protected
 // Now we need to undo the effect of the initial ra and get the usecount back to its original value, with a matching tpush on the stack.
 // We could just do a tpush of the new block, but (1) we would just as soon do fa() rather than tpush() to save the overhead; (2) if the block was originally inplaceable
 // we would like to continue with it inplaceable.  The interesting case is when the block was NOT freed during the tpop.  That means that
 // the block was allocated somewhere else, either farther up the stack or in a name.  If the block is in a name, we must NOT do fa(), in case the
 // name is reassigned, freeing the components, while one of the components is a result.  OTOH, if the block actually is on the stack, it is safe
 // to do the fa().  We don't get it exactly right, but we note that any block that is part of a name will not be inplaceable, so we do the fa() only if
 // w is inplaceable - and in that case we can make the result here also inplaceable.  If the block was not inplaceable, or if it was freed during the tpop,
 // we push it again here.  In any case, if the input was inplaceable, so is the result.
 // If w was virtual, it cannot have been inplaceable, and will always go through the tpush path, whether it was realized or not
 //
 // NOTE: certain functions (ex: rational determinant) perform operations 'in place' on non-direct names and then protect those names using gc().  The protection is
 // ineffective if the code goes through the fa() path here, because components that were modified will be freed immediately rather than later.  In those places we
 // must either use gc3() which always does the tpush, or do ACIPNO to force us through the tpush path here.  We generally use gc3().
 if((c&(1-AC(w)))<0){fa(w);} else {tpush(w);}  // test is c<0 && AC(w)>1
 // The usecount of w is now back to where it started, or possibly lower, if the block was popped multiple times.
 // But we know for sure that if the block was inplaceable to begin with, its usecount is 1 now, and we should make it inplaceable on exit
 // Note: a block that was originally VIRTUAL cannot have been inplaceable
 if(c<0)AC(w) = c;  // restore inplaceability.  Could use AC(w)=(c<0)?c:AC(w) to avoid conditional jump
 R w;
}

// similar to jtgc, but done the simple way, by ra/pop/push always.  This is the thing to use if the argument
// is nonstandard, such as an argument that is operated on in-place with the result that the contents are younger than
// the enclosing area.  Modify the args if they need to be realized
// If the arguments are virtual, they will be realized
I jtgc3(J jt,A *x,A *y,A *z,I old){
 if(x)RZ(ras(*x)); if(y)RZ(ras(*y)); if(z)RZ(ras(*z));
 tpop(old);
 if(x)tpush(*x); if(y)tpush(*y); if(z)tpush(*z);
 R 1;  // good return
}

// This routine handles the recursion for ra().  ra() itself does the top level, this routine handles the contents
I jtra(J jt,AD* RESTRICT wd,I t){I af=AFLAG(wd); I n=AN(wd);
 if(t&BOX){AD* np;
  // boxed.  Loop through each box, recurring if called for.  Two passes are intertwined in the loop
  A* RESTRICT wv=AAV(wd);  // pointer to box pointers
  I wrel = af&AFNJA?(I)wd:0;  // If relative, add wv[] to wd; otherwise wv[] is a direct pointer
  if((af&AFNJA)||n==0)R 0;  // no processing if not J-managed memory (rare)
  np=(A)(intptr_t)((I)*wv+(I)wrel); ++wv;  // point to block for the box
  while(1){AD* np0;  // n is always > 0 to start
   if(--n<0)break;
   if(n){   // mustn't read past the end of the block, in case of protection check
    np0=(A)(intptr_t)((I)*wv+(I)wrel); ++wv;  // point to block for next box
#ifdef PREFETCH
    PREFETCH((C*)np0);   // prefetch the next box
#endif
   }
   if(np){
    ra(np);  // increment the box, possibly turning it to recursive
   }
   np=np0;  // advance to next box
  }
 } else if(t&(VERB|ADV|CONJ)){V* RESTRICT v=FAV(wd);
  // ACV.  Recur on each component
  ras(v->fgh[0]); ras(v->fgh[1]); ras(v->fgh[2]);
 } else if(t&(RAT|XNUM|XD)) {A* RESTRICT v=AAV(wd);
  // single-level indirect forms.  handle each block
  DO(t&RAT?2*n:n, if(*v)ACINCR(*v); ++v;);
 } else if(t&SPARSE){P* RESTRICT v=PAV(wd); A x;
  // all elements of sparse blocks are guaranteed non-virtual, so ra will not reassign them
  x = SPA(v,a); ras(x);     x = SPA(v,e); ras(x);     x = SPA(v,i); ras(x);     x = SPA(v,x); ras(x);
 }
 R 1;
}

// This handles the recursive part of fa(), freeing the contents of wd
I jtfa(J jt,AD* RESTRICT wd,I t){I af=AFLAG(wd); I n=AN(wd);
 if(t&BOX){AD* np;
  // boxed.  Loop through each box, recurring if called for.
  A* RESTRICT wv=AAV(wd);  // pointer to box pointers
  I wrel = af&AFNJA?(I)wd:0;  // If relative, add wv[] to wd; othewrwise wv[] is a direct pointer
  if((af&AFNJA)||n==0)R 0;  // no processing if not J-managed memory (rare)
  np=(A)(intptr_t)((I)*wv+(I)wrel); ++wv;   // point to block for box
  while(1){AD* np0;
   if(--n<0)break;
   if(n){
    np0=(A)(intptr_t)((I)*wv+(I)wrel); ++wv;   // point to block for next box
#ifdef PREFETCH
    PREFETCH((C*)np0);   // prefetch the next box
#endif
   }
   fana(np);  // free the contents, but don't audit
   np = np0;  // advance to next box
  }
 } else if(t&(VERB|ADV|CONJ)){V* RESTRICT v=FAV(wd);
  // ACV.
  fana(v->fgh[0]); fana(v->fgh[1]); fana(v->fgh[2]);
 } else if(t&(RAT|XNUM|XD)) {A* RESTRICT v=AAV(wd);
  // single-level indirect forms.  handle each block
  DO(t&RAT?2*n:n, if(*v)fr(*v); ++v;);
 } else if(t&SPARSE){P* RESTRICT v=PAV(wd);
  fana(SPA(v,a)); fana(SPA(v,e)); fana(SPA(v,i)); fana(SPA(v,x)); 
 }
 R 1;
}


// Push wd onto the pop stack, and its descendants, possibly recurring on the descendants
// Result is new value of jt->tnextpushx, or 0 if error
// Note: wd CANNOT be virtual
I jttpush(J jt,AD* RESTRICT wd,I t,I pushx){I af=AFLAG(wd); I n=AN(wd);
 if(t&BOX){
  // boxed.  Loop through each box, recurring if called for.
  A* RESTRICT wv=AAV(wd);  // pointer to box pointers
  A* tstack=jt->tstack;  // base of current output block
  if((af&AFNJA)||n==0)R pushx;  // no processing if not J-managed memory (rare)
  while(n--){
   A np=*wv; ++wv;   // point to block for box
   if(np){     // it can be 0 if there was error
    I tp=AT(np); I flg=AFLAG(np); // fetch type
    *(A*)((I)tstack+pushx)=np;  // put the box on the stack
      // Don't bother to prefetch, since we do so little with the fetched word
    pushx += SZI;  // advance to next output slot
    if(!(pushx&(NTSTACK-1))){
     // pushx has crossed the block boundary.  Allocate a new block.
     RZ(tstack=tg(pushx)); pushx+=SZI;   // If error, abort with values set; if not, step pushx over the chain field
    } // if the buffer ran out, allocate another, save its address
    if(!ACISPERM(AC(np))&&(tp^flg)&TRAVERSIBLE){RZ(pushx=jttpush(jt,np,tp,pushx)); tstack=jt->tstack;}  // if NOT recursive usecount, recur, and restore stack pointers after recursion
   }
  }

 } else if(t&(VERB|ADV|CONJ)){V* RESTRICT v=FAV(wd);
  // ACV.  Recur on each component
  if(v->fgh[0])tpushi(v->fgh[0]); if(v->fgh[1])tpushi(v->fgh[1]); if(v->fgh[2])tpushi(v->fgh[2]);
 } else if(t&(RAT|XNUM|XD)) {A* RESTRICT v=AAV(wd);
  // single-level indirect forms.  handle each block
  DO(t&RAT?2*n:n, if(*v)tpushi(*v); ++v;);
 } else if(t&SPARSE){P* RESTRICT v=PAV(wd);
  if(SPA(v,a))tpushi(SPA(v,a)); if(SPA(v,e))tpushi(SPA(v,e)); if(SPA(v,x))tpushi(SPA(v,x)); if(SPA(v,i))tpushi(SPA(v,i));
 }
 R pushx;
}

// Result is address of new stack block.  pushx must have just rolled over, i. e. is the 0 entry for the new block
A* jttg(J jt, I pushx){     // Filling last slot; must allocate next page.  Caller is responsible for advancing pushx
 if(jt->tstacknext) {   // if we already have a page to move to
//  jt->tstacknext[0] = jt->tstack;   // next was chained to prev before it was saved as next
  jt->tstack = (A*)((I)jt->tstacknext-pushx);   // set new buffer as current
  jt->tstacknext = 0;    // indicate no new one available now
 } else {A *v;   // no page to move to - better read one
  // We don't account for the NTSTACK blocks as part of memory space used, because it's so unpredictable and large as to be confusing
  if(!(v=MALLOC(NTSTACK))){  // Allocate block
   // Unable to allocate a new block.  This is catastrophic, because we have done ra for blocks that we
   // will now not be able to tpop.  Memory is going to be lost.  The best we can do is prevent a crash.
   // We will leave tstack as is, pointing to the last block, and set nextpushx to the last entry in it.
   // This loses the last entry in the last block, and all the tpushes we couldn't perform.
   // The return will go all the way back to the first caller and beyond, so we set the values in jt as best we can
   jt->tnextpushx = pushx-SZI;
   ASSERT(v,EVWSFULL);   // this always fails
  }
  *v = (A)jt->tstack;   // backchain old buffers to new, including bias
  jt->tstack = (A*)((I)v-pushx);    // set new buffer as the one to use, biased so we can index it from pushx
 }
 R jt->tstack;  // Return base address of block, biased
}


// pop stack,  ending when we have freed the entry with tnextpushx==old.  tnextpushx is left pointing to an empty slot
// return value is pushx
// If the block has recursive usecount, decrement usecount in children if we free it
I jttpop(J jt,I old){I pushx=jt->tnextpushx; I endingtpushx;
 if(old>=pushx)R pushx;  // return fast if nothing to do
 while(1) {A np;  // loop till end.  Return is at bottom of loop
  endingtpushx = MAX(old,SZI+((pushx-SZI)&-NTSTACK));  // Get # of frees we can perform in this tstack block.  Could be 0
  I nfrees=(A*)pushx-(A*)endingtpushx;
  A* RESTRICT fp = (A*)((I)jt->tstack+(pushx-SZI));  // point to first slot to free, possibly rolling to end of block
  np=*fp--;   // point to block to be freed
  while(--nfrees>=0){A np0;
   // It is OK to prefetch the next box even on the last pass, because the next pointer IS a pointer to a valid box, or a chain pointer
   // to the previous free block (or 0 at end), al of which is OK to read and then prefetch from
   np0=*fp--;   // point to next block
   I c=AC(np);  // fetch usecount
#ifdef PREFETCH
   PREFETCH((C*)np0);   // prefetch the next box
#endif
#if MEMAUDIT&2
   jt->tnextpushx -= SZI;  // remove the buffer-to-be-freed from the stack for auditing
#endif
   // We never tpush a PERMANENT block so we needn't check for it
   if(--c<=0){if(AFLAG(np)&AFVIRTUAL){A b=ABACK(np); fana(b);} if(UCISRECUR(np)){fana(np);}else{mf(np);}}else AC(np)=c;  // decrement usecount and either store it back or free the block
   np=np0;  // Advance to next block
  }
  // See if there are more blocks to do
  if(endingtpushx>old){      // If we haven't done them all, we must have hit start-of-block.  Move back to previous block
   if(jt->tstacknext)FREECHK(jt->tstacknext);   // We will set the block we are vacating as the next-to-use.  We can have only 1 such; if there is one already, free it
   // move the start pointer forward; past old, if this is the last pass
   pushx=endingtpushx-SZI;  // back up to slot 0, so when the next ending address is calculated, it goes all the way back to beginning of block
   jt->tstacknext=(A*)((I)jt->tstack+pushx);  // save the next-to-use, after removing bias
   jt->tstack=(A*)jt->tstacknext[0];   // back up to the previous block (including bias), leaving tstacknext pointing to free buffer
#if MEMAUDIT&2
   jt->tnextpushx -= SZI;  // skip the chain field on the stack for auditing
#endif
  } else {
   // The return point:
#if MEMAUDIT&2
   jt->tnextpushx -= endingtpushx;
   audittstack(jt);   // one audit for each tpop.  Mustn't audit inside tpop loop, because that's inconsistent state
#endif
   R jt->tnextpushx=endingtpushx;  // On last time through, update starting pointer for next push, and return that value
  }
 }
}


// Protect a value temporarily
// w is a block that we want to make ineligible for inplacing.  We increment its usecount (which protects it) and tpush it (which
// undoes the incrementing after we are gone).  The protection lasts until the tpop for the stack level in effect at the call to here.
// Protection is needed only for names, for example in samenoun =: (samenoun verb verb) samenoun  where we must make sure
// the noun is not operated on inplace lest it destroy the value stored in the fork, which might be saved in an explicit definition.
// If the noun is assigned as part of a named derived verb, protection is not needed (but harmless) because if the same value is
// assigned to another name, the usecount will be >1 and therefore not inplaceable.  Likewise, the the noun is non-DIRECT we need
// only protect the top level, because if the named value is incorporated at a lower level its usecount must be >1.
F1(jtrat){RZ(w); ras(w); tpush(w); R w;}  // recursive.  w can be zero only if explicit definition had a failing sentence

A jtras(J jt, AD * RESTRICT w) { RZ(w); realizeifvirtual(w); ra(w); R w; }  // subroutine version of ra() to save space
A jtrifvs(J jt, AD * RESTRICT w) { RZ(w); realizeifvirtual(w); R w; }  // subroutine version of rifv() to save space and be an rvalue

#if MEMAUDIT&8
static I lfsr = 1;  // holds varying memory pattern
#endif

// static auditmodulus = 0;
RESTRICTF A jtgaf(J jt,I blockx){A z;I mfreeb;I n = (I)1<<blockx;
// audit free chain I i,j;MS *x; for(i=PMINL;i<=PLIML;++i){j=0; x=(jt->mfree[-PMINL+i].pool); while(x){x=(MS*)(x->a); if(++j>25)break;}}  // every time, audit first 25 entries
// audit free chain if(++auditmodulus>25){auditmodulus=0; for(i=PMINL;i<=PLIML;++i){j=0; x=(jt->mfree[-PMINL+i].pool); while(x){x=(MS*)(x->a); ++j;}}}
// use 6!:5 to start audit I i,j;MS *x; if(jt->peekdata){for(i=PMINL;i<=PLIML;++i){j=0; x=(jt->mfree[-PMINL+i].pool); while(x){x=(MS*)(x->a); ++j;}}}
#if MEMAUDIT&16
auditmemchains();
#endif
#if MEMAUDIT&15
if((I)jt&3)SEGFAULT
#endif
 z=jt->mfree[-PMINL+blockx].pool;   // tentatively use head of free list as result - normal case, and even if blockx is out of bounds will not segfault
 if(2>*jt->adbreakr){  // this is JBREAK0, done this way so predicted fallthrough will be true
  I pushx=jt->tnextpushx;  // start reads for tpush
  A* tstack=jt->tstack;
  if(blockx<=PLIML){             /* large block: straight malloc    */
   mfreeb=jt->mfree[-PMINL+blockx].ballo; // bytes in pool allocations
   if(z){         // allocate from a chain of free blocks
    jt->mfree[-PMINL+blockx].pool = AFCHAIN(z);  // remove & use the head of the free chain
#if MEMAUDIT&1
    if(AFCHAIN(z)&&FHRHPOOLBIN(AFHRH(AFCHAIN(z)))!=(blockx-PMINL))SEGFAULT  // reference the next block to verify chain not damaged
    if(FHRHPOOLBIN(AFHRH(z))!=(blockx-PMINL))SEGFAULT  // verify block has correct size
    if(!(z->fill==(UI4)AFHRH(z)))SEGFAULT  // fill should duplicate h
#endif
   }else{A u,chn; US hrh;                    // small block, but chain is empty.  Alloc PSIZE and split it into blocks
#if 1 || ALIGNTOCACHE   // with smaller headers, always align pool allo to cache bdy
    // align the buffer list on a cache-line boundary
    I *v;
    ASSERT(v=MALLOC(PSIZE+CACHELINESIZE),EVWSFULL);
    z=(A)(((I)v+CACHELINESIZE)&-CACHELINESIZE);   // get cache-aligned section
    ((I**)z)[-1] = v;   // save address of entire allocation in the word before the aligned section
#else
    // allocate without alignment
    ASSERT(av=MALLOC(PSIZE),EVWSFULL);
#endif
    // split the allocation into blocks.  Chain them together, and flag the base.  We chain them in ascending order (the order doesn't matter), but
    // we visit them in back-to-front order so the first-allocated headers are in cache
#if MEMAUDIT&17 && BW==64
    u=(A)((C*)z+PSIZE); chn = 0; hrh = FHRHENDVALUE(blockx-PMINL); DQ(PSIZE>>blockx, u=(A)((C*)u-n); AFCHAIN(u)=chn; chn=u; hrh -= FHRHBININCR(blockx-PMINL); AFHRH(u)=hrh; u->fill=AFHRH(u););    // chain blocks to each other; set chain of last block to 0
    AFHRH(u) = hrh|FHRHROOT;  u->fill=AFHRH(u);  // flag first block as root.  It has 0 offset already
#else
    u=(A)((C*)z+PSIZE); chn = 0; hrh = FHRHENDVALUE(blockx-PMINL); DQ(PSIZE>>blockx, u=(A)((C*)u-n); AFCHAIN(u)=chn; chn=u; hrh -= FHRHBININCR(blockx-PMINL); AFHRH(u)=hrh;);    // chain blocks to each other; set chain of last block to 0
    AFHRH(u) = hrh|FHRHROOT;    // flag first block as root.  It has 0 offset already
#endif
    jt->mfree[-PMINL+blockx].pool=(A)((C*)u+n);  // the second block becomes the head of the free list
    mfreeb-=PSIZE;     // We are adding a bunch of free blocks now...
    jt->mfreegenallo+=PSIZE;   // ...add them to the total bytes allocated
   }
   jt->mfree[-PMINL+blockx].ballo=mfreeb+=n;
  } else {  // here for non-pool allocs...
   mfreeb=jt->mfreegenallo;    // bytes in large allocations
#if ALIGNTOCACHE
   // Allocate the block, and start it on a cache-line boundary
   I *v;
   ASSERT(v=MALLOC(n+CACHELINESIZE),EVWSFULL);
   z=(MS *)(((I)v+CACHELINESIZE)&-CACHELINESIZE);   // get cache-aligned section
   ((I**)z)[-1] = v;    // save address of original allocation
#else
   // Allocate without alignment
   ASSERT(z=MALLOC(n),EVWSFULL);
#endif
   AFHRH(z) = (US)FHRHSYSJHDR(blockx);    // Save the size of the allocation so we know how to free it and how big it was
#if MEMAUDIT&17
   z->fill=(UI4)AFHRH(z);
#endif
   jt->mfreegenallo=mfreeb+=n;    // mfreegenallo is the byte count allocated for large blocks
  }
#if MEMAUDIT&8
  DO((((I)1)<<(blockx-LGSZI)), lfsr = (lfsr<<1LL) ^ (lfsr<0?0x1b:0); if(i!=6)((I*)z)[i] = lfsr;);   // fill block with garbage - but not the allocation word
#endif
  AFLAG(z)=0; AC(z)=ACUC1|ACINPLACE;  // all blocks are born inplaceable 
   // we do not attempt to combine the AFLAG write into a 64-bit operation, because as of 2017 Intel processors
   // will properly store-forward any read that is to the same boundary as the write, and we always read the same way we write
  *(I*)((I)tstack+pushx)=(I)z; pushx+=SZI;
  if((pushx&(NTSTACK-1))){jt->tnextpushx=pushx;}else{RZ(tg(pushx)); jt->tnextpushx=pushx+SZI;}  // advance to next slot; skip over chain if new block needed
#if LEAKSNIFF
  if(leakcode>0){  // positive starts logging; set to negative at end to clear out the parser allocations etc
   if(leaknbufs*2 >= AN(leakblock)){
   }else{
    I* lv = IAV(leakblock);
    lv[2*leaknbufs] = (I)z; lv[2*leaknbufs+1] = leakcode;  // install address , code
    leaknbufs++;  // account for new value
   }
  }
#endif
  // If the user is keeping track of memory high-water mark with 7!:2, figure it out & keep track of it.  Otherwise save the cycles
  if(!(mfreeb&MFREEBCOUNTING))R z;  // this is a so-far-fruitless attempt to fall through to a return
  jt->bytes += n; if(jt->bytes>jt->bytesmax)jt->bytesmax=jt->bytes;
  R z;
 }else{jsignal(EVBREAK); R 0;}  // If there was a break event, take it
}

RESTRICTF A jtgafv(J jt, I bytes){UI4 j; 
 CTLZI((UI)(bytes-1),j); ++j;   // 3 or 4 should return 2; 5 should return 3
 if((UI)bytes<=(UI)jt->mmax){
  j=(j<PMINL)?PMINL:j;
  R jtgaf(jt,(I)j);
 }else{jsignal(EVLIMIT); R 0;}  // do it this way for branch-prediction
}

RESTRICTF A jtga(J jt,I type,I atoms,I rank,I* shaape){A z;
 // Get the number of bytes needed, including the header, the atoms, and a full I appended for types that require a
 // trailing NUL (because boolean-op code needs it)
 I bytes = ALLOBYTESVSZ(atoms,rank,bp(type),type&LAST0,0);  // We never use GA for NAME types, so we don't need to check for it
#if SY_64
 if((UI)atoms<TOOMANYATOMS && !(rank&~RMAX)){ // check for too many atoms, to preempt overflow
#else
 if(bytes>atoms&&atoms>=0){ // beware integer overflow
#endif
  RZ(z = jtgafv(jt, bytes));   // allocate the block, filling in AC and AFLAG
  I akx=AKXR(rank);   // Get offset to data
  AK(z)=akx; AT(z)=type; AN(z)=atoms;   // Fill in AK, AT, AN
  // Set rank, and shape if user gives it.  This might leave the shape unset, but that's OK
  AR(z)=(RANKT)rank;   // Storing the extra last I (as was done originally) might wipe out rank, so defer storing rank till here
  GACOPYSHAPEG(z,type,atoms,rank,shaape)  /* 1==atoms always if t&SPARSE  */  // copy shape by hand since short
  // because COPYSHAPE will always write one shape value, we have to delay the memset to handle the case of rank 0 with atoms (used internally only)
  if(!(type&DIRECT))memset((C*)z+akx,C0,bytes-akx);  // For indirect types, zero the data area.  Needed in case an indirect array has an error before it is valid
    // All non-DIRECT types have items that are multiples of I, so no need to round the length
  else if(type&LAST0){((I*)((C*)z+((bytes-SZI)&(-SZI))))[0]=(I)0x85d9a62c08a4f927 /* scaf */;}  // We allocated a full SZI for the trailing NUL, because the
     // code for boolean verbs needs it.
// obsolete if((1==rank&&type&SPARSE&&shaape) || (type&SPARSE && atoms && !(type&XZ)))
// obsolete  SEGFAULT  // scaf
// obsolete AT(z)&=~XZ; // scaf
  R z;
 }else{jsignal(EVLIMIT); R 0;}  // do it this way for branch-prediction
}

// free a block.  The usecount must make it freeable
void jtmf(J jt,A w){I mfreeb;
#if MEMAUDIT&16
auditmemchains();
#endif
#if MEMAUDIT&15
if((I)jt&3)SEGFAULT
#endif
#if LEAKSNIFF
 if(leakcode){I i;
  // Remove block from the table if the address matches
  I *lv=IAV(leakblock);
  for(i = 0;i<leaknbufs&&lv[2*i]!=(I)w;++i);  // find the match
  if(i<leaknbufs){while(i+1<leaknbufs){lv[2*i]=lv[2*i+2]; lv[2*i+1]=lv[2*i+3]; ++i;} leaknbufs=i;}  // remove it
 }
#endif

// audit free list {I Wi,Wj;MS *Wx; for(Wi=PMINL;Wi<=PLIML;++Wi){Wj=0; Wx=(jt->mfree[-PMINL+Wi].pool); while(Wx){Wx=(MS*)(Wx->a); ++Wj;}}}
 I hrh = AFHRH(w);   // the size/offset indicator
 I blockx=FHRHPOOLBIN(hrh);   // pool index, if pool
 I allocsize;  // size of full allocation for this block
 // SYMB must free as a monolith, with the symbols returned when the hashtables are
 if(AT(w)==SYMB){
  freesymb(jt,w);
 }
#if MEMAUDIT&1
 if(hrh==0 || blockx>(PLIML-PMINL+1))SEGFAULT  // pool number must be valid
#if MEMAUDIT&17
  if(!(w->fill==(UI4)hrh))SEGFAULT  // fill should duplicate h
#endif
#endif
 if(FHRHBINISPOOL(blockx)){   // allocated by malloc
  allocsize = FHRHPOOLBINSIZE(blockx);
#if MEMAUDIT&4
  DO((allocsize>>LGSZI), if(i!=6)((I*)w)[i] = (I)0xdeadbeefdeadbeefLL;);   // wipe the block clean before we free it - but not the reserved area
#endif
  mfreeb = jt->mfree[blockx].ballo;   // number of bytes allocated at this size (biased zero point)
  AFCHAIN(w)=jt->mfree[blockx].pool;  // append free list to the new addition...
  jt->mfree[blockx].pool=w;   //  ...and make new addition the new head
  if(0 > (mfreeb-=allocsize))jt->uflags.us.uq.uq_c.spfreeneeded=1;  // Indicate we have one more free buffer;
   // if this kicks the list into garbage-collection mode, indicate that
  jt->mfree[blockx].ballo=mfreeb;
 }else{                // buffer allocated from subpool.
  mfreeb = jt->mfreegenallo;
  allocsize = FHRHSYSSIZE(hrh);
#if MEMAUDIT&4
  DO((allocsize>>LGSZI), if(i!=6)((I*)w)[i] = (I)0xdeadbeefdeadbeefLL;);   // wipe the block clean before we free it - but not the reserved area
#endif
#if ALIGNTOCACHE
  FREECHK(((I**)w)[-1]);  // point to initial allocation and free it
#else
  FREECHK(w);  // point to initial allocation and free it
#endif
  jt->mfreegenallo = mfreeb-allocsize;
 }
 if(mfreeb&MFREEBCOUNTING){jt->bytes -= allocsize;}  // keep track of total allocation only if asked to
}

// allocate header with rank r; if r==1, move the item count to be the shape also
// a header is a simplified virtual block, for temporary use only, that must never escape into the wild, either in full or
// as a backer for a virtual block
RESTRICTF A jtgah(J jt,I r,A w){A z;
 ASSERT(RMAX>=r,EVLIMIT); 
 RZ(z=gafv(SZI*(NORMAH+r)));
 AT(z)=0;
 if(w){
  AT(z)=AT(w); AN(z)=AN(w); AR(z)=(RANKT)r; AK(z)=CAV(w)-(C*)z;
  if(1==r)*AS(z)=AN(w);
 }
 R z;
}    /* allocate header */ 

// clone w, returning the address of the cloned area.  If w is boxed relative it must be RELOCATED later
F1(jtca){A z;I t;P*wp,*zp;
 RZ(w);
 t=AT(w);
 if(t&SPARSE){
  GASPARSE(z,t,AN(w),AR(w),AS(w))
  wp=PAV(w); zp=PAV(z);
  SPB(zp,a,ca(SPA(wp,a)));
  SPB(zp,e,ca(SPA(wp,e)));
  SPB(zp,i,ca(SPA(wp,i)));
  SPB(zp,x,ca(SPA(wp,x)));
 }else{
  if(t&NAME){GATV(z,NAME,AN(w),AR(w),AS(w));AT(z)=t;}  // GA does not allow NAME type, for speed
  else GA(z,t,AN(w),AR(w),AS(w));
  MC(AV(z),AV(w),(AN(w)*bp(t))+(t&NAME?sizeof(NM):0));}
 R z;
}
// clone block only if it is read-only
F1(jtcaro){ if(AFLAG(w)&AFRO)RETF(ca(w)); RETF(w); }

// clone recursive.  The result is not relative, even if w is
F1(jtcar){A*u,*wv,z;I n;P*p;V*v;
 RZ(z=ca(w));
 n=AN(w);
 switch(CTTZ(AT(w))){
  case RATX:  n+=n;
  case XNUMX:
  case BOXX:  u=AAV(z); wv=AAV(w);  DO(n, RZ(*u++=car(wv[i]));); break;
  case SB01X: case SLITX: case SINTX: case SFLX: case SCMPXX: case SBOXX:
   p=PAV(z); 
   SPB(p,a,car(SPA(p,a)));
   SPB(p,e,car(SPA(p,e)));
   SPB(p,i,car(SPA(p,i)));
   SPB(p,x,car(SPA(p,x)));
   break;
  case VERBX: case ADVX: case CONJX: 
   v=FAV(z); 
   if(v->fgh[0])RZ(v->fgh[0]=car(v->fgh[0])); // no need to INCORP these, since no one will look and they aren't virtual
   if(v->fgh[1])RZ(v->fgh[1]=car(v->fgh[1])); 
   if(v->fgh[2])RZ(v->fgh[2]=car(v->fgh[2]));
 }
 R z;
}

// clone virtual block, producing a new virtual block
F1(jtclonevirtual){
 A z; RZ(z=virtual(w,0,AR(w)));  // allocate a new virtual block
 AN(z)=AN(w); MCISH(AS(z),AS(w),(I)AR(w));  // copy AN and shape; leave AC alone
 R z;
}

B jtspc(J jt){A z; RZ(z=MALLOC(1000)); FREECHK(z); R 1; }

// Double the allocation of w (twice as many atoms), then round up # items to max allowed in allocation
// if b=1, the result will replace w, so decrement usecount of w and increment usecount of new buffer
// the itemcount of the result is set as large as will fit evenly, and the atomcount is adjusted accordingly
A jtext(J jt,B b,A w){A z;I c,k,m,m1,t;
 RZ(w);                               /* assume AR(w)&&AN(w)    */
 m=*AS(w); PROD(c,AR(w)-1,AS(w)+1); t=AT(w); k=c*bp(t);
 GA(z,t,2*AN(w)+(AN(w)?0:c),AR(w),0);  // ensure we allocate SOMETHING to make progress
 m1=allosize(z)/k;  // start this divide before the copy
 MC(AV(z),AV(w),AN(w)*bp(t));                 /* copy old contents      */
 MCISH(&AS(z)[1],&AS(w)[1],AR(w)-1);
 if(b){RZ(ras(z)); fa(w);}                 /* 1=b iff w is permanent.  This frees up the old space */
 AS(z)[0]=m1; AN(z)=m1*c;       /* "optimal" use of space */
 if(!(t&DIRECT))memset(CAV(z)+m*k,C0,k*(m1-m));  // if non-DIRECT type, zero out new values to make them NULL
 R z;
}

A jtexta(J jt,I t,I r,I c,I m){A z;I m1; 
 GA(z,t,m*c,r,0); 
 I k=bp(t); *AS(z)=m1=allosize(z)/(c*k); AN(z)=m1*c;
 if(2==r)*(1+AS(z))=c;
 if(!(t&DIRECT))memset(AV(z),C0,k*AN(z));
 R z;
}    /* "optimal" allocation for type t rank r, c atoms per item, >=m items */


#if 0
/* debugging tools  */

B jtcheckmf(J jt){C c;I i,j;MS*x,*y;
 for(j=0;j<=PLIML;++j){
  i=0; y=0; x=(MS*)(jt->mfree[-PMINL+j]); /* head ptr for j-th pool */
  while(x){
   ++i; c=x->mflag;
   if(!(j==x->j)){
    ASSERTSYS(0,"checkmf 0");
   }
   if(!(!c||c==MFHEAD)){
    ASSERTSYS(0,"checkmf 1");
   }
   y=x; x=(MS*)x->a;
 }}
 R 1;
}    /* traverse free list */

B jtchecksi(J jt){DC d;I dt;
 d=jt->sitop;
 while(d&&!(DCCALL==d->dctype&&d->dcj)){
  dt=d->dctype;
  if(!(dt==DCPARSE||dt==DCSCRIPT||dt==DCCALL||dt==DCJUNK)){
   ASSERTSYS(0,"checksi 0");
  }
  if(!(d!=d->dclnk)){
   ASSERTSYS(0,"checksi 1");
  }
  d=d->dclnk;
 }
 R 1;
}    /* traverse stack per jt->sitop */
#endif
