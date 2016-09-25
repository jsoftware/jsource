/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Variants (!.)                                             */

#include "j.h"


static DF1(jtfitct1){DECLFG;F1PREFIP;A z;D old=jt->ct; jt->ct=*DAV(gs); z=CALL1IP(f1,  w,fs); jt->ct=old; R z;}
static DF2(jtfitct2){DECLFG;F2PREFIP;A z;D old=jt->ct; jt->ct=*DAV(gs); z=CALL2IP(f2,a,w,fs); jt->ct=old; R z;}

static F2(jtfitct){D d;V*sv;
 RZ(a&&w);
 ASSERT(!AR(w),EVRANK);
 sv=VAV(a);
 RZ(w=cvt(FL,w)); d=*DAV(w); ASSERT(0<=d&&d<5.82076609134675e-11,EVDOMAIN);
 R fdef(CFIT,VERB,(AF)(jtfitct1),(AF)(jtfitct2),a,w ,0L,sv->flag&(VIRS1|VIRS2|VINPLACEOK1|VINPLACEOK2|VISATOMIC1),(I)(sv->mr),(I)(sv->lr),(I)(sv->rr));  // preserve INPLACE flags
}

static DF2(jtfitexp2){
 F2RANK(0,0,jtfitexp2,self);
 ASSERT(0<=i0(w)&&!jt->jerr,EVDOMAIN);
 R aslash(CSTAR,plus(a,df2(iota(w),VAV(self)->g,slash(ds(CSTAR)))));
}    /* a ^!.s w */

static DF2(jtfitpoly2){
 F2RANK(1,0,jtfitpoly2,self);
 R aslash(CPLUS,tymes(a,ascan(CSTAR,shift1(plus(w,df2(IX(IC(a)),VAV(self)->g,slash(ds(CSTAR))))))));
}    /* a p.!.s w */

static DF1(jtfitfill1){DECLFG;F1PREFIP;A z; jt->fill=gs; z=CALL1IP(f1,  w,fs); jt->fill=0; R z;}
static DF2(jtfitfill2){DECLFG;F2PREFIP;A z; jt->fill=gs; z=CALL2IP(f2,a,w,fs); jt->fill=0; R z;}

static DF1(jtfitpp1){DECLFG;A z;C d[8],*s=3+jt->pp;
 MC(d,s,8L); 
 sprintf(s,FMTI"g",*AV(gs)); 
 z=CALL1(f1,w,fs); MC(s,d,8L);
 R z;
}

static DF1(jtfitf1){V*sv=VAV(self); R df1(  w,fit(fix(sv->f),sv->g));}
static DF2(jtfitf2){V*sv=VAV(self); R df2(a,w,fit(fix(sv->f),sv->g));}

// Fit conjunction u!.n
// Preserve IRS1/IRS2 from u in result verb (exception: CEXP)
// Preserve VISATOMIC1 from u (applies only to numeric atomic ops)
F2(jtfit){A f;C c;I k,l,m,r;V*sv;
 ASSERTVN(a,w);
 sv=VAV(a); m=sv->mr; l=sv->lr; r=sv->rr;
 switch(sv->id){
  case CLE:    case CLT:    case CGE:    case CGT:       case CNE:      case CEQ:
  case CMATCH: case CEPS:   case CIOTA:  case CICO:      case CNUB:     case CSTAR:  
  case CFLOOR: case CCEIL:  case CSTILE: case CPLUSDOT:  case CSTARDOT: case CABASE:
  case CNOT:   case CXCO:   case CSLDOT: case CSPARSE:   case CEBAR:
   R fitct(a,w);
  case CEXP:
   ASSERT(AT(w)&NUMERIC,EVDOMAIN);
   R CDERIV(CFIT,0L,jtfitexp2,0L, m,l,r);
  case CPOLY:
   ASSERT(AT(w)&NUMERIC,EVDOMAIN);
   R CDERIV(CFIT,0L,jtfitpoly2,0L, m,l,r);   // CPOLY has no VIRS
  case CPOWOP:  // support for #^:_1!.n
   if(VERB&AT(sv->g)||!equ(num[-1],sv->g))R fitct(a,w);
   f=sv->f; c=ID(f);
   if(c==CPOUND){ASSERT(!AR(w),EVRANK); R CDERIV(CFIT,0,jtfitfill2,VFLAGNONE,m,l,r);}  // CPOWOP has no VIRS
   ASSERT(c==CAMP,EVDOMAIN);
   f=VAV(f)->g; ASSERT(CPOUND==ID(f),EVDOMAIN);
  case CPOUND:  case CTAKE:  case CTAIL: case CCOMMA:  case CCOMDOT: case CLAMIN: case CRAZE:
   ASSERT(!AR(w),EVRANK);  /* fall thru */
  case CROT: case CDOLLAR: 
   ASSERT(1>=AR(w),EVRANK);
   ASSERT(!AR(w)||!AN(w),EVLENGTH);
   R CDERIV(CFIT,jtfitfill1,jtfitfill2,sv->flag&(VIRS1|VIRS2|VINPLACEOK1|VINPLACEOK2),m,l,r); // preserve INPLACE flags
  case CTHORN:
   RE(w=sc(k=i0(w)));
   ASSERT(0<k,EVDOMAIN);
   ASSERT(k<=NPP,EVLIMIT); 
   R CDERIV(CFIT,jtfitpp1,sv->f2,0L,m,l,r);  // CTHORN lacks VIRS
  case CCYCLE:
   RE(k=i0(w)); ASSERT(2==k,EVDOMAIN); RZ(w=sc(k));
   R CDERIV(CFIT,jtpparity,0L,0L,m,RMAX,RMAX);  // CCYCLE lacks VIRS
  case CTILDE:
   ASSERT(NOUN&AT(sv->f),EVDOMAIN);
   R CDERIV(CFIT,jtfitf1,jtfitf2,0L,m,l,r);  // m~ has no VIRS
  default:
   ASSERT(0,EVDOMAIN);
}}
