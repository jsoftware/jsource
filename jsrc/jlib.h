/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */

/* maintainer note: define JFRONTEND for non jsource front-ends */
#ifdef JFRONTEND
#undef JS
typedef void* JS
#endif

CDPROC JS _stdcall JInit(void);                     /* init instance */
CDPROC JS _stdcall JInit2(C*libpath);               /* init instance with libpath */
CDPROC void _stdcall JSM(JS jt, void*callbacks[]);  /* set callbacks */
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

typedef void* (_stdcall *JInitType)     ();
typedef void* (_stdcall *JInit2Type)    (C*);
typedef int   (_stdcall *JDoType)       (JS, C*);
typedef void  (_stdcall *JInterruptType)(JS);
typedef C*    (_stdcall *JGetLocaleType)(JS);
typedef void  (_stdcall *JSMType)       (JS, void*);
typedef void  (_stdcall *JSMXType)      (JS, void*, void*, void*, void*, I);
typedef void  (_stdcall *JFreeType)     (JS);
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

void _stdcall Joutput(JS jt, int type, C* s);
int _stdcall Jwd(JS jt, int x, A parg, A* pres);
C* _stdcall Jinput(JS jt, C*);

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
