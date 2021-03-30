#ifndef SSE2NEON2_H
#define SSE2NEON2_H

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

#include <stdint.h>
#include <stdlib.h>

#include <arm_neon.h>

#undef SET32x4
#define SET32x4(res, e0, e1, e2, e3)                     \
    __asm__ __volatile__ (                                  \
        "mov %[r].s[0], %w[x]        \n\t"                  \
        "mov %[r].s[1], %w[y]        \n\t"                  \
        "mov %[r].s[2], %w[z]        \n\t"                  \
        "mov %[r].s[3], %w[k]        \n\t"                  \
        :[r]"=w"(res)                                       \
        :[x]"r"(e0), [y]"r"(e1), [z]"r"(e2), [k]"r"(e3)     \
    );

#undef SET64x2
#define  SET64x2(res, e0, e1)                            \
    __asm__ __volatile__ (                                  \
        "mov %[r].d[0], %[x]         \n\t"                  \
        "mov %[r].d[1], %[y]         \n\t"                  \
        :[r]"=w"(res)                                       \
        :[x]"r"(e0), [y]"r"(e1)                             \
    );

FORCE_INLINE __m128d _mm_permute_pd(__m128d a, int control)
{
    double const* src = (double const*)&a;
    __m128d dest;
    int i=0;

    for (; i<2; ++i)
        dest[i] = src[ 1 & (control >> i) ];

    return dest;
}

#define _mm_permute_ps(a, control) _mm_castsi128_ps( _mm_shuffle_epi32( *(__m128i*)&a, control ) )

// FORCE_INLINE __m128d _mm_xor_pd(__m128d a, __m128d b)
// {
//     return vreinterpretq_f64_s32(
//         veorq_s32(vreinterpretq_s32_f64(a), vreinterpretq_s32_f64(b)));
// }

// FORCE_INLINE __m128d _mm_sub_pd(__m128d a, __m128d b)
// {
//     return vsubq_f64(a, b);
// }

// FORCE_INLINE __m128d _mm_mul_pd(__m128d a, __m128d b)
// {
//     return vmulq_f64(a, b);
// }

// Reads the lower 64 bits of b and stores them into the lower 64 bits of a.
// FORCE_INLINE void _mm_storel_pd(double * mem_addr, __m128d a)
// {
//     *mem_addr = vget_low_u64(vreinterpretq_u64_f64(a));
// }

// FORCE_INLINE __m128d _mm_set_pd(double e1, double e0)
// {
//     __m128d res;
//     SET64x2(res, e0, e1);
//     return res;
// }

// FORCE_INLINE __m128d _mm_set1_pd(double a)
// {
//     return vdupq_n_f64(a);
// }

// emmintrin

FORCE_INLINE __m128i _mm_div_epi8(__m128i a, __m128i b)
{
    __m128i res_m128i;
    int16x8_t ta[2], tb[2];
    int16x8_t tmp_lo, tmp_hi;
    int32x4_t la[4], lb[4];
    float32x4_t res[4];

    ta[0] = vmovl_s8(vget_low_s8(a));
    tb[0] = vmovl_s8(vget_low_s8(b));
    ta[1] = vmovl_s8(vget_high_s8(a));
    tb[1] = vmovl_s8(vget_high_s8(b));

    la[0] = vmovl_s16(vget_low_s16(ta[0]));
    lb[0] = vmovl_s16(vget_low_s16(tb[0]));
    la[1] = vmovl_s16(vget_high_s16(ta[0]));
    lb[1] = vmovl_s16(vget_high_s16(tb[0]));
    la[2] = vmovl_s16(vget_low_s16(ta[1]));
    lb[2] = vmovl_s16(vget_low_s16(tb[1]));
    la[3] = vmovl_s16(vget_high_s16(ta[1]));
    lb[3] = vmovl_s16(vget_high_s16(tb[1]));

    res[1] = vdivq_f32(vcvtq_f32_s32(la[1]), vcvtq_f32_s32(lb[1]));
    res[2] = vdivq_f32(vcvtq_f32_s32(la[2]), vcvtq_f32_s32(lb[2]));
    res[0] = vdivq_f32(vcvtq_f32_s32(la[0]), vcvtq_f32_s32(lb[0]));
    res[3] = vdivq_f32(vcvtq_f32_s32(la[3]), vcvtq_f32_s32(lb[3]));
    tmp_lo = vcombine_s16(vmovn_s32(vcvtq_s32_f32(res[0])), vmovn_s32(vcvtq_s32_f32(res[1])));
    tmp_hi = vcombine_s16(vmovn_s32(vcvtq_s32_f32(res[2])), vmovn_s32(vcvtq_s32_f32(res[3])));
    res_m128i = vcombine_s8(vmovn_s16(tmp_lo), vmovn_s16(tmp_hi));

    return res_m128i;
}

FORCE_INLINE __m128i _mm_div_epi16(__m128i a, __m128i b)
{
    __m128i res;
    float32x4_t fa[2], fb[2];
    float32x4_t res_lo, res_hi;
    fa[0] = vcvtq_f32_s32(vmovl_s16(vget_low_s16(a)));
    fb[0] = vcvtq_f32_s32(vmovl_s16(vget_low_s16(b)));    
    fa[1] = vcvtq_f32_s32(vmovl_s16(vget_high_s16(a)));
    fb[1] = vcvtq_f32_s32(vmovl_s16(vget_high_s16(b)));
    res_lo = vdivq_f32(fa[0], fb[0]);
    res_hi = vdivq_f32(fa[1], fb[1]);
    res = vcombine_s16(vmovn_s32(vcvtq_s32_f32(res_lo)), vmovn_s32(vcvtq_s32_f32(res_hi)));
    return res;
}

FORCE_INLINE __m128i _mm_div_epu8(__m128i a, __m128i b)
{
    __m128i res;
    uint16x8_t ta[2], tb[2];
    uint16x8_t tmp_lo, tmp_hi;
    uint32x4_t la[4], lb[4];
    float32x4_t res32[4];

    ta[0] = vmovl_u8(vget_low_u8(a));
    tb[0] = vmovl_u8(vget_low_u8(b));
    ta[1] = vmovl_u8(vget_high_u8(a));
    tb[1] = vmovl_u8(vget_high_u8(b));

    la[0] = vmovl_u16(vget_low_u16(ta[0]));
    lb[0] = vmovl_u16(vget_low_u16(tb[0]));
    la[1] = vmovl_u16(vget_high_u16(ta[0]));
    lb[1] = vmovl_u16(vget_high_u16(tb[0]));
    la[2] = vmovl_u16(vget_low_u16(ta[1]));
    lb[2] = vmovl_u16(vget_low_u16(tb[1]));
    la[3] = vmovl_u16(vget_high_u16(ta[1]));
    lb[3] = vmovl_u16(vget_high_u16(tb[1]));

    res32[1] = vdivq_f32(vcvtq_f32_u32(la[1]), vcvtq_f32_u32(lb[1]));
    res32[2] = vdivq_f32(vcvtq_f32_u32(la[2]), vcvtq_f32_u32(lb[2]));
    res32[0] = vdivq_f32(vcvtq_f32_u32(la[0]), vcvtq_f32_u32(lb[0]));
    res32[3] = vdivq_f32(vcvtq_f32_u32(la[3]), vcvtq_f32_u32(lb[3]));
    tmp_lo = vcombine_u16(vmovn_u32(vcvtq_u32_f32(res32[0])), vmovn_u32(vcvtq_u32_f32(res32[1])));
    tmp_hi = vcombine_u16(vmovn_u32(vcvtq_u32_f32(res32[2])), vmovn_u32(vcvtq_u32_f32(res32[3])));
    res = vcombine_u8(vmovn_u16(tmp_lo), vmovn_u16(tmp_hi));

    return res;
}

FORCE_INLINE __m128i _mm_div_epu16(__m128i a, __m128i b)
{
    __m128i res;
    float32x4_t fa[2], fb[2];
    float32x4_t res_lo, res_hi;
    fa[0] = vcvtq_f32_u32(vmovl_u16(vget_low_u16(a)));
    fb[0] = vcvtq_f32_u32(vmovl_u16(vget_low_u16(b)));    
    fa[1] = vcvtq_f32_u32(vmovl_u16(vget_high_u16(a)));
    fb[1] = vcvtq_f32_u32(vmovl_u16(vget_high_u16(b)));
    res_lo = vdivq_f32(fa[0], fb[0]);
    res_hi = vdivq_f32(fa[1], fb[1]);
    res = vcombine_u16(vmovn_u32(vcvtq_u32_f32(res_lo)), vmovn_u32(vcvtq_u32_f32(res_hi)));
    return res;
}

// Cast vector of type __m128d to type __m128. This intrinsic is only used for
// compilation and does not generate any instructions, thus it has zero latency.
// https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_castpd_ps
// FORCE_INLINE __m128 _mm_castpd_ps(__m128d a)
// {
//     return vreinterpretq_m128_s64(vreinterpretq_s64_m128d(a));
// }

#if defined(__GNUC__) || defined(__clang__)
#pragma pop_macro("ALIGN_STRUCT")
#pragma pop_macro("FORCE_INLINE")
#endif

#endif  // SSE2NEON2_H
