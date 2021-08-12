/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Inner Product                                             */

#include "j.h"
#include "vasm.h"
#include "gemm.h"

#define MAXAROWS 384  // max rows of a that we can process to stay in L2 cache   a strip is m*CACHEHEIGHT, z strip is m*CACHEWIDTH   this is wired to 128*3 - check if you change

// Analysis for inner product
// a,w are arguments
// zt is type of result
// *pm is # 1-cells of a
// *pn is # atoms in an item of w (but if w is a list, it is treated as a pxn column, and n=1)
// *pp is number of inner-product muladds
//   (in each, an atom of a multiplies an item of w)
static A jtipprep(J jt,A a,A w,I zt,I*pm,I*pn,I*pp){A z=mark;I*as,ar,ar1,m,mn,n,p,*ws,wr,wr1;
 ar=AR(a); as=AS(a); ar1=ar-1>=0?ar-1:0; PRODX(m,ar1,as,1) *pm=m;  // m=# 1-cells of a.  It could overflow, if there are no atoms
 wr=AR(w); ws=AS(w); wr1=wr-1>=0?wr-1:0; PRODX(n,wr1,ws+1,1) *pn=n; DPMULDE(m,n,mn);  // n=#atoms in item of w; mn = #atoms in result
 I t=AS(w)[0]; p=wr?t:1; t=AS(a)[ar1]; p=ar?t:p; *pp=p;  // if a is an array, the length of a 1-cell; otherwise, the number of items of w
 ASSERT(!(ar&&wr)||p==ws[0],EVLENGTH);
 GA00(z,zt,mn,ar1+wr1);   // allocate result area
 MCISH(AS(z),as,ar1);  // Set shape: 1-frame of a followed by shape of item of w
 MCISH(AS(z)+ar1,1+ws,wr1);
 R z;
}    /* argument validation & result for an inner product */

#define IINC(x,y)    {b=0>x; x+=y; BOV(b==0>y&&b!=0>x);}
#define DINC(x,y)    {x+=y;}
#define ZINC(x,y)    {(x).re+=(y).re; (x).im+=(y).im;}

#define PDTBY(Tz,Tw,INC)          \
 {Tw*v,*wv;Tz c,*x,zero,*zv;      \
  v=wv=(Tw*)AV(w); zv=(Tz*)AV(z); mvc(sizeof(Tz),&zero,1,MEMSET00);     \
  if(1==n)DQ(m, v=wv; c=zero;           DQ(p,       if(*u++)INC(c,*v);             ++v;);             *zv++=c;)   \
  else    DQ(m, v=wv; mvc(zk,zv,1,MEMSET00); DQ(p, x=zv; if(*u++)DQ(n, INC(*x,*v); ++x; ++v;) else v+=n;); zv+=n;  );  \
 }

#define PDTXB(Tz,Ta,INC,INIT)     \
 {Ta*u;Tz c,*x,zero,*zv;          \
  u=   (Ta*)AV(a); zv=(Tz*)AV(z); mvc(sizeof(Tz),&zero,1,MEMSET00);     \
  if(1==n)DQ(m, v=wv; c=zero;           DQ(p,                   if(*v++)INC(c,*u); ++u;  ); *zv++=c;)   \
  else    DQ(m, v=wv; mvc(zk,zv,1,MEMSET00); DQ(p, x=zv; INIT; DQ(n, if(*v++)INC(*x,c); ++x;);); zv+=n;  );  \
 }

static F2(jtpdtby){A z;B b,*u,*v,*wv;C er=0;I at,m,n,p,t,wt,zk;
 at=AT(a); wt=AT(w); t=at&B01?wt:at;
 RZ(z=ipprep(a,w,t,&m,&n,&p)); zk=n<<bplg(t); u=BAV(a); v=wv=BAV(w);
 NAN0;
 switch(CTTZ(t)){
  default:   ASSERT(0,EVDOMAIN);
  case CMPXX:  if(at&B01)PDTBY(Z,Z,ZINC) else PDTXB(Z,Z,ZINC,c=*u++   ); break;
  case FLX:    if(at&B01)PDTBY(D,D,DINC) else PDTXB(D,D,DINC,c=*u++   ); break;
  case INTX:   if(at&B01)PDTBY(I,I,IINC) else PDTXB(I,I,IINC,c=*u++   ); 
             if(er>=EWOV){
              RZ(z=ipprep(a,w,FL,&m,&n,&p)); zk=n*sizeof(D); u=BAV(a); v=wv=BAV(w);
              if(at&B01)PDTBY(D,I,IINC) else PDTXB(D,I,IINC,c=(D)*u++);
 }}
 NAN1;
 RETF(z);
}    /* x +/ .* y where x or y (but not both) is Boolean */

#define BBLOCK(nn)  \
     d=ti; DQ(nw, *d++=0;);                                           \
     DQ(nn, if(*u++){vi=(UI*)v; d=ti; DQ(nw, *d+++=*vi++;);} v+=n;);  \
     x=zv; c=tc; DQ(n, *x+++=*c++;);

#if (C_AVX || EMU_AVX) && defined(PREFETCH)
// blocked multiply, processing vertical mx16 strips of y.  Good when y has few rows
// *av is mxp, *wv is pxn, *zv is mxn
// flgs is 0 for float, 1 for complex, i. e. lg2(# values per atom), 2 for upper-tri, 4 for INT.  If FLGCMP is set, n and p are even, and give the lengths of the arguments in values
//FLGCMP is not supported yet
//  FLGWMINUSZ is supported
//  FLGAUTRI is supported
// m, n, and p are all non0
I blockedmmult(J jt,D* av,D* wv,D* zv,I m,I n,I pnom,I pstored,I flgs){
 // Since we sometimes use 128-bit instructions in other places, make sure we don't get stuck in slow state
 NAN0;
 _mm256_zeroupperx(VOIDARG) 
 __m256d z00=_mm256_setzero_pd(); // set here to avoid warnings
 // handle small mx2 separately
 // for(each vertical strip of w, processed 4-16 values at a time)
 //  for(each pair of rows of a)
 //   for(each pair of rows of w)
 //    take product of 2x2 of a with 2x16 of w, producing 2x16 result, accumulated
 //   if there is another row of w, accumulate it too
 //   write out 2x16 result
 //  if there is another row of a
 //   for(each row of w)
 //    accumulate 1x16 a*w
 //   write out 1x16 result
 //   if AUTRI, advance a & w start pointers & decr #ps to process
 // for(last partial strip of w)
 //  do it all again, under mask
 // 
#define INITTO0(reg) _mm256_setzero_pd()   // should be _mm256_xor_pd(reg,reg)  but compiler complains
#define LD4EXP(wr,wc,base) _mm256_loadu_pd(base+(wr)*n+(wc)*NPAR)
#define LD4(wr,wc) wt=LD4EXP(wr,wc,wv1);
#define ST1(wr,wc) _mm256_storeu_pd(zv1+(wr)*n+(wc)*NPAR,z##wr##wc);
#define ST2(wc) {ST1(0,wc) ST1(1,wc)}
#define MUL2x4(wr,wc,ldm) {ldm(wr,wc) z0##wc=MUL_ACC(z0##wc,a0,wt); z1##wc=MUL_ACC(z1##wc,a1,wt);}  // (wr,wc) is multiplied by a0,:a1
#define MUL2x16r(nc,ac,ldm) {a0=_mm256_broadcast_sd(&av1[0+(ac)]); a1=_mm256_broadcast_sd(&av1[pstored+(ac)]); MUL2x4(ac,0,ldm) if(nc>1)MUL2x4(ac,1,ldm) if(nc>2)MUL2x4(ac,2,ldm) if(nc>3)MUL2x4(ac,3,ldm)}  // ac is col of a=row of w
#define MUL2x16(nr,nc,ldm) {MUL2x16r(nc,0,ldm) if((nr)>1)MUL2x16r(nc,1,ldm)}
#define MUL1x4(wr,wc,ldm) {ldm(wr,wc) z0##wc=MUL_ACC(z0##wc,a0,wt);}  // (wr,wc) is multiplied by a0
#define MUL1x16(nc,ldm) {a0=_mm256_broadcast_sd(&av1[0]); MUL1x4(0,0,ldm) if((nc)>1)MUL1x4(0,1,ldm) if((nc)>2)MUL1x4(0,2,ldm) if((nc)>3)MUL1x4(0,3,ldm)}  // ac is col of a=row of w
#define WMZ(wr,wc) {z##wr##wc=_mm256_sub_pd(LD4EXP(wr,wc,zv1),z##wr##wc);}
 // Handle the special case where a is small, mx2 m<=4.  We MUST do this because %. expects to be able operate on w inplace when m is 2x2.  We want to do this because
 // it reduces the inner-loop overhead.
 I nrem=n;  // number of columns left
 if(((pnom-2)|(pnom-m))==0){  // m=p=2
  // m is 2x2.  preload it, then read pairs of inputs to produce pairs of outputs.  Must allow inplace ops
  D *wv1=wv, *zv1=zv;  // scan pointer through row-pairs of w and z (which may be the same)
  __m256d z10, a00=_mm256_broadcast_sd(&av[0]), a01=_mm256_broadcast_sd(&av[1]), a10=_mm256_broadcast_sd(&av[2]), a11=_mm256_broadcast_sd(&av[3]);
  while(nrem>NPAR){  // guarantee nonempty remnant
   __m256d w0=_mm256_loadu_pd(wv1), w1=_mm256_loadu_pd(wv1+n);
   z00=_mm256_mul_pd(a00,w0); z00=MUL_ACC(z00,a01,w1); z10=_mm256_mul_pd(a10,w0); z10=MUL_ACC(z10,a11,w1);
   if(flgs&FLGWMINUSZ){WMZ(0,0) WMZ(1,0)}  // handle WMINUSZ, used for next-to-last bit of %.
   ST2(0)  // (over?)write the result
   wv1+=NPAR; zv1+=NPAR; nrem-=NPAR;  // advance to next strip of w/z
  }
  // handle the remnant, which is never empty (to avoid branch misprediction)
  __m256i mask;  // horizontal mask for w values
  mask=_mm256_loadu_si256((__m256i*)(validitymask+3-((nrem-1)&(NPAR-1))));  // nrem { x 1 2 3 4 x x x
  __m256d w0=_mm256_maskload_pd(wv1,mask), w1=_mm256_maskload_pd(wv1+n,mask);
  z00=_mm256_mul_pd(a00,w0); z00=MUL_ACC(z00,a01,w1); z10=_mm256_mul_pd(a10,w0); z10=MUL_ACC(z10,a11,w1);
  if(flgs&FLGWMINUSZ){WMZ(0,0) WMZ(1,0)}  // handle WMINUSZ, used for next-to-last bit of %.
  _mm256_maskstore_pd(zv1,mask,z00);_mm256_maskstore_pd(zv1+n,mask,z10);
  R 0==NANTEST;  // return 0 if floating-point error
 }
 // not 2x2
 I wskips=pnom-NPAR*4; wskips=flgs&FLGWUTRI?wskips:0; wskips=wskips<0?0:wskips;  // number of known trailing 0s in w, therefore shortening each dp
 while(nrem>=NPAR){  // do 1x4s as long as possible.  The load bandwidth is twice as high
  // create mx16 strip of result
  I mrem=m;  // number of rows of a left
  D *av1=av;  // scan pointer through a values, by cols then by rows, i. e. incrementing
  D *zv1=zv;  // output pointer down the column
  D *wvtri=wv;  // pointer to first row to process - advanced if AUTRI
  I ptri=pnom;  // ptri is length of an inner product, which goes down as we advance through upper-tri a
  for(--mrem;mrem>0;mrem-=2){  // bias mrem down 1, so <=0 if no pairs; do each pair
   // create 2x16 section of result
   I prem=ptri-wskips;  // number of cols of a/rows of w to be accumulated into one 2x16 result.  May go negative.
   z00=INITTO0(z00); __m256d z01=z00,  z02=z00, z03=z00, z10=z00, z11=z00, z12=z00, z13=z00;
   D *wv1=wvtri;  // top-left of current strip of w we are processing (skipping over AUTRI areas)
   D *svav1=av1;  // save the offset start of line, since we do not increment to the end if WUTRI
   for(--prem;prem>0;prem-=2){__m256d wt, a0, a1;  // staging nodes for a and w values.  Bias prem down 1, process in pairs
    if(nrem>=4*NPAR)MUL2x16(2,4,LD4) else if(nrem>=3*NPAR)MUL2x16(2,3,LD4) else if(nrem>=2*NPAR)MUL2x16(2,2,LD4) else MUL2x16(2,1,LD4)
    av1+=2; wv1+=2*n;  // advance to next columns of a and rows of w
   }
   if(prem==0){__m256d wt, a0, a1;  // staging nodes for a and w values.  prem is <=0 here, and was biased down 1
    // a has an odd number of columns (and w has an odd number of rows).  Add in the last product
    if(nrem>=4*NPAR)MUL2x16(1,4,LD4) else if(nrem>=3*NPAR)MUL2x16(1,3,LD4) else if(nrem>=2*NPAR)MUL2x16(1,2,LD4) else MUL2x16(1,1,LD4)
   }
   // store the 2x16.  If WMINUSZ, do that first
   if(flgs&FLGWMINUSZ){WMZ(0,0) WMZ(1,0) if(nrem>=2*NPAR){WMZ(0,1) WMZ(1,1)} if(nrem>=3*NPAR){WMZ(0,2) WMZ(1,2)} if(nrem>=4*NPAR){WMZ(0,3) WMZ(1,3)}}
   ST2(0) if(nrem>=2*NPAR)ST2(1) if(nrem>=3*NPAR)ST2(2) if(nrem>=4*NPAR)ST2(3) 
   av1=svav1+2*pstored; zv1+=2*n;  // since we go through two rows of a at a time, we must skip exactly one row at the end
   // if AUTRI, advance the a and w startpoints (by 2 cols and 2 rows respectively) and decrement the number of products in each row
   if(flgs&FLGAUTRI){av1+=2; wvtri+=2*n; ptri-=2;}  // a skips over 2 more values each row
  }

  if(mrem==0){  // mrem is 0 or -1 here.  0 means one more to do.  av1 still has the offset start of the last row of a
   // a has an odd number of rows.  Handle the last one one at a time.  This doesn't have enough accumulators, but it also needs more memory bandwidth and loop
   // overhead, thus runs about half as fast
   I prem=ptri-wskips;  // number of cols of a/rows of w to be accumulated into one 2x16 result.  May be negative
   __m256d z00=INITTO0(z00); __m256d z01=z00, z02=z00, z03=z00;
   D *wv1=wvtri;  // top-left of current strip of w
   while(--prem>=0){__m256d wt, a0;  // staging nodes for a and w values
    if(nrem>=4*NPAR)MUL1x16(4,LD4) else if(nrem>=3*NPAR)MUL1x16(3,LD4) else if(nrem>=2*NPAR)MUL1x16(2,LD4) else MUL1x16(1,LD4)
    av1+=1; wv1+=n;  // advance to next columns of a and rows of w
   };
   // store the 1x16
   if(flgs&FLGWMINUSZ){WMZ(0,0) if(nrem>=2*NPAR){WMZ(0,1)} if(nrem>=3*NPAR){WMZ(0,2)} if(nrem>=4*NPAR){WMZ(0,3)}}
   ST1(0,0); if(nrem>=2*NPAR)ST1(0,1); if(nrem>=3*NPAR)ST1(0,2); if(nrem>=4*NPAR)ST1(0,3);
  }

  wv+=4*NPAR; zv+=4*NPAR; nrem-=4*NPAR;  // advance to next vertical strip of w/z
  wskips-=NPAR*4; wskips=wskips<0?0:wskips;  // for WUTRI, reduce the number of skips as we go
 }

 // We have done all we can do in full NPARs; we are left (possibly) with 1-3 columns of w that must be processed through a
 // Since we have to maskload the w values, there is no way to get full speed on the multiplies.  We use 4 accumulators for latency,
 // but we branch for each one
 // Ignore WUTRI since we are at the full part of w
 if(nrem&(NPAR-1)){  // if there is a remnant of w...
  // Back up wv and zv to the start of the remnant
  wv+=nrem&-NPAR; zv+=nrem&-NPAR;  // restore pointer to the unprocessed data
  // Get the mask for the w columns
  __m256i mask;  // horizontal mask for w values
  mask=_mm256_loadu_si256((__m256i*)(validitymask+4-(nrem&(NPAR-1))));  // a3rem { 4 3 2 1 0 0 0 0

  I mrem=m;  // number of rows of a left
  D *av1=av;  // scan pointer through a values, by cols then by rows, i. e. incrementing
  D *zv1=zv;  // output pointer down the column
  D *wvtri=wv;  // pointer to first row to process - advanced if AUTRI
  I ptri=pnom;  // ptri is length of an inner product, which goes down as we advance through upper-tri a
  I avtri=pstored-pnom;  // number of a values to skip at the start of a line, to get over the zeros
  do{
   // create 1x4 section of result
   I prem=ptri;  // number of cols of a/rows of w to be accumulated into one 2x16 result
   z00=INITTO0(z00); __m256d z01=z00, z02=z00, z03=z00;  // 4 accumulators for latency
   D *wv1=wvtri;  // top-left of current strip of w
   while(1){  // loop with 4 accumulators to create dot-product.  We can run about one product per cycle
    z00=MUL_ACC(z00,_mm256_broadcast_sd(&av1[0]),_mm256_maskload_pd(wv1+0*n,mask)); if(--prem<=0)break;
    z01=MUL_ACC(z01,_mm256_broadcast_sd(&av1[1]),_mm256_maskload_pd(wv1+1*n,mask)); if(--prem<=0)break;
    z02=MUL_ACC(z02,_mm256_broadcast_sd(&av1[2]),_mm256_maskload_pd(wv1+2*n,mask)); if(--prem<=0)break;
    z03=MUL_ACC(z03,_mm256_broadcast_sd(&av1[3]),_mm256_maskload_pd(wv1+3*n,mask));
    av1+=4; wv1+=4*n;  // advance to next columns of a and rows of w
    if(--prem<=0)break;
   };
   // advance a to account for the p values processed before breaking out of the last loop
   av1+=(ptri&3);
   // combine accumulators & store the result
   z00=_mm256_add_pd(_mm256_add_pd(z00,z01),_mm256_add_pd(z02,z03));
   // do WMINUS if called for
   if(flgs&FLGWMINUSZ){z00=_mm256_sub_pd(_mm256_maskload_pd(zv1,mask),z00);}
   _mm256_maskstore_pd(zv1,mask,z00);  // store result
   zv1+=n;  // advance to next row
   if(flgs&FLGAUTRI){avtri+=1; av1+=avtri; wvtri+=n; ptri-=1;}
  }while(--mrem);
 }
 R NANTEST==0;  // return with error (0) if any FP error
}
// cache-blocking code
#define OPHEIGHTX 2
#define OPHEIGHT ((I)1<<OPHEIGHTX)  // height of outer-product block
#define OPWIDTHX 3
#define OPWIDTH ((I)1<<OPWIDTHX)  // width of outer-product block
#define CACHEWIDTH 64  // width of resident cache block (in D atoms)
#define CACHEHEIGHT 16  // height of resident cache block
static D missingrow[CACHEHEIGHT]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
// Floating-point matrix multiply, hived off to a subroutine to get fresh register allocation
// *zv=*av * *wv, with *cv being a cache-aligned region big enough to hold CACHEWIDTH*CACHEHEIGHT floats
// a is shape mxp, w is shape pxn.  Result is 0 if OK, 1 if fatal error
// m must not exceed MAXAROWS.  mfull is the number of rows from the actual starting row to the end of a
// Result is 0 if NaN error, 1 if OK
static I cachedmmultx(J jt,D* av,D* wv,D* zv,I m,I n,I pnom,I pstored,I flgs){D c[(CACHEHEIGHT+1)*CACHEWIDTH + (CACHEHEIGHT+1)*OPHEIGHT*OPWIDTH*2 + 2*CACHELINESIZE/sizeof(D)];  // 2 in case complex
 // Allocate a temporary result area for the stripe of z results
 D zt[((MAXAROWS+OPHEIGHT)&(-OPHEIGHT))*CACHEWIDTH+5*CACHELINESIZE/SZD];
 D *zblock=(D*)(((I)zt+5*CACHELINESIZE-1)&(-CACHELINESIZE));  // cache-aligned area to hold z values
 NAN0;  // see if we hit any errors during this block

 // m is # 1-cells of a
 // n is # values in an item of w (and result)
 // p is number of inner-product muladds (length of a row of a, and # items of w)
 // point to cache-aligned areas we will use for staging the inner-product info
 // flgs is 0 for float, 1 for complex, i. e. lg2(# values per atom), 2 for upper-tri, 4 for INT.  If FLGCMP is set, n and p are even, and give the lengths of the arguments in values
 // Since we sometimes use 128-bit instructions in other places, make sure we don't get stuck in slow state
_mm256_zeroupperx(VOIDARG) 
 D *cvw = (D*)(((I)&c+(CACHELINESIZE-1))&-CACHELINESIZE);  // place where cache-blocks of w are staged
 D (*cva)[2][OPHEIGHT][CACHEHEIGHT] = (D (*)[2][OPHEIGHT][CACHEHEIGHT])(((I)cvw+(CACHEHEIGHT+1)*CACHEWIDTH*sizeof(D)+(CACHELINESIZE-1))&-CACHELINESIZE);   // place where expanded rows of a are staged
 // If a is upper-triangular, we write out the entire column of z values only when we process the last section of the w stripe.  If w is also upper-triangular,
 // we stop processing sections before we get to the bottom.  So in that case (which never happens currently), clear the entire result areas leaving 0 in the untouched bits
 if((flgs&(FLGWUTRI|FLGAUTRI))==(FLGWUTRI|FLGAUTRI))mvc(m*n*SZD,zv,1,MEMSET00);  // if w is upper-triangular, we will not visit all the z values and we must clear the lower-triangular part.  Here we just clear them all
 // process each 64-float vertical stripe of w against the entirety of a, producing the corresponding columns of z
 D* w0base = wv; D* z0base = zv; I w0rem = n;   // w0rem counts doubles
 for(;w0rem>0;w0rem-=CACHEWIDTH,w0base+=CACHEWIDTH,z0base+=CACHEWIDTH){
  // process each 16x64 section of w, adding each result to the same columns of z.  Each section goes through one corresponding set of 16/32 columns of a.  First time through init z values to 0
  D* a1base=av; D* w1base=w0base; D* z1base=z0base; I w1rem=pnom>>(flgs&FLGCMP); flgs|=FLGZFIRST;  // w1rem counts atoms
  // if w is upper-triangular, there is no need to process sections whose upper index exceeds the rightmost index; that is, limit w1rem based on w0rem
  if(flgs&FLGWUTRI){I bottomlen=(w0rem-CACHEWIDTH)>>(flgs&FLGCMP); bottomlen=bottomlen>=0?bottomlen:0; w1rem-=bottomlen;}
  I preva2rem=0;   // number of rows of a processed in previous pass (used to establish newrowsct1)
  I newrowsct1=0;   // the number of new exposed rows of upper-triangular a (they need initializing), plus 1
  for(;w1rem>0;w1rem-=CACHEHEIGHT,a1base+=CACHEHEIGHT<<(flgs&FLGCMP),w1base+=CACHEHEIGHT*n){D* RESTRICT cvx;D* w1next=w1base;I i;
   // if this is the last section for this stripe, set a flag to tell us the results of this section must go to the final result area
   flgs|=w1rem<=CACHEHEIGHT?FLGZLAST:0;
   // z1base tracks the output position in the result.  zilblock goes sequentially through the zblock, in an order determined by the inner loop.  Its value is opaque outside the inner loop.
   D *zilblock=zblock;  // running pointer in the cache-aligned z area.  Each section accumulates into these values
   // read the 16x64 section of w into the cache area (8KB, 2 ways of cache), with prefetch of rows
   for(i=MIN(CACHEHEIGHT,w1rem),cvx=cvw;i;--i){I j;
    D* RESTRICT w1x=w1next; w1next+=n;  // save start of current input row, point to next row...
    j=0;
    for(j=0;j<(MIN(CACHEWIDTH,w0rem)&-NPAR);j+=NPAR){_mm256_store_pd(cvx,_mm256_loadu_pd(w1x)); cvx+=NPAR; w1x+=NPAR;}
    for(;j<MIN(CACHEWIDTH,w0rem);++j){D fv = *w1x; *cvx++=fv; w1x++;}  // move the valid data
    for(;j<CACHEWIDTH;++j)*cvx++=0.0;   // fill the rest with 0
   }
   // w1next is left pointing to the next cache block in the column.  We will use that to prefetch
   D *nextprefetch=w1next;  // start prefetches for the next block at the beginning

   // the single mx16 vertical strip of a (mx32 doubles if complex) will be multiplied by the 16x64 section of w and accumulated into the mx64 slice of z
   // process each 4x16 (or 32) section of a against the 16x64 cache block
   D *a2base0=a1base; D* w2base=w1base; I a2rem=m; D* z2base=z1base; D* c2base=cvw;
   // if a is upper-triangular, we can stop when the top index of a exceeds the bottommost index of w.
   if(flgs&FLGAUTRI){
    // see where the validity of a expires (at a distance, based on w position, from the actual bottom of the full a); clamp #rows to process; see how many are new rows that must be initialized
    I fullrem=(pnom>>(flgs&FLGCMP))-(w1rem-CACHEHEIGHT); fullrem=fullrem<0?0:fullrem; a2rem=a2rem>fullrem?fullrem:a2rem; newrowsct1=a2rem-preva2rem+1; preva2rem=a2rem;
    // This won't work if WUTRI is also set - then w1rem doesn't count from the top of the region to the bottom, but only down to the diagonal - you would need to bring w0rem into the mix
   }
   // scaf could skip leading 16x8s for uppertri w, if that doesn't foul z
   for(;a2rem>0;a2rem-=OPHEIGHT,a2base0+=OPHEIGHT*pstored,z2base+=OPHEIGHT*n){  // a2rem is the number of lines left in the entire column of a
    // Prepare for the 4x16 block of a (4x32 if cmplx)
    // If a row of a is off the end of the data, we mustn't fetch it - repeat a row instead so it won't give NaN error on multiplying by infinity
    // The result is in (*cvx)[component][row][col]  where col is the column number in a 0-15, row is the row 0-3,
    // component is 0 except 1 for imaginary part of complex a.  Complex inputs have to be twice as big as real ones
    I nvalidops=MIN(CACHEHEIGHT,w1rem);  // number of outer products before we run out of block or input
    // If the entire block of real a is present, move it without any looping overhead
   if((((CACHEHEIGHT-1)-nvalidops)&((OPHEIGHT-1)-a2rem)&((flgs&FLGCMP)-1))<0){  // full block and not complex
    // load in horizontal order for best prefetch; store aligned in column order which is order of use
    __m256d t0,t1,t2,t3,t4,t5,t6,t7;
    t0=_mm256_loadu_pd(a2base0+0*pstored+0*NPAR); t1=_mm256_loadu_pd(a2base0+0*pstored+1*NPAR); _mm256_store_pd(&(*cva)[0][0][0*NPAR],t0);  _mm256_store_pd(&(*cva)[0][0][1*NPAR],t1); t2=_mm256_loadu_pd(a2base0+0*pstored+2*NPAR); t3=_mm256_loadu_pd(a2base0+0*pstored+3*NPAR);
    t0=_mm256_loadu_pd(a2base0+1*pstored+0*NPAR); t1=_mm256_loadu_pd(a2base0+1*pstored+1*NPAR); _mm256_store_pd(&(*cva)[0][1][0*NPAR],t0);  _mm256_store_pd(&(*cva)[0][1][1*NPAR],t1); t4=_mm256_loadu_pd(a2base0+1*pstored+2*NPAR); t5=_mm256_loadu_pd(a2base0+1*pstored+3*NPAR);
    t0=_mm256_loadu_pd(a2base0+2*pstored+0*NPAR); t1=_mm256_loadu_pd(a2base0+2*pstored+1*NPAR); _mm256_store_pd(&(*cva)[0][2][0*NPAR],t0);  _mm256_store_pd(&(*cva)[0][2][1*NPAR],t1); t6=_mm256_loadu_pd(a2base0+2*pstored+2*NPAR); t7=_mm256_loadu_pd(a2base0+2*pstored+3*NPAR);
    t0=_mm256_loadu_pd(a2base0+3*pstored+0*NPAR); t1=_mm256_loadu_pd(a2base0+3*pstored+1*NPAR); _mm256_store_pd(&(*cva)[0][3][0*NPAR],t0);  _mm256_store_pd(&(*cva)[0][3][1*NPAR],t1); t0=_mm256_loadu_pd(a2base0+3*pstored+2*NPAR); t1=_mm256_loadu_pd(a2base0+3*pstored+3*NPAR);
    _mm256_store_pd(&(*cva)[0][0][2*NPAR],t2);  _mm256_store_pd(&(*cva)[0][0][3*NPAR],t3); _mm256_store_pd(&(*cva)[0][1][2*NPAR],t4);  _mm256_store_pd(&(*cva)[0][1][3*NPAR],t5);
    _mm256_store_pd(&(*cva)[0][2][2*NPAR],t6);  _mm256_store_pd(&(*cva)[0][2][3*NPAR],t7); _mm256_store_pd(&(*cva)[0][3][2*NPAR],t0);  _mm256_store_pd(&(*cva)[0][3][3*NPAR],t1);
   }else{  // partial block
    for(i=0;i<OPHEIGHT;++i){I j;  // for each row (i. e. for the length of the outer product)
      D *a0x=a2base0+pstored*i; a0x=i>=a2rem?missingrow:a0x;  // start of samples for the row, or a repeated row if past the end
      if(!(flgs&(FLGCMP))){
       for(j=0;j<(nvalidops&-NPAR);j+=NPAR){_mm256_store_pd(&(*cva)[0][i][j],_mm256_loadu_pd(a0x)); a0x+=NPAR;}  // quads as long as possible
       for(;j<nvalidops;++j){(*cva)[0][i][j]=*a0x; ++a0x;}  // float
      }else {for(j=0;j<nvalidops;++j){(*cva)[0][i][j]=*a0x; ++a0x; (*cva)[1][i][j]=*a0x; ++a0x;}}  // complex: real and imaginary
     }
    }

    // While we are processing the sections of a, move the next cache block of w into L2 (not L1, so we don't overrun it)
    // We would like to do all the prefetches for a CACHEWIDTH at once to stay in page mode
    // but that might overrun the prefetch queue, which holds 10 prefetches.
    // The length of a cache row is (CACHEWIDTH*sizeof(D))/CACHELINESIZE=8 cache lines, plus one in case the data is misaligned.
    // We start the prefetches when we get to within 3*CACHEHEIGHT iterations from the end, which gives us 3 iterations
    // to fetch each row of the cache, 3 fetches per iteration.
#if 1  // doesn't help much, but cheap insurance
    if(a2rem<=3*OPHEIGHT*CACHEHEIGHT+(OPHEIGHT-1)){
     PREFETCH2((C*)nextprefetch); PREFETCH2((C*)nextprefetch+CACHELINESIZE); PREFETCH2((C*)nextprefetch+2*CACHELINESIZE);  // 3 prefetches
     if(nextprefetch==(D*)((C*)w1next+6*CACHELINESIZE)){nextprefetch = w1next += n;}else{nextprefetch+=(3*CACHELINESIZE)/sizeof(*nextprefetch);}  // next col, or next row after 9 prefetches
    }
#endif

    // A little tweak for upper-triangular a.  The problem is that processing of the column of a stops when it falls below the diagonal, which is at a different point
    // for each section of w.  This means that the z values get initialized only as far as the a values were read, and each section encounters a garbage section at the end.
    // To fix this we turn on ZFIRST when we are processing the LAST new rows of upper-triangular a.  The new rows are any after the previous section.  This works because (1) each succeeding section of w exposes exactly
    // CACHEHEIGHT new nonzero columns of a whose corresponding rows must be processed; (2) the exposed sections of a are on CACHEHEIGHT boundaries until the last remnant.
    // We are guaranteed to initialize each bit of the z stripe exactly once.
    flgs|=REPSGN(a2rem-newrowsct1)&FLGZFIRST;

    // process each 16x8 section of w, accumulating into z (this holds 16x4 complex values, if FLGCMP)
    I a3rem=MIN(w0rem,CACHEWIDTH);
    D* RESTRICT z3base=z2base; D* c3base=c2base;
    for(;a3rem>0;a3rem-=OPWIDTH,c3base+=OPWIDTH){  // a3rem is the number of row/col products left in this cache block (rows of a, cols of w)

     // process outer product of each 4x1 section on each 1x8 section of cache

     // Now do the 16 outer products for the block, each 4ax8w (or 4ax4w if FLGCMP)
     D (*a4base0)[2][OPHEIGHT][CACHEHEIGHT]=cva;   // Can't put RESTRICT on this - the loop to init *cva gets optimized away
     if(!(flgs&FLGCMP)){
      I a4rem=nvalidops;
      D * RESTRICT c4base=c3base;
      // initialize accumulator with the z values accumulated so far.
      // We guarantee that we do not add to or store a row that does not exist in a, so we don't have to initialize them
      // We have to use masked load at the edges of the array, to make sure we don't fetch from undefined memory.  Fill anything not loaded with 0
      __m256d z00,z01,z10,z11,z20,z21,z30,z31;   // (float) zij has the total for row i col j  (complex) ?

      // load running total, or 0 if first time
      /*if((a3rem|a3rem-1)<1)scaf to disable*/if(!(flgs&FLGZFIRST)){
#define ACCZ(r,c) z##r##c=_mm256_load_pd(zilblock+NPAR*(2*r+c));
        ACCZ(0,0); ACCZ(0,1); ACCZ(1,0); ACCZ(1,1); ACCZ(2,0); ACCZ(2,1); ACCZ(3,0); ACCZ(3,1);
      }else z31 = z30 = z21 = z20 = z11 = z10 = z01 = z00 = _mm256_setzero_pd();

// we might want to prefetch a anyway in case a row is a multiple of a cache line
#if 0   // needed if a gets bigger than L2.  Since we don't allow m to exceed 512, a cannot exceed m*CACHEHEIGHT Ds which fits in L2.  z similarly
      // Prefetch the next lines of a into L2 cache.  We don't prefetch all the way to L1 because that might overfill L1,
      // if all the prefetches hit the same cache line (we already have 2 full banks for our cache area, plus the current z values)
      // The a area to fetch is OPHEIGHTxCACHEHEIGHT: the next section of a.  The number of PREFETCH instructions needed for a is OPHEIGHT*(CACHEHEIGHT*sizeof(D)/CACHELINESIZE)+1=4*3; for complex
      // this is 4*5.
      // These prefetches must be spread across the CACHEWIDTH/OPWIDTH times we come through this loop.
      // For the current sizes, that means one set of prefetches every time
      // We defer the prefetches till here to fill the time between the fetch of z above and the main multiply
      // We don't prefetch z because it is sequentially accessed in zilblock and we will prefetch there if we need it
      if(a3rem&OPWIDTH){
       I lineno = ((-a3rem)>>(OPWIDTHX+1))&((CACHEWIDTH>>(OPWIDTHX+1))-1);  // iteration number, 0-3
       D *base = a2base0 + (lineno+2*OPHEIGHT)*p;  // address to prefetch from - >=1 full OPHEIGHT block after the current base pointers
       PREFETCH((C*)base); PREFETCH((C*)base+CACHELINESIZE); PREFETCH((C*)base+2*CACHELINESIZE);
      }
#endif
      // The inner loop.  If everything is inbounds do the whole thing without loops
      if(a4rem==CACHEHEIGHT&&a2rem>3){
        __m256d wval0, wval1, aval;
#define LDW(opno)  wval0=_mm256_load_pd(&c4base[opno*CACHEWIDTH+0]); wval1=_mm256_load_pd(&c4base[opno*CACHEWIDTH+NPAR]);  // opno=outer-product number
#define ONEP(opno,opx) aval=_mm256_broadcast_sd(&(*a4base0)[0][opx][opno]); z##opx##0 = MUL_ACC(z##opx##0,wval0,aval); z##opx##1 = MUL_ACC(z##opx##1,wval1,aval);  // opx=a row number=mul# within outer product  could allow compiler to gather commons
#define OUTERP(opno) LDW(opno) ONEP(opno,0) ONEP(opno,1) ONEP(opno,2) ONEP(opno,3)
       OUTERP(0) OUTERP(1) OUTERP(2) OUTERP(3) OUTERP(4) OUTERP(5) OUTERP(6) OUTERP(7) OUTERP(8) OUTERP(9) OUTERP(10) OUTERP(11) OUTERP(12) OUTERP(13) OUTERP(14) OUTERP(15)
// prefetch doesn't seem to help

      }else{
       // variable version
       do{
        __m256d wval0=_mm256_load_pd(&c4base[0]), wval1=_mm256_load_pd(&c4base[NPAR]);
#define COL0(row) {z##row##0 = MUL_ACC(z##row##0,wval0,_mm256_broadcast_sd(&(*a4base0)[0][row][0])); z##row##1 = MUL_ACC(z##row##1,wval1,_mm256_broadcast_sd(&(*a4base0)[0][row][0]));}
        COL0(0); if(a2rem>1)COL0(1) if(a2rem>1)COL0(2) if(a2rem>3)COL0(3)
        a4base0=(D (*)[2][OPHEIGHT][CACHEHEIGHT])((D*)a4base0+1);  // advance to next col
        c4base+=CACHEWIDTH;  // advance to next row
       }while(--a4rem>0);
      }

// for some reason, storing into z slows down processing by a factor of 2 when m exceeds 500 and n,p are 16 64 - even if we don't load
// it appears that this is because the Z strip exceeds L2 cache and the bandwidth of L# is not enough to support the traffic.  So we
// ask the caller to split a up in units of 500 rows or less.  This might obviate the need for the Z buffer, but we keep it because
// it allows the result to be inplaced if a doesn't have to be split, and might use cache bandwidth better since it's aligned
      if(!(flgs&FLGZLAST)){
       _mm256_store_pd(zilblock,z00); _mm256_store_pd(zilblock+NPAR,z01); _mm256_store_pd(zilblock+2*NPAR,z10); _mm256_store_pd(zilblock+3*NPAR,z11);
       _mm256_store_pd(zilblock+4*NPAR,z20); _mm256_store_pd(zilblock+5*NPAR,z21); _mm256_store_pd(zilblock+6*NPAR,z30); _mm256_store_pd(zilblock+7*NPAR,z31);
      }else {
        // for the last z pass we have to store into the final result area
       if(a3rem>=OPWIDTH){  // no problem horizontally
        _mm256_storeu_pd(z3base,z00); _mm256_storeu_pd(z3base+NPAR,z01);
        if(a2rem>1){_mm256_storeu_pd(z3base+n,z10); _mm256_storeu_pd(z3base+n+NPAR,z11);}
        if(a2rem>2){_mm256_storeu_pd(z3base+2*n,z20); _mm256_storeu_pd(z3base+2*n+NPAR,z21);}
        if(a2rem>3){_mm256_storeu_pd(z3base+3*n,z30); _mm256_storeu_pd(z3base+3*n+NPAR,z31);}
       } else {
        __m256i mask0, mask1;  // horizontal masks for w values, if needed
        I nvalids=0x048cdef0>>(a3rem<<2);  // 4 bits: f1f0 l1l0 where f10 is the offset to use for first 4 values, l10 if offset-1 for last 4.  Offset0=4 words, 1=3 words, 2-2 words, 3=1 word.  Can't have 0 words for f, can for l
        mask0=_mm256_loadu_si256((__m256i*)(validitymask+(nvalids&0x3)));  // a3rem { 4 3 2 1 0 0 0 0
        mask1=_mm256_loadu_si256((__m256i*)(validitymask+1+((nvalids>>2)&0x3)));  // a3rem { 4 4 4 4 4 3 2 1
        _mm256_maskstore_pd(z3base,mask0,z00); _mm256_maskstore_pd(z3base+NPAR,mask1,z01);
        if(a2rem>1){_mm256_maskstore_pd(z3base+n,mask0,z10); _mm256_maskstore_pd(z3base+n+NPAR,mask1,z11);}
        if(a2rem>2){_mm256_maskstore_pd(z3base+2*n,mask0,z20); _mm256_maskstore_pd(z3base+2*n+NPAR,mask1,z21);}
        if(a2rem>3){_mm256_maskstore_pd(z3base+3*n,mask0,z30); _mm256_maskstore_pd(z3base+3*n+NPAR,mask1,z31);}
       }
       z3base+=OPWIDTH;  // This is normal loop housekeeping, but needed only on the last pass when z3base is used.  We save the cycle
      }
      zilblock+=OPHEIGHT*OPWIDTH;  // step over the zs to the next sequential input position
     
     }else{
      // CMPX case.  Do the multiply.
      I a5rem=MIN(a2rem,4);  // number of valid rows in a for the current pass.  if only 1 we mustn't do the bottom sample
      // Two passes, since a is wide.
      D * RESTRICT z4base=z3base;
      do{
       I a4rem=nvalidops;  // number of outer products
       D * RESTRICT c4base=c3base;  // pointer to top of w block
       // First get the 
       // initialize accumulator with the z values accumulated so far.
       __m256d z00r,z00i,z01r,z01i,z10r,z10i,z11r,z11i;   // (complex) zijr has row i real x col j; ziji has row i imag x col j.  Result goes into the rs
       z00i = z01i = z10i = z11i = _mm256_setzero_pd();   // values are accumulated & stored in r vbls
       if(flgs&FLGZFIRST){z00r = z01r = z10r = z11r = _mm256_setzero_pd();
       }else{
        z00r=_mm256_load_pd(zilblock); z01r=_mm256_load_pd(zilblock+NPAR); z10r=_mm256_load_pd(zilblock+2*NPAR); z11r=_mm256_load_pd(zilblock+3*NPAR);  // scaf move these loads earlier
       }

       // do all the columns of a, but only 2 rows.  This is half the outer product
       do{
        __m256d wval0=_mm256_loadu_pd(&c4base[0]), wval1=_mm256_loadu_pd(&c4base[NPAR]), aval;
        aval=_mm256_broadcast_sd(&(*a4base0)[0][0][0]); z00r = MUL_ACC(z00r,wval0,aval); z01r = MUL_ACC(z01r,wval1,aval);  // rrrr * riri => riri
        aval=_mm256_broadcast_sd(&(*a4base0)[1][0][0]); z00i = MUL_ACC(z00i,wval0,aval); z01i = MUL_ACC(z01i,wval1,aval);   // iiii * riri => iRiR
        if(a5rem>1){
         aval=_mm256_broadcast_sd(&(*a4base0)[0][1][0]); z10r = MUL_ACC(z10r,wval0,aval); z11r = MUL_ACC(z11r,wval1,aval);  // rrrr * riri => riri
         aval=_mm256_broadcast_sd(&(*a4base0)[1][1][0]); z10i = MUL_ACC(z10i,wval0,aval); z11i = MUL_ACC(z11i,wval1,aval);   // iiii * riri => iRiR
        }
        a4base0=(D (*)[2][OPHEIGHT][CACHEHEIGHT])((D*)a4base0+1);  // advance to next col
        c4base+=CACHEWIDTH;  // advance to next row
       }while(--a4rem>0);

       // combine real & imaginary parts
       z00r=_mm256_addsub_pd(z00r,_mm256_permute_pd(z00i, 0x5));
       z01r=_mm256_addsub_pd(z01r,_mm256_permute_pd(z01i, 0x5));
       if(a5rem>1){
        z10r=_mm256_addsub_pd(z10r,_mm256_permute_pd(z10i, 0x5));
        z11r=_mm256_addsub_pd(z11r,_mm256_permute_pd(z11i, 0x5));
       }
       // write em out
       if(!(flgs&FLGZLAST)){
        _mm256_store_pd(zilblock,z00r); _mm256_store_pd(zilblock+NPAR,z01r); _mm256_store_pd(zilblock+2*NPAR,z10r); _mm256_store_pd(zilblock+3*NPAR,z11r);
       }else{
         // for the last z pass we have to store into the final result area
        if(a3rem>7){  // no problem horizontally
         _mm256_storeu_pd(z4base,z00r); _mm256_storeu_pd(z4base+NPAR,z01r);
         if(a5rem>1){_mm256_storeu_pd(z4base+n,z10r); _mm256_storeu_pd(z4base+n+NPAR,z11r);}
        } else {
         __m256i mask0, mask1;  // horizontal masks for w values, if needed
         mask0=_mm256_loadu_si256((__m256i*)(validitymask+((0x00001234>>(a3rem<<2))&0x7)));  // a3rem { 4 3 2 1 0 0 0 0
         mask1=_mm256_loadu_si256((__m256i*)(validitymask+((0x12344444>>(a3rem<<2))&0x7)));  // a3rem { 4 4 4 4 4 3 2 1
         _mm256_maskstore_pd(z4base,mask0,z00r); _mm256_maskstore_pd(z4base+NPAR,mask1,z01r);
         if(a5rem>1){_mm256_maskstore_pd(z4base+n,mask0,z10r); _mm256_maskstore_pd(z4base+n+NPAR,mask1,z11r);}
        }
       }
       zilblock+=OPHEIGHT*OPWIDTH/2;  // step over the zs to the next sequential position

       // For the second pass shift over to start on row 2 of a
// VS2013 doesn't work       a4base0=(__m256d (*)[2][OPHEIGHT][CACHEHEIGHT])&cva[0][2][0];
       a4base0=(D (*)[2][OPHEIGHT][CACHEHEIGHT])((D*)(cva)+2*CACHEHEIGHT);
       z4base+=2*n;  // advance output pointer to second linepair
      }while((a5rem-=2)>0);
      z3base+=OPWIDTH;  // This is normal loop housekeeping, but needed only on the last pass when z3base is used.  We save the cycle for the FL code
     }
    }
   }
   flgs&=~(FLGZFIRST|FLGZLAST);  // we have finished a 16x64 cache section.  That touched all the columns of z.  For the remaining sections we must accumulate into the z values.  If this was the last pass, clear that flag too, since we're finished
  }  // end of loop for each 16x64 section of w
 }  // end of loop for each 64-col slice of w
 R NANTEST==0;  // return with error (0) if any FP error
}
// looping entry point for cached mmul
// We split the input into products where the left arg has at most MAXAROWS rows.  This is to avoid overrunning L2 cache
// Result is 0 if error, which must be NaN error
// For historical reason (i. e. to match the non-AVX2 version) n and p have been multiplied by 2 for complex multiplies
I cachedmmult(J jt,D* av,D* wv,D* zv,I m,I n,I p,I flgs){
// TODO: bug when EMU_AVX
 int rc=1,i;
 I blocksize,nblocks,(*fn)();  // loop controls
 if(((((50-m)&(50-n)&(16-p)&(DCACHED_THRES-m*n*p))|SGNIF(flgs,FLGCMPX))&SGNIFNOT(flgs,FLGWMINUSZX))>=0){  // blocked for small arrays in either dimension (after threading); not if CMP; force if WMINUSZ (can't be both)
  // blocked algorithm.  there is no size limit on the blocks
  fn=blockedmmult;  // select function
  nblocks=1; blocksize=m; // do it all in a single block
 }else{
  // cached algorithm.  blocks must not exceed MAXAROWS lines
  fn=cachedmmultx;  // select function
  // Figure out the number of blocks we will use, and the size of each.  We make the number of blocks a multiple of the number of threads, and round the
  // block size up to a multiple of OPHEIGHT
  nblocks = ((m+MAXAROWS)*0x55555555)>>(32+7);  // minimum number of blocks needed
  blocksize=MAXAROWS;   // max size of each
 }
 for(i=0;i<nblocks;++i){
  // if AUTRI, bring a in from the left and w down from the top as we proceed.  And shorten p.
  if(0==(*fn)(jt,
              av+(i*MAXAROWS*(p+(((flgs>>FLGAUTRIX)&1)<<(flgs&FLGCMP)))),
              wv+((n*i*MAXAROWS)&-((flgs>>FLGAUTRIX)&1)),
              zv+(n*i*MAXAROWS),
              MIN(blocksize,m-i*MAXAROWS),
              n,
              p-(((i*MAXAROWS)&-((flgs>>FLGAUTRIX)&1))<<(flgs&FLGCMP)),
              p,flgs)){rc=0; break;}  // set error if one was found
 }
 R rc;
}

#else
// cache-blocking code
#define OPHEIGHT 2  // height of outer-product block
#define OPWIDTH 4  // width of outer-product block
#define CACHEWIDTH 64  // width of resident cache block (in D atoms)
#define CACHEHEIGHT 16  // height of resident cache block
static D missingrow[CACHEHEIGHT]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
// Floating-point matrix multiply, hived off to a subroutine to get fresh register allocation
// *zv=*av * *wv, with *cv being a cache-aligned region big enough to hold CACHEWIDTH*CACHEHEIGHT floats
// a is shape mxp, w is shape pxn.  Result is 0 if OK, 1 if overflow
I cachedmmult(J jt,D* av,D* wv,D* zv,I m,I n,I p,I flgs){D c[(CACHEHEIGHT+1)*CACHEWIDTH + (CACHEHEIGHT+1)*OPHEIGHT*OPWIDTH*2 + 2*CACHELINESIZE/sizeof(D)];  // 2 in case complex
 // m is # 1-cells of a
 // n is # values in an item of w (and result)
 // p is number of inner-product muladds (length of a row of a, and # items of w)
 // point to cache-aligned areas we will use for staging the inner-product info
 // flgs is 0 for float, 1 for complex, i. e. lg2(# values per atom).  If flgs is set, n and p are even, and give the lengths of the arguments in values
 D *cvw = (D*)(((I)&c+(CACHELINESIZE-1))&-CACHELINESIZE);  // place where cache-blocks of w are staged
 D *cva = (D*)(((I)cvw+(CACHEHEIGHT+1)*CACHEWIDTH*sizeof(D)+(CACHELINESIZE-1))&-CACHELINESIZE);   // place where expanded rows of a are staged
 // zero the result area
 mvc(m*n*sizeof(D),zv,1,MEMSET00);
 // process each 64-float vertical stripe of w, producing the corresponding columns of z
 D* w0base = wv; D* z0base = zv; I w0rem = n;
 for(;w0rem>0;w0rem-=CACHEWIDTH,w0base+=CACHEWIDTH,z0base+=CACHEWIDTH){
  // process each 16x64 section of w, adding each result to the columns of z.  Each section goes through a different set of 16/32 columns of a 
  D* a1base=av; D* w1base=w0base; D* z1base=z0base; I w1rem=p>>(flgs&FLGCMP);
  for(;w1rem>0;w1rem-=CACHEHEIGHT,a1base+=CACHEHEIGHT<<(flgs&FLGCMP),w1base+=CACHEHEIGHT*n){D* RESTRICT cvx;D* w1next=w1base;I i;
   // read the 16x64 section of w into the cache area (8KB, 2 ways of cache), with prefetch of rows
   for(i=MIN(CACHEHEIGHT,w1rem),cvx=cvw;i;--i){I j;
    D* RESTRICT w1x=w1next; w1next+=n;  // save start of current input row, point to next row...
    // I don't think it's worth the trouble to move the data with AVX instructions - though it was to prefetch it
    for(j=0;j<MIN(CACHEWIDTH,w0rem);++j){D fv = *w1x; if(flgs&FLGINT)fv=(D)*(I*)w1x; *cvx++=fv; w1x++;}  // move the valid data
    for(;j<CACHEWIDTH;++j)*cvx++=0.0;   // fill the rest with 0
   }
   // Because of loop unrolling, we fetch and multiply one extra value in each cache column.  We make sure those values are 0 to avoid NaN errors
   for(i=0;i<MIN(CACHEWIDTH,w0rem);++i)*cvx++=0.0;

   // w1next is left pointing to the next cache block in the column.  We will use that to prefetch
#ifdef PREFETCH
   D *nextprefetch=w1next;  // start prefetches for the next block at the beginning
#endif

   // the mx16 vertical strip of a (mx32 if flgs) will be multiplied by the 16x64 section of w and accumulated into z
   // process each 2x16 section of a against the 16x64 cache block
   D *a2base0=a1base; D* w2base=w1base; I a2rem=m; D* z2base=z1base; D* c2base=cvw;
   for(;a2rem>0;a2rem-=OPHEIGHT,a2base0+=OPHEIGHT*p,z2base+=OPHEIGHT*n){
    // Prepare for the 2x16 block of a (2x32 if flgs)
    // If the second row of a is off the end of the data, we mustn't fetch it - switch the pointer to a row of 1s so it won't give NaN error on multiplying by infinity
    D *a2base1 = (a2rem>1)?a2base0+p:missingrow;
#ifdef PREFETCH
    // While we are processing the sections of a, move the next cache block into L2 (not L1, so we don't overrun it)
    // We would like to do all the prefetches for a CACHEWIDTH at once to stay in page mode
    // but that might overrun the prefetch queue, which holds 10 prefetches.
    // The length of a cache row is (CACHEWIDTH*sizeof(D))/CACHELINESIZE=8 cache lines, plus one in case the data is misaligned.
    // We start the prefetches when we get to within 3*CACHEHEIGHT iterations from the end, which gives us 3 iterations
    // to fetch each row of the cache, 3 fetches per iteration.
    if(a2rem<=3*OPHEIGHT*CACHEHEIGHT+(OPHEIGHT-1)){
     PREFETCH2((C*)nextprefetch); PREFETCH2((C*)nextprefetch+CACHELINESIZE); PREFETCH2((C*)nextprefetch+2*CACHELINESIZE);  // 3 prefetches
     if(nextprefetch==(D*)((C*)w1next+6*CACHELINESIZE)){nextprefetch = w1next += n;}else{nextprefetch+=(3*CACHELINESIZE)/sizeof(*nextprefetch);}  // next col, or next row after 9 prefetches
    }
#endif
    // process each 16x4 section of cache, accumulating into z (this holds 16x2 complex values, if flgs)
    I a3rem=MIN(w0rem,CACHEWIDTH);
    D* RESTRICT z3base=z2base; D* c3base=c2base;
    for(;a3rem>0;a3rem-=OPWIDTH,c3base+=OPWIDTH,z3base+=OPWIDTH){
     // initialize accumulator with the z values accumulated so far.
     D z00,z01,z02,z03,z10,z11,z12,z13;
     z00=z3base[0];
     if(a3rem>3){z01=z3base[1],z02=z3base[2],z03=z3base[3]; if(a2rem>1)z10=z3base[n],z11=z3base[n+1],z12=z3base[n+2],z13=z3base[n+3];
     }else{if(a3rem>1){z01=z3base[1];if(a3rem>2)z02=z3base[2];}; if(a2rem>1){z10=z3base[n];if(a3rem>1)z11=z3base[n+1];if(a3rem>2)z12=z3base[n+2];}}
     // process outer product of each 2x1 section on each 1x4 section of cache

     // Prefetch the next lines of a and z into L2 cache.  We don't prefetch all the way to L1 because that might overfill L1,
     // if all the prefetches hit the same line (we already have 2 lines for our cache area, plus the current z values)
     // The number of prefetches we need per line is (CACHEHEIGHT*sizeof(D)/CACHELINESIZE)+1, and we need that for
     // OPHEIGHT*(OPWIDTH/4) lines for each of a and z.  We squeeze off half the prefetches for a row at once so we can use fast page mode
     // to read the data (only half to avoid overfilling the prefetch buffer), and we space the reads as much as possible through the column-swatches
#ifdef PREFETCH   // if architecture doesn't support prefetch, skip it
     if((3*OPWIDTH)==(a3rem&(3*OPWIDTH))){  // every fourth swatch
      I inc=((a3rem&(8*OPWIDTH))?p:n)*sizeof(D);    // counting down, a then z
      C *base=(C*)((a3rem&(8*OPWIDTH))?a2base0:z2base) + inc + (a3rem&(4*OPWIDTH)?0:inc);
      PREFETCH2(base); PREFETCH2(base+CACHELINESIZE); PREFETCH2(base+2*CACHELINESIZE);
     }
#endif

     // Now do the 16 outer products for the block, each 2ax4w (or 2ax2w if flgs)
     I a4rem=MIN(w1rem,CACHEHEIGHT);
     D * RESTRICT c4base=c3base;
     if(!(flgs&(FLGCMP|FLGINT))){  // real
      D* RESTRICT a4base0=a2base0; D* RESTRICT a4base1=a2base1; 
      do{  // loop for each small outer product
       // read the 2x1 a values and the 1x4 cache values
       // form outer product, add to accumulator
       z00+=a4base0[0]*c4base[0];
       z01+=a4base0[0]*c4base[1];
       z02+=a4base0[0]*c4base[2];
       z03+=a4base0[0]*c4base[3];
       z10+=a4base1[0]*c4base[0];
       z11+=a4base1[0]*c4base[1];
       z12+=a4base1[0]*c4base[2];
       z13+=a4base1[0]*c4base[3];
       a4base0++,a4base1++;
       c4base+=CACHEWIDTH;
      }while(--a4rem>0);
     }else if(flgs&FLGINT){  // INT
      I* RESTRICT a4base0=(I*)a2base0; I* RESTRICT a4base1=(I*)a2base1; 
      do{  // loop for each small outer product
       // read the 2x1 a values and the 1x4 cache values
       // form outer product, add to accumulator
       z00+=(D)a4base0[0]*(D)c4base[0];
       z01+=(D)a4base0[0]*(D)c4base[1];
       z02+=(D)a4base0[0]*(D)c4base[2];
       z03+=(D)a4base0[0]*(D)c4base[3];
       z10+=(D)a4base1[0]*(D)c4base[0];
       z11+=(D)a4base1[0]*(D)c4base[1];
       z12+=(D)a4base1[0]*(D)c4base[2];
       z13+=(D)a4base1[0]*(D)c4base[3];
       a4base0++,a4base1++;
       c4base+=CACHEWIDTH;
      }while(--a4rem>0);
     }else{
      // complex.  The 1x4 cache values represent 1x2 complex values.  The a is fetched as 2x1 complex values.  Result is 2x2 conplex values
      D* RESTRICT a4base0=a2base0; D* RESTRICT a4base1=a2base1; 
      do{  // loop for each small outer product
       // read the 2x1 a values and the 1x4 cache values
       // form outer product, add to accumulator
       z00+=a4base0[0]*c4base[0]-a4base0[1]*c4base[1];
       z01+=a4base0[0]*c4base[1]+a4base0[1]*c4base[0];
       z02+=a4base0[0]*c4base[2]-a4base0[1]*c4base[3];
       z03+=a4base0[0]*c4base[3]+a4base0[1]*c4base[2];
       z10+=a4base1[0]*c4base[0]-a4base1[1]*c4base[1];
       z11+=a4base1[0]*c4base[1]+a4base1[1]*c4base[0];
       z12+=a4base1[0]*c4base[2]-a4base1[1]*c4base[3];
       z13+=a4base1[0]*c4base[3]+a4base1[1]*c4base[2];
       a4base0+=2,a4base1+=2;
       c4base+=CACHEWIDTH;
      }while(--a4rem>0);
     }

     // Store accumulator into z.  Don't store outside the array
     z3base[0]=z00;
     if(a3rem>3){z3base[1]=z01,z3base[2]=z02,z3base[3]=z03; if(a2rem>1)z3base[n]=z10,z3base[n+1]=z11,z3base[n+2]=z12,z3base[n+3]=z13;
     }else{if(a3rem>1){z3base[1]=z01;if(a3rem>2)z3base[2]=z02;}; if(a2rem>1){z3base[n]=z10;if(a3rem>1){z3base[n+1]=z11;if(a3rem>2)z3base[n+2]=z12;}}}
    }
   }
  }
 }
 R 1;
}

#endif

// +/ . *
F2(jtpdt){PROLOG(0038);A z;I ar,at,i,m,n,p,p1,t,wr,wt;
 ARGCHK2(a,w);
 // ?r = rank, ?t = type (but set Boolean type for an empty argument)
 ar=AR(a); at=AT(a); at=AN(a)?at:B01;
 wr=AR(w); wt=AT(w); wt=AN(w)?wt:B01;
 if(unlikely(ISSPARSE(at|wt)))R pdtsp(a,w);  // Transfer to sparse code if either arg sparse
 if(unlikely(((at|wt)&XNUM+RAT)!=0))R df2(z,a,w,atop(slash(ds(CPLUS)),qq(ds(CSTAR),v2(1L,AR(w)))));  // On indirect numeric, execute as +/@(*"(1,(wr)))
 if(unlikely(B01&(at|wt)&&TYPESNE(at,wt)&&((ar-1)|(wr-1)|(AN(a)-1)|(AN(w)-1))>=0))R pdtby(a,w);   // If exactly one arg is boolean, handle separately
 {t=maxtyped(at,wt); if(!TYPESEQ(t,AT(a))){RZ(a=cvt(t,a));} if(!TYPESEQ(t,AT(w))){RZ(w=cvt(t,w));}}  // convert args to compatible precisions, changing a and w if needed.  B01 if both empty
 ASSERT(t&NUMERIC,EVDOMAIN);
 // Allocate result area and calculate loop controls
 // m is # 1-cells of a
 // n is # atoms in an item of w
 // p is number of inner-product muladds (length of a row of a)

 // INT multiplies convert to float, for both 32- and 64-bit systems.  It is converted back if there is no overflow
 m=t; m=t&INT?FL:m; m=t&B01?INT:m;  // type of result, promoting bool and int
 RZ(z=ipprep(a,w,m,&m,&n,&p));  // allocate the result area, with the needed shape and type
 if(AN(z)==0)R z;  // return without computing if result is empty
 if(!p){mvc(AN(z)<<bplg(AT(z)),AV(z),1,MEMSET00); R z;}  // if dot-products are all 0 length, set them all to 0
 // If either arg is atomic, reshape it to a list
 if(!ar!=!wr){if(ar)RZ(w=reshape(sc(p),w)) else RZ(a=reshape(sc(p),a));}
 p1=p-1;
 // Perform the inner product according to the type
 switch(CTTZNOFLAG(t)){
 case B01X:
  if(0==(n&(SZI-1))||!SY_ALIGN){A tt;B*u,*v,*wv;I nw,*x,*zv;UC*c,*tc;UI*d,*ti,*vi;
   nw=(n+SZI-1)>>LGSZI;
   GATV0(tt,INT,nw,1); ti=(UI*)AV(tt); tc=(UC*)ti;
   u=BAV(a); v=wv=BAV(w); zv=AV(z);
   for(i=0;i<m;++i,v=wv,zv+=n){x=zv; DQ(n, *x++=0;); I pp=p; while((pp-=255)>=0){BBLOCK(255);} BBLOCK(pp+255);}
  }else{B*u,*v,*wv;I*x,*zv;
   u=BAV(a); v=wv=BAV(w); zv=AV(z);
   for(i=0;i<m;++i,v=wv,zv+=n){
           x=zv; if(*u++)DQ(n, *x++ =*v++;) else{v+=n; DQ(n, *x++=0;);}
    DQ(p1, x=zv; if(*u++)DQ(n, *x+++=*v++;) else v+=n;);
   }
  }
  break;
 case INTX:
  {
#if SY_64
 /*
   for(i=0;i<m;++i,v=wv,zv+=n){
     x=zv; c=*u++; er=asmtymes1v(n,x,c,v);    if(er)break; v+=n;
     DQ(p1, x=zv; c=*u++; er=asminnerprodx(n,x,c,v); if(er)break; v+=n;);

 */
   // INT product is problematic, because it is used for many internal purposes, such as #. and indexing of { and m} .  For these uses,
   // one argument (usually w) has only one item, a list that is reused.  So, we check for that case; if found we go through faster code that just
   // performs vector inner products, accumulating in registers.  And we have multiple versions of that: one when the totals can't get close to
   // overflow, and other belt-and-suspenders variants for arbitrary inputs
   if(n==1){DPMULDDECLS I tot;I* RESTRICT zv, * RESTRICT av;
    // vector products
    // The fast loop will be used if each multiplicand, and each product, fits in 32 bits
    I er=0;  // will be set if overflow detected
    I* RESTRICT wv=AV(w); tot=0; DQ(p, I wvv=*wv; if((I4)wvv!=wvv){er=1; break;} wvv=wvv<0?-wvv:wvv; tot+=wvv; wv++;)
    if(!er){
     // w fits in 32 bits.  Try to accumulate the products.  If we can be sure that the total will not exceed 32 bits unless
     // an a-value does, do the fastest loop
     zv=AV(z); av=AV(a);
     if((UI)(p*tot)<(UI)0x100000000){ // integer overflow
      // The total in w is so small that a mere m values each less than 2^31 cannot overflow
      DQ(m, I tot=0; wv=AV(w); DQ(p, I mpcnd=*av++; I prod=mpcnd**wv++; if(mpcnd!=(I4)mpcnd)goto oflo1; tot+=prod;) *zv++=tot;)
     }else{
      // w has some big numbers, so we have to check each individual product
      DQ(m, I tot=0; wv=AV(w); DQ(p, I mpcnd=*av++; I prod=mpcnd**wv++; if(mpcnd!=(I4)mpcnd||prod!=(I4)prod)goto oflo1; tot+=prod;) *zv++=tot;)
     }
     AT(z)=INT; break;
    }
oflo1:
    // Something overflowed 32 bits.  See if it fits in 64.
    zv=AV(z); av=AV(a);
    DQ(m, I lp; I tot=0; wv=AV(w); DQ(p, DPMULD(*av++,*wv++, lp, goto oflo2;) I oc=(~tot)^lp; tot+=lp; lp^=tot; if(XANDY(oc,lp)<0)goto oflo2;) *zv++=tot;)
    AT(z)=INT; break;
oflo2:
    // Result does not fit in INT.  Do the computation as float, with float result
    if(m)RZ(jtsumattymesprods(jt,INT,voidAV(w),voidAV(a),p,1,1,1,m,voidAV(z)));  // use +/@:*"1 .  Exchange w and a because a is the repeated arg in jtsumattymesprods.  If error, clear z (should not happen here)
   }else{
     // full matrix products
     I probsize = m*n*(IL)p;  // This is proportional to the number of multiply-adds.  We use it to select the implementation
     if((UI)probsize < (UI)JT(jt,igemm_thres)){RZ(a=cvt(FL,a)); RZ(w=cvt(FL,w)); cachedmmult(jt,DAV(a),DAV(w),DAV(z),m,n,p,0);}  // Do our matrix multiply - converting   TUNE
     else {
      // for large problem, use BLAS
      mvc(m*n*sizeof(D),DAV(z),1,MEMSET00);
      igemm_nn(m,n,p,1,(I*)DAV(a),p,1,(I*)DAV(w),n,1,0,DAV(z),n,1);
    }
    // If the result has a value that has been truncated, we should keep it as a float.  Unfortunately, there is no way to be sure that some
    // overflow has not occurred.  So we guess.  If the result is much less than the dynamic range of a float integer, convert the result
    // to integer.
    I i; D *zv=DAV(z);
    for(zv=DAV(z), i=AN(z); i; --i, ++zv)if(*zv>1e13 || *zv<-1e13)break;   // see if any value is out of range
    if(!i){AT(z)=INT;for(zv=DAV(z), i=AN(z); i; --i, ++zv)*(I*)zv=(I)*zv;}  // if not, convert all to integer
   }
#else
   // 32-bit version - old style, converting to float
  {I smallprob; 
   NAN0;
   if(n==1){D* RESTRICT zv; I* RESTRICT av, * RESTRICT wv;
    zv=DAV(z); av=AV(a);
    DQ(m, D tot=0; wv=AV(w); DQ(p, tot+=((D)*av++)*((D)*wv++);) *zv++=tot;)
    smallprob=0;  // Don't compute it again
   }else if(!(smallprob = m*n*(IL)p<1000LL)){  // if small problem, avoid the startup overhead of the matrix version  TUNE
      mvc(m*n*sizeof(D),DAV(z),1,MEMSET00);
      igemm_nn(m,n,p,1,(I*)DAV(a),p,1,(I*)DAV(w),n,1,0,DAV(z),n,1);
   }
   // If there was a floating-point error, retry it the old way in case it was _ * 0
   if(smallprob||NANTEST){D c,*x,*zv;I*u,*v,*wv;
    u=AV(a); v=wv=AV(w); zv=DAV(z);
    if(1==n)DQ(m, v=wv; c=0.0; DQ(p, c+=*u++*(D)*v++;); *zv++=c;)
    else for(i=0;i<m;++i,v=wv,zv+=n){
            x=zv; c=(D)*u++; DQ(n, *x++ =c**v++;);
     DQ(p1, x=zv; c=(D)*u++; DQ(n, *x+++=c**v++;););
    }
   }
  // convert float result back to int if it will fit
  RZ(z=icvt(z));
  }
#endif
  }
  break;
 case FLX:
  {I smallprob; 
   // As for INT, handle the cases where n=1 separately, because they are used internally & don't require as much setup as matrix multiply
   NAN0;
   if(n==1){
    if(m)RZ(jtsumattymesprods(jt,FL,voidAV(w),voidAV(a),p,1,1,1,m,voidAV(z)));  // use +/@:*"1 .  Exchange w and a because a is the repeated arg in jtsumattymesprods.  If error, clear z
    smallprob=0;  // Don't compute it again
   }else{
#if 0   // for TUNEing
// %.: 100 .0008, 200 0.005, 500 0.62, 1000 0.4, 10000 285
// Results 10/2019
// m n 
// 2 2  blocked always; smallprob beats cached up to 100000
// 3 3  blocked always; smallprob beats cached up to 10000
// 4 4  blocked always; smallprob beats cached up to 200
// 8 8  blocked always; smallprob beats cached below 20
// 16 16 blocked always; cached competitive for 5000 and above
// 24 24 blocked always; cached competitive
// 28 28 blocked always; cached competitive
// 32 32 blocked up to 1000, then cached
// 64 64 blocked up to 52, then cached
// 128 128 cached
// 256 256 cached
// 512 512 cached
// 768 768 cached
// 1024 1024 BLAS because cached takes a beating - fix this
// others cached
//
// p n
// 2 2  blocked always; smallprob beats cached up to 100000
// 3 3  blocked always; smallprob beats cached up to 100000
// 4 4  blocked always; smallprob beats cached up to 100000
// 8 8  blocked; otherwise cached
// 16 16  blocked; otherwise cached
// 24 24  blocked; otherwise BLAS
// 32 32  blocked; otherwise BLAS
// 64 64  cached till m=500; then blocked till 10000; then BLAS
// 132 132  cached till m=2000; then BLAS
// 256 256  cached till m=2000; then BLAS
// 520 520  cached till m=2000; then BL7AS
// 1032 1032  cached till m=5000; then BLAS
// 2056 2056  cached till m=5000; then BLAS
//
/*
NB. y is m,p,n, result is 1 timing value
time1 =: 3 : 0"1
rpts =. 10000 <. 5 >. <. 1e9% * / y
l =. (2 {. y) ?@$ 0
r =. (_2 {. y) ?@$ 0
rpts 6!:2 'l +/ . * r'
)
*/
#if 0  // large n, possibly short m p
/*
NB. x is m, y is p, we run timings with a range of n for each algorithm
NB. result is 4 rows, 1 for each algo
timemp =: 4 : 0
lens =. (1e10 % x*y) (> # ]) 12 20 52 100 200 500 1000 2000 5000 10000 20000 50000 100000 200000 500000 1000000
time1 (x,y)&,"0 ((256 1e20 1e20 65536 > x*y) # 0 1 2 3) +/ lens
)
*/
    // if low 2 bits of n are 00, use small; if 01, use cached; if 10, use BLAS; if 11 use blocked
    smallprob=0;
    if((n&3)==3){blockedmmult(jt,DAV(a),DAV(w),DAV(z),m,n,p,0);}
    else if(n&2){
     mvc(m*n*sizeof(D),DAV(z),1,MEMSET00);
     dgemm_nn(m,n,p,1.0,DAV(a),p,1,DAV(w),n,1,0.0,DAV(z),n,1);
    }else if(n&1){cachedmmult(jt,DAV(a),DAV(w),DAV(z),m,n,p,0);
    }else smallprob=1;
#else  // large m, possibly short p n
/*
NB. x is m, y is p, we run timings with a range of n for each algorithm
NB. result is 4 rows, 1 for each algo
timemp =: 4 : 0
lens =. (1e10 % x*y) (> # ]) 12 20 52 100 200 500 1000 2000 5000 10000 20000 50000 100000 200000 500000 1000000
time1 ,&(x,y)"0 ((256 1e20 1e20 65536 > x*y) # 0 1 2 3) +/ lens
)
*/
    // if low 2 bits of m are 00, use small; if 01, use cached; if 10, use BLAS; if 11 use blocked
    smallprob=0;
    if((m&3)==3){blockedmmult(jt,DAV(a),DAV(w),DAV(z),m,n,p,0);}
    else if(m&2){
     mvc(m*n*sizeof(D),DAV(z),1,MEMSET00);
     dgemm_nn(m,n,p,1.0,DAV(a),p,1,DAV(w),n,1,0.0,DAV(z),n,1);
    }else if(m&1){cachedmmult(jt,DAV(a),DAV(w),DAV(z),m,n,p,0);
    }else smallprob=1;
#endif
#else
   // not single column.  Choose the algorithm to use
#if (C_AVX || EMU_AVX) && defined(PREFETCH)
    smallprob=0;  // never use Dic method; but used to detect pick up NaN errors
    D *av=DAV(a), *wv=DAV(w), *zv=DAV(z);  //  pointers to sections
    I flgs=((AFLAG(a)>>(AFUPPERTRIX-FLGAUTRIX))&FLGAUTRI)|((AFLAG(w)>>(AFUPPERTRIX-FLGWUTRIX))&FLGWUTRI);  // flags from a or w
    if((UI)(m*n*(IL)p)>=(UI)JT(jt,dgemm_thres)){   // test for BLAS.  For AVX2 this should not be taken; for other architectures tuning is required
     mvc(m*n*sizeof(D),DAV(z),1,MEMSET00);
     dgemm_nn(m,n,p,1.0,DAV(a),p,1,DAV(w),n,1,0.0,DAV(z),n,1);
    } else {
     smallprob=1^cachedmmult(jt,av,wv,zv,m,n,p,flgs);  // run the cached mult; if NaN error, remember that fact
    }
#else
    I probsize = (m-1)*n*(IL)p;  // This is proportional to the number of multiply-adds.  We use it to select the implementation.  If m==1 we are doing dot-products; no gain from fancy code then
    if(!(smallprob = (m<=4||probsize<1000LL))){  // if small problem, avoid the startup overhead of the matrix version  TUNE
     if((UI)probsize < (UI)JT(jt,dgemm_thres))
      cachedmmult(jt,DAV(a),DAV(w),DAV(z),m,n,p,((AFLAG(a)>>(AFUPPERTRIX-FLGAUTRIX))&FLGAUTRI)|((AFLAG(w)>>(AFUPPERTRIX-FLGWUTRIX))&FLGWUTRI));  // Do our one-core matrix multiply - real   TUNE this is 160x160 times 160x160.  Tell routine if uppertri
     else{
      // If the problem is really big, use BLAS
      mvc(m*n*sizeof(D),DAV(z),1,MEMSET00);
      dgemm_nn(m,n,p,1.0,DAV(a),p,1,DAV(w),n,1,0.0,DAV(z),n,1);
     }
    }
#endif
#endif
   }
   // If there was a floating-point error, retry it the old way in case it was _ * 0
   if(smallprob||NANTEST){D c,s,t,*u,*v,*wv,*x,*zv;
    u=DAV(a); v=wv=DAV(w); zv=DAV(z);
    NAN0;
    if(1==n){DQ(m, v=wv; c=0.0; DQ(p, s=*u++; t=*v++; c+=TYMES(s,t);); *zv++=c;);}
    else for(i=0;i<m;++i,v=wv,zv+=n){
            x=zv; if(c=*u++){if(INF(c))DQ(n, *x++ =TYMES(*v,c); ++v;)else DQ(n, *x++ =c**v++;);}else{v+=n; DQ(n, *x++=0.0;);}
     DQ(p1, x=zv; if(c=*u++){if(INF(c))DQ(n, *x+++=TYMES(*v,c); ++v;)else DQ(n, *x+++=c**v++;);}else v+=n;);
    }
    NAN1;
   }
  }
  break;
 case CMPXX:
  {NAN0;
   I probsize = m*n*(IL)p;  // This is proportional to the number of multiply-adds.  We use it to select the implementation
   I smallprob=probsize<1000;  // set if we do the old-fashioned way, possibly after error
   if(!smallprob){  // use old-fashioned way if small.  16b3.4 comes though here
    if((UI)probsize<(UI)JT(jt,zgemm_thres)){smallprob=1^cachedmmult(jt,DAV(a),DAV(w),DAV(z),m,n*2,p*2,((AFLAG(a)>>(AFUPPERTRIX-FLGAUTRIX))&FLGAUTRI)|((AFLAG(w)>>(AFUPPERTRIX-FLGWUTRIX))&FLGWUTRI)|FLGCMP);}  // Do the fast matrix multiply - complex.  Change widths to widths in D atoms, not complex atoms  TUNE  this is 130x130 times 130x130
    else {
      // Large problem - start up BLAS
      mvc(2*m*n*sizeof(D),DAV(z),1,MEMSET00);
      zgemm_nn(m,n,p,zone,(dcomplex*)DAV(a),p,1,(dcomplex*)DAV(w),n,1,zzero,(dcomplex*)DAV(z),n,1);
    }
   }
   if(smallprob||NANTEST){Z c,*u,*v,*wv,*x,*zv;
    // There was a floating-point error.  In case it was 0*_ retry old-style
    u=ZAV(a); v=wv=ZAV(w); zv=ZAV(z);
    NAN0;
    if(1==n)DQ(m, v=wv; c=zeroZ; DQ(p, c.re+=ZRE(*u,*v); c.im+=ZIM(*u,*v); ++u; ++v;); *zv++=c;)
    else for(i=0;i<m;++i,v=wv,zv+=n){
            x=zv; c=*u++; DQ(n, x->re =ZRE(c,*v); x->im =ZIM(c,*v); ++x; ++v;);
     DQ(p1, x=zv; c=*u++; DQ(n, x->re+=ZRE(c,*v); x->im+=ZIM(c,*v); ++x; ++v;););
    }
    NAN1;
   }
  }
  break;
 }
 EPILOG(z);
}


#define IPBX0  0
#define IPBX1  1
#define IPBXW  2
#define IPBXNW 3

// a f/ . g w  for boolean a and w
// c is pseudochar for f, d is pseudochar for g
static A jtipbx(J jt,A a,A w,C c,C d){A g=0,x0,x1,z;B*av,*av0,b,*v0,*v1,*zv;C c0,c1;
    I ana,i,j,m,n,p,q,r,*uu,*vv,wc;
 ARGCHK2(a,w);
 RZ(z=ipprep(a,w,B01,&m,&n,&p));
 // m=#1-cells of a, n=# bytes in 1-cell of w, p=length of individual inner product creating an atom
 ana=!!AR(a); wc=AR(w)?n:0; q=(n-1)>>LGSZI; r=(-n)&(SZI-1);  // ana = 1 if a is not atomic; wc = stride between items of w; q=#fullwords to proc, r=#bytes of last one NOT to proc
 // Set c0 & c1 to classify the g operation
#if !SY_64
 switch(B01&AT(w)?d:0){
  case CEQ:                             c0=IPBXNW; c1=IPBXW;  break;
  case CNE:                             c0=IPBXW;  c1=IPBXNW; break;
  case CPLUSDOT: case CMAX:             c0=IPBXW;  c1=IPBX1;  break;
  case CSTARDOT: case CMIN: case CSTAR: c0=IPBX0;  c1=IPBXW;  break;
  case CLT:                             c0=IPBXW;  c1=IPBX0;  break;
  case CGT:                             c0=IPBX0;  c1=IPBXNW; break; 
  case CLE:                             c0=IPBX1;  c1=IPBXW;  break;
  case CGE:                             c0=IPBXNW; c1=IPBX1;  break;
  case CPLUSCO:                         c0=IPBXNW; c1=IPBX0;  break;
  case CSTARCO:                         c0=IPBX1;  c1=IPBXNW; break;
  default: c0=c1=-1; g=ds(d); RZ(df2(x0,num(0),w,g)); RZ(df2(x1,num(0),w,g)); break;
 }
 // Set up x0 to be the argument to use for y if the atom of x is 0: 0, 1, y, -.y
 // Set up x1 to be the arg if xatom is 1
 if(!g)RZ(x0=c0==IPBX0?reshape(sc(n),num(0)):c0==IPBX1?reshape(sc(c==CNE?AN(w):n),num(1)):c0==IPBXW?w:not(w));
 if(!g)RZ(x1=c1==IPBX0?reshape(sc(n),num(0)):c1==IPBX1?reshape(sc(c==CNE?AN(w):n),num(1)):c1==IPBXW?w:not(w));
#else
 // See if we can use special techniques
#define Q9(f,c0,c1) ((I)(((c1)<<2)+(c0))<<(((f)-CSTARCO)<<2))
#define PMSK Q9(CSTARCO,IPBX1,IPBXNW)+Q9(CPLUSCO,IPBXNW,IPBX0)+Q9(CSTAR,IPBX0,IPBXW)+Q9(CPLUS,0,0)+Q9(CPLUSDOT,IPBXW,IPBX1)+Q9(CSTARDOT,IPBX0,IPBXW)+Q9(CEQ,IPBXNW,IPBXW)+Q9(CNE,IPBXW,IPBXNW)+Q9(CLT,IPBXW,IPBX0)+Q9(CLE,IPBX1,IPBXW)+Q9(CGE,IPBXNW,IPBX1)+Q9(CGT,IPBX0,IPBXNW)+Q9(CEBAR,0,0)+Q9(CEPS,0,0)+Q9(CMIN,IPBX0,IPBXW)+Q9(CMAX,IPBXW,IPBX1)
 I pmsk=0; pmsk=BETWEENC(d,CSTARCO,CMAX)?PMSK:pmsk; pmsk&=-(B01&AT(w));
 c0=(pmsk>>((d-CSTARCO)<<2))&3; c1=(pmsk>>(((d-CSTARCO)<<2)+2))&3;  // get control bits if known fn, (0,0) if unknown
 // create x0 and x1, the result of (g 0) and (g 1).  If we know the function we can avoid invoking g sometimes
 if(c0+c1==0){
  // unsupported g.  Set c0/c1 to invalid and execute g to find x0/x1
  c0=c1=-1; g=ds(d); RZ(df2(x0,num(0),w,g)); RZ(df2(x1,num(0),w,g));
 }else{
  RZ(x0=c0==IPBX0?reshape(sc(n),num(0)):c0==IPBX1?reshape(sc(c==CNE?AN(w):n),num(1)):c0==IPBXW?w:not(w));
  RZ(x1=c1==IPBX0?reshape(sc(n),num(0)):c1==IPBX1?reshape(sc(c==CNE?AN(w):n),num(1)):c1==IPBXW?w:not(w));
 }
 #endif
 // av->a arg, zv->result, v0->input for 0, v1->input for 1
 av0=BAV(a); zv=BAV(z); v0=BAV(x0); v1=BAV(x1);

 // See if the operations are such that a 0 or 1 from a can skip the innerproduct, or perhaps
 // terminate the entire operation
 I esat=2, eskip=2;  // if byte of a equals esat, the entire result-cell is set to its limit value; if it equals eskip, the innerproduct is skipped
 if(c==CPLUSDOT&&(c0==IPBX1||c1==IPBX1)||c==CSTARDOT&&(c0==IPBX0||c1==IPBX0)){
 // +./ . [+. <: >: *:}   *./ . [*. < > +:]   a byte of a can saturate the entire result-cell: see which value does that
  esat=c==CPLUSDOT?c1==IPBX1:c1==IPBX0;  // esat==1 if +./ . (+. >:)   or *./ . (< +:) where x=1 overrides y (producing 1);  if esat=0, x=0 overrides y (producing 1)
 }else if(c==CPLUSDOT&&(c0==IPBX0||c1==IPBX0)||c==CSTARDOT&&(c0==IPBX1||c1==IPBX1)||
     c==CNE&&(c0==IPBX0||c1==IPBX0)){
  // [+. ~:]/ . [*. < > +:]   *./ . [+. <: >: *:]  a byte of a can guarantee the current innerproduct has no effect
  eskip=c==CSTARDOT?c1==IPBX1:c1==IPBX0;  // eskip==1 if  (+. ~:)/ . (+:)   *./ . (+. >:)  where x=1 has no effect; if esat=0, x=0 has no effect
 }

 switch(c){
  case CPLUSDOT:
#define F |=
#include "cip_t.h"
   break;
  case CSTARDOT:
#define F &=
#include "cip_t.h"
   break;
  case CNE:
#define F ^=
#include "cip_t.h"
   break;
 }
 R z;
}    /* a f/ . g w  where a and w are nonempty and a is boolean */

static DF2(jtdotprod){A fs,gs;C c;I r;V*sv;
 ARGCHK3(a,w,self);
 sv=FAV(self); fs=sv->fgh[0]; gs=sv->fgh[1];  // op is fs . gs
 if((SGNIF(AT(a)&AT(w),B01X)&-AN(a)&-AN(w)&-(FAV(gs)->flag&VISATOMIC2))<0&&CSLASH==FAV(fs)->id&&  // fs is c/
     (c=FAV(FAV(fs)->fgh[0])->id,BETWEENC((c^(CPLUSDOT^CEQ)),CSTARDOT,CNE)))R ipbx(a,w,c,FAV(gs)->id);  // [+.*.~:]/ . boolean   swap = and +., then test for range
 r=lr(gs);   // left rank of v
 A z; R df2(z,a,w,atop(fs,qq(gs,v2(r==RMAX?r:1+r,RMAX))));  // inner product according to the Dic
}


static F1(jtminors){A d,z;
 RZ(d=apvwr(3L,-1L,1L)); AV(d)[0]=0;
 R drop(d,df2(z,num(1),w,bsdot(ds(CLEFT))));  // 0 0 1 }. 1 [\. w 
}

static DF1(jtdet){DECLFG;A h=sv->fgh[2];I c,r,*s;
 ARGCHK1(w);
 r=AR(w); s=AS(w);
 A z; if(h&&1<r&&2==s[r-1]&&s[r-2]==s[r-1])R df1(z,w,h);
 F1RANK(2,jtdet,self);
 c=2>r?1:s[1];
 R !c ? df1(z,mtv,slash(gs)) : 1==c ? CALL1(f1,ravel(w),fs) : h && c==s[0] ? gaussdet(w) : detxm(w,self); 
}

DF1(jtdetxm){A z; R dotprod(IRS1(w,0L,1L,jthead,z),det(minors(w),self),self);}
     /* determinant via expansion by minors. w is matrix with >1 columns */

F2(jtdot){A f,h=0;AF f2=jtdotprod;C c,d;
 ASSERTVV(a,w);
 if(CSLASH==FAV(a)->id){
  f=FAV(a)->fgh[0]; c=FAV(f)->id; d=FAV(w)->id;  // op was c/ . d
  if(d==CSTAR){
   if(c==CPLUS )f2=jtpdt;   // +/ . * is a special function
   if(c==CMINUS)RZ(h=eval("[: -/\"1 {.\"2 * |.\"1@:({:\"2)"));  // -/ . * - calculate some function used by determinant?
  }
 }
 R fdef(0,CDOT,VERB, jtdet,f2, a,w,h, 0L, 2L,RMAX,RMAX);
}
