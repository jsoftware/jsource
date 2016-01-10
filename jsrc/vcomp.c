/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Comparatives                                                     */

#include "j.h"
#include "ve.h"
#include "vcomp.h"


B jtteq(J jt,D u,D v){D d,s;
 if(u==v)R 1;
 else R 0<u==0<v && 
    ((0<u ? (u>v ? (s= u,d=u-v) : (s= v,d=v-u))
          : (u<v ? (s=-u,d=v-u) : (s=-v,d=u-v)) ), d<=s*jt->ct && s!=inf );
}

B jttlt(J jt,D u,D v){R u<v && !teq(u,v);}

D jttfloor(J jt,D v){D x; R v<-4e35||4e35<v ? v : (x=jfloor(0.5+v), x-TGT(x,v));}
D jttceil (J jt,D v){D x; R v<-4e35||4e35<v ? v : (x=jfloor(0.5+v), x+TLT(x,v));}


BPFX(eqBB, EQ,BEQ,EQ,BEQ)
BPFX(neBB, NE,BNE,NE,BNE)
BPFX(ltBB, LT,BLT,GT,BGT)
BPFX(leBB, LE,BLE,GE,BGE)
BPFX(geBB, GE,BGE,LE,BLE)
BPFX(gtBB, GT,BGT,LT,BLT)

                          AIFX(eqBI, B,B,I, ==   )  ACMP(eqBD, B,B,D, teq  )
AIFX(eqIB, B,I,B, ==   )  AIFX(eqII, B,I,I, ==   )  ACMP(eqID, B,I,D, teq  )
ACMP(eqDB, B,D,B, teq  )  ACMP(eqDI, B,D,I, teq  )  ACMP(eqDD, B,D,D, teq  )
APFX(eqZZ, B,Z,Z, zeq  )   
APFX(eqXX, B,X,X, equ  )   
APFX(eqQQ, B,Q,Q, QEQ  )
AIFX(eqCC, B,C,C, ==   )  AIFX(eqCS, B,UC,US, == )  AIFX(eqSC, B,US,UC, == )  AIFX(eqSS, B,S,S, ==)
APFY(eqAA, B,A,A, equ  )

                          AIFX(neBI, B,B,I, !=   )  ACMP(neBD, B,B,D, !teq )
AIFX(neIB, B,I,B, !=   )  AIFX(neII, B,I,I, !=   )  ACMP(neID, B,I,D, !teq )
ACMP(neDB, B,D,B, !teq )  ACMP(neDI, B,D,I, !teq )  ACMP(neDD, B,D,D, !teq )
APFX(neZZ, B,Z,Z, !zeq )  
APFX(neXX, B,X,X, !equ )
APFX(neQQ, B,Q,Q, !QEQ )
AIFX(neCC, B,C,C, !=   )  AIFX(neCS, B,UC,US, != )  AIFX(neSC, B,US,UC, != )  AIFX(neSS, B,S,S, !=)
APFY(neAA, B,A,A, !equ )

                          AIFX(ltBI, B,B,I, <    )  ACMP(ltBD, B,B,D, TLT  )
AIFX(ltIB, B,I,B, <    )  AIFX(ltII, B,I,I, <    )  ACMP(ltID, B,I,D, TLT  )
ACMP(ltDB, B,D,B, TLT  )  ACMP(ltDI, B,D,I, TLT  )  ACMP(ltDD, B,D,D, TLT  )
APFX(ltXX, B,X,X, -1==xcompare)
APFX(ltQQ, B,Q,Q, QLT  )
APFX(ltSS, B,SB,SB, SBLT)

                          AIFX(leBI, B,B,I, <=   )  ACMP(leBD, B,B,D, TLE  )
AIFX(leIB, B,I,B, <=   )  AIFX(leII, B,I,I, <=   )  ACMP(leID, B,I,D, TLE  )
ACMP(leDB, B,D,B, TLE  )  ACMP(leDI, B,D,I, TLE  )  ACMP(leDD, B,D,D, TLE  )
APFX(leXX, B,X,X,  1!=xcompare)
APFX(leQQ, B,Q,Q, QLE  )
APFX(leSS, B,SB,SB, SBLE)

                          AIFX(geBI, B,B,I, >=   )  ACMP(geBD, B,B,D, TGE  )
AIFX(geIB, B,I,B, >=   )  AIFX(geII, B,I,I, >=   )  ACMP(geID, B,I,D, TGE  )
ACMP(geDB, B,D,B, TGE  )  ACMP(geDI, B,D,I, TGE  )  ACMP(geDD, B,D,D, TGE  )
APFX(geXX, B,X,X, -1!=xcompare)
APFX(geQQ, B,Q,Q, QGE  )
APFX(geSS, B,SB,SB, SBGE)

                           AIFX(gtBI, B,B,I, >   )  ACMP(gtBD, B,B,D, TGT  )
AIFX(gtIB, B,I,B, >    )   AIFX(gtII, B,I,I, >   )  ACMP(gtID, B,I,D, TGT  )
ACMP(gtDB, B,D,B, TGT  )   ACMP(gtDI, B,D,I, TGT )  ACMP(gtDD, B,D,D, TGT  )
APFX(gtXX, B,X,X,  1==xcompare)
APFX(gtQQ, B,Q,Q, QGT  )
APFX(gtSS, B,SB,SB, SBGT)
