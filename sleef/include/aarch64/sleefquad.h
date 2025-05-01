//   Copyright Naoki Shibata and contributors 2010 - 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __SLEEFQUAD_H__
#define __SLEEFQUAD_H__

#define SLEEF_VERSION_MAJOR 4
#define SLEEF_VERSION_MINOR 0
#define SLEEF_VERSION_PATCHLEVEL 0

/* #undef SLEEF_FLOAT128_IS_IEEEQP */
/* #undef SLEEF_LONGDOUBLE_IS_IEEEQP */

#include "sleef.h"
#include <string.h>

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

#if defined (__GNUC__) || defined (__clang__)
#define SLEEF_CONST __attribute__((const))
#define SLEEF_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define SLEEF_CONST
#define SLEEF_INLINE __forceinline
#endif

//

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

#if !defined(Sleef_rvvm1quad_DEFINED)
#if defined(__riscv) && defined(__riscv_v)
#define Sleef_rvvm1quadDEFINED
typedef vuint64m1x2_t Sleef_rvvm1quad;
#endif
#endif

#if !defined(Sleef_rvvm2quad_DEFINED)
#if defined(__riscv) && defined(__riscv_v)
#define Sleef_rvvm2quadDEFINED
typedef vuint64m2x2_t Sleef_rvvm2quad;
#endif
#endif

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

#ifdef __ARM_NEON
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_addq2_u05advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_subq2_u05advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_mulq2_u05advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_divq2_u05advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_negq2_advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_sqrtq2_u05advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cbrtq2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_icmpltq2_advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_icmpleq2_advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_icmpgtq2_advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_icmpgeq2_advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_icmpeqq2_advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_icmpneq2_advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_icmpq2_advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_iunordq2_advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_iselectq2_advsimd(int32x2_t, Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST float64x2_t Sleef_cast_to_doubleq2_advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cast_from_doubleq2_advsimd(float64x2_t);
SLEEF_IMPORT SLEEF_CONST int64x2_t Sleef_cast_to_int64q2_advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cast_from_int64q2_advsimd(int64x2_t);
SLEEF_IMPORT SLEEF_CONST uint64x2_t Sleef_cast_to_uint64q2_advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cast_from_uint64q2_advsimd(uint64x2_t);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_loadq2_advsimd(Sleef_quad *);
SLEEF_IMPORT void Sleef_storeq2_advsimd(Sleef_quad *, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_getq2_advsimd(Sleef_quadx2, int);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_setq2_advsimd(Sleef_quadx2, int, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_splatq2_advsimd(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_sinq2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cosq2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_tanq2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_asinq2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_acosq2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_atanq2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_atan2q2_u10advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_expq2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_exp2q2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_exp10q2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_expm1q2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_logq2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_log2q2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_log10q2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_log1pq2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_powq2_u10advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_sinhq2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_coshq2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_tanhq2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_asinhq2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_acoshq2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_atanhq2_u10advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_truncq2_advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_floorq2_advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_ceilq2_advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_roundq2_advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_rintq2_advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fabsq2_advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_copysignq2_advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fmaxq2_advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fminq2_advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fdimq2_u05advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fmodq2_advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_remainderq2_advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT Sleef_quadx2 Sleef_frexpq2_advsimd(Sleef_quadx2, int32x2_t *);
SLEEF_IMPORT Sleef_quadx2 Sleef_modfq2_advsimd(Sleef_quadx2, Sleef_quadx2 *);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_hypotq2_u05advsimd(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_ldexpq2_advsimd(Sleef_quadx2, int32x2_t);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_ilogbq2_advsimd(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_fmaq2_u05advsimd(Sleef_quadx2, Sleef_quadx2, Sleef_quadx2);
#endif
#ifdef __ARM_FEATURE_SVE
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_addqx_u05sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_subqx_u05sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_mulqx_u05sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_divqx_u05sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_negqx_sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_sqrtqx_u05sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_cbrtqx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST svint32_t Sleef_icmpltqx_sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST svint32_t Sleef_icmpleqx_sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST svint32_t Sleef_icmpgtqx_sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST svint32_t Sleef_icmpgeqx_sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST svint32_t Sleef_icmpeqqx_sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST svint32_t Sleef_icmpneqx_sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST svint32_t Sleef_icmpqx_sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST svint32_t Sleef_iunordqx_sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_iselectqx_sve(svint32_t, Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST svfloat64_t Sleef_cast_to_doubleqx_sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_cast_from_doubleqx_sve(svfloat64_t);
SLEEF_IMPORT SLEEF_CONST svint64_t Sleef_cast_to_int64qx_sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_cast_from_int64qx_sve(svint64_t);
SLEEF_IMPORT SLEEF_CONST svuint64_t Sleef_cast_to_uint64qx_sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_cast_from_uint64qx_sve(svuint64_t);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_loadqx_sve(Sleef_quad *);
SLEEF_IMPORT void Sleef_storeqx_sve(Sleef_quad *, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_quad Sleef_getqx_sve(Sleef_svquad, int);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_setqx_sve(Sleef_svquad, int, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_splatqx_sve(Sleef_quad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_sinqx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_cosqx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_tanqx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_asinqx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_acosqx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_atanqx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_atan2qx_u10sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_expqx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_exp2qx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_exp10qx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_expm1qx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_logqx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_log2qx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_log10qx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_log1pqx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_powqx_u10sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_sinhqx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_coshqx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_tanhqx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_asinhqx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_acoshqx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_atanhqx_u10sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_truncqx_sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_floorqx_sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_ceilqx_sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_roundqx_sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_rintqx_sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_fabsqx_sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_copysignqx_sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_fmaxqx_sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_fminqx_sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_fdimqx_u05sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_fmodqx_sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_remainderqx_sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT Sleef_svquad Sleef_frexpqx_sve(Sleef_svquad, svint32_t *);
SLEEF_IMPORT Sleef_svquad Sleef_modfqx_sve(Sleef_svquad, Sleef_svquad *);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_hypotqx_u05sve(Sleef_svquad, Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_ldexpqx_sve(Sleef_svquad, svint32_t);
SLEEF_IMPORT SLEEF_CONST svint32_t Sleef_ilogbqx_sve(Sleef_svquad);
SLEEF_IMPORT SLEEF_CONST Sleef_svquad Sleef_fmaqx_u05sve(Sleef_svquad, Sleef_svquad, Sleef_svquad);
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
SLEEF_IMPORT void Sleef_storeq1_purec(Sleef_quad *, Sleef_quad);
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
SLEEF_IMPORT void Sleef_storeq1_purecfma(Sleef_quad *, Sleef_quad);
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
SLEEF_IMPORT void Sleef_storeq1(Sleef_quad *, Sleef_quad);
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
#ifdef __ARM_NEON
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_addq2_u05(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_subq2_u05(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_mulq2_u05(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_divq2_u05(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_negq2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_sqrtq2_u05(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cbrtq2_u10(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_icmpltq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_icmpleq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_icmpgtq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_icmpgeq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_icmpeqq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_icmpneq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_icmpq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_iunordq2(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_iselectq2(int32x2_t, Sleef_quad, Sleef_quad);
SLEEF_IMPORT SLEEF_CONST float64x2_t Sleef_cast_to_doubleq2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cast_from_doubleq2(float64x2_t);
SLEEF_IMPORT SLEEF_CONST int64x2_t Sleef_cast_to_int64q2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cast_from_int64q2(int64x2_t);
SLEEF_IMPORT SLEEF_CONST uint64x2_t Sleef_cast_to_uint64q2(Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_cast_from_uint64q2(uint64x2_t);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_loadq2(Sleef_quad *);
SLEEF_IMPORT void Sleef_storeq2(Sleef_quad *, Sleef_quadx2);
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
SLEEF_IMPORT Sleef_quadx2 Sleef_frexpq2(Sleef_quadx2, int32x2_t *);
SLEEF_IMPORT Sleef_quadx2 Sleef_modfq2(Sleef_quadx2, Sleef_quadx2 *);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_hypotq2_u05(Sleef_quadx2, Sleef_quadx2);
SLEEF_IMPORT SLEEF_CONST Sleef_quadx2 Sleef_ldexpq2(Sleef_quadx2, int32x2_t);
SLEEF_IMPORT SLEEF_CONST int32x2_t Sleef_ilogbq2(Sleef_quadx2);
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
