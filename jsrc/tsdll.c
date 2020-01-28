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
#ifdef _MSC_VER
#define CDPROC
#elif defined(__GNUC__)
#define CDPROC __attribute__ ((visibility ("default")))
#else
#define CDPROC
#endif
#include <math.h>
#include <complex.h>
#undef I
#ifdef _MSC_VER
typedef _Fcomplex float_complex;
typedef _Dcomplex double_complex;
#else
typedef float complex float_complex;
typedef double complex double_complex;
#endif

#ifdef ANDROID
#if __ANDROID_API__ < 23
#undef creal
#undef cimag
#undef cabs
#undef crealf
#undef cimagf
#undef cabsf
#define creal(x)  (*(double*)&x)
#define cimag(x)  (*(1+(double*)&x))
#define cabs(x)   (sqrt((*(double*)&x)*(*(double*)&x)+(*(1+(double*)&x))*(*(1+(double*)&x))))
#define crealf(x) (*(float*)&x)
#define cimagf(x) (*(1+(float*)&x))
#define cabsf(x)  (sqrtf((*(float*)&x)*(*(float*)&x)+(*(1+(float*)&x))*(*(1+(float*)&x))))
#endif
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

typedef unsigned char BYTE;
typedef double D;
typedef float F;

// test each integer type
// tests in place s (and i for J64) in place conversion
CDPROC char    _stdcall cbasic(char*  a,  char  b, char*  c) sum
CDPROC wc      _stdcall wbasic(wc*    a,  wc    b, wc*    c) sum
CDPROC uc      _stdcall ubasic(uc*    a,  uc    b, uc*    c) sum
CDPROC BYTE    _stdcall bbasic(BYTE*  a,  BYTE  b, BYTE*  c) sum
CDPROC short   _stdcall sbasic(short* a,  short b, short* c) sum
CDPROC int     _stdcall ibasic(int*   a,  int   b, int*   c) sum
CDPROC I       _stdcall xbasic(I*     a,  I     b, I*     c) sum
CDPROC D       _stdcall dbasic(D*     a,  D     b, D*     c) sum
CDPROC F       _stdcall fbasic(F*     a,  F     b, F*     c) sum

#ifdef _MSC_VER
#define dsum {a[0]=_DCOMPLEX_(creal(b[0])+creal(c[0])+creal(c[1]), cimag(b[0])+cimag(c[0])+cimag(c[1])); return cabs(a[0]);}
#define fsum {a[0]=_FCOMPLEX_(crealf(b[0])+crealf(c[0])+crealf(c[1]), cimagf(b[0])+cimagf(c[0])+cimagf(c[1])); return cabsf(a[0]);}
#else
#define dsum {a[0]=b[0]+c[0]+c[1]; return cabs(a[0]);}
#define fsum {a[0]=b[0]+c[0]+c[1]; return cabsf(a[0]);}
#endif

CDPROC D _stdcall jbasic(double_complex* a, double_complex* b, double_complex* c) dsum
CDPROC F _stdcall zbasic(float_complex* a,  float_complex* b,  float_complex* c)  fsum

// test pointer result
static char cd[]="test";
CDPROC char* _stdcall pc(){ return cd;}

// test in place convert down/up
CDPROC void _stdcall downup(I n, short* a, int* b, float* c) {I i; for(i=0;i<n;++i){a[i]=-a[i];b[i]=-b[i];c[i]=-c[i];}}

// test d result and *d
CDPROC double _stdcall dipdpd(int c, double* p, double* pr)
{
	double d=0; int i;
	for(i=0; i<c; ++i)
	{
		*pr=d+=p[i];
	}
	return d;
}

// test f result and *f (convert in place)
CDPROC float _stdcall fipfpf(int c, float* p, float* pr)
{
	float f=0; int i;
	for(i=0; i<c; ++i)
	{
		*pr=f+=(float)p[i];
	}
	return f;
}

// test b result and *b (convert in place)
CDPROC BYTE _stdcall bipbpb(int c, BYTE* p, BYTE* pr)
{
	BYTE f=0; int i;
	for(i=0; i<c; ++i)
	{
		*pr=f+=(BYTE)p[i];
	}
	return f;
}

CDPROC double _stdcall dcomplex0(int c, double* j){ return j[c];}
CDPROC float _stdcall fcomplex0(int c, float* j){ return j[c];}
CDPROC void _stdcall dcomplex1(int c, double_complex* j, double* a, double* b) {I i; for(i=0;i<c;++i){a[i]=creal(j[i]);b[i]=-cimag(j[i]);}}
CDPROC void _stdcall fcomplex1(int c, float_complex* j, float* a, float* b) {I i; for(i=0;i<c;++i){a[i]=crealf(j[i]);b[i]=-cimagf(j[i]);}}

CDPROC float _stdcall  f(){ return (float)1.5;}
CDPROC double _stdcall d(){ return (double)1.5;}

CDPROC D _stdcall dd(D d0){return d0*2;}
CDPROC D _stdcall ddd(D d0, D d1){return d0+d1;}
CDPROC D _stdcall dddd(D d0, D d1, D d2){return d0+d1+d2;}

CDPROC D _stdcall dx0(I a,D b)              {return a+b;}
CDPROC D _stdcall dx1(D a,I b)              {return a+b;}
CDPROC D _stdcall dx2(I a,D b,I c)          {return a+b+c;}
CDPROC D _stdcall dx3(D a,I b,D c)          {return a+b+c;}
CDPROC D _stdcall dx4(I a,D b,I c,D d)      {return a+b+c+d;}
CDPROC D _stdcall dx5(D a,I b,D c,I d)      {return a+b+c+d;}
CDPROC D _stdcall dx6(I a,D b,I c,D d,I e)  {return a+b+c+d+e;}
CDPROC D _stdcall dx7(D a,I b,D c,I d,D e)  {return a+b+c+d+e;}

CDPROC D _stdcall d1(D a,D b,D c,D d,D e,D f,D g,D h){
	return a+b+c+d+e+f+g+h;}

CDPROC D _stdcall d1a(D a,D b,D c,D d,D e,D f,D g,D h,D i){
	return a+b+c+d+e+f+g+h+i;}

CDPROC D _stdcall d1b(D a,D b,D c,D d,D e,D f,D g,D h,D i,D j,D k,D l,D m){
       return a+b+c+d+e+f+g+h+i+j+k+l+m;}
CDPROC D _stdcall d1c(D a,D b,D c,D d,D e,D f,D g,D h,D i,D j,D k,D l,D m,D n){
       return a+b+c+d+e+f+g+h+i+j+k+l+m+n;}
CDPROC D _stdcall d1z(D a,D b,D c,D d,D e,D f,D g,D h,D i,D j,D k,D l,D m,D n,D o,D p,D q,D r,D s,D t,D u,D v,D w,D x,D y,D z){
       return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+w+x+y+z;}

CDPROC I _stdcall d2(D a,D b,D c,D d,D e,D f,D g,D h){
	return (I)(a+b+c+d+e+f+g+h);}

CDPROC D _stdcall d3(D a,I b,D c,I d,D e,I f,D g,I h,D i,I j,D k,I l,D m,I n,D o,I p){
	return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;}

CDPROC D _stdcall d4(D a,int b,D c,int d,D e,int f,D g,int h,D i,int j,D k,int l,D m,int n,D o,int p){
	return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;}

CDPROC D _stdcall d5(D a,I b, D c, I d, D e, I f, D g, D* pd, F* pf, I* px, int* pi){
	return a+b+c+d+e+f+g+pd[0]+pd[1]+pf[0]+pf[1]+px[0]+px[1]+pi[0]+pi[1];}

// corner cases
CDPROC D _stdcall di1d9a(I ia,D a,D b,D c,D d,D e,D f,D g,D h,D i){
	return ia + a+b+c+d+e+f+g+h+i;}

CDPROC D _stdcall di1d9b(D a,D b,D c,D d,D e,D f,D g,D h,D i,I ia){
	return ia + a+b+c+d+e+f+g+h+i;}

CDPROC D _stdcall di6d9a(I ia,I ib,I ic,I id,I ie,I iF,D a,D b,D c,D d,D e,D f,D g,D h,D i){
	return ia+ib+ic+id+ie+iF + a+b+c+d+e+f+g+h+i;}

CDPROC D _stdcall di6d9b(I ia,I ib,I ic,I id,I ie,D a,D b,D c,D d,D e,D f,D g,D h,D i,I iF){
	return ia+ib+ic+id+ie+iF + a+b+c+d+e+f+g+h+i;}

CDPROC D _stdcall di7d9a(I ia,I ib,I ic,I id,I ie,I iF,I ig,D a,D b,D c,D d,D e,D f,D g,D h,D i){
	return ia+ib+ic+id+ie+iF+ig + a+b+c+d+e+f+g+h+i;}

CDPROC D _stdcall di7d9b(I ia,I ib,I ic,I id,I ie,I iF,D a,D b,D c,D d,D e,D f,D g,D h,D i,I ig){
	return ia+ib+ic+id+ie+iF+ig + a+b+c+d+e+f+g+h+i;}

CDPROC D _stdcall di8d9a(I ia,I ib,I ic,I id,I ie,I iF,I ig,I ih,D a,D b,D c,D d,D e,D f,D g,D h,D i){
	return ia+ib+ic+id+ie+iF+ig+ih + a+b+c+d+e+f+g+h+i;}

CDPROC D _stdcall di8d9b(I ia,I ib,I ic,I id,I ie,I iF,I ig,D a,D b,D c,D d,D e,D f,D g,D h,D i,I ih){
	return ia+ib+ic+id+ie+iF+ig+ih + a+b+c+d+e+f+g+h+i;}

CDPROC D _stdcall di9d9a(I ia,I ib,I ic,I id,I ie,I iF,I ig,I ih,I ii,D a,D b,D c,D d,D e,D f,D g,D h,D i){
	return ia+ib+ic+id+ie+iF+ig+ih+ii + a+b+c+d+e+f+g+h+i;}

CDPROC D _stdcall di9d9b(I ia,I ib,I ic,I id,I ie,I iF,I ig,I ih,D a,D b,D c,D d,D e,D f,D g,D h,D i,I ii){
	return ia+ib+ic+id+ie+iF+ig+ih+ii + a+b+c+d+e+f+g+h+i;}

// raspberry pi ARMHF
#define I3 I i1,I i2,I i3
#define I4 I i1,I i2,I i3,I i4
#define I5 I i1,I i2,I i3,I i4,I i5
#define I6 I i1,I i2,I i3,I i4,I i5,I i6
#define I7 I i1,I i2,I i3,I i4,I i5,I i6,I i7
#define I8 I i1,I i2,I i3,I i4,I i5,I i6,I i7,I i8
#define I9 I i1,I i2,I i3,I i4,I i5,I i6,I i7,I i8,I i9
#define D7 D d1,D d2,D d3,D d4,D d5,D d6,D d7
#define D8 D d1,D d2,D d3,D d4,D d5,D d6,D d7,D d8

CDPROC D _stdcall dr160(I3,F fa,D7,F fb,F fc,D da,F fd,I ia){
	return ia+fa+fb+fc+fd+da + i1+i2+i3 + d1+d2+d3+d4+d5+d6+d7;}

CDPROC D _stdcall dr161(I3,F fa,D7,F fb,D da,F fc,D db,I ia){
	return ia+fa+fb+fc+da+db + i1+i2+i3 + d1+d2+d3+d4+d5+d6+d7;}

CDPROC D _stdcall dr162(I3,F fa,D7,I ia,F fb,F fc,D da,I ib){
	return ia+ib+fa+fb+fc+da + i1+i2+i3 + d1+d2+d3+d4+d5+d6+d7;}

CDPROC D _stdcall dr163(I3,F fa,D7,I ia,F fb,D da,F fc,I ib){
	return ia+ib+fa+fb+fc+da + i1+i2+i3 + d1+d2+d3+d4+d5+d6+d7;}

CDPROC D _stdcall dr164(I3,F fa,D7,I ia,D da,F fb,D db,I ib){
	return ia+ib+fa+fb+da+db + i1+i2+i3 + d1+d2+d3+d4+d5+d6+d7;}

CDPROC D _stdcall dr165(I4,F fa,D7,F fb,F fc,D da,I ia){
	return ia+fa+fb+fc+da + i1+i2+i3+i4 + d1+d2+d3+d4+d5+d6+d7;}

CDPROC D _stdcall dr166(I4,F fa,D7,F fb,D da,F fc,I ia){
	return ia+fa+fb+fc+da + i1+i2+i3+i4 + d1+d2+d3+d4+d5+d6+d7;}

CDPROC D _stdcall dr167(I4,F fa,D7,D da,F fb,D db,I ia){
	return ia+fa+fb+da+db + i1+i2+i3+i4 + d1+d2+d3+d4+d5+d6+d7;}

CDPROC D _stdcall dr168(I4,F fa,D7,I ia,F fb,D da,I ib){
	return ia+ib+fa+fb+da + i1+i2+i3+i4 + d1+d2+d3+d4+d5+d6+d7;}

CDPROC D _stdcall dr169(I4,F fa,D7,I ia,D da,F fb,I ib){
	return ia+ib+fa+fb+da + i1+i2+i3+i4 + d1+d2+d3+d4+d5+d6+d7;}

CDPROC D _stdcall dr10a(I ia,F fa,D7,F fb){
	return ia+fa+fb + d1+d2+d3+d4+d5+d6+d7;}

CDPROC D _stdcall dr10b(F fa,D7,F fb,I ia){
	return ia+fa+fb + d1+d2+d3+d4+d5+d6+d7;}

CDPROC D _stdcall dr10c(F fa,D7,F fb,D da){
	return da+fa+fb + d1+d2+d3+d4+d5+d6+d7;}

CDPROC D _stdcall dr11a(I ia,F fa,D8,F fb){
	return ia+fa+fb + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr11b(F fa,D8,F fb,I ia){
	return ia+fa+fb + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr11c(F fa,D8,F fb,D da){
	return da+fa+fb + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr12a(I ia,F fa,D8,F fb,D da){
	return ia+fa+fb+da + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr12b(F fa,D8,F fb,I ia,D da){
	return ia+fa+fb+da + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr12c(F fa,D8,F fb,F fc,D da){
	return da+fa+fc+fb + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr13a(I4,F fa,D8){
	return fa + i1+i2+i3+i4 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr13b(I3,F fa,D8,I ia){
	return ia+fa + i1+i2+i3 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr13c(I3,F fa,D8,F fb){
	return fa+fb + i1+i2+i3 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr14a(I5,F fa,D8){
	return fa + i1+i2+i3+i4+i5 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr14b(I4,F fa,D8,I ia){
	return ia+fa + i1+i2+i3+i4 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr14c(I4,F fa,D8,F fb){
	return fa+fb + i1+i2+i3+i4 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr15a(I6,F fa,D8){
	return fa + i1+i2+i3+i4+i5+i6 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr15b(I5,F fa,D8,I ia){
	return ia+fa + i1+i2+i3+i4+i5 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr15c(I5,F fa,D8,F fb){
	return fa+fb + i1+i2+i3+i4+i5 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr16a(I7,F fa,D8){
	return fa + i1+i2+i3+i4+i5+i6+i7 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr16b(I6,F fa,D8,I ia){
	return ia+fa + i1+i2+i3+i4+i5+i6 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr16c(I6,F fa,D8,F fb){
	return fa+fb + i1+i2+i3+i4+i5+i6 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr17a(I7,F fa,D8, F fb){
	return fa+fb + i1+i2+i3+i4+i5+i6+i7 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr17b(I6,F fa,D8,F fb,I ia){
	return ia+fa+fb + i1+i2+i3+i4+i5+i6 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr17c(I6,F fa,D8,F fb,D da){
	return fa+fb+da + i1+i2+i3+i4+i5+i6 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr17d(I8,F fa,D8){
	return fa + i1+i2+i3+i4+i5+i6+i7+i8 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr17e(I7,F fa,D8,I ia){
	return ia+fa + i1+i2+i3+i4+i5+i6+i7 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr17f(I7,F fa,D8,F fb){
	return fa+fb + i1+i2+i3+i4+i5+i6+i7 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr18a(I8,F fa,D8, F fb){
	return fa+fb + i1+i2+i3+i4+i5+i6+i7+i8 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr18b(I7,F fa,D8,F fb,I ia){
	return ia+fa+fb + i1+i2+i3+i4+i5+i6+i7 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr18c(I7,F fa,D8,F fb,D da){
	return fa+fb+da + i1+i2+i3+i4+i5+i6+i7 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr18d(I9,F fa,D8){
	return fa + i1+i2+i3+i4+i5+i6+i7+i8+i9 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr18e(I8,F fa,D8,I ia){
	return ia+fa + i1+i2+i3+i4+i5+i6+i7+i8 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr18f(I8,F fa,D8,F fb){
	return fa+fb + i1+i2+i3+i4+i5+i6+i7+i8 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr19a(I9,F fa,D8,F fb){
	return fa+fb + i1+i2+i3+i4+i5+i6+i7+i8+i9 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr19b(I8,F fa,D8,F fb,I ia){
	return ia+fa+fb + i1+i2+i3+i4+i5+i6+i7+i8 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC D _stdcall dr19c(I8,F fa,D8,F fb,D da){
	return fa+fb+da + i1+i2+i3+i4+i5+i6+i7+i8 + d1+d2+d3+d4+d5+d6+d7+d8;}

CDPROC F _stdcall f1(F a,F b,F c,F d,F e,F f,F g,F h){
	return a+b+c+d+e+f+g+h;}

CDPROC I _stdcall f2(F a,F b,F c,F d,F e,F f,F g,F h){
	return (I)(a+b+c+d+e+f+g+h);}

CDPROC F _stdcall f3(F a,I b,F c,I d,F e,I f,F g,I h,F i,I j,F k,I l,F m,I n,F o,I p){
	return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;}

CDPROC float _stdcall fff(float  f0, float  f1){return f0+f1;}

CDPROC D _stdcall fd(float f0, D d0, float f1, D d1, float* fp, D* fd){
	fp[0]=f0; fp[1]=f1; fd[0]=d0; fd[1]=d1;
	return f0+f1+d0+d1;
}

CDPROC D _stdcall fdi(F a,D b,I c,F d,D e,I f,F g,D h,I i,F j,D k,I l,F m,D n,I o,F p,D q,I r,F s,D t,I u,F v,D w,I x,F y,D z){
       return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+w+x+y+z;}
CDPROC D _stdcall ffi(F a,F b,I c,F d,F e,I f,F g,F h,I i,F j,F k,I l,F m,F n,I o,F p,F q,I r,F s,F t,I u,F v,F w,I x,F y,F z){
       return a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p+q+r+s+t+u+v+w+x+y+z;}

// __cdecl - default C calling convention
// __cdecl is the alternate (+ cd flag) calling convention
CDPROC int _cdecl altinci(int i){return ++i;}

// '0 procaddress ...'
CDPROC I _stdcall xbasic_add(){return (I)xbasic;}

// '1 procindex ...'
#ifndef _WIN32
typedef I (_stdcall *PROC)();
#endif
CDPROC I _stdcall objxxx(void* obj,I a,I b){return a+b;}
CDPROC D _stdcall objddd(void* obj,D a,D b){return a+b;}
//PROC vtable[]={&(PROC)objxxx,&(PROC)objddd};
CDPROC PROC vtable[]={(PROC)objxxx,(PROC)objddd};
CDPROC PROC* object=vtable;
CDPROC I _stdcall obj_add(){return(I)&object;}
