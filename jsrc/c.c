/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions                                                            */

#include "j.h"

// obv1 and obv2 merely pass the call to f.  Since we took the inplace flags for the compound from the original a, we can pass them on too.  Pass inplaceability through
static DF2(obv12cell){F12IP;w=EPDYAD?w:0; A fs=FAV(self)->fgh[0]; AF f12=FAV(fs)->valencefns[!!w];A z;PROLOG(0103); z=CALL12IP(w,f12,a,w,fs); EPILOG(z);}
// obsolete static DF1(obv1){F12IP;A a=w; w=self; PREF2IP(obv12cell); R obv12cell(jtfg,a,w,self);}
static DF2(obv12){F12IP;self=AT(w)&VERB?w:self; PREF2IP(obv12cell); R obv12cell(jtfg,a,w,self);}  // Pass inplaceability through


// Set ASGSAFE from a&w; set INPLACE from a
F2(jtobverse){F12IP;ASSERTVV(a,w); R fdef(0L,COBVERSE,VERB,obv12,obv12,a,w ,0L,((FAV(a)->flag&FAV(w)->flag&VASGSAFE)),mr(a),lr(a),rr(a));}

// Adverse.  Run f, and if that fails (and not with THROW/EXIT), run g (or use its value if it's a noun).  Bivalent  a,w,self or w,self,self
static DF2(ad12){F12IP;A z; A childself=FAV(self)->fgh[0]; 
 ARGCHK2(a,w); A *old=jt->tnextpushp;
 I dyad=EPDYAD; w=EPDYAD?w:childself;  // Set w for bivalent call 
 WITHDEBUGOFF(z=CALL2(FAV(childself)->valencefns[dyad],a,w,childself);)
 if(unlikely(jt->jerr==EVTHROW))R 0;  // THROW is caught only by try.
 if(unlikely(jt->jerr==EVEXIT))R 0;  // EXIT is never caught
 if(BETWEENC(jt->jerr,EVATTN,EVBREAK))CLRATTN  // if the error was ATTN/BREAK, clear the source of the error
 RESETERR;
 if(likely(z))RETF(z);  // normal return.  fall through to execute v
 tpop(old);  // the error exit leaves the stack unpopped
 childself=FAV(self)->fgh[1];  w=dyad?w:childself; R AT(childself)&NOUN?childself:CALL2(FAV(childself)->valencefns[dyad],a,dyad?w:childself,childself);
}

// Set ASGSAFE from operands.  Noun operand is always safe
F2(jtadverse){F12IP;ASSERTVVn(a,w); R fdef(0L,CADVERSE,VERB,ad12,ad12,a,w ,0L,(FAV(a)->flag&(AT(w)&VERB?FAV(w)->flag:~0)&VASGSAFE),RMAX,RMAX,RMAX);}

