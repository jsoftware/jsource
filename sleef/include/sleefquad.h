//   Copyright Naoki Shibata and contributors 2010 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __SLEEFQUAD_H__
#define __SLEEFQUAD_H__

#define SLEEF_VERSION_MAJOR 3
#define SLEEF_VERSION_MINOR 6
#define SLEEF_VERSION_PATCHLEVEL 0

#include "sleef.h"
#include <string.h>
// #define Sleef_quad_DEFINED
#ifdef __cplusplus
extern "C"
{
#endif

#if (defined(__MINGW32__) || defined(__MINGW64__) || defined(__CYGWIN__) || defined(_MSC_VER)) && !defined(SLEEF_STATIC_LIBS)
#ifdef SLEEF_IMPORT_IS_EXPORT
#define SLEEF_IMPORT __declspec(dllexport)
#else // #ifdef SLEEF_IMPORT_IS_EXPORT
#define SLEEF_IMPORT __declspec(dllimport)
#if (defined(_MSC_VER))
#pragma comment(lib,"sleefquad.lib")
#endif // #if (defined(_MSC_VER))
#endif // #ifdef SLEEF_IMPORT_IS_EXPORT
#else // #if (defined(__MINGW32__) || defined(__MINGW64__) || defined(__CYGWIN__) || defined(_MSC_VER)) && !defined(SLEEF_STATIC_LIBS)
#define SLEEF_IMPORT
#endif // #if (defined(__MINGW32__) || defined(__MINGW64__) || defined(__CYGWIN__) || defined(_MSC_VER)) && !defined(SLEEF_STATIC_LIBS)

//

#if defined (__GNUC__) || defined (__clang__) || defined(__INTEL_COMPILER)
#define SLEEF_CONST __attribute__((const))
#define SLEEF_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define SLEEF_CONST
#define SLEEF_INLINE __forceinline
#endif

//

#if (defined(__SIZEOF_FLOAT128__) && __SIZEOF_FLOAT128__ == 16) || (defined(__linux__) && defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))) || (defined(__PPC64__) && defined(__GNUC__) && !defined(__clang__) && __GNUC__ >= 8)
#define SLEEF_FLOAT128_IS_IEEEQP
#endif

#if !defined(SLEEF_FLOAT128_IS_IEEEQP) && defined(__SIZEOF_LONG_DOUBLE__) && __SIZEOF_LONG_DOUBLE__ == 16 && (defined(__aarch64__) || defined(__zarch__))
#define SLEEF_LONGDOUBLE_IS_IEEEQP
#endif

#if !defined(Sleef_quad_DEFINED)
#define Sleef_quad_DEFINED
typedef struct { uint64_t x, y; } Sleef_uint64_2t;
#if defined(SLEEF_FLOAT128_IS_IEEEQP)
typedef __float128 Sleef_quad;
#define SLEEF_QUAD_C(x) (x ## Q)
#elif defined(SLEEF_LONGDOUBLE_IS_IEEEQP)
typedef long double Sleef_quad;
#define SLEEF_QUAD_C(x) (x ## L)
#else
typedef Sleef_uint64_2t Sleef_quad;
#endif
#endif

//

#if !defined(Sleef_quadx1_DEFINED)
#define Sleef_quadx1_DEFINED
typedef struct {
  uint64_t x, y;
} Sleef_quadx1;
#endif

#if !defined(Sleef_quadx2_DEFINED)

#if defined(__SSE2__)
#define Sleef_quadx2_DEFINED
typedef struct {
  __m128i x, y;
} Sleef_quadx2;
#endif

#if defined(__aarch64__)
#define Sleef_quadx2_DEFINED
typedef struct {
  uint32x4_t x, y;
} Sleef_quadx2;
#endif

#if defined(__VSX__)
#define Sleef_quadx2_DEFINED
typedef struct {
  __vector unsigned int x, y;
} Sleef_quadx2;
#endif

#if defined(__VX__) && (defined(__VECTOR_KEYWORD_SUPPORTED__) || defined(__VEC__))
#define Sleef_quadx2_DEFINED
typedef struct {
  __vector unsigned long long x, y;
} Sleef_quadx2;
#endif

#endif // #if !defined(Sleef_quadx2_DEFINED)

#if !defined(Sleef_quadx4_DEFINED)
#if defined(__AVX__)
#define Sleef_quadx4_DEFINED
typedef struct {
  __m256i x, y;
} Sleef_quadx4;
#endif
#endif

#if !defined(Sleef_quadx8_DEFINED)
#if defined(__AVX512F__)
#define Sleef_quadx8_DEFINED
typedef struct {
  __m512i x, y;
} Sleef_quadx8;
#endif
#endif

#if !defined(Sleef_svquad_DEFINED)
#if defined(__ARM_FEATURE_SVE)
#define Sleef_svquad_DEFINED
typedef svfloat64x2_t Sleef_svquad;
#endif
#endif

//

#if !defined(SLEEF_Q_DEFINED)
#define SLEEF_Q_DEFINED
static inline Sleef_quad sleef_q(int64_t H, uint64_t L, int E) {
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
  struct { uint64_t h, l; } c;
#else
  struct { uint64_t l, h; } c;
#endif
  c.h = (((int64_t)(H) < 0 ? 1ULL : 0ULL) << 63) |
    ((0x7fff & (uint64_t)((E) + 16383)) << 48) |
    ((int64_t)(H) < 0 ? -(int64_t)(H) : (int64_t)(H) & 0xffffffffffffULL);
  c.l = (uint64_t)(L);

  Sleef_quad q;
  memcpy(&q, &c, 16);
  return q;
}
#endif

//

#ifdef SLEEF_QUAD_C

#define SLEEF_M_Eq SLEEF_QUAD_C(+0x1.5bf0a8b1457695355fb8ac404e7ap+1)
#define SLEEF_M_LOG2Eq SLEEF_QUAD_C(+0x1.71547652b82fe1777d0ffda0d23ap+0)
#define SLEEF_M_LOG10Eq SLEEF_QUAD_C(+0x1.bcb7b1526e50e32a6ab7555f5a68p-2)
#define SLEEF_M_LN2q SLEEF_QUAD_C(+0x1.62e42fefa39ef35793c7673007e6p-1)
#define SLEEF_M_LN10q SLEEF_QUAD_C(+0x1.26bb1bbb5551582dd4adac5705a6p+1)
#define SLEEF_M_PIq SLEEF_QUAD_C(+0x1.921fb54442d18469898cc51701b8p+1)
#define SLEEF_M_PI_2q SLEEF_QUAD_C(+0x1.921fb54442d18469898cc51701b8p+0)
#define SLEEF_M_PI_4q SLEEF_QUAD_C(+0x1.921fb54442d18469898cc51701b8p-1)
#define SLEEF_M_1_PIq SLEEF_QUAD_C(+0x1.45f306dc9c882a53f84eafa3ea6ap-2)
#define SLEEF_M_2_PIq SLEEF_QUAD_C(+0x1.45f306dc9c882a53f84eafa3ea6ap-1)
#define SLEEF_M_2_SQRTPIq SLEEF_QUAD_C(+0x1.20dd750429b6d11ae3a914fed7fep+0)
#define SLEEF_M_SQRT2q SLEEF_QUAD_C(+0x1.6a09e667f3bcc908b2fb1366ea95p+0)
#define SLEEF_M_SQRT3q SLEEF_QUAD_C(+0x1.bb67ae8584caa73b25742d7078b8p+0)
#define SLEEF_M_INV_SQRT3q SLEEF_QUAD_C(+0x1.279a74590331c4d218f81e4afb25p-1)
#define SLEEF_M_SQRT1_2q SLEEF_QUAD_C(+0x1.6a09e667f3bcc908b2fb1366ea95p-1)
#define SLEEF_M_INV_SQRTPIq SLEEF_QUAD_C(+0x1.20dd750429b6d11ae3a914fed7fep-1)
#define SLEEF_M_EGAMMAq SLEEF_QUAD_C(+0x1.2788cfc6fb618f49a37c7f0202a6p-1)
#define SLEEF_M_PHIq SLEEF_QUAD_C(+0x1.9e3779b97f4a7c15f39cc0605ceep+0)
#define SLEEF_QUAD_MAX SLEEF_QUAD_C(+0x1.ffffffffffffffffffffffffffffp+16383)
#define SLEEF_QUAD_MIN SLEEF_QUAD_C(+0x1p-16382)
#define SLEEF_QUAD_EPSILON SLEEF_QUAD_C(+0x1p-112)
#define SLEEF_QUAD_DENORM_MIN SLEEF_QUAD_C(+0x0.0000000000000000000000000001p-16382)

#else // #ifdef SLEEF_QUAD_C

#define SLEEF_M_Eq sleef_q(+0x15bf0a8b14576LL, 0x95355fb8ac404e7aULL, 1)
#define SLEEF_M_LOG2Eq sleef_q(+0x171547652b82fLL, 0xe1777d0ffda0d23aULL, 0)
#define SLEEF_M_LOG10Eq sleef_q(+0x1bcb7b1526e50LL, 0xe32a6ab7555f5a68ULL, -2)
#define SLEEF_M_LN2q sleef_q(+0x162e42fefa39eLL, 0xf35793c7673007e6ULL, -1)
#define SLEEF_M_LN10q sleef_q(+0x126bb1bbb5551LL, 0x582dd4adac5705a6ULL, 1)
#define SLEEF_M_PIq sleef_q(+0x1921fb54442d1LL, 0x8469898cc51701b8ULL, 1)
#define SLEEF_M_PI_2q sleef_q(+0x1921fb54442d1LL, 0x8469898cc51701b8ULL, 0)
#define SLEEF_M_PI_4q sleef_q(+0x1921fb54442d1LL, 0x8469898cc51701b8ULL, -1)
#define SLEEF_M_1_PIq sleef_q(+0x145f306dc9c88LL, 0x2a53f84eafa3ea6aULL, -2)
#define SLEEF_M_2_PIq sleef_q(+0x145f306dc9c88LL, 0x2a53f84eafa3ea6aULL, -1)
#define SLEEF_M_2_SQRTPIq sleef_q(+0x120dd750429b6LL, 0xd11ae3a914fed7feULL, 0)
#define SLEEF_M_SQRT2q sleef_q(+0x16a09e667f3bcLL, 0xc908b2fb1366ea95ULL, 0)
#define SLEEF_M_SQRT3q sleef_q(+0x1bb67ae8584caLL, 0xa73b25742d7078b8ULL, 0)
#define SLEEF_M_INV_SQRT3q sleef_q(+0x1279a74590331LL, 0xc4d218f81e4afb25ULL, -1)
#define SLEEF_M_SQRT1_2q sleef_q(+0x16a09e667f3bcLL, 0xc908b2fb1366ea95ULL, -1)
#define SLEEF_M_INV_SQRTPIq sleef_q(+0x120dd750429b6LL, 0xd11ae3a914fed7feULL, -1)
#define SLEEF_M_EGAMMAq sleef_q(+0x12788cfc6fb61LL, 0x8f49a37c7f0202a6ULL, -1)
#define SLEEF_M_PHIq sleef_q(+0x19e3779b97f4aLL, 0x7c15f39cc0605ceeULL, 0)
#define SLEEF_QUAD_MAX sleef_q(+0x1ffffffffffffLL, 0xffffffffffffffffULL, 16383)
#define SLEEF_QUAD_MIN sleef_q(+0x1000000000000LL, 0x0000000000000000ULL, -16382)
#define SLEEF_QUAD_EPSILON sleef_q(+0x1000000000000LL, 0x0000000000000000ULL, -112)
#define SLEEF_QUAD_DENORM_MIN sleef_q(+0x0000000000000LL, 0x0000000000000001ULL, -16382)

#endif // #ifdef SLEEF_QUAD_C

#define SLEEF_QUAD_MANT_DIG 113
#define SLEEF_QUAD_MIN_EXP (-16381)
#define SLEEF_QUAD_MAX_EXP 16384
#define SLEEF_QUAD_DIG 33
#define SLEEF_QUAD_MIN_10_EXP (-4931)
#define SLEEF_QUAD_MAX_10_EXP 4932

//

#include <stdio.h>
#include <stdarg.h>

SLEEF_IMPORT Sleef_quad Sleef_strtoq(const char *str, char **endptr);

SLEEF_IMPORT int Sleef_fprintf(FILE *fp, const char *fmt, ...);
SLEEF_IMPORT int Sleef_vfprintf(FILE *fp, const char *fmt, va_list ap);
SLEEF_IMPORT int Sleef_printf(const char *fmt, ...);
SLEEF_IMPORT int Sleef_vprintf(const char *fmt, va_list ap);
SLEEF_IMPORT int Sleef_snprintf(char *str, size_t size, const char *fmt, ...);
SLEEF_IMPORT int Sleef_vsnprintf(char *str, size_t size, const char *fmt, va_list ap);

SLEEF_IMPORT int Sleef_registerPrintfHook();
SLEEF_IMPORT void Sleef_unregisterPrintfHook();

//

#ifdef __SSE2__
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_addq2_u05sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_subq2_u05sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_mulq2_u05sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_divq2_u05sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_negq2_sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_sqrtq2_u05sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cbrtq2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpltq2_sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpleq2_sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpgtq2_sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpgeq2_sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpeqq2_sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpneq2_sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpq2_sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_iunordq2_sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_iselectq2_sse2(__m128i, Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST __m128d Sleef_cast_to_doubleq2_sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cast_from_doubleq2_sse2(__m128d);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_cast_to_int64q2_sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cast_from_int64q2_sse2(__m128i);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_cast_to_uint64q2_sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cast_from_uint64q2_sse2(__m128i);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_loadq2_sse2(Sleef_quad *);
SLEEF_IMPORT SLEEF_CONST void Sleef_storeq2_sse2(Sleef_quad *, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_getq2_sse2(Sleef_quadx2, int);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_setq2_sse2(Sleef_quadx2, int, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_splatq2_sse2(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_sinq2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cosq2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_tanq2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_asinq2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_acosq2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_atanq2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_atan2q2_u10sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_expq2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_exp2q2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_exp10q2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_expm1q2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_logq2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_log2q2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_log10q2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_log1pq2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_powq2_u10sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_sinhq2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_coshq2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_tanhq2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_asinhq2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_acoshq2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_atanhq2_u10sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_truncq2_sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_floorq2_sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_ceilq2_sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_roundq2_sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_rintq2_sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fabsq2_sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_copysignq2_sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fmaxq2_sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fminq2_sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fdimq2_u05sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fmodq2_sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_remainderq2_sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT Sleef_quadx2 Sleef_frexpq2_sse2(Sleef_quadx2, __m128i *);
SLEEF_IMPORT Sleef_quadx2 Sleef_modfq2_sse2(Sleef_quadx2, Sleef_quadx2 *);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_hypotq2_u05sse2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_ldexpq2_sse2(Sleef_quadx2, __m128i);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_ilogbq2_sse2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fmaq2_u05sse2(Sleef_quadx2, Sleef_quadx2, Sleef_quadx2);
#endif
#ifdef __SSE2__
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_addq2_u05avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_subq2_u05avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_mulq2_u05avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_divq2_u05avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_negq2_avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_sqrtq2_u05avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cbrtq2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpltq2_avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpleq2_avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpgtq2_avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpgeq2_avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpeqq2_avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpneq2_avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpq2_avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_iunordq2_avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_iselectq2_avx2128(__m128i, Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST __m128d Sleef_cast_to_doubleq2_avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cast_from_doubleq2_avx2128(__m128d);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_cast_to_int64q2_avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cast_from_int64q2_avx2128(__m128i);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_cast_to_uint64q2_avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cast_from_uint64q2_avx2128(__m128i);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_loadq2_avx2128(Sleef_quad *);
SLEEF_IMPORT SLEEF_CONST void Sleef_storeq2_avx2128(Sleef_quad *, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_getq2_avx2128(Sleef_quadx2, int);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_setq2_avx2128(Sleef_quadx2, int, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_splatq2_avx2128(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_sinq2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cosq2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_tanq2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_asinq2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_acosq2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_atanq2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_atan2q2_u10avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_expq2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_exp2q2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_exp10q2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_expm1q2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_logq2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_log2q2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_log10q2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_log1pq2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_powq2_u10avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_sinhq2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_coshq2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_tanhq2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_asinhq2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_acoshq2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_atanhq2_u10avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_truncq2_avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_floorq2_avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_ceilq2_avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_roundq2_avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_rintq2_avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fabsq2_avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_copysignq2_avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fmaxq2_avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fminq2_avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fdimq2_u05avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fmodq2_avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_remainderq2_avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT Sleef_quadx2 Sleef_frexpq2_avx2128(Sleef_quadx2, __m128i *);
SLEEF_IMPORT Sleef_quadx2 Sleef_modfq2_avx2128(Sleef_quadx2, Sleef_quadx2 *);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_hypotq2_u05avx2128(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_ldexpq2_avx2128(Sleef_quadx2, __m128i);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_ilogbq2_avx2128(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fmaq2_u05avx2128(Sleef_quadx2, Sleef_quadx2, Sleef_quadx2);
#endif
#ifdef __AVX__
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_addq4_u05avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_subq4_u05avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_mulq4_u05avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_divq4_u05avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_negq4_avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_sqrtq4_u05avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_cbrtq4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpltq4_avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpleq4_avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpgtq4_avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpgeq4_avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpeqq4_avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpneq4_avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpq4_avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_iunordq4_avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_iselectq4_avx2(__m128i, Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST __m256d Sleef_cast_to_doubleq4_avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_cast_from_doubleq4_avx2(__m256d);
SLEEF_IMPORT SLEEF_CONST __m256i Sleef_cast_to_int64q4_avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_cast_from_int64q4_avx2(__m256i);
SLEEF_IMPORT SLEEF_CONST __m256i Sleef_cast_to_uint64q4_avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_cast_from_uint64q4_avx2(__m256i);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_loadq4_avx2(Sleef_quad *);
SLEEF_IMPORT SLEEF_CONST void Sleef_storeq4_avx2(Sleef_quad *, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_getq4_avx2(Sleef_quadx4, int);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_setq4_avx2(Sleef_quadx4, int, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_splatq4_avx2(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_sinq4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_cosq4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_tanq4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_asinq4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_acosq4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_atanq4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_atan2q4_u10avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_expq4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_exp2q4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_exp10q4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_expm1q4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_logq4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_log2q4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_log10q4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_log1pq4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_powq4_u10avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_sinhq4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_coshq4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_tanhq4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_asinhq4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_acoshq4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_atanhq4_u10avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_truncq4_avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_floorq4_avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_ceilq4_avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_roundq4_avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_rintq4_avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_fabsq4_avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_copysignq4_avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_fmaxq4_avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_fminq4_avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_fdimq4_u05avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_fmodq4_avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_remainderq4_avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT Sleef_quadx4 Sleef_frexpq4_avx2(Sleef_quadx4, __m128i *);
SLEEF_IMPORT Sleef_quadx4 Sleef_modfq4_avx2(Sleef_quadx4, Sleef_quadx4 *);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_hypotq4_u05avx2(Sleef_quadx4, Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_ldexpq4_avx2(Sleef_quadx4, __m128i);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_ilogbq4_avx2(Sleef_quadx4);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx4 Sleef_fmaq4_u05avx2(Sleef_quadx4, Sleef_quadx4, Sleef_quadx4);
#endif
#ifdef __AVX512F__
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_addq8_u05avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_subq8_u05avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_mulq8_u05avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_divq8_u05avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_negq8_avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_sqrtq8_u05avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_cbrtq8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST __m256i Sleef_icmpltq8_avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST __m256i Sleef_icmpleq8_avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST __m256i Sleef_icmpgtq8_avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST __m256i Sleef_icmpgeq8_avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST __m256i Sleef_icmpeqq8_avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST __m256i Sleef_icmpneq8_avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST __m256i Sleef_icmpq8_avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST __m256i Sleef_iunordq8_avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_iselectq8_avx512f(__m256i, Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST __m512d Sleef_cast_to_doubleq8_avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_cast_from_doubleq8_avx512f(__m512d);
SLEEF_IMPORT SLEEF_CONST __m512i Sleef_cast_to_int64q8_avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_cast_from_int64q8_avx512f(__m512i);
SLEEF_IMPORT SLEEF_CONST __m512i Sleef_cast_to_uint64q8_avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_cast_from_uint64q8_avx512f(__m512i);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_loadq8_avx512f(Sleef_quad *);
SLEEF_IMPORT SLEEF_CONST void Sleef_storeq8_avx512f(Sleef_quad *, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_getq8_avx512f(Sleef_quadx8, int);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_setq8_avx512f(Sleef_quadx8, int, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_splatq8_avx512f(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_sinq8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_cosq8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_tanq8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_asinq8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_acosq8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_atanq8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_atan2q8_u10avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_expq8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_exp2q8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_exp10q8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_expm1q8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_logq8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_log2q8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_log10q8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_log1pq8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_powq8_u10avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_sinhq8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_coshq8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_tanhq8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_asinhq8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_acoshq8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_atanhq8_u10avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_truncq8_avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_floorq8_avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_ceilq8_avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_roundq8_avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_rintq8_avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_fabsq8_avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_copysignq8_avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_fmaxq8_avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_fminq8_avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_fdimq8_u05avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_fmodq8_avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_remainderq8_avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT Sleef_quadx8 Sleef_frexpq8_avx512f(Sleef_quadx8, __m256i *);
SLEEF_IMPORT Sleef_quadx8 Sleef_modfq8_avx512f(Sleef_quadx8, Sleef_quadx8 *);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_hypotq8_u05avx512f(Sleef_quadx8, Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_ldexpq8_avx512f(Sleef_quadx8, __m256i);
SLEEF_IMPORT SLEEF_CONST __m256i Sleef_ilogbq8_avx512f(Sleef_quadx8);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx8 Sleef_fmaq8_u05avx512f(Sleef_quadx8, Sleef_quadx8, Sleef_quadx8);
#endif
#ifdef __STDC__
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_addq1_u05purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_subq1_u05purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_mulq1_u05purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_divq1_u05purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_negq1_purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_sqrtq1_u05purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_cbrtq1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpltq1_purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpleq1_purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpgtq1_purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpgeq1_purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpeqq1_purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpneq1_purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpq1_purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_iunordq1_purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_iselectq1_purec(int32_t, Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST double Sleef_cast_to_doubleq1_purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_cast_from_doubleq1_purec(double);
SLEEF_IMPORT SLEEF_CONST int64_t Sleef_cast_to_int64q1_purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_cast_from_int64q1_purec(int64_t);
SLEEF_IMPORT SLEEF_CONST uint64_t Sleef_cast_to_uint64q1_purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_cast_from_uint64q1_purec(uint64_t);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_loadq1_purec(Sleef_quad *);
SLEEF_IMPORT             void Sleef_storeq1_purec(Sleef_quad *, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_getq1_purec(Sleef_quad, int);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_setq1_purec(Sleef_quad, int, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_splatq1_purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_sinq1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_cosq1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_tanq1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_asinq1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_acosq1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_atanq1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_atan2q1_u10purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_expq1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_exp2q1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_exp10q1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_expm1q1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_logq1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_log2q1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_log10q1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_log1pq1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_powq1_u10purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_sinhq1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_coshq1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_tanhq1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_asinhq1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_acoshq1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_atanhq1_u10purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_truncq1_purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_floorq1_purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_ceilq1_purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_roundq1_purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_rintq1_purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fabsq1_purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_copysignq1_purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fmaxq1_purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fminq1_purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fdimq1_u05purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fmodq1_purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_remainderq1_purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT Sleef_quad Sleef_frexpq1_purec(Sleef_quad, int32_t *);
SLEEF_IMPORT Sleef_quad Sleef_modfq1_purec(Sleef_quad, Sleef_quad *);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_hypotq1_u05purec(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_ldexpq1_purec(Sleef_quad, int32_t);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_ilogbq1_purec(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fmaq1_u05purec(Sleef_quad, Sleef_quad, Sleef_quad);
#endif
#ifdef __STDC__
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_addq1_u05purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_subq1_u05purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_mulq1_u05purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_divq1_u05purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_negq1_purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_sqrtq1_u05purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_cbrtq1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpltq1_purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpleq1_purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpgtq1_purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpgeq1_purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpeqq1_purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpneq1_purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpq1_purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_iunordq1_purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_iselectq1_purecfma(int32_t, Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST double Sleef_cast_to_doubleq1_purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_cast_from_doubleq1_purecfma(double);
SLEEF_IMPORT SLEEF_CONST int64_t Sleef_cast_to_int64q1_purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_cast_from_int64q1_purecfma(int64_t);
SLEEF_IMPORT SLEEF_CONST uint64_t Sleef_cast_to_uint64q1_purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_cast_from_uint64q1_purecfma(uint64_t);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_loadq1_purecfma(Sleef_quad *);
SLEEF_IMPORT             void Sleef_storeq1_purecfma(Sleef_quad *, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_getq1_purecfma(Sleef_quad, int);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_setq1_purecfma(Sleef_quad, int, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_splatq1_purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_sinq1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_cosq1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_tanq1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_asinq1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_acosq1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_atanq1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_atan2q1_u10purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_expq1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_exp2q1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_exp10q1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_expm1q1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_logq1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_log2q1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_log10q1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_log1pq1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_powq1_u10purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_sinhq1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_coshq1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_tanhq1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_asinhq1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_acoshq1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_atanhq1_u10purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_truncq1_purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_floorq1_purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_ceilq1_purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_roundq1_purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_rintq1_purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fabsq1_purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_copysignq1_purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fmaxq1_purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fminq1_purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fdimq1_u05purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fmodq1_purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_remainderq1_purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT Sleef_quad Sleef_frexpq1_purecfma(Sleef_quad, int32_t *);
SLEEF_IMPORT Sleef_quad Sleef_modfq1_purecfma(Sleef_quad, Sleef_quad *);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_hypotq1_u05purecfma(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_ldexpq1_purecfma(Sleef_quad, int32_t);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_ilogbq1_purecfma(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fmaq1_u05purecfma(Sleef_quad, Sleef_quad, Sleef_quad);
#endif
#ifdef __STDC__
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_addq1_u05(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_subq1_u05(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_mulq1_u05(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_divq1_u05(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_negq1(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_sqrtq1_u05(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_cbrtq1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpltq1(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpleq1(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpgtq1(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpgeq1(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpeqq1(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpneq1(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_icmpq1(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_iunordq1(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_iselectq1(int32_t, Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST double Sleef_cast_to_doubleq1(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_cast_from_doubleq1(double);
SLEEF_IMPORT SLEEF_CONST int64_t Sleef_cast_to_int64q1(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_cast_from_int64q1(int64_t);
SLEEF_IMPORT SLEEF_CONST uint64_t Sleef_cast_to_uint64q1(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_cast_from_uint64q1(uint64_t);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_loadq1(Sleef_quad *);
SLEEF_IMPORT             void Sleef_storeq1(Sleef_quad *, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_getq1(Sleef_quad, int);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_setq1(Sleef_quad, int, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_splatq1(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_sinq1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_cosq1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_tanq1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_asinq1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_acosq1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_atanq1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_atan2q1_u10(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_expq1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_exp2q1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_exp10q1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_expm1q1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_logq1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_log2q1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_log10q1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_log1pq1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_powq1_u10(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_sinhq1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_coshq1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_tanhq1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_asinhq1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_acoshq1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_atanhq1_u10(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_truncq1(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_floorq1(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_ceilq1(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_roundq1(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_rintq1(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fabsq1(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_copysignq1(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fmaxq1(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fminq1(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fdimq1_u05(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fmodq1(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_remainderq1(Sleef_quad, Sleef_quad);
SLEEF_IMPORT Sleef_quad Sleef_frexpq1(Sleef_quad, int32_t *);
SLEEF_IMPORT Sleef_quad Sleef_modfq1(Sleef_quad, Sleef_quad *);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_hypotq1_u05(Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_ldexpq1(Sleef_quad, int32_t);
SLEEF_IMPORT SLEEF_CONST int32_t Sleef_ilogbq1(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_fmaq1_u05(Sleef_quad, Sleef_quad, Sleef_quad);
#endif
#ifdef __SSE2__
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_addq2_u05(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_subq2_u05(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_mulq2_u05(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_divq2_u05(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_negq2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_sqrtq2_u05(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cbrtq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpltq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpleq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpgtq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpgeq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpeqq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpneq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_icmpq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_iunordq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_iselectq2(__m128i, Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST __m128d Sleef_cast_to_doubleq2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cast_from_doubleq2(__m128d);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_cast_to_int64q2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cast_from_int64q2(__m128i);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_cast_to_uint64q2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cast_from_uint64q2(__m128i);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_loadq2(Sleef_quad *);
SLEEF_IMPORT SLEEF_CONST void Sleef_storeq2(Sleef_quad *, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_getq2(Sleef_quadx2, int);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_setq2(Sleef_quadx2, int, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_splatq2(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_sinq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cosq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_tanq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_asinq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_acosq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_atanq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_atan2q2_u10(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_expq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_exp2q2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_exp10q2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_expm1q2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_logq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_log2q2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_log10q2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_log1pq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_powq2_u10(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_sinhq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_coshq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_tanhq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_asinhq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_acoshq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_atanhq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_truncq2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_floorq2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_ceilq2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_roundq2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_rintq2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fabsq2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_copysignq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fmaxq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fminq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fdimq2_u05(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fmodq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_remainderq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT Sleef_quadx2 Sleef_frexpq2(Sleef_quadx2, __m128i *);
SLEEF_IMPORT Sleef_quadx2 Sleef_modfq2(Sleef_quadx2, Sleef_quadx2 *);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_hypotq2_u05(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_ldexpq2(Sleef_quadx2, __m128i);
SLEEF_IMPORT SLEEF_CONST __m128i Sleef_ilogbq2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fmaq2_u05(Sleef_quadx2, Sleef_quadx2, Sleef_quadx2);
#endif

//

#if defined(SLEEF_REPLACE_LIBQUADMATH_FUNCS)

#ifdef SLEEF_QUAD_C

#define M_Eq SLEEF_QUAD_C(+0x1.5bf0a8b1457695355fb8ac404e7ap+1)
#define M_LOG2Eq SLEEF_QUAD_C(+0x1.71547652b82fe1777d0ffda0d23ap+0)
#define M_LOG10Eq SLEEF_QUAD_C(+0x1.bcb7b1526e50e32a6ab7555f5a68p-2)
#define M_LN2q SLEEF_QUAD_C(+0x1.62e42fefa39ef35793c7673007e6p-1)
#define M_LN10q SLEEF_QUAD_C(+0x1.26bb1bbb5551582dd4adac5705a6p+1)
#define M_PIq SLEEF_QUAD_C(+0x1.921fb54442d18469898cc51701b8p+1)
#define M_PI_2q SLEEF_QUAD_C(+0x1.921fb54442d18469898cc51701b8p+0)
#define M_PI_4q SLEEF_QUAD_C(+0x1.921fb54442d18469898cc51701b8p-1)
#define M_1_PIq SLEEF_QUAD_C(+0x1.45f306dc9c882a53f84eafa3ea6ap-2)
#define M_2_PIq SLEEF_QUAD_C(+0x1.45f306dc9c882a53f84eafa3ea6ap-1)
#define M_2_SQRTPIq SLEEF_QUAD_C(+0x1.20dd750429b6d11ae3a914fed7fep+0)
#define M_SQRT2q SLEEF_QUAD_C(+0x1.6a09e667f3bcc908b2fb1366ea95p+0)
#define M_SQRT1_2q SLEEF_QUAD_C(+0x1.6a09e667f3bcc908b2fb1366ea95p-1)
#define FLT128_MAX SLEEF_QUAD_C(+0x1.ffffffffffffffffffffffffffffp+16383)
#define FLT128_MIN SLEEF_QUAD_C(+0x1p-16382)
#define FLT128_EPSILON SLEEF_QUAD_C(+0x1p-112)
#define FLT128_DENORM_MIN SLEEF_QUAD_C(+0x0.0000000000000000000000000001p-16382)

#else // #ifdef SLEEF_QUAD_C

#define M_Eq sleef_q(+0x15bf0a8b14576LL, 0x95355fb8ac404e7aULL, 1)
#define M_LOG2Eq sleef_q(+0x171547652b82fLL, 0xe1777d0ffda0d23aULL, 0)
#define M_LOG10Eq sleef_q(+0x1bcb7b1526e50LL, 0xe32a6ab7555f5a68ULL, -2)
#define M_LN2q sleef_q(+0x162e42fefa39eLL, 0xf35793c7673007e6ULL, -1)
#define M_LN10q sleef_q(+0x126bb1bbb5551LL, 0x582dd4adac5705a6ULL, 1)
#define M_PIq sleef_q(+0x1921fb54442d1LL, 0x8469898cc51701b8ULL, 1)
#define M_PI_2q sleef_q(+0x1921fb54442d1LL, 0x8469898cc51701b8ULL, 0)
#define M_PI_4q sleef_q(+0x1921fb54442d1LL, 0x8469898cc51701b8ULL, -1)
#define M_1_PIq sleef_q(+0x145f306dc9c88LL, 0x2a53f84eafa3ea6aULL, -2)
#define M_2_PIq sleef_q(+0x145f306dc9c88LL, 0x2a53f84eafa3ea6aULL, -1)
#define M_2_SQRTPIq sleef_q(+0x120dd750429b6LL, 0xd11ae3a914fed7feULL, 0)
#define M_SQRT2q sleef_q(+0x16a09e667f3bcLL, 0xc908b2fb1366ea95ULL, 0)
#define M_SQRT1_2q sleef_q(+0x16a09e667f3bcLL, 0xc908b2fb1366ea95ULL, -1)
#define FLT128_MAX sleef_q(+0x1ffffffffffffLL, 0xffffffffffffffffULL, 16383)
#define FLT128_MIN sleef_q(+0x1000000000000LL, 0x0000000000000000ULL, -16382)
#define FLT128_EPSILON sleef_q(+0x1000000000000LL, 0x0000000000000000ULL, -112)
#define FLT128_DENORM_MIN sleef_q(+0x0000000000000LL, 0x0000000000000001ULL, -16382)

#endif // #ifdef SLEEF_QUAD_C

#define FLT128_MANT_DIG 113
#define FLT128_MIN_EXP (-16381)
#define FLT128_MAX_EXP 16384
#define FLT128_DIG 33
#define FLT128_MIN_10_EXP (-4931)
#define FLT128_MAX_10_EXP 4932

#endif // #if defined(SLEEF_REPLACE_LIBQUADMATH_FUNCS)

#ifdef __cplusplus
} // extern "C"

namespace sleef {
#endif

#if defined(SLEEF_REPLACE_LIBQUADMATH_FUNCS) || defined(__cplusplus)
static SLEEF_CONST Sleef_quad strtoflt128(const char *str, char **endptr) { return Sleef_strtoq(str, endptr); }
static SLEEF_CONST int quadmath_snprintf(char *str, size_t size, const char *fmt) { return Sleef_snprintf(str, size, fmt); }
static SLEEF_CONST Sleef_quad acosq(Sleef_quad x) { return Sleef_acosq1_u10(x); }
static SLEEF_CONST Sleef_quad acoshq(Sleef_quad x) { return Sleef_acoshq1_u10(x); }
static SLEEF_CONST Sleef_quad asinq(Sleef_quad x) { return Sleef_asinq1_u10(x); }
static SLEEF_CONST Sleef_quad asinhq(Sleef_quad x) { return Sleef_asinhq1_u10(x); }
static SLEEF_CONST Sleef_quad atanq(Sleef_quad x) { return Sleef_atanq1_u10(x); }
static SLEEF_CONST Sleef_quad atanhq(Sleef_quad x) { return Sleef_atanhq1_u10(x); }
static SLEEF_CONST Sleef_quad atan2q(Sleef_quad y, Sleef_quad x) { return Sleef_atan2q1_u10(y, x); }
static SLEEF_CONST Sleef_quad cbrtq(Sleef_quad x) { return Sleef_cbrtq1_u10(x); }
static SLEEF_CONST Sleef_quad ceilq(Sleef_quad x) { return Sleef_ceilq1(x); }
static SLEEF_CONST Sleef_quad copysignq(Sleef_quad x, Sleef_quad y) { return Sleef_copysignq1(x, y); }
static SLEEF_CONST Sleef_quad coshq(Sleef_quad x) { return Sleef_coshq1_u10(x); }
static SLEEF_CONST Sleef_quad cosq(Sleef_quad x) { return Sleef_cosq1_u10(x); }
static SLEEF_CONST Sleef_quad expq(Sleef_quad x) { return Sleef_expq1_u10(x); }
static SLEEF_CONST Sleef_quad expm1q(Sleef_quad x) { return Sleef_expm1q1_u10(x); }
static SLEEF_CONST Sleef_quad fabsq(Sleef_quad x) { return Sleef_fabsq1(x); }
static SLEEF_CONST Sleef_quad fdimq(Sleef_quad x, Sleef_quad y) { return Sleef_fdimq1_u05(x, y); }
static SLEEF_CONST Sleef_quad floorq(Sleef_quad x) { return Sleef_floorq1(x); }
static SLEEF_CONST Sleef_quad fmaq(Sleef_quad x, Sleef_quad y, Sleef_quad z) { return Sleef_fmaq1_u05(x, y, z); }
static SLEEF_CONST Sleef_quad fmaxq(Sleef_quad x, Sleef_quad y) { return Sleef_fmaxq1(x, y); }
static SLEEF_CONST Sleef_quad fminq(Sleef_quad x, Sleef_quad y) { return Sleef_fminq1(x, y); }
static SLEEF_CONST Sleef_quad fmodq(Sleef_quad x, Sleef_quad y) { return Sleef_fmodq1(x, y); }
static SLEEF_CONST Sleef_quad frexpq(Sleef_quad x, int *ptr) { return Sleef_frexpq1(x, ptr); }
static SLEEF_CONST Sleef_quad hypotq(Sleef_quad x, Sleef_quad y) { return Sleef_hypotq1_u05(x, y); }
static SLEEF_CONST int ilogbq(Sleef_quad x) { return Sleef_ilogbq1(x); }
static SLEEF_CONST Sleef_quad ldexpq(Sleef_quad x, int e) { return Sleef_ldexpq1(x, e); }
static SLEEF_CONST Sleef_quad logq(Sleef_quad x) { return Sleef_logq1_u10(x); }
static SLEEF_CONST Sleef_quad log10q(Sleef_quad x) { return Sleef_log10q1_u10(x); }
static SLEEF_CONST Sleef_quad log2q(Sleef_quad x) { return Sleef_log2q1_u10(x); }
static SLEEF_CONST Sleef_quad log1pq(Sleef_quad x) { return Sleef_log1pq1_u10(x); }
static SLEEF_CONST Sleef_quad modfq(Sleef_quad x, Sleef_quad *ptr) { return Sleef_modfq1(x, ptr); }
static SLEEF_CONST Sleef_quad powq(Sleef_quad x, Sleef_quad y) { return Sleef_powq1_u10(x, y); }
static SLEEF_CONST Sleef_quad remainderq(Sleef_quad x, Sleef_quad y) { return Sleef_remainderq1(x, y); }
static SLEEF_CONST Sleef_quad rintq(Sleef_quad x) { return Sleef_rintq1(x); }
static SLEEF_CONST Sleef_quad roundq(Sleef_quad x) { return Sleef_roundq1(x); }
static SLEEF_CONST Sleef_quad sinhq(Sleef_quad x) { return Sleef_sinhq1_u10(x); }
static SLEEF_CONST Sleef_quad sinq(Sleef_quad x) { return Sleef_sinq1_u10(x); }
static SLEEF_CONST Sleef_quad sqrtq(Sleef_quad x) { return Sleef_sqrtq1_u05(x); }
static SLEEF_CONST Sleef_quad tanq(Sleef_quad x) { return Sleef_tanq1_u10(x); }
static SLEEF_CONST Sleef_quad tanhq(Sleef_quad x) { return Sleef_tanhq1_u10(x); }
static SLEEF_CONST Sleef_quad truncq(Sleef_quad x) { return Sleef_truncq1(x); }

#ifdef __cplusplus
} // namespace sleef
#endif
#endif // #if defined(SLEEF_REPLACE_LIBQUADMATH_FUNCS) || defined(__cplusplus)

#endif // #ifndef __SLEEFQUAD_H__
