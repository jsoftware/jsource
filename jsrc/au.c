/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Utilities                                                      */

#include "j.h"

static I jtfdepger(J jt,A w){R 0;
}


DF1(jtdfs1){F1PREFIP;A s=jt->parserstackframe.sf,z; RZ(self); df1(z,w,jt->parserstackframe.sf=self); jt->parserstackframe.sf=s; RETF(z);}
DF2(jtdfs2){F2PREFIP;
A s=jt->parserstackframe.sf,z; 
RZ(self); 
df2(z,a,w,jt->parserstackframe.sf=self); jt->parserstackframe.sf=s; 
RETF(z);}    
     /* for monads and dyads that can possibly involve $: */


// $: itself
F1(jtself1){A z; FDEPINC(d=fdep(jt->parserstackframe.sf)); STACKCHKOFL df1(z,  w,jt->parserstackframe.sf);  FDEPDEC(d); forcetomemory(w); RETF(z);}
F2(jtself2){A z; FDEPINC(d=fdep(jt->parserstackframe.sf)); STACKCHKOFL df2(z,a,w,jt->parserstackframe.sf);  FDEPDEC(d); forcetomemory(w); RETF(z);}

A jtac1(J jt,AF f){R fdef(0,0,VERB, f,jtvalenceerr, 0L,0L,0L, VFLAGNONE, RMAX,RMAX,RMAX);}
A jtac2(J jt,AF f){R fdef(0,0,VERB, jtvalenceerr,f, 0L,0L,0L, VFLAGNONE, RMAX,RMAX,RMAX);}

F1(jtvalenceerr){F1PREFIP; ASSERT(0,EVVALENCE);}  // used for undefined valences, including [:

// return 1 if w contains no names or explicit definitions
B nameless(A w){A f,g,h;C id;V*v;
 if(!w||NOUN&AT(w))R 1;
 v=FAV(w);
 id=v->id; f=v->fgh[0]; g=v->fgh[1]; h=v->fgh[2];
 R !(id==CCOLON&&f&&NOUN&AT(f)) && !(id==CTILDE&&f&&NAME&AT(f)) && nameless(f) && nameless(g) && (id==CFORK&&h?nameless(h):1);  // h only if uncapped fork
}

B jtprimitive(J jt,A w){A x=w;V*v;
 ARGCHK1(w);
 v=VAV(w);
 if(CTILDE==v->id&&NOUN&AT(v->fgh[0]))RZ(x=fix(w,zeroionei(0)));
 R!VAV(x)->fgh[0];
}    /* 1 iff w is a primitive */

// w is a conj, f C n
// Return 1 if f is of the form <@:g  (or <@g when g has infinite rank)
B jtboxatop(J jt,A w){ARGCHK1(w); R 1&boxat(FAV(w)->fgh[0],RMAX,RMAX,RMAX);}

// x is a verb
// Return if verb is of the form <@:g  (or <@g when g has rank >= input rank)
// bit 0 for monad case, bit 1 for dyad
I boxat(A x, I m, I l, I r){C c;V*v;
 if(!x)R 0;
 v=FAV(x); c=v->id;   // x->fgh[0], v->value, c=id of f
 if(!COMPOSE(c))R 0;  // Return if not @ @: & &:
 if(CBOX==ID(v->fgh[0])) {  // if u is <...
   if(c==CATCO) R 3; if(c==CAMPCO) R 1;  // @: is always good, and &: for monad
   I res = 0;
   if(v->fgh[1]&&VERB&AT(v->fgh[1])){
    res |= FAV(v->fgh[1])->mr>=m;    // monad ok if rank big enough  for either @ or &
    res |= (c==CAT)&&lr(v->fgh[1])>=(UI)l&&rr(v->fgh[1])>=(UI)r?2:0;  // dyad for @ only, if rank big enough
   }
   R res;
 }
 R 0; 
}    /* 1 iff w is <@:f or <@f where f has infinite rank */
