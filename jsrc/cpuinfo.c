#include "cpuinfo.h"

#ifdef _WIN32
#include <windows.h>
extern void __cpuid(int CPUInfo[4], int InfoType);
#define x86_cpuid(x,y) __cpuid(y,x)
#else
#include <string.h>
#include <sys/utsname.h>
#if defined(__x86_64__)||defined(__i386__)
#ifndef ANDROID
#include <cpuid.h>
#endif
#endif
#endif

static  uint64_t           g_cpuFeatures;

#if defined(__x86_64__)||defined(__i386__)||defined(_MSC_VER)
static int check_xcr0_ymm()
{
  uint32_t xcr0;
#if defined(_MSC_VER)
  xcr0 = (uint32_t)_xgetbv(0);  /* min VS2010 SP1 compiler is required */
#else
  __asm__ ("xgetbv" : "=a" (xcr0) : "c" (0) : "%edx" );
#endif
  return ((xcr0 & 6) == 6); /* checking if xmm and ymm state are enabled in XCR0 */
}
#endif

#ifndef _WIN32
#ifndef ANDROID
#define x86_cpuid(func, values) __get_cpuid(func, values, values+1, values+2, values+3)
#else
#ifdef __i386__
static __inline__ void x86_cpuid(int func, int values[4])
{
  int a, b, c, d;
  /* We need to preserve ebx since we're compiling PIC code */
  /* this means we can't use "=b" for the second output register */
  __asm__ __volatile__ ( \
                         "push %%ebx\n"
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
static __inline__ void x86_cpuid(int func, int values[4])
{
  int64_t a, b, c, d;
  /* We need to preserve ebx since we're compiling PIC code */
  /* this means we can't use "=b" for the second output register */
  __asm__ __volatile__ ( \
                         "push %%rbx\n"
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

#if defined(__i386__) || defined(__x86_64__) || defined(_MSC_VER)
  int regs[4];

  /* According to http://en.wikipedia.org/wiki/CPUID */
#define VENDOR_INTEL_b  0x756e6547
#define VENDOR_INTEL_c  0x6c65746e
#define VENDOR_INTEL_d  0x49656e69

  x86_cpuid(0, regs);
  int maxid = regs[0];
  int vendorIsIntel = (regs[1] == VENDOR_INTEL_b &&
                       regs[2] == VENDOR_INTEL_c &&
                       regs[3] == VENDOR_INTEL_d);

  x86_cpuid(1, regs);
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
// OSXSAVE
  int OSXSAVE = 0;
  if ((regs[2] & (1 << 27)) != 0) {
    OSXSAVE = 1;
  }

  if (maxid>=7) {
  x86_cpuid(7, regs);
  if ((regs[1] & (1 << 5)) != 0) {
    g_cpuFeatures |= CPU_X86_FEATURE_AVX2;
  }
  if ((regs[1] & (1 << 29)) != 0) {
    g_cpuFeatures |= CPU_X86_FEATURE_SHA_NI;
  }
  }
// mask off avx if os does not support
  int AVX=0;
#ifdef _WIN32
#define XSTATE_MASK_AVX   (XSTATE_MASK_GSSE)
  typedef DWORD64 (WINAPI *GETENABLEDXSTATEFEATURES)();
  GETENABLEDXSTATEFEATURES pfnGetEnabledXStateFeatures = NULL;
// Get the addresses of the AVX XState functions.
  HMODULE hm = GetModuleHandleA("kernel32.dll");
  if ((pfnGetEnabledXStateFeatures = (GETENABLEDXSTATEFEATURES)GetProcAddress(hm, "GetEnabledXStateFeatures")) &&
      ((pfnGetEnabledXStateFeatures() & XSTATE_MASK_AVX) != 0))
    AVX=1;
  FreeLibrary(hm);
  if (!check_xcr0_ymm()) AVX=0;
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
  if (!check_xcr0_ymm()) AVX=0;
#endif
  if (!(AVX&&OSXSAVE)) {
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX;
    g_cpuFeatures &= ~CPU_X86_FEATURE_AVX2;
  }
#endif

}

uint64_t getCpuFeatures(void)
{
  return g_cpuFeatures;
}

