/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
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
 NOUNROLL while(d&&DCCALL!=d->dctype){e=d; d=d->dclnk;}  // find bottommost call, e=previous ele
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
 if(d&&!(JT(jt,dbuser)&DBSUSFROMSCRIPT))d->dcss=0;  // in super-debug mode (dbr 16b81), we continue reading suspension lines from the script; otherwise turn it off
 JT(jt,dbuser)&=~DBSUSCLEAR;  // when we start a new suspension, wait for a new clear
 // Make sure we have a decent amount of stack space left to run sentences in suspension
#if USECSTACK
 jt->cstackmin=MAX(jt->cstackinit-(CSTACKSIZE-CSTACKRESERVE),jt->cstackmin-CSTACKSIZE/10);
#else
 jt->fdepn =MIN(NFDEP ,jt->fdepn +NFDEP /10);
#endif
 jt->fcalln=MIN(NFCALL,jt->fcalln+NFCALL/10);
 // if there is a 13!:15 sentence (latent expression) to execute before going into debug, do it
 A trap=0; READLOCK(JT(jt,dblock)) if((trap=JT(jt,dbtrap))!=0)ra(trap); READUNLOCK(JT(jt,dblock))  // fetch trap sentence and protect it
 if(trap){RESETERR; immex(trap); fa(trap); tpop(old);}  // execute with force typeout, remove protection
 // Loop executing the user's sentences until one returns a value that is flagged as 'end of suspension'
 J jtold=jt;  // save the thread that we started in
 UI savcstackmin=0;  // when we switch threads, we keep our stack; so we must use our stack-end.  If this is not zero, we must reset the stack on exit/change
 while(1){A  inp;
  jt->jerr=0;
  A iep=0;
  // if there is an immex phrase, protect it during its execution
  if(jt->iepdo){READLOCK(JT(jt,felock)) if((iep=JT(jt,iep))!=0)ra(iep); READUNLOCK(JT(jt,felock))}
  if(iep){
   // if there is an immex latent expression (9!:27), execute it before prompting
   jt->iepdo=0; z=immex(iep); fa(iep);  // reset request flag; run sentence & force typeout; undo the ra() that protected the immex sentence
   if(JT(jt,dbuser)&DBSUSCLEAR+DBSUSSS)break;  // dbr 0/1 forces end of suspension, as does single-step request
   if(z&&AFLAG(z)&AFDEBUGRESULT)break;  // dbr * exits suspension, even dbr 1.  PFkeys may come through iep
   tpop(old);  // if we don't need the result for the caller here, free up the space
  }
  // Execute one sentence from the user
  if((inp=jgets("      "))==0){z=0; break;} inp=jtddtokens(jt,inp,1+!!EXPLICITRUNNING); z=immex(inp); // force prompt and typeout read and execute a line, but exit debug if error reading line
  // If the result came from a suspension-ending command, get out of suspension
  // Kludge: 13 : 0 and single-step can be detected here by flag bits in dbuser.  We do this because the lab code doen't properly route the result of these to the
  // suspension result and we would lose them.  Fortunately they have no arguments
  if(JT(jt,dbuser)&DBSUSCLEAR+DBSUSSS)break;  // dbr 0/1 forces immediate end of suspension, as does single-step request
  if(z&&AFLAG(z)&AFDEBUGRESULT&&IAV(C(AAV(z)[0]))[0]==SUSTHREAD){  // (0;0) {:: z; is this T. y?
   J newjt=JTFORTHREAD(jt,IAV(C(AAV(z)[1]))[0]);  // T. y - switch to the indicated thread
   if(savcstackmin!=0)jt->cstackmin=savcstackmin;  // if the old jt had a modified stack limit, restore it
   savcstackmin=newjt->cstackmin; newjt->cstackmin=jtold->cstackmin; jt=newjt;  // switch to new jt, but keep our original stack limit
   old=jt->tnextpushp;  // now that we are under a new jt, we must use its tpush stack
   continue;
  }
  if(z&&AFLAG(z)&AFDEBUGRESULT)break;  // dbr * exits suspension, even dbr 1
  tpop(old);  // if we don't need the result for the caller here, free up the space
 }
 // Coming out of suspension.  z has the result to pass up the line, containing the suspension-ending info
 if(savcstackmin!=0)jt->cstackmin=savcstackmin;  // if the old jt had a modified stack limit, restore it
 jt=jtold;  // Reset to original debug thread.  NOTE that old is no longer valid, so don't tpop
 // Reset stack
 if(JT(jt,dbuser)&DB1){
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
  __atomic_store_n(&JT(jt,adbreak)[1],0,__ATOMIC_RELEASE);  // Now that we know all threads have gone into debug, we must clear ATTN/BREAK in case we start an explicit definition
 c=jt->sitop; NOUNROLL while(c){if(c->dctype==DCCALL)c->dcss=0; c=c->dclnk;}  // clear all previous ss state, since this might be a new error
 RZ(d=suspset(jt->sitop));  // find the topmost CALL frame and mark it as suspended
 if(d->dcix<0)R 0;  // if the verb has exited, all we can do is return
 e=jt->jerr; jt->jerr=0;
 // Suspend.  execute from the keyboard until a suspension-ending result is seen
 z=susp();
 // Process the end-of-suspension.  There are several different ending actions
 // The end block is a list of boxes, where the first box, an integer atom, contains the operation type
 I susact;   // requested action
 if(!z||AN(z)==0||JT(jt,dbuser)&DBSUSCLEAR+DBSUSSS){susact=JT(jt,dbuser)&DBSUSSS?SUSSS:SUSCLEAR; JT(jt,dbuser)&=~(DBSUSCLEAR+DBSUSSS);}  // if error in suspension, exit debug mode; empty arg or DBSUSCLEAR is always 13!:0
 else susact=IAV(C(AAV(z)[0]))[0];  // (0;0) {:: z
 // susact describes what is to be done; it has already been stored into dcss
 switch(susact){
 case SUSRUN:  // rerun, possibly with changed arguments for tacit verb
  // rerun the line; pass the arguments, if any, as the result
  DGOTO(d,d->dcix); z=behead(z); break;
 case SUSRET: // exit current function, with result
  DGOTO(d,-1)   z=C(AAV(z)[1]); break;
 case SUSJUMP:  // goto line number.  Result not given, use i. 0 0
  DGOTO(d,lnumcw(IAV(C(AAV(z)[1]))[0],d->dcc)) z=mtm; break;
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

// Take system lock before going into debug.  If the debug request is granted to another thread, keep putting it up until we get it
// return is result from debug, or 0 if we are exiting debug or hit an error
static A jtdebugmux(J jt){A z;
 do{
  z=jtsystemlock(jt,LOCKPRIDEBUG,jtdebug);  // request debug
  // when we return, we may not have been the selected thread, in which case we need to put our request up again.
  // but if the user directed us to terminate, we must do that.
  if(!jt->uflags.us.cx.cx_c.db){RESETERR R 0;}  // if user turned off debug mode, fail all tasks (back to a try.) and print no message.  This is the only valid time for result 0 with jt->jerr=0
  if(jt->jerr==EVDEBUGEND){R 0;}  // if user suppressed this thread, fail it back to start/console (with no message)
 }while(z==(A)1);  // loop back if we were not the selected thread
 R z;  // if we were selected, carry on as requested by user: line# will have been set, and value if any
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
 if(c&&jt->uflags.us.cx.cx_c.db){jt->sitop->dcj=jt->jerr; z=jtdebugmux(jt); jt->sitop->dcj=0;} //  d is PARSE type; set d->dcj=err#; d->dcn must remain # tokens debz();  not sure why we change previous frame
 if(jt->jerr)z=0; R z;  // if we entered debug, the error may have been cleared.  If not, clear the result.  Return debug result, which is result to use or 0 to indicate jump
}

// parsex: parse an explicit defn line when the debugger/pm/ATTN is running
// queue is words of sentence, length m
/* ci - current row of control matrix               */
/* c  - stack entry for dbunquote for this function */
A jtparsex(J jt,A* queue,I m,CW*ci,DC c){A z,parsez;
 movesentencetosi(jt,queue,m,0);  // install sentence-to-be-executed for stop purposes
 // if there is a system lock to take, take it and continue
 S attnval=__atomic_load_n((S*)JT(jt,adbreakr),__ATOMIC_ACQUIRE);
 if(attnval&(S)~0xff){jtsystemlockaccept(jt,LOCKPRISYM+LOCKPRIPATH+LOCKPRIDEBUG);}
 // if there is an ATTN/BREAK to take, take it and enter debug suspension
 if(attnval&0xff){
  if(!(jt->uflags.us.cx.cx_c.db&(DB1)))__atomic_store_n(JT(jt,adbreak),2,__ATOMIC_RELEASE);  // if not debug, promote the ATTN to BREAK for other threads to speed it up
  jsignal(EVATTN); z=parsez=0; goto noparse;  // if debug is not enabled, this will just be an error in the unparsed line
 }
 // we can stop before the sentence, or after it if it fails.  Stopping before is better because then we know we can restart safely
 // if there is a stop, enter debug suspension
 if(c&&dbstop(c,ci->source)){z=parsez=0; jsignal(EVSTOP); goto noparse;}
 // xdefn adds a stack entry for PARSE, needed to get anonymous operators right
 z=PARSERVALUE(parsez=parsea(queue,m));  // make sure we preserve ASGN flag in parsez
noparse: ;
 // If we hit a stop or ATTN, or if we hit an error (outside of try./catch., which turns debug off), enter debug suspension if enabled.  But if debug mode is off now, we must have just
 // executed 13!:0]0 or a suspension-ending command, and we should continue on outside of debug mode.  Error processing filled the current si line with the info from the parse
 if(!z&&c&&jt->uflags.us.cx.cx_c.db){DC t=jt->sitop->dclnk; t->dcj=jt->sitop->dcj=jt->jerr; parsez=jtdebugmux(jt); t->dcj=0;} //  d is PARSE type; set d->dcj=err#; d->dcn must remain # tokens
 // we have come out of suspension (if we went into it).  parsez has the value to return to execution.  It may have a value created by the user, or the value from the non-failing sentence.
 // Or, it may be 0, in which case the definition will fail.  In this case the error-code matters: the error will be printed when the failure reaches console level (in the master thread).  An error code of
 // EVDEBUGEND is used to force quiet failure all the way back to console level (usually in a task); an error code of 0, normal in a single-task system, will cause the failure to go back to
 // the first try./catch. or starting level and then print nothing.
 R parsez;
}

A jtdbunquote(J jt,A a,A w,A self,DC d){A t,z;B s;V*sv;
 sv=FAV(self); t=sv->fgh[0]; 
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
   if(!z&&jt->uflags.us.cx.cx_c.db){d->dcj=jt->jerr; movecurrtoktosi(jt); z=jtdebugmux(jt); if(self!=jt->sitop->dcf)self=jt->sitop->dcf;}
   if(!(d->dcnewlineno&&d->dcix!=-1))break;  // if 'run' specified (by jump not to -1), loop again.  Otherwise exit with result given
   // for 'run', the value of z gives the argument(s) to set; but if no args given, leave them unchanged
   if(AN(z)){w=C(AAV(z)[0]); a=AN(z)==2?C(AAV(z)[1]):0;}  // extract new args if there are any
  }
 }
 if(d->dcss)ssnext(d,d->dcss);  // if we step over/into on the last line of a function, we must stop on the next line of the caller
 R z;
}    /* function call, debug version */

F1(jtdbq){ASSERTMTV(w); R sc(JT(jt,dbuser)&~DBSUSCLEAR);}
     /* 13!:17 debug flag */

// x 13!:11 y set error number(s) in threads.  Error _1 is converted to EVDEBUGEND
F2(jtdberr2){
 ARGCHK2(a,w);
 ASSERT(AR(a)<2,EVRANK); ASSERT(AN(a)>0,EVDOMAIN); if(AT(a)!=INT)RZ(a=cvt(INT,a));  // verify #threads OK
 ASSERT(AR(w)<2,EVRANK); ASSERT(AN(w)==1||AN(w)==AN(a),EVDOMAIN); if(AT(w)!=INT)RZ(w=cvt(INT,w));  // verify #err#s OK
 DONOUNROLL(AN(a), ASSERT(BETWEENC(IAV(a)[i],0,JT(jt,nwthreads)),EVDOMAIN))  // verify threads exist
 JTT *jjbase=JTTHREAD0(jt); I winc=AN(a)!=1;  //  base of thread blocks  1 if we are not repeating w
 DONOUNROLL(AN(a), jjbase[IAV(a)[i]].jerr=IAV(w)[i*winc]>=0?IAV(w)[i*winc]:EVDEBUGEND;)  // install the requested error number
 R mtm;  // i. 0 0
}



// Suspension-ending commands.  These commands return a list of boxes flagged with the AFDEBUGRESULT flag.  The first box is always an integer atom and gives the type
// of exit (run, step, clear, etc).  Other boxes give values for the run and ret types.  EXCEPTION: 13!:0 returns i. 0 0 for compatibility, but still flagged as AFDEBUGRESULT
F1(jtdbc){UC k;
 ARGCHK1(w);
 if(AN(w)){
  RE(k=(UC)i0(w));
  ASSERT(!(k&~0x81),EVDOMAIN);
  ASSERT(!k||!jt->glock,EVDOMAIN);
 }
 if(AN(w)){
  // turn debugging on/off in all threads
  JTT *jjbase=JTTHREAD0(jt);  // base of thread blocks
  DONOUNROLL(MAXTASKS, __atomic_store_n(&jjbase[i].uflags.us.cx.cx_c.db,k&1,__ATOMIC_RELEASE);) JT(jt,dbuser)=k;
#if USECSTACK
  jt->cstackmin=jt->cstackinit-((CSTACKSIZE-CSTACKRESERVE)>>k);
#else
  jt->fdepn=NFDEP>>k;
#endif
  jt->fcalln=NFCALL/(k?2:1);
 }
 JT(jt,dbuser)|=DBSUSCLEAR;  // come out of suspension, whether value given or not
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
F1(jttcapdot1){I thno; RE(thno=i0(w)); ASSERT(BETWEENC(thno,0,JT(jt,nwthreads)),EVDOMAIN) A z; RZ(z=mkwris(jlink(sc(SUSTHREAD),sc(thno)))); AFLAGORLOCAL(z,AFDEBUGRESULT) R z;}

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


