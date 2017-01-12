

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Thu Jan 12 17:46:21 2017
 */
/* Compiler settings for jdll.odl:
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


#ifndef __jdllodl_h__
#define __jdllodl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IJDLLServer_FWD_DEFINED__
#define __IJDLLServer_FWD_DEFINED__
typedef interface IJDLLServer IJDLLServer;

#endif 	/* __IJDLLServer_FWD_DEFINED__ */


#ifndef __JDLLServer_FWD_DEFINED__
#define __JDLLServer_FWD_DEFINED__

#ifdef __cplusplus
typedef class JDLLServer JDLLServer;
#else
typedef struct JDLLServer JDLLServer;
#endif /* __cplusplus */

#endif 	/* __JDLLServer_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __JDLL_LIBRARY_DEFINED__
#define __JDLL_LIBRARY_DEFINED__

/* library JDLL */
/* [version][lcid][helpstring][uuid] */ 


EXTERN_C const IID LIBID_JDLL;

#ifndef __IJDLLServer_INTERFACE_DEFINED__
#define __IJDLLServer_INTERFACE_DEFINED__

/* interface IJDLLServer */
/* [object][dual][helpstring][uuid] */ 


EXTERN_C const IID IID_IJDLLServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("21EB05EC-1AB3-11cf-A2AC-8FF70874C460")
    IJDLLServer : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Do( 
            /* [in] */ BSTR input,
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Show( 
            /* [in] */ long b,
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Log( 
            /* [in] */ long b,
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsBusy( 
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Break( 
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Quit( 
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Get( 
            /* [in] */ BSTR jname,
            /* [out] */ VARIANT *v,
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Set( 
            /* [in] */ BSTR jname,
            /* [in] */ VARIANT *v,
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetM( 
            /* [in] */ BSTR jname,
            /* [out] */ long *jtype,
            /* [out] */ long *jrank,
            /* [out] */ long *jshape,
            /* [out] */ long *jdata,
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetM( 
            /* [in] */ BSTR jname,
            /* [in] */ long *jtype,
            /* [in] */ long *jrank,
            /* [in] */ long *jshape,
            /* [in] */ long *jdata,
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ErrorText( 
            /* [in] */ long error,
            /* [out] */ VARIANT *v,
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ErrorTextM( 
            /* [in] */ long error,
            /* [out] */ long *text,
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Clear( 
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Transpose( 
            /* [in] */ long b,
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ErrorTextB( 
            /* [in] */ long error,
            /* [out] */ VARIANT *v,
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetB( 
            /* [in] */ BSTR jname,
            /* [out] */ VARIANT *v,
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetB( 
            /* [in] */ BSTR jname,
            /* [in] */ VARIANT *v,
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DoR( 
            /* [in] */ BSTR input,
            /* [out] */ VARIANT *v,
            /* [retval][out] */ long *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Int64( 
            /* [in] */ long b,
            /* [retval][out] */ long *r) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IJDLLServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IJDLLServer * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IJDLLServer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IJDLLServer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IJDLLServer * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IJDLLServer * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IJDLLServer * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IJDLLServer * This,
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
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Do )( 
            IJDLLServer * This,
            /* [in] */ BSTR input,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Show )( 
            IJDLLServer * This,
            /* [in] */ long b,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Log )( 
            IJDLLServer * This,
            /* [in] */ long b,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *IsBusy )( 
            IJDLLServer * This,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Break )( 
            IJDLLServer * This,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Quit )( 
            IJDLLServer * This,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Get )( 
            IJDLLServer * This,
            /* [in] */ BSTR jname,
            /* [out] */ VARIANT *v,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Set )( 
            IJDLLServer * This,
            /* [in] */ BSTR jname,
            /* [in] */ VARIANT *v,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetM )( 
            IJDLLServer * This,
            /* [in] */ BSTR jname,
            /* [out] */ long *jtype,
            /* [out] */ long *jrank,
            /* [out] */ long *jshape,
            /* [out] */ long *jdata,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetM )( 
            IJDLLServer * This,
            /* [in] */ BSTR jname,
            /* [in] */ long *jtype,
            /* [in] */ long *jrank,
            /* [in] */ long *jshape,
            /* [in] */ long *jdata,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ErrorText )( 
            IJDLLServer * This,
            /* [in] */ long error,
            /* [out] */ VARIANT *v,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ErrorTextM )( 
            IJDLLServer * This,
            /* [in] */ long error,
            /* [out] */ long *text,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IJDLLServer * This,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Transpose )( 
            IJDLLServer * This,
            /* [in] */ long b,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ErrorTextB )( 
            IJDLLServer * This,
            /* [in] */ long error,
            /* [out] */ VARIANT *v,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetB )( 
            IJDLLServer * This,
            /* [in] */ BSTR jname,
            /* [out] */ VARIANT *v,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetB )( 
            IJDLLServer * This,
            /* [in] */ BSTR jname,
            /* [in] */ VARIANT *v,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *DoR )( 
            IJDLLServer * This,
            /* [in] */ BSTR input,
            /* [out] */ VARIANT *v,
            /* [retval][out] */ long *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Int64 )( 
            IJDLLServer * This,
            /* [in] */ long b,
            /* [retval][out] */ long *r);
        
        END_INTERFACE
    } IJDLLServerVtbl;

    interface IJDLLServer
    {
        CONST_VTBL struct IJDLLServerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IJDLLServer_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IJDLLServer_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IJDLLServer_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IJDLLServer_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IJDLLServer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IJDLLServer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IJDLLServer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IJDLLServer_Do(This,input,r)	\
    ( (This)->lpVtbl -> Do(This,input,r) ) 

#define IJDLLServer_Show(This,b,r)	\
    ( (This)->lpVtbl -> Show(This,b,r) ) 

#define IJDLLServer_Log(This,b,r)	\
    ( (This)->lpVtbl -> Log(This,b,r) ) 

#define IJDLLServer_IsBusy(This,r)	\
    ( (This)->lpVtbl -> IsBusy(This,r) ) 

#define IJDLLServer_Break(This,r)	\
    ( (This)->lpVtbl -> Break(This,r) ) 

#define IJDLLServer_Quit(This,r)	\
    ( (This)->lpVtbl -> Quit(This,r) ) 

#define IJDLLServer_Get(This,jname,v,r)	\
    ( (This)->lpVtbl -> Get(This,jname,v,r) ) 

#define IJDLLServer_Set(This,jname,v,r)	\
    ( (This)->lpVtbl -> Set(This,jname,v,r) ) 

#define IJDLLServer_GetM(This,jname,jtype,jrank,jshape,jdata,r)	\
    ( (This)->lpVtbl -> GetM(This,jname,jtype,jrank,jshape,jdata,r) ) 

#define IJDLLServer_SetM(This,jname,jtype,jrank,jshape,jdata,r)	\
    ( (This)->lpVtbl -> SetM(This,jname,jtype,jrank,jshape,jdata,r) ) 

#define IJDLLServer_ErrorText(This,error,v,r)	\
    ( (This)->lpVtbl -> ErrorText(This,error,v,r) ) 

#define IJDLLServer_ErrorTextM(This,error,text,r)	\
    ( (This)->lpVtbl -> ErrorTextM(This,error,text,r) ) 

#define IJDLLServer_Clear(This,r)	\
    ( (This)->lpVtbl -> Clear(This,r) ) 

#define IJDLLServer_Transpose(This,b,r)	\
    ( (This)->lpVtbl -> Transpose(This,b,r) ) 

#define IJDLLServer_ErrorTextB(This,error,v,r)	\
    ( (This)->lpVtbl -> ErrorTextB(This,error,v,r) ) 

#define IJDLLServer_GetB(This,jname,v,r)	\
    ( (This)->lpVtbl -> GetB(This,jname,v,r) ) 

#define IJDLLServer_SetB(This,jname,v,r)	\
    ( (This)->lpVtbl -> SetB(This,jname,v,r) ) 

#define IJDLLServer_DoR(This,input,v,r)	\
    ( (This)->lpVtbl -> DoR(This,input,v,r) ) 

#define IJDLLServer_Int64(This,b,r)	\
    ( (This)->lpVtbl -> Int64(This,b,r) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IJDLLServer_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_JDLLServer;

#ifdef __cplusplus

class DECLSPEC_UUID("21EB05EA-1AB3-11cf-A2AC-8FF70874C460")
JDLLServer;
#endif
#endif /* __JDLL_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


