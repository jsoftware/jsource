#ifndef __GEMM_H
#define __GEMM_H

#include "cblas.h"

#ifndef _DEFINED_SCOMPLEX
#define _DEFINED_SCOMPLEX
typedef struct
{
	float  real;
	float  imag;
} scomplex;
#endif

#ifndef _DEFINED_DCOMPLEX
#define _DEFINED_DCOMPLEX
typedef struct
{
	double real;
	double imag;
} dcomplex;
#endif

extern void dgemm_nnblis
        (I              m,
         I              n,
         I              k,
         double         alpha,
         double         *A,
         I              rs_A,
         I              cs_A,
         double         *B,
         I              rs_B,
         I              cs_B,
         double         beta,
         double         *C,
         I              rs_C,
         I              cs_C);

extern void zgemm_nnblis
        (I              m,
         I              n,
         I              k,
         dcomplex       *alpha,
         dcomplex       *A,
         I              rs_A,
         I              cs_A,
         dcomplex       *B,
         I              rs_B,
         I              cs_B,
         dcomplex       *beta,
         dcomplex       *C,
         I              rs_C,
         I              cs_C);

extern void igemm_nnblis
        (I              m,
         I              n,
         I              k,
         I              alpha,
         I              *A,
         I              rs_A,
         I              cs_A,
         I              *B,
         I              rs_B,
         I              cs_B,
         I              beta,
         double         *C,
         I              rs_C,
         I              cs_C);

// enum CBLAS_ORDER {CblasRowMajor=101, CblasColMajor=102 };
// enum CBLAS_TRANSPOSE {CblasNoTrans=111, CblasTrans=112, CblasConjTrans=113, AtlasConj=114};
// typedef int __LAPACK_int;
// // typedef scomplex __LAPACK_float_complex;
// typedef dcomplex __LAPACK_double_complex;
// // extern void cblas_sgemm(const enum CBLAS_ORDER ORDER, const enum CBLAS_TRANSPOSE TRANSA, const enum CBLAS_TRANSPOSE TRANSB, const __LAPACK_int M, const __LAPACK_int N, const __LAPACK_int K, const float ALPHA, const float * A, const __LAPACK_int LDA, const float * B, const __LAPACK_int LDB, const float BETA, float * C, const __LAPACK_int LDC);
// extern void cblas_dgemm(const enum CBLAS_ORDER ORDER, const enum CBLAS_TRANSPOSE TRANSA, const enum CBLAS_TRANSPOSE TRANSB, const __LAPACK_int M, const __LAPACK_int N, const __LAPACK_int K, const double ALPHA, const double * A, const __LAPACK_int LDA, const double * B, const __LAPACK_int LDB, const double BETA, double * C, const __LAPACK_int LDC);
// // extern void cblas_cgemm(const enum CBLAS_ORDER ORDER, const enum CBLAS_TRANSPOSE TRANSA, const enum CBLAS_TRANSPOSE TRANSB, const __LAPACK_int M, const __LAPACK_int N, const __LAPACK_int K, const __LAPACK_float_complex * ALPHA, const __LAPACK_float_complex * A, const __LAPACK_int LDA, const __LAPACK_float_complex * B, const __LAPACK_int LDB, const __LAPACK_float_complex * BETA, __LAPACK_float_complex * C, const __LAPACK_int LDC);
// extern void cblas_zgemm(const enum CBLAS_ORDER ORDER, const enum CBLAS_TRANSPOSE TRANSA, const enum CBLAS_TRANSPOSE TRANSB, const __LAPACK_int M, const __LAPACK_int N, const __LAPACK_int K, const __LAPACK_double_complex * ALPHA, const __LAPACK_double_complex * A, const __LAPACK_int LDA, const __LAPACK_double_complex * B, const __LAPACK_int LDB, const __LAPACK_double_complex * BETA, __LAPACK_double_complex * C, const __LAPACK_int LDC);
// //note: we never get passed significant values for cs_x
// #define dgemm_nn( m, n, k, alpha, A, rs_A, cs_A, B, rs_B, cs_B, beta, C, rs_C, cs_C) \
//  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, (m), (n), (k), (alpha), (A), (rs_A), (B), (rs_B), (beta), (C), (rs_C))
//
// //note: we never get passed significant values for cs_x
// #define zgemm_nn( m, n, k, alpha, A, rs_A, cs_A, B, rs_B, cs_B, beta, C, rs_C, cs_C) \
//  cblas_zgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, (m), (n), (k), &(alpha), (A), (rs_A), (B), (rs_B), &(beta), (C), (rs_C))

INLINE void
dgemm_nn(I              m,
         I              n,
         I              k,
         double         alpha,
         double         *A,
         I              rs_A,
         I              cs_A,
         double         *B,
         I              rs_B,
         I              cs_B,
         double         beta,
         double         *C,
         I              rs_C,
         I              cs_C)
{
 if(!libcblas)
  dgemm_nnblis((m), (n), (k), (alpha), (A), (rs_A), (cs_A), (B), (rs_B), (cs_B), (beta), (C), (rs_C), (cs_C));
 else
  jcblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, (m), (n), (k), (alpha), (A), (rs_A), (B), (rs_B), (beta), (C), (rs_C));
}

INLINE void
zgemm_nn(I              m,
         I              n,
         I              k,
         dcomplex       *alpha,
         dcomplex       *A,
         I              rs_A,
         I              cs_A,
         dcomplex       *B,
         I              rs_B,
         I              cs_B,
         dcomplex       *beta,
         dcomplex       *C,
         I              rs_C,
         I              cs_C)
{
 if(!libcblas)
  zgemm_nnblis((m), (n), (k), (alpha), (A), (rs_A), (cs_A), (B), (rs_B), (cs_B), (beta), (C), (rs_C), (cs_C));
 else
  jcblas_zgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, (m), (n), (k), (alpha), (A), (rs_A), (B), (rs_B), (beta), (C), (rs_C));
}

//note: no such cblas_igemm
INLINE void
igemm_nn(I              m,
         I              n,
         I              k,
         I              alpha,
         I              *A,
         I              rs_A,
         I              cs_A,
         I              *B,
         I              rs_B,
         I              cs_B,
         I              beta,
         double         *C,
         I              rs_C,
         I              cs_C)
{
 igemm_nnblis((m), (n), (k), (alpha), (A), (rs_A), (cs_A), (B), (rs_B), (cs_B), (beta), (C), (rs_C), (cs_C));
}

#endif
