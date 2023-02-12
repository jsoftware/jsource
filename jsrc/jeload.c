/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
// utilities for JFE to load JE, initiallize, and run profile sentence
// JFEs are jconsole, jwdw, and jwdp
#define PLEN 1000 // path length
#ifdef _WIN32
 #include <windows.h>
 #define GETPROCADDRESS(h,p) GetProcAddress(h,p)
 #define JDLLNAME "j"
 #define JDLLEXT ".dll"
 #define filesep '\\'
 #define filesepx "\\"
 #ifdef MMSC_VER
 #define strcasecmp _stricmp
 #endif
#else
 #include <unistd.h>
 #include <dlfcn.h>
 #define GETPROCADDRESS(h,p)	dlsym(h,p)
 #define _stdcall
 #define filesep '/'
 #define filesepx "/"
 #define JDLLNAME "libj"
 #ifdef __APPLE__
  extern int _NSGetExecutablePath(char*, int*);
  #define JDLLEXT ".dylib"
 #else
  #include <sys/utsname.h>
  #define JDLLEXT ".so"
 #endif
#endif
#include "j.h"
#include "jversion.h"
#include <stdint.h>

static void* hjdll;
static JST* jt;
static JDoType jdo;
static JInterruptType jinterrupt;
static JFreeType jfree;
static JgaType jga;
static JGetLocaleType jgetlocale;
static JGetAType jgeta;
static JSetAType jseta;
char path[PLEN];
char libpathj[PLEN];
char pathdll[PLEN];
static char pathetcpx[PLEN];
static char pathexec0[PLEN];
static char pathexec[PLEN];
static char jdllver[20];
static int FHS=0;
#ifdef ANDROID
#include <sys/system_properties.h>
#include <android/log.h>
static char install[PLEN];

#if __ANDROID_API__ >= 21
// Android 'L' makes __system_property_get a non-global symbol.
// Here we provide a stub which loads the symbol from libc via dlsym.
typedef int (*PFN_SYSTEM_PROP_GET)(const char *, char *);
static int _system_property_get(const char* name, char* value)
{
    static PFN_SYSTEM_PROP_GET _real_system_property_get = NULL;
    if (!_real_system_property_get) {
        // libc.so should already be open, get a handle to it.
        void *handle = dlopen("libc.so", RTLD_NOLOAD);
        if (!handle) {
            __android_log_print(ANDROID_LOG_ERROR, "foobar", "Cannot dlopen libc.so: %s.\n", dlerror());
        } else {
            _real_system_property_get = (PFN_SYSTEM_PROP_GET)dlsym(handle, "__system_property_get");
        }
        if (!_real_system_property_get) {
            __android_log_print(ANDROID_LOG_ERROR, "foobar", "Cannot resolve __system_property_get(): %s.\n", dlerror());
        }
    }
    return (*_real_system_property_get)(name, value);
}
#else
#define _system_property_get __system_property_get
#endif
#endif

int jedo(char* sentence)
{
	return jdo(jt,(C*)sentence);
}

void jeinterrupt()
{
 jinterrupt(jt);
}

A jegeta(I n, char* s){return jgeta(jt,n,(C*)s);}
I jeseta(I n,char* name,I x,char* d){return jseta(jt,n,(C*)name,x,(C*)d);}
void jefree(){jfree(jt);}
char* jegetlocale(){return (char*)jgetlocale(jt);}
A jega(I t, I n, I r, I*s){return jga(jt,t,n,r,s);}
void* jehjdll(){return hjdll;}

// load JE, Jinit, getprocaddresses, JSM
JST* jeload(void* callbacks)
{
#ifdef _WIN32
 WCHAR wpath[PLEN];
 MultiByteToWideChar(CP_UTF8,0,pathdll,1+(int)strlen(pathdll),wpath,PLEN);
 hjdll=LoadLibraryW(wpath);
#else
 hjdll=dlopen(pathdll,RTLD_LAZY);
#endif
 if(!hjdll)return 0;
// jt=((JInit2Type)GETPROCADDRESS(hjdll,"JInit2"))((C*)libpathj);
 jt=((JInitType)GETPROCADDRESS(hjdll,"JInit"))();
 if(!jt) return 0;
 ((JSMType)GETPROCADDRESS(hjdll,"JSM"))(jt,callbacks);
 jdo=(JDoType)GETPROCADDRESS(hjdll,"JDo");
 jinterrupt=(JInterruptType)GETPROCADDRESS(hjdll,"JInterrupt");
 jfree=(JFreeType)GETPROCADDRESS(hjdll,"JFree");
 jga=(JgaType)GETPROCADDRESS(hjdll,"Jga");
 jgetlocale=(JGetLocaleType)GETPROCADDRESS(hjdll,"JGetLocale");
 jgeta=(JGetAType)GETPROCADDRESS(hjdll,"JGetA");
 jseta=(JSetAType)GETPROCADDRESS(hjdll,"JSetA");
 return jt;
}

// set path and pathdll (wpath also set for win)
// WIN arg is 0, Unix arg is argv[0]
void jepath(char* arg,char* lib)
{
#ifndef _WIN32
 struct stat st;
#endif
#ifdef _WIN32
 WCHAR wpath[PLEN];
 GetModuleFileNameW(0,wpath,_MAX_PATH);
 *(wcsrchr(wpath, '\\')) = 0;
 WideCharToMultiByte(CP_UTF8,0,wpath,1+(int)wcslen(wpath),path,PLEN,0,0);
 strcpy(libpathj,path);
#elif defined(ANDROID)
#define AndroidPackage "com.jsoftware.j.android"
 char tmp[PLEN];
 strcpy(path,"/data/data/");
 strcat(path,AndroidPackage);
 strcpy(pathdll,path);
 strcat(pathdll,"/lib/");
 strcpy(libpathj,pathdll);
 strcat(pathdll,JDLLNAME);
 strcat(pathdll,JDLLEXT);
 if(stat(path,&st)){ /* android 5 or newer */
 strcpy(path,"/data/user/0/");
 strcat(path,AndroidPackage);
 }
 if(stat(pathdll,&st)){ /* android 4 or newer */
#if defined(__aarch64__)||defined(_M_ARM64)
#define arch "arm64"
#elif defined(__x86_64__)
#define arch "x86_64"
#elif defined(__i386__)
#define arch "x86"
#else
#define arch "arm"
#endif
 int i;
 for(i=0;i<20;i++){
  if(i)
   sprintf(pathdll,"/data/app/%s-%d/lib/%s/%s.%s",AndroidPackage,i,arch,JDLLNAME,JDLLEXT);
  else
   sprintf(pathdll,"/data/app/%s/lib/%s/%s.%s",AndroidPackage,arch,JDLLNAME,JDLLEXT);
  if(!stat(pathdll,&st))break;
  if(i)
   sprintf(pathdll,"/data/app-lib/%s-%d/%s.%s",AndroidPackage,i,JDLLNAME,JDLLEXT);
  else
   sprintf(pathdll,"/data/app-lib/%s/%s.%s",AndroidPackage,JDLLNAME,JDLLEXT);
  if(!stat(pathdll,&st))break;
  if(i)
   sprintf(pathdll,"/mnt/asec/%s-%d/lib/%s.%s",AndroidPackage,i,JDLLNAME,JDLLEXT);
  else
   sprintf(pathdll,"/mnt/asec/%s/lib/%s.%s",AndroidPackage,JDLLNAME,JDLLEXT);
  if(!stat(pathdll,&st))break;
 }
 }
 strcpy(tmp, "/mnt/sdcard/Android/data");
 strcpy(install,(stat(tmp,&st))?((stat(tmp+4,&st))?"/storage/emulated/0/Android/data/":"/sdcard/Android/data/"):"/mnt/sdcard/Android/data/");
 strcat(install,AndroidPackage);
 strcat(install,"/files");
 setenv("HOME",install,1);
 if(!getenv("TMPDIR")) {
  strcpy(tmp, path);
  strcat(tmp, "/app_jandroid/tmp");
  if(stat(tmp,&st)) mkdir(tmp, S_IRWXU | S_IRWXG | S_IRWXO);
  chmod(tmp, S_IRWXU | S_IRWXG | S_IRWXO);
  setenv("TMPDIR",tmp,1);
 }
 chmod(getenv("TMPDIR"), S_IRWXU | S_IRWXG | S_IRWXO);
 strcat(path,"/app_jandroid/bin");
#endif

#ifndef _WIN32
#define sz 4000
 char arg2[sz],arg3[sz];
 char* src,*snk;int n,len=sz;
 // fprintf(stderr,"arg0 %s\n",arg);
 // try host dependent way to get path to executable
 // use arg if they fail (arg command in PATH won't work)
#ifdef __APPLE__ 
 n=_NSGetExecutablePath(arg2,&len);
 if(0!=n) strcat(arg2,arg);
#else
 n=readlink("/proc/self/exe",arg2,sizeof(arg2));
 if(-1==n) strcpy(arg2,arg); else arg2[n]=0;
#endif
 strcpy(pathexec0,arg2);
 // fprintf(stderr,"arg2 %s\n",arg2);
 // arg2 is path (abs or relative) to executable or soft link
 n=readlink(arg2,arg3,sz);
 if(-1==n) strcpy(arg3,arg2); else arg3[n]=0;
 // fprintf(stderr,"arg3 %s\n",arg3);
 strcpy(pathexec,arg3);
 if('/'==*arg3)
  strcpy(path,arg3);
 else
 {
  if(!getcwd(path,sizeof(path)))path[0]=0;
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
#ifdef ANDROID
 strcpy(tmp,pathdll);
#endif
 strcpy(pathdll,path);
 strcpy(libpathj,pathdll);
 strcat(pathdll,filesepx);
 strcat(pathdll,JDLLNAME);
 strcat(pathdll,JDLLEXT);
#ifdef ANDROID
 if(stat(pathdll,&st))strcpy(pathdll,tmp);
#endif
#if !defined(_WIN32) && !defined(ANDROID)
 if(stat(pathdll,&st)||strncmp(pathexec0,"/usr/bin/",9)||strncmp(pathexec0,"/usr/local/bin/",15)||strncmp(pathexec0,"/opt/homebrew/bin/",18)){
 char pathpx[PLEN];
 if('/'==*pathexec){
 if(!strncmp(pathexec,"/opt/homebrew/bin/",strlen("/opt/homebrew/bin/"))){FHS=1; strcat(pathetcpx,"/opt/homebrew");}
 else if(!strncmp(pathexec,"/usr/local/bin/",strlen("/usr/local/bin/"))){FHS=1; strcat(pathetcpx,"/usr/local");}
 else if(!strncmp(pathexec,"/usr/bin/",strlen("/usr/bin/"))){FHS=1; pathetcpx[0]=0; }
 }else{
 strcpy(pathpx,"/opt/homebrew/bin/"); strcat(pathpx,pathexec); if(!stat(pathpx,&st)){FHS=1; strcat(pathetcpx,"/opt/homebrew"); }
 else {strcpy(pathpx,"/usr/local/bin/"); strcat(pathpx,pathexec); if(!stat(pathpx,&st)){FHS=1; strcat(pathetcpx,"/usr/local"); }
 else {strcpy(pathpx,"/usr/bin/"); strcat(pathpx,pathexec); if(!stat(pathpx,&st)){FHS=1; pathetcpx[0]=0; }
 }}
 }
 if (FHS) {
  char *jv1;
// jversion   "9.4.0-betaX"
  if (jv1=strchr(jversion,'.')) if (jv1=strchr(jv1+1,'.')){ memcpy(jdllver,jversion,jv1-(jversion));jdllver[jv1-jversion]=0; }
  if (!jv1) strcpy(jdllver,"9.4");
  if (5<strlen(jdllver)) strcpy(jdllver,"9.4");
  strcpy(pathdll,JDLLNAME);
#if defined(__APPLE__)
  strcat(pathdll,".");
  strcat(pathdll,jdllver);
  strcat(pathdll,JDLLEXT);
#else
  strcat(pathdll,JDLLEXT);
  strcat(pathdll,".");
  strcat(pathdll,jdllver);
#endif
 }
 }
#endif
 if(lib&&*lib)
 {
	 if(filesep==*lib || ('\\'==filesep && ':'==lib[1]))
		 strcpy(pathdll,lib); // absolute path
	 else
	 {
		 strcpy(pathdll,path);
		 strcat(pathdll,filesepx);
		 strcat(pathdll,lib); // relative path
	 }
#ifdef _WIN32
	 char *p1,*p2;
	 p1=strrchr(pathdll,filesep); p2=strrchr(pathdll,'/');
	 if(p1&&p2){strcpy(libpathj,pathdll);libpathj[((p1>p2)?p1:p2)-pathdll]=0;}
	 else if(p1){strcpy(libpathj,pathdll);libpathj[p1-pathdll]=0;}
	 else if(p2){strcpy(libpathj,pathdll);libpathj[p2-pathdll]=0;}
#else
	 char *p1;
	 if((p1=strrchr(pathdll,filesep))){strcpy(libpathj,pathdll);libpathj[p1-pathdll]=0;}
#endif
 }
}

// called by jwdp (java jnative.c) to set path
void jesetpath(char* arg)
{
	strcpy(pathdll,arg); // jwdp gives path to j.dll
	strcpy(path,arg);
	if(strrchr(path,filesep))*(strrchr(path,filesep)) = 0;
	strcpy(libpathj,path);
}

// build and run first sentence to set BINPATH, ARGV, and run profile
// arg is command line ready to set in ARGV_z_
// type is 0 normal, 1 -jprofile xxx, 2 ijx basic, 3 nothing
// type mask 256 runjscript
// profile[ARGV_z_=:...[BINPATH=:....
// profile is from BINPATH, ARGV, ijx basic, or nothing
int jefirst(int type,char* arg)
{
	int r; char* p,*q;
	char* input=malloc(2000+strlen(arg));
#if 0
	char buf[50];
#endif
	int runjscript=!!(type&256);
	type=type&255;
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
		strcat(input,"(3 : '0!:0 y')<'");
		strcat(input,pathetcpx);
		strcat(input,"/etc/j/");
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
  char propval[PROP_VALUE_MAX+1];
  if (_system_property_get("ro.build.version.sdk", propval)){
	strcat(input,"[APILEVEL_ja_=:");
	strcat(input,propval);
  }
  if (_system_property_get("ro.build.version.release", propval)){
	strcat(input,"[OSRELEASE_ja_=:'");
	strcat(input,propval);
	strcat(input,"'");
  }
	strcat(input,"[UNAME_z_=:'Android'");
	strcat(input,"[INSTALLROOT_z_=:'");
	strcat(input,install);
	strcat(input,"'");
	strcat(input,"[AndroidPackage_z_=:'");
	strcat(input,AndroidPackage);
	strcat(input,"'");
#endif
#if defined(RASPI)
	strcat(input,"[IFRASPI_z_=:1");
#else
	strcat(input,"[IFRASPI_z_=:0");
#endif
#if defined(_WIN32)
	strcat(input,"[UNAME_z_=:'Win'");
#elif defined(__APPLE__)
	strcat(input,"[UNAME_z_=:'Darwin'");
#elif defined(__FreeBSD__)
	strcat(input,"[UNAME_z_=:'FreeBSD'");
#elif defined(__OpenBSD__)
 strcat(input,"[UNAME_z_=:'OpenBSD'");
#elif !defined(ANDROID)
	strcat(input,"[UNAME_z_=:'Linux'");
#endif
	if(FHS) strcat(input,"[FHS_z_=:1");
	else strcat(input,"[FHS_z_=:0");
#if 0
	sprintf(buf,"(" FMTI ")",(I)(intptr_t)hjdll);
	strcat(input,"[HLIBJ_z_=:");
	strcat(input,buf);
#endif
	if(runjscript) strcat(input,"[RUNJSCRIPT_z_=:1");
	else strcat(input,"[RUNJSCRIPT_z_=:0");
	strcat(input,"[BINPATH_z_=:'");
	if(!FHS){
	p=path;
	q=input+strlen(input);
	while(*p)
	{
		if(*p=='\'') *q++='\'';	// 's doubled
		*q++=*p++;
	}
	*q=0;
	} else {
	if(0==*pathetcpx) strcat(input,"/usr/bin");
	else {strcat(input,pathetcpx); strcat(input,"/bin"); }
	}
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
	strcat(msg," failed: ");
#ifdef _WIN32
	char buf[256];
	FormatMessageA(
	 FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	 NULL, GetLastError(),
	 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  /* Default language */
	 buf, (sizeof(buf)/sizeof(char)), 0);
	strcat(msg,buf);
#else
	char *dlerr=dlerror();
	if(dlerr)strcat(msg,dlerr);
	else{
		char ermsg[1024];
		if(errno&&!strerror_r(errno,ermsg,1024))strcat(msg,ermsg);}
#endif
	strcat(msg,"\n");
}
