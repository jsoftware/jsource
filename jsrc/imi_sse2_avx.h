#ifndef _IMI_SSE2_AVX
#define _IMI_SSE2_AVX

// utilities for avx imitation

#if !C_AVX && defined(__SSE2__)

#include <emmintrin.h>

#define __m256 __m128       // MUST have this line
#define __m256d __m128d
#define __m256i __m128i

#undef _mm256_zeroupper
#define _mm256_zeroupper(x)
#undef VOIDARG
#define VOIDARG

#undef _mm256_and_pd
#undef _mm256_castsi256_pd
#undef _mm256_cmp_pd
#undef _mm256_div_pd
#undef _mm256_maskload_pd
#undef _mm256_maskstore_pd
#undef _mm256_movemask_pd
#undef _mm256_mul_pd
#undef _mm256_set1_epi64x
#undef _mm256_set1_pd
#undef _mm256_setzero_pd

#undef _mm256_loadu_pd
#undef _mm256_storeu_pd
#undef _mm256_loadu_si256

#define _mm256_storeu_pd _mm_storeu_pd
#define _mm256_loadu_pd _mm_loadu_pd
#define _mm256_loadu_si256 _mm_loadu_si128

#define _mm256_and_pd _mm_and_pd
#define _mm256_castsi256_pd _mm_castsi128_pd
#define _mm256_cmp_pd __emu_mm_cmp_pd
#define _mm256_div_pd _mm_div_pd
#define _mm256_maskload_pd __emu_mm_maskload_pd
#define _mm256_maskstore_pd __emu_mm_maskstore_pd
#define _mm256_movemask_pd _mm_movemask_pd
#define _mm256_mul_pd _mm_mul_pd
#define _mm256_set1_epi64x _mm_set1_epi64x
#define _mm256_set1_pd _mm_set1_pd
#define _mm256_setzero_pd _mm_setzero_pd

#define _CMP_EQ          0
#define _CMP_LT          1
#define _CMP_LE          2
#define _CMP_UNORD       3
#define _CMP_NEQ         4
#define _CMP_NLT         5
#define _CMP_NLE         6
#define _CMP_ORD         7

#undef _CMP_EQ_OQ
#undef _CMP_GE_OQ
#undef _CMP_GT_OQ
#undef _CMP_LE_OQ
#undef _CMP_LT_OQ
#undef _CMP_NEQ_OQ

#define _CMP_EQ_OQ _CMP_EQ
#define _CMP_GE_OQ _CMP_NLT
#define _CMP_GT_OQ _CMP_NLE
#define _CMP_LE_OQ _CMP_LE
#define _CMP_LT_OQ _CMP_LT
#define _CMP_NEQ_OQ _CMP_NEQ

#if defined( MMSC_VER )
    #define __EMU_M256_ALIGN( a ) __declspec(align(a))
    #define __emu_inline          __forceinline
    #define __emu_int64_t         __int64
#else
    #define __EMU_M256_ALIGN( a ) __attribute__((__aligned__(a)))
    #define __emu_inline          __inline __attribute__((__always_inline__))
    #define __emu_int64_t         long long
#endif
#if defined( MMSC_VER )
static __emu_inline __m128d __emu_mm_maskload_pd(double const *a, __m128i mask) {
    const size_t size_type = sizeof( double );
    const size_t size = sizeof( __m128d ) / size_type;
    __EMU_M256_ALIGN(32) double res[ 2 ];           // double res[ size ];  is a c99 feature unavailable in msvc
    const __emu_int64_t* p_mask = (const __emu_int64_t*)&mask;
    size_t i = 0;
    __emu_int64_t sign_bit = 1;
    sign_bit <<= (8*size_type - 1);
    for ( ; i < size; ++i )
        res[ i ] = (sign_bit & *(p_mask + i)) ? *(a+i) : 0;
    return (*(__m128d*)&res);
}

static __emu_inline void __emu_mm_maskstore_pd(double *a, __m128i mask, __m128d data) {
    const size_t size_type = sizeof( double );
    const size_t size = sizeof( __m128d ) / sizeof( double );
    double* p_data = (double*)&data;
    const __emu_int64_t* p_mask = (const __emu_int64_t*)&mask;
    size_t i = 0;
    __emu_int64_t sign_bit = 1;
    sign_bit <<= (8*size_type - 1);
    for ( ; i < size; ++i )
        if ( *(p_mask + i ) & sign_bit)
            *(a + i) = *(p_data + i);
}

#else
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
        res[ i ] = (sign_bit & *(p_mask + i)) ? *(a+i) : 0;           \
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
        if ( *(p_mask + i ) & sign_bit)                               \
            *(a + i) = *(p_data + i);                                 \
}

__emu_maskload_impl( __emu_mm_maskload_pd, __m128d, __m128i, double, __emu_int64_t );
__emu_maskstore_impl( __emu_mm_maskstore_pd, __m128d, __m128i, double, __emu_int64_t );
#endif

#ifdef MMSC_VER
static __emu_inline __m128d __emu_mm_cmp_pd(__m128d m1, __m128d m2, int predicate) {
    switch (predicate) {
    case 0: return _mm_cmpeq_pd(m1,m2);
    case 1: return _mm_cmplt_pd(m1,m2);
    case 2: return _mm_cmple_pd(m1,m2);
    case 3: return _mm_cmpunord_pd(m1,m2);
    case 4: return _mm_cmpneq_pd(m1,m2);
    case 5: return _mm_cmpge_pd(m1,m2);
    case 6: return _mm_cmpgt_pd(m1,m2);
    case 7: return _mm_cmpord_pd(m1,m2);
    default: *(volatile size_t*)0 = 0;
    }
}
#else
#define __emu_mm_cmp_pd(m1, m2, predicate) \
({ \
    __m128 res_ = (m1), m2_ = (m2); \
    if ( 7 < (unsigned)predicate ) __asm__ __volatile__ ( "ud2" : : : "memory" ); /* not supported yet */ \
    __asm__ ( "cmppd %[pred_], %[m2_], %[res_]" : [res_] "+x" (res_) : [m2_] "xm" (m2_), [pred_] "i" (predicate) ); \
    res_; })
#endif

#endif

#endif
