/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Interval Index, mostly by Binary Search                          */

#include "j.h"


#define BXLOOP(T)  \
 {T*wv=(T*)AV(w),x;                                                  \
  if(descend){DQ(m, x=*wv++; I t=n; t=x>=0?p:t; t=x>=1?0:t; *zv++=t;);        /* p q */  \
  }else if(p<0){DQ(m, *zv++=n*(1<*wv++);) /*   q */  \
  }else if(q<0){DQ(m, *zv++=n*(0<*wv++););   /* p   */  \
  }else{DQ(m, x=*wv++; I t=n; t=x<=1?q:t; t=x<=0?0:t; *zv++=t;) /* p q */  \
  }  \
 }

static B jtiixBX(J jt,I n,I m,A a,A w,I*zv){B*av,*b,descend;I p,q;
 av=BAV(a); descend=av[0]>av[n-1];
 // p is index of 1st 0, or -1 if none; q similar for first 1
 b=memchr(av,1-descend,n);  // if ascending, get position of 1st 1; if descending, 1st 0.
 p=(b-av)-(1-descend); p=(1-descend)|(I)b?p:-1; p=descend|(I)b?p:0; // set p to position of 0 (descending) or before 1st 1 (ascending).  If descending & 0 not found, use -1; if ascending & 1 not found, use 0
 q=(b-av)-descend; q=descend|(I)b?q:-1; q=(1-descend)|(I)b?q:0; // set q to position of 1 (ascending) or before 1st 0 (descending).  If ascending & 1 not found, use -1; if descending & 0 not found, use 0
 switch(AT(w)){
 case INT: BXLOOP(I); break;
 case FL:  BXLOOP(D); break;
 case B01:
  b=BAV(w);
  if(descend){DQ(m, *zv++=p*!*b++;)         /* p q */
  }else if(p<0){mvc(m*SZI,zv,MEMSET00LEN,MEMSET00);  /*   q */
  }else if(q<0){DQ(m, *zv++=n* *b++;)  /* p   */
  }else{DQ(m, *zv++=q* *b++;)  /* p q */
  }
 }
 R 1;
}    /* a I."r w where a is a boolean list */

static B jtiixI(J jt,I n,I m,A a,A w,I*zv){A t;B ascend;I*av,j,p,q,*tv,*u,*v,*wv,x,y;
 av=AV(a); wv=AV(w);
 p=av[0]; q=av[n-1]; ascend=p<=q; if(!ascend){x=p; p=q; q=x;}
 GATV0(t,INT,1+q-p,1); v=AV1(t); tv=v-p;   // v->buffer; tv->virtual buffer origin, where p=0; vv->buffer end
  // This could be recoded to allocate slots for <p and >q, but it would be better only if those cases were common
 if(ascend){u=av;     x=*u++; *v++=j=0; DQ(n-1, ++j; y=*u++; ASSERT(BETWEENC(y,x,q),EVDOMAIN); DQ(y-x, *v++=j;); x=y;);}
 else      {u=av+n-1; x=*u--;      j=n; DQ(n-1, --j; y=*u--; ASSERT(BETWEENC(y,x,q),EVDOMAIN); DQ(y-x, *v++=j;); x=y;);}
 if(ascend)DQ(m, x=*wv++; *zv++=x<=p?0:q<x?n:tv[x];)
 else      DQ(m, x=*wv++; *zv++=x>=q?0:p>x?n:tv[x];);
 R 1;
}    /* a I. w where a is a list of small range integers */

#define SBCOMP(x,y) (SBGT((x),(y))-SBLT((x),(y)))
#define COMPVLOOP(T,c)       \
 {T*u=(T*)uu,*v=(T*)vv; DQ(c, if(*u!=*v){cc=*u<*v?-1:1; break;} ++u; ++v;);}
#define COMPVLOOF(T,c,COMP)  \
 {T*u=(T*)uu,*v=(T*)vv; DQ(c, if(cc=COMP(*u,*v))break;          ++u; ++v;);}

#define MID(k,p,q)   k=(UI)((p)+(q))>>1  /* beware integer overflow */

#define BSLOOP1(CMP)           \
  p=0; q=n-1; y=*wv++;         \
  while(p<=q){MID(k,p,q); CMP; \
      if(b)q=k-1; else p=k+1;}
// Binary search without misprediction, on atoms
#define BSLOOP1x(CMP)    /* if CMP is true, move q; otherwise p */       \
  y=*wv++; p=-1; q=n; k=(q-1)>>1;         \
  do{   /* empty lists handled earlier */ \
   x=av[k]; /* read the next value */ \
   I k2, k1=k; MID(k2,k,q); k=k2-((q-p)>>1); /* calculate new values for k */\
   k=CMP?k:k2; p=CMP?p:k1; q=CMP?k1:q; /* move k first to start next fetch */\
  }while(q>p+1); 

#define BSLOOPN(NE,CMP)        \
  p=0; q=n-1;                  \
  while(p<=q){MID(k,p,q); u=av+c*k; v=wv; b=1; DQ(c, x=*u++; y=*v++; if(NE){CMP; break;}); /* make this compare fixed length? */   \
      if(b)q=k-1; else p=k+1;}
// Without misprediction.
#define BSLOOPNx(NE,CMP)    /* if CMP is true, move q; otherwise p */       \
  p=-1; q=n; k=(q-1)>>1;         \
  do{   /* empty lists handled earlier */ \
   u=av+k*c; v=wv; I _n=c; while(1){x=*u; y=*v; if(--_n==0||NE)break; ++u; ++v;};  \
   I k2, k1=k; MID(k2,k,q); k=k2-((q-p)>>1); /* calculate new values for k */\
   k=CMP?k:k2; p=CMP?p:k1; q=CMP?k1:q; /* move k first to start next fetch */\
  }while(q>p+1); 

#define BSLOOP(Ta,Tw)       \
 {Ta*av=(Ta*)AV(a),x; Tw y; Tw*wv=(Tw*)AV(w);                                             \
   if(c==1){if(ge!=1)DQ(m, BSLOOP1x(x>=y); *zv++=q;)else DQ(m, BSLOOP1x(x<=y); *zv++=q;)}      \
   else{if(ge!=1)DQ(m, Ta* u; Tw *v; BSLOOPNx(x!=y,x>=y); *zv++=q; wv+=c;)else DQ(m, Ta* u; Tw *v; BSLOOPNx(x!=y,x<=y); *zv++=q; wv+=c;)}      \
 }

#define BSLOOF(Ta,Tw,COMP)  \
 {Ta*av=(Ta*)AV(a),*u,x;                                              \
  Tw*wv=(Tw*)AV(w),*v,y;                                              \
  if(1==c) DQ(m, BSLOOP1(b=ge!=COMP(av[k],y));   *zv++=1+q;       )   \
  else     DQ(m, BSLOOPN(cc=COMP(x,y),b=gt==cc); *zv++=1+q; wv+=c;);  \
 }

// Combine two types into a single value.  Originally this was 7*s+t which produced
// large values unsuitable for a branch table, and also took advantage of the fact that
// codes produced by multiple combinations, such as LIT,B01 and B01,FL which both produce
// 1111 would not generate spurious accepted cases because only one of them is HOMO.
#define CVCASE(a,b)     (6*((0xc28c>>(a))&7)+((0xc28c>>(b))&7))   // Must distinguish 0 2 3 4 6 7 13->4 3 1 0 2 5 6  1100001010001100
#define CVCASECHAR(a,b) ((4*(0x30004>>(a))+(0x30004>>(b)))&0xf)  // distinguish character cases and SBT

// parallel implementations of I. in assembly
// currently not supported on windows because fsgsbase isn't enabled there.  But once it is: the windows abi doc says you have to always have a valid stack pointer and we clobber it; probably SEH related stuff or something?  Do we have to actually do that?
#if C_AVX512 && C_FSGSBASE
#define FAST_IIX
#define IIXIA 4 //iix on ints alignemnt.  Update below code if this exceeds 8
__attribute__((sysv_abi)) void jtiixi_1a(I*z,I*a,I*w,I n,I m,I nct),jtiixi_1d(I*,I*,I*,I,I,I);
#define IIXFA 6 //iix on floats alignment
__attribute__((sysv_abi)) void jtiixf_1a (I*,D*,D*,I,I,I),jtiixf_1d (I*,D*,D*,I,I,I);
__attribute__((sysv_abi)) void jtiixif_1a(I*,I*,D*,I,I,I),jtiixif_1d(I*,I*,D*,I,I,I);
__attribute__((sysv_abi)) void jtiixfi_1a(I*,D*,I*,I,I,I),jtiixfi_1d(I*,D*,I*,I,I,I);
#define RNDDN(x,y) ((-1)+x-(x-1)%y) //x-x%y, unless x is a multiple of y, in which case it's x-y
#define ITERCT(x) (64-__builtin_ia32_lzcnt_u64(x)) // 2 >.@^. 1+x.  In other words: the #bisections we will need for an array of length x (which has 1+x potential insertion points)
// assumes c==1 for now
// todo less copy-paste below
static void jtiixi(I*z,I*a,I*w,I n,I m,I c,B ge){I k,p,q;C x,y;
 if(m<=IIXIA){ // if w/z is too small, copy out the elements into a separate buffer.  We then _underread_ from w (doing redundant work--ok), which should always be ok so long as IIXIA is <8
               // we also take this path for m=IIXIA, to save a branch below
  I tz[IIXIA]; // the previous version of this code saved the data before z, underwrote, and then restored it, but I decided that was too ugly
  if(ge!=1)jtiixi_1a(tz,a,w+m-IIXIA,n,IIXIA,ITERCT(n));else jtiixi_1d(tz,a,w+m-IIXIA,n,IIXIA,ITERCT(n));
  MCIS(z,tz+IIXIA-m,m);
  R;}
 if(ge!=1){
  jtiixi_1a(z+m-IIXIA,a,w+m-IIXIA,n,IIXIA,ITERCT(n)); // process last batch
  jtiixi_1a(z,a,w,n,RNDDN(m,IIXIA),ITERCT(n));}       // process first ~m batches
 else{
  jtiixi_1d(z+m-IIXIA,a,w+m-IIXIA,n,IIXIA,ITERCT(n)); // (we process the last batch first for better locality in w, because these routines go back-to-front)
  jtiixi_1d(z,a,w,n,RNDDN(m,IIXIA),ITERCT(n));}}
static void jtiixf(I*z,D*a,D*w,I n,I m,I c,B ge){I k,p,q;C x,y;
 if(m<=IIXFA){
  I tz[IIXFA];D tw[IIXFA]={0}; // unlike iixi, we have to copy out the elements of w, because underreading might yield nan, which would be a problem
  MCIS((I*)tw,(I*)w,m);
  if(ge!=1)jtiixf_1a(tz,a,tw,n,IIXFA,ITERCT(n));else jtiixf_1d(tz,a,tw,n,IIXFA,ITERCT(n));
  MCIS(z,tz,m);
  R;}
 if(ge!=1){
  jtiixf_1a(z+m-IIXFA,a,w+m-IIXFA,n,IIXFA,ITERCT(n)); // process last batch
  jtiixf_1a(z,a,w,n,RNDDN(m,IIXFA),ITERCT(n));}            // process first ~m batches
 else{
  jtiixf_1d(z+m-IIXFA,a,w+m-IIXFA,n,IIXFA,ITERCT(n)); // we process the last batch first because these routines go back-to-front
  jtiixf_1d(z,a,w,n,RNDDN(m,IIXFA),ITERCT(n));}}
static void jtiixif(I*z,I*a,D*w,I n,I m,I c,B ge){I k,p,q;C x,y;
 if(m<=IIXFA){
  I tz[IIXFA];D tw[IIXFA]={0};
  MCIS((I*)tw,(I*)w,m);
  if(ge!=1)jtiixif_1a(tz,a,tw,n,IIXFA,ITERCT(n));else jtiixif_1d(tz,a,tw,n,IIXFA,ITERCT(n));
  MCIS(z,tz,m);
  R;}
 if(ge!=1){
  jtiixif_1a(z+m-IIXFA,a,w+m-IIXFA,n,IIXFA,ITERCT(n));
  jtiixif_1a(z,a,w,n,RNDDN(m,IIXFA),ITERCT(n));}
 else{
  jtiixif_1d(z+m-IIXFA,a,w+m-IIXFA,n,IIXFA,ITERCT(n));
  jtiixif_1d(z,a,w,n,RNDDN(m,IIXFA),ITERCT(n));}}
static void jtiixfi(I*z,D*a,I*w,I n,I m,I c,B ge){I k,p,q;C x,y;
 if(m<=IIXFA){
  I tz[IIXFA];
  if(ge!=1)jtiixfi_1a(tz,a,w+m-IIXFA,n,IIXFA,ITERCT(n));else jtiixfi_1d(tz,a,w+m-IIXFA,n,IIXFA,ITERCT(n));
  MCIS(z,tz+IIXFA-m,m);
  R;}
 if(ge!=1){
  jtiixfi_1a(z+m-IIXFA,a,w+m-IIXFA,n,IIXFA,ITERCT(n));
  jtiixfi_1a(z,a,w,n,RNDDN(m,IIXFA),ITERCT(n));}
 else{
  jtiixfi_1d(z+m-IIXFA,a,w+m-IIXFA,n,IIXFA,ITERCT(n));
  jtiixfi_1d(z,a,w,n,RNDDN(m,IIXFA),ITERCT(n));}}
#endif //C_AVX2 && SY_LINUX

// x I. y
DF2(jticap2){A*av,*wv,z;C*uu,*vv;I ar,*as,at,b,c,ck,cm,ge,gt,j,k,m,n,p,q,r,t,wr,*ws,wt,* RESTRICT zv;I cc;
 ARGCHK2(a,w);
 ar=AR(a); at=AT(a); as=AS(a); SETIC(a,n); r=ar-((UI)ar>0);  // n=length of 1-cell of a, r=frame of a
 wr=AR(w); wt=AT(w); ws=AS(w);
 ASSERT(r<=wr,EVRANK);
 ASSERTAGREE(as+ar-r,ws+wr-r,r)
 ASSERT((POSIFHOMO(at,wt)&-AN(a)&-AN(w))>=0,EVINHOMO); ASSERT(!ISSPARSE(at|wt),EVNONCE); // if no empties, verify agreement & non-sparse
 CPROD(AN(w),m,wr-r,ws); CPROD(AN(w),c,r,ws+wr-r);  // m=#atoms in result   c=# atoms in a cell of w
 GATV(z,INT,m,wr-r,ws); zv=AVn(wr-r,z);
 if(((m-1)|(n-1)|(c-1))<0){DQ(m, *zv++=0;); R z;}  // exit with zeros for empty args
 t=maxtyped(at,wt);
 if(1==c){  // the search is for atoms
  if(at&B01&&wt&B01+INT+FL){RZ(iixBX(n,m,a,w,zv)); R z;}
  if(at&wt&INT&0){
   // Integer search.  check for small-range
   // TUNE: given b=: 1e6?@$1e9, check tm=. {{ 1e9*1e6%~timex'a I.b' [ a=: /:~y?@$1e9 }}"0] 2^i.30  ns per lookup
   // and eg (i.#tm),.tm  and  2%~/\tm etc.  Use cmptm in place of timex for more reliable results
   // I haven't actually tuned this; new I. invalidated the previous tuning, but partitioning w will invalidate it again.  Hence the &0 above disabling this path.  Current code is damn fast when a is small enough for this to matter, so minimal loss
   // (above is also a good way of benchmarking the routine in general at various points at the cache hierarchy; check also float char matrix ...)
   UI r=IAV(a)[n-1]-IAV(a)[0]; r=IAV(a)[n-1]<IAV(a)[0]?0-r:r;  // get range, which may overflow I but will stay within UI
   UI4 nlg=CTLZI(n); nlg=(nlg<<1)+(SGNTO0(SGNIF((n<<1),nlg)));   // approx lg with 1 bit frac precision.  Can't shift 64 bits in case r=1
   if((I)((r>>2)+2*n)<(I)(m*nlg)){RZ(iixI(n,m,a,w,zv)); R z;}  // weight misbranches as equiv to 8 stores
  }
 }
 cc=0; uu=CAV(a); vv=CAV(a)+(c*(n-1)<<bplg(at));
 // first decide if the input array is ascending or descending, comparing the first & last items atom by atom
 switch(CTTZ(at)){
 case INTX:  COMPVLOOP(I, c);           break;
 default:   ASSERT(0,EVNONCE);
 case B01X:  COMPVLOOP(B, c);           break;
 case LITX:  COMPVLOOP(UC,c);           break;
 case FLX:   COMPVLOOP(D, c);           break;
 case QPX:
 case CMPXX: COMPVLOOP(D, c+c);         break;
 case C2TX:  COMPVLOOP(US,c);           break;
 case C4TX:  COMPVLOOP(C4,c);           break;
 case INT2X: COMPVLOOP(I2,c);           break;
 case INT4X: COMPVLOOP(I4,c);           break;
 case SBTX:  COMPVLOOF(SB,c, SBCOMP  ); break;
 case XNUMX: COMPVLOOF(X, c, xcompare); break;
 case RATX:  COMPVLOOF(Q, c, qcompare); break;
 case BOXX:  
  av=AAV(a); wv=AAV(w);
  DO(c, if(cc=compare(C(av[i]),C(av[i+c*(n-1)])))break;);
 }
 ge=cc; gt=-ge;
 if(unlikely(t&JCHAR+SBT)){
  switch(CVCASECHAR(CTTZ(at),CTTZ(wt))){
  case CVCASECHAR(LITX, C2TX): BSLOOP(UC,US); break;
  case CVCASECHAR(LITX, C4TX): BSLOOP(UC,C4); break;
  case CVCASECHAR(LITX, LITX): BSLOOP(UC,UC); break;
  case CVCASECHAR(C2TX, C2TX): BSLOOP(US,US); break;
  case CVCASECHAR(C2TX, C4TX): BSLOOP(US,C4); break;
  case CVCASECHAR(C2TX, LITX): BSLOOP(US,UC); break;
  case CVCASECHAR(C4TX, C2TX): BSLOOP(C4,US); break;
  case CVCASECHAR(C4TX, C4TX): BSLOOP(C4,C4); break;
  case CVCASECHAR(C4TX, LITX): BSLOOP(C4,UC); break;
  case CVCASECHAR(SBTX, SBTX): BSLOOF(SB,SB, SBCOMP); break;
  default:   ASSERT(0,EVNONCE);
  }
 }else if(unlikely(t&BOX)){
   for(j=0,cm=c*m;j<cm;j+=c){
    p=0; q=n-1;
    while(p<=q){
     MID(k,p,q); ck=c*k; I b=1; 
     DO(c, if(cc=compare(C(av[i+ck]),C(wv[i+j]))){b=gt==cc; break;});
     if(b)q=k-1; else p=k+1;
    } 
    *zv++=1+q;
   }
 }else{
  // loop on the argument types.  We handle the plausible combinations without requiring a conversion
  switch(CVCASE(CTTZ(at),CTTZ(wt))){
  case CVCASE(B01X, B01X ): BSLOOP(C, C); break;
  case CVCASE(B01X, INTX ): BSLOOP(C, I); break;
  case CVCASE(B01X, FLX  ): BSLOOP(C, D); break;
  case CVCASE(INTX, B01X ): BSLOOP(I, C); break;
#ifndef FAST_IIX
  case CVCASE(INTX, INTX ): BSLOOP(I, I); break;
  case CVCASE(INTX, FLX  ): BSLOOP(I, D); break;
  case CVCASE(FLX,  INTX ): BSLOOP(D, I); break;
  case CVCASE(QPX,QPX):
  case CVCASE(CMPXX,CMPXX): c+=c;  /* fall thru */
  case CVCASE(FLX,  FLX  ): BSLOOP(D, D); break;
#else
  case CVCASE(INTX, INTX ): if(c==1){jtiixi (zv, AV(a), AV(w),n,m,c,ge);}else{BSLOOP(I, I);} break;
  case CVCASE(INTX, FLX  ): if(c==1){jtiixif(zv, AV(a),DAV(w),n,m,c,ge);}else{BSLOOP(I, D);} break;
  case CVCASE(FLX,  INTX ): if(c==1){jtiixfi(zv,DAV(a), AV(w),n,m,c,ge);}else{BSLOOP(D, I);} break;
  case CVCASE(FLX,  FLX  ):
  if(c==1){jtiixf(zv,DAV(a),DAV(w),n,m,c,ge);}
  else{
   if(0)case CVCASE(QPX,QPX): case CVCASE(CMPXX,CMPXX):c+=c;  // fallthru.  Lovely spaghetti
   BSLOOP(D, D);}
  break;
#endif
  case CVCASE(FLX,  B01X ): BSLOOP(D, C ); break;
  case CVCASE(XNUMX,XNUMX): BSLOOF(X, X, xcompare); break;
  case CVCASE(RATX, RATX ): BSLOOF(Q, Q, qcompare); break;
  default:
   // the combination was not handled.  Convert if necessary.  INT2,INT2 and INT4,INT4 come through here
   if(TYPESNE(t,at))RZ(a=cvt(t,a));
   if(TYPESNE(t,wt))RZ(w=cvt(t,w));
   switch(CTTZ(t)){
   case QPX:
   case CMPXX: c+=c;  /* fall thru */ 
   case FLX:   BSLOOP(D,D);           break;
   case XNUMX: BSLOOF(X,X, xcompare); break;
   case RATX:  BSLOOF(Q,Q, qcompare); break;
   case INT2X:  BSLOOP(I2,I2); break;
   case INT4X:  BSLOOP(I4,I4); break;
   default:   ASSERT(0,EVNONCE);
   }
  }
 }
 RETF(z);
}    /* a I. w */
