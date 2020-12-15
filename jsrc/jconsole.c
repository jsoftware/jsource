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
#include <locale.h>
#ifdef __MACH__
#include <xlocale.h>
#endif

#include "j.h"
#include "jeload.h"

#define J_STACK  0x1000000uL // 16mb

static int forceprmpt=0;   /* emit prompt even if isatty is false */
static int breadline=0;    /* 0: none  1: libedit  2: linenoise */
static int norl=0;         /* disable readline/linenoise */
static char **adadbreak;
static void sigint(int k){**adadbreak+=1;signal(SIGINT,sigint);}
static void sigint2(int k){**adadbreak+=1;}
static char input[30000];

#if defined(ANDROID) || defined(_WIN32)
#undef USE_LINENOISE
#ifdef READLINE
#define USE_LINENOISE
#endif
#endif
/* J calls for keyboard input (debug suspension and 1!:1[1) */
/* we call to get next input */
#ifdef READLINE
/* readlin.h */
/* if not working properly, export TERM=dumb */
#if defined(USE_LINENOISE)
#include "linenoise.h"
#endif
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
char* rl_readline_name;

int hist=1;
char histfile[512];

#if !defined(ANDROID) && !defined(_WIN32)
static int readlineinit()
{
 if(hreadline)return 0; // already run
#ifndef __MACH__
 if(!(hreadline=dlopen("libedit.so.3",RTLD_LAZY)))
 if(!(hreadline=dlopen("libedit.so.2",RTLD_LAZY)))
  if(!(hreadline=dlopen("libedit.so.1",RTLD_LAZY)))
   if(!(hreadline=dlopen("libedit.so.0",RTLD_LAZY))){
#else
 if(!(hreadline=dlopen("libedit.dylib",RTLD_LAZY))){
#endif
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

void rlexit(int c){	if(!hist&&histfile[0]) write_history(histfile);}

char* Jinput_rl(char* prompt)
{
	static char* line=0;
if(hist)
	{
		char* s;
		hist=0;
		histfile[0]=0;
#ifdef _WIN32
		s=getenv("USERPROFILE");
#else
		s=getenv("HOME");
#endif
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
  if(prompt&&strlen(prompt)){
	fputs(prompt,stdout);
	fflush(stdout); /* windows emacs */
  }
	if(!fgets(input, sizeof(input), stdin))
	{
#ifdef _WIN32
		/* ctrl+c gets here for win */
		if(!(forceprmpt||_isatty(_fileno(stdin)))) return "2!:55''";
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

C* _stdcall Jinput(JST* jt,C* prompt){
#ifdef READLINE
    if(!norl&&_isatty(_fileno(stdin))){
		return (C*)Jinput_rl((char*)prompt);
    } else 
#endif
	return (C*)Jinput_stdio((char*)prompt);
}

/* J calls for output */
void _stdcall Joutput(JST* jt,int type, C* s)
{
 if(MTYOEXIT==type)
 {
  jefree();
#ifdef READLINE
  if(!norl)rlexit((int)(intptr_t)s);
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

JST* jt;

int main(int argc, char* argv[])
{
 setlocale(LC_ALL, "");
#if !(defined(ANDROID)||defined(_WIN32))
 locale_t loc;
 if ((loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0))) uselocale(loc);
#else
 setlocale(LC_NUMERIC,"C");
#endif
 void* callbacks[] ={Joutput,0,Jinput,0,(void*)SMCON}; int type;
 int i,poslib=0,poslibpath=0,posnorl=0,posprmpt=0; // assume all absent
 for(i=1;i<argc;i++){
  if(!poslib&&!strcmp(argv[i],"-lib")){poslib=i; if((i<argc-1)&&('-'!=*(argv[i+1])))poslibpath=i+1;}
  else if(!posnorl&&!strcmp(argv[i],"-norl")) {posnorl=i; norl=1;}
  else if(!posprmpt&&!strcmp(argv[i],"-prompt")) {posprmpt=i; forceprmpt=1;}
 }
// fprintf(stderr,"poslib %d,poslibpath %d,posnorl %d,posprmpt %d\n",poslib,poslibpath,posnorl,posprmpt);
 jepath(argv[0],(poslibpath)?argv[poslibpath]:"");
 // remove processed arg
 if(poslib||poslibpath||posnorl||posprmpt){
  int j=0; 
  char **argvv = malloc(argc*sizeof(char*));
  argvv[j++]=argv[0];
  for(i=1;i<argc;i++){
   if(!(i==poslib||i==poslibpath||i==posnorl||i==posprmpt))argvv[j++]=argv[i];
  }
  argc=j;
  for(i=1;i<argc;++i)argv[i]=argvv[i];
  free(argvv);
 }

#if !defined(WIN32)
// set stack size to get limit error instead of crash
 struct rlimit lim;
 if(!getrlimit(RLIMIT_STACK,&lim)){
  if(lim.rlim_cur!=RLIM_INFINITY && lim.rlim_cur<J_STACK){
   lim.rlim_cur=(lim.rlim_max==RLIM_INFINITY)?J_STACK:(lim.rlim_max<J_STACK)?lim.rlim_max:J_STACK;
   setrlimit(RLIMIT_STACK,&lim);
  }
 }
#endif
#ifdef READLINE
  norl|=!_isatty(_fileno(stdin));    // readline works on tty only
#if defined(USE_LINENOISE)
  if(!norl){
   char *term;
   term=getenv("TERM");
   if(term){
    static const char *unsupported_term[] = {"dumb","cons25","emacs",NULL};
    int j;
    for(j=0; unsupported_term[j]; j++)
     if (strcmp(term, unsupported_term[j]) == 0) {norl=1; break; }
   }
  }
#ifdef _WIN32
  if(!norl) norl|=!!getenv("SHELL");  // only works on real windows terminals
#endif
#endif
  if(!norl&&_isatty(_fileno(stdin)))
   breadline=readlineinit();
#endif

 jt=jeload(callbacks);
 if(!jt){char m[1000]; jefail(m); fputs(m,stderr); exit(1);}
 adadbreak=(char**)jt; // first address in jt is address of breakdata
#ifndef _WIN32
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
 if(!norl){
 rl_readline_name="jconsole"; /* argv[0] varies too much*/
#if defined(USE_LINENOISE)
 if(2==breadline)linenoiseSetMultiLine(1);
#endif
 }
#endif

 if(argc==2&&!strcmp(argv[1],"-jprofile"))
	 type=3;
 else if(argc>2&&!strcmp(argv[1],"-jprofile"))
	 type=1;
 else
	 type=0;
 addargv(argc,argv,input+strlen(input));
#if !defined(READLINE) && defined(__MINGW32__)
  if(!norl)
  _setmode( _fileno( stdin ), _O_TEXT ); //readline filters '\r' (so does this)
#endif
 jefirst(type,input);
 while(1){jedo((char*)Jinput(jt,(forceprmpt||_isatty(_fileno(stdin)))?(C*)"   ":(C*)""));}
 jefree();
#if !(defined(ANDROID)||defined(_WIN32))
 if(loc)freelocale(loc);
#endif
 return 0;
}
