/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Elementary Functions (Arithmetic, etc.)                          */

#include "j.h"
#include "vasm.h"

#define DIVI(u,v)     (u||v ? ddiv2(u,(D)v) : 0.0)
#define DIVBB(u,v)    (v?u:u?inf:0.0)

#define TYMESBX(u,v)  (u?v:0)
#define TYMESXB(u,v)  (v?u:0)
#define TYMESID(u,v)  (u   ?u*v:0)
#define TYMESDI(u,v)  (   v?u*v:0)
#define TYMESDD(u,v)  TYMES(u,v)

// II add, noting overflow and leaving it, possibly in place
AHDR2(plusII,I,I,I){I u;I v;I w;I oflo=0;
 // overflow is (input signs equal) and (result sign differs from one of them)
 // If u==0, v^=u is always 0 & therefore no overflow
 if(n-1==0) DQ(m, u=*x; v=*y; w= ~u; u+=v; *z=u; ++x; ++y; ++z; w^=v; v^=u; if(XANDY(w,v)<0)++oflo;)
 else if(n-1<0)DQ(m, u=*x++; I thresh = IMIN-u; if (u<=0){DQC(n, v=*y; if(v<thresh)++oflo; v=u+v; *z++=v; y++;)}else{DQC(n, v=*y; if(v>=thresh)++oflo; v=u+v; *z++=v; y++;)})
 else      DQ(m, v=*y++; I thresh = IMIN-v; if (v<=0){DQ(n, u=*x; if(u<thresh)++oflo; u=u+v; *z++=u; x++;)}else{DQ(n, u=*x; if(u>=thresh)++oflo; u=u+v; *z++=u; x++;)})
 if(oflo)jt->jerr=EWOVIP+EWOVIPPLUSII;
}

// BI add, noting overflow and leaving it, possibly in place.  If we add 0, copy the numbers (or leave unchanged, if in place)
AHDR2(plusBI,I,B,I){I u;I v;I oflo=0;
 if(n-1==0)  DQ(m, u=(I)*x; v=*y; if(v==IMAX)oflo+=u; v=u+v; *z++=v; x++; y++; )
 else if(n-1<0){n=~n; DQ(m, u=(I)*x++; if(u){DQ(n, v=*y; if(v==IMAX)oflo=1; v=v+1; *z++=v; y++;)}else{if(z!=y)MC(z,y,n<<LGSZI); z+=n; y+=n;})}
 else      DQ(m, v=*y++; DQ(n, u=(I)*x; if(v==IMAX)oflo+=u; u=u+v; *z++=u; x++;))
 if(oflo)jt->jerr=EWOVIP+EWOVIPPLUSBI;
}

// IB add, noting overflow and leaving it, possibly in place.  If we add 0, copy the numbers (or leave unchanged, if in place)
AHDR2(plusIB,I,I,B){I u;I v;I oflo=0;
 if(n-1==0)  DQ(m, u=*x; v=(I)*y; if(u==IMAX)oflo+=v; u=u+v; *z++=u; x++; y++; )
 else if(n-1<0)DQ(m, u=*x++; DQC(n, v=(I)*y; if(u==IMAX)oflo+=v; v=u+v; *z++=v; y++;))
 else      DQ(m, v=(I)*y++; if(v){DQ(n, u=*x; if(u==IMAX)oflo=1; u=u+1; *z++=u; x++;)}else{if(z!=x)MC(z,x,n<<LGSZI); z+=n; x+=n;})
 if(oflo)jt->jerr=EWOVIP+EWOVIPPLUSIB;
}

#if C_AVX&&SY_64
#define primop256(name,primop) \
AHDR2(name,D,D,D){ \
 __m256i endmask; /* length mask for the last word */ \
 _mm256_zeroupper(VOIDARG); \
 NAN0; \
 if(n-1==0){ \
  /* vector-to-vector, no repetitions */ \
  endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-m)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
  DQ((m-1)>>LGNPAR, _mm256_storeu_pd(z, primop(_mm256_loadu_pd(x),_mm256_loadu_pd(y))); x+=NPAR; y+=NPAR; z+=NPAR;) \
  /* runout, using mask */ \
  _mm256_maskstore_pd(z, endmask, primop(_mm256_maskload_pd(x,endmask),_mm256_maskload_pd(y,endmask))); \
 }else{ \
  if(n-1<0){n=~n; \
   /* atom+vector */ \
   endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-n)&(NPAR-1)))); \
   DQ(m, __m256d u; u=_mm256_set1_pd(*x); ++x; \
     DQ((n-1)>>LGNPAR, _mm256_storeu_pd(z, primop(u,_mm256_loadu_pd(y))); y+=NPAR; z+=NPAR;)  _mm256_maskstore_pd(z, endmask, primop(u,_mm256_maskload_pd(y,endmask))); \
     y+=((n-1)&(NPAR-1))+1; z+=((n-1)&(NPAR-1))+1;) \
  }else{ \
   /* vector+atom */ \
   endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-n)&(NPAR-1)))); \
   DQ(m, __m256d v; v=_mm256_set1_pd(*y); ++y; \
     DQ((n-1)>>LGNPAR, _mm256_storeu_pd(z, primop(_mm256_loadu_pd(x),v)); x+=NPAR; z+=NPAR;)  _mm256_maskstore_pd(z, endmask, primop(_mm256_maskload_pd(x,endmask),v)); \
     x+=((n-1)&(NPAR-1))+1; z+=((n-1)&(NPAR-1))+1;) \
  } \
 } \
 NAN1V; \
}
// D + D, never 0 times
primop256(plusDD,_mm256_add_pd)
primop256(minusDD,_mm256_sub_pd)
primop256(minDD,_mm256_min_pd)
primop256(maxDD,_mm256_max_pd)

// * and % have special requirements
AHDR2(tymesDD,D,D,D){
 // install the length mask for the last word
 __m256i endmask;
 __m256d zero; zero=_mm256_set1_pd(0.0);
 __m256d u, v, unonzero, vnonzero;  // mask: all ones unless the arg is 0.  AND with OTHER arg to turn 0*inf into 0*0
 _mm256_zeroupper(VOIDARG);
 NAN0;
 if(n-1==0){
  // vector-to-vector *, no repetitions
  endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-m)&(NPAR-1))));  // mask for 00=1111, 01=1000, 10=1100, 11=1110
  DQ((m-1)>>LGNPAR, u=_mm256_loadu_pd(x); unonzero=_mm256_cmp_pd(u,zero,_CMP_NEQ_OQ); v=_mm256_loadu_pd(y); vnonzero=_mm256_cmp_pd(v,zero,_CMP_NEQ_OQ);
                    _mm256_storeu_pd(z, _mm256_mul_pd(_mm256_and_pd(u,vnonzero),_mm256_and_pd(v,unonzero))); x+=NPAR; y+=NPAR; z+=NPAR;)
  // runout, using mask
  u=_mm256_maskload_pd(x,endmask); unonzero=_mm256_cmp_pd(u,zero,_CMP_NEQ_OQ); v=_mm256_maskload_pd(y,endmask); vnonzero=_mm256_cmp_pd(v,zero,_CMP_NEQ_OQ);
  _mm256_maskstore_pd(z, endmask, _mm256_mul_pd(_mm256_and_pd(u,vnonzero),_mm256_and_pd(v,unonzero)));
 }else{
  if(n-1<0){n=~n;
   // atom*vector
   endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-n)&(NPAR-1))));  // mask for 00=1111, 01=1000, 10=1100, 11=1110
   DQ(m, u=_mm256_set1_pd(*x); unonzero=_mm256_cmp_pd(u,zero,_CMP_NEQ_OQ); ++x;
     DQ((n-1)>>LGNPAR, v=_mm256_loadu_pd(y); vnonzero=_mm256_cmp_pd(v,zero,_CMP_NEQ_OQ);
                       _mm256_storeu_pd(z, _mm256_mul_pd(_mm256_and_pd(u,vnonzero),_mm256_and_pd(v,unonzero))); y+=NPAR; z+=NPAR;)
     v=_mm256_maskload_pd(y,endmask); vnonzero=_mm256_cmp_pd(v,zero,_CMP_NEQ_OQ);
     _mm256_maskstore_pd(z, endmask, _mm256_mul_pd(_mm256_and_pd(u,vnonzero),_mm256_and_pd(v,unonzero)));
     y+=((n-1)&(NPAR-1))+1; z+=((n-1)&(NPAR-1))+1;
   )
  }else{
   // vector*atom
   endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-n)&(NPAR-1))));  // mask for 00=1111, 01=1000, 10=1100, 11=1110
   DQ(m, v=_mm256_set1_pd(*y); vnonzero=_mm256_cmp_pd(v,zero,_CMP_NEQ_OQ); ++y;
     DQ((n-1)>>LGNPAR, u=_mm256_loadu_pd(x); unonzero=_mm256_cmp_pd(u,zero,_CMP_NEQ_OQ);
                       _mm256_storeu_pd(z, _mm256_mul_pd(_mm256_and_pd(u,vnonzero),_mm256_and_pd(v,unonzero))); x+=NPAR; z+=NPAR;)
     u=_mm256_maskload_pd(x,endmask); unonzero=_mm256_cmp_pd(u,zero,_CMP_NEQ_OQ);
     _mm256_maskstore_pd(z, endmask, _mm256_mul_pd(_mm256_and_pd(u,vnonzero),_mm256_and_pd(v,unonzero)));
     x+=((n-1)&(NPAR-1))+1; z+=((n-1)&(NPAR-1))+1;
   )
  }
 }
 NAN1V;
}
AHDR2(divDD,D,D,D){
 // install the length mask for the last word
 __m256i endmask;
 __m256d zero; zero=_mm256_set1_pd(0.0);
 __m256d one; one=_mm256_set1_pd(1.0);
 __m256d signmask; signmask=_mm256_castsi256_pd(_mm256_set1_epi64x (0x8000000000000000));

 __m256d u, v, unonzero;  // mask: all ones unless u arg is 0.
 _mm256_zeroupper(VOIDARG);
 NAN0;
 if(n-1==0){
  // vector-to-vector %, no repetitions
  endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-m)&(NPAR-1))));  // mask for 00=1111, 01=1000, 10=1100, 11=1110
  DQ((m-1)>>LGNPAR, u=_mm256_loadu_pd(x); unonzero=_mm256_cmp_pd(u,zero,_CMP_NEQ_OQ); v=_mm256_loadu_pd(y);
                    _mm256_storeu_pd(z, _mm256_div_pd(u,_mm256_blendv_pd(_mm256_or_pd(one,_mm256_and_pd(v,signmask)),v,unonzero))); x+=NPAR; y+=NPAR; z+=NPAR;)
  // runout, using mask
  u=_mm256_maskload_pd(x,endmask); unonzero=_mm256_cmp_pd(u,zero,_CMP_NEQ_OQ); v=_mm256_maskload_pd(y,endmask);
  _mm256_maskstore_pd(z, endmask, _mm256_div_pd(u,_mm256_blendv_pd(_mm256_or_pd(one,_mm256_and_pd(v,signmask)),v,unonzero)));
 }else{
  if(n-1<0){n=~n;
   // atom%vector
   endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-n)&(NPAR-1))));  // mask for 00=1111, 01=1000, 10=1100, 11=1110
   DQ(m, u=_mm256_set1_pd(*x); unonzero=_mm256_cmp_pd(u,zero,_CMP_NEQ_OQ); ++x;
     DQ((n-1)>>LGNPAR, v=_mm256_loadu_pd(y);
                       _mm256_storeu_pd(z, _mm256_div_pd(u,_mm256_blendv_pd(_mm256_or_pd(one,_mm256_and_pd(v,signmask)),v,unonzero))); y+=NPAR; z+=NPAR;)
     v=_mm256_maskload_pd(y,endmask);
     _mm256_maskstore_pd(z, endmask, _mm256_div_pd(u,_mm256_blendv_pd(_mm256_or_pd(one,_mm256_and_pd(v,signmask)),v,unonzero)));
     y+=((n-1)&(NPAR-1))+1; z+=((n-1)&(NPAR-1))+1;
   )
  }else{
   // vector%atom
   endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-n)&(NPAR-1))));  // mask for 00=1111, 01=1000, 10=1100, 11=1110
   DQ(m, v=_mm256_set1_pd(*y); ++y;
     DQ((n-1)>>LGNPAR, u=_mm256_loadu_pd(x); unonzero=_mm256_cmp_pd(u,zero,_CMP_NEQ_OQ);
                       _mm256_storeu_pd(z, _mm256_div_pd(u,_mm256_blendv_pd(_mm256_or_pd(one,_mm256_and_pd(v,signmask)),v,unonzero))); x+=NPAR; z+=NPAR;)
     u=_mm256_maskload_pd(x,endmask); unonzero=_mm256_cmp_pd(u,zero,_CMP_NEQ_OQ);
     _mm256_maskstore_pd(z, endmask, _mm256_div_pd(u,_mm256_blendv_pd(_mm256_or_pd(one,_mm256_and_pd(v,signmask)),v,unonzero)));
     x+=((n-1)&(NPAR-1))+1; z+=((n-1)&(NPAR-1))+1;
   )
  }
 }
 NAN1V;
}
#else
ANAN( plusDD, D,D,D, PLUS)
ANAN(minusDD, D,D,D, MINUS)
APFX(minDD, D,D,D, MIN)
APFX(maxDD, D,D,D, MAX)
APFX(tymesDD, D,D,D, TYMESDD)
ANAN(  divDD, D,D,D, DIV)
#endif
// BD DB add similarly?

// II subtract, noting overflow and leaving it, possibly in place
AHDR2(minusII,I,I,I){I u;I v;I w;I oflo=0;
 // overflow is (input signs differ) and (result sign differs from minuend sign)
 if(n-1==0)  {DQ(m, u=*x; v=*y; w=u-v; *z=w; ++x; ++y; ++z; v^=u; u^=w; if(XANDY(u,v)<0)++oflo;)}
// if u<0, oflo if u-v < IMIN => v > u-IMIN; if u >=0, oflo if u-v>IMAX => v < u+IMIN+1 => v <= u+IMIN => v <= u-IMIN
 else if(n-1<0)DQ(m, u=*x++; I thresh = u-IMIN; if (u<0){DQC(n, v=*y; if(v>thresh)++oflo; w=u-v; *z++=w; y++;)}else{DQC(n, v=*y; if(v<=thresh)++oflo; w=u-v; *z++=w; y++;)})
 // if v>0, oflo if u-v < IMIN => u < v+IMIN = v-IMIN; if v<=0, oflo if u-v > IMAX => u>v+IMAX => u>v-1-IMIN => u >= v-IMIN
 else      DQ(m, v=*y++; I thresh = v-IMIN; if (v<=0){DQ(n, u=*x; if(u>=thresh)++oflo; u=u-v; *z++=u; x++;)}else{DQ(n, u=*x; if(u<thresh)++oflo; u=u-v; *z++=u; x++;)})
 if(oflo)jt->jerr=EWOVIP+EWOVIPMINUSII;
}

// BI subtract, noting overflow and leaving it, possibly in place.  If we add 0, copy the numbers (or leave unchanged, if in place)
AHDR2(minusBI,I,B,I){I u;I v;I w;I oflo=0;
 if(n-1==0)  DQ(m, u=(I)*x; v=*y; u=u-v; if(v<=IMIN+1&&u<0)++oflo; *z++=u; x++; y++; )
 else if(n-1<0)DQ(m, u=(I)*x++; DQC(n, v=*y; w=u-v; if(v<=IMIN+1&&w<0)++oflo; *z++=w; y++;))
 else      DQ(m, v=*y++; DQ(n, u=(I)*x; u=u-v; if(v<=IMIN+1&&u<0)++oflo; *z++=u; x++;))
 if(oflo)jt->jerr=EWOVIP+EWOVIPMINUSBI;
}

// IB subtract, noting overflow and leaving it, possibly in place.  If we add 0, copy the numbers (or leave unchanged, if in place)
AHDR2(minusIB,I,I,B){I u;I v;I w;I oflo=0;
 if(n-1==0)  DQ(m, u=*x; v=(I)*y; if(u==IMIN)oflo+=v; u=u-v; *z++=u; x++; y++; )
 else if(n-1<0)DQ(m, u=*x++; DQC(n, v=(I)*y; if(u==IMIN)oflo+=v; w=u-v; *z++=w; y++;))
 else      DQ(m, v=(I)*y++; if(v){DQ(n, u=*x; if(u==IMIN)oflo=1; u=u-1; *z++=u; x++;)}else{if(z!=x)MC(z,x,n<<LGSZI); z+=n; x+=n;})
 if(oflo)jt->jerr=EWOVIP+EWOVIPMINUSIB;
}

// II multiply, in double precision
AHDR2(tymesII,I,I,I){DPMULDECLS I u;I v;if(jt->jerr)R; I *zi=z;
 if(n-1==0)  DQ(m, u=*x; v=*y; DPMUL(u,v,z, goto oflo;) z++; x++; y++; )
 else if(n-1<0)DQ(m, u=*x; DQC(n, v=*y; DPMUL(u,v,z, goto oflo;) z++; y++;) x++;)
 else      DQ(m, v=*y; DQ(n, u=*x; DPMUL(u,v,z, goto oflo;) z++; x++;) y++;)
exit: jt->mulofloloc += z-zi; R;
oflo: jt->jerr = EWOVIP+EWOVIPMULII; *x=u; *y=v; goto exit;  // back out the last store, in case it's in-place; gcc stores before overflow
}

// BI multiply, using clear/copy
AHDR2(tymesBI,I,B,I){B u;I v;
 if(n-1==0)  DQ(m, u=*x; *z++=u?*y:0; x++; y++; )
 else if(n-1<0){n=~n; DQ(m, u=*x++; if(u){if(z!=y)MC(z,y,n<<LGSZI);}else{memset(z,0,n<<LGSZI);} z+=n; y+=n;)}
 else      DQ(m, v=*y++; DQ(n, u=*x; *z++=u?v:0; x++;))
}

// IB multiply, using clear/copy
AHDR2(tymesIB,I,I,B){I u;B v;
 if(n-1==0)  DQ(m, v=*y; *z++=v?*x:0; x++; y++; )
 else if(n-1<0)DQ(m, u=*x++; DQC(n, v=*y; *z++=v?u:0; y++;))
 else      DQ(m, v=*y++; if(v){if(z!=x)MC(z,x,n<<LGSZI);}else{memset(z,0,n<<LGSZI);} z+=n; x+=n;)
}

// BD multiply, using clear/copy
AHDR2(tymesBD,D,B,D){B u;D v;
 if(n-1==0)  DQ(m, u=*x; *z++=u?*y:0; x++; y++; )
 else if(n-1<0){n=~n; DQ(m, u=*x++; if(u){if(z!=y)MC(z,y,n*sizeof(D));}else{memset(z,0,n*sizeof(D));} z+=n; y+=n;)}
 else      DQ(m, v=*y++; DQ(n, u=*x; *z++=u?v:0; x++;))
}

// DB multiply, using clear/copy
AHDR2(tymesDB,D,D,B){D u;B v;
 if(n-1==0)  DQ(m, v=*y; *z++=v?*x:0; x++; y++; )
 else if(n-1<0)DQ(m, u=*x++; DQC(n, v=*y; *z++=v?u:0; y++;))
 else      DQ(m, v=*y++; if(v){if(z!=x)MC(z,x,n*sizeof(D));}else{memset(z,0,n*sizeof(D));} z+=n; x+=n;)
}

// Overflow repair routines
// *x is a non-in-place argument, and n and m advance through it
//  each atom of *x is repeated n times
// *y is the I result of the operation that overflowed
// *z is the D result area (which might be the same as *y)
// b is unused for plus
AHDR2(plusIIO,D,I,I){I u; I absn=n^(n>>(BW-1));
 DQ(m, u=*x++; DQ(absn, *z=(D)u + (D)(*y-u); ++y; ++z;));
}
AHDR2(plusBIO,D,B,I){I u; I absn=n^(n>>(BW-1));
 DQ(m, u=(I)*x++; DQ(absn, *z=(D)u + (D)(*y-u); ++y; ++z;));
}

// For subtract repair, b is 1 if x was the subtrahend, 0 if the minuend
AHDR2(minusIIO,D,I,I){I u; I absn=n^(n>>(BW-1));
 DQ(m, u=*x++; DQ(absn, *z=n<0?((D)(*y+u)-(D)u):((D)u - (D)(u-*y)); ++y; ++z;));
}
AHDR2(minusBIO,D,B,I){I u; I absn=n^(n>>(BW-1));
 DQ(m, u=(I)*x++; DQ(absn, *z=n<0?((D)(*y+u)-(D)u):((D)u - (D)(u-*y)); ++y; ++z;));
}

// In multiply repair, z points to result, x and y to inputs
// Parts of z before mulofloloc have been filled in already
// We have to track the inputs just as for any other action routine
AHDR2(tymesIIO,D,I,I){I u,v; I absn=n^(n>>(BW-1));
 // if all the multiplies are to be skipped, skip them quickly
 I skipct=jt->mulofloloc;
 if(skipct>=m*absn){skipct-=m*absn;
 }else{
  // There are unskipped multiplies.  Do them.
  if(n-1==0)  DQ(m, u=*x; v=*y; if(skipct){--skipct;}else{*z=(D)u * (D)v;} z++; x++; y++; )
  else if(n-1<0)DQ(m, u=*x++; DQC(n, v=*y; if(skipct){--skipct;}else{*z=(D)u * (D)v;} z++; y++;))
  else      DQ(m, v=*y++; DQ(n, u=*x; if(skipct){--skipct;}else{*z=(D)u * (D)v;} z++; x++;))
 }
 // Store the new skipct
 jt->mulofloloc=skipct;
}


// All these lines define functions for various operand combinations
// The comments indicate special cases that are defined by verbs that don't follow the
// AOVF/AIFX/ANAN etc. template


// These routines are used by sparse processing, which doesn't do in-place overflow
APFX( plusIO, D,I,I,  PLUSO)
APFX(minusIO, D,I,I, MINUSO)
APFX(tymesIO, D,I,I, TYMESO)

AIFX( plusBB, I,B,B, +     )    /* plusBI */                AIFX( plusBD, D,B,D, +   )
   /* plusIB */                 /* plusII */                AIFX( plusID, D,I,D, +   )
AIFX( plusDB, D,D,B, +     )  AIFX( plusDI, D,D,I, +)       /* plusDD */
ANAN( plusZZ, Z,Z,Z, zplus )


AIFX(minusBB, I,B,B, -     )    /* minusBI */               AIFX(minusBD, D,B,D, -    )
  /* minusIB */                 /* minusII */               AIFX(minusID, D,I,D, -    )
AIFX(minusDB, D,D,B, -     )  AIFX(minusDI, D,D,I, -)       /* minusDD */
ANAN(minusZZ, Z,Z,Z, zminus)

    /* andBB */                 /* tymesBI */                   /* tymesBD */            
    /* tymesIB */               /* tymesII */               APFX(tymesID, D,I,D, TYMESID)  
    /* tymesDB */             APFX(tymesDI, D,D,I, TYMESDI)    /* tymesDD */ 
ANAN(tymesZZ, Z,Z,Z, ztymes )

APFX(  divBB, D,B,B, DIVBB)   APFX(  divBI, D,B,I, DIVI)    APFX(  divBD, D,B,D, DIV)
APFX(  divIB, D,I,B, DIVI )   APFX(  divII, D,I,I, DIVI)    APFX(  divID, D,I,D, DIV)
APFX(  divDB, D,D,B, DIVI )   APFX(  divDI, D,D,I, DIVI)       /* divDD */
ANAN(  divZZ, Z,Z,Z, zdiv )

     /* orBB */               APFX(  minBI, I,B,I, MIN)     APFX(  minBD, D,B,D, MIN)    
APFX(  minIB, I,I,B, MIN)     APFX(  minII, I,I,I, MIN)     APFX(  minID, D,I,D, MIN)  
APFX(  minDB, D,D,B, MIN)     APFX(  minDI, D,D,I, MIN)        /* minDD */
APFX(  minSS, SB,SB,SB, SBMIN)

    /* andBB */               APFX(  maxBI, I,B,I, MAX)     APFX(  maxBD, D,B,D, MAX)    
APFX(  maxIB, I,I,B, MAX)     APFX(  maxII, I,I,I, MAX)     APFX(  maxID, D,I,D, MAX)  
APFX(  maxDB, D,D,B, MAX)     APFX(  maxDI, D,D,I, MAX)         /* maxDD */
APFX(  maxSS, SB,SB,SB, SBMAX)


D jtremdd(J jt,D a,D b){D q,x,y;
 if(!a)R b;
 ASSERT(!INF(b),EVNAN);
 if(a==inf )R 0<=b?b:a;
 if(a==infm)R 0>=b?b:a;
 q=b/a; x=tfloor(q); y=tceil(q); R teq(x,y)?0:b-a*x;
}

ANAN(remDD, D,D,D, remdd)
ANAN(remZZ, Z,Z,Z, zrem )

I jtremid(J jt,I a,D b){D r;I k;
 ASSERT(a&&-9e15<b&&b<9e15,EWOV);
 r=b-a*floor(b/a); k=(I)r;
 ASSERT(k==r,EWOV);   // not really overflow - just has a fractional part
 R k;
}

APFX(remID, I,I,D, remid)

I remii(I a,I b){I r; R (a!=(a>>(BW-1)))?(r=b%a,0<a?r+(a&(r>>(BW-1))):r+(a&((-r)>>(BW-1)))):a?0:b;}  // must handle IMIN/-1, which overflows.  If a=0, return b.

AHDR2(remII,I,I,I){I u,v;
 if(n-1==0){DQ(m,*z++=remii(*x,*y); x++; y++; )
 }else if(n-1<0){   // repeated x.  Handle special cases and avoid integer divide
#if SY_64 && C_USEMULTINTRINSIC
  DQ(m, u=*x++;
    // take abs(x); handle negative x in a postpass
   UI ua=-u>=0?-u:u;  // abs(x)
   if(!(ua&(ua-1))){I umsk = ua-1; DQC(n, *z++=umsk&*y++;);  // x is a power of 2, including 0
   }else{
    // calculate 1/abs(x) to 53-bit precision.  Remember, x is at least 3, so the MSB will never have signed significance
    UI uarecip = (UI)(18446744073709551616.0/(D)(I)ua);  // recip, with binary point above the msb.  2^64 / ua
    // add in correction for the remaining precision.  The result will still never be higher than the true reciprocal
    I deficitprec = -(I)(uarecip*ua);  // we need to increase uarecip by enough to add (deficitprec) units to (uarecip*ua)
    // because of rounding during the divide, deficitprec may be positive or negative, so we must 2's-comp-correct the product
    UI xx; UI himul; DPUMUL(uarecip,(UI)deficitprec,xx,himul); uarecip=deficitprec<0?0:uarecip; uarecip+=himul;   // now we have 63 bits of uarecip
    // Now loop through each input value.  It is possible that the quotient coming out of the multiplication will be
    // low by at most 1; we correct it if it is
    // The computations here are unsigned, because if signed the binary point gets offset and the upper significance requires a 128-bit shift.
    // Since negative arguments are unusual, we use 1 branch to handle them.  This may mispredict.
    DQC(n, I yv=*y;
      // Multiply by recip to get quotient, which is up to 1/2 LSB low; get remainder; adjust remainder if too high; store
      // 2's-complement adjust for negative y; to make the result still always on the low side, subtract an extra 1.
      DPUMUL(uarecip,(UI)yv,xx,himul); himul-=(uarecip+1)&(yv>>(BW-1)); I rem=yv-himul*ua; rem=(rem-(I)ua)>=0?rem-(I)ua:rem; *z++=rem;
     y++;)
   }
   // if x was negative, move the remainder into the x+1 to 0 range
   if(u<-1){I *zt=z; DQC(n, I t=*--zt; t=t>0?t-ua:t; *zt=t;)}
  )
#else
  DQ(m, u=*x++;
   if(0<=u&&!(u&(u-1))){--u; DQC(n, *z++=u&*y++;);}
   else DQC(n, *z++=remii( u,*y);      y++;)
  )
#endif
 }else      DQ(m, v=*y++; DQ(n, *z++=remii(*x, v); x++;     ));
}


static I igcd1(I a,I b){R a?igcd1(b%a,a):b;}  // Emulate Euclid

I jtigcd(J jt,I a,I b){
 if(a>IMIN&&b>IMIN){a=ABS(a); b=ABS(b);}
 else{
  if(a==b||!a||!b){jt->jerr=EWOV; R 0;}
  if(a==IMIN){b=ABS(b); a=-(a+b);}else{a=ABS(a); b=-(a+b);}
 }
 R a?igcd1(b%a,a):b;
}

D jtdgcd(J jt,D a,D b){D a1,b1,t;B stop = 0;
 a=ABS(a); b=ABS(b); if(a>b){t=a; a=b; b=t;}
 ASSERT(inf!=b,EVNAN);
 if(!a)R b;
 a1=a; b1=b;
 while(remdd(a1/jfloor(0.5+a1/a),b1)){t=a; if((a=remdd(a,b))==0)break; b=t;}  // avoid infinite loop if a goes to 0
 R a;
}    /* D.L. Forkes 1984; E.E. McDonnell 1992 */
I jtilcm(J jt,I a,I b){I z;I d;
 if(a&&b){RZ(d=igcd(a,b)); if(0==(z=jtmult(0,a,b/d)))jt->jerr=EWOV; R z;}else R 0;
}

#define GCDIO(u,v)      (dgcd((D)u,(D)v))
#define LCMIO(u,v)      (dlcm((D)u,(D)v))

D jtdlcm(J jt,D a,D b){ASSERT(!(INF(a)||INF(b)),EVNAN); R a&&b?a*(b/dgcd(a,b)):0;}

APFX(gcdIO, D,I,I, GCDIO)
APFX(gcdII, I,I,I, igcd )
APFX(gcdDD, D,D,D, dgcd )
APFX(gcdZZ, Z,Z,Z, zgcd )

APFX(lcmII, I,I,I, ilcm )
APFX(lcmIO, D,I,I, LCMIO)
APFX(lcmDD, D,D,D, dlcm )
APFX(lcmZZ, Z,Z,Z, zlcm )


#define GETD          {d=*wv++; if(!d){z=0; break;}}
#define INTDIVF(c,d)  (((c^d)>=0)?c/d:c%d?c/d-1:c/d)  // c/d - (c^d)<0 && c%d
#define INTDIVC(c,d)  (((c^d)<0)?c/d:c%d?c/d+1:c/d)   // c/d + (c^d)>=0 && c%d

F2(jtintdiv){A z;B b,flr;I an,ar,*as,*av,c,d,j,k,m,n,p,p1,r,*s,wn,wr,*ws,*wv,*zv;
 RZ(a&&w);
 an=AN(a); ar=AR(a); as=AS(a); av=AV(a);
 wn=AN(w); wr=AR(w); ws=AS(w); wv=AV(w); b=ar>=wr; r=b?wr:ar; s=b?as:ws;
 ASSERTAGREE(as,ws,r);
 if(an&&wn){PROD(m,r,s); PROD(n,b?ar-r:wr-r,r+s);}else m=n=0; 
 GATV(z,INT,b?an:wn,b?ar:wr,s); zv=AV(z);
 d=wn?*wv:0; p=0<d?d:-d; p1=d==IMIN?p:p-1; flr=XMFLR==jt->xmode;
 if(!wr&&p&&!(p&p1)){
  k=0; j=1; while(p>j){++k; j<<=1;}
  switch((0<d?0:2)+(flr?0:1)){
   case 0: DQ(n,          *zv++=*av++>>k;);                    break;
   case 1: DQ(n, c=*av++; *zv++=0< c?1+((c-1)>>k):(c+p1)>>k;); break;
   case 2: DQ(n, c=*av++; *zv++=c>IMIN?-c>>k:-(-c>>k););       break;
   case 3: DQ(n, c=*av++; *zv++=0<=c?-(c>>k):1+(-(1+c)>>k););
 }}else if(flr){
  if(1==n)    DQ(m, c=*av++; GETD;                *zv++=INTDIVF(c,d);  )
  else   if(b)DQ(m,          GETD; DQ(n, c=*av++; *zv++=INTDIVF(c,d););)
  else        DQ(m, c=*av++;       DQ(n, GETD;    *zv++=INTDIVF(c,d););)
 }else{
  if(1==n)    DQ(m, c=*av++; GETD;                *zv++=INTDIVC(c,d);  )
  else   if(b)DQ(m,          GETD; DQ(n, c=*av++; *zv++=INTDIVC(c,d););)
  else        DQ(m, c=*av++;       DQ(n, GETD;    *zv++=INTDIVC(c,d););)
 }
 R z?z:flr?floor1(divide(a,w)):ceil1(divide(a,w));
}    /* <.@% or >.@% on integers */


static F2(jtweight){RZ(a&&w); R df1(behead(over(AR(w)?w:reshape(a,w),num[1])),bsdot(slash(ds(CSTAR))));}  // */\. }. (({:$a)$w),1

F1(jtbase1){A z;B*v;I c,d,m,n,p,r,*s,t,*x;
 RZ(w);
 n=AN(w); t=AT(w); r=AR(w); s=AS(w); c=r?*(s+r-1):1;
 ASSERT(t&DENSE,EVNONCE);
 if(c>(SY_64?63:31)||!(t&B01))R pdt(w,weight(sc(c),t&RAT+XNUM?cvt(XNUM,num[2]):num[2]));
 CPROD1(n,m,r-1,s);
 GATV(z,INT,m,r?r-1:0,s); x=m+AV(z); v=n+BAV(w);
 if(c)DQ(m, p=0; d=1; DQ(c, if(*--v)p+=d; d+=d;); *--x=p;)
 else memset(x-m,C0,m*SZI);
 RETF(z);
}

F2(jtbase2){I ar,*as,at,c,t,wr,*ws,wt;
 RZ(a&&w);
 at=AT(a); ar=AR(a); as=AS(a);
 wt=AT(w); wr=AR(w); ws=AS(w); c=wr?*(ws+wr-1):1;
 ASSERT(!((at|wt)&SPARSE),EVNONCE); t=maxtyped(at,wt);
 if(!(t&at))RZ(a=cvt(t,a));
 if(!(t&wt))RZ(w=cvt(t,w));
 R 1>=ar?pdt(w,weight(sc(c),a)):rank2ex(w,rank2ex(sc(c),a,0L,0L,MIN(ar,1),0L,MIN(ar,1),jtweight),0L,MIN(wr,1),1L,MIN(wr,1),1L,jtpdt);
}

// #: y
F1(jtabase1){A d,z;B*zv;I c,n,p,r,t,*v;UI x;
 RZ(w);
 // n = #atoms, r=rank, t=type
 n=AN(w); r=AR(w); t=AT(w);
 ASSERT(t&DENSE,EVNONCE);
 // Result has rank one more than the input.  If there are no atoms,
 // return (($w),0)($,)w; if Boolean, return (($w),1)($,)w
 if(!n||t&B01)R reshape(over(shape(w),num[n!=0]),w);
 if(!(t&INT)){
  // Not integer.  Calculate # digits-1 as d = 2 <.@^. >./ | , w  
  d=df2(num[2],maximum(zeroionei[1],aslash(CMAX,mag(ravel(w)))),atop(ds(CFLOOR),ds(CLOG)));
  // Calculate z = ((1+d)$2) #: w
  RZ(z=abase2(reshape(increm(d),num[2]),w));
  // If not float, result is exact or complex; either way, keep it
  if(!(t&FL))R z;
  // If float, see if we had one digit too many (could happen, if the log was too close to an integer)
  // calculate that as (0 = >./ ({."1 z)).  If so, return }."1 z ,  otherwise z
  // But we can't delete a digit if any of the values were negative - all are significant then
  // We also can't delete a digit if there is only 1 digit in the numbers
  if(AS(z)[AR(z)-1]<=1 || i0(aslash(CPLUSDOT,ravel(lt(w,zeroionei[0])))))R z;
  if(0==i0(aslash(CMAX,ravel(IRS1(z,0L,1L,jthead,d)))))R IRS1(z,0L,1L,jtbehead,d);
  RETF(z);
 }
 // Integer.  Calculate x=max magnitude encountered (minimum of 1, to leave 1 output value)
 x=1; v=AV(w);
 DQ(n, p=*v++; x|=(UI)(p>0?p:-p););  // overflow happens on IMIN, no prob
 for(c=0;x;x>>=1){++c;}  // count # bits in result
 GATV(z,B01,n*c,1+r,AS(w)); AS(z)[r]=c;  // Allocate result area, install shape
 v=n+AV(w); zv=AN(z)+BAV(z);  // v->last input location (prebiased), zv->last result location (prebiased)
 DQ(n, x=*--v; DQ(c, *--zv=(B)(x&1); x>>=1;));  // copy in the bits, starting with the LSB
 RETF(z);
}

F2(jtabase2){A z;I an,ar,at,t,wn,wr,wt,zn;
 RZ(a&&w);
 an=AN(a); ar=AR(a); at=AT(a);
 wn=AN(w); wr=AR(w); wt=AT(w);
 ASSERT(at&DENSE&&wt&DENSE,EVNONCE);
 if(1>ar)R residue(a,w);
 if(1==ar&&!((at|wt)&(NOUN-(B01+INT)))){I*av,d,r,*u,*wv,x,*zv;
  // both types are int/boolean, and ar is a list
  {t=INT; if(!TYPESEQ(t,at)){RZ(a=cvt(t,a));} if(!TYPESEQ(t,wt)){RZ(w=cvt(t,w));}}  // convert args to compatible precisions, changing a and w if needed.  INT if both empty
  // If a ends with _1 followed by any number of 1, there will be overflow if w contains any imin.  Detect that very rare case
  av=an+AV(a); wv=wn+AV(w);
  for(zv=av, d=an;d&&*--zv==1;--d);
  if(d&&*zv==-1){zv=wv; DQ(wn, if(*--zv==IMIN){d=0; break;}) if(!d){RZ(a=cvt(FL,a)); R abase2(a,w);}}
  RE(zn=mult(an,wn)); GATV(z,INT,zn,1+wr,AS(w)); AS(z)[wr]=an;  // allocate result area
  zv=zn+AV(z);
  if(2==an&&!av[-2]&&0<(d=av[-1])){I d1,k;
   // Special case: a is (0,d) where d is positive
   if(d&(d1=d-1)){I q,r,xs;
    // d is not a power of 2
    DQ(wn, x=*--wv; xs=(x>>(BW-1)); q=(x-xs)/d+xs; r=x-q*d; *--zv=r; *--zv=q;)  // remainder has same sign as dividend.  If neg, add 1, divide, sub 1 from quotient; then make remainder right
   }else{
    // d is a power of 2
    k=CTTZ(d);  // k = #zeros below the 1 in d
    DQ(wn, x=*--wv; *--zv=x&d1; *--zv=x>>k;)
   }
  }else DQ(wn, x=*--wv; u=av; DQ(an, d=*--u; *--zv=r=remii(d,x); x=d?(x-r)/d:0;););
  RETF(z);
 }
 {PROLOG(0070);A y,*zv;C*u,*yv;I k;
  F2RANK(1,0,jtabase2,0);
  k=bpnoun(at); u=an*k+CAV(a);
  GA(y,at, 1, 0,0); yv=CAV(y);
  GATV0(z,BOX,an,1); zv=an+AAV(z);
  DQ(an, MC(yv,u-=k,k); RZ(w=divide(minus(w,*--zv=residue(y,w)),y)););
  z=ope(z);
  EPILOG(z);
}}

// Compute power-of-2 | w for INT w, by ANDing.  Result is boolean if mod is 1 or 2
A jtintmod2(J jt,A w,I mod){A z;B *v;I n,q,r,*u;UI m=0;  // init m for warning
 RZ(w);F1PREFIP;
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
