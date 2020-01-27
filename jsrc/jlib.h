/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */

CDPROC J _stdcall JInit();                         /* init instance */
CDPROC void _stdcall JSM(J jt, void*callbacks[]);  /* set callbacks */
CDPROC void _stdcall JSMX(J jt, void*, void*, void*, void*, I);
CDPROC int _stdcall JDo(J jt,C*);                  /* run sentence */
CDPROC C* _stdcall JGetLocale(J jt);               /* get locale */
CDPROC A _stdcall Jga(J jt, I t, I n, I r, I*s);
CDPROC int _stdcall JFree(J jt);                   /* free instance */
CDPROC A _stdcall JGetA(J jt,I n,C* name);         /* get 3!:1 from name */
CDPROC C* _stdcall JGetR(J jt);                    /* get capture */
CDPROC I _stdcall JSetA(J jt,I n,C* name,I x,C* d);/* name=:3!:2 data */
CDPROC int _stdcall JGetM(J jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata);
CDPROC int _stdcall JSetM(J jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata);
CDPROC int _stdcall JErrorTextM(J jt, I ec, I* p);

typedef void* (_stdcall *JInitType)     ();
typedef int   (_stdcall *JDoType)       (J, C*);
typedef C*    (_stdcall *JGetLocaleType)(J);
typedef void  (_stdcall *JSMType)       (J, void*);
typedef void  (_stdcall *JSMXType)      (J, void*, void*, void*, void*, I);
typedef void  (_stdcall *JFreeType)     (J);
typedef A     (_stdcall *JgaType)       (J jt, I t, I n, I r, I*s);
typedef int   (_stdcall *JGetMType)     (J jt,C* name,I* jtype,I* jrank,I* jshape,I* jdata);
typedef int   (_stdcall *JSetMType)     (J jt,C* name,I* jtype,I* jrank,I* jshape,I* jdata);
typedef C*    (_stdcall *JGetRType)     (J jt);
typedef int   (_stdcall *JErrorTextMType)(J jt,I ec,I* p);
typedef A     (_stdcall *JGetAType)     (J jt,I n,C* name);
typedef I     (_stdcall *JSetAType)     (J jt,I n,C* name,I x,C* d);

/*  void* callbacks[] = {Joutput, Jwd, Jinput, unused, smoptions}; */

typedef void  (_stdcall * outputtype)(J,int,C*);
typedef int   (_stdcall * dowdtype)  (J,int, A, A*);
typedef int   (_stdcall * dowdtype2) (J,int, A, A*, C*);  // pass current locale
typedef C* (_stdcall * inputtype) (J,C*);
typedef C* (_stdcall * polltype) (J,int,int);

void _stdcall Joutput(J jt, int type, C* s);
int _stdcall Jwd(J jt, int x, A parg, A* pres);
C* _stdcall Jinput(J jt, C*);

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
