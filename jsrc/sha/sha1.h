/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* SHA1 calcuation                                                         */

#ifndef SHA1_H
#define SHA1_H

/*
   SHA-1 in C
   By Steve Reid <steve@edmweb.com>
   100% Public Domain
 */

#include <stdint.h>

typedef struct {
  uint32_t state[5];
  size_t count[2];
  unsigned char buffer[64];
} SHA1_CTX;

static void SHA1Transform(
  uint32_t state[5],
  const unsigned char buffer[64]
);

static void SHA1Init(
  SHA1_CTX * context
);

static void SHA1Update(
  SHA1_CTX * context,
  const unsigned char *data,
  size_t len
);

static void SHA1Final(
  unsigned char digest[20],
  SHA1_CTX * context
);

/*
void SHA1(
  char *hash_out,
  const char *str,
  size_t len);
*/

#endif /* SHA1_H */
