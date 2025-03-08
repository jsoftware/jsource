/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Variants (!.)                                             */

#include "j.h"

// 9!:22 read current fill, or i. 0 0 if none.  Clear fill after reading it
F1(jtqfill){F12IP;PROLOG(976); ASSERTMTV(w); A z=jt->fill; z=z?z:mtm; jt->fill=0; EPILOG(z);}

// here to set jt->fill over the execution of u
static DF2(jtfitfill12){F12IP;w=EPMONAD?0:w; A fs=FAV(self)->fgh[0]; AF f12=FAV(fs)->valencefns[!!w]; A z; jt->fill=FAV(self)->fgh[1]; z=CALL12IP(w,f12,a,w,fs); jt->fill=0; RETF(z);}

static DF1(jtfitct1){F12IP;A fs=FAV(self)->fgh[0]; AF f1=FAV(fs)->valencefns[0]; A z; PUSHCCT(FAV(self)->localuse.lu1.cct) z=CALL1IP(f1,  w,fs); POPCCT RETF(z);}  // lD has the complementary ct

#define fitctvector(name,vector) DF2(name){F12IP;A fs=FAV(self)->fgh[0]; AF f2=FAV(fs)->valencefns[1]; A z; ASSERT(0<=1.0-FAV(self)->localuse.lu1.cct&&1.0-FAV(self)->localuse.lu1.cct<5.82076609134675e-11,EVLIMIT) PUSHCCT(FAV(self)->localuse.lu1.cct) z=vector; POPCCT RETF(z);}
   // we muct audit ct again in case bivalent >!.f was used
static fitctvector(jtfitct2,CALL2IP(f2,a,w,fs))
fitctvector(jtfitcteq,jtatomic2(jtinplace,a,w,fs))
// Note: it is OK to call eformat before popping ct because ct cannot possibly introduce error

// for key, we pass in the tolerance to use for the classification
static DF2(jtfitctkey){F12IP;A fs=FAV(self)->fgh[0]; R jtkeyct(jt,a,w,fs,FAV(self)->localuse.lu1.cct);}  // inplace is OK, since we don't use jt

// To avoid multiple indirect branches, we vector the common comparisons to a routine that jumps directly to them
static const AF aff2[] = {jtfitct2,jtfitcteq,jtfitctkey,
#if SY_64
 jtsfu,   // only in viavx.c
#else
 0,
#endif
};
// cno is 5 for >, 3 for i., 2 for f/.[.], 1 for atomic2 comparison, 0 otherwise (indirect branch)
static A jtfitct(J jt,A a,A w,I cno,A z){
 ARGCHK2(a,w);
 ASSERT(!AR(w),EVRANK);
 D d=1.0; A dw=w; // ct, when that's what we have (init to invalid); w converted to float
 if(likely(w==num(0)))d=0.0; else{if(!ISDENSETYPE(AT(w),FL))dw=ccvt(FL,w,0); if(dw)d=DAV(dw)[0]; RESETERR}  // 0 is usual; otherwise it better be FL, but convert in case its value is 0  It's not a hard error
 // Handle i.!.1 specially; otherwise drop i. back to normal for i.!.0 of i.!.f
 if(unlikely(cno==3)){if(d==1.0){d=1.0-jt->cct; if(!SY_64)cno=0;}else cno=0;}   // i.!.1 is special on 64-bit systems; others just specify fit
 // u!.ct, unless u is > in which case it could be anything
 if(cno!=5){ASSERT(dw,EVDOMAIN) ASSERT(0<=d&&d<5.82076609134675e-11,EVLIMIT)}  // fit is ct, must be float and can't be greater than 2^_34
  // >!.f   we can't audit till we get the valence.  If error converting to float, make d give a runtime error.  Any w is OK for fill
 fdeffillall(z,0,CFIT,VERB,cno==5?(AF)(jtfitfill12):(AF)(jtfitct1),aff2[cno&~4],a,w ,0L,FAV(a)->flag&(VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2|VISATOMIC1|VFCOMPCOMP|VASGSAFE),(I)(FAV(a)->mr),lrv(FAV(a)),rrv(FAV(a)),fffv->localuse.lu0.cachedloc=0,FAV(z)->localuse.lu1.cct = 1.0-d);  // preserve INPLACE flags
 R z;
}

static DF2(jtfitexp2){F12IP;
 F2RANK(0,0,jtfitexp2,self);
 ASSERT(0<=i0(w)&&!jt->jerr,EVDOMAIN);
 A z; R aslash(CSTAR,plus(a,dfv2(z,iota(w),FAV(self)->fgh[1],slash(ds(CSTAR)))));
}    /* a ^!.s w */

static DF2(jtfitpoly2){F12IP;I j;
 F2RANK(1,0,jtfitpoly2,self);
 A z; R aslash(CPLUS,tymes(a,ascan(CSTAR,shift1(plus(w,dfv2(z,IX(SETIC(a,j)),FAV(self)->fgh[1],slash(ds(CSTAR))))))));
}    /* a p.!.s w */

// print precision, just the number of fractional digits requested from sprintf
static DF1(jtfitpp1){F12IP;A fs=FAV(self)->fgh[0]; AF f1=FAV(fs)->valencefns[0]; A z;
 I stkppn=jt->ppn; jt->ppn=AV(FAV(self)->fgh[1])[0]; z=CALL1(f1,w,fs); jt->ppn=stkppn;
 RETF(z);
}

static DF1(jtfitf1){F12IP;V*sv=FAV(self); A z; R dfv1(z,  w,fit(fix(sv->fgh[0],zeroionei(0)),sv->fgh[1]));}  // ?? noun~!.n scaf combine?  Who calls?
static DF2(jtfitf2){F12IP;V*sv=FAV(self); A z; R dfv2(z,a,w,fit(fix(sv->fgh[0],zeroionei(0)),sv->fgh[1]));}

// Fit conjunction u!.v
// Preserve IRS1/IRS2 from u in result verb (exception: CEXP)
// Preserve VISATOMIC1 from u (applies only to numeric atomic ops)
// Preserve comparison-combination flags for tolerance fit, in case this is a fit-allowing primitive that uses them
F2(jtfit){F12IP;A f;C c;I k,l,m,r;
//  ASSERTVN(a,w);  // a must be a verb, w a noun (except for $!.v)
 ASSERT(AT(a)&VERB,EVDOMAIN)  // a must be a verb
 A z; fdefallo(z)
 m=FAV(a)->mr; l=lrv(FAV(a)); r=rrv(FAV(a));
 if(likely(!(AT(w)&VERB))){  // is v a noun?
  // Noun v.
  I cno=0;
  switch(FAV(a)->id){I wval;
  case CGT: cno+=2;  //  >!.f
  case CIOTA: ++cno;  // i.!.1 supported only in viavx.c
  case CSLDOT: case CSLDOTDOT: ++cno;   case CLE: case CLT: case CGE: case CNE: case CEQ: ++cno;
  case CMATCH: case CEPS:    case CICO:      case CNUB:     case CSTAR:  
  case CFLOOR: case CCEIL:  case CSTILE: case CPLUSDOT:  case CSTARDOT: case CABASE:
  case CNOT:   case CXCO:   case CSPARSE:   case CEBAR:
   R jtfitct(jt,a,w,cno,z);
  case CQQ: ;
   RE(wval=i0(w)); ASSERT(BETWEENC(wval,0,1),EVDOMAIN);  // only f"r!.[01] is supported
   ASSERT(FAV(a)->valencefns[1]==jtsumattymes1,EVDOMAIN)  // Must be +/@:*"1!.[01]
   fdeffillall(z,0L,CFIT,VERB,jtvalenceerr,jtsumattymes1,a,w,0L,FAV(a)->flag&(VIRS2|VJTFLGOK2|VFCOMPCOMP|VASGSAFE),m,l,r,fffv->localuse.lu0.cachedloc=0,FAV(z)->localuse.lu1.fittype=wval) RETF(z);  // supports IRS
  case CSLASH: ;
   RE(wval=i0(w)); ASSERT(wval==0,EVDOMAIN);  // only f/!.0 is supported
   ASSERT(FAV(FAV(a)->fgh[0])->id==CPLUS,EVDOMAIN)  // Must be +/!.0
   fdeffill(z,0L,CFIT,VERB,jtcompsum,jtvalenceerr,a,w,0L,VIRS1+VASGSAFE,m,l,r) RETF(z);  // supports IRS
  case CEXP:
   ASSERT(AT(w)&NUMERIC,EVDOMAIN);
   fdeffill(z,0L,CFIT,VERB,jtvalenceerr,jtfitexp2,a,w,0L,VFLAGNONE+VASGSAFE,m,l,r) RETF(z);  // ^!.f
  case CPOLY:
   ASSERT(AT(w)&NUMERIC,EVDOMAIN);
   fdeffill(z,0L,CFIT,VERB,jtvalenceerr,jtfitpoly2,a,w,0L,VFLAGNONE+VASGSAFE,m,l,r) RETF(z);  // p.!.f

  case CHOOK:   // only ($,)
   if(FAV(a)->valencefns[1]==jtreshape)goto fillreshape;
   break;
  case CPOWOP:  // support for #^:_1!.n
   if(FAV(a)->fgh[1]!=num(-1))R jtfitct(jt,a,w,0,z);
   f=FAV(a)->fgh[0]; c=FAV(f)->id;
   if(c==CPOUND){ASSERT(!AR(w),EVRANK); fdeffill(z,0L,CFIT,VERB,jtvalenceerr,jtfitfill12,a,w,0L,VFLAGNONE+VASGSAFE,m,l,r) RETF(z);}  // #^:_1!.f
   ASSERT(c==CAMP,EVDOMAIN);
   f=FAV(f)->fgh[1]; ASSERT(CPOUND==IDD(f),EVDOMAIN);
    // fall through for x&#^:_1!.f
   // fill atoms:
  case CPOUND:  case CTAKE:  case CTAIL: case CCOMMA:  case CCOMDOT: case CLAMIN: case CRAZE:
   ASSERT(!AR(w),EVRANK);  /* fall thru */
  case CROT: case CDOLLAR:  // these allow an empty array 
fillreshape:;
   ASSERT(1>=AR(w),EVRANK);
   ASSERT(!AR(w)||!AN(w),EVLENGTH);
   fdeffill(z,0L,CFIT,VERB,jtfitfill12,jtfitfill12,a,w,0L,FAV(a)->flag&(VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2|VASGSAFE),m,l,r) RETF(z);  // various allowing empty fill

  case CTHORN:
   RE(w=sc(k=i0(w)));
   ASSERT(0<k,EVDOMAIN);
   ASSERT(k<=NPP,EVLIMIT); 
   fdeffill(z,0L,CFIT,VERB,jtfitpp1,FAV(a)->valencefns[1],a,w,0L,VFLAGNONE,m,l,r) RETF(z);  // ":!.f
  case CCYCLE:
   RE(k=i0(w)); ASSERT(2==k,EVDOMAIN); RZ(w=sc(k));
   fdeffill(z,0L,CFIT,VERB,jtpparity,jtvalenceerr,a,w,0L,VFLAGNONE,m,RMAX,RMAX) RETF(z);  // C.!.2
  case CTILDE:   // noun~!.n - what in the world is that?
   ASSERT(NOUN&AT(FAV(a)->fgh[0]),EVDOMAIN);
   fdeffill(z,0L,CFIT,VERB,jtfitf1,jtfitf2,a,w,0L,VFLAGNONE,m,l,r) RETF(z);
  // other cases continue on to error
  }
 }else{
  // v is a verb.  Supported now only in $[!.n]!.v or ($,)[!.n]!.v
  AF rtn1=FAV(a)->valencefns[1]; if(rtn1==jtfitfill12)rtn1=FAV(FAV(a)->fgh[0])->valencefns[1];  // what routine will process?  (if fit, look back to underlying function)
  if(rtn1==jtreshape||rtn1==jtreitem)
   fdeffill(z,0L,CFIT,VERB,jtvalenceerr,jtreshapeblankfn,a,w,0L,FAV(a)->flag&(VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2|VASGSAFE),m,l,r) FAV(z)->localuse.lu1.fittype=rtn1==jtreitem; RETF(z);  // fittype tells whether ($,) or $
 }
 ASSERT(0,EVDOMAIN);
} 

