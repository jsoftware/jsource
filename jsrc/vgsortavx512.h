#define PCNT __builtin_popcountll
#define V __m512i
#define VH __m256i
#define M1 __mmask8
#define M2 __mmask16
#define M4 __mmask32
#define VL _mm512_loadu_epi64
#define VS _mm512_storeu_epi64
#define VSNT _mm512_stream_si512
#define CMPLS8 _mm512_cmplt_epi64_mask
#define CMPLU4 _mm512_cmplt_epu32_mask
#define CMPLMS8(x,y,m) _mm512_mask_cmplt_epi64_mask(m,x,y)
#define CMPLMU4(x,y,m) _mm512_mask_cmplt_epu32_mask(m,x,y)
#define CMPGS8 _mm512_cmpge_epi64_mask
#define CMPGU4 _mm512_cmpge_epu32_mask
#define CMPGMS8(x,y,m) _mm512_mask_cmpge_epi64_mask(m,x,y)
#define CMPGMU4(x,y,m) _mm512_mask_cmpge_epu32_mask(m,x,y)
#define COMP8(x,m) _mm512_maskz_compress_epi64(m,x)
#define COMP4(x,m) _mm512_maskz_compress_epi32(m,x)
#define COMPS8(p,m,x) _mm512_mask_compressstoreu_epi64(p,m,x)
#define COMPS4(p,m,x) _mm512_mask_compressstoreu_epi32(p,m,x)
#define VMINS8 _mm512_min_epi64
#define VMINU4 _mm512_min_epu32
#define VMINMS8(x,y,m) _mm512_mask_min_epi64(x,m,x,y)
#define VMINMU4(x,y,m) _mm512_mask_min_epu32(x,m,x,y)
#define VMAXS8 _mm512_max_epi64
#define VMAXU4 _mm512_max_epu32
#define VMAXMS8(x,y,m) _mm512_mask_max_epi64(x,m,x,y)
#define VMAXMU4(x,y,m) _mm512_mask_max_epu32(x,m,x,y)
#define VMINRS8 _mm512_reduce_min_epi64
#define VMINRU4 _mm512_reduce_min_epu32
#define VMAXRS8 _mm512_reduce_max_epi64
#define VMAXRU4 _mm512_reduce_max_epu32
#define VLM8(p,m) _mm512_maskz_loadu_epi64(m,p)
#define VLM4(p,m) _mm512_maskz_loadu_epi32(m,p)
#define VLM2(p,m) _mm512_maskz_loadu_epi16(m,p)
#define VLMM8(s,p,m) _mm512_mask_loadu_epi64(s,m,p)
#define VBC8 _mm512_set1_epi64
#define VBC4 _mm512_set1_epi32
#define VBC2 _mm512_set1_epi16
#define VSUB8 _mm512_sub_epi64

#include "vgsortinavx512.h"

static INLINE void mask28(I n,M1 *m0,M1 *m1) {
 *m0=BZHI(0xff,n);
 // or maybe should be BZHI(0xff,n-8)&~(n>>63) or something?
 *m1=(n-8)<0?0:BZHI(0xff,n-8);}
static INLINE void mask48(I n,M1 *m0,M1 *m1,M1 *m2,M1 *m3) {
 *m0=BZHI(0xff,n);
 *m1=(n- 8)<0?0:BZHI(0xff,n- 8);
 *m2=(n-16)<0?0:BZHI(0xff,n-16);
 *m3=(n-24)<0?0:BZHI(0xff,n-24);}
