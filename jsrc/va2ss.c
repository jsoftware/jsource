/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
// Verbs: Atomic (Scalar) Dyadic when arguments have one atom
// Support for Speedy Singletons

#include "j.h"
#include "ve.h"
#include "vcomp.h"

#define SSRDB(w) (*(B *)CAV(w))
#define SSRDI(w) (*(I *)CAV(w))
#define SSRDS(w) (*(SB*)CAV(w))
#define SSRDC(w) (*(UC*)CAV(w))
#define SSRDW(w) (*(US*)CAV(w))
#define SSRDU(w) (*(C4*)CAV(w))
#define SSRDD(w) (*(D *)CAV(w))

#define SSINGENC(a,w) (3*(UNSAFE(a)>>INTX)+(UNSAFE(w)>>INTX))
#define SSINGBB SSINGENC(B01,B01)
#define SSINGBI SSINGENC(B01,INT)
#define SSINGBD SSINGENC(B01,FL)
#define SSINGIB SSINGENC(INT,B01)
#define SSINGII SSINGENC(INT,INT)
#define SSINGID SSINGENC(INT,FL)
#define SSINGDB SSINGENC(FL,B01)
#define SSINGDI SSINGENC(FL,INT)
#define SSINGDD SSINGENC(FL,FL)

#define SSSTORENV(v,z,t,type) {*((type *)CAV(z)) = (v); AT(z)=(t); }  // When we know that if the block is reused, we are not changing the type; but we change the type of a new block
#define SSSTORE(v,z,t,type) SSSTORENV(v,z,t,type)  // we don't use MODBLOCKTYPE any more
#define SSSTORENVFL(v,z,t,type) {*((type *)CAV(z)) = (v); }  // When we know the type/shape doesn't change (FL,FL->FL)

// Return int version of d, with error if loss of significance
static I intforD(J jt, D d){D q;I z;
 q=jround(d); z=(I)q;
 ASSERT(d==q || FFIEQ(d,q),EVDOMAIN);
 // too-large values don't convert, handle separately
 if(d<(D)IMIN){ASSERT(d>=IMIN*(1+FUZZ),EVDOMAIN); z=IMIN;}  // if tolerantly < IMIN, error; else take IMIN
 else if(d>=-(D)IMIN){ASSERT(d<=IMAX*(1+FUZZ),EVDOMAIN); z=IMAX;}  // if tolerantly > IMAX, error; else take IMAX
 R z;
}

#define SSINGCASE(id,subtype) (9*(id)+(subtype))   // encode case/args into one branch value
A jtssingleton(J jt, A a,A w,A self,RANK2T awr,RANK2T ranks){A z;
 F2PREFIP;
 I aiv=FAV(self)->lc;   // temp, but start as function #
 // Allocate the result area
 {
  // Calculate inplaceability for a and w.  Result must be 0 or 1
  // Inplaceable if: count=1 and zombieval, or count<0, PROVIDED the arg is inplaceable and the block is not UNINCORPABLE
  I aipok = ((((AC(a)-1)|((I)a^(I)jt->zombieval))==0)|(SGNTO0(AC(a)))) & ((UI)jtinplace>>JTINPLACEAX) & ~(AFLAG(a)>>AFUNINCORPABLEX);
  I wipok = ((((AC(w)-1)|((I)w^(I)jt->zombieval))==0)|(SGNTO0(AC(w)))) & ((UI)jtinplace>>JTINPLACEWX) & ~(AFLAG(w)>>AFUNINCORPABLEX);
  z=0;
  // find or allocate the result area
  if(awr==0){  // both atoms
   // The usual case: both singletons are atoms.  Verb rank is immaterial.  Pick any inplaceable input, or allocate a FL atom if none
   // For comparison operations leave z=0 rather than allocate, so we can use num[].  But if we can inplace that's better
   z=aipok?a:z; z=wipok?w:z; if(!((I)z|(aiv&0x80)))GAT0(z,FL,1,0);  // top  bit of lc means 'comparison'
  }else{
   // The nouns have rank, and thus there may be frames.  Calculate the rank of the result, and then see if we can inplace an argument of the needed rank
   I4 fa=(awr>>RANKTX)-(ranks>>RANKTX); fa=fa<0?0:fa;  // framelen of a - will become larger frame, and then desired rank
   I4 fw=(awr&RANKTMSK)-(ranks&RANKTMSK); fw=fw<0?0:fw;  // framelen of w
   I4 ca=(awr>>RANKTX)-fa; I4 cw=(awr&RANKTMSK)-fw; ca=ca<cw?cw:ca;  // ca=larger cell-rank
   fa=fa<fw?fw:fa; fa+=ca;  // fa=larger framelen, and then desired rank
   z=aipok>((I)(awr>>RANKTX)^fa)?a:z; z=wipok>((I)(awr&RANKTMSK)^fa)?w:z; if(!z)GATV1(z,FL,1,fa);  // accept ip only if rank also matches; if we allocate, fill in the shape with 1s
  }
 }

 I wiv,ziv;D adv,wdv,zdv;
 // Huge switch statement to handle every case.  Lump all the booleans together at 0
 aiv=(aiv&0x7f)-VA2BF; aiv=aiv<0?0:aiv;
 switch(SSINGCASE(aiv,SSINGENC(AT(a),AT(w)))){
 default: ASSERTSYS(0,"ssing");
 case SSINGCASE(VA2PLUS-VA2BF,SSINGBB): SSSTORENV(SSRDB(a)+SSRDB(w),z,INT,I) R z;  // NV because B01 is never virtual inplace
 case SSINGCASE(VA2PLUS-VA2BF,SSINGBD): SSSTORENV(SSRDB(a)+SSRDD(w),z,FL,D) R z;
 case SSINGCASE(VA2PLUS-VA2BF,SSINGDB): SSSTORENV(SSRDD(a)+SSRDB(w),z,FL,D) R z;
 case SSINGCASE(VA2PLUS-VA2BF,SSINGID): SSSTORE(SSRDI(a)+SSRDD(w),z,FL,D) R z;
 case SSINGCASE(VA2PLUS-VA2BF,SSINGDI): SSSTORE(SSRDD(a)+SSRDI(w),z,FL,D) R z;
 case SSINGCASE(VA2PLUS-VA2BF,SSINGBI): 
  {B av = SSRDB(a); I wv = SSRDI(w); I zv = av+wv;
  if(zv>=wv)SSSTORENV(zv,z,INT,I) else SSSTORE((D)av+(D)wv,z,FL,D)
  R z;}
 case SSINGCASE(VA2PLUS-VA2BF,SSINGIB):
  {I av = SSRDI(a); B wv = SSRDB(w); I zv = av + wv;
  if (zv>=av)SSSTORENV(zv,z,INT,I) else SSSTORE((D)av+(D)wv,z,FL,D)
  R z;}
 case SSINGCASE(VA2PLUS-VA2BF,SSINGII):
  {I av = SSRDI(a); I wv = SSRDI(w); I zv = av + wv;
  if (XANDY((zv^av),(zv^wv))>=0)SSSTORENV(zv,z,INT,I) else SSSTORE((D)av+(D)wv,z,FL,D)
  R z;}
 case SSINGCASE(VA2PLUS-VA2BF,SSINGDD):
  {NAN0; SSSTORENVFL(SSRDD(a)+SSRDD(w),z,FL,D) NAN1; R z;}


 case SSINGCASE(VA2MINUS-VA2BF,SSINGBB): SSSTORENV(SSRDB(a)-SSRDB(w),z,INT,I) R z;
 case SSINGCASE(VA2MINUS-VA2BF,SSINGBD): SSSTORENV(SSRDB(a)-SSRDD(w),z,FL,D) R z;
 case SSINGCASE(VA2MINUS-VA2BF,SSINGDB): SSSTORENV(SSRDD(a)-SSRDB(w),z,FL,D) R z;
 case SSINGCASE(VA2MINUS-VA2BF,SSINGID): SSSTORE(SSRDI(a)-SSRDD(w),z,FL,D) R z;
 case SSINGCASE(VA2MINUS-VA2BF,SSINGDI): SSSTORE(SSRDD(a)-SSRDI(w),z,FL,D) R z;
 case SSINGCASE(VA2MINUS-VA2BF,SSINGBI): 
  {B av = SSRDB(a); I wv = SSRDI(w); I zv = av-wv;
  if((wv&zv)>=0)SSSTORENV(zv,z,INT,I) else SSSTORE((D)av-(D)wv,z,FL,D)
  R z;}
 case SSINGCASE(VA2MINUS-VA2BF,SSINGIB):
  {I av = SSRDI(a); I wv = (I)SSRDB(w); I zv = av - wv;   
  if (zv<=av)SSSTORENV(zv,z,INT,I) else SSSTORE((D)av-(D)wv,z,FL,D)
  R z;}
 case SSINGCASE(VA2MINUS-VA2BF,SSINGII):
  {I av = SSRDI(a); I wv = SSRDI(w); I zv = av - wv;
  if (XANDY((zv^av),~(zv^wv))>=0)SSSTORENV(zv,z,INT,I) else SSSTORE((D)av-(D)wv,z,FL,D)
  R z;}
 case SSINGCASE(VA2MINUS-VA2BF,SSINGDD):
  {NAN0; SSSTORENVFL(SSRDD(a)-SSRDD(w),z,FL,D) NAN1;  R z;}


 case SSINGCASE(VA2MIN-VA2BF,SSINGBB): SSSTORENV(SSRDB(a)&SSRDB(w),z,B01,B) R z;
 case SSINGCASE(VA2MIN-VA2BF,SSINGBD): SSSTORENV(MIN(SSRDB(a),SSRDD(w)),z,FL,D) R z;
 case SSINGCASE(VA2MIN-VA2BF,SSINGDB): SSSTORENV(MIN(SSRDD(a),SSRDB(w)),z,FL,D) R z;
 case SSINGCASE(VA2MIN-VA2BF,SSINGID): SSSTORE(MIN(SSRDI(a),SSRDD(w)),z,FL,D) R z;
 case SSINGCASE(VA2MIN-VA2BF,SSINGDI): SSSTORE(MIN(SSRDD(a),SSRDI(w)),z,FL,D) R z;
 case SSINGCASE(VA2MIN-VA2BF,SSINGBI): SSSTORENV(MIN(SSRDB(a),SSRDI(w)),z,INT,I) R z;
 case SSINGCASE(VA2MIN-VA2BF,SSINGIB): SSSTORENV(MIN(SSRDI(a),SSRDB(w)),z,INT,I) R z;
 case SSINGCASE(VA2MIN-VA2BF,SSINGII): SSSTORENV(MIN(SSRDI(a),SSRDI(w)),z,INT,I) R z;
 case SSINGCASE(VA2MIN-VA2BF,SSINGDD): SSSTORENVFL(MIN(SSRDD(a),SSRDD(w)),z,FL,D) R z;


 case SSINGCASE(VA2MAX-VA2BF,SSINGBB): SSSTORENV(SSRDB(a)|SSRDB(w),z,B01,B) R z;
 case SSINGCASE(VA2MAX-VA2BF,SSINGBD): SSSTORENV(MAX(SSRDB(a),SSRDD(w)),z,FL,D) R z;
 case SSINGCASE(VA2MAX-VA2BF,SSINGDB): SSSTORENV(MAX(SSRDD(a),SSRDB(w)),z,FL,D) R z;
 case SSINGCASE(VA2MAX-VA2BF,SSINGID): SSSTORE(MAX(SSRDI(a),SSRDD(w)),z,FL,D) R z;
 case SSINGCASE(VA2MAX-VA2BF,SSINGDI): SSSTORE(MAX(SSRDD(a),SSRDI(w)),z,FL,D) R z;
 case SSINGCASE(VA2MAX-VA2BF,SSINGBI): SSSTORENV(MAX(SSRDB(a),SSRDI(w)),z,INT,I) R z;
 case SSINGCASE(VA2MAX-VA2BF,SSINGIB): SSSTORENV(MAX(SSRDI(a),SSRDB(w)),z,INT,I) R z;
 case SSINGCASE(VA2MAX-VA2BF,SSINGII): SSSTORENV(MAX(SSRDI(a),SSRDI(w)),z,INT,I) R z;
 case SSINGCASE(VA2MAX-VA2BF,SSINGDD): SSSTORENVFL(MAX(SSRDD(a),SSRDD(w)),z,FL,D) R z;


 case SSINGCASE(VA2MULT-VA2BF,SSINGBB): SSSTORENV(SSRDB(a)&SSRDB(w),z,B01,B) R z;
 case SSINGCASE(VA2MULT-VA2BF,SSINGBD): SSSTORENV(SSRDB(a)?SSRDD(w):0.0,z,FL,D) R z;
 case SSINGCASE(VA2MULT-VA2BF,SSINGDB): SSSTORENV(SSRDB(w)?SSRDD(a):0.0,z,FL,D) R z;
 case SSINGCASE(VA2MULT-VA2BF,SSINGID): {I av=SSRDI(a); SSSTORE(av?av*SSRDD(w):0.0,z,FL,D) R z;}
 case SSINGCASE(VA2MULT-VA2BF,SSINGDI): {I wv=SSRDI(w); SSSTORE(wv?wv*SSRDD(a):0.0,z,FL,D) R z;}
 case SSINGCASE(VA2MULT-VA2BF,SSINGBI): SSSTORENV(SSRDB(a)?SSRDI(w):0,z,INT,I) R z;
 case SSINGCASE(VA2MULT-VA2BF,SSINGIB): SSSTORENV(SSRDB(w)?SSRDI(a):0,z,INT,I) R z;
 case SSINGCASE(VA2MULT-VA2BF,SSINGII): {I av=SSRDI(a), wv=SSRDI(w), zv;
   if(!(av&&wv)) SSSTORENV(0L,z,INT,I)
   else if (zv=jtmult(0,av,wv)) SSSTORENV(zv,z,INT,I)  // 0 result on errors
   else SSSTORE((D)av*(D)wv,z,FL,D)
   R z;}
 case SSINGCASE(VA2MULT-VA2BF,SSINGDD): {D av=SSRDD(a), wv=SSRDD(w);
   SSSTORENVFL(TYMES(av,wv),z,FL,D) R z;}


 case SSINGCASE(VA2DIV-VA2BF,SSINGBB): {B av=SSRDB(a); B wv=SSRDB(w); SSSTORENV(wv?av:av?inf:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2DIV-VA2BF,SSINGBD): {B av=SSRDB(a); D wv=SSRDD(w); SSSTORENV((av||wv)?av/(D)wv:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2DIV-VA2BF,SSINGDB): {D av=SSRDD(a); B wv=SSRDB(w); SSSTORENV((av||wv)?av/(D)wv:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2DIV-VA2BF,SSINGID): {I av=SSRDI(a); D wv=SSRDD(w); SSSTORE((av||wv)?av/(D)wv:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2DIV-VA2BF,SSINGDI): {D av=SSRDD(a); I wv=SSRDI(w); SSSTORE((av||wv)?av/(D)wv:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2DIV-VA2BF,SSINGBI): {B av=SSRDB(a); I wv=SSRDI(w); SSSTORE((av||wv)?av/(D)wv:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2DIV-VA2BF,SSINGIB): {I av=SSRDI(a); B wv=SSRDB(w); SSSTORE((av||wv)?av/(D)wv:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2DIV-VA2BF,SSINGII): {I av=SSRDI(a); I wv=SSRDI(w); SSSTORE((av||wv)?av/(D)wv:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2DIV-VA2BF,SSINGDD): {NAN0; SSSTORENVFL(DIV(SSRDD(a),(D)SSRDD(w)),z,FL,D) NAN1; R z;}


 case SSINGCASE(VA2GCD-VA2BF,SSINGBB): {SSSTORENV(SSRDB(a)|SSRDB(w),z,B01,B) R z;}
 case SSINGCASE(VA2GCD-VA2BF,SSINGBD): {adv=SSRDB(a); wdv=SSRDD(w); goto gcdflresult;}
 case SSINGCASE(VA2GCD-VA2BF,SSINGDB): {adv=SSRDD(a); wdv=SSRDB(w); goto gcdflresult;}
 case SSINGCASE(VA2GCD-VA2BF,SSINGID): {adv=(D)SSRDI(a); wdv=SSRDD(w); goto gcdflresult;}
 case SSINGCASE(VA2GCD-VA2BF,SSINGDI): {adv=SSRDD(a); wdv=(D)SSRDI(w); goto gcdflresult;}
 case SSINGCASE(VA2GCD-VA2BF,SSINGBI): aiv=SSRDB(a); wiv=SSRDI(w); goto gcdintresult;
 case SSINGCASE(VA2GCD-VA2BF,SSINGIB): aiv=SSRDI(a); wiv=SSRDB(w); goto gcdintresult;
 case SSINGCASE(VA2GCD-VA2BF,SSINGII): aiv=SSRDI(a); wiv=SSRDI(w); goto gcdintresult;
 case SSINGCASE(VA2GCD-VA2BF,SSINGDD): {adv=SSRDD(a); wdv=SSRDD(w); goto gcdflresult;}


 case SSINGCASE(VA2LCM-VA2BF,SSINGBB): {SSSTORENV(SSRDB(a)&SSRDB(w),z,B01,B) R z;}
 case SSINGCASE(VA2LCM-VA2BF,SSINGBD): {adv=SSRDB(a); wdv=SSRDD(w); goto lcmflresult;}
 case SSINGCASE(VA2LCM-VA2BF,SSINGDB): {adv=SSRDD(a); wdv=SSRDB(w); goto lcmflresult;}
 case SSINGCASE(VA2LCM-VA2BF,SSINGID): {adv=(D)SSRDI(a); wdv=SSRDD(w); goto lcmflresult;}
 case SSINGCASE(VA2LCM-VA2BF,SSINGDI): {adv=SSRDD(a); wdv=(D)SSRDI(w); goto lcmflresult;}
 case SSINGCASE(VA2LCM-VA2BF,SSINGBI): aiv=SSRDB(a); wiv=SSRDI(w); goto lcmintresult;
 case SSINGCASE(VA2LCM-VA2BF,SSINGIB): aiv=SSRDI(a); wiv=SSRDB(w); goto lcmintresult;
 case SSINGCASE(VA2LCM-VA2BF,SSINGII): aiv=SSRDI(a); wiv=SSRDI(w); goto lcmintresult;
 case SSINGCASE(VA2LCM-VA2BF,SSINGDD): {adv=SSRDD(a); wdv=SSRDD(w); goto lcmflresult;}


 case SSINGCASE(VA2NAND-VA2BF,SSINGBB): aiv = SSRDB(a); wiv = SSRDB(w); goto nandresult;
 case SSINGCASE(VA2NAND-VA2BF,SSINGBD): wdv=SSRDD(w); aiv = SSRDB(a); ASSERT(wdv==0.0 || TEQ(wdv,1.0),EVDOMAIN); wiv=(I)wdv; goto nandresult;
 case SSINGCASE(VA2NAND-VA2BF,SSINGDB): adv=SSRDD(a); wiv = SSRDB(w); ASSERT(adv==0.0 || TEQ(adv,1.0),EVDOMAIN); aiv=(I)adv; goto nandresult;
 case SSINGCASE(VA2NAND-VA2BF,SSINGID): wdv=SSRDD(w); aiv = SSRDI(a); ASSERT(!(aiv&-2) && (wdv==0.0 || TEQ(wdv,1.0)),EVDOMAIN); wiv=(I)wdv; goto nandresult;
 case SSINGCASE(VA2NAND-VA2BF,SSINGDI): adv=SSRDD(a); wiv = SSRDI(w); ASSERT(!(wiv&-2) && (adv==0.0 || TEQ(adv,1.0)),EVDOMAIN); aiv=(I)adv; goto nandresult;
 case SSINGCASE(VA2NAND-VA2BF,SSINGBI): aiv = SSRDB(a); wiv = SSRDI(w); ASSERT(!(wiv&-2),EVDOMAIN); goto nandresult;
 case SSINGCASE(VA2NAND-VA2BF,SSINGIB): aiv=SSRDI(a); wiv=SSRDB(w); ASSERT(!(aiv&-2),EVDOMAIN); goto nandresult;
 case SSINGCASE(VA2NAND-VA2BF,SSINGII): aiv=SSRDI(a); wiv=SSRDI(w); ASSERT(!((aiv|wiv)&-2),EVDOMAIN); goto nandresult;
 case SSINGCASE(VA2NAND-VA2BF,SSINGDD): adv=SSRDD(a); wdv=SSRDD(w); ASSERT((adv==0.0 || TEQ(adv,1.0)) && (wdv==0.0 || TEQ(wdv,1.0)),EVDOMAIN); wiv=(I)wdv; aiv=(I)adv; goto nandresult;


 case SSINGCASE(VA2NOR-VA2BF,SSINGBB): aiv = SSRDB(a); wiv = SSRDB(w); goto norresult;
 case SSINGCASE(VA2NOR-VA2BF,SSINGBD): wdv=SSRDD(w); aiv = SSRDB(a); ASSERT(wdv==0.0 || TEQ(wdv,1.0),EVDOMAIN); wiv=(I)wdv; goto norresult;
 case SSINGCASE(VA2NOR-VA2BF,SSINGDB): adv=SSRDD(a); wiv = SSRDB(w); ASSERT(adv==0.0 || TEQ(adv,1.0),EVDOMAIN); aiv=(I)adv; goto norresult;
 case SSINGCASE(VA2NOR-VA2BF,SSINGID): wdv=SSRDD(w); aiv = SSRDI(a); ASSERT(!(aiv&-2) && (wdv==0.0 || TEQ(wdv,1.0)),EVDOMAIN); wiv=(I)wdv; goto norresult;
 case SSINGCASE(VA2NOR-VA2BF,SSINGDI): adv=SSRDD(a); wiv = SSRDI(w); ASSERT(!(wiv&-2) && (adv==0.0 || TEQ(adv,1.0)),EVDOMAIN); aiv=(I)adv; goto norresult;
 case SSINGCASE(VA2NOR-VA2BF,SSINGBI): aiv = SSRDB(a); wiv = SSRDI(w); ASSERT(!(wiv&-2),EVDOMAIN); goto norresult;
 case SSINGCASE(VA2NOR-VA2BF,SSINGIB): aiv=SSRDI(a); wiv=SSRDB(w); ASSERT(!(aiv&-2),EVDOMAIN); goto norresult;
 case SSINGCASE(VA2NOR-VA2BF,SSINGII): aiv=SSRDI(a); wiv=SSRDI(w); ASSERT(!((aiv|wiv)&-2),EVDOMAIN); goto norresult;
 case SSINGCASE(VA2NOR-VA2BF,SSINGDD): adv=SSRDD(a); wdv=SSRDD(w); ASSERT((adv==0.0 || TEQ(adv,1.0)) && (wdv==0.0 || TEQ(wdv,1.0)),EVDOMAIN); wiv=(I)wdv; aiv=(I)adv; goto norresult;


 case SSINGCASE(VA2OUTOF-VA2BF,SSINGBB): SSSTORENV(SSRDB(a)<=SSRDB(w),z,B01,B) R z;
 case SSINGCASE(VA2OUTOF-VA2BF,SSINGBD): adv=SSRDB(a); wdv=SSRDD(w);  goto outofresult;
 case SSINGCASE(VA2OUTOF-VA2BF,SSINGDB): adv=SSRDD(a); wdv=SSRDB(w);  goto outofresult;
 case SSINGCASE(VA2OUTOF-VA2BF,SSINGID): adv=(D)SSRDI(a); wdv=SSRDD(w);  goto outofresult;
 case SSINGCASE(VA2OUTOF-VA2BF,SSINGDI): adv=SSRDD(a); wdv=(D)SSRDI(w);  goto outofresult;
 case SSINGCASE(VA2OUTOF-VA2BF,SSINGBI): adv=(D)SSRDB(a); wdv=(D)SSRDI(w); goto outofresultcvti;
 case SSINGCASE(VA2OUTOF-VA2BF,SSINGIB): adv=(D)SSRDI(a); wdv=(D)SSRDB(w); goto outofresultcvti;
 case SSINGCASE(VA2OUTOF-VA2BF,SSINGII): adv=(D)SSRDI(a); wdv=(D)SSRDI(w); goto outofresultcvti;
 case SSINGCASE(VA2OUTOF-VA2BF,SSINGDD): adv=SSRDD(a); wdv=SSRDD(w);  goto outofresult;


 case SSINGCASE(VA2POW-VA2BF,SSINGBB): SSSTORE((I)SSRDB(a)|(I)!SSRDB(w),z,INT,I) R z;  // normal code produces B01
 case SSINGCASE(VA2POW-VA2BF,SSINGBD): SSSTORE(SSRDB(a)?1.0:(zdv=SSRDD(w))<0?inf:zdv==0?1:0,z,FL,D) R z;
 case SSINGCASE(VA2POW-VA2BF,SSINGDB): SSSTORE(SSRDB(w)?SSRDD(a):1.0,z,FL,D) R z;
 case SSINGCASE(VA2POW-VA2BF,SSINGID): RE(zdv=pospow((D)SSRDI(a),SSRDD(w))) SSSTORE(zdv,z,FL,D) R z;
 case SSINGCASE(VA2POW-VA2BF,SSINGDI): RE(zdv=intpow(SSRDD(a),SSRDI(w))) SSSTORE(zdv,z,FL,D) R z;
 case SSINGCASE(VA2POW-VA2BF,SSINGBI): SSSTORE(SSRDB(a)?1.0:(zdv=(D)SSRDI(w))<0?inf:zdv==0?1:0,z,FL,D) R z;
 case SSINGCASE(VA2POW-VA2BF,SSINGIB): SSSTORE(SSRDB(w)?(D)SSRDI(a):1.0,z,FL,D) R z;
 case SSINGCASE(VA2POW-VA2BF,SSINGII): RE(zdv=intpow((D)SSRDI(a),SSRDI(w))) SSSTORE(zdv,z,FL,D) R z;
 case SSINGCASE(VA2POW-VA2BF,SSINGDD): RE(zdv=pospow(SSRDD(a),SSRDD(w))) SSSTORENVFL(zdv,z,FL,D) R z;


 case SSINGCASE(VA2BF-VA2BF,SSINGBB): aiv=SSRDB(a); wiv=SSRDB(w); goto bitwiseresult;
 case SSINGCASE(VA2BF-VA2BF,SSINGBD): aiv=SSRDB(a); wiv=intforD(jt,SSRDD(w)); goto bitwiseresult;
 case SSINGCASE(VA2BF-VA2BF,SSINGDB): aiv=intforD(jt,SSRDD(a)); wiv=SSRDB(w); goto bitwiseresult;
 case SSINGCASE(VA2BF-VA2BF,SSINGID): aiv=SSRDI(a); wiv=intforD(jt,SSRDD(w)); goto bitwiseresult;
 case SSINGCASE(VA2BF-VA2BF,SSINGDI): aiv=intforD(jt,SSRDD(a)); wiv=SSRDI(w); goto bitwiseresult;
 case SSINGCASE(VA2BF-VA2BF,SSINGBI): aiv=SSRDB(a); wiv=SSRDI(w); goto bitwiseresult;
 case SSINGCASE(VA2BF-VA2BF,SSINGIB): aiv=SSRDI(a); wiv=SSRDB(w); goto bitwiseresult;
 case SSINGCASE(VA2BF-VA2BF,SSINGII): aiv=SSRDI(a); wiv=SSRDI(w); goto bitwiseresult;
 case SSINGCASE(VA2BF-VA2BF,SSINGDD): aiv=intforD(jt,SSRDD(a)); wiv=intforD(jt,SSRDD(w)); goto bitwiseresult;


 case SSINGCASE(VA2LT-VA2BF,SSINGBB): ziv=SSRDB(a)<SSRDB(w); goto compareresult;
 case SSINGCASE(VA2LT-VA2BF,SSINGBD): ziv=TLT(SSRDB(a),SSRDD(w)); goto compareresult;
 case SSINGCASE(VA2LT-VA2BF,SSINGDB): ziv=TLT(SSRDD(a),SSRDB(w)); goto compareresult;
 case SSINGCASE(VA2LT-VA2BF,SSINGID): ziv=TLT((D)SSRDI(a),SSRDD(w)); goto compareresult;
 case SSINGCASE(VA2LT-VA2BF,SSINGDI): ziv=TLT(SSRDD(a),(D)SSRDI(w)); goto compareresult;
 case SSINGCASE(VA2LT-VA2BF,SSINGBI): ziv=SSRDB(a)<SSRDI(w); goto compareresult;
 case SSINGCASE(VA2LT-VA2BF,SSINGIB): ziv=SSRDI(a)<SSRDB(w); goto compareresult;
 case SSINGCASE(VA2LT-VA2BF,SSINGII): ziv=SSRDI(a)<SSRDI(w); goto compareresult;
 case SSINGCASE(VA2LT-VA2BF,SSINGDD): ziv=TLT(SSRDD(a),SSRDD(w)); goto compareresult;


 case SSINGCASE(VA2GT-VA2BF,SSINGBB): ziv=SSRDB(a)>SSRDB(w); goto compareresult;
 case SSINGCASE(VA2GT-VA2BF,SSINGBD): ziv=TGT(SSRDB(a),SSRDD(w)); goto compareresult;
 case SSINGCASE(VA2GT-VA2BF,SSINGDB): ziv=TGT(SSRDD(a),SSRDB(w)); goto compareresult;
 case SSINGCASE(VA2GT-VA2BF,SSINGID): ziv=TGT((D)SSRDI(a),SSRDD(w)); goto compareresult;
 case SSINGCASE(VA2GT-VA2BF,SSINGDI): ziv=TGT(SSRDD(a),(D)SSRDI(w)); goto compareresult;
 case SSINGCASE(VA2GT-VA2BF,SSINGBI): ziv=SSRDB(a)>SSRDI(w); goto compareresult;
 case SSINGCASE(VA2GT-VA2BF,SSINGIB): ziv=SSRDI(a)>SSRDB(w); goto compareresult;
 case SSINGCASE(VA2GT-VA2BF,SSINGII): ziv=SSRDI(a)>SSRDI(w); goto compareresult;
 case SSINGCASE(VA2GT-VA2BF,SSINGDD): ziv=TGT(SSRDD(a),SSRDD(w)); goto compareresult;


 case SSINGCASE(VA2LE-VA2BF,SSINGBB): ziv=SSRDB(a)<=SSRDB(w); goto compareresult;
 case SSINGCASE(VA2LE-VA2BF,SSINGBD): ziv=TLE(SSRDB(a),SSRDD(w)); goto compareresult;
 case SSINGCASE(VA2LE-VA2BF,SSINGDB): ziv=TLE(SSRDD(a),SSRDB(w)); goto compareresult;
 case SSINGCASE(VA2LE-VA2BF,SSINGID): ziv=TLE((D)SSRDI(a),SSRDD(w)); goto compareresult;
 case SSINGCASE(VA2LE-VA2BF,SSINGDI): ziv=TLE(SSRDD(a),(D)SSRDI(w)); goto compareresult;
 case SSINGCASE(VA2LE-VA2BF,SSINGBI): ziv=SSRDB(a)<=SSRDI(w); goto compareresult;
 case SSINGCASE(VA2LE-VA2BF,SSINGIB): ziv=SSRDI(a)<=SSRDB(w); goto compareresult;
 case SSINGCASE(VA2LE-VA2BF,SSINGII): ziv=SSRDI(a)<=SSRDI(w); goto compareresult;
 case SSINGCASE(VA2LE-VA2BF,SSINGDD): ziv=TLE(SSRDD(a),SSRDD(w)); goto compareresult;


 case SSINGCASE(VA2GE-VA2BF,SSINGBB): ziv=SSRDB(a)>=SSRDB(w); goto compareresult;
 case SSINGCASE(VA2GE-VA2BF,SSINGBD): ziv=TGE(SSRDB(a),SSRDD(w)); goto compareresult;
 case SSINGCASE(VA2GE-VA2BF,SSINGDB): ziv=TGE(SSRDD(a),SSRDB(w)); goto compareresult;
 case SSINGCASE(VA2GE-VA2BF,SSINGID): ziv=TGE((D)SSRDI(a),SSRDD(w)); goto compareresult;
 case SSINGCASE(VA2GE-VA2BF,SSINGDI): ziv=TGE(SSRDD(a),(D)SSRDI(w)); goto compareresult;
 case SSINGCASE(VA2GE-VA2BF,SSINGBI): ziv=SSRDB(a)>=SSRDI(w); goto compareresult;
 case SSINGCASE(VA2GE-VA2BF,SSINGIB): ziv=SSRDI(a)>=SSRDB(w); goto compareresult;
 case SSINGCASE(VA2GE-VA2BF,SSINGII): ziv=SSRDI(a)>=SSRDI(w); goto compareresult;
 case SSINGCASE(VA2GE-VA2BF,SSINGDD): ziv=TGE(SSRDD(a),SSRDD(w)); goto compareresult;


 case SSINGCASE(VA2NE-VA2BF,SSINGBB): ziv=SSRDB(a)!=SSRDB(w); goto compareresult;
 case SSINGCASE(VA2NE-VA2BF,SSINGBD): ziv=TNE(SSRDB(a),SSRDD(w)); goto compareresult;
 case SSINGCASE(VA2NE-VA2BF,SSINGDB): ziv=TNE(SSRDD(a),SSRDB(w)); goto compareresult; 
 case SSINGCASE(VA2NE-VA2BF,SSINGID): ziv=TNE((D)SSRDI(a),SSRDD(w)); goto compareresult;
 case SSINGCASE(VA2NE-VA2BF,SSINGDI): ziv=TNE(SSRDD(a),(D)SSRDI(w)); goto compareresult;
 case SSINGCASE(VA2NE-VA2BF,SSINGBI): ziv=SSRDB(a)!=SSRDI(w); goto compareresult;
 case SSINGCASE(VA2NE-VA2BF,SSINGIB): ziv=SSRDI(a)!=SSRDB(w); goto compareresult;
 case SSINGCASE(VA2NE-VA2BF,SSINGII): ziv=SSRDI(a)!=SSRDI(w); goto compareresult;
 case SSINGCASE(VA2NE-VA2BF,SSINGDD): ziv=TNE(SSRDD(a),SSRDD(w)); goto compareresult;


 case SSINGCASE(VA2EQ-VA2BF,SSINGBB): ziv=SSRDB(a)==SSRDB(w); goto compareresult;
 case SSINGCASE(VA2EQ-VA2BF,SSINGBD): ziv=TEQ(SSRDB(a),SSRDD(w)); goto compareresult;
 case SSINGCASE(VA2EQ-VA2BF,SSINGDB): ziv=TEQ(SSRDD(a),SSRDB(w)); goto compareresult; 
 case SSINGCASE(VA2EQ-VA2BF,SSINGID): ziv=TEQ((D)SSRDI(a),SSRDD(w)); goto compareresult;
 case SSINGCASE(VA2EQ-VA2BF,SSINGDI): ziv=TEQ(SSRDD(a),(D)SSRDI(w)); goto compareresult;
 case SSINGCASE(VA2EQ-VA2BF,SSINGBI): ziv=SSRDB(a)==SSRDI(w); goto compareresult;
 case SSINGCASE(VA2EQ-VA2BF,SSINGIB): ziv=SSRDI(a)==SSRDB(w); goto compareresult;
 case SSINGCASE(VA2EQ-VA2BF,SSINGII): ziv=SSRDI(a)==SSRDI(w); goto compareresult;
 case SSINGCASE(VA2EQ-VA2BF,SSINGDD): ziv=TEQ(SSRDD(a),SSRDD(w)); goto compareresult;


 case SSINGCASE(VA2CIRCLE-VA2BF,SSINGBB): adv=SSRDB(a); wdv=SSRDB(w); goto circleresult;
 case SSINGCASE(VA2CIRCLE-VA2BF,SSINGBD): adv=SSRDB(a); wdv=SSRDD(w); goto circleresult;
 case SSINGCASE(VA2CIRCLE-VA2BF,SSINGDB): adv=SSRDD(a); wdv=SSRDB(w); goto circleresult;
 case SSINGCASE(VA2CIRCLE-VA2BF,SSINGID): adv=(D)SSRDI(a); wdv=SSRDD(w);  goto circleresult;
 case SSINGCASE(VA2CIRCLE-VA2BF,SSINGDI): adv=SSRDD(a); wdv=(D)SSRDI(w);  goto circleresult;
 case SSINGCASE(VA2CIRCLE-VA2BF,SSINGBI): adv=(D)SSRDB(a); wdv=(D)SSRDI(w); goto circleresult;
 case SSINGCASE(VA2CIRCLE-VA2BF,SSINGIB): adv=(D)SSRDI(a); wdv=(D)SSRDB(w); goto circleresult;
 case SSINGCASE(VA2CIRCLE-VA2BF,SSINGII): adv=(D)SSRDI(a); wdv=(D)SSRDI(w); goto circleresult;
 case SSINGCASE(VA2CIRCLE-VA2BF,SSINGDD): adv=SSRDD(a); wdv=SSRDD(w);  goto circleresult;


 case SSINGCASE(VA2RESIDUE-VA2BF,SSINGBB): ziv=SSRDB(a)<SSRDB(w); goto compareresult;
 case SSINGCASE(VA2RESIDUE-VA2BF,SSINGIB): aiv=SSRDI(a); wiv=(I)SSRDB(w); goto intresidue;
 case SSINGCASE(VA2RESIDUE-VA2BF,SSINGBI): aiv=(I)SSRDB(a); wiv=SSRDI(w); goto intresidue;
 case SSINGCASE(VA2RESIDUE-VA2BF,SSINGII): aiv=SSRDI(a); wiv=SSRDI(w);
  intresidue: ;
   ziv=((aiv&-aiv)+(aiv<=0)==0)?wiv&(aiv-1):remii(aiv,wiv);  // if positive power of 2, just AND; otherwise divide
   SSSTORE(ziv,z,INT,I); R z;
 case SSINGCASE(VA2RESIDUE-VA2BF,SSINGID): aiv=SSRDI(a); wdv=SSRDD(w);
   ziv=jtremid(jt,aiv,wdv); if(!jt->jerr){SSSTORE(ziv,z,INT,I);}else z=0; R z;  // Since this can retry, we must not modify the input block if there is an error

 case SSINGCASE(VA2RESIDUE-VA2BF,SSINGBD): adv=(D)SSRDB(a); wdv=SSRDD(w); goto floatresidue;
 case SSINGCASE(VA2RESIDUE-VA2BF,SSINGDB): adv=SSRDD(a); wdv=(D)SSRDB(w); goto floatresidue;
 case SSINGCASE(VA2RESIDUE-VA2BF,SSINGDI): adv=SSRDD(a); wdv=(D)SSRDI(w);  goto floatresidue;
 case SSINGCASE(VA2RESIDUE-VA2BF,SSINGDD): adv=SSRDD(a); wdv=SSRDD(w);
  floatresidue: ;
   zdv=jtremdd(jt,adv,wdv); if(!jt->jerr){SSSTORE(zdv,z,FL,D);}else z=0; R z;  // Since this can retry, we must not modify the input block if there is an error

 }
 // The only thing left is exit processing for the different functions
 gcdintresult:
 ziv=igcd(aiv,wiv); if(ziv||!jt->jerr){SSSTORE(ziv,z,INT,I) R z;}  // if no error, store an int
 if(jt->jerr<EWOV)R 0;  // If not overflow, what can it be?
 RESETERR; adv=(D)aiv; wdv=(D)wiv;  // Rack em up again; Convert int args to float, and fall through to float case
 gcdflresult:
 if((zdv=dgcd(adv,wdv))||!jt->jerr){SSSTORE(zdv,z,FL,D) R z;}  // float result is the last fallback
 R 0;  // if there was an error, fail, jerr is set

 lcmintresult:
 ziv=ilcm(aiv,wiv); if(!jt->jerr){SSSTORE(ziv,z,INT,I) R z;}  // if no error, store an int
 if(jt->jerr<EWOV)R 0;  // If not overflow, what can it be?
 RESETERR; adv=(D)aiv; wdv=(D)wiv;  // Rack em up again; Convert int args to float, and fall through to float case
 lcmflresult:
 zdv=dlcm(adv,wdv); if(!jt->jerr){SSSTORE(zdv,z,FL,D) R z;}  // float result is the last fallback
 R 0;  // if there was an error, fail, jerr is set

 nandresult:
 SSSTORE((B)(1^(aiv&wiv)),z,B01,B) R z;

 norresult:
 SSSTORE((B)(1^(aiv|wiv)),z,B01,B) R z;

 outofresult:
 NAN0; zdv=bindd(adv,wdv); NAN1;
 SSSTORE(zdv,z,FL,D) R z;  // Return the value if valid

 outofresultcvti:
 NAN0; zdv=bindd(adv,wdv); NAN1;
 if(zdv>=(D)IMIN&&zdv<=(D)IMAX){SSSTORE((I)zdv,z,INT,I)}else{SSSTORE(zdv,z,FL,D)} R z;  // Return the value if valid, as integer if possible

 bitwiseresult:
 RE(0);  // if error on D arg, make sure we abort
 ziv=FAV(self)->lc-VA2B0;  // mask describing operation
 ziv=((aiv&wiv)&REPSGN(ziv<<(BW-1-0)))|((aiv&~wiv)&REPSGN(ziv<<(BW-1-1)))|((~aiv&wiv)&REPSGN(ziv<<(BW-1-2)))|((~aiv&~wiv)&REPSGN(ziv<<(BW-1-3)));
 SSSTORE(ziv,z,INT,I) R z;

 compareresult:
 ziv&=1;  // Since we are writing into num[], invest 1 instruction to make sure we don't have an invalid boolean
 // If we did not inplace a result block, return num[ziv].  To avoid a misbranch, we store the value and type into num[], which is OK since they never change what's there already
 aiv=(I)(num[ziv]); z=z?z:(A)aiv;
 SSSTORE((B)ziv,z,B01,B) R z;

 circleresult: ;
 D cirvals[3]={adv,wdv};  // put ops into memory
 cirDD(1,1,cirvals,cirvals+1,cirvals+2,jt);  // run the routine
 if(!jt->jerr){SSSTORE(cirvals[2],z,FL,D);}else z=0; R z;  // Don't change the input block if there is an error

}


