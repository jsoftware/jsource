/* Copyright 1990-2004, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions                                                            */

#include "j.h"


// obv1 and obv2 merely pass the call to f.  Since we took the inplace flags for the compound from the original a, we can pass them on too
CS1IP(static,obv1, z=(f1)(jtinplace,w,fs),0103)
CS2IP(static,static,obv2, z=(f2)(jtinplace,a,w,fs),0104)

// Set ASGSAFE from a&w; set INPLACE from a
F2(jtobverse){ASSERTVV(a,w); R CDERIV(COBVERSE,obv1,obv2,((FAV(a)->flag&FAV(w)->flag&VASGSAFE)+(FAV(a)->flag&(VJTFLGOK1|VJTFLGOK2))),mr(a),lr(a),rr(a));}


// Adverse.  Run f, and if that fails (and not with THROW), run g
static DF1(ad1){DECLFG;A z;
 ARGCHK1(w); 
 WITHDEBUGOFF(z=CALL1(f1,  w,fs);)
 if(EVTHROW==jt->jerr)R 0;
 RESETERR;
 R z?z:AT(gs)&NOUN?gs:CALL1(g1,  w,gs);
}

static DF2(ad2){DECLFG;A z;
 ARGCHK2(a,w); 
 WITHDEBUGOFF(z=CALL2(f2,a,w,fs);)
 if(EVTHROW==jt->jerr)R 0;
 RESETERR; 
 R z?z:AT(gs)&NOUN?gs:CALL2(g2,a,w,gs);
}

// Set ASGSAFE from operands.  Noun operand is always safe
F2(jtadverse){ASSERTVVn(a,w); R CDERIV(CADVERSE,ad1,ad2,(FAV(a)->flag&(AT(w)&VERB?FAV(w)->flag:~0)&VASGSAFE),RMAX,RMAX,RMAX);}

