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
#if 1  // early equality result
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

#if 0
B jtteq(J jt,D u,D v){D d,s;
 if(u==v)R 1;
 else R 0<u==0<v && 
    ((0<u ? (u>v ? (s= u,d=u-v) : (s= v,d=v-u))
          : (u<v ? (s=-u,d=v-u) : (s=-v,d=u-v)) ), d<=s*jt->ct && s!=inf );
}
B jttlt(J jt,D u,D v){R u<v && !teq(u,v);}
#else
B jtteq(J jt,D u,D v){R TCMPEQ(1.0-jt->ct,u,v);}
#endif

D jttfloor(J jt,D v){D x; R v<-4e35||4e35<v ? v : (x=jfloor(0.5+v), x-TGT(x,v));}
D jttceil (J jt,D v){D x; R v<-4e35||4e35<v ? v : (x=jfloor(0.5+v), x+TLT(x,v));}


BPFX(eqBB, EQ,BEQ,EQ,BEQ)
BPFX(neBB, NE,BNE,NE,BNE)
BPFX(ltBB, LT,BLT,GT,BGT)
BPFX(leBB, LE,BLE,GE,BGE)
BPFX(geBB, GE,BGE,LE,BLE)
BPFX(gtBB, GT,BGT,LT,BLT)

// AIFX with == compiles down to CMP/jne.  APFX with CMPEQ compiles down to cmp/sete.  On the Intel Core i3-3120M that I am testing on,
// the test for integer equality seems to be moved up in the pipeline far enough to avoid branch misprediction, and becomes the winner (by 5%).
// This needs to be reanalyzed from time to time.  Oddly, je is sped up, but not jne
//
// All the other tests show a big win (300%) for the setcc instruction

                          EQTEMPLATE(eqBI, B,B,I, CMPEQ   )  ACMP(eqBD, B,B,D, TEQ  )
EQTEMPLATE(eqIB, B,I,B, CMPEQ   )  EQTEMPLATE(eqII, B,I,I, CMPEQ   )  ACMP(eqID, B,I,D, TEQ  )
ACMP(eqDB, B,D,B, TEQ  )  ACMP(eqDI, B,D,I, TEQ  )  ACMP(eqDD, B,D,D, TEQ  )
APFX(eqZZ, B,Z,Z, zeq  )   
APFX(eqXX, B,X,X, equ  )   
APFX(eqQQ, B,Q,Q, QEQ  )
EQTEMPLATE(eqCC, B,C,C, CMPEQ   )  EQTEMPLATE(eqCS, B,UC,US, CMPEQ )  EQTEMPLATE(eqSC, B,US,UC, CMPEQ )  EQTEMPLATE(eqSS, B,S,S, CMPEQ)
EQTEMPLATE(eqUU, B,C4,C4, CMPEQ )  EQTEMPLATE(eqUS, B,C4,US, CMPEQ )  EQTEMPLATE(eqSU, B,US,C4, CMPEQ )
EQTEMPLATE(eqCU, B,UC,C4, CMPEQ )  EQTEMPLATE(eqUC, B,C4,UC, CMPEQ )
APFY(eqAA, B,A,A, equ  )

                          NETEMPLATE(neBI, B,B,I, CMPNE   )  ACMP(neBD, B,B,D, TNE )
NETEMPLATE(neIB, B,I,B, CMPNE   )  NETEMPLATE(neII, B,I,I, CMPNE   )  ACMP(neID, B,I,D, TNE )
ACMP(neDB, B,D,B, TNE )  ACMP(neDI, B,D,I, TNE )  ACMP(neDD, B,D,D, TNE )
APFX(neZZ, B,Z,Z, !zeq )  
APFX(neXX, B,X,X, !equ )
APFX(neQQ, B,Q,Q, !QEQ )
NETEMPLATE(neCC, B,C,C, CMPNE   )  NETEMPLATE(neCS, B,UC,US, CMPNE )  NETEMPLATE(neSC, B,US,UC, CMPNE )  NETEMPLATE(neSS, B,S,S, CMPNE)
NETEMPLATE(neUU, B,C4,C4, CMPNE )  NETEMPLATE(neUS, B,C4,US, CMPNE )  NETEMPLATE(neSU, B,US,C4, CMPNE )
NETEMPLATE(neCU, B,UC,C4, CMPNE )  NETEMPLATE(neUC, B,C4,UC, CMPNE )
APFY(neAA, B,A,A, !equ )

                          APFX(ltBI, B,B,I, CMPLT    )  ACMP(ltBD, B,B,D, TLT  )
APFX(ltIB, B,I,B, CMPLT    )  APFX(ltII, B,I,I, CMPLT    )  ACMP(ltID, B,I,D, TLT  )
ACMP(ltDB, B,D,B, TLT  )  ACMP(ltDI, B,D,I, TLT  )  ACMP(ltDD, B,D,D, TLT  )
APFX(ltXX, B,X,X, -1==xcompare)
APFX(ltQQ, B,Q,Q, QLT  )
APFX(ltSS, B,SB,SB, SBLT)

                          APFX(leBI, B,B,I, CMPLE   )  ACMP(leBD, B,B,D, TLE  )
APFX(leIB, B,I,B, CMPLE   )  APFX(leII, B,I,I, CMPLE   )  ACMP(leID, B,I,D, TLE  )
ACMP(leDB, B,D,B, TLE  )  ACMP(leDI, B,D,I, TLE  )  ACMP(leDD, B,D,D, TLE  )
APFX(leXX, B,X,X,  1!=xcompare)
APFX(leQQ, B,Q,Q, QLE  )
APFX(leSS, B,SB,SB, SBLE)

                          APFX(geBI, B,B,I, CMPGE   )  ACMP(geBD, B,B,D, TGE  )
APFX(geIB, B,I,B, CMPGE   )  APFX(geII, B,I,I, CMPGE   )  ACMP(geID, B,I,D, TGE  )
ACMP(geDB, B,D,B, TGE  )  ACMP(geDI, B,D,I, TGE  )  ACMP(geDD, B,D,D, TGE  )
APFX(geXX, B,X,X, -1!=xcompare)
APFX(geQQ, B,Q,Q, QGE  )
APFX(geSS, B,SB,SB, SBGE)

                           APFX(gtBI, B,B,I, CMPGT   )  ACMP(gtBD, B,B,D, TGT  )
APFX(gtIB, B,I,B, CMPGT    )   APFX(gtII, B,I,I, CMPGT   )  ACMP(gtID, B,I,D, TGT  )
ACMP(gtDB, B,D,B, TGT  )   ACMP(gtDI, B,D,I, TGT )  ACMP(gtDD, B,D,D, TGT  )
APFX(gtXX, B,X,X,  1==xcompare)
APFX(gtQQ, B,Q,Q, QGT  )
APFX(gtSS, B,SB,SB, SBGT)
