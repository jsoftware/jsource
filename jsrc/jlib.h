/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */

CDPROC JST* _stdcall JInit(void);                         /* init instance */
CDPROC void _stdcall JSM(JST* jt, void*callbacks[]);  /* set callbacks */
CDPROC void _stdcall JSMX(JST* jt, void*, void*, void*, void*, I);
CDPROC int _stdcall JDo(JST* jt,C*);                  /* run sentence */
CDPROC C* _stdcall JGetLocale(JST* jt);               /* get locale */
CDPROC A _stdcall Jga(JST* jt, I t, I n, I r, I*s);
CDPROC int _stdcall JFree(JST* jt);                   /* free instance */
CDPROC A _stdcall JGetA(JST* jt,I n,C* name);         /* get 3!:1 from name */
CDPROC C* _stdcall JGetR(JST* jt);                    /* get capture */
CDPROC I _stdcall JSetA(JST* jt,I n,C* name,I x,C* d);/* name=:3!:2 data */
CDPROC int _stdcall JGetM(JST* jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata);
CDPROC int _stdcall JSetM(JST* jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata);
CDPROC int _stdcall JErrorTextM(JST* jt, I ec, I* p);

typedef void* (_stdcall *JInitType)     ();
typedef int   (_stdcall *JDoType)       (JST*, C*);
typedef C*    (_stdcall *JGetLocaleType)(JST*);
typedef void  (_stdcall *JSMType)       (JST*, void*);
typedef void  (_stdcall *JSMXType)      (JST*, void*, void*, void*, void*, I);
typedef void  (_stdcall *JFreeType)     (JST*);
typedef A     (_stdcall *JgaType)       (JST* jt, I t, I n, I r, I*s);
typedef int   (_stdcall *JGetMType)     (JST* jt,C* name,I* jtype,I* jrank,I* jshape,I* jdata);
typedef int   (_stdcall *JSetMType)     (JST* jt,C* name,I* jtype,I* jrank,I* jshape,I* jdata);
typedef C*    (_stdcall *JGetRType)     (JST* jt);
typedef int   (_stdcall *JErrorTextMType)(JST* jt,I ec,I* p);
typedef A     (_stdcall *JGetAType)     (JST* jt,I n,C* name);
typedef I     (_stdcall *JSetAType)     (JST* jt,I n,C* name,I x,C* d);

/*  void* callbacks[] = {Joutput, Jwd, Jinput, unused, smoptions}; */

typedef void  (_stdcall * outputtype)(JST*,int,C*);
typedef int   (_stdcall * dowdtype)  (JST*,int, A, A*);
typedef int   (_stdcall * dowdtype2) (JST*,int, A, A*, C*);  // pass current locale
typedef C* (_stdcall * inputtype) (JST*,C*);
typedef C* (_stdcall * polltype) (JST*,int,int);

void _stdcall Joutput(JST* jt, int type, C* s);
int _stdcall Jwd(JST* jt, int x, A parg, A* pres);
C* _stdcall Jinput(JST* jt, C*);

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
