/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
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

#include "j.h"
#include "x.h"
#undef num
#include "cpuinfo.h"
// static UC hwsha1=0,hwsha2=0,hwssse3=0,hwsse41=0,hwavx=0,hwavx2=0;

#ifndef BYTE_ORDER
#if defined(MMSC_VER)
#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN    4321
#define BYTE_ORDER LITTLE_ENDIAN
#endif
#endif

#include "openssl/sha/openssl.h"
#include "openssl/sha/md4.h"
#include "openssl/sha/md5.h"
#include "openssl/sha/sha.h"
#include "openssl/sha/sha3.h"

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
  F2RANK(0,1,jtshasum2,DUMMYSELF);  // do rank loop if necessary
  RZ(a=vi(a));
  n=AN(w);
  v=UAV(w);
  ASSERT(!n||AT(w)&LIT,EVDOMAIN);
  I s=AV(a)[0];

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
  10   KECCAK_224
  11   KECCAK_256
  12   KECCAK_384
  13   KECCAK_512
  14   MD4
  15   MD5
  */

// hardwared optimized openssl failed to build,
// SHA1 SHA224 SHA256 for armv8 x86_64
// use other sources

  switch((s>0)?s:-s) {
  case 1: {
    UC md[20],dh[2*20];
    SHA1(v, n, md);
    z = (s<0)?str(20, md):str(2*20, tohex(dh,md,20));
  }
  break;
  case 2: {
    UC md[32],dh[2*28];
    SHA224(v, n, md);
    z = (s<0)?str(28, md):str(2*28, tohex(dh,md,28));
  }
  break;
  case 3: {
    UC md[32],dh[2*32];
    SHA256(v, n, md);
    z = (s<0)?str(32, md):str(2*32, tohex(dh,md,32));
  }
  break;
  case 4: {
    UC md[64],dh[2*48];
    SHA384(v, n, md);
    z = (s<0)?str(48, md):str(2*48, tohex(dh,md,48));
  }
  break;
  case 5: {
    UC md[64],dh[2*64];
    SHA512(v, n, md);
    z = (s<0)?str(64, md):str(2*64, tohex(dh,md,64));
  }
  break;
  case 6:
  case 10: {
    UC d[64], dh[2*28];
    KECCAK1600_CTX c;
    sha3_reset(&c);
    sha3_init(&c, (10==((s>0)?s:-s))?0x01:0x06, 28*8);
    sha3_update(&c, v, n);
    sha3_final(d, &c);
    z = (s<0)?str(28, d):str(2*28, tohex(dh,d,28));
  }
  break;
  case 7:
  case 11: {
    UC d[64], dh[2*32];
    KECCAK1600_CTX c;
    sha3_reset(&c);
    sha3_init(&c, (11==((s>0)?s:-s))?0x01:0x06, 32*8);
    sha3_update(&c, v, n);
    sha3_final(d, &c);
    z = (s<0)?str(32, d):str(2*32, tohex(dh,d,32));
  }
  break;
  case 8:
  case 12: {
    UC d[64], dh[2*48];
    KECCAK1600_CTX c;
    sha3_reset(&c);
    sha3_init(&c, (12==((s>0)?s:-s))?0x01:0x06, 48*8);
    sha3_update(&c, v, n);
    sha3_final(d, &c);
    z = (s<0)?str(48, d):str(2*48, tohex(dh,d,48));
  }
  break;
  case 9:
  case 13: {
    UC d[64], dh[2*64];
    KECCAK1600_CTX c;
    sha3_reset(&c);
    sha3_init(&c, (13==((s>0)?s:-s))?0x01:0x06, 64*8);
    sha3_update(&c, v, n);
    sha3_final(d, &c);
    z = (s<0)?str(64, d):str(2*64, tohex(dh,d,64));
  }
  break;
  case 14: {
    UC md[16],dh[2*16];
    MD4(v, n, md);
    z = (s<0)?str(16, md):str(2*16, tohex(dh,md,16));
  }
  break;
  case 15: {
    UC md[16],dh[2*16];
    MD5(v, n, md);
    z = (s<0)?str(16, md):str(2*16, tohex(dh,md,16));
  }
  break;
  default:
    ASSERT(0,EVDOMAIN);
  }
  R z;
}
