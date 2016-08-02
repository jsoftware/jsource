/* Copyright 1990-2011, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */

/* used to build tsdll.dll for validating cd */

#ifdef _WIN32
#include <windows.h>
int WINAPI DllMain (HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved)
{
	return TRUE;
}
typedef wchar_t wc;
typedef unsigned int uc;
#else
#define _stdcall
#define _cdecl
typedef unsigned short wc;
typedef unsigned int uc;
#endif

#include <stdio.h>

#if defined(_WIN64) || defined(_UNIX64)
#define SY_64 1
typedef long long I;
#else
#define SY_64 0
typedef long I;
#endif

#define sum {return a[0]=b+c[0]+c[1];}

typedef double D;
typedef float F;

// test each integer type
// tests in place s (and i for J64) in place conversion
char    _stdcall cbasic(char*  a,  char  b, char*  c) sum
wc      _stdcall wbasic(wc*    a,  wc    b, wc*    c) sum
uc      _stdcall ubasic(uc*    a,  uc    b, uc*    c) sum
short   _stdcall sbasic(short* a,  short b, short* c) sum
int     _stdcall ibasic(int*   a,  int   b, int*   c) sum
I       _stdcall xbasic(I*     a,  I     b, I*     c) sum

// test pointer result
char cd[]="test";
char* _stdcall pc(){ return cd;}


// test d result and *d
double _stdcall dipdpd(int c, double* p, double* pr)
{
	double d=0; int i;
	for(i=0; i<c; ++i)
	{
		*pr=d+=p[i];
	}
	return d;
}

// test f result and *f (convert in place)
float _stdcall fipfpf(int c, float* p, float* pr)
{
	float f=0; int i;
	for(i=0; i<c; ++i)
	{
		*pr=f+=(float)p[i];
	}
	return f;
}

double _stdcall complex(int c, double* j){ return j[c];}

float _stdcall  f(){ return (float)1.5;}
double _stdcall d(){ return (double)1.5;}

D _stdcall dd(D d0){return d0*2;}
D _stdcall ddd(D d0, D d1){return d0+d1;}
D _stdcall dddd(D d0, D d1, D d2){return d0+d1+d2;}

D _stdcall dx0(I a,D b)              {return a+b;}
D _stdcall dx1(D a,I b)              {return a+b;}
D _stdcall dx2(I a,D b,I c)          {return a+b+c;}
D _stdcall dx3(D a,I b,D c)          {return a+b+c;}
D _stdcall dx4(I a,D b,I c,D d)      {return a+b+c+d;}
D _stdcall dx5(D a,I b,D c,I d)      {return a+b+c+d;}
D _stdcall dx6(I a,D b,I c,D d,I e)  {return a+b+c+d+e;}
D _stdcall dx7(D a,I b,D c,I d,D e)  {return a+b+c+d+e;}

D _stdcall d1(D a,D b,D c,D d,D e,D f,D g,D h){
	return a+b+c+d+e+f+g+h;}

/* 9 double scalars is error 7 0 in linux64 */
D _stdcall d1a(D a,D b,D c,D d,D e,D f,D g,D h,D i){
	return a+b+c+d+e+f+g+h+i;}

I _stdcall d2(D a,D b,D c,D d,D e,D f,D g,D h){
	return (I)(a+b+c+d+e+f+g+h);}

D _stdcall d3(D a,I b,D c,I d,D e,I f,D g,I h,D i,I j,D k,I l,D m,I n,D o,I p){
	return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;}

D _stdcall d4(D a,int b,D c,int d,D e,int f,D g,int h,D i,int j,D k,int l,D m,int n,D o,int p){
	return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;}

D _stdcall d5(D a,I b, D c, I d, D e, I f, D g, D* pd, F* pf, I* px, int* pi){
	return a+b+c+d+e+f+g+pd[0]+pd[1]+pf[0]+pf[1]+px[0]+px[1]+pi[0]+pi[1];}

F _stdcall f1(F a,F b,F c,F d,F e,F f,F g,F h){
	return a+b+c+d+e+f+g+h;}

I _stdcall f2(F a,F b,F c,F d,F e,F f,F g,F h){
	return (I)(a+b+c+d+e+f+g+h);}

F _stdcall f3(F a,I b,F c,I d,F e,I f,F g,I h,F i,I j,F k,I l,F m,I n,F o,I p){
	return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;}

float _stdcall fff(float  f0, float  f1){return f0+f1;}

D _stdcall fd(float f0, D d0, float f1, D d1, float* fp, D* fd){
	fp[0]=f0; fp[1]=f1; fd[0]=d0; fd[1]=d1;
	return f0+f1+d0+d1;
}

// __cdecl - default C calling convention
// __cdecl is the alternate (+ cd flag) calling convention
int _cdecl altinci(int i){return ++i;}

// '0 procaddress ...'
I _stdcall xbasic_add(){return (I)xbasic;}

// '1 procindex ...'
#ifndef _WIN32
typedef I (_stdcall *PROC)();
#endif
I _stdcall objxxx(void* obj,I a,I b){return a+b;}
D _stdcall objddd(void* obj,D a,D b){return a+b;}
//PROC vtable[]={&(PROC)objxxx,&(PROC)objddd};
PROC vtable[]={(PROC)objxxx,(PROC)objddd};
PROC* object=vtable;
I _stdcall obj_add(){return(I)&object;}
