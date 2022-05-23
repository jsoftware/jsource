/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/*
Input/Output

io.c implements J Engine interface with the J Front End

overview of jfe and je interface

jfe is a repl (read/execute/print/loop) that uses je as a server
 jfe reads line from kb
 jfe calls jdo() in je to execute line
  je calls jsto() to give string to jfe to print
  je calls jgets() to get kb input for debug or (1!:2[2) or (n : 0)
  je jdo() returns
 jfe loop

je can call jsto()  0 or more times in a single repl loop
je can call jgets() 0 or more times in a single repl loop

jfe routines (jconsole.c):
 load je shared library
 init je (get jt instance)
 set je callbacks (sm prefix indicates session manager)
  JT(jt,sminput)=input
  JT(jt,smoutput)=output

 input()        - get kb line for jdo() or JT(jt,sminput)() result
 
 output(string) - print s from JT(jt,smoutput)(type,string) 

je routines (io.c):
 jdo(line)

 jsto(type,string) - JT(jt,smoutput)() to give jfe output
             
 jgets() - JT(jt,sminput)() callback to get jfe kb input
            or
           advl() to get next script line


 jtwri()    - write to file - if file is 2, calls jsto()

 jtjfread() - read file - if file is 1, calls jgets()
 
 jtcolon0() - get defn lines with jgets()

*** je debug
set je debug stops at jsto() and jgets()
and continue through various inputs to see the flow
 
*** jfe repl (read/execute/print/loop)
 s=input()
 call jdo(s)           ---------> jdo() calls ddtokens(sentence)
   input()   (optional) <-------- if DD seen, get more lines if needed by calling JT(jt,sminput)
                                  jdo() calls immex(inpl(sentence))
                                   ... 
   output(s)           <--------  jsto(type,s) - JT(jt,smoutput(type,s)
   output returns      ---------> ...
 ...                   <--------- jdo returns with error code
 loop

*** repl with write to display (1!:2[2) - echo/smoutput
 ...
                                  1:2[2 calls jtwri()
                                  jsto(type,string)
   output(s)           <--------- JT(jt,smoutput(type,s)
   output returns      ---------> ...

*** repl with read from keyboard (1!:1[1)
 ...
                                  1:1[1 calls jtjfread()
   input()             <--------  JT(jt,sminput)()
   input returns line  -------->  ...

*** repl with debug suspension
 ...
                                     loop as long as suspended
                                       call jgets()
   input()            <---------       call JT(jt,sminput)()
   return line        --------->
   input()   (optional) <--------      if DD seen, get more lines if needed by calling JT(jt,sminput)
                                       call immex(inpl(line)
                                       loop
*** m : 0
similar to debug suspension except jgets() lines added  to defn.  m : 0 stops reading
after encountering ) on a line by itself.  If not 0 : 0, call ddtokens() after each line
to see if more lines need to be read to finish the DD; is so, call jgets() to get them

*** script load
linf() is called first to read in all lines.  It sets d->dcss in the SCRIPT entry to indicate that fact.  Thereafter
all calls to jgets() return lines from the file without calling JT(jt,sminput)().
jgets() calls advl() to advance through the lines, returns 0 for EOF.  Error is possible.

The lines are executed one by one.  Before each is executed, ddtokens() is called to see if more lines
are needed to finish a DD.

*** jwd (11!:x)
similar to debug suspension except output/input
 processed by gui sm

*** JHS nfe (native front end)
JT(jt,nfe) flag - JE does not use JT(jt,smoutput() and JT(jt,sminput)()
instead it calls J code that provides equivalent services
JHS routines are J socket code to talk with javascript browser page

DD NOTE: DD is not supported on special sentences: iep, dbssexec, dbtrap.  This is mostly because I don't understand how these work.

DD is supported on ". y, but only if the DD is fully contained in the string
A recursive JDo may use a DD, but only if it is fully contained in the string

*/


/* break attention attn interrupt crtl+c

user doc at: https://code.jsoftware.com/wiki/Guides/Interrupt

JE uses JATTN and JBREAK0 to stop execution of sentences with an error

JATTN signals an attention at the start of a line.  The system can resume execution after JATTN
JBREAK0 signals an interrupt in a long compute, 6!:3, socket select, ... Since the lowest sentence was interrupted resumption is not guaranteed

JATTN and JBREAK0 poll a breakbyte
 the value can be 0 continue, 1 signal in JATTN, or >1 signal in JBREAK0

NOTE: some JEs define a multi-byte breakarea.  ONLY THE FIRST BYTE can be changed by break processing.
Later bytes are used for internal break processing

the first word in jt (jt->adbreak) is a pointer to the breakbyte.  

a separate task or thread increments the breakbyte to request a break
 JE resets to 0 on new user input or on processing an error (including the one caused by ATTN/BREAK0)

jconsole is simplest case as ctrl+c is runs as a signal
 and gets breakbyte address from jt and increments it
 crtl+c logically runs a separate thread and can do this while JE is running

there is not always a ctrl+c mechanism to access breakbyte

in that case, the user runs verb setbreak to create a mapped breakfile with a
published name that uses the first byte of the mapped file as breakbyte

another task (J or whatever) gets the name of the breakfile and
 writes to the first byte (only!!) to signal JE

verb jtbreakfns gets the breakfile name and points jt->adbreak to it

study the definitions of setbreak and break in a J session and 9!:46/47 C code
??? where is setbreak?  do you mean breakfns?  where is break?

when J starts, the jt address of the breakbyte is an internal workarea jt->breakbytes
running jtbreakfns sets the address to the first byte of the newly mapped file

JHS has the additional complication of critical sections of J code
 JHS frontend input/output code is implemented in J
 a break in this code can can cause a crash or confusion
 JHS must be able to mask off break during the critical sections

 this is done by having TWO pointers to the breakbyte: one for requesting a break (jt->adbreak)
 and another for testing (jt->adbreakr).  Normally these are the same, but to disable break adbreakr is
 set to a pointer to a fixed 0 byte; at the end of the critical section adbreakr is set back to equal adbreak.

*/

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#else
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/mman.h>
#define _stdcall
#endif
#include <stdint.h>

#include "j.h"
#include "d.h"
#undef JT
#define JT(p,n) p->n  // used for references in JS, which most references in this module are
#define IJT(p,n) JT(JJTOJ(p),n)    // used in function that interface to internal functions and thus take a JJ
// given a pointer which might be a JST* or JTT*, set pointers to use for the shared and thread regions.
// If we were given JST*, keep it as shared & use master thread; if JTT*, keep it as thread & use shared region
#define SETJTJM(in,jstout,jttout) \
 JJ jttout; \
 if((I)in&(JTALIGNBDY-1)){jttout=(JJ)in; jstout=JJTOJ(in);   /* if jt is a thread pointer, use it and set jt to the shared */ \
 }else{jttout=MTHREAD(in);}  /* if jt is a shared pointer, use the master thread */



extern void dllquit(JJ);

// flags in jt indicate whether display is suppressed.  p is the prompt, s is the text.  suppression of s happens when it is created;
// here we control suppression of p; but we suppress all anyway
void jtwri(JS jt,I type,C*p,I m,C*s){FPREFIP(JS);C buf[1024],*t=OUTSEQ,*v=buf;I c,d,e,n;
 if(!((I)jtinplace&JTPRNOSTDOUT)){  // if the prompt is not suppressed...
  c=strlen(p);            /* prompt      */
  e=strlen(t);            /* end-of-line */
  n=sizeof(buf)-(c+e+1);  /* main text   */
  d=m>n?n-3:m;
  MC(v,p,c); v+=c;
  MC(v,s,d); v+=d; if(m>n){MC(v,"...",3L); v+=3;}
  MC(v,t,e); v+=e;   // join prompt/body/EOL
  *v=0;   // NUL termination
#ifdef ANDROID
  A z=jttocesu8(MTHREAD(jt),jtstr(MTHREAD(jt),strlen(buf),buf));  // calling internal jt... functions
  CAV(z)[AN(z)]=0;
  jsto(jt,type,CAV(z));
#else
  jsto(jt,type,buf);
#endif
 }
}

static void jtwrf(JS jt,I n,C*v,F f){C*u,*x;I j=0,m;
 while(n>j){
  u=j+v; 
  m=(x=memchr(u,CLF,n-j))?1+x-u:n-j; 
  fwrite(u,sizeof(C),m,f);
  j+=m;
}}

// input is string *s of length n
// result is A block for the string
// if !b (normal except for m : 0), look at the string:
//   if it contains XOFF (ctrl-D), exit J
//   if the vertical boxing character is Unicode, discard leading SP or | characters (which might have been error typeout)
static A jtinpl(JJ jt,B b,I n,C*s){C c;I k=0;
 if(n&&(c=s[n-1],CLF==c||CCR==c))--n;  // discard trailing [CR], CRLF, CRCR
#if _WIN32
 if(n&&(c=s[n-1],CCR==c))--n;
#endif
 ASSERT(!__atomic_load_n(IJT(jt,adbreakr),__ATOMIC_ACQUIRE),EVINPRUPT);
 if(!b){ /* 1==b means literal input */
  if(n&&COFF==s[n-1])joff(num(0));
  c=IJT(jt,bx)[9]; if((UC)c>127)DO(n, if(' '!=s[i]&&c!=s[i]){k=i; break;});  // discard stuff that looks like error typeout
 }
 R str(n-k,s+k);
}

// s->beginning of input, j is starting index of search, n is #characters
// result is index just past the end-of-line, which ends after CR, LF, or both
static I advl(I j,I n,C*s){B b;C c,*v;
 v=j+s; 
 DO(n-j, c=*v++; b=c==CCR; if(b||c==CLF)R j+1+i+(I )(b&&CLF==*v););
 R n;
}    /* advance one line on CR, CRLF, or LF */

void breakclose(JS jt);
#define DISABLEATTN JT(jt,adbreakr)=BREAK0;
#define ENABLEATTN JT(jt,adbreakr)=JT(jt,adbreak);
#define WITHATTNDISABLED(s) {DISABLEATTN s ENABLEATTN}

static C* nfeinput(JS jt,C* s){A y;
 WITHATTNDISABLED(y=jtexec1(MTHREAD(jt),jtcstr(MTHREAD(jt),s));)  // exec the sentence with break interrupts disabled
 if(!y){breakclose(jt);exit(2);} /* J input verb failed */
 jtwri(jt,MTYOLOG,"",strlen(CAV(y)),CAV(y));  // call to nfeinput() comes from a prompt or from jdo.  In either case we want to display the result.  Thus jt
 return CAV(y); /* don't combine with previous line! CAV runs (x) 2 times! */
}

// type NUL-terminated prompt string p, read 1 line, & return
// if *p is (C)1 (which comes from m : 0), the request is for unprocessed 'literal input'
// otherwise processed in inpl
// Lines may come from a script, in which case return 0 on EOF, but EVINPRUPT is still possible as an error
A jtjgets(JJ jt,C*p){A y;B b;C*v;I j,k,m,n;UC*s;
 __atomic_store_n(IJT(jt,adbreak),0,__ATOMIC_RELEASE);  // turn off any pending break
 if(b=1==*p)p=""; /* 1 means literal input; remember & clear prompt */
 DC d; for(d=jt->sitop; d&&d->dctype!=DCSCRIPT; d=d->dclnk);  // d-> last SCRIPT type, if any
 if(d&&d->dcss){   // enabled DCSCRIPT debug type - means we are reading from file (or string)  for 0!:x
  ++d->dcn; j=d->dcix; // increment line# and fetch current start index
  y=d->dcy; n=AN(y); s=UAV(y);
  if(!(j<n))R 0;  // return 0 for EOF
  d->dcj=k=j;  // k=start index
  d->dcix=j=advl(j,n,s);  // j=end+1 index
  m=j-k; if(m&&32>s[k+m-1])--m; if(m&&32>s[k+m-1])--m;  // m is length; discard trailing control characters (usually CRLF, but not necessarily) ?not needed: done in inpl
  jtwri((JS)((I)JJTOJ(jt)+d->dcpflags),MTYOLOG,p,m,k+s);  // log the input, but only if we wanted to echo the input
  R inpl(b,m,k+s);  // process & return the line
 }

 /* J calls for input in 3 cases:
    debug suspension for normal input
    n : 0 input lines up to terminating )
    1!:1[1 read from keyboard */
 // if we are already prompting, a second prompt would be unrecoverable & we fail this request
 ASSERT(jt->recurstate<RECSTATEPROMPT,EVCTRL)
 showerr();  // if there is an error at this point, display it (shouldn't happen)   use jt to force typeout
 // read from the front end. This is either through the nfe path or via the callback to the FE
 if(IJT(jt,nfe)){
  // Native Front End
  jt->recurstate=RECSTATEPROMPT;  // advance to PROMPT state
  v=nfeinput(JJTOJ(jt),*p?"input_jfe_'      '":"input_jfe_''");   // use jt so always emit prompt
 }else{
  ASSERT(IJT(jt,sminput),EVBREAK); 
  jt->recurstate=RECSTATEPROMPT;  // advance to PROMPT state
  v=((inputtype)(IJT(jt,sminput)))(JJTOJ(jt),p);
 }
 jt->recurstate=RECSTATEBUSY;  // prompt complete, go back to normal running state
 R inpl(b,(I)strlen(v),v);  // return A block for string
}


#if SYS&SYS_UNIX
void breakclose(JS jt)
{
 if(JT(jt,adbreak)==(C*)&JT(jt,breakbytes)) return;  // if no mapped file has been created, exit fast
 *&JT(jt,breakbytes)=*(US*)(JT(jt,adbreak));  // copy over any pending break request, plus other breakdata
 munmap(JT(jt,adbreak),1);
 if(JT(jt,adbreakr)==JT(jt,adbreak))JT(jt,adbreakr)=(C*)&JT(jt,breakbytes);  // move to look at the new data - but not if attn disabled
 JT(jt,adbreak)=(C*)&JT(jt,breakbytes);  // move request pointer in any case
 close((intptr_t)JT(jt,breakfh));
 JT(jt,breakfh)=0;
 unlink(JT(jt,breakfn));
 *JT(jt,breakfn)=0;
}
#else
void breakclose(JS jt)
{
 if(JT(jt,adbreak)==(C*)&JT(jt,breakbytes)) return;  // if no mapped file has been created, exit fast
 *&JT(jt,breakbytes)=*(US*)(JT(jt,adbreak));  // copy over any pending break request, plus other breakdata
 UnmapViewOfFile(JT(jt,adbreak));
 if(JT(jt,adbreakr)==JT(jt,adbreak))JT(jt,adbreakr)=(C*)&JT(jt,breakbytes);  // move to look at the new data - but not if attn disabled
 JT(jt,adbreak)=(C*)&JT(jt,breakbytes);  // move attn-request pointer in any case
 CloseHandle(JT(jt,breakmh));
 JT(jt,breakmh)=0;
 CloseHandle(JT(jt,breakfh));
 JT(jt,breakfh)=0;
#if SY_WINCE
 DeleteFile(tounibuf(JT(jt,breakfn)));
#else
 WCHAR wpath[NPATH];
 MultiByteToWideChar(CP_UTF8,0,JT(jt,breakfn),1+(int)strlen(JT(jt,breakfn)),wpath,NPATH);
 DeleteFileW(wpath);
#endif
 *JT(jt,breakfn)=0;
}
#endif

// 2!:55 retcode
F1(jtjoff){I x;
 ARGCHK1(w);
 x=i0(w);
 jt->jerr=EVEXIT; jt->etxn=0; // clear old errors, replacing with the higher-priority EVEXIT
 if(IJT(jt,sesm))jsto(JJTOJ(jt), MTYOEXIT,(C*)x); else JFree(JJTOJ(jt));
 // let front-end handle exit.
// exit((int)x);
 R 0;
}

// wrapper to raise the execct of the starting locale while an immex is running
// If the global locale changes during execution, we must have called cocurrent or 18!:4 directly.  If cocurrent, there will be a
// POPFIRST on the stack (which is otherwise empty).  If there is a POPFIRST we need to decrement the current global locale.
// in this routine jt is a thread pointer and jjt is the shared pointer
static void jtimmexexecct(JJ jt, A x){
 I4 savcallstack = jt->callstacknext;   // starting callstack
 A startloc=jt->global;  // point to current global locale
 if(likely(startloc!=0))INCREXECCT(startloc);  // raise usecount of current locale to protect it while running
 jtimmex(jt,x);   // run the sentence
 if(likely(startloc!=0))DECREXECCT(startloc);  // remove protection from executed locale.  This may result in its deletion
 jtstackepilog(jt, savcallstack); // handle any remnant on the call stack
}


// if there is an immex sentence, fetch it, protect it from deletion, run it, and undo the protection
// in this routine jt is a thread pointer and jjt is the shared pointer
static void runiep(JS jjt,JJ jt,A *old,I4 savcallstack){
 while(1){
 // if there is an immex phrase, protect it during its execution
  A iep=0; if(jt->iepdo){READLOCK(jjt->felock) if((iep=jjt->iep)!=0)ra(iep); READUNLOCK(jjt->felock)}
  if(iep==0)break;
  jt->iepdo=0; jtimmexexecct(jt,iep); fa(iep) if(savcallstack==0)CALLSTACKRESET(jt) MODESRESET(jt) jt->jerr=0; jttpop(jt,old);
 }
}

static I jdo(JS jt, C* lp){I e;A x;JJ jm=MTHREAD(jt);  // get address of thread struct we are using (the master thread)
 jm->jerr=0; jm->etxn=0; /* clear old errors */
 // The named-execution stack contains information on resetting the current locale.  If the first named execution deletes the locale it is running in,
 // that deletion is deferred until the locale is no longer running, which is never detected because there is no earlier named execution to clean up.
 // To prevent the stack from growing indefinitely, we reset it here.  We reset the callstack only if it was 0, so that a recursive immex will have its deletes handled by
 // the resumption of the name that was interrupted.
 I4 savcallstack = jm->callstacknext;
 if(JT(jt,capture))JT(jt,capture)[0]=0; // clear capture buffer
 A *old=jm->tnextpushp;
 __atomic_store_n(JT(jt,adbreak),0,__ATOMIC_RELEASE);  // remove pending ATTN before executing the sentence
 x=jtinpl(jm,0,(I)strlen(lp),lp);
 I wasidle=jtsettaskrunning(jm);  // We must mark the master thread as 'running' so that a system lock started in another task will include the master thread in the sync.
      // but if the master task is already running, this is a recursion, and just stay in running state
 // if there is an immex latent expression (9!:27), execute it before prompting
 // All these immexes run with result-display enabled (jt flags=0)
 // Run any enabled immex sentences both before & after the line being executed.  I don't understand why we do it before, but it can't hurt since there won't be any.
 // BUT: don't do it if the call is recursive.  The user might have set the iep before a prompt, and won't expect it to be executed asynchronously
 if(likely(jm->recurstate<RECSTATEPROMPT))runiep(jt,jm,old,savcallstack);
 // Check for DDs in the input sentence.  If there is one, call jgets() to finish it.  Result is enqueue()d sentence.  If recursive, don't allow call to jgets()
 x=jtddtokens(jm,x,(((jm->recurstate&RECSTATEPROMPT)<<(2-1)))+1+(AN(jm->locsyms)>SYMLINFOSIZE)); if(!jm->jerr)jtimmexexecct(jm,x);  // allow reads from jgets() if not recursive; return enqueue() result
 e=jm->jerr; if(savcallstack==0)CALLSTACKRESET(jm) MODESRESET(jm) jm->jerr=0;
 if(likely(jm->recurstate<RECSTATEPROMPT))runiep(jt,jm,old,savcallstack);
 if(likely(wasidle))jtclrtaskrunning(jm);  //  when we finally return to FE, clear running state in case other tasks are running and need system lock - but not if recursion
 jtshowerr(jm);   // jt flags=0 to force typeout of iep errors
 jtspfree(jm);
 jttpop(jm,old);
 R e;
}

#define SZINT             ((I)sizeof(int))

C* getlocale(JS jt){JJ jm=MTHREAD(jt); A y=jtlocname(jm,mtv); y=AAV(y)[0]; R CAV(jtstr0(jm,y));}   // return current locale of master thread

// Front-ends can call any functions exposed by JE, but the callback function for 11!:0 only calls jga to allocate a new literal array for returning result.
// A front-end knows nothing how J memory pool works and it won't try to free or pop memory itself. This was just a design decision. eg,
// jqt front-end uses another alternative, it stores result in a static variable and pass its address back to JE so that no jga is needed.
//
// The program flow begins with a J script execution of a sentence containing 11!:0 which is implemented by jtwd.
// but jtwd itself knows nothing about the actual implementation, it just calls the callback function. and then
// after callback function completed, the callback allocate an literal array inside JE memory pool and fill up the content of array.
// The array allocated by jga is post processed inside jtwd and becomes the return value of jtwd, ie, the result of calling 11!:0 in J script.
//
// This may be confusing because the actual callback functions do not appear anywhere inside JE source.
DF1(jtwd){A z=0;C*p=0;D*pd;I e,*pi,t;V*sv;
  FPREFIP(JJ);
  F1RANK(1,jtwd,self);
  ARGCHK1(w);
  ASSERT(2>AR(w),EVRANK);
  sv=VAV(self);
  t=i0(sv->fgh[1]);  // the n arg from the original 11!:n
  if(BETWEENO(t,2000,3000) && AN(w) && !(LIT+C2T+C4T+INT&AT(w))) {  // 2000<=t<3000
    switch(AT(w)) {
    case B01:
      RZ(w=vi(w));
      break;
    case FL:
      pd=DAV(w);
      {A wsav=w; GATV0(w,INT,AN(wsav),AR(wsav)); }
      pi=AV(w);
      DQ(AN(w),*pi++=(I)(jround(*pd++)););
      break;
    default:
      ASSERT(0,EVDOMAIN);
    }
  }
  RZ(w=jtmemu(jtinplace,w));
  // Now call the host and get the response
  // Calling the Host takes us out of BUSY state, back to IDLE.  (If for some reason we are prompting, that is not affected)
  // That is, while we are waiting for the reply we are back to interruptible state.  If the interrupt issues another wd,
  // that too is interruptible, for as many levels as needed.
// t is 11!:t and w is wd argument
// smoption:
//   1=pass current locale
//   2=result not allocated by jga
//   4=use smpoll to get last result
//   8=multithreaded
// smdowd = function pointer to Jwd, if NULL nothing will be called
  ASSERT(IJT(jt,smdowd),EVDOMAIN);
  jt->recurstate&=~RECSTATEBUSY;  // back to IDLE/PROMPT state
  if(SMOPTLOCALE&IJT(jt,smoption)) {
// pass locale as parameter of callback
    e=((dowdtype2)(IJT(jt,smdowd)))(JJTOJ(jt), (int)t, w, &z, getlocale(JJTOJ(jt)));
  } else {
// front-end will call getlocale() inside callback
    e=((dowdtype)(IJT(jt,smdowd)))(JJTOJ(jt), (int)t, w, &z);
  }
  jt->recurstate|=RECSTATEBUSY;  // wd complete, go back to normal running state, BUSY normally or RECUR if a prompt is pending
  if(!e) R mtm;   // e==0 is MTM
  ASSERT(e<=0,e); // e>=0 is EVDOMAIN etc
  if(e==-1){      // e==-1 is lit
  if(SMOPTPOLL&IJT(jt,smoption)){jt->recurstate=RECSTATEPROMPT; z=(A)((polltype)(IJT(jt,smpoll)))(JJTOJ(jt), (int)t, (int)e); jt->recurstate=RECSTATEBUSY; RZ(z);} // alternate way to get result aftercallback, but not yet used in any front-end
  if(SMOPTNOJGA&IJT(jt,smoption)) z=ca(z);  // front-end promised not to use Jga to allocate memory, but not yet used in any front-end
  RETF(z);
  } else if(e==-2){      // e==-2 is lit pairs
// callback result z is a rank-1 literal array 
// literal array will be cut into rank-1 box array here using  <;._2 
// and then reshape into rank-2  ((n%2),2)$
    A x=z; RZ(df1(z,x,cut(ds(CBOX),num(-2))));
    RETF(reshape(v2(AN(z)>>1,2L),z));
  } else {RETF(z);}
}

static char breaknone=0;

// Init anything the sessions manager needs in the jt for a new instance
B jtsesminit(JS jjt, I nthreads){R 1;}

// Main entry point to run the sentence in *lp in the master thread, or in the thread given if jt is not a JS pointer
int _stdcall JDo(JS jt, C* lp){int r; UI savcstackmin, savcstackinit, savqtstackinit;
 SETJTJM(jt,jt,jm)
  // Normal output.  Call the output routine
 if(unlikely(jm->recurstate>RECSTATEIDLE)){
  // recursive call.  If we are busy or already recurring, this would be an uncontrolled recursion.  Fail that
  savcstackmin=jm->cstackmin, savcstackinit=jm->cstackinit, savqtstackinit=JT(jt,qtstackinit);  // save stack pointers over recursion, in case the host resets them
  ASSERTTHREAD(!(jm->recurstate&(RECSTATEBUSY&RECSTATERECUR)),EVCTRL)  // fail if BUSY or RECUR
  // we know that in PROMPT state there is no volatile C state about, such as zombie status
 }
#if USECSTACK
 if(JT(jt,cstacktype)==2){
  // multithreaded FE.  Reinit the stack limit on every call, as long as cstackmin is nonzero
  JT(jt,qtstackinit) = (uintptr_t)&jt;
  if(jm->cstackmin)jm->cstackmin=(jm->cstackinit=JT(jt,qtstackinit))-(CSTACKSIZE-CSTACKRESERVE);
 }
#endif
 ++jm->recurstate;  // advance, to BUSY or RECUR state
 r=(int)jdo(jt,lp);
 if(unlikely(--jm->recurstate>RECSTATEIDLE)){  // return to IDLE or PROMPT state
  // return from recursive call.  Restore stackpointers
  jm->cstackmin=savcstackmin, jm->cstackinit=savcstackinit, JT(jt,qtstackinit)=savqtstackinit;  // restore stack pointers after recursion
 }
 while(JT(jt,nfe)){  // nfe normally loops here forever
  A *old=jm->tnextpushp; r=(int)jdo(jt,nfeinput(jt,"input_jfe_'   '")); jttpop(jm,old);  // use jt to force output in nfeinput
 }
 R r;
} 

C* _stdcall JGetR(JS jt){
 SETJTJM(jt,jt,jm)
 R JT(jt,capture)?JT(jt,capture):(C*)"";
}

/* socket protocol CMDGET name */
// Return the binary rep of the given name
// We MALLOC a return block so that we don't lose J memory.  We reuse the block for successive calls, or maybe free it,
// so the user must save it before re-calling.  This is a kludge - the user should pass in the address/length of the block to use - but
// it preserves the interface
// If the pointer to the name is NULL we just free the block
A _stdcall JGetA(JS jt, I n, C* name){A x,z=0;
 SETJTJM(jt,jt,jm)
 if(name==0){if(JT(jt,iomalloc)){FREE(JT(jt,iomalloc)); jm->malloctotal -= JT(jt,iomalloclen); JT(jt,iomalloc)=0; JT(jt,iomalloclen)=0;} R 0;}
 jm->jerr=0;
 A *old=jm->tnextpushp;
 if(!(x=jtsymbrdlock(jm,jtnfs(jm,n,name)))){jtjsignal(jm,EVILNAME);  // look up the name, error if invalid
 }else if(FUNC&AT(x)){jtjsignal(jm,EVDOMAIN);   // verify the value is not adv/verb/conj
 }else{
  // name is OK; get the binary rep
  if(z=jtbinrep1(jm,x)){
   // bin rep was found.  Transfer it to MALLOC memory.  It is a LIT array
   // we transfer the whole thing, header and all.  Fortunately it is relocatable
   I replen = &CAV(z)[AN(z)] - (C*)z;  // length from start of z to end+1 of data
   // See if we can reuse the block.  We can, if it is big enough.  But if it is twice as big as the return value, don't.  Watch for overflow!
   if(JT(jt,iomalloc) && (JT(jt,iomalloclen) < replen || (JT(jt,iomalloclen)>>1) > replen)){FREE(JT(jt,iomalloc)); jm->malloctotal -= JT(jt,iomalloclen); JT(jt,iomalloc)=0;}  // free block if not reusable
   if(!JT(jt,iomalloc)){if(JT(jt,iomalloc)=MALLOC(replen)){jm->malloctotal += replen; JT(jt,iomalloclen) = replen;}}  // allocate block if needed, and account for its space
   if(JT(jt,iomalloc)){memcpy(JT(jt,iomalloc),z,replen); z=(A)JT(jt,iomalloc);  // normal case: block exists, move the data, set the return address to the malloc block
   }else{JT(jt,iomalloclen)=0; z=0;}   // if unable to allocate, return error and indicate block is empty
  }
 }
 // z has the result, which is in MALLOC memory if it exists.  Free any J memory we used
 jttpop(jm,old);
 R z;   // return the allocated (or reused) area
}

/* socket protocol CMDSET */
I _stdcall JSetA(JS jt,I n,C* name,I dlen,C* d){
 SETJTJM(jt,jt,jm)
 jm->jerr=0;
 if(!jtvnm(jm,n,name)){jtjsignal(jm,EVILNAME); R EVILNAME;}
 A *old=jm->tnextpushp;
 jtsymbisdel(jm,jtnfs(jm,n,name),jtunbin(jm,jtstr(jm,dlen,d)),jm->global);
 jttpop(jm,old);
 R jm->jerr;
}

/* set jclient callbacks */
/*
void* callbacks[] = {Joutput, Jwd, Jinput, unused, smoptions};
typedef void  (_stdcall * outputtype)(J,int,C*);
typedef int   (_stdcall * dowdtype)  (J,int, A, A*);
typedef int   (_stdcall * dowdtype2) (J,int, A, A*, C*);  // pass current locale
typedef C* (_stdcall * inputtype) (J,C*);
typedef C* (_stdcall * polltype) (J,int,int);
*/

void _stdcall JSM(JS jt, void* callbacks[])
{
 SETJTJM(jt,jt,jm)
 JT(jt,smoutput) = (outputtype)callbacks[0];  // callback function for output to J session
// output type
// #define MTYOFM  1 /* formatted result array output */
// #define MTYOER  2 /* error output */
// #define MTYOLOG  3 /* output log */
// #define MTYOSYS  4 /* system assertion failure */
// #define MTYOEXIT 5 /* exit */
// #define MTYOFILE 6 /* output 1!:2[2 */
 JT(jt,smdowd) = callbacks[1];                // callback function for 11!:x
 JT(jt,sminput) = (inputtype)callbacks[2];    // callback function for input from J session keyboard
 JT(jt,smpoll) = (polltype)callbacks[3];      // for Qt, the (optional) end-of-stack pointer; otherwise the smpoll flag
 JT(jt,sm) = 0xff & (I)callbacks[4];          // lowest byte, sm : session manager type
// smoptions
// #define SMWIN    0  /* j.exe    Jwdw (Windows) front end */
// #define SMJAVA   2  /* j.jar    Jwdp (Java) front end */
// #define SMCON    3  /* jconsole */
// #define SMQT     4  /* jqt */
// #define SMJA     5  /* jandroid */
 JT(jt,smoption) = ((~0xff) & (UI)callbacks[4]) >> 8;  // upper bytes, smoption : 
// #define SMOPTLOCALE 1  /* pass current locale */
// #define SMOPTNOJGA  2  /* result not allocated by jga */
// #define SMOPTPOLL   4  /* use smpoll to get last result */
// #define SMOPTMTH    8  /* multithreaded */
 if(JT(jt,sm)==SMJAVA) JT(jt,smoption) |= SMOPTMTH;  /* assume java is multithreaded */
 // set the stack discipline to be used for this session.  This persists until changed by another call
 if(SMOPTMTH&JT(jt,smoption)){
  // Session in a multithreaded FE.  EVERY call to JDo() will set a new stackmin for JE to use (unless stack checking is disabled).  The stack is assumed to grow down, with
  // jt allocated within the same stack.  There must be CSTACKSIZE-CSTACKRESERVE bytes in the stack before jt
  JT(jt,cstacktype) = 2;
  JT(jt,qtstackinit) = (uintptr_t)&jt;  // jt itself is used as an end-of-stack pointer
  if(jm->cstackmin)jm->cstackmin=(jm->cstackinit=JT(jt,qtstackinit))-(CSTACKSIZE-CSTACKRESERVE);
 }else if(SMQT==JT(jt,sm)){
  // Qt session.  End-of-stack should be given in callbacks[3].  There must be CSTACKSIZE-CSTACKRESERVE bytes in the stack
  JT(jt,cstacktype) = 1;
  JT(jt,qtstackinit) = (uintptr_t)callbacks[3];
  JT(jt,smpoll) = 0;
  JT(jt,smoption) = (~SMOPTPOLL) & JT(jt,smoption);  /* smpoll not used */
  // If the user is giving us a better view of the stack through JT(jt,qtstackinit), use it.  We get just the top-of-stack
  // address so we have to guess about the bottom, using our view of the minimum possible stack we have.
  // if cstackmin is 0, the user has turned off stack checking and we honor that decision
  if(JT(jt,qtstackinit)&&jm->cstackmin)jm->cstackmin=(jm->cstackinit=JT(jt,qtstackinit))-(CSTACKSIZE-CSTACKRESERVE);
 }
 // default: cstacktype=0, cstackmin set at initialization from the C stackpointer at that time
}

/* set jclient callbacks from values - easier for nodejs */
void _stdcall JSMX(JS jt, void* out, void* wd, void* in, void* poll, I opts)
{
 SETJTJM(jt,jt,jm)
 JT(jt,smoutput) = (outputtype)out;
 JT(jt,smdowd) = wd;
 JT(jt,sminput) = (inputtype)in;
 JT(jt,smpoll) = (polltype)poll;
 JT(jt,sm) = 0xff & opts;
 JT(jt,qtstackinit) = (SMQT==JT(jt,sm)) ? (uintptr_t)poll : 0;  // poll arg means 'end-of-stack pointer' for Qt
 JT(jt,smoption) = ((~0xff) & (UI)opts) >> 8;
 if(JT(jt,sm)==SMJAVA) JT(jt,smoption) |= SMOPTMTH;  /* assume java is multithreaded */
 // stack discipline, see above
 if(SMOPTMTH&JT(jt,smoption)){
  JT(jt,cstacktype) = 2;
  JT(jt,qtstackinit) = (uintptr_t)&jt;
  if(jm->cstackmin)jm->cstackmin=(jm->cstackinit=JT(jt,qtstackinit))-(CSTACKSIZE-CSTACKRESERVE);
 }else if(SMQT==JT(jt,sm)){
  JT(jt,cstacktype) = 1;
  JT(jt,qtstackinit) = (uintptr_t)poll;
  JT(jt,smpoll) = 0;
  JT(jt,smoption) = (~SMOPTPOLL) & JT(jt,smoption);  /* smpoll not used */
  if(JT(jt,qtstackinit)&&jm->cstackinit)jm->cstackmin=(jm->cstackinit=JT(jt,qtstackinit))-(CSTACKSIZE-CSTACKRESERVE);
 }
}

// return pointer to string name of current locale, or 0 if error
C* _stdcall JGetLocale(JS jt){
 SETJTJM(jt,jt,jm)
 A *old=jm->tnextpushp;  // set free-back-to point
 if(JT(jt,iomalloc)){FREE(JT(jt,iomalloc)); jm->malloctotal -= JT(jt,iomalloclen); JT(jt,iomalloc)=0; JT(jt,iomalloclen)=0;}  // free old block if any
 C* z=getlocale(jt);  // get address of string to return
 if(JT(jt,iomalloc)=MALLOC(1+strlen(z))){jm->malloctotal += 1+strlen(z); JT(jt,iomalloclen) = 1+strlen(z); strcpy(JT(jt,iomalloc),z); }  // allocate & copy, and account for its space
 jttpop(jm,old);  // free allocated blocks
 R JT(jt,iomalloc);  // return pointer to string
}

A _stdcall Jga(JS jjt, I t, I n, I r, I*s){A z;
 SETJTJM(jjt,jjt,jt)  // the name 'jt' is used by ga() for the shared pointer
 GA(z,t,n,r,s);
 ACINIT(z,ACUC1)  // set nonrecursive usecount so that parser won't free the block prematurely.  This gives the usecount as if the block were 'assigned' by this call
 return z;
}

void oleoutput(JS jt, I n, char* s); /* SY_WIN32 only */

#define capturesize 80000

/* jsto - display output in output window */
// type is mtyo of string, s->null-terminated string
void jsto(JS jt,I type,C*s){C e;I ex;
 if(JT(jt,nfe))
 {JJ jm=MTHREAD(jt);  // get address of thread struct we are using
  // here for Native Front End state, toggled by 15!:16
  // we execute the sentence:  type output_jfe_ s    in the master thread
  fauxblockINT(fauxtok,3,1); A tok; fauxBOXNR(tok,fauxtok,3,1);  // allocate 3-word sentence on stack, rank 1
  DISABLEATTN
  AAV1(tok)[0]=PTROP(num(type),|,QCNOUN); AAV1(tok)[1]=PTROP(jtnfs(jm,11,"output_jfe_"),|,QCISLKPNAME); AAV1(tok)[2]=PTROP(jtcstr(jm,s),|,QCNOUN);  // the sentence to execute, tokenized and with flag-types installed.  Better not fail!
  e=jm->jerr; ex=jm->etxn;   // save error state before running the output sentence
  jm->jerr=0; jm->etxn=0;
  jtparse(jm,tok);  // run sentence, with no interrupts.  ignore errors.
  jm->jerr=e; jm->etxn=ex; // restore
  ENABLEATTN
 }else{
  // Normal output.  Call the output routine
  if(JT(jt,smoutput)){((outputtype)(JT(jt,smoutput)))(jt,(int)type,s);R;} // JFE output
#if SY_WIN32 && !SY_WINCE && defined(OLECOM)
  if(JT(jt,oleop) && (type & MTYOFM)){oleoutput(jt,strlen(s),s);R;} // ole output
#endif
  // lazy - malloc failure will crash and should alloc larger when full
  if(!JT(jt,capture)){JT(jt,capture)=MALLOC(capturesize);JT(jt,capture)[0]=0;}
  if(capturesize>2+strlen(JT(jt,capture))+strlen(s))
   strcat(JT(jt,capture),s);
  else
   strcpy(JT(jt,capture),"too much output ...\n");
 }
 R;
}

#if SYS&SYS_UNIX

C dll_initialized= 0; // dll init sets to 1

// dll init on load - eqivalent to windows DLLMAIN DLL_ATTACH_PROOCESS
__attribute__((constructor)) static void Initializer(int argc, char** argv, char** envp)
{
 // Initialize J globals.  This is done only once.  Many of the globals are in static memory, initialized
 // by the compiler; some must be initialized a run-time in static memory; some must be allocated into A blocks
 // pointed to by static names.  Because of the A blocks, we have to perform a skeletal initialization of jt,
 // just enough to do ga().  The rest of jt is never used
 JS jtnobdy=malloc(sizeof(JST)+JTALIGNBDY-1);
 if(!jtnobdy) R;
 JS jt = (JS)(((I)jtnobdy+JTALIGNBDY-1)&-JTALIGNBDY);  // force to SDRAM page boundary
 mvc(sizeof(JST),jt,1,MEMSET00);
 if(!jtglobinit(jt)){free(jtnobdy); R;}
 dll_initialized= 1; JT(jt,heap)=(void *)jtnobdy;  // save allo address for later free
 // The g_jt heap MUST NOT be freed, because it holds the blocks pointed to by initialized globals.
 // g_jt itself, a JST struct, is not used.  Perhaps it could be freed, as long as the rest of the heap remains.
}

 // Init for a new J instance.  Globals have already been initialized.
 // Create a new jt, which will be the one we use for the entirety of the instance.
JS _stdcall JInit(void){
 if(!dll_initialized) R 0; // constructor failed
 JS jtnobdy;
 RZ(jtnobdy=malloc(sizeof(JST)+JTALIGNBDY-1));
 JS jt = (JS)(((I)jtnobdy+JTALIGNBDY-1)&-JTALIGNBDY);  // force to SDRAM page boundary
 mvc(sizeof(JST),jt,1,MEMSET00);
 // Initialize all the info for the shared region and the master thread
 if(!jtjinit2(jt,0,0)){free(jtnobdy); R 0;};
 JT(jt,heap)=(void *)jtnobdy;  // save allo address for later free
 R jt;  // R (JS)MTHREAD(jt);
}

// clean up at the end of a J instance
int _stdcall JFree(JS jt){
  if(!jt) R 0;
  SETJTJM(jt,jt,jm)
  breakclose(jt);
  jm->jerr=0; jm->etxn=0; /* clear old errors */
  dllquit(jm);  // clean up call dll
#if PYXES
  aligned_free(JT(jt,jobqueue));
#endif
  free(JT(jt,heap));  // free the initial allocation
  R 0;
}
#endif

F1(jtbreakfnq){
 ASSERTMTV(w);
 R cstr(IJT(jt,breakfn));
}

// w is a filename; use it as the breakfile
F1(jtbreakfns){A z;I *fh,*mh=0; void* ad;
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(!AN(w)||AT(w)&LIT,EVDOMAIN);
 ASSERT(AN(w)<NPATH,EVDOMAIN);
 w=str0(w);   // add NUL termination
 if(!strcmp(IJT(jt,breakfn),CAV(w))) R mtm;   // return fast if no name change
 breakclose(JJTOJ(jt));
#if SYS&SYS_UNIX
 fh=(I*)(I)open(CAV(w),O_RDWR);
 ASSERT(-1!=(I)fh,EVDOMAIN);
 ad=mmap(0,1,PROT_READ|PROT_WRITE,MAP_SHARED,(I)fh,0);
 if(0==ad){close((intptr_t)fh); ASSERT(0,EVDOMAIN);}
#else
 RZ(z=toutf16x(w));
 fh=CreateFileW(USAV(z),GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0);
 ASSERT(INVALID_HANDLE_VALUE!=fh,EVDOMAIN);
 mh=CreateFileMapping(fh,0,PAGE_READWRITE,0,1,0);
 if(0==mh){CloseHandle(fh); ASSERT(0,EVDOMAIN);}
 ad=MapViewOfFile(mh,FILE_MAP_WRITE,0,0,0);
 if(0==ad){CloseHandle(mh); CloseHandle(fh); ASSERT(0,EVDOMAIN);}
#endif
 strcpy(IJT(jt,breakfn),CAV(w));
 IJT(jt,breakfh)=fh;
 IJT(jt,breakmh)=mh;
 *(US*)ad==*(US*)IJT(jt,adbreak);  // copy breakstatus from current setting
 if(IJT(jt,adbreakr)==IJT(jt,adbreak))IJT(jt,adbreakr)=ad;  // move attn-read pointer, unless interrupts disabled
 IJT(jt,adbreak)=ad;  // start using the mapped area
 R mtm;
}

int valid(C* psrc, C* psnk)
{
 NOUNROLL while(*psrc == ' ') ++psrc;
 if(!isalpha(*psrc)) return EVILNAME;
 NOUNROLL while(isalnum(*psrc) || *psrc=='_') *psnk++ = *psrc++;
 NOUNROLL while(*psrc == ' ') ++psrc;
 if(*psrc) return EVILNAME;
 *psnk = 0;
 return 0;  
}

int _stdcall JGetM(JS jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata)
{
 SETJTJM(jt,jt,jm)
 A a; char gn[256]; int z;
 A *old=jm->tnextpushp;
 if(strlen(name) >= sizeof(gn)){jtjsignal(jm,z=EVILNAME);
 }else if(valid(name, gn)){jtjsignal(jm,z=EVILNAME);
 }else if(!(a=jtsymbrdlock(jm,jtnfs(jm,strlen(gn),gn)))){jtjsignal(jm,z=EVDOMAIN);
 }else if(FUNC&AT(a)){jtjsignal(jm,z=EVDOMAIN);
 }else{
  *jtype = AT(a);
  *jrank = AR(a);
  *jshape = (I)AS(a);
  *jdata = (I)AV(a);
  z=0;  // good return
 }
 jttpop(jm,old);
 return z;
}

static int setterm(JS jtt, C* name, I* jtype, I* jrank, I* jshape, I* jdata)
{JJ jt=MTHREAD(jtt);   // use master thread
 A a;
 I k=1,i,n;
 char gn[256];

 switch(CTTZNOFLAG(*jtype))
 {
 case LITX:
 case B01X:
  n = sizeof(char);
  break;

 case C2TX:
  n = sizeof(unsigned short);
  break;

 case C4TX:
  n = sizeof(unsigned int);
  break;

 case INTX:
 case SBTX:
  n = sizeof(I);
  break;
  
 case FLX:
  n = sizeof(double);
  break;
  
 case CMPXX:
  n = 2 * sizeof(double);
  break;
  
 default:
  return EVDOMAIN; 
 }

 // validate name
 if(strlen(name) >= sizeof(gn)) return EVILNAME;
 if(valid(name, gn)) return EVILNAME; 
 for(i=0; i<*jrank; ++i) k *= ((I*)(*jshape))[i];
 GAE(a,*jtype, k, *jrank, (I*)*jshape,R EVWSFULL);
 MC(AV(a), (void*)*jdata, n*k);
 jtjset(jt,gn, a);
 return jt->jerr;
}

int _stdcall JSetM(JS jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata)
{
 SETJTJM(jt,jt,jm)
 int er;

 A *old=jm->tnextpushp;
 er = setterm(jt, name, jtype, jrank, jshape, jdata);
 jttpop(jm,old);
 return er;
}

#define EDCBUSY -1
#define EDCEXE -2

C* esub(JS jt, I ec)
{
 if(!ec) return "";
 if(ec == EDCBUSY) return "busy with previous input";
 if(ec == EDCEXE) return "not supported in EXE server";
 if(ec > NEVM || ec < 0) return "unknown error";
 return CAV(AAV(JT(jt,evm))[ec]);
}

int _stdcall JErrorTextM(JS jt, I ec, I* p)
{
 SETJTJM(jt,jt,jm)
 *p = (I)esub(jt, ec);
 return 0;
}

#if 0
int enabledebug=0;
F1(jttest1){
 ARGCHK1(w);
 if((AT(w)&B01+INT)&&AN(w)){
  enabledebug=i0(w);
 }
 R sc(1);
}
#endif

