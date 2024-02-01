/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
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
#endif

#if defined(__SSE2__) || EMU_AVX2
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
#endif

#if defined(__SSE2__) || EMU_AVX2
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
#endif

#if defined(__SSE2__) || EMU_AVX2
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
#endif

#if defined(__SSE2__) || EMU_AVX2
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
#endif

#if defined(__SSE2__) || EMU_AVX2
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
#endif

#if defined(__SSE2__) || EMU_AVX2
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
#if C_AVX2 || EMU_AVX2

 // align to 32 bytes
 while ((len>0) && ((((intptr_t)str) & 31) != 0)) {
  *str = tolower(*str);
  len--;
  ++str;
 }
 const __m256i mm1 = _mm256_set1_epi8( 'A'-1 );
 const __m256i mm2 = _mm256_set1_epi8( 'Z' );
 const __m256i mm3 = _mm256_set1_epi8( 0x20 );
 const __m256i mm4 = _mm256_cmpeq_epi8( mm3, mm3 );
 while (len >= 32) {
  __m256i r0 = _mm256_load_si256((__m256i*)str);
  // maskaz contains 0x00 where character between 'A' and 'Z', 0xff otherwise.
  __m256i maskaz = _mm256_or_si256(_mm256_andnot_si256(_mm256_cmpgt_epi8(r0, mm1),mm4), _mm256_cmpgt_epi8(r0, mm2));
  // flip the 6th bit to 0 only for uppercase characters.
  _mm256_store_si256((__m256i*)str, _mm256_xor_si256(r0, _mm256_andnot_si256(maskaz, mm3)));
  len -= 32;
  str += 32;
 }
#endif

#if defined(__SSE2__) || EMU_AVX2

 // align to 16 bytes
 while ((len>0) && ((((intptr_t)str) & 15) != 0)) {
  *str = tolower(*str);
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
  *str = tolower(*str);
  ++str;
 }
}

/* Same, but to uppercase. */
void StringToUpper(char *str,size_t len){
#if C_AVX2 || EMU_AVX2

 // align to 32 bytes
 while ((len>0) && ((((intptr_t)str) & 31) != 0)) {
  *str = toupper(*str);
  len--;
  ++str;
 }
 const __m256i mm1 = _mm256_set1_epi8( 'a'-1 );
 const __m256i mm2 = _mm256_set1_epi8( 'z' );
 const __m256i mm3 = _mm256_set1_epi8( 0x20 );
 const __m256i mm4 = _mm256_cmpeq_epi8( mm3, mm3 );
 while (len >= 32) {
  __m256i r0 = _mm256_load_si256((__m256i*)str);
  // maskaz contains 0x00 where character between 'a' and 'z', 0xff otherwise.
  __m256i maskaz = _mm256_or_si256(_mm256_andnot_si256(_mm256_cmpgt_epi8(r0, mm1),mm4), _mm256_cmpgt_epi8(r0, mm2));
  // flip the 6th bit to 0 only for lowercase characters.
  _mm256_store_si256((__m256i*)str, _mm256_xor_si256(r0, _mm256_andnot_si256(maskaz, mm3)));
  len -= 32;
  str += 32;
 }
#endif

#if defined(__SSE2__) || EMU_AVX2

 // align to 16 bytes
 while ((len>0) && ((((intptr_t)str) & 15) != 0)) {
  *str = toupper(*str);
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
  *str = toupper(*str);
  ++str;
 }
}

/* A SIMD function for SSE2 which changes all uppercase ASCII digits to lowercase. */
void StringToLowerUCS2(unsigned short *str,size_t len){
 const char OFFSET = 'a' - 'A';
#if C_AVX2 || EMU_AVX2

 // align to 32 bytes
 while ((len>0) && ((((intptr_t)str) & 31) != 0)) {
  *str = (*str>= 'A' && *str<= 'Z') ? *str += OFFSET : *str;
  len--;
  ++str;
 }
 const __m256i mm1 = _mm256_set1_epi16( 'A'-1 );
 const __m256i mm2 = _mm256_set1_epi16( 'Z' );
 const __m256i mm3 = _mm256_set1_epi16( 0x20 );
 const __m256i mm4 = _mm256_cmpeq_epi16( mm3, mm3 );
 while (len >= 16) {
  __m256i r0 = _mm256_load_si256((__m256i*)str);
  // maskaz contains 0x00 where character between 'A' and 'Z', 0xff otherwise.
  __m256i maskaz = _mm256_or_si256(_mm256_andnot_si256(_mm256_cmpgt_epi16(r0, mm1),mm4), _mm256_cmpgt_epi16(r0, mm2));
  // flip the 6th bit to 0 only for uppercase characters.
  _mm256_store_si256((__m256i*)str, _mm256_xor_si256(r0, _mm256_andnot_si256(maskaz, mm3)));
  len -= 16;
  str += 16;
 }
#endif

#if defined(__SSE2__) || EMU_AVX2

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
#if C_AVX2 || EMU_AVX2

 // align to 32 bytes
 while ((len>0) && ((((intptr_t)str) & 31) != 0)) {
  *str = (*str>= 'a' && *str<= 'z') ? *str -= OFFSET : *str;
  len--;
  ++str;
 }
 const __m256i mm1 = _mm256_set1_epi16( 'a'-1 );
 const __m256i mm2 = _mm256_set1_epi16( 'z' );
 const __m256i mm3 = _mm256_set1_epi16( 0x20 );
 const __m256i mm4 = _mm256_cmpeq_epi16( mm3, mm3 );
 while (len >= 16) {
  __m256i r0 = _mm256_load_si256((__m256i*)str);
  // maskaz contains 0x00 where character between 'a' and 'z', 0xff otherwise.
  __m256i maskaz = _mm256_or_si256(_mm256_andnot_si256(_mm256_cmpgt_epi16(r0, mm1),mm4), _mm256_cmpgt_epi16(r0, mm2));
  // flip the 6th bit to 0 only for lowercase characters.
  _mm256_store_si256((__m256i*)str, _mm256_xor_si256(r0, _mm256_andnot_si256(maskaz, mm3)));
  len -= 16;
  str += 16;
 }
#endif

#if defined(__SSE2__) || EMU_AVX2

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
#if C_AVX2 || EMU_AVX2

 // align to 32 bytes
 while ((len>0) && ((((intptr_t)str) & 31) != 0)) {
  *str = (*str>= 'A' && *str<= 'Z') ? *str += OFFSET : *str;
  len--;
  ++str;
 }
 const __m256i mm1 = _mm256_set1_epi32( 'A'-1 );
 const __m256i mm2 = _mm256_set1_epi32( 'Z' );
 const __m256i mm3 = _mm256_set1_epi32( 0x20 );
 const __m256i mm4 = _mm256_cmpeq_epi32( mm3, mm3 );
 while (len >= 8) {
  __m256i r0 = _mm256_load_si256((__m256i*)str);
  // maskaz contains 0x00 where character between 'A' and 'Z', 0xff otherwise.
  __m256i maskaz = _mm256_or_si256(_mm256_andnot_si256(_mm256_cmpgt_epi32(r0, mm1),mm4), _mm256_cmpgt_epi32(r0, mm2));
  // flip the 6th bit to 0 only for uppercase characters.
  _mm256_store_si256((__m256i*)str, _mm256_xor_si256(r0, _mm256_andnot_si256(maskaz, mm3)));
  len -= 8;
  str += 8;
 }
#endif

#if defined(__SSE2__) || EMU_AVX2

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
#if C_AVX2 || EMU_AVX2

 // align to 32 bytes
 while ((len>0) && ((((intptr_t)str) & 31) != 0)) {
  *str = (*str>= 'a' && *str<= 'z') ? *str -= OFFSET : *str;
  len--;
  ++str;
 }
 const __m256i mm1 = _mm256_set1_epi32( 'a'-1 );
 const __m256i mm2 = _mm256_set1_epi32( 'z' );
 const __m256i mm3 = _mm256_set1_epi32( 0x20 );
 const __m256i mm4 = _mm256_cmpeq_epi32( mm3, mm3 );
 while (len >= 8) {
  __m256i r0 = _mm256_load_si256((__m256i*)str);
  // maskaz contains 0x00 where character between 'a' and 'z', 0xff otherwise.
  __m256i maskaz = _mm256_or_si256(_mm256_andnot_si256(_mm256_cmpgt_epi32(r0, mm1),mm4), _mm256_cmpgt_epi32(r0, mm2));
  // flip the 6th bit to 0 only for lowercase characters.
  _mm256_store_si256((__m256i*)str, _mm256_xor_si256(r0, _mm256_andnot_si256(maskaz, mm3)));
  len -= 8;
  str += 8;
 }
#endif

#if defined(__SSE2__) || EMU_AVX2

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
