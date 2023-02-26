/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Initializations                                                         */

#ifdef _WIN32
#include <windows.h>
#else
#define __cdecl
#endif
#include "j.h"
#include "w.h"
#include "cpuinfo.h"

#if SYS & SYS_FREEBSD
#include <floatingpoint.h>
#endif


// create name block for xyuvmn.  flags is type flags to use, either 0 or NAMEBYVALUE
static A jtmakename(J jt,C*s,I flags){A z;I m;NM*zv;
 m=strlen(s);
 GATV0(z,NAME,m,1); AT(z)=NAME|flags; zv=NAV(z);  // Use GATV because GA doesn't support NAME type; but we must have NAMEBYVALUE set
 MC(zv->s,s,m); zv->s[m]=0;
 zv->m   =(UC)m; 
 zv->bucket=0;
 zv->bucketx=0;
 zv->flag=NMDOT;
 zv->hash=(UI4)nmhash(m,(UC*)&zv->s);  // use copy because hic may overread
 ACX(z);
 R z;
}

/* 
JE can be used by multiple tasks and threads
tasks are easier than threads
global memory is distinct between tasks but is shared between threads

JE support for multiple tasks is good

 global storage that changes after globinit is a bad bug waiting to happen

storage belonging to a task or thread MUST be be in the J structure
there are only a few globals that have storage not in J
*/

// globals 
JS gjt=0; // JPF debug - convenience debug single process - points to shared area bacause sto() uses it

// thread-safe/one-time initialization of all global constants
// Use GA for all these initializations, to save space since they're done only once
// The call gives us jt, but we initialize in the master thread
// The jt we are given is a throwaway, needed ONLY so we can allocate some A blocks here.  Anything stored
// into jt will never be used.  jinit3 will later be called with the real jt, to initialize it
B jtglobinit(JS jjt){A x,y;J jt=MTHREAD(jjt);  // initialize in master thread
 jtmeminits(jjt);
 jtmeminitt(jt);  // init allocation queues & tpop stack, master thread only
 RZ(mnuvxynam[0]=makename("m",0));
 RZ(mnuvxynam[1]=makename("n",0));
 RZ(mnuvxynam[2]=makename("u",NAMEBYVALUE));  // uv (and thus u. v.) must be defined when used in an explicit def.  Otherwise OK
 RZ(mnuvxynam[3]=makename("v",NAMEBYVALUE));
 RZ(mnuvxynam[4]=makename("x",0));
 RZ(mnuvxynam[5]=makename("y",0));
 // can be left at initial value v00[0]=v00[1]=0;   // vector 0 0, for rank
 pf=qpf();  // init performance monitor count info
 pinit();  // init block for a.
 jtsbtypeglobinit();  // init empty symbol
 cpuInit();  // get CPU characteristics
#if defined(__aarch64__)
 hwaes=(getCpuFeatures()&ARM_HWCAP_AES)?1:0;
#elif (defined(__i386__) || defined(_M_X64) || defined(__x86_64__))
 hwaes=((getCpuFeatures()&CPU_X86_FEATURE_SSE4_1)&&(getCpuFeatures()&CPU_X86_FEATURE_AES_NI))?1:0;
#endif
#if C_AVX && !defined(ANDROID)
 hwfma=(getCpuFeatures()&CPU_X86_FEATURE_FMA)?1:0;
#endif
 DO(IOTAVECLEN, iotavec[i]=i+IOTAVECBEGIN;)  // init our vector of ascending integers
 // We have no more use for the jt block.  The values allocated from it will never be freed.  We leave them extant,
 // but we can free the tpop stack that we allocated
 FREE(jt->tstackcurr);
 R 1;
}

static B jtevinit(JS jjt){A q,*v;JJ jt=MTHREAD(jjt);
 GA10(q,BOX,1+NEVM); v=AAV(q);
 DO(AN(q), v[i]=mtv;);
 v[EVALLOC  ]=INCORPNA(cstr("allocation error"           ));
 v[EVASSERT ]=INCORPNA(cstr("assertion failure"          ));
 v[EVATTN   ]=INCORPNA(cstr("attention interrupt"        ));
 v[EVBREAK  ]=INCORPNA(cstr("break"                      ));
 v[EVCTRL   ]=INCORPNA(cstr("control error"              ));
 v[EVDOMAIN ]=INCORPNA(cstr("domain error"               ));
 v[EVFACCESS]=INCORPNA(cstr("file access error"          ));
 v[EVFNAME  ]=INCORPNA(cstr("file name error"            ));
 v[EVFOLDLIMIT]=INCORPNA(cstr("fold limit"            ));
 v[EVFNUM   ]=INCORPNA(cstr("file number error"          ));
 v[EVILNAME ]=INCORPNA(cstr("ill-formed name"            ));
 v[EVILNUM  ]=INCORPNA(cstr("ill-formed number"          ));
 v[EVINDEX  ]=INCORPNA(cstr("index error"                ));
 v[EVINPRUPT]=INCORPNA(cstr("input interrupt"            ));
 v[EVFACE   ]=INCORPNA(cstr("interface error"            ));
 v[EVLENGTH ]=INCORPNA(cstr("length error"               ));
 v[EVLIMIT  ]=INCORPNA(cstr("limit error"                ));
 v[EVLOCALE ]=INCORPNA(cstr("locale error"               ));
 v[EVNAN    ]=INCORPNA(cstr("NaN error"                  ));
 v[EVNONCE  ]=INCORPNA(cstr("nonce error"                ));
 v[EVNONNOUN]=INCORPNA(cstr("noun result was required"   ));
 v[EVSPARSE ]=INCORPNA(cstr("non-unique sparse elements" ));
 v[EVOPENQ  ]=INCORPNA(cstr("open quote"                 ));
 v[EVWSFULL ]=INCORPNA(cstr("out of memory"              ));
 v[EVRANK   ]=INCORPNA(cstr("rank error"                 ));
 v[EVRO     ]=INCORPNA(cstr("read-only data"             ));
 v[EVSECURE ]=INCORPNA(cstr("security violation"         ));
 v[EVSPELL  ]=INCORPNA(cstr("spelling error"             ));
 v[EVSTACK  ]=INCORPNA(cstr("stack error"                ));
 v[EVSTOP   ]=INCORPNA(cstr("stop"                       ));
 v[EVSYNTAX ]=INCORPNA(cstr("syntax error"               ));
 v[EVSYSTEM ]=INCORPNA(cstr("system error"               ));
 v[EVTHROW  ]=INCORPNA(cstr("uncaught throw."            ));
 v[EVTIME   ]=INCORPNA(cstr("time limit"                 ));
 v[EVVALENCE]=INCORPNA(cstr("valence error"              ));
 v[EVVALUE  ]=INCORPNA(cstr("value error"                ));
 v[EVINHOMO  ]=v[EVDOMAIN];ras(v[EVINHOMO]);
 v[EVINDEXDUP  ]=v[EVINDEX];ras(v[EVINDEXDUP]);
 v[EVEMPTYT  ]=v[EVCTRL];ras(v[EVEMPTYT]);
 v[EVEMPTYDD  ]=v[EVCTRL];ras(v[EVEMPTYDD]);
 v[EVMISSINGGMP  ]=v[EVFACE];ras(v[EVFACE]);
 v[EVSIDAMAGE  ]=v[EVSTACK];ras(v[EVSTACK]);
 v[EVDEADLOCK  ]=INCORPNA(cstr("would deadlock"                ));
 v[EVASSEMBLY  ]=INCORPNA(cstr("domain error"               ));
 ACINITZAPRECUR(q,BOX); INITJT(jjt,evm)=q;   // q and its contents are not on tstack; this way the contents are freed on assignment
 if(jt->jerr){printf("evinit failed; error %hhi\n", jt->jerr); R 0;} else R 1;
}

/* static void sigflpe(int k){jsignal(EVDOMAIN); signal(SIGFPE,sigflpe);} */

static B jtconsinits(JS jjt){D y;JJ jt=MTHREAD(jjt);
// This is an initialization routine, so memory allocations performed here are NOT
// automatically freed by tpop()
#if AUDITCOMPILER
// verify that CTLZ works correctly, and that all calls to BP return what they used to
if (CTTZ(0x1000LL) != 12)*(I *)0 = 100;   // Create program check if error
if (CTTZZ(0x80000000LL) != 31)*(I *)1 = 101;   // Create program check if error
if (CTTZZ(0x100000002LL) != 1)*(I *)2 = 102;   // Create program check if error
if (CTTZZ(0x140000000LL) != 30)*(I *)3 = 103;   // Create program check if error
// verify that (I)x >> does sign-extension.  jtmult relies on that
if(((-1) >> 1) != -1)*(I *)4 = 104;
#endif

 INITJT(jjt,asgzomblevel) = 1;  // allow premature change to zombie names, but not data-dependent errors
 INITJT(jjt,assert) = 1;
 MC(INITJT(jjt,bx),"+++++++++|-",sizeof(INITJT(jjt,bx)));
 INITJT(jjt,disp)[0]=1; INITJT(jjt,disp)[1]=5;
 INITJT(jjt,outmaxafter)=222;
 INITJT(jjt,outmaxlen)=256;
 INITJT(jjt,retcomm)=1;
 INITJT(jjt,transposeflag)=1;
// INITJT(jjt,int64rflag)=0;
 MC(INITJT(jjt,baselocale),"base",sizeof(INITJT(jjt,baselocale)));   // establish value & hash of "base"
 INITJT(jjt,baselocalehash)=(UI4)nmhash(sizeof(INITJT(jjt,baselocale)),INITJT(jjt,baselocale));
  // Init for u./v.
 A uimp=ca(mnuvxynam[2]); NAV(uimp)->flag|=NMIMPLOC;  // create the name for u.
 INITJT(jjt,implocref)[0] = fdef(0,CTILDE,VERB,jtvalenceerr,jtvalenceerr, uimp,0L,0L, 0, RMAX,RMAX,RMAX); AC(INITJT(jjt,implocref)[0])=ACUC1;  //create 'u.'~, mark an not abandoned (no ra() needed)
 A vimp=ca(mnuvxynam[3]); NAV(vimp)->flag|=NMIMPLOC;
 INITJT(jjt,implocref)[1] = fdef(0,CTILDE,VERB,jtvalenceerr,jtvalenceerr, vimp,0L,0L, 0, RMAX,RMAX,RMAX); AC(INITJT(jjt,implocref)[1])=ACUC1;  //create 'v.'~

 INITJT(jjt,igemm_thres)=FLOATTOFLOAT16(IGEMM_THRES);   // tuning parameters for cip.c
 INITJT(jjt,dgemm_thres)=FLOATTOFLOAT16(DGEMM_THRES);
 INITJT(jjt,zgemm_thres)=FLOATTOFLOAT16(ZGEMM_THRES);
 INITJT(jjt,deprecex)=num(7);  // scaf suppress msg 7 for the nonce
 jt->cstackinit=(uintptr_t)&y;  // use a static variable to get the stack address
 jt->cstackmin=jt->cstackinit-(CSTACKSIZE-CSTACKRESERVE);
 MTHREAD(jjt)->threadpoolno=-1; // the master thread is in no pool, ever
 R 1;
}

static B jtconsinitt(J jt){
 RESETRANK;  // init both ranks to RMAX
 jt->ppn=6;  // default precision for printf
 jt->fcalln=NFCALL;
 jt->cct= 1.0-FUZZ;
 jt->xmode=XMEXACT;
 // create an initial stack, so that stack[-1] can be used for saving error messages
 jt->parserstackframe.parserstkbgn=jt->parserstackframe.parserstkend1=&jt->initparserstack[1];  // ensure valid error stack after final return
 R 1;
}

// initialise shared buffers
static B jtbufferinits(JS jjt){
 R !!(INITJT(jjt,breakfn)=calloc(1,NPATH)); // place to hold the break filename
}

// initialise thread-local buffers for thread threadno.  Requires synchronisation
B jtbufferinitt(J jt){
 RZ(jt->etxinfo=malloc(sizeof(ETXDATA)));  // error-message buffer
 RZ(jt->callstack=malloc(sizeof(LS)*(1+NFCALL)));  // function-call stack
 RZ(jt->rngdata=aligned_malloc(sizeof(RNG),CACHELINESIZE)); // place to hold RNG data, aligned to cacheline
 memset(jt->rngdata,0,sizeof(RNG));
 R 1;
}

// We have completed initial allocation.  Everything allocated so far will not be freed by a tpop, because
// tpop() isn't called during initialization.  So, to keep the memory auditor happy, we reset ttop so that it doesn't
// look like those symbols have a free outstanding.
// This also has the effect that buffers allocated during init do not need ra() to protect them, since they have no free outstanding
static B jtinitfinis(J jt){
 jt->tnextpushp=(A*)(((I)jt->tstackcurr+NTSTACKBLOCK)&(-NTSTACKBLOCK))+1;  // first store is to entry 1 of the first block
 R 1;
}

// Initialise thread-specific data for jt.  Idempotent.  Requires synchronisation.
C jtjinitt(J jt){
 if(jt->etxinfo)R 1; // already initialised; ok
 RZ(jtbufferinitt(jt));  // init thread-local buffers
 RZ(jtmeminitt(jt));
 RZ(jtconsinitt(jt));
 RZ(jtrnginit(jt)); // thread only
 RZ(jtecvtinit(jt));
 RZ(jtinitfinis(jt));
 R 1;}

// initialize the master thread for a new instance.  This fills in the JS block, which will remain
// for the duration of the instance.  It also fills in the JJ block for each thread
static C jtjinit3(JS jjt){S t;JJ jt=MTHREAD(jjt);
/* required for jdll and doesn't hurt others */
 gjt=jjt; // global jt for JPF debug
 jt->taskstate=0;  // The master thread is non-running when it is idle, so that system lock doesn't wait for it
 
#if (SYS & SYS_DOS)
 t=EM_ZERODIVIDE+EM_INVALID; _controlfp(t,t);
#endif
#if (SYS & SYS_OS2)
 t=EM_ZERODIVIDE+EM_INVALID+EM_OVERFLOW+EM_UNDERFLOW; _control87(t,t);
#endif
#if (SYS & SYS_FREEBSD)
 fpsetmask(0);
#endif
 INITJT(jjt,tssbase)=tod();  // starting time for all threads
#if PYXES
 INITJT(jjt,jobqueue)=aligned_malloc(sizeof(JOBQ[MAXTHREADPOOLS]),CACHELINESIZE); // job queue, cache-line aligned
 memset(INITJT(jjt,jobqueue),0,sizeof(JOBQ[MAXTHREADPOOLS]));
 DO(MAXTHREADPOOLS, (*INITJT(jjt,jobqueue))[i].ht[1]=(JOB *)&(*INITJT(jjt,jobqueue))[i].ht[1];)  // when q is empty, tail points to itself, as a safe NOP store
#endif
// only crashing on startup INITJT(jjt,peekdata)=1;  // wake up auditing
 // Initialize subsystems in order.  Each initializes all threads, if there are thread variables
 RZ(jtbufferinits(jjt)); // init the buffers pointed to by jjt
 RZ(jtbufferinitt(jt));  // init thread-local buffers
 RZ(jtmeminits(jjt));
 RZ(jtmeminitt(jt));
 RZ(jtsesminit(jjt,1));  // master only.  scaf pointless?
 RZ(jtcdinit(jjt));  // master only
 RZ(jtevinit(jjt));  // master only
 RZ(jtconsinits(jjt));
 RZ(jtconsinitt(jt));
 RZ(jtxsinit(jjt));  // must be before symbinit  master only
 RZ(jtsymbinit(jjt));  // must be after consinit   master only - global/locsyms must init at start of op
 RZ(jtxoinit(jjt));  // master only
 RZ(jtsbtypeinit(jjt));  // master only
 RZ(jtrnginit(jt));
// #if (SYS & SYS_DOS+SYS_MACINTOSH+SYS_UNIX)
#if (SYS & SYS_DOS+SYS_MACINTOSH)
 RZ(jtxlinit(jjt));  // file info, master only
#endif
 RZ(jtecvtinit(jt));
 RZ(jtinitfinis(jt));
 R 1;
}

// Here we initialize the new jt for a new J instance.
C jtjinit2(JS jt,int dummy0,C**dummy1){INITJT(jt,sesm)=1; R jinit3();}
