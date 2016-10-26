/* Copyright 1990-2004, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions                                                            */

#include "j.h"


// obv1 and obv2 merely pass the call to f.  Since we took the inplace flags for the compound from the original a, we can pass them on too
static CS1IP(obv1, z=(f1)(jtinplace,w,fs),0103)
static CS2IP(obv2, z=(f2)(jtinplace,a,w,fs),0104)

// Set ASGSAFE from a&w; set INPLACE from a
F2(jtobverse){ASSERTVV(a,w); R CDERIV(COBVERSE,obv1,obv2,((VAV(a)->flag&VAV(w)->flag&VASGSAFE)+(VAV(a)->flag&(VINPLACEOK1|VINPLACEOK2))),mr(a),lr(a),rr(a));}


static DF1(ad1){DECLFG;A z;I od=jt->db; 
 RZ(w); 
 jt->db=0; z=CALL1(f1,  w,fs); jt->db=od;
 if(EWTHROW==jt->jerr)R 0;
 RESETERR;  
 R z?z:CALL1(g1,  w,gs);
}

static DF2(ad2){DECLFG;A z;I od=jt->db;
 RZ(a&&w); 
 jt->db=0; z=CALL2(f2,a,w,fs); jt->db=od;
 if(EWTHROW==jt->jerr)R 0;
 RESETERR; 
 R z?z:CALL2(g2,a,w,gs);
}

// Set ASGSAFE from operands
F2(jtadverse){ASSERTVV(a,w); R CDERIV(CADVERSE,ad1,ad2,(VAV(a)->flag&VAV(w)->flag&VASGSAFE),RMAX,RMAX,RMAX);}


static CS1(even1, halve(df1(w,folk(fs,ds(CPLUS ),atop(fs,gs)))),0115)
static CS1(odd1,  halve(df1(w,folk(fs,ds(CMINUS),atop(fs,gs)))),0116)

F2(jteven){ASSERTVV(a,w); R CDERIV(CEVEN,even1,0L, VFLAGNONE, RMAX,0L,0L);}
F2(jtodd ){ASSERTVV(a,w); R CDERIV(CODD, odd1, 0L, VFLAGNONE, RMAX,0L,0L);}
