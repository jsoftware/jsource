/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
// Verbs: Atomic (Scalar) Monadic when arguments have one atom

#include "j.h"
#include "ve.h"
#include "vcomp.h"

// Support for Speedy Singletons

#define SSSTORENV(v,z,t,type) {AT(z)=(t); *((type *)zv) = (v); }  // When we know that if the block is reused, we are not changing the type; but we change the type of a new block
#define SSSTORE(v,z,t,type) SSSTORENV(v,z,t,type)  // we don't use MODBLOCKTYPE any more
#define SSSTORENVFL(v,z,t,type) {*((type *)zv) = (v); }  // When we know the type/shape doesn't change (FL,FL->FL)

#define SSINGENC(type) ((type)>>INTX)
#define SSINGCASE(id,subtype) (3*(id)+(subtype))   // encode case/args into one branch value
A jtssingleton1(J jt, A w,I caseno){A z;void *zv;
 F2PREFIP;
 I ar=AR(w);
 // Calculate inplaceability
 // Inplaceable if: count=1 and zombieval, or count<0, PROVIDED the arg is inplaceable and the block is not UNINCORPABLE.  No inplace if on NVR stack (AM is NVR and count>0)
 I wipok = ((((AC(w)-1)|((I)w^(I)jt->asginfo.zombieval))==0)|(SGNTO0(AC(w)))) & ((UI)jtinplace>>JTINPLACEWX) & !(AFLAG(w)&AFUNINCORPABLE+AFRO) & ((AM(w)&SGNTO0(AM(w)-AMNVRCT))^1);
// Allocate the result area
 if(wipok){ z=w; zv=voidAV(w); } else if(likely(ar==0)){GAT0(z,FL,1,0); zv=voidAV0(z);} else{GATV1(z,FL,1,ar); zv=voidAVn(z,ar);}

 // Start loading everything we will need as values before the pipeline break.  Tempting to convert int-to-float as well, but perhaps it will predict right?
 I wiv=IAV(w)[0],ziv; D wdv=DAV(w)[0],zdv;
 // Huge switch statement to handle every case.
 switch(caseno){

 case SSINGCASE(VA2CMIN-VA2CMIN,SSINGENC(B01)): 
 case SSINGCASE(VA2CMIN-VA2CMIN,SSINGENC(INT)): R w;
 case SSINGCASE(VA2CMIN-VA2CMIN,SSINGENC(FL)):
   {D x=wdv; wdv=jround(x); wdv-=TGT(wdv,x);}  // do round/floor in parallel
   if(likely(wdv == (D)(I)wdv)) SSSTORE((I)wdv,z,INT,I) else SSSTORENVFL(wdv,z,FL,D)
   R z;


 case SSINGCASE(VA2CMAX-VA2CMIN,SSINGENC(B01)):
 case SSINGCASE(VA2CMAX-VA2CMIN,SSINGENC(INT)): R w;
 case SSINGCASE(VA2CMAX-VA2CMIN,SSINGENC(FL)):
   {D x=wdv; wdv=jround(x); wdv+=TLT(wdv,x);}  // do round/ceil in parallel
   if(likely(wdv == (D)(I)wdv)) SSSTORE((I)wdv,z,INT,I) else SSSTORENVFL(wdv,z,FL,D)
   R z;


 case SSINGCASE(VA2CSTAR-VA2CMIN,SSINGENC(B01)): R w;
 case SSINGCASE(VA2CSTAR-VA2CMIN,SSINGENC(INT)): SSSTORENV((wiv>0)-(SGNTO0(wiv)),z,INT,I) R z;
 case SSINGCASE(VA2CSTAR-VA2CMIN,SSINGENC(FL)):
   SSSTORE((wdv>=1.0-jt->cct)-(-wdv>=1.0-jt->cct),z,INT,I)
   R z;


 case SSINGCASE(VA1CROOT-VA2CMIN,SSINGENC(B01)): R w;  // normal code returns B01, but INT seems better
 case SSINGCASE(VA1CROOT-VA2CMIN,SSINGENC(INT)):
    if(wiv>=0){SSSTORE(sqrt((D)wiv),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex
 case SSINGCASE(VA1CROOT-VA2CMIN,SSINGENC(FL)):
    if(wdv>=0){SSSTORENVFL(sqrt(wdv),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex


 case SSINGCASE(VA2CEXP-VA2CMIN,SSINGENC(B01)): SSSTORENV((B)wiv?2.71828182845904523536:1.0,z,FL,D) R z;
 case SSINGCASE(VA2CEXP-VA2CMIN,SSINGENC(INT)):
    SSSTORE(wiv<EMIN?0.0:EMAX<wiv?inf:exp((D)wiv),z,FL,D) R z;
 case SSINGCASE(VA2CEXP-VA2CMIN,SSINGENC(FL)):
    SSSTORENVFL(wdv<EMIN?0.0:EMAX<wdv?inf:exp(wdv),z,FL,D) R z;


 case SSINGCASE(VA1CLOG-VA2CMIN,SSINGENC(B01)): SSSTORENV((B)wiv?0.0:infm,z,FL,D) R z;
 case SSINGCASE(VA1CLOG-VA2CMIN,SSINGENC(INT)):
    if(wiv>=0){SSSTORE(log((D)wiv),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex
 case SSINGCASE(VA1CLOG-VA2CMIN,SSINGENC(FL)):
    if(wdv>=0){SSSTORENVFL(log(wdv),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex


 case SSINGCASE(VA2CSTILE-VA2CMIN,SSINGENC(B01)): SSSTORENV((B)wiv,z,B01,B) R z;
 case SSINGCASE(VA2CSTILE-VA2CMIN,SSINGENC(INT)):
    wiv=(I)((UI)(wiv^REPSGN(wiv))-(UI)REPSGN(wiv)); if(likely(wiv>=0)){SSSTORENV(wiv,z,INT,I)}else SSSTORE(-(D)IMIN,z,FL,D) R z;
 case SSINGCASE(VA2CSTILE-VA2CMIN,SSINGENC(FL)):
    wdv=ABS(wdv); SSSTORENVFL(wdv,z,FL,D) R z;


 case SSINGCASE(VA2CBANG-VA2CMIN,SSINGENC(B01)): SSSTORENV(1,z,B01,B) R z;
 case SSINGCASE(VA2CBANG-VA2CMIN,SSINGENC(INT)):
    SSSTORE(dgamma(1.0+(D)wiv),z,FL,D) RE(0) R z;
 case SSINGCASE(VA2CBANG-VA2CMIN,SSINGENC(FL)):
    SSSTORENVFL(_isnan(wdv)?wdv:dgamma(1.0+wdv),z,FL,D) RE(0) R z;


 case SSINGCASE(VA2CCIRCLE-VA2CMIN,SSINGENC(B01)): SSSTORENV((B)wiv?PI:0.0,z,FL,D) R z;
 case SSINGCASE(VA2CCIRCLE-VA2CMIN,SSINGENC(INT)):
    SSSTORE(PI*(D)wiv,z,FL,D) R z;
 case SSINGCASE(VA2CCIRCLE-VA2CMIN,SSINGENC(FL)):
    SSSTORENVFL(PI*wdv,z,FL,D) R z;


 case SSINGCASE(VA2CPLUS-VA2CMIN,SSINGENC(B01)): 
 case SSINGCASE(VA2CPLUS-VA2CMIN,SSINGENC(INT)):
 case SSINGCASE(VA2CPLUS-VA2CMIN,SSINGENC(FL)):
  R w;  // conjugating reals is a NOP

 default: ASSERTSYS(0,"ssing1");

 }
}

