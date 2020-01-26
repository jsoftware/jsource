/* Copyright 1990-2017, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Initializations                                                         */

#ifdef _WIN32
#include <windows.h>
#endif
#include "j.h"
#include "w.h"
#include "cpuinfo.h"

#if SYS & SYS_FREEBSD
#include <floatingpoint.h>
#endif


// These statics get copied into jt for cache footprint.  If you change them,
// change the definition in jt.h

// For each Type, the length of a data-item of that type.  The order
// here is by number of trailing 0s in the (32-bit) type; aka the bit-number index.
// Example: LITX is 1, so location 1 contains sizeof(C)
static UC typesizes[] = {
B01SIZE, LITSIZE, INTSIZE, FLSIZE, CMPXSIZE, BOXSIZE, XNUMSIZE, RATSIZE,
-1,           -1, SB01SIZE, SLITSIZE, SINTSIZE, SFLSIZE, SCMPXSIZE, SBOXSIZE,
SBTSIZE, C2TSIZE, C4TSIZE, XDSIZE, XZSIZE, ASGNSIZE, MARKSIZE, NAMESIZE,
SYMBSIZE, CONWSIZE, LPARSIZE, INTSIZE, INTSIZE, INTSIZE, RPARSIZE,-1,  // note ACV sizes are in INTs
};

// Priority is
// B01 LIT C2T C4T INT BOX XNUM RAT SBT FL CMPX
// For sparse types, we encode here the corresponding dense type
static UC typepriority[] = {   // convert type bit to priority
0, 1, 4, 9, 10, 5, 6, 7,  // B01-RAT
0, 0, 0, 1, 4, 9, 10, 5,  // x x SB01-SBOX
8, 2, 3};  // SBT C2T C4T
static UC prioritytype[] = {  // Convert priority to type bit
B01X, LITX, C2TX, C4TX, INTX, BOXX, XNUMX, RATX, SBTX, FLX, CMPXX};

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

JE support for threads has problems
 unix globinit is not thread-safe
 there are a few globals not handled in globinit or that
  are not constants and they need to be found and sorted out
 global storage that changes after globinit is a bad bug waiting to happen

storage belonging to a task or thread should be be rooted in the J structure
there are only a few globals that have storage not in J

global storage should be initialized in globinit()
this is thread-safe in windows - called from dllmain
not currently thread-safe in unix, but could be (at least is all in one spot)
*/

// globals 
J gjt=0; // JPF debug - convenience debug single process

// thread-safe/one-time initialization of all global constants
// Use GA for all these initializations, to save space since they're done only once
B jtglobinit(J jt){A x,y;D*d;A *oldpushx=jt->tnextpushp;
 liln=1&&C_LE;
 MC(jt->typesizes,typesizes,sizeof(jt->typesizes));  // required for ma.  Repeated for each thread in jtinit3
 MC(jt->typepriority,typepriority,sizeof(jt->typepriority));  // may not be needed
 MC(jt->prioritytype,prioritytype,sizeof(jt->prioritytype));  // may not be needed
 jt->adbreakr=jt->adbreak=&breakdata; /* required for ma to work */
 meminit();  /* required for ma to work */
 MC(&inf, XINF,SZD); 
 MC(&jnan,XNAN,SZD);
 infm=-inf;
 DO(-NUMMIN, GA(x,INT,1,0,0); ACX(x); * AV(x)=i+NUMMIN;   num[i+NUMMIN]   =x;);
 DO(NUMMAX-1, GA(x,INT,1,0,0); ACX(x); * AV(x)=i+2;       num[i+2]   =x;);
 DO(sizeof(numvr)/sizeof(numvr[0]), GA(x,FL,1,0,0); ACX(x); *DAV(x)=(D)i;    numvr[i]   =x;);
 GA(x,B01, 1,0,0     ); ACX(x); *BAV(x)=0;                num[0]=x;
 GA(x,B01, 1,0,0     ); ACX(x); *BAV(x)=1;                num[1]=x;
 memset(chr,C0,256*SZI);
 GA(x,LIT, 1,0,0     ); ACX(x); *CAV(x)=' ';              chr[' '   ]=x;
 GA(x,LIT, 1,0,0     ); ACX(x); *CAV(x)=':';              chr[':'   ]=x;
 GA(x,LIT, 1,0,0     ); ACX(x); *CAV(x)='/';              chr['/'   ]=x;
 GA(x,LIT, 1,0,0     ); ACX(x); *CAV(x)='\\';             chr['\\'  ]=x;
 GA(x,LIT, 1,0,0     ); ACX(x); *CAV(x)=CQUOTE;           chr[CQUOTE]=x;
 GA(x,B01, 0,1,0     ); ACX(x);                           mtv        =x;
 GA(x,LIT, 0,1,0     ); ACX(x);                           aqq        =x;
 GA(x,INT, 1,1,0     ); ACX(x); * AV(x)=0;                iv0=x;
 GA(x,INT, 1,1,0     ); ACX(x); * AV(x)=1;                iv1=x;
 GA(x,INT, 1,0,0     ); ACX(x); * AV(x)=IMAX;             imax=x;
 GA(x,INT, 1,0,0     ); ACX(x); * AV(x)=0;                zeroionei[0]=x;
 GA(x,INT, 1,0,0     ); ACX(x); * AV(x)=1;                zeroionei[1]=x;
 GA(x,FL,  1,0,0     ); ACX(x); *DAV(x)=inf;              ainf       =x;
 GA(x,FL,  1,0,0     ); ACX(x); *DAV(x)=PI;               pie        =x;
 GA(x,FL,  1,0,0     ); ACX(x); *DAV(x)=0.5;              onehalf    =x;
 GA(x,MARK,1,0,0     ); ACX(x); * AV(x)=0;                mark       =x; 
 GA(x,B01, 0,2,&zeroZ); ACX(x);                           mtm        =x;
 GA(x,CMPX,1,0,0     ); ACX(x); d=DAV(x); *d=0; *(1+d)=1; a0j1       =x;
 RZ(y=rifvs(str(1L,"z")));     ACX(y); AS(y)[0]=BUCKETXLOC(1,"z");   // for paths, the shape holds the bucketx
 GA(x,BOX, 1,1,0     ); ACX(x); *AAV(x)=y;                zpath      =x;  AFLAG(zpath) |= (AT(zpath)&TRAVERSIBLE);  // ensure that traversible types in pst are marked traversible, so tpush/ra/fa will not recur on them
 GA(x,ASGN+ASGNLOCAL+ASGNTONAME, 1,1,0     ); ACX(x); *CAV(x)=CASGN; asgnlocsimp=x;
 GA(x,ASGN+ASGNTONAME, 1,1,0     ); ACX(x); *CAV(x)=CGASGN; asgngloname=x;
 GA(x,ASGN+ASGNTONAME, 1,1,0     ); ACX(x); *CAV(x)=CASGN; asgnforcegloname=x;  // =. converted to global+NAME
 GA(x,ASGN, 1,1,0     ); ACX(x); *CAV(x)=CASGN; asgnforceglo=x;  // =. converted to global
 RZ(mnuvxynam[0]=makename("m"));
 RZ(mnuvxynam[1]=makename("n"));
 RZ(mnuvxynam[2]=makename("u"));
 RZ(mnuvxynam[3]=makename("v"));
 RZ(mnuvxynam[4]=makename("x"));
 RZ(mnuvxynam[5]=makename("y"));
 zeroQ.n =iv0; zeroQ.d =iv1;
 zeroDX.e=0;     zeroDX.x=iv0;
 memset(minus0,C0,8L); minus0[C_LE?7:0]='\200';
 // can be left at initial value v00[0]=v00[1]=0;   // vector 0 0, for rank
 pf=qpf();
 pinit();

 cpuInit();
#if defined(__aarch64__)
 hwaes=(getCpuFeatures()&ARM_HWCAP_AES)?1:0;
#elif (defined(__i386__) || defined(_M_X64) || defined(__x86_64__))
 hwaes=((getCpuFeatures()&CPU_X86_FEATURE_SSE4_1)&&(getCpuFeatures()&CPU_X86_FEATURE_AES_NI))?1:0;
#endif
#if C_AVX && !defined(ANDROID)
 hwfma=(getCpuFeatures()&CPU_X86_FEATURE_FMA)?1:0;
#endif
 // take all the permanent blocks off the tpop stack so that we don't decrement their usecount.  All blocks allocated here must be permanent
 jt->tnextpushp=oldpushx;
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
 ras(q); jt->evm=q;
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
jt->asgzomblevel = 1;  // allow premature change to zombie names, but not data-dependent errors
jt->assert = 1;
 RZ(jt->bxa=cstr("+++++++++|-")); jt->bx=CAV(jt->bxa);
 jt->cctdefault=jt->cct= 1.0-FUZZ; jt->fuzz=FUZZ;
 jt->disp[0]=1; jt->disp[1]=5;
 jt->fcalln=NFCALL;
#if USECSTACK
 jt->cstackinit=(uintptr_t)&y;  // use a static variable to get the stack address
 jt->cstackmin=jt->cstackinit-(CSTACKSIZE-CSTACKRESERVE);
#else
 jt->fdepn=NFDEP;
#endif
 jt->outmaxafter=222;
 jt->outmaxlen=256;
 strcpy(jt->outseq,"\x0a");
 strcpy(jt->pp,"%0.6g");
 jt->retcomm=1;
 jt->tostdout=1;
 jt->transposeflag=1;
// jt->int64rflag=0;
 jt->xmode=XMEXACT;
 MC(jt->baselocale,"base",sizeof(jt->baselocale));   // establish value & hash of "base"
 jt->baselocalehash=(UI4)nmhash(sizeof(jt->baselocale),jt->baselocale);
 RESETRANK;  // init both ranks to RMAX
 DO(IOTAVECLEN, jt->iotavec[i]=i+IOTAVECBEGIN;)  // init our vector of ascending integers
#if C_AVX&&SY_64
 memset(&jt->validitymask[0],-1,4*sizeof(I)); memset(&jt->validitymask[4],0,4*sizeof(I));  // -1, -1, -1, -1, 0, 0, 0, 0   used to prepare for mask load/store
#endif
  // Init for u./v.
 A uimp=ca(mnuvxynam[2]); NAV(uimp)->flag|=NMIMPLOC;  // create the name for u.
 jt->implocref[0] = fdef(0,CTILDE,VERB, 0,0, uimp,0L,0L, 0, RMAX,RMAX,RMAX);  //create 'u.'~
 A vimp=ca(mnuvxynam[3]); NAV(vimp)->flag|=NMIMPLOC;
 jt->implocref[1] = fdef(0,CTILDE,VERB, 0,0, vimp,0L,0L, 0, RMAX,RMAX,RMAX);  //create 'v.'~

 jt->igemm_thres=IGEMM_THRES;   // tuning parameters for cip.c
 jt->dgemm_thres=DGEMM_THRES;
 jt->zgemm_thres=ZGEMM_THRES;
 R 1;
}

static C jtjinit3(J jt){S t;
#if defined(USE_THREAD)
#ifdef _WIN32
 if ((jt->plock=(I)CreateMutex(NULL,FALSE,NULL)) == 0) {
  VLOGFD("jt %p mutex init failed rc %u\n",jt,GetLastError());
  return 0;
 }
#else
 int rc;
 if ((rc=pthread_mutex_init(&jt->plock, NULL)) != 0) {
  VLOGFD("jt %p mutex init failed rc %d\n",jt,rc);
  return 0;
 }
#endif
 jt->ptid=-1;
#endif
/* required for jdll and doesn't hurt others */
 gjt=jt; // global jt for JPF debug
 MC(jt->typesizes,typesizes,sizeof(jt->typesizes));  // required for ma.
 MC(jt->typepriority,typepriority,sizeof(jt->typepriority));  // required for ma.  Repeated for each thread in jtinit3
 MC(jt->prioritytype,prioritytype,sizeof(jt->prioritytype));  // required for ma.  Repeated for each thread in jtinit3
#if (SYS & SYS_DOS)
 t=EM_ZERODIVIDE+EM_INVALID; _controlfp(t,t);
#endif
#if (SYS & SYS_OS2)
 t=EM_ZERODIVIDE+EM_INVALID+EM_OVERFLOW+EM_UNDERFLOW; _control87(t,t);
#endif
#if (SYS & SYS_FREEBSD)
 fpsetmask(0);
#endif
 jt->tssbase=tod();
 jt->thornuni=0;  // init to non-unicode (normal) state
 jt->jprx=0;      // init to non jprx jconsole output (normal) state
 meminit();
// extern void * HeapAlloc();
 sesminit();
 evinit();
 consinit();
 symbinit();  // must be after consinit
 parseinit();
 xoinit();
 xsinit();
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

C jtjinit2(J jt,int dummy0,C**dummy1){jt->sesm=1; R jinit3();}



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

