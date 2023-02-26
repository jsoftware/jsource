/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Variants (!.)                                             */

#include "j.h"

// 9!:22 read current fill, or i. 0 0 if none.  Clear fill after reading it
F1(jtqfill){PROLOG(976); ASSERTMTV(w); A z=jt->fill; z=z?z:mtm; jt->fill=0; EPILOG(z);}


static DF1(jtfitct1){DECLFG;F1PREFIP;A z; PUSHCCT(FAV(self)->localuse.lu1.cct) z=CALL1IP(f1,  w,fs); POPCCT RETF(z);}  // lD has the complementary ct

#define fitctvector(name,vector) DF2(name){DECLFG;F2PREFIP;A z; PUSHCCT(FAV(self)->localuse.lu1.cct) z=vector; POPCCT RETF(z);}
static fitctvector(jtfitct2,CALL2IP(f2,a,w,fs))
fitctvector(jtfitcteq,jtatomic2(jtinplace,a,w,fs))
// Note: it is OK to call eformat before popping ct because ct cannot possibly introduce error

// for key, we pass in the tolerance to use for the classification
static DF2(jtfitctkey){DECLFG;R jtkeyct(jt,a,w,fs,FAV(self)->localuse.lu1.cct);}  // inplace is OK, since we don't use jt

// To avoid multiple indirect branches, we vector the common comparisons to a routine that jumps directly to them
static const AF aff2[] = {jtfitct2,jtfitcteq,jtfitctkey
#if SY_64
 ,jtsfu   // only in viavx.c
#endif
};
// cno is 3 for i., 2 for f/.[.], 1 for comparison, 0 otherwise
static A jtfitct(J jt,A a,A w,I cno,A z){V*sv;
 ARGCHK2(a,w);
 ASSERT(!AR(w),EVRANK);
 sv=FAV(a);
 // Get the tolerance, as a float
 D d; if(likely(w==num(0)))d=0.0; else{if(!ISDENSETYPE(AT(w),FL))RZ(w=cvt(FL,w)); d=DAV(w)[0];}  // 0 is usual; otherwise it better be FL, but convert in case its value is 0
 // Handle i.!.1 specially; otherwise drop i. back to normal
 if(unlikely(cno==3))if(d==1.0){d=1.0-jt->cct; if(!SY_64)cno=0;}else cno=0;   // i.!.1 is special on 64-bit systems; others just specify fit
 ASSERT(0<=d&&d<5.82076609134675e-11,EVDOMAIN);  // can't be greater than 2^_34
 fdeffillall(z,0,CFIT,VERB,(AF)(jtfitct1),aff2[cno],a,w ,0L,sv->flag&(VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2|VISATOMIC1|VFCOMPCOMP),(I)(sv->mr),lrv(sv),rrv(sv),fffv->localuse.lu0.cachedloc=0,FAV(z)->localuse.lu1.cct = 1.0-d);  // preserve INPLACE flags
 R z;
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

static DF1(jtfitfill1){DECLFG;F1PREFIP;A z; jt->fill=gs; z=CALL1IP(f1,w,fs); jt->fill=0; RETF(z);}  // gs cannot be virtual
static DF2(jtfitfill2){DECLFG;F2PREFIP;A z; jt->fill=gs; z=CALL2IP(f2,a,w,fs); jt->fill=0; RETF(z);}

// print precision, just the number of fractional digits requested from sprintf
static DF1(jtfitpp1){DECLFG;A z;
 I stkppn=jt->ppn; jt->ppn=AV(gs)[0]; z=CALL1(f1,w,fs); jt->ppn=stkppn;
 RETF(z);
}

static DF1(jtfitf1){V*sv=FAV(self); A z; R df1(z,  w,fit(fix(sv->fgh[0],zeroionei(0)),sv->fgh[1]));}  // ?? noun~!.n
static DF2(jtfitf2){V*sv=FAV(self); A z; R df2(z,a,w,fit(fix(sv->fgh[0],zeroionei(0)),sv->fgh[1]));}

// Fit conjunction u!.n
// Preserve IRS1/IRS2 from u in result verb (exception: CEXP)
// Preserve VISATOMIC1 from u (applies only to numeric atomic ops)
// Preserve comparison-combination flags for tolerance fit, in case this is a fit-allowing primitive that uses them
F2(jtfit){F2PREFIP;A f;C c;I k,l,m,r;V*sv;
 ASSERTVN(a,w);  // a must be a verb, w a noun
 sv=FAV(a); m=sv->mr; l=lrv(sv); r=rrv(sv);
 A z; fdefallo(z)
 I cno=0;
 switch(sv->id){I wval;
  case CIOTA: ++cno;  // i.!.1 supported only in viavx.c
  case CSLDOT: case CSLDOTDOT: ++cno;   case CLE: case CLT: case CGE: case CGT: case CNE: case CEQ: ++cno;
  case CMATCH: case CEPS:    case CICO:      case CNUB:     case CSTAR:  
  case CFLOOR: case CCEIL:  case CSTILE: case CPLUSDOT:  case CSTARDOT: case CABASE:
  case CNOT:   case CXCO:   case CSPARSE:   case CEBAR:
   R jtfitct(jt,a,w,cno,z);
  case CQQ: ;
   RE(wval=i0(w)); ASSERT(BETWEENC(wval,0,1),EVDOMAIN);  // only f"r!.[01] is supported
   ASSERT(sv->valencefns[1]==jtsumattymes1,EVDOMAIN)  // Must be +/@:*"1!.[01]
   fdeffillall(z,0L,CFIT,VERB,jtvalenceerr,jtsumattymes1,a,w,0L,VIRS2,m,l,r,fffv->localuse.lu0.cachedloc=0,FAV(z)->localuse.lu1.fittype=wval) RETF(z);  // supports IRS
  case CSLASH: ;
   RE(wval=i0(w)); ASSERT(wval==0,EVDOMAIN);  // only f/!.0 is supported
   ASSERT(FAV(sv->fgh[0])->id==CPLUS,EVDOMAIN)  // Must be +/!.0
   fdeffill(z,0L,CFIT,VERB,jtcompsum,jtvalenceerr,a,w,0L,VIRS1,m,l,r) RETF(z);  // supports IRS
  case CEXP:
   ASSERT(AT(w)&NUMERIC,EVDOMAIN);
   fdeffill(z,0L,CFIT,VERB,jtvalenceerr,jtfitexp2,a,w,0L,VFLAGNONE,m,l,r) RETF(z);  // ^!.f
  case CPOLY:
   ASSERT(AT(w)&NUMERIC,EVDOMAIN);
   fdeffill(z,0L,CFIT,VERB,jtvalenceerr,jtfitpoly2,a,w,0L,VFLAGNONE,m,l,r) RETF(z);  // p.!.f
  case CPOWOP:  // support for #^:_1!.n
   if(sv->fgh[1]!=num(-1))R jtfitct(jt,a,w,0,z);
   f=sv->fgh[0]; c=ID(f);
   if(c==CPOUND){ASSERT(!AR(w),EVRANK); fdeffill(z,0L,CFIT,VERB,jtvalenceerr,jtfitfill2,a,w,0L,VFLAGNONE,m,l,r) RETF(z);}  // #^:_1!.f
   ASSERT(c==CAMP,EVDOMAIN);
   f=FAV(f)->fgh[1]; ASSERT(CPOUND==ID(f),EVDOMAIN);
   // fall through for x&#^:_1!.f
  // fill atoms:
  case CPOUND:  case CTAKE:  case CTAIL: case CCOMMA:  case CCOMDOT: case CLAMIN: case CRAZE:
   ASSERT(!AR(w),EVRANK);  /* fall thru */
  case CROT: case CDOLLAR:  // these allow an empty array 
   ASSERT(1>=AR(w),EVRANK);
   ASSERT(!AR(w)||!AN(w),EVLENGTH);
   fdeffill(z,0L,CFIT,VERB,jtfitfill1,jtfitfill2,a,w,0L,sv->flag&(VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2),m,l,r) RETF(z);  // ^!.f
  case CTHORN:
   RE(w=sc(k=i0(w)));
   ASSERT(0<k,EVDOMAIN);
   ASSERT(k<=NPP,EVLIMIT); 
   fdeffill(z,0L,CFIT,VERB,jtfitpp1,sv->valencefns[1],a,w,0L,VFLAGNONE,m,l,r) RETF(z);  // ":!.f
  case CCYCLE:
   RE(k=i0(w)); ASSERT(2==k,EVDOMAIN); RZ(w=sc(k));
   fdeffill(z,0L,CFIT,VERB,jtpparity,jtvalenceerr,a,w,0L,VFLAGNONE,m,RMAX,RMAX) RETF(z);  // C.!.2
  case CTILDE:   // noun~!.n - what in the world is that?
   ASSERT(NOUN&AT(sv->fgh[0]),EVDOMAIN);
   fdeffill(z,0L,CFIT,VERB,jtfitf1,jtfitf2,a,w,0L,VFLAGNONE,m,l,r) RETF(z);
  default:
   ASSERT(0,EVDOMAIN);
}}

