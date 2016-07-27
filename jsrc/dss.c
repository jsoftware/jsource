/* Copyright 1990-2002, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Debug: Single Step                                                      */

#include "j.h"
#include "d.h"


/* d->dcss   - single step code in current function                        */
/* jt->dbssd - the d whose dcss is non-zero                                */
/* jt->dbss  - single step code for propagation to appropriate function    */


DC jtssnext(J jt,DC d,C c){
 d=d->dclnk;
 while(d&&DCCALL!=d->dctype)d=d->dclnk;      /* find next call                 */
 if(d&&!d->dcsusp){d->dcss=c; jt->dbssd=d;} 
 else             {d=0;       jt->dbssd=0;}
 R d;
}    /* set dcss for next stack level */

static A jtssdo(J jt,A a,A w,C c){DC d,e;I n,*v;
 RZ(w=vs(w));
 ASSERT(jt->db,EVDOMAIN);
 d=jt->sitop;                               /* cut back to topmost suspension  */
 while(d&&!d->dcsusp){                      /* do until topmost suspension     */
  if(d->dctype==DCCALL)*(I*)(d->dci)=-2;    /* terminate each call             */
  d=d->dclnk;
 }
 ASSERT(d,EVDOMAIN);                        /* must have a suspension          */
 while(d&&DCCALL!=d->dctype)d=d->dclnk;     /* find topmost call               */
 ASSERT(d,EVDOMAIN);                        /* must have a call                */
 if(a)RE(n=lnumcw(i0(a),d->dcc));           /* source line # to cw line #      */
 v=(I*)d->dci;                              /* pointer to line #               */
 jt->dbsusact=SUSSS;
 switch(c){
  case SSSTEPOVER: if(a)*v=n-1; else --*v; jt->dbss=d->dcss=c; jt->dbssd=d;   break;
  case SSSTEPINTO: if(a)*v=n-1; else --*v; jt->dbss=d->dcss=c; jt->dbssd=d;   break;
  case SSSTEPOUT:  if(a)*v=n-1; else --*v; jt->dbss=d->dcss=0;   ssnext(d,c); break;
  case SSCUTBACK:  *v=-2; jt->dbss=d->dcss=0; e=ssnext(d,c); if(e)--*(I*)e->dci;
 }
 fa(jt->dbssexec); if(AN(w)){ra(w); jt->dbssexec=w;}else jt->dbssexec=0;
 R mtm;                                     /* 0 return to terminate call      */
}

F1(jtdbcutback  ){R ssdo(0L,w,SSCUTBACK );}  /* 13!:19 */

F1(jtdbstepover1){R ssdo(0L,w,SSSTEPOVER);}  /* 13!:20 */
F2(jtdbstepover2){R ssdo(a, w,SSSTEPOVER);}

F1(jtdbstepinto1){R ssdo(0L,w,SSSTEPINTO);}  /* 13!:21 */
F2(jtdbstepinto2){R ssdo(a, w,SSSTEPINTO);}

F1(jtdbstepout1 ){R ssdo(0L,w,SSSTEPOUT );}  /* 13!:22 */
F2(jtdbstepout2 ){R ssdo(a, w,SSSTEPOUT );}
