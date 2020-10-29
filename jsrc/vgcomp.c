/* Copyright 1990-2004, Jsoftware Inc.  All rights reserved.               */
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
B compr(I n, A *a, A *b){J jt=(J)n; I j; n=jt->workareas.compare.compn; do{if(j=compare(*a,*b))R SGNTO0(j); if(!--n)break; ++a; ++b;}while(1); R a<b;}  // compare returns compgt value. a<b makes the sort stable
B compxu(I n, X *a, X *b){J jt=(J)n; I j; n=jt->workareas.compare.compn; do{if(j=xcompare(*a,*b))R SGNTO0(j); if(!--n)break; ++a; ++b;}while(1); R a<b;} // xcompare returns 1/0/-1
B compxd(I n, X *a, X *b){J jt=(J)n; I j; n=jt->workareas.compare.compn; do{if(j=xcompare(*b,*a))R SGNTO0(j); if(!--n)break; ++a; ++b;}while(1); R a<b;} // xcompare returns 1/0/-1
B compqu(I n, Q *a, Q *b){J jt=(J)n; I j; n=jt->workareas.compare.compn; do{if(j=QCOMP(*a,*b))R SGNTO0(j); if(!--n)break; ++a; ++b;}while(1); R a<b;} // QCOMP returns 1/0/-1
B compqd(I n, Q *a, Q *b){J jt=(J)n; I j; n=jt->workareas.compare.compn; do{if(j=QCOMP(*b,*a))R SGNTO0(j); if(!--n)break; ++a; ++b;}while(1); R a<b;} // QCOMP returns 1/0/-1
B compp(I n,I *a, I *b){J jt=(J)n; I*cv=jt->workareas.compare.compsyv; DO(jt->workareas.compare.compn, if(a[cv[i]]!=b[cv[i]])R a[cv[i]]<b[cv[i]];); R a<b;}

#define CF(f)            int f(J jt,I a,I b)

#define COMPDCLP(T)      T*x=(T*)(jt->workareas.compare.compv+a*jt->workareas.compare.compk),*y=(T*)(jt->workareas.compare.compv+b*jt->workareas.compare.compk)
#define COMPDCLQ(T)      T*x=(T*)av,*y=(T*)wv
#define COMPDCLS(T)      T*x=(T*)SBAV(a),*y=(T*)SBAV(w)
#define COMPLOOP(T,m)    {COMPDCLP(T); DO(m, if(x[i]!=y[i])R jt->workareas.compare.complt*((x[i]>y[i])?-1:1);)}
#define COMPLOOQ(T,m)    {COMPDCLQ(T); DO(m, if(x[i]!=y[i])R jt->workareas.compare.complt*((x[i]>y[i])?-1:1);)}
#define COMPLOOS(T,m)    {COMPDCLS(T); DO(m, if(SBNE(x[i],y[i]))R jt->workareas.compare.complt*((SBGT(x[i],y[i]))?-1:1););}
#define COMPLOOPF(T,m,f) {COMPDCLP(T);I j; DO(m, if(j=f(x[i],y[i]))R j;);}
#define COMPLOOPG(T,m,f) {COMPDCLP(T);I j; DO(m, if(j=f(x[i],y[i]))R jt->workareas.compare.complt*((j>0)?-1:1););}
#define COMPLOOQG(T,m,f) {COMPDCLQ(T);I j; DO(m, if(j=f(x[i],y[i]))R jt->workareas.compare.complt*((j>0)?-1:1););}

// this is used by routines outside of sort/merge & therefore a & w can be dissimilar
I jtcompare(J jt,A a,A w){C*av,*wv;I ar,an,*as,at,c,d,j,m,t,wn,wr,*ws,wt;
 ARGCHK2(a,w);
 an=AN(a); at=an?AT(a):B01; ar=AR(a); as=AS(a);
 wn=AN(w); wt=wn?AT(w):B01; wr=AR(w); ws=AS(w); t=maxtyped(at,wt);
 if(unlikely(!HOMO(at,wt)))R (at&BOX?-1:wt&BOX?1:at&JCHAR?-1:wt&JCHAR?1:
                   at&SBT?-1:1)*jt->workareas.compare.complt;
 if(ar!=wr)R (ar>wr?-1:1)*jt->workareas.compare.complt;
 if(1<ar&&ICMP(1+as,1+ws,ar)){A s;I*v;fauxblockINT(sfaux,4,1);
  fauxINT(s,sfaux,ar,1) v=AV(s);
  DO(ar, v[i]=MAX(as[i],ws[i]);); v[0]=MIN(as[0],ws[0]);
  RZ(a=take(s,a)); an=wn=AN(a);
  RZ(w=take(s,w));
 }
 m=MIN(an,wn); 
 if(t&XNUM+RAT&&((at|wt)&FL+CMPX)){A p,q;B*u,*v;  // indirect type vs flt/complex: create boolean vector for each value in turn
  RZ(p=lt(a,w)); u=BAV(p);
  RZ(q=gt(a,w)); v=BAV(q);
  DO(m, if(u[i]|v[i])R (u[i]?1:-1)*jt->workareas.compare.complt;);
 }else{
  if(TYPESNE(t,at))RZ(a=cvt(t,a));
  if(TYPESNE(t,wt))RZ(w=cvt(t,w));
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
   case BOXX:  {COMPDCLQ(A);I j; DO(m, if(j=compare(        x[i],           y[i]   ))R j;);} break;
  }
 }
 if(1>=ar)R ((I )(an<wn)-(I )(an>wn))*jt->workareas.compare.complt;
 DQ(j=ar, --j; c=as[j]; d=ws[j]; if(c!=d)R ((c>d)?-1:1)*jt->workareas.compare.complt;);
 R 0;
}    /* compare 2 arbitrary dense arrays; _1 0 1 per a<w, a=w, a>w */


#define COMPSPSS(f,T,e1init,esel)  \
 CF(f){I c,ia,ib,na,nb,p,*tv,wf,xc,*ya,*yb,yc,*yv;T e,e1,*xa,*xb,*xv;    \
  e=*(T*)jt->workareas.compare.compsev; e1=e1init;                                                                         \
  wf=jt->workareas.compare.compswf; tv=jt->workareas.compare.compstv;                                                                        \
  yv=jt->workareas.compare.compsyv+wf+1; yc=jt->workareas.compare.compsyc; p=yc-1-wf;                                                        \
  xv=(T*)jt->workareas.compare.compsxv; xc=jt->workareas.compare.compsxc;                                                                    \
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
    case -1: DO(xc, if(xa[i] <(esel))R (int)(SGNTO0(jt->workareas.compare.complt)); else if(xa[i] >(esel))R (int)((jt->workareas.compare.complt+1)>>1);); xa+=xc; ++ia;               break;  \
    case  1: DO(xc, if((esel)<xb[i] )R (int)(SGNTO0(jt->workareas.compare.complt)); else if((esel)>xb[i] )R (int)((jt->workareas.compare.complt+1)>>1););               xb+=xc; ++ib; break;  \
    case  0: DO(xc, if(xa[i] <xb[i] )R (int)(SGNTO0(jt->workareas.compare.complt)); else if(xa[i] >xb[i] )R (int)((jt->workareas.compare.complt+1)>>1);); xa+=xc; ++ia; xb+=xc; ++ib;         \
 }}}

#define COMPSPDS(f,T,e1init,esel)  \
 CF(f){I c,ia,ib,n,na,nb,p,*tv,xc,*ya,*yb,yc,*yv;T e,e1,*xa,*xb,*xv;     \
  tv=jt->workareas.compare.compstv;                                                                                        \
  e=*(T*)jt->workareas.compare.compsev; e1=e1init;                                                                         \
  tv=jt->workareas.compare.compstv; n=jt->workareas.compare.compn;                                                                           \
  yv=jt->workareas.compare.compsyv+1;                yc=jt->workareas.compare.compsyc; p=yc-1;                                               \
  xv=n*jt->workareas.compare.compsi+(T*)jt->workareas.compare.compsxv; xc=jt->workareas.compare.compsxc;                                                       \
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
    case -1: DO(n, if(xa[i] <(esel))R (int)(SGNTO0(jt->workareas.compare.complt)); else if(xa[i] >(esel))R (int)((jt->workareas.compare.complt+1)>>1);); xa+=xc; ++ia;               break;   \
    case  1: DO(n, if((esel)<xb[i] )R (int)(SGNTO0(jt->workareas.compare.complt)); else if((esel)>xb[i] )R (int)((jt->workareas.compare.complt+1)>>1););               xb+=xc; ++ib; break;   \
    case  0: DO(n, if(xa[i] <xb[i] )R (int)(SGNTO0(jt->workareas.compare.complt)); else if(xa[i] >xb[i] )R (int)((jt->workareas.compare.complt+1)>>1);); xa+=xc; ++ia; xb+=xc; ++ib;          \
 }}}

COMPSPDS(compspdsB,B,0,                   e       )
COMPSPDS(compspdsI,I,0,                   e       )
COMPSPDS(compspdsD,D,0,                   e       )
COMPSPDS(compspdsZ,D,*(1+(D*)jt->workareas.compare.compsev),(i&1)?e1:e)

COMPSPSS(compspssB,B,0,                   e       )
COMPSPSS(compspssI,I,0,                   e       )
COMPSPSS(compspssD,D,0,                   e       )
COMPSPSS(compspssZ,D,*(1+(D*)jt->workareas.compare.compsev),(i&1)?e1:e)
