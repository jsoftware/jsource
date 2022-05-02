/* Copyright (c) 1990-2022, Iverson Software Inc.  All rights reserved.    */
/* Licensed to Jsoftware Inc.                                              */
/*                                                                         */
// included in both JEXE and JDLL servers

// #undef _UNICODE
#include "windows.h"
#include "jdllcom.h"
#include "jdlltype.h"
#define EVLENGTH        9

#ifdef _JDLL
void showapp(long b){};
#else
// JEXEServer untested !!!
// #include "..\smsrc\isi.h"
extern "C" void *jt;
static int displayflag = 0;
void showapp(long b){};
void shutdown(){};
extern BOOL quitflag;
#endif

extern "C"
{
	int _stdcall JFree(void* jt);
	void* _stdcall JInit();
	long runit(WCHAR* p, BOOL f);
	int uniflag = -1;	// -1 not set (unicode), 0 ansi, 1 unicode
//	HINSTANCE g_hinst;
// #ifdef _JDLL
	void touni(char* src, WCHAR* sink);
	void toasc(WCHAR* src, LPSTR sink);
// #endif
}

#ifdef _JDLL
extern "C"
{
	JDoType			JDo;
	JBreakType		JBreak;
	JIsBusyType		JIsBusy;
	JGetType		JGet;
	JSetType		JSet;
	JGetMType		JGetM;
	JSetMType		JSetM;
	JErrorTextType	JErrorText;
	JErrorTextMType	JErrorTextM;
	JClearType		JClear;
	JShowType		JShow;
	JLogType		JLog;
	JQuitType		JQuit;
	JTransposeType	JTranspose;
	JErrorTextBType	JErrorTextB;
	JGetBType		JGetB;
	JSetBType		JSetB;
	JDoRType		JDoR;
	JInt64RType	JInt64R;
}
#else
	JDoType			*JDo;
	JBreakType		*JBreak;
	JIsBusyType		*JIsBusy;
	JGetType		*JGet;
	JSetType		*JSet;
	JGetMType		*JGetM;
	JSetMType		*JSetM;
	JErrorTextType	*JErrorText;
	JErrorTextMType	*JErrorTextM;
	JClearType		*JClear;
	JShowType		*JShow;
	JLogType		*JLog;
	JQuitType		*JQuit;
	JTransposeType	*JTranspose;
	JErrorTextBType	*JErrorTextB;
	JGetBType		*JGetB;
	JSetBType		*JSetB;
	JDoRType		*JDoR;
	JInt64RType	*JInt64R;
#endif

char jclass[100] = "";
char jversion[100] = "3";

GUID jclsid;
GUID jlibid;
GUID jiid;

void setguids()
{
	char d[50];
	WCHAR duni[50];
	char* p; char* q;
	char match[]="Class={";
	HRSRC h= FindResourceA(g_hinst, (LPCSTR)MAKEINTRESOURCE(1), "typelib");
	p=(char*)LockResource(LoadResource(g_hinst, h));
	q=p+SizeofResource(g_hinst, h);
	while(p<q)
	{
		p=(char*)memchr(p,match[0],q-p);
		if(!p) return; // bad typelib
		if(!memcmp(p, match, strlen(match))) break;
		++p;
	}
	p+=strlen(match);	// start of class name
	q=strchr(p, '}');	// end of class name
	memcpy(jclass, p, q-p);
	jclass[p-q]=0;
	p=q+2;				// start of version
	q=strchr(p, '}');	// end of version
	memcpy(jversion, p, q-p);
	jversion[q-p]=0;
	p=q+1;				// { at start of CLSD
	q=strchr(p, '}');	// } at end of CLSID
	memcpy(d,p,1+q-p);
	d[1+q-p]=0;
	touni(d, duni);
    CLSIDFromString(duni, &jclsid);
	jlibid=jclsid;
	jiid=jclsid;
	jlibid.Data1+=1;
	jiid.Data1+=2;
}

int toutf8n(WCHAR* src, LPSTR snk, int snklen)
{
  LPSTR p;

  if(!uniflag) {
    p = (LPSTR)src;
    if((int)strlen(p) > snklen) return EVLENGTH;
    strcpy(snk, p);
    return 0;
  }
  if (snklen<WideCharToMultiByte(CP_UTF8,0,src,1+(int)wcslen(src),0,0,0,0)) return EVLENGTH;
  WideCharToMultiByte(CP_UTF8,0,src,1+(int)wcslen(src),snk,snklen,0,0);
  return 0;
}

CJServer::CJServer(LPUNKNOWN pUnkOuter, PFNDESTROYED pfnDestroy)
{
    m_cRef=0;
    m_pUnkOuter=pUnkOuter;
    m_pfnDestroy=pfnDestroy;
    m_pITINeutral=NULL;
#ifndef _JDLL
	m_pjst=jt; // J instance data
#endif

    return;
}

CJServer::~CJServer(void)
{
	if (NULL!=m_pITINeutral)
	{
		m_pITINeutral->Release();
		m_pITINeutral=NULL;
	}		

#ifdef _JDLL
	JFree(m_pjst); // free J instance data
	m_pjst = 0;
#endif

    return;
}

STDMETHODIMP CJServer::QueryInterface(REFIID riid, PPVOID ppv)
{
    *ppv=NULL;

    if (IID_IUnknown==riid || jiid==riid
        || IID_IDispatch==riid)
        *ppv=this;

    if (NULL!=*ppv)
    {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
    }
    return ResultFromScode(E_NOINTERFACE);
}

STDMETHODIMP_(ULONG) CJServer::AddRef(void)
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CJServer::Release(void)
{
    if (0L!=--m_cRef)
        return m_cRef;

    if (NULL!=m_pfnDestroy)
        (*m_pfnDestroy)();

    delete this;
    return 0L;
}

STDMETHODIMP CJServer::GetTypeInfoCount(UINT *pctInfo)
{
    *pctInfo=1;
    return NOERROR;
}

STDMETHODIMP CJServer::GetTypeInfo(UINT itInfo, LCID lcid, ITypeInfo **ppITypeInfo)
{
    HRESULT     hr;
    ITypeLib   *pITypeLib;
    ITypeInfo **ppITI=NULL;

    if (0!=itInfo) return TYPE_E_ELEMENTNOTFOUND;

    if (NULL==ppITypeInfo) return E_POINTER;

    *ppITypeInfo=NULL;
    switch (PRIMARYLANGID(lcid))
    {
        case LANG_NEUTRAL:
        case LANG_ENGLISH:
            ppITI=&m_pITINeutral;
            break;

        default:
            return DISP_E_UNKNOWNLCID;
    }

    //Load a type lib if we don't have the information already.
    if (NULL==*ppITI)
    {
        hr=LoadRegTypeLib(jlibid, atoi(jversion), 0, PRIMARYLANGID(lcid), &pITypeLib);
        if (FAILED(hr)) return hr;

        //Got the type lib, get type info for the interface we want
        hr=pITypeLib->GetTypeInfoOfGuid(jiid, ppITI);
        pITypeLib->Release();
        if (FAILED(hr)) return hr;
	}

	// the type library is still loaded since we have an ITypeInfo from it.
    (*ppITI)->AddRef();
    *ppITypeInfo=*ppITI;
    return NOERROR;
}

STDMETHODIMP CJServer::GetIDsOfNames(REFIID riid
    , OLECHAR **rgszNames, UINT cNames, LCID lcid, DISPID *rgDispID)
{
    HRESULT     hr;
    ITypeInfo  *pTI;

    if (IID_NULL!=riid)
        return ResultFromScode(DISP_E_UNKNOWNINTERFACE);

    //Get the right ITypeInfo for lcid.
    hr=GetTypeInfo(0, lcid, &pTI);
    if (SUCCEEDED(hr))
    {
        hr=DispGetIDsOfNames(pTI, rgszNames, cNames, rgDispID);
        pTI->Release();
    }
    return hr;
}

STDMETHODIMP CJServer::Invoke(DISPID dispID, REFIID riid
    , LCID lcid, unsigned short wFlags, DISPPARAMS *pDispParams
    , VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
    HRESULT     hr;
    ITypeInfo  *pTI;
    LANGID      langID=PRIMARYLANGID(lcid);

    if (IID_NULL!=riid) return DISP_E_UNKNOWNINTERFACE;
    hr=GetTypeInfo(0, lcid, &pTI);
    if (FAILED(hr)) return hr;

    //This is exactly what DispInvoke does--so skip the overhead.
    hr=pTI->Invoke((SERVERCLASS *)this, dispID, wFlags
        , pDispParams, pVarResult, pExcepInfo, puArgErr);

    //Exception handling is done within ITypeInfo::Invoke
    pTI->Release();
    return hr;
}

#ifdef _JDLL

// JDLLServer methods
BOOL CJServer::Init(void)
{
    LPUNKNOWN       pIUnknown=this;

    if (NULL!=m_pUnkOuter) pIUnknown=m_pUnkOuter;
	m_pjst = JInit();
	return m_pjst ? 1 : 0;
}

STDMETHODIMP CJServer::Do(BSTR input, long *pr)
{
	char line[1000];

	*pr = toutf8n(input, line, sizeof line);
	if(*pr) return NOERROR;
	*pr = JDo(m_pjst, line);
    return NOERROR;
}

STDMETHODIMP CJServer::DoR(BSTR input, VARIANT* v, long *pr)
{
	char line[1000];

	*pr = toutf8n(input, line, sizeof line);
	if(*pr) return NOERROR;
	*pr = JDoR(m_pjst, line, v);
    return NOERROR;
}


STDMETHODIMP CJServer::Break(long *pr)
{
	*pr = JBreak(m_pjst);
    return NOERROR;
}

STDMETHODIMP CJServer::IsBusy(long *pr)
{
	*pr = 0;	// no kblockstate in dll
    return NOERROR;
}

STDMETHODIMP CJServer::GetM(BSTR name, long* jtype, long* jrank, long* jshape, long* jdata, long *pr)
{
	char aname[256];

	*pr = toutf8n(name, aname, sizeof aname);
	if(*pr) return NOERROR;
	*pr = JGetM(m_pjst, aname, jtype, jrank, jshape, jdata);
    return NOERROR;
}

STDMETHODIMP CJServer::SetM(BSTR name, long* jtype, long* jrank, long* jshape, long* jdata, long *pr)
{
	char aname[256];

	*pr = toutf8n(name, aname, sizeof aname);
	if(*pr) return NOERROR;
	*pr = JSetM(m_pjst, aname, jtype, jrank, jshape, jdata);
    return NOERROR;
}

STDMETHODIMP CJServer::ErrorTextM(long ec, long* text, long* pr)
{
	*pr = JErrorTextM(m_pjst, ec, text);
    return NOERROR;			 
}

#else

#define EDCEXE 0
// JEXEServer methods
VARIANT* outputvariant;

BOOL CJServer::Init(void)
{
    LPUNKNOWN       pIUnknown=this;

    if (NULL!=m_pUnkOuter) pIUnknown=m_pUnkOuter;
	return 1;
}

STDMETHODIMP CJServer::Do(BSTR input, long *pr)
{
//	*pr = runit(input, !displayflag);

  char line[1000];
  *pr = toutf8n(input, line, sizeof line);
  if(*pr) return NOERROR;
	*pr = JDo(m_pjst, line);

    return NOERROR;
}

STDMETHODIMP CJServer::DoR(BSTR input, VARIANT* v, long* pr)
{
	outputvariant=v;	// global kludge to capture output
	Do(input, pr);
	outputvariant = 0;	// turn off capture
    return NOERROR;
}


STDMETHODIMP CJServer::Break(long *pr)
{
    return NOERROR;
}

STDMETHODIMP CJServer::IsBusy(long *pr)
{
	*pr = 0;
    return NOERROR;
}

STDMETHODIMP CJServer::GetM(BSTR name, long* jtype, long* jrank, long* jshape, long* jdata, long *pr)
{
	*pr = EDCEXE;
    return NOERROR;
}

STDMETHODIMP CJServer::SetM(BSTR name, long* jtype, long* jrank, long* jshape, long* jdata, long *pr)
{
	*pr = EDCEXE;
    return NOERROR;
}

STDMETHODIMP CJServer::ErrorTextM(long ec, long* text, long* pr)
{
	*pr = EDCEXE;
    return NOERROR;			 
}

#endif // _JDLL

// following methods are identical for JEXE and JDLL
STDMETHODIMP CJServer::Get(BSTR name, VARIANT* v, long *pr)
{
	char aname[256];

	*pr = toutf8n(name, aname, sizeof aname);
	if(*pr) return NOERROR;
	*pr = JGet(m_pjst, aname, v);
    return NOERROR;
}

STDMETHODIMP CJServer::Set(BSTR name, VARIANT* v, long *pr)
{
	char aname[256];

	*pr = toutf8n(name, aname, sizeof aname);
	if(*pr) return NOERROR;
	*pr = JSet(m_pjst, aname, v);
    return NOERROR;
}

STDMETHODIMP CJServer::GetB(BSTR name, VARIANT* v, long *pr)
{
	char aname[256];

	*pr = toutf8n(name, aname, sizeof aname);
	if(*pr) return NOERROR;
	*pr = JGetB(m_pjst, aname, v);
    return NOERROR;
}

STDMETHODIMP CJServer::SetB(BSTR name, VARIANT* v, long *pr)
{
	char aname[256];

	*pr = toutf8n(name, aname, sizeof aname);
	if(*pr) return NOERROR;
	*pr = JSetB(m_pjst, aname, v);
    return NOERROR;
}

STDMETHODIMP CJServer::Clear(long* pr)
{
	*pr = JClear(m_pjst);
    return NOERROR;
}

STDMETHODIMP CJServer::ErrorText(long ec, VARIANT* v, long* pr)
{
	*pr = JErrorText(m_pjst, ec, v);
    return NOERROR;
}

STDMETHODIMP CJServer::ErrorTextB(long ec, VARIANT* v, long* pr)
{
	*pr = JErrorTextB(m_pjst, ec, v);
    return NOERROR;
}

STDMETHODIMP CJServer::Transpose(long b, long* pr)
{
	*pr = JTranspose(m_pjst, b);
    return NOERROR;
}

STDMETHODIMP CJServer::Show(long b, long* pr)
{
	showapp(b);
	*pr = 0;
    return NOERROR;
}

STDMETHODIMP CJServer::Log(long b, long* pr)
{
#ifndef _JDLL
	displayflag= b;
#endif
	*pr = 0;
    return NOERROR;
}

STDMETHODIMP CJServer::Quit(long* pr)
{
#ifndef _JDLL
	quitflag = 1;
#endif
	*pr = 0;
    return NOERROR;
}

STDMETHODIMP CJServer::Int64R(long b, long* pr)
{
	*pr = JInt64R(m_pjst, b);
    return NOERROR;
}

int SetKeyAndNamedValue(BOOL set, LPSTR keys, LPSTR szK, LPSTR szSubkey, LPSTR szV, LPSTR name)
{
    HKEY hKey; long r; char szk[256];

	strcpy(szk, szK);
    if (szSubkey)
    {
        strcat(szk, "\\");
        strcat(szk, szSubkey);
    }
	strcat(keys, "\t");
	strcat(keys, szk);
	if(!set) return 0;

	r=RegCreateKeyExA(HKEY_CLASSES_ROOT,szk,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
	if(ERROR_SUCCESS!=r) return r;

    if(szV)
	{
        r=RegSetValueExA(hKey, name, 0, REG_SZ, (BYTE *)szV, (UINT)((strlen(szV)+1)));
		if(ERROR_SUCCESS!=r) return r;
	}

    RegCloseKey(hKey);
    return 0;
}

// return 0 success or winerror.h error code - probably no access
int SetKeyAndValue(BOOL set, LPSTR keys, LPSTR szK, LPSTR szSubkey, LPSTR szV)
{
    HKEY hKey; char szk[256]; long r;

	strcpy(szk, szK);
    if (szSubkey)
    {
        strcat(szk, "\\");
        strcat(szk, szSubkey);
    }
	strcat(keys, "\t");
	strcat(keys, szk);
	if(!set) return 0;

	r=RegCreateKeyExA(HKEY_CLASSES_ROOT,szk,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
    if(ERROR_SUCCESS!=r) return r;

    if (szV)
    {
		r=RegSetValueExA(hKey, NULL, 0, REG_SZ, (BYTE *)szV, (UINT)((strlen(szV)+1)));
        if(ERROR_SUCCESS!=r) return r;
    }
    RegCloseKey(hKey);
    return 0;
}

#define INTERCLSID	"{00020424-0000-0000-C000-000000000046}"
#define setpath(p,t)	strcpy(path,p);strcat(path,t);

// returns 0 or winerror.h error code
int reg(BOOL set, LPSTR keys)
{
    WCHAR szwork[128];
    char szID[128];
    char szCLSID[128];
    char szLIBID[128];
	char iid[128];
    char szLIBTYPELIBID[128];
	char szdir[512];
	char path[256];
	char inter[256];
	char typetext[256];
	char progidcurtext[256];
	char progidtext[256];
	LPSTR p;
	char progidcur[100];
	char module[_MAX_PATH];
	int r;

    GetModuleFileNameA(g_hinst, module, sizeof(module));

#ifdef _JDLL
	char progid[100] = "JDLLServer";
	char producttext[] = "J DLL Server ";
#else
	char progid[100] = "JEXEServer";
	char producttext[] = "J EXE Server ";
#endif

	setguids();
	strcat(progid, jclass);
	strcpy(progidcur, progid);
	strcat(progidcur, ".");
	strcat(progidcur, jversion);

	StringFromGUID2(jclsid, szwork, 128);
	toasc(szwork, szID);
    strcpy(szCLSID, "CLSID\\");
    strcat(szCLSID, szID);

    StringFromGUID2(jlibid, szwork, 128);
	toasc(szwork, szLIBID);
    strcpy(szLIBTYPELIBID, "TypeLib\\");
    strcat(szLIBTYPELIBID, szLIBID);

    StringFromGUID2(jiid, szwork, 128);
	toasc(szwork, iid);
	strcpy(inter, "Interface\\");
	strcat(inter, iid);

	strcpy(szdir, module);
	p=strrchr(szdir, '\\');
	*p=0;
	keys[0]=0;

	strcpy(progidtext, "Jsoftware : ");
	strcat(progidtext, producttext);
	strcat(progidtext, jclass);

	strcpy(progidcurtext, progidtext);
	strcat(progidcurtext, " (version ");
	strcat(progidcurtext, jversion);
	strcat(progidcurtext, ")");

//HKEY_CLASSES_ROOT\JServer.Object = J Server
    r=SetKeyAndValue(set, keys, progid, 0, progidtext);
	if(r) return r;

//HKEY_CLASSES_ROOT\JServer.Object\CLSID = {21EB05E0-1AB3-11cf-A2AC-8FF70874C460}
    setpath(progid,"\\CLSID");
	r=SetKeyAndValue(set, keys, path, 0, szID);
	if(r) return r;

//HKEY_CLASSES_ROOT\JServer.Object\CurVer = JServer.Object.1
	setpath(progid,"\\CurVer");
	r=SetKeyAndValue(set, keys, path, 0, progidcur);		
	if(r) return r;

//HKEY_CLASSES_ROOT\JServer.Object\NotInsertable
	setpath(progid,"\\NotInsertable");
	r=SetKeyAndValue(set, keys, path, 0, 0);
	if(r) return r;


//HKEY_CLASSES_ROOT\JServer.Object.1 = J Server (Ver 1.0)
	r=SetKeyAndValue(set, keys, progidcur, 0, progidcurtext);
	if(r) return r;

//HKEY_CLASSES_ROOT\JServer.Object.1\CLSID = {21EB05E0-1AB3-11cf-A2AC-8FF70874C460}
	setpath(progidcur,"\\CLSID");
	r=SetKeyAndValue(set, keys, path, 0, szID);
	if(r) return r;

//HKEY_CLASSES_ROOT\JServer.Object.1\NotInsertable
	setpath(progidcur, "\\NotInsertable");
	r=SetKeyAndValue(set, keys, path, 0, 0);
	if(r) return r;

//HKEY_CLASSES_ROOT\CLSID\{21EB05E0-1AB3-11cf-A2AC-8FF70874C460} = J Server (Ver 1.0)
	r=SetKeyAndValue(set, keys, szCLSID, 0, progidcurtext);
	if(r) return r;

#ifdef _JDLL
//HKEY_CLASSES_ROOT\CLSID\{21EB05E0-1AB3-11cf-A2AC-8FF70874C460}\InprocServer32 = d:\dev\jcom\jcom\windebug\jserver.dll
	r=SetKeyAndValue(set, keys, szCLSID, (char*)"InprocServer32", module);
	if(r) return r;

	r=SetKeyAndNamedValue(set, keys, szCLSID, (char*)"InprocServer32", (char*)"Apartment", (char*)"ThreadingModel");
	if(r) return r;
#else
//HKEY_CLASSES_ROOT\CLSID\{21EB05E0-1AB3-11cf-A2AC-8FF70874C460}\LocalServer32 = d:\dev\jcom\jcom\windebug\jserver.dll
	r=SetKeyAndValue(set, keys, szCLSID, (char*)"LocalServer32", module);
	if(r) return r;

//HKEY_CLASSES_ROOT\CLSID\{21EB05E0-1AB3-11cf-A2AC-8FF70874C460}\LocalHandler32 = ole32.dll
	r=SetKeyAndValue(set, keys, szCLSID, (char*)"LocalHandler32", (char*)"ole32.dll");
	if(r) return r;
#endif

//HKEY_CLASSES_ROOT\CLSID\{21EB05E0-1AB3-11cf-A2AC-8FF70874C460}\ProgID = JServer.Object.1
	r=SetKeyAndValue(set, keys, szCLSID, (char*)"ProgID", progidcur);
	if(r) return r;

//HKEY_CLASSES_ROOT\CLSID\{21EB05E0-1AB3-11cf-A2AC-8FF70874C460}\VersionIndependentProgID = JServer.Object
	r=SetKeyAndValue(set, keys, szCLSID, (char*)"VersionIndependentProgID", progid);
	if(r) return r;

//HKEY_CLASSES_ROOT\CLSID\{21EB05E0-1AB3-11cf-A2AC-8FF70874C460}\TypeLib = {21EB05E1-1AB3-11cf-A2AC-8FF70874C460}
	r=SetKeyAndValue(set, keys, szCLSID, (char*)"TypeLib", szLIBID);
	if(r) return r;

//HKEY_CLASSES_ROOT\CLSID\{21EB05E0-1AB3-11cf-A2AC-8FF70874C460}\Programmable
	r=SetKeyAndValue(set, keys, szCLSID, (char*)"Programmable", 0);
	if(r) return r;

//HKEY_CLASSES_ROOT\CLSID\{21EB05E0-1AB3-11cf-A2AC-8FF70874C460}\NotInsertable
	r=SetKeyAndValue(set, keys, szCLSID, (char*)"NotInsertable", 0);
	if(r) return r;

//HKEY_CLASSES_ROOT\TypeLib\{21EB05E1-1AB3-11cf-A2AC-8FF70874C460} = J Server Type Library
	strcpy(typetext, producttext);
	strcat(typetext, jclass);
	strcat(typetext, " Type Library");
	r=SetKeyAndValue(set, keys, szLIBTYPELIBID, 0, typetext);
	if(r) return r;

//HKEY_CLASSES_ROOT\TypeLib\{21EB05E1-1AB3-11cf-A2AC-8FF70874C460}\DIR = d:\dev\jcom
	r=SetKeyAndValue(set, keys, szLIBTYPELIBID, (char*)"DIR", szdir);
	if(r) return r;

//HKEY_CLASSES_ROOT\TypeLib\{21EB05E1-1AB3-11cf-A2AC-8FF70874C460}\HELPDIR = d:\dev\jcom\jcomhelp
	r=SetKeyAndValue(set, keys, szLIBTYPELIBID, (char*)"HELPDIR", szdir);
	if(r) return r;

//HKEY_CLASSES_ROOT\TypeLib\{21EB05E1-1AB3-11cf-A2AC-8FF70874C460}\1.0 = J Server (Ver 1.0) Type Library
	setpath(jversion, ".0");
	strcpy(typetext, producttext);
	strcat(typetext, jclass);
	strcat(typetext, " (version ");
	strcat(typetext, jversion);
	strcat(typetext, ") Type Library");
	r=SetKeyAndValue(set, keys, szLIBTYPELIBID, path, typetext);
	if(r) return r;

// note: key explicity added so we will be able to delete it
//HKEY_CLASSES_ROOT\TypeLib\{21EB05E1-1AB3-11cf-A2AC-8FF70874C460}\1.0\0
	setpath(jversion, ".0\\0");
	r=SetKeyAndValue(set, keys, szLIBTYPELIBID, path, module);
	if(r) return r;

//HKEY_CLASSES_ROOT\TypeLib\{21EB05E1-1AB3-11cf-A2AC-8FF70874C460}\1.0\0\win32 = d:\dev\jcom\jserver.tlb
	setpath(jversion, ".0\\0\\win32");
	r=SetKeyAndValue(set, keys, szLIBTYPELIBID, path, module);
	if(r) return r;

// interface must be registered for VB <set js as New IJEXEServer>

//HKEY_CLASSES_ROOT\Interface\{21EB05E2-1AB3-11cf-A2AC-8FF70874C460} = IJEXEServer
#ifdef _JDLL
	setpath("IJDLLServer", jclass);
#else	
	setpath("IJEXEServer", jclass);
#endif
	r=SetKeyAndValue(set, keys, inter, 0, path);
	if(r) return r;

//HKEY_CLASSES_ROOT\Interface\{21EB05E2-1AB3-11cf-A2AC-8FF70874C460}\ProxyStubClsid = {00020424-0000-0000-C000-000000000046}
	r=SetKeyAndValue(set, keys, inter, (char*)"ProxyStubClsid", (char*)INTERCLSID);
	if(r) return r;

//HKEY_CLASSES_ROOT\Interface\{21EB05E2-1AB3-11cf-A2AC-8FF70874C460}\ProxyStubClsid32 = {00020424-0000-0000-C000-000000000046}
	r=SetKeyAndValue(set, keys, inter, (char*)"ProxyStubClsid32", (char*)INTERCLSID);
	if(r) return r;

//HKEY_CLASSES_ROOT\Interface\{21EB05E2-1AB3-11cf-A2AC-8FF70874C460}\TypeLib = {21EB05E1-1AB3-11cf-A2AC-8FF70874C460}
	r=SetKeyAndValue(set, keys, inter, (char*)"TypeLib", szLIBID);
	if(r) return r;
	return 0;
}

static ULONG	g_cObj=0;		// object count
static ULONG	g_cLock=0;		// server locks

void ObjectDestroyed(void)
{
    g_cObj--;
#ifndef _JDLL
	if(!g_cObj && !g_cLock && quitflag)	shutdown();
#endif
    return;
}


CJServerFactory::CJServerFactory(void)
{
    m_cRef=0L;
    return;
}

CJServerFactory::~CJServerFactory(void)
{
    return;
}

STDMETHODIMP CJServerFactory::QueryInterface(REFIID riid, PPVOID ppv)
{
    *ppv=NULL;
    if (IID_IUnknown==riid || IID_IClassFactory==riid) *ppv=this;

    if (NULL!=*ppv)
    {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
    }
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CJServerFactory::AddRef(void)
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CJServerFactory::Release(void)
{
    if (0!=--m_cRef) return m_cRef;
    delete this;
    return 0;
}

STDMETHODIMP CJServerFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, PPVOID ppvObj)
{
    CJServer* pObj;
    HRESULT hr;

    *ppvObj=NULL;
    if (NULL!=pUnkOuter && IID_IUnknown!=riid)
        return ResultFromScode(CLASS_E_NOAGGREGATION);

#ifdef _JDLL
//! java doesn't seem to unload properly
//  repeated refresh of calc applet gets cocreate failure on odd times
//  but this change isn't right either
//  because it reconnects to the same dll
//  kludge java fix is to remove following g_cObj line
//	if(g_cObj) return MAKE_HRESULT(S_FALSE, FACILITY_ITF, EHRSINGLE);
#else
	if(g_cObj) return MAKE_HRESULT(S_FALSE, FACILITY_ITF, EHRSINGLE);
#endif
	pObj=new CJServer(pUnkOuter, ObjectDestroyed);
    if (NULL==pObj) return E_OUTOFMEMORY;
    if (!pObj->Init()) return E_OUTOFMEMORY;
    hr=pObj->QueryInterface(riid, ppvObj);
    if (FAILED(hr))
    	delete pObj;
    else
        g_cObj++;
    return hr;
}

STDMETHODIMP CJServerFactory::LockServer(BOOL fLock)
{
    if (fLock)
        g_cLock++;
    else
        g_cLock--;
    return NOERROR;
}

#ifdef _JDLL

HRESULT APIENTRY DllGetClassObject(REFCLSID rclsid, REFIID riid, PPVOID ppv)
{
    CJServerFactory *pBF;
    HRESULT         hr;
	
	setguids();
    if (jclsid!=rclsid) return E_FAIL;
    if (IID_IUnknown!=riid && IID_IClassFactory!=riid) return E_NOINTERFACE;
    pBF=new CJServerFactory();
    if (NULL==pBF) return E_OUTOFMEMORY;
    hr=pBF->QueryInterface(riid, ppv);
    if (FAILED(hr)) delete pBF;
    return hr;
}


STDAPI DllCanUnloadNow(void)
{
    SCODE   sc;

    sc=(0L==g_cObj && 0L==g_cLock) ? S_OK : S_FALSE;
    return ResultFromScode(sc);
}

STDAPI DllRegisterServer(void)
{
	char keys[2000];
	return reg(1, keys);
}

STDAPI DllUnregisterServer(void)
{
	char keys[2000];
	LPSTR p;

	reg(0, keys);	// collect the keys we register
	p = keys + strlen(keys);
	while(p>keys)
	{
		p=strrchr(keys, '\t');	
		*p=0;
	    int r=RegDeleteKeyA(HKEY_CLASSES_ROOT, p+1);
		if(ERROR_ACCESS_DENIED==r) return 1; // vista requires admin
	}
    return 0;
}

#else

void shutdown();

void toasc(WCHAR* src, LPSTR sink);

static DWORD	dwRegCO;
static BOOL coregflag=0;

BOOL quitflag=0;		


int registerserver(void)
{
	char keys[2000];
	return reg(1, keys);
}

int unregisterserver(void)
{
	char keys[2000];
	LPSTR p;

	reg(0, keys);	// collect the keys we register
	p = keys + strlen(keys);
	while(p>keys)
	{
		p=strrchr(keys, '\t');	
		*p=0;
	    int r=RegDeleteKeyA(HKEY_CLASSES_ROOT, p+1);
		if(ERROR_ACCESS_DENIED==r) return 1; // vista requires admin
	}
    return 0;
}

CJServerFactory* pFactory;

// initexeserver - embedding
BOOL initexeserver()
{
    HRESULT     hr;
	setguids();

	pFactory=new CJServerFactory();
    if (NULL==pFactory) return 0;
    pFactory->AddRef();	// we hold on to this till we quit

    // REGCLS_MULTIPLEUSE allows multiple users (or reconnections) to same task
	   hr=CoRegisterClassObject(jclsid, pFactory, CLSCTX_LOCAL_SERVER,
    	 REGCLS_SINGLEUSE, &dwRegCO);
	//	sometimes REGCLS_SINGLEUSE, sometimes REGCLS_MULTIPLEUSE
    if (FAILED(hr)) return FALSE;
	coregflag = 1;
    return TRUE;
}

void quitexeserver()
{
	if(!coregflag) return;
	CoRevokeClassObject(dwRegCO);
    pFactory->Release();
}

#endif
