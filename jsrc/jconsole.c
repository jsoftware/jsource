/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/* J console */
/* #define READLINE for Unix readline support */
#ifdef _WIN32
#include <windows.h>
#include <io.h> 
#include <fcntl.h>
#else
#include <unistd.h>
#include <sys/resource.h>
#define _isatty isatty
#define _fileno fileno
#include <dlfcn.h>
#define GETPROCADDRESS(h,p) dlsym(h,p)
#endif
#include <signal.h>
#include <stdint.h>

#include "j.h"
#include "jeload.h"

static int breadline=0;    /* 0: none  1: libedit  2: linenoise */
static char **adadbreak;
static void sigint(int k){**adadbreak+=1;signal(SIGINT,sigint);}
static void sigint2(int k){**adadbreak+=1;}
static char input[30000];

/* J calls for keyboard input (debug suspension and 1!:1[1) */
/* we call to get next input */
#ifdef READLINE
/* readlin.h */
/* if not working properly, export TERM=dumb */
#if defined(USE_LINENOISE)
#include "linenoise.h"
#endif
#ifndef __MACH__
typedef int (*ADD_HISTORY) (const char *);
typedef int (*READ_HISTORY) (const char *);
typedef int (*WRITE_HISTORY) (const char *);
typedef char* (*PREADLINE) (const char *);
typedef void (*USING_HISTORY) (void);
static void *hreadline=0;
static ADD_HISTORY add_history;
static READ_HISTORY read_history;
static WRITE_HISTORY write_history;
static PREADLINE readline;
static USING_HISTORY using_history;
#else
extern int   add_history(const char *);
extern int   read_history(const char *);
extern int   write_history(const char *);
extern char* readline(const char *);
extern void  using_history(void);
#endif
char* rl_readline_name;

int hist=1;
char histfile[512];

#ifndef __MACH__
#if !defined(ANDROID) && !defined(_WIN32)
static int readlineinit()
{
 if(hreadline)return 0; // already run
 if(!(hreadline=dlopen("libedit.so.3",RTLD_LAZY)))
 if(!(hreadline=dlopen("libedit.so.2",RTLD_LAZY)))
  if(!(hreadline=dlopen("libedit.so.1",RTLD_LAZY)))
   if(!(hreadline=dlopen("libedit.so.0",RTLD_LAZY))){
#if defined(USE_LINENOISE)
    add_history=linenoiseHistoryAdd;
    read_history=linenoiseHistoryLoad;
    write_history=linenoiseHistorySave;
    readline=linenoise;
    return 2;
#else
    return 0;
#endif
   }
 add_history=(ADD_HISTORY)GETPROCADDRESS(hreadline,"add_history");
 read_history=(READ_HISTORY)GETPROCADDRESS(hreadline,"read_history");
 write_history=(WRITE_HISTORY)GETPROCADDRESS(hreadline,"write_history");
 readline=(PREADLINE)GETPROCADDRESS(hreadline,"readline");
 using_history=(USING_HISTORY)GETPROCADDRESS(hreadline,"using_history");
 return 1;
}
#else
static int readlineinit()
{
#if defined(USE_LINENOISE)
    add_history=linenoiseHistoryAdd;
    read_history=linenoiseHistoryLoad;
    write_history=linenoiseHistorySave;
    readline=linenoise;
    return 2;
#else
    return 0;
#endif
}
#endif
#endif

void rlexit(int c){	if(!hist&&histfile[0]) write_history(histfile);}

char* Jinput_rl(char* prompt)
{
	static char* line=0;
if(hist)
	{
		char* s;
		hist=0;
		histfile[0]=0;
		s=getenv("HOME");
		if(s)
		{
			strcpy(histfile,s);
			if(1==breadline){
			  strcat(histfile,"/.jhistory");
			  using_history();
			}else
			  strcat(histfile,"/.jshistory");
			read_history(histfile);
		}
	}
	if(line) free(line); /* free last input */
	line = readline(prompt);
	if(!line) return "2!:55''"; /* ^d eof */
	if(*line) add_history(line); 
	return line;
}
#endif

char* Jinput_stdio(char* prompt)
{
  if(_isatty(_fileno(stdin))){
	fputs(prompt,stdout);
	fflush(stdout); /* windows emacs */
  }
	if(!fgets(input, sizeof(input), stdin))
	{
#ifdef _WIN32
		/* ctrl+c gets here for win */
		if(!_isatty(_fileno(stdin))) return "2!:55''";
		fputs("\n",stdout);
		fflush(stdout);
		**adadbreak+=1;
#else
		/* unix eof without readline */
		return "2!:55''";
#endif
	}
	return input;
}

C* _stdcall Jinput(J jt,C* prompt){
#ifdef READLINE
    if(_isatty(_fileno(stdin))){
		return (C*)Jinput_rl((char*)prompt);
    } else 
#endif
	return (C*)Jinput_stdio((char*)prompt);
}

/* J calls for output */
void _stdcall Joutput(J jt,int type, C* s)
{
 if(MTYOEXIT==type)
 {
  jefree();
#ifdef READLINE
  rlexit((int)(intptr_t)s);
#endif
  exit((int)(intptr_t)s);
 }
 fputs((char*)s,stdout);
 fflush(stdout);
}

void addargv(int argc, char* argv[], char* d)
{
 C *p,*q; I i;

 p=(C*)d+strlen(d);
 for(i=0;i<argc;++i)
 {
  if(sizeof(input)<(100+strlen(d)+2*strlen(argv[i]))) exit(100);
  if(1==argc){*p++=',';*p++='<';}
  if(i)*p++=';';	
  *p++='\'';
  q=(C*)argv[i];
  while(*q)
  {
   *p++=*q++;
   if('\''==*(p-1))*p++='\'';
  }
  *p++='\'';
 } 
 *p=0;
}

J jt;

int main(int argc, char* argv[])
{
 void* callbacks[] ={Joutput,0,Jinput,0,(void*)SMCON}; int type; int flag=0; int forceavx=0;

 if(argc>=3&&!strcmp(argv[1],"-lib")&&'-'!=*(argv[2])) flag=1;
// something like the following to eventuaully allow autodetection
// else if(!flag&&argc>=2&&!strcmp(argv[1],"-avx")) forceavx=1;  // avx
// else if(!flag&&argc>=2&&!strcmp(argv[1],"-noavx")) forceavx=2;  // no avx
 jepath(argv[0],flag?argv[2]:"",forceavx);
 if(argc>=2&&(!strcmp(argv[1],"-lib")||forceavx)) // remove processed arg
 {
	 int i;
	 int n=(flag)?2:1;
	 for(i=1;i<argc-n;++i)
	 {
		 argv[i]=argv[i+n];
	 }
	 argc=argc-n;
 }

#if !defined(WIN32)
// set stack size to get limit error instead of crash
 struct rlimit lim;
 getrlimit(RLIMIT_STACK,&lim);
 lim.rlim_cur=0x10000000; // 0xc000000 12mb works, but let's be safe with 16mb
 setrlimit(RLIMIT_STACK,&lim);
#endif
#ifdef READLINE
 if(_isatty(_fileno(stdin)))
#ifndef __MACH__
 breadline=readlineinit();
#else
 breadline=1;
#endif
#endif

 jt=jeload(callbacks);
 if(!jt){char m[1000]; jefail(m), fputs(m,stderr); exit(1);}
 adadbreak=(char**)jt; // first address in jt is address of breakdata
#ifndef _MSC_VER
 if(2==breadline){
  struct sigaction sa;
  sa.sa_flags = 0;
  sa.sa_handler = sigint2;
  sigemptyset(&(sa.sa_mask));
  sigaddset(&(sa.sa_mask), SIGINT);
  sigaction(SIGINT, &sa, NULL);
 }else
#endif
  signal(SIGINT,sigint);
 
#ifdef READLINE
 rl_readline_name="jconsole"; /* argv[0] varies too much*/
#if defined(USE_LINENOISE)
 if(2==breadline)linenoiseSetMultiLine(1);
#endif
#endif

 if(argc==2&&!strcmp(argv[1],"-jprofile"))
	 type=3;
 else if(argc>2&&!strcmp(argv[1],"-jprofile"))
	 type=1;
 else
	 type=0;
 addargv(argc,argv,input+strlen(input));
#if !defined(READLINE) && defined(__MINGW32__)
  _setmode( _fileno( stdin ), _O_TEXT ); //readline filters '\r' (so does this)
#endif
 jefirst(type,input);
 while(1){jedo((char*)Jinput(jt,(C*)"   "));}
 jefree();
 return 0;
}
