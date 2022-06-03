/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.           */
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
extern size_t Stringrchr(char *str,char ch, size_t stride,size_t len);
extern size_t Stringrchr2(unsigned short *str, unsigned short ch, size_t stride,size_t len);
extern size_t Stringrchr4(unsigned int *str, unsigned int ch, size_t stride,size_t len);

#if C_AVX2 || EMU_AVX2

static size_t srchr(char* str, char ch, size_t len){
 size_t i=len;
 // align to 32 bytes
 while ((i>0) && ((((intptr_t)str+i) & 31) != 0)){if (ch!=str[i-1]) return i; else --i;}
 if(!i) return 0;
/* don't test i>=0 which is always true because size_t is unsigned */
 const __m256i xmm0 = _mm256_set1_epi8( ch );
 const __m256i xmm2 = _mm256_set1_epi8( 0xff );
 while (i > 32) {
  // search for ch
  int mask = 0;
   __m256i xmm1 = _mm256_load_si256((__m256i *)(str+i-32));
   xmm1 = _mm256_andnot_si256(_mm256_cmpeq_epi8(xmm1, xmm0),xmm2);
   if ((mask = _mm256_movemask_epi8(xmm1)) != 0) {   // some character is not ch
    // got 0 somewhere within 32 bytes in xmm1, or within 32 bits in mask
    // find index of last set bit
#if (MMSC_VER)   // make sure <intrin.h> is included
    unsigned long pos;
    _BitScanBackward(&pos, mask);
    i -= (size_t)pos;
#elif defined(__clang__) || ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))) // modern GCC has built-in __builtin_ctz
    i -= __builtin_clz(mask);
#else  // none of choices exist, use local BSR implementation
#error __builtin_clz
#endif
    return i;
  }
  i -= 32;
 }
 while (i > 16) {
  const __m128i xmm0 = _mm_set1_epi8( ch );
  const __m128i xmm2 = _mm_set1_epi8( 0xff );
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
#elif defined(__clang__) || ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))) // modern GCC has built-in __builtin_ctz
    i -= __builtin_clz(mask)-16;  // mask is 32-bits but only lower 16-bits are significant
#else  // none of choices exist, use local BSR implementation
#error __builtin_clz
#endif
    return i;
  }
  i -= 16;
 }

 while (i>0){if (ch!=str[i-1]) return i; else --i;}
 return 0;
}
#elif defined(__SSE2__) || EMU_AVX

static size_t srchr(char* str, char ch, size_t len){
 size_t i=len;
 // align to 16 bytes
 while ((i>0) && ((((intptr_t)str+i) & 15) != 0)){if (ch!=str[i-1]) return i; else --i;}
 if(!i) return 0;
/* don't test i>=0 which is always true because size_t is unsigned */
 const __m128i xmm0 = _mm_set1_epi8( ch );
 const __m128i xmm2 = _mm_set1_epi8( 0xff );
 while (i > 16) {
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
#elif defined(__clang__) || ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))) // modern GCC has built-in __builtin_ctz
    i -= __builtin_clz(mask)-16;  // mask is 32-bits but only lower 16-bits are significant
#else  // none of choices exist, use local BSR implementation
#error __builtin_clz
#endif
    return i;
  }
  i -= 16;
 }

 while (i>0){if (ch!=str[i-1]) return i; else --i;}
 return 0;
}
#else
static size_t srchr(char* str, char ch, size_t len){
/* don't test i>=0 which is always true because size_t is unsigned */
 for(size_t i=len; i>0; i--) if (ch!=str[i-1]) return i;
 return 0;
}
#endif

static size_t srchr2(unsigned short* str, unsigned short ch, size_t len){
/* don't test i>=0 which is always true because size_t is unsigned */
 for(size_t i=len; i>0; i--) if (ch!=str[i-1]) return i;
 return 0;
}

static size_t srchr4(unsigned int* str, unsigned int ch, size_t len){
/* don't test i>=0 which is always true because size_t is unsigned */
 for(size_t i=len; i>0; i--) if (ch!=str[i-1]) return i;
 return 0;
}

#if C_AVX2 || EMU_AVX2

/* A SIMD function for SSE2 which changes all uppercase ASCII digits to lowercase. */
void StringToLower(char *str,size_t len){
 while (len >= 32) {
  __m256i sv = _mm256_loadu_si256(( __m256i*)str);
  /* From A */
  __m256i rangeshift = _mm256_sub_epi8(sv, _mm256_set1_epi8((char)('A'+256)));
  /* To Z */
  __m256i nomodify = _mm256_cmpgt_epi8(rangeshift, _mm256_set1_epi8(-256 + 25));
  /* ^ ' ' */
  _mm_storeu_si256((__m256i*)str, _mm256_xor_si256(sv, _mm256_andnot_si256(nomodify, _mm256_set1_epi8(0x20))));
  len -= 32;
  str += 32;
 }
 while (len >= 16) {
  __m128i sv = _mm_loadu_si128(( __m128i*)str);
  /* From A */
  __m128i rangeshift = _mm_sub_epi8(sv, _mm_set1_epi8((char)('A'+128)));
  /* To Z */
  __m128i nomodify = _mm_cmpgt_epi8(rangeshift, _mm_set1_epi8(-128 + 25));
  /* ^ ' ' */
  _mm_storeu_si128((__m128i*)str, _mm_xor_si128(sv, _mm_andnot_si128(nomodify, _mm_set1_epi8(0x20))));
  len -= 16;
  str += 16;
 }
 while (len-- > 0) {
  *str = tolower(*str);
  ++str;
 }
}

/* Same, but to uppercase. */
void StringToUpper(char *str,size_t len){
 while (len >= 32) {
 // Unaligned load.
 __m256i r0 = _mm256_loadu_si256((__m256i*)str);
 // It is also possible to perform aligned loads by skipping enough characters in the front

 // maskaz contains 0x00 where character between 'a' and 'z', 0xff otherwise.
 __m256i maskaz = _mm256_or_si256(_mm256_cmplt_epi8(r0, _mm256_set1_epi8( 'a' )), _mm256_cmpgt_epi8(r0, _mm256_set1_epi8( 'z' )));

 // Set the 6th bit to 0 only for lowercase characters.
 _mm_storeu_si256((__m256i*)str, _mm256_andnot_si256(_mm256_andnot_si256(maskaz, _mm256_set1_epi8(0x20)),r0));
  len -= 32;
  str += 32;
 }
 while (len >= 16) {
 // Unaligned load.
 __m128i r0 = _mm_loadu_si128((__m128i*)str);
 // It is also possible to perform aligned loads by skipping enough characters in the front

 // maskaz contains 0x00 where character between 'a' and 'z', 0xff otherwise.
 __m128i maskaz = _mm_or_si128(_mm_cmplt_epi8(r0, _mm_set1_epi8( 'a' )), _mm_cmpgt_epi8(r0, _mm_set1_epi8( 'z' )));

 // Set the 6th bit to 0 only for lowercase characters.
 _mm_storeu_si128((__m128i*)str, _mm_andnot_si128(_mm_andnot_si128(maskaz, _mm_set1_epi8(0x20)),r0));
  len -= 16;
  str += 16;
 }
 while (len-- > 0) {
  *str = toupper(*str);
  ++str;
 }
}
#elif defined(__SSE2__) || EMU_AVX

/* A SIMD function for SSE2 which changes all uppercase ASCII digits to lowercase. */
void StringToLower(char *str,size_t len){
 while (len >= 16) {
  __m128i sv = _mm_loadu_si128(( __m128i*)str);
  /* From A */
  __m128i rangeshift = _mm_sub_epi8(sv, _mm_set1_epi8((char)('A'+128)));
  /* To Z */
  __m128i nomodify = _mm_cmpgt_epi8(rangeshift, _mm_set1_epi8(-128 + 25));
  /* ^ ' ' */
  _mm_storeu_si128((__m128i*)str, _mm_xor_si128(sv, _mm_andnot_si128(nomodify, _mm_set1_epi8(0x20))));
  len -= 16;
  str += 16;
 }
 while (len-- > 0) {
  *str = tolower(*str);
  ++str;
 }
}

/* Same, but to uppercase. */
void StringToUpper(char *str,size_t len){
 while (len >= 16) {
 // Unaligned load.
 __m128i r0 = _mm_loadu_si128((__m128i*)str);
 // It is also possible to perform aligned loads by skipping enough characters in the front

 // maskaz contains 0x00 where character between 'a' and 'z', 0xff otherwise.
 __m128i maskaz = _mm_or_si128(_mm_cmplt_epi8(r0, _mm_set1_epi8( 'a' )), _mm_cmpgt_epi8(r0, _mm_set1_epi8( 'z' )));

 // Set the 6th bit to 0 only for lowercase characters.
 _mm_storeu_si128((__m128i*)str, _mm_andnot_si128(_mm_andnot_si128(maskaz, _mm_set1_epi8(0x20)),r0));
  len -= 16;
  str += 16;
 }
 while (len-- > 0) {
  *str = toupper(*str);
  ++str;
 }
}
#elif defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>

/* Literally the exact same code as above, but for NEON. */
void StringToLower(char *str,size_t len){
 const uint8x16_t asciiA = vdupq_n_u8('A' - 1);
 const uint8x16_t asciiZ = vdupq_n_u8('Z' + 1);
 const uint8x16_t diff = vdupq_n_u8('a' - 'A');
 while (len >= 16) {
  uint8x16_t inp = vld1q_u8((uint8_t *)str);
  uint8x16_t greaterThanA = vcgtq_u8(inp, asciiA);
  uint8x16_t lessEqualZ = vcltq_u8(inp, asciiZ);
  uint8x16_t mask = vandq_u8(greaterThanA, lessEqualZ);
  uint8x16_t toAdd = vandq_u8(mask, diff);
  uint8x16_t added = vaddq_u8(inp, toAdd);
  vst1q_u8((uint8_t *)str, added);
  len -= 16;
  str += 16;
 }
 while (len-- > 0) {
  *str = tolower(*str);
  ++str;
 }
}

/* Literally the exact same code as above, but for NEON. */
void StringToUpper(char *str,size_t len){
 const uint8x16_t asciia = vdupq_n_u8('a' - 1);
 const uint8x16_t asciiz = vdupq_n_u8('z' + 1);
 const uint8x16_t diff = vdupq_n_u8('a' - 'A');
 while (len >= 16) {
  uint8x16_t inp = vld1q_u8((uint8_t *)str);
  uint8x16_t greaterThana = vcgtq_u8(inp, asciia);
  uint8x16_t lessEqualz = vcltq_u8(inp, asciiz);
  uint8x16_t mask = vandq_u8(greaterThana, lessEqualz);
  uint8x16_t toSub = vandq_u8(mask, diff);
  uint8x16_t added = vsubq_u8(inp, toSub);
  vst1q_u8((uint8_t *)str, added);
  len -= 16;
  str += 16;
 }
 while (len-- > 0) {
  *str = toupper(*str);
  ++str;
 }
}
#else
/* Just go scalar. */
void StringToLower(char *str,size_t len){
 while (len-- > 0) {
  *str = tolower(*str);
  ++str;
 }
}

void StringToUpper(char *str,size_t len){
 while (len-- > 0) {
  *str = toupper(*str);
  ++str;
 }
}
#endif

void StringToLowerUCS2(unsigned short *str,size_t len){
 const char OFFSET = 'a' - 'A';
 while (len-- > 0) {
  *str= (*str>= 'A' && *str<= 'Z') ? *str += OFFSET : *str;
  ++str;
 }
}

void StringToUpperUCS2(unsigned short *str,size_t len){
 const char OFFSET = 'a' - 'A';
 while (len-- > 0) {
  *str= (*str>= 'a' && *str<= 'z') ? *str -= OFFSET : *str;
  ++str;
 }
}

void StringToLowerUCS4(unsigned int *str,size_t len){
 const char OFFSET = 'a' - 'A';
 while (len-- > 0) {
  *str= (*str>= 'A' && *str<= 'Z') ? *str += OFFSET : *str;
  ++str;
 }
}

void StringToUpperUCS4(unsigned int *str,size_t len){
 const char OFFSET = 'a' - 'A';
 while (len-- > 0) {
  *str= (*str>= 'a' && *str<= 'z') ? *str -= OFFSET : *str;
  ++str;
 }
}

size_t Stringrchr(char *str, char ch, size_t stride, size_t len){
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
 size_t i=len,ln=0;
 while (i-- > 0) {
  size_t l=srchr4(str,ch,stride);
  ln=(ln<l)?l:ln;
  if(ln==stride) return ln;
  str+=stride;
 }
 return ln;
}
