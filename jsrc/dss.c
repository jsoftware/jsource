/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Debug: Single Step                                                      */

#include "j.h"
#include "d.h"


/* d->dcss   - single step code in current function                        */

// d points to current stack frame, c is the single-step type
// install c into dcss of the caller to this function (if any); return address of caller
DC jtssnext(J jt,DC d,C c){
 d=d->dclnk;
 NOUNROLL while(d&&DCCALL!=d->dctype)d=d->dclnk;      /* find next call                 */
 if(d&&!d->dcsusp){d->dcss=c; } 
 else             {d=0;      }
 R d;
}    /* set dcss for next stack level */

// Process one of the step verbs.  We adjust the next-line addresses in the functions,
// and the single-step codes, as requested by the user.  We may modify this level or the previous level
// (for step into, we will set up the next level when it is created).  We return a flagged value to cause
// the suspension to terminate
static A jtssdo(J jt,A a,A w,C c){DC d,e;I n;
 RZ(w=vs(w));  // w is ignored; must be list or atom
 ASSERT(jt->uflags.trace&TRACEDB,EVDOMAIN);
 d=jt->sitop;                               /* cut back to topmost suspension  */
 NOUNROLL while(d&&!d->dcsusp){                      /* do until topmost suspension     */
  if(d->dctype==DCCALL)DGOTO(d,-1)    /* terminate each call             */
  d=d->dclnk;
 }
 ASSERT(d!=0,EVDOMAIN);                        /* must have a suspension          */
 NOUNROLL while(d&&DCCALL!=d->dctype)d=d->dclnk;     /* find topmost call               */
 ASSERT(d!=0,EVDOMAIN);                        /* must have a call                */
 if(a)RE(n=lnumcw(i0(a),d->dcc));           // for dyad, source line # to cw line #
 switch(c){
  case SSSTEPOVER: DGOTO(d,a?n:d->dcix) d->dcss=c;    break;  // rerun stop line (executing it), then stop in this function
  case SSSTEPINTO: DGOTO(d,a?n:d->dcix) d->dcss=c;    break;  // rerun stop line(executing it), then stop in any function
  case SSSTEPOUT:  DGOTO(d,a?n:d->dcix) d->dcss=0;   ssnext(d,SSSTEPOVERs); break;  // rerun stop line, stop in calling function
  case SSCUTBACK:  DGOTO(d,-1) d->dcss=0; e=ssnext(d,SSSTEPOVERs); if(e)DGOTO(e,e->dcix) jt->jerr=EVCUTSTACK; break;  // terminate current verb, resume previous fn, stop after executing there.  Set jerr which will fail sentence and set z=0
 }
 // Return a suspension-ending value.  Kludge we also set a flag to process the step because the labs can't route the value correctly
 JT(jt,dbuser)|=TRACEDBSUSSS;  // indicate single-step pending.  This is non-reentrant and may have multithread issues
 A z; RZ(z=mkwris(box(sc(SUSSS)))); AFLAGORLOCAL(z,AFDEBUGRESULT) R z;
}

F1(jtdbcutback  ){R ssdo(0L,w,SSCUTBACK);}  /* 13!:19 */
// should allow w to have the value to return to the function

F1(jtdbstepover1){R ssdo(0L,w,SSSTEPOVER);}  /* 13!:20 */
F2(jtdbstepover2){R ssdo(a, w,SSSTEPOVER);}

F1(jtdbstepinto1){R ssdo(0L,w,SSSTEPINTO);}  /* 13!:21 */
F2(jtdbstepinto2){R ssdo(a, w,SSSTEPINTO);}

F1(jtdbstepout1 ){R ssdo(0L,w,SSSTEPOUT );}  /* 13!:22 */
F2(jtdbstepout2 ){R ssdo(a, w,SSSTEPOUT );}
