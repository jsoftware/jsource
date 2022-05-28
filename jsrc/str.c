/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* string utiliy                                                           */

#include <stddef.h>
#include <ctype.h>

extern void StringToLower(char *str,size_t len);
extern void StringToUpper(char *str,size_t len);
extern void StringToLowerUCS2(unsigned short *str,size_t len);
extern void StringToUpperUCS2(unsigned short *str,size_t len);
extern void StringToLowerUCS4(unsigned int *str,size_t len);
extern void StringToUpperUCS4(unsigned int *str,size_t len);

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
 while (len >= 16) {
  __m128i sv = _mm_load_si128(( __m128i*)str);
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
void StringToUpper(char *str,size_t len)
{
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

void StringToLowerUCS2(unsigned short *str,size_t len)
{
 const char OFFSET = 'a' - 'A';
 while (len-- > 0) {
  *str= (*str>= 'A' && *str<= 'Z') ? *str += OFFSET : *str;
  ++str;
 }
}

void StringToUpperUCS2(unsigned short *str,size_t len)
{
 const char OFFSET = 'a' - 'A';
 while (len-- > 0) {
  *str= (*str>= 'a' && *str<= 'z') ? *str -= OFFSET : *str;
  ++str;
 }
}

void StringToLowerUCS4(unsigned int *str,size_t len)
{
 const char OFFSET = 'a' - 'A';
 while (len-- > 0) {
  *str= (*str>= 'A' && *str<= 'Z') ? *str += OFFSET : *str;
  ++str;
 }
}

void StringToUpperUCS4(unsigned int *str,size_t len)
{
 const char OFFSET = 'a' - 'A';
 while (len-- > 0) {
  *str= (*str>= 'a' && *str<= 'z') ? *str -= OFFSET : *str;
  ++str;
 }
}

