/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* cblas                                                                   */

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <dlfcn.h>
#endif
#include <stdio.h>

#define CBLASINIT
#include "j.h"
#include "cpuinfo.h"

void*libcblas=0;

#if defined(__APPLE__)
#define LIBCBLASNAME "/System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/vecLib"
#elif defined(_WIN32)
#define LIBCBLASNAME "libopenblas.dll"
#elif defined(ANDROID)
#define LIBCBLASNAME "liblapack.so"
#else
#define LIBCBLASNAME "liblapack.so.3"
#endif

#ifdef _WIN32
static void dldiag(){}
#define jcblasfn1(fn) j##fn= GetProcAddress(libcblas,"_"#fn); if(!(j##fn)){fprintf(stderr,"%s\n","error loading "#fn);};
#elif defined(__wasm__)
static void dldiag(){}
#define jcblasfn1(fn)
#else
#if defined(__APPLE__)
#define jcblasfn1(fn) j##fn= fn;    // from framework accelerate
#else
static void dldiag(){char*s=dlerror();if(s)fprintf(stderr,"%s\n",s);}
#define jcblasfn1(fn) j##fn= dlsym(libcblas,""#fn); dldiag();
#endif
#endif

// cblas dynamic linking
void cblasinit(C*libpath) {
 C dllpath[1000];
#if defined(__APPLE__)
 libcblas= dlopen(LIBCBLASNAME, RTLD_LAZY);
#elif defined(__wasm__)
 libcblas= 0;
#elif defined(_WIN32)
 if(libpath&&*libpath){
  strcpy(dllpath,libpath);strcat(dllpath,"\\");strcat(dllpath,LIBCBLASNAME);
  if(!(libcblas= LoadLibraryA(dllpath)))  /* first try current directory */
  libcblas= LoadLibraryA(LIBCBLASNAME);
 } else libcblas= LoadLibraryA(LIBCBLASNAME);
 if(libcblas && !GetProcAddress(libcblas,"cblas_dgemm")){   /* check cblas routine */
  FreeLibrary(libcblas); libcblas= 0;
 }
#else
 if(libpath&&*libpath){
  int i=0;
  while(usrlib[i]){
   if(!strcmp(libpath,usrlib[i])) {break;}
   i++;
  }
  strcpy(dllpath,libpath);strcat(dllpath,"/");strcat(dllpath,LIBCBLASNAME);
  if((libcblas= dlopen(dllpath, RTLD_LAZY))){
   if(!dlsym(libcblas,"cblas_dgemm")){   /* check cblas routine */
    dlclose(libcblas); libcblas= 0;
   }
  }
  if(!libcblas) libcblas= dlopen(LIBCBLASNAME, RTLD_LAZY);
 } else libcblas= dlopen(LIBCBLASNAME, RTLD_LAZY);
 if(libcblas && !dlsym(libcblas,"cblas_dgemm")){   /* check cblas routine */
  dlclose(libcblas); libcblas= 0;
 }
#endif
// fprintf(stderr, "%s\n", (libcblas)?"using cblas":"not using cblas");

 if(libcblas) {
//   jcblasfn1(cblas_sdsdot)
//   jcblasfn1(cblas_dsdot)
//   jcblasfn1(cblas_sdot)
//   jcblasfn1(cblas_ddot)
//   jcblasfn1(cblas_cdotu_sub)
//   jcblasfn1(cblas_cdotc_sub)
//   jcblasfn1(cblas_zdotu_sub)
//   jcblasfn1(cblas_zdotc_sub)
//   jcblasfn1(cblas_snrm2)
//   jcblasfn1(cblas_sasum)
//   jcblasfn1(cblas_dnrm2)
//   jcblasfn1(cblas_dasum)
//   jcblasfn1(cblas_scnrm2)
//   jcblasfn1(cblas_scasum)
//   jcblasfn1(cblas_dznrm2)
//   jcblasfn1(cblas_dzasum)
//   jcblasfn1(cblas_isamax)
//   jcblasfn1(cblas_idamax)
//   jcblasfn1(cblas_icamax)
//   jcblasfn1(cblas_izamax)
//   jcblasfn1(cblas_sswap)
//   jcblasfn1(cblas_scopy)
//   jcblasfn1(cblas_saxpy)
//   jcblasfn1(cblas_dswap)
//   jcblasfn1(cblas_dcopy)
//   jcblasfn1(cblas_daxpy)
//   jcblasfn1(cblas_cswap)
//   jcblasfn1(cblas_ccopy)
//   jcblasfn1(cblas_caxpy)
//   jcblasfn1(cblas_zswap)
//   jcblasfn1(cblas_zcopy)
//   jcblasfn1(cblas_zaxpy)
//   jcblasfn1(cblas_srotg)
//   jcblasfn1(cblas_srotmg)
//   jcblasfn1(cblas_srot)
//   jcblasfn1(cblas_srotm)
//   jcblasfn1(cblas_drotg)
//   jcblasfn1(cblas_drotmg)
//   jcblasfn1(cblas_drot)
//   jcblasfn1(cblas_drotm)
//   jcblasfn1(cblas_sscal)
//   jcblasfn1(cblas_dscal)
//   jcblasfn1(cblas_cscal)
//   jcblasfn1(cblas_zscal)
//   jcblasfn1(cblas_csscal)
//   jcblasfn1(cblas_zdscal)
//   jcblasfn1(cblas_sgemv)
//   jcblasfn1(cblas_sgbmv)
//   jcblasfn1(cblas_strmv)
//   jcblasfn1(cblas_stbmv)
//   jcblasfn1(cblas_stpmv)
//   jcblasfn1(cblas_strsv)
//   jcblasfn1(cblas_stbsv)
//   jcblasfn1(cblas_stpsv)
//   jcblasfn1(cblas_dgemv)
//   jcblasfn1(cblas_dgbmv)
//   jcblasfn1(cblas_dtrmv)
//   jcblasfn1(cblas_dtbmv)
//   jcblasfn1(cblas_dtpmv)
//   jcblasfn1(cblas_dtrsv)
//   jcblasfn1(cblas_dtbsv)
//   jcblasfn1(cblas_dtpsv)
//   jcblasfn1(cblas_cgemv)
//   jcblasfn1(cblas_cgbmv)
//   jcblasfn1(cblas_ctrmv)
//   jcblasfn1(cblas_ctbmv)
//   jcblasfn1(cblas_ctpmv)
//   jcblasfn1(cblas_ctrsv)
//   jcblasfn1(cblas_ctbsv)
//   jcblasfn1(cblas_ctpsv)
//   jcblasfn1(cblas_zgemv)
//   jcblasfn1(cblas_zgbmv)
//   jcblasfn1(cblas_ztrmv)
//   jcblasfn1(cblas_ztbmv)
//   jcblasfn1(cblas_ztpmv)
//   jcblasfn1(cblas_ztrsv)
//   jcblasfn1(cblas_ztbsv)
//   jcblasfn1(cblas_ztpsv)
//   jcblasfn1(cblas_ssymv)
//   jcblasfn1(cblas_ssbmv)
//   jcblasfn1(cblas_sspmv)
//   jcblasfn1(cblas_sger)
//   jcblasfn1(cblas_ssyr)
//   jcblasfn1(cblas_sspr)
//   jcblasfn1(cblas_ssyr2)
//   jcblasfn1(cblas_sspr2)
//   jcblasfn1(cblas_dsymv)
//   jcblasfn1(cblas_dsbmv)
//   jcblasfn1(cblas_dspmv)
//   jcblasfn1(cblas_dger)
//   jcblasfn1(cblas_dsyr)
//   jcblasfn1(cblas_dspr)
//   jcblasfn1(cblas_dsyr2)
//   jcblasfn1(cblas_dspr2)
//   jcblasfn1(cblas_chemv)
//   jcblasfn1(cblas_chbmv)
//   jcblasfn1(cblas_chpmv)
//   jcblasfn1(cblas_cgeru)
//   jcblasfn1(cblas_cgerc)
//   jcblasfn1(cblas_cher)
//   jcblasfn1(cblas_chpr)
//   jcblasfn1(cblas_cher2)
//   jcblasfn1(cblas_chpr2)
//   jcblasfn1(cblas_zhemv)
//   jcblasfn1(cblas_zhbmv)
//   jcblasfn1(cblas_zhpmv)
//   jcblasfn1(cblas_zgeru)
//   jcblasfn1(cblas_zgerc)
//   jcblasfn1(cblas_zher)
//   jcblasfn1(cblas_zhpr)
//   jcblasfn1(cblas_zher2)
//   jcblasfn1(cblas_zhpr2)
//   jcblasfn1(cblas_sgemm)
//   jcblasfn1(cblas_ssymm)
//   jcblasfn1(cblas_ssyrk)
//   jcblasfn1(cblas_ssyr2k)
//   jcblasfn1(cblas_strmm)
//   jcblasfn1(cblas_strsm)
  jcblasfn1(cblas_dgemm)
//   jcblasfn1(cblas_dsymm)
//   jcblasfn1(cblas_dsyrk)
//   jcblasfn1(cblas_dsyr2k)
//   jcblasfn1(cblas_dtrmm)
//   jcblasfn1(cblas_dtrsm)
//   jcblasfn1(cblas_cgemm)
//   jcblasfn1(cblas_csymm)
//   jcblasfn1(cblas_csyrk)
//   jcblasfn1(cblas_csyr2k)
//   jcblasfn1(cblas_ctrmm)
//   jcblasfn1(cblas_ctrsm)
  jcblasfn1(cblas_zgemm)
//   jcblasfn1(cblas_zsymm)
//   jcblasfn1(cblas_zsyrk)
//   jcblasfn1(cblas_zsyr2k)
//   jcblasfn1(cblas_ztrmm)
//   jcblasfn1(cblas_ztrsm)
//   jcblasfn1(cblas_chemm)
//   jcblasfn1(cblas_cherk)
//   jcblasfn1(cblas_cher2k)
//   jcblasfn1(cblas_zhemm)
//   jcblasfn1(cblas_zherk)
//   jcblasfn1(cblas_zher2k)
//   jcblasfn1(cblas_xerbla)
 }
 libcblas= !!libcblas;
}
