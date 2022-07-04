#include <stdint.h>

#ifndef NO_SHA_ASM

#if defined(__aarch64__)||defined(_M_ARM64)
#define SHA1_ASM
#define SHA256_ASM
#define SHA512_ASM
#define KECCAK1600_ASM
#define SELFTEST
#define MD5_ASM
#elif defined(__aarch32__)||defined(__arm__)||defined(_M_ARM)
#define SHA1_ASM
#define SHA256_ASM
#define SHA512_ASM
#define KECCAK1600_ASM
#define SELFTEST
// #define MD5_ASM
#elif defined(__x86_64__)||defined(_M_X64)
#define SHA1_ASM
#define SHA256_ASM
#define SHA512_ASM
#define KECCAK1600_ASM
#define SELFTEST
#define MD5_ASM
#elif defined(__i386__)||defined(_M_IX86)
#define SHA1_ASM
#define SHA256_ASM
#define SHA512_ASM
#define KECCAK1600_ASM
#define SELFTEST
#define MD5_ASM
#endif

#endif

extern void OPENSSL_cleanse(void *ptr, size_t len);
