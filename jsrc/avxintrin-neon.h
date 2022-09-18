#include <arm_neon.h>

#include <fenv.h>
#include <math.h>
#include <assert.h>
#ifdef __cplusplus
using namespace std;
#endif

#if defined(__GNUC__) || defined(__clang__)

#pragma push_macro("FORCE_INLINE")
#pragma push_macro("ALIGN_STRUCT")
#define FORCE_INLINE static inline __attribute__((always_inline))
#define ALIGN_STRUCT(x) __attribute__((aligned(x)))

#else

#error "Macro name collisions may happens with unknown compiler"
#ifdef FORCE_INLINE
#undef FORCE_INLINE
#endif

#define FORCE_INLINE static inline
#ifndef ALIGN_STRUCT
#define ALIGN_STRUCT(x) __declspec(align(x))
#endif

#endif

static uint64_t g_mask_epi64[2] __attribute__((aligned(16))) = {0x01, 0x02};
static uint32_t g_mask_epi32[4] __attribute__((aligned(16))) = {0x01, 0x02, 0x04, 0x08};
static uint16_t g_mask_epi16[8] __attribute__((aligned(16))) = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
static uint8_t g_mask_epi8[16] __attribute__((aligned(16))) = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

typedef union {
    int8x16_t vect_s8;
    int16x8_t vect_s16;
    int32x4_t vect_s32;
    int64x2_t vect_s64;
    uint8x16_t vect_u8;
    uint16x8_t vect_u16;
    uint32x4_t vect_u32;
    uint64x2_t vect_u64;
} __m128I __attribute__((aligned(16)));

typedef union {
    int8x16_t vect_s8[2];
    int16x8_t vect_s16[2];
    int32x4_t vect_s32[2];
    int64x2_t vect_s64[2];
    uint8x16_t vect_u8[2];
    uint16x8_t vect_u16[2];
    uint32x4_t vect_u32[2];
    uint64x2_t vect_u64[2];
    int64x2x2_t vect_i128;
} __m256i __attribute__((aligned(32)));

typedef union {
    float32x4_t vect_f32[2];
    float32x4x2_t vect_i128;
} __m256 __attribute__((aligned(32)));

typedef union {
    float64x2_t vect_f64[2];
    float64x2x2_t vect_i128;
} __m256d __attribute__((aligned(32)));

typedef union {
    __m256 m256;
    __m256d m256d;
    __m256i m256i;
} __m256I __attribute__((aligned(32)));

#define _CMP_EQ_OQ 0x00
#define _CMP_LT_OS 0x01
#define _CMP_LE_OS 0x02
#define _CMP_UNORD_Q 0x03
#define _CMP_NEQ_UQ 0x04
#define _CMP_NLT_US 0x05
#define _CMP_NLE_US 0x06
#define _CMP_ORD_Q 0x07
#define _CMP_EQ_UQ 0x08
#define _CMP_NGE_US 0x09
#define _CMP_NGT_US 0x0a
#define _CMP_FALSE_OQ 0x0b
#define _CMP_NEQ_OQ 0x0c
#define _CMP_GE_OS 0x0d
#define _CMP_GT_OS 0x0e
#define _CMP_TRUE_UQ 0x0f
#define _CMP_EQ_OS 0x10
#define _CMP_LT_OQ 0x11
#define _CMP_LE_OQ 0x12
#define _CMP_UNORD_S 0x13
#define _CMP_NEQ_US 0x14
#define _CMP_NLT_UQ 0x15
#define _CMP_NLE_UQ 0x16
#define _CMP_ORD_S 0x17
#define _CMP_EQ_US 0x18
#define _CMP_NGE_UQ 0x19
#define _CMP_NGT_UQ 0x1a
#define _CMP_FALSE_OS 0x1b
#define _CMP_NEQ_OS 0x1c
#define _CMP_GE_OQ 0x1d
#define _CMP_GT_OQ 0x1e
#define _CMP_TRUE_US 0x1f

FORCE_INLINE __m256i _mm256_setzero_si256(void)
{
    __m256i ret;
    ret.vect_s32[0] = ret.vect_s32[1] = vdupq_n_s32(0);
    return ret;
}

FORCE_INLINE __m256 _mm256_setzero_ps(void)
{
    __m256 ret;
    ret.vect_f32[0] = ret.vect_f32[1] = vdupq_n_f32(0.0f);
    return ret;
}

FORCE_INLINE __m256d _mm256_setzero_pd(void)
{
    __m256d ret;
    ret.vect_f64[0] = ret.vect_f64[1] = vdupq_n_f64(0.0);
    return ret;
}

FORCE_INLINE void _mm256_convert_to_int32(int32_t* ptr_a, __m256i a)
{
    ptr_a[0] = vgetq_lane_s32(a.vect_s32[0], 0);
    ptr_a[1] = vgetq_lane_s32(a.vect_s32[0], 1);
    ptr_a[2] = vgetq_lane_s32(a.vect_s32[0], 2);
    ptr_a[3] = vgetq_lane_s32(a.vect_s32[0], 3);
    ptr_a[4] = vgetq_lane_s32(a.vect_s32[1], 0);
    ptr_a[5] = vgetq_lane_s32(a.vect_s32[1], 1);
    ptr_a[6] = vgetq_lane_s32(a.vect_s32[1], 2);
    ptr_a[7] = vgetq_lane_s32(a.vect_s32[1], 3);
}

FORCE_INLINE void _mm256_convert_to_int64(int64_t* ptr_a, __m256i a)
{
    ptr_a[0] = vgetq_lane_s64(a.vect_s64[0], 0);
    ptr_a[1] = vgetq_lane_s64(a.vect_s64[0], 1);
    ptr_a[2] = vgetq_lane_s64(a.vect_s64[1], 0);
    ptr_a[3] = vgetq_lane_s64(a.vect_s64[1], 1);
}

FORCE_INLINE void _mm256_convert_to_uint32(uint32_t* ptr_a, __m256i a)
{
    ptr_a[0] = vgetq_lane_u32(a.vect_u32[0], 0);
    ptr_a[1] = vgetq_lane_u32(a.vect_u32[0], 1);
    ptr_a[2] = vgetq_lane_u32(a.vect_u32[0], 2);
    ptr_a[3] = vgetq_lane_u32(a.vect_u32[0], 3);
    ptr_a[4] = vgetq_lane_u32(a.vect_u32[1], 0);
    ptr_a[5] = vgetq_lane_u32(a.vect_u32[1], 1);
    ptr_a[6] = vgetq_lane_u32(a.vect_u32[1], 2);
    ptr_a[7] = vgetq_lane_u32(a.vect_u32[1], 3);
}

FORCE_INLINE void _mm256_convert_to_uint64(uint64_t* ptr_a, __m256i a)
{
    ptr_a[0] = vgetq_lane_u64(a.vect_u64[0], 0);
    ptr_a[1] = vgetq_lane_u64(a.vect_u64[0], 1);
    ptr_a[2] = vgetq_lane_u64(a.vect_u64[1], 0);
    ptr_a[3] = vgetq_lane_u64(a.vect_u64[1], 1);
}

FORCE_INLINE __m256i _mm256_div_epi8(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_i128.val[0] = _mm_div_epi8(a.vect_i128.val[0], b.vect_i128.val[0]);
    res_m256i.vect_i128.val[1] = _mm_div_epi8(a.vect_i128.val[1], b.vect_i128.val[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_div_epi16(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_i128.val[0] = _mm_div_epi16(a.vect_i128.val[0], b.vect_i128.val[0]);
    res_m256i.vect_i128.val[1] = _mm_div_epi16(a.vect_i128.val[1], b.vect_i128.val[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_div_epi32(__m256i a, __m256i b)
{
    __m256i res = _mm256_setzero_si256();
    int32_t ptr_a[8], ptr_b[8], ptr_r[8];
    _mm256_convert_to_int32(ptr_a, a);
    _mm256_convert_to_int32(ptr_b, b);
    ptr_r[0] = ptr_a[0] / ptr_b[0];
    ptr_r[1] = ptr_a[1] / ptr_b[1];
    ptr_r[2] = ptr_a[2] / ptr_b[2];
    ptr_r[3] = ptr_a[3] / ptr_b[3];
    ptr_r[4] = ptr_a[4] / ptr_b[4];
    ptr_r[5] = ptr_a[5] / ptr_b[5];
    ptr_r[6] = ptr_a[6] / ptr_b[6];
    ptr_r[7] = ptr_a[7] / ptr_b[7];
    res.vect_s32[0] = vsetq_lane_s32(ptr_r[0], res.vect_s32[0], 0);
    res.vect_s32[0] = vsetq_lane_s32(ptr_r[1], res.vect_s32[0], 1);
    res.vect_s32[0] = vsetq_lane_s32(ptr_r[2], res.vect_s32[0], 2);
    res.vect_s32[0] = vsetq_lane_s32(ptr_r[3], res.vect_s32[0], 3);
    res.vect_s32[1] = vsetq_lane_s32(ptr_r[4], res.vect_s32[1], 0);
    res.vect_s32[1] = vsetq_lane_s32(ptr_r[5], res.vect_s32[1], 1);
    res.vect_s32[1] = vsetq_lane_s32(ptr_r[6], res.vect_s32[1], 2);
    res.vect_s32[1] = vsetq_lane_s32(ptr_r[7], res.vect_s32[1], 3);
    return res;
}

FORCE_INLINE __m256i _mm256_div_epi64(__m256i a, __m256i b)
{
    __m256i res = _mm256_setzero_si256();
    int64_t ptr_a[4], ptr_b[4], ptr_r[4];
    _mm256_convert_to_int64(ptr_a, a);
    _mm256_convert_to_int64(ptr_b, b);
    ptr_r[0] = ptr_a[0] / ptr_b[0];
    ptr_r[1] = ptr_a[1] / ptr_b[1];
    ptr_r[2] = ptr_a[2] / ptr_b[2];
    ptr_r[3] = ptr_a[3] / ptr_b[3];
    res.vect_s64[0] = vsetq_lane_s64(ptr_r[0], res.vect_s64[0], 0);
    res.vect_s64[0] = vsetq_lane_s64(ptr_r[1], res.vect_s64[0], 1);
    res.vect_s64[1] = vsetq_lane_s64(ptr_r[2], res.vect_s64[1], 0);
    res.vect_s64[1] = vsetq_lane_s64(ptr_r[3], res.vect_s64[1], 1);
    return res;
}
FORCE_INLINE __m256i _mm256_div_epu8(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_i128.val[0] = _mm_div_epu8(a.vect_i128.val[0], b.vect_i128.val[0]);
    res_m256i.vect_i128.val[1] = _mm_div_epu8(a.vect_i128.val[1], b.vect_i128.val[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_div_epu16(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_i128.val[0] = _mm_div_epu16(a.vect_i128.val[0], b.vect_i128.val[0]);
    res_m256i.vect_i128.val[1] = _mm_div_epu16(a.vect_i128.val[1], b.vect_i128.val[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_div_epu32(__m256i a, __m256i b)
{
    __m256i res = _mm256_setzero_si256();
    uint32_t ptr_a[8], ptr_b[8], ptr_r[8];
    _mm256_convert_to_uint32(ptr_a, a);
    _mm256_convert_to_uint32(ptr_b, b);
    ptr_r[0] = ptr_a[0] / ptr_b[0];
    ptr_r[1] = ptr_a[1] / ptr_b[1];
    ptr_r[2] = ptr_a[2] / ptr_b[2];
    ptr_r[3] = ptr_a[3] / ptr_b[3];
    ptr_r[4] = ptr_a[4] / ptr_b[4];
    ptr_r[5] = ptr_a[5] / ptr_b[5];
    ptr_r[6] = ptr_a[6] / ptr_b[6];
    ptr_r[7] = ptr_a[7] / ptr_b[7];
    res.vect_u32[0] = vsetq_lane_u32(ptr_r[0], res.vect_u32[0], 0);
    res.vect_u32[0] = vsetq_lane_u32(ptr_r[1], res.vect_u32[0], 1);
    res.vect_u32[0] = vsetq_lane_u32(ptr_r[2], res.vect_u32[0], 2);
    res.vect_u32[0] = vsetq_lane_u32(ptr_r[3], res.vect_u32[0], 3);
    res.vect_u32[1] = vsetq_lane_u32(ptr_r[4], res.vect_u32[1], 0);
    res.vect_u32[1] = vsetq_lane_u32(ptr_r[5], res.vect_u32[1], 1);
    res.vect_u32[1] = vsetq_lane_u32(ptr_r[6], res.vect_u32[1], 2);
    res.vect_u32[1] = vsetq_lane_u32(ptr_r[7], res.vect_u32[1], 3);
    return res;
}

FORCE_INLINE __m256i _mm256_div_epu64(__m256i a, __m256i b)
{
    __m256i res = _mm256_setzero_si256();
    uint64_t ptr_a[4], ptr_b[4], ptr_r[4];
    _mm256_convert_to_uint64(ptr_a, a);
    _mm256_convert_to_uint64(ptr_b, b);
    ptr_r[0] = ptr_a[0] / ptr_b[0];
    ptr_r[1] = ptr_a[1] / ptr_b[1];
    ptr_r[2] = ptr_a[2] / ptr_b[2];
    ptr_r[3] = ptr_a[3] / ptr_b[3];
    res.vect_u64[0] = vsetq_lane_u64(ptr_r[0], res.vect_u64[0], 0);
    res.vect_u64[0] = vsetq_lane_u64(ptr_r[1], res.vect_u64[0], 1);
    res.vect_u64[1] = vsetq_lane_u64(ptr_r[2], res.vect_u64[1], 0);
    res.vect_u64[1] = vsetq_lane_u64(ptr_r[3], res.vect_u64[1], 1);
    return res;
}

FORCE_INLINE __m256 _mm256_div_ps(__m256 a, __m256 b)
{
    __m256 res_m256;
    res_m256.vect_f32[0] = vdivq_f32(a.vect_f32[0], b.vect_f32[0]);
    res_m256.vect_f32[1] = vdivq_f32(a.vect_f32[1], b.vect_f32[1]);
    return res_m256;
}

FORCE_INLINE __m256d _mm256_div_pd(__m256d a, __m256d b)
{
    __m256d res_m256d;
    res_m256d.vect_f64[0] = vdivq_f64(a.vect_f64[0], b.vect_f64[0]);
    res_m256d.vect_f64[1] = vdivq_f64(a.vect_f64[1], b.vect_f64[1]);
    return res_m256d;
}

FORCE_INLINE __m256i _mm256_add_epi8(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s8[0] = vaddq_s8(a.vect_s8[0], b.vect_s8[0]);
    res_m256i.vect_s8[1] = vaddq_s8(a.vect_s8[1], b.vect_s8[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_add_epi16(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s16[0] = vaddq_s16(a.vect_s16[0], b.vect_s16[0]);
    res_m256i.vect_s16[1] = vaddq_s16(a.vect_s16[1], b.vect_s16[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_add_epi32(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s32[0] = vaddq_s32(a.vect_s32[0], b.vect_s32[0]);
    res_m256i.vect_s32[1] = vaddq_s32(a.vect_s32[1], b.vect_s32[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_add_epi64(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s64[0] = vaddq_s64(a.vect_s64[0], b.vect_s64[0]);
    res_m256i.vect_s64[1] = vaddq_s64(a.vect_s64[1], b.vect_s64[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_adds_epi8(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s8[0] = vqaddq_s8(a.vect_s8[0], b.vect_s8[0]);
    res_m256i.vect_s8[1] = vqaddq_s8(a.vect_s8[1], b.vect_s8[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_adds_epi16(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s16[0] = vqaddq_s16(a.vect_s16[0], b.vect_s16[0]);
    res_m256i.vect_s16[1] = vqaddq_s16(a.vect_s16[1], b.vect_s16[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_adds_epu8(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_u8[0] = vqaddq_u8(a.vect_u8[0], b.vect_u8[0]);
    res_m256i.vect_u8[1] = vqaddq_u8(a.vect_u8[1], b.vect_u8[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_adds_epu16(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_u16[0] = vqaddq_u16(a.vect_u16[0], b.vect_u16[0]);
    res_m256i.vect_u16[1] = vqaddq_u16(a.vect_u16[1], b.vect_u16[1]);
    return res_m256i;
}

FORCE_INLINE __m256 _mm256_add_ps(__m256 a, __m256 b)
{
    __m256 res_m256;
    res_m256.vect_f32[0] = vaddq_f32(a.vect_f32[0], b.vect_f32[0]);
    res_m256.vect_f32[1] = vaddq_f32(a.vect_f32[1], b.vect_f32[1]);
    return res_m256;
}

FORCE_INLINE __m256d _mm256_add_pd(__m256d a, __m256d b)
{
    __m256d res_m256d;
    res_m256d.vect_f64[0] = vaddq_f64(a.vect_f64[0], b.vect_f64[0]);
    res_m256d.vect_f64[1] = vaddq_f64(a.vect_f64[1], b.vect_f64[1]);
    return res_m256d;
}

FORCE_INLINE __m256 _mm256_addsub_ps (__m256 a, __m256 b)
{
    __m256 c = _mm256_setzero_ps();
    __asm__ __volatile__ (
        "fsub %2.4s, %0.4s, %4.4s        \n\t"
        "fsub %3.4s, %1.4s, %5.4s        \n\t"
        "fadd %0.4s, %0.4s, %4.4s        \n\t"
        "fadd %1.4s, %1.4s, %5.4s        \n\t"
        "mov %2.s[1], %0.s[1]           \n\t"
        "mov %3.s[1], %1.s[1]           \n\t"
        "mov %2.s[3], %0.s[3]           \n\t"
        "mov %3.s[3], %1.s[3]           \n\t"
        :"+w"(a.vect_f32[0]), "+w"(a.vect_f32[1]), "+w"(c.vect_f32[0]), "+w"(c.vect_f32[1])
        :"w"(b.vect_f32[0]), "w"(b.vect_f32[1])
    );
    return c;
}
FORCE_INLINE __m256d _mm256_addsub_pd_bug (__m256d a, __m256d b)
{
    __m256d c = _mm256_setzero_pd();
    __asm__ __volatile__ (
        "fsub %2.2d, %0.2d, %4.2d        \n\t"
        "fsub %3.2d, %1.2d, %5.2d        \n\t"
        "fadd %0.2d, %0.2d, %4.2d        \n\t"
        "fadd %1.2d, %1.2d, %5.2d        \n\t"
        "mov %2.d[1], %0.d[1]           \n\t"
        "mov %3.d[1], %1.d[1]           \n\t"
        :"+w"(a.vect_f64[0]), "+w"(a.vect_f64[1]), "+w"(c.vect_f64[0]), "+w"(c.vect_f64[1])
        :"w"(b.vect_f64[0]), "w"(b.vect_f64[1])
    );
    return c;
}

FORCE_INLINE __m256i _mm256_sub_epi16 (__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s16[0] = vsubq_s16(a.vect_s16[0], b.vect_s16[0]);
    res_m256i.vect_s16[1] = vsubq_s16(a.vect_s16[1], b.vect_s16[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_sub_epi32(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s32[0] = vsubq_s32(a.vect_s32[0], b.vect_s32[0]);
    res_m256i.vect_s32[1] = vsubq_s32(a.vect_s32[1], b.vect_s32[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_sub_epi64(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s64[0] = vsubq_s64(a.vect_s64[0], b.vect_s64[0]);
    res_m256i.vect_s64[1] = vsubq_s64(a.vect_s64[1], b.vect_s64[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_sub_epi8(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s8[0] = vsubq_s8(a.vect_s8[0], b.vect_s8[0]);
    res_m256i.vect_s8[1] = vsubq_s8(a.vect_s8[1], b.vect_s8[1]);
    return res_m256i;
}

FORCE_INLINE __m256d _mm256_sub_pd(__m256d a, __m256d b)
{
    __m256d res_m256d;
    res_m256d.vect_f64[0] = vsubq_f64(a.vect_f64[0], b.vect_f64[0]);
    res_m256d.vect_f64[1] = vsubq_f64(a.vect_f64[1], b.vect_f64[1]);
    return res_m256d;
}

FORCE_INLINE __m256 _mm256_sub_ps(__m256 a, __m256 b)
{
    __m256 res_m256;
    res_m256.vect_f32[0] = vsubq_f32(a.vect_f32[0], b.vect_f32[0]);
    res_m256.vect_f32[1] = vsubq_f32(a.vect_f32[1], b.vect_f32[1]);
    return res_m256;
}

FORCE_INLINE __m256i _mm256_subs_epi16(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s16[0] = vqsubq_s16(a.vect_s16[0], b.vect_s16[0]);
    res_m256i.vect_s16[1] = vqsubq_s16(a.vect_s16[1], b.vect_s16[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_subs_epi8(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s8[0] = vqsubq_s8(a.vect_s8[0], b.vect_s8[0]);
    res_m256i.vect_s8[1] = vqsubq_s8(a.vect_s8[1], b.vect_s8[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_subs_epu16(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_u16[0] = vqsubq_u16(a.vect_u16[0], b.vect_u16[0]);
    res_m256i.vect_u16[1] = vqsubq_u16(a.vect_u16[1], b.vect_u16[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_subs_epu8(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_u8[0] = vqsubq_u8(a.vect_u8[0], b.vect_u8[0]);
    res_m256i.vect_u8[1] = vqsubq_u8(a.vect_u8[1], b.vect_u8[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_mul_epi32_depre(__m256i a, __m256i b)
{
    __asm__ __volatile__ (
        "ins %[a0].s[1], %[a0].s[2]             \n\t"
        "ins %[a1].s[1], %[a1].s[2]             \n\t"
        "ins %[b0].s[1], %[b0].s[2]             \n\t"
        "ins %[b1].s[1], %[b1].s[2]             \n\t"
        "smull %[a0].2d, %[a0].2s, %[b0].2s     \n\t"
        "smull %[a1].2d, %[a1].2s, %[b1].2s     \n\t"
        :[a0]"+w"(a.vect_s32[0]), [a1]"+w"(a.vect_s32[1]), [b0]"+w"(b.vect_s32[0]), [b1]"+w"(b.vect_s32[1])
        :
        :
    );
    return a;
}

FORCE_INLINE __m256i _mm256_mul_epu32_depre(__m256i a, __m256i b)
{
    __asm__ __volatile__ (
        "ins %[a0].s[1], %[a0].s[2]             \n\t"
        "ins %[a1].s[1], %[a1].s[2]             \n\t"
        "ins %[b0].s[1], %[b0].s[2]             \n\t"
        "ins %[b1].s[1], %[b1].s[2]             \n\t"
        "umull %[a0].2d, %[a0].2s, %[b0].2s     \n\t"
        "umull %[a1].2d, %[a1].2s, %[b1].2s     \n\t"
        :[a0]"+w"(a.vect_u32[0]), [a1]"+w"(a.vect_u32[1]), [b0]"+w"(b.vect_u32[0]), [b1]"+w"(b.vect_u32[1])
        :
        :
    );
    return a;
}

FORCE_INLINE __m256d _mm256_mul_pd(__m256d a, __m256d b)
{
    __m256d res_m256d;
    res_m256d.vect_f64[0] = vmulq_f64(a.vect_f64[0], b.vect_f64[0]);
    res_m256d.vect_f64[1] = vmulq_f64(a.vect_f64[1], b.vect_f64[1]);
    return res_m256d;
}

FORCE_INLINE __m256 _mm256_mul_ps(__m256 a, __m256 b)
{
    __m256 res_m256;
    res_m256.vect_f32[0] = vmulq_f32(a.vect_f32[0], b.vect_f32[0]);
    res_m256.vect_f32[1] = vmulq_f32(a.vect_f32[1], b.vect_f32[1]);
    return res_m256;
}

FORCE_INLINE __m256i _mm256_mulhi_epi16(__m256i a, __m256i b)
{
    __asm__ __volatile__ (
        "mov v4.d[0], %[a0].d[1]                    \n\t"
        "mov v5.d[0], %[a1].d[1]                    \n\t"
        "smull %[a0].4s, %[a0].4h, %[b0].4h         \n\t"
        "mov v6.d[0], %[b0].d[1]                    \n\t"
        "mov v7.d[0], %[b1].d[1]                    \n\t"
        "smull %[b0].4s, v4.4h, v6.4h               \n\t"
        "uzp2  %[a0].8h, %[a0].8h, %[b0].8h         \n\t"
        "smull %[a1].4s, %[a1].4h, %[b1].4h         \n\t"
        "smull %[b1].4s, v5.4h, v7.4h               \n\t"
        "uzp2  %[a1].8h, %[a1].8h, %[b1].8h         \n\t"
        :[a0]"+w"(a.vect_s16[0]), [a1]"+w"(a.vect_s16[1]), [b0]"+w"(b.vect_s16[0]), [b1]"+w"(b.vect_s16[1])
        :
        :"v4", "v5", "v6", "v7"
    );
    return a;
}

FORCE_INLINE __m256i _mm256_mulhi_epu16(__m256i a, __m256i b)
{
    __asm__ __volatile__ (
        "mov v4.d[0], %[a0].d[1]                    \n\t"
        "mov v5.d[0], %[a1].d[1]                    \n\t"
        "umull %[a0].4s, %[a0].4h, %[b0].4h         \n\t"
        "mov v6.d[0], %[b0].d[1]                    \n\t"
        "mov v7.d[0], %[b1].d[1]                    \n\t"
        "umull %[b0].4s, v4.4h, v6.4h               \n\t"
        "uzp2  %[a0].8h, %[a0].8h, %[b0].8h         \n\t"
        "umull %[a1].4s, %[a1].4h, %[b1].4h         \n\t"
        "umull %[b1].4s, v5.4h, v7.4h               \n\t"
        "uzp2  %[a1].8h, %[a1].8h, %[b1].8h         \n\t"
        :[a0]"+w"(a.vect_u16[0]), [a1]"+w"(a.vect_u16[1]), [b0]"+w"(b.vect_u16[0]), [b1]"+w"(b.vect_u16[1])
        :
        :"v4", "v5", "v6", "v7"
    );
    return a;
}

FORCE_INLINE __m256i _mm256_mulhi_epi32(__m256i a, __m256i b)
{
    __asm__ __volatile__ (
        "mov v4.d[0], %[a0].d[1]                    \n\t"
        "mov v5.d[0], %[a1].d[1]                    \n\t"
        "smull %[a0].2d, %[a0].2s, %[b0].2s         \n\t"
        "mov v6.d[0], %[b0].d[1]                    \n\t"
        "mov v7.d[0], %[b1].d[1]                    \n\t"
        "smull %[b0].2d, v4.2s, v6.2s               \n\t"
        "uzp2  %[a0].4s, %[a0].4s, %[b0].4s         \n\t"
        "smull %[a1].2d, %[a1].2s, %[b1].2s         \n\t"
        "smull %[b1].2d, v5.2s, v7.2s               \n\t"
        "uzp2  %[a1].4s, %[a1].4s, %[b1].4s         \n\t"
        :[a0]"+w"(a.vect_s32[0]), [a1]"+w"(a.vect_s32[1]), [b0]"+w"(b.vect_s32[0]), [b1]"+w"(b.vect_s32[1])
        :
        :"v4", "v5", "v6", "v7"
    );
    return a;
}

FORCE_INLINE __m256i _mm256_mulhi_epu32(__m256i a, __m256i b)
{
    __asm__ __volatile__ (
        "mov v4.d[0], %[a0].d[1]                    \n\t"
        "mov v5.d[0], %[a1].d[1]                    \n\t"
        "umull %[a0].2d, %[a0].2s, %[b0].2s         \n\t"
        "mov v6.d[0], %[b0].d[1]                    \n\t"
        "mov v7.d[0], %[b1].d[1]                    \n\t"
        "umull %[b0].2d, v4.2s, v6.2s               \n\t"
        "uzp2  %[a0].4s, %[a0].4s, %[b0].4s         \n\t"
        "umull %[a1].2d, %[a1].2s, %[b1].2s         \n\t"
        "umull %[b1].2d, v5.2s, v7.2s               \n\t"
        "uzp2  %[a1].4s, %[a1].4s, %[b1].4s         \n\t"
        :[a0]"+w"(a.vect_s32[0]), [a1]"+w"(a.vect_s32[1]), [b0]"+w"(b.vect_s32[0]), [b1]"+w"(b.vect_s32[1])
        :
        :"v4", "v5", "v6", "v7"
    );
    return a;
}

FORCE_INLINE __m256i _mm256_mullo_epi16(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s16[0] = vmulq_s16(a.vect_s16[0], b.vect_s16[0]);
    res_m256i.vect_s16[1] = vmulq_s16(a.vect_s16[1], b.vect_s16[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_mullo_epi32(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s32[0] = vmulq_s32(a.vect_s32[0], b.vect_s32[0]);
    res_m256i.vect_s32[1] = vmulq_s32(a.vect_s32[1], b.vect_s32[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_mullo_epi64(__m256i a, __m256i b)
{
    __m256i res = _mm256_setzero_si256();
    int64_t ptr_a[4], ptr_b[4], ptr_r[4];
    _mm256_convert_to_int64(ptr_a, a);
    _mm256_convert_to_int64(ptr_b, b);
    ptr_r[0] = ptr_a[0] * ptr_b[0];
    ptr_r[1] = ptr_a[1] * ptr_b[1];
    ptr_r[2] = ptr_a[2] * ptr_b[2];
    ptr_r[3] = ptr_a[3] * ptr_b[3];
    res.vect_s64[0] = vsetq_lane_s64(ptr_r[0], res.vect_s64[0], 0);
    res.vect_s64[0] = vsetq_lane_s64(ptr_r[1], res.vect_s64[0], 1);
    res.vect_s64[1] = vsetq_lane_s64(ptr_r[2], res.vect_s64[1], 0);
    res.vect_s64[1] = vsetq_lane_s64(ptr_r[3], res.vect_s64[1], 1);
    return res;
}

FORCE_INLINE __m256i _mm256_mulhrs_epi16(__m256i a, __m256i b)
{
    __m256i res;
    int32x4_t r_0 = vmull_s16(vget_low_s16(a.vect_s16[0]), vget_low_s16(b.vect_s16[0]));
    int32x4_t r_1 = vmull_s16(vget_low_s16(a.vect_s16[1]), vget_low_s16(b.vect_s16[1]));
    int32x4_t r_2 = vmull_s16(vget_high_s16(a.vect_s16[0]), vget_high_s16(b.vect_s16[0]));
    int32x4_t r_3 = vmull_s16(vget_high_s16(a.vect_s16[1]), vget_high_s16(b.vect_s16[1]));
    
    int32x4_t inc = vdupq_n_s32(0x00004000);
    r_0 = vshrq_n_s32(vaddq_s32(r_0, inc), 15);
    r_1 = vshrq_n_s32(vaddq_s32(r_1, inc), 15);
    r_2 = vshrq_n_s32(vaddq_s32(r_2, inc), 15);
    r_3 = vshrq_n_s32(vaddq_s32(r_3, inc), 15);
    res.vect_s16[0] = vuzp1q_s16(vreinterpretq_s16_s32(r_0), vreinterpretq_s16_s32(r_2));
    res.vect_s16[1] = vuzp1q_s16(vreinterpretq_s16_s32(r_1), vreinterpretq_s16_s32(r_3));
    return res;
}

FORCE_INLINE void _mm256_zeroupper(void)
{
    return;  //
}

// FORCE_INLINE __m256i _mm256_sll_epi32(__m256i a, __m128i count)
#define _mm256_sll_epi32( a, count) \
({ \
    long long c = count.vect_s64[0]; \
    __m256i result_m256i; \
    if (likely(c >= 0 && c < 32)) { \
        result_m256i.vect_s32[0] = vshlq_n_s32(a.vect_s32[0], c); \
        result_m256i.vect_s32[1] = vshlq_n_s32(a.vect_s32[1], c); \
    } else { \
        result_m256i.vect_s32[0] = vdupq_n_s32(0); \
        result_m256i.vect_s32[1] = vdupq_n_s32(0); \
    }  \
    result_m256i; \
})

// FORCE_INLINE __m256i _mm256_sll_epi64(__m256i a, __m128i count)
#define _mm256_sll_epi64( a,  count) \
({ \
    long long c = count.vect_s64[0]; \
    __m256i result_m256i; \
    if (likely(c >= 0 && c < 64)) { \
        result_m256i.vect_s64[0] = vshlq_n_s64(a.vect_s64[0], c); \
        result_m256i.vect_s64[1] = vshlq_n_s64(a.vect_s64[1], c); \
    } else { \
        result_m256i.vect_s64[0] = vdupq_n_s64(0); \
        result_m256i.vect_s64[1] = vdupq_n_s64(0); \
    }  \
    result_m256i; \
})

// FORCE_INLINE __m256i _mm256_slli_epi16(__m256i a, int imm8)
#define _mm256_slli_epi16( a,  imm8) \
({ \
    __m256i result_m256i; \
    if (likely(imm8 >= 0 && imm8 < 16)) { \
        result_m256i.vect_s16[0] = vshlq_n_s16(a.vect_s16[0], imm8); \
        result_m256i.vect_s16[1] = vshlq_n_s16(a.vect_s16[1], imm8); \
    } else { \
        result_m256i.vect_s16[0] = vdupq_n_s16(0); \
        result_m256i.vect_s16[1] = vdupq_n_s16(0); \
    }  \
    result_m256i; \
})

// FORCE_INLINE __m256i _mm256_slli_epi32(__m256i a, int imm8)
#define _mm256_slli_epi32( a,  imm8) \
({ \
    __m256i result_m256i; \
    if (likely(imm8 >= 0 && imm8 < 32)) { \
        result_m256i.vect_s32[0] = vshlq_n_s32(a.vect_s32[0], imm8); \
        result_m256i.vect_s32[1] = vshlq_n_s32(a.vect_s32[1], imm8); \
    } else { \
        result_m256i.vect_s32[0] = vdupq_n_s32(0); \
        result_m256i.vect_s32[1] = vdupq_n_s32(0); \
    }  \
    result_m256i; \
})

// FORCE_INLINE __m256i _mm256_slli_epi64(__m256i a, int imm8)
#define  _mm256_slli_epi64( a, imm8) \
({ \
    __m256i result_m256i; \
    if (likely(imm8 >= 0 && imm8 < 64)) { \
        result_m256i.vect_s64[0] = vshlq_n_s64(a.vect_s64[0], imm8); \
        result_m256i.vect_s64[1] = vshlq_n_s64(a.vect_s64[1], imm8); \
    } else { \
        result_m256i.vect_s64[0] = vdupq_n_s64(0); \
        result_m256i.vect_s64[1] = vdupq_n_s64(0); \
    }  \
    result_m256i; \
})

// FORCE_INLINE __m256i _mm256_srli_epi16(__m256i a, int imm8)
#define _mm256_srli_epi16( a,  imm8) \
({ \
    __m256i result_m256i; \
     \
    if (likely(imm8 >= 0 && imm8 < 16)) { \
        int16x8_t vect_imm = vdupq_n_s16(-imm8); \
        result_m256i.vect_u16[0] = vshlq_u16(a.vect_u16[0], vect_imm); \
        result_m256i.vect_u16[1] = vshlq_u16(a.vect_u16[1], vect_imm); \
    } else { \
        result_m256i.vect_u16[0] = vdupq_n_u16(0); \
        result_m256i.vect_u16[1] = vdupq_n_u16(0); \
    }  \
    result_m256i; \
})

// FORCE_INLINE __m256i _mm256_srli_epi32(__m256i a, int imm8)
#define _mm256_srli_epi32( a,  imm8) \
({ \
    __m256i result_m256i; \
     \
    if (likely(imm8 >= 0 && imm8 < 32)) { \
        int32x4_t vect_imm = vdupq_n_s32(-imm8); \
        result_m256i.vect_u32[0] = vshlq_u32(a.vect_u32[0], vect_imm); \
        result_m256i.vect_u32[1] = vshlq_u32(a.vect_u32[1], vect_imm); \
    } else { \
        result_m256i.vect_u32[0] = vdupq_n_u32(0); \
        result_m256i.vect_u32[1] = vdupq_n_u32(0); \
    }  \
    result_m256i; \
})

// FORCE_INLINE __m256i _mm256_srli_epi64(__m256i a, int imm8)
#define _mm256_srli_epi64( a,  imm8) \
({ \
    __m256i result_m256i; \
     \
    if (likely(imm8 >= 0 && imm8 < 64)) { \
        int64x2_t vect_imm = vdupq_n_s64(-imm8); \
        result_m256i.vect_u64[0] = vshlq_u64(a.vect_u64[0], vect_imm); \
        result_m256i.vect_u64[1] = vshlq_u64(a.vect_u64[1], vect_imm); \
    } else { \
        result_m256i.vect_u64[0] = vdupq_n_u64(0); \
        result_m256i.vect_u64[1] = vdupq_n_u64(0); \
    }  \
    result_m256i; \
})

// FORCE_INLINE __m256i _mm256_slli_si256(__m256i a, const int imm8)
#define _mm256_slli_si256( a, imm8) \
({ \
    assert(imm8 >=0 && imm8 <256); \
    __m256i result_m256i; \
    if (likely(imm8 > 0 && imm8 <= 15)) { \
        result_m256i.vect_s8[0] = vextq_s8(vdupq_n_s8(0), a.vect_s8[0], 16 - imm8); \
        result_m256i.vect_s8[1] = vextq_s8(vdupq_n_s8(0), a.vect_s8[1], 16 - imm8); \
    } else if (imm8 == 0) { \
        result_m256i = a; \
    } else { \
        result_m256i.vect_s8[0] = vdupq_n_s8(0); \
        result_m256i.vect_s8[1] = vdupq_n_s8(0); \
    } \
    result_m256i; \
})

// FORCE_INLINE __m256i _mm256_srli_si256(__m256i a, const int imm8)
#define _mm256_srli_si256( a,  imm8) \
({ \
    assert(imm8 >=0 && imm8 <256); \
    __m256i result_m256i; \
    if (likely(imm8 > 0 && imm8 <= 15)) { \
        result_m256i.vect_s8[0] = vextq_s8(a.vect_s8[0], vdupq_n_s8(0), imm8); \
        result_m256i.vect_s8[1] = vextq_s8(a.vect_s8[1], vdupq_n_s8(0), imm8); \
    } else if (imm8 == 0) { \
        result_m256i = a; \
    } else { \
        result_m256i.vect_s8[0] = vdupq_n_s8(0); \
        result_m256i.vect_s8[1] = vdupq_n_s8(0); \
    }  \
    result_m256i; \
})

FORCE_INLINE __m256i _mm256_unpackhi_epi8(__m256i a, __m256i b)
{
    __m256i result_m256i;
    result_m256i.vect_s8[0] = vzip2q_s8(a.vect_s8[0], b.vect_s8[0]);
    result_m256i.vect_s8[1] = vzip2q_s8(a.vect_s8[1], b.vect_s8[1]);
    return result_m256i;
}

FORCE_INLINE __m256i _mm256_unpacklo_epi8(__m256i a, __m256i b)
{
    __m256i result_m256i;
    result_m256i.vect_s8[0] = vzip1q_s8(a.vect_s8[0], b.vect_s8[0]);
    result_m256i.vect_s8[1] = vzip1q_s8(a.vect_s8[1], b.vect_s8[1]);
    return result_m256i;
}

FORCE_INLINE __m256i _mm256_and_si256(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s32[0] = vandq_s32(a.vect_s32[0], b.vect_s32[0]);
    res_m256i.vect_s32[1] = vandq_s32(a.vect_s32[1], b.vect_s32[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_andnot_si256(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s32[0] = vbicq_s32(b.vect_s32[0], a.vect_s32[0]);
    res_m256i.vect_s32[1] = vbicq_s32(b.vect_s32[1], a.vect_s32[1]);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_xor_si256(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s32[0] = veorq_s32(a.vect_s32[0], b.vect_s32[0]);
    res_m256i.vect_s32[1] = veorq_s32(a.vect_s32[1], b.vect_s32[1]);
    return res_m256i;
}

FORCE_INLINE __m256 _mm256_or_ps (__m256 a, __m256 b)
{
    __asm__ __volatile(
        "orr %0.16b, %0.16b, %2.16b     \n\t"
        "orr %1.16b, %1.16b, %3.16b     \n\t"
        :"+w"(a.vect_f32[0]), "+w"(a.vect_f32[1])
        :"w"(b.vect_f32[0]), "w"(b.vect_f32[1])
    );
    return a;
}

FORCE_INLINE __m256d _mm256_or_pd (__m256d a, __m256d b)
{
    __asm__ __volatile(
        "orr %0.16b, %0.16b, %2.16b     \n\t"
        "orr %1.16b, %1.16b, %3.16b     \n\t"
        :"+w"(a.vect_f64[0]), "+w"(a.vect_f64[1])
        :"w"(b.vect_f64[0]), "w"(b.vect_f64[1])
    );
    return a;
}

FORCE_INLINE int _mm256_movemask_epi8_bug (__m256i a)
{
    int res;
    __asm__ __volatile__ (
        "ushr %[a0].16b, %[a0].16b, #7          \n\t"
        "ushr %[a1].16b, %[a1].16b, #7          \n\t"
        "usra %[a0].8h, %[a0].8h, #7            \n\t"
        "usra %[a1].8h, %[a1].8h, #7            \n\t"
        "usra %[a0].4s, %[a0].4s, #14           \n\t"
        "usra %[a1].4s, %[a1].4s, #14           \n\t"
        "usra %[a0].2d, %[a0].2d, #28           \n\t"
        "usra %[a1].2d, %[a1].2d, #28           \n\t"
        "ins %[a0].b[1], %[a0].b[8]             \n\t"
        "ins %[a0].b[2], %[a1].b[0]             \n\t"
        "ins %[a0].b[3], %[a1].b[8]             \n\t"
        "umov %w[r], %[a0].s[0]"
        :[r]"=r"(res), [a0]"+w"(a.vect_u8[0]), [a1]"+w"(a.vect_u8[1])
        :
        :
    );
    return res;
}

FORCE_INLINE int _mm256_movemask_ps(__m256 a)
{
    __m256i res_m256i;
    res_m256i.vect_u32[0] = vshrq_n_u32(vreinterpretq_u32_f32(a.vect_f32[0]), 31);
    res_m256i.vect_u32[1] = vshrq_n_u32(vreinterpretq_u32_f32(a.vect_f32[1]), 31);
    res_m256i.vect_u64[0] = vsraq_n_u64(res_m256i.vect_u64[0], res_m256i.vect_u64[0], 31);
    res_m256i.vect_u64[1] = vsraq_n_u64(res_m256i.vect_u64[1], res_m256i.vect_u64[1], 31);
    return (int)(vgetq_lane_u8(res_m256i.vect_u8[0], 0) | (vgetq_lane_u8(res_m256i.vect_u8[0], 8) << 2) |
                (vgetq_lane_u8(res_m256i.vect_u8[1], 0) << 4) | (vgetq_lane_u8(res_m256i.vect_u8[1], 8) << 6));
}

FORCE_INLINE uint64x2_t vmvnq_u64(uint64x2_t a) {
    return vreinterpretq_u64_u32(vmvnq_u32(vreinterpretq_u32_u64(a)));
}

FORCE_INLINE int64x2_t vmvnq_s64(int64x2_t a) {
    return vreinterpretq_s64_s32(vmvnq_s32(vreinterpretq_s32_s64(a)));
}

FORCE_INLINE int _mm_testc_pd(__m128d a, __m128d b)
{
    __m128i res_m128i;
    res_m128i = vandq_s64(vmvnq_s64(vreinterpretq_s64_f64(a)), vreinterpretq_s64_f64(b));
    int64x2_t tmp = vandq_s64(res_m128i, _mm_set_epi64x(1LL<<63,1LL<<63));
    return !(vgetq_lane_s64(tmp, 0) | vgetq_lane_s64(tmp, 1));
}

FORCE_INLINE int _mm_testz_pd(__m128d a, __m128d b)
{
    __m128i res_m128i;
    res_m128i = vandq_s64(vreinterpretq_s64_f64(a), vreinterpretq_s64_f64(b));
    int64x2_t tmp = vandq_s64(res_m128i, _mm_set_epi64x(1LL<<63,1LL<<63));
    return !(vgetq_lane_s64(tmp, 0) | vgetq_lane_s64(tmp, 1));
}

FORCE_INLINE int _mm256_testc_pd(__m256d a, __m256d b)
{
    __m256i res_m256i;
    res_m256i.vect_s64[0] = vandq_s64(vmvnq_s64(vreinterpretq_s64_f64(a.vect_f64[0])), vreinterpretq_s64_f64(b.vect_f64[0]));
    res_m256i.vect_s64[1] = vandq_s64(vmvnq_s64(vreinterpretq_s64_f64(a.vect_f64[1])), vreinterpretq_s64_f64(b.vect_f64[1]));
    int64x2_t tmp = vandq_s64(vorrq_s64(res_m256i.vect_s64[0], res_m256i.vect_s64[1]), _mm_set_epi64x(1LL<<63,1LL<<63));
    return !(vgetq_lane_s64(tmp, 0) | vgetq_lane_s64(tmp, 1));
}

FORCE_INLINE int _mm256_testc_si256(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s64[0] = vandq_s64(vmvnq_s64(a.vect_s64[0]), b.vect_s64[0]);
    res_m256i.vect_s64[1] = vandq_s64(vmvnq_s64(a.vect_s64[1]), b.vect_s64[1]);
    int64x2_t tmp = vorrq_s64(res_m256i.vect_s64[0], res_m256i.vect_s64[1]);
    return !(vgetq_lane_s64(tmp, 0) | vgetq_lane_s64(tmp, 1));
}

FORCE_INLINE int _mm256_testz_pd(__m256d a, __m256d b)
{
    __m256i res_m256i;
    res_m256i.vect_s64[0] = vandq_s64(vreinterpretq_s64_f64(a.vect_f64[0]), vreinterpretq_s64_f64(b.vect_f64[0]));
    res_m256i.vect_s64[1] = vandq_s64(vreinterpretq_s64_f64(a.vect_f64[1]), vreinterpretq_s64_f64(b.vect_f64[1]));
    int64x2_t tmp = vandq_s64(vorrq_s64(res_m256i.vect_s64[0], res_m256i.vect_s64[1]), _mm_set_epi64x(1LL<<63,1LL<<63));
    return !(vgetq_lane_s64(tmp, 0) | vgetq_lane_s64(tmp, 1));
}

FORCE_INLINE int _mm256_testz_si256(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s64[0] = vandq_s64(a.vect_s64[0], b.vect_s64[0]);
    res_m256i.vect_s64[1] = vandq_s64(a.vect_s64[1], b.vect_s64[1]);
    int64x2_t tmp = vorrq_s64(res_m256i.vect_s64[0], res_m256i.vect_s64[1]);
    return !(vgetq_lane_s64(tmp, 0) | vgetq_lane_s64(tmp, 1));
}

FORCE_INLINE __m256i _mm256_or_si256(__m256i a, __m256i b)
{
    __m256i res_m256i;
    res_m256i.vect_s32[0] = vorrq_s32(a.vect_s32[0], b.vect_s32[0]);
    res_m256i.vect_s32[1] = vorrq_s32(a.vect_s32[1], b.vect_s32[1]);
    return res_m256i;
}

FORCE_INLINE __m128i _mm256_extracti128_si256(__m256i a, const int imm8)
{
    assert(imm8 >= 0 && imm8 <= 1);
    __m128I res_m128i;
    res_m128i.vect_s64 = a.vect_s64[imm8];
    return res_m128i.vect_s64;
}

FORCE_INLINE int32_t _mm256_extract_epi32 (__m256i a, const int index)
{
    assert(index >= 0 && index <= 7);
    return a.vect_s32[!!(index & 0x04)][index & 0x03];
}

FORCE_INLINE int64_t _mm256_extract_epi64 (__m256i a, const int index)
{
    assert(index >= 0 && index <= 3);
    return a.vect_s64[!!(index & 0x02)][index & 0x01];
}

FORCE_INLINE __m128 _mm256_extractf128_ps (__m256 a, const int imm8)
{
    assert(imm8 >= 0 && imm8 <= 1);
    return a.vect_f32[imm8];
}

FORCE_INLINE __m128d _mm256_extractf128_pd (__m256d a, const int imm8)
{
    assert(imm8 >= 0 && imm8 <= 1);
    return a.vect_f64[imm8];
}

#define _mm256_permute_ps(a, imm8)  \
({ \
    __m256 res;  \
    res.vect_f32[0] = _mm_castsi128_ps( _mm_shuffle_epi32( *(__m128i*)&(a.vect_f32[0]), imm8 ) ); \
    res.vect_f32[1] = _mm_castsi128_ps( _mm_shuffle_epi32( *(__m128i*)&(a.vect_f32[1]), imm8 ) ); \
    res; \
})

FORCE_INLINE __m256i _mm256_permute2f128_si256 (__m256i a, __m256i b, int imm8)
{
    __m256i res;
    int bit_0 = imm8 & 0x1, bit_1 = imm8 & 0x2, bit_3 = imm8 & 0x8;
    if (bit_1 == 0) {
        res.vect_s32[0] = a.vect_s32[bit_0];
    } else {
        res.vect_s32[0] = b.vect_s32[bit_0];
    }
    if (bit_3) {
        res.vect_s32[0] = vdupq_n_s32(0);
    }
    bit_0 = (imm8 & 0x10) >> 4, bit_1 = imm8 & 0x20, bit_3 = imm8 & 0x80;
    if (bit_1 == 0) {
        res.vect_s32[1] = a.vect_s32[bit_0];
    } else {
        res.vect_s32[1] = b.vect_s32[bit_0];
    }
    if (bit_3) {
        res.vect_s32[1] = vdupq_n_s32(0);
    }
    return res;
}

FORCE_INLINE __m256 _mm256_permute2f128_ps (__m256 a, __m256 b, int imm8)
{
    __m256 res;
    int bit_0 = imm8 & 0x1, bit_1 = imm8 & 0x2, bit_3 = imm8 & 0x8;
    if (bit_1 == 0) {
        res.vect_f32[0] = a.vect_f32[bit_0];
    } else {
        res.vect_f32[0] = b.vect_f32[bit_0];
    }
    if (bit_3) {
        res.vect_f32[0] = vdupq_n_f32(0);
    }
    bit_0 = (imm8 & 0x10) >> 4, bit_1 = imm8 & 0x20, bit_3 = imm8 & 0x80;
    if (bit_1 == 0) {
        res.vect_f32[1] = a.vect_f32[bit_0];
    } else {
        res.vect_f32[1] = b.vect_f32[bit_0];
    }
    if (bit_3) {
        res.vect_f32[1] = vdupq_n_f32(0);
    }
    return res;
}

FORCE_INLINE __m256i _mm256_permute4x64_epi64(__m256i a, const int imm8)
{
    __m256i res = _mm256_setzero_si256();
    int64_t ptr_a[4];
    vst1q_s64(ptr_a, a.vect_s64[0]);
    vst1q_s64(ptr_a + 2, a.vect_s64[1]);
    const int id0 = imm8 & 0x03;
    const int id1 = (imm8 >> 2) & 0x03;
    const int id2 = (imm8 >> 4) & 0x03;
    const int id3 = (imm8 >> 6) & 0x03;
    res.vect_s64[0] = vsetq_lane_s64(ptr_a[id0], res.vect_s64[0], 0);
    res.vect_s64[0] = vsetq_lane_s64(ptr_a[id1], res.vect_s64[0], 1);
    res.vect_s64[1] = vsetq_lane_s64(ptr_a[id2], res.vect_s64[1], 0);
    res.vect_s64[1] = vsetq_lane_s64(ptr_a[id3], res.vect_s64[1], 1);
    return res;
}

FORCE_INLINE __m256d _mm256_permute4x64_pd(__m256d a, const int control)
{
    __m256d A;
    double* ptr_a = (double*) &a;
    const int id0 = control & 0x03;
    const int id1 = (control >> 2) & 0x03;
    const int id2 = (control >> 4) & 0x03;
    const int id3 = (control >> 6) & 0x03;
    (A.vect_f64[0])[0] = ptr_a[id0];
    (A.vect_f64[0])[1] = ptr_a[id1];
    (A.vect_f64[1])[0] = ptr_a[id2];
    (A.vect_f64[1])[1] = ptr_a[id3];
    return A;
}

FORCE_INLINE __m256i _mm256_set_epi32(int e7, int e6, int e5, int e4, int e3, int e2, int e1, int e0)
{
    __m256i res_m256i;
    SET32x4(res_m256i.vect_s32[0], e0, e1, e2, e3);
    SET32x4(res_m256i.vect_s32[1], e4, e5, e6, e7);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_set_epi64x(int64_t e3, int64_t e2, int64_t e1, int64_t e0)
{
    __m256i res_m256i;
    SET64x2(res_m256i.vect_s64[0], e0, e1);
    SET64x2(res_m256i.vect_s64[1], e2, e3);
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_set_m128i(__m128i hi, __m128i lo)
{
    __m256i res_m256i;
    res_m256i.vect_i128.val[0] = lo;
    res_m256i.vect_i128.val[1] = hi;
    return res_m256i;
}

FORCE_INLINE __m256 _mm256_set_ps(float e7, float e6, float e5, float e4, float e3, float e2, float e1, float e0)
{
    __m256 res_m256;
    SET32x4(res_m256.vect_f32[0], e0, e1, e2, e3);
    SET32x4(res_m256.vect_f32[1], e4, e5, e6, e7);
    return res_m256;
}

FORCE_INLINE __m256d _mm256_set_pd(double e3, double e2, double e1, double e0)
{
    __m256d res_m256d;
    SET64x2(res_m256d.vect_f64[0], e0, e1);
    SET64x2(res_m256d.vect_f64[1], e2, e3);
    return res_m256d;
}

FORCE_INLINE __m256i _mm256_set1_epi8(int8_t a)
{
    __m256i ret;
    ret.vect_s8[0] = ret.vect_s8[1] = vdupq_n_s8(a);
    return ret;
}

FORCE_INLINE __m256i _mm256_set1_epi16(int32_t a)
{
    __m256i res;
    res.vect_s16[0] = res.vect_s16[1] = vdupq_n_s16(a);
    return res;
}

FORCE_INLINE __m256i _mm256_set1_epi32(int32_t a)
{
    __m256i ret;
    ret.vect_s32[0] = ret.vect_s32[1] = vdupq_n_s32(a);
    return ret;
}

FORCE_INLINE __m256i _mm256_set1_epi64x(int64_t a)
{
    __m256i ret;
    ret.vect_s64[0] = ret.vect_s64[1] = vdupq_n_s64(a);
    return ret;
}

FORCE_INLINE __m256d _mm256_set1_pd(double a)
{
    __m256d ret;
    ret.vect_f64[0] = ret.vect_f64[1] = vdupq_n_f64(a);
    return ret;
}

FORCE_INLINE __m256 _mm256_set1_ps(float a)
{
    __m256 ret;
    ret.vect_f32[0] = ret.vect_f32[1] = vdupq_n_f32(a);
    return ret;
}

FORCE_INLINE void _mm256_store_si256(__m256i* mem_addr, __m256i a)
{
    vst1q_s32((int32_t*)mem_addr, a.vect_s32[0]);
    vst1q_s32((int32_t*)mem_addr + 4, a.vect_s32[1]);
}

FORCE_INLINE void _mm256_storeu_si256(__m256i* mem_addr, __m256i a)
{
    vst1q_s8((int8_t*)mem_addr, a.vect_s8[0]);
    vst1q_s8((int8_t*)mem_addr + 16, a.vect_s8[1]);
}

FORCE_INLINE void _mm256_stream_si256 (__m256i * mem_addr, __m256i a)
{
    vst1q_s32((int32_t*)mem_addr, a.vect_s32[0]);
    vst1q_s32((int32_t*)mem_addr + 4, a.vect_s32[1]);
}

FORCE_INLINE __m256i _mm256_load_si256(__m256i const* mem_addr)
{
    __m256i ret;
    ret.vect_s32[0] = vld1q_s32((int32_t const*)mem_addr);
    ret.vect_s32[1] = vld1q_s32(((int32_t const*)mem_addr) + 4);
    return ret;
}

FORCE_INLINE __m256i _mm256_loadu_si256(__m256i const* mem_addr)
{
    __m256i ret;
    ret.vect_s32[0] = vld1q_s32((int32_t const*)mem_addr);
    ret.vect_s32[1] = vld1q_s32(((int32_t const*)mem_addr) + 4);
    return ret;
}

FORCE_INLINE __m256i _mm256_maskload_epi32(int const* mem_addr, __m256i mask)
{
    __m256i ret;

    int32x4_t vecZero = vdupq_n_s32(0);
    __m128i flag;

    flag = vcltq_s32(mask.vect_s32[0], vecZero);
    ret.vect_s32[0] = vandq_s32(flag, vld1q_s32(mem_addr));

    flag = vcltq_s32(mask.vect_s32[1], vecZero);
    ret.vect_s32[1] = vandq_s32(flag, vld1q_s32(mem_addr + 4));

    return ret;
}

FORCE_INLINE __m256i _mm256_broadcastq_epi64(__m128i a)
{
    __m256i res;
    __asm__ __volatile__ (
        "dup %[r0].2d, %[a].d[0]           \n\t"
        "dup %[r1].2d, %[a].d[0]           \n\t"
        :[r0]"=w"(res.vect_i128.val[0]), [r1]"=w"(res.vect_i128.val[1])
        :[a]"w"(a)
        :
    );
    return res;
}

FORCE_INLINE __m256i _mm256_broadcastsi128_si256(__m128i a)
{
    __m256i ret;

    ret.vect_s64[0] = a;
    ret.vect_s64[1] = a;

    return ret;
}

FORCE_INLINE __m256d _mm256_castpd128_pd256(__m128d a)
{
    __m256d res;
    res.vect_f64[0] = a;
    return res;
}

FORCE_INLINE __m128d _mm256_castpd256_pd128(__m256d a)
{
    return a.vect_f64[0];
}

FORCE_INLINE __m256 _mm256_castps128_ps256(__m128 a)
{
    __m256 res;
    res.vect_f32[0] = a;
    return res;
}

FORCE_INLINE __m128 _mm256_castps256_ps128(__m256 a)
{
    return a.vect_f32[0];
}

FORCE_INLINE __m256i _mm256_castsi128_si256(__m128i a)
{
    __m256i res;
    res.vect_s32[0] = a;
    return res;
}

FORCE_INLINE __m256 _mm256_castsi256_ps(__m256i a)
{
    __m256 b;

    b.vect_f32[0] = vreinterpretq_f32_s32(a.vect_s32[0]);
    b.vect_f32[1] = vreinterpretq_f32_s32(a.vect_s32[1]);

    return b;
}

FORCE_INLINE __m128i _mm256_castsi256_si128(__m256i a)
{
    __m128i ret;
    ret = a.vect_s32[0];
    return ret;
}

#define _mm_broadcast_impl( name, res_type, type )     \
FORCE_INLINE res_type  name(type const *a) {         \
    const size_t size = sizeof( res_type ) / sizeof( type );\
    ALIGN_STRUCT(32) type res[ size ];                  \
    size_t i = 0;                                           \
    for ( ; i < size; ++i )                                 \
        res[ i ] = *a;                                      \
    return (*(res_type*)&res);                              \
}

_mm_broadcast_impl( _mm_broadcast_ss, __m128, float )
_mm_broadcast_impl( _mm256_broadcast_ss, __m256, float )

_mm_broadcast_impl( _mm_broadcast_sd, __m128d, double )
_mm_broadcast_impl( _mm256_broadcast_sd, __m256d, double )

_mm_broadcast_impl( _mm256_broadcast_ps, __m256, __m128 )
_mm_broadcast_impl( _mm256_broadcast_pd, __m256d, __m128d )

FORCE_INLINE __m256d _mm256_cvtepi32_pd(__m128i a)
{
    __m256d res;
    __asm__ __volatile__ (
        "scvtf v0.4s, %[a].4s           \n\t"
        "fcvtl %[r0].2d, v0.2s          \n\t"
        "mov v1.d[0], v0.d[1]           \n\t"
        "fcvtl %[r1].2d, v1.2s          \n\t"
        :[r0]"=w"(res.vect_f64[0]), [r1]"=w"(res.vect_f64[1])
        :[a]"w"(a)
        :"v0", "v1"
    );
    return res;
}

FORCE_INLINE __m256 _mm256_cvtepi32_ps(__m256i a)
{
    __m256 ret;
    ret.vect_f32[0] = vcvtq_f32_s32(a.vect_s32[0]);
    ret.vect_f32[1] = vcvtq_f32_s32(a.vect_s32[1]);
    return ret;
}

// Converts the four unsigned 8-bit integers in the lower 32 bits to four
// unsigned 64-bit integers.
FORCE_INLINE __m256i _mm256_cvtepu8_epi64(__m128i a)
{
    __m256i ret;
    ret.vect_s64[0] = _mm_cvtepu8_epi64(a);
    ret.vect_s64[1] = _mm_cvtepu8_epi64(_mm_srli_si128(a,2));
    return ret;
}

FORCE_INLINE  __m256i _mm256_sllv_epi64(__m256i a, __m256i count)
{
 __m256i res;
 uint64_t *c = (uint64_t*) &count;
 int64_t *m = (int64_t*) &a;
 int64_t *p = (int64_t*) &res;
 for (int i=0; i<4; i++) p[i] = (c[i]<64) ? (m[i]<<c[i]) : 0;
 return res;
}

FORCE_INLINE __m256i _mm256_broadcastd_epi32       ( __m128i a )
{
    __m256i A;
    int *m = (int*) &a;
    int *p = (int*) &A;
    for (int i=0; i<8; i++) p[i] = m[0];
    return A;
}

FORCE_INLINE __m256i _mm256_broadcastq_epi64_REF   ( __m128i a )
{
    __m256i A;
    int64_t *m = (int64_t*) &a;
    int64_t *p = (int64_t*) &A;
    for (int i=0; i<4; i++) p[i] = m[0];
    return A;
}

FORCE_INLINE __m256i _mm256_shuffle_epi8(__m256i a, __m256i b)
{
    __m256i res_m256i;
    uint8x16_t mask_and = vdupq_n_u8(0x8f);
    res_m256i.vect_u8[0] = vqtbl1q_u8(a.vect_u8[0], vandq_u8(b.vect_u8[0], mask_and));
    res_m256i.vect_u8[1] = vqtbl1q_u8(a.vect_u8[1], vandq_u8(b.vect_u8[1], mask_and));
    return res_m256i;
}

FORCE_INLINE __m256i _mm256_multishift_epi64_epi8(__m256i a, __m256i b)
{
    __m256i res_m256i, tmp0, tmp1, tb0, tb1, sft0, sft1;
    uint8x16_t low3bit = vdupq_n_u8(0x07);
    uint8x16_t inc = vdupq_n_u8(0x01);
    
    tmp0.vect_u8[0] = vshrq_n_u8(a.vect_u8[0], 3);
    tmp0.vect_u8[1] = vshrq_n_u8(a.vect_u8[1], 3);
    tmp1.vect_u8[0] = vaddq_u8(tmp0.vect_u8[0], inc);
    tmp1.vect_u8[1] = vaddq_u8(tmp0.vect_u8[1], inc);
    tmp0.vect_u8[0] = vandq_u8(tmp0.vect_u8[0], low3bit);
    tmp0.vect_u8[1] = vandq_u8(tmp0.vect_u8[1], low3bit);
    tmp1.vect_u8[0] = vandq_u8(tmp1.vect_u8[0], low3bit);
    tmp1.vect_u8[1] = vandq_u8(tmp1.vect_u8[1], low3bit);
    inc = vcombine_u8(vdup_n_u8(0x00), vdup_n_u8(0x08));
    tmp0.vect_u8[0] = vaddq_u8(tmp0.vect_u8[0], inc);
    tmp0.vect_u8[1] = vaddq_u8(tmp0.vect_u8[1], inc);
    tmp1.vect_u8[0] = vaddq_u8(tmp1.vect_u8[0], inc);
    tmp1.vect_u8[1] = vaddq_u8(tmp1.vect_u8[1], inc);
    a.vect_u8[0] = vandq_u8(a.vect_u8[0], low3bit);
    a.vect_u8[1] = vandq_u8(a.vect_u8[1], low3bit);

    tb0.vect_u8[0] = vqtbl1q_u8(b.vect_u8[0], tmp0.vect_u8[0]);
    tb0.vect_u8[1] = vqtbl1q_u8(b.vect_u8[1], tmp0.vect_u8[1]);
    tb1.vect_u8[0] = vqtbl1q_u8(b.vect_u8[0], tmp1.vect_u8[0]);
    tb1.vect_u8[1] = vqtbl1q_u8(b.vect_u8[1], tmp1.vect_u8[1]);
    tmp0.vect_u8[0] = vzip1q_u8(tb0.vect_u8[0], tb1.vect_u8[0]);
    tmp0.vect_u8[1] = vzip2q_u8(tb0.vect_u8[0], tb1.vect_u8[0]);
    tmp1.vect_u8[0] = vzip1q_u8(tb0.vect_u8[1], tb1.vect_u8[1]);
    tmp1.vect_u8[1] = vzip2q_u8(tb0.vect_u8[1], tb1.vect_u8[1]);

    uint8x8_t a0_low64_bit = vget_low_u8(a.vect_u8[0]);
    uint8x8_t a0_high64_bit = vget_high_u8(a.vect_u8[0]);
    uint8x8_t a1_low64_bit = vget_low_u8(a.vect_u8[1]);
    uint8x8_t a1_high64_bit = vget_high_u8(a.vect_u8[1]);
    sft0.vect_u16[0] = vmovl_u8(a0_low64_bit);
    sft0.vect_u16[1] = vmovl_u8(a0_high64_bit);
    sft1.vect_u16[0] = vmovl_u8(a1_low64_bit);
    sft1.vect_u16[1] = vmovl_u8(a1_high64_bit);
    sft0.vect_s16[0] = vnegq_s16(sft0.vect_s16[0]);
    sft0.vect_s16[1] = vnegq_s16(sft0.vect_s16[1]);
    sft1.vect_s16[0] = vnegq_s16(sft1.vect_s16[0]);
    sft1.vect_s16[1] = vnegq_s16(sft1.vect_s16[1]);
    tmp0.vect_u16[0] = vshlq_u16(tmp0.vect_u16[0], sft0.vect_s16[0]);
    tmp0.vect_u16[1] = vshlq_u16(tmp0.vect_u16[1], sft0.vect_s16[1]);
    tmp1.vect_u16[0] = vshlq_u16(tmp1.vect_u16[0], sft1.vect_s16[0]);
    tmp1.vect_u16[1] = vshlq_u16(tmp1.vect_u16[1], sft1.vect_s16[1]);
    res_m256i.vect_u8[0] = vuzp1q_u8(tmp0.vect_u8[0], tmp0.vect_u8[1]);
    res_m256i.vect_u8[1] = vuzp1q_u8(tmp1.vect_u8[0], tmp1.vect_u8[1]);

    return res_m256i;
}

FORCE_INLINE __m256i _mm256_alignr_epi8(__m256i a, __m256i b, const int count)
{
    __m256i res_m256i;
    int8x16_t tmp0[3], tmp1[3];
    int shift = count > 32 ? 32 : count;
    tmp0[2] = vdupq_n_s8(0);
    tmp1[2] = vdupq_n_s8(0);
    tmp0[0] = b.vect_s8[0];
    tmp0[1] = a.vect_s8[0];
    tmp1[0] = b.vect_s8[1];
    tmp1[1] = a.vect_s8[1];
    res_m256i.vect_s8[0] = vld1q_s8((int8_t *)tmp0 + shift);
    res_m256i.vect_s8[1] = vld1q_s8((int8_t *)tmp1 + shift);
    return res_m256i;
}

FORCE_INLINE __m256d _mm256_blendv_pd(__m256d a, __m256d b, __m256d mask)
{
    __m256d result_m256d;
    uint64x2_t vect_flag[2];
    vect_flag[0] = vcgeq_s64((int64x2_t)mask.vect_f64[0], vdupq_n_s64(0));
    vect_flag[1] = vcgeq_s64((int64x2_t)mask.vect_f64[1], vdupq_n_s64(0));
    result_m256d.vect_f64[0] = vbslq_f64(vect_flag[0], a.vect_f64[0], b.vect_f64[0]);
    result_m256d.vect_f64[1] = vbslq_f64(vect_flag[1], a.vect_f64[1], b.vect_f64[1]);
    return result_m256d;
}

FORCE_INLINE __m256 _mm256_blendv_ps(__m256 a, __m256 b, __m256 mask)
{
    __m256 result_m256;
    uint32x4_t vect_flag[2];
    vect_flag[0] = vcgeq_s32((int32x4_t)mask.vect_f32[0], vdupq_n_s32(0));
    vect_flag[1] = vcgeq_s32((int32x4_t)mask.vect_f32[1], vdupq_n_s32(0));
    result_m256.vect_f32[0] = vbslq_f32(vect_flag[0], a.vect_f32[0], b.vect_f32[0]);
    result_m256.vect_f32[1] = vbslq_f32(vect_flag[1], a.vect_f32[1], b.vect_f32[1]);
    return result_m256;
}

FORCE_INLINE __m256i _mm256_blend_epi32(__m256i a, __m256i b, const int imm8)
{
    assert(imm8 >= 0 && imm8 <= 255);
    __m256i result_m256;
    uint32x4_t vect_mask = vld1q_u32(g_mask_epi32);
    uint32x4_t vect_imm = vdupq_n_u32(imm8);
    uint32x4_t flag[2];
    flag[0] = vtstq_u32(vect_imm, vect_mask);
    flag[1] = vtstq_u32(vshrq_n_u32(vect_imm, 4), vect_mask);
    result_m256.vect_s32[0] = vbslq_s32(flag[0], b.vect_s32[0], a.vect_s32[0]);
    result_m256.vect_s32[1] = vbslq_s32(flag[1], b.vect_s32[1], a.vect_s32[1]);
    return result_m256;
} 

FORCE_INLINE __m256 _mm256_blend_ps(__m256 a, __m256 b, const int imm8)
{
    assert(imm8 >= 0 && imm8 <= 255);
    __m256 result_m256;
    uint32x4_t vect_mask = vld1q_u32(g_mask_epi32);
    uint32x4_t vect_imm = vdupq_n_u32(imm8);
    uint32x4_t flag[2];
    flag[0] = vtstq_u32(vect_imm, vect_mask);
    flag[1] = vtstq_u32(vshrq_n_u32(vect_imm, 4), vect_mask);
    result_m256.vect_f32[0] = vbslq_f32(flag[0], b.vect_f32[0], a.vect_f32[0]);
    result_m256.vect_f32[1] = vbslq_f32(flag[1], b.vect_f32[1], a.vect_f32[1]);
    return result_m256;
} 

FORCE_INLINE __m256d _mm256_blend_pd(__m256d a, __m256d b, const int imm8)
{
    assert(imm8 >= 0 && imm8 <= 15);
    __m256d result_m256d;
    uint64x2_t vect_mask = vld1q_u64(g_mask_epi64);
    uint64x2_t vect_imm = vdupq_n_u64(imm8);
    uint64x2_t flag[2];
    flag[0] = vtstq_u64(vect_imm, vect_mask);
    flag[1] = vtstq_u64(vshrq_n_u64(vect_imm, 2), vect_mask);
    result_m256d.vect_f64[0] = vbslq_f64(flag[0], b.vect_f64[0], a.vect_f64[0]);
    result_m256d.vect_f64[1] = vbslq_f64(flag[1], b.vect_f64[1], a.vect_f64[1]);
    return result_m256d;
}

FORCE_INLINE __m256i _mm256_inserti128_si256(__m256i a, __m128i b, const int imm8)
{
    assert(imm8 == 0 || imm8 == 1);
    __m256i res;
    uint32x4_t vmask = vceqq_s32(vdupq_n_s32(imm8), vdupq_n_s32(0));
    res.vect_s32[0] = vbslq_s32(vmask, b, a.vect_s32[0]);
    res.vect_s32[1] = vbslq_s32(vmask, a.vect_s32[1], b);
    return res;
}

FORCE_INLINE __m256d _mm256_insertf128_pd(__m256d a, __m128d b, int imm8)
{
    assert(imm8 == 0 || imm8 == 1);
    __m256d res;
    uint64x2_t vmask = vceqq_s64(vdupq_n_s64(imm8), vdupq_n_s64(0));
    res.vect_f64[0] = vbslq_f64(vmask, b, a.vect_f64[0]);
    res.vect_f64[1] = vbslq_f64(vmask, a.vect_f64[1], b);
    return res;
}

FORCE_INLINE __m256 _mm256_insertf128_ps(__m256 a, __m128 b, int imm8)
{
    assert(imm8 == 0 || imm8 == 1);
    __m256 res;
    uint32x4_t vmask = vceqq_s32(vdupq_n_s32(imm8), vdupq_n_s32(0));
    res.vect_f32[0] = vbslq_f32(vmask, b, a.vect_f32[0]);
    res.vect_f32[1] = vbslq_f32(vmask, a.vect_f32[1], b);
    return res;
}

// FORCE_INLINE __m256i _mm256_insert_epi32 (__m256i a, int32_t i, const int index)
#define _mm256_insert_epi32 ( a,  i,  index) \
({ \
    assert(index >= 0 && index <= 7); \
    if (index > 3) { \
        a.vect_s32[1] = vsetq_lane_s32(i, a.vect_s32[1], index & 3); \
    } else { \
        a.vect_s32[0] = vsetq_lane_s32(i, a.vect_s32[0], index); \
    } \
    a; \
})

// FORCE_INLINE __m256i _mm256_insert_epi64 (__m256i a, int64_t i, const int index)
#define _mm256_insert_epi64 ( a,  i,  index) \
({ \
    assert(index >= 0 && index <= 3); \
    if (index > 1) { \
        a.vect_s64[1] = vsetq_lane_s64(i, a.vect_s64[1], index & 1); \
    } else { \
        a.vect_s64[0] = vsetq_lane_s64(i, a.vect_s64[0], index); \
    } \
    a; \
})

FORCE_INLINE __m256i _mm256_cmpgt_epi32 (__m256i a, __m256i b)
{
    __m256i res;
    res.vect_u32[0] = vcgtq_s32(a.vect_s32[0], b.vect_s32[0]);
    res.vect_u32[1] = vcgtq_s32(a.vect_s32[1], b.vect_s32[1]);
    return res;
}

FORCE_INLINE __m256i _mm256_cmpgt_epi16 (__m256i a, __m256i b)
{
    __m256i res;
    res.vect_u16[0] = vcgtq_s16(a.vect_s16[0], b.vect_s16[0]);
    res.vect_u16[1] = vcgtq_s16(a.vect_s16[1], b.vect_s16[1]);
    return res;
}

FORCE_INLINE __m256i _mm256_cmpgt_epi8 (__m256i a, __m256i b)
{
    __m256i result_m256i;
    result_m256i.vect_u8[0] = vcgtq_s8(a.vect_s8[0], b.vect_s8[0]);
    result_m256i.vect_u8[1] = vcgtq_s8(a.vect_s8[1], b.vect_s8[1]);
    return result_m256i;
}

FORCE_INLINE __m256i _mm256_cmpeq_epi32 (__m256i a, __m256i b)
{
    __m256i result_m256i;
    result_m256i.vect_u32[0] = vceqq_s32(a.vect_s32[0], b.vect_s32[0]);
    result_m256i.vect_u32[1] = vceqq_s32(a.vect_s32[1], b.vect_s32[1]);
    return result_m256i;
}

FORCE_INLINE __m256i _mm256_cmpeq_epi16 (__m256i a, __m256i b)
{
    __m256i result_m256i;
    result_m256i.vect_u16[0] = vceqq_s16(a.vect_s16[0], b.vect_s16[0]);
    result_m256i.vect_u16[1] = vceqq_s16(a.vect_s16[1], b.vect_s16[1]);
    return result_m256i;
}

FORCE_INLINE __m256i _mm256_cmpeq_epi8(__m256i a, __m256i b)
{
    __m256i result_m256i;
    result_m256i.vect_u8[0] = vceqq_s8(a.vect_s8[0], b.vect_s8[0]);
    result_m256i.vect_u8[1] = vceqq_s8(a.vect_s8[1], b.vect_s8[1]);
    return result_m256i;
}

typedef uint64x2_t (*TYPE_FUNC_CMP_PD)(__m128d a, __m128d b);
typedef struct {
    int opDef;
    TYPE_FUNC_CMP_PD cmpFun;
} FuncListCmp256Pd;

static uint64x2_t _cmp_eq_oq(__m128d a, __m128d b)
{ /* Equal (ordered, non-signaling) */
    return vceqq_f64(a, b);
}

static uint64x2_t _cmp_lt_os(__m128d a, __m128d b)
{ /* Less-than (ordered, signaling)  */
    return vcltq_f64(a, b);
}

static uint64x2_t _cmp_le_os(__m128d a, __m128d b)
{ /* Less-than-or-equal (ordered, signaling)  */
    return vcleq_f64(a, b);
}

static uint64x2_t _cmp_unord_q(__m128d a, __m128d b)
{ /* Unordered (non-signaling)  */

    uint64x2_t res_m128i;
    float64_t *ptr_a = (float64_t *)&a;
    float64_t *ptr_b = (float64_t *)&b;
    uint64_t *ptr_r = (uint64_t *)&res_m128i;

    ptr_r[0] = isunordered(ptr_a[0], ptr_b[0]) ? -1 : 0;
    ptr_r[1] = isunordered(ptr_a[1], ptr_b[1]) ? -1 : 0;
    return res_m128i;
}

static uint64x2_t _cmp_neq_uq(__m128d a, __m128d b)
{ /* Not-equal (unordered, non-signaling)  */
    __m128I res;
    res.vect_u64 = vceqq_f64(a, b);
    res.vect_u32 = vmvnq_u32(res.vect_u32);
    return res.vect_u64;
}

static uint64x2_t _cmp_nlt_us(__m128d a, __m128d b)
{ /* Not-less-than (unordered, signaling) */

    uint64x2_t res_m128i;
    float64_t *ptr_a = (float64_t *)&a;
    float64_t *ptr_b = (float64_t *)&b;
    uint64_t *ptr_r = (uint64_t *)&res_m128i;

    res_m128i = vcgeq_f64(a, b);
    
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    return res_m128i;
}

static uint64x2_t _cmp_nle_us(__m128d a, __m128d b)
{ /* Not-less-than-or-equal (unordered, signaling)  */
    uint64x2_t res_m128i;
    float64_t *ptr_a = (float64_t *)&a;
    float64_t *ptr_b = (float64_t *)&b;
    uint64_t *ptr_r = (uint64_t *)&res_m128i;

    res_m128i = vcgtq_f64(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    return res_m128i;
}

static uint64x2_t _cmp_ord_q(__m128d a, __m128d b)
{ /* Ordered (nonsignaling) */
    uint64x2_t res_m128i;
    float64_t *ptr_a = (float64_t *)&a;
    float64_t *ptr_b = (float64_t *)&b;
    uint64_t *ptr_r = (uint64_t *)&res_m128i;

    ptr_r[0] = !isunordered(ptr_a[0], ptr_b[0]) ? -1 : 0;
    ptr_r[1] = !isunordered(ptr_a[1], ptr_b[1]) ? -1 : 0;
    return res_m128i;
}

static uint64x2_t _cmp_eq_uq(__m128d a, __m128d b)
{ /* Equal (unordered, non-signaling) */
    uint64x2_t res_m128i;
    float64_t *ptr_a = (float64_t *)&a;
    float64_t *ptr_b = (float64_t *)&b;
    uint64_t *ptr_r = (uint64_t *)&res_m128i;

    res_m128i = vceqq_f64(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    return res_m128i;
}

static uint64x2_t _cmp_nge_us(__m128d a, __m128d b)
{ /* Not-greater-than-or-equal (unordered, signaling) */
    uint64x2_t res_m128i;
    float64_t *ptr_a = (float64_t *)&a;
    float64_t *ptr_b = (float64_t *)&b;
    uint64_t *ptr_r = (uint64_t *)&res_m128i;

    res_m128i = vcltq_f64(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    return res_m128i;
}

static uint64x2_t _cmp_ngt_us(__m128d a, __m128d b)
{ /* Not-greater-than (unordered, signaling)  */
    uint64x2_t res_m128i;
    float64_t *ptr_a = (float64_t *)&a;
    float64_t *ptr_b = (float64_t *)&b;
    uint64_t *ptr_r = (uint64_t *)&res_m128i;

    res_m128i = vcleq_f64(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    return res_m128i;
}

static uint64x2_t _cmp_false_oq(__m128d a, __m128d b)
{ /* False (ordered, non-signaling)  */
    (void)a;
    (void)b;

    return vdupq_n_u64(0);
}

static uint64x2_t _cmp_neq_oq(__m128d a, __m128d b)
{ /* Not-equal (ordered, non-signaling)  */
    __m128I res_m128i;
    float64_t *ptr_a = (float64_t *)&a;
    float64_t *ptr_b = (float64_t *)&b;
    uint64_t *ptr_r = (uint64_t *)&res_m128i;

    res_m128i.vect_u64 = vceqq_f64(a, b);
    res_m128i.vect_u32 = vmvnq_u32(res_m128i.vect_u32);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = 0;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = 0;
    }
    return res_m128i.vect_u64;
}

static uint64x2_t _cmp_ge_os(__m128d a, __m128d b)
{ /* Greater-than-or-equal (ordered, signaling)  */
    return vcgeq_f64(a, b);
}

static uint64x2_t _cmp_gt_os(__m128d a, __m128d b)
{ /* Greater-than (ordered, signaling)  */
    return vcgtq_f64(a, b);
}

static uint64x2_t _cmp_true_uq(__m128d a, __m128d b)
{ /* True (unordered, non-signaling) */
    (void)a;
    (void)b;

    return vdupq_n_u64(-1);
}

static uint64x2_t _cmp_eq_os(__m128d a, __m128d b)
{ /* Equal (ordered, signaling)  */
    return vceqq_f64(a, b);
}

static uint64x2_t _cmp_lt_oq(__m128d a, __m128d b)
{ /* Less-than (ordered, non-signaling)  */
    return vcltq_f64(a, b);
}

static uint64x2_t _cmp_le_oq(__m128d a, __m128d b)
{ /* Less-than-or-equal (ordered, non-signaling)  */
    return vcleq_f64(a, b);
}

static uint64x2_t _cmp_unord_s(__m128d a, __m128d b)
{ /* Unordered (signaling)  */
    uint64x2_t res_m128i;
    float64_t *ptr_a = (float64_t *)&a;
    float64_t *ptr_b = (float64_t *)&b;
    uint64_t *ptr_r = (uint64_t *)&res_m128i;

    ptr_r[0] = isunordered(ptr_a[0], ptr_b[0]) ? -1 : 0;
    ptr_r[1] = isunordered(ptr_a[1], ptr_b[1]) ? -1 : 0;
    return res_m128i;
}

static uint64x2_t _cmp_neq_us(__m128d a, __m128d b)
{ /* Not-equal (unordered, signaling) */
    __m128I res_m128i;
    res_m128i.vect_u64 = vceqq_f64(a, b);
    res_m128i.vect_u32 = vmvnq_u32(res_m128i.vect_u32);
    return res_m128i.vect_u64;
}

static uint64x2_t _cmp_nlt_uq(__m128d a, __m128d b)
{ /* Not-less-than (unordered, non-signaling)*/
    uint64x2_t res_m128i;
    float64_t *ptr_a = (float64_t *)&a;
    float64_t *ptr_b = (float64_t *)&b;
    uint64_t *ptr_r = (uint64_t *)&res_m128i;

    res_m128i = vcgeq_f64(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    return res_m128i;
}

static uint64x2_t _cmp_nle_uq(__m128d a, __m128d b)
{ /* Not-less-than-or-equal (unordered, non-signaling) */
    uint64x2_t res_m128i;
    float64_t *ptr_a = (float64_t *)&a;
    float64_t *ptr_b = (float64_t *)&b;
    uint64_t *ptr_r = (uint64_t *)&res_m128i;

    res_m128i = vcgtq_f64(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    return res_m128i;
}

static uint64x2_t _cmp_ord_s(__m128d a, __m128d b)
{ /* Ordered (signaling)  */
    uint64x2_t res_m128i;
    float64_t *ptr_a = (float64_t *)&a;
    float64_t *ptr_b = (float64_t *)&b;
    uint64_t *ptr_r = (uint64_t *)&res_m128i;

    res_m128i = vceqq_f64(a, b);
    ptr_r[0] = !isunordered(ptr_a[0], ptr_b[0]) ? -1 : 0;
    ptr_r[1] = !isunordered(ptr_a[1], ptr_b[1]) ? -1 : 0;
    return res_m128i;
}

static uint64x2_t _cmp_eq_us(__m128d a, __m128d b)
{ /* Equal (unordered, signaling)  */
    uint64x2_t res_m128i;
    float64_t *ptr_a = (float64_t *)&a;
    float64_t *ptr_b = (float64_t *)&b;
    uint64_t *ptr_r = (uint64_t *)&res_m128i;

    res_m128i = vceqq_f64(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    return res_m128i;
}

static uint64x2_t _cmp_nge_uq(__m128d a, __m128d b)
{ /* Not-greater-than-or-equal (unordered, non-signaling)  */
    uint64x2_t res_m128i;
    float64_t *ptr_a = (float64_t *)&a;
    float64_t *ptr_b = (float64_t *)&b;
    uint64_t *ptr_r = (uint64_t *)&res_m128i;

    res_m128i = vcltq_f64(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    return res_m128i;
}

static uint64x2_t _cmp_ngt_uq(__m128d a, __m128d b)
{ /* Not-greater-than (unordered, non-signaling) */
    uint64x2_t res_m128i;
    float64_t *ptr_a = (float64_t *)&a;
    float64_t *ptr_b = (float64_t *)&b;
    uint64_t *ptr_r = (uint64_t *)&res_m128i;

    res_m128i = vcleq_f64(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    return res_m128i;
}

static uint64x2_t _cmp_false_os(__m128d a, __m128d b)
{ /* False (ordered, signaling)  */
    (void)a;
    (void)b;

    return vdupq_n_u64(0);
}

static uint64x2_t _cmp_neq_os(__m128d a, __m128d b)
{ /* Not-equal (ordered, signaling)  */
    __m128I res_m128i;
    float64_t *ptr_a = (float64_t *)&a;
    float64_t *ptr_b = (float64_t *)&b;
    uint64_t *ptr_r = (uint64_t *)&res_m128i;

    res_m128i.vect_u64 = vceqq_f64(a, b);
    res_m128i.vect_u32 = vmvnq_u32(res_m128i.vect_u32);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = 0;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = 0;
    }
    return res_m128i.vect_u64;
}

static uint64x2_t _cmp_ge_oq(__m128d a, __m128d b)
{ /* Greater-than-or-equal (ordered, non-signaling)  */
    return vcgeq_f64(a, b);
}

static uint64x2_t _cmp_gt_oq(__m128d a, __m128d b)
{ /* Greater-than (ordered, non-signaling)  */
    return vcgtq_f64(a, b);
}

static uint64x2_t _cmp_true_us(__m128d a, __m128d b)
{ /* True (unordered, signaling)  */
    (void)a;
    (void)b;

    return vdupq_n_u64(-1);
}

static FuncListCmp256Pd g_FunListCmp256Pd[] = {
    {_CMP_EQ_OQ, _cmp_eq_oq},   {_CMP_LT_OS, _cmp_lt_os},   {_CMP_LE_OS, _cmp_le_os},   {_CMP_UNORD_Q, _cmp_unord_q},
    {_CMP_NEQ_UQ, _cmp_neq_uq}, {_CMP_NLT_US, _cmp_nlt_us}, {_CMP_NLE_US, _cmp_nle_us}, {_CMP_ORD_Q, _cmp_ord_q},
    {_CMP_EQ_UQ, _cmp_eq_uq},   {_CMP_NGE_US, _cmp_nge_us}, {_CMP_NGT_US, _cmp_ngt_us}, {_CMP_FALSE_OQ, _cmp_false_oq},
    {_CMP_NEQ_OQ, _cmp_neq_oq}, {_CMP_GE_OS, _cmp_ge_os},   {_CMP_GT_OS, _cmp_gt_os},   {_CMP_TRUE_UQ, _cmp_true_uq},
    {_CMP_EQ_OS, _cmp_eq_os},   {_CMP_LT_OQ, _cmp_lt_oq},   {_CMP_LE_OQ, _cmp_le_oq},   {_CMP_UNORD_S, _cmp_unord_s},
    {_CMP_NEQ_US, _cmp_neq_us}, {_CMP_NLT_UQ, _cmp_nlt_uq}, {_CMP_NLE_UQ, _cmp_nle_uq}, {_CMP_ORD_S, _cmp_ord_s},
    {_CMP_EQ_US, _cmp_eq_us},   {_CMP_NGE_UQ, _cmp_nge_uq}, {_CMP_NGT_UQ, _cmp_ngt_uq}, {_CMP_FALSE_OS, _cmp_false_os},
    {_CMP_NEQ_OS, _cmp_neq_os}, {_CMP_GE_OQ, _cmp_ge_oq},   {_CMP_GT_OQ, _cmp_gt_oq},   {_CMP_TRUE_US, _cmp_true_us}};

FORCE_INLINE __m256d _mm256_cmp_pd(__m256d a, __m256d b, const int imm8)
{
    assert(imm8 < 32 && imm8 >= 0);
    __m256d dst;
    dst.vect_f64[0] = (float64x2_t)g_FunListCmp256Pd[imm8].cmpFun(a.vect_f64[0], b.vect_f64[0]);
    dst.vect_f64[1] = (float64x2_t)g_FunListCmp256Pd[imm8].cmpFun(a.vect_f64[1], b.vect_f64[1]);
    return dst;
}

typedef uint32x4_t (*TYPE_FUNC_CMP_PS)(__m128 a, __m128 b);
typedef struct {
    int opDef;
    TYPE_FUNC_CMP_PS cmpFun;
} FuncListCmp256Ps;

static uint32x4_t _cmp_eq_oq_ps(__m128 a, __m128 b)
{ /* Equal (ordered, non-signaling) */
    return vceqq_f32(a, b);
}

static uint32x4_t _cmp_lt_os_ps(__m128 a, __m128 b)
{ /* Less-than (ordered, signaling)  */
    return vcltq_f32(a, b);
}

static uint32x4_t _cmp_le_os_ps(__m128 a, __m128 b)
{ /* Less-than-or-equal (ordered, signaling)  */
    return vcleq_f32(a, b);
}

static uint32x4_t _cmp_unord_q_ps(__m128 a, __m128 b)
{ /* Unordered (non-signaling)  */
    uint32x4_t res;
    float32_t *ptr_a = (float32_t *)&a;
    float32_t *ptr_b = (float32_t *)&b;
    uint32_t *ptr_r = (uint32_t *)&res;

    ptr_r[0] = isunordered(ptr_a[0], ptr_b[0]) ? -1 : 0;
    ptr_r[1] = isunordered(ptr_a[1], ptr_b[1]) ? -1 : 0;
    ptr_r[2] = isunordered(ptr_a[2], ptr_b[2]) ? -1 : 0;
    ptr_r[3] = isunordered(ptr_a[3], ptr_b[3]) ? -1 : 0;
    return res;
}

static uint32x4_t _cmp_neq_uq_ps(__m128 a, __m128 b)
{ /* Not-equal (unordered, non-signaling)  */
    uint32x4_t res = vceqq_f32(a, b);
    return vmvnq_u32(res);
}

static uint32x4_t _cmp_nlt_us_ps(__m128 a, __m128 b)
{ /* Not-less-than (unordered, signaling)  */
    uint32x4_t res;
    float32_t *ptr_a = (float32_t *)&a;
    float32_t *ptr_b = (float32_t *)&b;
    uint32_t *ptr_r = (uint32_t *)&res;

    res = vcgeq_f32(a, b);
    
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    if (isunordered(ptr_a[2], ptr_b[2])) {
        ptr_r[2] = -1;
    }
    if (isunordered(ptr_a[3], ptr_b[3])) {
        ptr_r[3] = -1;
    }
    return res;
}

static uint32x4_t _cmp_nle_us_ps(__m128 a, __m128 b)
{ /* Not-less-than-or-equal (unordered, signaling)  */
    uint32x4_t res;
    float32_t *ptr_a = (float32_t *)&a;
    float32_t *ptr_b = (float32_t *)&b;
    uint32_t *ptr_r = (uint32_t *)&res;

    res = vcgtq_f32(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    if (isunordered(ptr_a[2], ptr_b[2])) {
        ptr_r[2] = -1;
    }
    if (isunordered(ptr_a[3], ptr_b[3])) {
        ptr_r[3] = -1;
    }
    return res;
}

static uint32x4_t _cmp_ord_q_ps(__m128 a, __m128 b)
{ /* Ordered (nonsignaling) */
    uint32x4_t res;
    float32_t *ptr_a = (float32_t *)&a;
    float32_t *ptr_b = (float32_t *)&b;
    uint32_t *ptr_r = (uint32_t *)&res;

    ptr_r[0] = !isunordered(ptr_a[0], ptr_b[0]) ? -1 : 0;
    ptr_r[1] = !isunordered(ptr_a[1], ptr_b[1]) ? -1 : 0;
    ptr_r[2] = !isunordered(ptr_a[2], ptr_b[2]) ? -1 : 0;
    ptr_r[3] = !isunordered(ptr_a[3], ptr_b[3]) ? -1 : 0;
    return res;
}

static uint32x4_t _cmp_eq_uq_ps(__m128 a, __m128 b)
{ /* Equal (unordered, non-signaling) */ 
    uint32x4_t res;
    float32_t *ptr_a = (float32_t *)&a;
    float32_t *ptr_b = (float32_t *)&b;
    uint32_t *ptr_r = (uint32_t *)&res;

    res = vceqq_f32(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    if (isunordered(ptr_a[2], ptr_b[2])) {
        ptr_r[2] = -1;
    }
    if (isunordered(ptr_a[3], ptr_b[3])) {
        ptr_r[3] = -1;
    }
    return res;
}

static uint32x4_t _cmp_nge_us_ps(__m128 a, __m128 b)
{ /* Not-greater-than-or-equal (unordered, signaling) */
    uint32x4_t res;
    float32_t *ptr_a = (float32_t *)&a;
    float32_t *ptr_b = (float32_t *)&b;
    uint32_t *ptr_r = (uint32_t *)&res;

    res = vcltq_f32(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    if (isunordered(ptr_a[2], ptr_b[2])) {
        ptr_r[2] = -1;
    }
    if (isunordered(ptr_a[3], ptr_b[3])) {
        ptr_r[3] = -1;
    }
    return res;
}

static uint32x4_t _cmp_ngt_us_ps(__m128 a, __m128 b)
{ /* Not-greater-than (unordered, signaling)  */
    uint32x4_t res;
    float32_t *ptr_a = (float32_t *)&a;
    float32_t *ptr_b = (float32_t *)&b;
    uint32_t *ptr_r = (uint32_t *)&res;

    res = vcleq_f32(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    if (isunordered(ptr_a[2], ptr_b[2])) {
        ptr_r[2] = -1;
    }
    if (isunordered(ptr_a[3], ptr_b[3])) {
        ptr_r[3] = -1;
    }
    return res;
}

static uint32x4_t _cmp_false_oq_ps(__m128 a, __m128 b)
{ /* False (ordered, non-signaling)  */
    (void)a;
    (void)b;
    return vdupq_n_u32(0);
}

static uint32x4_t _cmp_neq_oq_ps(__m128 a, __m128 b)
{ /* Not-equal (ordered, non-signaling)  */
    uint32x4_t res;
    float32_t *ptr_a = (float32_t *)&a;
    float32_t *ptr_b = (float32_t *)&b;
    uint32_t *ptr_r = (uint32_t *)&res;

    res = vceqq_f32(a, b);
    res = vmvnq_u32(res);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = 0;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = 0;
    }
    if (isunordered(ptr_a[2], ptr_b[2])) {
        ptr_r[2] = 0;
    }
    if (isunordered(ptr_a[3], ptr_b[3])) {
        ptr_r[3] = 0;
    }
    return res;
}

static uint32x4_t _cmp_ge_os_ps(__m128 a, __m128 b)
{ /* Greater-than-or-equal (ordered, signaling)  */
    return vcgeq_f32(a, b);
}

static uint32x4_t _cmp_gt_os_ps(__m128 a, __m128 b)
{ /* Greater-than (ordered, signaling)  */
    return vcgtq_f32(a, b);
}

static uint32x4_t _cmp_true_uq_ps(__m128 a, __m128 b)
{ /* True (unordered, non-signaling) */
    (void)a;
    (void)b;
    return vdupq_n_u32(-1);
}

static uint32x4_t _cmp_eq_os_ps(__m128 a, __m128 b)
{ /* Equal (ordered, signaling)  */
    return vceqq_f32(a, b);
}

static uint32x4_t _cmp_lt_oq_ps(__m128 a, __m128 b)
{ /* Less-than (ordered, non-signaling)  */
    return vcltq_f32(a, b);
}

static uint32x4_t _cmp_le_oq_ps(__m128 a, __m128 b)
{ /* Less-than-or-equal (ordered, non-signaling)  */
    return vcleq_f32(a, b);
}

static uint32x4_t _cmp_unord_s_ps(__m128 a, __m128 b)
{ /* Unordered (signaling)  */
    uint32x4_t res;
    float32_t *ptr_a = (float32_t *)&a;
    float32_t *ptr_b = (float32_t *)&b;
    uint32_t *ptr_r = (uint32_t *)&res;

    ptr_r[0] = isunordered(ptr_a[0], ptr_b[0]) ? -1 : 0;
    ptr_r[1] = isunordered(ptr_a[1], ptr_b[1]) ? -1 : 0;
    ptr_r[2] = isunordered(ptr_a[2], ptr_b[2]) ? -1 : 0;
    ptr_r[3] = isunordered(ptr_a[3], ptr_b[3]) ? -1 : 0;
    return res;
}

static uint32x4_t _cmp_neq_us_ps(__m128 a, __m128 b)
{ /* Not-equal (unordered, signaling) */
    uint32x4_t res = vceqq_f32(a, b);
    return vmvnq_u32(res);
}

static uint32x4_t _cmp_nlt_uq_ps(__m128 a, __m128 b)
{ /* Not-less-than (unordered, non-signaling)*/  
    uint32x4_t res;
    float32_t *ptr_a = (float32_t *)&a;
    float32_t *ptr_b = (float32_t *)&b;
    uint32_t *ptr_r = (uint32_t *)&res;

    res = vcgeq_f32(a, b);
    
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    if (isunordered(ptr_a[2], ptr_b[2])) {
        ptr_r[2] = -1;
    }
    if (isunordered(ptr_a[3], ptr_b[3])) {
        ptr_r[3] = -1;
    }
    return res;
}

static uint32x4_t _cmp_nle_uq_ps(__m128 a, __m128 b)
{ /* Not-less-than-or-equal (unordered, non-signaling) */
    uint32x4_t res;
    float32_t *ptr_a = (float32_t *)&a;
    float32_t *ptr_b = (float32_t *)&b;
    uint32_t *ptr_r = (uint32_t *)&res;

    res = vcgtq_f32(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    if (isunordered(ptr_a[2], ptr_b[2])) {
        ptr_r[2] = -1;
    }
    if (isunordered(ptr_a[3], ptr_b[3])) {
        ptr_r[3] = -1;
    }
    return res;
}

static uint32x4_t _cmp_ord_s_ps(__m128 a, __m128 b)
{ /* Ordered (signaling)  */
    uint32x4_t res;
    float32_t *ptr_a = (float32_t *)&a;
    float32_t *ptr_b = (float32_t *)&b;
    uint32_t *ptr_r = (uint32_t *)&res;

    ptr_r[0] = !isunordered(ptr_a[0], ptr_b[0]) ? -1 : 0;
    ptr_r[1] = !isunordered(ptr_a[1], ptr_b[1]) ? -1 : 0;
    ptr_r[2] = !isunordered(ptr_a[2], ptr_b[2]) ? -1 : 0;
    ptr_r[3] = !isunordered(ptr_a[3], ptr_b[3]) ? -1 : 0;
    return res;
}

static uint32x4_t _cmp_eq_us_ps(__m128 a, __m128 b)
{ /* Equal (unordered, signaling)  */
    uint32x4_t res;
    float32_t *ptr_a = (float32_t *)&a;
    float32_t *ptr_b = (float32_t *)&b;
    uint32_t *ptr_r = (uint32_t *)&res;

    res = vceqq_f32(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    if (isunordered(ptr_a[2], ptr_b[2])) {
        ptr_r[2] = -1;
    }
    if (isunordered(ptr_a[3], ptr_b[3])) {
        ptr_r[3] = -1;
    }
    return res;
}

static uint32x4_t _cmp_nge_uq_ps(__m128 a, __m128 b)
{ /* Not-greater-than-or-equal (unordered, non-signaling)  */
    uint32x4_t res;
    float32_t *ptr_a = (float32_t *)&a;
    float32_t *ptr_b = (float32_t *)&b;
    uint32_t *ptr_r = (uint32_t *)&res;

    res = vcltq_f32(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    if (isunordered(ptr_a[2], ptr_b[2])) {
        ptr_r[2] = -1;
    }
    if (isunordered(ptr_a[3], ptr_b[3])) {
        ptr_r[3] = -1;
    }
    return res;
}

static uint32x4_t _cmp_ngt_uq_ps(__m128 a, __m128 b)
{ /* Not-greater-than (unordered, non-signaling) */ 
    uint32x4_t res;
    float32_t *ptr_a = (float32_t *)&a;
    float32_t *ptr_b = (float32_t *)&b;
    uint32_t *ptr_r = (uint32_t *)&res;

    res = vcleq_f32(a, b);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = -1;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = -1;
    }
    if (isunordered(ptr_a[2], ptr_b[2])) {
        ptr_r[2] = -1;
    }
    if (isunordered(ptr_a[3], ptr_b[3])) {
        ptr_r[3] = -1;
    }
    return res;
}

static uint32x4_t _cmp_false_os_ps(__m128 a, __m128 b)
{ /* False (ordered, signaling)  */
    (void)a;
    (void)b;
    return vdupq_n_u32(0);
}

static uint32x4_t _cmp_neq_os_ps(__m128 a, __m128 b)
{ /* Not-equal (ordered, signaling)  */
    uint32x4_t res;
    float32_t *ptr_a = (float32_t *)&a;
    float32_t *ptr_b = (float32_t *)&b;
    uint32_t *ptr_r = (uint32_t *)&res;

    res = vceqq_f32(a, b);
    res = vmvnq_u32(res);
    if (isunordered(ptr_a[0], ptr_b[0])) {
        ptr_r[0] = 0;
    }
    if (isunordered(ptr_a[1], ptr_b[1])) {
        ptr_r[1] = 0;
    }
    if (isunordered(ptr_a[2], ptr_b[2])) {
        ptr_r[2] = 0;
    }
    if (isunordered(ptr_a[3], ptr_b[3])) {
        ptr_r[3] = 0;
    }
    return res;
}

static uint32x4_t _cmp_ge_oq_ps(__m128 a, __m128 b)
{ /* Greater-than-or-equal (ordered, non-signaling)  */
    return vcgeq_f32(a, b);
}

static uint32x4_t _cmp_gt_oq_ps(__m128 a, __m128 b)
{ /* Greater-than (ordered, non-signaling)  */
    return vcgtq_f32(a, b);
}

static uint32x4_t _cmp_true_us_ps(__m128 a, __m128 b)
{ /* True (unordered, signaling)  */
    (void)a;
    (void)b;
    return vdupq_n_u32(-1);
}

static FuncListCmp256Ps g_FunListCmp256Ps[] = {
    {_CMP_EQ_OQ, _cmp_eq_oq_ps},       {_CMP_LT_OS, _cmp_lt_os_ps},     {_CMP_LE_OS, _cmp_le_os_ps},
    {_CMP_UNORD_Q, _cmp_unord_q_ps},   {_CMP_NEQ_UQ, _cmp_neq_uq_ps},   {_CMP_NLT_US, _cmp_nlt_us_ps},
    {_CMP_NLE_US, _cmp_nle_us_ps},     {_CMP_ORD_Q, _cmp_ord_q_ps},     {_CMP_EQ_UQ, _cmp_eq_uq_ps},
    {_CMP_NGE_US, _cmp_nge_us_ps},     {_CMP_NGT_US, _cmp_ngt_us_ps},   {_CMP_FALSE_OQ, _cmp_false_oq_ps},
    {_CMP_NEQ_OQ, _cmp_neq_oq_ps},     {_CMP_GE_OS, _cmp_ge_os_ps},     {_CMP_GT_OS, _cmp_gt_os_ps},
    {_CMP_TRUE_UQ, _cmp_true_uq_ps},   {_CMP_EQ_OS, _cmp_eq_os_ps},     {_CMP_LT_OQ, _cmp_lt_oq_ps},
    {_CMP_LE_OQ, _cmp_le_oq_ps},       {_CMP_UNORD_S, _cmp_unord_s_ps}, {_CMP_NEQ_US, _cmp_neq_us_ps},
    {_CMP_NLT_UQ, _cmp_nlt_uq_ps},     {_CMP_NLE_UQ, _cmp_nle_uq_ps},   {_CMP_ORD_S, _cmp_ord_s_ps},
    {_CMP_EQ_US, _cmp_eq_us_ps},       {_CMP_NGE_UQ, _cmp_nge_uq_ps},   {_CMP_NGT_UQ, _cmp_ngt_uq_ps},
    {_CMP_FALSE_OS, _cmp_false_os_ps}, {_CMP_NEQ_OS, _cmp_neq_os_ps},   {_CMP_GE_OQ, _cmp_ge_oq_ps},
    {_CMP_GT_OQ, _cmp_gt_oq_ps},       {_CMP_TRUE_US, _cmp_true_us_ps}};

FORCE_INLINE __m256 _mm256_cmp_ps(__m256 a, __m256 b, const int imm8)
{
    assert(imm8 < 32 && imm8 >= 0);
    __m256 dst;
    dst.vect_f32[0] = vreinterpretq_f32_u32(g_FunListCmp256Ps[imm8].cmpFun(a.vect_f32[0], b.vect_f32[0]));
    dst.vect_f32[1] = vreinterpretq_f32_u32(g_FunListCmp256Ps[imm8].cmpFun(a.vect_f32[1], b.vect_f32[1]));
    return dst;
}

FORCE_INLINE __m256i _mm256_load_epi32 (void const* mem_addr)
{
    __m256i res;
    res.vect_s32[0] = vld1q_s32((const int32_t *)mem_addr);
    res.vect_s32[1] = vld1q_s32((const int32_t *)mem_addr + 4);
    return res;
}

FORCE_INLINE __m256i _mm256_load_epi64 (void const* mem_addr)
{
    __m256i res;
    res.vect_s64[0] = vld1q_s64((const int64_t *)mem_addr);
    res.vect_s64[1] = vld1q_s64((const int64_t *)mem_addr + 2);
    return res;
}

FORCE_INLINE __m256d _mm256_load_pd (double const * mem_addr)
{
    __m256d res;
    res.vect_f64[0] = vld1q_f64((const double *)mem_addr);
    res.vect_f64[1] = vld1q_f64((const double *)mem_addr + 2);
    return res;
}

FORCE_INLINE __m256 _mm256_load_ps (float const * mem_addr)
{
    __m256 res;
    res.vect_f32[0] = vld1q_f32((const float *)mem_addr);
    res.vect_f32[1] = vld1q_f32((const float *)mem_addr + 4);
    return res;
}

FORCE_INLINE void _mm256_store_epi32 (void* mem_addr, __m256i a)
{
    vst1q_s32((int32_t *)mem_addr, a.vect_s32[0]);
    vst1q_s32((int32_t *)mem_addr + 4, a.vect_s32[1]);
}

FORCE_INLINE void _mm256_store_epi64 (void* mem_addr, __m256i a)
{
    vst1q_s64((int64_t *)mem_addr, a.vect_s64[0]);
    vst1q_s64((int64_t *)mem_addr + 2, a.vect_s64[1]);
}

FORCE_INLINE void _mm256_store_pd (double * mem_addr, __m256d a)
{
    vst1q_f64(mem_addr, a.vect_f64[0]);
    vst1q_f64(mem_addr + 2, a.vect_f64[1]);
}

FORCE_INLINE void _mm256_store_ps (float * mem_addr, __m256 a)
{
    vst1q_f32(mem_addr, a.vect_f32[0]);
    vst1q_f32(mem_addr + 4, a.vect_f32[1]);
}

FORCE_INLINE void _mm256_storeu_ps(float* mem_addr, __m256 a)
{
    vst1q_f32(mem_addr, a.vect_f32[0]);
    vst1q_f32(mem_addr + 4, a.vect_f32[1]);
}

FORCE_INLINE __m256i _mm256_max_epi32 (__m256i a, __m256i b)
{
    __m256i res;
    res.vect_s32[0] = vmaxq_s32(a.vect_s32[0], b.vect_s32[0]);
    res.vect_s32[1] = vmaxq_s32(a.vect_s32[1], b.vect_s32[1]);
    return res;
}

FORCE_INLINE __m256i _mm256_packs_epi32 (__m256i a, __m256i b)
{
    __m256i res;
    res.vect_s16[0] = vcombine_s16(vqmovn_s32(a.vect_s32[0]), vqmovn_s32(b.vect_s32[0]));
    res.vect_s16[1] = vcombine_s16(vqmovn_s32(a.vect_s32[1]), vqmovn_s32(b.vect_s32[1]));
    return res;
}

// added by Bill

FORCE_INLINE __m256i _mm256_mul_epi32(__m256i a, __m256i b)
{
 __m256i res;
 res.vect_i128.val[0] =  _mm_mul_epi32(a.vect_i128.val[0], b.vect_i128.val[0]);
 res.vect_i128.val[1] =  _mm_mul_epi32(a.vect_i128.val[1], b.vect_i128.val[1]);
 return res;
}

FORCE_INLINE __m256i _mm256_mul_epu32(__m256i a, __m256i b)
{
 __m256i res;
 res.vect_i128.val[0] =  _mm_mul_epu32(a.vect_i128.val[0], b.vect_i128.val[0]);
 res.vect_i128.val[1] =  _mm_mul_epu32(a.vect_i128.val[1], b.vect_i128.val[1]);
 return res;
}

FORCE_INLINE __m256d _mm256_addsub_pd (__m256d a, __m256d b)
{
  __m256d res;
  __m128d c,d;
  int i;
  for(i=0; i<2; i++) {
  c = vsubq_f64( a.vect_f64[i] , b.vect_f64[i] );
  d = vaddq_f64( a.vect_f64[i] , b.vect_f64[i] );
  res.vect_f64[i][0] = c[0];
  res.vect_f64[i][1] = d[1];
  }
  return res;
}


FORCE_INLINE int _mm256_movemask_pd(__m256d a)
{
    static const int64x2_t shift = {-63, -62};
    static const uint32x4_t highbit = {0x00000000, 0x80000000, 0x00000000, 0x80000000};
    return (vaddvq_u64(vshlq_u64(vandq_u64(vreinterpretq_u32_f64(a.vect_f64[1]), vreinterpretq_u64_u32(highbit)), shift)) << 2) |
           vaddvq_u64(vshlq_u64(vandq_u64(vreinterpretq_u32_f64(a.vect_f64[0]), vreinterpretq_u64_u32(highbit)), shift));
}

FORCE_INLINE __m256d _mm256_xor_pd(__m256d a, __m256d b)
{
    __m256d res_m256d;
    res_m256d.vect_f64[0] = vreinterpretq_f64_u64(veorq_u64(
        vreinterpretq_u64_f64(a.vect_f64[0]), vreinterpretq_u64_f64(b.vect_f64[0])));
    res_m256d.vect_f64[1] = vreinterpretq_f64_u64(veorq_u64(
        vreinterpretq_u64_f64(a.vect_f64[1]), vreinterpretq_u64_f64(b.vect_f64[1])));
    return res_m256d;
}

FORCE_INLINE __m256d _mm256_and_pd(__m256d a, __m256d b)
{
    __m256d res_m256d;
    res_m256d.vect_f64[0] = vreinterpretq_f64_u64(vandq_u64(
        vreinterpretq_u64_f64(a.vect_f64[0]), vreinterpretq_u64_f64(b.vect_f64[0])));
    res_m256d.vect_f64[1] = vreinterpretq_f64_u64(vandq_u64(
        vreinterpretq_u64_f64(a.vect_f64[1]), vreinterpretq_u64_f64(b.vect_f64[1])));
    return res_m256d;
}

FORCE_INLINE __m256d _mm256_andnot_pd(__m256d a, __m256d b)
{
    __m256d res_m256d;
    res_m256d.vect_f64[0] = vreinterpretq_f64_u64(vbicq_u64(
        vreinterpretq_u64_f64(b.vect_f64[0]), vreinterpretq_u64_f64(a.vect_f64[0])));  // *NOTE* argument swap
    res_m256d.vect_f64[1] = vreinterpretq_f64_u64(vbicq_u64(
        vreinterpretq_u64_f64(b.vect_f64[1]), vreinterpretq_u64_f64(a.vect_f64[1])));  // *NOTE* argument swap
    return res_m256d;
}

FORCE_INLINE __m256d _mm256_sqrt_pd(__m256d in)
{
 __m256d res;
 res.vect_f64[0] = vsqrtq_f64(in.vect_f64[0]);
 res.vect_f64[1] = vsqrtq_f64(in.vect_f64[1]);
 return res;
}

FORCE_INLINE __m256 _mm256_castpd_ps(__m256d a)
{
    __m256 b;

    b.vect_f32[0] = vreinterpretq_f32_f64(a.vect_f64[0]);
    b.vect_f32[1] = vreinterpretq_f32_f64(a.vect_f64[1]);

    return b;
}

FORCE_INLINE __m256i _mm256_castpd_si256(__m256d a)
{
    __m256i b;

    b.vect_s64[0] = vreinterpretq_s64_f64(a.vect_f64[0]);
    b.vect_s64[1] = vreinterpretq_s64_f64(a.vect_f64[1]);

    return b;
}

FORCE_INLINE __m256d _mm256_castps_pd(__m256 a)
{
    __m256d b;

    b.vect_f64[0] = vreinterpretq_f64_f32(a.vect_f32[0]);
    b.vect_f64[1] = vreinterpretq_f64_f32(a.vect_f32[1]);

    return b;
}

FORCE_INLINE __m256i _mm256_castps_si256(__m256 a)
{
    __m256i b;

    b.vect_s64[0] = vreinterpretq_s64_f32(a.vect_f32[0]);
    b.vect_s64[1] = vreinterpretq_s64_f32(a.vect_f32[1]);

    return b;
}

FORCE_INLINE __m256d _mm256_castsi256_pd(__m256i a)
{
    __m256d b;

    b.vect_f64[0] = vreinterpretq_f64_s64(a.vect_s64[0]);
    b.vect_f64[1] = vreinterpretq_f64_s64(a.vect_s64[1]);

    return b;
}

FORCE_INLINE __m256d _mm256_loadu_pd (double const * mem_addr)
{
    __m256d res;
    res.vect_f64[0] = vld1q_f64((const double *)mem_addr);
    res.vect_f64[1] = vld1q_f64((const double *)mem_addr + 2);
    return res;
}

FORCE_INLINE __m256 _mm256_loadu_ps (float const * mem_addr)
{
    __m256 res;
    res.vect_f32[0] = vld1q_f32((const float *)mem_addr);
    res.vect_f32[1] = vld1q_f32((const float *)mem_addr + 4);
    return res;
}

FORCE_INLINE void _mm256_storeu_pd (double * mem_addr, __m256d a)
{
    vst1q_f64(mem_addr, a.vect_f64[0]);
    vst1q_f64(mem_addr + 2, a.vect_f64[1]);
}

FORCE_INLINE __m256d _mm256_max_pd (__m256d a, __m256d b)
{
    __m256d res;
    res.vect_f64[0] = vmaxq_f64(a.vect_f64[0], b.vect_f64[0]);
    res.vect_f64[1] = vmaxq_f64(a.vect_f64[1], b.vect_f64[1]);
    return res;
}

FORCE_INLINE __m256d _mm256_min_pd (__m256d a, __m256d b)
{
    __m256d res;
    res.vect_f64[0] = vminq_f64(a.vect_f64[0], b.vect_f64[0]);
    res.vect_f64[1] = vminq_f64(a.vect_f64[1], b.vect_f64[1]);
    return res;
}

FORCE_INLINE __m256i _mm256_cmpgt_epi64 (__m256i a, __m256i b)
{
    __m256i res;
    res.vect_u64[0] = vcgtq_s64(a.vect_s64[0], b.vect_s64[0]);
    res.vect_u64[1] = vcgtq_s64(a.vect_s64[1], b.vect_s64[1]);
    return res;
}

FORCE_INLINE __m256i _mm256_cmpeq_epi64 (__m256i a, __m256i b)
{
    __m256i res;
    res.vect_u64[0] = vceqq_s64(a.vect_s64[0], b.vect_s64[0]);
    res.vect_u64[1] = vceqq_s64(a.vect_s64[1], b.vect_s64[1]);
    return res;
}

#define _maskload_impl( name, vec_type, mask_vec_type, type, mask_type ) \
FORCE_INLINE vec_type  name(type const *a, mask_vec_type mask) {   \
    const size_t size_type = sizeof( type );                          \
    const size_t size = sizeof( vec_type ) / size_type;               \
    ALIGN_STRUCT(32) type res[ size ];                            \
    const mask_type* p_mask = (const mask_type*)&mask;                \
    size_t i = 0;                                                     \
    mask_type sign_bit = 1;                                           \
    sign_bit <<= (8*size_type - 1);                                   \
    for ( ; i < size; ++i )                                           \
        res[ i ] = (sign_bit & p_mask[i]) ? a[i] : 0;           \
    return (*(vec_type*)&res);                                        \
}

#define _maskstore_impl( name, vec_type, mask_vec_type, type, mask_type ) \
FORCE_INLINE void  name(type *a, mask_vec_type mask, vec_type data) { \
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

// _maskload_impl( _mm256_maskload_pd, __m256d, __m256i, double, int64_t );
// _maskstore_impl( _mm256_maskstore_pd, __m256d, __m256i, double, int64_t );
// _maskload_impl( _mm256_maskload_epi64, __m256i, __m256i, int64_t, int64_t );
// _maskstore_impl( _mm256_maskstore_epi64, __m256i, __m256i, int64_t, int64_t );

FORCE_INLINE __m256d _mm256_maskload_pd(double const* mem_addr, __m256i mask)
{
    __m256d ret;
   int i;

    for (i=0; i<2; i++){
      if (mask.vect_s64[0][i])
        ret.vect_f64[0][i] = mem_addr[i];
      else
        ret.vect_f64[0][i] = 0.0;
    }
    for (i=0; i<2; i++){
      if (mask.vect_s64[1][i])
        ret.vect_f64[1][i] = mem_addr[i + 2];
      else
        ret.vect_f64[1][i] = 0.0;
    }

    return ret;
}

FORCE_INLINE void _mm256_maskstore_pd(double * mem_addr, __m256i mask, __m256d a)
{
   int i;
    for (i=0; i<2; i++){
      if (mask.vect_s64[0][i])
         mem_addr[i] = a.vect_f64[0][i];
    }
    for (i=0; i<2; i++){
      if (mask.vect_s64[1][i])
         mem_addr[i + 2] = a.vect_f64[1][i];
    }
}

FORCE_INLINE __m256i _mm256_maskload_epi64(long long const* mem_addr, __m256i mask)
{
  __m256I res;
  res.m256d = _mm256_maskload_pd((double const*) mem_addr, mask);
  return res.m256i;
}

FORCE_INLINE void _mm256_maskstore_epi64(long long * mem_addr, __m256i mask, __m256i a)
{
 __m256I av;
 av.m256i = a;
_mm256_maskstore_pd((double *) mem_addr, mask, av.m256d);
}

#define _M256_IMPL2_M1I_SHIFT( type, func, shift_for_hi ) \
FORCE_INLINE type _mm256_##func( type m256_param1, const int param2 ) \
{   type res; \
    res.vect_i128.val[0] = _mm_##func( m256_param1.vect_i128.val[0], param2 & ((1<<shift_for_hi)-1) ); \
    res.vect_i128.val[1] = _mm_##func( m256_param1.vect_i128.val[1], param2 >> shift_for_hi); \
    return ( res ); \
}

// _M256_IMPL2_M1I_SHIFT( __m256d, permute_pd, 2 );

FORCE_INLINE __m256d _mm256_permute_pd (__m256d a, int imm8)
{
    __m256d dest;
    int i,j;
    for (j=0; j<2; j++)
     for (i=0; i<2; i++)
      dest.vect_f64[j][i] = a.vect_f64[j][ 1 & (imm8 >> (i+j*2)) ];
    return dest;
}

FORCE_INLINE __m256d _mm256_permute2f128_pd (__m256d a, __m256d b, int imm8)
{
    __m256d dest;
    if ( imm8 & 0x8) 
      dest.vect_f64[0] = vdupq_n_f64(0.0);
    else {
      switch (imm8 & 0x7) {
      case 0: dest.vect_f64[0] = a.vect_f64[0]; break;
      case 1: dest.vect_f64[0] = a.vect_f64[1]; break;
      case 2: dest.vect_f64[0] = b.vect_f64[0]; break;
      case 3: dest.vect_f64[0] = b.vect_f64[1]; break;
      }
    }
    if (( imm8 >> 4) & 0x8) 
      dest.vect_f64[1] = vdupq_n_f64(0.0);
    else {
      switch ((imm8 >> 4) & 0x7) {
      case 0: dest.vect_f64[1] = a.vect_f64[0]; break;
      case 1: dest.vect_f64[1] = a.vect_f64[1]; break;
      case 2: dest.vect_f64[1] = b.vect_f64[0]; break;
      case 3: dest.vect_f64[1] = b.vect_f64[1]; break;
      }
    }
    return dest;
}

FORCE_INLINE int _mm256_movemask_epi8(__m256i a)
{
    return
        (_mm_movemask_epi8( a.vect_s64[1] ) << 16) |
         _mm_movemask_epi8( a.vect_s64[0] );
}

FORCE_INLINE __m256i _mm256_permute2x128_si256 (__m256i a, __m256i b, const int imm8) {
 __m256i A;
 __m256i av;
 __m256i bv;
	switch (imm8 & 0b11) {
	case 0:	A.vect_s64[0]  = a.vect_s64[0] ; break ;
	case 1:	A.vect_s64[0]  = a.vect_s64[1] ; break ;
	case 2:	A.vect_s64[0]  = b.vect_s64[0] ; break ;
	case 3:	A.vect_s64[0]  = b.vect_s64[1] ; break ;
	}
	if (imm8 & 0b1000)
  A.vect_s64[0] = vdupq_n_s64(0);

	switch ((imm8 >> 4) & 0b11) {
	case 0:	A.vect_s64[1]  = a.vect_s64[0] ; break ;
	case 1:	A.vect_s64[1]  = a.vect_s64[1] ; break ;
	case 2:	A.vect_s64[1]  = b.vect_s64[0] ; break ;
	case 3:	A.vect_s64[1]  = b.vect_s64[1] ; break ;
	}
	if (imm8 & 0b10000000)
  A.vect_s64[1] = vdupq_n_s64(0);

 return A;
}

FORCE_INLINE __m256i _mm256_i64gather_epi64(long long const *addr, __m256i index, const int scale) {
__m256i B;
 int64_t *p = (int64_t*) &B;
 p[0] = *(int64_t*)((int8_t*)addr + (index.vect_s64[ 0 ])[0] * scale);
 p[1] = *(int64_t*)((int8_t*)addr + (index.vect_s64[ 0 ])[1] * scale);
 p[2] = *(int64_t*)((int8_t*)addr + (index.vect_s64[ 1 ])[0] * scale);
 p[3] = *(int64_t*)((int8_t*)addr + (index.vect_s64[ 1 ])[1] * scale);
 return B;
}

FORCE_INLINE __m256i _mm256_mask_i64gather_epi64( __m256i src, long long const *addr, __m256i index, __m256i mask, const int scale) {
 __m256i B;
 int64_t *p = (int64_t*) &B;
 int64_t *m = (int64_t*) &mask;
 p[0] = m[0] ? *(int64_t*)((int8_t*)addr + (index.vect_s64[ 0 ])[0] * scale) : (src.vect_s64[0])[0];
 p[1] = m[1] ? *(int64_t*)((int8_t*)addr + (index.vect_s64[ 0 ])[1] * scale) : (src.vect_s64[0])[1];
 p[2] = m[2] ? *(int64_t*)((int8_t*)addr + (index.vect_s64[ 1 ])[0] * scale) : (src.vect_s64[1])[0];
 p[3] = m[3] ? *(int64_t*)((int8_t*)addr + (index.vect_s64[ 1 ])[1] * scale) : (src.vect_s64[1])[1];
 return B;
}

FORCE_INLINE __m256d _mm256_i64gather_pd(double const *addr, __m256i index, const int scale) {
__m256d B;
 double *p = (double*) &B;
 p[0] = *(double*)((int8_t*)addr + (index.vect_s64[ 0 ])[0] * scale);
 p[1] = *(double*)((int8_t*)addr + (index.vect_s64[ 0 ])[1] * scale);
 p[2] = *(double*)((int8_t*)addr + (index.vect_s64[ 1 ])[0] * scale);
 p[3] = *(double*)((int8_t*)addr + (index.vect_s64[ 1 ])[1] * scale);
 return B;
}

#if 0
#define _mm_load_impl( name, sfx, m256_sfx, m256_type, type_128, type )           \
FORCE_INLINE m256_type  _mm256_##name##_##m256_sfx(const type* a) { \
   #m256_type res;                                                              \
    res.vect_i128.val[0] = _mm_##name##_##sfx( (const type_128 *)a );                     \
    res.vect_i128.val[1] = _mm_##name##_##sfx( (const type_128 *)(1+(const __m128 *)a) ); \
    return (res);                                                                      \
}

#define _mm_store_impl( name, sfx, m256_sfx, m256_type, type_128, type ) \
FORCE_INLINE void _mm256_##name##_##m256_sfx(type *a, m256_type b) {  \
    _mm_##name##_##sfx( (type_128*)a, b.vect_i128.val[0] );                                 \
    _mm_##name##_##sfx( (type_128*)(1+(__m128*)a), b.vect_i128.val[1] );   \
}

_mm_load_impl( load, pd, pd, __m256d, double, double );
_mm_store_impl( store, pd, pd, __m256d, double, double );

_mm_load_impl( load, ps, ps, __m256, float, float );
_mm_store_impl( store, ps, ps, __m256, float, float );

_mm_load_impl( loadu, pd, pd, __m256d, double, double );
_mm_store_impl( storeu, pd, pd, __m256d, double, double );

#endif

FORCE_INLINE __m256i _mm256_broadcastb_epi8 ( __m128i a )
{
//     __m256i A;
//     __m128i av = a;
//     memset(&A, *(int8_t*)&av, 32);
//     return A;
  __m256i A;
  A.vect_u8[1]=A.vect_u8[0]=vdupq_n_u8(a[0]);
  return A;
}

#define _mm256_shuffle_pd( m256_param1, m256_param2, param3 ) \
({   __m256d res; \
    res.vect_i128.val[0] = _mm_shuffle_pd( m256_param1.vect_i128.val[0], m256_param2.vect_i128.val[0], (param3) & ((1<<2)-1) ); \
    res.vect_i128.val[1] = _mm_shuffle_pd( m256_param1.vect_i128.val[1], m256_param2.vect_i128.val[1], (param3) >> 2 ); \
    res; \
})
#define _mm256_shuffle_ps( m256_param1, m256_param2, param3 ) \
({   __m256 res; \
    res.vect_i128.val[0] = _mm_shuffle_ps( m256_param1.vect_i128.val[0], m256_param2.vect_i128.val[0], param3 ); \
    res.vect_i128.val[1] = _mm_shuffle_ps( m256_param1.vect_i128.val[1], m256_param2.vect_i128.val[1], param3 ); \
    res; \
})

FORCE_INLINE __m256d _mm256_fmadd_pd(__m256d a, __m256d b, __m256d c)
{
    __m256d res;
    res.vect_f64[0] = vfmaq_f64(c.vect_f64[0], a.vect_f64[0], b.vect_f64[0]);  // *NOTE* argument swap
    res.vect_f64[1] = vfmaq_f64(c.vect_f64[1], a.vect_f64[1], b.vect_f64[1]);  // *NOTE* argument swap
    return res;
}

FORCE_INLINE __m256d _mm256_fnmadd_pd(__m256d a, __m256d b, __m256d c)
{
    __m256d res;
    res.vect_f64[0] = vmlsq_f64(c.vect_f64[0], a.vect_f64[0], b.vect_f64[0]);  // *NOTE* argument swap
    res.vect_f64[1] = vmlsq_f64(c.vect_f64[1], a.vect_f64[1], b.vect_f64[1]);  // *NOTE* argument swap
    return res;
}

FORCE_INLINE __m256 _mm256_fmadd_ps(__m256 a, __m256 b, __m256 c)
{
    return _mm256_add_ps(_mm256_mul_ps(a,b),c);
}

FORCE_INLINE __m256d _mm256_fmsub_pd(__m256d a, __m256d b, __m256d c)
{
    __m256d res;
    res.vect_f64[0] = vfmsq_f64(c.vect_f64[0], a.vect_f64[0], b.vect_f64[0]);  // *NOTE* argument swap
    res.vect_f64[1] = vfmsq_f64(c.vect_f64[1], a.vect_f64[1], b.vect_f64[1]);  // *NOTE* argument swap
    return res;
}

FORCE_INLINE __m256 _mm256_fmsub_ps(__m256 a, __m256 b, __m256 c)
{
    return _mm256_sub_ps(_mm256_mul_ps(a,b),c);
}


#if defined(__GNUC__) || defined(__clang__)
#pragma pop_macro("ALIGN_STRUCT")
#pragma pop_macro("FORCE_INLINE")
#endif
