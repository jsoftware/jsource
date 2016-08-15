/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
// Verbs: Atomic (Scalar) Monadic when arguments have one atom

#include "j.h"
#include "ve.h"
#include "vcomp.h"

// Support for Speedy Singletons
#define SSINGF1(f) A f(J jtf, A w){ J jt=(J)((I)jtf&-4); // header for function definition
#define SSINGF1OP(f) A f(J jtf, A w, I op){ J jt=(J)((I)jtf&-4);   // header for function definition

// An argument can be inplaced if it is enabled in the block AND in the call
#define WINPLACE ((I)jtf&1 && ACIPISOK(w))

#define SSRDB(w) (*(B *)CAV(w))
#define SSRDI(w) (*(I *)CAV(w))
#define SSRDD(w) (*(D *)CAV(w))
#define SSSTORE(v,z,t,type) {*((type *)CAV(z)) = (v); if((t)!=FL)AT(z)=(t);}

#define SSNUMPREFIX A z;  \
/* Establish the output area.  If this operation is in-placeable, reuse an in-placeable operand */ \
/* If not, allocate a single FL block with the required rank/shape.  We will */ \
/* change the type of this block when we get the result type */ \
/* Try the zombiesym first, because if we use it the assignment is faster */ \
{I wr = AR(w);    /* get rank */ \
 if(jt->zombieval && AN(jt->zombieval)==1 && AR(jt->zombieval)>=wr){AR(jt->zombieval)=wr; AT(z=jt->zombieval)=FL;}  \
 else if (WINPLACE){ AT(z=w) = FL; } \
 else {GATV(z, FL, 1, wr, AS(w)); if((I)jtf&3)ACIPYES(z);} \
} /* We have the output block */


// speedy singleton routines: each argument has one atom.  The shapes may be
// any length, but we know they contain all 1s, so we don't care about jt->rank except to clear it
SSINGF1(jtssfloor) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(AT(w)) {D f;
  default: R 0;
  case B01: SSSTORE(SSRDB(w),z,B01,B) R z;
  case INT: SSSTORE(SSRDI(w),z,INT,I) R z;
  case FL:
   f = tfloor(SSRDD(w));
   if(f == (D)(I)f) SSSTORE((I)f,z,INT,I) else SSSTORE(f,z,FL,D)
   R z;
 }
}
