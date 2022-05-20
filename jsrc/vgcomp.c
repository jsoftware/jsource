/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Grade -- Compare                                                 */

#include "j.h"
#include "vg.h"

// return 1 if a before b, 0 otherwise
// inlinable functions are moved to vg.c
// functions differing between merge & sort are moved to those modules
B compcu(I n, UC *a, UC *b){do{if(*a!=*b)R *a<*b; if(!--n)break; ++a; ++b;}while(1); R a<b;}
B compcd(I n, UC *a, UC *b){do{if(*a!=*b)R *a>*b; if(!--n)break; ++a; ++b;}while(1); R a<b;}
B compuu(I n, US *a, US *b){do{if(*a!=*b)R *a<*b; if(!--n)break; ++a; ++b;}while(1); R a<b;}
B compud(I n, US *a, US *b){do{if(*a!=*b)R *a>*b; if(!--n)break; ++a; ++b;}while(1); R a<b;}
B comptu(I n, C4 *a, C4 *b){do{if(*a!=*b)R *a<*b; if(!--n)break; ++a; ++b;}while(1); R a<b;}
B comptd(I n, C4 *a, C4 *b){do{if(*a!=*b)R *a>*b; if(!--n)break; ++a; ++b;}while(1); R a<b;}
B compr(I n, A *a, A *b){SORT *sbk=(SORT *)n; I j; n=sbk->n; J jt=sbk->jt; do{if(j=compare(C(*a),C(*b)))R SGNTO0(j^SGNIF(jt,JTDESCENDX)); if(!--n)break; ++a; ++b;}while(1); R a<b;}  // compare returns 1/0/-1 value, switch if descending a<b makes the sort stable
B compxu(I n, X *a, X *b){SORT *sbk=(SORT *)n; I j; n=sbk->n; J jt=(J)((I)sbk->jt&~JTFLAGMSK); do{if(j=xcompare(*a,*b))R SGNTO0(j); if(!--n)break; ++a; ++b;}while(1); R a<b;} // xcompare returns 1/0/-1
B compxd(I n, X *a, X *b){SORT *sbk=(SORT *)n; I j; n=sbk->n; J jt=(J)((I)sbk->jt&~JTFLAGMSK); do{if(j=xcompare(*b,*a))R SGNTO0(j); if(!--n)break; ++a; ++b;}while(1); R a<b;} // xcompare returns 1/0/-1
B compqu(I n, Q *a, Q *b){SORT *sbk=(SORT *)n; I j; n=sbk->n; J jt=(J)((I)sbk->jt&~JTFLAGMSK); do{if(j=QCOMP(*a,*b))R SGNTO0(j); if(!--n)break; ++a; ++b;}while(1); R a<b;} // QCOMP returns 1/0/-1
B compqd(I n, Q *a, Q *b){SORT *sbk=(SORT *)n; I j; n=sbk->n; J jt=(J)((I)sbk->jt&~JTFLAGMSK); do{if(j=QCOMP(*b,*a))R SGNTO0(j); if(!--n)break; ++a; ++b;}while(1); R a<b;} // QCOMP returns 1/0/-1

#define CF(f)            int f(SORT * RESTRICT sortblok,I a,I b)

// if expr true (1), return 1; if 0, return -1
#define RETGT(x) (((x)<<1)-1) 

#define COMPDCLQ(T)      T*x=(T*)av,*y=(T*)wv
#define COMPDCLS(T)      T*x=(T*)SBAV(a),*y=(T*)SBAV(w)
#define COMPLOOQ(T,m)    {COMPDCLQ(T); DO(m, if(x[i]!=y[i])R RETGT(x[i]>y[i]);)}
#define COMPLOOS(T,m)    {COMPDCLS(T); DO(m, if(SBNE(x[i],y[i]))R RETGT(SBGT(x[i],y[i])););}
#define COMPLOOQG(T,m,f) {COMPDCLQ(T);I j; DO(m, if(j=f(x[i],y[i]))R RETGT(j>0););}

// this is used by routines outside of sort/merge & therefore a & w can be dissimilar
// jt has the JTDESCEND flag
I jtcompare(J jt,A a,A w){C*av,*wv;I ar,an,*as,at,c,d,j,m,t,wn,wr,*ws,wt;F1PREFJT;
 ARGCHK2(a,w); 
 an=AN(a); at=an?AT(a):B01; ar=AR(a); as=AS(a);
 wn=AN(w); wt=wn?AT(w):B01; wr=AR(w); ws=AS(w); t=maxtyped(at,wt);
 if(unlikely(!HOMO(at,wt))){
  ar=(at>>SBTX)&1; ar=wt&JCHAR?0:ar; ar=at&JCHAR?1:ar; ar=wt&BOX?0:ar; ar=at&BOX?1:ar; R RETGT(ar);
 }
 if(unlikely(ar!=wr))R RETGT(ar>wr);  // unequal ranks, higher rank is bigger
 if(unlikely(1<ar))if(ICMP(1+as,1+ws,ar)){A s;I*v;fauxblockINT(sfaux,4,1);
  // dissimilar shapes - bring to common shape
  fauxINT(s,sfaux,ar,1) v=AV(s);
  DO(ar, v[i]=MAX(as[i],ws[i]);); v[0]=MIN(as[0],ws[0]);
  RZ(a=take(s,a)); an=wn=AN(a);
  RZ(w=take(s,w));
 }
 m=MIN(an,wn); 
 if(unlikely((-(t&XNUM+RAT)&-((at|wt)&FL+CMPX))<0)){A p,q;B*u,*v;
  // indirect numeric type vs flt/complex: create boolean vector for each value in turn
  RZ(p=lt(a,w)); u=BAV(p);
  RZ(q=gt(a,w)); v=BAV(q);
  DO(m, if(u[i]|v[i])R RETGT(!u[i]););
 }else{
  if(unlikely(TYPESNE(t,at)))RZ(a=cvt(t,a));
  if(unlikely(TYPESNE(t,wt)))RZ(w=cvt(t,w));
  av=CAV(a); wv=CAV(w);
  switch(CTTZ(t)){
   case INTX:  COMPLOOQ (I, m  );         break;
   default:   COMPLOOQ (UC,m  );         break;
   case C2TX:  COMPLOOQ (US,m  );         break;
   case C4TX:  COMPLOOQ (C4,m  );         break;
   case SBTX:  COMPLOOS (SB,m  );         break;
   case FLX:   COMPLOOQ (D, m  );         break;
   case CMPXX: COMPLOOQ (D, m+m);         break;
   case XNUMX: COMPLOOQG(X, m, xcompare); break;
   case RATX:  COMPLOOQG(Q, m, QCOMP   ); break;
   case BOXX:  {COMPDCLQ(A);I j; DO(m, if(j=jtcompare(jtinplace,x[i],y[i]))R j;);} break;
  }
 }
 if(1>=ar)R an==wn?0:RETGT(an>wn);   // all compared items matched.  If they weren't the same length, the longer is bigger
 DQ(ar, c=as[i]; d=ws[i]; if(c!=d)R RETGT(c>d););  // finally, compare original shape, lowest axis first
 R 0;
}    /* compare 2 arbitrary dense arrays; _1 0 1 per a<w, a=w, a>w */


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

COMPSPDS(compspdsB,B,0,                   e       )
COMPSPDS(compspdsI,I,0,                   e       )
COMPSPDS(compspdsD,D,0,                   e       )
COMPSPDS(compspdsZ,D,*(1+(D*)spblok->sev),(i&1)?e1:e)

COMPSPSS(compspssB,B,0,                   e       )
COMPSPSS(compspssI,I,0,                   e       )
COMPSPSS(compspssD,D,0,                   e       )
COMPSPSS(compspssZ,D,*(1+(D*)spblok->sev),(i&1)?e1:e)
