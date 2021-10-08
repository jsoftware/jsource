/* Copyright 1990-2010, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Memory Management                                                       */

#ifdef _WIN32
#include <windows.h>
#else
#define __cdecl
#endif
#ifdef MMSC_VER
#pragma warning(disable: 4334)
#endif

#include "j.h"

#define LEAKSNIFF 0
#define SHOWALLALLOC 0 // to display log of allo/free
/*  to analyze
;@:(<@({.~  2 | #)/.)~ (#~ ('0000' -: 4&{.)@>) {.@;:;._2 wd 'clippaste'
*/

//NOTE: alignment to cache is now required because of LSB flags in enqueue()
#define ALIGNTOCACHE 1   // set to 1 to align each OS-allocated block block to cache-line boundary.  Will reduce cache usage for headers
#define ALIGNPOOLTOCACHE 1   // set to 1 to align each pool block to cache-line boundary.  Will reduce cache usage for headers
#define TAILPAD (32)  // we must ensure that a 32-byte masked op fetch to the last byte doesn't run off into unallocated memory

#define MEMJMASK 0xf   // these bits of j contain subpool #; higher bits used for computation for subpool entries
#define SBFREEBLG (14+PMINL)   // lg2(SBFREEB)
#define SBFREEB (1L<<SBFREEBLG)   // number of bytes that need to be freed before we rescan
#define MFREEBCOUNTING 1   // When this bit is set in mfreeb[], we keep track of max space usage

// Format of h, the 16-bit workarea for free and allocated blocks in main memory.  This is used for in-memory headers for NJS blocks, but not for NJA blocks all on disk
#define AFCHAIN(a) ((a)->kchain.chain)  // the chain field, when the block is not allocated
#define AFPROXYCHAIN(a) ((a)->tproxy.proxychain)  // chain field for base proxies during garbage collection
#define FHRHROOTX 15
#define FHRHROOT (((I)1)<<FHRHROOTX)  // set if the current block is the root (the first of the consecutive blocks making up its allocation)
#define FHRHROOTFREE ((I)2<<(PSIZEL-PMINL))   // If this bit is set at the end of garbage-collection, the whole allocation can be freed.  LSB (precisely, lowest 1-bit) is the size indicator
//
// the lower bits encode the size of the block, by the position of the lowest 1 bit, and in the upper bits either (1) the full size of the block for large allocations
// (2) the offset of the block from the root, for pool allocations.  The following macros define the field
#define FHRHPOOLBIN(h) CTTZ(h)     // pool bin# for free (0 means allo of size PMIN, etc).  If this gives PLIML-PMINL+1, the allocation is a system allo
#define FHRHBINISPOOL(h) ((h)&((2LL<<(PLIML-PMINL))-1))      // true is this is a pool allo, false if system (h is mask from block)
#define ALLOJISPOOL(j) ((j)<=PLIML)     // true if pool allo, false if system (j is lg2(requested size))
#define ALLOJBIN(j) ((j)-PMINL)   // convert j (=lg2(size)) to pool bin#
#define FHRHPOOLBINSIZE(h) (LOWESTBIT(h)<<PMINL)        // convert hmask to size for pool bin#
#define FHRHSYSSIZE(h) (((I)1)<<((h)>>(PLIML-PMINL+2)))        // convert h to size for system alloc
#define FHRHSIZE(h) ((FHRHBINISPOOL(h) ? FHRHPOOLBINSIZE(h) : FHRHSYSSIZE(h)))
#define FHRHSYSJHDR(j) ((2*(j)+1)<<(PLIML-PMINL+1))        // convert j (=lg(size)) to h format for a system allo
#define FHRHBININCR(b) ((I)2<<(b))      // when garbage-collecting bin b, add this much to the root for each free block encountered.  This is also the amount by which the h values of successive blocks in an allocation differ
#define FHRHBLOCKOFFSETMASK(b) (FHRHROOTFREE - FHRHBININCR(b))  // for blocks in pool b, mask to use to extract offset to root
#define FHRHBLOCKOFFSET(h,m) ((((h)>>(FHRHROOTX-0))-1) & (((h)&(m))<<(PMINL-1)))     // how far the pool block with h is offset from its root.  0 if FHRHROOT is set.  m is FHRHBLOCKOFFSETMASK
#define FHRHISROOTALLOFREE(h) ((h)&FHRHROOTFREE)   // given the root's h after garbage collection, is the entire allocation free?
#define FHRHROOTADDR(a,m) ((A)((C*)(a) - FHRHBLOCKOFFSET(AFHRH(a),(m))))   // address of root for block a.  m is FHRHBLOCKOFFSETMASK
#define FHRHISALLOFREE(a,m) FHRHISROOTALLOFREE(AFHRH(FHRHROOTADDR((a),(m))))      // is the given block a free after garbage collection? m is FHRHBLOCKOFFSETMASK
#define FHRHRESETROOT(b) (FHRHROOT + (((I)1)<<(b)))     // value to set root to after garbage-collection if the allocation was NOT freed
#define FHRHENDVALUE(b) (FHRHROOTFREE + (((I)1)<<(b)))     // value representing last+1 block in allo.  Subtract FHRHBININCR to get to previous

// the size of the total allocation of the block for w, always a power of 2
#define alloroundsize(w)  FHRHSIZE(AFHRH(w))

#if (MEMAUDIT==0 || !_WIN32)
#define FREECHK(x) FREE(x)
#else
#define FREECHK(x) if(!FREE(x))SEGFAULT;  // crash on error
#endif


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

#if MEMHISTO
I memhisto[64];  // histogram of requested memory blocks (9!:54, 9!:55)
static I histarea[16384][2] = {0};  // name, frequency of calls to jtgaf

void memhashadd(I lineno, C *string){
 C string8[8]="        ";  // padded string
 string+=strlen(string);  // go to end
 NOUNROLL while(string[-1]!='/' && string[-1]!='\\')--string;  // back up to filename
 I nwrit=snprintf(string8,8,"%lld",lineno);
 MC(string8+nwrit,string,MIN(8-nwrit,(I)strlen(string)));
 I stringi=*(I*)&string8;  // the string as int
 UI hash=16383&hic(sizeof(string8),string8);
 NOUNROLL while(1){if(histarea[hash][0]==stringi)break; if(histarea[hash][0]==0){histarea[hash][0]=stringi; break;} if(--hash<0)hash=16383;}  // find hash slot
 ++histarea[hash][1];  // increment count
}

// 9!:54/55  read/set memory histogram
F1(jtmemhistoq){
 ASSERTMTV(w);
 R vec(INT,sizeof(jt->memhisto)/sizeof(jt->memhisto)[0],jt->memhisto);
}

F1(jtmemhistos){I k;
 ASSERTMTV(w); 
 mvc(sizeof(jt->memhisto),jt->memhisto,1,MEMSET00);
 R mtm;
}


// return histo area

// process using   ;"1 (":@{. ; ' ' ; 3 (3!:4) {:)"1 (20) {. \:~ |."1 (_2) ]\ 9!:62''
F1(jtmemhashq){
 R vec(INT,sizeof(histarea)/sizeof(histarea)[0][0],histarea);
}
F1(jtmemhashs){
 ASSERTMTV(w); 
 mvc(sizeof(histarea,histarea,1,MEMSET00));
 R mtm;
}

#endif

B jtmeminit(JS jjt,I nthreads){I k,m=MLEN;
 INITJT(jjt,adbreakr)=INITJT(jjt,adbreak)=(C*)&INITJT(jjt,breakbytes); /* required for ma to work */
 INITJT(jjt,mmax) =(I)1<<(m-1);
 I threadno; for(threadno=0;threadno<nthreads;++threadno){JJ jt=&jjt->threaddata[threadno];
  // init tpop stack
  jt->tstackcurr=(A*)MALLOC(NTSTACK+NTSTACKBLOCK);  // save address of first allocation
  jt->malloctotal = NTSTACK+NTSTACKBLOCK;
  jt->tnextpushp = (A*)(((I)jt->tstackcurr+NTSTACKBLOCK)&(-NTSTACKBLOCK));  // get address of aligned block AFTER the first word
  *jt->tnextpushp++=0;  // blocks chain to blocks, allocations to allocations.  0 in first block indicates end.  We will never try to go past the first allo, so no chain needed
  // init all subpools to empty, setting the garbage-collection trigger points
  for(k=PMINL;k<=PLIML;++k){jt->mfree[-PMINL+k].ballo=SBFREEB;jt->mfree[-PMINL+k].pool=0;}  // init so we garbage-collect after SBFREEB frees
  jt->mfreegenallo=-SBFREEB*(PLIML+1-PMINL);   // balance that with negative general allocation
 }
#if LEAKSNIFF
 leakblock = 0;
 leaknbufs = 0;
 leakcode = 0;
#endif
 R 1;
}

// Audit all memory chains to detect overrun
#if SY_64
#define AUDITFILL ||((MEMAUDIT&0x4)?AC(Wx)!=(I)0xdeadbeefdeadbeefLL:0)
#else
#define AUDITFILL ||((MEMAUDIT&0x4)?AC(Wx)!=(I)0xdeadbeefL:0)
#endif
void jtauditmemchains(J jt){F1PREFIP;
#if MEMAUDIT&0x30
I Wi,Wj;A Wx,prevWx=0; if((MEMAUDITPCALLENABLE)&&((MEMAUDIT&0x20)||JT(jt,peekdata))){
 for(Wi=PMINL;Wi<=PLIML;++Wi){Wj=0; Wx=(jt->mfree[-PMINL+Wi].pool);
 NOUNROLL while(Wx){if(FHRHPOOLBIN(AFHRH(Wx))!=(Wi-PMINL)AUDITFILL||Wj>0x10000000)SEGFAULT; prevWx=Wx; Wx=AFCHAIN(Wx); ++Wj;}}
}
#endif
}
// 13!:23  check the memory free list, a la auditmemchains()
// return error info, a 2-atom list where
//  atom 0 is return code 0=OK 1=pool number corrupted 2=header corrupted 3=usecount corrupted (valid only if MEMAUDIT&0x4) 4=loop in chain 
//  atom 1 is lg of failing blocksize
// if arg is not empty, crash on any error
F1(jtcheckfreepool){
 I Wi,Wj,ecode=0;A Wx,prevWx=0; 
 for(Wi=PMINL;Wi<=PLIML;++Wi){  // for each free list
  Wj=0; Wx=(jt->mfree[-PMINL+Wi].pool);  // get head of chain, init count of # eles
  NOUNROLL while(Wx){
   if(FHRHPOOLBIN(AFHRH(Wx))!=(Wi-PMINL)){ecode=1; break;}  // will crash here if chain is corrupted
#if MEMAUDIT&4
   if(AC(Wx)!=(I)0xdeadbeefdeadbeefLL){ecode=3; break;}
#endif
   if(Wj>0x10000000){ecode=4; break;}
   prevWx=Wx; Wx=AFCHAIN(Wx); ++Wj;  // prevwx saves previous value in case of wild pointer
  }
 }
 if(ecode&&AN(w))SEGFAULT;  // if arg not empty, crash on all errors
 R v2(ecode,ecode?Wi:0);  // return error code and chain 
}

F1(jtspcount){A z;I c=0,i,j,*v;A x;
 ASSERTMTV(w);
 GATV0(z,INT,2*(-PMINL+PLIML+1),2); v=AV(z);
 for(i=PMINL;i<=PLIML;++i){j=0; x=(jt->mfree[-PMINL+i].pool); NOUNROLL while(x){x=AFCHAIN(x); ++j;} if(j){++c; *v++=(I)1<<i; *v++=j;}}
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
    if(FHRHPOOLBIN(AFHRH(p))!=i)SEGFAULT;  // make sure chains are valid
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
     jt->malloctotal-=PSIZE+TAILPAD+CACHELINESIZE;  // return storage+bdy
     jt->mfreegenallo-=TAILPAD+CACHELINESIZE;  // remove pad from the amount we report allocated
#else
     FREECHK(baseblock);
     jt->malloctotal-=PSIZE+TAILPAD;  // return storage
     jt->mfreegenallo-=TAILPAD;  // remove pad from the amount we report allocated
#endif
    }else{AFHRH(baseblock) = virginbase;}   // restore the count to 0 in the rest
    p=np;   //  step to next base block
   } 

   // set up for next spfree: set mfreeb to a value such that when SPFREEB bytes have been freed,
   // mfreeb will hit 0, causing a rescan.
   // Account for the buffers that were freed during the coalescing by reducing the number of PSIZEL bytes allocated
   // coalescing doesn't change the allocation, but it does change the accounting.  The change to mfree[] must be
   // compensated for by a change to mfreegenallo.  mfreegenallo must also account for the excess padding that is now being returned
   // This elides the step of subtracting coalesced buffers from the number of allocated buffers of size i, followed by
   // adding the bytes for those blocks to mfreebgenallo
   jt->mfreegenallo -= SBFREEB - (jt->mfree[i].ballo & ~MFREEBCOUNTING);  // subtract diff between current mfreeb[] and what it will be set to
   jt->mfree[i].ballo = SBFREEB + (jt->mfree[i].ballo & MFREEBCOUNTING);  // set so we trigger rescan when we have allocated another SBFREEB bytes
  }
 }
 jt->uflags.us.uq.uq_c.spfreeneeded = 0;  // indicate no check needed yet
 R 1;
}    /* free unused blocks */

// return space used by w and its descendants
static D jtspfor1(J jt, A w){D tot=0.0;
 if(unlikely(w==0))R 0.0;
 switch(CTTZ(AT(w))){
  case XNUMX: case BOXX:
   if(!ISSPARSE(AT(w))){
    if(!(AFLAG(w)&AFNJA)){A*wv=AAV(w);
     {DO(AN(w), if(wv[i])tot+=spfor1(QCWORD(wv[i])););}
    }
    break;
   }
  case B01X: case INTX: case FLX: case CMPXX: case LITX:
   if(ISSPARSE(AT(w))){P*v=PAV(w); if(SPA(v,a))tot+=spfor1(SPA(v,a)); if(SPA(v,e))tot+=spfor1(SPA(v,e)); if(SPA(v,i))tot+=spfor1(SPA(v,i)); if(SPA(v,x))tot+=spfor1(SPA(v,x));} break;
  case VERBX: case ADVX:  case CONJX: 
   {V*v=FAV(w); if(v->fgh[0])tot+=spfor1(v->fgh[0]); if(v->fgh[1])tot+=spfor1(v->fgh[1]); if(v->fgh[2])tot+=spfor1(v->fgh[2]);} break;
  case XDX:
   {DX*v=(DX*)AV(w); DQ(AN(w), if(v->x)tot+=spfor1(v->x); ++v;);} break;
  case RATX:  
   {A*v=AAV(w); DQ(2*AN(w), if(*v)tot+=spfor1(*v++););} break;
 }
 if(!ACISPERM(AC(w))) {
  // for NJA allocations with contiguous header, the size is the header size (7+64 words) plus the data size
  // for NJA allocations with separate header, the size is the data size plus the size of the base block
  if(AFNJA&AFLAG(w)) {
   if(AK(w)>0&&AK(w)<=AM(w))tot += SZI*WP(AT(w),AN(w),64);  // fixed rank of 64 in NJA memory
   else{
    tot += SZI*((1&&AT(w)&LAST0)+(((AT(w)&NAME?sizeof(NM):0)+(AN(w)<<bplg(AT(w)))+SZI-1)>>LGSZI));  // data size only
    tot += alloroundsize(w);  // add in the header
   }
  } else {
  // for non-NJA allocations, just take the full size of the block
   tot += alloroundsize(w);
  }
 }
 R tot;
}

F1(jtspfor){A*wv,x,y,z;C*s;D*zv;I i,m,n;
 ARGCHK1(w);
 n=AN(w); wv=AAV(w);
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 GATV(z,FL,n,AR(w),AS(w)); zv=DAV(z); 
 for(i=0;i<n;++i){
  x=wv[i]; m=AN(x); s=CAV(x);
  ASSERT(LIT&AT(x),EVDOMAIN);
  ASSERT(1>=AR(x),EVRANK);
  ASSERT(vnm(m,s),EVILNAME);
  RZ(y=symbrd(nfs(m,s))); 
  zv[i]=spfor1(y);
 }
 RETF(z);
}    /* 7!:5 space for named object; w is <'name' */

F1(jtspforloc){A*wv,x,y,z;C*s;D tot,*zv;I i,j,m,n;L*u;LX *yv,c;
 ARGCHK1(w);
 n=AN(w); wv=AAV(w);
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 GATV(z,FL,n,AR(w),AS(w)); zv=DAV(z);   // zv-> results
 for(i=0;i<n;++i){   // loop over each name given...
  x=wv[i];  // x is the name/number
  I bucketx;  // will be hash/number for the locale
  if(!AR(x)&&AT(x)&INT){
   m=-1; bucketx=IAV(x)[0];   // signal numeric-atom locale; fetch number
  }else{
   m=AN(x); s=CAV(x);
   if(!m){m=sizeof(JT(jt,baselocale)); s=JT(jt,baselocale);}
   ASSERT(LIT&AT(x),EVDOMAIN);
   ASSERT(1>=AR(x),EVRANK);
   ASSERT(vlocnm(m,s),EVILNAME);
   bucketx=BUCKETXLOC(m,s);
  }
  y=stfind(m,s,bucketx);   // y is the block for the locale
  ASSERT(y!=0&&LOCPATH(y),EVLOCALE);
  tot=(D)(FHRHSIZE(AFHRH(y)));  // start with the size of the locale block (always a normal block)
//  tot+=spfor1(LOCPATH(y));  // ignore the size of the path, since it's just other locales
  tot+=spfor1(LOCNAME(y));  // add in the size of the path and name
  m=AN(y); yv=LXAV0(y); 
  for(j=SYMLINFOSIZE;j<m;++j){  // for each name in the locale
   for(c=yv[j];c=SYMNEXT(c),c;c=u->next){tot+=sizeof(L); u=c+JT(jt,sympv); tot+=spfor1(u->name); tot+=spfor1(u->val);}  // add in the size of the name itself and the value, and the L block for the name
  }
  zv[i]=tot;
 }
 RETF(z);
}    /* 7!:6 space for a locale */


F1(jtmmaxq){ASSERTMTV(w); RETF(sc(JT(jt,mmax)));}
     /* 9!:20 space limit query */

F1(jtmmaxs){I j,m=MLEN,n;
 RE(n=i0(vib(w)));
 ASSERT(1E5<=n,EVLIMIT);
 j=m-1; DO(m, if(n<=(I)1<<i){j=i; break;});
 JT(jt,mmax)=(I)1<<j;
 RETF(mtm);
}    /* 9!:21 space limit set */


// Get total # bytes in use.  That's total allocated so far, minus the bytes in the free lists.
// mfreeb[] is a negative count of blocks in the free list, and biased so the value goes negative
// when garbage-collection is required.  All non-pool allocations are accounted for in
// mfreegenallo
// At init, each mfreeb indicates SBFREEB bytes. mfreegenallo is negative to match that total,
// indicating nothing has really been allocated; that's (PLIML-PMINL+1)*SBFREEB to begin with.  When a block
// is allocated, mfreeb[] increases; when a big block is allocated, mfreegenallo increases by the
// amount of the allocation, and mfree[-PMINL+n] decreases by the amount in all the blocks that are now
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
 if(AFLAG(w)>>AFAUDITUCX)SEGFAULT;   // hang if nonzero count
 if(AC(w)==0 || (AC(w)<0 && AC(w)!=ACINPLACE+ACUC1 && AC(w)!=ACINPLACE+2))SEGFAULT; 
 if(AFLAG(w)&AFVIRTUAL)auditsimverify0(ABACK(w));  // check backer
 if(AT(w)&(RAT|XNUM)) {A* v=AAV(w);  DQ(AT(w)&RAT?2*AN(w):AN(w), if(*v)auditsimverify0(*v); ++v;)}
 if(!(AFLAG(w)&AFVIRTUAL)&&UCISRECUR(w)){  // process children
  if((AT(w)&BOX+SPARSE)>0){
   I n=AN(w); I af=AFLAG(w);
   A* RESTRICT wv=AAV(w);  // pointer to box pointers
   I wrel = af&AFNJA?(I)w:0;  // If NJA, add wv[] to wd; otherwise wv[] is a direct pointer
   if((af&AFNJA)||n==0)R;  // no processing if not J-managed memory (rare)
   DO(n, auditsimverify0((A)(intptr_t)((I)wv[i]+(I)wrel)););
  }else if(AT(w)&FUNC) {V* RESTRICT v=VAV(w);
   auditsimverify0(v->fgh[0]); auditsimverify0(v->fgh[1]); auditsimverify0(v->fgh[2]);
  }else if(AT(w)&RAT|XNUM) {
  }else SEGFAULT;  // inadmissible type for recursive usecount
 }
 R;
}

// Simulate tpop on the input block.  If that produces a delete count that equals the usecount,
// recur on children if any.  If it produces a delete count higher than the use count in the block, abort
static void auditsimdelete(A w){I delct;
 if(!w)R;
 if((UI)AN(w)==0xdeadbeefdeadbeef||(UI)AN(w)==0xfeeefeeefeeefeee)SEGFAULT;
 if((delct = ((AFLAG(w)+=AFAUDITUC)>>AFAUDITUCX))>ACUC(w))SEGFAULT;   // hang if too many deletes
 if(AFLAG(w)&AFVIRTUAL && (AT(w)^AFLAG(w))&RECURSIBLE)SEGFAULT;   // hang if nonrecursive virtual
 if(delct==ACUC(w)&&AFLAG(w)&AFVIRTUAL){A wb = ABACK(w);
  // we fa() the backer, while we mf() the block itself.  So if the backer is NOT recursive, we have to
  // handle nonrecursive children.  All recursible types will be recursive
  if(AFLAG(w)&AFVIRTUAL && (AT(wb)^AFLAG(wb))&RECURSIBLE)SEGFAULT;  // backer must be recursive
  auditsimdelete(wb);  // delete backer of virtual block, recursibly
 }
 if(delct==ACUC(w)&&!(AFLAG(w)&AFVIRTUAL)&&(UCISRECUR(w))){  // we deleted down to 0.  process children
  if((AT(w)&BOX+SPARSE)>0){
   I n=AN(w); I af=AFLAG(w);
   A* RESTRICT wv=AAV(w);  // pointer to box pointers
   I wrel = af&AFNJA?(I)w:0;  // If NJA, add wv[] to wd; othewrwise wv[] is a direct pointer
   if((af&AFNJA)||n==0)R;  // no processing if not J-managed memory (rare)
   DO(n, auditsimdelete((A)(intptr_t)((I)wv[i]+(I)wrel)););
  }else if(AT(w)&FUNC) {V* RESTRICT v=VAV(w);
   auditsimdelete(v->fgh[0]); auditsimdelete(v->fgh[1]); auditsimdelete(v->fgh[2]);
  }else if(AT(w)&RAT|XNUM) {A* v=AAV(w);  DQ(AT(w)&RAT?2*AN(w):AN(w), if(*v)auditsimdelete(*v); ++v;)
  }else SEGFAULT;  // inadmissible type for recursive usecount
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
  if(AT(wb)&(RAT|XNUM)) {A* v=AAV(wb);  DQ(AT(wb)&RAT?2*AN(wb):AN(wb), if(*v)auditsimreset(*v); ++v;)}  // reset children
 }
 if(delct==ACUC(w)&&!(AFLAG(w)&AFVIRTUAL)&&(UCISRECUR(w))){  // if so, recursive reset
  if((AT(w)&BOX+SPARSE)>0){
   I n=AN(w); I af=AFLAG(w);
   A* RESTRICT wv=AAV(w);  // pointer to box pointers
   I wrel = af&AFNJA?(I)w:0;  // If NJA, add wv[] to wd; othewrwise wv[] is a direct pointer
   if((af&AFNJA)||n==0)R;  // no processing if not J-managed memory (rare)
   DO(n, auditsimreset((A)(intptr_t)((I)wv[i]+(I)wrel)););
  }else if(AT(w)&FUNC) {V* RESTRICT v=VAV(w);
   auditsimreset(v->fgh[0]); auditsimreset(v->fgh[1]); auditsimreset(v->fgh[2]);
  }else if(AT(w)&RAT|XNUM) {A* v=AAV(w);  DQ(AT(w)&RAT?2*AN(w):AN(w), if(*v)auditsimreset(*v); ++v;)
  }else SEGFAULT;  // inadmissible type for recursive usecount
 }
 R;
}

#endif

// Register the value to insert into leak-sniff records
void jtsetleakcode(J jt, I code) {
#if LEAKSNIFF
 if(!leakblock)GAT0(leakblock,INT,10000,1); ACINITZAP(leakblock);
 leakcode = code;
#endif
}

F1(jtleakblockread){
#if LEAKSNIFF
if(!leakblock)R num(0);
R vec(INT,2*leaknbufs,IAV(leakblock));
#else
R num(0);
#endif
}
F1(jtleakblockreset){
#if LEAKSNIFF
leakcode = 0;
leaknbufs = 0;
R num(0);
#else
R num(0);
#endif
}

// Verify that block w does not appear on tstack more than lim times
// nextpushp might start out on a boundary
void audittstack(J jt){F1PREFIP;
#if BW==64 && MEMAUDIT&2
 if(JT(jt,audittstackdisabled)&1)R;
 A *ttop;
 A *nvrav=AAV1(jt->nvra);
 // verify counts start clear
 for(ttop=jt->tnextpushp-!!((I)jt->tnextpushp&(NTSTACKBLOCK-1));ttop;){
  // loop through each entry, skipping the first which is a chain
  for(;(I)ttop&(NTSTACKBLOCK-1);ttop--){
   if(*ttop)auditsimverify0(*ttop);
  }
  // back up to previous block
  ttop = (A*)*ttop;  // back up to end of previous block, or 0 if last block
 }
 // Process the NVR stack as well
 DO(jt->parserstackframe.nvrtop, auditsimverify0(nvrav[i]);)
 // loop through each block of stack
 for(ttop=jt->tnextpushp-!!((I)jt->tnextpushp&(NTSTACKBLOCK-1));ttop;){
  for(;(I)ttop&(NTSTACKBLOCK-1);ttop--){
   if(*ttop)auditsimdelete(*ttop);
  }
  ttop = (A*)*ttop;  // back up to end of previous block, or 0 if last block
 }
 // simulate nvr-stack frees
 DO(jt->parserstackframe.nvrtop, if((AM(nvrav[i])-=AMNVRCT)==AMFREED){auditsimdelete(nvrav[i]);})
 DO(jt->parserstackframe.nvrtop, AM(nvrav[i])+=AMNVRCT)  // restore AMs
 // again to clear the counts
 for(ttop=jt->tnextpushp-!!((I)jt->tnextpushp&(NTSTACKBLOCK-1));ttop;){
  for(;(I)ttop&(NTSTACKBLOCK-1);ttop--){
   if(*ttop)auditsimreset(*ttop);
  }
  ttop = (A*)*ttop;  // back up to end of previous block, or 0 if last block
 }
 DO(jt->parserstackframe.nvrtop, auditsimreset(nvrav[i]);)
#endif
}

// Free all symbols pointed to by the SYMB block w, including PERMANENT ones.  But don't return CACHED values to the symbol pool
void freesymb(J jt, A w){I j,wn=AN(w); LX k,* RESTRICT wv=LXAV0(w);
 LX freeroot=0; LX *freetailchn=(LX *)jt->shapesink;  // sym index of first freed ele; addr of chain field in last freed ele
 L *jtsympv=JT(jt,sympv);  // Move base of symbol block to a register.  Block 0 is the base of the free chain.  MUST NOT move the base of the free queue to a register,
  // because when we free a locale it frees its symbols here, and one of them might be a verb that contains a nested SYMB, giving recursion.  It is safe to move sympv to a register because
  // we know there will be no allocations during the free process.
 // loop through each hash chain, clearing the blocks in the chain
 for(j=SYMLINFOSIZE;j<wn;++j){
  // free the chain; kt->last block freed
  if(k=wv[j]){LX *asymx=&wv[j];  // pointer to previous chain
   do{
    k=SYMNEXT(k);
    LX nextk=jtsympv[k].next;  // unroll loop 1 time
    fa(jtsympv[k].name);jtsympv[k].name=0;  // always release name
    if(likely(!(jtsympv[k].flag&LCACHED))){
     SYMVALFA(jtsympv[k]);    // free value
     jtsympv[k].val=0;jtsympv[k].valtype=0;jtsympv[k].sn=0;jtsympv[k].flag=0;
     asymx=&jtsympv[k].next;  // make the current next field the previous for the next iteration
    }else{*asymx=SYMNEXT(jtsympv[k].next);}  // for cached value, remove from list to be freed.  It becomes unmoored.
    k=nextk;  // advance to next block in chain
   }while(k);
   // if the chain is not (now) empty, make it the base of the free pool & chain previous pool from it.  CACHED items have been removed
   if(likely(wv[j]!=0)){freeroot=freeroot?freeroot:wv[j]; *freetailchn=wv[j]; freetailchn=asymx;}  // free chain may have permanent flags   save addr of the very first freed item
  }
 }
 if(likely(freeroot!=0)){*freetailchn=jtsympv[0].next;jtsympv[0].next=freeroot;}  // put all blocks freed here onto the free chain
}

// free the symbol table (i. e. locale) w.  AR(w) has been loaded.  We return w so caller doesn't have to save it
A jtfreesymtab(J jt,A w,I arw){  // don't make this static - it will be inlined and that will make jtmf() save several more registers
 if(likely(arw&ARLOCALTABLE)){
  // local tables have no path or name, and are not listed in any index.  Just delete the local names
  freesymb(jt,w);   // delete all the names/values
 }else{
  // freeing a named/numbered locale.  The locale must have had all names freed earlier, and the path must be 0.
  // First, free the path and name (in the SYMLINFO block), and then free the SYMLINFO block itself
  LX k,* RESTRICT wv=LXAV0(w);
  L *jtsympv=JT(jt,sympv);
  if(likely((k=wv[SYMLINFO])!=0)){  // if no error in allocation...
   // Remove the locale from its global table, depending on whether it is named or numbered
   NM *locname=NAV(LOCNAME(w));  // NM block for name
   if(likely(!(arw&ARNAMED))){
    // For numbered locale, find the locale in the list of numbered locales, wipe it out, free the locale, and decrease the number of those locales
    jterasenl(jt,locname->bucketx);  // remove the locale from the hash table
   } else {
    // For named locale, find the entry for this locale in the locales symbol table, and free the locale and the entry for it
    ACINIT(w,2) jtprobedel((J)((I)jt+locname->m),locname->s,locname->hash,JT(jt,stloc));  // free the L block for the locale.  Protect the locale itself so it is not freed, as we are just about to do that
   }
   // Free the name
   fr(LOCNAME(w));
   // clear the data fields in symbol 0   kludge but this is how it was done (should be done in symnew)
   jtsympv[k].name=0;jtsympv[k].val=0;jtsympv[k].valtype=0;jtsympv[k].sn=0;jtsympv[k].flag=0;
   jtsympv[k].next=jtsympv[0].next;jtsympv[0].next=k;  // put symbol on the free list.  JT(jt,sympv)[0] is the base of the free chain
  }
 }
 // continue to free the table itself
 R w;
}

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
A jtincorp(J jt, A w) {ARGCHK1(w); INCORP(w); R w;}

// allocate a virtual block, given the backing block
// offset is offset in atoms from start of w; r is rank
// result block is never inplaceable, never recursible, virtual.  Can return 0 if allocation error
// result is PRISTINE iff the backer is
// This is inplaceable, and we inplace the w block.  'Inplaceable' here includes being the target of jt->asginfo.assignsym
// We fill in everything but AN and AS, which are done in the caller
// You should be wary of making an NJA block virtual, because with a usecount of 1 it might be inplaced by the code for x,y or x u}y
// If this code is called with inplacing turned on (* w inplaceable), we assume that w is going to be replaced by the virtual result,
// and we make the virtual block inplaceable if w was
RESTRICTF A jtvirtual(J jtip, AD *RESTRICT w, I offset, I r){AD* RESTRICT z;
 J jt=(J)(intptr_t)((I)jtip&~JTFLAGMSK);  // get flag-free pointer to J block
 ASSERT(RMAX>=r,EVLIMIT);
 I t=AT(w);  // type of input
 offset<<=bplg(t);  // length of an atom of t
 I wf=AFLAG(w);  // flags in input
 I wip=SGNIF((I)jtip,JTINPLACEWX)&AC(w);   // sgn if w is abandoned
 // If this is an inplaceable request for an inplaceable DIRECT block, we don't need to create a new virtual block: just modify the offset in the old block.  Make sure the shape fits
 // if the block is UNINCORPABLE, we don't modify it, because then we would have to check everywhere to see if a parameter block had changed
 // We could check for assignsym etc, but it's not worth it: all we are saving is allocating one lousy block, usually 64 bytes
 if((wip & (-(t&DIRECT)) & (r-(AR(w)+1)) & SGNIFNOT(wf,AFUNINCORPABLEX))<0){
  // virtual-in-place.  There's nothing to do but change the pointer and fill in the new rank.  AN and AS are handled in the caller
  // We leave the usecount unchanged, so the block still shows as inplaceable
  AK(w)+=offset; AR(w)=(RANKT)r;
  R w;
 }else{
  // not self-virtual block: allocate a new one
  RZ(z=gafv(SZI*(NORMAH+r)-1));  // allocate the block
  AK(z)=(CAV(w)-(C*)z)+offset;
  AFLAGINIT(z,AFVIRTUAL | (wf & ((UI)wip>>(BW-1-AFPRISTINEX))) | (t&TRAVERSIBLE))  // flags: recursive, not UNINCORPABLE, not NJA.  If w is inplaceable, inherit its PRISTINE status
  A wback=ABACK(w); A *wzaploc=(A*)wback; wback=wf&AFVIRTUAL?wback:w; ABACK(z)=wback;  // wtpop is AZAPLOC(w) in case it is to be zapped
  AT(z)=t;
  ACINIT(z,wip+ACUC1)   // transfer inplaceability from original block
  ARINIT(z,(RANKT)r);
  if((wip&((wf&(AFVIRTUAL|AFUNINCORPABLE))-1))<0){
    // w (the old block) is abandoned and is not UNINCORPABLE.  It must still have an entry on the tpop stack.  Rather than incrementing its
    // usecount, we can simply remove its tpop entry.  We must also mark the block as uninplaceable, since it is a backer now (might not be necessary,
    // because to get here we must know that w has been abandoned)
    // We must ensure that the backer has recursive usecount, as a way of protecting the CONTENTS.  We zap the tpop for the backer itself, but
    // not for the contents.
    ACRESET(w,ACUC1) *wzaploc=0;  // zap the tpop for w in lieu of ra() for it
    if((t^wf)&RECURSIBLE){AFLAGRESET(w,wf|=(t&RECURSIBLE)) jtra(w,t);}  // make w recursive, raising contents if was nonrecurive.  Like ra0()
// when virtuals can be zapped, use that here
  }else{
   // if we can't transfer ownership, must ra the backer.  UNINCORPORABLEs go through here, and must be virtual so the backer, not the indirect block, is raised
   // We must also remove inplaceability from w, since it too has an alias at large
   ACIPNO(w); ra(wback);
  }

  // As a result of the above we can say that all backers must have recursive usecount
  R z;
 }
}  

#if 0 // not used
// convert a block, recently allocated in the caller, to virtual.  The caller got the shape right; we just have to fill in all the other fields, including the data pointer.
// User still has to fill in AN and AS
void jtexpostvirtual(J jt,A z,A w,I offset){
ACINIT(z,ACUC1) AK(z)=(CAV(w)-(C*)z)+offset; // virtual, not inplaceable
if(AFLAG(w)&AFVIRTUAL){
 // If w is virtual, me must disallow inplacing for it, since it may be at large in the execution and we are creating an alias to it
 ACIPNO(w);  // turn off inplacing
 w=ABACK(w);  // if w is itself virtual, use its original backer.  Otherwise we would have trouble knowing when the backer for z is freed.  Backer is never virtual
}
AFLAGINIT(z,AFVIRTUAL|(AFLAG(w)&AFPRISTINE)|(AT(z)&TRAVERSIBLE))  // flags: recursive, not UNINCORPABLE, not NJA, with PRISTINE inherited from backer
ABACK(z)=w;   // set the pointer to the base: w or its base
ra(w);   // ensure that the backer is not deleted while it is a backer.
}
#endif

// realize a virtual block (error if not virtual)
// allocate a new block, copy the data to it.  result is address of new block; can be 0 if allocation failure
// only non-sparse nouns can be virtual
// Mark the backing block non-PRISTINE, because realize is a form of escaping from the backer
A jtrealize(J jt, A w){A z; I t;
// allocate a block of the correct type and size.  Copy the shape
 ARGCHK1(w);
 t=AT(w);
 AFLAGPRISTNO(ABACK(w))  // clear PRISTINE in the backer, since its contents are escaping
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


A jtgc(J jt,A w,A* old){
 ARGCHK1(w);  // return if no input (could be error or unfilled box)
 I c=AC(w);  // remember original usecount/inplaceability
 // We want to avoid realizing w if possible, so we handle virtual w separately
 if(AFLAG(w)&(AFVIRTUAL|AFVIRTUALBOXED)){
  if(AFLAG(w)&AFVIRTUALBOXED)R w;  // We don't disturb VIRTUALBOXED arrays because we know they're going to be opened presently.  The backer(s) might be on the stack.
  // It might be right to just return fast for any virtual block
  if(likely(!(AFLAG(w)&AFUNINCORPABLE))){
   A b=ABACK(w);  // backing block for w.  It is known to be direct or recursible, and had its usecount incremented by w
   // Raise the count of w to protect it.  Since w raised the count of b when w was created, this protects b also.  Afterwards, if
   // b need not be deleted, w can survive as is; but if b is to be deleted, we must realize w.  We don't keep b around because it may be huge
   // (could look at relative size to make this decision).  It is possible that the usecount of w is > 1 ONLY if the usecount was raised when
   // w was assigned to x or y.
   // Detecting when the backer is going away is subtle because it may have been zapped in a transfer of ownership and have NO entry to the stack.
   // In that case, it is totally unmoored and will persist for as long as w does.  We assume that the decision to transfer ownership was made
   // advisedly and we do not delete the backer, but leave w alone.  Thus the test for realizing is (backer count changed during tpop) AND
   // (backer count is now <2)
   I bc=AC(b); bc=bc>2?2:bc;  // backer count before tpop.  We will delete backer if value goes down, to a value less than 2.  
   ACSET(w,2)  // protect w from being freed.  Safe to use 2, since any higher value implies the backer is protected
   tpop(old);  // delete everything allocated on the stack, except for w and b which were protected
   // if the block backing w has no reason to persist except as the backer for w, we delete it to avoid wasting space.  We must realize w to protect it; and we must also ra() the contents of w to protect them.
   // If there are multiple virtual blocks relying on the backer, we can't realize them all so we have to keep the backer around.
   if(unlikely(AC(b)<bc)){A origw = w; RZ(w=realize(w)); radescend(w,); fa(b); mf(origw); }  // if b exists only for w, delete b.  get w out of the way.  w cannot be sparse.  Since we
                                      // raised the usecount of w only, we use mf rather than fa to free just the virtual block
                                      // fa the backer to undo the ra when the virtual block was created
   else{
    // if the backing block survives, w can continue as virtual; we must undo the increment above.  If the usecount was changed by the tpop, we must replace
    // the stack entry.  Otherwise we can keep the stack entry we have, wherever it is, but we must restore the usecount to its original value, which might
    // include inplaceability
    if(AC(w)<2)tpush1(w);  // if the stack entry for w was removed, restore it
    ACSET(w,c)  // restore initial usecount and inplaceability
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
 ra(w);  // protect w and its descendants from tpop; also converts w to recursive usecount (unless sparse).
  // if we are turning w to recursive, this is the last pass through all of w incrementing usecounts.  All currently-on-stack pointers to blocks are compatible with the increment
  // NOTE: it won't do to zap w, because AM may be invalid in WILLBEOPENED results from result.h (this could be fixed)
 tpop(old);  // delete everything allocated on the stack, except for w which was protected
 // Now we need to undo the effect of the initial ra and get the usecount back to its original value, with a matching tpush on the stack.
 // We could just do a tpush of the new block, but (1) we would just as soon do fa() rather than tpush() to save the overhead; (2) if the block was originally inplaceable
 // we would like to continue with it inplaceable.  The interesting case is when the block was NOT freed during the tpop.  That means that
 // the block was allocated somewhere else, either farther up the stack or in a name.  If the block is in a name, we must NOT do fa(), in case the
 // name is reassigned, freeing the components, while one of the components is a result.  OTOH, if the block actually is on the stack, it is safe
 // to do the fa().  We don't get it exactly right, but we note that any block that is part of a name will not be inplaceable, so we do the fa() only if
 // w is inplaceable - and in that case we can make the result here also inplaceable.  If the block was not inplaceable, or if it was freed during the tpop,
 // we push it again here.  In any case, if the input was inplaceable, so is the result.
 //
 // NOTE: certain functions (ex: rational determinant) perform operations 'in place' on non-direct names and then protect those names using gc().  The protection is
 // ineffective if the code goes through the fa() path here, because components that were modified will be freed immediately rather than later.  In those places we
 // must either use gc3() which always does the tpush, or do ACIPNO to force us through the tpush path here.  We generally use gc3().
 // Since w now has recursive usecounts (except for sparse, which is never inplaceable), we don't have to do a full fa() on a block that is returning
 // inplaceable - we just reset the usecount in the block.  If the block is returning inplaceable, we must update AM if we tpush; AM may have other uses if it is not returning inplaceable
 I cafter=AC(w); if((c&(1-cafter))>=0){A **amptr=(c<0?&AZAPLOC(w):(A**)&jt->shapesink); *amptr=jt->tnextpushp; tpush(w);}  // push unless was inplaceable and was not freed during tpop
 I *cptr=&AC(w); cptr=c<0?cptr:(I*)&jt->shapesink; *cptr=c; // make inplaceable if it was originally
 R w;
}
// EPILOGZAP: ra00; zap; tpop; AM=t->tnextpushp; tpush(w); ACRESET(, ACINPLACE|ACUC1)
// similar to jtgc, but done the simple way, by ra/pop/push always.  This is the thing to use if the argument
// is nonstandard, such as an argument that is operated on in-place with the result that the contents are younger than
// the enclosing area.  Modify the args if they need to be realized
// If the arguments are virtual, they will be realized
I jtgc3(J jt,A *x,A *y,A *z,A* old){
 if(x)RZ(ras(*x)); if(y)RZ(ras(*y)); if(z)RZ(ras(*z));
 tpop(old);
 if(x)tpush(*x); if(y)tpush(*y); if(z)tpush(*z);
 R 1;  // good return
}

// subroutine version of ra without rifv to save space
static A raonlys(AD * RESTRICT w) { RZQ(w);
#if AUDITEXECRESULTS
 if(AFLAG(w)&(AFVIRTUAL|AFUNINCORPABLE))SEGFAULT;
#endif
 ra(w); R w; }

// This routine handles the recursion for ra().  ra() itself does the top level, this routine handles the contents
I jtra(AD* RESTRICT wd,I t){I n=AN(wd);
 // we use if rather than switch because the first leg is most likely and the first two legs get almost everything
 if((t&BOX+SPARSE)>0){AD* np;
  // boxed.  Loop through each box, recurring if called for.  Two passes are intertwined in the loop
  A* RESTRICT wv=AAV(wd);  // pointer to box pointers
  if(n==0)R 0;  // Can't be mapped boxed; skip everything if no boxes
  np=*wv;  // prefetch first box
  NOUNROLL while(--n>0){AD* np0;  // n is always >0 to start.  Loop for n-1 times
   np0=*++wv;  // fetch next box if it exists, otherwise harmless value.  This fetch settles while the ra() is running
#ifdef PREFETCH
   PREFETCH((C*)np0);   // prefetch the next box while ra() is running
#endif
#if AUDITEXECRESULTS
if(np&&AC(np)<0)SEGFAULT;  // contents are never inplaceable
#endif
   if((np=QCWORD(np))!=0){ra(np);}  // increment the box, possibly turning it to recursive.  Low bits of box addr may be enqueue flags
   np=np0;  // advance to next box
  };
  if(np=QCWORD(np)){ra(np);}  // handle last one
 } else if(t&(VERB|ADV|CONJ)){V* RESTRICT v=FAV(wd);
  // ACV.
  // If it is a nameref, clear the bucket info.  Explanation in nameref()
  if(unlikely(v->id==CTILDE))if(v->fgh[0]&&AT(v->fgh[0])&NAME)NAV(v->fgh[0])->sb.sb.bucket=0;
  //  Recur on each component
  raonlys(v->fgh[0]); raonlys(v->fgh[1]); raonlys(v->fgh[2]);
 } else if(t&(RAT|XNUM|XD)) {A* RESTRICT v=AAV(wd);
  // single-level indirect forms.  handle each block
  DQ(t&RAT?2*n:n, if(*v)ACINCR(*v); ++v;);
 } else if(ISSPARSE(t)){P* RESTRICT v=PAV(wd); A x;
  // all elements of sparse blocks are guaranteed non-virtual, so ra will not reassign them
  x = SPA(v,a); raonlys(x);     x = SPA(v,e); raonlys(x);     x = SPA(v,i); raonlys(x);     x = SPA(v,x); raonlys(x);
 }
 R 1;
}

// This optionally deletes wd, after deleting its contents.  t is the recursion mask: if t contains a bit set for a recursive
// type, the contents of that type are processed.  We come here only if there is a traversible block to check
// Calls are from two sources.
// 1. fa().  In this case there must have been an earlier ra(), and thus we know that the block is
// recursive if it is RECURSIBLE.  The t argument is AT(wd).
// 1a. If the block is sparse, it is TRAVERSIBLE but not RECURSIBLE.  In this case we are called for any usecount,
// to call the descendants.  This is an old-style nonrecursive block.  We must check the usecount of this block and free
// it only if it is going to 0.  When we are called the usecount has not been modified and we must decrement it.
// 1b (normal). If the block is DIRECT or RECURSIBLE, we are called only when the usecount is going to 0.  We recur
// on descendants, and then free wd itself.  There is a special case for NAME blocks, described below, in which wd
// is not freed.
// 2. tpop processing, either in tpop itself or in early frees of unused arguments.  We are called only when the usecount
// is going to 0.  In this case we do not know
// that the block has been made recursive.  Also, we must NOT traverse nonrecursive traversible arguments, because each component of one
// is separately on the tpop stack.  However, we DO traverse a recursible block when its count goes to 0: making the block
// recursive created the need to traverse, and that must be honored.  Ex: create - ra - fa - tpop.  The t argument is
// AFLAG(wd)&RECURSIBLE, from which we can see the type and recursive status
void jtfamftrav(J jt,AD* RESTRICT wd,I t){I n=AN(wd);
  if((t&BOX+SPARSE)>0){AD* np;
   // boxed.  Loop through each box, recurring if called for.
   A* RESTRICT wv=AAV(wd);  // pointer to box pointers
   if(likely(n!=0)){  // skip everything if no boxes
    np=*wv;  // prefetch first box
    NOUNROLL while(--n>0){AD* np0;  // n is always >0 to start.  Loop for n-1 times
     np0=*++wv;  // fetch next box if it exists, otherwise harmless value.  This fetch settles while the ra() is running
#ifdef PREFETCH
     PREFETCH((C*)np0);   // prefetch the next box while ra() is running
#endif
     if((np=QCWORD(np))!=0){fanano0(np);}  // increment the box, possibly turning it to recursive.  Low bits of box addr may be enqueue flags
     np=np0;  // advance to next box
    };
    if((np=QCWORD(np))!=0){fanano0(np);}  // last box
   }
  } else if(t&NAME){A ref;
   if((ref=NAV(wd)->cachedref)!=0 && !(NAV(wd)->flag&NMCACHEDSYM)){I rc;  // reference, and not to a symbol.  must be to a ~ reference
    // we have to free cachedref, but it is tricky because it points back to us and we will have a double-free.  So, we have to change
    // the pointer to us, which is in fgh[0].  We look at the usecount of cachedref: if it is going to go away on the next fa(), we just clear fgh[0];
    // if it is going to stick around (which means that it is part of a tacit function that got assigned to a name, or the like), we return without freeing the reference
    if(AC(ref)<=1){FAV(ref)->fgh[0]=0; rc=0;  // cachedref going away - clear the pointer to prevent refree
    }else{  // cachedref survives - modify its NM block to break the loop
     NAV(wd)->cachedref=0; ACSET(wd,1) rc=1; // clear ref to leave name only, set count so it will free when reference is freed, prevent free of wd
    }
    fana(ref);  // free, now that nm is unlooped
    if(rc)R;  // avoid free if that is called for
   }
  } else if(t&(VERB|ADV|CONJ)){V* RESTRICT v=FAV(wd);
   // ACV.
   fana(v->fgh[0]); fana(v->fgh[1]); fana(v->fgh[2]);
  // SYMB must free as a monolith, with the symbols returned when the hashtables are
  }else if(t&SYMB){wd=jtfreesymtab(jt,wd,AR(wd));  // SYMB is used as a flag; we test here AFTER NAME and ADV which are lower bits
  } else if(t&(RAT|XNUM|XD)) {A* RESTRICT v=AAV(wd);
   // single-level indirect forms.  handle each block
   DQ(t&RAT?2*n:n, if(*v)fr(*v); ++v;);
  }else if(ISSPARSE(t)){P* RESTRICT v=PAV(wd);
   fana(SPA(v,a)); fana(SPA(v,e)); fana(SPA(v,i)); fana(SPA(v,x));
   // for sparse, decrement the usecount
   I c=AC(wd); if(--c>0){AC(wd)=c; R;}  // if sparse block not going away, just decr the usecount
  }
 mf(wd);
}
// Entry point to free after optional traversal.  clang gets this just right: if not TRAVERSIBLE, it inlines jtmf and pushes/pops no registers.
void jtfamf(J jt,AD* RESTRICT wd,I t){
 if(!(t&TRAVERSIBLE)){mf(wd); R;}
 jtfamftrav(jt,wd,t);
}

// Push wd onto the pop stack, and its descendants, possibly recurring on the descendants
// Result is new value of jt->tnextpushp, or 0 if error
// Note: wd CANNOT be virtual
// tpush, the macro parent of this routine, calls here only if a nonrecursive block is pushed.  This never happens for
// non-sparse nouns, because they always go through ra() somewhere before the tpush().  Pushing is mostly in gc() and on allocation in ga().
// It appears that non-nouns never come here either, so this is only for sparse
A *jttpush(J jt,AD* RESTRICT wd,I t,A *pushp){I af=AFLAG(wd); I n=AN(wd);
 if(likely(ISSPARSE(t))){P* RESTRICT v=PAV(wd);
  if(SPA(v,a))tpushi(SPA(v,a)); if(SPA(v,e))tpushi(SPA(v,e)); if(SPA(v,x))tpushi(SPA(v,x)); if(SPA(v,i))tpushi(SPA(v,i));
 }
 R pushp;
}

// Result is address of new stack pointer pushp, or 0 if error.  pushx must have just rolled over, i. e. is the 0 entry for the new block
// Caller is responsible for storing new pushp.
// We advance to a new stack block, and to a new allocation if necessary.
// BUT: if pushp is not within the current allocation, we do nothing.  In this case there has been a transfer of ownership and the allocated
// blocks are being put directly into 
A* jttg(J jt, A *pushp){     // Filling last slot; must allocate next page.
 // If pushp is outside the current allocation, do nothing
 if((UI)pushp-(UI)jt->tstackcurr>NTSTACK+NTSTACKBLOCK)R pushp;  // pushp outside top allocation: it's not the tpush stack, leave it alone.  > because we just stored into the previous word, so - = would be coming from inside
 A *prevpushp=pushp-1;  // the next block must chain back to the last valid pushp, not that value+1
 // If there is another block in the current allocation, use it.  When we finish pushp will point to the new block to use
 if((UI)pushp-(UI)jt->tstackcurr>NTSTACK){  // if there is room, pushp is already set
  // Not enough room for NTSTACKBLOCK bytes starting at pushp.  We need a new allocation
  // We keep up to one page that was previously allocated, so that we don't find ourselves allocating and freeing large blocks repeatedly as pushp crosses & recrosses
  // a block boundary
  if(jt->tstacknext) {   // if we already have a page to move to
//  jt->tstacknext[0] = jt->tstack;   // next was chained to prev before it was saved as next
   jt->tstackcurr = jt->tstacknext;   // switch back to it
   jt->tstacknext = 0;    // indicate no new one available now
  } else {A *v;   // no page to move to - better read one
   // We don't account for the NTSTACK blocks as part of memory space used, because it's so unpredictable and large as to be confusing
   if(!(v=MALLOC(NTSTACK+NTSTACKBLOCK))){  // Allocate block, with padding so we can have NTSTACK words on a block bdy AFTER the first word (which is a chain)
    // Unable to allocate a new block.  This is catastrophic, because we have done ra for blocks that we
    // will now not be able to tpop.  Memory is going to be lost.  The best we can do is prevent a crash.
    // We will leave tstack as is, pointing to the last block, and set nextpushx to the last entry in it.
    // This loses the last entry in the last block, and all the tpushes we couldn't perform.
    // The return will go all the way back to the first caller and beyond, so we set the values in jt as best we can
    jt->tnextpushp = --pushp;  // back up the push pointer to the last valid location
    ASSERT(0,EVWSFULL);   // fail
   }
   jt->malloctotal += NTSTACK+NTSTACKBLOCK;  // add to total allocated
   // chain previous allocation to the new one
   *v = (A)jt->tstackcurr;   // backchain old buffers to new, including bias
   jt->tstackcurr = (A*)v;    // set new buffer as the one to use, biased so we can index it from pushx
  }
  // use the first aligned block in the allocation 
  pushp = (A*)(((I)jt->tstackcurr+NTSTACKBLOCK)&(-NTSTACKBLOCK));  // get address of aligned block AFTER the first word
 }
 // point the chain of the new block to the end of the previous
 *pushp=(A)prevpushp;
 R pushp+1;  // Return pointer to first usable slot in the allocated block
}


// pop stack,  ending when we have freed the entry with tnextpushp==old.  tnextpushp is left pointing to an empty slot
// return value is pushp
// If the block has recursive usecount, decrement usecount in children if we free it
// stats I totalpops=0, nonnullpops=0, frees=0;
void jttpop(J jt,A *old){A *endingtpushp;
 // pushp points to an empty cell.  old points to the last cell to be freed.  decrement pushp to point to the cell to free (or to the chain).  decr old to match
 A *pushp=jt->tnextpushp;
 jt->tnextpushp = old;  // when we finish, this will be the new start point.  Set it early so we don't audit things in the middle of popping
 --pushp; --old;
 while(1) {A np;  // loop till end.  Return is at bottom of loop
  // pushp points to next cell to free
  // get the address of the first cell we cannot free in this block: old-1, if in the same block as pushp; otherwise cell 0 in pushp
  endingtpushp=(A*)((I)pushp&(-NTSTACKBLOCK));  // in case oldx in different block, use start of this one
  endingtpushp=((I)pushp^(I)old)&(-NTSTACKBLOCK)?endingtpushp:old;  // if old in this block, use it
  np=*pushp;   // get addr of first block to free
  NOUNROLL while(pushp!=endingtpushp){A np0;
   // np has next block to process.  It is *pushp
   --pushp;  // back up to next block (might be one after the last).  pushp now points to the block being processed in this pass
   // It is OK to prefetch the next box even on the last pass, because the next pointer IS a pointer to a valid box, or a chain pointer
   // to the previous free block (or 0 at end), all of which is OK to read and then prefetch from
   np0=*pushp;   // point to block for next pass through loop
// stats totalpops++;
   if(np){
// stats nonnullpops++;
    I c=AC(np);  // fetch usecount.
    I flg=AFLAG(np);  // fetch flags, just in case
#ifdef PREFETCH
    PREFETCH((C*)np0);   // prefetch the next box.  Might be 0; that's no crime
#endif
    // We never tpush a PERMANENT block so we needn't check for it.
    // If count goes to 0: if the usercount is marked recursive, do the recursive fa(), otherwise just free using mf().  If virtual, the backer must be recursive, so fa() it
    // Otherwise just decrement the count
    if(--c<=0){
// stats ++frees;
     // The block is going to be destroyed.  See if there are further ramifications
     if(!(flg&AFVIRTUAL)){fanapop(np,flg);}   // do the recursive POP only if RECURSIBLE block; then free np
     else{A b=ABACK(np); fanano0(b); mf(np);}  // if virtual block going away, reduce usecount in backer, ignore the flagged recursiveness just free the virt block
      // NOTE that ALL non-faux virtual blocks, even self-virtual ones, are on the tpop stack & are deleted here
      // NOTE: a sparse recursive would cause trouble, because the sparseness is not in the flag and we would have to test the type as well.  To avoid this,
      // we make sure no such block is created in sprz()
    }else ACSET(np,c)
   }
   np=np0;  // Advance to next block
  }
  // np has the pointer before the last one we processed in this block.  pushp points to that.  See if there are more blocks to do
  if(endingtpushp!=old){      // If we haven't done them all, we must have hit start-of-block.  Move back to previous block
   // end-of-block.  np=*pushp is the chain to the end of the previous block.  We will go there, but first see if we have finished the current allocation
   // There is no way two allocations could back up so as to make the end of one exactly the beginning of the other
   if((A*)np!=pushp-1){
    // if there is another block in this allocation, step to it.  Otherwise:
    if(jt->tstacknext){FREECHK(jt->tstacknext); jt->malloctotal-=NTSTACK+NTSTACKBLOCK;}   // account for malloc'd memory
  // We will set the block we are vacating as the next-to-use.  We can have only 1 such; if there is one already, free it
    jt->tstacknext=jt->tstackcurr;  // save the next-to-use, after removing bias
    jt->tstackcurr=(A*)jt->tstackcurr[0];   // back up to the previous block
   }
   pushp=(A*)np; // move to the next block, whichever allocation it is in 
  } else {
   // The return point:
#if MEMAUDIT&2
   audittstack(jt);   // one audit for each tpop.  Mustn't audit inside tpop loop, because that's inconsistent state
#endif
   R;
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
F1(jtrat){ARGCHK1(w); ras(w); tpush(w); R w;}  // recursive.  w can be zero only if explicit definition had a failing sentence

A jtras(J jt, AD * RESTRICT w) { ARGCHK1(w); realizeifvirtual(w); ra(w); R w; }  // subroutine version of ra() to save space
A jtra00s(J jt, AD * RESTRICT w) { ARGCHK1(w); ra00(w,AT(w)); R w; }  // subroutine version of ra00() to save space
A jtrifvs(J jt, AD * RESTRICT w) { ARGCHK1(w); realizeifvirtual(w); R w; }  // subroutine version of rifv() to save space and be an rvalue
A jtmkwris(J jt, AD * RESTRICT w) { ARGCHK1(w); makewritable(w); R w; }  // subroutine version of makewritable() to save space and be an rvalue

#if MEMAUDIT&8
static I lfsr = 1;  // holds varying memory pattern
#endif

// call tp, but return the value passed in as z.  Used to save a register in caller
__attribute__((noinline)) A jttgz(J jt,A *tp, A z){RZ(tp=tg(tp)); jt->tnextpushp=tp; R z;}

__attribute__((noinline)) A jtgafallopool(J jt,I blockx,I n){
 A u,chn; US hrh;
#if ALIGNPOOLTOCACHE   // with smaller headers, always align pool allo to cache bdy
 // align the buffer list on a cache-line boundary
 I *v; ASSERT(v=MALLOC(PSIZE+TAILPAD+ALIGNPOOLTOCACHE*CACHELINESIZE),EVWSFULL);
 A z=(A)(((I)v+CACHELINESIZE)&-CACHELINESIZE);   // get cache-aligned section
 ((I**)z)[-1] = v;   // save address of entire allocation in the word before the aligned section
#else
 // allocate without alignment
 ASSERT(av=MALLOC(PSIZE+TAILPAD),EVWSFULL);
#endif
 I nt=jt->malloctotal+=PSIZE+TAILPAD+ALIGNPOOLTOCACHE*CACHELINESIZE;  // add to total JE mem allocated
 jt->mfreegenallo+=PSIZE+TAILPAD+ALIGNPOOLTOCACHE*CACHELINESIZE;   // ...add them to the total bytes allocated drom OS
 {I ot=jt->malloctotalhwmk; ot=ot>nt?ot:nt; jt->malloctotalhwmk=ot;}
 // split the allocation into blocks.  Chain them together, and flag the base.  We chain them in ascending order (the order doesn't matter), but
 // we visit them in back-to-front order so the first-allocated headers are in cache
#if MEMAUDIT&17 && BW==64
 u=(A)((C*)z+PSIZE); chn = 0; hrh = FHRHENDVALUE(1+blockx-PMINL);
 DQ(PSIZE/2>>blockx, u=(A)((C*)u-n); AFCHAIN(u)=chn; chn=u; if(MEMAUDIT&4)AC(u)=(I)0xdeadbeefdeadbeefLL; hrh -= FHRHBININCR(1+blockx-PMINL); AFHRH(u)=hrh;);   // chain blocks to each other; set chain of last block to 0
 AFHRH(u) = hrh|FHRHROOT;    // flag first block as root.  It has 0 offset already
#else
 u=(A)((C*)z+PSIZE); chn = 0; hrh = FHRHENDVALUE(1+blockx-PMINL); DQ(PSIZE/2>>blockx, u=(A)((C*)u-n); AFCHAIN(u)=chn; chn=u; hrh -= FHRHBININCR(1+blockx-PMINL); AFHRH(u)=hrh;);    // chain blocks to each other; set chain of last block to 0
 AFHRH(u) = hrh|FHRHROOT;  // flag first block as root.  It has 0 offset already
#endif
 jt->mfree[-PMINL+1+blockx].pool=(A)((C*)u+n);  // the second block becomes the head of the free list
 if(unlikely((((jt->mfree[-PMINL+1+blockx].ballo+=n-PSIZE)&MFREEBCOUNTING)!=0))){     // We are adding a bunch of free blocks now...
  jt->bytes += n; if(jt->bytes>jt->bytesmax)jt->bytesmax=jt->bytes;
 }
 A *tp=jt->tnextpushp; AZAPLOC(z)=tp; *tp++=z; jt->tnextpushp=tp; if(unlikely(((I)tp&(NTSTACKBLOCK-1))==0))RZ(z=jttgz(jt,tp,z)); // do the tpop/zaploc chaining
 R z;
}

// allocate from OS and fill in h field.  n is full size to allocate, padded for all reasons
__attribute__((noinline)) A jtgafalloos(J jt,I blockx,I n){A z;
#if ALIGNTOCACHE
 // Allocate the block, and start it on a cache-line boundary
 I *v;
 ASSERT(v=MALLOC(n),EVWSFULL);
 z=(A)(((I)v+CACHELINESIZE)&-CACHELINESIZE);   // get cache-aligned section
 ((I**)z)[-1] = v;    // save address of original allocation
#else
 ASSERT(z=MALLOC(n),EVWSFULL);
#endif
 AFHRH(z) = (US)FHRHSYSJHDR(1+blockx);    // Save the size of the allocation so we know how to free it and how big it was
 if(unlikely((((jt->mfreegenallo+=n)&MFREEBCOUNTING)!=0))){
  jt->bytes += n; if(jt->bytes>jt->bytesmax)jt->bytesmax=jt->bytes;
 }
 I nt=jt->malloctotal+=n;
 {I ot=jt->malloctotalhwmk; ot=ot>nt?ot:nt; jt->malloctotalhwmk=ot;}
 A *tp=jt->tnextpushp; AZAPLOC(z)=tp; *tp++=z; jt->tnextpushp=tp; if(unlikely(((I)tp&(NTSTACKBLOCK-1))==0))RZ(z=jttgz(jt,tp,z)); // do the tpop/zaploc chaining
 R z;
}

// static auditmodulus = 0;
// blockx is bit# of MSB in (length-1), i. e. lg2(bufsize)-1
RESTRICTF A jtgaf(J jt,I blockx){A z;
// audit free chain I i,j;MS *x; for(i=PMINL;i<=PLIML;++i){j=0; x=(jt->mfree[-PMINL+i].pool); while(x){x=(MS*)(x->a); if(++j>25)break;}}  // every time, audit first 25 entries
// audit free chain if(++auditmodulus>25){auditmodulus=0; for(i=PMINL;i<=PLIML;++i){j=0; x=(jt->mfree[-PMINL+i].pool); while(x){x=(MS*)(x->a); ++j;}}}
// use 6!:5 to start audit I i,j;MS *x; if(JT(jt,peekdata)){for(i=PMINL;i<=PLIML;++i){j=0; x=(jt->mfree[-PMINL+i].pool); while(x){x=(MS*)(x->a); ++j;}}}
#if MEMAUDIT&16
auditmemchains();
#endif
#if MEMAUDIT&15
if((I)jt&3)SEGFAULT;
#endif
#if MEMHISTO
 jt->memhisto[blockx+1]++;  // record the request, at its size
#endif
 z=jt->mfree[-PMINL+1+blockx].pool;   // tentatively use head of free list as result - normal case, and even if blockx is out of bounds will not segfault
 I n=(I)2<<blockx;  // n=size of allocated block
 ASSERT(2>*JT(jt,adbreakr),EVBREAK)  // this is JBREAK0.  Fails if break pressed twice

 if(blockx<PLIML){
  // small block: allocate from pool

  if(likely(z!=0)){         // allocate from a chain of free blocks
   jt->mfree[-PMINL+1+blockx].pool = AFCHAIN(z);  // remove & use the head of the free chain
   // If the user is keeping track of memory high-water mark with 7!:2, figure it out & keep track of it.  Otherwise save the cycles.  All allo routines must do this
   if(unlikely((((jt->mfree[-PMINL+1+blockx].ballo+=n)&MFREEBCOUNTING)!=0))){
    jt->bytes += n; if(jt->bytes>jt->bytesmax)jt->bytesmax=jt->bytes;
   }
   // Put the new block into the tpop stack and point the blocks to its zappable tpop slot.  We have to check for a new tpop stack block, and we cleverly
   // pass z into that function, which will return it unchanged, so that we don't have to push the value in this routine
   // We require each other allocation routine to copy this, so that they don't need registers saved either
   A *tp=jt->tnextpushp; AZAPLOC(z)=tp; *tp++=z; jt->tnextpushp=tp; if(unlikely(((I)tp&(NTSTACKBLOCK-1))==0))RZ(z=jttgz(jt,tp,z)); // advance to next slot, allocating a new block as needed
#if MEMAUDIT&1
   if(AFCHAIN(z)&&FHRHPOOLBIN(AFHRH(AFCHAIN(z)))!=(1+blockx-PMINL))SEGFAULT;  // reference the next block to verify chain not damaged
   if(FHRHPOOLBIN(AFHRH(z))!=(1+blockx-PMINL))SEGFAULT;  // verify block has correct size
#endif
  }else{ // small block, but chain is empty.  Alloc PSIZE and split it into blocks
   RZ(z=jtgafallopool(jt,blockx,n));
  }
 } else {      // here for non-pool allocs...
  n+=TAILPAD+ALIGNTOCACHE*CACHELINESIZE;  // add to the allocation for the fixed tail and the alignment area
  RZ(z=jtgafalloos(jt,blockx,n));  // ask OS for block, and fill in AFHRH.  We want to keep only jt over this call
 }
#if MEMAUDIT&8
 DO((((I)1)<<(1+blockx-LGSZI)), lfsr = (lfsr<<1LL) ^ (lfsr<0?0x1b:0); if(i!=2&&i!=6)((I*)z)[i] = lfsr;);   // fill block with garbage - but not the allocation word or zaploc
#endif
 AFLAGINIT(z,0) ACINIT(z,ACUC1|ACINPLACE)  // all blocks are born inplaceable, and point to their deletion entry in tpop
  // we do not attempt to combine the AFLAG write into a 64-bit operation
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
#if SHOWALLALLOC
printf("%p+\n",z);
#endif
 R z;
}

// bytes is total #bytes needed including headers, -1
RESTRICTF A jtgafv(J jt, I bytes){UI4 j;
#if NORMAH*(SY_64?8:4)<(1LL<<(PMINL-1))
 bytes|=(I)1<<(PMINL-1);  // if the memory header itself doesn't meet the minimum buffer length, insert a minimum
#endif
 CTLZI((UI)bytes,j);  // 3 or 4 should return 2; 5 should return 3
 ASSERT((UI)bytes<=(UI)JT(jt,mmax),EVLIMIT)
 R jtgaf(jt,(I)j);
}

#if 0 // obsolete 
RESTRICTF A jtga(J jt,I type,I atoms,I rank,I* shaape){A z;
 // Get the number of bytes needed-1, including the header, the atoms, and a full I appended for types that require a
 // trailing NUL (because boolean-op code needs it)
// obsolete  I bpt; if(likely(CTTZ(type)<=C4TX))bpt=bpnoun(type);else bpt=bp(type);
// obsolete  I bytes = ALLOBYTESVSZ(atoms,rank,bpt,type&LAST0,0);  // We never use GA for NAME types, so we don't need to check for it
 I bytes; if(likely(type&(((I)1<<(LASTNOUNX+1))-1)))bytes = ALLOBYTESVSZLG(atoms,rank,bplg(type),type&LAST0,0);else bytes = ALLOBYTESVSZ(atoms,rank,bpnonnoun(type),type&LAST0,0);
#if SY_64
 ASSERT(!((((unsigned long long)(atoms))&~TOOMANYATOMS)+((rank)&~RMAX)),EVLIMIT)
#else
 ASSERT(((I)bytes>(I)(atoms)&&(I)(atoms)>=(I)0)&&!((rank)&~RMAX),EVLIMIT)
#endif
 RZ(z=jtgafv(jt, bytes));   // allocate the block, filling in AC and AFLAG
 I akx=AKXR(rank);   // Get offset to data
 AK(z)=akx; AT(z)=type; AN(z)=atoms;   // Fill in AK, AT, AN
 // Set rank, and shape if user gives it.  This might leave the shape unset, but that's OK
 AR(z)=(RANKT)rank;   // Storing the extra last I (as was done originally) might wipe out rank, so defer storing rank till here
 // Since we allocate powers of 2, we can make the memset a multiple of 32 bytes.  The value of an atomic box would come before the cleared region, but we pick that up here when the shape is cleared
// obsolete  if(!((type&DIRECT)>0)){if(SY_64){mvc((bytes-32)&-32,(C*)(AS(z)+1),1,MEMSET00);}else{mvc(bytes+1-akx,(C*)z+akx,1,MEMSET00);}}  // bytes=63=>0 bytes cleared.  bytes=64=>32 bytes cleared.  bytes=64 means the block is 65 bytes long
 if(!((type&DIRECT)>0)){AS(z)[0]=0; mvc((bytes-(offsetof(AD,s[1])-32))&-32,(C*)(AS(z)+1),1,MEMSET00);}  // bytes=63=>0 bytes cleared.  bytes=64=>32 bytes cleared.  bytes=64 means the block is 65 bytes long
 GACOPYSHAPEG(z,type,atoms,rank,shaape)  /* 1==atoms always if ISSPARSE(t)  */  // copy shape by hand since short
  // Tricky point: if rank=0, GACOPYSHAPEG stores 0 in AS[0] so we don't have to do that in the DIRECT path
   // All non-DIRECT types have items that are multiples of I, so no need to round the length
// stats  obsolete ++scafnga; scafngashape+=shaape!=0;
 R z;
}
#endif

#if SY_64
// stats obsolete I scafnga=0, scafngashape=0;
// like jtga, but don't copy shape,   Never called for SPARSE type
// We pack rank+type into one reg to save registers (it also halps the LIMIT test).  With this, the compiler should be able to save/restore
// only 2 regs (ranktype and bytes) but the prodigal compiler saves 3.  We accept this to be able to save AK AR AT here, so that the caller doesn't have to preserve them over the call.
// We don't store AN, because that would take another push/pop and we hope the caller needs to preserve it anyway.
RESTRICTF A jtga0(J jt,I ranktype,I atoms){A z;
 // Get the number of bytes needed-1, including the header, the atoms, and a full I appended for types that require a
 // trailing NUL (because boolean-op code needs it)
// obsolete  I bpt; if(likely(CTTZ(type)<=C4TX))bpt=bpnoun(type);else bpt=bp(type);
// obsolete  I bytes = ALLOBYTESVSZ(atoms,rank,bpt,type&LAST0,0)&-2;  // We never use GA for NAME types, so we don't need to check for it
 I bytes; if(likely(ranktype&(((I)1<<(LASTNOUNX+1))-1)))bytes = ALLOBYTESVSZLG(atoms,ranktype>>32,bplg(ranktype),ranktype&LAST0,0);else bytes = ALLOBYTESVSZ(atoms,ranktype>>32,bpnonnoun(ranktype),ranktype&LAST0,0);
    // We never use GA for NAME types, so we don't need to check for it
 ASSERTSYS(!ISSPARSE((I4)ranktype),"jtga0 Never called for SPARSE type");
 ASSERT(((atoms|ranktype)>>(32+LGRMAX))==0,EVLIMIT)
 RZ(z=jtgafv(jt, bytes));   // allocate the block, filling in AC AFLAG AM
 AT(z)=(I4)ranktype; I rank=(UI)ranktype>>32; ARINIT(z,rank); AK(z)=AKXR(rank);  // UI to prevent reusing the value from before the call
 // Clear data for non-DIRECT types in case of error
 // Since we allocate powers of 2, we can make the memset a multiple of 32 bytes.
 if(unlikely(!(((I4)ranktype&DIRECT)>0))){AS(z)[0]=0; mvc((bytes-32)&-32,&AS(z)[1],1,MEMSET00);}  // unlikely is important!  compiler strains then to use one less temp reg
// obsolete  if(!((type&DIRECT)>0)){if(SY_64){mvc((bytes-(offsetof(AD,s[1])-32))&-32,(C*)(AS(z)+1),1,MEMSET00);}else{mvc(bytes+1-akx,(C*)z+akx,1,MEMSET00);}}  // bytes=63=>0 bytes cleared.  bytes=64=>32 bytes cleared.  bytes=64 means the block is 65 bytes long
// stats  obsolete ++scafnga; scafngashape+=shaape!=0;
 R z;
}
#else
RESTRICTF A jtga0(J jt,I type,I rank,I atoms){A z;
 I bytes; if(likely(type&(((I)1<<(LASTNOUNX+1))-1)))bytes = ALLOBYTESVSZLG(atoms,rank,bplg(type),type&LAST0,0);else bytes = ALLOBYTESVSZ(atoms,rank,bpnonnoun(type),type&LAST0,0);
 ASSERT(((I)bytes>(I)(atoms)&&(I)(atoms)>=(I)0)&&!((rank)&~RMAX),EVLIMIT)
 RZ(z=jtgafv(jt, bytes));   // allocate the block, filling in AC and AFLAG
 AT(z)=type; ARINIT(z,rank); AK(z)=AKXR(rank);  // UI to prevent reusing the value from before the call
 if(unlikely(!((type&DIRECT)>0))){AS(z)[0]=0; mvc((bytes-(offsetof(AD,s[1])-32))&-32,(C*)(AS(z)+1),1,MEMSET00);}
 R z;
}
#endif


// free a block.  The usecount must make it freeable
void jtmf(J jt,A w,I hrh){I mfreeb;
#if MEMAUDIT&16
auditmemchains();
#endif
#if MEMAUDIT&15
if((I)jt&3)SEGFAULT;
#endif
#if MEMAUDIT&4
if((AC(w)>>(BW-2))==-1)SEGFAULT;  // high bits 11 must be deadbeef
#endif
#if LEAKSNIFF
 if(leakcode){I i;
  // Remove block from the table if the address matches
  I *lv=IAV(leakblock);
  for(i = 0;i<leaknbufs&&lv[2*i]!=(I)w;++i);  // find the match
  if(i<leaknbufs){NOUNROLL while(i+1<leaknbufs){lv[2*i]=lv[2*i+2]; lv[2*i+1]=lv[2*i+3]; ++i;} leaknbufs=i;}  // remove it
 }
#endif

// audit free list {I Wi,Wj;MS *Wx; for(Wi=PMINL;Wi<=PLIML;++Wi){Wj=0; Wx=(jt->mfree[-PMINL+Wi].pool); while(Wx){Wx=(MS*)(Wx->a); ++Wj;}}}
#if SHOWALLALLOC
printf("%p-\n",w);
#endif
 I blockx=FHRHPOOLBIN(hrh);   // pool index, if pool
#if MEMAUDIT&1
 if(hrh==0 || blockx>(PLIML-PMINL+1))SEGFAULT;  // pool number must be valid
#if MEMAUDIT&17
#endif
#endif
 I allocsize;  // size of full allocation for this block
 if(FHRHBINISPOOL(hrh)){   // allocated from subpool
  allocsize = FHRHPOOLBINSIZE(hrh);
#if MEMAUDIT&4
  DO((allocsize>>LGSZI), if(i!=6)((I*)w)[i] = (I)0xdeadbeefdeadbeefLL;);   // wipe the block clean before we free it - but not the reserved area
#endif
  jt->bytes -= allocsize;  // keep track of total allocation
  mfreeb = jt->mfree[blockx].ballo;   // number of bytes allocated at this size (biased zero point)
  AFCHAIN(w)=jt->mfree[blockx].pool;  // append free list to the new addition...
  jt->mfree[blockx].pool=w;   //  ...and make new addition the new head
  if(unlikely(0 > (mfreeb-=allocsize)))jt->uflags.us.uq.uq_c.spfreeneeded=1;  // Indicate we have one more free buffer;
   // if this kicks the list into garbage-collection mode, indicate that
  jt->mfree[blockx].ballo=mfreeb;
 }else{                // buffer allocated from malloc
  mfreeb = jt->mfreegenallo;
  allocsize = FHRHSYSSIZE(hrh);
#if MEMAUDIT&4
  DO((allocsize>>LGSZI), if(i!=6)((I*)w)[i] = (I)0xdeadbeefdeadbeefLL;);   // wipe the block clean before we free it - but not the reserved area
#endif
  allocsize+=TAILPAD+ALIGNTOCACHE*CACHELINESIZE;  // the actual allocation had a tail pad and boundary
  jt->bytes -= allocsize;  // keep track of total allocation
  jt->malloctotal-=allocsize;
  jt->mfreegenallo-=allocsize;  // account for all the bytes returned to the OS
#if ALIGNTOCACHE
  FREECHK(((I**)w)[-1]);  // point to initial allocation and free it
#else
  FREECHK(w);  // free the block
#endif
 }
}

// allocate header with rank r; if r==1, move the item count to be the shape also
// a header is a simplified virtual block, for temporary use only, that must never escape into the wild, either in full or
// as a backer for a virtual block
RESTRICTF A jtgah(J jt,I r,A w){A z;
 ASSERT(RMAX>=r,EVLIMIT); 
 RZ(z=gafv(SZI*(NORMAH+r)-1));
 AT(z)=0;
 if(w){
  AT(z)=AT(w); AN(z)=AN(w); ARINIT(z,(RANKT)r); AK(z)=CAV(w)-(C*)z;
  if(1==r)AS(z)[0]=AN(w);
 }
 R z;
}    /* allocate header */ 

// clone w, returning the address of the cloned area.  Result is NOT recursive, not AFRO, not virtual
F1(jtca){A z;I t;P*wp,*zp;
 ARGCHK1(w);
 I n=AN(w);  t=AT(w);
 if(unlikely(ISSPARSE(t))){
  GASPARSE(z,t,n,AR(w),AS(w))
  wp=PAV(w); zp=PAV(z);
  SPB(zp,a,ca(SPA(wp,a)));
  SPB(zp,e,ca(SPA(wp,e)));
  SPB(zp,i,ca(SPA(wp,i)));
  SPB(zp,x,ca(SPA(wp,x)));
 }else{
  if(t&NAME){GATV(z,NAME,n,AR(w),AS(w));AT(z)=t;}  // GA does not allow NAME type, for speed
  else {
   n=t&FUNC?(VERBSIZE+SZI-1)>>LGSZI:n;  // AN field of func is used for 
   GA(z,t,n,AR(w),AS(w));
   AN(z)=AN(w);  // copy AN, which has its own meaning in FUNC
  }
  I bpt; if(likely(CTTZ(t)<=C4TX))bpt=bpnoun(t);else bpt=bp(t);
  MC(AV(z),AV(w),(n*bpt)+(t&NAME?sizeof(NM):0));
 }
 R z;
}
// clone block only if it is read-only
F1(jtcaro){ if(AFLAG(w)&AFRO){RETF(ca(w));} RETF(w); }

// clone virtual block, producing a new virtual block
F1(jtclonevirtual){
 A z; RZ(z=virtual(w,0,AR(w)));  // allocate a new virtual block
 AN(z)=AN(w); MCISH(AS(z),AS(w),(I)AR(w));  // copy AN and shape; leave AC alone
 R z;
}

B jtspc(J jt){A z; RZ(z=MALLOC(1000)); FREECHK(z); R 1; }  // see if 1000 bytes are available before we embark on error display

// Double the allocation of w (twice as many atoms), then round up # items to max allowed in allocation
// if b=1, the result will replace w, so decrement usecount of w and increment usecount of new buffer
// the itemcount of the result is set as large as will fit evenly, and the atomcount is adjusted accordingly
// NOTE: w is not recursive, and its usecount must always be 1
A jtext(J jt,B b,A w){A z;I c,k,m,m1,t;
 ARGCHK1(w);                               /* assume AR(w)&&AN(w)    */
 m=AS(w)[0]; PROD(c,AR(w)-1,AS(w)+1);
 t=AT(w); I bpt; if(likely(CTTZ(t)<=C4TX))bpt=bpnoun(t);else bpt=bp(t); k=c*bpt;
 GA00(z,t,2*AN(w)+(AN(w)?0:c),AR(w));  // ensure we allocate SOMETHING to make progress
 m1=allosize(z)/k;  // start this divide before the copy
 MC(AV(z),AV(w),AN(w)*bpt);                 /* copy old contents      */
 MCISH(&AS(z)[1],&AS(w)[1],AR(w)-1);
 if(b){ACINITZAP(z); mf(w);}          // 1=b iff w is permanent.  This frees up the old block but not the contents, which were transferred as is
 AS(z)[0]=m1; AN(z)=m1*c;       /* "optimal" use of space */
 if(!((t&DIRECT)>0))mvc(k*(m1-m),CAV(z)+m*k,1,MEMSET00);  // if non-DIRECT type, zero out new values to make them NULL
 R z;
}

A jtexta(J jt,I t,I r,I c,I m){A z;I m1; 
 GA00(z,t,m*c,r); 
 I k=bp(t); AS(z)[0]=m1=allosize(z)/(c*k); AN(z)=m1*c;
 if(2==r)*(1+AS(z))=c;
 if(!((t&DIRECT)>0))mvc(k*AN(z),AV(z),1,MEMSET00);
 R z;
}    /* "optimal" allocation for type t rank r, c atoms per item, >=m items */

// forcetomemory does nothing, but it does take an array as argument.  This will spook the compiler out of trying to assign parts of the array to registers.
void forcetomemory(void * w){R; }

#if (C_AVX2&&SY_64) || EMU_AVX2
__m256d initecho(void * addr) {R _mm256_broadcast_sd((D*)addr);}   // no longer used
#endif

