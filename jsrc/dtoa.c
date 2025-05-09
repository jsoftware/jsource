/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* The author of this software is David M. Gay.                            */
/* Jsoftware Copyright applies only to changes made by Jsoftware           */

/****************************************************************
 *
 * The author of this software is David M. Gay.
 *
 * Copyright (c) 1991, 2000, 2001 by Lucent Technologies.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHOR NOR LUCENT MAKES ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
 * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 *
 ***************************************************************/

/* Please send bug reports to David M. Gay (dmg at acm dot org,
 * with " at " changed at "@" and " dot " changed to "."). */

/* On a machine with IEEE extended-precision registers, it is
 * necessary to specify double-precision (53-bit) rounding precision
 * before invoking strtod or dtoa.  If the machine uses (the equivalent
 * of) Intel 80x87 arithmetic, the call
 * _control87(PC_53, MCW_PC);
 * does this with many compilers.  Whether this or another call is
 * appropriate depends on the compiler; for this to work, it may be
 * necessary to #include "float.h" or another system-dependent header
 * file.
 */

/* strtod for IEEE-, VAX-, and IBM-arithmetic machines.
 *
 * This strtod returns a nearest machine number to the input decimal
 * string (or sets errno to ERANGE).  With IEEE arithmetic, ties are
 * broken by the IEEE round-even rule.  Otherwise ties are broken by
 * biased rounding (add half and chop).
 *
 * Inspired loosely by William D. Clinger's paper "How to Read Floating
 * Point Numbers Accurately" [Proc. ACM SIGPLAN '90, pp. 92-101].
 *
 * Modifications:
 *
 * 1. We only require IEEE, IBM, or VAX double-precision
 *  arithmetic (not IEEE double-extended).
 * 2. We get by with floating-point arithmetic in a case that
 *  Clinger missed -- when we're computing d * 10^n
 *  for a small integer d and the integer n is not too
 *  much larger than 22 (the maximum integer k for which
 *  we can represent 10^k exactly), we may be able to
 *  compute (d*10^k) * 10^(e-k) with just one roundoff.
 * 3. Rather than a bit-at-a-time adjustment of the binary
 *  result in the hard case, we use floating-point
 *  arithmetic to determine the adjustment to within
 *  one bit; only in really hard cases do we need to
 *  compute a second residual.
 * 4. Because of 3., we don't need a large table of powers of 10
 *  for ten-to-e (just some small tables, e.g. of 10^k
 *  for 0 <= k <= 22).
 */

/*
 * #define IEEE_8087 for IEEE-arithmetic machines where the least
 * significant byte has the lowest address.
 * #define IEEE_MC68k for IEEE-arithmetic machines where the most
 * significant byte has the lowest address.
 * #define Long int on machines with 32-bit ints and 64-bit longs.
 * #define IBM for IBM mainframe-style floating-point arithmetic.
 * #define VAX for VAX-style floating-point arithmetic (D_floating).
 * #define No_leftright to omit left-right logic in fast floating-point
 * computation of dtoa.
 * #define Honor_FLT_ROUNDS if FLT_ROUNDS can assume the values 2 or 3
 * and strtod and dtoa should round accordingly.
 * #define Check_FLT_ROUNDS if FLT_ROUNDS can assume the values 2 or 3
 * and Honor_FLT_ROUNDS is not #defined.
 * #define RND_PRODQUOT to use rnd_prod and rnd_quot (assembly routines
 * that use extended-precision instructions to compute rounded
 * products and quotients) with IBM.
 * #define ROUND_BIASED for IEEE-format with biased rounding.
 * #define Inaccurate_Divide for IEEE-format with correctly rounded
 * products but inaccurate quotients, e.g., for Intel i860.
 * #define NO_LONG_LONG on machines that do not have a "long long"
 * integer type (of >= 64 bits).  On such machines, you can
 * #define Just_16 to store 16 bits per 32-bit Long when doing
 * high-precision integer arithmetic.  Whether this speeds things
 * up or slows things down depends on the machine and the number
 * being converted.  If long long is available and the name is
 * something other than "long long", #define Llong to be the name,
 * and if "unsigned Llong" does not work as an unsigned version of
 * Llong, #define #ULLong to be the corresponding unsigned type.
 * #define KR_headers for old-style C function headers.
 * #define Bad_float_h if your system lacks a float.h or if it does not
 * define some or all of DBL_DIG, DBL_MAX_10_EXP, DBL_MAX_EXP,
 * FLT_RADIX, FLT_ROUNDS, and DBL_MAX.
 * #define MALLOC your_malloc, where your_malloc(n) acts like malloc(n)
 * if memory is available and otherwise does something you deem
 * appropriate.  If MALLOC is undefined, malloc will be invoked
 * directly -- and assumed always to succeed.
 * #define Omit_Private_Memory to omit logic (added Jan. 1998) for making
 * memory allocations from a private pool of memory when possible.
 * When used, the private pool is PRIVATE_MEM bytes long:  2304 bytes,
 * unless #defined to be a different length.  This default length
 * suffices to get rid of MALLOC calls except for unusual cases,
 * such as decimal-to-binary conversion of a very long string of
 * digits.  The longest string dtoa can return is about 751 bytes
 * long.  For conversions by strtod of strings of 800 digits and
 * all dtoa conversions in single-threaded executions with 8-byte
 * pointers, PRIVATE_MEM >= 7400 appears to suffice; with 4-byte
 * pointers, PRIVATE_MEM >= 7112 appears adequate.
 * #define INFNAN_CHECK on IEEE systems to cause strtod to check for
 * Infinity and NaN (case insensitively).  On some systems (e.g.,
 * some HP systems), it may be necessary to #define NAN_WORD0
 * appropriately -- to the most significant word of a quiet NaN.
 * (On HP Series 700/800 machines, -DNAN_WORD0=0x7ff40000 works.)
 * When INFNAN_CHECK is #defined and No_Hex_NaN is not #defined,
 * strtod also accepts (case insensitively) strings of the form
 * NaN(x), where x is a string of hexadecimal digits and spaces;
 * if there is only one string of hexadecimal digits, it is taken
 * for the 52 fraction bits of the resulting NaN; if there are two
 * or more strings of hex digits, the first is for the high 20 bits,
 * the second and subsequent for the low 32 bits, with intervening
 * white space ignored; but if this results in none of the 52
 * fraction bits being on (an IEEE Infinity symbol), then NAN_WORD0
 * and NAN_WORD1 are used instead.
 * #define MULTIPLE_THREADS if the system offers preemptively scheduled
 * multiple threads.  In this case, you must provide (or suitably
 * #define) two locks, acquired by ACQUIRE_DTOA_LOCK(n) and freed
 * by FREE_DTOA_LOCK(n) for n = 0 or 1.  (The second lock, accessed
 * in pow5mult, ensures lazy evaluation of only one copy of high
 * powers of 5; omitting this lock would introduce a small
 * probability of wasting memory, but would otherwise be harmless.)
 * You must also invoke freedtoa(s) to free the value s returned by
 * dtoa.  You may do so whether or not MULTIPLE_THREADS is #defined.
 * #define NO_IEEE_Scale to disable new (Feb. 1997) logic in strtod that
 * avoids underflows on inputs whose result does not underflow.
 * If you #define NO_IEEE_Scale on a machine that uses IEEE-format
 * floating-point numbers and flushes underflows to zero rather
 * than implementing gradual underflow, then you must also #define
 * Sudden_Underflow.
 * #define YES_ALIAS to permit aliasing certain double values with
 * arrays of ULongs.  This leads to slightly better code with
 * some compilers and was always used prior to 19990916, but it
 * is not strictly legal and can cause trouble with aggressively
 * optimizing compilers (e.g., gcc 2.95.1 under -O2).
 * #define USE_LOCALE to use the current locale's decimal_point value.
 * #define SET_INEXACT if IEEE arithmetic is being used and extra
 * computation should be done to set the inexact flag when the
 * result is inexact and avoid setting inexact when the result
 * is exact.  In this case, dtoa.c must be compiled in
 * an environment, perhaps provided by #include "dtoa.c" in a
 * suitable wrapper, that defines two functions,
 *  int get_inexact(void);
 *  void clear_inexact(void);
 * such that get_inexact() returns a nonzero value if the
 * inexact bit is already set, and clear_inexact() sets the
 * inexact bit to 0.  When SET_INEXACT is #defined, strtod
 * also does extra computations to set the underflow and overflow
 * flags when appropriate (i.e., when the result is tiny and
 * inexact or when it is a numeric value rounded to +-infinity).
 * #define NO_ERRNO if strtod should not assign errno = ERANGE when
 * the result overflows to +-Infinity or underflows to 0.
 */

/* Options for use with J */
#include "js.h"
#define Long int
#if C_LE
#define IEEE_8087
#else
#define IEEE_MC68k
#endif
#define MULTIPLE_THREADS
#define ACQUIRE_DTOA_LOCK(n) /* handled by using jt */
#define FREE_DTOA_LOCK(n)    /* handled by using jt */
/* #define Omit_Private_Memory */
#define PRIVATE_MEM 8000
#define Use_J_Memory
#if SY_WIN32
#define Llong __int64
#endif

#ifndef Long
#define Long long
#endif
#ifndef ULong
typedef unsigned Long ULong;
#endif

#ifdef DEBUG
#include "stdio.h"
#define Bug(x) {fprintf(stderr, "%s\n", x); exit(1);}
#endif

#include "stdlib.h"
#include "string.h"

#ifdef USE_LOCALE
#include "locale.h"
#endif

#ifndef Omit_Private_Memory
#ifndef PRIVATE_MEM
#define PRIVATE_MEM 2304
#endif
#define PRIVATE_mem (long)((PRIVATE_MEM+sizeof(double)-1)/sizeof(double))
#ifndef Use_J_Memory
static double private_mem[PRIVATE_mem], *pmem_next = private_mem;
#endif
#endif

#undef IEEE_Arith
#undef Avoid_Underflow
#ifdef IEEE_MC68k
#define IEEE_Arith
#endif
#ifdef IEEE_8087
#define IEEE_Arith
#endif

#ifdef Bad_float_h

#ifdef IEEE_Arith
#define DBL_DIG 15
#define DBL_MAX_10_EXP 308
#define DBL_MAX_EXP 1024
#define FLT_RADIX 2
#endif /*IEEE_Arith*/

#ifdef IBM
#define DBL_DIG 16
#define DBL_MAX_10_EXP 75
#define DBL_MAX_EXP 63
#define FLT_RADIX 16
#define DBL_MAX 7.2370055773322621e+75
#endif

#ifdef VAX
#define DBL_DIG 16
#define DBL_MAX_10_EXP 38
#define DBL_MAX_EXP 127
#define FLT_RADIX 2
#define DBL_MAX 1.7014118346046923e+38
#endif

#ifndef LONG_MAX
#define LONG_MAX 2147483647
#endif

#else /* ifndef Bad_float_h */
#include "float.h"
#endif /* Bad_float_h */

#ifndef __MATH_H__
#include "math.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONSTANT
#ifdef KR_headers
#define CONSTANT /* blank */
#else
#define CONSTANT const
#endif
#endif

#if defined(IEEE_8087) + defined(IEEE_MC68k) + defined(VAX) + defined(IBM) != 1
Exactly one of IEEE_8087, IEEE_MC68k, VAX, or IBM should be defined.
#endif

typedef union { double d; ULong L[2]; } U;

#ifdef YES_ALIAS
#define dval(x) x
#ifdef IEEE_8087
#define word0(x) ((ULong *)&x)[1]
#define word1(x) ((ULong *)&x)[0]
#else
#define word0(x) ((ULong *)&x)[0]
#define word1(x) ((ULong *)&x)[1]
#endif
#else
#ifdef IEEE_8087
#define word0(x) ((U*)&x)->L[1]
#define word1(x) ((U*)&x)->L[0]
#else
#define word0(x) ((U*)&x)->L[0]
#define word1(x) ((U*)&x)->L[1]
#endif
#define dval(x) ((U*)&x)->d
#endif

/* The following definition of Storeinc is appropriate for MIPS processors.
 * An alternative that might be better on some machines is
 * #define Storeinc(a,b,c) (*a++ = b << 16 | c & 0xffff)
 */
#if defined(IEEE_8087) + defined(VAX)
#define Storeinc(a,b,c) (((unsigned short *)a)[1] = (unsigned short)b, \
((unsigned short *)a)[0] = (unsigned short)c, a++)
#else
#define Storeinc(a,b,c) (((unsigned short *)a)[0] = (unsigned short)b, \
((unsigned short *)a)[1] = (unsigned short)c, a++)
#endif

/* #define P DBL_MANT_DIG */
/* Ten_pmax = floor(P*log(2)/log(5)) */
/* Bletch = (highest power of 2 < DBL_MAX_10_EXP) / 16 */
/* Quick_max = floor((P-1)*log(FLT_RADIX)/log(10) - 1) */
/* Int_max = floor(P*log(FLT_RADIX)/log(10) - 1) */

#ifdef IEEE_Arith
#define Exp_shift  20
#define Exp_shift1 20
#define Exp_msk1    0x100000
#define Exp_msk11   0x100000
#define Exp_mask  0x7ff00000
#define P 53
#define Bias 1023
#define Emin (-1022)
#define Exp_1  0x3ff00000
#define Exp_11 0x3ff00000
#define Ebits 11
#define Frac_mask  0xfffff
#define Frac_mask1 0xfffff
#define Ten_pmax 22
#define Bletch 0x10
#define Bndry_mask  0xfffff
#define Bndry_mask1 0xfffff
#define LSB 1
#define Sign_bit 0x80000000
#define Log2P 1
#define Tiny0 0
#define Tiny1 1
#define Quick_max 14
#define Int_max 14
#ifndef NO_IEEE_Scale
#define Avoid_Underflow
#ifdef Flush_Denorm /* debugging option */
#undef Sudden_Underflow
#endif
#endif

#ifndef Flt_Rounds
#ifdef FLT_ROUNDS
#define Flt_Rounds FLT_ROUNDS
#else
#define Flt_Rounds 1
#endif
#endif /*Flt_Rounds*/

#ifdef Honor_FLT_ROUNDS
#define Rounding rounding
#undef Check_FLT_ROUNDS
#define Check_FLT_ROUNDS
#else
#define Rounding Flt_Rounds
#endif

#else /* ifndef IEEE_Arith */
#undef Check_FLT_ROUNDS
#undef Honor_FLT_ROUNDS
#undef SET_INEXACT
#undef  Sudden_Underflow
#define Sudden_Underflow
#ifdef IBM
#undef Flt_Rounds
#define Flt_Rounds 0
#define Exp_shift  24
#define Exp_shift1 24
#define Exp_msk1   0x1000000
#define Exp_msk11  0x1000000
#define Exp_mask  0x7f000000
#define P 14
#define Bias 65
#define Exp_1  0x41000000
#define Exp_11 0x41000000
#define Ebits 8 /* exponent has 7 bits, but 8 is the right value in b2d */
#define Frac_mask  0xffffff
#define Frac_mask1 0xffffff
#define Bletch 4
#define Ten_pmax 22
#define Bndry_mask  0xefffff
#define Bndry_mask1 0xffffff
#define LSB 1
#define Sign_bit 0x80000000
#define Log2P 4
#define Tiny0 0x100000
#define Tiny1 0
#define Quick_max 14
#define Int_max 15
#else /* VAX */
#undef Flt_Rounds
#define Flt_Rounds 1
#define Exp_shift  23
#define Exp_shift1 7
#define Exp_msk1    0x80
#define Exp_msk11   0x800000
#define Exp_mask  0x7f80
#define P 56
#define Bias 129
#define Exp_1  0x40800000
#define Exp_11 0x4080
#define Ebits 8
#define Frac_mask  0x7fffff
#define Frac_mask1 0xffff007f
#define Ten_pmax 24
#define Bletch 2
#define Bndry_mask  0xffff007f
#define Bndry_mask1 0xffff007f
#define LSB 0x10000
#define Sign_bit 0x8000
#define Log2P 1
#define Tiny0 0x80
#define Tiny1 0
#define Quick_max 15
#define Int_max 15
#endif /* IBM, VAX */
#endif /* IEEE_Arith */

#ifndef IEEE_Arith
#define ROUND_BIASED
#endif

#ifdef RND_PRODQUOT
#define rounded_product(a,b) a = rnd_prod(a, b)
#define rounded_quotient(a,b) a = rnd_quot(a, b)
#ifdef KR_headers
extern double rnd_prod(), rnd_quot();
#else
extern double rnd_prod(double, double), rnd_quot(double, double);
#endif
#else
#define rounded_product(a,b) a *= b
#define rounded_quotient(a,b) a /= b
#endif

#define Big0 (Frac_mask1 | Exp_msk1*(DBL_MAX_EXP+Bias-1))
#define Big1 0xffffffff

#ifndef Pack_32
#define Pack_32
#endif

#ifdef KR_headers
#define FFFFFFFF ((((unsigned long)0xffff)<<16)|(unsigned long)0xffff)
#else
#define FFFFFFFF 0xffffffffUL
#endif

#ifdef NO_LONG_LONG
#undef ULLong
#ifdef Just_16
#undef Pack_32
/* When Pack_32 is not defined, we store 16 bits per 32-bit Long.
 * This makes some inner loops simpler and sometimes saves work
 * during multiplications, but it often seems to make things slightly
 * slower.  Hence the default is now to store 32 bits per Long.
 */
#endif
#else /* long long available */
#ifndef Llong
#define Llong long long
#endif
#ifndef ULLong
#define ULLong unsigned Llong
#endif
#endif /* NO_LONG_LONG */

#ifndef MULTIPLE_THREADS
#define ACQUIRE_DTOA_LOCK(n) /*nothing*/
#define FREE_DTOA_LOCK(n) /*nothing*/
#endif

#define Kmax 15

 struct
Bigint {
 struct Bigint *next;
 int k, maxwds, sign, wds;
 ULong x[1];
 };

 typedef struct Bigint Bigint;
#define HAVE_BIGINT
#include "dtoa.h"
 
#ifndef Use_J_Memory
Error!  Code requires Use_J_Memory due to modifications.
#endif

static void *d2a_Malloc(struct dtoa_info *d2a, int k);
static Bigint *d2a_Balloc(struct dtoa_info *d2a, int k);
static void d2a_Bfree(struct dtoa_info *d2a, Bigint *v);

#define p5s         (d2a->_p5s)
#define private_mem (d2a->_private_mem)
#define pmem_next   (d2a->_pmem_next)
#define freelist    (d2a->_freelist)

#define MALLOC(n) d2a_Malloc(d2a, n)
#define Balloc(k)      d2a_Balloc(d2a, k)
#define Bfree(v)       d2a_Bfree(d2a, v)
#define multadd(b,m,a) d2a_multadd(d2a, b, m, a)
#define i2b(i)         d2a_i2b(d2a, i)
#define mult(a,b)      d2a_mult(d2a, a, b)
#define nrv_alloc(s,rve,n) d2a_nrv_alloc(d2a,s,rve,n)
#define rv_alloc(n) d2a_rv_alloc(d2a, n)
#define freedtoa(x) d2a_freedtoa(d2a, x)
#define d2b(d,e,b)  d2a_d2b(d2a,d,e,b)
#define pow5mult(b,k) d2a_pow5mult(d2a,b,k)
#define diff(x,y) d2a_diff(d2a,x,y)
#define lshift(x,y) d2a_lshift(d2a,x,y)

/* static Bigint *freelist[Kmax+1]; */

 static Bigint *
d2a_Balloc
#ifdef KR_headers
 (k) int k;
#else
 (struct dtoa_info *d2a, int k)
#endif
{
 int x;
 Bigint *rv;
#ifndef Omit_Private_Memory
 unsigned int len;
#endif

 ACQUIRE_DTOA_LOCK(0);
 if (rv = freelist[k]) {
  freelist[k] = rv->next;
  }
 else {
  x = 1 << k;
#ifdef Omit_Private_Memory
  rv = (Bigint *)MALLOC(sizeof(Bigint) + (x-1)*sizeof(ULong));
#else
  len = (sizeof(Bigint) + (x-1)*sizeof(ULong) + sizeof(double) - 1)
   /sizeof(double);
  if (pmem_next - private_mem + len <= PRIVATE_mem) {
   rv = (Bigint*)pmem_next;
   pmem_next += len;
   }
  else
   rv = (Bigint*)MALLOC(len*sizeof(double));
#endif
  rv->k = k;
  rv->maxwds = x;
  }
 FREE_DTOA_LOCK(0);
 rv->sign = rv->wds = 0;
 return rv;
 }

 static void
d2a_Bfree
#ifdef KR_headers
 (v) Bigint *v;
#else
 (struct dtoa_info *d2a, Bigint *v)
#endif
{
 if (v) {
  ACQUIRE_DTOA_LOCK(0);
  v->next = freelist[v->k];
  freelist[v->k] = v;
  FREE_DTOA_LOCK(0);
  }
 }

#define Bcopy(x,y) memcpy((char *)&x->sign, (char *)&y->sign, \
y->wds*sizeof(Long) + 2*sizeof(int))

 static Bigint *
d2a_multadd
#ifdef KR_headers
 (b, m, a) Bigint *b; int m, a;
#else
 (struct dtoa_info *d2a, Bigint *b, int m, int a) /* multiply by m and add a */
#endif
{
 int i, wds;
#ifdef ULLong
 ULong *x;
 ULLong carry, y;
#else
 ULong carry, *x, y;
#ifdef Pack_32
 ULong xi, z;
#endif
#endif
 Bigint *b1;

 wds = b->wds;
 x = b->x;
 i = 0;
 carry = a;
 do {
#ifdef ULLong
  y = *x * (ULLong)m + carry;
  carry = y >> 32;
  *x++ = (ULong)(y & FFFFFFFF);
#else
#ifdef Pack_32
  xi = *x;
  y = (xi & 0xffff) * m + carry;
  z = (xi >> 16) * m + (y >> 16);
  carry = z >> 16;
  *x++ = (z << 16) + (y & 0xffff);
#else
  y = *x * m + carry;
  carry = y >> 16;
  *x++ = y & 0xffff;
#endif
#endif
  }
  while(++i < wds);
 if (carry) {
  if (wds >= b->maxwds) {
   b1 = Balloc(b->k+1);
   Bcopy(b1, b);
   Bfree(b);
   b = b1;
   }
  b->x[wds++] = (ULong)carry;
  b->wds = wds;
  }
 return b;
 }



 static int
hi0bits
#ifdef KR_headers
 (x) register ULong x;
#else
 (register ULong x)
#endif
{
 register int k = 0;

 if (!(x & 0xffff0000)) {
  k = 16;
  x <<= 16;
  }
 if (!(x & 0xff000000)) {
  k += 8;
  x <<= 8;
  }
 if (!(x & 0xf0000000)) {
  k += 4;
  x <<= 4;
  }
 if (!(x & 0xc0000000)) {
  k += 2;
  x <<= 2;
  }
 if (!(x & 0x80000000)) {
  k++;
  if (!(x & 0x40000000))
   return 32;
  }
 return k;
 }

 static int
lo0bits
#ifdef KR_headers
 (y) ULong *y;
#else
 (ULong *y)
#endif
{
 register int k;
 register ULong x = *y;

 if (x & 7) {
  if (x & 1)
   return 0;
  if (x & 2) {
   *y = x >> 1;
   return 1;
   }
  *y = x >> 2;
  return 2;
  }
 k = 0;
 if (!(x & 0xffff)) {
  k = 16;
  x >>= 16;
  }
 if (!(x & 0xff)) {
  k += 8;
  x >>= 8;
  }
 if (!(x & 0xf)) {
  k += 4;
  x >>= 4;
  }
 if (!(x & 0x3)) {
  k += 2;
  x >>= 2;
  }
 if (!(x & 1)) {
  k++;
  x >>= 1;
  if (!x)
   return 32;
  }
 *y = x;
 return k;
 }

 static Bigint *
d2a_i2b
#ifdef KR_headers
 (i) int i;
#else
 (struct dtoa_info *d2a, int i)
#endif
{
 Bigint *b;

 b = Balloc(1);
 b->x[0] = i;
 b->wds = 1;
 return b;
 }

 static Bigint *
d2a_mult
#ifdef KR_headers
 (a, b) Bigint *a, *b;
#else
 (struct dtoa_info *d2a, Bigint *a, Bigint *b)
#endif
{
 Bigint *c;
 int k, wa, wb, wc;
 ULong *x, *xa, *xae, *xb, *xbe, *xc, *xc0;
 ULong y;
#ifdef ULLong
 ULLong carry, z;
#else
 ULong carry, z;
#ifdef Pack_32
 ULong z2;
#endif
#endif

 if (a->wds < b->wds) {
  c = a;
  a = b;
  b = c;
  }
 k = a->k;
 wa = a->wds;
 wb = b->wds;
 wc = wa + wb;
 if (wc > a->maxwds)
  k++;
 c = Balloc(k);
 for(x = c->x, xa = x + wc; x < xa; x++)
  *x = 0;
 xa = a->x;
 xae = xa + wa;
 xb = b->x;
 xbe = xb + wb;
 xc0 = c->x;
#ifdef ULLong
 for(; xb < xbe; xc0++) {
  if (y = *xb++) {
   x = xa;
   xc = xc0;
   carry = 0;
   do {
    z = *x++ * (ULLong)y + *xc + carry;
    carry = z >> 32;
    *xc++ = (ULong)(z & FFFFFFFF);
    }
    while(x < xae);
   *xc = (ULong)carry;
   }
  }
#else
#ifdef Pack_32
 for(; xb < xbe; xb++, xc0++) {
  if (y = *xb & 0xffff) {
   x = xa;
   xc = xc0;
   carry = 0;
   do {
    z = (*x & 0xffff) * y + (*xc & 0xffff) + carry;
    carry = z >> 16;
    z2 = (*x++ >> 16) * y + (*xc >> 16) + carry;
    carry = z2 >> 16;
    Storeinc(xc, z2, z);
    }
    while(x < xae);
   *xc = carry;
   }
  if (y = *xb >> 16) {
   x = xa;
   xc = xc0;
   carry = 0;
   z2 = *xc;
   do {
    z = (*x & 0xffff) * y + (*xc >> 16) + carry;
    carry = z >> 16;
    Storeinc(xc, z, z2);
    z2 = (*x++ >> 16) * y + (*xc & 0xffff) + carry;
    carry = z2 >> 16;
    }
    while(x < xae);
   *xc = z2;
   }
  }
#else
 for(; xb < xbe; xc0++) {
  if (y = *xb++) {
   x = xa;
   xc = xc0;
   carry = 0;
   do {
    z = *x++ * y + *xc + carry;
    carry = z >> 16;
    *xc++ = z & 0xffff;
    }
    while(x < xae);
   *xc = carry;
   }
  }
#endif
#endif
 for(xc0 = c->x, xc = xc0 + wc; wc > 0 && !*--xc; --wc) ;
 c->wds = wc;
 return c;
 }

#ifndef Use_J_Memory
 static Bigint *p5s;
#endif

 static Bigint *
d2a_pow5mult
#ifdef KR_headers
 (b, k) Bigint *b; int k;
#else
 (struct dtoa_info *d2a, Bigint *b, int k)
#endif
{
 Bigint *b1, *p5, *p51;
 int i;
 static const int p05[3] = { 5, 25, 125 };

 if (i = k & 3)
  b = multadd(b, p05[i-1], 0);

 if (!(k >>= 2))
  return b;
 if (!(p5 = p5s)) {
  /* first time */
#ifdef MULTIPLE_THREADS
  ACQUIRE_DTOA_LOCK(1);
  if (!(p5 = p5s)) {
   p5 = p5s = i2b(625);
   p5->next = 0;
   }
  FREE_DTOA_LOCK(1);
#else
  p5 = p5s = i2b(625);
  p5->next = 0;
#endif
  }
 for(;;) {
  if (k & 1) {
   b1 = mult(b, p5);
   Bfree(b);
   b = b1;
   }
  if (!(k >>= 1))
   break;
  if (!(p51 = p5->next)) {
#ifdef MULTIPLE_THREADS
   ACQUIRE_DTOA_LOCK(1);
   if (!(p51 = p5->next)) {
    p51 = p5->next = mult(p5,p5);
    p51->next = 0;
    }
   FREE_DTOA_LOCK(1);
#else
   p51 = p5->next = mult(p5,p5);
   p51->next = 0;
#endif
   }
  p5 = p51;
  }
 return b;
 }

 static Bigint *
d2a_lshift
#ifdef KR_headers
 (b, k) Bigint *b; int k;
#else
 (struct dtoa_info *d2a, Bigint *b, int k)
#endif
{
 int i, k1, n, n1;
 Bigint *b1;
 ULong *x, *x1, *xe, z;

#ifdef Pack_32
 n = k >> 5;
#else
 n = k >> 4;
#endif
 k1 = b->k;
 n1 = n + b->wds + 1;
 for(i = b->maxwds; n1 > i; i <<= 1)
  k1++;
 b1 = Balloc(k1);
 x1 = b1->x;
 for(i = 0; i < n; i++)
  *x1++ = 0;
 x = b->x;
 xe = x + b->wds;
#ifdef Pack_32
 if (k &= 0x1f) {
  k1 = 32 - k;
  z = 0;
  do {
   *x1++ = *x << k | z;
   z = *x++ >> k1;
   }
   while(x < xe);
  if (*x1 = z)
   ++n1;
  }
#else
 if (k &= 0xf) {
  k1 = 16 - k;
  z = 0;
  do {
   *x1++ = *x << k  & 0xffff | z;
   z = *x++ >> k1;
   }
   while(x < xe);
  if (*x1 = z)
   ++n1;
  }
#endif
 else do
  *x1++ = *x++;
  while(x < xe);
 b1->wds = n1 - 1;
 Bfree(b);
 return b1;
 }

 static int
cmp
#ifdef KR_headers
 (a, b) Bigint *a, *b;
#else
 (Bigint *a, Bigint *b)
#endif
{
 ULong *xa, *xa0, *xb, *xb0;
 int i, j;

 i = a->wds;
 j = b->wds;
#ifdef DEBUG
 if (i > 1 && !a->x[i-1])
  Bug("cmp called with a->x[a->wds-1] == 0");
 if (j > 1 && !b->x[j-1])
  Bug("cmp called with b->x[b->wds-1] == 0");
#endif
 if (i -= j)
  return i;
 xa0 = a->x;
 xa = xa0 + j;
 xb0 = b->x;
 xb = xb0 + j;
 for(;;) {
  if (*--xa != *--xb)
   return *xa < *xb ? -1 : 1;
  if (xa <= xa0)
   break;
  }
 return 0;
 }

 static Bigint *
d2a_diff
#ifdef KR_headers
 (a, b) Bigint *a, *b;
#else
 (struct dtoa_info *d2a, Bigint *a, Bigint *b)
#endif
{
 Bigint *c;
 int i, wa, wb;
 ULong *xa, *xae, *xb, *xbe, *xc;
#ifdef ULLong
 ULLong borrow, y;
#else
 ULong borrow, y;
#ifdef Pack_32
 ULong z;
#endif
#endif

 i = cmp(a,b);
 if (!i) {
  c = Balloc(0);
  c->wds = 1;
  c->x[0] = 0;
  return c;
  }
 if (i < 0) {
  c = a;
  a = b;
  b = c;
  i = 1;
  }
 else
  i = 0;
 c = Balloc(a->k);
 c->sign = i;
 wa = a->wds;
 xa = a->x;
 xae = xa + wa;
 wb = b->wds;
 xb = b->x;
 xbe = xb + wb;
 xc = c->x;
 borrow = 0;
#ifdef ULLong
 do {
  y = (ULLong)*xa++ - *xb++ - borrow;
  borrow = y >> 32 & (ULong)1;
  *xc++ = (ULong)(y & FFFFFFFF);
  }
  while(xb < xbe);
 while(xa < xae) {
  y = *xa++ - borrow;
  borrow = y >> 32 & (ULong)1;
  *xc++ = (ULong)(y & FFFFFFFF);
  }
#else
#ifdef Pack_32
 do {
  y = (*xa & 0xffff) - (*xb & 0xffff) - borrow;
  borrow = (y & 0x10000) >> 16;
  z = (*xa++ >> 16) - (*xb++ >> 16) - borrow;
  borrow = (z & 0x10000) >> 16;
  Storeinc(xc, z, y);
  }
  while(xb < xbe);
 while(xa < xae) {
  y = (*xa & 0xffff) - borrow;
  borrow = (y & 0x10000) >> 16;
  z = (*xa++ >> 16) - borrow;
  borrow = (z & 0x10000) >> 16;
  Storeinc(xc, z, y);
  }
#else
 do {
  y = *xa++ - *xb++ - borrow;
  borrow = (y & 0x10000) >> 16;
  *xc++ = y & 0xffff;
  }
  while(xb < xbe);
 while(xa < xae) {
  y = *xa++ - borrow;
  borrow = (y & 0x10000) >> 16;
  *xc++ = y & 0xffff;
  }
#endif
#endif
 while(!*--xc)
  wa--;
 c->wds = wa;
 return c;
 }

 static Bigint *
d2a_d2b
#ifdef KR_headers
 (d, e, bits) double d; int *e, *bits;
#else
 (struct dtoa_info *d2a, double d, int *e, int *bits)
#endif
{
 Bigint *b;
 int de, k;
 ULong *x, y, z;
#ifndef Sudden_Underflow
 int i;
#endif
#ifdef VAX
 ULong d0, d1;
 d0 = word0(d) >> 16 | word0(d) << 16;
 d1 = word1(d) >> 16 | word1(d) << 16;
#else
#define d0 word0(d)
#define d1 word1(d)
#endif

#ifdef Pack_32
 b = Balloc(1);
#else
 b = Balloc(2);
#endif
 x = b->x;

 z = d0 & Frac_mask;
 d0 &= 0x7fffffff; /* clear sign bit, which we ignore */
#ifdef Sudden_Underflow
 de = (int)(d0 >> Exp_shift);
#ifndef IBM
 z |= Exp_msk11;
#endif
#else
 if (de = (int)(d0 >> Exp_shift))
  z |= Exp_msk1;
#endif
#ifdef Pack_32
 if (y = d1) {
  if (k = lo0bits(&y)) {
   x[0] = y | z << (32 - k);
   z >>= k;
   }
  else
   x[0] = y;
#ifndef Sudden_Underflow
  i =
#endif
      b->wds = (x[1] = z) ? 2 : 1;
  }
 else {
#ifdef DEBUG
  if (!z)
   Bug("Zero passed to d2b");
#endif
  k = lo0bits(&z);
  x[0] = z;
#ifndef Sudden_Underflow
  i =
#endif
      b->wds = 1;
  k += 32;
  }
#else
 if (y = d1) {
  if (k = lo0bits(&y))
   if (k >= 16) {
    x[0] = y | z << 32 - k & 0xffff;
    x[1] = z >> k - 16 & 0xffff;
    x[2] = z >> k;
    i = 2;
    }
   else {
    x[0] = y & 0xffff;
    x[1] = y >> 16 | z << 16 - k & 0xffff;
    x[2] = z >> k & 0xffff;
    x[3] = z >> k+16;
    i = 3;
    }
  else {
   x[0] = y & 0xffff;
   x[1] = y >> 16;
   x[2] = z & 0xffff;
   x[3] = z >> 16;
   i = 3;
   }
  }
 else {
#ifdef DEBUG
  if (!z)
   Bug("Zero passed to d2b");
#endif
  k = lo0bits(&z);
  if (k >= 16) {
   x[0] = z;
   i = 0;
   }
  else {
   x[0] = z & 0xffff;
   x[1] = z >> 16;
   i = 1;
   }
  k += 32;
  }
 while(!x[i])
  --i;
 b->wds = i + 1;
#endif
#ifndef Sudden_Underflow
 if (de) {
#endif
#ifdef IBM
  *e = (de - Bias - (P-1) << 2) + k;
  *bits = 4*P + 8 - k - hi0bits(word0(d) & Frac_mask);
#else
  *e = de - Bias - (P-1) + k;
  *bits = P - k;
#endif
#ifndef Sudden_Underflow
  }
 else {
  *e = de - Bias - (P-1) + 1 + k;
#ifdef Pack_32
  *bits = 32*i - hi0bits(x[i-1]);
#else
  *bits = (i+2)*16 - hi0bits(x[i]);
#endif
  }
#endif
 return b;
 }
#undef d0
#undef d1


 static CONSTANT double
tens[] = {
  1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9,
  1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19,
  1e20, 1e21, 1e22
#ifdef VAX
  , 1e23, 1e24
#endif
  };

 static CONSTANT double
#ifdef IEEE_Arith
bigtens[] = { 1e16, 1e32, 1e64, 1e128, 1e256 };
static CONSTANT double tinytens[] = { 1e-16, 1e-32, 1e-64, 1e-128,
#ifdef Avoid_Underflow
  9007199254740992.*9007199254740992.e-256
  /* = 2^106 * 1e-53 */
#else
  1e-256
#endif
  };
/* The factor of 2^53 in tinytens[4] helps us avoid setting the underflow */
/* flag unnecessarily.  It leads to a song and dance at the end of strtod. */
#define Scale_Bit 0x10
#define n_bigtens 5
#else
#ifdef IBM
bigtens[] = { 1e16, 1e32, 1e64 };
static CONSTANT double tinytens[] = { 1e-16, 1e-32, 1e-64 };
#define n_bigtens 3
#else
bigtens[] = { 1e16, 1e32 };
static CONSTANT double tinytens[] = { 1e-16, 1e-32 };
#define n_bigtens 2
#endif
#endif

#ifndef IEEE_Arith
#undef INFNAN_CHECK
#endif

#ifdef INFNAN_CHECK

#ifndef NAN_WORD0
#define NAN_WORD0 0x7ff80000
#endif

#ifndef NAN_WORD1
#define NAN_WORD1 0
#endif

 static int
match
#ifdef KR_headers
 (sp, t) char **sp, *t;
#else
 (CONSTANT char **sp, char *t)
#endif
{
 int c, d;
 CONSTANT char *s = *sp;

 while(d = *t++) {
  if ((c = *++s) >= 'A' && c <= 'Z')
   c += 'a' - 'A';
  if (c != d)
   return 0;
  }
 *sp = s + 1;
 return 1;
 }

#ifndef No_Hex_NaN
 static void
hexnan
#ifdef KR_headers
 (rvp, sp) double *rvp; CONSTANT char **sp;
#else
 (double *rvp, CONSTANT char **sp)
#endif
{
 ULong c, x[2];
 CONSTANT char *s;
 int havedig, udx0, xshift;

 x[0] = x[1] = 0;
 havedig = xshift = 0;
 udx0 = 1;
 s = *sp;
 while(c = *(CONSTANT unsigned char*)++s) {
  if (c >= '0' && c <= '9')
   c -= '0';
  else if (c >= 'a' && c <= 'f')
   c += 10 - 'a';
  else if (c >= 'A' && c <= 'F')
   c += 10 - 'A';
  else if (c <= ' ') {
   if (udx0 && havedig) {
    udx0 = 0;
    xshift = 1;
    }
   continue;
   }
  else if (/*(*/ c == ')' && havedig) {
   *sp = s + 1;
   break;
   }
  else
   return; /* invalid form: don't change *sp */
  havedig = 1;
  if (xshift) {
   xshift = 0;
   x[0] = x[1];
   x[1] = 0;
   }
  if (udx0)
   x[0] = (x[0] << 4) | (x[1] >> 28);
  x[1] = (x[1] << 4) | c;
  }
 if ((x[0] &= 0xfffff) || x[1]) {
  word0(*rvp) = Exp_mask | x[0];
  word1(*rvp) = x[1];
  }
 }
#endif /*No_Hex_NaN*/
#endif /* INFNAN_CHECK */


 static int
quorem
#ifdef KR_headers
 (b, S) Bigint *b, *S;
#else
 (Bigint *b, Bigint *S)
#endif
{
 int n;
 ULong *bx, *bxe, q, *sx, *sxe;
#ifdef ULLong
 ULLong borrow, carry, y, ys;
#else
 ULong borrow, carry, y, ys;
#ifdef Pack_32
 ULong si, z, zs;
#endif
#endif

 n = S->wds;
#ifdef DEBUG
 /*debug*/ if (b->wds > n)
 /*debug*/ Bug("oversize b in quorem");
#endif
 if (b->wds < n)
  return 0;
 sx = S->x;
 sxe = sx + --n;
 bx = b->x;
 bxe = bx + n;
 q = *bxe / (*sxe + 1); /* ensure q <= true quotient */
#ifdef DEBUG
 /*debug*/ if (q > 9)
 /*debug*/ Bug("oversized quotient in quorem");
#endif
 if (q) {
  borrow = 0;
  carry = 0;
  do {
#ifdef ULLong
   ys = *sx++ * (ULLong)q + carry;
   carry = ys >> 32;
   y = *bx - (ys & FFFFFFFF) - borrow;
   borrow = y >> 32 & (ULong)1;
   *bx++ = (ULong)(y & FFFFFFFF);
#else
#ifdef Pack_32
   si = *sx++;
   ys = (si & 0xffff) * q + carry;
   zs = (si >> 16) * q + (ys >> 16);
   carry = zs >> 16;
   y = (*bx & 0xffff) - (ys & 0xffff) - borrow;
   borrow = (y & 0x10000) >> 16;
   z = (*bx >> 16) - (zs & 0xffff) - borrow;
   borrow = (z & 0x10000) >> 16;
   Storeinc(bx, z, y);
#else
   ys = *sx++ * q + carry;
   carry = ys >> 16;
   y = *bx - (ys & 0xffff) - borrow;
   borrow = (y & 0x10000) >> 16;
   *bx++ = y & 0xffff;
#endif
#endif
   }
   while(sx <= sxe);
  if (!*bxe) {
   bx = b->x;
   while(--bxe > bx && !*bxe)
    --n;
   b->wds = n;
   }
  }
 if (cmp(b, S) >= 0) {
  q++;
  borrow = 0;
  carry = 0;
  bx = b->x;
  sx = S->x;
  do {
#ifdef ULLong
   ys = *sx++ + carry;
   carry = ys >> 32;
   y = *bx - (ys & FFFFFFFF) - borrow;
   borrow = y >> 32 & (ULong)1;
   *bx++ = (ULong)(y & FFFFFFFF);
#else
#ifdef Pack_32
   si = *sx++;
   ys = (si & 0xffff) + carry;
   zs = (si >> 16) + (ys >> 16);
   carry = zs >> 16;
   y = (*bx & 0xffff) - (ys & 0xffff) - borrow;
   borrow = (y & 0x10000) >> 16;
   z = (*bx >> 16) - (zs & 0xffff) - borrow;
   borrow = (z & 0x10000) >> 16;
   Storeinc(bx, z, y);
#else
   ys = *sx++ + carry;
   carry = ys >> 16;
   y = *bx - (ys & 0xffff) - borrow;
   borrow = (y & 0x10000) >> 16;
   *bx++ = y & 0xffff;
#endif
#endif
   }
   while(sx <= sxe);
  bx = b->x;
  bxe = bx + n;
  if (!*bxe) {
   while(--bxe > bx && !*bxe)
    --n;
   b->wds = n;
   }
  }
 return q;
 }

#ifndef MULTIPLE_THREADS
 static char *dtoa_result;
#endif

 static char *
#ifdef KR_headers
rv_alloc(i) int i;
#else
d2a_rv_alloc(struct dtoa_info *d2a, int i)
#endif
/* i is guarenteed i >= 0 by caller so the cast to unsigned below is safe */
{
#ifndef Use_J_Memory
 int j, k, *r;

 j = sizeof(ULong);
 for(k = 0;
  sizeof(Bigint) - sizeof(ULong) - sizeof(int) + j <= (unsigned)i;
  j <<= 1)
   k++;
 r = (int*)Balloc(k);
 *r = k;
 return
#ifndef MULTIPLE_THREADS
 dtoa_result =
#endif
  (char *)(r+1);
#else
 if(i>d2a->ndp) longjmp(d2a->_env, 2); /* this shouldn't happen? */
 return d2a->result;
#endif
 }

 static char *
#ifdef KR_headers
nrv_alloc(s, rve, n) char *s, **rve; int n;
#else
d2a_nrv_alloc(struct dtoa_info *d2a, char *s, char **rve, int n)
#endif
{
 char *rv, *t;

 t = rv = rv_alloc(n);
 /*while(*t = *s++) t++;*/
 while(*s){*t++=*s++;} /* don't copy the NUL */
 if (rve)
  *rve = t;
 return rv;
 }

/* dtoa for IEEE arithmetic (dmg): convert double to ASCII string.
 *
 * Inspired by "How to Print Floating-Point Numbers Accurately" by
 * Guy L. Steele, Jr. and Jon L. White [Proc. ACM SIGPLAN '90, pp. 112-126].
 *
 * Modifications:
 * 1. Rather than iterating, we use a simple numeric overestimate
 *    to determine k = floor(log10(d)).  We scale relevant
 *    quantities using O(log2(k)) rather than O(k) multiplications.
 * 2. For some modes > 2 (corresponding to ecvt and fcvt), we don't
 *    try to generate digits strictly left to right.  Instead, we
 *    compute with fewer bits and propagate the carry if necessary
 *    when rounding the final digit up.  This is often faster.
 * 3. Under the assumption that input will be rounded nearest,
 *    mode 0 renders 1e23 as 1e23 rather than 9.999999999999999e22.
 *    That is, we allow equality in stopping tests when the
 *    round-nearest rule will give the same floating-point value
 *    as would satisfaction of the stopping test with strict
 *    inequality.
 * 4. We remove common factors of powers of 2 from relevant
 *    quantities.
 * 5. When converting floating-point integers less than 1e16,
 *    we use floating-point arithmetic rather than resorting
 *    to multiple-precision integers.
 * 6. When asked to produce fewer than 15 digits, we first try
 *    to get by with floating-point arithmetic; we resort to
 *    multiple-precision integer arithmetic only if we cannot
 *    guarantee that the floating-point calculation has given
 *    the correctly rounded result.  For k requested digits and
 *    "uniformly" distributed input, the probability is
 *    something like 10^(k-15) that we must resort to the Long
 *    calculation.
 */

 static char *
d2a_dtoa
#ifdef KR_headers
 (d, mode, ndigits, decpt, sign, rve)
 double d; int mode, ndigits, *decpt, *sign; char **rve;
#else
 (struct dtoa_info *d2a, double d, int mode, int ndigits, int *decpt, int *sign, char **rve)
#endif
{
 /* Arguments ndigits, decpt, sign are similar to those
 of ecvt and fcvt; trailing zeros are suppressed from
 the returned string.  If not null, *rve is set to point
 to the end of the return value.  If d is +-Infinity or NaN,
 then *decpt is set to 9999.

 mode:
  0 ==> shortest string that yields d when read in
   and rounded to nearest.
  1 ==> like 0, but with Steele & White stopping rule;
   e.g. with IEEE P754 arithmetic , mode 0 gives
   1e23 whereas mode 1 gives 9.999999999999999e22.
  2 ==> max(1,ndigits) significant digits.  This gives a
   return value similar to that of ecvt, except
   that trailing zeros are suppressed.
  3 ==> through ndigits past the decimal point.  This
   gives a return value similar to that from fcvt,
   except that trailing zeros are suppressed, and
   ndigits can be negative.
  4,5 ==> similar to 2 and 3, respectively, but (in
   round-nearest mode) with the tests of mode 0 to
   possibly return a shorter string that rounds to d.
   With IEEE arithmetic and compilation with
   -DHonor_FLT_ROUNDS, modes 4 and 5 behave the same
   as modes 2 and 3 when FLT_ROUNDS != 1.
  6-9 ==> Debugging modes similar to mode - 4:  don't try
   fast floating-point estimate (if applicable).

  Values of mode other than 0-9 are treated as mode 0.

  Sufficient space is allocated to the return value
  to hold the suppressed trailing zeros.
 */

 int bbits, b2, b5, be, dig, i, ieps, ilim, ilim0, ilim1,
  j, j1, k, k0, k_check, leftright, m2, m5, s2, s5,
  spec_case, try_quick;
 Long L;
#ifndef Sudden_Underflow
 int denorm;
 ULong x;
#endif
 Bigint *b, *b1, *delta, *mlo, *mhi, *S;
 double d2, ds, eps;
 char *s, *s0;
#ifdef Honor_FLT_ROUNDS
 int rounding;
#endif
#ifdef SET_INEXACT
 int inexact, oldinexact;
#endif

#ifndef MULTIPLE_THREADS
 if (dtoa_result) {
  freedtoa(dtoa_result);
  dtoa_result = 0;
  }
#endif

 if (word0(d) & Sign_bit) {
  /* set sign for everything, including 0's and NaNs */
  *sign = 1;
  word0(d) &= ~Sign_bit; /* clear sign bit */
  }
 else
  *sign = 0;

#if defined(IEEE_Arith) + defined(VAX)
#ifdef IEEE_Arith
 if ((word0(d) & Exp_mask) == Exp_mask)
#else
 if (word0(d)  == 0x8000)
#endif
  {
  /* Infinity or NaN */
  *decpt = 9999;
#ifdef IEEE_Arith
  if (!word1(d) && !(word0(d) & 0xfffff))
   return nrv_alloc("Infinity", rve, 8);
#endif
  return nrv_alloc("NaN", rve, 3);
  }
#endif
#ifdef IBM
 dval(d) += 0; /* normalize */
#endif
 if (!dval(d)) {
  *decpt = 1;
  return nrv_alloc("0", rve, 1);
  }

#ifdef SET_INEXACT
 try_quick = oldinexact = get_inexact();
 inexact = 1;
#endif
#ifdef Honor_FLT_ROUNDS
 if ((rounding = Flt_Rounds) >= 2) {
  if (*sign)
   rounding = rounding == 2 ? 0 : 2;
  else
   if (rounding != 2)
    rounding = 0;
  }
#endif

 b = d2b(dval(d), &be, &bbits);
#ifdef Sudden_Underflow
 i = (int)(word0(d) >> Exp_shift1 & (Exp_mask>>Exp_shift1));
#else
 if (i = (int)(word0(d) >> Exp_shift1 & (Exp_mask>>Exp_shift1))) {
#endif
  dval(d2) = dval(d);
  word0(d2) &= Frac_mask1;
  word0(d2) |= Exp_11;
#ifdef IBM
  if (j = 11 - hi0bits(word0(d2) & Frac_mask))
   dval(d2) /= 1 << j;
#endif

  /* log(x) ~=~ log(1.5) + (x-1.5)/1.5
   * log10(x)  =  log(x) / log(10)
   *  ~=~ log(1.5)/log(10) + (x-1.5)/(1.5*log(10))
   * log10(d) = (i-Bias)*log(2)/log(10) + log10(d2)
   *
   * This suggests computing an approximation k to log10(d) by
   *
   * k = (i - Bias)*0.301029995663981
   * + ( (d2-1.5)*0.289529654602168 + 0.176091259055681 );
   *
   * We want k to be too large rather than too small.
   * The error in the first-order Taylor series approximation
   * is in our favor, so we just round up the constant enough
   * to compensate for any error in the multiplication of
   * (i - Bias) by 0.301029995663981; since |i - Bias| <= 1077,
   * and 1077 * 0.30103 * 2^-52 ~=~ 7.2e-14,
   * adding 1e-13 to the constant term more than suffices.
   * Hence we adjust the constant term to 0.1760912590558.
   * (We could get a more accurate k by invoking log10,
   *  but this is probably not worthwhile.)
   */

  i -= Bias;
#ifdef IBM
  i <<= 2;
  i += j;
#endif
#ifndef Sudden_Underflow
  denorm = 0;
  }
 else {
  /* d is denormalized */

  i = bbits + be + (Bias + (P-1) - 1);
  x = i > 32  ? word0(d) << (64 - i) | word1(d) >> (i - 32)
       : word1(d) << (32 - i);
  dval(d2) = x;
  word0(d2) -= 31*Exp_msk1; /* adjust exponent */
  i -= (Bias + (P-1) - 1) + 1;
  denorm = 1;
  }
#endif
 ds = (dval(d2)-1.5)*0.289529654602168 + 0.1760912590558 + i*0.301029995663981;
 k = (int)ds;
 if (ds < 0. && ds != k)
  k--; /* want k = floor(ds) */
 k_check = 1;
 if (k >= 0 && k <= Ten_pmax) {
  if (dval(d) < tens[k])
   k--;
  k_check = 0;
  }
 j = bbits - i - 1;
 if (j >= 0) {
  b2 = 0;
  s2 = j;
  }
 else {
  b2 = -j;
  s2 = 0;
  }
 if (k >= 0) {
  b5 = 0;
  s5 = k;
  s2 += k;
  }
 else {
  b2 -= k;
  b5 = -k;
  s5 = 0;
  }
 if (mode < 0 || mode > 9)
  mode = 0;

#ifndef SET_INEXACT
#ifdef Check_FLT_ROUNDS
 try_quick = Rounding == 1;
#else
 try_quick = 1;
#endif
#endif /*SET_INEXACT*/

 if (mode > 5) {
  mode -= 4;
  try_quick = 0;
  }
 leftright = 1;
 switch(mode) {
  case 0:
  case 1:
   ilim = ilim1 = -1;
   i = 18;
   ndigits = 0;
   break;
  case 2:
   leftright = 0;
   /* no break */
  case 4:
   if (ndigits <= 0)
    ndigits = 1;
   ilim = ilim1 = i = ndigits;
   break;
  case 3:
   leftright = 0;
   /* no break */
  case 5:
   i = ndigits + k + 1;
   ilim = i;
   ilim1 = i - 1;
   if (i <= 0)
    i = 1;
  }
 s = s0 = rv_alloc(i);

#ifdef Honor_FLT_ROUNDS
 if (mode > 1 && rounding != 1)
  leftright = 0;
#endif

 if (ilim >= 0 && ilim <= Quick_max && try_quick) {

  /* Try to get by with floating-point arithmetic. */

  i = 0;
  dval(d2) = dval(d);
  k0 = k;
  ilim0 = ilim;
  ieps = 2; /* conservative */
  if (k > 0) {
   ds = tens[k&0xf];
   j = k >> 4;
   if (j & Bletch) {
    /* prevent overflows */
    j &= Bletch - 1;
    dval(d) /= bigtens[n_bigtens-1];
    ieps++;
    }
   for(; j; j >>= 1, i++)
    if (j & 1) {
     ieps++;
     ds *= bigtens[i];
     }
   dval(d) /= ds;
   }
  else if (j1 = -k) {
   dval(d) *= tens[j1 & 0xf];
   for(j = j1 >> 4; j; j >>= 1, i++)
    if (j & 1) {
     ieps++;
     dval(d) *= bigtens[i];
     }
   }
  if (k_check && dval(d) < 1. && ilim > 0) {
   if (ilim1 <= 0)
    goto fast_failed;
   ilim = ilim1;
   k--;
   dval(d) *= 10.;
   ieps++;
   }
  dval(eps) = ieps*dval(d) + 7.;
  word0(eps) -= (P-1)*Exp_msk1;
  if (ilim == 0) {
   S = mhi = 0;
   dval(d) -= 5.;
   if (dval(d) > dval(eps))
    goto one_digit;
   if (dval(d) < -dval(eps))
    goto no_digits;
   goto fast_failed;
   }
#ifndef No_leftright
  if (leftright) {
   /* Use Steele & White method of only
    * generating digits needed.
    */
   dval(eps) = 0.5/tens[ilim-1] - dval(eps);
   for(i = 0;;) {
    L = (Long)dval(d);
    dval(d) -= L;
    *s++ = '0' + (int)L;
    if (dval(d) < dval(eps))
     goto ret1;
    if (1. - dval(d) < dval(eps))
     goto bump_up;
    if (++i >= ilim)
     break;
    dval(eps) *= 10.;
    dval(d) *= 10.;
    }
   }
  else {
#endif
   /* Generate ilim digits, then fix them up. */
   dval(eps) *= tens[ilim-1];
   for(i = 1;; i++, dval(d) *= 10.) {
    L = (Long)(dval(d));
    if (!(dval(d) -= L))
     ilim = i;
    *s++ = '0' + (int)L;
    if (i == ilim) {
     if (dval(d) > 0.5 + dval(eps))
      goto bump_up;
     else if (dval(d) < 0.5 - dval(eps)) {
      while(*--s == '0');
      s++;
      goto ret1;
      }
     break;
     }
    }
#ifndef No_leftright
   }
#endif
 fast_failed:
  s = s0;
  dval(d) = dval(d2);
  k = k0;
  ilim = ilim0;
  }

 /* Do we have a "small" integer? */

 if (be >= 0 && k <= Int_max) {
  /* Yes. */
  ds = tens[k];
  if (ndigits < 0 && ilim <= 0) {
   S = mhi = 0;
   if (ilim < 0 || dval(d) <= 5*ds)
    goto no_digits;
   goto one_digit;
   }
  for(i = 1;; i++, dval(d) *= 10.) {
   L = (Long)(dval(d) / ds);
   dval(d) -= L*ds;
#ifdef Check_FLT_ROUNDS
   /* If FLT_ROUNDS == 2, L will usually be high by 1 */
   if (dval(d) < 0) {
    L--;
    dval(d) += ds;
    }
#endif
   *s++ = '0' + (int)L;
   if (!dval(d)) {
#ifdef SET_INEXACT
    inexact = 0;
#endif
    break;
    }
   if (i == ilim) {
#ifdef Honor_FLT_ROUNDS
    if (mode > 1)
    switch(rounding) {
      case 0: goto ret1;
      case 2: goto bump_up;
      }
#endif
    dval(d) += dval(d);
    if (dval(d) > ds || dval(d) == ds && L & 1) {
 bump_up:
     while(*--s == '9')
      if (s == s0) {
       k++;
       *s = '0';
       break;
       }
     ++*s++;
     }
    break;
    }
   }
  goto ret1;
  }

 m2 = b2;
 m5 = b5;
 mhi = mlo = 0;
 if (leftright) {
  i =
#ifndef Sudden_Underflow
   denorm ? be + (Bias + (P-1) - 1 + 1) :
#endif
#ifdef IBM
   1 + 4*P - 3 - bbits + ((bbits + be - 1) & 3);
#else
   1 + P - bbits;
#endif
  b2 += i;
  s2 += i;
  mhi = i2b(1);
  }
 if (m2 > 0 && s2 > 0) {
  i = m2 < s2 ? m2 : s2;
  b2 -= i;
  m2 -= i;
  s2 -= i;
  }
 if (b5 > 0) {
  if (leftright) {
   if (m5 > 0) {
    mhi = pow5mult(mhi, m5);
    b1 = mult(mhi, b);
    Bfree(b);
    b = b1;
    }
   if (j = b5 - m5)
    b = pow5mult(b, j);
   }
  else
   b = pow5mult(b, b5);
  }
 S = i2b(1);
 if (s5 > 0)
  S = pow5mult(S, s5);

 /* Check for special case that d is a normalized power of 2. */

 spec_case = 0;
 if ((mode < 2 || leftright)
#ifdef Honor_FLT_ROUNDS
   && rounding == 1
#endif
    ) {
  if (!word1(d) && !(word0(d) & Bndry_mask)
#ifndef Sudden_Underflow
   && word0(d) & (Exp_mask & ~Exp_msk1)
#endif
    ) {
   /* The special case */
   b2 += Log2P;
   s2 += Log2P;
   spec_case = 1;
   }
  }

 /* Arrange for convenient computation of quotients:
  * shift left if necessary so divisor has 4 leading 0 bits.
  *
  * Perhaps we should just compute leading 28 bits of S once
  * and for all and pass them and a shift to quorem, so it
  * can do shifts and ors to compute the numerator for q.
  */
#ifdef Pack_32
 if (i = ((s5 ? 32 - hi0bits(S->x[S->wds-1]) : 1) + s2) & 0x1f)
  i = 32 - i;
#else
 if (i = ((s5 ? 32 - hi0bits(S->x[S->wds-1]) : 1) + s2) & 0xf)
  i = 16 - i;
#endif
 if (i > 4) {
  i -= 4;
  b2 += i;
  m2 += i;
  s2 += i;
  }
 else if (i < 4) {
  i += 28;
  b2 += i;
  m2 += i;
  s2 += i;
  }
 if (b2 > 0)
  b = lshift(b, b2);
 if (s2 > 0)
  S = lshift(S, s2);
 if (k_check) {
  if (cmp(b,S) < 0) {
   k--;
   b = multadd(b, 10, 0); /* we botched the k estimate */
   if (leftright)
    mhi = multadd(mhi, 10, 0);
   ilim = ilim1;
   }
  }
 if (ilim <= 0 && (mode == 3 || mode == 5)) {
  if (ilim < 0 || cmp(b,S = multadd(S,5,0)) <= 0) {
   /* no digits, fcvt style */
 no_digits:
   k = -1 - ndigits;
   goto ret;
   }
 one_digit:
  *s++ = '1';
  k++;
  goto ret;
  }
 if (leftright) {
  if (m2 > 0)
   mhi = lshift(mhi, m2);

  /* Compute mlo -- check for special case
   * that d is a normalized power of 2.
   */

  mlo = mhi;
  if (spec_case) {
   mhi = Balloc(mhi->k);
   Bcopy(mhi, mlo);
   mhi = lshift(mhi, Log2P);
   }

  for(i = 1;;i++) {
   dig = quorem(b,S) + '0';
   /* Do we yet have the shortest decimal string
    * that will round to d?
    */
   j = cmp(b, mlo);
   delta = diff(S, mhi);
   j1 = delta->sign ? 1 : cmp(b, delta);
   Bfree(delta);
#ifndef ROUND_BIASED
   if (j1 == 0 && mode != 1 && !(word1(d) & 1)
#ifdef Honor_FLT_ROUNDS
    && rounding >= 1
#endif
           ) {
    if (dig == '9')
     goto round_9_up;
    if (j > 0)
     dig++;
#ifdef SET_INEXACT
    else if (!b->x[0] && b->wds <= 1)
     inexact = 0;
#endif
    *s++ = dig;
    goto ret;
    }
#endif
   if (j < 0 || j == 0 && mode != 1
#ifndef ROUND_BIASED
       && !(word1(d) & 1)
#endif
     ) {
    if (!b->x[0] && b->wds <= 1) {
#ifdef SET_INEXACT
     inexact = 0;
#endif
     goto accept_dig;
     }
#ifdef Honor_FLT_ROUNDS
    if (mode > 1)
     switch(rounding) {
      case 0: goto accept_dig;
      case 2: goto keep_dig;
      }
#endif /*Honor_FLT_ROUNDS*/
    if (j1 > 0) {
     b = lshift(b, 1);
     j1 = cmp(b, S);
     if ((j1 > 0 || j1 == 0 && dig & 1)
     && dig++ == '9')
      goto round_9_up;
     }
 accept_dig:
    *s++ = dig;
    goto ret;
    }
   if (j1 > 0) {
#ifdef Honor_FLT_ROUNDS
    if (!rounding)
     goto accept_dig;
#endif
    if (dig == '9') { /* possible if i == 1 */
 round_9_up:
     *s++ = '9';
     goto roundoff;
     }
    *s++ = dig + 1;
    goto ret;
    }
#ifdef Honor_FLT_ROUNDS
 keep_dig:
#endif
   *s++ = dig;
   if (i == ilim)
    break;
   b = multadd(b, 10, 0);
   if (mlo == mhi)
    mlo = mhi = multadd(mhi, 10, 0);
   else {
    mlo = multadd(mlo, 10, 0);
    mhi = multadd(mhi, 10, 0);
    }
   }
  }
 else
  for(i = 1;; i++) {
   *s++ = dig = quorem(b,S) + '0';
   if (!b->x[0] && b->wds <= 1) {
#ifdef SET_INEXACT
    inexact = 0;
#endif
    goto ret;
    }
   if (i >= ilim)
    break;
   b = multadd(b, 10, 0);
   }

 /* Round off last digit */

#ifdef Honor_FLT_ROUNDS
 switch(rounding) {
   case 0: goto trimzeros;
   case 2: goto roundoff;
   }
#endif
 b = lshift(b, 1);
 j = cmp(b, S);
 if (j > 0 || j == 0 && dig & 1) {
 roundoff:
  while(*--s == '9')
   if (s == s0) {
    k++;
    *s++ = '1';
    goto ret;
    }
  ++*s++;
  }
 else {
/* trimzeros: */ /* since it's unreferenced */
  while(*--s == '0');
  s++;
  }
 ret:
 Bfree(S);
 if (mhi) {
  if (mlo && mlo != mhi)
   Bfree(mlo);
  Bfree(mhi);
  }
 ret1:
#ifdef SET_INEXACT
 if (inexact) {
  if (!oldinexact) {
   word0(d) = Exp_1 + (70 << Exp_shift);
   word1(d) = 0;
   dval(d) += 1.;
   }
  }
 else if (!oldinexact)
  clear_inexact();
#endif
 Bfree(b);
 /* *s = 0; */ /* don't NUL terminate */
 *decpt = k + 1;
 if (rve)
  *rve = s;
 return s0;
 }

#ifdef Use_J_Memory
#undef diff
#undef mult
#undef P
#undef MALLOC
#include "j.h"

 static void *
d2a_Malloc
#ifdef KR_headers
 (k) int k;
#else
 (struct dtoa_info *di, int n)
#endif
{
 A z;
 J jt=(J)di->jt;

 GA10(z,LIT,n);
 if(!z || jt->jerr) longjmp(di->_env, 1);
 return  AV(z);
 }


#endif

// called only at initialization, so no ras() needed
B jtecvtinit(J jt) {A x; struct dtoa_info *di;
 GATV0(x, LIT, sizeof(struct dtoa_info), 1);
 di=(struct dtoa_info*)AV(x); 
 di->_p5s=0;
 di->_pmem_next=di->_private_mem;
 mvc(sizeof(di->_private_mem),di->_private_mem,MEMSET00LEN,MEMSET00);
 mvc( sizeof(di->_freelist),di->_freelist,MEMSET00LEN,MEMSET00);
 di->jt=jt;  // remember thread pointer in case further ga() needed
 jt->dtoa=di;
 R 1;
}

/* uses dtoa and behaves like ecvt (well, ecvt_r) */
/* this writes exactly ndp bytes at dest */
B jtecvt(J jt, D dw, I ndp, int *decpt, int *sign, C *dest)
{
 struct dtoa_info *di=(struct dtoa_info*)jt->dtoa;
 C *y,*z=0;

 ASSERTSYS(ndp<=INT_MAX, "jtecvt: too long");
 di->ndp=(int)ndp; di->result=dest;
 y=d2a_dtoa(di, dw, 2, (int)ndp, decpt, sign, (char**)&z);
 RZ(y&&z);
 mvc( ndp-(z-y),z,1,iotavec-IOTAVECBEGIN+'0');
 R 1;
}

#ifdef __cplusplus
}
#endif
