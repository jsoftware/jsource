/* Copyright 1990-2014, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Global Definitions                                                      */

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
#include <memory.h>
#include <sys/types.h>
#endif

#if 1
#include <float.h>
#include <limits.h>
#define link unused_syscall_link
#define qdiv unused_netbsd_qdiv
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


#if SY_64
#define IMAX            9223372036854775807L
#define FMTI            "%lli"
#define FMTI02          "%02lli"
#define FMTI04          "%04lli"
#define FMTI05          "%05lli"

#if SY_WIN32
#define strtoI         _strtoi64
#else
#define strtoI          strtoll
#endif

#else
#define IMAX            2147483647L
#define FMTI            "%li"
#define FMTI02          "%02li"
#define FMTI04          "%04li"
#define FMTI05          "%05li"
#define strtoI          strtol
#endif

#define IMIN            (~IMAX)   /* ANSI C LONG_MIN is  -LONG_MAX */

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
#if SY_64
#if SY_WIN32 
// RESTRICT is an attribute of a pointer, and indicates that no other pointer points to the same area
#define RESTRICT __restrict
// RESTRICTF is an attribute of a function, and indicates that the object returned by the function is not aliased with any other object
#define RESTRICTF __declspec(restrict)
#endif
#if SY_LINUX || SY_MAC
#define RESTRICT __restrict
// No RESTRICTF on GCC
#endif
#endif
#ifndef RESTRICT
#define RESTRICT
#endif
#ifndef RESTRICTF
#define RESTRICTF
#endif

// If the user switch C_NOMULTINTRINSIC is defined, suppress using it
#ifdef C_NOMULTINTRINSIC
#define C_USEMULTINTRINSIC 0
#else
#define C_USEMULTINTRINSIC 1
#endif

// disable C_USEMULTINTRINSIC if un-available
#if C_USEMULTINTRINSIC
#if !defined(_MSC_VER)
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

#define NALP            256             /* size of alphabet                */
#define NETX            2000            /* size of error display buffer    */
#define NPP             20              /* max value for quad pp           */
#define RMAX            IMAX            /* max rank                        */
#define NPATH           1024            /* max length for path names,      */
                                        /* including trailing 0 byte       */
#define NFDEP           (8000L+12000L*SY_64)             // fn call depth
#define NFCALL          (MAX(40,NFDEP/10)) // call depth for named calls - can be expensive

#define NTSTACK         16384L          // number of BYTES in an allocated block of tstack - pointers to allocated blocks

#define IIDOT           0               /* modes for indexofsub()          */
#define IICO            1
#define INUBSV          2
#define INUB            3
#define ILESS           4
#define INUBI           5
#define IEPS            6
#define II0EPS          7
#define II1EPS          8
#define IJ0EPS          9
#define IJ1EPS          10
#define ISUMEPS         11
#define IANYEPS         12
#define IALLEPS         13
#define IIFBEPS         14

#define IPHOFFSET       30              /* offset for prehashed versions   */
#define IPHIDOT         30
#define IPHICO          31
#define IPHLESS         34
#define IPHEPS          36
#define IPHI0EPS        37
#define IPHI1EPS        38
#define IPHJ0EPS        39
#define IPHJ1EPS        40
#define IPHSUMEPS       41
#define IPHANYEPS       42
#define IPHALLEPS       43
#define IPHIFBEPS       44

#define jfloor          floor

#define BB              8      /* # bits in a byte */
#if SY_64
#define BW              64     /* # bits in a word */
#define LGSZI 3    // lg(#bytes in an I)
#else
#define BW              32
#define LGSZI 2
#endif

#define TOOMANYATOMS 0x01000000000000LL  // more atoms than this is considered overflow (64-bit)

#define ABS(a)          (0<=(a)?(a):-(a))
#define ACX(a)          {AC(a)=IMAX/2;}
#define ASSERT(b,e)     {if(!(b)){jsignal(e); R 0;}}
#define ASSERTD(b,s)    {if(!(b)){jsigd((s)); R 0;}}
#define ASSERTMTV(w)    {RZ(w); ASSERT(1==AR(w),EVRANK); ASSERT(!AN(w),EVLENGTH);}
#define ASSERTN(b,e,nm) {if(!(b)){jt->curname=(nm); jsignal(e); R 0;}}
#define ASSERTSYS(b,s)  {if(!(b)){jsignal(EVSYSTEM); jtwri(jt,MTYOSYS,"",(I)strlen(s),s); R 0;}}
#define ASSERTW(b,e)    {if(!(b)){if((e)<=NEVM)jsignal(e); else jt->jerr=(e); R;}}
#define CALL1(f,w,fs)   ((f)(jt,    (w),(A)(fs)))
#define CALL2(f,a,w,fs) ((f)(jt,(a),(w),(A)(fs)))
#define CALL1IP(f,w,fs)   ((f)(jtinplace,    (w),(A)(fs)))
#define CALL2IP(f,a,w,fs) ((f)(jtinplace,(a),(w),(A)(fs)))
#define CLEARZOMBIE     {jt->assignsym=0; jt->zombieval=0;}  // Used when we know there shouldn't be an assignsym, just in case
#define DF1(f)          A f(J jt,    A w,A self)
#define DF2(f)          A f(J jt,A a,A w,A self)
#define DO(n,stm)       {I i=0,_n=(n); for(;i<_n;i++){stm}}
#define DQ(n,stm)       {I i=(n)-1;    for(;i>=0;--i){stm}}
#define ds(c)           pst[(UC)(c)]
#define FDEPDEC(d)      {jt->fdepi-=d;}
#define FDEPINC(d)      {ASSERT(jt->fdepn>=d+jt->fdepi,EVSTACK); jt->fdepi+=d;}
#define FCONS(x)        fdef(CFCONS,VERB,jtnum1,jtnum2,0L,0L,(x),VFLAGNONE, RMAX,RMAX,RMAX)
#define FEQ(u,v)        (ABS((u)-(v))<=jt->fuzz*MAX(ABS(u),ABS(v)))
#define F1(f)           A f(J jt,    A w)
#define F2(f)           A f(J jt,A a,A w)
#define FPREF           
#define F1PREF          FPREF
#define F2PREF          FPREF
#define FPREFIP         J jtinplace=jt; jt=(J)((I)jt&~(JTINPLACEW+JTINPLACEA))
#define F1PREFIP        FPREFIP
#define F2PREFIP        FPREFIP
#define F1RANK(m,f,self)    {RZ(   w); if(m<AR(w)         )R rank1ex(  w,(A)self,(I)m,     f);}  // if there is more than one cell, run rank1ex on them.  m=monad rank, f=function to call for monad cell
#define F2RANK(l,r,f,self)  {RZ(a&&w); if(l<AR(a)||r<AR(w))R rank2ex(a,w,(A)self,(I)l,(I)r,f);}  // If there is more than one cell, run rank2ex on them.  l,r=dyad ranks, f=function to call for dyad cell

// Memory-allocation macros
// Size-of-block calculations.  VSZ when size is constant or variable
// Because the Boolean dyads write beyond the end of the byte area (up to 1 extra word), we add one SZI for islast (which includes B01), rather than adding 1
#define ALLOBYTESVSZ(atoms,rank,size,islast,isname)      ( ((((rank)|(!SY_64))*SZI  + ((islast)? (isname)?(AH*SZI+sizeof(NM)+SZI+mhb):(AH*SZI+SZI+mhb) : (AH*SZI+mhb)) + (atoms)*(size)))  )  // # bytes to allocate allowing only 1 byte for string pad - include mem hdr
// here when size is constant.  The number of bytes must not exceed 2^(PMINL+5)
#define ALLOBYTES(atoms,rank,size,islast,isname)      ((size%SZI)?ALLOBYTESVSZ(atoms,rank,size,islast,isname):(SZI*(((rank)|(!SY_64))+AH+mhw+((size)/SZI)*(atoms))))  // # bytes to allocate
#define ALLOBLOCK(n) ((n)<=2*PMIN?((n)<=PMIN?PMINL:PMINL+1) : (n)<=8*PMIN?((n)<=4*PMIN?PMINL+2:PMINL+3) : (n)<=32*PMIN?PMINL+4:IMIN)   // lg2(#bytes to allocate)
// GA() is used when the type is unknown.  This routine is in m.c and documents the function of these macros.
// NEVER use GA() for NAME types - it doesn't honor it.
#define GA(v,t,n,r,s)   RZ(v=ga(t,(I)(n),(I)(r),(I*)(s)))
// When the type and all rank/shape are known, use GAT.  The compiler precalculates almost everything
// For best results declare name as: AD* RESTRICT name;
#define GAT(name,type,atoms,rank,shaape) \
{ I bytes = ALLOBYTES(atoms,rank,type##SIZE,(type)&LAST0,(type)&NAME); \
 name = jtgaf(jt, ALLOBLOCK(bytes)); \
 I akx=AKXR(rank);   \
 RZ(name);   \
 AK(name)=akx; AT(name)=type; AN(name)=atoms;   \
 if(!(type&DIRECT))memset((C*)name+akx,C0,bytes-mhb-akx);  \
 else if(type&LAST0){((I*)((C*)name+((bytes-SZI-mhb)&(-SZI))))[0]=0; }     \
 AR(name)=rank;     \
 if((1==(rank))&&!(type&SPARSE))*AS(name)=atoms; else if((shaape)&&(rank)){AS(name)[0]=((I*)(shaape))[0]; DO(rank-1, AS(name)[i+1]=((I*)(shaape))[i+1];)}    \
 AM(name)=((I)1<<ALLOBLOCK(bytes))-mhb-akx;    \
}
// Used when type is known and something else is variable.  ##SIZE must be applied before type is substituted, so we have GATVS to use inside other macros.  Normally use GATV
#define GATVS(name,type,atoms,rank,shaape,size) \
{ I bytes = ALLOBYTES(atoms,rank,size,(type)&LAST0,(type)&NAME); \
 ASSERT(SY_64?((unsigned long long)(atoms))<TOOMANYATOMS:(I)bytes>(I)(atoms)&&(I)(atoms)>=(I)0,EVLIMIT); \
 AD* ZZz = jtgafv(jt, bytes);   \
 I akx=AKXR(rank);   \
 RZ(ZZz);   \
 if(!(type&DIRECT))memset((C*)ZZz+akx,C0,bytes-mhb-akx);  \
 else if(type&LAST0){((I*)((C*)ZZz+((bytes-SZI-mhb)&(-SZI))))[0]=0; }     \
 AK(ZZz)=akx; AT(ZZz)=type; AN(ZZz)=atoms; AR(ZZz)=rank;     \
 if((1==(rank))&&!(type&SPARSE))*AS(ZZz)=atoms; else if((shaape)&&(rank)){AS(ZZz)[0]=((I*)(shaape))[0]; DO(rank-1, AS(ZZz)[i+1]=((I*)(shaape))[i+1];)}   \
 AM(ZZz)=((I)1<<((MS*)ZZz-1)->j)-mhb-akx;     \
 name=ZZz;   \
}
#define  GATV(name,type,atoms,rank,shaape) GATVS(name,type,atoms,rank,shaape,type##SIZE)

#define HN              4L  // number of boxes per valence to hold exp-def info (words, control words, original (opt.), symbol table)
#define IC(w)           (AR(w) ? *AS(w) : 1L)
#define ICMP(z,w,n)     memcmp((z),(w),(n)*SZI)
#define ICPY(z,w,n)     memcpy((z),(w),(n)*SZI)
#define INF(x)          ((x)==inf||(x)==infm)
#define IX(n)           apv((n),0L,1L)
#define JATTN           {if(*jt->adbreak&&!jt->breakignore){jsignal(EVATTN); R 0;}}
#define JBREAK0         {if(2<=*jt->adbreak&&!jt->breakignore){jsignal(EVBREAK); R 0;}}
#define JTINPLACEW      1   // turn this on in jt to indicate that w can be inplaced
#define JTINPLACEA      2   // turn this on in jt to indicate that a can be inplaced
#define MAX(a,b)        ((a)>(b)?(a):(b))
#define MC              memcpy
#define MIN(a,b)        ((a)<(b)?(a):(b))
#define MLEN            (SY_64?63:31)
#define NAN0            (_clearfp())
#if defined(_MSC_VER) && _MSC_VER==1800 && !SY_64 // bug in some versions of VS 2013
#define NAN1            {if(_SW_INVALID&_statusfp()){_clearfp();jsignal(EVNAN); R 0;}}
#define NAN1V           {if(_SW_INVALID&_statusfp()){_clearfp();jsignal(EVNAN); R  ;}}
#else
#define NAN1            {if(_SW_INVALID&_clearfp()){jsignal(EVNAN); R 0;}}
#define NAN1V           {if(_SW_INVALID&_clearfp()){jsignal(EVNAN); R  ;}}
#endif
#define NUMMIN          (-9)    // smallest number represented in num[]
#define NUMMAX          9    // largest number represented in num[]
// PROD multiplies a list of numbers, where the product is known not to overflow a signed int (for example, it might be part of the shape of a dense array)
#define PROD(result,length,ain) {I _i; if((_i=(length)-1)<0)result=1;else{result=*(ain);if(_i>0){I *_ain=(ain); do{result*=*++_ain;}while(--_i);}}}
// CPROD is to be used to create a test testing #atoms.  Because empty arrays can have cells that have too many atoms, we can't use PROD if
// we don't know that the array isn't empty or will be checked later
#define CPROD(t,z,x,a)if(t)PROD(z,x,a)else RE(z=prod(x,a))
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
#define PROLOG(x)       I _ttop=jt->tnextpushx
#define EPILOG(z)       R gc(z,_ttop)   // z is the result block
// Some primitives (such as x { y, x {. y, etc.) only copy the first level (i. e. direct data, or pointers to indirect data) and never make any copies of indirect data.
// For such primitives, it is unnecessary to check the descendants of the result: they are not going to be deleted by the tpop for the primitive, and increasing the usecount followed
// by decreasing it in the caller can never change the point at which a block will be freed.  So for these cases, we increment the usecount, and perform a final tpush, only of
// the block that was allocated in the current primitive
#define EPILOG1(z)       {ACINCR(z); tpop(_ttop); tpush1(z); R z;}   // z is the result block
// Routines that do little except call a function that does PROLOG/EPILOG have EPILOGNULL as a placeholder
#define EPILOGNULL(z)   R z
// Routines that do not return A
#define EPILOG0         tpop(_ttop)
#define PTO             3L  // Number of prefix entries of ptab[] that are used only for local symbol tables
// Compounds push zombie to begin with and restore before the last operation, which can execute in place.
// zombieval is used as a way of flagging reusable blocks.  They are reused only if they are marked as inplaceable; in other words,
// zombieval is an alternative to AC<0.  We could try to overwrite the zombieval during final assignment, even if it is
// not an argument, but this seems to be more trouble than it's worth, so we don't bother detecting final assignment.
#define PUSHZOMB L*savassignsym = jt->assignsym; A savzombval; if(savassignsym){if(!jt->asgzomblevel||!jt->local){savzombval=jt->zombieval;CLEARZOMBIE}}
#define POPZOMB if(savassignsym){jt->assignsym=savassignsym;jt->zombieval=savzombval;}
#define R               return
#define RE(exp)         {if((exp),jt->jerr)R 0;}
#define RER             {if(er){jt->jerr=er; R;}}
#define RESETERR        {jt->etxn=jt->jerr=0;}
#define RNE(exp)        {R jt->jerr?0:(exp);}
#define RZ(exp)         {if(!(exp))R 0;}
#define SBSV(x)         (jt->sbsv+(I)(x))
#define SBUV(x)         (jt->sbuv+(I)(x))
#define SGN(a)          ((0<(a))-(0>(a)))
#define SMAX            65535
#define SMIN            (-65536)
#define SYMHASH(h,n)    ((h)%(n)+SYMLINFOSIZE)   // h is hash value for symbol; n is number of symbol chains (not including LINFO entries)
#define SZA             ((I)sizeof(A))
#define SZD             ((I)sizeof(D))
#define SZI             ((I)sizeof(I))
#define VAL1            '\001'
#define VAL2            '\002'

#if C_LE
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


#include "ja.h" 
#include "jc.h" 
#include "jtype.h" 
#include "m.h"
#include "jt.h" 
#include "jlib.h"
#include "je.h" 
#include "jerr.h" 
#include "va.h" 
#include "vq.h" 
#include "vx.h" 
#include "vz.h"
#include "vdx.h"  
#include "a.h"
#include "s.h"


// CTTZ(w) counts trailing zeros in low 32 bits of w.  Result is undefined if w is 0.
// CTTZZ(w) does the same, but returns 32 if w is 0
// CTTZI(w) counts trailing zeros in an argument of type I (32 or 64 bits depending on architecture)
// CTLZ would be a better primitive to support, except that LZCNT executes as BSR on some Intel processors,
// but produces incompatible results! (BSR returns bit# of leading 1, LZCNT returns #leading 0s)
// since we don't require CTLZ yet, we defer that problem to another day

// CTTZ uses the single-instruction count-trailing-zeros instruction to convert
// a 1-bit mask to a bit number.  If this instruction is available on your architecture/compiler,
// you should use the compiler intrinsic to create this instruction, and define the CTTZ and CTTZZ
// macros to use the instruction inline.  It is used enough in the J engine to make a difference.

// If you set AUDITCOMPILER to 1, i.c will include code to test CTTZ (and signed shift) on startup and crash if it
// does not perform properly, as a debugging aid.

// If CTTZ is not defined, the default routine defined in u.c will be used.  You can look there
// for the complete spec for CTTZ and CTTZZ.

#if SY_WIN32 
#include <intrin.h>
#define CTTZ(w) _tzcnt_u32((UINT)(w))
#if SY_64
#define CTTZI(w) _tzcnt_u64((UI)(w))
#else
#define CTTZI(w) _tzcnt_u32((UINT)(w))
#endif
#define CTTZZ(w) ((w)==0 ? 32 : CTTZ(w))
#endif

#if SY_LINUX || SY_MAC
#define CTTZ(w) __builtin_ctzl((UINT)(w))
#if SY_64
#define CTTZI(w) __builtin_ctzll((UI)(w))
#else
#define CTTZI(w) __builtin_ctzl((UINT)(w))
#endif
#define CTTZZ(w) ((w)==0 ? 32 : CTTZ(w))
#endif

// For older processors, TZCNT is executed as BSF, which differs from TZCNT in that it does not
// set the Z flag if the result is 0.  The optimizer sometimes turns a switch into tests rather than a branch
// table, and it expects TSCNT to set the Z flag properly.  We use CTTZNOFLAG to set it right
#define CTTZNOFLAG(w) (CTTZ(w)&31)

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

// Set these switches for testing
#define AUDITBP 0  // Verify that bp() returns expected data
#define AUDITCOMPILER 0  // Verify compiler features CTTZ, signed >>


// JPFX("here we are\n")
// JPF("size is %i\n", v)
// JPF("size and extra: %i %i\n", (v,x))
#define JPFX(s)  {char b[1000]; sprintf(b, s);    jsto(gjt,MTYOFM,b);}
#define JPF(s,v) {char b[1000]; sprintf(b, s, v); jsto(gjt,MTYOFM,b);}
extern J gjt; // global for JPF (procs without jt)

#if SY_WINCE_MIPS
/* strchr fails for CE MIPS - neg chars - spellit fails in ws.c for f=.+.  */
#define strchr(a,b)     (C*)strchr((unsigned char*)(a), (unsigned char)(b))
#endif
#if (defined(__arm__)||defined(__aarch64__)) && !defined(__MACH__)
// option -fsigned-char in android and raspberry
#ifdef strchr
#undef strchr
#endif
#define strchr(a,b)     (C*)strchr((unsigned char*)(a), (unsigned char)(b))
#endif

#if SYS & SYS_UNIX
#include <fenv.h>
#define _isnan       isnan
#define _SW_INVALID  FE_INVALID

static inline UINT _clearfp(void){int r=fetestexcept(FE_ALL_EXCEPT);
 feclearexcept(FE_ALL_EXCEPT); return r;
}
#endif

// Use MEMAUDIT to sniff out errant memory alloc/free
#define MEMAUDIT 0   // Bitmask for memory audits: 1=check headers 2=full audit of tpush/tpop 4=write garbage to memory before freeing it 8=write garbage to memory after getting it
 // 2 will detect double-frees before they happen, at the time of the erroneous tpush
#define CACHELINESIZE 128  // free pool is aligned on this boundary

#if C_HASH
#define HASH0           224273737UL
#else
#define HASH0           0UL
#endif


// Define integer multiply, *z=x*y but do something else if integer overflow.
// Depending on the compiler, the overflowed result may or may not have been stored
#if SY_64

#if C_USEMULTINTRINSIC
#if SY_WIN32 
#define DPMULDECLS I l,h;
// DPMUL: *z=x*y, execute s if overflow
#define DPMUL(x,y,z,s) *z=l=_mul128(x,y,&h); if(h+((UI)l>>(BW-1)))s
#define DPMULDDECLS I h;
#define DPMULD(x,y,z,s) z=_mul128(x,y,&h); if(h+((UI)z>>(BW-1)))s
#else
#define DPMULDECLS
#define DPMUL(x,y,z,s) if(__builtin_smulll_overflow(x,y,z))s
#define DPMULDDECLS
#define DPMULD(x,y,z,s) if(__builtin_smulll_overflow(x,y,&z))s
#endif
#else // C_USEMULTINTRINSIC 0 - use standard-C version (64-bit)
#define DPMULDECLS I _l;D _d;
#define DPMUL(x,y,z,s) _l=(x)*(y); _d=(D)(x)*(D)(y)-(D)_l; *z=_l; _d=ABS(_d); if(_d>1e8)s  // *z may be the same as x or y
#define DPMULDDECLS I _l;D _d;
#define DPMULD(x,y,z,s) _l=(x)*(y); _d=(D)(x)*(D)(y)-(D)_l; z=_l; _d=ABS(_d); if(_d>1e8)s
#endif

#else  // 32-bit

#if C_USEMULTINTRINSIC
#if SY_WIN32 
// optimizer can't handle this #define SPDPADD(addend, sumlo, sumhi) {C c; c=_addcarry_u32(0,addend,sumlo,&sumlo); _addcarry_u32(c,0,sumhi,&sumhi);}
#define DPMULDECLS unsigned __int64 _p;
#define DPMUL(x,y,z,s) _p = __emul(x,y); *z=(I)_p; if((_p+0x80000000U)>0xFFFFFFFFU)s
#define DPMULDDECLS unsigned __int64 _p;
#define DPMULD(x,y,z,s) _p = __emul(x,y); z=(I)_p; if((_p+0x80000000U)>0xFFFFFFFFU)s
#else
#define DPMULDECLS
#define DPMUL(x,y,z,s) if(__builtin_smull_overflow(x,y,z))s
#define DPMULDDECLS
#define DPMULD(x,y,z,s) if(__builtin_smull_overflow(x,y,&z))s
#endif
#else // C_USEMULTINTRINSIC 0 - use standard-C version (32-bit)
#define DPMULDECLS D _p;
#define DPMUL(x,y,z,s) _p = (D)x*(D)y; *z=(I)_p; if(_p>IMAX||_p<IMIN)s
#define DPMULDDECLS D _p;
#define DPMULD(x,y,z,s) _p = (D)x*(D)y; z=(I)_p; if(_p>IMAX||_p<IMIN)s
#endif

#endif
// end of multiply builtins

// define single+double-recision integer add
#if SY_64
#if SY_WIN32 
#define SPDPADD(addend, sumlo, sumhi) {C c; c=_addcarry_u64(0,addend,sumlo,&sumlo); _addcarry_u64(c,0,sumhi,&sumhi);}
#endif
#endif

#ifndef SPDPADD   // default version for systems without addcarry
#define SPDPADD(addend, sumlo, sumhi) sumlo += addend; sumhi += (addend > sumlo);
#endif
// end of addition builtins

// Create (x&y) where x and y are signed, so we can test for overflow.
#if SY_WIN32
#define XANDY(x,y) ((x)&(y))
#else
#define XANDY(x,y) ((I)((UI)(x)&(UI)(y)))
#endif

