/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: SHA calculation                                                  */

#ifdef ANDROID
#include <endian.h>
#elif defined(__MACH__)
#include <machine/endian.h>
#else
#include <sys/types.h>
#endif

#ifndef BYTE_ORDER
#ifdef _MSC_VER
#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN    4321
#define BYTE_ORDER LITTLE_ENDIAN
#endif
#endif

#include "j.h"
#include "x.h"

#include "sha/sha1.h"
#include "sha/sha1.c"
#include "sha/sha2.h"
#include "sha/sha2.c"
#include "sha/sha3.h"
#include "sha/sha3.c"

static const char *hex_digits = "0123456789abcdef";

/* not null terminated */
static UC* tohex(UC* dest, UC* src, I len)
{
  I i;
  UC* d = src;
  UC* buffer = dest;
  for (i = 0; i < len; i++) {
    *buffer++ = hex_digits[(*d & 0xf0) >> 4];
    *buffer++ = hex_digits[*d & 0x0f];
    d++;
  }
  R dest;
}

F1(jtshasum1)
{
  R shasum2(sc(1),w);
}

F2(jtshasum2)
{
  I n;
  A z;
  UC *v;
  RZ(a&&w);
  RZ(a=vi(a));
  ASSERT(1==AN(a),EVLENGTH);
  ASSERT(0==AR(a),EVRANK);
  ASSERT(1>=AR(w),EVRANK);
  n=AN(w);
  v=UAV(w);
  ASSERT(!n||AT(w)&LIT,EVDOMAIN);
  I s=*AV(a);

  /*
  1    SHA1
  2    SHA224
  3    SHA256
  4    SHA384
  5    SHA512
  6    SHA3_224
  7    SHA3_256
  8    SHA3_384
  9    SHA3_512
  */

  switch((s>0)?s:-s) {
  case 1: {
    UC d[20],dh[2*20];
    SHA1_CTX c;
    SHA1Init(&c);
    SHA1Update(&c, v, n);
    SHA1Final(d, &c);
    z = (s<0)?str(20, d):str(2*20, tohex(dh,d,20));
  }
  break;
  case 2: {
    UC d[28],dh[2*28];
    SHA256_CTX c;
    SHA224_Init(&c);
    SHA256_Update(&c, (const uint8_t *)v, n);
    SHA256_Final(d, &c);
    z = (s<0)?str(28, d):str(2*28, tohex(dh,d,28));
  }
  break;
  case 3: {
    UC d[32],dh[2*32];
    SHA256_CTX c;
    SHA256_Init(&c);
    SHA256_Update(&c, (const uint8_t *)v, n);
    SHA256_Final(d, &c);
    z = (s<0)?str(32, d):str(2*32, tohex(dh,d,32));
  }
  break;
  case 4: {
    UC d[48],dh[2*48];
    SHA384_CTX c;
    SHA384_Init(&c);
    SHA384_Update(&c, (const uint8_t *)v, n);
    SHA384_Final(d, &c);
    z = (s<0)?str(48, d):str(2*48, tohex(dh,d,48));
  }
  break;
  case 5: {
    UC d[64],dh[2*64];
    SHA512_CTX c;
    SHA512_Init(&c);
    SHA512_Update(&c, (const uint8_t *)v, n);
    SHA512_Final(d, &c);
    z = (s<0)?str(64, d):str(2*64, tohex(dh,d,64));
  }
  break;
  case 6: {
    UC *d, dh[2*28];
    sha3_context c;
    sha3_Init224(&c);
    sha3_Update(&c, v, n);
    d=(UC*)sha3_Finalize(&c);
    z = (s<0)?str(28, d):str(2*28, tohex(dh,d,28));
  }
  break;
  case 7: {
    UC *d, dh[2*32];
    sha3_context c;
    sha3_Init256(&c);
    sha3_Update(&c, v, n);
    d=(UC*)sha3_Finalize(&c);
    z = (s<0)?str(32, d):str(2*32, tohex(dh,d,32));
  }
  break;
  case 8: {
    UC *d, dh[2*48];
    sha3_context c;
    sha3_Init384(&c);
    sha3_Update(&c, v, n);
    d=(UC*)sha3_Finalize(&c);
    z = (s<0)?str(48, d):str(2*48, tohex(dh,d,48));
  }
  break;
  case 9: {
    UC *d, dh[2*64];
    sha3_context c;
    sha3_Init512(&c);
    sha3_Update(&c, v, n);
    d=(UC*)sha3_Finalize(&c);
    z = (s<0)?str(64, d):str(2*64, tohex(dh,d,64));
  }
  break;
  default:
    ASSERT(0,EVDOMAIN);
  }
  R z;
}

