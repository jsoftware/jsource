/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Monadic Atomic                                                   */

#include "j.h"
#include "ve.h"


#if BW==64
static AMONPS(floorDI,I,D,
 D mplrs[2]; mplrs[0]=2.0-jt->cct; mplrs[1]=jt->cct-0.00000000000000011; ,
 {if((*(UI*)x&0x7fffffffffffffff)<0x4310000000000000){I neg=SGNTO0((*(UI*)x)-SGNTO0(*(UI*)x)); *z=(I)(*x*mplrs[neg])-neg;}  // -0 is NOT neg
  else{D d=tfloor(*x); if(d!=(I)d){jt->workareas.ceilfloor.oflondx=n+~i; jt->jerr=EWOV; R;} *z=(I)d;}} ,  // n+~i because we use DQ
 ; )  // x100 0011 0001 =>2^50
#else
static AMON(floorDI,I,D, {D d=tfloor(*x); *z=(I)d; ASSERTW(d==*z,EWOV);})
#endif
static AMON(floorD, D,D, *z=tfloor(*x);)
static AMON(floorZ, Z,Z, *z=zfloor(*x);)

#if BW==64
static AMONPS(ceilDI,I,D,
 D mplrs[2]; mplrs[0]=2.0-jt->cct; mplrs[1]=jt->cct-0.00000000000000011; ,
 {if((*(UI*)x&0x7fffffffffffffff)<0x4310000000000000){I pos=SGNTO0((0-*(UI*)x)-SGNTO0(0-*(UI*)x)); *z=(I)(*x*mplrs[pos])+pos;}  // 0 is NOT pos
  else{D d=tceil(*x); if(d!=(I)d){jt->workareas.ceilfloor.oflondx=n+~i; jt->jerr=EWOV; R;} *z=(I)d;}} ,
 ; )  // x100 0011 0001 =>2^50
#else
static AMON(ceilDI, I,D, {D d=tceil(*x);  *z=(I)d; ASSERTW(d==*z,EWOV);})
#endif
static AMON(ceilD,  D,D, *z=tceil(*x);)
static AMON(ceilZ,  Z,Z, *z=zceil(*x);)

static AMON(cjugZ,  Z,Z, *z=zconjug(*x);)

static AMON(sgnI,   I,I, I xx=*x; *z=REPSGN(xx)|SGNTO0(-xx);)
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

static AMONPS(absI,   I,I, I vtot=0; , I val=*x; val=(val^REPSGN(val))-REPSGN(val); vtot |= val; *z=val; , if(vtot<0)jt->jerr=EWOV;)
static AMON(absZ,   D,Z, *z=zmag(*x);)

static AHDR1(oneB,C,C){memset(z,C1,n);}


#define VIP (VIPOKW)   // inplace is OK
#define VIP64 ((VIPOKW*(sizeof(I)==sizeof(D))))  // inplace if D is same length as I

static UA va1tab[]={
 /* <. */ {{{ 0,VB}, {  0,VI}, {floorDI,VI+VIP64}, {floorZ,VZ}, {  0,VX}, {floorQ,VX}}},
 /* >. */ {{{ 0,VB}, {  0,VI}, { ceilDI,VI+VIP64}, { ceilZ,VZ}, {  0,VX}, { ceilQ,VX}}},
 /* +  */ {{{ 0,VB}, {  0,VI}, {    0,VD}, { cjugZ,VZ}, {  0,VX}, {   0,VQ}}},
 /* *  */ {{{ 0,VB}, { sgnI,VI+VIP}, {   sgnD,VI+VIP64}, {  sgnZ,VZ}, { sgnX,VX}, {  sgnQ,VX}}},
 /* ^  */ {{{expB,VD}, { expI,VD}, {   expD,VD+VIP}, {  expZ,VZ}, { expX,VX}, {  expD,VD+VDD}}},
 /* |  */ {{{ 0,VB}, { absI,VI+VIP}, {   absD,VD+VIP}, {  absZ,VD}, { absX,VX}, {  absQ,VQ}}},
 /* !  */ {{{oneB,VB}, {factI,VD}, {  factD,VD}, { factZ,VZ}, {factX,VX}, { factQ,VX}}},
 /* o. */ {{{  0L,0L}, {   0L,0L}, {     0L,0L}, {    0L,0L}, { pixX,VX}, {    0L,0L}}}, // others handled as dyads
 /* %: */ {{{ 0,VB}, {sqrtI,VD}, {  sqrtD,VD+VIP}, { sqrtZ,VZ}, {sqrtX,VX}, { sqrtQ,VQ}}},
 /* ^. */ {{{logB,VD}, { logI,VD}, {   logD,VD}, {  logZ,VZ}, { logX,VX}, { logQD,VD}}},
};

static A jtva1(J,A,A);

static A jtva1s(J jt,A w,A self,I cv,VF ado){A e,x,z,ze,zx;B c;C ee;I n,t,zt;P*wp,*zp;
 t=atype(cv); zt=rtype(cv);
 wp=PAV(w); e=SPA(wp,e); x=SPA(wp,x); c=t&&TYPESNE(t,AT(e));
 if(c)RZ(e=cvt(t,e));          GA(ze,zt,1,0,    0    ); ((AHDR1FN*)ado)(jt,1L,AV(ze),AV(e));
 if(c)RZ(e=cvt(t,x)); n=AN(x); GA(zx,zt,n,AR(x),AS(x)); if(n)((AHDR1FN*)ado)(jt,n, AV(zx),AV(x));
 if(jt->jerr){
  if(jt->jerr<=NEVM)R 0;
  J jtinplace=(J)((I)jt+JTRETRY);
  ee=jt->jerr; RZ(ze=jtva1(jtinplace,e,self)); 
  jt->jerr=ee; RZ(zx=jtva1(jtinplace,x,self)); 
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
 UC xlatedid = (UC)FAV(w)->lc&0x7f;  // see which VA2 type it is
 if(xlatedid>=VA2MIN)FAV(w)->localuse.lvp[1]=&va1tab[xlatedid-VA2MIN];  // if there is a va1 function, install it
}

static A jtva1(J jt,A w,A self){A z;I cv,n,t,wt,zt;VF ado;
 UA *u=(UA *)FAV(self)->localuse.lvp[1];
 RZ(w);F1PREFIP;
 wt=AT(w); n=AN(w); wt=(I)jtinplace&JTEMPTY?B01:wt;
#if SY_64
 VA2 *p=&u->p1[(0x0321000054032100>>(CTTZ(wt)<<2))&7];  // from MSB, we need xxx 011 010 001 xxx 000 xxx xxx   101 100 xxx 011 010 001 xxx 000
#else
 if(wt&SPARSE){wt=AT(SPA(PAV(w),e));}
 VA2 *p=&u->p1[(0x54032100>>(CTTZ(wt)<<2))&7];  // from MSB, we need 101 100 xxx 011 010 001 xxx 000
#endif
 ASSERT(wt&NUMERIC,EVDOMAIN);
 if(!((I)jtinplace&JTRETRY)){
  ado=p->f; cv=p->cv;
 }else{
  I m=REPSGN((wt&XNUM+RAT)-1);   // -1 if not XNUM/RAT
  switch(VA1CASE(jt->jerr,FAV(self)->lc-VA2MIN)){
   default:     R 0;  // unknown type - error must have come from previous verb
   // all these cases are needed because sparse code may fail over to them
   case VA1CASE(EWOV,  VA2MIN-VA2MIN): cv=VD;       ado=floorD;               break;
   case VA1CASE(EWOV,  VA2MAX-VA2MIN): cv=VD;       ado=ceilD;                break;
   case VA1CASE(EWOV,  VA2RESIDUE-VA2MIN): cv=VD+VDD;   ado=absD;                 break;
   case VA1CASE(EWIRR, VA1ROOT-VA2MIN): cv=VD+VDD;   ado=sqrtD;                break;
   case VA1CASE(EWIRR, VA2POW-VA2MIN): cv=VD+VDD;   ado=expD;                 break;
   case VA1CASE(EWIRR, VA2OUTOF-VA2MIN): cv=VD+VDD;   ado=factD;                break;
   case VA1CASE(EWIRR, VA1LOG-VA2MIN): cv=VD+(VDD&m); ado=m?(VF)logD:(VF)logXD; break;
   case VA1CASE(EWIMAG,VA1ROOT-VA2MIN): cv=VZ+VZZ;   ado=sqrtZ;                break;  // this case remains because singleton code fails over to it
   case VA1CASE(EWIMAG,VA1LOG-VA2MIN): cv=VZ+(VZZ&m); ado=m?(VF)logZ:wt&XNUM?(VF)logXZ:(VF)logQZ;   // singleton code fails over to this too
  }
  RESETERR;
 }
 if(ado==0)R w;  // if function is identity, return arg
 if(AT(w)&SPARSE&&n)R va1s(w,self,cv,ado);  // branch off to do sparse
 // from here on is dense va1
 t=atype(cv); zt=rtype(cv);  // extract required type of input and result
 if(UNSAFE(t&~wt)){RZ(w=cvt(t,w)); jtinplace=(J)((I)jtinplace|JTINPLACEW);}  // convert input if necessary; if we converted, converted result is ipso facto inplaceable.  t is usually 0
 if(ASGNINPLACESGN(SGNIF((I)jtinplace,JTINPLACEWX)&SGNIF(cv,VIPOKWX),w)){z=w; if(TYPESNE(AT(w),zt))MODBLOCKTYPE(z,zt)}else{GA(z,zt,n,AR(w),AS(w));}
 if(!n)RETF(z); ((AHDR1FN*)ado)(jt,n,AV(z),AV(w));  // perform the operation on all the atoms
 if(!jt->jerr){RETF(cv&VRI+VRD?cvz(cv,z):z);}  // Normal return point: if no error, convert the result if necessary
 else{
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
  // not recoverable in place.  If recoverable with a retry, do the retry; otherwise fail.  Caller will decide; we return error indic
  R 0;
 }
}

// Consolidated entry point for ATOMIC1 verbs.
// This entry point supports inplacing
DF1(jtatomic1){A z;
 RZ(w);
 F1PREFIP;
 I awm1=AN(w)-1;
 // check for singletons
 if(!(awm1|(AT(w)&(NOUN&UNSAFE(~(B01+INT+FL)))))){  // len=1 andbool/int/float
  z=jtssingleton1(jtinplace,w,self);
  if(z||jt->jerr<=NEVM)RETF(z);  // normal return, or non-retryable error
  // if retryable error, fall through.  The retry will not be through the singleton code
 }
 // while it's convenient, check for empty result
 jtinplace=(J)((I)jtinplace+(((SGNTO0(awm1)))<<JTEMPTYX));
 // Run the full dyad, retrying if a retryable error is returned
 while(1){  // run until we get no error
  z=jtva1(jtinplace,w,self);  // execute the verb
  if(z||jt->jerr<=NEVM)RETF(z);   // return if no error or error not retryable
  jtinplace=(J)((I)jtinplace|JTRETRY);  // indicate that we are retrying the operation
 }
}

DF1(jtpix   ){RZ(w); F1PREFIP; if(XNUM&AT(w)&&(jt->xmode==XMFLR||jt->xmode==XMCEIL))R jtatomic1(jtinplace,w,self); R jtatomic2(jtinplace,pie,w,ds(CSTAR));}
