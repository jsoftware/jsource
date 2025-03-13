/*
 * Copyright 1995-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdio.h>
#include <string.h>
#include "openssl.h"
#include "sha.h"
#include "sha_locl.h"

unsigned char *SHA1(const unsigned char *d, size_t n, unsigned char *md)
{
    SHA_CTX c;
    static unsigned char m[SHA_DIGEST_LENGTH];

    if (md == NULL)
        md = m;
#if defined(__OpenBSD__)
  fprintf(stdout,"openssl a1 \n");
#endif
    if (!SHA1_Init(&c))
        return NULL;
#if defined(__OpenBSD__)
  fprintf(stdout,"openssl a2 \n");
#endif
    SHA1_Update(&c, d, n);
#if defined(__OpenBSD__)
  fprintf(stdout,"openssl a3 \n");
#endif
    SHA1_Final(md, &c);
#if defined(__OpenBSD__)
  fprintf(stdout,"openssl a4 \n");
#endif
    OPENSSL_cleanse(&c, sizeof(c));
#if defined(__OpenBSD__)
  fprintf(stdout,"openssl a5 \n");
#endif
    return md;
}
