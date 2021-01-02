/* Copyright 1990-2017, Jsoftware Inc.  All rights reserved.               */
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
#include "m.h"
#include "cpuinfo.h"

#if SYS & SYS_FREEBSD
#include <floatingpoint.h>
#endif


// create name block for xyuvmn
static A jtmakename(J jt,C*s){A z;I m;NM*zv;
 m=strlen(s);
 GATV0(z,NAME,m,1); zv=NAV(z);  // Use GATV because GA doesn't support NAME type
 MC(zv->s,s,m); zv->s[m]=0;
 zv->m   =(UC)m; 
 zv->bucket=0;
 zv->bucketx=0;
 zv->flag=NMDOT+NMXY;
 zv->hash=(UI4)nmhash(m,s);
 ACX(z);
 R z;
}

/* 
JE can be used by multiple tasks and threads
tasks are easier than threads
global memory is distinct between tasks but is shared between threads

JE support for multiple tasks is good

JE support for threads has a few problems
 there are a few global constants not handled in globinit
 they need to be found and sorted out

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
// obsolete  MC(jt->typesizes,typesizes,sizeof(jt->typesizes));  // required for ma.  Repeated for each thread in jtinit3
// obsolete  MC(jt->typepriority,typepriority,sizeof(jt->typepriority));  // may not be needed
// obsolete MC(jt->prioritytype,prioritytype,sizeof(jt->prioritytype));  // may not be needed
 jtmeminit(jjt,1);  // init allocation queues & tpop stack, master thread only
 RZ(y=rifvs(str(1L,"z")));     ACX(y); AS(y)[0]=BUCKETXLOC(1,"z");   // for paths, the shape holds the bucketx
 GA(x,BOX, 1,1,0     ); ACX(x); AAV(x)[0]=y;                zpath      =x;  AFLAG(zpath) |= (AT(zpath)&TRAVERSIBLE);  // ensure that traversible types in pst are marked traversible, so tpush/ra/fa will not recur on them
 RZ(mnuvxynam[0]=makename("m"));
 RZ(mnuvxynam[1]=makename("n"));
 RZ(mnuvxynam[2]=makename("u"));
 RZ(mnuvxynam[3]=makename("v"));
 RZ(mnuvxynam[4]=makename("x"));
 RZ(mnuvxynam[5]=makename("y"));
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

static B jtevinit(JS jjt,I nthreads){A q,*v;JJ jt=MTHREAD(jjt);
 GA(q,BOX,1+NEVM,1,0); v=AAV(q);
 DO(AN(q), v[i]=mtv;);
 v[EVALLOC  ]=cstr("allocation error"           );
 v[EVASSERT ]=cstr("assertion failure"          );
 v[EVATTN   ]=cstr("attention interrupt"        );
 v[EVBREAK  ]=cstr("break"                      );
 v[EVCTRL   ]=cstr("control error"              );
 v[EVDOMAIN ]=cstr("domain error"               );
 v[EVFACCESS]=cstr("file access error"          );
 v[EVFNAME  ]=cstr("file name error"            );
 v[EVFOLDLIMIT]=cstr("fold limit"            );
 v[EVFNUM   ]=cstr("file number error"          );
 v[EVILNAME ]=cstr("ill-formed name"            );
 v[EVILNUM  ]=cstr("ill-formed number"          );
 v[EVINDEX  ]=cstr("index error"                );
 v[EVINPRUPT]=cstr("input interrupt"            );
 v[EVFACE   ]=cstr("interface error"            );
 v[EVLENGTH ]=cstr("length error"               );
 v[EVLIMIT  ]=cstr("limit error"                );
 v[EVLOCALE ]=cstr("locale error"               );
 v[EVNAN    ]=cstr("NaN error"                  );
 v[EVNONCE  ]=cstr("nonce error"                );
 v[EVNONNOUN]=cstr("noun result was required"   );
 v[EVSPARSE ]=cstr("non-unique sparse elements" );
 v[EVOPENQ  ]=cstr("open quote"                 );
 v[EVWSFULL ]=cstr("out of memory"              );
 v[EVRANK   ]=cstr("rank error"                 );
 v[EVRO     ]=cstr("read-only data"             );
 v[EVSECURE ]=cstr("security violation"         );
 v[EVSPELL  ]=cstr("spelling error"             );
 v[EVSTACK  ]=cstr("stack error"                );
 v[EVSTOP   ]=cstr("stop"                       );
 v[EVSYNTAX ]=cstr("syntax error"               );
 v[EVSYSTEM ]=cstr("system error"               );
 v[EVTHROW  ]=cstr("uncaught throw."            );
 v[EVTIME   ]=cstr("time limit"                 );
 v[EVVALUE  ]=cstr("value error"                );
 ras(q); INITJT(jjt,evm)=q;
 if(jt->jerr){printf("evinit failed; error %hhi\n", jt->jerr); R 0;} else R 1;
}

/* static void sigflpe(int k){jsignal(EVDOMAIN); signal(SIGFPE,sigflpe);} */

static B jtconsinit(JS jjt,I nthreads){D y;JJ jt=MTHREAD(jjt);
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
// obsolete  RZ(INITJT(jjt,bx)a=cstr("+++++++++|-")); INITJT(jjt,bx)=CAV(INITJT(jjt,bx)a);
 MC(INITJT(jjt,bx),"+++++++++|-",sizeof(INITJT(jjt,bx)));
// obsolete  jt->cctdefault=
 INITJT(jjt,disp)[0]=1; INITJT(jjt,disp)[1]=5;
 INITJT(jjt,outmaxafter)=222;
 INITJT(jjt,outmaxlen)=256;
// obsolete strcpy(jt->outseq,"\x0a");
 INITJT(jjt,retcomm)=1;
// obsolete  jt->tostdout=1;
 INITJT(jjt,transposeflag)=1;
// INITJT(jjt,int64rflag)=0;
 MC(INITJT(jjt,baselocale),"base",sizeof(INITJT(jjt,baselocale)));   // establish value & hash of "base"
 INITJT(jjt,baselocalehash)=(UI4)nmhash(sizeof(INITJT(jjt,baselocale)),INITJT(jjt,baselocale));
  // Init for u./v.
 A uimp=ca(mnuvxynam[2]); NAV(uimp)->flag|=NMIMPLOC;  // create the name for u.
 INITJT(jjt,implocref)[0] = fdef(0,CTILDE,VERB, 0,0, uimp,0L,0L, 0, RMAX,RMAX,RMAX); AC(INITJT(jjt,implocref)[0])=ACUC1;  //create 'u.'~, mark an not abandoned (no ra() needed)
 A vimp=ca(mnuvxynam[3]); NAV(vimp)->flag|=NMIMPLOC;
 INITJT(jjt,implocref)[1] = fdef(0,CTILDE,VERB, 0,0, vimp,0L,0L, 0, RMAX,RMAX,RMAX); AC(INITJT(jjt,implocref)[1])=ACUC1;  //create 'v.'~

 INITJT(jjt,igemm_thres)=IGEMM_THRES;   // tuning parameters for cip.c
 INITJT(jjt,dgemm_thres)=DGEMM_THRES;
 INITJT(jjt,zgemm_thres)=ZGEMM_THRES;
 I threadno; for(threadno=0;threadno<nthreads;++threadno){jt=&jjt->threaddata[threadno];
  RESETRANK;  // init both ranks to RMAX
  strcpy(jt->pp,"%0.6g");
  jt->fcalln=NFCALL;
  jt->cct= 1.0-FUZZ;
#if USECSTACK
  jt->cstackinit=(uintptr_t)&y;  // use a static variable to get the stack address
  jt->cstackmin=jt->cstackinit-(CSTACKSIZE-CSTACKRESERVE);
#else
  jt->fdepn=NFDEP;
#endif
  jt->xmode=XMEXACT;
 }
 R 1;
}

static B jtbufferinit(JS jjt,I nthreads){
 INITJT(jjt,breakfn)=malloc(NPATH); memset(INITJT(jjt,breakfn),0,NPATH);  // place to hold the break filename
 I threadno; for(threadno=0;threadno<nthreads;++threadno){JJ jt=&jjt->threaddata[threadno];
  jt->etx=malloc(1+NETX);  // error-message buffer
  jt->callstack=(LS *)malloc(sizeof(LS)*(1+NFCALL));  // function-call stack
  jt->rngdata=(RNG*)(((I)malloc(sizeof(RNG)+CACHELINESIZE)+CACHELINESIZE-1)&-CACHELINESIZE); memset(jt->rngdata,0,sizeof(RNG));  // place to hold RNG data, aligned to cacheline
 }
 R 1;
}

 // We have completed initial allocation.  Everything allocated so far will not be freed by a tpop, because
 // tpop() isn't called during initialization.  So, to keep the memory auditor happy, we reset ttop so that it doesn't
 // look like those symbols have a free outstanding.
 // This also has the effect that buffers allocated during init do not need ra() to protect them, since they have no free outstanding
static B jtinitfinis(JS jjt,I nthreads){
 I threadno; for(threadno=0;threadno<nthreads;++threadno){JJ jt=&jjt->threaddata[threadno];
  jt->tnextpushp=(A*)(((I)jt->tstackcurr+NTSTACKBLOCK)&(-NTSTACKBLOCK))+1;  // first store is to entry 1 of the first block
 }
 R 1;
}


// initialize the master thread for a new instance.  This fills in the JS block, which will remain
// for the duration of the instance.  It also fills in the JJ block for each thread
static C jtjinit3(JS jjt){S t;
/* required for jdll and doesn't hurt others */
 gjt=jjt; // global jt for JPF debug
 
// obsolete MC(jt->typesizes,typesizes,sizeof(jt->typesizes));  // required for ma.
// obsolete  MC(jt->typepriority,typepriority,sizeof(jt->typepriority));  // required for ma.  Repeated for each thread in jtinit3
// obsolete  MC(jt->prioritytype,prioritytype,sizeof(jt->prioritytype));  // required for ma.  Repeated for each thread in jtinit3
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
// obsolete  jt->prxthornuni=0;  // init to non-unicode (normal) state
// obsolete  jt->jprx=0;      // init to non jprx jconsole output (normal) state
 // Initialize subsystems in order.  Each initializes all threads, if there are thread variables
 RZ(jtbufferinit(jjt,MAXTHREADS)); // init the buffers pointed to by jjt
 RZ(jtmeminit(jjt,MAXTHREADS));
 RZ(jtsesminit(jjt,MAXTHREADS));  // master only
 RZ(jtevinit(jjt,MAXTHREADS));  // master only
 RZ(jtconsinit(jjt,MAXTHREADS));
 RZ(jtxsinit(jjt,MAXTHREADS));  // must be before symbinit  master only
 RZ(jtsymbinit(jjt,MAXTHREADS));  // must be after consinit   master only - global/locsyms must init at start of op
 RZ(jtparseinit(jjt,MAXTHREADS));
 RZ(jtxoinit(jjt,MAXTHREADS));  // master only
 RZ(jtsbtypeinit(jjt,MAXTHREADS));  // master only
 RZ(jtrnginit(jjt,MAXTHREADS));
// #if (SYS & SYS_DOS+SYS_MACINTOSH+SYS_UNIX)
#if (SYS & SYS_DOS+SYS_MACINTOSH)
 RZ(jtxlinit(jjt,MAXTHREADS));  // file info, master only
#endif
 RZ(jtecvtinit(jjt,MAXTHREADS));
 RZ(jtinitfinis(jjt,MAXTHREADS));
 R 1;
}

// Here we initialize the new jt for a new J instance.
C jtjinit2(JS jt,int dummy0,C**dummy1){INITJT(jt,sesm)=1; R jinit3();}



/* unused cpuInfo

#if 0   // Now we detect architecture at installation time, using C_AVX
 // See if processor supports AVX instructions
 // Tip o' hat to InsufficientlyComplicated and the commenter
 // Checking for AVX requires 3 things:
 // 1) CPUID indicates that the OS uses XSAVE and XRSTORE
 //     instructions (allowing saving YMM registers on context
 //     switch)
 // 2) CPUID indicates support for AVX
 // 3) XGETBV indicates the AVX registers will be saved and
 //     restored on context switch
 //
 // Note that XGETBV is only available on 686 or later CPUs, so
 // the instruction needs to be conditionally run.
 int cpuInfo[4];
 __cpuid(cpuInfo, 1);
 
 I osUsesXSAVE_XRSTORE = cpuInfo[2] & (1L << 27);
 I cpuAVXSuport = cpuInfo[2] & (1L << 28);
 
 if (osUsesXSAVE_XRSTORE && cpuAVXSuport)
 {
  // Check if the OS will save the YMM registers
  unsigned long long xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
  jt->cpuarchavx = (xcrFeatureMask & 0x6) == 0x6;
 }
#endif
*/

