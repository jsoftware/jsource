/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Reduce (Insert), Outer Product, and Fold                       */

#include "j.h"
#include "vasm.h"
#include "ve.h"
#include "vcomp.h"
#include "ar.h"

static DF1(jtreduce);


#define PARITYW(s) (s=BW>32?s^s>>32:s, s^=s>>16, s^=s>>8)  // parity of a word full of Bs.  Upper bits are garbage

// vdo for eq/ne. take parity of *x over n bytes, store into *z; invert if !pc; repeat for m cells
static void vdone(I m,I n,B*x,B*z,B pc){
 I nfull=(n-1)>>LGSZI; I ndisc=(-n)&(SZI-1); // number of full Is, and number of bytes to discard from the next I
 DQ(m, I s=pc^1; I *y=(I*)x; DQ(nfull, s^=*y++;); s^=(*y<<(ndisc<<3)); PARITYW(s); *z++=s; x+=n;);  // parity, discarding trailing bytes of overfetch
}

#if SY_ALIGN
#define RBFXODDSIZE(pfx,bpfx)  RBFXLOOP(C,bpfx)
#define REDUCECFX              REDUCEBFX
#else
  // m is # cells to operate on; n is # items in 1 such cell; d is # atoms in one such item
#define RCFXLOOP(T,pfx)  \
 {T* RESTRICT xx=(T*)x,* RESTRICT yy,*z0,* RESTRICT zz=(T*)z;   \
  q=d/sizeof(T);                  \
  for(j=0;j<m;++j){               \
   yy=xx; xx+=q; z0=zz; DQ(q, *zz++=pfx(*yy,*xx); ++xx; ++yy;);    \
   DQ(n-2,       zz=z0; DQ(q, *zz++=pfx(*zz,*xx); ++xx;      ););  \
 }}  /* commutative */

#define RBFXODDSIZE(pfx,bpfx)  \
 {B*zz;I r,t,*xi,*yi,*zi;                                                       \
  q=d>>LGSZI; r=d&(SZI-1); xi=(I*)x; zz=z;                                             \
  for(j=0;j<m;++j,zz-=d){                                                       \
   yi=xi; xi=(I*)((B*)xi-d); zi=(I*)zz;                                         \
   DQ(q, --xi; --yi; *--zi=pfx(*xi,*yi););                                      \
    xi=(I*)((B*)xi-r); yi=(I*)((B*)yi-r); t=pfx(*xi,*yi); MC((B*)zi-r,&t,r);    \
   DQ(n-2, zi=(I*)zz; DQ(q, --xi; --zi; *zi=pfx(*xi,*zi););                     \
    xi=(I*)((B*)xi-r); zi=(I*)((B*)zi-r); t=pfx(*xi,*zi); MC(    zi,  &t,r););  \
 }}  /* non-commutative */

#define RCFXODDSIZE(pfx,bpfx)  \
 {I r,t,*xi,*yi,*zi;                                                            \
  q=d>>LGSZI; r=d&(SZI-1);                                                             \
  for(j=0;j<m;++j,x+=d,z+=d){                                                   \
   yi=(I*)x; x+=d; xi=(I*)x; zi=(I*)z; DQ(q, *zi++=pfx(*yi,*xi); ++xi; ++yi;); t=pfx(*yi,*xi); MC(zi,&t,r);    \
   DQ(n-2,   x+=d; xi=(I*)x; zi=(I*)z; DQ(q, *zi++=pfx(*zi,*xi); ++xi;      ); t=pfx(*zi,*xi); MC(zi,&t,r););  \
 }}  /* commutative */

#define REDUCECFX(f,pfx,ipfx,spfx,bpfx,vdo)  \
 AHDRP(f,B,B){B*y=0;I j,q;                       \
  if(d==1){vdo; R;}                                \
  if(1==n)DQ(d, *z++=*x++;)                        \
  else if(0==d%sizeof(UI  ))RCFXLOOP(UI,   pfx)    \
  else if(0==d%sizeof(UINT))RCFXLOOP(UINT,ipfx)    \
  else if(0==d%sizeof(US  ))RCFXLOOP(US,  spfx)    \
  else                      RCFXODDSIZE(pfx,bpfx)  \
 }  /* commutative */

#endif

// no errors possible here
  // this version, which processes by column and doesn't write out intermediates, will be better unless the arg is huge and blows out of cache.  Solution to that would be to block the computation.
  // m is # cells to operate on; n is # items in 1 such cell; d is # atoms in one such item
#define REDUCEBFX(f,pfx,ipfx,spfx,bpfx,vdo)  \
AHDRP(f,B,B){  \
 if(d==1){vdo; R EVOK;}     /* if arg is byte list, do it */   \
 I dipercell=(d+SZI-1)>>LGSZI;  \
 DQ(m, B *xx=x+(n-1)*d; B *zz=z;  /* end-of-column scanner, start at end of first column; row result pointer */ \
  DQ(dipercell, B *xxc=xx; I r=*(I*)xxc;  /* init to end of column */ \
   DQ(n-1, xxc-=d; r=pfx(*(I*)xxc,r);)  /* roll up the column */ \
   if(i){*(I*)zz=r; xx+=SZI; zz+=SZI;}else STOREBYTES(zz,r,-d&(SZI-1));  /* store valid bytes, advance to next column if there is one */ \
  ) x+=n*d; z+=d;  /* advance to next cell */ \
 ) \
 R EVOK; \
}  /* non-commutative */

REDUCECFX(  eqinsB, EQ,  IEQ,  SEQ,  BEQ,  vdone(m,n,x,z,(B)(n&1)))
REDUCECFX(  neinsB, NE,  INE,  SNE,  BNE,  vdone(m,n,x,z,1       ))
REDUCECFX(  orinsB, OR,  IOR,  SOR,  BOR,  {DQ(m, *z++=1&&memchr(x,C1,n);                         x+=n;)}) 
REDUCECFX( andinsB, AND, IAND, SAND, BAND, {DQ(m, *z++=!  memchr(x,C0,n);                         x+=n;)})
REDUCEBFX(  ltinsB, LT,  ILT,  SLT,  BLT,  {DQ(m, *z++= x[n-1]&&!memchr(x,C1,n-1)?1:0;          x+=n;)})
REDUCEBFX(  leinsB, LE,  ILE,  SLE,  BLE,  {DQ(m, *z++=!x[n-1]&&!memchr(x,C0,n-1)?0:1;          x+=n;)})
REDUCEBFX(  gtinsB, GT,  IGT,  SGT,  BGT,  {DQ(m, B *y=memchr(x,C0,n); *z++=1&&(y?1&(y-x):1&n);      x+=n;)})
REDUCEBFX(  geinsB, GE,  IGE,  SGE,  BGE,  {DQ(m, B *y=memchr(x,C1,n); *z++=!  (y?1&(y-x):1&n);      x+=n;)})
REDUCEBFX( norinsB, NOR, INOR, SNOR, BNOR, {DQ(m, B *y=memchr(x,C1,n); d=y?y-x:n; *z++=(1&d)==d<n-1; x+=n;)})
REDUCEBFX(nandinsB, NAND,INAND,SNAND,BNAND,{DQ(m, B *y=memchr(x,C0,n); d=y?y-x:n; *z++=(1&d)!=d<n-1; x+=n;)})


#if SY_ALIGN
AHDRR(plusinsB,I,B){
 if(d==1){
  for(;m;--m,x+=n){
   *z++=bsum(n,x);
  }
 }else{
  for(;m;--m,x+=n*d,z+=d){
   // Adding items with more than 1 boolean.  Process a columnar swath with 1 accumulator
   UI *xu=(UI*)x; I dd; // start position in column swath.  nn is # columns left
   I *zz=z;  // output pointer, switched if not valid
   // Now do the SZI-byte swaths, possibly with overfetch
   for(dd=d;dd>0;dd-=SZI,xu+=1){  // for all columns...
    I acc20=0; I acc21=0; I acc22=0; I acc23=0; I acc24=0; I acc25=0; I acc26=0; I acc27=0; 
    UI *xu2=xu; I nn=n;   // accumulator, address moving down the swath, number of rows left in swath
    while(nn){  // till swath finished
     I nloops=nn; nloops=nloops>255?255:nloops;  // max 255 loops, each 4 words
     nn-=nloops;  // keep nn = # bytes remaining
     I acc0=0;
     DQ(nloops, acc0+=xu2[0]; xu2=(I*)((I)xu2+d););  // add up each byte-lane in the swatch
     acc20+=acc0&255; acc0>>=8; acc21+=acc0&255; acc0>>=8; acc22+=acc0&255; acc0>>=8; acc23+=acc0&255;
#if SY_64
     acc0>>=8; acc24+=acc0&255; acc0>>=8; acc25+=acc0&255; acc0>>=8; acc26+=acc0&255; acc0>>=8; acc27+=acc0&255;
#endif
    }
    // column is added.  Write out the valid totals, divert the rest to the sink
    *zz++=acc20; zz=dd<=1?&jt->shapesink[0]:zz; *zz++=acc21; zz=dd<=2?&jt->shapesink[0]:zz; *zz++=acc22; zz=dd<=3?&jt->shapesink[0]:zz; *zz++=acc23; 
#if SY_64
    zz=dd<=4?&jt->shapesink[0]:zz; *zz++=acc24; zz=dd<=5?&jt->shapesink[0]:zz; *zz++=acc25; zz=dd<=6?&jt->shapesink[0]:zz; *zz++=acc26; zz=dd<=7?&jt->shapesink[0]:zz; *zz++=acc27; 
#endif
   }
  }
 }
 R EVOK;
}
#else

AHDRR(plusinsB,I,B){I dw,i,p,q,r,r1,s;UC*tu;UI*v;
 if(d==1&n<SZI)DQ(m, s=0; DQ(n, s+=*x++;); *z++=s;)
 else if(d==1){UI t;
  p=n>>LGSZI; q=p/255; r=p%255; r1=n&(SZI-1); tu=(UC*)&t;
  for(i=0;i<m;++i){
   s=0; v=(UI*)x; 
   DO(q, t=0; DO(255, t+=*v++;); DO(SZI, s+=tu[i];));
         t=0; DO(r,   t+=*v++;); DO(SZI, s+=tu[i];);
   x=(B*)v; DQ(r1, s+=*x++;); 
   *z++=s;
 }}else{A t;UI*tv;
  dw=(d+SZI-1)>>LGSZI; p=dw*SZI; mvc(m*d*SZI,z,MEMSET00LEN,MEMSET00);
  q=n/255; r=n%255;
  GA10(t,INT,dw); if(!t)R;
  tu=UAV(t); tv=(UI*)tu; v=(UI*)x;
  for(i=0;i<m;++i,z+=d){
   DO(q, mvc(p,tv,MEMSET00LEN,MEMSET00); DO(255, DO(dw,tv[i]+=v[i];); x+=d; v=(UI*)x;); DO(d,z[i]+=tu[i];));
         mvc(p,tv,MEMSET00LEN,MEMSET00); DO(r,   DO(dw,tv[i]+=v[i];); x+=d; v=(UI*)x;); DO(d,z[i]+=tu[i];) ;
}}}  /* +/"r w on boolean w, originally by Roger Moore */
#endif

#define PLUSI1(x) if(unlikely(__builtin_add_overflow((x),t,&t)))R EWOV;
#define MINUSI1(x) if(unlikely(__builtin_sub_overflow((x),t,&t)))R EWOV;
#define TYMESI1(x) if(unlikely(__builtin_mul_overflow((x),t,&t)))R EWOV;
// m is #cells, n is #items per cell, d is #atoms in item
#define REDUCEOVF(f,neut,ft1)  \
 AHDRR(f,I,I){I i;                          \
  if(d==1){DQ(m,  \
   UI dlct=(n+3)>>2;  \
   UI dlct0=dlct; x+=(dlct-1)*(1LL<<2); I t=neut; switch(n&3){do{case 0: ft1(x[3]) case 3: ft1(x[2]) case 2: ft1(x[1]) case 1: ft1(x[0]) x-=4; }while(--dlct0);}   \
   *z++=t; x+=n+4;  \
   ) R EVOK;  \
  }        \
  UI dlct=(n+3)>>2; I xstride1=d*SZI; I xstride3=3*xstride1;  \
  DQ(m,  \
   DQ(d, x+=d*(dlct-1)*(1LL<<2);  \
    UI dlct0=dlct; I t=neut; switch(n&3){do{case 0: ft1(*(I*)((C*)x+xstride3)) case 3: ft1(*(I*)((C*)x+2*xstride1)) case 2: ft1(*(I*)((C*)x+xstride1)) case 1: ft1(x[0]) x=(I*)((C*)x-4*xstride1); }while(--dlct0);}   \
    *z++=t; x=(I*)((C*)x+4*xstride1)+1;  \
   )  \
   x+=(n-1)*d;  \
  )  \
 R EVOK;  \
 }

REDUCEOVF( plusinsI, 0, PLUSI1) 
REDUCEOVF(minusinsI, 0, MINUSI1) 
REDUCEOVF(tymesinsI, 1, TYMESI1)


REDUCCPFX( plusinsO, D, I,  PLUSO)
REDUCCPFX(minusinsO, D, I, MINUSO) 
REDUCCPFX(tymesinsO, D, I, TYMESO) 

#define redprim256rk1(prim,identity) \
 __m256i endmask; /* length mask for the last word */ \
 /* prim/ vectors */ \
 __m256d idreg=_mm256_broadcast_sd(&identity); \
 endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-n)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
 DQ(m, __m256d acc0=idreg; __m256d acc1=idreg; __m256d acc2=idreg; __m256d acc3=idreg; __m256d acc4=idreg; __m256d acc5=idreg; __m256d acc6=idreg; __m256d acc7=idreg; \
  UI n2=DUFFLPCT(n-1,3);  /* # turns through duff loop */ \
  if(n2>0){ \
   UI backoff=DUFFBACKOFF(n-1,3); \
   x+=(backoff+1)*NPAR; \
   switch(backoff){ \
   do{ \
   case -1: acc0=prim(acc0,_mm256_loadu_pd(x)); \
   case -2: acc1=prim(acc1,_mm256_loadu_pd(x+1*NPAR)); \
   case -3: acc2=prim(acc2,_mm256_loadu_pd(x+2*NPAR)); \
   case -4: acc3=prim(acc3,_mm256_loadu_pd(x+3*NPAR)); \
   case -5: acc4=prim(acc4,_mm256_loadu_pd(x+4*NPAR)); \
   case -6: acc5=prim(acc5,_mm256_loadu_pd(x+5*NPAR)); \
   case -7: acc6=prim(acc6,_mm256_loadu_pd(x+6*NPAR)); \
   case -8: acc7=prim(acc7,_mm256_loadu_pd(x+7*NPAR)); \
   x+=(1LL<<3)*NPAR; \
   }while(--n2!=0); \
   } \
  } \
  acc0=prim(acc0,_mm256_blendv_pd(idreg,_mm256_maskload_pd(x,endmask),_mm256_castsi256_pd(endmask))); x+=((n-1)&(NPAR-1))+1; \
  acc1=prim(acc1,acc5); acc2=prim(acc2,acc6); acc3=prim(acc3,acc7); acc0=prim(acc0,acc4); \
  acc2=prim(acc2,acc3); acc0=prim(acc0,acc1); acc0=prim(acc0,acc2); /* combine accumulators vertically */ \
  acc0=prim(acc0,_mm256_permute4x64_pd(acc0,0b11111110)); acc0=prim(acc0,_mm256_permute_pd(acc0,0xf));   /* combine accumulators horizontally  01+=23, 0+=1 */ \
  *(I*)z=_mm256_extract_epi64(_mm256_castpd_si256(acc0),0x0); /* AVX2 *z=_mm256_cvtsd_f64(acc0); */ ++z;  /* store the single result */ \
 )

// f/ on rank>1, going down columns to save bandwidth
#define redprim256rk2(prim,identity) \
 __m256i endmask; /* length mask for the last word */ \
 __m256d idreg=_mm256_broadcast_sd(&identity); \
 endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-d)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
 I xstride1=d*SZD; I xstride3=3*xstride1; I xstride5=5*xstride1; I xstride7=xstride5+2*xstride1; \
 DQ(m, D *x0; \
  DQ((d-1)>>LGNPAR, \
   x0=(D*)((C*)x-((-n)&((1LL<<3)-1))*xstride1); UI n0=(n+(1LL<<3)-1)>>3; __m256d acc0=idreg; __m256d acc1=idreg; __m256d acc2=idreg; __m256d acc3=idreg; __m256d acc4=idreg; __m256d acc5=idreg; __m256d acc6=idreg; __m256d acc7=idreg;  \
   switch(n&((1LL<<3)-1)){ \
   do{ \
    case 0: acc0=prim(acc0,_mm256_loadu_pd(x0)); \
    case 7: acc1=prim(acc1,_mm256_loadu_pd((D*)((C*)x0+xstride1))); \
    case 6: acc2=prim(acc2,_mm256_loadu_pd((D*)((C*)x0+2*xstride1))); \
    case 5: acc3=prim(acc3,_mm256_loadu_pd((D*)((C*)x0+xstride3))); \
    case 4: acc4=prim(acc4,_mm256_loadu_pd((D*)((C*)x0+4*xstride1))); \
    case 3: acc5=prim(acc5,_mm256_loadu_pd((D*)((C*)x0+xstride5))); \
    case 2: acc6=prim(acc6,_mm256_loadu_pd((D*)((C*)x0+2*xstride3))); \
    case 1: acc7=prim(acc7,_mm256_loadu_pd((D*)((C*)x0+xstride7))); \
     x0=(D*)((C*)x0+(1LL<<3)*xstride1); \
   }while(--n0); \
   } \
   acc0=prim(acc0,acc4);  acc1=prim(acc1,acc5); acc2=prim(acc2,acc6); acc3=prim(acc3,acc7); \
   acc0=prim(acc0,acc1);  acc2=prim(acc2,acc3); acc0=prim(acc0,acc2); _mm256_storeu_pd(z,acc0); \
   x+=NPAR; z+=NPAR; \
  ) \
  x0=(D*)((C*)x-((-n)&((1LL<<3)-1))*xstride1); UI n0=(n+(1LL<<3)-1)>>3; __m256d acc0=idreg; __m256d acc1=idreg; __m256d acc2=idreg; __m256d acc3=idreg; __m256d acc4=idreg; __m256d acc5=idreg; __m256d acc6=idreg; __m256d acc7=idreg;  \
  switch(n&((1LL<<3)-1)){ \
  do{ \
   case 0: acc0=prim(acc0,_mm256_maskload_pd(x0,endmask)); \
   case 7: acc1=prim(acc1,_mm256_maskload_pd((D*)((C*)x0+xstride1),endmask)); \
   case 6: acc2=prim(acc2,_mm256_maskload_pd((D*)((C*)x0+2*xstride1),endmask)); \
   case 5: acc3=prim(acc3,_mm256_maskload_pd((D*)((C*)x0+xstride3),endmask)); \
   case 4: acc4=prim(acc4,_mm256_maskload_pd((D*)((C*)x0+4*xstride1),endmask)); \
   case 3: acc5=prim(acc5,_mm256_maskload_pd((D*)((C*)x0+xstride5),endmask)); \
   case 2: acc6=prim(acc6,_mm256_maskload_pd((D*)((C*)x0+2*xstride3),endmask)); \
   case 1: acc7=prim(acc7,_mm256_maskload_pd((D*)((C*)x0+xstride7),endmask)); \
    x0=(D*)((C*)x0+(1LL<<3)*xstride1); \
  }while(--n0); \
  } \
  acc0=prim(acc0,acc4);  acc1=prim(acc1,acc5); acc2=prim(acc2,acc6); acc3=prim(acc3,acc7); \
  acc0=prim(acc0,acc1);  acc2=prim(acc2,acc3); acc0=prim(acc0,acc2); _mm256_maskstore_pd(z,endmask,acc0); \
  x=x0-((d-1)&-NPAR); z+=((d-1)&(NPAR-1))+1; \
 )

// prim for +/ II  primplusII(x,y)  _mm256_castsi256_pd(_mm256_add_epi64(_mm256_castpd_si256(x),_mm256_castpd_si256(y))); oflo=_mm256_or_pd(oflo,_mm256_andnot_pd(_mm256_xor_pd(x,y),_mm256_xor_pd(x,zz)));,

AHDRR(plusinsD,D,D){I i;D* RESTRICT y;
  NAN0;
  // latency of add is 2, so use 4 accumulators for 2 reads per cycle
  if(d==1){
#if C_AVX2 || EMU_AVX2
   redprim256rk1(_mm256_add_pd,dzero)
#else
  DQ(m, I n0=n; D acc0=0.0; D acc1=0.0; D acc2=0.0; D acc3=0.0;
   switch(n0&3){
   loopback2:
   case 0: acc0+=*x++; case 3: acc1+=*x++; case 2: acc2+=*x++; case 1: acc3+=*x++; 
   if((n0-=4)>0)goto loopback2;
   }
   acc0+=acc1; acc2+=acc3; *z++=acc0+acc2;
  )
#endif
  }
  else{
#if C_AVX2 || EMU_AVX2
   redprim256rk2(_mm256_add_pd,dzero)
#elif 1
   // add down the columns to reduce memory b/w.  4 accumulators
   DQ(m, D *x0;
    DQ(d, x0=x;
     I n0=n; D acc0=0.0; D acc1=0.0; D acc2=0.0; D acc3=0.0;
     switch(n0&3){
     loopback:
     case 0: acc0+=*x0; x0+=d;
     case 3: acc1+=*x0; x0+=d;
     case 2: acc2+=*x0; x0+=d;
     case 1: acc3+=*x0; x0+=d;
     if((n0-=4)>0)goto loopback;
     }
     acc0+=acc1; acc2+=acc3; acc0+=acc2; *z++=acc0;
     ++x;
    )
    x=x0-(d-1); 
   )
#endif
  }
  R NANTEST?EVNAN:EVOK;
}

REDUCENAN( plusinsZ, Z, Z, zplus, plusZZ )
REDUCEPFX( plusinsX, X, X, xplus, plusXX, plusXX )

static OP1XYZ(plus,I,I2,I2,pfxplus)
OP1XYZ(plus,I,I2,I,pfxplus)
REDUCEPFX(plusinsI2, I, I2, pfxplus, plus1I2I2I, plus1I2II )
static OP1XYZ(plus,I,I4,I4,pfxplus)
OP1XYZ(plus,I,I4,I,pfxplus)
REDUCEPFX( plusinsI4, I, I4, pfxplus, plus1I4I4I, plus1I4II )


#if C_AVX2 || EMU_AVX2
// version for QP with AVX2.  Bandwidth is not an issue, so we accumulate into memory for rank > 1
I plusinsE(I d,I n,I m,E* RESTRICTI x,E* RESTRICTI z,J jt){I i;  // m is # cells to operate on; n is # items in 1 such cell; d is # atoms in one such item
  if(d==1){x += m*n; z+=m;
   __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin); __m256d mantmask=_mm256_broadcast_sd((D*)&(I){0x000fffffffffffff});  /* needed masks: sign, mantissa */
   /* we will read twice, masking.  We may read the first half twice to avoid overfetch */
   __m256i rdmask=_mm256_castps_si256(_mm256_permutevar_ps(_mm256_castpd_ps(_mm256_broadcast_sd((D*)&maskec4123[n&(NPAR-1)])),_mm256_loadu_si256((__m256i*)&validitymask[2]))); /* masks for the 2 reads */
   for(i=m;i;--i){   /* for each accumulated row */
    I rematom;  /* # atoms to accumulate */
    __m256d x0,x1,y0,y1,z0,z1;
    x -= ((n-1)&(NPAR-1))+1;  /* back up to beginning of last NPAR-atom section */
    x0=_mm256_maskload_pd((D*)(x),rdmask);
    x1=_mm256_maskload_pd((D*)(x+((n-1)&(NPAR/2))),_mm256_slli_epi64(rdmask,1));
    SHUFIN(0,x0,x1,z0,z1);
    for(rematom=(n-1)&-NPAR;rematom;rematom-=NPAR){  /* for each full batch */
     x-=NPAR; x0=_mm256_loadu_pd((D*)x); x1=_mm256_loadu_pd((D*)(x+NPAR/2)); SHUFIN(0,x0,x1,y0,y1);  /* read 4 values, put into lanes */ 
     PLUSEE(y0,y1,z0,z1,z0,z1);  /* add to total */
    }
    y0=_mm256_permute_pd(z0,0b1111); y1=_mm256_permute_pd(z1,0b1111);  /* atoms 0+1, 2+3 */
    PLUSEE(y0,y1,z0,z1,z0,z1);
    y0=_mm256_permute4x64_pd(z0,0b10101010); y1=_mm256_permute4x64_pd(z1,0b10101010);  /* atoms 0+2 */
    PLUSEE(y0,y1,z0,z1,z0,z1);
    CANONE(z0,z1)
    --z; *(I*)&z->hi=_mm256_extract_epi64(_mm256_castpd_si256(z0),0x0); *(I*)&z->lo=_mm256_extract_epi64(_mm256_castpd_si256(z1),0x0);
   }
  }else{z+=(m-1)*d; x+=(m*n-1)*d;                                       
   for(i=0;i<m;++i,z-=d){I rc;                                   
    E* RESTRICT y=x; x-=d; if(255&(rc=plusEE AH2A_v(d,x,y,z,jt), rc=rc<0?EWOVIP+EWOVIPMULII:rc))R rc; x-=d;       
    DQ(n-2,    if(255&(rc=plusEE AH2A_v(d,x,z,z,jt), rc=rc<0?EWOVIP+EWOVIPMULII:rc))R rc; x-=d;);       
  }
 }
 R EVOK;
}
#endif


REDUCEPFX(minusinsB, I, B, MINUS, minusBB, minusBI ) 
REDUCENAN(minusinsD, D, D, MINUS, minusDD ) 
REDUCENAN(minusinsZ, Z, Z, zminus, minusZZ) 

REDUCEPFX(tymesinsD, D, D, TYMES, tymesDD, tymesDD ) 
REDUCEPFX(tymesinsZ, Z, Z, ztymes, tymesZZ, tymesZZ) 

REDUCENAN(  divinsD, D, D, DIV, divDD   )
REDUCENAN(  divinsZ, Z, Z, zdiv, divZZ  )

REDUCEPFXIDEM2(  maxinsI, I, I, MAX, maxII   )
REDUCEPFXIDEM2PRIM256(  maxinsD, D, D, MAX, maxDD, _mm256_max_pd, infm  )
REDUCEPFX(  maxinsX, X, X, XMAX, maxXX , maxXX )
REDUCEPFX(  maxinsS, SB,SB,SBMAX, maxSS, maxSS )

REDUCEPFXIDEM2(  mininsI, I, I, MIN, minII   )
REDUCEPFXIDEM2PRIM256(  mininsD, D, D, MIN, minDD, _mm256_min_pd, inf   )
REDUCEPFX(  mininsX, X, X, XMIN, minXX, minXX  )
REDUCEPFX(  mininsS, SB,SB,SBMIN, minSS, minSS )

// +/!.0"r, compensated summation
static DF1(jtreduce);  // forward declaration
DF1(jtcompsum){
 ARGCHK1(w)
 I wr=AR(w); I *ws=AS(w);
 // Create  r: the effective rank; f: length of frame; n: # items in a CELL of w
 I r=(RANKT)jt->ranks; r=wr<r?wr:r; I f=wr-r; I n; SETICFR(w,f,r,n);  // no RESETRANK
 // if the argument is not float, or if there are not more than 2 items, process as normal +/
 if(unlikely((-(AT(w)&FL)&(2-n))>=0))R reduce(w,FAV(self)->fgh[0]);
 // calculate cell sizes and allocate the result
 I d; PROD(d,r-1,f+ws+1);  //  */ }. $ cell
 // m=*/ frame (i. e. #cells to operate on)
 // r cannot be 0 (would be handled above).  Calculate low part of zn first
 I m; PROD(m,f,ws);
 // Allocate the result area
 A z; GATV(z,FL,m*d,MAX(0,wr-1),ws); if(1<r)MCISH(f+AS(z),f+1+ws,r-1);  // allocate, and install shape below the frame
 if(unlikely(m*d==0)){RETF(z);}  // mustn't call the function on an empty argument!
 // Do the operation
 NAN0;
 D *wv=DAV(w), *zv=DAV(z);
#if C_AVX2 || EMU_AVX2
 __m256d __attribute__((aligned(64))) accc[2][8];   // accumulators and error terms
 __m256i endmask; /* length mask for the last word */
 if(d==1){
  // rank-1 case: operate across the row, with 4 accumulators
  endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-n)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */
  for(;m>0;--m){
   __m256d acc0; __m256d acc1; __m256d acc2; __m256d acc3;
   __m256d c0; __m256d c1; __m256d c2; __m256d c3; // error terms
// KAHAN has a dependency loop of 4 instructions, thus latency of 16 cycles (Skylake).  It takes unrolling by 8 to get the latency to match
// the launch rate of 2 cycles per iteration.  This requires spilling results to memory, which will add more latency, but it's still better than unrolling only 4.
// Unfortunately, clang goes nuts trying to handle this loop, and ends up doing 14 loads and 14 stores for every iteration.  So we have to unroll it
// by hand using an array to hold the state
   mvc(sizeof(accc),accc,MEMSET00LEN,MEMSET00);
   UI nlp=(n-1)>>LGNPAR; 
   for(;nlp;--nlp){KAHANA(_mm256_loadu_pd(wv),nlp&7) wv+=NPAR;}
   KAHANA(_mm256_maskload_pd(wv,endmask),7) wv+=((n-1)&(NPAR-1))+1;
   __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin);
   // add all the low parts together into c0 - the low bits of the low will not make it through to the result
#if 0  // clang
   c1=_mm256_add_pd(c1,c5); c2=_mm256_add_pd(c2,c6); c3=_mm256_add_pd(c3,c7); c0=_mm256_add_pd(c0,c4);
#else
   c0=_mm256_add_pd(accc[1][6],accc[1][5]); c1=_mm256_add_pd(accc[1][4],accc[1][3]); c2=_mm256_add_pd(accc[1][2],accc[1][1]); c3=_mm256_add_pd(accc[1][0],accc[1][7]);
#endif 
   c0=_mm256_add_pd(c0,c1); c2=_mm256_add_pd(c2,c3); c0=_mm256_add_pd(c0,c2);
   // TWOSUM the accumulators into acc0, adding all the resulting low parts into c0
#if 0  // clang
   TWOSUM(acc0,acc4,acc0,c1) c0=_mm256_add_pd(c0,c1); TWOSUM(acc1,acc5,acc1,c1) c0=_mm256_add_pd(c0,c1); 
   TWOSUM(acc2,acc6,acc2,c1) c0=_mm256_add_pd(c0,c1); TWOSUM(acc3,acc7,acc3,c1) c0=_mm256_add_pd(c0,c1);
#else
   TWOSUM(accc[0][6],accc[0][5],acc0,c1) c0=_mm256_add_pd(c0,c1); TWOSUM(accc[0][4],accc[0][3],acc1,c1) c0=_mm256_add_pd(c0,c1); 
   TWOSUM(accc[0][2],accc[0][1],acc2,c1) c0=_mm256_add_pd(c0,c1); TWOSUM(accc[0][0],accc[0][7],acc3,c1) c0=_mm256_add_pd(c0,c1);
#endif 
   TWOSUM(acc0,acc1,acc0,c1) c0=_mm256_add_pd(c0,c1); TWOSUM(acc2,acc3,acc2,c1) c0=_mm256_add_pd(c0,c1);    // add 0+1, 2+3; combine all low parts of Kahan corrections into low total
   TWOSUM(acc0,acc2,acc0,c1) c0=_mm256_add_pd(c0,c1);  // 0+2, bringing along low part
  // acc0/c0 survive.  Combine horizontally
   c0=_mm256_add_pd(c0,_mm256_permute4x64_pd(c0,0b11111110)); acc1=_mm256_permute4x64_pd(acc0,0b11111110);  // c0: 01+=23, acc1<-23
   TWOSUM(acc0,acc1,acc0,c1); c0=_mm256_add_pd(c0,c1); // combine p=01+23
   c0=_mm256_add_pd(c0,_mm256_permute_pd(c0,0xf)); acc1=_mm256_permute_pd(acc0,0xf);   // combine c0+c1, acc1<-1
   TWOSUM(acc0,acc1,acc0,c1); c0=_mm256_add_pd(c0,c1);    // combine 0123, combine all low parts
   acc0=_mm256_add_pd(acc0,c0);  // add low parts back into high in case there is overlap
   *(I*)zv=_mm256_extract_epi64(_mm256_castpd_si256(acc0),0x0); /* AVX2 *zv=_mm256_cvtsd_f64(acc0);*/ ++zv;  // store the single result
  }
 }else{
  // rank>1, going down columns to save bandwidth and add accuracy
  endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-d)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */
  DQ(m, D *wv0; I n0;
   __m256d y; __m256d t;   // new input value, temp to hold high part of sum
   __m256d acc0; __m256d acc1; __m256d acc2; __m256d acc3; __m256d c0; __m256d c1; __m256d c2; __m256d c3;  // accumulators, error terms
   DQ((d-1)>>LGNPAR,
    wv0=wv;
#if 0  // clang
    n0=n; acc0=acc1=acc2=acc3=c0=c1=c2=c3=_mm256_setzero_pd();
    switch(n0&3){
    label1:
    case 0: KAHAN(_mm256_loadu_pd(wv0),0) wv0+=d; case 3: KAHAN(_mm256_loadu_pd(wv0),1) wv0+=d; case 2: KAHAN(_mm256_loadu_pd(wv0),2) wv0+=d; case 1: KAHAN(_mm256_loadu_pd(wv0),3) wv0+=d;
     if((n0-=4)>0)goto label1;
    }
#else
    mvc(sizeof(accc),accc,MEMSET00LEN,MEMSET00);
    UI nlp=n; 
    for(;nlp;--nlp){KAHANA(_mm256_loadu_pd(wv0),nlp&7) wv0+=d;}
#endif
    __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin);
    // combine accumulators
    c0=_mm256_add_pd(accc[1][6],accc[1][5]); c1=_mm256_add_pd(accc[1][4],accc[1][3]); c2=_mm256_add_pd(accc[1][2],accc[1][1]); c3=_mm256_add_pd(accc[1][0],accc[1][7]);
    c0=_mm256_add_pd(c0,c1); c2=_mm256_add_pd(c2,c3); c0=_mm256_add_pd(c0,c2);   // add all the low parts together - the low bits of the low will not make it through to the result
    TWOSUM(accc[0][7],accc[0][6],acc0,c1) c0=_mm256_add_pd(c0,c1); TWOSUM(accc[0][5],accc[0][4],acc1,c1) c0=_mm256_add_pd(c0,c1); 
    TWOSUM(accc[0][3],accc[0][2],acc2,c1) c0=_mm256_add_pd(c0,c1); TWOSUM(accc[0][1],accc[0][0],acc3,c1) c0=_mm256_add_pd(c0,c1);
    TWOSUM(acc0,acc1,acc0,c1) TWOSUM(acc2,acc3,acc2,c2) c2=_mm256_add_pd(c1,c2); c0=_mm256_sub_pd(c2,c0);   // add 0+1, 2+3  KAHAN corrections are negative - change that now
    TWOSUM(acc0,acc2,acc0,c1) c0=_mm256_add_pd(c1,c0);  // 0+2
    acc0=_mm256_add_pd(acc0,c0);  // add low parts back into high in case there is overlap
    _mm256_storeu_pd(zv,acc0); wv+=NPAR; zv+=NPAR;
   )
   // repeat for partial column
   wv0=wv;
#if 0  // clang
   n0=n; acc0=acc1=acc2=acc3=c0=c1=c2=c3=_mm256_setzero_pd();
   switch(n0&3){
   label2:
    case 0: KAHAN(_mm256_maskload_pd(wv0,endmask),0) wv0+=d; case 3: KAHAN(_mm256_maskload_pd(wv0,endmask),1) wv0+=d; case 2: KAHAN(_mm256_maskload_pd(wv0,endmask),2) wv0+=d; case 1: KAHAN(_mm256_maskload_pd(wv0,endmask),3) wv0+=d;
    if((n0-=4)>0)goto label2;
   }
#else
    mvc(sizeof(accc),accc,MEMSET00LEN,MEMSET00);
    UI nlp=n; 
    for(;nlp;--nlp){KAHANA(_mm256_maskload_pd(wv0,endmask),nlp&7) wv0+=d;}
#endif
   __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin);
   c0=_mm256_add_pd(accc[1][6],accc[1][5]); c1=_mm256_add_pd(accc[1][4],accc[1][3]); c2=_mm256_add_pd(accc[1][2],accc[1][1]); c3=_mm256_add_pd(accc[1][0],accc[1][7]);
   c0=_mm256_add_pd(c0,c1); c2=_mm256_add_pd(c2,c3); c0=_mm256_add_pd(c0,c2);   // add all the low parts together - the low bits of the low will not make it through to the result
   TWOSUM(accc[0][7],accc[0][6],acc0,c1) c0=_mm256_add_pd(c0,c1); TWOSUM(accc[0][5],accc[0][4],acc1,c1) c0=_mm256_add_pd(c0,c1); 
   TWOSUM(accc[0][3],accc[0][2],acc2,c1) c0=_mm256_add_pd(c0,c1); TWOSUM(accc[0][1],accc[0][0],acc3,c1) c0=_mm256_add_pd(c0,c1);
   TWOSUM(acc0,acc1,acc0,c1) TWOSUM(acc2,acc3,acc2,c2) c2=_mm256_add_pd(c1,c2); c0=_mm256_sub_pd(c2,c0);   // add 0+1, 2+3  KAHAN corrections are negative - change that now
   TWOSUM(acc0,acc2,acc0,c1) c0=_mm256_add_pd(c1,c0);  // 0+2
   acc0=_mm256_add_pd(acc0,c0);  // add low parts back into high in case there is overlap
   _mm256_maskstore_pd(zv,endmask,acc0); wv=wv0-((d-1)&-NPAR); zv+=((d-1)&(NPAR-1))+1;
  )
 }
#else
 D t, y;
 if(d==1){
  DQ(m, D acc=0.0; D c=0.0; DQ(n, y=*wv-c; t=acc+y; acc=t-acc; c=acc-y; acc=t; ++wv;) *zv++=acc;)
 }else{
  // add down the columns to reduce memory b/w.  4 accumulators
  DQ(m, D *wv0; DQ(d, wv0=wv; D acc=0.0; D c=0.0; DQ(n, y=*wv0-c; t=acc+y; acc=t-acc; c=acc-y; acc=t; wv0+=d;) *zv++=acc; ++wv;) wv=wv0-(d-1); )
 }
#endif
 if(unlikely(NANTEST))R reduce(w,FAV(self)->fgh[0]);  // if NaN error, fail over to normal summation.  Infinities can cause it.  Ranks still set

 RETF(z);
}

// w is an array with 0 items, self is f, result is frame $ ,: identity-verb cell-shape $ atom-of-type
static DF1(jtred0){A x,z;I f,r,wr,*s;
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r; RESETRANK; s=AS(w);
 if(likely(!ISSPARSE(AT(w)))){GA(x,AT(w),0L,r,f+s);}else{GASPARSE(x,AT(w),1,r,f+s);}  // x exists only for type and shape
 R reitem(vec(INT,f,s),lamin1(dfv1(z,x,(AT(w)&SBT)?idensb(self):iden(self))));
}    /* f/"r w identity case */

// general reduce.  We inplace the results into the next iteration.  This routine cannot inplace its inputs.
static DF1(jtredg){F1PREFIP;PROLOG(0020);A fs=FAV(self)->fgh[0]; AF f2=FAV(fs)->valencefns[1]; AD * RESTRICT a;I i,n,r,wr;
 ARGCHK1(w);
 ASSERT(!ISSPARSE(AT(w)),EVNONCE);
 // loop over rank
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; RESETRANK;
 if(r<wr)R rank1ex(w,self,r,jtredg);
 // From here on we are doing a single reduction
 n=AS(w)[0]; // n=#cells
 // Allocate virtual block for the running x argument.
 fauxblock(virtafaux); fauxvirtual(a,virtafaux,w,r-1,ACUC1);  // allocate UNINCORPORABLE block
 // wfaux will hold the result from the iterations.  Init to value of last cell
 // Allocate fauxvirtual arg for the first cell, so it can be inplaceable/pristine if needed (tail returned a virtual block, which messed things up for high rank)
 fauxblock(virtwfaux); A wfaux; fauxvirtual(wfaux,virtwfaux,w,r-1,ACUC1);  // allocate UNINCORPORABLE block, mark inplaceable - used only once
   // finish filling the virt block
 A *old=jt->tnextpushp; // save stack mark for subsequent frees.  We keep the a argument over the calls, but allow the w to be deleted
 // fill in the shape, offset, and item-count of the virtual block
 I k; PROD(k,r-1,AS(w)+1);  // k=#atoms of cell of w
 AN(wfaux)=k; AN(a)=k;
 k<<=bplg(AT(w)); // k now=length of input cell in bytes, where it will remain
 AK(wfaux)+=(n-1)*k; AK(a)+=(n-2)*k; MCISH(AS(wfaux),AS(w)+1,r-1); MCISH(AS(a),AS(w)+1,r-1);  // make the virtual block look like the tail, except for the offset
 // Calculate inplaceability.  We can inplace the left arg, which is always virtual, if w is inplaceable and (w is direct or fs is &.>)
 // and the input jtinplace.  We turn off WILLBEOPENED status in jtinplace for the callee.
 // We include contextual inplaceability (from jtinplace) here because if the block is returned, its pristinity will be checked if it is inplaceable.  Thus
 // we do not want to call a faux argument inplaceable if it really isn't.  This gives us leeway with jtinplace itself
 I aipok = (SGNIF((I)jtinplace&(((AT(w)&TYPEVIPOK)!=0)|f2==jtevery2self),JTINPLACEWX)&AC(w))+ACUC1;   // requires JTINPLACEWX==0.  This is 1 or 8..1
 // We can inplace the right arg the first time if it is direct inplaceable, and always after that (assuming it is an inplaceable result).
 ACINIT(wfaux,aipok)   // first cell is inplaceable if second is
 jtinplace = (J)(intptr_t)(((I)jt) + (JTINPLACEW+JTINPLACEA)*((FAV(fs)->flag>>(VJTFLGOK2X-JTINPLACEWX)) & JTINPLACEW));  // all items are used only once

 // We need to free memory in case the called routine leaves it unfreed (that's bad form & we shouldn't expect it), and also to free the result of the
 // previous iteration.  We don't want to free every time, though, because that does ra() on w which could be a costly traversal if it's a nonrecursive recursible type.
 // As a compromise we free every few iterations: at least one per 8 iterations, and at least 8 times through the process
#define LGMINGCS 3  // lg2 of minimum number of times we call gc
#define MINGCINTERVAL 8  // max spacing between frees
 I freedist=MIN((n+((1<<LGMINGCS)-1))>>LGMINGCS,MINGCINTERVAL); I freephase=freedist;
 i=n-1; while(1){  // for each cell except the last
  ACRESET(a,aipok)   // in case we created a virtual block from it, restore inplaceability to the UNINCORPABLE block
  RZ(wfaux=CALL2IP(f2,a,wfaux,fs));
  if(--i==0)break;   // stop housekeeping after last iteration
  // if w happens to be the same virtual block that we passed in as x, we have to clone it before we change the pointer
  if(unlikely(a==wfaux)){RZ(wfaux=virtual(wfaux,0,AR(a))); AN(wfaux)=AN(a); MCISH(AS(wfaux),AS(a),AR(a));}
  if(--freephase==0){wfaux=gc(wfaux,old); freephase=freedist;}   // free the buffers we allocated, except for the result
  // move to next input cell
  AK(a) -= k;
 }
 // At the end of all this, it is possible that the result is the original first or last cell, resting in its original virtual block.
 // In that case, we have to realize it, so that we don't let the fauxvirtual block escape
 if(unlikely((AFLAG(wfaux)&AFUNINCORPABLE)!=0))wfaux=realize(wfaux);
 EPILOG(wfaux);  // this frees the virtual block, at the least
}    /* f/"r w for general f and 1<(-r){$w */


static const C fca[]={CSTAR, CPLUS, CEQ, CMIN, CMAX, CPLUSDOT, CSTARDOT, CNE, 0};  /* commutative & associative */

static A jtredsp1a(J jt,C id,A z,A e,I n,I r,I*s){A t;B b,p=0;D d=1;
 switch(id){
  default:       ASSERT(0,EVNONCE);
  case CPLUSDOT: R n?gcd(z,e):ca(e);
  case CSTARDOT: R n?lcm(z,e):ca(e);
  case CMIN:     R n?minimum(z,e):ca(e);
  case CMAX:     R n?maximum(z,e):ca(e);
  case CPLUS:    if(n&&equ(e,num(0)))R z; DO(r, d*=s[i];); t=tymes(e,d>=FLIMAX?scf(d-n):sc((I)d-n)); R n?plus(z,t):t;
  case CSTAR:    if(n&&equ(e,num(1) ))R z; DO(r, d*=s[i];); t=expn2(e,d>=FLIMAX?scf(d-n):sc((I)d-n)); R n?tymes(z,t):t;
  case CEQ:      p=1;  /* fall thru */
  case CNE:
   ASSERT(B01&AT(e),EVNONCE); 
   if(!n)BAV(z)[0]=p; 
   b=1; DO(r, if(!(s[i]&1)){b=0; break;}); 
   R !p==*BAV(e)&&b!=(n&1)?not(z):z;
}}   /* f/w on sparse vector w, post processing */

static A jtredsp1(J jt,A w,A self,C id,VARPSF ado,I cv,I f,I r,I zt){A e,x,z;I m,n;P*wp;
 ARGCHK1(w);
 wp=PAV(w); e=SPA(wp,e); x=SPA(wp,x); n=AN(x); m=AS(w)[0];
 GA00(z,zt,1,0);
 if(n){I rc=((AHDRRFN*)ado)(1L,n,1L,AV(x),AV(z),jt); if(255&rc)jsignal(rc); RE(0); if(m==n)R z;}
 R redsp1a(id,z,e,n,AR(w),AS(w));
}    /* f/"r w for sparse vector w */

DF1(jtredravel){A f,x,z;I n;P*wp;
 F1PREFIP;PROLOG(0000);
 ARGCHK1(w);
 f=FAV(self)->fgh[0];  // f/
 if(likely(!ISSPARSE(AT(w)))){RZ(z=reduce(jtravel(jtinplace,w),f));
 }else{
  // The rest is sparse
  wp=PAV(w); x=SPA(wp,x); n=AN(x);
  I rc=EVOK;
  while(1){  // Loop to handle restart on overflow
   VARPS adocv; varps(adocv,f,AT(x),0);
   ASSERT(adocv.f,EVNONCE);
   GA00(z,rtype(adocv.cv),1,0);
   if(n){rc=((AHDRRFN*)adocv.f)((I)1,n,(I)1,AV(x),AV(z),jt);}  // mustn't adocv on empty
   rc&=255; if(rc)jsignal(rc); if(rc<EWOV){if(rc)R0; RZ(z=redsp1a(FAV(FAV(f)->fgh[0])->id,z,SPA(wp,e),n,AR(w),AS(w))); break;}  // since f has an insert fn, its id must be OK
  }
 }
 EPILOG(z);
}  /* f/@, w */

static A jtredspd(J jt,A w,A self,C id,VARPSF ado,I cv,I f,I r,I zt){A a,e,x,z,zx;I c,m,n,*s,t,*v,wr,*ws,xf,xr;P*wp,*zp;
 ARGCHK1(w);
 ASSERT(strchr(fca,id),EVNONCE);
 wp=PAV(w); a=SPA(wp,a); e=SPA(wp,e); x=SPA(wp,x); s=AS(x);
 xr=r; v=AV(a); DO(AN(a), if(f<v[i])--xr;); xf=AR(x)-xr;
 m=prod(xf,s); c=m?AN(x)/m:0; n=s[xf];
 GA(zx,zt,AN(x)/n,AR(x)-1,s); MCISH(xf+AS(zx),1+xf+s,xr-1); 
 I rc=((AHDRRFN*)ado)(c/n,n,m,AV(x),AV(zx),jt); if(255&rc)jsignal(rc); RE(0);
 switch(id){
  case CPLUS: if(!equ(e,num(0)))RZ(e=tymes(e,sc(n))); break; 
  case CSTAR: if(!equ(e,num(1) )&&!equ(e,num(0)))RZ(e=expn2(e,sc(n))); break;
  case CEQ:   ASSERT(B01&AT(x),EVNONCE); if(!BAV(e)[0]&&0==(n&1))e=num(1); break;
  case CNE:   ASSERT(B01&AT(x),EVNONCE); if( BAV(e)[0]&&1==(n&1))e=num(0);
 }
 if(TYPESNE(AT(e),AT(zx))){t=maxtypene(AT(e),AT(zx)); if(TYPESNE(t,AT(zx)))RZ(zx=cvt(t,zx));}
 wr=AR(w); ws=AS(w);
 GASPARSE(z,STYPE(AT(zx)),1,wr-1,ws); if(1<wr)MCISH(f+AS(z),f+1+ws,wr-1);
 zp=PAV(z);
 RZ(a=ca(a)); v=AV(a); DO(AN(a), if(f<v[i])--v[i];);
 SPB(zp,a,a);
 SPB(zp,e,cvt(AT(zx),e));
 SPB(zp,i,SPA(wp,i));
 SPB(zp,x,zx);
 R z;
}    /* f/"r w for sparse w, rank > 1, dense axis */

static B jtredspsprep(J jt,C id,I f,I zt,A a,A e,A x,A y,I*zm,I**zdv,B**zpv,I**zqv,C**zxxv,A*zsn){
     A d,p,q,sn=0,xx;B*pv;C*xxv;I*dv,j,k,m,mm,*qv=0,*u,*v,yc,yr,yr1,*yv;
 v=AS(y); yr=v[0]; yc=v[1]; yr1=yr-1;
 RZ(d=grade1(eq(a,sc(f)))); dv=AV(d); 
 DO(AN(a), if(i!=dv[i]){RZ(q=grade1p(d,y)); qv=AV(q); break;});
 GATV0(p,B01,yr,1); pv=BAV1(p); mvc(yr,pv,MEMSET00LEN,MEMSET00);
 u=yv=AV(y); m=mm=0; j=-1; if(qv)v=yv+yc*qv[0];
 for(k=0;k<yr1;++k){
  if(qv){u=v; v=yv+yc*qv[1+k];}else v=u+yc;
  DO(yc-1, if(u[dv[i]]!=v[dv[i]]){++m; pv[k]=1; mm=MAX(mm,k-j); j=k; break;});
  if(!qv)u+=yc;
 }
 if(yr){++m; pv[yr1]=1; mm=MAX(mm,yr1-j);}
 if(qv){j=mm*aii(x); GA10(xx,AT(x),j); xxv=CAV(xx);}
 switch(id){
  case CPLUS: case CPLUSDOT: j=!equ(e,num(0)); break;
  case CSTAR: case CSTARDOT: j=!equ(e,num(1));  break;
  case CMIN:                 j=!equ(e,zt&B01?num(1) :zt&INT?sc(IMAX):ainf     ); break;
  case CMAX:                 j=!equ(e,zt&B01?num(0):zt&INT?sc(IMIN):scf(infm)); break;
  case CEQ:                  j=!*BAV(e);     break;
  case CNE:                  j= *BAV(e);     break;
 }
 if(j)GATV0(sn,INT,m,1);
 *zm=m; *zdv=dv; *zpv=pv; *zqv=qv; *zxxv=xxv; *zsn=sn;
 R 1;
}

static B jtredspse(J jt,C id,I wm,I xt,A e,A zx,A sn,A*ze,A*zzx){A b;B nz;I t,zt;
 RZ(b=ne(num(0),sn)); nz=!all0(b); zt=AT(zx);
 switch(id){
  case CPLUS:    if(nz)RZ(zx=plus (zx,       tymes(e,sn) )); RZ(e=       tymes(e,sc(wm)) ); break; 
  case CSTAR:    if(nz)RZ(zx=tymes(zx,bcvt(1,expn2(e,sn)))); RZ(e=bcvt(1,expn2(e,sc(wm)))); break;
  case CPLUSDOT: if(nz)RZ(zx=gcd(zx,from(b,over(num(0),e))));                 break;
  case CSTARDOT: if(nz)RZ(zx=lcm(zx,from(b,over(num(1),e))));                 break;
  case CMIN:     if(nz)RZ(zx=minimum(zx,from(b,over(zt&B01?num(1): zt&INT?sc(IMAX):ainf,     e)))); break;
  case CMAX:     if(nz)RZ(zx=maximum(zx,from(b,over(zt&B01?num(0):zt&INT?sc(IMIN):scf(infm),e)))); break;
  case CEQ:      ASSERT(B01&xt,EVNONCE); if(nz)RZ(zx=eq(zx,eq(num(0),residue(num(2),sn)))); if(!(wm&1))e=num(1);  break;
  case CNE:      ASSERT(B01&xt,EVNONCE); if(nz)RZ(zx=ne(zx,eq(num(1), residue(num(2),sn)))); if(!(wm&1))e=num(0); break;
 }
 if(TYPESNE(AT(e),AT(zx))){t=maxtypene(AT(e),AT(zx)); if(TYPESNE(t,AT(zx)))RZ(zx=cvt(t,zx));}
 *ze=e; *zzx=zx;
 R 1;
}

static A jtredsps(J jt,A w,A self,C id,VARPSF ado,I cv,I f,I r,I zt){A a,a1,e,sn,x,x1=0,y,z,zx,zy;B*pv;
     C*xv,*xxv,*zv;I*dv,i,m,n,*qv,*sv,*v,wr,xk,xt,wm,*ws,xc,yc,yr,*yu,*yv,zk;P*wp,*zp;
 ARGCHK1(w);
 ASSERT(strchr(fca,id),EVNONCE);
 wr=AR(w); ws=AS(w); wm=ws[f];
 wp=PAV(w); a=SPA(wp,a); e=SPA(wp,e); 
 y=SPA(wp,i); v=AS(y); yr=v[0]; yc=v[1]; yv=AV(y);
 x=SPA(wp,x); xt=AT(x); xc=aii(x);
 RZ(redspsprep(id,f,zt,a,e,x,y,&m,&dv,&pv,&qv,&xxv,&sn));
 xv=CAV(x); xk=xc<<bplg(xt);
 I zxr=AR(x); GA(zx,zt,m*xc,AR(x),AS(x)); AS(zx)[0]=m; zv=CAVn(zxr,zx); zk=xc<<bplg(zt);
 GATV0(zy,INT,m*(yc-1),2); v=AS(zy); v[0]=m; v[1]=yc-1; yu=AV2(zy);
 v=qv; if(sn)sv=AV(sn);
 for(i=0;i<m;++i){A y;B*p1;C*u;I*vv;
  p1=1+(B*)memchr(pv,C1,yr); n=p1-pv; if(sn)sv[i]=wm-n; pv=p1;
  vv=qv?yv+yc**v:yv; DO(yc-1, *yu++=vv[dv[i]];);
  if(1<n){if(qv){u=xxv; DO(n, MC(u,xv+xk*v[i],xk); u+=xk;);} I rc=((AHDRRFN*)ado)(xc,n,1L,qv?xxv:xv,zv,jt); if(255&rc)jsignal(rc); RE(0);}
  else   if(zk==xk)MC(zv,qv?xv+xk**v:xv,xk);
  else   {if(!x1)GA10(x1,xt,xc); MC(AV(x1),qv?xv+xk**v:xv,xk); RZ(y=cvt(zt,x1)); MC(zv,AV(y),zk);}
  zv+=zk; if(qv)v+=n; else{xv+=n*xk; yv+=n*yc;}
 }
 if(sn)RZ(redspse(id,wm,xt,e,zx,sn,&e,&zx));
 RZ(a1=ca(a)); v=AV(a1); n=0; DO(AN(a), if(f!=v[i])v[n++]=v[i]-(I )(f<v[i]););
 GASPARSE(z,STYPE(AT(zx)),1,wr-1,ws); if(1<r)MCISH(f+AS(z),f+1+ws,r-1);
 zp=PAV(z);
 SPB(zp,a,vec(INT,n,v)); 
 SPB(zp,e,cvt(AT(zx),e));
 SPB(zp,x,zx); 
 SPB(zp,i,zy);
 R z;
}    /* f/"r w for sparse w, rank > 1, sparse axis */

static DF1(jtreducesp){A a,g,z;B b;I f,n,r,*v,wn,wr,*ws,wt,zt;P*wp;
 ARGCHK1(w);J jtinplace=jt;
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r;  // no RESETRANK
 wn=AN(w); ws=AS(w); n=r?ws[f]:1;
 wt=AT(w); wt=wn?DTYPE(wt):B01;
 g=VAV(self)->fgh[0];  // g is the f in f/
 if(!n)R red0(w,g);  // red0 uses ranks, and resets them
 C id; if(AT(g)&VERB){id=FAV(g)->id; id=FAV(g)->flag&VISATOMIC2?id:0;}else id=0;
 VARPS adocv; varps(adocv,self,wt,0);
 if(2==n&&!(adocv.f&&strchr(fca,id))){
  A x; IRS2(num(0),w,0L,0,r,jtfrom,x); A y; IRS2(num(1),w,0L,0,r,jtfrom,y);
  R df2(z,x,y,g);  // rank has been reset for this call
 }
 // original rank still set
 if(!adocv.f)R redg(w,self);
 if(1==n)R tail(w);
 zt=rtype(adocv.cv);
 RESETRANK;
 if(1==wr)z=redsp1(w,self,id,adocv.f,adocv.cv,f,r,zt);
 else{
  wp=PAV(w); a=SPA(wp,a); v=AV(a);
  b=0; DO(AN(a), if(f==v[i]){b=1; break;});
  z=b?redsps(w,self,id,adocv.f,adocv.cv,f,r,zt):redspd(w,self,id,adocv.f,adocv.cv,f,r,zt);
 }
 R jt->jerr>=EWOV?IRS1(w,self,r,jtreducesp,z):z;
}    /* f/"r for sparse w */

#define BR2CASE(t,id)   ((((id)-CSTARCO)*7)+((0x160008>>(t))&7))  // unique inputs are 0 1 2 3 16 17 18-> 0 4 2 1 6 3 5    10110 .... .... .... 1000
// perform expression op on x and y, which have booleans in alternate bytes
// m is the number of bytes
#if SY_64
#define BOOLPAIRS(op) \
 {I *u=(I*)wv,x,y; UI4 *zvui=(UI4*)zv; /* running pointer, operand areas */ \
 DQ((m+SZI-1)>>(LGSZI-1), x=*u++; y=(x>>8); x=op; x&=ALTBYTES; x|=x>>8; x&=ALTSHORTS; x|=x>>16; x&=VALIDBOOLEAN; *zvui++=(UI4)x;)}
#define LITEQ(op) \
 {I *u=(I*)wv,x,y; UI4 *zvui=(UI4*)zv; /* running pointer, operand areas */ \
 DQ((m+SZI-1)>>(LGSZI-1), x=*u++; y=(x>>8); x&=ALTBYTES; y&=ALTBYTES; x=(op((x^y)+ALTBYTES)>>8)&(ALTBYTES&VALIDBOOLEAN); x|=x>>8; x&=ALTSHORTS; x|=x>>16; *zvui++=(UI4)x;)}
#else
#define BOOLPAIRS(op) \
 {I *u=(I*)wv,x,y; US *zvus=(US*)zv; /* running pointer, operand areas */ \
 DQ((m+SZI-1)>>(LGSZI-1), x=*u++; y=(x>>8); x=op; x&=ALTBYTES; x|=x>>8; x&=VALIDBOOLEAN; *zvus++=(US)x;)}
#define LITEQ(op) \
 {I *u=(I*)wv,x,y; US *zvus=(US*)zv; /* running pointer, operand areas */ \
 DQ((m+SZI-1)>>(LGSZI-1), x=*u++; y=(x>>8); x&=ALTBYTES; y&=ALTBYTES; x=(op((x^y)+ALTBYTES)>>8)&(ALTBYTES&VALIDBOOLEAN); x|=x>>8; *zvus++=(US)x;)}
#endif
#define TYPEDPAIRS(T,op) {T*u=(T*)wv,x,y; DQ(m, x=*u++; y=*u++; *zv++=op;)}
// compare literals for equality/inequality.  op  is ~ for ==

// handle reduction along final axis of length 2.  cv is case# to run.  Return allocated result
static A jtreduce2(J jt,A w,I cv,I f){A z=(A)1;B *zv;I m,*ws;
 I wn=AN(w); I wr=AR(w); ws=AS(w); void *wv=voidAV(w);
 PROD(m,f,ws);  // number of cells, each 2 atoms
 GATV(z,B01,wn>>1,wr-1,ws); zv=BAVn(wr-1,z);
 switch(cv){
  case BR2CASE(B01X,CEQ     ): BOOLPAIRS(~x^y) break;
  case BR2CASE(B01X,CNE     ): BOOLPAIRS(x^y) break;
  case BR2CASE(B01X,CLT     ): BOOLPAIRS(~x&y) break;
  case BR2CASE(B01X,CLE     ): BOOLPAIRS(~x|y) break;
  case BR2CASE(B01X,CGT     ): BOOLPAIRS(x&~y) break;
  case BR2CASE(B01X,CGE     ): BOOLPAIRS(x|~y) break;
  case BR2CASE(B01X,CMAX    ):
  case BR2CASE(B01X,CPLUSDOT): BOOLPAIRS(x|y) break;
  case BR2CASE(B01X,CPLUSCO ): BOOLPAIRS(~(x|y)) break;
  case BR2CASE(B01X,CMIN    ):
  case BR2CASE(B01X,CSTAR   ):
  case BR2CASE(B01X,CSTARDOT): BOOLPAIRS(x&y) break;
  case BR2CASE(B01X,CSTARCO ): BOOLPAIRS(~(x&y)) break;
  case BR2CASE(LITX,CEQ     ): LITEQ(~); break;
  case BR2CASE(LITX,CNE     ): LITEQ(); break;
  case BR2CASE(C2TX,CEQ     ): TYPEDPAIRS(US,x==y); break;
  case BR2CASE(C2TX,CNE     ): TYPEDPAIRS(US,x!=y); break;
  case BR2CASE(C4TX,CEQ     ): TYPEDPAIRS(C4,x==y); break;
  case BR2CASE(C4TX,CNE     ): TYPEDPAIRS(C4,x!=y); break;
  case BR2CASE(SBTX,CEQ     ): TYPEDPAIRS(SB,x==y); break;
  case BR2CASE(SBTX,CLT     ): TYPEDPAIRS(SB,SBLT(x,y)); break;
  case BR2CASE(SBTX,CLE     ): TYPEDPAIRS(SB,SBLE(x,y)); break;
  case BR2CASE(SBTX,CGT     ): TYPEDPAIRS(SB,SBGT(x,y)); break;
  case BR2CASE(SBTX,CGE     ): TYPEDPAIRS(SB,SBGE(x,y)); break;
  case BR2CASE(SBTX,CNE     ): TYPEDPAIRS(SB,x!=y); break;
  case BR2CASE(INTX,CEQ     ): TYPEDPAIRS(I,x==y); break;
  case BR2CASE(INTX,CLT     ): TYPEDPAIRS(I,x<y); break;
  case BR2CASE(INTX,CLE     ): TYPEDPAIRS(I,x<=y); break;
  case BR2CASE(INTX,CGT     ): TYPEDPAIRS(I,x>y); break;
  case BR2CASE(INTX,CGE     ): TYPEDPAIRS(I,x>=y); break;
  case BR2CASE(INTX,CNE     ): TYPEDPAIRS(I,x!=y); break;
  case BR2CASE(FLX, CEQ     ): TYPEDPAIRS(D,TEQ(x,y)); break;
  case BR2CASE(FLX, CLT     ): TYPEDPAIRS(D,TLT(x,y)); break;
  case BR2CASE(FLX, CLE     ): TYPEDPAIRS(D,TLE(x,y)); break;
  case BR2CASE(FLX, CGT     ): TYPEDPAIRS(D,TGT(x,y)); break;
  case BR2CASE(FLX, CGE     ): TYPEDPAIRS(D,TGE(x,y)); break;
  case BR2CASE(FLX, CNE     ): TYPEDPAIRS(D,TNE(x,y)); break;
  default: ASSERTSYS(0,"reduce2");
 }
 R z;
}    /* f/"r for dense w over an axis of length 2; boolean results only */
#define TW0(x,y) ((BR2CASE(x,y)>>LGBW)==0?1LL<<(BR2CASE(x,y)&(BW-1)):0)
#define TW1(x,y) ((BR2CASE(x,y)>>LGBW)==1?1LL<<(BR2CASE(x,y)&(BW-1)):0)
#define TW2(x,y) ((BR2CASE(x,y)>>LGBW)==2?1LL<<(BR2CASE(x,y)&(BW-1)):0)
#define TW3(x,y) ((BR2CASE(x,y)>>LGBW)==3?1LL<<(BR2CASE(x,y)&(BW-1)):0)
#define TWV0 TW0(B01X,CEQ)+TW0(B01X,CNE)+TW0(B01X,CLT)+TW0(B01X,CLE)+TW0(B01X,CGT)+TW0(B01X,CGE)+TW0(B01X,CMAX)+TW0(B01X,CPLUSDOT)+TW0(B01X,CPLUSCO)+TW0(B01X,CMIN)+TW0(B01X,CSTAR)+TW0(B01X,CSTARDOT)+ \
TW0(B01X,CSTARCO)+TW0(LITX,CEQ)+TW0(LITX,CNE)+TW0(C2TX,CEQ)+TW0(C2TX,CNE)+TW0(C4TX,CEQ)+TW0(C4TX,CNE)+TW0(SBTX,CEQ)+TW0(SBTX,CLT)+TW0(SBTX,CLE)+TW0(SBTX,CGT)+TW0(SBTX,CGE)+TW0(SBTX,CNE)+ \
TW0(INTX,CEQ)+TW0(INTX,CLT)+TW0(INTX,CLE)+TW0(INTX,CGT)+TW0(INTX,CGE)+TW0(INTX,CNE)+TW0(FLX, CEQ)+TW0(FLX, CLT)+TW0(FLX, CLE)+TW0(FLX, CGT)+TW0(FLX, CGE)+TW0(FLX, CNE)
#define TWV1 TW1(B01X,CEQ)+TW1(B01X,CNE)+TW1(B01X,CLT)+TW1(B01X,CLE)+TW1(B01X,CGT)+TW1(B01X,CGE)+TW1(B01X,CMAX)+TW1(B01X,CPLUSDOT)+TW1(B01X,CPLUSCO)+TW1(B01X,CMIN)+TW1(B01X,CSTAR)+TW1(B01X,CSTARDOT)+ \
TW1(B01X,CSTARCO)+TW1(LITX,CEQ)+TW1(LITX,CNE)+TW1(C2TX,CEQ)+TW1(C2TX,CNE)+TW1(C4TX,CEQ)+TW1(C4TX,CNE)+TW1(SBTX,CEQ)+TW1(SBTX,CLT)+TW1(SBTX,CLE)+TW1(SBTX,CGT)+TW1(SBTX,CGE)+TW1(SBTX,CNE)+ \
TW1(INTX,CEQ)+TW1(INTX,CLT)+TW1(INTX,CLE)+TW1(INTX,CGT)+TW1(INTX,CGE)+TW1(INTX,CNE)+TW1(FLX, CEQ)+TW1(FLX, CLT)+TW1(FLX, CLE)+TW1(FLX, CGT)+TW1(FLX, CGE)+TW1(FLX, CNE)
#if !SY_64
#define TWV2 TW2(B01X,CEQ)+TW2(B01X,CNE)+TW2(B01X,CLT)+TW2(B01X,CLE)+TW2(B01X,CGT)+TW2(B01X,CGE)+TW2(B01X,CMAX)+TW2(B01X,CPLUSDOT)+TW2(B01X,CPLUSCO)+TW2(B01X,CMIN)+TW2(B01X,CSTAR)+TW2(B01X,CSTARDOT)+ \
TW2(B01X,CSTARCO)+TW2(LITX,CEQ)+TW2(LITX,CNE)+TW2(C2TX,CEQ)+TW2(C2TX,CNE)+TW2(C4TX,CEQ)+TW2(C4TX,CNE)+TW2(SBTX,CEQ)+TW2(SBTX,CLT)+TW2(SBTX,CLE)+TW2(SBTX,CGT)+TW2(SBTX,CGE)+TW2(SBTX,CNE)+ \
TW2(INTX,CEQ)+TW2(INTX,CLT)+TW2(INTX,CLE)+TW2(INTX,CGT)+TW2(INTX,CGE)+TW2(INTX,CNE)+TW2(FLX, CEQ)+TW2(FLX, CLT)+TW2(FLX, CLE)+TW2(FLX, CGT)+TW2(FLX, CGE)+TW2(FLX, CNE)
#define TWV3 TW3(B01X,CEQ)+TW3(B01X,CNE)+TW3(B01X,CLT)+TW3(B01X,CLE)+TW3(B01X,CGT)+TW3(B01X,CGE)+TW3(B01X,CMAX)+TW3(B01X,CPLUSDOT)+TW3(B01X,CPLUSCO)+TW3(B01X,CMIN)+TW3(B01X,CSTAR)+TW3(B01X,CSTARDOT)+ \
TW3(B01X,CSTARCO)+TW3(LITX,CEQ)+TW3(LITX,CNE)+TW3(C2TX,CEQ)+TW3(C2TX,CNE)+TW3(C4TX,CEQ)+TW3(C4TX,CNE)+TW3(SBTX,CEQ)+TW3(SBTX,CLT)+TW3(SBTX,CLE)+TW3(SBTX,CGT)+TW3(SBTX,CGE)+TW3(SBTX,CNE)+ \
TW3(INTX,CEQ)+TW3(INTX,CLT)+TW3(INTX,CLE)+TW3(INTX,CGT)+TW3(INTX,CGE)+TW3(INTX,CNE)+TW3(FLX, CEQ)+TW3(FLX, CLT)+TW3(FLX, CLE)+TW3(FLX, CGT)+TW3(FLX, CGE)+TW3(FLX, CNE)
#endif
static DF1(jtreduce){A z;I d,f,m,n,r,t,wr,*ws,zt;
 F1PREFIP;ARGCHK1(w);
 if(unlikely(ISSPARSE(AT(w))))RETF(reducesp(w,self));  // If sparse, go handle it
 wr=AR(w); ws=AS(w);
 // Create  r: the effective rank; f: length of frame; n: # items in a CELL of w
 r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r; SETICFR(w,f,r,n);  // no RESETRANK
 // Handle the special cases: neutrals, single items, lists of length 2
 I wt=AT(w); wt=AN(w)?wt:B01;   // Treat empty as Boolean type

 if(unlikely(n<=2)){
  if(unlikely(n==1))RETF(head(w));   // 1 item: the result is the item.  Rank is still set
  if(unlikely(n==0))RETF(red0(w,FAV(self)->fgh[0]));  // 0 item: return a neutral using shape and rank.  Rank is still set
  if(unlikely(r==1))if(likely(wt&B01+LIT+INT+FL+SBT+C2T+C4T)){  // 2 items: special processing only if the operation is on rank 1: then we avoid loop overheads
   C id=FAV(FAV(self)->fgh[0])->id; 
   if(unlikely(BETWEENC(id,CSTARCO,CMAX))){  // only boolean results are supported
    I cv=BR2CASE(CTTZ(wt),id); UI cwd=TWV0; cwd=(cv>>LGBW)==1?TWV1:cwd;  // figure the case, and see if it is one of those in the big macros above
#if !SY_64
    cwd=(cv>>LGBW)==2?TWV2:cwd; cwd=(cv>>LGBW)==3?TWV3:cwd;
#endif
    if(likely(((1LL<<(cv&(BW-1)))&cwd)!=0))RETF(jtreduce2(jt,w,cv,f));
   }
  }  // fall through for 2 items that can't be handled specially
 }
 // Normal case, >1 item.
 // The case of empty w is interesting, because the #cells, and the #atoms in an item of a cell, may both overflow if
 // n=0.  But we have handled that case above.  If n is not 0, there may be other zeros in the shape that allow
 // an overflow when an infix of the shape is multiplied; but that won't matter because the other 0 will guarantee that there
 // are no atoms written

 // Normal processing for multiple items.  Get the routine & flags to process it
 VARPS adocv; varps(adocv,self,wt,0);
 // If there is no special routine, go perform general reduce
 if(!adocv.f)RETF(redg(w,self));  // jt->ranks is still set.  redg will clear the ranks
 // Here for primitive reduce handled by special code.
 // Calculate m: #cells of w to operate on; d: #atoms in an item of a cell of w (a cell to which u is applied);
 // zn: #atoms in result
 PROD(d,r-1,f+ws+1);  //  */ }. $ cell
 // m=*/ frame (i. e. #cells to operate on)
 // r cannot be 0 (would be handled above).  Calculate low part of zn first
 PROD(m,f,ws);
 RESETRANK;   // clear rank now that we've used it - not really required here?
 // Allocate the result area
 zt=rtypew(adocv.cv,wt);  // Use specified type if given, otherwise use type of argument
 GA(z,zt,m*d,MAX(0,wr-1),ws); if(1<r)MCISH(f+AS(z),f+1+ws,r-1);  // allocate, and install shape
 if(m*d==0){RETF(z);}  // mustn't call the function on an empty argument!
 // Convert inputs if needed 
 if((t=atype(adocv.cv))&&TYPESNE(t,wt))RZ(w=cvt(t,w));
 // call the selected reduce routine.
 I rc=((AHDRRFN*)adocv.f)(d,n,m,AV(w),AV(z),jt);
 // if return is EWOV, it's an integer overflow and we must restart, after restoring the ranks
 // EWOV1 means that there was an overflow on a single result, which was calculated accurately and stored as a D.  So in that case all we
 // have to do is change the type of the result.
 if(unlikely((255&~EVNOCONV)&rc)){if(unlikely(rc==EVNOCONV))RETF(z); if(jt->jerr==EWOV1){AT(z)=FL;RETF(z);}else {jsignal(rc); RETF(rc>=EWOV?IRS1(w,self,r,jtreduce,z):0);}} else {RETF((adocv.cv&VRI+VRD)&&rc!=EVNOCONV?cvz(adocv.cv,z):z);}
}    /* f/"r w main control */

static A jtredcatsp(J jt,A w,A z,I r){A a,q,x,y;B*b;I c,d,e,f,j,k,m,n,n1,p,*u,*v,wr,*ws,xr;P*wp,*zp;
 ws=AS(w); wr=AR(w); f=wr-r; p=ws[1+f];
 wp=PAV(w); x=SPA(wp,x); y=SPA(wp,i); a=SPA(wp,a); v=AV(a); 
 m=AS(y)[0]; n=AN(a); n1=n-1; xr=AR(x);
 RZ(b=bfi(wr,a,1));
 c=b[f]; d=b[1+f]; if(c&&d)b[f]=0; e=f+!c;
 j=0; DO(n, if(e==v[i]){j=i; break;}); 
 k=1; DO(f, if(!b[i])++k;);
 zp=PAV(z); SPB(zp,e,ca(SPA(wp,e)));
 GATV0(q,INT,n-(I )(c&&d),1); v=AV1(q); DO(wr, if(b[i])*v++=i-(I )(i>f);); SPB(zp,a,q);
 if(c&&d){          /* sparse sparse */
  SPB(zp,x,ca(x));
  SPB(zp,i,repeatr(ne(a,sc(f)),y)); q=SPA(zp,i);  // allow for virtualization of SPB
  v=j+AV(q); u=j+AV(y);
  DQ(m, *v=p*u[0]+u[1]; v+=n1; u+=n;);
 }else if(!c&&!d){  /* dense dense */
  u=AS(x); GA(q,AT(x),AN(x),xr-1,u); v=k+AS(q); *v=u[k]*u[1+k]; MCISH(1+v,2+k+u,xr-k-2);
  MC(AV(q),AV(x),AN(x)<<bplg(AT(x)));
  SPB(zp,x,q); SPB(zp,i,ca(y));
 }else{             /* other */
  GATV0(q,INT,xr,1); v=AV1(q); 
  if(1!=k){*v++=0; *v++=k; e=0; DQ(xr-1, ++e; if(e!=k)*v++=e;); RZ(x=cant2(q,x));}
  v=AV(q); u=AS(x); *v=u[0]*u[1]; MCISH(1+v,2+u,xr-1); RZ(x=reshape(vec(INT,xr-1,v),x));
  e=ws[f+c]; RZ(y=repeat(sc(e),y)); RZ(y=mkwris(y)); v=j+AV(y);
  if(c)DO(m, k=p**v; DO(e, *v=k+  i; v+=n;);)
  else DO(m, k=  *v; DO(e, *v=k+p*i; v+=n;);); 
  RZ(q=grade1(y)); RZ(y=from(q,y)); RZ(x=from(q,x));
  SPB(zp,x,x); SPB(zp,i,y);
 }
 R z;
}    /* ,/"r w for sparse w, 2<r */

// ,&.:(<"r)  run together all axes above the last r.  r must not exceed AR(w)-1
// w must not be sparse or empty
A jtredcatcell(J jt,A w,I r){A z;
 F1PREFIP;ARGCHK1(w);
 I wr=AR(w);  // get original rank, which may change if we inplace into the same block
 if(r>=wr-1)R RETARG(w);  // if only 1 axis left to run together, return the input
 if((ASGNINPLACESGN(SGNIF(jtinplace,JTINPLACEWX)&(-r),w) && !(AFLAG(w)&AFUNINCORPABLE))){  // inplace allowed, usecount is right
  // operation is loosely inplaceable.  Just shorten the shape to frame,(#atoms in cell).  We do this here rather than relying on
  // the self-virtual-block code in virtual() because we can do it for indirect types also, since we know we are not changing
  // the number of atoms
  z=w; AR(z)=(RANKT)(r+1);  // inplace it, install new rank
 }else{
  RZ(z=virtual(w,0,r+1)); AN(z)=AN(w);   // create virtual block
 }
 I m; PROD(m,wr-r,AS(w));   // # cells being strung together
 AS(z)[0]=m; MCISH(AS(z)+1,AS(w)+wr-r,r);  // install # cells and then shift the last r axes back 
 R z;
}


DF1(jtredcat){A z;B b;I f,r,*s,*v,wr;
 F1PREFIP;ARGCHK1(w);
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r; s=AS(w); RESETRANK;
 b=1==r&&1==s[f];  // special case: ,/ on last axis which has length 1: in that case, the rules say the axis disappears (because of the way ,/ works on length-1 lists)
 if(2>r&&!b)RCA(w);  // in all OTHER cases, result=input for ranks<2
 // use virtual block (possibly self-virtual) for all cases except sparse
 if(likely(!ISSPARSE(AT(w)))){
  RZ(z=jtvirtual(jtinplace,w,0,wr-1)); AN(z)=AN(w); // Allocate the block.  Then move in AN and shape
  I *zs=AS(z); MCISH(zs,s,f); if(!b){DPMULDE(s[f],s[f+1],zs[f]); MCISH(zs+f+1,s+f+2,r-2);}
  R z;
 }else{
  GASPARSE(z,AT(w),AN(w),wr-1,s); 
  if(!b){v=f+AS(z); DPMULDE(s[f],s[1+f],*v); MCISH(1+v,2+f+s,r-2);}
  R redcatsp(w,z,r);
 }
}    /* ,/"r w */

static DF1(jtredsemi){I f,n,r,wr;
 ARGCHK1(w);
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r; SETICFR(w,f,r,n);   // let the rank run into tail   n=#items in a cell of w
 if(2>n){ASSERT(n!=0,EVDOMAIN); R tail(w);}  // rank still set
 if(BOX&AT(w))R jtredg(jt,w,self);  // the old way failed because it did not mimic scalar replication; revert to the long way.  ranks are still set
 else{A z; R IRS1(w,0L,r-1,jtbox,z);}  // unboxed, just box the cells
}    /* ;/"r w */

static DF1(jtredstitch){A c,y;I f,n,r,*s,*v,wr;
 ARGCHK1(w);
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r; RESETRANK;
 s=AS(w); SETICFR(w,f,r,n);
 ASSERT(n!=0,EVDOMAIN);
 if(1==n)R IRS1(w,0L,r,jthead,y);
 if(1==r){if(2==n)R RETARG(w); A z1,z2,z3; RZ(IRS2(num(-2),w,0L,0L,1L,jtdrop,z1)); RZ(IRS2(num(-2),w,0L,0L,1L,jttake,z2)); R IRS2(z1,z2,self,1L,0L,jtover,z3);}
 if(2==r)R IRS1(w,0L,2L,jtcant1,y);
 RZ(c=apvwr(wr,0L,1L)); v=AV(c); v[f]=f+1; v[f+1]=f; RZ(y=cant2(c,w));  // transpose last 2 axes
 if(unlikely(ISSPARSE(AT(w)))){A x;
  GATV0(x,INT,f+r-1,1); v=AV1(x); MCISH(v,AS(y),f+1);
  DPMULDE(s[f],s[f+2],v[f+1]); MCISH(v+f+2,s+3+f,r-3);
  RETF(reshape(x,y));
 }else{
  v=AS(y); 
  DPMULDE(s[f],s[f+2],v[f+1]); MCISH(v+f+2,s+3+f,r-3);
  --AR(y); 
  RETF(y);
}}   /* ,./"r w */

static DF1(jtredstiteach){A*wv,y;I n,p,r,t;
 ARGCHK1(w);
 n=AN(w);
 if(!(2<n&&1==AR(w)&&BOX&AT(w)))R reduce(w,self);
 wv=AAV(w);  y=C(wv[0]); SETIC(y,p); t=AT(y);
 DO(n, y=C(wv[i]); r=AR(y); if(!((((r-1)&-2)==0)&&p==SETIC(y,n)&&TYPESEQ(t,AT(y))))R reduce(w,self););  // rank 1 or 2, rows match, equal types
 R box(razeh(w));
}    /* ,.&.>/ w */

static DF1(jtredcateach){A*u,*v,*wv,x,*xv,z,*zv;I f,m,mn,n,r,wr,*ws,zm,zn;I n1=0,n2=0;
 ARGCHK1(w);
 wr=AR(w); ws=AS(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r; RESETRANK;
 SETICFR(w,f,r,n);
 if(!r||1>=n)R reshape(repeat(ne(sc(f),IX(wr)),shape(w)),n?w:ds(CACE));
 if(!(BOX&AT(w))){A t; RZ(t=cant2(sc(f),w)) R IRS1(t,0,1,jtbox,z);}  // handle unboxed args by transposing on the given axis and then boxing lists
// bug: ,&.>/ y does scalar replication wrong
// wv=AN(w)+AAV(w); DQ(AN(w), if(AN(*--wv)&&AR(*wv)&&n1&&n2) ASSERT(0,EVNONCE); if((!AR(*wv))&&n1)n2=1; if(AN(*wv)&&1<AR(*wv))n1=1;);
 zn=AN(w)/n; PROD(zm,f,ws); PROD(m,r-1,ws+f+1); mn=m*n;
 GATV(z,BOX,zn,wr-1,ws); MCISH(AS(z)+f,ws+f+1,r-1);
 GATV0(x,BOX,n,1); xv=AAV1(x);
 zv=AAVn(wr-1,z); wv=AAV(w); 
 DO(zm, u=wv; DO(m, v=u++; DO(n, xv[i]=*v; v+=m;); A Zz; RZ(Zz=raze(x)); INCORP(Zz); *zv++ = Zz;); wv+=mn;);  // no need to incorp *v since it's already boxed
 RETF(z);
}    /* ,&.>/"r w */

static DF2(jtoprod){A z; R dfv2(z,a,w,FAV(self)->fgh[2]);}  // x u/ y - transfer to the u"lr,_ verb (precalculated)


F1(jtslash){F1PREFIP;A h;AF f1;C c;V*v;
 ARGCHK1(w);
 if(NOUN&AT(w))R evger(w,sc(GINSERT));  // treat m/ as m;.6.  This means that a node with CSLASH never contains gerund u
 // falling through w is a verb
 A z; fdefallo(z)
 v=FAV(w); 
 I flag=v->flag&VASGSAFE;  // if u is asgsafe, so is u/
 switch(v->id){  // select the monadic case
 case CCOMMA:  f1=jtredcat; flag|=VJTFLGOK1;   break;
 case CCOMDOT: f1=jtredstitch; break;
 case CSEMICO: f1=jtredsemi; break;
 case CUNDER:  f1=jtreduce; if(COPE==IDD(v->fgh[1])){c=FAV(v->fgh[0])->id; if(c==CCOMMA)f1=jtredcateach; else if(c==CCOMDOT)f1=jtredstiteach;} break;
 default: f1=jtreduce; flag|=(v->flag&VJTFLGOK2)>>(VJTFLGOK2X-VJTFLGOK1X); break;  // monad is inplaceable if the dyad for u is
 }
 RZ(h=qq(w,v2(lr(w),RMAX)));  // create the rank compound to use if dyad
 fdeffillall(z,0,CSLASH,VERB, f1,jtoprod, w,0L,h, flag|FAV(ds(CSLASH))->flag, RMAX,RMAX,RMAX,fffv->localuse.lu0.cachedloc=0,FAV(z)->localuse.lu1.redfn=v->flag&VISATOMIC2?((VA*)((I)va+v->localuse.lu1.uavandx[1]))->rps:&rpsnull);
 // set localuse to point to the VARPSA block for w if w is atomic dyad; otherwise to the null VARPSA block
 R z;
}

A jtaslash (J jt,C c,    A w){RZ(   w); A z; R dfv1(z,  w,   slash(ds(c))     );}
A jtaslash1(J jt,C c,    A w){RZ(   w); A z; R dfv1(z,  w,qq(slash(ds(c)),zeroionei(1)));}
A jtatab   (J jt,C c,A a,A w){ARGCHK2(a,w); A z; R dfv2(z,a,w,   slash(ds(c))     );}

DF1(jtmean){
 ARGCHK1(w);
 I wr=AR(w); I r=(RANKT)jt->ranks; r=wr<r?wr:r;
 I n=AS(w)[wr-r]; n=r?n:1;
 // leave jt->ranks unchanged to pass into +/
A sum=reduce(w,FAV(self)->fgh[0]);  // calculate +/"r
 RESETRANK;  // back to infinite rank for the divide
 RZ(sum);
 RZ(w=jtatomic2(JTIPA,sum,sc(n),ds(CDIV)));  // take quotient inplace and return it
 RETF(w);
}    // (+/%#)"r w, implemented as +/"r % cell-length

#if 0   // keep as example of coding a primitive as an addon
// entry point to execute monad/dyad Fold after the noun arguments are supplied
static DF2(jtfoldx){F2PREFIP;  // this stands in place of jtxdefn, which inplaces
 // see if this is monad or dyad
 I foldflag=((~AT(w))>>(VERBX-3))&8;  // flags: dyad mult fwd rev  if w is not conj, this must be a dyad call
 self=foldflag?self:w; w=foldflag?w:a; a=foldflag?a:mtv; // if monad, it's w self garbage,  move to '' w self
 // get the rest of the flags from the original ID byte, which was moved to lc
 foldflag|=FAV(self)->lu2.lc-CFDOT;  // this sets mult fwd rev
 // define the flags as the special global
 RZ(jtsymbis((J)((I)jt|JTFINALASGN),nfs(11,"Foldtype_j_"),sc(foldflag),jt->locsyms));
 // execute the Fold.  While it is running, set the flag to allow Z:
 B foldrunning=jt->foldrunning; jt->foldrunning=1; A z=(*(FAV(self)->localuse.lu1.foldfn))(jt,a,w,self); jt->foldrunning=foldrunning;
 // if there was an error, save the error code and recreate the error at this level, to cover up details inside the script
 if(jt->jerr){I e=jt->jerr; RESETERR; jsignal(e);}
 R z;
}

// u F. F.. F.: F: F:. F:: v
DF2(jtfold){F2PREFIP;
 // Apply Fold_j_ to the input arguments, creating a derived verb to do the work
 ASSERTVV(a,w)   // nouns not allowed
 A foldconj; ASSERT(foldconj=jtfindnameinscript(jt,"~addons/dev/fold/foldr.ijs","Foldr_j_",CONJ),EVNONCE);
 A derivvb; RZ(derivvb=jtunquote((J)((I)jt|JTXDEFMODIFIER),a,w,foldconj));
 // If the returned verb has VXOPCALL set, that means we are in debug and a namerefop has been interposed for Foldr_j_.  We don't want that - get the real verb
 if(unlikely(FAV(derivvb)->flag&VXOPCALL))derivvb=FAV(derivvb)->fgh[2];  // the verb is saved in h of the reference
 // Modify the derived verb to go to our preparatory stub.  Save the dyadic entry point for the derived verb so the stub can call it
 FAV(derivvb)->localuse.lu1.foldfn=FAV(derivvb)->valencefns[1];
 FAV(derivvb)->valencefns[0]=FAV(derivvb)->valencefns[1]=jtfoldx;
 // Tell the stub what the original fold type was
 FAV(derivvb)->lu2.lc=FAV(self)->id;
 // For display purposes, give the fold the spelling of the original
 FAV(derivvb)->id=FAV(self)->id;
 R derivvb;
}

// [x] Z: y, bivalent
DF2(jtfoldZ){
 ASSERT(jt->foldrunning,EVSYNTAX);  // If fold not running, fail.  Should be a semantic error rather than syntax
 // The name FoldZ_j_ should have been loaded at startup.  If not, fail
 A foldvb; ASSERT(foldvb=jtfindnameinscript(jt,"~addons/dev/fold/foldr.ijs","FoldZ_j_",VERB),EVNONCE)   // error if undefined or not verb
 A z=unquote(a,EPMONAD?foldvb:w,foldvb); // Apply FoldZ_j_ to the input arguments, creating a derived verb to do the work.  If monad, overwrite w
 // if there was an error, save the error code and recreate the error at this level, to cover up details inside the script
 if(jt->jerr){I e=jt->jerr; RESETERR; jsignal(e);}
 R z;
}

#else
#define ZZDEFN
#include "result.h"

 // execute fold on input arguments, creating a derived verb to do the work.  Bivalent
// bit 24 is PRISTINE
#define STATEREVX 26   // reverse fold
#define STATEREV ((I)1<<STATEREVX)
#define STATEFWDX 27   // forward fold
#define STATEFWD ((I)1<<STATEFWDX)
#define STATEMULTX 28   // multiple fold
#define STATEMULT ((I)1<<STATEMULTX)
#define STATEDYADX 29   // call is dyadic.  Must be the MSB of the flags
#define STATEDYAD ((I)1<<STATEDYADX)
static DF2(jtfold12){F2PREFIP;A z,vz;
 struct foldstatus foldinfo={{0,0,0},0};  // fold status shared between F: and Z:   zstatus: fold limit; abort; abort iteration; quiet iteration; halt after current
 ARGCHK2(a,w);
 I dyad=EPDYAD; w=EPDYAD?w:a; A uself=FAV(self)->fgh[0]; A vself=FAV(self)->fgh[1];
// obsolete self=dyad?self:w;
 if(unlikely(((UI)a^(UI)w)<(UI)dyad))jtinplace=(J)((I)jtinplace&~(JTINPLACEA|JTINPLACEW));  // can't inplace equal args
#define ZZFLAGWORD dmfr
 I dmfr=ZZFLAGINITSTATE+((8*dyad+FAV(self)->lu2.lc-CFDOT)<<STATEREVX);  // init flags, including zz flags
 struct foldstatus *stkfoldinfo=jt->afoldinfo; jt->afoldinfo=&foldinfo;  // push fold status, init to zeros
 //  ***** no error returns allowed!  This push must be matched *******
 // allocate result, always boxes.  For Multiple, it will be a list of boxes; for Single, it will be an atomic box.  It is recursive, so that we are allowed
 // to tpop everything that comes after.  We will open it at the end
 I wr=AR(w); I wcr=wr-SGNTO0(-wr);  // rank of w, rank of an item of w
 UI wni; SETIC(w,wni); UI nitems=wni+(dmfr>>STATEDYADX);  // #items of y; # items to process into u including x if given; number of turns through loop
 UI zzalloc=64-(AKXR(1)>>LGSZI); zzalloc=MIN(nitems,zzalloc); zzalloc=dmfr&STATEFWD+STATEREV?zzalloc:16-(AKXR(1)>>LGSZI); zzalloc=dmfr&STATEMULT?zzalloc:1;  // initial result allo.  zzalloc is #boxes in zz; AN(zz) is # valid
 A zz; GATV0E(zz,INT,zzalloc,dmfr&STATEMULT?1:0,goto exitpop;) AT(zz)=BOX; AFLAG(zz)=BOX&RECURSIBLE; AN(zz)=0;   // alloc result area.  avoid init of BOX area
 A virtw;  // virtual block for items of y, if needed
 I wstride;  // dist between items of y, in bytes, pos/neg/0 based on fwd/rev
 if(likely((dmfr&STATEFWD+STATEREV)!=0)){
  // fwd/reverse fold, the usual case
  if(unlikely(nitems<2)){  // < 2 items
   // < 2 items total.  We can't run v on items of input.
   // Create the cell to run u on: one given argument or a cell of fills
   if(nitems==1){
    // 1 item.  Could come from x (if y is empty) or y.  Apply u to it, to give the final result
    ++foldinfo.exestats[0]; foldinfo.zstatus=0; dfv1(z,dmfr&STATEDYAD?a:head(w),uself);
   }else{
    // 0 items (necessarily monadic).  Error if fold multiple.  Create a neutral for v from an item of y, and apply u to it
    ASSERT(!(dmfr&STATEMULT),EVDOMAIN)  // empty multiple fold is < 0 applications of v, error
    A fillcell=jtred0(jt,w,vself);  // a neutral with the shape of an item of y
    ASSERTGOTO(fillcell!=0,EVDOMAIN,exitpop)   // error if v has no neutral
    ++foldinfo.exestats[0]; foldinfo.zstatus=0; dfv1(z,fillcell,uself);
   }
   // we have applied u to the single cell.
   ASSERTGOTO(!(foldinfo.zstatus&0b01111),EVNORESULT,exitpop)  // z stopped iteration and no result was created: that's a no result error
   if(z==0)goto errfinish;
   // no error, return the single result.  If multiple, prepend a leading 0 axis
   ++foldinfo.exestats[2];
   if(dmfr&STATEMULT)z=reshape(over(zeroionei(0),shape(z)),z);  // if multiple, z =. (00,$z) ($,) z
   zz=z; goto abortexit;  // return z
  }else{
   // At least 2 items.  Run the fold loop.
   // Track the items of y (x arg into v) using a virtual arg
   fauxblock(virtwfaux);
   // if the original block was direct inplaceable, make the virtual block inplaceable.  (We can't do this for indirect blocks because a virtual block is not marked recursive - rather it increments
   // the usecount of the entire backing block - and modifying the virtual contents would leave the usecounts invalid since the backing block is always recursive (having been ra'd).  Maybe could do this if it isn't?)
   I wcn; PROD(wcn,wcr,AS(w)+1);   // number of atoms in a cell
   fauxvirtualcommon(virtw,virtwfaux,w,wcr,ACUC1,goto exitpop) MCISH(AS(virtw),AS(w)+1,wcr); AN(virtw)=wcn;  // note: no self-virtual
   I wk=bplg(AT(w));  // number of bytes in an atom of w
   I item0nofst=(wni-1)*(wcn&REPSGN(SGNIF(dmfr,STATEREVX)));  // offset to first/last element of y, in atoms
   wstride=((wcn<<wk)^REPSGN(SGNIF(dmfr,STATEREVX)))-(REPSGN(SGNIF(dmfr,STATEREVX)));  // dist between items of y, in bytes, pos/neg based on rev
   AK(virtw)+=(item0nofst<<wk)+(dmfr&STATEDYAD?0:wstride);  // point to first item (if dyad) or second (if monad) as the first x arg
   // Mark the virtual block (coming into v as a) as inplaceable only if input y is fully inplaceable.  We have to turn off inplaceability in the virtual block so that
   // a non-inplaceable value might cause PRISTINE to be set.  We also require the type to be right, with some allowances for &.>
   dmfr |= (UI)(SGNIF(jtinplace,JTINPLACEWX)&(-(AT(w)&TYPEVIPOK)&AC(w)))>>(BW-1-ZZFLAGVIRTAINPLACEX);   // requires JTINPLACEWX==0.  Single flag bit

   // vz will be the previous full result of v, usually inplaceable; init to x or first item of y
   if(dmfr&STATEDYAD){vz=a; dmfr|=((I)jtinplace&JTINPLACEA)<<(ZZFLAGVIRTWINPLACEX-JTINPLACEAX); // x given, use it as first input and use its inplaceability
   }else{
    vz=virtualip(w,item0nofst,wcr); if(unlikely(vz==0))goto exitpop; AN(vz)=wcn; MCISH(AS(vz),AS(w)+1,wcr);  // create a virtual block to the first item, possibly self-virtual, fill in
    if(dmfr&ZZFLAGVIRTAINPLACE){ACRESET(vz,ACUC1+ACINPLACE); dmfr|=ZZFLAGVIRTWINPLACE;} // inplaceability of w might have changed: if it was originally inplaceable, make this virtual inplaceable
   }

   // Install initial inplaceability.  Since the inplaceability of v was used for F., we can assume that any inplaceability shown is allowed for v.
   // Remove WILLOPEN status which we are ignoring.  Values were calculated above
   jtinplace = (J)(((I)jtinplace & ~(JTWILLBEOPENED+JTCOUNTITEMS+JTINPLACEW+JTINPLACEA))|((dmfr>>ZZFLAGVIRTWINPLACEX)&(JTINPLACEW+JTINPLACEA)));

   --nitems;  // convert #items to # executions of loop
  }
 }else{  // here for unlimited fold
  nitems=0;  // convert to infinite loop
  jtinplace = (J)((I)jtinplace & ~(JTWILLBEOPENED+JTCOUNTITEMS+JTINPLACEA));  // never inplace x, which repeats if given; pass along original inplaceability of y
  virtw=a; vz=w;  // v starts on w and then reapplies to the result; save a reg by moving a to virtw
 }
 A *_old=jt->tnextpushp;  // pop back to here to clear everything except the result & any early allocation

 I zstatus;  // combined Z: results from this exec of v/u
 // Loop executing u/v
 do{
  A tz;   // will hold result from v while we are deciding whether to keep it
  zstatus=0;  // saved zstatus for this turn through the loop
  ++foldinfo.exestats[0]; foldinfo.zstatus=0;  // incr stats for exec
  // ************* run v
  if(dmfr&STATEFWD+STATEREV){
   if(dmfr&ZZFLAGVIRTAINPLACE)ACRESET(virtw,ACUC1+ACINPLACE);  // in case it was modified, restore inplaceability to the UNINCORPABLE block
   tz=CALL2IP(FAV(vself)->valencefns[1],virtw,vz,vself);  // fwd/rev.  newitem v vz   a is inplaceable if y was (set above).  w is inplaceable first time based on initial-item status
   if(unlikely(tz==virtw)){if(unlikely((tz=clonevirtual(tz))==0))goto exitpop;}
   AK(virtw)+=wstride;  // advance item pointer to next/prev if there is one
   jtinplace=(J)((I)jtinplace|JTINPLACEW);  // w inplaceable on all iterations after the first
  }else if(dmfr&STATEDYAD){tz=CALL2IP(FAV(vself)->valencefns[1],virtw,vz,vself);  // directionless dyad  x v vz
  }else tz=CALL1IP(FAV(vself)->valencefns[0],vz,vself);   // directionless monad   v vz

  zstatus|=foldinfo.zstatus;  // remember termination flags from zstatus
  if(unlikely(zstatus&0b00011))goto errfinish;  // z stopped iteration: finish up
  vz=tz!=0?tz:vz;   //  if v ran to completion, use its result for the next iteration
  if(likely(!(zstatus&0b00100))){  // is 'abort iteration'? (has z=0).  If so, skip u & result store
   if(unlikely(tz==0))goto errfinish;   // error in v, exit
   // ************** run u
   ++foldinfo.exestats[1]; foldinfo.zstatus=0; z=CALL1(FAV(uself)->valencefns[0],tz,uself);  // never inplace
   zstatus|=foldinfo.zstatus;  // remember termination flags from zstatus
   if(unlikely(zstatus&0b00011))goto errfinish;  // z stopped iteration: finish up
   if(likely(!(zstatus&0b00100))){  // is 'abort iteration'? (has z=0)  if so, skip result store
    if(unlikely(z==0))goto errfinish;   // error in u, exit
    if(!(zstatus&0b01000)){  // if store of result not suppressed... */
     // ******************** the u result is to be added to the total (possibly replacing it)
     ++foldinfo.exestats[2]; realizeifvirtual(z); razaptstackend(z);  // we are adding uz to a recursible block, with transfer of ownership using zap if possible.  If uz is abandoned it is safe to zap even if it is x.  Sets new uz
     if(dmfr&STATEMULT){   // is Fold Multiple?
      // Fold Multiple.  Add the new value to the result array
      UI newslot=AN(zz);  // where the new value will go
      if(withprob(newslot==zzalloc,0.03)){  // current alloc full?
       // current alloc is full.  Double the allocation, swap it with zz (transferring ownership), and copy the data
       zzalloc=2*zzalloc+(AKXR(1)>>LGSZI);  // new allocation, cacheline multiple
       A zznew; GATV0E(zznew,INT,zzalloc,1,goto exitpop;) A *zznewzap=AZAPLOC(zznew); A *zzzap=AZAPLOC(zz);  // allocate, & get pointers to tstack slots old & new
       JMC(AAV1(zznew),AAV1(zz),newslot<<LGSZI,0) AT(zz)=INT; AFLAG(zz)=0; *zzzap=zznew; *zznewzap=zz; zz=zznew;  // swap buffers, transferring ownership to zznew & protecting it, neutering zz, setting zz to be freed
       AZAPLOC(zz)=zzzap; AT(zz)=BOX; AFLAG(zz)=BOX&RECURSIBLE;    // new zz now has pointers to allocated blocks and to its dedicated zaploc
      }
      AAV1(zz)[newslot]=z; AN(zz)=newslot+1;  // install the new value & account for it in len
     }else{
      // Fold Single.  Replace the value in zz
      if(AN(zz)!=0){fa(AAV0(zz)[0]);} else{AN(zz)=1;}  // free old value if any, mark value now valid
      AAV0(zz)[0]=z;  // install new value
     }
    }
   }else RESETERR  // 'abort iteration' from u: clear error for next time
  }else RESETERR  // 'abort iteration' from v: clear error for next time
  // ready for next iteration, whether the previous one completed or not
  if(unlikely(zstatus&0b10000))break;  // if early termination, exit loop
  // it is possible that virtw has been passed through to vz.  In that case, we have to copy it
  // because we are about to relocate virtw.  It is OK to keep vz virtual.
  if(unlikely((vz=gc(vz,_old))==0))goto exitpop;  // pop back everything but vz, result & virtuals (removing z at least)
 }while(--nitems);
loopend:;

ASSERTGOTO(AN(zz)!=0,EVNORESULT,exitpop)  // error if we never added to the result
if(dmfr&STATEMULT)AS(zz)[0]=AN(zz); zz=ope(zz);  // set AS(0) right; open zz to give unboxed result
 // if zz is boxed, it might be PRISTINE.  We take the trouble to check, because there's a good chance the user is going to use &.> next
 if(AT(zz)&BOX){DO(AN(zz), A c=C(AAV(zz)[i]); if(!(AT(c)&DIRECT)||AC(c)>ACUC1)goto noprist;) AFLAGORLOCAL(zz,AFPRISTINE); noprist:;}

abortexit:;  // exit, returning whatever is in zz
 jt->afoldinfo=stkfoldinfo;
 RETF(zz);

errfinish:;  // come here when we have to abort the loop
 if(zstatus&0b00001){zz=0; goto abortexit;}  // _3&Z: gives immediate error, so leave the code, which is 'fold limit'
 if(zstatus&0b00010){RESETERR; goto loopend;}  // _2&Z: gives immediate error: clear it. zz must not be 0; return whatever is in it
exitpop:;   // here to abort on error, after popping stack
 zz=0; goto abortexit;  // otherwise, must be error in u or v, which will have been previously signaled - keep the error
}
// u F. F.. F.: F: F:. F:: v
DF2(jtfold){F2PREFIP;
 ASSERTVV(a,w);  // must be verb ops
 A z; fdefallo(z);   // allocate verb result
 I flag = (FAV(w)->flag&(VJTFLGOK1|VJTFLGOK2));  // there is never a need to inplace u.  Inplace v if possible
 fdeffillall(z,0,FAV(self)->id,VERB, jtfold12,jtfold12, a,w,0L, flag, RMAX,RMAX,RMAX,FAV(z)->lu2.lc=FAV(self)->id,);
 R z;
}

// Z: y
DF1(jtfoldZ1){
 ARGCHK1(w)
 ASSERT(jt->afoldinfo,EVSYNTAX);  // If fold not running, fail.  scaf Should be a semantic error rather than syntax
 I type; RE(type=i0(w));  // verify atomic integer
 ASSERT(BETWEENC(type,0,2),EVINDEX)  //  requested stat index must be in range
 RETF(sc(jt->afoldinfo->exestats[type]));  // return the value
}
// x Z: y
DF2(jtfoldZ2){
 ARGCHK2(a,w)
 ASSERT(jt->afoldinfo,EVSYNTAX);  // If fold not running, fail.  scaf Should be a semantic error rather than syntax
 I type; RE(type=i0(a));  // verify atomic integer
 ASSERT(BETWEENC(type,-3,1),EVINDEX)  //  requested action index must be in range
 I y;
 if(type==-3){RE(y=i0(w)); y=jt->afoldinfo->exestats[0]>=y;  // set y if current v count high enough
 }else RE(y=b0(w));  // verify boolean
 if(y){
  I ymask=1<<(type-(-3));  // convert type to one-hot
  jt->afoldinfo->zstatus|=ymask;  // accumulate zstatus
  ASSERT(type!=-3,EVFOLDLIMIT) // if failure, make it 'fold error' (which is signaling)
  if(type<0){jt->jerr=EVFOLDTHROW; jt->emsgstate|=EMSGSTATEFORMATTED; R 0;}   // if abort, set err to cause us to fail up to the Fold; no need to format this since it's not an error
  // types 0 & 1 set zflags but do not abort execution orset error
 }
 RETF(mtv);  // return harmless value
}


#endif

      
