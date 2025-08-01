/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Atomic (Scalar) Dyadic                                           */

#include "j.h"
#include "ve.h"
#include "vcomp.h"

// Speedy Singletons, moved here to be inlined

#include <fenv.h>

#define SSINGENC(a,w) ((3*(a)+((w)&INT+FL))>>INTX)
#define SSINGBB SSINGENC(B01,B01)
#define SSINGBI SSINGENC(B01,INT)
#define SSINGBD SSINGENC(B01,FL)
#define SSINGIB SSINGENC(INT,B01)
#define SSINGII SSINGENC(INT,INT)
#define SSINGID SSINGENC(INT,FL)
#define SSINGDB SSINGENC(FL,B01)
#define SSINGDI SSINGENC(FL,INT)
#define SSINGDD SSINGENC(FL,FL)

#define SSSTORENV(v,z,t,type) {AT(z)=(t); *((type *)zv) = (v); }  // When we know that if the block is reused, we are not changing the type; but we change the type of a new block
#define SSSTORE(v,z,t,type) SSSTORENV(v,z,t,type)  // we don't use MODBLOCKTYPE any more
#define SSSTORENVFL(v,z,t,type) {*((type *)zv) = (v); }  // When we know the type/shape doesn't change (FL,FL->FL)

#define EXECNAN(exp) {zdv=(exp); if(unlikely(zdv!=zdv))ASSERT(adv!=adv||wdv!=wdv,EVNAN)}  // execute exp, error if NaN result
#define EXECNANH(exp) {NAN0; zdv=(exp); NAN1;}  // same but use composite detection in hardware


// Return int version of d, with error if loss of significance
static NOINLINE I intforD(J jt, D d){D q;I z;  // noinline because it uses so many ymm regs that the caller has to save them too, and it is little needed
 q=jround(d); z=(I)q;
 ASSERT(ISFTOIOK(d,q),EVDOMAIN);
 R z;
}

#define SSINGCASE(id,subtype) (9*(id)+(subtype))   // encode case/args into one branch value

// we know that AN=1 in a and w, which are FL/INT/B01 types.  af is larger arg rank (=rank of result)
INLINE static A jtssingleton(J jt,A a,A w,I af,I at,I wt,A self){
 I awip=2*SGNTO0(AC(a))+SGNTO0(AC(w));  // collect inplaceable status for a and w
 I opcode=(I)FAV(self)->lu2.lc;  // fetch operation#
 I jtfg=(I)jt; jt=(J)(intptr_t)((I)jt&~JTFLAGMSK);   // save jt as an I, clear low bits
 A z=jt->zombieval;  // fetch address of assignand, which we presumptively make the result
 void *av=voidAV(a), *wv=voidAV(w), *zv;  // point to the argument values and result
 I caseno=(opcode&0x7f)-VA2CBW1111; caseno=caseno<0?0:caseno; caseno=SSINGCASE(caseno,SSINGENC(at,wt));  // case # for eventual switch.  Lump all Booleans at 0
 // if the operation is a rank-0 comparison that can return num[result], don't bother with inplacing.  Inplacing would be
 // a potential gain if the result can itself be inplaced, but it is a certain loser when deciding where the result is
 if((opcode>>7)>af){z=0; goto nozv;}  // true if 0x80 (comparison op) and af=0.  Set z=0 as a flag to return num(result)
 // See if we can inplace.  We let some chances get away because they aren't worth testing for.  There are two main possibilities: assignment (checked above)
 // and abandoned arg (checked presently).  If either passes, it must further be not VIRTUAL if assigned (lest it overwrite the backer of a virtual x/y arg)
 // and not AFRO if bare; and never UNINCORPABLE since we may change the type and we don't want callers to bear the burden of checking that.  Assign in place is best,
 // because it makes the assignment skip the free
 // See if we can inplace an assignment.  That is always a good idea, though rare
 if(unlikely((2*(a==z)+(w==z))&jtfg)){   // one of the args is being reassigned
  if(likely((AFLAG(z)&AFVIRTUAL+AFUNINCORPABLE)+(af^AR(z))==0)){goto getzv;}   // mustn't modify type of VIRTUAL or INCORPABLE, and reassigned value must have the higher rank
 }
 if(awip&=jtfg){z=awip&JTINPLACEW?w:a;   // block is abandoned inplaceable, : pick it.  Priority to w
  if(likely((AFLAG(z)&AFUNINCORPABLE+AFRO)+(af^AR(z))==0))goto getzv;  // not disallowed and correct rank: use it
  if(awip==3){z=a; if(likely((AFLAG(a)&AFUNINCORPABLE+AFRO)+(af^AR(a))==0))goto getzv;}  // if a & w both eligible, check a if w failed
 }
 // fall through: no inplacing, allocate the result, usually an atom.  If not atom, make the shape all 1s
 if(likely(af==0)){GAT0(z,FL,1,0); zv=voidAV0(z);}else{GATV1(z,FL,1,af); zv=voidAVn(af,z);}  // af persists over call, then freed
 goto nozv;
getzv:;  // here when we are operating inplace on z
 zv=voidAV(z);  // get addr of value
nozv:;  // here when we have zv or don't need it
 // z is 0 ONLY for comparisons with no rank.
 I ziv; D zdv;
 // Start loading everything we will need as values before the pipeline break.  Tempting to convert int-to-float as well, but perhaps it will predict right?
 I aiv=*(I*)av; I wiv=*(I*)wv; D adv,wdv;  // arg values
#if defined(__aarch32__)||defined(__arm__)||defined(_M_ARM)
 memcpy(&adv,av,4);
 memcpy(4+(char*)&adv,4+(char*)av,4);   // avoid bus error
 memcpy(&wdv,wv,4);
 memcpy(4+(char*)&wdv,4+(char*)wv,4);   // avoid bus error
#else
 adv=*(D*)av,wdv=*(D*)wv;
#endif
 
 // Huge switch statement to handle every case.  Lump all the booleans together at 0
 switch(caseno){
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGBB): SSSTORENV((B)aiv+(B)wiv,z,INT,I) R z;  // NV because B01 is never virtual inplace
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGBD): SSSTORENV((B)aiv+wdv,z,FL,D) R z;
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGDB): SSSTORENV(adv+(B)wiv,z,FL,D) R z;
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGID): SSSTORE(aiv+wdv,z,FL,D) R z;
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGDI): SSSTORE(adv+wiv,z,FL,D) R z;
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGBI): 
  {if(unlikely(__builtin_add_overflow((B)aiv,wiv,&ziv)))SSSTORE((D)(B)aiv+(D)wiv,z,FL,D) else SSSTORENV(ziv,z,INT,I) R z;}
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGIB):
  {if(unlikely(__builtin_add_overflow(aiv,(B)wiv,&ziv)))SSSTORE((D)aiv+(D)(B)wiv,z,FL,D) else SSSTORENV(ziv,z,INT,I) R z;}
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGII):
  {if(unlikely(__builtin_add_overflow(aiv,wiv,&ziv)))SSSTORE((D)aiv+(D)wiv,z,FL,D) else SSSTORENV(ziv,z,INT,I) R z;}
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGDD):
  {EXECNAN(adv+wdv); SSSTORENVFL(zdv,z,FL,D) R z;}

 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGBB): SSSTORENV((B)aiv-(B)wiv,z,INT,I) R z;
 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGBD): SSSTORENV((B)aiv-wdv,z,FL,D) R z;
 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGDB): SSSTORENV(adv-(B)wiv,z,FL,D) R z;
 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGID): SSSTORE(aiv-wdv,z,FL,D) R z;
 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGDI): SSSTORE(adv-wiv,z,FL,D) R z;
 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGBI): 
  {if(unlikely(__builtin_sub_overflow((B)aiv,wiv,&ziv)))SSSTORE((D)(B)aiv-(D)wiv,z,FL,D) else SSSTORENV(ziv,z,INT,I) R z;}
 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGIB):
  {if(unlikely(__builtin_sub_overflow(aiv,(B)wiv,&ziv)))SSSTORE((D)aiv-(D)(B)wiv,z,FL,D) else SSSTORENV(ziv,z,INT,I) R z;}
 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGII):
  {if(unlikely(__builtin_sub_overflow(aiv,wiv,&ziv)))SSSTORE((D)aiv-(D)wiv,z,FL,D) else SSSTORENV(ziv,z,INT,I) R z;}
 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGDD):
  {EXECNAN(adv-wdv); SSSTORENVFL(zdv,z,FL,D) R z;}

 case SSINGCASE(VA2CMIN-VA2CBW1111,SSINGBB): SSSTORENV((B)aiv&(B)wiv,z,B01,B) R z;
 case SSINGCASE(VA2CMIN-VA2CBW1111,SSINGBD): SSSTORENV(MIN((B)aiv,wdv),z,FL,D) R z;
 case SSINGCASE(VA2CMIN-VA2CBW1111,SSINGDB): SSSTORENV(MIN(adv,(B)wiv),z,FL,D) R z;
 case SSINGCASE(VA2CMIN-VA2CBW1111,SSINGID): SSSTORE(MIN(aiv,wdv),z,FL,D) R z;
 case SSINGCASE(VA2CMIN-VA2CBW1111,SSINGDI): SSSTORE(MIN(adv,wiv),z,FL,D) R z;
 case SSINGCASE(VA2CMIN-VA2CBW1111,SSINGBI): SSSTORENV(MIN((B)aiv,wiv),z,INT,I) R z;
 case SSINGCASE(VA2CMIN-VA2CBW1111,SSINGIB): SSSTORENV(MIN(aiv,(B)wiv),z,INT,I) R z;
 case SSINGCASE(VA2CMIN-VA2CBW1111,SSINGII): SSSTORENV(MIN(aiv,wiv),z,INT,I) R z;
 case SSINGCASE(VA2CMIN-VA2CBW1111,SSINGDD): SSSTORENVFL(MIN(adv,wdv),z,FL,D) R z;


 case SSINGCASE(VA2CMAX-VA2CBW1111,SSINGBB): SSSTORENV((B)aiv|(B)wiv,z,B01,B) R z;
 case SSINGCASE(VA2CMAX-VA2CBW1111,SSINGBD): SSSTORENV(MAX((B)aiv,wdv),z,FL,D) R z;
 case SSINGCASE(VA2CMAX-VA2CBW1111,SSINGDB): SSSTORENV(MAX(adv,(B)wiv),z,FL,D) R z;
 case SSINGCASE(VA2CMAX-VA2CBW1111,SSINGID): SSSTORE(MAX(aiv,wdv),z,FL,D) R z;
 case SSINGCASE(VA2CMAX-VA2CBW1111,SSINGDI): SSSTORE(MAX(adv,wiv),z,FL,D) R z;
 case SSINGCASE(VA2CMAX-VA2CBW1111,SSINGBI): SSSTORENV(MAX((B)aiv,wiv),z,INT,I) R z;
 case SSINGCASE(VA2CMAX-VA2CBW1111,SSINGIB): SSSTORENV(MAX(aiv,(B)wiv),z,INT,I) R z;
 case SSINGCASE(VA2CMAX-VA2CBW1111,SSINGII): SSSTORENV(MAX(aiv,wiv),z,INT,I) R z;
 case SSINGCASE(VA2CMAX-VA2CBW1111,SSINGDD): SSSTORENVFL(MAX(adv,wdv),z,FL,D) R z;


 case SSINGCASE(VA2CSTAR-VA2CBW1111,SSINGBB): SSSTORENV((B)aiv&(B)wiv,z,B01,B) R z;
 case SSINGCASE(VA2CSTAR-VA2CBW1111,SSINGBD): SSSTORENV((B)aiv?wdv:0.0,z,FL,D) R z;
 case SSINGCASE(VA2CSTAR-VA2CBW1111,SSINGDB): SSSTORENV((B)wiv?adv:0.0,z,FL,D) R z;
 case SSINGCASE(VA2CSTAR-VA2CBW1111,SSINGID): SSSTORE(aiv?aiv*wdv:0.0,z,FL,D) R z;
 case SSINGCASE(VA2CSTAR-VA2CBW1111,SSINGDI): SSSTORE(wiv?wiv*adv:0.0,z,FL,D) R z;
 case SSINGCASE(VA2CSTAR-VA2CBW1111,SSINGBI): SSSTORENV((B)aiv?wiv:0,z,INT,I) R z;
 case SSINGCASE(VA2CSTAR-VA2CBW1111,SSINGIB): SSSTORENV((B)wiv?aiv:0,z,INT,I) R z;
 case SSINGCASE(VA2CSTAR-VA2CBW1111,SSINGII): {
   DPMULDDECLS DPMULD(aiv,wiv,ziv,SSSTORE((D)aiv*(D)wiv,z,FL,D))  // overflow
   else SSSTORENV(ziv,z,INT,I)  // normal
   R z;}
 case SSINGCASE(VA2CSTAR-VA2CBW1111,SSINGDD): {
   SSSTORENVFL(TYMES(adv,wdv),z,FL,D) R z;}


 case SSINGCASE(VA2CDIV-VA2CBW1111,SSINGBB): {SSSTORENV((B)wiv?(B)aiv:(B)aiv?inf:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2CDIV-VA2CBW1111,SSINGBD): {SSSTORENV(((B)aiv||wdv)?(B)aiv/wdv:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2CDIV-VA2CBW1111,SSINGDB): {SSSTORENV((adv||(B)wiv)?adv/(B)wiv:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2CDIV-VA2CBW1111,SSINGID): {SSSTORE((aiv||wdv)?aiv/wdv:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2CDIV-VA2CBW1111,SSINGDI): {SSSTORE((adv||wiv)?adv/wiv:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2CDIV-VA2CBW1111,SSINGBI): {SSSTORE(((B)aiv||wiv)?(B)aiv/(D)wiv:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2CDIV-VA2CBW1111,SSINGIB): {SSSTORE((aiv||(B)wiv)?aiv/(D)(B)wiv:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2CDIV-VA2CBW1111,SSINGII): {SSSTORE((aiv||wiv)?aiv/(D)wiv:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2CDIV-VA2CBW1111,SSINGDD): {EXECNAN(DIV(adv,wdv)); SSSTORENVFL(zdv,z,FL,D) R z;}


 case SSINGCASE(VA2CPLUSDOT-VA2CBW1111,SSINGBB): {SSSTORENV((B)aiv|(B)wiv,z,B01,B) R z;}
 case SSINGCASE(VA2CPLUSDOT-VA2CBW1111,SSINGBD): {adv=(D)(B)aiv; goto gcdflresult;}
 case SSINGCASE(VA2CPLUSDOT-VA2CBW1111,SSINGDB): {wdv=(D)(B)wiv; goto gcdflresult;}
 case SSINGCASE(VA2CPLUSDOT-VA2CBW1111,SSINGID): {adv=(D)aiv; goto gcdflresult;}
 case SSINGCASE(VA2CPLUSDOT-VA2CBW1111,SSINGDI): {wdv=(D)wiv; goto gcdflresult;}
 case SSINGCASE(VA2CPLUSDOT-VA2CBW1111,SSINGBI): aiv=(B)aiv; goto gcdintresult;
 case SSINGCASE(VA2CPLUSDOT-VA2CBW1111,SSINGIB): wiv=(B)wiv; goto gcdintresult;
 case SSINGCASE(VA2CPLUSDOT-VA2CBW1111,SSINGII): goto gcdintresult;
 case SSINGCASE(VA2CPLUSDOT-VA2CBW1111,SSINGDD): {goto gcdflresult;}


 case SSINGCASE(VA2CSTARDOT-VA2CBW1111,SSINGBB): {SSSTORENV((B)aiv&(B)wiv,z,B01,B) R z;}
 case SSINGCASE(VA2CSTARDOT-VA2CBW1111,SSINGBD): {adv=(B)aiv; goto lcmflresult;}
 case SSINGCASE(VA2CSTARDOT-VA2CBW1111,SSINGDB): {wdv=(B)wiv; goto lcmflresult;}
 case SSINGCASE(VA2CSTARDOT-VA2CBW1111,SSINGID): {adv=aiv; goto lcmflresult;}
 case SSINGCASE(VA2CSTARDOT-VA2CBW1111,SSINGDI): {wdv=wiv; goto lcmflresult;}
 case SSINGCASE(VA2CSTARDOT-VA2CBW1111,SSINGBI): aiv=(B)aiv; goto lcmintresult;
 case SSINGCASE(VA2CSTARDOT-VA2CBW1111,SSINGIB): wiv=(B)wiv; goto lcmintresult;
 case SSINGCASE(VA2CSTARDOT-VA2CBW1111,SSINGII): goto lcmintresult;
 case SSINGCASE(VA2CSTARDOT-VA2CBW1111,SSINGDD): {goto lcmflresult;}


 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGBB): aiv=(B)aiv; wiv=(B)wiv; goto nandresult;
 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGBD): aiv=(B)aiv; ASSERT(wdv==0.0 || TEQ(wdv,1.0),EVDOMAIN); wiv=wdv!=0.0; goto nandresult;
 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGDB): wiv=(B)wiv; ASSERT(adv==0.0 || TEQ(adv,1.0),EVDOMAIN); aiv=adv!=0.0; goto nandresult;
 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGID): ASSERT(!(aiv&-2) && (wdv==0.0 || TEQ(wdv,1.0)),EVDOMAIN); wiv=wdv!=0.0; goto nandresult;
 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGDI): ASSERT(!(wiv&-2) && (adv==0.0 || TEQ(adv,1.0)),EVDOMAIN); aiv=adv!=0.0; goto nandresult;
 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGBI): aiv=(B)aiv; ASSERT(!(wiv&-2),EVDOMAIN); goto nandresult;
 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGIB): wiv=(B)wiv; ASSERT(!(aiv&-2),EVDOMAIN); goto nandresult;
 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGII): ASSERT(!((aiv|wiv)&-2),EVDOMAIN); goto nandresult;
 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGDD): ASSERT((adv==0.0 || TEQ(adv,1.0)) && (wdv==0.0 || TEQ(wdv,1.0)),EVDOMAIN); wiv=wdv!=0.0; aiv=adv!=0.0; goto nandresult;


 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGBB): aiv=(B)aiv; wiv=(B)wiv; goto norresult;
 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGBD): aiv=(B)aiv; ASSERT(wdv==0.0 || TEQ(wdv,1.0),EVDOMAIN); wiv=wdv!=0.0; goto norresult;
 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGDB): wiv=(B)wiv; ASSERT(adv==0.0 || TEQ(adv,1.0),EVDOMAIN); aiv=adv!=0.0; goto norresult;
 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGID): ASSERT(!(aiv&-2) && (wdv==0.0 || TEQ(wdv,1.0)),EVDOMAIN); wiv=wdv!=0.0; goto norresult;
 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGDI): ASSERT(!(wiv&-2) && (adv==0.0 || TEQ(adv,1.0)),EVDOMAIN); aiv=adv!=0.0; goto norresult;
 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGBI): aiv=(B)aiv; ASSERT(!(wiv&-2),EVDOMAIN); goto norresult;
 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGIB): wiv=(B)wiv; ASSERT(!(aiv&-2),EVDOMAIN); goto norresult;
 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGII): ASSERT(!((aiv|wiv)&-2),EVDOMAIN); goto norresult;
 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGDD): ASSERT((adv==0.0 || TEQ(adv,1.0)) && (wdv==0.0 || TEQ(wdv,1.0)),EVDOMAIN); wiv=wdv!=0.0; aiv=adv!=0.0; goto norresult;


 case SSINGCASE(VA2CBANG-VA2CBW1111,SSINGBB): SSSTORENV((B)aiv<=(B)wiv,z,B01,B) R z;
 case SSINGCASE(VA2CBANG-VA2CBW1111,SSINGBD): adv=(B)aiv; goto outofresult;
 case SSINGCASE(VA2CBANG-VA2CBW1111,SSINGDB): wdv=(B)wiv;  goto outofresult;
 case SSINGCASE(VA2CBANG-VA2CBW1111,SSINGID): adv=(D)aiv; goto outofresult;
 case SSINGCASE(VA2CBANG-VA2CBW1111,SSINGDI): wdv=(D)wiv;  goto outofresult;
 case SSINGCASE(VA2CBANG-VA2CBW1111,SSINGBI): adv=(D)(B)aiv; wdv=(D)wiv; goto outofresultcvti;
 case SSINGCASE(VA2CBANG-VA2CBW1111,SSINGIB): adv=(D)aiv; wdv=(D)(B)wiv; goto outofresultcvti;
 case SSINGCASE(VA2CBANG-VA2CBW1111,SSINGII): adv=(D)aiv; wdv=(D)wiv; goto outofresultcvti;
 case SSINGCASE(VA2CBANG-VA2CBW1111,SSINGDD): goto outofresult;


 case SSINGCASE(VA2CEXP-VA2CBW1111,SSINGBB): SSSTORENV((I)(B)aiv|(I)!(B)wiv,z,B01,B) R z;
 case SSINGCASE(VA2CEXP-VA2CBW1111,SSINGBD): SSSTORE((B)aiv?1.0:(zdv=wdv)<0?inf:zdv==0?1:0,z,FL,D) R z;
 case SSINGCASE(VA2CEXP-VA2CBW1111,SSINGDB): SSSTORE((B)wiv?adv:1.0,z,FL,D) R z;
 case SSINGCASE(VA2CEXP-VA2CBW1111,SSINGID): RE(zdv=pospow((D)aiv,wdv)) SSSTORE(zdv,z,FL,D) R z;
 case SSINGCASE(VA2CEXP-VA2CBW1111,SSINGDI): RE(zdv=intpow(adv,wiv)) SSSTORE(zdv,z,FL,D) R z;
 case SSINGCASE(VA2CEXP-VA2CBW1111,SSINGBI): SSSTORE((B)aiv?1.0:(zdv=(D)wiv)<0?inf:zdv==0?1:0,z,FL,D) R z;
 case SSINGCASE(VA2CEXP-VA2CBW1111,SSINGIB): SSSTORE((B)wiv?aiv:1,z,INT,I) R z;
 case SSINGCASE(VA2CEXP-VA2CBW1111,SSINGII): RE(zdv=intpow((D)aiv,wiv)) SSSTORE(zdv,z,FL,D) R z;
 case SSINGCASE(VA2CEXP-VA2CBW1111,SSINGDD): RE(zdv=pospow(adv,wdv)) SSSTORENVFL(zdv,z,FL,D) R z;


 case SSINGCASE(VA2CBW1111-VA2CBW1111,SSINGBB): aiv=(B)aiv; wiv=(B)wiv; goto bitwiseresult;
 case SSINGCASE(VA2CBW1111-VA2CBW1111,SSINGBD): aiv=(B)aiv; wiv=intforD(jt,wdv); goto bitwiseresult;
 case SSINGCASE(VA2CBW1111-VA2CBW1111,SSINGDB): aiv=intforD(jt,adv); wiv=(B)wiv; goto bitwiseresult;
 case SSINGCASE(VA2CBW1111-VA2CBW1111,SSINGID): wiv=intforD(jt,wdv); goto bitwiseresult;
 case SSINGCASE(VA2CBW1111-VA2CBW1111,SSINGDI): aiv=intforD(jt,adv); goto bitwiseresult;
 case SSINGCASE(VA2CBW1111-VA2CBW1111,SSINGBI): aiv=(B)aiv; goto bitwiseresult;
 case SSINGCASE(VA2CBW1111-VA2CBW1111,SSINGIB): wiv=(B)wiv; goto bitwiseresult;
 case SSINGCASE(VA2CBW1111-VA2CBW1111,SSINGII): goto bitwiseresult;
 case SSINGCASE(VA2CBW1111-VA2CBW1111,SSINGDD): ((D*)jt->shapesink)[0]=wdv; aiv=intforD(jt,adv); wiv=intforD(jt,((D*)jt->shapesink)[0]); goto bitwiseresult;  // hide wdv so it doesn't get allocated to reg requiring save/restore

 case SSINGCASE(VA2CBW10000-VA2CBW1111,SSINGBB): aiv=(B)aiv; wiv=(B)wiv; goto rotateresult;
 case SSINGCASE(VA2CBW10000-VA2CBW1111,SSINGBD): aiv=(B)aiv; wiv=intforD(jt,wdv); goto rotateresult;
 case SSINGCASE(VA2CBW10000-VA2CBW1111,SSINGDB): aiv=intforD(jt,adv); wiv=(B)wiv; goto rotateresult;
 case SSINGCASE(VA2CBW10000-VA2CBW1111,SSINGID): wiv=intforD(jt,wdv); goto rotateresult;
 case SSINGCASE(VA2CBW10000-VA2CBW1111,SSINGDI): aiv=intforD(jt,adv); goto rotateresult;
 case SSINGCASE(VA2CBW10000-VA2CBW1111,SSINGBI): aiv=(B)aiv; goto rotateresult;
 case SSINGCASE(VA2CBW10000-VA2CBW1111,SSINGIB): wiv=(B)wiv; goto rotateresult;
 case SSINGCASE(VA2CBW10000-VA2CBW1111,SSINGII): goto rotateresult;
 case SSINGCASE(VA2CBW10000-VA2CBW1111,SSINGDD): ((D*)jt->shapesink)[0]=wdv; aiv=intforD(jt,adv); wiv=intforD(jt,((D*)jt->shapesink)[0]); goto rotateresult;  // hide wdv so it doesn't get allocated to reg requiring save/restore

 case SSINGCASE(VA2CBW10001-VA2CBW1111,SSINGBB): aiv=(B)aiv; wiv=(B)wiv; goto lslresult;
 case SSINGCASE(VA2CBW10001-VA2CBW1111,SSINGBD): aiv=(B)aiv; wiv=intforD(jt,wdv); goto lslresult;
 case SSINGCASE(VA2CBW10001-VA2CBW1111,SSINGDB): aiv=intforD(jt,adv); wiv=(B)wiv; goto lslresult;
 case SSINGCASE(VA2CBW10001-VA2CBW1111,SSINGID): wiv=intforD(jt,wdv); goto lslresult;
 case SSINGCASE(VA2CBW10001-VA2CBW1111,SSINGDI): aiv=intforD(jt,adv); goto lslresult;
 case SSINGCASE(VA2CBW10001-VA2CBW1111,SSINGBI): aiv=(B)aiv; goto lslresult;
 case SSINGCASE(VA2CBW10001-VA2CBW1111,SSINGIB): wiv=(B)wiv; goto lslresult;
 case SSINGCASE(VA2CBW10001-VA2CBW1111,SSINGII): goto lslresult;
 case SSINGCASE(VA2CBW10001-VA2CBW1111,SSINGDD): ((D*)jt->shapesink)[0]=wdv; aiv=intforD(jt,adv); wiv=intforD(jt,((D*)jt->shapesink)[0]); goto lslresult;  // hide wdv so it doesn't get allocated to reg requiring save/restore

 case SSINGCASE(VA2CBW10010-VA2CBW1111,SSINGBB): aiv=(B)aiv; wiv=(B)wiv; goto aslresult;
 case SSINGCASE(VA2CBW10010-VA2CBW1111,SSINGBD): aiv=(B)aiv; wiv=intforD(jt,wdv); goto aslresult;
 case SSINGCASE(VA2CBW10010-VA2CBW1111,SSINGDB): aiv=intforD(jt,adv); wiv=(B)wiv; goto aslresult;
 case SSINGCASE(VA2CBW10010-VA2CBW1111,SSINGID): wiv=intforD(jt,wdv); goto aslresult;
 case SSINGCASE(VA2CBW10010-VA2CBW1111,SSINGDI): aiv=intforD(jt,adv); goto aslresult;
 case SSINGCASE(VA2CBW10010-VA2CBW1111,SSINGBI): aiv=(B)aiv; goto aslresult;
 case SSINGCASE(VA2CBW10010-VA2CBW1111,SSINGIB): wiv=(B)wiv; goto aslresult;
 case SSINGCASE(VA2CBW10010-VA2CBW1111,SSINGII): goto aslresult;
 case SSINGCASE(VA2CBW10010-VA2CBW1111,SSINGDD): ((D*)jt->shapesink)[0]=wdv; aiv=intforD(jt,adv); wiv=intforD(jt,((D*)jt->shapesink)[0]); goto aslresult;  // hide wdv so it doesn't get allocated to reg requiring save/restore


 case SSINGCASE(VA2CLT-VA2CBW1111,SSINGBB): ziv=(B)aiv<(B)wiv; goto compareresult;
 case SSINGCASE(VA2CLT-VA2CBW1111,SSINGBD): ziv=TLT((B)aiv,wdv); goto compareresult;
 case SSINGCASE(VA2CLT-VA2CBW1111,SSINGDB): ziv=TLT(adv,(B)wiv); goto compareresult;
 case SSINGCASE(VA2CLT-VA2CBW1111,SSINGID): ziv=TLT((D)aiv,wdv); goto compareresult;
 case SSINGCASE(VA2CLT-VA2CBW1111,SSINGDI): ziv=TLT(adv,(D)wiv); goto compareresult;
 case SSINGCASE(VA2CLT-VA2CBW1111,SSINGBI): ziv=(B)aiv<wiv; goto compareresult;
 case SSINGCASE(VA2CLT-VA2CBW1111,SSINGIB): ziv=aiv<(B)wiv; goto compareresult;
 case SSINGCASE(VA2CLT-VA2CBW1111,SSINGII): ziv=aiv<wiv; goto compareresult;
 case SSINGCASE(VA2CLT-VA2CBW1111,SSINGDD): ziv=TLT(adv,wdv); goto compareresult;

 case SSINGCASE(VA2CGT-VA2CBW1111,SSINGBB): ziv=(B)aiv>(B)wiv; goto compareresult;
 case SSINGCASE(VA2CGT-VA2CBW1111,SSINGBD): ziv=TGT((B)aiv,wdv); goto compareresult;
 case SSINGCASE(VA2CGT-VA2CBW1111,SSINGDB): ziv=TGT(adv,(B)wiv); goto compareresult;
 case SSINGCASE(VA2CGT-VA2CBW1111,SSINGID): ziv=TGT((D)aiv,wdv); goto compareresult;
 case SSINGCASE(VA2CGT-VA2CBW1111,SSINGDI): ziv=TGT(adv,(D)wiv); goto compareresult;
 case SSINGCASE(VA2CGT-VA2CBW1111,SSINGBI): ziv=(B)aiv>wiv; goto compareresult;
 case SSINGCASE(VA2CGT-VA2CBW1111,SSINGIB): ziv=aiv>(B)wiv; goto compareresult;
 case SSINGCASE(VA2CGT-VA2CBW1111,SSINGII): ziv=aiv>wiv; goto compareresult;
 case SSINGCASE(VA2CGT-VA2CBW1111,SSINGDD): ziv=TGT(adv,wdv); goto compareresult;

 case SSINGCASE(VA2CLE-VA2CBW1111,SSINGBB): ziv=(B)aiv<=(B)wiv; goto compareresult;
 case SSINGCASE(VA2CLE-VA2CBW1111,SSINGBD): ziv=TLE((B)aiv,wdv); goto compareresult;
 case SSINGCASE(VA2CLE-VA2CBW1111,SSINGDB): ziv=TLE(adv,(B)wiv); goto compareresult;
 case SSINGCASE(VA2CLE-VA2CBW1111,SSINGID): ziv=TLE((D)aiv,wdv); goto compareresult;
 case SSINGCASE(VA2CLE-VA2CBW1111,SSINGDI): ziv=TLE(adv,(D)wiv); goto compareresult;
 case SSINGCASE(VA2CLE-VA2CBW1111,SSINGBI): ziv=(B)aiv<=wiv; goto compareresult;
 case SSINGCASE(VA2CLE-VA2CBW1111,SSINGIB): ziv=aiv<=(B)wiv; goto compareresult;
 case SSINGCASE(VA2CLE-VA2CBW1111,SSINGII): ziv=aiv<=wiv; goto compareresult;
 case SSINGCASE(VA2CLE-VA2CBW1111,SSINGDD): ziv=TLE(adv,wdv); goto compareresult;

 case SSINGCASE(VA2CGE-VA2CBW1111,SSINGBB): ziv=(B)aiv>=(B)wiv; goto compareresult;
 case SSINGCASE(VA2CGE-VA2CBW1111,SSINGBD): ziv=TGE((B)aiv,wdv); goto compareresult;
 case SSINGCASE(VA2CGE-VA2CBW1111,SSINGDB): ziv=TGE(adv,(B)wiv); goto compareresult;
 case SSINGCASE(VA2CGE-VA2CBW1111,SSINGID): ziv=TGE((D)aiv,wdv); goto compareresult;
 case SSINGCASE(VA2CGE-VA2CBW1111,SSINGDI): ziv=TGE(adv,(D)wiv); goto compareresult;
 case SSINGCASE(VA2CGE-VA2CBW1111,SSINGBI): ziv=(B)aiv>=wiv; goto compareresult;
 case SSINGCASE(VA2CGE-VA2CBW1111,SSINGIB): ziv=aiv>=(B)wiv; goto compareresult;
 case SSINGCASE(VA2CGE-VA2CBW1111,SSINGII): ziv=aiv>=wiv; goto compareresult;
 case SSINGCASE(VA2CGE-VA2CBW1111,SSINGDD): ziv=TGE(adv,wdv); goto compareresult;

 case SSINGCASE(VA2CNE-VA2CBW1111,SSINGBB): ziv=(B)aiv!=(B)wiv; goto compareresult;
 case SSINGCASE(VA2CNE-VA2CBW1111,SSINGBD): ziv=TNE((B)aiv,wdv); goto compareresult;
 case SSINGCASE(VA2CNE-VA2CBW1111,SSINGDB): ziv=TNE(adv,(B)wiv); goto compareresult; 
 case SSINGCASE(VA2CNE-VA2CBW1111,SSINGID): ziv=TNE((D)aiv,wdv); goto compareresult;
 case SSINGCASE(VA2CNE-VA2CBW1111,SSINGDI): ziv=TNE(adv,(D)wiv); goto compareresult;
 case SSINGCASE(VA2CNE-VA2CBW1111,SSINGBI): ziv=(B)aiv!=wiv; goto compareresult;
 case SSINGCASE(VA2CNE-VA2CBW1111,SSINGIB): ziv=aiv!=(B)wiv; goto compareresult;
 case SSINGCASE(VA2CNE-VA2CBW1111,SSINGII): ziv=aiv!=wiv; goto compareresult;
 case SSINGCASE(VA2CNE-VA2CBW1111,SSINGDD): ziv=TNE(adv,wdv); goto compareresult;

 case SSINGCASE(VA2CEQ-VA2CBW1111,SSINGBB): ziv=(B)aiv==(B)wiv; goto compareresult;
 case SSINGCASE(VA2CEQ-VA2CBW1111,SSINGBD): ziv=TEQ((B)aiv,wdv); goto compareresult;
 case SSINGCASE(VA2CEQ-VA2CBW1111,SSINGDB): ziv=TEQ(adv,(B)wiv); goto compareresult; 
 case SSINGCASE(VA2CEQ-VA2CBW1111,SSINGID): ziv=TEQ((D)aiv,wdv); goto compareresult;
 case SSINGCASE(VA2CEQ-VA2CBW1111,SSINGDI): ziv=TEQ(adv,(D)wiv); goto compareresult;
 case SSINGCASE(VA2CEQ-VA2CBW1111,SSINGBI): ziv=(B)aiv==wiv; goto compareresult;
 case SSINGCASE(VA2CEQ-VA2CBW1111,SSINGIB): ziv=aiv==(B)wiv; goto compareresult;
 case SSINGCASE(VA2CEQ-VA2CBW1111,SSINGII): ziv=aiv==wiv; goto compareresult;
 case SSINGCASE(VA2CEQ-VA2CBW1111,SSINGDD): ziv=TEQ(adv,wdv); goto compareresult;

 case SSINGCASE(VA2CEQABS-VA2CBW1111,SSINGDD): ziv=TEQ(adv,ABS(wdv)); goto compareresult;
 case SSINGCASE(VA2CNEABS-VA2CBW1111,SSINGDD): ziv=TNE(adv,ABS(wdv)); goto compareresult;
 case SSINGCASE(VA2CLTABS-VA2CBW1111,SSINGDD): ziv=TLT(adv,ABS(wdv)); goto compareresult;
 case SSINGCASE(VA2CLEABS-VA2CBW1111,SSINGDD): ziv=TLE(adv,ABS(wdv)); goto compareresult;
 case SSINGCASE(VA2CGEABS-VA2CBW1111,SSINGDD): ziv=TGE(adv,ABS(wdv)); goto compareresult;
 case SSINGCASE(VA2CGTABS-VA2CBW1111,SSINGDD): ziv=TGT(adv,ABS(wdv)); goto compareresult;


 case SSINGCASE(VA2CCIRCLE-VA2CBW1111,SSINGBB): adv=(B)aiv; wdv=(B)wiv; goto circleresult;
 case SSINGCASE(VA2CCIRCLE-VA2CBW1111,SSINGBD): adv=(B)aiv; goto circleresult;
 case SSINGCASE(VA2CCIRCLE-VA2CBW1111,SSINGDB): wdv=(B)wiv; goto circleresult;
 case SSINGCASE(VA2CCIRCLE-VA2CBW1111,SSINGID): adv=(D)aiv; goto circleresult;
 case SSINGCASE(VA2CCIRCLE-VA2CBW1111,SSINGDI): wdv=(D)wiv;  goto circleresult;
 case SSINGCASE(VA2CCIRCLE-VA2CBW1111,SSINGBI): adv=(D)(B)aiv; wdv=(D)wiv; goto circleresult;
 case SSINGCASE(VA2CCIRCLE-VA2CBW1111,SSINGIB): adv=(D)aiv; wdv=(D)(B)wiv; goto circleresult;
 case SSINGCASE(VA2CCIRCLE-VA2CBW1111,SSINGII): adv=(D)aiv; wdv=(D)wiv; goto circleresult;
 case SSINGCASE(VA2CCIRCLE-VA2CBW1111,SSINGDD): goto circleresult;


 case SSINGCASE(VA2CSTILE-VA2CBW1111,SSINGBB): ziv=(B)aiv<(B)wiv; goto compareresult;
 case SSINGCASE(VA2CSTILE-VA2CBW1111,SSINGIB): wiv=(I)(B)wiv; goto intresidue;
 case SSINGCASE(VA2CSTILE-VA2CBW1111,SSINGBI): aiv=(I)(B)aiv; goto intresidue;
 case SSINGCASE(VA2CSTILE-VA2CBW1111,SSINGII): 
  intresidue: ;
   ziv=((aiv&-aiv)+(aiv<=0)==0)?wiv&(aiv-1):remii(aiv,wiv);  // if positive power of 2, just AND; otherwise divide
   SSSTORE(ziv,z,INT,I); R z;
 case SSINGCASE(VA2CSTILE-VA2CBW1111,SSINGID): 
   ziv=jtremid(jt,aiv,wdv); if(!jt->jerr){SSSTORE(ziv,z,INT,I);}else z=0; R z;  // Since this can retry, we must not modify the input block if there is an error

 case SSINGCASE(VA2CSTILE-VA2CBW1111,SSINGBD): adv=(D)(B)aiv; goto floatresidue;
 case SSINGCASE(VA2CSTILE-VA2CBW1111,SSINGDB): wdv=(D)(B)wiv; goto floatresidue;
 case SSINGCASE(VA2CSTILE-VA2CBW1111,SSINGDI): wdv=(D)wiv;  goto floatresidue;
 case SSINGCASE(VA2CSTILE-VA2CBW1111,SSINGDD): 
  floatresidue: ;
   zdv=jtremdd(jt,adv,wdv); if(!jt->jerr){SSSTORE(zdv,z,FL,D);}else z=0; R z;  // Since this can retry, we must not modify the input block if there is an error
 default: ASSERTSYS(0,"ssing");
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
  // Kludge.  The calls to NAN0 and NAN1 (_clearfp or fetestexcept) are not known to the compiler, which assumes they clear ymm0-5.
  // This forces the compiler to allocate adv/wdv to ymm6-7, which in turns means they have to be saved over this routine.  Ecch.
  // To prevent this, we force them out to a new memory variable and refer to those copies.  And, we finish our use of zdv before the call
  // to NAN1
  {((D*)jt->shapesink)[0]=adv, ((D*)jt->shapesink)[1]=wdv; NAN0; zdv=bindd(((D*)jt->shapesink)[0],((D*)jt->shapesink)[1]); }
  SSSTORE(zdv,z,FL,D) if(unlikely(jt->jerr==EVNOCONV))jt->jerr=0;
  if(unlikely(zdv==0))if(jt->jerr!=0)R 0; NAN1;   // NaN error picked up in the routine sets result=0
  R z;  // Return the value if valid

 outofresultcvti:
  {((D*)jt->shapesink)[0]=adv, ((D*)jt->shapesink)[1]=wdv; NAN0; zdv=bindd(((D*)jt->shapesink)[0],((D*)jt->shapesink)[1]); }
  if(zdv>=(D)IMIN&&zdv<=(D)IMAX&&jt->jerr!=EVNOCONV){SSSTORE((I)zdv,z,INT,I)}else{SSSTORE(zdv,z,FL,D) if(unlikely(jt->jerr==EVNOCONV))jt->jerr=0;}
  if(unlikely(zdv==0))if(jt->jerr!=0)R 0; NAN1;   // NaN error picked up in the routine sets result=0
  R z;  // Return the value if valid, as integer if possible

 bitwiseresult:
 ziv=(I)FAV(self)->lu2.lc-VA2CBW0000;  // mask describing operation.  We refetch because self is needed in a reg and opcode isn't
 RE(0);  // if error on D arg, make sure we abort
 ziv=((aiv&wiv)&REPSGN(SGNIF(ziv,0)))|((aiv&~wiv)&REPSGN(SGNIF(ziv,1)))|((~aiv&wiv)&REPSGN(SGNIF(ziv,2)))|((~aiv&~wiv)&REPSGN(SGNIF(ziv,3)));
bitwiseexit:;
 SSSTORE(ziv,z,INT,I) R z;

 rotateresult:
 RE(0);  // if error on D arg, make sure we abort
 ziv=BW10000(aiv,wiv);  // do the rotate
 goto bitwiseexit;

 lslresult:
 RE(0);  // if error on D arg, make sure we abort
 ziv=BW10001(aiv,wiv);  // do the lsl
 goto bitwiseexit;

 aslresult:
 RE(0);  // if error on D arg, make sure we abort
 ziv=BW10010(aiv,wiv);  // do the rsl
 goto bitwiseexit;

 compareresult:
 if(likely(!z))R num(ziv);  // Don't store into num[], even though it wouldn't change the value - it would invalidate caches.  Only fallthrough is for nonzero rank
 SSSTORE((B)ziv,z,B01,B) R z;  // OK to store into allocated/inplace area.

 circleresult: ;
 D cirvals[3]={adv,wdv,0.0};  // put ops into memory (0.0 to avoid asan warnings)
 I rc=cirDD AH2A_v(1,cirvals,cirvals+1,cirvals+2,jt); rc=rc<0?EWOVIP+EWOVIPMULII:rc;  // run the routine
 if(rc==EVOK){SSSTORE(cirvals[2],z,FL,D);}else{jsignal(rc); z=0;} R z;  // Don't change the input block if there is an error.  If there is, post it to the return area
}


// reduce/prefix/suffix routines
// first word is the maximum valid precision bit index, followed by that many+1 routines for reduce, and then for prefix and suffix.
// routines are in bit-index order
// the last routine is always 0 to indicate invalid
// if there are integer-overflow routine, they comes after the others, in the order rps
VARPSA rpsnull = {0, {0}};

static VARPSA rpsbw0000 = {INTX+1 , { {(VARPSF)bw0000insI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw0000insI,VCVTIP+VI},
                             {(VARPSF)bw0000pfxI,VCVTIP+VI+VII}, {0,0}, {(VARPSF)bw0000pfxI,VCVTIP+VI},
                             {(VARPSF)bw0000sfxI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw0000sfxI,VCVTIP+VI},
                             }};
static VARPSA rpsbw0001 = {INTX+1 , { {(VARPSF)bw0001insI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw0001insI,VCVTIP+VI},
                             {(VARPSF)bw0001pfxI,VCVTIP+VI+VII}, {0,0}, {(VARPSF)bw0001pfxI,VCVTIP+VI},
                             {(VARPSF)bw0001sfxI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw0001sfxI,VCVTIP+VI},
                             }};
static VARPSA rpsbw0010 = {INTX+1 , { {(VARPSF)bw0010insI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw0010insI,VCVTIP+VI},
                             {0,0}, {0,0}, {0,0},
                             {(VARPSF)bw0010sfxI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw0010sfxI,VCVTIP+VI},
                             }};
static VARPSA rpsbw0011 = {INTX+1 , { {(VARPSF)bw0011insI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw0011insI,VCVTIP+VI},
                             {(VARPSF)bw0011pfxI,VCVTIP+VI+VII}, {0,0}, {(VARPSF)bw0011pfxI,VCVTIP+VI},
                             {(VARPSF)bw0011sfxI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw0011sfxI,VCVTIP+VI},
                             }};
static VARPSA rpsbw0100 = {INTX+1 , { {(VARPSF)bw0100insI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw0100insI,VCVTIP+VI},
                             {0,0}, {0,0}, {0,0},
                             {(VARPSF)bw0100sfxI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw0100sfxI,VCVTIP+VI},
                             }};
static VARPSA rpsbw0101 = {INTX+1 , { {(VARPSF)bw0101insI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw0101insI,VCVTIP+VI},
                             {(VARPSF)bw0101pfxI,VCVTIP+VI+VII}, {0,0}, {(VARPSF)bw0101pfxI,VCVTIP+VI},
                             {(VARPSF)bw0101sfxI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw0101sfxI,VCVTIP+VI},
                             }};
static VARPSA rpsbw0110 = {INTX+1 , { {(VARPSF)bw0110insI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw0110insI,VCVTIP+VI},
                             {(VARPSF)bw0110pfxI,VCVTIP+VI+VII}, {0,0}, {(VARPSF)bw0110pfxI,VCVTIP+VI},
                             {(VARPSF)bw0110sfxI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw0110sfxI,VCVTIP+VI},
                             }};
static VARPSA rpsbw0111 = {INTX+1 , { {(VARPSF)bw0111insI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw0111insI,VCVTIP+VI},
                             {(VARPSF)bw0111pfxI,VCVTIP+VI+VII}, {0,0}, {(VARPSF)bw0111pfxI,VCVTIP+VI},
                             {(VARPSF)bw0111sfxI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw0111sfxI,VCVTIP+VI},
                             }};
static VARPSA rpsbw1000 = {INTX+1 , { {(VARPSF)bw1000insI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw1000insI,VCVTIP+VI},
                             {0,0}, {0,0}, {0,0},
                             {(VARPSF)bw1000sfxI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw1000sfxI,VCVTIP+VI},
                             }};
static VARPSA rpsbw1001 = {INTX+1 , { {(VARPSF)bw1001insI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw1001insI,VCVTIP+VI},
                             {(VARPSF)bw1001pfxI,VCVTIP+VI+VII}, {0,0}, {(VARPSF)bw1001pfxI,VCVTIP+VI},
                             {(VARPSF)bw1001sfxI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw1001sfxI,VCVTIP+VI},
                             }};
static VARPSA rpsbw1010 = {INTX+1 , { {(VARPSF)bw1010insI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw1010insI,VCVTIP+VI},
                             {0,0}, {0,0}, {0,0},
                             {(VARPSF)bw1010sfxI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw1010sfxI,VCVTIP+VI},
                             }};
static VARPSA rpsbw1011 = {INTX+1 , { {(VARPSF)bw1011insI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw1011insI,VCVTIP+VI},
                             {0,0}, {0,0}, {0,0},
                             {(VARPSF)bw1011sfxI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw1011sfxI,VCVTIP+VI},
                             }};
static VARPSA rpsbw1100 = {INTX+1 , { {(VARPSF)bw1100insI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw1100insI,VCVTIP+VI},
                             {0,0}, {0,0}, {0,0},
                             {(VARPSF)bw1100sfxI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw1100sfxI,VCVTIP+VI},
                             }};
static VARPSA rpsbw1101 = {INTX+1 , { {(VARPSF)bw1101insI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw1101insI,VCVTIP+VI},
                             {0,0}, {0,0}, {0,0},
                             {(VARPSF)bw1101sfxI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw1101sfxI,VCVTIP+VI},
                             }};
static VARPSA rpsbw1110 = {INTX+1 , { {(VARPSF)bw1110insI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw1110insI,VCVTIP+VI},
                             {0,0}, {0,0}, {0,0},
                             {(VARPSF)bw1110sfxI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw1110sfxI,VCVTIP+VI},
                             }};
static VARPSA rpsbw1111 = {INTX+1 , { {(VARPSF)bw1111insI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw1111insI,VCVTIP+VI},
                             {(VARPSF)bw1111pfxI,VCVTIP+VI+VII}, {0,0}, {(VARPSF)bw1111pfxI,VCVTIP+VI},
                             {(VARPSF)bw1111sfxI,VCVTIP+VI+VII}, {0}, {(VARPSF)bw1111sfxI,VCVTIP+VI},
                             }};

static VARPSA rpsne = {B01X+1 , { {(VARPSF)neinsB,VCVTIP+VB},
                         {(VARPSF)nepfxB,VCVTIP+VB},
                         {(VARPSF)nesfxB,VCVTIP+VB},
                         }};
static VARPSA rpsnor = {B01X+1 , { {(VARPSF)norinsB,VCVTIP+VB},
                         {(VARPSF)norpfxB,VCVTIP+VB},
                         {(VARPSF)norsfxB,VCVTIP+VB},
                         }};
static VARPSA rpsor = {B01X+1 , { {(VARPSF)orinsB,VCVTIP+VB},
                         {(VARPSF)orpfxB,VCVTIP+VB},
                         {(VARPSF)orsfxB,VCVTIP+VB},
                         }};
static VARPSA rpseq = {B01X+1 , { {(VARPSF)eqinsB,VCVTIP+VB},
                         {(VARPSF)eqpfxB,VCVTIP+VB},
                         {(VARPSF)eqsfxB,VCVTIP+VB},
                         }};
static VARPSA rpsand = {B01X+1 , { {(VARPSF)andinsB,VCVTIP+VB},
                         {(VARPSF)andpfxB,VCVTIP+VB},
                         {(VARPSF)andsfxB,VCVTIP+VB},
                         }};
static VARPSA rpsnand = {B01X+1 , { {(VARPSF)nandinsB,VCVTIP+VB},
                         {(VARPSF)nandpfxB,VCVTIP+VB},
                         {(VARPSF)nandsfxB,VCVTIP+VB},
                         }};
static VARPSA rpsge = {B01X+1 , { {(VARPSF)geinsB,VCVTIP+VB},
                         {(VARPSF)gepfxB,VCVTIP+VB},
                         {(VARPSF)gesfxB,VCVTIP+VB},
                         }};
static VARPSA rpsle = {B01X+1 , { {(VARPSF)leinsB,VCVTIP+VB},
                         {(VARPSF)lepfxB,VCVTIP+VB},
                         {(VARPSF)lesfxB,VCVTIP+VB},
                         }};
static VARPSA rpsgt = {B01X+1 , { {(VARPSF)gtinsB,VCVTIP+VB},
                         {(VARPSF)gtpfxB,VCVTIP+VB},
                         {(VARPSF)gtsfxB,VCVTIP+VB},
                         }};
static VARPSA rpslt = {B01X+1 , { {(VARPSF)ltinsB,VCVTIP+VB},
                         {(VARPSF)ltpfxB,VCVTIP+VB},
                         {(VARPSF)ltsfxB,VCVTIP+VB},
                         }};

static VARPSA rpsdiv = {CMPXX+1 , { {(VARPSF)divinsD,VCVTIP+VD+VDD}, {0}, {(VARPSF)divinsD,VCVTIP+VD+VDD}, {(VARPSF)divinsD,VCVTIP+VD}, {(VARPSF)divinsZ,VCVTIP+VZ},
                          {(VARPSF)divpfxD,VCVTIP+VD+VDD}, {0}, {(VARPSF)divpfxD,VCVTIP+VD+VDD}, {(VARPSF)divpfxD,VCVTIP+VD}, {(VARPSF)divpfxZ,VCVTIP+VZ},
                          {(VARPSF)divsfxD,VCVTIP+VD+VDD}, {0}, {(VARPSF)divsfxD,VCVTIP+VD+VDD}, {(VARPSF)divsfxD,VCVTIP+VD}, {(VARPSF)divsfxZ,VCVTIP+VZ},
                         }};

static VARPSA rpsminus = {RATX+1 , {
{(VARPSF)minusinsB,VCVTIP+VI}, {0}, {(VARPSF)minusinsI,VCVTIP+VI}, {(VARPSF)minusinsD,VCVTIP+VD}, {(VARPSF)minusinsZ,VCVTIP+VZ},        {0}, {0}, {0},
{(VARPSF)minuspfxB,VCVTIP+VI}, {0}, {(VARPSF)minuspfxI,VCVTIP+VI}, {(VARPSF)minuspfxD,VCVTIP+VD+VIPOKW}, {(VARPSF)minuspfxZ,VCVTIP+VZ}, {0}, {(VARPSF)minuspfxX,VCVTIP+VX}, {(VARPSF)minuspfxQ,VCVTIP+VQ},
{(VARPSF)minussfxB,VCVTIP+VI}, {0}, {(VARPSF)minussfxI,VCVTIP+VI}, {(VARPSF)minussfxD,VCVTIP+VD+VIPOKW}, {(VARPSF)minussfxZ,VCVTIP+VZ},  {0}, {0}, {0},
{(VARPSF)minusinsO,VCVTIP+VD},{(VARPSF)minuspfxO,VCVTIP+VD},{(VARPSF)minussfxO,VCVTIP+VD},  // integer-overflow routines
}};
static VARPSA rpsplus = {QPX+1 , {
{(VARPSF)plusinsB,VCVTIP+VI}, {0}, {(VARPSF)plusinsI,VCVTIP+VI}, {(VARPSF)plusinsD,VCVTIP+VD}, {(VARPSF)plusinsZ,VCVTIP+VZ},        {0}, {0}, {0}, {0}, {(VARPSF)plusinsI2,VCVTIP+VI}, {(VARPSF)plusinsI4,VCVTIP+VI}, {0}, {0}, {SY_64?(VARPSF)plusinsE:0,VCVTIP+VUNCH},
{(VARPSF)pluspfxB,VCVTIP+VI}, {0}, {(VARPSF)pluspfxI,VCVTIP+VI}, {(VARPSF)pluspfxD,VCVTIP+VD+VIPOKW}, {(VARPSF)pluspfxZ,VCVTIP+VZ}, {0}, {(VARPSF)pluspfxX,VCVTIP+VX}, {(VARPSF)pluspfxQ,VCVTIP+VQ}, {0}, {(VARPSF)pluspfxI2,VCVTIP+VI}, {(VARPSF)pluspfxI4,VCVTIP+VI}, {0}, {0}, {0},
{(VARPSF)plussfxB,VCVTIP+VI}, {0}, {(VARPSF)plussfxI,VCVTIP+VI}, {(VARPSF)plussfxD,VCVTIP+VD+VIPOKW}, {(VARPSF)plussfxZ,VCVTIP+VZ}, {0}, {(VARPSF)plussfxX,VCVTIP+VX}, {(VARPSF)plussfxQ,VCVTIP+VQ}, {0}, {(VARPSF)plussfxI2,VCVTIP+VI}, {(VARPSF)plussfxI4,VCVTIP+VI}, {0}, {0}, {0},
{(VARPSF)plusinsO,VCVTIP+VD},{(VARPSF)pluspfxO,VCVTIP+VD},{(VARPSF)plussfxO,VCVTIP+VD},  // integer-overflow routines
}};
static VARPSA rpstymes = {RATX+1 , {
{(VARPSF)andinsB,VCVTIP+VB}, {0}, {(VARPSF)tymesinsI,VCVTIP+VI}, {(VARPSF)tymesinsD,VCVTIP+VD}, {(VARPSF)tymesinsZ,VCVTIP+VZ},        {0}, {0}, {0},
{(VARPSF)andpfxB,VCVTIP+VB}, {0}, {(VARPSF)tymespfxI,VCVTIP+VI}, {(VARPSF)tymespfxD,VCVTIP+VD+VIPOKW}, {(VARPSF)tymespfxZ,VCVTIP+VZ}, {0}, {(VARPSF)tymespfxX,VCVTIP+VX}, {(VARPSF)tymespfxQ,VCVTIP+VQ},
{(VARPSF)andsfxB,VCVTIP+VB}, {0}, {(VARPSF)tymessfxI,VCVTIP+VI}, {(VARPSF)tymessfxD,VCVTIP+VD+VIPOKW}, {(VARPSF)tymessfxZ,VCVTIP+VZ}, {0}, {(VARPSF)tymessfxX,VCVTIP+VX}, {(VARPSF)tymessfxQ,VCVTIP+VQ},
{(VARPSF)tymesinsO,VCVTIP+VD},{(VARPSF)tymespfxO,VCVTIP+VD},{(VARPSF)tymessfxO,VCVTIP+VD},  // integer-overflow routines
}};

static VARPSA rpsmin = {SBTX+1 , {
{(VARPSF)andinsB,VCVTIP+VB}, {0}, {(VARPSF)mininsI,VCVTIP+VI}, {(VARPSF)mininsD,VCVTIP+VD}, {(VARPSF)mininsD,VCVTIP+VD+VDD}, {0}, {(VARPSF)mininsX,VCVTIP+VX}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {(VARPSF)mininsS,VCVTIP+VSB},
{(VARPSF)andpfxB,VCVTIP+VB}, {0}, {(VARPSF)minpfxI,VCVTIP+VI+VIPOKW}, {(VARPSF)minpfxD,VCVTIP+VD+VIPOKW}, {(VARPSF)minpfxD,VCVTIP+VD+VDD}, {0}, {(VARPSF)minpfxX,VCVTIP+VX}, {(VARPSF)minpfxQ,VCVTIP+VQ}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {(VARPSF)minpfxS,VCVTIP+VSB},
{(VARPSF)andsfxB,VCVTIP+VB}, {0}, {(VARPSF)minsfxI,VCVTIP+VI+VIPOKW}, {(VARPSF)minsfxD,VCVTIP+VD+VIPOKW}, {(VARPSF)minsfxD,VCVTIP+VD+VDD}, {0}, {(VARPSF)minsfxX,VCVTIP+VX}, {(VARPSF)minsfxQ,VCVTIP+VQ}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {(VARPSF)minsfxS,VCVTIP+VSB},
}};
static VARPSA rpsmax = {SBTX+1 , {
{(VARPSF)orinsB,VCVTIP+VB}, {0}, {(VARPSF)maxinsI,VCVTIP+VI}, {(VARPSF)maxinsD,VCVTIP+VD}, {(VARPSF)maxinsD,VCVTIP+VD+VDD}, {0}, {(VARPSF)maxinsX,VCVTIP+VX}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {(VARPSF)maxinsS,VCVTIP+VSB},
{(VARPSF)orpfxB,VCVTIP+VB}, {0}, {(VARPSF)maxpfxI,VCVTIP+VI+VIPOKW}, {(VARPSF)maxpfxD,VCVTIP+VD+VIPOKW}, {(VARPSF)maxpfxD,VCVTIP+VD+VDD}, {0}, {(VARPSF)maxpfxX,VCVTIP+VX}, {(VARPSF)maxpfxQ,VCVTIP+VQ}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {(VARPSF)maxpfxS,VCVTIP+VSB},
{(VARPSF)orsfxB,VCVTIP+VB}, {0}, {(VARPSF)maxsfxI,VCVTIP+VI+VIPOKW}, {(VARPSF)maxsfxD,VCVTIP+VD+VIPOKW}, {(VARPSF)maxsfxD,VCVTIP+VD+VDD}, {0}, {(VARPSF)maxsfxX,VCVTIP+VX}, {(VARPSF)maxsfxQ,VCVTIP+VQ}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {(VARPSF)maxsfxS,VCVTIP+VSB},
}};


VA va[]={
/* non-atomic functions      */ {
 {{0,0}, {0,0}, {0,0},                                /* BB BI BD              */
  {0,0}, {0,0}, {0,0},                                /* IB II ID              */
  {0,0}, {0,0}, {0,0},                                /* DB DI DD              */
  {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},                        /* ZZ XX QQ Symb DS E I2 I4         */
  &rpsnull},

/* 10    */ {
 {{(VF)bw0000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0000II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw0000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0000II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw0000II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw0000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0000II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw0000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0000II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw0000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0000I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw0000I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsbw0000},

/* 11    */ {
 {{(VF)bw0001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0001II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw0001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0001II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw0001II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw0001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0001II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw0001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0001II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw0001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0001I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw0001I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsbw0001},

/* 12    */ {
 {{(VF)bw0010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0010II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw0010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0010II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw0010II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw0010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0010II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw0010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0010II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw0010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0010I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw0010I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsbw0010},

/* 13    */ {
 {{(VF)bw0011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0011II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw0011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0011II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw0011II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw0011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0011II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw0011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0011II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw0011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0011I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw0011I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsbw0011},

/* 14    */ {
 {{(VF)bw0100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0100II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw0100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0100II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw0100II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw0100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0100II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw0100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0100II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw0100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0100I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw0100I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsbw0100},

/* 15    */ {
 {{(VF)bw0101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0101II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw0101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0101II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw0101II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw0101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0101II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw0101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0101II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw0101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0101I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw0101I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsbw0101},

/* 16    */ {
 {{(VF)bw0110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0110II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw0110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0110II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw0110II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw0110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0110II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw0110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0110II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw0110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0110I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw0110I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsbw0110},

/* 17    */ {
 {{(VF)bw0111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0111II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw0111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0111II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw0111II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw0111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0111II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw0111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0111II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw0111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw0111I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw0111I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsbw0111},

/* 18    */ {
 {{(VF)bw1000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1000II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw1000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1000II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw1000II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw1000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1000II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw1000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1000II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw1000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1000I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw1000I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsbw1000},

/* 19    */ {
 {{(VF)bw1001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1001II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw1001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1001II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw1001II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw1001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1001II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw1001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1001II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw1001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1001I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw1001I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsbw1001},

/* 1a    */ {
 {{(VF)bw1010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1010II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw1010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1010II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw1010II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw1010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1010II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw1010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1010II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw1010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1010I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw1010I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsbw1010},

/* 1b    */ {
 {{(VF)bw1011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1011II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw1011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1011II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw1011II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw1011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1011II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw1011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1011II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw1011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1011II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1011I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw1011I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsbw1011},

/* 1c    */ {
 {{(VF)bw1100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1100II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw1100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1100II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw1100II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw1100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1100II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw1100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1100II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw1100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1100II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1100I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw1100I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsbw1100},

/* 1d    */ {
 {{(VF)bw1101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1101II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw1101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1101II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw1101II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw1101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1101II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw1101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1101II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw1101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1101II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1101I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw1101I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsbw1101},

/* 1e    */ {
 {{(VF)bw1110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1110II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw1110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1110II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw1110II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw1110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1110II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw1110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1110II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw1110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1110II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1110I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw1110I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsbw1110},

/* 1f    */ {
 {{(VF)bw1111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1111II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw1111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1111II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw1111II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw1111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1111II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw1111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1111II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw1111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1111II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw1111I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw1111I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsbw1111},

/* 32 b.    */ {
 {{(VF)bw10000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10000II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw10000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10000II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw10000II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw10000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10000II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw10000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10000II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw10000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10000II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10000I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw10000I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsnull},

/* 33 b.    */ {
 {{(VF)bw10001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10001II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw10001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10001II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw10001II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw10001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10001II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw10001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10001II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw10001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10001II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10001I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw10001I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsnull},

/* 34 b.    */ {
 {{(VF)bw10010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10010II,VRNONE+VCVTIP+VI+VII+VIP}, 
  {(VF)bw10010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10010II,VRNONE+VCVTIP+VI+VIP},     {(VF)bw10010II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw10010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10010II,VRNONE+VCVTIP+VI+VII+VIP},
  {(VF)bw10010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10010II,VRNONE+VCVTIP+VI+VII+VIP}, {0,0}, {(VF)bw10010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10010II,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)bw10010I2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)bw10010I4I4,VRNONE+VCVTIP+VUNCH+VIP}}, 
  &rpsnull},


   // For Booleans, VIP means 'inplace if rank not specified and there is no frame'
/* 95 ~: */ {
 {{(VF)neBB,VRNONE+VCVTIP+VB+VIP}, {(VF)neBI,VRNONE+VCVTIP+VB+VIPOKA}, {(VF)neBD,VRNONE+VCVTIP+VB+VIPOKA},
  {(VF)neIB,VRNONE+VCVTIP+VB+VIPOKW}, {(VF)neII,VRNONE+VCVTIP+VB}, {(VF)neID,VRNONE+VCVTIP+VB},
  {(VF)neDB,VRNONE+VCVTIP+VB+VIPOKW}, {(VF)neDI,VRNONE+VCVTIP+VB}, {(VF)neDD,VRNONE+VCVTIP+VB}, 
  {(VF)neZZ,VRNONE+VCVTIP+VB+VZZ}, {(VF)neXX,VRNONE+VCVTIP+VB+VXEQ}, {(VF)neQQ,VRNONE+VCVTIP+VB+VQQ}, {0,0}, {(VF)0,VRNONE+VCVTIP+VB}, {(VF)neEE,VRNONE+VCVTIP+VB}, {(VF)neI2I2,VRNONE+VCVTIP+VB}, {(VF)neI4I4,VRNONE+VCVTIP+VB}},
  &rpsne},

/* 25 %  */ {
 {{(VF)divBB,VRNONE+VCVTIP+VD}, {(VF)divBI,VRNONE+VCVTIP+VD+VIP0I}, {(VF)divBD,VRNONE+VCVTIP+VD+VIPOKW},
  {(VF)divIB,VRNONE+VCVTIP+VD+VIPI0}, {(VF)divII,VRNONE+VCVTIP+VD+VIPI0+VIP0I}, {(VF)divID,VRNONE+VCVTIP+VD+VIPID},
  {(VF)divDB,VRNONE+VCVTIP+VD+VIPOKA}, {(VF)divDI,VRNONE+VCVTIP+VD+VIPDI}, {(VF)divDD,VRNONE+VCVTIP+VD+VIP+VCANHALT}, 
  {(VF)divZZ,VRNONE+VCVTIP+VZ+VZZ+VIP}, {(VF)divXX,VRNONE+VCVTIP+VX+VXX}, {(VF)divQQ,VRNONE+VCVTIP+VQ+VQQ}, {0,0}, {(VF)0,VRNONE+VCVTIP+VIP+VCANHALT}, {(VF)divEE,VRNONE+VCVTIP+VIP+VCANHALT}, {(VF)divII,VRNONE+VCVTIP+VII+VD+VIPI0+VIP0I}, {(VF)divII,VRNONE+VCVTIP+VII+VD+VIPI0+VIP0I}},
  &rpsdiv},

/* 89 +: */ {
 {{(VF)norBB,VRNONE+VCVTIP+VB+VIP    }, {(VF)norBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)norBB,VRNONE+VCVTIP+VB+VBB+VIP},
  {(VF)norBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)norBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)norBB,VRNONE+VCVTIP+VB+VBB+VIP},
  {(VF)norBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)norBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)norBB,VRNONE+VCVTIP+VB+VBB+VIP}, 
  {(VF)norBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)norBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)norBB,VRNONE+VCVTIP+VB+VBB+VIP}, {0,0}, {(VF)norBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)norBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)norBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)norBB,VRNONE+VCVTIP+VB+VBB+VIP}},
  &rpsnor},

/* 88 +. */ {
 {{(VF)orBB,VRNONE+VCVTIP+VB+VIP     }, {(VF)gcdII,VRNONE+VCVTIP+VI+VII}, {(VF)gcdDD,VRNONE+VCVTIP+VD+VDD+VIP},
  {(VF)gcdII,VRNONE+VCVTIP+VI+VII}, {(VF)gcdII,VRNONE+VCVTIP+VI}, {(VF)gcdDD,VRNONE+VCVTIP+VD+VDD+VIP},
  {(VF)gcdDD,VRNONE+VCVTIP+VD+VDD+VIP}, {(VF)gcdDD,VRNONE+VCVTIP+VD+VDD+VIP}, {(VF)gcdDD,VRNONE+VCVTIP+VD+VIP+VCANHALT}, 
  {(VF)gcdZZ,VRNONE+VCVTIP+VZ+VZZ}, {(VF)gcdXX,VRNONE+VCVTIP+VX+VXX}, {(VF)gcdQQ,VRNONE+VCVTIP+VQ+VQQ}, {0,0}, {(VF)gcdDD,VRNONE+VCVTIP+VD+VDD+VIP+VCANHALT}, {(VF)gcdDD,VRNONE+VCVTIP+VD+VDD+VIP+VCANHALT}, {(VF)gcdII,VRNONE+VCVTIP+VII+VI}, {(VF)gcdII,VRNONE+VCVTIP+VII+VI}},
  &rpsor},

/* 2d -  */ {
 {{(VF)minusBB,VRNONE+VCVTIP+VI    }, {(VF)minusBI,VRNONE+VCVTIP+VI+VIPOKW}, {(VF)minusBD,VRNONE+VCVTIP+VD+VIPOKW}, 
  {(VF)minusIB,VRNONE+VCVTIP+VI+VIPOKA}, {(VF)minusII,VRNONE+VCVTIP+VI+VIP}, {(VF)minusID,VRNONE+VCVTIP+VD+VIPID},
  {(VF)minusDB,VRNONE+VCVTIP+VD+VIPOKA    }, {(VF)minusDI,VRNONE+VCVTIP+VD+VIPDI    }, {(VF)minusDD,VRNONE+VCVTIP+VD+VIP+VCANHALT}, 
  {(VF)minusZZ,VRNONE+VCVTIP+VZ+VZZ+VIP}, {(VF)minusXX,VRNONE+VCVTIP+VX+VXX}, {(VF)minusQQ,VRNONE+VCVTIP+VQ+VQQ}, {0,0}, {(VF)0,VRNONE+VCVTIP+VIP+VCANHALT}, {(VF)minusEE,VRNONE+VCVTIP+VIP+VCANHALT}, {(VF)minusI2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)minusI4I4,VRNONE+VCVTIP+VUNCH+VIP}},
  &rpsminus},

   // For Booleans, VIP means 'inplace if rank not specified and there is no frame'
/* 3c <  */ {
 {{(VF)ltBB,VRNONE+VCVTIP+VB+VIP}, {(VF)ltBI,VRNONE+VCVTIP+VB+VIPOKA}, {(VF)ltBD,VRNONE+VCVTIP+VB+VIPOKA},
  {(VF)ltIB,VRNONE+VCVTIP+VB+VIPOKW}, {(VF)ltII,VRNONE+VCVTIP+VB}, {(VF)ltID,VRNONE+VCVTIP+VB},
  {(VF)ltDB,VRNONE+VCVTIP+VB+VIPOKW}, {(VF)ltDI,VRNONE+VCVTIP+VB}, {(VF)ltDD,VRNONE+VCVTIP+VB}, 
  {(VF)ltDD,VRNONE+VCVTIP+VB+VDD+VIP}, {(VF)ltXX,VRNONE+VCVTIP+VB+VXFC}, {(VF)ltQQ,VRNONE+VCVTIP+VB+VQQ}, {(VF)ltSS,VRNONE+VCVTIP+VB}, {(VF)0,VRNONE+VCVTIP+VB}, {(VF)ltEE,VRNONE+VCVTIP+VB}, {(VF)ltI2I2,VRNONE+VCVTIP+VB}, {(VF)ltI4I4,VRNONE+VCVTIP+VB}},
  &rpslt},

/* 3d =  */ {
 {{(VF)eqBB,VRNONE+VCVTIP+VB+VIP}, {(VF)eqBI,VRNONE+VCVTIP+VB+VIPOKA}, {(VF)eqBD,VRNONE+VCVTIP+VB+VIPOKA},
  {(VF)eqIB,VRNONE+VCVTIP+VB+VIPOKW}, {(VF)eqII,VRNONE+VCVTIP+VB}, {(VF)eqID,VRNONE+VCVTIP+VB},
  {(VF)eqDB,VRNONE+VCVTIP+VB+VIPOKW}, {(VF)eqDI,VRNONE+VCVTIP+VB}, {(VF)eqDD,VRNONE+VCVTIP+VB}, 
  {(VF)eqZZ,VRNONE+VCVTIP+VB+VZZ}, {(VF)eqXX,VRNONE+VCVTIP+VB+VXEQ}, {(VF)eqQQ,VRNONE+VCVTIP+VB+VQQ}, {(VF)eqII,VRNONE+VCVTIP+VB}, {(VF)0,VRNONE+VCVTIP+VB}, {(VF)eqEE,VRNONE+VCVTIP+VB}, {(VF)eqI2I2,VRNONE+VCVTIP+VB}, {(VF)eqI4I4,VRNONE+VCVTIP+VB}},
  &rpseq},

/* 3e >  */ {
 {{(VF)gtBB,VRNONE+VCVTIP+VB+VIP}, {(VF)gtBI,VRNONE+VCVTIP+VB+VIPOKA}, {(VF)gtBD,VRNONE+VCVTIP+VB+VIPOKA},
  {(VF)gtIB,VRNONE+VCVTIP+VB+VIPOKW}, {(VF)gtII,VRNONE+VCVTIP+VB}, {(VF)gtID,VRNONE+VCVTIP+VB},
  {(VF)gtDB,VRNONE+VCVTIP+VB+VIPOKW}, {(VF)gtDI,VRNONE+VCVTIP+VB}, {(VF)gtDD,VRNONE+VCVTIP+VB}, 
  {(VF)gtDD,VRNONE+VCVTIP+VB+VDD+VIP}, {(VF)gtXX,VRNONE+VCVTIP+VB+VXCF}, {(VF)gtQQ,VRNONE+VCVTIP+VB+VQQ}, {(VF)gtSS,VRNONE+VCVTIP+VB}, {(VF)0,VRNONE+VCVTIP+VB}, {(VF)gtEE,VRNONE+VCVTIP+VB}, {(VF)gtI2I2,VRNONE+VCVTIP+VB}, {(VF)gtI4I4,VRNONE+VCVTIP+VB}},
  &rpsgt},

/* 8a *. */ {
 {{(VF)andBB,VRNONE+VCVTIP+VB+VIP    }, {(VF)lcmII,VRNONE+VCVTIP+VI+VII}, {(VF)lcmDD,VRNONE+VCVTIP+VD+VDD+VIP},
  {(VF)lcmII,VRNONE+VCVTIP+VI+VII}, {(VF)lcmII,VRNONE+VCVTIP+VI    }, {(VF)lcmDD,VRNONE+VCVTIP+VD+VDD+VIP},
  {(VF)lcmDD,VRNONE+VCVTIP+VD+VDD+VIP}, {(VF)lcmDD,VRNONE+VCVTIP+VD+VDD+VIP}, {(VF)lcmDD,VRNONE+VCVTIP+VD+VIP+VCANHALT}, 
  {(VF)lcmZZ,VRNONE+VCVTIP+VZ+VZZ}, {(VF)lcmXX,VRNONE+VCVTIP+VX+VXX}, {(VF)lcmQQ,VRNONE+VCVTIP+VQ+VQQ}, {0,0}, {(VF)0,VRNONE+VCVTIP+VD+VDD+VIP+VCANHALT}, {(VF)lcmDD,VRNONE+VCVTIP+VD+VDD+VIP+VCANHALT}, {(VF)lcmII,VRNONE+VCVTIP+VII+VI}, {(VF)lcmII,VRNONE+VCVTIP+VII+VI}},
  &rpsand},

/* 8b *: */ {
 {{(VF)nandBB,VRNONE+VCVTIP+VB+VIP},     {(VF)nandBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)nandBB,VRNONE+VCVTIP+VB+VBB+VIP},
  {(VF)nandBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)nandBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)nandBB,VRNONE+VCVTIP+VB+VBB+VIP},
  {(VF)nandBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)nandBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)nandBB,VRNONE+VCVTIP+VB+VBB+VIP}, 
  {(VF)nandBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)nandBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)nandBB,VRNONE+VCVTIP+VB+VBB+VIP}, {0,0}, {(VF)nandBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)nandBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)nandBB,VRNONE+VCVTIP+VB+VBB+VIP}, {(VF)nandBB,VRNONE+VCVTIP+VB+VBB+VIP}},
  &rpsnand},

/* 85 >: */ {
 {{(VF)geBB,VRNONE+VCVTIP+VB+VIP}, {(VF)geBI,VRNONE+VCVTIP+VB+VIPOKA}, {(VF)geBD,VRNONE+VCVTIP+VB+VIPOKA},
  {(VF)geIB,VRNONE+VCVTIP+VB+VIPOKW}, {(VF)geII,VRNONE+VCVTIP+VB}, {(VF)geID,VRNONE+VCVTIP+VB},
  {(VF)geDB,VRNONE+VCVTIP+VB+VIPOKW}, {(VF)geDI,VRNONE+VCVTIP+VB}, {(VF)geDD,VRNONE+VCVTIP+VB}, 
  {(VF)geDD,VRNONE+VCVTIP+VB+VDD+VIP}, {(VF)geXX,VRNONE+VCVTIP+VB+VXFC}, {(VF)geQQ,VRNONE+VCVTIP+VB+VQQ}, {(VF)geSS,VRNONE+VCVTIP+VB}, {(VF)0,VRNONE+VCVTIP+VB}, {(VF)geEE,VRNONE+VCVTIP+VB}, {(VF)geI2I2,VRNONE+VCVTIP+VB}, {(VF)geI4I4,VRNONE+VCVTIP+VB}},
  &rpsge},

/* 83 <: */ {
 {{(VF)leBB,VRNONE+VCVTIP+VB+VIP}, {(VF)leBI,VRNONE+VCVTIP+VB+VIPOKA}, {(VF)leBD,VRNONE+VCVTIP+VB+VIPOKA},
  {(VF)leIB,VRNONE+VCVTIP+VB+VIPOKW}, {(VF)leII,VRNONE+VCVTIP+VB}, {(VF)leID,VRNONE+VCVTIP+VB},
  {(VF)leDB,VRNONE+VCVTIP+VB+VIPOKW}, {(VF)leDI,VRNONE+VCVTIP+VB}, {(VF)leDD,VRNONE+VCVTIP+VB}, 
  {(VF)leDD,VRNONE+VCVTIP+VB+VDD+VIP}, {(VF)leXX,VRNONE+VCVTIP+VB+VXCF}, {(VF)leQQ,VRNONE+VCVTIP+VB+VQQ}, {(VF)leSS,VRNONE+VCVTIP+VB}, {(VF)0,VRNONE+VCVTIP+VB}, {(VF)leEE,VRNONE+VCVTIP+VB}, {(VF)leI2I2,VRNONE+VCVTIP+VB}, {(VF)leI4I4,VRNONE+VCVTIP+VB}},
  &rpsle},

/* 82 <. */ {
 {{(VF)andBB,VRNONE+VCVTIP+VB+VIP}, {(VF)minBI,VRNONE+VCVTIP+VI+VIPOKW}, {(VF)minBD,VRNONE+VCVTIP+VD+VIPOKW},
  {(VF)minIB,VRNONE+VCVTIP+VI+VIPOKA}, {(VF)minII,VRNONE+VCVTIP+VI+VIP}, {(VF)minID,VRNONE+VCVTIP+VD+VIPID},
  {(VF)minDB,VRNONE+VCVTIP+VD+VIPOKA}, {(VF)minDI,VRNONE+VCVTIP+VD+VIPDI}, {(VF)minDD,VRNONE+VCVTIP+VD+VIP}, 
  {(VF)minDD,VRNONE+VCVTIP+VD+VDD+VIP}, {(VF)minXX,VRNONE+VCVTIP+VX+VXX}, {(VF)minQQ,VRNONE+VCVTIP+VQ+VQQ}, {(VF)minSS,VRNONE+VCVTIP+VSB}, {(VF)0,VRNONE+VCVTIP+VIP}, {(VF)minEE,VRNONE+VCVTIP+VIP}, {(VF)minI2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)minI4I4,VRNONE+VCVTIP+VUNCH+VIP}},  // always VIP a forced conversion
  &rpsmin},

/* 84 >. */ {
 {{(VF)orBB,VRNONE+VCVTIP+VB+VIP}, {(VF)maxBI,VRNONE+VCVTIP+VI+VIPOKW}, {(VF)maxBD,VRNONE+VCVTIP+VD+VIPOKW},
  {(VF)maxIB,VRNONE+VCVTIP+VI+VIPOKA}, {(VF)maxII,VRNONE+VCVTIP+VI+VIP}, {(VF)maxID,VRNONE+VCVTIP+VD+VIPID},
  {(VF)maxDB,VRNONE+VCVTIP+VD+VIPOKA}, {(VF)maxDI,VRNONE+VCVTIP+VD+VIPDI}, {(VF)maxDD,VRNONE+VCVTIP+VD+VIP}, 
  {(VF)maxDD,VRNONE+VCVTIP+VD+VDD+VIP}, {(VF)maxXX,VRNONE+VCVTIP+VX+VXX}, {(VF)maxQQ,VRNONE+VCVTIP+VQ+VQQ}, {(VF)maxSS,VRNONE+VCVTIP+VSB}, {(VF)0,VRNONE+VCVTIP+VIP}, {(VF)maxEE,VRNONE+VCVTIP+VIP}, {(VF)maxI2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)maxI4I4,VRNONE+VCVTIP+VUNCH+VIP}},
  &rpsmax},

/* 2b +  */ {
 {{(VF)plusBB,VRNONE+VCVTIP+VI    }, {(VF)plusBI,VRNONE+VCVTIP+VI+VIPOKW}, {(VF)plusBD,VRNONE+VCVTIP+VD+VIPOKW}, 
  {(VF)plusIB,VRNONE+VCVTIP+VI+VIPOKA}, {(VF)plusII,VRNONE+VCVTIP+VI+VIP}, {(VF)plusID,VRNONE+VCVTIP+VD+VIPID}, 
  {(VF)plusDB,VRNONE+VCVTIP+VD+VIPOKA    }, {(VF)plusDI,VRNONE+VCVTIP+VD+VIPDI    }, {(VF)plusDD,VRNONE+VCVTIP+VD+VIP+VCANHALT}, 
  {(VF)plusZZ,VRNONE+VCVTIP+VZ+VZZ+VIP}, {(VF)plusXX,VRNONE+VCVTIP+VX+VXX}, {(VF)plusQQ,VRNONE+VCVTIP+VQ+VQQ}, {0,0}, {(VF)0,VRNONE+VCVTIP+VIP+VCANHALT}, {(VF)plusEE,VRNONE+VCVTIP+VIP+VCANHALT}, {(VF)plusI2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)plusI4I4,VRNONE+VCVTIP+VUNCH+VIP}},
  &rpsplus},

/* 2a *  */ {
#if SY_64  // boolean multiply is the same for float and double if value length the same
#define tymesBI tymesBD
#define tymesIB tymesDB
#endif
 {{(VF)andBB,VRNONE+VCVTIP+VB+VIP}, {(VF)tymesBI,VRNONE+VCVTIP+VI+VIPOKW}, {(VF)tymesBD,VRNONE+VCVTIP+VD+VIPOKW},
  {(VF)tymesIB,VRNONE+VCVTIP+VI+VIPOKA}, {(VF)tymesII,VRNONE+VCVTIP+VI+VIP}, {(VF)tymesID,VRNONE+VCVTIP+VD+VIPID},
  {(VF)tymesDB,VRNONE+VCVTIP+VD+VIPOKA}, {(VF)tymesDI,VRNONE+VCVTIP+VD+VIPDI}, {(VF)tymesDD,VRNONE+VCVTIP+VD+VIP}, 
  {(VF)tymesZZ,VRNONE+VCVTIP+VZ+VZZ+VIP}, {(VF)tymesXX,VRNONE+VCVTIP+VX+VXX}, {(VF)tymesQQ,VRNONE+VCVTIP+VQ+VQQ}, {0,0}, {(VF)0,VRNONE+VCVTIP+VIP}, {(VF)tymesEE,VRNONE+VCVTIP+VIP}, {(VF)tymesI2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)tymesI4I4,VRNONE+VCVTIP+VUNCH+VIP}},
  &rpstymes},

/* 5e ^  */ {   // may produce complex numbers
 {{(VF)geBB,VRNONE+VCVTIP+VB+VIP}, {(VF)powBI,VRNONE+VCVTIP+VD}, {(VF)powBD,VRNONE+VCVTIP+VD},
  {(VF)powIB,VRNONE+VCVTIP+VI}, {(VF)powII,VRNONE+VCVTIP+VD}, {(VF)powID,VRNONE+VCVTIP+VD+VCANHALT},
  {(VF)powDB,VRNONE+VCVTIP+VD}, {(VF)powDI,VRNONE+VCVTIP+VD}, {(VF)powDD,VRNONE+VCVTIP+VD+VCANHALT}, 
  {(VF)powZZ,VRNONE+VCVTIP+VZ+VZZ}, {(VF)powXX,VRNONE+VCVTIP+VX+VXX}, {(VF)powQQ,VRNONE+VCVTIP+VQ+VQQ}, {0,0}, {(VF)0,VRNONE+VCVTIP+VD+VDD+VCANHALT}, {(VF)powEE,VRNONE+VCVTIP+VCANHALT}, {(VF)powII,VRNONE+VCVTIP+VII+VD+VCANHALT}, {(VF)powII,VRNONE+VCVTIP+VII+VD+VCANHALT}},
  &rpsge},

/* 7c |  */ {
 {{(VF)ltBB,VRNONE+VCVTIP+VB+VIP    }, {(VF)remII,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)remDD,VRNONE+VCVTIP+VD+VDD+VIP},
  {(VF)remII,VRNONE+VCVTIP+VI+VII+VIP}, {(VF)remII,VRNONE+VCVTIP+VI+VIP}, {(VF)remID,VRNONE+VCVTIP+VI+VCANHALT    },   // remID can 'overflow' if result is nonintegral
  {(VF)remDD,VRNONE+VCVTIP+VD+VDD+VIP}, {(VF)remDD,VRNONE+VCVTIP+VD+VDD+VIP}, {(VF)remDD,VRNONE+VCVTIP+VD+VIP+VCANHALT}, 
  {(VF)remZZ,VRNONE+VCVTIP+VZ+VZZ}, {(VF)remXX,VRNONE+VCVTIP+VX+VXX}, {(VF)remQQ,VRNONE+VCVTIP+VQ+VQQ}, {0,0}, {(VF)0,VRNONE+VCVTIP+VD+VDD+VIP+VCANHALT}, {(VF)remDD,VRNONE+VCVTIP+VD+VDD+VIP+VCANHALT}, {(VF)remI2I2,VRNONE+VCVTIP+VUNCH+VIP}, {(VF)remI4I4,VRNONE+VCVTIP+VUNCH+VIP}},
  &rpslt},

/* 21 !  */ {
 {{(VF)leBB,VRNONE+VCVTIP+VB+VIP            }, {(VF)binDD,VRI+VCVTIP+VD+VDD+VIP}, {(VF)binDD,VRNONE+VCVTIP+VD+VDD+VIP}, 
  {(VF)binDD,VRI+VCVTIP+VD+VDD+VIP}, {(VF)binDD,VRI+VCVTIP+VD+VDD+VIP}, {(VF)binDD,VRNONE+VCVTIP+VD+VDD+VIP}, 
  {(VF)binDD,VRNONE+VCVTIP+VD+VDD+VIP    }, {(VF)binDD,VRNONE+VCVTIP+VD+VDD+VIP    }, {(VF)binDD,VRNONE+VCVTIP+VD+VIP}, 
  {(VF)binZZ,VRNONE+VCVTIP+VZ+VZZ}, {(VF)binXX,VRNONE+VCVTIP+VX+VXX}, {(VF)binQQ,VRNONE+VCVTIP+VX+VQQ}, {0,0}, {(VF)0,VRNONE+VCVTIP+VD+VDD+VIP}, {(VF)binDD,VRNONE+VCVTIP+VD+VDD+VIP}, {(VF)binDD,VRNONE+VCVTIP+VD+VDD+VIP}, {(VF)binDD,VRNONE+VCVTIP+VD+VDD+VIP}}, 
  &rpsle},

/* d1 o. */ {
 {{(VF)cirDD,VRNONE+VCVTIP+VD+VDD}, {(VF)cirDD,VRNONE+VCVTIP+VD+VDD}, {(VF)cirBD,VRNONE+VCVTIP+VD},
  {(VF)cirDD,VRNONE+VCVTIP+VD+VDD}, {(VF)cirDD,VRNONE+VCVTIP+VD+VDD}, {(VF)cirID,VRNONE+VCVTIP+VD},
  {(VF)cirDD,VRNONE+VCVTIP+VD+VDD}, {(VF)cirDD,VRNONE+VCVTIP+VD+VDD}, {(VF)cirDD,VRNONE+VCVTIP+VD}, 
  {(VF)cirZZ,VRD+VCVTIP+VZ+VZZ}, {(VF)cirDD,VRNONE+VCVTIP+VD+VDD}, {(VF)cirDD,VRNONE+VCVTIP+VD+VDD}, {0,0}, {0,0}, {(VF)cirEE,VRNONE+VCVTIP+VCANHALT}, {(VF)cirDD,VRNONE+VCVTIP+VDD+VD}, {(VF)cirDD,VRNONE+VCVTIP+VDD+VD}},
  &rpsnull},


/* -- (compare |) */ {
// these routines are used only for floating-point types (DD), so they overlap
 {{0,0}, {0,0}, {0,0},
  {(VF)eqabsDD,VRNONE+VCVTIP+VB}, {(VF)neabsDD,VRNONE+VCVTIP+VB}, {(VF)ltabsDD,VRNONE+VCVTIP+VB},
  {(VF)leabsDD,VRNONE+VCVTIP+VB}, {(VF)geabsDD,VRNONE+VCVTIP+VB}, {(VF)gtabsDD,VRNONE+VCVTIP+VB}, 
  {0,0}, {0,0}, {0,0}, {0,0}},
  &rpsnull},


};

A jtcvz(J jt,I cv,A w){I t;
 t=AT(w);
 if(cv&VRD&&!(t&FL) ){
  // conversion to D is needed when we took the real/int/mag of a complex number and we want a FL result from that.
  // unfortunately it's hard to tell when that's what we did; rather than converting intolerantly (which would work but allocates
  // a block always), we scan here to see if all the imaginary parts are 0; if so, then we convert
  Z *wv=ZAV(w); DQ(AN(w), if((*wv).im!=0)R w; ++wv;)
  // imaginaries all 0, can demote to float
  R ccvt(FL,w,0);  // convert - must be possible
 }
 if(cv&VRI&&!(t&INT))R icvt(w);  // convert to integer if possible
 R w;
}    /* convert result */

#if 0 // for debug, to display info about a sparse block
if(ISSPARSE(AT(a))){
printf("va2a: shape="); A spt=a; DO(AR(spt), printf(" %d",AS(spt)[i]);) printf("\n");
printf("va2a: axes="); spt=SPA(PAV(spt),a); DO(AN(spt), printf(" %d",IAV(spt)[i]);) printf("\n"); 
printf("va2a: indexes="); spt=SPA(PAV(a),i); DO(AN(spt), printf(" %d",IAV(spt)[i]);) printf("\n");
}
#endif
// repair routines for integer overflow, possibly in place
static VF repairip[4] = {plusBIO, plusIIO, minusBIO, minusIIO};

// All dyadic arithmetic verbs f enter here, and also f"n.  a and w are the arguments, id
// is the pseudocharacter indicating what operation is to be performed.  self is the block for this primitive,
// allranks is (ranks of a and w),(verb ranks)
static A jtva2(J jtfg,AD * RESTRICT a,AD * RESTRICT w,AD * RESTRICT self,UI allranks){F12IP;  // allranks is argranks/ranks
 A z;I m,mf,n,nf,zn;VA2 adocv,*aadocv;UI fr;  // fr will eventually be frame/rank  nf (and mf) change roles during execution  fr/shortr use all bits and shift
 I aawwzknfxrz[10];  // a outer/only, a inner, w outer/only, w inner, z, n parm to ado, nf, nf wkarea, rc, offset to start of last z result
 {I at=AT(a);
  I wt=AT(w);
  if(likely(!(((I)jtfg&JTRETRY)+((at|wt)&((SPARSE|NOUN)&~(B01|INT|FL)))))){  // no error, bool/int/fl nonsparse args
   // Here for the fast and important case, where the arguments are both dense B01/INT/FL
   VA *vainfo=((VA*)((I)va+FAV(self)->localuse.lu1.uavandx[1]));  // extract table line from the primitive
   aadocv=&vainfo->p2[(at*3+(wt&INT+FL))>>INTX];   // test here to avoid the call overhead
  }else{

   // If we switch a sparse nonnumeric matrix to boolean, that may be a space problem; but we don't
   // support nonnumeric sparse now
   // if an operand is sparse, replace its type with the corresponding non-sparse type, for purposes of testing operand precisions
   if(unlikely(ISSPARSE(at|wt))){
    at&=~SPARSE; wt&=~SPARSE;
    jtfg=0;  // We use jtfg==0 as a flag meaning 'sparse'
    forcetomemory(aawwzknfxrz);  // make sure we don't try to keep these values in registers
   }
   // Get the result type and routine
   adocv=var(self,at,wt);
   if(unlikely(adocv.f==0)){
    // There is no routine for these argument types.  That's an error unless an argument is empty
    // If an operand is empty, turn it to Boolean, and if the OTHER operand is non-numeric, turn that to Boolean too (leaving
    //  rank and shape untouched).  This change to the other operand is notional only - we won't actually convert
    // when there is an empty - but it guarantees that execution on an empty never fails.
    at=((-AN(a)&(-AN(w)|-(at&NUMERIC)))>=0)?B01:at;
    wt=((-AN(w)&(-AN(a)|-(wt&NUMERIC)))>=0)?B01:wt;
    adocv=var(self,at,wt);   // rerun the decode with safer types
   }
   aadocv=&adocv;  // we save the address of the struct
   // We could allocate the result block here & avoid the test after the allocation later.  But we would have to check for agreement etc
   // Don't signal domain error on the types yet, because domain has lower priority than agreement
  }
 }

 // finish up the computation of sizes.  We have to defer this till after var() because
 // if we are retrying the operation, we may be in error state until var clears it; and prod and mult can fail,
 // so we have to RE when we call them

 // Analyze the rank and calculate cell shapes, counts, and sizes.
 // We detect agreement error before domain error
 {aawwzknfxrz[9]=0;  // init to stop outer loop after first iteration
  if(likely((allranks&RANK2TMSK)==0)){ // rank 0 0 means no outer frames, sets up faster
   fr=allranks>>(3*RANKTX); UI shortr=(allranks>>(2*RANKTX))&RANKTMSK;  // fr,shortr = ar,wr to begin with.  Changes later
   // No rank specified.  Since all these verbs have rank 0, that simplifies quite a bit.  ak/wk/zk are not needed and are garbage
   // n is not needed for sparse, but we start it early to get it finished
   if(likely(jtfg!=0)){  // nonsparse
    I an=AN(a); m=zn=AN(w);
    I raminusw=fr-shortr;   // ar-wr, neg if WISLONG
    zn=raminusw<0?zn:an; aawwzknfxrz[5]=m=raminusw<0?an:m;  // zn=# atoms in higher-rank operand, m=#atoms in lower-rank
    jtfg = (J)(((I)jtfg&3)+aadocv->cv+(raminusw&VIPWCRLONG));  // inplaceability plus routine flags, and VIPWCRLONG if a repeated (safely in the negative part of raminusw)
    mf=REPSGN(raminusw);  // mf=-1 if w has longer frame, means cannot inplace a
    raminusw=-raminusw;   // now wr-ar
    nf=REPSGN(raminusw);  // nf=-1 if a has longer frame, means cannot inplace w
    raminusw=raminusw&mf; fr+=raminusw; shortr-=raminusw;  // if ar is the longer one, change nothing; otherwise transfer aw-ar from shortr to r.  f (high part of fr) is 0
    PRODRNK(n,fr-shortr,AS((I)jtfg&VIPWCRLONG?w:a)+shortr);  // treat the entire operands as one big cell; get the rest of the values needed
   // notionally we now repurpose fr to be frame/rank, with the frame 0
    shortr=((I)jtfg>>VIPOKWX);  // only if supported by routine
    shortr=a==w?0:shortr;  // not if args equal
    // Non-sparse setup for copy loop, no rank
      // get number of inner cells
    nf= 3 + mf*2 + nf;  // set inplaceability here: only if nonrepeated cell
    nf&=shortr;  // we use shortr to shorten dependency chain on nf
    nf+=4*nf-16;  // make 2 copies of the 2 bits, bits 4+=1  This is a long dependency chain through nf
    jtfg = (J)((I)jtfg&nf);  // bit 2-3=routine/rank/arg inplaceable, 0-1=routine/rank/arg/input inplaceable
    // parm aawwzknfxrz[5] is orig m, i. e. the length of the inner or only loop.
    n=2*n-mf;  // parm m if there are 2 loops.  The value is 2 * (length of inner loop), with LSB set if x is the repeated value (i. e. w has long frame)
    m=~m;  // parm m if there is only 1 loop - the length of the loop, complemented as a flag.  The aawwzknfxrz[5] value is unused in this case
    m=n>3?n:m;  // if inner-loop len > 1, there are 2 loops, use mf; if inner-loop len=1, use the 1-loop value
   }else{
    // Sparse setup: move the block-local variables to longer-lived ones.  We are trying to reduce register pressure
    // repurpose ak/wk/mf/nf to hold acr/wcr/af/wf, which we will pass into vasp.  This allows acr/wcr/af/wf to be block-local
    aawwzknfxrz[0]=fr; aawwzknfxrz[1]=shortr; fr=shortr>fr?shortr:fr; mf=0; nf=0;
   }
  }else{I ak,wk;UI acr,wcr;  // fr, shortr are left/right verb rank here
   // Here, a rank was specified.  That means there must be a frame, according to the IRS rules
   {I af,wf;
    // Heavy register pressure here.  Seemingly trivial changes to allranks cause early spill of a in next-to-last PRODRNK
    // When this was written with 6 names it barely fit in registers, so I rewrote it with 4 names.  Compiler's ideas about handling allranks fill spare register
    acr=0; af=allranks>>2*RANKTX;  // allranks = anr/wnr/avr/wvr (nr=noun rank)  wcr will hold verbrank
    wcr=(RANK2T)allranks; af|=RANKTMSK; af-=wcr; af=((I)af<0)?acr:af; af&=~RANKTMSK;  // af is 0/0/anr/wnr -> 0/0/anr/ffff -> 0/0/afr/x -> clamp at 0 -> 0/0/afr/0
    wf=allranks>>=2*RANKTX; wcr&=RANKTMSK; wf&=RANKTMSK; wf-=wcr; wf=((I)wf<0)?acr:wf;    // wf is 0/0/anr/wnr -> 0/0/0/anr ->  0/0/0/afr -> clamp at 0
    acr=allranks; acr-=af; acr-=wf;  // acr = 0/0/acr/wcr  (nr-fr=cr)
    wcr=acr; wcr&=RANKTMSK; wf<<=RANKTX; wcr|=wf;   // wcr = wfr/wcr  final value
    acr>>=RANKTX; acr|=af;  // acr = afr/acr    final value
    // allranks is noun 0/0/anr/wnr
   }
   if(likely(jtfg!=0)){  // If not sparse... This block isn't needed for sparse arguments, and may fail on them.
    jtfg = (J)((I)jtfg&3);  // remove all but the inplacing bits
    jtfg = (J)((I)jtfg+aadocv->cv);  // insert flag bits for routine (always has bits 0-3=0xc); set bits 2-3 (converted inplacing bits)

    jtfg = (J)((I)jtfg+(((wcr+(acr^(((1LL<<(RANKTX-1))-1)*((1LL<<(RANKTX))+1)))) & (((1LL<<(RANKTX-1)))+((1LL<<(2*RANKTX-1))))) <<(VIPWCRLONGX-(RANKTX-1))));  // set flag for 'w has longer cell-rank' (VIPWCRLONG) and 'w has longer frame (wrt verb)' (VIPWFLONG)
    wcr+=acr<<2*RANKTX;  // afr/acr/wfr/wcr

    PRODRNK(ak,acr, AS(a)+(wcr>>(3*RANKTX))); PRODRNK(wk,wcr,AS(w)+((wcr>>RANKTX)&RANKTMSK));   // left/right #atoms/cell  length is assigned first
       // note: the prod above can never fail, because it gives the actual # cells of an existing noun  acr free
    // m=#atoms in cell with shorter rank; n=#times shorter-rank cells must be repeated; r=larger of cell-ranks
    // fr has the longer cell-rank
    // if looping required, calculate the strides for input & output.  Needed only if mf or nf>1, but not worth testing, since presumably one will, else why use rank?
    // zk=result-cell size in bytes; ak,wk=left,right arg-cell size in bytes.  Not needed if not looping
    // bits 0-1 of jtfg are combined input+local; 2-3 just local; 4+ hold adocv.cv; sign set if ak==0. output type is always set to show non-sparse
    // 0-1 are set if operand is inplaceable according to prim & input inplaceability; 2-3 from prim only.  We use 0-1 unless we convert; then we use 2-3
     // jtfg VIPWFLONG set if wf>af, bit VIPWCRLONG set if wcr>acr
    zn=(I)jtfg&VIPWCRLONG?wk:ak;    // zn=#atoms in cell with larger rank
    m=(I)jtfg&VIPWCRLONG?ak:wk;  // m=#atoms in common inner cell, i. e. the smaller
    I at=AT(a); I wt=AT(w); I cvt=rtype((I)jtfg);  // if rtype=0, we use the arg type of w
    aawwzknfxrz[4]=zn<<bplg(cvt!=0?cvt:(I)jtfg&VCOPYA?at:wt);  // calc result-cell size and move it out of registers
    ak<<=bplg(at); wk<<=bplg(wt);  // convert cell sizes to bytes
    aawwzknfxrz[0]=ak; aawwzknfxrz[2]=wk; ak=((I)jtfg&VIPWFLONG)?0:ak; wk=((I)jtfg&VIPWFLONG)?wk:0; aawwzknfxrz[1]=ak; aawwzknfxrz[3]=wk;  // set inner cell size for last followed by non-last.  Last is 0 for a repeated cell ak/wk free
    I shortr=wcr>>(((I)jtfg>>(VIPWCRLONGX-LGRANK2TX))&(VIPWCRLONG>>(VIPWCRLONGX-LGRANK2TX))); fr=wcr>>((((I)jtfg>>(VIPWCRLONGX-LGRANK2TX))&(VIPWCRLONG>>(VIPWCRLONGX-LGRANK2TX)))^RANK2TX); // shortr=frame(short cell)/cellrank(short cell)  fr=frame(long cell)/cellrank(long cell)
    shortr&=RANKTMSK; fr&=RANK2TMSK; // cellrank(short cell)
    shortr*=((I)1<<2*RANKTX)+((I)1<<RANKTX)-1;   //   cellrank(short cell)/cellrank(short cell)/-cellrank(short cell)  100000000+10000+ffffffffffffffff
    shortr+=fr;  // cellrank(short cell)/frame(long cell)+cellrank(short cell)/cellrank(long cell)-cellrank(short cell)
                 //  length for agreement / offset to excess frame, for calc n  / length for calc n,(# intracell repeats) - final value
    // fr is now frame/rank of long cell
    // fr will be (frame(long cell))  /  (shorter frame len)   /  (longer frame len)                      /   (longer frame len+longer celllen)
    //  (offset to store cellshape to)  / for #outer cells mf  / length of frame to copy, also to calc nf / ranks that = this have no repeats, can inplace (also used to figure cellen for shape copy)
#ifdef PEXT
    UI f=PEXT(wcr,RANKTMSK*((1LL<<RANKTX)+(1LL<<3*RANKTX)));   // 0/0/aframe/wframe; 
#else
    UI f=wcr>>RANKTX; f&=~(RANKTMSK<<RANKTX); f=(RANK2T)(f+(f>>RANKTX)); // 0/0/aframe/wframe; 
#endif
    f+=f<<2*RANKTX;   //  aframe/wframe/aframe/wframe
    f>>=((I)jtfg&VIPWFLONG)>>(VIPWFLONGX-LGRANKTX);  // shift by 0/8 (8 if w has long frame) to give x/x/longframe/shortframe
    f&=RANKTMSK*(1+(1LL<<RANKTX)); f=(f<<2*RANKTX)+(f>>RANKTX);  // longframe/shortframe/0/longframe
#if SY_64
    f+=wcr<<4*RANKTX;  // afr/acr/wfr/wcr/long frame/short frame/0/long frame   wcr free
#define acrwcr (fr>>4*RANKTX)  // put frames into fr to save a register
#else
#define acrwcr wcr
#endif
    fr+=f;    //   fr=afr/acr/wfr/wcr/longframe/shortframe/frame(long cell)/longframe+cellrank(long cell)
    f=fr&RANKTMSK; allranks|=(1LL<<(RANKTX-1))+(1LL<<(2*RANKTX-1)); allranks-=f; f<<=RANKTX; allranks-=f;  // set sign bit of rank if = long frame + long cell (can't be any bigger) f free
    ASSERTAGREE(AS(a)+(acrwcr>>3*RANKTX), AS(w)+(((RANK2T)acrwcr>>RANKTX)), (shortr>>2*RANKTX))  // offset to each cellshape, and cellrank(short cell) acr wcr free but were actually spilled earlier
    PRODRNK(n,shortr,AS((I)jtfg&VIPWCRLONG?w:a)+((RANK2T)shortr>>RANKTX));  // n is #atoms in excess frame of inner cells, length assigned first shortr free
    // if the cell-shapes don't match, that's an agreement error UNLESS the frame contains 0; in that case it counts as
    // 'error executing on the cell of fills' and produces a scalar 0 as the result for that cell, which we handle by changing the result-cell rank to 0
    // Nonce: continue giving the error even when frame contains 0 - remove 1|| in the next line to conform to fill-cell rules
// this shows the fix   if(ICMP(as+af,ws+wf,MIN(acr,wcr))){if(1||zn)ASSERT(0,EVLENGTH)else r = 0;}
#ifdef PEXT
    nf=((I)jtfg>>VIPOKWX)&PEXT(allranks,(1LL<<(RANKTX-1))+(1LL<<(2*RANKTX-1)));  // extract inplaceability from ranks   allranks free
#else
    nf=((I)jtfg>>VIPOKWX)&(((allranks>>(2*RANKTX-1-1))&2)+((allranks>>(RANKTX-1))&1));  // extract inplaceability from ranks   allranks free
#endif
    nf=a==w?0:nf;  // not inplaceable if args identical
    nf+=4*nf-16;  // make 2 copies of the 2 bits protect high bits of jtfg.  This is a long dependency chain through nf but it will overlap the PRODs coming up
    jtfg = (J)((I)jtfg&nf);  // bit 2-3=routine/rank/arg inplaceable, 0-1=routine/rank/arg/input inplaceable   nf free
    f=fr>>(2*RANKTX); f&=RANKTMSK;  // recover (shorter frame len) from upper fr
    PRODRNK(nf,((fr>>3*RANKTX)-f),f+AS(((I)jtfg&VIPWFLONG)?w:a));    // nf=#times shorter-frame cell must be repeated;  offset is (shorter frame len), i. e. loc of excess frame
         // length is (longer frame len)-(shorter frame len)  i. e. length of excess frame
    PRODRNK(mf,f,AS(w));  //  mf=#cells in common frame [either arg ok]   f is (shorter frame len)      f free now
    
    // Now nf=outer repeated frame  mf=outer common frame  n=inner repeated frame  m=inner common frame
    //    leading axes --------------------------------------------------------------> trailing axes
    // loop migration: if the outer loop can be subsumed into the inner loop do that to make the faster inner loops more effective
    // the cases we see are:
    // 1: m=1 and n=1: move mf->m, nf->n, and complement n if ak==0 (i. e. x arg is repeated) example:  list *"0 _ atom
    // 2: n=1 and nf=1: multiply m by mf, leave n  example: (shape 4 5) *"1 (shape 4 5 or 1 4 5)
    // 3: m=1 and nf=1: multiply m by mf, leave n  example: (shape 4 5) *"1 0 (shape 5)
    I migrmf=mf;
    DPMULDE(nf,mf,mf);  // mf is total # iterations
    DPMULDE(zn,mf,zn)  // zn is total # atoms in result
    I flagorign=((I)jtfg>>VIPWCRLONGX)&(n!=1);  // if n was not 1 before migration, it must be flagged if WCRLONG is set; in this case nf must be 1 and there is no further flagging
    if(unlikely(((C)(nf!=1)+(C)(n!=1)-(C)(m==1))<=(C)0)){  // 2 values=1, can lose a loop
     // migration is possible
     flagorign|=((I)jtfg>>VIPWFLONGX)&(nf!=1);  // repetition also comes if nf is not 1 and WFLONG.  In this case n must be 1 & thus nop flag set yet
     m*=migrmf; n*=nf;   // propagate mf and nf down
    }else{aawwzknfxrz[6]=--nf; aawwzknfxrz[9]=(mf-1)*aawwzknfxrz[4];}     // All 4 loops (normal case since rank given).  nf is outer loop repeat count-1.  zend ([9]) is offset to result of last iteration
    // Use new semantics for m and n
    aawwzknfxrz[5]=m;  // parm n is orig m, i. e. the length of the inner or only loop.
    n=2*n+flagorign;  // parm m if there are 2 loops.  The value is 2 * (length of inner loop), with LSB set if x is the repeated value (i. e. w has long frame)
    m=~m;  // parm m if there is only 1 loop - the length of the loop, complemented as a flag.  The aawwzknfxrz[5] value is unused in this case
    m=n>3?n:m;  // if inner-loop len > 1, there are 2 loops, use mf; if inner-loop len=1, use the 1-loop value
   }else{  // sparse case
    I af=acr>>(RANKTX), wf=wcr>>(RANKTX); acr&=RANKTMSK; wcr&=RANKTMSK;   // separate cr and f for sparse
    fr=acr<wcr?wcr:acr; I f=(af<wf)?wf:af; fr+=(f<<RANKTX)+f; aawwzknfxrz[0]=acr; aawwzknfxrz[1]=wcr; mf=af; nf=wf;
    // For sparse, repurpose aawwzknfxrz/mf/nf to hold acr/wcr/af/wf, which we will pass into vasp.  This allows acr/wcr/af/wf to be block-local
    // Note: this code passed the test suite even when fr was garbage
   }
  }
 }

 RESETRANK;  // Ranks are required for sparse, which calls IRS-enabled routines internally.  We clear in case the action routine calls a function with IRS

 // Signal domain error if appropriate. Must do this after agreement tests
 ASSERT(aadocv->f,EVDOMAIN);
 if(likely(jtfg!=0)){   // if not sparse...
  // Not sparse.

  // If op specifies forced input conversion AND if both arguments are non-sparse: convert them to the selected type.
  // Incompatible arguments were detected in var().  If there is an empty operand, skip conversions which
  // might fail because the type in t is incompatible with the actual type in a.  t is rare.
  //
  // Because of the priority of errors we mustn't check the type until we have verified agreement above
  if(unlikely(((I)jtfg&VARGMSK)!=0))if(likely(zn>0)){  // input conversion required (rare), and the result is not empty
   I at=AT(a), wt=AT(w), t=atype((I)jtfg); t=(I)jtfg&VCOPYA?at:t; t=(I)jtfg&VCOPYW?wt:t; I bt=bplg(t);  // get shared input type, which might be from one of the inputs
   // Conversions to XNUM use a routine that pushes/sets/pops jt->mode, which controls the
   // type of conversion to XNUM in use.  Any result of the conversion is automatically inplaceable.  If type changes, change the cell-size too, possibly larger or smaller
   // bits 2-3 of jtfg indicate whether inplaceability is allowed by the op, the ranks, and the addresses
   if(TYPESNE(t,at)){I ba=bplg(at); aawwzknfxrz[0]=(aawwzknfxrz[0]>>ba)<<bt; aawwzknfxrz[1]=(aawwzknfxrz[1]>>ba)<<bt; RZ(a=cvt(t|((I)jtfg&XCVTXNUMORIDEMSK),a)); jtfg = (J)(intptr_t)((I)jtfg | (((I)jtfg>>2)&JTINPLACEA));}
   if(TYPESNE(t,wt)){I bw=bplg(wt); aawwzknfxrz[2]=(aawwzknfxrz[2]>>bw)<<bt; aawwzknfxrz[3]=(aawwzknfxrz[3]>>bw)<<bt; RZ(w=cvt(t|((I)jtfg&XCVTXNUMORIDEMSK),w)); jtfg = (J)(intptr_t)((I)jtfg | (((I)jtfg>>2)&JTINPLACEW));}
  }

  // From here on we have possibly changed the address of a and w

  // Allocate a result area of the right type, and copy in its cell-shape after the frame
  // If an argument can be overwritten, use it rather than allocating a new one
  // Argument can be overwritten if: action routine allows it; flagged in jtfg; usecount 1 or zombie; rank equals (length of longer frame)+(length of longer cell)
  // If the argument has rank that large, and the arguments agree, the argument MUST have the same number of atoms as the result, because all shape is accounted for.
  // rank = rank of result (the rank of the result is the sum of (the longer frame-length) plus (the larger cell-rank))
  // Also, if the operation is one that may abort, we suppress inplacing it if the user can't handle early assignment.
  // Finally, if a==w suppress inplacing, in case the operation must be retried (we could check which ones but they are
  // just not likely to be used reflexively)
  I ipw=ASGNINPLACENEG(SGNIF(jtfg,JTINPLACEWX),w), ipa=ASGNINPLACENEG(SGNIF(jtfg,JTINPLACEAX),a), zt=rtype((I)jtfg);  // get type of result; is w inplaceable?
  if((ipw|ipa)<0){  // see if either w or a is inplaceable
   // we are reusing an argument (ipw is neg if it's w, which has priority); make sure the type is updated to the result type
   z=ipw<0?w:a;  // z=inplaceable arg; 
   if(unlikely((zt&~AT(z))!=0)){   // if type changes (zt!=incumbent and zt!=0)...  zt does not have upper flag bits
    // the type of inplaceable z must change.  But if z is UNINCORPABLE, it might be virtual.  Realizing it is a losing move.  And, we don't change the type of an UNINCORPABLE so that the caller
    // that created it doesn't have to keep reinitializing the type.  So, we give up on inplacing it.  If both args are inplaceable, we try a (which might have the right type).  If neither works, we allocate
    if(AFLAG(z)&AFUNINCORPABLE){
     if((ipa&((zt&~AT(a))-1))>=0)goto allocate;  // unless a is inplaceable and does not require a new type, go GA the result area
        // we could use a even if it changes type, if it is not UNINCORPABLE.  But if w is UNINCORPABLE and a is inplaceable, it's surely because a is an unrepeated UNINCORPABLE cell in dyad u"n - not worth checking
     z=a;  // we can use a as is, do so
    }else AT(z)=zt;  // OK to change type of z to match the result, do so
   }
  }else{
allocate:;  // come here if no inplaceable block could have the type changed
   I wt=AT(w); zt=zt?zt:wt; GA00(z,zt,zn,(RANKT)fr);   // get type and allocate result area (zn survives the call)
   if(unlikely(zt&CMPX+QP))AK(z)=(AK(z)+SZD)&~SZD;  // move 16-byte values to 16-byte bdy
#define scell AS((I)jtfg&VIPWCRLONG?w:a)+((RANK2T)fr>>RANKTX)  // address of start of cell shape     shape of long cell+frame(long cell)
    // fr is (frame(long cell))  /  (shorter frame len)   /  (longer frame len)                      /   (longer frame len+longer celllen)
   MCISH(AS(z),AS(((I)jtfg&VIPWFLONG)?w:a),(RANK4T)fr>>3*RANKTX); MCISH(AS(z)+((RANK4T)fr>>3*RANKTX),scell,(fr&RANKTMSK)-((RANK4T)fr>>3*RANKTX));  // copy shape
//                                     frame loc     shape of long frame             len of long frame  cellshape       longer cellen 
  } 
  // fr free
  if(unlikely(zn==0)){RETF(z);}  // If the result is empty, the allocated area says it all
  VF adocvfn=aadocv->f;  // extract the function address.  This frees aadocv, and in clang16 brings adocvfn into register early to speed the expected misprediction
  // zn, zt  NOT USED FROM HERE ON

  // End of setup phase.  The execution phase:

  // The compiler thinks that because ak/wk/zk are used in the loop they should reside in registers.  So we force the compiler to spill aawwzknfxrz using forcetomemory.
  // We want m, mend, av, wv, zv, jj to be in registers  (m and zv so that the action routine can test them and do boundary alignment right away; mend so that the misprediction
  // of the last loop, if it happens, will be detected right away).  It might be better to have n (aawwzknfxrz[9]) in a register as well.
  {C *zv=CAV(z); C *av=CAV(a); C *wv=CAV(w);   // point to the data.  Get zv settled first because it's tested for boundary in the action routine.  Could move up but gain is small
   aawwzknfxrz[8]=(I)jtfg&VRMSK;  // init good composite rc, and transfer output conversion to it, freeing up inplace (we know that anything needing conversion will not need retries)
   I mulofloloc;   // number of good results before we encountered integer overflow on multiply
   {
    I mend=aawwzknfxrz[9]+(I)zv;   // add addr to offset to get addr of last block of z
    // Call the action routines:
    // aawwzknfxrz[6] is original nf-1, the number of inner outer loops; but IMIN if there are no outer loops.  We do this so that executions with no rank will ratify the branch immediately, reducing misbranch
    // overhead if there is any.  If there are outer loops, jj counts the inners and aawwzknfxrz[9] has the starting offset of the last inner loop of all.  aawwzknfxrz[0,1] are the cell-size of a for the outer loop, aawwzknfxrz[2,3] are for w;
    // but aawwzknfxrz[1,3] have 0 in a repeated argument.  aawwzknfxrz[1,3] are added for each inner iteration, aawwzk[0,2] at the end of an inner cycle
    // m is the length of the inner loop, with flags: complement=single loop of length ~m, otherwise each loop has length m>>1, and LSB of m is set if a atom is repeated
    // aawwzknfxrz[5] is the number of outer inner loops, used only if m>0.  n*m cannot=0. 
    // Each release, monitor that clang brings adocvfn into register early to advance the expected misprediction.
    I jj=aawwzknfxrz[6];  // number of outer-outer loops, number-1 of each outer-inner loop
    lp000: ;
    {I lrc=((AHDR2FN*)adocvfn)AH2A(aawwzknfxrz[5],m,av,wv,zv,jt);    // run one section.  Result is EOK normally, otherwise error code, as examined below
     if(unlikely(lrc!=EVOK)){   // scaf can make EVOK=0
      // section did not complete normally.
      if(unlikely(lrc<0)){mulofloloc=((zv-CAV(z))>>LGSZI)+~lrc; aawwzknfxrz[8]=EWOVIP+EWOVIPMULII; goto lp000e;}  // integer multiply overflow.  ~lrc is index of failing location; create global failure index.  Abort the computation to retry
      if(lrc<aawwzknfxrz[8])aawwzknfxrz[8]=lrc;   // set rc to worst error found so far
      if(lrc<EWOVIP)goto lp000e;  // error not recoverable in-place.  fail or retry, but no reason to continue loop
      // here error is correctable in place.  Continue loop
     }
     if((I)zv==mend)goto lp000e; aawwzknfxrz[7]=--jj; jj=REPSGN(jj); zv+=aawwzknfxrz[4]; av+=aawwzknfxrz[1+jj]; wv+=aawwzknfxrz[3+jj]; jj=aawwzknfxrz[7+jj];  goto lp000; // jj is -1 on the last inner iter, where we use outer incr
    }
    lp000e: ;
   }

   // The work has been done.  If there was no error, check for optional conversion-if-possible or -if-necessary
   if(likely(aawwzknfxrz[8]==VRNONE)){RETF(z) // normal return is here.  If NOCONV or error happened we lost VRI+VRD+VRNONE.
   }else if(aawwzknfxrz[8]&VRI+VRD){RETF(cvz(aawwzknfxrz[8],z))   // if result conversion still required, do it
   }else if(aawwzknfxrz[8]&EVNOCONV){RETF(z)   // if no true error (but only suppression of conversion), OK (extremely rare)

   // ********* error recovery starts here **********
   }else if(aawwzknfxrz[8]-EWOVIP>=0){A zz;C *zzv;I zzk;
    // Here for overflow that can be corrected in place.  The routines use the old semantics for m and n, so we convert them back
    n=(m>>1)^-(m&1); n=m<0?1:n;  // original n is 1 if m is complementary; otherwise m>>1, complemented if x is repeated
    m=aawwzknfxrz[5];  // original m
// not yet    if(rc==EVOKCLEANUP){jt->mulofloloc=0; RETF(z);}  // if multiply that did not overflow, clear the oflo position for next time, and return
    // If the original result block cannot hold the D result, allocate a separate result area
    if(sizeof(D)==sizeof(I)){zz=z; MODBLOCKTYPE(zz,FL); zzk=aawwzknfxrz[4];   // shape etc are already OK
    }else{GATV(zz,FL,AN(z),AR(z),AS(z)); zzk=aawwzknfxrz[4]*(sizeof(D)/sizeof(I));}
    // restore pointers to beginning of arguments
    zzv=CAV(zz);  // point to new-result data
    // Set up pointers etc for the overflow handling.  Set b=1 if w is taken for the x argument to repair
    if(aawwzknfxrz[8]==EWOVIP+EWOVIPMULII){D *zzvd=(D*)zzv; I *zvi=IAV(z);
     // Integer-multiply repair.  We have to convert all the pre-overflow results to float, and then finish the multiplies
     DQ(mulofloloc, *zzvd++=(D)*zvi++;);  // convert the multiply results to float.  mulofloloc is known negative, and must be complemented
     // Now repeat the processing.  Unlike with add/subtract overflow, we have to match up all the argument atoms
     {C *av=CAV(a); C *wv=CAV(w);
      I jj=aawwzknfxrz[6]; I mend=aawwzknfxrz[9]*(SZD/SZI)+(I)zzv;  // extract offset to end buffer, convert it to D length, relocate to new output area
      while(1){
       tymesIIO(n,m,(I*)av,(I*)wv,(D*)zzv,mulofloloc);
       if((I)zzv==mend)break; mulofloloc-=m*(n^REPSGN(n)); aawwzknfxrz[7]=--jj; jj=REPSGN(jj); zzv+=zzk; av+=aawwzknfxrz[1+jj]; wv+=aawwzknfxrz[3+jj]; jj=aawwzknfxrz[7+jj];  // jj1 is -1 on the last inner iter, where we use outer incr
      }
     }
    } else {   // not multiply repair, but something else to do inplace
     AHDR2FN *repairfn = (AHDR2FN*)repairip[(aawwzknfxrz[8]-EWOVIP)&3];   // fetch ep from table
     I nipw = ((z!=w) & (aawwzknfxrz[8]-EWOVIP)) ^ (((aawwzknfxrz[8]-EWOVIP)>>2) & 1);  // nipw from z!=w if bits2,0==01; 1 if 10; 0 if 00
     // nipw means 'use w as not-in-place'; c means 'repeat cells of a'; so if nipw!=c we repeat cells of not-in-place, if nipw==c we set nf to 1
     // if we are repeating cells of the not-in-place, we leave the repetition count in nf, otherwise subsume it in mf
     // b means 'repeat atoms inside a'; so if nipw!=b we repeat atoms of not-in-place, if nipw==b we set n to 1
     {C *av, *zv=CAV(z);
      // zv and zzv process without repeats; they contain all the information for the in-place argument (if any).
      // av may have repeats.  Repeats before the function call are handled exactly as the first time through, by using aawwzknfxrz.
      // repeats inside the function call (from n) must appear only on a, i. e. n<0 to repeat a, or n==1 for no repeat
      // if the original internal repeat was on the argument that is now z (e. g. nipw is set, meaning original a is going to z, and n is negative, meaning repeat inside a)
      // we must multiply out the repeat to leave n=1.
      av=CAV(nipw?w:a);  // point to the not-in-place argument
      I nsgn=SGNTO0(n); n^=REPSGN(n); if(nipw==nsgn){m*=n; n=1;} n^=-nipw;  // force n to <=1; make n flag indicate whether args were switched
      I jj=aawwzknfxrz[6]; I mend=aawwzknfxrz[9]+(I)zv;
      NOUNROLL while(1){(repairfn)AH2A(n,m,av,zv,zzv,jt); if((I)zv==mend)break; aawwzknfxrz[7]=--jj; jj=REPSGN(jj); zv+=aawwzknfxrz[4]; zzv+=zzk; av+=aawwzknfxrz[2*nipw+1+jj]; jj=aawwzknfxrz[7+jj];}  // jj1 is -1 on the last inner iter, where we use outer incr
     }
    }
    R zz;  // Return the result after overflow has been corrected
   }
   // retry required, not inplaceable.  Signal the error code to the caller.  If the error is not retryable, set the error message.
   // The caller will call again with the error set, which will change our selection of processing routines
   if(aawwzknfxrz[8]<=NEVM)jsignal(aawwzknfxrz[8]);else jt->jerr=(UC)aawwzknfxrz[8];
  }

 // sparse processing:
 }else{z=vasp(a,w,FAV(self)->id,aadocv->f,aadocv->cv,atype(aadocv->cv),rtype(aadocv->cv),mf,aawwzknfxrz[0],nf,aawwzknfxrz[1],fr>>RANKTX,(RANKT)fr-(fr>>RANKTX)); if(!jt->jerr)R z;}  // handle sparse arrays separately.
 R 0;  // return to the caller, who will retry any retryable errors
}    /* scalar fn primitive and f"r main control */


// 4-nested loop for dot-products.  Handles repeats for inner and outer frame.  oneprod is the code for calculating a single vector inner product *zv++ = *av++ dot *wv++
// If there is inner frame, it is the a arg that is repeated
// LIT is set in it if it is OK to use 2x2 operations (viz a has no inner frame & w has no outer frame)
#define SUMATLOOP2(ti,to,oneprod2,oneprod1) \
  {ti * RESTRICT av=avp,* RESTRICT wv=wvp; to * RESTRICT zv=zvp; \
   __m256i endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-dplen)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
   __m256d acc000; __m256d acc010; __m256d acc100; __m256d acc110; \
   __m256d acc001; __m256d acc011; __m256d acc101; __m256d acc111; \
   DQ(nfro, I jj=nfri; ti *ov0=it&BOX?av:wv; \
    while(1){  \
     DQ(ndpo, I j=ndpi; ti *av0=av; /* i is how many a's are left, j is how many w's*/ \
      while(1){ \
       if(it&LIT&&jj>1){ \
        ti * RESTRICT wv1=wv+dplen; wv1=j==1?wv:wv1; \
        oneprod2  \
        if(j>1){--j; _mm_storeu_pd(zv,_mm256_castpd256_pd128 (acc000)); _mm_storeu_pd(zv+ndpi,_mm256_castpd256_pd128 (acc100)); wv+=dplen; zv +=2;} \
        else{*(I*)zv=_mm256_extract_epi64(_mm256_castpd_si256(acc000),0x0); *(I*)(zv+ndpi)=_mm256_extract_epi64(_mm256_castpd_si256(acc100),0x0); /* AVX2 *zv=_mm256_cvtsd_f64(acc000); *(zv+ndpi)=_mm256_cvtsd_f64(acc100); */  zv+=1;} \
       }else{ \
        oneprod1  \
        *(I*)zv=_mm256_extract_epi64(_mm256_castpd_si256(acc000),0x0); /* AVX2 *zv=_mm256_cvtsd_f64(acc000); */ \
        zv+=1; \
       } \
       if(!--j)break; \
       av=av0;  \
      } \
      if(it&LIT&&jj>1){--i; av+=dplen; zv+=ndpi;} \
     ) \
     if((jj-=(((it>>LITX)&(LIT>>LITX))+1))<=0)break; \
     if(it&BOX)av=ov0;else wv=ov0; \
    } \
   ) \
  }

#define SUMATLOOP(ti,to,oneprod) \
  {ti * RESTRICT av=avp,* RESTRICT wv=wvp; to * RESTRICT zv=zvp; \
   DQ(nfro, I jj=nfri; ti *ov0=it&BOX?av:wv; \
    while(1){  \
     DQ(ndpo, I j=ndpi; ti *av0=av; /* i is how many a's are left, j is how many w's*/ \
      while(1){oneprod if(!--j)break; av=av0;} \
     ) \
     if(!--jj)break; \
     if(it&BOX)av=ov0;else wv=ov0; } \
   ) \
  }

#if C_AVX2 || EMU_AVX2

// Do one 2x2 product of length dplen.  Leave results in accxx0.  dplen must be >0
// av, wv, wv1 are set up.  Special branch for case of len<=4
#define ONEPRODAVXD2(label,mid2x2,last2x2) {\
   acc000=_mm256_setzero_pd(); acc010=acc000; acc100=acc000; acc110=acc000; \
   if(dplen<=NPAR)goto label##9; \
   acc001=acc000; acc011=acc000; acc101=acc000; acc111=acc000; \
   UI n2=DUFFLPCT((dplen)-1,3);  /* # turns through duff loop */ \
   if(n2>0){ \
    I backoff=DUFFBACKOFF((dplen)-1,3); \
    av+=(backoff+1)*NPAR; wv+=(backoff+1)*NPAR; wv1+=(backoff+1)*NPAR; \
    switch(backoff){ \
    do{ \
    case -1: mid2x2(0,0)  case -2: mid2x2(1,1)  case -3: mid2x2(2,0)  case -4: mid2x2(3,1)  \
    case -5: mid2x2(4,1)  case -6: mid2x2(5,1)  case -7: mid2x2(6,0)  case -8: mid2x2(7,1)  \
    av+=8*NPAR; wv+=8*NPAR; wv1+=8*NPAR; \
    }while(--n2!=0); \
    } \
   } \
   acc000=_mm256_add_pd(acc000,acc001); acc010=_mm256_add_pd(acc010,acc011); acc100=_mm256_add_pd(acc100,acc101); acc110=_mm256_add_pd(acc110,acc111);  \
   label##9: last2x2  \
   acc000=_mm256_add_pd(acc000,_mm256_permute4x64_pd(acc000,0b11111110)); acc010=_mm256_add_pd(acc010,_mm256_permute4x64_pd(acc010,0b11111110)); \
    acc100=_mm256_add_pd(acc100,_mm256_permute4x64_pd(acc100,0b11111110)); acc110=_mm256_add_pd(acc110,_mm256_permute4x64_pd(acc110,0b11111110)); \
   acc000=_mm256_add_pd(acc000,_mm256_permute_pd (acc000,0xf)); acc010=_mm256_add_pd(acc010,_mm256_permute_pd (acc010,0x0));  \
    acc100=_mm256_add_pd(acc100,_mm256_permute_pd (acc100,0xf)); acc110=_mm256_add_pd(acc110,_mm256_permute_pd (acc110,0x0)); \
   acc000=_mm256_blend_pd(acc000,acc010,0xa); acc100=_mm256_blend_pd(acc100,acc110,0xa); \
   av+=((dplen-1)&(NPAR-1))+1;  wv+=((dplen-1)&(NPAR-1))+1; \
   }

// do one 2x2, 4 combinations from offset using accumulator accno.
// av, wv, and wv1 are set
#define CELL2X2M(offset,accno) \
 acc00##accno = MUL_ACC(acc00##accno, _mm256_loadu_pd(&av[offset*NPAR]), _mm256_loadu_pd(&wv[offset*NPAR])); \
 acc01##accno = MUL_ACC(acc01##accno, _mm256_loadu_pd(&av[offset*NPAR]), _mm256_loadu_pd(&wv1[offset*NPAR])); \
 acc10##accno = MUL_ACC(acc10##accno, _mm256_loadu_pd(&av[dplen+offset*NPAR]), _mm256_loadu_pd(&wv[offset*NPAR])); \
 acc11##accno = MUL_ACC(acc11##accno, _mm256_loadu_pd(&av[dplen+offset*NPAR]), _mm256_loadu_pd(&wv1[offset*NPAR]));

// same but with mask, on cell number 0
#define CELL2X2L \
 acc000 = MUL_ACC(acc000, _mm256_maskload_pd(&av[0],endmask), _mm256_maskload_pd(&wv[0],endmask)); \
 acc010 = MUL_ACC(acc010, _mm256_maskload_pd(&av[0],endmask), _mm256_maskload_pd(&wv1[0],endmask)); \
 acc100 = MUL_ACC(acc100, _mm256_maskload_pd(&av[dplen+0],endmask), _mm256_maskload_pd(&wv[0],endmask)); \
 acc110 = MUL_ACC(acc110, _mm256_maskload_pd(&av[dplen+0],endmask), _mm256_maskload_pd(&wv1[0],endmask));

// Do one 1x1 product of length dplen.  Leave results in acc000.  dplen must be >0
// av,  wv, are set up.  We do a quick check for short arg, since 3-long is a common case
#define ONEPRODAVXD1(label,mid1x1,last1x1) {\
   acc000=_mm256_setzero_pd(); if(dplen<=NPAR)goto label##9; \
   acc010=acc000; acc100=acc000; acc110=acc000; \
   acc001=acc000; acc011=acc000; acc101=acc000; acc111=acc000; \
   UI n2=DUFFLPCT((dplen)-1,3);  /* # turns through duff loop */ \
   if(n2>0){ \
    I backoff=DUFFBACKOFF((dplen)-1,3); \
    av+=(backoff+1)*NPAR; wv+=(backoff+1)*NPAR; \
    switch(backoff){ \
    do{ \
    case -1: mid1x1(0,000)  case -2: mid1x1(1,001)  case -3: mid1x1(2,010)  case -4: mid1x1(3,011)  \
    case -5: mid1x1(4,100)  case -6: mid1x1(5,101)  case -7: mid1x1(6,110)  case -8: mid1x1(7,111)  \
    av+=8*NPAR; wv+=8*NPAR; \
    }while(--n2!=0); \
    } \
   } \
   acc000=_mm256_add_pd(acc000,acc001); acc010=_mm256_add_pd(acc010,acc011); acc100=_mm256_add_pd(acc100,acc101); acc110=_mm256_add_pd(acc110,acc111);  \
   acc000=_mm256_add_pd(acc000,acc010); acc100=_mm256_add_pd(acc100,acc110); \
   acc000=_mm256_add_pd(acc000,acc100);  \
   label##9: last1x1  \
   acc000=_mm256_add_pd(acc000,_mm256_permute4x64_pd(acc000,0b11111110)); \
   acc000=_mm256_add_pd(acc000,_mm256_permute_pd(acc000,0xf)); \
   av+=((dplen-1)&(NPAR-1))+1;  wv+=((dplen-1)&(NPAR-1))+1; \
   }

// do one 1x1, using accumulator accno.
// av, wv are set
#define CELL1X1M(offset,accno) \
 acc##accno = MUL_ACC(acc##accno, _mm256_loadu_pd(&av[offset*NPAR]), _mm256_loadu_pd(&wv[offset*NPAR]));

// same but with mask, on cell number 0
#define CELL1X1L \
 acc000 = MUL_ACC(acc000, _mm256_maskload_pd(&av[0],endmask), _mm256_maskload_pd(&wv[0],endmask));

#define ONEPRODD \
 __m256i endmask; /* length mask for the last word */ \
 /* +/ vectors */ \
 __m256d idreg=_mm256_setzero_pd(); \
 endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-dplen)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
 __m256d acc0=idreg; __m256d acc1=idreg; __m256d acc2=idreg; __m256d acc3=idreg; \
 UI n2=DUFFLPCT((dplen)-1,3);  /* # turns through duff loop */ \
 if(n2>0){ \
  I backoff=DUFFBACKOFF((dplen)-1,3); \
  av+=(backoff+1)*NPAR; wv+=(backoff+1)*NPAR; \
  switch(backoff){ \
  do{ \
  case -1: acc0=MUL_ACC(acc0,_mm256_loadu_pd(av),_mm256_loadu_pd(wv)); \
  case -2: acc1=MUL_ACC(acc1,_mm256_loadu_pd(av+1*NPAR),_mm256_loadu_pd(wv+1*NPAR)); \
  case -3: acc2=MUL_ACC(acc2,_mm256_loadu_pd(av+2*NPAR),_mm256_loadu_pd(wv+2*NPAR)); \
  case -4: acc3=MUL_ACC(acc3,_mm256_loadu_pd(av+3*NPAR),_mm256_loadu_pd(wv+3*NPAR)); \
  case -5: acc0=MUL_ACC(acc0,_mm256_loadu_pd(av+4*NPAR),_mm256_loadu_pd(wv+4*NPAR)); \
  case -6: acc1=MUL_ACC(acc1,_mm256_loadu_pd(av+5*NPAR),_mm256_loadu_pd(wv+5*NPAR)); \
  case -7: acc2=MUL_ACC(acc2,_mm256_loadu_pd(av+6*NPAR),_mm256_loadu_pd(wv+6*NPAR)); \
  case -8: acc3=MUL_ACC(acc3,_mm256_loadu_pd(av+7*NPAR),_mm256_loadu_pd(wv+7*NPAR)); \
  av+=8*NPAR; wv+=8*NPAR; \
  }while(--n2!=0); \
  } \
 } \
 acc3=MUL_ACC(acc3,_mm256_maskload_pd(av,endmask),_mm256_maskload_pd(wv,endmask)); av+=((dplen-1)&(NPAR-1))+1;  wv+=((dplen-1)&(NPAR-1))+1; \
 acc0=_mm256_add_pd(acc0,acc1); acc2=_mm256_add_pd(acc2,acc3); acc0=_mm256_add_pd(acc0,acc2); /* combine accumulators vertically */ \
 acc0=_mm256_add_pd(acc0,_mm256_permute4x64_pd(acc0,0b11111110)); acc0=_mm256_add_pd(acc0,_mm256_permute_pd(acc0,0xf));   /* combine accumulators horizontally  01+=23, 0+=1 */ \
 *(I*)zv=_mm256_extract_epi64(_mm256_castpd_si256(acc0),0x0); /* AVX2 *zv=_mm256_cvtsd_f64(acc0); */ ++zv;
#else
#define ONEPRODD D total0=0.0; D total1=0.0; if(dplen&1)total1=(D)*av++*(D)*wv++; DQ(dplen>>1, total0+=(D)*av++*(D)*wv++; total1+=(D)*av++*(D)*wv++;); *zv++=total0+total1;
#endif

// routine to do the dot-product calculations.  Brought out to help the compiler allocate registers
// it=type of input, a,w=args dplen=len of each dot-product
// ndpi is the number of times to repeat each list of a within the inner loop, i. e. # dps to repeat it on
// ndpo is */ inner frame of w, i. e. the number of times to repeat the inner loop
// nfri is the number of times to repeat the short-frame operand for the outer loop
// nfri is */ surplus outer frame
// it&BOX is set if a has the shorter outer frame
// w is never repeated in the inner loop (i. e. you can have multiple w but not multiple a; exchange args to ensure this
// if LIT is set in it, it is OK to use 2x2 operations (viz inner frame of a and outer frame of w are empty)
// 'repeata' flag comes from it&BOX
I jtsumattymesprods(J jt,I it,void *avp, void *wvp,I dplen,I nfro,I nfri,I ndpo,I ndpi,void *zvp){
 if(it&FL){
  NAN0;
#if C_AVX2 || EMU_AVX2
  SUMATLOOP2(D,D,ONEPRODAVXD2(D2,CELL2X2M,CELL2X2L),ONEPRODAVXD1(D1,CELL1X1M,CELL1X1L));
#else
  SUMATLOOP(D,D,ONEPRODD)
#endif
  if(NANTEST){  // if there was an error, it might be 0 * _ which we will turn to 0.  So rerun, checking for that.
   NAN0;
   SUMATLOOP(D,D,D total=0.0; DQ(dplen, D u=*av++; D v=*wv++; if(u&&v)total+=dmul2(u,v);); *zv++=total;)
   NAN1;
  }
 }else if(it&INT){
  SUMATLOOP(I,D,D total0=0.0; D total1=0.0; if(dplen&1)total1=(D)*av++*(D)*wv++; DQ(dplen>>1, total0+=(D)*av++*(D)*wv++; total1+=(D)*av++*(D)*wv++;); *zv++=total0+total1;)
 }else{
  SUMATLOOP(B,I,
   I total=0; UI total2; I *avi=(I*)av; I *wvi=(I*)wv; I q=(dplen-1)>>LGSZI;
    NOUNROLL while(1){total2=0; DQ(MIN(254,q), total2+=*avi++&*wvi++;) q-=254; if(q<=0)break; ADDBYTESINI(total2); total+=total2;} total2+=(*avi&*wvi)&((UI)~(I)0 >> (((-dplen)&(SZI-1))<<3)); ADDBYTESINI(total2); total+=total2;
    *zv++=total;
  )
 }
 R 1;
}

#if C_AVX2 || EMU_AVX2
// +/@:*"1 for QP, with IRS by hand
static DF2(jtsumattymes1E){F12IP;
 if(unlikely((I)((1-AR(a))|(1-AR(w)))<0)){I lr=MIN((RANKT)jt->ranks,AR(a)); I rr=MIN(jt->ranks>>RANKTX,AR(w)); R rank2ex(a,w,(A)self,1,1,lr,rr,jtsumattymes1E);}  // if multiple results needed, do rank loop
 I i; I n=AS(a)[AR(a)-1]; ASSERT(AS(w)[AR(w)-1]==n,EVLENGTH);  // length of vector; verify agreement
 E *x=EAV(a)+n, *y=EAV(w)+n;  // input pointers, advanced past end
 __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin); __m256d mantmask=_mm256_broadcast_sd((D*)&(I){0x000fffffffffffff});  /* needed masks: sign, mantissa */
 /* we will read twice, masking.  We may read the first half twice to avoid overfetch */
 __m256i rdmask=_mm256_castps_si256(_mm256_permutevar_ps(_mm256_castpd_ps(_mm256_broadcast_sd((D*)&maskec4123[n&(NPAR-1)])),_mm256_loadu_si256((__m256i*)&validitymask[2]))); /* masks for the 2 reads */
 I rematom;  /* # atoms to accumulate */
 __m256d in0,in1,x0,x1,y0,y1,z0,z1;
 x -= ((n-1)&(NPAR-1))+1;  /* back up to beginning of last NPAR-atom section */
 in0=_mm256_maskload_pd((D*)(x),rdmask);
 in1=_mm256_maskload_pd((D*)(x+((n-1)&(NPAR/2))),_mm256_slli_epi64(rdmask,1));
 SHUFIN(0,in0,in1,x0,x1);
 y -= ((n-1)&(NPAR-1))+1; in0=_mm256_maskload_pd((D*)(y),rdmask); in1=_mm256_maskload_pd((D*)(y+((n-1)&(NPAR/2))),_mm256_slli_epi64(rdmask,1)); SHUFIN(0,in0,in1,y0,y1);
 MULTEE(x0,x1,y0,y1,z0,z1)
 for(rematom=(n-1)&-NPAR;rematom;rematom-=NPAR){  /* for each full batch */
  x-=NPAR; in0=_mm256_loadu_pd((D*)x); in1=_mm256_loadu_pd((D*)(x+NPAR/2)); SHUFIN(0,in0,in1,x0,x1);  /* read 4 values, put into lanes */ 
  y-=NPAR; in0=_mm256_loadu_pd((D*)y); in1=_mm256_loadu_pd((D*)(y+NPAR/2)); SHUFIN(0,in0,in1,y0,y1);  /* read 4 values, put into lanes */ 
  MULTEE(x0,x1,y0,y1,x0,x1)
  PLUSEE(x0,x1,z0,z1,z0,z1);  /* add to total */
 }
 y0=_mm256_permute_pd(z0,0b1111); y1=_mm256_permute_pd(z1,0b1111);  /* atoms 0+1, 2+3 */
 PLUSEE(y0,y1,z0,z1,z0,z1);
 y0=_mm256_permute4x64_pd(z0,0b10101010); y1=_mm256_permute4x64_pd(z1,0b10101010);  /* atoms 0+2 */
 PLUSEE(y0,y1,z0,z1,z0,z1);
 CANONE(z0,z1)
 A zz; GAT0(zz,QP,1,0); E *z=EAV(zz);  // result area
 *(I*)&z->hi=_mm256_extract_epi64(_mm256_castpd_si256(z0),0x0); *(I*)&z->lo=_mm256_extract_epi64(_mm256_castpd_si256(z1),0x0);
 RETF(zz);
}
#endif


// +/@:*"1 with IRS, also +/@:*"1!.0 on float args and +/@:*"1!.1 producing a float extended-precision result, a length-2 list per product
DF2(jtsumattymes1){F12IP;
 ARGCHK2(a,w);
 I ar=AR(a); I wr=AR(w); I acr=jt->ranks>>RANKTX; I wcr=jt->ranks&RMAX;
 // get the cell-ranks to use 
 acr=ar<acr?ar:acr;   // r=left rank of verb, acr=effective rank
 wcr=wr<wcr?wr:wcr;  // r=right rank of verb, wcr=effective rank
     // note: the prod above can never fail, because it gives the actual # cells of an existing noun
   // Now that we have used the rank info, clear jt->ranks.  All verbs start with jt->ranks=RMAXX unless they have "n applied
   // we do this before we generate failures

 I fit=0; if(unlikely(FAV(self)->id==CFIT))fit=1+FAV(self)->localuse.lu1.fittype;  // fit 0=normal, 1=!.0, 2=!.1
 I *as=AS(a), *ws=AS(w);

 // if an argument is empty, sparse, or not a fast arithmetic type, or only one arg has rank 0, revert to the code for f/@:g atomic
 if(((-((AT(a)|AT(w))&((NOUN|SPARSE)&~(B01|INT|FL))))|(AN(a)-1)|(AN(w)-1)|((acr-1)^(wcr-1)))<0) { // test for all unusual cases
  if(unlikely(fit==2)){
   if((((AN(a)-1)|(AN(w)-1))&~((acr-1)^(wcr-1)))<0){  // if there is an empty and both ranks are 0 or both >0
    ASSERT(unlikely(acr==0)||as[ar-1]==ws[wr-1],EVLENGTH)  // ensure last axis agrees
    A z; dfv2(z,a,w,eval("0 0\"1")); R z;  // !.2 with an empty argument.  Implement as 0 0"1
   }
   ASSERT(0,EVNONCE)  // user expected 2 atoms per result, but we don't support that for repeated atomic arg.
  }
#if C_AVX2 || EMU_AVX2    // high-perf QP only on 64-bit
  if(ISDENSETYPE(AT(a)|AT(w),QP)&&((AN(a)-1)|(AN(w)-1)|(acr-1)|(wcr-1))>=0){
   // QP dot-product.  Transfer to that code with rank still set
   if(unlikely(!(AT(a)&QP)))RZ(a=cvt(QP,a)) else if(unlikely(!(AT(w)&QP)))RZ(w=cvt(QP,w))  // convert lower arg to qp
   RETF(jtsumattymes1E(jt,a,w,self));
  }
#endif
  if(fit!=0)self=FAV(self)->fgh[0];  // lose the !.[01] if we revert
  RESETRANK;  // This is required if we go to slower code
  R rank2ex(a,w,FAV(self)->fgh[0],MIN(acr,1),MIN(wcr,1),acr,wcr,jtfslashatg);
 }
 // We can handle it here, and both ranks are at least 1 or both are rank 0.

 if(unlikely(acr==0)){A t;
  // cell-ranks are 0: append a length-1 trailing axis to the shape and add to the rank
  ++ar; RZ(t=vec(INT,ar,as)); as=IAV(t); as[ar-1]=1; acr=1;  // ,"0 a
  ++wr; RZ(t=vec(INT,wr,ws)); ws=IAV(t); ws[wr-1]=1; wcr=1;  // ,"0 w
 }

 // If there is no additional rank (i. e. highest cell-rank is 1), ignore the given rank (which must be 1 1) and use the argument rank
 // This promotes the outer loops to inner loops
 {I rankgiven = (acr|wcr)-1; acr=rankgiven?acr:ar; wcr=rankgiven?wcr:wr;}

 // Exchange if needed to make the cell-rank of a no greater than that of w.  That way we know that w will never repeat in the inner loop
 if(acr>wcr){A t=w; w=a; a=t; I tr=wr; wr=ar; ar=tr; I tcr=wcr; wcr=acr; acr=tcr; I *ts=as; as=ws; ws=ts;}

 // Verify inner frames match
 ASSERTAGREE(as+ar-acr, ws+wr-wcr, acr-1) ASSERT(as[ar-1]==ws[wr-1],EVLENGTH);  // agreement error if not prefix match

 // Convert arguments as required
 I it=maxtyped(AT(a),AT(w)); it=fit!=0?FL:it;   // if input types are dissimilar, convert to the larger.  For +/@:*"1!.[01], convert everything to float
 if(unlikely(it!=(AT(w)|AT(a)))){   // except when both types equal the desired type...
  if(TYPESNE(it,AT(a))){RZ(a=cvt(it,a));}  // ...convert to common input type
  if(TYPESNE(it,AT(w))){RZ(w=cvt(it,w));}
 }

 // calculate inner repeat amounts and result shape
 I dplen = as[ar-1];  // number of atoms in 1 dot-product
 I ndpo; PROD(ndpo,acr-1,ws+wr-wcr);  // number of cells of a = # 2d-level loops
 I ndpi; PROD(ndpi,wcr-acr,ws+wr-wcr+acr-1);  // number of times each cell of a must be repeated (= excess frame of w)

 A z; 
 // if there is frame, create the outer loop values
 I nfro,nfri;  // outer loop counts, and which arg is repeated
 if(likely(((ar-acr)|(wr-wcr))==0)){  // normal case of no frame
  nfro=nfri=1;  // no outer loops, repeata immaterial
  GA(z,FL>>(it&B01),(ndpo*ndpi)<<(fit>>1),wcr-1+(fit>>1),ws);  // type is INT if inputs booleans, otherwise FL
 }else{
  // There is frame, analyze and check it
  I af=ar-acr; I wf=wr-wcr; I commonf=wf; I *longs=as;
  it|=(ndpo==1)>wf?LIT:0;  // if there is no inner frame for a, and no outer frame for w, signal OK to use 2x2 multiplies.  Mainly this is +/@:*"1/
  commonf=wf>=af?af:commonf; longs=wf>=af?ws:longs; it|=wf>=af?BOX:0;  // repeat flag, length of common frame, pointer to long shape
  af+=wf; af-=2*commonf;  // repurpose af to be length of surplus frame
  ASSERTAGREE(as,ws,commonf)  // verify common frame
  PROD(nfri,af,longs+commonf); PROD(nfro,commonf,longs);   // number of outer loops, number of repeats
  I zn = ndpo*ndpi*nfro; DPMULDE(zn,nfri,zn);  // no error possible till we extend the shape
  GA00(z,FL>>(it&B01),zn<<(fit>>1),af+commonf+wcr-1+(fit>>1)); I *zs=AS(z);  // type is INT if inputs booleans, otherwise FL
  // install the shape
  MCISH(zs,longs,af+commonf); MCISH(zs+af+commonf,ws+wr-wcr,wcr-1);
 }
 if(unlikely(fit==2))AS(z)[AR(z)-1]=2;  // if +/@:*"1!.1, we store two atoms per sum

 if(likely(fit==0)){RZ(jtsumattymesprods(jt,it,voidAV(a),voidAV(w),dplen,nfro,nfri,ndpo,ndpi,voidAV(z)));  // eval standard dot-product, check for error
 }else{
  // here for +/@:*"1!.[01], double-precision dot product  https://www-pequan.lip6.fr/~graillat/papers/IC2012.pdf
  NAN0;
#if C_AVX2 || EMU_AVX2
#define OGITA(in0,in1,n) TWOPROD(in0,in1,h,y) TWOSUM(acc##n,h,acc##n,q) c##n=_mm256_add_pd(_mm256_add_pd(q,y),c##n);
  __m256i endmask; /* length mask for the last word */
  __m256d idreg=_mm256_setzero_pd();
  __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin);
  endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-dplen)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */
  D * RESTRICT av=DAV(a),* RESTRICT wv=DAV(w); D * RESTRICT zv=DAV(z);
  for(--nfro;nfro>=0;--nfro){
   I jj=nfri; D *ov0=it&BOX?av:wv; 
   while(1){
    I iii=ndpo-1;for(;iii>=0;--iii){
     I j=ndpi; D *av0=av; /* i is how many a's are left, j is how many w's*/
     while(1){
      // do one dot-product, av*wv, length dplen
      __m256d acc0=idreg; __m256d acc1=idreg; __m256d acc2=idreg; __m256d acc3=idreg;
      __m256d c0=idreg; __m256d c1=idreg; __m256d c2=idreg; __m256d c3=idreg;  // error terms
      __m256d h; __m256d y; __m256d q; __m256d t;   // new input value, temp to hold high part of sum
      UI n2=DUFFLPCT(dplen-1,2);  /* # turns through duff loop */
      if(n2>0){
       UI backoff=DUFFBACKOFF(dplen-1,2);
       av+=(backoff+1)*NPAR; wv+=(backoff+1)*NPAR;
       switch(backoff){
       do{
       case -1: OGITA(_mm256_loadu_pd(av),_mm256_loadu_pd(wv),0)
       case -2: OGITA(_mm256_loadu_pd(av+1*NPAR),_mm256_loadu_pd(wv+1*NPAR),1)
       case -3: OGITA(_mm256_loadu_pd(av+2*NPAR),_mm256_loadu_pd(wv+2*NPAR),2)
       case -4: OGITA(_mm256_loadu_pd(av+3*NPAR),_mm256_loadu_pd(wv+3*NPAR),3)
       av+=4*NPAR; wv+=4*NPAR;
       }while(--n2>0);
       }
      }
      OGITA(_mm256_maskload_pd(av,endmask),_mm256_maskload_pd(wv,endmask),0) av+=((dplen-1)&(NPAR-1))+1; wv+=((dplen-1)&(NPAR-1))+1;  // the remnant at the end
      c0=_mm256_add_pd(c0,c1); c2=_mm256_add_pd(c2,c3); c0=_mm256_add_pd(c0,c2);   // add all the low parts together - the low bits of the low will not make it through to the result
      TWOSUM(acc0,acc1,acc0,c1) TWOSUM(acc2,acc3,acc2,c2) c2=_mm256_add_pd(c1,c2); c0=_mm256_add_pd(c0,c2);   // add 0+1, 2+3
      TWOSUM(acc0,acc2,acc0,c1) c0=_mm256_add_pd(c0,c1);  // 0+2
      // acc0/c0 survive.  Combine horizontally.  Anything the high part touches must be extended precision; the low in one float.  We guarantee extended precision from
      // the largest intermediate total encountered; sometimes we get a little more.
      c0=_mm256_add_pd(c0,_mm256_permute4x64_pd(c0,0b11111110)); acc1=_mm256_permute4x64_pd(acc0,0b11111110);  // c0: lo01+=lo23, acc1<-hi23
      TWOSUM(acc0,acc1,acc0,c1); c0=_mm256_add_pd(c0,c1); // combine acc0 = hi0+2/1+3, c0 accumulates lo0+lo2+extension0, lo1+lo3+extension1 
      c0=_mm256_add_pd(c0,_mm256_permute_pd(c0,0xf)); acc1=_mm256_permute_pd(acc0,0xf);   // c0[0] has total of all low parts, acc1=hi1+hi3
      TWOSUM(acc0,acc1,acc0,c1); c0=_mm256_add_pd(c0,c1);    // acc0 has sum of all hi parts, c1 sum of all low parts+extensions
      if(fit==1){
       // normal result.  Just add the extensions into the hi part
       acc0=_mm256_add_pd(acc0,c0);  // add low parts back into high in case there is overlap
      }else{
       // extended result.  We must preserve the extension bits in the total and write them out
       TWOSUM(acc0,c0,acc0,c1);  // extended total
       ((I*)zv)[1]=_mm256_extract_epi64(_mm256_castpd_si256(c1),0x0); /* AVX2 zv[1]=_mm256_cvtsd_f64(c1); */  // store it out

      }
      ((I*)zv)[0]=_mm256_extract_epi64(_mm256_castpd_si256(acc0),0x0);  /*  AVX2 *zv=_mm256_cvtsd_f64(acc0); */ zv+=fit;  // store out high (perhaps only) part
      if(!--j)break; av=av0;  // repeat a if needed
     }
    }
    if(!--jj)break;
    if(it&BOX)av=ov0;else wv=ov0;  // repeat whichever arg needs it (there must be one, if jj is not 1)
   }
  }
#else
  D * RESTRICT av=DAV(a),* RESTRICT wv=DAV(w); D * RESTRICT zv=DAV(z);
  for(--nfro;nfro>=0;--nfro){
   I jj=nfri; D *ov0=it&BOX?av:wv; 
   while(1){
    I iii=ndpo-1;for(;iii>=0;--iii){
     I j=ndpi; D *av0=av; /* i is how many a's are left, j is how many w's*/
     while(1){
      // do one dot-product, av*wv, length dplen;
      D p=0.0, s=0.0;
      DQ(dplen, D h; D r; D q; D t; D i00; D i01; D i10; D i11; TWOPROD1(*av,*wv,h,r) TWOSUM1(p,h,p,q) s=q+r+s; ++av; ++wv;)
      *zv++=p+s; // store the single result
      if(!--j)break; av=av0;  // repeat a if needed
     }
    }
    if(!--jj)break;
    if(it&BOX)av=ov0;else wv=ov0;  // repeat whichever arg needs it (there must be one, if jj is not 1)
   }
  }
 #endif
  if(unlikely(NANTEST)){ASSERT(fit==1,EVNAN) RETF(jtsumattymes1(jt,a,w,FAV(self)->fgh[0]))}  // !.1 cannot recover from NaN (which can come from infinities).  Revert !.0 to uncompensated case
 } 
 RETF(z);
}


// f/@:g when f and g are atomic.  If the args are big and not inplace it pays to execute one cell of g at a time to save cache footprint
DF2(jtfslashatg){F12IP;A fs,gs,y,z;B b;C*av,*wv;I ak,an,ar,*as,at,m,
     n,nn,r,rs,*s,t,wk,wn,wr,*ws,wt,yt,zn,zt;VA2 adocv,adocvf;
 ARGCHK3(a,w,self);
 an=AN(a); ar=AR(a); as=AS(a); at=AT(a); at=an?at:B01;
 wn=AN(w); wr=AR(w); ws=AS(w); wt=AT(w); wt=wn?wt:B01;
 b=ar<=wr; r=b?wr:ar; rs=b?ar:wr; s=b?ws:as; nn=s[0]; nn=r?nn:1;  // b='w has higher rank'; r=higher rank rs=lower rank s->longer shape  nn=#items in longer-shape arg
 ASSERTAGREE(as,ws,MIN(ar,wr));
 fs=FAV(self)->fgh[0]; gs=FAV(self)->fgh[1];   // b=0 if @:, 1 if fork; take fs,gs accordingly  ```
 rs=MAX(1,rs); PROD(m,rs-1,s+1); PROD(n,r-rs,s+rs); zn=m*n;   // zn=#atoms in _1-cell of longer arg = #atoms in result; m=#atoms in _1-cell of shorter arg  n=#times to repeat shorter arg  (*/ surplus longer shape)
   // if the short-frame arg is an atom, move its rank to 1 so we get the lengths of the _1-cells of the replicated arguments
 // look for cases that we should NOT handle with special code: empty arg; less than 4 result items of g (we would allocate 3 here); too few items in result (the internal call overhead is high then) 
 if(unlikely((SGNIFDENSE(at|wt)&-an&-wn&(3-nn)&(3-zn))>=0)){R jtupon2cell(jtfg,a,w,self);}  // if sparse or empty, or just 1 item, do it the old-fashioned way
 y=FAV(fs)->fgh[0];  // look at f/
 adocv=var(gs,at,wt); ASSERT(adocv.f,EVDOMAIN); yt=rtype(adocv.cv ); t=atype(adocv.cv);  // get type info on g
 adocvf=var(y,yt,yt); ASSERT(adocvf.f,EVDOMAIN); zt=rtypew(adocvf.cv,yt);   // get type info on f/
 // Also, don't use special code if g is inplaceable.  There's no gain then, because f/ is always inplaceable.  The gain comes when g can be split into small pieces with small overall cache footprint
 if(((JTINPLACEA*((r==ar)&SGNTO0(AC(a)))+((r==wr)&SGNTO0(AC(w))))&(I)jtfg&(adocv.cv>>VIPOKWX)))R jtupon2cell(jtfg,a,w,self);  // if inplaceable, revert
 if(unlikely(!TYPESEQ(yt,zt)))R jtupon2cell(jtfg,a,w,self);  // if the result of f (which feeds through f/) isn't the same type as the result of g, revert
 if(t){  // convert args if needed
  if(TYPESNE(t,at))RZ(a=cvt(t|(adocv.cv&XCVTXNUMORIDEMSK),a));
  if(TYPESNE(t,wt))RZ(w=cvt(t|(adocv.cv&XCVTXNUMORIDEMSK),w));
 }
 ak=b?m:zn; wk=b?zn:m; ak=an<nn?0:ak; wk=wn<nn?0:wk; ak<<=bplg(AT(a));wk<<=bplg(AT(w));
 GA10(y,yt,zn);  // allocate one item for result of g
 GA(z,zt,zn,r-1,1+s);  // allocate main output area for final result from f/
 n^=-b; n=(n==~1)?1:n;  // encode b flag in sign of n
 A z1;B p=0;C*yv,*zu,*zv;  // general f/@:g for atomic f,g.  Do not run g on entire y; instead run one cell at a time
 av=CAV(a)+ak*(nn-1); wv=CAV(w)+wk*(nn-1); yv=CAV(y); zv=CAV(z);  // input and output pointers.  We process cells from back to front
 if(likely((adocvf.cv&VIPOKW)!=0)){
  zu=zv; // f is inplaceable: we don't need ping-pong nuffers, just keep operating inplace on z
 }else{
  // f not inplaceable.  Use zv and zu as ping-pong buffers.  zv comes from z, zu from z1.  The last cell of g goes into zv; the next-last goes into
  // yv and then we take zu=yv f zv; then third-last into yv, then zv=yv f zu, etc.  This way we need only 2 cells of cache even without inplacing f or g
  // We exchange zv/zu in the loop to do the ping-pong
  GA(z1,zt,zn,r-1,1+s); zu=CAVn(r-1,z1);  // allocate ping-pong output area for f/
  z=nn&1?z:z1;  // If the number of items is odd, the final result is in original zv, otherwise original zu
 }
 I rc;  // accumulate error returns
 rc=((AHDR2FN*)adocv.f)AH2A_nm(n,m,av,wv,zv,jt); rc=rc<0?EWOVIP+EWOVIPMULII:rc;  // create first result-cell of g
 DQ(nn-1, av-=ak; wv-=wk; I lrc; lrc=((AHDR2FN*)adocv.f)AH2A_nm(n,m,av,wv,yv,jt); lrc=lrc<0?EWOVIP+EWOVIPMULII:lrc; rc=lrc<rc?lrc:rc;
    lrc=((AHDR2FN*)adocvf.f)AH2A_v(zn,yv,zv,zu,jt); lrc=lrc<0?EWOVIP+EWOVIPMULII:lrc; rc=lrc<rc?lrc:rc; {C* ztemp=zu; zu=zv; zv=ztemp;});  // p==1 means result goes to ping buffer zv
 if(NEVM<(rc&255)){z=jtupon2cell(jtfg,a,w,self);}else{if(rc&255)jsignal(rc);}  // if overflow, revert to old-fashioned way.  If p points to ping, prev result went to pong, make pong the result
 RE(0); RETF(z);
}    /* a f/@:g w where f and g are atomic*/

// Consolidated entry point for ATOMIC2 verbs.  These can be called with self pointing either to a rank block or to the block for
// the atomic.  If the block is a rank block, we will switch self over to the block for the atomic.
// Rank can be passed in via jt->ranks, or in the rank for self.  jt->ranks has priority.
// This entry point supports inplacing
DF2(jtatomic2){F12IP;A z;
 ARGCHK2(a,w);
 UI ar=AR(a), wr=AR(w); I at=AT(a), wt=AT(w); I af;
 if((ar+wr+((at|wt)&((NOUN|SPARSE)&~(B01+INT+FL))))==0){af=0; goto forcess;}  // if args are both INT/FL/B01 atoms, verb rank is immaterial - run as singleton
 A realself=FAV(self)->fgh[0];  // if rank operator, this is nonzero and points to the left arg of rank
 RANK2T selfranks=FAV(self)->lrr;  // get left & right rank from rank/primitive
 self=realself?realself:self;  // if this is a rank block, move to the primitive to get to the function pointers.  u b. or any atomic primitive has f clear
 RANK2T jtranks=jt->ranks;  // fetch IRS ranks if any
 UI awr; I awm1=(AN(a)-1)|(AN(w)-1);  // awr is composite arg ranks; awm1 is 0 for singleton, - for empty
 selfranks=jtranks==R2MAX?selfranks:jtranks;
 // find frame
 af=(I)(ar-((UI)selfranks>>RANKTX)); af=af<0?0:af;  // framelen of a
 I wf=(I)(wr-((UI)selfranks&RANKTMSK)); wf=wf<0?0:wf;  // framelen of w
 // check for non-atomic singletons, which are rare
 if(awm1|((at|wt)&((NOUN|SPARSE)&~(B01+INT+FL)))){
  // not singleton: carry on with normal setup
  awr=(ar<<RANKTX)+wr;  // composite ranks, needed by va2
  // if there is no frame wrt rank, shift down to working on frame wrt 0.  Set selfranks=0 to signal that case.  It uses simpler setup
  selfranks=af+wf==0?0:selfranks; af=af+wf==0?ar:af; wf=selfranks==0?wr:wf;  // the conditions had to be like this to prevent a jmp
  af=af<wf?af:wf;  // set af to short frame for agreement test
 }else{
  // singleton.  we need the rank of the result
  ar-=af; wr-=wf; ar=ar>wr?ar:wr; af=af>wf?af:wf; af+=ar;   // set af to max len of frame, ar to max cell rank; then af=max framelen + max rank = resultrank
forcess:;  // branch point for rank-0 singletons from above, always with atomic result
  z=jtssingleton(jtfg,a,w,af,at,wt,self);
  if(likely(z!=0)){RETF(z);}  // normal case is good return; the rest is retry for singletons
  if(unlikely(jt->jerr<=NEVM)){RETF(z);}   // if error is unrecoverable, don't retry
  // if retryable error, fall through.  The retry will not be through the singleton code
  awr=(AR(a)<<RANKTX)+AR(w);   // recover ranks
  jtfg=(J)((I)jtfg|JTRETRY);  // indicate that we are retrying the operation.  We must, because jt->jerr is set with the retry code
  awm1=af=0;  // when we fall through, neuter the agreement test below, and indicate 'not empty result'
  // we do not do the selfranks adjustment on this leg, because it's very rare & we don't want to have
  // to keep the registers over the subroutine call

  // Recalc values created in the main line.  This is very rare so use minimal registers.  self has been destroyed if ranks were not 0; self & selfranks survive the call then
  if(awr==0){
   // atomic args. self is at its initial value; realself and selfranks have not been created
   realself=FAV(self)->fgh[0];  // if rank operator, this is nonzero and points to the left arg of rank
   selfranks=FAV(self)->lrr;  // get left & right rank from rank/primitive
   jtranks=jt->ranks;  // fetch IRS ranks if any
   self=realself?realself:self;  // if this is a rank block, move to the primitive to get to the function pointers.  u b. or any atomic primitive has f clear
   selfranks=jtranks==R2MAX?selfranks:jtranks;
  }
  // self, awr, and selfranks are needed in the retry
 }
 // not singleton, or singleton needing retry
 
 ASSERTAGREE(AS(a),AS(w),af);  // outermost (or only) agreement check
 NOUNROLL while(1){
  // Run the full dyad, retrying if a retryable error is returned
  z=jtva2(jtfg,a,w,self,(awr<<RANK2TX)+selfranks);  // execute the verb
  if(likely(z!=0)){RETF(z);}  // normal case is good return
  if(unlikely(jt->jerr<=NEVM))break;  // if nonretryable error, exit
  jtfg=(J)((I)jtfg|JTRETRY);  // indicate that we are retrying the operation
 }
 // We hit an error.  We will format it now because we have the IRS ranks that were used in selfranks.  It might be possible to get the ranks from the self?
 // convert 0 rank back to R2MAX to avoid "0 0 in msg
 jt->ranks=selfranks?selfranks:R2MAX;
 if(FAV(self)->flag&VWASUNARY){  // originally monadic shorthand?
  // the verb was translated from a unary shorthand like -: to 0.5 * .  We must translate back for display.
  switch(FAV(self)->id){  // for each id, revert to the original id.  If the original arg was in w, move it to a.
  case CMINUS: switch(FAV(self)->flag&VUNARYCODE3){case VUNARYCODE0: self=ds(CNOT); a=w;  break; case VUNARYCODE1: self=ds(CMINUS); a=w; break; case VUNARYCODE2: self=ds(CLE); break;} break;
  case CPLUS: self=ds(CGE); a=w; break;
  case CSTAR: switch(FAV(self)->flag&VUNARYCODE3){case VUNARYCODE0: self=ds(CPLUSCO); a=w;  break; case VUNARYCODE1: self=ds(CSTARCO); break; case VUNARYCODE2: self=ds(CHALVE); a=w; break; case VUNARYCODE3: self=ds(CCIRCLE); a=w; break;} break;
  case CDIV: switch(FAV(self)->flag&VUNARYCODE3){case VUNARYCODE0: self=ds(CDIV); a=w;  break; case VUNARYCODE1: self=ds(CHALVE); break;} break;
  }
  w=0; // now a monad
 }
 jteformat(jt,self,a,w,0); RESETRANK;
 RETF(z);
}

DF2(jtexpn2  ){F12IP; ARGCHK2(a,w); if(unlikely(((((I)AR(w)-1)&SGNIF(AT(w),FLX))<0)))if(unlikely(0.5==DAV(w)[0]))R sqroot(a);  R jtatomic2(jtfg,a,w,self);}  // use sqrt hardware for sqrt.  Only for atomic w. 
DF2(jtresidue){F12IP; ARGCHK2(a,w); I intmod; if(!((AT(a)|AT(w))&((NOUN|SPARSE)&~INT)|AR(a))&&(intmod=IAV(a)[0], (intmod&-intmod)+(intmod<=0)==0))R intmod2(w,intmod); R jtatomic2(jtfg,a,w,self);}  // special case for x an atom power of 2 


// These are the unary ops that are implemented using a canned argument
// NOTE that they pass through inplaceability

// Shift the w-is-inplaceable flag to a.  Bit 1 is known to be 0 in any call to a monad
#define IPSHIFTWA (jtfg = (J)(intptr_t)(((I)jtfg+JTINPLACEW)&-JTINPLACEA))

// We use the right type of singleton so that we engage AVX loops and avoid promotion of INT[124]
#define SETCONPTR(n) A conptr=num(n); \
  if(unlikely(!(AT(w)&B01+INT+FL))){A conptr2=numi2(n); conptr=AT(w)&INT2?conptr2:conptr; conptr2=numi4(n); conptr=AT(w)&INT4?conptr2:conptr;  \
  }else{A conptr2; if(n<2){conptr2=zeroionei(n); conptr=AT(w)&INT?conptr2:conptr;} conptr2=numvr(n); conptr=AT(w)&FL?conptr2:conptr;}
F1(jtnot){F12IP;ARGCHK1(w); SETCONPTR(1) R AT(w)&B01?eq(num(0),w):jtatomic2(jtfg,conptr,w,(A)&dsCMINUS_NOT);}
// negate moved to va1
F1(jtdecrem){F12IP;ARGCHK1(w); SETCONPTR(1) IPSHIFTWA; R jtatomic2(jtfg,w,conptr,(A)&dsCMINUS_DEC);}
F1(jtincrem){F12IP;ARGCHK1(w); SETCONPTR(1) R jtatomic2(jtfg,conptr,w,(A)&dsCPLUS_INC);}  // leave inplaceable in w only
F1(jtduble ){F12IP;ARGCHK1(w); SETCONPTR(2) R jtatomic2(jtfg,conptr,w,(A)&dsCSTAR_DUBL);}  // leave inplaceable in w only
F1(jtsquare){F12IP;ARGCHK1(w); R jtatomic2(jt,w,w,(A)&dsCSTAR_SQUARE);}   // Never inplace (would be OK if can't overflow)
// recip moved to va1
F1(jthalve ){F12IP;ARGCHK1(w); if(!(AT(w)&XNUM+RAT))R jtatomic2(jtfg,onehalf,w,(A)&dsCSTAR_HALVE); IPSHIFTWA; R jtatomic2(jtfg,w,num(2),(A)&dsCDIV_HALVE);}
// pix moved to va1

static AHDR2(zeroF,B,void,void){n=m<0?1:n; m>>=!SGNTO0(m); m^=REPSGN(m); mvc(m*n,z,MEMSET00LEN,MEMSET00);R EVOK;}
static AHDR2(oneF,B,void,void){n=m<0?1:n; m>>=!SGNTO0(m); m^=REPSGN(m); mvc(m*n,z,1,MEMSET01);R EVOK;}

// table of routines to handle = ~:
static VF eqnetbl[2][16] = {
//    11        12        14        BX        21        22        24       x       41        42        44        x      x      x      SB      INHOMO  // char len of aw, or HOMO SB BX
{ (VF)eqCC, (VF)eqCS, (VF)eqCU, (VF)eqAA, (VF)eqSC, (VF)eqSS, (VF)eqSU, (VF)0, (VF)eqUC, (VF)eqUS, (VF)eqUU, (VF)0, (VF)0, (VF)0, (VF)eqII, (VF)zeroF },
{ (VF)neCC, (VF)neCS, (VF)neCU, (VF)neAA, (VF)neSC, (VF)neSS, (VF)neSU, (VF)0, (VF)neUC, (VF)neUS, (VF)neUU, (VF)0, (VF)0, (VF)0, (VF)neII, (VF)oneF },
};

// Analyze the verb and arguments and come up with *ado, address of the routine to handle one
// list of arguments producing a list of results; and *cv, the conversion control which specifies
// the precision inputs must be converted to, and what the result type will be.
// The flags in cv have doubled letters (e.g. VDD) for input precision, single letters (e. g. VD) for result
// result is a VA2 struct  containing ado and cv.  If failure, ado is 0 and the caller should signal domain error
// The type has been converted to dense type
VA2 jtvar(J jt,A self,I at,I wt){I t;
 // If there is a pending error, it might be one that can be cured with a retry; for example, fixed-point
 // overflow, where we will convert to float.  If the error is one of those, get the routine and conversion
 // for it, and return.
 if(likely(!jt->jerr)){
  // Normal case where we are not retrying: here for numeric arguments
  // vaptr converts the character pseudocode into an entry in va;
  // that entry contains 34 (ado,cv) pairs, indexed according to verb/argument types.
  // the first 9 entries [0-8] are a 3x3 array of the combinations of the main numeric types
  // B,I,D; then [9] CMPX [10] XNUM (but not RAT) [11] RAT [12] SBT (symbol) [13] SP [14] QP [15] INT2 [16] INT4
  // then [17-25] are for verb/, with precisions B I D Z X Q Symb INT2 INT4
  // [26-34] for verb\, and [35-43] for verb\.
  VA *vainfo=((VA*)((I)va+FAV(self)->localuse.lu1.uavandx[1]));  // extract table line from the primitive
  if(withprob(!((t=(at|wt))&(NOUN&~(B01|INT|FL))),0.1)){  // the normal case was tested for in va2()
   // Here for the fast and important case, where the arguments are both B01/INT/FL
   R vainfo->p2[(at*3+(wt&INT+FL))>>INTX];

  }else if(likely(!(t&(NOUN&~NUMERIC)))) {
   // Numeric args, but one of the arguments is CMPX/RAT/XNUM/other numeric precisions 
   I apri=TYPEPRIORITYNUM(at), wpri=TYPEPRIORITYNUM(wt), pri=MAX(apri,wpri);  // conversion priority for each arg
   //  0   1   2   3   4   5   6    7  8  9  A  B  C  D  E   F    10   // priorities
   // B01 LIT C2T C4T INT BOX XNUM RAT FL I1 I2 I4 HP SP QP CMPX SBT
   // 0 4 8 9 10 11   12 13 14 15 16 routine indexes for homogeneous args
   //   0 4 5  6  7    8  9 10 11 12 biased by 4, the smallest we use here
   // B I D Z  X  Q Symb DS  E I2 I4 
   pri=4+((0x5affcbf476ffffffLL>>(pri<<2))&0xf);  // 4 is II, lower than the lowest routine# we can call for here
   VA2 selva2 = vainfo->p2[pri];  // routine/flags for the top-priority arg
   I cvtflgs=(apri>wpri?VCOPYA:0)+(apri<wpri?VCOPYW:0);  // set the flag to cause conversion of low-pri arg to the upper.  This handles ALL mixed-mode conversions  scaf would be nice to avoid conversion of left arg of o.
   cvtflgs=selva2.cv&(VBB|VII|VDD|VZZ)?0:cvtflgs;  //  If the routine already forces a conversion, don't override.  Most DD, SP, QP specify no conversion, but +. or bitwise require bool or integer 
   selva2.cv|=cvtflgs;
   R selva2;
  }else{
   // No retry, but something is nonnumeric.  This will be a domain error except for = and ~:, and a few symbol operations
   VA2 retva2;  retva2.cv=VRNONE+VB; // where we build the return value   cv indicates no input conversion, boolean result, no result conversion
   if(likely(((UC)FAV(self)->id&~1)==CEQ)){I opcode;  // CEQ or CNE
    // = or ~:, possibly inhomogeneous
    if(likely(HOMO(at,wt))){
     opcode=((at>>(C2TX-2))&0b1100)+((wt>>C2TX)&0b0011); opcode=at&SBT?0b1110:opcode; opcode=at&BOX?0b0011:opcode; // bits are a4 a2 w4 w2 if char, 1110 if symbol, 0011 if box.
    }else opcode=15;  // inhomogeneous line
    retva2.f=eqnetbl[(UC)FAV(self)->id&1][opcode];  // return the comparison
    R retva2;
   }
   // not = ~:, better be a symbol
   if(at&wt&SBT)R vainfo->p2[12];  // symbol on symbol - process it through the optbl
   retva2.f=0; R retva2;  // if not symbol, return not found
  }
 }else{VA2 retva2;
  // Here there was an error in a previous run.  We see if we have a way to retry the operation
  retva2.f=0;  // error if not filled in
  switch((UC)FAV(self)->id){
  case CCIRCLE: if(jt->jerr==EWIMAG){retva2.f=(VF)cirZZ; retva2.cv=VRD+VZ+VZZ;} break;
  case CEXP: if(jt->jerr==EWIMAG){retva2.f=(VF)powZZ; retva2.cv=VRNONE+VZ+VZZ;}
             else if(jt->jerr==EWRAT){retva2.f=(VF)powQQ; retva2.cv=VRNONE+VQ+VQQ;}
             else if(jt->jerr==EWIRR){retva2.f=(VF)powDD; retva2.cv=VRNONE+VD+VDD;} break;
  case CBANG: if(jt->jerr==EWIRR){retva2.f=(VF)binDD; retva2.cv=VRNONE+VD+VDD;} break;
  case CDIV: if(jt->jerr==EWRAT){retva2.f=(VF)divQQ; retva2.cv=VRNONE+VQ+VQQ;}
             else if(jt->jerr==EWDIV0){retva2.f=(VF)divDD; retva2.cv=VRNONE+VD+VDD;} break;
// the following errors are normally retryable in place.  We keep the alternate code for sparse
  case CPLUS: if(jt->jerr==EWOVIP+EWOVIPPLUSII||jt->jerr==EWOVIP+EWOVIPPLUSBI||jt->jerr==EWOVIP+EWOVIPPLUSIB){retva2.f=(VF)plusIO; retva2.cv=VRNONE+VD+VII;} break;
  case CMINUS: if(jt->jerr==EWOVIP+EWOVIPMINUSII||jt->jerr==EWOVIP+EWOVIPMINUSBI||jt->jerr==EWOVIP+EWOVIPMINUSIB){retva2.f=(VF)minusIO; retva2.cv=VRNONE+VD+VII;} break;
  case CSTAR: if(jt->jerr==EWOVIP+EWOVIPMULII){retva2.f=(VF)tymesIO; retva2.cv=VRNONE+VD+VII;} break;
  case CPLUSDOT: if(jt->jerr==EWOV){retva2.f=(VF)gcdIO; retva2.cv=VRNONE+VD+VII;} break;
  case CSTARDOT: if(jt->jerr==EWOV){retva2.f=(VF)lcmIO; retva2.cv=VRNONE+VD+VII;} break;
  case CSTILE: if(jt->jerr==EWOV){retva2.f=(VF)remDD; retva2.cv=VRNONE+VD+VDD+VIP;} break;
  }
  if(likely(retva2.f)){RESETERR}else{if(jt->jerr>NEVM){RESETERR jsignal(EVSYSTEM);}}  // system error if unhandled exception.  Otherwise reset error only if we handled it
  R retva2;
 }
}    /* function and control for rank */
