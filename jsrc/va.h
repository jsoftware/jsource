/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Macros and Defined Constants for Atomic (Scalar) Verbs           */

typedef struct {VA2 p2[13];VA2 pins[7];VA2 ppfx[7];VA2 psfx[7];} VA;
typedef struct {VA2 p1[6];} UA;

                                    /*   cv - control vector               */
#define VIPOKWX         0      // This routine can put its result over W
#define VIPOKW          (1<<VIPOKWX)
#define VIPOKAX         1      // This routine can put its result over A
#define VIPOKA          (1<<VIPOKAX)
// bits 2-3 kept open for flags in va2
#define VARGX           4           // bit position for arg flags
#define VBB             (B01<<VARGX)         /* convert arguments to B              */
#define VII             (INT<<VARGX)         /* convert arguments to I              */
#define VDD             (FL<<VARGX)          /* convert arguments to D              */
#define VZZ             (CMPX<<VARGX)        /* convert arguments to Z              */
#define Vxx             (XNUM<<VARGX)        /* convert arguments to XNUM           */
#define VQQ             (RAT<<VARGX)         /* convert arguments to RAT            */
#define VARGMSK         (VBB|VII|VDD|VZZ|Vxx|VQQ)  // mask for argument requested type
#define VRESX           12           // bit position for result flags
#define VB              (B01<<VRESX)/* result type B                       */
#define VI              (INT<<VRESX)/* result type I                       */
#define VD              (FL<<VRESX) /* result type D                       */
#define VZ              (CMPX<<VRESX)/* result type Z                       */
#define VX              (XNUM<<VRESX)/* result type XNUM                    */
#define VQ              (RAT<<VRESX) /* result type RAT                     */
#define VSB             (SBT<<VRESX) /* result type SBT                     */
#define VRESMSK         (VB|VI|VD|VZ|VX|VQ|VSB)  // mask for result-type
#define VRD             (SLIT<<VRESX)// convert result to D if possible - unused code point
#define VRI             (SBOX<<VRESX)// convert result to I if possible - unused code point
// bits VRESX+8 9 10 12 13 14 are free
#define VXCVTYPEX       29          // bit position for VX conversion type
#define VXCVTYPEMSK     ((I)3<<VXCVTYPEX)  // mask for bit-positions hold XNUM conversion type
#define VXX             (Vxx|((I)XMEXACT<<VXCVTYPEX))  // exact conversion
#define VXEQ            (Vxx|((I)XMEXMT<<VXCVTYPEX))   /* convert to XNUM for = ~:            */
#define VXCF            (Vxx|((I)XMCEIL<<VXCVTYPEX))   /* convert to XNUM ceiling/floor       */
#define VXFC            (Vxx|((I)XMFLR<<VXCVTYPEX))  /* convert to XNUM floor/ceiling       */
#define VCANHALTX       31    // This routine can generate an error after it has started
#define VCANHALT        (1<<VCANHALTX)

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

typedef void AHDR1FN(JST * RESTRICT jt,I n,void* z,void* x);
typedef void AHDR2FN(I n,I m,void* RESTRICTI x,void* RESTRICTI y,void* RESTRICTI z,J jt);
typedef void AHDRPFN(I d,I n,I m,void* RESTRICTI x,void* RESTRICTI z,J jt);
typedef void AHDRRFN(I d,I n,I m,void* RESTRICTI x,void* RESTRICTI z,J jt);
typedef void AHDRSFN(I d,I n,I m,void* RESTRICTI x,void* RESTRICTI z,J jt);

#define AHDR1(f,Tz,Tx)          void f(JST * RESTRICT jt,I n,Tz* z,Tx* x)
#define AHDR2(f,Tz,Tx,Ty)       void f(I n,I m,Tx* RESTRICTI x,Ty* RESTRICTI y,Tz* RESTRICTI z,J jt)
#define AHDRP(f,Tz,Tx)          void f(I d,I n,I m,Tx* RESTRICTI x,Tz* RESTRICTI z,J jt)
#define AHDRR(f,Tz,Tx)          void f(I d,I n,I m,Tx* RESTRICTI x,Tz* RESTRICTI z,J jt)
#define AHDRS(f,Tz,Tx)          void f(I d,I n,I m,Tx* RESTRICTI x,Tz* RESTRICTI z,J jt)

// value in vaptr[]
#define VA2B0 1
#define VA2B1 2
#define VA2B2 3
#define VA2B3 4
#define VA2B4 5
#define VA2B5 6
#define VA2B6 7
#define VA2B7 8
#define VA2B8 9
#define VA2B9 10
#define VA2BA 11
#define VA2BB 12
#define VA2BC 13
#define VA2BD 14
#define VA2BE 15
#define VA2BF 16
#define VA2NE 17 // 35
#define VA2DIV 18
#define VA2NOR 19 // 32
#define VA2GCD 20 // 31
#define VA2MINUS 21
#define VA2LT 22
#define VA2EQ 23
#define VA2GT 24
#define VA2LCM 25 // 33
#define VA2NAND 26 // 34
#define VA2GE 27 // 30
#define VA2LE 28
// the following are in the same order in va1
#define VA2MIN 29 // 27
#define VA2MAX 30 // 29
#define VA2PLUS 31 // 20
#define VA2MULT 32 // 19
#define VA2POW 33 // 25
#define VA2RESIDUE 34 // 26
#define VA2OUTOF 35 // 17
#define VA2CIRCLE 36
#define VA1ROOT 37
#define VA1LOG 38


/*
 b    1 iff cell rank of a <= cell rank of w
 m    # atoms of in the cell with the smaller rank
 n    excess # of cell atoms
 z    pointer to result atoms
 x    pointer to a      atoms
 y    pointer to w      atoms
*/

#define AMON(f,Tz,Tx,stmt)      AHDR1(f,Tz,Tx){DQ(n, {stmt} ++z; ++x;);}
#define AMONPS(f,Tz,Tx,prefix,stmt,suffix)      AHDR1(f,Tz,Tx){prefix DQ(n, {stmt} ++z; ++x;) suffix}

#define AIFX(f,Tz,Tx,Ty,symb)  \
 AHDR2(f,Tz,Tx,Ty){Tx u;Ty v;                            \
  if(n-1==0)  DQ(m,               *z++=*x++ symb *y++; )   \
  else if(n-1<0)DQ(m, u=*x++; DQC(n, *z++=u    symb *y++;))   \
  else      DQ(m, v=*y++; DQ(n, *z++=*x++ symb v;   ));  \
 }

#define AOVF(f,Tz,Tx,Ty,fvv,f1v,fv1)  \
 AHDR2(f,I,I,I){C er=0;I u,v,*x1,*y1,*z1;                                       \
  if(n-1==0)  {fvv(m,z,x,y); RER;}                                                \
  else if(n-1<0){z1=z; y1=y; n=~n; DQ(m, u=*x++; f1v(n,z,u,y); RER; z=z1+=n; y=y1+=n;);}  \
  else      {z1=z; x1=x; DQ(m, v=*y++; fv1(n,z,x,v); RER; z=z1+=n; x=x1+=n;);}  \
 }

#define APFX(f,Tz,Tx,Ty,pfx)   \
 AHDR2(f,Tz,Tx,Ty){Tx u;Ty v;                                  \
  if(n-1==0)  DQ(m,               *z++=pfx(*x,*y); x++; y++; )   \
  else if(n-1<0)DQ(m, u=*x++; DQC(n, *z++=pfx( u,*y);      y++;))   \
  else      DQ(m, v=*y++; DQ(n, *z++=pfx(*x, v); x++;     ));  \
 }

#define ANAN(f,Tz,Tx,Ty,pfx)   \
 AHDR2(f,Tz,Tx,Ty){Tx u;Ty v;                                  \
  NAN0;                                                        \
  if(n-1==0)  DQ(m,               *z++=pfx(*x,*y); x++; y++; )   \
  else if(n-1<0)DQ(m, u=*x++; DQC(n, *z++=pfx( u,*y);      y++;))   \
  else      DQ(m, v=*y++; DQ(n, *z++=pfx(*x, v); x++;     ));  \
  NAN1V;                                                       \
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
 }


// n and m are never 0.
#if 0 // waiting till me learn how to XCTL
static void f##1(J jt,I m,void* RESTRICTI z,void* RESTRICTI x,void* RESTRICTI y){I u,v; \
 if(C_AVX&&SY_64){__m256i u256,v256; \
  __m256i bool256=_mm256_set1_epi64x(0x0101010101010101); /* valid boolean bits */ \
  __m256i workarea; workarea=_mm256_xor_si256(bool256,bool256); /* temp, init to avoid warning */ \
  DQ((m-1)>>(LGSZI+LGNPAR), u256=_mm256_loadu_si256((__m256i*)x); v256=_mm256_loadu_si256((__m256i*)y); \
   _mm256_storeu_si256((__m256i*)z, fuv); x=(C*)x+NPAR*SZI; y=(C*)y+NPAR*SZI; z=(C*)z+NPAR*SZI; \
  ) \
 } \
 DQ(((m-1)>>LGSZI)&(C_AVX&&SY_64?(NPAR-1):-1), u=*(I*)x; v=*(I*)y; *(I*)z=pfx(u,v); x=(C*)x+SZI; y=(C*)y+SZI; z=(C*)z+SZI;);           \
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
}

#if C_AVX&&SY_64
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
}

#if C_AVX2
#define BPFXAVX2(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256) BPFX(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256)
#else
#define BPFXAVX2(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256) BPFXNOAVX(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256)
#endif
#else
#define BPFXAVX2(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256) BPFXNOAVX(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256)
#define BPFX(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256) BPFXNOAVX(f,pfx,bpfx,pfyx,bpfyx,fuv,decls,decls256)
#endif
