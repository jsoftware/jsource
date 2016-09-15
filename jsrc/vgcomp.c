/* Copyright 1990-2004, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Grade -- Compare                                                 */
/* the result of a CF comparison fn is 1 if (a{w)>(b{w), -1 otherwise      */

#include "j.h"
#include "vg.h"


#define CF(f)            int f(J jt,I a,I b)

#define COMPDCLP(T)      T*x=(T*)(jt->compv+a*jt->compk),*y=(T*)(jt->compv+b*jt->compk)
#define COMPDCLQ(T)      T*x=(T*)av,*y=(T*)wv
#define COMPDCLS(T)      T*x=(T*)SBAV(a),*y=(T*)SBAV(w)
#define COMPLOOP(T,m)    {COMPDCLP(T); DO(m, if(x[i]>y[i])R jt->compgt; else if(x[i]<y[i])R jt->complt;);}
#define COMPLOOQ(T,m)    {COMPDCLQ(T); DO(m, if(x[i]>y[i])R jt->compgt; else if(x[i]<y[i])R jt->complt;);}
#define COMPLOOS(T,m)    {COMPDCLS(T); DO(m, if(SBGT(x[i],y[i]))R jt->compgt; else if(SBLT(x[i],y[i]))R jt->complt;);}
#define COMPLOOPF(T,m,f) {COMPDCLP(T);int j; DO(m, if(j=f(x[i],y[i]))R j;);}
#define COMPLOOPR(T,m,f) {COMPDCLP(T);int j; DO(m, if(j=f((A)AABS(x[i],jt->compw),(A)AABS(y[i],jt->compw)))R j;);}
#define COMPLOOPG(T,m,f) {COMPDCLP(T);int j; DO(m, if(j=f(x[i],y[i]))R 0<j?jt->compgt:jt->complt;);}
#define COMPLOOQG(T,m,f) {COMPDCLQ(T);int j; DO(m, if(j=f(x[i],y[i]))R 0<j?jt->compgt:jt->complt;);}

CF(compc){COMPLOOP (UC,jt->compn);          R a>b?1:-1;}
CF(compu){COMPLOOP (US,jt->compn);          R a>b?1:-1;}
CF(compt){COMPLOOP (C4,jt->compn);          R a>b?1:-1;}
CF(compi){COMPLOOP (I, jt->compn);          R a>b?1:-1;}
CF(compd){COMPLOOP (D, jt->compn);          R a>b?1:-1;}
CF(compa){COMPLOOPF(A, jt->compn,compare ); R a>b?1:-1;}
CF(compr){COMPLOOPR(A1,jt->compn,compare ); R a>b?1:-1;}
CF(compx){COMPLOOPG(X, jt->compn,xcompare); R a>b?1:-1;}
CF(compq){COMPLOOPG(Q, jt->compn,QCOMP   ); R a>b?1:-1;}

CF(compt1){C4 p=*(a+(C4*)jt->compv),q=*(b+(C4*)jt->compv); R p>q?jt->compgt:p<q?jt->complt:a>b?1:-1;}
CF(compi1){I  p=*(a+ (I*)jt->compv),q=*(b+ (I*)jt->compv); R p>q?jt->compgt:p<q?jt->complt:a>b?1:-1;}
CF(compd1){D  p=*(a+ (D*)jt->compv),q=*(b+ (D*)jt->compv); R p>q?jt->compgt:p<q?jt->complt:a>b?1:-1;}

CF(compp){COMPDCLP(I);I*cv=jt->compsyv,xi,yi;
 DO(jt->compn, xi=x[cv[i]]; yi=y[cv[i]]; if(xi>yi)R 1; else if(xi<yi)R -1;);
 R a>b?1:-1;
}

int jtcompare(J jt,A a,A w){C*av,*wv;I ar,an,*as,at,c,d,j,m,t,wn,wr,*ws,wt;
 RZ(a&&w);
 an=AN(a); at=an?AT(a):B01; ar=AR(a); as=AS(a);
 wn=AN(w); wt=wn?AT(w):B01; wr=AR(w); ws=AS(w); RE(t=maxtype(at,wt));
 if(!HOMO(at,wt))R at&BOX?jt->compgt:wt&BOX?jt->complt:at&JCHAR?jt->compgt:wt&JCHAR?jt->complt:
                   at&SBT?jt->compgt:jt->complt;
 if(ar!=wr)R ar>wr?jt->compgt:jt->complt;
 if(1<ar&&ICMP(1+as,1+ws,ar)){A s;I*v;
  GATV(s,INT,ar,1,0); v=AV(s);
  DO(ar, v[i]=MAX(as[i],ws[i]);); v[0]=MIN(as[0],ws[0]);
  RZ(a=take(s,a)); an=wn=AN(a);
  RZ(w=take(s,w));
 }
 m=MIN(an,wn); 
 if(t&XNUM+RAT&&(at&FL+CMPX||wt&FL+CMPX)){A p,q;B*u,*v;
  RZ(p=lt(a,w)); u=BAV(p);
  RZ(q=gt(a,w)); v=BAV(q);
  DO(m, if(u[i])R jt->complt; else if(v[i])R jt->compgt;);
 }else{
  if(TYPESNE(t,at))RZ(a=cvt(t,a));
  if(TYPESNE(t,wt))RZ(w=cvt(t,w));
  av=CAV(a); wv=CAV(w);
  switch(CTTZ(t)){
   default:   COMPLOOQ (UC,m  );         break;
   case C2TX:  COMPLOOQ (US,m  );         break;
   case C4TX:  COMPLOOQ (C4,m  );         break;
   case SBTX:  COMPLOOS (SB,m  );         break;
   case INTX:  COMPLOOQ (I, m  );         break;
   case FLX:   COMPLOOQ (D, m  );         break;
   case CMPXX: COMPLOOQ (D, m+m);         break;
   case XNUMX: COMPLOOQG(X, m, xcompare); break;
   case RATX:  COMPLOOQG(Q, m, QCOMP   ); break;
   case BOXX:  switch(2*ARELATIVE(a)+ARELATIVE(w)){ 
    case 0: {COMPDCLQ(A);int j; DO(m, if(j=compare(        x[i],           y[i]   ))R j;);} break;
    case 1: {COMPDCLQ(A);int j; DO(m, if(j=compare(        x[i],   (A)AABS(y[i],w)))R j;);} break;
    case 2: {COMPDCLQ(A);int j; DO(m, if(j=compare((A)AABS(x[i],a),        y[i]   ))R j;);} break;
    case 3: {COMPDCLQ(A);int j; DO(m, if(j=compare((A)AABS(x[i],a),(A)AABS(y[i],w)))R j;);} break;
 }}}
 if(1>=ar)R an>wn?jt->compgt:an<wn?jt->complt:0;
 DO(j=ar, --j; c=as[j]; d=ws[j]; if(c>d)R jt->compgt; else if (c<d)R jt->complt;);
 R 0;
}    /* compare 2 arbitrary dense arrays; _1 0 1 per a<w, a=w, a>w */


#define COMPSPSS(f,T,e1init,esel)  \
 CF(f){I c,ia,ib,na,nb,p,*tv,wf,xc,*ya,*yb,yc,*yv;int gt=jt->compgt,lt=jt->complt;T e,e1,*xa,*xb,*xv;    \
  e=*(T*)jt->compsev; e1=e1init;                                                                         \
  wf=jt->compswf; tv=jt->compstv;                                                                        \
  yv=jt->compsyv+wf+1; yc=jt->compsyc; p=yc-1-wf;                                                        \
  xv=(T*)jt->compsxv; xc=jt->compsxc;                                                                    \
  ia=tv[a]; na=tv[1+a]; xa=xv+xc*ia;                                                                     \
  ib=tv[b]; nb=tv[1+b]; xb=xv+xc*ib;                                                                     \
  while(1){                                                                                              \
   switch((ia<na?2:0)+(ib<nb)){                                                                          \
    case 0: R a>b?1:-1;                                                                                  \
    case 1: c= 1; break;                                                                                 \
    case 2: c=-1; break;                                                                                 \
    case 3: c= 0; ya=yv+yc*ia; yb=yv+yc*ib; DO(p, if(c=ya[i]-yb[i]){c=0>c?-1:1; break;});                \
   }                                                                                                     \
   switch(c){                                                                                            \
    case -1: DO(xc, if(xa[i] <(esel))R lt; else if(xa[i] >(esel))R gt;); xa+=xc; ++ia;               break;  \
    case  1: DO(xc, if((esel)<xb[i] )R lt; else if((esel)>xb[i] )R gt;);               xb+=xc; ++ib; break;  \
    case  0: DO(xc, if(xa[i] <xb[i] )R lt; else if(xa[i] >xb[i] )R gt;); xa+=xc; ++ia; xb+=xc; ++ib;         \
 }}}

#define COMPSPDS(f,T,e1init,esel)  \
 CF(f){I c,ia,ib,n,na,nb,p,*tv,xc,*ya,*yb,yc,*yv;int gt=jt->compgt,lt=jt->complt;T e,e1,*xa,*xb,*xv;     \
  tv=jt->compstv;                                                                                        \
  e=*(T*)jt->compsev; e1=e1init;                                                                         \
  tv=jt->compstv; n=jt->compn;                                                                           \
  yv=jt->compsyv+1;                yc=jt->compsyc; p=yc-1;                                               \
  xv=n*jt->compsi+(T*)jt->compsxv; xc=jt->compsxc;                                                       \
  ia=tv[a]; na=tv[1+a]; xa=xv+xc*ia;                                                                     \
  ib=tv[b]; nb=tv[1+b]; xb=xv+xc*ib;                                                                     \
  while(1){                                                                                              \
   switch((ia<na?2:0)+(ib<nb)){                                                                          \
    case 0: R a>b?1:-1;                                                                                  \
    case 1: c= 1; break;                                                                                 \
    case 2: c=-1; break;                                                                                 \
    case 3: c= 0; ya=yv+yc*ia; yb=yv+yc*ib; DO(p, if(c=ya[i]-yb[i]){c=0>c?-1:1; break;});                \
   }                                                                                                     \
   switch(c){                                                                                            \
    case -1: DO(n, if(xa[i] <(esel))R lt; else if(xa[i] >(esel))R gt;); xa+=xc; ++ia;               break;   \
    case  1: DO(n, if((esel)<xb[i] )R lt; else if((esel)>xb[i] )R gt;);               xb+=xc; ++ib; break;   \
    case  0: DO(n, if(xa[i] <xb[i] )R lt; else if(xa[i] >xb[i] )R gt;); xa+=xc; ++ia; xb+=xc; ++ib;          \
 }}}

COMPSPDS(compspdsB,B,0,                   e       )
COMPSPDS(compspdsI,I,0,                   e       )
COMPSPDS(compspdsD,D,0,                   e       )
COMPSPDS(compspdsZ,D,*(1+(D*)jt->compsev),i%2?e1:e)

COMPSPSS(compspssB,B,0,                   e       )
COMPSPSS(compspssI,I,0,                   e       )
COMPSPSS(compspssD,D,0,                   e       )
COMPSPSS(compspssZ,D,*(1+(D*)jt->compsev),i%2?e1:e)
