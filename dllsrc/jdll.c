/* Copyright 1990-2001, Jsoftware Inc.  All rights reserved. */
/* Licensed use only. Any other use is in violation of copyright. */
/*                                                                         */
/* J Windows dll and com interface (old jcom.c, jdll.c, jwin32.c)          */

// Included as part of JDLL server
// Routines here provide services for JEXE object, JDLL object,
// and good old JDLL dll calls

// Steps required to add a new JEXE method (change names for JDLL)
// 1. add method to methods.odl
// 2. smsrc\tlbdebug.bat to create jexe.tlb and jexeodl.h (also creates jdll.tlb)
//    be sure new tlb files are in directory when jreg is run!
// 3. add method to jdllcom.h
// 4. add method declarations (dll and exe) to jdllcomx.cpp (JDoType JDo/*Jdo)
// 5. add method to jdllcomx.cpp (dll and exe sections)
// 6. add method (with J prefix) to jcom.c
// 7. add typedef to jdlltype.h
// 8. add export to jdll.def
// 9. add setting of dll proc address in smj32.dll (getprocaddress)
// 10. run jreg with new tlb files

#include <windows.h>
#include "../jsrc/j.h"
#include "../jsrc/jlib.h"

extern void wtom(US* src, I srcn, UC* snk);
extern void utow(C4* src, I srcn, US* snk);
extern I utowsize(C4* src, I srcn);
int valid(C* psrc, C* psnk);
C* esub(J jt, long ec);

#ifdef OLECOM
extern int uniflag;
#endif

I jdo(J, C*);

#define MAXRANK	60

static I tounin(C* src, UI m, WCHAR* sink, UI n)
{
  return MultiByteToWideChar(CP_UTF8,0,src,(int)m,sink,(int)n);
}

void touni(C* src, WCHAR* sink)
{
	while(*src)
		*sink++ = *src++;
	*sink++=0;
}

static void toutf8n(WCHAR* src, LPSTR sink, UI n)
{
  WideCharToMultiByte(CP_UTF8,0,src,1+(int)wcslen(src),sink,(int)n,0,0);
}

void toasc(WCHAR* src, C* sink)
{
	while(*src)
		*sink++ = (char)*src++;
	*sink++=0;
}

int _stdcall JBreak(J jt){ return 0;}

int _stdcall JIsBusy(J jt){	return 0;}

#ifdef OLECOM

#if !SY_WINCE

//! 64 bit problems - com and dll interface is 32 bit - needs test and thought
static int a2v (J jt, A a, VARIANT *v, int dobstrs)
{
	SAFEARRAY FAR* psa; 
	SAFEARRAYBOUND rgsabound[MAXRANK];
	int er;
	I i,r,k,kw,t,cb,*pi;
	VARTYPE vt;

	k=AN(a);
	pi=AV(a);
	r=AR(a);
	t=NOUN&AT(a);
	if(r>MAXRANK) return EVRANK;
	if(dobstrs && r<2 && (t&LIT+C2T+C4T)) 	// char scalar or vector returned as BSTR
	{
    WCHAR *wstr;
		BSTR bstr;
    if (LIT&t) {
      wstr = malloc(sizeof(WCHAR)*k);
		  kw=tounin((C*)pi, k, wstr, k);
		  bstr = SysAllocStringLen(wstr, (UINT)kw);
    } else if (C4T&t) {
      kw=utowsize((C4*)pi, k);
      kw=(kw<0)?(-kw):kw;
      wstr = malloc(sizeof(WCHAR)*kw);
      utow((C4*)pi, k, wstr);
		  bstr = SysAllocStringLen(wstr, (UINT)kw);
    } else
		  bstr = SysAllocStringLen((WCHAR*)pi, (UINT)k);
		v->vt=VT_BSTR;
		v->bstrVal=bstr;
    if (t&LIT+C4T) free(wstr);
		R 0;
	}
	switch(t)
	{
	case LIT:
		if(!r) {v->vt=VT_UI1; v->bVal = *(C*)pi; return 0;}
		vt=VT_UI1;
		cb=k*sizeof(char);
		break;

	case C2T:
		if(!r) {v->vt=VT_UI2; v->iVal = *(WCHAR*)pi; return 0;}
		vt=VT_UI2;
		cb=k*sizeof(WCHAR);
		break;

	case C4T:
		if(!r) {v->vt=VT_UI4; v->iVal = *(UI4*)pi; return 0;}
		vt=VT_UI4;
		cb=k*sizeof(C4);
		break;

	case B01:
		if(!r) {
			v->vt=VT_BOOL;
			v->boolVal = *(B*)pi ? VARIANT_TRUE : VARIANT_FALSE;
			return 0;
		}
		vt=VT_BOOL;
		break;

	case INT:
#if SY_64
		if(jt->int64rflag) {
		  if(!r) {v->vt=VT_I8; v->llVal = (I)(*pi); return 0;}
		  vt=VT_I8;
		  cb=k*sizeof(long long);
    } else {
		  if(!r) {v->vt=VT_I4; v->lVal = (int)(*pi); return 0;}
		  vt=VT_I4;
		  cb=k*sizeof(int);
    }
#else
		if(!r) {v->vt=VT_I4; v->lVal = (I)(*pi); return 0;}
		vt=VT_I4;
		cb=k*sizeof(int);
#endif
		break;

	case FL:
		if(!r) {v->vt=VT_R8; v->dblVal = *(D*)pi; return 0;}
		vt=VT_R8;
		cb=k*sizeof(double);
		break;

	case BOX:
		if(!r)
		{
			// Pass a scalar box as a 1-elem VARIANT VT_ARRAY.
			// It's marked as such by a lower bound set at -1.
			// (All "true" boxed arrays will have the usual lb 0.)
			rgsabound[0].lLbound = -1;
			rgsabound[0].cElements = 1;

			if ( ! (psa = SafeArrayCreate (VT_VARIANT, 1, rgsabound)))
				return EVWSFULL;
			if (0!= (er = a2v (jt, *(A*)pi, (VARIANT*)psa->pvData, dobstrs)))
			{
				SafeArrayDestroy (psa);
				return er;
			}
			v->vt=VT_ARRAY|VT_VARIANT;
			v->parray = psa;
			return 0;
		}
		vt=VT_VARIANT;
		cb=k*sizeof(A);
		break;

	default:
		return EVDOMAIN;
	}


	if(1<r && jt->transposeflag)
		RE(a=cant1(a));  // undo shape reversal later!

	for(i=0; i<r; ++i)
	{
		rgsabound[i].lLbound = 0; 
		// undo shape reversal from cant1() here.
		// In case of Transpose(0), the shape is
		// still passed in Column-major notation.
		rgsabound[i].cElements = (ULONG)AS(a)[r-1-i]; 
	}
	psa = SafeArrayCreate(vt, (UINT)r, rgsabound); 
	if(!psa)
	{
		return EVWSFULL;
	}

	switch (NOUN&AT(a))
	{
	case B01:
	{
		VARIANT_BOOL *pv = (VARIANT_BOOL*) psa->pvData;
		B *ap = BAV(a);

		while (k--)
			*pv++ = *ap++ ? VARIANT_TRUE : VARIANT_FALSE;
		break;
	}
	case BOX:
	{
		A* ap;
		VARIANT *v;

		for (ap=AAV(a), SafeArrayAccessData(psa, (void **)&v);
			 ap<AAV(a)+k;
			 ++ap, ++v)
		{
			PROLOG(0118);
			er=a2v (jt, *ap, v, dobstrs);
			tpop(_ttop);
			if (er!=0)
			{
				SafeArrayUnaccessData (psa);
				SafeArrayDestroy (psa);
				return er;
			}
		}
		SafeArrayUnaccessData (psa);
		break;
	}
#if SY_64
  case INT:
  {
    if (!jt->int64rflag) {
      long *p1=psa->pvData;
      I *p2=AV(a);
      while (k--)
        *p1++=(long)*p2++;
    }
		break;
	}
#endif
	default:
		memcpy(psa->pvData, AV(a), cb);
	}
	v->vt=VT_ARRAY|vt;
	v->parray = psa;
	return 0;
}

int jget(J jt, C* name, VARIANT* v, int dobstr)
{
	A a;
	char gn[256];
	int er;

	if(strlen(name) >= sizeof(gn)) return EVILNAME;
	if(valid(name, gn)) return EVILNAME; 
	RZ(a=symbrd(nfs(strlen(gn),gn)));
	A *old = jt->tnextpushp;
	er = a2v (jt, a, v, dobstr);
	tpop (old);
	return er;
}

int _stdcall JGet(J jt, C* name, VARIANT* v)
{
	return jget(jt, name, v, 0); // no bstrs
}

int _stdcall JGetB(J jt, C* name, VARIANT* v)
{
	return jget(jt, name, v, 1); // do bstrs
}

// convert a VARIANT to a J array
// returns 0 on error with detail in jerr.
static A v2a(J jt, VARIANT* v, int dobstrs)
{
	A a;
	SAFEARRAY* psa;
	SAFEARRAYBOUND* pb;
	I shape[MAXRANK];
	I k=1,n,r,i;
	I* pintsnk;
#if SY_64
	int* pint32src;
#else
	long long* pint64src;
#endif
	short* pshortsrc;
	unsigned short* pboolsrc;
	char* pboolsnk;
	VARTYPE t;
	int byref;
	double* pdoublesnk;
	float* pfloatsrc;

#define OPTREF(v,field)		(byref ? *v->p##field : v->field)

	t=v->vt;
	byref = t & VT_BYREF;
	t = t & ~VT_BYREF;

	if(dobstrs && t == VT_BSTR)
	{
		BSTR bstr; int len;

		bstr = OPTREF(v,bstrVal);

		if(uniflag)
			// len=SysStringLen(bstr);
      len=WideCharToMultiByte(CP_UTF8,0,bstr,(int)SysStringLen(bstr),0,0,0,0);
		else
			len=SysStringByteLen(bstr);
		RE(a=ga(LIT, len, 1, 0));
		if(uniflag)
			toutf8n(bstr, (C*)AV(a), len);
		else
			memcpy((C*)AV(a), (C*)bstr, len);
		R a;
	}
	if(t & VT_ARRAY)
	{
		psa = OPTREF(v,parray);
		pb = psa->rgsabound;
		r=psa->cDims;
		ASSERT(r<=MAXRANK,EVRANK);
		for(i=0; i<r; ++i)
		{
			n = pb[i].cElements;
			shape[i] = n; 
			k *= n;
		}
	}
	else
		r = 0;

	switch(t)
	{
	case VT_VARIANT | VT_ARRAY:
	{
		A *boxes;
		VARIANT* pv;
		
		// fixup scalar boxes which arrive
		// as a 1-elem vector with a lower bound at -1, not 0.
		if (pb[0].lLbound == -1)
		{
			ASSERT(psa->cDims==1 && pb[0].cElements==1, EVDOMAIN);
			r = 0;
		}
		RE(a=ga(BOX, k, r, (I*)&shape));
		ASSERT(S_OK==SafeArrayAccessData(psa, (void **)&pv),EVFACE);
		boxes = AAV(a);
		while(k--)
		{
			A z;
			// Don't use a PROLOG/EPILOG during v2a.
			// The z's are not getting their reference
			// count set until everything is in place
			// and the jset() is done in Jset().
			z = *boxes++ = v2a(jt, pv++, dobstrs);
			if (!z) break;
		}
		SafeArrayUnaccessData(psa);
		if (jt->jerr) return 0;
		break;
	}
	case VT_BOOL | VT_ARRAY:
		RE(a=ga(B01, k, r, (I*)&shape));
		pboolsrc = (VARIANT_BOOL*)psa->pvData;
		pboolsnk = BAV(a);
		// J bool returned from VB boolean, a -1 and 0 mess.
		// It wouldn't be that bad if the Microsoft folks used their own macros
		// and kept an eye an sign extensions.  But the way they are
		// doing it they are returning at least some TRUEs as value 255
		// instead of VARIANT_TRUE.  Therefore, we have to compare against
		// VARIANT_FALSE which -we hope- is consistently defined (as 0).
		while(k--)
			*pboolsnk++ = (*pboolsrc++)!=VARIANT_FALSE;
		break;

	case VT_UI1 | VT_ARRAY:
		RE(a=ga(LIT, k, r, (I*)&shape));
		memcpy(AV(a), psa->pvData, k * sizeof(char));
		break;

	case VT_UI2 | VT_ARRAY:
		RE(a=ga(C2T, k, r, (I*)&shape));
		memcpy(AV(a), psa->pvData, k * sizeof(short));
		break;

	case VT_UI4 | VT_ARRAY:
		RE(a=ga(C4T, k, r, (I*)&shape));
		memcpy(AV(a), psa->pvData, k * sizeof(int));
		break;

	case VT_I2 | VT_ARRAY:
		RE(a=ga(INT, k, r, (I*)&shape));
		pshortsrc = (short*)psa->pvData;
		pintsnk = AV(a);
		while(k--)
			*pintsnk++ = *pshortsrc++;
		break;

	case VT_I4 | VT_ARRAY:
		RE(a=ga(INT, k, r, (I*)&shape));
#if SY_64
		pint32src = (int*)psa->pvData;
		pintsnk = AV(a);
		while(k--)
			*pintsnk++ = *pint32src++;
#else
		memcpy(AV(a), psa->pvData, k * sizeof(int));
#endif
		break;

	case VT_I8 | VT_ARRAY:
		RE(a=ga(INT, k, r, (I*)&shape));
#if SY_64
		memcpy(AV(a), psa->pvData, k * sizeof(I));
#else
		pint64src = (long long*)psa->pvData;
		pintsnk = AV(a);
		while(k--)
			*pintsnk++ = (I)*pint64src++;
#endif
		break;

	case VT_R4 | VT_ARRAY:
		RE(a=ga(FL, k, r, (I*)&shape));
		pfloatsrc = (float*)psa->pvData;
		pdoublesnk = (double*)AV(a);
		while(k--)
			*pdoublesnk++ = *pfloatsrc++;
		break;

	case VT_R8 | VT_ARRAY:
		RE(a=ga(FL, k, r, (I*)&shape));
		memcpy(AV(a), psa->pvData, k * sizeof(double));
		break;

	case VT_UI1:
		RE(a=ga(LIT, 1, 0, 0));
		*CAV(a) = OPTREF(v,bVal);
		break;

	case VT_UI2:
		RE(a=ga(C2T, 1, 0, 0));
		*USAV(a) = (US)OPTREF(v,iVal);
		break;

	case VT_UI4:
		RE(a=ga(C4T, 1, 0, 0));
		*C4AV(a) = (C4)OPTREF(v,lVal);
		break;

	case VT_BOOL:
		RE(a=ga(B01, 1, 0, 0));
		// array case above explains this messy phrase:
		*BAV(a) = OPTREF(v,boolVal)!=VARIANT_FALSE;
		break;

	case VT_I2:
		RE(a=ga(INT, 1, 0, 0));
		*IAV(a) = OPTREF(v,iVal);
		break;

	case VT_I4:
		RE(a=ga(INT, 1, 0, 0));
		*IAV(a) = OPTREF(v,lVal);
		break;

	case VT_I8:
		RE(a=ga(INT, 1, 0, 0));
		*IAV(a) = (I)OPTREF(v,llVal);
		break;

	case VT_R4:
		RE(a=ga(FL, 1, 0, 0));
		*DAV(a) = OPTREF(v,fltVal);
		break;

	case VT_R8:
		RE(a=ga(FL, 1, 0, 0));
		*DAV(a) = OPTREF(v,dblVal);
		break;

	default:
		ASSERT(0,EVDOMAIN);
	}
	if(1<r && jt->transposeflag)
	{
		RE(a=cant1(a));
		DO(r, AS(a)[i]=shape[r-1-i];);
	}
	return a;
}
#endif // wince

// copy non-nulls only
static void touninx(C* src, WCHAR* sink, UI n)
{
	while(n--)
	{
		if(*src)
			*sink++ = *src++;
		else
			src++;
	}
	*sink++=0;
}

// copy output with nulls deleted, lf to crlf, cr(no lf) to crlf
void fixoutput(char* src, WCHAR* snk, I k)
{
	int i; char c;

    for(i=0; i<k; i++)
    {
		c = src[i];
		if(c==CCR || c==CLF)
		{
			*snk++ = CCR;
			*snk++ = CLF;
			if(c==CCR && (i+1)<k && src[i+1]==CLF) ++i;	// skip lf of crlf
		}
		else
		{
			if(c) *snk++ = c;
		}
	}
}

// adjust space required for output
// nulls -1, lf +1, cr (without lf) +1
I countoutput(I n, char*s)
{
    I i,k=0;

	for(i=0; i<n; i++)
    {
		switch(s[i])
		{
		case 0:  k -= 1; break;	// delete nulls
		case CLF: k += 1; break;	// lf needs a cr
		case CCR: 
			if((i+1)<n && CLF != s[i+1])
				k += 1;			// lone cr needs an lf
			else
				++i;			// skip lf of crlf
		}
	}
	return k;
}

#if !SY_WINCE
void oleoutput(J jt, I n, char* s)
{
	I k;

	if(!jt->oleop) return;
	k = countoutput(n, s);
	if(!jt->opbstr)
	{
		jt->opbstr = SysAllocStringLen(0, (UINT)(n+k));
		fixoutput(s, jt->opbstr, n);
	}
	else
	{
		I len = SysStringLen(jt->opbstr);
		SysReAllocStringLen((BSTR*)&jt->opbstr, 0, (UINT)(len+n+k));
		fixoutput(s, (BSTR)jt->opbstr + len, n);
	}
}

int jsetx(J jt, C* name, VARIANT* v, int dobstrs)
{
	int er;
	A *old=jt->tnextpushp;
	char gn[256];

	// validate name
	if(strlen(name) >= sizeof(gn)) return EVILNAME;
	if(valid(name, gn)) return EVILNAME; 

	er=jt->jerr=0;
	jset (gn, v2a(jt, v,dobstrs));	// no bstrs
	er=jt->jerr; jt->jerr=0;
	tpop(old);
	return er;
}

int _stdcall JSet(J jt, C* name, VARIANT* v)
{
	return jsetx(jt, name, v, 0);	// no bstrs
}

int _stdcall JSetB(J jt, C* name, VARIANT* v)
{
	return jsetx(jt, name, v, 1);	// do bstrs
}

int _stdcall JErrorText(J jt, long ec, VARIANT* v)
{
	C* p;
	SAFEARRAY FAR* psa; 
	SAFEARRAYBOUND rgsabound;
	I cb;

	p=esub(jt, ec);
	cb=1+strlen(p);	// include null
	rgsabound.lLbound = 0; 
	rgsabound.cElements = (ULONG)cb; 
	psa = SafeArrayCreate(VT_UI1, 1, &rgsabound); 
	if(!psa) return EVWSFULL;
	memcpy(psa->pvData, p, cb);
	v->vt = VT_ARRAY | VT_UI1;
	v->parray = psa;
	return 0;
}

int _stdcall JClear(J jt){ return 0;};

int _stdcall JInt64R(J jt, long b)
{
#if SY_64
	jt->int64rflag = b;
#endif
	return 0;
}

int _stdcall JTranspose(J jt, long b)
{
	jt->transposeflag = b;
	return 0;
}

int _stdcall JErrorTextB(J jt, long ec, VARIANT* v)
{
	C* p;
	BSTR bstr;

	p=esub(jt, ec);
	bstr = SysAllocStringLen(0, (UINT)strlen(p));
	tounin(p, strlen(p), bstr, strlen(p));
	v->vt=VT_BSTR;
	v->bstrVal=bstr;
	R 0;
}

int _stdcall JDoR(J jt, C* p, VARIANT* v)
{
	int e;
	
	jt->oleop=1;	// capture output
	jt->opbstr=0;	// none so far
	e=JDo(jt, p);
	jt->oleop=0;
	v->vt=VT_BSTR;
	v->bstrVal=jt->opbstr;
	R e;
}
#endif // wince

#endif

// previously in separate file when jdll.c and jcom.c both exisited
char modulepath[_MAX_PATH];
char dllpath[_MAX_PATH];
void dllquit(J);
void oleoutput(J,I n,char* s);
HINSTANCE g_hinst;
J g_jt;

extern C* getlocale(J);
extern void  FreeGL(HANDLE hglrc);


#if SY_WINCE
void getpath(HINSTANCE hi, C* path)
{
	WCHAR wpath[_MAX_PATH]; WCHAR* p;

	GetModuleFileName(hi, wpath,_MAX_PATH);
	p = wcsrchr(wpath, '\\');
	if(!p) p = wcsrchr(wpath, ':');
	*(1+p) = 0;
	wtom(wpath,wcslen(wpath),path);
}
#else
void getpath(HINSTANCE hi, C* path)
{
	WCHAR wpath[_MAX_PATH];WCHAR* p;

	GetModuleFileNameW(hi, wpath, _MAX_PATH);
	p = wcsrchr(wpath, '\\');
	if(!p) p = wcsrchr(wpath, ':');
	*(1+p) = 0;
	wtom(wpath,wcslen(wpath),path);
}
#endif


J heapinit(int size)
{
	HANDLE h;
	J jt;

	h = HeapCreate(0, size, 0);
	if(!h) return 0;
	jt = HeapAlloc(h, 0, sizeof(JST)+JTALIGNBDY-1);
	if(!jt)
	{
		HeapDestroy(h);
		return 0;
	}
 jt = (J)(((I)jt+JTALIGNBDY-1)&-JTALIGNBDY);  // force to SDRAM page boundary
	memset(jt,0,sizeof(JST));
	jt->heap = h;
	return jt;
}

int WINAPI DllMain (HINSTANCE hDLL, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
    case DLL_PROCESS_ATTACH:
		g_hinst = hDLL;
/*
		{
			SYSTEMTIME s;
			GetSystemTime(&s);
            if(s.wYear != 2000 || 11<s.wMonth)
			{
				MessageBox(0, "J.DLL beta test period has expired.", "J", MB_OK);
				return 0; 
			}
		}
*/
		getpath(0, modulepath);
		getpath(hDLL, dllpath);
		g_jt=heapinit(10000);
		if(!g_jt) R 0;
		if(!jtglobinit(g_jt)) {HeapDestroy(g_jt->heap); g_jt=0; R 0;};
		break;

    case DLL_THREAD_ATTACH:
      break;

    case DLL_THREAD_DETACH:
      break;

    case DLL_PROCESS_DETACH:
		if(g_jt) HeapDestroy(g_jt->heap);
		break;
  }
return TRUE;
}

J _stdcall JInit()
{
	JST* jt;

	jt=heapinit(1000000);
	if(!jt) R 0;
	if(!jtjinit2(jt,0,0))
	{
		HeapDestroy(jt->heap);
		R 0;
	};
	return jt;
}

// clean up at the end of a J instance
int _stdcall JFree(J jt)
{
	if(!jt) return 0;
#if !SY_WINCE
	dllquit(jt);  // clean up call dll
#endif
	if(jt->xep&&AN(jt->xep)){A *old=jt->tnextpushp; immex(jt->xep); fa(jt->xep); jt->xep=0; jt->jerr=0; jt->etxn=0; tpop(old); }
	HeapDestroy(jt->heap);
	return 0;
}

// previously in jwin32.c

#ifndef _JDLL
char dllpath[] = "";				    /* dll path is empty */
#endif

#ifdef _MAC
#ifdef _DEBUG
LONG _lcbExtraStack = 256*1024;	        /* more than 215k required in debug */
#else
LONG _lcbExtraStack = 200*1024;	        /* 175 fails */
#endif
#endif

#if !SY_WINCE
unsigned int getfileattr(char *p)
{
	return GetFileAttributesA(p);
}

int setfileattr(char*p, unsigned int x)
{
	return SetFileAttributesA(p, x);
}
#endif

F1(jtts){A z;D*x;SYSTEMTIME t;
 ASSERTMTV(w);
 GetLocalTime(&t);
 GA(z,FL,6,1,0); x=DAV(z);
 x[0]=t.wYear;
 x[1]=t.wMonth;
 x[2]=t.wDay;
 x[3]=t.wHour;
 x[4]=t.wMinute;
 x[5]=t.wSecond+(D)t.wMilliseconds/1000;
 R z;
}

