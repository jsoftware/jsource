/* Copyright 1990-2010, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Memory Management                                                       */

#ifdef _WIN32
#include <windows.h>
#else
#define __cdecl
#endif

#include "j.h"

#define ALIGNTOCACHE 0   // set to 1 to align each block to cache-line boundary.  Doesn't seem to help much.

#define MEMJMASK 0xf   // these bits of j contain subpool #; higher bits used for computation for subpool entries
#define SBFREEBLG (14+PMINL)   // lg2(SBFREEB)
#define SBFREEB (1L<<SBFREEBLG)   // number of bytes that need to be freed before we rescan
#define MFREEBCOUNTING 1   // When this bit is set in mfreeb[], we keep track of max space usage

#if (MEMAUDIT==0 || !_WIN32)
#define FREECHK(x) FREE(x)
#else
#define FREECHK(x) if(!FREE(x))*(I*)0=0;  // crash on error
#endif

static void jttraverse(J,A,AF);

// msize[k]=2^k, for sizes up to the size of an I.  Not used in this file any more
B jtmeminit(J jt){I k,m=MLEN;
 if(jt->tstack==0){  // meminit gets called twice.  Alloc the block only once
  jt->tstack=(A*)MALLOC(NTSTACK);
  jt->tnextpushx = SZI;  // start storing at position 1 (the chain field in entry 0 is unused)
 }
 jt->mmax =(I)1<<(m-1);
 for(k=PMINL;k<=PLIML;++k){jt->mfree[-PMINL+k].ballo=SBFREEB;jt->mfree[-PMINL+k].pool=0;}  // init so we garbage-collect after SBFREEB frees
 jt->mfreegenallo=-SBFREEB*(PLIML+1-PMINL);   // balance that with negative general allocation
 R 1;
}


F1(jtspcount){A z;I c=0,i,j,*v;MS*x;
 ASSERTMTV(w);
 GATV(z,INT,2*(-PMINL+PLIML+1),2,0); v=AV(z);
 for(i=PMINL;i<=PLIML;++i){j=0; x=(MS*)(jt->mfree[-PMINL+i].pool); while(x){x=(MS*)(x->a); ++j;} if(j){++c; *v++=(I)1<<i; *v++=j;}}
 v=AS(z); v[0]=c; v[1]=2; AN(z)=2*c;
 R z;
}    /* 7!:3 count of unused blocks */

// Garbage collector.  Called when free has decided a call is needed
B jtspfree(J jt){I i;MS*p;
 for(i = PMINL;i<=PLIML;++i) {
  // Check each chain to see if it is ready to coalesce
  if(jt->mfree[-PMINL+i].ballo<=0) {
   // garbage collector: coalesce blocks in chain i
   // pass through the chain, incrementing the j field in the base allo for each
   // also, create a chain, using a spare word in the data area, with one entry for each base block that
   // appears in the scan.  To ensure base blocks are represented only once, we add to this chain only the first
   // time the count is incremented.
   // also, keep track of whether any block is incremented to the point where it can be freed
   US incr = 0x8000>>(PSIZEL-i);  // number of subbuffers=2^(PSIZEL-i); we want this number to come out 0x8000 (i. e. negative in a S)
   I freereqd = 0; MS *baseblockproxyroot = 0;  // init no full blocks, no touched blocks 
   for(p=jt->mfree[-PMINL+i].pool;p;p=(MS*)p->a){
    I incrj = ((MS *)((C*)p-(p->blkx<<i)))->j += incr;  // increment count in base
    if(incrj&0x8000){freereqd = 1;   // if the base block will be freed, note that fact
    }else if((incrj&~MEMJMASK)==incr){ ((MS**)p)[2] = baseblockproxyroot; baseblockproxyroot = p;}  // on first encounter of base block, chain the proxy for it
   }
   // if any blocks can be freed, pass through the chain to remove them.
   if(freereqd) {
    MS *survivetail = (MS *)&jt->mfree[-PMINL+i].pool;  // pointer to last block in chain of blocks that are NOT dropped off
      // NOTE PUN: ->a must be at offset 0 of the MS struct
    for(p=jt->mfree[-PMINL+i].pool;p;p=(MS*)p->a){   // for each free block
     MS *baseblock = (MS *)((C*)p-(p->blkx<<i));  // get address of corresponding base block
     if(!(baseblock->j&0x8000)){  // if block is not to be deleted...
      survivetail->a=(I*)p;survivetail=p;  // ...add it as tail of survival chain
     }
    }
    survivetail->a=0;  // terminate the chain of surviving buffers.  We leave the [].pool entry pointing to the free list
   }
   // We have kept the surviving buffers in order because the head of the free list is the most-recently-freed buffer
   // and therefore most likely to be in cache.  This would work better if we could avoid trashing the caches while we chase the chain

   // Traverse the list of base-block proxies.  There is one per base block.  If the base count is 8000, free it;
   // otherwise clear the count
   for(p=baseblockproxyroot;p;){MS *np = (((MS**)p)[2]);  // next-in-chain
    MS *baseblock = (MS *)((C*)p-(p->blkx<<i));  // get address of corresponding base block
    if(baseblock->j&0x8000){ // Free fully-unused base blocks;
#if ALIGNTOCACHE
     FREECHK(((I**)baseblock)[-1]);  // If aligned, the word before the block points to the original block address
#else
     FREECHK(baseblock);
#endif
    }else{baseblock->j &= MEMJMASK;}   // restore the count to 0 in the rest
    p=np;   //  step to next base block
   } 

   // set up for next spfree: set mfreeb to a value such that when SPFREEB bytes have been freed,
   // mfreeb will hit 0, causing a rescan.
   // Account for the buffers that were freed during the coalescing by reducing the number of PSIZEL bytes allocated
   // coalescing doesn't change the allocation, but it does change the accounting.  The change to mfreeb[] must be
   // compensated for by a change to mfreegenallo
   // This elides the step of subtracting coalesced buffers from the number of allocated buffers of size i, followed by
   // adding the bytes for those blocks to mfreebgenallo
   jt->mfreegenallo -= SBFREEB - (jt->mfree[-PMINL+i].ballo & ~MFREEBCOUNTING);  // subtract diff between current mfreeb[] and what it will be set to
   jt->mfree[-PMINL+i].ballo = SBFREEB + (jt->mfree[-PMINL+i].ballo & MFREEBCOUNTING);  // set so we trigger rescan when we have allocated another SBFREEB bytes
  }
 }
 jt->spfreeneeded = 0;  // indicate no check needed yet
 R 1;
}    /* free unused blocks */

static F1(jtspfor1){
 RZ(w);
 if(BOX&AT(w)){A*wv=AAV(w);I wd=(I)w*ARELATIVE(w); DO(AN(w), spfor1(WVR(i)););}
 else if(AT(w)&TRAVERSIBLE)traverse(w,jtspfor1); 
 if(1e9>AC(w)||AFSMM&AFLAG(w))
  if(AFNJA&AFLAG(w)){I j,m,n,p;
   m=SZI*WP(AT(w),AN(w),AR(w)); 
   n=p=m+mhb; 
   j=PMINL; n>>=j; 
   while(n){n>>=1; ++j;} 
   if(p==(I)1<<(j-1))--j;
   jt->spfor+=(I)1<<j;
  }else jt->spfor+=(I)1<<(((MS*)w-1)->j);
 R mtm;
}

F1(jtspfor){A*wv,x,y,z;C*s;D*v,*zv;I i,m,n,wd;
 RZ(w);
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w); v=&jt->spfor;
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 GATV(z,FL,n,AR(w),AS(w)); zv=DAV(z); 
 for(i=0;i<n;++i){
  x=WVR(i); m=AN(x); s=CAV(x);
  ASSERT(LIT&AT(x),EVDOMAIN);
  ASSERT(1>=AR(x),EVRANK);
  ASSERT(vnm(m,s),EVILNAME);
  RZ(y=symbrd(nfs(m,s))); 
  *v=0.0; spfor1(y); zv[i]=*v;
 }
 R z;
}    /* 7!:5 space for named object; w is <'name' */

F1(jtspforloc){A*wv,x,y,z;C*s;D*v,*zv;I c,i,j,m,n,wd,*yv;L*u;
 RZ(w);
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w); v=&jt->spfor;
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 GATV(z,FL,n,AR(w),AS(w)); zv=DAV(z); 
 for(i=0;i<n;++i){
  x=WVR(i); m=AN(x); s=CAV(x);
  if(!m){m=4; s="base";}
  ASSERT(LIT&AT(x),EVDOMAIN);
  ASSERT(1>=AR(x),EVRANK);
  ASSERT(vlocnm(m,s),EVILNAME);
  y=stfind(0,m,s);
  ASSERT(y,EVLOCALE);
  *v=(D)((I)1<<(((MS*)y-1)->j));
  spfor1(LOCPATH(y)); spfor1(LOCNAME(y));
  m=AN(y); yv=AV(y); 
  for(j=1;j<m;++j){
   c=yv[j];
   while(c){*v+=sizeof(L); u=c+jt->sympv; spfor1(u->name); spfor1(u->val); c=u->next;}
  }
  zv[i]=*v;
 }
 R z;
}    /* 7!:6 space for a locale */


F1(jtmmaxq){ASSERTMTV(w); R sc(jt->mmax);}
     /* 9!:20 space limit query */

F1(jtmmaxs){I j,m=MLEN,n;
 RE(n=i0(vib(w)));
 ASSERT(1E5<=n,EVLIMIT);
 j=m-1; DO(m, if(n<=(I)1<<i){j=i; break;});
 jt->mmax=(I)1<<j;
 R mtm;
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


// Verify that block w does not appear on tstack more than lim times
void audittstack(J jt, A w, I lim){
#if MEMAUDIT&2
 // loop through each block of stack
 A* tstack; I ttop,stackct=0;
 for(tstack=jt->tstack,ttop=jt->tnextpushx;ttop>0;){I j;
  // loop through each entry, skipping the first which is a chain
  for(j=((ttop-SZI)&(NTSTACK-1));j>0;j-=SZI){
   A stkent = *(A*)((I)tstack+j);
   if(stkent==w){
    ++stackct;   // increment number of times we have seen w
    if(stackct>lim)*(I*)0=0;  // if too many, abort
   }
   if(AC(stkent))stkent=0;  // touch the entry to catch skipped blocks
  }
  // back up to previous block
  ttop = (ttop-SZI)&-NTSTACK;  // decrement to start of block, will roll over boundary above
  tstack=(A*)*tstack; // back up to data for previous field
 }
#endif
}

static void freesymb(J jt, A w){I j,k,kt,wn=AN(w),*wv=AV(w);
 // First, free the path and name (in the SYMLINFO block), and then free the SYMLINFO block itself
 fr(LOCPATH(w));
 fr(LOCNAME(w));
 if(k=wv[SYMLINFO]){  // The LINFO block might not have been allocated
  // clear the data fields   kludge but this is how it was done (should be done in symnew)
  (jt->sympv)[k].name=0;(jt->sympv)[k].val=0;(jt->sympv)[k].sn=0;(jt->sympv)[k].flag=0;(jt->sympv)[k].prev=0;
  (jt->sympv)[k].next=jt->sympv->next;jt->sympv->next=k;  // Note: must not try to hold chainbase in a register, because this routine is recursive
 }
 for(j=SYMLINFOSIZE;j<wn;++j){
  // free the chain; kt->last block freed
  for(k=wv[j];k;k=(jt->sympv)[k].next){kt=k;fr((jt->sympv)[k].name);fa((jt->sympv)[k].val);(jt->sympv)[k].name=0;(jt->sympv)[k].val=0;(jt->sympv)[k].sn=0;(jt->sympv)[k].flag=0;(jt->sympv)[k].prev=0;}  // prev for 18!:31
  // if the chain is not empty, chain previous pool from it & make it the base of the free pool
  // if the chain is not empty, make it the base of the free pool & chain previous pool from it
  if(k=wv[j]){(jt->sympv)[kt].next=jt->sympv->next;jt->sympv->next=k;}
 }
}

static void jttraverse(J jt,A wd,AF f){
 switch(CTTZ(AT(wd))){
  case XDX:
   {DX*v=(DX*)AV(wd); DO(AN(wd), CALL1(f,v->x,0L); ++v;);} break;
  case RATX:  
   {A*v=AAV(wd); DO(2*AN(wd), CALL1(f,*v++,0L););} break;
  case XNUMX: case BOXX:
   if(!(AFLAG(wd)&AFNJA+AFSMM)){A*wv=AAV(wd);
    if(AFLAG(wd)&AFREL){DO(AN(wd), CALL1(f,WVR(i),0L););}
    else{DO(AN(wd), CALL1(f,wv[i],0L););}
   }
   break;
  case VERBX: case ADVX:  case CONJX: 
   {V*v=VAV(wd); CALL1(f,v->f,0L); CALL1(f,v->g,0L); CALL1(f,v->h,0L);} break;
  case SB01X: case SINTX: case SFLX: case SCMPXX: case SLITX: case SBOXX:
   {P*v=PAV(wd); CALL1(f,SPA(v,a),0L); CALL1(f,SPA(v,e),0L); CALL1(f,SPA(v,i),0L); CALL1(f,SPA(v,x),0L);} break;
 }
}

void jtfh(J jt,A w){fr(w);}

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

// macros copied here for reordering & common elimination
A jtgc (J jt,A w,I old){
// ra(w)
RZ(w); I* cc=&AC(w); I tt=AT(w); I c=*cc; if(tt&TRAVERSIBLE)jtra(jt,w,tt); *cc=(c+1)&~ACINPLACE;
// tpop(old)
I pushx=tpop(old);
// if block was originally inplaceable, restore it to inplaceable; if usecount was not decremented by tpop, return to avoid the tpush
if(c<0){
 I c2=*cc;  // get usecount after tpop
 *cc=c;  // restore inplaceability, if the block is inplaceable
 if(c2>ACUC1){
#if MEMAUDIT&2
  audittstack(jt,w,ACUC(w));
#endif
  R w;  // if the block was not popped, don't push it again
 }
}
// tpush(w)
*(I*)((I)jt->tstack+(pushx&(NTSTACK-1)))=(I)(w); pushx+=SZI; if(!(pushx&(NTSTACK-1))){RZ(tg(pushx)); pushx+=SZI;} if(tt&TRAVERSIBLE)RZ(pushx=jttpush(jt,w,tt,pushx)); jt->tnextpushx=pushx; if(MEMAUDIT&2)audittstack(jt,w,ACUC(w));
R w;
}


I jtgc3(J jt,A x,A y,A z,I old){
 if(x)ra(x);    if(y)ra(y);    if(z)ra(z);
 tpop(old);
 if(x)tpush(x); if(y)tpush(y); if(z)tpush(z);
 R 1;  // good return
}

I jtra(J jt,AD* RESTRICT wd,I t){I af=AFLAG(wd); I n=AN(wd);
 if(t&BOX){
  // boxed.  Loop through each box, recurring if called for.  Two passes are intertwined in the loop
  A* RESTRICT wv=AAV(wd);  // pointer to box pointers
  I wrel = af&AFREL?(I)wd:0;  // If relative, add wv[] to wd; othewrwise wv[] is a direct pointer
  if((af&AFNJA+AFSMM)||n==0)R 0;  // no processing if not J-managed memory (rare)
  while(n--){
   AD* np=(A)((I)*wv+(I)wrel); ++wv;  // point to block for the box
   if(np){    // it can be 0, if there was an error
    I tp=AT(np);  // fetch type
    AC(np)=(AC(np)+ACUC1)&~ACINPLACE;   // incr usecount
    if(tp&TRAVERSIBLE)jtra(jt,np,tp);   // recur if recursible
   }
  }
 } else if(t&(VERB|ADV|CONJ)){V* RESTRICT v=VAV(wd);
  // ACV.  Recur on each component; but this is a problem because it is done in unquote as part of executing
  // any name.  So we take advantage of the fact that all non-noun references are through names, not values; and
  // thus it is impossible to delete something that is referred to by a named ACV.  We use the recursive increment
  // only for assignments; for temporary locking of a definition, we increment the execct nonrecursively
  if(v->f)ra(v->f); if(v->g)ra(v->g); if(v->h)ra(v->h);
 } else if(t&(RAT|XNUM|XD)) {A* RESTRICT v=AAV(wd);
  // single-level indirect forms.  handle each block
  DO(t&RAT?2*n:n, if(*v)ACINCR(*v); ++v;);
 } else if(t&SPARSE){P* RESTRICT v=PAV(wd);
  if(SPA(v,a))ra(SPA(v,a)); if(SPA(v,e))ra(SPA(v,e)); if(SPA(v,i))ra(SPA(v,i)); if(SPA(v,x))ra(SPA(v,x)); 
 }
 R 1;
}


I jtfa(J jt,AD* RESTRICT wd,I t){I af=AFLAG(wd); I n=AN(wd);
 if(t&BOX){
  // boxed.  Loop through each box, recurring if called for.  Two passes are intertwined in the loop
  A* RESTRICT wv=AAV(wd);  // pointer to box pointers
  I wrel = af&AFREL?(I)wd:0;  // If relative, add wv[] to wd; othewrwise wv[] is a direct pointer
  if((af&AFNJA+AFSMM)||n==0)R 0;  // no processing if not J-managed memory (rare)
  while(n--){
   AD* np=(A)((I)*wv+(I)wrel); ++wv;   // point to block for box
   if(np){    // it could be 0 if there was error
    I tp=AT(np);  // fetch type
    I c = AC(np);  // fetch usecount
    if(tp&TRAVERSIBLE)jtfa(jt,np,tp);  // recur before we free this block
    if(--c<=0)mf(np);else AC(np)=c;  // decrement usecount; free if it goes to 0; otherwise store decremented count
   }
  }
 } else if(t&(VERB|ADV|CONJ)){V* RESTRICT v=VAV(wd);
  // ACV.  We look at execct to see if this name is in execution; if so, just decrement the execct and wait till
  // the executions finish to recursively decrement.  Because of the way we implement fa(), where we decrement the
  // count in a static variable before calling this routine, we had to increment the usecount at the same time
  // we increment execct.
  if(v->execct){--v->execct;}else{if(v->f)fa(v->f); if(v->g)fa(v->g); if(v->h)fa(v->h);}
 } else if(t&(RAT|XNUM|XD)) {A* RESTRICT v=AAV(wd);
  // single-level indirect forms.  handle each block
  DO(t&RAT?2*n:n, if(*v)fr(*v); ++v;);
 } else if(t&SPARSE){P* RESTRICT v=PAV(wd);
  if(SPA(v,a))fa(SPA(v,a)); if(SPA(v,e))fa(SPA(v,e)); if(SPA(v,i))fa(SPA(v,i)); if(SPA(v,x))fa(SPA(v,x)); 
 }
 R 1;
}

// subroutine to save space, just like tpush macro
static I subrtpush(J jt, A wd, I pushx){
I tt=AT(wd); *(I*)((I)jt->tstack+(pushx&(NTSTACK-1)))=(I)wd; pushx+=SZI; if(!(pushx&(NTSTACK-1))){RZ(tg(pushx)); pushx+=SZI;} if(tt&TRAVERSIBLE)pushx=jttpush(jt,wd,tt,pushx);  if(MEMAUDIT&2){jt->tnextpushx = pushx; (jt,wd,ACUC(wd));}R pushx;
}

// Result is new value of jt->tnextpushx, or 0 if error
I jttpush(J jt,AD* RESTRICT wd,I t,I pushx){I af=AFLAG(wd); I n=AN(wd);
 if(t&BOX){
  // boxed.  Loop through each box, recurring if called for.  Two passes are intertwined in the loop
  A* RESTRICT wv=AAV(wd);  // pointer to box pointers
  A* tstack=jt->tstack;  // base of current output block
  I wrel = af&AFREL?(I)wd:0;  // If relative, add wv[] to wd; othewrwise wv[] is a direct pointer
  if((af&AFNJA+AFSMM)||n==0)R pushx;  // no processing if not J-managed memory (rare)
  while(n--){
   A np=(A)((I)*wv+(I)wrel); ++wv;   // point to block for box
   if(np){     // it can be 0 if there was error
    I tp=AT(np);  // fetch type
    *(A*)((I)tstack+(pushx&(NTSTACK-1)))=np;  // put the box on the stack
    pushx += SZI;  // advance to next output slot
    if(!(pushx&(NTSTACK-1))){
     // pushx has crossed the block boundary.  Allocate a new block.
     RZ(tstack=tg(pushx)); pushx+=SZI;   // If error, abort with values set; if not, step pushx over the chain field
    } // if the buffer ran out, allocate another, save its address
#if MEMAUDIT&2
    jt->tnextpushx=pushx;
    audittstack(jt,np,ACUC(np));
#endif
    if(tp&TRAVERSIBLE){RZ(pushx=jttpush(jt,np,tp,pushx)); tstack=jt->tstack;}  // recur, and restore stack pointers after recursion
   }
  }

 } else if(t&(VERB|ADV|CONJ)){V* RESTRICT v=VAV(wd);
  // ACV.  Recur on each component; but this is a problem because it is done in unquote as part of executing
  // any name.  So we take advantage of the fact that all non-noun references are through names, not values; and
  // thus it is impossible to delete something that is referred to by a named ACV.  The ACV becomes a non-recursive
  // usecount, with a separate count of the number of assignments that have been made.  When this count increments to
  // 1 or decrements to 0, we propagate the change to descendants, but not otherwise
  if(v->f)RZ(pushx=subrtpush(jt,v->f,pushx)); if(v->g)RZ(pushx=subrtpush(jt,v->g,pushx)); if(v->h)RZ(pushx=subrtpush(jt,v->h,pushx));
 } else if(t&(RAT|XNUM|XD)) {A* RESTRICT v=AAV(wd);
  // single-level indirect forms.  handle each block
  DO(t&RAT?2*n:n, if(*v)RZ(pushx=subrtpush(jt,*v,pushx)); ++v;);
 } else if(t&SPARSE){P* RESTRICT v=PAV(wd);
  if(SPA(v,a))RZ(pushx=subrtpush(jt,SPA(v,a),pushx)); if(SPA(v,e))RZ(pushx=subrtpush(jt,SPA(v,e),pushx)); if(SPA(v,i))RZ(pushx=subrtpush(jt,SPA(v,i),pushx)); if(SPA(v,x))RZ(pushx=subrtpush(jt,SPA(v,x),pushx)); 
 }
 R pushx;
}

// Result is address of new stack block
A* jttg(J jt, I pushx){     // Filling last slot; must allocate next page.  Caller is responsible for advancing pushx
 if(jt->tstacknext) {   // if we already have a page to move to
//  jt->tstacknext[0] = jt->tstack;   // next was chained to prev before it was saved as next
  jt->tstack = jt->tstacknext;   // set new buffer as current
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
  *v = (A)jt->tstack;   // backchain old buffers to new
  jt->tstack = v;    // set new buffer as the one to use
 }
 R jt->tstack;  // Return base address of block
}


// pop stack,  ending when we have freed the entry with tnextpushx==old.  tnextpushx is left pointing to an empty slot
// return value is pushx
I jttpop(J jt,I old){I pushx=jt->tnextpushx; I endingtpushx;
 if(old>=pushx)R pushx;  // return fast if nothing to do
 while(1) {  // loop till end.  Return is at bottom of loop
  endingtpushx = MAX(old,SZI+((pushx-SZI)&-NTSTACK));  // Get # of frees we can perform in this tstack block
  I nfrees=(A*)pushx-(A*)endingtpushx;
  A* RESTRICT fp = (A*)((I)jt->tstack+((pushx-SZI)&(NTSTACK-1)));  // point to first slot to free, possibly rolling to end of block
  while(nfrees--){
   A np=*fp--;   // point to block to be freed
   I c=AC(np);  // fetch usecount
#if MEMAUDIT&2
   jt->tnextpushx -= SZI;  // remove the buffer-to-be-freed from the stack for auditing
#endif
   if(--c<=0)mf(np);else AC(np)=c;  // decrement usecount and either store it back or free the block
  }
  // See if there are more blocks to do
  if(endingtpushx>old){      // If we haven't done them all, we must have hit start-of-block.  Move back to previous block
   if(jt->tstacknext)FREECHK(jt->tstacknext);   // We will set the block we are vacating as the next-to-use.  We can have only 1 such; if there is one already, free it
   jt->tstacknext=jt->tstack;  // save the next-to-use
   jt->tstack=(A*)jt->tstack[0];   // back up to the previous block, leaving tstacknext pointing to tstack
   // move the start pointer forward; past old, if this is the last pass
   pushx=endingtpushx-SZI;  // back up to slot 0, so when the next starting address is calculated, it goes all the way back to beginning of block
#if MEMAUDIT&2
   jt->tnextpushx -= SZI;  // skip the chain field on the stack for auditing
#endif
   // The return point:
  } else R jt->tnextpushx=endingtpushx;  // On last time through, update starting pointer for next push, and return that value
 }
}



// Add jt->arg to the usecount of w and all its descendants.
static F1(jtra1){RZ(w); if(AT(w)&TRAVERSIBLE)traverse(w,jtra1); ACINCRBY(w,jt->arg); R w;}
// Add k to the usecount of w and all its descendants
A jtraa(J jt,I k,A w){A z;I m=jt->arg; jt->arg=k; z=ra1(w); jt->arg=m; R z;}  // preserve jt->arg; return w

// Protect a value temporarily
// w is a block that we want to make ineligible for inplacing.  We increment its usecount (which protects it) and tpush it (which
// undoes the incrementing after we are gone).  The protection lasts until the tpop for the stack level in effect at the call to here.
// Protection is needed only for names, for example in samenoun =: (samenoun verb verb) samenoun  where we must make sure
// the noun is not operated on inplace lest it destroy the value stored in the fork, which might be saved in an explicit definition.
// If the noun is assigned as part of a named derived verb, protection is not needed (but harmless) because if the same value is
// assigned to another name, the usecount will be >1 and therefore not inplaceable.  Likewise, the the noun is non-DIRECT we need
// only protect the top level, because if the named value is incorporated at a lower level its usecount must be >1.
F1(jtrat){RZ(w); ra(w); tpush(w); R w;}  // recursive.  w can be zero only if explicit definition had a failing sentence
F1(jtrat1s){rat1(w); R w;}   // top level only.  Subroutine version to save code space

#if MEMAUDIT&8
static I lfsr = 1;  // holds varying memory pattern
#endif

// static auditmodulus = 0;
RESTRICTF A jtgaf(J jt,I blockx){A z;MS *av;I mfreeb;I n = (I)1<<blockx;
// audit free chain I i,j;MS *x; for(i=PMINL;i<=PLIML;++i){j=0; x=(jt->mfree[-PMINL+i].pool); while(x){x=(MS*)(x->a); if(++j>25)break;}}  // every time, audit first 25 entries
// audit free chain if(++auditmodulus>25){auditmodulus=0; for(i=PMINL;i<=PLIML;++i){j=0; x=(jt->mfree[-PMINL+i].pool); while(x){x=(MS*)(x->a); ++j;}}}
// use 6!:5 to start audit I i,j;MS *x; if(jt->peekdata){for(i=PMINL;i<=PLIML;++i){j=0; x=(jt->mfree[-PMINL+i].pool); while(x){x=(MS*)(x->a); ++j;}}}
// unconditional {I Wi,Wj;MS *Wx; for(Wi=PMINL;Wi<=PLIML;++Wi){Wj=0; Wx=(jt->mfree[-PMINL+Wi].pool); while(Wx){Wx=(MS*)(Wx->a); ++Wj;}}}

 I pushx=jt->tnextpushx;  // start reads for tpush
 A* tstack=jt->tstack;
 if(blockx>PLIML){             /* large block: straight malloc    */
  mfreeb=jt->mfreegenallo;    // bytes in large allocations
  JBREAK0;  // See if we are interrupted
#if ALIGNTOCACHE
  // Allocate the block, and start it on a cache-line boundary
  I *v;
  ASSERT(v=MALLOC(n+CACHELINESIZE),EVWSFULL);
  av=(MS *)(((I)v+CACHELINESIZE)&-CACHELINESIZE);   // get cache-aligned section
  av->a = (I*)v;    // save address of original allocation
#else
  // Allocate without alignment
  ASSERT(av=MALLOC(n),EVWSFULL);
#if MEMAUDIT&1
  av->a=(I*)0xdeadbeefdeadbeefLL;  // flag block as allocated (only if a not used for base pointer)
#endif
#endif
  av->j = (US)blockx;    // Save the size of the allocation so we know how to free it and how big it was
  jt->mfreegenallo=mfreeb+=n;    // mfreegenallo is the byte count allocated for large blocks
 } else {
  av=jt->mfree[-PMINL+blockx].pool;   // tentatively use head of free list as result
  mfreeb=jt->mfree[-PMINL+blockx].ballo; // bytes in pool allocations
  JBREAK0;  // See if we are interrupted
  if(av){         // allocate from a chain of free blocks
   jt->mfree[-PMINL+blockx].pool = (MS *)av->a;  // remove & use the head of the free chain
#if MEMAUDIT&1
   if(av->j!=blockx)*(I*)0=0;  // verify block has correct size
#endif
  }else{MS *x;C* u;I nblocks=PSIZE>>blockx;                    // small block, but chain is empty.  Alloc PSIZE and split it into blocks
#if ALIGNTOCACHE
   // align the buffer list on a cache-line boundary
   I *v;
   ASSERT(v=MALLOC(PSIZE+CACHELINESIZE),EVWSFULL);
   av=(MS *)(((I)v+CACHELINESIZE)&-CACHELINESIZE);   // get cache-aligned section
   ((I**)av)[-1] = (I*)v;   // save address of entire allocation in the word before the aligned section
#else
   // allocate without alignment
   ASSERT(av=MALLOC(PSIZE),EVWSFULL);
#endif
   u=(C*)av; DO(nblocks, x=(MS*)u; u+=n; x->a=(I*)u; x->j=(US)blockx; x->blkx=(US)i;); x->a=0;  // chain blocks to each other; set chain of last block to 0
   jt->mfree[-PMINL+blockx].pool=(MS*)((C*)av+n);  // the second block becomes the head of the free list
   mfreeb-=PSIZE;     // We are adding a bunch of free blocks now...
   jt->mfreegenallo+=PSIZE;   // ...add them to the total bytes allocated
  }
#if MEMAUDIT&1
  av->a=(I*)0xdeadbeefdeadbeefLL;  // flag block as allocated (only if a not used for base pointer)
#endif
  jt->mfree[-PMINL+blockx].ballo=mfreeb+=n;
 }
 z=(A)&av[1];  // advance past the memory header
 // If the user is keeping track of memory high-water mark with 7!:2, figure it out & keep track of it
 if(mfreeb&MFREEBCOUNTING){jt->bytes += n; if(jt->bytes>jt->bytesmax)jt->bytesmax=jt->bytes;}
#if MEMAUDIT&2
 audittstack(jt,z,0);  // verify buffer not on stack
#endif
#if MEMAUDIT&8
 DO((1<<(blockx-LGSZI))-2, lfsr = (lfsr<<1) ^ (lfsr<0?0x1b:0); ((I*)z)[i] = lfsr;);   // fill block with garbage
#endif
 AFLAG(z)=0; AC(z)=ACUC1|ACINPLACE;  // all blocks are born inplaceable 
 *(I*)((I)tstack+(pushx&(NTSTACK-1)))=(I)z; pushx+=SZI; if(!(pushx&(NTSTACK-1))){RZ(tg(pushx)); pushx+=SZI;}
 jt->tnextpushx=pushx;
 R z;
}

RESTRICTF A jtgafv(J jt, I bytes){I j=PMINL, n=PMIN; 
 ASSERT((UI)bytes<=(UI)jt->mmax,EVLIMIT);
 while(n<bytes)n<<=1, ++j;
 R jtgaf(jt,j);
}

RESTRICTF A jtga(J jt,I type,I atoms,I rank,I* shaape){A z;
 // Get the number of bytes needed, including the header, the atoms, and a full I appended for types that require a
 // trailing NUL (because boolean-op code needs it)
 I bytes = ALLOBYTESVSZ(atoms,rank,bp(type),type&LAST0,0);  // We never use GA for NAME types, so we don't need to check for it
#if SY_64
 ASSERT((UI)atoms<TOOMANYATOMS,EVLIMIT); // check for too many atoms, to preempt overflow
#else
 ASSERT(bytes>atoms&&atoms>=0,EVLIMIT);   /* beware integer overflow */
#endif
 RZ(z = jtgafv(jt, bytes));   // allocate the block, filling in AC and AFLAG
 I akx=AKXR(rank);   // Get offset to data
 if(!(type&DIRECT))memset((C*)z+akx,C0,bytes-mhb-akx);  // For indirect types, zero the data area.  Needed in case an indirect array has an error before it is valid
   // All non-DIRECT types have items that are multiples of I, so no need to round the length
 else if(type&LAST0){((I*)((C*)z+((bytes-SZI-mhb)&(-SZI))))[0]=0;}  // We allocated a full SZI for the trailing NUL, because the
    // code for boolean verbs needs it.  But we don't need to set more than just the word containing the trailing NUL (really, just the byte would be OK).
    // To find that byte, back out the SZI added nulls 
 AK(z)=akx; AT(z)=type; AN(z)=atoms;   // Fill in AK, AT, AN
 // Set rank, and shape if user gives it.  This might leave the shape unset, but that's OK
 AR(z)=rank;   // Storing the extra last I (as was done originally) might wipe out rank, so defer storing rank till here
 if(1==rank&&!(type&SPARSE))*AS(z)=atoms; else if(shaape&&rank){AS(z)[0]=((I*)shaape)[0]; DO(rank-1, AS(z)[i+1]=((I*)shaape)[i+1];)}  /* 1==atoms always if t&SPARSE  */  // copy shape by hand since short
 AM(z)=((I)1<<((MS*)z-1)->j)-mhb-akx;   // get rid of this
 R z;
}

// free a block.  The usecount must make it freeable
void jtmf(J jt,A w){I mfreeb;
// unconditional {I Wi,Wj;MS *Wx; for(Wi=PMINL;Wi<=PLIML;++Wi){Wj=0; Wx=(jt->mfree[-PMINL+Wi].pool); while(Wx){Wx=(MS*)(Wx->a); ++Wj;}}}
 I blockx=((MS*)w)[-1].j;   // lg(buffer size)
 I n=1LL<<blockx;   // number of bytes in the allocation
#if MEMAUDIT&2
 audittstack(jt,w,0);  // must not free anything on the stack
#endif
 // SYMB must free as a monolith, with the symbols returned when the hashtables are
 if(AT(w)==SYMB){
  freesymb(jt,w);
 }
#if MEMAUDIT&1
 if(blockx<PMINL||blockx>=BW)*(I*)0=0;  // pool number must be valid
#endif
#if MEMAUDIT&4
 DO((1<<(blockx-LGSZI))-2, ((I*)w)[i] = (I)0xdeadbeefdeadbeefLL;);   // wipe the block clean before we free it
#endif
 if(PLIML+1<=blockx){   // allocated by malloc
  mfreeb = jt->mfreegenallo;
#if ALIGNTOCACHE
  FREECHK(((MS*)w)[-1].a);  // point to initial allocation and free it
#else
#if MEMAUDIT&1
  if(((MS*)w)[-1].a!=(I*)0xdeadbeefdeadbeefLL)*(I*)0=0;  // a field is set in pool allocs if not cache-aligned
#endif
  FREECHK((MS*)w-1);  // point to initial allocation and free it
#endif
  jt->mfreegenallo = mfreeb-n;
 }else{                // buffer allocated from subpool.
#if MEMAUDIT&1
  if(((MS*)w)[-1].a!=(I*)0xdeadbeefdeadbeefLL)*(I*)0=0;  // a field is set in pool allocs
#endif
  mfreeb = jt->mfree[-PMINL+blockx].ballo;   // number of bytes allocated at this size (biased zero point)
  ((MS*)w)[-1].a=(I*)jt->mfree[-PMINL+blockx].pool;  // append free list to the new addition...
  jt->mfree[-PMINL+blockx].pool=((MS*)w-1);   //  ...and make new addition the new head
  if(0 > (mfreeb-=n))jt->spfreeneeded=1;  // Indicate we have one more free buffer;
   // if this kicks the list into garbage-collection mode, indicate that
  jt->mfree[-PMINL+blockx].ballo=mfreeb;
 }
 if(mfreeb&MFREEBCOUNTING){jt->bytes -= n;}  // keep track of total allocation only if asked to
}


RESTRICTF A jtgah(J jt,I r,A w){A z;
 ASSERT(RMAX>=r,EVLIMIT); 
 RZ(z=gafv(SZI*(AH+r)+mhb));
 AT(z)=0;
 if(w){
  AFLAG(z)=0; AM(z)=AM(w); AT(z)=AT(w); AN(z)=AN(w); AR(z)=r; AK(z)=CAV(w)-(C*)z;
  if(1==r)*AS(z)=AN(w);
 }
 R z;
}    /* allocate header */ 

// clone w, returning the address of the cloned area
F1(jtca){A z;I t;P*wp,*zp;
 RZ(w);
 t=AT(w);
 if(t&NAME){GATV(z,NAME,AN(w),AR(w),AS(w));AT(z)=t;}  // GA does not allow NAME type, for speed
 else{GA(z,t,AN(w),AR(w),AS(w));}
 if(AFLAG(w)&AFNJA+AFSMM+AFREL)AFLAG(z)=AFREL;
 if(t&SPARSE){
  wp=PAV(w); zp=PAV(z);
  SPB(zp,a,ca(SPA(wp,a)));
  SPB(zp,e,ca(SPA(wp,e)));
  SPB(zp,i,ca(SPA(wp,i)));
  SPB(zp,x,ca(SPA(wp,x)));
 }else MC(AV(z),AV(w),AN(w)*bp(t)+(t&NAME?sizeof(NM):0)); 
 R z;
}

F1(jtcar){A*u,*wv,z;I n,wd;P*p;V*v;
 RZ(z=ca(w));
 n=AN(w);
 switch(CTTZ(AT(w))){
  case RATX:  n+=n;
  case XNUMX:
  case BOXX:  u=AAV(z); wv=AAV(w); wd=(I)w*ARELATIVE(w); DO(n, RZ(*u++=car(WVR(i)));); break;
  case SB01X: case SLITX: case SINTX: case SFLX: case SCMPXX: case SBOXX:
   p=PAV(z); 
   SPB(p,a,car(SPA(p,a)));
   SPB(p,e,car(SPA(p,e)));
   SPB(p,i,car(SPA(p,i)));
   SPB(p,x,car(SPA(p,x)));
   break;
  case VERBX: case ADVX: case CONJX: 
   v=VAV(z); 
   if(v->f)RZ(v->f=car(v->f)); 
   if(v->g)RZ(v->g=car(v->g)); 
   if(v->h)RZ(v->h=car(v->h));
 }
 R z;
}

B jtspc(J jt){A z; RZ(z=MALLOC(1000)); FREECHK(z); R 1; }

// Double the allocation of w (twice as many atoms), then round up # items to max allowed in allocation
// if b=1, the result will replace w, so decrement usecount of w and increment usecount of new buffer
A jtext(J jt,B b,A w){A z;I c,k,m,m1,t;
 RZ(w);                               /* assume AR(w)&&AN(w)    */
 m=*AS(w); c=AN(w)/m; t=AT(w); k=c*bp(t);
 GA(z,t,2*AN(w),AR(w),AS(w)); 
 MC(AV(z),AV(w),m*k);                 /* copy old contents      */
 if(b){ra(z); fa(w);}                 /* 1=b iff w is permanent */
 *AS(z)=m1=AM(z)/k; AN(z)=m1*c;       /* "optimal" use of space */
 if(!(t&DIRECT))memset(CAV(z)+m*k,C0,k*(m1-m));  // if non-DIRECT type, zero out new values to make them NULL
 R z;
}

A jtexta(J jt,I t,I r,I c,I m){A z;I k,m1; 
 GA(z,t,m*c,r,0); 
 k=bp(t); *AS(z)=m1=AM(z)/(c*k); AN(z)=m1*c;
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
