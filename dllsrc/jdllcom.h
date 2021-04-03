/* Copyright 1990-2000, Jsoftware Inc.  All rights reserved. */
/* Licensed use only. Any other use is in violation of copyright. */
/*                                                                         */
/* included in both J.EXE and J.dll                                        */

#ifndef _JCOM_H_
#define _JCOM_H_

#include <windows.h>
#include <ole2.h>
#include <ole2ver.h>

//Type for an object-destroyed callback
typedef void (*PFNDESTROYED)(void);
typedef LPVOID * PPVOID;

// HRESULT errors
#define EHRSINGLE	(MAKE_HRESULT(S_FALSE, FACILITY_ITF, 0x201))

#ifdef _JDLL
#include "jdll.h"
#define SERVERCLASS		IJDLLServer
#else
#include "jexe.h"
#define SERVERCLASS		IJEXEServer
#endif

void setguids();
int reg(BOOL set, LPSTR keys);

extern char jmodule[];
extern char jclass[];
extern char jversion[];
extern GUID jclsid;
extern GUID jlibid;
extern GUID jiid;
extern "C" HINSTANCE g_hinst;

class CJServer : public SERVERCLASS
    {
    protected:
        ULONG           m_cRef;             //Object reference count
        LPUNKNOWN       m_pUnkOuter;        //Controlling unknown
        PFNDESTROYED    m_pfnDestroy;       //To call on closure
        ITypeInfo      *m_pITINeutral;      //Type information
		void*			m_pjst;				// J instance data

    public:
        CJServer(LPUNKNOWN, PFNDESTROYED);
        ~CJServer(void);
        BOOL         Init(void);
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IDispatch members
        STDMETHODIMP GetTypeInfoCount(UINT *);
        STDMETHODIMP GetTypeInfo(UINT, LCID, ITypeInfo **);
        STDMETHODIMP GetIDsOfNames(REFIID, OLECHAR **, UINT, LCID, DISPID *);
        STDMETHODIMP Invoke(DISPID, REFIID, LCID, WORD, DISPPARAMS *, VARIANT *, EXCEPINFO *, UINT *);

        //IJServer functions
        STDMETHODIMP Do(BSTR, long*);
        STDMETHODIMP Break(long*);
        STDMETHODIMP IsBusy(long*);
        STDMETHODIMP Get(BSTR, VARIANT*, long*);
        STDMETHODIMP Set(BSTR, VARIANT*, long*);
        STDMETHODIMP GetM(BSTR, long*, long*, long*, long*, long*);
        STDMETHODIMP SetM(BSTR, long*, long*, long*, long*, long*);
        STDMETHODIMP ErrorText(long, VARIANT*, long*);
        STDMETHODIMP ErrorTextM(long, long*, long*);
        STDMETHODIMP Clear(long*);
        STDMETHODIMP Show(long, long*);
        STDMETHODIMP Log(long, long*);
        STDMETHODIMP Transpose(long, long*);
        STDMETHODIMP Quit(long*);
        STDMETHODIMP ErrorTextB(long, VARIANT*, long*);
        STDMETHODIMP GetB(BSTR, VARIANT*, long*);
        STDMETHODIMP SetB(BSTR, VARIANT*, long*);
        STDMETHODIMP DoR(BSTR, VARIANT*, long*);
        STDMETHODIMP Int64R(long, long*);
    };

void ObjectDestroyed(void);

class CJServerFactory : public IClassFactory
    {
    protected:
        ULONG           m_cRef;

    public:
        CJServerFactory(void);
        ~CJServerFactory(void);

        //IUnknown members
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IClassFactory members
        STDMETHODIMP     CreateInstance(LPUNKNOWN, REFIID, PPVOID);
        STDMETHODIMP     LockServer(BOOL);
    };

#endif //_JCOM_H_
