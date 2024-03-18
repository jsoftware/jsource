/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Macros and Defined Constants for Atomic (Scalar) Verbs           */

                                    /*   cv - control vector               */
// bits 0-1 kept open for jtflags
// bits 2-3 should be forced to 1 jtflags;
#define VCVTIP          0xc  // bits 2-3 should always be set, indicating that a converted argument can be inplaced
#define VARGX           4           // bit position for arg flags
#define VBB             (B01<<VARGX)         // convert arguments to B 4   could put VARGMSK into 3 bits
#define VII             (INT<<VARGX)         /* convert arguments to I 6             */
#define VDD             (FL<<VARGX)          /* convert arguments to D 7             */
#define VZZ             (CMPX<<VARGX)        /* convert arguments to Z 8             */
#define VIPWCRLONGX     9  // internal use in va2, overlaps BOX 9
#define VIPWCRLONG      ((I)1<<VIPWCRLONGX)
#define Vxx             (XNUM<<VARGX)        /* convert arguments to XNUM 10           */
#define VQQ             (RAT<<VARGX)         /* convert arguments to RAT  11          */
#define VARGMSK         (VBB|VII|VDD|VZZ|Vxx|VQQ|VCOPYW|VCOPYA)  // mask for argument requested type
#define VRESX           12           // bit position for result flags
#define VB              (B01<<VRESX)  // result type B  bit 12   could put VRESMSK into 3 bits
#define VI              (INT<<VRESX)/* result type I  bit 14                     */
#define VD              (FL<<VRESX) /* result type D  bit 15                     */
#define VZ              (CMPX<<VRESX)/* result type Z bit 16                      */
#define VX              (XNUM<<VRESX)/* result type XNUM  bit 18                  */
#define VQ              (RAT<<VRESX) /* result type RAT  bit 19                   */
#define VSB             (SBT<<VRESX) /* result type SBT bit 28                    */
#define VUNCH           (0<<VRESX)  // leave result unchanged
#define VRESMSK         (VB|VI|VD|VZ|VX|VQ|VSB)  // mask for result-type - if all 0, take result type from the args
#define VRD             (0x800<<VRESX)// convert result to D if possible 23
#define VRI             (0x8000<<VRESX)// convert result to I if possible  27
#define VCOPYWX         13  // set (by var) to indicate that a should be converted to type of w
#define VCOPYW          ((I)1<<VCOPYWX)
#define VCOPYAX         29  // set (by var) to indicate that w should be converted to type of a
#define VCOPYA          ((I)1<<VCOPYAX)
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
// the conversion info is in bits 22 and 24:
#define VXX             (Vxx|XMODETOCVT((I)XMEXACT))  // exact conversion
#define VXEQ            (Vxx|XMODETOCVT((I)XMEXMT))   /* convert to XNUM for = ~:            */
#define VXCF            (Vxx|XMODETOCVT((I)XMCEIL))   /* convert to XNUM ceiling/floor       */
#define VXFC            (Vxx|XMODETOCVT((I)XMFLR))  /* convert to XNUM floor/ceiling       */
// bit 31 must not be used - it may be a sign bit, which has a meaning
#define VFRCEXMT        XMODETOCVT((I)XMEXMT)   // set in arg to cvt() to do rounding for = ~:, if the conversion happens to be to XNUM
#define VIPOKRNKWX         28      // filled by va2 if the ranks allow inplacing w
#define VIPOKRNKW          ((I)1<<VIPOKRNKWX)
#define VIPOKRNKAX         30      // filled by va2 if the ranks allow inplacing a
#define VIPOKRNKA          ((I)1<<VIPOKRNKAX)

// Extract the argument-conversion type from cv coming from the table
#define atype(x) (((x)>>VARGX)&(VARGMSK>>VARGX))

// Extract the result type from cv coming from the table
#define rtype(x) (((x)>>VRESX)&(VRESMSK>>VRESX))

#define NOT(v) ((v)^VALIDBOOLEAN)

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

// Routines for QP operations
#define PLUSE(u,v) ({D h,l,t; TWOSUM1(u.hi,v.hi,h,l); l=u.lo+v.lo+l; TWOSUMBS1(h,l,t,h); CANONE1(t,h); })
#define MINUSE(u,v) ({D h,l,t; TWOSUM1(u.hi,-v.hi,h,l); l=u.lo-v.lo+l; TWOSUMBS1(h,l,t,h); CANONE1(t,h); })
#define TYMESE(u,v) ({D h,l,t,x; if(u.hi!=0. && v.hi!=0.){TWOPROD1(u.hi,v.hi,h,l); TWOPROD1(u.hi,v.lo,t,x); l+=t; TWOPROD1(u.lo,v.hi,t,x); l+=t; TWOSUMBS1(h,l,t,h);}else t=h=0.; CANONE1(t,h); })
// reciprocal of v: 1.0/h gives H, which is a truncated version of 1/h.  To find out what H is the true reciprocal of, we take hH=1+d where d is small.  Then we see
// that H is the reciprocal of h/(1+d) which we approximate as h(1-d)=h-hd (error is h*d^2 which is below the ULP).  The full reciprocal we want is that of (h+l)=(h-hd)+(l+hd), which is
// 1/((h-hd)+(l+hd)) = H - (l+hd)/((h-hd)(h+l)) which we approximate as H - (l+hd)*H*H
#define RECIPE(v) ({D zh,one,d,H=1.0/(v).hi; TWOPROD1(H,(v).hi,one,d); one-=1.0; d+=one; d*=(v).hi; d+=(v).lo; d*=H; d*=-H; TWOSUMBS1(H,d,zh,H); (E){.hi=zh,.lo=H}; })  // noncanonical result, in {zh,H}
#define DIVE(u,v) ({E vr=RECIPE(v); TYMESE(u,vr); })
#define MAXE(u,v) ({E vr; if(u.hi>v.hi)vr=u; else if(u.hi<v.hi)vr=v; else{vr.hi=u.hi; vr.lo=MAX(u.lo,v.lo);} vr;})
#define MINE(u,v) ({E vr; if(u.hi<v.hi)vr=u; else if(u.hi>v.hi)vr=v; else{vr.hi=u.hi; vr.lo=MIN(u.lo,v.lo);} vr;})

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
// verbs atomic only on the monad
#define VA1CROOT 37
#define VA1CLOG 38
// extension: verbs that are implemented by calls to the dyad for some precisions, but not all
#define VA1CNEG 39  // -, which is 0-y except for E
#define VA1CRECIP 40  // %, which is 1/y except for E


/*
 b    1 iff cell rank of a <= cell rank of w
 m    # atoms of in the cell with the smaller rank
 n    excess # of cell atoms
 z    pointer to result atoms
 x    pointer to a      atoms
 y    pointer to w      atoms
*/

// exp takes u and v, sets zz
#define AEXP(f,Tz,Tx,Ty,exp)  \
 AHDR2(f,Tz,Tx,Ty){Tx u;Ty v;Tz zz;                            \
  if(n-1==0)  DQ(m, u=*x++; v=*y++; exp  *z++=zz; )   \
  else if(n-1<0)DQ(m, u=*x++; DQC(n, v=*y++; exp  *z++=zz;))   \
  else      DQ(m, v=*y++; DQ(n, u=*x++; exp  *z++=zz;   ));  \
  R EVOK; \
 }


#define AIFX(f,Tz,Tx,Ty,symb) AEXP(f,Tz,Tx,Ty,zz=u symb v;) 

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
AHDR2(name,void,void,void){ \
 __m256d xx,yy,zz; \
 __m256i endmask; /* length mask for the last word */ \
   /* will be removed except for divide */ \
 CVTEPI64DECLS pref \
 if(n-1==0){ \
  /* vector-to-vector of length m, no repetitions */ \
  /* align dest to NPAR boundary, if needed and len makes it worthwhile */ \
  PRMALIGN(zzop,3,fz,m)  /* this changes m */ \
  PRMDUFF(zzop,3,fz,m,32+16+8) \
  PRMMASK(zzop,3,fz) /* runout, using mask */ \
 }else{ \
  if(!((fz)&1)&&n-1<0){n=~n; \
   /* m applications of atom+vector of length ~n (never used if commutative) */ \
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
   /* m applications of vector of length n+atom */ \
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

// convert from 4 16-byte atoms to 2 AVX registers in order 0 2 1 3
#define SHUFIN(fz,v0,v1,out0,out1) if(fz&2){out0=v0;out1=v1;}else{out0=_mm256_unpacklo_pd(v0,v1); out1=_mm256_unpackhi_pd(v0,v1);}
#define SHUFOUT(fz,out0,out1) if(!(fz&2)){__m256d t0=out0; out0=_mm256_shuffle_pd(out0,out1,0b0000); out1=_mm256_shuffle_pd(t0,out1,0b1111);}
#define PREFNULL(lo,hi)  // modify lo and hi as needed
// loop for types that use 2 D values: CMPX and QP.  The inner loop is lengthy, so to save
// instruction cache we do it only once, building all the other loops around the core.
// prefL and prefR are macros, used for noncommutative operations
// fz: 1 set if NONcommutative operator  2 set if shuffle suppressed
// n=1 vec+vec n<0 atom+vec  n>1 vec+atom

#define primop256CE(name,fz,CET,cepref,ceprefL,ceprefR,zzop,cesuff) \
AHDR2(name,CET,CET,CET){ \
 __m256d z0, z1, x0, x1, y0, y1, in0, in1; \
 cepref \
 /* convert vector args, which are the same size as z, to offsets from z; flag atom args */ \
 if(likely(n-1==0)){x=(CET*)((C*)x-(C*)z); y=(CET*)((C*)y-(C*)z);  /* vector op vector, both args offset */ \
 }else{  /* one arg is atom - flag addr and fetch repeated value.  m is #atom-vec loops, n is length of each and switch flag */ \
  {I taddr=(I)x^(I)y; x=n-1>0?y:x; y=(CET*)((I)x^taddr);}  /* if repeated vector op atom, exchange to be atom op vector for ease of fetch */ \
  y=(CET*)((C*)y-(C*)z);  /* convert the full-sized y arg to offset form */ \
  x=(CET*)((I)x+1); if(fz&1){x=(CET*)((I)x+(~REPSGN(n)&2));}  /* flag x: atom in bit 0, swapped in bit 1 */ \
  I t=m; m=n^REPSGN(n); n=t; /* convert vec len to positive, move to m; move outer loop count to n */ \
atomveclp: ;  /* come back here to do next atom op vector loop, with z running */ \
  /* read the repeated value and convert to internal form */ \
  if(!(fz&1)){  /* commutative value */ \
   if(!(fz&2)){x0=_mm256_broadcast_sd((D*)((I)x&-4)), x1=_mm256_broadcast_sd((D*)((I)x&-4)+1); /* read and shuffle=broadcast */ \
   }else{x0=_mm256_broadcast_pd((__m128d*)((I)x&-4)), x1=x0;}  /* broadcast pairs, no shuffle */ \
  }else{ \
   if((I)x&2){ \
    if(!(fz&2)){y0=_mm256_broadcast_sd((D*)((I)x&-4)), y1=_mm256_broadcast_sd((D*)((I)x&-4)+1); \
    }else{y0=_mm256_broadcast_pd((__m128d*)((I)x&-4)), y1=y0;}  /* broadcast pairs, no shuffle */ \
    ceprefR(y0,y1) \
   }else{ \
    if(!(fz&2)){x0=_mm256_broadcast_sd((D*)((I)x&-4)), x1=_mm256_broadcast_sd((D*)((I)x&-4)+1); \
    }else{x0=_mm256_broadcast_pd((__m128d*)((I)x&-4)), x1=x0;}  /* broadcast pairs, no shuffle */ \
    ceprefL(x0,x1) \
   } \
  }  /* do LR processing for noncommut */ \
 } \
 /* loop n times - usually once, but may be repeated for each atom.  The loop is by branch back to atomveclp */ \
 \
 /* The loop is split into 3 parts: prefix/body/suffix.  The prefix gets z onto a cacheline boundary; the */ \
 /* body processes full cachelines; the suffix finishes.  Prefix/suffix use masked stores. */ \
 /* Here we calculate length of prefix and body+suffix.  We then encode them into one value. */ \
 /* We keep a mask for the current part */ \
 I len0=-(I)z>>(LGSZI+1);  /* ...aa amt to proc to get to 2cacheline bdy */  \
 len0=m<8?m:len0;  /* if short, switch len0 to full length to reduce passes through op */ \
 len0&=NPAR-1;  /* prefix len: if long, to get to bdy; if short, to leave last block exactly NPAR or 0 */ \
 /* get mask for first read/write: same 2-bit values in lanes 01, and the other 2 bits in 23 */ \
 __m256i wrmask=_mm256_castps_si256(_mm256_permutevar_ps(_mm256_castpd_ps(_mm256_broadcast_sd((D*)&maskec4123[len0])),_mm256_loadu_si256((__m256i*)&validitymask[2]))); \
 I len1=m+((4|-len0)<<(BW-3));    /* make len1 negative so we set new masks for the body.  We can recover len0 from len1.  We do this even if len0=0 to avoid misbranches */ \
 \
 /* loop m times, for each operation */ \
rdmasklp: ;  /* here when we must read the new args under mask */ \
 \
 /* read any nonrepeated argument, shuffle */ \
 I totallen=len1&((1LL<<(BW-3))-1);  /* total remaining length */ \
 I zinc=(totallen>2)<<(LGNPAR+LGSZI);  /* offset to second half of input, if it is valid */ \
 if(likely(!((I)x&1))){  /* if x is not repeated... */ \
  in0=_mm256_maskload_pd((D*)((C*)z+(I)x),wrmask), in1=_mm256_maskload_pd((D*)((C*)z+(I)x+zinc),_mm256_slli_epi64(wrmask,1)); \
  SHUFIN(fz,in0,in1,x0,x1);  /* convert to llll hhhh form */ \
  if(fz&1){ceprefL(x0,x1)}  /* do LR processing for noncommut */ \
 } \
 /* always read the y arg */ \
 in0=_mm256_maskload_pd((D*)((C*)z+(I)y),wrmask), in1=_mm256_maskload_pd((D*)((C*)z+(I)y+zinc),_mm256_slli_epi64(wrmask,1)); \
 \
mainlp:  /* here when args have already been read.  x has been converted & prefixed; y not */ \
 if(!(fz&1)){SHUFIN(fz,in0,in1,y0,y1)}  /* convert y, which is always read, to llll hhhh form */ \
 else{if((I)x&2){SHUFIN(fz,in0,in1,x0,x1) ceprefL(x0,x1)}else{SHUFIN(fz,in0,in1,y0,y1) ceprefR(y0,y1)}} \
 zzop;  /* do the main processing */ \
 \
 SHUFOUT(fz,z0,z1);  /* put result into interleaved form for writing */ \
 /* write out the result and loop */ \
 if(len1>=2*NPAR){ \
  /* the NEXT batch can be written out in full (and so can this one).  Write the result, read new args and shuffle, and loop quickly */ \
  _mm256_storeu_pd((D*)z,z0); _mm256_storeu_pd((D*)z+NPAR,z1);   /* write out */ \
  z=(CET*)((I)z+2*NPAR*SZI); len1-=NPAR;  /* advance to next batch */ \
rdlp: ;  /* come here to fetch next batch & store it without masking */ \
  if(likely(!((I)x&1))){  /* if x is not repeated... */ \
   in0=_mm256_loadu_pd((D*)((C*)z+(I)x)), in1=_mm256_loadu_pd((D*)((C*)z+(I)x)+NPAR); \
   SHUFIN(fz,in0,in1,x0,x1);  /* convert to llll hhhh form */ \
   if(fz&1){ceprefL(x0,x1)}  /* do L processing for noncommut - value was not swapped */ \
  } \
  /* always read the y arg */ \
  in0=_mm256_loadu_pd((D*)((C*)z+(I)y)), in1=_mm256_loadu_pd((D*)((C*)z+(I)y)+NPAR); \
  goto mainlp; \
 }else if(len1>=NPAR){  /* next-to-last, or possibly last, batch */ \
  /* the next batch must be masked.  This one is OK; write the result, set the new mask, go back to read under mask */ \
  _mm256_storeu_pd((D*)z,z0); _mm256_storeu_pd((D*)z+NPAR,z1);   /* write out */ \
  z=(CET*)((I)z+2*NPAR*SZI); len1-=NPAR;  /* advance to next batch */ \
  if(len1!=0)goto rdmasklp;  /* process nonempty last batch, under mask, which has already been set */ \
  /* if len is 0, fall through to loop exit */ \
 }else{ \
  /* The current batch must write under mask.  Do so, and continue as called for, to body, suffix, or exit */ \
  /* The length of this batch comes from len0 or len1 */ \
  len0=-(len1>>(BW-3));   /* extract len0 from combined len0/len1, range 1 to 4, or 0 if not first batch */ \
  len0=len1<0?len0:len1;  /* len0=length of batch: len0 (first batch) or len1 (others) */ \
  len1&=((1LL<<(BW-3))-1); /* discard len0 from le ngth remaining */ \
  I zinc=(len0>2)<<(LGNPAR+LGSZI);  /* offset to second half of result, if it can be written */ \
  _mm256_maskstore_pd((D*)((C*)z+zinc),_mm256_slli_epi64(wrmask,1),z1); \
  _mm256_maskstore_pd((D*)(z),wrmask,z0); \
  z=(CET*)((I)z+(len0<<(LGSZI+1))); len1-=len0;  /* advance to next batch */ \
  if(len1!=0){  /* z is advanced.  Continue if there is more to do */ \
   /* set the mask for the last batch.  Unless m is 5-8, this will not hold anything up */ \
   wrmask=_mm256_castps_si256(_mm256_permutevar_ps(_mm256_castpd_ps(_mm256_broadcast_sd((D*)&maskec4123[len1&(NPAR-1)])),_mm256_loadu_si256((__m256i*)&validitymask[2]))); \
   if(likely(len1>=NPAR))goto rdlp; else goto rdmasklp;  /* process nonempty next batch, under mask if it is the last one */ \
  } \
  /* fall through to loop exit if len hit 0 */  \
 } \
 /* this is the exit from the loop, possibly reached by fallthrough */ \
 \
 /* end of one vector operation.  If there are multiple atom*vector, loop */ \
 if(unlikely(--n!=0)){++x; goto atomveclp;}  /* if multiple atom*vec, move to next atom.  z/y stay in step */ \
 cesuff \
 R EVOK; \
}


/* Embedded visual tools v3.0 fails perform the z++ on all wince platforms. -KBI */
#define ACMP(f,Tz,Tx,Ty,pfx)   \
 AHDR2(f,B,Tx,Ty){D u,v;                                             \
  if(n-1==0)  DQ(m, u=(D)*x++;       v=(D)*y++; *z=pfx(u,v); z++; )    \
  else if(n-1<0)DQ(m, u=(D)*x++; DQC(n, v=(D)*y++; *z=pfx(u,v); z++;))    \
  else      DQ(m, v=(D)*y++; DQ(n, u=(D)*x++; *z=pfx(u,v); z++;));   \
  R EVOK; \
 }

#define ACMP0T(f,Tz,Tx,Ty,Txy,pfx,pfx0)   \
 AHDR2(f,B,Tx,Ty){Txy u; Txy v;                                             \
  if(jt->cct!=1.0){ \
   if(n-1==0)  DQ(m, u=*x++;       v=*y++; *z=pfx(u,v); z++; )    \
   else if(n-1<0)DQ(m, u=*x++; DQC(n, v=*y++; *z=pfx(u,v); z++;))    \
   else      DQ(m, v=*y++; DQ(n, u=*x++; *z=pfx(u,v); z++;));   \
  }else{ \
   if(n-1==0)  DQ(m, u=*x++;       v=*y++; *z=pfx0(u,v); z++; )    \
   else if(n-1<0)DQ(m, u=*x++; DQC(n, v=*y++; *z=pfx0(u,v); z++;))    \
   else      DQ(m, v=*y++; DQ(n, u=*x++; *z=pfx0(u,v); z++;));   \
  } \
  R EVOK; \
 }
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

#if C_AVX2 || EMU_AVX2
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
