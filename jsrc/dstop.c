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
  while(' '==*p)++p;
  if(!strncmp(p,nw,n)&&(q==p+n||' '==*(p+n)))R 1;
  while(q>p&&' '!=*p)++p;
 }
 R 0;
}

B jtdbstop(J jt,DC d,I i){A a;B b,c=0,e;C nw[11],*s,*t,*u,*v;I md,n,p,q;
 if(!jt->dbss&&d->dcss){d->dcss=0; jt->dbssd=0; c=i!=d->dcstop; d->dcstop=i; R c;}
 switch(jt->dbss){
  case SSSTEPOVER:  jt->dbss=0;           break;  
  case SSSTEPINTO:  jt->dbss=SSSTEPINTOs; break;  
  case SSSTEPINTOs: jt->dbss=0; if(jt->dbssd){jt->dbssd->dcss=0; jt->dbssd=0;} 
   c=i!=d->dcstop; d->dcstop=i; R c;
 }       
 if(i==d->dcstop){d->dcstop=-2; R 0;}     /* not stopping if already stopped at the same place */
 RZ(jt->dbstops); s=CAV(jt->dbstops); sprintf(nw,FMTI,i);
 a=d->dca; n=d->dcm; t=NAV(a)->s; md=d->dcx&&d->dcy?2:1; 
 while(s){
  while(' '==*s)++s; if(b='~'==*s)++s; while(' '==*s)++s;
  u=strchr(s,'*'); v=strchr(s,' '); if(!v)break;; 
  if(!u||u>v)e=!strncmp(s,t,MAX(n,v-s));
  else{p=u-s; q=v-u-1; e=p<=n&&!strncmp(s,t,p)&&q<=n&&!strncmp(1+u,t+n-q,q);}
  if(e){s=1+v; if(stopsub(s,nw,md)){if(b){c=0; break;} c=1;}}
  s=strchr(s,';'); if(s)++s;
 }
 if(c){d->dcstop=i; d->dcss=jt->dbss=0; if(jt->dbssd){jt->dbssd->dcss=0; jt->dbssd=0;}}
 else  d->dcstop=-2;
 R c;
}    /* stop on line i? */


F1(jtdbstopq){ASSERTMTV(w); R jt->dbstops?jt->dbstops:mtv;}
     /* 13!:2  query stops */

F1(jtdbstops){RZ(w=vs(w)); fa(jt->dbstops); if(AN(w)){ra(w); jt->dbstops=w;}else jt->dbstops=0; R mtm;}
     /* 13!:3  set stops */
