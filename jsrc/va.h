/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Macros and Defined Constants for Atomic (Scalar) Verbs           */

                                    /*   cv - control vector               */
// bits 0-1 kept open for jtflags
// bits 2-3 should be forced to 1 jtflags;
#define VCVTIP          0xc  // bits 2-3 should always be set, indicating that a converted argument can be inplaced
#define VARGX           4           // bit position for arg flags
#define VBB             (B01<<VARGX)         /* convert arguments to B 4             */
#define VII             (INT<<VARGX)         /* convert arguments to I 5             */
#define VDD             (FL<<VARGX)          /* convert arguments to D 6             */
#define VZZ             (CMPX<<VARGX)        /* convert arguments to Z 7             */
#define VIPWCRLONGX     9  // internal use in va2, overlaps BOX 9
#define VIPWCRLONG      ((I)1<<VIPWCRLONGX)
#define Vxx             (XNUM<<VARGX)        /* convert arguments to XNUM 10           */
#define VQQ             (RAT<<VARGX)         /* convert arguments to RAT  11          */
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
#define VRD             (0x800<<VRESX)// convert result to D if possible 23
#define VRI             (0x8000<<VRESX)// convert result to I if possible  27
// bits VRESX+ 1 10 12 16 are free 13 22 24 29-30
#define VIPWFLONGX     17  //  internal use in va2.  Spaced RANKTX from VIPWCRLONGX
#define VIPWFLONG      ((I)1<<VIPWFLONGX)
#define VIPOKWX         20      // This routine can put its result over W
#define VIPOKW          ((I)1<<VIPOKWX)
#define VIPOKAX         21      // This routine can put its result over A
#define VIPOKA          ((I)1<<VIPOKAX)
#define VCANHALTX       25    // This routine can generate an error after it has started
#define VCANHALT        ((I)1<<VCANHALTX)
#define VXCHASVTYPEX    26  // set if there is forced conversion to XNUM =CONW
#define VXCHASVTYPE     ((I)1<<VXCHASVTYPEX)
#define VXX             (Vxx|XMODETOCVT((I)XMEXACT))  // exact conversion
#define VXEQ            (Vxx|XMODETOCVT((I)XMEXMT))   /* convert to XNUM for = ~:            */
#define VXCF            (Vxx|XMODETOCVT((I)XMCEIL))   /* convert to XNUM ceiling/floor       */
#define VXFC            (Vxx|XMODETOCVT((I)XMFLR))  /* convert to XNUM floor/ceiling       */
// bit 31 must not be used - it may be a sign bit, which has a meaning
#define VFRCEXMT        XMODETOCVT((I)XMEXMT)   // set in arg to cvt() to do rounding for = ~:, if the conversion happens to be to XNUM
// upper bits for 64-bit va2
#define VIPOKRNKWX         28      // filled by va2 if the ranks allow inplacing w
#define VIPOKRNKW          ((I)1<<VIPOKRNKWX)
#define VIPOKRNKAX         30      // filled by va2 if the ranks allow inplacing a
#define VIPOKRNKA          ((I)1<<VIPOKRNKAX)

// Extract the argument-conversion type from cv coming from the table
#define atype(x) (((x)>>VARGX)&(VARGMSK>>VARGX))

// Extract the result type from cv coming from the table
#define rtype(x) (((x)>>VRESX)&(VRESMSK>>VRESX))

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

#define BW0000(x,y)     (0&(x)&(y))
#define BW0001(x,y)     (   (x)& (y) )
#define BW0010(x,y)     (   (x)&~(y) )
#define BW0011(x,y)     (x|(0&(y)))

#define BW0100(x,y)     (  ~(x)& (y) )
#define BW0101(x,y)     (y|(0&(x)))
#define BW0110(x,y)     (   (x)^ (y) )
#define BW0111(x,y)     (   (x)| (y) )

#define BW1000(x,y)     (~( (x)| (y)))
#define BW1001(x,y)     (~( (x)^ (y)))
#define BW1010(x,y)     ( ~(y)|(0&x) )
#define BW1011(x,y)     (   (x)|~(y) )

#define BW1100(x,y)     (  ~(x)|(0&y)      )
#define BW1101(x,y)     (  ~(x)| (y) )
#define BW1110(x,y)     (~( (x)& (y)))
#define BW1111(x,y)     (-1|(x)|(y))

typedef I AHDR1FN(J RESTRICT jt,I n,void* z,void* x);  // negative return is offset to failure point in >. or <.
typedef I AHDR2FN(I n,I m,void* RESTRICTI x,void* RESTRICTI y,void* RESTRICTI z,J jt);  // negative return is failure point for integer multiply
typedef I AHDRPFN(I d,I n,I m,void* RESTRICTI x,void* RESTRICTI z,J jt);  // these 3 must be the same for now, for VARPS.  The return is never negative
typedef I AHDRRFN(I d,I n,I m,void* RESTRICTI x,void* RESTRICTI z,J jt);
typedef I AHDRSFN(I d,I n,I m,void* RESTRICTI x,void* RESTRICTI z,J jt);

#define AHDR1(f,Tz,Tx)          I f(J RESTRICT jt,I n,Tz* z,Tx* x)   // must match VA1F, AHDR1FN
#define AMON(f,Tz,Tx,stmt)      AHDR1(f,Tz,Tx){DQ(n, {stmt} ++z; ++x;); R EVOK;}
#define AMONPS(f,Tz,Tx,prefix,stmt,suffix)      AHDR1(f,Tz,Tx){prefix DQ(n, {stmt} ++z; ++x;) suffix}
#define HDR1JERR I rc=jt->jerr; jt->jerr=0; R rc?rc:EVOK;   // translate no error to no-error value
#define HDR1JERRNAN I rc=jt->jerr; rc=NANTEST?EVNAN:rc; jt->jerr=0; R rc?rc:EVOK;   // translate no error to no-error value



#define AHDR2(f,Tz,Tx,Ty)       I f(I n,I m,Tx* RESTRICTI x,Ty* RESTRICTI y,Tz* RESTRICTI z,J jt)  // must match VF, AHDR2FN  n is #repeats of arg; if n neg, repeat x ~n times.  m is # times to repeat an n-cell
  // Calculate m: #cells of w to operate on; d: #atoms in an item of a cell of w (a cell to which u is applied);
 // Create  r: the effective rank; f: length of frame; n: # items in a CELL of w
#define AHDRP(f,Tz,Tx)          I f(I d,I n,I m,Tx* RESTRICTI x,Tz* RESTRICTI z,J jt)  // m is # cells to operate on; n is # items in 1 such cell; d is # atoms in one such item
#define AHDRR(f,Tz,Tx)          I f(I d,I n,I m,Tx* RESTRICTI x,Tz* RESTRICTI z,J jt)  // m is # cells to operate on; n is # items in 1 such cell; d is # atoms in one such item
#define AHDRS(f,Tz,Tx)          I f(I d,I n,I m,Tx* RESTRICTI x,Tz* RESTRICTI z,J jt)  // m is # cells to operate on; n is # items in 1 such cell; d is # atoms in one such item

// value in vaptr[]
// first part: verbs that are atomic only in the dyad
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
#define VA2CNE 17
#define VA2CDIV 18
#define VA2CPLUSCO 19
#define VA2CPLUSDOT 20
#define VA2CMINUS 21
#define VA2CLT 22
#define VA2CEQ 23
#define VA2CGT 24
#define VA2CSTARDOT 25
#define VA2CSTARCO 26
#define VA2CGE 27
#define VA2CLE 28
// shared part: verbs that are atomic in monad and dyad
// the following are in the same order in va1
#define VA2CMIN 29
#define VA2CMAX 30
#define VA2CPLUS 31
#define VA2CSTAR 32
#define VA2CEXP 33
#define VA2CSTILE 34
#define VA2CBANG 35
#define VA2CCIRCLE 36
// the following are used only for execution, not definition, and only for singletons
#define VA2CEQABS 37  // all the entires in va[] share this block
#define VA2CNEABS 38
#define VA2CLTABS 39
#define VA2CLEABS 40
#define VA2CGEABS 41
#define VA2CGTABS 42
// end of the dyads
#define VA1ORIGIN 29 // the start of the monadic section
#define VA1CMIN 29
#define VA1CMAX 30
#define VA1CPLUS 31
#define VA1CSTAR 32
#define VA1CEXP 33
#define VA1CSTILE 34
#define VA1CBANG 35
#define VA1CCIRCLE 36
// last part: verbs atomic only on the monad
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
#define APFXL(f,Tz,Tx,Ty,ldfn,pfx,pref,suff)   \
 AHDR2(f,Tz,Tx,Ty){Tx u;Ty v;                                  \
  pref \
  if(n-1==0)  DQ(m, u=ldfn(*x++); v=ldfn(*y++); *z++=pfx(u,v); )   \
  else if(n-1<0)DQ(m, u=ldfn(*x++); DQC(n, v=ldfn(*y++); *z++=pfx(u,v);))   \
  else      DQ(m,  v=ldfn(*y++); DQ(n, u=ldfn(*x++); *z++=pfx(u, v);    ));  \
  suff \
 }

#define APFX(f,Tz,Tx,Ty,pfx,pref,suff)  APFXL(f,Tz,Tx,Ty,,pfx,pref,suff)

// TUNE as of 20210330 Skylake 2.5GHz
// measurements with  2.5e9*(#i)%~(*/$a0)%~6!:2'3 : ''for. i do. a2>.a3 end. 0'' 0'  on length 1e3, aligned or not
// units: cycles/word on +
// shortest loop time unrolled 4 times < 0.23
//                             2 times 0.28
//                             0 times 0.40
// times: not inplace 0.55, inplace aligned 0.34, inplace unaligned input 0.55, 1address 0.23.  Unaligned result 2.3
// conclusion: unroll by 4 on 1address , by 2 on 2-3 address
// This was done with poor choice of address modes; needs to be rerun

// fz=bit0 = commutative, bit1 set if incomplete y must be filled with 0 (to avoid isub oflo), bit2 set if incomplete x must be filled with i (for fdiv NaN),
// bit3 set for int-to-float on x, bit4 for int-to-float on y
// bit5 set to suppress loop-unrolling
// bit6 set for bool-to-int on x, bit7 for bool-to-int on y
// bit8 set for bool-to-float on x, bit9 for bool-to-float on y
// bit10 this is a boolean multiply function: skip if repeated op true
// bit11 this is a boolean addition function: skip if repeated op false
// bit12 this is a comparison combination
// 

// do one computation. xy bit 0 means fetch/incr y, bit 1 means fetch/incr x.  lineno is the offset to the row being worked on
#define PRMDO(zzop,xy,fz,lineno)  if(xy&2)LDBID(xx,OFFSETBID(x,lineno*NPAR,fz,0x8,0x40,0x100),fz,0x8,0x40,0x100) if(xy&1)LDBID(yy,OFFSETBID(y,lineno*NPAR,fz,0x10,0x80,0x200),fz,0x10,0x80,0x200)  \
     if(xy&2)CVTBID(xx,xx,fz,0x8,0x40,0x100) if(xy&1)CVTBID(yy,yy,fz,0x10,0x80,0x200)  \
     zzop; _mm256_storeu_pd(OFFSETBID(z,lineno*NPAR,0,0,0,0), zz);

#define PRMINCR(xy,fz,ct) if(xy&2)INCRBID(x,ct,fz,0x8,0x40,0x100) if(xy&1)INCRBID(y,ct,fz,0x10,0x80,0x200) INCRBID(z,ct,0,0,0,0)

#define PRMALIGN(zzop,xy,fz,len)  I alignreq=(-(I)z>>LGSZI)&(NPAR-1); \
  if((-alignreq&(8*NPAR-len))<0){ \
   endmask = _mm256_loadu_si256((__m256i*)(validitymask+NPAR-alignreq));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
   if(xy&2)LDBID(xx,x,fz,0x8,0x40,0x100) if(xy&1)LDBID(yy,y,fz,0x10,0x80,0x200)  \
   if(xy&2)CVTBID(xx,xx,fz,0x8,0x40,0x100) if(xy&1)CVTBID(yy,yy,fz,0x10,0x80,0x200)  \
   zzop; _mm256_maskstore_pd(z, endmask, zz); PRMINCR(xy,fz,alignreq)  /* need mask store in case inplace */ \
   len-=alignreq;  /* leave remlen>0 */ \
  } \
  endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-len)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */


#define PRMDUFF(zzop,xy,fz,len,lpmsk) \
     if(!((fz)&(lpmsk))){ \
      UI n2=DUFFLPCT(len-1,3);  /* # turns through duff loop */ \
      if(n2>0){ \
       UI backoff=DUFFBACKOFF(len-1,3); \
       PRMINCR(xy,fz,(backoff+1)*NPAR) \
       switch(backoff){ \
       do{ \
       case -1: PRMDO(zzop,xy,fz,0) case -2: PRMDO(zzop,xy,fz,1) case -3: PRMDO(zzop,xy,fz,2) case -4: PRMDO(zzop,xy,fz,3) case -5: PRMDO(zzop,xy,fz,4) case -6: PRMDO(zzop,xy,fz,5) case -7: PRMDO(zzop,xy,fz,6) case -8: PRMDO(zzop,xy,fz,7) \
       PRMINCR(xy,fz,8*NPAR) \
       }while(--n2!=0); \
       } \
      } \
     }else{ \
      DQNOUNROLL((len-1)>>LGNPAR, \
       PRMDO(zzop,xy,fz,0) PRMINCR(xy,fz,NPAR)  \
      ) \
     } \

#define PRMMASK(zzop,xy,fz) if(xy&2)LDBIDM(xx,x,fz,0x8,0x40,0x100,endmask) if(xy&1)LDBIDM(yy,y,fz,0x10,0x80,0x200,endmask)  \
  if(xy&2)CVTBID(xx,xx,fz,0x8,0x40,0x100) if(xy&1)CVTBID(yy,yy,fz,0x10,0x80,0x200)  \
  if((fz)&2)yy=_mm256_blendv_pd(_mm256_castsi256_pd(endmask),yy,_mm256_castsi256_pd(endmask)); \
  if((fz)&4)xx=_mm256_blendv_pd(_mm256_broadcast_sd(&zone.real),xx,_mm256_castsi256_pd(endmask)); \
  zzop; _mm256_maskstore_pd(z, endmask, zz);


#define primop256(name,fz,pref,zzop,suff) \
I name(I n,I m,void* RESTRICTI x,void* RESTRICTI y,void* RESTRICTI z,J jt){ \
 __m256d xx,yy,zz; \
 __m256i endmask; /* length mask for the last word */ \
 _mm256_zeroupperx(VOIDARG) \
   /* will be removed except for divide */ \
 CVTEPI64DECLS pref \
 if(n-1==0){ \
  /* vector-to-vector, no repetitions */ \
  /* align dest to NPAR boundary, if needed and len makes it worthwhile */ \
  PRMALIGN(zzop,3,fz,m)  /* this changes m */ \
  PRMDUFF(zzop,3,fz,m,32+16+8) \
  PRMMASK(zzop,3,fz) /* runout, using mask */ \
 }else{ \
  if(!((fz)&1)&&n-1<0){n=~n; \
   /* atom+vector */ \
   DQNOUNROLL(m, \
    if(unlikely((fz)&0x140 && (((fz)&0x400 && z==y && *(C*)x!=0) || ((fz)&0x800 && z==y && *(C*)x==0)))){ \
     INCRBID(x,1,fz,0x8,0x40,0x100) INCRBID(y,n,fz,0x10,0x80,0x200) INCRBID(z,n,fz,0,0,0) \
    }else{ \
     LDBID1(xx,x,fz,0x8,0x40,0x100) CVTBID1(xx,xx,fz,0x8,0x40,0x100) INCRBID(x,1,fz,0x8,0x40,0x100) \
     I n0=n; \
     PRMALIGN(zzop,1,fz,n0) /* changes n0 */\
     PRMDUFF(zzop,1,fz,n0,32+16) \
     PRMMASK(zzop,1,fz) /* runout, using mask */ \
     PRMINCR(1,fz,((n0-1)&(NPAR-1))+1)  \
    } \
   ) \
  }else{ \
   /* vector+atom */ \
   if((fz)&1){I taddr=(I)x^(I)y; x=n<0?y:x; y=(D*)((I)x^taddr); n^=REPSGN(n);} \
   DQNOUNROLL(m, \
    if(unlikely((fz)&0x280 && (((fz)&0x400 && z==x && *(C*)y!=0) || ((fz)&0x800 && z==x && *(C*)y==0)))){ \
     INCRBID(x,n,fz,0x8,0x40,0x100) INCRBID(y,1,fz,0x10,0x80,0x200) INCRBID(z,n,fz,0,0,0) \
    }else { \
     LDBID1(yy,y,fz,0x10,0x80,0x200) CVTBID1(yy,yy,fz,0x10,0x80,0x200) INCRBID(y,1,fz,0x10,0x80,0x200) \
     I n0=n; \
     PRMALIGN(zzop,2,fz,n0) /* changes n0 */ \
     PRMDUFF(zzop,2,fz,n0,32+8) \
     PRMMASK(zzop,2,fz) /* runout, using mask */ \
     PRMINCR(2,fz,((n0-1)&(NPAR-1))+1)  \
    } \
   ) \
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
// support intolerant comparisons explicitly, with absolute value of y
#define ACMP0ABS(f,Tz,Tx,Ty,pfx,pfx0)   \
 AHDR2(f,B,Tx,Ty){D u,v;                                             \
  if(jt->cct!=1.0){ \
   if(n-1==0)  DQ(m, u=(D)*x++;       v=(D)*y++; v=ABS(v); *z=pfx(u,v); z++; )    \
   else if(n-1<0)DQ(m, u=(D)*x++; DQC(n, v=(D)*y++; v=ABS(v); *z=pfx(u,v); z++;))    \
   else      DQ(m, v=(D)*y++; v=ABS(v); DQ(n, u=(D)*x++; *z=pfx(u,v); z++;));   \
  }else{ \
   if(n-1==0)  DQ(m, u=(D)*x++;       v=(D)*y++; v=ABS(v); *z=u pfx0 v; z++; )    \
   else if(n-1<0)DQ(m, u=(D)*x++; DQC(n, v=(D)*y++; v=ABS(v); *z=u pfx0 v; z++;))    \
   else      DQ(m, v=(D)*y++; v=ABS(v); DQ(n, u=(D)*x++; *z=u pfx0 v; z++;));   \
  } \
  R EVOK; \
 }



// n and m are never 0.
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
   __m256d u256;__m256d v256; u256=_mm256_castsi256_pd(_mm256_set1_epi8(*(C*)x)); \
   if((n-1)>>(LGSZI+LGNPAR)){decls256 \
    DQU((n-1)>>(LGSZI+LGNPAR), v256=_mm256_loadu_pd(y); \
     _mm256_storeu_pd(z, fuv); y=(C*)y+NPAR*SZI; z=(C*)z+NPAR*SZI; \
    ) \
   } \
   u=_mm_extract_epi64(_mm256_castsi256_si128(_mm256_castpd_si256(u256)),0); x=(C*)x+1; \
   DQ(((n-1)>>LGSZI)&(NPAR-1), v=*(I*)y; *(I*)z=pfx(u,v); y=(C*)y+SZI; z=(C*)z+SZI;)           \
   v=*(I*)y; u=pfx(u,v); STOREBYTES(z,u,(-n)&(SZI-1)); y=(I*)((UC*)y+(((n-1)&(SZI-1))+1)); z=(I*)((UC*)z+(((n-1)&(SZI-1))+1)); \
  ) \
 }else{  \
  DQ(m, \
   __m256d u256;__m256d v256; v256=_mm256_castsi256_pd(_mm256_set1_epi8(*(C*)y)); \
   if((n-1)>>(LGSZI+LGNPAR)){decls256 \
    DQU((n-1)>>(LGSZI+LGNPAR), u256=_mm256_loadu_pd(x); \
     _mm256_storeu_pd(z, fuv); x=(C*)x+NPAR*SZI; z=(C*)z+NPAR*SZI; \
    ) \
   } \
   v=_mm_extract_epi64(_mm256_castsi256_si128(_mm256_castpd_si256(v256)),0); y=(C*)y+1; \
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
