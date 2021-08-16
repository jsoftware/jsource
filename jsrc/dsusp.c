/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Debug: Suspension                                                       */

#include "j.h"
#include "d.h"
#include "w.h"

// When we move off of a parser frame, or when we go into debug with a new parser frame, fill the frame with
// the info for the parse that was interrupted
static void movesentencetosi(J jt,void *wds,I nwds,I errwd){if(jt->sitop&&jt->sitop->dctype==DCPARSE){jt->sitop->dcy=(A)wds; jt->sitop->dcn=(I)nwds; jt->sitop->dcix=(I)errwd; }}
static void movecurrtoktosi(J jt){if(jt->sitop&&jt->sitop->dctype==DCPARSE){jt->sitop->dcix=jt->parserstackframe.parserstkbgn[-1].t; }}
void moveparseinfotosi(J jt){movesentencetosi(jt,jt->parserstackframe.parserstkbgn[-1].a,jt->parserstackframe.parserstkbgn[-1].t,jt->parserstackframe.parsercurrtok); }


/* deba() and debz() must be coded and executed in pairs */
/* in particular, do NOT do error exits between them     */
/* e.g. the following is a NO NO:                        */
/*    d=deba(...);                                       */
/*    ASSERT(blah,EVDOMAIN);                             */
/*    debz()                                             */

DC jtdeba(J jt,C t,void *x,void *y,A fs){DC d;
 {A q; GAT0(q,LIT,sizeof(DST),1); d=(DC)AV(q);}
 mvc(sizeof(DST),d,1,MEMSET00);
 if(jt->sitop&&t!=DCJUNK)moveparseinfotosi(jt);  // if we are creating a space between normal and suspension, don't modify the normal stack
 d->dctype=t; d->dclnk=jt->sitop; jt->sitop=d;
 switch(t){
  case DCPARSE:  d->dcy=(A)x; d->dcn=(I)y; break;
  case DCSCRIPT: d->dcy=y; d->dcm=(I)fs; break;
  case DCCALL:   
   d->dcx=x; d->dcy=y; d->dcf=fs; 
   d->dca=jt->curname; d->dcm=NAV(d->dca)->m;
   d->dcstop=-2;
   // dcn fill in in caller
   // if we were waiting to step into a function, this is it: mark this function as stoppable
   // and remove the stop in the caller
   DC e=d->dclnk; NOUNROLL while(e&&DCCALL!=e->dctype)e=e->dclnk;  // find previous call
   if(e&&e->dcss==SSSTEPINTOs){d->dcss=SSSTEPINTOs; e->dcss=0;}
 }
 R d;
}    /* create new top of si stack */

void jtdebz(J jt){jt->sitop=jt->sitop->dclnk;}
     /* remove     top of si stack */

F1(jtsiinfo){A z,*zv;DC d;I c=5,n,*s;
 ASSERTMTV(w);
 n=0; d=jt->sitop; NOUNROLL while(d){++n; d=d->dclnk;}
 GATV0(z,BOX,c*n,2); s=AS(z); s[0]=n; s[1]=c; zv=AAV(z);
 d=jt->sitop;
 while(d){
  RZ(zv[0]=sc(d->dctype));
  RZ(zv[1]=d->dcsusp?scc('*'):scc(' '));
  RZ(zv[2]=sc((I)d->dcss));
  RZ(zv[3]=d->dctype==DCCALL?sc(lnumsi(d)):mtv);
  switch(d->dctype){
   case DCPARSE:  RZ(zv[4]=unparse(d->dcy)); break;
   case DCCALL:   RZ(zv[4]=sfn(0,d->dca));   break;
   case DCSCRIPT: zv[4]=d->dcy;              break;
   case DCJUNK:   zv[4]=mtv;                 break; 
  }
  zv+=c; d=d->dclnk;
 }
 R z;
}    /* 13!:32 si info */

I lnumcw(I j,A w){CW*u;
 if(0>j)R -2; 
 else if(!w)R j; 
 else{u=(CW*)AV(w); DO(AN(w), if(j<=u[i].source)R i;) R IMAX/2;}
}    /* line number in CW corresp. to j */

I lnumsi(DC d){A c;I i;
 if(c=d->dcc){i=d->dcix; R(MIN(i,AN(c)-1)+(CW*)AV(c))->source;}else R 0;
}    /* source line number from DCCALL-type stack entry */



static DC suspset(DC d){DC e=0;
 NOUNROLL while(d&&DCCALL!=d->dctype){e=d; d=d->dclnk;}  /* find bottommost call                 */
 if(!(d&&DCCALL==d->dctype))R 0;                /* don't suspend if no such call     */
 if(d->dcc){RZQ(e); e->dcsusp=1;}               // if explicit, set susp on line - there should always be a following frame, but if not do nothing
 else      d->dcsusp=1;                         /* if not explicit, set susp on call */
 R d;
}    /* find topmost call and set suspension flag */

static B jterrcap(J jt){A y,*yv;
 GAT0(y,BOX,4,1); yv=AAV(y);
 RZ(yv[0]=sc(jt->jerr1));
 RZ(yv[1]=str(jt->etxn1,jt->etx));
 RZ(yv[2]=dbcall(mtv));
 RZ(yv[3]=locname(mtv));
 RZ(symbis(nfs(22L,"STACK_ERROR_INFO_base_"),y,mark));
 R 1;
}    /* error capture */

// suspension.  Loop on keyboard input.  Keep executing sentences until something changes dbsusact.
static A jtsusp(J jt){A z;
 // normally we run with an empty stack frame which is always ready to hold the display of the next sentence
 // to execute; the values are filled in when there is an error.  We are about to call immex to run sentences,
 // and it will create a stack frame for its result.  CREATION of this stack frame will overwrite the current top-of-stack
 // if it holds error information.  So, we create an empty frame to take the store from immex.  This frame has no display.
 RZ(deba(DCJUNK,0,0,0)); // create spacer frame
 A *old=jt->tnextpushp;  // fence must be after we have allocated our stack block
 // If the failure happened while a script was being loaded, we have to take jgets() out of script mode so we can prompt the user.  We will restore on exit
 DC d; for(d=jt->sitop; d&&d->dctype!=DCSCRIPT; d=d->dclnk);  // d-> last SCRIPT type, if any
 if(d&&!(JT(jt,dbuser)&0x80))d->dcss=0;  // in super-debug mode (dbr 16b81), we continue reading suspension lines from the script; otherwise turn it off
 // Make sure we have a decent amount of stack space left to run sentences in suspension
#if USECSTACK
 jt->cstackmin=MAX(jt->cstackinit-(CSTACKSIZE-CSTACKRESERVE),jt->cstackmin-CSTACKSIZE/10);
#else
 jt->fdepn =MIN(NFDEP ,jt->fdepn +NFDEP /10);
#endif
 jt->fcalln=MIN(NFCALL,jt->fcalln+NFCALL/10);
 // if there is a 13!:15 sentence (latent expression) to execute before going into debug, do it
 if(JT(jt,dbtrap)){RESETERR; immex(JT(jt,dbtrap)); tpop(old);}  // force typeout
 // Loop executing the user's sentences until one returns a value that is flagged as 'end of suspension'
 while(1){A  inp;
  jt->jerr=0;
  if(jt->iepdo&&JT(jt,iep)){
   // if there is an immex latent expression (9!:27), execute it before prompting
   jt->iepdo=0; z=immex(JT(jt,iep));  // reset requesy flag; run sentence & force typeout
   if(z&&AFLAG(z)&AFDEBUGRESULT)break;  // dbr * exits suspension, even dbr 1.  PFkeys may come through iep
   tpop(old);  // if we don't need the result for the caller here, free up the space
  }
  // Execute one sentence from the user
  if((inp=jgets("      "))==0){z=0; break;} z=immex(inp); // force prompt and typeout read and execute a line, but exit debug if error reading line
  // If the result came from a suspension-ending command, get out of suspension
  if(z&&AFLAG(z)&AFDEBUGRESULT)break;  // dbr * exits suspension, even dbr 1
  tpop(old);  // if we don't need the result for the caller here, free up the space
 }
 // Coming out of suspension.  z has the result to pass up the line, containing the suspension-ending info
 // Reset stack
 if(JT(jt,dbuser)){
#if USECSTACK
  jt->cstackmin+=CSTACKSIZE/10;
#else
  jt->fdepn-=NFDEP/10;
#endif
  jt->fcalln-=NFCALL/10;
 } else {
#if USECSTACK
  jt->cstackmin=jt->cstackinit-(CSTACKSIZE-CSTACKRESERVE);
#else
  jt->fdepn =NFDEP;
#endif
  jt->fcalln =NFCALL;
 }
 debz(); 
 if(d)d->dcss=1;  // restore jgets() state if there is an active script
 R z;
}    /* user keyboard loop while suspended */

// Go into debug mode.  Run sentences in suspension until we come out of suspension
// Result is the value that will be used for the failing sentence.  This should not be 0 unless there is an error, because
// jtxdefn requires nonzero z during normal operation
// We come into debug when there has been an error with debug enabled.  Stops are detected as errors before the stopped line is executed.
// Tacit definitions detect stop before they execute.
static A jtdebug(J jt){A z=0;C e;DC c,d;
 c=jt->sitop; NOUNROLL while(c){if(c->dctype==DCCALL)c->dcss=0; c=c->dclnk;}  // clear all previous ss state, since this might be a new error
 RZ(d=suspset(jt->sitop));  // find the topmost CALL frame and mark it as suspended
 if(d->dcix<0)R 0;  // if the verb has exited, all we can do is return
 e=jt->jerr; jt->jerr=0;
 // Suspend.  execute from the keyboard until a suspension-ending result is seen
 z=susp();
 // Process the end-of-suspension.  There are several different ending actions
 // The end block is a list of boxes, where the first box, an integer atom, contains the operation type
 I susact;   // requested action
 if(!z||AN(z)==0)susact=SUSCLEAR;  // if error in suspension, exit debug mode; empty arg is always 13!:0
 else susact=IAV(AAV(z)[0])[0];  // (0;0) {:: z
 switch(susact){
 case SUSRUN:  // rerun, possibly with changed arguments for tacit verb
  // rerun the line; pass the arguments, if any, as the result
  DGOTO(d,d->dcix); z=behead(z); break;
 case SUSRET: // exit current function, with result
  DGOTO(d,-1)   z=AAV(z)[1]; break;
 case SUSJUMP:  // goto line number.  Result not given, use i. 0 0
  DGOTO(d,lnumcw(IAV(AAV(z)[1])[0],d->dcc)) z=mtm; break;
 case SUSCLEAR:  // exit from debug
  jt->jerr=e;    // restore the error state before debug
  c=jt->sitop; z=mtm;  // in case no error, give empty result
  NOUNROLL while(c){if(DCCALL==c->dctype)DGOTO(c,-1) c=c->dclnk;} break;   // exit from all functions, back to immed mode
 case SUSNEXT:  // continue execution on next line
 case SUSSS:  // single-step continuation
  z=mtm; break;  // pick up wherever it left off; no result
 }
 d->dcsusp=0;   // Mark the current definition as no longer suspended
 // If there is an error, set z=0; if not, make sure z is nonzero (use i. 0 0)
 if(jt->jerr)z=0; // return z=0 to cause us to look for resumption address
 R z;
}

// post-execution error.  Used to signal an error on sentences whose result is bad only in context, i. e. non-nouns or assertions
// we reconstruct conditions at the beginning of the parse, and set an error on token 1.
A jtpee(J jt,A *queue,CW*ci,I err,I lk,DC c){A z=0;
 ASSERT(lk<=0,err);  //  locked fn is totally opaque, with no stack.  Exit with 0 result, indicating error
 // create a parser-stack frame for the old sentence and switch to it
 PFRAME oframe=jt->parserstackframe; PSTK newparseinfo[1]={{.a=(A)(queue+ci->ig.indiv.sentx),.t=ci->ig.indiv.sentn}};
 jt->parserstackframe.parserstkbgn=&newparseinfo[1]; jt->parserstackframe.parsercurrtok=1;  // unless locked, indicate failing-sentence info
 jsignal(err);   // signal the requested error
 jt->parserstackframe=oframe;  // restore to the executing sentence
 // enter debug mode if that is enabled
 if(c&&jt->uflags.us.cx.cx_c.db){jt->sitop->dcj=jt->jerr; z=debug(); jt->sitop->dcj=0;} //  d is PARSE type; set d->dcj=err#; d->dcn must remain # tokens debz();  not sure why we change previous frame
 if(jt->jerr)z=0; R z;  // if we entered debug, the error may have been cleared.  If not, clear the result.  Return debug result, which is result to use or 0 to indicate jump
}

// parsex: parse an explicit defn line when the debugger is running
/* w  - line to be parsed                           */
/* lk - 1 iff locked function; _1 to signal noun error at beginning of sentence */
/* ci - current row of control matrix               */
/* c  - stack entry for dbunquote for this function */
// d - DC area to use in deba

A jtparsex(J jt,A* queue,I m,CW*ci,DC c){A z;B s;
 movesentencetosi(jt,queue,m,0);  // install sentence-to-be-executed for stop purposes
 if(s=dbstop(c,ci->source)){z=0; jsignal(EVSTOP);}
 else                      {z=PARSERVALUE(parsea(queue,m));     }
 // If we hit a stop, or if we hit an error outside of try./catch., enter debug mode.  But if debug mode is off now, we must have just
 // executed 13!:0]0, and we should continue on outside of debug mode.  Error processing filled the current si line with the info from the parse
 if(!z&&jt->uflags.us.cx.cx_c.db){DC t=jt->sitop->dclnk; t->dcj=jt->sitop->dcj=jt->jerr; z=debug(); t->dcj=0;} //  d is PARSE type; set d->dcj=err#; d->dcn must remain # tokens
 R z;
}

A jtdbunquote(J jt,A a,A w,A self,L *stabent){A t,z;B s;DC d;V*sv;
 sv=FAV(self); t=sv->fgh[0]; 
 RZ(d=deba(DCCALL,a,w,self)); d->dcn=(I)stabent;
 if(CCOLON==sv->id&&(sv->flag&VXOP||t&&NOUN&AT(t))){  // : and executable body: either OP (adv/conj now with noun operands) or m : n
  ras(self); z=a?dfs2(a,w,self):dfs1(w,self); fa(self);
 }else{                              /* tacit    */
  d->dcix=0;  // set a pseudo-line-number for display purposes for the tacit 
  while(1){
   d->dcnewlineno=0;  // turn off 'reexec requested' flag
   if(s=dbstop(d,0L)){z=0; jsignal(EVSTOP);}  // if first line is a stop
   else              {ras(self); z=a?dfs2(a,w,self):dfs1(w,self); fa(self);}
   // If we hit a stop, or if we hit an error outside of try./catch., enter debug mode.  But if debug mode is off now, we must have just
   // executed 13!:8]0, and we should continue on outside of debug mode
   if(!z&&jt->uflags.us.cx.cx_c.db){d->dcj=jt->jerr; movecurrtoktosi(jt); z=debug(); if(self!=jt->sitop->dcf)self=jt->sitop->dcf;}
   if(!(d->dcnewlineno&&d->dcix!=-1))break;  // if 'run' specified (by jump not to -1), loop again.  Otherwise exit with result given
   // for 'run', the value of z gives the argument(s) to set; but if no args given, leave them unchanged
   if(AN(z)){w=AAV(z)[0]; a=AN(z)==2?AAV(z)[1]:0;}  // extract new args if there are any
  }
 }
 if(d->dcss)ssnext(d,d->dcss);  // if we step over/into on the last line of a function, we must stop on the next line of the caller
 debz();
 R z;
}    /* function call, debug version */

F1(jtdbq){ASSERTMTV(w); R sc(JT(jt,dbuser));}
     /* 13!:17 debug flag */

// Suspension-ending commands.  These commands return a list of boxed flagged with the AFDEBUGRESULT flag.  The first box is always an integer atom and gives the type
// of exit (run, step, clear, etc).  Other boxes give values for the run and ret types.  EXCEPTION: 13!:0 returns i. 0 0 for compatibility, but still flagged as AFDEBUGRESULT
F1(jtdbc){UC k;
 ARGCHK1(w);
 if(AN(w)){
  RE(k=(UC)i0(w));
  ASSERT(!(k&~0x81),EVDOMAIN);
  ASSERT(!k||!jt->glock,EVDOMAIN);
 }
 if(AN(w)){
  jt->uflags.us.cx.cx_c.db=k&1; JT(jt,dbuser)=k;
#if USECSTACK
  jt->cstackmin=jt->cstackinit-((CSTACKSIZE-CSTACKRESERVE)>>k);
#else
  jt->fdepn=NFDEP>>k;
#endif
  jt->fcalln=NFCALL/(k?2:1);
 }

 A z; RZ(z=ca(mtm)); AFLAGORLOCAL(z,AFDEBUGRESULT) R z;
}    /* 13!:0  clear stack; enable/disable suspension */

F1(jtdbrun ){ASSERTMTV(w); A z; RZ(z=mkwris(box(sc(SUSRUN)))); AFLAGORLOCAL(z,AFDEBUGRESULT) R z;}
     /* 13!:4  run again */

F1(jtdbnext){ASSERTMTV(w); A z; RZ(z=mkwris(box(sc(SUSNEXT)))); AFLAGORLOCAL(z,AFDEBUGRESULT) R z;}
     /* 13!:5  run next */

F1(jtdbret ){ARGCHK1(w); A z; RZ(z=mkwris(link(sc(SUSRET),box(w)))); AFLAGORLOCAL(z,AFDEBUGRESULT) R z;}
     /* 13!:6  exit with result */

F1(jtdbjump){I jump; RE(jump=i0(w)); A z; RZ(z=mkwris(link(sc(SUSJUMP),sc(jump)))); AFLAGORLOCAL(z,AFDEBUGRESULT) R z;}
     /* 13!:7  resume at line n (return result error if out of range) */

static F2(jtdbrr){DC d;
 RE(0);
 d=jt->sitop; NOUNROLL while(d&&DCCALL!=d->dctype)d=d->dclnk; 
 ASSERT(d&&VERB&AT(d->dcf)&&!d->dcc,EVDOMAIN);  // must be tacit verb
 A z; RZ(z=box(w)); if(a)RZ(z=over(w,box(a))); 
 RZ(z=mkwris(link(sc(SUSRUN),z))); AFLAGORLOCAL(z,AFDEBUGRESULT)  // RUN ; w [;a]
 R z;
}

F1(jtdbrr1 ){R dbrr(0L,w);}   /* 13!:9   re-run with arg(s) */
F2(jtdbrr2 ){R dbrr(a, w);}

F1(jtdbtrapq){ASSERTMTV(w); R JT(jt,dbtrap)?JT(jt,dbtrap):mtv;}   
     /* 13!:14 query trap */

F1(jtdbtraps){RZ(w=vs(w)); if(AN(w)){RZ(ras(w)); fa(JT(jt,dbtrap)); JT(jt,dbtrap)=w;}else JT(jt,dbtrap)=0L; R mtm;}
     /* 13!:15 set trap */
