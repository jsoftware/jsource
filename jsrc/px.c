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


A jteval(J jt,C*s){R parse(tokens(cstr(s),1+!!jt->local));}

A jtev1(J jt,    A w,C*s){R df1(  w,eval(s));}
A jtev2(J jt,A a,A w,C*s){R df2(a,w,eval(s));}
A jteva(J jt,    A w,C*s){R df1(  w,colon(one,   cstr(s)));}
A jtevc(J jt,A a,A w,C*s){R df2(a,w,colon(num[2],cstr(s)));}

F1(jtexec1){A z;
 F1RANK(1,jtexec1,0);
 FDEPINC(1); z=parse(tokens(vs(w),1+!!jt->local)); jt->asgn=0; FDEPDEC(1);
 R z&&AT(z)&VERB+ADV+CONJ+MARK?mtv:z;
}

F1(jtimmex){A z;
 FDEPINC(1); z=parse(tokens(w,1+!!jt->local)); FDEPDEC(1); 
 if(EWTHROW==jt->jerr){RESETERR; z=mtm;}
 if(z&&!jt->asgn)jpr(z);
 R z;
}

F1(jtimmea){A t,z;
 z=immex(w); 
 ASSERT(jt->asgn||!z||!(AT(z)&NOUN)||(t=eq(one,z),
     all1(AT(z)&SPARSE?df1(t,atop(slash(ds(CSTARDOT)),ds(CCOMMA))):t)),EVASSERT);
 R z;
}

static A jtcex(J jt,A w,AF f){A z; RE(w); z=f(jt,w); RESETERR; R z;}
     /* conditional execute */

F1(jtexg){A*v,*wv,x,y,z;I n,wd;
 RZ(w);
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w);
 ASSERT(n,EVLENGTH);
 ASSERT(1>=AR(w),EVRANK);
 if(VERB&AT(w))R w;
 ASSERT(BOX&AT(w),EVDOMAIN);
 GATV(z,BOX,n,1,0); v=AAV(z);
 DO(n, x=WVR(i); RZ(*v++=(y=cex(x,jtfx))?y:exg(x)););
 R parse(z);
}

A jtjset(J jt,C*name,A x){R symbis(nfs((I)strlen(name),name),x,jt->global);}

F2(jtapplystr){PROLOG(0054);A fs,z;I d;
 F2RANK(1,RMAX,jtapplystr,0);
 RZ(fs=parse(tokens(vs(a),1+!!jt->local)));
 ASSERT(VERB&AT(fs),EVSYNTAX);
 RE(d=fdep(fs));
 FDEPINC(d); z=CALL1(VAV(fs)->f1,w,fs); FDEPDEC(d);
 EPILOG(z); 
}    /* execute string a on argument w */
