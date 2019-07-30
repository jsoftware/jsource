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
#define SSINGCASE(id,subtype) (3*(id)+(subtype))   // encode case/args into one branch value scaf
A jtssingleton1(J jt, A w,A self){A z;
 F2PREFIP;
 I wiv=FAV(self)->lc;   // temp, but start as function #
 // Allocate the result area
 {
  // Calculate inplaceability for a and w.
  // Inplaceable if: count=1 and zombieval, or count<0, PROVIDED the arg is inplaceable and the block is not UNINCORPABLE
  I wipok = ((((AC(w)-1)|((I)w^(I)jt->zombieval))==0)|((UI)AC(w)>>(BW-1))) & ((UI)jtinplace>>JTINPLACEWX) & ~(AFLAG(w)>>AFUNINCORPABLEX);
  if(wipok){ z=w; } else {GATV(z, FL, 1, AR(w), AS(w));}
 }

 D wdv;
 // Huge switch statement to handle every case.
 switch(SSINGCASE(wiv-VA2MIN,SSINGENC(AT(w)))){

 case SSINGCASE(VA2MIN-VA2MIN,SSINGENC(B01)): 
 case SSINGCASE(VA2MIN-VA2MIN,SSINGENC(INT)): R w;
 case SSINGCASE(VA2MIN-VA2MIN,SSINGENC(FL)):
   wdv = tfloor(SSRDD(w));
   if(wdv == (D)(I)wdv) SSSTORE((I)wdv,z,INT,I) else SSSTORENVFL(wdv,z,FL,D)
   R z;


 case SSINGCASE(VA2MAX-VA2MIN,SSINGENC(B01)):
 case SSINGCASE(VA2MAX-VA2MIN,SSINGENC(INT)): R w;
 case SSINGCASE(VA2MAX-VA2MIN,SSINGENC(FL)):
   wdv = tceil(SSRDD(w));
   if(wdv == (D)(I)wdv) SSSTORE((I)wdv,z,INT,I) else SSSTORENVFL(wdv,z,FL,D)
   R z;


 case SSINGCASE(VA2MULT-VA2MIN,SSINGENC(B01)): R w;
 case SSINGCASE(VA2MULT-VA2MIN,SSINGENC(INT)): SSSTORENV((SSRDI(w)>0)-((UI)SSRDI(w)>>(BW-1)),z,INT,I) R z;
 case SSINGCASE(VA2MULT-VA2MIN,SSINGENC(FL)):
   wdv = SSRDD(w);
// obsolete   SSSTORE(f>=1.0-jt->cct?1:f<=-(1.0-jt->cct)?-1:0,z,INT,I)
   SSSTORE((wdv>=1.0-jt->cct)-(-wdv>=1.0-jt->cct),z,INT,I)
   R z;


 case SSINGCASE(VA1ROOT-VA2MIN,SSINGENC(B01)): R w;  // normal code returns B01, but INT seems better
 case SSINGCASE(VA1ROOT-VA2MIN,SSINGENC(INT)):
    if((wiv = SSRDI(w))>=0){SSSTORE(sqrt((D)wiv),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex
 case SSINGCASE(VA1ROOT-VA2MIN,SSINGENC(FL)):
    if((wdv = SSRDD(w))>=0){SSSTORENVFL(sqrt(wdv),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex


 case SSINGCASE(VA2POW-VA2MIN,SSINGENC(B01)): SSSTORENV(SSRDB(w)?2.71828182845904523536:1.0,z,FL,D) R z;
 case SSINGCASE(VA2POW-VA2MIN,SSINGENC(INT)):
    wiv = SSRDI(w);
    SSSTORE(wiv<EMIN?0.0:EMAX<wiv?inf:exp((D)wiv),z,FL,D) R z;
 case SSINGCASE(VA2POW-VA2MIN,SSINGENC(FL)):
    wdv = SSRDD(w);
    SSSTORENVFL(wdv<EMIN?0.0:EMAX<wdv?inf:exp(wdv),z,FL,D) R z;


 case SSINGCASE(VA1LOG-VA2MIN,SSINGENC(B01)): SSSTORENV(SSRDB(w)?0.0:infm,z,FL,D) R z;
 case SSINGCASE(VA1LOG-VA2MIN,SSINGENC(INT)):
    if((wiv = SSRDI(w))>=0){SSSTORE(log((D)wiv),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex
 case SSINGCASE(VA1LOG-VA2MIN,SSINGENC(FL)):
    if((wdv = SSRDD(w))>=0){SSSTORENVFL(log(wdv),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex


 case SSINGCASE(VA2RESIDUE-VA2MIN,SSINGENC(B01)): SSSTORENV(SSRDB(w),z,INT,I) R z;   // return INT rather than normal B01
 case SSINGCASE(VA2RESIDUE-VA2MIN,SSINGENC(INT)):
    wiv = SSRDI(w); wiv=(wiv^(wiv>>(BW-1)))-(wiv>>(BW-1)); if(wiv>=0){SSSTORENV(wiv,z,INT,I)}else SSSTORE(-(D)IMIN,z,FL,D) R z;
 case SSINGCASE(VA2RESIDUE-VA2MIN,SSINGENC(FL)):
    wdv = SSRDD(w); wdv=ABS(wdv); SSSTORENVFL(wdv,z,FL,D) R z;


 case SSINGCASE(VA2OUTOF-VA2MIN,SSINGENC(B01)): SSSTORENV(1,z,INT,I) R z;   // return INT rather than normal B01
 case SSINGCASE(VA2OUTOF-VA2MIN,SSINGENC(INT)):
    SSSTORE(dgamma(1.0+(D)SSRDI(w)),z,FL,D) RE(0) R z;
 case SSINGCASE(VA2OUTOF-VA2MIN,SSINGENC(FL)):
    wdv = SSRDD(w); SSSTORENVFL(_isnan(wdv)?wdv:dgamma(1.0+wdv),z,FL,D) RE(0) R z;


 case SSINGCASE(VA2CIRCLE-VA2MIN,SSINGENC(B01)): SSSTORENV(SSRDB(w)?PI:0.0,z,FL,D) R z;
 case SSINGCASE(VA2CIRCLE-VA2MIN,SSINGENC(INT)):
    SSSTORE(PI*(D)SSRDI(w),z,FL,D) R z;
 case SSINGCASE(VA2CIRCLE-VA2MIN,SSINGENC(FL)):
    SSSTORENVFL(PI*SSRDD(w),z,FL,D) R z;


 case SSINGCASE(VA2PLUS-VA2MIN,SSINGENC(B01)): 
 case SSINGCASE(VA2PLUS-VA2MIN,SSINGENC(INT)):
 case SSINGCASE(VA2PLUS-VA2MIN,SSINGENC(FL)):
  R w;  // conjugating reals is a NOP

 default: ASSERTSYS(0,"ssing1");

 }
}

#if 0 // obsolete
#define SSINGF1(f) A f(J jtf, A w){ J jt=(J)(intptr_t)((I)jtf&~JTFLAGMSK); // header for function definition
#define SSINGF1OP(f) A f(J jtf, A w, I op){ J jt=(J)(intptr_t)((I)jtf&~JTFLAGMSK);   // header for function definition

// An argument can be inplaced if it is enabled in the block AND in the call
#define WINPLACE ((I)jtf&JTINPLACEW && ((AC(w)<1) || ((AC(w)==1) && (w==jt->zombieval))))



#define SSNUMPREFIX A z; I wtc=CTTZ(AT(w));  \
/* Establish the output area.  If this operation is in-placeable, reuse an in-placeable operand */ \
/* If not, allocate a single FL block with the required rank/shape.  We will */ \
/* change the type of this block when we get the result type */ \
/* Clear zombieval after use to avoid overwriting it ??? */ \
{  \
 if (WINPLACE){ z=w; } \
 else {GATV(z, FL, 1, AR(w), AS(w));} \
} /* We have the output block */


// speedy singleton routines: each argument has one atom.  The shapes may be
// any length, but we know they contain all 1s, so we don't care about jt->rank except to clear it
SSINGF1(jtssfloor) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wtc) {D f;
  default: R 0;
  case B01X: SSSTORENV(SSRDB(w),z,B01,B) R z;
  case INTX: SSSTORENV(SSRDI(w),z,INT,I) R z;
  case FLX:
   f = tfloor(SSRDD(w));
   if(f == (D)(I)f) SSSTORE((I)f,z,INT,I) else SSSTORENVFL(f,z,FL,D)
   R z;
 }
}

SSINGF1(jtssceil) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wtc) {D f;
  default: R 0;
  case B01X: SSSTORENV(SSRDB(w),z,B01,B) R z;
  case INTX: SSSTORENV(SSRDI(w),z,INT,I) R z;
  case FLX:
   f = tceil(SSRDD(w));
   if(f == (D)(I)f) SSSTORE((I)f,z,INT,I) else SSSTORENVFL(f,z,FL,D)
   R z;
 }
}

SSINGF1(jtsssignum) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wtc) {D f;
  default: R 0;
  case B01X: SSSTORENV(SSRDB(w),z,B01,B) R z;
  case INTX: SSSTORENV((SSRDI(w)>0)-((UI)SSRDI(w)>>(BW-1)),z,INT,I) R z;
  case FLX:
   f = SSRDD(w);
// obsolete   SSSTORE(f>=1.0-jt->cct?1:f<=-(1.0-jt->cct)?-1:0,z,INT,I)
   SSSTORE((f>=1.0-jt->cct)-(-f>=1.0-jt->cct),z,INT,I)
   R z;
 }
}

SSINGF1(jtsssqrt) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wtc) {D f; I i;
  default: R 0;
  case B01X: SSSTORENV(SSRDB(w),z,INT,I) R z;  // normal code returns B01, but INT seems better
  case INTX:
    if((i = SSRDI(w))>=0){SSSTORE(sqrt((D)i),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex
  case FLX:
    if((f = SSRDD(w))>=0){SSSTORENVFL(sqrt(f),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex
 }
}

SSINGF1(jtssexp) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wtc) {D f; I i;
  default: R 0;
  case B01X: SSSTORENV(SSRDB(w)?2.71828182845904523536:1.0,z,FL,D) R z;
  case INTX:
    i = SSRDI(w);
    SSSTORE(i<EMIN?0.0:EMAX<i?inf:exp((D)i),z,FL,D) R z;
  case FLX:
    f = SSRDD(w);
    SSSTORENVFL(f<EMIN?0.0:EMAX<f?inf:exp(f),z,FL,D) R z;
 }
}

SSINGF1(jtsslog) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wtc) {D f; I i;
  default: R 0;
  case B01X: SSSTORENV(SSRDB(w)?0.0:infm,z,FL,D) R z;
  case INTX:
    if((i = SSRDI(w))>=0){SSSTORE(log((D)i),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex
  case FLX:
    if((f = SSRDD(w))>=0){SSSTORENVFL(log(f),z,FL,D) R z;}   // return value if real
    jt->jerr=EWIMAG; R 0;   // otherwise fall through to normal code, returning complex
 }
}

SSINGF1(jtssmag) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wtc) {D f; I i;
  default: R 0;
  case B01X: SSSTORENV(SSRDB(w),z,INT,I) R z;   // return INT rather than normal B01
  case INTX:
    i = SSRDI(w); i=(i^(i>>(BW-1)))-(i>>(BW-1)); if(i>=0){SSSTORENV(i,z,INT,I)}else SSSTORE(-(D)IMIN,z,FL,D) R z;
  case FLX:
    f = SSRDD(w); f=(f<0)?-f:f; SSSTORENVFL(f,z,FL,D) R z;
 }
}

SSINGF1(jtssfact) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wtc) {D f;
  default: R 0;
  case B01X: SSSTORENV(1,z,INT,I) R z;   // return INT rather than normal B01
  case INTX:
    SSSTORE(dgamma(1.0+(D)SSRDI(w)),z,FL,D) RE(0) R z;
  case FLX:
    f = SSRDD(w); SSSTORENVFL(_isnan(f)?f:dgamma(1.0+f),z,FL,D) RE(0) R z;
 }
}

SSINGF1(jtsspix) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(wtc) {
  default: R 0;
  case B01X: SSSTORENV(SSRDB(w)?PI:0.0,z,FL,D) R z;
  case INTX:
    SSSTORE(PI*(D)SSRDI(w),z,FL,D) R z;
  case FLX:
    SSSTORENVFL(PI*SSRDD(w),z,FL,D) R z;
 }
}
#endif
