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
#ifdef __APPLE__
#include <xlocale.h>
#endif

#include "j.h"
#include "jeload.h"

#if !defined(_WIN32) && !defined(__OpenBSD__) && !defined(__FreeBSD__) //temporary
#include "../libbacktrace/backtrace.h"
#endif

#define J_STACK  0xc00000uL // 12mb

static int runjscript=0;   /* exit after running script */
static int forceprmpt=0;   /* emit prompt even if isatty is false */
static int breadline=0;    /* 0: none  1: libedit  2: linenoise */
static int norl=0;         /* disable readline/linenoise */
static void sigint(int k){jeinterrupt();signal(SIGINT,sigint);}
static void sigint2(int k){jeinterrupt();}
#if defined(_WIN32)
static BOOL WINAPI CtrlHandler(DWORD fdwCtrlType){
 switch (fdwCtrlType){
// Handle the CTRL-C signal.
  case CTRL_C_EVENT:
   jeinterrupt(); return TRUE;
  default: return FALSE;
 }
}
#endif
#if !defined(_WIN32) && !defined(__OpenBSD__) && !defined(__FreeBSD__) //temporary
static int err_write(void *data, uintptr_t pc, const char *file, int line, const char *function){
 char buf[512];
 file = file ? file : "?";
 while(!strncmp(file, "../", 3)) file += 3; // strip leading '../'.  Don't strip leading 'jsrc/' to avoid ambiguity with source files with other origins.
 snprintf(buf, sizeof(buf), "%0*lx: %s:%d:\t%s\n", BW==64?16:8, (unsigned long)pc, file, line, function ? function : "?");
 (void)!write(STDERR_FILENO, buf, strlen(buf));
 R 0;}
static void err_error(void *data, const char *msg, int errnum) {
 char buf[512];
 snprintf(buf, sizeof(buf), "Could not generate stack trace: %s (%d)\n", msg, errnum);
 (void)!write(STDERR_FILENO, buf, strlen(buf));
}
static void sigsegv(int k){
 //todo should say to report to the beta forums for beta builds
 const char msg[] = "JE has crashed, likely due to an internal bug.  Please report the code which caused the crash, as well as the following printout, to the J programming forum.\n";
 // write is async-signal safe; fwrite&co are not, but still do this, just to be safe
 // similarly, can't fflush(stderr) first; too bad
 (void)!write(STDERR_FILENO, msg, sizeof(msg)-1);
 struct backtrace_state *state = backtrace_create_state(NULL, 1, NULL, NULL);
 if(state)backtrace_full(state, 0, err_write, err_error, NULL);
 const char line[] = "-----------------------------------------------------------------------------\n";
 (void)!write(STDERR_FILENO, line, 78);
 fsync(STDERR_FILENO);
 //abort rather than exit to ensure a core dump is still generated
 abort();}
#endif

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
 static const char *libedit[] = {
#ifndef __APPLE__
  "libedit.so.5.2","libedit.so.3","libedit.so.2","libedit.so.1","libedit.so.0","libedit.so",
#else
  "libedit.dylib",
#endif
  0};
 for(const char **l = libedit;*l;l++) if((hreadline=dlopen(*l,RTLD_LAZY))) goto ledit;
 // couldn't find libedit
#if defined(USE_LINENOISE)
 add_history=linenoiseHistoryAdd;
 read_history=linenoiseHistoryLoad;
 write_history=linenoiseHistorySave;
 readline=linenoise;
 return 2;
#else
 return 0;
#endif
ledit:
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
  if(!runjscript&&prompt&&strlen(prompt)){
	fputs(prompt,stdout);
	fflush(stdout); /* windows emacs */
  }
	while(!fgets(input, sizeof(input), stdin))  // loop if there is an error.  If it's ctrl-c, read again; otherwise terminate
	{
#ifdef _WIN32
		/* ctrl+c gets here for win */
		if(!(forceprmpt||_isatty(_fileno(stdin)))) return "2!:55''";
		fputs("\n",stdout);
		fflush(stdout);
		jeinterrupt();
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
 if((2==type)||(4==type)){
 fputs((char*)s,stderr);
 fflush(stderr);
 }else{
 fputs((char*)s,stdout);
 fflush(stdout);
 }
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
#if !defined(_WIN32) && !defined(__OpenBSD__) && !defined(__FreeBSD__) //temporary
 signal(SIGSEGV,sigsegv);
 signal(SIGILL,sigsegv);
#ifdef __APPLE__
 signal(SIGTRAP,sigsegv); //catch SEGFAULT
#endif
#endif
 setlocale(LC_ALL, "");
#if !(defined(ANDROID)||defined(_WIN32))
 locale_t loc;
 if ((loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0))) uselocale(loc);
#else
 setlocale(LC_NUMERIC,"C");
#endif
 void* callbacks[] ={Joutput,0,Jinput,0,(void*)SMCON}; int type;
 int i,poslib=0,poslibpath=0,posnorl=0,posprmpt=0,posscrpt=0; // assume all absent
 for(i=1;i<argc;i++){
  if(!poslib&&!strcmp(argv[i],"-lib")){poslib=i; if((i<argc-1)&&('-'!=*(argv[i+1])))poslibpath=i+1;}
  else if(!posnorl&&!strcmp(argv[i],"-norl")) {posnorl=i; norl=1;}
  else if(!posprmpt&&!strcmp(argv[i],"-prompt")) {posprmpt=i; forceprmpt=1;}
  else if(!posscrpt&&!strcmp(argv[i],"-jscript")) {posscrpt=i; runjscript=1; norl=1; forceprmpt=0;}
 }
// fprintf(stderr,"poslib %d,poslibpath %d,posnorl %d,posprmpt %d,posscrpt %d\n",poslib,poslibpath,posnorl,posprmpt,posscrpt);
 jepath(argv[0],(poslibpath)?argv[poslibpath]:"");
 // remove processed arg
 if(poslib||poslibpath||posnorl||posprmpt||posscrpt){
  int j=0; 
  char **argvv = malloc(argc*sizeof(char*));
  argvv[j++]=argv[0];
  for(i=1;i<argc;i++){
   if(!(i==poslib||i==poslibpath||i==posnorl||i==posprmpt||i==posscrpt))argvv[j++]=argv[i];
  }
  argc=j;
  for(i=1;i<argc;++i)argv[i]=argvv[i];
  free(argvv);
 }

#if 0
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
  if(!norl&&_isatty(_fileno(stdin))){
   breadline=readlineinit();
   dlerror(); //clear error
  }
#endif

#ifdef JAMALGAM
#ifdef _WIN32
 extern int attach_process();
 attach_process();
#endif
#endif
 jt=jeload(callbacks);
 if(!jt){char m[1000]; jefail(m); fputs(m,stderr); exit(1);}
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
#if defined(_WIN32)
  SetConsoleCtrlHandler(CtrlHandler, TRUE);
#else
  signal(SIGINT,sigint);
#endif
 
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
 if(runjscript)type|=256;
 int r=jefirst(type,input);
 if(!runjscript)while(1){r=jedo((char*)Jinput(jt,(forceprmpt||_isatty(_fileno(stdin)))?(C*)"   ":(C*)""));}
 jefree();
#if !(defined(ANDROID)||defined(_WIN32))
 if(loc)freelocale(loc);
#endif
 return r;
}
