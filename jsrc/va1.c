/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Monadic Atomic                                                   */

#include "j.h"
#include "ve.h"
#include "vcomp.h"
#include "va.h"
// Support for Speedy Singletons

#define SSSTORENV(v,z,t,type) {AT(z)=(t); *((type *)zv) = (v); }  // When we know that if the block is reused, we are not changing the type; but we change the type of a new block
#define SSSTORE(v,z,t,type) SSSTORENV(v,z,t,type)  // we don't use MODBLOCKTYPE any more
#define SSSTORENVFL(v,z,t,type) {*((type *)zv) = (v); }  // When we know the type/shape doesn't change (FL->FL)

#define SSINGENC(type) ((type)>>INTX)
#define SSINGCASE(id,subtype) (3*(id)+(subtype))   // encode case/args into one branch value

INLINE static A jtssingleton1(J jtfg, A w,I caseno){F12JT;A z;void *zv;
 I ar=AR(w);
 // Start loading everything we will need as values before the pipeline break.  Tempting to convert int-to-float as well, but perhaps it will predict right?
 I wiv=IAV(w)[0],ziv;
#if defined(__aarch32__)||defined(__arm__)||defined(_M_ARM)
 wdv=(wt&FL)?*(D*)wv:0.0;   // all atoms are aligned to a boundary of their size.  avoid spec check if loading an FL from a non-FL boundary
// obsolete  memcpy(&wdv,DAV(w),4);
// obsolete  memcpy(4+(char*)&wdv,4+(char*)DAV(w),4);   // avoid bus error
#else
 D wdv=DAV(w)[0];
#endif
 // Calculate inplaceability
 // Inplaceable if: count=2 and zombieval, or count<0, PROVIDED the arg is inplaceable and the block is not UNINCORPABLE/READONLY.
 I wipok = ((w==jt->zombieval)|(SGNTO0(AC(w)))) & ((UI)jtfg>>JTINPLACEWX) & !(AFLAG(w)&AFUNINCORPABLE+AFRO);
 // Allocate the result area
 if(wipok){z=w; zv=voidAV(w);} else if(likely(ar==0)){GAT0(z,FL,1,0); zv=voidAV0(z);} else{GATV1(z,FL,1,ar); zv=voidAVn(ar,z);}

 D zdv;
 // Huge switch statement to handle every case.
 switch(caseno){

 case SSINGCASE(VA1CMIN-VA1ORIGIN,SSINGENC(B01)): 
 case SSINGCASE(VA1CMIN-VA1ORIGIN,SSINGENC(INT)): R w;
 case SSINGCASE(VA1CMIN-VA1ORIGIN,SSINGENC(FL)):
   {D x=wdv; wdv=jround(x); wdv-=TGT(wdv,x);}  // do round/floor in parallel
   if(likely(wdv==(D)(I)wdv)) SSSTORE((I)wdv,z,INT,I) else SSSTORENVFL(wdv,z,FL,D)
   R z;


 case SSINGCASE(VA1CMAX-VA1ORIGIN,SSINGENC(B01)):
 case SSINGCASE(VA1CMAX-VA1ORIGIN,SSINGENC(INT)): R w;
 case SSINGCASE(VA1CMAX-VA1ORIGIN,SSINGENC(FL)):
   {D x=wdv; wdv=jround(x); wdv+=TLT(wdv,x);}  // do round/ceil in parallel
   if(likely(wdv==(D)(I)wdv)) SSSTORE((I)wdv,z,INT,I) else SSSTORENVFL(wdv,z,FL,D)
   R z;


 case SSINGCASE(VA1CSTAR-VA1ORIGIN,SSINGENC(B01)): R w;
 case SSINGCASE(VA1CSTAR-VA1ORIGIN,SSINGENC(INT)): SSSTORENV(SGNTO0(-wiv)|(REPSGN(wiv)),z,INT,I) R z;
 case SSINGCASE(VA1CSTAR-VA1ORIGIN,SSINGENC(FL)):
   SSSTORE((wdv>=1.0-jt->cct)-(-wdv>=1.0-jt->cct),z,INT,I)
   R z;


 case SSINGCASE(VA1CROOT-VA1ORIGIN,SSINGENC(B01)): R w;  // normal code returns B01, but INT seems better
 case SSINGCASE(VA1CROOT-VA1ORIGIN,SSINGENC(INT)):
    if(wiv>=0){SSSTORE(sqrt((D)wiv),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex
 case SSINGCASE(VA1CROOT-VA1ORIGIN,SSINGENC(FL)):
    if(wdv>=0){SSSTORENVFL(sqrt(wdv),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex


 case SSINGCASE(VA1CEXP-VA1ORIGIN,SSINGENC(B01)): SSSTORENV((B)wiv?2.71828182845904523536:1.0,z,FL,D) R z;
 case SSINGCASE(VA1CEXP-VA1ORIGIN,SSINGENC(INT)):
    SSSTORE(unlikely(wiv<EMIN)?0.0:unlikely(EMAX<wiv)?inf:exp((D)wiv),z,FL,D) R z;
 case SSINGCASE(VA1CEXP-VA1ORIGIN,SSINGENC(FL)):
    SSSTORENVFL(unlikely(wdv<EMIN)?0.0:unlikely(EMAX<wdv)?inf:exp(wdv),z,FL,D) R z;


 case SSINGCASE(VA1CLOG-VA1ORIGIN,SSINGENC(B01)): SSSTORENV((B)wiv?0.0:infm,z,FL,D) R z;
 case SSINGCASE(VA1CLOG-VA1ORIGIN,SSINGENC(INT)):
    if(wiv>=0){SSSTORE(log((D)wiv),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex
 case SSINGCASE(VA1CLOG-VA1ORIGIN,SSINGENC(FL)):
    if(wdv>=0){SSSTORENVFL(log(wdv),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex


 case SSINGCASE(VA1CSTILE-VA1ORIGIN,SSINGENC(B01)): SSSTORENV((B)wiv,z,B01,B) R z;
 case SSINGCASE(VA1CSTILE-VA1ORIGIN,SSINGENC(INT)):
    {I nwiv=(I)((0U-(UI)wiv)); wiv=nwiv>0?nwiv:wiv;}  if(likely(wiv>=0)){SSSTORENV(wiv,z,INT,I)}else SSSTORE(-(D)IMIN,z,FL,D) R z;
 case SSINGCASE(VA1CSTILE-VA1ORIGIN,SSINGENC(FL)):
    wdv=ABS(wdv); SSSTORENVFL(wdv,z,FL,D) R z;


 case SSINGCASE(VA1CBANG-VA1ORIGIN,SSINGENC(B01)): SSSTORENV(1,z,B01,B) R z;
 case SSINGCASE(VA1CBANG-VA1ORIGIN,SSINGENC(INT)):
    SSSTORE(dgamma(1.0+(D)wiv),z,FL,D) RE(0) R z;
 case SSINGCASE(VA1CBANG-VA1ORIGIN,SSINGENC(FL)):
    SSSTORENVFL(_isnan(wdv)?wdv:dgamma(1.0+wdv),z,FL,D) RE(0) R z;


 case SSINGCASE(VA1CCIRCLE-VA1ORIGIN,SSINGENC(B01)): SSSTORENV((B)wiv?PI:0.0,z,FL,D) R z;
 case SSINGCASE(VA1CCIRCLE-VA1ORIGIN,SSINGENC(INT)):
    SSSTORE(PI*(D)wiv,z,FL,D) R z;
 case SSINGCASE(VA1CCIRCLE-VA1ORIGIN,SSINGENC(FL)):
    SSSTORENVFL(PI*wdv,z,FL,D) R z;


 case SSINGCASE(VA1CPLUS-VA1ORIGIN,SSINGENC(B01)): 
 case SSINGCASE(VA1CPLUS-VA1ORIGIN,SSINGENC(INT)):
 case SSINGCASE(VA1CPLUS-VA1ORIGIN,SSINGENC(FL)):
  R w;  // conjugating reals is a NOP

 default: ASSERTSYS(0,"ssing1");

 }
}


#if BW==64
static AMONPS(floorDI,I,D,
 I rc=0; UI fbits; ,
 {if(likely(((fbits=*(UI*)x)&0x7fffffffffffffff)<0x43c0000000000000)){D wdv=jround(*x); *z=wdv-TGT(wdv,*x);}
  // if there is a value above 2^61, encode it by setting bit 62 to the opposite of bit 63 (we know bit 62 was 1 originally).  Remember the fact that we need a correction pass.
  // See if the value must be promoted to floating-point in the correction pass.  Return value of EWOVFLOOR0 if there are values all of which fit in an integer, EWOVFLOOR1 if float is required
  else{rc|=EWOVFLOOR0; D d=tfloor(*x); *z=fbits^(SGNTO0(fbits)<<(BW-2)); if(d!=(I)d)rc|=EWOVFLOOR1;} } ,  // we use DQ; i is n-1-reali, ~i = (reali-n+1)-1 = i-n
  R rc?rc:EVOK;
 ; )  // x100 0011 1100 =>2^61
#else
static AMON(floorDI,I,D, {D d=tfloor(*x); *z=(I)d; ASSERTWR(d==*z,EWOV);})
#endif
static AMON(floorD, D,D, *z=tfloor(*x);)
static AMON(floorZ, Z,Z, *z=zfloor(*x);)
static AMON(floorEI,I,E, {D d=tfloor(x->hi); *z=(I)d; ASSERTWR(d==*z,EWOV1);})  // tolerant floor of high part only...
static AMON(floorE, E,E, *z=efloor(*x);)  // ... if INT overflow, keep as extended to preserve precision

#if BW==64
static AMONPS(ceilDI,I,D,
 I rc=0; UI fbits; ,
 {if(likely(((fbits=*(UI*)x)&0x7fffffffffffffff)<0x43c0000000000000)){D wdv=jround(*x); *z=wdv+TLT(wdv,*x);}
  // if there is a value above 2^61, encode it by setting bit 62 to the opposite of bit 63 (we know bit 62 was 1 originally).  Remember the fact that we need a correction pass.
  // See if the value must be promoted to floating-point in the correction pass.  Return value of -2 if there are values all of which fit in an integer, -3 if float is required
  else{rc|=EWOVFLOOR0; D d=tceil(*x); *z=fbits^(SGNTO0(fbits)<<(BW-2)); if(d!=(I)d)rc|=EWOVFLOOR1;} } ,  // we use DQ; i is n-1-reali, ~i = (reali-n+1)-1 = i-n
  R rc?rc:EVOK;
 ; )  // x100 0011 1100 =>2^61
#else
static AMON(ceilDI, I,D, {D d=tceil(*x);  *z=(I)d; ASSERTWR(d==*z,EWOV);})
#endif
static AMON(ceilEI,I,E, {D d=tceil(x->hi); *z=(I)d; ASSERTWR(d==*z,EWOV1);})
static AMON(ceilD,  D,D, *z=tceil(*x);)
static AMON(ceilZ,  Z,Z, *z=zceil(*x);)
static AMON(ceilE,  E,E, *z=eceil(*x);)

static AMON(cjugZ,  Z,Z, *z=zconjug(*x);)

static AMON(sgnI,   I,I, I xx=*x; *z=REPSGN(xx)|SGNTO0(-xx);)
static AMON(sgnI2,   I,I2, I xx=*x; *z=(I2)(REPSGN(xx)|SGNTO0(-xx));)
static AMON(sgnI4,   I,I4, I xx=*x; *z=(I4)(REPSGN(xx)|SGNTO0(-xx));)
static AMON(sgnD,   I,D, *z=((1.0-jt->cct)<=*x) - (-(1.0-jt->cct)>=*x);)
static AMONPS(sgnZ,   Z,Z, , if((1.0-jt->cct)>zmag(*x))*z=zeroZ; else *z=ztrend(*x); , HDR1JERR)
static AMON(sgnE,   I,E, *z=((1.0-jt->cct)<=x->hi) - (-(1.0-jt->cct)>=x->hi);)

static AMON(sqrtI,  D,I, ASSERTWR(0<=*x,EWIMAG); *z=sqrt((D)*x);)

#if C_AVX2 || EMU_AVX2
AHDR1(sqrtD,D,D){
 AVXATOMLOOP(1,
 __m256d zero; zero=_mm256_setzero_pd();
 __m256d neg; __m256d comp; __m256d anyneg; anyneg=zero;

,
  neg=_mm256_cmp_pd(u,zero,_CMP_LT_OQ); comp=_mm256_sub_pd(zero,u); u=_mm256_blendv_pd(u,comp,neg); // convert to positive; then back to negative
  anyneg=_mm256_or_pd(anyneg,neg);
  u=_mm256_sqrt_pd(_mm256_blendv_pd(u,comp,neg)); comp=_mm256_sub_pd(zero,u); u=_mm256_blendv_pd(u,comp,neg);  // store sqrt, with sign of the original value

 ,
 R (!_mm256_testc_pd(zero,anyneg))?EWIMAG:EVOK;  // if there are any negative values, call for a postpass
 )
}

AHDR1(absD,D,D){
 AVXATOMLOOP(0,
  __m256d absmask; absmask=_mm256_broadcast_sd((D*)&Iimax);
 ,
  u=_mm256_and_pd(u,absmask);
 ,
  R EVOK;
 )
}

AHDR1(absI,D,D){
 AVXATOMLOOP(0,
 __m256d zero=_mm256_setzero_pd();
 __m256d uneg; __m256d anyneg=zero;

 ,
  uneg=_mm256_castsi256_pd(_mm256_sub_epi64(_mm256_castpd_si256(zero),_mm256_castpd_si256(u))); u=_mm256_blendv_pd(u,uneg,u); // take abs
  anyneg=_mm256_or_pd(anyneg,u);  // remember if any overflow

 ,
 R (!_mm256_testc_pd(zero,anyneg))?EWIMAG:EVOK;  // if there are any negative values, call for a postpass.  Could just convert in place
 )
}


#else
static AMONPS(sqrtD,  D,D, I ret=EVOK; , if(*x>=0)*z=sqrt(*x);else{*z=-sqrt(-*x); ret=EWIMAG;}, R ret;)  // if input is negative, leave sqrt as negative
static AMONPS(absI,   I,I, UI vtot=0; , UI val=*(UI*)x; I nval=(I)((0U-val)); val=nval>0?nval:val; vtot |= val; *z=(I)val; , R (I)vtot<0?EWOV:EVOK;)
#if BW==64
static AMON(absD,   I,I, *z= *x&IMAX;)
#else
static AMON(absD,   D,D, *z= ABS(*x);)
#endif
#endif
static AMONPS(absI2,   I2,I2, I ret=EVOK;  , I2 val=*x; I2 nval; if(unlikely(__builtin_sub_overflow((I2)0,val,&nval)))ret=EVOFLO; val=nval>0?nval:val; *z=val; , R ret;)
static AMONPS(absI4,   I4,I4, I ret=EVOK;  , I4 val=*x; I4 nval; if(unlikely(__builtin_sub_overflow((I4)0,val,&nval)))ret=EVOFLO; val=nval>0?nval:val; *z=val; , R ret;)

static AMON(sqrtZ,  Z,Z, *z=zsqrt(*x);)
AMONPS(sqrtE,  E,E, I ret=EVOK; , D l; D h; D rh; D rl; D dh; D dl; D th; D tl; *(UIL*)&l=*(UIL*)&x->lo^(*(UIL*)&x->hi&*(UIL*)&minus0); *(UIL*)&h=*(UIL*)&x->hi&~*(UIL*)&minus0; \
   rh=sqrt(h); if(rh<1e-100)rl=0; else{TWOPROD1(rh,rh,dh,dl) dl-=l; TWOSUMBS1(dh,-h,th,tl) tl+=dl; TWOSUM1(th,tl,h,l) h/=-2*rh; TWOSUMBS1(rh,h,th,tl) E r; r=CANONE1(th,tl); rh=r.hi; rl=r.lo;} \
   if(x->hi>=0){z->hi=rh; z->lo=rl;} else{z->hi=-rh; ret=EWIMAG;}, R ret;)  // if input is negative, leave sqrt as negative
static AMON(expB,   D,B, *z=*x?2.71828182845904523536:1;)
static AMONPS(expZ, Z,Z, , *z=zexp(*x); , HDR1JERR)

static AMON(logB,   D,B, *z=*x?0:infm;)
static AMON(logZ,   Z,Z, *z=zlog(*x);)

static AMONPS(absZ,   D,Z, , *z=zmag(*x); , HDR1JERR)
static AMONPS(absE,   E,E, , {*(UIL*)&z->lo=*(UIL*)&x->lo^(*(UIL*)&x->hi&0x8000000000000000LL); *(UIL*)&z->hi=*(UIL*)&x->hi&0x7fffffffffffffffLL; } , HDR1JERR)  // ABS of high part, & flip low part if hi changed

static AMONPS(negE,   E,E, , {*(UIL*)&z->lo=*(UIL*)&x->lo^0x8000000000000000LL; *(UIL*)&z->hi=*(UIL*)&x->hi^0x8000000000000000LL; } , HDR1JERR)  // ABS of high part, & flip low part if hi changed
static AMONPS(recipE,   E,E, , {E r; r=RECIPE(*x); r=CANONE1(r.hi,r.lo); z->hi=r.hi; z->lo=r.lo; } , HDR1JERR)  // ABS of high part, & flip low part if hi changed
#if 0  // used for debugging
#define f recipE
#define Tz E
#define Tx E
#define debprefix
#define debsuffix HDR1JERR
static I f(J RESTRICT jt,I n,Tz* z,Tx* x){I i;
debprefix
for(i=0;i<n;++i){
// stmt here
E r;E v=*x;
D zh,one,d,H=1.0/(v).hi;
TWOPROD1(H,(v).hi,one,d);
one-=1.0;
d+=one;
d*=(v).hi;
d+=(v).lo;
d*=H;
d*=-H;
TWOSUMBS1(H,d,zh,H);
r=(E){.hi=zh,.lo=H};
r=CANONE1(r.hi,r.lo);
z->hi=r.hi;
z->lo=r.lo;
// end stmt
++z; ++x;
}
debsuffix
}
#undef f
#undef Tz
#undef Tx
#undef prefix
#undef suffix
#endif

static AHDR1(oneB,C,C){mvc(n,z,1,MEMSET01); R EVOK;}

extern AHDR1FN expI, expD, expE, logI, logD, logE;

UA va1tab[]={
 /* <. */ {{{ 0,VB}, {  0,VI}, {floorDI,VI+VIP64}, {floorZ,VZ}, {  0,VX}, {floorQ,VX}, {0,VI}, {floorEI,VI}, {  0, VUNCH}, {0, VUNCH}}},
 /* >. */ {{{ 0,VB}, {  0,VI}, { ceilDI,VI+VIP64}, { ceilZ,VZ}, {  0,VX}, { ceilQ,VX}, {0,VI}, {ceilEI,VI}, {  0, VUNCH}, {0, VUNCH}}},
 /* +  */ {{{ 0,VB}, {  0,VI}, {    0,VD}, { cjugZ,VZ}, {  0,VX}, {   0,VQ}, {  0, VUNCH}, {0, VUNCH}, {  0, VUNCH}, {0, VUNCH}}},
 /* *  */ {{{ 0,VB}, { sgnI,VI+VIPW}, {   sgnD,VI+VIP64}, {  sgnZ,VZ}, { sgnX,VX}, {  sgnQ,VX}, {0,VI}, {sgnE,VI}, {sgnI2,VI}, {sgnI4,VI}}},
 /* ^  */ {{{expB,VD}, { expI,VD}, {   expD,VD+VIPW}, {  expZ,VZ}, { expX,VX}, {  expD,VD+VDD}, {0,0}, {expE,VUNCH}, { expD,VDD+VD}, { expD,VDD+VD}}},
 /* |  */ {{{ 0,VB}, { absI,VI+VIPW}, {   absD,VD+VIPW}, {  absZ,VD}, { absX,VX}, {  absQ,VQ}, {0,0}, {absE,VUNCH+VIPW}, { absI2,VUNCH+VIPW}, { absI4,VUNCH+VIPW}}},
 /* !  */ {{{oneB,VB}, {factI,VD}, {  factD,VD}, { factZ,VZ}, {factX,VX}, {factQ,VQ}, {0,0}, {factD,VD+VDD}, {factD,VD+VDD}, {factD,VD+VDD}}},
 /* o. */ {{{  0L,0L}, {   0L,0L}, {     0L,0L}, {    0L,0L}, { pixX,VX}, {0L,0L}, {0L,0L}, {0L,0L}, {0L,0L}, {0L,0L}}}, // others handled as dyads
 /* %: */ {{{ 0,VB}, {sqrtI,VD}, {sqrtD,VD+VIPW}, { sqrtZ,VZ}, {sqrtX,VX}, { sqrtQ,VQ}, {0,0}, {sqrtE,VUNCH}, {sqrtD,VD+VDD+VIPW}, {sqrtD,VD+VDD+VIPW}}},  // most cannot inplace lest CMPX
 /* ^. */ {{{logB,VD}, { logI,VD}, {   logD,VD}, {  logZ,VZ}, { logX,VX}, { logQD,VD}, {0,0}, {logE,VUNCH}, {logD,VD+VDD}, {logD,VD+VDD}}},
 /* 10 - (QP only) */ {{{}, {}, {}, {}, {}, {}, {}, {negE,VUNCH+VIPW}}},
 /* 11 % (QP only) */ {{{}, {}, {}, {}, {}, {}, {}, {recipE,VUNCH+VIPW}}},
};


static A jtva1(J,A,A);

static A jtva1s(J jt,A w,A self,I cv,VA1F ado){A e,x,z,ze,zx;B c;I n,oprc,t,zt;P*wp,*zp;
 t=atype(cv); zt=rtype(cv);
 wp=PAV(w); e=SPA(wp,e); x=SPA(wp,x); c=t&&TYPESNE(t,AT(e));
 if(c)RZ(e=cvt(t,e)); GA00(ze,zt,1,0); oprc=((AHDR1FN*)ado)(jt,1L,AV0(ze),AV(e));
 if(c)RZ(e=cvt(t,x)); n=AN(x); if(oprc==EVOK){GA(zx,zt,n,AR(x),AS(x)); if(n){oprc=((AHDR1FN*)ado)(jt,n, AV(zx),AV(x));}}
 // sparse does not do inplace repair.  Always retry, and never inplace.
 oprc=oprc<0?EWOV:oprc;  //   If a restart is required, turn the result to EWOV (must be floor/ceil)
 if(oprc&(255&~EVNOCONV)){
  jt->jerr=(UC)oprc;  // signal error to the retry code, or to the system
  if(jt->jerr<=NEVM)R 0;
  J jtfg=(J)((I)jt+JTRETRY);  // tell va1 it's a retry
  RZ(ze=jtva1(jtfg,e,self)); 
  jt->jerr=(UC)oprc; RZ(zx=jtva1(jtfg,x,self));   // restore restart signal for the main data too
 }else if(cv&VRI+VRD&&oprc!=EVNOCONV){RZ(ze=cvz(cv,ze)); RZ(zx=cvz(cv,zx));}
 GASPARSE(z,STYPE(AT(ze)),1,AR(w),AS(w)); zp=PAV(z);
 SPB(zp,a,ca(SPA(wp,a)));
 SPB(zp,i,ca(SPA(wp,i)));
 SPB(zp,e,ze);
 SPB(zp,x,zx);
 R z;
}

#define VA1CASE(e,f) (10*(e)+(f))

static DF1(jtva1){F12IP;A z;I cv,n,t,wt,zt;VA1F ado;
 UA *u=((UA*)((I)va1tab+FAV(self)->localuse.lu1.uavandx[0]));
 ARGCHK1(w);
 wt=AT(w); n=AN(w);
 if(unlikely(!(wt&NUMERIC))){ASSERT(AN(w)==0,EVDOMAIN) wt=B01;}  // arg must be numeric.  If it is, keep its type even if empty; if not, fail unless empty, for which treat as boolean
 VA1 *p=&u->p1[(0x76098054032100>>(CTTZ(wt)<<2))&0xf];  // convert numeric type to 4-bit fn#

 if(likely(!((I)jtfg&JTRETRY))){
  ado=p->f; cv=p->cv;
 }else{
  I m=REPSGN((wt&XNUM+RAT)-1);   // -1 if not XNUM/RAT
  switch(VA1CASE(jt->jerr,FAV(self)->lu2.lc-VA1ORIGIN)){
  default:     R 0;  // unknown type - error must have come from previous verb
  // all these cases are needed because sparse code may fail over to them
  case VA1CASE(EWOV,  VA1CMIN-VA1ORIGIN): cv=VD;       ado=floorD;               break;
  case VA1CASE(EWOV,  VA1CMAX-VA1ORIGIN): cv=VD;       ado=ceilD;                break;
  case VA1CASE(EWOV1,  VA1CMIN-VA1ORIGIN): cv=VUNCH;       ado=floorE;               break;
  case VA1CASE(EWOV1,  VA1CMAX-VA1ORIGIN): cv=VUNCH;       ado=ceilE;                break;
  case VA1CASE(EWOV,  VA1CSTILE-VA1ORIGIN): cv=VD+VDD;   ado=absD;                 break;
  case VA1CASE(EWIRR, VA1CROOT-VA1ORIGIN): cv=VD+VDD;   ado=sqrtD;                break;
  case VA1CASE(EWIRR, VA1CEXP-VA1ORIGIN): cv=VD+VDD;   ado=expD;                 break;
  case VA1CASE(EWIRR, VA1CBANG-VA1ORIGIN): cv=VD+VDD;   ado=factD;                break;
  case VA1CASE(EWIRR, VA1CLOG-VA1ORIGIN): cv=VD+(VDD&m); ado=m?(VA1F)logD:(VA1F)logXD; break;
  case VA1CASE(EWIMAG,VA1CROOT-VA1ORIGIN): cv=VZ+VZZ;   ado=sqrtZ;                break;  // this case remains because singleton code fails over to it
  case VA1CASE(EWIMAG,VA1CLOG-VA1ORIGIN): cv=VZ+(VZZ&m); ado=m?(VA1F)logZ:wt&XNUM?(VA1F)logXZ:(VA1F)logQZ; break;   // singleton code fails over to this too
  case VA1CASE(EWRAT,VA1CMIN-VA1ORIGIN): cv=VQ; ado=floorQQ; break;   // must be <. _
  case VA1CASE(EWRAT,VA1CMAX-VA1ORIGIN): cv=VQ; ado=ceilQQ; break;   // must be >. _
  }
  RESETERR;
 }
 zt=rtypew(cv,wt);  // Extract output type; if none given, means 'keep same type'
 if(unlikely(ado==0)){  // the function is an identity function
  if(((zt^AT(w))&NUMERIC)==0)RCA(w);  // if the argument has the correct type, return the argument
  GA(z,zt,n,AR(w),AS(w)); RETF(z);  // if not, must be empty, make an appropriate empty and return it
 }
 if(unlikely((SGNIFSPARSE(AT(w))&-n)<0))R va1s(w,self,cv,ado);  // branch off to do sparse
 // from here on is dense va1
 t=atype(cv);  // extract required type of input and result
 if(t&~wt){RZ(w=cvt(t,w)); jtfg=(J)((I)jtfg|JTINPLACEW);}  // convert input if necessary; if we converted, converted result is ipso facto inplaceable.  t is usually 0
 if(ASGNINPLACESGN(SGNIF(jtfg,JTINPLACEWX)&SGNIF(cv,VIPOKWX),w)){z=w; if(TYPESNE(AT(w),zt))MODBLOCKTYPE(z,zt)}else{GA(z,zt,n,AR(w),AS(w)); if(unlikely(zt&CMPX+QP))AK(z)=(AK(z)+SZD)&~SZD;}  // move 16-byte values to 16-byte bdy
 if(!n){RETF(z);}
 I oprc = ((AHDR1FN*)ado)(jt,n,AV(z),AV(w));  // perform the operation on all the atoms, save result status.  If an error was signaled it will be reported here, but not necessarily vice versa
 if(likely(!(oprc&(255&~EVNOCONV)))){RETF(unlikely(cv&VRI+VRD&&oprc!=EVNOCONV)?cvz(cv,z):z);}  // Normal return point: if no error, convert the result if necessary (rare)
 else{
  // There was an error.  If it is recoverable in place, handle the cases here
  // positive result gives error type to use for retrying the operation; negative is 1's complement of the restart point (first value NOT stored)
  // integer abs: convert everything to float, changing IMIN to IMAX+1
  if(ado==absI){A zz=z; if(VIP64){MODBLOCKTYPE(zz,FL)}else{GATV(zz,FL,n,AR(z),AS(z))}; I *zv=IAV(z); D *zzv=DAV(zz); DQ(n, if(unlikely(*zv<0))*zzv=-(D)*zv;else*zzv=(D)*zv; ++zv; ++zzv;) RETF(zz);}
  // float sqrt: reallocate as complex, scan to make positive results real and negative ones imaginary
  if(ado==sqrtD){A zz; I zzr=AR(z); GATV(zz,CMPX,n,AR(z),AS(z)); D *zv=DAV(z); Z *zzv=ZAVn(zzr,zz); DQ(n, if(*zv>=0){zzv->re=*zv;zzv->im=0.0;}else{zzv->im=-*zv;zzv->re=0.0;} ++zv; ++zzv;) RETF(zz);}
  // QP sqrt: like D but no need to reallocate, just change the result type.  This puns on the fact that E and Z types overlap perfectly
  if(ado==sqrtE){AT(z)=CMPX; E *zv=EAV(z); DQ(n, if(zv->hi>=0){zv->lo=0.0;}else{zv->lo=-zv->hi;zv->hi=0.0;} ++zv;) RETF(z);}
  // float floor: unconvertable cases are stored with bit 63 and bit 62 unlike; restore the float value by setting bit 62.
  // if bit 0 of oprc is 1, values must be converted to float; if 0, they can be left as int
  if(VIP64&&ado==floorDI){A zz=z;
   if(!(oprc&1)){I *zv=IAV(z); DQ(n, I bits=*(I*)zv; if((bits^SGNIF(bits,BW-2))<0){bits|=0x4000000000000000; *zv=(I)tfloor(*(D*)&bits);} ++zv;)}  // convert overflows, turn back to integer
   else{MODBLOCKTYPE(zz,FL) I *zv=IAV(z); D *zzv=DAV(zz); DQ(n, I bits=*(I*)zv++; if((bits^SGNIF(bits,BW-2))>=0)*zzv=(D)bits;else{bits|=0x4000000000000000; *zzv=tfloor(*(D*)&bits);} ++zzv;)}   // force float conversion
   RETF(zz);
  }
  // float ceil, similarly
  if(VIP64&&ado==ceilDI){A zz=z;
   if(!(oprc&1)){I *zv=IAV(z); DQ(n, I bits=*(I*)zv; if((bits^SGNIF(bits,BW-2))<0){bits|=0x4000000000000000; *zv=(I)tceil(*(D*)&bits);} ++zv;)}  // convert overflows, turn back to integer
   else{MODBLOCKTYPE(zz,FL) I *zv=IAV(z); D *zzv=DAV(zz); DQ(n, I bits=*(I*)zv++; if((bits^SGNIF(bits,BW-2))>=0)*zzv=(D)bits;else{bits|=0x4000000000000000; *zzv=tceil(*(D*)&bits);} ++zzv;)}   // force float conversion
   RETF(zz);
  }

  // not recoverable in place.  If recoverable with a retry, do the retry; otherwise fail.  Caller will decide; we return error indic
  // we set the error code from the value given by the routine, except that if it involves a restart it must have been ceil/floor that we couldn't restart - that's a EWOV
  oprc=oprc<0?EWOV:oprc;
  if(oprc<=NEVM){RESETERR; jsignal(oprc);}else jt->jerr=(UC)oprc;  // if real error, set error text; otherwise save error code for analysis
  R 0;
 }
}

// Consolidated entry point for ATOMIC1 verbs.
// This entry point supports inplacing
DF1(jtatomic1){F12IP;A z;
 ARGCHK1(w);
 I awm1=AN(w)-1;
 // check for singletons
 if(!(awm1|(AT(w)&((NOUN|SPARSE)&~(B01+INT+FL))))){  // len=1 and bool/int/float
  z=jtssingleton1(jtfg,w,3*(FAV(self)->lu2.lc-VA1ORIGIN)+(AT(w)>>INTX));
  if(likely(z!=0)||unlikely(jt->jerr<=NEVM)){RETF(z);}  // normal return, or non-retryable error
  // if retryable error, fall through.  The retry will not be through the singleton code
  jtfg=(J)((I)jtfg|JTRETRY);  // indicate that we are retrying the operation
 }
 // Run the full op, retrying if a retryable error is returned
 NOUNROLL while(1){  // run until we get no error
  z=jtva1(jtfg,w,self);  // execute the verb
  if(likely(z!=0)){RETF(z);}  // normal case is good return
  if(unlikely(jt->jerr<=NEVM))break;  // if nonretryable error, exit
  jtfg=(J)((I)jtfg|JTRETRY);  // indicate that we are retrying the operation
 }
 // There was an error. format it now
 jteformat(jt,self,w,0,0);
 RETF(z);
}

#define SETCONPTR(n) A conptr=num(n); A conptr2=zeroionei(n); conptr=AT(w)&INT?conptr2:conptr; conptr2=numvr(n); conptr=AT(w)&FL?conptr2:conptr;  // for 0 or 1 only
DF1(jtnegate){F12IP;ARGCHK1(w); if(unlikely(AT(w)&QP))R jtva1(jt,w,self); SETCONPTR(0) R jtatomic2(jtfg,conptr,w,(A)&dsCMINUS_NEG);}   // w remains inplaced
DF1(jtrecip ){F12IP;ARGCHK1(w); if(unlikely(AT(w)&QP))R jtva1(jt,w,self); A conptr=num(1); A conptr2=numvr(1); R jtatomic2(jtfg,AT(w)&XNUM+RAT?conptr:conptr2,w,(A)&dsCDIV_RECIP);}  // w remains inplaced
DF1(jtpix){F12IP; ARGCHK1(w); if(unlikely(XNUM&AT(w)))if(jt->xmode==XMFLR||jt->xmode==XMCEIL)R jtatomic1(jtfg,w,self); R jtatomic2(jtfg,AT(w)&QP?pieE:pie,w,(A)&dsCSTAR_CIRCLE);}

// special code for x ((<[!.0] |) * ]) y, implemented as if !.0
#if C_AVX2 || EMU_AVX2
DF2(jtdeadband){F12IP;A zz;
 ARGCHK2(a,w);
 I n=AN(w);  // number of atoms to process
  // revert if not the special case we handle: both args FL, not sparse, a is an atom, w is nonempty
 if(unlikely((((AT(a)|AT(w))&(NOUN+SPARSE))&(REPSGN(((I)AR(a)-1)&-n)))!=FL))R jtfolk2(jtfg,a,w,self);
 // allocate the result area, inplacing w if possible
 if(ASGNINPLACESGN(SGNIF(jtfg,JTINPLACEWX),w)){zz=w;}else{GATV(zz,FL,AN(w),AR(w),AS(w));}
 // perform the operation
 D *x=DAV(w), *z=DAV(zz);  // input and output pointers
 AVXATOMLOOP(0,  // unroll loop
 __m256d absmsk; absmsk=_mm256_set1_pd(*(D*)&Iimax);
 __m256d threshold; threshold=_mm256_set1_pd(DAV(a)[0]);  // install threshold

 ,
  u=_mm256_and_pd(_mm256_cmp_pd(threshold,_mm256_and_pd(absmsk,u),_CMP_LT_OQ),u);  // abs value, compare, set to 0 if < threshold
 ,
 )
 RETF(zz);
}
#endif
