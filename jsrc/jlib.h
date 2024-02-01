/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* libj header                                                             */

#ifndef JLIB_H
#define JLIB_H

/* maintainer note: define JFRONTEND for non jsource front-ends */
// #define JFRONTEND
#ifdef JFRONTEND
#ifndef _WIN32
#define _stdcall
#endif
#ifdef _WIN32
#define CDPROC
#elif defined(__GNUC__)
#define CDPROC __attribute__ ((visibility ("default")))
#else
#define CDPROC
#endif
#if defined(_WIN64)||defined(__LP64__)
typedef long long I;
#define FMTI            "%lli"
#else
typedef long I;
#define FMTI            "%li"
#endif
typedef char C;
typedef void* JS;
typedef struct A_RECORD {
  I k,flag,m,t,c,n,r,s[1];
}* A;
typedef struct AREP_RECORD {
  I n,t,c,r,s[1];
}* AREP;
#define AK(x)           ((x)->k)        /* offset of ravel wrt x           */
#define AT(x)           ((x)->t)        /* Type; one of the #define below  */
#define AC(x)           ((x)->c)        /* Reference count.                */
#define AN(x)           ((x)->n)        /* # elements in ravel             */
#define AR(x)           ((x)->r)        /* Rank                            */
#define ARNK(x)         ((unsigned char)((x)->r))  /* Rank lowest byte     */
#define AS(x)           ((x)->s)
#define B01             ((I)1L<<0)           // B  boolean
#define LIT             ((I)1L<<1)           // C  literal (character)
#define INT             ((I)1L<<2)           // I  integer
#define FL              ((I)1L<<3)           // D  double (IEEE floating point)
#define CMPX            ((I)1L<<4)           // Z  complex
#define BOX             ((I)1L<<5)           // A  boxed
#define XNUM            ((I)1L<<6)           // X  extended precision integer
#define RAT             ((I)1L<<7)           // Q  rational number
#define PYX             ((I)1L<<8) // if BOX set, this flag is set if the value is a pyx.  A pyx is an atomic box (which may be an element of an array).
#define SBT             ((I)1L<<16)          // SB symbol
#define C2T             ((I)1L<<17)          // C2 unicode (2-byte characters)
#define C4T             ((I)1L<<18)          // C4 unicode (4-byte characters)
#define AV(x)           ( (I*)((C*)(x)+AK(x)))  /* pointer to ravel        */
#define CAV(x)          (      (C*)(x)+AK(x) )  /* character               */
#define IAV(x)          AV(x)                   /* integer                 */
#endif  // JFRONTEND

#ifdef __cplusplus
extern "C" {
#endif
CDPROC JS _stdcall JInit(void);                     /* init instance */
CDPROC JS _stdcall JInit2(C*libpath);               /* init instance with libpath */
CDPROC void _stdcall JSM(JS jt, void**callbacks);  /* set callbacks */
CDPROC void _stdcall JSMX(JS jt, void*, void*, void*, void*, I);
CDPROC int _stdcall JDo(JS jt,C*);                  /* run sentence */
CDPROC void _stdcall JInterrupt(JS jt);             /* signal interrupt */
CDPROC C* _stdcall JGetLocale(JS jt);               /* get locale */
CDPROC A _stdcall Jga(JS jt, I t, I n, I r, I*s);
CDPROC int _stdcall JFree(JS jt);                   /* free instance */
CDPROC A _stdcall JGetA(JS jt,I n,C* name);         /* get 3!:1 from name */
CDPROC C* _stdcall JGetR(JS jt);                    /* get capture */
CDPROC I _stdcall JSetA(JS jt,I n,C* name,I x,C* d);/* name=:3!:2 data */
CDPROC int _stdcall JGetM(JS jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata);
CDPROC int _stdcall JSetM(JS jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata);
CDPROC int _stdcall JErrorTextM(JS jt, I ec, I* p);
#ifdef __cplusplus
}
#endif

typedef void* (_stdcall *JInitType)     ();
typedef void* (_stdcall *JInit2Type)    (C*);
typedef int   (_stdcall *JDoType)       (JS, C*);
typedef void  (_stdcall *JInterruptType)(JS);
typedef C*    (_stdcall *JGetLocaleType)(JS);
typedef void  (_stdcall *JSMType)       (JS, void**);
typedef void  (_stdcall *JSMXType)      (JS, void*, void*, void*, void*, I);
typedef int   (_stdcall *JFreeType)     (JS);
typedef A     (_stdcall *JgaType)       (JS jt, I t, I n, I r, I*s);
typedef int   (_stdcall *JGetMType)     (JS jt,C* name,I* jtype,I* jrank,I* jshape,I* jdata);
typedef int   (_stdcall *JSetMType)     (JS jt,C* name,I* jtype,I* jrank,I* jshape,I* jdata);
typedef C*    (_stdcall *JGetRType)     (JS jt);
typedef int   (_stdcall *JErrorTextMType)(JS jt,I ec,I* p);
typedef A     (_stdcall *JGetAType)     (JS jt,I n,C* name);
typedef I     (_stdcall *JSetAType)     (JS jt,I n,C* name,I x,C* d);

/*  void* callbacks[] = {Joutput, Jwd, Jinput, unused, smoptions}; */

typedef void  (_stdcall * outputtype)(JS,int,C*);
typedef int   (_stdcall * dowdtype)  (JS,int, A, A*);
typedef int   (_stdcall * dowdtype2) (JS,int, A, A*, C*);  // pass current locale
typedef C* (_stdcall * inputtype) (JS,C*);
typedef C* (_stdcall * polltype) (JS,int,int);

#ifdef __cplusplus
extern "C" {
#endif
void _stdcall Joutput(JS jt, int type, C* s);
int _stdcall Jwd(JS jt, int x, A parg, A* pres, C* loc);
C* _stdcall Jinput(JS jt, C*);
#ifdef __cplusplus
}
#endif

// output type
#define MTYOFM  1 /* formatted result array output */
#define MTYOER  2 /* error output */
#define MTYOLOG  3 /* output log */
#define MTYOSYS  4 /* system assertion failure */
#define MTYOEXIT 5 /* exit */
#define MTYOFILE 6 /* output 1!:2[2 */

// smoptions
#define SMWIN    0  /* j.exe    Jwdw (Windows) front end */
#define SMJAVA   2  /* j.jar    Jwdp (Java) front end */
#define SMCON    3  /* jconsole */
#define SMQT     4  /* jqt */
#define SMJA     5  /* jandroid */

#define SMOPTLOCALE 1  /* pass current locale */
#define SMOPTNOJGA  2  /* result not allocated by jga */
#define SMOPTPOLL   4  /* use smpoll to get last result */
#define SMOPTMTH    8  /* multithreaded */

#endif
