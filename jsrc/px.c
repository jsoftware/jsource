/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
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
/* B     tostdout  1 iff output to standard output                         */

#include "j.h"
#include "p.h"


A jteval(J jt,C*s){R parse(tokens(cstr(s),1+(AN(jt->locsyms)>1)));}

A jtev1(J jt,    A w,C*s){R df1(  w,eval(s));}
A jtev2(J jt,A a,A w,C*s){R df2(a,w,eval(s));}
A jteva(J jt,    A w,C*s){R df1(  w,colon(num[1],   cstr(s)));}
A jtevc(J jt,A a,A w,C*s){R df2(a,w,colon(num[2],cstr(s)));}

// ". y
F1(jtexec1){A z;
 RZ(w);
 if(AT(w)&NAME){z=nameref(w);  // the case ".@'name' which is the fastest way to refer to a deferred name
 }else{
  F1RANK(1,jtexec1,0);
  STACKCHKOFL FDEPINC(1); z=parse(tokens(vs(w),1+(AN(jt->locsyms)>1))); jt->asgn=0; FDEPDEC(1);
 }
 RETF(z&&!(AT(z)&NOUN)?mtv:z);  // if non-noun result, return empty $0
}

F1(jtimmex){A z;
 if(!w)R A0;  // if no string, return empty result
 AKGST(jt->locsyms)=jt->global; // in case the sentence has operators, set a locale for it
 STACKCHKOFL FDEPINC(1); z=parse(tokens(w,1+(AN(jt->locsyms)>1))); FDEPDEC(1);
 if(z&&!jt->asgn)jpr(z);
 RETF(z);
}

F1(jtimmea){A t,z;
 z=immex(w); 
 ASSERT(jt->asgn||!z||!(AT(z)&NOUN)||(t=eq(num[1],z),
     all1(AT(z)&SPARSE?df1(t,atop(slash(ds(CSTARDOT)),ds(CCOMMA))):t)),EVASSERT);
 RETF(z);
}

static A jtcex(J jt,A w,AF f,A self){A z; RE(w); z=f(jt,w,self); RESETERR; R z;}
     /* conditional execute */

F1(jtexg){A*v,*wv,x,y,z;I n;
 RZ(w);
 n=AN(w); wv=AAV(w); 
 ASSERT(n,EVLENGTH);
 ASSERT(1>=AR(w),EVRANK);
 if(VERB&AT(w))R w;
 ASSERT(BOX&AT(w),EVDOMAIN);
 GATV0(z,BOX,n,1); v=AAV(z);
 DO(n, x=wv[i]; RZ(*v++=(y=cex(x,jtfx,0L))?y:exg(x)););
 R parse(z);
}

L* jtjset(J jt,C*name,A x){R symbisdel(nfs((I)strlen(name),name),x,jt->global);}

F2(jtapplystr){PROLOG(0054);A fs,z;
 F2RANK(1,RMAX,jtapplystr,0);
 RZ(fs=parse(tokens(vs(a),1+(AN(jt->locsyms)>1))));
 ASSERT(VERB&AT(fs),EVSYNTAX);
 STACKCHKOFL FDEPINC(d=fdep(fs)); z=CALL1(FAV(fs)->valencefns[0],w,fs); FDEPDEC(d);
 EPILOG(z); 
}    /* execute string a on argument w */
