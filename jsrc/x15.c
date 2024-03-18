/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
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

#if defined(C_CD_ARMHF) || defined(C_CD_ARMEL)
#define ARMARGS 1
#else
#define ARMARGS 0
#endif

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

// align memory pointer to natural alignment
#define alignto(dvc,align) (void*)((uintptr_t)((dvc)+((align)-1)) & ~((align)-1))
/*
static void* alignto(void* dvc, int align){
// fprintf(stderr,"%p %d %p \n", dvc, align, (void*)((uintptr_t)((dvc)+((align)-1)) & ~((align)-1)) );
return (void*)((uintptr_t)((dvc)+((align)-1)) & ~((align)-1));
}
*/

#define SY_UNIX64 (SY_64 && (SY_LINUX || SY_MAC || SY_FREEBSD || SY_OPENBSD))

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

#if !defined(__wasm__) && !defined(TARGET_IOS)
#include <dlfcn.h>
#endif

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
  C type;               // type of arg: 0=char 1=int 2=fl 3=complex 4=no type given (* bare)
  C lgsz;            // lg(atom size) - for complex, half an atom
  C flags;            // 1:set if this type is a short type (b s i f) that allows LIT as a data source 2: set if type that can be sign-extended if unboxed
  I4 jtype;           // j type for the cd rg type letter
 } starlett[NCDARGS];   // flag/letter per arg
} CCT;

#if SY_64 && SY_WIN32
extern void double_trick(D,D,D,D);
#endif

#if SYS & (SYS_MACOSX | SYS_LINUX | SYS_FREEBSD | SYS_OPENBSD)
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
 ZEROUPPER; // see comment in io.c
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

// convert down/up.  n is #atoms, t is type 0123, sizes is bits 0-1: source lgsz, 2-3: target lgsz
// we come here only if target size < source size: 3 ways for char, 6 ways for int,3 for float, 1 for complex

static void convertdown(void *pi,I n,C t,C sizes){
 if(unlikely(n==0))R;
// possible values of sizes are
// char: 6 2 (C4T), 1 (C2T)  -1 -> 5 1 0
// int: 11 7 3 (INT), 6 2 (INT4), 1 (INT2) +1 -> 12 8 7 4 3 2
// fl: 11 7 (FL), 6 (SP) +3 -> 14 10 9
// complex: 11 +4= 15
 I ssz=sizes&3; I tsz=sizes>>2;  // extract sizes
 I adj=0x431f>>(t<<2);  // 0 1 2 3 -> -1 1 3 4
 switch((sizes+adj)&0xf){
 // fl cases:
 case 3+0b1011: {float*pt=(float*)pi; D*ps=(D*)pi; DO(n, pt[i]=(float)ps[i];);} break;
 // int cases:
 case 1+0b1011: {I4*pt=(I4*)pi; I*ps=(I*)pi; DO(n, pt[i]=(I4)ps[i];);} break;
 case 1+0b0111: {I2*pt=(I2*)pi; I*ps=(I*)pi; DO(n, pt[i]=(I2)ps[i];);} break;
 case 1+0b0011: {I1*pt=(I1*)pi; I*ps=(I*)pi; DO(n, pt[i]=(I1)ps[i];);} break;
 case 1+0b0110: {I2*pt=(I2*)pi; I4*ps=(I4*)pi; DO(n, pt[i]=(I2)ps[i];);} break;
 case 1+0b0010: {I1*pt=(I1*)pi; I4*ps=(I4*)pi; DO(n, pt[i]=(I1)ps[i];);} break;
 case 1+0b0001: {I1*pt=(I1*)pi; I2*ps=(I2*)pi; DO(n, pt[i]=(I1)ps[i];);} break;
 // char cases:
 case -1+0b0110:{C2*pt=(C2*)pi; C4*ps=(C4*)pi; DO(n, pt[i]=(C2)ps[i];);} break;
 case -1+0b0010:{C2*pt=(C2*)pi; C4*ps=(C4*)pi; DO(n, pt[i]=(C2)ps[i];);} break;
 case -1+0b0001:{C*pt=(C*)pi; C2*ps=(C2*)pi; DO(n, pt[i]=(C)ps[i];);} break;
 // complex case
 case 4+0b1011:
#ifdef _WIN32
  {float_complex*pt=(float_complex*)pi;D*ps=(D*)pi; DO(n, pt[i]=_FCOMPLEX_((float)ps[2*i],(float)ps[1+2*i]););} break;
#else
 {float_complex*pt=(float_complex*)pi;D*ps=(D*)pi; DO(n, pt[i]=(float)ps[2*i]+_Complex_I*(float)ps[1+2*i];);} break;
#endif
 }
}

// convert up, similarly
static void convertup(void *pi,I n,C t,C sizes){
 if(unlikely(n==0))R;
// possible values of sizes are
// char: 6 2 (C4T), 1 (C2T)  -1 -> 5 1 0
// int: 11 7 3 (INT), 6 2 (INT4), 1 (INT2) +1 -> 12 8 7 4 3 2
// fl: 11 7 (FL), 6 (SP) +3 -> 14 10 9
// complex: 11 +4= 15
 I ssz=sizes&3; I tsz=sizes>>2;  // extract sizes
 I adj=0x431f>>(t<<2);  // 0 1 2 3 -> -1 1 3 4
 switch((sizes+adj)&0xf){
 // fl cases:
 case 3+0b1011: {float*ps=(float*)pi; D*pt=(D*)pi; DQ(n, pt[i]=(D)ps[i];);} break;
 // int cases:
 case 1+0b1011: {I4*ps=(I4*)pi; I*pt=(I*)pi; DQ(n, pt[i]=(I)ps[i];);} break;
 case 1+0b0111: {I2*ps=(I2*)pi; I*pt=(I*)pi; DQ(n, pt[i]=(I)ps[i];);} break;
 case 1+0b0011: {I1*ps=(I1*)pi; I*pt=(I*)pi; DQ(n, pt[i]=(I)ps[i];);} break;
 case 1+0b0110: {I2*ps=(I2*)pi; I4*pt=(I4*)pi; DQ(n, pt[i]=(I4)ps[i];);} break;
 case 1+0b0010: {I1*ps=(I1*)pi; I4*pt=(I4*)pi; DQ(n, pt[i]=(I4)ps[i];);} break;
 case 1+0b0001: {I1*ps=(I1*)pi; I2*pt=(I2*)pi; DQ(n, pt[i]=(I2)ps[i];);} break;
 // char cases:
 case -1+0b0110:{C2*ps=(C2*)pi; C4*pt=(C4*)pi; DQ(n, pt[i]=(C4)ps[i];);} break;
 case -1+0b0010:{C*ps=(C*)pi; C4*pt=(C4*)pi; DQ(n, pt[i]=(C4)ps[i];);} break;
 case -1+0b0001:{C*ps=(C*)pi; C2*pt=(C2*)pi; DQ(n, pt[i]=(C2)ps[i];);} break;
 // complex case
 case 4+0b1011: {float_complex*ps=(float_complex*)pi;D*pt=(D*)pi; DQ(n, pt[1+2*i]=(D)cimagf(ps[i]); pt[2*i]=(D)crealf(ps[i]););} break;
 }
}

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

B jtcdinit(JS jjt){A x;JJ jt=MTHREAD(jjt);
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
   if(*lib){
  #ifdef FIXWINUTF8
    wchar_t wlib[_MAX_PATH];
    MultiByteToWideChar(CP_UTF8,0,lib,1+(int)strlen(lib),wlib,_MAX_PATH);
    h=LoadLibraryW(wlib);
  #else
    h=LoadLibraryA(lib);
  #endif
   }else h=GetModuleHandle(NULL);
 #endif
 CDASSERT((UI)h>HINSTANCE_ERROR,DEBADLIB);
#endif
#if defined(__wasm__) || defined(TARGET_IOS)
  CDASSERT(0,DEBADLIB);
#elif SYS & SYS_UNIX
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
#if defined(__wasm__) || defined(TARGET_IOS)
  CDASSERT(0,DEBADLIB);
#elif (SYS & SYS_UNIX)
 f=(FARPROC)dlsym(h,proc);
#endif
 CDASSERT(f!=0,DEBADFN);
 cc->fp=f;
 R cc;
}

     /* J type from type letter */
static C letttotypex[32]={['c'-'a']=LITX, ['w'-'a']=C2TX, ['u'-'a']=C4TX, ['j'-'a']=CMPXX, ['z'-'a']=CMPXX, ['d'-'a']=FLX, ['f'-'a']=FLX, ['s'-'a']=INT2X, ['i'-'a']=INT4X,
  ['b'-'a']=INTX, ['l'-'a']=SY_64?INTX:0, ['x'-'a']=INTX, ['n'-'a']=INTX, };
static I cdjtype(C c){I i=c-'a'; I x=letttotypex[i&0x1f]; I t=1LL<<x; x=i&-0x20?0:x; t=x==0?0:t; R t;} // fetch bit#, convert to onehot; clear to 0 if char out of range or fetched value 0

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

static CCT*jtcdparse(J jt,A a){C c,lib[NPATH],*p,proc[NPATH],*s,*s0;CCT*cc,cct;I an,der,i,pi;
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
 NOUNROLL while(*s==' ')++s;   // skip to first char
 CDASSERT(*s,DEDEC);   // result type is required
 cc->zl=c=*s++;  // fetch type char or *, save as result type
 // verify that result is a valid type
#define vresvalues(w) CCM(w,'c')+CCM(w,'w')+CCM(w,'u')+CCM(w,'b')+CCM(w,'s')+CCM(w,'i')+CCM(w,'l')+CCM(w,'x')+CCM(w,'f')+CCM(w,'d')+CCM(w,'*')+CCM(w,'n')
 CCMWDS(vres) CCMCAND(vres,cand,c)  // see if zl is one of the allowed types
 CDASSERT(CCMTST(cand,c),DEDEC);
 CDASSERT(SY_64||'l'!=c,DEDEC);
#define vargtvalues(w) CCM(w,'c')+CCM(w,'w')+CCM(w,'u')+CCM(w,'b')+CCM(w,'s')+CCM(w,'i')+CCM(w,'l')+CCM(w,'x')+CCM(w,'f')+CCM(w,'d')+CCM(w,'z')+CCM(w,'j')
 CCMWDS(vargt)  // set up for comparisons against list of bytes
 // if result is * followed by valid arg type, ratify it by advancing the pointer over the type (otherwise fail in next test)
 if(c=='*'){
  cc->zt=INT;  // '*' overrides to INT type
  if(*s){CCMCAND(vargt,cand,*s) s+=SGNTO0(CCMSGN(cand,*s));} // if *s is valid, skip over it
 }else{I rt=cdjtype(c); rt=rt&INT1+INT2+INT4?INT:rt; cc->zt=rt;}  // not *, decode type letter.  For compatibility, don't allow result type INT[124], push them to INT
 CDASSERT((*s&~' ')==0,DEDEC);  // we should have skipped over type to reach 0 or SP   * at end is treated as valid?
#ifdef C_CD_NODF // platform does not support f result
 CDASSERT(cc->zl!='f',DEDEC)
#endif

 /* argument type declarations */
 i=-1;
 while(c=*s++){
  if(' '==c)continue;
  ++i; der=DEDEC+256*(1+i);
  CDASSERT(i<NCDARGS,DECOUNT);
  cc->starlett[i].star=0; cc->starlett[i].lgsz=0x10; cc->starlett[i].jtype=0;  // no star, and 'no type given' and 'very big field' (in case of * bare)
  CDASSERT(i||'1'!=cc->cc||'x'==c||'*'==c&&(!*s||' '==*s),der);  // if calltype is '1', 1st arg must be x or * with no letter
  if('*'==c||'&'==c){cc->starlett[i].star=1+(I )('&'==c); c=*s++; if(!c)break; if(' '==c)continue;}  // indicate if * or &; advance to type if any, load it into c, and fall through to process it
  CCMCAND(vargt,cand,c) CDASSERT(CCMTST(cand,c),der);  // vrgt defined above,list of valid arg bytes.  Verify validity, which narrows range to 'a'-'z'
  CDASSERT((c!='z'&&c!='j')||cc->starlett[i].star,der);  // j and z require *
  CDASSERT(SY_64||c!='l',der); c=c=='l'?'x':c;  // l is x, but error on 32-bit
  c=!SY_64&&c=='x'?'i':c;  // replace x with i in 32-bit
#define F2C(c,n) ((UI8)(n)<<(2*(c-'a')))
  cc->starlett[i].jtype=(UI4)cdjtype(c); cc->starlett[i].jtype=cc->starlett[i].jtype&INT1+INT2+INT4?INT:cc->starlett[i].jtype; // get the desired result type  For compatibility, don't allow result type INT[124], push them to INT
  cc->starlett[i].type=((F2C('c',0)|F2C('w',0)|F2C('u',0)|F2C('b',1)|F2C('s',1)|F2C('i',1)|F2C('l',1)|F2C('x',1)|F2C('f',2)|F2C('d',2)|F2C('z',3)|F2C('j',3))>>(2*(c-'a')))&3;  //0=char 1=int 2=fl 3=cmpx
  cc->starlett[i].lgsz=((F2C('c',0)|F2C('w',1)|F2C('u',2)|F2C('b',0)|F2C('s',1)|F2C('i',2)|F2C('l',3)|F2C('x',3)|F2C('f',2)|F2C('d',3)|F2C('z',2)|F2C('j',3))>>(2*(c-'a')))&3;  // lg(atom len needed)
#define F1C(c) ((I)1<<(c-'a'))
#define F1CN(c,n) ((I)(n)<<(c-'a'))
  cc->starlett[i].flags=((F1C('b')|F1C('s')|F1C('f')|F1C('i'))>>(c-'a'))&1;  // integer/float type that can be sourced from a LIT
#ifdef C_CD_NODF // platform does not support f or d args
 CDASSERT(cc->starlett[i].star==1 || (cc->starlett[i].type!=2),der);
#endif
  cc->starlett[i].flags|=2*(((F1C('b')|F1C('c')|F1C('w')|F1C('u')|F1C('s')|F1C('i')|F1C('x')|F1CN('d',(I)!(SY_MACPPC||SY_UNIX64||ARMARGS||!SY_64)))>>(c-'a'))&1);

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

#define CDT(x,y) (8*((0x161C04>>(x))&7)+((0x161C04>>(y))&0x7))  // encode 0 1 2 3 9 10 17 18-> 4 2 1 0 6 7 3 5  1011. .... .... .... 0100

static I*jtconvert0(J jt,I zt,I*v,I wt,C*u){D p,q;I k=0;US s;C4 s4;
 if(unlikely((zt|wt)&((NOUN|SPARSE)&~(B01+LIT+INT+INT2+INT4+FL+C2T+C4T))))R 0;  // if unallowed type, abort
 switch(CDT(CTTZ(zt),CTTZ(wt))){
 default:           R 0;
 case CDT(FLX, B01X): *(D*)v=(D)*(B*)u; break;
 case CDT(FLX, INTX): *(D*)v=(D)*(I*)u; break;
 case CDT(FLX,INT2X): *(D*)v=(D)*(I2*)u; break;
 case CDT(FLX,INT4X): *(D*)v=(D)*(I4*)u; break;
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
 case CDT(INTX,B01X): *(I*)v=*(B*)u; break;
 case CDT(INTX,INTX): *(I*)v=*(I*)u; break;
 case CDT(INTX,INT2X): *(I*)v=*(I2*)u; break;
 case CDT(INTX,INT4X): *(I*)v=*(I4*)u; break;
 case CDT(INTX,FLX ):
#if SY_64
  p=*(D*)u; q=jround(p);
  if(!ISFTOIOK(p,q))R 0;  // must equal int, possibly out of range.  Exact equality is common enough to test for
  *v=(I)q;
#else
  p=*(D*)u; q=jfloor(p);
  if(p<IMIN*(1+FUZZ)||IMAX*(1+FUZZ)<p)R 0;
  if(FFEQ(p,q))*v=(I)q; else if(FFEQ(p,1+q))*v=(I)(1+q); else R 0;
#endif
  break;
 case CDT(INT2X,B01X): *(I2*)v=*(B*)u; break;
 case CDT(INT2X,INTX): *(I2*)v=*(I*)u; break;
 case CDT(INT2X,INT2X): *(I2*)v=*(I4*)u; break;
 case CDT(INT2X,INT4X): *(I2*)v=*(I4*)u; break;
 case CDT(INT2X,FLX ):
#if SY_64
  p=*(D*)u; q=jround(p);
  if(!ISFTOIOK(p,q))R 0;  // must equal int, possibly out of range.  Exact equality is common enough to test for
  *(I2*)v=(I2)q;
#else
  p=*(D*)u; q=jfloor(p);
  if(p<IMIN*(1+FUZZ)||IMAX*(1+FUZZ)<p)R 0;
  if(FFEQ(p,q))*(I2*)v=(I)q; else if(FFEQ(p,1+q))*(I2*)v=(I)(1+q); else R 0;
#endif
  break;
 case CDT(INT4X,B01X): *(I4*)v=*(B*)u; break;
 case CDT(INT4X,INTX): *(I4*)v=*(I*)u; break;
 case CDT(INT4X,INT2X): *(I4*)v=*(I4*)u; break;
 case CDT(INT4X,INT4X): *(I4*)v=*(I4*)u; break;
 case CDT(INT4X,FLX ):
#if SY_64
  p=*(D*)u; q=jround(p);
  if(!ISFTOIOK(p,q))R 0;  // must equal int, possibly out of range.  Exact equality is common enough to test for
  *(I4*)v=(I4)q;
#else
  p=*(D*)u; q=jfloor(p);
  if(p<IMIN*(1+FUZZ)||IMAX*(1+FUZZ)<p)R 0;
  if(FFEQ(p,q))*(I4*)v=(I)q; else if(FFEQ(p,1+q))*(I4*)v=(I)(1+q); else R 0;
#endif
  break;
 }
 R v;
}    /* convert a single atom. I from D code adapted from IfromD() in k.c */

// make one call to the DLL.
// if cc->zbx is true, zv0 points to AAV(z) where z is the block that will hold the list of boxes that
// will be the result of 15!:0.  z is always nonrecursive
static B jtcdexec1(J jt,CCT*cc,C*zv0,C*wu,I wk,I wt,I wd){A*wv=(A*)wu,x,y,*zv;B zbx;
    C *u;FARPROC fp;float f;I cipcount=0,cv0[2],
    data[NCDARGS*2],dcnt=0,fcnt=0,*dv,i,n,per,xn,xr,xt,*xv; DoF dd[NCDARGS];
    // conversion-in-place info.  For types that convert in place, we remember the addr/length/type/source length/target length
    struct {
     void *v;  // pointer to data
     I n;  // number of atoms of data
     C t;  // type of data, 0=char 1=int 2=fl 3=complex
     C cxlgsz;  // lg of atom size (in complex, half an atom), then target - 2 bits each, source=0-1 targ=2-3
    }cip[NCDARGS];
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
  per=DEPARM+i*256; I star=cc->starlett[i].star;
  I ctype=cc->starlett[i].type, clgsz=cc->starlett[i].lgsz, t=cc->starlett[i].jtype; I xlgsz=0;  // desired type 0123, desired size (high if no type, to suppress inplace conv), J type for non-in-place conversions (0 if type unknown)
  // c is type in the call, t is the J type for that.  star is the *& qualifier
  I litsgn;  // will be neg if lit array that can be used as pointer
  I boxatomsgn;  // neg if boxed atom used as pointer
  if(likely(wt&BOX)){  // inputs are boxes, pointed to by wv
   x=C(wv[i]); xt=AT(x); xn=AN(x); xr=AR(x);   // x is argument i
   boxatomsgn=-star&(xr-1)&SGNIF(xt,BOXX);  // neg if boxed atom used as pointer
   CDASSERT((-xr&(star-1))>=0,per);         /* non-pointers must be scalars   !xr||star */
// long way   lit=star&&xt&LIT&&(c=='b'||c=='s'&&0==(xn&1)||c=='f'&&0==(xn&3));
   // litsgn means x is a LIT area whose address can be used as a source for numeric data
   litsgn=-star & SGNIF(xt,LITX) & SGNIF(cc->starlett[i].flags,0) & ((((1LL<<clgsz)-1)&xn)-1);   // LIT array can source short numerics, if integral # atoms.  Suppress converting it then
// long way    if(t&&TYPESNE(t,xt)&&!(lit||star&&!xr&&xt&BOX)){x=cvt(xt=t,x); CDASSERT(x!=0,per);}
   // perform non-in-place conversions.  These are conversions to larger atoms, or to different formats.
#define F2T(t,n) ((UI8)n<<(2*CTTZ(t)))
   I xtype=(((F2T(LIT,0)|F2T(C2T,0)|F2T(C4T,0)|F2T(B01,1)|F2T(INT,1)|F2T(INT1,1)|F2T(INT2,1)|F2T(INT4,1)|F2T(HP,2)|F2T(SP,2)|F2T(FL,2)|F2T(QP,2)|F2T(CMPX,3)))>>(2*CTTZ(xt)))&3;  // type 0123
   xtype|=xt&XNUM+RAT;  // indirect types are in a class by themselves & must always be converted
   xlgsz=bplg(xt);  // lg(actual atom len)
   if(unlikely(((t-1)|litsgn|boxatomsgn|(((ctype^xtype)-1)&(clgsz-xlgsz-1)))>=0)){ x=cvt(xt=t,x); xlgsz=bplg(xt); CDASSERT(x!=0,per); }  // convert if result type known & (not what we have, or bigger than what we have)
   // We know that x originated in a box, so it can't be PRISTINE.  But it may have been converted, so we have to
   // make sure that what we install into *zv is not inplaceable.  *zv is never recursive.
   xv=AV(x); if(zbx)*zv=incorp(x);
  }else{   // non-boxed input.  Must be atom
   xv=convert0(t,cv0,wt,u); xt=t; u+=wk;  // this seems to install an atom on the stack, of the desired type?
   xlgsz=bplg(xt);  // lg(actual atom len)
   boxatomsgn=-star&(xr-1)&SGNIF(xt,BOXX);  // neg if boxed atom used as pointer
   CDASSERT(xv!=0,per);   // abort if conversion failed
   if(zbx){GA00(y,t,1,0); MC(AV(y),xv,bpnoun(t)); *zv=incorp(y);}  // must never install inplaceable block; move value into a box
  }
  xlgsz=xt&CMPX?3:xlgsz;  // the length for CMPX is the length of an atom

  // now xv points to the actual arg data for arg i, and an A-block for same has been installed into *zv.  Set *dv (the parm, either a pointer or a value)
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
   // remember inplace-conversions (i. e. conversions to smaller precisions, which can be handled in place).  Conversions to larger precisions, or to other types, were handled above.
   // Here we see if a conversion will be needed and add it to the list of inplace conversions
   CDASSERT(ISDENSETYPE(xt,B01+LIT+C2T+C4T+INT+INT2+INT4+FL+CMPX),per);  // verify J type is DIRECT  scaf not needed, impossible
   if(unlikely(xlgsz>clgsz)){  // x is bigger than needed (ignoring cases of LIT buffers)
    cip[cipcount].v=xv; cip[cipcount].n=xn; cip[cipcount].t=ctype; cip[cipcount].cxlgsz=4*clgsz+xlgsz;   // save conversion info
    ++cipcount;
   }
  }else{  // boxed atom
   // coded to avoid switch, which mispredicts
   if(likely(cc->starlett[i].flags&2)){  // the type is sign-extendable (short INT or INT-sized INT/FL)
    // integer.  Most singletons are integers.  d on SY_64 and no weird calling convention is handled like an 8-byte integer
    // general plan for integers is: fetch 8 bytes (overfetch OK); get length; sign-extend at that length; mask off if not sign-extended
    // code: b  c  w  u  s  i  x  d
    // ASC:  62 63 77 75 73 69 78 64
    // >>1   31 31 3b 3a 39 34 3c 32
    // &15   1  1  11 10 9  4  12  2  decimal
    // len   0  0  1  2  1  2  3/2 3
    // sign  x  0  0  0  1  1  x   x
    I iwd=*xv;  // fetch 8 bytes, possibly overfetching
    I lglen=clgsz; I sxt=ctype==1?2:0;  // lg2(len of data in bytes)  2 if sign-extend required
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
   }else if(likely((SY_MACPPC||SY_UNIX64||ARMARGS||!SY_64)&&ctype==2&&clgsz==3)){  // double-precision arg with a weird calling sequence
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
   }else if(likely(ctype==2&&clgsz==2)){   // single-precision arg
#if SY_MACPPC
          dd[dcnt++]=(float)*(D*)xv;
#endif
#if SY_64 && (SY_LINUX  || SY_MAC || SY_FREEBSD || SY_OPENBSD)
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

 DO(cipcount, convertdown(cip[i].v,cip[i].n,cip[i].t,cip[i].cxlgsz););  /* convert I to s and int and d to f as required */
 // allocate the result area and point to it
 if(zbx){GA00(x,cc->zt,1,0); xv=AV(x); *(A*)zv0=incorp(x);}else xv=(I*)zv0;  // must not box an inplaceable.  xv points to where the function will store its result: in zv or in a fresh box
 // get the address of the function
 if('1'==cc->cc){fp=(FARPROC)*((I)cc->fp+(I*)*(I*)*data); CDASSERT(fp!=0,DEBADFN);}else fp=cc->fp;

 // call it.  This is a safe recursion point.  Back up to IDLE
 jt->recurstate&=~RECSTATEBUSY;  // back to IDLE/PROMPT state
 docall(fp, data, dv-data, dd, dcnt, cc->zl, xv, cc->alternate);  // call the function, set the result
 jt->recurstate|=RECSTATEBUSY;  // cd complete, go back to normal running state, BUSY normally or RECUR if a prompt is pending

 DO(cipcount, convertup(cip[i].v,cip[i].n,cip[i].t,cip[i].cxlgsz);); /* convert s and int to I and f to d as required */
#if SY_WIN32
 I en= GetLastError();
 if(cc->fpreset)_fpreset();         /* delphi dll (and others) damage fp state */
#endif
#if SYS&SYS_UNIX
 I en=errno;
#endif
 if(unlikely(en!=0))jt->getlasterror=en;
 R 1;
}

F2(jtcd){A z;C *wv,*zv;CCT*cc;I k,m,n,p,q,t,wr,*ws,wt;
 F2PREFIP;
 ARGCHK2(a,w); ASSERT(!JT(jt,seclev),EVSECURE)
 AFLAGPRISTNO(w)  // we transfer boxes from w to the result, thereby letting them escape.  That makes w non-pristine
 if(1<AR(a)){I rr=AR(w); rr=rr==0?1:rr; R rank2ex(a,w,DUMMYSELF,1L,rr,1L,rr,jtcd);}
 wt=AT(w); wr=AR(w); ws=AS(w); PRODX(m,wr-1,ws,1);   // m = # 1-cells of w
 ASSERT(!ISSPARSE(wt),EVDOMAIN);
 ASSERT(LIT&AT(a),EVDOMAIN);
 C* enda=&CAV(a)[AN(a)]; C endc=*enda; *enda=0; cc=jtcdparse(jt,a); *enda=endc; RZ(cc); // should do outside rank2 loop?
 n=cc->n;  // n=# arguments
 I nn; CDASSERT(n==SHAPEN(w,wr-1,nn),DECOUNT);
 if(cc->zbx){GATV(z,BOX,m*(1+n),MAX(1,wr),ws); AS(z)[AR(z)-1]=1+n;}  // allocate n boxes for each result arg, plus 1 for the result
 else{CDASSERT('*'!=cc->zl,DEDEC); GA(z,cc->zt,m,MAX(0,wr-1),ws);}  // if fast form, just allocate the return value
 // z is always nonrecursive
 if(unlikely((-m&-n&SGNIFNOT(wt,BOXX))<0)){   // if w is NOT boxed, and a and w arenot both empty
  t=0; DQ(n, k=cc->starlett[i].jtype; t=MAX(t,k););
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
#if defined(__wasm__) || defined(TARGET_IOS)
#define FREELIB(x) 0
#elif (SYS & SYS_UNIX)
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

F1(jtcdf){ASSERT(!JT(jt,seclev),EVSECURE) ASSERTMTV(w); dllquit(jt); R mtm;}
     /* 15!:5 */

/* return error info from last cd domain error - resets to DEOK */
F1(jtcder){I4 t; ASSERT(!JT(jt,seclev),EVSECURE) ASSERTMTV(w); t=jt->dlllasterror; jt->dlllasterror=DEOK; R v2(t&0xff,t>>8);}
     /* 15!:10 */

/* return errno info from last cd with errno not equal to 0 - resets to 0 */
F1(jtcderx){I4 t;C buf[1024];
 ASSERT(!JT(jt,seclev),EVSECURE) ASSERTMTV(w); t=jt->getlasterror; jt->getlasterror=0;

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

#if defined(__wasm__) || defined(TARGET_IOS)
 {strcpy (buf, "");}
#elif SYS&SYS_UNIX
 {const char *e = dlerror(); strcpy (buf, e?e:"");}
#endif
 R jlink(sc(t),cstr(buf));
}    /* 15!:11  GetLastError information */

F1(jtmema){I k; ASSERT(!JT(jt,seclev),EVSECURE) RE(k=i0(w)); R sc((I)MALLOC(k));} /* ce */
     /* 15!:3  memory allocate */

F1(jtmemf){I k; ASSERT(!JT(jt,seclev),EVSECURE) RE(k=i0(w)); FREE((void*)k); R num(0);}
     /* 15!:4  memory free */

F1(jtmemr){C*u;I m,n,t,*v;US*us;C4*c4;
 ARGCHK1(w); ASSERT(!JT(jt,seclev),EVSECURE)
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
 ARGCHK2(a,w); ASSERT(!JT(jt,seclev),EVSECURE)
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


// 15!:18 memalign: return block with data portion aligned to cache.  Bivalent.  If x given, it is shape to use.  Fill result with copies of y
DF2(jtmemalign){I *s, n, r;  // shape and rank
 if(AT(w)&VERB){  // monad
  if(unlikely(ISSPARSE(AT(w))))R RETARG(a);  // we cannot align sparse blocks
  if((((I)a+AK(a))&(CACHELINESIZE-1))==0)R RETARG(a);  // if the current value is already aligned, keep it unchanged
  w=a; s=AS(w); r=AR(w); n=AN(w);  // monad: take rank/shape from w
 }else{  // dyad
  if(unlikely(ISSPARSE(AT(w))))R RETARG(w);  // we cannot align sparse blocks
  RZ(a=vip(a)) s=IAV(a); r=AN(a); PRODX(n,r,s,1)  // take rank/shape from a
 }
 // look at the rank of w to see what rank we should request to get aligned
 I neededrank=(-(CACHELINESIZE/SZI)&((NORMAH+r)+(CACHELINESIZE/SZI-1)))-NORMAH;  // rank to allocate to get on cacheline boundary
 I t=AT(w);
 A z; GA00(z,t,n,neededrank) AR(z)=r; MCISH(AS(z),s,r)  // allocate result with data on boundary; fill in correct rank
 I k=bplg(t);
 mvc(n<<k,voidAV(z),AN(w)<<k,voidAV(w));  // copy the data block in full
 R z;
}

// 15!:15 memu - make a copy of y if it is not writable (inplaceable and not read-only)
// We have to check jt in case this usage is in a fork that will use the block later
F1(jtmemu) { F1PREFIP; ARGCHK1(w); ASSERT(!JT(jt,seclev),EVSECURE) if(!((I)jtinplace&JTINPLACEW && (AC(w)<(AFLAG(w)<<((BW-1)-AFROX)))))w=ca(w);
 // We will NUL-terminate a string in case called routines need it (not sure this is necessary)
#if SY_64
 if(unlikely((AT(w)&LAST0)!=0))*(C4*)&CAV(w)[AN(w)*bpnoun(AT(w))]=0;  // on 64-bit systems, always safe to add 4 bytes at end of data
#else
 if(unlikely((AT(w)&LAST0)!=0)){I nulofst=AN(w)*bpnoun(AT(w)); *(UI4*)&CAV(w)[nulofst&~3]&=~((UI4)-1<<((nulofst&3)<<LGBB));}  // zero out the 4-byte word starting at NUL.  addr 00->mask w/0, 01->ff 02->ffff 03->ffffff
#endif
 RETF(w);
}
F2(jtmemu2) { ASSERT(!JT(jt,seclev),EVSECURE) RETF(ca(w)); }  // dyad - force copy willy-nilly

F1(jtgh15){A z;I k; ASSERT(!JT(jt,seclev),EVSECURE) RE(k=i0(w)); RZ(z=gah(k,0L)); ACINIT(z,ACUC2); R sc((I)z);}   // ra the header
     /* 15!:8  get header */

F1(jtfh15){I k; ASSERT(!JT(jt,seclev),EVSECURE) RE(k=i0(w)); fr((A)k); R num(0);}
     /* 15!:9  free header */

// 15!:7.  w has the address of a header.  Put that address in play as an A block
F1(jtdllsymset){ARGCHK1(w); ASSERT(!JT(jt,seclev),EVSECURE) R (A)i0(w);}      /* do some validation here */

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
 ZEROUPPER;
 if(!r||AR(r)) R 0;
 if(INT&AT(r)) R AV(r)[0];
 if(B01&AT(r)) R ((BYTE*)AV(r))[0];
 R 0;
}

static I cbnew(){A r;
 J jt=cbjt;
 r=exec1(cstr("cdcallback''"));
 ZEROUPPER; // see comment in io.c
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
// static I cbx[CBTYPESMAX-1];
static I cbx[10*CBTYPESMAX-1];
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

static I CALLBACK cbx00(){cbxn=0;R cbnew();}
static I CALLBACK cbx01(I a){cbxn=1;cbx[0]=a;R cbnew();}
static I CALLBACK cbx02(I a,I b){cbxn=2;cbx[0]=a;cbx[1]=b;R cbnew();}
static I CALLBACK cbx03(I a,I b,I c){cbxn=3;cbx[0]=a;cbx[1]=b;cbx[2]=c;R cbnew();}
static I CALLBACK cbx04(I a,I b,I c,I d){cbxn=4;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;R cbnew();}
static I CALLBACK cbx05(I a,I b,I c,I d,I e){cbxn=5;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;R cbnew();}
static I CALLBACK cbx06(I a,I b,I c,I d,I e,I f){cbxn=6;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;cbx[5]=f;R cbnew();}
static I CALLBACK cbx07(I a,I b,I c,I d,I e,I f,I g){cbxn=7;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;cbx[5]=f;cbx[6]=g;R cbnew();}
static I CALLBACK cbx08(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=8;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;cbx[5]=f;cbx[6]=g;cbx[7]=h;R cbnew();}
static I CALLBACK cbx09(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=9;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;cbx[5]=f;cbx[6]=g;cbx[7]=h;cbx[8]=i;R cbnew();}

static I CALLBACK cbx10(){cbxn=10;R cbnew();}
static I CALLBACK cbx11(I a){cbxn=11;cbx[10]=a;R cbnew();}
static I CALLBACK cbx12(I a,I b){cbxn=12;cbx[10]=a;cbx[11]=b;R cbnew();}
static I CALLBACK cbx13(I a,I b,I c){cbxn=13;cbx[10]=a;cbx[11]=b;cbx[12]=c;R cbnew();}
static I CALLBACK cbx14(I a,I b,I c,I d){cbxn=14;cbx[10]=a;cbx[11]=b;cbx[12]=c;cbx[13]=d;R cbnew();}
static I CALLBACK cbx15(I a,I b,I c,I d,I e){cbxn=15;cbx[10]=a;cbx[11]=b;cbx[12]=c;cbx[13]=d;cbx[14]=e;R cbnew();}
static I CALLBACK cbx16(I a,I b,I c,I d,I e,I f){cbxn=16;cbx[10]=a;cbx[11]=b;cbx[12]=c;cbx[13]=d;cbx[14]=e;cbx[15]=f;R cbnew();}
static I CALLBACK cbx17(I a,I b,I c,I d,I e,I f,I g){cbxn=17;cbx[10]=a;cbx[11]=b;cbx[12]=c;cbx[13]=d;cbx[14]=e;cbx[15]=f;cbx[16]=g;R cbnew();}
static I CALLBACK cbx18(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=18;cbx[10]=a;cbx[11]=b;cbx[12]=c;cbx[13]=d;cbx[14]=e;cbx[15]=f;cbx[16]=g;cbx[17]=h;R cbnew();}
static I CALLBACK cbx19(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=19;cbx[10]=a;cbx[11]=b;cbx[12]=c;cbx[13]=d;cbx[14]=e;cbx[15]=f;cbx[16]=g;cbx[17]=h;cbx[18]=i;R cbnew();}

static I CALLBACK cbx20(){cbxn=20;R cbnew();}
static I CALLBACK cbx21(I a){cbxn=21;cbx[20]=a;R cbnew();}
static I CALLBACK cbx22(I a,I b){cbxn=22;cbx[20]=a;cbx[21]=b;R cbnew();}
static I CALLBACK cbx23(I a,I b,I c){cbxn=23;cbx[20]=a;cbx[21]=b;cbx[22]=c;R cbnew();}
static I CALLBACK cbx24(I a,I b,I c,I d){cbxn=24;cbx[20]=a;cbx[21]=b;cbx[22]=c;cbx[23]=d;R cbnew();}
static I CALLBACK cbx25(I a,I b,I c,I d,I e){cbxn=25;cbx[20]=a;cbx[21]=b;cbx[22]=c;cbx[23]=d;cbx[24]=e;R cbnew();}
static I CALLBACK cbx26(I a,I b,I c,I d,I e,I f){cbxn=26;cbx[20]=a;cbx[21]=b;cbx[22]=c;cbx[23]=d;cbx[24]=e;cbx[25]=f;R cbnew();}
static I CALLBACK cbx27(I a,I b,I c,I d,I e,I f,I g){cbxn=27;cbx[20]=a;cbx[21]=b;cbx[22]=c;cbx[23]=d;cbx[24]=e;cbx[25]=f;cbx[26]=g;R cbnew();}
static I CALLBACK cbx28(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=28;cbx[20]=a;cbx[21]=b;cbx[22]=c;cbx[23]=d;cbx[24]=e;cbx[25]=f;cbx[26]=g;cbx[27]=h;R cbnew();}
static I CALLBACK cbx29(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=29;cbx[20]=a;cbx[21]=b;cbx[22]=c;cbx[23]=d;cbx[24]=e;cbx[25]=f;cbx[26]=g;cbx[27]=h;cbx[28]=i;R cbnew();}

static I CALLBACK cbx30(){cbxn=30;R cbnew();}
static I CALLBACK cbx31(I a){cbxn=31;cbx[30]=a;R cbnew();}
static I CALLBACK cbx32(I a,I b){cbxn=32;cbx[30]=a;cbx[31]=b;R cbnew();}
static I CALLBACK cbx33(I a,I b,I c){cbxn=33;cbx[30]=a;cbx[31]=b;cbx[32]=c;R cbnew();}
static I CALLBACK cbx34(I a,I b,I c,I d){cbxn=34;cbx[30]=a;cbx[31]=b;cbx[32]=c;cbx[33]=d;R cbnew();}
static I CALLBACK cbx35(I a,I b,I c,I d,I e){cbxn=35;cbx[30]=a;cbx[31]=b;cbx[32]=c;cbx[33]=d;cbx[34]=e;R cbnew();}
static I CALLBACK cbx36(I a,I b,I c,I d,I e,I f){cbxn=36;cbx[30]=a;cbx[31]=b;cbx[32]=c;cbx[33]=d;cbx[34]=e;cbx[35]=f;R cbnew();}
static I CALLBACK cbx37(I a,I b,I c,I d,I e,I f,I g){cbxn=37;cbx[30]=a;cbx[31]=b;cbx[32]=c;cbx[33]=d;cbx[34]=e;cbx[35]=f;cbx[36]=g;R cbnew();}
static I CALLBACK cbx38(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=38;cbx[30]=a;cbx[31]=b;cbx[32]=c;cbx[33]=d;cbx[34]=e;cbx[35]=f;cbx[36]=g;cbx[37]=h;R cbnew();}
static I CALLBACK cbx39(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=39;cbx[30]=a;cbx[31]=b;cbx[32]=c;cbx[33]=d;cbx[34]=e;cbx[35]=f;cbx[36]=g;cbx[37]=h;cbx[38]=i;R cbnew();}

static I CALLBACK cbx40(){cbxn=40;R cbnew();}
static I CALLBACK cbx41(I a){cbxn=41;cbx[40]=a;R cbnew();}
static I CALLBACK cbx42(I a,I b){cbxn=42;cbx[40]=a;cbx[41]=b;R cbnew();}
static I CALLBACK cbx43(I a,I b,I c){cbxn=43;cbx[40]=a;cbx[41]=b;cbx[42]=c;R cbnew();}
static I CALLBACK cbx44(I a,I b,I c,I d){cbxn=44;cbx[40]=a;cbx[41]=b;cbx[42]=c;cbx[43]=d;R cbnew();}
static I CALLBACK cbx45(I a,I b,I c,I d,I e){cbxn=45;cbx[40]=a;cbx[41]=b;cbx[42]=c;cbx[43]=d;cbx[44]=e;R cbnew();}
static I CALLBACK cbx46(I a,I b,I c,I d,I e,I f){cbxn=46;cbx[40]=a;cbx[41]=b;cbx[42]=c;cbx[43]=d;cbx[44]=e;cbx[45]=f;R cbnew();}
static I CALLBACK cbx47(I a,I b,I c,I d,I e,I f,I g){cbxn=47;cbx[40]=a;cbx[41]=b;cbx[42]=c;cbx[43]=d;cbx[44]=e;cbx[45]=f;cbx[46]=g;R cbnew();}
static I CALLBACK cbx48(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=48;cbx[40]=a;cbx[41]=b;cbx[42]=c;cbx[43]=d;cbx[44]=e;cbx[45]=f;cbx[46]=g;cbx[47]=h;R cbnew();}
static I CALLBACK cbx49(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=49;cbx[40]=a;cbx[41]=b;cbx[42]=c;cbx[43]=d;cbx[44]=e;cbx[45]=f;cbx[46]=g;cbx[47]=h;cbx[48]=i;R cbnew();}

static I CALLBACK cbx50(){cbxn=50;R cbnew();}
static I CALLBACK cbx51(I a){cbxn=51;cbx[50]=a;R cbnew();}
static I CALLBACK cbx52(I a,I b){cbxn=52;cbx[50]=a;cbx[51]=b;R cbnew();}
static I CALLBACK cbx53(I a,I b,I c){cbxn=53;cbx[50]=a;cbx[51]=b;cbx[52]=c;R cbnew();}
static I CALLBACK cbx54(I a,I b,I c,I d){cbxn=54;cbx[50]=a;cbx[51]=b;cbx[52]=c;cbx[53]=d;R cbnew();}
static I CALLBACK cbx55(I a,I b,I c,I d,I e){cbxn=55;cbx[50]=a;cbx[51]=b;cbx[52]=c;cbx[53]=d;cbx[54]=e;R cbnew();}
static I CALLBACK cbx56(I a,I b,I c,I d,I e,I f){cbxn=56;cbx[50]=a;cbx[51]=b;cbx[52]=c;cbx[53]=d;cbx[54]=e;cbx[55]=f;R cbnew();}
static I CALLBACK cbx57(I a,I b,I c,I d,I e,I f,I g){cbxn=57;cbx[50]=a;cbx[51]=b;cbx[52]=c;cbx[53]=d;cbx[54]=e;cbx[55]=f;cbx[56]=g;R cbnew();}
static I CALLBACK cbx58(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=58;cbx[50]=a;cbx[51]=b;cbx[52]=c;cbx[53]=d;cbx[54]=e;cbx[55]=f;cbx[56]=g;cbx[57]=h;R cbnew();}
static I CALLBACK cbx59(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=59;cbx[50]=a;cbx[51]=b;cbx[52]=c;cbx[53]=d;cbx[54]=e;cbx[55]=f;cbx[56]=g;cbx[57]=h;cbx[58]=i;R cbnew();}

static I CALLBACK cbx60(){cbxn=60;R cbnew();}
static I CALLBACK cbx61(I a){cbxn=61;cbx[60]=a;R cbnew();}
static I CALLBACK cbx62(I a,I b){cbxn=62;cbx[60]=a;cbx[61]=b;R cbnew();}
static I CALLBACK cbx63(I a,I b,I c){cbxn=63;cbx[60]=a;cbx[61]=b;cbx[62]=c;R cbnew();}
static I CALLBACK cbx64(I a,I b,I c,I d){cbxn=64;cbx[60]=a;cbx[61]=b;cbx[62]=c;cbx[63]=d;R cbnew();}
static I CALLBACK cbx65(I a,I b,I c,I d,I e){cbxn=65;cbx[60]=a;cbx[61]=b;cbx[62]=c;cbx[63]=d;cbx[64]=e;R cbnew();}
static I CALLBACK cbx66(I a,I b,I c,I d,I e,I f){cbxn=66;cbx[60]=a;cbx[61]=b;cbx[62]=c;cbx[63]=d;cbx[64]=e;cbx[65]=f;R cbnew();}
static I CALLBACK cbx67(I a,I b,I c,I d,I e,I f,I g){cbxn=67;cbx[60]=a;cbx[61]=b;cbx[62]=c;cbx[63]=d;cbx[64]=e;cbx[65]=f;cbx[66]=g;R cbnew();}
static I CALLBACK cbx68(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=68;cbx[60]=a;cbx[61]=b;cbx[62]=c;cbx[63]=d;cbx[64]=e;cbx[65]=f;cbx[66]=g;cbx[67]=h;R cbnew();}
static I CALLBACK cbx69(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=69;cbx[60]=a;cbx[61]=b;cbx[62]=c;cbx[63]=d;cbx[64]=e;cbx[65]=f;cbx[66]=g;cbx[67]=h;cbx[68]=i;R cbnew();}

static I CALLBACK cbx70(){cbxn=70;R cbnew();}
static I CALLBACK cbx71(I a){cbxn=71;cbx[70]=a;R cbnew();}
static I CALLBACK cbx72(I a,I b){cbxn=72;cbx[70]=a;cbx[71]=b;R cbnew();}
static I CALLBACK cbx73(I a,I b,I c){cbxn=73;cbx[70]=a;cbx[71]=b;cbx[72]=c;R cbnew();}
static I CALLBACK cbx74(I a,I b,I c,I d){cbxn=74;cbx[70]=a;cbx[71]=b;cbx[72]=c;cbx[73]=d;R cbnew();}
static I CALLBACK cbx75(I a,I b,I c,I d,I e){cbxn=75;cbx[70]=a;cbx[71]=b;cbx[72]=c;cbx[73]=d;cbx[74]=e;R cbnew();}
static I CALLBACK cbx76(I a,I b,I c,I d,I e,I f){cbxn=76;cbx[70]=a;cbx[71]=b;cbx[72]=c;cbx[73]=d;cbx[74]=e;cbx[75]=f;R cbnew();}
static I CALLBACK cbx77(I a,I b,I c,I d,I e,I f,I g){cbxn=77;cbx[70]=a;cbx[71]=b;cbx[72]=c;cbx[73]=d;cbx[74]=e;cbx[75]=f;cbx[76]=g;R cbnew();}
static I CALLBACK cbx78(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=78;cbx[70]=a;cbx[71]=b;cbx[72]=c;cbx[73]=d;cbx[74]=e;cbx[75]=f;cbx[76]=g;cbx[77]=h;R cbnew();}
static I CALLBACK cbx79(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=79;cbx[70]=a;cbx[71]=b;cbx[72]=c;cbx[73]=d;cbx[74]=e;cbx[75]=f;cbx[76]=g;cbx[77]=h;cbx[78]=i;R cbnew();}

static I CALLBACK cbx80(){cbxn=80;R cbnew();}
static I CALLBACK cbx81(I a){cbxn=81;cbx[80]=a;R cbnew();}
static I CALLBACK cbx82(I a,I b){cbxn=82;cbx[80]=a;cbx[81]=b;R cbnew();}
static I CALLBACK cbx83(I a,I b,I c){cbxn=83;cbx[80]=a;cbx[81]=b;cbx[82]=c;R cbnew();}
static I CALLBACK cbx84(I a,I b,I c,I d){cbxn=84;cbx[80]=a;cbx[81]=b;cbx[82]=c;cbx[83]=d;R cbnew();}
static I CALLBACK cbx85(I a,I b,I c,I d,I e){cbxn=85;cbx[80]=a;cbx[81]=b;cbx[82]=c;cbx[83]=d;cbx[84]=e;R cbnew();}
static I CALLBACK cbx86(I a,I b,I c,I d,I e,I f){cbxn=86;cbx[80]=a;cbx[81]=b;cbx[82]=c;cbx[83]=d;cbx[84]=e;cbx[85]=f;R cbnew();}
static I CALLBACK cbx87(I a,I b,I c,I d,I e,I f,I g){cbxn=87;cbx[80]=a;cbx[81]=b;cbx[82]=c;cbx[83]=d;cbx[84]=e;cbx[85]=f;cbx[86]=g;R cbnew();}
static I CALLBACK cbx88(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=88;cbx[80]=a;cbx[81]=b;cbx[82]=c;cbx[83]=d;cbx[84]=e;cbx[85]=f;cbx[86]=g;cbx[87]=h;R cbnew();}
static I CALLBACK cbx89(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=89;cbx[80]=a;cbx[81]=b;cbx[82]=c;cbx[83]=d;cbx[84]=e;cbx[85]=f;cbx[86]=g;cbx[87]=h;cbx[88]=i;R cbnew();}

static I CALLBACK cbx90(){cbxn=90;R cbnew();}
static I CALLBACK cbx91(I a){cbxn=91;cbx[90]=a;R cbnew();}
static I CALLBACK cbx92(I a,I b){cbxn=92;cbx[90]=a;cbx[91]=b;R cbnew();}
static I CALLBACK cbx93(I a,I b,I c){cbxn=93;cbx[90]=a;cbx[91]=b;cbx[92]=c;R cbnew();}
static I CALLBACK cbx94(I a,I b,I c,I d){cbxn=94;cbx[90]=a;cbx[91]=b;cbx[92]=c;cbx[93]=d;R cbnew();}
static I CALLBACK cbx95(I a,I b,I c,I d,I e){cbxn=95;cbx[90]=a;cbx[91]=b;cbx[92]=c;cbx[93]=d;cbx[94]=e;R cbnew();}
static I CALLBACK cbx96(I a,I b,I c,I d,I e,I f){cbxn=96;cbx[90]=a;cbx[91]=b;cbx[92]=c;cbx[93]=d;cbx[94]=e;cbx[95]=f;R cbnew();}
static I CALLBACK cbx97(I a,I b,I c,I d,I e,I f,I g){cbxn=97;cbx[90]=a;cbx[91]=b;cbx[92]=c;cbx[93]=d;cbx[94]=e;cbx[95]=f;cbx[96]=g;R cbnew();}
static I CALLBACK cbx98(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=98;cbx[90]=a;cbx[91]=b;cbx[92]=c;cbx[93]=d;cbx[94]=e;cbx[95]=f;cbx[96]=g;cbx[97]=h;R cbnew();}
static I CALLBACK cbx99(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=99;cbx[90]=a;cbx[91]=b;cbx[92]=c;cbx[93]=d;cbx[94]=e;cbx[95]=f;cbx[96]=g;cbx[97]=h;cbx[98]=i;R cbnew();}

static I cbvx[]={(I)&cbx00,(I)&cbx01,(I)&cbx02,(I)&cbx03,(I)&cbx04,(I)&cbx05,(I)&cbx06,(I)&cbx07,(I)&cbx08,(I)&cbx09, \
                 (I)&cbx10,(I)&cbx11,(I)&cbx12,(I)&cbx13,(I)&cbx14,(I)&cbx15,(I)&cbx16,(I)&cbx17,(I)&cbx18,(I)&cbx19, \
                 (I)&cbx20,(I)&cbx21,(I)&cbx22,(I)&cbx23,(I)&cbx24,(I)&cbx25,(I)&cbx26,(I)&cbx27,(I)&cbx28,(I)&cbx29, \
                 (I)&cbx30,(I)&cbx31,(I)&cbx32,(I)&cbx33,(I)&cbx34,(I)&cbx35,(I)&cbx36,(I)&cbx37,(I)&cbx38,(I)&cbx39, \
                 (I)&cbx40,(I)&cbx41,(I)&cbx42,(I)&cbx43,(I)&cbx44,(I)&cbx45,(I)&cbx46,(I)&cbx47,(I)&cbx48,(I)&cbx49, \
                 (I)&cbx50,(I)&cbx51,(I)&cbx52,(I)&cbx53,(I)&cbx54,(I)&cbx55,(I)&cbx56,(I)&cbx57,(I)&cbx58,(I)&cbx59, \
                 (I)&cbx60,(I)&cbx61,(I)&cbx62,(I)&cbx63,(I)&cbx64,(I)&cbx65,(I)&cbx66,(I)&cbx67,(I)&cbx68,(I)&cbx69, \
                 (I)&cbx70,(I)&cbx71,(I)&cbx72,(I)&cbx73,(I)&cbx74,(I)&cbx75,(I)&cbx76,(I)&cbx77,(I)&cbx78,(I)&cbx79, \
                 (I)&cbx80,(I)&cbx81,(I)&cbx82,(I)&cbx83,(I)&cbx84,(I)&cbx85,(I)&cbx86,(I)&cbx87,(I)&cbx88,(I)&cbx89, \
                 (I)&cbx90,(I)&cbx91,(I)&cbx92,(I)&cbx93,(I)&cbx94,(I)&cbx95,(I)&cbx96,(I)&cbx97,(I)&cbx98,(I)&cbx99  };

#if SY_WIN32
static I _cdecl cbxalt00(){cbxn=0;R cbnew();}
static I _cdecl cbxalt01(I a){cbxn=1;cbx[0]=a;R cbnew();}
static I _cdecl cbxalt02(I a,I b){cbxn=2;cbx[0]=a;cbx[1]=b;R cbnew();}
static I _cdecl cbxalt03(I a,I b,I c){cbxn=3;cbx[0]=a;cbx[1]=b;cbx[2]=c;R cbnew();}
static I _cdecl cbxalt04(I a,I b,I c,I d){cbxn=4;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;R cbnew();}
static I _cdecl cbxalt05(I a,I b,I c,I d,I e){cbxn=5;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;R cbnew();}
static I _cdecl cbxalt06(I a,I b,I c,I d,I e,I f){cbxn=6;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;cbx[5]=f;R cbnew();}
static I _cdecl cbxalt07(I a,I b,I c,I d,I e,I f,I g){cbxn=7;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;cbx[5]=f;cbx[6]=g;R cbnew();}
static I _cdecl cbxalt08(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=8;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;cbx[5]=f;cbx[6]=g;cbx[7]=h;R cbnew();}
static I _cdecl cbxalt09(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=9;cbx[0]=a;cbx[1]=b;cbx[2]=c;cbx[3]=d;cbx[4]=e;cbx[5]=f;cbx[6]=g;cbx[7]=h;cbx[8]=i;R cbnew();}

static I _cdecl cbxalt10(){cbxn=10;R cbnew();}
static I _cdecl cbxalt11(I a){cbxn=11;cbx[10]=a;R cbnew();}
static I _cdecl cbxalt12(I a,I b){cbxn=12;cbx[10]=a;cbx[11]=b;R cbnew();}
static I _cdecl cbxalt13(I a,I b,I c){cbxn=13;cbx[10]=a;cbx[11]=b;cbx[12]=c;R cbnew();}
static I _cdecl cbxalt14(I a,I b,I c,I d){cbxn=14;cbx[10]=a;cbx[11]=b;cbx[12]=c;cbx[13]=d;R cbnew();}
static I _cdecl cbxalt15(I a,I b,I c,I d,I e){cbxn=15;cbx[10]=a;cbx[11]=b;cbx[12]=c;cbx[13]=d;cbx[14]=e;R cbnew();}
static I _cdecl cbxalt16(I a,I b,I c,I d,I e,I f){cbxn=16;cbx[10]=a;cbx[11]=b;cbx[12]=c;cbx[13]=d;cbx[14]=e;cbx[15]=f;R cbnew();}
static I _cdecl cbxalt17(I a,I b,I c,I d,I e,I f,I g){cbxn=17;cbx[10]=a;cbx[11]=b;cbx[12]=c;cbx[13]=d;cbx[14]=e;cbx[15]=f;cbx[16]=g;R cbnew();}
static I _cdecl cbxalt18(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=18;cbx[10]=a;cbx[11]=b;cbx[12]=c;cbx[13]=d;cbx[14]=e;cbx[15]=f;cbx[16]=g;cbx[17]=h;R cbnew();}
static I _cdecl cbxalt19(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=19;cbx[10]=a;cbx[11]=b;cbx[12]=c;cbx[13]=d;cbx[14]=e;cbx[15]=f;cbx[16]=g;cbx[17]=h;cbx[18]=i;R cbnew();}

static I _cdecl cbxalt20(){cbxn=20;R cbnew();}
static I _cdecl cbxalt21(I a){cbxn=21;cbx[20]=a;R cbnew();}
static I _cdecl cbxalt22(I a,I b){cbxn=22;cbx[20]=a;cbx[21]=b;R cbnew();}
static I _cdecl cbxalt23(I a,I b,I c){cbxn=23;cbx[20]=a;cbx[21]=b;cbx[22]=c;R cbnew();}
static I _cdecl cbxalt24(I a,I b,I c,I d){cbxn=24;cbx[20]=a;cbx[21]=b;cbx[22]=c;cbx[23]=d;R cbnew();}
static I _cdecl cbxalt25(I a,I b,I c,I d,I e){cbxn=25;cbx[20]=a;cbx[21]=b;cbx[22]=c;cbx[23]=d;cbx[24]=e;R cbnew();}
static I _cdecl cbxalt26(I a,I b,I c,I d,I e,I f){cbxn=26;cbx[20]=a;cbx[21]=b;cbx[22]=c;cbx[23]=d;cbx[24]=e;cbx[25]=f;R cbnew();}
static I _cdecl cbxalt27(I a,I b,I c,I d,I e,I f,I g){cbxn=27;cbx[20]=a;cbx[21]=b;cbx[22]=c;cbx[23]=d;cbx[24]=e;cbx[25]=f;cbx[26]=g;R cbnew();}
static I _cdecl cbxalt28(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=28;cbx[20]=a;cbx[21]=b;cbx[22]=c;cbx[23]=d;cbx[24]=e;cbx[25]=f;cbx[26]=g;cbx[27]=h;R cbnew();}
static I _cdecl cbxalt29(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=29;cbx[20]=a;cbx[21]=b;cbx[22]=c;cbx[23]=d;cbx[24]=e;cbx[25]=f;cbx[26]=g;cbx[27]=h;cbx[28]=i;R cbnew();}

static I _cdecl cbxalt30(){cbxn=30;R cbnew();}
static I _cdecl cbxalt31(I a){cbxn=31;cbx[30]=a;R cbnew();}
static I _cdecl cbxalt32(I a,I b){cbxn=32;cbx[30]=a;cbx[31]=b;R cbnew();}
static I _cdecl cbxalt33(I a,I b,I c){cbxn=33;cbx[30]=a;cbx[31]=b;cbx[32]=c;R cbnew();}
static I _cdecl cbxalt34(I a,I b,I c,I d){cbxn=34;cbx[30]=a;cbx[31]=b;cbx[32]=c;cbx[33]=d;R cbnew();}
static I _cdecl cbxalt35(I a,I b,I c,I d,I e){cbxn=35;cbx[30]=a;cbx[31]=b;cbx[32]=c;cbx[33]=d;cbx[34]=e;R cbnew();}
static I _cdecl cbxalt36(I a,I b,I c,I d,I e,I f){cbxn=36;cbx[30]=a;cbx[31]=b;cbx[32]=c;cbx[33]=d;cbx[34]=e;cbx[35]=f;R cbnew();}
static I _cdecl cbxalt37(I a,I b,I c,I d,I e,I f,I g){cbxn=37;cbx[30]=a;cbx[31]=b;cbx[32]=c;cbx[33]=d;cbx[34]=e;cbx[35]=f;cbx[36]=g;R cbnew();}
static I _cdecl cbxalt38(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=38;cbx[30]=a;cbx[31]=b;cbx[32]=c;cbx[33]=d;cbx[34]=e;cbx[35]=f;cbx[36]=g;cbx[37]=h;R cbnew();}
static I _cdecl cbxalt39(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=39;cbx[30]=a;cbx[31]=b;cbx[32]=c;cbx[33]=d;cbx[34]=e;cbx[35]=f;cbx[36]=g;cbx[37]=h;cbx[38]=i;R cbnew();}

static I _cdecl cbxalt40(){cbxn=40;R cbnew();}
static I _cdecl cbxalt41(I a){cbxn=41;cbx[40]=a;R cbnew();}
static I _cdecl cbxalt42(I a,I b){cbxn=42;cbx[40]=a;cbx[41]=b;R cbnew();}
static I _cdecl cbxalt43(I a,I b,I c){cbxn=43;cbx[40]=a;cbx[41]=b;cbx[42]=c;R cbnew();}
static I _cdecl cbxalt44(I a,I b,I c,I d){cbxn=44;cbx[40]=a;cbx[41]=b;cbx[42]=c;cbx[43]=d;R cbnew();}
static I _cdecl cbxalt45(I a,I b,I c,I d,I e){cbxn=45;cbx[40]=a;cbx[41]=b;cbx[42]=c;cbx[43]=d;cbx[44]=e;R cbnew();}
static I _cdecl cbxalt46(I a,I b,I c,I d,I e,I f){cbxn=46;cbx[40]=a;cbx[41]=b;cbx[42]=c;cbx[43]=d;cbx[44]=e;cbx[45]=f;R cbnew();}
static I _cdecl cbxalt47(I a,I b,I c,I d,I e,I f,I g){cbxn=47;cbx[40]=a;cbx[41]=b;cbx[42]=c;cbx[43]=d;cbx[44]=e;cbx[45]=f;cbx[46]=g;R cbnew();}
static I _cdecl cbxalt48(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=48;cbx[40]=a;cbx[41]=b;cbx[42]=c;cbx[43]=d;cbx[44]=e;cbx[45]=f;cbx[46]=g;cbx[47]=h;R cbnew();}
static I _cdecl cbxalt49(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=49;cbx[40]=a;cbx[41]=b;cbx[42]=c;cbx[43]=d;cbx[44]=e;cbx[45]=f;cbx[46]=g;cbx[47]=h;cbx[48]=i;R cbnew();}

static I _cdecl cbxalt50(){cbxn=50;R cbnew();}
static I _cdecl cbxalt51(I a){cbxn=51;cbx[50]=a;R cbnew();}
static I _cdecl cbxalt52(I a,I b){cbxn=52;cbx[50]=a;cbx[51]=b;R cbnew();}
static I _cdecl cbxalt53(I a,I b,I c){cbxn=53;cbx[50]=a;cbx[51]=b;cbx[52]=c;R cbnew();}
static I _cdecl cbxalt54(I a,I b,I c,I d){cbxn=54;cbx[50]=a;cbx[51]=b;cbx[52]=c;cbx[53]=d;R cbnew();}
static I _cdecl cbxalt55(I a,I b,I c,I d,I e){cbxn=55;cbx[50]=a;cbx[51]=b;cbx[52]=c;cbx[53]=d;cbx[54]=e;R cbnew();}
static I _cdecl cbxalt56(I a,I b,I c,I d,I e,I f){cbxn=56;cbx[50]=a;cbx[51]=b;cbx[52]=c;cbx[53]=d;cbx[54]=e;cbx[55]=f;R cbnew();}
static I _cdecl cbxalt57(I a,I b,I c,I d,I e,I f,I g){cbxn=57;cbx[50]=a;cbx[51]=b;cbx[52]=c;cbx[53]=d;cbx[54]=e;cbx[55]=f;cbx[56]=g;R cbnew();}
static I _cdecl cbxalt58(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=58;cbx[50]=a;cbx[51]=b;cbx[52]=c;cbx[53]=d;cbx[54]=e;cbx[55]=f;cbx[56]=g;cbx[57]=h;R cbnew();}
static I _cdecl cbxalt59(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=59;cbx[50]=a;cbx[51]=b;cbx[52]=c;cbx[53]=d;cbx[54]=e;cbx[55]=f;cbx[56]=g;cbx[57]=h;cbx[58]=i;R cbnew();}

static I _cdecl cbxalt60(){cbxn=60;R cbnew();}
static I _cdecl cbxalt61(I a){cbxn=61;cbx[60]=a;R cbnew();}
static I _cdecl cbxalt62(I a,I b){cbxn=62;cbx[60]=a;cbx[61]=b;R cbnew();}
static I _cdecl cbxalt63(I a,I b,I c){cbxn=63;cbx[60]=a;cbx[61]=b;cbx[62]=c;R cbnew();}
static I _cdecl cbxalt64(I a,I b,I c,I d){cbxn=64;cbx[60]=a;cbx[61]=b;cbx[62]=c;cbx[63]=d;R cbnew();}
static I _cdecl cbxalt65(I a,I b,I c,I d,I e){cbxn=65;cbx[60]=a;cbx[61]=b;cbx[62]=c;cbx[63]=d;cbx[64]=e;R cbnew();}
static I _cdecl cbxalt66(I a,I b,I c,I d,I e,I f){cbxn=66;cbx[60]=a;cbx[61]=b;cbx[62]=c;cbx[63]=d;cbx[64]=e;cbx[65]=f;R cbnew();}
static I _cdecl cbxalt67(I a,I b,I c,I d,I e,I f,I g){cbxn=67;cbx[60]=a;cbx[61]=b;cbx[62]=c;cbx[63]=d;cbx[64]=e;cbx[65]=f;cbx[66]=g;R cbnew();}
static I _cdecl cbxalt68(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=68;cbx[60]=a;cbx[61]=b;cbx[62]=c;cbx[63]=d;cbx[64]=e;cbx[65]=f;cbx[66]=g;cbx[67]=h;R cbnew();}
static I _cdecl cbxalt69(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=69;cbx[60]=a;cbx[61]=b;cbx[62]=c;cbx[63]=d;cbx[64]=e;cbx[65]=f;cbx[66]=g;cbx[67]=h;cbx[68]=i;R cbnew();}

static I _cdecl cbxalt70(){cbxn=70;R cbnew();}
static I _cdecl cbxalt71(I a){cbxn=71;cbx[70]=a;R cbnew();}
static I _cdecl cbxalt72(I a,I b){cbxn=72;cbx[70]=a;cbx[71]=b;R cbnew();}
static I _cdecl cbxalt73(I a,I b,I c){cbxn=73;cbx[70]=a;cbx[71]=b;cbx[72]=c;R cbnew();}
static I _cdecl cbxalt74(I a,I b,I c,I d){cbxn=74;cbx[70]=a;cbx[71]=b;cbx[72]=c;cbx[73]=d;R cbnew();}
static I _cdecl cbxalt75(I a,I b,I c,I d,I e){cbxn=75;cbx[70]=a;cbx[71]=b;cbx[72]=c;cbx[73]=d;cbx[74]=e;R cbnew();}
static I _cdecl cbxalt76(I a,I b,I c,I d,I e,I f){cbxn=76;cbx[70]=a;cbx[71]=b;cbx[72]=c;cbx[73]=d;cbx[74]=e;cbx[75]=f;R cbnew();}
static I _cdecl cbxalt77(I a,I b,I c,I d,I e,I f,I g){cbxn=77;cbx[70]=a;cbx[71]=b;cbx[72]=c;cbx[73]=d;cbx[74]=e;cbx[75]=f;cbx[76]=g;R cbnew();}
static I _cdecl cbxalt78(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=78;cbx[70]=a;cbx[71]=b;cbx[72]=c;cbx[73]=d;cbx[74]=e;cbx[75]=f;cbx[76]=g;cbx[77]=h;R cbnew();}
static I _cdecl cbxalt79(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=79;cbx[70]=a;cbx[71]=b;cbx[72]=c;cbx[73]=d;cbx[74]=e;cbx[75]=f;cbx[76]=g;cbx[77]=h;cbx[78]=i;R cbnew();}

static I _cdecl cbxalt80(){cbxn=80;R cbnew();}
static I _cdecl cbxalt81(I a){cbxn=81;cbx[80]=a;R cbnew();}
static I _cdecl cbxalt82(I a,I b){cbxn=82;cbx[80]=a;cbx[81]=b;R cbnew();}
static I _cdecl cbxalt83(I a,I b,I c){cbxn=83;cbx[80]=a;cbx[81]=b;cbx[82]=c;R cbnew();}
static I _cdecl cbxalt84(I a,I b,I c,I d){cbxn=84;cbx[80]=a;cbx[81]=b;cbx[82]=c;cbx[83]=d;R cbnew();}
static I _cdecl cbxalt85(I a,I b,I c,I d,I e){cbxn=85;cbx[80]=a;cbx[81]=b;cbx[82]=c;cbx[83]=d;cbx[84]=e;R cbnew();}
static I _cdecl cbxalt86(I a,I b,I c,I d,I e,I f){cbxn=86;cbx[80]=a;cbx[81]=b;cbx[82]=c;cbx[83]=d;cbx[84]=e;cbx[85]=f;R cbnew();}
static I _cdecl cbxalt87(I a,I b,I c,I d,I e,I f,I g){cbxn=87;cbx[80]=a;cbx[81]=b;cbx[82]=c;cbx[83]=d;cbx[84]=e;cbx[85]=f;cbx[86]=g;R cbnew();}
static I _cdecl cbxalt88(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=88;cbx[80]=a;cbx[81]=b;cbx[82]=c;cbx[83]=d;cbx[84]=e;cbx[85]=f;cbx[86]=g;cbx[87]=h;R cbnew();}
static I _cdecl cbxalt89(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=89;cbx[80]=a;cbx[81]=b;cbx[82]=c;cbx[83]=d;cbx[84]=e;cbx[85]=f;cbx[86]=g;cbx[87]=h;cbx[88]=i;R cbnew();}

static I _cdecl cbxalt90(){cbxn=90;R cbnew();}
static I _cdecl cbxalt91(I a){cbxn=91;cbx[90]=a;R cbnew();}
static I _cdecl cbxalt92(I a,I b){cbxn=92;cbx[90]=a;cbx[91]=b;R cbnew();}
static I _cdecl cbxalt93(I a,I b,I c){cbxn=93;cbx[90]=a;cbx[91]=b;cbx[92]=c;R cbnew();}
static I _cdecl cbxalt94(I a,I b,I c,I d){cbxn=94;cbx[90]=a;cbx[91]=b;cbx[92]=c;cbx[93]=d;R cbnew();}
static I _cdecl cbxalt95(I a,I b,I c,I d,I e){cbxn=95;cbx[90]=a;cbx[91]=b;cbx[92]=c;cbx[93]=d;cbx[94]=e;R cbnew();}
static I _cdecl cbxalt96(I a,I b,I c,I d,I e,I f){cbxn=96;cbx[90]=a;cbx[91]=b;cbx[92]=c;cbx[93]=d;cbx[94]=e;cbx[95]=f;R cbnew();}
static I _cdecl cbxalt97(I a,I b,I c,I d,I e,I f,I g){cbxn=97;cbx[90]=a;cbx[91]=b;cbx[92]=c;cbx[93]=d;cbx[94]=e;cbx[95]=f;cbx[96]=g;R cbnew();}
static I _cdecl cbxalt98(I a,I b,I c,I d,I e,I f,I g,I h){cbxn=98;cbx[90]=a;cbx[91]=b;cbx[92]=c;cbx[93]=d;cbx[94]=e;cbx[95]=f;cbx[96]=g;cbx[97]=h;R cbnew();}
static I _cdecl cbxalt99(I a,I b,I c,I d,I e,I f,I g,I h,I i){cbxn=99;cbx[90]=a;cbx[91]=b;cbx[92]=c;cbx[93]=d;cbx[94]=e;cbx[95]=f;cbx[96]=g;cbx[97]=h;cbx[98]=i;R cbnew();}

static I cbvxalt[]={(I)&cbxalt00,(I)&cbxalt01,(I)&cbxalt02,(I)&cbxalt03,(I)&cbxalt04,(I)&cbxalt05,(I)&cbxalt06,(I)&cbxalt07,(I)&cbxalt08,(I)&cbxalt09, \
                    (I)&cbxalt10,(I)&cbxalt11,(I)&cbxalt12,(I)&cbxalt13,(I)&cbxalt14,(I)&cbxalt15,(I)&cbxalt16,(I)&cbxalt17,(I)&cbxalt18,(I)&cbxalt19, \
                    (I)&cbxalt20,(I)&cbxalt21,(I)&cbxalt22,(I)&cbxalt23,(I)&cbxalt24,(I)&cbxalt25,(I)&cbxalt26,(I)&cbxalt27,(I)&cbxalt28,(I)&cbxalt29, \
                    (I)&cbxalt30,(I)&cbxalt31,(I)&cbxalt32,(I)&cbxalt33,(I)&cbxalt34,(I)&cbxalt35,(I)&cbxalt36,(I)&cbxalt37,(I)&cbxalt38,(I)&cbxalt39, \
                    (I)&cbxalt40,(I)&cbxalt41,(I)&cbxalt42,(I)&cbxalt43,(I)&cbxalt44,(I)&cbxalt45,(I)&cbxalt46,(I)&cbxalt47,(I)&cbxalt48,(I)&cbxalt49, \
                    (I)&cbxalt50,(I)&cbxalt51,(I)&cbxalt52,(I)&cbxalt53,(I)&cbxalt54,(I)&cbxalt55,(I)&cbxalt56,(I)&cbxalt57,(I)&cbxalt58,(I)&cbxalt59, \
                    (I)&cbxalt60,(I)&cbxalt61,(I)&cbxalt62,(I)&cbxalt63,(I)&cbxalt64,(I)&cbxalt65,(I)&cbxalt66,(I)&cbxalt67,(I)&cbxalt68,(I)&cbxalt69, \
                    (I)&cbxalt70,(I)&cbxalt71,(I)&cbxalt72,(I)&cbxalt73,(I)&cbxalt74,(I)&cbxalt75,(I)&cbxalt76,(I)&cbxalt77,(I)&cbxalt78,(I)&cbxalt79, \
                    (I)&cbxalt80,(I)&cbxalt81,(I)&cbxalt82,(I)&cbxalt83,(I)&cbxalt84,(I)&cbxalt85,(I)&cbxalt86,(I)&cbxalt87,(I)&cbxalt88,(I)&cbxalt89, \
                    (I)&cbxalt90,(I)&cbxalt91,(I)&cbxalt92,(I)&cbxalt93,(I)&cbxalt94,(I)&cbxalt95,(I)&cbxalt96,(I)&cbxalt97,(I)&cbxalt98,(I)&cbxalt99  };
#endif
/* end of code generated by J script x15_callback.ijs */

F1(jtcallback){
 ARGCHK1(w);
 R jtcallback2(jt,sc(0),w);
}

F2(jtcallback2){I k1;
 cbjt=jt; /* callbacks don't work with multiple instances of j */
 ARGCHK2(a,w);
 RE(k1=i0(a));
 ASSERT(BETWEENC(k1,0,9),EVDOMAIN);
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
  R sc((alt?cbvxalt:cbvx)[k1*10+ --cnt]); /* select callback based on alt * args */
#else
  R sc(cbvx[k1*10+ --cnt]); /* select callback based on alt * args */
#endif
 }
 else
 {
  I k;
  RE(k=i0(w));
  ASSERT(0==k1,EVDOMAIN);
  ASSERT((UI)k<(UI)sizeof(cbv)/SZI, EVINDEX);
  R sc(cbv[k]);
 }
}    /* 15!:13 */

F1(jtnfes){I k;I r;
 ASSERT(!JT(jt,seclev),EVSECURE)
 RE(k=i0(w));
 ASSERT(BETWEENC(k,0,1),EVDOMAIN);
 r=JT(jt,nfe);
 JT(jt,nfe)=k;
 R sc(r);
} /* 15!:16 toggle native front end (nfe) state */

F1(jtcallbackx){
 ASSERT(!JT(jt,seclev),EVSECURE) ASSERTMTV(w);
 R vec(INT,cbxn,cbx);
} /* 15!:17 return x callback arguments */

F1(jtcddlopen){HMODULE h;
 ARGCHK1(w); ASSERT(!JT(jt,seclev),EVSECURE)
 ASSERT(LIT&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVRANK);
 if(AN(w)){
 ASSERT(AN(w),EVLENGTH);
 C*lib=CAV(str0(w));
#ifdef _WIN32
 wchar_t wlib[_MAX_PATH];
 MultiByteToWideChar(CP_UTF8,0,lib,1+(int)strlen(lib),wlib,_MAX_PATH);
 h=LoadLibraryW(wlib);
#else
#if defined(__wasm__) || defined(TARGET_IOS)
 CDASSERT(0,DEBADLIB);
#else
 h=dlopen((*lib)?lib:0,RTLD_LAZY);
#endif
#endif
 }else{
#ifdef _WIN32
 h=GetModuleHandle(NULL);
#else
#if defined(__wasm__) || defined(TARGET_IOS)
 CDASSERT(0,DEBADLIB);
#else
 h=dlopen(0,RTLD_LAZY);
#endif
#endif
 }
R sc((I)(intptr_t)h);
}    /* 15!:20 return library handle */

F2(jtcddlsym){C*proc;FARPROC f;HMODULE h;
 ARGCHK2(a,w); ASSERT(!JT(jt,seclev),EVSECURE)
 ASSERT(LIT&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(AN(w),EVLENGTH);
 proc=CAV(str0(w));
 RE(h=(HMODULE)i0(a));
#ifdef _WIN32
 f=GetProcAddress(h,(LPCSTR)proc);
#else
#if defined(__wasm__) || defined(TARGET_IOS)
 CDASSERT(0,DEBADLIB);
#else
 f=(FARPROC)dlsym(h,proc);
#endif
/*
 char *error;
 if(!f){
  if ((error = dlerror()) != NULL)  {
   fprintf(stderr, "%s\n", error);
  }
 }
*/
#endif
 R sc((I)(intptr_t)f);
}    /* 15!:21 return proc address */

F1(jtcddlclose){HMODULE h;I rc;
 ARGCHK1(w); ASSERT(!JT(jt,seclev),EVSECURE)
 RE(h=(HMODULE)i0(w));
#ifdef _WIN32
 rc= !FREELIB(h);       /* FreeLibrary return non-zero on success */
#else
 rc= !!FREELIB(h);      /* dlcose return zero on success */
#endif
R sc(rc);   /* return zero on success */
}    /* 15!:22 close lilbrary handle */

F1(jtcdq){I rc;
 ARGCHK1(w); ASSERT(!JT(jt,seclev),EVSECURE)
#if defined(__wasm__) || defined(TARGET_IOS)
 R sc(0);
#else
 R sc(1);
#endif
}    /* 15!:23 test 15!:0 availability */

