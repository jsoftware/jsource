/* Copyright (c) 1990-2026, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* string utiliy                                                           */

#include "j.h"

#include <stddef.h>
#include <ctype.h>

extern void StringToLower(char *str,size_t len);
extern void StringToUpper(char *str,size_t len);
extern void StringToLowerUCS2(unsigned short *str,size_t len);
extern void StringToUpperUCS2(unsigned short *str,size_t len);
extern void StringToLowerUCS4(unsigned int *str,size_t len);
extern void StringToUpperUCS4(unsigned int *str,size_t len);
extern size_t Stringlchr(char *str,char ch, size_t stride,size_t len,size_t klen,size_t *pi);
extern size_t Stringlchr2(unsigned short *str, unsigned short ch, size_t stride,size_t len,size_t klen,size_t *pi);
extern size_t Stringlchr4(unsigned int *str, unsigned int ch, size_t stride,size_t len,size_t klen,size_t *pi);
extern size_t Stringrchr(char *str,char ch, size_t stride,size_t len);
extern size_t Stringrchr2(unsigned short *str, unsigned short ch, size_t stride,size_t len);
extern size_t Stringrchr4(unsigned int *str, unsigned int ch, size_t stride,size_t len);

#define OMP_THRESHOLD 64
#define OMP_THREADS 4      /* bottleneck is memory bus contention */
#undef _OPENMP             /* disable openmp. actually openmp runs slower */

/* use of sse2/avx2 seems negligible improvement, perhaps compiler already has excellent loop optimization */

// -------------------------------------------------------
// slchr

static size_t slchr(char* str, char ch, size_t len){
 size_t i=0;

#if C_AVX2 || EMU_AVX2
 // align to 32 bytes
 while ((i<len) && ((((intptr_t)(str+i)) & 31) != 0)){if (ch!=str[i]) return i; else ++i;}
/* don't test i>=0 which is always true because size_t is unsigned */
 const __m256i mm0 = _mm256_set1_epi8( ch );
 const __m256i mm2 = _mm256_set1_epi8( 0xff );
 while (len >= i+32) {
  // search for ch
  int mask = 0;
   __m256i mm1 = _mm256_load_si256((__m256i *)(str+i));
   mm1 = _mm256_andnot_si256(_mm256_cmpeq_epi8(mm1, mm0),mm2);
   if ((mask = _mm256_movemask_epi8(mm1)) != 0) {   // some character is not ch
    // got 0 somewhere within 32 bytes in mm1, or within 32 bits in mask
    // find index of last set bit
#if (MMSC_VER)   // make sure <intrin.h> is included
    unsigned long pos;
    _BitScanForward(&pos, mask);
    i += (size_t)pos;
#elif defined(__clang__) || ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))) // modern GCC has built-in
    i += __builtin_ctz(mask);
#else  // none of choices exist, use local BSR implementation
#error __builtin_ctz
#endif
    return i;
  }
  i += 32;
 }

#elif defined(__SSE2__)
 // align to 16 bytes
 while ((i<len) && ((((intptr_t)(str+i)) & 15) != 0)){if (ch!=str[i]) return i; else ++i;}
 const __m128i xmm0 = _mm_set1_epi8( ch );
 const __m128i xmm2 = _mm_set1_epi8( 0xff );
 while (len >= i+16) {
  // search for ch
  int mask = 0;
   __m128i xmm1 = _mm_load_si128((__m128i *)(str+i));
   xmm1 = _mm_andnot_si128(_mm_cmpeq_epi8(xmm1, xmm0),xmm2);
   if ((mask = _mm_movemask_epi8(xmm1)) != 0) {   // some character is not ch
    // got 0 somewhere within 16 bytes in xmm1, or within 16 bits in mask
    // find index of last set bit
#if (MMSC_VER)   // make sure <intrin.h> is included
    unsigned long pos;
    _BitScanForward(&pos, mask);
    i += (size_t)pos;
#elif defined(__clang__) || ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))) // modern GCC has built-in
    i += __builtin_ctz(mask);  // mask is 32-bits but only lower 16-bits are significant
#else  // none of choices exist, use local BSR implementation
#error __builtin_ctz
#endif
    return i;
  }
  i += 16;
 }
#endif

 while (len>i){if (ch!=str[i]) return i; else ++i;}
 return len;
}

static size_t slchr2(unsigned short* str, unsigned short ch, size_t len){
 size_t i=0;

#if C_AVX2 || EMU_AVX2
 // align to 32 bytes
 while ((i<len) && ((((intptr_t)(str+i)) & 31) != 0)){if (ch!=str[i]) return i; else ++i;}
/* don't test i>=0 which is always true because size_t is unsigned */
 const __m256i mm0 = _mm256_set1_epi16( ch );
 const __m256i mm2 = _mm256_set1_epi16( 0xffff );
 while (len >= i+16) {
  // search for ch
  int mask = 0;
   __m256i mm1 = _mm256_load_si256((__m256i *)(str+i));
   mm1 = _mm256_andnot_si256(_mm256_cmpeq_epi16(mm1, mm0),mm2);
   if ((mask = _mm256_movemask_epi8(mm1)) != 0) {   // some character is not ch
    // got 0 somewhere within 32 bytes in mm1, or within 32 bits in mask
    // find index of last set bit
#if (MMSC_VER)   // make sure <intrin.h> is included
    unsigned long pos;
    _BitScanForward(&pos, mask);
    i += ((size_t)pos)>>1;
#elif defined(__clang__) || ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))) // modern GCC has built-in
    i += (__builtin_ctz(mask))>>1;
#else  // none of choices exist, use local BSR implementation
#error __builtin_ctz
#endif
    return i;
  }
  i += 16;
 }

#elif defined(__SSE2__)
 // align to 16 bytes
 while ((i<len) && ((((intptr_t)(str+i)) & 15) != 0)){if (ch!=str[i]) return i; else ++i;}
 const __m128i xmm0 = _mm_set1_epi16( ch );
 const __m128i xmm2 = _mm_set1_epi16( 0xffff );
 while (len >= i+8) {
  // search for ch
  int mask = 0;
   __m128i xmm1 = _mm_load_si128((__m128i *)(str+i));
   xmm1 = _mm_andnot_si128(_mm_cmpeq_epi16(xmm1, xmm0),xmm2);
   if ((mask = _mm_movemask_epi8(xmm1)) != 0) {   // some character is not ch
    // got 0 somewhere within 16 bytes in xmm1, or within 16 bits in mask
    // find index of last set bit
#if (MMSC_VER)   // make sure <intrin.h> is included
    unsigned long pos;
    _BitScanForward(&pos, mask);
    i += ((size_t)pos)>>1;
#elif defined(__clang__) || ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))) // modern GCC has built-in
    i += (__builtin_ctz(mask))>>1;  // mask is 32-bits but only lower 16-bits are significant
#else  // none of choices exist, use local BSR implementation
#error __builtin_ctz
#endif
    return i;
  }
  i += 8;
 }
#endif

 while (len>i){if (ch!=str[i]) return i; else ++i;}
 return len;
}

static size_t slchr4(unsigned int* str, unsigned int ch, size_t len){
 size_t i=0;

#if C_AVX2 || EMU_AVX2
 // align to 32 bytes
 while ((i<len) && ((((intptr_t)(str+i)) & 31) != 0)){if (ch!=str[i]) return i; else ++i;}
/* don't test i>=0 which is always true because size_t is unsigned */
 const __m256i mm0 = _mm256_set1_epi32( ch );
 const __m256i mm2 = _mm256_set1_epi32( 0xffffffff );
 while (len >= i+8) {
  // search for ch
  int mask = 0;
   __m256i mm1 = _mm256_load_si256((__m256i *)(str+i));
   mm1 = _mm256_andnot_si256(_mm256_cmpeq_epi32(mm1, mm0),mm2);
   if ((mask = _mm256_movemask_epi8(mm1)) != 0) {   // some character is not ch
    // got 0 somewhere within 32 bytes in mm1, or within 32 bits in mask
    // find index of last set bit
#if (MMSC_VER)   // make sure <intrin.h> is included
    unsigned long pos;
    _BitScanForward(&pos, mask);
    i += ((size_t)pos)>>2;
#elif defined(__clang__) || ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))) // modern GCC has built-in
    i += (__builtin_ctz(mask))>>2;
#else  // none of choices exist, use local BSR implementation
#error __builtin_ctz
#endif
    return i;
  }
  i += 8;
 }

#elif defined(__SSE2__)
 // align to 16 bytes
 while ((i<len) && ((((intptr_t)(str+i)) & 15) != 0)){if (ch!=str[i]) return i; else ++i;}
 const __m128i xmm0 = _mm_set1_epi32( ch );
 const __m128i xmm2 = _mm_set1_epi32( 0xffffffff );
 while (len >= i+4) {
  // search for ch
  int mask = 0;
   __m128i xmm1 = _mm_load_si128((__m128i *)(str+i));
   xmm1 = _mm_andnot_si128(_mm_cmpeq_epi32(xmm1, xmm0),xmm2);
   if ((mask = _mm_movemask_epi8(xmm1)) != 0) {   // some character is not ch
    // got 0 somewhere within 16 bytes in xmm1, or within 16 bits in mask
    // find index of last set bit
#if (MMSC_VER)   // make sure <intrin.h> is included
    unsigned long pos;
    _BitScanForward(&pos, mask);
    i += ((size_t)pos)>>2;
#elif defined(__clang__) || ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))) // modern GCC has built-in
    i += (__builtin_ctz(mask))>>2;  // mask is 32-bits but only lower 16-bits are significant
#else  // none of choices exist, use local BSR implementation
#error __builtin_ctz
#endif
    return i;
  }
  i += 4;
 }
#endif

 while (len>i){if (ch!=str[i]) return i; else ++i;}
 return len;
}

// -------------------------------------------------------
// srchr

static size_t srchr(char* str, char ch, size_t len){
 size_t i=len;

#if C_AVX2 || EMU_AVX2
 // align to 32 bytes
 while ((i>0) && ((((intptr_t)(str+i)) & 31) != 0)){if (ch!=str[i-1]) return i; else --i;}
/* don't test i>=0 which is always true because size_t is unsigned */
 const __m256i mm0 = _mm256_set1_epi8( ch );
 const __m256i mm2 = _mm256_set1_epi8( 0xff );
 while (i >= 32) {
  // search for ch
  int mask = 0;
   __m256i mm1 = _mm256_load_si256((__m256i *)(str+i-32));
   mm1 = _mm256_andnot_si256(_mm256_cmpeq_epi8(mm1, mm0),mm2);
   if ((mask = _mm256_movemask_epi8(mm1)) != 0) {   // some character is not ch
    // got 0 somewhere within 32 bytes in mm1, or within 32 bits in mask
    // find index of last set bit
#if (MMSC_VER)   // make sure <intrin.h> is included
    unsigned long pos;
    _BitScanBackward(&pos, mask);
    i -= (size_t)pos;
#elif defined(__clang__) || ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))) // modern GCC has built-in
    i -= __builtin_clz(mask);
#else  // none of choices exist, use local BSR implementation
#error __builtin_clz
#endif
    return i;
  }
  i -= 32;
 }

#elif defined(__SSE2__)
 // align to 16 bytes
 while ((i>0) && ((((intptr_t)(str+i)) & 15) != 0)){if (ch!=str[i-1]) return i; else --i;}
 const __m128i xmm0 = _mm_set1_epi8( ch );
 const __m128i xmm2 = _mm_set1_epi8( 0xff );
 while (i >= 16) {
  // search for ch
  int mask = 0;
   __m128i xmm1 = _mm_load_si128((__m128i *)(str+i-16));
   xmm1 = _mm_andnot_si128(_mm_cmpeq_epi8(xmm1, xmm0),xmm2);
   if ((mask = _mm_movemask_epi8(xmm1)) != 0) {   // some character is not ch
    // got 0 somewhere within 16 bytes in xmm1, or within 16 bits in mask
    // find index of last set bit
#if (MMSC_VER)   // make sure <intrin.h> is included
    unsigned long pos;
    _BitScanBackward(&pos, mask);
    i -= (size_t)pos-16;
#elif defined(__clang__) || ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))) // modern GCC has built-in __builtin_clz
    i -= __builtin_clz(mask)-16;  // mask is 32-bits but only lower 16-bits are significant
#else  // none of choices exist, use local BSR implementation
#error __builtin_clz
#endif
    return i;
  }
  i -= 16;
 }
#endif

 while (i>0){if (ch!=str[i-1]) return i; else --i;}
 return 0;
}

static size_t srchr2(unsigned short* str, unsigned short ch, size_t len){
 size_t i=len;

#if C_AVX2 || EMU_AVX2
 // align to 32 bytes
 while ((i>0) && ((((intptr_t)(str+i)) & 31) != 0)){if (ch!=str[i-1]) return i; else --i;}
/* don't test i>=0 which is always true because size_t is unsigned */
 const __m256i mm0 = _mm256_set1_epi16( ch );
 const __m256i mm2 = _mm256_set1_epi16( 0xffff );
 while (i >= 16) {
  // search for ch
  int mask = 0;
   __m256i mm1 = _mm256_load_si256((__m256i *)(str+i-16));
   mm1 = _mm256_andnot_si256(_mm256_cmpeq_epi16(mm1, mm0),mm2);
   // no such thing as _mm256_movemask_epi16
   // Shift each 16-bit element to the right by 8 bits, zero-filling the upper
   // bits.  This will remove the leading high byte from coming up in the mask
   // we generate below, allowing us to use popcount to get the number of slots
   // to compare in the subsequent step.
   if ((mask = _mm256_movemask_epi8(_mm256_srli_epi16(mm1, 8))) != 0) {   // some character is not ch
    // got 0 somewhere within 32 bytes in mm1, or within 32 bits in mask
    // find index of last set bit
#if (MMSC_VER)   // make sure <intrin.h> is included
    unsigned long pos;
    _BitScanBackward(&pos, mask);
    i -= ((size_t)pos)>>1;
#elif defined(__clang__) || ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))) // modern GCC has built-in
    i -= (__builtin_clz(mask))>>1;
#else  // none of choices exist, use local BSR implementation
#error __builtin_clz
#endif
    return i;
  }
  i -= 16;
 }

#elif defined(__SSE2__)
 // align to 16 bytes
 while ((i>0) && ((((intptr_t)(str+i)) & 15) != 0)){if (ch!=str[i-1]) return i; else --i;}
 const __m128i xmm0 = _mm_set1_epi16( ch );
 const __m128i xmm2 = _mm_set1_epi16( 0xffff );
 while (i >= 8) {
  // search for ch
  int mask = 0;
   __m128i xmm1 = _mm_load_si128((__m128i *)(str+i-8));
   xmm1 = _mm_andnot_si128(_mm_cmpeq_epi16(xmm1, xmm0),xmm2);
   if ((mask = _mm_movemask_epi8(_mm_slli_epi16(xmm1, 8))) != 0) {   // some character is not ch
    // got 0 somewhere within 16 bytes in xmm1, or within 16 bits in mask
    // find index of last set bit
#if (MMSC_VER)   // make sure <intrin.h> is included
    unsigned long pos;
    _BitScanBackward(&pos, mask);
    i -= ((size_t)pos-16)>>1;
#elif defined(__clang__) || ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))) // modern GCC has built-in
    i -= (__builtin_clz(mask)-16)>>1;  // mask is 32-bits but only lower 16-bits are significant
#else  // none of choices exist, use local BSR implementation
#error __builtin_clz
#endif
    return i;
  }
  i -= 8;
 }
#endif

 while (i>0){if (ch!=str[i-1]) return i; else --i;}
 return 0;
}

static size_t srchr4(unsigned int* str, unsigned int ch, size_t len){
 size_t i=len;

#if C_AVX2 || EMU_AVX2
 // align to 32 bytes
 while ((i>0) && ((((intptr_t)(str+i)) & 31) != 0)){if (ch!=str[i-1]) return i; else --i;}
/* don't test i>=0 which is always true because size_t is unsigned */
 const __m256i mm0 = _mm256_set1_epi32( ch );
 const __m256i mm2 = _mm256_set1_epi32( 0xffffffff );
 while (i >= 8) {
  // search for ch
  int mask = 0;
   __m256i mm1 = _mm256_load_si256((__m256i *)(str+i-8));
   mm1 = _mm256_andnot_si256(_mm256_cmpeq_epi32(mm1, mm0),mm2);
   // no such thing as _mm256_movemask_epi32
   // Shift each 16-bit element to the right by 8 bits, zero-filling the upper
   // bits.  This will remove the leading high byte from coming up in the mask
   // we generate below, allowing us to use popcount to get the number of slots
   // to compare in the subsequent step.
   if ((mask = _mm256_movemask_epi8(_mm256_srli_epi32(mm1, 24))) != 0) {   // some character is not ch
    // got 0 somewhere within 32 bytes in mm1, or within 32 bits in mask
    // find index of last set bit
#if (MMSC_VER)   // make sure <intrin.h> is included
    unsigned long pos;
    _BitScanBackward(&pos, mask);
    i -= ((size_t)pos)>>2;
#elif defined(__clang__) || ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))) // modern GCC has built-in
    i -= (__builtin_clz(mask))>>2;
#else  // none of choices exist, use local BSR implementation
#error __builtin_clz
#endif
    return i;
  }
  i -= 8;
 }

#elif defined(__SSE2__)
 // align to 16 bytes
 while ((i>0) && ((((intptr_t)(str+i)) & 15) != 0)){if (ch!=str[i-1]) return i; else --i;}
 const __m128i xmm0 = _mm_set1_epi32( ch );
 const __m128i xmm2 = _mm_set1_epi32( 0xffffffff );
 while (i >= 4) {
  // search for ch
  int mask = 0;
   __m128i xmm1 = _mm_load_si128((__m128i *)(str+i-4));
   xmm1 = _mm_andnot_si128(_mm_cmpeq_epi32(xmm1, xmm0),xmm2);
   if ((mask = _mm_movemask_epi8(_mm_slli_epi32(xmm1, 24))) != 0) {   // some character is not ch
    // got 0 somewhere within 16 bytes in xmm1, or within 16 bits in mask
    // find index of last set bit
#if (MMSC_VER)   // make sure <intrin.h> is included
    unsigned long pos;
    _BitScanBackward(&pos, mask);
    i -= ((size_t)pos-16)>>2;
#elif defined(__clang__) || ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))) // modern GCC has built-in
    i -= (__builtin_clz(mask)-16)>>2;  // mask is 32-bits but only lower 16-bits are significant
#else  // none of choices exist, use local BSR implementation
#error __builtin_clz
#endif
    return i;
  }
  i -= 4;
 }
#endif

 while (i>0){if (ch!=str[i-1]) return i; else --i;}
 return 0;
}

// -------------------------------------------------------
// tolower toupper

/* A SIMD function for SSE2 which changes all uppercase ASCII digits to lowercase. */
void StringToLower(char *str,size_t len){
#if defined(__aarch64__)
 // align to 16 bytes
 while ((len>0) && ((((intptr_t)str) & 15) != 0)) {
// tolower toupper unreliable in some compiler
//  *str = tolower(*str);
  char c = *str;
  *str = (c >= 'A' && c <= 'Z') ? (c | 0x20) : c;
  len--;
  ++str;
 }
    size_t i = 0;

    // Pre-load constants for comparison and transformation
    // 'A' - 1 = 64 (0x40)
    // 'Z' = 90 (0x5a)
    uint8x16_t upper_min = vdupq_n_u8('A' - 1);
    uint8x16_t upper_max = vdupq_n_u8('Z');
    uint8x16_t add_mask  = vdupq_n_u8(0x20);

    // Process 16 bytes at a time
    while (len >= 16) { // Check if 16 bytes are available
        // Load 16 bytes from memory
        // Using vld1q_u8 for unaligned access safety
        uint8x16_t data = vld1q_u8((const uint8_t *)(str + i));

        // Create a mask where elements are 1 if char is in [A, Z]
        // mask = (data > 'A'-1) AND (data <= 'Z')
        uint8x16_t gt_min = vcgtq_u8(data, upper_min);
        uint8x16_t le_max = vcleq_u8(data, upper_max);
        uint8x16_t mask   = vandq_u8(gt_min, le_max);

        // Let's use the bitwise selection method:
        // result = mask? (data + 32) : data
        // Since we can't branch, we use the mask to zero out bits in the add_mask 
        // or use bitwise logic.
        
        // Correct Bitwise Method:
        // We only want to add 0x20 if the char is in [A-Z].
        // We create a mask of 0x20 where true, and 0x00 where false.
        uint8x16_t val_to_add = vandq_u8(mask, add_mask);
        uint8x16_t final_data = vaddq_u8(data, val_to_add);

        vst1q_u8((uint8_t *)(str + i), final_data);
        len -= 16;
        i += 16;

        // Safety break if we've reached end of string (if string is not multiple of 16)
    }
    str+=i;
    // remember to handle the remainder (elements that don't fit in a 16-byte chunk)

/*
### How it works:

1.  **Vectorization**: Instead of checking one character at a time, we load 16 characters into a single `uint8x16_t` register.
2.  **Parallel Comparison**: 
    *   `vcgtq_u8`: Performs a \"Greater Than\" comparison across all 16 bytes simultaneously.
    *   `vcleq_u8`: Performs a \"Less Than or Equal\" comparison.
    *   `vandq_u8`: Performs a bitwise `AND`. This creates a bitmask where a byte is `0xFF` (all 1s) if the character is uppercase, and `0x00` if it is not.
3.  **Conditional Addition**: 
    *   We create a vector `val_to_add` which is `0x20` where the character is uppercase and `0x00` where it isn't.
    *   By adding this to the original data, we effectively perform `char + 32` for uppercase letters and `char + 0` for everything else.
4.  **Complexity**: The algorithm is $O(n)$ and significantly faster on large strings because it utilizes the wide execution units of the ARM core, reducing the number of conditional branches (which are expensive due to potential mispredictions).
*/
#elif (C_AVX2 || EMU_AVX2)
 // align to 32 bytes
 while ((len>0) && ((((intptr_t)str) & 31) != 0)) {
//  *str = tolower(*str);
  char c = *str;
  *str = (c >= 'A' && c <= 'Z') ? (c | 0x20) : c;
  len--;
  ++str;
 }
 const __m256i mm1 = _mm256_set1_epi8( 'A' );
 const __m256i mm2 = _mm256_set1_epi8( 'Z' );
 const __m256i mm3 = _mm256_set1_epi8( 0x20 );
 while (len >= 32) {
  __m256i r0 = _mm256_load_si256((__m256i*)str);
  // maskaz contains 0x00 where character between 'A' and 'Z', 0xff otherwise.
  __m256i maskaz = _mm256_or_si256(_mm256_cmpgt_epi8(mm1,r0), _mm256_cmpgt_epi8(r0, mm2));
  // flip the 6th bit to 0 only for uppercase characters.
  _mm256_store_si256((__m256i*)str, _mm256_xor_si256(r0, _mm256_andnot_si256(maskaz, mm3)));
  len -= 32;
  str += 32;
 }

#elif defined(__SSE2__)
 // align to 16 bytes
 while ((len>0) && ((((intptr_t)str) & 15) != 0)) {
//  *str = tolower(*str);
  char c = *str;
  *str = (c >= 'A' && c <= 'Z') ? (c | 0x20) : c;
  len--;
  ++str;
 }
 const __m128i xmm1 = _mm_set1_epi8( 'A' );
 const __m128i xmm2 = _mm_set1_epi8( 'Z' );
 const __m128i xmm3 = _mm_set1_epi8( 0x20 );
 while (len >= 16) {
  __m128i r0 = _mm_load_si128((__m128i*)str);
  // maskaz contains 0x00 where character between 'A' and 'Z', 0xff otherwise.
  __m128i maskaz = _mm_or_si128(_mm_cmplt_epi8(r0, xmm1), _mm_cmpgt_epi8(r0, xmm2));
  // flip the 6th bit to 0 only for uppercase characters.
  _mm_store_si128((__m128i*)str, _mm_xor_si128(r0, _mm_andnot_si128(maskaz, xmm3)));
  len -= 16;
  str += 16;
 }
#endif

 while (len-- > 0) {
//  *str = tolower(*str);
  char c = *str;
  *str = (c >= 'A' && c <= 'Z') ? (c | 0x20) : c;
  ++str;
 }
}

/* Same, but to uppercase. */
void StringToUpper(char *str,size_t len){
#if defined(__aarch64__)
 // align to 16 bytes
 while ((len>0) && ((((intptr_t)str) & 15) != 0)) {
//  *str = toupper(*str);
  char c = *str;
  *str = (c >= 'a' && c <= 'z') ? (c & ~0x20) : c;
  len--;
  ++str;
 }
    size_t i = 0;

    // Pre-load constants for comparison and transformation
    // 'a' - 1 = 64 (0x60)
    // 'z' = 90 (0x7a)
    uint8x16_t upper_min = vdupq_n_u8('a' - 1);
    uint8x16_t upper_max = vdupq_n_u8('z');
    uint8x16_t add_mask  = vdupq_n_u8(0x20);

    // Process 16 bytes at a time
    while (len >= 16) { // Check if 16 bytes are available
        // Load 16 bytes from memory
        // Using vld1q_u8 for unaligned access safety
        uint8x16_t data = vld1q_u8((const uint8_t *)(str + i));

        // Create a mask where elements are 1 if char is in [a, z]
        // mask = (data > 'a'-1) AND (data <= 'z')
        uint8x16_t gt_min = vcgtq_u8(data, upper_min);
        uint8x16_t le_max = vcleq_u8(data, upper_max);
        uint8x16_t mask   = vandq_u8(gt_min, le_max);

        // Let's use the bitwise selection method:
        // result = mask? (data - 32) : data
        // Since we can't branch, we use the mask to zero out bits in the add_mask 
        // or use bitwise logic.
        
        // Correct Bitwise Method:
        // We only want to subtract 0x20 if the char is in [a-z].
        // We create a mask of 0x20 where true, and 0x00 where false.
        uint8x16_t val_to_add = vandq_u8(mask, add_mask);
        uint8x16_t final_data = vsubq_u8(data, val_to_add);

        vst1q_u8((uint8_t *)(str + i), final_data);
        len -= 16;
        i += 16;

        // Safety break if we've reached end of string (if string is not multiple of 16)
    }
    str+=i;
    // remember to handle the remainder (elements that don't fit in a 16-byte chunk)

#elif (C_AVX2 || EMU_AVX2)
 // align to 32 bytes
 while ((len>0) && ((((intptr_t)str) & 31) != 0)) {
//  *str = toupper(*str);
  char c = *str;
  *str = (c >= 'a' && c <= 'z') ? (c & ~0x20) : c;
  len--;
  ++str;
 }
 const __m256i mm1 = _mm256_set1_epi8( 'a' );
 const __m256i mm2 = _mm256_set1_epi8( 'z' );
 const __m256i mm3 = _mm256_set1_epi8( 0x20 );
 while (len >= 32) {
  __m256i r0 = _mm256_load_si256((__m256i*)str);
  // maskaz contains 0x00 where character between 'a' and 'z', 0xff otherwise.
  __m256i maskaz = _mm256_or_si256(_mm256_cmpgt_epi8(mm1,r0), _mm256_cmpgt_epi8(r0, mm2));
  // flip the 6th bit to 0 only for lowercase characters.
  _mm256_store_si256((__m256i*)str, _mm256_xor_si256(r0, _mm256_andnot_si256(maskaz, mm3)));
  len -= 32;
  str += 32;
 }

#elif defined(__SSE2__)
 // align to 16 bytes
 while ((len>0) && ((((intptr_t)str) & 15) != 0)) {
//  *str = toupper(*str);
  char c = *str;
  *str = (c >= 'a' && c <= 'z') ? (c & ~0x20) : c;
  len--;
  ++str;
 }
 const __m128i xmm1 = _mm_set1_epi8( 'a' );
 const __m128i xmm2 = _mm_set1_epi8( 'z' );
 const __m128i xmm3 = _mm_set1_epi8( 0x20 );
 while (len >= 16) {
  __m128i r0 = _mm_load_si128((__m128i*)str);
  // maskaz contains 0x00 where character between 'a' and 'z', 0xff otherwise.
  __m128i maskaz = _mm_or_si128(_mm_cmplt_epi8(r0, xmm1), _mm_cmpgt_epi8(r0, xmm2));
  // flip the 6th bit to 0 only for lowercase characters.
  _mm_store_si128((__m128i*)str, _mm_xor_si128(r0, _mm_andnot_si128(maskaz, xmm3)));
  len -= 16;
  str += 16;
 }
#endif

 while (len-- > 0) {
//  *str = toupper(*str);
  char c = *str;
  *str = (c >= 'a' && c <= 'z') ? (c & ~0x20) : c;
  ++str;
 }
}

/* A SIMD function for SSE2 which changes all uppercase ASCII digits to lowercase. */
void StringToLowerUCS2(unsigned short *str,size_t len){
 const char OFFSET = 'a' - 'A';
#if defined(__aarch64__)
 // align to 16 bytes
 while ((len>0) && ((((intptr_t)str) & 15) != 0)) {
  *str = (*str>= 'A' && *str<= 'Z') ? *str += OFFSET : *str;
  len--;
  ++str;
 }
    // Constants for ASCII 'A' (0x0041), 'Z' (0x005A), and the case flip bit (0x0020)
    const uint16x8_t v_a = vdupq_n_u16(0x0041);
    const uint16x8_t v_z = vdupq_n_u16(0x005A);
    const uint16x8_t v_case_bit = vdupq_n_u16(0x0020);

    size_t i = 0;
    
    // --- Main Loop: Process 8 characters (16 bytes) per iteration ---
    for (; i + 7 < len; i += 8) {
        // 1. Load 8 x uint16_t (128 bits / 16 bytes). 
        // vld1q_u16 handles unaligned access safely on ARMv8-A.
        uint16x8_t chunk = vld1q_u16(str + i);

        // 2. Create mask for range [A, Z]
        // vcgeq_u16: Sets lane to 0xFFFF if chunk >= 'A', else 0x0000
        // vcleq_u16: Sets lane to 0xFFFF if chunk <= 'Z', else 0x0000
        // Note: vcleq = !(chunk > 'Z'). NEON has vcgeq/vcleq but not vcltq directly for unsigned 
        // in some intrinsic versions, but vcleq_u16 is standard in arm_neon.h.
        uint16x8_t mask_ge_a = vcgeq_u16(chunk, v_a);
        uint16x8_t mask_le_z = vcleq_u16(chunk, v_z);

        // 3. Combine masks: is_upper = (c >= 'A') & (c <= 'Z')
        // Result lanes are 0xFFFF (true) or 0x0000 (false)
        uint16x8_t mask_upper = vandq_u16(mask_ge_a, mask_le_z);

        // 4. Calculate addition: 0x20 if upper, 0x00 otherwise
        uint16x8_t add_val = vandq_u16(mask_upper, v_case_bit);

        // 5. Add to original chunk. 
        // Saturated add (vqaddq) not needed here as 0x5A + 0x20 = 0x7A < 0xFFFF.
        uint16x8_t result = vaddq_u16(chunk, add_val);

        // 6. Store back
        vst1q_u16(str + i, result);
    }
    str += i;
    len -= i;
    // --- Tail Handling: Process remaining 1-7 characters scalar ---

#elif (C_AVX2 || EMU_AVX2)
 // align to 32 bytes
 while ((len>0) && ((((intptr_t)str) & 31) != 0)) {
  *str = (*str>= 'A' && *str<= 'Z') ? *str += OFFSET : *str;
  len--;
  ++str;
 }
 const __m256i mm1 = _mm256_set1_epi16( 'A' );
 const __m256i mm2 = _mm256_set1_epi16( 'Z' );
 const __m256i mm3 = _mm256_set1_epi16( 0x20 );
 while (len >= 16) {
  __m256i r0 = _mm256_load_si256((__m256i*)str);
  // maskaz contains 0x00 where character between 'A' and 'Z', 0xff otherwise.
  __m256i maskaz = _mm256_or_si256(_mm256_cmpgt_epi16(mm1,r0), _mm256_cmpgt_epi16(r0, mm2));
  // flip the 6th bit to 0 only for uppercase characters.
  _mm256_store_si256((__m256i*)str, _mm256_xor_si256(r0, _mm256_andnot_si256(maskaz, mm3)));
  len -= 16;
  str += 16;
 }

#elif defined(__SSE2__)
 // align to 16 bytes
 while ((len>0) && ((((intptr_t)str) & 15) != 0)) {
  *str = (*str>= 'A' && *str<= 'Z') ? *str += OFFSET : *str;
  len--;
  ++str;
 }
 const __m128i xmm1 = _mm_set1_epi16( 'A' );
 const __m128i xmm2 = _mm_set1_epi16( 'Z' );
 const __m128i xmm3 = _mm_set1_epi16( 0x20 );
 while (len >= 8) {
  __m128i r0 = _mm_load_si128((__m128i*)str);
  // maskaz contains 0x00 where character between 'A' and 'Z', 0xff otherwise.
  __m128i maskaz = _mm_or_si128(_mm_cmplt_epi16(r0, xmm1), _mm_cmpgt_epi16(r0, xmm2));
  // flip the 6th bit to 0 only for uppercase characters.
  _mm_store_si128((__m128i*)str, _mm_xor_si128(r0, _mm_andnot_si128(maskaz, xmm3)));
  len -= 8;
  str += 8;
 }
#endif

 while (len-- > 0) {
  *str = (*str>= 'A' && *str<= 'Z') ? *str += OFFSET : *str;
  ++str;
 }
}

/* Same, but to uppercase. */
void StringToUpperUCS2(unsigned short *str,size_t len){
 const char OFFSET = 'a' - 'A';
#if defined(__aarch64__)
 // align to 16 bytes
 while ((len>0) && ((((intptr_t)str) & 15) != 0)) {
  *str = (*str>= 'a' && *str<= 'z') ? *str -= OFFSET : *str;
  len--;
  ++str;
 }
    // Constants for ASCII 'a' (0x0061), 'z' (0x007A), and the case flip bit (0x0020)
    const uint16x8_t v_a = vdupq_n_u16(0x0061);
    const uint16x8_t v_z = vdupq_n_u16(0x007A);
    const uint16x8_t v_case_bit = vdupq_n_u16(0x0020);

    size_t i = 0;
    
    // --- Main Loop: Process 8 characters (16 bytes) per iteration ---
    for (; i + 7 < len; i += 8) {
        // 1. Load 8 x uint16_t (128 bits / 16 bytes). 
        // vld1q_u16 handles unaligned access safely on ARMv8-A.
        uint16x8_t chunk = vld1q_u16(str + i);

        // 2. Create mask for range [a, z]
        // vcgeq_u16: Sets lane to 0xFFFF if chunk >= 'a', else 0x0000
        // vcleq_u16: Sets lane to 0xFFFF if chunk <= 'z', else 0x0000
        // Note: vcleq = !(chunk > 'Z'). NEON has vcgeq/vcleq but not vcltq directly for unsigned 
        // in some intrinsic versions, but vcleq_u16 is standard in arm_neon.h.
        uint16x8_t mask_ge_a = vcgeq_u16(chunk, v_a);
        uint16x8_t mask_le_z = vcleq_u16(chunk, v_z);

        // 3. Combine masks: is_upper = (c >= 'a') & (c <= 'z')
        // Result lanes are 0xFFFF (true) or 0x0000 (false)
        uint16x8_t mask_upper = vandq_u16(mask_ge_a, mask_le_z);

        // 4. Calculate addition: 0x20 if lower, 0x00 otherwise
        uint16x8_t sub_val = vandq_u16(mask_upper, v_case_bit);

        // 5. Add to original chunk. 
        // Saturated add (vqsubq) not needed here as 0x7A - 0x20 = 0x5A < 0xFFFF.
        uint16x8_t result = vsubq_u16(chunk, sub_val);

        // 6. Store back
        vst1q_u16(str + i, result);
    }
    str += i;
    len -= i;
    // --- Tail Handling: Process remaining 1-7 characters scalar ---

#elif (C_AVX2 || EMU_AVX2)
 // align to 32 bytes
 while ((len>0) && ((((intptr_t)str) & 31) != 0)) {
  *str = (*str>= 'a' && *str<= 'z') ? *str -= OFFSET : *str;
  len--;
  ++str;
 }
 const __m256i mm1 = _mm256_set1_epi16( 'a' );
 const __m256i mm2 = _mm256_set1_epi16( 'z' );
 const __m256i mm3 = _mm256_set1_epi16( 0x20 );
 while (len >= 16) {
  __m256i r0 = _mm256_load_si256((__m256i*)str);
  // maskaz contains 0x00 where character between 'a' and 'z', 0xff otherwise.
  __m256i maskaz = _mm256_or_si256(_mm256_cmpgt_epi16(mm1,r0), _mm256_cmpgt_epi16(r0, mm2));
  // flip the 6th bit to 0 only for lowercase characters.
  _mm256_store_si256((__m256i*)str, _mm256_xor_si256(r0, _mm256_andnot_si256(maskaz, mm3)));
  len -= 16;
  str += 16;
 }

#elif defined(__SSE2__)
 // align to 16 bytes
 while ((len>0) && ((((intptr_t)str) & 15) != 0)) {
  *str = (*str>= 'a' && *str<= 'z') ? *str -= OFFSET : *str;
  len--;
  ++str;
 }
 const __m128i xmm1 = _mm_set1_epi16( 'a' );
 const __m128i xmm2 = _mm_set1_epi16( 'z' );
 const __m128i xmm3 = _mm_set1_epi16( 0x20 );
 while (len >= 8) {
  __m128i r0 = _mm_load_si128((__m128i*)str);
  // maskaz contains 0x00 where character between 'a' and 'z', 0xff otherwise.
  __m128i maskaz = _mm_or_si128(_mm_cmplt_epi16(r0, xmm1), _mm_cmpgt_epi16(r0, xmm2));
  // flip the 6th bit to 0 only for lowercase characters.
  _mm_store_si128((__m128i*)str, _mm_xor_si128(r0, _mm_andnot_si128(maskaz, xmm3)));
  len -= 8;
  str += 8;
 }
#endif

 while (len-- > 0) {
  *str = (*str>= 'a' && *str<= 'z') ? *str -= OFFSET : *str;
  ++str;
 }
}

/* A SIMD function for SSE2 which changes all uppercase ASCII digits to lowercase. */
void StringToLowerUCS4(unsigned int *str,size_t len){
 const char OFFSET = 'a' - 'A';
#if defined(__aarch64__)
 // align to 16 bytes
 while ((len>0) && ((((intptr_t)str) & 15) != 0)) {
  *str = (*str>= 'A' && *str<= 'Z') ? *str += OFFSET : *str;
  len--;
  ++str;
 }
    // Constants for ASCII 'A' (0x0041), 'Z' (0x005A), and the case flip bit (0x0020)
    const uint32x4_t v_a = vdupq_n_u32(0x0041);
    const uint32x4_t v_z = vdupq_n_u32(0x005A);
    const uint32x4_t v_case_bit = vdupq_n_u32(0x0020);

    size_t i = 0;
    
    // --- Main Loop: Process 4 characters (16 bytes) per iteration ---
    for (; i + 3 < len; i += 4) {
        // 1. Load 4 x uint32_t (128 bits / 16 bytes). 
        // vld1q_u32 handles unaligned access safely on ARMv8-A.
        uint32x4_t chunk = vld1q_u32(str + i);

        // 2. Create mask for range [A, Z]
        // vcgeq_u32: Sets lane to 0xFFFF if chunk >= 'A', else 0x0000
        // vcleq_u32: Sets lane to 0xFFFF if chunk <= 'Z', else 0x0000
        // Note: vcleq = !(chunk > 'Z'). NEON has vcgeq/vcleq but not vcltq directly for unsigned 
        // in some intrinsic versions, but vcleq_u32 is standard in arm_neon.h.
        uint32x4_t mask_ge_a = vcgeq_u32(chunk, v_a);
        uint32x4_t mask_le_z = vcleq_u32(chunk, v_z);

        // 3. Combine masks: is_upper = (c >= 'A') & (c <= 'Z')
        // Result lanes are 0xFFFF (true) or 0x0000 (false)
        uint32x4_t mask_upper = vandq_u32(mask_ge_a, mask_le_z);

        // 4. Calculate addition: 0x20 if upper, 0x00 otherwise
        uint32x4_t add_val = vandq_u32(mask_upper, v_case_bit);

        // 5. Add to original chunk. 
        // Saturated add (vqaddq) not needed here as 0x5A + 0x20 = 0x7A < 0xFFFF.
        uint32x4_t result = vaddq_u32(chunk, add_val);

        // 6. Store back
        vst1q_u32(str + i, result);
    }
    str += i;
    len -= i;
    // --- Tail Handling: Process remaining 1-3 characters scalar ---

#elif (C_AVX2 || EMU_AVX2)
 // align to 32 bytes
 while ((len>0) && ((((intptr_t)str) & 31) != 0)) {
  *str = (*str>= 'A' && *str<= 'Z') ? *str += OFFSET : *str;
  len--;
  ++str;
 }
 const __m256i mm1 = _mm256_set1_epi32( 'A' );
 const __m256i mm2 = _mm256_set1_epi32( 'Z' );
 const __m256i mm3 = _mm256_set1_epi32( 0x20 );
 while (len >= 8) {
  __m256i r0 = _mm256_load_si256((__m256i*)str);
  // maskaz contains 0x00 where character between 'A' and 'Z', 0xff otherwise.
  __m256i maskaz = _mm256_or_si256(_mm256_cmpgt_epi32(mm1,r0), _mm256_cmpgt_epi32(r0, mm2));
  // flip the 6th bit to 0 only for uppercase characters.
  _mm256_store_si256((__m256i*)str, _mm256_xor_si256(r0, _mm256_andnot_si256(maskaz, mm3)));
  len -= 8;
  str += 8;
 }

#elif defined(__SSE2__)
 // align to 16 bytes
 while ((len>0) && ((((intptr_t)str) & 15) != 0)) {
  *str = (*str>= 'A' && *str<= 'Z') ? *str += OFFSET : *str;
  len--;
  ++str;
 }
 const __m128i xmm1 = _mm_set1_epi32( 'A' );
 const __m128i xmm2 = _mm_set1_epi32( 'Z' );
 const __m128i xmm3 = _mm_set1_epi32( 0x20 );
 while (len >= 4) {
  __m128i r0 = _mm_load_si128((__m128i*)str);
  // maskaz contains 0x00 where character between 'A' and 'Z', 0xff otherwise.
  __m128i maskaz = _mm_or_si128(_mm_cmplt_epi32(r0, xmm1), _mm_cmpgt_epi32(r0, xmm2));
  // flip the 6th bit to 0 only for uppercase characters.
  _mm_store_si128((__m128i*)str, _mm_xor_si128(r0, _mm_andnot_si128(maskaz, xmm3)));
  len -= 4;
  str += 4;
 }
#endif

 while (len-- > 0) {
  *str = (*str>= 'A' && *str<= 'Z') ? *str += OFFSET : *str;
  ++str;
 }
}

/* Same, but to uppercase. */
void StringToUpperUCS4(unsigned int *str,size_t len){
 const char OFFSET = 'a' - 'A';
#if defined(__aarch64__)
 // align to 16 bytes
 while ((len>0) && ((((intptr_t)str) & 15) != 0)) {
  *str = (*str>= 'a' && *str<= 'z') ? *str -= OFFSET : *str;
  len--;
  ++str;
 }
    // Constants for ASCII 'a' (0x0061), 'z' (0x007A), and the case flip bit (0x0020)
    const uint32x4_t v_a = vdupq_n_u32(0x0061);
    const uint32x4_t v_z = vdupq_n_u32(0x007A);
    const uint32x4_t v_case_bit = vdupq_n_u32(0x0020);

    size_t i = 0;
    
    // --- Main Loop: Process 4 characters (16 bytes) per iteration ---
    for (; i + 3 < len; i += 4) {
        // 1. Load 4 x uint32_t (128 bits / 16 bytes). 
        // vld1q_u32 handles unaligned access safely on ARMv8-A.
        uint32x4_t chunk = vld1q_u32(str + i);

        // 2. Create mask for range [a, z]
        // vcgeq_u32: Sets lane to 0xFFFF if chunk >= 'a', else 0x0000
        // vcleq_u32: Sets lane to 0xFFFF if chunk <= 'z', else 0x0000
        // Note: vcleq = !(chunk > 'Z'). NEON has vcgeq/vcleq but not vcltq directly for unsigned 
        // in some intrinsic versions, but vcleq_u32 is standard in arm_neon.h.
        uint32x4_t mask_ge_a = vcgeq_u32(chunk, v_a);
        uint32x4_t mask_le_z = vcleq_u32(chunk, v_z);

        // 3. Combine masks: is_upper = (c >= 'a') & (c <= 'z')
        // Result lanes are 0xFFFF (true) or 0x0000 (false)
        uint32x4_t mask_upper = vandq_u32(mask_ge_a, mask_le_z);

        // 4. Calculate addition: 0x20 if upper, 0x00 otherwise
        uint32x4_t sub_val = vandq_u32(mask_upper, v_case_bit);

        // 5. Add to original chunk. 
        // Saturated add (vqaddq) not needed here as 0x7A - 0x20 = 0x5A < 0xFFFF.
        uint32x4_t result = vsubq_u32(chunk, sub_val);

        // 6. Store back
        vst1q_u32(str + i, result);
    }
    str += i;
    len -= i;
    // --- Tail Handling: Process remaining 1-3 characters scalar ---

#elif (C_AVX2 || EMU_AVX2)
 // align to 32 bytes
 while ((len>0) && ((((intptr_t)str) & 31) != 0)) {
  *str = (*str>= 'a' && *str<= 'z') ? *str -= OFFSET : *str;
  len--;
  ++str;
 }
 const __m256i mm1 = _mm256_set1_epi32( 'a' );
 const __m256i mm2 = _mm256_set1_epi32( 'z' );
 const __m256i mm3 = _mm256_set1_epi32( 0x20 );
 while (len >= 8) {
  __m256i r0 = _mm256_load_si256((__m256i*)str);
  // maskaz contains 0x00 where character between 'a' and 'z', 0xff otherwise.
  __m256i maskaz = _mm256_or_si256(_mm256_cmpgt_epi32(mm1,r0), _mm256_cmpgt_epi32(r0, mm2));
  // flip the 6th bit to 0 only for lowercase characters.
  _mm256_store_si256((__m256i*)str, _mm256_xor_si256(r0, _mm256_andnot_si256(maskaz, mm3)));
  len -= 8;
  str += 8;
 }

#elif defined(__SSE2__)
 // align to 16 bytes
 while ((len>0) && ((((intptr_t)str) & 15) != 0)) {
  *str = (*str>= 'a' && *str<= 'z') ? *str -= OFFSET : *str;
  len--;
  ++str;
 }
 const __m128i xmm1 = _mm_set1_epi32( 'a' );
 const __m128i xmm2 = _mm_set1_epi32( 'z' );
 const __m128i xmm3 = _mm_set1_epi32( 0x20 );
 while (len >= 4) {
  __m128i r0 = _mm_load_si128((__m128i*)str);
  // maskaz contains 0x00 where character between 'a' and 'z', 0xff otherwise.
  __m128i maskaz = _mm_or_si128(_mm_cmplt_epi32(r0, xmm1), _mm_cmpgt_epi32(r0, xmm2));
  // flip the 6th bit to 0 only for lowercase characters.
  _mm_store_si128((__m128i*)str, _mm_xor_si128(r0, _mm_andnot_si128(maskaz, xmm3)));
  len -= 4;
  str += 4;
 }
#endif

 while (len-- > 0) {
  *str = (*str>= 'a' && *str<= 'z') ? *str -= OFFSET : *str;
  ++str;
 }
}

// -------------------------------------------------------
// Stringlchr_omp

static size_t Stringlchr_omp(char *str, char ch, size_t stride, size_t len, size_t klen, size_t *pi){
 size_t i,ln=stride,rlen,num_threads;
#pragma omp parallel
 {
#pragma omp single
 {
  num_threads = omp_get_num_threads();
  num_threads = (num_threads>OMP_THREADS) ? OMP_THREADS : num_threads;
  rlen = len / num_threads;
 }
#pragma omp parallel for default(none),firstprivate(str,stride,klen,ch,num_threads,rlen),private(i),shared(ln,pi)
 for(i=0; i<num_threads; i++) {
  size_t j,lm=stride;
  for(j=0; j<rlen; j++) {
   size_t l=slchr(str+i*rlen*stride+j*stride,ch,klen);
   pi[i*rlen+j]=l;
   lm=(lm>l)?l:lm;
  }
  #pragma omp critical
  {
   ln=(ln>lm)?lm:ln;
  }
 }
 }
 for(i=num_threads * rlen; i<len; i++) {
  size_t l=slchr(str+i*stride,ch,klen);
  pi[i]=l;
  ln=(ln>l)?l:ln;
 }
 return ln;
}

static size_t Stringlchr2_omp(unsigned short *str, unsigned short ch, size_t stride, size_t len, size_t klen, size_t *pi){
 size_t i,ln=stride,rlen,num_threads;
#pragma omp parallel
 {
#pragma omp single
 {
  num_threads = omp_get_num_threads();
  num_threads = (num_threads>OMP_THREADS) ? OMP_THREADS : num_threads;
  rlen = len / num_threads;
 }
#pragma omp parallel for default(none),firstprivate(str,stride,klen,ch,num_threads,rlen),private(i),shared(ln,pi)
 for(i=0; i<num_threads; i++) {
  size_t j,lm=stride;
  for(j=0; j<rlen; j++) {
   size_t l=slchr2(str+i*rlen*stride+j*stride,ch,klen);
   pi[i*rlen+j]=l;
   lm=(lm>l)?l:lm;
  }
  #pragma omp critical
  {
   ln=(ln>lm)?lm:ln;
  }
 }
 }
 for(i=num_threads * rlen; i<len; i++) {
  size_t l=slchr2(str+i*stride,ch,klen);
  pi[i]=l;
  ln=(ln>l)?l:ln;
 }
 return ln;
}

static size_t Stringlchr4_omp(unsigned int *str, unsigned int ch, size_t stride, size_t len, size_t klen, size_t *pi){
 size_t i,ln=stride,rlen,num_threads;
#pragma omp parallel
 {
#pragma omp single
 {
  num_threads = omp_get_num_threads();
  num_threads = (num_threads>OMP_THREADS) ? OMP_THREADS : num_threads;
  rlen = len / num_threads;
 }
#pragma omp parallel for default(none),firstprivate(str,stride,klen,ch,num_threads,rlen),private(i),shared(ln,pi)
 for(i=0; i<num_threads; i++) {
  size_t j,lm=stride;
  for(j=0; j<rlen; j++) {
   size_t l=slchr4(str+i*rlen*stride+j*stride,ch,klen);
   pi[i*rlen+j]=l;
   lm=(lm>l)?l:lm;
  }
  #pragma omp critical
  {
   ln=(ln>lm)?lm:ln;
  }
 }
 }
 for(i=num_threads * rlen; i<len; i++) {
  size_t l=slchr4(str+i*stride,ch,klen);
  pi[i]=l;
  ln=(ln>l)?l:ln;
 }
 return ln;
}

size_t Stringlchr(char *str, char ch, size_t stride, size_t len, size_t klen, size_t *pi){
#ifdef _OPENMP
 if (len >= OMP_THRESHOLD) {
  return Stringlchr_omp(str, ch, stride, len, klen, pi);
 }
#endif
 size_t i=len,ln=stride;
 while (i-- > 0) {
  size_t l=slchr(str,ch,klen);
  *pi++=l;
  ln=(ln>l)?l:ln;
  str+=stride;
 }
 return ln;
}

size_t Stringlchr2(unsigned short *str, unsigned short ch, size_t stride, size_t len, size_t klen, size_t *pi){
#ifdef _OPENMP
 if (len >= OMP_THRESHOLD) {
  return Stringlchr2_omp(str, ch, stride, len, klen, pi);
 }
#endif
 size_t i=len,ln=stride;
 while (i-- > 0) {
  size_t l=slchr2(str,ch,klen);
  *pi++=l;
  ln=(ln>l)?l:ln;
  str+=stride;
 }
 return ln;
}

size_t Stringlchr4(unsigned int *str, unsigned int ch, size_t stride, size_t len, size_t klen, size_t *pi){
#ifdef _OPENMP
 if (len >= OMP_THRESHOLD) {
  return Stringlchr4_omp(str, ch, stride, len, klen, pi);
 }
#endif
 size_t i=len,ln=stride;
 while (i-- > 0) {
  size_t l=slchr4(str,ch,klen);
  *pi++=l;
  ln=(ln>l)?l:ln;
  str+=stride;
 }
 return ln;
}

// -------------------------------------------------------
// Stringrchr_omp

static size_t Stringrchr_omp(char *str, char ch, size_t stride, size_t len){
 size_t i,ln=0,rlen,num_threads;
 volatile short flag=0;
#pragma omp parallel
 {
#pragma omp single
 {
  num_threads = omp_get_num_threads();
  num_threads = (num_threads>OMP_THREADS) ? OMP_THREADS : num_threads;
  rlen = len / num_threads;
 }
#pragma omp parallel for default(none),firstprivate(str,stride,ch,num_threads,rlen),private(i),shared(ln,flag)
 for(i=0; i<num_threads; i++) {
  size_t j,lm=0;
  for(j=0; j<rlen; j++) {
   if(flag) continue;
   size_t l=srchr(str+i*rlen*stride+j*stride,ch,stride);
   lm=(lm<l)?l:lm;
   if(l==stride) flag=1;
  }
  #pragma omp critical
  {
   ln=(ln<lm)?lm:ln;
  }
 }
 }
 if(flag||ln==stride) return stride;
 for(i=num_threads * rlen; i<len; i++) {
  size_t l=srchr(str+i*stride,ch,stride);
  ln=(ln<l)?l:ln;
  if(ln==stride) return ln;
 }
 return ln;
}

static size_t Stringrchr2_omp(unsigned short *str, unsigned short ch, size_t stride, size_t len){
 size_t i,ln=0,rlen,num_threads;
 volatile short flag=0;
#pragma omp parallel
 {
#pragma omp single
 {
  num_threads = omp_get_num_threads();
  num_threads = (num_threads>OMP_THREADS) ? OMP_THREADS : num_threads;
  rlen = len / num_threads;
 }
#pragma omp parallel for default(none),firstprivate(str,stride,ch,num_threads,rlen),private(i),shared(ln,flag)
 for(i=0; i<num_threads; i++) {
  size_t j,lm=0;
  for(j=0; j<rlen; j++) {
   if(flag) continue;
   size_t l=srchr2(str+i*rlen*stride+j*stride,ch,stride);
   lm=(lm<l)?l:lm;
   if(l==stride) flag=1;
  }
  #pragma omp critical
  {
   ln=(ln<lm)?lm:ln;
  }
 }
 }
 if(flag||ln==stride) return stride;
 for(i=num_threads * rlen; i<len; i++) {
  size_t l=srchr2(str+i*stride,ch,stride);
  ln=(ln<l)?l:ln;
  if(ln==stride) return ln;
 }
 return ln;
}

static size_t Stringrchr4_omp(unsigned int *str, unsigned int ch, size_t stride, size_t len){
 size_t i,ln=0,rlen,num_threads;
 volatile short flag=0;
#pragma omp parallel
 {
#pragma omp single
 {
  num_threads = omp_get_num_threads();
  num_threads = (num_threads>OMP_THREADS) ? OMP_THREADS : num_threads;
  rlen = len / num_threads;
 }
#pragma omp parallel for default(none),firstprivate(str,stride,ch,num_threads,rlen),private(i),shared(ln,flag)
 for(i=0; i<num_threads; i++) {
  size_t j,lm=0;
  for(j=0; j<rlen; j++) {
   if(flag) continue;
   size_t l=srchr4(str+i*rlen*stride+j*stride,ch,stride);
   lm=(lm<l)?l:lm;
   if(l==stride) flag=1;
  }
  #pragma omp critical
  {
   ln=(ln<lm)?lm:ln;
  }
 }
 }
 if(flag||ln==stride) return stride;
 for(i=num_threads * rlen; i<len; i++) {
  size_t l=srchr4(str+i*stride,ch,stride);
  ln=(ln<l)?l:ln;
  if(ln==stride) return ln;
 }
 return ln;
}

size_t Stringrchr(char *str, char ch, size_t stride, size_t len){
#ifdef _OPENMP
 if (len >= OMP_THRESHOLD) {
  return Stringrchr_omp(str, ch, stride, len);
 }
#endif
 size_t i=len,ln=0;
 while (i-- > 0) {
  size_t l=srchr(str,ch,stride);
  ln=(ln<l)?l:ln;
  if(ln==stride) return ln;
  str+=stride;
 }
 return ln;
}

size_t Stringrchr2(unsigned short *str, unsigned short ch, size_t stride, size_t len){
#ifdef _OPENMP
 if (len >= OMP_THRESHOLD) {
  return Stringrchr2_omp(str, ch, stride, len);
 }
#endif
 size_t i=len,ln=0;
 while (i-- > 0) {
  size_t l=srchr2(str,ch,stride);
  ln=(ln<l)?l:ln;
  if(ln==stride) return ln;
  str+=stride;
 }
 return ln;
}

size_t Stringrchr4(unsigned int *str, unsigned int ch, size_t stride, size_t len){
#ifdef _OPENMP
 if (len >= OMP_THRESHOLD) {
  return Stringrchr4_omp(str, ch, stride, len);
 }
#endif
 size_t i=len,ln=0;
 while (i-- > 0) {
  size_t l=srchr4(str,ch,stride);
  ln=(ln<l)?l:ln;
  if(ln==stride) return ln;
  str+=stride;
 }
 return ln;
}
