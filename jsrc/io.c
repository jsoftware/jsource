/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Input/Output                                                            */

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

 jinterrupt() - signal interrupt in all threads.  Safe to call from signal handlers, and even from non-j threads, but shouldn't be called concurrently

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
A reentrant JDo may use a DD, but only if it is fully contained in the string

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

/*
Example J script using window driver (wd).

f=: 3 :0
NB. p is path to non-existent file.
p=: '~/j9.5-user/temp/42.ijs'
wd 'sm open tab *' , p
)
f ''

Flow diagram.

Qt                                        JE
JDo, lp="f ''"
                                          JDo recurstate=0
                                          jdo recurstate=1
                                          Calls jtsettaskrunning.
                                          wd 'sm open tab *...'
                                          wd runs and sets recurstate=0.
                                          wd1, 15!:0
Parses 'sm open tab *...'.
smopen()
Sequence of calls to get absolute path to file
using J script >{.getscripts_j_ '~/j9.5-user/temp/42.ijs'
smgetscript(), dors(), dora().
dora requests that JE calculate the path.
Calls JDo, lp="r_jrx_=:>{.getscripts_j_
   '~/j9.5-user/temp/42.ijs'".
                                          JDo recurstate=1
                                          jdo recurstate=1
                                          Calls jtsettaskrunning.
                                          JDo gets return from jdo and sets recurstate=0.
Calls JDo, lp="q_jrx_=:4!:0<'r_jrx_'".
                                          JDo recurstate=1
                                          jdo recurstate=1
                                          Calls jtsettaskrunning.
                                          JDo gets return from jdo and sets recurstate=0.
Calls JGetA, name="q_jrx_".
                                          JGetA recurstate=0
Calls JGetA, name="r_jrx_".
                                          JGetA recurstate=0
Gets the pointer to result (r_jrx) and
additional information (q_jrx).
Uses q_jrx to check if type and rank are correct.
dors returns absolute path as std::string.
smopen detects that file does not exist.
Returns 1(=error) to wd.
                                          wd 'qer'
                                          wd1, 15!:0
Returns error string.
                                          wd 'qer' returns error string to first wd.
                                          'error string' 13!:8 (3)
                                          Calls jtclrtaskrunning.

                                          Beware!
                                          There WAS the following bug when the debugger was enabled.
                                          Immediately enters debug inside wd.
                                          Goes into debug suspension before it has returned to JQt - lockup.
                                          Fixed (May 2024).
*/

/*
Internally, on avx2/avx512 builds, JE always uses avx instructions, never sse instructions.
Hence, we keep the cpu in avx mode all the time, instructing the compiler to avoid emitting spurious vzeroupper instructions between internal function boundaries with -mno-vzeroupper.
However, a frontend might call into je and then try to execute some sse instructions while the cpu is still in avx mode, and thereby enter dirty high half mode, slowing down for no reason; that would be bad.
Therefore, every function in this file MUST execute vzeroupper before returning, to put the cpu back into sse mode for the caller.
Similarly, calls to external libraries MUST be preceded by a vzeroupper, to put the cpu into sse mode for the callee.
*/

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#define filesep '\\'
#else
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#if !defined(__wasm__)
#include <dlfcn.h>
#endif
#ifdef __MACH__
#include <mach-o/dyld.h>
#else // Linux
#include <link.h>
#endif
#include <ctype.h>
#include <unistd.h>
#include <sys/mman.h>
#define filesep '/'
#define _stdcall
#endif
#include <stdint.h>

#include "j.h"
#include "d.h"
#undef JT
#define JT(p,n) p->n  // used for references in JS, which most references in this module are
#define IJT(p,n) JT(JJTOJ(p),n)    // used in function that interfaces to internal functions and thus take a JJ

JS _Initializer(void*);
extern void dllquit(JJ);
extern void cblasinit(C*libpath);

// flags in jt indicate whether display is suppressed.  p is the prompt, s is the text (whose length is m).  suppression of s happens when it is created;
// here we control suppression of p; but we suppress all anyway
void NOINLINE jtwri(JS jt,I type,C*p,I m,C*s){FPREFIP(JS);C buf[1024],*t=OUTSEQ,*v=buf;I c,d,e,n;
 if(!((I)jtfg&JTPRNOSTDOUT)){  // if the prompt is not suppressed...
  c=strlen(p);            /* prompt      */
  e=strlen(t);            /* end-of-line */
  n=sizeof(buf)-(c+e+1);  /* main text   */
  d=m>n?n-3:m;
  MC(v,p,c); v+=c;
  MC(v,s,d); v+=d; if(m>n){MC(v,"...",3L); v+=3;}
  MC(v,t,e); v+=e;   // join prompt/body/EOL
  *v=0;   // NUL termination
#ifdef ANDROID
  A z=jttocesu8(MDTHREAD(jt),jtstr(MDTHREAD(jt),strlen(buf),buf));  // calling internal jt... functions
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
 if(!b){  // if user didn't suppress control checks...
  // edit the line
  if(n&&COFF==s[n-1])joff(num(0));  // exit J on XOFF
  c=IJT(jt,bx)[9]; if((UC)c>127)DO(n, if(' '!=s[i]&&c!=s[i]){k=i; break;});  // if first char is non-ASCII, it will be a spelling error anyway, so trim leading | and SP which might have been added as an error prefix
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

// alternative input prompt
// s is a NUL-terminated sentence that does the prompt (usually 'input_jfe_ prompt-string')
// result is NUL-terminated string which is on the tstack
// We echo the string before returning
static C* nfeinput(JS jt,C* s){A y;
 WITHATTNDISABLED(y=jtstr0(MDTHREAD(jt),PARSERVALUE(jtparse((JTT*)((I)MDTHREAD(jt)|JTFROMEXEC),jtddtokens(MDTHREAD(jt),jtcstr(MDTHREAD(jt),s),4+1+0)))););  // replace DDs, but require that they be complete within the string (no jgets); 0 is !!EXPLICITRUNNING; execute, NUL-terminate

 if(!y){breakclose(jt);exit(2);} /* J input verb failed */
 jtwri(jt,MTYOLOG,"",AN(y)-1,CAV(y));  // call to nfeinput() comes from a prompt or from jdo.  In either case we want to display the result.  Thus jt
 return CAV(y); /* don't combine with previous line! CAV runs (x) 2 times! */
 // the value y is still on the tpop stack
}

// type NUL-terminated prompt string p, read 1 line, & return A block for the line
// p[0] is 0 for prompts from 1!:1]1 and m : 0.  In that case p[1] is 0 for 1!:1]1, 1 for m : 0
// otherwise processed in inpl
// Lines may come from a script, in which case return 0 on EOF, but EVINPRUPT is still possible as an error (user pressed ATTN)
// If Jinput() returns 0, exit with result 0 and no error.  If Jinput() returns 1, signal error EVFACE.
A jtjgets(JJ jt,C*p){A y;C*v;I j,k,m,n;UC*s;
 __atomic_store_n(&IJT(jt,adbreak)[0],0,__ATOMIC_RELEASE);  // this is CLRATTN but for the definition of JT here
 B raw=p[0]==0;  // if no prompt, it's 1!:1]1 or m :  0 - suppress editing the line
 DC d; for(d=jt->sitop; d&&d->dctype!=DCSCRIPT; d=d->dclnk);  // d-> last SCRIPT type, if any
 if(d&&d->dcss){   // enabled DCSCRIPT debug type - means we are reading from file (or string)  for 0!:x
  // read next line from script
  while(1){
   ++d->dcn; j=d->dcix; // increment line# and fetch current start index
   y=d->dcy; n=AN(y); s=UAV(y);
   if(!BETWEENO(j,0,n)){jt->scriptskipbyte=0; R 0;}  // return 0 for EOF or aborted read (dcix<0)
   d->dcj=k=j;  // k=start index
   d->dcix=j=advl(j,n,s);  // j=end+1 index
   if(unlikely(jt->scriptskipbyte!=0)){if(j-k>=4&&s[k]=='N'&&s[k+1]=='B'&&s[k+2]=='.'&&s[k+3]==jt->scriptskipbyte)jt->scriptskipbyte=0; else continue;}  // if skipping in script, skip if not end marker
   break;
  }
  m=j-k; if(m&&32>s[k+m-1])--m; if(m&&32>s[k+m-1])--m;  // m is length; discard trailing control characters (usually CRLF, but not necessarily) ?not needed: done in inpl
  jtwri((JS)((I)JJTOJ(jt)+d->dcpflags),MTYOLOG,p,m,k+s);  // log the input, but only if we wanted to echo the input
  R inpl(raw,m,k+s);  // process & return the line
 }
 // read from keyboard
 /* J calls for input in 3 cases:
    debug suspension for normal input
    n : 0 input lines up to terminating ), or {{ reading for trailing }}
    1!:1[1 read from keyboard */
 showerr();  // there may be error text that has not been emitted.  jt->jerr will be clear.
 ASSERT(IJT(jt,promptthread)==THREADID(jt),EVINPRUPT)  // only one thread is allowed to read from m : 0 or stdin - make sure we are it
 ASSERT(!(jt->recurstate&RECSTATEPROMPTING),EVCTRL)   // if we are already prompting, a second prompt would be unrecoverable & we fail this request
 ASSERT(IJT(jt,nfe)||IJT(jt,sminput),EVBREAK);  // make sure there is a routine to read with
 // read from the front end. This is either through the nfe path or via the callback to the FE
 // make sure only one thread prompts at a time.
 I origstate=jt->recurstate; jt->recurstate|=RECSTATERENT+RECSTATEPROMPTING; jt->recurstate&=~RECSTATERUNNING;  // indic reenterable & prompt; if any sentences come into JDo during the call, they are recursions
 if(IJT(jt,nfe)){  // call the front end to prompt
  // Native Front End
  v=nfeinput(JJTOJ(jt),*p?"input_jfe_'      '":"input_jfe_''");   // use jt so always emit prompt
 }else{
  v=((inputtype)(IJT(jt,sminput)))(JJTOJ(jt),p);
 }
 jt->recurstate=origstate;   // prompt complete, go back to normal (running) state
 if(unlikely(v==0))R0;  // return 0 on EOF
 ASSERT(v!=(C*)1,EVFACE)  // if prompt returned error, call that EVFACE
 R inpl(raw,(I)strlen(v),v);  // return A block for string
}


#if SYS&SYS_UNIX
void breakclose(JS jt)
{
 if(JT(jt,adbreak)==(C*)&JT(jt,breakbytes)) return;  // if no mapped file has been created, exit fast
 __atomic_store_n(&JT(jt,breakbytes),*(US*)(JT(jt,adbreak)),__ATOMIC_RELEASE);  // copy over any pending break request, plus other breakdata
 if(JT(jt,adbreakr)==JT(jt,adbreak))__atomic_store_n(&JT(jt,adbreakr),(C*)&JT(jt,breakbytes),__ATOMIC_RELEASE);  // move to look at the new data - but not if attn disabled
 C *oldbreak=JT(jt,adbreak); __atomic_store_n(&JT(jt,adbreak),(C*)&JT(jt,breakbytes),__ATOMIC_RELEASE);  // move request pointer in any case
 munmap(oldbreak,1);  // don't unmap the old area until pointers have been moved
 close((intptr_t)JT(jt,breakfh));
 JT(jt,breakfh)=0;
 unlink(JT(jt,breakfn));
 *JT(jt,breakfn)=0;
}
#else
void breakclose(JS jt)
{
 if(JT(jt,adbreak)==(C*)&JT(jt,breakbytes)) return;  // if no mapped file has been created, exit fast
 __atomic_store_n(&JT(jt,breakbytes),*(US*)(JT(jt,adbreak)),__ATOMIC_RELEASE);  // copy over any pending break request, plus other breakdata
 if(JT(jt,adbreakr)==JT(jt,adbreak))__atomic_store_n(&JT(jt,adbreakr),(C*)&JT(jt,breakbytes),__ATOMIC_RELEASE);  // move to look at the new data - but not if attn disabled
 C *oldbreak=JT(jt,adbreak); __atomic_store_n(&JT(jt,adbreak),(C*)&JT(jt,breakbytes),__ATOMIC_RELEASE);  // move request pointer in any case
 UnmapViewOfFile(oldbreak); // don't unmap the old area until pointers have been moved
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
F1(jtjoff){F12IP;I x;
 ARGCHK1(w);
 x=i0(w);  // use 0 for any nonnumeric arg
 jt->jerr=EVEXIT; jt->etxn=0; // clear old errors, replacing with the higher-priority EVEXIT
 if(IJT(jt,sesm))jsto(JJTOJ(jt), MTYOEXIT,(C*)x); else JFree(JJTOJ(jt));
 // let front-end handle exit.
// exit((int)x);
 R 0;
}

// wrapper to put a new exec chain into play
// set bstkreqd, incr the starting locale, decr the final locale.  unquote will EXECCTIF all changes 
static void jtimmexexecct(JJ jt, A x){
 jt->uflags.bstkreqd=1; INCREXECCTIF(jt->global); jtimmex(jt,x); DECREXECCTIF(jt->global);  // execution of the sentence may change jt->global
}


// if there is an immex sentence, fetch it, protect it from deletion, run it, and undo the protection
// in this routine jt is a thread pointer and jjt is the shared pointer
static void runiep(JS jjt,JJ jt,A *old){
 while(1){
  // if there is an immex phrase, protect it during its execution
  A iep=0; if(jt->iepdo&1){READLOCK(jjt->felock) if((iep=jjt->iep)!=0)ra(iep); READUNLOCK(jjt->felock)}
  if(iep==0)break;
  // run the IEP and clean up after.  We leave iepdo set to 'running' during the exec to suppress postmortem debugging while IEPs are about
  jt->iepdo=2; jtimmexexecct(jt,iep); fa(iep) jt->iepdo&=~2;
  MODESRESET(jt) RESETERR jttpop(jt,old,jt->tnextpushp);
 }
}

// execute the given sentence.  Result is 0 if OK, otherwise error code.  The low 9 bits of jtflagged are available as flags
// We handle postmortem debugging here.
// The sentence may have components that require prompting (m : 0 or {{ }}), which we allow from console only
// at end, if sentence was from console, execute iep if any before returning
static I jdo(JS jtflagged, C* lp){I e;A x;JS jt=(JS)((I)jtflagged&~JTFLAGMSK);JJ jm=MDTHREAD(jt);  // get address of thread struct we are executing in (the master/debug thread)
 jm->jerr=0; jm->etxn=0; /* clear old errors */
 // if the previous console sentence ended with error, and the user replies with ENTER (i. e. empty string), treat that as a request to debug.
 // on any other reply, free up the values and allocations made by the failing sentence
 if(unlikely(jm->pmstacktop!=0&&!(jm->recurstate&RECSTATERENT))){  // last sentence failed and ran to completion to get a full error stack; and our line is from the console
  C *lp2; for(lp2=lp;*lp2;++lp2)if(*lp2!=' '&&*lp2!='\t'&&*lp2!='\n')break;   // stop on non-whitespace or end-of-string
  // reverse the order of the pmstack so we process from the newest frames to the oldest
  DC pmcurr=jm->pmstacktop, pmrevd=0, pmnext; do{pmnext=pmcurr->dclnk; pmcurr->dclnk=pmrevd; pmrevd=pmcurr; pmcurr=pmnext;}while(pmcurr);
  if(*lp2){  // nonnull string
   // user responded to error with a new sentence.  Clear the error stack.  After we delete a symbol table, we tpop back to where the table was created 
   jttpop(jm,pmrevd->dcttop,jm->tnextpushp);  // When we entered PM debug, we probably did eformat(), which allocates some headers (gah()).  These might alias named variables, and must be popped off the
        // tstack before any names are deleted, lest pointers in the name be invalidated.  After the headers are gone it is safe to delete names.  We pop the stack back to where it was when
        // the first (i. e. newest) PM frame was created.
   DC s=pmrevd; while(s){jtsymfreeha(jm,s->dcloc);  __atomic_store_n(&AR(s->dcloc),ARLOCALTABLE,__ATOMIC_RELEASE); s=s->dclnk;} jm->pmstacktop=0;  // purge symbols & clear stack
   jttpop(jm,jm->pmttop,jm->tnextpushp);  // free all memory allocated in the previous sentence
   jm->pmttop=0;  // clear to avoid another reset
  }else{
   // user wants to debug the error.  Transfer the pmstack to the debug stack in reverse order.  ra() the self for each block - necessary in case they are reassigned while on the stack
   DC s=pmrevd; while(s){if(s->dctype==DCCALL&&s->dcpflags==1)ra(s->dcf) s=s->dclnk;} jm->sitop=pmrevd; jm->pmstacktop=0;  // ra() selfs, set pmstack as debug stack
   pmrevd->dcsusp=1;   // debug discards lines before the suspension, so we have to mark the stack-top as starting suspension
   lp="'dbr 0 to end inspection; use y___1 to look inside top stack frame (see code.jsoftware.com/wiki/Debug/Stack#irefs)' [ dbg_z_ 513";  // change the sentence to one that starts the debug window with no TRACEDBSUSCLEAR flag
  }
 }

 if(JT(jt,capture))JT(jt,capture)[0]=0; // clear capture buffer
 A *old=jm->tnextpushp;
 __atomic_store_n(&JT(jt,adbreak)[0],0,__ATOMIC_RELEASE);  // this is CLRATTN but for the definition of JT here
 x=jtinpl(jm,0,(I)strlen(lp),lp);   // convert user's line to an A block
 I wasidle=jtsettaskrunning(jm);  // We must mark the master thread as 'running' so that a system lock started in another task will include the master thread in the sync.
      // but if the master task is already running, this is a recursion, and just stay in running state
 // Check for DDs in the input sentence.  If there is one, call jgets() to finish it.  Result is enqueue()d sentence.  If recursive, don't allow call to jgets()
 x=jtddtokens(jm,x,(((jm->recurstate&RECSTATERENT)<<(2-RECSTATERENTX)))+1+(AN(jm->locsyms)>SYMLINFOSIZE));  // allow reads from jgets() if not recursive; return enqueue() result
 if(!jm->jerr)jtimmexexecct(jm,x);  //  ****** here is where we execute the user's sentence ******
 // if PM debugging is active, we must have just executed the user's sentence to go there.  Go into suspension so that we get the prompt and engage with the debugger
 if(unlikely(jm->pmttop!=0)){
  jtsusp(jm,0);  // further prompts come from suspension.  We will stay there till dbr 0
  // End of PM debug.  go through the stack, which must all have come from post-mortem.  Free the symbols and the block itself (to match the ra when we moved the pm stack to the debug stack)
  JJ jt=jm; DC s=jm->sitop; I tpopped=0;  // fa vbl, scan ptr for blocks, one-time tppo
  while(s){   // for each debug block
   if(s->dctype==DCCALL&&s->dcpflags==1){  // if PM block
    if(!tpopped){jttpop(jm,s->dcttop,jm->tnextpushp); tpopped=1;}  // tpop before the first PM block
    if(s->dcc!=0){jtsymfreeha(jm,s->dcloc); __atomic_store_n(&AR(s->dcloc),ARLOCALTABLE,__ATOMIC_RELEASE);}  // free symbol table
    if(s->dcf!=0)fa(s->dcf);  // undo the ra above
   }
   s=s->dclnk;  // step to next block
  }
  jm->sitop=0; old=jm->pmttop; jm->pmttop=0;  // clear debug stack; back up the tpop pointer to the pm error; remove request for it
 }
 e=jm->jerr; MODESRESET(jm)  // save error on sentence to be our return code
 jtshowerr(jm);   // jt flags=0 to force typeout of iep errors
 RESETERRT(jm)
 // if there is an immex latent expression (9!:27), execute it before prompting.  It runs in the same state as the user sentence
 // All these immexes run with result-display enabled (jt flags=0)
 // BUT: don't do it if the call is recursive.  The user might have set the iep before a prompt, and won't expect it to be executed asynchronously
 // we could do this in a loop back through exexct, but we choose not to
 if(likely(!(jm->recurstate&RECSTATERENT))){   // if line to be executed was from the user...
  JT(jt,dbuser)&=~TRACEDBSUSCLEAR;  // SUSCLEAR must clear all levels of suspension.  When we get back to user-prompt, it is safe to remove it
  runiep(jt,jm,old);  // IEP does not display its errors
 }

 // user's sentence and iep if any are finished.  e has the return code.  Return to user
 if(likely(wasidle)){  // returning to immex in the FE
  jtclrtaskrunning(jm);  //  clear running state in case other tasks are running and need system lock - but not if recursion
  // since we are returning to user-prompt level, we might as well take user think time to trim up memory
  jtrepatrecv(jm);  // receive any repatriated sentences
  jtspfree(jm);  // check for garbage collection
 }
 // free any memory left at the end of the sentence.  BUT if the sentence failed and created a pm stack, defer the free and offer pm debugging
 if(likely(((I)jm->pmstacktop|(I)jm->pmttop)==0))jttpop(jm,old,jm->tnextpushp); else{if(jm->pmttop==0)jsto(jt,MTYOER,"Press ENTER to inspect\n"); jm->pmttop=old;}  // free if not new or continuing pm session; message first time through
 R e;
}

#define SZINT             ((I)sizeof(int))

C* getlocale(JS jt){JJ jm=MDTHREAD(jt); A y=jtlocname(jm,mtv); y=AAV(y)[0]; R CAV(jtstr0(jm,y));}   // return current locale of executing thread

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
DF1(jtwd){F12IP;A z=0;C*p=0;D*pd;I e,*pi,t;V*sv;
  F1RANK(1,jtwd,self);
  ARGCHK1(w);
  ASSERT(2>AR(w),EVRANK);
  sv=VAV(self);
  t=sv->localuse.lu1.foreignmn[1];  // the n arg from the original 11!:n
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
  RZ(w=jtmemu(jtfg,w));
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
  ASSERT(IJT(jt,smdowd),EVFACE);  // error if no handler in the host
  I origstate=jt->recurstate; jt->recurstate|=RECSTATERENT; jt->recurstate&=~RECSTATERUNNING;  // indic reenterable, and a console task is running
  if(SMOPTLOCALE&IJT(jt,smoption)) {
// pass locale as parameter of callback
    e=((dowdtype2)(IJT(jt,smdowd)))(JJTOJ(jt), (int)t, w, &z, getlocale(JJTOJ(jt)));
  } else {
// front-end will call getlocale() inside callback
    e=((dowdtype)(IJT(jt,smdowd)))(JJTOJ(jt), (int)t, w, &z);
  }
  jt->recurstate=origstate;  // wd complete, restore previous running state
  if(!e) R mtm;   // e==0 is MTM
  ASSERT(e<=0,e); // e>=0 is EVDOMAIN etc
  if(SMOPTPOLL&IJT(jt,smoption)){ jt->recurstate|=RECSTATERENT; jt->recurstate&=~RECSTATERUNNING; z=(A)((polltype)(IJT(jt,smpoll)))(JJTOJ(jt), (int)t, (int)e); jt->recurstate=origstate; RZ(z);} // alternate way to get result aftercallback, but not yet used in any front-end
  if(SMOPTNOJGA&IJT(jt,smoption)) z=ca(z);  // front-end promised not to use Jga to allocate memory, but not yet used in any front-end
  if(e==-1){      // e==-1 is lit or int
  RETF(z);
  } else if(e==-2){      // e==-2 is lit pairs
// callback result z is a rank-1 literal array 
// literal array will be cut into rank-1 box array here using  <;._2 
// and then reshape into rank-2  ((n%2),2)$
    A x=z; RZ(dfv1(z,x,cut(ds(CBOX),num(-2))));
    RETF(reshape(v2(AN(z)>>1,2L),z));
  } else {RETF(z);}      // no need to fix anything
}

static char breaknone=0;

// Init anything the sessions manager needs in the jt for a new instance
B jtsesminit(JS jjt, I nthreads){R 1;}

// Main entry point to run the sentence in *lp in the master thread, or in the thread given if jt is not a JS pointer
// Run sentence; result is jt->jerr
// the low bits of jt are available as flags
// If JE(jt,nfe), loop forever reading sentences (but not if this is a recursive call)
CDPROC int _stdcall JDo(JS jtflagged, C* lp){int r;JS jt=(JS)((I)jtflagged&~JTFLAGMSK);
 SETJTJM(jt,jm)

 // Move to next state.  If we are not interruptible, that's an error
 I origrstate=jm->recurstate;   // save initial state
 if(unlikely(origrstate&RECSTATERUNNING))R EVCTRL;  // running: reentrance not alllowed
 // set the stacklimits to use.  Normally we keep the same stackpointer throughout, but (1) if the FE is multithreaded, we check for a new one on each call and restore it after a reentrant call
 UI savcstackmin=0, savcstackinit, savqtstackinit;
 if(JT(jt,cstacktype)==2){
  // multithreaded FE.  Reinit the stack limit on every call, as long as cstackmin is nonzero
  JT(jt,qtstackinit) = (uintptr_t)&jt;
  if(jm->cstackmin){
   if(origrstate&RECSTATERENT)savcstackmin=jm->cstackmin, savcstackinit=jm->cstackinit, savqtstackinit=JT(jt,qtstackinit);  // save stack pointers over recursion
   jm->cstackmin=(jm->cstackinit=JT(jt,qtstackinit))-(CSTACKSIZE-CSTACKRESERVE);
  }
 }


 A *old=jm->tnextpushp;  // For JHS we call nfeinput, which comes back with a string address, perhaps after considerable processing.  We need to clean up the stack
  // after nfeinput, but we dare not until the sentence has been executed, lest we deallocate the unexecuted sentence.  Also we have to suppress the pops while there is a
  // pm debug stack.  We take advantage of the fact that the popto point doesn't change, save that here, and suppress the pop during pm
 while(1){   // normal console runs 1 sentence; for comp ease JHS loops forever bere executing sentences
  jm->recurstate|=RECSTATERUNNING;  // if we were from console, this goes to running.  If we are running already, we must be at a recursion point that set RENT
  MAYBEWITHDEBUG(!(origrstate&RECSTATERENT),jm,r=(int)jdo(jtflagged,lp);)
  jm->recurstate=origrstate;   // restore original idle/rent state

  if(jm->recurstate&RECSTATERENT)break;  // if the call was a recursion, don't prompt again
  else JT(jt,sidamage)=0;  // when any non-recursion goes to prompt level, clear sidamage (backstop - usually suspension does this)
  if(!JT(jt,nfe))break;  // normal exit: return to host
  if(jm->jerr==EVEXIT)break;  // if user terminated J, terminate this loop also

  // falling through here is for JHS
  // If there is a postmortem stack active, jdo has frozen tpops and we have to honor that here, to keep the stack data allocated.
  if(likely(jm->pmttop==0))jttpop(jm,old,jm->tnextpushp);  // when the stack has been tpopped it is safe for us to resume
  jm->recurstate|=RECSTATERENT;  // This is a prompt spot.  Any new call is an allowed recursion
  lp=nfeinput(jt,"input_jfe_'   '");  // JHS: get next input.  This is a prompt spot
  jm->recurstate=origrstate;  // after prompt, return to idle
 }

 if(unlikely(savcstackmin!=0))jm->cstackmin=savcstackmin, jm->cstackinit=savcstackinit, JT(jt,qtstackinit)=savqtstackinit;  // if stack pointers were saved, retire them

 R r;
} 

CDPROC C* _stdcall JGetR(JS jt){
 jt=JorJJTOJ(jt);  // point to shared block
 R JT(jt,capture)?JT(jt,capture):(C*)"";
}

/* socket protocol CMDGET name */
// Return the binary rep of the given name
// We MALLOC a return block so that we don't lose J memory.  We reuse the block for successive calls, or maybe free it,
// so the user must save it before re-calling.  This is a kludge - the user should pass in the address/length of the block to use - but
// it preserves the interface
// If the pointer to the name is NULL we just free the block
CDPROC A _stdcall JGetA(JS jt, I n, C* name){A x,z=0;
 SETJTJM(jt,jm)
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
 jttpop(jm,old,jm->tnextpushp);
 R z;   // return the allocated (or reused) area
}

/* socket protocol CMDSET */
CDPROC I _stdcall JSetA(JS jt,I n,C* name,I dlen,C* d){
 SETJTJM(jt,jm)
 jm->jerr=0;
 if(!jtvnm(jm,n,name)){jtjsignal(jm,EVILNAME); R EVILNAME;}
 A *old=jm->tnextpushp;
 jtsymbisdel(jm,jtnfs(jm,n,name),jtunbin(jm,jtstr(jm,dlen,d)),jm->global);
 jttpop(jm,old,jm->tnextpushp);
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

CDPROC void _stdcall JSM(JS jt, void* callbacks[])
{
 SETJTJM(jt,jm)
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
CDPROC void _stdcall JSMX(JS jt, void* out, void* wd, void* in, void* poll, I opts)
{
 SETJTJM(jt,jm)
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
CDPROC C* _stdcall JGetLocale(JS jt){
 SETJTJM(jt,jm)
 A *old=jm->tnextpushp;  // set free-back-to point
 if(JT(jt,iomalloc)){FREE(JT(jt,iomalloc)); jm->malloctotal -= JT(jt,iomalloclen); JT(jt,iomalloc)=0; JT(jt,iomalloclen)=0;}  // free old block if any
 C* z=getlocale(jt);  // get address of string to return
 if(JT(jt,iomalloc)=MALLOC(1+strlen(z))){jm->malloctotal += 1+strlen(z); JT(jt,iomalloclen) = 1+strlen(z); strcpy(JT(jt,iomalloc),z); }  // allocate & copy, and account for its space
 jttpop(jm,old,jm->tnextpushp);  // free allocated blocks
 R JT(jt,iomalloc);  // return pointer to string
}

CDPROC A _stdcall Jga(JS jjt, I t, I n, I r, I*s){A z;
 SETJTJM(jjt,jt)  // the name 'jt' is used by ga() for the shared pointer
 GA(z,t,n,r,s);
 ACINIT(z,ACUC1)  // set nonrecursive usecount so that parser won't free the block prematurely.  This gives the usecount as if the block were 'assigned' by this call
 return z;
}

CDPROC void _stdcall JInterrupt(JS jt){
 jt=JorJJTOJ(jt);
 // increment adbreak by 1, capping at 2
 C old=lda(&jt->adbreak[0]);
 while(1){
  if(old>=2)break;
  if(casa(&jt->adbreak[0],&old,1+old))break;}
 // There is no need to wakeup waiting threads (good thing, because jbreak.bat can't do that, and anyway the master/debug thread may already be running).  All wait loops time out to recheck break status.
}

void oleoutput(JS jt, I n, char* s); /* SY_WIN32 only */

#define capturesize 80000

/* jsto - display output in output window */
// type is mtyo of string, s->null-terminated string
void jsto(JS jt,I type,C*s){C e;I ex;
 if(JT(jt,nfe))
 {JJ jm=MTHREAD(jt);  // get address of thread struct we are using.  For the nonce we always use the master in case there is stored state there, since we aren't executing sentences
 if(MTYOEXIT==type) { JFree(JJTOJ(jt)); exit((int)(intptr_t)s); }
  // here for Native Front End state, toggled by 15!:16
  // we execute the sentence:  type output_jfe_ s    in the master thread
  fauxblockINT(fauxtok,3,1); A tok; fauxBOXNR(tok,fauxtok,3,1);  // allocate 3-word sentence on stack, rank 1
  DISABLEATTN
  AAV1(tok)[0]=PTROP(num(type),|,QCNAMED+QCNOUN); AAV1(tok)[1]=PTROP(jtnfs(jm,11,"output_jfe_"),|,QCISLKPNAME); AAV1(tok)[2]=PTROP(jtcstr(jm,s),|,QCNAMED+QCNOUN);  // the sentence to execute, tokenized and with flag-types installed.  Better not fail!
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

C dll_initialized= 0; // dll init sets to 1

#ifdef _WIN32
char modulepath[_MAX_PATH]="";
char sopath[_MAX_PATH]="";

void getsopath(HINSTANCE hi, char* path)
{
 WCHAR wpath[_MAX_PATH];char* p;

 GetModuleFileNameW(hi, wpath, _MAX_PATH);
 WideCharToMultiByte(CP_UTF8,0,wpath,1+(int)wcslen(wpath),path,_MAX_PATH,0,0);
 p = strrchr(path, filesep);
 if(!p) {p = strrchr(path, ':'); p[1] = 0;} else p[0] = 0;
}

#else
// SYS&SYS_UNIX
char sopath[PATH_MAX]="";

void getsopath(char* path){
const char *sym_name = "JGetLocale";
#if defined(__wasm__)
//  *path=0;  /* missing dladdr function */
 strcpy(path,"/");
#else
 void *sym_ptr = dlsym(RTLD_DEFAULT,sym_name);
// fprintf(stdout,"SYMBOL %s ADDRESS %p\n", sym_name, sym_ptr);
 if (sym_ptr){
  Dl_info info;
  if (dladdr(sym_ptr,&info)){ // non-zero is success
   strcpy(path,info.dli_fname);
   char *p1;
   if((p1=strrchr(info.dli_fname,filesep))){path[p1-(char*)info.dli_fname]=0;}
  } else *path=0;
 } else *path=0;
#endif
// fprintf(stdout,"sopath: %s\n", path);
}

// dll init on load - eqivalent to windows DLLMAIN DLL_ATTACH_PROOCESS
__attribute__((constructor)) static void Initializer(){
 _Initializer(0);
}
#endif

// dll init on load - eqivalent to windows DLLMAIN DLL_ATTACH_PROOCESS
JS _Initializer(void* hDLL){
 // Initialize J globals.  This is done only once.  Many of the globals are in static memory, initialized
 // by the compiler; some must be initialized a run-time in static memory; some must be allocated into A blocks
 // pointed to by static names.  Because of the A blocks, we have to perform a skeletal initialization of jt,
 // just enough to do ga().  The rest of jt is never used
#ifdef _WIN32
 getsopath(0,modulepath);
 getsopath(hDLL,sopath);
#else
 getsopath(sopath);
#endif
 JS jt=jvmreservea(sizeof(JST),__builtin_ctz(JTALIGNBDY));
 if(!jt)R 0;
 I sz=offsetof(JST,threaddata[1]); // #relevant bytes: just JS and the first JT.  This makes MDTHREAD() valid
 if(!jvmcommit(jt,sz)){jvmrelease(jt,sizeof(JST));R 0;}
 if(!jtglobinit(jt)){jvmrelease(jt,sizeof(JST)); R 0;}
 dll_initialized=1;
 jvmrelease(jt,sizeof(JST)); //the jt block itself can be released; we effectively orphan any blocks pointed to thereby, because they are used by the globals we've just initialised
 cblasinit(sopath); // cblas
 R jt;
}

 // Init for a new J instance.  Globals have already been initialized.
 // Create a new jt, which will be the one we use for the entirety of the instance.
CDPROC JS _stdcall JInit(void){
 if(!dll_initialized)R 0; // constructor failed
 JS jt=jvmreservea(sizeof(JST),__builtin_ctz(JTALIGNBDY));
 if(!jt)R 0;
 if(!jvmcommit(jt,offsetof(JST,threaddata[1]))){jvmrelease(jt,sizeof(JST));R 0;}
 jvmwire(jt,offsetof(JST,threaddata[1])); //JS should probably not ever be swapped out.  But failure is non-catastrophic
 mvc(offsetof(JST,threaddata[1]),jt,MEMSET00LEN,MEMSET00);  // init JST and JST[0] (master thread) to all 0
 // Initialize all the info for the shared region and the master thread
 if(!jtjinit2(jt,0,0)){jvmrelease(jt,sizeof(JST)); R 0;}
 jgmpinit(sopath); // mp support for 1x and 2r3
 ZEROUPPER;
 R jt;  // return JST
}

 // Init for a new J instance.  Globals have already been initialized.
 // Create a new jt, which will be the one we use for the entirety of the instance.
CDPROC JS _stdcall JInit2(C*libpath){
 if(!dll_initialized)R 0; // constructor failed
 JS jt=jvmreservea(sizeof(JST),__builtin_ctz(JTALIGNBDY));
 if(!jt)R 0;
 if(!jvmcommit(jt,offsetof(JST,threaddata[1]))){jvmrelease(jt,sizeof(JST));R 0;}
 jvmwire(jt,offsetof(JST,threaddata[1])); //JS should probably not ever be swapped out.  But failure is non-catastrophic
 mvc(offsetof(JST,threaddata[1]),jt,MEMSET00LEN,MEMSET00);
 // Initialize all the info for the shared region and the master thread
 if(!jtjinit2(jt,0,0)){jvmrelease(jt,sizeof(JST)); R 0;}
 if(libpath){strcpy(sopath,libpath);if(strlen(sopath)&&(filesep==sopath[strlen(sopath)-1]))sopath[strlen(sopath)-1]=0;}
 jgmpinit(sopath); // mp support for 1x and 2r3
 ZEROUPPER;
 R jt;  // return JST
}

// clean up at the end of a J instance
CDPROC int _stdcall JFree(JS jt){
  if(!jt) R 0;
  SETJTJM(jt,jm)
  breakclose(jt);
  jm->jerr=0; jm->etxn=0; /* clear old errors */
  dllquit(jm);  // clean up call dll
#if PYXES
  aligned_free(JT(jt,jobqueues));
#endif
  jvmrelease(jt,sizeof(JST)); // free the initial allocation
  ZEROUPPER;
  R 0;
}

F1(jtbreakfnq){F12IP;
 ASSERTMTV(w);
 R cstr(IJT(jt,breakfn));
}

// w is a filename; use it as the breakfile
F1(jtbreakfns){F12IP;A z;I *fh,*mh=0; void* ad;
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
 __atomic_store_n((US*)ad,*(US*)IJT(jt,adbreak),__ATOMIC_RELEASE);  // copy breakstatus from current setting
 if(IJT(jt,adbreakr)==IJT(jt,adbreak))IJT(jt,adbreakr)=ad;  // move attn-read pointer, unless interrupts disabled
 IJT(jt,adbreak)=ad;  // start using the mapped area
 ZEROUPPER;
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
 ZEROUPPER;
 return 0;  
}

CDPROC int _stdcall JGetM(JS jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata)
{
 SETJTJM(jt,jm)
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
 jttpop(jm,old,jm->tnextpushp);
 ZEROUPPER;
 return z;
}

static int setterm(JS jtt, C* name, I* jtype, I* jrank, I* jshape, I* jdata)
{JJ jt=MTHREAD(jtt);   // use master thread always since there is no execution
 A a;
 I k=1,i,n;
 char gn[256];

 switch(CTTZNOFLAG(*jtype)){
 case LITX:
 case B01X:
  n = sizeof(char);
  break;

 case C2TX: case INT2X:
  n = sizeof(unsigned short);
  break;

 case C4TX: case INT4X:
  n = sizeof(unsigned int);
  break;

 case INTX:
 case SBTX:
  n = sizeof(I);
  break;
  
 case FLX:
  n = sizeof(double);
  break;
  
 case CMPXX: case QPX:
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
 ZEROUPPER;
 return jt->jerr;
}

CDPROC int _stdcall JSetM(JS jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata)
{
 SETJTJM(jt,jm)
 int er;

 A *old=jm->tnextpushp;
 er = setterm(jt, name, jtype, jrank, jshape, jdata);
 jttpop(jm,old,jm->tnextpushp);
 ZEROUPPER;
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

CDPROC int _stdcall JErrorTextM(JS jt, I ec, I* p)
{
 jt=JorJJTOJ(jt);
 *p = (I)esub(jt, ec);
 ZEROUPPER;
 return 0;
}

#if 0  // for debugging
int enabledebug=0;
F1(jttest1){F12IP;
 ARGCHK1(w);
 if((AT(w)&B01+INT)&&AN(w)){
  enabledebug=i0(w);
 }
 R sc(1);
}
#endif

