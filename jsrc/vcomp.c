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
#if 0  // obsolete early equality result
#define EQTEMPLATE AIFX
#define NETEMPLATE APFX
#define CMPEQ ==
#define CMPNE(a,b) ((a)!=(b)?1:0)
#else  // setcc instruction, better if early equality not available
#define EQTEMPLATE APFX
#define NETEMPLATE APFX
#define CMPEQ(a,b) ((a)==(b)?1:0)
#define CMPNE(a,b) ((a)!=(b)?1:0)
#endif

B jtteq(J jt,D u,D v){R TCMPEQ(jt->cct,u,v);}

D jttfloor(J jt,D v){D x; R v<-4e35||4e35<v ? v : (x=jfloor(0.5+v), x-TGT(x,v));}
D jttceil (J jt,D v){D x; R v<-4e35||4e35<v ? v : (x=jfloor(0.5+v), x+TLT(x,v));}

BPFX(eqBB, EQ,BEQ,EQ,BEQ, _mm256_xor_pd(bool256,_mm256_xor_pd(u256,v256)) , ,__m256d bool256=_mm256_castsi256_pd(_mm256_set1_epi64x(0x0101010101010101));)
BPFX(neBB, NE,BNE,NE,BNE, _mm256_xor_pd(u256,v256) , ,)
BPFX(ltBB, LT,BLT,GT,BGT, _mm256_andnot_pd(u256,v256) , ,)
BPFX(leBB, LE,BLE,GE,BGE, _mm256_xor_pd(bool256,_mm256_andnot_pd(v256,u256)) , ,__m256d bool256=_mm256_castsi256_pd(_mm256_set1_epi64x(0x0101010101010101));)
BPFX(geBB, GE,BGE,LE,BLE, _mm256_xor_pd(bool256,_mm256_andnot_pd(u256,v256)) , ,__m256d bool256=_mm256_castsi256_pd(_mm256_set1_epi64x(0x0101010101010101));)
BPFX(gtBB, GT,BGT,LT,BLT, _mm256_andnot_pd(v256,u256) , , )
 

                          EQTEMPLATE(eqBI, B,B,I, CMPEQ   )  ACMP0(eqBD, B,B,D, TEQ, ==  )
EQTEMPLATE(eqIB, B,I,B, CMPEQ   )  EQTEMPLATE(eqII, B,I,I, CMPEQ   )  ACMP0(eqID, B,I,D, TEQ, ==  )
ACMP0(eqDB, B,D,B, TEQ, ==  )  ACMP0(eqDI, B,D,I, TEQ, ==  )  ACMP0(eqDD, B,D,D, TEQ, ==  )
APFX(eqZZ, B,Z,Z, zeq  )   
APFX(eqXX, B,X,X, equ  )   
APFX(eqQQ, B,Q,Q, QEQ  )
BPFXAVX2(eqCC, CMPEQCC,x, CMPEQCC, x,
 (workarea=_mm256_xor_pd(u256,v256), workarea=_mm256_or_pd(workarea,_mm256_castsi256_pd(_mm256_srli_epi64(_mm256_castpd_si256(workarea),4))) ,
     workarea=_mm256_or_pd(workarea,_mm256_castsi256_pd(_mm256_srli_epi64(_mm256_castpd_si256(workarea),2))),
     _mm256_andnot_pd(_mm256_or_pd(workarea,_mm256_castsi256_pd(_mm256_srli_epi64(_mm256_castpd_si256(workarea),1))),bool256) ) , 
   I work; , __m256d workarea; __m256d bool256=_mm256_castsi256_pd(_mm256_set1_epi64x(0x0101010101010101));
   )
                           EQTEMPLATE(eqCS, B,UC,US, CMPEQ )  EQTEMPLATE(eqSC, B,US,UC, CMPEQ )  EQTEMPLATE(eqSS, B,S,S, CMPEQ)
EQTEMPLATE(eqUU, B,C4,C4, CMPEQ )  EQTEMPLATE(eqUS, B,C4,US, CMPEQ )  EQTEMPLATE(eqSU, B,US,C4, CMPEQ )
EQTEMPLATE(eqCU, B,UC,C4, CMPEQ )  EQTEMPLATE(eqUC, B,C4,UC, CMPEQ )
APFX(eqAA, B,A,A, equ  )

                          NETEMPLATE(neBI, B,B,I, CMPNE   )  ACMP0(neBD, B,B,D, TNE, != )
NETEMPLATE(neIB, B,I,B, CMPNE   )  NETEMPLATE(neII, B,I,I, CMPNE   )  ACMP0(neID, B,I,D, TNE, != )
ACMP0(neDB, B,D,B, TNE, != )  ACMP0(neDI, B,D,I, TNE, != )  ACMP0(neDD, B,D,D, TNE, != )
APFX(neZZ, B,Z,Z, !zeq )  
APFX(neXX, B,X,X, !equ )
APFX(neQQ, B,Q,Q, !QEQ )
BPFXAVX2(neCC, CMPNECC,x, CMPNECC, x,
 (workarea=_mm256_xor_pd(u256,v256), workarea=_mm256_or_pd(workarea,_mm256_castsi256_pd(_mm256_srli_epi64(_mm256_castpd_si256(workarea),4))) ,
     workarea=_mm256_or_pd(workarea,_mm256_castsi256_pd(_mm256_srli_epi64(_mm256_castpd_si256(workarea),2))),
     _mm256_and_pd(_mm256_or_pd(workarea,_mm256_castsi256_pd(_mm256_srli_epi64(_mm256_castpd_si256(workarea),1))),bool256) ) , 
   I work; , __m256d workarea; __m256d bool256=_mm256_castsi256_pd(_mm256_set1_epi64x(0x0101010101010101));
   )
                           NETEMPLATE(neCS, B,UC,US, CMPNE )  NETEMPLATE(neSC, B,US,UC, CMPNE )  NETEMPLATE(neSS, B,S,S, CMPNE)
NETEMPLATE(neUU, B,C4,C4, CMPNE )  NETEMPLATE(neUS, B,C4,US, CMPNE )  NETEMPLATE(neSU, B,US,C4, CMPNE )
NETEMPLATE(neCU, B,UC,C4, CMPNE )  NETEMPLATE(neUC, B,C4,UC, CMPNE )
APFX(neAA, B,A,A, !equ )

                          APFX(ltBI, B,B,I, CMPLT    )  ACMP0(ltBD, B,B,D, TLT, <  )
APFX(ltIB, B,I,B, CMPLT    )  APFX(ltII, B,I,I, CMPLT    )  ACMP0(ltID, B,I,D, TLT, <  )
ACMP0(ltDB, B,D,B, TLT, <  )  ACMP0(ltDI, B,D,I, TLT, <  )  ACMP0(ltDD, B,D,D, TLT, <  )
APFX(ltXX, B,X,X, -1==xcompare)
APFX(ltQQ, B,Q,Q, QLT  )
APFX(ltSS, B,SB,SB, SBLT)

                          APFX(leBI, B,B,I, CMPLE   )  ACMP0(leBD, B,B,D, TLE, <=  )
APFX(leIB, B,I,B, CMPLE   )  APFX(leII, B,I,I, CMPLE   )  ACMP0(leID, B,I,D, TLE, <=  )
ACMP0(leDB, B,D,B, TLE, <=  )  ACMP0(leDI, B,D,I, TLE, <=  )  ACMP0(leDD, B,D,D, TLE, <=  )
APFX(leXX, B,X,X,  1!=xcompare)
APFX(leQQ, B,Q,Q, QLE  )
APFX(leSS, B,SB,SB, SBLE)

                          APFX(geBI, B,B,I, CMPGE   )  ACMP0(geBD, B,B,D, TGE, >=  )
APFX(geIB, B,I,B, CMPGE   )  APFX(geII, B,I,I, CMPGE   )  ACMP0(geID, B,I,D, TGE, >=  )
ACMP0(geDB, B,D,B, TGE, >=  )  ACMP0(geDI, B,D,I, TGE, >=  )  ACMP0(geDD, B,D,D, TGE, >=  )
APFX(geXX, B,X,X, -1!=xcompare)
APFX(geQQ, B,Q,Q, QGE  )
APFX(geSS, B,SB,SB, SBGE)

                           APFX(gtBI, B,B,I, CMPGT   )  ACMP0(gtBD, B,B,D, TGT, >  )
APFX(gtIB, B,I,B, CMPGT    )   APFX(gtII, B,I,I, CMPGT   )  ACMP0(gtID, B,I,D, TGT, >  )
ACMP0(gtDB, B,D,B, TGT, >  )   ACMP0(gtDI, B,D,I, TGT, > )  ACMP0(gtDD, B,D,D, TGT, >  )
APFX(gtXX, B,X,X,  1==xcompare)
APFX(gtQQ, B,Q,Q, QGT  )
APFX(gtSS, B,SB,SB, SBGT)
