/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
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

// p points to stop numbers, nw is character form of line number (must be canonical), md=1 for monad, 2 for dyad
// result is 1 if there is a stop on this line 
static B stopsub(C*p,C*nw,I md){C*q,*s;I n;
 s=strchr(p,';'); if(!s)s=p+strlen(p);  // s-> end+1-of-line#s (first ; or ending NUL)
 q=strchr(p,':'); if(!q||q>s)q=s;  // q-> end+1-of-monad-line#s (first : or s)
 if(2==md){p=q; q=s; if(':'==*p)++p;}  // adjust for dyad.  Now p=start of line#s, q=end+1 of line#s
 s=strchr(p,'*'); if(s&&q>s)R 1;   // if '*' in the line#s, return true
 n=strlen(nw);    // n=length of line# we are looking for (in canonical form)
 while(q>p){    // until all #s checked...
  NOUNROLL while(' '==*p)++p;  // skip leading blanks
  if(!strncmp(p,nw,n)&&(q==p+n||' '==p[n]))R 1;   // match strings for the length of the name & verify stopnum is at the end; if so, success
  NOUNROLL while(q>p&&' '!=*p)++p;   // skip unmatched part of stopnum
 }
 R 0;   // no stopnum matched, return failure
}

// i is the line we are about to execute, c is the call-stack entry for the current function
// return 1 if we should stop before executing the line
B jtdbstop(J jt,DC d,I i){A a;B b,c=0,e;C nw[11],*s,*t,*u,*v;I md,n,p,q;
 if(!d)R 0;  // if there is no debug stack, there is no stop
 if(d->dca&&!strcmp(NAV(d->dca)->s,"output_jfe_"))R 0; // JHS - ignore stepinto output_jfe
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
 READLOCK(JT(jt,dblock));  // lock the stops table while we inspect it
 if((d->dca&&JT(jt,dbstops))){  // if the name is given and there are stops...
  s=CAV(str0(JT(jt,dbstops))); sprintf(nw,FMTI,i);  // s->stop strings, nw=character form of line#
  a=d->dca; n=NAV(a)->m; t=NAV(a)->s; md=d->dcx&&d->dcy?2:1;   // t->name we are looking for, n=its length, md=valence of call
  NOUNROLL while(s){  // until we have looked at all stops...
   NOUNROLL while(' '==*s)++s; if(b='~'==*s)++s; while(' '==*s)++s;  // skip over spaces and ~.  Set b = '~' found
   u=strchr(s,'*'); v=strchr(s,' '); if(!v)break;   // u=address of first '*' if any; v=first space if any; if no space, it's name with no stops, quit
   if(!u||u>v)e=!strncmp(s,t,MAX(n,v-s));   // if there is no * before the first space, set e to 'names match in full'
   else{p=u-s; q=v-u-1; e=p<=n&&!strncmp(s,t,p)&&q<=n&&!strncmp(1+u,t+n-q,q);}  // p=length of stopname before *, v=length of stopname after *, e to 'names match' if both parts matched
   if(e){s=1+v; if(stopsub(s,nw,md)){if(b){c=0; break;} c=1;}}  // if matched, point to line numbers and see if there is a stop.  If there is, if it's not ~ set c and continue (there might be a later ~); if ~, exit 'no stop'
   s=strchr(s,';'); if(s)++s;  // advance to ; if any (s=0 if no ;) and then beyond
  }
  if(c){d->dcstop=i;  NOUNROLL while(d){if(d->dctype==DCCALL)d->dcss=0; d=d->dclnk;}}  // if stop found, turn off single-step everywhere
  else  d->dcstop=-2;
 }
 READUNLOCK(JT(jt,dblock)); 
 R c;
}    /* stop on line i? */

// 13!:2, query stops
F1(jtdbstopq){
 ASSERTMTV(w); 
 // we must read & protect the sentence under lock in case another thread is changing it
 READLOCK(JT(jt,dblock)) A stops=JT(jt,dbstops); if(stops)ras(stops); READUNLOCK(JT(jt,dblock))  // must ra() while under lock
 if(stops){tpushnr(stops);}else stops=mtv;  // if we did ra(), stack a fa() on the tpop stack
 R stops;
}

// 13!:3, set stops
F1(jtdbstops){
 ARGCHK1(w);
 RZ(w=vs(w));
 if(AN(w)){RZ(ras(w));}else w=0;  // protect w if it is nonempty; if empty, convert to null
 WRITELOCK(JT(jt,dblock)) A stops=JT(jt,dbstops); JT(jt,dbstops)=w; WRITEUNLOCK(JT(jt,dblock))  // swap addresses under lock
 if(stops!=0)fa(stops);  // undo the ra() done when value was stored
 R mtm;
}


