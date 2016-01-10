/* Copyright 1990-2000, Jsoftware Inc.  All rights reserved. */
/* Licensed use only. Any other use is in violation of copyright. */
/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Jun 22 11:55:16 2000
 */
/* Compiler settings for default\jdll.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __jdll_h__
#define __jdll_h__

#ifdef __cplusplus
extern "C"{
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


/* header files for imported files */
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IJDLLServer_INTERFACE_DEFINED__
#define __IJDLLServer_INTERFACE_DEFINED__

/* interface IJDLLServer */
/* [oleautomation][dual][unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IJDLLServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("21EB05EC-1AB3-11cf-A2AC-8FF70874C460")
    IJDLLServer : public IDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Do( 
            /* [in] */ BSTR input,
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Show( 
            /* [in] */ long b,
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Log( 
            /* [in] */ long b,
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsBusy( 
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Break( 
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Quit( 
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Get( 
            /* [in] */ BSTR jname,
            /* [out] */ VARIANT __RPC_FAR *v,
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Set( 
            /* [in] */ BSTR jname,
            /* [in] */ VARIANT __RPC_FAR *v,
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetM( 
            /* [in] */ BSTR jname,
            /* [out] */ long __RPC_FAR *jtype,
            /* [out] */ long __RPC_FAR *jrank,
            /* [out] */ long __RPC_FAR *jshape,
            /* [out] */ long __RPC_FAR *jdata,
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetM( 
            /* [in] */ BSTR jname,
            /* [in] */ long __RPC_FAR *jtype,
            /* [in] */ long __RPC_FAR *jrank,
            /* [in] */ long __RPC_FAR *jshape,
            /* [in] */ long __RPC_FAR *jdata,
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ErrorText( 
            /* [in] */ long error,
            /* [out] */ VARIANT __RPC_FAR *v,
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ErrorTextM( 
            /* [in] */ long error,
            /* [out] */ long __RPC_FAR *text,
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Clear( 
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Transpose( 
            /* [in] */ long b,
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ErrorTextB( 
            /* [in] */ long error,
            /* [out] */ VARIANT __RPC_FAR *v,
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetB( 
            /* [in] */ BSTR jname,
            /* [out] */ VARIANT __RPC_FAR *v,
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetB( 
            /* [in] */ BSTR jname,
            /* [in] */ VARIANT __RPC_FAR *v,
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DoR( 
            /* [in] */ BSTR input,
            /* [out] */ VARIANT __RPC_FAR *v,
            /* [retval][out] */ long __RPC_FAR *r) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IJDLLServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IJDLLServer __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IJDLLServer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IJDLLServer __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Do )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ BSTR input,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Show )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ long b,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Log )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ long b,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsBusy )( 
            IJDLLServer __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Break )( 
            IJDLLServer __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Quit )( 
            IJDLLServer __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ BSTR jname,
            /* [out] */ VARIANT __RPC_FAR *v,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Set )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ BSTR jname,
            /* [in] */ VARIANT __RPC_FAR *v,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetM )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ BSTR jname,
            /* [out] */ long __RPC_FAR *jtype,
            /* [out] */ long __RPC_FAR *jrank,
            /* [out] */ long __RPC_FAR *jshape,
            /* [out] */ long __RPC_FAR *jdata,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetM )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ BSTR jname,
            /* [in] */ long __RPC_FAR *jtype,
            /* [in] */ long __RPC_FAR *jrank,
            /* [in] */ long __RPC_FAR *jshape,
            /* [in] */ long __RPC_FAR *jdata,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ErrorText )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ long error,
            /* [out] */ VARIANT __RPC_FAR *v,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ErrorTextM )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ long error,
            /* [out] */ long __RPC_FAR *text,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clear )( 
            IJDLLServer __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Transpose )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ long b,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ErrorTextB )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ long error,
            /* [out] */ VARIANT __RPC_FAR *v,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetB )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ BSTR jname,
            /* [out] */ VARIANT __RPC_FAR *v,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetB )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ BSTR jname,
            /* [in] */ VARIANT __RPC_FAR *v,
            /* [retval][out] */ long __RPC_FAR *r);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DoR )( 
            IJDLLServer __RPC_FAR * This,
            /* [in] */ BSTR input,
            /* [out] */ VARIANT __RPC_FAR *v,
            /* [retval][out] */ long __RPC_FAR *r);
        
        END_INTERFACE
    } IJDLLServerVtbl;

    interface IJDLLServer
    {
        CONST_VTBL struct IJDLLServerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IJDLLServer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IJDLLServer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IJDLLServer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IJDLLServer_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IJDLLServer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IJDLLServer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IJDLLServer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IJDLLServer_Do(This,input,r)	\
    (This)->lpVtbl -> Do(This,input,r)

#define IJDLLServer_Show(This,b,r)	\
    (This)->lpVtbl -> Show(This,b,r)

#define IJDLLServer_Log(This,b,r)	\
    (This)->lpVtbl -> Log(This,b,r)

#define IJDLLServer_IsBusy(This,r)	\
    (This)->lpVtbl -> IsBusy(This,r)

#define IJDLLServer_Break(This,r)	\
    (This)->lpVtbl -> Break(This,r)

#define IJDLLServer_Quit(This,r)	\
    (This)->lpVtbl -> Quit(This,r)

#define IJDLLServer_Get(This,jname,v,r)	\
    (This)->lpVtbl -> Get(This,jname,v,r)

#define IJDLLServer_Set(This,jname,v,r)	\
    (This)->lpVtbl -> Set(This,jname,v,r)

#define IJDLLServer_GetM(This,jname,jtype,jrank,jshape,jdata,r)	\
    (This)->lpVtbl -> GetM(This,jname,jtype,jrank,jshape,jdata,r)

#define IJDLLServer_SetM(This,jname,jtype,jrank,jshape,jdata,r)	\
    (This)->lpVtbl -> SetM(This,jname,jtype,jrank,jshape,jdata,r)

#define IJDLLServer_ErrorText(This,error,v,r)	\
    (This)->lpVtbl -> ErrorText(This,error,v,r)

#define IJDLLServer_ErrorTextM(This,error,text,r)	\
    (This)->lpVtbl -> ErrorTextM(This,error,text,r)

#define IJDLLServer_Clear(This,r)	\
    (This)->lpVtbl -> Clear(This,r)

#define IJDLLServer_Transpose(This,b,r)	\
    (This)->lpVtbl -> Transpose(This,b,r)

#define IJDLLServer_ErrorTextB(This,error,v,r)	\
    (This)->lpVtbl -> ErrorTextB(This,error,v,r)

#define IJDLLServer_GetB(This,jname,v,r)	\
    (This)->lpVtbl -> GetB(This,jname,v,r)

#define IJDLLServer_SetB(This,jname,v,r)	\
    (This)->lpVtbl -> SetB(This,jname,v,r)

#define IJDLLServer_DoR(This,input,v,r)	\
    (This)->lpVtbl -> DoR(This,input,v,r)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_Do_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [in] */ BSTR input,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_Do_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_Show_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [in] */ long b,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_Show_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_Log_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [in] */ long b,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_Log_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_IsBusy_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_IsBusy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_Break_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_Break_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_Quit_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_Quit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_Get_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [in] */ BSTR jname,
    /* [out] */ VARIANT __RPC_FAR *v,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_Get_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_Set_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [in] */ BSTR jname,
    /* [in] */ VARIANT __RPC_FAR *v,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_Set_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_GetM_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [in] */ BSTR jname,
    /* [out] */ long __RPC_FAR *jtype,
    /* [out] */ long __RPC_FAR *jrank,
    /* [out] */ long __RPC_FAR *jshape,
    /* [out] */ long __RPC_FAR *jdata,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_GetM_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_SetM_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [in] */ BSTR jname,
    /* [in] */ long __RPC_FAR *jtype,
    /* [in] */ long __RPC_FAR *jrank,
    /* [in] */ long __RPC_FAR *jshape,
    /* [in] */ long __RPC_FAR *jdata,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_SetM_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_ErrorText_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [in] */ long error,
    /* [out] */ VARIANT __RPC_FAR *v,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_ErrorText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_ErrorTextM_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [in] */ long error,
    /* [out] */ long __RPC_FAR *text,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_ErrorTextM_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_Clear_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_Clear_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_Transpose_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [in] */ long b,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_Transpose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_ErrorTextB_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [in] */ long error,
    /* [out] */ VARIANT __RPC_FAR *v,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_ErrorTextB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_GetB_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [in] */ BSTR jname,
    /* [out] */ VARIANT __RPC_FAR *v,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_GetB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_SetB_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [in] */ BSTR jname,
    /* [in] */ VARIANT __RPC_FAR *v,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_SetB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IJDLLServer_DoR_Proxy( 
    IJDLLServer __RPC_FAR * This,
    /* [in] */ BSTR input,
    /* [out] */ VARIANT __RPC_FAR *v,
    /* [retval][out] */ long __RPC_FAR *r);


void __RPC_STUB IJDLLServer_DoR_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IJDLLServer_INTERFACE_DEFINED__ */



#ifndef __JDLLServerLib_LIBRARY_DEFINED__
#define __JDLLServerLib_LIBRARY_DEFINED__

/* library JDLLServerLib */
/* [version][helpstring][uuid] */ 


EXTERN_C const IID LIBID_JDLLServerLib;

EXTERN_C const CLSID CLSID_JDLLServer;

#ifdef __cplusplus

class DECLSPEC_UUID("21EB05EA-1AB3-11cf-A2AC-8FF70874C460")
JDLLServer;
#endif
#endif /* __JDLLServerLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long __RPC_FAR *, unsigned long            , VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long __RPC_FAR *, VARIANT __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
