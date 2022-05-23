/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Variants (!.)                                             */

#include "j.h"


static DF1(jtfitct1){DECLFG;F1PREFIP;A z; PUSHCCT(FAV(self)->localuse.lu1.cct) z=CALL1IP(f1,  w,fs); POPCCT RETF(z);}  // lD has the complementary ct

#define fitctvector(name,vector) static DF2(name){DECLFG;F2PREFIP;A z; PUSHCCT(FAV(self)->localuse.lu1.cct) z=vector; POPCCT RETF(z);}
fitctvector(jtfitct2,CALL2IP(f2,a,w,fs))
fitctvector(jtfitcteq,jtatomic2(jtinplace,a,w,fs))

// for key, we pass in the tolerance to use for the classification
static DF2(jtfitctkey){DECLFG;R jtkeyct(jt,a,w,fs,FAV(self)->localuse.lu1.cct);}  // inplace is OK, since we don't use jt

// To avoid multiple indirect branches, we vector the common comparisons to a routine that jumps directly to them
static const AF aff2[] = {jtfitct2,jtfitcteq,jtfitctkey
#if SY_64
 ,jtsfu   // only in viavx.c
#endif
};
// cno is 3 for i., 2 for f/., 1 for comparison, 0 otherwise
static A jtfitct(J jt,A a,A w,I cno){V*sv;
 ARGCHK2(a,w);
 ASSERT(!AR(w),EVRANK);
 sv=FAV(a);
 // Get the tolerance, as a float
 D d; if(likely(w==num(0)))d=0.0; else{if(!ISDENSETYPE(AT(w),FL))RZ(w=cvt(FL,w)); d=DAV(w)[0];}  // 0 is usual; otherwise it better be FL, but convert in case its value is 0
 // Handle i.!.1 specially; otherwise drop i. back to normal
 if(unlikely(cno==3))if(d==1.0){d=1.0-jt->cct; if(!SY_64)cno=0;}else cno=0;   // i.!.1 is special on 64-bit systems; others just specify fit
 ASSERT(0<=d&&d<5.82076609134675e-11,EVDOMAIN);  // can't be greater than 2^_34
 A fn = fdef(0,CFIT,VERB,(AF)(jtfitct1),aff2[cno],a,w ,0L,sv->flag&(VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2|VISATOMIC1|VFCOMPCOMP),(I)(sv->mr),lrv(sv),rrv(sv));  // preserve INPLACE flags
 RZ(fn); FAV(fn)->localuse.lu1.cct = 1.0-d; R fn;  // save the fit value in this verb
}

static DF2(jtfitexp2){
 F2RANK(0,0,jtfitexp2,self);
 ASSERT(0<=i0(w)&&!jt->jerr,EVDOMAIN);
 A z; R aslash(CSTAR,plus(a,df2(z,iota(w),FAV(self)->fgh[1],slash(ds(CSTAR)))));
}    /* a ^!.s w */

static DF2(jtfitpoly2){I j;
 F2RANK(1,0,jtfitpoly2,self);
 A z; R aslash(CPLUS,tymes(a,ascan(CSTAR,shift1(plus(w,df2(z,IX(SETIC(a,j)),FAV(self)->fgh[1],slash(ds(CSTAR))))))));
}    /* a p.!.s w */

static DF1(jtfitfill1){DECLFG;F1PREFIP;A z; jt->fill=gs; z=CALL1IP(f1,  w,fs); jt->fill=0; RETF(z);}  // gs cannot be virtual
static DF2(jtfitfill2){DECLFG;F2PREFIP;A z; jt->fill=gs; z=CALL2IP(f2,a,w,fs); jt->fill=0; RETF(z);}

// print precision, just the number of fractional digits requested from sprintf
static DF1(jtfitpp1){DECLFG;A z;
 I stkppn=jt->ppn; jt->ppn=AV(gs)[0]; z=CALL1(f1,w,fs); jt->ppn=stkppn;
 RETF(z);
}

static DF1(jtfitf1){V*sv=FAV(self); A z; R df1(z,  w,fit(fix(sv->fgh[0],zeroionei(0)),sv->fgh[1]));}
static DF2(jtfitf2){V*sv=FAV(self); A z; R df2(z,a,w,fit(fix(sv->fgh[0],zeroionei(0)),sv->fgh[1]));}

// Fit conjunction u!.n
// Preserve IRS1/IRS2 from u in result verb (exception: CEXP)
// Preserve VISATOMIC1 from u (applies only to numeric atomic ops)
// Preserve comparison-combination flags for tolerance fit, in case this is a fit-allowing primitive that uses them
F2(jtfit){F2PREFIP;A f;C c;I k,l,m,r;V*sv;
 ASSERTVN(a,w);  // a must be a verb, w a noun
 sv=FAV(a); m=sv->mr; l=lrv(sv); r=rrv(sv);
 I cno=0;
 switch(sv->id){I wval;
  case CIOTA: ++cno;  // i.!.1 supported only in viavx.c
  case CSLDOT: ++cno;   case CLE: case CLT: case CGE: case CGT: case CNE: case CEQ: ++cno;
  case CMATCH: case CEPS:    case CICO:      case CNUB:     case CSTAR:  
  case CFLOOR: case CCEIL:  case CSTILE: case CPLUSDOT:  case CSTARDOT: case CABASE:
  case CNOT:   case CXCO:   case CSPARSE:   case CEBAR:
   R fitct(a,w,cno);
  case CQQ: ;
   RE(wval=i0(w)); ASSERT(wval==0,EVDOMAIN);  // only f"r!.0 is supported
   ASSERT(sv->valencefns[1]==jtsumattymes1,EVDOMAIN)  // Must be +/@:*"1!:0
   R CDERIV(CFIT,0,jtsumattymes1,VIRS2, m,l,r);  // supports IRS
  case CSLASH: ;
   RE(wval=i0(w)); ASSERT(wval==0,EVDOMAIN);  // only f/!.0 is supported
   ASSERT(FAV(sv->fgh[0])->id==CPLUS,EVDOMAIN)  // Must be +/!:0
   R CDERIV(CFIT,jtcompsum,0,VIRS1, m,l,r);  // supports IRS
  case CEXP:
   ASSERT(AT(w)&NUMERIC,EVDOMAIN);
   R CDERIV(CFIT,0L,jtfitexp2,0L, m,l,r);
  case CPOLY:
   ASSERT(AT(w)&NUMERIC,EVDOMAIN);
   R CDERIV(CFIT,0L,jtfitpoly2,0L, m,l,r);   // CPOLY has no VIRS
  case CPOWOP:  // support for #^:_1!.n
   if(sv->fgh[1]!=num(-1))R fitct(a,w,0);
   f=sv->fgh[0]; c=ID(f);
   if(c==CPOUND){ASSERT(!AR(w),EVRANK); R CDERIV(CFIT,0,jtfitfill2,VFLAGNONE,m,l,r);}  // CPOWOP has no VIRS
   ASSERT(c==CAMP,EVDOMAIN);
   f=FAV(f)->fgh[1]; ASSERT(CPOUND==ID(f),EVDOMAIN);
  // fill atoms:
  case CPOUND:  case CTAKE:  case CTAIL: case CCOMMA:  case CCOMDOT: case CLAMIN: case CRAZE:
   ASSERT(!AR(w),EVRANK);  /* fall thru */
  case CROT: case CDOLLAR:  // these allow an empty array 
   ASSERT(1>=AR(w),EVRANK);
   ASSERT(!AR(w)||!AN(w),EVLENGTH);
   R CDERIV(CFIT,jtfitfill1,jtfitfill2,sv->flag&(VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2),m,l,r); // preserve INPLACE flags
  case CTHORN:
   RE(w=sc(k=i0(w)));
   ASSERT(0<k,EVDOMAIN);
   ASSERT(k<=NPP,EVLIMIT); 
   R CDERIV(CFIT,jtfitpp1,sv->valencefns[1],0L,m,l,r);  // CTHORN lacks VIRS
  case CCYCLE:
   RE(k=i0(w)); ASSERT(2==k,EVDOMAIN); RZ(w=sc(k));
   R CDERIV(CFIT,jtpparity,0L,0L,m,RMAX,RMAX);  // CCYCLE lacks VIRS
  case CTILDE:
   ASSERT(NOUN&AT(sv->fgh[0]),EVDOMAIN);
   R CDERIV(CFIT,jtfitf1,jtfitf2,0L,m,l,r);  // m~ has no VIRS
  default:
   ASSERT(0,EVDOMAIN);
}}

