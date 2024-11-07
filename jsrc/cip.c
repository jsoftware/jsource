/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Inner Product                                             */

#include "j.h"
#include "vasm.h"
#include "gemm.h"

// Analysis for inner product
// a,w are arguments
// zt is type of result
// *pm is # 1-cells of a
// *pn is # atoms in an item of w (but if w is a list, it is treated as a pxn column, and n=1)
// *pp is number of inner-product muladds
//   (in each, an atom of a multiplies an item of w)
static A jtipprep(J jt,A a,A w,I zt,I*pm,I*pn,I*pp){A z=mark;I*as,ar,ar1,m,mn,n,p,*ws,wr,wr1;
 ar=AR(a); as=AS(a); ar1=ar-((UI)ar>0); PRODX(m,ar1,as,1) *pm=m;  // m=# 1-cells of a.  It could overflow, if there are no atoms
 wr=AR(w); ws=AS(w); wr1=wr-((UI)wr>0); PRODX(n,wr1,ws+1,1) *pn=n; DPMULDE(m,n,mn);  // n=#atoms in item of w; mn = #atoms in result
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
  v=wv=(Tw*)AV(w); zv=(Tz*)AV(z); mvc(sizeof(Tz),&zero,MEMSET00LEN,MEMSET00);     \
  if(1==n)DQ(m, v=wv; c=zero;           DQ(p,       if(*u++)INC(c,*v);             ++v;);             *zv++=c;)   \
  else    DQ(m, v=wv; mvc(zk,zv,MEMSET00LEN,MEMSET00); DQ(p, x=zv; if(*u++)DQ(n, INC(*x,*v); ++x; ++v;) else v+=n;); zv+=n;  );  \
 }

#define PDTXB(Tz,Ta,INC,INIT)     \
 {Ta*u;Tz c,*x,zero,*zv;          \
  u=   (Ta*)AV(a); zv=(Tz*)AV(z); mvc(sizeof(Tz),&zero,MEMSET00LEN,MEMSET00);     \
  if(1==n)DQ(m, v=wv; c=zero;           DQ(p,                   if(*v++)INC(c,*u); ++u;  ); *zv++=c;)   \
  else    DQ(m, v=wv; mvc(zk,zv,MEMSET00LEN,MEMSET00); DQ(p, x=zv; INIT; DQ(n, if(*v++)INC(*x,c); ++x;);); zv+=n;  );  \
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

#if C_AVX2 || EMU_AVX2
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
  __m256d z10, a00=_mm256_broadcast_sd(&av[0]), a01=_mm256_broadcast_sd(&av[1]), a10=_mm256_broadcast_sd(&av[pstored]), a11=_mm256_broadcast_sd(&av[pstored+1]);
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
 while(nrem>=NPAR){  // do ?x4s as long as possible.  The load bandwidth is twice as high
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
// ctx block passed in from the task code
typedef struct {
 D*av,*wv,*zv;  // arg pointers into cachedmmultx, defined below
 I m,n,p;  // dimensions ((mxp) x (pxn)) defined below
 I flgs;   // complex, triangular processing flags
 I nbigtasks[2];  // number of tasks using taskm[0]; number of tasks that are not in the shortened tail
 I4 taskm[2];  // number of rows in leading tasks, unshortened trailing tasks
} CACHEMMSTATE;
#define OPHEIGHTX 2
#define OPHEIGHT ((I)1<<OPHEIGHTX)  // height of outer-product block
#define OPWIDTHX 3
#define OPWIDTH ((I)1<<OPWIDTHX)  // width of outer-product block
#define CACHEWIDTH 64  // width of resident cache block (in D atoms)
#define CACHEHEIGHTX 4
#define CACHEHEIGHT ((I)1<<CACHEHEIGHTX)  // height of resident cache block
#define MAXAROWS ((L2CACHESIZE/CACHEWIDTH/sizeof(D))*3/4)  // max #rows of a/z that we can process while staying in L2 cache   a strip is m*CACHEHEIGHT, z strip is m*CACHEWIDTH
static D missingrow[CACHEHEIGHT]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
// Floating-point matrix multiply, hived off to a subroutine to get fresh register allocation
// *zv=*av * *wv, with *cv being a cache-aligned region big enough to hold CACHEWIDTH*CACHEHEIGHT floats
// a is shape mxp, w is shape pxn.  Result is 0 if OK, 1 if fatal error
// m must not exceed MAXAROWS.
// Result is for taskrun: 0=OK, otherwise error#
static NOINLINE C cachedmmultx(J jt,void *ctx,UI4 ti){ CACHEMMSTATE *pd=ctx;
 // The task# ti must be converted to a slice of a to use: height m at offset moffset
 I flgs=pd->flgs;
 I tishort=ti-pd->nbigtasks[0]; I m=pd->taskm[REPSGN(tishort)+1];  // tishort is how far ti is into the second set of tasks.  If negative, it's in the first set.  Set m to the appropriate tasksize.
 I mtailct=pd->nbigtasks[1]-ti;  // mtailct is negative of (how far ti is past the second set of tasks).  The third set is one cacheblock shorter than the second set
 I moffset=ti*pd->taskm[0]+tishort*(m-pd->taskm[0])+((mtailct&REPSGN(mtailct))<<CACHEHEIGHTX);  // combine to get position of task: size if big blocks, minus adj for blocks in second group, minus single-block adj for third group
 m+=REPSGN(mtailct-1)<<CACHEHEIGHTX; m=MIN(m,pd->m-moffset);  // if third group, reduce the size by one block; truncate the last task if it goes beyond the overall size
 I n=pd->n;
 I pnom=pd->p-(((moffset)&-((flgs>>FLGAUTRIX)&1))<<(flgs&FLGCMP));  // for upper-tri, shorten the rows of a to the nonzero part
 I pstored=pd->p;
 D *av=pd->av+(moffset*(pd->p+(((flgs>>FLGAUTRIX)&1)<<(flgs&FLGCMP))));   // for upper-triangular, horizontally skip the zeros of a
 D *wv=pd->wv+((n*moffset)&-((flgs>>FLGAUTRIX)&1)); // for upper-tri, vertically skip the zeroed-out rows of w
 D *zv=pd->zv+(n*moffset);
 // Small problems should use a multiplier that does not use full cacheblock.  For jobs, we make the decision for each task
 if(((((50-m)&(50-n)&(16-pstored)&((DCACHED_THRES-1)-m*n*pnom))|SGNIF(flgs,FLGCMPX))&SGNIFNOT(flgs,FLGWMINUSZX))>=0){  // blocked for small arrays in either dimension (after threading); not if CMP; force if WMINUSZ (can't be both)
  // blocked algorithm.  there is no size limit on the blocks
  R blockedmmult(jt,av,wv,zv,m,n,pnom,pstored,flgs)?0:EVNAN;  // blockedmmult uses normal JE return of 0=error; we return job semantics, 0=OK
 }

 D c[(CACHEHEIGHT+1)*CACHEWIDTH + (CACHEHEIGHT+1)*OPHEIGHT*OPWIDTH*2 + 2*CACHELINESIZE/sizeof(D)];  // 2 in case complex
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
 D *cvw = (D*)(((I)&c+(CACHELINESIZE-1))&-CACHELINESIZE);  // place where cache-blocks of w are staged
 D (*cva)[2][OPHEIGHT][CACHEHEIGHT] = (D (*)[2][OPHEIGHT][CACHEHEIGHT])(((I)cvw+(CACHEHEIGHT+1)*CACHEWIDTH*sizeof(D)+(CACHELINESIZE-1))&-CACHELINESIZE);   // place where expanded rows of a are staged
 // If a is upper-triangular, we write out the entire column of z values only when we process the last section of the w stripe.  If w is also upper-triangular,
 // we stop processing sections before we get to the bottom.  So in that case (which never happens currently), clear the entire result areas leaving 0 in the untouched bits
 if(unlikely((flgs&(FLGWUTRI|FLGAUTRI))==(FLGWUTRI|FLGAUTRI)))mvc(m*n*SZD,zv,MEMSET00LEN,MEMSET00);  // if w is upper-triangular, we will not visit all the z values and we must clear the lower-triangular part.  Here we just clear them all
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
    if(common(!(flgs&FLGCMP))){ //real case; contiguous, so simpler
     for(I i=0;i<OPHEIGHT;i++){
      D *a0x=a2base0+pstored*i; a0x=i>=a2rem?missingrow:a0x;  // start of samples for the row, or a repeated row if past the end
      if(common(nvalidops>=4)){ //fast path: handle using overlapping accesses
       for(I j=0;j<(nvalidops-NPAR);j+=NPAR){_mm256_store_pd(&(*cva)[0][i][j],_mm256_loadu_pd(a0x+j));}  //round down if not a whole number of quads, otherwise subtract one full quad
       _mm256_storeu_pd(&(*cva)[0][i][nvalidops-NPAR],_mm256_loadu_pd(a0x+nvalidops-NPAR)); //to avoid doing redundant work here
      }else{ //uncommon case; handle using scalar ops
       for(I j=0;j<nvalidops;++j){(*cva)[0][i][j]=a0x[j];}}}}  // float
    else{ //complex case; could be optimised
     for(I i=0;i<OPHEIGHT;i++){
      D *a0x=a2base0+pstored*i; a0x=i>=a2rem?missingrow:a0x;  // start of samples for the row, or a repeated row if past the end
      for(I j=0;j<nvalidops;++j){(*cva)[0][i][j]=*a0x; ++a0x; (*cva)[1][i][j]=*a0x; ++a0x;}}}}  // complex: real and imaginary

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
#define ACCz(r,c) z##r##c=_mm256_load_pd(zilblock+NPAR*(2*r+c));
        ACCz(0,0) ACCz(0,1) ACCz(1,0) ACCz(1,1) ACCz(2,0) ACCz(2,1) ACCz(3,0) ACCz(3,1)
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
// it appears that this is because the Z strip exceeds L2 cache and the bandwidth of L3 is not enough to support the traffic.  So we
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
 R unlikely(NANTEST)?EVNAN:0;  // return job semantics, 0=OK
}
// looping entry point for cached mmul
// We split the input into products where the left arg has at most MAXAROWS rows.  This is to avoid overrunning L2 cache
// Result is 0 if error, which must be NaN error
// For historical reason (i. e. to match the non-AVX2 version) n and p have been multiplied by 2 for complex multiplies
I cachedmmult(J jt,D* av,D* wv,D* zv,I m,I n,I p,I flgs){
// TODO: bug when EMU_AVX2
 if(((((24-m)&(24-n)&(16-p)&((DCACHED_THRESn-1)-m*n*p))|SGNIF(flgs,FLGCMPX))&SGNIFNOT(flgs,FLGWMINUSZX))>=0){  // TUNE blocked for small arrays in either dimension (after threading); not if CMP; force if WMINUSZ (can't be both)
  // small problem, not worth splitting.  there is no size limit on the blocks
  // 16x16 multiply takes about 1us; we are guessing task-wakeup takes a similar amount of time.  So it's not worth a split unless the time gets substantially above 1us
  R blockedmmult(jt,av,wv,zv,m,n,p,p,flgs); }
 UI nfulltasks, nremnant, tailtasks, endtasksize, fulltasksize;
 // big problem, split into tasks.  The tasks may be either cached or blocked
 UI nthreads=__atomic_load_n(&(*JT(jt,jobqueue))[0].nthreads,__ATOMIC_ACQUIRE)+(jt->threadpoolno!=0);  // get # running threads, just once so we have a consistent view.  We count our thread too, if it's not in pool 0, since it runs tasks for the job
 UI ncache=(m+CACHEHEIGHT-1)>>CACHEHEIGHTX;  // number of cacheblocks of a, including remnant
 // The cached algorithm works on sections that are a multiple of CACHEHEIGHT high, up to MAXAROWS.  For big arguments, we spread the CACHEHEIGHT sections through the threads as evenly as possible.
 // for modest arguments, breaking into CACHEHEIGHT blocks may not allow use of all threads.  In that case, use more threads (each of which will use the blocking algorithm)
 if(unlikely(ncache*4<nthreads*3)){
  // caching would leave 1/4 of the threads unused.  Do blocking
#define THR0HEADSTART 0  // if we know the lead thread will keep running, we should assign more work to it.  But it doesn't seem to.
  endtasksize=(((m-THR0HEADSTART-1)/nthreads)+1+3)&-4;  // # rows per thread, rounded up to multiple of 4; but reserving 8 rows for the lead thread
  UI effthreads=(m-THR0HEADSTART+endtasksize-1)/endtasksize; nthreads=effthreads<nthreads?effthreads:nthreads;  // get the max number threads we can use
  nfulltasks=1; fulltasksize=endtasksize+THR0HEADSTART;  // the first task, which will be handled in this thread (presumably without waiting) gets a few extra rows
  nremnant=tailtasks=nthreads-1;   // the rest of the tasks all have the same size
 }else{
  // caching will use all the threads
  UI nperthread=ncache/nthreads; nremnant=ncache%nthreads;  // # pieces in all threads, #threads with an extra piece
  nfulltasks=nperthread/(MAXAROWS/CACHEHEIGHT); endtasksize=nperthread%(MAXAROWS/CACHEHEIGHT); // # full tasks per thread, # cacheblocks in final task in each thread
  tailtasks=nremnant; tailtasks=endtasksize!=0?nthreads:tailtasks;  // number of tasks in the tail: if there is at least one block in all threads, nthreads; otherwise the number of single blocks in the remnant
  endtasksize++;  // if there is an extra cacheblock in some threads, extend the length of the last blocks to account for it.  If there is no extra cache block, tailtasks will be 0 and this won't matter
  endtasksize<<=CACHEHEIGHTX;  // convert # blocks at end to #rows at end
  nfulltasks*=nthreads;  // get total# tasks using the large block
  fulltasksize=MAXAROWS;  // in this branch, the big block is max for cachedmmult
 }
 CACHEMMSTATE ctx={.av=av,.wv=wv,.zv=zv,.m=m,.n=n,.p=p,.flgs=flgs,.nbigtasks={nfulltasks,nfulltasks+nremnant},.taskm={fulltasksize,endtasksize}};
   // number of full tasks, followed by number that have size 'endtasksize'.  Later tasks have size endtasksize-CACHEHEIGHT
 R !jtjobrun(jt,cachedmmultx,&ctx,nfulltasks+tailtasks,0);  // go run the tasks - default to threadpool 0.  Switch return from job semantice to JE error samantics - 0 if error, 1 if OK
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
 mvc(m*n*sizeof(D),zv,MEMSET00LEN,MEMSET00);
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
 if(unlikely(((at|wt)&XNUM+RAT+QP+INT2+INT4)!=0))R dfv2(z,a,w,atop(slash(ds(CPLUS)),qq(ds(CSTAR),v2(1L,AR(w)))));  // On nonbasic types, execute as +/@(*"(1,(wr)))
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
 if(AN(z)==0)R z;  // return without computing if result is empty.  Type stays FL
 if(!p){mvc(AN(z)<<bplg(AT(z)),AV(z),MEMSET00LEN,MEMSET00); R z;}  // if dot-products are all 0 length, set them all to 0
 // If either arg is atomic, reshape it to a list
 if(!ar!=!wr){if(ar)RZ(w=reshape(sc(p),w)) else RZ(a=reshape(sc(p),a));}
 p1=p-1;
 // Perform the inner product according to the type
 switch(CTTZNOFLAG(t)){
 case B01X:
  if(0==(n&(SZI-1))||!SY_ALIGN){A tt;B*u,*v,*wv;I nw,*x,*zv;UC*c,*tc;UI*d,*ti,*vi;
   nw=(n+SZI-1)>>LGSZI;
   GATV0(tt,INT,nw,1); ti=(UI*)AV1(tt); tc=(UC*)ti;
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
   // INT product is problematic, because it is used for many internal purposes, such as #., indexing of { and m}, and constants with a base.  For these uses,
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
     if((UI)probsize < (UI)FLOAT16TOFLOAT(JT(jt,igemm_thres))){RZ(a=ccvt(FL,a,0)); RZ(w=ccvt(FL,w,0)); cachedmmult(jt,DAV(a),DAV(w),DAV(z),m,n,p,0);}  // Do our matrix multiply - converting   TUNE
     else {
      // for large problem, use BLAS
      mvc(m*n*sizeof(D),DAV(z),MEMSET00LEN,MEMSET00);
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
      mvc(m*n*sizeof(D),DAV(z),MEMSET00LEN,MEMSET00);
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
     mvc(m*n*sizeof(D),DAV(z),MEMSET00LEN,MEMSET00);
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
     mvc(m*n*sizeof(D),DAV(z),MEMSET00LEN,MEMSET00);
     dgemm_nn(m,n,p,1.0,DAV(a),p,1,DAV(w),n,1,0.0,DAV(z),n,1);
    }else if(m&1){cachedmmult(jt,DAV(a),DAV(w),DAV(z),m,n,p,0);
    }else smallprob=1;
#endif
#else
   // not single column.  Choose the algorithm to use
#if C_AVX2 || EMU_AVX2
    smallprob=0;  // never use Dic method; but used to detect pick up NaN errors
    D *av=DAV(a), *wv=DAV(w), *zv=DAV(z);  //  pointers to sections
    I flgs=((AFLAG(a)>>(AFUPPERTRIX-FLGAUTRIX))&FLGAUTRI)|((AFLAG(w)>>(AFUPPERTRIX-FLGWUTRIX))&FLGWUTRI);  // flags from a or w
    if((UI)(m*n*(IL)p)>=(UI)FLOAT16TOFLOAT(JT(jt,dgemm_thres))){   // test for BLAS.  For AVX2 this should not be taken; for other architectures tuning is required
     mvc(m*n*sizeof(D),DAV(z),MEMSET00LEN,MEMSET00);
     dgemm_nn(m,n,p,1.0,DAV(a),p,1,DAV(w),n,1,0.0,DAV(z),n,1);
    } else {
     smallprob=1^cachedmmult(jt,av,wv,zv,m,n,p,flgs);  // run the cached mult; if NaN error, remember that fact
    }
#else
    I probsize = (m-1)*n*(IL)p;  // This is proportional to the number of multiply-adds.  We use it to select the implementation.  If m==1 we are doing dot-products; no gain from fancy code then
    if(!(smallprob = (m<=4||probsize<1000LL))){  // if small problem, avoid the startup overhead of the matrix version  TUNE
     if((UI)probsize < (UI)FLOAT16TOFLOAT(JT(jt,dgemm_thres)))
      cachedmmult(jt,DAV(a),DAV(w),DAV(z),m,n,p,((AFLAG(a)>>(AFUPPERTRIX-FLGAUTRIX))&FLGAUTRI)|((AFLAG(w)>>(AFUPPERTRIX-FLGWUTRIX))&FLGWUTRI));  // Do our one-core matrix multiply - real   TUNE this is 160x160 times 160x160.  Tell routine if uppertri
     else{
      // If the problem is really big, use BLAS
      mvc(m*n*sizeof(D),DAV(z),MEMSET00LEN,MEMSET00);
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
    if((UI)probsize<(UI)FLOAT16TOFLOAT(JT(jt,zgemm_thres))){smallprob=1^cachedmmult(jt,DAV(a),DAV(w),DAV(z),m,n*2,p*2,((AFLAG(a)>>(AFUPPERTRIX-FLGAUTRIX))&FLGAUTRI)|((AFLAG(w)>>(AFUPPERTRIX-FLGWUTRIX))&FLGWUTRI)|FLGCMP);}  // Do the fast matrix multiply - complex.  Change widths to widths in D atoms, not complex atoms  TUNE  this is 130x130 times 130x130
    else {
      // Large problem - start up BLAS
      mvc(2*m*n*sizeof(D),DAV(z),MEMSET00LEN,MEMSET00);
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
  default: c0=c1=-1; g=ds(d); RZ(dfv2(x0,num(0),w,g)); RZ(dfv2(x1,num(0),w,g)); break;
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
  c0=c1=-1; g=ds(d); RZ(dfv2(x0,num(0),w,g)); RZ(dfv2(x1,num(0),w,g));
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
 if(((UI)(AT(a)&AT(w)&B01)>SGNTO0((SGNIFSPARSE(AT(a)|AT(w)))))&&(-AN(a)&-AN(w)&-(FAV(gs)->flag&VISATOMIC2))<0&&CSLASH==FAV(fs)->id&&  // fs is c/
     (c=FAV(FAV(fs)->fgh[0])->id,BETWEENC((c^(CPLUSDOT^CEQ)),CSTARDOT,CNE)))R ipbx(a,w,c,FAV(gs)->id);  // [+.*.~:]/ . boolean   swap = and +., then test for range
 r=lr(gs);   // left rank of v
 A z; R dfv2(z,a,w,atop(fs,qq(gs,v2(r==RMAX?r:1+r,RMAX))));  // inner product according to the Dic
}


static F1(jtminors){A d,z;
 RZ(d=apvwr(3L,-1L,1L)); AV(d)[0]=0;
 R drop(d,dfv2(z,num(1),w,bsdot(ds(CLEFT))));  // 0 0 1 }. 1 [\. w 
}

DF1(jtdet){A fs=FAV(self)->fgh[0]; A gs=FAV(self)->fgh[1]; AF f1=FAV(fs)->valencefns[0]; A h=FAV(self)->fgh[2];I c,r,*s;
 ARGCHK1(w);
 r=AR(w); s=AS(w);
 A z; if(h&&1<r&&2==s[r-1]&&s[r-2]==s[r-1])R dfv1(z,w,h);
 F1RANK(2,jtdet,self);
 c=2>r?1:s[1];
 R !c ? dfv1(z,mtv,slash(gs)) : 1==c ? CALL1(f1,ravel(w),fs) : h && c==s[0] ? gaussdet(w) : detxm(w,self); 
}

DF1(jtdetxm){A z; R dotprod(IRS1(w,0L,1L,jthead,z),det(minors(w),self),self);}
     /* determinant via expansion by minors. w is matrix with >1 columns */

F2(jtdot){F2PREFIP;A f,h=0;AF f2=jtdotprod;C c,d;
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

// general LU decomp using generic arithmetic
DF1(jtludecompg){F1PREFIP;PROLOG(823);
 F1RANK(2,jtludecompg,self)  // if rank > 2, call rank loop
 ASSERT(AR(w)>=2,EVRANK);   // require rank>=2
 ASSERT(AS(w)[0]==AS(w)[1],EVLENGTH);  // matrix must be square
 A luvb; ASSERT(luvb=jtfindnameinscript(jt,"~addons/dev/lu/lu.ijs","Lu_j_",VERB),EVNONCE)   // error if undefined or not verb
 // Apply Lu_j_ to the input argument
 A z=unquote(w,luvb,luvb);  // monadic call to unquote
 // if there was an error, save the error code and recreate the error at this level, to cover up details inside the script
 if(jt->jerr){I e=jt->jerr; RESETERR; jsignal(e);}
 R z;
}


// 128!:10 LU decomposition for square real arrays LUP=A
// returns permutation ; L+U-I (Doolittle form)
// the ith element of the permutation is the original row of row i of LU
// Bivalent.  a, if given, is the sequence of thresholds to try
DF2(jtludecomp){F1PREFIP;PROLOG(823);
 static D pthresh[2]={1e-6,0}, *pivotthresh; I npivotthresh, curpivotthreshx;  // list of successive thresholds for pivots, last one usually 0.0
 if(AT(w)&NOUN){ASSERT(AR(a)<=1,EVRANK); ASSERT(AN(a)>0,EVLENGTH) if(unlikely(!(AT(a)&FL)))RZ(a=ccvt(FL,a,0)); pivotthresh=DAV(a); npivotthresh=AN(a);}else{w=a; pivotthresh=pthresh; npivotthresh=sizeof(pthresh)/sizeof(pthresh[0]);}
#if C_AVX2 || EMU_AVX2
 // We operate on 4x4 blocks of A, which we transform into 4x4 blocks of LU.  The ravel of each LU block is stored for cache ease,
 // and the U blocks are ordered in transpose form to speed up the dot-product operations.
 // For each row of A, we subtract the matrix product of (preceding U) * (preceding L) to get the new U row and the first L entry L0.
 // Then we go down the column of A, multiplying by 1/L0, to get the new L column.  Repeat for each gamma-shaped LU row/col.
 // Processing in 4x4 blocks means we have one code for the first 4x4 (the corner), another for L blocks, a third for U.  The
 // code to multiply (preceding U) * (preceding L) is common between all three.
 // Allocate area for the cacheblocks of L and U
#define BLKSZ 4  // size of cache block
#define LGBLKSZ 2  // lg(BLKSZ)
 I nzeroblocks=0;  // number of zero blocks created.  If negative, we have given up on zero blocks
 B lookfor0blocks;  // set if we think it's worthwhile to check for sparse array
 ASSERT(AR(w)>=2,EVRANK);   // require rank>=2
 ASSERT(AS(w)[0]==AS(w)[1],EVLENGTH);  // matrix must be square
 if((AT(w)&SPARSE+B01+INT+FL)<=0)R jtludecompg(jt,w,DUMMYSELF);  // if not real float type, use general version
 if(unlikely(!(AT(w)&FL)))RZ(w=ccvt(FL,w,0));
 I wn=AS(w)[0];  // n=size of square matrix
 // Allocate the result
 A z; GATV(z,FL,wn*wn,2,AS(w)) if(unlikely(wn==0))R jlink(mtv,z);  // if empty result, return fast.  Now nr must be >0
 // Allocate the result permutation, an index vector
 A aresultperm; RZ(aresultperm=apvwr(wn,0,1)); I *resultperm=IAV1(aresultperm);  // we overstore the permutation to a BLKSZ bdy, but that's ok, it's allocated
 // Allocate the areas where we calculate the permutations: one for input, one for output
 A asrclines; GAT0(asrclines,INT,4,1) I *srclines=IAV1(asrclines);  // accumulates the source lines that are permuted into the next block; AN() and AS[0] tell how many are needed
 A acalcperm; RZ(acalcperm=eval("(~.@, ,: ~.@,@|.)@(#\"1~ ~:/)@,:"))   // verb to calculate src/dest lines#s for permutation
 I nr=(wn+BLKSZ-1)>>LGBLKSZ;  // nr=total # blocks on a side (including partial ones)
#define CORNERBLOCK(rc) (cb+(rc)*(nr+1))  // address of corner cblock in row&col rc
#define LBLOCK(r,c) (cb+(r)*nr+(c))  // address of L cblock for row r col c (0<=c<r)
#define UBLOCK(r,c) (cb+(nr-(c))*nr - (c) + (r)) // address of U cblock for row r col c (0<=r<c) - transposed order going left-to-right, starting after corner block
#define LBIT(r,c) (lb + (((nr+63)>>6)+2)*(r) + ((c)>>6))  // address of 64-bit word containing L bitmask bit for (r,c) (0<=c<r)  stride of ((nr+63)>>6)+2, advances east
#define LBITX(r,c) ((c)&(BW-1))  //  index of bit (r,c) in selected word
#define UBIT(r,c) (ub - (((nr+63)>>6)+2)*(c) - ((r)>>6))  // address of 64-bit word containing U bitmask bit for (r,c) (0<=r<c)  stride of -(((nr+63)>>6)+2), advances northwest
#define UBITX(r,c) ((r)&(BW-1))  //  index of bit (r,c) in selected word
 // Allocate the cblock/bitmap area, rank 1 so it stays cache-aligned
 A cba; GA10(cba,FL,nr*nr*BLKSZ*BLKSZ+nr*(((nr+63)>>6)+2)+nr) D (*cb)[BLKSZ][BLKSZ]=(D (*)[BLKSZ][BLKSZ])DAV(cba);  // cb=pointer to start of cblock area
 UI *lb=(I*)(cb+nr*nr); UI *ub=lb+(((nr+63)>>6)+2)*(nr)-1;  // lb=pointer to L bitmap; ub=pointer to (end of) U bitmap
 UI4 *rleb=(UI4 *)(ub+1);  // rleb is the workarea where we build (#on,#off) run-lengths for each dot-product

 __m256i endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-wn)&(BLKSZ-1))));   // mask for storing last block in a row
 __m256d ones = _mm256_set1_pd(1.0);   // numerator of reciprocals, or value for an identity matrix
 __m256d sgnbit=_mm256_broadcast_sd((D*)&Iimin);  // sign bit
 D maxoobpivot;  // maximum unacceptable pivot value
 __m256d minoobrecip=_mm256_set1_pd(1/(maxoobpivot=pivotthresh[0]));   // smallest out-of-bounds reciprocal for a pivot
 D *wv=DAV(w), *zv=DAV(z);  // pointer to corner block of the input
 I r;  // size-1 of ring being processed.  The ring has 2r+1 cblocks.  The corner block is (nr-1-r,nr-1-r)
 for(r=nr-1;r>=0;--r){
  __m256d a00,a01,a02,a03,a10,a11,a12,a13,nexta0,nexta1,nexta2,nexta3,recips;  // double accumulators for the 4x4 block; staging area for A data; reciprocals to use to propagate down the column of L
  // process one ring: the corner block, a column of L, a row of U
  D (*scv)[BLKSZ][BLKSZ];  // where the current block will be stored: c/l going south, u going northwest
  D __attribute__((aligned(CACHELINESIZE))) linv[BLKSZ][BLKSZ], uinv[BLKSZ][BLKSZ];  // 'inverses' of the corner block, used to calculate L and U blocks 
  // initialize A[nr-1-r,nr-1-r] (pointed to by wclv) into nexta0..3
  // on every corner block, decide the status of sparse checking.  For the first few rings, we look for zero blocks and count them.
  // We also make a decision before each ring whether it is worthwhile to look for zeros.  After a few rings, we give up on counting if we haven't seen many
  // zero blocks.  Since each zero block zaps multiple dot-product blocks, it doesn't take many to be worthwhile
  lookfor0blocks=nzeroblocks*64>(nr-1-r)*(2*r+1);    // # blocks so far is (nr-1-r) * (2nr-1-2*(nr-1-r)).  If 1 in 64 is 0, look for it.  But never first time
  nzeroblocks|=(lookfor0blocks|((nr-1-r)!=10))-1;    // on the 10th ring, disable zero checks if we aren't using them
  scv=LBLOCK(nr-1-r,nr-1-r);   // start store at corner block.  It will not advance until pivots have been found
  curpivotthreshx=0;  // start every ring looking for large pivots, even if we had to relax the criterion for an earlier ring

 restartring:; // *** restart point after permutation has been updated.  We restart the ring at the corner, which will succeed
  D (*llv)[BLKSZ][BLKSZ], (*luv)[BLKSZ][BLKSZ], (*prechv)[BLKSZ][BLKSZ];  // start point of dot-products (both going L-to-R), startpoint of next dot-product (first L block)
  UI *lbv0, *ubv0;  // point to the bit vectors for the corner position.  These pointers are advanced after we finish each block, to handle the dot-product for the next block
  // fetch the corner block from A using permutation, row and col are nr-1-r
  if(r>0){
   // normal case when A(nr-1-r,nr-1-r) is a full block
   nexta0=_mm256_loadu_pd(wv+resultperm[(nr-1-r)*BLKSZ]*wn+(nr-1-r)*BLKSZ); nexta1=_mm256_loadu_pd(wv+resultperm[(nr-1-r)*BLKSZ+1]*wn+(nr-1-r)*BLKSZ); nexta2=_mm256_loadu_pd(wv+resultperm[(nr-1-r)*BLKSZ+2]*wn+(nr-1-r)*BLKSZ); nexta3=_mm256_loadu_pd(wv+resultperm[(nr-1-r)*BLKSZ+3]*wn+(nr-1-r)*BLKSZ);  // lots of cache misses here
  }else{
   // last block, containing 1-4 rows/cols.  Read in the valid bits, replacing the others with 0 so they don't get selected as pivots
   nexta0=_mm256_maskload_pd(wv+resultperm[(nr-1-r)*BLKSZ]*wn+(nr-1-r)*BLKSZ,endmask); 
   nexta1=((wn-1)&(BLKSZ-1))>0?_mm256_maskload_pd(wv+resultperm[(nr-1-r)*BLKSZ+1]*wn+(nr-1-r)*BLKSZ,endmask):_mm256_setzero_pd();
   nexta2=((wn-1)&(BLKSZ-1))>1?_mm256_maskload_pd(wv+resultperm[(nr-1-r)*BLKSZ+2]*wn+(nr-1-r)*BLKSZ,endmask):_mm256_setzero_pd();
   nexta3=((wn-1)&(BLKSZ-1))>2?_mm256_maskload_pd(wv+resultperm[(nr-1-r)*BLKSZ+3]*wn+(nr-1-r)*BLKSZ,endmask):_mm256_setzero_pd();
   AN(asrclines)=AS(asrclines)[0]=((wn-1)&(BLKSZ-1))+1;   // in last ring, we need fewer pivots
  }

  I wrappedcorner=0;  // set if we have tried all 'corner' blocks once and are looking at them again for the same pivots
  I permblkofst=0;  // number of perm blocks that we processed without finding BLKSZ pivots
  I ngoodperma=0;  // number of good values in perma.  Used only on corner blocks
  I scanstart=0;  // permblkofst at the time the current search for a pivot was started.  If permblkofst gets back to here there is no pivot
  I r0;  // index of corner-, L- or U-block being processed: -r for corner, -r+1..0 for L, 1..r for U
  for(r0=-r;r0<=r;++r0){  // corner then L then U
   __m256d perma[7];  // for perm calc, we save the good values from previous rows in the first up to 3 locations, and new candidates in the next 4 locs
nextperm:;  // *** loopback point to continue search for pivots in the next column down.  permblkofst and ngoodperma are set to indicate where we are in the search, and perma holds values inherited from earlier blocks
   // move the next A block into the accumulators a00..a03
   a00=nexta0; a01=nexta1; a02=nexta2; a03=nexta3;
   // unroll fetching the FOLLOWING block of A, using permutation.  This has to be there for the dot-product for the next block
   // This is the only time we fetch from A, so it will probably miss to L3.  No matter, because we have a whole block of processing before we get back around
   // advance to following block and fetch it.
   // Since the corner block was fetched first, the block we are fetching can go out on only one side.  We fill with zeros on the right, identity diagonals down (in case the block is used as a coner block in pivot search)
   if(r0==-r){  // searching for pivots, including the case of r=0
    // we are about to process a 'corner' block, possibly in the search for pivots.  The next block to fetch will always be an L block, possibly incomplete, and possibly wrapped around
    // if we process the first corner block successfully, we continue on with the first row of L.  If there is a permutation we will restart and discard the last prefetch.
    I nextpermblkofst=permblkofst+1; nextpermblkofst=permblkofst==r?0:nextpermblkofst; wrappedcorner=(permblkofst==0)&&(ngoodperma!=0)?1:wrappedcorner;  // offset of next row to try as corner.  Set if we are rescanning blocks that might have been selected already
    if(likely(r!=0)){  // if r!=0, we are fetching the next 'corner', the L row below.  If r=0, we are on the last corner, and we just keep it in nexta[0-3].  nextpermblkofst was always set to 0
     if(nextpermblkofst<r){  // if next row is not last row of L
      nexta0=_mm256_loadu_pd(wv+resultperm[(nr+r0+nextpermblkofst-1)*BLKSZ]*wn+(nr-1-r)*BLKSZ); nexta1=_mm256_loadu_pd(wv+resultperm[(nr+r0+nextpermblkofst-1)*BLKSZ+1]*wn+(nr-1-r)*BLKSZ);
        nexta2=_mm256_loadu_pd(wv+resultperm[(nr+r0+nextpermblkofst-1)*BLKSZ+2]*wn+(nr-1-r)*BLKSZ); nexta3=_mm256_loadu_pd(wv+resultperm[(nr+r0+nextpermblkofst-1)*BLKSZ+3]*wn+(nr-1-r)*BLKSZ);  // fetch next block
     }else{  // prefetching the very last 'corner'.  Make the extra rows 0 so they will never be selected as valid pivots
      nexta0=_mm256_loadu_pd(wv+resultperm[(nr-1)*BLKSZ]*wn+(nr-1-r)*BLKSZ);
      nexta1=((wn-1)&(BLKSZ-1))>0?_mm256_loadu_pd(wv+resultperm[(nr-1)*BLKSZ+1]*wn+(nr-1-r)*BLKSZ):_mm256_setzero_pd();
      nexta2=((wn-1)&(BLKSZ-1))>1?_mm256_loadu_pd(wv+resultperm[(nr-1)*BLKSZ+2]*wn+(nr-1-r)*BLKSZ):_mm256_setzero_pd();
      nexta3=((wn-1)&(BLKSZ-1))>2?_mm256_loadu_pd(wv+resultperm[(nr-1)*BLKSZ+3]*wn+(nr-1-r)*BLKSZ):_mm256_setzero_pd();
     }
    }
    llv=LBLOCK(nr-1-r+permblkofst,0); luv=UBLOCK(0,nr-1-r), prechv=LBLOCK(nr-1-r+nextpermblkofst,0);  // start point of dot-products (both going L-to-R), prefetch point of next dot-product (next L block)
    lbv0=LBIT(nr-1-r+permblkofst,0); ubv0=UBIT(0,nr-1-r);  // point to the bit vectors for the 'corner' position.

   }else if(r0<=0){  // L blocks
    llv=prechv; lbv0+=((nr+63)>>6)+2; scv+=nr;  // advance to the prefetched (possibly partial) L block, 1 row down.  Advance store pointer down too
    if(r0<-1){   // processing L row with 2 later L rows.  prefetch next full L block, which is row (nr-1-r) + r0-(-r) + 1, col (nr-1-r)
     nexta0=_mm256_loadu_pd(wv+resultperm[(nr+r0)*BLKSZ]*wn+(nr-1-r)*BLKSZ); nexta1=_mm256_loadu_pd(wv+resultperm[(nr+r0)*BLKSZ+1]*wn+(nr-1-r)*BLKSZ); nexta2=_mm256_loadu_pd(wv+resultperm[(nr+r0)*BLKSZ+2]*wn+(nr-1-r)*BLKSZ); nexta3=_mm256_loadu_pd(wv+resultperm[(nr+r0)*BLKSZ+3]*wn+(nr-1-r)*BLKSZ);  // lots of cache misses here
     prechv+=nr;  // move L down 1 row, keep precharging L
    }else if(r0==-1){  // processing next-last row of L.  prefetch partial L block, filled with 0 (immaterial)   (nr-1-r) + r0-(-r) + 1, col (nr-1-r)
     nexta0=_mm256_loadu_pd(wv+resultperm[(nr-1)*BLKSZ]*wn+(nr-1-r)*BLKSZ);
     nexta1=((wn-1)&(BLKSZ-1))>0?_mm256_loadu_pd(wv+resultperm[(nr-1)*BLKSZ+1]*wn+(nr-1-r)*BLKSZ):_mm256_setzero_pd();
     nexta2=((wn-1)&(BLKSZ-1))>1?_mm256_loadu_pd(wv+resultperm[(nr-1)*BLKSZ+2]*wn+(nr-1-r)*BLKSZ):_mm256_setzero_pd();
     nexta3=((wn-1)&(BLKSZ-1))>2?_mm256_loadu_pd(wv+resultperm[(nr-1)*BLKSZ+3]*wn+(nr-1-r)*BLKSZ):_mm256_setzero_pd();
     prechv+=nr;  // move L down 1 row, keep precharging L
    }else if(r0==0){   // processing last row of L.  Prefetch full U block, row (nr-1-r), col (nr-1-r)+r0+1
     nexta0=_mm256_loadu_pd(wv+resultperm[(nr-1-r)*BLKSZ]*wn+(nr-r+r0)*BLKSZ); nexta1=_mm256_loadu_pd(wv+resultperm[(nr-1-r)*BLKSZ+1]*wn+(nr-r+r0)*BLKSZ); nexta2=_mm256_loadu_pd(wv+resultperm[(nr-1-r)*BLKSZ+2]*wn+(nr-r+r0)*BLKSZ); nexta3=_mm256_loadu_pd(wv+resultperm[(nr-1-r)*BLKSZ+3]*wn+(nr-r+r0)*BLKSZ);  // lots of cache misses here
     prechv=LBLOCK(nr-1-r,0);  // move L down 1 row, but precharge the first row of L since we are going to snap back
    }
   }else{   // U blocks
    if(r0==1){   // switching from L to U.
     llv=prechv; prechv=luv-(nr+1); lbv0=LBIT(nr-1-r,0); scv=UBLOCK(nr-1-r,nr-1-r+1)+nr+1;  // move L to top row, where it will remain.  Init precharge to first U col in ring.  Init store pointer to 1st col of U, prebiased
    }
    luv=prechv; ubv0-=((nr+63)>>6)+2; scv-=nr+1;  // reset L pointers to top row; advance U pointers; advance store pointer to first/next col of U
    if(r0<r-1){  // processing col of U before the next-last.  Prefetch full U block, row (nr-1-r), col (nr-1-r)+r0+1
     nexta0=_mm256_loadu_pd(wv+resultperm[(nr-1-r)*BLKSZ]*wn+(nr-r+r0)*BLKSZ); nexta1=_mm256_loadu_pd(wv+resultperm[(nr-1-r)*BLKSZ+1]*wn+(nr-r+r0)*BLKSZ); nexta2=_mm256_loadu_pd(wv+resultperm[(nr-1-r)*BLKSZ+2]*wn+(nr-r+r0)*BLKSZ); nexta3=_mm256_loadu_pd(wv+resultperm[(nr-1-r)*BLKSZ+3]*wn+(nr-r+r0)*BLKSZ);  // lots of cache misses here
     prechv-=nr+1;  // leave L unchanged, precharge
    }else if(r0==r-1){   // processing next-last col of U.  Prefetch partial U block, row (nr-1-r), col is (nr-1)
     nexta0=_mm256_maskload_pd(wv+resultperm[(nr-1-r)*BLKSZ]*wn+(nr-1)*BLKSZ,endmask); nexta1=_mm256_maskload_pd(wv+resultperm[(nr-1-r)*BLKSZ+1]*wn+(nr-1)*BLKSZ,endmask); nexta2=_mm256_maskload_pd(wv+resultperm[(nr-1-r)*BLKSZ+2]*wn+(nr-1)*BLKSZ,endmask); nexta3=_mm256_maskload_pd(wv+resultperm[(nr-1-r)*BLKSZ+3]*wn+(nr-1)*BLKSZ,endmask);
     prechv-=nr+1;  // leave L unchanged, precharge
    }   // otherwise processing LAST col of U (which might also be the first or second).  No prefetch, no precharge
   }

   // produce a block of LU: either corner(nr-1-r,nr-1-r) or L(r0,nr-1-r) or U(nr-1-r,r0)
   // The next 4x4 of A has been started into registers nexta0..nexta3
   // First, take A[x,y]-L*U
   // **************** This is the O(n^3) part, where the time is spent in this algorithm ***********************
   if(nr-1-r){  // skip the dot-product if it is empty.  This makes the tests inside the loop faster too
    // Create & use sparse sections
    // we go through the sparse maps and convert the OR of the rows & columns into a sequence of (#non0,#0) pairs which we process later
    UI4 *runv=rleb;  // pointer to next run
    if(0&&lookfor0blocks){  // if we think it's worthwhile to skip over zero blocks...  scaf must permute the L bit vectors if we are to enable this
     UI *lbv=lbv0, *ubv=ubv0;  // point to the start of the bit vectors
     UI polarity=~0;  // 00.00 if we are looking for 0s, 11..11 if we are looking for 1s.  We start looking for skipped blocks (1s)
     I bitsleft=nr-1-r, lensofar=0, bitsinstack;  // number of bits left to process, length carried over from previous maskword
     UI bitstack;
     while(1){  // loop to build the run lengths
      // we are scanning qwords looking for a mismatch, where the bits don't equal 'polarity'.  Switch the polarity so we are always looking for 0
      NOUNROLL while((bitstack=(*lbv++|*ubv--)^polarity)==0){lensofar+=BW; if((bitsleft-=BW)<=0){lensofar+=bitsleft; goto finrle;}};  //  read until we hit end or a desired bit.  If end, skip to end of loop
      // we found a word with a mismatch.  Scan it.  lensofar is the number of matches we have found, bitsinstack is #valid bits in stack
      bitsinstack=BW; bitsinstack=bitsleft<BW?bitsleft:bitsinstack; bitsleft-=bitsinstack;  // init # valid bits in bitstack, never more than bitsleft; predecrement bitstack to len AFTER this bitstack is processed
      while(1){  // build the run of non-polarity
       I runlen=CTTZI(bitstack); runlen=bitstack==0?bitsinstack:runlen;  // Find the lowest 1 bit
       lensofar+=runlen;  // presumptively add the bits to the run
       if((bitsinstack-=runlen)<=0){lensofar+=bitsinstack; if(bitsleft<=0)goto finrle; break;}  // bitstack exhausted: add valid bits to run.  If there are no more bits, exit, otherwise continue in next word
       // we found the end of a run inside the bitstring.  Out the run, change polarity, continue look
       *runv++=lensofar; lensofar=0;  // write out run, clear length of new run
       bitstack>>=runlen; bitstack=~bitstack; polarity=~polarity;   // remove the old bits; change polarity to turn the 0s to 1s
      }
     }
finrle: ;
     *runv++=lensofar;  // out the last run
    }else{
     *runv++=0; *runv++=nr-1-r;  // go through all the blocks up to the current ring in one run, which is known not to be empty
    }
    UI nruns=(runv-rleb)>>1;  // number of runs, which may have only the starting component or be empty
    runv=rleb;  // point to length of first run
    // take L*U for a 4x(4(nr-1-r)) * (4(nr-1-r)x4), giving a 4x4 result, in registers.  Also, prefetch the next row/col of U/L (the other is reused).
    // The full read bandwidth of L1 is taken with reading arguments, and the full write bandwidth is taken by the prefetch
    // *llv is the horizontal L-strip, *luv is the horizontal U-strip, *prechv is the first block to prefetch.  We prefetch one block
    // for each block we process
    // To avoid lots of casting we use D* in this loop.  Each loop handles BLKSZ^2 atoms stored in row-major order
    // establish pointers & offsets for the args.  We increment only the L pointer & use indexes for the rest
    D *lvv=(D*)llv; I uofst=(D*)luv-lvv, pofst=(D*)prechv-lvv;  // the 2 args+prefetch: one pointer, 2 offsets
    a10=_mm256_setzero_pd(); a11=_mm256_setzero_pd(); a12=_mm256_setzero_pd(); a13=_mm256_setzero_pd(); // clear the uninitialized accumulators
    while(nruns){
     lvv+=*runv++*BLKSZ*BLKSZ;  // skip over the zeros.
     UI4 ndp=*runv++;  // get the length of the run
     do{
      __m256d tmp;  // where we save a row of U to multiply by 4 scalars from L
      tmp=_mm256_loadu_pd(lvv+uofst);  // read U00-U03
      a00=_mm256_fnmadd_pd(_mm256_set1_pd(lvv[0]),tmp,a00); a01=_mm256_fnmadd_pd(_mm256_set1_pd(lvv[4]),tmp,a01); a02=_mm256_fnmadd_pd(_mm256_set1_pd(lvv[8]),tmp,a02); a03=_mm256_fnmadd_pd(_mm256_set1_pd(lvv[12]),tmp,a03);
      tmp=_mm256_loadu_pd(lvv+uofst+4);  // read U10-U13
      a10=_mm256_fnmadd_pd(_mm256_set1_pd(lvv[1]),tmp,a10); a11=_mm256_fnmadd_pd(_mm256_set1_pd(lvv[5]),tmp,a11); a12=_mm256_fnmadd_pd(_mm256_set1_pd(lvv[9]),tmp,a12); a13=_mm256_fnmadd_pd(_mm256_set1_pd(lvv[13]),tmp,a13);
      PREFETCH2(lvv+pofst);  // prefetch for the next loop.  Repeating arg stays in L1, nonrepeating in L2
      tmp=_mm256_loadu_pd(lvv+uofst+8);  // read U20-U23
      a00=_mm256_fnmadd_pd(_mm256_set1_pd(lvv[2]),tmp,a00); a01=_mm256_fnmadd_pd(_mm256_set1_pd(lvv[6]),tmp,a01); a02=_mm256_fnmadd_pd(_mm256_set1_pd(lvv[10]),tmp,a02); a03=_mm256_fnmadd_pd(_mm256_set1_pd(lvv[14]),tmp,a03);
      tmp=_mm256_loadu_pd(lvv+uofst+12);  // read U30-U33
      a10=_mm256_fnmadd_pd(_mm256_set1_pd(lvv[3]),tmp,a10); a11=_mm256_fnmadd_pd(_mm256_set1_pd(lvv[7]),tmp,a11); a12=_mm256_fnmadd_pd(_mm256_set1_pd(lvv[11]),tmp,a12); a13=_mm256_fnmadd_pd(_mm256_set1_pd(lvv[15]),tmp,a13);
      PREFETCH2(lvv+pofst+8);  // prefetch for the next loop
      lvv+=BLKSZ*BLKSZ;  // advance loop pointer/counter
     }while(--ndp);
     --nruns;
    }
   // ****************** end of O(n^3) part **************************
   // combine the accumulators into the 0 side
    a00=_mm256_add_pd(a00,a10); a01=_mm256_add_pd(a01,a11); a02=_mm256_add_pd(a02,a12); a03=_mm256_add_pd(a03,a13);  // this finishes the 16 dot-products
    // now a10..3 are free
   }  // end of dot-product block, executed except first time
   // A[x,y]-L*U (product over nr-1-r blocks) is now in the register block on the 0 side

   // We are solving L(x,0..n-1)*U(0..n-1,y)=A(x,y) for L(x,nr-1-r) and/or U(nr-1-r,y) where x,y>=nr-1-r and one is equal.  Because of the triangularity of L and U,
   // this reduces to
   // L(nr-1-r,0..nr-1-r-1)*U(0..nr-1-r-1,y) + L(nr-1-r,nr-1-r)*U(nr-1-r,y)=A(x,y)   for the row of U
   // L(x,0..nr-1-r-1)*U(0..x-1,nr-1-r) + L(x,nr-1-r)*U(nr-1-r,nr-1-r)=A(x,y)  for the column of L
   //
   // L(nr-1-r,nr-1-r)*U(nr-1-r,y) = A(x,y)-L*U   for row of U
   // L(x,nr-1-r)*U(nr-1-r,nr-1-r) = A(x,y)-L*U    for col of L
   //
   // The RHS is the dot-product D that we just calculated.  The formulas give 3 cases, which we solve in turn.
   // For x=y=nr-1-r, we have a corner block and we calculate L(nr-1-r,nr-1-r) and U(nr-1-r,nr-1-r) by the usual row operations
   // For y>nr-1-r we calculate U(nr-1-r,y)=L-1(nr-1-r,nr-1-r) * D but rather than a matrix multiply we perform the same row operations.
   // For x>nr-1-r we calculate L(x,r) = D * U-1(nr-1-r,nr-1-r), doing the matrix multiply
   //
   // When we calculate the corner block we also write out coefficients that will ne needed to calculate the other blocks of L and U.
   if(r0==-r){
    // corner block.

    // First row of U is set.  Alternate creating columns of L and rows of U

    // We handle permutation in two steps.  First, we check all 24 permutations of the a0x rows to see if there is one that works on all 4 columns.
    // if so, we use it, marking its permutation.  We are so thorough because it saves our doing another dot-product.

    // If no permutation of these 4 rows works, we keep as many as do work (0-3 rows) and go back to get 4 more to try
    // perma is the list of values inherited from earlier blocks in this column
    // ngoodperma is the number of values in perma inherited from earlier blocks
    
    UI permx=0; C perm=0xE4;  // index of perm we are using, and that perm
    static C perms8[25] = {0xE4,0xB4,0xD8,0x78,0x9C,0x6C,0xE1,0xB1,0xC9,0x39,0x8D,0x2D,0xD2,0x72,0xC6,0x36,0x4E,0x1E,0x93,0x63,0x87,0x27,0x4B,0x1B,~0x1B};   // perms in order of reversed bitpos, i. e. 3 2 1 0 first
    I bestnperma=ngoodperma; C bestperm=perm;  // best incomplete value so far

    // loop till we find a valid permutation
    _mm256_storeu_pd((D*)&perma[ngoodperma],a00); _mm256_storeu_pd((D*)&perma[ngoodperma+1],a01); _mm256_storeu_pd((D*)&perma[ngoodperma+2],a02); _mm256_storeu_pd((D*)&perma[ngoodperma+3],a03);  // save the input for possible perm calc
    if(unlikely(wrappedcorner)){
     // Our search for pivots has wrapped around to the original corner.  We are looking at corners for a second time.  In that rare situation we must take steps to ensure that we
     // do not repeat a row, because roundoff may cause it to be reselected (even though it is dependent).  To avoid that we zero the row completely if it has been used before. 
     DO(ngoodperma, if((srclines[i]>>LGBLKSZ)==(nr-1-r+permblkofst))perma[ngoodperma+(srclines[i]&(BLKSZ-1))]=_mm256_setzero_pd();)
    }
    while(1){
     if(likely(ngoodperma+permx==0)){
      // the usual case of the first shot at a corner block.  a00..a03 are ready to go
      recips=_mm256_div_pd(ones,a00);  // 1/U00 x x x  // start the first reciprocal
      if(_mm256_cvtsd_f64(_mm256_andnot_pd(sgnbit,a00))<=maxoobpivot)goto pivotmiss0;  // nothing else to do while waiting for recip, so check for quick exit
     }else{
      // we are looking for a permutation.  a00..a03 must be reloaded.  The first ngoodperma regs are loaded from perma[0..2]; the rest come from ngoodperma+(permutation value)
      I permtemp=perm;  // 
      I findex; findex=(permtemp&3)+ngoodperma; findex=ngoodperma>0?0:findex; permtemp=ngoodperma>0?permtemp:permtemp>>2; a00=_mm256_loadu_pd((D*)&perma[findex]);
      recips=_mm256_div_pd(ones,a00);  // 1/U00 x x x  // start the first reciprocal
      findex=(permtemp&3)+ngoodperma; findex=ngoodperma>1?1:findex; permtemp=ngoodperma>1?permtemp:permtemp>>2; a01=_mm256_loadu_pd((D*)&perma[findex]);
      findex=(permtemp&3)+ngoodperma; findex=ngoodperma>2?2:findex; permtemp=ngoodperma>2?permtemp:permtemp>>2; a02=_mm256_loadu_pd((D*)&perma[findex]);
      findex=(permtemp&3)+ngoodperma; a03=_mm256_loadu_pd((D*)&perma[findex]);
//       a01=_mm256_loadu_pd(&perma[1]); a02=_mm256_loadu_pd(&perma[2]); a03=_mm256_loadu_pd(&perma[3]);  // if there are inherited values, start with them
     }

     // This is the core calculation for the corner block/linv ***
     a01=_mm256_blend_pd(a01,_mm256_mul_pd(a01,recips),0b0001);  // a01 is L10 A11 A12 A13
     _mm256_storeu_pd(&linv[0][0],a01);    // write out U10/U00 x x x for calculating U
     a01=_mm256_blend_pd(a01,_mm256_fnmadd_pd(a00,_mm256_permute4x64_pd(a01,0b00000000),a01),0b1110);  // a01 is A1x-L10*A0x = L10 U11 U12 U13
     recips=_mm256_blend_pd(recips,_mm256_div_pd(ones,a01),0b0010);  // 1/U00 1/U11 x x

     a02=_mm256_blend_pd(a02,_mm256_fnmadd_pd(a00,_mm256_permute4x64_pd(_mm256_mul_pd(a02,recips),0b00000000),a02),0b1110);  // a02 is A20 (A21..A23 - L20*U01..U03)
     a02=_mm256_blend_pd(a02,_mm256_mul_pd(a02,recips),0b0011);  // a02 is L20 L21 A22' A23'
     _mm256_storeu_pd(&linv[1][0],a02);    // write out U20/U00 U21/U11 x x for calculating U
     a02=_mm256_blend_pd(a02,_mm256_fnmadd_pd(a01,_mm256_permute4x64_pd(a02,0b01010101),a02),0b1100);  // a02 is A2x-L20*A0x-L21*U1x = L20 L21 U22 U23
     recips=_mm256_blend_pd(recips,_mm256_div_pd(ones,a02),0b0100);  // 1/U00 1/U11 1/U22 x

     a03=_mm256_blend_pd(a03,_mm256_fnmadd_pd(a00,_mm256_permute4x64_pd(_mm256_mul_pd(a03,recips),0b00000000),a03),0b1110);  // a03 is A30 (A31..A33 - L30*U01..U03)
     a03=_mm256_blend_pd(a03,_mm256_fnmadd_pd(a01,_mm256_permute4x64_pd(_mm256_mul_pd(a03,recips),0b01010101),a03),0b1100);  // a03 is A30 (A31..A33 - L31*U11..U13)
     a03=_mm256_blend_pd(a03,_mm256_mul_pd(a03,recips),0b0111);  // a03 is L30 L31 L32 A33''
     _mm256_storeu_pd(&linv[2][0],a03);    // write out U30/U00 U31/U11 U32/U22 for calculating U
     a03=_mm256_blend_pd(a03,_mm256_fnmadd_pd(a02,_mm256_permute4x64_pd(a03,0b010101010),a03),0b1000);  // a03 is A3x-L30*A0x-L31*U1x-L32*U3x = L30 L31 L32 U33
     recips=_mm256_blend_pd(recips,_mm256_div_pd(ones,a03),0b1000);  // 1/U00 1/U11 1/U22 1/U33
     // end of corner-block calc ***

     // if the current permutation is valid, exit to process using it
     __m256d recipbad=_mm256_cmp_pd(_mm256_andnot_pd(sgnbit,recips),minoobrecip,_CMP_GE_OQ);  // where we build inverse lines to write out
     if(_mm256_testz_pd(recipbad,sgnbit))break;  // testz is 1 if all comparisons false, i. e. all recips in range
       // we wait to test until we have all the reciprocals because we expect few permutations.  If there are 0 pivots they may generate NaNs in the L values, but not in recips

     // falling through, we didn't get all the pivots: we must search for a permutation.

     // if the current permutation is a new best, remember it
     I ngood=CTTZI(_mm256_movemask_pd(recipbad));  // number of leading OK pivots
     if(ngood==AN(asrclines))break;  // if we have enough pivots, use them
     bestperm=ngood>bestnperma?perm:bestperm; bestnperma=ngood>bestnperma?ngood:bestnperma;  // if new best, remember perm & how many good pivots

     if(0){pivotmiss0: ngood=0;}  // come here when the first column is invalid as a pivot
     // advance to next permutation.   skip anything that repeats the first failing index, since that will fail in the same place
     do{++permx;}while((((perms8[permx]^perm)>>(2*ngood))&3)==0); perm=perms8[permx];  // find first perm that doesn't repeat the failure
     if(permx<sizeof(perms8)/sizeof(perms8[0])-1)continue;  // if there is another permutation to check, go try it

     // here we were unable to find a full set of pivots with any ordering of the input block.  Update the permutation for the pivots we did find, and go back to dot-product to get the next batch of rows.
     if(bestnperma>ngoodperma)scanstart=permblkofst;  // if we made any improvement, restart the scan for the next improvement
     DO(bestnperma-ngoodperma, perma[6-i]=perma[ngoodperma+((bestperm>>(2*i))&3)];)  // save selected pivots at end of perma, in reverse order of use
     I i; for(i=6;ngoodperma<bestnperma;--i,++ngoodperma,bestperm>>=2){perma[ngoodperma]=perma[i]; srclines[ngoodperma]=(nr-1-r+permblkofst)*BLKSZ+(bestperm&3);} // install the selected pivots, and the source of each found row

     if(++permblkofst>r)permblkofst=0;   // step south to try the next 'corner' block, wrapping around at end
     if(permblkofst==scanstart){
      // we have tried all the L blocks without finding pivots.  We relax the pivot threshold (it will stay relaxed for the rest of the run)
      ++curpivotthreshx;  // advance to next threshold
      ASSERT(curpivotthreshx<npivotthresh,EVDOMAIN)  // if we run out of thresholds, it's a singular matrix
      minoobrecip=_mm256_set1_pd(1/(maxoobpivot=pivotthresh[curpivotthreshx]));   // reset the pivot thresholds
     }
     goto nextperm;  // recalculate next 'corner' from the next L block.  ngoodperma is set
    }

    // success - we found pivots, using permutation (perm).
    if(unlikely(permblkofst+ngoodperma+permx!=0)){  // identity perm is match on the first shot at the threshold: first corner block in this column, no pivots from earlier blocks, no permutation in this corner block
     // the permutation is not identity. store the part that was added here, apply it to the rows of L, and restart the ring
     for(;ngoodperma<AN(asrclines);++ngoodperma,perm>>=2){srclines[ngoodperma]=(nr-1-r+permblkofst)*BLKSZ+(perm&3); } // install the source of each found row
     // permute rows of L, and the result permutation, so that processing L in order matches processing A in permuted order.  We will subsequently fetch L sequentially, A through permutation
     // src (~.@, ,: ~.@,@|.)@(#"1~ ~:/)@,: dest is 2xn array of (values to read,:values to write), up to 8 values
     A arwindexes; RZ(df2(arwindexes,asrclines,apv(AN(asrclines),(nr-1-r)*BLKSZ,1),acalcperm))
     // apply the permutation: to resultperm, the internal L blocks, and the values already written to the output area
     I permtemp[8]; I *pv=IAV(arwindexes); DO(AS(arwindexes)[1], permtemp[i]=resultperm[*pv++];) DO(AS(arwindexes)[1], resultperm[*pv++]=permtemp[i];)   // apply perm to resultperm
     D ltemp[8][4]; DO(nr-1-r, D (*lcol)[BLKSZ][BLKSZ]=(D (*)[BLKSZ][BLKSZ])LBLOCK(0,i); D *rescol=zv+BLKSZ*i; I *pv=IAV(arwindexes);  // for each column, point to tops of columns and also to permutation
     D ltemp[8][4]; I i,_n;
     DO(AS(arwindexes)[1], I pvv=*pv++; _mm256_storeu_pd(ltemp[i],_mm256_loadu_pd(lcol[(pvv>>LGBLKSZ)*nr][pvv&(BLKSZ-1)]));)   // copy old values out
      DO(AS(arwindexes)[1], I pvv=*pv++;
       _mm256_storeu_pd(lcol[(pvv>>LGBLKSZ)*nr][pvv&(BLKSZ-1)],_mm256_loadu_pd(ltemp[i])); _mm256_storeu_pd(&rescol[pvv*wn],_mm256_loadu_pd(ltemp[i]));  // copy permuted values back in, to both destinations
       if(!_mm256_testc_si256(_mm256_castpd_si256(sgnbit),_mm256_castpd_si256(_mm256_loadu_pd(ltemp[i]))))*(LBIT(pvv>>LGBLKSZ,i))&=~(1LL<<LBITX(pvv>>LGBLKSZ,i));  //  if the block contains a non0, cleat the 'block is 0' bit for it
      )
     )
     goto restartring;  // rerun the permuted ring, which is known to succeed
    }

    // falling through, the permutation is right & we can process L & U to completion for this ring

    // Now calculate uinv, the inverse of the remaining U matrix.  Do this by backsubstitution up the line.  Leave register a00-a03 holding the block result
    a13=_mm256_blend_pd(_mm256_setzero_pd(),recips,0b1000);
    _mm256_storeu_pd(&uinv[3][0],a13);  // row 3 is 0 0 0 I33=1/U33.
    a12=_mm256_blend_pd(_mm256_blend_pd(_mm256_mul_pd(_mm256_permute_pd(recips,0b0001),_mm256_fnmadd_pd(a02,recips,_mm256_setzero_pd())),recips,0b0100),_mm256_setzero_pd(),0b0011);             // U22*I23+U23*I33=0, so I23=(-U23*I33)/U22
    _mm256_storeu_pd(&uinv[2][0],a12);  // row 2 is 0 0 I22=1/U22 I23
      //  U11*I12+U12*I22=0   U11*I13+U12*I23+U13*I33=0 
    a11=_mm256_blend_pd(_mm256_blend_pd(_mm256_mul_pd(_mm256_permute4x64_pd(recips,0b01010101),_mm256_fnmadd_pd(a13,a01,_mm256_fnmadd_pd(a12,_mm256_permute_pd(a01,0b0011),_mm256_setzero_pd()))),recips,0b0010),_mm256_setzero_pd(),0b0001);
    _mm256_storeu_pd(&uinv[1][0],a11);  // row 1 is 0 I11=1/U11 I12 I23
      // U00*I01+U01*I11=0    U00*I02+U01*I12+U02*I22=0    U00*I03+U01*I13+U02*I23+U03*I33=0
    a10=_mm256_blend_pd(_mm256_mul_pd(_mm256_permute4x64_pd(recips,0b00000000),_mm256_fnmadd_pd(a13,a00,_mm256_fnmadd_pd(a12,_mm256_permute_pd(a00,0b0011),_mm256_fnmadd_pd(a11,_mm256_permute4x64_pd(a00,0b01010101),_mm256_setzero_pd())))),recips,0b0001);
    _mm256_storeu_pd(&uinv[0][0],a10);  // row 0 is I00=1/U00 I01 I02 I03

    // block created; advance input pointers.  Output pointer still set to initial value
    // the bitmask for a corner block is never used
   }else{  // U or L block, not corner
    UI *bma; I bmx;  // address and bit# of the bitmap address to store all-0 status into
    // We build the result in place in a00..a03
    if(r0>0){
     // U block.  Simulate L^-1 * D.  Subtract multiples of each row from all the following rows, cumulatively
     a01=_mm256_fnmadd_pd(a00,_mm256_set1_pd(linv[0][0]),a01);  // a1-=-a0*U10/U00
     a02=_mm256_fnmadd_pd(a00,_mm256_set1_pd(linv[1][0]),a02);  // a2-=-a0*U20/U00
     a03=_mm256_fnmadd_pd(a00,_mm256_set1_pd(linv[2][0]),a03);  // a3-=-a0*U30/U00
     a02=_mm256_fnmadd_pd(a01,_mm256_set1_pd(linv[1][1]),a02);  // a2-=-a1*U21/U11
     a03=_mm256_fnmadd_pd(a01,_mm256_set1_pd(linv[2][1]),a03);  // a3-=-a1*U31/U11
     a03=_mm256_fnmadd_pd(a02,_mm256_set1_pd(linv[2][2]),a03);  // a3-=-a2*U32/U22
     // block created; advance output and input pointers
     // get the address of the bitmask for this block, in the U bitmap
     bma=UBIT(nr-1-r,(nr-1-r)+r0); bmx=UBITX(nr-1-r,(nr-1-r)+r0);  // point to the bit to store all-0 status to.  col is (nr-1-r)+r0
    }else{
     // L block.  Take D * U^-1.  Fastest way is to dump to memory so we can use broadcast to replicate the L values across the row
     D __attribute__((aligned(CACHELINESIZE))) lmem[BLKSZ][BLKSZ];     // memory workarea
     _mm256_storeu_pd(&lmem[0][0],a00); _mm256_storeu_pd(&lmem[1][0],a01); _mm256_storeu_pd(&lmem[2][0],a02); _mm256_storeu_pd(&lmem[3][0],a03);
     __m256d tmp;  // where we bring in rows of U^-1
     tmp=_mm256_loadu_pd(&uinv[0][0]);  // row 0 of U^-1
     a00=_mm256_mul_pd(_mm256_set1_pd(lmem[0][0]),tmp); a01=_mm256_mul_pd(_mm256_set1_pd(lmem[1][0]),tmp); a02=_mm256_mul_pd(_mm256_set1_pd(lmem[2][0]),tmp); a03=_mm256_mul_pd(_mm256_set1_pd(lmem[3][0]),tmp); 
     tmp=_mm256_loadu_pd(&uinv[1][0]);  // row 1 of U^-1
     a00=_MM256_FMADD_PD(_mm256_set1_pd(lmem[0][1]),tmp,a00); a01=_MM256_FMADD_PD(_mm256_set1_pd(lmem[1][1]),tmp,a01); a02=_MM256_FMADD_PD(_mm256_set1_pd(lmem[2][1]),tmp,a02); a03=_MM256_FMADD_PD(_mm256_set1_pd(lmem[3][1]),tmp,a03); 
     tmp=_mm256_loadu_pd(&uinv[2][0]);  // row 2 of U^-1
     a00=_MM256_FMADD_PD(_mm256_set1_pd(lmem[0][2]),tmp,a00); a01=_MM256_FMADD_PD(_mm256_set1_pd(lmem[1][2]),tmp,a01); a02=_MM256_FMADD_PD(_mm256_set1_pd(lmem[2][2]),tmp,a02); a03=_MM256_FMADD_PD(_mm256_set1_pd(lmem[3][2]),tmp,a03); 
     tmp=_mm256_loadu_pd(&uinv[3][0]);  // row 3 of U^-1
     a00=_MM256_FMADD_PD(_mm256_set1_pd(lmem[0][3]),tmp,a00); a01=_MM256_FMADD_PD(_mm256_set1_pd(lmem[1][3]),tmp,a01); a02=_MM256_FMADD_PD(_mm256_set1_pd(lmem[2][3]),tmp,a02); a03=_MM256_FMADD_PD(_mm256_set1_pd(lmem[3][3]),tmp,a03); 
     // block created; advance pointers
     // get the address of the bitmask for this block, in the L bitmap
     bma=LBIT(r0+nr-1,nr-1-r); bmx=LBITX(r0+nr-1,nr-1-r);  // point to the bit to store all-0 status to    row is (nr-1-r)+(r0-(-r))
    }

    // update sparse bitmap
    if(nzeroblocks>=0){   // if we haven't given up on sparse checking
     // check for all-zero block, and update the sparse bitmap
     a10=_mm256_or_pd(a01,a00); a11=_mm256_or_pd(a02,a03); a10=_mm256_or_pd(a11,a10);  // OR of all values
     I blkis0=_mm256_testc_si256(_mm256_castpd_si256(sgnbit),_mm256_castpd_si256(a10))==1;   // testc is 1 if all values are +-0
     *bma=((*bma)&~(1LL<<bmx))|(blkis0<<bmx);  //  set bit to (all values are not NE)
     nzeroblocks+=blkis0;  // increment count of zero blocks
    }
   }

   // write the block to the result address from before update
   _mm256_storeu_pd(&scv[0][0][0],a00); _mm256_storeu_pd(&scv[0][1][0],a01); _mm256_storeu_pd(&scv[0][2][0],a02); _mm256_storeu_pd(&scv[0][3][0],a03);  // Store the 4x4 in the corner

   // write the block result to the overall result area.  We do this now because it's going to be a cache miss and we want to dribble out the data during the processing.  Also, the data is in registers now so we don't have to read it
   D *resultaddr;  // place to store result
   resultaddr=r0<=0?zv+(nr-1+r0)*BLKSZ*wn+(nr-1-r)*BLKSZ:zv+(nr-1-r)*BLKSZ*wn+(nr-1-r+r0)*BLKSZ;   // C/L block  row is (nr-1-r)+(r0-(-r))  col is nr-1-r  or   U block row is nr-1-r col is (nr-1-r)+r0
   if(r>0){
    // not the bottom-right corner.
    if(r0==r){     // last block of U - truncated on the right
     _mm256_maskstore_pd(resultaddr,endmask,a00); _mm256_maskstore_pd(resultaddr+wn,endmask,a01); _mm256_maskstore_pd(resultaddr+2*wn,endmask,a02); _mm256_maskstore_pd(resultaddr+3*wn,endmask,a03);
    }else if(r0==0){     // last block of L - truncated at bottom
     _mm256_storeu_pd(resultaddr,a00); if(((wn-1)&(BLKSZ-1))>0){_mm256_storeu_pd(resultaddr+wn,a01); if(((wn-1)&(BLKSZ-1))>1){_mm256_storeu_pd(resultaddr+2*wn,a02); if(((wn-1)&(BLKSZ-1))>2){_mm256_storeu_pd(resultaddr+3*wn,a03); }}}
    }else{     // normal full block
     _mm256_storeu_pd(resultaddr,a00);  _mm256_storeu_pd(resultaddr+wn,a01);  _mm256_storeu_pd(resultaddr+2*wn,a02);  _mm256_storeu_pd(resultaddr+3*wn,a03); 
    }
   }else{  // bottom-right corner
    _mm256_maskstore_pd(resultaddr,endmask,a00); if(((wn-1)&(BLKSZ-1))>0){_mm256_maskstore_pd(resultaddr+wn,endmask,a01); if(((wn-1)&(BLKSZ-1))>1){_mm256_maskstore_pd(resultaddr+2*wn,endmask,a02); if(((wn-1)&(BLKSZ-1))>2){_mm256_maskstore_pd(resultaddr+3*wn,endmask,a03); }}}
   }
  }
 }
 EPILOG(jlink(aresultperm,z));
#endif
 // here if fast FP code not supported, either because we don't have AVX or the input is not float.  Fall back to general version
 R jtludecompg(jt,w,DUMMYSELF);
}

