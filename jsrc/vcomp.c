/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
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
// for E, if the high part has no fractional significance, the whole must be tolerantly equal to an integer, and we round off the fraction.
// otherwise the low part must be tiny and we zero it, floor/ceiling the upper
E jtefloor(J jt,E v){D h,l,r; if(unlikely(ABS(v.hi)>=4503599627370496.)){if(unlikely(ABS(v.hi)==inf))R v; r=jround(v.lo); TWOSUMBS1(v.hi,r,h,l) E z; z=CANONE1(h,l); R z; } else{r=tfloor(v.hi); if(r==v.hi)r+=tfloor(v.lo); R (E){r,0*r};}  }
E jteceil(J jt,E v){D h,l,r; if(unlikely(ABS(v.hi)>=4503599627370496.)){if(unlikely(ABS(v.hi)==inf))R v; r=jround(v.lo); TWOSUMBS1(v.hi,r,h,l) E z; z=CANONE1(h,l); R z; } else{r=tceil(v.hi); if(r==v.hi)r+=tceil(v.lo); R (E){r,0*r};}  }

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
APFX(eqXX, B,X,X, equx,, R EVOK;)   
APFX(eqQQ, B,Q,Q, QEQ,, R EVOK;)
BPFXAVX2(eqCC, CMPEQCC,x, CMPEQCC, x,
   (workarea=_mm256_castpd_si256(_mm256_xor_pd(u256,v256)), _mm256_castsi256_pd(_mm256_and_si256(_mm256_srli_epi64(_mm256_andnot_si256(workarea,_mm256_sub_epi8(workarea,bool256)),7),bool256))) ,
   I work; , __m256i workarea; __m256i bool256=_mm256_castpd_si256(_mm256_broadcast_sd((D*)&Ivalidboolean));
   )

EQTEMPLATE(eqCS, B,UC,US, CMPEQ,, R EVOK;)  EQTEMPLATE(eqSC, B,US,UC, CMPEQ,, R EVOK;)  EQTEMPLATE(eqSS, B,S,S, CMPEQ,, R EVOK;)
EQTEMPLATE(eqUU, B,C4,C4, CMPEQ,, R EVOK;)  EQTEMPLATE(eqUS, B,C4,US, CMPEQ,, R EVOK;)  EQTEMPLATE(eqSU, B,US,C4, CMPEQ,, R EVOK;)
EQTEMPLATE(eqCU, B,UC,C4, CMPEQ,, R EVOK;)  EQTEMPLATE(eqUC, B,C4,UC, CMPEQ,, R EVOK;)
APFXL(eqAA, B,A,A,CERR,equ,, R jt->jerr?jt->jerr:EVOK;)

                          NETEMPLATE(neBI, B,B,I, CMPNE,, R EVOK;)  ACMP0(neBD, B,B,D, TNE, != )
NETEMPLATE(neIB, B,I,B, CMPNE,, R EVOK;)    ACMP0(neID, B,I,D, TNE, != )
ACMP0(neDB, B,D,B, TNE, != )  ACMP0(neDI, B,D,I, TNE, != )  
APFX(neZZ, B,Z,Z, !zeq,, R EVOK;)  
APFX(neXX, B,X,X, !equx,, R EVOK;)
APFX(neQQ, B,Q,Q, !QEQ,, R EVOK;)
BPFXAVX2(neCC, CMPNECC,x, CMPNECC, x,
   (workarea=_mm256_castpd_si256(_mm256_xor_pd(u256,v256)), _mm256_castsi256_pd(_mm256_andnot_si256(_mm256_srli_epi64(_mm256_andnot_si256(workarea,_mm256_sub_epi8(workarea,bool256)),7),bool256))) ,
   I work; , __m256i workarea; __m256i bool256=_mm256_castpd_si256(_mm256_broadcast_sd((D*)&Ivalidboolean));
   )
NETEMPLATE(neCS, B,UC,US, CMPNE,, R EVOK;)  NETEMPLATE(neSC, B,US,UC, CMPNE,, R EVOK;)  NETEMPLATE(neSS, B,S,S, CMPNE,, R EVOK;)
NETEMPLATE(neUU, B,C4,C4, CMPNE,, R EVOK;)  NETEMPLATE(neUS, B,C4,US, CMPNE,, R EVOK;)  NETEMPLATE(neSU, B,US,C4, CMPNE,, R EVOK;)
NETEMPLATE(neCU, B,UC,C4, CMPNE,, R EVOK;)  NETEMPLATE(neUC, B,C4,UC, CMPNE,, R EVOK;)
APFXL(neAA, B,A,A,CERR,!equ,, R jt->jerr?jt->jerr:EVOK;)

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
#define PCOMPDO(zzop,xy,lineno,LOADFN)  if(xy&2)u=_mm256_loadu_pd(x+(lineno)*NPAR); if(xy&1)v=LOADFN(_mm256_loadu_pd(y+(lineno)*NPAR)); \
     *(I4*)(z+(lineno)*NPAR)=VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(zzop));

#define PCOMPINCR(ct,xy) if(xy&2)x+=(ct); if(xy&1)y+=(ct); z+=(ct);

#define PCOMPDUFF(zzop,len,xy,LOADFN) \
      UI n2=DUFFLPCT((len)-1,3);  /* # turns through duff loop */ \
      if(n2>0){ \
       UI backoff=DUFFBACKOFF((len)-1,3); \
       PCOMPINCR((backoff+1)*NPAR,xy) \
       switch(backoff){ \
       do{ \
       case -1: PCOMPDO(zzop,xy,0,LOADFN) case -2: PCOMPDO(zzop,xy,1,LOADFN) case -3: PCOMPDO(zzop,xy,2,LOADFN) case -4: PCOMPDO(zzop,xy,3,LOADFN) \
       case -5: PCOMPDO(zzop,xy,4,LOADFN) case -6: PCOMPDO(zzop,xy,5,LOADFN) case -7: PCOMPDO(zzop,xy,6,LOADFN) case -8: PCOMPDO(zzop,xy,7,LOADFN) \
       PCOMPINCR(8*NPAR,xy) \
       }while(--n2!=0); \
       } \
      } 


#define PCOMPMASK(zzop,xy,len,LOADFN) if(xy&2)u=_mm256_maskload_pd(x,endmask); if(xy&1)v=LOADFN(_mm256_maskload_pd(y,endmask)); \
   STOREBYTES(z,VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(zzop)),((-(len))&(NPAR-1))+NPAR);  /* could just overstore */

#define PCOMPID(y) y
#define PCOMPABS(y) _mm256_and_pd(y,absmask)

#if C_AVX2 || EMU_AVX2
#define primcmpD256(name,tolres,intolres,decls,LOADFN) \
AHDR2(name,B,D,D){ \
 __m256i endmask; /* length mask for the last word */ \
 __m256d u,v,cct,ctop,eq; /* args, ct, eq result and also main result */ \
 decls \
 if(jt->cct!=1.0){  /* tolerant */ \
  cct=_mm256_broadcast_sd(&jt->cct); \
  if(m<0){m=~m; \
   /* vector op vector, no repetitions */ \
   endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-m)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
   DQ((m-1)>>LGNPAR, \
     u=_mm256_loadu_pd(x);v=LOADFN(_mm256_loadu_pd(y)); \
     eq=_mm256_xor_pd(_mm256_cmp_pd(u,_mm256_mul_pd(v,cct),_CMP_GT_OQ),_mm256_cmp_pd(v,_mm256_mul_pd(u,cct),_CMP_LE_OQ)); \
     eq=tolres; \
     *(I4*)z=VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)); \
     x+=NPAR; y+=NPAR; z+=NPAR;) \
   /* runout, using mask */ \
   u=_mm256_maskload_pd(x,endmask);v=LOADFN(_mm256_maskload_pd(y,endmask)); \
   eq=_mm256_xor_pd(_mm256_cmp_pd(u,_mm256_mul_pd(v,cct),_CMP_GT_OQ),_mm256_cmp_pd(v,_mm256_mul_pd(u,cct),_CMP_LE_OQ)); \
   eq=tolres; \
   STOREBYTES(z,VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)),((-m)&(NPAR-1))+NPAR);  /* could just overstore */ \
  }else{ \
   if(m&1){m>>=1; \
    /* atom op vector */ \
    if(n==1 && *x==0.0)goto name##av0;  /* if comparing against 0, switch to intolerant */ \
    endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-m)&(NPAR-1)))); \
    DQU(n, u=_mm256_broadcast_sd(x); ++x; \
      ctop=_mm256_mul_pd(u,cct); \
      DQ((m-1)>>LGNPAR, \
        v=LOADFN(_mm256_loadu_pd(y)); \
        eq=_mm256_xor_pd(_mm256_cmp_pd(u,_mm256_mul_pd(v,cct),_CMP_GT_OQ),_mm256_cmp_pd(v,ctop,_CMP_LE_OQ)); \
        eq=tolres; \
        *(I4*)z=VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)); \
        y+=NPAR; z+=NPAR;) \
      v=LOADFN(_mm256_maskload_pd(y,endmask)); \
      eq=_mm256_xor_pd(_mm256_cmp_pd(u,_mm256_mul_pd(v,cct),_CMP_GT_OQ),_mm256_cmp_pd(v,ctop,_CMP_LE_OQ)); \
      eq=tolres; \
      STOREBYTES(z,VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)),((-m)&(NPAR-1))+NPAR);  /* could just overstore */ \
      y+=((m-1)&(NPAR-1))+1; z+=((m-1)&(NPAR-1))+1;) \
   }else{m>>=1; \
    /* vector op atom */ \
    if(n==1 && *y==0.0)goto name##va0;  /* if comparing against 0, switch to intolerant */ \
    endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-m)&(NPAR-1)))); \
    DQU(n, v=LOADFN(_mm256_broadcast_sd(y)); ++y; \
      ctop=_mm256_mul_pd(v,cct); \
      DQ((m-1)>>LGNPAR, \
        u=_mm256_loadu_pd(x); \
        eq=_mm256_xor_pd(_mm256_cmp_pd(u,ctop,_CMP_GT_OQ),_mm256_cmp_pd(v,_mm256_mul_pd(u,cct),_CMP_LE_OQ)); \
        eq=tolres; \
        *(I4*)z=VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)); \
        x+=NPAR; z+=NPAR;) \
      u=_mm256_maskload_pd(x,endmask); \
      eq=_mm256_xor_pd(_mm256_cmp_pd(u,ctop,_CMP_GT_OQ),_mm256_cmp_pd(v,_mm256_mul_pd(u,cct),_CMP_LE_OQ)); \
      eq=tolres; \
      STOREBYTES(z,VALIDBOOLEAN&_mm256_movemask_epi8(_mm256_castpd_si256(eq)),((-m)&(NPAR-1))+NPAR);  /* could just overstore */ \
      x+=((m-1)&(NPAR-1))+1; z+=((m-1)&(NPAR-1))+1;) \
   } \
  } \
 }else{  /* intolerant */ \
  if(m<0){m=~m; \
   /* vector op!.0 vector, no repetitions */ \
   endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-m)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
   PCOMPDUFF(intolres,m,3,LOADFN) \
   PCOMPMASK(intolres,3,m,LOADFN) /* runout, using mask */ \
  }else{ \
   if(m&1){m>>=1; \
    name##av0: /* atom op!.0 vector */ \
    endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-m)&(NPAR-1)))); \
    DQU(n, u=_mm256_broadcast_sd(x); ++x; \
     PCOMPDUFF(intolres,m,1,LOADFN) \
     PCOMPMASK(intolres,1,m,LOADFN) /* runout, using mask */ \
     PCOMPINCR(((m-1)&(NPAR-1))+1,1) \
    ) \
   }else{m>>=1; \
    name##va0: /* vector op!.0 atom */ \
    endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-m)&(NPAR-1)))); \
    DQU(n, v=LOADFN(_mm256_broadcast_sd(y)); ++y; \
     PCOMPDUFF(intolres,m,2,LOADFN) \
     PCOMPMASK(intolres,2,m,LOADFN) /* runout, using mask */ \
     PCOMPINCR(((m-1)&(NPAR-1))+1,2) \
    ) \
   } \
  } \
 } \
 R EVOK; \
}

primcmpD256(geDD, _mm256_or_pd(eq,_mm256_cmp_pd(u,v,_CMP_GE_OQ)) , _mm256_cmp_pd(u,v,_CMP_GE_OQ) , , PCOMPID)
primcmpD256(gtDD, _mm256_andnot_pd(eq,_mm256_cmp_pd(u,v,_CMP_GT_OQ)) , _mm256_cmp_pd(u,v,_CMP_GT_OQ) ,  , PCOMPID)
primcmpD256(leDD, _mm256_or_pd(eq,_mm256_cmp_pd(u,v,_CMP_LE_OQ)) , _mm256_cmp_pd(u,v,_CMP_LE_OQ) ,  , PCOMPID)
primcmpD256(ltDD, _mm256_andnot_pd(eq,_mm256_cmp_pd(u,v,_CMP_LT_OQ)) , _mm256_cmp_pd(u,v,_CMP_LT_OQ) ,  , PCOMPID)
primcmpD256(eqDD, eq , _mm256_cmp_pd(u,v,_CMP_EQ_OQ) ,  , PCOMPID)
primcmpD256(neDD, _mm256_xor_pd(eq,one) , _mm256_cmp_pd(u,v,_CMP_NEQ_OQ) , __m256d one=_mm256_broadcast_sd((D*)&validitymask); , PCOMPID)   // warnings from one=_mm256_cmp_pd(cct,cct,_CMP_TRUE_UQ);
primcmpD256(geabsDD, _mm256_or_pd(eq,_mm256_cmp_pd(u,v,_CMP_GE_OQ)) , _mm256_cmp_pd(u,v,_CMP_GE_OQ) , __m256d absmask=_mm256_broadcast_sd((D*)&Iimax); , PCOMPABS)
primcmpD256(gtabsDD, _mm256_andnot_pd(eq,_mm256_cmp_pd(u,v,_CMP_GT_OQ)) , _mm256_cmp_pd(u,v,_CMP_GT_OQ) , __m256d absmask=_mm256_broadcast_sd((D*)&Iimax); , PCOMPABS)
primcmpD256(leabsDD, _mm256_or_pd(eq,_mm256_cmp_pd(u,v,_CMP_LE_OQ)) , _mm256_cmp_pd(u,v,_CMP_LE_OQ) , __m256d absmask=_mm256_broadcast_sd((D*)&Iimax); , PCOMPABS)
primcmpD256(ltabsDD, _mm256_andnot_pd(eq,_mm256_cmp_pd(u,v,_CMP_LT_OQ)) , _mm256_cmp_pd(u,v,_CMP_LT_OQ) , __m256d absmask=_mm256_broadcast_sd((D*)&Iimax); , PCOMPABS)
primcmpD256(eqabsDD, eq , _mm256_cmp_pd(u,v,_CMP_EQ_OQ) , __m256d absmask=_mm256_broadcast_sd((D*)&Iimax); , PCOMPABS)
primcmpD256(neabsDD, _mm256_xor_pd(eq,one) , _mm256_cmp_pd(u,v,_CMP_NEQ_OQ) , __m256d one=_mm256_broadcast_sd((D*)&validitymask); __m256d absmask=_mm256_broadcast_sd((D*)&Iimax); , PCOMPABS)   // warnings from one=_mm256_cmp_pd(cct,cct,_CMP_TRUE_UQ);

#define primcmpI256(name,result,decls) \
AHDR2(name,B,I,I){ \
 __m256i endmask; /* length mask for the last word */ \
 __m256i u,v,eq; /* args, ct, main result */ \
 decls \
 if(m<0){m=~m; \
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
  if(m&1){m>>=1; \
   /* atom+vector */ \
   endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-m)&(NPAR-1)))); \
   DQU(n,; u=_mm256_castpd_si256(_mm256_broadcast_sd((D*)x)); ++x; \
     DQ((m-1)>>LGNPAR, \
       v=_mm256_loadu_si256((__m256i*)y); \
       eq=result; \
       *(I4*)z=VALIDBOOLEAN&_mm256_movemask_epi8(eq); \
       y+=NPAR; z+=NPAR;) \
     v=_mm256_maskload_epi64(y,endmask); \
     eq=result; \
     STOREBYTES(z,VALIDBOOLEAN&_mm256_movemask_epi8(eq),((-m)&(NPAR-1))+NPAR);  /* could just overstore */ \
     y+=((m-1)&(NPAR-1))+1; z+=((m-1)&(NPAR-1))+1;) \
  }else{m>>=1; \
   /* vector+atom */ \
   endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-m)&(NPAR-1)))); \
   DQU(n, v=_mm256_castpd_si256(_mm256_broadcast_sd((D*)y)); ++y; \
     DQ((m-1)>>LGNPAR, \
       u=_mm256_loadu_si256((__m256i*)x); \
       eq=result; \
       *(I4*)z=VALIDBOOLEAN&_mm256_movemask_epi8(eq); \
       x+=NPAR; z+=NPAR;) \
     u=_mm256_maskload_epi64(x,endmask); \
     eq=result; \
     STOREBYTES(z,VALIDBOOLEAN&_mm256_movemask_epi8(eq),((-m)&(NPAR-1))+NPAR);  /* could just overstore */ \
     x+=((m-1)&(NPAR-1))+1; z+=((m-1)&(NPAR-1))+1;) \
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
ACMP0ABS(geabsDD, B,D,D, TGE, >=  )
ACMP0ABS(gtabsDD, B,D,D, TGT, >  )
ACMP0ABS(leabsDD, B,D,D, TLE, <=  )
ACMP0ABS(ltabsDD, B,D,D, TLT, <  )
ACMP0ABS(neabsDD, B,D,D, TNE, != )
ACMP0ABS(eqabsDD, B,D,D, TEQ, ==  )

APFX(gtII, B,I,I, CMPGT,,R EVOK;)
APFX(geII, B,I,I, CMPGE,,R EVOK;)
APFX(leII, B,I,I, CMPLE,,R EVOK;)
APFX(ltII, B,I,I, CMPLT,,R EVOK;)
NETEMPLATE(neII, B,I,I, CMPNE,,R EVOK;)
EQTEMPLATE(eqII, B,I,I, CMPEQ,,R EVOK;)
#endif
APFX(gtI2I2, B,I2,I2, CMPGT,,R EVOK;)
APFX(geI2I2, B,I2,I2, CMPGE,,R EVOK;)
APFX(leI2I2, B,I2,I2, CMPLE,,R EVOK;)
APFX(ltI2I2, B,I2,I2, CMPLT,,R EVOK;)
NETEMPLATE(neI2I2, B,I2,I2, CMPNE,,R EVOK;)
EQTEMPLATE(eqI2I2, B,I2,I2, CMPEQ,,R EVOK;)
APFX(gtI4I4, B,I4,I4, CMPGT,,R EVOK;)
APFX(geI4I4, B,I4,I4, CMPGE,,R EVOK;)
APFX(leI4I4, B,I4,I4, CMPLE,,R EVOK;)
APFX(ltI4I4, B,I4,I4, CMPLT,,R EVOK;)
NETEMPLATE(neI4I4, B,I4,I4, CMPNE,,R EVOK;)
EQTEMPLATE(eqI4I4, B,I4,I4, CMPEQ,,R EVOK;)
ACMP0T( eqEE, B,E,E,E, TEQE,EQE0)
ACMP0T( neEE, B,E,E,E, TNEE,NEE0)
ACMP0T( ltEE, B,E,E,E, TLTE,LTE0)
ACMP0T( leEE, B,E,E,E, TLEE,LEE0)
ACMP0T( gtEE, B,E,E,E, TGTE,GTE0)
ACMP0T( geEE, B,E,E,E, TGEE,GEE0)
