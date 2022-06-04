/* Copyright 1990-2020, Jsoftware Inc.  All rights reserved. */
/* License in license.txt.                                   */
/*                                                           */
/* avx2 emulation                                            */

#ifndef __EMU_M256_AVX2IMMINTRIN_EMU_H__
#define __EMU_M256_AVX2IMMINTRIN_EMU_H__

#ifdef __SSE__
#include <xmmintrin.h>
#endif

#ifdef __SSE2__
#include <emmintrin.h>
#endif

#ifdef __SSE3__
#include <pmmintrin.h>
#endif

#ifdef __SSSE3__
#include <tmmintrin.h>
#endif

#if defined (__SSE4_2__) || defined (__SSE4_1__)
#include <smmintrin.h>
#endif

#if defined (__AES__) || defined (__PCLMUL__)
#include <wmmintrin.h>
#endif

#ifdef __AVX__
#include <immintrin.h>
#endif

/*
 * Intel(R) AVX2 compiler intrinsics.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This is an emulation of Intel AVX2
 */

#if defined( _MSC_VER ) || defined( __INTEL_COMPILER )
    #define __EMU_M256_ALIGN( a ) __declspec(align(a))
    #define __emu_inline          __forceinline
    #define __emu_int64_t         __int64
    #define __emu_uint64_t        unsigned __int64
#elif defined( __GNUC__ )
    #define __EMU_M256_ALIGN( a ) __attribute__((__aligned__(a)))
    #define __emu_inline          __inline __attribute__((__always_inline__))
    #define __emu_int64_t         long long
    #define __emu_uint64_t        unsigned long long
#else
    #error "unsupported platform"
#endif

typedef __m256 __emu__m256;
typedef __m256d __emu__m256d;
typedef __m256i __emu__m256i;

typedef union __EMU_M256_ALIGN(32) __emv__m256
{
    float       __emu_arr[8];
    __m128      __emu_m128[2];
    __m256      emu__m256;
} __emv__m256;

typedef union __EMU_M256_ALIGN(32) __emv__m256d
{
    double      __emu_arr[4];
    __m128d     __emu_m128[2];
    __m256d     emu__m256d;
} __emv__m256d;

typedef union __EMU_M256_ALIGN(32) __emv__m256i
{
    int         __emu_arr[8];
    __m128i     __emu_m128[2];
    __m256i     emu__m256i;
} __emv__m256i;

#define __EMU_M256_IMPL_M2( type, func ) \
static __emu_inline __emu##type __emu_mm256_##func( __emu##type m256_param1, __emu##type m256_param2 ) \
{   __emv##type res; \
    __emv##type m256v_param1; \
    __emv##type m256v_param2; \
    m256v_param1.emu##type = m256_param1; \
    m256v_param2.emu##type = m256_param2; \
    res.__emu_m128[0] = _mm_##func( m256v_param1.__emu_m128[0], m256v_param2.__emu_m128[0] ); \
    res.__emu_m128[1] = _mm_##func( m256v_param1.__emu_m128[1], m256v_param2.__emu_m128[1] ); \
    return ( res.emu##type ); \
}

#define __EMU_M256_128_IMPL_M2( type, func ) \
static __emu_inline __emu##type __emu_mm256_##func##256( __emu##type m256_param1, __emu##type m256_param2 ) \
{   __emv##type res; \
    __emv##type m256v_param1; \
    __emv##type m256v_param2; \
    m256v_param1.emu##type = m256_param1; \
    m256v_param2.emu##type = m256_param2; \
    res.__emu_m128[0] = _mm_##func##128( m256v_param1.__emu_m128[0], m256v_param2.__emu_m128[0] ); \
    res.__emu_m128[1] = _mm_##func##128( m256v_param1.__emu_m128[1], m256v_param2.__emu_m128[1] ); \
    return ( res.emu##type ); \
}

/** \{Reference,_mm256_blend_epi32} */
static __emu_inline __emu__m256i __emu_mm256_blend_epi32_REF        ( __emu__m256i a, __emu__m256i b, const int mask )
{
    __emv__m256i A,B,ret;
    A.emu__m256i = a;
    B.emu__m256i = b;
    ret.__emu_arr[0] = (mask & 0x1) ? B.__emu_arr[0] : A.__emu_arr[0];
    ret.__emu_arr[1] = (mask & 0x2) ? B.__emu_arr[1] : A.__emu_arr[1];
    ret.__emu_arr[2] = (mask & 0x4) ? B.__emu_arr[2] : A.__emu_arr[2];
    ret.__emu_arr[3] = (mask & 0x8) ? B.__emu_arr[3] : A.__emu_arr[3];
    ret.__emu_arr[4] = (mask & 0x10) ? B.__emu_arr[4] : A.__emu_arr[4];
    ret.__emu_arr[5] = (mask & 0x20) ? B.__emu_arr[5] : A.__emu_arr[5];
    ret.__emu_arr[6] = (mask & 0x40) ? B.__emu_arr[6] : A.__emu_arr[6];
    ret.__emu_arr[7] = (mask & 0x80) ? B.__emu_arr[7] : A.__emu_arr[7];
    return ret.emu__m256i;
}


static __emu_inline __emu__m256i __emu_mm256_broadcastb_epi8        ( __m128i a )
{
    __emv__m256i A;
    __m128i av = a;
    memset(&A, *(int8_t*)&av, 32);
    return A.emu__m256i;
}

static __emu_inline __emu__m256i __emu_mm256_broadcastd_epi32       ( __m128i a )
{
    __emv__m256i A;
    int *m = (int*) &a;
    int *p = (int*) &A;
    for (int i=0; i<8; i++) p[i] = m[0];
    return A.emu__m256i;
}

static __emu_inline __emu__m256i __emu_mm256_broadcastq_epi64       ( __m128i a )
{
    __emv__m256i A;
    __emu_int64_t *m = (__emu_int64_t*) &a;
    __emu_int64_t *p = (__emu_int64_t*) &A;
    for (int i=0; i<4; i++) p[i] = m[0];
    return A.emu__m256i;
}

static __emu_inline __emu__m256i __emu_mm256_broadcastsi128_si256       ( __m128i a )
{
    __emv__m256i A;
    A.__emu_m128[0] = a;
    A.__emu_m128[1] = a;
    return A.emu__m256i;
}

static __emu_inline __emu__m256i __emu_mm256_slli_epi64 ( __emu__m256i a, int imm )
{
    __emv__m256i A;
    __emv__m256i av;
    av.emu__m256i = a;
    A.__emu_m128[0] = _mm_slli_epi64( av.__emu_m128[0], imm);
    A.__emu_m128[1] = _mm_slli_epi64( av.__emu_m128[1], imm);
    return A.emu__m256i;
}

static __emu_inline __emu__m256i __emu_mm256_sllv_epi64(__emu__m256i a, __emu__m256i count)
{
    __emv__m256i A;
    __emu_uint64_t *c = (__emu_uint64_t*) &count;
    __emu_int64_t *m = (__emu_int64_t*) &a;
    __emu_int64_t *p = (__emu_int64_t*) &A;
    for (int i=0; i<4; i++) p[i] = (c[i]<64) ? (m[i]<<c[i]) : 0;
    return A.emu__m256i;
}

static __emu_inline __emu__m256i __emu_mm256_srli_epi16 ( __emu__m256i a, int imm )
{
    __emv__m256i A;
    __emv__m256i av;
    av.emu__m256i = a;
    A.__emu_m128[0] = _mm_srli_epi16( av.__emu_m128[0], imm);
    A.__emu_m128[1] = _mm_srli_epi16( av.__emu_m128[1], imm);
    return A.emu__m256i;
}

static __emu_inline __emu__m256i __emu_mm256_srli_epi32 ( __emu__m256i a, int imm )
{
    __emv__m256i A;
    __emv__m256i av;
    av.emu__m256i = a;
    A.__emu_m128[0] = _mm_srli_epi32( av.__emu_m128[0], imm);
    A.__emu_m128[1] = _mm_srli_epi32( av.__emu_m128[1], imm);
    return A.emu__m256i;
}

static __emu_inline __emu__m256i __emu_mm256_srli_epi64 ( __emu__m256i a, int imm )
{
    __emv__m256i A;
    __emv__m256i av;
    av.emu__m256i = a;
    A.__emu_m128[0] = _mm_srli_epi64( av.__emu_m128[0], imm);
    A.__emu_m128[1] = _mm_srli_epi64( av.__emu_m128[1], imm);
    return A.emu__m256i;
}

#if defined(__clang__)
#define __emu_mm256_srli_si256( a, imm ) \
({                                       \
    __emv__m256i A;                      \
    __emv__m256i av;                     \
    av.emu__m256i = a;                   \
    A.__emu_m128[0] = _mm_srli_si128( av.__emu_m128[0], imm); \
    A.__emu_m128[1] = _mm_srli_si128( av.__emu_m128[1], imm); \
    A.emu__m256i; \
})
#else
static __emu_inline __emu__m256i __emu_mm256_srli_si256 ( __emu__m256i a, int imm )
{
    __emv__m256i A;
    __emv__m256i av;
    av.emu__m256i = a;
    A.__emu_m128[0] = _mm_srli_si128( av.__emu_m128[0], imm);
    A.__emu_m128[1] = _mm_srli_si128( av.__emu_m128[1], imm);
    return A.emu__m256i;
}
#endif

__EMU_M256_IMPL_M2( __m256i, cmpgt_epi16 );
__EMU_M256_IMPL_M2( __m256i, cmpgt_epi32 );
__EMU_M256_IMPL_M2( __m256i, cmpgt_epi64 );
__EMU_M256_IMPL_M2( __m256i, cmpgt_epi8 );
__EMU_M256_IMPL_M2( __m256i, add_epi16 );
__EMU_M256_IMPL_M2( __m256i, add_epi32 );
__EMU_M256_IMPL_M2( __m256i, add_epi64 );
__EMU_M256_IMPL_M2( __m256i, sub_epi64 );

__EMU_M256_IMPL_M2( __m256d, add_pd );
__EMU_M256_IMPL_M2( __m256, add_ps );

__EMU_M256_IMPL_M2( __m256i, cmpeq_epi16 );
__EMU_M256_IMPL_M2( __m256i, cmpeq_epi32 );
__EMU_M256_IMPL_M2( __m256i, cmpeq_epi64 );
__EMU_M256_IMPL_M2( __m256i, cmpeq_epi8 );
__EMU_M256_IMPL_M2( __m256i, mul_epu32 );
__EMU_M256_IMPL_M2( __m256i, add_epi8 );
__EMU_M256_IMPL_M2( __m256i, sub_epi8 );
__EMU_M256_128_IMPL_M2( __m256i, and_si  );
__EMU_M256_128_IMPL_M2( __m256i, xor_si  );
__EMU_M256_128_IMPL_M2( __m256i, or_si  );
__EMU_M256_128_IMPL_M2( __m256i, andnot_si  );


static __emu_inline int __emu_mm256_movemask_epi8(__emu__m256i a)
{
 __emv__m256i av;
 av.emu__m256i = a;
    return
        (_mm_movemask_epi8( av.__emu_m128[1] ) << 16) |
         _mm_movemask_epi8( av.__emu_m128[0] );
}

static __emu_inline __emu__m256i __emu_mm256_permute2x128_si256 (__emu__m256i a, __emu__m256i b, const int imm8) {
 __emv__m256i A;
 __emv__m256i av;
 __emv__m256i bv;
 av.emu__m256i = a;
 bv.emu__m256i = b;
	switch (imm8 & 0b11) {
	case 0:	A.__emu_m128[0]  = av.__emu_m128[0] ; break ;
	case 1:	A.__emu_m128[0]  = av.__emu_m128[1] ; break ;
	case 2:	A.__emu_m128[0]  = bv.__emu_m128[0] ; break ;
	case 3:	A.__emu_m128[0]  = bv.__emu_m128[1] ; break ;
	}
	if (imm8 & 0b1000)
  A.__emu_m128[0] = _mm_setzero_si128();

	switch ((imm8 >> 4) & 0b11) {
	case 0:	A.__emu_m128[1]  = av.__emu_m128[0] ; break ;
	case 1:	A.__emu_m128[1]  = av.__emu_m128[1] ; break ;
	case 2:	A.__emu_m128[1]  = bv.__emu_m128[0] ; break ;
	case 3:	A.__emu_m128[1]  = bv.__emu_m128[1] ; break ;
	}
	if (imm8 & 0b10000000)
  A.__emu_m128[1] = _mm_setzero_si128();

 return A.emu__m256i;
}

#if defined(__clang__) || defined( __GNUC__ )
#define __emu_mm256_permute4x64_epi64( a, control) \
({                                                              \
   _mm256_castpd_si256(_mm256_permute4x64_pd(_mm256_castsi256_pd(a),control)); \
})
#else
static __emu_inline __emu__m256i __emu_mm256_permute4x64_epi64(__emu__m256i a, const int control)
{
   return _mm256_castpd_si256(_mm256_permute4x64_pd(_mm256_castsi256_pd(a),control));
}
#endif

static __emu_inline __emu__m256d __emu_mm256_permute4x64_pd(__emu__m256d a, const int control)
{
    __emv__m256d A;
    __emv__m256d av;
    av.emu__m256d = a;
    const int id0 = control & 0x03;
    const int id1 = (control >> 2) & 0x03;
    const int id2 = (control >> 4) & 0x03;
    const int id3 = (control >> 6) & 0x03;
    (A.__emu_m128[0])[0] = av.__emu_arr[id0];
    (A.__emu_m128[0])[1] = av.__emu_arr[id1];
    (A.__emu_m128[1])[0] = av.__emu_arr[id2];
    (A.__emu_m128[1])[1] = av.__emu_arr[id3];
    return A.emu__m256d;
}

static __emu_inline __emu__m256d __emu_mm256_fnmadd_pd(__emu__m256d a, __emu__m256d b, __emu__m256d c)
{
    return _mm256_sub_pd(c,_mm256_mul_pd(a,b));
}

static __emu_inline __emu__m256d __emu_mm256_fmadd_pd(__emu__m256d a, __emu__m256d b, __emu__m256d c)
{
    return _mm256_add_pd(_mm256_mul_pd(a,b),c);
}

static __emu_inline __emu__m256 __emu_mm256_fmadd_ps(__emu__m256 a, __emu__m256 b, __emu__m256 c)
{
    return _mm256_add_ps(_mm256_mul_ps(a,b),c);
}

static __emu_inline __emu__m256d __emu_mm256_fmsub_pd(__emu__m256d a, __emu__m256d b, __emu__m256d c)
{
    return _mm256_sub_pd(_mm256_mul_pd(a,b),c);
}

static __emu_inline __emu__m256 __emu_mm256_fmsub_ps(__emu__m256 a, __emu__m256 b, __emu__m256 c)
{
    return _mm256_sub_ps(_mm256_mul_ps(a,b),c);
}

static __emu_inline __emu__m256i __emu_mm256_i64gather_epi64(__emu_int64_t const *addr, __emu__m256i index, const int scale) {
 __emv__m256i A;
 __emv__m256i B;
 A.emu__m256i = index;
 __emu_int64_t *p = (__emu_int64_t*) &B;
 p[0] = *(__emu_int64_t*)((int8_t*)addr + (A.__emu_m128[ 0 ])[0] * scale);
 p[1] = *(__emu_int64_t*)((int8_t*)addr + (A.__emu_m128[ 0 ])[1] * scale);
 p[2] = *(__emu_int64_t*)((int8_t*)addr + (A.__emu_m128[ 1 ])[0] * scale);
 p[3] = *(__emu_int64_t*)((int8_t*)addr + (A.__emu_m128[ 1 ])[1] * scale);
 return B.emu__m256i;
}

static __emu_inline __emu__m256i __emu_mm256_mask_i64gather_epi64( __emu__m256i src, __emu_int64_t const *addr, __emu__m256i index, __emu__m256i mask, const int scale) {
 __emv__m256i A;
 __emv__m256i B;
 __emv__m256i C;
 __emv__m256i D;
 A.emu__m256i = index;
 C.emu__m256i = src;
 D.emu__m256i = mask;
 __emu_int64_t *p = (__emu_int64_t*) &B;
 __emu_int64_t *m = (__emu_int64_t*) &D;
 p[0] = m[0] ? *(__emu_int64_t*)((int8_t*)addr + (A.__emu_m128[ 0 ])[0] * scale) : (C.__emu_m128[0])[0];
 p[1] = m[1] ? *(__emu_int64_t*)((int8_t*)addr + (A.__emu_m128[ 0 ])[1] * scale) : (C.__emu_m128[0])[1];
 p[2] = m[2] ? *(__emu_int64_t*)((int8_t*)addr + (A.__emu_m128[ 1 ])[0] * scale) : (C.__emu_m128[1])[0];
 p[3] = m[3] ? *(__emu_int64_t*)((int8_t*)addr + (A.__emu_m128[ 1 ])[1] * scale) : (C.__emu_m128[1])[1];
 return B.emu__m256i;
}

static __emu_inline __emu__m256d __emu_mm256_i64gather_pd(double const *addr, __emu__m256i index, const int scale) {
 __emv__m256i A;
 __emv__m256d B;
 A.emu__m256i = index;
 double *p = (double*) &B;
 p[0] = *(double*)((int8_t*)addr + (A.__emu_m128[ 0 ])[0] * scale);
 p[1] = *(double*)((int8_t*)addr + (A.__emu_m128[ 0 ])[1] * scale);
 p[2] = *(double*)((int8_t*)addr + (A.__emu_m128[ 1 ])[0] * scale);
 p[3] = *(double*)((int8_t*)addr + (A.__emu_m128[ 1 ])[1] * scale);
 return B.emu__m256d;
}

static __emu_inline int8_t mm_extract_epi8_var_indx(__m128i vec, int i )
{
    __m128i indx = _mm_cvtsi32_si128(i);
    __m128i val  = _mm_shuffle_epi8(vec, indx);
            return (uint8_t)_mm_cvtsi128_si32(val);
}

#define __emu_maskload_impl( name, vec_type, mask_vec_type, type, mask_type ) \
static __emu_inline vec_type  name(type const *a, mask_vec_type mask) {   \
    const size_t size_type = sizeof( type );                          \
    const size_t size = sizeof( vec_type ) / size_type;               \
    __EMU_M256_ALIGN(32) type res[ size ];                            \
    const mask_type* p_mask = (const mask_type*)&mask;                \
    size_t i = 0;                                                     \
    mask_type sign_bit = 1;                                           \
    sign_bit <<= (8*size_type - 1);                                   \
    for ( ; i < size; ++i )                                           \
        res[ i ] = (sign_bit & p_mask[i]) ? a[i] : 0;           \
    return (*(vec_type*)&res);                                        \
}

#define __emu_maskstore_impl( name, vec_type, mask_vec_type, type, mask_type ) \
static __emu_inline void  name(type *a, mask_vec_type mask, vec_type data) { \
    const size_t size_type = sizeof( type );                          \
    const size_t size = sizeof( vec_type ) / sizeof( type );          \
    type* p_data = (type*)&data;                                      \
    const mask_type* p_mask = (const mask_type*)&mask;                \
    size_t i = 0;                                                     \
    mask_type sign_bit = 1;                                           \
    sign_bit <<= (8*size_type - 1);                                   \
    for ( ; i < size; ++i )                                           \
        if ( p_mask[i] & sign_bit)                               \
            a[i] = p_data[i];                                 \
}

// __emu_maskload_impl( __emu_mm256_maskload_epi64, __emu__m256i, __emu__m256i, __emu_int64_t, __emu_int64_t );
// __emu_maskstore_impl( __emu_mm256_maskstore_epi64, __emu__m256i, __emu__m256i, __emu_int64_t, __emu_int64_t );

static __emu_inline __emu__m256i __emu_mm256_maskload_epi64(__emu_int64_t const *b , __emu__m256i m)
{
 return _mm256_castpd_si256(_mm256_maskload_pd((double const *)b ,  m));
}


static void __emu_mm256_maskstore_epi64(__emu_int64_t *b , __emu__m256i m, __emu__m256i a)
{
 _mm256_maskstore_pd((double*)b, m, _mm256_castsi256_pd(a));
}

static __emu_inline __emu__m256i __emu_mm256_cvtepu8_epi64(__m128i a)
{
 __emv__m256i res;
 res.__emu_m128[0] = _mm_cvtepu8_epi64(a);
 res.__emu_m128[1] = _mm_cvtepu8_epi64(_mm_srli_si128(a,2));
 return res.emu__m256i;
}

#if defined __cplusplus
}; /* End "C" */
#endif /* __cplusplus */

#ifndef __EMU_M256AVX2_NOMAP

#undef _mm256_add_epi16
#undef _mm256_add_epi32
#undef _mm256_add_epi64
#undef _mm256_add_epi8
#undef _mm256_andnot_si256
#undef _mm256_and_si256
#undef _mm256_blend_epi32
#undef _mm256_broadcastb_epi8
#undef _mm256_broadcastd_epi32
#undef _mm256_broadcastq_epi64
#undef _mm256_broadcastsi128_si256
#undef _mm256_cmpeq_epi16
#undef _mm256_cmpeq_epi32
#undef _mm256_cmpeq_epi64
#undef _mm256_cmpeq_epi8
#undef _mm256_cmpgt_epi16
#undef _mm256_cmpgt_epi32
#undef _mm256_cmpgt_epi64
#undef _mm256_cmpgt_epi8
#undef _mm256_cvtepu8_epi64
#undef _mm256_fnmadd_pd
#undef _mm256_fmadd_pd
#undef _mm256_fmadd_ps
#undef _mm256_fmsub_pd
#undef _mm256_fmsub_ps
#undef _mm256_i64gather_epi64
#undef _mm256_i64gather_pd
#undef _mm256_mask_i64gather_epi64
#undef _mm256_maskload_epi64
#undef _mm256_maskstore_epi64
#undef _mm256_movemask_epi8
#undef _mm256_mul_epu32
#undef _mm256_or_si256
#undef _mm256_permute2x128_si256
#undef _mm256_permute4x64_epi64
#undef _mm256_permute4x64_pd
#undef _mm256_slli_epi64
#undef _mm256_sllv_epi64
#undef _mm256_srli_epi16
#undef _mm256_srli_epi32
#undef _mm256_srli_epi64
#undef _mm256_srli_si256
#undef _mm256_sub_epi64
#undef _mm256_sub_epi8
#undef _mm256_xor_si256

#define _mm256_add_epi16 __emu_mm256_add_epi16
#define _mm256_add_epi32 __emu_mm256_add_epi32
#define _mm256_add_epi64 __emu_mm256_add_epi64
#define _mm256_add_epi8 __emu_mm256_add_epi8
#define _mm256_andnot_si256 __emu_mm256_andnot_si256
#define _mm256_and_si256 __emu_mm256_and_si256
#define _mm256_blend_epi32 __emu_mm256_blend_epi32_REF
#define _mm256_broadcastb_epi8 __emu_mm256_broadcastb_epi8
#define _mm256_broadcastd_epi32 __emu_mm256_broadcastd_epi32
#define _mm256_broadcastq_epi64 __emu_mm256_broadcastq_epi64
#define _mm256_broadcastsi128_si256 __emu_mm256_broadcastsi128_si256
#define _mm256_cmpeq_epi16 __emu_mm256_cmpeq_epi16
#define _mm256_cmpeq_epi32 __emu_mm256_cmpeq_epi32
#define _mm256_cmpeq_epi64 __emu_mm256_cmpeq_epi64
#define _mm256_cmpeq_epi8 __emu_mm256_cmpeq_epi8
#define _mm256_cmpgt_epi16 __emu_mm256_cmpgt_epi16
#define _mm256_cmpgt_epi32 __emu_mm256_cmpgt_epi32
#define _mm256_cmpgt_epi64 __emu_mm256_cmpgt_epi64
#define _mm256_cmpgt_epi8 __emu_mm256_cmpgt_epi8
#define _mm256_cvtepu8_epi64 __emu_mm256_cvtepu8_epi64
#define _mm256_fnmadd_pd __emu_mm256_fnmadd_pd
#define _mm256_fmadd_pd __emu_mm256_fmadd_pd
#define _mm256_fmadd_ps __emu_mm256_fmadd_ps
#define _mm256_fmsub_pd __emu_mm256_fmsub_pd
#define _mm256_fmsub_ps __emu_mm256_fmsub_ps
#define _mm256_i64gather_epi64 __emu_mm256_i64gather_epi64
#define _mm256_i64gather_pd __emu_mm256_i64gather_pd
#define _mm256_mask_i64gather_epi64 __emu_mm256_mask_i64gather_epi64
#define _mm256_maskload_epi64 __emu_mm256_maskload_epi64
#define _mm256_maskstore_epi64 __emu_mm256_maskstore_epi64
#define _mm256_movemask_epi8 __emu_mm256_movemask_epi8
#define _mm256_mul_epu32 __emu_mm256_mul_epu32
#define _mm256_or_si256 __emu_mm256_or_si256
#define _mm256_permute2x128_si256 __emu_mm256_permute2x128_si256
#define _mm256_permute4x64_epi64 __emu_mm256_permute4x64_epi64
#define _mm256_permute4x64_pd __emu_mm256_permute4x64_pd
#define _mm256_slli_epi64 __emu_mm256_slli_epi64
#define _mm256_sllv_epi64 __emu_mm256_sllv_epi64
#define _mm256_srli_epi16 __emu_mm256_srli_epi16
#define _mm256_srli_epi32 __emu_mm256_srli_epi32
#define _mm256_srli_epi64 __emu_mm256_srli_epi64
#define _mm256_srli_si256 __emu_mm256_srli_si256
#define _mm256_sub_epi64 __emu_mm256_sub_epi64
#define _mm256_sub_epi8 __emu_mm256_sub_epi8
#define _mm256_xor_si256 __emu_mm256_xor_si256

#endif /* __EMU_M256AVX2_NOMAP */

#endif /* __EMU_M256_AVX2IMMINTRIN_EMU_H__ */
