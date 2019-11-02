/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */

J _stdcall JInit();                         /* init instance */
void _stdcall JSM(J jt, void*callbacks[]);  /* set callbacks */
void _stdcall JSMX(J jt, void*, void*, void*, void*, I);
int _stdcall JDo(J jt,C*);                  /* run sentence */
C* _stdcall JGetLocale(J jt);               /* get locale */
A _stdcall Jga(J jt, I t, I n, I r, I*s);
int _stdcall JFree(J jt);                   /* free instance */
A _stdcall JGetA(J jt,I n,C* name);         /* get 3!:1 from name */
C* _stdcall JGetR(J jt);                    /* get capture */
I _stdcall JSetA(J jt,I n,C* name,I x,C* d);/* name=:3!:2 data */
int _stdcall JGetM(J jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata);
int _stdcall JSetM(J jt, C* name, I* jtype, I* jrank, I* jshape, I* jdata);
int _stdcall JErrorTextM(J jt, I ec, I* p);

typedef void* (_stdcall *JInitType)     ();
typedef int   (_stdcall *JDoType)       (J, C*);
typedef C*    (_stdcall *JGetLocaleType)(J);
typedef void  (_stdcall *JSMType)       (J, void*);
typedef void  (_stdcall *JSMXType)      (J, void*, void*, void*, void*, I);
typedef void  (_stdcall *JFreeType)     (J);
typedef A     (_stdcall *JgaType)       (J jt, I t, I n, I r, I*s);
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

