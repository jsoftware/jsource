/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* SHA3 calcuation                                                         */

#ifndef SHA3_H
#define SHA3_H

#include <stdint.h>
#include <string.h>

#define SHA3_ASSERT( x )
#if defined(_MSC_VER)
#define SHA3_TRACE( format, ...)
#define SHA3_TRACE_BUF( format, buf, l, ...)
#else
#define SHA3_TRACE(format, args...)
#define SHA3_TRACE_BUF(format, buf, l, args...)
#endif

//#define SHA3_USE_KECCAK
/*
 * Define SHA3_USE_KECCAK to run "pure" Keccak, as opposed to SHA3.
 * The tests that this macro enables use the input and output from [Keccak]
 * (see the reference below). The used test vectors aren't correct for SHA3,
 * however, they are helpful to verify the implementation.
 * SHA3_USE_KECCAK only changes one line of code in Finalize.
 */

#if defined(_MSC_VER)
#define SHA3_CONST(x) x
#else
#define SHA3_CONST(x) x##L
#endif

/* The following state definition should normally be in a separate
 * header file
 */

/* 'Words' here refers to uint64_t */
#define SHA3_KECCAK_SPONGE_WORDS \
 (((1600)/8/*bits to byte*/)/sizeof(uint64_t))
typedef struct sha3_context_ {
  uint64_t saved;             /* the portion of the input message that we
                                 * didn't consume yet */
  union {                     /* Keccak's state */
    uint64_t s[SHA3_KECCAK_SPONGE_WORDS];
    uint8_t sb[SHA3_KECCAK_SPONGE_WORDS * 8];
  };
  unsigned byteIndex;         /* 0..7--the next byte after the set one
                                 * (starts from 0; 0--none are buffered) */
  unsigned wordIndex;         /* 0..24--the next word to integrate input
                                 * (starts from 0) */
  unsigned capacityWords;     /* the double size of the hash output in
                                 * words (e.g. 16 for Keccak 512) */
} sha3_context;

static void sha3_Init224(void *priv);
static void sha3_Init256(void *priv);
static void sha3_Init384(void *priv);
static void sha3_Init512(void *priv);
static void sha3_Update(void *priv, void const *bufIn, size_t len);
static void const * sha3_Finalize(void *priv);

#endif /* SHA3_H */
