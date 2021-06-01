/* Copyright 1990-2003, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Debug: Stops                                                            */

#include "j.h"
#include "d.h"


/* check for stop before each function line; return 1 if stop requested */
/* 0 or more repetitions of the following patterns, separated by        */
/* abc*yz m:d   all names beginning with abc and ending with yz;        */
/*              monad line numbers and dyad line numbers (* means all). */
/* ~abc*yz m:d  don't stop                                              */

static B stopsub(C*p,C*nw,I md){C*q,*s;I n;
 s=strchr(p,';'); if(!s)s=p+strlen(p);
 q=strchr(p,':'); if(!q||q>s)q=s;
 if(2==md){p=q; q=s; if(':'==*p)++p;}
 s=strchr(p,'*'); if(s&&q>s)R 1;
 n=strlen(nw);
 while(q>p){
  NOUNROLL while(' '==*p)++p;
  if(!strncmp(p,nw,n)&&(q==p+n||' '==p[n]))R 1;
  NOUNROLL while(q>p&&' '!=*p)++p;
 }
 R 0;
}

// i is the line we are about to execute, c is the call-stack entry for the current function
// return 1 if we should stop before executing the line
B jtdbstop(J jt,DC d,I i){A a;B b,c=0,e;C nw[11],*s,*t,*u,*v;I md,n,p,q;
 if(!d)R 0;  // if there is no debug stack, there is no stop
 if(!strcmp(NAV(d->dca)->s,"output_jfe_"))R 0; // JHS - ignore stepinto output_jfe
 // Handle stop owing to single-step
 switch(d->dcss){
 case SSSTEPINTO:  d->dcss=SSSTEPINTOs; break;  // first time is executing the stop line.  Then wait for any next line
 case SSSTEPOVER:  d->dcss=SSSTEPOVERs; break;  // first time is executing the stop line.  Then wait for line in this function
 case SSSTEPOVERs:
 case SSSTEPINTOs: c=i!=d->dcstop; d->dcstop=i; NOUNROLL while(d){if(d->dctype==DCCALL)d->dcss=0; d=d->dclnk;} R c;  // about to execute a second line.  stop, unless already stopped there; clear all stops once we take a stop
// others not stored
 }
 // if no single-step stop, try looking the line up in the stops table
 if(i==d->dcstop){d->dcstop=-2; R 0;}     /* not stopping if already stopped at the same place */
 if(!(JT(jt,dbstops)))R 0; s=CAV(str0(JT(jt,dbstops))); sprintf(nw,FMTI,i);
 a=d->dca; n=d->dcm; t=NAV(a)->s; md=d->dcx&&d->dcy?2:1; 
 NOUNROLL while(s){
  NOUNROLL while(' '==*s)++s; if(b='~'==*s)++s; while(' '==*s)++s;
  u=strchr(s,'*'); v=strchr(s,' '); if(!v)break;; 
  if(!u||u>v)e=!strncmp(s,t,MAX(n,v-s));
  else{p=u-s; q=v-u-1; e=p<=n&&!strncmp(s,t,p)&&q<=n&&!strncmp(1+u,t+n-q,q);}
  if(e){s=1+v; if(stopsub(s,nw,md)){if(b){c=0; break;} c=1;}}
  s=strchr(s,';'); if(s)++s;
 }
 if(c){d->dcstop=i;  NOUNROLL while(d){if(d->dctype==DCCALL)d->dcss=0; d=d->dclnk;}}  // if stop found, turn off single-step everywhere
 else  d->dcstop=-2;
 R c;
}    /* stop on line i? */


F1(jtdbstopq){ASSERTMTV(w); R JT(jt,dbstops)?JT(jt,dbstops):mtv;}
     /* 13!:2  query stops */

F1(jtdbstops){RZ(w=vs(w)); if(AN(w)){RZ(ras(w)); fa(JT(jt,dbstops)); JT(jt,dbstops)=w;}else JT(jt,dbstops)=0; R mtm;}
     /* 13!:3  set stops */
