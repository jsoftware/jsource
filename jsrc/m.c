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

#define MEMAUDIT 0   // Audit level: 0=fastest, 1=buffer checks but not tstack 2=buffer+tstack 3 +scrub freed areas

#define PSIZE       65536L         /* size of each pool                    */
#define PSIZEL      16L            // lg(PSIZE)
#define PLIM        1024L          /* pool allocation for blocks <= PLIM   */
#define PLIML       10L            /* base 2 log of PLIM                   */

#define TOOMANYATOMS 0x01000000000000LL  // more atoms than this is considered overflow

static void jttraverse(J,A,AF);

// msize[k]=2^k, for sizes up to the size of an I.  Not used in this file any more
B jtmeminit(J jt){I k,m=MLEN;
 k=1; DO(m, msize[i]=k; k+=k;);  /* OK to do this line in each thread */
 jt->tbase=-NTSTACK;
 jt->ttop = NTSTACK;
 jt->mmax =msize[m-1];
 DO(m, jt->mfree[i]=0; jt->mfreeb[i]=0; jt->mfreet[i]=1048576;);
 R 1;
}


F1(jtspcount){A z;I c=0,j,m=1+PLIML,*v;MS*x;
 ASSERTMTV(w);
 GA(z,INT,2*m,2,0); v=AV(z);
 DO(m, j=0; x=(MS*)(jt->mfree[i]); while(x){x=(MS*)(x->a); ++j;} if(j){++c; *v++=msize[i]; *v++=j;});
 v=AS(z); v[0]=c; v[1]=2; AN(z)=2*c;
 R z;
}    /* 7!:3 count of unused blocks */

static int __cdecl spfreecomp(const void *x,const void *y){R *(I*)x<*(I*)y?-1:1;}

B jtspfree(J jt){A t;I c,d,i,j,m,n,*u,*v;MS*x;
 m=0; u=5+jt->mfreet; v=5+jt->mfreeb;
 /* DO(1+PLIML, if(jt->mfreet[i]<=jt->mfreeb[i]){j=jt->mfreeb[i]/msize[i]; m=MAX(m,j);}); */
 if(*++u<=*++v){j=*v/  64; m=MAX(m,j);}
 if(*++u<=*++v){j=*v/ 128; m=MAX(m,j);}
 if(*++u<=*++v){j=*v/ 256; m=MAX(m,j);}
 if(*++u<=*++v){j=*v/ 512; m=MAX(m,j);}
 if(*++u<=*++v){j=*v/1024; m=MAX(m,j);}
 if(!m)R 1;
 GA(t,INT,1+m,1,0); v=AV(t);
 /* must not allocate memory after this point */
 for(i=6;i<=PLIML;++i){
  if(jt->mfreet[i]>jt->mfreeb[i])continue;
  n=0; x=(MS*)(jt->mfree[i]); 
  while(x){v[n++]=(I)x; x=(MS*)(x->a);}
  qsort(v,n,SZI,spfreecomp); 
  j=0; u=0; c=msize[i]; d=PSIZE/c;
  while(n>j){
   x=(MS*)v[j];
   if(MFHEAD&x->mflag&&n>=j+d&&PSIZE==c+v[j+d-1]-v[j]){
    j+=d; 
    FREE(x); 
    jt->mfreeb[i]-=PSIZE;
   }else{x->a=u; u=(I*)v[j]; ++j;}
  }
  jt->mfree[i]=u; jt->mfreet[i]=1048576+jt->mfreeb[i];
 }
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
   j=6; n>>=j; 
   while(n){n>>=1; ++j;} 
   if(p==msize[j-1])--j;
   jt->spfor+=msize[j];
  }else jt->spfor+=msize[((MS*)w-1)->j];
 R mtm;
}

F1(jtspfor){A*wv,x,y,z;C*s;D*v,*zv;I i,m,n,wd;
 RZ(w);
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w); v=&jt->spfor;
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 GA(z,FL,n,AR(w),AS(w)); zv=DAV(z); 
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
 GA(z,FL,n,AR(w),AS(w)); zv=DAV(z); 
 for(i=0;i<n;++i){
  x=WVR(i); m=AN(x); s=CAV(x);
  if(!m){m=4; s="base";}
  ASSERT(LIT&AT(x),EVDOMAIN);
  ASSERT(1>=AR(x),EVRANK);
  ASSERT(vlocnm(m,s),EVILNAME);
  y=stfind(0,m,s);
  ASSERT(y,EVLOCALE);
  *v=(D)msize[((MS*)y-1)->j];
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
 j=m-1; DO(m, if(n<=msize[i]){j=i; break;});
 jt->mmax=msize[j];
 R mtm;
}    /* 9!:21 space limit set */

#if MEMAUDIT>=1
// Verify that block w does not appear on tstack more than lim times
static void audittstack(J jt, A w, I lim){
 // loop through each block of stack
 I base; A* tstack; I ttop,stackct=0;
 for(base=jt->tbase,tstack=jt->tstack,ttop=jt->ttop;base>=0;base-=NTSTACK){I j;
  // loop through each entry, skipping the first which is a chain
  for(j=1;j<ttop;++j)if(tstack[j]==w){
   ++stackct;   // increment number of times we have seen w
   if(stackct>lim)*(I*)0=0;  // if too many, abort
  }
  // back up to previous block
  base -= NTSTACK;  // this leaves a gap but it matches other code
  ttop = NTSTACK;
  if(base>=0)tstack=AAV(tstack[0]); // back up to data for previous field
 }
}
#endif

static void freesymb(J jt, A w){I j,k,kt,wn=AN(w),*wv=AV(w);
 fr(LOCPATH(w));
 fr(LOCNAME(w));
 for(j=1;j<wn;++j){
  // free the chain; kt->last block freed
  for(k=wv[j];k;k=(jt->sympv)[k].next){kt=k;fr((jt->sympv)[k].name);fr((jt->sympv)[k].val);(jt->sympv)[k].name=0;(jt->sympv)[k].val=0;(jt->sympv)[k].sn=0;(jt->sympv)[k].flag=0;(jt->sympv)[k].prev=0;}  // prev for 18!:31
  // if the chain is not empty, make it the base of the free pool & chain previous pool from it
  if(k=wv[j]){(jt->sympv)[kt].next=jt->sympv->next;jt->sympv->next=k;}
 }
}

void jtfr(J jt,A w){I j,n;MS*x;
 if(!w)R;
 x=(MS*)w-1;   // point to free header
#if MEMAUDIT>=1
 if(!(AFLAG(w)&(AFNJA|AFSMM)||x->a==(I*)0xdeadbeef))*(I*)0=0;  // testing - verify block is memmapped/SMM or allocated
#endif
 if(ACDECR(w)>0)R;  // fall through if decr to 0, or from 100001 to 100000
#if MEMAUDIT>=1
 if(ACUC(w))*(I*)0=0;  // usecount should not go below 0
#if MEMAUDIT>=2
 audittstack(jt,w,0);  // must not free anything on the stack
#endif
#endif
 // SYMB must free as a monolith, with the symbols returned when the hashtables are
 if(AT(w)==SYMB)freesymb(jt,w);
 j=x->j;
#if MEMAUDIT>=1
 if(j<6||j>63)*(I*)0=0;  // pool number must be valid
#if MEMAUDIT>=3
 DO(1<(j-LGSZI), ((I*)x)[i] = 0xdeadbeef;);
#endif
#endif
 n=1LL<<j;
 if(PLIML<j)FREE(x);  /* malloc-ed       */
 else{                /* pool allocation */
  x->a=jt->mfree[j]; 
  jt->mfree[j]=(I*)x; 
  jt->mfreeb[j]+=n;
#if MEMAUDIT>=1
  x->j=0xdeaf;
#endif
 }
 jt->bytes-=n;
}

static A jtma(J jt,I m){A z;C*u;I j,n,p,*v;MS*x;
 p=m+mhb; 
 ASSERT((UI)p<=(UI)jt->mmax,EVLIMIT);
 for(n=64;n<p;n=n+n);
 j=CTTZI(n);
 JBREAK0;  // Here to allow instruction scheduling
 if(jt->mfree[j]){         /* allocate from free list         */
  z=(A)(mhw+jt->mfree[j]);
#if MEMAUDIT>=1
  if(((MS*)z-1)->j!=(S)0xdeaf)*(I*)0=0;  // verify block has free-pool marker
#endif
  jt->mfree[j]=((MS*)(jt->mfree[j]))->a;
  jt->mfreeb[j]-=n;
 }else if(j>PLIML){         /* large block: straight malloc    */
  v=MALLOC(n);
  ASSERT(v,EVWSFULL); 
  z=(A)(v+mhw);
 }else{                    /* small block: do pool allocation */
  v=MALLOC(PSIZE);
  ASSERT(v,EVWSFULL);
  u=(C*)v; DO(PSIZE>>j, x=(MS*)u; u+=n; x->a=(I*)u; x->mflag=0;); x->a=0;  // chain blocks to each other; set chain of last block to 0
#if MEMAUDIT>=1
   u=(C*)v; DO(PSIZE>>j, ((MS*)u)->j=0xdeaf; u+=n;);
#endif
  ((MS*)v)->mflag=MFHEAD;
  z=(A)(mhw+v); 
  jt->mfree[j]=((MS*)v)->a;
  jt->mfreeb[j]+=PSIZE-n;
 }
 if(jt->bytesmax<(jt->bytes+=n))jt->bytesmax=jt->bytes;
 x=(MS*)z-1; x->j=(C)j;  // Why clear a?
#if MEMAUDIT>=1
 x->a=(I*)0xdeadbeef;  // flag block as allocated
#endif
 R z;
}

static void jttraverse(J jt,A wd,AF f){
// obsolete RZ(w);
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
// obsolete R mark;
}

void jtfh(J jt,A w){fr(w);}

static A jttg(J jt){A t=jt->tstacka,z;
 RZ(z=ma(SZI*WP(BOX,NTSTACK,1L)));
 AT(z)=BOX; AC(z)=ACUC1; AR(z)=1; AN(z)=*AS(z)=NTSTACK; AM(z)=NTSTACK*SZA; AK(z)=AKX(z);
 jt->tstacka=z; jt->tstack=AAV(jt->tstacka); jt->tbase+=NTSTACK; jt->ttop=1;
 *jt->tstack=t;
 R z;
}

static void jttf(J jt){A t=jt->tstacka;
 jt->tstacka=*jt->tstack; jt->tstack=AAV(jt->tstacka); jt->tbase-=NTSTACK; jt->ttop=NTSTACK;
 fr(t);
}

F1(jttpush){
 RZ(w);
 if(AT(w)&TRAVERSIBLE)traverse(w,jttpush);
 if(jt->ttop>=NTSTACK)RZ(tg());
 jt->tstack[jt->ttop]=w;
 ++jt->ttop;
#if MEMAUDIT>=2
 audittstack(jt,w,ACUC(w));  // verify total # w on stack does not exceed usecount
#endif
 R w;
}

I jttpop(J jt,I old){
 while(old<jt->tbase+jt->ttop)if(1<jt->ttop){jtfr(jt,jt->tstack[--jt->ttop]);} else tf(); 
 R old;
}

A jtgc (J jt,A w,I old){ra(w); tpop(old); R tpush(w);}

I jtgc3(J jt,A x,A y,A z,I old){
 if(x)ra(x);    if(y)ra(y);    if(z)ra(z);
 tpop(old);
 if(x)tpush(x); if(y)tpush(y); if(z)tpush(z);
 R 1;  // good return
}

#if 1
I jtra(J jt,AD* RESTRICT wd,I t){I af=AFLAG(wd); I n=AN(wd);
 if(t==BOX){
  // boxed.  Loop through each box, recurring if called for.  Two passes are intertwined in the loop
  A* RESTRICT wv=AAV(wd);  // pointer to box pointers
  I wrel = af&AFREL?(I)wd:0;  // If relative, add wv[] to wd; othewrwise wv[] is a direct pointer
  if((af&AFNJA+AFSMM)||n==0)R 0;  // no processing if not J-managed memory (rare)
  // runin for loop
  AD* RESTRICT np1= (A)((I)*wv+(I)wrel); ++wv; // np -> box
  I t1=np1?AT(np1):0;  // type for box.  the pointer may be 0, if there was an error creating the boxed result
  // The loop, pipelined
  while(--n) {   // loop n-1 times
   AD* RESTRICT np2=np1;  // pipeline stage 2
   np1 = (A)((I)*wv+(I)wrel); ++wv; // np -> box
   I c2=np2?AC(np2):0;   // fetch count in stage 2.  Will be in cache
   I t2 = t1;    // save fetch from previous loop
   t1=np1?AT(np1):0;  // fetch type.  Will complete in next loop
   if(t2&TRAVERSIBLE)jtra(jt,np2,t2);  // recur if recursible
   if(np2)AC(np2)=(c2+ACUC1)&~ACINPLACE;  // increment usecount
  }
  // runout for loop
  I c1=np1?AC(np1):0;
  if(t1&TRAVERSIBLE)jtra(jt,np1,t1);  // recur if recursible
  if(np1)AC(np1)=(c1+ACUC1)&~ACINPLACE;  // increment usecount
 } else if(t&(VERB|ADV|CONJ)){V* RESTRICT v=VAV(wd);
  // ACV.  Recur on each component; but this is a problem because it is done in unquote as part of executing
  // any name.  So we take advantage of the fact that all non-noun references are through names, not values; and
  // thus it is impossible to delete something that is referred to by a named ACV.  The ACV becomes a non-recursive
  // usecount, with a separate count of the number of assignments that have been made.  When this count increments to
  // 1 or decrements to 0, we propagate the change to descendants, but not otherwise
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
 if(t==BOX){
  // boxed.  Loop through each box, recurring if called for.  Two passes are intertwined in the loop
  A* RESTRICT wv=AAV(wd);  // pointer to box pointers
  I wrel = af&AFREL?(I)wd:0;  // If relative, add wv[] to wd; othewrwise wv[] is a direct pointer
  if((af&AFNJA+AFSMM)||n==0)R 0;  // no processing if not J-managed memory (rare)
  // runin for loop
  AD* RESTRICT np1= (A)((I)*wv+(I)wrel); ++wv; // np -> box
  I t1=np1?AT(np1):0;  // type for box.  the pointer may be 0, if there was an error creating the boxed result
  // The loop, pipelined
  while(--n) {   // loop n-1 times
   AD* RESTRICT np2=np1;  // pipeline stage 2
   np1 = (A)((I)*wv+(I)wrel); ++wv; // np -> box
   I c2=np2?AC(np2):0;   // fetch count in stage 2.  Will be in cache
   I t2 = t1;    // save fetch from previous loop
   t1=np1?AT(np1):0;  // fetch type.  Will complete in next loop
   if(t2&TRAVERSIBLE)jtfa(jt,np2,t2);  // recur if recursible
   if(np2)if(--c2<=0){fr(np2)} else AC(np2)=c2;  // increment usecount  scaf change to mf
  }
  // runout for loop
  I c1=np1?AC(np1):0;
  if(t1&TRAVERSIBLE)jtfa(jt,np1,t1);  // recur if recursible
  if(np1)if(--c1<=0){fr(np1)}else AC(np1)=c1;  // increment usecount  scaf change to mf
 } else if(t&(VERB|ADV|CONJ)){V* RESTRICT v=VAV(wd);
  // ACV.  Recur on each component; but this is a problem because it is done in unquote as part of executing
  // any name.  So we take advantage of the fact that all non-noun references are through names, not values; and
  // thus it is impossible to delete something that is referred to by a named ACV.  The ACV becomes a non-recursive
  // usecount, with a separate count of the number of assignments that have been made.  When this count increments to
  // 1 or decrements to 0, we propagate the change to descendants, but not otherwise
  if(v->f)fa(v->f); if(v->g)fa(v->g); if(v->h)fa(v->h);
 } else if(t&(RAT|XNUM|XD)) {A* RESTRICT v=AAV(wd);
  // single-level indirect forms.  handle each block
  DO(t&RAT?2*n:n, if(*v)fr(*v); ++v;);
 } else if(t&SPARSE){P* RESTRICT v=PAV(wd);
  if(SPA(v,a))fa(SPA(v,a)); if(SPA(v,e))fa(SPA(v,e)); if(SPA(v,i))fa(SPA(v,i)); if(SPA(v,x))fa(SPA(v,x)); 
 }
 R 1;
}
#else
I jtra(J jt, A w,I f){I *acaddr=&AC(w); RZ(w); I ac=*acaddr; traverse(w,jtra); *acaddr=(ac+ACUSECOUNT)&~ACINPLACE; R w;   }
I jtfa(J jt, A w, I f){RZ(w); traverse(w,jtfa); fr(w);   R mark;}
#endif


static F1(jtra1){RZ(w); if(AT(w)&TRAVERSIBLE)traverse(w,jtra1); ACINCRBY(w,jt->arg); R w;}
A jtraa(J jt,I k,A w){A z;I m=jt->arg; jt->arg=k; z=ra1(w); jt->arg=m; R z;}

F1(jtrat){ra(w);R tpush(w);}



#if 0
// include in all files for inlining, except that we can't return from a conexpr
static I alloblock(J jt,I bytes){ASSERT((UI)bytes<=(UI)jt->mmax,EVLIMIT); I n=64, j=6; while(n<bytes)n<<=1, ++j; R j;}

// VSZ when size is constant or variable
#define ALLOBYTESVSZ(atoms,rank,size,islast,isname)      ( ((rank*SZI  + ((islast)? (isname)?(AH*SZI+sizeof(NM)+SZI+mhb):(AH*SZI+SZI+mhb) : (AH*SZI+SZI+mhb-1)) + (atoms)*(size)) & (-SZI))  )  // # bytes to allocate allowing only 1 byte for string pad - include mem hdr
// here when size is constant
#define ALLOBYTES(atoms,rank,size,islast,isname)      ((size%SZI)?ALLOBYTESVSZ(atoms,rank,size,islast,isname):(SZI*(rank+AH+mhw+(size/SZI)*atoms)))  // # bytes to allocate
#define ALLOBLOCK(n) ((n)<=128?((n)<=64?6:7) : (n)<=512?((n)<=256?8:9) : (n)<=1024?10:*(C*)0)
#define ALLOBLOCKV(bytes) alloblock(jt, bytes)
#define ALLONULL(n) (n)

#define GAFx(name,type,atoms,rank,shaape,size,func,allo) \
{ I bytes = ALLOBYTES(atoms,rank,size,type&LAST0,type==NAME); \
  ASSERT((UI)atoms<TOOMANYATOMS,EVLIMIT);  /* 32-bit? */  \
RZ(name = func(jt, allo(bytes))); \
AT(name)=type; \
if(1==rank&&!(type&SPARSE))*AS(z)=atoms; else if(rank&&shaape)ICPY(AS(name),shaape,rank);  /* 1==atoms always if t&SPARSE  could check rank&&shape first - would that avoid SPARSE test? */ \
AN(name)=atoms; AR(name)=rank; AK(name)=SZI*AH+SZI*rank; \
if(!(type&DIRECT))memset((C*)name+(SZI*AH+SZI*rank),C0,bytes-(SZI*AH+SZI*rank)-mhb); \
if(type&LAST0){((I*)((C*)name+bytes))[-(I)mhw-1]=0; }  /* if LAST0, clear the last I  */ \
}
#define GAZ(name,type,atoms,rank,shaape)  GAFx(name,type,atoms,rank,shaape,type ## SIZE,jtgaf,ALLOBLOCK)
#define GAZZ(name,type,atoms,rank,shaape)  GAFx(name,type,atoms,rank,shaape,type ## SIZE,jtgafv,ALLONULL)


#define GAV(name,type,atoms,rank,shaape) RZ(name = jtgaa(jt,type,atoms,rank,shaape)) // use for variable type

extern A jtgaf(J jt, I block){if(jt->parsercalls)++jt->parsercalls; R block?jtgaf(jt,block-1):0;}
extern A jtgafv(J jt, I block){if(jt->parsercalls)++jt->parsercalls; R block?jtgaf(jt,block-1):0;}
A testga(I n){A z;J jt;
#if 0
I mmul = n*32;
I msl = n<<6;
I mdiv = n/32;
I msr = n>>6;
I mrem = n%64;
I mand = n&-128;
#endif
GAZ(z,INT,1, 1, 0);
//GAF(z,LIT,6, 1, 0);
//GAFV(z,INT,n,1,0);
GAZZ(z,LIT,n,1,0);
//GAFV(z,INT,n,2,&num);
R z /*+ mmul+msl+mdiv+msr+mrem+mand*/;
}



need version for general GA - should it take type?  Yes - do all the stores inside, call afv
A jtgav(J jt,I type,I atoms,I rank,I shaape){A z;
I bytes = ALLOBYTESVSZ(atoms,rank,type ## SIZE,type&LAST0,type==NAME);
  ASSERT((UI)atoms<TOOMANYATOMS,EVLIMIT); /* 32-bit? */ 
RZ(z = jtgafv(jt, bytes));
AT(z)=type; \
if(1==rank&&!(type&SPARSE))*AS(z)=atoms; else if(rank&&shaape)ICPY(AS(name),shaape,rank);  /* 1==atoms always if t&SPARSE  could check rank&&shape first - would that avoid SPARSE test? */
AN(name)=atoms; AR(name)=rank; AK(name)=SZI*AH+SZI*rank;
if(!(type&DIRECT))memset((C*)name+(SZI*AH+SZI*rank),C0,bytes-(SZI*AH+SZI*rank)-mhb);
if(type&LAST0){((I*)((C*)name+bytes))[-(I)mhw-1]=0; }  /* if LAST0, clear the last I  */
}

A jtgafv(J jt, I bytes){I j=(AH*SZI+mhb)<=64?6:7, n=(AH*SZI+mhb)<=64?64:128; 
 ASSERT((UI)bytes<=(UI)jt->mmax,EVLIMIT);
 while(n<bytes)n<<=1, ++j;
 R jtgaf(jt,j);
}
A jtgaf(J jt,I blockx){A z;MS *v;
 I mfree=jt->mfree[blockx], mfreeb=jt->mfreeb[blockx], ttop=jt->ttop, tbase=jt->tbase; // start reads early
 JBREAK0;  // Put it here to assist instruction scheduling
 if(blockx>PLIML){I n = 1<<blockx;         /* large block: straight malloc    */
  ASSERT(v=MALLOC(n),EVWSFULL);
  v->j = blockx;    // Save the size of the allocation so we know how to free it and how big it was
  mfreeb+=n;    // mfreeb[PLIML+1] is the byte count allocated for large blocks
 } else {
  mfreeb-=MFREEBINCR;            // we are going to remove 1 free block
  if(mfree){         // allocate from a chain of free blocks
   v = jt->mfree[blockx]);
   jt->mfree[blockx] = v->a;  // remove & use the head of the free chain
#if MEMAUDIT>=1
   if(v->j!=blockx)*(I*)0=0;  // verify block has correct size
#endif
  }else{MS *x;C* u;I nblocks=PSIZE>>blockx;                    // small block, but chain is empty.  Alloc PSIZE and split it into blocks
   ASSERT(v=MALLOC(PSIZE),EVWSFULL);
   u=(C*)v; DO(nblocks, x=(MS*)u; u+=(1<<blockx); x->a=(I*)u; x->j=blockx; x->ofst=i<<blockx;); x->a=0;  // chain blocks to each other; set chain of last block to 0
   jt->mfree[j]=(MS*)((C*)v+(1<<blockx);  // the second block becomes the head of the free list
   mfreeb+=nblocks<<MFREEBBUFSHIFT;   // We are adding a bunch of free blocks
   jt->mfreeb[PSIZEL]+=PSIZE;   // add to the total bytes allocated
  }
 }

#if MEMAUDIT>=1
 v->a=(I*)0xdeadbeef;  // flag block as allocated
#endif
  z=(A)&v[1];  // advance past the memory header
 jt->mfreeb[blockx]=mfreeb;
 // If the user is keeping track of memory high-water mark with 7!:2, figure it out & keep track of it
 if(mfreeb&1){I bytes = spbytesinuse(); if(bytes>jt->bytesmax)jt->bytesmax=bytes;}


#if MEMAUDIT>=2
 audittstack(jt,z,0);  // verify buffer not on stack
#endif
 AC(z)=ACUC1; AFLAG(z)=0; 
AM(z)=msize[((MS*)z-1)->j]-(AK(z)+sizeof(MS));   // get rid of this?  Or change to offset from original value.  Must mod, since AK not set yet

 jt->tstack[ttop]=z; if(ttop==tbase+NTSTACK-1)RZ(tpushnextbuf());else jt->ttop = ttop+1;  // Perform tpush1 (we already read ttop and tbase)

 R z;
}


// free a block whose usecount makes it freeable
void jtmf(J jt,A w){MS*x; 
 x=(MS*)w-1;   // point to free header
 I blockx=x->j;   // lg(buffer size)
#if MEMAUDIT>=1
 if(!(AFLAG(w)&(AFNJA|AFSMM)||x->a==(I*)0xdeadbeef))*(I*)0=0;  // testing - verify block is memmapped/SMM or allocated
#endif
#if MEMAUDIT>=2
 audittstack(jt,w,0);  // must not free anything on the stack
#endif
 I mfreeb=jt->mfreeb[blockx];   // start reads early
 A mfree=jt->mfree[blockx];
 // SYMB must free as a monolith, with the symbols returned when the hashtables are
 if(AT(w)==SYMB){
  freesymb(jt,w);
  mfreeb=jt->mfreeb[blockx];   // restore values that were read early.  This path is very rare
  mfree=jt->mfree[blockx];
 }
#if MEMAUDIT>=1
 if(blockx<6||blockx>=sizeof(jt->mfreeb)/sizeof(jt->mfreeb[0]))*(I*)0=0;  // pool number must be valid
#endif
#if MEMAUDIT>=3
 DO((1<<(blockx-LGSZI))-2, ((I*)x)[i+2] = 0xdeadbeef;);   // wipe the block clean before we free it
#endif
 if(PLIML+1<=blockx){   // allocated by malloc
  I n=1LL<<blockx;   // number of bytes in the allocation
  FREE(x);
  mfreeb-=n;
 else{                // buffer allocated from subpool.
  x->a=mfree;
  jt->mfree[j]=(I*)x;
  if((0 <= (mfreeb+=MFREEBINCR))&&(mfreeb<=MFREEBINCR))jt->spfreeneeded=1;  // Indicate we have one more free buffer;
   // if this kicks the list into garbage-collection mode, indicate that
 }
 jt->mfreeb[blockx]=mfreeb;
}

// Get total # bytes in use.  That's total allocated so far, minus the bytes in the free lists
I jtspbytesinuse(J jt){I i,totalfree = 0;
for(i=6;i<=PLIML;++i){totalfree-=jt->(mfreet[i]+(jt->mfreeb[i]>>MFREEBBUFSHIFT))<<i;  // for pool sizes, count bytes in free buffers (negative addition to use)
for(;i<=48;++i){totalfree+=jt->mfreeb[i];}  // for allocations, count the number of bytes allocated.  48 is enough bins for MY lifetime.
R totalfree;
}

// Start tracking jt->bytes and jt->bytesmax.  We indicate this by setting the LSB of EVERY entry of mfreeb
// Also count current space, and set that into jt->bytes and the result of this function
I jtspstarttracking(J jt){
 DO(sizeof(jt->mfreeb)/sizeof(jt->mfreeb[0]), jt->mfreeb[i] |= MFREEBCOUNTING);
 R spbytesinuse();
}

// Turn off tracking.
void jtspendtracking(J jt){
 DO(sizeof(jt->mfreeb)/sizeof(jt->mfreeb[0]), jt->mfreeb[i] &= ~MFREEBCOUNTING);
 R;
}

#define tpush1(a) { I ttop = jt->ttop; jt->tstack[ttop]=a; if(ttop==jt->tbase+NTSTACK-1)RZ(tpushnextbuf());else jt->ttop = ttop+1;}

I tpushnextbuf(){     // Filling last slot; must allocate next page
 if(jt->nextstack) {   // if we already have a page to move to
  jt->base += NTSTACK; jt->ttop = jt->base+1;  // advance next-slot and base pointers
//  jt->nextstack[0] = jt->tstack;   // next was chained to prev before it was saved as next
  jt->tstack = jt=->nextstack;   // set new buffer as current
  jt->nextstack = 0;    // indicate no new one available now
 } else {I *v;   // no page to move to - better read one
   // We don't account for the NTSTACK blocks as part of memory space used, because it's so unpredictable and large as to be confusing
   ASSERT(v=MALLOC(NTSTACK*SZI),EVWSFULL);
   jt->base += NTSTACK; jt->ttop = jt->base+1;  // advance buffer pointers for the next store
   *v = jt->tstack;   // backchain old buffers to new
   jt->tstack = v;    // set new buffer as the one to use
 }
R 1;
}

#define fr(a) {I s = AC(a)-1; if(s<=0)mf(a);else AC(a)=s;}

// pop stack,  ending when we have freed the entry with ttop==old.  ttop is left pointing to an empty slot
I jttpop(J jt,I old){I ttop=jt->ttop;
 while(old<jt->tbase+ttop) {
  I endingttop = MAX(old,jt->tbase+1);  // Get # of frees we can perform in this tstack block
  while(ttop-- > endingttop)fr(jt->tstack[ttop]);  // Do em
  if(ttop>old){      // If we haven't done them all, we must have hit start-of-block.  Move back to previous block
    if(jt->nextstack)FREE(jt->nextstack);   // We will set the block we are vacating as the next-to-use.  We can have only 1 such; if there is one already, free it
    jt->nextstack=jt->tstack;  // save the next-to-use
    jt->tstack=jt->tstack[0];   // back up to the previous block
    ttop=NTSTACK;    // position our pointers at the end of it
    jt->base -= NTSTACK;
  }
 }
 jt->ttop=ttop;
}

#define MEMJMASK 0xf   // these bits of j contain subpool #; higher bits used for computation for subpool entries
#define SBFREEB 1048576L   // number of bytes that need to be freed before we rescan
#define SBFREEBLG 20   // jg2(SBFREEB)
#define MFREEBCOUNTING 1   // When this bit is set in mfreeb[], we keep track of max space usage
#define MFREEBBUFSHIFT 1   // the buffer-count portion of mfreeb is shifted this many bits over
#define MFREEBINCR (1<<MFREEBBUFSHIFT)
 for(i = 6;i<=PLIML;++1) {
must initialize mfreeb[] to proper negative values
  // Check each chain to see if it is ready to coalesce
  if(mfreeb[i]>=0) {
   // garbage collector: coalesce blocks in chain i
   // pass through the chain, incrementing the j field in the base allo for each
   incr = 0x8000>>(PSIZEL-i);  // number of subbuffers=2^(PSIZEL-i); we want this number to come out 0x8000 (i. e. negative in a S)
   for(p=(MS *)jt->mfree[i];p;p=p->a){(MS *)((C*)p-p->ofst)->j += incr;}
   // pass through the chain again, looking for blocks that have negative j in the corresponding base.  They should
   // all the removed from the chain; the base block is added to the chain of blocks to be freed.
   // In all surviving blocks the count must be removed from the j field (to set up for next time)
   MS *freehead = 0; MS *survivetail = (MS *)&jt->mfree[i];
   for(p=(MS *)jt->mfree[i];p;p=p->a){
    if((MS *)((C*)p-p->ofst)->j<0){if(p->ofst==0){p->a=freehead; freehead=p;}}
    else{survivetail->a=p;survivetail=p;p->j&=MEMJMASK;}
   }
   survivetail->a=0;  // terminate the chain of surviving buffers
   // free the buffers that need to be freed; count them
   I nfreebufs=0;
   while(freehead){MS *nfreehead = freehead->a; FREE(freehead); ++ nfreebufs; freehead = nfreehead;}  // save chain before freeing the block
   // set up for next spfree: set mfreeb to a negative value such that when SPFREEB bytes have been freed,
   // mfreeb will hit 0, causing a rescan.  Set mfreet so that
   // # bytes used in subpool = (subpool size 1<<j) * (mfreet + mfreeb)
   // Account for the buffers that were freed during the coaleascing
   I newbval = SBFREEBLG>>i;   // this many buffers need to be freed before we rescan
   jt->mfreet[i] += (jt->mfreeb[i] >> MFREEBBUFSHIFT) + newbval - (nfreebufs<<(PSIZEL-i));  // add diff between current mfreeb[] and what it will be set to; deduct freed buffers
   jt->mfreeb[i] = ((-newbval) << MFREEBBUFSHIFT) + (jt->mfreeb[i] & ~MFREEBCOUNTING);
   jt->mfreeb[PSIZEL] -= nfreebufs<<PSIZEL;   // take freed buffers out of allocated byte count
  }
 }
jt->spfreeneeded = 0;  // indicate no check needed yet


#endif

A jtga(J jt,I t,I n,I r,I*s){A z;I m,w;
 if(t&BIT){const I c=8*SZI;              /* bit type: pad last axis to fullword */
  ASSERTSYS(1>=r||s,"ga bit array shape");
  if(1>=r)w=(n+c-1)/c; else RE(w=mult(prod(r-1,s),(s[r-1]+c-1)/c));
#if SY_64
  m=BP(INT,0L,r);
  ASSERT((UI)n<TOOMANYATOMS,EVLIMIT);
#else
  // This test is not perfect but it's close
  w+=WP(INT,0L,r); m=SZI*w; 
  ASSERT(     n>=0&&m>w&&w>0,EVLIMIT);   /* beware integer overflow */
#endif
 }else{
#if SY_64
  m=BP(t,n,r);
  ASSERT((UI)n<TOOMANYATOMS,EVLIMIT);
#else
  w=WP(t,n,r);     m=SZI*w; 
  ASSERT(m>n&&n>=0&&m>w&&w>0,EVLIMIT);   /* beware integer overflow */
#endif
 }
 RZ(z=ma(m));
#if MEMAUDIT>=2
 audittstack(jt,z,0);  // verify buffer not on stack
#endif
 if(!(t&DIRECT))memset(z,C0,m);
 if(t&LAST0){I*v=(I*)((C*)z+m); v[-1]=0; v[-2]=0;}  // if LAST0, clear the last two Is.
 AC(z)=ACUC1; AN(z)=n; AR(z)=r; AFLAG(z)=0; AK(z)=AKX(z); AM(z)=msize[((MS*)z-1)->j]-(AK(z)+sizeof(MS)); 
 AT(z)=0; tpush(z); AT(z)=t;
 if(1==r&&!(t&SPARSE))*AS(z)=n; else if(r&&s)ICPY(AS(z),s,r);  /* 1==n always if t&SPARSE */
 R z;
}

A jtgah(J jt,I r,A w){A z;
 ASSERT(RMAX>=r,EVLIMIT); 
 RZ(z=ma(SZI*(AH+r)));
 AT(z)=0; AC(z)=ACUC1; tpush(z);  // original had ++AC(z)!?
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
 GA(z,t,AN(w),AR(w),AS(w)); if(AFLAG(w)&AFNJA+AFSMM+AFREL)AFLAG(z)=AFREL;
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
 switch(AT(w)){
  case RAT:  n+=n;
  case XNUM:
  case BOX:  u=AAV(z); wv=AAV(w); wd=(I)w*ARELATIVE(w); DO(n, RZ(*u++=car(WVR(i)));); break;
  case SB01: case SLIT: case SINT: case SFL: case SCMPX: case SBOX:
   p=PAV(z); 
   SPB(p,a,car(SPA(p,a)));
   SPB(p,e,car(SPA(p,e)));
   SPB(p,i,car(SPA(p,i)));
   SPB(p,x,car(SPA(p,x)));
   break;
  case VERB: case ADV: case CONJ: 
   v=VAV(z); 
   if(v->f)RZ(v->f=car(v->f)); 
   if(v->g)RZ(v->g=car(v->g)); 
   if(v->h)RZ(v->h=car(v->h));
 }
 R z;
}

B jtspc(J jt){A z; RZ(z=MALLOC(1000)); FREE(z); R 1; }

A jtext(J jt,B b,A w){A z;I c,k,m,m1,t;
 RZ(w);                               /* assume AR(w)&&AN(w)    */
 m=*AS(w); c=AN(w)/m; t=AT(w); k=c*bp(t);
 GA(z,t,2*AN(w),AR(w),AS(w)); 
 MC(AV(z),AV(w),m*k);                 /* copy old contents      */
 if(b){ra(z); fa(w);}                 /* 1=b iff w is permanent */
 *AS(z)=m1=AM(z)/k; AN(z)=m1*c;       /* "optimal" use of space */
 if(!(t&DIRECT))memset(CAV(z)+m*k,C0,k*(m1-m));
 R z;
}

A jtexta(J jt,I t,I r,I c,I m){A z;I k,m1; 
 GA(z,t,m*c,r,0); 
 k=bp(t); *AS(z)=m1=AM(z)/(c*k); AN(z)=m1*c;
 if(2==r)*(1+AS(z))=c;
 if(!(t&DIRECT))memset(AV(z),C0,k*AN(z));
 R z;
}    /* "optimal" allocation for type t rank r, c atoms per item, >=m items */


/* debugging tools  */

B jtcheckmf(J jt){C c;I i,j;MS*x,*y;
 for(j=0;j<=PLIML;++j){
  i=0; y=0; x=(MS*)(jt->mfree[j]); /* head ptr for j-th pool */
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
