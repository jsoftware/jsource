#ifndef  _BLIS_HEADER
#define  _BLIS_HEADER

/* clang-cl */
#if defined(__clang__) && !defined(__GNUC__)
#define __GNUC__ 5
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#undef MMSC_VER
#define MMSC_VER
#else
#undef MMSC_VER
#endif

/* simplified blis header */

#define BLIS_DRIVER_GENERIC     0
#define BLIS_DRIVER_GNUVEC      1
#define BLIS_DRIVER_SSE2        2
#define BLIS_DRIVER_AVX         3
#define BLIS_DRIVER_HASWELL     4
#define BLIS_DRIVER_AARCH64     5

#define restrict __restrict

/* msvc does not define __SSE2__ */
#if !defined(__SSE2__)
#if defined(MMSC_VER)
#if (defined(_M_AMD64) || defined(_M_X64))
#define __SSE2__ 1
#include <emmintrin.h>
#include <xmmintrin.h>   /* header file for _mm_prefetch() */
#elif _M_IX86_FP==2
#define __SSE2__ 1
#include <emmintrin.h>
#include <xmmintrin.h>   /* header file for _mm_prefetch() */
#endif
#endif
#endif

#if defined(MMSC_VER)
#ifndef PREFETCH
#define PREFETCH(x) _mm_prefetch((x),_MM_HINT_T0)
#define PREFETCH2(x) _mm_prefetch((x),_MM_HINT_T1)   // prefetch into L2 cache but not L1
#endif
#endif
#ifdef __GNUC__
#ifndef PREFETCH
#define PREFETCH(x) __builtin_prefetch(x)
#define PREFETCH2(x) __builtin_prefetch((x),0,2)   // prefetch into L2 cache but not L1
#endif
#endif

#ifndef CCBLOCK
/* (hopefully) turn off some re-scheduling optimization  */
#ifdef __GNUC__
#define CCBLOCK __asm__("" ::: "cc")
#else
#define CCBLOCK
#endif
#endif

#if defined(_WIN64)||defined(__LP64__)
typedef long long  gint_t;
#else
typedef long       gint_t;
#endif

typedef  gint_t  dim_t;      // dimension type
typedef  gint_t  inc_t;      // increment/stride type

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

// -- Auxiliary kernel info type --

// Note: This struct is used by macro-kernels to package together extra
// parameter values that may be of use to the micro-kernel without
// cluttering up the micro-kernel interface itself.

typedef struct
{
	// Pointers to the micro-panels of A and B which will be used by the
	// next call to the micro-kernel.
	void*  a_next;
	void*  b_next;
} auxinfo_t;

// -- Context type --

typedef struct cntx_s
{
	void*  dummy;  // not used but msvc doesn't like empty structure
} cntx_t;

#define bli_auxinfo_next_a( data ) (data)->a_next
#define bli_auxinfo_next_b( data ) (data)->b_next

#define bli_auxinfo_set_next_a( data , next_a) (data)->a_next=(void*)(next_a);
#define bli_auxinfo_set_next_b( data , next_b) (data)->b_next=(void*)(next_b);

// SIMD-Register width in bits
// SSE:         128
// AVX/FMA:     256
// AVX-512:     512
#if C_AVX && (defined(_WIN64)||defined(__LP64__))
#define SIMD_REGISTER_WIDTH 256
#elif defined(__aarch64__)
#define SIMD_REGISTER_WIDTH 128
#elif defined(__SSE2__)
#define SIMD_REGISTER_WIDTH 128
#else
#define SIMD_REGISTER_WIDTH 128
#endif

#if SIMD_REGISTER_WIDTH==256
// #define alignv 32  // (rwidth / 8)
#define alignv 32   // cacheline alignment
#define vlen   4   // (rwidth / (8*sizeof(double)))
#elif SIMD_REGISTER_WIDTH==128
// #define alignv 16  // (rwidth / 8)
#define alignv 32   // cacheline alignment
#define vlen   2   // (rwidth / (8*sizeof(double)))
#else
#define alignv 8
#define vlen   1
#endif

// -- LEVEL-3 MICRO-KERNEL CONSTANTS AND DEFINITIONS ---------------------------

//
// Constraints:
//
// (1) MC must be a multiple of:
//     (a) MR (for zero-padding purposes)
//     (b) NR (for zero-padding purposes when MR and NR are "swapped")
// (2) NC must be a multiple of
//     (a) NR (for zero-padding purposes)
//     (b) MR (for zero-padding purposes when MR and NR are "swapped")
//

#define DGEMM(f)                                         \
extern void bli_dgemm_##f                                \
     (                                                   \
       dim_t               k,                            \
       double*    restrict alpha_,                       \
       double*    restrict a,                            \
       double*    restrict b,                            \
       double*    restrict beta_,                        \
       double*    restrict c, inc_t rs_c, inc_t cs_c,    \
       auxinfo_t* restrict data,                         \
       cntx_t*    restrict cntx                          \
     );

#define ZGEMM(f)                                         \
extern void bli_zgemm_##f                                \
     (                                                   \
       dim_t               k,                            \
       dcomplex*  restrict alpha_,                       \
       dcomplex*  restrict a,                            \
       dcomplex*  restrict b,                            \
       dcomplex*  restrict beta_,                        \
       dcomplex*  restrict c, inc_t rs_c, inc_t cs_c,    \
       auxinfo_t* restrict data,                         \
       cntx_t*    restrict cntx                          \
     );

#define DGEMM2(f)                                        \
extern void bli_dgemm2_##f                               \
     (                                                   \
       dim_t               k,                            \
       double*    restrict alpha_,                       \
       double*    restrict a,                            \
       double*    restrict b,                            \
       double*    restrict beta_,                        \
       double*    restrict c, inc_t rs_c, inc_t cs_c,    \
       auxinfo_t* restrict data,                         \
       cntx_t*    restrict cntx                          \
     );

#define ZGEMM2(f)                                        \
extern void bli_zgemm2_##f                               \
     (                                                   \
       dim_t               k,                            \
       dcomplex*  restrict alpha_,                       \
       dcomplex*  restrict a,                            \
       dcomplex*  restrict b,                            \
       dcomplex*  restrict beta_,                        \
       dcomplex*  restrict c, inc_t rs_c, inc_t cs_c,    \
       auxinfo_t* restrict data,                         \
       cntx_t*    restrict cntx                          \
     );


#if C_AVX && (defined(_WIN64)||defined(__LP64__))

#if 0

// haswell  /* not included in repository */

#define BLIS_DGEMM_UKERNEL         bli_dgemm_asm_6x8
#define BLIS_DEFAULT_MC_D          72
#define BLIS_DEFAULT_KC_D          256
#define BLIS_DEFAULT_NC_D          4080
#define BLIS_DEFAULT_MR_D          6
#define BLIS_DEFAULT_NR_D          8

#define BLIS_ZGEMM_UKERNEL         bli_zgemm_asm_3x4
#define BLIS_DEFAULT_MC_Z          72
#define BLIS_DEFAULT_KC_Z          256
#define BLIS_DEFAULT_NC_Z          4080
#define BLIS_DEFAULT_MR_Z          3
#define BLIS_DEFAULT_NR_Z          4

#define BLIS_DRIVER BLIS_DRIVER_HASWELL
DGEMM(asm_6x8)
ZGEMM(asm_3x4)

#else

// sandy bridge (FMA if detected)

// intrinsic is almost as good as asm
#define BLIS_DGEMM_UKERNEL         bli_dgemm_int_6x8
#define BLIS_DGEMM2_UKERNEL        bli_dgemm2_int_6x8
#define BLIS_DEFAULT_MC_D          72
#define BLIS_DEFAULT_KC_D          256
#define BLIS_DEFAULT_NC_D          4080
#define BLIS_DEFAULT_MR_D          6
#define BLIS_DEFAULT_NR_D          8

#define BLIS_ZGEMM_UKERNEL         bli_zgemm_int_3x4
#define BLIS_ZGEMM2_UKERNEL        bli_zgemm2_int_3x4
#define BLIS_DEFAULT_MC_Z          72
#define BLIS_DEFAULT_KC_Z          256
#define BLIS_DEFAULT_NC_Z          4080
#define BLIS_DEFAULT_MR_Z          3
#define BLIS_DEFAULT_NR_Z          4

#define BLIS_DRIVER BLIS_DRIVER_AVX
DGEMM(int_6x8)
ZGEMM(int_3x4)
DGEMM2(int_6x8)
ZGEMM2(int_3x4)

#endif

#elif defined(__SSE2__)

// sse2 x86_64 or x86

#define BLIS_DGEMM_UKERNEL         bli_dgemm_int_d4x4
#define BLIS_DEFAULT_MC_D          72
#define BLIS_DEFAULT_KC_D          256
#define BLIS_DEFAULT_NC_D          4080
#define BLIS_DEFAULT_MR_D          4
#define BLIS_DEFAULT_NR_D          4

#define BLIS_ZGEMM_UKERNEL         bli_zgemm_int_d2x2
#define BLIS_DEFAULT_MC_Z          72 
#define BLIS_DEFAULT_KC_Z          256
#define BLIS_DEFAULT_NC_Z          4080
#define BLIS_DEFAULT_MR_Z          2
#define BLIS_DEFAULT_NR_Z          2

#define BLIS_DRIVER BLIS_DRIVER_SSE2
DGEMM(int_d4x4)
ZGEMM(int_d2x2)

#elif defined(__aarch64__)||defined(_M_ARM64)

// armv8 neon

#define BLIS_DGEMM_UKERNEL         bli_dgemm_opt_8x6
#define BLIS_DEFAULT_MC_D          72
#define BLIS_DEFAULT_KC_D          256
#define BLIS_DEFAULT_NC_D          4080
#define BLIS_DEFAULT_MR_D          8
#define BLIS_DEFAULT_NR_D          6

#define BLIS_ZGEMM_UKERNEL         bli_zgemm_opt_4x4
#define BLIS_DEFAULT_MC_Z          64
#define BLIS_DEFAULT_KC_Z          128
#define BLIS_DEFAULT_NC_Z          4096
#define BLIS_DEFAULT_MR_Z          4
#define BLIS_DEFAULT_NR_Z          4

#define BLIS_DRIVER BLIS_DRIVER_AARCH64
DGEMM(opt_8x6)
ZGEMM(opt_4x4)

#elif defined(__GNUC__)

// gcc vector extension

#define BLIS_DGEMM_UKERNEL         bli_dgemm_128_6x4
#define BLIS_DEFAULT_MC_D          72
#define BLIS_DEFAULT_KC_D          256
#define BLIS_DEFAULT_NC_D          4080
#define BLIS_DEFAULT_MR_D          6
#define BLIS_DEFAULT_NR_D          4

#define BLIS_ZGEMM_UKERNEL         bli_zgemm_128_3x2
#define BLIS_DEFAULT_MC_Z          72
#define BLIS_DEFAULT_KC_Z          256
#define BLIS_DEFAULT_NC_Z          4080
#define BLIS_DEFAULT_MR_Z          3
#define BLIS_DEFAULT_NR_Z          2

#define BLIS_DRIVER BLIS_DRIVER_GNUVEC
DGEMM(128_6x4)
ZGEMM(128_3x2)

#else

#define BLIS_DGEMM_UKERNEL         bli_dgemm_ukn_ref
#define BLIS_DEFAULT_MC_D          96
#define BLIS_DEFAULT_KC_D          256
#define BLIS_DEFAULT_NC_D          4096
#define BLIS_DEFAULT_MR_D          4
#define BLIS_DEFAULT_NR_D          4

#define BLIS_ZGEMM_UKERNEL         bli_zgemm_ukn_ref
#define BLIS_DEFAULT_MC_Z          64 
#define BLIS_DEFAULT_KC_Z          192
#define BLIS_DEFAULT_NC_Z          4096
#define BLIS_DEFAULT_MR_Z          4
#define BLIS_DEFAULT_NR_Z          4

#define BLIS_DRIVER BLIS_DRIVER_GENERIC
DGEMM(ukn_ref)
ZGEMM(ukn_ref)

#endif

#define dgemm_micro_kernel BLIS_DGEMM_UKERNEL
#define zgemm_micro_kernel BLIS_ZGEMM_UKERNEL
#ifdef BLIS_DGEMM2_UKERNEL
#define dgemm2_micro_kernel BLIS_DGEMM2_UKERNEL
#define zgemm2_micro_kernel BLIS_ZGEMM2_UKERNEL
#else
#define dgemm2_micro_kernel BLIS_DGEMM_UKERNEL
#define zgemm2_micro_kernel BLIS_ZGEMM_UKERNEL
#endif

#endif  /*  _BLIS_HEADER */
