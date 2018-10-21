/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
// utilities for JFE to load JE, initiallize, and run profile sentence
// JFEs are jconsole, jwdw, and jwdp
#define PLEN 1000 // path length
#ifdef _WIN32
 #include <windows.h>
 #define GETPROCADDRESS(h,p) GetProcAddress(h,p)
 #define JDLLNAME "j.dll"
 #define JNONAVXDLLNAME "j-nonavx.dll"
 #define filesep '\\'
 #define filesepx "\\"
 #ifdef _MSC_VER
 #define strcasecmp _stricmp
 #endif
#else
 #include <unistd.h>
 #include <dlfcn.h>
 #define GETPROCADDRESS(h,p)	dlsym(h,p)
 #define _stdcall
 #define filesep '/'
 #define filesepx "/"
 #ifdef __MACH__
  extern int _NSGetExecutablePath(char*, int*);
  #define JDLLNAME "libj.dylib"
  #define JNONAVXDLLNAME "libj-nonavx.dylib"
 #else
  #include <sys/utsname.h>
  #define JDLLNAME "libj.so"
  #define JNONAVXDLLNAME "libj-nonavx.so"
 #endif
#endif
#include "j.h"
#include "jversion.h"
#include <stdint.h>
#if !(defined(_M_X64) || defined(__x86_64__))
#undef JNONAVXDLLNAME
#define JNONAVXDLLNAME JDLLNAME
#endif

static void* hjdll;
static J jt;
static JDoType jdo;
static JFreeType jfree;
static JgaType jga;
static JGetLocaleType jgetlocale;
static JGetAType jgeta;
static JSetAType jseta;
char path[PLEN];
char pathdll[PLEN];
static char jdllver[20];
static int FHS=0;
#if !(defined(_M_X64) || defined(__x86_64__))
static int AVX=0;
#else
static int AVX=1;
#endif
#ifdef ANDROID
static char install[PLEN];
#endif

int jedo(char* sentence)
{
	return jdo(jt,(C*)sentence);
}

A jegeta(I n, char* s){return jgeta(jt,n,(C*)s);}
I jeseta(I n,char* name,I x,char* d){return jseta(jt,n,(C*)name,x,(C*)d);}
void jefree(){jfree(jt);}
char* jegetlocale(){return (char*)jgetlocale(jt);}
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
 jgeta=(JGetAType)GETPROCADDRESS(hjdll,"JGetA");
 jseta=(JSetAType)GETPROCADDRESS(hjdll,"JSetA");
 return jt;
}

// set path and pathdll (wpath also set for win)
// WIN arg is 0, Unix arg is argv[0]
void jepath(char* arg,char* lib,int forceavx)
{
#ifdef _WIN32
 WCHAR wpath[PLEN];
 GetModuleFileNameW(0,wpath,_MAX_PATH);
 *(wcsrchr(wpath, '\\')) = 0;
 WideCharToMultiByte(CP_UTF8,0,wpath,1+(int)wcslen(wpath),path,PLEN,0,0);
#if SY_64
 char *jeavx=getenv("JEAVX");
 if (forceavx==1) AVX=1;       // force enable avx
 else if (forceavx==2) AVX=0;  // force disable avx
 else if (jeavx&&!strcasecmp(jeavx,"avx")) AVX=1;
 else if (jeavx&&!strcasecmp(jeavx,"noavx")) AVX=0;
 else { // auto detect
#if 0
//  AVX= 0!=(0x4UL & GetEnabledXStateFeatures());
// above line not worked for pre WIN7 SP1
// Working with XState Context (Windows)
// https://msdn.microsoft.com/en-us/library/windows/desktop/hh134240(v=vs.85).aspx
// Windows 7 SP1 is the first version of Windows to support the AVX API.
 #define XSTATE_MASK_AVX   (XSTATE_MASK_GSSE)
 typedef DWORD64 (WINAPI *GETENABLEDXSTATEFEATURES)();
 GETENABLEDXSTATEFEATURES pfnGetEnabledXStateFeatures = NULL;
 // Get the addresses of the AVX XState functions.
 HMODULE hm = GetModuleHandleA("kernel32.dll");
 if ((pfnGetEnabledXStateFeatures = (GETENABLEDXSTATEFEATURES)GetProcAddress(hm, "GetEnabledXStateFeatures")) &&
     ((pfnGetEnabledXStateFeatures() & XSTATE_MASK_AVX) != 0))
  AVX=1;
 FreeLibrary(hm);
#endif
 }
#endif
#elif defined(ANDROID)
#define AndroidPackage "com.jsoftware.j.android"
 struct stat st; int qsdcard; char tmp[PLEN];
 strcpy(path,"/data/data/");
 strcat(path,AndroidPackage);
 strcpy(pathdll,path);
 strcat(pathdll,"/lib/");
 strcat(pathdll,(AVX)?JDLLNAME:JNONAVXDLLNAME);
 if(stat(pathdll,&st)){ /* android 5 or newer */
#if defined(__aarch64__)
#define arch "arm64"
#elif defined(__x86_64__)
#define arch "x86_64"
#elif defined(__i386__)
#define arch "x86"
#else
#define arch "arm"
#endif
 int i;
 for(i=0;i<10;i++){
  if(i)
   sprintf(pathdll,"/data/app/%s-%d/lib/%s/%s",AndroidPackage,i,arch,(AVX)?JDLLNAME:JNONAVXDLLNAME);
  else
   sprintf(pathdll,"/data/app/%s/lib/%s/%s",AndroidPackage,arch,(AVX)?JDLLNAME:JNONAVXDLLNAME);
  if(!stat(pathdll,&st))break;
 }
 }
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
#if defined(__x86_64__)
// http://en.wikipedia.org/wiki/Advanced_Vector_Extensions
// Linux: supported since kernel version 2.6.30 released on June 9, 2009.
 char *jeavx=getenv("JEAVX");
 if (forceavx==1) AVX=1;       // force enable avx
 else if (forceavx==2) AVX=0;  // force disable avx
 else if (jeavx&&!strcasecmp(jeavx,"avx")) AVX=1;
 else if (jeavx&&!strcasecmp(jeavx,"noavx")) AVX=0;
 else { // auto detect by uname -r
#if 0
 struct utsname unm;
 if (!uname(&unm) &&
     ((unm.release[0]>'2'&&unm.release[0]<='9')||  // avoid sign/unsigned char difference
      (strlen(unm.release)>5&&unm.release[0]=='2'&&unm.release[2]=='6'&&unm.release[4]=='3'&&
       (unm.release[5]>='0'&&unm.release[5]<='9'))))
  AVX= 0!= __builtin_cpu_supports("avx");
// fprintf(stderr,"kernel release :%s:\n",unm.release);
#endif
 }
#endif
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
 strcat(pathdll,(AVX)?JDLLNAME:JNONAVXDLLNAME);
#ifndef _WIN32
 struct stat st;
 char pathdllpx[10];
 strncpy(pathdllpx,pathdll,10); pathdllpx[9]=0;
 if(stat(pathdll,&st)&&!strcmp(pathdllpx,"/usr/bin/")) FHS=1;
 if (FHS) {
  char _jdllver[20];
  strcpy(_jdllver,jversion);
  jdllver[0]=_jdllver[0];
  jdllver[1]='.';
  strcat(jdllver+2,_jdllver+1);
  strcpy(pathdll,(AVX)?JDLLNAME:JNONAVXDLLNAME);
  strcat(pathdll,".");
  strcat(pathdll,jdllver);
 }
#endif
 if(*lib)
 {
	 if(filesep==*lib || ('\\'==filesep && ':'==lib[1]))
		 strcpy(pathdll,lib); // absolute path
	 else
	 {
		 strcpy(pathdll,path);
		 strcat(pathdll,filesepx);
		 strcat(pathdll,lib); // relative path
	 }
 }
#endif
}

// called by jwdp (java jnative.c) to set path
void jesetpath(char* arg)
{
	strcpy(pathdll,arg); // jwdp gives path to j.dll
	strcpy(path,arg);
	if(strrchr(path,filesep))*(strrchr(path,filesep)) = 0;
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
		strcat(input,"");
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
#if defined(_WIN32)
	strcat(input,"[UNAME_z_=:'Win'");
#elif defined(__MACH__)
	strcat(input,"[UNAME_z_=:'Darwin'");
#elif !defined(ANDROID)
	strcat(input,"[UNAME_z_=:'Linux'");
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

	strcat(input,"[LIBFILE_z_=:'");
	p=pathdll;
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
	strcat(msg," failed.\n");
}
