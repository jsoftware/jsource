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
extern int numberOfCores;

#if defined(__aarch32__)||defined(__arm__)||defined(_M_ARM)
uint32_t OPENSSL_armcap_P;

void cpuInit(void)
{
  g_cpuFeatures = 0;
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
  numberOfCores=getNumberOfCores();

#if defined(__linux__)
  unsigned long hwcaps= getauxval(AT_HWCAP);

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

// mask off avx if os does not support
static int AVX=0;
// OSXSAVE
static int OSXSAVE = 0;
static int vendorIsIntel = 0, vendorIsAMD = 0;

uint32_t OPENSSL_ia32cap_P[4];

#ifdef MMSC_VER
#include <windows.h>
extern void __cpuid(int CPUInfo[4], int InfoType);
#define x86_cpuid(x,y) __cpuid(y,x)
#else
#ifndef _WIN32
#include <string.h>
#include <sys/utsname.h>
#if defined(__x86_64__)||defined(__i386__)
#ifndef ANDROID
#include <cpuid.h>
#endif
#endif
#endif
#endif

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

#ifndef MMSC_VER
#if !defined(ANDROID) && !defined(_WIN32)
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
#else
#ifdef __i386__
static __inline__ void x86_cpuid(unsigned int func, unsigned int values[4])
{
  int a, b, c, d;
  /* We need to preserve ebx since we're compiling PIC code */
  /* this means we can't use "=b" for the second output register */
  __asm__ __volatile__ ( \
                         "push %%ebx\n"
                         "xor %%ecx,%%ecx\n"
                         "cpuid\n" \
                         "mov %%ebx, %1\n"
                         "pop %%ebx\n"
                         : "=a" (a), "=r" (b), "=c" (c), "=d" (d) \
                         : "a" (func) \
                       );
  values[0] = a;
  values[1] = b;
  values[2] = c;
  values[3] = d;
}
#elif defined(__x86_64__)
static __inline__ void x86_cpuid(unsigned int func, unsigned int values[4])
{
  int64_t a, b, c, d;
  /* We need to preserve ebx since we're compiling PIC code */
  /* this means we can't use "=b" for the second output register */
  __asm__ __volatile__ ( \
                         "push %%rbx\n"
                         "xor %%ecx,%%ecx\n"
                         "cpuid\n" \
                         "mov %%rbx, %1\n"
                         "pop %%rbx\n"
                         : "=a" (a), "=r" (b), "=c" (c), "=d" (d) \
                         : "a" (func) \
                       );
  values[0] = a;
  values[1] = b;
  values[2] = c;
  values[3] = d;
}
#endif
#endif
#endif

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
#elif defined(__MACH__)
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
  }
#endif

  OPENSSL_setcap();
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
#ifdef _WIN32
 SYSTEM_INFO sysinfo;
 GetSystemInfo(&sysinfo);
 return sysinfo.dwNumberOfProcessors;
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
#else
 return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

