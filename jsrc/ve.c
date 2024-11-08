/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Elementary Functions (Arithmetic, etc.)                          */

#include "j.h"
#include "vasm.h"
#include "vcomp.h"
#include "ve.h"

#define DIVI(u,v)     (u||v ? ddiv2(u,(D)v) : 0.0)
#define DIVBB(u,v)    (v?u:u?inf:0.0)

#define TYMESBX(u,v)  (u?v:0)
#define TYMESXB(u,v)  (v?u:0)
#define TYMESID(u,v)  (u   ?u*v:0)
#define TYMESDI(u,v)  (   v?u*v:0)
#define TYMESDD(u,v)  TYMES(u,v)

// commutative function sharing.  We can share only if the function produces no error codes, because sparse code doesn't know how to run recovery
// We swap the x & y args and switch the specification of repeated arg if there is one
AHDR2(plusID,PVD,PVI,PVD){R plusDI(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(plusBD,PVD,PVB,PVD){R plusDB(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(plusBI,PVI,PVB,PVI){R EVOK==plusIB(m^1^SGNTO0(m),z,y,x,n,jt)?EVOK:EWOVIP+EWOVIPPLUSBI;}
AHDR2(minID,PVD,PVI,PVD){R minDI(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(minBD,PVD,PVB,PVD){R minDB(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(minBI,PVI,PVB,PVI){R minIB(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(maxID,PVD,PVI,PVD){R maxDI(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(maxBD,PVD,PVB,PVD){R maxDB(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(maxBI,PVI,PVB,PVI){R maxIB(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(tymesDI,PVD,PVD,PVI){R tymesID(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(tymesDB,PVD,PVD,PVB){R tymesBD(m^1^SGNTO0(m),z,y,x,n,jt);}  // does tymesBI too

#if C_AVX2 || EMU_AVX2
#define ORIGMN I nsav=n; if(msav>=0){n=m; m=nsav; nsav^=-(msav&1);}  // restore old-style mn from modified mn and msav.  If msav<0, OK already, otherwise swap & transfer flag to nsav
primop256(plusDD,0x7,NAN0;,zz=_mm256_add_pd(xx,yy),R NANTEST?EVNAN:EVOK;)
primop256(minusDD,0x6,NAN0;,zz=_mm256_sub_pd(xx,yy),R NANTEST?EVNAN:EVOK;)
primop256(minDD,0x7,,zz=_mm256_min_pd(xx,yy),R EVOK;)
primop256(maxDD,0x7,,zz=_mm256_max_pd(xx,yy),R EVOK;)
primop256(tymesDD,0x7,D *zsav=z;NAN0;,zz=_mm256_mul_pd(xx,yy),if(unlikely(NANTEST)){z=zsav; DQ(n*m, if(_isnan(*(D*)z))*(D*)z=0.0; z=(C*)z+SZD;)} R EVOK;)
// div can fail from 0%0 (which we turn to 0) or inf%inf (which we fail)
primop256(divDD,4,D *zsav=z; D *xsav=x; D *ysav=y; I msav=m;NAN0;,zz=_mm256_div_pd(xx,yy),
  if(unlikely(NANTEST)){ORIGMN z=zsav; xsav=zsav==ysav?xsav:ysav; m*=n; n=(nsav^SGNIF(zsav==ysav,0))>=0?n:1; nsav=--n; DQ(m, if(_isnan(*(D*)z)){ASSERTWR(*xsav==0,EVNAN); *(D*)z=0.0;} z=(C*)z+SZD; --n; xsav-=REPSGN(n); n=n<0?nsav:n;)} R EVOK;)

#else
APFX( plusDD, D,D,D, PLUS,NAN0;,ASSERTWR(!NANTEST,EVNAN); R EVOK;)
APFX(minusDD, D,D,D, MINUS,NAN0;,ASSERTWR(!NANTEST,EVNAN); R EVOK;)
APFX(minDD, D,D,D, MIN,,R EVOK;)
APFX(maxDD, D,D,D, MAX,,R EVOK;)
APFX(tymesDD, D,D,D, TYMESDD,,R EVOK;)
APFX(  divDD, D,D,D, DIV,NAN0;,ASSERTWR(!NANTEST,EVNAN); R EVOK;)
APFX( plusEE, E,E,E, PLUSE,NAN0;,ASSERTWR(!NANTEST,EVNAN); R EVOK;)
APFX( minusEE, E,E,E, MINUSE,NAN0;,ASSERTWR(!NANTEST,EVNAN); R EVOK;)
#endif
APFX( divEE, E,E,E, DIVE,NAN0;,ASSERTWR(!NANTEST,EVNAN); R EVOK;)
APFX( minEE, E,E,E, MINE,,R EVOK;)
APFX( maxEE, E,E,E, MAXE,,R EVOK;)

#if C_AVX2 || EMU_AVX2
primop256(plusDI,0x16,,zz=_mm256_add_pd(xx,yy),R EVOK;)
// commutative primop256(plusID,8,,zz=_mm256_add_pd(xx,yy),R EVOK;)
primop256(plusDB,0xa06,,zz=_mm256_add_pd(xx,yy),R EVOK;)
// commutative primop256(plusBD,0x900,,zz=_mm256_add_pd(xx,yy),R EVOK;)
primop256(plusII,0x23,__m256d oflo=_mm256_setzero_pd();,
 zz=_mm256_castsi256_pd(_mm256_add_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))); oflo=_mm256_or_pd(oflo,_mm256_andnot_pd(_mm256_xor_pd(xx,yy),_mm256_xor_pd(xx,zz)));,
 R !_mm256_testc_pd(_mm256_setzero_pd(),oflo)?EWOVIP+EWOVIPPLUSII:EVOK;)  // ~0 & oflo, testc if =0 which means no overflow
// commutative primop256(plusBI,0x860,__m256d oflo=_mm256_setzero_pd();,
// commutative zz=_mm256_castsi256_pd(_mm256_add_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))); oflo=_mm256_or_pd(oflo,_mm256_castsi256_pd(_mm256_cmpgt_epi32(_mm256_castpd_si256(yy),_mm256_castpd_si256(zz))));,
// commutative R !_mm256_testc_pd(_mm256_setzero_pd(),oflo)?EWOVIP+EWOVIPPLUSBI:EVOK;)  // ~0 & oflo, testc if =0 which means no overflow
primop256(plusIB,0x882,__m256d oflo=_mm256_setzero_pd();,
 zz=_mm256_castsi256_pd(_mm256_add_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))); oflo=_mm256_or_pd(oflo,_mm256_castsi256_pd(_mm256_cmpgt_epi32(_mm256_castpd_si256(xx),_mm256_castpd_si256(zz))));,
 R !_mm256_testc_pd(_mm256_setzero_pd(),oflo)?EWOVIP+EWOVIPPLUSIB:EVOK;)  // ~0 & oflo, testc if =0 which means no overflow
primop256(plusBB,0xc1,,
zz=_mm256_castsi256_pd(_mm256_add_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,R EVOK;)
primop256(minusDI,0x16,,zz=_mm256_sub_pd(xx,yy),R EVOK;)
primop256(minusID,0xa,,zz=_mm256_sub_pd(xx,yy),R EVOK;)
primop256(minusDB,0xa06,,zz=_mm256_sub_pd(xx,yy),R EVOK;)
primop256(minusBD,0x102,,zz=_mm256_sub_pd(xx,yy),R EVOK;)
primop256(minusII,0x22,__m256d oflo=_mm256_setzero_pd();,
 zz=_mm256_castsi256_pd(_mm256_sub_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))); oflo=_mm256_or_pd(oflo,_mm256_and_pd(_mm256_xor_pd(xx,yy),_mm256_xor_pd(xx,zz)));,
 R !_mm256_testc_pd(_mm256_setzero_pd(),oflo)?EWOVIP+EWOVIPMINUSII:EVOK;)  // ~0 & oflo, testc if =0 which means no overflow
primop256(minusBI,0x42,__m256d oflo=_mm256_setzero_pd();,
 zz=_mm256_castsi256_pd(_mm256_sub_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));oflo=_mm256_or_pd(oflo,_mm256_and_pd(zz,yy));,  // only oflo is 0 - imin
 R !_mm256_testc_pd(_mm256_setzero_pd(),oflo)?EWOVIP+EWOVIPMINUSBI:EVOK;)  // ~0 & oflo, testc if =0 which means no overflow
primop256(minusIB,0x882,__m256d oflo=_mm256_setzero_pd();,
 zz=_mm256_castsi256_pd(_mm256_sub_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))); oflo=_mm256_or_pd(oflo,_mm256_castsi256_pd(_mm256_cmpgt_epi64(_mm256_castpd_si256(zz),_mm256_castpd_si256(xx))));,
 R !_mm256_testc_pd(_mm256_setzero_pd(),oflo)?EWOVIP+EWOVIPMINUSIB:EVOK;)  // ~0 & oflo, testc if =0 which means no overflow
primop256(minusBB,0xc0,,
 zz=_mm256_castsi256_pd(_mm256_sub_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,R EVOK;)
primop256(minDI,0x16,,zz=_mm256_min_pd(xx,yy),R EVOK;)
// commutative primop256(minID,8,,zz=_mm256_min_pd(xx,yy),R EVOK;)
// commutative primop256(minBD,0x100,,zz=_mm256_min_pd(xx,yy),R EVOK;)
primop256(minDB,0x206,,zz=_mm256_min_pd(xx,yy),R EVOK;)
#if C_AVX512
primop256(minII,1,,   zz=_mm256_castsi256_pd(_mm256_min_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))),R EVOK;)
primop256(minIB,0x80,,zz=_mm256_castsi256_pd(_mm256_min_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))),R EVOK;)
// commutative primop256(minBI,0x40,,zz=_mm256_castsi256_pd(_mm256_min_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))),R EVOK;)
#else
primop256(minII,1,,
 zz=_mm256_castsi256_pd(BLENDVI(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy),_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)))); ,R EVOK;)
// commutative primop256(minBI,0x40,,
// commutative  zz=_mm256_castsi256_pd(BLENDVI(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy),_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)))); ,R EVOK;)
primop256(minIB,0x80,,
 zz=_mm256_castsi256_pd(BLENDVI(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy),_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)))); ,R EVOK;)
#endif
primop256(maxDI,0x16,,zz=_mm256_max_pd(xx,yy),R EVOK;)
primop256(maxDB,0x206,,zz=_mm256_max_pd(xx,yy),R EVOK;)
// commutative primop256(maxID,8,,zz=_mm256_max_pd(xx,yy),R EVOK;)
// commutative primop256(maxBD,0x100,,zz=_mm256_max_pd(xx,yy),R EVOK;)
#if C_AVX512
primop256(maxII,1,,   zz=_mm256_castsi256_pd(_mm256_max_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))),R EVOK;)
primop256(maxIB,0x80,,zz=_mm256_castsi256_pd(_mm256_max_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))),R EVOK;)
// commutative primop256(maxBI,0x40,,zz=_mm256_castsi256_pd(_mm256_max_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy))),R EVOK;)
#else
primop256(maxII,1,,
 zz=_mm256_castsi256_pd(BLENDVI(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx),_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)))); ,R EVOK;)
primop256(maxIB,0x80,,
 zz=_mm256_castsi256_pd(BLENDVI(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx),_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)))); ,R EVOK;)
// commutative primop256(maxBI,0x40,,
// commutative  zz=_mm256_castsi256_pd(BLENDVI(_mm256_castpd_si256(yy),_mm256_castpd_si256(xx),_mm256_cmpgt_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)))); ,R EVOK;)
#endif
// commutative primop256(tymesDI,0x10,D *zsav=z;NAN0;,zz=_mm256_mul_pd(xx,yy),if(unlikely(NANTEST)){z=zsav; DQ(n*m, if(_isnan(*(D*)z))*(D*)z=0.0; z=(C*)z+SZD;)} R EVOK;)
// commutative primop256(tymesDB,0x480,,zz=_mm256_and_pd(yy,xx),R EVOK;)
primop256(tymesID,0xa,D *zsav=z;NAN0;,zz=_mm256_mul_pd(xx,yy),if(unlikely(NANTEST)){z=zsav; DQ(n*m, if(_isnan(*(D*)z))*(D*)z=0.0; z=(C*)z+SZD;)} R EVOK;)
primop256(tymesBD,0x442,,zz=_mm256_and_pd(xx,yy),R EVOK;)
primop256(divDI,0x14,D *zsav=z; D *xsav=x; D *ysav=y; I msav=m;NAN0;,zz=_mm256_div_pd(xx,yy),
  if(unlikely(NANTEST)){ORIGMN z=zsav; xsav=zsav==ysav?xsav:ysav; m*=n; n=(nsav^SGNIF(zsav==ysav,0))>=0?n:1; nsav=--n; DQ(m, if(_isnan(*(D*)z)){ASSERTWR(*xsav==0,EVNAN); *(D*)z=0.0;} z=(C*)z+SZD; --n; xsav-=REPSGN(n); n=n<0?nsav:n;)} R EVOK;)
primop256(divII,0x1c,D *zsav=z; D *xsav=x; D *ysav=y; I msav=m;NAN0;,zz=_mm256_div_pd(xx,yy),
  if(unlikely(NANTEST)){ORIGMN z=zsav; xsav=zsav==ysav?xsav:ysav; m*=n; n=(nsav^SGNIF(zsav==ysav,0))>=0?n:1; nsav=--n; DQ(m, if(_isnan(*(D*)z)){ASSERTWR(*xsav==0,EVNAN); *(D*)z=0.0;} z=(C*)z+SZD; --n; xsav-=REPSGN(n); n=n<0?nsav:n;)} R EVOK;)
primop256(divBB,0x324,D *zsav=z; C *bsav=(C*)x; I msav=m; NAN0;,zz=_mm256_div_pd(xx,yy),  // x B->D, y B->D, no unroll, 0s at end
  if(unlikely(NANTEST)){ORIGMN z=zsav; m*=n; n=nsav<0?n:1; nsav=--n; DQ(m, if(_isnan(*(D*)z)){ASSERTWR(*bsav==0,EVNAN); *(D*)z=0.0;} z=(C*)z+SZD; --n; bsav-=REPSGN(n); n=n<0?nsav:n;)} R EVOK;)
primop256(divID,0xc,D *zsav=z; D *xsav=x; D *ysav=y; I msav=m;NAN0;,zz=_mm256_div_pd(xx,yy),
  if(unlikely(NANTEST)){ORIGMN z=zsav; xsav=zsav==ysav?xsav:ysav; m*=n; n=(nsav^SGNIF(zsav==ysav,0))>=0?n:1; nsav=--n; DQ(m, if(_isnan(*(D*)z)){ASSERTWR(*xsav==0,EVNAN); *(D*)z=0.0;} z=(C*)z+SZD; --n; xsav-=REPSGN(n); n=n<0?nsav:n;)} R EVOK;)
primop256(divDB,0x224,D *zsav=z; C *bsav=(C*)y; I msav=m;NAN0;,zz=_mm256_div_pd(xx,yy),
  if(unlikely(NANTEST)){ORIGMN z=zsav; m*=n; n=nsav>=0?n:1; nsav=--n; DQ(m, if(_isnan(*(D*)z)){ASSERTWR(*bsav==0,EVNAN); *(D*)z=0.0;} z=(C*)z+SZD; --n; bsav-=REPSGN(n); n=n<0?nsav:n;)} R EVOK;)
primop256(divIB,0x22c,D *zsav=z; C *bsav=(C*)y; I msav=m;NAN0;,zz=_mm256_div_pd(xx,yy),  // x I->D, y B->D, no unroll, 0s at end
  if(unlikely(NANTEST)){ORIGMN z=zsav; m*=n; n=nsav>=0?n:1; nsav=--n; DQ(m, if(_isnan(*(D*)z)){ASSERTWR(*bsav==0,EVNAN); *(D*)z=0.0;} z=(C*)z+SZD; --n; bsav-=REPSGN(n); n=n<0?nsav:n;)} R EVOK;)
primop256(divBD,0x104,D *zsav=z; C *bsav=(C*)x; I msav=m;NAN0;,zz=_mm256_div_pd(xx,yy),
  if(unlikely(NANTEST)){ORIGMN z=zsav; m*=n; n=nsav<0?n:1; nsav=--n; DQ(m, if(_isnan(*(D*)z)){ASSERTWR(*bsav==0,EVNAN); *(D*)z=0.0;} z=(C*)z+SZD; --n; bsav-=REPSGN(n); n=n<0?nsav:n;)} R EVOK;)
primop256(divBI,0x134,D *zsav=z; C *bsav=(C*)x; I msav=m;NAN0;,zz=_mm256_div_pd(xx,yy),  // x B->D, y I->D, no unroll, 0s at end
  if(unlikely(NANTEST)){ORIGMN z=zsav; m*=n; n=nsav<0?n:1; nsav=--n; DQ(m, if(_isnan(*(D*)z)){ASSERTWR(*bsav==0,EVNAN); *(D*)z=0.0;} z=(C*)z+SZD; --n; bsav-=REPSGN(n); n=n<0?nsav:n;)} R EVOK;)
#else
AIFX(minusDI, D,D,I, -) AIFX(minusID, D,I,D, -    ) 
// commutative APFX(  minID, D,I,D, MIN,,R EVOK;)
APFX(  minDI, D,D,I, MIN,,R EVOK;)
// commutative  APFX(  maxID, D,I,D, MAX,,R EVOK;)
APFX(  maxDI, D,D,I, MAX,,R EVOK;) 
APFX(tymesID, D,I,D, TYMESID,,R EVOK;)
// commutative APFX(tymesDI, D,D,I, TYMESDI,,R EVOK;)
 APFX(  divID, D,I,D, DIV,,R EVOK;) APFX(  divDI, D,D,I, DIVI,,R EVOK;) 
 AIFX( plusDI, D,D,I, +)
// commutative  AIFX( plusID, D,I,D, +   )
// II add, noting overflow and leaving it, possibly in place
AHDR2(plusII,I,I,I){I u;I v;I w;I oflo=0;
 // overflow is (input signs equal) and (result sign differs from one of them)
 // If u==0, v^=u is always 0 & therefore no overflow
 if(m<0) DQUC(m, u=*x; v=*y; w= ~u; u+=v; *z=u; ++x; ++y; ++z; w^=v; v^=u; if(XANDY(w,v)<0)++oflo;)
 else if(m&1){m>>=1; DQU(n, u=*x++; I thresh = IMIN-u; if (u<=0){DQU(m, v=*y; if(v<thresh)++oflo; v=u+v; *z++=v; y++;)}else{DQU(m, v=*y; if(v>=thresh)++oflo; v=u+v; *z++=v; y++;)})}
 else{m>>=1; DQU(n, v=*y++; I thresh = IMIN-v; if (v<=0){DQU(m, u=*x; if(u<thresh)++oflo; u=u+v; *z++=u; x++;)}else{DQU(m, u=*x; if(u>=thresh)++oflo; u=u+v; *z++=u; x++;)})}
 R oflo?EWOVIP+EWOVIPPLUSII:EVOK;
// II subtract, noting overflow and leaving it, possibly in place
}
AHDR2(minusII,I,I,I){I u;I v;I w;I oflo=0;
 // overflow is (input signs differ) and (result sign differs from minuend sign)
 if(m<0){DQUC(m, u=*x; v=*y; w=u-v; *z=w; ++x; ++y; ++z; v^=u; u^=w; if(XANDY(u,v)<0)++oflo;)}
// if u<0, oflo if u-v < IMIN => v > u-IMIN; if u >=0, oflo if u-v>IMAX => v < u+IMIN+1 => v <= u+IMIN => v <= u-IMIN
 else if(m&1){m>>=1; DQU(n, u=*x++; I thresh = u-IMIN; if (u<0){DQU(m, v=*y; if(v>thresh)++oflo; w=u-v; *z++=w; y++;)}else{DQU(m, v=*y; if(v<=thresh)++oflo; w=u-v; *z++=w; y++;)})}
 // if v>0, oflo if u-v < IMIN => u < v+IMIN = v-IMIN; if v<=0, oflo if u-v > IMAX => u>v+IMAX => u>v-1-IMIN => u >= v-IMIN
 else{m>>=1; DQU(n, v=*y++; I thresh = v-IMIN; if (v<=0){DQU(m, u=*x; if(u>=thresh)++oflo; u=u-v; *z++=u; x++;)}else{DQU(m, u=*x; if(u<thresh)++oflo; u=u-v; *z++=u; x++;)})}
 R oflo?EWOVIP+EWOVIPMINUSII:EVOK;
}
APFX(  minII, I,I,I, MIN,,R EVOK;)
APFX(  maxII, I,I,I, MAX,,R EVOK;)
// commutative APFX(  maxBI, I,B,I, MAX,,R EVOK;)
// commutative APFX(  maxBD, D,B,D, MAX,,R EVOK;)    
// IB add, noting overflow and leaving it, possibly in place.  If we add 0, copy the numbers (or leave unchanged, if in place)
AHDR2(plusIB,I,I,B){I u;I v;I oflo=0;
 if(m<0)  DQUC(m, u=*x; v=(I)*y; if(u==IMAX)oflo+=v; u=u+v; *z++=u; x++; y++; )
 else if(m&1){m>>=1; DQU(n, u=*x++; DQU(m, v=(I)*y; if(u==IMAX)oflo+=v; v=u+v; *z++=v; y++;))}
 else{m>>=1; DQU(n, v=(I)*y++; if(v){DQU(m, u=*x; if(u==IMAX)oflo=1; u=u+1; *z++=u; x++;)}else{if(z!=x)MC(z,x,(m)<<LGSZI); z+=m; x+=m;})}
 R oflo?EWOVIP+EWOVIPPLUSIB:EVOK;
}
// commutative // BI add, noting overflow and leaving it, possibly in place.  If we add 0, copy the numbers (or leave unchanged, if in place)
// commutative AHDR2(plusBI,I,B,I){I u;I v;I oflo=0;
// commutative  if(m<0)  DQUC(m, u=(I)*x; v=*y; if(v==IMAX)oflo+=u; v=u+v; *z++=v; x++; y++; )
// commutative  else if(m&1){m>>=1; DQU(n, u=(I)*x++; if(u){DQU(m, v=*y; if(v==IMAX)oflo=1; v=v+1; *z++=v; y++;)}else{if(z!=y)MC(z,y,(m)<<LGSZI); z+=(m); y+=(m);})}
// commutative  else{m>>=1; DQU(n, v=*y++; DQU(m, u=(I)*x; if(v==IMAX)oflo+=u; u=u+v; *z++=u; x++;))}
// commutative  R oflo?EWOVIP+EWOVIPPLUSBI:EVOK;
// commutative }
// BI subtract, noting overflow and leaving it, possibly in place.  If we add 0, copy the numbers (or leave unchanged, if in place)
AHDR2(minusBI,I,B,I){I u;I v;I w;I oflo=0;
 if(m<0)  DQUC(m, u=(I)*x; v=*y; u=u-v; if((v&u)<0)++oflo; *z++=u; x++; y++; )
 else if(m&1){m>>=1; DQU(n, u=(I)*x++; DQU(m, v=*y; w=u-v; if((v&w)<0)++oflo; *z++=w; y++;))}
 else{m>>=1; DQU(n, v=*y++; DQU(m, u=(I)*x; u=u-v; if((v&u)<0)++oflo; *z++=u; x++;))}
 R oflo?EWOVIP+EWOVIPMINUSBI:EVOK;
}
// IB subtract, noting overflow and leaving it, possibly in place.  If we add 0, copy the numbers (or leave unchanged, if in place)
AHDR2(minusIB,I,I,B){I u;I v;I w;I oflo=0;
 if(m<0)  DQUC(m, u=*x; v=(I)*y; if(u==IMIN)oflo+=v; u=u-v; *z++=u; x++; y++; )
 else if(m&1){m>>=1; DQU(n, u=*x++; DQU(m, v=(I)*y; if(u==IMIN)oflo+=v; w=u-v; *z++=w; y++;))}
 else{m>>=1; DQU(n, v=(I)*y++; if(v){DQU(m, u=*x; if(u==IMIN)oflo=1; u=u-1; *z++=u; x++;)}else{if(z!=x)MC(z,x,(m)<<LGSZI); z+=m; x+=m;})}
 R oflo?EWOVIP+EWOVIPMINUSIB:EVOK;
}
// BD multiply, using clear/copy
AHDR2(tymesBD,D,B,D){
if(m<0)  DQUC(m, D *yv=(D*)&dzero; yv=*x?y:yv; *z++=*yv; x++; y++; )
 else if(m&1){m>>=1; DQU(n, B u=*x++; if(u){if(z!=y)MC(z,y,(m)*sizeof(D));}else{mvc((m)*sizeof(D),z,MEMSET00LEN,MEMSET00);} z+=m; y+=m;)}
 else{m>>=1; DQU(n, DQU(m, D *yv=(D*)&dzero; yv=*x?y:yv; *z++=*yv; x++;) ++y;)}
  R EVOK;
}
// DB multiply, using clear/copy
// commutative AHDR2(tymesDB,D,D,B){
// commutative  if(m<0)  DQUC(m, D *yv=(D*)&dzero; yv=*y?x:yv; *z++=*yv; x++; y++; )
// commutative  else if(m&1){m>>=1; DQU(n, DQU(m, D *yv=(D*)&dzero; yv=*y?x:yv; *z++=*yv; y++;) ++x;)}
// commutative  else{m>>=1; DQU(n, B v=*y++; if(v){if(z!=x)MC(z,x,(m)*sizeof(D));}else{mvc((m)*sizeof(D),z,MEMSET00LEN,MEMSET00);} z+=m; x+=m;)}
// commutative  R EVOK;
// commutative }
#if !SY_64  // boolean multiply is the same for float and double
// BI multiply, using clear/copy
AHDR2(tymesBI,I,B,I){I v;
 if(m<0)  DQUC(m, I u=*x; *z++=*y&-u; x++; y++; )
 else if(m&1){m>>=1; DQU(n, B u=*x++; if(u){if(z!=y)MC(z,y,(m)<<LGSZI);}else{mvc((m)<<LGSZI,z,MEMSET00LEN,MEMSET00);} z+=m; y+=m;)}
 else{m>>=1;  DQU(n, v=*y++; DQU(m, I u=*x; *z++=v&-u; x++;))}
 R EVOK;
}
// IB multiply, using clear/copy
AHDR2(tymesIB,I,I,B){I u;
 if(m<0)  DQUC(m, I v=*y; *z++=*x&-v; x++; y++; )
 else if(m&1){m>>=1; DQU(n, u=*x++; DQU(m, I v=*y; *z++=u&-v; y++;))}
 else{m>>=1; DQU(n, B v=*y++; if(v){if(z!=x)MC(z,x,(m)<<LGSZI);}else{mvc((m)<<LGSZI,z,MEMSET00LEN,MEMSET00);} z+=m; x+=m;)}
 R EVOK;
}
#endif
// commutative AIFX( plusBD, D,B,D, +   )
AIFX( plusDB, D,D,B, +     )       /* plusDD */
AIFX(minusBD, D,B,D, -    )
AIFX(minusDB, D,D,B, -     )        /* minusDD */
APFX(  divBD, D,B,D, DIV,,R EVOK;)
APFX(  divDB, D,D,B, DIVI ,,R EVOK;)         /* divDD */
APFX(  minIB, I,I,B, MIN,,R EVOK;)     /* minII */                   
APFX(  minDB, D,D,B, MIN,,R EVOK;)           /* minDD */
// commutative APFX(  minBI, I,B,I, MIN,,R EVOK;)
// commutative APFX(  minBD, D,B,D, MIN,,R EVOK;)    
APFX(  maxIB, I,I,B, MAX,,R EVOK;)     /* maxII */                    
APFX(  maxDB, D,D,B, MAX,,R EVOK;)            /* maxDD */
APFX(  divBB, D,B,B, DIVBB,,R EVOK;) 
AIFX(minusBB, I,B,B, -     )    /* minusBI */            
  APFX(  divBI, D,B,I, DIVI,,R EVOK;) 
APFX(  divIB, D,I,B, DIVI ,,R EVOK;)
   APFX(  divII, D,I,I, DIVI,,R EVOK;)    
AIFX( plusBB, I,B,B, +     )    /* plusBI */
#endif
APFX(minI2I2, I2,I2,I2, MIN,,R EVOK;)
APFX(minI4I4, I4,I4,I4, MIN,,R EVOK;)
APFX(maxI2I2, I2,I2,I2, MAX,,R EVOK;)
APFX(maxI4I4, I4,I4,I4, MAX,,R EVOK;)
#define OFOPTEST(op) if(unlikely(__builtin_##op##_overflow(u,v,&zz)))R EVOFLO;
AEXP(plusI2I2, I2,I2,I2,OFOPTEST(add))
AEXP(plusI4I4, I4,I4,I4,OFOPTEST(add))
AEXP(minusI2I2, I2,I2,I2,OFOPTEST(sub))
AEXP(minusI4I4, I4,I4,I4,OFOPTEST(sub))
AEXP(tymesI2I2, I2,I2,I2,OFOPTEST(mul))
AEXP(tymesI4I4, I4,I4,I4,OFOPTEST(mul))


// II multiply, in double precision.  Always return error code so we can clean up
AHDR2(tymesII,I,I,I){DPMULDECLS I u;I v;I *zi=z;   // could use a side channel to avoid having main loop look at rc
 if(m<0) DQUC(m, u=*x; v=*y; DPMUL(u,v,z, goto oflo;) z++; x++; y++; )
 else if(m&1){m>>=1; DQU(n, u=*x; DQU(m, v=*y; DPMUL(u,v,z, goto oflo;) z++; y++;) x++;)}
 else{m>>=1; DQU(n, v=*y; DQU(m, u=*x; DPMUL(u,v,z, goto oflo;) z++; x++;) y++;)}
 R EVOK;
oflo: *x=u; *y=v; R ~(z-zi);  // back out the last store, in case it's in-place; gcc stores before overflow.  Return complement of overflow offset as special signal
}

// Overflow repair routines.  These use the old interpretation of m/n (m=outer or only, n=inner)
// *x is a non-in-place argument, and n and m advance through it
//  each atom of *x is repeated n times
// *y is the I result of the operation that overflowed
// *z is the D result area (which might be the same as *y)
// b is unused for plus
AHDR2(plusIIO,D,I,I){I u; I absn=n^REPSGN(n);
 DQ(m, u=*x++; DQ(absn, *z=(D)u + (D)(*y-u); ++y; ++z;));
 R EVOK;
}
AHDR2(plusBIO,D,B,I){I u; I absn=n^REPSGN(n);
 DQ(m, u=(I)*x++; DQ(absn, *z=(D)u + (D)(*y-u); ++y; ++z;));
 R EVOK;
}

// For subtract repair, b is 1 if x was the subtrahend, 0 if the minuend
AHDR2(minusIIO,D,I,I){I u; I absn=n^REPSGN(n);
 DQ(m, u=*x++; DQ(absn, *z=n<0?((D)(*y+u)-(D)u):((D)u - (D)(u-*y)); ++y; ++z;));
 R EVOK;
}
AHDR2(minusBIO,D,B,I){I u; I absn=n^REPSGN(n);
 DQ(m, u=(I)*x++; DQ(absn, *z=n<0?((D)(*y+u)-(D)u):((D)u - (D)(u-*y)); ++y; ++z;));
 R EVOK;
}

// In multiply repair, z points to result, x and y to inputs
// Parts of z before mulofloloc have been filled in already
// We have to track the inputs just as for any other action routine
I tymesIIO(I n,I m,I* RESTRICTI x,I* RESTRICTI y,D* RESTRICTI z,I skipct){I u,v; I absn=n^REPSGN(n);
 // if all the multiplies are to be skipped, skip them quickly
 if(skipct<m*absn){
  // There are unskipped multiplies.  Do them.
  if(n-1==0)  DQ(m, u=*x; v=*y; if(--skipct<0){*z=(D)u * (D)v;} z++; x++; y++; )
  else if(n-1<0)DQ(m, u=*x++; DQC(n, v=*y; if(--skipct<0){*z=(D)u * (D)v;} z++; y++;))
  else DQ(m, v=*y++; DQ(n, u=*x; if(--skipct<0){*z=(D)u * (D)v;} z++; x++;))
 }
 R EVOK;
}


// All these lines define functions for various operand combinations
// The comments indicate special cases that are defined by verbs that don't follow the
// AOVF/AIFX/ANAN etc. template


// These routines are used by sparse processing, which doesn't do in-place overflow
APFX( plusIO, D,I,I,  PLUSO,,R EVOK;)
APFX(minusIO, D,I,I, MINUSO,,R EVOK;)
APFX(tymesIO, D,I,I, TYMESO,,R EVOK;)

#if C_AVX2 || EMU_AVX2
// complex arithmetic

#define PREFZNEG(lo,hi) lo=_mm256_xor_pd(lo,sgnbit); hi=_mm256_xor_pd(hi,sgnbit); 
primop256CE(plusZZ,2,Z,NAN0;,PREFNULL,PREFNULL,{z0=_mm256_add_pd(x0,y0); z1=_mm256_add_pd(x1,y1);},ASSERTWR(!NANTEST,EVNAN);)
primop256CE(minusZZ,3,Z,__m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin); NAN0;,PREFNULL,PREFNULL,{z0=_mm256_sub_pd(x0,y0); z1=_mm256_sub_pd(x1,y1);},ASSERTWR(!NANTEST,EVNAN);)
// multiplication
#define PREFCMPTO0Z(in0,in1) in0##non0=_mm256_cmp_pd(in0,_mm256_setzero_pd(),_CMP_NEQ_OQ); \
in1##non0=_mm256_cmp_pd(in1,_mm256_setzero_pd(),_CMP_NEQ_OQ);

#define MULZ { \
 z0=_mm256_fmsub_pd(_mm256_and_pd(x0,y0non0),_mm256_and_pd(y0,x0non0),_mm256_mul_pd(_mm256_and_pd(x1,y1non0),_mm256_and_pd(y1,x1non0)));  /* real */ \
 z1=_mm256_fmadd_pd(_mm256_and_pd(x1,y0non0),_mm256_and_pd(y0,x1non0),_mm256_mul_pd(_mm256_and_pd(x0,y1non0),_mm256_and_pd(y1,x0non0)));  /* imag */ \
}

primop256CE(tymesZZ,1,Z,__m256d x0non0; __m256d x1non0; __m256d y0non0; __m256d y1non0; NAN0;,PREFCMPTO0Z,PREFCMPTO0Z,MULZ,ASSERTWR(!NANTEST,EVNAN);)

// division
// the tricky part of taking the reciprocal of a complex number is avoiding overflow on the magnitude.
// we create a power-of-2 multiplier that will move the value towards 1, and scale both parts by that amount; then
// we take the magnitude^2 without overflow, scale the parts again, and divide to get the reciprocal
// division by 0 is also tricky, as we must avoid NaN errors along the way
#define RECIPZ(re,im) { \
__m256i max=_mm256_max_epu32(_mm256_castpd_si256(_mm256_andnot_pd(sgnbit,re)),_mm256_castpd_si256(_mm256_andnot_pd(sgnbit,im)));  /* max absolute value */ \
/* exponent range [0,1]*fullscale is mapped into multiplier of [7/8,1/8]*fullscale which leaves the multiplied exponent  */ \
/* in the range [3/8,5/8]  which will not overflow when squared or corrected again.  mplr = 7/8+1/4*exp-exp */ \
/* we use multiply to modify the exponent so that 0 and inf will not change */ \
__m256d temp7ffd=_mm256_broadcast_sd((D*)&(I){0x7ff0000000000000}); \
__m256i temp6eei=_mm256_castpd_si256(_mm256_broadcast_sd((D*)&(I){0x6ee0000000000000})); \
__m256d mplr=_mm256_and_pd(temp7ffd,_mm256_castsi256_pd(_mm256_add_epi64(temp6eei,_mm256_sub_epi64(_mm256_srli_epi64(max,2),max)))); \
/* we use multiply to modify the exponent so that 0 and inf will not change */ \
y0=_mm256_mul_pd(mplr,y0); y1=_mm256_mul_pd(mplr,y1);  /* apply first correction */ \
__m256d denom=_mm256_mul_pd(y0,y0); denom=_mm256_fmadd_pd(y1,y1,denom);  /* mag^2 of corrected cmplx */ \
denomis0=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(denom),_mm256_setzero_si256()));  /* is denom 0? */ \
denom=_mm256_blendv_pd(denom,temp7ffd,denomis0); /* if denom=0, set to non0 so that result 0/denom is 0 with no NaN error */ \
re=_mm256_mul_pd(mplr,re); im=_mm256_mul_pd(mplr,im);  /* apply second correction */ \
__m256d denomisinf=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(denom),_mm256_castpd_si256(temp7ffd)));  /* is denom +inf? */ \
re=_mm256_blendv_pd(re,_mm256_setzero_pd(),denomisinf); /* if denom inf, clear numerator in case it is _ also.  denom is inf^2 */ \
im=_mm256_blendv_pd(im,_mm256_setzero_pd(),denomisinf);\
re=_mm256_div_pd(re,denom); im=_mm256_div_pd(im,denom);  /* CONJUGATE of reciprocal */ \
y0non0=_mm256_or_pd(_mm256_cmp_pd(y0,_mm256_setzero_pd(),_CMP_NEQ_OQ),denomisinf); /* if non0, allow NaN on _*0; always if denom is _ */ \
y1non0=_mm256_or_pd(_mm256_cmp_pd(y1,_mm256_setzero_pd(),_CMP_NEQ_OQ),denomisinf); \
}

// y0 and y1 have the CONJUGATE of reciprocal of the divisor; denomis0 is set for each lane if denom was 0
#define DIVZ {  \
if(likely(_mm256_testz_pd(denomis0,denomis0))){ /* no 0 divisors */ \
 z0=_mm256_fmadd_pd(_mm256_and_pd(x0,y0non0),y0,_mm256_mul_pd(_mm256_and_pd(x1,y1non0),y1));  /* real */ \
 z1=_mm256_fmsub_pd(_mm256_and_pd(x1,y0non0),y0,_mm256_mul_pd(_mm256_and_pd(x0,y1non0),y1));  /* imag */ \
}else{__m256d num0,num1;  /* there is a zero divisor - do the paths separately and combine */ \
 num0=_mm256_andnot_pd(denomis0,x0); num1=_mm256_andnot_pd(denomis0,x1); /* convert nums at 0 to 0 */ \
 z0=_mm256_fmadd_pd(_mm256_and_pd(x0,y0non0),y0,_mm256_mul_pd(_mm256_and_pd(x1,y1non0),y1));  /* real */ \
 z1=_mm256_fmsub_pd(_mm256_and_pd(x1,y0non0),y0,_mm256_mul_pd(_mm256_and_pd(x0,y1non0),y1));  /* imag */ \
 __m256d temp=_mm256_broadcast_sd((D*)&inf);  \
 z0=_mm256_blendv_pd(z0,_mm256_and_pd(_mm256_cmp_pd(x0,_mm256_setzero_pd(),_CMP_NEQ_OQ),_mm256_or_pd(temp,_mm256_and_pd(sgnbit,x0))),denomis0); /* if not 0, set to inf with sign */ \
 z1=_mm256_blendv_pd(z1,_mm256_and_pd(_mm256_cmp_pd(x1,_mm256_setzero_pd(),_CMP_NEQ_OQ),_mm256_or_pd(temp,_mm256_and_pd(sgnbit,x1))),denomis0); /* if not 0, set to inf with sign */ \
} \
}

primop256CE(divZZ,1,Z,__m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin); __m256d y0non0; __m256d y1non0; NAN0; __m256d denomis0;,PREFNULL,RECIPZ,DIVZ,ASSERTWR(!NANTEST,EVNAN);)


// QP arithmetic.  We do not support infinities.  If you multiply one, it might give NaN
#define PLUSEEF {PLUSEE(x0,x1,y0,y1,z0,z1) CANONE(z0,z1)}

#define MINUSEE {__m256d t,t0,t1;\
t0=_mm256_sub_pd(x0,y0); t1=_mm256_sub_pd(t0,x0); \
t1=_mm256_sub_pd(_mm256_sub_pd(x0,_mm256_sub_pd(t0,t1)),_mm256_add_pd(y0,t1)); /* t1/t0 = x1-y1, QP */ \
t1=_mm256_add_pd(t1,_mm256_sub_pd(x1,y1));  /* accumulate lower significance */ \
z0=_mm256_add_pd(t1,t0); z1=_mm256_add_pd(t1,_mm256_sub_pd(t0,z0));  /* remove any overlap */ \
CANONE(z0,z1) \
}

// This version is good to 103 bits: lower pps might have 4x the weight of the upper
#if 1  // 103 bits
#define MULTEEF {MULTEE(x0,x1,y0,y1,z0,z1) CANONE(z0,z1)}

#else  // 106 bits
// We keep this valid to the last bit, which might be overkill
#define MULTEE {__m256d pp00, pp01, pp10, pp11; \
TWOPROD(x1,y1,pp00h,pp00l)  /* partial product 0 */ \
pp11=_mm256_mul_pd(x1,y1);  /* partial product 3 */ \
TWOPROD(x1,y0,pp01,t2) pp11=_mm256_add_pd(pp11,t2); /* pp1 */ \
TWOPROD(x0,y1,pp10,t2) pp11=_mm256_add_pd(pp11,t2); /* pp2 */ \
TWOSUM(pp00l,pp01,t1,t2) pp11=_mm256_add_pd(pp11,t2); /* pp0+pp1 */ \
TWOSUM(t1,pp10,pp01,t2) pp11=_mm256_add_pd(pp11,t2); /* pp0+pp1+pp2 */ \
TWOSUMBS(pp00,pp01,t1,t2) t2=_mm256_add_pd(pp11,t2);  /* create result 0 & 1, and propagate res2 into 1 */ \
TWOSUMBS(t1,t2,z1,z0) /* remove overlap */ \
CANONE(z1,z0)  /* canonicalize the extension */ \
}
#endif
primop256CE(plusEE,0,E,__m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin); __m256d mantmask=_mm256_broadcast_sd((D*)&(I){0x000fffffffffffff}); NAN0;,PREFNULL,PREFNULL,PLUSEEF,ASSERTWR(!NANTEST,EVNAN);)
primop256CE(minusEE,1,E,__m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin); __m256d mantmask=_mm256_broadcast_sd((D*)&(I){0x000fffffffffffff}); NAN0;,PREFNULL,PREFNULL,MINUSEE,ASSERTWR(!NANTEST,EVNAN);)
#if !(EMU_AVX2 && defined(__x86_64__))  // x86 emulator doesn't do fmsub right.  ARM is OK
primop256CE(tymesEE,0,E,__m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin); __m256d mantmask=_mm256_broadcast_sd((D*)&(I){0x000fffffffffffff}); NAN0;,PREFNULL,PREFNULL,MULTEEF,ASSERTWR(!NANTEST,EVNAN);)
#else
APFX( tymesEE, E,E,E, TYMESE,NAN0;,ASSERTWR(!NANTEST,EVNAN); R EVOK;)
#endif

#if 0  // obsolete  this template used to debug primop256CE
#define fz 0
#define CET E
#define cepref __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin); __m256d mantmask=_mm256_broadcast_sd((D*)&(I){0x000fffffffffffff}); NAN0; 
#define ceprefL PREFNULL  // replaced in main line
#define ceprefR PREFNULL  // replaced in main line
#define cesuff ASSERTWR(!NANTEST,EVNAN);
AHDR2(plusEE,CET,CET,CET){
 __m256d z0, z1, x0, x1, y0, y1, in0, in1;
 cepref
 /* convert vector args, which are the same size as z, to offsets from z; flag atom args */
 if(likely(n-1==0)){x=(CET*)((C*)x-(C*)z); y=(CET*)((C*)y-(C*)z);  /* vector op vector, both args offset */
 }else{  /* one arg is atom - flag addr and fetch repeated value.  m is #atom-vec loops, n is length of each and switch flag */
  {I taddr=(I)x^(I)y; x=n-1>0?y:x; y=(CET*)((I)x^taddr);}  /* if repeated vector op atom, exchange to be atom op vector for ease of fetch */
  y=(CET*)((C*)y-(C*)z);  /* convert the full-sized y arg to offset form */
  x=(CET*)((I)x+1); if(fz&1){x=(CET*)((I)x+(~REPSGN(n)&2));}  /* flag x: atom in bit 0, swapped in bit 1 */
  I t=m; m=n^REPSGN(n); n=t; /* convert vec len to positive, move to m; move outer loop count to n */
atomveclp: ;  /* come back here to do next atom op vector loop, with z running */
  /* read the repeated value and convert to internal form */
  if(!(fz&1)){x0=_mm256_broadcast_sd((D*)((I)x&-4)), x1=_mm256_broadcast_sd((D*)((I)x&-4)+1);
  }else{if((I)x&2){y0=_mm256_broadcast_sd((D*)((I)x&-4)), y1=_mm256_broadcast_sd((D*)((I)x&-4)+1); ceprefR(y0,y1)}else{x0=_mm256_broadcast_sd((D*)((I)x&-4)), x1=_mm256_broadcast_sd((D*)((I)x&-4)+1); ceprefL(x0,x1)}}  /* do LR processing for noncommut */
 }
 /* loop n times - usually once, but may be repeated for each atom.  The loop is by branch back to atomveclp */

 /* The loop is split into 3 parts: prefix/body/suffix.  The prefix gets z onto a cacheline boundary; the */
 /* body processes full cachelines; the suffix finishes.  Prefix/suffix use masked stores. */
 /* Here we calculate length of prefix and body+suffix.  We then encode them into one value. */
 /* We keep a mask for the current part */
 I len0=-(I)z>>(LGSZI+1);  /* ...aa amt to proc to get to 2cacheline bdy */ 
 len0=m<8?m:len0;  /* if short, switch len0 to full length to reduce passes through op */
 len0&=NPAR-1;  /* prefix len: if long, to get to bdy; if short, to leave last block exactly NPAR or 0 */
 /* get mask for first read/write: same 2-bit values in lanes 01, and the other 2 bits in 23 */
 __m256i wrmask=_mm256_castps_si256(_mm256_permutevar_ps(_mm256_castpd_ps(_mm256_broadcast_sd((D*)&maskec4123[len0])),_mm256_loadu_si256((__m256i*)&validitymask[2])));
 I len1=m+((4|-len0)<<(BW-3));    /* make len1 negative so we set new masks for the body.  We can recover len0 from len1.  We do this even if len0=0 to avoid misbranches */

 /* loop m times, for each operation */
rdmasklp: ;  /* here when we must read the new args under mask */

 /* read any nonrepeated argument, shuffle */
 I totallen=len1&((1LL<<(BW-3))-1);  /* total remaining length */
 I zinc=(totallen>2)<<(LGNPAR+LGSZI);  /* offset to second half of input, if it is valid */
 if(likely(!((I)x&1))){  /* if x is not repeated... */
  in0=_mm256_maskload_pd((D*)((C*)z+(I)x),wrmask), in1=_mm256_maskload_pd((D*)((C*)z+(I)x+zinc),_mm256_slli_epi64(wrmask,1));
  SHUFIN(fz,in0,in1,x0,x1);  /* convert to llll hhhh form */
  if(fz&1){ceprefL(x0,x1)}  /* do LR processing for noncommut */
 }
 /* always read the y arg */
 in0=_mm256_maskload_pd((D*)((C*)z+(I)y),wrmask), in1=_mm256_maskload_pd((D*)((C*)z+(I)y+zinc),_mm256_slli_epi64(wrmask,1));

mainlp:  /* here when args have already been read.  x has been converted & prefixed; y not, it is in in0/1 */
 if(!(fz&1)){SHUFIN(fz,in0,in1,y0,y1)}  /* convert y, which is always read, to llll hhhh form */
 else{if((I)x&2){SHUFIN(fz,in0,in1,x0,x1) ceprefL(x0,x1)}else{
 SHUFIN(fz,in0,in1,y0,y1)
// ceprefR here
// end ceprefR
}  /* do LR processing for noncommut */
}

// zzop here
{__m256d t,t0,t1;
t0=_mm256_add_pd(x0,y0); t1=_mm256_sub_pd(t0,x0); 
t1=_mm256_add_pd(_mm256_sub_pd(x0,_mm256_sub_pd(t0,t1)),_mm256_sub_pd(y0,t1)); /* t1/t0 = x0+y0, QP */ 
t1=_mm256_add_pd(t1,_mm256_add_pd(x1,y1));  /* accumulate lower significance */ 
z0=_mm256_add_pd(t1,t0); z1=_mm256_add_pd(t1,_mm256_sub_pd(t0,z0));  /* remove any overlap */ 
CANONE(z0,z1) 
}// end zzop

 SHUFOUT(fz,z0,z1);  /* put result into interleaved form for writing */
 /* write out the result and loop */
 if(len1>=2*NPAR){
  /* the NEXT batch can be written out in full (and so can this one).  Write the result, read new args and shuffle, and loop quickly */
  _mm256_storeu_pd((D*)z,z0); _mm256_storeu_pd((D*)z+NPAR,z1);   /* write out */
  z=(CET*)((I)z+2*NPAR*SZI); len1-=NPAR;  /* advance to next batch */
rdlp: ;  /* come here to fetch next batch & store it without masking */
  if(likely(!((I)x&1))){  /* if x is not repeated... */
   in0=_mm256_loadu_pd((D*)((C*)z+(I)x)), in1=_mm256_loadu_pd((D*)((C*)z+(I)x)+NPAR);
   SHUFIN(fz,in0,in1,x0,x1);  /* convert to llll hhhh form */
   if(fz&1){ceprefL(x0,x1)}  /* do L processing for noncommut - value was not swapped */
  }
  /* always read the y arg */
  in0=_mm256_loadu_pd((D*)((C*)z+(I)y)), in1=_mm256_loadu_pd((D*)((C*)z+(I)y)+NPAR);
  goto mainlp;
 }else if(len1>=NPAR){  /* next-to-last, or possibly last, batch */
  /* the next batch must be masked.  This one is OK; write the result, set the new mask, go back to read under mask */
  _mm256_storeu_pd((D*)z,z0); _mm256_storeu_pd((D*)z+NPAR,z1);   /* write out */
  z=(CET*)((I)z+2*NPAR*SZI); len1-=NPAR;  /* advance to next batch */
  if(len1!=0)goto rdmasklp;  /* process nonempty last batch, under mask, which has already been set */
  /* if len is 0, fall through to loop exit */
 }else{
  /* The current batch must write under mask.  Do so, and continue as called for, to body, suffix, or exit */
  /* The length of this batch comes from len0 or len1 */
  len0=-(len1>>(BW-3));   /* extract len0 from combined len0/len1, range 1 to 4, or 0 if not first batch */
  len0=len1<0?len0:len1;  /* len0=length of batch: len0 (first batch) or len1 (others) */
  len1&=((1LL<<(BW-3))-1); /* discard len0 from le ngth remaining */
  I zinc=(len0>2)<<(LGNPAR+LGSZI);  /* offset to second half of result, if it can be written */
  _mm256_maskstore_pd((D*)((C*)z+zinc),_mm256_slli_epi64(wrmask,1),z1);
  _mm256_maskstore_pd((D*)(z),wrmask,z0);
  z=(CET*)((I)z+(len0<<(LGSZI+1))); len1-=len0;  /* advance to next batch */
  if(len1!=0){  /* z is advanced.  Continue if there is more to do */
   /* set the mask for the last batch.  Unless m is 5-8, this will not hold anything up */
   wrmask=_mm256_castps_si256(_mm256_permutevar_ps(_mm256_castpd_ps(_mm256_broadcast_sd((D*)&maskec4123[len1&(NPAR-1)])),_mm256_loadu_si256((__m256i*)&validitymask[2])));
   if(likely(len1>=NPAR))goto rdlp; else goto rdmasklp;  /* process nonempty next batch, under mask if it is the last one */
  }
  /* fall through to loop exit if len hit 0 */ 
 }
 /* this is the exit from the loop, possibly reached by fallthrough */

 /* end of one vector operation.  If there are multiple atom*vector, loop */
 if(unlikely(--n!=0)){++x; goto atomveclp;}  /* if multiple atom*vec, move to next atom.  z/y stay in step */
 cesuff
 R EVOK;
}
#endif

#if 0 // obsolete This template used to debug APFX
 I divEE(I n,I m,E* RESTRICTI x,E* RESTRICTI y,E* RESTRICTI z,J jt){E u;E v;
  NAN0;
  // this supports only scalar op scalar
  u = *x; v=*y;
  D zh,one,d,H=1.0/(v).hi;
  TWOPROD1(H,(v).hi,one,d);
  one-=1.0; d+=one;
  d*=(v).hi;
  d+=(v).lo;
  d*=H; d*=-H;
  TWOSUMBS1(H,d,zh,H);
  v=(E){.hi=zh,.lo=H}; 

  D h,l,t,xx;
  if(u.hi!=0. && v.hi!=0.){
   TWOPROD1(u.hi,v.hi,h,l);
   TWOPROD1(u.hi,v.lo,t,xx);
   l+=t; TWOPROD1(u.lo,v.hi,t,xx);
   l+=t; TWOSUMBS1(h,l,t,h);}
  else t=h=0.;
  *z=CANONE1(t,h);

  ASSERTWR(!NANTEST,EVNAN); R EVOK;
 }

#endif


#else
   /* plusIB */                 /* plusII */                
APFX( plusZZ, Z,Z,Z, zplus,NAN0;,ASSERTWR(!NANTEST,EVNAN); R EVOK; )

  /* minusIB */                 /* minusII */               
APFX(minusZZ, Z,Z,Z, zminus,NAN0;,ASSERTWR(!NANTEST,EVNAN); R EVOK;)
    /* tymesIB */               /* tymesII */                
    /* tymesDB */                /* tymesDD */ 
APFX(tymesZZ, Z,Z,Z, ztymes,NAN0;,ASSERTWR(!NANTEST,EVNAN); R EVOK; )
APFX(  divZZ, Z,Z,Z, zdiv,NAN0;,HDR1JERRNAN  )
APFX( tymesEE, E,E,E, TYMESE,NAN0;,ASSERTWR(!NANTEST,EVNAN); R EVOK;)

 #endif
   /* andBB */                 /* tymesBI */                   /* tymesBD */            

     /* orBB */
APFX(  minSS, SB,SB,SB, SBMIN,,R EVOK;)

    /* andBB */              
APFX(  maxSS, SB,SB,SB, SBMAX,,R EVOK;)

D jtremdd(J jt,D a,D b){D q,x,y;
 if(!a)R b;
 ASSERT(!INF(b),EVNAN);
 if(a==inf )R 0<=b?b:a;
 if(a==infm)R 0>=b?b:a;
 q=b/a; x=tfloor(q); y=tceil(q); R TEQ(x,y)?0:b-a*x;
}

APFX(remDD, D,D,D, remdd,,HDR1JERR)
APFX(remZZ, Z,Z,Z, zrem ,,HDR1JERR)

I jtremid(J jt,I a,D b){D r;I k;
 ASSERT(a&&-9e15<b&&b<9e15,EWOV);
 r=b-a*jfloor(b/a); k=(I)r;
 ASSERT(k==r,EWOV);   // not really overflow - just has a fractional part
 R k;
}

APFX(remID, I,I,D, remid,,HDR1JERR)

I remii(I a,I b){I r; R (a!=REPSGN(a))?(r=b%a,0<a?r+(a&REPSGN(r)):r+(a&REPSGN(-r))):b&~a;}  // must handle IMIN/-1, which overflows.  If a=0, return b.

AHDR2(remII,I,I,I){I u,v;
 if(m<0){DQUC(m,*z++=remii(*x,*y); x++; y++; )
 }else if(m&1){m>>=1;   // repeated x.  Handle special cases and avoid integer divide
#if SY_64 && C_USEMULTINTRINSIC
  DQU(n, u=*x++;
    // take abs(x); handle negative x in a postpass
   UI ua=-u>=0?-u:u;  // abs(x)
   if(!(ua&(ua-1))){ I umsk = ua-1; bw0001II AH2A_x1(m,y,&umsk,z,jt); z+=m; y+=m;   // x is a power of 2, including 0
   }else{
    // calculate 1/abs(x) to 53-bit precision.  Remember, x is at least 3, so the MSB will never have signed significance
    UI uarecip = (UI)(18446744073709551616.0/(D)(I)ua);  // recip, with binary point above the msb.  2^64 / ua
    // add in correction for the remaining precision.  The result will still never be higher than the true reciprocal
    I deficitprec = -(I)(uarecip*ua);  // we need to increase uarecip by enough to add (deficitprec) units to (uarecip*ua)
    // because of rounding during the divide, deficitprec may be positive or negative, so we must 2's-comp-correct the product
    UI himul; DPUMULH(uarecip,(UI)deficitprec,himul); uarecip=deficitprec<0?0:uarecip; uarecip+=himul;   // now we have 63 bits of uarecip
    // Now loop through each input value.  It is possible that the quotient coming out of the multiplication will be
    // low by at most 1; we correct it if it is
    // The computations here are unsigned, because if signed the binary point gets offset and the upper significance requires a 128-bit shift.
    // Since negative arguments are unusual, we use 1 branch to handle them.  This may mispredict.
    DQU(m, I yv=*y;
      // Multiply by recip to get quotient, which is up to 1/2 LSB low; get remainder; adjust remainder if too high; store
      // 2's-complement adjust for negative y; to make the result still always on the low side, subtract an extra 1.
      DPUMULH(uarecip,(UI)yv,himul); himul-=(uarecip+1)&REPSGN(yv); I rem=yv-himul*ua; rem=(rem-(I)ua)>=0?rem-(I)ua:rem; *z++=rem;
     y++;)
   }
   // if x was negative, move the remainder into the x+1 to 0 range
   if(u<-1){I *zt=z; DQU(m, I t=*--zt; t=t>0?t-ua:t; *zt=t;)}
  )
#else
  DQU(n, u=*x++;
   if(0<=u&&!(u&(u-1))){--u; DQU(m, *z++=u&*y++;);}
   else DQU(m, *z++=remii( u,*y);      y++;)
  )
#endif
 }else{m>>=1; DQU(n, v=*y++; DQU(m, *z++=remii(*x, v); x++;     ))}  // repeated y
 R EVOK;
}

AHDR2(remI2I2,I2,I2,I2){I u,v;
 if(m<0){DQUC(m,*z++=remii(*x,*y); x++; y++; )
 }else if(m&1){m>>=1;    // repeated x.  Handle special cases and avoid integer divide
#if SY_64 && C_USEMULTINTRINSIC
  DQU(n, u=*x++;
   UI ua=-u>=0?-u:u;  // abs(x)
   if(!(ua&(ua-1))){I umsk = ua-1; DOU(m, z[i]=y[i]&umsk;) z+=m; y+=m;   // x is a power of 2, including 0
   }else{
    UI uarecip = (UI)(18446744073709551616.0/(D)(I)ua);  // recip, with binary point above the msb.  2^64 / ua
    I deficitprec = -(I)(uarecip*ua);  // we need to increase uarecip by enough to add (deficitprec) units to (uarecip*ua)
    UI himul; DPUMULH(uarecip,(UI)deficitprec,himul); uarecip=deficitprec<0?0:uarecip; uarecip+=himul;   // now we have 63 bits of uarecip
    DQU(m, I yv=*y;
      DPUMULH(uarecip,(UI)yv,himul); himul-=(uarecip+1)&REPSGN(yv); I rem=yv-himul*ua; rem=(rem-(I)ua)>=0?rem-(I)ua:rem; *z++=rem;
     y++;)
   }
   if(u<-1){I2 *zt=z; DQU(m, I2 t=*--zt; t=t>0?t-ua:t; *zt=t;)}
  )
#else
  DQU(n, u=*x++;
   if(0<=u&&!(u&(u-1))){--u; DQU(m, *z++=u&*y++;);}
   else DQU(m, *z++=remii( u,*y);      y++;)
  )
#endif
 }else{m>>=1; DQU(n, v=*y++; DQU(m, *z++=remii(*x, v); x++;     ))}  // repeated y
 R EVOK;
}

AHDR2(remI4I4,I4,I4,I4){I u,v;
 if(m<0){DQUC(m,*z++=remii(*x,*y); x++; y++; )
 }else if(m&1){m>>=1;    // repeated x.  Handle special cases and avoid integer divide
#if SY_64 && C_USEMULTINTRINSIC
  DQU(n, u=*x++;
   UI ua=-u>=0?-u:u;  // abs(x)
   if(!(ua&(ua-1))){I umsk = ua-1; DOU(m, z[i]=y[i]&umsk;) z+=m; y+=m;   // x is a power of 2, including 0
   }else{
    UI uarecip = (UI)(18446744073709551616.0/(D)(I)ua);  // recip, with binary point above the msb.  2^64 / ua
    I deficitprec = -(I)(uarecip*ua);  // we need to increase uarecip by enough to add (deficitprec) units to (uarecip*ua)
    UI himul; DPUMULH(uarecip,(UI)deficitprec,himul); uarecip=deficitprec<0?0:uarecip; uarecip+=himul;   // now we have 63 bits of uarecip
    DQU(m, I yv=*y;
      DPUMULH(uarecip,(UI)yv,himul); himul-=(uarecip+1)&REPSGN(yv); I rem=yv-himul*ua; rem=(rem-(I)ua)>=0?rem-(I)ua:rem; *z++=rem;
     y++;)
   }
   if(u<-1){I4 *zt=z; DQU(m, I4 t=*--zt; t=t>0?t-ua:t; *zt=t;)}
  )
#else
  DQU(n, u=*x++;
   if(0<=u&&!(u&(u-1))){--u; DQU(m, *z++=u&*y++;);}
   else DQU(m, *z++=remii( u,*y);      y++;)
  )
#endif
 }else{m>>=1; DQU(n, v=*y++; DQU(m, *z++=remii(*x, v); x++;))}  // repeated y
 R EVOK;
}


// 'binary gcd' algorithm, per Lemire https://lemire.me/blog/2013/12/26/fastest-way-to-compute-the-greatest-common-divisor/
// can be vectorised, annoying without hardware ctz (avx512 has clz which works)
I jtigcd(J jt,I a,I b){I d;UI4 s,xz,yz;UI x,y;
 // IMIN+.IMIN, IMIN+.0, and 0+.IMIN are |IMIN, which is not representable
 if(unlikely((a|b)==IMIN)){jt->jerr=EWOV; R 0;}
 // switch to UI so shifts are logical, in case of IMIN
 x=ABS(a);y=ABS(b);
 if(!x||!y)R x|y;  // if either value is 0, return absolute value of the other
 // The algorithm is as follows:
 // s=CTTZI(x|y) gives the number of factors of 2 shared between x and y
 // we save this, and remove that many factors of 2 from x and y, as well as all the remaining factors of 2, since they cannot contribute to the result
 // Then, compute gcd by repeated subtraction, except that for any intermediate value y, we can substitute y>>CTTZI(y), since (again) additional factors of 2 will not contribute
 // This is generally much faster than repeated division, largely because CTTZI is disproportionately cheap
 // Finally, restore the factors of 2 which were removed at the beginning
 xz=CTTZI(x); yz=CTTZI(y); s=CTTZI(x|y);
 x>>=xz;
 while(1){
  y>>=yz;
  d=y-x;  // Euclid's step
  yz=CTTZI(d);  // #LSB's is same for true & comp
  if(!d)break;
  if(d<0)x=y; // if x>y, (x,y)<-(y,x-y)
  y=ABS(d);   // if y>x, (x,y)<-(x,y-x)
 }
 R x<<s;
}

D jtdgcd(J jt,D a,D b){D a1,b1,t;B stop = 0;
 if(unlikely(_isnan(a)))R a;
 if(unlikely(_isnan(b)))R b;
 a=ABS(a); b=ABS(b); if(a>b){t=a; a=b; b=t;}
 ASSERT(inf!=b,EVNAN);
 if(!a)R b;
 a1=a; b1=b;
 while(remdd(a1/jround(a1/a),b1)){t=a; if((a=remdd(a,b))==0)break; b=t;}  // avoid infinite loop if a goes to 0
 R a;
}    /* D.L. Forkes 1984; E.E. McDonnell 1992 */
I jtilcm(J jt,I a,I b){I z;I d;
 if(a&&b){RZ(d=igcd(a,b)); DPMULDDECLS DPMULD(a,b/d,z,jt->jerr=EWOV; z=0;) R z;}else R 0;
}

#define GCDIO(u,v)      (dgcd((D)u,(D)v))
#define LCMIO(u,v)      (dlcm((D)u,(D)v))

D jtdlcm(J jt,D a,D b){ASSERT(!(INF(a)||INF(b)),EVNAN); R a&&b?a*(b/dgcd(a,b)):0;}

APFX(gcdIO, D,I,I, GCDIO,,HDR1JERR)
APFX(gcdII, I,I,I, igcd ,,HDR1JERR)
APFX(gcdDD, D,D,D, dgcd ,,HDR1JERR)
APFX(gcdZZ, Z,Z,Z, zgcd ,,HDR1JERR)

APFX(lcmII, I,I,I, ilcm ,,HDR1JERR)
APFX(lcmIO, D,I,I, LCMIO,,HDR1JERR)
APFX(lcmDD, D,D,D, dlcm ,,HDR1JERR)
APFX(lcmZZ, Z,Z,Z, zlcm ,,HDR1JERR)

// <.@% and >.@%

// All integers with magnitude <:2^53 are also floating-point numbers.
// Int division is much slower than float division (6 cycles vs 2)
// Therefore, for integers in this range, we would like to divide them as floats.  Will this give correct results, or could double rounding cause problems?
// Assume wlog that a>:0 and w>:2 (since w=1 will of course round correctly, and w=0 needs to be handled specially anyway).
// Let q, r denote the exact (floored) quotient and remainder.
// Since a<:2^53 and q<:a, q is exactly representable as a floating-point number.
// If r=0, then we are done, for division is faithfully rounded.
// Otherwise, it suffices to show that (<.a%w) < (round a%w) and (round a%w) < (>.a%w).
// But since r<:2^53, and since q<2^53 (and therefore has more low-level range), r%w and its complement must both be >1 ulp of q.
// Hence, a%w is at least 1 ulp away from the integers surrounding it, so it will round to a value (exclusively) between them.

// I'm pretty sure that we can do something with full 64-bit divides too.  Something like:
// q0 = (I)((D)n/(D)d)
// r0 = n - q0*d
// q1 = (float)r0/(float)d
// r1 = n - q1*d
// q = q0 + q1 + (r1/d)
// (Yes, single-prec float.  It should always have enough bits to accomodate r0; perhaps not d, but fine.  And singles are faster than doubles.)
// Where r1/d can be calculated by repeated subtraction.  The annoying thing I still have to work out here: can we bound r1 at, say, 1 or 2?  If so, then there's no need to loop there.

// On avx2, where vectorised int->float conversions are not available, we
// instead use the range of magnitudes <2^52, and produce denormals with the
// same ratio, as this is a bit cheaper.

#define GETD          {d=*wv++; if(unlikely(!d)){z=0; goto end;}}
#define INTDIVF(c,d) (c/d-(SGNTO0(c^d)&(c%d!=0)))  // c/d - (c^d)<0 && c%d
#define INTDIVC(c,d) (c/d+(~SGNTO0((c^d))&(c%d!=0)))   // c/d + (c^d)>=0 && c%d

F2(jtintdiv){A z;B b,flr;I an,ar,*as,*av,c,d,j,k,m,n,p,p1,r,*s,wn,wr,*ws,*wv,*zv;
 ARGCHK2(a,w);
 an=AN(a); ar=AR(a); as=AS(a); av=AV(a);
 wn=AN(w); wr=AR(w); ws=AS(w); wv=AV(w); b=ar>=wr; r=b?wr:ar; s=b?as:ws;
 ASSERTAGREE(as,ws,r);
 if(an&&wn){PROD(m,r,s); PROD(n,b?ar-r:wr-r,r+s);}else m=n=0; 
 GATV(z,INT,b?an:wn,b?ar:wr,s); zv=AVn(b?ar:wr,z);
 d=wn?*wv:0; p=0<d?d:-d; p1=d==IMIN?p:p-1; flr=XMFLR==jt->xmode;  // p is abs(divisor), p1 is p-1 unless d=IMIN; IMIN then
 if(!wr&&p&&!(p&p1)){  // divisor is power of 2, perhaps negative
  k=CTTZI(p);  // bit# of the sole 1 bit
  if(d>0)if(flr){DQ(n,*zv++=*av++>>k;)}else{DQ(n, c=*av++; *zv++=likely(!__builtin_add_overflow(c,p1,&c))?c>>k:(UI)c>>k;)}
  else if(flr){DQ(n, c=*av++; *zv++=likely(c>IMIN)?-c>>k:-(IMIN>>k);)}else{DQ(n, *zv++=((~*av++)>>k)+1;)}
 }else{
#if C_AVX512
#define ISBADFLT(x) _mm512_cmpgt_epu64_mask(_mm512_add_epi64(x, _mm512_set1_epi64(1ull<<53)),_mm512_set1_epi64(1ull<<54))
// using or here instead of max admits false positives in case one argument is 2^53.  Too bad.
#define HASBADFLT(x,y) _mm512_cmpgt_epu64_mask(_mm512_or_epi64(_mm512_add_epi64(x,_mm512_set1_epi64(1ull<<53)),\
                                                               _mm512_add_epi64(y,_mm512_set1_epi64(1ull<<53))),\
                                               _mm512_set1_epi64(1ull<<54))
#define DIVRN(vn,vd,mode) ({ __m512d quot=_mm512_div_round_pd(_mm512_cvtepi64_pd(vn),_mm512_cvtepi64_pd(vd),_MM_FROUND_NO_EXC); /*must suppress exceptions here*/\
                             quot=_mm512_fixupimm_pd(quot,quot,_mm512_set1_epi64(0x00550088),0xa0); /*convert nan to 0 (because we want 0%0 to be 0).  Trap on -inf and inf.  Pass everything else through.*/\
                             _mm512_cvt_roundpd_epi64(quot, mode|_MM_FROUND_NO_EXC); })
  if(1==n){
#define EVEN(mode,div) \
   for(I i=0;i<((m-1)&~7);i+=8){ \
    __m512i vd=_mm512_loadu_epi64(wv+i); \
    __m512i vn=_mm512_loadu_epi64(av+i); \
    if(unlikely(HASBADFLT(vd,vn))){ /*could also do this by checking the inexact flag after converting, probably*/ \
     for(I j=i;j<i+8;j++){c=av[j]; d=wv[j]; if(!d){if(!c){d=1;}else{z=0;goto end;}}; zv[j]=div(c,d);} \
     continue;} \
    _mm512_storeu_epi64(zv+i, DIVRN(vn,vd,mode));} \
   I tlen=1+((m-1)&7),off=(m-1)&~7; \
   __mmask8 mask=BZHI(0xff,tlen); \
   __m512i vd=_mm512_maskz_loadu_epi64(mask,wv+off); \
   __m512i vn=_mm512_maskz_loadu_epi64(mask,av+off); \
   if(unlikely(HASBADFLT(vd,vn))){DO(tlen, c=av[i+off]; d=wv[i+off]; if(!d){if(!c){d=1;}else{z=0;goto end;}} zv[i+off]=div(c,d);)} \
   else{_mm512_mask_storeu_epi64(zv+off, mask, DIVRN(vn,vd,mode));}
   if(flr){ EVEN(_MM_FROUND_TO_NEG_INF,INTDIVF); }
   else{    EVEN(_MM_FROUND_TO_POS_INF,INTDIVC); }
   if(NANTEST)z=0;}
#undef EVEN
  else if(b){
#define MOREA(mode,div) \
   for(I _ct=m;_ct--;wv++,av+=n,zv+=n){\
    I d=*wv;\
    if(uncommon(!d)){ /*zero denominator*/\
     DO(n,if(av[i])goto end;zv[i]=0;)\
     continue;}\
    __m512i vd=_mm512_set1_epi64(d);\
    if(unlikely(ISBADFLT(vd))){ /*since this is a cold branch, better to vectorise the condition than to waste registers or cycles on the big immediate*/\
     I d=_mm_cvtsi128_si64(_mm512_castsi512_si128(vd)); DO(n,c=av[i]; zv[i]=div(c,d);)\
     continue;}\
    for(I i=0;i<((n-1)&~7);i+=8){\
     __m512i vn=_mm512_loadu_epi64(av+i);\
     if(unlikely(ISBADFLT(vn))){\
      I d=_mm_cvtsi128_si64(_mm512_castsi512_si128(vd));\
      for(I j=i;j<i+8;j++){zv[j]=div(av[j],d);}\
      continue;}\
     /*we already know w isn't 0, so we don't have to handle the overflow case*/\
     _mm512_storeu_epi64(zv+i, _mm512_cvt_roundpd_epi64(_mm512_div_pd(_mm512_cvtepi64_pd(vn),_mm512_cvtepi64_pd(vd)),_MM_FROUND_NO_EXC|mode));}\
    I tlen=1+((n-1)&7),off=(n-1)&~7; \
    __mmask8 mask=BZHI(0xff,tlen); \
    __m512i vn=_mm512_maskz_loadu_epi64(mask,av+off);\
    if(unlikely(ISBADFLT(vn))){\
     I d=_mm_cvtsi128_si64(_mm512_castsi512_si128(vd));\
     DO(tlen,zv[i+off]=div(av[i+off],d);)}\
    else{\
     _mm512_mask_storeu_epi64(zv+off,mask,_mm512_cvt_roundpd_epi64(_mm512_div_pd(_mm512_cvtepi64_pd(vn),_mm512_cvtepi64_pd(vd)),_MM_FROUND_NO_EXC|mode));}}
   if(flr){ MOREA(_MM_FROUND_TO_NEG_INF,INTDIVF); }
   else{    MOREA(_MM_FROUND_TO_POS_INF,INTDIVC); }}
#undef MOREA
  else{
   //since we can catch a 0 numerator ahead of time, hoist a branch for it, and trap divide-by-zero to avoid needing to play the fixup dance
   I exmask=_MM_GET_EXCEPTION_MASK(); _MM_SET_EXCEPTION_MASK(exmask|_MM_MASK_DIV_ZERO);
#define MOREW(mode,div) \
   for(I _ct=m;_ct--;av++,wv+=n,zv+=n){\
    I c=*av;\
    if(!c){DO(n,zv[i]=0;) continue;}\
    __m512i vn=_mm512_set1_epi64(c);\
    if(unlikely(ISBADFLT(vn))){\
     DO(n,if(!wv[i]){z=0;goto resexend;}zv[i]=div(c,wv[i]);)\
     continue;}\
    for(I i=0;i<((n-1)&~7);i+=8){\
     __m512i vd=_mm512_loadu_epi64(wv+i);\
     if(unlikely(ISBADFLT(vd))){\
      if(_mm512_cmpeq_epi64_mask(vd,_mm512_set1_epi64(0))){z=0;goto resexend;} /*any zero?*/\
      I c=_mm_cvtsi128_si64(_mm512_castsi512_si128(vn));\
      for(I j=i;j<i+8;j++){zv[j]=div(c,wv[j]);}\
      continue;}\
     _mm512_storeu_epi64(zv+i, _mm512_cvt_roundpd_epi64(_mm512_div_pd(_mm512_cvtepi64_pd(vn),_mm512_cvtepi64_pd(vd)),_MM_FROUND_NO_EXC|mode));}\
    I tlen=1+((n-1)&7),off=(n-1)&~7; \
    __mmask8 mask=BZHI(0xff,tlen); \
    __m512i vd=_mm512_maskz_loadu_epi64(mask,wv+off);\
    if(unlikely(ISBADFLT(vd))){\
     if(_mm512_mask_cmpeq_epi64_mask(mask,vd,_mm512_set1_epi64(0))){z=0;goto resexend;} /*any zero in the tail?*/\
     I c=_mm_cvtsi128_si64(_mm512_castsi512_si128(vn));\
     DO(tlen,zv[i+off]=div(c,wv[i+off]);)}\
    else{\
     /*masking here is necessary to avoid faulting in the zeroed irrelevant lanes*/\
     _mm512_mask_storeu_epi64(zv+off,mask,_mm512_cvt_roundpd_epi64(_mm512_maskz_div_pd(mask,_mm512_cvtepi64_pd(vn),_mm512_cvtepi64_pd(vd)),_MM_FROUND_NO_EXC|mode));}}
   if(flr){ MOREW(_MM_FROUND_TO_NEG_INF,INTDIVF); }
   else{    MOREW(_MM_FROUND_TO_POS_INF,INTDIVC); }
#undef MOREW
   if(FE_DIVBYZERO&_clearfp())z=0; //did we divide by any zeroes along the way?
   resexend:
   _MM_SET_EXCEPTION_MASK(exmask);}
#else //C_AVX512
  if(1==n){if(flr)  DQ(m, c=*av++; GETD;                *zv++=INTDIVF(c,d);)
           else     DQ(m, c=*av++; GETD;                *zv++=INTDIVC(c,d);)}
  else if(b){if(flr)DQ(m,          GETD; DQ(n, c=*av++; *zv++=INTDIVF(c,d););)
             else   DQ(m,          GETD; DQ(n, c=*av++; *zv++=INTDIVC(c,d););)}
  else      {if(flr)DQ(m, c=*av++;       DQ(n, GETD;    *zv++=INTDIVF(c,d););)
             else   DQ(m, c=*av++;       DQ(n, GETD;    *zv++=INTDIVC(c,d););)}
#endif
 }
end:
 R z?z:flr?floor1(divide(a,w)):ceil1(divide(a,w));
}    /* <.@% or >.@% on integers */


static F2(jtweight){ARGCHK2(a,w); A z; R dfv1(z,behead(over(AR(w)?w:reshape(a,w),num(1))),bsdot(slash(ds(CSTAR))));}  // */\. }. (({:$a)$w),1

F1(jtbase1){A z;B*v;I c,m,p,r,*s,t,*x;PROLOG(889);
 ARGCHK1(w);
 t=AT(w); r=AR(w); s=AS(w); c=AS(w)[r-1]; c=r?c:1;
 ASSERT(!ISSPARSE(t),EVNONCE);
 if(((c-BW)&SGNIF(t,B01X))>=0)R pdt(w,weight(sc(c),t&RAT+XNUM?cvt(XNUM,num(2)):num(2)));  // 
 CPROD(,m,r-1,s);
 GATV(z,INT,m,r-((UI)r>0),s); x=AV(z); v=BAV(w);
 if(c)DQ(m, p=0; DQ(c, p=2*p+*v++;); *x++=p;)
 else mvc(m*SZI,x,MEMSET00LEN,MEMSET00);
 EPILOG(z);
}

F2(jtbase2){I ar,at,c,t,wr,wt;PROLOG(888);
 ARGCHK2(a,w);
 at=AT(a); ar=AR(a);
 wt=AT(w); wr=AR(w); c=AS(w)[wr-1]; c=wr?c:1;
 ASSERT(!ISSPARSE(at|wt),EVNONCE); t=maxtyped(at,wt);
 if(!(t&at))RZ(a=cvt(t,a));
 if(!(t&wt))RZ(w=cvt(t,w));
 A z=1>=ar?pdt(w,weight(sc(c),a)):rank2ex(w,rank2ex(sc(c),a,DUMMYSELF,0L,MIN(ar,1),0L,MIN(ar,1),jtweight),DUMMYSELF,MIN(wr,1),1L,MIN(wr,1),1L,jtpdt);
 EPILOG(z);
}

// #: y
F1(jtabase1){A d,z;B*zv;I c,n,p,r,t,*v;UI x;
 ARGCHK1(w);
 // n = #atoms, r=rank, t=type
 n=AN(w); r=AR(w); t=AT(w);
 ASSERT(!ISSPARSE(t),EVNONCE);
 // Result has rank one more than the input.  If there are no atoms,
 // return (($w),0)($,)w; if Boolean, return (($w),1)($,)w
 if((-n&SGNIFNOT(t,B01X))>=0)R reshape(apip(shape(w),zeroionei(n!=0)),w);
 if(!(t&INT)){
  // Not integer.  Calculate # digits-1 as d = 2 <.@^. >./ | , w  
  dfv2(d,num(2),maximum(zeroionei(1),aslash(CMAX,mag(ravel(w)))),atop(ds(CFLOOR),ds(CLOG)));
  // Calculate z = ((1+d)$2) #: w
  RZ(z=abase2(reshape(increm(d),num(2)),w));
  // If not float, result is exact or complex; either way, keep it
  if(!(t&FL))R z;
  // If float, see if we had one digit too many (could happen, if the log was too close to an integer)
  // calculate that as (0 = >./ ({."1 z)).  If so, return }."1 z ,  otherwise z
  // But we can't delete a digit if any of the values were negative - all are significant then
  // We also can't delete a digit if there is only 1 digit in the numbers
  if(AS(z)[AR(z)-1]<=1 || i0(aslash(CPLUSDOT,ravel(lt(w,zeroionei(0))))))R z;
  if(0==i0(aslash(CMAX,ravel(IRS1(z,0L,1L,jthead,d)))))R IRS1(z,0L,1L,jtbehead,d);
  RETF(z);
 }
 // Integer.  Calculate x=max magnitude encountered (minimum of 1, to leave 1 output value)
 x=1; v=AV(w);
 DQ(n, p=*v++; x|=(UI)(p>0?p:-p););  // overflow happens on IMIN, no prob
 for(c=0;x;x>>=1){++c;}  // count # bits in result
 GATV0(z,B01,n*c,1+r); MCISH(AS(z),AS(w),r) AS(z)[r]=c;  // Allocate result area, install shape
 v=n+AV(w); zv=AN(z)+BAVn(1+r,z);  // v->last input location (prebiased), zv->last result location (prebiased)
 DQ(n, x=*--v; DQ(c, *--zv=(B)(x&1); x>>=1;));  // copy in the bits, starting with the LSB
 RETF(z);
}

DF2(jtabase2){A z;I an,ar,at,t,wn,wr,wt,zn;
 ARGCHK2(a,w);
 an=AN(a); ar=AR(a); at=AT(a);
 wn=AN(w); wr=AR(w); wt=AT(w);
 ASSERT(!ISSPARSE(at|wt),EVNONCE);
 if(1>ar)R residue(a,w);
 if(1==ar&&!((at|wt)&(NOUN-(B01+INT)))){I*av,d,r,*u,*wv,x,*zv;
  // both types are int/boolean, and ar is a list
  {t=INT; if(!TYPESEQ(t,at)){RZ(a=cvt(t,a));} if(!TYPESEQ(t,wt)){RZ(w=cvt(t,w));}}  // convert args to compatible precisions, changing a and w if needed.  INT if both empty
  // If a ends with _1 followed by any number of 1, there will be overflow if w contains any imin.  Detect that very rare case
  av=an+AV(a); wv=wn+AV(w);
  for(zv=av, d=an;d&&*--zv==1;--d);
  if(d&&*zv==-1){zv=wv; DQ(wn, if(*--zv==IMIN){d=0; break;}) if(!d){RZ(a=cvt(FL,a)); R abase2(a,w);}}
  DPMULDE(an,wn,zn); GATV0(z,INT,zn,1+wr); MCISH(AS(z),AS(w),wr) AS(z)[wr]=an;  // allocate result area
  zv=zn+AVn(1+wr,z);
  if((((2^an)-1)&(av[-2]-1)&-(d=av[-1]))<0){I d1,k;
   // Special case: a is (0,d) where d is positive
   if((d&(d1=d-1))==0){
    // d is a power of 2
    k=CTTZ(d);  // k = #zeros below the 1 in d
    DQ(wn, x=*--wv; *--zv=x&d1; *--zv=x>>k;)
   }else{
    // d is not a power of 2
#if SY_64
    // Avoid integer division by calculating the reciprocal of d to 63-bit accuracy
    // We want to end up with a reciprocal with binary point at 62, and a corresponding shift count.  We will normalize the value to MSB in bit 62 (i. e. in (.5,1)) and divide it into 1.0.
    // Because d is not a power of 2, this divide will not overflow & will yield a 63-bit unsigned fraction.
    I norm=CTLZI(d); norm=62-norm; I normd=d<<norm;
    // The following is a 128-bit/64-bit divide, which clang doesn't do right.  We end with a positive remainder, meaning the quotient is always biased <= the true quotient
    D quo=42535295865117307932921825928971026432./(D)normd; UI quo63=(I)quo; quo63=quo>=9223372036854773760.?IMAX:quo63; I corr=(((unsigned __int128)0x2000000000000000<<64)-(unsigned __int128)normd*quo63)>>32;
    quo63+=(I)((corr*4294967296.)/(D)normd); corr=(((unsigned __int128)0x2000000000000000<<64)-(unsigned __int128)normd*quo63);
    while(corr<0){corr+=normd; --quo63;} while(corr>=normd){corr-=normd; ++quo63;}  // correct quotient to have small positive remainder
    // process each atom.  The long multiply is signed integer (binary point 0) * positive fraction (bp 62), leaving bp 62 in the product before normalizing for the original shift.  add 1/2 the multiplier to round
    // the product, which will then always be the correct quotient
    {I q,r,xs; DQ(wn, x=*--wv; q=((__int128)x*quo63+(quo63>>1))>>(125-norm); r=x-q*d; *--zv=r; *--zv=q;)} 
#else
    // use integer division
    {I q,r,xs; DQ(wn, x=*--wv; xs=REPSGN(x); q=(x-xs)/d+xs; r=x-q*d; *--zv=r; *--zv=q;)}  // remainder has same sign as dividend.  If neg, add 1, divide, sub 1 from quotient; then make remainder right
#endif
   }
  }else DQ(wn, x=*--wv; u=av; DQ(an, d=*--u; *--zv=r=remii(d,x); x=d?(x-r)/d:0;););
  RETF(z);
 }
 {PROLOG(0070);A y,*zv;C*u,*yv;I k;
  F2RANK(1,0,jtabase2,self);
  k=bpnoun(at); u=an*k+CAV(a);
  GA00(y,at,1,0); yv=CAV0(y);
  GATV0(z,BOX,an,1); zv=an+AAV1(z);
  DQ(an, MC(yv,u-=k,k); A tt; RZ(w=divide(minus(w,tt=residue(y,w)),y)); INCORP(tt); *--zv=tt;);
  z=ope(z);
  EPILOG(z);
}}

// Compute power-of-2 | w for INT w, by ANDing.  Result is boolean if mod is 1 or 2
A jtintmod2(J jt,A w,I mod){A z;B *v;I n,q,r,*u;UI m=0;  // init m for warning
 F1PREFIP;ARGCHK1(w);
 if(mod>2)R jtatomic2(jtinplace,sc(mod-1),w,ds(CBW0001));  // INT result, by AND
 // the rest is boolean result
 n=AN(w); v=BAV(w);  // littleendian only
 GATV(z,B01,n,AR(w),AS(w)); RZ(n);  // loops below can't handle empty
 u=AV(z); q=(n-1)>>(LGSZI/C_LE); r=((n-1)&(SZI-1))+1;   // there is always a remnant
 I mask=mod==2?VALIDBOOLEAN:0;  // if mod is 1, all results will be 0; otherwise boolean result
 DQ(q, DQ(SZI, m=(m>>8)+((UI)*v<<((SZI-1)*8)); v+=SZI;); *u++=m&mask;)
 DQ(r, m=(m>>8)+((UI)*v<<((SZI-1)*8)); v+=SZI;);  // 1-8 bytes
 STOREBYTES(v,m&mask,8-r);
 RETF(z);
}
