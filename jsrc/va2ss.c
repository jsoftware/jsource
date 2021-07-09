/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
// Verbs: Atomic (Scalar) Dyadic when arguments have one atom
// Support for Speedy Singletons

#include "j.h"
#include "ve.h"
#include "vcomp.h"

// obsolete #define (B)wiv ((B)w##iv)
// obsolete #define wiv w##iv
// obsolete #define SSRDS(w) ((SB)w##iv)
// obsolete #define SSRDC(w) ((C)w##iv)
// obsolete #define SSRDW(w) ((US)w##iv)
// obsolete #define SSRDU(w) ((C$)w##iv)
// obsolete #define wdv w##dv
#define SSINGENC(a,w) (3*(a>>INTX)+(w>>INTX))
#define SSINGBB SSINGENC(B01,B01)
#define SSINGBI SSINGENC(B01,INT)
#define SSINGBD SSINGENC(B01,FL)
#define SSINGIB SSINGENC(INT,B01)
#define SSINGII SSINGENC(INT,INT)
#define SSINGID SSINGENC(INT,FL)
#define SSINGDB SSINGENC(FL,B01)
#define SSINGDI SSINGENC(FL,INT)
#define SSINGDD SSINGENC(FL,FL)

#define SSSTORENV(v,z,t,type) {*((type *)zv) = (v); AT(z)=(t); }  // When we know that if the block is reused, we are not changing the type; but we change the type of a new block
#define SSSTORE(v,z,t,type) SSSTORENV(v,z,t,type)  // we don't use MODBLOCKTYPE any more
#define SSSTORENVFL(v,z,t,type) {*((type *)zv) = (v); }  // When we know the type/shape doesn't change (FL,FL->FL)

// Return int version of d, with error if loss of significance
static I intforD(J jt, D d){D q;I z;
 q=jround(d); z=(I)q;
 ASSERT(d==q || FFIEQ(d,q),EVDOMAIN);
 // too-large values don't convert, handle separately
 if(d<(D)IMIN){ASSERT(d>=IMIN*(1+FUZZ),EVDOMAIN); z=IMIN;}  // if tolerantly < IMIN, error; else take IMIN
 else if(d>=FLIMAX){ASSERT(d<=-(IMIN*(1+FUZZ)),EVDOMAIN); z=IMAX;}  // if tolerantly > IMAX, error; else take IMAX
 R z;
}

#define SSINGCASE(id,subtype) (9*(id)+(subtype))   // encode case/args into one branch value
A jtssingleton(J jt, A a,A w,A self,RANK2T awr,RANK2T ranks){A z;
 F2PREFIP;
 // Get the address of an inplaceable assignment, if any
 I aiv=FAV(self)->lc;   // temp, but start as function #
 I caseno=(aiv&0x7f)-VA2CBW1111; caseno=caseno<0?0:caseno;
 caseno=SSINGCASE(caseno,SSINGENC(AT(a),AT(w)));  // start calculating case early
 // Allocate the result area
 {
  // Calculate inplaceability for a and w.  Result must be 0 or 1
  // Inplaceable if: count=1 and zombieval, or count<0, PROVIDED the arg is inplaceable and the block is not UNINCORPABLE  No inplace if on NVR stack (AM is NVR and count>0)
  // UNINCORPABLE may not be set because we aren't looking at the type here and if the value is B01 we might overstore.  It might be worthwhile to accept UNINCORP if
  // len=8.
  I aipok = (((/* obsolete (AC(a)-1)|*/((I)a^(I)jt->asginfo.zombieval))==0)|(SGNTO0(AC(a)))) & ((UI)jtinplace>>JTINPLACEAX) & !(AFLAG(a)&AFUNINCORPABLE+AFRO) & ~(AM(a)&SGNTO0((AMNVRCT-1)-AM(a)));
  I wipok = (((/* obsolete (AC(w)-1)|*/((I)w^(I)jt->asginfo.zombieval))==0)|(SGNTO0(AC(w)))) & ((UI)jtinplace>>JTINPLACEWX) & !(AFLAG(w)&AFUNINCORPABLE+AFRO) & ~(AM(w)&SGNTO0((AMNVRCT-1)-AM(w)));
  // find or allocate the result area
  z=0;
  if(likely(awr==0)){  // both atoms
   // The usual case: both singletons are atoms.  Verb rank is immaterial.  Pick any inplaceable input, or allocate a FL atom if none
   // For comparison operations don't allocate, just return from num().  Leave z=0 to indicate this case
   if((aipok|wipok)+(aiv&0x80)){z=aipok?a:z; z=wipok?w:z;}else GAT0(z,FL,1,0);  // top  bit of lc means 'comparison'
  }else{
   // The nouns have rank, and thus there may be frames.  Calculate the rank of the result, and then see if we can inplace an argument of the needed rank
   I4 fa=(awr>>RANKTX)-(ranks>>RANKTX); fa=fa<0?0:fa;  // framelen of a - will become larger frame, and then desired rank
   I4 fw=(awr&RANKTMSK)-(ranks&RANKTMSK); fw=fw<0?0:fw;  // framelen of w
   I4 ca=(awr>>RANKTX)-fa; I4 cw=(awr&RANKTMSK)-fw; ca=ca<cw?cw:ca;  // ca=larger cell-rank
   fa=fa<fw?fw:fa; fa+=ca;  // fa=larger framelen, and then desired rank
   z=aipok>((I)(awr>>RANKTX)^fa)?a:z; z=wipok>((I)(awr&RANKTMSK)^fa)?w:z; if(!z)GATV1(z,FL,1,fa);  // accept ip only if rank also matches; if we allocate, fill in the shape with 1s
  }
 }
#if 0
 z=0; ac= wc=
 if(likely(rank 0)){
  if(!comparison){
   if(unlikely((((I)(((UI)a^(UI)zombieval)>>1)-((I)jtinplace&JTINPLACEA))|((I)(((UI)w^(UI)zombieval)>>1)-((I)jtinplace&JTINPLACEW)))<0){
    if(likely(((AFLAG(zombieval)>>AFUNINCORPABLEX) | (AM(zombieval)&SGNTO0((AMNVRCT-1)-AM(zombieval))))&1)){
     z=zombieval; goto gotit;
    }
   }
   
   allocate scalar FL
  }
 }else{
 }
gotit:;
#endif
 // Start loading everything we will need as values before the pipeline break.  Tempting to convert int-to-float as well, but perhaps it will predict right?
 aiv=IAV(a)[0]; I wiv=IAV(w)[0],ziv; D adv=DAV(a)[0],wdv=DAV(w)[0],zdv; void *zv=z?voidAV(z):0;  // fetch args before the case breaks the pipe   scaf no conditional - make it always use canned result
 // Huge switch statement to handle every case.  Lump all the booleans together at 0
 switch(caseno){
 default: ASSERTSYS(0,"ssing");
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGBB): SSSTORENV((B)aiv+(B)wiv,z,INT,I) R z;  // NV because B01 is never virtual inplace
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGBD): SSSTORENV((B)aiv+wdv,z,FL,D) R z;
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGDB): SSSTORENV(adv+(B)wiv,z,FL,D) R z;
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGID): SSSTORE(aiv+wdv,z,FL,D) R z;
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGDI): SSSTORE(adv+wiv,z,FL,D) R z;
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGBI): 
  {if(unlikely(__builtin_add_overflow((B)aiv,wiv,&ziv)))SSSTORE((D)(B)aiv+(D)wiv,z,FL,D) else SSSTORENV(ziv,z,INT,I) R z;}
// obsolete  I zv = (I)((UI)av+(UI)wv);  if(zv>=wv)SSSTORENV(zv,z,INT,I) else 
// obsolete   R z;}
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGIB):
  {if(unlikely(__builtin_add_overflow(aiv,(B)wiv,&ziv)))SSSTORE((D)aiv+(D)(B)wiv,z,FL,D) else SSSTORENV(ziv,z,INT,I) R z;}
// obsolete  I zv = (I)((UI)av + (UI)wv);
// obsolete   if (zv>=av)SSSTORENV(zv,z,INT,I) else SSSTORE((D)av+(D)wv,z,FL,D)
// obsolete   R z;}
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGII):
  {if(unlikely(__builtin_add_overflow(aiv,wiv,&ziv)))SSSTORE((D)aiv+(D)wiv,z,FL,D) else SSSTORENV(ziv,z,INT,I) R z;}
// obsolete  I zv = (I)((UI)av + (UI)wv);
// obsolete   if (XANDY((zv^av),(zv^wv))>=0)SSSTORENV(zv,z,INT,I) else SSSTORE((D)av+(D)wv,z,FL,D)
// obsolete   R z;}
 case SSINGCASE(VA2CPLUS-VA2CBW1111,SSINGDD):
  {NAN0; SSSTORENVFL(adv+wdv,z,FL,D) NAN1; R z;}


 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGBB): SSSTORENV((B)aiv-(B)wiv,z,INT,I) R z;
 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGBD): SSSTORENV((B)aiv-wdv,z,FL,D) R z;
 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGDB): SSSTORENV(adv-(B)wiv,z,FL,D) R z;
 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGID): SSSTORE(aiv-wdv,z,FL,D) R z;
 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGDI): SSSTORE(adv-wiv,z,FL,D) R z;
 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGBI): 
  {if(unlikely(__builtin_sub_overflow((B)aiv,wiv,&ziv)))SSSTORE((D)(B)aiv-(D)wiv,z,FL,D) else SSSTORENV(ziv,z,INT,I) R z;}
// obsolete  I zv = (I)((UI)av - (UI)wv);
// obsolete   if((wv&zv)>=0)SSSTORENV(zv,z,INT,I) else SSSTORE((D)av-(D)wv,z,FL,D)
// obsolete   R z;}
 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGIB):
  {if(unlikely(__builtin_sub_overflow(aiv,(B)wiv,&ziv)))SSSTORE((D)aiv-(D)(B)wiv,z,FL,D) else SSSTORENV(ziv,z,INT,I) R z;}
// obsolete  I zv = (I)((UI)av - (UI)wv);
// obsolete   if (zv<=av)SSSTORENV(zv,z,INT,I) else SSSTORE((D)av-(D)wv,z,FL,D)
// obsolete   R z;}
 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGII):
  {if(unlikely(__builtin_sub_overflow(aiv,wiv,&ziv)))SSSTORE((D)aiv-(D)wiv,z,FL,D) else SSSTORENV(ziv,z,INT,I) R z;}
// obsolete  I zv = (I)((UI)av - (UI)wv);
// obsolete   if (XANDY((zv^av),~(zv^wv))>=0)SSSTORENV(zv,z,INT,I) else SSSTORE((D)av-(D)wv,z,FL,D)
// obsolete   R z;}
 case SSINGCASE(VA2CMINUS-VA2CBW1111,SSINGDD):
  {NAN0; SSSTORENVFL(adv-wdv,z,FL,D) NAN1;  R z;}


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
 case SSINGCASE(VA2CSTAR-VA2CBW1111,SSINGID): {I av=aiv; SSSTORE(av?av*wdv:0.0,z,FL,D) R z;}
 case SSINGCASE(VA2CSTAR-VA2CBW1111,SSINGDI): {I wv=wiv; SSSTORE(wv?wv*adv:0.0,z,FL,D) R z;}
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
 case SSINGCASE(VA2CDIV-VA2CBW1111,SSINGDD): {NAN0; SSSTORENVFL(DIV(adv,(D)wdv),z,FL,D) NAN1; R z;}


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
 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGBD): aiv=(B)aiv; ASSERT(wdv==0.0 || TEQ(wdv,1.0),EVDOMAIN); wiv=(I)wdv; goto nandresult;
 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGDB): wiv=(B)wiv; ASSERT(adv==0.0 || TEQ(adv,1.0),EVDOMAIN); aiv=(I)adv; goto nandresult;
 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGID): ASSERT(!(aiv&-2) && (wdv==0.0 || TEQ(wdv,1.0)),EVDOMAIN); wiv=(I)wdv; goto nandresult;
 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGDI): ASSERT(!(wiv&-2) && (adv==0.0 || TEQ(adv,1.0)),EVDOMAIN); aiv=(I)adv; goto nandresult;
 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGBI): aiv=(B)aiv; ASSERT(!(wiv&-2),EVDOMAIN); goto nandresult;
 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGIB): wiv=(B)wiv; ASSERT(!(aiv&-2),EVDOMAIN); goto nandresult;
 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGII): ASSERT(!((aiv|wiv)&-2),EVDOMAIN); goto nandresult;
 case SSINGCASE(VA2CSTARCO-VA2CBW1111,SSINGDD): ASSERT((adv==0.0 || TEQ(adv,1.0)) && (wdv==0.0 || TEQ(wdv,1.0)),EVDOMAIN); wiv=(I)wdv; aiv=(I)adv; goto nandresult;


 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGBB): aiv=(B)aiv; wiv=(B)wiv; goto norresult;
 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGBD): aiv=(B)aiv; ASSERT(wdv==0.0 || TEQ(wdv,1.0),EVDOMAIN); wiv=(I)wdv; goto norresult;
 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGDB): wiv=(B)wiv; ASSERT(adv==0.0 || TEQ(adv,1.0),EVDOMAIN); aiv=(I)adv; goto norresult;
 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGID): ASSERT(!(aiv&-2) && (wdv==0.0 || TEQ(wdv,1.0)),EVDOMAIN); wiv=(I)wdv; goto norresult;
 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGDI): ASSERT(!(wiv&-2) && (adv==0.0 || TEQ(adv,1.0)),EVDOMAIN); aiv=(I)adv; goto norresult;
 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGBI): aiv=(B)aiv; ASSERT(!(wiv&-2),EVDOMAIN); goto norresult;
 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGIB): wiv=(B)wiv; ASSERT(!(aiv&-2),EVDOMAIN); goto norresult;
 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGII): ASSERT(!((aiv|wiv)&-2),EVDOMAIN); goto norresult;
 case SSINGCASE(VA2CPLUSCO-VA2CBW1111,SSINGDD): ASSERT((adv==0.0 || TEQ(adv,1.0)) && (wdv==0.0 || TEQ(wdv,1.0)),EVDOMAIN); wiv=(I)wdv; aiv=(I)adv; goto norresult;


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
 case SSINGCASE(VA2CBW1111-VA2CBW1111,SSINGDD): aiv=intforD(jt,adv); wiv=intforD(jt,wdv); goto bitwiseresult;


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
 ziv=FAV(self)->lc-VA2CBW0000;  // mask describing operation
 ziv=((aiv&wiv)&REPSGN(SGNIF(ziv,0)))|((aiv&~wiv)&REPSGN(SGNIF(ziv,1)))|((~aiv&wiv)&REPSGN(SGNIF(ziv,2)))|((~aiv&~wiv)&REPSGN(SGNIF(ziv,3)));
 SSSTORE(ziv,z,INT,I) R z;

 compareresult:
 if(!z)R num(ziv);  // Don't store into num[].  Could perhaps do this without the branch, but it seems too lengthy
 SSSTORE((B)ziv,z,B01,B) R z;  // OK to store into allocated/inplace area.

 circleresult: ;
 D cirvals[3]={adv,wdv};  // put ops into memory
 I rc=cirDD(1,1,cirvals,cirvals+1,cirvals+2,jt); rc=rc<0?EWOVIP+EWOVIPMULII:rc;  // run the routine
 if(rc==EVOK){SSSTORE(cirvals[2],z,FL,D);}else{jsignal(rc); z=0;} R z;  // Don't change the input block if there is an error.  If there is, post it to the return area

}


