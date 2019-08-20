/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* SHA1 calcuation                                                         */

/*
SHA-1 in C
By Steve Reid <steve@edmweb.com>
100% Public Domain

Test Vectors (from FIPS PUB 180-1)
"abc"
  A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
  84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
A million repetitions of "a"
  34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
*/

/* #define LITTLE_ENDIAN * This should be #define'd already, if true. */
/* #define SHA1HANDSOFF * Copies data before messing with it. */

#define SHA1HANDSOFF

// #include <stdio.h>
#include <string.h>

/* for uint32_t */
#include <stdint.h>

// #include "sha1.h"

#if defined(__aarch64__)
extern void sha1_process_arm(uint32_t state[5], const uint8_t data[], uintptr_t length);
#endif
#if defined(_M_X64) || defined(__x86_64__)
/* ssse3 */
extern void sha1_update_intel(int *hash, const char* input, size_t num_blocks );
#endif

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
#if BYTE_ORDER == LITTLE_ENDIAN
#define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) \
    |(rol(block->l[i],8)&0x00FF00FF))
#elif BYTE_ORDER == BIG_ENDIAN
#define blk0(i) block->l[i]
#else
#error "Endianness not defined!"
#endif
#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
    ^block->l[(i+2)&15]^block->l[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define SHA1R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define SHA1R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define SHA1R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define SHA1R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define SHA1R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);


/* Hash a single 512-bit block. This is the core of the algorithm. */

static void SHA1Transform(
  uint32_t state[5],
  const unsigned char buffer[64]
)
{
  uint32_t a, b, c, d, e;

  typedef union {
    unsigned char c[64];
    uint32_t l[16];
  } CHAR64LONG16;

#ifdef SHA1HANDSOFF
  CHAR64LONG16 block[1];      /* use array to appear as a pointer */

  memcpy(block, buffer, 64);
#else
  /* The following had better never be used because it causes the
   * pointer-to-const buffer to be cast into a pointer to non-const.
   * And the result is written through.  I threw a "const" in, hoping
   * this will cause a diagnostic.
   */
  CHAR64LONG16 *block = (const CHAR64LONG16 *) buffer;
#endif
  /* Copy context->state[] to working vars */
  a = state[0];
  b = state[1];
  c = state[2];
  d = state[3];
  e = state[4];
  /* 4 rounds of 20 operations each. Loop unrolled. */
  SHA1R0(a, b, c, d, e, 0);
  SHA1R0(e, a, b, c, d, 1);
  SHA1R0(d, e, a, b, c, 2);
  SHA1R0(c, d, e, a, b, 3);
  SHA1R0(b, c, d, e, a, 4);
  SHA1R0(a, b, c, d, e, 5);
  SHA1R0(e, a, b, c, d, 6);
  SHA1R0(d, e, a, b, c, 7);
  SHA1R0(c, d, e, a, b, 8);
  SHA1R0(b, c, d, e, a, 9);
  SHA1R0(a, b, c, d, e, 10);
  SHA1R0(e, a, b, c, d, 11);
  SHA1R0(d, e, a, b, c, 12);
  SHA1R0(c, d, e, a, b, 13);
  SHA1R0(b, c, d, e, a, 14);
  SHA1R0(a, b, c, d, e, 15);
  SHA1R1(e, a, b, c, d, 16);
  SHA1R1(d, e, a, b, c, 17);
  SHA1R1(c, d, e, a, b, 18);
  SHA1R1(b, c, d, e, a, 19);
  SHA1R2(a, b, c, d, e, 20);
  SHA1R2(e, a, b, c, d, 21);
  SHA1R2(d, e, a, b, c, 22);
  SHA1R2(c, d, e, a, b, 23);
  SHA1R2(b, c, d, e, a, 24);
  SHA1R2(a, b, c, d, e, 25);
  SHA1R2(e, a, b, c, d, 26);
  SHA1R2(d, e, a, b, c, 27);
  SHA1R2(c, d, e, a, b, 28);
  SHA1R2(b, c, d, e, a, 29);
  SHA1R2(a, b, c, d, e, 30);
  SHA1R2(e, a, b, c, d, 31);
  SHA1R2(d, e, a, b, c, 32);
  SHA1R2(c, d, e, a, b, 33);
  SHA1R2(b, c, d, e, a, 34);
  SHA1R2(a, b, c, d, e, 35);
  SHA1R2(e, a, b, c, d, 36);
  SHA1R2(d, e, a, b, c, 37);
  SHA1R2(c, d, e, a, b, 38);
  SHA1R2(b, c, d, e, a, 39);
  SHA1R3(a, b, c, d, e, 40);
  SHA1R3(e, a, b, c, d, 41);
  SHA1R3(d, e, a, b, c, 42);
  SHA1R3(c, d, e, a, b, 43);
  SHA1R3(b, c, d, e, a, 44);
  SHA1R3(a, b, c, d, e, 45);
  SHA1R3(e, a, b, c, d, 46);
  SHA1R3(d, e, a, b, c, 47);
  SHA1R3(c, d, e, a, b, 48);
  SHA1R3(b, c, d, e, a, 49);
  SHA1R3(a, b, c, d, e, 50);
  SHA1R3(e, a, b, c, d, 51);
  SHA1R3(d, e, a, b, c, 52);
  SHA1R3(c, d, e, a, b, 53);
  SHA1R3(b, c, d, e, a, 54);
  SHA1R3(a, b, c, d, e, 55);
  SHA1R3(e, a, b, c, d, 56);
  SHA1R3(d, e, a, b, c, 57);
  SHA1R3(c, d, e, a, b, 58);
  SHA1R3(b, c, d, e, a, 59);
  SHA1R4(a, b, c, d, e, 60);
  SHA1R4(e, a, b, c, d, 61);
  SHA1R4(d, e, a, b, c, 62);
  SHA1R4(c, d, e, a, b, 63);
  SHA1R4(b, c, d, e, a, 64);
  SHA1R4(a, b, c, d, e, 65);
  SHA1R4(e, a, b, c, d, 66);
  SHA1R4(d, e, a, b, c, 67);
  SHA1R4(c, d, e, a, b, 68);
  SHA1R4(b, c, d, e, a, 69);
  SHA1R4(a, b, c, d, e, 70);
  SHA1R4(e, a, b, c, d, 71);
  SHA1R4(d, e, a, b, c, 72);
  SHA1R4(c, d, e, a, b, 73);
  SHA1R4(b, c, d, e, a, 74);
  SHA1R4(a, b, c, d, e, 75);
  SHA1R4(e, a, b, c, d, 76);
  SHA1R4(d, e, a, b, c, 77);
  SHA1R4(c, d, e, a, b, 78);
  SHA1R4(b, c, d, e, a, 79);
  /* Add the working vars back into context.state[] */
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
  /* Wipe variables */
  a = b = c = d = e = 0;
#ifdef SHA1HANDSOFF
  memset(block, '\0', sizeof(block));
#endif
}


/* SHA1Init - Initialize new context */

static void SHA1Init(
  SHA1_CTX * context
)
{
  /* SHA1 initialization constants */
  context->state[0] = 0x67452301;
  context->state[1] = 0xEFCDAB89;
  context->state[2] = 0x98BADCFE;
  context->state[3] = 0x10325476;
  context->state[4] = 0xC3D2E1F0;
  context->count[0] = context->count[1] = 0;
}


/* Run your data through this. */

static void SHA1Update(
  SHA1_CTX * context,
  const unsigned char *data,
  size_t len
)
{
  size_t i;
  size_t j;

  j = context->count[0];
  if ((context->count[0] += len << 3) < j)
    context->count[1]++;
  context->count[1] += (len >> 29);
  j = (j >> 3) & 63;
  if ((j + len) > 63) {
    memcpy(&context->buffer[j], data, (i = 64 - j));
#if defined(__aarch64__)
    if(hwsha1)
      sha1_process_arm(context->state, (const uint8_t *)context->buffer, 64);
    else
      SHA1Transform(context->state, context->buffer);
#elif defined(_M_X64) || defined(__x86_64__)
    if(hwssse3)
      sha1_update_intel(context->state, context->buffer, 1);
    else
      SHA1Transform(context->state, context->buffer);
#else
    SHA1Transform(context->state, context->buffer);
#endif
#if defined(__aarch64__)
    if(hwsha1 && i + 63 < len) {
      UI iter = (len - i) >> 6;
      sha1_process_arm(context->state, (const uint8_t *)&data[i], iter << 6);
      i += iter << 6;
    }
#elif defined(_M_X64) || defined(__x86_64__)
    if(hwssse3 && i + 63 < len) {
      UI iter = (len - i) >> 6;
      sha1_update_intel(context->state, &data[i], iter);
      i += iter << 6;
    }
#endif
    for (; i + 63 < len; i += 64) {
#if defined(__aarch64__)
      if(hwsha1)
        sha1_process_arm(context->state, (const uint8_t *)&data[i], 64);
      else
        SHA1Transform(context->state, &data[i]);
#elif defined(_M_X64) || defined(__x86_64__)
      if(hwssse3)
        sha1_update_intel(context->state, &data[i], 1);
      else
        SHA1Transform(context->state, &data[i]);
#else
      SHA1Transform(context->state, &data[i]);
#endif
    }
    j = 0;
  } else
    i = 0;
  memcpy(&context->buffer[j], &data[i], len - i);
}

/* Add padding and return the message digest. */

static void SHA1Final(
  unsigned char digest[20],
  SHA1_CTX * context
)
{
  unsigned int i;

  unsigned char finalcount[8];

  unsigned char c;

#if 0    /* untested "improvement" by DHR */
  /* Convert context->count to a sequence of bytes
   * in finalcount.  Second element first, but
   * big-endian order within element.
   * But we do it all backwards.
   */
  unsigned char *fcp = &finalcount[8];

  for (i = 0; i < 2; i++) {
    uint32_t t = context->count[i];

    int j;

    for (j = 0; j < 4; t >>= 8, j++)
      *--fcp = (unsigned char) t
    }
#else
  for (i = 0; i < 8; i++) {
    finalcount[i] = (unsigned char) ((context->count[(i >= 4 ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 255);      /* Endian independent */
  }
#endif
  c = 0200;
  SHA1Update(context, &c, 1);
  while ((context->count[0] & 504) != 448) {
    c = 0000;
    SHA1Update(context, &c, 1);
  }
  SHA1Update(context, finalcount, 8); /* Should cause a SHA1Transform() */
  for (i = 0; i < 20; i++) {
    digest[i] = (unsigned char)
                ((context->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
  }
  /* Wipe variables */
  memset(context, '\0', sizeof(*context));
  memset(&finalcount, '\0', sizeof(finalcount));
}

/*
void SHA1(
  char *hash_out,
  const char *str,
  size_t len)
{
  SHA1_CTX ctx;
  size_t ii;

  SHA1Init(&ctx);
  for (ii=0; ii<len; ii+=1)
    SHA1Update(&ctx, (const unsigned char*)str + ii, 1);
  SHA1Final((unsigned char *)hash_out, &ctx);
  hash_out[20] = '\0';
}
*/

