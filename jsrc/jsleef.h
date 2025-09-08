#ifndef J_SLEEF_H
#define J_SLEEF_H

#ifdef _OPENMP
#define SLEEF_ENABLE_OMP_SIMD
#endif

#if SLEEF
#if defined(__aarch64__)
#include "../sleef/include/aarch64/sleefinline_advsimd.h"
#elif C_AVX2
#include "../sleef/include/x86_64/sleefinline_avx2.h"
#elif defined(__SSE2__)
#include "../sleef/include/x86_64/sleefinline_sse2.h"
#endif
#endif

#if SLEEFQUAD
#if HASFMA
#include "../sleef/include/x86_64/sleefquadinline_purecfma_scalar.h"
#else
#include "../sleef/include/x86_64/sleefquadinline_purec_scalar.h"
#endif
#endif

#if SLEEF
/* arm64 --------------------------------------------------- */

// Define entry points to use for the architectures of interest
#if C_AVX2
#define Sleef_expd4 Sleef_expd4_u10avx2
#define Sleef_logd4 Sleef_logd4_u10avx2
#define Sleef_log2d4 Sleef_log2d4_u35avx2
#define Sleef_exp2d4 Sleef_exp2d4_u35avx2
#define Sleef_sind4 Sleef_sind4_u35avx2
#define Sleef_cosd4 Sleef_cosd4_u35avx2
#define Sleef_tand4 Sleef_tand4_u35avx2
#define Sleef_tanhd4 Sleef_tanhd4_u35avx2
#define Sleef_asind4 Sleef_asind4_u35avx2
#define Sleef_acosd4 Sleef_acosd4_u35avx2
#define Sleef_atand4 Sleef_atand4_u35avx2
#define IGNORENAN
#elif EMU_AVX2 && defined(__aarch64__)
#define __EMU_M256_SLEEF_IMPL_M1( type, func , ud ) \
static INLINE type Sleef_##func##d4( type m256_param1 ) \
{   type res; \
    res.vect_i128.val[0] = Sleef_##func##d2##ud##advsimd( m256_param1.vect_i128.val[0] ); \
    res.vect_i128.val[1] = Sleef_##func##d2##ud##advsimd( m256_param1.vect_i128.val[1] ); \
    return ( res ); \
}
__EMU_M256_SLEEF_IMPL_M1( __m256d, exp, _u10 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, log, _u10 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, log2, _u35 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, exp2, _u35 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, sin, _u35 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, cos, _u35 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, tan, _u35 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, tanh, _u35 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, asin, _u35 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, acos, _u35 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, atan, _u35 );
#define IGNORENAN NAN0;  // some of these functions produce NaN along the way
#elif defined(__aarch64__)
#define Sleef_expd4 Sleef_expd2_u10advsimd
#define Sleef_logd4 Sleef_logd2_u10advsimd
#define Sleef_log2d4 Sleef_log2d2_u35advsimd
#define Sleef_exp2d4 Sleef_exp2d2_u35advsimd
#define Sleef_sind4 Sleef_sind2_u35advsimd
#define Sleef_cosd4 Sleef_cosd2_u35advsimd
#define Sleef_tand4 Sleef_tand2_u35advsimd
#define Sleef_tanhd4 Sleef_tanhd2_u35advsimd
#define Sleef_asind4 Sleef_asind2_u35advsimd
#define Sleef_acosd4 Sleef_acosd2_u35advsimd
#define Sleef_atand4 Sleef_atand2_u35advsimd
#define IGNORENAN
#elif EMU_AVX2 && defined(__SSE2__)
// already renamed
#define __EMU_M256_SLEEF_IMPL_M1( type, func , ud ) \
static INLINE __emu##type Sleef_##func##d4( __emu##type m256_param1 ) \
{   __emu##type res; \
    res.__emu_m128[0] = Sleef_##func##d2##ud##sse2( m256_param1.__emu_m128[0] ); \
    res.__emu_m128[1] = Sleef_##func##d2##ud##sse2( m256_param1.__emu_m128[1] ); \
    return ( res ); \
}
__EMU_M256_SLEEF_IMPL_M1( __m256d, exp, _u10 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, log, _u10 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, log2, _u35 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, exp2, _u35 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, sin, _u35 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, cos, _u35 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, tan, _u35 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, tanh, _u35 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, asin, _u35 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, acos, _u35 );
__EMU_M256_SLEEF_IMPL_M1( __m256d, atan, _u35 );
#define IGNORENAN NAN0;  // some of these functions produce NaN along the way
#elif defined(__SSE2__)
#define Sleef_expd4 Sleef_expd2_u10sse2
#define Sleef_logd4 Sleef_logd2_u10sse2
#define Sleef_log2d4 Sleef_log2d2_u35sse2
#define Sleef_exp2d4 Sleef_exp2d2_u35sse2
#define Sleef_sind4 Sleef_sind2_u35sse2
#define Sleef_cosd4 Sleef_cosd2_u35sse2
#define Sleef_tand4 Sleef_tand2_u35sse2
#define Sleef_tanhd4 Sleef_tanhd2_u35sse2
#define Sleef_asind4 Sleef_asind2_u35sse2
#define Sleef_acosd4 Sleef_acosd2_u35sse2
#define Sleef_atand4 Sleef_atand2_u35sse2
#define IGNORENAN
#endif

#endif

#endif /* J_SLEEF_H */
