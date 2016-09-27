/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
// utilities for JFE to load JE, initiallize, and run profile sentence
// JFEs are jconsole, jwdw, and jwdp
#define PLEN 1000 // path length
#ifdef _WIN32
 #include <windows.h>

#ifdef UNDER_CE
 #define GETPROCADDRESS(h,p) GetProcAddress(h,_T(p))
#else
 #define GETPROCADDRESS(h,p) GetProcAddress(h,p)
#endif
 #define JDLLNAME "j.dll"
 #define filesep '\\'
 #define filesepx "\\"
// setfocus e required for pocketpc and doesn't hurt others
#define ijx "11!:0'pc ijx closeok;xywh 0 0 300 200;cc e editijx rightmove bottommove ws_vscroll ws_hscroll;setfont e \"Courier New\" 12;setfocus e;pas 0 0;pgroup jijx;pshow;'[18!:4<'base'"

#else
 #include <unistd.h>
 #include <dlfcn.h>
 #define GETPROCADDRESS(h,p)	dlsym(h,p)
 #define _stdcall
 #define filesep '/'
 #define filesepx "/"
 #define ijx "11!:0'pc ijx closeok;xywh 0 0 300 200;cc e editijx rightmove bottommove ws_vscroll ws_hscroll;setfont e monospaced 12;pas 0 0;pgroup jijx;pshow;'[18!:4<'base'"
 #ifdef __MACH__
  #define JDLLNAME "libj.dylib"
 #else
  #define JDLLNAME "libj.so"
 #endif
#endif
#include "j.h"
#include "jversion.h"
#include <stdint.h>

static void* hjdll;
static J jt;
static JDoType jdo;
static JFreeType jfree;
static JgaType jga;
static JGetLocaleType jgetlocale;
static char path[PLEN];
static char pathdll[PLEN];
static char jdllver[20];
static int FHS=0;
#ifdef ANDROID
static char install[PLEN];
#endif

int jedo(char* sentence)
{
	return jdo(jt,sentence);
}

void jefree(){jfree(jt);}
char* jegetlocale(){return jgetlocale(jt);}
A jega(I t, I n, I r, I*s){return jga(jt,t,n,r,s);}
void* jehjdll(){return hjdll;}

// load JE, Jinit, getprocaddresses, JSM
J jeload(void* callbacks)
{
#ifdef _WIN32
 WCHAR wpath[PLEN];
 MultiByteToWideChar(CP_UTF8,0,pathdll,1+(int)strlen(pathdll),wpath,PLEN);
 hjdll=LoadLibraryW(wpath);
#else
 hjdll=dlopen(pathdll,RTLD_LAZY);
#endif
 if(!hjdll)return 0;
 jt=((JInitType)GETPROCADDRESS(hjdll,"JInit"))();
 if(!jt) return 0;
 ((JSMType)GETPROCADDRESS(hjdll,"JSM"))(jt,callbacks);
 jdo=(JDoType)GETPROCADDRESS(hjdll,"JDo");
 jfree=(JFreeType)GETPROCADDRESS(hjdll,"JFree");
 jga=(JgaType)GETPROCADDRESS(hjdll,"Jga");
 jgetlocale=(JGetLocaleType)GETPROCADDRESS(hjdll,"JGetLocale");
 return jt;
}

// set path and pathdll (wpath also set for win)
// WIN arg is 0, Unix arg is argv[0]
void jepath(char* arg)
{
#ifdef _WIN32
 WCHAR wpath[PLEN];
 GetModuleFileNameW(0,wpath,_MAX_PATH);
 *(wcsrchr(wpath, '\\')) = 0;
 WideCharToMultiByte(CP_UTF8,0,wpath,1+(int)wcslen(wpath),path,PLEN,0,0);
#elif defined(ANDROID)
#define AndroidPackage "com.jsoftware.j.android"
 struct stat st; int qsdcard; char tmp[PLEN];
 strcpy(path,"/data/data/");
 strcat(path,AndroidPackage);
 strcpy(pathdll,path);
 strcat(pathdll,"/lib/");
 strcat(pathdll,JDLLNAME);
 strcpy(tmp, "/sdcard/Android/data");
 qsdcard=stat(tmp,&st);
 strcpy(install,(qsdcard)?"/storage/emulated/0/Android/data/":"/sdcard/Android/data/");
 strcat(install,AndroidPackage);
 strcat(install,"/files");
 setenv("HOME",(qsdcard)?"/storage/emulated/0":"/sdcard",1);
 if(!getenv("TMPDIR")) {
  strcpy(tmp, path);
  strcat(tmp, "/app_jandroid/tmp");
  if(stat(tmp,&st)) mkdir(tmp, S_IRWXU | S_IRWXG | S_IRWXO);
  chmod(tmp, S_IRWXU | S_IRWXG | S_IRWXO);
  setenv("TMPDIR",tmp,1);
 }
 chmod(getenv("TMPDIR"), S_IRWXU | S_IRWXG | S_IRWXO);
 strcat(path,"/app_jandroid/bin");
#else

#define sz 4000
 char arg2[sz],arg3[sz];
 char* src,*snk;int n,len=sz;
 // fprintf(stderr,"arg0 %s\n",arg);
 // try host dependent way to get path to executable
 // use arg if they fail (arg command in PATH won't work)
#ifdef __MACH__ 
 n=_NSGetExecutablePath(arg2,&len);
 if(0!=n) strcat(arg2,arg);
#else
 n=readlink("/proc/self/exe",arg2,sizeof(arg2));
 if(-1==n) strcpy(arg2,arg); else arg2[n]=0;
#endif
 // fprintf(stderr,"arg2 %s\n",arg2);
 // arg2 is path (abs or relative) to executable or soft link
 n=readlink(arg2,arg3,sz);
 if(-1==n) strcpy(arg3,arg2); else arg3[n]=0;
 // fprintf(stderr,"arg3 %s\n",arg3);
 if('/'==*arg3)
  strcpy(path,arg3);
 else
 {
  getcwd(path,sizeof(path));
  strcat(path,"/");
  strcat(path,arg3);
 }
 *(1+strrchr(path,'/'))=0;
 // remove ./ and backoff ../
 snk=src=path;
 while(*src)
 {
	 if('/'==*src&&'.'==*(1+src)&&'.'==*(2+src)&&'/'==*(3+src))
	 {
		 *snk=0;
		 snk=strrchr(path,'/');
		 snk=0==snk?path:snk;
		 src+=3;
	 }
	 else if('/'==*src&&'.'==*(1+src)&&'/'==*(2+src))
      src+=2;
	 else
	  *snk++=*src++;
 }
 *snk=0;
 snk=path+strlen(path)-1;
 if('/'==*snk) *snk=0;
#endif
#ifndef ANDROID
 strcpy(pathdll,path);
 strcat(pathdll,filesepx);
 strcat(pathdll,JDLLNAME);
#ifndef _WIN32
 struct stat st;
 if(stat(pathdll,&st)) FHS=1;
 if (FHS) {
  char _jdllver[20];
  strcpy(_jdllver,jversion);
  jdllver[0]=_jdllver[0];
  jdllver[1]='.';
  strcat(jdllver+2,_jdllver+1);
  strcpy(pathdll,JDLLNAME);
 }
#endif
#endif
 // fprintf(stderr,"arg4 %s\n",path);
}

// called by jwdp (java jnative.c) to set path
void jesetpath(char* arg)
{
	strcpy(pathdll,arg); // jwdp gives path to j.dll
	strcpy(path,arg);
	*(strrchr(path,filesep)) = 0;
}

// build and run first sentence to set BINPATH, ARGV, and run profile
// arg is command line ready to set in ARGV_z_
// type is 0 normal, 1 -jprofile xxx, 2 ijx basic, 3 nothing
// profile[ARGV_z_=:...[BINPATH=:....
// profile is from BINPATH, ARGV, ijx basic, or nothing
int jefirst(int type,char* arg)
{
	int r; char* p,*q;
	char* input=malloc(2000+strlen(arg));
#if 0
	char buf[50];
#endif
	*input=0;
	if(0==type)
	{
#ifdef ANDROID
		strcat(input,"(3 : '0!:0 y')<INSTALLROOT,'");
		strcat(input,"/bin");
#else
  if (!FHS)
		strcat(input,"(3 : '0!:0 y')<BINPATH,'");
  else {
		strcat(input,"(3 : '0!:0 y')<'/etc/j/");
		strcat(input,jdllver);
	}
#endif
		strcat(input,filesepx);
		strcat(input,"profile.ijs'");
	}
	else if(1==type)
		strcat(input,"(3 : '0!:0 y')2{ARGV");
	else if(2==type)
		strcat(input,ijx);
	else
		strcat(input,"i.0 0");
	strcat(input,"[ARGV_z_=:");
	strcat(input,arg);
#ifdef ANDROID
	strcat(input,"[UNAME_z_=:'Android'");
	strcat(input,"[INSTALLROOT_z_=:'");
	strcat(input,install);
	strcat(input,"'");
	strcat(input,"[AndroidPackage_z_=:'");
	strcat(input,AndroidPackage);
	strcat(input,"'");
#elif defined(RASPI)
	strcat(input,"[IFRASPI_z_=:1");
#endif
#if 0
	sprintf(buf,"(" FMTI ")",(I)(intptr_t)hjdll);
	strcat(input,"[HLIBJ_z_=:");
	strcat(input,buf);
#endif
	strcat(input,"[BINPATH_z_=:'");
	p=path;
	q=input+strlen(input);
	while(*p)
	{
		if(*p=='\'') *q++='\'';	// 's doubled
		*q++=*p++;
	}
	*q=0;
	strcat(input,"'");
	r=jedo(input);
	free(input);
	return r;
}

void jefail(char* msg)
{
	strcpy(msg, "Load library ");
	strcat(msg, pathdll);
	strcat(msg," failed.");
}
