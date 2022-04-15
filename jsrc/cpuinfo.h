#ifndef CPU_INFO_H
#define CPU_INFO_H

/* clang-cl */
#if defined(__clang__) && !defined(__GNUC__)
#define __GNUC__ 5
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#undef MMSC_VER
#define MMSC_VER
#else
#undef MMSC_VER
#endif

#include <stdint.h>

extern void cpuInit(void);
extern uint64_t getCpuFeatures(void);
extern intptr_t getCpuFamily(void);
extern void OPENSSL_setcap(void);
extern int getNumberOfCores(void);

#if defined(__aarch32__)||defined(__arm__)||defined(_M_ARM)||defined(__aarch64__)||defined(_M_ARM64)

extern uint32_t OPENSSL_armcap_P;

enum {
  ARMV7_NEON    = (1<<0),
  ARMV7_TICK    = (1<<1),
  ARMV8_AES     = (1<<2),
  ARMV8_SHA1    = (1<<3),
  ARMV8_SHA256  = (1<<4),
  ARMV8_PMULL   = (1<<5),
  ARMV8_SHA512  = (1<<6),
};

enum {
  ARM_HWCAP_FP = (1 << 0),
  ARM_HWCAP_ASIMD = (1 << 1),
  ARM_HWCAP_EVTSTRM = (1 << 2),
  ARM_HWCAP_AES = (1 << 3),
  ARM_HWCAP_PMULL = (1 << 4),
  ARM_HWCAP_SHA1 = (1 << 5),
  ARM_HWCAP_SHA2 = (1 << 6),
  ARM_HWCAP_CRC32 = (1 << 7),
  ARM_HWCAP_ATOMICS = (1 << 8),
  ARM_HWCAP_FPHP = (1 << 9),
  ARM_HWCAP_ASIMDHP = (1 << 10),
  ARM_HWCAP_CPUID = (1 << 11),
  ARM_HWCAP_ASIMDRDM = (1 << 12),
  ARM_HWCAP_JSCVT = (1 << 13),
  ARM_HWCAP_FCMA = (1 << 14),
  ARM_HWCAP_LRCPC = (1 << 15),
  ARM_HWCAP_DCPOP = (1 << 16),
  ARM_HWCAP_SHA3 = (1 << 17),
  ARM_HWCAP_SM3 = (1 << 18),
  ARM_HWCAP_SM4 = (1 << 19),
  ARM_HWCAP_ASIMDDP = (1 << 20),
  ARM_HWCAP_SHA512 = (1 << 21),
  ARM_HWCAP_SVE = (1 << 22),
  ARM_HWCAP_ASIMDFHM = (1 << 23),
  ARM_HWCAP_DIT = (1 << 24),
  ARM_HWCAP_USCAT = (1 << 25),
  ARM_HWCAP_ILRCPC = (1 << 26),
  ARM_HWCAP_FLAGM = (1 << 27),
};

#elif defined(__x86_64__)||defined(__i386__)||defined(_M_X64)||defined(_M_IX86)

extern uint32_t OPENSSL_ia32cap_P[4];

enum {
  CPU_X86_FEATURE_SSSE3  = (1 << 0),
  CPU_X86_FEATURE_POPCNT = (1 << 1),
  CPU_X86_FEATURE_MOVBE  = (1 << 2),
  CPU_X86_FEATURE_SSE4_1 = (1 << 3),
  CPU_X86_FEATURE_SSE4_2 = (1 << 4),
  CPU_X86_FEATURE_AES_NI = (1 << 5),
  CPU_X86_FEATURE_AVX =    (1 << 6),
  CPU_X86_FEATURE_RDRAND = (1 << 7),
  CPU_X86_FEATURE_AVX2 =   (1 << 8),
  CPU_X86_FEATURE_SHA_NI = (1 << 9),
  CPU_X86_FEATURE_FMA    = (1 << 10),
  CPU_X86_FEATURE_RDSEED = (1 << 11),
  CPU_X86_FEATURE_BMI1 = (1 << 12),
  CPU_X86_FEATURE_BMI2 = (1 << 13),
};

#endif

enum {
  CPU_FAMILY_UNKNOWN = 0,
  CPU_FAMILY_ARM,
  CPU_FAMILY_X86,
  CPU_FAMILY_ARM64,
  CPU_FAMILY_X86_64,
  CPU_FAMILY_MAX /* do not remove */
};

#endif /* CPU_INFO_H */
