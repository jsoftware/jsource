/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Macros and Defined Constants for Atomic (Scalar) Verbs           */

                                    /*   cv - control vector               */
// bits 0-1 kept open for jtflags
// bits 2-3 should be forced to 1 jtflags;
#define VCVTIP          0xc  // bits 2-3 should always be set, indicating that a converted argument can be inplaced
#define VARGX           4           // bit position for arg flags
#define VBB             (B01<<VARGX)         /* convert arguments to B              */
#define VII             (INT<<VARGX)         /* convert arguments to I              */
#define VDD             (FL<<VARGX)          /* convert arguments to D              */
#define VZZ             (CMPX<<VARGX)        /* convert arguments to Z              */
#define Vxx             (XNUM<<VARGX)        /* convert arguments to XNUM           */
#define VQQ             (RAT<<VARGX)         /* convert arguments to RAT            */
#define VARGMSK         (VBB|VII|VDD|VZZ|Vxx|VQQ)  // mask for argument requested type
#define VRESX           12           // bit position for result flags
#define VB              (B01<<VRESX)/* result type B  bit 12                     */
#define VI              (INT<<VRESX)/* result type I  bit 14                     */
#define VD              (FL<<VRESX) /* result type D  bit 15                     */
#define VZ              (CMPX<<VRESX)/* result type Z bit 16                      */
#define VX              (XNUM<<VRESX)/* result type XNUM  bit 18                  */
#define VQ              (RAT<<VRESX) /* result type RAT  bit 19                   */
#define VSB             (SBT<<VRESX) /* result type SBT bit 28                    */
#define VRESMSK         (VB|VI|VD|VZ|VX|VQ|VSB)  // mask for result-type
#define VRD             (SLIT<<VRESX)// convert result to D if possible - unused code point
#define VRI             (SBOX<<VRESX)// convert result to I if possible - unused code point
// bits VRESX+ 1 10 11 12 are free
#define VIPWFLONGX     (SY_64?63:17)  // (must be >RANKTX) internal use in va2.  We use sign bit where possible
#define VIPWFLONG      ((I)1<<VIPWFLONGX)
#define VIPOKWX         20      // This routine can put its result over W
#define VIPOKW          ((I)1<<VIPOKWX)
#define VIPOKAX         21      // This routine can put its result over A
#define VIPOKA          ((I)1<<VIPOKAX)
#define VCANHALTX       25    // This routine can generate an error after it has started
#define VCANHALT        ((I)1<<VCANHALTX)
#define VXCHASVTYPEX    26  // set if there is forced conversion to XNUM
#define VXCHASVTYPE     ((I)1<<VXCHASVTYPEX)
#define VXCVTYPEX       29          // bit position for VX conversion type
#define VXCVTYPEMSK     ((I)3<<VXCVTYPEX)  // mask for bit-positions hold XNUM conversion type - leave here where they don't overlap noun types
#define VXX             (Vxx|VXCHASVTYPE|((I)XMEXACT<<VXCVTYPEX))  // exact conversion
#define VXEQ            (Vxx|VXCHASVTYPE|((I)XMEXMT<<VXCVTYPEX))   /* convert to XNUM for = ~:            */
#define VXCF            (Vxx|VXCHASVTYPE|((I)XMCEIL<<VXCVTYPEX))   /* convert to XNUM ceiling/floor       */
#define VXFC            (Vxx|VXCHASVTYPE|((I)XMFLR<<VXCVTYPEX))  /* convert to XNUM floor/ceiling       */
#define VIPWCRLONGX     31  // internal use in va2, must be sign bit
#define VIPWCRLONG      ((I)1<<VIPWCRLONGX)
// bit 31 must not be used - it may be a sign bit, which has a meaning
#define VARGCVTMSKF     (VXCHASVTYPE|VXCVTYPEMSK)  // mask for type to pass into XCVT, includes XNUM override
#define VFRCEXMT        (VXCHASVTYPE|((I)XMEXMT<<VXCVTYPEX))   // set in arg to cvt() to do rounding for = ~:, if the conversion happens to be to XNUM
// upper bits for 64-bit va2
#define VIPOKRNKWX         30      // filled by va2 if the ranks allow inplacing w
#define VIPOKRNKW          ((I)1<<VIPOKRNKWX)
#define VIPOKRNKAX         32      // filled by va2 if the ranks allow inplacing a
#define VIPOKRNKA          ((I)1<<VIPOKRNKAX)

// Extract the argument-conversion type from cv coming from the table
#define atype(x) (((x)&VARGMSK)>>VARGX)

// Extract the result type from cv coming from the table
#define rtype(x) (((x)&VRESMSK)>>VRESX)

#if SY_64
#define NOT(v)          ((v)^0x0101010101010101)
#else
#define NOT(v)          ((v)^0x01010101)
#endif

#define SNOT(v)         ((v)^0x0101)
#define INOT(v)         ((v)^0x01010101)

#define AND(u,v)        ( u &  v)
#define GT(u,v)         ( u & ~v)
#define LT(u,v)         (~u &  v)
#define OR(u,v)         ( u |  v)
#define NE(u,v)         ( u ^  v)
#define GE(u,v)         NOT( LT(u,v))
#define LE(u,v)         NOT( GT(u,v))
#define EQ(u,v)         NOT( NE(u,v))
#define NOR(u,v)        NOT( OR(u,v))
#define NAND(u,v)       NOT(AND(u,v))

#define IAND            AND
#define IGT             GT
#define ILT             LT
#define IOR             OR
#define INE             NE
#define IGE(u,v)        INOT(ILT(u,v))
#define ILE(u,v)        INOT(IGT(u,v))
#define IEQ(u,v)        INOT(INE(u,v))
#define INOR(u,v)       INOT(IOR(u,v))
#define INAND(u,v)      INOT(IAND(u,v))

#define SAND            AND
#define SGT             GT
#define SLT             LT
#define SOR             OR
#define SNE             NE
#define SGE(u,v)        SNOT(ILT(u,v))
#define SLE(u,v)        SNOT(IGT(u,v))
#define SEQ(u,v)        SNOT(INE(u,v))
#define SNOR(u,v)       SNOT(IOR(u,v))
#define SNAND(u,v)      SNOT(IAND(u,v))

// comparisons one value at a time
#define BAND(u,v)       (u && v)
#define BGT(u,v)        (u >  v)
#define BLT(u,v)        (u <  v)
#define BOR(u,v)        (u || v)
#define BNE(u,v)        (u != v)
#define BGE(u,v)        (u >= v)
#define BLE(u,v)        (u <= v)
#define BEQ(u,v)        (u == v)
#define BNOR(u,v)       (!(u||v))
#define BNAND(u,v)      (!(u&&v))

// comparisons between LIT types, one word at a time producing bits in v.  work is destroyed
#define CMPEQCC(u,v)    (work=(u)^(v), ZBYTESTOZBITS(work), work=~work, work&=VALIDBOOLEAN)
#define CMPNECC(u,v)    (work=(u)^(v), ZBYTESTOZBITS(work), work&=VALIDBOOLEAN)

#define PLUS(u,v)       ((u)+   (v))
#define PLUSO(u,v)      ((u)+(D)(v))
#define MINUS(u,v)      ((u)-   (v))
#define MINUSO(u,v)     ((u)-(D)(v))
#define TYMES(u,v)      ((u)&&(v)?dmul2(u,v):0)
#define TYMESO(u,v)     ((u)&&(v)?dmul2(u,(D)v):0)
#define DIV(u,v)        ((u)||(v)?ddiv2(u,v):0)

#define SBORDER(v)      (SBUV(v)->order)

#define SBNE(u,v)       (SBORDER(u)!=SBORDER(v))
#define SBLT(u,v)       (SBORDER(u)< SBORDER(v))
#define SBLE(u,v)       (SBORDER(u)<=SBORDER(v))
#define SBGT(u,v)       (SBORDER(u)> SBORDER(v))
#define SBGE(u,v)       (SBORDER(u)>=SBORDER(v))

#define SBMIN(u,v)      (SBORDER(u)<=SBORDER(v)?(u):(v))
#define SBMAX(u,v)      (SBORDER(u)>=SBORDER(v)?(u):(v))

#define BOV(exp)        if(exp){er=EWOV; break;}

#define BW0000(x,y)     (0)
#define BW0001(x,y)     (   (x)& (y) )
#define BW0010(x,y)     (   (x)&~(y) )
#define BW0011(x,y)     (x)

#define BW0100(x,y)     (  ~(x)& (y) )
#define BW0101(x,y)     (y)
#define BW0110(x,y)     (   (x)^ (y) )
#define BW0111(x,y)     (   (x)| (y) )

#define BW1000(x,y)     (~( (x)| (y)))
#define BW1001(x,y)     (~( (x)^ (y)))
#define BW1010(x,y)     (       ~(y) )
#define BW1011(x,y)     (   (x)|~(y) )

#define BW1100(x,y)     (  ~(x)      )
#define BW1101(x,y)     (  ~(x)| (y) )
#define BW1110(x,y)     (~( (x)& (y)))
#define BW1111(x,y)     (-1)

typedef I AHDR1FN(JST * RESTRICT jt,I n,void* z,void* x);
typedef I AHDR2FN(I n,I m,void* RESTRICTI x,void* RESTRICTI y,void* RESTRICTI z,J jt);
typedef I AHDRPFN(I d,I n,I m,void* RESTRICTI x,void* RESTRICTI z,J jt);  // these 3 must be the same for now, for VARPS
typedef I AHDRRFN(I d,I n,I m,void* RESTRICTI x,void* RESTRICTI z,J jt);
typedef I AHDRSFN(I d,I n,I m,void* RESTRICTI x,void* RESTRICTI z,J jt);

#define AHDR1(f,Tz,Tx)          I f(JST * RESTRICT jt,I n,Tz* z,Tx* x)   // must match VA1F, AHDR1FN
#define AMON(f,Tz,Tx,stmt)      AHDR1(f,Tz,Tx){DQ(n, {stmt} ++z; ++x;); R EVOK;}
#define AMONPS(f,Tz,Tx,prefix,stmt,suffix)      AHDR1(f,Tz,Tx){prefix DQ(n, {stmt} ++z; ++x;) suffix}
#define HDR1JERR I rc=jt->jerr; jt->jerr=0; R rc?rc:EVOK;   // translate no error to no-error value
#define HDR1JERRNAN I rc=jt->jerr; rc=NANTEST?EVNAN:rc; jt->jerr=0; R rc?rc:EVOK;   // translate no error to no-error value



#define AHDR2(f,Tz,Tx,Ty)       I f(I n,I m,Tx* RESTRICTI x,Ty* RESTRICTI y,Tz* RESTRICTI z,J jt)  // must match VF, AHDR2FN
#define AHDRP(f,Tz,Tx)          I f(I d,I n,I m,Tx* RESTRICTI x,Tz* RESTRICTI z,J jt)
#define AHDRR(f,Tz,Tx)          I f(I d,I n,I m,Tx* RESTRICTI x,Tz* RESTRICTI z,J jt)
#define AHDRS(f,Tz,Tx)          I f(I d,I n,I m,Tx* RESTRICTI x,Tz* RESTRICTI z,J jt)

// value in vaptr[]
#define VA2CBW0000 1
#define VA2CBW0001 2
#define VA2CBW0010 3
#define VA2CBW0011 4
#define VA2CBW0100 5
#define VA2CBW0101 6
#define VA2CBW0110 7
#define VA2CBW0111 8
#define VA2CBW1000 9
#define VA2CBW1001 10
#define VA2CBW1010 11
#define VA2CBW1011 12
#define VA2CBW1100 13
#define VA2CBW1101 14
#define VA2CBW1110 15
#define VA2CBW1111 16
#define VA2CNE 17 // 35
#define VA2CDIV 18
#define VA2CPLUSCO 19 // 32
#define VA2CPLUSDOT 20 // 31
#define VA2CMINUS 21
#define VA2CLT 22
#define VA2CEQ 23
#define VA2CGT 24
#define VA2CSTARDOT 25 // 33
#define VA2CSTARCO 26 // 34
#define VA2CGE 27 // 30
#define VA2CLE 28
// the following are in the same order in va1
#define VA2CMIN 29 // 27
#define VA2CMAX 30 // 29
#define VA2CPLUS 31 // 20
#define VA2CSTAR 32 // 19
#define VA2CEXP 33 // 25
#define VA2CSTILE 34 // 26
#define VA2CBANG 35 // 17
#define VA2CCIRCLE 36
#define VA1CMIN 29 // 27
#define VA1CMAX 30 // 29
#define VA1CPLUS 31 // 20
#define VA1CSTAR 32 // 19
#define VA1CEXP 33 // 25
#define VA1CSTILE 34 // 26
#define VA1CBANG 35 // 17
#define VA1CCIRCLE 36
#define VA1CROOT 37
#define VA1CLOG 38


/*
 b    1 iff cell rank of a <= cell rank of w
 m    # atoms of in the cell with the smaller rank
 n    excess # of cell atoms
 z    pointer to result atoms
 x    pointer to a      atoms
 y    pointer to w      atoms
*/


#define AIFX(f,Tz,Tx,Ty,symb)  \
 AHDR2(f,Tz,Tx,Ty){Tx u;Ty v;                            \
  if(n-1==0)  DQ(m,               *z++=*x++ symb *y++; )   \
  else if(n-1<0)DQ(m, u=*x++; DQC(n, *z++=u    symb *y++;))   \
  else      DQ(m, v=*y++; DQ(n, *z++=*x++ symb v;   ));  \
  R EVOK; \
 }

// suff must return the correct result
#define APFX(f,Tz,Tx,Ty,pfx,pref,suff)   \
 AHDR2(f,Tz,Tx,Ty){Tx u;Ty v;                                  \
  pref \
  if(n-1==0)  DQ(m,               *z++=pfx(*x,*y); x++; y++; )   \
  else if(n-1<0)DQ(m, u=*x++; DQC(n, *z++=pfx( u,*y);      y++;))   \
  else      DQ(m, v=*y++; DQ(n, *z++=pfx(*x, v); x++;     ));  \
  suff \
 }

// commute=bit0 = commutative, bit1 set if incomplete y must be filled with 0 (to avoid isub oflo), bit2 set if incomplete x must be filled with i (for fdiv NaN) 
#define primop256(name,commute,pref,zzop,suff) \
AHDR2(name,D,D,D){ \
 __m256d xx,yy,zz; \
 __m256i endmask; /* length mask for the last word */ \
 _mm256_zeroupper(VOIDARG); \
   /* will be removed except for divide */ \
 pref \
 if(n-1==0){ \
  /* vector-to-vector, no repetitions */ \
  endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-m)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
  DQ((m-1)>>LGNPAR, xx=_mm256_loadu_pd(x); yy=_mm256_loadu_pd(y); zzop; _mm256_storeu_pd(z, zz); x+=NPAR; y+=NPAR; z+=NPAR;) \
  /* runout, using mask */ \
  xx=_mm256_maskload_pd(x,endmask); yy=_mm256_maskload_pd(y,endmask); if(commute&4)xx=_mm256_blendv_pd(_mm256_set1_pd(1.0),xx,_mm256_castsi256_pd(endmask)); zzop; _mm256_maskstore_pd(z, endmask, zz); \
 }else{ \
  if(!(commute&1)&&n-1<0){n=~n; \
   /* atom+vector */ \
   endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-n)&(NPAR-1)))); \
   DQ(m, xx=_mm256_set1_pd(*x); ++x; \
     DQ((n-1)>>LGNPAR, yy=_mm256_loadu_pd(y); zzop; _mm256_storeu_pd(z, zz); y+=NPAR; z+=NPAR;)  yy=_mm256_maskload_pd(y,endmask); if(commute&4)xx=_mm256_blendv_pd(_mm256_set1_pd(1.0),xx,_mm256_castsi256_pd(endmask)); zzop; _mm256_maskstore_pd(z, endmask, zz); \
     y+=((n-1)&(NPAR-1))+1; z+=((n-1)&(NPAR-1))+1;) \
  }else{ \
   /* vector+atom */ \
   if(commute&1){I taddr=(I)x^(I)y; x=n<0?y:x; y=(D*)((I)x^taddr); n^=REPSGN(n);}; endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-n)&(NPAR-1)))); \
   DQ(m, yy=_mm256_set1_pd(*y); ++y; \
     DQ((n-1)>>LGNPAR, xx=_mm256_loadu_pd(x); zzop; _mm256_storeu_pd(z, zz); x+=NPAR; z+=NPAR;)  xx=_mm256_maskload_pd(x,endmask); if(commute&2)yy=_mm256_blendv_pd(_mm256_castsi256_pd(endmask),yy,_mm256_castsi256_pd(endmask)); zzop; _mm256_maskstore_pd(z, endmask, zz); \
     x+=((n-1)&(NPAR-1))+1; z+=((n-1)&(NPAR-1))+1;) \
  } \
 } \
 suff \
}

/* Embedded visual tools v3.0 fails perform the z++ on all wince platforms. -KBI */
#if SY_WINCE
#define ACMP(f,Tz,Tx,Ty,pfx)   \
 AHDR2(f,B,Tx,Ty){D u,v;                                          \
  if(n-1==0)  DQ(m, u=(D)*x++;       v=(D)*y++; *z++=pfx(u,v); )    \
  else if(n-1<0)DQ(m, u=(D)*x++; DQC(n, v=(D)*y++; *z++=pfx(u,v);))    \
  else      DQ(m, v=(D)*y++; DQ(n, u=(D)*x++; *z++=pfx(u,v);));   \
 }
#else
#define ACMP(f,Tz,Tx,Ty,pfx)   \
 AHDR2(f,B,Tx,Ty){D u,v;                                             \
  if(n-1==0)  DQ(m, u=(D)*x++;       v=(D)*y++; *z=pfx(u,v); z++; )    \
  else if(n-1<0)DQ(m, u=(D)*x++; DQC(n, v=(D)*y++; *z=pfx(u,v); z++;))    \
  else      DQ(m, v=(D)*y++; DQ(n, u=(D)*x++; *z=pfx(u,v); z++;));   \
  R EVOK; \
 }
#endif
// support intolerant comparisons explicitly
#define ACMP0(f,Tz,Tx,Ty,pfx,pfx0)   \
 AHDR2(f,B,Tx,Ty){D u,v;                                             \
  if(jt->cct!=1.0){ \
   if(n-1==0)  DQ(m, u=(D)*x++;       v=(D)*y++; *z=pfx(u,v); z++; )    \
   else if(n-1<0)DQ(m, u=(D)*x++; DQC(n, v=(D)*y++; *z=pfx(u,v); z++;))    \
   else      DQ(m, v=(D)*y++; DQ(n, u=(D)*x++; *z=pfx(u,v); z++;));   \
  }else{ \
   if(n-1==0)  DQ(m, u=(D)*x++;       v=(D)*y++; *z=u pfx0 v; z++; )    \
   else if(n-1<0)DQ(m, u=(D)*x++; DQC(n, v=(D)*y++; *z=u pfx0 v; z++;))    \
   else      DQ(m, v=(D)*y++; DQ(n, u=(D)*x++; *z=u pfx0 v; z++;));   \
  } \
  R EVOK; \
 }


// n and m are never 0.
#if 0 // waiting till me learn how to XCTL
static void f##1(J jt,I m,void* RESTRICTI z,void* RESTRICTI x,void* RESTRICTI y){I u,v; \
 if((C_AVX&&SY_64)||EMU_AVX){__m256i u256,v256; \
  __m256i bool256=_mm256_set1_epi64x(0x0101010101010101); /* valid boolean bits */ \
  __m256i workarea; workarea=_mm256_xor_si256(bool256,bool256); /* temp, init to avoid warning */ \
  DQ((m-1)>>(LGSZI+LGNPAR), u256=_mm256_loadu_si256((__m256i*)x); v256=_mm256_loadu_si256((__m256i*)y); \
   _mm256_storeu_si256((__m256i*)z, fuv); x=(C*)x+NPAR*SZI; y=(C*)y+NPAR*SZI; z=(C*)z+NPAR*SZI; \
  ) \
 } \
 DQ(((m-1)>>LGSZI)&(((C_AVX&&SY_64)||EMU_AVX)?(NPAR-1):-1), u=*(I*)x; v=*(I*)y; *(I*)z=pfx(u,v); x=(C*)x+SZI; y=(C*)y+SZI; z=(C*)z+SZI;);           \
 u=*(I*)x; v=*(I*)y; u=pfx(u,v); STOREBYTES(z,u,(-m)&(SZI-1));  \
}
#endif

#define BPFXNOAVX(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256)  \
AHDR2(f,void,void,void){ I u,v;       \
 decls \
 if(n-1==0){                                             \
  DQ(((m-1)>>LGSZI), u=*(I*)x; v=*(I*)y; *(I*)z=pfx(u,v); x=(C*)x+SZI; y=(C*)y+SZI; z=(C*)z+SZI;);           \
  u=*(I*)x; v=*(I*)y; u=pfx(u,v); STOREBYTES(z,u,(-m)&(SZI-1));  \
 }else if(n-1<0){n=~n;                      \
  DQ(m, \
   REPLBYTETOW(*(C*)x,u); x=(C*)x+1; \
   DQ(((n-1)>>LGSZI), v=*(I*)y; *(I*)z=pfx(u,v); y=(C*)y+SZI; z=(C*)z+SZI;)           \
   v=*(I*)y; u=pfx(u,v); STOREBYTES(z,u,(-n)&(SZI-1)); y=(I*)((UC*)y+(((n-1)&(SZI-1))+1)); z=(I*)((UC*)z+(((n-1)&(SZI-1))+1)); \
  ) \
 }else{  \
  DQ(m, \
   REPLBYTETOW(*(C*)y,v); y=(C*)y+1; \
   DQ(((n-1)>>LGSZI), u=*(I*)x; *(I*)z=pfx(u,v); x=(C*)x+SZI; z=(C*)z+SZI;)         \
   u=*(I*)x; u=pfx(u,v); STOREBYTES(z,u,(-n)&(SZI-1)); x=(I*)((UC*)x+(((n-1)&(SZI-1))+1)); z=(I*)((UC*)z+(((n-1)&(SZI-1))+1)); \
  ) \
 } \
 R EVOK; \
}

#if (C_AVX&&SY_64) || EMU_AVX
#define BPFX(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256)  \
AHDR2(f,void,void,void){ I u,v;       \
 decls  \
 if(n-1==0){                                             \
  if((m-1)>>(LGSZI+LGNPAR)){__m256d u256,v256; decls256 \
   DQU((m-1)>>(LGSZI+LGNPAR), u256=_mm256_loadu_pd(x); v256=_mm256_loadu_pd(y); \
    _mm256_storeu_pd(z, fuv); x=(C*)x+NPAR*SZI; y=(C*)y+NPAR*SZI; z=(C*)z+NPAR*SZI; \
   ) \
  } \
  DQ(((m-1)>>LGSZI)&(NPAR-1), u=*(I*)x; v=*(I*)y; *(I*)z=pfx(u,v); x=(C*)x+SZI; y=(C*)y+SZI; z=(C*)z+SZI;);           \
  u=*(I*)x; v=*(I*)y; u=pfx(u,v); STOREBYTES(z,u,(-m)&(SZI-1));  \
 }else if(n-1<0){n=~n;                      \
  DQ(m, \
   REPLBYTETOW(*(C*)x,u); x=(C*)x+1; \
   if((n-1)>>(LGSZI+LGNPAR)){__m256d u256;__m256d v256; decls256 \
    u256=_mm256_castsi256_pd(_mm256_set1_epi64x(u)); \
    DQU((n-1)>>(LGSZI+LGNPAR), v256=_mm256_loadu_pd(y); \
     _mm256_storeu_pd(z, fuv); y=(C*)y+NPAR*SZI; z=(C*)z+NPAR*SZI; \
    ) \
   } \
   DQ(((n-1)>>LGSZI)&(NPAR-1), v=*(I*)y; *(I*)z=pfx(u,v); y=(C*)y+SZI; z=(C*)z+SZI;)           \
   v=*(I*)y; u=pfx(u,v); STOREBYTES(z,u,(-n)&(SZI-1)); y=(I*)((UC*)y+(((n-1)&(SZI-1))+1)); z=(I*)((UC*)z+(((n-1)&(SZI-1))+1)); \
  ) \
 }else{  \
  DQ(m, \
   REPLBYTETOW(*(C*)y,v); y=(C*)y+1; \
   if((n-1)>>(LGSZI+LGNPAR)){__m256d u256;__m256d v256; decls256 \
    v256=_mm256_castsi256_pd(_mm256_set1_epi64x(v)); \
    DQU((n-1)>>(LGSZI+LGNPAR), u256=_mm256_loadu_pd(x); \
     _mm256_storeu_pd(z, fuv); x=(C*)x+NPAR*SZI; z=(C*)z+NPAR*SZI; \
    ) \
   } \
   DQ(((n-1)>>LGSZI)&(NPAR-1), u=*(I*)x; *(I*)z=pfx(u,v); x=(C*)x+SZI; z=(C*)z+SZI;)         \
   u=*(I*)x; u=pfx(u,v); STOREBYTES(z,u,(-n)&(SZI-1)); x=(I*)((UC*)x+(((n-1)&(SZI-1))+1)); z=(I*)((UC*)z+(((n-1)&(SZI-1))+1)); \
  ) \
 } \
 R EVOK; \
}

#if C_AVX2 || EMU_AVX2
#define BPFXAVX2(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256) BPFX(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256)
#else
#define BPFXAVX2(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256) BPFXNOAVX(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256)
#endif
#else
#define BPFXAVX2(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256) BPFXNOAVX(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256)
#define BPFX(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256) BPFXNOAVX(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256)
#endif
