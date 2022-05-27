/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* string utiliy                                                           */

#include <stddef.h>
#include <ctype.h>

extern void StringToLower(char *str,size_t len);
extern void StringToUpper(char *str,size_t len);

/* msvc does not define __SSE2__ */
#if !defined(__SSE2__)
#if defined(_MSC_VER) && defined(_WIN32)
#if (defined(_M_AMD64) || defined(_M_X64))
#define __SSE2__ 1
#elif _M_IX86_FP==2
#define __SSE2__ 1
#endif
#endif
#endif

#ifdef __SSE2__
#include <immintrin.h>

/* A SIMD function for SSE2 which changes all uppercase ASCII digits to lowercase. */
void StringToLower(char *str,size_t len)
{
 const __m128i asciiA = _mm_set1_epi8('A' - 1);
 const __m128i asciiZ = _mm_set1_epi8('Z' + 1);
 const __m128i diff = _mm_set1_epi8('a' - 'A');
 while (len >= 16) {
  __m128i inp = _mm_loadu_si128((__m128i*)str);
  /* > 'A': 0xff, < 'A': 0x00 */
  __m128i greaterThanA = _mm_cmpgt_epi8(inp, asciiA);
  /* <= 'Z': 0xff, > 'Z': 0x00 */
  __m128i lessEqualZ = _mm_cmplt_epi8(inp, asciiZ);
  /* 'Z' >= x >= 'A': 0xFF, else 0x00 */
  __m128i mask = _mm_and_si128(greaterThanA, lessEqualZ);
  /* 'Z' >= x >= 'A': 'a' - 'A', else 0x00 */
  __m128i toAdd = _mm_and_si128(mask, diff);
  /* add to change to lowercase */
  __m128i added = _mm_add_epi8(inp, toAdd);
  _mm_storeu_si128((__m128i *)str, added);
  len -= 16;
  str += 16;
 }
 while (len-- > 0) {
  *str = tolower(*str);
  ++str;
 }
}

/* Same, but to uppercase. */
void StringToUpper(char *str,size_t len)
{
 const __m128i asciia = _mm_set1_epi8('a' - 1);
 const __m128i asciiz = _mm_set1_epi8('z' + 1);
 const __m128i diff = _mm_set1_epi8('a' - 'A');
 while (len >= 16) {
  __m128i inp = _mm_loadu_si128((__m128i*)str);
  /* > 'a': 0xff, < 'a': 0x00 */
  __m128i greaterThana = _mm_cmpgt_epi8(inp, asciia);
  /* <= 'z': 0xff, > 'z': 0x00 */
  __m128i lessEqualz = _mm_cmplt_epi8(inp, asciiz);
  /* 'z' >= x >= 'a': 0xFF, else 0x00 */
  __m128i mask = _mm_and_si128(greaterThana, lessEqualz);
  /* 'z' >= x >= 'a': 'a' - 'A', else 0x00 */
  __m128i toSub = _mm_and_si128(mask, diff);
  /* subtract to change to uppercase */
  __m128i added = _mm_sub_epi8(inp, toSub);
  _mm_storeu_si128((__m128i *)str, added);
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
void StringToLower(char *str,size_t len)
{
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
void StringToUpper(char *str,size_t len)
{
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
void StringToLower(char *str,size_t len)
{
 while (len-- > 0) {
  *str = tolower(*str);
  ++str;
 }
}

void StringToUpper(char *str,size_t len)
{
 while (len-- > 0) {
  *str = toupper(*str);
  ++str;
 }
}
#endif
