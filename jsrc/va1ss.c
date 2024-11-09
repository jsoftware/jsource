/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
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
 // Inplaceable if: count=2 and zombieval, or count<0, PROVIDED the arg is inplaceable and the block is not UNINCORPABLE.  No inplace if on NVR stack (AM is NVR and count>0)
 I wipok = ((w==jt->zombieval)|(SGNTO0(AC(w)))) & ((UI)jtinplace>>JTINPLACEWX) & !(AFLAG(w)&AFUNINCORPABLE+AFRO);
// Allocate the result area
 if(wipok){ z=w; zv=voidAV(w); } else if(likely(ar==0)){GAT0(z,FL,1,0); zv=voidAV0(z);} else{GATV1(z,FL,1,ar); zv=voidAVn(ar,z);}

 // Start loading everything we will need as values before the pipeline break.  Tempting to convert int-to-float as well, but perhaps it will predict right?
 I wiv=IAV(w)[0],ziv;
#if defined(__aarch32__)||defined(__arm__)||defined(_M_ARM)
 D wdv;
 memcpy(&wdv,DAV(w),4);
 memcpy(4+(char*)&wdv,4+(char*)DAV(w),4);   // avoid bus error
#else
 D wdv=DAV(w)[0];
#endif
 D zdv;
 // Huge switch statement to handle every case.
 switch(caseno){

 case SSINGCASE(VA1CMIN-VA1ORIGIN,SSINGENC(B01)): 
 case SSINGCASE(VA1CMIN-VA1ORIGIN,SSINGENC(INT)): R w;
 case SSINGCASE(VA1CMIN-VA1ORIGIN,SSINGENC(FL)):
   {D x=wdv; wdv=jround(x); wdv-=TGT(wdv,x);}  // do round/floor in parallel
   if(likely(wdv == (D)(I)wdv)) SSSTORE((I)wdv,z,INT,I) else SSSTORENVFL(wdv,z,FL,D)
   R z;


 case SSINGCASE(VA1CMAX-VA1ORIGIN,SSINGENC(B01)):
 case SSINGCASE(VA1CMAX-VA1ORIGIN,SSINGENC(INT)): R w;
 case SSINGCASE(VA1CMAX-VA1ORIGIN,SSINGENC(FL)):
   {D x=wdv; wdv=jround(x); wdv+=TLT(wdv,x);}  // do round/ceil in parallel
   if(likely(wdv == (D)(I)wdv)) SSSTORE((I)wdv,z,INT,I) else SSSTORENVFL(wdv,z,FL,D)
   R z;


 case SSINGCASE(VA1CSTAR-VA1ORIGIN,SSINGENC(B01)): R w;
 case SSINGCASE(VA1CSTAR-VA1ORIGIN,SSINGENC(INT)): SSSTORENV((wiv>0)-(SGNTO0(wiv)),z,INT,I) R z;
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

