/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Memory Management                                                       */

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
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
#define TAILPAD (32)  // we must ensure that a 32-byte masked op fetch to the last+1 byte doesn't run off into unallocated memory

#define MEMJMASK 0xf   // these bits of j contain subpool #; higher bits used for computation for subpool entries
#define SBFREEBLG (14+PMINL)   // lg2(SBFREEB)
#define SBFREEB (1L<<SBFREEBLG)   // number of bytes that need to be freed before we rescan
#define MFREEBCOUNTING 1   // When this bit is set in mfreeb[], we keep track of max space usage

#if (MEMAUDIT==0 || !_WIN32) || 1  // windows makes free() a void
#define FREECHK(x) FREE(x)
#else
#define FREECHK(x) if(!FREE(x))SEGFAULT;  // crash on error
#endif

// memory reservation routines
#if !SY_WIN32
static I pagesz=0,pagemask=0,pagermask=0;
void *jvmreserve(I n){ // returns a pointer to a reservation of n bytes.  Must be committed before use.  Always at least 4k aligned
 void *r=mmap(0, n, PROT_NONE, MAP_PRIVATE|MAP_ANON, -1, 0);
 R r==MAP_FAILED?0:r;}
B jvmcommit(void *p,I n){ // commits n bytes starting at p, a pointer within a previous reservation.  Returns 1=success
 if(!pagesz)pagesz=sysconf(_SC_PAGESIZE),pagemask=pagesz-1,pagermask=~pagemask;
 I pi=(I)p;
 n+=pi-(pagermask&pi);
 p=(void*)(pagermask&pi);
 R !mprotect(p,n,PROT_READ|PROT_WRITE);}
void *jvmalloc(I n){ //reserve+commit in one step
 void *r=mmap(0, n, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0);
 R r==MAP_FAILED?0:r;}
void jvmdecommit(void *p,I n){ // decommits n bytes starting at p (while keeping the address space reserved).  Beware of page boundaries!
 mprotect(p,n,PROT_NONE);}
void jvmrelease(void *p,I n){ // unreserves (and decommits) n bytes starting at p.  Note: must apply to the entirety of the original reservation at once for compat with windows
 munmap(p,n);}
void *jvmreservea(I n,I a){ //jvmreserve, but result is a multiple of 1<<a
 // todo: freebsd has MAP_ALIGNED
 a=1<<a;I m=a-1;
 C *r=jvmreserve(n+a);I ri=(I)r;
 if(!(ri&m)){ //result already aligned; skip a syscall and just release the tail
  jvmrelease(r+n,a);
  R r;}
 jvmrelease(r,a-(ri&m));
 jvmrelease(r+n+a-(ri&m),ri&m);
 R r+a-(ri&m);}
void *jvmalloca(I n,I a){ //jvmalloc, but result is a multiple of 1<<a
 void *r=jvmreservea(n,a);
 if(!jvmcommit(r,n)){jvmrelease(r,n);R 0;}
 R r;}
B jvmwire(void *p,I n){ // wires n bytes starting at p, ensuring they will not be paged out.  Must have been previously committed.  Returns 1=success
 I pi=(I)p;
 n+=pi-(pagermask&pi);
 p=(void*)(pagermask&pi);
 R !mlock(p,n);}
void jvmunwire(void *p,I n){ // unwires n bytes starting at p (while keeping them committed).  Beware of page boundaries!
 I pi=(I)p;
 n+=pi-(pagermask&pi);
 p=(void*)(pagermask&pi);
 munlock(p,n);}
#else //windows.  Interface is the same
void *jvmreserve(I n){ R VirtualAlloc(0,n,MEM_RESERVE,PAGE_NOACCESS); }
B jvmcommit(void *p,I n){ R !!VirtualAlloc(p,n,MEM_COMMIT,PAGE_READWRITE); }
void *jvmalloc(I n){ R VirtualAlloc(0,n,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE); }
void jvmdecommit(void *p,I n){ VirtualFree(p,n,MEM_DECOMMIT); }
void jvmrelease(void *p,I n){ VirtualFree(p,n,MEM_DECOMMIT); }  //using MEM_DECOMMIT rather than MEM_RELEASE for now, so long as vmreservea/vmalloca return partial results
//Actually, it should be possible to use VirtualAlloc to query the base ptr, and then MEM_RELEASE that 
#if 0 //can't use VirtualAlloc2--why?
void *jvmreservea(I n,I a){
 MEM_ADDRESS_REQUIREMENTS req = {.Alignment=1<<a};
 MEM_EXTENDED_PARAMETER opt = {.Type=MemExtendedParameterAddressRequirements, .Pointer=&req};
 R VirtualAlloc2(0,0,n,MEM_RESERVE,0,&opt,1);}
void *jvmalloca(I n,I a){
 MEM_ADDRESS_REQUIREMENTS req = {.Alignment=1<<a};
 MEM_EXTENDED_PARAMETER opt = {.Type=MemExtendedParameterAddressRequirements, .Pointer=&req};
 R VirtualAlloc2(0,0,n,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE,&opt,1);}
#else
void *jvmreservea(I n,I a){
 a=1<<a;
 void *r=VirtualAlloc(0,n+a,MEM_RESERVE,PAGE_NOACCESS);
 R (void*)((-a)&((I)r+a-1));} //no null-checking needed--this will be 0 if r is.  Assumes null is 0, which is only untrue in adversarial environments.
void *jvmalloca(I n,I a){
 a=1<<a;
 void *p=VirtualAlloc(0,n+a,MEM_RESERVE,PAGE_NOACCESS);
 if(!p)R p;
 void *r=VirtualAlloc((void*)((-a)&((I)p+a-1)),n,MEM_COMMIT,PAGE_READWRITE);
 if(!r){VirtualFree(p,0,MEM_RELEASE);R 0;}
 R r;}
B jvmwire(void *p,I n){
 R !!VirtualLock(p,n);}
void jvmunwire(void *p,I n){
 VirtualUnlock(p,n);}
#endif
#endif

#if LEAKSNIFF
static I leakcode;
static A leakblock;
static I leaknbufs;
#endif

#if CRASHLOG // debugging
#define LOGFILE "/home/chris/t9.txt"
// #define LOGFILE "C:/My Documents/crashlog.txt"
int logfirsttime = 1;
int logparm=0;  // set to control logging inside m.c
char logarea[200];  // where messages are built
void writetolog(J jt,C *s){A z;
RESETRANK;
int svlogparm=logparm; logparm=0;
(logfirsttime?jtjfwrite:jtjfappend)(jt,str(strlen(s),s),box(str(strlen(LOGFILE),LOGFILE)));
logfirsttime=0; logparm=svlogparm;
}
#endif


// Return the total length of the data area of y, i. e. the number of bytes from start-of-data to end-of-allocation
// The allocation size depends on the type of allocation.  The block must not be read-only
I allosize(A y) {
 if(unlikely(AFLAG(y)&AFVIRTUAL))R 0;  // if this block is virtual, you can't append to the data, so don't ask about the length
 if(likely(!(AFLAG(y)&(AFNJA)))) {
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
F1(jtmemhistoq){F12IP;
 ASSERTMTV(w);
 R vec(INT,sizeof(jt->memhisto)/sizeof(jt->memhisto)[0],jt->memhisto);
}

F1(jtmemhistos){F12IP;I k;
 ASSERTMTV(w); 
 mvc(sizeof(jt->memhisto),jt->memhisto,MEMSET00LEN,MEMSET00);
 R mtm;
}


// return histo area

// process using   ;"1 (":@{. ; ' ' ; 3 (3!:4) {:)"1 (20) {. \:~ |."1 (_2) ]\ 9!:62''
F1(jtmemhashq){F12IP;
 R vec(INT,sizeof(histarea)/sizeof(histarea)[0][0],histarea);
}
F1(jtmemhashs){F12IP;
 ASSERTMTV(w); 
 mvc(sizeof(histarea,histarea,MEMSET00LEN,MEMSET00));
 R mtm;
}

#endif

// initialize shared state for memory allocator
B jtmeminits(JS jjt){
 INITJT(jjt,adbreakr)=INITJT(jjt,adbreak)=(C*)&INITJT(jjt,breakbytes); /* required for ma to work */
 INITJT(jjt,mmax) =(I)1<<(MLEN-1);
 R 1;}

// initialise thread-specific state for memory allocator
B jtmeminitt(JJ jt){I k;
 // init tpop stack
 jt->tstackcurr=(A*)MALLOC(NTSTACK+NTSTACKBLOCK);  // save address of first allocation
 jt->malloctotal = NTSTACK+NTSTACKBLOCK;
 jt->tnextpushp = (A*)(((I)jt->tstackcurr+NTSTACKBLOCK)&(-NTSTACKBLOCK));  // get address of aligned block AFTER the first word
 *jt->tnextpushp++=0;  // blocks chain to blocks, allocations to allocations.  0 in first block indicates end.  We will never try to go past the first allo, so no chain needed
 // init all subpools to empty, setting the garbage-collection trigger points
 for(k=PMINL;k<=PLIML;++k){jt->memballo[-PMINL+k]=SBFREEB;jt->mempool[-PMINL+k]=0;}  // init so we garbage-collect after SBFREEB frees
 jt->mfreegenallo=-SBFREEB*(PLIML+1-PMINL);   // balance that with negative general allocation
#if LEAKSNIFF
 leakblock = 0;
 leaknbufs = 0;
 leakcode = 0;
#endif
 R 1;}

// Audit all memory chains to detect overrun
#if SY_64   // define optional audits.
#define AUDITFILL
#else
#define AUDITFILL
#endif
// your code for which the warning gets suppressed 
void jtauditmemchains(J jt){
#if MEMAUDIT&0x30
  I Wi,Wj;A Wx,prevWx=0; forcetomemory(&prevWx);  if((MEMAUDITPCALLENABLE)&&((MEMAUDIT&0x20)||JT(jt,peekdata))){
 for(Wi=PMINL;Wi<=PLIML;++Wi){Wj=0; Wx=(jt->mempool[-PMINL+Wi]);
 NOUNROLL while(Wx){if(Wx->origin!=THREADID1(jt)||FHRHPOOLBIN(AFHRH(Wx))!=(Wi-PMINL)AUDITFILL||Wj>0x10000000)SEGFAULT; prevWx=Wx; Wx=AFCHAIN(Wx); ++Wj;}}
}
#endif
}
// 13!:_8  check the memory free list, a la auditmemchains()
// return error info, a 2-atom list where
//  atom 0 is return code 0=OK 1=pool number corrupted 2=header corrupted 3=usecount corrupted (valid only if MEMAUDIT&0x4) 4=loop in chain 
//  atom 1 is lg of failing blocksize
// if arg is not empty, crash on any error
F1(jtcheckfreepool){F12IP;
 I Wi,Wj,ecode=0;A Wx; 
 for(Wi=PMINL;Wi<=PLIML;++Wi){  // for each free list
  Wj=0; Wx=(jt->mempool[-PMINL+Wi]);  // get head of chain, init count of # eles
  NOUNROLL while(Wx){
   if(FHRHPOOLBIN(AFHRH(Wx))!=(Wi-PMINL)){ecode=1; break;}  // will crash here if chain is corrupted
#if MEMAUDIT&4
   if(AC(Wx)!=(I)0xdeadbeefdeadbeefLL){ecode=3; break;}
#endif
   if(Wj>0x10000000){ecode=4; break;}
// debug   prevWx=Wx;
 Wx=AFCHAIN(Wx); ++Wj;  // prevwx saves previous value in case of wild pointer
  }
 }
 if(ecode&&AN(w))SEGFAULT;  // if arg not empty, crash on all errors
 R v2(ecode,ecode?Wi:0);  // return error code and chain 
}

F1(jtspcount){F12IP;A z;I c=0,i,j,*v;A x;
 ASSERTMTV(w);
 GATV0(z,INT,2*(-PMINL+PLIML+1),2); v=AV2(z);
 for(i=PMINL;i<=PLIML;++i){j=0; x=(jt->mempool[-PMINL+i]); NOUNROLL while(x){x=AFCHAIN(x); ++j;} if(j){++c; *v++=(I)1<<i; *v++=j;}}
 v=AS(z); v[0]=c; v[1]=2; AN(z)=2*c;
 RETF(z);
}    /* 7!:3 count of unused blocks */

// Garbage collector.  Called when free has decided a call is needed.
B jtspfree(J jt){I i;A p;
  // We don't check the repatq, because we always test it before coming here
  for(i = 0;i<=PLIML-PMINL;++i) {
  // Check each chain to see if it is ready to coalesce
  if(jt->memballo[i]<=0) {
   // garbage collector: coalesce blocks in chain i
   // pass through the chain, incrementing the j field in the base allo for each
   // Also create a 'proxy chain' - one element for each base block processed, not necessarily the base block (because the base block may not be free)
   US incr = FHRHBININCR(i);  // offset to add into root
   US virginbase = FHRHRESETROOT(i);  // value the root has when it is unincremented
   US offsetmask = FHRHBLOCKOFFSETMASK(i);  // mask to use for extracting offset to root
   A baseblockproxyroot = 0;  // init to empty proxy chain
   US freereqd = 0;  // indicate if any fully-freed block is found
   // after we finish the main free list, we will try the expatq
   I nexpats=IMIN;  // number of expats repatriated
   for(p=jt->mempool[i];p;){
#if MEMAUDIT&1
    if(FHRHPOOLBIN(AFHRH(p))!=i)SEGFAULT;  // make sure chains are valid
    if(ISGMP(p)&&!ACISPERM(p)&&!AZAPLOC(p))SEGFAULT; // catch an old libgmp integration failure mode
#endif
    A base = FHRHROOTADDR(p,offsetmask);   // address of base
    US baseh = AFHRH(base);  // fetch header for base
    if(baseh==virginbase) {AFPROXYCHAIN(p) = baseblockproxyroot; baseblockproxyroot = p;}  // on first encounter of base block, chain the proxy for it
    AFHRH(base) = baseh += incr;  // increment header in base & restore
    freereqd |= baseh;  // accumulate indication of freed base
    // advance to next free block, which may involve switching to the repatq
    A prevp=p;  // save end-of-chain pointer
    p=AFCHAIN(p);  // next block if any
   }
   // if any blocks can be freed, pass through the chain to remove them.
   if(FHRHISROOTALLOFREE(freereqd)) {   // if any of the base blocks were freed...
    A survivetail = (A)&jt->mempool[i];  // running pointer to last block in chain of blocks that are NOT dropped off.  Chain is rooted in jt->mempool[i], i. e. it replaces the previous chain there
      // NOTE PUN: AFCHAIN(a) must be offset 0 of a
    for(p=jt->mempool[i];p;p=AFCHAIN(p)){   // for each free block
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
#if ALIGNTOCACHE || 1   // with short headers, always align to cache bdy
     FREECHK(((I**)baseblock)[-1]);  // If aligned, the word before the block points to the original block address
     jt->malloctotal-=PSIZE+TAILPAD+ALIGNPOOLTOCACHE*CACHELINESIZE;  // return storage+bdy
     jt->mfreegenallo-=TAILPAD+ALIGNPOOLTOCACHE*CACHELINESIZE;  // only the pad is net allocation
#else
     FREECHK(baseblock);
     jt->malloctotal-=PSIZE+TAILPAD+ALIGNPOOLTOCACHE*CACHELINESIZE;  // return storage
     jt->mfreegenallo-=TAILPAD+ALIGNPOOLTOCACHE*CACHELINESIZE;  // remove pad from the amount we report allocated
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
   jt->mfreegenallo-=SBFREEB - (jt->memballo[i] & ~MFREEBCOUNTING);  // subtract diff between current mfreeb[] and what it will be set to
   jt->memballo[i] = SBFREEB + (jt->memballo[i] & MFREEBCOUNTING);  // set so we trigger rescan when we have allocated another SBFREEB bytes

   // transfer bytes freed in other threads back to the totals for this thread
   I xfct=jt->malloctotalremote; jt->malloctotal+=xfct; __atomic_fetch_sub(&jt->malloctotalremote,xfct,__ATOMIC_ACQ_REL);  // remote mods must be atomic
   xfct=jt->mfreegenalloremote; jt->mfreegenallo+=xfct; __atomic_fetch_sub(&jt->mfreegenalloremote,xfct,__ATOMIC_ACQ_REL);  // remote mods must be atomic
  }
 }
 jt->uflags.spfreeneeded = 0;  // indicate no check needed yet
// audit free list {I xxi,xxj;A xxx; {for(xxi=PMINL;xxi<=PLIML;++xxi){xxj=0; xxx=(jt->mempool[-PMINL+xxi]); while(xxx){xxx=xxx->kchain.chain; ++xxj;}}}}
 R 1;
}

// return space used by w and its descendants
static D jtspfor1(J jt, A w){D tot=0.0;
 if(unlikely(w==0))R 0.0;
 // recur on contents,  to get their total size
 switch(CTTZ(AT(w))){
 case BOXX:
  if(!ISSPARSE(AT(w))){  // I don't know why we don't account for space in sparse box
   if(!(AFLAG(w)&AFNJA)){A*wv=AAV(w);
    STACKCHKOFL    // protect against excessive box depth
    {DO(AN(w), if(wv[i])tot+=spfor1(QCWORD(wv[i])););}  // No C() for the value!  If a pyx is not ready, we don't
          // wait for it, giving it a space of 0.  If ready, we use its size.  Also, we may be processing any type
          // of internal value here, so if we did check for a pyx, we would need to check for PYX+BOX
   }
   break;
  }
 case B01X: case INTX: case FLX: case CMPXX: case LITX:  // has contents if sparse
  if(ISSPARSE(AT(w))){P*v=PAV(w); if(SPA(v,a))tot+=spfor1(SPA(v,a)); if(SPA(v,e))tot+=spfor1(SPA(v,e)); if(SPA(v,i))tot+=spfor1(SPA(v,i)); if(SPA(v,x))tot+=spfor1(SPA(v,x));} break;
 case VERBX: case ADVX:  case CONJX: 
  {V*v=FAV(w); if(v->fgh[0])tot+=spfor1(C(v->fgh[0])); if(v->fgh[1])tot+=spfor1(C(v->fgh[1])); if(v->fgh[2])tot+=spfor1(C(v->fgh[2]));} break;
 case RATX: case XNUMX:
  {A*v=AAV(w); DQ(AN(w)<<(AT(w)>>RATX), if(*v)tot+=spfor1((*v)); ++v;);} break;  // no QCWORD on XNUM/RAT
 }
 // done with contents; now get the size of w itself
 if(!ACISPERM(AC(w))) {  // permanent blocks add nothing to size
  if(AFNJA&AFLAG(w)) {  
   if(BETWEENC(AK(w),0,AM(w)))tot += SZI*WP(AT(w),AN(w),64);  // for NJA allocations with contiguous header, the size is the header size (7+64 words) plus the data size. fixed rank of 64 in NJA memory
   else{  // for NJA allocations with separate header, the size is the data size plus the size of the base block
// obsolete     tot += SZI*((1&&AT(w)&LAST0)+(((AT(w)&NAME?sizeof(NM):0)+(AN(w)<<bplg(AT(w)))+SZI-1)>>LGSZI));  // data size only
    tot += SZI*((1&&AT(w)&LAST0)+(((AN(w)<<bplg(AT(w)))+SZI-1)>>LGSZI));  // data size only.  NJA must be DIRECT type, so not NAME
    tot += alloroundsize(w);  // add in the header
   }
  } else {
  // for non-NJA allocations, just take the full size of the block
   tot += alloroundsize(w);
  }
 }
 R tot;
}

F1(jtspfor){F12IP;A*wv,x,y,z;C*s;D*zv;I i,m,n;
 ARGCHK1(w);
 n=AN(w); wv=AAV(w);
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 I zr=AR(w); GATV(z,FL,n,AR(w),AS(w)); zv=DAVn(zr,z); 
 for(i=0;i<n;++i){
  x=C(wv[i]); m=AN(x); s=CAV(x);
  ASSERT(LIT&AT(x),EVDOMAIN);
  ASSERT(1>=AR(x),EVRANK);
  ASSERT(vnm(m,s),EVILNAME);
  RZ(y=symbrd(nfs(m,s,0)));   // get value for name (protected by tpush)
  zv[i]=spfor1(y);  // get size
 }
 RETF(z);
}    /* 7!:5 space for named object; w is <'name' */

F1(jtspforloc){F12IP;A*wv,x,y,z;C*s;D tot,*zv;I i,j,m,n;L*u;LX *yv,c;
 ARGCHK1(w);
 n=AN(w); wv=AAV(w);
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 I zr=AR(w); GATV(z,FL,n,AR(w),AS(w)); zv=DAVn(zr,z);   // zv-> results
 for(i=0;i<n;++i){   // loop over each name given...
  x=C(wv[i]);  // x is the name/number
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
  for(j=SYMLINFOSIZE;j<m;++j){  // for each hashchain in the locale
   for(c=yv[j];c=SYMNEXT(c),c;c=u->next){tot+=sizeof(L); u=c+SYMORIGIN; tot+=spfor1(u->name); tot+=spfor1(QCWORD(u->fval));}  // add in the size of the name itself and the value, and the L block for the name
  }
  zv[i]=tot;
 }
 RETF(z);
}    /* 7!:6 space for a locale */


F1(jtmmaxq){F12IP;ASSERTMTV(w); RETF(sc(JT(jt,mmax)));}
     /* 9!:20 space limit query */

F1(jtmmaxs){F12IP;I j,m=MLEN,n;
 RE(n=i0(vib(w)));
 ASSERT(1E5<=n,EVLIMIT);
 j=m-1; DO(m, if(n<=(I)1<<i){j=i; break;});
 JT(jt,mmax)=(I)1<<j;
 RETF(mtm);
}    /* 9!:21 space limit set */


// Get total # bytes in use in the current thread.  That's total allocated so far, minus the bytes in the free lists and the blocks to be repatriated.
// mfreeb[] is a negative count of blocks in the free list, and biased so the value goes negative
// when garbage-collection is required.  All non-pool allocations are accounted for in mfreegenallo
// At init, each mfreeb indicates SBFREEB bytes. mfreegenallo is negative to match that total,
// indicating nothing has really been allocated; that's (PLIML-PMINL+1)*SBFREEB to begin with.  When a block
// is allocated, mfreeb[] increases; when a big block is allocated, mfreegenallo increases by the
// amount of the allocation, and mfree[-PMINL+n] decreases by the amount in all the blocks that are now
// on the free list.
// At coalescing, mfreeb is set back to indicate SBFREEB bytes, and mfreegenallo is decreased by the amount of the setback.
I jtspbytesinuse(J jt){I i,totalallo = (jt->mfreegenallo&~MFREEBCOUNTING)+jt->mfreegenalloremote;  // start with bias value
 if(jt->repatq)totalallo-=AC(jt->repatq);  // bytes awaiting gc should not be considered inuse
 for(i=PMINL;i<=PLIML;++i){totalallo+=jt->memballo[-PMINL+i]&~MFREEBCOUNTING;}  // add all the allocations
 R totalallo;
}

// called under systemlock to add up all threads
static A jtspallthreadsx(J jt){I grandtotal;
 grandtotal=0; DO(NALLTHREADS(jt), grandtotal+=jtspbytesinuse(JTFORTHREAD(jt,i));if(JTFORTHREAD(jt,i)->repato)grandtotal-=AC(JTFORTHREAD(jt,i)->repato);)  // add total for all threads, active or not
 R sc(grandtotal);
}

// 7!:8 Get total# bytes used in all threads
// Call a system lock, then get bytes in use for each thread.  Inactive threads may have bytes in use.
F1(jtspallthreads){F12IP;A z;
 ASSERTMTV(w);  // no args allowed
 do{z=jtsystemlock(jt,LOCK78MEM,jtspallthreadsx);}while(z==(A)1);  // call a lock, and repeat if another thread got the same lock request
 R z;  // return value/err from call
}


// Start tracking jt->bytesmax (and jt->bytes which we need to update it).  We indicate this by setting the LSB of EVERY entry of mfreeb
// Also count current space, and set that into jt->bytes and the result of this function
I jtspstarttracking(J jt){I i;
 for(i=PMINL;i<=PLIML;++i){jt->memballo[-PMINL+i] |= MFREEBCOUNTING;}
 jt->mfreegenallo|=MFREEBCOUNTING;  // same for non-pool alloc
 R jt->bytes = spbytesinuse();
}

// Turn off tracking.
void jtspendtracking(J jt){I i;
 for(i=PMINL;i<=PLIML;++i){jt->memballo[-PMINL+i] &= ~MFREEBCOUNTING;}
 R;
}

#if BW==64 && MEMAUDIT&2
// Make sure all deletecounts start at 0
static void auditsimverify0(J jt,A w){
 if(!w)R;
 if(AFLAG(w)>>AFAUDITUCX){
  fprintf(stderr, "auditsimverify0 w: %llx, AFLAG(w)>>AFUDITUCX: %llx, ", (UI)w, AFLAG(w)>>AFAUDITUCX);
  fprintf(stderr,"AK(w): %llx (%lli), ", AK(w), AK(w));
  fprintf(stderr,"AFLAG(w): %llx (%lli), ", AFLAG(w), AFLAG(w));
  fprintf(stderr,"AT(w): %llx (%lli), ", AT(w), AT(w));
  fprintf(stderr,"AC(w): %llx (%lli), ", AC(w), AC(w));
  fprintf(stderr,"AN(w): %llx (%lli), ", AN(w), AN(w));
  fprintf(stderr,"AR(w): %hhx (%hhu), ", AR(w), AR(w));
  fprintf(stderr,"AFHRH(w): %hx (%hi)\n", AFHRH(w), AFHRH(w));
  SEGFAULT;
 }   // hang if nonzero count
 if(ACISPERM(AC(w)))R;  // PERMANENT block may be referred to; don't touch it
 if((AFHRH(w)==0))SEGFAULT;  // pool number must be valid if not GMP block
 if(AC(w)==0 || (AC(w)<0 && AC(w)!=ACINPLACE+ACUC1 && AC(w)!=ACINPLACE+2 && AC(w)!=ACINPLACE+3))SEGFAULT;   // could go higher but doesn't in our tests
 if(AFLAG(w)&AFVIRTUAL)auditsimverify0(jt,ABACK(w));  // check backer
 if(!(AFLAG(w)&AFVIRTUAL)&&UCISRECUR(w)){  // process children
  if((AT(w)&(RAT|XNUM|BOX|SPARSE))>0) {A* v=AAV(w);  DQ(AT(w)&RAT?2*AN(w):AN(w), if(*v)auditsimverify0(jt, unlikely(AT(w)&XNUM+RAT) ?*v :CNULLNOERR(QCWORD(*v))); ++v;)}  // check descendants even if nonrecursive
  if((AT(w)&BOX+SPARSE)>0){
  }else if(AT(w)&FUNC) {V* RESTRICT v=FAV(w);
   auditsimverify0(jt,v->fgh[0]); auditsimverify0(jt,v->fgh[1]); auditsimverify0(jt,v->fgh[2]);
  }else if(AT(w)&(RAT|XNUM)) {
  }else if(AT(w)&(SYMB|NAME)) {
  }else SEGFAULT;  // inadmissible type for recursive usecount
 }
 R;
}

// Simulate tpop on the input block.  If that produces a delete count that equals the usecount,
// recur on children if any.  If it produces a delete count higher than the use count in the block, abort
static void auditsimdelete(J jt,A w){I delct;
 if(!w)R;
 if((UI)AN(w)==0xdeadbeefdeadbeef||(UI)AN(w)==0xfeeefeeefeeefeee)SEGFAULT;
 if(ACISPERM(AC(w)))R;  // PERMANENT block may be referred to; don't touch it
 if((delct = ((AFLAG(w)+=AFAUDITUC)>>AFAUDITUCX))>ACUC(w))SEGFAULT;   // hang if too many deletes
 if(AFLAG(w)&AFVIRTUAL && (AT(w)^AFLAG(w))&RECURSIBLE)SEGFAULT;   // hang if nonrecursive virtual
 if(delct==ACUC(w)&&AFLAG(w)&AFVIRTUAL){A wb = ABACK(w);
  // we fa() the backer, while we mf() the block itself.  So if the backer is NOT recursive, we have to
  // handle nonrecursive children.  All recursible types will be recursive
  if(AFLAG(w)&AFVIRTUAL && (AT(wb)^AFLAG(wb))&RECURSIBLE)SEGFAULT;  // backer must be recursive
  auditsimdelete(jt,wb);  // delete backer of virtual block, recursibly
 }
 if(delct==ACUC(w)&&!(AFLAG(w)&AFVIRTUAL)&&(UCISRECUR(w))){  // we deleted down to 0.  process children
  if((AT(w)&BOX+SPARSE)>0){
   I n=AN(w); I af=AFLAG(w);
   A* RESTRICT wv=AAV(w);  // pointer to box pointers
   I wrel = af&AFNJA?(I)w:0;  // If NJA, add wv[] to wd; othewrwise wv[] is a direct pointer
   if((af&AFNJA)||n==0)R;  // no processing if not J-managed memory (rare)
   DO(n, auditsimdelete(jt,(A)(intptr_t)((I)CNULLNOERR(QCWORD(wv[i]))+(I)wrel)););
  }else if(AT(w)&FUNC) {V* RESTRICT v=FAV(w);
   auditsimdelete(jt,v->fgh[0]); auditsimdelete(jt,v->fgh[1]); auditsimdelete(jt,v->fgh[2]);
  }else if(AT(w)&(RAT|XNUM)) {A* v=AAV(w);  DQ(AT(w)&RAT?2*AN(w):AN(w), if(*v)auditsimdelete(jt,*v); ++v;)
  }else if(AT(w)&(SYMB|NAME)) {
  }else SEGFAULT;  // inadmissible type for recursive usecount
 }
 R;
}
// clear delete counts back to 0 for next run
static void auditsimreset(J jt,A w){I delct;
 if(!w)R;
 delct = AFLAG(w)>>AFAUDITUCX;   // did this recur?
 AFLAG(w) &= AFAUDITUC-1;   // clear count for next time
 if(AFLAG(w)&AFVIRTUAL){A wb = ABACK(w);
  auditsimreset(jt,wb);  // reset backer of virtual block
  if(AT(wb)&(RAT|XNUM)) {A* v=AAV(wb);  DQ(AT(wb)&RAT?2*AN(wb):AN(wb), if(*v)auditsimreset(jt,*v); ++v;)}  // reset children
 }
 if(delct==ACUC(w)&&!(AFLAG(w)&AFVIRTUAL)&&(UCISRECUR(w))){  // if so, recursive reset
  if((AT(w)&BOX+SPARSE)>0){
   I n=AN(w); I af=AFLAG(w);
   A* RESTRICT wv=AAV(w);  // pointer to box pointers
   I wrel = af&AFNJA?(I)w:0;  // If NJA, add wv[] to wd; othewrwise wv[] is a direct pointer
   if((af&AFNJA)||n==0)R;  // no processing if not J-managed memory (rare)
   DO(n, auditsimreset(jt,(A)(intptr_t)((I)CNULLNOERR(QCWORD(wv[i]))+(I)wrel)););
  }else if(AT(w)&FUNC) {V* RESTRICT v=FAV(w);
   auditsimreset(jt,v->fgh[0]); auditsimreset(jt,v->fgh[1]); auditsimreset(jt,v->fgh[2]);
  }else if(AT(w)&(RAT|XNUM)) {A* v=AAV(w);  DQ(AT(w)&RAT?2*AN(w):AN(w), if(*v)auditsimreset(jt,*v); ++v;)
  }else if(AT(w)&(SYMB|NAME)) {
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

F1(jtleakblockread){F12IP;
#if LEAKSNIFF
if(!leakblock)R num(0);
R vec(INT,2*leaknbufs,IAV(leakblock));
#else
R num(0);
#endif
}
F1(jtleakblockreset){F12IP;
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
void audittstack(J jt){
 static B chkenabled= 0;
#if BW==64 && MEMAUDIT&2
 if(JT(jt,audittstackdisabled)&1)R;
 A *ttop;
 // verify counts start clear
 for(ttop=jt->tnextpushp-!!((I)jt->tnextpushp&(NTSTACKBLOCK-1));ttop;){
  // loop through each entry, skipping the first which is a chain
  for(;(I)ttop&(NTSTACKBLOCK-1);ttop--){
   if(*ttop)auditsimverify0(jt,*ttop);
  }
  // back up to previous block
  ttop = (A*)*ttop;  // back up to end of previous block, or 0 if last block
 }
 // loop through each block of stack
 for(ttop=jt->tnextpushp-!!((I)jt->tnextpushp&(NTSTACKBLOCK-1));ttop;){
  for(;(I)ttop&(NTSTACKBLOCK-1);ttop--){
   if(*ttop)auditsimdelete(jt,*ttop);
  }
  ttop = (A*)*ttop;  // back up to end of previous block, or 0 if last block
 }
 // again to clear the counts
 for(ttop=jt->tnextpushp-!!((I)jt->tnextpushp&(NTSTACKBLOCK-1));ttop;){
  for(;(I)ttop&(NTSTACKBLOCK-1);ttop--){
   if(*ttop)auditsimreset(jt,*ttop);
  }
  ttop = (A*)*ttop;  // back up to end of previous block, or 0 if last block
 }
#endif
}

// x 13!:_5 y  audit a recursive pyx array y 
// result 0=ok, 1=pyx found with usecount<x 2=pyx found with usecount<0 (maybe deadbeef) 3=nonrecursive block found 4=dead value found
F2(jtauditpyx){F12IP;I mindepth;
 ARGCHK2(a,w);
 RE(mindepth=i0(a));   // minimum expected depth
 I n=AN(w); // number of boxes
 ASSERT(n==0||AT(w)&BOX,EVDOMAIN)
 if(!AFLAG(w)&BOX)R num(3);
 // boxed.  Loop through each box, recurring if called for.  Two passes are intertwined in the loop
 A* RESTRICT wv=AAV(w);  // pointer to box pointers
 if(unlikely(n==0))R num(0);  // Can't be mapped boxed; skip everything if no boxes
 NOUNROLL while(--n>=0){  // n is always >0 to start.
  A np=*wv++;
  // inspect the cell.  If it is a pyx, verify that its usecount is sufficient and that its value, if given, has not been freed
  // if not a pyx, recur if it is a box
  if((np=QCWORD(np))!=0){
   if(AC(np)<0)R num(2);  // the value has been freed - error
   if(AT(np)&BOX){
    if(AT(np)&PYX){
     if(AC(np)<mindepth)R num(1);
     if(AAV(np)[0]!=0 && AC(AAV(np)[0])<0)R num(4);  // dead contents
    }else{
     A z=jtauditpyx(jt,a,np);
     if(BIV0(z)!=0)R z;  // if recursion found error, pass it up
    }
   }
  }
 }
 R num(0);
}


// Free all symbols pointed to by the SYMB block w, including PERMANENT ones.  But don't return CACHED values to the symbol pool
void freesymb(J jt, A w){I j,wn=AN(w); LX k,* RESTRICT wv=LXAV0(w);
 LX freeroot=0; L *freetailchn=(L*)((I)jt->shapesink-offsetof(L,next));  // sym index of first freed ele; addr of chain field in last freed ele
 L *jtsympv=SYMORIGIN;  // Move base of symbol block to a register.  Block 0 is the base of the free chain.  MUST NOT move the base of the free queue to a register,
  // because when we free a locale it frees its symbols here, and one of them might be a verb that contains a nested SYMB, giving recursion.  It is safe to move sympv to a register because
  // we know there will be no allocations during the free process.
 // loop through each hash chain, clearing the blocks in the chain
 I nfreed=0;  // total count of blocks accumulated into free list
 LX lastk;  // last in chain of freed blocks
 for(j=SYMLINFOSIZE;j<wn;++j){
  // free the chain; kt->last block freed
  if(k=wv[j]){
   freeroot=freeroot?freeroot:SYMNEXT(k);  // chain is nonempty; if first time, it becomes the head of the growing free chain
   do{
    k=SYMNEXT(k);
    ++nfreed;  // k is a valid free; count it
    LX nextk=jtsympv[k].next;  // unroll loop 1 time
    if(jtsympv[k].name!=0)fatype(jtsympv[k].name,NAME);jtsympv[k].name=0;  // always release name
    SYMVALFA(jtsympv[k]);    // free value
    jtsympv[k].fval=0;jtsympv[k].sn=0;jtsympv[k].flag=0;  // clear symbol fields for next time (that's Roger's way)
    lastk=k;  // remember end-of-chain
    k=nextk;  // advance to next block in chain
   }while(k);
   // chain the new chain (starting at wv[j]) to the growing free chain
   freetailchn->next=wv[j]; freetailchn=&jtsympv[lastk];  // append new chain to growing chain, remember end of growing chain
  }
 }
 if(likely(freeroot!=0)){jtsymreturn(jt,freeroot,lastk,nfreed);}  // put all blocks freed here onto the free chain
}

// free the symbol table (i. e. locale) w.  AR(w) has been loaded.  We return w so caller doesn't have to save it
NOINLINE A jtfreesymtab(J jt,A w,I arw){  // don't make this static - it will be inlined and that will make jtmf() save several more registers.  It is called infrequently
 if(likely(arw&ARLOCALTABLE)){
  // local tables have no path or name, and are not listed in any index.  Just delete the local names
  freesymb(jt,w);   // delete all the names/values
 }else if(likely(!(arw&ARINVALID))){  // INVALID means there was an error filling in the locale - don't look at LOCNAME etc
  // freeing a named/numbered locale.  The locale must have had all names freed earlier, and the path must be 0.
  // First, free the name (in the SYMLINFO block), and then free the SYMLINFO block itself
  LX k,* RESTRICT wv=LXAV0(w);
  L *jtsympv=SYMORIGIN;
  if(likely((k=wv[SYMLINFO])!=0)){  // if no error in allocation...
   // Remove the locale from its global table, depending on whether it is named or numbered
   NM *locname=NAV(LOCNAME(w));  // NM block for name
   if(likely(!(arw&ARNAMED))){
    // For numbered locale, find the locale in the list of numbered locales, wipe it out, free the locale, and decrease the number of those locales
    jterasenl(jt,LOCNUM(w));  // remove the locale from the hash table.
   } else {
    // For named locale, find the entry for this locale in the locales symbol table, and free the locale and the entry for it
    ACINIT(w,2) WRITELOCK(JT(jt,stloc)->lock) jtprobedel((J)((I)jt+locname->m),locname->s,locname->hash,JT(jt,stloc)); WRITEUNLOCK(JT(jt,stloc)->lock)   // free the L block for the locale.  Protect the locale itself so it is not freed, as we are just about to do that
   }
   // Free the name
   fr(LOCNAME(w));
   // clear the data fields in symbol SYMLINFO   kludge but this is how it was done (should be done in symnew)
   jtsympv[k].name=0;jtsympv[k].fval=0;jtsympv[k].sn=0;jtsympv[k].flag=0;
   jtsymreturn(jt,k,k,1);  // return symbol to free lists
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
// mf() frees a block.  If what is freed is a symbol table, all the symbols are freed first.

// mark w incorporated, reassigning if necessary.  Return the address of the block.  Used when w is an rvalue
A jtincorp(J jt, A w) {ARGCHK1(w); INCORP(w); R w;}

// incorp+ra.  Return 0 if error.  w must not be 0
A jtincorpra(J jt, A w) {INCORPNC(w); ra(w); R w;}

// allocate a virtual block, given the backing block
// offset is offset in atoms from start of w; r is rank
// result block is never inplaceable, recursible, virtual.  Can return 0 if allocation error
// result is PRISTINE iff the backer is
// This is inplaceable, and we inplace the w block.  'Inplaceable' here includes being the target of jt->zombieval
// We fill in everything but AN and AS, which are done in the caller
// You should be wary of calling a virtual block NJA, because with a usecount of 1 it might be inplaced by the code for x,y or x u}y
// If this code is called with inplacing turned on (* w inplaceable), we assume that w is going to be replaced by the virtual result,
// and we make the virtual block inplaceable if w was
RESTRICTF A jtvirtual(J jtip, AD *RESTRICT w, I offset, I r){AD* RESTRICT z;
 J jt=(J)(intptr_t)((I)jtip&~JTFLAGMSK);  // get flag-free pointer to J block
 ASSERT(RMAX>=r,EVLIMIT);
 I t=AT(w);  // type of input
 offset<<=bplg(t);  // length of an atom of t
 I wf=AFLAG(w);  // flags in input
 I wip=SGNIF(jtip,JTINPLACEWX)&AC(w);   // sgn if w is abandoned - this clears all but the sign bit
 // If this is an inplaceable request for an inplaceable DIRECT block, we don't need to create a new virtual block: just modify the offset in the old block.  Make sure the shape fits
 // if the block is UNINCORPABLE, we don't modify it, because then we would have to check everywhere to see if a parameter block had changed
 // We could check for zombieval etc, but it's not worth it: all we are saving is allocating one lousy block, usually 64 bytes
 if((wip & (-(t&DIRECT)) & (r-(AR(w)+1)) & SGNIFNOT(wf,AFUNINCORPABLEX))<0){
  // virtual-in-place.  There's nothing to do but change the pointer and fill in the new rank.  AN and AS are handled in the caller
  // We leave the usecount unchanged, so the block still shows as inplaceable
  AK(w)+=offset; AR(w)=(RANKT)r;
  // No change to pristinity; and anyway non-DIRECT doesn't come through here.  Inplaceability is inherited
  R w;
 }else{
  // not self-virtual block: allocate a new one
  RZ(z=gafv(SZI*(NORMAH+r)-1));  // allocate the block
  AK(z)=(CAV(w)-(C*)z)+offset;
  AFLAGINIT(z,AFVIRTUAL | (wf & ((UI)wip>>(BW-1-AFPRISTINEX))) | (t&RECURSIBLE))  // flags: recursive, not UNINCORPABLE, not NJA.  If w is inplaceable, inherit its PRISTINE status
  A wback=ABACK(w); A *wzaploc=(A*)wback; wback=wf&AFVIRTUAL?wback:w; ABACK(z)=wback;  // wzaploc is AZAPLOC(w) in case it is to be zapped
  AT(z)=t;
  ACINIT(z,wip+ACUC1)   // transfer inplaceability from original block
  ARINIT(z,(RANKT)r);
  // ra the backer.  It would be nice to transfer ownership to an abandoned block, but there is a possibility that there is a nonrecursive block N somewhere that got its data
  // from w which is higher in the stack than N.  In that case zapping w will lose the protection for N which is NOT abandoned.
  // The rule is that we can zap a result from something we called, but not a block passed in to us.
  // UNINCORPORABLEs go through here, and must be virtual so the backer, not the indirect block, is raised
  // We must also remove inplaceability from w, since it too has an alias at large
  ACIPNO(w); ra(wback);
  // It is not necessary to remove pristinity from the backer, because the backer is not inplaceable and never will be, and pristinity applies only to abandoned blocks

  // As a result of the above we can say that all backers must have recursive usecount
  R z;
 }
}  

// realize a virtual block (error if not virtual)
// allocate a new block, copy the data to it.  result is address of new block; can be 0 if allocation failure
// only non-sparse nouns can be virtual
// Mark the backing block non-PRISTINE, because realize is a form of escaping from the backer
// Might return 0
A jtrealize(J jt, A w){A z; I t;
// allocate a block of the correct type and size.  Copy the shape
 ARGCHK1(w);
 t=AT(w);
 AFLAGPRISTNO(ABACK(w))  // clear PRISTINE in the backer, since its contents are escaping
 I zr=AR(w); GA(z,t,AN(w),zr,AS(w));
 // new block is not VIRTUAL, not RECURSIBLE
// copy the contents.
 MC(AVn(zr,z),AV(w),AN(w)<<bplg(t));
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
//
// On return tnextpushp will be old or old+1, UNLESS (1) w is 0 - no guarantees then; (2) w is VIRTUALBOXED, where we know
// the value is going to be opened & we don't want to free any backers

A jtgc(J jt,A w,A* old){
 ARGCHK1(w);  // return if no input (could be error or unfilled box)
 I c=AC(w);  // remember original usecount/inplaceability
 // We want to avoid realizing w if possible, so we handle virtual w separately
 if(unlikely(AFLAG(w)&(AFVIRTUAL|AFVIRTUALBOXED))){
  if(unlikely(AFLAG(w)&AFVIRTUALBOXED))R w;  // We don't disturb VIRTUALBOXED arrays because we know they're going to be opened presently.  The backer(s) might be on the stack.
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
   ACINCRVIRT(w);  // protect w from being freed.  Its usecount may be >1.  Local because no other thread can see our virtual, and it can't be PERM
   tpop(old);  // delete everything allocated on the stack, except for w and b which were protected
   // if the block backing w has no reason to persist except as the backer for w, we delete it to avoid wasting space.  We must realize w to protect it; and we must also ra() the contents of w to protect them.
   // If there are multiple virtual blocks relying on the backer, we can't realize them all so we have to keep the backer around.
   if(unlikely(AC(b)<bc)){A origw=w; RZ(w=realize(w)); radescend(w); fa(b); mf(origw); }  // if b exists only for w, delete b.  get w out of the way.  w cannot be sparse.  Since we
                                      // raised the usecount of w only, we use mf rather than fa to free just the virtual block
                                      // fa the backer to undo the ra when the virtual block was created
   else{
    // if the backing block survives, w can continue as virtual; we must undo the increment above.  If the usecount was changed by the tpop, we don't know whether
    // there is an extant free for the block on the stack, so we must replace the stack entry.
    // Otherwise we can keep the stack entry we have, wherever it is, and we can also restore the usecount to its original value, which might
    // include inplaceability
    if((c-AC(w))&ACPERMANENT){ACSETLOCAL(w,c)  // count now is > original: the tpop didn't touch it. restore initial usecount and inplaceability
    }else{tpush1(w);}  // if the stack entry for w was removed, restore it.  This undoes the effect of incrementing the usecount
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
 // calls where w is the oldest thing on the tpush stack are not uncommon.  In that case we don't need to do ra/tpop/fa/repair-inplacing.  We can also save the repair if we KNOW w will be freed during the tpop
 // coding tip: routines that allocate multiple blocks should allocate the result first
 A *pushp=jt->tnextpushp;  // top of tstack
 if(old==pushp){if(AC(w)>=0){ra(w); tpush(w);}   // if nothing to pop: (a) if inplaceable, make no change (value must be protected up the tstack); (b) otherwise protect the value on the tstack 
 }else if(likely(ISDENSE(AT(w)))){  // sparse blocks cannot simply be left in *old because the contents are farther down the stack and would have to be protected too
  if(*old==w){   // does the start of tstack point to w?
   // w is the first element on the tstack.  
   A *old1=old+1; if(unlikely(((UI)old1&(NTSTACKBLOCK-1))==0)){A *curblk=pushp; while(((A*)*(curblk=(A*)((UI)curblk&-NTSTACKBLOCK))!=old))curblk=(A*)*curblk; old1=curblk+1;}  // old1=next element in stack (must exist)
   if(unlikely(old1==pushp))/* do nothing */;  //  If w is the ONLY element, we can stand pat; no need to make w recursive.  But why did the user bother to call us?
   else{radescend(w); tpop(old1);}   // there are other elements on tstack, we have to make w recursive (if not already) because freeing one might otherwise delete contents of w.  We can leave inplace status unchanged for w
  }else if(((UI)REPSGN(AC(w))&(UI)AZAPLOC(w))>=(UI)old && likely((((UI)old^(UI)pushp)&-NTSTACKBLOCK)==0)){  // inplaceable zaploc>=old - but that is valid only when we know pushp and old are in the same stack block
   // We can see that w is abandoned and is about to be freed.  Swap it with *old and proceed, leaving it unpopped on the stack
   radescend(w); *AZAPLOC(w)=*old; *old=w; AZAPLOC(w)=old; tpop(old+1);  // update ZAPLOC to point to new position in stack
  }else goto general;  // no applicable special case, do the ra/tpop sequence
 }else{
general:;
  // general case, w not freed or not abandoned
  ra(w);  // protect w and its descendants from tpop; also converts w to recursive usecount (unless sparse).
   // if we are turning w to recursive, this is the last pass through all of w incrementing usecounts.  All currently-on-stack pointers to blocks are compatible with the increment
   // NOTE: it wouldn't do to zap w, because AM may be invalid in WILLBEOPENED results from result.h (this could be fixed)
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
  // NOTE: certain functions (ex: rational determinant) perform operations 'in place' on non-direct values and then protect those values using gc().  The protection is
  // ineffective if the code goes through the fa() path here, because components that were modified will be freed immediately rather than later.  In those places we
  // must either use gc3() which always does the tpush, or do ACIPNO to force us through the tpush path here.  We generally use gc3().
  // Since w now has recursive usecounts (except for sparse, which is never inplaceable), we don't have to do a full fa() on a block that is returning
  // inplaceable - we just reset the usecount in the block.  If the block is returning inplaceable, we must update AM if we tpush; AM may have other uses if it is not returning inplaceable
  I cafter=AC(w); if((c&(1-cafter))>=0){A **amptr=(c<0?&AZAPLOC(w):(A**)&jt->shapesink); *amptr=jt->tnextpushp; tpush(w);}  // push unless was inplaceable and was not freed during tpop
  I *cptr=&AC(w); cptr=c<0?cptr:(I*)&jt->shapesink; *cptr=c; // make inplaceable if it was originally
 }
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
// sv is passed in so it can be returned as the result, to save a register in the caller
A jtra(AD* RESTRICT wd,I t,A sv){I n=AN(wd);
 // we use if rather than switch because the first leg is most likely and the first two legs get almost everything
 if((t&BOX+SPARSE)>0){AD* np;
  // boxed.  Loop through each box, recurring if called for.  Two passes are intertwined in the loop
  A* RESTRICT wv=AAV(wd);  // pointer to box pointers
  if(unlikely(n==0))R sv;  // Can't be mapped boxed; skip everything if no boxes
  np=*wv;  // prefetch first box
  NOUNROLL while(--n>0){AD* np0;  // n is always >0 to start.  Loop for n-1 times
   np0=*++wv;  // fetch next box address.  This fetch settles while the ra() is running
   PREFETCH((C*)np0);   // prefetch the next box while ra() is running
#if AUDITEXECRESULTS
if(np&&AC(np)<0)SEGFAULT;  // contents are never inplaceable
#endif
   if((np=QCWORD(np))!=0){racontents(np);}  // increment the box, possibly turning it to recursive.  Low bits of box addr may be enqueue flags.
     // a pyx is always recursive; we can increment the pyx's usecount here but we will never go to the contents
   np=np0;  // advance to next box
  };
  if((np=QCWORD(np))!=0){racontents(np);}  // handle last one
 } else if(t&(VERB|ADV|CONJ)){V* RESTRICT v=FAV(wd);
  // ACV.
  // If it is a nameref, clear the bucket info.  Explanation in nameref()
  if(unlikely(v->id==CTILDE))if(v->fgh[0]&&AT(v->fgh[0])&NAME)NAV(v->fgh[0])->bucket=0;
  //  Recur on each component
  DO(3, if(v->fgh[i])ra(v->fgh[i]);)  // ra f, g, h
 } else if(t&(RAT|XNUM|XD)) {A* RESTRICT v=AAV(wd);
  // single-level indirect forms.  handle each block
  DQ(t&RAT?2*n:n, if(*v)ACINCR(*v); ++v;);  // not INCRPOS, because EPILOG is used in connum to make invalid blocks recursive (kludge)
 } else if(ISSPARSE(t)){P* RESTRICT v=PAV(wd); A x;
  // all elements of sparse blocks are guaranteed non-virtual, so ra will not reassign them
  x = SPA(v,a); raonlys(x);     x = SPA(v,e); raonlys(x);     x = SPA(v,i); raonlys(x);     x = SPA(v,x); raonlys(x);
 }
 R sv;
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
static void jtfamftrav(J jt,AD* RESTRICT wd,I t){I n=AN(wd);
 if((t&BOX+SPARSE)>0){AD* np;
  // boxed.  Loop through each box, recurring if called for.
  A* RESTRICT wv=AAV(wd);  // pointer to box pointers
  if(likely(--n>=0)){  // skip everything if no boxes
   np=wv[n];  // prefetch last box.  We go last to front so we can overfetch the last box
   A wdprev=0;  // for nested boxed arrays we keep a stack of levels.  This is the previous level, initially 0
   while(1){
    NOUNROLL do{AD* np0;  // n is always >=0 to start.  Loop for n+1 times (i. e. original n times)
     np0=wv[n-1];  // fetch next box.  This fetch settles while the ra() is running
     // NOTE that we do not use C() here, so that we free pyxes as well as contents.  The usecount of the pyx will protect it until its
     // value has been installed.  Thus we ensure that fa() never causes a system lock.
     PREFETCH((C*)np0);   // prefetch the next box while ra() is running
// obsolete runout:;  // 
     // We now free virtual blocks in boxed nouns, as a step toward making it easier to return them to WILLOPEN
     if(likely((np=QCWORD(np))!=0)){  // value is 0 only if error filling boxed noun.  If the value is a parsed word, it may have low-order bit flags
      if(likely(!(AFLAG(np)&AFVIRTUAL))){
       I Zc=AC(np); I tt=AT(np);
       if(likely(((Zc-2)|tt)<0) ||  // if sparse or count<2, go to free the block
          (likely(!ACISPERM(Zc)) && unlikely(__atomic_fetch_sub(&AC(np),1,__ATOMIC_ACQ_REL)<2))){  // not permanent and count < 2
        if(tt&BOX){I nn;
         // box within box.  We can't recur because the recursion might go arbitrarily deep and overflow any reasonable stack limit.
         // Instead, we use spare space in to-be-freed np as a stack area and loop back to keep running the free loop
         if(likely((nn=AN(np))!=0)){  // if there is something to recur to...
          A *wvv=AAV(np);   // len/addr of new level
          ABACK(np)=wdprev; AKASA(np)=(A)wv; AC(np)=n;  // save n/wv on stack
          n=nn; wdprev=np; wv=wvv; np=wv[n-1];  // get new n/wv, update stack, fetch new last element
          continue;  // loop back to continue on new level
         }
        }
        jtfamf(jt,np,tt);  // free the block if it can't recur
       }   // do the recursive POP only if RECURSIBLE block; then free np
      }else{I c=AC(np);
       // virtual block.  Must be the contents of a WILLBEOPENED, but it may have other aliases so the usecount must be checked.  If this recurs deeply, it will be caught when non-virtual
       if(--c<=0){
        A b=ABACK(np); fanano0(b); mf(np);  // virtual block going away.  Check the backer.  The virtual block can never have been passed to another thread since it is contents of WILLBEOPENED
       }else ACSETLOCAL(np,c)  // virtual block survives, decrement its count
      }  // if virtual block going away, reduce usecount in backer; ignore the flagged recursiveness, just free the virt block
     }
     np=np0;  // advance to next box
    }while(--n>=0);
    do{
     if(wdprev==0)goto finbox;  // exit loop when we are back to top boxing level
     // We were in multiple boxing levels.  back up to the previous level and carry on with it
     A owdprev=wdprev;  // popping up puts us at wdprev, which we will free & continue from
     wv=(A*)AKASA(wdprev); n=AC(wdprev); wdprev=ABACK(wdprev);  // pick up where we left off.  owdprev=wv[n] is the interrupted np
     np=wv[n-1]; mf(owdprev);   // prefetch next element, free current
    }while(--n<0);   // pop up as many levels as end here
    // now we pick up with the element before the one we just deleted, which is wv[n] with n having been decremented
   }
finbox:;
// obsolete    if(n==0)goto runout;  // skip prefetch last time.  Maybe not needed.  This will alternate branch prediction except when n was 1.  Saves I1$
  }
 } else if(t&(VERB|ADV|CONJ)){V* RESTRICT v=FAV(wd);
  // ACV.
  DO(3, fana(v->fgh[i]);)  // free f, g, h
 } else if(t&NAME){A ref;   // NAMEs are usually in namerefs in verb defns and don't get freed
  if((ref=QCWORD(NAV(wd)->cachedref))!=0 && !(ACISPERM(ref))){I rc;  // reference, and not permanent, which means not to a nameless adv.  must be to a ~ reference
   // we have to free cachedref, but it is tricky because it points back to us and we will have a double-free.  So, we have to change
   // the pointer to us, which is in fgh[0].  We look at the usecount of cachedref: if it is going to go away on the next fa(), we just clear fgh[0];
   // if it is going to stick around (which means that it is part of a tacit function that got assigned to a name, or the like), we return without freeing the reference
   if(AC(ref)<=1){FAV(ref)->fgh[0]=0; rc=0;  // cachedref going away - clear the pointer to prevent refree
   }else{  // cachedref survives - modify its NM block to break the loop
    NAV(wd)->cachedref=0; ACSET(wd,1) rc=1; // clear ref to leave name only, set count so it will free when reference is freed, prevent free of wd
   }
   fa(ref);  // free, now that nm is unlooped
   if(rc)R;  // avoid free if that is called for
  }
 // SYMB must free as a monolith, with the symbols returned when the hashtables are
 }else if(t&SYMB){wd=jtfreesymtab(jt,wd,AR(wd));  // SYMB is used as a flag; we test here AFTER NAME and ADV which are lower bits
 }else if(t&(RAT|XNUM|XD)) {A* RESTRICT v=AAV(wd);
  // single-level indirect forms.  handle each block
  DQ(t&RAT?2*n:n, if(*v)fr(*v); ++v;);
 }else if(ISSPARSE(t)){P* RESTRICT v=PAV(wd);
  fana(SPA(v,a)); fana(SPA(v,e)); fana(SPA(v,i)); fana(SPA(v,x));
  // for sparse, decrement the usecount
  I c=AC(wd); if(--c>0){AC(wd)=c; R;}  // if sparse block not going away, just decr the usecount
 }
 mf(wd);  // free the block after all contents have been decremented
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
// We advance to a new stack block, and to a new allocation if necessary.  Within an NTSTACK block the NTSTACKBLOCK blocks are in ascending order
// BUT: if pushp is not within the current allocation, we do nothing.  In this case there has been a transfer of ownership and the pointers to the allocated
// blocks are being put directly into the user's block  (ex.: <"1 i. 5 5)
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
    // Unable to allocate a new block.  We have stored the address of the most recent allocation at the end of the previous block.
    // Leave pushp pointing AFTER that location: it may be unmapped memory, but that's OK, because we will always back the pointer before
    // popping the stack, and popping the stack must be the next thing we do
    jt->tnextpushp = pushp;  // set the push pointer so we can back out the last allocation
    ASSERT(0,EVWSFULL);   // fail
   }
   jt->malloctotal+=NTSTACK+NTSTACKBLOCK;  // add to total allocated
   // chain previous allocation to the new one
   *v = (A)jt->tstackcurr;   // backchain old buffers to new, including bias
   jt->tstackcurr = (A*)v;    // set new buffer as the one to use, biased so we can index it from pushx
  }
  // use the first aligned block in the allocation 
  pushp = (A*)(((I)jt->tstackcurr+NTSTACKBLOCK)&(-NTSTACKBLOCK));  // get address of aligned block AFTER the first word
 }
 // point the chain of the new block to the end of the previous
 *pushp=(A)prevpushp;  // point new block to last entry in previous block
 R pushp+1;  // Return pointer to first usable slot in the allocated block
}

// back the tpush stack up to the previous allocation.  We have just popped off the last element of the current allocation
// (that is, we have moved tnextpushp to the chain field at the start of the allocation)
// we keep one allocation in hand in tstacknext to avoid hysteresis.  If there is one already there we free it
void freetstackallo(J jt){
 if(jt->tstacknext){FREECHK(jt->tstacknext); jt->malloctotal-=NTSTACK+NTSTACKBLOCK;}   // account for malloc'd memory
 // We will set the block we are vacating as the next-to-use.  We keep only 1 such; if there is one already, free it
 jt->tstacknext=jt->tstackcurr;  // save the next-to-use, after removing bias
 jt->tstackcurr=(A*)jt->tstackcurr[0];   // back up to the previous block
}

// measureI tpopscaf[10];  // # tpops requested
// pop stack,  ending when we have freed the entry with tnextpushp==old.  tnextpushp is left pointing to an empty slot
// If the block has recursive usecount, decrement usecount in children if we free it
// stats I totalpops=0, nonnullpops=0, frees=0;
void jttpop(J jt,A *old,A *pushp){A *endingtpushp;
 // pushp points to an empty cell.  old points to the last cell to be freed.  decrement pushp to point to the cell to free (or to the chain).  decr old to match
 // if jttg failed to allocate a new block, we will have left pushp pointing to the cell after the last valid cell.  This may be in unmapped memory, but
 // that's OK, because we start by decrementing it to point to the last valid push
 // errors that could not be eformatted at once might do tpop on the way out.  We ignore these if there is a pmstack.
 if(unlikely(jt->pmstacktop!=0))R;
// measure if(EXPLICITRUNNING){I scafn=pushp-old; scafn=(UI)scafn>9?9:scafn; ++tpopscaf[scafn];}  // histo the stack size
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
    // We never tpush a PERMANENT block, but a block can become PERMANENT during the run, so we have to check
    if(likely(!ACISPERM(c))){     // if block not PERMANENT...
     I flg=AFLAG(np);  // fetch flags, just in case
     PREFETCH((C*)np0);   // prefetch the next box.  Might be 0; that's no crime
     // If count goes to 0: if the usercount is marked recursive, do the recursive fa(), otherwise just free using mf().  If virtual, the backer must be recursive, so fa() it
     // Otherwise just decrement the count
     if(c<=1||ACDECRNOPERM(np)<=1){  // avoid RFO if count is 1
// stats ++frees;
      // The block is going to be destroyed.  See if there are further ramifications
      if(!(flg&AFVIRTUAL)){fanapop(np,flg);}   // do the recursive POP only if RECURSIBLE block; then free np
      else{A b=ABACK(np); fanano0(b); mf(np);}  // if virtual block going away, reduce usecount in backer, ignore the flagged recursiveness just free the virt block
       // NOTE non-faux virtual blocks are deleted either here or in jtfamftrav() where they can be CONTENTS of boxes (presumably created in a WILLBEOPENED).  A virtual block is
       // never freed by fa() as a top-level block
       // NOTE: a sparse recursive would cause trouble, because the sparseness is not in the flag and we would have to test the type as well.  To avoid this,
       // we make sure no such block is created in sprz()
     }
    }
   }
   np=np0;  // Advance to next block
  }
  // np has the pointer before the last one we processed in this block.  pushp points to that.  See if there are more blocks to do
  if(endingtpushp!=old){      // If we haven't done them all, we must have hit start-of-block.  Move back to previous block
   // end-of-block.  np=*pushp is the chain to the end of the previous block.  We will go there, but first see if we have finished the current allocation
   // There is no way two allocations could back up so as to make the end of one exactly the beginning of the other
   if(unlikely((A*)np!=pushp-1))freetstackallo(jt);    // if the new block ends just before the current one, they are in the same allocation and we just continue.  Otherwise switch allocations
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
F1(jtrat){F12IP;ARGCHK1(w); ras(w); tpush(w); R w;}  // recursive.  w can be zero only if explicit definition had a failing sentence

A jtras(J jt, AD * RESTRICT w) { ARGCHK1(w); realizeifvirtual(w); ra(w); R w; }  // subroutine version of ra() to save space
A jtra00s(J jt, AD * RESTRICT w) { ARGCHK1(w); ra00(w,AT(w)); R w; }  // subroutine version of ra00() to save space
A jtrifvs(J jt, AD * RESTRICT w) { ARGCHK1(w); realizeifvirtual(w); R w; }  // subroutine version of rifv() to save space and be an rvalue
A jtmkwris(J jt, AD * RESTRICT w) { ARGCHK1(w); makewritable(w); R w; }  // subroutine version of makewritable() to save space and be an rvalue

#if MEMAUDIT&8
static I lfsr = 1;  // holds varying memory pattern
#endif
// call tp, but return the value passed in as z.  Used to save a register in caller
__attribute__((noinline)) A jttgz(J jt,A *tp, A z){RZ(tp=tg(tp)); jt->tnextpushp=tp; R z;}

__attribute__((noinline)) A jtgafallopool(J jt){
 A u,chn; US hrh;
#if ALIGNPOOLTOCACHE   // with smaller headers, always align pool allo to cache bdy
 // align the buffer list on a cache-line boundary
 I *v; ASSERTSUFF(v=MALLOC(PSIZE+TAILPAD+ALIGNPOOLTOCACHE*CACHELINESIZE),EVWSFULL,WRITEUNLOCK(JT(jt,dblock)); R 0;);
 A z=(A)(((I)v+CACHELINESIZE)&-CACHELINESIZE);   // get cache-aligned section
 ((I**)z)[-1] = v;   // save address of entire allocation in the word before the aligned section
#else
 // allocate without alignment
 ASSERTSUFF(av=MALLOC(PSIZE+TAILPAD),EVWSFULL,WRITEUNLOCK(JT(jt,dblock); R 0;);
#endif
 I blockx=(I)jt&63; jt=(J)((I)jt&-64);
 jt->malloctotal+=PSIZE+TAILPAD+ALIGNPOOLTOCACHE*CACHELINESIZE;  // add to total JE mem allocated
 I nt=jt->malloctotalremote+jt->malloctotal;  // get net total allocated from this thread & not freed
 jt->mfreegenallo+=PSIZE+TAILPAD+ALIGNPOOLTOCACHE*CACHELINESIZE;   // add to total from OS
 {I ot=jt->malloctotalhwmk; ot=ot>nt?ot:nt; jt->malloctotalhwmk=ot;}
 // split the allocation into blocks.  Chain them together, and flag the base.  We chain them in ascending order (the order doesn't matter), but
 // we visit them in back-to-front order so the first-allocated headers are in cache
#if PYXES
// the lock must always be cleared when the block is returned, so we can set it once.  The origin likewise doesn't change
#define MOREINIT(u) *(I4 *)&u->origin=THREADID1(jt);  // init allocating thread# and clear the lock
#else
#define MOREINIT(u)
#endif
 u=(A)((C*)z+PSIZE); chn = 0; hrh = FHRHENDVALUE(1+blockx-PMINL); I n=2L<<blockx;
#if MEMAUDIT&17
 DQ(PSIZE/2>>blockx, u=(A)((C*)u-n); AFCHAIN(u)=chn; chn=u; if(MEMAUDIT&4)AC(u)=(I)0xdeadbeefdeadbeefLL; hrh -= FHRHBININCR(1+blockx-PMINL); AFHRH(u)=hrh; MOREINIT(u));   // chain blocks to each other; set chain of last block to 0
#else
 DQ(PSIZE/2>>blockx, u=(A)((C*)u-n); AFCHAIN(u)=chn; chn=u; hrh -= FHRHBININCR(1+blockx-PMINL); AFHRH(u)=hrh; MOREINIT(u));    // chain blocks to each other; set chain of last block to 0
#endif
 AFHRH(u) = hrh|FHRHROOT;  // flag first block as root.  It has 0 offset already
 jt->mempool[-PMINL+1+blockx]=(A)((C*)u+n);  // the second block becomes the head of the free list
 if(unlikely((((jt->memballo[-PMINL+1+blockx]+=n-PSIZE)&MFREEBCOUNTING)!=0))){     // We are adding a bunch of free blocks now...
  I jtbytes=jt->bytes+=n; if(jtbytes>jt->bytesmax)jt->bytesmax=jtbytes;
 }
 A *tp=jt->tnextpushp; AZAPLOC(z)=tp; *tp++=z; jt->tnextpushp=tp; if(unlikely(((I)tp&(NTSTACKBLOCK-1))==0))RZ(z=jttgz(jt,tp,z)); // do the tpop/zaploc chaining
 R z;
}

// allocate from OS and fill in h field.  n is full size to allocate, padded for all reasons
__attribute__((noinline)) A jtgafalloos(J jt,I blockx,I n){A z;
#if ALIGNTOCACHE
 // Allocate the block, and start it on a cache-line boundary
 I *v;
 ASSERT(v=MALLOC(n),EVWSFULL)   // allocate the memory
 z=(A)(((I)v+CACHELINESIZE)&-CACHELINESIZE);   // get cache-aligned section
 ((I**)z)[-1] = v;    // save address of original allocation
#else
 ASSERTSUFF(z=MALLOC(n),EVWSFULL,WRITEUNLOCK(JT(jt,dblock)); R 0;);
#endif
 AFHRH(z) = (US)FHRHSYSJHDR(1+blockx);    // Save the size of the allocation so we know how to free it and how big it was
 if(unlikely((((jt->mfreegenallo+=n)&MFREEBCOUNTING)!=0))){
  I jtbytes=jt->bytes+=n; if(jtbytes>jt->bytesmax)jt->bytesmax=jtbytes;
 }
 jt->malloctotal+=n;  // add to our allocations
 I nt=jt->malloctotalremote+jt->malloctotal;  // get net total allocated from this thread & not freed
 {I ot=jt->malloctotalhwmk; ot=ot>nt?ot:nt; jt->malloctotalhwmk=ot;}
 A *tp=jt->tnextpushp; AZAPLOC(z)=tp; *tp++=z; jt->tnextpushp=tp; if(unlikely(((I)tp&(NTSTACKBLOCK-1))==0))RZ(z=jttgz(jt,tp,z)); // do the tpop/zaploc chaining
 MOREINIT(z);  // init allocating thread# and clear the lock
 R z;
}

#if ((MEMAUDIT&5)==5) && SY_64
#define ALLOSIZE (1024*1024)
static C allohash[ALLOSIZE];  // 0 is empty, 1 is tombstone, 2 is valid
static C * alloblocks[ALLOSIZE]; static US allolock=0; I nalloblocks=0; I allorunin=0;
static I alloring[2048]; static I alloringx=0, alloct[3]={0,0,0};
I syslockactive=0;

#define HASHBUF(buf) ((CRC32L((I)buf,~0)*ALLOSIZE)>>32)
// look up buf starting in hashslot.  Return 0 if error, 1 if OK
static NOINLINE I findbuf(I hashslot,void *buf,I probewrdel){I z;  // must have lock
 ++alloct[probewrdel];
 if(nalloblocks==0){nalloblocks=1; DO(ALLOSIZE, allohash[i]=0;) DO(sizeof(alloring)/sizeof(alloring)[0], alloring[i]=0;)}  // first time after enagagement or reengagement
 I maxstop=probewrdel&1;  // max stopper value to pause at: 0, except 1 for wr
 while(1){I stopper;
  if((stopper=allohash[hashslot])<=maxstop){  // found a stopper (tombstone or empty)
   if(unlikely((probewrdel&1)==0))R 0;  // probe or delete not found, error
   if(maxstop==1){allohash[hashslot]=2; alloblocks[hashslot]=buf; maxstop=0;}  // first wr, store value and advance to finishing wr state
   if(stopper==0)R 1;  // if we hit empty, we're finished
  }else if(allohash[hashslot]==2&&alloblocks[hashslot]==buf){
   if(probewrdel==2){  // found deletion point
    allohash[hashslot]=1;  // install tombstone
    if(allohash[(hashslot-1)&(ALLOSIZE-1)]==0){for(;allohash[hashslot]==1;hashslot=(hashslot+1)&(ALLOSIZE-1))allohash[hashslot]=0;}  // replace trailing tombstones with empty
   }
   R probewrdel!=1;  // match is failure for add, success for probe/del
  }
  hashslot=(hashslot-1)&(ALLOSIZE-1);  // advance to next slot to try
 }
}

static NOINLINE void printbufhist(void *buf){I arx=alloringx;
DQ(sizeof(alloring)/sizeof(alloring)[0], arx=(arx-1)&(sizeof(alloring)/sizeof(alloring)[0]-1);
 if((I)buf==(alloring[arx]&0x00ffffffffffffffLL))printf("pos %5lld: %16p\n",i,(void *)alloring[arx]); )
printf("\n");
}

static void addbuf(J jt,void *buf){
if(allorunin==0)R;
I hashslot=HASHBUF(buf);  // starting slot
WRITELOCK(allolock);
 if(++allorunin==0)allorunin=1; if(unlikely(JT(jt,peekdata==0)))allorunin=0;  // go into normal state after runin completes
 alloring[alloringx]=(THREADID(jt)<<56)+(I)buf; alloringx=(alloringx+1)&(sizeof(alloring)/sizeof(alloring)[0]-1);
 if(unlikely(!findbuf(hashslot,buf,1))){
  printf("allorunin=%lld: allocated %p which is already in the list\nRing history:\n",allorunin,buf);
  printbufhist(buf);
  printf("block header for block %p: 0x" FMTX " 0x" FMTX " 0x" FMTX " 0x" FMTX " 0x" FMTX " 0x" FMTX " 0x" FMTX " 0x" FMTX "\n",buf,((I*)buf)[0],((I*)buf)[1],((I*)buf)[2],((I*)buf)[3],((I*)buf)[4],((I*)buf)[5],((I*)buf)[6],((I*)buf)[7]);
  WRITEUNLOCK(allolock);
  SEGFAULT;  // error if already in list
 }
WRITEUNLOCK(allolock);
}

static void rembuf(J jt,A buf){
if(allorunin==0)R;
I hashslot=HASHBUF(buf);  // starting slot
// obsolete if((AT(buf)|AN(buf)|AFLAG(buf))&0xffffffff00000000)goto printblock;  // unfreed buf. 
WRITELOCK(allolock);
 alloring[alloringx]=((128|THREADID(jt))<<56)+(I)buf; alloringx=(alloringx+1)&(sizeof(alloring)/sizeof(alloring)[0]-1);
 if(unlikely(!findbuf(hashslot,buf,2))){
  if(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)>2)goto exit;  // ignore audit failure during system lock, which might by a symbol change eg
  if(allorunin<=0)goto exit;
  printf("allorunin=%lld: removed %p which is not in the list\nRing history:\n",allorunin,buf);
  printbufhist(buf);
  WRITEUNLOCK(allolock);
// obsolete printblock:;
  printf("block header for block %p: 0x" FMTX " 0x" FMTX " 0x" FMTX " 0x" FMTX " 0x" FMTX " 0x" FMTX " 0x" FMTX " 0x" FMTX "\n",buf,((I*)buf)[0],((I*)buf)[1],((I*)buf)[2],((I*)buf)[3],((I*)buf)[4],((I*)buf)[5],((I*)buf)[6],((I*)buf)[7]);
  SEGFAULT;
 }  // error if not in list, except during runin
exit: ;
WRITEUNLOCK(allolock);
}

void testbuf(A buf){
if(allorunin<=0)R;  // no test till initialized
if((AT(buf)|AN(buf)|AFLAG(buf))&0xffffffff00000000)goto printblock;  // unfreed buf.  no sparse here
if(AC(buf)&0x3fffffff00000000)goto printblock;  // unfreed buf
if(!(AT(buf)&NOUN))R;  // check only nouns, since ACV might be very old
if(AC(buf)&ACPERMANENT)R;  // PERMANENT is not included
if(AFLAG(buf)&AFUNINCORPABLE)R;  // PERMANENT is not included
I hashslot=HASHBUF(buf);  // starting slot
// obsolete READLOCK(allolock);
 if(unlikely(!findbuf(hashslot,buf,0))){
  if(syslockactive)R;  // ignore audit failure during system lock, which might by a symbol change eg
  printf("allorunin=%lld: testing %p which is not in the list\nRing history:\n",allorunin,buf);
  printbufhist(buf);
printblock:;
  printf("block header for block %p: 0x" FMTX " 0x" FMTX " 0x" FMTX " 0x" FMTX " 0x" FMTX " 0x" FMTX " 0x" FMTX " 0x" FMTX "\n",buf,((I*)buf)[0],((I*)buf)[1],((I*)buf)[2],((I*)buf)[3],((I*)buf)[4],((I*)buf)[5],((I*)buf)[6],((I*)buf)[7]);
  SEGFAULT;  // error if not in list
 }
// obsolete READUNLOCK(allolock);
}
#endif


// static auditmodulus = 0;
// blockx is bit# of MSB in (length-1), i. e. lg2(bufsize)-1
RESTRICTF A jtgaf(J jt,I blockx){A z;
// audit free chain I i,j;MS *x; for(i=PMINL;i<=PLIML;++i){j=0; x=(jt->mempool[-PMINL+i]); while(x){x=(MS*)(x->a); if(++j>25)break;}}  // every time, audit first 25 entries
// audit free chain if(++auditmodulus>25){auditmodulus=0; for(i=PMINL;i<=PLIML;++i){j=0; x=(jt->mempool[-PMINL+i]); while(x){x=(MS*)(x->a); ++j;}}}
// audit free chain {I xxi,xxj;A xxx; {for(xxi=PMINL;xxi<=PLIML;++xxi){xxj=0; xxx=(jt->mempool[-PMINL+xxi]); while(xxx){xxx=xxx->kchain.chain; ++xxj;}}}}
#if MEMAUDIT&16
auditmemchains();
#endif
#if MEMAUDIT&15
if((I)jt&3)SEGFAULT;
#endif
#if MEMHISTO
 jt->memhisto[blockx+1]++;  // record the request, at its size
#endif
 ASSERT(2>*JT(jt,adbreakr),EVBREAK)  // this is JBREAK0.  Fails if break pressed twice

 if(likely(blockx<PLIML)){
  // small block: allocate from pool
  z=jt->mempool[-PMINL+1+blockx];   // head of free list.  We wait till blockx is valid because an allo of 2^29 bytes could fetch out of JTT.  Rearranging could get to 2^33, not enough
  if(likely(z!=0)){         // allocate from a chain of free blocks
   jt->mempool[-PMINL+1+blockx] = AFCHAIN(z);  // remove & use the head of the free chain
   // If the user is keeping track of memory high-water mark with 7!:2, figure it out & keep track of it.  Otherwise save the cycles.  All allo routines must do this
   if(unlikely((((jt->memballo[-PMINL+1+blockx]+=(I)2<<blockx)&MFREEBCOUNTING)!=0))){
    jt->bytes += (I)2<<blockx; if(jt->bytes>jt->bytesmax)jt->bytesmax=jt->bytes;
   }
   // Put the new block into the tpop stack and point the blocks to its zappable tpop slot.  We have to check for a new tpop stack block, and we cleverly
   // pass z into that function, which will return it unchanged, so that we don't have to push the value in this routine
   // We require each other allocation routine to copy this, so that they don't need registers saved either
   A *tp=jt->tnextpushp; AZAPLOC(z)=tp; *tp++=z; jt->tnextpushp=tp; if(unlikely(((I)tp&(NTSTACKBLOCK-1))==0))RZ(z=jttgz(jt,tp,z)); // advance to next slot, allocating a new block as needed
#if MEMAUDIT&1
   if(AFCHAIN(z)&&FHRHPOOLBIN(AFHRH(AFCHAIN(z)))!=(1+blockx-PMINL))SEGFAULT;  // reference the next block to verify chain not damaged
   if(FHRHPOOLBIN(AFHRH(z))!=(1+blockx-PMINL))SEGFAULT;  // verify block has correct size
#endif
  }else{
// not worth checking   if(unlikely(lda(&jt->repatq)))if(jtrepatrecv(jt),z=jt->mempool[-PMINL+1+blockx])goto frompool; // didn't have any blocks of the right size, but managed to repatriate one
   // chain is empty, alloc PSIZE and split it into blocks
   RZ(z=jtgafallopool((J)((I)jt+blockx)));
  }
 } else {      // here for non-pool allocs...
  // add to the allocation for the fixed tail and the alignment area
  RZ(z=jtgafalloos(jt,blockx,((I)2<<blockx)+TAILPAD+ALIGNTOCACHE*CACHELINESIZE));  // ask OS for block, and fill in AFHRH.  We want to keep only jt over this call
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
#if ((MEMAUDIT&5)==5) && SY_64
if(JT(jt,peekdata))addbuf(jt,z);  // add to allocated list
#endif
 R z;
}

// bytes is total #bytes needed including headers, -1
RESTRICTF A jtgafv(J jt, I bytes){UI4 j;
#if NORMAH*(SY_64?8:4)<(1LL<<(PMINL-1))
 bytes|=(I)1<<(PMINL-1);  // if the memory header itself doesn't meet the minimum buffer length, insert a minimum
#endif
 j=CTLZI((UI)bytes);  // 3 or 4 should return 2; 5 should return 3
 ASSERT((UI)bytes<=(UI)JT(jt,mmax),EVLIMIT)
 R jtgaf(jt,(I)j);
}

#if SY_64
// fill an INDIRECT block with 0s, starting with s[0].  m is #bytes requested for allo-1
A zfillind(A w, I m){
 AS(w)[0]=0;  // the first byte by hand
 if((m=(m-64)&-32)>=0){   // get #32-byte sections - 1; if there are some to do...
  void *z=&AS(w)[1];   // point to cache-aligned result area
  __m256i wd=_mm256_setzero_si256();  // write data, all 0
  // store 128-byte sections, first one being 0, 4, 8, or 12 Is. There could be 0 to do
  // m=0-31=>1 loop, backoff -4  32-63=>1, -3  etc
  UI n2=(m>>(LGNPAR+LGSZI+2))+1;  // # turns through duff loop
  UI backoff=(m>>(LGNPAR+LGSZI))|-4;  // backoff, -4 to -1
  z=(C*)z+(backoff+1)*NPAR*SZI;
  if(likely(m<L3CACHESIZE)){
   // Normal case: not a huge copy
   switch(backoff){
   do{ ;
   case -1: _mm256_storeu_si256((__m256i*)z,wd);
   case -2: _mm256_storeu_si256((__m256i*)((C*)z+1*NPAR*SZI),wd);
   case -3: _mm256_storeu_si256((__m256i*)((C*)z+2*NPAR*SZI),wd);
   case -4: _mm256_storeu_si256((__m256i*)((C*)z+3*NPAR*SZI),wd);
   z=(C*)z+4*NPAR*SZI;
   }while(--n2>0);
   }
  }else{
   // The copy length is bigger than L3 cache: use non-temporal stores to avoid excessive cache traffic
   switch(backoff){
   do{ ;
   case -1: _mm256_stream_si256((__m256i*)z,wd);
   case -2: _mm256_stream_si256((__m256i*)((C*)z+1*NPAR*SZI),wd);
   case -3: _mm256_stream_si256((__m256i*)((C*)z+2*NPAR*SZI),wd);
   case -4: _mm256_stream_si256((__m256i*)((C*)z+3*NPAR*SZI),wd);
   z=(C*)z+4*NPAR*SZI;
   }while(--n2>0);
   }
  }
 }
 R w;
}
// stats I statsnga=0, statsngashape=0;
// like jtga, but don't copy shape.   Never called for SPARSE type
// We pack rank+type into one reg to save registers (it also helps the LIMIT test).  With this, the compiler should be able to save/restore
// only 2 regs (ranktype and bytes) but the prodigal compiler saves 3.  We accept this to be able to save AK AR AT here, so that the caller doesn't have to preserve them over the call.
// We don't store AN, because that would take another push/pop and we hope the caller needs to preserve it anyway.
RESTRICTF A jtga0(J jt,I ranktype,I atoms){A z;
 // Get the number of bytes needed-1, including the header, the atoms, and a full I appended for types that require a
 // trailing NUL (because boolean-op code needs it)
 I bytes; if(likely(ranktype&(((I)1<<(LASTNOUNX+1))-1)))bytes = ALLOBYTESVSZLG(atoms,ranktype>>32,bplg(ranktype),ranktype&LAST0,0);else bytes = ALLOBYTESVSZ(atoms,ranktype>>32,bpnonnoun(ranktype),ranktype&LAST0,0);
 ASSERT(((atoms|ranktype)>>(32+LGRMAX))==0,EVLIMIT)
    // We never use GA for NAME types, so we don't need to check for it
 RZ(z=jtgafv(jt, bytes));   // allocate the block, filling in AC AFLAG AM
 AT(z)=(I4)ranktype; I rank=(UI)ranktype>>32; ARINIT(z,rank); AK(z)=AKXR(rank);  // UI to prevent reusing the value from before the call
 // Clear data for non-DIRECT types in case of error
 // Since we allocate powers of 2, we can make the memset a multiple of 32 bytes.
 if(unlikely(!(((I4)ranktype&DIRECT)>0))){z=zfillind(z,bytes);}  // unlikely is important!  compiler strains then to use one less temp reg
// stats  ++statsnga; statsngashape+=shaape!=0;
 R z;
}
#else
RESTRICTF A jtga0(J jt,I type,I rank,I atoms){A z;
 I bytes; if(likely(type&(((I)1<<(LASTNOUNX+1))-1)))bytes = ALLOBYTESVSZLG(atoms,rank,bplg(type),type&LAST0,0);else bytes = ALLOBYTESVSZ(atoms,rank,bpnonnoun(type),type&LAST0,0);
 ASSERT(((I)bytes>(I)(atoms)&&(I)(atoms)>=(I)0)&&!((rank)&~RMAX),EVLIMIT)
 RZ(z=jtgafv(jt, bytes));   // allocate the block, filling in AC and AFLAG
 AT(z)=type; ARINIT(z,rank); AK(z)=AKXR(rank);  // UI to prevent reusing the value from before the call
 if(unlikely(!((type&DIRECT)>0))){AS(z)[0]=0; mvc((bytes-(offsetof(AD,s[1])-32))&-32,(C*)(AS(z)+1),MEMSET00LEN,MEMSET00);}
 R z;
}
#endif

// send expatriate blocks back home

// move the repato list (whose blocks are known to be from a single jt) to the repatq for that jt, and signal repat in that jt if there is enough to process
// We simply set sprepatneeded when we add to the repatq, with no interlock.  It is not vital to repat blocks immediately, and impossible anyway because they are
// held in repato for a while.  What IS vital is to flush repato and repatq when a task ends.  The task will call repatsend in its own jt and then set repatneeded
// in the jt that started the job.
// this is called only from the owning thread.  This thread owns repato but must arbitrate for repatq
void jtrepatsend(J jt){
#if PYXES
 A repato=jt->repato;
 if(!repato)R; // nothing to repatriate
 A tail=AAV0(repato)[0];  // extract tail
 I origthread1=repato->origin;
 I allocsize=AN(repato);  // extract total length in repato
 jt->repato=0;  // clear repato to empty
 jt=JTFORTHREAD1(jt,origthread1); // switch to the thread the chain must return to
  // ******* jt has changed ********
 I zero=0,exsize;
 // Add chain of new blocks to repatq.  AN(repatq) has total alloc size in repatq.  This code fails if ABA happens in repatq (the count is wrong), but that is vanishingly unlikely.  AN(repato) goes through invalid counts if there is retry
 A expval=lda(&jt->repatq); do {AFCHAIN(tail)=expval; AN(repato)=allocsize+(exsize=AN(expval?expval:ANLEN0));}while(!casa(&jt->repatq, &expval, repato));   // hang old chain off tail; atomic install at head of chain; set new total size
 if(common(((allocsize-REPATGCLIM-1)^(exsize+allocsize-REPATGCLIM-1))<0))__atomic_store_n(&jt->uflags.sprepatneeded,1,__ATOMIC_RELEASE); // if amt freed crosses boundary, request reclamation in the home thread
#endif
}

// free all the blocks on this jt's repatq, including requesting spfree if needed.  This is called only from this jt, which owns jt->mfree but must arbitrate for repatq
void jtrepatrecv(J jt){
#if PYXES
 A p=xchga(&jt->repatq,0);   // dequeue the current repatq; remember head pointer (p) and set repatq empty
 __atomic_store_n(&jt->uflags.sprepatneeded,0,__ATOMIC_RELEASE);
 if(likely(p)){  // if anything to repat here...
// obsolete   // this duplicates mf() and perhaps should just call there instead
  jt->bytes-=AN(p);  // remove repats from byte count.  Not worth testing whether counting enabled
  A nextp; do{nextp=AFCHAIN(p); AC(p)=0; mf(p);}while(p=nextp);  // send the blocks to their various queues (clear AC to avoid audit failure)
// obsolete    I blockx=FHRHPOOLBIN(AFHRH(p));   // queue number of block
// obsolete    if (unlikely((jt->memballo[blockx] -= FHRHPOOLBINSIZE(AFHRH(p))) <= 0))jt->uflags.spfreeneeded=1;  // if we have freed enough to call for garbage collection, do
// obsolete    AFCHAIN(p)=jt->mempool[blockx];  // chain new block at head of queue
// obsolete    jt->mempool[blockx]=p;
// obsolete   }
 }
#endif
}

// repatriate a single block onto the outbound queue, flushing if there is a change of owner or too much data
static void jtrepat1(J jt, A w, I allocsize){
#if PYXES
 // repatriate a block allocated in another thread.  AN(jt->repato) holds the total allocated size of the blocks in repato  AAV0(repato)[0] is the tail pointer.  The tail has no AFCHAIN pointer
 A repato=jt->repato;
 if(common(repato&&repato->origin==w->origin)){      // adding to existing repatriation queue
  allocsize+=AN(jt->repato);AN(jt->repato)=allocsize; // update allocated size
  AFCHAIN(AAV0(repato)[0])=w; AAV0(repato)[0]=w;          // add block to chain
 }else{
  if(repato)jtrepatsend(jt);                             // repatriation queue was not empty; flush it now (TODO could do better and buffer more)
  jt->repato=w; AN(w)=allocsize; AAV0(w)[0]=w;             // queue now empty regardless; install w as both head and tail
 }
 if(uncommon(allocsize>=REPATOLIM))jtrepatsend(jt);    // allocsize now has total size of repato.  if size of chain exceeded limit, flush
#endif
}

static void jtmfgmp(J jt,A w){mfgmp(w);}




#if MEMAUDIT&0x40
extern void jgmpguard(X);
#endif

// free a block.  The usecount must make it freeable.  If the block was a small block allocated in a different thread,
// repatriate it
void jtmf(J jt,A w,I hrh,I blockx){
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

// audit free list {I Wi,Wj;MS *Wx; for(Wi=PMINL;Wi<=PLIML;++Wi){Wj=0; Wx=(jt->mempool[-PMINL+Wi]); while(Wx){Wx=(MS*)(Wx->a); ++Wj;}}}
#if SHOWALLALLOC
printf("%p-\n",w);
#endif
#if MEMAUDIT&1
 if(hrh!=FHRHISGMP) {
  if((hrh==0 || blockx>(PLIML-PMINL+1)))SEGFAULT;  // pool number must be valid if not GMP block
 } else {
#if MEMAUDIT&0x40
  jgmpguard(w);
#endif
 }
#if MEMAUDIT&17
#endif
#endif
 if(FHRHBINISPOOL(hrh)){   // allocated from subpool
  I allocsize = FHRHPOOLBINTOSIZE(blockx);
#if MEMAUDIT&4
  DO((allocsize>>LGSZI), if(i!=6)((I*)w)[i] = (I)0xdeadbeefdeadbeefLL;);   // wipe the block clean before we free it - but not the reserved area
#endif
#if PYXES
  if(unlikely(w->origin!=(US)THREADID1(jt))){jtrepat1(jt,w,allocsize); R;}  // if block was allocated from a different thread, pass it back to that thread where it can be garbage collected
#endif
#if ((MEMAUDIT&5)==5) && SY_64
if(JT(jt,peekdata))rembuf(jt,w);  // remove from allocated list
#endif
  AFCHAIN(w)=jt->mempool[blockx];  // append free list to the new addition...
  jt->mempool[blockx]=w;   //  ...and make new addition the new head
  I mfreeb = jt->memballo[blockx] -= allocsize;   // number of bytes allocated at this size (biased zero point)
  if(unlikely((mfreeb&(0x80000000+MFREEBCOUNTING))!=0)){  // normally we're done
   if(mfreeb&MFREEBCOUNTING)jt->bytes-=allocsize;  // keep track of total allocation, needed only if enabled
   if(mfreeb<0)jt->uflags.spfreeneeded=1;  // Indicate we have one more free buffer if this kicks the list into garbage-collection mode, indicate that
  }
 }else if(unlikely(blockx==FHRHBINISGMP)){jtmfgmp(jt,w);  // if GMP allocation, free it through GMP
 }else{    // buffer allocated from malloc
#if ((MEMAUDIT&5)==5) && SY_64
if(JT(jt,peekdata))rembuf(jt,w);  // remove from allocated list
#endif
  I allocsize = FHRHSYSSIZE(hrh);
#if MEMAUDIT&4
  DO((MEMAUDIT&1?8:(allocsize>>LGSZI)), if(i!=6)((I*)w)[i] = (I)0xdeadbeefdeadbeefLL;);   // wipe the block clean before we free it - but not the reserved area
#endif
  allocsize+=TAILPAD+ALIGNTOCACHE*CACHELINESIZE;  // the actual allocation had a tail pad and boundary
#if PYXES
  J jtremote=JTFORTHREAD1(jt,w->origin);
  if(likely(jtremote==jt)){  // normal case of freeing in the allocating thread: avoid atomics
   jt->malloctotal-=allocsize;
   jt->mfreegenallo-=allocsize;  // account for all the bytes returned to the OS
  }else{  // the block was allocated in another thread.  Account for its free in that thread so we won't show migration of memory
   __atomic_fetch_sub(&jtremote->malloctotalremote,allocsize,__ATOMIC_ACQ_REL);
   __atomic_fetch_sub(&jtremote->mfreegenalloremote,allocsize,__ATOMIC_ACQ_REL);
  }
  if(unlikely(jtremote->mfreegenallo&MFREEBCOUNTING))__atomic_fetch_sub(&jtremote->bytes,allocsize,__ATOMIC_ACQ_REL);  // keep track of total allocation, needed only if enabled
#else
  jt->malloctotal-=allocsize;
  jt->mfreegenallo-=allocsize;  // account for all the bytes returned to the OS
  if(unlikely(jt->mfreegenallo&MFREEBCOUNTING))jt->bytes-=allocsize;  // keep track of total allocation, needed only if enabled
#endif
#if MEMAUDIT&4
 ((I*)w)[6] = (I)0xdeadbeefdeadbeefLL;   //  Reserved area in malloc blocks is not permanent
#endif

#if ALIGNTOCACHE
  FREECHK(((I**)w)[-1]);  // point to initial allocation and free it
#else
  FREECHK(w);  // free the block
#endif
 }
}

// allocate header with rank r; if w is given init z to be a surrogate of w (but do not init shape); if r==1, move the item count to be the shape also
// a header is a noninplaceable simplified virtual block, for temporary use only, that must never escape into the wild, either in full or
// as a backer for a virtual block.  It must be a noun
RESTRICTF A jtgah(J jt,I r,A w){A z;
 ASSERT(RMAX>=r,EVLIMIT); 
 RZ(z=gafv(SZI*(NORMAH+r)-1));
 AT(z)=0;
 if(w){
  AT(z)=AT(w); AN(z)=AN(w); ARINIT(z,(RANKT)r); AK(z)=CAV(w)-(C*)z; AC(z)=ACUC1;
  if(1==r)AS(z)[0]=AN(w);
 }
 R z;
}    /* allocate header */ 

// clone w, returning the address of the cloned area.  Result is NOT recursive, not AFRO, not virtual
F1(jtca){F12IP;A z;P*wp,*zp;
 ARGCHK1(w);
 I n=AN(w), t=AT(w);
 if(unlikely(ISSPARSE(t))){
  GASPARSE(z,t,n,AR(w),AS(w))
  wp=PAV(w); zp=PAV(z);
  SPB(zp,a,ca(SPA(wp,a)));
  SPB(zp,e,ca(SPA(wp,e)));
  SPB(zp,i,ca(SPA(wp,i)));
  SPB(zp,x,ca(SPA(wp,x)));
 }else{
  void *wv=t&FUNC?FAV(w):t&NAME?NAV(w):voidAV(w);  // source address
  if(t&NAME){n=NAV(w)->n;GATV(z,NAME,n,AR(w),AS(w));AT(z)=t;AC(z)=ACUC1;z->mback.lookaside=0;}  // GA does not allow NAME type, for speed.  NAME is always non-ip.  Take n from struct
  else {
   n=t&FUNC?(VERBSIZE+SZI-1)>>LGSZI:n;  // AN field of func is used for minimum rank, someday
   GA(z,t,n,AR(w),AS(w));
   AN(z)=AN(w);  // copy AN, which has its own meaning in FUNC
  }
  void *zv=voidAV(z);  // dest address
  I bpt=bp(t);  // bp needed for non-noun
  MC(zv,wv,(n*bpt)+(t&NAME?sizeof(NM):0));
  if(t&FUNC+NAME){AK(z)=AK(w); AN(z)=AN(w);}  // FUNC/NAME use AK/AN as localuse, preserve them
 }
 R z;
}
// clone block only if it is read-only
F1(jtcaro){F12IP; if(AFLAG(w)&AFRO){RETF(ca(w));} RETF(w); }

// clone virtual block, producing a new virtual block
F1(jtclonevirtual){F12IP;
 A z; RZ(z=virtual(w,0,AR(w)));  // allocate a new virtual block
 AN(z)=AN(w); MCISH(AS(z),AS(w),(I)AR(w));  // copy AN and shape; leave AC alone
 R z;
}

B jtspc(J jt){A z; RZ(z=MALLOC(1000)); FREECHK(z); R 1; }  // see if 1000 bytes are available before we embark on error display

// Double the allocation of w (twice as many atoms), then round up # items to max allowed in allocation
// if b=0 it is up to the user to manage freecounts
// if b=1, the result will replace w, so decrement usecount of w and increment usecount of new buffer.
// if b=2, the old allocation is an initial stack allocation: increment new buffer but don't free w
// the itemcount of the result is set as large as will fit evenly, and the atomcount is adjusted accordingly
// NOTE: w is not recursive, and its usecount must always be 1
// We leave AM of the block to be available to the user
A jtext(J jt,B b,A w){A z;I c,k,m,m1,t;
 ARGCHK1(w);                               /* assume AR(w)&&AN(w)    */
 m=AS(w)[0]; PROD(c,AR(w)-1,AS(w)+1);
 t=AT(w); I bpt=bp(t); k=c*bpt;
 I zr=AR(w); GA00(z,t,2*AN(w)+(AN(w)?0:c),zr);  // ensure we allocate SOMETHING to make progress
 m1=allosize(z)/k;  // start this divide before the copy
 MC(AVn(zr,z),AV(w),AN(w)*bpt);                 /* copy old contents      */
 MCISH(&AS(z)[1],&AS(w)[1],AR(w)-1);
 if(b){ACINITUNPUSH(z); mf(w);}          // 1=b iff w is permanent.  This frees up the old block but not the contents, which were transferred as is
 AS(z)[0]=m1; AN(z)=m1*c;       /* "optimal" use of space */
 if(!((t&DIRECT)>0))mvc(k*(m1-m),CAV(z)+m*k,MEMSET00LEN,MEMSET00);  // if non-DIRECT type, zero out new values to make them NULL
 R z;
}

A jtexta(J jt,I t,I r,I c,I m){A z;I m1; 
 GA00(z,t,m*c,r); 
 I k=bp(t); AS(z)[0]=m1=allosize(z)/(c*k); AN(z)=m1*c;
 if(2==r)*(1+AS(z))=c;
 if(!((t&DIRECT)>0))mvc(k*AN(z),AVn(r,z),MEMSET00LEN,MEMSET00);
 R z;
}    /* "optimal" allocation for type t rank r, c atoms per item, >=m items */

// forcetomemory does nothing, but it does take an array as argument.  This will spook the compiler out of trying to assign parts of the array to registers.
void forcetomemory(void * w){R;}
