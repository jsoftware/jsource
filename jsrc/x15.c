/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: DLL call driver                                                  */

// TODO: remove idiv from hashing here

/*
switchcall builds all the required dll calls.
The dll call stack frame is built entirely with I values.
All the int types (char, unicode, short etc) are stacked as I valued.
Pointers are stacked as I values.
Short floats are stacked as I values.
Doubles are stacked as I values (2 for J32 and 1 for J64).

This gives a fairly good and easy way that is portable across platforms.

But some architectures don't pass short floats or doubles in the stack
frame and instead passes the first few in registers.

Mac32 ppc passes floats/doubles with switchcall parms but the 1st 14 must
be put in regs. Mac double_trick does inline asm to load them.
Perhaps this should use win64 trick instead of asm.

Mac32 intel needs nothing special.

XP64 pass floats/doubles with switchcall parms. But any floats/doubles
in the 1st 4 positions must be put in the corresponding mmx registers.
The XP64 double_trick puts the 1st four parameters in mmx regs.
XP64 runs no code, it just does a call with 4 doubles to get the mmx
regs loded before the switchcall.

Linux64/Mac64 pass the 1st 8 float/double args in mmx regs and they
are NOT in the switchcall parameters.  If more than 8 f/D args exist,
putting D after the 8th on stack starting from the 7th I.  Note
that the first 6 I args will be in regs that are unavailable to D..

double_trick call must be immediately before SWITCHCALL
otherwise the regs may be used and the parameter lost.
*/

#if _WIN32
#include <windows.h>
#include <windowsx.h>
#define FIXWINUTF8 // possibly should not be defined for MINGW32
#ifdef __MINGW32__
#ifndef _stdcall
#define _stdcall __stdcall
#define _cdecl __cdecl
#endif
#endif
#else
#include <stdlib.h>
#include <unistd.h>
typedef unsigned char       BYTE;
#define CALLBACK
#endif
#include <stdint.h>
#include <wchar.h>
#include <complex.h>
#undef I
#ifdef _WIN32
typedef _Fcomplex float_complex;
typedef _Dcomplex double_complex;
#else
typedef float complex float_complex;
typedef double complex double_complex;
#endif

#ifdef ANDROID
#if __ANDROID_API__ < 23
#undef creal
#undef cimag
#undef crealf
#undef cimagf
#define creal(x)  (*(double*)&x)
#define cimag(x)  (*(1+(double*)&x))
#define crealf(x)  (*(float*)&x)
#define cimagf(x)  (*(1+(float*)&x))
#endif
#endif

#include "j.h"
// test compilation of apple m1
// #define __APPLE__ 1

// align memory pointer to natural alignment
#define alignto(dvc,align) (void*)((uintptr_t)((dvc)+((align)-1)) & ~((align)-1))
/*
static void* alignto(void* dvc, int align){
// fprintf(stderr,"%p %d %p \n", dvc, align, (void*)((uintptr_t)((dvc)+((align)-1)) & ~((align)-1)) );
return (void*)((uintptr_t)((dvc)+((align)-1)) & ~((align)-1));
}
*/

#define SY_FREEBSD 0  // ??
#define SY_UNIX64 (SY_64 && (SY_LINUX || SY_MAC || SY_FREEBSD))

#if SY_WINCE
#define HINSTANCE_ERROR 0
wchar_t *tounibuf(char * src);
char *toascbuf(wchar_t *src);
#endif

/*  unix issues                                                 */
/*  if there is only one calling convention then                */
/*  ALTCALLINT/DOUBLE routines are not be required              */
/*  but the easiest thing to do is to make them be the same     */
/*  HMODULE type                                                */
/*  LoadLibrary, GetProcAddress, FreeLibrary routines           */
/*  GetLastError and FormatMessage                              */

#if (SYS & SYS_UNIX)

#include <dlfcn.h>

#if SYS & SYS_FREEBSD
/* resolve some harmless name clashes */
#undef atop
#endif

#ifndef ANDROID
#undef MAX     /* defined in sys/param.h */
#undef MIN     /* defined in sys/param.h */
#endif
#include <sys/param.h>

typedef void *HMODULE;
typedef char *LPSTR;
typedef I (*FARPROC)();
#define __stdcall
#define _cdecl
#endif

/* windows has 2 dll calling conventions - __stdcall and _cdecl */
/* __stdcall is used by most DLLs, including all system APIs    */
/* _cdecl is used by some C DLLs and by Fortran                 */
/* + flag in cd arg selects the alternate _cdecl convention     */

typedef I     (__stdcall *STDCALLI)();
typedef I     (_cdecl    *ALTCALLI)();
typedef D     (__stdcall *STDCALLD)();
typedef D     (_cdecl    *ALTCALLD)();

#ifdef C_CD_ARMHF
typedef float              DoF;
#else
typedef double             DoF;
#endif

#if SY_64 || defined(__arm__)   /* J64 requires special float result */
typedef float (__stdcall *STDCALLF)();
typedef float (_cdecl    *ALTCALLF)();
#endif

/* error return codes */
#define DEOK            0
#define DEBADLIB        1
#define DEBADFN         2
#define DETOOMANY       3       /* too many dlls loaded                 */
#define DECOUNT         4       /* too many args or (#args)~:#parms     */
#define DEDEC           5
#define DEPARM          6
#define DELIMIT         7       /* too many float/double args  */

#define NCDARGS         64      /* hardwired max number of arguments    */
#define NLIBS           100     /* max number of libraries              */

#define NLEFTARG        (2*NPATH+4+3*(1+NCDARGS))
                                /* max length of 15!:0 left argument    */

#define CDASSERT(p,c)   {if(unlikely(!(p))){jt->dlllasterror=c; ASSERT(0,EVDOMAIN);}}

typedef struct {
 FARPROC fp;                    /* proc function address                */
 HMODULE h;                     /* library (module) handle              */
 I4 ai;                          /* argument string index in cdstr      */
 I4 li;                          // library name index in cdstr - set nonzero in cdlookup to indicate that the current cc has a new libary name
 UI4 zt;                          /* result jtype                         */
 US an;                          /* argument string length               */
 US ln;                          // library name length
 S n;                           /* number of arguments (excl. result)   */
 C cc;                          /* call class: 0x00 or '0' or '1'       */
 C zl;                          /* result type letter                   */
 B zbx;                         /* > 1 iff result is boxed              */
 B fpreset;                     /* % 1 iff do FPRESET after call        */
 B alternate;                   /* + 1 iff alternate calling convention */
 B hloaded;                    // set if the h entry of this block was loaded
 struct {
  C star;               // arguments star or not: 0=not 1=* 2=&
  C tletter;            /* arguments type letters, cwusi etc.   */
 } starlett[NCDARGS];   // flag/letter per arg
} CCT;

#if SY_64 && SY_WIN32
extern void double_trick(D,D,D,D);
#endif

#if (SYS & SYS_MACOSX) | (SYS & SYS_LINUX)
#ifdef C_CD_ARMHF
extern void double_trick(float,float,float,float,float,float,float,float,float,float,float,float,float,float,float,float);
#else
#ifdef __PPC64__
extern void double_trick(D,D,D,D,D,D,D,D,D,D,D,D,D);
#else
extern void double_trick(D,D,D,D,D,D,D,D);
#endif
#endif
#endif
#if SY_MACPPC
static void double_trick(double*v, I n){I i=0;
 for(;i<n;i++)
#define l(a) case (a-1): asm volatile ( "mr r14, %0\nlfd f" #a ", 0(r14)" : : "r" (v+a-1 ) : "f" #a , "r14" ); break;
  switch(i) {
   l(1)
   l(2)
   l(3)
   l(4)
   l(5)
   l(6)
   l(7)
   l(8)
   l(9)
   l(10)
   l(11)
   l(12)
   l(13)
  }
#undef l
}
#endif

/*
#if SYS & SYS_MACOSX
 #define dtrick double_trick(dd,dcnt);
#elif SY_64 && SY_WIN32
 #define dtrick {D*pd=(D*)d; double_trick(pd[0],pd[1],pd[2],pd[3]);}
#elif SY_64 && SY_LINUX
 #define dtrick double_trick(dd[0],dd[1],dd[2],dd[3],dd[4],dd[5],dd[6],dd[7]);
#elif 1
 #define dtrick ;
#endif
*/

#if SY_64
 #if SY_WIN32
  #define dtrick {D*pd=(D*)d; double_trick(pd[0],pd[1],pd[2],pd[3]);}
 #elif SY_UNIX64
  #ifdef __PPC64__
   #define dtrick double_trick(dd[0],dd[1],dd[2],dd[3],dd[4],dd[5],dd[6],dd[7],dd[8],dd[9],dd[10],dd[11],dd[12]);
  #elif defined(__x86_64__)
#if 0
   #define dtrick double_trick(dd[0],dd[1],dd[2],dd[3],dd[4],dd[5],dd[6],dd[7]);
#else
/* might be faster */
   #define dtrick \
  __asm__ ("movq (%0),%%xmm0\n\t"       \
        "movq  8(%0), %%xmm1\n\t"       \
        "movq 16(%0), %%xmm2\n\t"       \
        "movq 24(%0), %%xmm3\n\t"       \
        "movq 32(%0), %%xmm4\n\t"       \
        "movq 40(%0), %%xmm5\n\t"       \
        "movq 48(%0), %%xmm6\n\t"       \
        "movq 56(%0), %%xmm7\n\t"       \
        : /* no output operands */      \
        : "r" (dd)                      \
        : "xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7","cc");
#endif
  #elif defined(__aarch64__)
   #define dtrick double_trick(dd[0],dd[1],dd[2],dd[3],dd[4],dd[5],dd[6],dd[7]);
  #endif
 #else
  #define dtrick ;
 #endif
#else
 #if SY_WIN32
  #define dtrick ;
 #elif SY_LINUX
 #ifdef C_CD_ARMHF
  #define dtrick double_trick(dd[0],dd[1],dd[2],dd[3],dd[4],dd[5],dd[6],dd[7],dd[8],dd[9],dd[10],dd[11],dd[12],dd[13],dd[14],dd[15]);
 #else
  #define dtrick ;
 #endif
 #elif SY_FREEBSD
  #define dtrick ;
 #elif SY_MACPPC
  #define dtrick double_trick(dd,dcnt);
 #else
  #define dtrick ;
 #endif
#endif


#define SWITCHCALL                                                         \
  dtrick                                                                   \
  switch(cnt){                                                             \
  case 0:  r = fp(); break;                                                \
  case 1:  r = fp(d[0]); break;                                            \
  case 2:  r = fp(d[0], d[1]); break;                                      \
  case 3:  r = fp(d[0], d[1], d[2]); break;                                \
  case 4:  r = fp(d[0], d[1], d[2], d[3]); break;                          \
  case 5:  r = fp(d[0], d[1], d[2], d[3], d[4]); break;                    \
  case 6:  r = fp(d[0], d[1], d[2], d[3], d[4], d[5]); break;              \
  case 7:  r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6]); break;        \
  case 8:  r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7]); break;  \
  case 9:  r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8]); break;                                            \
  case 10: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9]); break;                                      \
  case 11: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10]); break;                               \
  case 12: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11]); break;                         \
  case 13: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12]); break;                   \
  case 14: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13]); break;             \
  case 15: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14]); break;       \
  case 16: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15]); break; \
  case 17: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16]); break;                                           \
  case 18: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17]); break;                                     \
  case 19: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18]); break;                               \
  case 20: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19]); break;                         \
  case 21: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20]); break;                   \
  case 22: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21]); break;             \
  case 23: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22]); break;       \
  case 24: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23]); break; \
  case 25: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24]);break;                                            \
  case 26: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25]);break;                                      \
  case 27: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26]);break;                                \
  case 28: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27]);break;                          \
  case 29: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28]);break;                    \
  case 30: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29]);break;              \
  case 31: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30]);break;        \
  case 32: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31]);break;  \
  case 33: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32]);break;                                            \
  case 34: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33]);break;                                      \
  case 35: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34]);break;                                \
  case 36: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35]);break;                          \
  case 37: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36]);break;                    \
  case 38: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37]);break;              \
  case 39: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38]);break;        \
  case 40: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39]);break;  \
  case 41: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40]);break;                                            \
  case 42: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41]);break;                                      \
  case 43: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42]);break;                                \
  case 44: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43]);break;                          \
  case 45: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44]);break;                    \
  case 46: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45]);break;              \
  case 47: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46]);break;        \
  case 48: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47]);break;  \
  case 49: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47],         \
                  d[48]);break;                                            \
  case 50: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47],         \
                  d[48],d[49]);break;                                      \
  case 51: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47],         \
                  d[48],d[49],d[50]);break;                                \
  case 52: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47],         \
                  d[48],d[49],d[50],d[51]);break;                          \
  case 53: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47],         \
                  d[48],d[49],d[50],d[51],d[52]);break;                    \
  case 54: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47],         \
                  d[48],d[49],d[50],d[51],d[52],d[53]);break;              \
  case 55: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47],         \
                  d[48],d[49],d[50],d[51],d[52],d[53],d[54]);break;        \
  case 56: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47],         \
                  d[48],d[49],d[50],d[51],d[52],d[53],d[54],d[55]);break;  \
  case 57: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47],         \
                  d[48],d[49],d[50],d[51],d[52],d[53],d[54],d[55],         \
                  d[56]);break;                                            \
  case 58: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47],         \
                  d[48],d[49],d[50],d[51],d[52],d[53],d[54],d[55],         \
                  d[56],d[57]);break;                                      \
  case 59: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47],         \
                  d[48],d[49],d[50],d[51],d[52],d[53],d[54],d[55],         \
                  d[56],d[57],d[58]);break;                                \
  case 60: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47],         \
                  d[48],d[49],d[50],d[51],d[52],d[53],d[54],d[55],         \
                  d[56],d[57],d[58],d[59]);break;                          \
  case 61: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47],         \
                  d[48],d[49],d[50],d[51],d[52],d[53],d[54],d[55],         \
                  d[56],d[57],d[58],d[59],d[60]);break;                    \
  case 62: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47],         \
                  d[48],d[49],d[50],d[51],d[52],d[53],d[54],d[55],         \
                  d[56],d[57],d[58],d[59],d[60],d[61]);break;              \
  case 63: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47],         \
                  d[48],d[49],d[50],d[51],d[52],d[53],d[54],d[55],         \
                  d[56],d[57],d[58],d[59],d[60],d[61],d[62]);break;        \
  case 64: r = fp(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],          \
                  d[8], d[9], d[10],d[11],d[12],d[13],d[14],d[15],         \
                  d[16],d[17],d[18],d[19],d[20],d[21],d[22],d[23],         \
                  d[24],d[25],d[26],d[27],d[28],d[29],d[30],d[31],         \
                  d[32],d[33],d[34],d[35],d[36],d[37],d[38],d[39],         \
                  d[40],d[41],d[42],d[43],d[44],d[45],d[46],d[47],         \
                  d[48],d[49],d[50],d[51],d[52],d[53],d[54],d[55],         \
                  d[56],d[57],d[58],d[59],d[60],d[61],d[62],d[63]);break;  \
}

static I     NOOPTIMIZE stdcalli(STDCALLI fp,I*d,I cnt,DoF*dd,I dcnt){I r;
 SWITCHCALL;
 R r;
}  /* I result */
static I     NOOPTIMIZE altcalli(ALTCALLI fp,I*d,I cnt,DoF*dd,I dcnt){I r;
 SWITCHCALL;
 R r;
}
static D     NOOPTIMIZE stdcalld(STDCALLD fp,I*d,I cnt,DoF*dd,I dcnt){D r;
 SWITCHCALL;
 R r;
}  /* D result */
static D     NOOPTIMIZE altcalld(ALTCALLD fp,I*d,I cnt,DoF*dd,I dcnt){D r;
 SWITCHCALL;
 R r;
}

#if SY_64 || defined(__arm__)
static float NOOPTIMIZE stdcallf(STDCALLF fp,I*d,I cnt,DoF*dd,I dcnt){float r;
SWITCHCALL;
R r;
}  /* J64 float result */
static float NOOPTIMIZE altcallf(ALTCALLF fp,I*d,I cnt,DoF*dd,I dcnt){float r;
  SWITCHCALL;
 R r;
}
#endif

/* fp        - function                                    */
/* d         - address values for call arguments           */
/* cnt       - count of values                             */
/* zl        - result type letter                          */
/* v         - result data area                            */
/* alternate - whether to use alternate calling convention */

static void docall(FARPROC fp, I*d, I cnt, DoF* dd, I dcnt, C zl, I*v, B alternate){
#define vdresvalues(w) CCM(w,'c')+CCM(w,'w')+CCM(w,'u')+CCM(w,'b')+CCM(w,'s')+CCM(w,'i')+CCM(w,'l')+CCM(w,'x')+CCM(w,'*')+CCM(w,'n')
 CCMWDS(vdres) CCMCAND(vdres,cand,zl)   // see if zl is one of the direct results, which can be moved into the result directly

 if(CCMTST(cand,zl)){I r;
  // result has integer type.  Call with that linkage
  r= alternate ? altcalli((ALTCALLI)fp,d,cnt,dd,dcnt) : stdcalli((STDCALLI)fp,d,cnt,dd,dcnt);
  // Move the return code into an I, from whatever size the user says it is
  // Convert the type char to a concise code
  zl=zl=='b'?'a':zl; I charcode=(zl&0x1f)>>1;  // unique value for each field type, remapping 'b'
  // the fields are
  // (12) x w u s _ n l * i _ _ c b (0)
  // There are 5 fates for the result: passthrough; sign-extension of 1, 2, or 4 bytes; 0
  // the first 4 are encoded as lg2(# bytes to keep):
  // 64b  11111101xxxx111110xxxx1100 = 3f43e0c
  // 32b  10101001xxxx101010xxxx1000 = 2a22a08
  I shiftamt=BW-((1LL<<(((SY_64?0x3f43e0c:0x2a22a08)>>(charcode<<1))&3))<<3);  // # high bits to fill
  r=(r<<shiftamt)>>shiftamt;  // fill em
  r=zl=='n'?0:r;  // handle 'n' specially: store 0
  *v=r;  // write out the formatted return code
 }else
#if !SY_64 && !defined(__arm__)
 {D r;
  r= alternate ? altcalld((ALTCALLD)fp,d,cnt,dd,dcnt) : stdcalld((STDCALLD)fp,d,cnt,dd,dcnt);
  *(D*)v=r;
 }
#else
 {/* the above doesn't work for J64 */
  if(likely(zl=='d')){D r;
   r= alternate ? altcalld((ALTCALLD)fp,d,cnt,dd,dcnt) : stdcalld((STDCALLD)fp,d,cnt,dd,dcnt);
   *(D*)v=r;
  }else{float r;
   r= alternate ? altcallf((ALTCALLF)fp,d,cnt,dd,dcnt) : stdcallf((STDCALLF)fp,d,cnt,dd,dcnt);
   *(D*)v=(D)r;
  }
 }
#endif
}

static void convertdown(I*pi,I n,C t){
 if(n)switch(t){
  case 'f': {float*pt=(float*)pi;D*pd=(D*)pi; DO(n, pt[i]=(float)pd[i];);} break;
  case 'b': {BYTE*pt=(BYTE*)pi;               DO(n, pt[i]=(BYTE)pi[i];);} break;
  case 's': {short*pt=(short*)pi;             DO(n, pt[i]=(short)pi[i];);} break;
  case 'i': {int  *pt=(int  *)pi;             DO(n, pt[i]=(int)  pi[i];);} break;
#ifdef _WIN32
  case 'z': {float_complex*pt=(float_complex*)pi;D*pd=(D*)pi; DO(n, pt[i]=_FCOMPLEX_((float)pd[2*i],(float)pd[1+2*i]););} break;
#else
  case 'z': {float_complex*pt=(float_complex*)pi;D*pd=(D*)pi; DO(n, pt[i]=(float)pd[2*i]+_Complex_I*(float)pd[1+2*i];);} break;
#endif
}}   /* convert I in place to s or int and d to f and j to z */

static void convertup(I*pi,I n,C t){I j=n;
 if(n)switch(t){
  case 'f': {float*pt=(float*)pi;D*pd=(D*)pi; DQ(n, --j; pd[j]=(D)pt[j];);} break;
  case 'b': {BYTE*pt=(BYTE*)pi;               DQ(n, --j; pi[j]=(I)pt[j];);} break;
  case 's': {short*pt=(short*)pi;             DQ(n, --j; pi[j]=(I)pt[j];);} break;
  case 'i': {int  *pt=(int  *)pi;             DQ(n, --j; pi[j]=(I)pt[j];);} break;
  case 'z': {float_complex*pt=(float_complex*)pi;D*pd=(D*)pi; DQ(n, --j; pd[1+2*j]=(D)cimagf(pt[j]); pd[2*j]=(D)crealf(pt[j]););} break;
}}   /* convert s or int to I and f to d and z to j */


/* cache of 15!:0 parsed left arguments                                 */
/*                                                                      */
// cdarg   - append-only table of parsed results - rank 0
//           each entry is pointer to the A block for the CCT of the string (which is in the IAV1 of the block)
/*           when space runs out allocation is doubled                  */
// AM(cdarg) is # rows filled
// cdstr   - append-only table of 15!:0 left argument strings verbatim - rank 0
/*           indexed by ai and an in the CCT struct                     */
/*           when space runs out allocation is doubled                  */
// AM(cdstr) is # bytes allocated
/* cdhash  - hash table                                                 */
//           one entry per row of cdarg
/*           when 2*cdna exceeds # entries allocation is doubled        */
/*           table values are indices into cdarg, or -1                 */
// AM(cdhash) is # entries filled, always equal to AM(cdarg)
/* cdhashl - hash table for libraries (modules)                         */
// AM(cdhashl) is # entries filled, max (NLIBS)

// allocate hashtable, fill with -1.  Result is address
static A jtcdgahash(J jt,I n){A z;I hn;
 FULLHASHSIZE(n,INTSIZE,0,0,hn);
 GATV0(z,INT,hn,0); ACINITZAP(z); mvc(hn*SZI,AV(z),1,MEMSETFF);  // no rank - use all words for table
 R z;
}

B jtcdinit(JS jjt,I nthreads){A x;JJ jt=MTHREAD(jjt);
 RZ(x=exta(BOX,0L,1L,100L )); ACINITZAP(x) INITJT(jjt,cdarg)=x;  // allocate indirect pointers to CCT blocks
 RZ(INITJT(jjt,cdhash) =cdgahash(4*AN(INITJT(jjt,cdarg))));  // start with 4x allocation for the strings.  We will reallocate when it gets to 2x.
 RZ(INITJT(jjt,cdhashl)=cdgahash(NLIBS+16));  // will round up to power of 2 - we allow 100 libraries, which will almost never be used, so we don't get the usual 2x
 RZ(x=exta(LIT,0L,1L,         5000L)); ACINITZAP(x) mvc(AN(x),AV(x),1,MEMSET00); INITJT(jjt,cdstr)=x;  //  Do this last; it indicates validity for all
 AM(INITJT(jjt,cdstr))=AM(INITJT(jjt,cdarg))=AM(INITJT(jjt,cdhash))=AM(INITJT(jjt,cdhashl))=0;  // init all tables to empty
 R 1;
}

// find the starting index for v->string (length n) in table tbl
#if SY_64
#define HASHINDEX(tbl,n,v) ((hic(n,v)*(UI)AN(tbl))>>32)
#else
#define HASHINDEX(tbl,n,v) ((hic(n,v)*(UIL)AN(tbl))>>32)
#endif

// see if v->string (length nn) is in hashtable tbl.  The hash in tbl contains indexes into cdarg, or -1 for empty slot.
// return retval, where pv[k] is the address of the found slot in cdarg
#define HASHLOOKUP(tbl,nn,vv,pvklett,retval) I j=HASHINDEX(tbl,nn,vv); I *hv=IAV0(tbl); C *s=CAV0(JT(jt,cdstr)); A *pv=AAV0(JT(jt,cdarg)); \
 NOUNROLL while(1){I k=hv[j]; if(k<0)R 0; if(nn==((CCT*)IAV1(pv[k]))->pvklett##n&&!memcmpne(vv,s+((CCT*)IAV1(pv[k]))->pvklett##i,nn))R retval; if(unlikely(--j<0))j+=AN(tbl);}

// add the index argx into the hashtable tbl for the key v->string (length n).  argx is an index into cdarg.  Increment AM(tbl), which contains the # hashed items
#define HASHINSERT(tbl,n,v,argx) I j=HASHINDEX(tbl,n,v); I *hv=IAV0(tbl); ++AM(tbl); NOUNROLL while(hv[j]>=0)if(unlikely(--j<0))j+=AN(tbl); hv[j]=argx;


// a is a string block for a cd string
// result is the address in cdarg of the CCT block for the string, or 0 if not found
static CCT*jtcdlookup(J jt,A a){
 HASHLOOKUP(JT(jt,cdhash),AN(a),UAV(a),a,((CCT*)IAV1(pv[k])))
}

// av->null-terminated name of library
// result is h field of the entry in cdarg for the library.  This entry may come from any CCT that matches the library name
static HMODULE jtcdlookupl(J jt,C*av){
 I an=strlen(av);
 HASHLOOKUP(JT(jt,cdhashl),an,av,l,((CCT*)IAV1(pv[k]))->h)
}

// add a hash entry for a cd left arg.  It is known not to be present already
// a is the A block for the string
// cc->parsed CCT block
// the string is appended to the end of cdstr, and its offset is installed into cc->ai (cc->an is already filled in)
// the CCT is appended to cdarg, and then the string is hashed into cdhash
// result is the address of the permanent CCT entry, which is a permanent block that we allocate here & copy input cc into
// return 0 if error
static CCT*jtcdinsert(J jt,A a,CCT*cc){A x;A z;I an,hn,k;
 // Make a copy of the valid part of cc where it will reside forever, and point cc to it
 I cclen=offsetof(CCT, starlett)+cc->n*sizeof(cc->starlett[0]); GAT0(z,LIT,cclen,1) ACINITZAP(z)  MC(IAV1(z),cc,cclen); cc= (CCT*)IAV1(z);
 an=AN(a);
 // add the new string - under lock.  If table extensions are required we perform any necessary allocations outside the lock
 
 WRITELOCK(JT(jt,cdlock));

 NOUNROLL while(AM(JT(jt,cdstr)) > AN(JT(jt,cdstr))-an)RZ(jtextendunderlock(jt,&JT(jt,cdstr),&JT(jt,cdlock),0))
 C *s=CAV0(JT(jt,cdstr)); 
 cc->ai=AM(JT(jt,cdstr)); MC(s+AM(JT(jt,cdstr)),CAV(a),an); AM(JT(jt,cdstr))+=an;  // put addr of next string ito cc; move the string; add its length to table len
 // add the pointer to the new cc - under lock
 NOUNROLL while(AM(JT(jt,cdarg))==AN(JT(jt,cdarg))){RZ(jtextendunderlock(jt,&JT(jt,cdarg),&JT(jt,cdlock),0)) s=CAV0(JT(jt,cdstr));}
 k=AM(JT(jt,cdarg));   // index of the string to be inserted
 A *pv=AAV0(JT(jt,cdarg));
 pv[k]=z;   // copy the cc to a new block.
 ++AM(JT(jt,cdarg));
 // if requested, hash the libary name into cdhashl - under lock
 if(cc->li>=0){
  /* assumes the hash table for libraries (JT(jt,cdhashl)) is fixed sized */
  // a new lib was loaded and verified.  Add it to cdhashl, pointing to the arg block
  cc->hloaded=1;
  if(AM(JT(jt,cdhashl))<AN(JT(jt,cdhashl))) {
   cc->li+=cc->ai; HASHINSERT(JT(jt,cdhashl),cc->ln,s+cc->li,k)   // convert string offset to offset inside strings table; hash the library
  }else cc=0;  // no room to insert - we will have to fail the operation.  This sucks, because we wait for the table to totally fill
 }else cc->hloaded=0;
 // hash the new string into cdhash, possibly resizing the hashtable - under lock
 if(AN(JT(jt,cdhash))<=2*AM(JT(jt,cdarg))){RZ(jtextendunderlock(jt,&JT(jt,cdhash),&JT(jt,cdlock),1)) s=CAV0(JT(jt,cdstr)); pv=AAV0(JT(jt,cdarg)); k=AM(JT(jt,cdarg));}
 // insert any elements into the hash that aren't there already.  The strings are in cdarg.  Usually the only string to add is
 // the last one, but it is also possible that there are none to add (if someone else got in and resized/rehashed the array), or perhaps
 // we have to hash everything (if we ourselves just did a resize).  AM(cdhash) tells how many atoms are valid, and we install any others
 DQ(AM(JT(jt,cdarg))-k, HASHINSERT(JT(jt,cdhash),((CCT*)IAV1(pv[k]))->an,s+((CCT*)IAV1(pv[k]))->ai,k) ++k;);  // add 1 ele to cdarg, and all or 1 to cdhash

 WRITEUNLOCK(JT(jt,cdlock));
 ASSERT(cc!=0,EVFACE)  // give message if we couldn't insert library name into library hash

 R cc;  // return the address of the CC block after we copied it to a safe place
}

// given a cc block being created, and pointers to strings for the library and procedure, fill in the pointers to
// the library and procedure.  cc->li has been initialized with the offset of the library name in the cd string - not used here, but set to -1 to SUPPRESS hashing the library name
static CCT*jtcdload(J jt,CCT*cc,C*lib,C*proc){FARPROC f;HMODULE h;
 /* not all platforms support GetModuleHandle, so we do it ourselves */
 /* we match on exactly the arg the user supplied                    */
 /* search path and case can cause us to reload the same dll         */
 if(cc->cc){C buf[SY_64?21:12];I k,n;
  n=strlen(proc);
  CDASSERT(BETWEENO(n,1,sizeof(buf)),DEBADFN);
  k='_'==*proc?-strtoI(1+proc,0L,10):strtoI(proc,0L,10);
  CDASSERT(k&&'0'==*lib||0<=k&&'1'==*lib,DEBADFN);
  sprintf(buf,FMTI,k); if(0>k)*buf='_';
  CDASSERT(!strcmp(proc,buf),DEBADFN);
  cc->fp=(FARPROC)k;
  cc->li=-1;  // suppress hashing library name
  R cc;
 }
 if(h=cdlookupl(lib)){cc->h=h; cc->li=-1;    // if lib is in hash table, use the handle for it.  Suppress rehashing the library
 }else{
  CDASSERT(AM(JT(jt,cdhashl))<NLIBS,DETOOMANY);    /* too many dlls loaded */
#if SY_WIN32
#if SY_WINCE
  h=LoadLibrary(tounibuf(lib));
#else
#ifdef FIXWINUTF8
  wchar_t wlib[1024];
  MultiByteToWideChar(CP_UTF8,0,lib,1+(int)strlen(lib),wlib,1024);
  h=LoadLibraryW(wlib);
#else
  h=LoadLibraryA(lib);
#endif
#endif
  CDASSERT((UI)h>HINSTANCE_ERROR,DEBADLIB);
#endif
#if SYS & SYS_UNIX
  CDASSERT(h=dlopen((*lib)?lib:0,RTLD_LAZY),DEBADLIB);
#endif
  cc->h=h;   // remember the handle; leave cc->li set to cause the library name to be hashed into cdhashl
 }
#if SY_WIN32 && !SY_WINCE
 f=GetProcAddress(h,'#'==*proc?(LPCSTR)(I)atoi(proc+1):(LPCSTR)proc);
#endif
#if SY_WINCE
 f=GetProcAddress(h,tounibuf(proc));
#endif
#if (SYS & SYS_UNIX)
 f=(FARPROC)dlsym(h,proc);
#endif
 CDASSERT(f!=0,DEBADFN);
 cc->fp=f;
 R cc;
}

     /* J type from type letter */
static I cdjtype(C c){I r=INT; r=c=='c'?LIT:r; r=c=='w'?C2T:r; r=c=='u'?C4T:r; r=(c&(C)~('j'^'z'))=='j'?CMPX:r; r=(c&(C)~('d'^'f'))=='d'?FL:r; r=c==0?0:r; R r;}  // d/f and j/z differ by only 1 bit

/* See "Calling DLLs" chapter in J User Manual                  */
/* format of left argument to 15!:0                             */
/*    filename procedure [>][+][%] declarations                 */
/* e.g.                                                         */
/*    opengl32.dll glVertex3d > n d d d                         */
/*                                                              */
/* file      - aka module aka library                           */
/*             can also be 0 or 1                               */
/*             0: "procedure" is memory address                 */
/*             1: "procedure" is index and                      */
/*                first parm is address of address of a vtable  */
/* procedure - name of function within library                  */
/* >         - if result is open (non-boxed) atom               */
/* +         - alternate calling convention                     */
/* %         - execute _fpreset() after dll call                */
/*                                                              */
/* declarations data types                                      */
/*  c   LIT   char                                              */
/*  w   C2T   wide char (2 bytes)                               */
/*  u   C4T   wide char (4 bytes)                               */
/*  b   INT   unsigned tiny int (1 byte)                        */
/*  s   INT   short int (2 bytes)                               */
/*  i   INT   int;  4 byte on 32, 4 byte on 64                  */
/*  l   INT   long; error  on 32, 8 byte on 64                  */
/*  x   INT   long; 4 byte on 32, 8 byte on 64                  */
/*  f   FL    4 byte float                                      */
/*  d   FL    8 byte float                                      */
/*  z   CMPX  8 byte complex (must be preceded by *)            */
/*  j   CMPX  16 byte complex (must be preceded by *)           */
/*  *         pointer (memu will be applied before call)        */
/*  &         pointer NOT requiring memu                        */
/*  n   INT   no result (integer 0)                             */

static CCT*jtcdparse(J jt,A a,I empty){C c,lib[NPATH],*p,proc[NPATH],*s,*s0;CCT*cc,cct;I an,der,i,pi;
 ASSERT(LIT&AT(a),EVDOMAIN);
 ASSERT(1>=AR(a),EVRANK);
 ASSERT(NLEFTARG>=AN(a),EVLIMIT);
 READLOCK(JT(jt,cdlock)); cc=cdlookup(a); READUNLOCK(JT(jt,cdlock));  // see if the string has been encountered before.  The CCT itself is permanent
 if(cc)R cc;   // if the string is cached, use the cc block for it
 cc=&cct; cc->an=an=AN(a); s=s0=CAV(str0(a));
 /* library (module, file) name */
 NOUNROLL while(*s==' ')++s; p=*s=='"'?strchr(++s,'"'):strchr(s,' '); cc->li=s-s0; cc->ln=p?p-s:0;
 CDASSERT(p&&NPATH>cc->ln,DEBADLIB);
 cc->cc=1==cc->ln&&('0'==*s||'1'==*s)?*s:0;
 /* procedure name */
 s=p+1+(I )(*p=='"');
 NOUNROLL while(*s==' ')++s; p=strchr(s,' '); if(!p)p=s+strlen(s);    pi=s-s0;
 I procnamelen=p-s;  // length of proc name
 CDASSERT(NPATH>procnamelen,DEBADFN);
 /* > + % */
 s=p+1;
 cc->zbx      =1; NOUNROLL while(*s==' ')++s; if('>'==*s){cc->zbx      =0; ++s;}
 cc->alternate=0; NOUNROLL while(*s==' ')++s; if('+'==*s){cc->alternate=1; ++s;}
 cc->fpreset  =0; NOUNROLL while(*s==' ')++s; if('%'==*s){cc->fpreset  =1; ++s;}
 /* result type declaration */
 NOUNROLL while(*s==' ')++s;
 if(empty&&!*s){
  cc->zl=c='x'; cc->zt=cdjtype(c);
 }else{
  CDASSERT(*s,DEDEC);
  cc->zl=c=*s++; cc->zt=cdjtype(c);
 }
 // verify that result is a valid type
#define vresvalues(w) CCM(w,'c')+CCM(w,'w')+CCM(w,'u')+CCM(w,'b')+CCM(w,'s')+CCM(w,'i')+CCM(w,'l')+CCM(w,'x')+CCM(w,'f')+CCM(w,'d')+CCM(w,'*')+CCM(w,'n')
 CCMWDS(vres) CCMCAND(vres,cand,c)  // see if zl is one of the allowed types
 CDASSERT(CCMTST(cand,c),DEDEC);
 CDASSERT(SY_64||'l'!=c,DEDEC);
 // if result is * followed by valid arg type, ratify it by advancing the pointer over the type (otherwise fail in next test)
#define vargtvalues(w) CCM(w,'c')+CCM(w,'w')+CCM(w,'u')+CCM(w,'b')+CCM(w,'s')+CCM(w,'i')+CCM(w,'l')+CCM(w,'x')+CCM(w,'f')+CCM(w,'d')+CCM(w,'z')+CCM(w,'j')
 CCMWDS(vargt)  // set up for comparisons against list of bytes
 if(c=='*' && *s){
  CCMCAND(vargt,cand,*s) s+=SGNTO0(CCMSGN(cand,*s)); // if *s is valid, skip over it
 }
 CDASSERT((*s&~' ')==0,DEDEC);  // 0 or SP
#ifdef C_CD_NODF // platform does not support f result
 CDASSERT(cc->zl!='f',DEDEC)
#endif
 /* argument type declarations */
 i=-1;
 while(c=*s++){
  if(' '==c)continue;
  ++i; der=DEDEC+256*(1+i);
  CDASSERT(i<NCDARGS,DECOUNT);
  cc->starlett[i].tletter=0; cc->starlett[i].star=0;
  CDASSERT(i||'1'!=cc->cc||'x'==c||'*'==c&&(!*s||' '==*s),der);  // verify result type is allowed
  if('*'==c||'&'==c){cc->starlett[i].star=1+(I )('&'==c); c=*s++; if(!c)break; if(' '==c)continue;}
  cc->starlett[i].tletter=c;
  CCMCAND(vargt,cand,c) CDASSERT(CCMTST(cand,c),der);  // vrgt defined above,list of valid arg bytes
  CDASSERT((c!='z'&&c!='j')||cc->starlett[i].star,der);
  if('l'==c){CDASSERT(SY_64,der); cc->starlett[i].tletter='x';}
#ifdef C_CD_NODF // platform does not support f or d args
 CDASSERT(cc->starlett[i].star==1 || (cc->starlett[i].tletter!='f' && cc->starlett[i].tletter!='d'),der);
#endif
 }
 CDASSERT(0<=i||'1'!=cc->cc,DEDEC+256);
 MC(lib, s0+cc->li,cc->ln); lib [cc->ln]=0;  // create lib string for fn lookup
 MC(proc,s0+pi,procnamelen); proc[procnamelen]=0;  // create procname string for fn lookup

#if SY_MAC && !SY_64
// mac osx 32 lseek off_t (64 bit) is not called properly
// map libc.dylib seek to be libj.dylib x15lseek32
if(!strcmp(lib,"libc.dylib")&&!strcmp(proc,"lseek"))
{
strcpy(lib,"libj.dylib");
strcpy(proc,"x15lseek32");
}
#endif

 RZ(cc=cdload(cc,lib,proc));  // cc doesn't change: load the library if necessary and fill in cc with its info.
 // if the library name is found, cdload will set cc->li negative to suppress adding the name to cdhashl
 cc->n=1+i; RZ(cc=cdinsert(a,cc));  // save # args(+1); add cc to hash and get its permanent address
 R cc;
}

#define CDT(x,y) (6*((0x160004>>(x))&7)+((0x160004>>(y))&0x7))  // encode 0 1 2 3 17 18-> 4 2 1 0 3 5  1011. .... .... .... 0100

static I*jtconvert0(J jt,I zt,I*v,I wt,C*u){D p,q;I k=0;US s;C4 s4;
 if(unlikely((zt|wt)&((NOUN|SPARSE)&~(B01+LIT+INT+FL+C2T+C4T))))R 0;  // if unallowed type, abort
 switch(CDT(CTTZ(zt),CTTZ(wt))){
  default:           R 0;
  case CDT(FLX, B01X): *(D*)v=*(B*)u; break;
  case CDT(FLX, INTX): *(D*)v=(D)*(I*)u; break;
  case CDT(FLX, FLX ): *(D*)v=*(D*)u; break;
  case CDT(C2TX,LITX): *(US*)v=*(UC*)u; break;
  case CDT(C2TX,C2TX): *(US*)v=*(US*)u; break;
  case CDT(LITX,LITX): *(UC*)v=*(UC*)u; break;
  case CDT(LITX,C2TX): s=*(US*)u; if(256<=(US)s)R 0; *(UC*)v=(UC)s; break;
  case CDT(C4TX,LITX): *(C4*)v=*(UC*)u; break;
  case CDT(C4TX,C2TX): *(C4*)v=*(US*)u; break;
  case CDT(C4TX,C4TX): *(C4*)v=*(C4*)u; break;
  case CDT(LITX,C4TX): s4=*(C4*)u; if(256<=(C4)s4)R 0; *(UC*)v=(UC)s4; break;
  case CDT(C2TX,C4TX): s4=*(C4*)u; if(65536<=(C4)s4)R 0; *(US*)v=(US)s4; break;
  case CDT(INTX,B01X): *    v=*(B*)u; break;
  case CDT(INTX,INTX): *    v=*(I*)u; break;
  case CDT(INTX,FLX ):
#if SY_64
  p=*(D*)u; q=jround(p); I rq=(I)q;
  if(!ISFTOIOK(p,q))R 0;  // must equal int, possibly out of range.  Exact equality is common enough to test for
  *v=rq;
#else
   p=*(D*)u; q=jfloor(p);
   if(p<IMIN*(1+FUZZ)||IMAX*(1+FUZZ)<p)R 0;
   if(FFEQ(p,q))*v=(I)q; else if(FFEQ(p,1+q))*v=(I)(1+q); else R 0;
#endif
 }
 R v;
}    /* convert a single atom. I from D code adapted from IfromD() in k.c */

// make one call to the DLL.
// if cc->zbx is true, zv0 points to AAV(z) where z is the block that will hold the list of boxes that
// will be the result of 15!:0.  z is always nonrecursive
static B jtcdexec1(J jt,CCT*cc,C*zv0,C*wu,I wk,I wt,I wd){A*wv=(A*)wu,x,y,*zv;B zbx;
    C c,cipt[NCDARGS],*u;FARPROC fp;float f;I cipcount=0,cipn[NCDARGS],*cipv[NCDARGS],cv0[2],
    data[NCDARGS*2],dcnt=0,fcnt=0,*dv,i,n,per,xn,xr,xt,*xv; DoF dd[NCDARGS]; I t;
#if defined(__aarch64__)
// parameter in stack is not fixed size
 char *dvc;        // character pointer to data[]
 int rcnt=0;       // next general purpose register number
#define maxrcnt 8  // number of register for passing integral parameter
#define maxdcnt 8  // number of simd register for passing float/double parameter
#endif
 FPREFIP(J);  // save inplace flag
 n=cc->n;  // n is # cd args
 if(unlikely(((n-1)|SGNIF(wt,BOXX))>=0)){DO(n, CDASSERT(!cc->starlett[i].star,DEPARM+256*i));}  // if there are args, and w is not boxed, verify there are no pointer parms
 zbx=cc->zbx; zv=1+(A*)zv0; dv=data; u=wu; xr=0;  // zv->first input arg  zbx is 1 if the result includes the input boxes; 0 if just bare value
#if defined(__aarch64__)
 dvc=maxrcnt*sizeof(I)+(char*)data;
#endif
 for(i=0;i<n;++i,++zv){  // for each input field
#if SY_UNIX64 && defined(__x86_64__)
  if(dv-data>=6&&dv-data<dcnt-2)dv=data+dcnt-2;
#elif SY_UNIX64 && defined(__aarch64__)
//  if(dcnt>8&&dvc-(char*)data==64)dvc=(char*)(data+dcnt);    /* v0 to v7 fully filled before x0 to x7 */
#elif defined(C_CD_ARMHF)
  if((fcnt>16||dcnt>16)&&dv-data==4)dv=data+MAX(fcnt,dcnt)-12;  /* v0 to v15 fully filled before x0 to x3 */
#endif
  per=DEPARM+i*256; I star=cc->starlett[i].star; c=cc->starlett[i].tletter; t=cdjtype(c);  // c is type in the call, t is the J type for that.  star is the *& qualifier
  I cbit=(I)1<<(c-'a');  // one-hot encoding of c
  I litsgn;  // will be neg if lit array that can be used as pointer
  I boxatomsgn;  // neg if boxed atom used as pointer
  if(likely(wt&BOX)){  // inputs are boxes, pointed to by wv
   x=C(wv[i]); xt=AT(x); xn=AN(x); xr=AR(x);   // x is argument i
   boxatomsgn=-star&(xr-1)&SGNIF(xt,BOXX);  // neg if boxed atom used as pointer
   CDASSERT((-xr&(star-1))>=0,per);         /* non-pointers must be scalars   !xr||star */
// long way   lit=star&&xt&LIT&&(c=='b'||c=='s'&&0==(xn&1)||c=='f'&&0==(xn&3));
   // litsgn means x is a character area whose address can be used as a source for numeric data
   litsgn=-star & SGNIF(xt,LITX) & -(cbit&(((I)1<<('b'-'a'))|((I)1<<('f'-'a'))|((I)1<<('s'-'a')))) & (((0x3040>>((c&7)<<1))&3&xn)-1);  // neg if lit
// long way    if(t&&TYPESNE(t,xt)&&!(lit||star&&!xr&&xt&BOX)){x=cvt(xt=t,x); CDASSERT(x!=0,per);}
   if(unlikely(((t-1)|(TYPESXOR(t,xt)-1)|litsgn|boxatomsgn)>=0)){x=cvt(xt=t,x); CDASSERT(x!=0,per);}
   // We know that x originated in a box, so it can't be PRISTINE.  But it may have been converted, so we have to
   // make sure that what we install into *zv is not inplaceable.  *zv is never recursive.
   xv=AV(x); if(zbx)*zv=incorp(x);
  }else{   // non-boxed input.  Must be atom
   xv=convert0(t,cv0,wt,u); xt=t; u+=wk;  // convert to required type ???
   boxatomsgn=-star&(xr-1)&SGNIF(xt,BOXX);  // neg if boxed atom used as pointer
   CDASSERT(xv!=0,per);   // abort if conversion failed
   if(zbx){GA00(y,t,1,0); MC(AV(y),xv,bpnoun(t)); *zv=incorp(y);}  // must never install inplaceable block; move value into a box
  }
  // now xv points to the actual arg data for arg i, and an A-block for same has been installed into *zv
  // if wt&BOX only, x is an A-block for arg i
  if(unlikely(boxatomsgn<0)){           // scalar boxed integer/boolean scalar is a pointer - NOT memu'd.  If xt is a box, wt must have been a box
   y=C(AAV(x)[0]);   // fetch the address of the A-block for the pointer
   CDASSERT(!AR(y)&&AT(y)&B01+INT,per);  // pointer must be B01 or INT type (if B01, nust be nullptr)
#if defined(__aarch64__)
   if(unlikely(AT(y)&B01)){CDASSERT(0==BAV(y)[0],per);
   if(rcnt<maxrcnt) data[rcnt++]=0; else{
    dvc=alignto(dvc,sizeof(void*)); *(I*)dvc=0; dvc+=sizeof(void*);}  // get nullptr or intptr, save in *dv
   }else{
   if(rcnt<maxrcnt) data[rcnt++]=AV(y)[0]; else{
    dvc=alignto(dvc,sizeof(void*)); *(I*)dvc=AV(y)[0]; dvc+=sizeof(void*);}} // get nullptr or intptr, save in *dv
#else
   if(unlikely(AT(y)&B01)){CDASSERT(0==BAV(y)[0],per); *dv++=0;}else *dv++=AV(y)[0];  // get nullptr or intptr, save in *dv
#endif
  }else if(star){  // pointer, but not boxed atom
   CDASSERT(xr&&((xt&DIRECT)>0),per);                /* pointer can't point at scalar, and it must point to direct values */
   // if type is * (not &), make a safe copy.
   if(star&1){RZ(x=jtmemu(jtinplace,x)); if(zbx)*zv=incorp(x); xv=AV(x);}  // what we install into * must be unaliased (into & is ok)
#if defined(__aarch64__)
   if(rcnt<maxrcnt) data[rcnt++]=(I)xv; else{
   dvc=alignto(dvc,sizeof(void*));
   *(I*)dvc=(I)xv; dvc+=sizeof(void*); /* pointer to J array memory     */
   }
#else
   *dv++=(I)xv;                     /* pointer to J array memory     */
#endif
   CDASSERT(ISDENSETYPE(xt,LIT+C2T+C4T+INT+FL+CMPX),per);  // verify J type is DIRECT why not B01?
   if(unlikely((litsgn | ((cbit&(((I)1<<('b'-'a'))|((I)1<<('f'-'a'))|((I)1<<('s'-'a'))|((I)1<<('z'-'a'))|((I)SY_64<<('i'-'a'))))-1))>=0)){
    cipv[cipcount]=xv;              /* convert in place arguments */
    cipn[cipcount]=xn;
    cipt[cipcount]=c;
    ++cipcount;
   }
  }else{
#if defined(C_CD_ARMHF) || defined(C_CD_ARMEL)
#define ARMARGS 1
#else
#define ARMARGS 0
#endif
   // coded to avoid switch, which mispredicts
   if(likely((cbit&(((I)1<<('b'-'a'))|((I)1<<('c'-'a'))|((I)1<<('w'-'a'))|((I)1<<('u'-'a'))|((I)1<<('s'-'a'))|((I)1<<('i'-'a'))|((I)1<<('x'-'a'))|((I)!(SY_MACPPC||SY_UNIX64||ARMARGS||!SY_64)<<('d'-'a'))))!=0)){
    // integers.  Most singletons are integers.  d on SY_64 and no weird calling convention is handled like an 8-byte integer
    // general plan for integers is: fetch 8 bytes (overfetch OK); get length; sign-extend at that length; mask off if not sign-extended
    // code: b  c  w  u  s  i  x  d
    // ASC:  62 63 77 75 73 69 78 64
    // >>1   31 31 3b 3a 39 34 3c 32
    // &15   1  1  11 10 9  4  12  2  decimal
    // len   0  0  1  2  1  2  3/2 3
    // sign  x  0  0  0  1  1  x   x
    I iwd=*xv;  // fetch 8 bytes, possibly overfetching
#if SY_64
    I lglen=(0x03640230 >>(c&0x1e))&3;  // lg2(len of data in bytes)  0000 001x 0110 0100 0000 0010 0011 0000
#else
    I lglen=(0x02640200 >>(c&0x1e))&3;  // lg2(len of data in bytes)  0000 001x 0110 0100 0000 0010 0000 0000
#endif
    I sxt=(0x00080200 >>(c&0x1e))&2;   // 2 if sign-extend required   0000 00x0 0000 1000 0000 0010 00x0 x000
    I nsig=(I)1<<(lglen+3);  // # significant bits in iwd
    I sxtwd=(iwd<<(BW-nsig))>>(BW-nsig);  // install sign extend over ignored bits
    iwd&=~((sxt-2)<<(nsig-1));  // if not sign-extend, clear upper bits.  Can't shift by BW.  If nsig is 8, sxt=1 ANDs with ~0, sxt=0 ANDs with ~0xFF..FF00
#if defined(__aarch64__)
   if(rcnt<maxrcnt) data[rcnt++]=iwd; else{
#if defined(__APPLE__)
    dvc=alignto(dvc,1<<lglen);
    *(I*)dvc=iwd; // write extended result
    dvc+=1<<lglen;
#else
    dvc=alignto(dvc,sizeof(I));
    *(I*)dvc=iwd; // write extended result
    dvc+=sizeof(I);
#endif
    }
#else
    *dv++=iwd;  // write extended result
#endif
    // long way    switch(c) /(  // not a pointer.  Must be a data atom.  If fixed-point convert to I; if float convert to D
// long way   case 'b': *dv++=(BYTE)*xv;break;
// long way   case 'c': *dv++=*(C*)xv;  break;
// long way   case 'w': *dv++=*(US*)xv; break;
// long way   case 'u': *dv++=*(C4*)xv; break;
// long way   case 's': *dv++=(S)*xv;   break;
// long way   case 'i': *dv++=(int)*xv; break;
// long way   case 'x': *dv++=*xv;      break;
// long way   case 'f':
   }else if(likely((SY_MACPPC||SY_UNIX64||ARMARGS||!SY_64)&&c=='d')){  // double-precision arg with a weird calling sequence
// long way    case 'd':
#if SY_MACPPC
             dd[dcnt++]=*(D*)xv;
#endif
#if SY_UNIX64
#if defined(__PPC64__)
             /* always need to increment dv, the contents get used from the 14th D */
             *(D*)dv++=dd[dcnt++]=*(D*)xv;
#elif defined(__aarch64__)
             if(dcnt<maxdcnt) dd[dcnt++]=*(D*)xv;
             else { dvc=alignto(dvc,sizeof(D)); *(I*)dvc=*xv; dvc+=sizeof(D); }
#elif defined(__x86_64__)
             dd[dcnt++]=*(D*)xv;
             if(dcnt>8){ /* push the 9th D and more on to stack (must be the 7th I onward) */
               if(dv-data>=6)*dv++=*xv;else data[dcnt-3]=*xv;}
#endif
#endif
#if !SY_UNIX64
#ifdef C_CD_ARMHF
            {if(dcnt<16){
               if (dcnt==fcnt) fcnt+=2;
               *(D*)(dd+dcnt++)= *(D*)xv; dcnt++;
             }else{
               if(dv-data>=4){
                 if((dv-data)&1)dv++;
                 *(D*)(dv++)=*(D*)xv; dv++;
                 dcnt=(dv-data)+12;
               }else{
                 dcnt=MAX(fcnt,dcnt);
                 dcnt = (dcnt+1)&-2;
                 *(D*)(data+dcnt++ -12)=*(D*)xv; dcnt++;
             }}}
#else
#ifdef C_CD_ARMEL
             if((data-dv)&1) *dv++=0;   /* 8-byte alignment for double */
#endif
             *dv++=xv[0];
#if !SY_64
             *dv++=xv[1];
#endif
#endif
#endif
   }else if(likely(c=='f')){   // single-precision arg
#if SY_MACPPC
          dd[dcnt++]=(float)*(D*)xv;
#endif
#if SY_64 && (SY_LINUX  || SY_MAC)
  #if defined(__PPC64__)
     /* +1 put the float in low bits in dv, but dd has to be D */
#if C_LE
     *dv=0; *(((float*)dv++))=(float)(dd[dcnt++]=*(D*)xv);
#else
     *dv=0; *(((float*)dv++)+1)=(float)(dd[dcnt++]=*(D*)xv);
#endif
     /* *dv=0; *(((float*)dv++)+1)=dd[dcnt++]=(float)*(D*)xv; */
  #elif defined(__aarch64__)
     {f=(float)*(D*)xv;
      if (dcnt<maxdcnt){dd[dcnt]=0; *(float*)(dd+dcnt++)=f;}
#if defined(__APPLE__)
      else {dvc=alignto(dvc,sizeof(float)); *(float*)dvc=f; dvc+=sizeof(float); }}
#else
      else {dvc=alignto(dvc,sizeof(I)); *(I*)dvc=0; *(float*)dvc=f; dvc+=sizeof(I); }}
#endif
  #elif defined(__x86_64__)
     {f=(float)*(D*)xv; dd[dcnt]=0; *(float*)(dd+dcnt++)=f;
      if(dcnt>8){ /* push the 9th F and more on to stack (must be the 7th I onward) */
        if(dv-data>=6)*(float*)(dv++)=f;else *(float*)(data+dcnt-3)=f;}}
  #endif
#else
#ifdef C_CD_ARMHF
            {f=(float)*(D*)xv;
             if(fcnt<16&&dcnt<=16){
               dd[fcnt]=0; *(float*)(dd+fcnt++)=f;
               if ((0==(fcnt&1)) && (fcnt<dcnt)) fcnt=dcnt;
               if ((1==(fcnt&1)) && (fcnt>dcnt)) dcnt=fcnt+1;
             }else{
               if(dv-data>=4){
                 *(float*)(dv++)=f;
                 fcnt=(dv-data)+12;
               }else{
                 fcnt=MAX(fcnt,dcnt);
                 *(((float*)data)+fcnt++ -12)=f;
             }}}
#else
             f=(float)*(D*)xv; *dv++=*(int*)&f;
#endif
#endif
   }
  }
 }  // end of loop for each argument
#ifdef C_CD_ARMHF
// CDASSERT(16>=fcnt,DELIMIT);
// CDASSERT(16>=dcnt,DELIMIT);
 if((fcnt>16||dcnt>16)&&dv-data<=4)dv=data+MAX(fcnt,dcnt)-12;  /* update dv to point to the end */
#elif SY_UNIX64 && defined(__x86_64__)
 if(dcnt>8&&dv-data<=6)dv=data+dcnt-2; /* update dv to point to the end */
#elif SY_UNIX64 && defined(__aarch64__)
 if(rcnt==maxrcnt||dcnt==8) dv=(I*)alignto(dvc,sizeof(I));   /* update (char*) dvc to (I*) dv , 8 byte aligned but will be 16 aligned during stdcalli etc */
 else dv=data+rcnt;                                          /* stack memory not used dv to point to the end of data */
#elif !SY_64
 CDASSERT(dv-data<=NCDARGS,DECOUNT); /* D needs 2 I args in 32bit system, check it again. */
#endif

 DO(cipcount, convertdown(cipv[i],cipn[i],cipt[i]););  /* convert I to s and int and d to f as required */
 // allocate the result area and point to it
 if(zbx){GA00(x,cc->zt,1,0); xv=AV(x); *(A*)zv0=incorp(x);}else xv=(I*)zv0;  // must not box an inplaceable.  xv points to where the function will store its result: in zv or in a fresh box
 // get the address of the function
 if('1'==cc->cc){fp=(FARPROC)*((I)cc->fp+(I*)*(I*)*data); CDASSERT(fp!=0,DEBADFN);}else fp=cc->fp;

 // call it.  This is a safe recursion point.  Back up to IDLE
 jt->recurstate&=~RECSTATEBUSY;  // back to IDLE/PROMPT state
 docall(fp, data, dv-data, dd, dcnt, cc->zl, xv, cc->alternate);  // call the function, set the result
 jt->recurstate|=RECSTATEBUSY;  // cd complete, go back to normal running state, BUSY normally or RECUR if a prompt is pending

 DO(cipcount, convertup(cipv[i],cipn[i],cipt[i]);); /* convert s and int to I and f to d as required */
#if SY_WIN32
 t= GetLastError();
 if(cc->fpreset)_fpreset();         /* delphi dll (and others) damage fp state */
#endif
#if SYS&SYS_UNIX
 t=errno;
#endif
 if(unlikely(t!=0))jt->getlasterror=t;
 R 1;
}

F2(jtcd){A z;C *wv,*zv;CCT*cc;I k,m,n,p,q,t,wr,*ws,wt;
 F2PREFIP;
 ARGCHK2(a,w);
 AFLAGPRISTNO(w)  // we transfer boxes from w to the result, thereby letting them escape.  That makes w non-pristine
 if(1<AR(a)){I rr=AR(w); rr=rr==0?1:rr; R rank2ex(a,w,DUMMYSELF,1L,rr,1L,rr,jtcd);}
 wt=AT(w); wr=AR(w); ws=AS(w); PRODX(m,wr-1,ws,1);   // m = # 1-cells of w
 ASSERT(!ISSPARSE(wt),EVDOMAIN);
 ASSERT(LIT&AT(a),EVDOMAIN);
 C* enda=&CAV(a)[AN(a)]; C endc=*enda; *enda=0; cc=cdparse(a,0); *enda=endc; RZ(cc); // should do outside rank2 loop?
 n=cc->n;  // n=# arguments
 I nn; CDASSERT(n==SHAPEN(w,wr-1,nn),DECOUNT);
 if(cc->zbx){GATV(z,BOX,m*(1+n),MAX(1,wr),ws); AS(z)[AR(z)-1]=1+n;}  // allocate n boxes for each result arg, plus 1 for the result
 else{CDASSERT('*'!=cc->zl,DEDEC); GA(z,cc->zt,m,MAX(0,wr-1),ws);}  // if fast form, just allocate the return value
 // z is always nonrecursive
 if(unlikely((-m&-n&SGNIFNOT(wt,BOXX))<0)){   // if w is NOT boxed, and a and w arenot both empty
  t=0; DQ(n, k=cdjtype(cc->starlett[i].tletter); t=MAX(t,k););
  CDASSERT(HOMO(t,wt),DEPARM);
  if(!ISDENSETYPE(wt,B01+INT+FL+LIT+C2T+C4T))RZ(w=cvt(wt=t,w));  // if w sparse or not DIRECT, convert it
 }
 wv=CAV(w); zv=CAV(z); k=bpnoun(wt);
 if(1==m)RZ(jtcdexec1(jtinplace,cc,zv,wv,k,wt,0))
 else{p=n*k; q=cc->zbx?sizeof(A)*(1+n):bpnoun(AT(z)); DQ(m, RZ(jtcdexec1(jtinplace,cc,zv,wv,k,wt,0)); wv+=p; zv+=q;);}
 R z;
}    /* 15!:0 */


#if SY_WIN32
#define FREELIB FreeLibrary
#endif
#if (SYS & SYS_UNIX)
#define FREELIB dlclose
#endif

void dllquit(J jt){I j,*v;
 if(!JT(jt,cdstr))R;   // if we never initialized, don't free
 A *av=AAV0(JT(jt,cdarg));  // point to A blocks for CCTs
 DQ(AM(JT(jt,cdarg)), if(((CCT*)IAV0(av[i]))->hloaded)FREELIB(((CCT*)IAV0(av[i]))->h); fr(av[i]));   // unload all libraries, and free the CCT blocks
 mvc(AN(JT(jt,cdstr)),CAV(JT(jt,cdstr)),1,MEMSET00);
 mvc(SZI*AN(JT(jt,cdhash)),CAV(JT(jt,cdhash)),1,MEMSETFF); mvc(SZI*AN(JT(jt,cdhashl)),CAV(JT(jt,cdhashl)),1,MEMSETFF); 
 AM(JT(jt,cdstr))=AM(JT(jt,cdarg))=AM(JT(jt,cdhash))=AM(JT(jt,cdhashl))=0;  // reset all tables to empty
 // leave the tables allocated
}    /* dllquit - shutdown and cdf clean up dll call resources */

F1(jtcdf){ASSERTMTV(w); dllquit(jt); R mtm;}
     /* 15!:5 */

/* return error info from last cd domain error - resets to DEOK */
F1(jtcder){I4 t; ASSERTMTV(w); t=jt->dlllasterror; jt->dlllasterror=DEOK; R v2(t&0xff,t>>8);}
     /* 15!:10 */

/* return errno info from last cd with errno not equal to 0 - resets to 0 */
F1(jtcderx){I4 t;C buf[1024];
 ASSERTMTV(w); t=jt->getlasterror; jt->getlasterror=0;

#if SY_WIN32 && !SY_WINCE
 FormatMessageA(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, (DWORD)t,
    MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),  /* Default language */
    buf, sizeof(buf), 0);
#endif

#if SY_WINCE
 {
 WCHAR wbuf[1024];
 FormatMessage(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, (DWORD)t,
    MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),  /* Default language */
    wbuf, sizeof(buf), 0);
 strcpy(buf,toascbuf(wbuf));
 }
#endif

#if SYS&SYS_UNIX
 {const char *e = dlerror(); strcpy (buf, e?e:"");}
#endif
 R jlink(sc(t),cstr(buf));
}    /* 15!:11  GetLastError information */

F1(jtmema){I k; RE(k=i0(w)); R sc((I)MALLOC(k));} /* ce */
     /* 15!:3  memory allocate */

F1(jtmemf){I k; RE(k=i0(w)); FREE((void*)k); R num(0);}
     /* 15!:4  memory free */

F1(jtmemr){C*u;I m,n,t,*v;US*us;C4*c4;
 ARGCHK1(w);
 ASSERT(INT&AT(w),EVDOMAIN);
 ASSERT(1==AR(w),EVRANK);
 n=AN(w); v=AV(w);
 ASSERT(3==n||4==n,EVLENGTH);
 m=v[2]; t=3==n?LIT:v[3]; u=(C*)(v[0]+v[1]);  // m=length in items; t=type to create; u=address to read from
 ASSERT(t&B01+LIT+C2T+C4T+INT+FL+CMPX+SBT,EVDOMAIN);
 if(-1==m){
  ASSERT(t&LIT+C2T+C4T,EVDOMAIN);
  if(t&LIT) m=strlen(u);
  else if(t&C2T) {
   if(sizeof(US)==sizeof(wchar_t)) m=wcslen((wchar_t*)u);
   else {m=0; us=(US*)u; while(*us++)m++;}
  }
  else {
   if(sizeof(C4)==sizeof(wchar_t)) m=wcslen((wchar_t*)u);
   else {m=0; c4=(C4*)u; while(*c4++)m++;}
  }
 }
#if SY_WIN32
// This function is obsolete and should not be used
// ASSERT(!IsBadReadPtr(u,m*k),EVDOMAIN);
#endif
 R vecb01(t,m,u);
}    /* 15!:1  memory read */

// write to memory.  w is addr,offset,count[,type]
F2(jtmemw){C*u;I m,n,t,*v;
 ARGCHK2(a,w);
 ASSERT(INT&AT(w),EVDOMAIN);
 ASSERT(1==AR(w),EVRANK);
 n=AN(w); v=AV(w);
 ASSERT(3==n||4==n,EVLENGTH);
 m=v[2]; t=3==n?LIT:v[3]; u=(C*)(v[0]+v[1]);
 ASSERT(ISDENSETYPE(t,B01+LIT+C2T+C4T+INT+FL+CMPX+SBT),EVDOMAIN);
 ASSERT(m==AN(a)||t&LIT+C2T+C4T&&1==AR(a)&&(m-1)==AN(a),EVLENGTH);
 if(B01&AT(a)&&t&INT) RZ(a=cvt(INT,a));
 if(INT&AT(a)&&t&B01) RZ(a=cvt(B01,a));
 ASSERT(TYPESEQ(t,AT(a)),EVDOMAIN);
#if SY_WIN32
// This function is obsolete and should not be used
// ASSERT(!IsBadWritePtr(u,m*k),EVDOMAIN);
#endif
 I mvlen=AN(a)<<bplg(t);
 MC(u,AV(a),mvlen);   // copy the valid bytes of a
 if(m>AN(a))mvc(1LL<<bplg(t),u+mvlen,1,MEMSET00);  // append zero if called for
 R mtm;
}    /* 15!:2  memory write */

// 15!:15 memu - make a copy of y if it is not writable (inplaceable and not read-only)
// We have to check jt in case this usage is in a fork that will use the block later
F1(jtmemu) { F1PREFIP; ARGCHK1(w); if(!((I)jtinplace&JTINPLACEW && (AC(w)<(AFLAG(w)<<((BW-1)-AFROX)))))w=ca(w);
 // We will NUL-terminate a string in case called routines need it (not sure this is necessary)
#if SY_64
 if(unlikely((AT(w)&LAST0)!=0))*(C4*)&CAV(w)[AN(w)*bpnoun(AT(w))]=0;  // on 64-bit systems, always safe to add 4 bytes at end of data
#else
 if(unlikely((AT(w)&LAST0)!=0)){I nulofst=AN(w)*bpnoun(AT(w)); *(UI4*)&CAV(w)[nulofst&~3]&=~((UI4)-1<<((nulofst&3)<<LGBB));}  // zero out the 4-byte word starting at NUL.  addr 00->mask w/0, 01->ff 02->ffff 03->ffffff
#endif
 RETF(w);
}
F2(jtmemu2) { RETF(ca(w)); }  // dyad - force copy willy-nilly

F1(jtgh15){A z;I k; RE(k=i0(w)); RZ(z=gah(k,0L)); ACINIT(z,ACUC2); R sc((I)z);}   // ra the header
     /* 15!:8  get header */

F1(jtfh15){I k; RE(k=i0(w)); fr((A)k); R num(0);}
     /* 15!:9  free header */

// 15!:7.  w has the address of a header.  Put that address in play as an A block
F1(jtdllsymset){ARGCHK1(w); R (A)i0(w);}      /* do some validation here */

/* dll callback routines */
static J cbjt; /* callbacks require jt and can only use the one */

static I cbold(I n,I *pi){char d[256],*p;A r;I i;
 J jt=cbjt;
 strcpy(d, "cdcallback ");
 p=d+strlen(d);
 for(i=0;i<n;++i){sprintf(p,FMTI,pi[i]); *p=(*p=='-' ? '_':*p);p+=strlen(p);*p++=' ';}
 if (!n) { *p++='\''; *p++='\''; }
 *p=0;
 r=exec1(cstr(d));
 if(!r||AR(r)) R 0;
 if(INT&AT(r)) R AV(r)[0];
 if(B01&AT(r)) R ((BYTE*)AV(r))[0];
 R 0;
}

static I cbnew(){A r;
 J jt=cbjt;
 r=exec1(cstr("cdcallback''"));
 if(!r||AR(r)) R 0;
 if(INT&AT(r)) R AV(r)[0];
 if(B01&AT(r)) R ((BYTE*)AV(r))[0];
 R 0;
}

#if SY_MAC && !SY_64
int x15lseek32(int fh,int off, int type){R (int)lseek(fh,(off_t)off,type);}// mac osx 32
#endif

/* start of code generated by J script x15_callback.ijs */
#define CBTYPESMAX 10 /* result and 9 args */
static I cbx[CBTYPESMAX-1];
I cbxn=0;

static I CALLBACK cb0(){I x[]={0};R cbold(0,x);}
static I CALLBACK cb1(I a){I x[]={a};R cbold(1,x);}
static I CALLBACK cb2(I a,I b){I x[]={a,b};R cbold(2,x);}
static I CALLBACK cb3(I a,I b,I c){I x[]={a,b,c};R cbold(3,x);}
static I CALLBACK cb4(I a,I b,I c,I d){I x[]={a,b,c,d};R cbold(4,x);}
static I CALLBACK cb5(I a,I b,I c,I d,I e){I x[]={a,b,c,d,e};R cbold(5,x);}
static I CALLBACK cb6(I a,I b,I c,I d,I e,I f){I x[]={a,b,c,d,e,f};R cbold(6,x);}
static I CALLBACK cb7(I a,I b,I c,I d,I e,I f,I g){I x[]={a,b,c,d,e,f,g};R cbold(7,x);}
static I CALLBACK cb8(I a,I b,I c,I d,I e,I f,I g,I h){I x[]={a,b,c,d,e,f,g,h};R cbold(8,x);}
static I CALLBACK cb9(I a,I b,I c,I d,I e,I f,I g,I h,I i){I x[]={a,b,c,d,e,f,g,h,i};R cbold(9,x);}
static I cbv[]={(I)&cb0,(I)&cb1,(I)&cb2,(I)&cb3,(I)&cb4,(I)&cb5,(I)&cb6,(I)&cb7,(I)&cb8,(I)&cb9};

static I CALLBACK cbx0(){cbxn=0;R cbnew();}
static I CALLBACK cbx1(I a){cbxn=1;cbx[0]=a;R cbnew();}
static I CALLBACK cbx2(I a,I b){cbxn=2;cbx[0]=a;cbx[1]=b;R cbnew();}
static I CALLBACK cbx3(I a,I b,I c){cbxn=3;cbx[0]=a;cbx[1]=b;cbx[2]=c;R cbnew();}
static I CALLBACK cbx4(I a,I b,I c,I d){cbxn=4;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;R cbnew();}
static I CALLBACK cbx5(I a,I b,I c,I d,I e){cbxn=5;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;R cbnew();}
static I CALLBACK cbx6(I a,I b,I c,I d,I e,I f){cbxn=6;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;cbx[5]=f;R cbnew();}
static I CALLBACK cbx7(I a,I b,I c,I d,I e,I f,I g){cbxn=7;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;cbx[5]=f;cbx[6]=g;R cbnew();}
static I CALLBACK cbx8(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=8;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;cbx[5]=f;cbx[6]=g;cbx[7]=h;R cbnew();}
static I CALLBACK cbx9(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=9;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;cbx[5]=f;cbx[6]=g;cbx[7]=h;cbx[8]=i;R cbnew();}
static I cbvx[]={(I)&cbx0,(I)&cbx1,(I)&cbx2,(I)&cbx3,(I)&cbx4,(I)&cbx5,(I)&cbx6,(I)&cbx7,(I)&cbx8,(I)&cbx9};

#if SY_WIN32
static I _cdecl cbxalt0(){cbxn=0;R cbnew();}
static I _cdecl cbxalt1(I a){cbxn=1;cbx[0]=a;R cbnew();}
static I _cdecl cbxalt2(I a,I b){cbxn=2;cbx[0]=a;cbx[1]=b;R cbnew();}
static I _cdecl cbxalt3(I a,I b,I c){cbxn=3;cbx[0]=a;cbx[1]=b;cbx[2]=c;R cbnew();}
static I _cdecl cbxalt4(I a,I b,I c,I d){cbxn=4;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;R cbnew();}
static I _cdecl cbxalt5(I a,I b,I c,I d,I e){cbxn=5;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;R cbnew();}
static I _cdecl cbxalt6(I a,I b,I c,I d,I e,I f){cbxn=6;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;cbx[5]=f;R cbnew();}
static I _cdecl cbxalt7(I a,I b,I c,I d,I e,I f,I g){cbxn=7;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;cbx[5]=f;cbx[6]=g;R cbnew();}
static I _cdecl cbxalt8(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=8;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;cbx[5]=f;cbx[6]=g;cbx[7]=h;R cbnew();}
static I _cdecl cbxalt9(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=9;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;cbx[5]=f;cbx[6]=g;cbx[7]=h;cbx[8]=i;R cbnew();}
static I cbvxalt[]={(I)&cbxalt0,(I)&cbxalt1,(I)&cbxalt2,(I)&cbxalt3,(I)&cbxalt4,(I)&cbxalt5,(I)&cbxalt6,(I)&cbxalt7,(I)&cbxalt8,(I)&cbxalt9};
#endif
/* end of code generated by J script x15_callback.ijs */

F1(jtcallback){
 cbjt=jt; /* callbacks don't work with multiple instances of j */
 ARGCHK1(w);
 if(LIT&AT(w))
 {
  I cnt,alt;C c;C* s;
  ASSERT(1>=AR(w),EVRANK);
  s=CAV(str0(w));
  alt=0; while(*s==' ')++s; if('+'==*s){alt=1; ++s;}
#if !SY_WIN32
  ASSERT(0==alt,EVDOMAIN);
#endif
  cnt=0; /* count x's in type declaration (including result) */
  while(c=*s++){
   if(' '==c)continue;
   ++cnt;
   ASSERT(c=='x',EVDOMAIN);
   ASSERT(0==*s||' '==*s,EVDOMAIN);
  }
  ASSERT(cnt>0&&cnt<CBTYPESMAX,EVDOMAIN);
#if SY_WIN32
  R sc((alt?cbvxalt:cbvx)[--cnt]); /* select callback based on alt * args */
#else
  R sc(cbvx[--cnt]); /* select callback based on alt * args */
#endif
 }
 else
 {
  I k;
  RE(k=i0(w));
  ASSERT((UI)k<(UI)sizeof(cbv)/SZI, EVINDEX);
  R sc(cbv[k]);
 }
}    /* 15!:13 */

F1(jtnfes){I k;I r;
 RE(k=i0(w));
 ASSERT(BETWEENC(k,0,1),EVDOMAIN);
 r=JT(jt,nfe);
 JT(jt,nfe)=k;
 R sc(r);
} /* 15!:16 toggle native front end (nfe) state */

F1(jtcallbackx){
 ASSERTMTV(w);
 R vec(INT,cbxn,cbx);
} /* 15!:17 return x callback arguments */

F1(jtcdjt){
 ASSERTMTV(w);
 R sc((I)(intptr_t)jt);
} /* 15!:19 return jt */

F1(jtcdlibl){
 ARGCHK1(w);
 ASSERT(LIT&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(AN(w),EVLENGTH);
 if(!AM(JT(jt,cdarg)))R num(0);
 R sc((I)cdlookupl(CAV(w)));
}    /* 15!:20 return library handle */

F1(jtcdproc1){CCT*cc;
 ARGCHK1(w);
 ASSERT(LIT&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(AN(w),EVLENGTH);
 C* enda=&CAV(w)[AN(w)]; C endc=*enda; *enda=0; cc=cdparse(w,1); *enda=endc; RE(cc); // should do outside rank2 loop?
 R sc((I)cc->fp);
}    /* 15!:21 return proc address */

#ifdef MMSC_VER
#pragma warning(disable: 4276)
#endif

#if SY_WIN32 && defined(OLECOM)
#define VARIANT void
CDPROC int _stdcall JBreak(J jt);
CDPROC int _stdcall JIsBusy(J jt);
CDPROC int _stdcall JGet(J jt, C* name, VARIANT* v);
CDPROC int _stdcall JGetB(J jt, C* name, VARIANT* v);
CDPROC int _stdcall JSet(J jt, C* name, VARIANT* v);
CDPROC int _stdcall JSetB(J jt, C* name, VARIANT* v);
CDPROC int _stdcall JErrorText(J jt, long ec, VARIANT* v);
CDPROC int _stdcall JClear(J jt);
CDPROC int _stdcall JTranspose(J jt, long b);
CDPROC int _stdcall JErrorTextB(J jt, long ec, VARIANT* v);
CDPROC int _stdcall JDoR(J jt, C* p, VARIANT* v);
CDPROC int _stdcall JInt64R(J jt, long b);
#endif

// procedures in jlib.h
static const void* jfntaddr[]={
JDo,
JErrorTextM,
JFree,
JGetA,
JGetLocale,
JGetM,
JGetR,
JInit,
JSM,
JSMX,
JSetA,
JSetM,
Jga,
#if SY_WIN32 && defined(OLECOM)
JBreak,
JClear,
JDoR,
JErrorText,
JErrorTextB,
JGet,
JGetB,
JIsBusy,
JSet,
JSetB,
JTranspose,
JInt64R,
#endif
};

static const C* jfntnm[]={
"JDo",
"JErrorTextM",
"JFree",
"JGetA",
"JGetLocale",
"JGetM",
"JGetR",
"JInit",
"JSM",
"JSMX",
"JSetA",
"JSetM",
"Jga",
#if SY_WIN32 && defined(OLECOM)
"JBreak",
"JClear",
"JDoR",
"JErrorText",
"JErrorTextB",
"JGet",
"JGetB",
"JIsBusy",
"JSet",
"JSetB",
"JTranspose",
"JInt64R",
#endif
};

F2(jtcdproc2){C*proc;FARPROC f;HMODULE h;
 ARGCHK2(a,w);
 ASSERT(LIT&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(AN(w),EVLENGTH);
 proc=CAV(w);
 RE(h=(HMODULE)i0(a));
 if(!h){I k=-1;
  DO(sizeof(jfntnm)/sizeof(C*), if(((I)strlen(jfntnm[i])==AN(w))&&!strncmp(jfntnm[i],proc,AN(w))){k=i; break;});
  f=(k==-1)?(FARPROC)0:(FARPROC)jfntaddr[k];
 }else{
#if SY_WIN32 && !SY_WINCE
  f=GetProcAddress(h,'#'==*proc?(LPCSTR)(I)atoi(proc+1):(LPCSTR)proc);
#endif
#if SY_WINCE
  f=GetProcAddress(h,tounibuf(proc));
#endif
#if (SYS & SYS_UNIX)
  f=(FARPROC)dlsym(h,proc);
#endif
 }
 CDASSERT(f!=0,DEBADFN);
 R sc((I)f);
}    /* 15!:21 return proc address */

