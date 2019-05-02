/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
// Verbs: Atomic (Scalar) Dyadic when arguments have one atom

#include "j.h"
#include "ve.h"
#include "vcomp.h"

#define SSRDB(w) (*(B *)CAV(w))
#define SSRDI(w) (*(I *)CAV(w))
#define SSRDS(w) (*(SB*)CAV(w))
#define SSRDC(w) (*(UC*)CAV(w))
#define SSRDW(w) (*(US*)CAV(w))
#define SSRDU(w) (*(C4*)CAV(w))

// Support for Speedy Singletons
#define SSINGF2(f) A f(J jtf, A a, A w){ J jt=(J)(intptr_t)((I)jtf&~JTFLAGMSK); // header for function definition
#define SSINGF2OP(f) A f(J jtf, A a, A w, I op){ J jt=(J)(intptr_t)((I)jtf&~JTFLAGMSK);   // header for function definition

// An argument can be inplaced if it is enabled in the block AND in the call
#define AINPLACE ((I)jtf&JTINPLACEA && ((AC(a)<1) || ((AC(a)==1) && (a==jt->zombieval))))
#define WINPLACE ((I)jtf&JTINPLACEW && ((AC(w)<1) || ((AC(w)==1) && (w==jt->zombieval))))

// #define SSINGENC(a,w) (((a)+((w)>>2))&(2*FL-1))   // Mask off SAFE bits
#define SSINGENC(a,w) ((UNSAFE(a))+((UNSAFE(w))*5))   // Mask off SAFE bits
#define SSINGBB SSINGENC(B01,B01)
#define SSINGBI SSINGENC(B01,INT)
#define SSINGBD SSINGENC(B01,FL)
#define SSINGIB SSINGENC(INT,B01)
#define SSINGII SSINGENC(INT,INT)
#define SSINGID SSINGENC(INT,FL)
#define SSINGDB SSINGENC(FL,B01)
#define SSINGDI SSINGENC(FL,INT)
#define SSINGDD SSINGENC(FL,FL)
#define SSINGCC SSINGENC(LIT,LIT)
#define SSINGCW SSINGENC(LIT,C2T)
#define SSINGCU SSINGENC(LIT,C4T)
#define SSINGWC SSINGENC(C2T,LIT)
#define SSINGWW SSINGENC(C2T,C2T)
#define SSINGWU SSINGENC(C2T,C4T)
#define SSINGUC SSINGENC(C4T,LIT)
#define SSINGUW SSINGENC(C4T,C2T)
#define SSINGUU SSINGENC(C4T,C4T)
// obsolete #define SSINGSS SSINGENC(SBT,SBT)


#define SSRDD(w) (*(D *)CAV(w))
#define SSSTORE(v,z,t,type) {MODBLOCKTYPE(z,t) *((type *)CAV(z)) = (v);}
#define SSSTORENV(v,z,t,type) {*((type *)CAV(z)) = (v); AT(z)=(t); }  // When we know that if the block is reused, we are not changing the type; but we change the type of a new block
#define SSSTORENVFL(v,z,t,type) {*((type *)CAV(z)) = (v); }  // When we know the type/shape doesn't change (FL,FL->FL)

// jt->rank is set; figure out the rank of the result.  If that's not the rank of one of the arguments,
// return the rank needed.  If it is, return -1; the argument with larger rank will be the one to use
// This doesn't have to be perfect, but it's used only when a rank is given, which should be rare when an argument is a singleton
static I ssingflen(J jt, I ra, I rw, RANK2T ranks){I ca,cw,fa,fw,r;
 fa=ra-(ranks>>RANKTX); fa=fa<0?0:fa; 
 fw=rw-(RANKT)ranks; fw=fw<0?0:fw; 
 ca=ra-fa; cw=rw-fw;  // cell ranks
 RESETRANK;  // clear global rank once we've used it
 r = MAX(fa,fw) + MAX(ca,cw);  // Rank of result is max frame + max cellshape
 if(r!=ra && r!=rw)R r;
 R -1;
}
#if 0 // obsolete
// allocate a singleton block of type t for rank r.
static A ssingallo(J jt,I r,I t){A z;
 GA(z,t,1,r,0); DO(r, AS(z)[i]=1;); R z;  // not inplaceable since we don't have jt (? we do)
SSINGALLO(z,r,FL) GAT0(z,FL,1,0)
}
#else
static A ssingalloFL(J jt,I r){A z; GATV1(z,FL,1,r,0); R z;}  // allocate with rank and shape= all 1
static A ssingalloB01(J jt,I r){A z; GATV1(z,B01,1,r,0); R z;}
// allocate a singleton block of type t for rank f.
#define SSINGALLO(z,f,t) {if(!f)GATS(z,t,1,0,0,t##SIZE,GACOPYSHAPE0)else RZ(z=ssingallo##t(jt,f))}  // normal case is atom; use subroutine for others
#endif

// MUST NOT USE AT after SSNUMPREFIX!  We overwrite the type.  Use sw only

// In a normal expression a =. b + c + d the w argument is more likely to be inplaceable than a
#define SSNUMPREFIX A z; I sw = SSINGENC(AT(a),AT(w));  /* prepare for switch*/ \
/* Establish the output area.  If this operation is in-placeable, reuse an in-placeable operand if */ \
/* it has the larger rank.  If not, allocate a single FL block with the required rank/shape.  We will */ \
/* change the type of this block when we get the result type */ \
{I ar = AR(a); I wr = AR(w); I f; /* get rank */ \
 if(jt->ranks!=(RANK2T)~0&&(f=ssingflen(jt,ar,wr,jt->ranks))>=0)SSINGALLO(z,f,FL) /* handle frames */ \
 else if (wr >= ar){  \
  if (WINPLACE){ z = w; } \
  else if (AINPLACE && ar == wr){ z = a; } \
  else {GATV(z, FL, 1, wr, AS(w));} \
 } else { \
  if (AINPLACE){ z = a; } \
  else {GATV(z, FL, 1, ar, AS(a));} \
 } \
} /* We have the output block */

// MUST NOT USE AT after SSCOMPPREFIX!  We overwrite the type.  Use sw only

// We don't bother checking zombieval for comparisons, since usually they're scalar constant results
#define SSCOMPPREFIX A z; I sw = SSINGENC(AT(a), AT(w)); I f; B zv;  \
/* Establish the output area.  If this produces an atom, it will be num[0] or num[1]; */ \
/* Nevertheless, we try to reuse an inplaceable argument because that allows the result to be inplaced */ \
/* If this operation is in-placeable, reuse an in-placeable operand if */ \
/* it has the larger rank.  If not, allocate a single B01 block with the required rank/shape. */ \
/* It's OK to modify the AT field of an inplaced input because comparisons (unlike computations) never failover to the normal code */ \
{I ar = AR(a); I wr = AR(w); \
 if((ar+wr)&&jt->ranks!=(RANK2T)~0&&(f=ssingflen(jt,ar,wr,jt->ranks))>=0)SSINGALLO(z,f,B01) /* handle frames */ \
 else if (wr >= ar){ \
  if (WINPLACE){ z = w; MODBLOCKTYPE(z,B01) } \
  else if (AINPLACE && ar == wr){ z = a; MODBLOCKTYPE(z,B01) } \
  else if (ar + wr == 0)z = 0; \
  else {GATV(z, B01, 1, wr, AS(w));} \
 } else { \
  if (AINPLACE){ z = a; MODBLOCKTYPE(z,B01) } \
  else if (ar + wr == 0)z = 0; \
  else {GATV(z, B01, 1, ar, AS(a));} \
 } \
} /* We have the output block, or 0 if we are returning an atom */

// speedy singleton routines: each argument has one atom.  The shapes may be
// any length, but we know they contain all 1s, so we don't care about jt->rank except to clear it
SSINGF2(jtssplus) SSNUMPREFIX
 if(jt->jerr&&jt->jerr<EWOV)R 0;  // If we have encountered error, give no result.  A bit kludgey, but that's how it was done.

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
  case SSINGBB: SSSTORENV(SSRDB(a)+SSRDB(w),z,INT,I) R z;  // NV because B01 is never virtual inplace
  case SSINGBD: SSSTORENV(SSRDB(a)+SSRDD(w),z,FL,D) R z;
  case SSINGDB: SSSTORENV(SSRDD(a)+SSRDB(w),z,FL,D) R z;
  case SSINGID: SSSTORE(SSRDI(a)+SSRDD(w),z,FL,D) R z;
  case SSINGDI: SSSTORE(SSRDD(a)+SSRDI(w),z,FL,D) R z;
  case SSINGBI: 
   {B av = SSRDB(a); I wv = SSRDI(w); I zv = av+wv;
   if(zv<wv)SSSTORE((D)av+(D)wv,z,FL,D) else SSSTORENV(zv,z,INT,I)
   R z;}
  case SSINGIB:
   {I av = SSRDI(a); B wv = SSRDB(w); I zv = av + wv;
   if (zv<av)SSSTORE((D)av+(D)wv,z,FL,D) else SSSTORENV(zv,z,INT,I)
   R z;}
  case SSINGII:
   {I av = SSRDI(a); I wv = SSRDI(w); I zv = av + wv;
   if (XANDY((zv^av),(zv^wv))<0)SSSTORE((D)av+(D)wv,z,FL,D) else SSSTORENV(zv,z,INT,I)
   R z;}
  case SSINGDD:
   {
   NAN0; SSSTORENVFL(SSRDD(a)+SSRDD(w),z,FL,D) NAN1; R z;}
 }
}

SSINGF2(jtssminus) SSNUMPREFIX
 if(jt->jerr&&jt->jerr<EWOV)R 0;  // If we have encountered error, give no result.  A bit kludgey, but that's how it was done.

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
  case SSINGBB: SSSTORENV(SSRDB(a)-SSRDB(w),z,INT,I) R z;
  case SSINGBD: SSSTORENV(SSRDB(a)-SSRDD(w),z,FL,D) R z;
  case SSINGDB: SSSTORENV(SSRDD(a)-SSRDB(w),z,FL,D) R z;
  case SSINGID: SSSTORE(SSRDI(a)-SSRDD(w),z,FL,D) R z;
  case SSINGDI: SSSTORE(SSRDD(a)-SSRDI(w),z,FL,D) R z;
  case SSINGBI: 
   {B av = SSRDB(a); I wv = SSRDI(w); I zv = av-wv;
   if(wv<0&&zv<=av)SSSTORE((D)av-(D)wv,z,FL,D) else SSSTORENV(zv,z,INT,I)
   R z;}
  case SSINGIB:
   {I av = SSRDI(a); I wv = (I)SSRDB(w); I zv = av - wv;   
   if (zv>av)SSSTORE((D)av-(D)wv,z,FL,D) else SSSTORENV(zv,z,INT,I)
   R z;}
  case SSINGII:
   {I av = SSRDI(a); I wv = SSRDI(w); I zv = av - wv;
   if (XANDY((zv^av),~(zv^wv))<0)SSSTORE((D)av-(D)wv,z,FL,D) else SSSTORENV(zv,z,INT,I)
   R z;}
  case SSINGDD:
   {
   NAN0; SSSTORENVFL(SSRDD(a)-SSRDD(w),z,FL,D) NAN1;  R z;}
 }
}

SSINGF2(jtssmin) SSNUMPREFIX
 if(jt->jerr)R 0;  // If we have encountered error, give no result.  A bit kludgey, but that's how it was done.

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
// obsolete   case SSINGSS: SSSTORENV(SBMIN(SSRDS(a),SSRDS(w)),z,SBT,SB) R z;
  case SSINGBB: SSSTORENV(SSRDB(a)&SSRDB(w),z,B01,B) R z;
  case SSINGBD: SSSTORENV(MIN(SSRDB(a),SSRDD(w)),z,FL,D) R z;
  case SSINGDB: SSSTORENV(MIN(SSRDD(a),SSRDB(w)),z,FL,D) R z;
  case SSINGID: SSSTORE(MIN(SSRDI(a),SSRDD(w)),z,FL,D) R z;
  case SSINGDI: SSSTORE(MIN(SSRDD(a),SSRDI(w)),z,FL,D) R z;
  case SSINGBI: SSSTORENV(MIN(SSRDB(a),SSRDI(w)),z,INT,I) R z;
  case SSINGIB: SSSTORENV(MIN(SSRDI(a),SSRDB(w)),z,INT,I) R z;
  case SSINGII: SSSTORENV(MIN(SSRDI(a),SSRDI(w)),z,INT,I) R z;
  case SSINGDD: SSSTORENVFL(MIN(SSRDD(a),SSRDD(w)),z,FL,D) R z;
 }
}

SSINGF2(jtssmax) SSNUMPREFIX
 if(jt->jerr)R 0;  // If we have encountered error, give no result.  A bit kludgey, but that's how it was done.

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
// obsolete   case SSINGSS: SSSTORENV(SBMAX(SSRDS(a),SSRDS(w)),z,SBT,SB) R z;
  case SSINGBB: SSSTORENV(SSRDB(a)|SSRDB(w),z,B01,B) R z;
  case SSINGBD: SSSTORENV(MAX(SSRDB(a),SSRDD(w)),z,FL,D) R z;
  case SSINGDB: SSSTORENV(MAX(SSRDD(a),SSRDB(w)),z,FL,D) R z;
  case SSINGID: SSSTORE(MAX(SSRDI(a),SSRDD(w)),z,FL,D) R z;
  case SSINGDI: SSSTORE(MAX(SSRDD(a),SSRDI(w)),z,FL,D) R z;
  case SSINGBI: SSSTORENV(MAX(SSRDB(a),SSRDI(w)),z,INT,I) R z;
  case SSINGIB: SSSTORENV(MAX(SSRDI(a),SSRDB(w)),z,INT,I) R z;
  case SSINGII: SSSTORENV(MAX(SSRDI(a),SSRDI(w)),z,INT,I) R z;
  case SSINGDD: SSSTORENVFL(MAX(SSRDD(a),SSRDD(w)),z,FL,D) R z;
 }
}

SSINGF2(jtssmult) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 if(jt->jerr&&jt->jerr<EWOV)R 0;  // If we have encountered error, give no result.  A bit kludgey, but that's how it was done.
    // +/@, on sparse relied on this test: it didn't abort on NaN found during scan
 switch(sw) {
  default: R 0;
  case SSINGBB: SSSTORENV(SSRDB(a)&SSRDB(w),z,B01,B) R z;
  case SSINGBD: SSSTORENV(SSRDB(a)?SSRDD(w):0.0,z,FL,D) R z;
  case SSINGDB: SSSTORENV(SSRDB(w)?SSRDD(a):0.0,z,FL,D) R z;
  case SSINGID: {I av=SSRDI(a); SSSTORE(av?av*SSRDD(w):0.0,z,FL,D) R z;}
  case SSINGDI: {I wv=SSRDI(w); SSSTORE(wv?wv*SSRDD(a):0.0,z,FL,D) R z;}
  case SSINGBI: SSSTORENV(SSRDB(a)?SSRDI(w):0,z,INT,I) R z;
  case SSINGIB: SSSTORENV(SSRDB(w)?SSRDI(a):0,z,INT,I) R z;
  case SSINGII: {I av=SSRDI(a), wv=SSRDI(w), zv;
   if(!(av&&wv)) SSSTORENV(0L,z,INT,I)
   else if (zv=jtmult(0,av,wv)) SSSTORENV(zv,z,INT,I)  // 0 result on errors
   else SSSTORE((D)av*(D)wv,z,FL,D)
   R z;}
  case SSINGDD: {D av=SSRDD(a), wv=SSRDD(w);
   SSSTORENVFL(TYMES(av,wv),z,FL,D) R z;}
 }
}



SSINGF2(jtssdiv) SSNUMPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 if(jt->jerr&&jt->jerr!=EWRAT)R 0;  // If we have encountered error, give no result.  A bit kludgey, but that's how it was done.
 switch(sw) {
  default: R 0;
  case SSINGBB: {B av=SSRDB(a); B wv=SSRDB(w); SSSTORENV(wv?av:av?inf:0.0,z,FL,D) R z;}
  case SSINGBD: {B av=SSRDB(a); D wv=SSRDD(w); SSSTORENV((av||wv)?av/(D)wv:0.0,z,FL,D) R z;}
  case SSINGDB: {D av=SSRDD(a); B wv=SSRDB(w); SSSTORENV((av||wv)?av/(D)wv:0.0,z,FL,D) R z;}
  case SSINGID: {I av=SSRDI(a); D wv=SSRDD(w); SSSTORE((av||wv)?av/(D)wv:0.0,z,FL,D) R z;}
  case SSINGDI: {D av=SSRDD(a); I wv=SSRDI(w); SSSTORE((av||wv)?av/(D)wv:0.0,z,FL,D) R z;}
  case SSINGBI: {B av=SSRDB(a); I wv=SSRDI(w); SSSTORE((av||wv)?av/(D)wv:0.0,z,FL,D) R z;}
  case SSINGIB: {I av=SSRDI(a); B wv=SSRDB(w); SSSTORE((av||wv)?av/(D)wv:0.0,z,FL,D) R z;}
  case SSINGII: {I av=SSRDI(a); I wv=SSRDI(w); SSSTORE((av||wv)?av/(D)wv:0.0,z,FL,D) R z;}
  case SSINGDD: {NAN0; SSSTORENVFL(DIV(SSRDD(a),(D)SSRDD(w)),z,FL,D) NAN1; R z;}
 }
}


SSINGF2(jtssgcd) SSNUMPREFIX  I aiv,wiv; D adv,wdv,zdv;

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 if(jt->jerr)R 0;  // If we have encountered error, give no result.  A bit kludgey, but that's how it was done.
 switch(sw) {
  default: R 0;
  case SSINGBB: {SSSTORENV(SSRDB(a)|SSRDB(w),z,B01,B) R z;}
  case SSINGBD: {adv=SSRDB(a); wdv=SSRDD(w); goto flresult;}
  case SSINGDB: {adv=SSRDD(a); wdv=SSRDB(w); goto flresult;}
  case SSINGID: {adv=(D)SSRDI(a); wdv=SSRDD(w); goto flresult;}
  case SSINGDI: {adv=SSRDD(a); wdv=(D)SSRDI(w); goto flresult;}
  case SSINGBI: aiv=SSRDB(a); wiv=SSRDI(w); break;
  case SSINGIB: aiv=SSRDI(a); wiv=SSRDB(w); break;
  case SSINGII: aiv=SSRDI(a); wiv=SSRDI(w); break;
  case SSINGDD: {adv=SSRDD(a); wdv=SSRDD(w); goto flresult;}
 }
 I ziv=igcd(aiv,wiv); if(ziv||!jt->jerr){SSSTORE(ziv,z,INT,I) R z;}  // if no error, store an int
 if(jt->jerr<EWOV)R 0;  // If not overflow, what can it be?
 RESETERR; adv=(D)aiv; wdv=(D)wiv;  // Rack em up again; Convert int args to float, and fall through to float case
 flresult:
 if((zdv=dgcd(adv,wdv))||!jt->jerr){SSSTORE(zdv,z,FL,D) R z;}  // float result is the last fallback
 R 0;  // if there was an error, fail, jerr is set
}



SSINGF2(jtsslcm) SSNUMPREFIX  I aiv,wiv; D adv,wdv,zdv;
 if(jt->jerr)R 0;  // If we have encountered error, give no result.  A bit kludgey, but that's how it was done.
 switch(sw) {
  default: R 0;
  case SSINGBB: {SSSTORENV(SSRDB(a)&SSRDB(w),z,B01,B) R z;}
  case SSINGBD: {adv=SSRDB(a); wdv=SSRDD(w); goto flresult;}
  case SSINGDB: {adv=SSRDD(a); wdv=SSRDB(w); goto flresult;}
  case SSINGID: {adv=(D)SSRDI(a); wdv=SSRDD(w); goto flresult;}
  case SSINGDI: {adv=SSRDD(a); wdv=(D)SSRDI(w); goto flresult;}
  case SSINGBI: aiv=SSRDB(a); wiv=SSRDI(w); break;
  case SSINGIB: aiv=SSRDI(a); wiv=SSRDB(w); break;
  case SSINGII: aiv=SSRDI(a); wiv=SSRDI(w); break;
  case SSINGDD: {adv=SSRDD(a); wdv=SSRDD(w); goto flresult;}
 }
 I ziv=ilcm(aiv,wiv); if(!jt->jerr){SSSTORE(ziv,z,INT,I) R z;}  // if no error, store an int
 if(jt->jerr<EWOV)R 0;  // If not overflow, what can it be?
 RESETERR; adv=(D)aiv; wdv=(D)wiv;  // Rack em up again; Convert int args to float, and fall through to float case
 flresult:
 zdv=dlcm(adv,wdv); if(!jt->jerr){SSSTORE(zdv,z,FL,D) R z;}  // float result is the last fallback
 R 0;  // if there was an error, fail, jerr is set
}

SSINGF2(jtssnand) SSNUMPREFIX  I aiv,wiv; D adv,wdv;

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
  case SSINGBB: aiv = SSRDB(a); wiv = SSRDB(w); break;
  case SSINGBD: wdv=SSRDD(w); aiv = SSRDB(a); ASSERT(wdv==0.0 || teq(wdv,1.0),EVDOMAIN); wiv=(I)wdv; break;
  case SSINGDB: adv=SSRDD(a); wiv = SSRDB(w); ASSERT(adv==0.0 || teq(adv,1.0),EVDOMAIN); aiv=(I)adv; break;
  case SSINGID: wdv=SSRDD(w); aiv = SSRDI(a); ASSERT(!(aiv&-2) && (wdv==0.0 || teq(wdv,1.0)),EVDOMAIN); wiv=(I)wdv; break;
  case SSINGDI: adv=SSRDD(a); wiv = SSRDI(w); ASSERT(!(wiv&-2) && (adv==0.0 || teq(adv,1.0)),EVDOMAIN); aiv=(I)adv; break;
  case SSINGBI: aiv = SSRDB(a); wiv = SSRDI(w); ASSERT(!(wiv&-2),EVDOMAIN); break;
  case SSINGIB: aiv=SSRDI(a); wiv=SSRDB(w); ASSERT(!(aiv&-2),EVDOMAIN); break;
  case SSINGII: aiv=SSRDI(a); wiv=SSRDI(w); ASSERT(!((aiv|wiv)&-2),EVDOMAIN); break;
  case SSINGDD: adv=SSRDD(a); wdv=SSRDD(w); ASSERT((adv==0.0 || teq(adv,1.0)) && (wdv==0.0 || teq(wdv,1.0)),EVDOMAIN); wiv=(I)wdv; aiv=(I)adv; break;
 }
 SSSTORE((B)(1^(aiv&wiv)),z,B01,B) R z;
}


SSINGF2(jtssnor) SSNUMPREFIX  I aiv,wiv; D adv,wdv;

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
  case SSINGBB: aiv = SSRDB(a); wiv = SSRDB(w); break;
  case SSINGBD: wdv=SSRDD(w); aiv = SSRDB(a); ASSERT(wdv==0.0 || teq(wdv,1.0),EVDOMAIN); wiv=(I)wdv; break;
  case SSINGDB: adv=SSRDD(a); wiv = SSRDB(w); ASSERT(adv==0.0 || teq(adv,1.0),EVDOMAIN); aiv=(I)adv; break;
  case SSINGID: wdv=SSRDD(w); aiv = SSRDI(a); ASSERT(!(aiv&-2) && (wdv==0.0 || teq(wdv,1.0)),EVDOMAIN); wiv=(I)wdv; break;
  case SSINGDI: adv=SSRDD(a); wiv = SSRDI(w); ASSERT(!(wiv&-2) && (adv==0.0 || teq(adv,1.0)),EVDOMAIN); aiv=(I)adv; break;
  case SSINGBI: aiv = SSRDB(a); wiv = SSRDI(w); ASSERT(!(wiv&-2),EVDOMAIN); break;
  case SSINGIB: aiv=SSRDI(a); wiv=SSRDB(w); ASSERT(!(aiv&-2),EVDOMAIN); break;
  case SSINGII: aiv=SSRDI(a); wiv=SSRDI(w); ASSERT(!((aiv|wiv)&-2),EVDOMAIN); break;
  case SSINGDD: adv=SSRDD(a); wdv=SSRDD(w); ASSERT((adv==0.0 || teq(adv,1.0)) && (wdv==0.0 || teq(wdv,1.0)),EVDOMAIN); wiv=(I)wdv; aiv=(I)adv; break;
 }
 SSSTORE((B)(1^(aiv|wiv)),z,B01,B) R z;
}



SSINGF2(jtssoutof) SSNUMPREFIX  D adv,wdv,zdv;

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 if(jt->jerr)R 0;  // If we have encountered error, give no result.  A bit kludgey, but that's how it was done.
 switch(sw) {
  default: R 0;
  case SSINGBB: SSSTORENV(SSRDB(a)<=SSRDB(w),z,B01,B) R z;
  case SSINGBD: adv=SSRDB(a); wdv=SSRDD(w);  break;
  case SSINGDB: adv=SSRDD(a); wdv=SSRDB(w);  break;
  case SSINGID: adv=(D)SSRDI(a); wdv=SSRDD(w);  break;
  case SSINGDI: adv=SSRDD(a); wdv=(D)SSRDI(w);  break;
  case SSINGBI: adv=(D)SSRDB(a); wdv=(D)SSRDI(w); break;
  case SSINGIB: adv=(D)SSRDI(a); wdv=(D)SSRDB(w); break;
  case SSINGII: adv=(D)SSRDI(a); wdv=(D)SSRDI(w); break;
  case SSINGDD: adv=SSRDD(a); wdv=SSRDD(w);  break;
 }
 NAN0; zdv=bindd(adv,wdv); NAN1;
 SSSTORE(zdv,z,FL,D) R z;  // Return the value if valid
}

// Return int version of d, with error if loss of significance
static I intforD(J jt, D d){I z;
 z=(I)jfloor(d);
 if(!FEQ((D)z,d)){++z;
  // see if >: <.a is tolerantly equal to (I)a
  ASSERT(FEQ((D)z,d),EVDOMAIN);
 }
 R z;
}

SSINGF2OP(jtssbitwise) SSNUMPREFIX  I aiv,wiv,ziv;
 // Each operand must be convertible to integer
 switch(sw){  // type has been overwritten, must take it from here
 case SSINGIB: case SSINGII: case SSINGID:  // a is INT
  aiv=SSRDI(a); break;
 case SSINGDB: case SSINGDI: case SSINGDD:  // a is FL
  aiv=intforD(jt,SSRDD(a)); break;   // see if <.a is tolerantly equal to (I)a
 default:  // a is B01
  aiv=SSRDB(a);
 }

 switch(sw){  // type has been overwritten, must take it from here
 case SSINGBI: case SSINGII: case SSINGDI:  // w is INT
  wiv=SSRDI(w); break;
 case SSINGBD: case SSINGID: case SSINGDD:  // w is FL
  wiv=intforD(jt,SSRDD(w)); break;   // see if <.w is tolerantly equal to (I)a
 default:  // w is B01
  wiv=SSRDB(w);
 }

  RE(0);  // return if noninteger argument.  This will leave the error set, and fail quickly

 // We have the operands, now perform the operation
 switch(op){
  case 0: ziv=0; break;
  case 1: ziv=aiv&wiv; break;
  case 2: ziv=aiv&~wiv; break;
  case 3: ziv=aiv; break;
  case 4: ziv=(~aiv)&wiv; break;
  case 5: ziv=wiv; break;
  case 6: ziv=aiv^wiv; break;
  case 7: ziv=aiv|wiv; break;
  case 8: ziv=~(aiv|wiv); break;
  case 9: ziv=~(aiv^wiv); break;
  case 10: ziv=~wiv; break;
  case 11: ziv=aiv|~wiv; break;
  case 12: ziv=~aiv; break;
  case 13: ziv=(~aiv)|wiv; break;
  case 14: ziv=~(aiv&wiv); break;
  case 15: ziv=~0; break;
  case 16: ziv=0>aiv ? ((UI)wiv>>((-aiv)&(BW-1)))|((UI)wiv<<(aiv&(BW-1))) : ((UI)wiv<<(aiv&(BW-1)))|((UI)wiv>>((-aiv)&(BW-1))); break;  // rotate
  case 17: ziv=0>aiv ? (aiv<=-BW?0:(UI)wiv>>-aiv) : (aiv>=BW?0:(UI)wiv<<aiv); break;  // shift logical
  case 18: ziv=0>aiv ? (aiv<=-BW?(wiv<0?-1:0):wiv>>-aiv) : (aiv>=BW?0:wiv<<aiv); break;  // shift arithmetic
 }
 SSSTORE(ziv,z,INT,I) R z;
}

SSINGF2(jtsspow) SSNUMPREFIX
 if(jt->jerr&&jt->jerr<EWOV)R 0;  // If we have encountered error, give no result.  A bit kludgey, but that's how it was done.

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 // If the calculation fails (if it produces complex results, for example), we will exit with
 // jt->jerr set, and the normal code will start with the error routine
 switch(sw) { D t;
  default: R 0;
  case SSINGBB: SSSTORE((I)SSRDB(a)|(I)!SSRDB(w),z,INT,I) R z;  // normal code produces B01
  case SSINGBD: SSSTORE(SSRDB(a)?1.0:(t=SSRDD(w))<0?inf:t==0?1:0,z,FL,D) R z;
  case SSINGDB: SSSTORE(SSRDB(w)?SSRDD(a):1.0,z,FL,D) R z;
  case SSINGID: RE(t=pospow((D)SSRDI(a),SSRDD(w))) SSSTORE(t,z,FL,D) R z;
  case SSINGDI: RE(t=intpow(SSRDD(a),SSRDI(w))) SSSTORE(t,z,FL,D) R z;
  case SSINGBI: SSSTORE(SSRDB(a)?1.0:(t=(D)SSRDI(w))<0?inf:t==0?1:0,z,FL,D) R z;
  case SSINGIB: SSSTORE(SSRDB(w)?(D)SSRDI(a):1.0,z,FL,D) R z;
  case SSINGII: RE(t=intpow((D)SSRDI(a),SSRDI(w))) SSSTORE(t,z,FL,D) R z;
  case SSINGDD: RE(t=pospow(SSRDD(a),SSRDD(w))) SSSTORENVFL(t,z,FL,D) R z;
 }
}

// Comparisons:

SSINGF2(jtsslt) SSCOMPPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
// obsolete   case SSINGSS: zv=SBLT(SSRDS(a),SSRDS(w)); break;
  case SSINGBB: zv=SSRDB(a)<SSRDB(w); break;
  case SSINGBD: zv=TLT(SSRDB(a),SSRDD(w)); break;
  case SSINGDB: zv=TLT(SSRDD(a),SSRDB(w)); break;
  case SSINGID: zv=TLT((D)SSRDI(a),SSRDD(w)); break;
  case SSINGDI: zv=TLT(SSRDD(a),(D)SSRDI(w)); break;
  case SSINGBI: zv=SSRDB(a)<SSRDI(w); break;
  case SSINGIB: zv=SSRDI(a)<SSRDB(w); break;
  case SSINGII: zv=SSRDI(a)<SSRDI(w); break;
  case SSINGDD: zv=TLT(SSRDD(a),SSRDD(w)); break;
 }
 // zv is the Boolean value to return.  If there is an output block, the result must be non-atomic:
 // just store the value in it.  If there is no output block, return zero or one depending on the result
 if(z==0)R num[zv];
 BAV(z)[0] = zv; R z;
}

SSINGF2(jtssgt) SSCOMPPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
// obsolete   case SSINGSS: zv=SBGT(SSRDS(a),SSRDS(w)); break;
  case SSINGBB: zv=SSRDB(a)>SSRDB(w); break;
  case SSINGBD: zv=TGT(SSRDB(a),SSRDD(w)); break;
  case SSINGDB: zv=TGT(SSRDD(a),SSRDB(w)); break;
  case SSINGID: zv=TGT((D)SSRDI(a),SSRDD(w)); break;
  case SSINGDI: zv=TGT(SSRDD(a),(D)SSRDI(w)); break;
  case SSINGBI: zv=SSRDB(a)>SSRDI(w); break;
  case SSINGIB: zv=SSRDI(a)>SSRDB(w); break;
  case SSINGII: zv=SSRDI(a)>SSRDI(w); break;
  case SSINGDD: zv=TGT(SSRDD(a),SSRDD(w)); break;
 }
 // zv is the Boolean value to return.  If there is an output block, the result must be non-atomic:
 // just store the value in it.  If there is no output block, return zero or one depending on the result
 if(z==0)R num[zv];
 BAV(z)[0] = zv; R z;
}

SSINGF2(jtssle) SSCOMPPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
// obsolete   case SSINGSS: zv=SBLE(SSRDS(a),SSRDS(w)); break;
  case SSINGBB: zv=SSRDB(a)<=SSRDB(w); break;
  case SSINGBD: zv=TLE(SSRDB(a),SSRDD(w)); break;
  case SSINGDB: zv=TLE(SSRDD(a),SSRDB(w)); break;
  case SSINGID: zv=TLE((D)SSRDI(a),SSRDD(w)); break;
  case SSINGDI: zv=TLE(SSRDD(a),(D)SSRDI(w)); break;
  case SSINGBI: zv=SSRDB(a)<=SSRDI(w); break;
  case SSINGIB: zv=SSRDI(a)<=SSRDB(w); break;
  case SSINGII: zv=SSRDI(a)<=SSRDI(w); break;
  case SSINGDD: zv=TLE(SSRDD(a),SSRDD(w)); break;
 }
 // zv is the Boolean value to return.  If there is an output block, the result must be non-atomic:
 // just store the value in it.  If there is no output block, return zero or one depending on the result
 if(z==0)R num[zv];
 BAV(z)[0] = zv; R z;
}

SSINGF2(jtssge) SSCOMPPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8
 switch(sw) {
  default: R 0;
// obsolete   case SSINGSS: zv=SBGE(SSRDS(a),SSRDS(w)); break;
  case SSINGBB: zv=SSRDB(a)>=SSRDB(w); break;
  case SSINGBD: zv=TGE(SSRDB(a),SSRDD(w)); break;
  case SSINGDB: zv=TGE(SSRDD(a),SSRDB(w)); break;
  case SSINGID: zv=TGE((D)SSRDI(a),SSRDD(w)); break;
  case SSINGDI: zv=TGE(SSRDD(a),(D)SSRDI(w)); break;
  case SSINGBI: zv=SSRDB(a)>=SSRDI(w); break;
  case SSINGIB: zv=SSRDI(a)>=SSRDB(w); break;
  case SSINGII: zv=SSRDI(a)>=SSRDI(w); break;
  case SSINGDD: zv=TGE(SSRDD(a),SSRDD(w)); break;
 }
 // zv is the Boolean value to return.  If there is an output block, the result must be non-atomic:
 // just store the value in it.  If there is no output block, return zero or one depending on the result
 if(z==0)R num[zv];
 BAV(z)[0] = zv; R z;
}


SSINGF2OP(jtsseqne) SSCOMPPREFIX

 // Switch on the types; do the operation, store the result, set the type of result
 // types are 1, 4, or 8, or here only LIT, C2T and C4T, and SBT
 if(sw<C2T){
  switch(sw) {
   case SSINGCC: zv=SSRDC(a)==SSRDC(w); break;
   case SSINGBB: zv=SSRDB(a)==SSRDB(w); break;
   case SSINGBD: zv=TEQ(SSRDB(a),SSRDD(w)); break;
   case SSINGDB: zv=TEQ(SSRDD(a),SSRDB(w)); break; 
   case SSINGID: zv=TEQ((D)SSRDI(a),SSRDD(w)); break;
   case SSINGDI: zv=TEQ(SSRDD(a),(D)SSRDI(w)); break;
   case SSINGBI: zv=SSRDB(a)==SSRDI(w); break;
   case SSINGIB: zv=SSRDI(a)==SSRDB(w); break;
   case SSINGII: zv=SSRDI(a)==SSRDI(w); break;
   case SSINGDD: zv=TEQ(SSRDD(a),SSRDD(w)); break;
   default: zv=0; break;   // INHOMO args come here, eg LIT=B01
  }
 }else{
  if(sw<(C2T*5) && sw&((FL|INT|B01)<<2))zv=0;  // a was C2T/C4T and INHOMO (1/4/8 before *5)
  else if(sw>=(C2T*5) && sw&(FL|INT|B01))zv=0;  // b was C2T/C4T and INHOMO
  else switch(sw>>C2TX) {
   default: R 0;
   case SSINGCW>>C2TX: zv=SSRDC(a)==SSRDW(w); break;
   case SSINGCU>>C2TX: zv=SSRDC(a)==SSRDU(w); break;
   case SSINGWC>>C2TX: zv=SSRDW(a)==SSRDC(w); break;
   case SSINGWW>>C2TX: zv=SSRDW(a)==SSRDW(w); break;
   case SSINGWU>>C2TX: zv=SSRDW(a)==SSRDU(w); break;
   case SSINGUC>>C2TX: zv=SSRDU(a)==SSRDC(w); break;
   case SSINGUW>>C2TX: zv=SSRDU(a)==SSRDW(w); break;
   case SSINGUU>>C2TX: zv=SSRDU(a)==SSRDU(w); break;
  }
 }
 // zv is the Boolean value to return.  If there is an output block, the result must be non-atomic:
 // just store the value in it.  If there is no output block, return zero or one depending on the result
 if(z==0)R num[zv^(B)op];
 BAV(z)[0] = zv^(B)op; R z;
}

