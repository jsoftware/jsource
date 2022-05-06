/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Comparatives                                                     */

#include "j.h"
#include "ve.h"
#include "vcomp.h"

#define CMPGT(a,b) ((a)>(b)?1:0)
#define CMPLT(a,b) ((a)<(b)?1:0)
#define CMPGE(a,b) ((a)>=(b)?1:0)
#define CMPLE(a,b) ((a)<=(b)?1:0)
#define EQTEMPLATE APFX
#define NETEMPLATE APFX
#define CMPEQ(a,b) ((a)==(b)?1:0)
#define CMPNE(a,b) ((a)!=(b)?1:0)

D jttfloor(J jt,D v){D x; R (x=jround(v), x-TGT(x,v));}
D jttceil (J jt,D v){D x; R (x=jround(v), x+TLT(x,v));}

BPFX(eqBB, EQ,BEQ,EQ,BEQ, _mm256_xor_pd(bool256,_mm256_xor_pd(u256,v256)) , ,__m256d bool256=_mm256_broadcast_sd((D*)&Ivalidboolean);)
BPFX(neBB, NE,BNE,NE,BNE, _mm256_xor_pd(u256,v256) , ,)
BPFX(ltBB, LT,BLT,GT,BGT, _mm256_andnot_pd(u256,v256) , ,)
BPFX(leBB, LE,BLE,GE,BGE, _mm256_xor_pd(bool256,_mm256_andnot_pd(v256,u256)) , ,__m256d bool256=_mm256_broadcast_sd((D*)&Ivalidboolean);)
BPFX(geBB, GE,BGE,LE,BLE, _mm256_xor_pd(bool256,_mm256_andnot_pd(u256,v256)) , ,__m256d bool256=_mm256_broadcast_sd((D*)&Ivalidboolean);)
BPFX(gtBB, GT,BGT,LT,BLT, _mm256_andnot_pd(v256,u256) , , )
 

EQTEMPLATE(eqBI, B,B,I, CMPEQ,, R EVOK;)  ACMP0(eqBD, B,B,D, TEQ, ==  )
EQTEMPLATE(eqIB, B,I,B, CMPEQ,, R EVOK;)  ACMP0(eqID, B,I,D, TEQ, ==  )
ACMP0(eqDB, B,D,B, TEQ, ==  )             ACMP0(eqDI, B,D,I, TEQ, ==  )  
APFX(eqZZ, B,Z,Z, zeq,, R EVOK;)   
APFX(eqXX, B,X,X, equ,, R EVOK;)   
APFX(eqQQ, B,Q,Q, QEQ,, R EVOK;)
BPFXAVX2(eqCC, CMPEQCC,x, CMPEQCC, x,
   (workarea=_mm256_castpd_si256(_mm256_xor_pd(u256,v256)), _mm256_castsi256_pd(_mm256_and_si256(_mm256_srli_epi64(_mm256_andnot_si256(workarea,_mm256_sub_epi8(workarea,bool256)),7),bool256))) ,
   I work; , __m256i workarea; __m256i bool256=_mm256_castpd_si256(_mm256_broadcast_sd((D*)&Ivalidboolean));
   )

EQTEMPLATE(eqCS, B,UC,US, CMPEQ,, R EVOK;)  EQTEMPLATE(eqSC, B,US,UC, CMPEQ,, R EVOK;)  EQTEMPLATE(eqSS, B,S,S, CMPEQ,, R EVOK;)
EQTEMPLATE(eqUU, B,C4,C4, CMPEQ,, R EVOK;)  EQTEMPLATE(eqUS, B,C4,US, CMPEQ,, R EVOK;)  EQTEMPLATE(eqSU, B,US,C4, CMPEQ,, R EVOK;)
EQTEMPLATE(eqCU, B,UC,C4, CMPEQ,, R EVOK;)  EQTEMPLATE(eqUC, B,C4,UC, CMPEQ,, R EVOK;)
APFXL(eqAA, B,A,A,CERR,equ,, R EVOK;)

                          NETEMPLATE(neBI, B,B,I, CMPNE,, R EVOK;)  ACMP0(neBD, B,B,D, TNE, != )
NETEMPLATE(neIB, B,I,B, CMPNE,, R EVOK;)    ACMP0(neID, B,I,D, TNE, != )
ACMP0(neDB, B,D,B, TNE, != )  ACMP0(neDI, B,D,I, TNE, != )  
APFX(neZZ, B,Z,Z, !zeq,, R EVOK;)  
APFX(neXX, B,X,X, !equ,, R EVOK;)
APFX(neQQ, B,Q,Q, !QEQ,, R EVOK;)
BPFXAVX2(neCC, CMPNECC,x, CMPNECC, x,
   (workarea=_mm256_castpd_si256(_mm256_xor_pd(u256,v256)), _mm256_castsi256_pd(_mm256_andnot_si256(_mm256_srli_epi64(_mm256_andnot_si256(workarea,_mm256_sub_epi8(workarea,bool256)),7),bool256))) ,
   I work; , __m256i workarea; __m256i bool256=_mm256_castpd_si256(_mm256_broadcast_sd((D*)&Ivalidboolean));
   )
NETEMPLATE(neCS, B,UC,US, CMPNE,, R EVOK;)  NETEMPLATE(neSC, B,US,UC, CMPNE,, R EVOK;)  NETEMPLATE(neSS, B,S,S, CMPNE,, R EVOK;)
NETEMPLATE(neUU, B,C4,C4, CMPNE,, R EVOK;)  NETEMPLATE(neUS, B,C4,US, CMPNE,, R EVOK;)  NETEMPLATE(neSU, B,US,C4, CMPNE,, R EVOK;)
NETEMPLATE(neCU, B,UC,C4, CMPNE,, R EVOK;)  NETEMPLATE(neUC, B,C4,UC, CMPNE,, R EVOK;)
APFXL(neAA, B,A,A,CERR,!equ,, R EVOK;)

APFX(ltBI, B,B,I, CMPLT,, R EVOK;)  ACMP0(ltBD, B,B,D, TLT, <  )
APFX(ltIB, B,I,B, CMPLT,, R EVOK;)    ACMP0(ltID, B,I,D, TLT, <  )
ACMP0(ltDB, B,D,B, TLT, <  )  ACMP0(ltDI, B,D,I, TLT, <  )  
APFX(ltXX, B,X,X, -1==xcompare,, R EVOK;)
APFX(ltQQ, B,Q,Q, QLT,, R EVOK;)
APFX(ltSS, B,SB,SB, SBLT,, R EVOK;)

APFX(leBI, B,B,I, CMPLE,, R EVOK;)  ACMP0(leBD, B,B,D, TLE, <=  )
APFX(leIB, B,I,B, CMPLE,, R EVOK;)   ACMP0(leID, B,I,D, TLE, <=  )
ACMP0(leDB, B,D,B, TLE, <=  )  ACMP0(leDI, B,D,I, TLE, <=  )  
APFX(leXX, B,X,X,  1!=xcompare,, R EVOK;)
APFX(leQQ, B,Q,Q, QLE,, R EVOK;)
APFX(leSS, B,SB,SB, SBLE,, R EVOK;)

APFX(geBI, B,B,I, CMPGE,, R EVOK;)  ACMP0(geBD, B,B,D, TGE, >=  )
APFX(geIB, B,I,B, CMPGE,, R EVOK;)    ACMP0(geID, B,I,D, TGE, >=  )
ACMP0(geDB, B,D,B, TGE, >=  )  ACMP0(geDI, B,D,I, TGE, >=  )  
APFX(geXX, B,X,X, -1!=xcompare,, R EVOK;)
APFX(geQQ, B,Q,Q, QGE,, R EVOK;)
APFX(geSS, B,SB,SB, SBGE,, R EVOK;)

APFX(gtBI, B,B,I, CMPGT,, R EVOK;)  ACMP0(gtBD, B,B,D, TGT, >  )
APFX(gtIB, B,I,B, CMPGT,, R EVOK;)     ACMP0(gtID, B,I,D, TGT, >  )
ACMP0(gtDB, B,D,B, TGT, >  )   ACMP0(gtDI, B,D,I, TGT, > )  
APFX(gtXX, B,X,X,  1==xcompare,, R EVOK;)
APFX(gtQQ, B,Q,Q, QGT,, R EVOK;)
APFX(gtSS, B,SB,SB, SBGT,, R EVOK;)

// 
#if (C_AVX2&&SY_64) || EMU_AVX2
#define primcmpD256(name,tolres,intolres,decls) \
AHDR2(name,B,D,D){ \
 __m256i endmask; /* length mask for the last word */ \
 __m256d u,v,cct,ctop,eq; /* args, ct, eq result and also main result */ \
 decls \
 _mm256_zeroupperx(VOIDARG) \
 if(jt->cct!=1.0){ \
  cct=_mm256_broadcast_sd(&jt->cct); \
  if(n-1==0){ \
   /* vector op vector, no repetitions */ \
   endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-m)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
   DQ((m-1)>>LGNPAR, \
     u=_mm256_loadu_pd(x);v=_mm256_loadu_pd(y); \
     eq=_mm256_xor_pd(_mm256_cmp_pd(u,_mm256_mul_pd(v,cct),_CMP_GT_OQ),_mm256_cmp_pd(v,_mm256_mul_pd(u,cct),_CMP_LE_OQ)); \
     eq=tolres; \
     *(I4*)z=VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)); \
     x+=NPAR; y+=NPAR; z+=NPAR;) \
   /* runout, using mask */ \
   u=_mm256_maskload_pd(x,endmask);v=_mm256_maskload_pd(y,endmask); \
   eq=_mm256_xor_pd(_mm256_cmp_pd(u,_mm256_mul_pd(v,cct),_CMP_GT_OQ),_mm256_cmp_pd(v,_mm256_mul_pd(u,cct),_CMP_LE_OQ)); \
   eq=tolres; \
   STOREBYTES(z,VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)),((-m)&(NPAR-1))+NPAR);  /* could just overstore */ \
  }else{ \
   if(n-1<0){n=~n; \
    /* atom op vector */ \
    if(m==1 && *x==0.0)goto name##av0;  /* if comparing against 0, switch to intolerant */ \
    endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-n)&(NPAR-1)))); \
    DQ(m, u=_mm256_broadcast_sd(x); ++x; \
      ctop=_mm256_mul_pd(u,cct); \
      DQ((n-1)>>LGNPAR, \
        v=_mm256_loadu_pd(y); \
        eq=_mm256_xor_pd(_mm256_cmp_pd(u,_mm256_mul_pd(v,cct),_CMP_GT_OQ),_mm256_cmp_pd(v,ctop,_CMP_LE_OQ)); \
        eq=tolres; \
        *(I4*)z=VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)); \
        y+=NPAR; z+=NPAR;) \
      v=_mm256_maskload_pd(y,endmask); \
      eq=_mm256_xor_pd(_mm256_cmp_pd(u,_mm256_mul_pd(v,cct),_CMP_GT_OQ),_mm256_cmp_pd(v,ctop,_CMP_LE_OQ)); \
      eq=tolres; \
      STOREBYTES(z,VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)),((-n)&(NPAR-1))+NPAR);  /* could just overstore */ \
      y+=((n-1)&(NPAR-1))+1; z+=((n-1)&(NPAR-1))+1;) \
   }else{ \
    /* vector op atom */ \
    if(m==1 && *y==0.0)goto name##va0;  /* if comparing against 0, switch to intolerant */ \
    endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-n)&(NPAR-1)))); \
    DQ(m, v=_mm256_broadcast_sd(y); ++y; \
      ctop=_mm256_mul_pd(v,cct); \
      DQ((n-1)>>LGNPAR, \
        u=_mm256_loadu_pd(x); \
        eq=_mm256_xor_pd(_mm256_cmp_pd(u,ctop,_CMP_GT_OQ),_mm256_cmp_pd(v,_mm256_mul_pd(u,cct),_CMP_LE_OQ)); \
        eq=tolres; \
        *(I4*)z=VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)); \
        x+=NPAR; z+=NPAR;) \
      u=_mm256_maskload_pd(x,endmask); \
      eq=_mm256_xor_pd(_mm256_cmp_pd(u,ctop,_CMP_GT_OQ),_mm256_cmp_pd(v,_mm256_mul_pd(u,cct),_CMP_LE_OQ)); \
      eq=tolres; \
      STOREBYTES(z,VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)),((-n)&(NPAR-1))+NPAR);  /* could just overstore */ \
      x+=((n-1)&(NPAR-1))+1; z+=((n-1)&(NPAR-1))+1;) \
   } \
  } \
 }else{ \
  if(n-1==0){ \
   /* vector op!.0 vector, no repetitions */ \
   endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-m)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
   DQ((m-1)>>LGNPAR, \
     u=_mm256_loadu_pd(x);v=_mm256_loadu_pd(y); \
     eq=intolres; \
     *(I4*)z=VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)); \
     x+=NPAR; y+=NPAR; z+=NPAR;) \
   /* runout, using mask */ \
   u=_mm256_maskload_pd(x,endmask);v=_mm256_maskload_pd(y,endmask); \
   eq=intolres; \
   STOREBYTES(z,VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)),((-m)&(NPAR-1))+NPAR);  /* could just overstore */ \
  }else{ \
   if(n-1<0){n=~n; \
    name##av0: /* atom op!.0 vector */ \
    endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-n)&(NPAR-1)))); \
    DQ(m,; u=_mm256_broadcast_sd(x); ++x; \
      DQ((n-1)>>LGNPAR, \
        v=_mm256_loadu_pd(y); \
        eq=intolres; \
        *(I4*)z=VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)); \
        y+=NPAR; z+=NPAR;) \
      v=_mm256_maskload_pd(y,endmask); \
      eq=intolres; \
      STOREBYTES(z,VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)),((-n)&(NPAR-1))+NPAR);  /* could just overstore */ \
      y+=((n-1)&(NPAR-1))+1; z+=((n-1)&(NPAR-1))+1;) \
   }else{ \
    name##va0: /* vector op!.0atom */ \
    endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-n)&(NPAR-1)))); \
    DQ(m, v=_mm256_broadcast_sd(y); ++y; \
      DQ((n-1)>>LGNPAR, \
        u=_mm256_loadu_pd(x); \
        eq=intolres; \
        *(I4*)z=VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)); \
        x+=NPAR; z+=NPAR;) \
      u=_mm256_maskload_pd(x,endmask); \
      eq=intolres; \
      STOREBYTES(z,VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)),((-n)&(NPAR-1))+NPAR);  /* could just overstore */ \
      x+=((n-1)&(NPAR-1))+1; z+=((n-1)&(NPAR-1))+1;) \
   } \
  } \
 } \
 R EVOK; \
}

primcmpD256(geDD, _mm256_or_pd(eq,_mm256_cmp_pd(u,v,_CMP_GE_OQ)) , _mm256_cmp_pd(u,v,_CMP_GE_OQ) , )
primcmpD256(gtDD, _mm256_andnot_pd(eq,_mm256_cmp_pd(u,v,_CMP_GT_OQ)) , _mm256_cmp_pd(u,v,_CMP_GT_OQ) , )
primcmpD256(leDD, _mm256_or_pd(eq,_mm256_cmp_pd(u,v,_CMP_LE_OQ)) , _mm256_cmp_pd(u,v,_CMP_LE_OQ) , )
primcmpD256(ltDD, _mm256_andnot_pd(eq,_mm256_cmp_pd(u,v,_CMP_LT_OQ)) , _mm256_cmp_pd(u,v,_CMP_LT_OQ) , )
primcmpD256(eqDD, eq , _mm256_cmp_pd(u,v,_CMP_EQ_OQ) , )
primcmpD256(neDD, _mm256_xor_pd(eq,one) , _mm256_cmp_pd(u,v,_CMP_NEQ_OQ) , __m256d one=_mm256_broadcast_sd((D*)&validitymask);)   // warnings from one=_mm256_cmp_pd(cct,cct,_CMP_TRUE_UQ);

#define primcmpI256(name,result,decls) \
AHDR2(name,B,I,I){ \
 __m256i endmask; /* length mask for the last word */ \
 __m256i u,v,eq; /* args, ct, main result */ \
 decls \
 _mm256_zeroupperx(VOIDARG) \
 if(n-1==0){ \
  /* vector-to-vector, no repetitions */ \
  endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-m)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
  DQ((m-1)>>LGNPAR, \
    u=_mm256_loadu_si256((__m256i*)x);v=_mm256_loadu_si256((__m256i*)y); \
    eq=result; \
    *(I4*)z=VALIDBOOLEAN&_mm256_movemask_epi8(eq); \
    x+=NPAR; y+=NPAR; z+=NPAR;) \
  /* runout, using mask */ \
  u=_mm256_maskload_epi64(x,endmask);v=_mm256_maskload_epi64(y,endmask); \
  eq=result; \
  STOREBYTES(z,VALIDBOOLEAN&_mm256_movemask_epi8(eq),((-m)&(NPAR-1))+NPAR);  /* could just overstore */ \
 }else{ \
  if(n-1<0){n=~n; \
   /* atom+vector */ \
   endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-n)&(NPAR-1)))); \
   DQ(m,; u=_mm256_castpd_si256(_mm256_broadcast_sd((D*)x)); ++x; \
     DQ((n-1)>>LGNPAR, \
       v=_mm256_loadu_si256((__m256i*)y); \
       eq=result; \
       *(I4*)z=VALIDBOOLEAN&_mm256_movemask_epi8(eq); \
       y+=NPAR; z+=NPAR;) \
     v=_mm256_maskload_epi64(y,endmask); \
     eq=result; \
     STOREBYTES(z,VALIDBOOLEAN&_mm256_movemask_epi8(eq),((-n)&(NPAR-1))+NPAR);  /* could just overstore */ \
     y+=((n-1)&(NPAR-1))+1; z+=((n-1)&(NPAR-1))+1;) \
  }else{ \
   /* vector+atom */ \
   endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-n)&(NPAR-1)))); \
   DQ(m, v=_mm256_castpd_si256(_mm256_broadcast_sd((D*)y)); ++y; \
     DQ((n-1)>>LGNPAR, \
       u=_mm256_loadu_si256((__m256i*)x); \
       eq=result; \
       *(I4*)z=VALIDBOOLEAN&_mm256_movemask_epi8(eq); \
       x+=NPAR; z+=NPAR;) \
     u=_mm256_maskload_epi64(x,endmask); \
     eq=result; \
     STOREBYTES(z,VALIDBOOLEAN&_mm256_movemask_epi8(eq),((-n)&(NPAR-1))+NPAR);  /* could just overstore */ \
     x+=((n-1)&(NPAR-1))+1; z+=((n-1)&(NPAR-1))+1;) \
  } \
 } \
 R EVOK; \
}

primcmpI256(eqII, _mm256_cmpeq_epi64(u,v) , )
primcmpI256(neII, _mm256_xor_si256(one,_mm256_cmpeq_epi64(u,v)) , __m256i one=_mm256_castpd_si256(_mm256_broadcast_sd((D*)&validitymask));)
primcmpI256(gtII, _mm256_cmpgt_epi64(u,v) , )
primcmpI256(geII, _mm256_or_si256(_mm256_cmpeq_epi64(u,v),_mm256_cmpgt_epi64(u,v)) , )
primcmpI256(ltII, _mm256_cmpgt_epi64(v,u) , )
primcmpI256(leII, _mm256_or_si256(_mm256_cmpeq_epi64(u,v),_mm256_cmpgt_epi64(v,u)) , )

#if 0  // time these with the following differential test to remove parsing overhead
l=.1000 ?@$ 0
r=.1000 ?@$ 0
r2=.2 ?@$ 0
l2=.2 ?@$ 0
(1000000 (6!:2) 'l =!.0 r') -  (1000000 (6!:2) 'l2 =!.0 r2')
#endif
#else
ACMP0(geDD, B,D,D, TGE, >=  )
ACMP0(gtDD, B,D,D, TGT, >  )
ACMP0(leDD, B,D,D, TLE, <=  )
ACMP0(ltDD, B,D,D, TLT, <  )
ACMP0(neDD, B,D,D, TNE, != )
ACMP0(eqDD, B,D,D, TEQ, ==  )

APFX(gtII, B,I,I, CMPGT,,R EVOK;)
APFX(geII, B,I,I, CMPGE,,R EVOK;)
APFX(leII, B,I,I, CMPLE,,R EVOK;)
APFX(ltII, B,I,I, CMPLT,,R EVOK;)
NETEMPLATE(neII, B,I,I, CMPNE,,R EVOK;)
EQTEMPLATE(eqII, B,I,I, CMPEQ,,R EVOK;)
#endif
