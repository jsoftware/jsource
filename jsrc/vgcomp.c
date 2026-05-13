/* Copyright (c) 1990-2026, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Grade -- Compare                                                 */

#include "j.h"
#include "vg.h"

#define PREFLT(a,b) ((a)>(b))
#define PREFNE(a,b) ((a)!=(b))
#define PREFLTZ(a,b) (a.re>b.re || (a.re==b.re && a.im>b.im))
#define PREFNEZ(a,b) (a.re!=b.re) || (a.im!=b.im)
#define PREFLTE(a,b) (a.hi>b.hi || (a.hi==b.hi && a.lo>b.lo))
#define PREFNEE(a,b) (a.hi!=b.hi) || (a.lo!=b.lo)
// the TAO routines are called outside of sort & thus may have a=b, which allows fast exit
#define TAOGRADE(nm,T,lt,ne) I nm(I n, T *a, T *b){if(a!=b)DO(n, if(ne(a[i],b[i])) R lt(a[i],b[i])-lt(b[i],a[i]);) R 0;}
TAOGRADE(taoc,UC,PREFLT,PREFNE) TAOGRADE(taoi,I,PREFLT,PREFNE) TAOGRADE(taou,US,PREFLT,PREFNE) TAOGRADE(taot,C4,PREFLT,PREFNE) TAOGRADE(taos,I2,PREFLT,PREFNE) TAOGRADE(taol,I4,PREFLT,PREFNE)
TAOGRADE(taof,D,PREFLT,PREFNE) TAOGRADE(taoz,Z,PREFLTZ,PREFNEZ) TAOGRADE(taoe,E,PREFLTE,PREFNEE)
#define TAOGRADEC(nm,T,t) I nm(I n, T *a, T *b, J jt){I comp=0; if(a!=b)DO(n, if(comp=t(a[i],b[i]))break;) R comp;}
TAOGRADEC(taor,A,compare) TAOGRADEC(taox,A,xcompare) TAOGRADEC(taoq,Q,QCOMP)

I (*taocomproutine[])()=   // routines for different datatypes. index is [precisionx]
{[B01X]=taoc, [LITX]=taoc, [INTX]=taoi, [FLX]=taof,
[CMPXX]=taoz,[BOXX]=taor, [XNUMX]=taox, [RATX]=taoq,
[QPX]=taoe,
[C2TX]=taou, [C4TX]=taot, [SBTX]=taol,
[INT2X]=taos, [INT4X]=taol,
};

// a 16!:0 w   compare 2 nouns that have the same shape and type, returning -1/0/1
F2(jttao){F12IP;
 ARGCHK2(a,w)
 ASSERT(!(ISSPARSE(AT(a)|AT(w))),EVNONCE)
 ASSERT(AR(a)==AR(w),EVRANK);
 ASSERTAGREE(AS(a),AS(w),AR(a))  // rank and shape must agree
 ASSERT(AT(a)==AT(w),EVINHOMO)  // types must be equal
 R sc((*taocomproutine[CTTZI(AT(a))])(AN(a),voidAV(a),voidAV(w),jt));  // return 
}

// return 1 if a before b, 0 otherwise.  These routines are called only in sort and do not encounter a=b
// inlinable functions are moved to vg.c
// functions differing between merge & sort are moved to those modules
B compcu(I n, UC *a, UC *b){do{if(*a!=*b)R *a<*b; if(!--n)break; ++a; ++b;}while(1); R a<b;}
// interesting but no faster B compcu(I n, UC *a, UC *b){I lsh=(-n&(SZI-1))<<LGSZI; do{UI ai=*(UI*)a; UI bi=*(UI*)b; I sh=(n-SZI)<=0?lsh:0; UI comp=(ai^bi)<<sh; if(comp){sh+=BW-SZI-(CTTZI(comp)&-SZI); R (ai<<sh)<(bi<<sh);} if((n=n-SZI)<=0)break; a+=SZI; b+=SZI;}while(1); R a<b;}
B compcd(I n, UC *a, UC *b){do{if(*a!=*b)R *a>*b; if(!--n)break; ++a; ++b;}while(1); R a<b;}
B compuu(I n, US *a, US *b){do{if(*a!=*b)R *a<*b; if(!--n)break; ++a; ++b;}while(1); R a<b;}
B compud(I n, US *a, US *b){do{if(*a!=*b)R *a>*b; if(!--n)break; ++a; ++b;}while(1); R a<b;}
B comptu(I n, C4 *a, C4 *b){do{if(*a!=*b)R *a<*b; if(!--n)break; ++a; ++b;}while(1); R a<b;}
B comptd(I n, C4 *a, C4 *b){do{if(*a!=*b)R *a>*b; if(!--n)break; ++a; ++b;}while(1); R a<b;}
B compsu(I n, I2 *a, I2 *b){do{if(*a!=*b)R *a<*b; if(!--n)break; ++a; ++b;}while(1); R a<b;}
B compsd(I n, I2 *a, I2 *b){do{if(*a!=*b)R *a>*b; if(!--n)break; ++a; ++b;}while(1); R a<b;}
B complu(I n, I4 *a, I4 *b){do{if(*a!=*b)R *a<*b; if(!--n)break; ++a; ++b;}while(1); R a<b;}
B compld(I n, I4 *a, I4 *b){do{if(*a!=*b)R *a>*b; if(!--n)break; ++a; ++b;}while(1); R a<b;}
B compr(I n, A *a, A *b){SORT *sbk=(SORT *)n; I j; n=sbk->n; J jt=sbk->jt; do{if(j=compare(C(*a),C(*b)))R SGNTO0(j^SGNIF(jt,JTDESCENDX)); if(!--n)break; ++a; ++b;}while(1); R a<b;}  // compare returns 1/0/-1 value, switch if descending a<b makes the sort stable
B compxu(I n, X *a, X *b){SORT *sbk=(SORT *)n; I j; n=sbk->n; J jt=(J)((I)sbk->jt&~JTFLAGMSK); do{if(j=xcompare(*a,*b))R SGNTO0(j); if(!--n)break; ++a; ++b;}while(1); R a<b;} // xcompare returns 1/0/-1
B compxd(I n, X *a, X *b){SORT *sbk=(SORT *)n; I j; n=sbk->n; J jt=(J)((I)sbk->jt&~JTFLAGMSK); do{if(j=xcompare(*b,*a))R SGNTO0(j); if(!--n)break; ++a; ++b;}while(1); R a<b;} // xcompare returns 1/0/-1
B compqu(I n, Q *a, Q *b){SORT *sbk=(SORT *)n; I j; n=sbk->n; J jt=(J)((I)sbk->jt&~JTFLAGMSK); do{if(j=QCOMP(*a,*b))R SGNTO0(j); if(!--n)break; ++a; ++b;}while(1); R a<b;} // QCOMP returns value with same sign as *a-*b
B compqd(I n, Q *a, Q *b){SORT *sbk=(SORT *)n; I j; n=sbk->n; J jt=(J)((I)sbk->jt&~JTFLAGMSK); do{if(j=QCOMP(*b,*a))R SGNTO0(j); if(!--n)break; ++a; ++b;}while(1); R a<b;} // QCOMP returns value with same sign as *a-*b


#define CF(f)            B f(SORT * RESTRICT sortblok,I a,I b)

// if expr true (1), return 1; if 0, return -1
#define RETGT(x) (((x)<<1)-1) 

#define COMPDCLQ(T)      T*x=(T*)av,*y=(T*)wv
#define COMPDCLS(T)      T*x=(T*)SBAV(a),*y=(T*)SBAV(w)
#define COMPLOOQ(T,m)    {COMPDCLQ(T); if(x!=y)DO(m, if(x[i]!=y[i])R RETGT(x[i]>y[i]);)}
#define COMPLOOS(T,m)    {COMPDCLS(T); if(x!=y)DO(m, if(SBNE(x[i],y[i]))R RETGT(SBGT(x[i],y[i])););}
#define COMPLOOQG(T,m,f) {COMPDCLQ(T);I j; if(x!=y)DO(m, if(j=f(x[i],y[i]))R RETGT(j>0););}

// this is used by routines outside of sort/merge & therefore a & w can be dissimilar
// jt has the JTDESCEND flag
I jtcompare(J jt,A a,A w){C*av,*wv;I c,d,j,m,t;F1PREFJT;
 ARGCHK2(a,w); 
 I an=AN(a), wn=AN(w); I at=an?AT(a):B01, wt=wn?AT(w):B01; t=maxtyped(at,wt);  // types & common type
 if(unlikely(!HOMO(at,wt))){t=(at>>SBTX)&1; t=wt&JCHAR?0:t; t=at&JCHAR?1:t; t=wt&BOX?0:t; t=at&BOX?1:t; R RETGT(t);}  // inhomogeneous: priority is BOX/CHAR/SYMBOL/NUM
 I ar=AR(a), wr=AR(w); if(unlikely(ar!=wr))R RETGT(ar>wr);  // unequal ranks, higher rank is bigger
 I *as=AS(a), *ws=AS(w);  // pointers to shapes
 // If the shapes are different, we compare leading items of the first cell (if all are equal, the longer arg comes last).  The item of the cell is the largest one where args have the same shape.
 I negifaislong=0;  // negative if a has the longer cell, 0 if all ranks =, neg if w longer.  Init to 0 in case atoms
 m=1; DQ(ar, m*=MIN(as[i],ws[i]); if((negifaislong=ws[i]-as[i])!=0)break;)  // count size of largest cell whose items are the same shape
 if(unlikely(TYPESNE(t,at)))RZ(a=cvt(t,a));  // convert to common types
 if(unlikely(TYPESNE(t,wt)))RZ(w=cvt(t,w));
 av=CAV(a); wv=CAV(w);
 switch(CTTZ(t)){
 case INTX:  COMPLOOQ (I, m  );         break;
 default:   COMPLOOQ (UC,m  );         break;
 case C2TX:  COMPLOOQ (US,m  );         break;
 case C4TX:  COMPLOOQ (C4,m  );         break;
 case SBTX:  COMPLOOS (SB,m  );         break;
 case FLX:   COMPLOOQ (D, m  );         break;
 case QPX:
 case CMPXX: COMPLOOQ (D, m+m);         break;
 case XNUMX: COMPLOOQG(X, m, xcompare); break;
 case RATX:  COMPLOOQG(Q, m, QCOMP   ); break;
 case INT2X:  COMPLOOQ (I2, m  );         break;
 case INT4X:  COMPLOOQ (I4, m  );         break;
 case BOXX:  {COMPDCLQ(A);I j; STACKCHKOFL if(x!=y)DO(m, if(j=jtcompare(jtfg,x[i],y[i]))R j;);} break;
 }
 // falling though, we must have equality on the compared part
 R SGNTO0(negifaislong)-SGNTO0(-negifaislong);  // the longer is higher, 0 if equal length
}    /* compare 2 arbitrary dense arrays as by a-w; _1 0 1 per a<w, a=w, a>w */


#define COMPSPSS(f,T,e1init,esel)  \
 CF(f){I c,ia,ib,na,nb,p,*tv,wf,xc,*ya,*yb,yc,*yv;T e,e1,*xa,*xb,*xv;SORTSP * RESTRICT spblok=sortblok->sp;    \
  e=*(T*)spblok->sev; e1=e1init;                                                                         \
  wf=spblok->swf; tv=spblok->stv;                                                                        \
  yv=spblok->syv+wf+1; yc=spblok->syc; p=yc-1-wf;                                                        \
  xv=(T*)spblok->sxv; xc=spblok->sxc;                                                                    \
  ia=tv[a]; na=tv[1+a]; xa=xv+xc*ia;                                                                     \
  ib=tv[b]; nb=tv[1+b]; xb=xv+xc*ib;                                                                     \
  while(1){                                                                                              \
   switch((ia<na?2:0)+(I )(ib<nb)){                                                                          \
   case 0: R a<b;                                                                                  \
   case 1: c= 1; break;                                                                                 \
   case 2: c=-1; break;                                                                                 \
   case 3: c= 0; ya=yv+yc*ia; yb=yv+yc*ib; DO(p, if(c=ya[i]-yb[i]){c=0>c?-1:1; break;});                \
   }                                                                                                     \
   switch(c){                                                                                            \
   case -1: DO(xc, if(xa[i] <(esel))R (int)((~(I)sortblok->jt>>JTDESCENDX)&1); else if(xa[i] >(esel))R (int)(((I)sortblok->jt>>JTDESCENDX)&1);); xa+=xc; ++ia;               break;  \
   case  1: DO(xc, if((esel)<xb[i] )R (int)((~(I)sortblok->jt>>JTDESCENDX)&1); else if((esel)>xb[i] )R (int)(((I)sortblok->jt>>JTDESCENDX)&1););               xb+=xc; ++ib; break;  \
   case  0: DO(xc, if(xa[i] <xb[i] )R (int)((~(I)sortblok->jt>>JTDESCENDX)&1); else if(xa[i] >xb[i] )R (int)(((I)sortblok->jt>>JTDESCENDX)&1);); xa+=xc; ++ia; xb+=xc; ++ib;         \
 }}}

#define COMPSPDS(f,T,e1init,esel)  \
 CF(f){I c,ia,ib,n,na,nb,p,*tv,xc,*ya,*yb,yc,*yv;T e,e1,*xa,*xb,*xv;SORTSP * RESTRICT spblok=sortblok->sp;     \
  tv=spblok->stv;                                                                                        \
  e=*(T*)spblok->sev; e1=e1init;                                                                         \
  tv=spblok->stv; n=sortblok->n;                                                                           \
  yv=spblok->syv+1;                yc=spblok->syc; p=yc-1;                                               \
  xv=n*spblok->si+(T*)spblok->sxv; xc=spblok->sxc;                                                       \
  ia=tv[a]; na=tv[1+a]; xa=xv+xc*ia;                                                                     \
  ib=tv[b]; nb=tv[1+b]; xb=xv+xc*ib;                                                                     \
  while(1){                                                                                              \
   switch((ia<na?2:0)+(I )(ib<nb)){                                                                          \
   case 0: R a<b;                                                                                  \
   case 1: c= 1; break;                                                                                 \
   case 2: c=-1; break;                                                                                 \
   case 3: c= 0; ya=yv+yc*ia; yb=yv+yc*ib; DO(p, if(c=ya[i]-yb[i]){c=0>c?-1:1; break;});                \
   }                                                                                                     \
   switch(c){                                                                                            \
   case -1: DO(n, if(xa[i] <(esel))R (int)((~(I)sortblok->jt>>JTDESCENDX)&1); else if(xa[i] >(esel))R (int)(((I)sortblok->jt>>JTDESCENDX)&1);); xa+=xc; ++ia;               break;   \
   case  1: DO(n, if((esel)<xb[i] )R (int)((~(I)sortblok->jt>>JTDESCENDX)&1); else if((esel)>xb[i] )R (int)(((I)sortblok->jt>>JTDESCENDX)&1););               xb+=xc; ++ib; break;   \
   case  0: DO(n, if(xa[i] <xb[i] )R (int)((~(I)sortblok->jt>>JTDESCENDX)&1); else if(xa[i] >xb[i] )R (int)(((I)sortblok->jt>>JTDESCENDX)&1);); xa+=xc; ++ia; xb+=xc; ++ib;          \
 }}}

COMPSPDS(compspdsB,B,0,                   (e1,e)       )
COMPSPDS(compspdsI,I,0,                   (e1,e)       )
COMPSPDS(compspdsD,D,0,                   (e1,e)       )
COMPSPDS(compspdsZ,D,*(1+(D*)spblok->sev),(i&1)?e1:e)

COMPSPSS(compspssB,B,0,                   (e1,e)       )
COMPSPSS(compspssI,I,0,                   (e1,e)       )
COMPSPSS(compspssD,D,0,                   (e1,e)       )
COMPSPSS(compspssZ,D,*(1+(D*)spblok->sev),(i&1)?e1:e)
