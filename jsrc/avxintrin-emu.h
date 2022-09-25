/*
 Copyright (c) 2010, Intel Corporation. All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of Intel Corporation nor the names of its contributors may
   be used to endorse or promote products derived from this software without
   specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
   THE POSSIBILITY OF SUCH DAMAGE.
*/

/***

 Provide feedback to: maxim.locktyukhin intel com, phil.j.kerly intel com

 Version 1.0 - Initial release.

    This AVX intrinsics emulation header file designed to work with Intel C/C++
    as well as GCC compilers.

    Known Issues and limitations:

    - does not support immediate values higher than 0x7 for _mm[256]_cmp_[ps|pd]
      intrinsics, UD2 instruction will be generated instead

    - -O0 optimization level may _sometimes_ result with compile time errors due
      to failed forced inline and compiler not being able to generate instruction
      with constant immediate operand becasue of it, compiling with -O1 and/or
      -finline-functions should help.

***/


#ifndef __EMU_M256_AVXIMMINTRIN_EMU_H__
#define __EMU_M256_AVXIMMINTRIN_EMU_H__

// for memset
#include <string.h>

#ifdef __GNUC__

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

#else

#include <wmmintrin.h>

#endif

#if !defined (__SSE3__)
#define _mm_lddqu_si128 _mm_loadu_si128

#define _mm_addsub_pd _mm_addsub_pd_SSE2
#define _mm_addsub_ps _mm_addsub_ps_SSE2
#define _mm_hadd_pd _mm_hadd_pd_SSE2
#define _mm_hadd_ps _mm_hadd_ps_SSE2
#define _mm_hsub_pd _mm_hsub_pd_SSE2
#define _mm_hsub_ps _mm_hsub_ps_SSE2
#define _mm_maddubs_epi16 _mm_maddubs_epi16_SSE2
#define _mm_movedup_pd _mm_movedup_pd_SSE2
#define _mm_movehdup_ps _mm_movehdup_ps_SSE2
#define _mm_moveldup_ps _mm_moveldup_ps_SSE2
#endif

#if !defined (__SSSE3__)
#define _mm_maddubs_epi16 _mm_maddubs_epi16_SSE2
#define _mm_shuffle_epi8 _mm_shuffle_epi8_REF
#endif

#if !(defined (__SSE4_2__) || defined (__SSE4_1__))
#define _mm_blend_pd _mm_blend_pd_REF
#define _mm_blend_ps _mm_blend_ps_SSE2
#define _mm_blendv_pd _mm_blendv_pd_SSE2
#define _mm_blendv_ps _mm_blendv_ps_SSE2
#define _mm_cmpeq_epi64 _mm_cmpeq_epi64_REF
#define _mm_cmpgt_epi64 _mm_cmpgt_epi64_REF
#define _mm_cvtepu8_epi64 _mm_cvtepu8_epi64_SSE2
#define _mm_extract_epi64 _mm_extract_epi64_REF
#define _mm_insert_epi32 _mm_insert_epi32_REF
#define _mm_insert_epi64 _mm_insert_epi64_REF
#define _mm_insert_epi8 _mm_insert_epi8_REF
#define _mm_testc_si128 _mm_testc_si128_REF
#define _mm_testnzc_si128 _mm_testnzc_si128_REF
#define _mm_testz_si128 _mm_testz_si128_REF
#endif

// #pragma message ("Warning: AVX intrinsics are emulated with SSE")

/*
 * Intel(R) AVX compiler intrinsics.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This is an emulation of Intel AVX
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

typedef union __EMU_M256_ALIGN(32) __emu__m256
{
    float       __emu_arr[8];
    __m128      __emu_m128[2];
} __emu__m256;

typedef union __EMU_M256_ALIGN(32) __emu__m256d
{
    double      __emu_arr[4];
    __m128d     __emu_m128[2];
} __emu__m256d;

typedef union __EMU_M256_ALIGN(32) __emu__m256i
{
    int         __emu_arr[8];
    __m128i     __emu_m128[2];
} __emu__m256i;

#define __m256_get_ps(vec, index) vec.__emu_m128[index >> 2][index & 3]

static __emu_inline __emu__m256  __emu_set_m128( const __m128 arr[] ) { __emu__m256 ret;  ret.__emu_m128[0] = arr[0]; ret.__emu_m128[1] = arr[1]; return (ret); }
static __emu_inline __emu__m256d __emu_set_m128d( const __m128d arr[] ) { __emu__m256d ret; ret.__emu_m128[0] = arr[0]; ret.__emu_m128[1] = arr[1]; return (ret); }
static __emu_inline __emu__m256i __emu_set_m128i( const __m128i arr[] ) { __emu__m256i ret; ret.__emu_m128[0] = arr[0]; ret.__emu_m128[1] = arr[1]; return (ret); }


#define __EMU_M256_IMPL_M1( type, func ) \
static __emu_inline __emu##type __emu_mm256_##func( __emu##type m256_param1 ) \
{   __emu##type res; \
    res.__emu_m128[0] = _mm_##func( m256_param1.__emu_m128[0] ); \
    res.__emu_m128[1] = _mm_##func( m256_param1.__emu_m128[1] ); \
    return ( res ); \
}

#define __EMU_M256_IMPL_M1_RET( ret_type, type, func ) \
static __emu_inline __emu##ret_type __emu_mm256_##func( __emu##type m256_param1 ) \
{   __emu##ret_type res; \
    res.__emu_m128[0] = _mm_##func( m256_param1.__emu_m128[0] ); \
    res.__emu_m128[1] = _mm_##func( m256_param1.__emu_m128[1] ); \
    return ( res ); \
}

#define __EMU_M256_IMPL_M1_RET_NAME( ret_type, type, func, name ) \
    static __emu_inline __emu##ret_type __emu_mm256_##name( __emu##type m256_param1 ) \
{   __emu##ret_type res; \
    res.__emu_m128[0] = _mm_##func( m256_param1.__emu_m128[0] ); \
    res.__emu_m128[1] = _mm_##func( m256_param1.__emu_m128[1] ); \
    return ( res ); \
}

#define __EMU_M256_IMPL_M1_LH( type, type_128, func ) \
static __emu_inline __emu##type __emu_mm256_##func( type_128 m128_param ) \
{   __emu##type res; \
    res.__emu_m128[0] = _mm_##func( m128_param ); \
    __m128 m128_param_high = _mm_movehl_ps( *(__m128*)&m128_param, *(__m128*)&m128_param ); \
    res.__emu_m128[1] = _mm_##func( *(type_128*)&m128_param_high ); \
    return ( res ); \
}

#define __EMU_M256_IMPL_M1_HL( type_128, type, func ) \
static __emu_inline type_128 __emu_mm256_##func( __emu##type m256_param1 ) \
{   type_128 res, tmp; \
    res = _mm_##func( m256_param1.__emu_m128[0] ); \
    tmp = _mm_##func( m256_param1.__emu_m128[1] ); \
    *(((__emu_int64_t*)&res)+1) = *(__emu_int64_t*)&tmp; \
    return ( res ); \
}

#define __EMU_M256_IMPL_M1P_DUP( type, type_param, func ) \
static __emu_inline __emu##type __emu_mm256_##func( type_param param ) \
{   __emu##type res; \
    res.__emu_m128[0] = _mm_##func( param ); \
    res.__emu_m128[1] = _mm_##func( param ); \
    return ( res ); \
}

#define __EMU_M256_IMPL_M1I_DUP( type, func ) \
    static __emu_inline __emu##type __emu_mm256_##func( __emu##type m256_param1, const int param2 ) \
{   __emu##type res; \
    res.__emu_m128[0] = _mm_##func( m256_param1.__emu_m128[0], param2 ); \
    res.__emu_m128[1] = _mm_##func( m256_param1.__emu_m128[1], param2 ); \
    return ( res ); \
}

#define __EMU_M256_IMPL2_M1I_DUP( type, func ) \
static __emu_inline __emu##type __emu_mm256_##func( __emu##type m256_param1, const int param2 ) \
{   __emu##type res; \
    res.__emu_m128[0] = __emu_mm_##func( m256_param1.__emu_m128[0], param2 ); \
    res.__emu_m128[1] = __emu_mm_##func( m256_param1.__emu_m128[1], param2 ); \
    return ( res ); \
}

#define __EMU_M256_IMPL2_M1I_SHIFT( type, func, shift_for_hi ) \
static __emu_inline __emu##type __emu_mm256_##func( __emu##type m256_param1, const int param2 ) \
{   __emu##type res; \
    res.__emu_m128[0] = __emu_mm_##func( m256_param1.__emu_m128[0], param2 & ((1<<shift_for_hi)-1) ); \
    res.__emu_m128[1] = __emu_mm_##func( m256_param1.__emu_m128[1], param2 >> shift_for_hi); \
    return ( res ); \
}

#define __EMU_M256_IMPL_M2( type, func ) \
static __emu_inline __emu##type __emu_mm256_##func( __emu##type m256_param1, __emu##type m256_param2 ) \
{   __emu##type res; \
    res.__emu_m128[0] = _mm_##func( m256_param1.__emu_m128[0], m256_param2.__emu_m128[0] ); \
    res.__emu_m128[1] = _mm_##func( m256_param1.__emu_m128[1], m256_param2.__emu_m128[1] ); \
    return ( res ); \
}

#define __EMU_M256_IMPL2_M2T( type, type_2, func ) \
static __emu_inline __emu##type __emu_mm256_##func( __emu##type m256_param1, __emu##type_2 m256_param2 ) \
{   __emu##type res; \
    res.__emu_m128[0] = __emu_mm_##func( m256_param1.__emu_m128[0], m256_param2.__emu_m128[0] ); \
    res.__emu_m128[1] = __emu_mm_##func( m256_param1.__emu_m128[1], m256_param2.__emu_m128[1] ); \
    return ( res ); \
}

#define __EMU_M256_IMPL_M2I_DUP( type, func ) \
static __emu_inline __emu##type __emu_mm256_##func( __emu##type m256_param1, __emu##type m256_param2, const int param3 ) \
{   __emu##type res; \
    res.__emu_m128[0] = _mm_##func( m256_param1.__emu_m128[0], m256_param2.__emu_m128[0], param3 ); \
    res.__emu_m128[1] = _mm_##func( m256_param1.__emu_m128[1], m256_param2.__emu_m128[1], param3 ); \
    return ( res ); \
}

#define __EMU_M256_IMPL2_M2I_DUP( type, func ) \
static __emu_inline __emu##type __emu_mm256_##func( __emu##type m256_param1, __emu##type m256_param2, const int param3 ) \
{   __emu##type res; \
    res.__emu_m128[0] = __emu_mm_##func( m256_param1.__emu_m128[0], m256_param2.__emu_m128[0], param3 ); \
    res.__emu_m128[1] = __emu_mm_##func( m256_param1.__emu_m128[1], m256_param2.__emu_m128[1], param3 ); \
    return ( res ); \
}

#define __EMU_M256_IMPL_M2I_SHIFT( type, func, shift_for_hi ) \
static __emu_inline  __emu##type __emu_mm256_##func( __emu##type m256_param1, __emu##type m256_param2, const int param3 ) \
{   __emu##type res; \
    res.__emu_m128[0] = _mm_##func( m256_param1.__emu_m128[0], m256_param2.__emu_m128[0], param3 & ((1<<shift_for_hi)-1) ); \
    res.__emu_m128[1] = _mm_##func( m256_param1.__emu_m128[1], m256_param2.__emu_m128[1], param3 >> shift_for_hi ); \
    return ( res ); \
}

#define __EMU_M256_IMPL_M3( type, func ) \
static __emu_inline __emu##type __emu_mm256_##func( __emu##type m256_param1, __emu##type m256_param2, __emu##type m256_param3 ) \
{   __emu##type res; \
    res.__emu_m128[0] = _mm_##func( m256_param1.__emu_m128[0], m256_param2.__emu_m128[0], m256_param3.__emu_m128[0] ); \
    res.__emu_m128[1] = _mm_##func( m256_param1.__emu_m128[1], m256_param2.__emu_m128[1], m256_param3.__emu_m128[1] ); \
    return ( res ); \
}

// sse2

/** */
static __emu_inline void ssp_convert_odd_even_ps_SSE2( __m128 *a, __m128 *b )
{
    // IN
    // a = a3,a2,a1,a0
    // b = b3,b2,b1,b0

    // OUT
    // a = b2,b0,a2,a0  // even
    // b = b3,b1,a3,a1  // odd

    __m128 c, d;
    c = _mm_shuffle_ps( *a, *b, _MM_SHUFFLE(3,1,3,1) );
    d = _mm_shuffle_ps( *a, *b, _MM_SHUFFLE(2,0,2,0) );
    *a = c;
    *b = d;
}

static __emu_inline __m128i ssp_movmask_imm8_to_epi32_SSE2( int mask )
{
    __m128i screen;
    const __m128i mulShiftImm = _mm_set_epi16( 0x1000, 0x0000, 0x2000, 0x0000, 0x4000, 0x0000, 0x8000, 0x0000 ); // Shift mask multiply moves all bits to left, becomes MSB
    screen = _mm_set1_epi16 ( mask                );   // Load the mask into register
    screen = _mm_mullo_epi16( screen, mulShiftImm );   // Shift bits to MSB
    screen = _mm_srai_epi32 ( screen, 31          );   // Shift bits to obtain all F's or all 0's
    return screen;
}


static __emu_inline __m128i ssp_logical_bitwise_select_SSE2( __m128i a, __m128i b, __m128i mask )   // Bitwise (mask ? a : b) 
{
    a = _mm_and_si128   ( a,    mask );                                 // clear a where mask = 0
    b = _mm_andnot_si128( mask, b    );                                 // clear b where mask = 1
    a = _mm_or_si128    ( a,    b    );                                 // a = a OR b                         
    return a; 
}

// avx2  __EMU_M256_128_IMPL_M2
#define __EMU_M256_128_IMPL_M2( type, func ) \
static __emu_inline __emu##type __emu_mm256_##func##256( __emu##type m256_param1, __emu##type m256_param2 ) \
{   __emu##type res; \
    res.__emu_m128[0] = _mm_##func##128( m256_param1.__emu_m128[0], m256_param2.__emu_m128[0] ); \
    res.__emu_m128[1] = _mm_##func##128( m256_param1.__emu_m128[1], m256_param2.__emu_m128[1] ); \
    return ( res ); \
}

/** \SSE4_1{Reference,_mm_blend_pd} */
static __emu_inline __m128d _mm_blend_pd_REF        ( __m128d a, __m128d b, const int mask )
{
    __m128d A, B;
    A = a;
    B = b;
    A[0] = (mask & 0x1) ? B[0] : A[0];
    A[1] = (mask & 0x2) ? B[1] : A[1];
    return A;
}

/** \SSE4_1{SSE2,_mm_blendv_pd} */
static __emu_inline __m128d _mm_blendv_pd_SSE2( __m128d a, __m128d b, __m128d mask )
{
    __m128i A, B, Mask;
    A = _mm_castpd_si128(a);
    B = _mm_castpd_si128(b);
    Mask = _mm_castpd_si128(mask);

    Mask = _mm_shuffle_epi32( Mask, _MM_SHUFFLE(3, 3, 1, 1) );
    Mask = _mm_srai_epi32   ( Mask, 31                      );

    B = _mm_and_si128( B, Mask );
    A = _mm_andnot_si128( Mask, A );
    A = _mm_or_si128( A, B );
    return _mm_castsi128_pd(A);
}

/** \SSE4_1{SSE2,_mm_blend_ps} */
static __emu_inline __m128 _mm_blend_ps_SSE2( __m128 a, __m128 b, const int mask )
{
    __m128i screen, A, B;
    A = _mm_castps_si128(a);
    B = _mm_castps_si128(b);
    screen = ssp_movmask_imm8_to_epi32_SSE2( mask );
    screen = ssp_logical_bitwise_select_SSE2( B, A, screen );
    return _mm_castsi128_ps(screen);
}

/** \SSE4_1{SSE2,_mm_blendv_epi8} */
static __emu_inline __m128 _mm_blendv_ps_SSE2( __m128 a, __m128 b, __m128 mask )
{
    __m128i A, B, Mask;
    A = _mm_castps_si128(a);
    B = _mm_castps_si128(b);
    Mask = _mm_castps_si128(mask);

    Mask = _mm_srai_epi32( Mask, 31 );
    B = _mm_and_si128( B, Mask );
    A = _mm_andnot_si128( Mask, A );
    A = _mm_or_si128( A, B );
    return _mm_castsi128_ps(A);
}

/** \SSE45{Reference,_mm_testc_si128,ptest} */
static __emu_inline int _mm_testc_si128_REF( __m128i a, __m128i b)
{
    __m128i A,B;
    A = a;
    B = b;

    return (((~A[0]) & B[0]) | ((~A[1]) & B[1])) == 0;
}

/** \SSE45{Reference,_mm_testz_si128,ptest} */
static __emu_inline int _mm_testz_si128_REF( __m128i a, __m128i b)
{
    __m128i A,B;
    A = a;
    B = b;

    return ( (A[0] & B[0]) | (A[1] & B[1])) == 0;
}

/** \SSE45{Reference,_mm_testnzc_si128,ptest} */
static __emu_inline int _mm_testnzc_si128_REF( __m128i a, __m128i b)
{
    int zf, cf;
    __m128i A,B;
    A = a;
    B = b;

    zf = _mm_testz_si128_REF(A, B);
    cf = _mm_testc_si128_REF(A, B);
    return ((int)!zf & (int)!cf);
}

/** \SSE3{SSE2,_mm_addsub_ps} */
static __emu_inline __m128 _mm_addsub_ps_SSE2(__m128 a, __m128 b)
{
    static __m128 const const_addSub_ps_neg  = { -1, 1, -1, 1 };

    b = _mm_mul_ps( b, const_addSub_ps_neg );
    a = _mm_add_ps( a, b   );
    return a;
}

/** \SSE3{SSE2,_mm_addsub_pd} */
static __emu_inline __m128d _mm_addsub_pd_SSE2(__m128d a, __m128d b)
{
    static __m128d const const_addSub_pd_neg = { -1, 1 };

    b = _mm_mul_pd( b, const_addSub_pd_neg );
    a = _mm_add_pd( a, b   );
    return a;
}

/** \SSE5{Reference,_mm_comgt_epi64, pcomq } */
static __emu_inline __m128i _mm_cmpgt_epi64_REF(__m128i a, __m128i b)
{
    __m128i A,B;
    A = a;
    B = b;
    A[0] = (A[0]>B[0]) ? 0xFFFFFFFFFFFFFFFF : 0;
    A[1] = (A[1]>B[1]) ? 0xFFFFFFFFFFFFFFFF : 0;
    return A;
}

//---------------------------------------
/** \SSE4_1{Reference,_mm_cmpeq_epi64} */
static __emu_inline __m128i _mm_cmpeq_epi64_REF( __m128i a, __m128i b )
{
    __m128i A, B;
    A = a;
    B = b;

    if( A[0] == B[0] )
        A[0] = 0xFFFFFFFFFFFFFFFFll;
    else
        A[0] = 0x0ll;

    if( A[1] == B[1] )
        A[1] = 0xFFFFFFFFFFFFFFFFll;
    else
        A[1] = 0x0ll;
    return A;
}

/** \SSE3{SSE2,_mm_movehdup_ps} */
static __emu_inline __m128 _mm_movehdup_ps_SSE2(__m128 a)
{
    __m128 A;
    A = _mm_castsi128_ps(_mm_shuffle_epi32( _mm_castps_si128(a), _MM_SHUFFLE( 3, 3, 1, 1) ));
    return A;
}

/** \SSE3{SSE2,_mm_moveldup_ps} */
static __emu_inline __m128 _mm_moveldup_ps_SSE2(__m128 a)
{
    __m128 A;
    A = _mm_castsi128_ps(_mm_shuffle_epi32( _mm_castps_si128(a), _MM_SHUFFLE( 2, 2, 0, 0) ));
    return A;
}

/** \SSE3{SSE2,_mm_movedup_pd} */
static __emu_inline __m128d _mm_movedup_pd_SSE2(__m128d a)
{
    __m128d A;
    A = a;
    return _mm_set_pd( A[0], A[0] );
}

/** \SSE3{SSE2,_mm_hadd_ps} */
static __emu_inline __m128 _mm_hadd_ps_SSE2(__m128 a, __m128 b)
{
    ssp_convert_odd_even_ps_SSE2( &a, &b );
    a = _mm_add_ps( a, b );
    return a;
}

/** \SSE3{SSE2,_mm_hadd_pd} */
static __emu_inline __m128d _mm_hadd_pd_SSE2(__m128d a, __m128d b)
{
    __m128d A,B,C;
    A = a;
    C = a;
    B = b;

    A = _mm_castps_pd( _mm_movelh_ps( _mm_castpd_ps(A), _mm_castpd_ps(B) ) );
    B = _mm_castps_pd( _mm_movehl_ps( _mm_castpd_ps(B), _mm_castpd_ps(C) ) );
    A = _mm_add_pd   ( A, B );
    return A;
}

/** \SSE3{SSE2,_mm_hsub_ps} */
static __emu_inline __m128 _mm_hsub_ps_SSE2(__m128 a, __m128 b)
{
    ssp_convert_odd_even_ps_SSE2( &a, &b );
    a = _mm_sub_ps( b, a );
    return a;
}

/** \SSE3{SSE2,_mm_hsub_pd} */
static __emu_inline __m128d _mm_hsub_pd_SSE2(__m128d a, __m128d b)
{
    __m128d A,B,C;
    A = a;
    C = a;
    B = b;

    A = _mm_castps_pd( _mm_movelh_ps( _mm_castpd_ps(A), _mm_castpd_ps(B) ) );
    B = _mm_castps_pd( _mm_movehl_ps( _mm_castpd_ps(B), _mm_castpd_ps(C) ) );
    A = _mm_sub_pd   ( A, B );
    return A;
}

/** \SSE4_1{Reference,_mm_insert_epi8} */
static __emu_inline __m128i _mm_insert_epi8_REF( __m128i a, int b, const int ndx )
{
    __m128i A;
    A = a;

    ((int8_t*)(&A))[ndx & 0xF] = b;
    return A;
}

/** \SSE4_1{Reference,_mm_insert_epi32} */
static __emu_inline __m128i _mm_insert_epi32_REF( __m128i a, int b, const int ndx )
{
    __m128i A;
    A = a;

    ((int*)(&A))[ndx & 0x3] = b;
    return A;
}

/** \SSE4_1{Reference,_mm_insert_epi64} */
static __emu_inline __m128i _mm_insert_epi64_REF( __m128i a, long long b, const int ndx )
{
    __m128i A;
    A = a;

    A[ndx & 0x1] = b;
    return A;
}

/** \SSE4_1{Reference,_mm_extract_epi64} */
static __emu_inline __emu_int64_t _mm_extract_epi64_REF( __m128i a, const int ndx )
{
    __m128i A;
    A = a;
    return A[ndx & 0x1];
}

//__m128i _mm_shuffle_epi8( __m128i a, __m128i mask);
/**  \SSSE3{Reference,_mm_shuffle_epi8} */
static __emu_inline __m128i _mm_shuffle_epi8_REF (__m128i a, __m128i mask)
{
 __m128i ret;
 int8_t A[16], MSK[16], B[16];
 memcpy(A,&a,16);
 memcpy(MSK,&mask,16);
// full unroll is faster
 B[0]  = (MSK[0]  & 0x80) ? 0 : A[(MSK[0]  & 0xf)];
 B[1]  = (MSK[1]  & 0x80) ? 0 : A[(MSK[1]  & 0xf)];
 B[2]  = (MSK[2]  & 0x80) ? 0 : A[(MSK[2]  & 0xf)];
 B[3]  = (MSK[3]  & 0x80) ? 0 : A[(MSK[3]  & 0xf)];
 B[4]  = (MSK[4]  & 0x80) ? 0 : A[(MSK[4]  & 0xf)];
 B[5]  = (MSK[5]  & 0x80) ? 0 : A[(MSK[5]  & 0xf)];
 B[6]  = (MSK[6]  & 0x80) ? 0 : A[(MSK[6]  & 0xf)];
 B[7]  = (MSK[7]  & 0x80) ? 0 : A[(MSK[7]  & 0xf)];
 B[8]  = (MSK[8]  & 0x80) ? 0 : A[(MSK[8]  & 0xf)];
 B[9]  = (MSK[9]  & 0x80) ? 0 : A[(MSK[9]  & 0xf)];
 B[10] = (MSK[10] & 0x80) ? 0 : A[(MSK[10] & 0xf)];
 B[11] = (MSK[11] & 0x80) ? 0 : A[(MSK[11] & 0xf)];
 B[12] = (MSK[12] & 0x80) ? 0 : A[(MSK[12] & 0xf)];
 B[13] = (MSK[13] & 0x80) ? 0 : A[(MSK[13] & 0xf)];
 B[14] = (MSK[14] & 0x80) ? 0 : A[(MSK[14] & 0xf)];
 B[15] = (MSK[15] & 0x80) ? 0 : A[(MSK[15] & 0xf)];

 memcpy(&ret,B,16);
 return ret;
}

/** \SSSE3{SSE2,_mm_maddubs_epi16} 

in:  2 registers x 16 x 8 bit values (a is unsigned, b is signed)
out: 1 register  x 8  x 16 bit values

r0 := SATURATE_16((a0 * b0) + (a1 * b1))

*/
static __emu_inline __m128i _mm_maddubs_epi16_SSE2( __m128i a,  __m128i b)
{
    const __m128i EVEN_8 = _mm_set_epi8( 0,0xFF,0,0xFF,0,0xFF,0,0xFF,0,0xFF,0,0xFF,0,0xFF,0,0xFF);
    __m128i Aodd, Aeven, Beven, Bodd;

    // Convert the 8 bit inputs into 16 bits by dropping every other value
    Aodd  = _mm_srli_epi16( a, 8 );             // A is unsigned  
    Bodd  = _mm_srai_epi16( b, 8 );             // B is signed

    Aeven = _mm_and_si128 ( a, EVEN_8 );        // A is unsigned   
    Beven = _mm_slli_si128( b,     1  );        // B is signed
    Beven = _mm_srai_epi16( Beven, 8  );

    a = _mm_mullo_epi16( Aodd , Bodd  );        // Will always fit in lower 16
    b = _mm_mullo_epi16( Aeven, Beven );  
    a = _mm_adds_epi16 ( a, b );
   	return a;
}

/** \{Reference,_mm256_blend_epi32} */
static __emu_inline __emu__m256i __emu_mm256_blend_epi32_REF        ( __emu__m256i a, __emu__m256i b, const int mask )
{
    __emu__m256i ret;
    ret.__emu_arr[0] = (mask & 0x1) ? b.__emu_arr[0] : a.__emu_arr[0];
    ret.__emu_arr[1] = (mask & 0x2) ? b.__emu_arr[1] : a.__emu_arr[1];
    ret.__emu_arr[2] = (mask & 0x4) ? b.__emu_arr[2] : a.__emu_arr[2];
    ret.__emu_arr[3] = (mask & 0x8) ? b.__emu_arr[3] : a.__emu_arr[3];
    ret.__emu_arr[4] = (mask & 0x10) ? b.__emu_arr[4] : a.__emu_arr[4];
    ret.__emu_arr[5] = (mask & 0x20) ? b.__emu_arr[5] : a.__emu_arr[5];
    ret.__emu_arr[6] = (mask & 0x40) ? b.__emu_arr[6] : a.__emu_arr[6];
    ret.__emu_arr[7] = (mask & 0x80) ? b.__emu_arr[7] : a.__emu_arr[7];
    return ret;
}

/** \AVX{Reference,_mm256_extract_epi64} */
static __emu_inline __emu_int64_t __emu_mm256_extract_epi64( __emu__m256i a, const int ndx )
{
    __emu__m256i A;
    A = a;
    return ((__emu_int64_t*)(&A))[ndx & 0x3];
}

/*
 * Compare predicates for scalar and packed compare intrinsics
 */
#define _CMP_EQ_OQ     0x00  /* Equal (ordered, nonsignaling)                       */
#define _CMP_LT_OS     0x01  /* Less-than (ordered, signaling)                      */
#define _CMP_LE_OS     0x02  /* Less-than-or-equal (ordered, signaling)             */
#define _CMP_UNORD_Q   0x03  /* Unordered (nonsignaling)                            */
#define _CMP_NEQ_UQ    0x04  /* Not-equal (unordered, nonsignaling)                 */
#define _CMP_NLT_US    0x05  /* Not-less-than (unordered, signaling)                */
#define _CMP_NLE_US    0x06  /* Not-less-than-or-equal (unordered, signaling)       */
#define _CMP_ORD_Q     0x07  /* Ordered (nonsignaling)                              */

#define _CMP_EQ_UQ     0x08  /* Equal (unordered, non-signaling)                    */
#define _CMP_NGE_US    0x09  /* Not-greater-than-or-equal (unordered, signaling)    */
#define _CMP_NGT_US    0x0A  /* Not-greater-than (unordered, signaling)             */
#define _CMP_FALSE_OQ  0x0B  /* False (ordered, nonsignaling)                       */
#define _CMP_NEQ_OQ    0x0C  /* Not-equal (ordered, non-signaling)                  */
#define _CMP_GE_OS     0x0D  /* Greater-than-or-equal (ordered, signaling)          */
#define _CMP_GT_OS     0x0E  /* Greater-than (ordered, signaling)                   */
#define _CMP_TRUE_UQ   0x0F  /* True (unordered, non-signaling)                     */
#define _CMP_EQ_OS     0x10  /* Equal (ordered, signaling)                          */
#define _CMP_LT_OQ     0x11  /* Less-than (ordered, nonsignaling)                   */
#define _CMP_LE_OQ     0x12  /* Less-than-or-equal (ordered, nonsignaling)          */
#define _CMP_UNORD_S   0x13  /* Unordered (signaling)                               */
#define _CMP_NEQ_US    0x14  /* Not-equal (unordered, signaling)                    */
#define _CMP_NLT_UQ    0x15  /* Not-less-than (unordered, nonsignaling)             */
#define _CMP_NLE_UQ    0x16  /* Not-less-than-or-equal (unordered, nonsignaling)    */
#define _CMP_ORD_S     0x17  /* Ordered (signaling)                                 */
#define _CMP_EQ_US     0x18  /* Equal (unordered, signaling)                        */
#define _CMP_NGE_UQ    0x19  /* Not-greater-than-or-equal (unordered, nonsignaling) */
#define _CMP_NGT_UQ    0x1A  /* Not-greater-than (unordered, nonsignaling)          */
#define _CMP_FALSE_OS  0x1B  /* False (ordered, signaling)                          */
#define _CMP_NEQ_OS    0x1C  /* Not-equal (ordered, signaling)                      */
#define _CMP_GE_OQ     0x1D  /* Greater-than-or-equal (ordered, nonsignaling)       */
#define _CMP_GT_OQ     0x1E  /* Greater-than (ordered, nonsignaling)                */
#define _CMP_TRUE_US   0x1F  /* True (unordered, signaling)                         */

// avx2
static __emu_inline __emu__m256i __emu_mm256_broadcastb_epi8        ( __m128i a )
{
    __emu__m256i A;
    __m128i av = a;
    memset(&A, *(int8_t*)&av, 32);
    return A;
}

static __emu_inline __emu__m256i __emu_mm256_broadcastd_epi32       ( __m128i a )
{
    __emu__m256i A;
    int *m = (int*) &a;
    int *p = (int*) &A;
    for (int i=0; i<8; i++) p[i] = m[0];
    return A;
}

static __emu_inline __emu__m256i __emu_mm256_broadcastq_epi64       ( __m128i a )
{
    __emu__m256i A;
    __emu_int64_t *m = (__emu_int64_t*) &a;
    __emu_int64_t *p = (__emu_int64_t*) &A;
    for (int i=0; i<4; i++) p[i] = m[0];
    return A;
}

static __emu_inline __emu__m256i __emu_mm256_broadcastsi128_si256       ( __m128i a )
{
    __emu__m256i A;
    A.__emu_m128[0] = a;
    A.__emu_m128[1] = a;
    return A;
}

static __emu_inline __emu__m256i __emu_mm256_slli_epi64 ( __emu__m256i a, int imm )
{
    __emu__m256i A;
    A.__emu_m128[0] = _mm_slli_epi64( a.__emu_m128[0], imm);
    A.__emu_m128[1] = _mm_slli_epi64( a.__emu_m128[1], imm);
    return A;
}

static __emu_inline __emu__m256i __emu_mm256_srli_epi16 ( __emu__m256i a, int imm )
{
    __emu__m256i A;
    A.__emu_m128[0] = _mm_srli_epi16( a.__emu_m128[0], imm);
    A.__emu_m128[1] = _mm_srli_epi16( a.__emu_m128[1], imm);
    return A;
}

static __emu_inline __emu__m256i __emu_mm256_srli_epi32 ( __emu__m256i a, int imm )
{
    __emu__m256i A;
    A.__emu_m128[0] = _mm_srli_epi32( a.__emu_m128[0], imm);
    A.__emu_m128[1] = _mm_srli_epi32( a.__emu_m128[1], imm);
    return A;
}

static __emu_inline __emu__m256i __emu_mm256_srli_epi64 ( __emu__m256i a, int imm )
{
    __emu__m256i A;
    A.__emu_m128[0] = _mm_srli_epi64( a.__emu_m128[0], imm);
    A.__emu_m128[1] = _mm_srli_epi64( a.__emu_m128[1], imm);
    return A;
}

#if defined(__clang__)
#define __emu_mm256_srli_si256( a, imm ) \
({                                       \
    __emu__m256i A;                      \
    A.__emu_m128[0] = _mm_srli_si128( a.__emu_m128[0], imm); \
    A.__emu_m128[1] = _mm_srli_si128( a.__emu_m128[1], imm); \
    A; \
})
#else
static __emu_inline __emu__m256i __emu_mm256_srli_si256 ( __emu__m256i a, int imm )
{
    __emu__m256i A;
    A.__emu_m128[0] = _mm_srli_si128( a.__emu_m128[0], imm);
    A.__emu_m128[1] = _mm_srli_si128( a.__emu_m128[1], imm);
    return A;
}
#endif
// avx2
__EMU_M256_IMPL_M2( __m256i, cmpgt_epi16 );
__EMU_M256_IMPL_M2( __m256i, cmpgt_epi32 );
__EMU_M256_IMPL_M2( __m256i, cmpgt_epi64 );
__EMU_M256_IMPL_M2( __m256i, cmpgt_epi8 );
__EMU_M256_IMPL_M2( __m256i, add_epi64 );
__EMU_M256_IMPL_M2( __m256i, sub_epi64 );

__EMU_M256_IMPL_M2( __m256d, add_pd );
__EMU_M256_IMPL_M2( __m256, add_ps );

__EMU_M256_IMPL_M2( __m256d, addsub_pd );
__EMU_M256_IMPL_M2( __m256, addsub_ps  );

__EMU_M256_IMPL_M2( __m256d, and_pd  );
__EMU_M256_IMPL_M2( __m256, and_ps );

__EMU_M256_IMPL_M2( __m256d, andnot_pd );
__EMU_M256_IMPL_M2( __m256, andnot_ps );

__EMU_M256_IMPL_M2( __m256d, div_pd );
__EMU_M256_IMPL_M2( __m256, div_ps );

__EMU_M256_IMPL_M2( __m256d, hadd_pd );
__EMU_M256_IMPL_M2( __m256, hadd_ps );

__EMU_M256_IMPL_M2( __m256d, hsub_pd );
__EMU_M256_IMPL_M2( __m256, hsub_ps );

__EMU_M256_IMPL_M2( __m256d, max_pd );
__EMU_M256_IMPL_M2( __m256, max_ps );

__EMU_M256_IMPL_M2( __m256d, min_pd );
__EMU_M256_IMPL_M2( __m256, min_ps );

__EMU_M256_IMPL_M2( __m256d, mul_pd );
__EMU_M256_IMPL_M2( __m256, mul_ps );

__EMU_M256_IMPL_M2( __m256d, or_pd );
__EMU_M256_IMPL_M2( __m256, or_ps );

#if defined(__clang__)
#define __emu_mm256_shuffle_pd( m256_param1, m256_param2, param3 ) \
({   __emu__m256d res; \
    res.__emu_m128[0] = _mm_shuffle_pd( m256_param1.__emu_m128[0], m256_param2.__emu_m128[0], (param3) & ((1<<2)-1) ); \
    res.__emu_m128[1] = _mm_shuffle_pd( m256_param1.__emu_m128[1], m256_param2.__emu_m128[1], (param3) >> 2 ); \
    res; \
})
#define __emu_mm256_shuffle_ps( m256_param1, m256_param2, param3 ) \
({   __emu__m256 res; \
    res.__emu_m128[0] = _mm_shuffle_ps( m256_param1.__emu_m128[0], m256_param2.__emu_m128[0], param3 ); \
    res.__emu_m128[1] = _mm_shuffle_ps( m256_param1.__emu_m128[1], m256_param2.__emu_m128[1], param3 ); \
    res; \
})
#else
__EMU_M256_IMPL_M2I_SHIFT( __m256d, shuffle_pd, 2 );
__EMU_M256_IMPL_M2I_DUP( __m256, shuffle_ps );
#endif

__EMU_M256_IMPL_M2( __m256d, sub_pd );
__EMU_M256_IMPL_M2( __m256, sub_ps );

__EMU_M256_IMPL_M2( __m256d, xor_pd );
__EMU_M256_IMPL_M2( __m256, xor_ps );

#if defined (__SSE4_2__) || defined (__SSE4_1__)

#if defined(__clang__)
#define  __emu_mm256_blend_pd( m256_param1, m256_param2, param3 ) \
({   __emu__m256d res; \
    res.__emu_m128[0] = _mm_blend_pd( m256_param1.__emu_m128[0], m256_param2.__emu_m128[0], (param3) & ((1<<2)-1) ); \
    res.__emu_m128[1] = _mm_blend_pd( m256_param1.__emu_m128[1], m256_param2.__emu_m128[1], (param3) >> 2 ); \
     res ; \
})
#define __emu_mm256_blend_ps( m256_param1, m256_param2, param3 ) \
({   __emu__m256 res; \
    res.__emu_m128[0] = _mm_blend_ps( m256_param1.__emu_m128[0], m256_param2.__emu_m128[0], (param3) & ((1<<4)-1) ); \
    res.__emu_m128[1] = _mm_blend_ps( m256_param1.__emu_m128[1], m256_param2.__emu_m128[1], (param3) >> 4 ); \
     res ; \
})
#define __emu_mm256_blendv_pd( m256_param1, m256_param2, m256_param3 ) \
({   __emu__m256d res; \
    res.__emu_m128[0] = _mm_blendv_pd( m256_param1.__emu_m128[0], m256_param2.__emu_m128[0], m256_param3.__emu_m128[0] ); \
    res.__emu_m128[1] = _mm_blendv_pd( m256_param1.__emu_m128[1], m256_param2.__emu_m128[1], m256_param3.__emu_m128[1] ); \
     res ; \
})
#define __emu_mm256_blendv_ps( m256_param1, m256_param2, m256_param3 ) \
({   __emu__m256 res; \
    res.__emu_m128[0] = _mm_blendv_ps( m256_param1.__emu_m128[0], m256_param2.__emu_m128[0], m256_param3.__emu_m128[0] ); \
    res.__emu_m128[1] = _mm_blendv_ps( m256_param1.__emu_m128[1], m256_param2.__emu_m128[1], m256_param3.__emu_m128[1] ); \
     res ; \
})
#else
__EMU_M256_IMPL_M2I_SHIFT( __m256d, blend_pd, 2 );
__EMU_M256_IMPL_M2I_SHIFT( __m256, blend_ps, 4 );

__EMU_M256_IMPL_M3( __m256d, blendv_pd );
__EMU_M256_IMPL_M3( __m256, blendv_ps );
#endif

#else

__EMU_M256_IMPL_M2I_SHIFT( __m256d, blend_pd, 2 );
__EMU_M256_IMPL_M2I_SHIFT( __m256, blend_ps, 4 );

__EMU_M256_IMPL_M3( __m256d, blendv_pd );
__EMU_M256_IMPL_M3( __m256, blendv_ps );

#endif

#if defined (__SSE4_2__) || defined (__SSE4_1__)

#if defined(__clang__)
#define __emu_mm256_dp_ps( m256_param1, m256_param2, param3 ) \
({   __emu__m256 res; \
    res.__emu_m128[0] = _mm_dp_ps( m256_param1.__emu_m128[0], m256_param2.__emu_m128[0], param3 ); \
    res.__emu_m128[1] = _mm_dp_ps( m256_param1.__emu_m128[1], m256_param2.__emu_m128[1], param3 ); \
    res ; \
})
#define __emu_mm256_round_pd(  m256_param1, param2 ) \
({   __emu__m256d res; \
    res.__emu_m128[0] = _mm_round_pd( m256_param1.__emu_m128[0], param2 ); \
    res.__emu_m128[1] = _mm_round_pd( m256_param1.__emu_m128[1], param2 ); \
    res ; \
})
#else
__EMU_M256_IMPL_M2I_DUP( __m256, dp_ps );
__EMU_M256_IMPL_M1I_DUP( __m256d, round_pd );
#endif
#define _mm256_ceil_pd(val)   _mm256_round_pd((val), 0x0A);
#define _mm256_floor_pd(val)  _mm256_round_pd((val), 0x09);

#if defined(__clang__)
#define __emu_mm256_round_ps(  m256_param1, param2 ) \
({   __emu__m256d res; \
    res.__emu_m128[0] = _mm_round_ps( m256_param1.__emu_m128[0], param2 ); \
    res.__emu_m128[1] = _mm_round_ps( m256_param1.__emu_m128[1], param2 ); \
    res ; \
})
#else
__EMU_M256_IMPL_M1I_DUP( __m256, round_ps );
#endif
#define _mm256_ceil_ps(val)   _mm256_round_ps((val), 0x0A);
#define _mm256_floor_ps(val)  _mm256_round_ps((val), 0x09);

#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#if defined (__SSE4_2__) || defined (__SSE4_1__)
#define __emu_mm_test_impl( op, sfx, vec_type ) \
static __emu_inline int     __emu_mm_test##op##_##sfx(vec_type s1, vec_type s2) {               \
    __m128d sign_bits_pd = _mm_castsi128_pd( _mm_set_epi32( 1 << 31, 0, 1 << 31, 0 ) );         \
    __m128  sign_bits_ps = _mm_castsi128_ps( _mm_set1_epi32( 1 << 31 ) );                       \
                                                                                                \
    s1 = _mm_and_##sfx( s1,  sign_bits_##sfx );                                                 \
    s2 = _mm_and_##sfx( s2,  sign_bits_##sfx );                                                 \
    return _mm_test##op##_si128( _mm_cast##sfx##_si128( s1 ), _mm_cast##sfx##_si128( s2 ) );        \
}

__emu_mm_test_impl( z, pd, __m128d );
__emu_mm_test_impl( c, pd, __m128d );
__emu_mm_test_impl( nzc, pd, __m128d );

__emu_mm_test_impl( z, ps, __m128 );
__emu_mm_test_impl( c, ps, __m128 );
__emu_mm_test_impl( nzc, ps, __m128 );
#else
#define __emu_mm_test_impl( op, sfx, vec_type ) \
static __emu_inline int     __emu_mm_test##op##_##sfx(vec_type s1, vec_type s2) {               \
    __m128d sign_bits_pd = _mm_castsi128_pd( _mm_set_epi32( 1 << 31, 0, 1 << 31, 0 ) );         \
    __m128  sign_bits_ps = _mm_castsi128_ps( _mm_set1_epi32( 1 << 31 ) );                       \
                                                                                                \
    s1 = _mm_and_##sfx( s1,  sign_bits_##sfx );                                                 \
    s2 = _mm_and_##sfx( s2,  sign_bits_##sfx );                                                 \
    return _mm_test##op##_si128_REF( _mm_cast##sfx##_si128( s1 ), _mm_cast##sfx##_si128( s2 ) );        \
}

__emu_mm_test_impl( z, pd, __m128d );
__emu_mm_test_impl( c, pd, __m128d );
__emu_mm_test_impl( nzc, pd, __m128d );

__emu_mm_test_impl( z, ps, __m128 );
__emu_mm_test_impl( c, ps, __m128 );
__emu_mm_test_impl( nzc, ps, __m128 );
#endif

#pragma GCC diagnostic pop

#define __emu_mm256_test_impl( prfx, op, sfx, sfx_impl, vec_type ) \
static __emu_inline int     __emu_mm256_test##op##_##sfx(vec_type s1, vec_type s2) { \
    int ret1 = prfx##_test##op##_##sfx_impl( s1.__emu_m128[0], s2.__emu_m128[0] );           \
    int ret2 = prfx##_test##op##_##sfx_impl( s1.__emu_m128[1], s2.__emu_m128[1] );           \
    return ( ret1 && ret2 );                                                         \
};

#if defined (__SSE4_2__) || defined (__SSE4_1__)

__emu_mm256_test_impl( _mm, z,   si256, si128, __emu__m256i );
__emu_mm256_test_impl( _mm, c,   si256, si128, __emu__m256i );
__emu_mm256_test_impl( _mm, nzc, si256, si128, __emu__m256i );

__emu_mm256_test_impl( __emu_mm, z,   pd, pd, __emu__m256d );
__emu_mm256_test_impl( __emu_mm, c,   pd, pd, __emu__m256d );
__emu_mm256_test_impl( __emu_mm, nzc, pd, pd, __emu__m256d );

__emu_mm256_test_impl( __emu_mm, z,   ps, ps, __emu__m256 );
__emu_mm256_test_impl( __emu_mm, c,   ps, ps, __emu__m256 );
__emu_mm256_test_impl( __emu_mm, nzc, ps, ps, __emu__m256 );

#else

__emu_mm256_test_impl( _mm, z,   si256, si128, __emu__m256i );
__emu_mm256_test_impl( _mm, c,   si256, si128, __emu__m256i );
__emu_mm256_test_impl( _mm, nzc, si256, si128, __emu__m256i );

__emu_mm256_test_impl( __emu_mm, z,   pd, pd, __emu__m256d );
__emu_mm256_test_impl( __emu_mm, c,   pd, pd, __emu__m256d );
__emu_mm256_test_impl( __emu_mm, nzc, pd, pd, __emu__m256d );

__emu_mm256_test_impl( __emu_mm, z,   ps, ps, __emu__m256 );
__emu_mm256_test_impl( __emu_mm, c,   ps, ps, __emu__m256 );
__emu_mm256_test_impl( __emu_mm, nzc, ps, ps, __emu__m256 );

#endif

#if defined(_WIN32) || ( !defined(__clang__) && defined( __GNUC__ ) && ( __GNUC__ == 4 ) && (__GNUC_MINOR__ < 4 ) )
/* use macro implementation instead of inline functions to allow -O0 for GCC pre 4.4 */

// #pragma message ("Using macro for GCC <4.4" )

#define __emu_mm_cmp_ps(m1, m2, predicate) \
({ \
    __m128 res_ = (m1), m2_ = (m2); \
    if ( 7 < (unsigned)predicate ) __asm__ __volatile__ ( "ud2" : : : "memory" ); \
    __asm__ ( "cmpps %[pred_], %[m2_], %[res_]" : [res_] "+x" (res_) : [m2_] "x" (m2_), [pred_] "i" (predicate) ); \
    res_; })

#define __emu_mm256_cmp_ps(m1, m2, predicate) \
({ \
    __emu__m256 res_ = (m1), m2_ = (m2); \
    if ( 7 < (unsigned)predicate ) __asm__ __volatile__ ( "ud2" : : : "memory" ); /* not supported yet */ \
    __asm__ ( "cmpps %[pred_], %[m2_], %[res_]" : [res_] "+x" (res_.__emu_m128[0]) : [m2_] "x" (m2_.__emu_m128[0]), [pred_] "i" (predicate) ); \
    __asm__ ( "cmpps %[pred_], %[m2_], %[res_]" : [res_] "+x" (res_.__emu_m128[1]) : [m2_] "x" (m2_.__emu_m128[1]), [pred_] "i" (predicate) ); \
    res_; })


#define __emu_mm_cmp_pd(m1, m2, predicate) \
({ \
    __m128d res_ = (m1), m2_ = (m2); \
    if ( 7 < (unsigned)predicate ) __asm__ __volatile__ ( "ud2" : : : "memory" ); /* not supported yet */ \
    __asm__ ( "cmppd %[pred_], %[m2_], %[res_]" : [res_] "+x" (res_) : [m2_] "x" (m2_), [pred_] "i" (predicate) ); \
    res_; })

#define __emu_mm256_cmp_pd(m1, m2, predicate) \
({ \
    __emu__m256d res_ = (m1), m2_ = (m2); \
    if ( 7 < (unsigned)predicate ) __asm__ __volatile__ ( "ud2" : : : "memory" ); /* not supported yet */ \
    __asm__ ( "cmppd %[pred_], %[m2_], %[res_]" : [res_] "+x" (res_.__emu_m128[0]) : [m2_] "x" (m2_.__emu_m128[0]), [pred_] "i" (predicate) ); \
    __asm__ ( "cmppd %[pred_], %[m2_], %[res_]" : [res_] "+x" (res_.__emu_m128[1]) : [m2_] "x" (m2_.__emu_m128[1]), [pred_] "i" (predicate) ); \
    res_; })


#define __emu_mm_cmp_ss(m1, m2, predicate) \
({ \
    __m128 res_ = (m1), m2_ = (m2); \
    if ( 7 < (unsigned)predicate ) __asm__ __volatile__ ( "ud2" : : : "memory" ); /* not supported yet */ \
    __asm__ ( "cmpss %[pred_], %[m2_], %[res_]" : [res_] "+x" (res_) : [m2_] "x" (m2_), [pred_] "i" (predicate) ); \
    res_; })

#define __emu_mm_cmp_sd(m1, m2, predicate) \
({ \
    __m128 res_ = (m1), m2_ = (m2); \
    if ( 7 < (unsigned)predicate ) __asm__ __volatile__ ( "ud2" : : : "memory" ); /* not supported yet */ \
    __asm__ ( "cmpsd %[pred_], %[m2_], %[res_]" : [res_] "+x" (res_) : [m2_] "x" (m2_), [pred_] "i" (predicate) ); \
    res_; })



#else /* __GNUC__==4 && __GNUC_MINOR__ <4 */


static __emu_inline __m128 __emu_mm_cmp_ps(__m128 m1, __m128 m2, const int predicate)
{
    __m128 res;

    if (__builtin_constant_p(predicate) && predicate >= 0 && predicate <= 7 ) {
        res = m1;
        __asm__ ( "cmpps %[pred_], %[m2_], %[res_]" : [res_] "+x" (res) : [m2_] "x" (m2), [pred_] "i" (predicate) );
    } else {
        res = _mm_setzero_ps();
        __asm__ __volatile__ ( "ud2" : : : "memory" ); /* not supported yet */
    }

    return ( res );
}
__EMU_M256_IMPL2_M2I_DUP( __m256, cmp_ps )

static __emu_inline __m128d __emu_mm_cmp_pd(__m128d m1, __m128d m2, const int predicate)
{
    __m128d res=res;

    if ( predicate >= 0 && predicate <= 7 ) {
        res = m1;
        __asm__ ( "cmppd %[pred_], %[m2_], %[res_]" : [res_] "+x" (res) : [m2_] "x" (m2), [pred_] "i" (predicate) );
    } else {
        __asm__ __volatile__ ( "ud2" : : : "memory" ); /* not supported yet */
    }

    return ( res );
}
__EMU_M256_IMPL2_M2I_DUP( __m256d, cmp_pd )


static __emu_inline __m128d __emu_mm_cmp_sd(__m128d m1, __m128d m2, const int predicate)
{
    __m128d res=res;

    if ( predicate >= 0 && predicate <= 7 ) {
        res = m1;
        __asm__ ( "cmpsd %[pred_], %[m2_], %[res_]" : [res_] "+x" (res) : [m2_] "x" (m2), [pred_] "i" (predicate) );
    } else {
        __asm__ __volatile__ ( "ud2" : : : "memory" ); /* not supported yet */
    }

    return ( res );
}

static __emu_inline __m128 __emu_mm_cmp_ss(__m128 m1, __m128 m2, const int predicate)
{
    __m128 res=res;

    if ( predicate >= 0 && predicate <= 7 ) {
        res = m1;
        __asm__ ( "cmpss %[pred_], %[m2_], %[res_]" : [res_] "+x" (res) : [m2_] "x" (m2), [pred_] "i" (predicate) );
    } else {
        __asm__ __volatile__ ( "ud2" : : : "memory" ); /* not supported yet */
    }

    return ( res );
}

#endif


__EMU_M256_IMPL_M1_LH( __m256d, __m128i, cvtepi32_pd );
__EMU_M256_IMPL_M1_RET( __m256, __m256i, cvtepi32_ps );
__EMU_M256_IMPL_M1_HL( __m128, __m256d, cvtpd_ps );
__EMU_M256_IMPL_M1_RET( __m256i, __m256, cvtps_epi32 );
__EMU_M256_IMPL_M1_LH( __m256d, __m128, cvtps_pd );
__EMU_M256_IMPL_M1_HL( __m128i, __m256d, cvttpd_epi32);
__EMU_M256_IMPL_M1_HL( __m128i, __m256d, cvtpd_epi32);
__EMU_M256_IMPL_M1_RET( __m256i, __m256, cvttps_epi32 );

// avx2
__EMU_M256_IMPL_M2( __m256i, add_epi16 );
__EMU_M256_IMPL_M2( __m256i, add_epi32 );
__EMU_M256_IMPL_M2( __m256i, add_epi8 );
__EMU_M256_IMPL_M2( __m256i, cmpeq_epi16 );
__EMU_M256_IMPL_M2( __m256i, cmpeq_epi32 );
__EMU_M256_IMPL_M2( __m256i, cmpeq_epi64 );
__EMU_M256_IMPL_M2( __m256i, cmpeq_epi8 );
__EMU_M256_IMPL_M2( __m256i, mul_epu32 );
__EMU_M256_IMPL_M2( __m256i, sub_epi8 );
__EMU_M256_128_IMPL_M2( __m256i, and_si  );
__EMU_M256_128_IMPL_M2( __m256i, xor_si  );
__EMU_M256_128_IMPL_M2( __m256i, or_si  );
__EMU_M256_128_IMPL_M2( __m256i, andnot_si  );

static __emu_inline __m128  __emu_mm256_extractf128_ps(__emu__m256 m1, const int offset) { return m1.__emu_m128[ offset ]; }
static __emu_inline __m128d __emu_mm256_extractf128_pd(__emu__m256d m1, const int offset) { return m1.__emu_m128[ offset ]; }
static __emu_inline __m128i __emu_mm256_extractf128_si256(__emu__m256i m1, const int offset) { return m1.__emu_m128[ offset ]; }

static __emu_inline void __emu_mm256_zeroall(void) {}
static __emu_inline void __emu_mm256_zeroupper(void) {}

static __emu_inline __m128  __emu_mm_permutevar_ps(__m128 a, __m128i control)
{
    int const* sel = (int const*)&control;
    float const* src = (float const*)&a;
    __EMU_M256_ALIGN(16) float dest[4];
    int i=0;

    for (; i<4; ++i)
        dest[i] = src[ 3 & sel[i] ];

    return ( *(__m128*)dest );
}
__EMU_M256_IMPL2_M2T( __m256, __m256i, permutevar_ps );
/*
static __emu_inline __m128 __emu_mm_permute_ps(__m128 a, const int control) {
  return _mm_castsi128_ps( _mm_shuffle_epi32( *(__m128i*)&a, control ) );
}
*/
#define __emu_mm_permute_ps(a, control) _mm_castsi128_ps( _mm_shuffle_epi32( *(__m128i*)&a, control ) )
// TODO: clang
#if !defined(__clang__)
__EMU_M256_IMPL2_M1I_DUP( __m256, permute_ps );
#else
#define __emu_mm256_permute_ps(a, control) ({  \
    __emu__m256 A;   \
    A.__emu_m128[ 0 ] = _mm_castsi128_ps( _mm_shuffle_epi32( *(__m128i*)&(a.__emu_m128[0]), control ) );  \
    A.__emu_m128[ 1 ] = _mm_castsi128_ps( _mm_shuffle_epi32( *(__m128i*)&(a.__emu_m128[1]), control ) );  \
    A;})
#endif


static __emu_inline __m128d __emu_mm_permutevar_pd(__m128d a, __m128i control)
{
    __emu_int64_t const* sel = (__emu_int64_t const*)&control;
    double const* src = (double const*)&a;
    __EMU_M256_ALIGN(16) double dest[2];
    int i=0;

    for (; i<2; ++i)
        dest[i] = src[ (2 & sel[i]) >> 1 ];

    return ( *(__m128d*)dest );
}
__EMU_M256_IMPL2_M2T( __m256d, __m256i, permutevar_pd );

static __emu_inline __m128d __emu_mm_permute_pd(__m128d a, int control)
{
    double const* src = (double const*)&a;
    __EMU_M256_ALIGN(16) double dest[2];
    int i=0;

    for (; i<2; ++i)
        dest[i] = src[ 1 & (control >> i) ];

    return ( *(__m128d*)dest );
}
__EMU_M256_IMPL2_M1I_SHIFT( __m256d, permute_pd, 2 );


#define __emu_mm256_permute2f128_impl( name, m128_type, m256_type ) \
static __emu_inline m256_type name( m256_type m1, m256_type m2, int control) { \
    m256_type res; \
    __m128 zero = _mm_setzero_ps(); \
    const m128_type param[4] = { m1.__emu_m128[0], m1.__emu_m128[1], m2.__emu_m128[0], m2.__emu_m128[1] }; \
    res.__emu_m128[0] = (control & 8) ? *(m128_type*)&zero : param[ control & 0x3 ]; control >>= 4; \
    res.__emu_m128[1] = (control & 8) ? *(m128_type*)&zero : param[ control & 0x3 ]; \
    return ( res ); \
}

__emu_mm256_permute2f128_impl( __emu_mm256_permute2f128_ps, __m128, __emu__m256 );
__emu_mm256_permute2f128_impl( __emu_mm256_permute2f128_pd, __m128d, __emu__m256d );
__emu_mm256_permute2f128_impl( __emu_mm256_permute2f128_si256, __m128i, __emu__m256i );


#define __emu_mm_broadcast_impl( name, res_type, type )     \
static __emu_inline res_type  name(type const *a) {         \
    const size_t size = sizeof( res_type ) / sizeof( type );\
    __EMU_M256_ALIGN(32) type res[ size ];                  \
    size_t i = 0;                                           \
    for ( ; i < size; ++i )                                 \
        res[ i ] = *a;                                      \
    return (*(res_type*)&res);                              \
}

__emu_mm_broadcast_impl( __emu_mm_broadcast_ss, __m128, float )
__emu_mm_broadcast_impl( __emu_mm_broadcast_sd, __m128d, double )
__emu_mm_broadcast_impl( __emu_mm256_broadcast_ss, __emu__m256, float )
__emu_mm_broadcast_impl( __emu_mm256_broadcast_sd, __emu__m256d, double )

#define __emu_mm_broadcast2s_impl( name, res_type, type)    \
static __emu_inline res_type  name(type const *a) {         \
    type b = _mm_loadu_ps((float*)a);                       \
    res_type res;                                           \
    res.__emu_m128[ 0 ] = b;                                \
    res.__emu_m128[ 1 ] = b;                                \
    return res;                                             \
}

#define __emu_mm_broadcast2d_impl( name, res_type, type)    \
static __emu_inline res_type  name(type const *a) {         \
    type b = _mm_loadu_pd((double*)a);                      \
    res_type res;                                           \
    res.__emu_m128[ 0 ] = b;                                \
    res.__emu_m128[ 1 ] = b;                                \
    return res;                                             \
}

__emu_mm_broadcast2s_impl( __emu_mm256_broadcast_ps, __emu__m256, __m128 )
__emu_mm_broadcast2d_impl( __emu_mm256_broadcast_pd, __emu__m256d, __m128d )

static __emu_inline __emu__m256  __emu_mm256_insertf128_ps(__emu__m256 a, __m128 b, int offset)  { a.__emu_m128[ offset ] = b; return a; }
static __emu_inline __emu__m256d __emu_mm256_insertf128_pd(__emu__m256d a, __m128d b, int offset)  { a.__emu_m128[ offset ] = b; return a; }
static __emu_inline __emu__m256i __emu_mm256_insertf128_si256(__emu__m256i a, __m128i b, int offset)  { a.__emu_m128[ offset ] = b; return a; }


#define __emu_mm_load_impl( name, sfx, m256_sfx, m256_type, type_128, type )           \
static __emu_inline __emu##m256_type  __emu_mm256_##name##_##m256_sfx(const type* a) { \
    __emu##m256_type res;                                                              \
    res.__emu_m128[0] = _mm_##name##_##sfx( (const type_128 *)a );                     \
    res.__emu_m128[1] = _mm_##name##_##sfx( (const type_128 *)(1+(const __m128 *)a) ); \
    return (res);                                                                      \
}

#define __emu_mm_store_impl( name, sfx, m256_sfx, m256_type, type_128, type ) \
static __emu_inline void __emu_mm256_##name##_##m256_sfx(type *a, __emu##m256_type b) {  \
    _mm_##name##_##sfx( (type_128*)a, b.__emu_m128[0] );                                 \
    _mm_##name##_##sfx( (type_128*)(1+(__m128*)a), b.__emu_m128[1] );   \
}

__emu_mm_load_impl( load, pd, pd, __m256d, double, double );
__emu_mm_store_impl( store, pd, pd, __m256d, double, double );

__emu_mm_load_impl( load, ps, ps, __m256, float, float );
__emu_mm_store_impl( store, ps, ps, __m256, float, float );

__emu_mm_load_impl( loadu, pd, pd, __m256d, double, double );
__emu_mm_store_impl( storeu, pd, pd, __m256d, double, double );

__emu_mm_load_impl( loadu, ps, ps, __m256, float, float );
__emu_mm_store_impl( storeu, ps, ps, __m256, float, float );

__emu_mm_load_impl( load, si128, si256, __m256i, __m128i, __emu__m256i );
__emu_mm_store_impl( store, si128, si256, __m256i, __m128i, __emu__m256i );

__emu_mm_load_impl( loadu, si128, si256, __m256i, __m128i, __emu__m256i );
__emu_mm_store_impl( storeu, si128, si256, __m256i, __m128i, __emu__m256i );


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

__emu_maskload_impl( __emu_mm256_maskload_pd, __emu__m256d, __emu__m256i, double, __emu_int64_t );
__emu_maskstore_impl( __emu_mm256_maskstore_pd, __emu__m256d, __emu__m256i, double, __emu_int64_t );

__emu_maskload_impl( __emu_mm_maskload_pd, __m128d, __m128i, double, __emu_int64_t );
__emu_maskstore_impl( __emu_mm_maskstore_pd, __m128d, __m128i, double, __emu_int64_t );

__emu_maskload_impl( __emu_mm256_maskload_ps, __emu__m256, __emu__m256i, float, int );
__emu_maskstore_impl( __emu_mm256_maskstore_ps, __emu__m256, __emu__m256i, float, int );

__emu_maskload_impl( __emu_mm_maskload_ps, __m128, __m128i, float, int );
__emu_maskstore_impl( __emu_mm_maskstore_ps, __m128, __m128i, float, int );


__EMU_M256_IMPL_M1( __m256, movehdup_ps );
__EMU_M256_IMPL_M1( __m256, moveldup_ps );
__EMU_M256_IMPL_M1( __m256d, movedup_pd );

__emu_mm_load_impl( lddqu, si128, si256, __m256i, __m128i, __emu__m256i );

__emu_mm_store_impl( stream, si128, si256, __m256i, __m128i, __emu__m256i );
__emu_mm_store_impl( stream, pd, pd, __m256d, double, double );
__emu_mm_store_impl( stream, ps, ps, __m256, float, float );


__EMU_M256_IMPL_M1( __m256, rcp_ps );
__EMU_M256_IMPL_M1( __m256, rsqrt_ps );

__EMU_M256_IMPL_M1( __m256d, sqrt_pd );
__EMU_M256_IMPL_M1( __m256, sqrt_ps );

__EMU_M256_IMPL_M2( __m256d, unpackhi_pd );
__EMU_M256_IMPL_M2( __m256, unpackhi_ps );
__EMU_M256_IMPL_M2( __m256d, unpacklo_pd );
__EMU_M256_IMPL_M2( __m256, unpacklo_ps );


static __emu_inline int     __emu_mm256_movemask_pd(__emu__m256d a)
{
    return
        (_mm_movemask_pd( a.__emu_m128[1] ) << 2) |
        _mm_movemask_pd( a.__emu_m128[0] );
}

static __emu_inline int     __emu_mm256_movemask_ps(__emu__m256 a)
{
    return
        (_mm_movemask_ps( a.__emu_m128[1] ) << 4) |
         _mm_movemask_ps( a.__emu_m128[0] );
}

static __emu_inline __emu__m256d __emu_mm256_setzero_pd(void) { __m128d ret[2] = { _mm_setzero_pd(), _mm_setzero_pd() }; return __emu_set_m128d( ret ); }
static __emu_inline __emu__m256  __emu_mm256_setzero_ps(void) { __m128  ret[2] = { _mm_setzero_ps(), _mm_setzero_ps() }; return __emu_set_m128( ret ); }
static __emu_inline __emu__m256i __emu_mm256_setzero_si256(void) { __m128i ret[2] = { _mm_setzero_si128(), _mm_setzero_si128() }; return __emu_set_m128i( ret ); }

static __emu_inline __emu__m256d __emu_mm256_set_pd(double a1, double a2, double a3, double a4)
{ __m128d ret[2] = { _mm_set_pd( a3, a4 ), _mm_set_pd( a1, a2 ) }; return __emu_set_m128d( ret ); }

static __emu_inline __emu__m256  __emu_mm256_set_ps(float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8)
{ __m128 ret[2] = { _mm_set_ps( a5, a6, a7, a8 ), _mm_set_ps( a1, a2, a3, a4 ) }; return __emu_set_m128( ret ); }

static __emu_inline __emu__m256i __emu_mm256_set_epi8(char a1, char a2, char a3, char a4, char a5, char a6, char a7, char a8,
                                       char a9, char a10, char a11, char a12, char a13, char a14, char a15, char a16,
                                       char a17, char a18, char a19, char a20, char a21, char a22, char a23, char a24,
                                       char a25, char a26, char a27, char a28, char a29, char a30, char a31, char a32)
{   __m128i ret[2] = { _mm_set_epi8( a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32 ),
                       _mm_set_epi8( a1,   a2,  a3,  a4,  a5,  a6,  a7,  a8,  a9, a10, a11, a12, a13, a14, a15, a16 ) };
    return __emu_set_m128i( ret );
}

static __emu_inline __emu__m256i __emu_mm256_set_epi16(short a1, short a2, short a3, short a4, short a5, short a6, short a7, short a8,
                                                       short a9, short a10, short a11, short a12, short a13, short a14, short a15, short a16)
{   __m128i ret[2] = { _mm_set_epi16( a9, a10, a11, a12, a13, a14, a15, a16 ),
                       _mm_set_epi16( a1,  a2,  a3,  a4,  a5,  a6,  a7,  a8 ) };
    return __emu_set_m128i( ret );
}

static __emu_inline __emu__m256i __emu_mm256_set_epi32(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{ __m128i ret[2] = { _mm_set_epi32( a5, a6, a7, a8 ), _mm_set_epi32( a1, a2, a3, a4 ) }; return __emu_set_m128i( ret ); }

static __emu_inline __m128i __emu_mm_set_epi64x( __emu_int64_t a, __emu_int64_t b ) { return _mm_set_epi64( *(__m64*)&a, *(__m64*)&b ); }

static __emu_inline __emu__m256i __emu_mm256_set_epi64x(__emu_int64_t a1, __emu_int64_t a2, __emu_int64_t a3, __emu_int64_t a4)
{ __m128i ret[2] = { __emu_mm_set_epi64x( a3, a4 ), __emu_mm_set_epi64x( a1, a2 ) }; return __emu_set_m128i( ret ); }


static __emu_inline __emu__m256d __emu_mm256_setr_pd(double a1, double a2, double a3, double a4)
{ __m128d ret[2] = { _mm_setr_pd( a1, a2 ), _mm_setr_pd( a3, a4 ) }; return __emu_set_m128d( ret ); }

static __emu_inline __emu__m256  __emu_mm256_setr_ps(float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8)
{ __m128 ret[2] = { _mm_setr_ps( a1, a2, a3, a4 ), _mm_setr_ps( a5, a6, a7, a8 ) }; return __emu_set_m128( ret ); }

static __emu_inline __emu__m256i __emu_mm256_setr_epi8(char a1, char a2, char a3, char a4, char a5, char a6, char a7, char a8,
                                                      char a9, char a10, char a11, char a12, char a13, char a14, char a15, char a16,
                                                      char a17, char a18, char a19, char a20, char a21, char a22, char a23, char a24,
                                                      char a25, char a26, char a27, char a28, char a29, char a30, char a31, char a32)
{   __m128i ret[2] = { _mm_setr_epi8( a1,   a2,  a3,  a4,  a5,  a6,  a7,  a8,  a9, a10, a11, a12, a13, a14, a15, a16 ),
                       _mm_setr_epi8( a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32 ) };
    return __emu_set_m128i( ret );
}

static __emu_inline __emu__m256i __emu_mm256_setr_epi16(short a1, short a2, short a3, short a4, short a5, short a6, short a7, short a8,
                                                       short a9, short a10, short a11, short a12, short a13, short a14, short a15, short a16)
{   __m128i ret[2] = { _mm_setr_epi16( a1,  a2,  a3,  a4,  a5,  a6,  a7,  a8 ),
                       _mm_setr_epi16( a9, a10, a11, a12, a13, a14, a15, a16 ) }; return __emu_set_m128i( ret );
}

static __emu_inline __emu__m256i __emu_mm256_setr_epi32(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8)
{   __m128i ret[2] = { _mm_setr_epi32( a1, a2, a3, a4 ), _mm_setr_epi32( a5, a6, a7, a8 ),  }; return __emu_set_m128i( ret ); }

static __emu_inline __emu__m256i __emu_mm256_setr_epi64x(__emu_int64_t a1, __emu_int64_t a2, __emu_int64_t a3, __emu_int64_t a4)
{   __m128i ret[2] = { __emu_mm_set_epi64x( a2, a1 ), __emu_mm_set_epi64x( a4, a3 ) }; return __emu_set_m128i( ret ); }



__EMU_M256_IMPL_M1P_DUP( __m256d, double, set1_pd );
__EMU_M256_IMPL_M1P_DUP( __m256, float, set1_ps );
__EMU_M256_IMPL_M1P_DUP( __m256i, char, set1_epi8 );
__EMU_M256_IMPL_M1P_DUP( __m256i, short, set1_epi16 );
__EMU_M256_IMPL_M1P_DUP( __m256i, int, set1_epi32 );

static __emu_inline __emu__m256i __emu_mm256_set1_epi64x(__emu_int64_t a)
{
    __emu_int64_t res[4] = { a, a, a, a };
    return *((__emu__m256i*)res);
}

/*
 * Support intrinsics to do vector type casts. These intrinsics do not introduce
 * extra moves to generated code. When cast is done from a 128 to 256-bit type
 * the low 128 bits of the 256-bit result contain source parameter value; the
 * upper 128 bits of the result are undefined
 */
__EMU_M256_IMPL_M1_RET( __m256, __m256d, castpd_ps );
__EMU_M256_IMPL_M1_RET( __m256d, __m256, castps_pd );

__EMU_M256_IMPL_M1_RET_NAME( __m256i, __m256, castps_si128, castps_si256 );
__EMU_M256_IMPL_M1_RET_NAME( __m256i, __m256d, castpd_si128, castpd_si256 );

__EMU_M256_IMPL_M1_RET_NAME( __m256, __m256i, castsi128_ps, castsi256_ps );
__EMU_M256_IMPL_M1_RET_NAME( __m256d, __m256i, castsi128_pd, castsi256_pd );

static __emu_inline __m128  __emu_mm256_castps256_ps128(__emu__m256 a) { return ( a.__emu_m128[0] ); }
static __emu_inline __m128d __emu_mm256_castpd256_pd128(__emu__m256d a) { return ( a.__emu_m128[0] ); }
static __emu_inline __m128i __emu_mm256_castsi256_si128(__emu__m256i a) { return ( a.__emu_m128[0] ); }

static __emu_inline __emu__m256  __emu_mm256_castps128_ps256(__m128 a) { __m128 ret[2] = { a, _mm_setzero_ps() }; return __emu_set_m128( ret ); };
static __emu_inline __emu__m256d __emu_mm256_castpd128_pd256(__m128d a) { __m128d ret[2] = { a, _mm_setzero_pd() }; return __emu_set_m128d( ret ); };
static __emu_inline __emu__m256i __emu_mm256_castsi128_si256(__m128i a) { __m128i ret[2] = { a, _mm_setzero_si128() }; return __emu_set_m128i( ret ); };

// avx2
static __emu_inline int __emu_mm256_movemask_epi8(__emu__m256i a)
{
    return
        (_mm_movemask_epi8( a.__emu_m128[1] ) << 16) |
         _mm_movemask_epi8( a.__emu_m128[0] );
}

static __emu_inline __emu__m256i __emu_mm256_permute2x128_si256 (__emu__m256i a, __emu__m256i b, const int imm8) {
 __emu__m256i A;
	switch (imm8 & 0b11) {
	case 0:	A.__emu_m128[0]  = a.__emu_m128[0] ; break ;
	case 1:	A.__emu_m128[0]  = a.__emu_m128[1] ; break ;
	case 2:	A.__emu_m128[0]  = b.__emu_m128[0] ; break ;
	case 3:	A.__emu_m128[0]  = b.__emu_m128[1] ; break ;
	}
	if (imm8 & 0b1000)
  A.__emu_m128[0] = _mm_setzero_si128();

	switch ((imm8 >> 4) & 0b11) {
	case 0:	A.__emu_m128[1]  = a.__emu_m128[0] ; break ;
	case 1:	A.__emu_m128[1]  = a.__emu_m128[1] ; break ;
	case 2:	A.__emu_m128[1]  = b.__emu_m128[0] ; break ;
	case 3:	A.__emu_m128[1]  = b.__emu_m128[1] ; break ;
	}
	if (imm8 & 0b10000000)
  A.__emu_m128[1] = _mm_setzero_si128();

 return A;
}

static __emu_inline __emu__m256i __emu_mm256_permute4x64_epi64(__emu__m256i a, const int control)
{
    __emu__m256i A;
    __emu_int64_t* ptr_a = (__emu_int64_t*) &a;
    const int id0 = control & 0x03;
    const int id1 = (control >> 2) & 0x03;
    const int id2 = (control >> 4) & 0x03;
    const int id3 = (control >> 6) & 0x03;
    (A.__emu_m128[0])[0] = ptr_a[id0];
    (A.__emu_m128[0])[1] = ptr_a[id1];
    (A.__emu_m128[1])[0] = ptr_a[id2];
    (A.__emu_m128[1])[1] = ptr_a[id3];
    return A;
}

static __emu_inline __emu__m256d __emu_mm256_permute4x64_pd(__emu__m256d a, const int control)
{
    __emu__m256d A;
    double* ptr_a = (double*) &a;
    const int id0 = control & 0x03;
    const int id1 = (control >> 2) & 0x03;
    const int id2 = (control >> 4) & 0x03;
    const int id3 = (control >> 6) & 0x03;
    (A.__emu_m128[0])[0] = ptr_a[id0];
    (A.__emu_m128[0])[1] = ptr_a[id1];
    (A.__emu_m128[1])[0] = ptr_a[id2];
    (A.__emu_m128[1])[1] = ptr_a[id3];
    return A;
}

static __emu_inline __emu__m256d __emu_mm256_fnmadd_pd(__emu__m256d a, __emu__m256d b, __emu__m256d c)
{
    __emu__m256d A;
    A.__emu_m128[ 0 ] = _mm_sub_pd(c.__emu_m128[ 0 ],_mm_mul_pd(a.__emu_m128[ 0 ],b.__emu_m128[ 0 ]));
    A.__emu_m128[ 1 ] = _mm_sub_pd(c.__emu_m128[ 1 ],_mm_mul_pd(a.__emu_m128[ 1 ],b.__emu_m128[ 1 ]));
    return A;
}

static __emu_inline __emu__m256d __emu_mm256_fmadd_pd(__emu__m256d a, __emu__m256d b, __emu__m256d c)
{
    __emu__m256d A;
    A.__emu_m128[ 0 ] = _mm_add_pd(_mm_mul_pd(a.__emu_m128[ 0 ],b.__emu_m128[ 0 ]),c.__emu_m128[ 0 ]);
    A.__emu_m128[ 1 ] = _mm_add_pd(_mm_mul_pd(a.__emu_m128[ 1 ],b.__emu_m128[ 1 ]),c.__emu_m128[ 1 ]);
    return A;
}

static __emu_inline __emu__m256 __emu_mm256_fmadd_ps(__emu__m256 a, __emu__m256 b, __emu__m256 c)
{
    __emu__m256 A;
    A.__emu_m128[ 0 ] = _mm_add_ps(_mm_mul_ps(a.__emu_m128[ 0 ],b.__emu_m128[ 0 ]),c.__emu_m128[ 0 ]);
    A.__emu_m128[ 1 ] = _mm_add_ps(_mm_mul_ps(a.__emu_m128[ 1 ],b.__emu_m128[ 1 ]),c.__emu_m128[ 1 ]);
    return A;
}

static __emu_inline __emu__m256d __emu_mm256_fmsub_pd(__emu__m256d a, __emu__m256d b, __emu__m256d c)
{
    __emu__m256d A;
    A.__emu_m128[ 0 ] = _mm_sub_pd(_mm_mul_pd(a.__emu_m128[ 0 ],b.__emu_m128[ 0 ]),c.__emu_m128[ 0 ]);
    A.__emu_m128[ 1 ] = _mm_sub_pd(_mm_mul_pd(a.__emu_m128[ 1 ],b.__emu_m128[ 1 ]),c.__emu_m128[ 1 ]);
    return A;
}

static __emu_inline __emu__m256 __emu_mm256_fmsub_ps(__emu__m256 a, __emu__m256 b, __emu__m256 c)
{
    __emu__m256 A;
    A.__emu_m128[ 0 ] = _mm_sub_ps(_mm_mul_ps(a.__emu_m128[ 0 ],b.__emu_m128[ 0 ]),c.__emu_m128[ 0 ]);
    A.__emu_m128[ 1 ] = _mm_sub_ps(_mm_mul_ps(a.__emu_m128[ 1 ],b.__emu_m128[ 1 ]),c.__emu_m128[ 1 ]);
    return A;
}

static __emu_inline __emu__m256i __emu_mm256_i64gather_epi64(__emu_int64_t const *addr, __emu__m256i index, const int scale) {
 __emu__m256i A;
 __emu__m256i B;
 A = index;
 __emu_int64_t *p = (__emu_int64_t*) &B;
 p[0] = *(__emu_int64_t*)((int8_t*)addr + (A.__emu_m128[ 0 ])[0] * scale);
 p[1] = *(__emu_int64_t*)((int8_t*)addr + (A.__emu_m128[ 0 ])[1] * scale);
 p[2] = *(__emu_int64_t*)((int8_t*)addr + (A.__emu_m128[ 1 ])[0] * scale);
 p[3] = *(__emu_int64_t*)((int8_t*)addr + (A.__emu_m128[ 1 ])[1] * scale);
 return B;
}

static __emu_inline __emu__m256d __emu_mm256_i64gather_pd(double const *addr, __emu__m256i index, const int scale) {
 __emu__m256i A;
 __emu__m256d B;
 A = index;
 double *p = (double*) &B;
 p[0] = *(double*)((int8_t*)addr + (A.__emu_m128[ 0 ])[0] * scale);
 p[1] = *(double*)((int8_t*)addr + (A.__emu_m128[ 0 ])[1] * scale);
 p[2] = *(double*)((int8_t*)addr + (A.__emu_m128[ 1 ])[0] * scale);
 p[3] = *(double*)((int8_t*)addr + (A.__emu_m128[ 1 ])[1] * scale);
 return B;
}

static __emu_inline __emu__m256i __emu_mm256_mask_i64gather_epi64( __emu__m256i src, __emu_int64_t const *addr, __emu__m256i index, __emu__m256i mask, const int scale) {
 __emu__m256i A;
 __emu__m256i B;
 __emu__m256i C;
 __emu__m256i D;
 A = index;
 C = src;
 D = mask;
 __emu_int64_t *p = (__emu_int64_t*) &B;
 __emu_int64_t *m = (__emu_int64_t*) &D;
 p[0] = m[0] ? *(__emu_int64_t*)((int8_t*)addr + (A.__emu_m128[ 0 ])[0] * scale) : (C.__emu_m128[0])[0];
 p[1] = m[1] ? *(__emu_int64_t*)((int8_t*)addr + (A.__emu_m128[ 0 ])[1] * scale) : (C.__emu_m128[0])[1];
 p[2] = m[2] ? *(__emu_int64_t*)((int8_t*)addr + (A.__emu_m128[ 1 ])[0] * scale) : (C.__emu_m128[1])[0];
 p[3] = m[3] ? *(__emu_int64_t*)((int8_t*)addr + (A.__emu_m128[ 1 ])[1] * scale) : (C.__emu_m128[1])[1];
 return B;
}

// static __emu_inline int8_t mm_extract_epi8_var_indx(__m128i vec, int i )
// {
//     __m128i indx = _mm_cvtsi32_si128(i);
//     __m128i val  = _mm_shuffle_epi8(vec, indx);
//             return (uint8_t)_mm_cvtsi128_si32(val);
// }

__emu_maskload_impl( __emu_mm256_maskload_epi64, __emu__m256i, __emu__m256i, __emu_int64_t, __emu_int64_t );
__emu_maskstore_impl( __emu_mm256_maskstore_epi64, __emu__m256i, __emu__m256i, __emu_int64_t, __emu_int64_t );

/** \SSE4_1{SSE2,_mm_cvtepu8_epi64} */
static __emu_inline __m128i _mm_cvtepu8_epi64_SSE2 ( __m128i a)
{
 __m128i b = _mm_setzero_si128 ();
 a = _mm_unpacklo_epi8(a, b);
 a = _mm_unpacklo_epi16(a, b);
 return _mm_unpacklo_epi32(a, b);
}

static __emu_inline __emu__m256i __emu_mm256_cvtepu8_epi64(__m128i a)
{
 __emu__m256i res;
 res.__emu_m128[0] = _mm_cvtepu8_epi64(a);
 res.__emu_m128[1] = _mm_cvtepu8_epi64(_mm_srli_si128(a,2));
 return res;
}

static __emu_inline __emu__m256i __emu_mm256_sllv_epi64(__emu__m256i a, __emu__m256i count)
{
 __emu__m256i res;
 __emu_uint64_t *c = (__emu_uint64_t*) &count;
 __emu_int64_t *m = (__emu_int64_t*) &a;
 __emu_int64_t *p = (__emu_int64_t*) &res;
 for (int i=0; i<4; i++) p[i] = (c[i]<64) ? (m[i]<<c[i]) : 0;
 return res;
}

#if defined __cplusplus
}; /* End "C" */
#endif /* __cplusplus */

#ifndef __EMU_M256_NOMAP

#define __m256  __emu__m256
#define __m256i __emu__m256i
#define __m256d __emu__m256d

#define _mm256_add_pd    __emu_mm256_add_pd
#define _mm256_add_ps    __emu_mm256_add_ps

#define _mm256_addsub_pd __emu_mm256_addsub_pd
#define _mm256_addsub_ps __emu_mm256_addsub_ps

#define _mm256_and_pd __emu_mm256_and_pd
#define _mm256_and_ps __emu_mm256_and_ps

#define _mm256_andnot_pd __emu_mm256_andnot_pd
#define _mm256_andnot_ps __emu_mm256_andnot_ps

#define _mm256_blend_pd __emu_mm256_blend_pd
#define _mm256_blend_ps __emu_mm256_blend_ps

#define _mm256_blendv_pd __emu_mm256_blendv_pd
#define _mm256_blendv_ps __emu_mm256_blendv_ps

#define _mm256_div_pd __emu_mm256_div_pd
#define _mm256_div_ps __emu_mm256_div_ps

#define _mm256_dp_ps __emu_mm256_dp_ps

#define _mm256_hadd_pd __emu_mm256_hadd_pd
#define _mm256_hadd_ps __emu_mm256_hadd_ps

#define _mm256_hsub_pd __emu_mm256_hsub_pd
#define _mm256_hsub_ps __emu_mm256_hsub_ps

#define _mm256_max_pd __emu_mm256_max_pd
#define _mm256_max_ps __emu_mm256_max_ps

#define _mm256_min_pd __emu_mm256_min_pd
#define _mm256_min_ps __emu_mm256_min_ps

#define _mm256_mul_pd __emu_mm256_mul_pd
#define _mm256_mul_ps __emu_mm256_mul_ps

#define _mm256_or_pd __emu_mm256_or_pd
#define _mm256_or_ps __emu_mm256_or_ps

#define _mm256_shuffle_pd __emu_mm256_shuffle_pd
#define _mm256_shuffle_ps __emu_mm256_shuffle_ps

#define _mm256_sub_pd __emu_mm256_sub_pd
#define _mm256_sub_ps __emu_mm256_sub_ps

#define _mm256_xor_pd __emu_mm256_xor_pd
#define _mm256_xor_ps __emu_mm256_xor_ps


#define _mm_cmp_pd __emu_mm_cmp_pd
#define _mm256_cmp_pd __emu_mm256_cmp_pd

#define _mm_cmp_ps __emu_mm_cmp_ps
#define _mm256_cmp_ps __emu_mm256_cmp_ps

#define _mm_cmp_sd __emu_mm_cmp_sd
#define _mm_cmp_ss __emu_mm_cmp_ss

#define _mm256_cvtepi32_pd __emu_mm256_cvtepi32_pd
#define _mm256_cvtepi32_ps __emu_mm256_cvtepi32_ps

#define _mm256_cvtpd_ps __emu_mm256_cvtpd_ps
#define _mm256_cvtps_epi32 __emu_mm256_cvtps_epi32
#define _mm256_cvtps_pd __emu_mm256_cvtps_pd

#define _mm256_cvttpd_epi32 __emu_mm256_cvttpd_epi32
#define _mm256_cvtpd_epi32 __emu_mm256_cvtpd_epi32
#define _mm256_cvttps_epi32 __emu_mm256_cvttps_epi32

#define _mm256_extractf128_ps __emu_mm256_extractf128_ps
#define _mm256_extractf128_pd __emu_mm256_extractf128_pd
#define _mm256_extractf128_si256 __emu_mm256_extractf128_si256

#define _mm256_zeroall __emu_mm256_zeroall
#define _mm256_zeroupper __emu_mm256_zeroupper

#define _mm256_permutevar_ps __emu_mm256_permutevar_ps
#define _mm_permutevar_ps __emu_mm_permutevar_ps

#define _mm256_permute_ps __emu_mm256_permute_ps
#define _mm_permute_ps __emu_mm_permute_ps

#define _mm256_permutevar_pd __emu_mm256_permutevar_pd
#define _mm_permutevar_pd __emu_mm_permutevar_pd

#define _mm256_permute_pd __emu_mm256_permute_pd
#define _mm_permute_pd __emu_mm_permute_pd

#define _mm256_permute2f128_ps __emu_mm256_permute2f128_ps
#define _mm256_permute2f128_pd __emu_mm256_permute2f128_pd
#define _mm256_permute2f128_si256 __emu_mm256_permute2f128_si256

#define _mm256_broadcast_ss __emu_mm256_broadcast_ss
#define _mm_broadcast_ss __emu_mm_broadcast_ss

#define _mm256_broadcast_sd __emu_mm256_broadcast_sd

#define _mm256_broadcast_ps __emu_mm256_broadcast_ps
#define _mm256_broadcast_pd __emu_mm256_broadcast_pd

#define _mm256_insertf128_ps __emu_mm256_insertf128_ps
#define _mm256_insertf128_pd __emu_mm256_insertf128_pd
#define _mm256_insertf128_si256 __emu_mm256_insertf128_si256

#define _mm256_load_pd __emu_mm256_load_pd
#define _mm256_store_pd __emu_mm256_store_pd
#define _mm256_load_ps __emu_mm256_load_ps
#define _mm256_store_ps __emu_mm256_store_ps

#define _mm256_loadu_pd __emu_mm256_loadu_pd
#define _mm256_storeu_pd __emu_mm256_storeu_pd
#define _mm256_loadu_ps __emu_mm256_loadu_ps
#define _mm256_storeu_ps __emu_mm256_storeu_ps

#define _mm256_load_si256 __emu_mm256_load_si256
#define _mm256_store_si256 __emu_mm256_store_si256
#define _mm256_loadu_si256 __emu_mm256_loadu_si256
#define _mm256_storeu_si256 __emu_mm256_storeu_si256

#define _mm256_maskload_pd __emu_mm256_maskload_pd
#define _mm256_maskstore_pd __emu_mm256_maskstore_pd
#define _mm_maskload_pd __emu_mm_maskload_pd
#define _mm_maskstore_pd __emu_mm_maskstore_pd

#define _mm256_maskload_ps __emu_mm256_maskload_ps
#define _mm256_maskstore_ps __emu_mm256_maskstore_ps
#define _mm_maskload_ps __emu_mm_maskload_ps
#define _mm_maskstore_ps __emu_mm_maskstore_ps

#define _mm256_movehdup_ps __emu_mm256_movehdup_ps
#define _mm256_moveldup_ps __emu_mm256_moveldup_ps

#define _mm256_movedup_pd __emu_mm256_movedup_pd
#define _mm256_lddqu_si256 __emu_mm256_lddqu_si256

#define _mm256_stream_si256 __emu_mm256_stream_si256
#define _mm256_stream_pd __emu_mm256_stream_pd
#define _mm256_stream_ps __emu_mm256_stream_ps

#define _mm256_rcp_ps __emu_mm256_rcp_ps
#define _mm256_rsqrt_ps __emu_mm256_rsqrt_ps

#define _mm256_sqrt_pd __emu_mm256_sqrt_pd
#define _mm256_sqrt_ps __emu_mm256_sqrt_ps

#define _mm256_round_pd __emu_mm256_round_pd

#define _mm256_round_ps __emu_mm256_round_ps

#define _mm256_unpackhi_pd __emu_mm256_unpackhi_pd
#define _mm256_unpackhi_ps __emu_mm256_unpackhi_ps

#define _mm256_unpacklo_pd __emu_mm256_unpacklo_pd
#define _mm256_unpacklo_ps __emu_mm256_unpacklo_ps

#define _mm256_testz_si256 __emu_mm256_testz_si256
#define _mm256_testc_si256 __emu_mm256_testc_si256
#define _mm256_testnzc_si256 __emu_mm256_testnzc_si256

#define _mm256_testz_pd __emu_mm256_testz_pd
#define _mm256_testc_pd __emu_mm256_testc_pd
#define _mm256_testnzc_pd __emu_mm256_testnzc_pd

#define _mm256_testz_ps __emu_mm256_testz_ps
#define _mm256_testc_ps __emu_mm256_testc_ps
#define _mm256_testnzc_ps __emu_mm256_testnzc_ps

#define _mm_testz_pd __emu_mm_testz_pd
#define _mm_testc_pd __emu_mm_testc_pd
#define _mm_testnzc_pd __emu_mm_testnzc_pd

#define _mm_testz_ps __emu_mm_testz_ps
#define _mm_testc_ps __emu_mm_testc_ps
#define _mm_testnzc_ps __emu_mm_testnzc_ps

#define _mm256_movemask_pd __emu_mm256_movemask_pd
#define _mm256_movemask_ps __emu_mm256_movemask_ps

#define _mm256_setzero_pd __emu_mm256_setzero_pd
#define _mm256_setzero_ps __emu_mm256_setzero_ps
#define _mm256_setzero_si256 __emu_mm256_setzero_si256

#define _mm256_set_pd __emu_mm256_set_pd
#define _mm256_set_ps __emu_mm256_set_ps
#define _mm256_set_epi8 __emu_mm256_set_epi8
#define _mm256_set_epi16 __emu_mm256_set_epi16
#define _mm256_set_epi32 __emu_mm256_set_epi32
#define _mm256_set_epi64x __emu_mm256_set_epi64x

#define _mm256_setr_pd __emu_mm256_setr_pd
#define _mm256_setr_ps __emu_mm256_setr_ps
#define _mm256_setr_epi8 __emu_mm256_setr_epi8
#define _mm256_setr_epi16 __emu_mm256_setr_epi16
#define _mm256_setr_epi32 __emu_mm256_setr_epi32
#define _mm256_setr_epi64x __emu_mm256_setr_epi64x

#define _mm256_set1_pd __emu_mm256_set1_pd
#define _mm256_set1_ps __emu_mm256_set1_ps
#define _mm256_set1_epi8 __emu_mm256_set1_epi8
#define _mm256_set1_epi16 __emu_mm256_set1_epi16
#define _mm256_set1_epi32 __emu_mm256_set1_epi32
#define _mm256_set1_epi64x __emu_mm256_set1_epi64x

#define _mm256_castpd_ps __emu_mm256_castpd_ps
#define _mm256_castps_pd __emu_mm256_castps_pd
#define _mm256_castps_si256 __emu_mm256_castps_si256
#define _mm256_castpd_si256 __emu_mm256_castpd_si256
#define _mm256_castsi256_ps __emu_mm256_castsi256_ps
#define _mm256_castsi256_pd __emu_mm256_castsi256_pd
#define _mm256_castps256_ps128 __emu_mm256_castps256_ps128
#define _mm256_castpd256_pd128 __emu_mm256_castpd256_pd128
#define _mm256_castsi256_si128 __emu_mm256_castsi256_si128
#define _mm256_castps128_ps256 __emu_mm256_castps128_ps256
#define _mm256_castpd128_pd256 __emu_mm256_castpd128_pd256
#define _mm256_castsi128_si256 __emu_mm256_castsi128_si256

// avx2
#define _mm256_add_epi16 __emu_mm256_add_epi16
#define _mm256_add_epi32 __emu_mm256_add_epi32
#define _mm256_add_epi8 __emu_mm256_add_epi8
#define _mm256_add_epi64 __emu_mm256_add_epi64
#define _mm256_sub_epi64 __emu_mm256_sub_epi64

#define _mm256_and_si256 __emu_mm256_and_si256
#define _mm256_andnot_si256 __emu_mm256_andnot_si256
#define _mm256_or_si256 __emu_mm256_or_si256
#define _mm256_xor_si256 __emu_mm256_xor_si256
#define _mm256_sub_epi8 __emu_mm256_sub_epi8

#define _mm256_mul_epu32 __emu_mm256_mul_epu32
#define _mm256_slli_epi64 __emu_mm256_slli_epi64
#define _mm256_srli_epi16 __emu_mm256_srli_epi16
#define _mm256_srli_epi32 __emu_mm256_srli_epi32
#define _mm256_srli_epi64 __emu_mm256_srli_epi64
#define _mm256_srli_si256 __emu_mm256_srli_si256

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

#define _mm256_maskload_epi64 __emu_mm256_maskload_epi64
#define _mm256_maskstore_epi64 __emu_mm256_maskstore_epi64

#define _mm256_movemask_epi8 __emu_mm256_movemask_epi8

#define _mm256_blend_epi32 __emu_mm256_blend_epi32_REF
#define _mm256_extract_epi64 __emu_mm256_extract_epi64

#define _mm256_fnmadd_pd __emu_mm256_fnmadd_pd
#define _mm256_fmadd_pd __emu_mm256_fmadd_pd
#define _mm256_fmadd_ps __emu_mm256_fmadd_ps
#define _mm256_fmsub_pd __emu_mm256_fmsub_pd
#define _mm256_fmsub_ps __emu_mm256_fmsub_ps
#define _mm256_i64gather_epi64 __emu_mm256_i64gather_epi64
#define _mm256_i64gather_pd __emu_mm256_i64gather_pd
#define _mm256_mask_i64gather_epi64 __emu_mm256_mask_i64gather_epi64
#define _mm256_permute2x128_si256 __emu_mm256_permute2x128_si256
#define _mm256_permute4x64_epi64 __emu_mm256_permute4x64_epi64
#define _mm256_permute4x64_pd __emu_mm256_permute4x64_pd

#define _mm256_cvtepu8_epi64 __emu_mm256_cvtepu8_epi64
#define _mm256_sllv_epi64 __emu_mm256_sllv_epi64

#endif /* __EMU_M256_NOMAP */

#endif /* __EMU_M256_AVXIMMINTRIN_EMU_H__ */
