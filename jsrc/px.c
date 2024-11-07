/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Execute and Associates                                                  */

/* Variables for controlling execution                                     */
/*                                                                         */
/* B     asgn      1 iff last operation on this line is assignment         */
/* DC    dcs       ptr to debug stack entry for current script             */
/* I     etxn      strlen(etx)                                             */
/* C     etx[]     space for holding error display                         */
/* I     jbrk      1 iff user has entered "break"                          */
/* C     jerr      error number for this line                              */
/* A     slist     script files used in right argument to 0!:              */
/* I     slisti    index in slist for current script                       */
/* I     slistn    slist # real entries; AN(slist) is reserved entries     */

#include "j.h"
#include "p.h"


A jteval(J jt,C*s){R PARSERVALUE(parseforexec(tokens(cstr(s),1+!!EXPLICITRUNNING)));}

// obsolete A jtev1(J jt,    A w,C*s){A z; R df1(z,  w,eval(s));}  // parse *s and apply to w  //
// obsolete A jtev2(J jt,A a,A w,C*s){A z; R df2(z,a,w,eval(s));}  // parse *s and apply to a and w.  If w is 0, use monad
// parse string s, producing an ACV, and apply the ACV as either a monad or a dyad.  w=0 if monad.  Result can be anything; use only when you control a, w, and s
A jtev12(J jt,A a,A w,C*s){A z; A fs; RZ(fs=eval(s)); R (FAV(fs)->valencefns[!!w])((J)((I)jt+(AT(fs)&VERB?0:JTXDEFMODIFIER)),a,w?w:fs,fs);}  // if modifier, so flag (for unquote & xdefn, so not needed)
// obsolete A jteva(J jt,    A w,C*s){A z; R df1(z,  w,colon(num(1),   cstr(s)));}  // parse to adverb
// obsolete A jtevc(J jt,A a,A w,C*s){A z; R df2(z,a,w,colon(num(2),cstr(s)));}  // parse to conjunction

// ". y
DF1(jtexec1){A z;
 ARGCHK1(w);
 if(AT(w)&NAME){z=nameref(w,jt->locsyms);  // the case ".@'name' which is the fastest way to refer to a deferred name
 }else{
  F1RANK(1,jtexec1,self);
  STACKCHKOFL z=PARSERVALUE(parseforexec(ddtokens(vs(w),4+1+!!EXPLICITRUNNING)));  // replace DDs, but require that they be complete within the string (no jgets)
 }
 RETF(z&&!(AT(z)&NOUN)?mtv:z);  // if non-noun result, return empty $0
}

// execute w, which is either a string or the boxed words of a string (as if from tokens())
// JT flags controlling print are passed through to jpr
// Result has assignment flag
// We check for system locks before executing the sentence
F1(jtimmex){F1PREFJT;A z;
 if(!w)R A0;  // if no string, return error
 if(unlikely(JT(jt,adbreak)[1])!=0)jtsystemlockaccept(jt,LOCKALL);  // if a systemlock has been requested, accept it.
 // When we start a sentence, we need to establish AKGST in locsyms as a shadow of jt->global, because that's
 // the normal condition and u./v. will rely on it.  During execution of a sentence, jt->global may differ from AKGST
 // (if the sentence calls a modifier through a locative, in which case AKGST holds the pre-locative locale), so recursive calls may not
 // change AKGST.  Of course, any event code must restore everything to its previous state, including locales
 // immex sentences are always directed to the master thread
 if(!(jt->recurstate&RECSTATERENT))AKGST(jt->locsyms)=jt->global; // in case the sentence has operators, set a locale for it
 STACKCHKOFL z=parse(AT(w)&BOX?w:tokens(w,1+!!EXPLICITRUNNING));
 if(((I)z&REPSGN(SGNIFNOT(z,PARSERASGNX)))&&!(AFLAG(z)&AFDEBUGRESULT))jtjpr((J)((I)jtinplace|JTPRFORSCREEN),z);   // print if z not 0 && LSB of z is 0 && Result is not for debug
 RETF(z);
}

// execute for assert during script: check result for not assigned or no error or non-noun or all 1
// jt has typeout flags, pass through to immex
// Result has assignment flag
F1(jtimmea){F1PREFJT;A t,z,z1;
 RZ(w=ddtokens(w,1+!!EXPLICITRUNNING)); z=jtimmex(jtinplace,w);   // check for DD, allow continuation
 ASSERT(PARSERASGN(z)||!z||!(AT(z)&NOUN)||(t=eq(num(1),z),
     all1(ISSPARSE(AT(z))?df1(z1,t,atop(slash(ds(CSTARDOT)),ds(CCOMMA))):t)),EVASSERT);  // apply *./@, if sparse
 RETF(z);
}

static A jtcex(J jt,A w,AF f,A self){A z; RE(w); z=f(jt,w,self); RESETERR; R z;}
     /* conditional execute - return 0 if error */

// convert the gerund (i.e  AR) in w into a verb
F1(jtexg){A*v,*wv,x,y,z;I n;
 ARGCHK1(w);
 n=AN(w); wv=AAV(w); 
 ASSERT(n!=0,EVLENGTH);
 ASSERT(1>=AR(w),EVRANK);
 if(VERB&AT(w))R w;
 ASSERT(BOX&AT(w),EVDOMAIN);
 GATV0(z,BOX,n,1); v=AAV1(z);
 DO(n, x=C(wv[i]); RZ(y=(y=cex(x,jtfx,0L))?y:exg(x)); *v++=QCINSTALLTYPE(y,ATYPETOVALTYPE(AT(y)));)  // if the AR can be converted to an A, do so; otherwise it should be a list of ARs, recur on each
 R PARSERVALUE(parseforexec(z));
}

// immediate assignment of value x to name
I jtjset(J jt,C*name,A x){R symbisdel(nfs((I)strlen(name),name),x,jt->global);}

// 128!:2
DF2(jtapplystr){PROLOG(0054);A fs,z;
 F2RANK(1,RMAX,jtapplystr,self);
 RZ(fs=PARSERVALUE(parseforexec(tokens(vs(a),1+!!EXPLICITRUNNING))));
 ASSERT(VERB&AT(fs),EVSYNTAX);
 STACKCHKOFL z=CALL1(FAV(fs)->valencefns[0],w,fs);
 EPILOG(z); 
}    /* execute string a on argument w */
