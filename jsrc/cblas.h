/* cblas.h
 *
 * This header defines C bindings for the Basic Linear Algebra Subprograms,
 * providing optimized basic operations on vectors and matrices.  Single-
 * and double-precision, real and complex data formats are supported by this
 * library.
 *
 * A note on complex data layouts:
 *
 * In order to allow straightforward interoperation with other libraries and
 * complex types in C and C++, complex data in BLAS is passed through an opaque
 * pointer (void *).  The layout requirements on this complex data are that
 * the real and imaginary parts are stored consecutively in memory, and have
 * the alignment of the corresponding real type (float or double).  The BLAS
 * complex interfaces are compatible with the following types:
 *
 *     - The C complex types, defined in <complex.h>.
 *     - The C++ std::complex types, defined in <complex>.
 *     - The LAPACK complex types, defined in <Accelerate/vecLib/clapack.h>.
 *     - The vDSP types DSPComplex and DSPDoubleComplex, defined in <Accelerate/vecLib/vDSP.h>.
 *     - An array of size two of the corresponding real type.
 *     - A structure containing two elements, each of the corresponding real type.
 *
 */

#ifndef CBLAS_H
#define CBLAS_H
#include <stddef.h>

/*
 * Enumerated and derived types
 */
#define CBLAS_INDEX size_t  /* this may vary between platforms */

enum CBLAS_ORDER {CblasRowMajor=101, CblasColMajor=102};
enum CBLAS_TRANSPOSE {CblasNoTrans=111, CblasTrans=112, CblasConjTrans=113};
enum CBLAS_UPLO {CblasUpper=121, CblasLower=122};
enum CBLAS_DIAG {CblasNonUnit=131, CblasUnit=132};
enum CBLAS_SIDE {CblasLeft=141, CblasRight=142};

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ===========================================================================
 * Prototypes for level 1 BLAS functions (complex are recast as routines)
 * ===========================================================================
 */
float  cblas_sdsdot(const int N, const float alpha, const float *X,
                    const int incX, const float *Y, const int incY);
double cblas_dsdot(const int N, const float *X, const int incX, const float *Y,
                   const int incY);
float  cblas_sdot(const int N, const float  *X, const int incX,
                  const float  *Y, const int incY);
double cblas_ddot(const int N, const double *X, const int incX,
                  const double *Y, const int incY);

/*
 * Functions having prefixes Z and C only
 */
void   cblas_cdotu_sub(const int N, const void *X, const int incX,
                       const void *Y, const int incY, void *dotu);
void   cblas_cdotc_sub(const int N, const void *X, const int incX,
                       const void *Y, const int incY, void *dotc);

void   cblas_zdotu_sub(const int N, const void *X, const int incX,
                       const void *Y, const int incY, void *dotu);
void   cblas_zdotc_sub(const int N, const void *X, const int incX,
                       const void *Y, const int incY, void *dotc);


/*
 * Functions having prefixes S D SC DZ
 */
float  cblas_snrm2(const int N, const float *X, const int incX);
float  cblas_sasum(const int N, const float *X, const int incX);

double cblas_dnrm2(const int N, const double *X, const int incX);
double cblas_dasum(const int N, const double *X, const int incX);

float  cblas_scnrm2(const int N, const void *X, const int incX);
float  cblas_scasum(const int N, const void *X, const int incX);

double cblas_dznrm2(const int N, const void *X, const int incX);
double cblas_dzasum(const int N, const void *X, const int incX);


/*
 * Functions having standard 4 prefixes (S D C Z)
 */
CBLAS_INDEX cblas_isamax(const int N, const float  *X, const int incX);
CBLAS_INDEX cblas_idamax(const int N, const double *X, const int incX);
CBLAS_INDEX cblas_icamax(const int N, const void   *X, const int incX);
CBLAS_INDEX cblas_izamax(const int N, const void   *X, const int incX);

/*
 * ===========================================================================
 * Prototypes for level 1 BLAS routines
 * ===========================================================================
 */

/*
 * Routines with standard 4 prefixes (s, d, c, z)
 */
void cblas_sswap(const int N, float *X, const int incX,
                 float *Y, const int incY);
void cblas_scopy(const int N, const float *X, const int incX,
                 float *Y, const int incY);
void cblas_saxpy(const int N, const float alpha, const float *X,
                 const int incX, float *Y, const int incY);

void cblas_dswap(const int N, double *X, const int incX,
                 double *Y, const int incY);
void cblas_dcopy(const int N, const double *X, const int incX,
                 double *Y, const int incY);
void cblas_daxpy(const int N, const double alpha, const double *X,
                 const int incX, double *Y, const int incY);

void cblas_cswap(const int N, void *X, const int incX,
                 void *Y, const int incY);
void cblas_ccopy(const int N, const void *X, const int incX,
                 void *Y, const int incY);
void cblas_caxpy(const int N, const void *alpha, const void *X,
                 const int incX, void *Y, const int incY);

void cblas_zswap(const int N, void *X, const int incX,
                 void *Y, const int incY);
void cblas_zcopy(const int N, const void *X, const int incX,
                 void *Y, const int incY);
void cblas_zaxpy(const int N, const void *alpha, const void *X,
                 const int incX, void *Y, const int incY);


/*
 * Routines with S and D prefix only
 */
void cblas_srotg(float *a, float *b, float *c, float *s);
void cblas_srotmg(float *d1, float *d2, float *b1, const float b2, float *P);
void cblas_srot(const int N, float *X, const int incX,
                float *Y, const int incY, const float c, const float s);
void cblas_srotm(const int N, float *X, const int incX,
                float *Y, const int incY, const float *P);

void cblas_drotg(double *a, double *b, double *c, double *s);
void cblas_drotmg(double *d1, double *d2, double *b1, const double b2, double *P);
void cblas_drot(const int N, double *X, const int incX,
                double *Y, const int incY, const double c, const double  s);
void cblas_drotm(const int N, double *X, const int incX,
                double *Y, const int incY, const double *P);


/*
 * Routines with S D C Z CS and ZD prefixes
 */
void cblas_sscal(const int N, const float alpha, float *X, const int incX);
void cblas_dscal(const int N, const double alpha, double *X, const int incX);
void cblas_cscal(const int N, const void *alpha, void *X, const int incX);
void cblas_zscal(const int N, const void *alpha, void *X, const int incX);
void cblas_csscal(const int N, const float alpha, void *X, const int incX);
void cblas_zdscal(const int N, const double alpha, void *X, const int incX);

/*
 * ===========================================================================
 * Prototypes for level 2 BLAS
 * ===========================================================================
 */

/*
 * Routines with standard 4 prefixes (S, D, C, Z)
 */
void cblas_sgemv(const enum CBLAS_ORDER order,
                 const enum CBLAS_TRANSPOSE TransA, const int M, const int N,
                 const float alpha, const float *A, const int lda,
                 const float *X, const int incX, const float beta,
                 float *Y, const int incY);
void cblas_sgbmv(const enum CBLAS_ORDER order,
                 const enum CBLAS_TRANSPOSE TransA, const int M, const int N,
                 const int KL, const int KU, const float alpha,
                 const float *A, const int lda, const float *X,
                 const int incX, const float beta, float *Y, const int incY);
void cblas_strmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const float *A, const int lda,
                 float *X, const int incX);
void cblas_stbmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const int K, const float *A, const int lda,
                 float *X, const int incX);
void cblas_stpmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const float *Ap, float *X, const int incX);
void cblas_strsv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const float *A, const int lda, float *X,
                 const int incX);
void cblas_stbsv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const int K, const float *A, const int lda,
                 float *X, const int incX);
void cblas_stpsv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const float *Ap, float *X, const int incX);

void cblas_dgemv(const enum CBLAS_ORDER order,
                 const enum CBLAS_TRANSPOSE TransA, const int M, const int N,
                 const double alpha, const double *A, const int lda,
                 const double *X, const int incX, const double beta,
                 double *Y, const int incY);
void cblas_dgbmv(const enum CBLAS_ORDER order,
                 const enum CBLAS_TRANSPOSE TransA, const int M, const int N,
                 const int KL, const int KU, const double alpha,
                 const double *A, const int lda, const double *X,
                 const int incX, const double beta, double *Y, const int incY);
void cblas_dtrmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const double *A, const int lda,
                 double *X, const int incX);
void cblas_dtbmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const int K, const double *A, const int lda,
                 double *X, const int incX);
void cblas_dtpmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const double *Ap, double *X, const int incX);
void cblas_dtrsv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const double *A, const int lda, double *X,
                 const int incX);
void cblas_dtbsv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const int K, const double *A, const int lda,
                 double *X, const int incX);
void cblas_dtpsv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const double *Ap, double *X, const int incX);

void cblas_cgemv(const enum CBLAS_ORDER order,
                 const enum CBLAS_TRANSPOSE TransA, const int M, const int N,
                 const void *alpha, const void *A, const int lda,
                 const void *X, const int incX, const void *beta,
                 void *Y, const int incY);
void cblas_cgbmv(const enum CBLAS_ORDER order,
                 const enum CBLAS_TRANSPOSE TransA, const int M, const int N,
                 const int KL, const int KU, const void *alpha,
                 const void *A, const int lda, const void *X,
                 const int incX, const void *beta, void *Y, const int incY);
void cblas_ctrmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const void *A, const int lda,
                 void *X, const int incX);
void cblas_ctbmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const int K, const void *A, const int lda,
                 void *X, const int incX);
void cblas_ctpmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const void *Ap, void *X, const int incX);
void cblas_ctrsv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const void *A, const int lda, void *X,
                 const int incX);
void cblas_ctbsv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const int K, const void *A, const int lda,
                 void *X, const int incX);
void cblas_ctpsv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const void *Ap, void *X, const int incX);

void cblas_zgemv(const enum CBLAS_ORDER order,
                 const enum CBLAS_TRANSPOSE TransA, const int M, const int N,
                 const void *alpha, const void *A, const int lda,
                 const void *X, const int incX, const void *beta,
                 void *Y, const int incY);
void cblas_zgbmv(const enum CBLAS_ORDER order,
                 const enum CBLAS_TRANSPOSE TransA, const int M, const int N,
                 const int KL, const int KU, const void *alpha,
                 const void *A, const int lda, const void *X,
                 const int incX, const void *beta, void *Y, const int incY);
void cblas_ztrmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const void *A, const int lda,
                 void *X, const int incX);
void cblas_ztbmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const int K, const void *A, const int lda,
                 void *X, const int incX);
void cblas_ztpmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const void *Ap, void *X, const int incX);
void cblas_ztrsv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const void *A, const int lda, void *X,
                 const int incX);
void cblas_ztbsv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const int K, const void *A, const int lda,
                 void *X, const int incX);
void cblas_ztpsv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag,
                 const int N, const void *Ap, void *X, const int incX);


/*
 * Routines with S and D prefixes only
 */
void cblas_ssymv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const int N, const float alpha, const float *A,
                 const int lda, const float *X, const int incX,
                 const float beta, float *Y, const int incY);
void cblas_ssbmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const int N, const int K, const float alpha, const float *A,
                 const int lda, const float *X, const int incX,
                 const float beta, float *Y, const int incY);
void cblas_sspmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const int N, const float alpha, const float *Ap,
                 const float *X, const int incX,
                 const float beta, float *Y, const int incY);
void cblas_sger(const enum CBLAS_ORDER order, const int M, const int N,
                const float alpha, const float *X, const int incX,
                const float *Y, const int incY, float *A, const int lda);
void cblas_ssyr(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                const int N, const float alpha, const float *X,
                const int incX, float *A, const int lda);
void cblas_sspr(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                const int N, const float alpha, const float *X,
                const int incX, float *Ap);
void cblas_ssyr2(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                const int N, const float alpha, const float *X,
                const int incX, const float *Y, const int incY, float *A,
                const int lda);
void cblas_sspr2(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                const int N, const float alpha, const float *X,
                const int incX, const float *Y, const int incY, float *A);

void cblas_dsymv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const int N, const double alpha, const double *A,
                 const int lda, const double *X, const int incX,
                 const double beta, double *Y, const int incY);
void cblas_dsbmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const int N, const int K, const double alpha, const double *A,
                 const int lda, const double *X, const int incX,
                 const double beta, double *Y, const int incY);
void cblas_dspmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const int N, const double alpha, const double *Ap,
                 const double *X, const int incX,
                 const double beta, double *Y, const int incY);
void cblas_dger(const enum CBLAS_ORDER order, const int M, const int N,
                const double alpha, const double *X, const int incX,
                const double *Y, const int incY, double *A, const int lda);
void cblas_dsyr(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                const int N, const double alpha, const double *X,
                const int incX, double *A, const int lda);
void cblas_dspr(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                const int N, const double alpha, const double *X,
                const int incX, double *Ap);
void cblas_dsyr2(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                const int N, const double alpha, const double *X,
                const int incX, const double *Y, const int incY, double *A,
                const int lda);
void cblas_dspr2(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                const int N, const double alpha, const double *X,
                const int incX, const double *Y, const int incY, double *A);


/*
 * Routines with C and Z prefixes only
 */
void cblas_chemv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const int N, const void *alpha, const void *A,
                 const int lda, const void *X, const int incX,
                 const void *beta, void *Y, const int incY);
void cblas_chbmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const int N, const int K, const void *alpha, const void *A,
                 const int lda, const void *X, const int incX,
                 const void *beta, void *Y, const int incY);
void cblas_chpmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const int N, const void *alpha, const void *Ap,
                 const void *X, const int incX,
                 const void *beta, void *Y, const int incY);
void cblas_cgeru(const enum CBLAS_ORDER order, const int M, const int N,
                 const void *alpha, const void *X, const int incX,
                 const void *Y, const int incY, void *A, const int lda);
void cblas_cgerc(const enum CBLAS_ORDER order, const int M, const int N,
                 const void *alpha, const void *X, const int incX,
                 const void *Y, const int incY, void *A, const int lda);
void cblas_cher(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                const int N, const float alpha, const void *X, const int incX,
                void *A, const int lda);
void cblas_chpr(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                const int N, const float alpha, const void *X,
                const int incX, void *A);
void cblas_cher2(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const void *alpha, const void *X, const int incX,
                const void *Y, const int incY, void *A, const int lda);
void cblas_chpr2(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const void *alpha, const void *X, const int incX,
                const void *Y, const int incY, void *Ap);

void cblas_zhemv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const int N, const void *alpha, const void *A,
                 const int lda, const void *X, const int incX,
                 const void *beta, void *Y, const int incY);
void cblas_zhbmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const int N, const int K, const void *alpha, const void *A,
                 const int lda, const void *X, const int incX,
                 const void *beta, void *Y, const int incY);
void cblas_zhpmv(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                 const int N, const void *alpha, const void *Ap,
                 const void *X, const int incX,
                 const void *beta, void *Y, const int incY);
void cblas_zgeru(const enum CBLAS_ORDER order, const int M, const int N,
                 const void *alpha, const void *X, const int incX,
                 const void *Y, const int incY, void *A, const int lda);
void cblas_zgerc(const enum CBLAS_ORDER order, const int M, const int N,
                 const void *alpha, const void *X, const int incX,
                 const void *Y, const int incY, void *A, const int lda);
void cblas_zher(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                const int N, const double alpha, const void *X, const int incX,
                void *A, const int lda);
void cblas_zhpr(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo,
                const int N, const double alpha, const void *X,
                const int incX, void *A);
void cblas_zher2(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const void *alpha, const void *X, const int incX,
                const void *Y, const int incY, void *A, const int lda);
void cblas_zhpr2(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N,
                const void *alpha, const void *X, const int incX,
                const void *Y, const int incY, void *Ap);

/*
 * ===========================================================================
 * Prototypes for level 3 BLAS
 * ===========================================================================
 */

/*
 * Routines with standard 4 prefixes (S, D, C, Z)
 */
void cblas_sgemm(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
                 const int K, const float alpha, const float *A,
                 const int lda, const float *B, const int ldb,
                 const float beta, float *C, const int ldc);
void cblas_ssymm(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N,
                 const float alpha, const float *A, const int lda,
                 const float *B, const int ldb, const float beta,
                 float *C, const int ldc);
void cblas_ssyrk(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K,
                 const float alpha, const float *A, const int lda,
                 const float beta, float *C, const int ldc);
void cblas_ssyr2k(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K,
                  const float alpha, const float *A, const int lda,
                  const float *B, const int ldb, const float beta,
                  float *C, const int ldc);
void cblas_strmm(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N,
                 const float alpha, const float *A, const int lda,
                 float *B, const int ldb);
void cblas_strsm(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N,
                 const float alpha, const float *A, const int lda,
                 float *B, const int ldb);

void cblas_dgemm(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
                 const int K, const double alpha, const double *A,
                 const int lda, const double *B, const int ldb,
                 const double beta, double *C, const int ldc);
void cblas_dsymm(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N,
                 const double alpha, const double *A, const int lda,
                 const double *B, const int ldb, const double beta,
                 double *C, const int ldc);
void cblas_dsyrk(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K,
                 const double alpha, const double *A, const int lda,
                 const double beta, double *C, const int ldc);
void cblas_dsyr2k(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K,
                  const double alpha, const double *A, const int lda,
                  const double *B, const int ldb, const double beta,
                  double *C, const int ldc);
void cblas_dtrmm(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N,
                 const double alpha, const double *A, const int lda,
                 double *B, const int ldb);
void cblas_dtrsm(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N,
                 const double alpha, const double *A, const int lda,
                 double *B, const int ldb);

void cblas_cgemm(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
                 const int K, const void *alpha, const void *A,
                 const int lda, const void *B, const int ldb,
                 const void *beta, void *C, const int ldc);
void cblas_csymm(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N,
                 const void *alpha, const void *A, const int lda,
                 const void *B, const int ldb, const void *beta,
                 void *C, const int ldc);
void cblas_csyrk(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K,
                 const void *alpha, const void *A, const int lda,
                 const void *beta, void *C, const int ldc);
void cblas_csyr2k(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K,
                  const void *alpha, const void *A, const int lda,
                  const void *B, const int ldb, const void *beta,
                  void *C, const int ldc);
void cblas_ctrmm(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N,
                 const void *alpha, const void *A, const int lda,
                 void *B, const int ldb);
void cblas_ctrsm(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N,
                 const void *alpha, const void *A, const int lda,
                 void *B, const int ldb);

void cblas_zgemm(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
                 const int K, const void *alpha, const void *A,
                 const int lda, const void *B, const int ldb,
                 const void *beta, void *C, const int ldc);
void cblas_zsymm(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N,
                 const void *alpha, const void *A, const int lda,
                 const void *B, const int ldb, const void *beta,
                 void *C, const int ldc);
void cblas_zsyrk(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K,
                 const void *alpha, const void *A, const int lda,
                 const void *beta, void *C, const int ldc);
void cblas_zsyr2k(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K,
                  const void *alpha, const void *A, const int lda,
                  const void *B, const int ldb, const void *beta,
                  void *C, const int ldc);
void cblas_ztrmm(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N,
                 const void *alpha, const void *A, const int lda,
                 void *B, const int ldb);
void cblas_ztrsm(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA,
                 const enum CBLAS_DIAG Diag, const int M, const int N,
                 const void *alpha, const void *A, const int lda,
                 void *B, const int ldb);


/*
 * Routines with prefixes C and Z only
 */
void cblas_chemm(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N,
                 const void *alpha, const void *A, const int lda,
                 const void *B, const int ldb, const void *beta,
                 void *C, const int ldc);
void cblas_cherk(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K,
                 const float alpha, const void *A, const int lda,
                 const float beta, void *C, const int ldc);
void cblas_cher2k(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K,
                  const void *alpha, const void *A, const int lda,
                  const void *B, const int ldb, const float beta,
                  void *C, const int ldc);

void cblas_zhemm(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side,
                 const enum CBLAS_UPLO Uplo, const int M, const int N,
                 const void *alpha, const void *A, const int lda,
                 const void *B, const int ldb, const void *beta,
                 void *C, const int ldc);
void cblas_zherk(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                 const enum CBLAS_TRANSPOSE Trans, const int N, const int K,
                 const double alpha, const void *A, const int lda,
                 const double beta, void *C, const int ldc);
void cblas_zher2k(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo,
                  const enum CBLAS_TRANSPOSE Trans, const int N, const int K,
                  const void *alpha, const void *A, const int lda,
                  const void *B, const int ldb, const double beta,
                  void *C, const int ldc);

void cblas_xerbla(int p, const char *rout, const char *form, ...);

// cover function pointer
#ifdef CBLASINIT
 /* context here is cblasinit.c */
 #define CBLASEXTERN
#else
 /* context here is external references to cblasinit.c */
 #define CBLASEXTERN extern
#endif

#ifdef IMPORTCBLASLIB
#define jcblas_sdsdot cblas_sdsdot
#define jcblas_dsdot cblas_dsdot
#define jcblas_sdot cblas_sdot
#define jcblas_ddot cblas_ddot
#define jcblas_cdotu_sub cblas_cdotu_sub
#define jcblas_cdotc_sub cblas_cdotc_sub
#define jcblas_zdotu_sub cblas_zdotu_sub
#define jcblas_zdotc_sub cblas_zdotc_sub
#define jcblas_snrm2 cblas_snrm2
#define jcblas_sasum cblas_sasum
#define jcblas_dnrm2 cblas_dnrm2
#define jcblas_dasum cblas_dasum
#define jcblas_scnrm2 cblas_scnrm2
#define jcblas_scasum cblas_scasum
#define jcblas_dznrm2 cblas_dznrm2
#define jcblas_dzasum cblas_dzasum
#define jcblas_isamax cblas_isamax
#define jcblas_idamax cblas_idamax
#define jcblas_icamax cblas_icamax
#define jcblas_izamax cblas_izamax
#define jcblas_sswap cblas_sswap
#define jcblas_scopy cblas_scopy
#define jcblas_saxpy cblas_saxpy
#define jcblas_dswap cblas_dswap
#define jcblas_dcopy cblas_dcopy
#define jcblas_daxpy cblas_daxpy
#define jcblas_cswap cblas_cswap
#define jcblas_ccopy cblas_ccopy
#define jcblas_caxpy cblas_caxpy
#define jcblas_zswap cblas_zswap
#define jcblas_zcopy cblas_zcopy
#define jcblas_zaxpy cblas_zaxpy
#define jcblas_srotg cblas_srotg
#define jcblas_srotmg cblas_srotmg
#define jcblas_srot cblas_srot
#define jcblas_srotm cblas_srotm
#define jcblas_drotg cblas_drotg
#define jcblas_drotmg cblas_drotmg
#define jcblas_drot cblas_drot
#define jcblas_drotm cblas_drotm
#define jcblas_sscal cblas_sscal
#define jcblas_dscal cblas_dscal
#define jcblas_cscal cblas_cscal
#define jcblas_zscal cblas_zscal
#define jcblas_csscal cblas_csscal
#define jcblas_zdscal cblas_zdscal
#define jcblas_sgemv cblas_sgemv
#define jcblas_sgbmv cblas_sgbmv
#define jcblas_strmv cblas_strmv
#define jcblas_stbmv cblas_stbmv
#define jcblas_stpmv cblas_stpmv
#define jcblas_strsv cblas_strsv
#define jcblas_stbsv cblas_stbsv
#define jcblas_stpsv cblas_stpsv
#define jcblas_dgemv cblas_dgemv
#define jcblas_dgbmv cblas_dgbmv
#define jcblas_dtrmv cblas_dtrmv
#define jcblas_dtbmv cblas_dtbmv
#define jcblas_dtpmv cblas_dtpmv
#define jcblas_dtrsv cblas_dtrsv
#define jcblas_dtbsv cblas_dtbsv
#define jcblas_dtpsv cblas_dtpsv
#define jcblas_cgemv cblas_cgemv
#define jcblas_cgbmv cblas_cgbmv
#define jcblas_ctrmv cblas_ctrmv
#define jcblas_ctbmv cblas_ctbmv
#define jcblas_ctpmv cblas_ctpmv
#define jcblas_ctrsv cblas_ctrsv
#define jcblas_ctbsv cblas_ctbsv
#define jcblas_ctpsv cblas_ctpsv
#define jcblas_zgemv cblas_zgemv
#define jcblas_zgbmv cblas_zgbmv
#define jcblas_ztrmv cblas_ztrmv
#define jcblas_ztbmv cblas_ztbmv
#define jcblas_ztpmv cblas_ztpmv
#define jcblas_ztrsv cblas_ztrsv
#define jcblas_ztbsv cblas_ztbsv
#define jcblas_ztpsv cblas_ztpsv
#define jcblas_ssymv cblas_ssymv
#define jcblas_ssbmv cblas_ssbmv
#define jcblas_sspmv cblas_sspmv
#define jcblas_sger cblas_sger
#define jcblas_ssyr cblas_ssyr
#define jcblas_sspr cblas_sspr
#define jcblas_ssyr2 cblas_ssyr2
#define jcblas_sspr2 cblas_sspr2
#define jcblas_dsymv cblas_dsymv
#define jcblas_dsbmv cblas_dsbmv
#define jcblas_dspmv cblas_dspmv
#define jcblas_dger cblas_dger
#define jcblas_dsyr cblas_dsyr
#define jcblas_dspr cblas_dspr
#define jcblas_dsyr2 cblas_dsyr2
#define jcblas_dspr2 cblas_dspr2
#define jcblas_chemv cblas_chemv
#define jcblas_chbmv cblas_chbmv
#define jcblas_chpmv cblas_chpmv
#define jcblas_cgeru cblas_cgeru
#define jcblas_cgerc cblas_cgerc
#define jcblas_cher cblas_cher
#define jcblas_chpr cblas_chpr
#define jcblas_cher2 cblas_cher2
#define jcblas_chpr2 cblas_chpr2
#define jcblas_zhemv cblas_zhemv
#define jcblas_zhbmv cblas_zhbmv
#define jcblas_zhpmv cblas_zhpmv
#define jcblas_zgeru cblas_zgeru
#define jcblas_zgerc cblas_zgerc
#define jcblas_zher cblas_zher
#define jcblas_zhpr cblas_zhpr
#define jcblas_zher2 cblas_zher2
#define jcblas_zhpr2 cblas_zhpr2
#define jcblas_sgemm cblas_sgemm
#define jcblas_ssymm cblas_ssymm
#define jcblas_ssyrk cblas_ssyrk
#define jcblas_ssyr2k cblas_ssyr2k
#define jcblas_strmm cblas_strmm
#define jcblas_strsm cblas_strsm
#define jcblas_dgemm cblas_dgemm
#define jcblas_dsymm cblas_dsymm
#define jcblas_dsyrk cblas_dsyrk
#define jcblas_dsyr2k cblas_dsyr2k
#define jcblas_dtrmm cblas_dtrmm
#define jcblas_dtrsm cblas_dtrsm
#define jcblas_cgemm cblas_cgemm
#define jcblas_csymm cblas_csymm
#define jcblas_csyrk cblas_csyrk
#define jcblas_csyr2k cblas_csyr2k
#define jcblas_ctrmm cblas_ctrmm
#define jcblas_ctrsm cblas_ctrsm
#define jcblas_zgemm cblas_zgemm
#define jcblas_zsymm cblas_zsymm
#define jcblas_zsyrk cblas_zsyrk
#define jcblas_zsyr2k cblas_zsyr2k
#define jcblas_ztrmm cblas_ztrmm
#define jcblas_ztrsm cblas_ztrsm
#define jcblas_chemm cblas_chemm
#define jcblas_cherk cblas_cherk
#define jcblas_cher2k cblas_cher2k
#define jcblas_zhemm cblas_zhemm
#define jcblas_zherk cblas_zherk
#define jcblas_zher2k cblas_zher2k
#define jcblas_xerbla cblas_xerbla
#else
CBLASEXTERN float (*jcblas_sdsdot)(const int N, const float alpha, const float *X, const int incX, const float *Y, const int incY);
CBLASEXTERN double (*jcblas_dsdot)(const int N, const float *X, const int incX, const float *Y, const int incY);
CBLASEXTERN float (*jcblas_sdot)(const int N, const float *X, const int incX, const float *Y, const int incY);
CBLASEXTERN double (*jcblas_ddot)(const int N, const double *X, const int incX, const double *Y, const int incY);
CBLASEXTERN void (*jcblas_cdotu_sub)(const int N, const void *X, const int incX, const void *Y, const int incY, void *dotu);
CBLASEXTERN void (*jcblas_cdotc_sub)(const int N, const void *X, const int incX, const void *Y, const int incY, void *dotc);
CBLASEXTERN void (*jcblas_zdotu_sub)(const int N, const void *X, const int incX, const void *Y, const int incY, void *dotu);
CBLASEXTERN void (*jcblas_zdotc_sub)(const int N, const void *X, const int incX, const void *Y, const int incY, void *dotc);
CBLASEXTERN float (*jcblas_snrm2)(const int N, const float *X, const int incX);
CBLASEXTERN float (*jcblas_sasum)(const int N, const float *X, const int incX);
CBLASEXTERN double (*jcblas_dnrm2)(const int N, const double *X, const int incX);
CBLASEXTERN double (*jcblas_dasum)(const int N, const double *X, const int incX);
CBLASEXTERN float (*jcblas_scnrm2)(const int N, const void *X, const int incX);
CBLASEXTERN float (*jcblas_scasum)(const int N, const void *X, const int incX);
CBLASEXTERN double (*jcblas_dznrm2)(const int N, const void *X, const int incX);
CBLASEXTERN double (*jcblas_dzasum)(const int N, const void *X, const int incX);
CBLASEXTERN CBLAS_INDEX (*jcblas_isamax)(const int N, const float *X, const int incX);
CBLASEXTERN CBLAS_INDEX (*jcblas_idamax)(const int N, const double *X, const int incX);
CBLASEXTERN CBLAS_INDEX (*jcblas_icamax)(const int N, const void *X, const int incX);
CBLASEXTERN CBLAS_INDEX (*jcblas_izamax)(const int N, const void *X, const int incX);
CBLASEXTERN void (*jcblas_sswap)(const int N, float *X, const int incX, float *Y, const int incY);
CBLASEXTERN void (*jcblas_scopy)(const int N, const float *X, const int incX, float *Y, const int incY);
CBLASEXTERN void (*jcblas_saxpy)(const int N, const float alpha, const float *X, const int incX, float *Y, const int incY);
CBLASEXTERN void (*jcblas_dswap)(const int N, double *X, const int incX, double *Y, const int incY);
CBLASEXTERN void (*jcblas_dcopy)(const int N, const double *X, const int incX, double *Y, const int incY);
CBLASEXTERN void (*jcblas_daxpy)(const int N, const double alpha, const double *X, const int incX, double *Y, const int incY);
CBLASEXTERN void (*jcblas_cswap)(const int N, void *X, const int incX, void *Y, const int incY);
CBLASEXTERN void (*jcblas_ccopy)(const int N, const void *X, const int incX, void *Y, const int incY);
CBLASEXTERN void (*jcblas_caxpy)(const int N, const void *alpha, const void *X, const int incX, void *Y, const int incY);
CBLASEXTERN void (*jcblas_zswap)(const int N, void *X, const int incX, void *Y, const int incY);
CBLASEXTERN void (*jcblas_zcopy)(const int N, const void *X, const int incX, void *Y, const int incY);
CBLASEXTERN void (*jcblas_zaxpy)(const int N, const void *alpha, const void *X, const int incX, void *Y, const int incY);
CBLASEXTERN void (*jcblas_srotg)(float *a, float *b, float *c, float *s);
CBLASEXTERN void (*jcblas_srotmg)(float *d1, float *d2, float *b1, const float b2, float *P);
CBLASEXTERN void (*jcblas_srot)(const int N, float *X, const int incX, float *Y, const int incY, const float c, const float s);
CBLASEXTERN void (*jcblas_srotm)(const int N, float *X, const int incX, float *Y, const int incY, const float *P);
CBLASEXTERN void (*jcblas_drotg)(double *a, double *b, double *c, double *s);
CBLASEXTERN void (*jcblas_drotmg)(double *d1, double *d2, double *b1, const double b2, double *P);
CBLASEXTERN void (*jcblas_drot)(const int N, double *X, const int incX, double *Y, const int incY, const double c, const double s);
CBLASEXTERN void (*jcblas_drotm)(const int N, double *X, const int incX, double *Y, const int incY, const double *P);
CBLASEXTERN void (*jcblas_sscal)(const int N, const float alpha, float *X, const int incX);
CBLASEXTERN void (*jcblas_dscal)(const int N, const double alpha, double *X, const int incX);
CBLASEXTERN void (*jcblas_cscal)(const int N, const void *alpha, void *X, const int incX);
CBLASEXTERN void (*jcblas_zscal)(const int N, const void *alpha, void *X, const int incX);
CBLASEXTERN void (*jcblas_csscal)(const int N, const float alpha, void *X, const int incX);
CBLASEXTERN void (*jcblas_zdscal)(const int N, const double alpha, void *X, const int incX);
CBLASEXTERN void (*jcblas_sgemv)(const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M, const int N, const float alpha, const float *A, const int lda, const float *X, const int incX, const float beta, float *Y, const int incY);
CBLASEXTERN void (*jcblas_sgbmv)(const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M, const int N, const int KL, const int KU, const float alpha, const float *A, const int lda, const float *X, const int incX, const float beta, float *Y, const int incY);
CBLASEXTERN void (*jcblas_strmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const float *A, const int lda, float *X, const int incX);
CBLASEXTERN void (*jcblas_stbmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const int K, const float *A, const int lda, float *X, const int incX);
CBLASEXTERN void (*jcblas_stpmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const float *Ap, float *X, const int incX);
CBLASEXTERN void (*jcblas_strsv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const float *A, const int lda, float *X, const int incX);
CBLASEXTERN void (*jcblas_stbsv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const int K, const float *A, const int lda, float *X, const int incX);
CBLASEXTERN void (*jcblas_stpsv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const float *Ap, float *X, const int incX);
CBLASEXTERN void (*jcblas_dgemv)(const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M, const int N, const double alpha, const double *A, const int lda, const double *X, const int incX, const double beta, double *Y, const int incY);
CBLASEXTERN void (*jcblas_dgbmv)(const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M, const int N, const int KL, const int KU, const double alpha, const double *A, const int lda, const double *X, const int incX, const double beta, double *Y, const int incY);
CBLASEXTERN void (*jcblas_dtrmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const double *A, const int lda, double *X, const int incX);
CBLASEXTERN void (*jcblas_dtbmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const int K, const double *A, const int lda, double *X, const int incX);
CBLASEXTERN void (*jcblas_dtpmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const double *Ap, double *X, const int incX);
CBLASEXTERN void (*jcblas_dtrsv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const double *A, const int lda, double *X, const int incX);
CBLASEXTERN void (*jcblas_dtbsv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const int K, const double *A, const int lda, double *X, const int incX);
CBLASEXTERN void (*jcblas_dtpsv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const double *Ap, double *X, const int incX);
CBLASEXTERN void (*jcblas_cgemv)(const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M, const int N, const void *alpha, const void *A, const int lda, const void *X, const int incX, const void *beta, void *Y, const int incY);
CBLASEXTERN void (*jcblas_cgbmv)(const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M, const int N, const int KL, const int KU, const void *alpha, const void *A, const int lda, const void *X, const int incX, const void *beta, void *Y, const int incY);
CBLASEXTERN void (*jcblas_ctrmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const void *A, const int lda, void *X, const int incX);
CBLASEXTERN void (*jcblas_ctbmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const int K, const void *A, const int lda, void *X, const int incX);
CBLASEXTERN void (*jcblas_ctpmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const void *Ap, void *X, const int incX);
CBLASEXTERN void (*jcblas_ctrsv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const void *A, const int lda, void *X, const int incX);
CBLASEXTERN void (*jcblas_ctbsv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const int K, const void *A, const int lda, void *X, const int incX);
CBLASEXTERN void (*jcblas_ctpsv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const void *Ap, void *X, const int incX);
CBLASEXTERN void (*jcblas_zgemv)(const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M, const int N, const void *alpha, const void *A, const int lda, const void *X, const int incX, const void *beta, void *Y, const int incY);
CBLASEXTERN void (*jcblas_zgbmv)(const enum CBLAS_ORDER order, const enum CBLAS_TRANSPOSE TransA, const int M, const int N, const int KL, const int KU, const void *alpha, const void *A, const int lda, const void *X, const int incX, const void *beta, void *Y, const int incY);
CBLASEXTERN void (*jcblas_ztrmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const void *A, const int lda, void *X, const int incX);
CBLASEXTERN void (*jcblas_ztbmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const int K, const void *A, const int lda, void *X, const int incX);
CBLASEXTERN void (*jcblas_ztpmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const void *Ap, void *X, const int incX);
CBLASEXTERN void (*jcblas_ztrsv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const void *A, const int lda, void *X, const int incX);
CBLASEXTERN void (*jcblas_ztbsv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const int K, const void *A, const int lda, void *X, const int incX);
CBLASEXTERN void (*jcblas_ztpsv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int N, const void *Ap, void *X, const int incX);
CBLASEXTERN void (*jcblas_ssymv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const float alpha, const float *A, const int lda, const float *X, const int incX, const float beta, float *Y, const int incY);
CBLASEXTERN void (*jcblas_ssbmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const int K, const float alpha, const float *A, const int lda, const float *X, const int incX, const float beta, float *Y, const int incY);
CBLASEXTERN void (*jcblas_sspmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const float alpha, const float *Ap, const float *X, const int incX, const float beta, float *Y, const int incY);
CBLASEXTERN void (*jcblas_sger)(const enum CBLAS_ORDER order, const int M, const int N, const float alpha, const float *X, const int incX, const float *Y, const int incY, float *A, const int lda);
CBLASEXTERN void (*jcblas_ssyr)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const float alpha, const float *X, const int incX, float *A, const int lda);
CBLASEXTERN void (*jcblas_sspr)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const float alpha, const float *X, const int incX, float *Ap);
CBLASEXTERN void (*jcblas_ssyr2)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const float alpha, const float *X, const int incX, const float *Y, const int incY, float *A, const int lda);
CBLASEXTERN void (*jcblas_sspr2)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const float alpha, const float *X, const int incX, const float *Y, const int incY, float *A);
CBLASEXTERN void (*jcblas_dsymv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const double alpha, const double *A, const int lda, const double *X, const int incX, const double beta, double *Y, const int incY);
CBLASEXTERN void (*jcblas_dsbmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const int K, const double alpha, const double *A, const int lda, const double *X, const int incX, const double beta, double *Y, const int incY);
CBLASEXTERN void (*jcblas_dspmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const double alpha, const double *Ap, const double *X, const int incX, const double beta, double *Y, const int incY);
CBLASEXTERN void (*jcblas_dger)(const enum CBLAS_ORDER order, const int M, const int N, const double alpha, const double *X, const int incX, const double *Y, const int incY, double *A, const int lda);
CBLASEXTERN void (*jcblas_dsyr)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const double alpha, const double *X, const int incX, double *A, const int lda);
CBLASEXTERN void (*jcblas_dspr)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const double alpha, const double *X, const int incX, double *Ap);
CBLASEXTERN void (*jcblas_dsyr2)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const double alpha, const double *X, const int incX, const double *Y, const int incY, double *A, const int lda);
CBLASEXTERN void (*jcblas_dspr2)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const double alpha, const double *X, const int incX, const double *Y, const int incY, double *A);
CBLASEXTERN void (*jcblas_chemv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const void *alpha, const void *A, const int lda, const void *X, const int incX, const void *beta, void *Y, const int incY);
CBLASEXTERN void (*jcblas_chbmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const int K, const void *alpha, const void *A, const int lda, const void *X, const int incX, const void *beta, void *Y, const int incY);
CBLASEXTERN void (*jcblas_chpmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const void *alpha, const void *Ap, const void *X, const int incX, const void *beta, void *Y, const int incY);
CBLASEXTERN void (*jcblas_cgeru)(const enum CBLAS_ORDER order, const int M, const int N, const void *alpha, const void *X, const int incX, const void *Y, const int incY, void *A, const int lda);
CBLASEXTERN void (*jcblas_cgerc)(const enum CBLAS_ORDER order, const int M, const int N, const void *alpha, const void *X, const int incX, const void *Y, const int incY, void *A, const int lda);
CBLASEXTERN void (*jcblas_cher)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const float alpha, const void *X, const int incX, void *A, const int lda);
CBLASEXTERN void (*jcblas_chpr)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const float alpha, const void *X, const int incX, void *A);
CBLASEXTERN void (*jcblas_cher2)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const void *alpha, const void *X, const int incX, const void *Y, const int incY, void *A, const int lda);
CBLASEXTERN void (*jcblas_chpr2)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const void *alpha, const void *X, const int incX, const void *Y, const int incY, void *Ap);
CBLASEXTERN void (*jcblas_zhemv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const void *alpha, const void *A, const int lda, const void *X, const int incX, const void *beta, void *Y, const int incY);
CBLASEXTERN void (*jcblas_zhbmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const int K, const void *alpha, const void *A, const int lda, const void *X, const int incX, const void *beta, void *Y, const int incY);
CBLASEXTERN void (*jcblas_zhpmv)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const void *alpha, const void *Ap, const void *X, const int incX, const void *beta, void *Y, const int incY);
CBLASEXTERN void (*jcblas_zgeru)(const enum CBLAS_ORDER order, const int M, const int N, const void *alpha, const void *X, const int incX, const void *Y, const int incY, void *A, const int lda);
CBLASEXTERN void (*jcblas_zgerc)(const enum CBLAS_ORDER order, const int M, const int N, const void *alpha, const void *X, const int incX, const void *Y, const int incY, void *A, const int lda);
CBLASEXTERN void (*jcblas_zher)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const double alpha, const void *X, const int incX, void *A, const int lda);
CBLASEXTERN void (*jcblas_zhpr)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const double alpha, const void *X, const int incX, void *A);
CBLASEXTERN void (*jcblas_zher2)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const void *alpha, const void *X, const int incX, const void *Y, const int incY, void *A, const int lda);
CBLASEXTERN void (*jcblas_zhpr2)(const enum CBLAS_ORDER order, const enum CBLAS_UPLO Uplo, const int N, const void *alpha, const void *X, const int incX, const void *Y, const int incY, void *Ap);
CBLASEXTERN void (*jcblas_sgemm)(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_TRANSPOSE TransB, const int M, const int N, const int K, const float alpha, const float *A, const int lda, const float *B, const int ldb, const float beta, float *C, const int ldc);
CBLASEXTERN void (*jcblas_ssymm)(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side, const enum CBLAS_UPLO Uplo, const int M, const int N, const float alpha, const float *A, const int lda, const float *B, const int ldb, const float beta, float *C, const int ldc);
CBLASEXTERN void (*jcblas_ssyrk)(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const float alpha, const float *A, const int lda, const float beta, float *C, const int ldc);
CBLASEXTERN void (*jcblas_ssyr2k)(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const float alpha, const float *A, const int lda, const float *B, const int ldb, const float beta, float *C, const int ldc);
CBLASEXTERN void (*jcblas_strmm)(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int M, const int N, const float alpha, const float *A, const int lda, float *B, const int ldb);
CBLASEXTERN void (*jcblas_strsm)(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int M, const int N, const float alpha, const float *A, const int lda, float *B, const int ldb);
CBLASEXTERN void (*jcblas_dgemm)(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_TRANSPOSE TransB, const int M, const int N, const int K, const double alpha, const double *A, const int lda, const double *B, const int ldb, const double beta, double *C, const int ldc);
CBLASEXTERN void (*jcblas_dsymm)(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side, const enum CBLAS_UPLO Uplo, const int M, const int N, const double alpha, const double *A, const int lda, const double *B, const int ldb, const double beta, double *C, const int ldc);
CBLASEXTERN void (*jcblas_dsyrk)(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const double alpha, const double *A, const int lda, const double beta, double *C, const int ldc);
CBLASEXTERN void (*jcblas_dsyr2k)(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const double alpha, const double *A, const int lda, const double *B, const int ldb, const double beta, double *C, const int ldc);
CBLASEXTERN void (*jcblas_dtrmm)(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int M, const int N, const double alpha, const double *A, const int lda, double *B, const int ldb);
CBLASEXTERN void (*jcblas_dtrsm)(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int M, const int N, const double alpha, const double *A, const int lda, double *B, const int ldb);
CBLASEXTERN void (*jcblas_cgemm)(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_TRANSPOSE TransB, const int M, const int N, const int K, const void *alpha, const void *A, const int lda, const void *B, const int ldb, const void *beta, void *C, const int ldc);
CBLASEXTERN void (*jcblas_csymm)(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side, const enum CBLAS_UPLO Uplo, const int M, const int N, const void *alpha, const void *A, const int lda, const void *B, const int ldb, const void *beta, void *C, const int ldc);
CBLASEXTERN void (*jcblas_csyrk)(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void *alpha, const void *A, const int lda, const void *beta, void *C, const int ldc);
CBLASEXTERN void (*jcblas_csyr2k)(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void *alpha, const void *A, const int lda, const void *B, const int ldb, const void *beta, void *C, const int ldc);
CBLASEXTERN void (*jcblas_ctrmm)(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int M, const int N, const void *alpha, const void *A, const int lda, void *B, const int ldb);
CBLASEXTERN void (*jcblas_ctrsm)(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int M, const int N, const void *alpha, const void *A, const int lda, void *B, const int ldb);
CBLASEXTERN void (*jcblas_zgemm)(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_TRANSPOSE TransB, const int M, const int N, const int K, const void *alpha, const void *A, const int lda, const void *B, const int ldb, const void *beta, void *C, const int ldc);
CBLASEXTERN void (*jcblas_zsymm)(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side, const enum CBLAS_UPLO Uplo, const int M, const int N, const void *alpha, const void *A, const int lda, const void *B, const int ldb, const void *beta, void *C, const int ldc);
CBLASEXTERN void (*jcblas_zsyrk)(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void *alpha, const void *A, const int lda, const void *beta, void *C, const int ldc);
CBLASEXTERN void (*jcblas_zsyr2k)(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void *alpha, const void *A, const int lda, const void *B, const int ldb, const void *beta, void *C, const int ldc);
CBLASEXTERN void (*jcblas_ztrmm)(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int M, const int N, const void *alpha, const void *A, const int lda, void *B, const int ldb);
CBLASEXTERN void (*jcblas_ztrsm)(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE TransA, const enum CBLAS_DIAG Diag, const int M, const int N, const void *alpha, const void *A, const int lda, void *B, const int ldb);
CBLASEXTERN void (*jcblas_chemm)(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side, const enum CBLAS_UPLO Uplo, const int M, const int N, const void *alpha, const void *A, const int lda, const void *B, const int ldb, const void *beta, void *C, const int ldc);
CBLASEXTERN void (*jcblas_cherk)(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const float alpha, const void *A, const int lda, const float beta, void *C, const int ldc);
CBLASEXTERN void (*jcblas_cher2k)(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void *alpha, const void *A, const int lda, const void *B, const int ldb, const float beta, void *C, const int ldc);
CBLASEXTERN void (*jcblas_zhemm)(const enum CBLAS_ORDER Order, const enum CBLAS_SIDE Side, const enum CBLAS_UPLO Uplo, const int M, const int N, const void *alpha, const void *A, const int lda, const void *B, const int ldb, const void *beta, void *C, const int ldc);
CBLASEXTERN void (*jcblas_zherk)(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const double alpha, const void *A, const int lda, const double beta, void *C, const int ldc);
CBLASEXTERN void (*jcblas_zher2k)(const enum CBLAS_ORDER Order, const enum CBLAS_UPLO Uplo, const enum CBLAS_TRANSPOSE Trans, const int N, const int K, const void *alpha, const void *A, const int lda, const void *B, const int ldb, const double beta, void *C, const int ldc);
CBLASEXTERN void (*jcblas_xerbla)(int p, const char *rout, const char *form, ...);
#endif

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

extern void dgetrf_(int *m, int *n, double *A, int *lda, int *ipiv, int *info);
CBLASEXTERN void (*jdgetrf_)(int *m, int *n, double *A, int *lda, int *ipiv, int *info);

extern void zgetrf_(int *m, int *n, dcomplex *A, int *lda, int *ipiv, int *info);
CBLASEXTERN void (*jzgetrf_)(int *m, int *n, dcomplex *A, int *lda, int *ipiv, int *info);

extern void dgetrs_(char *trans, int *n, int *nrhs, double *A, int *lda, int *ipiv, double *B, int *ldb, int *info);
CBLASEXTERN void (*jdgetrs_)(char *trans, int *n, int *nrhs, double *A, int *lda, int *ipiv, double *B, int *ldb, int *info);

extern void zgetrs_(char *trans, int *n, int *nrhs, dcomplex *A, int *lda, int *ipiv, dcomplex *B, int *ldb, int *info);
CBLASEXTERN void (*jzgetrs_)(char *trans, int *n, int *nrhs, dcomplex *A, int *lda, int *ipiv, dcomplex *B, int *ldb, int *info);

extern void dgetri_(int *m, double *A, int *lda, int *ipiv, double *work, int *lwork, int *info);
CBLASEXTERN void (*jdgetri_)(int *m, double *A, int *lda, int *ipiv, double *work, int *lwork, int *info);

extern void zgetri_(int *m, dcomplex *A, int *lda, int *ipiv, dcomplex *work, int *lwork, int *info);
CBLASEXTERN void (*jzgetri_)(int *m, dcomplex *A, int *lda, int *ipiv, dcomplex *work, int *lwork, int *info);

extern void dgels_(char *trans, int *m, int *n, int *nrhs, double *A, int *lda, double *B, int *ldb, double *work, int *lwork, int *info);
CBLASEXTERN void (*jdgels_)(char *trans, int *m, int *n, int *nrhs, double *A, int *lda, double *B, int *ldb, double *work, int *lwork, int *info);

extern void zgels_(char *trans, int *m, int *n, int *nrhs, dcomplex *A, int *lda, dcomplex *B, int *ldb, dcomplex *work, int *lwork, int *info);
CBLASEXTERN void (*jzgels_)(char *trans, int *m, int *n, int *nrhs, dcomplex *A, int *lda, dcomplex *B, int *ldb, dcomplex *work, int *lwork, int *info);

extern void dgecon_(char *norm, int *n, double *A, int *lda, double *anorm, double *rcond, double *work, int *iwork, int *info);
CBLASEXTERN void (*jdgecon_)(char *norm, int *n, double *A, int *lda, double *anorm, double *rcond, double *work, int *iwork, int *info);

extern void zgecon_(char *norm, int *n, dcomplex *A, int *lda, double *anorm, double *rcond, dcomplex *work, double *rwork, int *info);
CBLASEXTERN void (*jzgecon_)(char *norm, int *n, dcomplex *A, int *lda, double *anorm, double *rcond, dcomplex *work, double *rwork, int *info);

extern void dgeqrf_(int *m, int *n, double *A, int *lda, double *tau, double *work, int *lwork, int *info);
CBLASEXTERN void (*jdgeqrf_)(int *m, int *n, double *A, int *lda, double *tau, double *work, int *lwork, int *info);

extern void zgeqrf_(int *m, int *n, dcomplex *A, int *lda, dcomplex *tau, dcomplex *work, int *lwork, int *info);
CBLASEXTERN void (*jzgeqrf_)(int *m, int *n, dcomplex *A, int *lda, dcomplex *tau, dcomplex *work, int *lwork, int *info);

extern void dorgqr_(int *m, int *n, int *k, double *A, int *lda, double *tau, double *work, int *lwork, int *info);
CBLASEXTERN void (*jdorgqr_)(int *m, int *n, int *k, double *A, int *lda, double *tau, double *work, int *lwork, int *info);

extern void zungqr_(int *m, int *n, int *k, dcomplex *A, int *lda, dcomplex *tau, dcomplex *work, int *lwork, int *info);
CBLASEXTERN void (*jzungqr_)(int *m, int *n, int *k, dcomplex *A, int *lda, dcomplex *tau, dcomplex *work, int *lwork, int *info);

extern void dtrcon_(char *norm, char *uplo, char *diag, int *n, double *A, int *lda, double *rcond, double *work, int *iwork, int *info);
CBLASEXTERN void (*jdtrcon_)(char *norm, char *uplo, char *diag, int *n, double *A, int *lda, double *rcond, double *work, int *iwork, int *info);

extern void ztrcon_(char *norm, char *uplo, char *diag, int *n, dcomplex *A, int *lda, double *rcond, dcomplex *work, double *rwork, int *info);
CBLASEXTERN void (*jztrcon_)(char *norm, char *uplo, char *diag, int *n, dcomplex *A, int *lda, double *rcond, dcomplex *work, double *rwork, int *info);

#undef CBLASEXTERN

extern void*libcblas;
extern char hascblas;
extern char hasopenmp;

#ifdef __cplusplus
}
#endif
#endif
