/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
// Verbs: Atomic (Scalar) Monadic when arguments have one atom

#include "j.h"
#include "ve.h"
#include "vcomp.h"

// Support for Speedy Singletons
#define SSRDB(w) (*(B *)CAV(w))
#define SSRDI(w) (*(I *)CAV(w))
#define SSRDD(w) (*(D *)CAV(w))

#define SSSTORENV(v,z,t,type) {*((type *)CAV(z)) = (v); AT(z)=(t); }  // When we know that if the block is reused, we are not changing the type; but we change the type of a new block
#define SSSTORE(v,z,t,type) SSSTORENV(v,z,t,type)  // we don't use MODBLOCKTYPE any more
#define SSSTORENVFL(v,z,t,type) {*((type *)CAV(z)) = (v); }  // When we know the type/shape doesn't change (FL,FL->FL)

#define SSINGENC(type) (UNSAFE(type)>>INTX)
#define SSINGCASE(id,subtype) (3*(id)+(subtype))   // encode case/args into one branch value
A jtssingleton1(J jt, A w,A self){A z;
 F2PREFIP;
 // Get the address of an inplaceable assignment, if any
 L *asym = jt->assignsym; asym=asym?asym:(L*)(validitymask+12); asym=(L*)asym->val; // pending assignment if any; if non0, fetch address of value (otherwise 0)
 I wiv=FAV(self)->lc;   // temp, but start as function #
 wiv = SSINGCASE(wiv-VA2CMIN,SSINGENC(AT(w)));
 // Allocate the result area
 {
  // Calculate inplaceability for a and w.
  // Inplaceable if: count=1 and zombieval, or count<0, PROVIDED the arg is inplaceable and the block is not UNINCORPABLE
  I wipok = ((((AC(w)-1)|((I)w^(I)asym))==0)|(SGNTO0(AC(w)))) & ((UI)jtinplace>>JTINPLACEWX) & !(AFLAG(w)&AFUNINCORPABLE+AFRO+AFNVR);
  if(wipok){ z=w; } else {GATV(z, FL, 1, AR(w), AS(w));}
 }

 D wdv;
 // Huge switch statement to handle every case.
 switch(wiv){

 case SSINGCASE(VA2CMIN-VA2CMIN,SSINGENC(B01)): 
 case SSINGCASE(VA2CMIN-VA2CMIN,SSINGENC(INT)): R w;
 case SSINGCASE(VA2CMIN-VA2CMIN,SSINGENC(FL)):
   {D x=SSRDD(w); wdv=jround(x); D xf=jfloor(x); if(TNE(x,wdv))wdv=xf;}  // do round/floor in parallel
   if(wdv == (D)(I)wdv) SSSTORE((I)wdv,z,INT,I) else SSSTORENVFL(wdv,z,FL,D)
   R z;


 case SSINGCASE(VA2CMAX-VA2CMIN,SSINGENC(B01)):
 case SSINGCASE(VA2CMAX-VA2CMIN,SSINGENC(INT)): R w;
 case SSINGCASE(VA2CMAX-VA2CMIN,SSINGENC(FL)):
   {D x=SSRDD(w); wdv=jround(x); D xc=jceil(x); if(TNE(x,wdv))wdv=xc;}  // do round/ceil in parallel
   if(wdv == (D)(I)wdv) SSSTORE((I)wdv,z,INT,I) else SSSTORENVFL(wdv,z,FL,D)
   R z;


 case SSINGCASE(VA2CSTAR-VA2CMIN,SSINGENC(B01)): R w;
 case SSINGCASE(VA2CSTAR-VA2CMIN,SSINGENC(INT)): SSSTORENV((SSRDI(w)>0)-(SGNTO0(SSRDI(w))),z,INT,I) R z;
 case SSINGCASE(VA2CSTAR-VA2CMIN,SSINGENC(FL)):
   wdv = SSRDD(w);
   SSSTORE((wdv>=1.0-jt->cct)-(-wdv>=1.0-jt->cct),z,INT,I)
   R z;


 case SSINGCASE(VA1CROOT-VA2CMIN,SSINGENC(B01)): R w;  // normal code returns B01, but INT seems better
 case SSINGCASE(VA1CROOT-VA2CMIN,SSINGENC(INT)):
    if((wiv = SSRDI(w))>=0){SSSTORE(sqrt((D)wiv),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex
 case SSINGCASE(VA1CROOT-VA2CMIN,SSINGENC(FL)):
    if((wdv = SSRDD(w))>=0){SSSTORENVFL(sqrt(wdv),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex


 case SSINGCASE(VA2CEXP-VA2CMIN,SSINGENC(B01)): SSSTORENV(SSRDB(w)?2.71828182845904523536:1.0,z,FL,D) R z;
 case SSINGCASE(VA2CEXP-VA2CMIN,SSINGENC(INT)):
    wiv = SSRDI(w);
    SSSTORE(wiv<EMIN?0.0:EMAX<wiv?inf:exp((D)wiv),z,FL,D) R z;
 case SSINGCASE(VA2CEXP-VA2CMIN,SSINGENC(FL)):
    wdv = SSRDD(w);
    SSSTORENVFL(wdv<EMIN?0.0:EMAX<wdv?inf:exp(wdv),z,FL,D) R z;


 case SSINGCASE(VA1CLOG-VA2CMIN,SSINGENC(B01)): SSSTORENV(SSRDB(w)?0.0:infm,z,FL,D) R z;
 case SSINGCASE(VA1CLOG-VA2CMIN,SSINGENC(INT)):
    if((wiv = SSRDI(w))>=0){SSSTORE(log((D)wiv),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex
 case SSINGCASE(VA1CLOG-VA2CMIN,SSINGENC(FL)):
    if((wdv = SSRDD(w))>=0){SSSTORENVFL(log(wdv),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex


 case SSINGCASE(VA2CSTILE-VA2CMIN,SSINGENC(B01)): SSSTORENV(SSRDB(w),z,B01,B) R z;
 case SSINGCASE(VA2CSTILE-VA2CMIN,SSINGENC(INT)):
    wiv = SSRDI(w); wiv=(I)((UI)(wiv^REPSGN(wiv))-(UI)REPSGN(wiv)); if(wiv>=0){SSSTORENV(wiv,z,INT,I)}else SSSTORE(-(D)IMIN,z,FL,D) R z;
 case SSINGCASE(VA2CSTILE-VA2CMIN,SSINGENC(FL)):
    wdv = SSRDD(w); wdv=ABS(wdv); SSSTORENVFL(wdv,z,FL,D) R z;


 case SSINGCASE(VA2CBANG-VA2CMIN,SSINGENC(B01)): SSSTORENV(1,z,B01,B) R z;
 case SSINGCASE(VA2CBANG-VA2CMIN,SSINGENC(INT)):
    SSSTORE(dgamma(1.0+(D)SSRDI(w)),z,FL,D) RE(0) R z;
 case SSINGCASE(VA2CBANG-VA2CMIN,SSINGENC(FL)):
    wdv = SSRDD(w); SSSTORENVFL(_isnan(wdv)?wdv:dgamma(1.0+wdv),z,FL,D) RE(0) R z;


 case SSINGCASE(VA2CCIRCLE-VA2CMIN,SSINGENC(B01)): SSSTORENV(SSRDB(w)?PI:0.0,z,FL,D) R z;
 case SSINGCASE(VA2CCIRCLE-VA2CMIN,SSINGENC(INT)):
    SSSTORE(PI*(D)SSRDI(w),z,FL,D) R z;
 case SSINGCASE(VA2CCIRCLE-VA2CMIN,SSINGENC(FL)):
    SSSTORENVFL(PI*SSRDD(w),z,FL,D) R z;


 case SSINGCASE(VA2CPLUS-VA2CMIN,SSINGENC(B01)): 
 case SSINGCASE(VA2CPLUS-VA2CMIN,SSINGENC(INT)):
 case SSINGCASE(VA2CPLUS-VA2CMIN,SSINGENC(FL)):
  R w;  // conjugating reals is a NOP

 default: ASSERTSYS(0,"ssing1");

 }
}

