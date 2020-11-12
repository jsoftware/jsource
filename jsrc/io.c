/* Copyright 1990-2011, Jsoftware Inc.  All rights reserved.               */
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
  jt->sminput=input
  jt->smoutput=output

 input()        - get kb line for jdo() or jt->sminput() result
 
 output(string) - print s from jt->smoutput(type,string) 

je routines (io.c):
 jdo(line)

 jsto(type,string) - jt->smoutput() to give jfe output
             
 jgets() - jt->sminput() callback to get jfe kb input
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
   input()   (optional) <-------- if DD seen, get more lines if needed by calling jt->sminput
                                  jdo() calls immex(inpl(sentence))
                                   ... 
   output(s)           <--------  jsto(type,s) - jt->smoutout(type,s)
   output returns      ---------> ...
 ...                   <--------- jdo returns with error code
 loop

*** repl with write to display (1!:2[2) - echo/smoutput
 ...
                                  1:2[2 calls jtwri()
                                  jsto(type,string)
   output(s)           <--------- jt->smoutout(type,s)
   output returns      ---------> ...

*** repl with read from keyboard (1!:1[1)
 ...
                                  1:1[1 calls jtjfread()
   input()             <--------  jt->sminput()
   input returns line  -------->  ...

*** repl with debug suspension
 ...
                                     loop as long as suspended
                                       call jgets()
   input()            <---------       call jt->sminput()
   return line        --------->
   input()   (optional) <--------      if DD seen, get more lines if needed by calling jt->sminput
                                       call immex(inpl(line)
                                       loop
*** m : 0
similar to debug suspension except jgets() lines added  to defn.  m : 0 stops reading
after encountering ) on a line by itself.  If not 0 : 0, call ddtokens() after each line
to see if more lines need to be read to finish the DD; is so, call jgets() to get them

*** script load
linf() is called first to read in all lines.  It sets jt->dcs to indicate that fact.  Thereafter
all calls to jgets() return llies from the file without calling jt->sminput().
jgets() calls advl() to advance through the lines, returns 0 for EOF.  Error is possible.

The lines are executed one by one.  Before each is executed, ddtokens() is called to see if more lines
are needed to finish a DD.

*** jwd (11!:x)
similar to debug suspension except output/input
 processed by gui sm

*** JHS nfe (native front end)
jt->nfe flag - JE does not use jt->smoutout() and jt->sminput()
instead it calls J code that provides equivalent services
JHS routines are J socket code to talk with javascript browser page

DD NOTE: DD is not supported on special sentences: iep, xep, dbssexec, dbtrap.  This is mostly because I don't understand how these work.

DD is supported on ". y, but only if the DD is fully contained in the string
A recursive JDo may use a DD, but only if it is fully contained in the string

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

extern void dllquit(J);

// flags in jt indicate whether display is suppressed.  p is the prompt, s is the text.  suppression of s happen when it is created;
// here we control suppression of p; but we suppress all anyway
void jtwri(J jt,I type,C*p,I m,C*s){F1PREFJT;C buf[1024],*t=OUTSEQ,*v=buf;I c,d,e,n;
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
  A z=tocesu8(str(strlen(buf),buf));
  CAV(z)[AN(z)]=0;
  jsto(jt,type,CAV(z));
#else
  jsto(jt,type,buf);
#endif
 }
}

static void jtwrf(J jt,I n,C*v,F f){C*u,*x;I j=0,m;
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
A jtinpl(J jt,B b,I n,C*s){C c;I k=0;
 if(n&&(c=s[n-1],CLF==c||CCR==c))--n;  // discard trailing [CR], CRLF, CRCR
#if _WIN32
 if(n&&(c=s[n-1],CCR==c))--n;
#endif
 ASSERT(!*jt->adbreak,EVINPRUPT);
 if(!b){ /* 1==b means literal input */
  if(n&&COFF==s[n-1])joff(num(0));
  c=jt->bx[9]; if((UC)c>127)DO(n, if(' '!=s[i]&&c!=s[i]){k=i; break;});
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

void breakclose(J jt);

static C* nfeinput(J jt,C* s){A y;
 jt->adbreakr=&breakdata; y=exec1(cstr(s)); jt->adbreakr=jt->adbreak;
 if(!y){breakclose(jt);exit(2);} /* J input verb failed */
 jtwri(jt,MTYOLOG,"",strlen(CAV(y)),CAV(y));  // call to nfeinput() comes from a prompt or from jdo.  In either case we want to display the result.  Thus jt
 return CAV(y); /* don't combine with previous line! CAV runs (x) 2 times! */
}

// type NUL-terminated prompt string p, read 1 line, & return
// if *p is (C)1 (which comes from m : 0), the request is for unprocessed 'literal input'
// otherwise processed in inpl
// Lines may come from a script, in which case return 0 on EOF, but EVINPRUPT is still possible as an error
A jtjgets(J jt,C*p){A y;B b;C*v;I j,k,m,n;UC*s;
 *jt->adbreak=0;
 if(b=1==*p)p=""; /* 1 means literal input; remember & clear prompt */
 if(jt->dcs){   // DCSCRIPT debug type - means we are reading from file (or string)  for 0!:x
  ++jt->dcs->dcn; j=jt->dcs->dcix; // increment line# and fetch current start index
  y=jt->dcs->dcy; n=AN(y); s=UAV(y);
  if(!(j<n))R 0;  // return 0 for EOF
  jt->dcs->dcj=k=j;  // k=start index
  jt->dcs->dcix=j=advl(j,n,s);  // j=end+1 index
  m=j-k; if(m&&32>s[k+m-1])--m; if(m&&32>s[k+m-1])--m;  // m is length; discard trailing control characters (usually CRLF, but not necessarily) ?not needed: done in inpl
  jtwri((J)((I)jt+jt->dcs->dcpflags),MTYOLOG,p,m,k+s);  // log the input, but only if we wanted to echo the input
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
 if(jt->nfe){
  // Native Front End
  jt->recurstate=RECSTATEPROMPT;  // advance to PROMPT state
  v=nfeinput(jt,*p?"input_jfe_'      '":"input_jfe_''");   // use jt so always emit prompt
 }else{
  ASSERT(jt->sminput,EVBREAK); 
  jt->recurstate=RECSTATEPROMPT;  // advance to PROMPT state
  v=((inputtype)(jt->sminput))(jt,p);
 }
 jt->recurstate=RECSTATEBUSY;  // prompt complete, go back to normal running state
 R inpl(b,(I)strlen(v),v);  // return A block for string
}


#if SYS&SYS_UNIX
void breakclose(J jt)
{
 if(jt->adbreak==&breakdata) return;
 munmap(jt->adbreak,1);
 jt->adbreakr=jt->adbreak=&breakdata;
 close((intptr_t)jt->breakfh);
 jt->breakfh=0;
 unlink(jt->breakfn);
 *jt->breakfn=0;
}
#else
void breakclose(J jt)
{
 if(jt->adbreak==&breakdata) return;
 UnmapViewOfFile(jt->adbreak);
 jt->adbreakr=jt->adbreak=&breakdata;
 CloseHandle(jt->breakmh);
 jt->breakmh=0;
 CloseHandle(jt->breakfh);
 jt->breakfh=0;
#if SY_WINCE
 DeleteFile(tounibuf(jt->breakfn));
#else
 WCHAR wpath[NPATH];
 MultiByteToWideChar(CP_UTF8,0,jt->breakfn,1+(int)strlen(jt->breakfn),wpath,NPATH);
 DeleteFileW(wpath);
#endif
 *jt->breakfn=0;
}
#endif

F1(jtjoff){I x;
 ARGCHK1(w);
 x=i0(w);
 jt->jerr=0; jt->etxn=0; /* clear old errors */
 if(jt->sesm)jsto(jt, MTYOEXIT,(C*)x); else JFree(jt);
// let front-end to handle exit
// exit((int)x);
 R 0;
}

I jdo(J jt, C* lp){I e;A x;
 jt->jerr=0; jt->etxn=0; /* clear old errors */
 // The named-execution stack contains information on resetting the current locale.  If the first named execution deletes the locale it is running in,
 // that deletion is deferred until the locale is no longer running, which is never detected because there is no earlier named execution to clean up.
 // To prevent the stack from growing indefinitely, we reset it here.  We reset the callstack only if it was 0, so that a recursive immex will have its deletes handled by
 // the resumption of the name that was interrupted.
 I4 savcallstack = jt->callstacknext;
 if(jt->capture) jt->capture[0]=0; // clear capture buffer
 A *old=jt->tnextpushp;
 *jt->adbreak=0;
 x=inpl(0,(I)strlen(lp),lp);
 // All these immexes run with result-display enabled (jt flags=0)
 // Run any enabled immex sentences both before & after the line being executed.  I don't understand why we do it before, but it can't hurt since there won't be any.
 // BUT: don't do it if the call is recursive.  The user might have set the iep before a prompt, and won't expect it to be executed asynchronously
 if(likely(jt->recurstate<RECSTATEPROMPT))while(jt->iepdo&&jt->iep){jt->iepdo=0; immex(jt->iep); if(savcallstack==0)CALLSTACKRESET MODESRESET jt->jerr=0; tpop(old);}
 // Check for DDs in the input sentence.  If there is one, call jgets() to finish it.  Result is enqueue()d sentence.  If recursive, don't allow call to jgets()
 x=ddtokens(x,(((jt->recurstate&RECSTATEPROMPT)<<(2-1)))+1+(AN(jt->locsyms)>1)); if(!jt->jerr)immex(x);  // allow reads from jgets() if not recursive; return enqueue() result
 e=jt->jerr;
 if(savcallstack==0)CALLSTACKRESET MODESRESET jt->jerr=0;
 if(likely(jt->recurstate<RECSTATEPROMPT))while(jt->iepdo&&jt->iep){jt->iepdo=0; immex(jt->iep); if(savcallstack==0)CALLSTACKRESET MODESRESET jt->jerr=0; tpop(old);}
 showerr();   // jt flags=0 to force typeout
 spfree();
 tpop(old);
 R e;
}

#define SZINT             ((I)sizeof(int))

C* getlocale(J jt){A y=locname(mtv); y=AAV(y)[0]; R CAV(str0(y));}

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
  F1PREFIP;
  F1RANK(1,jtwd,self);
  ARGCHK1(w);
  ASSERT(2>AR(w),EVRANK);
  sv=VAV(self);
  t=i0(sv->fgh[1]);  // the n arg from the original 11!:n
  if(BETWEENO(t,2000,3000) && AN(w) && !(LIT+C2T+C4T+INT&AT(w))) {  // 2000<=t<3000
    switch(UNSAFE(AT(w))) {
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
  ASSERT(jt->smdowd,EVDOMAIN);
  jt->recurstate&=~RECSTATEBUSY;  // back to IDLE/PROMPT state
  if(SMOPTLOCALE&jt->smoption) {
// pass locale as parameter of callback
// obsolete     e= jt->smdowd? ((dowdtype2)(jt->smdowd))(jt, (int)t, w, &z, getlocale(jt)) : EVDOMAIN;
    e=((dowdtype2)(jt->smdowd))(jt, (int)t, w, &z, getlocale(jt));
  } else {
// front-end will call getlocale() inside callback
// obsolete     e=jt->smdowd ? ((dowdtype)(jt->smdowd))(jt, (int)t, w, &z) : EVDOMAIN;
    e=((dowdtype)(jt->smdowd))(jt, (int)t, w, &z);
  }
  jt->recurstate|=RECSTATEBUSY;  // wd complete, go back to normal running state, BUSY normally or RECUR if a prompt is pending
  if(!e) R mtm;   // e==0 is MTM
  ASSERT(e<=0,e); // e>=0 is EVDOMAIN etc
  if(SMOPTPOLL&jt->smoption){jt->recurstate=RECSTATEPROMPT; z=(A)((polltype)(jt->smpoll))(jt, (int)t, (int)e); jt->recurstate=RECSTATEBUSY; RZ(z);} // alternate way to get result aftercallback, but not yet used in any front-end
  if(SMOPTNOJGA&jt->smoption) z=ca(z);  // front-end promised not to use Jga to allocate memory, but not yet used in any front-end
  if(e==-2){      // e==-2 is lit pairs
// callback result z is a rank-1 literal array 
// literal array will be cut into rank-1 box array here using  <;._2 
// and then reshape into rank-2  ((n%2),2)$
    A x=z; RZ(df1(z,x,cut(ds(CBOX),num(-2))));
    RETF(reshape(v2(AN(z)>>1,2L),z));
  } else {RETF(z);}
}

static char breaknone=0;

B jtsesminit(J jt){jt->adbreakr=jt->adbreak=&breakdata; R 1;}

// Main entry point to run the sentence in *lp
int _stdcall JDo(J jt, C* lp){int r; UI savcstackmin, savcstackinit, savqtstackinit;
 if(unlikely(jt->recurstate>RECSTATEIDLE)){
  // recursive call.  If we are busy or already recurring, this would be an uncontrolled recursion.  Fail that
  savcstackmin=jt->cstackmin, savcstackinit=jt->cstackinit, savqtstackinit=jt->qtstackinit;  // save stack pointers over recursion, in case the host resets them
  ASSERT(!(jt->recurstate&(RECSTATEBUSY&RECSTATERECUR)),EVCTRL)  // fail if BUSY or RECUR
  CLEARZOMBIE   // since we are executing a surprise call, the verb that is prompting might have set assignsym, which is about to be invalidated.  Clear that.
 }
#if USECSTACK
 if(jt->cstacktype==2){
  jt->qtstackinit = (uintptr_t)&jt;
  if(jt->cstackmin)jt->cstackmin=(jt->cstackinit=jt->qtstackinit)-(CSTACKSIZE-CSTACKRESERVE);
 }
#endif
 ++jt->recurstate;  // advance, to BUSY or RECUR state
 r=(int)jdo(jt,lp);
 if(unlikely(--jt->recurstate>RECSTATEIDLE)){  // return to IDLE or PROMPT state
  // return from recursive call.  Restore stackpointers
  jt->cstackmin=savcstackmin, jt->cstackinit=savcstackinit, jt->qtstackinit=savqtstackinit;  // restore stack pointers after recursion
 }
 while(jt->nfe){  // nfe normally loops here forever
  A *old=jt->tnextpushp; r=(int)jdo(jt,nfeinput(jt,"input_jfe_'   '")); tpop(old);  // use jt to force output in nfeinput
 }
 R r;
} 

C* _stdcall JGetR(J jt){
 R jt->capture?jt->capture:(C*)"";
}

/* socket protocol CMDGET name */
// Return the binary rep of the given name
// We MALLOC a return block so that we don't lose J memory.  We reuse the block for successive calls, or maybe free it,
// so the user must save it before re-calling.  This is a kludge - the user should pass in the address/length of the block to use - but
// it preserves the interface
// If the pointer to the name is NULL we just free the block
A _stdcall JGetA(J jt, I n, C* name){A x,z=0;
 if(name==0){if(jt->iomalloc){FREE(jt->iomalloc); jt->malloctotal -= jt->iomalloclen; jt->iomalloc=0; jt->iomalloclen=0;} R 0;}
 jt->jerr=0;
 A *old=jt->tnextpushp;
 if(!(x=symbrdlock(nfs(n,name)))){ jsignal(EVILNAME);  // look up the name, error if invalid
 }else if(FUNC&AT(x)){ jsignal(EVDOMAIN);   // verify the value is not adv/verb/conj
 }else{
  // name is OK; get the binary rep
  if(z=binrep1(x)){
   // bin rep was found.  Transfer it to MALLOC memory.  It is a LIT array
   // we transfer the whole thing, header and all.  Fortunately it is relocatable
   I replen = &CAV(z)[AN(z)] - (C*)z;  // length from start of z to end+1 of data
   // See if we can reuse the block.  We can, if it is big enough.  But if it is twice as big as the return value, don't.  Watch for overflow!
   if(jt->iomalloc && (jt->iomalloclen < replen || (jt->iomalloclen>>1) > replen)){FREE(jt->iomalloc); jt->malloctotal -= jt->iomalloclen; jt->iomalloc=0;}  // free block if not reusable
   if(!jt->iomalloc){if(jt->iomalloc=MALLOC(replen)){jt->malloctotal += replen; jt->iomalloclen = replen;}}  // allocate block if needed, and account for its space
   if(jt->iomalloc){memcpy(jt->iomalloc,z,replen); z=(A)jt->iomalloc;  // normal case: block exists, move the data, set the return address to the malloc block
   }else{jt->iomalloclen=0; z=0;}   // if unable to allocate, return error and indicate block is empty
  }
 }
 // z has the result, which is in MALLOC memory if it exists.  Free any J memory we used
 tpop(old);
 R z;   // return the allocated (or reused) area
}

/* socket protocol CMDSET */
I _stdcall JSetA(J jt,I n,C* name,I dlen,C* d){
 jt->jerr=0;
 if(!vnm(n,name)){ jsignal(EVILNAME); R EVILNAME;}
 A *old=jt->tnextpushp;
 symbisdel(nfs(n,name),jtunbin(jt,str(dlen,d)),jt->global);
 tpop(old);
 R jt->jerr;
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

void _stdcall JSM(J jt, void* callbacks[])
{
 jt->smoutput = (outputtype)callbacks[0];  // callback function for output to J session
// output type
// #define MTYOFM  1 /* formatted result array output */
// #define MTYOER  2 /* error output */
// #define MTYOLOG  3 /* output log */
// #define MTYOSYS  4 /* system assertion failure */
// #define MTYOEXIT 5 /* exit */
// #define MTYOFILE 6 /* output 1!:2[2 */
 jt->smdowd = callbacks[1];                // callback function for 11!:x
 jt->sminput = (inputtype)callbacks[2];    // callback function for input from J session keyboard
 jt->smpoll = (polltype)callbacks[3];      // callback function for unused
 jt->sm = 0xff & (I)callbacks[4];          // lowest byte, sm : sessioin manager type
// smoptions
// #define SMWIN    0  /* j.exe    Jwdw (Windows) front end */
// #define SMJAVA   2  /* j.jar    Jwdp (Java) front end */
// #define SMCON    3  /* jconsole */
// #define SMQT     4  /* jqt */
// #define SMJA     5  /* jandroid */
 jt->smoption = ((~0xff) & (UI)callbacks[4]) >> 8;  // upper bytes, smoption : 
// #define SMOPTLOCALE 1  /* pass current locale */
// #define SMOPTNOJGA  2  /* result not allocated by jga */
// #define SMOPTPOLL   4  /* use smpoll to get last result */
// #define SMOPTMTH    8  /* multithreaded */
 if(jt->sm==SMJAVA) jt->smoption |= SMOPTMTH;  /* assume java is multithreaded */
 if(SMOPTMTH&jt->smoption){
  jt->cstacktype = 2;
  jt->qtstackinit = (uintptr_t)&jt;
  if(jt->cstackmin)jt->cstackmin=(jt->cstackinit=jt->qtstackinit)-(CSTACKSIZE-CSTACKRESERVE);
 }else if(SMQT==jt->sm){
  jt->cstacktype = 1;
  jt->qtstackinit = (uintptr_t)callbacks[3];
  jt->smpoll = 0;
  jt->smoption = (~SMOPTPOLL) & jt->smoption;  /* smpoll not used */
  // If the user is giving us a better view of the stack through jt->qtstackinit, use it.  We get just the top-of-stack
  // address so we have to guess about the bottom, using our view of the minimum possible stack we have.
  // if cstackmin is 0, the user has turned off stack checking and we honor that decision
  if(jt->qtstackinit&&jt->cstackmin)jt->cstackmin=(jt->cstackinit=jt->qtstackinit)-(CSTACKSIZE-CSTACKRESERVE);
 }
}

/* set jclient callbacks from values - easier for nodejs */
void _stdcall JSMX(J jt, void* out, void* wd, void* in, void* poll, I opts)
{
 jt->smoutput = (outputtype)out;
 jt->smdowd = wd;
 jt->sminput = (inputtype)in;
 jt->smpoll = (polltype)poll;
 jt->sm = 0xff & opts;
 jt->qtstackinit = (SMQT==jt->sm) ? (uintptr_t)poll : 0;
 jt->smoption = ((~0xff) & (UI)opts) >> 8;
 if(jt->sm==SMJAVA) jt->smoption |= SMOPTMTH;  /* assume java is multithreaded */
 if(SMOPTMTH&jt->smoption){
  jt->cstacktype = 2;
  jt->qtstackinit = (uintptr_t)&jt;
  if(jt->cstackmin)jt->cstackmin=(jt->cstackinit=jt->qtstackinit)-(CSTACKSIZE-CSTACKRESERVE);
 }else if(SMQT==jt->sm){
  jt->cstacktype = 1;
  jt->qtstackinit = (uintptr_t)poll;
  jt->smpoll = 0;
  jt->smoption = (~SMOPTPOLL) & jt->smoption;  /* smpoll not used */
  if(jt->qtstackinit&&jt->cstackinit)jt->cstackmin=(jt->cstackinit=jt->qtstackinit)-(CSTACKSIZE-CSTACKRESERVE);
 }
}

// return pointer to string name of current locale, or 0 if error
C* _stdcall JGetLocale(J jt){
 A *old=jt->tnextpushp;  // set free-back-to point
 if(jt->iomalloc){FREE(jt->iomalloc); jt->malloctotal -= jt->iomalloclen; jt->iomalloc=0; jt->iomalloclen=0;}  // free old block if any
 C* z=getlocale(jt);  // get address of string to return
 if(jt->iomalloc=MALLOC(1+strlen(z))){jt->malloctotal += 1+strlen(z); jt->iomalloclen = 1+strlen(z); strcpy(jt->iomalloc,z); }  // allocate & copy, and account for its space
 tpop(old);  // free allocated blocks
 R jt->iomalloc;  // return pointer to string
}

A _stdcall Jga(J jt, I t, I n, I r, I*s){A z;
 RZ(z=ga(t, n, r, s));
 AC(z)=ACUC1;  // set nonrecursive usecount so that parser won't free the block prematurely.  This gives the usecount as if the block were 'assigned' by this call
 return z;
}

void oleoutput(J jt, I n, char* s); /* SY_WIN32 only */

#define capturesize 80000

/* jsto - display output in output window */
// type is mtyo of string, s->null-terminated string
void jsto(J jt,I type,C*s){C e;I ex;
 if(jt->nfe)
 {
  // here for Native Front End state, toggled by 15!:16
  // we execute the sentence:  type output_jfe_ s
  fauxblockINT(fauxtok,3,1); A tok; fauxBOXNR(tok,fauxtok,3,1);  // allocate 3-word sentence on stack, rank 1
  AAV1(tok)[0]=num(type); AAV1(tok)[1]=nfs(11,"output_jfe_"); AAV1(tok)[2]=cstr(s);  // the sentence to execute, tokenized
// obsolete   C q[]="0 output_jfe_ (15!:18)0";
// obsolete   q[0]+=(C)type;
// obsolete   jt->mtyostr=s;
  e=jt->jerr; ex=jt->etxn;   // save error state before running the output sentence
  jt->jerr=0; jt->etxn=0;
  jt->adbreakr=&breakdata;
// obsolete   exec1(cstr(q));
  parse(tok);  // run it, ignoring errors.  always reset jt->asgn after every execution has had its chance to type
// obsolete  jt->asgn=0;
  jt->adbreakr=jt->adbreak;
  jt->jerr=e; jt->etxn=ex; // restore
 }else{
  // Normal output.  Call the output routine
  if(jt->smoutput){((outputtype)(jt->smoutput))(jt,(int)type,s);R;} // JFE output
#if SY_WIN32 && !SY_WINCE && defined(OLECOM)
  if(jt->oleop && (type & MTYOFM)){oleoutput(jt,strlen(s),s);R;} // ole output
#endif
  // lazy - malloc failure will crash and should alloc larger when full
  if(!jt->capture){jt->capture=MALLOC(capturesize);jt->capture[0]=0;}
  if(capturesize>2+strlen(jt->capture)+strlen(s))
   strcat(jt->capture,s);
  else
   strcpy(jt->capture,"too much output ...\n");
 }
 R;
}

#if SYS&SYS_UNIX

C dll_initialized= 0; // dll init sets to 1

// dll init on load - eqivalent to windows DLLMAIN DLL_ATTACH_PROOCESS
__attribute__((constructor)) static void Initializer(int argc, char** argv, char** envp)
{
 J jtnobdy=malloc(sizeof(JST)+JTALIGNBDY-1);
 if(!jtnobdy) R;
 J jt = (J)(((I)jtnobdy+JTALIGNBDY-1)&-JTALIGNBDY);  // force to SDRAM page boundary
 memset(jt,0,sizeof(JST));
 if(!jtglobinit(jt)){free(jtnobdy); R;}
 dll_initialized= 1; jt->heap=(void *)jtnobdy;  // save allo address for later free
}

J JInit(void){
 if(!dll_initialized) R 0; // constructor failed
 J jtnobdy;
 RZ(jtnobdy=malloc(sizeof(JST)+JTALIGNBDY-1));
 J jt = (J)(((I)jtnobdy+JTALIGNBDY-1)&-JTALIGNBDY);  // force to SDRAM page boundary
 memset(jt,0,sizeof(JST));
 if(!jtjinit2(jt,0,0)){free(jtnobdy); R 0;};
 jt->heap=(void *)jtnobdy;  // save allo address for later free
 R jt;
}

// clean up at the end of a J instance
int JFree(J jt){
  if(!jt) R 0;
  breakclose(jt);
  jt->jerr=0; jt->etxn=0; /* clear old errors */
  if(jt->xep&&AN(jt->xep)){A *old=jt->tnextpushp; immex(jt->xep); fa(jt->xep); jt->xep=0; jt->jerr=0; jt->etxn=0; tpop(old); }  // run with typeout enabled
  dllquit(jt);  // clean up call dll
  free(jt->heap);  // free the initial allocation
  R 0;
}
#endif

F1(jtbreakfnq){
 ASSERTMTV(w);
 R cstr(jt->breakfn);
}

F1(jtbreakfns){A z;I *fh,*mh=0; void* ad;
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(!AN(w)||AT(w)&LIT,EVDOMAIN);
 ASSERT(AN(w)<NPATH,EVDOMAIN);
 w=str0(w);
 if(!strcmp(jt->breakfn,CAV(w))) R mtm;
 breakclose(jt);
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
 strcpy(jt->breakfn,CAV(w));
 jt->breakfh=fh;
 jt->breakmh=mh;
 jt->adbreakr=jt->adbreak=ad;
 R mtm;
}

int valid(C* psrc, C* psnk)
{
 while(*psrc == ' ') ++psrc;
 if(!isalpha(*psrc)) return EVILNAME;
 while(isalnum(*psrc) || *psrc=='_') *psnk++ = *psrc++;
 while(*psrc == ' ') ++psrc;
 if(*psrc) return EVILNAME;
 *psnk = 0;
 return 0;  
}

int _stdcall JGetM(J jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata)
{
 A a; char gn[256]; int z;
 A *old=jt->tnextpushp;
 if(strlen(name) >= sizeof(gn)){ jsignal(z=EVILNAME);
 }else if(valid(name, gn)){ jsignal(z=EVILNAME);
 }else if(!(a=symbrdlock(nfs(strlen(gn),gn)))){ jsignal(z=EVDOMAIN);
 }else if(FUNC&AT(a)){ jsignal(z=EVDOMAIN);
 }else{
  *jtype = AT(a);
  *jrank = AR(a);
  *jshape = (I)AS(a);
  *jdata = (I)AV(a);
  z=0;  // good return
 }
 tpop(old);
 return z;
}

static int setterm(J jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata)
{
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
 a = ga(*jtype, k, *jrank, (I*)*jshape);
 if(!a) return EVWSFULL;
 MC(AV(a), (void*)*jdata, n*k);
 jset(gn, a);
 return jt->jerr;
}

int _stdcall JSetM(J jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata)
{
 int er;

 PROLOG(0051);
 er = setterm(jt, name, jtype, jrank, jshape, jdata);
 tpop(_ttop);
 return er;
}

#define EDCBUSY -1
#define EDCEXE -2

C* esub(J jt, I ec)
{
 if(!ec) return "";
 if(ec == EDCBUSY) return "busy with previous input";
 if(ec == EDCEXE) return "not supported in EXE server";
 if(ec > NEVM || ec < 0) return "unknown error";
 return CAV(AAV(jt->evm)[ec]);
}

int _stdcall JErrorTextM(J jt, I ec, I* p)
{
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

