/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Monadic Atomic                                                   */

#include "j.h"
#include "ve.h"


static AMON(floorDI,I,D, {D d=tfloor(*x); *z=(I)d; ASSERTW(d==*z,EWOV);})
static AMON(floorD, D,D, *z=tfloor(*x);)
static AMON(floorZ, Z,Z, *z=zfloor(*x);)

static AMON(ceilDI, I,D, {D d=tceil(*x);  *z=(I)d; ASSERTW(d==*z,EWOV);})
static AMON(ceilD,  D,D, *z=tceil(*x);)
static AMON(ceilZ,  Z,Z, *z=zceil(*x);)

static AMON(cjugZ,  Z,Z, *z=zconjug(*x);)

static AMON(sgnI,   I,I, *z=SGN(*x);)
static AMON(sgnD,   I,D, *z=jt->ct>ABS(*x)?0:SGN(*x);)
static AMON(sgnZ,   Z,Z, if(jt->ct>zmag(*x))*z=zeroZ; else *z=ztrend(*x);)

static AMON(sqrtI,  D,I, ASSERTW(0<=*x,EWIMAG); *z=sqrt((D)*x);)
static AMON(sqrtD,  D,D, ASSERTW(0<=*x,EWIMAG); *z=sqrt(   *x);)
static AMON(sqrtZ,  Z,Z, *z=zsqrt(*x);)

static AMON(expB,   D,B, *z=*x?2.71828182845904523536:1;)
static AMON(expI,   D,I, *z=*x<EMIN?0.0:EMAX<*x?inf:exp((D)*x);)
static AMON(expD,   D,D, *z=*x<EMIN?0.0:EMAX<*x?inf:exp(   *x);)
static AMON(expZ,   Z,Z, *z=zexp(*x);)

static AMON(logB,   D,B, *z=*x?0:infm;)
static AMON(logI,   D,I, ASSERTW(0<=*x,EWIMAG); *z=log((D)*x);)
static AMON(logD,   D,D, ASSERTW(0<=*x,EWIMAG); *z=log(   *x);)
static AMON(logZ,   Z,Z, *z=zlog(*x);)

static AMON(absI,   I,I, if(0<=*x)*z=*x; else{ASSERTW(IMIN<*x,EWOV); *z=-*x;})
static AMON(absD,   D,D, *z= ABS(*x);)
static AMON(absZ,   D,Z, *z=zmag(*x);)

static AHDR1(oneB,C,C){memset(z,C1,n);}
static AHDR1(idf ,C,C){}  /* dummy */

static UC va1fns[]={CFLOOR, CCEIL, CPLUS, CSTAR, CSQRT, CEXP, CLOG, CSTILE, CBANG, CCIRCLE, C0};

static UA va1tab[]={
 /* <. */ {{{ idf,VB}, {  idf,VI}, {floorDI,VI}, {floorZ,VZ}, {  idf,VX}, {floorQ,VX}}},
 /* >. */ {{{ idf,VB}, {  idf,VI}, { ceilDI,VI}, { ceilZ,VZ}, {  idf,VX}, { ceilQ,VX}}},
 /* +  */ {{{ idf,VB}, {  idf,VI}, {    idf,VD}, { cjugZ,VZ}, {  idf,VX}, {   idf,VQ}}},
 /* *  */ {{{ idf,VB}, { sgnI,VI}, {   sgnD,VI}, {  sgnZ,VZ}, { sgnX,VX}, {  sgnQ,VX}}},
 /* %: */ {{{ idf,VB}, {sqrtI,VD}, {  sqrtD,VD}, { sqrtZ,VZ}, {sqrtX,VX}, { sqrtQ,VQ}}},
 /* ^  */ {{{expB,VD}, { expI,VD}, {   expD,VD}, {  expZ,VZ}, { expX,VX}, {  expD,VD+VDD}}},
 /* ^. */ {{{logB,VD}, { logI,VD}, {   logD,VD}, {  logZ,VZ}, { logX,VX}, { logQD,VD}}},
 /* |  */ {{{ idf,VB}, { absI,VI}, {   absD,VD}, {  absZ,VD}, { absX,VX}, {  absQ,VQ}}},
 /* !  */ {{{oneB,VB}, {factI,VD}, {  factD,VD}, { factZ,VZ}, {factX,VX}, { factQ,VX}}},
 /* o. */ {{{  0L,0L}, {   0L,0L}, {     0L,0L}, {    0L,0L}, { pixX,VX}, {    0L,0L}}}
};

static A jtva1(J,A,C);

static A jtva1s(J jt,A w,C id,I cv,VF ado){A e,x,z,ze,zx;B c;C ee;I n,t,zt;P*wp,*zp;
 t=atype(cv); zt=rtype(cv);
 wp=PAV(w); e=SPA(wp,e); x=SPA(wp,x); c=t&&TYPESNE(t,AT(e));
 if(c)RZ(e=cvt(t,e));          GA(ze,zt,1,0,    0    ); ado(jt,1L,AV(ze),AV(e));
 if(c)RZ(e=cvt(t,x)); n=AN(x); GA(zx,zt,n,AR(x),AS(x)); ado(jt,n, AV(zx),AV(x));
 if(jt->jerr){
  if(jt->jerr<=NEVM)R 0;
  ee=jt->jerr; RZ(ze=va1(e,id)); 
  jt->jerr=ee; RZ(zx=va1(x,id)); 
 }else if(cv&VRI+VRD){RZ(ze=cvz(cv,ze)); RZ(zx=cvz(cv,zx));}
 GA(z,STYPE(AT(ze)),1,AR(w),AS(w)); zp=PAV(z);
 SPB(zp,a,ca(SPA(wp,a)));
 SPB(zp,i,ca(SPA(wp,i)));
 SPB(zp,e,ze);
 SPB(zp,x,zx);
 R z;
}

#define VA1CASE(e,f) (256*(e)+(f))

static A jtva1(J jt,A w,C id){A e,z;B b,m;I cv,n,t,wt,zt;P*wp;VA2 p;VF ado;
 RZ(w);
 n=AN(w); wt=n?AT(w):B01;
 ASSERT(wt&NUMERIC,EVDOMAIN);
 if(b=1&&wt&SPARSE){wp=PAV(w); e=SPA(wp,e); wt=AT(e);}
 if(jt->jerr){
  m=!(wt&XNUM+RAT);
  switch(VA1CASE(jt->jerr,id)){
   default:     R 0;
   case VA1CASE(EWOV,  CFLOOR): cv=VD;       ado=floorD;               break;
   case VA1CASE(EWOV,  CCEIL ): cv=VD;       ado=ceilD;                break;
   case VA1CASE(EWOV,  CSTILE): cv=VD+VDD;   ado=absD;                 break;
   case VA1CASE(EWIRR, CSQRT ): cv=VD+VDD;   ado=sqrtD;                break;
   case VA1CASE(EWIRR, CEXP  ): cv=VD+VDD;   ado=expD;                 break;
   case VA1CASE(EWIRR, CBANG ): cv=VD+VDD;   ado=factD;                break;
   case VA1CASE(EWIRR, CLOG  ): cv=VD+VDD*m; ado=m?(VF)logD:(VF)logXD; break;
   case VA1CASE(EWIMAG,CSQRT ): cv=VZ+VZZ;   ado=sqrtZ;                break;
   case VA1CASE(EWIMAG,CLOG  ): cv=VZ+VZZ*m; ado=m?(VF)logZ:wt&XNUM?(VF)logXZ:(VF)logQZ;
  }
  RESETERR;
 }else{
  p=((va1tab+((C*)strchr(va1fns,id)-(C*)va1fns))->p1)[wt&B01?0:wt&INT?1:wt&FL?2:wt&CMPX?3:wt&XNUM?4:5];
  ado=p.f; cv=p.cv;
 }
 if(ado==idf)R rat(w);  // no need to rat, really, but harmless since always DIRECT type
 if(b)R va1s(w,id,cv,ado);
 t=atype(cv); zt=rtype(cv);
 if(t&&TYPESNE(t,wt))RZ(w=cvt(t,w));
 GA(z,zt,n,AR(w),AS(w));
 ado(jt,n,AV(z),AV(w));
 if(jt->jerr)R NEVM<jt->jerr?va1(w,id):0; 
 else    R cv&VRI+VRD?cvz(cv,z):z;
}


// If argument has a single direct-numeric atom, go process through speedy-singleton code
#define CHECKSSING(w,f) RZ(w); if(AN(w)==1 && (AT(w)&(B01+INT+FL)))R f(jt,w); F1PREFIP;
#define CHECKSSINGNZ(w,f) RZ(w); if(AN(w)==1 && (AT(w)&(B01+INT+FL))){A z = f(jt,w); if(z)R z;} F1PREFIP;  // fall through if returns 0


F1(jtfloor1){CHECKSSING(w,jtssfloor) R va1(w,CFLOOR);}
F1(jtceil1 ){CHECKSSING(w,jtssceil) R va1(w,CCEIL );}
F1(jtconjug){R va1(w,CPLUS );}
F1(jtsignum){CHECKSSING(w,jtsssignum) R va1(w,CSTAR );}
F1(jtsqroot){CHECKSSINGNZ(w,jtsssqrt) R va1(w,CSQRT );}
F1(jtexpn1 ){CHECKSSING(w,jtssexp) R va1(w,CEXP  );}
F1(jtlogar1){CHECKSSINGNZ(w,jtsslog) R va1(w,CLOG  );}
F1(jtmag   ){CHECKSSING(w,jtssmag) R va1(w,CSTILE);}
F1(jtfact  ){CHECKSSING(w,jtssfact) R va1(w,CBANG );}
F1(jtpix   ){CHECKSSING(w,jtsspix) R XNUM&AT(w)&&(jt->xmode==XMFLR||jt->xmode==XMCEIL)?va1(w,CCIRCLE):tymes(pie,w);}
