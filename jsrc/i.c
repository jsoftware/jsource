/* Copyright 1990-2017, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Initializations                                                         */

#include "j.h"
#include "w.h"
#include "cpuinfo.h"

#if SYS & SYS_FREEBSD
#include <floatingpoint.h>
#endif


// create name block for xyuvmn
static A jtmakename(J jt,C*s){A z;I m;NM*zv;
 m=strlen(s);
 GATV0(z,NAME,m,1); zv=NAV(z);  // Use GATV because GA doesn't support NAME type
 MC(zv->s,s,m); *(m+zv->s)=0;
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
 meminit();  // init allocation queues & tpop stack
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

 R 1;
}

static B jtevinit(J jt){A q,*v;
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
 ras(q); JT(jt,evm)=q;
 if(jt->jerr){printf("evinit failed; error %hhi\n", jt->jerr); R 0;} else R 1;
}

/* static void sigflpe(int k){jsignal(EVDOMAIN); signal(SIGFPE,sigflpe);} */

static B jtconsinit(J jt){D y;
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
JT(jt,asgzomblevel) = 1;  // allow premature change to zombie names, but not data-dependent errors
JT(jt,assert) = 1;
// obsolete  RZ(JT(jt,bx)a=cstr("+++++++++|-")); JT(jt,bx)=CAV(JT(jt,bx)a);
 MC(JT(jt,bx),"+++++++++|-",sizeof(JT(jt,bx)));
// obsolete  jt->cctdefault=
 jt->cct= 1.0-FUZZ;
 JT(jt,disp)[0]=1; JT(jt,disp)[1]=5;
 jt->fcalln=NFCALL;
#if USECSTACK
 jt->cstackinit=(uintptr_t)&y;  // use a static variable to get the stack address
 jt->cstackmin=jt->cstackinit-(CSTACKSIZE-CSTACKRESERVE);
#else
 jt->fdepn=NFDEP;
#endif
 JT(jt,outmaxafter)=222;
 JT(jt,outmaxlen)=256;
// obsolete strcpy(jt->outseq,"\x0a");
 strcpy(jt->pp,"%0.6g");
 JT(jt,retcomm)=1;
// obsolete  jt->tostdout=1;
 JT(jt,transposeflag)=1;
// JT(jt,int64rflag)=0;
 jt->xmode=XMEXACT;
 MC(JT(jt,baselocale),"base",sizeof(JT(jt,baselocale)));   // establish value & hash of "base"
 JT(jt,baselocalehash)=(UI4)nmhash(sizeof(JT(jt,baselocale)),JT(jt,baselocale));
 RESETRANK;  // init both ranks to RMAX
  // Init for u./v.
 A uimp=ca(mnuvxynam[2]); NAV(uimp)->flag|=NMIMPLOC;  // create the name for u.
 JT(jt,implocref)[0] = fdef(0,CTILDE,VERB, 0,0, uimp,0L,0L, 0, RMAX,RMAX,RMAX);  //create 'u.'~
 A vimp=ca(mnuvxynam[3]); NAV(vimp)->flag|=NMIMPLOC;
 JT(jt,implocref)[1] = fdef(0,CTILDE,VERB, 0,0, vimp,0L,0L, 0, RMAX,RMAX,RMAX);  //create 'v.'~

 JT(jt,igemm_thres)=IGEMM_THRES;   // tuning parameters for cip.c
 JT(jt,dgemm_thres)=DGEMM_THRES;
 JT(jt,zgemm_thres)=ZGEMM_THRES;
 R 1;
}

// initialize the master thread for a new instance.  This fills in the jt block, which will remain
// for the duration of the instance
static C jtjinit3(J jt){S t;
/* required for jdll and doesn't hurt others */
 gjt=JJTOJ(jt); // global jt for JPF debug
  // init the buffers pointed to by jt
 jt->etx=malloc(1+NETX);  // error-message buffer
 jt->callstack=(LS *)malloc(sizeof(LS)*(1+NFCALL));  // function-call stack
 JT(jt,breakfn)=malloc(NPATH); memset(JT(jt,breakfn),0,NPATH);  // place to hold the break filename
 jt->rngdata=(RNG*)(((I)malloc(sizeof(RNG)+CACHELINESIZE)+CACHELINESIZE-1)&-CACHELINESIZE); memset(jt->rngdata,0,sizeof(RNG));  // place to hold RNG data, aligned to cacheline

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
 JT(jt,tssbase)=tod();
// obsolete  jt->prxthornuni=0;  // init to non-unicode (normal) state
// obsolete  jt->jprx=0;      // init to non jprx jconsole output (normal) state
 meminit();
 sesminit();
 evinit();
 consinit();
 xsinit();  // must be before symbinit
 symbinit();  // must be after consinit
 parseinit();
 xoinit();
 sbtypeinit();
 rnginit();
// #if (SYS & SYS_DOS+SYS_MACINTOSH+SYS_UNIX)
#if (SYS & SYS_DOS+SYS_MACINTOSH)
 xlinit();
#endif
 jtecvtinit(jt);
 // We have completed initial allocation.  Everything allocated so far will not be freed by a tpop, because
 // tpop() isn't called during initialization.  So, to keep the memory auditor happy, we reset ttop so that it doesn't
 // look like those symbols have a free outstanding.
 jt->tnextpushp=(A*)(((I)jt->tstackcurr+NTSTACKBLOCK)&(-NTSTACKBLOCK))+1;  // first store is to entry 1 of the first block
 R !jt->jerr;
}

// Here we initialize the new jt for a new J instance.  We do so in the master thread.
C jtjinit2(JS jjt,int dummy0,C**dummy1){J jt=MTHREAD(jjt); JT(jt,sesm)=1; R jinit3();}



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

