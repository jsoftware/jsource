/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Global Definitions                                                      */

#if defined(__clang_major__) && !defined(__clang__)
#error need workaround by define __clang__ in preprocessor macro
#endif

/* clang-cl */
#if defined(__clang__) && !defined(__GNUC__)
#define __GNUC__ 4
#undef __GNUC_MINOR__
#define __GNUC_MINOR__ 2
#undef __GNUC_PATCHLEVEL__
#define __GNUC_PATCHLEVEL__ 1
#endif

// ms vc++ defined _MSC_VER but clang-cl also defined _MSC_VER
// clang-cl doesn't emulate ms vc++ good enough
// and it breaks program logic previously guarded by _MSC_VER
// MMSC_VER means the real ms vc++ excluding clang-cl
// use MMSC_VER instead of _MSC_VER throughout JE source
#if defined(_MSC_VER) && !defined(__clang__)
#undef MMSC_VER
#define MMSC_VER
#else
#undef MMSC_VER
#endif
#if !defined(MMSC_VER)
#include <stddef.h>       // offsetof
#endif

#ifndef EMU_AVX
#define EMU_AVX 0
#endif
#if defined(MMSC_VER)
#undef EMU_AVX
#define EMU_AVX 0
#endif
#undef EMU_AVX2
#define EMU_AVX2 0

/* msvc does not define __SSE2__ */
#if !defined(__SSE2__)
#if defined(MMSC_VER)
#if (defined(_M_AMD64) || defined(_M_X64))
#define __SSE2__ 1
#elif _M_IX86_FP==2
#define __SSE2__ 1
#endif
#endif
#endif

// for debugging
#define NANTEST0        (fetestexcept(FE_INVALID))  // test but does not clear
#define dump_m128i(a,x) {__m128i _b=x;fprintf(stderr,"%s %x %x %x %x \n", a, ((unsigned int*)(&_b))[0], ((unsigned int*)(&_b))[1], ((unsigned int*)(&_b))[2], ((unsigned int*)(&_b))[3]);}
#define dump_m128i64(a,x) {__m128i _b=x;fprintf(stderr,"%s %lli %lli \n", a, ((long long*)(&_b))[0], ((long long*)(&_b))[1]);}
#define dump_m256i(a,x) {__m256i _b=x;fprintf(stderr,"%s %lli %lli %lli %lli \n", a, ((long long*)(&_b))[0], ((long long*)(&_b))[1], ((long long*)(&_b))[2], ((long long*)(&_b))[3]);}
#define dump_m256i16(a,x) {__m256i _b=x;fprintf(stderr,"%s %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x \n", a, ((unsigned short*)(&_b))[0], ((unsigned short*)(&_b))[1], ((unsigned short*)(&_b))[2], ((unsigned short*)(&_b))[3], ((unsigned short*)(&_b))[4], ((unsigned short*)(&_b))[5], ((unsigned short*)(&_b))[6], ((unsigned short*)(&_b))[7],((unsigned short*)(&_b))[8], ((unsigned short*)(&_b))[9], ((unsigned short*)(&_b))[10], ((unsigned short*)(&_b))[11], ((unsigned short*)(&_b))[12], ((unsigned short*)(&_b))[13], ((unsigned short*)(&_b))[14], ((unsigned short*)(&_b))[15]);}
#define dump_m256i32(a,x) {__m256i _b=x;fprintf(stderr,"%s %x %x %x %x %x %x %x %x \n", a, ((unsigned int*)(&_b))[0], ((unsigned int*)(&_b))[1], ((unsigned int*)(&_b))[2], ((unsigned int*)(&_b))[3], ((unsigned int*)(&_b))[4], ((unsigned int*)(&_b))[5], ((unsigned int*)(&_b))[6], ((unsigned int*)(&_b))[7]);}
#define dump_m256d(a,x) {__m256d _b=x;fprintf(stderr,"%s %f %f %f %f \n", a, ((double*)(&_b))[0], ((double*)(&_b))[1], ((double*)(&_b))[2], ((double*)(&_b))[3]);}
#define dump_m128d(a,x) {__m128d _b=x;fprintf(stderr,"%s %f %f \n", a, ((double*)(&_b))[0], ((double*)(&_b))[1]);}

#if defined(__i386__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86)
#ifndef C_AVX512
#define C_AVX512 0
#endif

#if !C_AVX2
#if C_AVX512
#ifdef C_AVX2
#undef C_AVX2
#endif
#define C_AVX2 1
#endif
#ifndef C_AVX2
#define C_AVX2 0
#endif
#endif

#if !C_AVX2
#if C_AVX512
#ifdef C_AVX2
#undef C_AVX2
#endif
#define C_AVX2 1
#endif
#ifndef C_AVX2
#define C_AVX2 0
#endif
#endif

#if !C_AVX
#if C_AVX2
#ifdef C_AVX
#undef C_AVX
#endif
#define C_AVX 1
#endif
#ifndef C_AVX
#define C_AVX 0
#endif
#endif
#endif

#ifdef _WIN32
#if EMU_AVX || EMU_AVX2 || C_AVX || C_AVX2
#ifndef _WIN64
#error not 64-bit compiler
#endif
#endif
#endif

#if C_AVX
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__i386__) || defined(__x86_64__))
#include <immintrin.h>
#endif
#if (defined(MMSC_VER))
#include <intrin.h>
#endif
// no EMU_AVX512; avx512 is not widespread yet, and older chips still downclock (so not worth it for small arrays), so still maintain avx2-specific paths
#if C_AVX2
#undef EMU_AVX2
#define EMU_AVX2 0
#else
#undef EMU_AVX2
#define EMU_AVX2 1
#include <stdint.h>
#include <string.h>
#include "avx2intrin-emu.h"
#endif
#undef EMU_AVX
#define EMU_AVX 0

#elif defined(__SSE2__)
#if EMU_AVX
#undef EMU_AVX2
#define EMU_AVX2 1   // test avx2 emulation
#include <stdint.h>
#include <string.h>
#include "avxintrin-emu.h"
#else
#include <emmintrin.h>
#endif
#define _CMP_EQ          0
#define _CMP_LT          1
#define _CMP_LE          2
#define _CMP_UNORD       3
#define _CMP_NEQ         4
#define _CMP_NLT         5
#define _CMP_NLE         6
#define _CMP_ORD         7
#undef _CMP_EQ_OQ
#undef _CMP_GE_OQ
#undef _CMP_GT_OQ
#undef _CMP_LE_OQ
#undef _CMP_LT_OQ
#undef _CMP_NEQ_OQ
#define _CMP_EQ_OQ _CMP_EQ
#define _CMP_GE_OQ _CMP_NLT
#define _CMP_GT_OQ _CMP_NLE
#define _CMP_LE_OQ _CMP_LE
#define _CMP_LT_OQ _CMP_LT
#define _CMP_NEQ_OQ _CMP_NEQ
#endif

#ifndef C_AVX2
#define C_AVX2 0
#endif
#ifndef C_AVX
#define C_AVX 0
#endif

#if defined(__aarch64__)||defined(_M_ARM64)
#if EMU_AVX
#undef EMU_AVX2
#define EMU_AVX2 1   // test avx2 emulation
#include <stdint.h>
#include <string.h>
#include "sse2neon.h"
#include "sse2neon2.h"
#include "avxintrin-neon.h"
#else
#include <arm_neon.h>
#endif
#endif

#if defined(__arm__)
#if defined(__ARM_NEON)
#include <arm_neon.h>
typedef double float64x2_t __attribute__ ((vector_size (16)));
#else
#include <stdint.h>
typedef int64_t int64x2_t __attribute__ ((vector_size (16)));
typedef double float64x2_t __attribute__ ((vector_size (16)));
#endif
#endif

#undef VOIDARG
#define VOIDARG

#if C_AVX512
#if (!defined(__clang__)) && defined(__GNUC__) && __GNUC__ < 10
static __inline __m512i
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_loadu_epi64 (void const *__P)
{
  struct __loadu_epi64 {
    __m512i_u __v;
  } __attribute__((__packed__, __may_alias__));
  return ((const struct __loadu_epi64*)__P)->__v;
}

static __inline void
__attribute__ ((__gnu_inline__, __always_inline__, __artificial__))
_mm512_storeu_epi64 (void *__P, __m512i __A)
{
  struct __storeu_epi64 {
    __m512i_u __v;
  } __attribute__((__packed__, __may_alias__));
  ((struct __storeu_epi64*)__P)->__v = __A;
}
#endif
#endif

#if SLEEF
#include "../sleef/include/sleef.h"
#endif

#if defined(_OPENMP)
#include <omp.h>
#else
typedef int omp_int_t;
static inline omp_int_t omp_get_thread_num() { return 0;}
static inline omp_int_t omp_get_max_threads() { return 1;}
static inline omp_int_t omp_get_num_threads() { return 1;}
#endif

#ifndef SYS // include js.h only once - dtoa.c
#include "js.h"
#endif

// If you are porting to a new compiler or architecture, see the bottom of this file
// for instructions on defining the CTTZ macros

#if SY_WINCE
#include "..\cesrc\cecompat.h"
#endif

#if (SYS & SYS_PCWIN)
#define HEAPCHECK  heapcheck()
#else
#define HEAPCHECK
#endif

#if (SYS & SYS_ATARIST)
#define __NO_INLINE__           1
#endif

#if (SYS & SYS_UNIX - SYS_SGI)
#include <unistd.h>
#include <memory.h>
#include <sys/types.h>
#endif

// likely/unlikely support
#if defined(__clang__) || defined(__GNUC__)
#ifndef likely
#define likely(x) __builtin_expect(!!(x),1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x),0)
#endif
#if defined(_WIN32) || defined(__clang__) || __GNUC__ > 9
#if (defined(__has_builtin) && __has_builtin(__builtin_expect_with_probability)) || (!defined(__clang__) && __GNUC__ >= 9)
#define common(x) __builtin_expect_with_probability(!!(x),1,0.6)
#define uncommon(x) __builtin_expect_with_probability(!!(x),1,0.4)
#else
#define common(x) likely(x)
#define uncommon(x) unlikely(x)
#endif
#else
#define common(x) likely(x)
#define uncommon(x) unlikely(x)
#endif
#else
#define likely(x) (!!(x))
#define unlikely(x) (!!(x))
#define common(x) (!!(x))
#define uncommon(x) (!!(x))
#endif

#if 1
#include <stdint.h>
#include <float.h>
#include <limits.h>
#define link unused_syscall_link
#define qdiv unused_netbsd_qdiv
#ifndef __USE_XOPEN2K
#define __USE_XOPEN2K  // for posix_memalign
#endif
#include <stdlib.h>
#undef link
#undef qdiv
#else
#define const /*nothing*/   /* blame rx.h */
#endif

#if ! SY_WINCE
#include <errno.h>
#include <stdio.h>
#endif

#include <math.h>
#include <string.h>  

#ifdef ANDROID
#include <android/log.h>
#define logcat_d(msg) __android_log_write(ANDROID_LOG_DEBUG,(const char*)"libj",msg)
#endif

#if defined(TARGET_OS_IPHONE)||defined(TARGET_OS_IOS)||defined(TARGET_OS_TV)||defined(TARGET_OS_WATCH)||defined(TARGET_OS_SIMULATOR)||defined(TARGET_OS_EMBEDDED)||defined(TARGET_IPHONE_SIMULATOR)
#define TARGET_IOS 1
#endif

#if defined(__aarch32__)||defined(__arm__)||defined(_M_ARM)||defined(__aarch64__)||defined(_M_ARM64)
#ifndef __ARM_FEATURE_UNALIGNED
#define ALIGNEDMEM
#endif
#endif

#if SY_WIN32
#if defined(_WIN32) && !defined(OLECOM)
#define OLECOM
#endif
#endif

#if SY_64
#define IMAX            9223372036854775807LL
#define IMAXPRIME       9223372036854775783LL
#define IMIN            (~9223372036854775807LL)   /* ANSI C LONG_MIN is  -LONG_MAX */
#define FLIMAX          9223372036854775296.     // largest FL value that can be converted to I
#define FLIMIN          ((D)IMIN)  // smallest FL value that can be converted to I
#define FMTI            "%lli"
#define FMTI02          "%02lli"
#define FMTI04          "%04lli"
#define FMTI05          "%05lli"

#if defined(MMSC_VER)  // SY_WIN32
#define strtoI         _strtoi64
#else
#define strtoI          strtoll
#endif

#else
#define IMAX            2147483647L
#define IMAXPRIME       IMAX
#define IMIN            (~2147483647L)   /* ANSI C LONG_MIN is  -LONG_MAX */
#define FLIMAX          ((D)IMAX+0.4)     // largest FL value that can be converted to I
#define FLIMIN          ((D)IMIN)  // smallest FL value that can be converted to I
#define FMTI            "%li"
#define FMTI02          "%02li"
#define FMTI04          "%04li"
#define FMTI05          "%05li"
#define strtoI          strtol
#endif

#define NEGATIVE0       (UIL)0x8000000000000000LL   // IEEE -0 (double precision)

#define C4MAX           0xffffffffUL
#define C4MIN           0L

#if (SYS & SYS_AMIGA)
#define XINF            "\177\377\000\000\000\000\000\000"
#define XNAN            "\177\361\000\000\000\000\000\000"
#endif

#if (SYS & SYS_ARCHIMEDES)
#define XINF            "\000\000\360\177\000\000\000\000"
#define XNAN            "\000\000\370\377\000\000\000\000"
#endif

#if (SYS & SYS_DEC5500) || SY_WINCE_SH
#define XINF            "\000\000\000\000\000\000\360\177"
#define XNAN            "\000\000\000\000\000\000\370\377"
#endif

#if (SYS & SYS_MACINTOSH)
/* for old versions of ThinkC */
/* #define XINF         "\177\377\000\000\000\000\000\000\000\000\000\000" */
/* #define XNAN         "\377\377\100\000\100\000\000\000\000\000\000\000" */
/* for ThinkC 7.0 or later */
#define XINF            "\177\377\177\377\000\000\000\000\000\000\000\000"
#define XNAN            "\377\377\377\377\100\000\000\000\000\000\000\000"
#endif

#if (SYS & SYS_SUN4+SYS_SUNSOL2)
#define XINF            "\177\360\000\000\000\000\000\000"
#define XNAN            "\177\377\377\377\377\377\377\377"
#endif

#if (SYS & SYS_VAX)
#define XINF            "\377\177\377\377\377\377\377\377"
#define XNAN            "\377\177\377\377\377\377\377\376" /* not right */
#endif

#if (SY_WINCE_MIPS || SY_WINCE_SH)
#if WIN32_PLATFORM_PSPC
#define XINF            "\000\000\000\000\000\000\360\177"
#define XNAN            "\377\377\377\377\377\377\367\177"
#else
#define XINF            "\000\000\000\000\000\000\360\177"
#define XNAN            "\001\000\000\000\000\000\360\177"
#endif
#endif

#if SY_WINCE_ARM
#define XINF            "\000\000\000\000\000\000\360\177"
#define XNAN            "\000\000\000\000\000\000\370\177"
#endif

#if C_LE
#ifndef XINF
#define XINF            "\000\000\000\000\000\000\360\177"
#define XNAN            "\000\000\000\000\000\000\370\377"
#endif
#endif

#ifndef XINF
#define XINF            "\177\360\000\000\000\000\000\000"
#define XNAN            "\177\370\000\000\000\000\000\000"
#endif


#ifndef PI
#define PI              ((D)3.14159265358979323846)
#endif
#define P2              ((D)6.28318530717958647693)
#ifndef OVERFLOW
#define OVERFLOW        ((D)8.988465674311578e307)
#endif
#ifndef UNDERFLOW
#define UNDERFLOW       ((D)4.450147717014403e-308)
#endif
// RESTRICT causes the compiler to generate better code by assuming no overlap of regions pointed to by pointers
// We use RESTRICT for routines that operate in-place on an argument.  This is strictly speaking a violation of the rule,
// but normally something like *z = *x + *y will not cause trouble because there is no reason to refetch an input after
// the result has been written.  On 32-bit machines, registers are so short that sometimes the compilers refetch an input
// after writing to *z, so we don't turn RESTRICT on for 32-bit
#if defined(MMSC_VER)
// RESTRICT is an attribute of a pointer, and indicates that no other pointer points to the same area
#define RESTRICT __restrict
// RESTRICTF is an attribute of a function, and indicates that the object returned by the function is not aliased with any other object
#define RESTRICTF __declspec(restrict)
#define PREFETCH(x) _mm_prefetch((x),_MM_HINT_T0)
#define PREFETCH2(x) _mm_prefetch((x),_MM_HINT_T1)   // prefetch into L2 cache but not L1
#endif
#ifdef __GNUC__
#define RESTRICT __restrict
// No RESTRICTF on GCC
#define PREFETCH(x) __builtin_prefetch(x)
#define PREFETCH2(x) __builtin_prefetch((x),0,2)   // prefetch into L2 cache but not L1
#endif

#ifdef __MINGW32__
#ifndef _SW_INVALID
#define _SW_INVALID    0x00000010 /* invalid */
#endif
#ifndef _EM_ZERODIVIDE
#define _EM_ZERODIVIDE  0x00000008
#endif
#define EM_INVALID    _SW_INVALID
#define EM_ZERODIVIDE _EM_ZERODIVIDE
#if defined(__STRICT_ANSI__)
extern int __cdecl _isnan (double);
extern unsigned int __cdecl _clearfp (void);
#endif
#ifndef _MAX_PATH
#define _MAX_PATH  (260)
#endif
#endif

#if SY_WIN32
struct jtimespec { long long tv_sec, tv_nsec; };
struct jtimeval { long long tv_sec, tv_usec; };
struct jtimezone { int tz_minuteswest, tz_dsttime; };
int jgettimeofday(struct jtimeval*, struct jtimezone*);
#else
#include <sys/time.h>
#include <time.h>
#define jtimespec timespec
#define jtimeval timeval
#define jtimezone timezone
#define jgettimeofday gettimeofday
#endif
struct jtimespec jmtclk(void); //monotonic clock.  Intended rel->abs conversions when sleeping; has poor granularity and slow on windows
struct jtimespec jmtfclk(void); //'fast clock'; maybe less inaccurate; intended for timed busywaiting

#if SY_64
#if defined(MMSC_VER)  // SY_WIN32
// RESTRICTI (for in-place) is used for things like *z++=*x++ - *y++;  Normally you wouldn't store to a z unless you were done reading
// the x and y, so it would be safe to get the faster loop that RESTRICT generates, even though strictly speaking if x or y is the
// same address as z the terms of the RESTRICT are violated.  But on 32-bit machines, registers are so tight that sometimes *z is used
// as a temp, which means we can't take the liberties there
#define RESTRICTI // __restrict don't take chances
#endif
#ifdef __GNUC__
#define RESTRICTI // __restrict  don't take chances
#endif
#endif  // SY_64

#ifndef RESTRICT
#define RESTRICT
#endif
#ifndef RESTRICTF
#define RESTRICTF
#endif
#ifndef RESTRICTI
#define RESTRICTI
#endif
// If PREFETCH is not defined, we won't generate prefetch instrs

// If the user switch C_NOMULTINTRINSIC is defined, suppress using it
#ifdef C_NOMULTINTRINSIC
#define C_USEMULTINTRINSIC 0
#else
#define C_USEMULTINTRINSIC 1
#endif

// disable C_USEMULTINTRINSIC if un-available
#if C_USEMULTINTRINSIC
#if !defined(MMSC_VER)
#if defined(__clang__)
#if !__has_builtin(__builtin_smull_overflow)
#undef C_USEMULTINTRINSIC
#define C_USEMULTINTRINSIC 0
#endif
#elif __GNUC__ < 5
#undef C_USEMULTINTRINSIC
#define C_USEMULTINTRINSIC 0
#endif
#endif
#endif

#if !SY_64 && defined(__GNUC__) && !defined(__clang__)
#if __GNUC__ < 5
#define __builtin_add_overflow(a,b,c) ({int64_t s=(int64_t)(a)+(int64_t)(b); *(c)=(long)s; (s<INT_MIN||s>INT_MAX);})
#define __builtin_sub_overflow(a,b,c) ({int64_t s=(int64_t)(a)-(int64_t)(b); *(c)=(long)s; (s<INT_MIN||s>INT_MAX);})
#define __builtin_mul_overflow(a,b,c) ({int64_t s=(int64_t)(a)*(int64_t)(b); *(c)=(long)s; (s<INT_MIN||s>INT_MAX);})
#endif
#endif

#if defined(__clang__) && ( (__clang_major__ > 3) || ((__clang_major__ == 3) && (__clang_minor__ > 5)))
/* needed by clang newer versions, no matter double_trick is inline asm or not */
#define NOOPTIMIZE __attribute__((optnone))
#elif __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 3))
#define NOOPTIMIZE __attribute__((optimize("O0")))
#else
#define NOOPTIMIZE
#endif

#define NALP            256             /* size of alphabet                */
#define NETX            2000            /* size of error display buffer    */
#define NPP             20              /* max value for quad pp           */
#define NPATH           1024            /* max length for path names,      */
                                        /* including trailing 0 byte       */
// Now we are trying to watch the C stack directly
#define USECSTACK       1   // 0 to go back to counting J recursions
#if USECSTACK
// NEW WAY
// The named-call stack is used only when there is a locative, EXCEPT that after a call to 18!:4 it is used until the function calling 18!:4 returns.
// Since startup calls 18!:4 without a name, we have to allow for the possibility of deep recursion in the name stack.  Normally only a little of the stack is used
#if defined(_WIN32)
#define CSTACKSIZE      (SY_64?12009472:1015808)  // size we allocate in the calling function, aligned to 16k system page size
#else
#if (defined(ANDROID) && !defined(__LP64__)) || (defined(__OpenBSD__) && defined(__aarch64__))
#define CSTACKSIZE      (SY_64?4194304:1015808)  // OS default stack size 4MB, aligned to 16k system page size
#else
#define CSTACKSIZE      (SY_64?7946240:1015808)  // OS default stack size 8MB, aligned to 16k system page size
#endif
#endif
#define CSTACKRESERVE   100000  // amount we allow for slop before we sample the stackpointer, and after the last check
#else
// OBSOLETE OLD WAY (with USECSTACK off)
// Sizes for the internal stacks.  The goal here is to detect a runaway recursion before it creates a segfault.  This cannot
// be done with precision because we don't know how much C stack we have, or how much is used by a recursion (and anyway it depends on
// what J functions are running).
// There are two limits: maximum depth of J functions, and maximum depth of named functions.
// increase OS stack limit instead of restricting NFDEP/NFCALL
#define NFDEP           2000L  // 4000             // fn call depth - for debug builds, must be small (<0xa00) to avoid stack overflow, even smaller for non-AVX
#endif 
//The named-function stack is intelligent
// and stacks only when there is a locale change or deletion; it almost never limits unless locatives are used to an extreme degree.
// The depth of the C stack will normally limit stack use.
#define NFCALL          (1000L)      // call depth for named calls, not important.  Must fit into an S

// start and length for the stored vector of ascending integers
#define IOTAVECBEGIN (-20)
#define IOTAVECLEN 400   // must be >= 256 so all memsets can be sourced from here

// modes for indexofsub()
#define IIOPMSKX        5  // # bits of flags
#define IIOPMSK         (((I)1<<IIOPMSKX)-1)     // operation bits.  INTER also uses bit 3, which is included as a modifier in the switches
#define IIOPMSKINIT     0xf  // 
#define IIDOT           0        // IIDOT and IICO must be 0-1
#define IICO            1
#define INUBSV          2   // BIT arrays INUBSV-INUBI init to 1 to that out-of-bounds in LESS keeps the value
#define INUB            3
#define ILESS           4
#define INUBI           5
#define IEPS            6   // BIT arrays IEPS and above init to 0 so out-of-bounds means not included
// the I...EPS values below are wired into the function table at the end of vcompsc.c, where they are combined with a comparison
#define II0EPS          7  // i.&0@:e.   this must come first; others base on it
#define II1EPS          8  // i.&1@:e.
#define IJ0EPS          9  // i:&0@:e.
#define IJ1EPS          10  // i:&1@:e.
#define ISUMEPS         11  // +/@:e.
#define IANYEPS         12  // +./@:e.
#define IALLEPS         13  // *./@:e.
#define IIFBEPS         14   // I.@e.
#define IFORKEY         15  // special key support: like i.~, but add # values mapped to the index, and return #unique values in AM
#define IINTER          16  // ([ -. -.)
#define IIMODFIELD      ((I)7<<IIOPMSKX)  // bits used to indicate processing options
#define IIMODPACKX      5
#define IIMODPACK       (((I)1)<<IIMODPACKX)  // modifier for type.  (small-range search except i./i:) In IIDOT/IICO, indicates reflexive application.  In others, indicates that the
                              // bitmask should be stored as packed bits rather than bytes
#define IIMODREFLEXX    5   // overlaps IIMODPACK; OK because reflexive i./i: needs to know where the match was & can't use bitmask
#define IIMODREFLEX     (((I)1)<<IIMODREFLEXX)  // (small-range i. and i:) this is i.~/i:~ (hashing) this is i.~/i:~/~./~:/I.@:~.
#define IIMODFULLX      6
#define IIMODFULL       (((I)1)<<IIMODFULLX)  // (small-range search) indicates that the min/max values cover the entire range of possible inputs, so no range checking is required.  Always set for hashing
#define IIMODBASE0X     7
#define IIMODBASE0      (((I)1)<<IIMODBASE0X)  // set in small-range i./i: (which never use BITS) to indicate that the hashtable starts at index 0 and has m in the place of unused indexes.  Set in hashing always, with same meaning
#define IIMODBITSX      8
#define IIMODBITS       (((I)1)<<IIMODBITSX)  // set if the hash field stores bits rather than indexes.  Used only for small-range and not i./i:.  IIMODPACK qualifies this, indicating that the bits are packed
#define IIMODFORCE0X    9
#define IIMODFORCE0     (((I)1)<<IIMODFORCE0X)  // set to REQUIRE a (non-bit) allocation to reset to offset 0 and clear
#define IPHCALCX        10
#define IPHCALC         (((I)1)<<IPHCALCX)   // set when we are calculating a prehashed table
#define IINOTALLOCATEDX  11
#define IINOTALLOCATED  (((I)1)<<IINOTALLOCATEDX)  // internal flag, set when the block has not been allocated
#define IIOREPSX        12
#define IIOREPS         (((I)1)<<IIOREPSX)  // internal flag, set if mode is i./i:/e./key, but not if prehashing
#define IREVERSEDX      13
#define IREVERSED       (((I)1)<<IREVERSEDX)   // set if we have decided to reverse the hash in a small-range situation
#define IPHOFFSETX      14
#define IPHOFFSET       (((I)1)<<IPHOFFSETX)              /* offset for prehashed versions - set when we are using a prehashed table   */
#define IPHIDOT         (IPHOFFSET+IIDOT)
#define IPHICO          (IPHOFFSET+IICO)
#define IPHEPS          (IPHOFFSET+IEPS)
#define IPHI0EPS        (IPHOFFSET+II0EPS)
#define IPHI1EPS        (IPHOFFSET+II1EPS)
#define IPHJ0EPS        (IPHOFFSET+IJ0EPS)
#define IPHJ1EPS        (IPHOFFSET+IJ1EPS)
#define IPHSUMEPS       (IPHOFFSET+ISUMEPS)
#define IPHANYEPS       (IPHOFFSET+IANYEPS)
#define IPHALLEPS       (IPHOFFSET+IALLEPS)
#define IPHIFBEPS       (IPHOFFSET+IIFBEPS)
#define IPHINTER        (IPHOFFSET+IINTER)
#define ISFUX           15
#define ISFU            (((I)1)<<ISFUX)  // i.!.1 - sequential file update

#if C_AVX   // _mm_round_pd requires sse4.1, mm256 needs avx
#define jceil(x) _mm256_cvtsd_f64(_mm256_round_pd(_mm256_set1_pd(x),(_MM_FROUND_TO_POS_INF |_MM_FROUND_NO_EXC)))   // ugly but clang understands
#define jfloor(x) _mm256_cvtsd_f64(_mm256_round_pd(_mm256_set1_pd(x),(_MM_FROUND_TO_NEG_INF |_MM_FROUND_NO_EXC)))
#define jround(x) _mm256_cvtsd_f64(_mm256_round_pd(_mm256_set1_pd(x),(_MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC)))
#else
#define jceil(x) ceil(x)
#define jfloor(x) floor(x)
#define jround(x) floor(0.5+(x))  // for paranoid compatibility with earlier versions
#endif


#define BB              8      /* # bits in a byte */
#define LGBB 3    // lg(BB)
#if SY_64
#define BW              64     /* # bits in a word */
#define LGSZI 3    // lg(#bytes in an I)
#else
#define BW              32
#define LGSZI 2
#endif
#define LGBW (LGSZI+LGBB)  // lg (# bits in a word)

// nominal cache sizes for current processors
#define L1CACHESIZE (((I)1)<<15)
#define L2CACHESIZE (((I)1)<<18)
#define L3CACHESIZE (((I)1)<<22)

#define TOOMANYATOMSX 47  // more atoms than this is considered overflow (64-bit).  i.-family can't handle more than 2G cells in array.

// Tuning options for cip.c
#if ((C_AVX || EMU_AVX) && PYXES) || !defined(_OPENMP)
#define IGEMM_THRES  (-1)     // when m*n*p less than this use cached; when higher, use BLAS
#define DGEMM_THRES  (-1)     // when m*n*p less than this use cached; when higher, use BLAS   _1 means 'never'
#define ZGEMM_THRES  (-1)     // when m*n*p less than this use cached; when higher, use BLAS   _1 means 'never'
#elif defined(_WIN32)
// tuned for windows
#define IGEMM_THRES  (400*400*400)   // when m*n*p less than this use cached; when higher, use BLAS
#define DGEMM_THRES  (300*300*300)   // when m*n*p less than this use cached; when higher, use BLAS   _1 means 'never'
#define ZGEMM_THRES  (400*400*400)   // when m*n*p less than this use cached; when higher, use BLAS  
#else
// tuned for linux
#define IGEMM_THRES  (200*200*200)   // when m*n*p less than this use cached; when higher, use BLAS
#define DGEMM_THRES  (200*200*200)   // when m*n*p less than this use cached; when higher, use BLAS   _1 means 'never'
#define ZGEMM_THRES  (60*60*60)      // when m*n*p less than this use cached; when higher, use BLAS  
#endif
#define DCACHED_THRES  (64*64*64)    // when m*n*p less than this in a single thread use blocked; when higher, use cached
#define DCACHED_THRESn  (24*24*24)    // when m*n*p less than this, don't even look for multithreads; use blocked


// Debugging options

// Use MEMAUDIT to sniff out errant memory alloc/free
#ifndef MEMAUDIT
#define MEMAUDIT 0x00   // Bitmask for memory audits: 
//        1:  make sure chains are valid (check headers)
//        2:  full audit of tpush/tpop
//            detect double-frees before they happen,
//            at the time of the erroneous tpush
//        4:  write garbage to memory before we free it (except reserved area)
//        8:  fill block with other garbage after we allocate it
//     0x10:  (or 16) audit freelist at every alloc/free
//            (starting after you have run 6!:5 (1) to turn it on)
//     0x20:  audit freelist at end of every sentence regardless of 6!:5
//     0x40:  enable guard blocks (libgmp mallocs only)
//
// Thus 1+4+8 (or 13 or 0xD) will verify that there are no blocks
// being used after they are freed, or freed prematurely. If you
// get a wild free, turn on bit 0x2. 2 will detect double-frees
// before they happen, at the time of the erroneous tpush
#endif
#define MEMAUDITPCALLENABLE 1     // expression for enabling stack auditing - enable auditing when true and enabled by MEMAUDIT&0x20 || jt->peekdata
#ifndef AUDITEXECRESULTS
#define AUDITEXECRESULTS 0    // When set, we go through all execution results to verify recursive and virtual bits are OK, and m nonzero if AC<0
#endif
#ifndef FORCEVIRTUALINPUTS
#define FORCEVIRTUALINPUTS 0  // When 1 set, we make all non-inplaceable noun inputs to executions VIRTUAL.  Tests should still run
#endif
                           // When 2 set, make all outputs from RETF() virtual.  Tests for inplacing will fail; that's OK if nothing crashes
#ifndef NAMETRACK
#define NAMETRACK 0    // turn on to define trackinfo in unquote, xdefn, line
#endif
// set FINDNULLRET to trap when a routine returns 0 without having set an error message
#ifndef FINDNULLRET
#define FINDNULLRET 0
#endif
#ifndef CRASHLOG
#define CRASHLOG 0     // set to allow writing to crashlog
#endif

#ifndef MEMHISTO
#define MEMHISTO 0       // set to create a histogram of memory requests, interrogated by 9!:54/9!:55
#endif

#define MAXTHREADS 63    // maximum number of tasks running at once, including the master thread.   System lock polls every thread, allocated or not, which is the only real limit on size.  Unactivated
                       // threads will be paged out.
#define MAXTHREADSRND 64  // MAXTHREADS+1, rounded up to power-of-2 bdy to get the the JST block aligned on a multiple of its size.  The JTT blocks come after the JTT block, which has the same size
#if MAXTHREADS>255
#define WLOCKBIT 0x8000  // the LSB of the part of a 16-bit lock used for write locks.
#else
#define WLOCKBIT 0x100  // With <256 threads, we split the lock into 2 8-bit sections so we can use LOCK XADD instructions
#endif

#define MAXTHREADPOOLS 8  // max # thread pools supported
#define MAXTHREADSINPOOL 63  // Max threads in a single pool.  The low bits of the task pointer are used as a lock, so that code will have to be rewritten if MAXTHREADSINPOOL>63 (which is the allocation boundary for the
                       // task block).  As of now, this limit is immaterial, because every thread in the system might choose to start a job on a single threadpool, which limits the total number of threads to 63.  But
                       // we could force job/task/thread creators to serialize on a lock, which would limit the number of waits from outside the pool to 1, and then we could have more threads total as long as
                       // the number in a single pool is limited

// tpop stack is allocated in units of NTSTACK, but processed in units of NTSTACKBLOCK on an NTSTACKBLOCK boundary to reduce waste in each allocation.
// If we audit execution results, we use a huge allocation so that tpop pointers can be guaranteed never to need a second one, & will thus be ordered
#define NTSTACK         (1LL<<(AUDITEXECRESULTS?24:14))          // number of BYTES in an allocated block of tstack - pointers to allocated blocks - allocation is bigger to leave this many bytes on boundary
#define NTSTACKBLOCK    2048            // boundary for beginning of stack block

// flags for jteformat
#define EMSGE 0xff  // the error-code part
#define EMSGNOEVM 0x200  // set to suppress moving the terse message
#define EMSGLINEISA 0x400  // line contains A block for message (otherwise it points to string if any and info has the length of the string)
#define EMSGCXINFO 0x800  // info contains line#/col# of error
#define EMSGSPACEAFTEREVM 0x1000 // set if terse message should be followed by a space 
#define EMSGLINEISTERSE 0x2000 // set if line has the text for the terse message (13!:8)
#define EMSGLINEISNAME 0x4000 // set if line has the name to use in place of jt->curname
#define EMSGFROMPYX 0x8000  // set if this error is being copied from a pyx (it can't be analyzed, and it should be marked specially
#define EMSGNOEFORMAT 0x10000  // set if this error should not be passed to eformat for processing
#define EMSGINVCHAR 0x20000  // set to append 'invalid char' to msg
#define EMSGINVINFL 0x40000  // set to append 'invalid inflection' to msg

#ifndef PYXES
#define PYXES 1
#endif
#if !SY_64
#undef PYXES
#define PYXES 0
#endif
#define ARTIFPYX 0
#if ARTIFPYX&&PYXES
#define HIPIFARTIF(w,f) jtartiffut(jt,w,f) // for testing, create pyx results from <, force-box, and sometimes ;
#else
#define HIPIFARTIF(w,f) (w)
#endif

// if we are not multithreading, report the master thread only
#if !PYXES
#undef MAXTHREADS
#define MAXTHREADS 1  // override to no tasks if no pyxes
#endif
#if defined(ANDROID) && defined(__x86_64__)
#undef MAXTHREADS
#define MAXTHREADS 1  // workaround for android x86_64
#endif

#if PYXES
#define REPATGCLIM 0x100000   // When this many bytes have been repatriated to a thread, call a GC in that thread
#define REPATOLIM (REPATGCLIM/32) // When an outgoing repatriation queue contains this many bytes, flush it
#else
// if we are not multithreading, we replace the atomic operations with non-atomic versions
#define __atomic_store_n(aptr,val, memorder) (*aptr=val)
#define __atomic_load_n(aptr, memorder) *aptr
#if defined(__clang__) || __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 8))
#define __atomic_compare_exchange_n(aptr, aexpected, desired, weak, success_memorder, failure_memorder) (*aptr=desired,1)
#define __atomic_exchange_n(aptr, val, memorder) ({__auto_type rrres=*aptr; *aptr =val; rrres;})
#define __atomic_fetch_or(aptr, val, memorder)   ({__auto_type rrres=*aptr; *aptr|=val; rrres;})
#define __atomic_fetch_sub(aptr, val, memorder)  ({__auto_type rrres=*aptr; *aptr-=val; rrres;})
#define __atomic_fetch_add(aptr, val, memorder)  ({__auto_type rrres=*aptr; *aptr+=val; rrres;})
#define __atomic_fetch_and(aptr, val, memorder)  ({__auto_type rrres=*aptr; *aptr&=val; rrres;})
#else
#define __atomic_compare_exchange_n(aptr, aexpected, desired, weak, success_memorder, failure_memorder) (*aptr=(void*)desired,1)
#define __atomic_exchange_n(aptr, val, memorder) ({I rrres=(intptr_t)*aptr; *aptr=val; rrres;})
#define __atomic_fetch_or(aptr, val, memorder) ({I rrres=(intptr_t)*aptr; *aptr|=val; rrres;})
#define __atomic_fetch_sub(aptr, val, memorder) ({I rrres=(intptr_t)*aptr; *aptr-=val; rrres;})
#define __atomic_fetch_add(aptr, val, memorder) ({I rrres=(intptr_t)*aptr; *aptr+=val; rrres;})
#define __atomic_fetch_and(aptr, val, memorder) ({I rrres=(intptr_t)*aptr; *aptr&=val; rrres;})
#endif
#define __atomic_add_fetch(aptr, val, memorder) (*aptr+=val)
#define __atomic_sub_fetch(aptr, val, memorder) (*aptr-=val)
#define __atomic_and_fetch(aptr, val, memorder) (*aptr&=val)
#define REPATGCLIM 0   // no repat
#endif
//convenient abbreviations
#define casa(p,e,d) __atomic_compare_exchange_n(p,e,d,0,__ATOMIC_ACQ_REL,__ATOMIC_RELAXED)
#define cass(p,e,d) __atomic_compare_exchange_n(p,e,d,0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST)
#define aadd(p,v) __atomic_fetch_add(p,v,__ATOMIC_ACQ_REL)
#define adda(p,v) __atomic_add_fetch(p,v,__ATOMIC_ACQ_REL)
#define lda(p) __atomic_load_n(p,__ATOMIC_ACQUIRE)
#define lds(p) __atomic_load_n(p,__ATOMIC_SEQ_CST)
#define sta(p,v) __atomic_store_n(p,v,__ATOMIC_RELEASE) //technically not 'a'
#define sts(p,v) __atomic_store_n(p,v,__ATOMIC_SEQ_CST)
#define xchga(p,n) __atomic_exchange_n(p,n,__ATOMIC_ACQ_REL)

#ifdef __x86_64__
#define FAST_AADD 1
#else
#define FAST_AADD 0
#endif

#define ADDBYTESINI1(t) (t=(t&ALTBYTES)+((t>>8)&ALTBYTES)) // sig in 01ff01ff01ff01ff, then xxxxxxxx03ff03ff, then xxxxxxxxxxxx07ff, then 00000000000007ff
#if BW==64
#define ALTBYTES 0x00ff00ff00ff00ffLL
#define ALTSHORTS 0x0000ffff0000ffffLL
// t has totals per byte-lane, result combines them into single total.  t must be an lvalue
#define ADDBYTESINIn(t) (t = (t>>32) + t, t = (t>>16) + t, t&=0xffff) // sig in 01ff01ff01ff01ff, then xxxxxxxx03ff03ff, then xxxxxxxxxxxx07ff, then 00000000000007ff
#define VALIDBOOLEAN 0x0101010101010101LL   // valid bits in a Boolean
#else
#define ALTBYTES 0x00ff00ffLL
#define ALTSHORTS 0x0000ffffLL
#define ADDBYTESINIn(t) (t = (t>>16) + t, t&=0xffff) // sig in 01ff01ff, then xxxx03ff, then 000003ff
#define VALIDBOOLEAN 0x01010101   // valid bits in a Boolean
#endif
#define ADDBYTESINI(t) (ADDBYTESINI1(t) , ADDBYTESINIn(t)) // sig in 01ff01ff, then xxxx03ff, then 000003ff
#define BOOLEANSIGN (VALIDBOOLEAN<<(BB-1))

// macros for bit testing
#define SGNIF(v,bitno) ((I)(v)<<(BW-1-(bitno)))  // Sets sign bit if the numbered bit is set
#define SGNIF4(v,bitno) ((I4)(v)<<(32-1-(bitno)))  // Sets sign bit if the numbered bit is set, in an I4
#define SGNONLYIF(v,bitno) (((v)>>(bitno))<<(BW-1))  // Sets sign bit if the numbered bit is set, clears all other bits
#define SGNIFNOT(v,bitno) (~SGNIF((v),(bitno)))  // Clears sign bit if the numbered bit is set
#define REPSGN(x) ((I)(x)>>(BW-1))  // replicate sign bit of x to entire word
#define REPSGN4(x) ((I4)(x)>>(32-1))  // replicate sign bit of x to entire I4 - x is forced to I4
#define SGNTO0(x) ((UI)(x)>>(BW-1))  // move sign bit to bit 0, clear other bits
#define SGNTO0US(x) ((US)(x)>>(16-1))  // move sign bit to bit 0, clear other bits

#define A0              0   // a nonexistent A-block
#define ABS(a)          (0<=(a)?(a):-(a))
#if 0   // obsolete
// support for eformat_j_
// Put the appropriate macro at the start of any routine that will do ASSERTE.
// the piece at the beginning of the routine will vector off to ASSERTE, passing in the error code, the ranks, the original self, and the args
// The EFORMAT macro must follow PREFIP.  The args are the ranks
#define ESECT(callstg) if(0){eformat: R jtjsignale callstg;}
#define EFORMAT1(m) A origw=w, origself=self; ESECT((jt,(R2MAX<<RANK2TX)|m,origself,0,origw))
#define EFORMAT2(l,r) A origa=a, origw=w, origself=self; ESECT((jt,(R2MAX<<RANK2TX)|((I)l<<RANKTX)|r,origself,origa,origw))
#define EFORMAT1IRS(m) A origw=w, origself=self; RANK2T origr=jt->ranks;  ESECT((jt,(origr<<RANK2TX)|m,origself,0,origw))  // use when verb supports IRS
#define EFORMAT2IRSCOMMON(l,r,irsr) A origa=a, origw=w, origself=self; RANK2T origr=irsr;  ESECT((jt,(origr<<RANK2TX)|((I)l<<RANKTX)|r,origself,origa,origw))
#define EFORMAT2IRS(l,r) EFORMAT2IRSCOMMON(l,r,jt->ranks)
#define ASSERTE(b,e) {if(unlikely(!(b))){jt=(J)((I)jt+e); goto eformat;}}
#define ASSERTF(b,e,s...){if(unlikely(!(b))){R jtjsignale((J)((I)jt+e),0,0,0,0);}}
#endif
#include "jr0.h" // #define ASSERT(b,e) {if(unlikely(!(b))){jsignal(e); R 0;}}
#define ASSERTF(b,e,s...)     {if(unlikely(!(b))){jsignal(e); R 0;}}
#define ASSERTSUFF(b,e,suff)   {if(unlikely(!(b))){jsignal(e); {suff}}}  // when the cleanup is more than a goto
#define ASSERTGOTO(b,e,lbl)   ASSERTSUFF(b,e,goto lbl;)
#define ASSERTTHREAD(b,e)     {if(unlikely(!(b))){jtjsignal(jm,e); R 0;}}   // used in io.c to signal in master thread
// version for debugging
// #define ASSERT(b,e)     {if(unlikely(!(b))){fprintf(stderr,"error code: %i : file %s line %d\n",(int)(e),__FILE__,__LINE__); jsignal(e); R 0;}}
#define ASSERTD(b,s)    {if(unlikely(!(b))){jsigd((s)); R 0;}}
#define ASSERTMTV(w)    {ARGCHK1(w); ASSERT(1==AR(w),EVRANK); ASSERT(!AN(w),EVLENGTH);}
#define ASSERTN(b,e,nm) {if(unlikely(!(b))){jtjsignale(jt,e|EMSGLINEISNAME,(nm),0); R 0;}}  // signal error, overriding the running name with a different one
#define ASSERTNGOTO(b,e,nm,lbl) {if(unlikely(!(b))){jtjsignale(jt,e|EMSGLINEISNAME,(nm),0); goto lbl;}}  // same, but without the exit
#define ASSERTPYX(e)   {jsignal((e)|(EMSGFROMPYX|EMSGNOEFORMAT)); R 0;}
#define ASSERTSYS(b,s)  {if(unlikely(!(b))){fprintf(stderr,"system error: %s : file %s line %d\n",s,__FILE__,__LINE__); jsignal(EVSYSTEM); jtwri(JJTOJ(jt),MTYOSYS,"",(I)strlen(s),s); R 0;}}
#define ASSERTSYSV(b,s)  {if(unlikely(!(b))){fprintf(stderr,"system error: %s : file %s line %d\n",s,__FILE__,__LINE__); jsignal(EVSYSTEM); jtwri(JJTOJ(jt),MTYOSYS,"",(I)strlen(s),s);}}
#define ASSERTW(b,e)    {if(unlikely(!(b))){if((e)<=NEVM)jsignal(e); else jt->jerr=(e); R;}}
#define ASSERTWR(c,e)   {if(unlikely(!(c))){R e;}}

// verify that shapes *x and *y match for l axes using AVX for rank<=vector size, memcmp otherwise
#if C_AVX512
// We would like to use these AVX versions because they generate fewest instructions.
// Avoid call to memcmp to save registers
#define ASSERTAGREECOMMON(x,y,l,ASTYPE) \
 {I *aaa=(x), *aab=(y); I aai=(l); \
  if(likely(aai<=4)){__mmask8 endmask=_bzhi_u32(0xf,aai); \
   endmask=_mm256_cmpneq_epi64_mask(_mm256_maskz_loadu_epi64(endmask,aaa),_mm256_maskz_loadu_epi64(endmask,aab)); \
   ASTYPE(!endmask,EVLENGTH); \
  }else{NOUNROLL do{--aai; ASTYPE(aaa[aai]==aab[aai],EVLENGTH)}while(aai);} \
 }
// set r nonzero if shapes disagree
#define TESTDISAGREE(r,x,y,l) \
 {I *aaa=(x), *aab=(y); I aai=(l); \
  if(likely(aai<=8)){__mmask8 endmask=_bzhi_u32(0xf,aai); \
   r=!!_mm256_cmpneq_epi64_mask(_mm256_maskz_loadu_epi64(endmask,aaa),_mm256_maskz_loadu_epi64(endmask,aab)); /* result is nonzero if any mismatch */ \
  }else{NOUNROLL do{--aai; r=0; if(aaa[aai]!=aab[aai]){r=1; break;}}while(aai);} \
 }
#elif (C_AVX2&&SY_64) || EMU_AVX
#define ASSERTAGREECOMMON(x,y,l,ASTYPE) \
 {D *aaa=(D*)(x), *aab=(D*)(y); I aai=(l); \
  if(likely(aai<=NPAR)){__m256i endmask = _mm256_loadu_si256((__m256i*)(validitymask+NPAR-aai)); \
   endmask=_mm256_castpd_si256(_mm256_xor_pd(_mm256_maskload_pd(aaa,endmask),_mm256_maskload_pd(aab,endmask))); \
   ASTYPE(_mm256_testz_si256(endmask,endmask),EVLENGTH); /* result is 1 if all match */ \
  }else{NOUNROLL do{--aai; ASTYPE(((I*)aaa)[aai]==((I*)aab)[aai],EVLENGTH)}while(aai);} \
 }
#define TESTDISAGREE(r,x,y,l) \
 {I *aaa=(x), *aab=(y); I aai=(l); \
  if(likely(aai<=NPAR)){__m256i endmask = _mm256_loadu_si256((__m256i*)(validitymask+NPAR-aai)); \
   endmask=_mm256_xor_si256(_mm256_maskload_epi64(aaa,endmask),_mm256_maskload_epi64(aab,endmask)); \
   r=!_mm256_testz_si256(endmask,endmask); /* result is 1 if any mismatch */ \
  }else{NOUNROLL do{--aai; r=0; if(aaa[aai]!=aab[aai]){r=1; break;}}while(aai);} \
 }
#else
#define ASSERTAGREECOMMON(x,y,l,ASTYPE) \
 {I *aaa=(x), *aab=(y); I aai=(l); \
  if(likely(aai<=2)){ \
   aai-=1; aaa=(aai<0)?(I*)&validitymask[1]:aaa; aab=(aai<0)?(I*)&validitymask[1]:aab; \
   ASTYPE(((aaa[0]^aab[0])+(aaa[aai]^aab[aai]))==0,EVLENGTH); \
  }else{ASTYPE(!memcmp(aaa,aab,aai<<LGSZI),EVLENGTH)} \
 }
  
#define TESTDISAGREE(r,x,y,l) \
 {I *aaa=(x), *aab=(y); I aai=(l); \
  if(likely(aai<=2)){ \
   aai-=1; aaa=(aai<0)?(I*)&validitymask[1]:aaa; aab=(aai<0)?(I*)&validitymask[1]:aab; \
   r=((aaa[0]^aab[0])+(aaa[aai]^aab[aai]))!=0;  \
  }else{r=memcmp(aaa,aab,aai<<LGSZI)!=0;} \
 }
#endif
// obsolete #define ASSERTAGREE(x,y,l) ASSERTAGREE2(x,y,l,l,l)
#define ASSERTAGREE(x,y,l) ASSERTAGREECOMMON(x,y,l,ASSERT)
// obsolete #define ASSERTEAGREE(x,y,l) ASSERTAGREECOMMON(x,y,l,ASSERTE)
#define ASSERTAGREESEGFAULT (x,y,l) {I *aaa=(x), *aab=(y), aai=(l)-1; do{aab=aai<0?aaa:aab; if(aaa[aai]!=aab[aai])SEGFAULT; --aai; aab=aai<0?aaa:aab; if(aaa[aai]!=aab[aai])SEGFAULT; --aai;}while(aai>=0); }
// BETWEENx requires that lo be <= hi
#define BETWEENC(x,lo,hi) ((UI)((x)-(lo))<=(UI)((hi)-(lo)))   // x is in [lo,hi]
#define BETWEENO(x,lo,hi) ((UI)((x)-(lo))<(UI)((hi)-(lo)))   // x is in [lo,hi)
#if SY_64
// The Bloom filter is set of 4 16-bit sections.  For each hash value, a single bit is set in each section.  The LOCBLOOM of a locale holds the OR or all the Bloom masks that
// have been written.  When a value is looked up, we skip the table if LOCBLOOM doesn't have a 1 in each position presented by the new mask.
#define BLOOMMASK(hash) ((0x1LL<<((hash)&15))+(0x10000LL<<(((hash)>>4)&15))+(0x100000000LL<<(((hash)>>8)&15))+(0x1000000000000LL<<(((hash)>>12)&15)))   // Bloom filter for a given hash
#else
#define BLOOMMASK(hash) ((1L<<((hash)&15))+(0x10000L<<(((hash)>>4)&15)))   // Bloom filter for a given hash
#endif
#define BMK(x) (1LL<<(x))  // bit number x
// test for equality of 2 8-bit values simultaneously
#define BOTHEQ8(x,y,X,Y) ( ((US)(C)(x)<<8)+(US)(C)(y) == ((US)(C)(X)<<8)+(US)(C)(Y) )
#if PYXES
#define CCOMMON(x,pref,err) ({A res=(x); pref if(unlikely(AT(res)&PYX))if(unlikely((res=jtpyxval(jt,res))==0))err; res; })   // extract & resolve contents; execute err if error in resolution  x may have side effects
#define READLOCK(lock) {S prev; if(unlikely(((prev=__atomic_fetch_add(&lock,1,__ATOMIC_ACQ_REL))&(S)-WLOCKBIT)!=0))readlock(&lock,prev);}
#if WLOCKBIT==0x8000
#define WRITELOCK(lock)  {S prev; if(unlikely((prev=__atomic_fetch_or(&lock,(S)WLOCKBIT,__ATOMIC_ACQ_REL))!=0))writelock(&lock,prev);}
#else
#define WRITELOCK(lock)  {S prev; if(unlikely((prev=__atomic_fetch_add(&lock,WLOCKBIT,__ATOMIC_ACQ_REL))!=0))writelock(&lock,prev);}
#endif
#define READUNLOCK(lock) __atomic_fetch_sub(&lock,1,__ATOMIC_ACQ_REL);  // decrement the read bits
#define WRITEUNLOCK(lock) __atomic_fetch_and(&lock,WLOCKBIT-1, __ATOMIC_ACQ_REL);  // clear all the write bits
#else
#define CCOMMON(x,pref,err) (x)
#define READLOCK(lock) ;
#define WRITELOCK(lock) ;
#define READUNLOCK(lock) ;
#define WRITEUNLOCK(lock) ;
#endif
#define C(x) CCOMMON(x,,R 0)  // normal case: return on error
#define CERR(x) CCOMMON(x,,R jt->jerr)  // return error code on error
#define CNOERR(x) CCOMMON(x,,)  // value has been resolved before & there cannot be an error
#define CNULL(x) CCOMMON(x,if(likely(res!=0)),R 0)  // if x is 0, keep it 0; return 0 if resolves to error
#define CNULLNOERR(x) CCOMMON(x,if(likely(res!=0)),)  // if x is 0, keep it 0; ignore error
// CALL[12] is an eformat point if we have a valid self.  AT is 0 in an invalid self
#define CALL1COMMON(f,w,fs,j,pop)   ({A carg1=(w), carg3=(A)(fs), cargz; cargz=(f)(j,carg1,carg3,carg3); pop if(unlikely(cargz==0)){if(AT(carg3)!=0)jteformat(j,carg3,carg1,0,0);} cargz;})
#define CALL1(f,w,fs)   CALL1COMMON(f,w,fs,jt,)
#define CALL1IP(f,w,fs)   CALL1COMMON(f,w,fs,jtinplace,)
#define CALL2COMMON(f,a,w,fs,j,pop)   ({A carg1=(a), carg2=(w), carg3=(A)(fs), cargz; cargz=(f)(j,carg1,carg2,carg3); pop if(unlikely(cargz==0)){if(AT(carg3)!=0)jteformat(j,carg3,carg1,carg2,0);} cargz;})
#define CALL2(f,a,w,fs)   CALL2COMMON(f,a,w,fs,jt,)
#define CALL2IP(f,a,w,fs)   CALL2COMMON(f,a,w,fs,jtinplace,)
// obsolete #define CALL1(f,w,fs)   ((f)(jt,    (w),(A)(fs),(A)(fs)))
// obsolete #define CALL2(f,a,w,fs) ((f)(jt,(a),(w),(A)(fs)))
// obsolete #define CALL1IP(f,w,fs)   ({A carg1=(w), carg3=(A)(fs), cargz; cargz=(f)(jtinplace,carg1,carg3,carg3); if(unlikely(cargz==0)){if(AT(carg3)!=0)jteformat(jtinplace,carg3,carg1,0,0);} cargz;})
// obsolete // obsolete #define CALL2IP(f,a,w,fs) ((f)(jtinplace,(a),(w),(A)(fs)))
// obsolete #define CALL2IP(f,a,w,fs)   ({A carg1=(a), carg2=(w), carg3=(A)(fs), cargz; cargz=(f)(jtinplace,carg1,carg2,carg3); if(unlikely(cargz==0)){if(AT(carg3)!=0)jteformat(jtinplace,carg3,carg1,carg2,0);} cargz;})
#define RETARG(z)       (z)   // These places were ca(z) in the original JE
#define CALLSTACKRESET(jm)  {jm->callstacknext=0; jm->uflags.bstkreqd = 0;} // establish initial conditions for things that might not get processed off the stack.  The last things stacked may never be popped
#define MODESRESET(jm)      {jm->xmode=XMEXACT;}  // anything that might get left in a bad state and should be reset on return to immediate mode
// see if a character matches one of many.  Example in ai.c
// create mask for the bit, if any, in word w for value.  Reverse order: 0=MSB
#define CCM(w,value) ((I)(((value)>>LGBW)==(w)?1LL<<(BW-1-((value)&BW-1)):0))
// user creates #define nm##values(w) CCM(w,value0)+CCM(w,value1)+...
// create 8 mask values, nm##0 to nm##7, for the given values.
#define CCMWD(nm,wd) I nm##wd=nm##values(wd);
#define CCMWDS(nm) CCMWD(nm,0); CCMWD(nm,1); CCMWD(nm,2); CCMWD(nm,3); CCMWD(nm,4); CCMWD(nm,5); CCMWD(nm,6); CCMWD(nm,7);
// Create the comparand for the test value, it the given cand name
#define  CCMCAND1(nm,cand,tval,w) cand=nm##w&&((UI)tval>>LGBW)==w?nm##w:cand;
#define CCMCAND(nm,cand,tval) I cand=0; CCMCAND1(nm,cand,tval,0); CCMCAND1(nm,cand,tval,1); CCMCAND1(nm,cand,tval,2); CCMCAND1(nm,cand,tval,3); CCMCAND1(nm,cand,tval,4); CCMCAND1(nm,cand,tval,5); CCMCAND1(nm,cand,tval,6); CCMCAND1(nm,cand,tval,7)
// set the sign bit to the selected bit of the mask
#define CCMSGN(cand,tval) (cand<<(tval&(BW-1)))   // set sign bit if value found
#define CCMTST(cand,tval) (cand&(1LL<<(~tval&(BW-1))))  // test true is value found
#define DF1(f)          A f(JJ jt,    A w,A self)
#define DF2(f)          A f(JJ jt,A a,A w,A self)
#define DO(n,stm...)          {I i=0,_n=(n); for(;i<_n;i++){stm}}  // i runs from 0 to n-1
#define DONOUNROLL(n,stm...)  {I i=0,_n=(n); NOUNROLL for(;i<_n;i++){stm}}  // i runs from 0 to n-1
#define DP(n,stm...)          {I i=-(n);    for(;i<0;++i){stm}}   // i runs from -n to -1 (faster than DO)
#define DPNOUNROLL(n,stm...)  {I i=-(n);   NOUNROLL for(;i<0;++i){stm}}   // i runs from -n to -1 (faster than DO)
#define DQ(n,stm...)          {I i=(I)(n)-1;    for(;i>=0;--i){stm}}   // i runs from n-1 downto 0 (fastest when you don't need i)
#define DQNOUNROLL(n,stm...)  {UI i=(n); if((I)i>0){--i; NOUNROLL do{stm}while(i--);}}  // i runs from n-1 downto 0 (fastest when you don't need i).  i is UI
#define DOU(n,stm...)         {I i=0,_n=(n); do{stm}while(++i<_n);}  // i runs from 0 to n-1, always at least once
#define DPU(n,stm...)         {I i=-(n);    do{stm}while(++i<0);}   // i runs from -n to -1 (faster than DO), always at least once
#define DQU(n,stm...)         {I i=(I)(n)-1;  do{stm}while(--i>=0);}  // i runs from n-1 downto 0, always at least once
#define DOSTEP(n,step,stm...) {I i=0,_n=(n); for(;_n;i++,_n-=(step)){stm}}  // i runs from 0 to n-1, but _n counts down

// C suffix indicates that the count is one's complement
#define DOC(n,stm...)    {I i=0,_n=~(n); for(;i<_n;i++){stm}}  // i runs from 0 to n-1
#define DPC(n,stm...)    {I i=(n)+1;    for(;i<0;++i){stm}}   // i runs from -n to -1 (faster than DO)
#define DQC(n,stm...)    {I i=-2-(I)(n);    for(;i>=0;--i){stm}}  // i runs from n-1 downto 0 (fastest when you don't need i)
#define DOUC(n,stm...)   {I i=0,_n=~(n); do{stm}while(++i<_n);}  // i runs from 0 to n-1, always at least once
#define DPUC(n,stm...)   {I i=(n)+1;    do{stm}while(++i<0);}   // i runs from -n to -1 (faster than DO), always at least once
#define DQUC(n,stm...)   {I i=-2-(I)(n);  do{stm}while(--i>=0);}  // i runs from n-1 downto 0, always at least once
#define ds(c)            (A)&primtab[(UC)(c)]
#define DUMMYSELF        ds(CDUMMY)  // harmless value to use for self in calls to rank loops
#define NOEMSGSELF       DUMMYSELF  // harmless value to use for self - no eformat
// see if value of x is the atom v.  Do INT/B01/FL here, subroutine for exotic cases
#define EQINTATOM(x,v)  ( (AR(x)==0) && ((AT(x)&(INT+B01)) ? (((*IAV0(x))&(((AT(x)&B01)<<8)-1))==(v)) : (AT(x)&FL) ? *DAV0(x)==(D)(v) : 0!=equ(num(v),x))  )
// define fs block used in every/every2.  It is the self for the f in f&.>, and contains only function pointers, an optional param in AK, and the flag field
#define EVERYFS(name,f0,f1,akparm,flg) PRIM name={{akparm,0,0,0,0,0,0},{.primvb={.valencefns={f0,f1},.flag=flg}}};

#if USECSTACK  // obsolete, always
#define FDEPDEC(d)
#define FDEPINC(d)
#define STACKCHKOFL {D stackpos; ASSERT((uintptr_t)&stackpos>=jt->cstackmin,EVSTACK);}
#define STACKCHKOFLSUFF(suff) {D stackpos; ASSERTSUFF((uintptr_t)&stackpos>=jt->cstackmin,EVSTACK,suff);}
#else  // old style counting J recursion levels
#define FDEPDEC(d)      jt->fdepi-=(I4)(d)  // can be used in conditional expressions
#define FDEPINC(d)      {ASSERT(jt->fdepn>=jt->fdepi+(I4)(d),EVSTACK); jt->fdepi+=(I4)(d);}
#define STACKVERIFY
#endif
#define FCONS(x)        fdef(0,CFCONS,VERB,jtnum1,jtnum2,0L,0L,(x),VJTFLGOK1+VIRS1+VASGSAFE, RMAX,RMAX,RMAX)  // used for _9: to 9:
// fuzzy-equal is used for tolerant comparisons not related to jt->cct; for example testing whether x in x { y is an integer
#define FUZZ            0.000000000000056843418860808015   // tolerance
// FEQ/FIEQ are used in bcvt, where FUZZ may be set to 0 to ensure only exact values are demoted to lower precision
#define FEQ(u,v,fuzz)    (ABS((u)-(v))<=fuzz*MAX(ABS(u),ABS(v)))
#define FIEQ(u,v,fuzz)   (ABS((u)-(v))<=fuzz*ABS(v))  // used when v is known to be exact integer.  It's close enough, maybe ULP too small on the high end
// FFEQ/FFIEQ (fixed fuzz) are used where we know for sure the test should be tolerant
#define FFEQ(u,v)        (ABS((u)-(v))<=FUZZ*MAX(ABS(u),ABS(v)))
#define FFIEQ(u,v)       (ABS((u)-(v))<=FUZZ*ABS(v))  // used when v is known to be exact integer.  It's close enough, maybe ULP too small on the high end
// see if i is close enough to f that it can be used in place of f without loss of significance.  i is round(f).
#if SY_64
#define ISFTOIOK(f,i)    (ABS(f)<-(D)IMIN && ((f)==(i) || FFIEQ(f,i)))  // 64 bit: a float of IMIN does not equal integer IMIN
#else
#define ISFTOIOK(f,i)    ((f)==(I)(i) || (ABS(f)<-(D)IMIN && FFIEQ(f,i)))  // 32 bit: float IMIN is exactly integer IMIN
#endif
#define ISFTOIOKFZ(f,i,fuzz) (ABS(f)<-(D)IMIN && ((f)==(i) || FIEQ(f,i,fuzz))) // same, but variable fuzz
#define F1(f)           A f(JJ jt,    A w)  // whether in an interface routine or not, these must use the internal parameter type
#define F2(f)           A f(JJ jt,A a,A w)
#define FPREF           
#define F1PREF          FPREF
#define F2PREF          FPREF
#define FPREFIP(T)         T jtinplace=jt; jt=(T)(intptr_t)((I)jt&~JTFLAGMSK)  // turn off all flag bits in jt, leave them in jtinplace
#define F1PREFIP        FPREFIP(J)
#define F2PREFIP        FPREFIP(J)
#define F1PREFJT        FPREFIP(J)  // for doc purposes, use when the JT flags are not for inplacing
#define F2PREFJT        FPREFIP(J)
#define F1RANK(m,f,self)    {ARGCHK1(w); if(unlikely(m<AR(w)))if(m==0)R rank1ex0(w,(A)self,f);else R rank1ex(  w,(A)self,(I)m,     f);}  // if there is more than one cell, run rank1ex on them.  m=monad rank, f=function to call for monad cell.  Fall through otherwise
#define F2RANKcommon(l,r,f,self,extra)  {ARGCHK2(a,w); extra if(unlikely((I)((l-AR(a))|(r-AR(w)))<0))if((l|r)==0)R rank2ex0(a,w,(A)self,f);else{I lr=MIN((I)l,AR(a)); I rr=MIN((I)r,AR(w)); R rank2ex(a,w,(A)self,lr,rr,lr,rr,f);}}  // If there is more than one cell, run rank2ex on them.  l,r=dyad ranks, f=function to call for dyad cell
#define F2RANK(l,r,f,self)  F2RANKcommon(l,r,f,self,)
// same, but used when the function may pull an address from w.  In that case, we have to turn pristine off since there may be duplicates in the result
#define F2RANKW(l,r,f,self) F2RANKcommon(l,r,f,self,PRISTCLR(w))
#define F1RANKIP(m,f,self)    {RZ(   w); if(unlikely(m<AR(w)))R jtrank1ex(jtinplpace,  w,(A)self,(I)m,     f);}  // if there is more than one cell, run rank1ex on them.  m=monad rank, f=function to call for monad cell
#define F2RANKIP(l,r,f,self)  {ARGCHK2(a,w); if(unlikely((I)((l-AR(a))|(r-AR(w)))<0)){I lr=MIN((I)l,AR(a)); I rr=MIN((I)r,AR(w)); R jtrank2ex(jtinplace,a,w,(A)self,REX2R(lr,rr,lr,rr),f);}}  // If there is more than one cell, run rank2ex on them.  l,r=dyad ranks, f=function to call for dyad cell
// fork, including simple variants and NVV
#define PTRSNE(a,b) ((((I)(a)^(I)(b))&~(JTINPLACEW|JTINPLACEA))!=0)  // pointers don't match, ignoring low 2 bits
#define PTR(a) ((A)((I)(a)&~(JTINPLACEW|JTINPLACEA)))  // base of a pointer that might have flag bits
#define PTROP(p,op,val) ((A)((I)(p) op (val)))
#define JPTROP(p,op,val) ((J)((I)(p) op (val)))

// opt is a bitmask of variants
// bit0=h is ][
// bit4=f is ][ bit5=NVV/@:/&: bit6=f not executed
// bit8 is 1 when h is not given (@: &: hook)
// hook is 110
// @: &: is 160
#define RZEFCALL(resval,call,callself,calla,callw) {if(unlikely((resval=call)==0))R jteformat(jt,callself,calla,callw,0);}  // resval=call, as a format point since we have self

#define FORK1(name,opt) \
DF1(name){F1PREFIP;PROLOG(0000); PUSHZOMB; ARGCHK1D(w) \
AF fghfn; A fs, gs, hs; \
if(opt&0x100){ \
 /* h is omitted.  Fetch from g and f, and ignore @][ for hs purposes.  hfn is dyad for @: only */ \
 hs=FAV(self)->fgh[1]; fghfn=FAVV(hs)->valencefns[0]; \
 gs=FAV(self)->fgh[0]; \
}else{ \
 /* h is given, fetch for fork. */ \
 hs=FAV(self)->fgh[2]; fghfn=FAVV(hs)->valencefns[0];  \
 gs=FAV(self)->fgh[1]; \
 if(!(opt&0x70)){fs=FAV(self)->fgh[0];} \
} \
A *tpopw=AZAPLOC(w); tpopw=(A*)((I)tpopw&REPSGN(SGNIF(jtinplace,JTINPLACEWX)&AC(w)&((AFLAG(w)&(AFVIRTUAL|AFUNINCORPABLE))-1))); tpopw=tpopw?tpopw:ZAPLOC0;  /* point to pointer to w (if it is inplace) */ \
w = PTROP(w,+,(I)jtinplace&JTINPLACEW); /* if w inplaceable, change the pointer to fail compares below */ \
/* the call to h is not inplaceable, but it may allow WILLOPEN and USESITEMCOUNT (from the apropriate valence of g).  Inplace h if f is x@] */ \
A hx; \
if(opt&0x1){hx=w; \
}else{J jtf; \
 I wof = (FAV(gs)->flag2>>((opt&0x40?VF2WILLOPEN1X:VF2WILLOPEN2WX)-VF2WILLOPEN1X)) + ((((I)jtinplace)>>1)&VF2WILLOPEN1PROP);  /* shift all willopen flags into position, carry PROP into WILLOPEN if incoming WILLOPEN */ \
 jtf=JPTROP(jt,+,REPSGN(SGNIF(FAV(hs)->flag,VJTFLGOK1X)) & (((I)w&(opt>>5)&1) + (wof & VF2WILLOPEN1+VF2USESITEMCOUNT1)));  \
 RZEFCALL(hx,(fghfn)(jtf,PTR(w),hs,hs),hs,PTR(w),0); \
 hx=PTROP(hx,+,(I)(hx!=w)*JTINPLACEW);  /* result is inplaceable unless it equals noninplaceable input */ \
 ARGCHK1D(hx) \
} \
/* the call to f is inplaceable if the caller allowed inplacing, and f is inplaceable; but not for an arg equal to hx (which is in use for g).  Both flags in jtinplace are used */ \
A fx; \
if(!(opt&0x40)){  /* f produces a result */ \
 if(opt&0x20){fx=FAV(self)->fgh[0];  /* NVV - never inplaceable */ \
 }else if(opt&0x10){fx=PTR(w); hx=PTROP(hx,+,((I)w&JTINPLACEW)<<JTINPLACEAX); \
 }else{J jtf; \
  fghfn=FAVV(fs)->valencefns[0]; \
  I wof = (FAV(gs)->flag2>>(VF2WILLOPEN2AX-VF2WILLOPEN1X)) + ((((I)jtinplace)>>1)&VF2WILLOPEN1PROP);  /* all willopen flags, carry PROP into WILLOPEN if incoming WILLOPEN */ \
  jtf=JPTROP(jt,+,REPSGN(SGNIF(FAV(fs)->flag,VJTFLGOK1X)) & (((I)w&(JTINPLACEW*(I)PTRSNE(hx,w))) + (wof & VF2WILLOPEN1+VF2USESITEMCOUNT1))); /* install inplace & willopen flags */\
  RZEFCALL(fx,(fghfn)(jtf,PTR(w),fs,fs),fs,PTR(w),0); \
  hx=PTROP(hx,+,(I)(fx!=w)*JTINPLACEA);  /* result is inplaceable unless it equals noninplaceable input */ \
  ARGCHK2D(fx,hx) \
 } \
} \
if(opt&0x40)fghfn=FAVV(gs)->valencefns[0];else fghfn=FAVV(gs)->valencefns[1]; /* monad if no f result, else dyad */ \
/* Before executing g, free any now-unused input arguments */ \
if(!((opt&1)||((opt&0x10)&&!(opt&0x20)))) \
 /* free only if c<0.  Shift (UI)c>>1 to disable the free.  Special case: PRISTINE blocks can generate AC=8..xx where xx is anything, if the block is abandoned & picked up multiple times. */ \
 /* Check AC and zap/free only if the block can be freed; zapping otherwise would leave no way to make the usecount positive in every() */ \
 if(w=*tpopw){I c2=AC(w), c=(UI)c2>>!PTRSNE(w,hx); if(!(opt&0x40))c=(UI)c>>(w==fx); if((c&(-(AT(w)&DIRECT)|SGNIF(AFLAG(w),AFPRISTINEX)))<0){if(likely(c2==(ACINPLACE+ACUC1))){*tpopw=0; fanapop(w,AFLAG(w));}}} \
/* The call to g is inplaceable if g allows it, UNLESS fx or hx is the same as disallowed y (passed in the hx value here).  Pass in WILLOPEN from the input */ \
/* If any result equals protw/prota, it must not be inplaced: if original w/a is inplaceable, protw/prota will not match anything */ \
/* pass flags from the next prim from the input flags */ \
POPZOMB; A z; \
if(opt&0x40){ \
 RZEFCALL(z,(fghfn)(JPTROP(JPTROP(JPTROP(jtinplace,&,(~(JTINPLACEW))),|,((I)hx&(JTINPLACEW))),&,(REPSGN(SGNIF(FAV(gs)->flag,VJTFLGOK1X))|~JTFLAGMSK)),PTR(hx),gs,gs),gs,PTR(hx),0); \
}else{ \
 RZEFCALL(z,(fghfn)(JPTROP(JPTROP(JPTROP(jtinplace,&,(~(JTINPLACEA+JTINPLACEW))),|,((I)hx&(JTINPLACEW|JTINPLACEA))),&,(REPSGN(SGNIF(FAV(gs)->flag,VJTFLGOK2X))|~JTFLAGMSK)),fx,PTR(hx),gs),gs,fx,PTR(hx)); \
} \
/* EPILOG to free up oddments from f/g/h, but not if we may be returning a virtual block */ \
if(likely(!((I)jtinplace&JTWILLBEOPENED)))z=EPILOGNORET(z); \
RETF(z); \
}

// opt is a bitmask of variants
// bit0=h is [ bit1=h is ] bit2=h is @[ bit3=h is @]
// bit4=f is [ bit5=f is ] bit6=f is @[ bit7=f is @]  (bits 4-7=0011 for NVV, 1100 when f is absent)
// bit8 is 1 when h is not given (@: &: hook) - h flags may be 0 or 8
// bit12 is set to analyze all components fully (forks only - for reversion paths)
// hook is 118
// @: is 1c0
// &: is 148
#define FORK2(name,opt) \
DF2(name){F2PREFIP;PROLOG(0000); PUSHZOMB; ARGCHK2D(a,w) \
AF fghfn; A fs, gs, hs; \
if(opt&0x100){ \
 /* h is omitted.  Fetch from g and f, and ignore @][ for hs purposes.  hfn is dyad for @: only */ \
 fs=hs=FAV(self)->fgh[1]; fghfn=FAVV(hs)->valencefns[!(opt&0x8)]; \
 gs=FAV(self)->fgh[0]; \
}else{ \
 /* h is given, fetch for fork.  hfn is in localuse */ \
 if(!(opt&0x1000))fghfn=FAVV(self)->localuse.lu1.fork2hfn; \
 fs=FAV(self)->fgh[0]; if((opt&0x30)!=0x30){if(!(opt&0x1000&&AT(fs)&NOUN)){if(opt&0xc0){fs=FAV(fs)->fgh[0];}}} \
 hs=FAV(self)->fgh[2]; if(opt&0xc)hs=FAV(hs)->fgh[0]; /* honor h@][ for hs purposes */ \
 gs=FAV(self)->fgh[1]; \
 if(opt&0x1000)fghfn=FAVV(hs)->valencefns[1]; \
} \
A *tpopw=AZAPLOC(w); tpopw=(A*)((I)tpopw&REPSGN(SGNIF(jtinplace,JTINPLACEWX)&AC(w)&((AFLAG(w)&(AFVIRTUAL|AFUNINCORPABLE))-1))); tpopw=tpopw?tpopw:ZAPLOC0;  /* point to pointer to w (if it is inplace) */ \
A *tpopa=AZAPLOC(a); tpopa=(A*)((I)tpopa&REPSGN(SGNIF(jtinplace,JTINPLACEAX)&AC(a)&((AFLAG(a)&(AFVIRTUAL|AFUNINCORPABLE))-1))); tpopa=tpopa?tpopa:ZAPLOC0;  /* point to pointer to a (if it is inplace) */ \
w = PTROP(w,+,(I)jtinplace&JTINPLACEW); a = PTROP(a,+,(I)jtinplace&JTINPLACEA);  /* if arg inplaceable, change the pointer to fail compares below */\
/* the call to h is not inplaceable, but it may allow WILLOPEN and USESITEMCOUNT.  Inplace h if f is x@], but not if a==w  Actually we turn off all flags here if a==w, for comp ease */ \
A hx; \
if(opt&0x2){hx=w; \
}else if(opt&0x1){hx=PTROP(a,-,((I)a>>JTINPLACEAX)&(JTINPLACEA>>JTINPLACEAX)); \
}else{J jtf; \
 I wof = (FAV(gs)->flag2>>(((opt&0xc0)==0xc0?VF2WILLOPEN1X:VF2WILLOPEN2WX)-VF2WILLOPEN1X)) + ((((I)jtinplace)>>1)&VF2WILLOPEN1PROP);  /* shift all willopen flags into position, propagate willopen */ \
 if(opt&0xc){ \
  /* h@][.  Don't allow inplacing if a=w, because f will need the value for sure then.  Exception: NVV, where f needs nothing */ \
  /* bits 4-5=f is [] per se, 6-7=@[], so OR means 'f ignores RL'.  Bits 2-3=h is @[] so ~bits 2-3 10=@], 01=@[ (only choices) which mean 'h uses RL' - inplace if h uses an arg f ignores  */ \
  /* the flags in gh@][ are passed through from gh */ \
  jtf=JPTROP(jt,+,(-((FAV(hs)->flag>>VJTFLGOK1X)&((I)(PTRSNE(a,w)|((opt&0x30)==0x30))))) & (((((I)(opt&0x4?a:w))&(((opt>>4)|(opt>>6))&~(opt>>2)&3))!=0) + (wof & VF2WILLOPEN1+VF2USESITEMCOUNT1))); \
  RZEFCALL(hx,(fghfn)(jtf,PTR(opt&0x4?a:w),hs,hs),hs,PTR(opt&0x4?a:w),0); \
  hx=PTROP(hx,+,(I)(hx!=(opt&0x4?a:w))*JTINPLACEW);  /* result is inplaceable unless it equals noninplaceable input */ \
 }else{ \
  jtf=JPTROP(jt,+,(-((FAV(hs)->flag>>VJTFLGOK2X)&((I)(PTRSNE(a,w)|((opt&0xc0)==0xc0))))) & ((((I)a|(I)w)&(((opt>>4)|(opt>>6))&3)) + (wof & VF2WILLOPEN1+VF2USESITEMCOUNT1))); \
  RZEFCALL(hx,(fghfn)(jtf,PTR(a),PTR(w),hs),hs,PTR(a),PTR(w)); \
  hx=PTROP(hx,+,(I)((hx!=w)&(hx!=a))*JTINPLACEW);  /* result is inplaceable unless it equals noninplaceable input */ \
 } \
 ARGCHK1D(hx) \
} \
/* the call to f is inplaceable if the caller allowed inplacing, and f is inplaceable; but not for an arg equal to hx (which is in use for g).  Both flags in jtinplace are used */ \
A fx; \
if((opt&0xc0)!=0xc0){ /* if we are running f */ \
 if((opt&0x30)==0x30){fx=fs;  /* NVV - never inplaceable */ \
 }else if(opt&0x1000&&AT(fs)&NOUN){fx=fs;  /* NVV detected from arguments */ \
 }else if(opt&0x20){fx=PTR(w); hx=PTROP(hx,+,((I)w&JTINPLACEW)<<JTINPLACEAX); \
 }else if(opt&0x10){fx=PTR(a); hx=PTROP(hx,+,((I)a&JTINPLACEA)); \
 }else{J jtf; \
  if(!(opt&0x100)){if(opt&0xc0){fghfn=FAVV(fs)->valencefns[0];}else{fghfn=FAVV(fs)->valencefns[1];}}  /* if not fork, leave f=h */ \
  I wof = (FAV(gs)->flag2>>(VF2WILLOPEN2AX-VF2WILLOPEN1X)) + ((((I)jtinplace)>>1)&VF2WILLOPEN1PROP);  /* all willopen flags, propagate willopen */ \
  if(opt&0xc0){ \
   /* f@][.  Before we execute, free the argument we don't need (unless it equals the other argument or hx) */ \
   jtf=JPTROP(jt,+,REPSGN(SGNIF(FAV(fs)->flag,VJTFLGOK1X)) & ((opt&0x40?((I)a>>JTINPLACEAX)&(I)PTRSNE(hx,a):((I)w>>JTINPLACEWX)&(I)PTRSNE(hx,w)) + (wof & VF2WILLOPEN1+VF2USESITEMCOUNT1))); \
   /* free only if c<0.  Shift (UI)c>>1 to disable the free.  Special case: PRISTINE blocks can generate AC=8..xx where xx is anything, if the block is abandoned & picked up multiple times.  So check AC */ \
   /* Check AC and zap/free only if the block can be freed; zapping otherwise would leave no way to make the usecount positive in every() */ \
   if(opt&0x40){if(w=*tpopw){I c2=AC(w), c=(UI)c2>>!PTRSNE(w,hx); c=(UI)c>>(tpopa==tpopw); if((c&(-(AT(w)&DIRECT)|SGNIF(AFLAG(w),AFPRISTINEX)))<0){if(likely(c==(ACINPLACE+ACUC1))){*tpopw=0; fanapop(w,AFLAG(w));}}}} \
   else{if(a=*tpopa){I c2=AC(a), c=(UI)c2>>!PTRSNE(a,hx); c=(UI)c>>(tpopa==tpopw); if((c&(-(AT(a)&DIRECT)|SGNIF(AFLAG(a),AFPRISTINEX)))<0){if(likely(c==(ACINPLACE+ACUC1))){*tpopa=0; fanapop(a,AFLAG(a));}}}} \
   RZEFCALL(fx,(fghfn)(jtf,PTR(opt&0x40?a:w),fs,fs),fs,PTR(opt&0x40?a:w),0); \
   hx=PTROP(hx,+,((I)(fx!=(opt&0x40?a:w)))*JTINPLACEA);  /* result is inplaceable unless it equals noninplaceable input */ \
  }else{ \
   jtf=JPTROP(jt,+,REPSGN(SGNIF(FAV(fs)->flag,VJTFLGOK2X)) & ((((I)a|(I)w)&(JTINPLACEA*(I)PTRSNE(hx,a)+JTINPLACEW*(I)PTRSNE(hx,w))) + (wof & VF2WILLOPEN1+VF2USESITEMCOUNT1))); \
   RZEFCALL(fx,(fghfn)(jtf,PTR(a),PTR(w),fs),fs,PTR(a),PTR(w)); \
   hx=PTROP(hx,+,((I)((fx!=w)&(fx!=a)))*JTINPLACEA);  /* result is inplaceable unless it equals noninplaceable input */ \
  } \
  ARGCHK2D(fx,hx) \
 } \
} \
if((opt&0xc0)==0xc0)fghfn=FAVV(gs)->valencefns[0];else fghfn=FAVV(gs)->valencefns[1];  /* dyad for g unless f is suppressed */  \
/* Before executing g, free any now-unused input arguments */ \
if(!((opt&2)||((opt&0x20)&&!(opt&0x10)))) \
if(w=*tpopw){I c2=AC(w), c=(UI)c2>>!PTRSNE(w,hx); if((opt&0xc0)!=0xc0&&(opt&0x30)!=0x30)c=(UI)c>>(w==fx); if((c&(-(AT(w)&DIRECT)|SGNIF(AFLAG(w),AFPRISTINEX)))<0){if(likely(c2==(ACINPLACE+ACUC1))){*tpopw=0; fanapop(w,AFLAG(w));}}} \
if(!((opt&1)||((opt&0x10)&&!(opt&0x20)))) \
if(a=*tpopa){I c2=AC(a), c=(UI)c2>>!PTRSNE(a,hx); if((opt&0xc0)!=0xc0&&(opt&0x30)!=0x30)c=(UI)c>>(a==fx); if((c&(-(AT(a)&DIRECT)|SGNIF(AFLAG(a),AFPRISTINEX)))<0){if(likely(c2==(ACINPLACE+ACUC1))){*tpopa=0; fanapop(a,AFLAG(a));}}} \
/* The call to g is inplaceable if g allows it, UNLESS fx or hx is the same as disallowed y (passed in the hx value here).  Pass in WILLOPEN from the input */ \
/* If any result equals protw/prota, it must not be inplaced: if original w/a is inplaceable, protw/prota will not match anything */ \
/* pass flags from the next prim from the input flags */ \
POPZOMB; A z; \
if((opt&0xc0)==0xc0){ \
 RZEFCALL(z,(fghfn)(JPTROP(JPTROP(JPTROP(jtinplace,&,(~(JTINPLACEW))),|,((I)hx&(JTINPLACEW))),&,(REPSGN(SGNIF(FAV(gs)->flag,VJTFLGOK1X))|~JTFLAGMSK)),PTR(hx),gs,gs),gs,PTR(hx),0); \
}else{ \
 RZEFCALL(z,(fghfn)(JPTROP(JPTROP(JPTROP(jtinplace,&,(~(JTINPLACEA+JTINPLACEW))),|,((I)hx&(JTINPLACEW|JTINPLACEA))),&,(REPSGN(SGNIF(FAV(gs)->flag,VJTFLGOK2X))|~JTFLAGMSK)),fx,PTR(hx),gs),gs,fx,PTR(hx)); \
} \
/* EPILOG to free up oddments from f/g/h, but not if we may be returning a virtual block */ \
if(likely(!((I)jtinplace&JTWILLBEOPENED)))z=EPILOGNORET(z); RETF(z); \
}

// get # of things of size s, rank r to allocate so as to have an odd number of them at least n, after discarding w items of waste.  Try to fill up a full buffer 
#define FULLHASHSIZE(n,s,r,w,z) {UI4 zzz;  CTLZI((((n)|1)+(w))*(s) + AKXR(r) - 1,zzz); z = ((((I)1<<(zzz+1)) - AKXR(r)) / (s) - 1) | (1&~(w)); }
// Memory-allocation macros
#if MEMHISTO   // create histogram of allocation calls
#define HISTOCALL memhashadd(__LINE__,__FILE__);
#else
#define HISTOCALL
#endif
// Size-of-block calculations.  VSZ when size is constant or variable.  Byte counts are (total length including header)-1
// Because the Boolean dyads read beyond the end of the byte area (up to 1 extra word), we add one SZI-1 for islast (which includes B01), rather than adding 1
// NOTE: sizeof(NM) is rounded up to a word multiple; offsetof() would be better.  But since it overallocates only for names of >20 characters, we keep it as is
// NOTE: we overfetch from all blocks we allocate.  We assume that there is at least 32 bytes of fetchable data at the end of the block.
//  * for AVX loops, we use maskload/maskstore.  We do not modify anything past the valid area but we need them to be mapped to avoid microprogram check
//  * for boolean operations, we are entitled to fetch an I from the start of any valid atom.  When creating result blocks we may write as much as an I into the address of any valid atom
//  * for singleton operations, we fetch a D from the address of the argument; it must be mapped
// to guarantee validity we allocate a tail area for everything we allocate, and insist that mapped files do the same.
#define ALLOBYTESVSZ(atoms,rank,size,islast,isname)      ( ((((rank)|(!SY_64))*SZI  + ((islast)? (isname)?(NORMAH*SZI+sizeof(NM)+SZI-1-1):(NORMAH*SZI+SZI-1-1) : (NORMAH*SZI-1)) + (atoms)*(size)))  )  // # bytes to allocate allowing 1 I for string pad - include mem hdr - minus 1
#define ALLOBYTESVSZLG(atoms,rank,sizelg,islast,isname)      ( ((((rank)|(!SY_64))*SZI  + ((islast)? (isname)?(NORMAH*SZI+sizeof(NM)+SZI-1-1):(NORMAH*SZI+SZI-1-1) : (NORMAH*SZI-1)) + ((atoms)<<(sizelg))))  )  // this version when we have power-of-2 size
// here when size is constant.  The number of bytes, rounded up with overhead added, must not exceed 2^(PMINL+5)
#define ALLOBYTES(atoms,rank,size,islast,isname)      ((size&(SZI-1))?ALLOBYTESVSZ(atoms,rank,size,islast,isname):(SZI*(((rank)|(!SY_64))+NORMAH+((size)>>LGSZI)*(atoms)+!!(islast))-1))  // # bytes to allocate-1
#define ALLOBLOCK(n) ((n)<2*PMIN?((n)<PMIN?PMINL-1:PMINL) : (n)<8*PMIN?((n)<4*PMIN?PMINL+1:PMINL+2) : (n)<32*PMIN?((n)<16*PMIN?PMINL+3:PMINL+4) : *(volatile I*)0)   // lg2(#bytes to allocate)-1.  n is #bytes-1
// gives errors in some versions #define ALLOBLOCK(n) MAX(PMINL-1,(31-__builtin_clzl((UI4)(n))))    // lg2(#bytes to allocate)-1.  n is #bytes-1
// value to put into name->bucketx for locale names: number if numeric, hash otherwise
#define BUCKETXLOC(len,s) ((*(s)<='9')?strtoI10s((len),(s)):(I)nmhash((len),(s)))
// Support for int-to-float, in parallel.  Input is u, 64-bit int with a type of float; result is 64-bit floats.  Define DECLS first.
// we use initecho() to initialize zero and oned because the compiler moves the initialization to inside the loop
#define CVTEPI64DECLS  __m256i magic_i_lo = _mm256_castpd_si256(_mm256_broadcast_sd(&two_52)); /* 2^52 */ \
      __m256i magic_i_hi32 = _mm256_castpd_si256(_mm256_broadcast_sd(&two_84_63)); /* 2^84+2^63 */  \
      __m256i magic_i_all  = _mm256_castpd_si256(_mm256_broadcast_sd(&two_84_63_52)); /* 2^84 + 2^63 + 2^52 */ \
      __m256d zero=_mm256_broadcast_sd(&zone.imag); __m256d oned=_mm256_broadcast_sd(&zone.real);  __m256d onei=_mm256_broadcast_sd((const double *)&oneone[0]);    \
      __m256i dts=_mm256_castpd_si256(_mm256_loadu_pd((D*)disttosign));
#if C_AVX512
#define CVTEPI64(z,u) z=_mm256_cvtepi64_pd(_mm256_castpd_si256(u));
#elif defined(__aarch64__)
#define  CVTEPI64(z,u)   z.vect_f64[0] = vcvtq_f64_s64(vreinterpretq_s64_f64(u.vect_f64[0])); \
                         z.vect_f64[1] = vcvtq_f64_s64(vreinterpretq_s64_f64(u.vect_f64[1]));
#else

#define  CVTEPI64(z,u) { __m256i u_lo = _mm256_castps_si256(_mm256_blend_ps(_mm256_castsi256_ps(magic_i_lo),_mm256_castpd_ps(u),0b01010101));         /* Blend the 32 lowest significant bits of u with magic_int_lo */ \
                        __m256i u_hi = _mm256_srli_epi64(_mm256_castpd_si256(u), 32);     /* Extract the 32 most significant bits of u */ \
                          u_hi = _mm256_xor_si256(u_hi, magic_i_hi32); /* Flip the msb of u_hi and blend with 0x45300000 */ \
                        __m256d u_hi_dbl = _mm256_sub_pd(_mm256_castsi256_pd(u_hi), _mm256_castsi256_pd(magic_i_all)); /* Compute in double precision:  */ \
                         z = _mm256_add_pd(u_hi_dbl, _mm256_castsi256_pd(u_lo));}  /* (u_hi - magic_d_all) + u_lo  Do not assume associativity of floating point addition !! */
#endif
// # turns through a Duff loop of m1+1 elements, with 1<<lgduff instances in the loop.  We assume we are handling [1,NPAR] elements at the end
#define DUFFLPCTV(m1,lgduff,lgeleperiter) ((((m1)+((((I)1<<(lgduff))-1)<<(lgeleperiter)))>>((lgeleperiter)+(lgduff))))
#define DUFFLPCT(m1,lgduff) DUFFLPCTV(m1,lgduff,LGNPAR)
// calculate the (backoff-1) in elements for the first pass through the Duff loop.  This (negative) value+1 must be added to the initial addresses
#define DUFFBACKOFFV(m1,lgduff,lgeleperiter) ((((m1)>>(lgeleperiter))-1)|-((I)1<<(lgduff)))  // 0->-1, 7->-2, 1->-8
#define DUFFBACKOFF(m1,lgduff) DUFFBACKOFFV(m1,lgduff,LGNPAR) // 0->-1, 7->-2, 1->-8
// offset by n items
#define OFFSETBID(ad,n,commute,id,bi,bd) (D*)((I)ad+((I)(n)*(((commute)&((bi)|(bd)))?1:SZD)))  // using shift gives warning on clang can't left-shift a negative constant!
// increment address by n items
#define INCRBID(ad,n,commute,id,bi,bd) ad=OFFSETBID(ad,n,commute,id,bi,bd);
// load 4 atoms.  For boolean each lane looks at a different byte
#define LDBID(z,ad,commute,id,bi,bd) {if((commute)&((bi)|(bd))){z=_mm256_broadcast_sd(ad); \
                                      if((commute)&(bi))z=_mm256_castsi256_pd(_mm256_cvtepu8_epi64(_mm256_castsi256_si128(_mm256_castpd_si256(z))));} else z=_mm256_loadu_pd(ad); }
#define LDOBID(z,ad,n,commute,id,bi,bd) {if((commute)&((bi)|(bd))){z=_mm256_broadcast_sd(OFFSETBID(ad,n,commute,id,bi,bd))); \
                                      if((commute)&(bi))z=_mm256_castsi256_pd(_mm256_cvtepu8_epi64(_mm256_castsi256_si128(_mm256_castpd_si256(z))));} else z=_mm256_loadu_pd(OFFSETBID(ad,n,commute,id,bi,bd))); }
// load 4 atoms. masking.  For boolean each lane looks at a different byte
#define LDBIDM(z,ad,commute,id,bi,bd,endmask) {if((commute)&((bi)|(bd))){z=_mm256_broadcast_sd(ad); \
                                               if((commute)&(bi))z=_mm256_castsi256_pd(_mm256_cvtepu8_epi64(_mm256_castsi256_si128(_mm256_castpd_si256(z))));} else z=_mm256_maskload_pd(ad,endmask); }
// load 1 atom into all lanes
#define LDBID1(z,ad,commute,id,bi,bd) {z=_mm256_broadcast_sd(ad);}
// convert a LDBID/LDBIDM to D or I.  Only the LSB of each B can be set.  0x400 turns -1 to 1, for multiplication
#define CVTBID(z,u,commute,id,bi,bd) \
                        { if((commute)&(id)){ CVTEPI64(z,u) \
                          }else if((commute)&(bi)){ \
                            if((commute)&0x400){z=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(onei),_mm256_castpd_si256(u)));} \
                          }else if((commute)&(bd)){ \
                            if(C_AVX2)z=_mm256_castsi256_pd(_mm256_sllv_epi64(_mm256_castpd_si256(u),dts)); \
                            else z=_mm256_castsi256_pd(_mm256_cmpeq_epi64(dts,_mm256_and_si256(dts,_mm256_castpd_si256(u)))); \
                            z=_mm256_blendv_pd(zero,oned,z); \
                        } }
// convert a LDBID1 to D or I.  z is garbage except for low byte.  0x400 turns -1 to 1, for multiplication
#define  CVTBID1(z,u,commute,id,bi,bd) \
                        { if((commute)&(id)){ CVTEPI64(z,u) \
                          }else if((commute)&(bi)){ \
                            z=_mm256_and_pd(u,onei); \
                            if((commute)&0x400){z=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(onei),_mm256_castpd_si256(z)));} \
                          }else if((commute)&(bd)){ \
                            z=_mm256_castsi256_pd(_mm256_slli_epi64(_mm256_castpd_si256(u),63)); \
                            z=_mm256_blendv_pd(zero,oned,z); \
                        } }

#define EXPLICITRUNNING (!((I)jt->locsyms&SZI))  // the null locale is put on an odd I boundary
// GA() is used when the type is unknown.  This routine is in m.c and documents the function of these macros.
// NEVER use GA() for NAME types - it doesn't honor it.
// SHAPER is used when shape is given and rank is SDT.  Usually 0/1 use COPYSHAPE0 but they can use this; it always copies from the shaape.  This works only up to rank 2 (but could be extended if needed)
#define GACOPYSHAPER(name,type,atoms,rank,shaape) if((rank)>0)AS(name)[0]=(shaape)[0]; if((rank)>1)AS(name)[1]=(shaape)[1]; if((rank)>2)AS(name)[2]=(shaape)[2];
// SHAPE0 is used when the shape is 0 - write shape only if rank==1
#define GACOPYSHAPE0(name,type,atoms,rank,shaape) if((rank)==1)AS(name)[0]=(atoms);
// Use when shape is known to be present but rank is not SDT.
#if (C_AVX&&SY_64) || EMU_AVX
#define GACOPYSHAPE(name,type,atoms,rank,shaape) MCISH(AS(name),shaape,rank)
#else
// in this version one value is always written to shape
#define GACOPYSHAPE(name,type,atoms,rank,shaape)  {I *_s=(I*)(shaape); I *_d=AS(name); *_d=*_s; I _r=1-(rank); NOUNROLL do{_s+=SGNTO0(_r); _d+=SGNTO0(_r); *_d=*_s;}while(++_r<0);}
#endif
#define GACOPY1(name,type,atoms,rank,shaape) {I *_d=AS(name); UI _r=(rank); NOUNROLL do{*_d++=1;}while(--_r);} // copy all 1s to shape - rank must not be 0

// GAE executes the given expression when there is an error
#if SY_64
#define GAE0(v,t,n,r,erraction) {HISTOCALL if(unlikely(!(v=jtga0(jt,((I)(r)<<32)+(t),(I)(n)))))erraction; AN(v)=(n);}  // used when shape=0 and rank is never 1 or will always be filled in by user even if rank 1
#else
#define GAE0(v,t,n,r,erraction) {HISTOCALL if(unlikely(!(v=jtga0(jt,(I)(t),(I)(r),(I)(n)))))erraction; AN(v)=(n);}  // used when shape=0 and rank is never 1 or will always be filled in by user even if rank 1
#endif
#define GAE(v,t,n,r,s,erraction)   {GAE0(v,t,n,r,erraction) MCISH(AS(v),(I*)(s),(r))}  // error action
#define GA00(v,t,n,r) {GAE0(v,t,n,r,R 0)}  // used when shape will always be filled in by user.  Default error action is to exit
#define GA(v,t,n,r,s)   {GA00(v,t,n,r) MCISH(AS(v),(I*)(s),(r))}   // s points to shape
#define GA0(v,t,n,r) {GA00(v,t,n,r) *((r)==1?AS(v):jt->shapesink)=(n);}  // used when shape=0 but rank may be 1 and must fill in with AN if so - never for sparse blocks
#define GA10(v,t,n) {GA00(v,t,n,1) AS(v)[0]=(n);}  // used when rank is known to be 1

// GAT[^V]*, used when the type and all rank/shape are known at compile time.  The compiler precalculates almost everything
// For best results declare name as: AD* RESTRICT name;  For GAT the number of bytes, rounded up with overhead added, must not exceed 2^(PMINL+4)
#define GATS(name,type,atoms,rank,shaape,size,shapecopier,erraction) \
{ ASSERT(!((rank)&~RMAX),EVLIMIT); \
 I bytes = ALLOBYTES(atoms,rank,size,(type)&LAST0,(type)&NAME); \
 HISTOCALL \
 name = jtgaf(jt, ALLOBLOCK(bytes)); \
 I akx=AKXR(rank);   \
 if(likely(name!=0)){   \
 AK(name)=akx; AT(name)=(type); AN(name)=atoms;   \
 ARINIT(name,rank);     \
 if(!(((type)&DIRECT))>0){if(rank==0)AS(name)[0]=0; if((bytes-(offsetof(AD,s[1])-32))&-32)mvc((bytes-(offsetof(AD,s[1])-32))&-32,&AS(name)[1],1,MEMSET00);}  \
 shapecopier(name,type,atoms,rank,shaape)   \
    \
 }else{erraction;} \
}
#define GAT(name,type,atoms,rank,shaape)  GATS(name,type,atoms,rank,shaape,type##SIZE,GACOPYSHAPE,R 0)  // shape must not be 0
#define GATR(name,type,atoms,rank,shaape)  GATS(name,type,atoms,rank,shaape,type##SIZE,GACOPYSHAPER,R 0)  // rank should be SDT
#define GAT0(name,type,atoms,rank)  GATS(name,type,atoms,rank,0,type##SIZE,GACOPYSHAPE0,R 0)  // shape is 0 pointer (i. e. no shape), or #atoms if rank is 1
#define GAT0E(name,type,atoms,rank,erraction)  GATS(name,type,atoms,rank,0,type##SIZE,GACOPYSHAPE0,erraction)  // shape is 0, with error branch

// GATV*, used when type is known and something else is variable.  ##SIZE must be applied before type is substituted, so we have GATVS to use inside other macros.  Normally use GATV
// Note: assigns name before assigning the components of the array, so the components had better not depend on name, i. e. no GATV(z,BOX,AN(z),AR(z),AS(z))
#define GATVS(name,type,atoms,rank,shaape,size,shapecopier,erraction) \
{ I bytes = ALLOBYTES(atoms,rank,size,(type)&LAST0,(type)&NAME); \
 if(SY_64){ASSERT((((I)(atoms)>>(SY_64?(TOOMANYATOMSX-RANKTX):0))|(I)(rank))<=RMAX,EVLIMIT)} /* SY_64? to avoid 32-bit compile error */\
 else{ASSERT(((I)bytes>(I)(atoms)&&(I)(atoms)>=(I)0)&&!((rank)&~RMAX),EVLIMIT)} \
 HISTOCALL \
 name = jtgafv(jt, bytes);   \
 I akx=AKXR(rank);   \
 if(likely(name!=0)){   \
  AK(name)=akx; AT(name)=(type); AN(name)=atoms; ARINIT(name,rank);     \
  if(!(((type)&DIRECT)>0)){AS(name)[0]=0; mvc((bytes-(offsetof(AD,s[1])-32))&-32,&AS(name)[1],1,MEMSET00);}   /* overclears the data but never over buffer bdy */ \
  shapecopier(name,type,atoms,rank,shaape)   \
     \
 }else{erraction;} \
}

// see warnings above under GATVS
#define GATV(name,type,atoms,rank,shaape) GATVS(name,type,atoms,rank,shaape,type##SIZE,GACOPYSHAPE,R 0)  // shape must not be 0
#define GATVR(name,type,atoms,rank,shaape) GATVS(name,type,atoms,rank,shaape,type##SIZE,GACOPYSHAPER,R 0)  // rank should be SDT
#define GATV1(name,type,atoms,rank) GATVS(name,type,atoms,rank,0,type##SIZE,GACOPY1,R 0)  // this version copies 1 to the entire shape
#define GATV0(name,type,atoms,rank) GATVS(name,type,atoms,rank,0,type##SIZE,GACOPYSHAPE0,R 0)  // shape not written unless rank==1
#define GATV0E(name,type,atoms,rank,erraction) GATVS(name,type,atoms,rank,0,type##SIZE,GACOPYSHAPE0,erraction)  // shape not written unless rank==1, with error branch
// use this version when you are allocating a sparse matrix.  It handles the AS[0] field correctly.  ALL sparse allocations must come through here so that AC is set correctly
#define GASPARSE(n,t,a,r,s) {GA(n,BOX,(sizeof(P)/sizeof(A)),r,s); AN(n)=1; AT(n)=(t)|SPARSE; if((r)==1){if(s)AS(n)[0]=(s)[0];} ACINIT(n,ACUC1);}
#define GASPARSE0(n,t,a,r) {GATV0(n,BOX,(sizeof(P)/sizeof(A)),r); AN(n)=1; AT(n)=(t)|SPARSE; ACINIT(n,ACUC1);}

#define HN              4L  // number of boxes per valence to hold exp-def info (words, control words, original (opt.), symbol table)
// Item count given frame and rank: AS(f) unless r is 0; then 1 
#define SETICFR(w,f,r,targ) (targ=(I)(AS(w)+f), targ=(r)?targ:(I)&I1mem, targ=*(I*)targ)
// Shape item s, but 1 if index is < 0
#define SHAPEN(w,s,targ) (targ=AS(w)[s], targ=(s)<0?1:targ)
// Item count
#define SETIC(w,targ)   (targ=AS(w)[0], targ=AR(w)?targ:1)  //   (AR(w) ? AS(w)[0] : 1L)
#define ICMP(z,w,n)     memcmpne((z),(w),(n)*SZI)
#define ICPY(z,w,n)     memcpy((z),(w),(n)*SZI)
// compare names.  We assume the names are usually short & avoid subroutine call, which ties up registers.  Names are overfetched
#define IFCMPNAME(name,string,len,hsh,stmt) if((name)->hash==(hsh))if(likely((name)->m==(len))){ \
         if((len)<5)stmt  /*  len 5 or less, hash is enough */ \
         else{C*c0=(name)->s, *c1=(string); I lzz=(len); NOUNROLL for(;lzz;c0+=((lzz-1)&(SZI-1))+1,c1+=((lzz-1)&(SZI-1))+1,lzz=(lzz-1)&-SZI)if(((*(I*)c0^*(I*)c1)<<((-lzz&(SZI-1))<<LGBB))!=0)break; \
          if(likely(lzz==0)){stmt} \
         } \
        }

// Mark a block as incorporated by removing its inplaceability.  The blocks that are tested for incorporation are ones that are allocated by partitioning, and they will always start out as inplaceable
// If a block is virtual, it must be realized before it can be incorporated.  realized blocks always start off inplaceable and non-pristine
// z is an lvalue
// Use INCORPNA if you need to tell the caller that the block e sent you has been incorporated.  If you created the block being incorporated,
// even by calling a function that returns it, you can be OK just using rifv() or rifvs().  This may leave an incorporated block marked inplaceable,
// but that's OK as long as you don't pass it to some place where it can become an argument to another function
// When a block is incorporated it becomes not pristine, because extractions from the parent may compromise it and we don't want to have to go through recursively to find them
#define INCORPNC(z) {if(unlikely((AFLAG(z)&AFVIRTUAL)!=0)){RZ((z)=realize(z))} else{AFLAGPRISTNO(z)} }  // use if you are immediately going to change AC, as with ras()
#define INCORPNCUI(z) {if(unlikely((AFLAG(z)&AFUNINCORPABLE)!=0)){RZ((z)=realize(z))} else{AFLAGPRISTNO(z)} }  // use if OK to incorporate virtual (but never UNINCORPABLE)
#define INCORP(z) {INCORPNC(z) ACIPNO(z);}  // the normal one to use when inserting into a non-DIRECT
#define INCORPNV(z) {AFLAGPRISTNO(z) ACIPNO(z);}  // use when z is known nonvirtual
// same, but for nonassignable argument.  Must remember to check the result for 0
#define INCORPNA(z) incorp(z)
// use to incorporate into a known-recursive box.  We raise the usecount of z
#define INCORPRA(z) {INCORPNC(z) ra(z); }
// use to incorporate a newly-created zapped block of type t into a known-recursive box.  If t is recursible, raise the contents of z
#define INCORPRAZAPPED(z,t) {ACINIT(z,ACUC1) if((t)&RECURSIBLE){AFLAGINIT(z,t); jtra(z,(t),0);}}
// Tests for whether a result incorporates its argument.  The originator, who is going to check this, always marks the argument inplaceable,
// and we signal incorporation either by returning the argument itself or by marking it non-inplaceable (if we box it)
#define WASINCORP1(z,w)    ((z)==(w)||0<=AC(w))
#define WASINCORP2(z,a,w)  ((z)==(w)||(z)==(a)||0<=(AC(a)|AC(w)))
#define INF(x)          ((x)==inf||(x)==infm)
// true if a has recursive usecount
#define UCISRECUR(a)    (AFLAG(a)&RECURSIBLE)
// Install new value z into xv[k], where xv is AAV(x).  If x has recursive usecount, we must increment the usecount of z.
// This also guarantees that z has recursive usecount whenever x does, and that z is realized
#define INSTALLBOX(x,xv,k,z) rifv(z); if(likely((UCISRECUR(x))!=0)){A zzZ=xv[k]; ra(z); fa(zzZ);} xv[k]=z  // we could be reinstalling the same value, so must ra before fa
#define INSTALLBOXNF(x,xv,k,z) rifv(z); if(likely((UCISRECUR(x))!=0)){ra(z);} xv[k]=z   // Don't do the free - if we are installing into known 0 or known nonrecursive
#define INSTALLBOXNVRECUR(xv,k,z) {I zzK=(k); A zzZ=(xv)[zzK]; (xv)[zzK]=(z); ra(z); fa(zzZ);}  // z is known non-virtual.  Don't test - we know we are installing into a recursive block
#define INSTALLBOXRECUR(xv,k,z) rifv(z); INSTALLBOXNVRECUR(xv,k,z)  // Don't test - we know we are installing into a recursive block
// Same thing for RAT type.  z is a Q, xv[k] is a Q
#define INSTALLRAT(x,xv,k,z) if(likely((UCISRECUR(x))!=0)){Q zzZ=xv[k]; ra(z.n); ra(z.d); fa(zzZ.n); fa(zzZ.d);} xv[k]=z
#define INSTALLRATNF(x,xv,k,z) if(likely((UCISRECUR(x))!=0)){ra(z.n); ra(z.d);} xv[k]=z   // Don't do the free - if we are installing into known 0
#define INSTALLRATRECUR(xv,k,z) rifv(z.n); rifv(z.d); {I zzK=(k); {Q zzZ=xv[k]; ra(z.n); ra(z.d); fa(zzZ.n); fa(zzZ.d);} xv[zzK]=z;}  // Don't test - we know we are installing into a recursive block
// Use IRS[12] to call a verb that supports IRS.  Rank is nonnegative; result is assigned to z.  z mustn't be any other arg - it is also used as a temp
// args should be names, because they are evaluated repeatedly, and also because rank is set before one of the evaluations
#define IRS1COMMON(j,w,fs,r,f1,z) (z=(A)(r),z=(I)AR(w)>(I)(r)?z:(A)~0,jt->ranks=(RANK2T)(I)z,z=((AF)(f1))(j,(w),(A)(fs)),jt->ranks=R2MAX,z)  // nonneg rank
#define IRS1(w,fs,r,f1,z) IRS1COMMON(jt,w,fs,r,f1,z)  // nonneg rank
#define IRSIP1(w,fs,r,f1,z) IRS1COMMON(jtinplace,w,fs,r,f1,z)  // nonneg rank
#define IRS2COMMON(j,a,w,fs,l,r,f2,z) (jt->ranks=(RANK2T)(((((I)AR(a)-(l)>0)?(l):RMAX)<<RANKTX)+(((I)AR(w)-(r)>0)?(r):RMAX)),z=((AF)(f2))(j,(a),(w),(A)(fs)),jt->ranks=R2MAX,z) // nonneg rank
#define IRS2(a,w,fs,l,r,f2,z) IRS2COMMON(jt,a,w,fs,l,r,f2,z)
#define IRSIP2(a,w,fs,l,r,f2,z) IRS2COMMON(jtinplace,a,w,fs,l,r,f2,z)
// obsolete #define IRS2AGREE(a,w,fs,l,r,f2,z) {I fl=(I)AR(a)-(l); fl=MAX(0,fl); I fr=(I)AR(w)-(r); fr=MAX(0,fr); ASSERTAGREE2(AS(a),AS(w),MIN(fl,fr),fl,fr) IRS2COMMON(jt,(a),(w),fs,(l),(r),(f2),z); } // nonneg rank; check agreement first
// no longer used #define IRS2AGREE(a,w,fs,l,r,f2,z) {I fl=(I)AR(a)-(l); fl=fl<0?0:fl; I fr=(I)AR(w)-(r); fr=fr<0?0:fr; fl=fr<fl?fr:fl; ASSERTAGREE(AS(a),AS(w),fl) IRS2COMMON(jt,(a),(w),fs,(l),(r),(f2),z); } // nonneg rank; check agreement first
// call to atomic2(), similar to IRS2.  fs is a local block to use to hold the rank (declared as D fs[16]), cxx is the Cxx value of the function to be called
#define ATOMIC2(jt,a,w,fs,l,r,cxx) (FAV((A)(fs))->fgh[0]=ds(cxx), FAV((A)(fs))->id=CQQ, FAV((A)(fs))->lc=FAV(ds(cxx))->lc, FAV((A)(fs))->lrr=(RANK2T)((l)<<RANKTX)+(r), jtatomic2(jt,(a),(w),(A)fs))

// memory copy, for J blocks.  Like memcpy, but knows it can fetch outside the arg boundaries for LIT-type args.  l may be 0
// if bytelen is 1, the arg may be of any length; if 0, will be lengthened to be a multiple of Is; full words only are moved
// address may be identical but otherwise should not overlap
// Normal use allowing overcopy: JMC(d,s,l,0)
// Normal use not allowing overcopy: JMC(d,s,l,1)
// For use in loop, allowing overcopy: JMCDECL(endmask) JMCSETMASK(endmask,l,0)   DO(...,  JMCR(d,s,l,0,endmask)    )
// For use in loop, not allowing overcopy: JMCDECL(endmask) JMCSETMASK(endmask,l,1)   DO(...,  JMCR(d,s,l,1,endmask)    )
//
// Unaligned stores seem to take 10 cycles to clear, so we try to have no more than 1 of them.  If the length exceeds one store,
// move 1 full store to begin with and align the store pointer and length to within that block
#if C_AVX2 || EMU_AVX2
#define JMCDECL(mskname) __m256i mskname;
#define JMCSETMASK(mskname,l,bytelen) mskname=_mm256_loadu_si256((__m256i*)(validitymask+((-(((l)+SZI*(1-bytelen)-1)>>LGSZI))&(NPAR-1)))); /* 0->1111 1->1000 3->1110 */
#define JMCcommon(d,s,l,bytelen,mskname,mskdecl,mskset) \
{ \
 void *src=(s); \
 PREFETCH(src);  /* start bringing in the start of data */  \
 mskdecl \
 void *dst=(d); \
 I ll=(l);  /* len of valid data */ \
 /* if the operation is long, move 1 block to align the store pointer */ \
 if(ll>32*SZI){ /* TUNE should depend on repetition? */\
  _mm256_storeu_si256((__m256i*)dst,_mm256_loadu_si256((__m256i*)src)); \
  I overwr=(I)dst&(NPAR*SZI-1); dst=(C*)dst+NPAR*SZI-overwr; src=(C*)src+NPAR*SZI-overwr; ll-=NPAR*SZI-overwr; JMCSETMASK(mskname,ll,bytelen) \
 }else{mskset}  /* if short op, set mask if not set outside loop */ \
 if(likely(ll>0)){ /* an empty vector has no guaranteed pad */ \
  ll+=SZI*(1-bytelen)-1; /* len-1 to move, rounded up if moving qwords */  \
  /* copy the remnants at the end - bytes and Is.  Do this early because they have address conflicts that will take 20 */ \
  /* cycles to sort out, and we can put that time into the switch and loop branch overhead  NO - see below */ \
  /* First, the odd bytes if any */ \
  /* if there is 1 byte to do low bits of ll are 0, which means protect 7 bytes, thus 0->7, 1->6, 7->0 */ \
  if(bytelen!=0)STOREBYTES((C*)dst+(ll&(-SZI)),*(UI*)((C*)src+(ll&(-SZI))),~ll&(SZI-1));  /* copy remnant, 1-8 bytes. */ \
  /* copy up till last section */ \
  if(likely((ll-=SZI)>=0)||(bytelen==0)){  /* reduce ll (=len-1) by # bytes processed above, 1-8 (if bytelen), 0 if !bytelen (discarding garbage length).  Any left? */ \
   /* copy 128-byte sections, first one being 0, 4, 8, or 12 Is. There could be 0 to do */ \
   UI n2=DUFFLPCT(ll>>LGSZI,2);  /* # turns through duff loop */ \
   if(n2>0){ \
    UI backoff=DUFFBACKOFF(ll>>LGSZI,2); \
    dst=(C*)dst+(backoff+1)*NPAR*SZI; src=(C*)src+(backoff+1)*NPAR*SZI; \
    switch(backoff){ \
    do{ \
    case -1: _mm256_storeu_si256((__m256i*)dst,_mm256_loadu_si256((__m256i*)src)); \
    case -2: _mm256_storeu_si256((__m256i*)((C*)dst+1*NPAR*SZI),_mm256_loadu_si256((__m256i*)((C*)src+1*NPAR*SZI))); \
    case -3: _mm256_storeu_si256((__m256i*)((C*)dst+2*NPAR*SZI),_mm256_loadu_si256((__m256i*)((C*)src+2*NPAR*SZI))); \
    case -4: _mm256_storeu_si256((__m256i*)((C*)dst+3*NPAR*SZI),_mm256_loadu_si256((__m256i*)((C*)src+3*NPAR*SZI))); \
    dst=(C*)dst+4*NPAR*SZI; src=(C*)src+4*NPAR*SZI; \
    }while(--n2>0); \
    } \
   } \
   /* copy last section, 1-4 Is. ll bits 00->4 Is, 01->3 Is, etc  Do last so that previous loop doesn't have back-to-back branches  */ \
   _mm256_maskstore_epi64((I*)dst,mskname,_mm256_maskload_epi64((I*)src,mskname)); \
  } \
 } \
}
#define JMC(d,s,l,bytelen) JMCcommon(d,s,l,bytelen,endmask,JMCDECL(endmask),JMCSETMASK(endmask,ll,bytelen))  //   0->1111 1->1000 3->1110 bytelen has already been applied here
#define JMCR(d,s,l,bytelen,maskname) JMCcommon(d,s,l,bytelen,maskname,,)
#else
#define JMC(d,s,l,bytelen) MC(d,s,l);
#define JMCR(d,s,l,bytelen,maskname) MC(d,s,l);
#define JMCDECL(mskname)
#define JMCSETMASK(mskname,l,bytelen)
#endif

#define IX(n)           apv((n),0L,1L)
#define JATTN           {if(unlikely(JT(jt,adbreakr)[0]!=0)){jsignal(EVATTN); R 0;}}   // requests orderly termination at start of sentence
#define JBREAK0         {if(unlikely(2<=JT(jt,adbreakr)[0])){jsignal(EVBREAK); R 0;}}  // requests immediate stop
#define JTIPA           ((J)((I)jt|JTINPLACEA))
#define JTIPAW          ((J)((I)jt|JTINPLACEA+JTINPLACEW))
#define JTIPW           ((J)((I)jt|JTINPLACEW))
#define JTIPAtoW        (J)((I)jt+(((I)jtinplace>>JTINPLACEAX)&JTINPLACEW))  // jtinplace, with a' inplaceability transferred to w
#define JTIPWonly       (J)((I)jtinplace&~(JTINPLACEA+JTWILLBEOPENED+JTCOUNTITEMS))  // dyad jt converted to monad for w
#define JTIPEX1(name,arg) jt##name(JTIPW,arg)   // like name(arg) but inplace
#define JTIPEX1S(name,arg,self) jt##name(JTIPW,arg,self)   // like name(arg,self) but inplace
#define JTIPAEX2(name,arga,argw) jt##name(JTIPA,arga,argw)   // like name(arga,argw) but inplace on a
#define JTIPAEX2S(name,arga,argw,self) jt##name(JTIPA,arga,argw,self)   // like name(arga,argw,self) but inplace on a
// compensated accumulation with multiple accumulators.  Accumulators are acc##n, low part is c##n.
// We add in to accumulators n.  This is defined so that c is the amount that must be ADDED to the total to make it right
// uses temps y and t
#define KAHAN(in,n) {__m256d y, t; y=_mm256_add_pd(in,c##n); t=_mm256_add_pd(acc##n,y); c##n=_mm256_sub_pd(y,_mm256_sub_pd(t,acc##n)); acc##n=t;}
// similar, but acc and c are in an array.  Uses temp tt
#define KAHANA(in,n) {__m256d y, t, tt; y=_mm256_add_pd(in,accc[1][n]); tt=accc[0][n]; accc[0][n]=t=_mm256_add_pd(tt,y); accc[1][n]=_mm256_sub_pd(y,_mm256_sub_pd(t,tt));}
// given a unique num, define loop begin and end labels
#define LOOPBEGIN(num) lp##num
#define LOOPEND(num) lp##num##e
#define MAX(a,b)        ((a)>(b)?(a):(b))
#define MC              memcpy
#define MCL(dest,src,n) memcpy(dest,src,n)  // use when copy is expected to be long
#define MCI(dest,src,n) memcpy(dest,src,(n)*sizeof(*src))   // copy items of source
#define MCIL(dest,src,n) memcpy(dest,src,(n)*sizeof(*src))   // use when copy expected to be long
#define MCIS(dest,src,n) {I * RESTRICT _d=(dest); I * RESTRICT _s=(src); I _n=~(n); while((_n-=REPSGN(_n))<0)*_d++=*_s++;}  // use for short copies.  the tricky stuff is to confound the compiler so it doesn't produce memcpy
#define MCISd(dest,src,n) {I * RESTRICT _s=(src); I _n=~(n); while((_n-=REPSGN(_n))<0)*dest++=*_s++;}  // ... this version when d increments through the loop
#define MCISs(dest,src,n) {I * RESTRICT _d=(dest); I _n=~(n); while((_n-=REPSGN(_n))<0)*_d++=*src++;}  // ... this when s increments through the loop
#define MCISds(dest,src,n) {I _n=~(n); while((_n-=REPSGN(_n))<0)*dest++=*src++;}  // ...this when both
// Copy shapes.  Optimized for length <5 (<9 on avx512), subroutine for others
// For AVX, we can profitably use the MASKLOAD/STORE instruction to do all the testing
// len is # words in shape
#if C_AVX512
#define MCISH(dest,src,n) \
 {void *_d=dest,*_s=src; I _n=n;\
  if(likely(_n<=8)){__mmask8 mask=_bzhi_u32(0xff,_n); _mm512_mask_storeu_epi64(_d,mask,_mm512_maskz_loadu_epi64(mask,_s));}\
  else{memmove(_d,_s,_n<<LGSZI);}}
#elif C_AVX
#define MCISH(dest,src,n) \
 {D *_d=(D*)(dest), *_s=(D*)(src); I _n=(I)(n); \
  if(likely(_n<=NPAR)){__m256i endmask = _mm256_loadu_si256((__m256i*)(validitymask+NPAR-_n)); \
   _mm256_maskstore_pd(_d,endmask,_mm256_maskload_pd(_s,endmask)); \
  }else{memmove(_d,_s,_n<<LGSZI);} \
 }
#else
#define MCISH(dest,src,n) \
 {I *_d=(dest), *_s=(src); I _n=(I)(n); \
  if(likely(_n<=2)){ \
   _n-=1; _d=(_n<0)?jt->shapesink+1:_d; _s=(_n<0)?jt->shapesink+1:_s; \
   I _s0=_s[0],_sn=_s[_n]; _d[0]=_s0; _d[_n]=_sn;  \
  }else{memmove(_d,_s,_n<<LGSZI);} \
 }
#endif
#define MCISHd(dest,src,n) {MCISH(dest,src,n) dest+=(n);}  // ... this version when d increments through the loop
#define MCISHs(dest,src,n) {MCISH(dest,src,n) src+=(n);}
#define MCISHds(dest,src,n) {MCISH(dest,src,n) dest+=(n); src+=(n);}
// not used #define MCISU(dest,src,n) {I * RESTRICT _d=(I*)(dest); I * RESTRICT _s=(I*)(src); I _n=-(n); do{*_d++=*_s++;}while((_n-=(_n>>(BW-1)))<0);}  // always runs once
// not used #define MCISUds(dest,src,n) {I _n=-(n); do{*dest++=*src++;}while((_n-=(_n>>(BW-1)))<0);}  // always runs once

#define MIN(a,b)        ((a)<(b)?(a):(b))
#define MLEN            (SY_64?63:31)
// change the type of the inplaceable block z to t.  We know or assume that the type is being changed.  If the block is UNINCORPABLE (& therefore virtual), replace it with a clone first.  z is an lvalue
#define MODBLOCKTYPE(z,t)  {if(unlikely((AFLAG(z)&AFUNINCORPABLE)!=0)){RZ(z=clonevirtual(z));} AT(z)=(t);}
#define MODIFIABLE(x)   (x)   // indicates that we modify the result, and it had better not be read-only
// define multiply-add
#if C_AVX2 || (EMU_AVX  && (defined(__aarch64__)||defined(_M_ARM64)))
#define MUL_ACC(addend,mplr1,mplr2) _mm256_fmadd_pd(mplr1,mplr2,addend)
#elif C_AVX || EMU_AVX
#define MUL_ACC(addend,mplr1,mplr2) _mm256_add_pd(addend , _mm256_mul_pd(mplr1,mplr2))
#elif defined(__SSE2__)
#define MUL_ACC(addend,mplr1,mplr2) _mm_add_pd(addend , _mm_mul_pd(mplr1,mplr2))
#endif
#define NAN0            (_clearfp())
#if defined(MMSC_VER) && _MSC_VER==1800 && !SY_64 // bug in some versions of VS 2013
#define NAN1            {if(_SW_INVALID&_statusfp()){_clearfp();jsignal(EVNAN); R 0;}}
#define NAN1V           {if(_SW_INVALID&_statusfp()){_clearfp();jsignal(EVNAN); R  ;}}
#define NANTEST         (_SW_INVALID&_statusfp())
#else
#define NAN1            {if(unlikely(_SW_INVALID&_clearfp())){jsignal(EVNAN); R 0;}}
#define NAN1V           {if(unlikely(_SW_INVALID&_clearfp())){jsignal(EVNAN); R  ;}}
#define NANTEST         (_SW_INVALID&_clearfp())
// for debug only
// #define NAN1            {if(_SW_INVALID&_clearfp()){fprintf(stderr,"nan error: file %s line %d\n",__FILE__,__LINE__);jsignal(EVNAN); R 0;}}
// #define NAN1V           {if(_SW_INVALID&_clearfp()){fprintf(stderr,"nan error: file %s line %d\n",__FILE__,__LINE__);jsignal(EVNAN); R  ;}}
// #define NAN1T           {if(_SW_INVALID&_clearfp()){fprintf(stderr,"nan error: file %s line %d\n",__FILE__,__LINE__);jsignal(EVNAN);     }}
#endif

// can't just emu vblendvb using vblendvps because different sizing => different behaviour; so use BLENDVI when the masks are guaranteed at least 32 bits.
// Use ps over pd for greater granularity and because old cpus don't have separate single/double domains; newer cpus will have the int blend so don't care
#if C_AVX2
#define BLENDVI _mm256_blendv_epi8
#elif C_AVX || EMU_AVX
#define BLENDVI(x,y,z) _mm256_castps_si256(_mm256_blendv_ps(_mm256_castsi256_ps(x), _mm256_castsi256_ps(y), _mm256_castsi256_ps(z)))
#endif

#define NOUNROLL _Pragma("clang loop unroll(disable)") _Pragma("clang loop vectorize(disable)")  // put this just before a loop to disable unroll
#if (C_AVX&&SY_64) || EMU_AVX
// j64avx gcc _mm256_zeroupper -O2 failed SLEEF for expression % /\ ^:_1 ,: 1 2 3  => 1 2 0
// upper half of all YMM registers clear AFTER loading endmask
// ??? is_mm256_zeroupper really needed
// -mavx or /arch:AVX should already generate VEX encoded for SSE instructions
#define _mm256_zeroupperx(x)
// this is faster than reusing another register as the source anyway, because it's not a recognized idiom, so we would have a false dependency on the other register
#define _mm_setone_si128() _mm_cmpeq_epi32(_mm_setzero_si128(), _mm_setzero_si128()) // set to all ~0
#define _mm256_setone_epi64() _mm256_cmpeq_epi64(_mm256_setzero_si256(), _mm256_setzero_si256())
#define _mm256_setone_pd() _mm256_castsi256_pd(_mm256_setone_epi64())
static inline __m256i LOADV32I(void *x) { return _mm256_loadu_si256(x); }
static inline __m256d LOADV32D(void *x) { return _mm256_loadu_pd(x); }
#define NPAR ((I)(sizeof(__m256d)/sizeof(D))) // number of Ds processed in parallel
#define LGNPAR 2  // no good automatic way to do this
// loop for atomic parallel ops.  // fixed: n is #atoms (never 0), x->input (as D*), z->result (as D*), u=input atom4 and result
//                                                                                                  __SSE2__    atom2
// loop advances x and y to end +1 of region
// parms: bit0=suppress unrolling, bit1=use maskload for any aligning fetch
#define AVXATOMLOOP(parms,preloop,loopbody,postloop) \
 __m256i endmask;  __m256d u; __m256d neut=_mm256_setzero_pd(); \
 _mm256_zeroupperx(VOIDARG) \
 preloop \
 I n0=n; \
 I alignreq=(-(I)z>>LGSZI)&(NPAR-1); \
 if((-alignreq&(NPAR-n0))<0){ \
  endmask = _mm256_loadu_si256((__m256i*)(validitymask+NPAR-alignreq));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
  u=_mm256_loadu_pd(x); if(((parms)&2))u=_mm256_blendv_pd(neut,u,_mm256_castsi256_pd(endmask)); \
  loopbody _mm256_maskstore_pd(z, endmask, u); x+=alignreq; z+=alignreq; n0-=alignreq;  /* leave remlen>0 */ \
 } \
 endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-n0)&(NPAR-1)))); \
 if(!((parms)&1)){ \
  UI n2=DUFFLPCT(n0-1,3);  /* # turns through duff loop */ \
  if(n2>0){ \
   UI backoff=DUFFBACKOFF(n0-1,3); \
   x+=(backoff+1)*NPAR; z+=(backoff+1)*NPAR; \
   switch(backoff){ \
   do{ \
   case -1: \
    u=_mm256_loadu_pd(x); loopbody _mm256_storeu_pd(z, u);  \
   case -2: \
    u=_mm256_loadu_pd(x+1*NPAR); loopbody _mm256_storeu_pd(z+1*NPAR, u);  \
   case -3: \
    u=_mm256_loadu_pd(x+2*NPAR); loopbody _mm256_storeu_pd(z+2*NPAR, u);  \
   case -4: \
    u=_mm256_loadu_pd(x+3*NPAR); loopbody _mm256_storeu_pd(z+3*NPAR, u);  \
   case -5: \
    u=_mm256_loadu_pd(x+4*NPAR); loopbody _mm256_storeu_pd(z+4*NPAR, u);  \
   case -6: \
    u=_mm256_loadu_pd(x+5*NPAR); loopbody _mm256_storeu_pd(z+5*NPAR, u);  \
   case -7: \
    u=_mm256_loadu_pd(x+6*NPAR); loopbody _mm256_storeu_pd(z+6*NPAR, u);  \
   case -8: \
    u=_mm256_loadu_pd(x+7*NPAR); loopbody _mm256_storeu_pd(z+7*NPAR, u);  \
   x+=8*NPAR; z+=8*NPAR; \
   }while(--n2!=0); \
   } \
  } \
 }else{ \
  UI i=(n0+NPAR-1)>>LGNPAR;  \
  NOUNROLL while(--i!=0){ u=_mm256_loadu_pd(x); loopbody _mm256_storeu_pd(z, u); x+=NPAR; z+=NPAR;} \
 } \
 u=_mm256_maskload_pd(x,endmask);  loopbody _mm256_maskstore_pd(z, endmask, u); \
 x+=((n0-1)&(NPAR-1))+1; z+=((n0-1)&(NPAR-1))+1; \
 postloop

// this version to alternate between two code blocks.  block0 is always executed last; block 0 or 1 may be executed first
// block 0 may be executed once at the beginning to establish alignment
// the loop is never unrolled: there is one even and one odd block in the loop
#define AVXATOMLOOPEVENODD(parms,preloop,loopbody0,loopbody1,postloop) \
 __m256i endmask;  __m256d u; __m256d neut=_mm256_setzero_pd(); \
 _mm256_zeroupperx(VOIDARG) \
 preloop \
 I n0=n; \
 I alignreq=(-(I)z>>LGSZI)&(NPAR-1); \
 if((-alignreq&(NPAR-n0))<0){ \
  endmask = _mm256_loadu_si256((__m256i*)(validitymask+NPAR-alignreq));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
  u=_mm256_loadu_pd(x); if(((parms)&2))u=_mm256_blendv_pd(neut,u,_mm256_castsi256_pd(endmask)); \
  loopbody0 _mm256_maskstore_pd(z, endmask, u); x+=alignreq; z+=alignreq; n0-=alignreq;  /* leave remlen>0 */ \
 } \
 endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-n0)&(NPAR-1)))); \
  UI n2=DUFFLPCT(n0-1,1);  /* # turns through duff loop */ \
  if(n2>0){ \
   UI backoff=DUFFBACKOFF(n0-1,1); \
   x+=(backoff+1)*NPAR; z+=(backoff+1)*NPAR; \
   switch(backoff){ \
   do{ \
   case -1: \
    u=_mm256_loadu_pd(x); loopbody0 _mm256_storeu_pd(z, u);  \
   case -2: \
    u=_mm256_loadu_pd(x+1*NPAR); loopbody1 _mm256_storeu_pd(z+1*NPAR, u);  \
   x+=2*NPAR; z+=2*NPAR; \
   }while(--n2!=0); \
   } \
  } \
 u=_mm256_maskload_pd(x,endmask);  loopbody0 _mm256_maskstore_pd(z, endmask, u); \
 x+=((n0-1)&(NPAR-1))+1; z+=((n0-1)&(NPAR-1))+1; \
 postloop

#elif defined(__GNUC__)   // vector extension

#if !(defined(__aarch64__)||defined(__arm__))
typedef int64_t int64x2_t __attribute__ ((vector_size (16),aligned(16)));
typedef double float64x2_t __attribute__ ((vector_size (16),aligned(16)));
#endif
#define dump_int64x2(a,x) {int64x2_t _b=x;fprintf(stderr,"%s %lli %lli \n", a, ((long long*)(&_b))[0], ((long long*)(&_b))[1]);}
#define dump_float64x2(a,x) {float64x2_t _b=x;fprintf(stderr,"%s %f %f \n", a, ((double*)(&_b))[0], ((double*)(&_b))[1]);}

static inline __attribute__((__always_inline__)) int vec_any_si128(int64x2_t mask)
{
   return (mask[0] & 0x8000000000000000)||(mask[1] & 0x8000000000000000);
}

static inline __attribute__((__always_inline__)) float64x2_t vec_maskload_pd(double const* mem_addr, int64x2_t mask)
{
   float64x2_t ret={0.0,0.0};
   int i;

    for (i=0; i<2; i++){
      if (mask[i] & 0x8000000000000000){
        ret[i] = mem_addr[i];
      }
    }
    return ret;
}

static inline __attribute__((__always_inline__)) void vec_maskstore_pd(double * mem_addr, int64x2_t  mask, float64x2_t a)
{
   int i;
    for (i=0; i<2; i++){
      if (mask[i] & 0x8000000000000000)
        mem_addr[i] = a[i];
    }
}

static inline __attribute__((__always_inline__)) int64x2_t vec_loadu_si128(int64x2_t const * a)
{
   int64x2_t ret;
   memcpy(&ret, (int64_t *)a, 2*sizeof(int64_t));  // must cast pointer otherwise segment 
   return ret;
}

static inline __attribute__((__always_inline__)) float64x2_t vec_loadu_pd(double const * a)
{
   float64x2_t ret;
   memcpy(&ret, a, 2*sizeof(double));
   return ret;
}

static inline __attribute__((__always_inline__)) void vec_storeu_pd(double * mem_addr, float64x2_t a)
{
   memcpy(mem_addr, &a , 2*sizeof(double));
}

static inline __attribute__((__always_inline__)) float64x2_t vec_and_pd(float64x2_t a, float64x2_t b)
{
   int64x2_t a1,b1;
   float64x2_t ret;
   memcpy(&a1, &a, 2*sizeof(double));
   memcpy(&b1, &b, 2*sizeof(double));
   a1 &= b1;
   memcpy(&ret, &a1, 2*sizeof(double));
   return ret;
}

#define NPAR ((I)(sizeof(float64x2_t)/sizeof(D))) // number of Ds processed in parallel
#define LGNPAR 1  // 128-bit no good automatic way to do this
// loop for atomic parallel ops.  // fixed: n is #atoms (never 0), x->input, z->result, u=input atom4 and result
//                                                                                  __SSE2__    atom2
#define AVXATOMLOOP(parms,preloop,loopbody,postloop) \
 int64x2_t endmask;  float64x2_t u; \
 endmask = vec_loadu_si128((int64x2_t*)(validitymask+((-n)&(NPAR-1))));  /* mask for 0 1 2 3 4 5 is xx 01 11 01 11 01 */ \
 preloop \
 I i=(n-1)>>LGNPAR;  /* # loops for 0 1 2 3 4 5 is x 0 0 0 0 1 */ \
            /* __SSE2__ # loops for 0 1 2 3 4 5 is x 1 0 1 0 1 */ \
 while(--i>=0){ u=vec_loadu_pd(x); \
  loopbody \
  vec_storeu_pd(z, u); x+=NPAR; z+=NPAR; \
 } \
 u=vec_maskload_pd(x,endmask); \
 loopbody \
 vec_maskstore_pd(z, endmask, u); \
 x+=((n-1)&(NPAR-1))+1; z+=((n-1)&(NPAR-1))+1; \
 postloop
#endif

#define NUMMAX          9    // largest number represented in num[]
#define NUMMIN          (~NUMMAX)    // smallest number represented in num[]
#define OUTSEQ          "\n"  // string used for internal end-of-line (not jtpx)
// Given SZI B01s read into p, pack the bits into the MSBs of p and clear the lower bits of p
#if C_LE  // if anybody makes a bigendian CPU we'll have to recode
#if BW==64
// this is what it should be #define PACKBITS(p) {p|=p>>7LL;p|=p>>14LL;p|=p>>28LL;p<<=56LL;}
#define PACKBITS(p) {p|=p>>7LL;p|=p>>14LL;p|=p<<28LL;p&=0xff0000000; p<<=28LL;}  // this generates one extra instruction, rather than the 3 for the correct version
#define PACKBITSINTO(p,out) {p|=p>>7LL;p|=p>>14LL;out=((p|(p>>28LL))<<56)|(out>>SZI);}  // pack and shift into out, which must be unsigned
#else
#define PACKBITS(p) {p|=p>>7LL;p|=p>>14LL;p<<=28LL;}
#define PACKBITSINTO(p,out) {p|=p>>7LL;p|=p>>14LL;out=(p<<28)|(out>>SZI);}  // pack and shift into out
#endif
#endif
// parser results have the LSB set if the last thing was an assignment
#define PARSERVALUE(p) ((A)((I)(p)&-2))   // the value part of the parser return, pointer to the A block
#define PARSERASGN(p) ((I)(p)&1)   // the assignment part of the parser return, 1 if assignment
#define PARSERASGNX 0  // bit# of asgn bit
// conversion from priority index to bit# in a type with that priority
// static const UC prioritytype[] = {  // Convert priority to type bit
// B01X, LITX, C2TX, C4TX, INTX, BOXX, XNUMX, RATX, SBTX, FLX, CMPXX};
#if SY_64
#define PRIORITYTYPE(p) (((((((((((((((((((((((I)CMPXX<<5)+FLX)<<5)+SBTX)<<5)+RATX)<<5)+XNUMX)<<5)+BOXX)<<5)+INTX)<<5)+C4TX)<<5)+C2TX)<<5)+LITX)<<5)+B01X)>>((p)*5))&0x1f)
#else
#define PRIORITYTYPE(p) (((p)>=6?(((((((((I)CMPXX<<5)+FLX)<<5)+SBTX)<<5)+RATX)<<5)+XNUMX)>>(((p)-6)*5):(((((((((((I)BOXX<<5)+INTX)<<5)+C4TX)<<5)+C2TX)<<5)+LITX)<<5)+B01X)>>((p)*5))&0x1f)
#endif
// Conversion from type to priority
// B01 LIT C2T C4T INT BOX XNUM RAT SBT FL CMPX
// For sparse types, we encode here the corresponding dense type
// static const UC typepriority[] = {   // convert type bit to priority
// 0, 1, 4, 9, 10, 5, 6, 7,  // B01-RAT
// 0, 0, 0, 1, 4, 9, 10, 5,  // x x SB01-SBOX
// 8, 2, 3};  // SBT C2T C4T
#if SY_64
#define TYPEPRIORITY(t) (((((t)&0xffff)?0x5a941000765a9410:0x328)>>((CTTZ(t)&0xf)*4))&0xf)
#else
#define TYPEPRIORITY(t) (((((t)&0xff)?0x765a9410:((t)&0xff00)?0x5a941000:0x328)>>((CTTZ(t)&0x7)*4))&0xf)
#endif

// same but destroy w
#define PRISTCLRF(w) if(unlikely((AFLAG(w)&AFVIRTUAL)!=0)){w=ABACK(w);} AFLAGPRISTNO(w)   // used only at end, when w can be destroyed
#define PRISTCOMMON(w) awback=(w); PRISTCLRF(awback)
#define PRISTCLRNODCL(w) PRISTCOMMON(w)
// normal entry points.  clear PRISTINE flag in w (or its backer, if virtual) because we have removed something from it
#define PRISTCLR(w) A awback; PRISTCLRNODCL(w)
#define PRISTFROMW(w) (AFLAG(w)&((SGNTO0(AC(w))&((I)jtinplace>>JTINPLACEWX))<<AFPRISTINEX))
#define PRISTFROMA(a) (AFLAG(a)&((SGNTO0(AC(a))&((I)jtinplace>>JTINPLACEAX))<<AFPRISTINEX))
// same, but destroy w in the process
// transfer pristinity of w to z
#define PRISTXFER(z,w) AFLAGORLOCAL(z,PRISTFROMW(w)) PRISTCOMMON(w)
// transfer pristinity of w to z, destroying w
#define PRISTXFERF(z,w) AFLAGORLOCAL(z,PRISTFROMW(w)) PRISTCLRF(w)  // use w bit of jtinplace
#define PRISTXFERAF(z,a) AFLAGORLOCAL(z,PRISTFROMA(w)) PRISTCLRF(a)  // use a bit of jtinplace
// same, but with an added condition (in bit 0)
#define PRISTXFERFIF(z,w,cond)AFLAGORLOCAL(z,AFLAG(w)&(((cond)&SGNTO0(AC(w))&((I)jtinplace>>JTINPLACEWX))<<AFPRISTINEX)) PRISTCLRF(w)
// transfer pristinity from a AND w to z (not if a==w)
#define PRISTXFERF2(z,a,w) AFLAGORLOCAL(z,AFLAG(a)&AFLAG(w)&(((a!=w)&SGNTO0(AC(a)&AC(w))&((I)jtinplace>>JTINPLACEAX)&((I)jtinplace>>JTINPLACEWX))<<AFPRISTINEX)) \
                           PRISTCLRF(a) PRISTCLRF(w)
// PROD multiplies a list of numbers, where the product is known not to overflow a signed int (for example, it might be part of the shape of a nonempty dense array)
// assign length first so we can sneak some computation into ain in va2.  DON'T call a subroutine, to keep registers free
#define PRODCOMMON(z,length,ain,type) {I _i=(length); I * RESTRICT _zzt=(ain); \
if(likely(type _i<3)){z=(I)&oneone; z=type _i>1?(I)_zzt:z; _zzt=type _i<1?(I*)z:_zzt; z=((I*)z)[1]; z*=_zzt[0];}else{z=1; NOUNROLL do{z*=_zzt[type --_i];}while(type _i); } }
#define PROD(z,length,ain) PRODCOMMON(z,length,ain,)
// This version ignores bits of length above the low RANKTX bits
#define PRODRNK(z,length,ain) PRODCOMMON(z,length,ain,(RANKT))

// PRODX replaces CPROD.  It is PROD with a test for overflow included.  To save calls to mult, PRODX takes an initial value
// PRODX takes the product of init and v[0..n-1], generating error if overflow, but waiting till the end so no error if there is a 0 in the product
// overflow sets z to the error value of 0; if we see a multiplicand of 0 we stop right away so we can skip the error
// This is written to be branchless for rank < 3
#if SY_64
// I have been unable to make clang produce a simple loop that doesn't end with a backward branch.  So I am going to handle ranks 0-2 here and call a subroutine for the rest
#define PRODX(z,n,v,init) \
 {I nn=(n); \
  if(likely(nn<3)){I temp=(init);  /* must use temp because init may depend on z */ \
   I *_zzt=(v); _zzt+=nn-2; z=(I)&oneone; _zzt=nn>=1?_zzt:(I*)z; z=nn>1?(I)_zzt:z;   /* set up pointers to args, and init value */ \
   z=((I*)z)[0]; if(likely(z!=0)){DPMULDZ(z,_zzt[1],z); if(likely(_zzt[1]!=0)){DPMULDZ(z,temp,z);if(likely(temp!=0)){ASSERT(z!=0,EVLIMIT)}}}  /* no error if any nonzero */ \
  }else{DPMULDE(init,prod(nn,v),z) RE(0)} /* error if error inside prod */ \
 }
#else
#define PRODX(z,n,v,init) RE(z=mult(init,prod(n,v)))
#endif
// CPROD is to be used to create a test testing #atoms.  Because empty arrays can have cells that have too many atoms, we can't use PROD if
// we don't know that the array isn't empty or will be checked later
#define CPROD(t,z,x,a) PRODX(z,x,a,1)
// PROLOG/EPILOG are the main means of memory allocation/free.  jt->tstack contains a pointer to every block that is allocated by GATV(i. e. all blocks).
// GA causes a pointer to the block to be pushed onto tstack.  PROLOG saves a copy of the stack pointer in _ttop, a local variable in its function.  Later, tpop(_ttop)
// can be executed to free every block that the function allocated, without requiring bookkeeping in the function.  This may be done from time to time in
// long-running definitions, to free memory [for this application it is normal to do some allocating of working memory, then save the tstack pointer in a local name
// other than _ttop, then periodically do tpop(other local name); such a sequence will free up all memory that was allocated after the working memory; the working
// memory itself will be freed by the eventual tpop(_ttop)].
// EPILOG performs the tpop(_ttop), but it has another important function: that of preserving the result of a function.  Of all the blocks that were allocated by a function,
// one (possibly including its descendants) is the result of the function.  It must not be freed, so that it can carry the result back to the caller of this function.
// So, it is preserved by incrementing its usecount before the tpop(_ttop); then after the tpop, it is pushed back onto the tstack, indicating that it will be freed
// by the next-higher-level function.  Thus, when X calls Y inside PROLOG/EPILOG, the result of Y (which is an A block), has the same viability as any other GA executed in X
// (unless its usecount is > 1 because it was assigned elsewhere)
#define PROLOG(x)       A *_ttop=jt->tnextpushp
#define PROLOGNOTREQ(x)   // if nothing is allocated, we don't really need PROLOG
#define EPILOGNORET(z) (gc(z,_ttop))   // protect z and return its address
#define EPILOG(z)       RETF(EPILOGNORET(z))   // z is the result block
#define EPILOGNOVIRT(z)       R rifvsdebug((gc(z,_ttop)))   // use this when the repercussions of allowing virtual result are too severe
#define EPILOGZOMB(z)       if(unlikely(!gc3(&(z),0L,0L,_ttop)))R0; RETF(z);   // z is the result block.  Use this if z may contain inplaceable contents that would free prematurely
// Routines that do little except call a function that does PROLOG/EPILOG have EPILOGNULL as a placeholder
#define EPILOGNULL(z)   R z
#define EPILOGNOTREQ(z)   R z  // used if originak JE had needless EPILOG
// Routines that do not return A
#define EPILOG0         tpop(_ttop)
// Routines that modify the comparison tolerance must stack it
#define PUSHCCT(value) D cctstack = jt->cct; jt->cct=(value);   // declare the stacked value where it can be popped
#define PUSHCCTIF(value,cond) D cctstack = jt->cct; if(cond)jt->cct=(value);   // declare the stacked value where it can be popped
#define POPCCT jt->cct=cctstack;
// When we push, we are about to execute verbs before the last one, and an inplacement there would lead to the name's being assigned with invalid
// data.  So, we clear the inplace variables if we don't want to allow that: if the user set zomblevel=0, or if there is no local symbol table
// (which means the user is fooling around at the keyboard & performance is not as important as transparency)
#define CLEARZOMBIE     {jt->zombieval=0;}  // Used when we know there shouldn't be an assignsym, just in case
#define PUSHZOMB A savasginfo = jt->zombieval; if(unlikely(JT(jt,asgzomblevel)==0)){CLEARZOMBIE}
#define POPZOMB {jt->zombieval=savasginfo;}
#define R               return

/* see above: include "jr0.h"
#if FINDNULLRET   // When we return 0, we should always have an error code set.  trap if not
#define R0 {if(!jt->jerr)SEGFAULT; R 0;}
#else
#define R0 R 0;
#endif
*/

// In the original JE many verbs returned a clone of the input, i. e. R ca(w).  We have changed these to avoid the clone, but we preserve the memory in case we need to go back
#define RCA(w)          R w
#define REGOTO(exp,lbl) {if(unlikely(((exp),jt->jerr!=0)))goto lbl;}
#define RESETERRT(t)    {t->etxn=t->jerr=0;t->emsgstate&=~(EMSGSTATEFORMATTED|EMSGSTATEPAREN);}
#define RESETERR        RESETERRT(jt)
#define RESETERRC       {jt->jerr=0; jt->etxn=MIN(jt->etxn,0);}  // clear error; clear error text too, but not if frozen.  Used only when formatting ARs
#define RESETERRNO      {jt->jerr=0;jt->emsgstate&=~(EMSGSTATEFORMATTED|EMSGSTATEPAREN);}  // reset the number but not the message; used in adverse/throw. to keep the user's message
// obsolete #define RESETERRANDMSG  {jt->etxn1=jt->etxn=jt->jerr=0;jt->emsgstate&=0x7f;}
#define RESETRANK       (jt->ranks=R2MAX)
#define RZSUFF(exp,suff) {if(unlikely(!(exp))){suff}}
#define RZ(exp)         RZSUFF(exp,R0)
#define RZQ(exp)         RZSUFF(exp,R 0;)  // allows FINDNULLRET without jt
// obsolete #define RE(exp)         {if(unlikely(((exp),jt->jerr!=0)))R 0;}
#define RE(exp)         RZ(((exp),jt->jerr==0))  // execute exp, then return if error
#define RZGOTO(exp,lbl) RZSUFF(exp,goto lbl;)
#define RNE(exp)        {R unlikely(jt->jerr!=0)?0:(exp);}  // always return, with exp if no error, 0 if error
#if MEMAUDIT&0xc
#define DEADARG(x)      (((I)(x)&~3)?(AFLAG((A)((I)(x)&~3))&LPAR?SEGFAULT:0):0); if(MEMAUDIT&0x10)auditmemchains(); if(MEMAUDIT&0x2)audittstack(jt); 
#define ARGCHK1D(x)     ARGCHK1(x)  // these not needed normally, but useful for debugging
#define ARGCHK2D(x,y)   ARGCHK2(x,y)
#else
#define DEADARG(x)      
#define ARGCHK1D(x)
#define ARGCHK2D(x,y)
#endif
#define ARGCHK1(x)      {RZ(x) DEADARG(x)}   // bit set in deadbeef
#define ARGCHK2(x,y)    {ARGCHK1(x) ARGCHK1(y)}
#define ARGCHK3(x,y,z)  {ARGCHK1(x) ARGCHK1(y) ARGCHK1(z)}


// RETF is the normal function return.  For debugging we hook into it
#if AUDITEXECRESULTS && (FORCEVIRTUALINPUTS==2)
#define RETF(exp)       A ZZZz = (exp); if (!ZZZz && !jt->jerr) SEGFAULT; auditblock(ZZZz,1,1); ZZZz = virtifnonip(jt,0,ZZZz); R ZZZz
#else
#if MEMAUDIT&0xc
#define RETF(exp)       {A ZZZz = (exp); DEADARG(ZZZz); R ZZZz;}
#else
#if FINDNULLRET   // When we return 0, we should always have an error code set.  trap if not
#define RETF(exp)       {A ZZZz = (exp); if(ZZZz==0)R0 R ZZZz;}
#else
#define RETF(exp)       R exp;
#endif
#endif
// Input is a byte.  It is replicated to all lanes of a UI
#endif
#if BW==64
#define REPLBYTETOW(in,out) (out=(UC)(in),out|=out<<8,out|=out<<16,out|=out<<32)
#else
#define REPLBYTETOW(in,out) (out=(UC)(in),out|=out<<8,out|=out<<16)
#endif
#if C_LE
// Output is pointer, Input is I/UI, count is # bytes to NOT store to output pointer (0-7).
#define STOREBYTES(out,in,n) {*(UI*)(out) = (*(UI*)(out)&~((UI)~(I)0 >> ((n)<<3))) | ((in)&((UI)~(I)0 >> ((n)<<3)));}
#endif
// Input is the name of word of bytes.  Result is modified name, 1 bit per input byte, spaced like B01s, with the bit 0 iff the corresponding input byte was all 0.  Non-boolean bits of result are garbage.
#define ZBYTESTOZBITS(b) (b=b|((b|(~b+VALIDBOOLEAN))>>7))  // for each byte: zero if b0 off, b7 off, and b7 turns on when you subtract 1 or 2
// to verify gah conversion #define RETF(exp)       { A retfff=(exp);  if ((retfff) && ((AT(retfff)&SPARSE && AN(retfff)!=1) || (!(AT(retfff)&SPARSE) && AN(retfff)!=prod(AR(retfff),AS(retfff)))))SEGFAULT;; R retfff; }
#define SBSV(x)         (CAV1(JT(jt,sbstrings))+(I)(x))
#define SBUV(x)         (SBUV4(JT(jt,sbu))+(I)(x))
// #define SEGFAULT        (__builtin_trap())
#ifdef _WIN32
#define FSYNC_STDERR
#else
#define FSYNC_STDERR fsync(STDERR_FILENO);
#endif
#define SEGFAULT        ({do{ \
                         fprintf(stderr,"trap : file %s line %d\n",__FILE__,__LINE__); \
                         FSYNC_STDERR; \
                         (void)__builtin_trap(); \
                        }while(0);0;})
#define SGN(a)          ((I )(0<(a))-(I )(0>(a)))
#define SMAX            65535
#define SMIN            (-65536)
#if SY_64
#define SYMHASH(h,n)    (((UI)(h)*(UI)(n)>>32)+SYMLINFOSIZE)   // h is hash value for symbol; n is number of symbol chains (not including LINFO entries)
#else
#define SYMHASH(h,n)    ((UI)(((D)(h)*(D)(n)*(1.0/4294967296.0))+SYMLINFOSIZE))   // h is hash value for symbol; n is number of symbol chains (not including LINFO entries)
#endif
// symbol tables.   jt->locsyms is locals and jt->global is globals.  AN(table) gives #hashchains+1; if it's 1 we have an empty table, used to indicate that there are no locals
// (the empty table can also be recognized by its address which is not on a cacheline boundary)
// At all times we keep the k field of locsyms as a copy of jt->global so that if we need it for u./v. we know what the symbol tables were.  We could remove jt->global but that would cost
// some load instructions sometimes.  AM(local table) points to the previous local table in the stack, looping to self at end
#define SYMSETGLOBAL(l,g) (jt->global=(g), AKGST(l)=(g))  // l is jt->locsyms, g is new global value
#define SYMRESTOREFROMLOCAL(l) (jt->locsyms=(l), jt->global=AKGST(l))  // go to stacked local l
#define SYMSWAPTOLOCAL(l,lsave) (lsave=jt->locsyms, SYMRESTOREFROMLOCAL(l))  // go to stacked local l, save current in lsave
#define SYMSETLOCAL(l) (AKGST(l)=jt->global, jt->locsyms=(l))  // change the locals to l
#define SYMPUSHLOCAL(l) (AM(l)=(I)jt->locsyms, SYMSETLOCAL(l))  // push l onto locals stack
#define SYMORIGIN JT(jt,sympv)  // the origin of the global symbol table
#define SYMGLOBALROOT SYMORIGIN[0].next   // the root of the shared free-symbol chain
#define SYMRESERVEPREFSUFF(n,pref,suff) if(unlikely(SYMNEXT(jt->symfreehead[0])==0||((n)>1&&SYMNEXT(SYMORIGIN[SYMNEXT(jt->symfreehead[0])].next)==0))){pref RZ(jtreservesym(jt,n)) suff}   // if call to reserve needed, bracket with pref/suff
#define SYMRESERVE(n) SYMRESERVEPREFSUFF(n,,)   // called outside of lock to make sure n symbols are available for assignment
// fa() the value when a symbol is deleted/reassigned.  If the symbol was ABANDONED, don't fa() because there was no ra() - but do revert 1 to 8..1 so that it may be freed by the caller as abandoned
// Implies that AM must not be modified when abandoned block is assigned to x/y.
// Clear KNOWNNAMED since we are removing the value from a name
// split into two parts: the symbol-dependent and not, so we can move the expensive part outside of lock
#define SYMVALFA1(l,faname) {if(faname!=0){if(unlikely(((l).flag&LWASABANDONED)!=0)){(l).flag&=~LWASABANDONED; AFLAGCLRKNOWN(faname); if(likely(AC(faname)<2))ACRESET(faname,ACINPLACE|ACUC1); faname=0;}}}
#define SYMVALFA2(faname) if(faname!=0){faaction(jt,faname,AFLAGCLRKNOWN(faname));}
#define SYMVALFA(l) {A v=(l).val; SYMVALFA1(l,v) SYMVALFA2(v)}
#define SZA             ((I)sizeof(A))
#define LGSZA    LGSZI  // we always require A and I to have same size
#define SZD             ((I)sizeof(D))
#define SZI             ((I)sizeof(I))
#define LGSZD    3  // lg(#bytes in a D)
#define SZI4            ((I)sizeof(I4))
#define LGSZI4   2  // lg (bytes in an I4)
#define SZUI4            ((I)sizeof(UI4))
#define LGSZUI4  2  // lg(#bytes in a UI4)
#define SZUS            ((I)sizeof(US))
#define LGSZUS   1  // lg(bytes in a US)
#define SZS            ((I)sizeof(S))
#define LGSZS   1  // lg (bytes in an S)

#if (C_AVX2&&SY_64) || EMU_AVX2
// create quad-precision product of double-precision inputs.  outhi must not be an input; outlo can
#define TWOPROD(in0,in1,outhi,outlo) outhi=_mm256_mul_pd(in0,in1); outlo=_mm256_fmsub_pd(in0,in1,outhi);
// create quad-precision sum of inputs, where it is not known which is larger  NOTE in0 and outhi might be identical.  outlo must not be an input.  Needs sgnbit.
#define TWOSUM(in0,in1,outhi,outlo) {__m256d t=_mm256_andnot_pd(sgnbit,in0); outlo=_mm256_andnot_pd(sgnbit,in1); t=_mm256_sub_pd(t,outlo); \
                                    outlo=_mm256_blendv_pd(in0,in1,t); t=_mm256_blendv_pd(in1,in0,t); /* outlo=val with larger abs t=val with smaller abs */ \
                                    outhi=_mm256_add_pd(in0,in1); /* single-prec sum */ \
                                    outlo=_mm256_sub_pd(outlo,outhi); /* big-(big+small): implied val of -small after rounding */ \
                                    outlo=_mm256_add_pd(outlo,t);}  // amt by which actual value exceeds implied: this is the lost low precision
// Same, but we know first argument has bigger absval.  outhi cannot be an input; outlo can be the same as inbig
#define TWOSUMBS(inbig,insmall,outhi,outlo) {outhi=_mm256_add_pd(inbig,insmall); /* single-prec sum */ \
                                    outlo=_mm256_sub_pd(inbig,outhi); /* big-(big+small): implied val of -small after rounding */ \
                                    outlo=_mm256_add_pd(outlo,insmall);}  // amt by which actual value exceeds implied: this is the lost low precision
// create quad-precision product of quad x double.  outlo can be qp0
// This result is not in canonical form: outlo may have magnitude more than 1 ULP of outhi, but not more than 2 ULP
#define TWOPRODQD(qp0,qp1,dp,outhi,outlo) TWOPROD(qp0,dp,outhi,outlo) outlo=_mm256_fmadd_pd(qp1,dp,outlo);
#define DPADD(hi0,lo0,hi1,lo1,outhi,outlo)  outhi=_mm256_add_pd(hi0,hi1); outlo=_mm256_add_pd(lo0,lo1);
#else
#define TWOSPLIT(a,x,y) y=(a)*134217730.0; x=y-(a); x=y-x; y=(a)-x;   // must avoid compiler tuning
#define TWOSUM(in0,in1,outhi,outlo) t=(in0)+(in1); outlo=t-(in0); outlo=((in0) - (t-outlo)) + ((in1)-outlo); outhi=t;
#define TWOPROD(in0,in1,outhi,outlo) TWOSPLIT(in0,i00,i01) TWOSPLIT(in1,i10,i11) outhi=(in0)*(in1); outlo=i01*i11 - (((outhi-i00*i10) - i01*i10) - i00*i11);  // must avoid compiler tuning   needs t, i00, i01, i10, i11
#define DPADD(hi0,lo0,hi1,lo1,outhi,outlo)  outhi=hi0+hi1; outlo=lo0+lo1;
#endif
#define VAL1            '\001'
#define VAL2            '\002'
// like vec(INT,n,v), but without the call and using shape-copy
#define VECI(z,n,v) {GATV0(z,INT,(I)(n),1); MCISH(IAV1(z),(v),(I)(n));}
#define WITHDEBUGOFF(stmt) {UC _d=jt->uflags.trace&TRACEDB;jt->uflags.trace&=~TRACEDB; \
  C _e=jt->emsgstate; jt->emsgstate|=EMSGSTATENOTEXT|EMSGSTATENOLINE|EMSGSTATENOEFORMAT; \
  stmt jt->uflags.trace=_d|(jt->uflags.trace&~TRACEDB); jt->emsgstate=_e;}  // execute stmt with debug turned off; restore at end
#if C_LE
#if BW==64
#define IHALF0  0x00000000ffffffffLL
#else
#define IHALF0  0x0000ffff
#endif
#define B0000   0x00000000
#define B0001   0x01000000
#define B0010   0x00010000
#define B0011   0x01010000
#define B0100   0x00000100
#define B0101   0x01000100
#define B0110   0x00010100
#define B0111   0x01010100
#define B1000   0x00000001
#define B1001   0x01000001
#define B1010   0x00010001
#define B1011   0x01010001
#define B1100   0x00000101
#define B1101   0x01000101
#define B1110   0x00010101
#define B1111   0x01010101
#define BS00    0x0000
#define BS01    0x0100
#define BS10    0x0001
#define BS11    0x0101
#else
#if BW==64
#define IHALF0  0xffffffff00000000LL
#else
#define IHALF0  0xffff0000
#endif
#define B0000   0x00000000
#define B0001   0x00000001
#define B0010   0x00000100
#define B0011   0x00000101
#define B0100   0x00010000
#define B0101   0x00010001
#define B0110   0x00010100
#define B0111   0x00010101
#define B1000   0x01000000
#define B1001   0x01000001
#define B1010   0x01000100
#define B1011   0x01000101
#define B1100   0x01010000
#define B1101   0x01010001
#define B1110   0x01010100
#define B1111   0x01010101
#define BS00    0x0000
#define BS01    0x0001
#define BS10    0x0100
#define BS11    0x0101
#endif



#define CACHELINESIZE 64  // size of processor cache line, in case we align to it
#define VIRTPAGESIZE 4096  // size of the memory mapped by a single TLB entry


// flags in call to cachedmmult and blockedmmult
#define FLGCMPX 0
#define FLGCMP ((I)1<<FLGCMPX)  // arguments are complex
#define FLGAUTRIX 1
#define FLGAUTRI ((I)1<<FLGAUTRIX)  // left arg is upper-triangular
#define FLGWUTRIX 2
#define FLGWUTRI ((I)1<<FLGWUTRIX)  // left arg is upper-triangular
#define FLGINTX 3
#define FLGINT ((I)1<<FLGINTX)  // args are INT
#define FLGZFIRSTX 4
#define FLGZFIRST ((I)1<<FLGZFIRSTX)  // first pass of the Z values, use 0
#define FLGZLASTX 5
#define FLGZLAST ((I)1<<FLGZLASTX)  // last pass of the Z values, write to output
#define FLGWMINUSZX 6
#define FLGWMINUSZ ((I)1<<FLGWMINUSZX)  // calculate z-x*y rather than x*y.  Used by %.

#if !defined(C_CRC32C)
#define C_CRC32C 0
#endif
#if (C_AVX&&SY_64) || defined(__aarch64__) || defined(_M_ARM64) || EMU_AVX
#undef C_CRC32C
#define C_CRC32C 1
#endif

#define J struct JSTstruct *
#include "ja.h" 
#include "jc.h" 
#include "jtype.h" 
#include "jerr.h" 
#include "m.h"
#include "jt.h" 
#include "mt.h"
#include "jlib.h"
#include "je.h" 
#include "va.h" 
#include "vq.h" 
#include "vx.h" 
#include "vz.h"
#include "vdx.h"  
#include "a.h"
#include "s.h"
#include "jgmp.h"


// CTTZ(w) counts trailing zeros in low 32 bits of w.  Result is undefined if w is 0.
// CTTZZ(w) does the same, but returns 32 if w is 0
// CTTZI(w) counts trailing zeros in an argument of type I (32 or 64 bits depending on architecture)

// CTTZ uses the single-instruction count-trailing-zeros instruction to convert
// a 1-bit mask to a bit number.  If this instruction is available on your architecture/compiler,
// you should use the compiler intrinsic to create this instruction, and define the CTTZ and CTTZZ
// macros to use the instruction inline.  It is used enough in the J engine to make a difference.

// If you set AUDITCOMPILER to 1, i.c will include code to test CTTZ (and signed shift) on startup and crash if it
// does not perform properly, as a debugging aid.

// If CTTZ is not defined, the default routine defined in u.c will be used.  You can look there
// for the complete spec for CTTZ and CTTZZ.

// CTLZ(I) returns the bit position of the highest 1-bit

#if defined(MMSC_VER)  // SY_WIN32
// do not include intrin.h
// #include <intrin.h>
#define CTTZ(w) _tzcnt_u32((UINT)(w))
#if SY_64
#define CTTZI(w) _tzcnt_u64((UI)(w))
#define CTLZI(in,out) _BitScanReverse64(&(out),in)  // actually bit # of highest set bit
#else
#define CTTZI(w) _tzcnt_u32((UINT)(w))
#define CTLZI(in,out) _BitScanReverse(&(out),in)
#endif
#define CTTZZ(w) ((w)==0 ? 32 : CTTZ(w))
#endif

#ifdef MMSC_VER
#define NOINLINE __declspec(noinline)
#else
#define NOINLINE __attribute__((noinline))
#ifndef __forceinline
#define __forceinline inline __attribute__((__always_inline__))
#endif
#endif
#ifdef __MINGW32__
// original definition
// #define __forceinline extern __inline__ __attribute__((__always_inline__,__gnu_inline__))
#ifdef __forceinline
#undef __forceinline
#endif
#define __forceinline __inline__ __attribute__((__always_inline__,__gnu_inline__))
#endif

#ifdef __GNUC__
#define CTTZ(w) __builtin_ctzl((UINT)(w))
#if SY_64
#define CTTZI(w) __builtin_ctzll((UI)(w))
#define CTLZI(w,out) (out=(63-__builtin_clzll((UI)(w))))
#else
#define CTTZI(w) __builtin_ctzl((UINT)(w))
#define CTLZI(w,out) (out=(31-__builtin_clzl((UI)(w))))
#endif
#define CTTZZ(w) ((w)==0 ? 32 : CTTZ(w))
#endif

// For older processors, TZCNT is executed as BSF, which differs from TZCNT in that it does not
// set the Z flag if the result is 0.  The optimizer sometimes turns a switch into tests rather than a branch
// table, and it expects TZCNT to set the Z flag properly.  We use CTTZNOFLAG to set it right
#define CTTZNOFLAG(w) (CTTZ(w)&31)

// parallel bit extract/deposit.  Operate on UI types.  In our use, the second argument is constant, so that if the compiler has to emulate
// the instruction it won't take too long.  It would be a good idea to check the generated code to ensure the compiler does this
#if (C_AVX2&&SY_64)
#define PEXT(s,m) _pext_u64(s,m)
#define PDEP(s,m) _pdep_u64(s,m)
#define BZHI(s,i) _bzhi_u64(s,i)
#else
// #define PEXT(s,m) _pext_u32(s,m)
// #define PDEP(s,m) _pdep_u32(s,m)
#endif

#ifndef offsetof
#ifdef __GNUC__
#define offsetof(TYPE, MEMBER)  __builtin_offsetof (TYPE, MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif
#endif

// Insert other compilers/architectures here

// Insert CTLZ here if CTTZ is not available

// If your machine supports count-leading-zeros but not count-trailing-zeros, you can define the macros
// CTLZ/CTLZI, which returns the number of high-order zeros in the low 32 bits of its argument, and the following
// CTTZ will be defined:
#if defined(CTLZ) && !defined(CTTZ)
#define CTTZ(w) (31-CTLZ((w)&-(w)))
#define CTTZI(w) (63-CTLZI((w)&-(w)))
#define CTTZZ(w) (0xffffffff&(w) ? CTTZ(w) : 32)
#endif

// If CTTZ is not defined, the following code will use the default from u.c:
#if !defined(CTTZ)
extern I CTTZ(I);
extern I CTTZI(I);
extern I CTTZZ(I);
#endif
#if !defined(CTLZI)
extern I CTLZI_(UI,UI4*);
#define CTLZI(in,out) CTLZI_(in,&(out))
#endif

#if C_AVX2&&__x86_64__
#define BLSI(x) _blsi_u64(x)
#define BLSR(x) _blsr_u64(x)
#else
#define BLSI(x) ((x)&-(x))
#define BLSR(x) ((x)&~BLSI(x))
#endif

static inline UI4 rol32(UI4 x,I s){ R (x<<s)|(x>>(32-s)); }
static inline UI4 ror32(UI4 x,I s){ R (x>>s)|(x<<(32-s)); }

// Set these switches for testing
#define AUDITBP 0  // Verify that bp() returns expected data
#define AUDITCOMPILER 0  // Verify compiler features CTTZ, signed >>


// JPFX("here we are\n")
// JPF("size is %i\n", v)
// JPF("size and extra: %i %i\n", (v,x))
#define JPFX(s)  {char b[1000]; sprintf(b, s);    jsto(gjt,MTYOFM,b);}
#define JPF(s,v) {char b[1000]; sprintf(b, s, v); jsto(gjt,MTYOFM,b);}
extern JS gjt; // global for JPF (procs without jt)

#if SY_WINCE_MIPS
/* strchr fails for CE MIPS - neg chars - spellit fails in ws.c for f=.+.  */
#define strchr(a,b)     (C*)strchr((unsigned char*)(a), (unsigned char)(b))
#endif
#if (defined(__arm__)||defined(__aarch64__)||defined(_M_ARM64)) && !defined(__APPLE__)
// option -fsigned-char in android and raspberry
#ifdef strchr
#undef strchr
#endif
#define strchr(a,b)     (C*)strchr((unsigned char*)(a), (unsigned char)(b))
#endif
#define ZZ(x)

/* workaround clang branch prediction side effect */
#if defined(__clang__) && ( (__clang_major__ > 3) || ((__clang_major__ == 3) && (__clang_minor__ > 3)))
#define dmul2(u,v) ({__asm__("" ::: "cc");(u)*(v);})
#define ddiv2(u,v) ({__asm__("" ::: "cc");(u)/(v);})
#else
#define dmul2(u,v) ((u)*(v))
#define ddiv2(u,v) ((u)/(v))
#endif

/* (hopefully) turn off some re-scheduling optimization  */
#ifdef __GNUC__
#define CCBLOCK __asm__("" ::: "cc")
#else
#define CCBLOCK
#endif

#include <fenv.h>
#if SYS & SYS_UNIX
// bug clang isnan(x) set NaN flag if x is NaN
#if defined(ANDROID) && (defined(__aarch32__)||defined(__arm__))
#define _isnan       __builtin_isnan
#else
#define _isnan       isnan
#endif
#define _SW_INVALID  FE_INVALID

static inline UINT _clearfp(void){int r=fetestexcept(FE_ALL_EXCEPT);
 feclearexcept(FE_ALL_EXCEPT); return r;
}
#endif

// Define integer multiply, *z=x*y but do something else if integer overflow.
// Depending on the compiler, the overflowed result may or may not have been stored
#if SY_64

#if C_USEMULTINTRINSIC
#if defined(MMSC_VER)  // SY_WIN32
#define DPMULDECLS
// DPMUL: *z=x*y, execute s if overflow
#define DPMUL(x,y,z,s) {I _l,_h; *z=_l=_mul128(x,y,&_h); if(_h+(SGNTO0(_l))){s}}
#define DPMULDDECLS
#define DPMULD(x,y,z,s) {I _h; z=_mul128(x,y,&_h); if(_h+(SGNTO0(z))){s}}
#define DPMULDZ(x,y,z) DPMULD(x,y,z,z=0;)
#define DPMULDE(x,y,z)  DPMULD(x,y,z,ASSERT(0,EVLIMIT))
#define DPUMUL(x,y,z,h) {z=_umul128((x),(y),&(h));}  // product in z and h
#define DPUMULH(x,y,h) {_umul128((x),(y),&(h));}  // high product in h
#else
#define DPMULDECLS
#define DPMUL(x,y,z,s) if(unlikely(__builtin_smulll_overflow(x,y,z))){s}
#define DPMULDDECLS
#define DPMULD(x,y,z,s) if(unlikely(__builtin_smulll_overflow(x,y,&z))){s}
#define DPMULDZ(x,y,z) z=__builtin_smulll_overflow(x,y,&z)?0:z;
#define DPMULDE(x,y,z) ASSERT(!__builtin_smulll_overflow(x,y,&z),EVLIMIT)
#define DPUMUL(x,y,z,h) {__int128 _t; _t=(__int128)(x)*(__int128)(y); z=(I)_t; h=(I)(_t>>64);}  // product in z and h
#define DPUMULH(x,y,h) {__int128 _t; _t=(__int128)(x)*(__int128)(y); h=(I)(_t>>64);}  // high product in h
#define DPUMULU(x,y,z,h) {__uint128_t _t=(__uint128_t)(x)*(__uint128_t)(y);z=(UI)_t;h=(UI)(_t>>64);}
#endif
#else // C_USEMULTINTRINSIC 0 - use standard-C version (64-bit)
#define DPMULDECLS
#define DPMUL(x,y,z,s) {I _l, _x=(x), _y=(y); D _d; _l=_x*_y; _d=(D)_x*(D)_y-(D)_l; *z=_l; _d=ABS(_d); if(_d>1e8){s}}  // *z may be the same as x or y
#define DPMULDDECLS
#define DPMULD(x,y,z,s) {I _l, _x=(x), _y=(y); D _d; _l=_x*_y; _d=(D)_x*(D)_y-(D)_l; z=_l; _d=ABS(_d); if(_d>1e8){s}}
#define DPMULDZ(x,y,z) DPMULD(x,y,z,z=0;)
#define DPMULDE(x,y,z)  DPMULD(x,y,z,ASSERT(0,EVLIMIT))
#endif

#else  // 32-bit

#if C_USEMULTINTRINSIC
#if defined(MMSC_VER)  // SY_WIN32
// optimizer can't handle this #define SPDPADD(addend, sumlo, sumhi) {C c; c=_addcarry_u32(0,addend,sumlo,&sumlo); _addcarry_u32(c,0,sumhi,&sumhi);}
#define DPMULDECLS unsigned __int64 _p;
#define DPMUL(x,y,z,s) _p = __emul(x,y); *z=(I)_p; if((_p+0x80000000U)>0xFFFFFFFFU){s}
#define DPMULDDECLS unsigned __int64 _p;
#define DPMULD(x,y,z,s) _p = __emul(x,y); z=(I)_p; if((_p+0x80000000U)>0xFFFFFFFFU){s}
#else
#define DPMULDECLS
#define DPMUL(x,y,z,s) if(__builtin_smull_overflow(x,y,z)){s}
#define DPMULDDECLS
#define DPMULD(x,y,z,s) if(__builtin_smull_overflow(x,y,&z)){s}
#endif
#else // C_USEMULTINTRINSIC 0 - use standard-C version (32-bit)
#define DPMULDECLS D _p;
#define DPMUL(x,y,z,s) _p = (D)x*(D)y; *z=(I)_p; if(_p>IMAX||_p<IMIN){s}
#define DPMULDDECLS D _p;
#define DPMULD(x,y,z,s) _p = (D)x*(D)y; z=(I)_p; if(_p>IMAX||_p<IMIN){s}
#endif
#define DPMULDZ(x,y,z) DPMULD(x,y,z,z=0;)
#define DPMULDE(x,y,z) RE(z=mult(x,y))

#endif
// end of multiply builtins

// define single+double-precision integer add
#if SY_64
#if defined(MMSC_VER)  // SY_WIN32
#define SPDPADD(addend, sumlo, sumhi) {C c; c=_addcarry_u64(0,addend,sumlo,&sumlo); _addcarry_u64(c,0,sumhi,&sumhi);}
#endif
#endif

#ifndef SPDPADD   // default version for systems without addcarry
#define SPDPADD(addend, sumlo, sumhi) sumlo += addend; sumhi += (addend > sumlo);
#endif
// end of addition builtins

// aligned memory allocation, assume align is power of 2
static __forceinline void* aligned_malloc(size_t size, size_t align) {
 void *result;
 align = (align>=sizeof(void*))?align:sizeof(void*);
#ifdef _WIN32
 result = _aligned_malloc(size, align);
#elif ( !defined(ANDROID) || defined(__LP64__) )
 if(posix_memalign(&result, align, size)) result = 0;
#else
 void *mem = malloc(size+(align-1)+sizeof(void*));
 if(mem){
  result = (void*)((uintptr_t)(mem+(align-1)+sizeof(void*)) & ~(align-1));
  ((void**)result)[-1] = mem;
 } else result = 0;
#endif
 return result;
}

static __forceinline void aligned_free(void *ptr) {
#ifdef _WIN32
 _aligned_free(ptr);
#elif ( !defined(ANDROID) || defined(__LP64__) )
 free(ptr);
#else
 free(((void**)ptr)[-1]);
#endif
}

// Create (x&y) where x and y are signed, so we can test for overflow.
#if defined(MMSC_VER)  // SY_WIN32
#define XANDY(x,y) ((x)&(y))
#else
#define XANDY(x,y) ((I)((UI)(x)&(UI)(y)))
#endif

// Supported in architecture ARMv8.1 and later
#if (C_CRC32C && (defined(__aarch64__)||defined(_M_ARM64)))
#define CRC32CW(crc, value) __asm__("crc32cw %w[c], %w[c], %w[v]":[c]"+r"(crc):[v]"r"(value))
#define CRC32CX(crc, value) __asm__("crc32cx %w[c], %w[c], %x[v]":[c]"+r"(crc):[v]"r"(value))
#define CRC32(crc,value)  ({ uint32_t crci=crc; CRC32CW(crci, value); crci; })
#define CRC32L(crc,value) ({ uint64_t crci=crc; CRC32CX(crci, value); crci; })
#define CRC32LL CRC32L                 // takes UIL (8 bytes), return UI
#endif

// The following definitions are used only in builds for the AVX instruction set
// 64-bit Atom cpu in android has hardware crc32c but not AVX
#if C_CRC32C && (defined(_M_X64) || defined(__x86_64__))
#if C_AVX || defined(ANDROID)
#if defined(MMSC_VER)  // SY_WIN32
// Visual Studio definitions
#define CRC32(x,y) _mm_crc32_u32(x,y)  // takes UI4, returns UI4
#define CRC32L(x,y) _mm_crc32_u64(x,y)  // takes UI, returns UI (top 32 bits 0)
#else
// gcc/clang definition
#define CRC32(x,y) __builtin_ia32_crc32si(x,y)  // returns UI4
#define CRC32L(x,y) __builtin_ia32_crc32di(x,y)  // returns UI
#endif
#elif EMU_AVX
extern uint64_t crc32csb8(uint64_t crc, uint64_t value);
extern uint32_t crc32csb4(uint32_t crc, uint32_t value);
#define CRC32(x,y)  crc32csb4(x,y) // returns UI4
#define CRC32L(x,y) crc32csb8(x,y) // returns UI
#endif
#define CRC32LL CRC32L                 // takes UIL (8 bytes), return UI
#endif
