/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* CPU information                                                         */

#include "cpuinfo.h"

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <sys/param.h>
#include <sys/sysctl.h>
#else
#include <unistd.h>
#endif

extern uint64_t g_cpuFeatures;
extern uint64_t g_cpuFeatures2;
extern int numberOfCores;

#if defined(__aarch32__)||defined(__arm__)||defined(_M_ARM)
uint32_t OPENSSL_armcap_P;

void cpuInit(void)
{
  g_cpuFeatures = 0;
  g_cpuFeatures2 = 0;
  OPENSSL_armcap_P = 0;
  numberOfCores=getNumberOfCores();
}

#elif defined(__aarch64__)||defined(_M_ARM64)

#if defined(__linux__)
#include <sys/auxv.h>
#include <asm/hwcap.h>
#include <arm_neon.h>
#endif

uint32_t OPENSSL_armcap_P;

void cpuInit(void)
{
  g_cpuFeatures = 0;
  g_cpuFeatures2 = 0;
  numberOfCores=getNumberOfCores();

#if defined(__linux__)
  unsigned long hwcaps= getauxval(AT_HWCAP);
  unsigned long hwcaps2= getauxval(AT_HWCAP2);

#if defined(ANDROID)
  if(hwcaps & HWCAP_FP) g_cpuFeatures |= ARM_HWCAP_FP;
  if(hwcaps & HWCAP_ASIMD) g_cpuFeatures |= ARM_HWCAP_ASIMD;
  if(hwcaps & HWCAP_EVTSTRM) g_cpuFeatures |= ARM_HWCAP_EVTSTRM;
  if(hwcaps & HWCAP_AES) g_cpuFeatures |= ARM_HWCAP_AES;
  if(hwcaps & HWCAP_PMULL) g_cpuFeatures |= ARM_HWCAP_PMULL;
  if(hwcaps & HWCAP_SHA1) g_cpuFeatures |= ARM_HWCAP_SHA1;
  if(hwcaps & HWCAP_SHA2) g_cpuFeatures |= ARM_HWCAP_SHA2;
  if(hwcaps & HWCAP_CRC32) g_cpuFeatures |= ARM_HWCAP_CRC32;
  if(hwcaps & HWCAP_ATOMICS) g_cpuFeatures |= ARM_HWCAP_ATOMICS;
  if(hwcaps & HWCAP_FPHP) g_cpuFeatures |= ARM_HWCAP_FPHP;
  if(hwcaps & HWCAP_ASIMDHP) g_cpuFeatures |= ARM_HWCAP_ASIMDHP;
  if(hwcaps & HWCAP_CPUID) g_cpuFeatures |= ARM_HWCAP_CPUID;
  if(hwcaps & HWCAP_ASIMDRDM) g_cpuFeatures |= ARM_HWCAP_ASIMDRDM;
  if(hwcaps & HWCAP_JSCVT) g_cpuFeatures |= ARM_HWCAP_JSCVT;
  if(hwcaps & HWCAP_FCMA) g_cpuFeatures |= ARM_HWCAP_FCMA;
  if(hwcaps & HWCAP_LRCPC) g_cpuFeatures |= ARM_HWCAP_LRCPC;
  if(hwcaps & HWCAP_DCPOP) g_cpuFeatures |= ARM_HWCAP_DCPOP;
  if(hwcaps & HWCAP_SHA3) g_cpuFeatures |= ARM_HWCAP_SHA3;
  if(hwcaps & HWCAP_SM3) g_cpuFeatures |= ARM_HWCAP_SM3;
  if(hwcaps & HWCAP_SM4) g_cpuFeatures |= ARM_HWCAP_SM4;
  if(hwcaps & HWCAP_ASIMDDP) g_cpuFeatures |= ARM_HWCAP_ASIMDDP;
  if(hwcaps & HWCAP_SHA512) g_cpuFeatures |= ARM_HWCAP_SHA512;
  if(hwcaps & HWCAP_SVE) g_cpuFeatures |= ARM_HWCAP_SVE;
  if(hwcaps & HWCAP_ASIMDFHM) g_cpuFeatures |= ARM_HWCAP_ASIMDFHM;
  if(hwcaps & HWCAP_DIT) g_cpuFeatures |= ARM_HWCAP_DIT;
  if(hwcaps & HWCAP_USCAT) g_cpuFeatures |= ARM_HWCAP_USCAT;
  if(hwcaps & HWCAP_ILRCPC) g_cpuFeatures |= ARM_HWCAP_ILRCPC;
  if(hwcaps & HWCAP_FLAGM) g_cpuFeatures |= ARM_HWCAP_FLAGM;
  if(hwcaps & HWCAP_SSBS) g_cpuFeatures |= ARM_HWCAP_SSBS;
  if(hwcaps & HWCAP_SB) g_cpuFeatures |= ARM_HWCAP_SB;
  if(hwcaps & HWCAP_PACA) g_cpuFeatures |= ARM_HWCAP_PACA;
  if(hwcaps & HWCAP_PACG) g_cpuFeatures |= ARM_HWCAP_PACG;

  if(hwcaps2 & HWCAP2_DCPODP) g_cpuFeatures2 |= ARM_HWCAP2_DCPODP;
  if(hwcaps2 & HWCAP2_SVE2) g_cpuFeatures2 |= ARM_HWCAP2_SVE2;
  if(hwcaps2 & HWCAP2_SVEAES) g_cpuFeatures2 |= ARM_HWCAP2_SVEAES;
  if(hwcaps2 & HWCAP2_SVEPMULL) g_cpuFeatures2 |= ARM_HWCAP2_SVEPMULL;
  if(hwcaps2 & HWCAP2_SVEBITPERM) g_cpuFeatures2 |= ARM_HWCAP2_SVEBITPERM;
  if(hwcaps2 & HWCAP2_SVESHA3) g_cpuFeatures2 |= ARM_HWCAP2_SVESHA3;
  if(hwcaps2 & HWCAP2_SVESM4) g_cpuFeatures2 |= ARM_HWCAP2_SVESM4;
  if(hwcaps2 & HWCAP2_FLAGM2) g_cpuFeatures2 |= ARM_HWCAP2_FLAGM2;
  if(hwcaps2 & HWCAP2_FRINT) g_cpuFeatures2 |= ARM_HWCAP2_FRINT;
  if(hwcaps2 & HWCAP2_SVEI8MM) g_cpuFeatures2 |= ARM_HWCAP2_SVEI8MM;
  if(hwcaps2 & HWCAP2_SVEF32MM) g_cpuFeatures2 |= ARM_HWCAP2_SVEF32MM;
  if(hwcaps2 & HWCAP2_SVEF64MM) g_cpuFeatures2 |= ARM_HWCAP2_SVEF64MM;
  if(hwcaps2 & HWCAP2_SVEBF16) g_cpuFeatures2 |= ARM_HWCAP2_SVEBF16;
  if(hwcaps2 & HWCAP2_I8MM) g_cpuFeatures2 |= ARM_HWCAP2_I8MM;
  if(hwcaps2 & HWCAP2_BF16) g_cpuFeatures2 |= ARM_HWCAP2_BF16;
  if(hwcaps2 & HWCAP2_DGH) g_cpuFeatures2 |= ARM_HWCAP2_DGH;
  if(hwcaps2 & HWCAP2_RNG) g_cpuFeatures2 |= ARM_HWCAP2_RNG;
  if(hwcaps2 & HWCAP2_BTI) g_cpuFeatures2 |= ARM_HWCAP2_BTI;
  if(hwcaps2 & HWCAP2_MTE) g_cpuFeatures2 |= ARM_HWCAP2_MTE;
#if 0
  if(hwcaps2 & HWCAP2_ECV) g_cpuFeatures2 |= ARM_HWCAP2_ECV;
  if(hwcaps2 & HWCAP2_AFP) g_cpuFeatures2 |= ARM_HWCAP2_AFP;
  if(hwcaps2 & HWCAP2_RPRES) g_cpuFeatures2 |= ARM_HWCAP2_RPRES;
  if(hwcaps2 & HWCAP2_MTE3) g_cpuFeatures2 |= ARM_HWCAP2_MTE3;
  if(hwcaps2 & HWCAP2_SME) g_cpuFeatures2 |= ARM_HWCAP2_SME;
  if(hwcaps2 & HWCAP2_SME_I16I64) g_cpuFeatures2 |= ARM_HWCAP2_SME_I16I64;
  if(hwcaps2 & HWCAP2_SME_F64F64) g_cpuFeatures2 |= ARM_HWCAP2_SME_F64F64;
  if(hwcaps2 & HWCAP2_SME_I8I32) g_cpuFeatures2 |= ARM_HWCAP2_SME_I8I32;
  if(hwcaps2 & HWCAP2_SME_F16F32) g_cpuFeatures2 |= ARM_HWCAP2_SME_F16F32;
  if(hwcaps2 & HWCAP2_SME_B16F32) g_cpuFeatures2 |= ARM_HWCAP2_SME_B16F32;
  if(hwcaps2 & HWCAP2_SME_F32F32) g_cpuFeatures2 |= ARM_HWCAP2_SME_F32F32;
  if(hwcaps2 & HWCAP2_SME_FA64) g_cpuFeatures2 |= ARM_HWCAP2_SME_FA64;
  if(hwcaps2 & HWCAP2_WFXT) g_cpuFeatures2 |= ARM_HWCAP2_WFXT;
  if(hwcaps2 & HWCAP2_EBF16) g_cpuFeatures2 |= ARM_HWCAP2_EBF16;
#endif
#else
// see <uapi/asm/hwcap.h> kernel header
  if(hwcaps & ARM_HWCAP_FP) g_cpuFeatures |= ARM_HWCAP_FP;
  if(hwcaps & ARM_HWCAP_ASIMD) g_cpuFeatures |= ARM_HWCAP_ASIMD;
  if(hwcaps & ARM_HWCAP_EVTSTRM) g_cpuFeatures |= ARM_HWCAP_EVTSTRM;
  if(hwcaps & ARM_HWCAP_AES) g_cpuFeatures |= ARM_HWCAP_AES;
  if(hwcaps & ARM_HWCAP_PMULL) g_cpuFeatures |= ARM_HWCAP_PMULL;
  if(hwcaps & ARM_HWCAP_SHA1) g_cpuFeatures |= ARM_HWCAP_SHA1;
  if(hwcaps & ARM_HWCAP_SHA2) g_cpuFeatures |= ARM_HWCAP_SHA2;
  if(hwcaps & ARM_HWCAP_CRC32) g_cpuFeatures |= ARM_HWCAP_CRC32;
  if(hwcaps & ARM_HWCAP_ATOMICS) g_cpuFeatures |= ARM_HWCAP_ATOMICS;
  if(hwcaps & ARM_HWCAP_FPHP) g_cpuFeatures |= ARM_HWCAP_FPHP;
  if(hwcaps & ARM_HWCAP_ASIMDHP) g_cpuFeatures |= ARM_HWCAP_ASIMDHP;
  if(hwcaps & ARM_HWCAP_CPUID) g_cpuFeatures |= ARM_HWCAP_CPUID;
  if(hwcaps & ARM_HWCAP_ASIMDRDM) g_cpuFeatures |= ARM_HWCAP_ASIMDRDM;
  if(hwcaps & ARM_HWCAP_JSCVT) g_cpuFeatures |= ARM_HWCAP_JSCVT;
  if(hwcaps & ARM_HWCAP_FCMA) g_cpuFeatures |= ARM_HWCAP_FCMA;
  if(hwcaps & ARM_HWCAP_LRCPC) g_cpuFeatures |= ARM_HWCAP_LRCPC;
  if(hwcaps & ARM_HWCAP_DCPOP) g_cpuFeatures |= ARM_HWCAP_DCPOP;
  if(hwcaps & ARM_HWCAP_SHA3) g_cpuFeatures |= ARM_HWCAP_SHA3;
  if(hwcaps & ARM_HWCAP_SM3) g_cpuFeatures |= ARM_HWCAP_SM3;
  if(hwcaps & ARM_HWCAP_SM4) g_cpuFeatures |= ARM_HWCAP_SM4;
  if(hwcaps & ARM_HWCAP_ASIMDDP) g_cpuFeatures |= ARM_HWCAP_ASIMDDP;
  if(hwcaps & ARM_HWCAP_SHA512) g_cpuFeatures |= ARM_HWCAP_SHA512;
  if(hwcaps & ARM_HWCAP_SVE) g_cpuFeatures |= ARM_HWCAP_SVE;
  if(hwcaps & ARM_HWCAP_ASIMDFHM) g_cpuFeatures |= ARM_HWCAP_ASIMDFHM;
  if(hwcaps & ARM_HWCAP_DIT) g_cpuFeatures |= ARM_HWCAP_DIT;
  if(hwcaps & ARM_HWCAP_USCAT) g_cpuFeatures |= ARM_HWCAP_USCAT;
  if(hwcaps & ARM_HWCAP_ILRCPC) g_cpuFeatures |= ARM_HWCAP_ILRCPC;
  if(hwcaps & ARM_HWCAP_FLAGM) g_cpuFeatures |= ARM_HWCAP_FLAGM;
  if(hwcaps & ARM_HWCAP_SSBS) g_cpuFeatures |= ARM_HWCAP_SSBS;
  if(hwcaps & ARM_HWCAP_SB) g_cpuFeatures |= ARM_HWCAP_SB;
  if(hwcaps & ARM_HWCAP_PACA) g_cpuFeatures |= ARM_HWCAP_PACA;

  if(hwcaps2 & ARM_HWCAP2_DCPODP) g_cpuFeatures2 |= ARM_HWCAP2_DCPODP;
  if(hwcaps2 & ARM_HWCAP2_SVE2) g_cpuFeatures2 |= ARM_HWCAP2_SVE2;
  if(hwcaps2 & ARM_HWCAP2_SVEAES) g_cpuFeatures2 |= ARM_HWCAP2_SVEAES;
  if(hwcaps2 & ARM_HWCAP2_SVEPMULL) g_cpuFeatures2 |= ARM_HWCAP2_SVEPMULL;
  if(hwcaps2 & ARM_HWCAP2_SVEBITPERM) g_cpuFeatures2 |= ARM_HWCAP2_SVEBITPERM;
  if(hwcaps2 & ARM_HWCAP2_SVESHA3) g_cpuFeatures2 |= ARM_HWCAP2_SVESHA3;
  if(hwcaps2 & ARM_HWCAP2_SVESM4) g_cpuFeatures2 |= ARM_HWCAP2_SVESM4;
  if(hwcaps2 & ARM_HWCAP2_FLAGM2) g_cpuFeatures2 |= ARM_HWCAP2_FLAGM2;
  if(hwcaps2 & ARM_HWCAP2_FRINT) g_cpuFeatures2 |= ARM_HWCAP2_FRINT;
  if(hwcaps2 & ARM_HWCAP2_SVEI8MM) g_cpuFeatures2 |= ARM_HWCAP2_SVEI8MM;
  if(hwcaps2 & ARM_HWCAP2_SVEF32MM) g_cpuFeatures2 |= ARM_HWCAP2_SVEF32MM;
  if(hwcaps2 & ARM_HWCAP2_SVEF64MM) g_cpuFeatures2 |= ARM_HWCAP2_SVEF64MM;
  if(hwcaps2 & ARM_HWCAP2_SVEBF16) g_cpuFeatures2 |= ARM_HWCAP2_SVEBF16;
  if(hwcaps2 & ARM_HWCAP2_I8MM) g_cpuFeatures2 |= ARM_HWCAP2_I8MM;
  if(hwcaps2 & ARM_HWCAP2_BF16) g_cpuFeatures2 |= ARM_HWCAP2_BF16;
  if(hwcaps2 & ARM_HWCAP2_DGH) g_cpuFeatures2 |= ARM_HWCAP2_DGH;
  if(hwcaps2 & ARM_HWCAP2_RNG) g_cpuFeatures2 |= ARM_HWCAP2_RNG;
  if(hwcaps2 & ARM_HWCAP2_BTI) g_cpuFeatures2 |= ARM_HWCAP2_BTI;
  if(hwcaps2 & ARM_HWCAP2_MTE) g_cpuFeatures2 |= ARM_HWCAP2_MTE;
  if(hwcaps2 & ARM_HWCAP2_ECV) g_cpuFeatures2 |= ARM_HWCAP2_ECV;
  if(hwcaps2 & ARM_HWCAP2_AFP) g_cpuFeatures2 |= ARM_HWCAP2_AFP;
  if(hwcaps2 & ARM_HWCAP2_RPRES) g_cpuFeatures2 |= ARM_HWCAP2_RPRES;
  if(hwcaps2 & ARM_HWCAP2_MTE3) g_cpuFeatures2 |= ARM_HWCAP2_MTE3;
  if(hwcaps2 & ARM_HWCAP2_SME) g_cpuFeatures2 |= ARM_HWCAP2_SME;
  if(hwcaps2 & ARM_HWCAP2_SME_I16I64) g_cpuFeatures2 |= ARM_HWCAP2_SME_I16I64;
  if(hwcaps2 & ARM_HWCAP2_SME_F64F64) g_cpuFeatures2 |= ARM_HWCAP2_SME_F64F64;
  if(hwcaps2 & ARM_HWCAP2_SME_I8I32) g_cpuFeatures2 |= ARM_HWCAP2_SME_I8I32;
  if(hwcaps2 & ARM_HWCAP2_SME_F16F32) g_cpuFeatures2 |= ARM_HWCAP2_SME_F16F32;
  if(hwcaps2 & ARM_HWCAP2_SME_B16F32) g_cpuFeatures2 |= ARM_HWCAP2_SME_B16F32;
  if(hwcaps2 & ARM_HWCAP2_SME_F32F32) g_cpuFeatures2 |= ARM_HWCAP2_SME_F32F32;
  if(hwcaps2 & ARM_HWCAP2_SME_FA64) g_cpuFeatures2 |= ARM_HWCAP2_SME_FA64;
  if(hwcaps2 & ARM_HWCAP2_WFXT) g_cpuFeatures2 |= ARM_HWCAP2_WFXT;
  if(hwcaps2 & ARM_HWCAP2_EBF16) g_cpuFeatures2 |= ARM_HWCAP2_EBF16;
#endif

#else
// apple m1/ios cannot detect, just hard wired some common arm64 features
  g_cpuFeatures |= ARM_HWCAP_FP;
  g_cpuFeatures |= ARM_HWCAP_ASIMD;
  g_cpuFeatures |= ARM_HWCAP_EVTSTRM;
  g_cpuFeatures |= ARM_HWCAP_AES;
  g_cpuFeatures |= ARM_HWCAP_PMULL;
  g_cpuFeatures |= ARM_HWCAP_SHA1;
  g_cpuFeatures |= ARM_HWCAP_SHA2;
  g_cpuFeatures |= ARM_HWCAP_CRC32;
#if defined(TARGET_OS_OSX)
  g_cpuFeatures |= ARM_HWCAP_ATOMICS;
  g_cpuFeatures |= ARM_HWCAP_FPHP;
#endif
//  g_cpuFeatures |= ARM_HWCAP_ASIMDHP;
  g_cpuFeatures |= ARM_HWCAP_CPUID;
//  g_cpuFeatures |= ARM_HWCAP_ASIMDRDM;
//  g_cpuFeatures |= ARM_HWCAP_JSCVT;
//  g_cpuFeatures |= ARM_HWCAP_FCMA;
//  g_cpuFeatures |= ARM_HWCAP_LRCPC;
//  g_cpuFeatures |= ARM_HWCAP_DCPOP;
#if defined(TARGET_OS_OSX)
  g_cpuFeatures |= ARM_HWCAP_SHA3;
#endif
//  g_cpuFeatures |= ARM_HWCAP_SM3;
//  g_cpuFeatures |= ARM_HWCAP_SM4;
//  g_cpuFeatures |= ARM_HWCAP_ASIMDDP;
#if defined(TARGET_OS_OSX)
  g_cpuFeatures |= ARM_HWCAP_SHA512;
#endif
//  g_cpuFeatures |= ARM_HWCAP_SVE;
//  g_cpuFeatures |= ARM_HWCAP_ASIMDFHM;
//  g_cpuFeatures |= ARM_HWCAP_DIT;
//  g_cpuFeatures |= ARM_HWCAP_USCAT;
//  g_cpuFeatures |= ARM_HWCAP_ILRCPC;
//  g_cpuFeatures |= ARM_HWCAP_FLAGM;

#endif

  OPENSSL_setcap();
}

#elif defined(__x86_64__)||defined(__i386__)||defined(_M_X64)||defined(_M_IX86)

#if defined(__linux__)
#include <sys/auxv.h>
#endif

// mask off avx if os does not support
static int AVX=0;
// OSXSAVE
static int OSXSAVE = 0;
static int vendorIsIntel = 0, vendorIsAMD = 0;

uint32_t OPENSSL_ia32cap_P[4];

#ifndef _WIN32
#include <string.h>
#include <sys/utsname.h>
#endif
#include <cpuid.h>

#if defined(__x86_64__)||defined(__i386__)||defined(_M_X64)||defined(_M_IX86)
static int check_xcr0_ymm()
{
  uint32_t xcr0;
#if defined(MMSC_VER)
  xcr0 = (uint32_t)_xgetbv(0);  /* min VS2010 SP1 compiler is required */
#else
  __asm__ ("xgetbv" : "=a" (xcr0) : "c" (0) : "%edx" );
#endif
  return ((xcr0 & 6) == 6); /* checking if xmm and ymm state are enabled in XCR0 */
}
#endif

static __inline int
get_cpuid_count (unsigned int __level, unsigned int __count,
                 unsigned int *__eax, unsigned int *__ebx,
                 unsigned int *__ecx, unsigned int *__edx)
{
  unsigned int __ext = __level & 0x80000000;
  if ((int)__get_cpuid_max (__ext, 0) < (int)__level)
    return 0;

  __cpuid_count (__level, __count, *__eax, *__ebx, *__ecx, *__edx);
  return 1;
}
#define x86_cpuid(func, values) get_cpuid_count(func, 0, values, values+1, values+2, values+3)
#define x86_cpuid_ex(func, subfunc, values) get_cpuid_count(func, subfunc, values, values+1, values+2, values+3)

void cpuInit(void)
{
  g_cpuFeatures = 0;
  numberOfCores=getNumberOfCores();

#if defined(__i386__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86)
  unsigned int regs[4];

  /* According to http://en.wikipedia.org/wiki/CPUID */

// $ echo -n GenuineIntel | hexdump -C
// 00000000  47 65 6e 75 69 6e 65 49  6e 74 65 6c              |GenuineIntel|
#define VENDOR_INTEL_b  0x756e6547
#define VENDOR_INTEL_c  0x6c65746e
#define VENDOR_INTEL_d  0x49656e69

// $ echo -n AuthenticAMD | hexdump -C
// 00000000  41 75 74 68 65 6e 74 69  63 41 4d 44              |AuthenticAMD|
#define VENDOR_AMD_b  0x68747541
#define VENDOR_AMD_c  0x444d4163
#define VENDOR_AMD_d  0x69746e65

  x86_cpuid(0, regs);
  int maxid = regs[0];
  vendorIsIntel = (regs[1] == VENDOR_INTEL_b &&
                   regs[2] == VENDOR_INTEL_c &&
                   regs[3] == VENDOR_INTEL_d);
  vendorIsAMD   = (regs[1] == VENDOR_AMD_b &&
                   regs[2] == VENDOR_AMD_c &&
                   regs[3] == VENDOR_AMD_d);

  x86_cpuid(1, regs);
  OPENSSL_ia32cap_P[0] = regs[3];
  OPENSSL_ia32cap_P[1] = regs[2];
  /*
  Index 0:
    EDX for CPUID where EAX = 1
    Bit 20 is always zero
    Bit 28 is adjusted to reflect whether the data cache is shared between
      multiple logical cores
    Bit 30 is used to indicate an Intel CPU
  Index 1:
    ECX for CPUID where EAX = 1
    Bit 11 is used to indicate AMD XOP support, not SDBG
  Index 2:
    EBX for CPUID where EAX = 7
  Index 3:
    ECX for CPUID where EAX = 7
  */
  OPENSSL_ia32cap_P[0] &= ~(1 << 20);
  OPENSSL_ia32cap_P[0] = (vendorIsIntel << 30) | ( OPENSSL_ia32cap_P[0] & ~(1 << 30));
  if(!vendorIsAMD) OPENSSL_ia32cap_P[1] &= ~(1 << 11);

  if ((regs[2] & (1 << 9)) != 0) {
    g_cpuFeatures |= CPU_X86_FEATURE_SSSE3;
  }
  if ((regs[2] & (1 << 23)) != 0) {
    g_cpuFeatures |= CPU_X86_FEATURE_POPCNT;
  }
  if ((regs[2] & (1 << 19)) != 0) {
    g_cpuFeatures |= CPU_X86_FEATURE_SSE4_1;
  }
  if ((regs[2] & (1 << 20)) != 0) {
    g_cpuFeatures |= CPU_X86_FEATURE_SSE4_2;
  }
  if (vendorIsIntel && (regs[2] & (1 << 22)) != 0) {
    g_cpuFeatures |= CPU_X86_FEATURE_MOVBE;
  }
  if ((regs[2] & (1 << 25)) != 0) {
    g_cpuFeatures |= CPU_X86_FEATURE_AES_NI;
  }
  if ((regs[2] & (1 << 28)) != 0) {
    g_cpuFeatures |= CPU_X86_FEATURE_AVX;
  }
  if ((regs[2] & (1 << 30)) != 0) {
    g_cpuFeatures |= CPU_X86_FEATURE_RDRAND;
  }
  if ((regs[2] & (1 << 12)) != 0) {
    g_cpuFeatures |= CPU_X86_FEATURE_FMA;
  }
  if ((regs[2] & (1 << 27)) != 0) {
    OSXSAVE = 1;
  }

  if (maxid>=7) {
    x86_cpuid(7, regs);
    OPENSSL_ia32cap_P[2] = regs[1];
    OPENSSL_ia32cap_P[3] = regs[2];
    if ((regs[1] & (1 << 5)) != 0) {
      g_cpuFeatures |= CPU_X86_FEATURE_AVX2;
    }
/*
EAX=07H, ECX=0
EBX[bit 16]  AVX512F
EBX[bit 17]  AVX512DQ
EBX[bit 21]  AVX512_IFMA
EBX[bit 26]  AVX512PF
EBX[bit 27]  AVX512ER
EBX[bit 28]  AVX512CD
EBX[bit 30]  AVX512BW
EBX[bit 31]  AVX512VL
ECX[bit 01]  AVX512_VBMI
ECX[bit 06]  AVX512_VBMI2
ECX[bit 08]  GFNI
ECX[bit 09]  VAES
ECX[bit 10]  VPCLMULQDQ
ECX[bit 11]  AVX512_VNNI
ECX[bit 12]  AVX512_BITALG
ECX[bit 14]  AVX512_VPOPCNTDQ
EDX[bit 02]  AVX512_4VNNIW
EDX[bit 03]  AVX512_4FMAPS
EDX[bit 08]  AVX512_VP2INTERSECT
EDX[bit 23]  AVX512_FP16
*/
    if ((regs[1] & (1 << 16)) != 0) {
      g_cpuFeatures |= CPU_X86_FEATURE_AVX512F;
    }
    if ((regs[1] & (1 << 17)) != 0) {
      g_cpuFeatures |= CPU_X86_FEATURE_AVX512DQ;
    }
    if ((regs[1] & (1 << 21)) != 0) {
      g_cpuFeatures |= CPU_X86_FEATURE_AVX512IFMA;
    }
    if ((regs[1] & (1 << 30)) != 0) {
      g_cpuFeatures |= CPU_X86_FEATURE_AVX512BW;
    }
    if ((regs[1] & (1 << 31)) != 0) {
      g_cpuFeatures |= CPU_X86_FEATURE_AVX512VL;
    }
    if ((regs[2] & (1 << 1)) != 0) {
      g_cpuFeatures |= CPU_X86_FEATURE_AVX512VBMI;
    }
    if ((regs[2] & (1 << 6)) != 0) {
      g_cpuFeatures |= CPU_X86_FEATURE_AVX512VBMI2;
    }
    if ((regs[1] & (1 << 29)) != 0) {
      g_cpuFeatures |= CPU_X86_FEATURE_SHA_NI;
    }
    if ((regs[1] & (1 << 18)) != 0) {
      g_cpuFeatures |= CPU_X86_FEATURE_RDSEED;
    }
    if ((regs[1] & (1 << 3)) != 0) {
      g_cpuFeatures |= CPU_X86_FEATURE_BMI1;
    }
    if ((regs[1] & (1 << 8)) != 0) {
      g_cpuFeatures |= CPU_X86_FEATURE_BMI2;
    }
  } else {
    OPENSSL_ia32cap_P[2] = 0;
    OPENSSL_ia32cap_P[3] = 0;
  }
#ifdef _WIN32
#if 0
  /* not very reliable check */
#define XSTATE_MASK_AVX   (XSTATE_MASK_GSSE)
  typedef DWORD64 (WINAPI *GETENABLEDXSTATEFEATURES)();
  GETENABLEDXSTATEFEATURES pfnGetEnabledXStateFeatures = NULL;
// Get the addresses of the AVX XState functions.
  HMODULE hm = GetModuleHandleA("kernel32.dll");
  if ((pfnGetEnabledXStateFeatures = (GETENABLEDXSTATEFEATURES)GetProcAddress(hm, "GetEnabledXStateFeatures")) &&
      ((pfnGetEnabledXStateFeatures() & XSTATE_MASK_AVX) != 0))
    AVX=1;
  FreeLibrary(hm);
#else
  AVX=1;
#endif
  if (!(g_cpuFeatures & CPU_X86_FEATURE_AVX) || !check_xcr0_ymm()) AVX=0;
#elif defined(__APPLE__)
// TODO
  AVX=1;
#else
  struct utsname unm;
  if (!uname(&unm) &&
      ((unm.release[0]>'2'&&unm.release[0]<='9')||  // avoid sign/unsigned char difference
       (strlen(unm.release)>5&&unm.release[0]=='2'&&unm.release[2]=='6'&&unm.release[4]=='3'&&
        (unm.release[5]>='0'&&unm.release[5]<='9'))))
    AVX=1;
  if (!(g_cpuFeatures & CPU_X86_FEATURE_AVX) || !check_xcr0_ymm()) AVX=0;
#endif
  if (!(AVX&&OSXSAVE)) {
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX;
    g_cpuFeatures &= ~CPU_X86_FEATURE_FMA;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX2;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512F;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512VL;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512DQ;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512BW;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512IFMA;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512VBMI;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512VBMI2;
  }
#endif

  OPENSSL_setcap();

#if defined(__linux__) && !defined(ANDROID)
#ifndef HWCAP2_RING3MWAIT
#define HWCAP2_RING3MWAIT      (1 << 0)
#endif
#ifndef HWCAP2_FSGSBASE
#define HWCAP2_FSGSBASE        (1 << 1)
#endif
  unsigned long hwcaps2= getauxval(AT_HWCAP2);
  if (hwcaps2 & HWCAP2_FSGSBASE) g_cpuFeatures2 |= CPU_X86_FEATURE2_RING3MWAIT;
  if (hwcaps2 & HWCAP2_FSGSBASE) g_cpuFeatures2 |= CPU_X86_FEATURE2_FSGSBASE;
#endif
}

#else

void cpuInit(void)
{
  g_cpuFeatures = 0;
  numberOfCores=getNumberOfCores();
}

#endif

uint64_t getCpuFeatures(void)
{
  return g_cpuFeatures;
}

uint64_t getCpuFeatures2(void)
{
  return g_cpuFeatures2;
}

intptr_t getCpuFamily(void)
{
#if defined(__aarch64__)||defined(_M_ARM64)
  return CPU_FAMILY_ARM64;
#elif defined(__arm__)||defined(_M_ARM)
  return CPU_FAMILY_ARM;
#elif defined(__x86_64__)||defined(_M_X64)
  return CPU_FAMILY_X86_64;
#elif defined(__i386__)||defined(_M_IX86)
  return CPU_FAMILY_X86;
#else
  return CPU_FAMILY_UNKNOWN ;
#endif
}

void OPENSSL_setcap(void)
{
#if defined(__aarch64__)||defined(_M_ARM64)
  OPENSSL_armcap_P = ARMV7_NEON;
  OPENSSL_armcap_P |= (g_cpuFeatures & ARM_HWCAP_AES) ? ARMV8_AES : 0;
  OPENSSL_armcap_P |= (g_cpuFeatures & ARM_HWCAP_SHA1) ? ARMV8_SHA1 : 0;
  OPENSSL_armcap_P |= (g_cpuFeatures & ARM_HWCAP_SHA2) ? ARMV8_SHA256 : 0;
  OPENSSL_armcap_P |= (g_cpuFeatures & ARM_HWCAP_PMULL) ? ARMV8_PMULL : 0;
  OPENSSL_armcap_P |= (g_cpuFeatures & ARM_HWCAP_SHA512) ? ARMV8_SHA512 : 0;
#elif defined(__x86_64__)||defined(__i386__)||defined(_M_X64)||defined(_M_IX86)
  if (!(AVX&&OSXSAVE)) {
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX;
    g_cpuFeatures &= ~CPU_X86_FEATURE_FMA;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX2;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512F;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512VL;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512DQ;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512BW;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512IFMA;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512VBMI;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512VBMI2;
  }
  OPENSSL_ia32cap_P[1] &= ~(1 << 9);
  OPENSSL_ia32cap_P[1] |= (g_cpuFeatures & CPU_X86_FEATURE_SSSE3) ? (1 << 9) : 0;
  OPENSSL_ia32cap_P[1] &= ~(1 << 23);
  OPENSSL_ia32cap_P[1] |= (g_cpuFeatures & CPU_X86_FEATURE_POPCNT) ? (1 << 23) : 0;
  OPENSSL_ia32cap_P[1] &= ~(1 << 19);
  OPENSSL_ia32cap_P[1] |= (g_cpuFeatures & CPU_X86_FEATURE_SSE4_1) ? (1 << 19) : 0;
  OPENSSL_ia32cap_P[1] &= ~(1 << 20);
  OPENSSL_ia32cap_P[1] |= (g_cpuFeatures & CPU_X86_FEATURE_SSE4_2) ? (1 << 20) : 0;
  OPENSSL_ia32cap_P[1] &= ~(1 << 22);
  OPENSSL_ia32cap_P[1] |= (g_cpuFeatures & CPU_X86_FEATURE_MOVBE) ? (1 << 22) : 0;
  OPENSSL_ia32cap_P[1] &= ~(1 << 25);
  OPENSSL_ia32cap_P[1] |= (g_cpuFeatures & CPU_X86_FEATURE_AES_NI) ? (1 << 25) : 0;
  OPENSSL_ia32cap_P[1] &= ~(1 << 28);
  OPENSSL_ia32cap_P[1] |= (g_cpuFeatures & CPU_X86_FEATURE_AVX) ? (1 << 28) : 0;
  OPENSSL_ia32cap_P[1] &= ~(1 << 30);
  OPENSSL_ia32cap_P[1] |= (g_cpuFeatures & CPU_X86_FEATURE_RDRAND) ? (1 << 30) : 0;
  OPENSSL_ia32cap_P[1] &= ~(1 << 12);
  OPENSSL_ia32cap_P[1] |= (g_cpuFeatures & CPU_X86_FEATURE_FMA) ? (1 << 12) : 0;
  OPENSSL_ia32cap_P[2] &= ~(1 << 5);
  OPENSSL_ia32cap_P[2] |= (g_cpuFeatures & CPU_X86_FEATURE_AVX2) ? (1 << 5) : 0;
  OPENSSL_ia32cap_P[2] &= ~(1 << 29);
  OPENSSL_ia32cap_P[2] |= (g_cpuFeatures & CPU_X86_FEATURE_SHA_NI) ? (1 << 29) : 0;
  OPENSSL_ia32cap_P[2] &= ~(1 << 18);
  OPENSSL_ia32cap_P[2] |= (g_cpuFeatures & CPU_X86_FEATURE_RDSEED) ? (1 << 18) : 0;

#endif
}

int getNumberOfCores(void) {
#if defined(__wasm__)
 return 1;
#elif defined(_WIN32)
 DWORD_PTR ProcessAffinityMask, SystemAffinityMask;
 if(GetProcessAffinityMask(GetCurrentProcess(), &ProcessAffinityMask, &SystemAffinityMask)){
#if defined(_WIN64)||defined(__LP64__)
  return __builtin_popcountll(ProcessAffinityMask);
#else
  return __builtin_popcount(ProcessAffinityMask);
#endif
 }else{
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
 }
#elif defined(__APPLE__)
 int nm[2];
 size_t len = 4;
 uint32_t count;

 nm[0] = CTL_HW; nm[1] = HW_AVAILCPU;
 sysctl(nm, 2, &count, &len, NULL, 0);

 if(count < 1) {
  nm[1] = HW_NCPU;
  sysctl(nm, 2, &count, &len, NULL, 0);
  if(count < 1) { count = 1; }
 }
 return count;
#elif defined(__linux__)
 cpu_set_t set;
 CPU_ZERO(&set);
 return (!sched_getaffinity(getpid(), sizeof(set), &set)) ? CPU_COUNT(&set) : sysconf(_SC_NPROCESSORS_ONLN);
#else
 return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

