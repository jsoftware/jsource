/* Copyright (c) 1990-2023, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: "Mathematical" Functions (Irrational, Transcendental, etc.)      */

#include "j.h"
#include "ve.h"
#if defined(__GNUC__) && !(C_AVX2 || EMU_AVX2)
static int64_t m7f = 0x7fffffffffffffffLL;
#define COMMA ,
#endif

D jtintpow(J jt,D x,I n){D r=1;
 if(0>n){x=1/x; if(n==IMIN){r=x; n=IMAX;} else n=-n;}  // kludge use r=x; n=-1-n;
 while(n){if(1&n)r*=x; x*=x; n>>=1;}
 R r;
}    /* x^n where x is real and n is integral */

D jtpospow(J jt,D x,D y){
 if(0==y)R 1.0;
 if(0==x)R 0<y?0.0:inf;
 if(0<x){
  if(y== inf)R 1<x?inf:1>x?0.0:1.0;
  if(y==-inf)R 1<x?0.0:1>x?inf:1.0;
  R exp(y*log(x));
 }
 if(y==-inf){ASSERT(-1>x,EVDOMAIN); R 0.0;}
 if(y== inf){ASSERT(-1<x,EVDOMAIN); R 0.0;}
 jt->jerr=EWIMAG;
 R 0;
}    /* x^y where x and y are real */

#define POWXB(u,v)  (v?u:1)
#define POWBX(u,v)  (u?1.0:v<0?inf:!v)
#define POWII(u,v)  intpow((D)u,v)
#define POWID(u,v)  pospow((D)u,v)

APFX(powBI, D,B,I, POWBX ,,R EVOK;)
APFX(powBD, D,B,D, POWBX ,,R EVOK;)
APFX(powIB, I,I,B, POWXB ,,R EVOK;)
APFX(powII, D,I,I, POWII ,,HDR1JERR)
APFX(powID, D,I,D, POWID ,,HDR1JERR)
APFX(powDB, D,D,B, POWXB ,,R EVOK;)
APFX(powZZ, Z,Z,Z, zpow  ,,HDR1JERR)



APFX(cirZZ, Z,Z,Z, zcir  ,NAN0;,HDR1JERRNAN)

// Call SLEEF with no checking
#define TRIGUNLIM(lbl,sleeffn)  {AVXATOMLOOP(1, \
 , \
 u=sleeffn(u); \
 , \
 )}


// Call SLEEF after checking symmetric 2-sided limits.  If comp is not true everywhere, signal err, else call sleeffn
#if C_AVX2 || EMU_AVX2
#define TRIGSYMM(lbl,limit,comp,err,sleeffn)  {AVXATOMLOOP(1, \
 __m256d thmax; thmax=_mm256_broadcast_sd(&limit); \
 __m256d absmask; absmask=_mm256_broadcast_sd((D*)&Iimax); \
 , \
 ASSERTWR(_mm256_testc_pd(absmask,_mm256_cmp_pd(_mm256_and_pd(u,absmask), thmax,comp)),err); /* assert all bits 0 */ \
 u=sleeffn(u); \
 , \
 )}

// Call SLEEF after checking limits, but calculate the value to use then
#define TRIGCLAMP(limit,decls,comp,argmod,sleeffn,resultmod)  {AVXATOMLOOP(1, \
 __m256d thmax; thmax=_mm256_broadcast_sd(&limit); \
 decls \
 __m256d absmask; absmask=_mm256_broadcast_sd((D*)&Iimax); \
 , \
 __m256d outofbounds = _mm256_cmp_pd(u, thmax,comp); \
 argmod \
 u=sleeffn(u); \
 resultmod \
 , \
 )}

#elif defined(__GNUC__)   // vector extension

#define TRIGSYMM(lbl,limit,comp,err,sleeffn)  {AVXATOMLOOP(1, \
 float64x2_t thmax={limit COMMA limit}; \
 float64x2_t absmask={*(D *)&m7f COMMA *(D *)&m7f}; \
 , \
 ASSERTSYS(comp==_CMP_GT_OQ,"vec_any only _CMP_GT_OQ support"); \
 ASSERTWR(vec_any_si128(vec_and_pd(u, absmask) > thmax)==0,err); \
 u=sleeffn(u); \
 , \
 )}

// Call SLEEF after checking limits, but calculate the value to use then
#define TRIGCLAMP(limit,decls,comp,argmod,sleeffn,resultmod)  {AVXATOMLOOP(1, \
 float64x2_t thmax={limit COMMA limit}; \
 decls \
 D absmask={*(D *)&m7f COMMA *(D *)&m7f}; \
 , \
 float64x2_t outofbounds = _mm256_cmp_pd(u, thmax,comp); \
 argmod \
 u=sleeffn(u); \
 resultmod \
 , \
 )}
#endif

#if SLEEF
AHDR1(expD,D,D) {  AVXATOMLOOP(1,
 ,
 u=Sleef_expd4(u);
 ,
 R EVOK;
 )
}

#if C_AVX2 || EMU_AVX2
AHDR1(logD,D,D) {  AVXATOMLOOP(1,
 __m256d zero; zero=_mm256_setzero_pd();
 ,
 ASSERTWR(_mm256_testc_pd(zero,_mm256_cmp_pd(u, zero,_CMP_LT_OQ)),EWIMAG);  // assert compares all 0
 u=Sleef_logd4(u);
 ,
 R EVOK;
 )
}

AHDR2(powDI,D,D,I) {I v;
 if(n-1==0)  DQ(m,               *z++=intpow(*x,*y); x++; y++; )
 else if(n-1<0)DQ(m, D u=*x++; DQC(n, *z++=intpow( u,*y);      y++;))
 else{  // repeated exponent: use parallel instructions
  DQ(m, v=*y++;  // for each exponent
   AVXATOMLOOP(1, // build result in u, which is also the input
    __m256d one = _mm256_broadcast_sd(&zone.real);
   ,
    {__m256d upow;
    UI rempow;  // power left to take
    if(v>=0){  // positive power
     upow=u; u = one;   // init result to 1 before powers
     rempow=v;
    }else{  // negative power, take recip of u and complement the power
     upow = u = _mm256_div_pd(one,u);  // start power at -1
     rempow=~v;  // subtract one from pos pow since we start with recip (avoids IMIN problem)
    }
    while(rempow){if(rempow&1)u=_mm256_mul_pd(u,upow); upow=_mm256_mul_pd(upow,upow); rempow>>=1;}}
   ,
   )
  )
 }      
 HDR1JERR
}

AHDR2(powDD,D,D,D) {D v;
 if(n-1==0) DQ(m, *z++=pospow(*x,*y); x++; y++; )
 else if(n-1<0)DQ(m, D u=*x++; DQC(n, *z++=pospow( u,*y); y++;))
 else{  // repeated exponent: use parallel instructions
  DQ(m, v=*y++;  // for each exponent
   if(v==0){DQ(n, *z++=1.0;) x+=n;}
   else if(ABS(v)==inf){DQ(n, D u=*x++; ASSERT(u>=0,EWIMAG); if(u==1.0)*z=1.0; else{*z=(v>0)^(u>1.0)?0.0:inf;} ++z;)}
   else{
    AVXATOMLOOP(1,  // build result in u, which is also the input
      __m256d zero = _mm256_setzero_pd();
      __m256d vv = _mm256_broadcast_sd(&v);  // 4 copies of exponent  (2 if __SSE2__)
     ,
      ASSERTWR(_mm256_testc_pd(zero,_mm256_cmp_pd(u, zero,_CMP_LT_OQ)),EWIMAG);  // assert all compare bit 0
      u=Sleef_log2d4(u);
      u=_mm256_mul_pd(u,vv);
      u=Sleef_exp2d4(u);
     ,
    )
   }
  )
 }      
 HDR1JERR
}
#elif defined(__GNUC__)   // vector extension
AHDR1(logD,D,D) {  AVXATOMLOOP(1,
 float64x2_t zero={0.0 COMMA 0.0};
 ,
 ASSERTWR(vec_any_si128(u<zero)==0,EWIMAG);
 u=Sleef_logd4(u);
 ,
 R EVOK;
 )
}

AHDR2(powDI,D,D,I) {I v;
 if(n-1==0)  DQ(m,               *z++=intpow(*x,*y); x++; y++; )
 else if(n-1<0)DQ(m, D u=*x++; DQC(n, *z++=intpow( u,*y);      y++;))
 else{  // repeated exponent: use parallel instructions
  DQ(m, v=*y++;  // for each exponent
   AVXATOMLOOP(1,  // build result in u, which is also the input
    float64x2_t one = {1.0 COMMA 1.0};
   ,
    float64x2_t upow;
    UI rempow;  // power left to take
    if(v>=0){  // positive power
     upow=u; u = one;   // init result to 1 before powers
     rempow=v;
    }else{  // negative power, take recip of u and complement the power
     upow = u = one/u;  // start power at -1
     rempow=~v;  // subtract one from pos pow since we start with recip (avoids IMIN problem)
    }
    while(rempow){if(rempow&1)u=u*upow; upow=upow*upow; rempow>>=1;}
   ,
   )
  )
 }      
 HDR1JERR
}

AHDR2(powDD,D,D,D) {D v;
 if(n-1==0) DQ(m, *z++=pospow(*x,*y); x++; y++; )
 else if(n-1<0)DQ(m, D u=*x++; DQC(n, *z++=pospow( u,*y); y++;))
 else{  // repeated exponent: use parallel instructions
  DQ(m, v=*y++;  // for each exponent
   if(v==0){DQ(n, *z++=1.0;) x+=n;}
   else if(ABS(v)==inf){DQ(n, D u=*x++; ASSERT(u>=0,EWIMAG); if(u==1.0)*z=1.0; else{*z=(v>0)^(u>1.0)?0.0:inf;} ++z;)}
   else{
    AVXATOMLOOP(1,  // build result in u, which is also the input
      float64x2_t zero = {0.0 COMMA 0.0};
      float64x2_t vv = {v COMMA v};  // 4 copies of exponent  (2 if __SSE2__)
     ,
      ASSERTWR(vec_any_si128(u<zero)==0,EWIMAG);
      u=Sleef_log2d4(u);
      u=u*vv;
      u=Sleef_exp2d4(u);
     ,
    )
   )
  }
 }      
 HDR1JERR
}
#endif


#else
AMON(expD,   D,D, *z=*x<EMIN?0.0:EMAX<*x?inf:exp(   *x);)
AMON(logD,   D,D, ASSERTWR(0<=*x,EWIMAG); *z=log(   *x);)
APFX(powDI, D,D,I, intpow,,HDR1JERR)
APFX(powDD, D,D,D, pospow,,HDR1JERR)

#endif
AMON(expI,   D,I, *z=*x<EMIN?0.0:EMAX<*x?inf:exp((D)*x);)
AMON(logI,   D,I, ASSERTWR(0<=*x,EWIMAG); *z=log((D)*x);)
static D thmaxx=THMAX;

static I jtcirx(J jt,I n,I k,D*z,D*x){D p,t;
 NAN0;
 switch(k){
 default: ASSERTWR(0,EWIMAG);
 case  0: DQ(n, t=*x++; ASSERTWR(ABS(t)<=1.0, EWIMAG ); *z++=sqrt(1.0-t*t);); break;
 case  1: ;
#if SLEEF
TRIGSYMM(lbl1,thmaxx,_CMP_GT_OQ,EVLIMIT,Sleef_sind4)
IGNORENAN
#else
   DQ(n, t=*x++; ASSERTWR(ABS(t)<THMAX,EVLIMIT); *z++=sin(t););
#endif
   break;
 case  2:  ;
#if SLEEF
TRIGSYMM(lbl2,thmaxx,_CMP_GT_OQ,EVLIMIT,Sleef_cosd4)
IGNORENAN
#else
 DQ(n, t=*x++; ASSERTWR(ABS(t)<THMAX,EVLIMIT); *z++=cos(t););
#endif
 break;
 case  3:  ;
#if SLEEF
 TRIGSYMM(lbl3,thmaxx,_CMP_GT_OQ,EVLIMIT,Sleef_tand4)
#else
 DQ(n, t=*x++; ASSERTWR(ABS(t)<THMAX,EVLIMIT); *z++=tan(t););       
#endif
 break;
 case  4: DQ(n, t=*x++;                                     *z++=t<-1e8?-t:1e8<t?t:sqrt(t*t+1.0););       break;
 case  5: DQ(n, t=*x++;                                     *z++=t<-EMAX2?infm:EMAX2<t?inf:sinh(t););     break;
 case  6: DQ(n, t=*x++;                                     *z++=t<-EMAX2||    EMAX2<t?inf:cosh(t););     break;
 case  7: ;
#if SLEEF
 TRIGUNLIM(lbl7,Sleef_tanhd4)
 NAN0;  // SLEEF gives the correct answer but may raise a NaN flag
#else
// math library tanh is slooooow  case  7: DQ(n, t=*x++;                                     *z++=t<-TMAX?-1:TMAX<t?1:tanh(t););           break;
 DQ(n, t=*x++;                                     *z++=t<-TMAX?-1:TMAX<t?1:(1.0-exp(-2*t))/(1.0+exp(-2*t)););
#endif
 break;
 case -1: ;
#if SLEEF
  TRIGSYMM(lblm1,zone.real,_CMP_GT_OQ,EWIMAG,Sleef_asind4)
#else
  DQ(n, t=*x++; ASSERTWR( -1.0<=t&&t<=1.0, EWIMAG ); *z++=asin(t););
#if defined(ANDROID) && (defined(__aarch32__)||defined(__arm__)||defined(__aarch64__))
// NaN bug in android asin()  _1 o. _1
NAN0;
#endif
#endif
 break;
 case -2: ;
#if SLEEF
  TRIGSYMM(lblm2,zone.real,_CMP_GT_OQ,EWIMAG,Sleef_acosd4)
#else
  DQ(n, t=*x++; ASSERTWR( -1.0<=t&&t<=1.0, EWIMAG ); *z++=acos(t););
#endif
  break;
 case -3: ;
#if SLEEF
  TRIGUNLIM(lblm3,Sleef_atand4)
#else
  DQ(n,                                             *z++=atan(*x++););
#endif
  break;
 case -4: DQ(n, t=*x++; ASSERTWR(t<=-1.0||1.0<=t,  EWIMAG ); *z++=t<-1e8||1e8<t?t:t==-1?0:(t+1)*sqrt((t-1)/(t+1));); break;
 case -5: p=log(2.0); 
           DQ(n, t=*x++; *z++=1.0e8<t?p+log(t):-7.8e3>t?-(p+log(-t)):log(t+sqrt(t*t+1.0)););               break;
 case -6: p=log(2.0); 
           DQ(n, t=*x++; ASSERTWR(          1.0<=t, EWIMAG ); *z++=1.0e8<t?p+log(t):log(t+sqrt(t*t-1.0));); break;
 case -7: DQ(n, t=*x++; ASSERTWR( -1.0<=t&&t<=1.0, EWIMAG ); *z++=0.5*log((1.0+t)/(1.0-t)););              break;
 case  9: DQ(n,         *z++=*x++;);           break;    
 case 10: DQ(n, t=*x++; *z++=ABS(t););         break;
 case 11: DQ(n,         *z++=0.0;);            break;
 case 12: DQ(n,         *z++=0<=*x++?0.0:PI;); break;
 }
 ASSERTWR(!NANTEST,EVNAN);
 R EVOK;
}

#if SLEEF  // scaf SLEEF quad required
// typedef struct {IL hi; IL lo; } Sleef_quad;
// in sleefquad.h
// typedef struct { uint64_t x, y; } Sleef_quad;
static Sleef_quad etof128(E w){
 IL ehi=*(IL*)&w.hi; UIL elo=*(UIL*)&w.lo;  // IEEE bits of w
 UIL loneg=(IL)(ehi^elo)>>63;  // -1 if bottom part has a different sign from the top part
 IL ihi=ehi+loneg;   // if the bottom part has a different sign from the top, its significance must be subtracted from the upper.  That will occasion a borrow, which we handle here.
                                       // it is possible that the decrement will flow through to the exponent; that's OK.  bottom can never be 0 of opposite sign.  52 bits of sig
 I eexpxs=52-((ehi>>52)&0x7ff)+((elo>>52)&0x7ff);  // excess exponent of elo, i. e. gap between ehi and elo.  Must be >=0.  If 0, the bits of elo are right next to ehi
 eexpxs=MIN(eexpxs,63);  // clamp shift count to within range
 elo=(elo&0x000fffffffffffffll)|(likely((elo&0x7ff0000000000000)!=0)?0x001000000000000ll:0);  // remove sign of elo, add hidden bit
 elo=(((elo<<11)>>eexpxs)^loneg)+loneg;  // shift sig to adj to hi, then add gap (all unsigned); then make the lo bits neg if needed
 elo=(elo>>4)|(ehi<<60);  // take contiguous lower bits, possibly with some low-order 0s
 ehi=((ehi&0x7fffffffffffffffll)>>4)+0x3c00000000000000+(ehi&0x8000000000000000ll);  // shift exponent (& mantissa) down 4; rebias exponent; preserve sign
 R *(Sleef_quad*)&(IL[2]){elo,ehi};  // return the value
}
E f128toe(Sleef_quad w){
 UIL exp=(((UIL*)&w)[1]>>48)&0x7fff;  // extract 15-bit biased SLEEF exponent
 if(unlikely(!BETWEENO(exp,0x3c00,0x4400))){  // exponent too big or too small
  R (E){.hi=exp<0x4000?0.:copysign(inf,((D*)&w)[1]),.lo=0.};   // clamp at limit
 }
 IL ihi=((((((IL*)&w)[1]<<4)|(((UIL*)&w)[0]>>(64-4)))+0x4000000000000000)&0x7fffffffffffffffLL)|(((IL*)&w)[1]&0x8000000000000000ll);  // shorten sign, replace with 4 LSBs, giving top 52 bits + hidden bit
 D dlo=(*(D*)&(IL){ihi&0x7ff000000000000ll})*2.22044604925031308e-16* *(D*)&(IL){((IL*)&w)[0]&0x0fffffffffffffffll};  // this is (1.0 with the exponent of ihi) * (2^_52) * (low bits of input)
 D hi,lo; TWOSUMBS1(*(D*)&ihi,dlo,hi,lo)  // remove any overlap
 R CANONE1(hi,lo);  // return canonical form
}
#endif

static I jtcire(J jt,I n,I k,E*z,E*x){E p,t;
#if SLEEF  // scaf
 Sleef_quad sleefq1=sleef_q(1LL,0LL,0);
 Sleef_quad sleefq0=sleef_q(0LL,0LL,0);
 Sleef_quad sleefq05=sleef_q(1LL,0LL,-1);
 NAN0;  // Note some of the SLEEF function raise NaN errors that we must clear
 switch(k){
 default: ASSERTWR(0,EWIMAG);
 case  0: {DQ(n, t=*x++; Sleef_quad ts=etof128(t); Sleef_quad tsqm1=Sleef_subq1_u05purecfma(sleefq1,Sleef_mulq1_u05purecfma(ts,ts));
    ASSERTWR(Sleef_icmpgeq1_purecfma(tsqm1,sleefq0), EWIMAG ) *z++=f128toe(Sleef_sqrtq1_u05purecfma(tsqm1));); } break;
 case  1: ;
   DQ(n, t=*x++; ASSERTWR(ABS(t.hi)<THMAX,EVLIMIT); *z++=f128toe(Sleef_sinq1_u10purecfma(etof128(t))););   break;
 case  2:  ;
   DQ(n, t=*x++; ASSERTWR(ABS(t.hi)<THMAX,EVLIMIT); *z++=f128toe(Sleef_cosq1_u10purecfma(etof128(t)));); break;
 case  3:  ;
   DQ(n, t=*x++; ASSERTWR(ABS(t.hi)<THMAX,EVLIMIT); *z++=f128toe(Sleef_tanq1_u10purecfma(etof128(t)));); break;
 break;
 case  4: {DQ(n, t=*x++; if(ABS(t.hi)>1e17){if(t.hi>0)*z++=t; else{z->hi=-t.hi; z->lo=-t.lo;}} else {Sleef_quad ts=etof128(t); Sleef_quad tsqp1=Sleef_addq1_u05purecfma(Sleef_mulq1_u05purecfma(ts,ts),sleefq1);
    *z=f128toe(Sleef_sqrtq1_u05purecfma(tsqp1));} ++z;) } break;
 case  5:   DQ(n, t=*x++; if(ABS(t.hi)>EMAX2){*z++=(E){.hi=t.hi<0?infm:inf,.lo=copysign(0.,t.hi)};} else {*z++=f128toe(Sleef_sinhq1_u10purecfma(etof128(t)));});   break;
 case  6:   DQ(n, t=*x++; if(ABS(t.hi)>EMAX2){*z++=(E){.hi=inf,.lo=0};} else {*z++=f128toe(Sleef_coshq1_u10purecfma(etof128(t)));});   break;
 case  7:   DQ(n, t=*x++; if(ABS(t.hi)>TMAX){*z++=(E){.hi=t.hi<0?-1.:1.,.lo=copysign(0.,t.hi)};} else {*z++=f128toe(Sleef_tanhq1_u10purecfma(etof128(t)));});   break;
 case -1: ;
  DQ(n, t=*x++; Sleef_quad ts=etof128(t); ASSERTWR(Sleef_icmpleq1_purecfma(Sleef_fabsq1_purecfma(ts),sleefq1), EWIMAG); *z++=f128toe(Sleef_asinq1_u10purecfma(ts)););   NAN0; break;
 case -2: ;
   DQ(n, t=*x++; Sleef_quad ts=etof128(t); ASSERTWR(Sleef_icmpleq1_purecfma(Sleef_fabsq1_purecfma(ts),sleefq1), EWIMAG); *z++=f128toe(Sleef_acosq1_u10purecfma(ts)););   NAN0; break;
 case -3: ;
   DQ(n, t=*x++; *z++=f128toe(Sleef_atanq1_u10purecfma(etof128(t))););   break;
  break;
 case -4: DQ(n, t=*x++; Sleef_quad ts=etof128(t);  ASSERTWR(Sleef_icmpgeq1_purecfma(Sleef_fabsq1_purecfma(ts),sleefq1),  EWIMAG );
          if(ABS(t.hi)>1e17)*z++=t; else {Sleef_quad tsp1=Sleef_addq1_u05purecfma(ts,sleefq1); if(Sleef_icmpeqq1_purecfma(tsp1,sleefq0))*z++=(E){.hi=-1,.lo=-0};
          else{Sleef_quad tsm1=Sleef_subq1_u05purecfma(ts,sleefq1); *z++=f128toe(Sleef_mulq1_u05purecfma(tsp1,Sleef_sqrtq1_u05purecfma(Sleef_divq1_u05purecfma(tsm1,tsp1))));}
          }
            );  NAN0; break;
 case -5: DQ(n, t=*x++; Sleef_quad ts=etof128(t);
                if(t.hi>1e17){*z++=f128toe(Sleef_addq1_u05purecfma(Sleef_logq1_u10purecfma(ts),SLEEF_M_LN2q));
                }else{*z++=f128toe(Sleef_addq1_u05purecfma(ts,Sleef_sqrtq1_u05purecfma(Sleef_subq1_u05purecfma(Sleef_mulq1_u05purecfma(ts,ts),sleefq1))));
                }
            );   break;
 case -6: DQ(n, t=*x++; Sleef_quad ts=etof128(t); ASSERTWR(Sleef_icmpgeq1_purecfma(ts,sleefq1), EWIMAG);
                if(t.hi>1e17){*z++=f128toe(Sleef_addq1_u05purecfma(Sleef_logq1_u10purecfma(ts),SLEEF_M_LN2q));
               }else if(t.hi<-8e6){*z++=f128toe(Sleef_addq1_u05purecfma(Sleef_logq1_u10purecfma(Sleef_negq1_purecfma(ts)),SLEEF_M_LN2q));
               }else{*z++=f128toe(Sleef_addq1_u05purecfma(ts,Sleef_sqrtq1_u05purecfma(Sleef_addq1_u05purecfma(Sleef_mulq1_u05purecfma(ts,ts),sleefq1))));
               }
            );   break;
 case -7: DQ(n, t=*x++; Sleef_quad ts=etof128(t); ASSERTWR(Sleef_icmpgeq1_purecfma(Sleef_fabsq1_purecfma(ts),sleefq1), EWIMAG);
                *z++=f128toe(Sleef_mulq1_u05purecfma(sleefq05,Sleef_logq1_u10purecfma(Sleef_divq1_u05purecfma(Sleef_addq1_u05purecfma(sleefq1,ts),Sleef_subq1_u05purecfma(sleefq1,ts)))));
            );   break;
       
 case  9: DQ(n,         *z++=*x++;);           break;    
 case 10: DQ(n, t=*x++; D oldsgn=t.hi; z->hi=ABS(t.hi); *(IL*)&z->lo=*(IL*)&t.lo^(*(IL*)&oldsgn&0x8000000000000000);    z++;);         break;
 case 11: DQ(n,         z->hi=z->lo=0.0; ++z;)            break;
 case 12: DQ(n,         *z++=*(0<=x->hi?(E*)&zeroZ:&epi); ++x;); break;
 }
 ASSERTWR(!NANTEST,EVNAN);
#endif
 R EVOK;
}

AHDR2(cirBD,D,B,D){ASSERTWR(n<=1&&1==m,EWIMAG); n^=REPSGN(n); R cirx(n,   (I)*x,z,y);}
AHDR2(cirID,D,I,D){ASSERTWR(n<=1&&1==m,EWIMAG); n^=REPSGN(n); R cirx(n,   *x,z,y);}

AHDR2(cirDD,D,D,D){I k=(I)jround(*x);
 ASSERTWR(k==*x,EVDOMAIN); 
 ASSERTWR(n<=1&&1==m,EWIMAG); // if more than one x value, retry as general case
 n^=REPSGN(n);   // convert complementary n to nonneg
 R cirx(n,k,z,y);
}

AHDR2(cirEE,E,E,E){I k=(I)jround(x->hi);
 ASSERTWR(k==x->hi,EVDOMAIN); 
 ASSERTWR(n<=1&&1==m,EWIMAG); // if more than one x value, retry as general case
 n^=REPSGN(n);   // convert complementary n to nonneg
 R jtcire(jt,n,k,z,y);
}


F2(jtlogar2){A z;I t;
 ARGCHK2(a,w); 
 RE(t=maxtype(AT(a),AT(w)));
 if(!(t&XNUM)||jt->xmode==XMEXACT){jt->xmode=XMEXACT; R jtatomic2(JTIPAW,logar1(w),logar1(a),ds(CDIV));}  // better to multiply by recip, but not much, & it makes 0 ^. 0 not fail
 z=rank2ex0(cvt(XNUM,a),cvt(XNUM,w),DUMMYSELF,jtxlog2a); 
 if(z)R z;
 if(jt->jerr==EWIMAG||jt->jerr==EWIRR){RESETERR; jt->xmode=XMEXACT; R divideAW(logar1(w),logar1(a));}
 R 0;
}
    
F2(jtroot){A z;I t;
 ARGCHK2(a,w);
 RE(t=maxtype(AT(a),AT(w)));
 A ma=a; if(TYPESNE(t,AT(a)))RZ(ma=cvt(t,a));
 A mw=w; if(TYPESNE(t,AT(w)))RZ(mw=cvt(t,w));
 if(!(t&XNUM))R expn2(mw,recip(ma));  // not inplaceable - could be IMAG
 z=rank2ex0(ma,mw,DUMMYSELF,jtxroota);
 switch(jt->jerr){
 case EWIMAG: RESETERR; R expn2(cvt(CMPX,w),recip(cvt(CMPX,a)));
 case EWRAT: 
 case EWIRR:  RESETERR; R expn2(cvt(FL,  w),recip(cvt(FL,  a)));
 default:     R z;
 }
}

F1(jtjdot1){R tymes(a0j1,w);}
// j. should preserve -0 when given real arguments
F2(jtjdot2){
 ARGCHK2(a,w); I at=AT(a), wt=AT(w);
 if((~(SGNIFSPARSE(at)|SGNIFSPARSE(wt))&-(at&B01+INT+FL)&-(wt&B01+INT+FL))<0){A z;  // if both args real and not sparse
  I ar=AR(a), wr=AR(w);
  ASSERTAGREE(AS(a),AS(w),MIN(ar,wr))  // verify agreement
  if(!(at&FL))RZ(a=cvt(FL,a)) if(!(wt&FL))RZ(w=cvt(FL,w))  // convert to FL if needed
  RZ(IRS2(a,w,DUMMYSELF,0,0,jtover,z)) makewritable(z) AN(z)>>=1; AR(z)=MAX(AR(a),AR(w)); AT(z)=CMPX;  // z=a ,"0 w, then switch to CMPX
  R z;
 }
 R plus(a,tymes(a0j1,w));
}
F1(jtrdot1){R expn1(jdot1(w));}
F2(jtrdot2){R tymes(a,rdot1(w));}


F1(jtpolar){ARGCHK1(w); A z; R cvt((AT(w)&SPARSE)+FL,df2(z,v2(10L,12L),w,qq(ds(CCIRCLE),v2(1L,0L))));}

DF1(jtrect){A e,z;B b;I r,t;P*wp,*zp;Z c;
 ARGCHK1(w); 
 t=AT(w); r=AR(w); RESETRANK;   // Run as infinite rank
 ASSERT(!AN(w)||t&NUMERIC,EVDOMAIN);
 if(ISDENSETYPE(t,CMPX)){GATV0(z,FL,2*AN(w),1+r); MCISH(AS(z),AS(w),r) AS(z)[r]=2; MC(AV(z),AV(w),AN(z)*sizeof(D)); R z;}
 else if(unlikely(ISSPARSE(t))){
  b=1&&t&CMPX;
  GASPARSE(z,b?FL:t,1,1+r,AS(w)); AS(z)[r]=2;
  wp=PAV(w); zp=PAV(z);
  if(b){e=SPA(wp,e); c=*ZAV(e); ASSERT(FFEQ(c.re,c.im),EVSPARSE); SPB(zp,e,scf(c.re));}
  else SPB(zp,e,ca(SPA(wp,e)));
  SPB(zp,a,ca(SPA(wp,a)));
  SPB(zp,i,ca(SPA(wp,i)));
  SPB(zp,x,rect(SPA(wp,x)));
  R z;
 }else R IRS2(w,num(0),self,0,0,jtover,z);
}
