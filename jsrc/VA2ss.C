/* Copyright 1990-2011, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
// Verbs: Atomic (Scalar) Dyadic when arguments have one atom

#include "j.h"
#include "ve.h"
#include "vcomp.h"

// Support for Speedy Singletons
#define SSINGF2(f) A f(J jtf, A a, A w){J jt=(J)((I)jtf&-4);   // header for function definition
#define SSINGF2OP(f) A f(J jtf, A a, A w, I op){J jt=(J)((I)jtf&-4);   // header for function definition


// tell if operands are inplaceable
#define AINPLACE ((I)jtf&2)
#define WINPLACE ((I)jtf&1)

#define SSINGENC(a,w) ((a)+((w)>>2))
#define SSINGBB SSINGENC(B01,B01)
#define SSINGBI SSINGENC(B01,INT)
#define SSINGBF SSINGENC(B01,FL)
#define SSINGIB SSINGENC(INT,B01)
#define SSINGII SSINGENC(INT,INT)
#define SSINGIF SSINGENC(INT,FL)
#define SSINGFB SSINGENC(FL,B01)
#define SSINGFI SSINGENC(FL,INT)
#define SSINGFF SSINGENC(FL,FL)

#define SSRDB(w) (*(B *)CAV(w))
#define SSRDI(w) (*(I *)CAV(w))
#define SSRDF(w) (*(D *)CAV(w))
#define SSSTORE(v,z,t,type) {*((type *)CAV(z)) = (v); if((t)!=FL)AT(z)=(t);}

// jt->rank is set; figure out the rank of the result.  If that's not the rank of one of the arguments,
// return the rank needed.  If it is, return -1; the argument with larger rank will be the one to use
static I ssingflen(J jt, A a, A w){
// 
 I ra=AR(a); I rw=AR(w); I ca,cw,fa,fw,r;
 if(jt->rank[0]>=0){if(0>(fa = ra-jt->rank[0]))fa=0;}  // frame for positive rank
 else{if(0>(fa = ra+jt->rank[0]))fa=0;}   // frame for negative rank
 if(jt->rank[1]>=0){if(0>(fw = rw-jt->rank[1]))fw=0;}  // frame for positive rank
 else{if(0>(fw = rw+jt->rank[1]))fw=0;}   // frame for negative rank
 ca=ra-fa; cw=rw-fw;  // cell ranks
 jt->rank = 0;  // clear global rank once we've used it
 r = MAX(fa,fw) + MAX(ca,cw);  // Rank of result is max frame + max cellshape
 if(r!=ra && r!=rw)R r;
 R -1;
}
// allocate a singleton block of type t for rank r.
static A ssingallo(J jt,I r,I t){A z;
 GA(z,t,1,r,0); DO(r, AS(z)[i]=1;); R z;
}

#define SSNUMPREFIX A z; I sw = SSINGENC(AT(a),AT(w));  /* prepare for switch*/ \
/* Establish the output area.  If this operation is in-placeable, reuse an in-placeable operand if */ \
/* it has the larger rank.  If not, allocate a single FL block with the required rank/shape.  We will */ \
/* change the type of this block when we get the result type */ \
/* Try the zombiesym first, because if we use it the assignment is faster */ \
{I ar = AR(a); I wr = AR(w); I f; /* get rank */ \
if(jt->rank&&(f=ssingflen(jt,a,w))>=0)RZ(z=ssingallo(jt,f,FL)) /* handle frames */ \
else if (ar >= wr){  \
    if(jt->zombieval && AN(jt->zombieval)==1 && AR(jt->zombieval)==ar){AT(z=jt->zombieval)=FL;}  \
    else if (AINPLACE){ z = a; AT(z) = FL; } \
    else if (WINPLACE && ar == wr){ z = w; AT(z) = FL; } \
    else GA(z, FL, 1, ar, AS(a)); \
} else { \
    if(jt->zombieval && AN(jt->zombieval)==1 && AR(jt->zombieval)==wr){AT(z=jt->zombieval)=FL;}  \
    else if (WINPLACE){ z = w; AT(z) = FL; } \
    else GA(z, FL, 1, wr, AS(w)); \
} \
} /* We have the output block */

// We don't bother checking zombiesym for comparisons, since usually they're scalar constant results
#define SSCOMPPREFIX A z; I sw = SSINGENC(AT(a), AT(w)); I f; B zv;  \
/* Establish the output area.  If this produces an atom, it will be one or zero, so do nothing here. */ \
/* Otherwise, if this operation is in-placeable, reuse an in-placeable operand if */ \
/* it has the larger rank.  If not, allocate a single B01 block with the required rank/shape. */ \
{I ar = AR(a); I wr = AR(w); \
if (ar + wr == 0)z = 0; \
else if(jt->rank&&(f=ssingflen(jt,a,w))>=0)RZ(z=ssingallo(jt,f,B01)) /* handle frames */ \
else if (ar >= wr){ \
    if (AINPLACE){ z = a; AT(z) = B01; } \
    else if (WINPLACE && ar == wr){ z = w; AT(z) = B01; } \
    else GA(z, B01, 1, ar, AS(a)); \
} \
else{ \
    if (WINPLACE){ z = w; AT(z) = B01; } \
    else GA(z, B01, 1, wr, AS(w)); \
} \
} /* We have the output block, or 0 if we are returning an atom */

// speedy singleton routines: each argument has one atom.  The shapes may be
// any length, but we know they contain all 1s, so we don't care about jt->rank except to clear it
SSINGF2(jtssplus) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
  case SSINGBB: SSSTORE(SSRDB(a)+SSRDB(w),z,INT,I) R z;
  case SSINGBF: SSSTORE(SSRDB(a)+SSRDF(w),z,FL,D) R z;
  case SSINGFB: SSSTORE(SSRDF(a)+SSRDB(w),z,FL,D) R z;
  case SSINGIF: SSSTORE(SSRDI(a)+SSRDF(w),z,FL,D) R z;
  case SSINGFI: SSSTORE(SSRDF(a)+SSRDI(w),z,FL,D) R z;
  case SSINGBI: 
   {B av = SSRDB(a); I wv = SSRDI(w); I zv = av+wv;
   if(zv<wv)SSSTORE((D)av+(D)wv,z,FL,D) else SSSTORE(zv,z,INT,I)
   R z;}
  case SSINGIB:
   {I av = SSRDI(a); B wv = SSRDB(w); I zv = av + wv;
   if (zv<av)SSSTORE((D)av+(D)wv,z,FL,D) else SSSTORE(zv,z,INT,I)
   R z;}
  case SSINGII:
   {I av = SSRDI(a); I wv = SSRDI(w); I zv = av + wv;
   if (((zv^av)&(zv^wv))<0)SSSTORE((D)av+(D)wv,z,FL,D) else SSSTORE(zv,z,INT,I)
   R z;}
  case SSINGFF:
   {D av = SSRDF(a); D wv = SSRDF(w);
   ASSERT(!((av==inf&&wv==infm)||(av==infm&&wv==inf)),EVNAN);
   SSSTORE(av+wv,z,FL,D)  R z;}
 }
}

SSINGF2(jtssminus) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
  case SSINGBB: SSSTORE(SSRDB(a)-SSRDB(w),z,INT,I) R z;
  case SSINGBF: SSSTORE(SSRDB(a)-SSRDF(w),z,FL,D) R z;
  case SSINGFB: SSSTORE(SSRDF(a)-SSRDB(w),z,FL,D) R z;
  case SSINGIF: SSSTORE(SSRDI(a)-SSRDF(w),z,FL,D) R z;
  case SSINGFI: SSSTORE(SSRDF(a)-SSRDI(w),z,FL,D) R z;
  case SSINGBI: 
   {B av = SSRDB(a); I wv = SSRDI(w); I zv = av-wv;
   if(wv<0&&zv<=wv)SSSTORE((D)av-(D)wv,z,FL,D) else SSSTORE(zv,z,INT,I)
   R z;}
  case SSINGIB:
   {I av = SSRDI(a); B wv = SSRDB(w); I zv = av - wv;
   if (zv>av)SSSTORE((D)av-(D)wv,z,FL,D) else SSSTORE(zv,z,INT,I)
   R z;}
  case SSINGII:
   {I av = SSRDI(a); I wv = SSRDI(w); I zv = av - wv;
   if (((zv^av)&~(zv^wv))<0)SSSTORE((D)av-(D)wv,z,FL,D) else SSSTORE(zv,z,INT,I)
   R z;}
  case SSINGFF:
   {D av = SSRDF(a); D wv = SSRDF(w);
   ASSERT(!((av==inf&&wv==inf)||(av==infm&&wv==infm)),EVNAN);
   SSSTORE(av-wv,z,FL,D)  R z;}
 }
}

SSINGF2(jtssmin) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
  case SSINGBB: SSSTORE(MIN(SSRDB(a),SSRDB(w)),z,B01,B) R z;
  case SSINGBF: SSSTORE(MIN(SSRDB(a),SSRDF(w)),z,FL,D) R z;
  case SSINGFB: SSSTORE(MIN(SSRDF(a),SSRDB(w)),z,FL,D) R z;
  case SSINGIF: SSSTORE(MIN(SSRDI(a),SSRDF(w)),z,FL,D) R z;
  case SSINGFI: SSSTORE(MIN(SSRDF(a),SSRDI(w)),z,FL,D) R z;
  case SSINGBI: SSSTORE(MIN(SSRDB(a),SSRDI(w)),z,INT,I) R z;
  case SSINGIB: SSSTORE(MIN(SSRDI(a),SSRDB(w)),z,INT,I) R z;
  case SSINGII: SSSTORE(MIN(SSRDI(a),SSRDI(w)),z,INT,I) R z;
  case SSINGFF: SSSTORE(MIN(SSRDF(a),SSRDF(w)),z,FL,D) R z;
 }
}

SSINGF2(jtssmax) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
  case SSINGBB: SSSTORE(MAX(SSRDB(a),SSRDB(w)),z,B01,B) R z;
  case SSINGBF: SSSTORE(MAX(SSRDB(a),SSRDF(w)),z,FL,D) R z;
  case SSINGFB: SSSTORE(MAX(SSRDF(a),SSRDB(w)),z,FL,D) R z;
  case SSINGIF: SSSTORE(MAX(SSRDI(a),SSRDF(w)),z,FL,D) R z;
  case SSINGFI: SSSTORE(MAX(SSRDF(a),SSRDI(w)),z,FL,D) R z;
  case SSINGBI: SSSTORE(MAX(SSRDB(a),SSRDI(w)),z,INT,I) R z;
  case SSINGIB: SSSTORE(MAX(SSRDI(a),SSRDB(w)),z,INT,I) R z;
  case SSINGII: SSSTORE(MAX(SSRDI(a),SSRDI(w)),z,INT,I) R z;
  case SSINGFF: SSSTORE(MAX(SSRDF(a),SSRDF(w)),z,FL,D) R z;
 }
}



SSINGF2(jtsslt) SSCOMPPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
  case SSINGBB: zv=SSRDB(a)<SSRDB(w); break;
  case SSINGBF: zv=TLT(SSRDB(a),SSRDF(w)); break;
  case SSINGFB: zv=TLT(SSRDF(a),SSRDB(w)); break;
  case SSINGIF: zv=TLT((D)SSRDI(a),SSRDF(w)); break;
  case SSINGFI: zv=TLT(SSRDF(a),(D)SSRDI(w)); break;
  case SSINGBI: zv=SSRDB(a)<SSRDI(w); break;
  case SSINGIB: zv=SSRDI(a)<SSRDB(w); break;
  case SSINGII: zv=SSRDI(a)<SSRDI(w); break;
  case SSINGFF: zv=TLT(SSRDF(a),SSRDF(w)); break;
 }
 // zv is the Boolean value to return.  If there is an output block, the result must be non-atomic:
 // just store the value in it.  If there is no output block, return zero or one depending on the result
 if(z==0)R zv?one:zero;
 BAV(z)[0] = zv; R z;
}

SSINGF2(jtssle) SSCOMPPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
  case SSINGBB: zv=SSRDB(a)<=SSRDB(w); break;
  case SSINGBF: zv=TLE(SSRDB(a),SSRDF(w)); break;
  case SSINGFB: zv=TLE(SSRDF(a),SSRDB(w)); break;
  case SSINGIF: zv=TLE((D)SSRDI(a),SSRDF(w)); break;
  case SSINGFI: zv=TLE(SSRDF(a),(D)SSRDI(w)); break;
  case SSINGBI: zv=SSRDB(a)<=SSRDI(w); break;
  case SSINGIB: zv=SSRDI(a)<=SSRDB(w); break;
  case SSINGII: zv=SSRDI(a)<=SSRDI(w); break;
  case SSINGFF: zv=TLE(SSRDF(a),SSRDF(w)); break;
 }
 // zv is the Boolean value to return.  If there is an output block, the result must be non-atomic:
 // just store the value in it.  If there is no output block, return zero or one depending on the result
 if(z==0)R zv?one:zero;
 BAV(z)[0] = zv; R z;
}

SSINGF2OP(jtsseqne) SSCOMPPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
  case SSINGBB: zv=SSRDB(a)==SSRDB(w); break;
  case SSINGBF: zv=TEQ(SSRDB(a),SSRDF(w)); break;
  case SSINGFB: zv=TEQ(SSRDF(a),SSRDB(w)); break;
  case SSINGIF: zv=TEQ((D)SSRDI(a),SSRDF(w)); break;
  case SSINGFI: zv=TEQ(SSRDF(a),(D)SSRDI(w)); break;
  case SSINGBI: zv=SSRDB(a)==SSRDI(w); break;
  case SSINGIB: zv=SSRDI(a)==SSRDB(w); break;
  case SSINGII: zv=SSRDI(a)==SSRDI(w); break;
  case SSINGFF: zv=TEQ(SSRDF(a),SSRDF(w)); break;
 }
 // zv is the Boolean value to return.  If there is an output block, the result must be non-atomic:
 // just store the value in it.  If there is no output block, return zero or one depending on the result
 if(z==0)R zv^(B)op?one:zero;
 BAV(z)[0] = zv^(B)op; R z;
}


