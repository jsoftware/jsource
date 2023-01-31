/* Copyright (c) 1990-2022, Jsoftware Inc.                                 */
/* Licensed use only.                                                      */
/*                                                                         */
/* libgmp support for extended precision and rational types                */

/* 
 * See https://gmplib.org/manual/Nomenclature-and-Types
 * for an introduction to libgmp nomenclature.
 *
 * This header file, along with jgmpinit.h, ties together
 * libgmp and the j engine.
 *
 * Previously, J's extended integers were polynomials in base 10000.
 * In other words, if Y was a struct AD with t=XNUM, it would have
 * values which were pointers to struct AD with t=INT, where the first
 * int in each of these would be (*Y)*10000|Y, and so on (there was
 * a special exception to this rule for infinity).
 *
 * And a rational number would be a pair of these extended integers,
 * one for the numerator the second for the denominator. (This is 
 * still the case).
 *
 * Under libgmp, this has changed. Instead of base 10000, we now use
 * base 18446744073709551616 on 64 bit systems. Also, the sign is
 * now stored in the array header rather than in the polynomial.
 *
 * Also, libgmp has its own datatype for representing extended integers
 * and rational numbers. Instead of using struct AD, libgmp requires 
 * an mpz_t for extended integers and an mpq_t for rationals.
 *
 * mpz_t is a one element array of __mpz_struct. (So as a parameter
 * it's a pointer but it also declares storage.) And, an __mpz_struct
 * has three fields:
 *  _mp_alloc used for memory management
 *  _mp_size polynomial length times sign
 *  _mp_d pointer to polynomial digits (aka "limbs" because they are big digits)
 *
 * mpq_t is an array of __mpq_struct (which is a pair of __mpz_struct,
 * one for numerator, one for denominator).
 *
 * In many ways, J's approach and libgmp's approach are very similar.
 * But of course, the differences are critical.
 *
 * In libgmp, the polynomial digits are "owned" by value of mpz_t
 * which points to them. In other words, libgmp operations which
 * generate extended integer results will delete/overwrite any
 * existing value. This means that when we have finished using
 * libgmp to generate a value, and hand it to J, we can not
 * give it back to libgmp for further modifications.
 *
 * More specifically, the lifecycle of an extended integer looks like this:
 *
 * (0) Storage is initially allocated, and zeroed
 *    _mp_alloc is set to 0 (no digits in polynomial)
 *    _mp_size is set to 0 (the extended integer is 0)
 *
 *    _mp_d would point to "safe memory" if libgmp initialized _mp_d
 *    (Instead, J would initialize _mp_d=NULL)
 * 
 * (1) libgmp performs some useful operation 
 *
 * (2) libgmp calls back to J for memory to store that result
 *     Here, J allocates sufficient space for a struct AD
 *     with the memory requested as the payload and then
 *     returns the address of the payload to libgmp.
 *     see jmalloc4gmp() in jgmpinit.h for more details
 *
 * (3) libgmp concludes its operation
 *
 * (4) J extracts the _mp_size field from the mpz_t value
 *     and uses this as the "shape" of the result (may be negative)
 *
 * (5) As far as the rest of j is concerned, this is just an
 *     extended integer, just like before (except some of the
 *     header values are ... different).
 *
 * From a memory management perspective, w is a libgmp "pseudo-array"
 * if ISGMP(w) is true.
 *
 * From a debugging perspective:
 *   AN(w) is the amount of memory requested by libgmp. This is
 *   used in alloroundsize() and memory accounting.
 *
 *   llabs(AS(w)[0]) is the number of "limbs" in the polynomial
 *
 *   And the sign of AS(w)[0] is the sign of the polynomial
 *
 *   If the represented number is relatively small, AS(w)[1] will be
 *   the absolute value of the number.
 *
 * Two other issues:
 *   There's no good way of passing a thread identifier through
 *   libgmp to the memory allocation function. So when allocating
 *   memory we cannot assume we know which thread we're in.
 *   (Perhaps we should create a "libgmp pseudo-thread" here.)
 *
 *   In J, errors are handled by recording the error and returning a
 *   null or zero (and bailing out early during error conditions),
 *   In libgmp, errors are handled by exiting the program. 
 *   Thus, it would be fatal for J to request an operation from
 *   libgmp when there's insufficient memory to represent the result.
 *   Consider ^~^:_(2) or function's like ackerman's canonical examples
 *   of this problem.
 *
 * The initial solution is to estimate how much memory an operation
 * would need, either statically (addition requires approximately
 * the same amount of memory for the result as the largest argument)
 * or through calculations, and throw errors before calling libgmp.
 */



/*
There are several shorthands for built-in types.
 short is (signed) short int
 long is (signed) long int
 long long is (signed) long long int.

 gmp.h type long int, is 32-bit on windows 64
*/

#undef __GMP_WITHIN_GMP
#ifdef _WIN32
#undef MSC_BUILD_DLL
#define MSC_USE_DLL
#include <mpir.h>
#else
#include <gmp.h>
#endif
#ifdef _WIN64
typedef unsigned long long int	mpir_ui;
typedef long long int		mpir_si;
#else
typedef unsigned long       mpir_ui;
typedef long                mpir_si;
#endif
extern void jgmpinit(C*);
extern B nogmp(void);
#define GMP ASSERT(!nogmp(), EVMISSINGGMP); GEMP0 // interface error if libgmp is not available
extern void*jmalloc4gmp(size_t);
#define GAGMP(z,n) (z=({n ?({GEMP0; mpz_t mpd= {n/SZI, n/SZI, jmalloc4gmp(n)}; Xmp(d);}) :X0;}))
extern X jtXmpzcommon(J, mpz_t, B);
extern Q jtQmpq(J, mpq_t);

#ifdef JGMPINIT
 /* context here is jgmpinit.c */
 #define EXTERN
#else
 /* context here is external references to jgmpinit.c */
 #define EXTERN extern
#endif

#define FHRHISGMP 0x4000 // this block was allocated by GMP (goes in AFHRH(x))
                         // see also FHRH... defines in m.c

#define ISGMP(x) unlikely(FHRHISGMP==AFHRH(x))

#if MEMAUDIT
#define CHECKZAP(xcheckzap) ({if (!ACISPERM(AC(xcheckzap))) if (!AZAPLOC(xcheckzap)) {fprintf(stderr, "%llx: perm: %i, zap: %llx\n", (UIL)xcheckzap, !!ACISPERM(AC(xcheckzap)), (UIL)AZAPLOC(xcheckzap)); SEGFAULT;} xcheckzap;})
#else
#define CHECKZAP(xcheckzap) (xcheckzap)
#endif

// parallel definition of R0 in j.h (Q0 defined further down here)
#if FINDNULLRET   // When we return 0, we should always have an error code set.  trap if not
#define RQ0 {if(!jt->jerr)SEGFAULT; R Q0;}
#else
#define RQ0 R Q0;
#endif

/* Shortcuts/Assumptions/Constraints for valid X x on 64 bit architecture
 *
 * base=: 2^64x
 * XSGN(x)-: (*x)*# base #.inv|x
 *
 * XLIMBLEN(x) is the magnitude of XSGN(x) and is the length
 * the representation of the absolute value of x
 *
 * (UI)*IAV1(x) in C would represent base|x in J
 * (so it's x when x<base)
 *
 * Note that type X is used only vaguely like type A
 * In particular, always AV1/CAV1/IAV1 when locating values,
 * (never use other members of the AV() family).
 * And, UNvoidAV1 to convert from that libgmp _mp_d value to
 * the corresponding pointer of type X.
 *
 *  X1 is 1x
 *  X0 is 0x
 *  X_1 is _1x
 *  
 *  0=XLIMBLEN(X0) so IAV1(X0) must never be referenced.
 */ 

/* libgmp would shut down j if it can't get enough memory to
 * represent a result. So we refuse to perform calculations 
 * if the result would need more limbs than this:
 */
#define XLIMBLIM (INT_MAX/(5*sizeof (UI)))

/*
 * when converting between libgmp and J's form for extended integers
 * and rationals, we adopt a naming convention where the libgmp
 * version of a name has an 'mp' prefix. To describe this in comments,
 * we call the 'mp' version "hydrated" and the J version "dehydrated".
 */

#define XLIMBLEN(x) llabs(XSGN(x))

// declare mpz_t mp##x and rehydrate from (X x)
// (typically use mpX instead of mpzX)
// see also jgmp_set_memory_functions() and jtXmp() in jgmpinit.c
#define mpzX(MPZ,x) GEMP0; mpzXnojt(MPZ,x)

// same but for use in macros where we might not have a jt
#define mpzXnojt(MPZ,x) mpz_t MPZ= {XLIMBLEN(x), XSGN((x)), voidAV1((x))}

// same, but without declaring MPZ and for use in contexts where there's no jt
#define mpzXnodeclnojt(MPZ,x) MPZ._mp_alloc= XLIMBLEN(x); MPZ._mp_size= XSGN(x); MPZ._mp_d= voidAV1((x))

// same, but leveraging jt to catch wsfull errors early
#define mpzXnodecl(MPZ,x) GEMP0; mpzXnodeclnojt(MPZ,x)

// declare and intialize mpz_t mp##x reflecting X x
#define mpX(x) mpzX(mp##x, (x))

// same but for use in macros where we might not have a jt
#define mpXnojt(x) mpzXnojt(mp##x, (x))

// declare and initialize mpz_t mp##x reflecting 0x 
// (use this for hypothetical x -- when x does not exist)
// obsolete #define mpX0(x) mpz_t mp##x= {0, 0, 0}
#define mpX0(x) GEMP0; mpz_t mp##x; (jmpz_init)(mp##x)

// dehydrate a gmp (mpz_t) as a J (X), produces the X result as a C rvalue
#define Xmp(x) Xmpzcommon(GEMPz(mp##x), 1)

// dehydrate a gmp (mpz_t) as a J (X), produces the X result as a C rvalue
#define Xmpnojt(x) Xmpzcommon(mp##x, 1)

// dehydrate a gmp (mpq_t mp##q) as a J value of type (Q)
// produces the Q result as a C rvalue
#define Qmp(q) jtQmpq(jt, GEMPq(mp##q))

// rehydrate a J (Q q) as a gmp (mpq_t mp##q) reflecting value of q
#define mpQ(q) \
  if (!AC(q.n)||!AC(q.d)) SEGFAULT; \
  mpq_t mp##q;  mpzXnodecl(mp##q[0]._mp_num, q.n); mpzXnodecl(mp##q[0]._mp_den, q.d);

// same but for use in contexts which cannot return a Q
#define mpQnojt(q) \
  if (!AC(q.n)||!AC(q.d)) SEGFAULT; \
  mpq_t mp##q;  mpzXnodeclnojt(mp##q[0]._mp_num, q.n); mpzXnodeclnojt(mp##q[0]._mp_den, q.d);

// same but for hypothetical q which does not actually exist, sets mp##q to 0r1
#define mpQ0(q) GEMP0; mpq_t mp##q; jmpq_init(mp##q); GEMP0

// make a temporary Q q (and mpq_t mp##q) copy of mpq_t mp##t
#define MPQtemp(q,t) GEMP0; mpq_t mp##q; jmpq_init(mp##q); GEMP0; jmpq_set(mp##q,t); Q q= Qmp(q);

EXTERN X X2;             //  2x internal form
EXTERN X X1;             //  1x internal form
EXTERN mpz_t mpX1;
EXTERN X X0;             //  0x internal form
EXTERN X X_1;            // _1x internal form
// EXTERN A AX1;            // 1x as a rank 0 array

EXTERN Q Q_;             // x: _   NB. 1r0 internal form
EXTERN Q Q1;             // 1r1 internal form
EXTERN Q Q1r2;           // 1r2 internal form
EXTERN Q Q0;             // 0r1 internal form
EXTERN Q Q_1;            // _1r1 internal form
EXTERN Q Q__;            // x: __ NB. _1r0 internal form

/*
 * referenced gmp routines are declared twice.
 * Here, for for static declaration.
 * And in jgmpinit.c for dynamic linking.
 */
#ifdef IMPORTGMPLIB
#define jmp_set_memory_functions __gmp_set_memory_functions
#define jmpq_add __gmpq_add                  // https://gmplib.org/manual/Rational-Arithmetic
#define jmpq_clear __gmpq_clear              // https://gmplib.org/manual/Initializing-Rationals
#define jmpq_cmp __gmpq_cmp                  // https://gmplib.org/manual/Comparing-Rationals
#define jmpq_cmp_z __gmpq_cmp_z              // https://gmplib.org/manual/Comparing-Rationals
#define jmpq_div __gmpq_div                  // https://gmplib.org/manual/Rational-Arithmetic
#define jmpq_get_d __gmpq_get_d              // https://gmplib.org/manual/Rational-Conversions
#define jmpq_get_str __gmpq_get_str          // https://gmplib.org/manual/Rational-Conversions
#define jmpq_init __gmpq_init                // https://gmplib.org/manual/Initializing-Rationals
#define jmpq_mul __gmpq_mul                  // https://gmplib.org/manual/Rational-Arithmetic
// not used #define jmpq_out___gstr mpq_out_str          // https://gmplib.org/manual/I_002fO-of-Rationals
#define jmpq_set __gmpq_set                  // https://gmplib.org/manual/Initializing-Rationals
#define jmpq_sub __gmpq_sub                  // https://gmplib.org/manual/Rational-Arithmetic
#define jmpz_abs __gmpz_abs                  // https://gmplib.org/manual/Integer-Arithmetic
#define jmpz_add __gmpz_add                  // https://gmplib.org/manual/Integer-Arithmetic
#define jmpz_add_ui __gmpz_add_ui            // https://gmplib.org/manual/Integer-Arithmetic
#define jmpz_bin_ui __gmpz_bin_ui            // https://gmplib.org/manual/Number-Theoretic-Functions
#define jmpz_cdiv_q __gmpz_cdiv_q            // https://gmplib.org/manual/Integer-Division
#define jmpz_fdiv_q __gmpz_fdiv_q            // https://gmplib.org/manual/Integer-Division
#define jmpz_clear __gmpz_clear              // https://gmplib.org/manual/Initializing-Integers
#define jmpz_cmp __gmpz_cmp                  // https://gmplib.org/manual/Integer-Comparisons
#define jmpz_cmpabs_ui __gmpz_cmpabs_ui      // https://gmplib.org/manual/Integer-Comparisons
#define jmpz_cmp_si __gmpz_cmp_si            // https://gmplib.org/manual/Integer-Comparisons
#define jmpz_fac_ui __gmpz_fac_ui            // https://gmplib.org/manual/Number-Theoretic-Functions
#define jmpz_fdiv_q __gmpz_fdiv_q            // https://gmplib.org/manual/Integer-Division
#define jmpz_fdiv_qr __gmpz_fdiv_qr          // https://gmplib.org/manual/Integer-Division
#define jmpz_fdiv_qr_ui __gmpz_fdiv_qr_ui    // https://gmplib.org/manual/Integer-Division
#define jmpz_fdiv_r __gmpz_fdiv_r            // https://gmplib.org/manual/Integer-Division
#define jmpz_gcd __gmpz_gcd                  // https://gmplib.org/manual/Number-Theoretic-Functions
#define jmpz_get_d __gmpz_get_d              // https://gmplib.org/manual/Converting-Integers
#define jmpz_get_d_2exp __gmpz_get_d_2exp    // https://gmplib.org/manual/Converting-Integers
#define jmpz_get_str __gmpz_get_str          // https://gmplib.org/manual/Converting-Integers
#define jmpz_get_si __gmpz_get_si            // https://gmplib.org/manual/Converting-Integers
#define jmpz_init __gmpz_init                // https://gmplib.org/manual/Initializing-Integers
#define jmpz_init_set_d __gmpz_init_set_d    // https://gmplib.org/manual/Simultaneous-Integer-Init-_0026-Assign
#define jmpz_init_set_str __gmpz_init_set_str// https://gmplib.org/manual/Simultaneous-Integer-Init-_0026-Assign
#define jmpz_init_set_si __gmpz_init_set_si  // https://gmplib.org/manual/Simultaneous-Integer-Init-_0026-Assign
#define jmpz_lcm __gmpz_lcm                  // https://gmplib.org/manual/Number-Theoretic-Functions
#define jmpz_mul __gmpz_mul                  // https://gmplib.org/manual/Integer-Arithmetic
#define jmpz_neg __gmpz_neg                  // https://gmplib.org/manual/Integer-Arithmetic
#define jmpz_out_str __gmpz_out_str          // (for debugging) https://gmplib.org/manual/I_002fO-of-Integers
#define jmpz_powm __gmpz_powm                // https://gmplib.org/manual/Integer-Exponentiation
#define jmpz_pow_ui __gmpz_pow_ui            // https://gmplib.org/manual/Integer-Exponentiation
#define jmpz_probab_prime_p __gmpz_probab_prime_p //https://gmplib.org/manual/Number-Theoretic-Functions
#define jmpz_ui_pow_ui __gmpz_ui_pow_ui      // https://gmplib.org/manual/Integer-Exponentiation
#define jmpz_root __gmpz_root                // https://gmplib.org/manual/Integer-Roots
#define jmpz_set __gmpz_set                  // https://gmplib.org/manual/Assigning-Integers
// not used #define jmpz_set_si __gmpz_set_si            // https://gmplib.org/manual/Assigning-Integers
#define jmpz_sizeinbase __gmpz_sizeinbase    // https://gmplib.org/manual/Miscellaneous-Integer-Functions
#define jmpz_sub __gmpz_sub                  // https://gmplib.org/manual/Integer-Arithmetic
#else
EXTERN void (*jmpq_add)(mpq_t, const mpq_t, const mpq_t);
EXTERN void (*jmpq_clear)(mpq_t);
EXTERN int  (*jmpq_cmp)(const mpq_t, const mpq_t);
EXTERN int  (*jmpq_cmp_z)(const mpq_t, const mpz_t);
EXTERN void (*jmpq_div)(mpq_t, const mpq_t, const mpq_t);
EXTERN D    (*jmpq_get_d)(const mpq_t);
EXTERN C*   (*jmpq_get_str)(C*, int, const mpq_t);
EXTERN void (*jmpq_init)(mpq_t);
EXTERN void (*jmpq_out_str)(FILE*,int,const mpq_t); // for debugging
EXTERN void (*jmpq_mul)(mpq_t, const mpq_t, const mpq_t);
EXTERN void (*jmpq_set)(mpq_t, const mpq_t);
EXTERN void (*jmpq_sub)(mpq_t, const mpq_t, const mpq_t);
EXTERN void (*jmpz_abs)(mpz_t, const mpz_t);
EXTERN void (*jmpz_add)(mpz_t, const mpz_t, const mpz_t);
EXTERN void (*jmpz_add_ui)(mpz_t, const mpz_t, mpir_ui);
EXTERN void (*jmpz_bin_ui)(mpz_t, const mpz_t, mpir_ui);
EXTERN void (*jmpz_cdiv_q)(mpz_t, const mpz_t, const mpz_t);
EXTERN void (*jmpz_clear)(mpz_t);
EXTERN int  (*jmpz_cmp)(const mpz_t, const mpz_t);
EXTERN int  (*jmpz_cmpabs_ui)(const mpz_t, mpir_ui);
EXTERN int  (*jmpz_cmp_si)(const mpz_t, mpir_si);
EXTERN void (*jmpz_fac_ui)(mpz_t, mpir_ui);
EXTERN void (*jmpz_fdiv_q)(mpz_t, const mpz_t, const mpz_t);
EXTERN void (*jmpz_fdiv_qr)(mpz_t, mpz_t, const mpz_t, const mpz_t);
EXTERN void (*jmpz_fdiv_qr_ui)(mpz_t, mpz_t, const mpz_t, mpir_ui);
EXTERN void (*jmpz_fdiv_r)(mpz_t, const mpz_t, const mpz_t);
EXTERN void (*jmpz_gcd)(mpz_t, const mpz_t, const mpz_t);
EXTERN D    (*jmpz_get_d)(const mpz_t);
EXTERN D    (*jmpz_get_d_2exp)(long int*, const mpz_t);
EXTERN C*   (*jmpz_get_str)(C*, int, const mpz_t);
EXTERN mpir_si(*jmpz_get_si)(const mpz_t);
EXTERN void (*jmpz_init)(mpz_t);
EXTERN void (*jmpz_init_set_d)(mpz_t, D); /* probably not used: see jtXfromD */
EXTERN int  (*jmpz_init_set_str)(mpz_t, C*, int);
EXTERN void (*jmpz_init_set_si)(mpz_t, mpir_si);
EXTERN void (*jmpz_lcm)(mpz_t, const mpz_t, const mpz_t);
EXTERN void (*jmpz_mul)(mpz_t, const mpz_t, const mpz_t);
EXTERN void (*jmpz_neg)(mpz_t, const mpz_t);
EXTERN void (*jmpz_out_str)(FILE*,int,const mpz_t); // for debugging
EXTERN void (*jmpz_powm)(mpz_t, const mpz_t, const mpz_t, const mpz_t);
EXTERN void (*jmpz_pow_ui)(mpz_t, const mpz_t, mpir_ui);
EXTERN int  (*jmpz_probab_prime_p)(const mpz_t,int);
EXTERN void (*jmpz_ui_pow_ui)(mpz_t, mpir_ui, mpir_ui);
EXTERN int  (*jmpz_root)(mpz_t, const mpz_t, mpir_ui);
EXTERN void (*jmpz_set)(mpz_t, const mpz_t);
// not used EXTERN void (*jmpz_set_si)(mpz_t, mpir_si);
EXTERN size_t(*jmpz_sizeinbase)(const mpz_t, int);
EXTERN void (*jmpz_sub)(mpz_t, const mpz_t, const mpz_t);
#endif

// for freeing C* results from libgmp
extern void jfree4gmp(void*, size_t);

/* CODE STRUCTURE
 *
 * generally speaking, jgmp.h defines the interface and
 * the libgmp implementation details are referenced from jgmpinit.h
 *
 * However, important note:
 * libgmp does not export a function for mpz_sign
 * documented at https://gmplib.org/manual/Integer-Comparisons
 * libgmp stores the sign of an mpz_t value in the _mp_size "size" field
 * (whose absolute value is the length of the _mp_d "data" field)
 *
 * Note: _mp_size is zero when the mpz_t is zero, and
 * if _mp_alloc is zero, _mp_d must be ignored.
 *
 * So we have to be aware of that here, any place where we
 * test the sign of one of these values, and when freeing memory (frgmp).
 *
 * Also, note that _mp_size will be 0 when representing 0x.
 *
 * When working with values of type X, the XSGN() macro provides the value
 * for the _mp_size field.  (Values of type X correspond to (and provide
 * the memory for) values of type mpz_t.) So the sign of XSGN(x) is
 * the sign of the value of x when x is of type X.
 */
//

// sign of type X:  negative, 0 or positive if extended integer x is negative, 0 or positive (abs(XSGN(x)) is length of representation)
#define XSGN(x) AS(x)[0]
#define XLIMB0(a) (UIAV1(a)[0])                // trailing "big digit" (probably base 1+ULLONG_MAX) in a non-zero extended integer if that could be represented directly in C
#define XLIMBn(a) (UIAV1(a)[llabs(XSGN(a))-1])  // leading "big digit" (probably base 1+ULLONG_MAX) in a non-zero extended integer if that could be represented directly in C
#define ISX0(x) (0==XSGN(x)) // x is 0
#define ISX1(x) ({X x1= x; CHECKZAP(x1); 1==XSGN(x1) && 1==XLIMB0(x1);}) // x is 1
#define ISX_1(x) ({X x1= x; CHECKZAP(x1); -1==XSGN(x1) && 1==XLIMB0(x1);}) // x is -1

// 2|x where is x is type X (may segfault if x is 0)
#define XODDnz(x) ({X nz= x; CHECKZAP(nz); UI d= *UIAV1(nz); 1&(d+(0>XSGN(x)));})

#define QSGN(q) XSGN((q).n)   // sign of type Q
#define ISQinf(q) ISX0((q).d) // test if type Q is an infinity
#define ISQ0(q) ISX0((q).n) // q is 0
#define ISQINT(q) ISX1((q).d) // (=|) q
#define ISQ1(q) ({Q Qq= q; ISQINT(Qq)&&ISX1(Qq.n);}) // q is 1

#define XHSZ AKXR(1)

#if MEMAUDIT
#define ZAPONLYONCE(x,p) {if (AZAPLOC(x)||ACISPERM(AC(x))) SEGFAULT; else AZAPLOC(x)= p; jt->tnextpushp= p;}
#else
#define ZAPONLYONCE(x,p) {AZAPLOC(x)= p; jt->tnextpushp= p;}
#endif

// optimization: 0==AZAPLOC(x) when ACISPERM(AC(x))

// like tpushi, but doesn't exit on failure and can't be sparse
// leaves pushp 0 on error
// has result 1 if x was transient (not permanent), 0 if x was permanent
#define tpushnoret(x) common(({\
   likely(!ACISPERM(AC(x))) ?({ \
       I tt=AT(x); pushp=jt->tnextpushp; *pushp++=(x); \
       if(unlikely(!((I)pushp&(NTSTACKBLOCK-1)))){pushp=tg(pushp);} \
       if(unlikely(!pushp)){jsignal(EVWSFULL);} \
       else ZAPONLYONCE(x, pushp)\
       1;}) \
    :0;}))

/*
 * some named boilerplate for hopefully commonly used expressions
 * (examples which do not get used should be removed)
 *
 * Beware name collisions between macros and invoked parameters
 * (fix the macro if its parameter names trigger compiler errors)
 */

#define shimXI(f, Xb,Ic) ({X b= Xb; mpXnojt(b); f(mpb, Ic);})
#define shimX(f, Xb) ({X b= Xb; mpXnojt(b); f(mpb);})
#define shimXX(f, Xb,Xc) ({X b= Xb, c= Xc; mpXnojt(b); mpXnojt(c); f(mpb, mpc);})
#define XshimX(f, Xb) ({mpX0(a); X b=Xb; mpX(b); f(mpa, mpb); Xmp(a);})
#define XshimUU(f, Ub,Uc) ({mpX0(a); f(mpa, Ub, Uc); Xmp(a);})
#define XshimXU(f, Xb,Uc) ({mpX0(a); X b= Xb; mpX(b); f(mpa, mpb, Uc); Xmp(a);})
#define XshimXX(f, Xb,Xc) ({mpX0(xa); X xb= Xb, xc= Xc; mpX(xb); mpX(xc); f(mpxa, mpxb, mpxc); Xmp(xa);})

// Rational implementation
#define QgetX(x) ({Q q= {x,X1}; q;})  // cast an X as a Q
#define XroundQ(q) ({Q rounded= QaddQQ(q, Q1r2); X z= Xfdiv_qXX(rounded.n, rounded.d);z;}) // cast Q as X rounding to nearest integer

#define shimQQ(f, Qb,Qc) ({Q qb= Qb, qc= Qc; mpQnojt(qb); mpQnojt(qc);  f(mpqb, mpqc);})
#define shimQX(f, Qb,Xc) ({Q b= Qb;X c= Xc; mpQnojt(b); mpX(c);  f(mpb, mpc);})
#define QshimQQ(f, Qb,Qc) ({mpQ0(a); Q b= Qb, c= Qc; mpQ(b); mpQ(c);  f(mpa, mpb, mpc); Q z= Qmp(a);z;})
#define DgetQ(Qy) ({Q yDgetQ= Qy; mpQnojt(yDgetQ); jmpq_get_d(mpyDgetQ);})
#define SgetQ(Qy) ({Q ySgetQ= Qy; mpQ(ySgetQ); C*str= jmpq_get_str(0,10,mpySgetQ); X tempx= UNvoidAV1(str); mpX(tempx); X safex= jtXmpzcommon(jt, mptempx, 0); CAV1(safex);}) // ":y

#define QaddQQ(x, y) QshimQQ(jmpq_add, x, y)            // x+y
#define icmpQQ(x, y) shimQQ(jmpq_cmp, x, y)         // k*  *x-y
#ifndef RASPI
#define icmpQX(x, y) shimQX(jmpq_cmp_z, x, y)       // k*  *x-y
#else
#define icmpQX(x, y) shimQQ(jmpq_cmp, x, QgetX(y))  // k*  *x-y
#endif
#define QdivQQ(x, y) QshimQQ(jmpq_div, x, y)            // x%y
#define QmulQQ(x, y) QshimQQ(jmpq_mul, x, y)            // x*y
#define QsubQQ(x, y) QshimQQ(jmpq_sub, x, y)            // x-y

// Extended integer implementations
#define XgetD(Dy) ({D Ty= Dy; mpz_t mpTy; GEMP0; jmpz_init_set_d(mpTy, Ty); Xmp(Ty);}) // cast a single double to X
#define XgetI(Iy) ({I Ty= Iy; mpz_t mpTy; GEMP0; jmpz_init_set_si(mpTy, Ty); Xmp(Ty);}) // cast a single signed integer to X
#define XabsX(y) XshimX(jmpz_abs, y)                    // |y
#define XaddXX(x, y) XshimXX(jmpz_add, x, y)            // x+y
#define XaddXU(x, y) XshimXU(jmpz_add_ui, x, y)         // x+y
#define XbinXU(x, y) XshimXU(jmpz_bin_ui, x, y)         // x!y
#define Xcdiv_qXX(x, y) XshimXX(jmpz_cdiv_q, x, y)      // >.x%y
#define Xfdiv_rXX(x, y) XshimXX(jmpz_fdiv_r, x, y)      // y|x
#define icmpXX(x,y) shimXX(jmpz_cmp, x,y)           // *x-y
#define icmpXI(x,y) shimXI(jmpz_cmp_si, x,y)
#define Xfdiv_qXX(x, y) XshimXX(jmpz_fdiv_q, x, y)      // <.x%y
#define XgcdXX(x, y) XshimXX(jmpz_gcd, x, y)            // x+.y
#define DgetX(y) shimX(jmpz_get_d, y)    // y+0.0
#define IgetX(y) shimX(jmpz_get_si, y)   // (I)y  NB. UINT_MAX&|&.(-&INT_MIN) y
#define IgetXor(y,er) ({X Xy= y; 0==XSGN(Xy) ?0 :1==XLIMBLEN(Xy) && GMP_NUMB_MAX>=XLIMB0(Xy) ?IgetX(Xy) :({er;LONG_MIN;});}) // IgetX with error action er which is taken when y won't fit in an I. er should abort processing and an arbitrary value would be used if it fails to do so (er would typically be an ASSERT, though other possibilities exist)
#define getSX(Sa,Xc) ({X c=Xc; mpX(c); GEMP1(C*, jmpz_get_str(Sa,10,mpc));}) // ":c
#define SgetX(Xy) ({\
 X Sy=Xy; mpX(Sy); C*s= jmpz_get_str(0,10,mpSy); \
 X tempx= UNvoidAV1(s); mpX(tempx); X safex= jtXmpzcommon(jt, mptempx, 0); \
 CAV1(safex);}) // ": y
#define XlcmXX(x, y) XshimXX(jmpz_lcm, x, y)            // x*.y
#define XmulXX(x, y) XshimXX(jmpz_mul, x, y)            // x+y
#define XnegX(y) XshimX(jmpz_neg, y)                    // -y
#define XpowXU(x, y) XshimXU(jmpz_pow_ui, x, y)         // x^y
#define XpowUU(x, y) XshimUU(jmpz_ui_pow_ui, x, y)      // x^y  // (UI)x^(UI)y
#define IsizeinbaseXI(x,y) shimXI(jmpz_sizeinbase, x,y)
#define IbitsX(x) IsizeinbaseXI(x,2)
#define XsubXX(x, y) XshimXX(jmpz_sub, x, y)            // x-y

/* -------------------------------------------------
 * GEMP -- gmp emergency memory pool --
 * hopefully temporary hack to avoid shutting down J
 * in low memory circumstances.
 * (In a future revision, we may instead inspect the libgmp result)
 */
#define GMPMAXSZ (1<<20)       // allowed #bytes for exponential contexts
#define GEMPSIZE (GMPMAXSZ<<6) // corresponding emergency pool size
EXTERN C gempool[GEMPSIZE];    // the pool itself
EXTERN C*gemptr;               // next available location in the pool
EXTERN I gempwsfull;           // non-zero when pool is occupied

#define INGEMP(pointer) unlikely(pointer >= gempool && pointer < gempool+GEMPSIZE)
#define GEMPWSFULL unlikely(__atomic_load_n(&gempwsfull, __ATOMIC_SEQ_CST))
#define GEMP0 ASSERT(!GEMPWSFULL,EVWSFULL)
#define GEMP1(type, value) ({type tmpresult= value; GEMP0; tmpresult;})
#define GEMPcommon(value,freeroutine) ({if(GEMPWSFULL) {freeroutine(value); ASSERT(0,EVWSFULL);} value;})
#define GEMPz(value) GEMPcommon(value, jmpz_clear)
#define GEMPq(value) GEMPcommon(value, jmpq_clear)
/* ------------------------------------------------- */

#undef EXTERN
