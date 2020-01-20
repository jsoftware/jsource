/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
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

BPFX(eqBB, EQ,BEQ,EQ,BEQ, _mm256_xor_pd(bool256,_mm256_xor_pd(u256,v256)) , ,__m256d bool256=_mm256_castsi256_pd(_mm256_set1_epi64x(0x0101010101010101));)
BPFX(neBB, NE,BNE,NE,BNE, _mm256_xor_pd(u256,v256) , ,)
BPFX(ltBB, LT,BLT,GT,BGT, _mm256_andnot_pd(u256,v256) , ,)
BPFX(leBB, LE,BLE,GE,BGE, _mm256_xor_pd(bool256,_mm256_andnot_pd(v256,u256)) , ,__m256d bool256=_mm256_castsi256_pd(_mm256_set1_epi64x(0x0101010101010101));)
BPFX(geBB, GE,BGE,LE,BLE, _mm256_xor_pd(bool256,_mm256_andnot_pd(u256,v256)) , ,__m256d bool256=_mm256_castsi256_pd(_mm256_set1_epi64x(0x0101010101010101));)
BPFX(gtBB, GT,BGT,LT,BLT, _mm256_andnot_pd(v256,u256) , , )
 

                          EQTEMPLATE(eqBI, B,B,I, CMPEQ   )  ACMP0(eqBD, B,B,D, TEQ, ==  )
EQTEMPLATE(eqIB, B,I,B, CMPEQ   )    ACMP0(eqID, B,I,D, TEQ, ==  )
ACMP0(eqDB, B,D,B, TEQ, ==  )  ACMP0(eqDI, B,D,I, TEQ, ==  )  
APFX(eqZZ, B,Z,Z, zeq  )   
APFX(eqXX, B,X,X, equ  )   
APFX(eqQQ, B,Q,Q, QEQ  )
BPFXAVX2(eqCC, CMPEQCC,x, CMPEQCC, x,
   (workarea=_mm256_castpd_si256(_mm256_xor_pd(u256,v256)), _mm256_castsi256_pd(_mm256_and_si256(_mm256_srli_epi64(_mm256_andnot_si256(workarea,_mm256_sub_epi8(workarea,bool256)),7),bool256))) ,
   I work; , __m256i workarea; __m256i bool256=_mm256_set1_epi64x(0x0101010101010101);
   )

#if 0 // obsolete 
 (workarea=_mm256_xor_pd(u256,v256), workarea=_mm256_or_pd(workarea,_mm256_castsi256_pd(_mm256_srli_epi64(_mm256_castpd_si256(workarea),4))) ,
     workarea=_mm256_or_pd(workarea,_mm256_castsi256_pd(_mm256_srli_epi64(_mm256_castpd_si256(workarea),2))),
     _mm256_andnot_pd(_mm256_or_pd(workarea,_mm256_castsi256_pd(_mm256_srli_epi64(_mm256_castpd_si256(workarea),1))),bool256) ) , 
#endif

                           EQTEMPLATE(eqCS, B,UC,US, CMPEQ )  EQTEMPLATE(eqSC, B,US,UC, CMPEQ )  EQTEMPLATE(eqSS, B,S,S, CMPEQ)
EQTEMPLATE(eqUU, B,C4,C4, CMPEQ )  EQTEMPLATE(eqUS, B,C4,US, CMPEQ )  EQTEMPLATE(eqSU, B,US,C4, CMPEQ )
EQTEMPLATE(eqCU, B,UC,C4, CMPEQ )  EQTEMPLATE(eqUC, B,C4,UC, CMPEQ )
APFX(eqAA, B,A,A, equ  )

                          NETEMPLATE(neBI, B,B,I, CMPNE   )  ACMP0(neBD, B,B,D, TNE, != )
NETEMPLATE(neIB, B,I,B, CMPNE   )    ACMP0(neID, B,I,D, TNE, != )
ACMP0(neDB, B,D,B, TNE, != )  ACMP0(neDI, B,D,I, TNE, != )  
APFX(neZZ, B,Z,Z, !zeq )  
APFX(neXX, B,X,X, !equ )
APFX(neQQ, B,Q,Q, !QEQ )
BPFXAVX2(neCC, CMPNECC,x, CMPNECC, x,
   (workarea=_mm256_castpd_si256(_mm256_xor_pd(u256,v256)), _mm256_castsi256_pd(_mm256_andnot_si256(_mm256_srli_epi64(_mm256_andnot_si256(workarea,_mm256_sub_epi8(workarea,bool256)),7),bool256))) ,
   I work; , __m256i workarea; __m256i bool256=_mm256_set1_epi64x(0x0101010101010101);
   )
#if 0 // obsolete 
 (workarea=_mm256_xor_pd(u256,v256), workarea=_mm256_or_pd(workarea,_mm256_castsi256_pd(_mm256_srli_epi64(_mm256_castpd_si256(workarea),4))) ,
     workarea=_mm256_or_pd(workarea,_mm256_castsi256_pd(_mm256_srli_epi64(_mm256_castpd_si256(workarea),2))),
     _mm256_and_pd(_mm256_or_pd(workarea,_mm256_castsi256_pd(_mm256_srli_epi64(_mm256_castpd_si256(workarea),1))),bool256) ) , 
#endif
                           NETEMPLATE(neCS, B,UC,US, CMPNE )  NETEMPLATE(neSC, B,US,UC, CMPNE )  NETEMPLATE(neSS, B,S,S, CMPNE)
NETEMPLATE(neUU, B,C4,C4, CMPNE )  NETEMPLATE(neUS, B,C4,US, CMPNE )  NETEMPLATE(neSU, B,US,C4, CMPNE )
NETEMPLATE(neCU, B,UC,C4, CMPNE )  NETEMPLATE(neUC, B,C4,UC, CMPNE )
APFX(neAA, B,A,A, !equ )

                          APFX(ltBI, B,B,I, CMPLT    )  ACMP0(ltBD, B,B,D, TLT, <  )
APFX(ltIB, B,I,B, CMPLT    )    ACMP0(ltID, B,I,D, TLT, <  )
ACMP0(ltDB, B,D,B, TLT, <  )  ACMP0(ltDI, B,D,I, TLT, <  )  
APFX(ltXX, B,X,X, -1==xcompare)
APFX(ltQQ, B,Q,Q, QLT  )
APFX(ltSS, B,SB,SB, SBLT)

                          APFX(leBI, B,B,I, CMPLE   )  ACMP0(leBD, B,B,D, TLE, <=  )
APFX(leIB, B,I,B, CMPLE   )   ACMP0(leID, B,I,D, TLE, <=  )
ACMP0(leDB, B,D,B, TLE, <=  )  ACMP0(leDI, B,D,I, TLE, <=  )  
APFX(leXX, B,X,X,  1!=xcompare)
APFX(leQQ, B,Q,Q, QLE  )
APFX(leSS, B,SB,SB, SBLE)

                          APFX(geBI, B,B,I, CMPGE   )  ACMP0(geBD, B,B,D, TGE, >=  )
APFX(geIB, B,I,B, CMPGE   )    ACMP0(geID, B,I,D, TGE, >=  )
ACMP0(geDB, B,D,B, TGE, >=  )  ACMP0(geDI, B,D,I, TGE, >=  )  
APFX(geXX, B,X,X, -1!=xcompare)
APFX(geQQ, B,Q,Q, QGE  )
APFX(geSS, B,SB,SB, SBGE)

                           APFX(gtBI, B,B,I, CMPGT   )  ACMP0(gtBD, B,B,D, TGT, >  )
APFX(gtIB, B,I,B, CMPGT    )     ACMP0(gtID, B,I,D, TGT, >  )
ACMP0(gtDB, B,D,B, TGT, >  )   ACMP0(gtDI, B,D,I, TGT, > )  
APFX(gtXX, B,X,X,  1==xcompare)
APFX(gtQQ, B,Q,Q, QGT  )
APFX(gtSS, B,SB,SB, SBGT)

// 
#if C_AVX2&&SY_64
#define primcmpD256(name,tolres,intolres,decls) \
AHDR2(name,B,D,D){ \
 __m256i endmask; /* length mask for the last word */ \
 __m256d u,v,cct,ctop,eq; /* args, ct, eq result and also main result */ \
 decls \
 _mm256_zeroupper(VOIDARG); \
 if(jt->cct!=1.0){ \
  cct=_mm256_set1_pd(jt->cct); \
  if(n-1==0){ \
   /* vector-to-vector, no repetitions */ \
   endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-m)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
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
    /* atom+vector */ \
    endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-n)&(NPAR-1)))); \
    DQ(m, u=_mm256_set1_pd(*x); ++x; \
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
    /* vector+atom */ \
    endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-n)&(NPAR-1)))); \
    DQ(m, v=_mm256_set1_pd(*y); ++y; \
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
   /* vector-to-vector, no repetitions */ \
   endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-m)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
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
    /* atom+vector */ \
    endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-n)&(NPAR-1)))); \
    DQ(m,; u=_mm256_set1_pd(*x); ++x; \
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
    /* vector+atom */ \
    endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-n)&(NPAR-1)))); \
    DQ(m, v=_mm256_set1_pd(*y); ++y; \
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
}

primcmpD256(geDD, _mm256_or_pd(eq,_mm256_cmp_pd(u,v,_CMP_GT_OQ)) , _mm256_cmp_pd(u,v,_CMP_GE_OQ) , )
primcmpD256(gtDD, _mm256_andnot_pd(eq,_mm256_cmp_pd(u,v,_CMP_GT_OQ)) , _mm256_cmp_pd(u,v,_CMP_GT_OQ) , )
primcmpD256(leDD, _mm256_or_pd(eq,_mm256_cmp_pd(u,v,_CMP_LT_OQ)) , _mm256_cmp_pd(u,v,_CMP_LE_OQ) , )
primcmpD256(ltDD, _mm256_andnot_pd(eq,_mm256_cmp_pd(u,v,_CMP_LT_OQ)) , _mm256_cmp_pd(u,v,_CMP_LT_OQ) , )
primcmpD256(eqDD, eq , _mm256_cmp_pd(u,v,_CMP_EQ_OQ) , )
primcmpD256(neDD, _mm256_xor_pd(eq,one) , _mm256_cmp_pd(u,v,_CMP_NEQ_OQ) , __m256d one=_mm256_castsi256_pd(_mm256_set1_epi64x(0xffffffffffffffff));)   // warnings from one=_mm256_cmp_pd(cct,cct,_CMP_TRUE_UQ);

#define primcmpI256(name,result,decls) \
AHDR2(name,B,I,I){ \
 __m256i endmask; /* length mask for the last word */ \
 __m256i u,v,eq; /* args, ct, main result */ \
 decls \
 _mm256_zeroupper(VOIDARG); \
 if(n-1==0){ \
  /* vector-to-vector, no repetitions */ \
  endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-m)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
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
   endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-n)&(NPAR-1)))); \
   DQ(m,; u=_mm256_set1_epi64x(*x); ++x; \
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
   endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-n)&(NPAR-1)))); \
   DQ(m, v=_mm256_set1_epi64x(*y); ++y; \
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
}

primcmpI256(eqII, _mm256_cmpeq_epi64(u,v) , )
primcmpI256(neII, _mm256_xor_si256(one,_mm256_cmpeq_epi64(u,v)) , __m256i one=_mm256_set1_epi64x(0xffffffffffffffff);)
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

APFX(gtII, B,I,I, CMPGT   )
APFX(geII, B,I,I, CMPGE   )
APFX(leII, B,I,I, CMPLE   )
APFX(ltII, B,I,I, CMPLT    )
NETEMPLATE(neII, B,I,I, CMPNE   )
EQTEMPLATE(eqII, B,I,I, CMPEQ   )
#endif