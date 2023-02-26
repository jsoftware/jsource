/* Copyright (c) 1990-2022, Jsoftware Inc.                                 */
/* Licensed use only.                                                      */
/*                                                                         */
/* j init libgmp support                                                   */

#define JGMPINIT    // tell jgmp.h to declare storage rather than externs
#ifdef _WIN32
 #include <windows.h>
#else
 #include <dlfcn.h>  // -ldl
#endif
#include "j.h"      // includes jgmp.h

///////////////////////////////////////////////////////////////
// Constants
// see jgmp.h for some notes on type X
//
#if C_LE
 #if SY_64
  #define Xrh 1,0,FHRHISGMP,0,0
 #else
  #define Xrh 1,0,FHRHISGMP
 #endif
#else
 #if SY_64
  #define Xrh 0,0,FHRHISGMP,0,1
 #else
  #define Xrh  FHRHISGMP,0,1
 #endif
#endif

#define XFIXED0(nam, typ,val) \
 struct AD __attribute__((aligned(CACHELINESIZE))) B##nam= \
 {AKXR(0),typ,0,typ,ACPERMANENT,1,Xrh,{(I)val}}; \
 X nam= (X)&B##nam

/* like struct AD but a data element */
struct BDV1 {I k;I f;I m;I t;I c;I n;
#if C_LE
 RANKT r;UC filler;US h;
#if SY_64
 US origin;S lock;
#endif
#else
 #if SY_64
  S lock;US origin;
 #endif
 US h;UC filler;RANKT r;
#endif
 I s[1];UI d;};
 
#define XFIXED1(nam, typ,sgn,val) \
 struct BDV1 __attribute__((aligned(CACHELINESIZE))) B##nam= \
 {XHSZ,typ,0,typ,ACPERMANENT,1,Xrh,sgn,(UI)val}; \
 X nam= (X)&B##nam

XFIXED1(X_1,LIT,-1,1);  // _1x (not an array)
XFIXED0(X0, LIT, 0);    //  0x (not an array)
XFIXED1(X1, LIT, 1,1);  //  1x (not an array)
XFIXED1(X2, LIT, 1,2);  //  2x (not an array

mpz_t mpX1= {1,1,(mp_limb_t*)&BX1.s[1]};

// one-off special case relying on X/A type pun, among other things
// XFIXED0(AX1,XNUM,(X)&BX1);// ''$1x

#undef Xrh
#undef XFIXED0
#undef XFIXED1

#define RATIO(n,d) {(X)&BX##n, (X)&BX##d}

Q Q__ = RATIO(_1, 0);  //  _1r0 (not an array)
Q Q_1 = RATIO(_1, 1);  //  _1r1 (not an array)
Q Q0  = RATIO( 0, 1);  //   0r1 (not an array)
Q Q1r2= RATIO( 1, 2);  //   1r2 (not an array)
Q Q1  = RATIO( 1, 1);  //   1r1 (not an array)
Q Q_  = RATIO( 1, 0);  //   1r0 (not an array)

#undef RATIO

////////////////////////////////////////////////////////////////
// memory management and glue for new libgmp numbers
// See jgmp.h for additional notes
//
// we use raw malloc/free here because libgmp
// does not know which thread we're running in. 
// and we finish initialization if/when this reaches J

// see: https://gmplib.org/manual/Custom-Allocation 
#ifndef IMPORTGMPLIB
static void (*jmp_set_memory_functions) (
 void *(*alloc_func_ptr) (size_t),
 void *(*realloc_func_ptr) (void *, size_t, size_t),
 void (*free_func_ptr) (void *, size_t)
);
#endif

#if PYXES==1
static pthread_mutex_t gemp_mutex; 
#define GMPLOCKINIT pthread_mutex_init(&gemp_mutex, NULL);
#define GMPLOCK pthread_mutex_lock(&gemp_mutex)
#define GMPUNLOCK pthread_mutex_unlock(&gemp_mutex)
#else
#define GMPLOCKINIT
#define GMPLOCK
#define GMPUNLOCK
#endif

long long gmpmallocs;

#if MEMAUDIT&0x40
#define GUARDSIZE  (CACHELINESIZE)
#define GUARDsSIZE (2*CACHELINESIZE)
C GUARDBLOCK[GUARDSIZE];
/* seppuku if copy of guard memory has been altered
 */
static void guard(C*m) {
 if(!memcmp(m, GUARDBLOCK, GUARDSIZE)) R;
 fprintf(stderr, "guard block corrupt\n"
  "m: %llx, GUARDBLOCK: %llx, GUARDSIZE: %x\n         m:",
  (U long long)m, (U long long)GUARDBLOCK, (U)GUARDSIZE);
 DO(GUARDSIZE, fprintf(stderr, " %.2hhx", m[i]);)
 fprintf(stderr, "\nGUARDBLOCK:");
 DO(GUARDSIZE, fprintf(stderr, " %.2hhx", GUARDBLOCK[i]);)
 fprintf(stderr, "\n");
 SEGFAULT;
}
// for use in m.c:
void jgmpguard(X x) {
 guard((C*)x-GUARDSIZE);
 guard((C*)x+XHSZ+AN(x));
}
#else
#define GUARDSIZE  0
#define GUARDsSIZE 0
#endif

/*static*/void*jmalloc4gmp(size_t size){
 size_t avxsize= (size+7)&(-8); // libgmp's use of __strlen_avx2 segfaults when libgmp asks for size=2
 size_t totalsize= avxsize+XHSZ+GUARDsSIZE;
 C*m= malloc(totalsize);
 if(!m){ // wsfull?
  GMPLOCK;
  __atomic_fetch_add(&gempwsfull, 1, __ATOMIC_SEQ_CST);
  m= (void*)__atomic_fetch_add(&gemptr, totalsize, __ATOMIC_SEQ_CST);
  if (unlikely(!INGEMP(gemptr))) SEGFAULT; // this should never happen but, if it does, try to make it a little less mysterious
  GMPUNLOCK;
 }
 if(unlikely(!m))R 0; // FIXME (but can't without replacing libgmp) does nothing
#if MEMAUDIT&0x40
 memcpy(m, GUARDBLOCK, GUARDSIZE); guard(m);
 memcpy(m+GUARDSIZE+XHSZ+size, GUARDBLOCK, GUARDSIZE); guard(m+GUARDSIZE+XHSZ+size);
#endif
 X z= (X)(m+GUARDSIZE); // J header starts here
 AK(z)= XHSZ;           // always rank 1
 AFLAG(z)= 0;           // should be a safe value
 AM(z)= 0;              // 0 let's us test if we've put it on the tpush stack
 AT(z)= LIT;            // matches significance of AN(z), simplifies 3!:1
 AC(z)= 1;              // always 1 ref in libgmp
 AN(z)= size;           // track amount of requested memory
 AR(z)= 1;              // always rank 1
 AFHRH(z)= FHRHISGMP;   // mark as a GMP psuedo-array
 /*
 fprintf(stderr,"jmalloc4gmp (%lli) z: %llx (size: %zx), ", ++gmpmallocs, (UI)z, size);
 fprintf(stderr,"AK(z): %llx (%lli), ", AK(z), AK(z));
 fprintf(stderr,"AFLAG(z): %llx (%lli), ", AFLAG(z), AFLAG(z));
 fprintf(stderr,"AT(z): %llx (%lli), ", AT(z), AT(z));
 fprintf(stderr,"AC(z): %llx (%lli), ", AC(z), AC(z));
 fprintf(stderr,"AN(z): %llx (%lli), ", AN(z), AN(z));
 fprintf(stderr,"AR(z): %hhx (%hhu), ", AR(z), AR(z));
 fprintf(stderr,"AFHRH(z): %hx (%hi)\n", AFHRH(z), AFHRH(z));
 */
#if MEMAUDIT&8
 static I lfsr= 1;
 DO(size/SZI, lfsr= (lfsr<<1) ^ (lfsr<0 ?0x1b :0); if (i!=2&&i!=6)((I*)(XHSZ+(C*)z))[i]= lfsr;);
#endif
 R CAV1(z);
}

static void*jrealloc4gmp(void*ptr, size_t old, size_t new){
 size_t avxnew= (new+7)&(-8); // libgmp's use of __strlen_avx2 segfaults when libgmp asks for size=2
 X x= (X)(((C*)ptr)-XHSZ);
 C*m0= (C*)x-GUARDSIZE;
#if MEMAUDIT&0x40
 guard(m0);
 guard(m0+GUARDSIZE+XHSZ+old);
#endif
 // assert(1==AC(x));   // must not have been exposed to J
 // assert(FHRHISGMP==AFHRH(x))
 size_t newsize= avxnew+XHSZ+GUARDsSIZE;
 C*m= INGEMP(m0) ?0 :realloc(m0, newsize);
 if(unlikely(!m)){ // wsfull?
  GMPLOCK;
  if (INGEMP(m0)) {
   if (new <= old) {
    GMPUNLOCK;
    return ptr;
   }
  } else __atomic_fetch_add(&gempwsfull, 1, __ATOMIC_SEQ_CST);
  m= (void*)__atomic_fetch_add(&gemptr, newsize, __ATOMIC_SEQ_CST);
  if (unlikely(!INGEMP(gemptr))) SEGFAULT; // this should never happen but, if it does, try to make it a little less mysterious
  size_t avxold= (old+7)&(-8); // recreate previous pad length
  size_t oldsize= avxold+XHSZ+GUARDsSIZE;
  memcpy(m, m0, oldsize);
  GMPUNLOCK;
 }
 if(!m)R 0; // assert(m);// FIXME (but can't without replacing libgmp)
#if MEMAUDIT&0x40
 memcpy(m+GUARDSIZE+XHSZ+new, GUARDBLOCK, GUARDSIZE); guard(m+GUARDSIZE+XHSZ+new);
#endif
 X z= (X)(m+GUARDSIZE); // J header starts here
 AN(z)= new;            // track allocated memory
 /*
 fprintf(stderr,"jrealloc4gmp z: %llx <- %llx (size: %zx <- %zx), ", (UI)z, (UI)x, new, old);
 fprintf(stderr,"AK(z): %llx (%lli), ", AK(z), AK(z));
 fprintf(stderr,"AFLAG(z): %llx (%lli), ", AFLAG(z), AFLAG(z));
 fprintf(stderr,"AT(z): %llx (%lli), ", AT(z), AT(z));
 fprintf(stderr,"AC(z): %llx (%lli), ", AC(z), AC(z));
 fprintf(stderr,"AN(z): %llx (%lli), ", AN(z), AN(z));
 fprintf(stderr,"AR(z): %hhx (%hhu), ", AR(z), AR(z));
 fprintf(stderr,"AFHRH(z): %hx (%hi)\n", AFHRH(z), AFHRH(z));
 */
#if MEMAUDIT&8
 static I lfsr= 1;
 if (new > old) {
  DO((new-old)/SZI, lfsr= (lfsr<<1)^(lfsr<0 ?0x1b :0); if (i!=2&&i!=6)((I*)(old+XHSZ+(C*)z))[i]= lfsr;);
 }
#endif
 R CAV1(z);
}

/*static*/ void jfree4gmp(void*ptr, size_t n){
 // assert(1==AC(x));
 // assert(FHRHISGMP=AT(x))
 X x= UNvoidAV1(ptr);
 if (ACPERMANENT&AC(x)) SEGFAULT;
 C* m= (C*)x-GUARDSIZE;
#if MEMAUDIT&0x40
 guard(m);
 guard(m+GUARDSIZE+XHSZ+n);
#endif
 /*
 fprintf(stderr,"\tjfree4gmp (%lli) x: %llx, ", gmpmallocs--, (UI)x);
 fprintf(stderr,"AK(x): %llx (%lli), ", AK(x), AK(x));
 fprintf(stderr,"AFLAG(x): %llx (%lli), ", AFLAG(x), AFLAG(x));
 fprintf(stderr,"AT(x): %llx (%lli), ", AT(x), AT(x));
 fprintf(stderr,"AC(x): %llx (%lli), ", AC(x), AC(x));
 fprintf(stderr,"AN(x): %llx (%lli), ", AN(x), AN(x));
 fprintf(stderr,"AR(x): %hhx (%hhu), ", AR(x), AR(x));
 fprintf(stderr,"AFHRH(x): %hx (%hi)\n", AFHRH(x), AFHRH(x));
 */
#if MEMAUDIT&0x4
 AK(x)= AFLAG(x)= AM(x)= AT(x)= AC(x)= AN(x)= 0xdeadbeef;
#endif
 if (INGEMP(m)) {
  GMPLOCK;
  if (0== __atomic_add_fetch(&gempwsfull, -1, __ATOMIC_SEQ_CST)) {
   __atomic_store_n(&gemptr, gempool, __ATOMIC_SEQ_CST);
  }
  GMPUNLOCK;
  return;
 }
 free(m);
}

// dehydrate fresh (mpz_t) as J (X)
X jtXmpzcommon(J jt, mpz_t mpz, B numeric) {
 if(unlikely(!mpz->_mp_size) && likely(numeric)) {
  jmpz_clear(mpz);
  R X0; // mpz is new but may not have memory
 }
 X x=UNvoidAV1(mpz->_mp_d);         // we gave libgmp AV1(x) for this block of memory
#if MEMAUDIT&1
 if(FHRHISGMP!=AFHRH(x)) SEGFAULT;
 if(1!=AC(x)) SEGFAULT;             // should be pristine from libgmp
 if(ACISPERM(AC(x))) SEGFAULT;      // should never happen
#endif
 XSGN(x)= mpz->_mp_size;            // size of number and its sign
 I n= AN(x);                        // length of memory allocated for number
 I sz= XHSZ+n;                      // bytes allocated
#if SY_64
 x->origin= THREADID(jt);           // track thread which created this array
#endif
 jt->bytes+= sz;                    // summarize the size of the new space
 jt->malloctotal+= sz;              // ditto
 jt->mfreegenallo+= sz;             // ditto
 A*pushp; tpushnoret(x);            // issue death warrant
 ASSERT(!(AFLAG(x)&AFRO),EVWSFULL); // error if we used an emergency buffer
 R x;
}

// dehydrate fresh (mpq_t) as J (Q)
Q jtQmpq(J jt, mpq_t mpq) {
 Q q; A*pushp;
 q.n= Xmpzcommon(&mpq->_mp_num, 1); // dehydrate numerator
 q.d= Xmpzcommon(&mpq->_mp_den, 1); // dehydrate denominator
 if (unlikely(!q.n)||unlikely(!q.d)||unlikely(ISQ0(q))) R Q0;
 R q;
}

///////////////////////////////////////////////////////////////
// link with libgmp
#ifdef _WIN32
 #define LIBEXT ".dll"
 #define LIBGMPNAME "mpir" LIBEXT
#else
 #ifdef __APPLE__
  #define LIBEXT ".dylib"
  #define LIBEXT10 LIBEXT 
 #else
  #define LIBEXT ".so"
  #ifdef __OpenBSD__
  #define LIBEXT10 ".so.11.0"    /* symlink of .so only available when devel package installed */
  #else
  #define LIBEXT10 ".so.10"    /* symlink of .so only available when devel package installed */
  #endif
 #endif
 #define LIBGMPNAME "libgmp" LIBEXT
 #define LIBGMPNAME10 "libgmp" LIBEXT10
 #define LIBJGMPNAME "libjgmp" LIBEXT
#endif

static void*libgmp=0;
#ifdef IMPORTGMPLIB
B nogmp(){R 0;}
#else
B nogmp(){R!libgmp;}
#endif

#ifdef _WIN32
void dldiag(){}
#define jgmpfn(fn) j##fn= GetProcAddress(libgmp,"__g"#fn); if(!(j##fn)){fprintf(stderr,"%s\n","error loading "#fn);};
#else
void dldiag(){char*s=dlerror();if(s)fprintf(stderr,"%s\n",s);}
#define jgmpfn(fn) j##fn= dlsym(libgmp,"__g"#fn); dldiag();
#endif

// referenced gmp routines are declared twice:
// once here for dynamic linking and 
// also in jgmp.h for global name declaration
void jgmpinit(C*libpath) {
 if (SZI != sizeof (mp_limb_t)) SEGFAULT; // verify a fundamental assumption
#if MEMAUDIT&0x40
 ((UIL*)GUARDBLOCK)[0]= (UIL)0x1abe11ed1abe11edLL;
 DO(GUARDSIZE-sizeof (UIL), GUARDBLOCK[i+sizeof (UIL)]= GUARDBLOCK[i]+i;)
#endif
#ifndef IMPORTGMPLIB
 if (jmpz_init) return; // link with libgmp only once
#endif
#ifndef IMPORTGMPLIB
 C dllpath[1000];
#ifdef _WIN32
 if(libpath&&*libpath){
  strcpy(dllpath,libpath);strcat(dllpath,"\\");strcat(dllpath,LIBGMPNAME);
  if(!(libgmp= LoadLibraryA(dllpath)))  /* first try current directory */
  libgmp= LoadLibraryA(LIBGMPNAME);
 } else libgmp= LoadLibraryA(LIBGMPNAME);
 if (!libgmp) {fprintf(stderr,"%s\n","error loading gmp library");R;}
#else
 if(libpath&&*libpath){
  int FHS=0,i=0;
  const char *usrlib[]={"/opt/homebrew/lib","/usr/local/lib","/lib64","/lib",
                  "/lib/aarch64-linux-gnu","/lib/arm-linux-gnueabihf",
                  "/lib/x86_64-linux-gnu","/lib/i386-linux-gnu",0 };
  while(usrlib[i]){
   if(!strcmp(libpath,usrlib[i])) {FHS=1; break;}
   i++;
  }
  strcpy(dllpath,libpath);strcat(dllpath,"/");strcat(dllpath,FHS?LIBJGMPNAME:LIBGMPNAME);
  if((libgmp= dlopen(LIBGMPNAME10, RTLD_LAZY))){
   if(!dlsym(libgmp,"__gmpn_gcd_11")){   /* check system libgmp version is 6.2.x  */
    dlclose(libgmp); libgmp= 0;
   }
  }
//  fprintf(stderr, "%s\n", (libgmp)?"using system libgmp":"not using system libgmp");
  if(!libgmp)
  if(!(libgmp= dlopen(dllpath, RTLD_LAZY)))  /* first try libj directory */
  libgmp= dlopen(FHS?LIBGMPNAME10:LIBGMPNAME, RTLD_LAZY);
 } else libgmp= dlopen(LIBGMPNAME10, RTLD_LAZY);
 if (!libgmp) {dldiag();R;}
#endif
#endif
#ifdef IMPORTGMPLIB
 mp_set_memory_functions(jmalloc4gmp, jrealloc4gmp, jfree4gmp);
#else
 jgmpfn(mp_set_memory_functions);
 jmp_set_memory_functions(jmalloc4gmp, jrealloc4gmp, jfree4gmp);
 gemptr= gempool;          // silly hack to avert silly libgmp failure case
 gempwsfull= 0;
 GMPLOCKINIT;
 jgmpfn(mpq_add);          // https://gmplib.org/manual/Rational-Arithmetic
 // DO NOT USE //          jgmpfn(mpq_canonicalize); // https://gmplib.org/manual/Rational-Number-Functions
 jgmpfn(mpq_clear);        // https://gmplib.org/manual/Initializing-Rationals
 jgmpfn(mpq_cmp);          // https://gmplib.org/manual/Comparing-Rationals
#ifndef RASPI
 jgmpfn(mpq_cmp_z);        // https://gmplib.org/manual/Comparing-Rationals
#endif
 jgmpfn(mpq_div);          // https://gmplib.org/manual/Rational-Arithmetic
 jgmpfn(mpq_get_d);        // https://gmplib.org/manual/Rational-Conversions
 jgmpfn(mpq_get_str);      // https://gmplib.org/manual/Rational-Conversions
 jgmpfn(mpq_init);         // https://gmplib.org/manual/Initializing-Rationals
 jgmpfn(mpq_mul);          // https://gmplib.org/manual/Rational-Arithmetic
 jgmpfn(mpq_out_str);      // https://gmplib.org/manual/I_002fO-of-Rationals
 jgmpfn(mpq_set);          // https://gmplib.org/manual/Initializing-Rationals
 jgmpfn(mpq_sub);          // https://gmplib.org/manual/Rational-Arithmetic
 jgmpfn(mpz_abs);          // https://gmplib.org/manual/Integer-Arithmetic
 jgmpfn(mpz_add);          // https://gmplib.org/manual/Integer-Arithmetic
 jgmpfn(mpz_add_ui);       // https://gmplib.org/manual/Integer-Arithmetic
 jgmpfn(mpz_bin_ui);       // https://gmplib.org/manual/Number-Theoretic-Functions
 jgmpfn(mpz_cdiv_q);       // https://gmplib.org/manual/Integer-Division
 jgmpfn(mpz_fdiv_q);       // https://gmplib.org/manual/Integer-Division
 jgmpfn(mpz_clear);        // https://gmplib.org/manual/Initializing-Integers
 jgmpfn(mpz_cmp);          // https://gmplib.org/manual/Integer-Comparisons
 jgmpfn(mpz_cmpabs_ui);    // https://gmplib.org/manual/Integer-Comparisons
 jgmpfn(mpz_cmp_si);       // https://gmplib.org/manual/Integer-Comparisons
 jgmpfn(mpz_fac_ui);       // https://gmplib.org/manual/Number-Theoretic-Functions
 jgmpfn(mpz_fdiv_q);       // https://gmplib.org/manual/Integer-Division
 jgmpfn(mpz_fdiv_qr);      // https://gmplib.org/manual/Integer-Division
 jgmpfn(mpz_fdiv_qr_ui);   // https://gmplib.org/manual/Integer-Division
 jgmpfn(mpz_fdiv_r);       // https://gmplib.org/manual/Integer-Division
 jgmpfn(mpz_gcd);          // https://gmplib.org/manual/Number-Theoretic-Functions
 jgmpfn(mpz_get_d);        // https://gmplib.org/manual/Converting-Integers
 jgmpfn(mpz_get_d_2exp);   // https://gmplib.org/manual/Converting-Integers
 jgmpfn(mpz_get_str);      // https://gmplib.org/manual/Converting-Integers
 jgmpfn(mpz_get_si);       // https://gmplib.org/manual/Converting-Integers
 jgmpfn(mpz_init);         // https://gmplib.org/manual/Initializing-Integers
 jgmpfn(mpz_init_set_d);   // https://gmplib.org/manual/Simultaneous-Integer-Init-_0026-Assign
 jgmpfn(mpz_init_set_str); // https://gmplib.org/manual/Simultaneous-Integer-Init-_0026-Assign
 jgmpfn(mpz_init_set_si);  // https://gmplib.org/manual/Simultaneous-Integer-Init-_0026-Assign
 jgmpfn(mpz_lcm);          // https://gmplib.org/manual/Number-Theoretic-Functions
 jgmpfn(mpz_mul);          // https://gmplib.org/manual/Integer-Arithmetic
 jgmpfn(mpz_neg);          // https://gmplib.org/manual/Integer-Arithmetic
 jgmpfn(mpz_out_str);      // (for debugging) https://gmplib.org/manual/I_002fO-of-Integers
 jgmpfn(mpz_powm);         // https://gmplib.org/manual/Integer-Exponentiation
 jgmpfn(mpz_pow_ui);       // https://gmplib.org/manual/Integer-Exponentiation
 jgmpfn(mpz_probab_prime_p);//https://gmplib.org/manual/Number-Theoretic-Functions
 jgmpfn(mpz_ui_pow_ui);    // https://gmplib.org/manual/Integer-Exponentiation
 jgmpfn(mpz_root);         // https://gmplib.org/manual/Integer-Roots
 jgmpfn(mpz_set);          // https://gmplib.org/manual/Assigning-Integers
// not used jgmpfn(mpz_set_si);       // https://gmplib.org/manual/Assigning-Integers
 jgmpfn(mpz_sizeinbase);   // https://gmplib.org/manual/Miscellaneous-Integer-Functions
 jgmpfn(mpz_sub);          // https://gmplib.org/manual/Integer-Arithmetic
#endif
}
