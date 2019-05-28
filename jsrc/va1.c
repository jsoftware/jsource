/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Monadic Atomic                                                   */

#include "j.h"
#include "ve.h"


#if BW==64
static AMONPS(floorDI,I,D,
 D mplrs[2]; mplrs[0]=2.0-jt->cct; mplrs[1]=jt->cct-0.00000000000000011; ,
 {if((*(UI*)x&0x7fffffffffffffff)<0x4310000000000000){I neg=((*(UI*)x)-(*(UI*)x>>(BW-1)))>>(BW-1); *z=(I)(*x*mplrs[neg])-neg;}  // -0 is NOT neg
  else{D d=tfloor(*x); if(d!=(I)d){jt->workareas.ceilfloor.oflondx=n+~i; jt->jerr=EWOV; R;} *z=(I)d;}} ,
 ; )  // x100 0011 0001 =>2^50
#else
static AMON(floorDI,I,D, {D d=tfloor(*x); *z=(I)d; ASSERTW(d==*z,EWOV);})
#endif
static AMON(floorD, D,D, *z=tfloor(*x);)
static AMON(floorZ, Z,Z, *z=zfloor(*x);)

#if BW==64
static AMONPS(ceilDI,I,D,
 D mplrs[2]; mplrs[0]=2.0-jt->cct; mplrs[1]=jt->cct-0.00000000000000011; ,
 {if((*(UI*)x&0x7fffffffffffffff)<0x4310000000000000){I pos=(((UI)-(I)*(UI*)x)-((UI)-(I)*(UI*)x>>(BW-1)))>>(BW-1); *z=(I)(*x*mplrs[pos])+pos;}  // 0 is NOT pos
  else{D d=tceil(*x); if(d!=(I)d){jt->workareas.ceilfloor.oflondx=n+~i; jt->jerr=EWOV; R;} *z=(I)d;}} ,
 ; )  // x100 0011 0001 =>2^50
#else
static AMON(ceilDI, I,D, {D d=tceil(*x);  *z=(I)d; ASSERTW(d==*z,EWOV);})
#endif
static AMON(ceilD,  D,D, *z=tceil(*x);)
static AMON(ceilZ,  Z,Z, *z=zceil(*x);)

static AMON(cjugZ,  Z,Z, *z=zconjug(*x);)

// obsolete static AMON(sgnI,   I,I, *z=SGN(*x);)
static AMON(sgnI,   I,I, I xx=*x; *z=(xx>>(BW-1))|(((UI)-xx)>>(BW-1));)
// obsolete static AMON(sgnD,   I,D, *z=(1.0-jt->cct)>ABS(*x)?0:SGN(*x);)
static AMON(sgnD,   I,D, *z=((1.0-jt->cct)<=*x) - (-(1.0-jt->cct)>=*x);)
static AMON(sgnZ,   Z,Z, if((1.0-jt->cct)>zmag(*x))*z=zeroZ; else *z=ztrend(*x);)

static AMON(sqrtI,  D,I, ASSERTW(0<=*x,EWIMAG); *z=sqrt((D)*x);)

#if C_AVX&&SY_64
AHDR1(sqrtD,D,D){
 AVXATOMLOOP(
 __m256d zero; zero=_mm256_set1_pd(0.0);
 __m256d neg; __m256d comp; __m256d anyneg; anyneg=zero;

,
  neg=_mm256_cmp_pd(u,zero,_CMP_LT_OQ); comp=_mm256_sub_pd(zero,u); u=_mm256_blendv_pd(u,comp,neg); // convert to positive; then back to negative
  anyneg=_mm256_or_pd(anyneg,neg);
  u=_mm256_sqrt_pd(_mm256_blendv_pd(u,comp,neg)); comp=_mm256_sub_pd(zero,u); u=_mm256_blendv_pd(u,comp,neg);

 ,
 if(_mm256_movemask_pd(anyneg)&0xf)jt->jerr=EWIMAG;  // if there are any negative values, call for a postpass
 )
}

AHDR1(absD,D,D){
 AVXATOMLOOP(
  __m256d absmask; absmask=_mm256_castsi256_pd(_mm256_set1_epi64x (0x7fffffffffffffff));
 ,
  u=_mm256_and_pd(u,absmask);
 ,
 )
}

#else
// obsolete static AMON(sqrtD,  D,D, ASSERTW(0<=*x,EWIMAG); *z=sqrt(   *x);)
static AMON(sqrtD,  D,D, if(*x>=0)*z=sqrt(*x);else{*z=-sqrt(-*x); jt->jerr=EWIMAG;})  // if input is negative, leave sqrt as negative
#if BW==64
static AMON(absD,   I,I, *z= *x&0x7fffffffffffffff;)
#else
static AMON(absD,   D,D, *z= ABS(*x);)
#endif
#endif
static AMON(sqrtZ,  Z,Z, *z=zsqrt(*x);)

static AMON(expB,   D,B, *z=*x?2.71828182845904523536:1;)
static AMON(expI,   D,I, *z=*x<EMIN?0.0:EMAX<*x?inf:exp((D)*x);)
static AMON(expD,   D,D, *z=*x<EMIN?0.0:EMAX<*x?inf:exp(   *x);)
static AMON(expZ,   Z,Z, *z=zexp(*x);)

static AMON(logB,   D,B, *z=*x?0:infm;)
static AMON(logI,   D,I, ASSERTW(0<=*x,EWIMAG); *z=log((D)*x);)
static AMON(logD,   D,D, ASSERTW(0<=*x,EWIMAG); *z=log(   *x);)
static AMON(logZ,   Z,Z, *z=zlog(*x);)

// obsolete static AMON(absI,   I,I, if(0<=*x)*z=*x; else{ASSERTW(IMIN<*x,EWOV); *z=-*x;})
static AMONPS(absI,   I,I, I vtot=0; , I val=*x; val=(val^(val>>(BW-1)))-(val>>(BW-1)); vtot |= val; *z=val; , if(vtot<0)jt->jerr=EWOV;)
static AMON(absZ,   D,Z, *z=zmag(*x);)

static AHDR1(oneB,C,C){memset(z,C1,n);}
// obsolete static AHDR1(idf ,C,C){}  /* dummy */

#define CFLOORva1 0
#define CCEILva1 1
#define CPLUSva1 2
#define CSTARva1 3
#define CSQRTva1 4
#define CEXPva1 5
#define CLOGva1 6
#define CSTILEva1 7 
#define CBANGva1 8
#define CCIRCLEva1 9

#define VIP (VIPOKW)   // inplace is OK
#define VIP64 ((VIPOKW*(sizeof(I)==sizeof(D))))  // inplace if D is same length as I

static UA va1tab[]={
 /* <. */ {{{ 0,VB}, {  0,VI}, {floorDI,VI+VIP64}, {floorZ,VZ}, {  0,VX}, {floorQ,VX}}},
 /* >. */ {{{ 0,VB}, {  0,VI}, { ceilDI,VI+VIP64}, { ceilZ,VZ}, {  0,VX}, { ceilQ,VX}}},
 /* +  */ {{{ 0,VB}, {  0,VI}, {    0,VD}, { cjugZ,VZ}, {  0,VX}, {   0,VQ}}},
 /* *  */ {{{ 0,VB}, { sgnI,VI+VIP}, {   sgnD,VI+VIP64}, {  sgnZ,VZ}, { sgnX,VX}, {  sgnQ,VX}}},
 /* %: */ {{{ 0,VB}, {sqrtI,VD}, {  sqrtD,VD+VIP}, { sqrtZ,VZ}, {sqrtX,VX}, { sqrtQ,VQ}}},
 /* ^  */ {{{expB,VD}, { expI,VD}, {   expD,VD+VIP}, {  expZ,VZ}, { expX,VX}, {  expD,VD+VDD}}},
 /* ^. */ {{{logB,VD}, { logI,VD}, {   logD,VD}, {  logZ,VZ}, { logX,VX}, { logQD,VD}}},
 /* |  */ {{{ 0,VB}, { absI,VI+VIP}, {   absD,VD+VIP}, {  absZ,VD}, { absX,VX}, {  absQ,VQ}}},
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
 GASPARSE(z,STYPE(AT(ze)),1,AR(w),AS(w)); zp=PAV(z);
 SPB(zp,a,ca(SPA(wp,a)));
 SPB(zp,i,ca(SPA(wp,i)));
 SPB(zp,e,ze);
 SPB(zp,x,zx);
 R z;
}

#define VA1CASE(e,f) (10*(e)+(f))

// prepare a primitive atomic verb for lookups using var()
// if the verb is atomic, we fill in the lc field with the index to the va row for the verb
void va1primsetup(A w){
// obsolete  C *idaddr=(C*)strchr(va1fns,(UC)FAV(w)->id);  // see which line it is
// obsolete  FAV(w)->lc=idaddr?(UC)(idaddr-(C*)va1fns):0;  // save it.  immaterial if no match
}

static A jtva1(J jt,A w,C id){A e,z;B b,m;I cv,n,t,wt,zt;P*wp;VF ado;
 RZ(w);F1PREFIP;
 n=AN(w); wt=n?AT(w):B01;
 ASSERT(wt&NUMERIC,EVDOMAIN);
 if(b=1&&wt&SPARSE){wp=PAV(w); e=SPA(wp,e); wt=AT(e);}
 VA2 *p=&va1tab[id].p1[(0x54032100>>(CTTZ(wt)<<2))&7];  // from MSB, we need 101 100 xxx 011 010 001 xxx 000
 if(!jt->jerr){
// obsolete   p=((va1tab+((C*)strchr(va1fns,id)-(C*)va1fns))->p1)[wt&B01?0:wt&INT?1:wt&FL?2:wt&CMPX?3:wt&XNUM?4:5];
  ado=p->f; cv=p->cv;
 }else{
  m=!(wt&XNUM+RAT);
  switch(VA1CASE(jt->jerr,id)){
   default:     R 0;  // unknown type - impossible
   // all these cases are needed because sparse code may fail over to them
   case VA1CASE(EWOV,  CFLOORva1): cv=VD;       ado=floorD;               break;
   case VA1CASE(EWOV,  CCEILva1 ): cv=VD;       ado=ceilD;                break;
   case VA1CASE(EWOV,  CSTILEva1): cv=VD+VDD;   ado=absD;                 break;
   case VA1CASE(EWIRR, CSQRTva1 ): cv=VD+VDD;   ado=sqrtD;                break;
   case VA1CASE(EWIRR, CEXPva1  ): cv=VD+VDD;   ado=expD;                 break;
   case VA1CASE(EWIRR, CBANGva1 ): cv=VD+VDD;   ado=factD;                break;
   case VA1CASE(EWIRR, CLOGva1  ): cv=VD+VDD*m; ado=m?(VF)logD:(VF)logXD; break;
   case VA1CASE(EWIMAG,CSQRTva1 ): cv=VZ+VZZ;   ado=sqrtZ;                break;  // this case remains because singleton code fails over to it
   case VA1CASE(EWIMAG,CLOGva1  ): cv=VZ+VZZ*m; ado=m?(VF)logZ:wt&XNUM?(VF)logXZ:(VF)logQZ;   // singleton code fails over to this too
  }
  RESETERR;
 }
 if(ado==0)R w;  // if function is identity, return arg
 if(b)R va1s(w,id,cv,ado);  // branch off to do sparse
 // from here on is dense va1
 t=atype(cv); zt=rtype(cv);  // extract required type of input and result
 if(t&&TYPESNE(t,wt)){RZ(w=cvt(t,w)); jtinplace=(J)((I)jtinplace|JTINPLACEW);}  // convert input if necessary; if we converted, converted result is ipso facto inplaceable
 if(((I)jtinplace&(cv>>VIPOKWX)&JTINPLACEW) && ASGNINPLACE(w)){z=w; if(TYPESNE(AT(w),zt))MODBLOCKTYPE(z,zt)}else{GA(z,zt,n,AR(w),AS(w));}
 if(!n)RETF(z); ado(jt,n,AV(z),AV(w));  // perform the operation on all the atoms
 if(jt->jerr){
  // There was an error.  If it is recoverable in place, handle the cases here
  // integer abs: convert everything to float, changing IMIN to IMAX+1
  if(ado==absI){RESETERR; A zz=z; if(VIP64){MODBLOCKTYPE(zz,FL)}else{GATV(zz,FL,n,AR(z),AS(z))}; I *zv=IAV(z); D *zzv=DAV(zz); DQ(n, if(*zv<0)*zzv=-(D)*zv;else*zzv=(D)*zv; ++zv; ++zzv;) RETF(zz);}
  // float sqrt: reallocate as complex, scan to make positive results real and negative ones imaginary
  if(ado==sqrtD){RESETERR; A zz; GATV(zz,CMPX,n,AR(z),AS(z)); D *zv=DAV(z); Z *zzv=ZAV(zz); DQ(n, if(*zv>=0){zzv->re=*zv;zzv->im=0.0;}else{zzv->im=-*zv;zzv->re=0.0;} ++zv; ++zzv;) RETF(zz);}
  // float floor: copy everything that was successfully converted, converting to float; then floor the rest as float
  if(VIP64&&ado==floorDI){RESETERR; A zz=z; MODBLOCKTYPE(zz,FL) I *zv=IAV(z); D *zzv=DAV(zz); DQ(jt->workareas.ceilfloor.oflondx, *zzv++=(D)*zv++;)
   D *wv=DAV(w)+jt->workareas.ceilfloor.oflondx; DQ(n-jt->workareas.ceilfloor.oflondx, *zzv++=tfloor(*wv++);) RETF(zz);}
  // float ceil, similarly
  if(VIP64&&ado==ceilDI){RESETERR; A zz=z; MODBLOCKTYPE(zz,FL) I *zv=IAV(z); D *zzv=DAV(zz); DQ(jt->workareas.ceilfloor.oflondx, *zzv++=(D)*zv++;)
   D *wv=DAV(w)+jt->workareas.ceilfloor.oflondx; DQ(n-jt->workareas.ceilfloor.oflondx, *zzv++=tceil(*wv++);) RETF(zz);}
  // not recoverable in place.  If recoverable with a retry, do the retry; otherwise fail
  R NEVM<jt->jerr?va1(w,id):0; // if recoverable error, recur to do recovery; if other error, fail
 }else    {RETF(cv&VRI+VRD?cvz(cv,z):z);}  // if no error, convert the result if necessary
}


// If argument has a single direct-numeric atom, go process through speedy-singleton code
// obsolete #define CHECKSSING(w,f) RZ(w); if(AN(w)==1 && (AT(w)&(B01+INT+FL)))R f(jt,w);
// obsolete #define CHECKSSINGNZ(w,f) RZ(w); if(AN(w)==1 && (AT(w)&(B01+INT+FL))){A z = f(jt,w); if(z)R z;}  // fall through if returns 0
#define CHECKSSING(w,f) RZ(w); if(!((AN(w)-1)|(AT(w)&(NOUN&~(B01+INT+FL)))))R f(jt,w);
#define CHECKSSINGNZ(w,f) RZ(w); if(!((AN(w)-1)|(AT(w)&(NOUN&~(B01+INT+FL))))){A z = f(jt,w); if(z)R z;}  // fall through if returns 0


F1(jtfloor1){CHECKSSING(w,jtssfloor) R va1(w,CFLOORva1);}
F1(jtceil1 ){CHECKSSING(w,jtssceil) R va1(w,CCEILva1 );}
F1(jtconjug){R va1(w,CPLUSva1 );}
F1(jtsignum){CHECKSSING(w,jtsssignum) R va1(w,CSTARva1 );}
F1(jtsqroot){CHECKSSINGNZ(w,jtsssqrt) R va1(w,CSQRTva1 );}
F1(jtexpn1 ){CHECKSSING(w,jtssexp) R va1(w,CEXPva1  );}
F1(jtlogar1){CHECKSSINGNZ(w,jtsslog) R va1(w,CLOGva1  );}
F1(jtmag   ){CHECKSSING(w,jtssmag) R va1(w,CSTILEva1);}
F1(jtfact  ){CHECKSSING(w,jtssfact) R va1(w,CBANGva1 );}
F1(jtpix   ){CHECKSSING(w,jtsspix) R XNUM&AT(w)&&(jt->xmode==XMFLR||jt->xmode==XMCEIL)?va1(w,CCIRCLEva1):tymes(pie,w);}

extern A jtva2recur(J jt, AD * RESTRICT a, AD * RESTRICT w, AD * RESTRICT self){R va2(a,w,self);}  // put in this module so compiler doesn't know it's recursive
