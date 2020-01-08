

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 8.00.0603 */
/* at Thu Apr 20 10:26:48 2017
 */
/* Compiler settings for jexe.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data
    VC __declspec() decoration level:
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __jexe_h__
#define __jexe_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */

#ifndef __IJEXEServer_FWD_DEFINED__
#define __IJEXEServer_FWD_DEFINED__
typedef interface IJEXEServer IJEXEServer;

#endif 	/* __IJEXEServer_FWD_DEFINED__ */


#ifndef __JEXEServer_FWD_DEFINED__
#define __JEXEServer_FWD_DEFINED__

#ifdef __cplusplus
typedef class JEXEServer JEXEServer;
#else
typedef struct JEXEServer JEXEServer;
#endif /* __cplusplus */

#endif 	/* __JEXEServer_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C" {
#endif



#ifndef __JEXEServerLib_LIBRARY_DEFINED__
#define __JEXEServerLib_LIBRARY_DEFINED__

/* library JEXEServerLib */
/* [custom][custom][helpstring][version][uuid] */



EXTERN_C const IID LIBID_JEXEServerLib;

#ifndef __IJEXEServer_INTERFACE_DEFINED__
#define __IJEXEServer_INTERFACE_DEFINED__

/* interface IJEXEServer */
/* [object][oleautomation][dual][helpstring][uuid] */


EXTERN_C const IID IID_IJEXEServer;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("21EB05E2-1AB3-11CF-A2AC-8FF70874C460")
IJEXEServer :
public IDispatch {
public:
  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Do(
    /* [in] */ BSTR input,
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Show(
    /* [in] */ long b,
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Log(
    /* [in] */ long b,
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsBusy(
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Break(
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Quit(
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Get(
    /* [in] */ BSTR jname,
    /* [out] */ VARIANT *v,
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Set(
    /* [in] */ BSTR jname,
    /* [in] */ VARIANT *v,
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetM(
    /* [in] */ BSTR jname,
    /* [out] */ long *jtype,
    /* [out] */ long *jrank,
    /* [out] */ long *jshape,
    /* [out] */ long *jdata,
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetM(
    /* [in] */ BSTR jname,
    /* [in] */ long *jtype,
    /* [in] */ long *jrank,
    /* [in] */ long *jshape,
    /* [in] */ long *jdata,
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ErrorText(
    /* [in] */ long error,
    /* [out] */ VARIANT *v,
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ErrorTextM(
    /* [in] */ long error,
    /* [out] */ long *text,
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clear(
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Transpose(
    /* [in] */ long b,
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ErrorTextB(
    /* [in] */ long error,
    /* [out] */ VARIANT *v,
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetB(
    /* [in] */ BSTR jname,
    /* [out] */ VARIANT *v,
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetB(
    /* [in] */ BSTR jname,
    /* [in] */ VARIANT *v,
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DoR(
    /* [in] */ BSTR input,
    /* [out] */ VARIANT *v,
    /* [retval][out] */ long *r) = 0;

  virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Int64R(
    /* [in] */ long b,
    /* [retval][out] */ long *r) = 0;

};


#else 	/* C style interface */

typedef struct IJEXEServerVtbl {
  BEGIN_INTERFACE

  HRESULT ( STDMETHODCALLTYPE *QueryInterface )(
    IJEXEServer * This,
    /* [in] */ REFIID riid,
    /* [annotation][iid_is][out] */
    _COM_Outptr_  void **ppvObject);

  ULONG ( STDMETHODCALLTYPE *AddRef )(
    IJEXEServer * This);

  ULONG ( STDMETHODCALLTYPE *Release )(
    IJEXEServer * This);

  HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )(
    IJEXEServer * This,
    /* [out] */ UINT *pctinfo);

  HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )(
    IJEXEServer * This,
    /* [in] */ UINT iTInfo,
    /* [in] */ LCID lcid,
    /* [out] */ ITypeInfo **ppTInfo);

  HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )(
    IJEXEServer * This,
    /* [in] */ REFIID riid,
    /* [size_is][in] */ LPOLESTR *rgszNames,
    /* [range][in] */ UINT cNames,
    /* [in] */ LCID lcid,
    /* [size_is][out] */ DISPID *rgDispId);

  /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )(
    IJEXEServer * This,
    /* [annotation][in] */
    _In_  DISPID dispIdMember,
    /* [annotation][in] */
    _In_  REFIID riid,
    /* [annotation][in] */
    _In_  LCID lcid,
    /* [annotation][in] */
    _In_  WORD wFlags,
    /* [annotation][out][in] */
    _In_  DISPPARAMS *pDispParams,
    /* [annotation][out] */
    _Out_opt_  VARIANT *pVarResult,
    /* [annotation][out] */
    _Out_opt_  EXCEPINFO *pExcepInfo,
    /* [annotation][out] */
    _Out_opt_  UINT *puArgErr);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Do )(
    IJEXEServer * This,
    /* [in] */ BSTR input,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Show )(
    IJEXEServer * This,
    /* [in] */ long b,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Log )(
    IJEXEServer * This,
    /* [in] */ long b,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsBusy )(
    IJEXEServer * This,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Break )(
    IJEXEServer * This,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Quit )(
    IJEXEServer * This,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Get )(
    IJEXEServer * This,
    /* [in] */ BSTR jname,
    /* [out] */ VARIANT *v,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Set )(
    IJEXEServer * This,
    /* [in] */ BSTR jname,
    /* [in] */ VARIANT *v,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetM )(
    IJEXEServer * This,
    /* [in] */ BSTR jname,
    /* [out] */ long *jtype,
    /* [out] */ long *jrank,
    /* [out] */ long *jshape,
    /* [out] */ long *jdata,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetM )(
    IJEXEServer * This,
    /* [in] */ BSTR jname,
    /* [in] */ long *jtype,
    /* [in] */ long *jrank,
    /* [in] */ long *jshape,
    /* [in] */ long *jdata,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ErrorText )(
    IJEXEServer * This,
    /* [in] */ long error,
    /* [out] */ VARIANT *v,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ErrorTextM )(
    IJEXEServer * This,
    /* [in] */ long error,
    /* [out] */ long *text,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clear )(
    IJEXEServer * This,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Transpose )(
    IJEXEServer * This,
    /* [in] */ long b,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ErrorTextB )(
    IJEXEServer * This,
    /* [in] */ long error,
    /* [out] */ VARIANT *v,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetB )(
    IJEXEServer * This,
    /* [in] */ BSTR jname,
    /* [out] */ VARIANT *v,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetB )(
    IJEXEServer * This,
    /* [in] */ BSTR jname,
    /* [in] */ VARIANT *v,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DoR )(
    IJEXEServer * This,
    /* [in] */ BSTR input,
    /* [out] */ VARIANT *v,
    /* [retval][out] */ long *r);

  /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Int64R )(
    IJEXEServer * This,
    /* [in] */ long b,
    /* [retval][out] */ long *r);

  END_INTERFACE
} IJEXEServerVtbl;

interface IJEXEServer {
  CONST_VTBL struct IJEXEServerVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IJEXEServer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )

#define IJEXEServer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) )

#define IJEXEServer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) )


#define IJEXEServer_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) )

#define IJEXEServer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) )

#define IJEXEServer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) )

#define IJEXEServer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) )


#define IJEXEServer_Do(This,input,r)	\
    ( (This)->lpVtbl -> Do(This,input,r) )

#define IJEXEServer_Show(This,b,r)	\
    ( (This)->lpVtbl -> Show(This,b,r) )

#define IJEXEServer_Log(This,b,r)	\
    ( (This)->lpVtbl -> Log(This,b,r) )

#define IJEXEServer_IsBusy(This,r)	\
    ( (This)->lpVtbl -> IsBusy(This,r) )

#define IJEXEServer_Break(This,r)	\
    ( (This)->lpVtbl -> Break(This,r) )

#define IJEXEServer_Quit(This,r)	\
    ( (This)->lpVtbl -> Quit(This,r) )

#define IJEXEServer_Get(This,jname,v,r)	\
    ( (This)->lpVtbl -> Get(This,jname,v,r) )

#define IJEXEServer_Set(This,jname,v,r)	\
    ( (This)->lpVtbl -> Set(This,jname,v,r) )

#define IJEXEServer_GetM(This,jname,jtype,jrank,jshape,jdata,r)	\
    ( (This)->lpVtbl -> GetM(This,jname,jtype,jrank,jshape,jdata,r) )

#define IJEXEServer_SetM(This,jname,jtype,jrank,jshape,jdata,r)	\
    ( (This)->lpVtbl -> SetM(This,jname,jtype,jrank,jshape,jdata,r) )

#define IJEXEServer_ErrorText(This,error,v,r)	\
    ( (This)->lpVtbl -> ErrorText(This,error,v,r) )

#define IJEXEServer_ErrorTextM(This,error,text,r)	\
    ( (This)->lpVtbl -> ErrorTextM(This,error,text,r) )

#define IJEXEServer_Clear(This,r)	\
    ( (This)->lpVtbl -> Clear(This,r) )

#define IJEXEServer_Transpose(This,b,r)	\
    ( (This)->lpVtbl -> Transpose(This,b,r) )

#define IJEXEServer_ErrorTextB(This,error,v,r)	\
    ( (This)->lpVtbl -> ErrorTextB(This,error,v,r) )

#define IJEXEServer_GetB(This,jname,v,r)	\
    ( (This)->lpVtbl -> GetB(This,jname,v,r) )

#define IJEXEServer_SetB(This,jname,v,r)	\
    ( (This)->lpVtbl -> SetB(This,jname,v,r) )

#define IJEXEServer_DoR(This,input,v,r)	\
    ( (This)->lpVtbl -> DoR(This,input,v,r) )

#define IJEXEServer_Int64R(This,b,r)	\
    ( (This)->lpVtbl -> Int64R(This,b,r) )

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IJEXEServer_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_JEXEServer;

#ifdef __cplusplus

class DECLSPEC_UUID("21EB05E0-1AB3-11CF-A2AC-8FF70874C460")
  JEXEServer;
#endif
#endif /* __JEXEServerLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


