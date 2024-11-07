/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
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
void moveparseinfotosi(J jt){movesentencetosi(jt,jt->parserstackframe.parserstkbgn[-1].a,jt->parserstackframe.parserstkbgn[-1].t,infererrtok(jt)); }

/* deba() and debz() must be coded and executed in pairs */
/* in particular, do NOT do error exits between them     */
/* e.g. the following is a NO NO:                        */
/*    d=deba(...);                                       */
/*    ASSERT(blah,EVDOMAIN);                             */
/*    debz()                                             */
// t is the type of stack entry.  The entry is allocated and filled in
// the type DCPM is for powt-mortem debugging.  It is like DCCALL but chained on jt->pmstacktop rather than jt->sitop, and chained
// in bottom-to-top order that must be reversed when transferred to the debug stack
// meanings of the args:
// DCPARSE: x=&tokens  y=#tokens
// DCSCRIPT: y=script text  fs=script index in script list
// DCCALL: x=left arg if any  y=right arg if any  fs=self
// DCJUNK: N/A
// DCPM: t=(line#<<8)+(dyadic<<7)+type  x=&local symbols   y=&control words  fs=self
DC jtdeba(J jt,I t,void *x,void *y,A fs){DC d;
 {A q; GAT0(q,LIT,sizeof(DST),1); d=(DC)AV1(q);}
 mvc(sizeof(DST),d,MEMSET00LEN,MEMSET00);
 I tt=t&0xf;  // type is low bits
 if(jt->sitop&&t<DCJUNK)moveparseinfotosi(jt);  // if we are creating a space between normal and suspension, don't modify the normal stack
 DC *root=tt==DCPM?&jt->pmstacktop:&jt->sitop;  // choose chain to add to
 d->dclnk=*root; *root=d;  // chain new block at head (oldest for normal blocks, youngest for DCPM)
 switch(tt){
 case DCPARSE:  d->dcy=(A)x; d->dcn=(I)y; break;
 case DCSCRIPT: d->dcy=y; d->dcm=(I)fs; break;
 case DCCALL:   
  d->dcx=x; d->dcy=y; d->dcf=fs; 
  d->dcdyad=AT(fs)&VERB&&x&&y;  // remember if dyadic verb execution
  d->dca=jt->curname;
  d->dcstop=-2;
  // dcn fill in in caller
  // if we were waiting to step into a function, this is it: mark this function as stoppable
  // and remove the stop in the caller
  DC e=d->dclnk; NOUNROLL while(e&&DCCALL!=e->dctype)e=e->dclnk;  // find previous call
  if(e&&e->dcss==SSSTEPINTOs){d->dcss=SSSTEPINTOs; e->dcss=0;}
  break;
 case DCPM:
  d->dcix=t>>8; d->dcf=fs; d->dcloc=x; d->dcc=y;  // save input parms
  d->dcdyad=(t>>7)&1;  // remember if dyadic verb execution
  d->dca=jt->curname;  // save executing name, if any
  d->dcstop=-2;  // ??
  d->dcj=jt->jerr;  // save error# - not really needed - used to see if in suspension
  tt=DCCALL; d->dcpflags=1;  // turn DCPM into DCCALL with pflags set
 }
 d->dctype=tt;  // install type, which might have been modified
 R d;
}    /* create new top of si stack */

void jtdebz(J jt){if(jt->sitop!=0)jt->sitop=jt->sitop->dclnk;}  // stack may vanish if 13!:0]0
     /* remove     top of si stack */

F1(jtsiinfo){A z,*zv;DC d;I c=5,n,*s;
 ASSERTMTV(w);
 n=0; d=jt->sitop; NOUNROLL while(d){++n; d=d->dclnk;}
 GATV0(z,BOX,c*n,2); s=AS(z); s[0]=n; s[1]=c; zv=AAV2(z);
 d=jt->sitop;
 while(d){
  RZ(zv[0]=sc(d->dctype));
  RZ(zv[1]=d->dcsusp?scc('*'):scc(' '));
  RZ(zv[2]=sc((I)d->dcss));
  RZ(zv[3]=d->dctype==DCCALL?sc(lnumsi(d)):mtv);
  switch(d->dctype){
  case DCPARSE:  RZ(zv[4]=unparse(d->dcy)); break;
  case DCCALL:   RZ(zv[4]=d->dca?sfn(0,d->dca):mtv);   break;
  case DCSCRIPT: zv[4]=d->dcy;              break;
  case DCJUNK:   zv[4]=mtv;                 break; 
  }
  zv+=c; d=d->dclnk;
 }
 R z;
}    /* 13!:32 si info */

// w is the compiled cw/sentence table
I lnumcw(I j,A w){
 if(0>j)R -2; 
 else if(!w)R j; 
 else{A *u=CWBASE(w); I n=CWNC(w);  DO(n-1, if(j<=CWSOURCE(u,-n,~i))R i;) R IMAX/2;}  // n-1 to stop before sentinel
}    /* line number in CW corresp. to j */

I lnumsi(DC d){A c;I i;
 if(c=d->dcc){i=d->dcix; A *u=CWBASE(c); I n=CWNC(c); R(CWSOURCE(u,-n,~MIN(i,n-2)));}else R 0;

}    /* source line number from DCCALL-type stack entry */

// set suspension flag on the most recent execution.  Called before going into suspension, and always with debug turned on.
// The flag is set in the most recent PARSE frame, if any; if the most recent thing is a tacit CALL, use that.  If the most
// recent frame is explicit CALL (no PARSE), that's weird: xdefn must have died aborning, do nothing
static DC suspset(DC d){DC e=0;
 NOUNROLL while(d&&d->dctype!=DCCALL){if(!e&&d->dctype==DCPARSE)e=d; d=d->dclnk;}  // find topmost call, e=first PARSE
 if(!d)R 0;                /* don't suspend if no such call     */
 if(d->dcc){RZQ(e); e->dcsusp=1;}               // if explicit, set susp on line - there should always be a following frame, but if not do nothing
 else      d->dcsusp=1;                         /* if not explicit, set susp on call */
 R d;
}

// 13!:24 Install suspension at the end of the debug stack.  Valid only when debug on.
// Result is i.0
F1(jtdbisolatestk){
 ASSERT(jt->uflags.trace&TRACEDB1,EVNONCE);  // debug must be on
 ASSERTMTV(w)  // no arg
 if(jt->sitop)jt->sitop->dcsusp=1;  // install suspension
 R mtv;
}

static B jterrcap(J jt){A y,*yv;
 GAT0(y,BOX,4,1); yv=AAV1(y);
 RZ(yv[0]=sc(jt->jerr1));
 RZ(yv[1]=str(jt->etxn1,jt->etxinfo->etx));
 RZ(yv[2]=dbcall(mtv));
 RZ(yv[3]=locname(mtv));
 RZ(symbis(nfs(22L,"STACK_ERROR_INFO_base_"),y,mark));
 R 1;
}    /* error capture */

// suspension.  Loop on keyboard input.  Keep executing sentences until something changes dbsusact.
// superdebug is set if we are reading the lines from a script
static A jtsusp(J jt, C superdebug){A z;
 // normally we run with an empty stack frame which is always ready to hold the display of the next sentence
 // to execute; the values are filled in when there is an error.  We are about to call immex to run sentences,
 // and it will create a stack frame for its result.  CREATION of this stack frame will overwrite the current top-of-stack
 // if it holds error information.  So, we create an empty frame to take the store from immex.  This frame has no display.
 RZ(deba(DCJUNK,0,0,0)); // create spacer frame
 A *old=jt->tnextpushp;  // fence must be after we have allocated our stack block
 JT(jt,dbuser)&=~TRACEDBSUSCLEAR;  // when we start a new suspension, wait for a new clear
 // Make sure we have a decent amount of stack space left to run sentences in suspension
 // ****** no errors till end of routine, where jt and stacks have been restored *******
 jt->cstackmin=MIN(jt->cstackinit-(CSTACKSIZE-CSTACKRESERVE),MAX(STACKPOS,jt->cstackinit-CSTACKSIZE+CSTACKDEBUGRESERVE)-CSTACKDEBUGRESERVE);
 // if there is a 13!:15 sentence (latent expression) to execute before going into debug, do it
 A trap=0; READLOCK(JT(jt,dblock)) if((trap=JT(jt,dbtrap))!=0)ra(trap); READUNLOCK(JT(jt,dblock))  // fetch trap sentence and protect it
 if(trap){RESETERR; immex(trap); fa(trap); tpop(old);}  // execute with force typeout, remove protection
 // Loop executing the user's sentences until one returns a value that is flagged as 'end of suspension'
 J jtold=jt;  // save the thread that we started in
 UI savcstackmin=0;  // when we switch threads, we keep our stack; so we must use our stack-end.  If this is not zero, we must reset the stack on exit/change
 JT(jt,promptthread)=THREADID(jt);  // set that the debug thread is the one allowed to prompt
 while(1){A  inp;   // this is the loop that repeatedly executes user commands from keyboard
  jt->jerr=0;   // not needed
  A iep=0;
  // We use TRACEDBSUSCLEAR up & down to indicate that we are clearing the debug stack & stopping all running explicits.
  // When it is set, we don't allow reentry into debug, and it remains set until we get to immediate level.  In other words,
  // it lasts until all suspensions are cleared.  BUT if we are reading suspension info from a script (for debug), we don't get back
  // to immediate level.  In that case we have to clear TRACEDBSUSCLEAR so that script lines can reenter suspension 
  if(unlikely(superdebug))JT(jt,dbuser)&=~(TRACEDBSUSCLEAR+TRACEDBSUSSS);
  // if there is an immex phrase, protect it during its execution
  if(jt->iepdo&1){READLOCK(JT(jt,felock)) if((iep=JT(jt,iep))!=0)ra(iep); READUNLOCK(JT(jt,felock))}
  if(iep){
   // if there is an immex expression (9!:27), execute it before prompting
   jt->iepdo=2; z=immex(iep); fa(iep); jt->iepdo&=~2;  // reset request flag, set to 'running'; run sentence & force typeout; undo the ra() that protected the immex sentence; clear running
   // this is tricky.  After every sentence we have to check to see if debug has been cleared, because we might get a second level of suspension running immex, and when it returns here
   // the suspension-ending values will have been cleared
   if(!(jt->uflags.trace&TRACEDB1)){z=0; break;}  // if we are clearing the stack (13!:0), we exit all suspensions.  z could have anything, so we clear it to prevent it from being analyzed as a suspension.
   if(JT(jt,dbuser)&TRACEDBSUSCLEAR+TRACEDBSUSSS)break;  // dbr 0/1 forces end of suspension, as does single-step request
   if(z&&AFLAG(z)&AFDEBUGRESULT)break;  // dbr * exits suspension, even dbr 1.  PFkeys may come through iep
   tpop(old);  // if we don't need the result for the caller here, free up the space
  }
  // Execute one sentence from the user
  if((inp=jgets("      "))==0){z=0; break;} inp=jtddtokens(jt,inp,1+!!EXPLICITRUNNING); z=immex(inp); // force prompt and typeout read and execute a line, but exit debug if error reading line
  if(unlikely(JT(jt,sidamage))){  // If there was SI damage (changing an executing function), execute 13!:0 (oldvalue) to clear the stack and exit suspension
   if(likely(JT(jt,dbuser)&TRACEDB1))jsto(JJTOJ(jt),MTYOER,"Debug suspension ended because an executing name was changed.  Debug is still enabled."); JT(jt,sidamage)=0; z=jtdbc(jt,num(JT(jt,dbuser)&TRACEDB1));  // give msg unless user has already cleared debug (very rare)
  }
  // If the result came from a suspension-ending command, get out of suspension
  // Kludge: 13!:0 and single-step can be detected here by flag bits in dbuser.  We do this because the lab code doesn't properly route the result of these to the
  // suspension result and we would lose them.  Fortunately they have no arguments.  debugmux also eats the result of 13!:0]0
  if(!(jt->uflags.trace&TRACEDB1)){z=0; break;}  // if we are clearing the stack (13!:0), we exit all suspensions.  z could have anything, so we clear it to prevent it from being analyzed as a suspension.
  if(JT(jt,dbuser)&TRACEDBSUSCLEAR+TRACEDBSUSSS)break;  // dbr 0/1 forces immediate end of suspension, as does single-step request
  if(z && AFLAG(z)&AFDEBUGRESULT && AT(z)&BOX && IAV(C(AAV(z)[0]))[0]==SUSTHREAD){  // (0;0) {:: z; is this T. y?
// obsolete ASSERTSYS(0,"debug thread"); continue;  // scaf
   J newjt=JTFORTHREAD(jt,IAV(C(AAV(z)[1]))[0]);  // T. y - switch to the indicated thread
   if(savcstackmin!=0)jt->cstackmin=savcstackmin;  // if the old jt had a modified stack limit, restore it
   savcstackmin=newjt->cstackmin; newjt->cstackmin=jtold->cstackmin; jt=newjt;  // switch to new jt, but keep our original stack limit
   JT(jt,promptthread)=THREADID(jt);  // set that the debug thread is the one allowed to prompt
   old=jt->tnextpushp;  // now that we are under a new jt, we must use its tpush stack
   continue;
  }
  if(likely(z!=0)&&AFLAG(z)&AFDEBUGRESULT)break;  // if suspension-ending command, end
  tpop(old);  // if we don't need the result for the caller here, free up the space
 }
 // Coming out of suspension.  z has the result to pass up the line, containing the suspension-ending info
 JT(jt,promptthread)=0;  // suspension over - the master thread is the one that can prompt
 if(savcstackmin!=0)jt->cstackmin=savcstackmin;  // if the old jt had a modified stack limit, restore it
 jt=jtold;  // Reset to original debug thread.  NOTE that old is no longer valid, so don't tpop
 // Reset stack
  if(!(JT(jt,dbuser)&TRACEDB1))jt->cstackmin=jt->cstackinit-(CSTACKSIZE-CSTACKRESERVE);  // set stacklim to normal if we are exiting debug
// obsolete   if(JT(jt,dbuser)&TRACEDB1)jt->cstackmin=MIN(jt->cstackmin,MAX(STACKPOS,jt->cstackinit-CSTACKSIZE+CSTACKDEBUGRESERVE)-CSTACKDEBUGRESERVE); // if still debug, leave debug space
// obsolete  jt->cstackmin=jt->cstackinit-(CSTACKSIZE-CSTACKRESERVE);
// obsolete   jt->cstackmin+=CSTACKSIZE/10;
// obsolete  } else {
// obsolete  }
 debz(); 
 R z;
}    /* user keyboard loop while suspended */

// Go into debug mode.  Run sentences in suspension until we come out of suspension
// Result is the value that will be used for the failing sentence.  This should not be 0 unless there is an error, because
// jtxdefn requires nonzero z during normal operation
// We come into debug when there has been an error with debug enabled.  Stops are detected as errors before the stopped line is executed.
// Tacit definitions detect stop before they execute.
static A jtdebug(J jt){A z=0;C e;DC c,d;
  __atomic_store_n(&JT(jt,adbreak)[1],0,__ATOMIC_RELEASE);  // Now that we know all threads have gone into debug, we must clear ATTN/BREAK in case we start an explicit definition
 c=jt->sitop; NOUNROLL while(c){if(c->dctype==DCCALL)c->dcss=0; c=c->dclnk;}  // clear all previous ss state, since this might be a new error
 d=suspset(jt->sitop);  // find the topmost CALL frame and mark it as suspended
 if(unlikely(d==0)||unlikely(d->dcix<0)){   // if we cannot suspend, either because stack is malformed or because the verb has finished, abort back to console level as if dbr 1 entered
  jsto(JJTOJ(jt),MTYOER,"Debug suspension ended by program logic error.    Debug is still enabled.");  // Tell the user we had a problem
  NOUNROLL for(c=jt->sitop;c;c=c->dclnk){if(c->dctype==DCCALL){DGOTO(c,-1) c->dcsusp=0;}} R 0;  // exit from all explicit defns; return error, with jt->err unchanged
 }
 e=jt->jerr; jt->jerr=0;  // remember error and reset it, possibly to be restored later
 // If the failure happened while a script was being loaded, we have to take jgets() out of script mode so we can prompt the user.  We will restore on exit
 DC sf; for(sf=jt->sitop; sf&&sf->dctype!=DCSCRIPT; sf=sf->dclnk);  // sf-> last SCRIPT type, if any
 C superdebug=JT(jt,dbuser)&TRACEDBSUSFROMSCRIPT;   // remember if we have to keep scripts reading for debug
 if(sf&&!superdebug)sf->dcss=0;  // in super-debug mode (dbr 16b81), we continue reading suspension lines from the script; otherwise turn it off
 // Suspend.  execute from the keyboard until a suspension-ending result is seen.  Clear the current name to start the suspension
 A savname=jt->curname; z=susp(superdebug); jt->curname=savname;  // suspension starts as anonymous
 // Process the end-of-suspension.  There are several different ending actions
 // The end block is a list of boxes, where the first box, an integer atom, contains the operation type
 // If we end because user has exited debug, z is 0
 I susact;   // requested action
 if(unlikely((jt->jerr&~0x60)==EVEXIT))susact=-1;  // if we are heading up the stack, no suspension action
 else {
  if(!z||AN(z)==0){JT(jt,dbuser)|=TRACEDBSUSCLEAR;}   // general exit from debug without a suspension-ending result; synthesize a 'clear' result
  if(JT(jt,dbuser)&TRACEDBSUSCLEAR+TRACEDBSUSSS){susact=JT(jt,dbuser)&TRACEDBSUSCLEAR?SUSCLEAR:SUSSS; JT(jt,dbuser)&=~TRACEDBSUSSS;}  // give CLEAR priority over SS.  SS applies to one level; CLEAR applies to all suspensions and is turned off only at user level
  else susact=IAV(C(AAV(z)[0]))[0];  // (0;0) {:: z
 }
 // susact describes what is to be done; it has already been stored into dcss
 switch(susact){
 case SUSRUN:  // rerun, possibly with changed arguments for tacit verb
  // rerun the line; pass the arguments, if any, as the result
  DGOTO(d,d->dcix); z=behead(z); break;
 case SUSRET: // exit current function, with result
  DGOTO(d,-1)   z=C(AAV(z)[1]); break;
 case SUSJUMP:  // goto line number.  Result not given, use i. 0 0
  DGOTO(d,lnumcw(IAV(C(AAV(z)[1]))[0],d->dcc)) z=mtm; break;
 case SUSCLEAR:  // exit from debug, which is achieved by returning through all levels up to user prompt.  We come here once for each suspension level
  jt->jerr=e; z=0;   // Restore the error that threw us into debug; set z=0 to check for GOTO at all levels
  NOUNROLL for(c=jt->sitop;c;c=c->dclnk){
   switch(c->dctype){
   case DCCALL: DGOTO(c,-1) c->dcsusp=0; break;   // exit from all functions, clearing suspensions; back to immed mode with clear debug stack
   case DCSCRIPT: if(!superdebug)c->dcix=-1; break;   // set line# in script to 'EOF' to suppress reading - but if we continue reading testcase lines, leave it open
   }
  }
  break;
 case SUSNEXT:  // continue execution on next line
 case SUSSS:  // single-step continuation
  z=mtm; break;  // pick up wherever it left off; no result
 }
 d->dcsusp=0;   // Mark the current definition as no longer suspended
 if(sf)sf->dcss=1;  // restore jgets() state if there is an active script

 // If there is an error, set z=0; if not, make sure z is nonzero (use i. 0 0)
 if(jt->jerr){z=0; jt->emsgstate|=EMSGSTATEFORMATTED;} // return z=0 to cause us to look for resumption address.  any message is for a long-gone self; don't call eformat for it.  If no message, allow the next one to be formatted
 R z;
}

// Take system lock before going into debug.  If the debug request is granted to another thread, keep putting it up until we get it
// return is result from debug, or 0 if we are exiting debug or hit an error
static A jtdebugmux(J jt){A z;
 do{
  z=jtsystemlock(jt,LOCKPRIDEBUG,jtdebug);  // request debug
  // when we return, we may not have been the selected thread, in which case we need to put our request up again.
  // but if the user directed us to terminate, we must do that.
  if(!(jt->uflags.trace&TRACEDB)){RESETERR R 0;}  // if user turned off debug mode, fail all tasks (back to a try.) and print no message.  This is the only valid time for result 0 with jt->jerr=0
  if(jt->jerr==EVDEBUGEND){R 0;}  // if user suppressed this thread, fail it back to start/console (with no message)
 }while(z==(A)1);  // loop back if we were not the selected thread
 R z;  // if we were selected, carry on as requested by user: line# will have been set, and value if any
}

// post-execution error.  Used to signal an error on sentences whose result is bad only in context, i. e. non-nouns or assertions
// we reconstruct conditions at the beginning of the parse, and set an error on token 1.
A jtpee(J jt,A *queue,UI8 tcesx2,I err,I lk,DC c){A z=0;
 ASSERT(lk<=0,err);  //  locked fn is totally opaque, with no stack.  Exit with 0 result, indicating error
 // create a parser-stack frame for the old sentence and switch to it
 PFRAME oframe=jt->parserstackframe; PSTK newparseinfo[1]={{.a=(A)(&queue[(tcesx2>>32)&TCESXSXMSK]),.t=(tcesx2-(tcesx2>>32))&TCESXSXMSK}};
 jt->parserstackframe.parserstkbgn=&newparseinfo[1]; jt->parserstackframe.parseroridetok=0; // unless locked, indicate failing-sentence info
 jsignal(err);   // signal the requested error
 jt->parserstackframe=oframe;  // restore to the executing sentence
 // enter debug mode if that is enabled, provided we are not on the way out of debug
 if(c&&(jt->uflags.trace&TRACEDB)&&!(JT(jt,dbuser)&TRACEDBSUSCLEAR)){jt->sitop->dcj=jt->jerr; z=jtdebugmux(jt); jt->sitop->dcj=0;} //  d is PARSE type; set d->dcj=err#; d->dcn must remain # tokens debz();  not sure why we change previous frame
 if(jt->jerr)z=0; R z;  // if we entered debug, the error may have been cleared.  If not, clear the result.  Return debug result, which is result to use or 0 to indicate jump
}

// parsex: parse an explicit defn line when the debugger/pm/ATTN is running
// queue is words of sentence, length m
// source - source line#
/* c  - stack entry for dbunquote for this function */
A jtparsex(J jt,A* queue,I m,I source,DC c){A z,parsez;
 movesentencetosi(jt,queue,m,0);  // install sentence-to-be-executed for stop purposes
 // if there is a system lock to take, take it and continue
 S attnval=__atomic_load_n((S*)JT(jt,adbreakr),__ATOMIC_ACQUIRE);
 if(attnval&(S)~0xff){jtsystemlockaccept(jt,LOCKALL);}
 // if there is an ATTN/BREAK to take, take it and enter debug suspension
 if(attnval&0xff){
  if(!(jt->uflags.trace&TRACEDB1))__atomic_store_n(JT(jt,adbreak),2,__ATOMIC_RELEASE);  // if not debug, promote the ATTN to BREAK for other threads to speed it up
  jsignal(EVATTN); z=parsez=0; goto noparse;  // if debug is not enabled, this will just be an error in the unparsed line
 }
 // we can stop before the sentence, or after it if it fails.  Stopping before is better because then we know we can restart safely
 // if there is a stop, enter debug suspension
 if(c&&dbstop(c,source)){z=parsez=0; jsignal(EVSTOP); goto noparse;}
 // xdefn adds a stack entry for PARSE, needed to get anonymous operators right
 z=PARSERVALUE(parsez=parsea(queue,m));  // make sure we preserve ASGN flag in parsez
noparse: ;
 // If we hit a stop or ATTN, or if we hit an error (outside of try./catch., which turns debug off), enter debug suspension if enabled.  But if debug mode is off now, we must have just
 // executed 13!:0]0 or a suspension-ending command, and we should continue on outside of debug mode.  Error processing filled the current si line with the info from the parse
 // Also, if we just executed dbr 0/1 or its equivalent (which set SUSCLEAR to clear back to immediate mode), don't reenter debug
 if(!z&&c&&(jt->uflags.trace&TRACEDB)&&!(JT(jt,dbuser)&TRACEDBSUSCLEAR)){
  if(jt->jerr==EVCUTSTACK){
  // If the line failed with EVCUTSTACK, it must be the caller's line that called the function where Cut Stack ran (the Cut Stack was returned from suspension).  This frame has already been
  // set up to restart on the same line, so all we have to do is clear the error and return 0 so that debugnewi restarts the line
   RESETERR parsez=0;  // set to branch to the selected line in this frame
  }else{
   // go into suspension
   DC t=jt->sitop->dclnk; t->dcj=jt->sitop->dcj=jt->jerr; parsez=jtdebugmux(jt); t->dcj=0; //  d is PARSE type; set d->dcj=err#; d->dcn must remain # tokens
  }
 }
 // we have come out of suspension (if we went into it).  parsez has the value to return to execution.  It may have a value created by the user, or the value from the non-failing sentence.
 // Or, it may be 0, in which case the definition will look for a restart.  In this case the error-code matters: the error will be printed when the failure reaches console level (in the master thread).  An error code of
 // EVDEBUGEND is used to force quiet failure all the way back to console level (usually in a task); an error code of 0, normal in a single-task system, will cause the failure to go back to
 // the first try./catch. or starting level and then print nothing.
 R parsez;
}

A jtdbunquote(J jt,A a,A w,A self,DC d){F2PREFIP;A t,z;B s;V*sv;
 sv=FAV(self); t=sv->fgh[0]; 
 if(sv->id==CCOLONE&&t!=0){  // : explicit and not anonymous (if anonymous, it goes to unquote which will reexecute the vebr with a name)
  ras(self); a?df2ip(z,a,w,self):df1ip(z,w,self);   if(unlikely(z==0)){jteformat(jt,self,a?a:w,a?w:0,0);} fa(self);  // we have self, so this can be a format point
 }else{                              /* tacit    */
  d->dcix=0;  // set a pseudo-line-number for display purposes for the tacit 
  while(1){
   d->dcnewlineno=0;  // turn off 'reexec requested' flag
   if(s=dbstop(d,0L)){z=0; jsignal(EVSTOP);}  // if first line is a stop
   else              {ras(self); a?df2ip(z,a,w,self):df1ip(z,w,self); if(unlikely(z==0)){jteformat(jt,self,a?a:w,a?w:0,0);} fa(self);}
   // If we hit a stop, or if we hit an error outside of try./catch., enter debug mode.  But if debug mode is off now, we must have just
   // executed 13!:0]0, and we should continue on outside of debug mode.  The debug stack frames are still on the stack, but they have been unchained from the root
   // Don't reenter debug if we are on the way out of it
   if(!z&&(jt->uflags.trace&TRACEDB)&&!(JT(jt,dbuser)&TRACEDBSUSCLEAR)){d->dcj=jt->jerr; movecurrtoktosi(jt); z=jtdebugmux(jt); if(jt->sitop!=0)self=jt->sitop->dcf;}
   if(!(d->dcnewlineno&&d->dcix!=-1))break;  // if 'run' specified (by jump not to -1), loop again.  Otherwise exit with result given
   // for 'run', the value of z gives the argument(s) to set; but if no args given, leave them unchanged
   if(AN(z)){w=C(AAV(z)[0]); a=AN(z)==2?C(AAV(z)[1]):0;}  // extract new args if there are any
  }
 }
 if(d->dcss)ssnext(d,d->dcss);  // if we step over/into on the last line of a function, we must stop on the next line of the caller
 R z;
}    /* function call, debug version */

F1(jtdbq){ASSERTMTV(w); R sc(JT(jt,dbuser)&~TRACEDBSUSCLEAR);}
     /* 13!:17 debug flag */

// x 13!:11 y set error number(s) in threads.  Error _1 is converted to EVDEBUGEND
F2(jtdberr2){
 ARGCHK2(a,w);
 ASSERT(AR(a)<2,EVRANK); ASSERT(AN(a)>0,EVDOMAIN); if(AT(a)!=INT)RZ(a=cvt(INT,a));  // verify #threads OK
 ASSERT(AR(w)<2,EVRANK); ASSERT(AN(w)==1||AN(w)==AN(a),EVDOMAIN); if(AT(w)!=INT)RZ(w=cvt(INT,w));  // verify #err#s OK
 DONOUNROLL(AN(a), ASSERT(BETWEENO(IAV(a)[i],0,THREADIDFORWORKER(JT(jt,wthreadhwmk))),EVDOMAIN))  // verify threads exist
 JTT *jjbase=JTTHREAD0(jt); I winc=AN(a)!=1;  //  base of thread blocks  1 if we are not repeating w
 DONOUNROLL(AN(a), jjbase[IAV(a)[i]].jerr=IAV(w)[i*winc]>=0?IAV(w)[i*winc]:EVDEBUGEND;)  // install the requested error number
 R mtm;  // i. 0 0
}

// x 13!:23 y  return y if it is a suspension result, otherwise x (which defaults to i. 0 0).  Bivalent 
F2(jtdbpasss){I t=AT(w); w=t&NOUN?w:a; a=t&NOUN?a:mtm; RETF(AFLAG(w)&AFDEBUGRESULT?w:a); }

// Suspension-ending commands.  These commands return a list of boxes flagged with the AFDEBUGRESULT flag.  The first box is always an integer atom and gives the type
// of exit (run, step, clear, etc).  Other boxes give values for the run and ret types.  EXCEPTION: 13!:0 returns i. 0 0 for compatibility, but still flagged as AFDEBUGRESULT
F1(jtdbc){I k;
 ARGCHK1(w);
 RE(k=i0(w));
 ASSERT(!(k&~(TRACEDBDEBUGENTRY|TRACEDBSUSFROMSCRIPT|TRACEDB1)),EVDOMAIN);
 ASSERT(!k||!jt->glock,EVDOMAIN);
 // turn debugging on/off in all threads
 JTT *jjbase=JTTHREAD0(jt);  // base of thread blocks
 DONOUNROLL(NALLTHREADS(jt), if(k&1)__atomic_fetch_or(&jjbase[i].uflags.trace,TRACEDB1,__ATOMIC_ACQ_REL);else __atomic_fetch_and(&jjbase[i].uflags.trace,~TRACEDB1,__ATOMIC_ACQ_REL);) JT(jt,dbuser)=k;
 jt->cstackmin=jt->cstackinit-((CSTACKSIZE-CSTACKRESERVE)>>(k&TRACEDB1));  // if we are setting debugging on, shorten the C stack to allow suspension commands room to run
 JT(jt,dbuser)|=TRACEDBSUSCLEAR; if(unlikely((k&TRACEDBDEBUGENTRY|TRACEDBSUSFROMSCRIPT)))JT(jt,dbuser)&=~TRACEDBSUSCLEAR;  // come out of suspension, whether 0 or 1.  If going into pm debug or running, suppress so don't immediately come out of debug; also if staying in script mode
 A z; RZ(z=ca(mtm)); AFLAGORLOCAL(z,AFDEBUGRESULT) R z;
}    /* 13!:0  clear stack; enable/disable suspension */

F1(jtdbrun ){ASSERTMTV(w); A z; RZ(z=mkwris(box(sc(SUSRUN)))); AFLAGORLOCAL(z,AFDEBUGRESULT) R z;}
     /* 13!:4  run again */

F1(jtdbnext){ASSERTMTV(w); A z; RZ(z=mkwris(box(sc(SUSNEXT)))); AFLAGORLOCAL(z,AFDEBUGRESULT) R z;}
     /* 13!:5  run next */

F1(jtdbret ){ARGCHK1(w); A z; RZ(z=mkwris(jlink(sc(SUSRET),box(w)))); AFLAGORLOCAL(z,AFDEBUGRESULT) R z;}
     /* 13!:6  exit with result */

F1(jtdbjump){I jump; RE(jump=i0(w)); A z; RZ(z=mkwris(jlink(sc(SUSJUMP),sc(jump)))); AFLAGORLOCAL(z,AFDEBUGRESULT) R z;}
     /* 13!:7  resume at line n (return result error if out of range) */

static F2(jtdbrr){DC d;
 RE(0);
 d=jt->sitop; NOUNROLL while(d&&DCCALL!=d->dctype)d=d->dclnk; 
 ASSERT(d&&VERB&AT(d->dcf)&&!d->dcc,EVDOMAIN);  // must be tacit verb
 A z; RZ(z=box(w)); if(a)RZ(z=over(w,box(a))); 
 RZ(z=mkwris(jlink(sc(SUSRUN),z))); AFLAGORLOCAL(z,AFDEBUGRESULT)  // RUN ; w [;a]
 R z;
}

F1(jtdbrr1 ){R dbrr(0L,w);}   /* 13!:9   re-run with arg(s) */
F2(jtdbrr2 ){R dbrr(a, w);}

// T. y - set debugging thread #
// This is a suspension command, but not suspension-ending
F1(jttcapdot1){I thno; RE(thno=i0(w)); ASSERT(BETWEENO(thno,0,THREADIDFORWORKER(JT(jt,wthreadhwmk))),EVDOMAIN) A z; RZ(z=mkwris(jlink(sc(SUSTHREAD),sc(thno)))); AFLAGORLOCAL(z,AFDEBUGRESULT) R z;}

// 13!:14, query suspension trap sentence
F1(jtdbtrapq){
 ASSERTMTV(w); 
 // we must read & protect the sentence under lock in case another thread is changing it
 READLOCK(JT(jt,dblock)) A trap=JT(jt,dbtrap); if(trap)ras(trap); READUNLOCK(JT(jt,dblock))  // must ra() while under lock
 if(trap){tpushnr(trap);}else trap=mtv;  // if we did ra(), stack a fa() on the tpop stack
 R trap;
}

// 13!:15, set suspension trap sentence
F1(jtdbtraps){
 ARGCHK1(w);
 RZ(w=vs(w));
 if(AN(w)){RZ(ras(w));}else w=0;  // protect w if it is nonempty; if empty, convert to null
 WRITELOCK(JT(jt,dblock)) A trap=JT(jt,dbtrap); JT(jt,dbtrap)=w; WRITEUNLOCK(JT(jt,dblock))  // swap addresses under lock
 fa(trap);  // undo the ra() done when value was stored - null is ok
 R mtm;
}


