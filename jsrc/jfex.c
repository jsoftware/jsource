// J Front End Example
// define _WIN32 for Windows, __MACH__ for MAC, J64 for 64-bit
// JE is loaded from current working directory

#ifdef _WIN32
 #define _CRT_SECURE_NO_WARNINGS
 #include <windows.h>
 #include <direct.h>
 #define GETPROCADDRESS(h,p) GetProcAddress(h,p)
 #define JDLLNAME "\\j.dll"
#else
 #define _stdcall
 #include <dlfcn.h>
 #define GETPROCADDRESS(h,p)	dlsym(h,p)
 #ifdef __MACH__ 
  #define JDLLNAME "/libj.dylib"
 #else
  #define JDLLNAME "/libj.so"
 #endif
 #define _getcwd getcwd
#endif

#include <stdio.h>
#include <signal.h>
#include "jfex.h"
#include "jlib.h"

static JDoType jdo;
static JFreeType jfree;
static JgaType jga;
static JGetLocaleType jgetlocale;

static J jt;
static void* hjdll;

static char **adadbreak;
static void sigint(int k){**adadbreak+=1;signal(SIGINT,sigint);}
static char input[1000];

// J calls for input (debug suspension and 1!:1[1) and we call for input
char* _stdcall Jinput(J jt,char* prompt)
{
	fputs(prompt,stdout);
	if(!fgets(input, sizeof(input), stdin))
	{
		fputs("\n",stdout);
		**adadbreak+=1;
	}
	return input;
}

// J calls for output
void _stdcall Joutput(J jt,int type, char* s)
{
	if(MTYOEXIT==type) exit((int)(I)s);
	fputs(s,stdout);
	fflush(stdout);
}

// J calls for 11!:x y - demonstrates all features of jwdw and jwdp
// pa is y and result array (if any) is returned in pz
// return  pz
//  0           - MTM i.0 0
// -1      A    - A result
// -2      A    - A is pairs of null terminated strings for event table
//  n           - EVDOMAIN etc
int _stdcall Jwd(J jt, int x, A pa, A* pz)
{
	A r; char c; I i;
	if(!AN(pa)) return EVDOMAIN;
	if(0==x)
	{
		if(!(LIT&AT(pa))) return EVDOMAIN;
		c=*CAV(pa);
		if('0'==c) return 0;
		if('1'==c)
		{
			char res[]="result";
			I cnt=(I)strlen(res);
			r=jga(jt,LIT,cnt,1,&cnt);
			if(0==r) return EVDOMAIN;
			strcpy(CAV(r),res);
			*pz=r;
			return -1; // string
		}
		if('2'==c)
		{
			char res[]="a\0aa\0b\0bbb\0"; // null delimited pairs
			I cnt=(I)sizeof(res);
			r=jga(jt,LIT,cnt,1,&cnt);
			memcpy(CAV(r),res,cnt);
			*pz=r;
			return -2; // event data table
		}
		if('3'==c)
		{
			char* res=jgetlocale(jt);
			I cnt=(I)strlen(res);
			r=jga(jt,LIT,cnt,1,&cnt);
			if(0==r) return EVDOMAIN;
			strcpy(CAV(r),res);
			*pz=r;
			return -1; // string from current locale
		}
	}
	if(x==2000)
	{
		if(!(INT&AT(pa))) return EVDOMAIN;
		i=*IAV(pa);
		if(0==i) return 0;
		if(1==i)
		{
			I res[]={1,2,3,4,5};
			I cnt=5;
			r=jga(jt,INT,cnt,1,&cnt);
			if(0==r) return EVDOMAIN;
			memcpy(IAV(r),res,cnt*sizeof(I));
			*pz=r;
			return -1; // int vector
		}
	}
	return EVDOMAIN;
}

int main(int argc, char* argv[])
{
	void* callbacks[] = {Joutput,Jwd,Jinput,0,(void*)SMCON};
	char pathdll[1000];
	if(!_getcwd(pathdll,sizeof(pathdll)))pathdll[0]=0;
	strcat(pathdll,JDLLNAME);
#ifdef _WIN32
	hjdll=LoadLibraryA(pathdll);
#else
	hjdll=dlopen(pathdll,RTLD_LAZY);
#endif
	if(!hjdll)
	{
		fputs("Load library failed: ",stderr);
		fputs(pathdll,stderr);
		fputs("\n",stderr);
		return 1; // load library failed
	}
	jt=((JInitType)GETPROCADDRESS(hjdll,"JInit"))();
	if(!jt) return 1; // JE init failed
	((JSMType)GETPROCADDRESS(hjdll,"JSM"))(jt,callbacks);
	jdo=(JDoType)GETPROCADDRESS(hjdll,"JDo");
	jfree=(JFreeType)GETPROCADDRESS(hjdll,"JFree");
	jga=(JgaType)GETPROCADDRESS(hjdll,"Jga");
	jgetlocale=(JGetLocaleType)GETPROCADDRESS(hjdll,"JGetLocale");
	adadbreak=(char**)jt; // first address in jt is address of breakdata
	signal(SIGINT,sigint);
	while(1){jdo(jt,Jinput(jt,"   "));}
	jfree(jt);
	return 0;
}
