/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* MD4 calcuation                                                         */

#if !defined(_MD4_H)
#define _MD4_H

/* Any 32-bit or wider unsigned integer data type will do */
typedef uint32_t MD4_u32plus;

typedef struct {
	MD4_u32plus lo, hi;
	MD4_u32plus abcd[4];
	unsigned char buffer[64];
	MD4_u32plus block[16];
} MD4_CTX;

extern void MD4_Init(MD4_CTX *ctx);
extern void MD4_Update(MD4_CTX *ctx, const void *data, size_t size);
extern void MD4_Final(unsigned char *result, MD4_CTX *ctx);

#endif
