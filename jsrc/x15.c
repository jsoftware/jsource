/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: DLL call driver                                                  */

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
#ifdef __MINGW32__
#ifndef _stdcall
#define _stdcall __stdcall
#define _cdecl __cdecl
#endif
#endif
#else
#include <stdlib.h>
typedef unsigned char       BYTE;
#define CALLBACK
#define FIXWINUTF8
#endif
#include <stdint.h>
#include <wchar.h>

#include "j.h"

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

#define CDASSERT(p,c)   {if(!(p)){jt->dlllasterror=c; ASSERT(0,EVDOMAIN);}}

typedef struct {
 FARPROC fp;                    /* proc function address                */
 HMODULE h;                     /* library (module) handle              */
 I ai;                          /* argument string index in cdstr       */
 I an;                          /* argument string length               */
 I li;                          /* library name index in cdstr          */
 I ln;                          /* library name length                  */
 I pi;                          /* proc name index in cdstr             */
 I pn;                          /* proc name length                     */
 I n;                           /* number of arguments (excl. result)   */
 I zt;                          /* result jtype                         */
 C cc;                          /* call class: 0x00 or '0' or '1'       */
 C zl;                          /* result type letter                   */
 B zbx;                         /* > 1 iff result is boxed              */
 B fpreset;                     /* % 1 iff do FPRESET after call        */
 B alternate;                   /* + 1 iff alternate calling convention */
 B star[NCDARGS];               /* arguments star or not                */
 C tletter[NCDARGS];            /* arguments type letters, cwusi etc.   */
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
  #elif defined(__x86_64__)||defined(__aarch64__)
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

static I     stdcalli(STDCALLI fp,I*d,I cnt,DoF*dd,I dcnt){I r;
 SWITCHCALL;
 R r;
}  /* I result */
static I     altcalli(ALTCALLI fp,I*d,I cnt,DoF*dd,I dcnt){I r;
 SWITCHCALL;
 R r;
}
static D     stdcalld(STDCALLD fp,I*d,I cnt,DoF*dd,I dcnt){D r;
 SWITCHCALL;
 R r;
}  /* D result */
static D     altcalld(ALTCALLD fp,I*d,I cnt,DoF*dd,I dcnt){D r;
 SWITCHCALL;
 R r;
}

#if SY_64 || defined(__arm__)
static float stdcallf(STDCALLF fp,I*d,I cnt,DoF*dd,I dcnt){float r;
SWITCHCALL;
R r;
}  /* J64 float result */
static float altcallf(ALTCALLF fp,I*d,I cnt,DoF*dd,I dcnt){float r;
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
 if(strchr("cwusilx*n",zl)){I r;
  r= alternate ? altcalli((ALTCALLI)fp,d,cnt,dd,dcnt) : stdcalli((STDCALLI)fp,d,cnt,dd,dcnt);
  switch(zl){
   case 'c': *(C*)v=(C)r;  break;
   case 'w': *(US*)v=(US)r;break;
   case 'u': *(C4*)v=(C4)r;break;
   case 's': *v=(I)(S)r;   break;
   case 'i': *v=(I)(int)r; break;
   case 'l':
   case 'x':
   case '*': *v=r;         break;
   case 'n': *v=0;         break;
  }}
 else
#if !SY_64 && !defined(__arm__)
 {D r;
  r= alternate ? altcalld((ALTCALLD)fp,d,cnt,dd,dcnt) : stdcalld((STDCALLD)fp,d,cnt,dd,dcnt);
  *(D*)v=r;
 }
#else
 {/* the above doesn't work for J64 */
  if(zl=='d'){D r;
   r= alternate ? altcalld((ALTCALLD)fp,d,cnt,dd,dcnt) : stdcalld((STDCALLD)fp,d,cnt,dd,dcnt);
   *(D*)v=r;
  }else{float r;
   r= alternate ? altcallf((ALTCALLF)fp,d,cnt,dd,dcnt) : stdcallf((STDCALLF)fp,d,cnt,dd,dcnt);
   *(D*)v=(D)r;
 }}
#endif
}

static void convertdown(I*pi,I n,C t){
 if(n)switch(t){
  case 's': {short*pt=(short*)pi;             DO(n, pt[i]=(short)pi[i];);} break;
  case 'i': {int  *pt=(int  *)pi;             DO(n, pt[i]=(int)  pi[i];);} break;
  case 'f': {float*pt=(float*)pi;D*pd=(D*)pi; DO(n, pt[i]=(float)pd[i];);} break;
}}   /* convert I in place to s or int and d to f */

static void convertup(I*pi,I n,C t){I j=n;
 if(n)switch(t){
  case 's': {short*pt=(short*)pi;             DO(n, --j; pi[j]=(I)pt[j];);} break;
  case 'i': {int  *pt=(int  *)pi;             DO(n, --j; pi[j]=(I)pt[j];);} break;
  case 'f': {float*pt=(float*)pi;D*pd=(D*)pi; DO(n, --j; pd[j]=(D)pt[j];);} break;
}}   /* convert s or int to I and f to d */


/* cache of 15!:0 parsed left arguments                                 */
/*                                                                      */
/* cdarg   - append-only table of parsed results                        */
/*           rows are interpreted by CCT                                */
/*           when space runs out allocation is doubled                  */
/* cdna    - # used entries in cdarg                                    */
/* cdstr   - append-only table of 15!:0 left argument strings verbatim  */
/*           indexed by ai and an in the CCT struct                     */
/*           when space runs out allocation is doubled                  */
/* cdns    - # used chars in cdstr                                      */
/* cdhash  - hash table                                                 */
/*           # entries is a prime                                       */
/*           when 2*cdna exceeds # entries allocation is doubled        */
/*           table values are indices into cdarg, or -1                 */
/* cdhashl - hash table for libraries (modules)                         */
/* cdnl    - # used entries in cdhashl                                  */

static A jtcdgahash(J jt,I n){A z;I hn,*v;
 v=ptab+PTO; while(n>*v)++v; hn=*v;
 GATV(z,INT,hn,0,0); memset(AV(z),CFF,hn*SZI);  // no rank - use all words for table
 ra(z); R z;
}

static B jtcdinit(J jt){A x;
 RZ(x=exta(LIT,2L,sizeof(CCT),100L )); ra(x); memset(AV(x),C0,AN(x)); jt->cdarg=x;
 RZ(x=exta(LIT,1L,1L,         5000L)); ra(x); memset(AV(x),C0,AN(x)); jt->cdstr=x;
 RZ(jt->cdhash =cdgahash(4**AS(jt->cdarg)));
 RZ(jt->cdhashl=cdgahash(NLIBS           ));
 jt->cdna=jt->cdns=jt->cdnl=0;
 R 1;
}

static CCT*jtcdlookup(J jt,A a){C*s;CCT*pv;I an,hn,*hv,j,k;UC*av;
 hn=AN(jt->cdhash); hv=AV(jt->cdhash); pv=(CCT*)AV(jt->cdarg); s=CAV(jt->cdstr);
 an=AN(a); av=UAV(a); j=hic(an,av)%hn;
 while(0<=(k=hv[j])){if(an==pv[k].an&&!memcmp(av,s+pv[k].ai,an))R k+pv; j=(j+1)%hn;}
 R 0;
}

static HMODULE jtcdlookupl(J jt,C*av){C*s;CCT*pv;I an,hn,*hv,j,k;
 hn=AN(jt->cdhashl); hv=AV(jt->cdhashl); pv=(CCT*)AV(jt->cdarg); s=CAV(jt->cdstr);
 an=strlen(av); j=hic(an,av)%hn; 
 while(0<=(k=hv[j])){if(an==pv[k].ln&&!memcmp(av,s+pv[k].li,an))R pv[k].h; j=(j+1)%hn;}
 R 0;
}

static CCT*jtcdinsert(J jt,A a,CCT*cc){A x;C*s;CCT*pv,*z;I an,hn,*hv,j,k;
 an=AN(a);
 if(jt->cdns> AN(jt->cdstr)-an)RZ(jt->cdstr=ext(1,jt->cdstr));
 if(jt->cdna==*AS(jt->cdarg))  RZ(jt->cdarg=ext(1,jt->cdarg));
 s=CAV(jt->cdstr); pv=(CCT*)AV(jt->cdarg);
 cc->ai=jt->cdns; MC(s+jt->cdns,CAV(a),an); jt->cdns+=an;
 z=pv+jt->cdna; MC(z,cc,sizeof(CCT)); k=jt->cdna++;
 if(AN(jt->cdhash)<=2*jt->cdna){k=0; RZ(x=cdgahash(2*jt->cdna)); fa(jt->cdhash); jt->cdhash=x;}
 hv=AV(jt->cdhash); hn=AN(jt->cdhash); 
 DO(jt->cdna-k, j=hic(pv[k].an,s+pv[k].ai)%hn; while(0<=hv[j])j=(j+1)%hn; hv[j]=k; ++k;);
 R z;
}


static CCT*jtcdload(J jt,CCT*cc,C*lib,C*proc){B ha=0;FARPROC f;HMODULE h;
 /* not all platforms support GetModuleHandle, so we do it ourselves */
 /* we match on exactly the arg the user supplied                    */
 /* search path and case can cause us to reload the same dll         */
 if(cc->cc){C buf[SY_64?21:12];I k,n;
  n=strlen(proc);
  CDASSERT(n&&n<sizeof(buf),DEBADFN);
  k='_'==*proc?-strtoI(1+proc,0L,10):strtoI(proc,0L,10);
  CDASSERT(k&&'0'==*lib||0<=k&&'1'==*lib,DEBADFN);
  sprintf(buf,FMTI,k); if(0>k)*buf='_';
  CDASSERT(!strcmp(proc,buf),DEBADFN);
  cc->fp=(FARPROC)k;
  R cc;
 }
 if(h=cdlookupl(lib))cc->h=h;
 else{
  CDASSERT(jt->cdnl<NLIBS,DETOOMANY);    /* too many dlls loaded */
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
  cc->h=h; ha=1;
 }
#if SY_WIN32 && !SY_WINCE
 f=GetProcAddress(h,'#'==*proc?(LPCSTR)(I)atoi(proc+1):proc);    
#endif
#if SY_WINCE
 f=GetProcAddress(h,tounibuf(proc));
#endif
#if (SYS & SYS_UNIX)
 f=(FARPROC)dlsym(h,proc);
#endif
 CDASSERT(f,DEBADFN);
 cc->fp=f;
 /* assumes the hash table for libraries (jt->cdhashl) is fixed sized */
 /* assumes cc will be cached as entry number jt->cdna                */
 if(ha){I hn,*hv,j;
  ++jt->cdnl; hv=AV(jt->cdhashl); hn=AN(jt->cdhashl); 
  j=hic(cc->ln,lib)%hn; while(0<=hv[j])j=(j+1)%hn; hv[j]=jt->cdna;
 }
 R cc;
}

static I cdjtype(C c){R c=='c'?LIT:c=='w'?C2T:c=='u'?C4T:c=='j'?CMPX:(c=='f'||c=='d')?FL:c?INT:0;}
     /* J type from type letter */

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
/*  s   INT   short int (2 bytes)                               */
/*  i   INT   int;  4 byte on 32, 8 byte on 64                  */
/*  l   INT   long; error  on 32, 8 byte on 64                  */
/*  x   INT   long; 4 byte on 32, 8 byte on 64                  */
/*  f   FL    4 byte float                                      */
/*  d   FL    8 byte float                                      */
/*  j   CMPX  16 byte complex (must be preceded by *)           */
/*  *         pointer                                           */
/*  n   INT   no result (integer 0)                             */

static CCT*jtcdparse(J jt,A a,I empty){C c,lib[NPATH],*p,proc[NPATH],*s,*s0;CCT*cc,cct;I an,der,i,li,pi;
 ASSERT(LIT&AT(a),EVDOMAIN);
 ASSERT(1>=AR(a),EVRANK);
 ASSERT(NLEFTARG>=AN(a),EVLIMIT);
 if(cc=cdlookup(a))R cc;
 cc=&cct; cc->an=an=AN(a); s=s0=CAV(a); 
 /* library (module, file) name */
 while(*s==' ')++s; p=*s=='"'?strchr(++s,'"'):strchr(s,' '); li=s-s0; cc->ln=p?p-s:0;
 CDASSERT(p&&NPATH>cc->ln,DEBADLIB);
 cc->cc=1==cc->ln&&('0'==*s||'1'==*s)?*s:0;
 /* procedure name */
 s=p+1+(*p=='"'); 
 while(*s==' ')++s; p=strchr(s,' '); if(!p)p=s+strlen(s);    pi=s-s0; cc->pn=p-s;    
 CDASSERT(NPATH>cc->pn,DEBADFN);
 /* > + % */
 s=p+1;
 cc->zbx      =1; while(*s==' ')++s; if('>'==*s){cc->zbx      =0; ++s;}
 cc->alternate=0; while(*s==' ')++s; if('+'==*s){cc->alternate=1; ++s;}
 cc->fpreset  =0; while(*s==' ')++s; if('%'==*s){cc->fpreset  =1; ++s;}
 /* result type declaration */
 while(*s==' ')++s;
 if(empty&&!*s){
  cc->zl=c='x'; cc->zt=cdjtype(c);
 }else{
  CDASSERT(*s,DEDEC);
  cc->zl=c=*s++; cc->zt=cdjtype(c);
 }
 CDASSERT(strchr("cwusilxfd*n",c),DEDEC);
 CDASSERT(SY_64||'l'!=c,DEDEC);
 if(c=='*' && *s && strchr("cwusilxfdj",*s)) ++s;
 CDASSERT(!*s||*s==' ',DEDEC);
#ifdef C_CD_NODF // platform does not support f result
 CDASSERT(cc->zl!='f',DEDEC)  
#endif
 /* argument type declarations */
 i=-1;
 while(c=*s++){
  if(' '==c)continue;
  ++i; der=DEDEC+256*(1+i);
  CDASSERT(i<NCDARGS,DECOUNT);
  cc->tletter[i]=0; cc->star[i]=0;
  CDASSERT(i||'1'!=cc->cc||'x'==c||'*'==c&&(!*s||' '==*s),der);
  if('*'==c){cc->star[i]=1; c=*s++; if(!c)break; if(' '==c)continue;}
  cc->tletter[i]=c;
  CDASSERT(strchr("cwusilxfdj",c),der);
  CDASSERT(c!='j'||cc->star[i],der);
  if('l'==c){CDASSERT(SY_64,der); cc->tletter[i]='x';}
#ifdef C_CD_NODF // platform does not support f or d args
 CDASSERT(cc->star[i]==1 || (cc->tletter[i]!='f' && cc->tletter[i]!='d'),der);
#endif  
 }
 CDASSERT(0<=i||'1'!=cc->cc,DEDEC+256);
 MC(lib, s0+li,cc->ln); lib [cc->ln]=0;
 MC(proc,s0+pi,cc->pn); proc[cc->pn]=0;
 
#if SY_MAC && !SY_64
// mac osx 32 lseek off_t (64 bit) is not called properly
// map libc.dylib seek to be libj.dylib x15lseek32
if(!strcmp(lib,"libc.dylib")&&!strcmp(proc,"lseek"))
{
strcpy(lib,"libj.dylib");
strcpy(proc,"x15lseek32");
}
#endif
 
 RZ(cc=cdload(cc,lib,proc));
 cc->n=1+i; RZ(cc=cdinsert(a,cc)); cc->li=li+cc->ai; cc->pi=pi+cc->ai;
 R cc;
}

#define CDT(x,y) ((x)+32*(y))  // x runs from LIT to C4T, 2-18

static I*jtconvert0(J jt,I zt,I*v,I wt,C*u){D p,q;I k=0;US s;C4 s4;
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
   p=*(D*)u; q=jfloor(p);
   if(p<IMIN*(1+jt->fuzz)||IMAX*(1+jt->fuzz)<p)R 0; 
#if SY_64
   if         (FEQ(p,q)){k=(I)q; *v=SGN(k)==SGN(q)?k:0>q?IMIN:IMAX;}
   else if(++q,FEQ(p,q)){k=(I)q; *v=SGN(k)==SGN(q)?k:0>q?IMIN:IMAX;}
   else R 0;
#else
   if(FEQ(p,q))*v=(I)q; else if(FEQ(p,1+q))*v=(I)(1+q); else R 0;
#endif
 }
 R v;
}    /* convert a single atom. I from D code adapted from IfromD() in k.c */  

static B jtcdexec1(J jt,CCT*cc,C*zv0,C*wu,I wk,I wt,I wd){A*wv=(A*)wu,x,y,*zv;B zbx,lit,star;
    C c,cipt[NCDARGS],*u;FARPROC fp;float f;I cipcount=0,cipn[NCDARGS],*cipv[NCDARGS],cv0[2],
    data[NCDARGS*2],dcnt=0,fcnt=0,*dv,i,n,per,t,xn,xr,xt,*xv; DoF dd[NCDARGS];
 n=cc->n;
 CDASSERT(!n||wt&BOX||!(u=memchr(cc->star,C1,n)),DEPARM+256*(((B*)u)-cc->star));
 zbx=cc->zbx; zv=1+(A*)zv0; dv=data; u=wu; xr=0;
 for(i=0;i<n;++i){
#if SY_UNIX64 && defined(__x86_64__)
  if(dv-data>=6&&dv-data<dcnt-2)dv=data+dcnt-2;
#elif SY_UNIX64 && defined(__aarch64__)
  if(dcnt>8&&dv-data==8)dv=data+dcnt;    /* v0 to v7 fully filled before x0 to x7 */
#elif defined(C_CD_ARMHF)
  if((fcnt>16||dcnt>16)&&dv-data==4)dv=data+MAX(fcnt,dcnt)-12;  /* v0 to v15 fully filled before x0 to x3 */
#endif
  per=DEPARM+i*256; star=cc->star[i]; c=cc->tletter[i]; t=cdjtype(c);
  if(wt&BOX){
   x=WVR(i); xt=AT(x); xn=AN(x); xr=AR(x);
   CDASSERT(!xr||star,per);         /* non-pointers must be scalars */
   lit=star&&xt&LIT&&(c=='s'&&0==xn%2||c=='f'&&0==xn%4);
   if(t&&TYPESNE(t,xt)&&!(lit||star&&!xr&&xt&BOX)){x=cvt(xt=t,x); CDASSERT(x,per);}
   xv=AV(x); if(zbx)*zv++=x;
  }else{
   xv=convert0(t,cv0,wt,u); xt=t; u+=wk;
   CDASSERT(xv,per); 
   if(zbx){GA(y,t,1,0,0); MC(AV(y),xv,bp(t)); *zv++=y;}
  }
  if(star&&!xr&&xt&BOX){           /* scalar boxed integer/boolean scalar is a pointer */
   y=AAV0(x);
   CDASSERT(!AR(y)&&AT(y)&B01+INT,per);
   if(AT(y)&B01){CDASSERT(0==*BAV(y),per); *dv++=0;}else *dv++=*AV(y);
  }else if(star){
   CDASSERT(xr,per);                /* pointer can't point at scalar */
   *dv++=(I)xv;                     /* pointer to J array memory     */
   CDASSERT(xt&LIT+C2T+C4T+INT+FL+CMPX,per);
   if(!lit&&(c=='s'||c=='f'||SY_64&&c=='i')){
    cipv[cipcount]=xv;              /* convert in place arguments */
    cipn[cipcount]=xn; 
    cipt[cipcount]=c; 
    ++cipcount;
  }}else switch(c){
   case 'c': *dv++=*(C*)xv;  break;
   case 'w': *dv++=*(US*)xv; break;
   case 'u': *dv++=*(C4*)xv; break;
   case 's': *dv++=(S)*xv;   break;
   case 'i': *dv++=(int)*xv; break;
   case 'x': *dv++=*xv;      break;
   case 'f':
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
     {f=(float)*(D*)xv; dd[dcnt]=0; *(float*)(dd+dcnt++)=f;
      if(dcnt>8){
        if(dv-data>=8)*(float*)(dv++)=f;else *(float*)(data+dcnt-1)=f;}}
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
               if ((0==fcnt%2) && (fcnt<dcnt)) fcnt=dcnt;
               if ((1==fcnt%2) && (fcnt>dcnt)) dcnt=fcnt+1;
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
             break;
   case 'd':
#if SY_MACPPC
             dd[dcnt++]=*(D*)xv;
#endif
#if SY_UNIX64
#if defined(__PPC64__)
             /* always need to increment dv, the contents get used from the 14th D */
             *(D*)dv++=dd[dcnt++]=*(D*)xv;
#elif defined(__aarch64__)
             dd[dcnt++]=*(D*)xv;
             if(dcnt>8){
               if(dv-data>=8)*dv++=*xv;else *(data+dcnt-1)=*xv;}
#elif defined(__x86_64__)
             dd[dcnt++]=*(D*)xv;
             if(dcnt>8){ /* push the 9th D and more on to stack (must be the 7th I onward) */
               if(dv-data>=6)*dv++=*xv;else *(data+dcnt-3)=*xv;}
#endif
#endif
#if !SY_UNIX64
#ifdef C_CD_ARMHF
            {if(dcnt<16){
               if (dcnt==fcnt) fcnt+=2;
               *(D*)(dd+dcnt++)= *(D*)xv; dcnt++;
             }else{
               if(dv-data>=4){
                 if((dv-data)%2)dv++;
                 *(D*)(dv++)=*(D*)xv; dv++;
                 dcnt=(dv-data)+12;
               }else{
                 dcnt=MAX(fcnt,dcnt);
                 if(dcnt%2)dcnt++;
                 *(D*)(data+dcnt++ -12)=*(D*)xv; dcnt++;
             }}}
#else
#ifdef C_CD_ARMEL
             if((data-dv)%2) *dv++=0;   /* 8-byte alignment for double */
#endif
             *dv++=xv[0];
#if !SY_64
             *dv++=xv[1];
#endif
#endif
#endif
 }}
#ifdef C_CD_ARMHF
// CDASSERT(16>=fcnt,DELIMIT);
// CDASSERT(16>=dcnt,DELIMIT);
 if((fcnt>16||dcnt>16)&&dv-data<=4)dv=data+MAX(fcnt,dcnt)-12;  /* update dv to point to the end */
#elif SY_UNIX64 && defined(__x86_64__)
 if(dcnt>8&&dv-data<=6)dv=data+dcnt-2; /* update dv to point to the end */
#elif SY_UNIX64 && defined(__aarch64__)
 if(dcnt>8&&dv-data<=8)dv=data+dcnt;  /* update dv to point to the end */
#elif !SY_64
 CDASSERT(dv-data<=NCDARGS,DECOUNT); /* D needs 2 I args in 32bit system, check it again. */
#endif

 DO(cipcount, convertdown(cipv[i],cipn[i],cipt[i]););  /* convert I to s and int and d to f as required */
 if(zbx){GA(x,cc->zt,1,0,0); xv=AV(x); *(A*)zv0=x;}else xv=(I*)zv0;
 if('1'==cc->cc){fp=(FARPROC)*((I)cc->fp+(I*)*(I*)*data); CDASSERT(fp,DEBADFN);}else fp=cc->fp;
 docall(fp, data, dv-data, dd, dcnt, cc->zl, xv, cc->alternate);
    
 DO(cipcount, convertup(cipv[i],cipn[i],cipt[i]);); /* convert s and int to I and f to d as required */
#if SY_WIN32
 t= GetLastError();
 if(cc->fpreset)_fpreset();         /* delphi dll (and others) damage fp state */
#endif
#if SYS&SYS_UNIX
 t=errno;
#endif
 if(t!=0)jt->getlasterror=t;
 R 1;
}

F2(jtcd){A z;C*tv,*wv,*zv;CCT*cc;I k,m,n,p,q,t,wd,wr,*ws,wt;
 RZ(a&&w);
 if(!jt->cdarg)RZ(cdinit());
 if(1<AR(a))R rank2ex(a,w,0L,1L,1L,jtcd);
 wt=AT(w); wr=AR(w); ws=AS(w); m=wr?prod(wr-1,ws):1;
 ASSERT(wt&DENSE,EVDOMAIN);
 RZ(cc=cdparse(a,0)); 
 n=cc->n; 
 CDASSERT(n==(wr?ws[wr-1]:1),DECOUNT);
 if(cc->zbx){GATV(z,BOX,m*(1+n),MAX(1,wr),ws); *(AS(z)+AR(z)-1)=1+n;}
 else{CDASSERT('*'!=cc->zl,DEDEC); GA(z,cc->zt,m,MAX(0,wr-1),ws);}
 if(m&&n&&!(wt&BOX)){
  t=0; tv=cc->tletter; DO(n, k=cdjtype(*tv++); t=MAX(t,k););
  CDASSERT(HOMO(t,wt),DEPARM);
  if(!(wt&B01+INT+FL+LIT+C2T+C4T))RZ(w=cvt(wt=t,w)); 
 }
 wv=CAV(w); zv=CAV(z); k=bp(wt); wd=(I)w*ARELATIVE(w);
 if(1==m)RZ(cdexec1(cc,zv,wv,k,wt,wd))
 else{p=n*k; q=cc->zbx?sizeof(A)*(1+n):bp(AT(z)); DO(m, RZ(cdexec1(cc,zv,wv,k,wt,wd)); wv+=p; zv+=q;);}
 R z;
}    /* 15!:0 */


#if SY_WIN32
#define FREELIB FreeLibrary
#endif
#if (SYS & SYS_UNIX)
#define FREELIB dlclose
#endif

void dllquit(J jt){CCT*av;I j,*v;
 if(!jt->cdarg)R;
 v=AV(jt->cdhashl); av=(CCT*)AV(jt->cdarg);
 DO(AN(jt->cdhashl), j=*v++; if(0<=j)FREELIB(av[j].h););
 fa(jt->cdarg);   jt->cdarg  =0; jt->cdna=0;
 fa(jt->cdstr);   jt->cdstr  =0; jt->cdns=0;
 fa(jt->cdhash);  jt->cdhash =0;
 fa(jt->cdhashl); jt->cdhashl=0; jt->cdnl=0;
}    /* dllquit - shutdown and cdf clean up dll call resources */

F1(jtcdf){ASSERTMTV(w); dllquit(jt); R mtm;}
     /* 15!:5 */

/* return error info from last cd domain error - resets to DEOK */
F1(jtcder){I t; ASSERTMTV(w); t=jt->dlllasterror; jt->dlllasterror=DEOK; R v2(t&0xff,t>>8);}
     /* 15!:10 */

/* return errno info from last cd with errno not equal to 0 - resets to 0 */
F1(jtcderx){I t;C buf[1024];
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
 R link(sc(t),cstr(buf));
}    /* 15!:11  GetLastError information */

F1(jtmema){I k; RE(k=i0(w)); R sc((I)MALLOC(k));} /* ce */
     /* 15!:3  memory allocate */

F1(jtmemf){I k; RE(k=i0(w)); FREE((void*)k); R zero;}
     /* 15!:4  memory free */

F1(jtmemr){C*u;I k,m,n,t,*v;US*us;C4*c4;
 RZ(w);
 ASSERT(INT&AT(w),EVDOMAIN);
 ASSERT(1==AR(w),EVRANK);
 n=AN(w); v=AV(w);
 ASSERT(3==n||4==n,EVLENGTH);
 m=v[2]; t=3==n?LIT:v[3]; u=(C*)(v[0]+v[1]);
 ASSERT(t&LIT+C2T+C4T+INT+FL+CMPX+SBT,EVDOMAIN);
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
 k=bp(t);
#if SY_WIN32
// This function is obsolete and should not be used
// ASSERT(!IsBadReadPtr(u,m*k),EVDOMAIN);
#endif
 R vec(t,m,u);
}    /* 15!:1  memory read */

F2(jtmemw){C*u;I k,m,n,t,*v;
 RZ(a&&w);
 ASSERT(INT&AT(w),EVDOMAIN);
 ASSERT(1==AR(w),EVRANK);
 n=AN(w); v=AV(w);
 ASSERT(3==n||4==n,EVLENGTH);
 m=v[2]; t=3==n?LIT:v[3]; u=(C*)(v[0]+v[1]);
 ASSERT(t&LIT+C2T+C4T+INT+FL+CMPX+SBT,EVDOMAIN);
 k=bp(t);
 ASSERT(m==AN(a)||t&LIT+C2T+C4T&&1==AR(a)&&(m-1)==AN(a),EVLENGTH);
 if(B01&AT(a)&&t&INT) RZ(a=cvt(INT,a));
 ASSERT(TYPESEQ(t,AT(a)),EVDOMAIN);
#if SY_WIN32
// This function is obsolete and should not be used
// ASSERT(!IsBadWritePtr(u,m*k),EVDOMAIN);
#endif
 MC(u,AV(a),m*k);
 R mtm;
}    /* 15!:2  memory write */

F1(jtgh15){A z;I k; RE(k=i0(w)); RZ(z=gah(k,0L)); ACINCR(z); R sc((I)z);}
     /* 15!:8  get header */

F1(jtfh15){I k; RE(k=i0(w)); fh((A)k); R zero;}
     /* 15!:9  free header */

F1(jtdllsymset){RZ(w); R (A)i0(w);}      /* do some validation here */
     /* 15!:7 */

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
 if(INT&AT(r)) R *AV(r);
 if(B01&AT(r)) R *(BYTE*)AV(r);
 R 0;
}

static I cbnew(){A r;
 J jt=cbjt;
 r=exec1(cstr("cdcallback''"));
 if(!r||AR(r)) R 0;
 if(INT&AT(r)) R *AV(r);
 if(B01&AT(r)) R *(BYTE*)AV(r);
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
 RZ(w);
 if(LIT&AT(w))
 {
  I cnt,alt;C c;C* s;
  ASSERT(1>=AR(w),EVRANK);
  s=CAV(w);
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
  ASSERT(k>=0&&k<sizeof(cbv)/SZI, EVINDEX);
  R sc(cbv[k]);
 }
}    /* 15!:13 */

F1(jtnfes){I k;I r;
 RE(k=i0(w));
 r=jt->nfe;
 jt->nfe=k;
 R sc(r);
} /* 15!:16 toggle native front end (nfe) state */

F1(jtcallbackx){
 ASSERTMTV(w);
 R vec(INT,cbxn,cbx);
} /* 15!:17 return x callback arguments */

F1(jtnfeoutstr){I k;
 RE(k=i0(w));
 ASSERT(0==k,EVDOMAIN);
 R cstr(jt->mtyostr?jt->mtyostr:(C*)"");
} /* 15!:18 return last jsto output */

F1(jtcdjt){
 ASSERTMTV(w);
 R sc((I)(intptr_t)jt);
} /* 15!:19 return jt */

F1(jtcdlibl){
 RZ(w);
 ASSERT(LIT&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(AN(w),EVLENGTH);
 if(!jt->cdarg)R zero;
 R sc((I)cdlookupl(CAV(w)));
}    /* 15!:20 return library handle */

F1(jtcdproc1){CCT*cc;
 RZ(w);
 ASSERT(LIT&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(AN(w),EVLENGTH);
 if(!jt->cdarg)RE(cdinit());
 RE(cc=cdparse(w,1));
 R sc((I)cc->fp);
}    /* 15!:21 return proc address */

#ifdef _MSC_VER
#pragma warning(disable: 4276)
#endif

#if SY_WIN32
#define VARIANT void
int _stdcall JBreak(J jt);
int _stdcall JIsBusy(J jt);
int _stdcall JGet(J jt, C* name, VARIANT* v);
int _stdcall JGetB(J jt, C* name, VARIANT* v);
int _stdcall JSet(J jt, C* name, VARIANT* v);
int _stdcall JSetB(J jt, C* name, VARIANT* v);
int _stdcall JErrorText(J jt, long ec, VARIANT* v);
int _stdcall JClear(J jt);
int _stdcall JTranspose(J jt, long b);
int _stdcall JErrorTextB(J jt, long ec, VARIANT* v);
int _stdcall JDoR(J jt, C* p, VARIANT* v);
int _stdcall JInt64(J jt, long b);
#endif

// procedures in jlib.h
static void* jfntaddr[]={
JDo,
JErrorTextM,
JFree,
JGetA,
JGetLocale,
JGetM,
JInit,
JSM,
JSetA,
JSetM,
Jga,
#if SY_WIN32
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
JInt64,
#endif
};

static C* jfntnm[]={
"JDo",
"JErrorTextM",
"JFree",
"JGetA",
"JGetLocale",
"JGetM",
"JInit",
"JSM",
"JSetA",
"JSetM",
"Jga",
#if SY_WIN32
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
"JInt64",
#endif
};

F2(jtcdproc2){C*proc;FARPROC f;HMODULE h;
 RZ(a&&w);
 ASSERT(LIT&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(AN(w),EVLENGTH);
 proc=CAV(w);
 RE(h=(HMODULE)i0(a));
 if(!h){I k=-1;
  DO(sizeof(jfntnm)/sizeof(C*), if(!strcmp(jfntnm[i],proc)){k=i; break;});
  f=(k==-1)?(FARPROC)0:(FARPROC)jfntaddr[k];
 }else{
#if SY_WIN32 && !SY_WINCE
  f=GetProcAddress(h,'#'==*proc?(LPCSTR)(I)atoi(proc+1):proc);
#endif
#if SY_WINCE
  f=GetProcAddress(h,tounibuf(proc));
#endif
#if (SYS & SYS_UNIX)
  f=(FARPROC)dlsym(h,proc);
#endif
 }
 CDASSERT(f,DEBADFN);
 R sc((I)f);
}    /* 15!:21 return proc address */

