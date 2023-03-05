#include <assert.h>
#include "j.h"

#if C_AVX512

#define PCNT __builtin_popcountll
#define V __m512i
#define VH __m256i
#define M1 __mmask8
#define M2 __mmask16
#define M4 __mmask32
#define INLINE inline __attribute__((always_inline))
#define VL _mm512_loadu_epi64
#define CMPLS8 _mm512_cmplt_epi64_mask
#define CMPLU4 _mm512_cmplt_epu32_mask
#define CMPLMS8(x,y,m) _mm512_mask_cmplt_epi64_mask(m,x,y)
#define CMPLMU4(x,y,m) _mm512_mask_cmplt_epu32_mask(m,x,y)
#define CMPGS8 _mm512_cmpge_epi64_mask
#define CMPGU4 _mm512_cmpge_epu32_mask
#define CMPGMS8(x,y,m) _mm512_mask_cmpge_epi64_mask(m,x,y)
#define CMPGMU4(x,y,m) _mm512_mask_cmpge_epu32_mask(m,x,y)
#define COMP8(x,m) _mm512_maskz_compress_epi64(m,x)
#define COMP4(x,m) _mm512_maskz_compress_epi32(m,x)
#define COMPS8(p,m,x) _mm512_mask_compressstoreu_epi64(p,m,x)
#define COMPS4(p,m,x) _mm512_mask_compressstoreu_epi32(p,m,x)
#define VMINS8 _mm512_min_epi64
#define VMINU4 _mm512_min_epu32
#define VMINMS8(x,y,m) _mm512_mask_min_epi64(x,m,x,y)
#define VMINMU4(x,y,m) _mm512_mask_min_epu32(x,m,x,y)
#define VMAXS8 _mm512_max_epi64
#define VMAXU4 _mm512_max_epu32
#define VMAXMS8(x,y,m) _mm512_mask_max_epi64(x,m,x,y)
#define VMAXMU4(x,y,m) _mm512_mask_max_epu32(x,m,x,y)
#define VMINRS8 _mm512_reduce_min_epi64
#define VMINRU4 _mm512_reduce_min_epu32
#define VMAXRS8 _mm512_reduce_max_epi64
#define VMAXRU4 _mm512_reduce_max_epu32
#define VLM8(p,m) _mm512_maskz_loadu_epi64(m,p)
#define VLM4(p,m) _mm512_maskz_loadu_epi32(m,p)
#define VLM2(p,m) _mm512_maskz_loadu_epi16(m,p)
#define VBC8 _mm512_set1_epi64
#define VBC4 _mm512_set1_epi32
#define VBC2 _mm512_set1_epi16
#define VSUB8 _mm512_sub_epi64

#include "vgsortinavx512.h"

// avx512 utilities; todo pull these out so they can be used more generally

// z is a high pointer; align it by rounding down, returning <=1 full vector, giving a mask for active elements in m and the #active elements in d
static INLINE V aligndown8d(I **z,M1 *m,I *d){
 *d=(1+(63&((I)*z-1)))>>3;
 R VLM8(*z = (I*)(((I)*z-1)&~63),*m=BZHI(0xff,*d));}
// same, but adjust n a count of elements
static INLINE V aligndown8n(I **z,M1 *m,I *n){I d;V r=aligndown8d(z,m,&d);*n-=d;R r;}
static INLINE V aligndown8(I **z,M1 *m){R aligndown8d(z,m,&(I){0});}
static INLINE V alignup8d(I **z,M1 *m,I *d){
 I *nz=(I*)(((I)*z+64)&~63);
 V r=VLM8(*z,*m=BZHI(0xff,*d=nz-*z));
 *z=nz;
 R r;}
static INLINE V alignup8n(I **z,M1 *m,I *n){I d;V r=alignup8d(z,m,&d);*n-=d;R r;}
static INLINE V alignup8(I **z,M1 *m){R alignup8d(z,m,&(I){0});}

static INLINE V aligndown4d(UI4 **z,M2 *m,I *d){
 *d=(1+(63&((I)*z-1)))>>2;
 R VLM4(*z = (UI4*)(((I)*z-1)&~63),*m=BZHI(0xffff,*d));}
// same, but adjust n a count of elements
static INLINE V aligndown4n(UI4 **z,M2 *m,I *n){I d;V r=aligndown4d(z,m,&d);*n-=d;R r;}
static INLINE V aligndown4(UI4 **z,M2 *m){R aligndown4d(z,m,&(I){0});}
static INLINE V alignup4d(UI4 **z,M2 *m,I *d){
 UI4 *nz=(UI4*)(((I)*z+64)&~63);
 V r=VLM4(*z,*m=BZHI(0xffff,*d=nz-*z));
 *z=nz;
 R r;}
static INLINE V alignup4n(UI4 **z,M2 *m,I *n){I d;V r=alignup4d(z,m,&d);*n-=d;R r;}
static INLINE V alignup4(UI4 **z,M2 *m){R alignup4d(z,m,&(I){0});}

static INLINE V aligndown2d(US **z,M4 *m,I *d){
 *d=(1+(63&((I)*z-1)))>>1;
 R VLM2(*z = (US*)(((I)*z-1)&~63),*m=BZHI(0xffffffff,*d));}
// same, but adjust n a count of elements
static INLINE V aligndown2n(US **z,M4 *m,I *n){I d;V r=aligndown2d(z,m,&d);*n-=d;R r;}
static INLINE V aligndown2(US **z,M4 *m){R aligndown2d(z,m,&(I){0});}
static INLINE V alignup2d(US **z,M4 *m,I *d){
 US *nz=(US*)(((I)*z+64)&~63);
 V r=VLM2(*z,*m=BZHI(0xffffffff,*d=nz-*z));
 *z=nz;
 R r;}
static INLINE V alignup2n(US **z,M4 *m,I *n){I d;V r=alignup2d(z,m,&d);*n-=d;R r;}
static INLINE V alignup2(US **z,M4 *m){R alignup2d(z,m,&(I){0});}

// takes two qword vectors, subtracts min from all lanes of both, and gives a new vector containing the low dword of each lane in some order
// important note: if adapting to an architecture that only has unsigned comparisons, 'min' can be tweaked so that unsigned comparison will collate properly
static INLINE V squish48(V x,V y,I min){
 x=VSUB8(x,VBC8(min));
 y=VSUB8(y,VBC8(min));
 R _mm512_mask_shuffle_epi32(x,0b1010101010101010,y,0b10100000);} // big endian is confusing
// same, but catenates x and y
static INLINE V squishc48(V x,V y,I min){
 x=VSUB8(x,VBC8(min));
 y=VSUB8(y,VBC8(min));
 R _mm512_permutex2var_epi32(x,_mm512_set_epi32(30,28,26,24,22,20,18,16,14,12,10,8,6,4,2,0),y);}
// compress v into [l h) according to m, adjusting l and h
static INLINE void comp4(UI4 **l,UI4 **h,V v,M2 m) {
 COMPS4(*l,m,v);*l+=PCNT(m);
 COMPS4(*h=*h-16+PCNT(m),~m,v);}
// same, but do the comparison for me
static INLINE void compc4(UI4 **l,UI4 **h,V v,V pn) { comp4(l,h,v,CMPLU4(v,pn)); }
static INLINE void compcm4(UI4 **l,UI4 **h,V v,V pn,M2 m){
 M2 ml=CMPLMU4(v,pn,m);
 M2 mh=CMPGMU4(v,pn,m);
 COMPS4(*l,ml,v);*l+=PCNT(ml);
 COMPS4(*h-=PCNT(mh),mh,v);}
static INLINE void comphc4(UI4 **l,UI4 **h,V v,I min,V po) {
 VH vh=_mm512_cvtepi64_epi32(VSUB8(v,VBC8(min)));
 M1 m=CMPLS8(v,po);
 _mm256_mask_compressstoreu_epi32(*l,m,vh);*l+=PCNT(m);
 _mm256_mask_compressstoreu_epi32(*h=*h-8+PCNT(m),~m,vh);}
static INLINE void comphcm4(UI4 **l,UI4 **h,V v,I min,V po,M1 m) {
 VH vh=_mm512_cvtepi64_epi32(VSUB8(v,VBC8(min)));
 M1 ml=CMPLMS8(v,po,m);
 M1 mh=CMPGMS8(v,po,m);
 _mm256_mask_compressstoreu_epi32(*l,ml,vh);*l+=PCNT(ml);
 _mm256_mask_compressstoreu_epi32(*h-=PCNT(mh),mh,vh);}
static INLINE void comphca4(UI4 **l,UI4 **h,V v,I min,V po,I4 *ls,I4 *hs) {
 VH vh=_mm512_cvtepi64_epi32(VSUB8(v,VBC8(min)));
 M1 m=CMPLS8(v,po);
 _mm256_mask_compressstoreu_epi32(*l,m,vh);*l+=PCNT(m);
 _mm256_mask_compressstoreu_epi32(*h=*h-8+PCNT(m),~m,vh);
 *ls-=PCNT(m);*hs-=8-PCNT(m);}
// adjust space metrics too
static INLINE void compca4(UI4 **l,UI4 **h,V v,V p,I4 *ls,I4 *hs){
 M2 m=CMPLU4(v,p);
 COMPS4(*l,m,v);*l+=PCNT(m);
 COMPS4(*h=*h-16+PCNT(m),~m,v);
 *ls-=PCNT(m);*hs-=16-PCNT(m);}
static INLINE void compc8(I **l,I **h,V v,V p){
 M1 m=CMPLS8(v,p);
 COMPS8(*l,m,v);*l+=PCNT(m);
 COMPS8(*h=*h-8+PCNT(m),~m,v);}
static INLINE void compcm8(I **l,I **h,V v,V p,M1 m){
 M1 ml=CMPLMS8(v,p,m);
 M1 mh=CMPGMS8(v,p,m);
 COMPS8(*l,ml,v);*l+=PCNT(ml);
 COMPS8(*h-=PCNT(mh),mh,v);}
static INLINE void compca8(I **l,I **h,V v,V p,I4 *ls,I4 *hs){
 M1 m=CMPLS8(v,p);
 COMPS8(*l,m,v);*l+=PCNT(m);
 COMPS8(*h=*h-8+PCNT(m),~m,v);
 *ls-=PCNT(m);*hs-=8-PCNT(m);}

#define AUTO __auto_type

#define PIVOT(w,n) \
({ AUTO w0=w[0];AUTO wm=w[n/2];AUTO wn=w[n-1]; \
   AUTO th=MAX(w0,wm);AUTO tl=MIN(w0,wm); \
   th<wn ? th : (tl<wn ? wn : tl); })

#define PIVOTP(l,h) \
({ AUTO w0=*l;AUTO wm=*(__typeof__(l))((((I)l+(I)h)>>1)&-sizeof(*l));AUTO wn=*(h-1); \
   AUTO th=MAX(w0,wm); AUTO tl=MIN(w0,wm); \
   th<wn ? th : (tl<wn ? wn : tl); })

#define PIVOTP5(l,h) \
 ({ AUTO l25=l+(h-l)*2/5;AUTO l35=l+(h-l)*3/5; \
    AUTO p0=PIVOTP(l,l25);AUTO p1=PIVOTP(l35,h); \
    (p0+p1)>>2; })

// computes pivot and writes its index to i
#define PIVOTI(w,n,i) \
({ I im=n/2; AUTO w0=w[0];AUTO wm=w[im];AUTO wn=w[n-1]; \
   AUTO th=w0>wm?w0:wm;I thi=w0>wm?0:im; \
   AUTO tl=w0<wm?w0:wm;I tli=w0<wm?0:im; \
   AUTO tf=tl<wn?wn:tl;I tfi=tl<wn?n-1:tli; \
   i=th<wn ? thi : tfi; \
   th<wn ? th : tf; })

// sort [z z+n), taking advantage of the fact that all elements are in the range [min min+2^32)
// start by squishing towards low address, and then when done expand from high addresses to low ones
#define PIVOTQSQUISHMIN 256
void pivotqs8_squishlo4(I *z,I n,I min,I *opivot,UI4 **omid,UI4 **ohi){
 I pivoti;I ps=PIVOTI(z,n,pivoti);V po=VBC8(ps);V pn=VBC4(ps-min);
 I *lir=z,*hir=z+(n>>1);
 UI4 *liw=(UI4*)(I)lir,*hiw=(UI4*)(I)hir; // lo/hi read/write heads
 I *hhi=z+((n+1)>>1); // start of the 'high region'; see below for why this is sometimes different from hi
 // we want to grab two qword vectors, reduce them by min, and then combine into a single dword vector (without regard for order).  How?  shufps can do it at least, as can pshufd with merge-masking
 // compress is 2c/3c (skx/icx) at p5
 // shuffle is 1c at p5
 // compare is also 1c at p5; annoying
 // alternately, sub+movd2m is 1c+1c at p0, but I think this means range has to be 2^31; ok
 // knot+kmov is 1c+1c at p0
 // popcnt at p1, so should be uncontended
 // that is 3/4c at p5, 4c at p0, and p1/p6 are free to do some extra arithmetic on p1/p6.  Good
 // when implementing grade or sort-by (assuming we have too many bits to interleave), we need to maintain order, so probably vpermt2*.  Cross that bridge when we get to it

 // could alternately do the compares early, catenate vectors instead of interleaving, and then kunpckbw.  Probably not worth it but maybe...
 // depends on sizes I guess.  If everything fits in cache, we should do multiple partitions in parallel.  Otherwise memory bound so it doesn't matter?  Maybe just 2 partitions is not enough, so the early compares help with span?
 // (we do this for the initial unaligned handling)

 // start with an in-place compress+partition from z through (UI4*)z+n, and then partition the remainder into the gap
 // if n is odd, then (UI4*)z+n will point to the middle of qword; slightly annoying as we need to start writing there.  This is why we floored hi and ceilinged hhi; if n is odd, then they both avoid this annoying dword (which happens to be the at highest position)
 // because we use </>= to partition, the pivot always goes into the high partition.  So we take the ignored qword and swap its position with that of the pivot, and then fix up the ignored qword into the ignored dword
 if(n&1){ z[pivoti]=*hir; *(UI4*)hir=ps-min; } // not worth dispatch to check if pivoti==hi.  WAR not RAW, so should not even be slow, but even if it is, who cares

 // first, load <=1 vector each from zl and zh to force them to be aligned; squish into one (potentially sparse) vector and compress into the buffer
 V bva;M2 bvam;
 {M1 lm,hm;V l=alignup8(&lir,&lm),h=aligndown8(&hir,&hm);
  bva=squishc48(l,h,min);bvam=_mm512_kunpackb(hm,lm);}
 // load 4 additional vectors each from lo and hi to make space to unroll twice; squish into 4 buffered vectors
 V bv0=squish48(VL(lir+0),VL(lir+ 8),min);lir+=16;
 V bv1=squish48(VL(lir+0),VL(lir+ 8),min);lir+=16;
 V bv2=squish48(VL(hir-8),VL(hir-16),min);hir-=16;
 V bv3=squish48(VL(hir-8),VL(hir-16),min);hir-=16;
 I4 hs=0,ls=0; // amount of high/low space.  Relative, so just initialise at 0.  TODO because we squish, I think we can get away with just alternately grabbing from high/low for all but the most pathological of distributions?
 // if the #vectors left to compress is not a multiple of 4, make it one
 if(16&(hir-lir)){
  compca4(&liw,&hiw,squish48(VL(lir),VL(lir+8),min),pn,&ls,&hs);
  lir+=16;ls+=16;}
 if( 8&(hir-lir)){
  comphca4(&liw,&hiw,VL(lir),min,po,&ls,&hs);
  lir+=8;ls+=8;}
 I niter=(hir-lir)>>5;
 hir-=32; // hir is now implicitly offset; points _to_ the last vector-quad, rather than one past it, so we can always load four full vectors from hir or lir
 while(niter--){
  I *ptr=ls<hs?lir:hir;
  I lad=ls<hs; // did we adjust the low read head or the high one?
  ls+=lad<<6;hs+=(1<<6)^(lad<<6); // whichever we're adjusting, we made space for 64 elements (popped 32 big elements; made space for 64 squished elements)
  lir+=lad<<5;hir-=(1<<5)^(lad<<5);
  V v0=squish48(VL(ptr),   VL(ptr+ 8),min),
    v1=squish48(VL(ptr+16),VL(ptr+24),min);
  compca4(&liw,&hiw,v0,pn,&ls,&hs);
  compca4(&liw,&hiw,v1,pn,&ls,&hs);}
 //assert(hir+32==lir);
 // todo since we're squishing, we should have space in the original array to store this there
 // to store what? what did I mean?
 compcm4(&liw,&hiw,bva,pn,bvam);
 compc4(&liw,&hiw,bv0,pn);
 compc4(&liw,&hiw,bv1,pn);
 compc4(&liw,&hiw,bv2,pn);
 compc4(&liw,&hiw,bv3,pn);
 // now compress hhi through z+n into the leftover space
 // first align hhi
 {M1 hhim;V v=alignup8(&hhi,&hhim);comphcm4(&liw,&hiw,v,min,po,hhim);}
 // force #vectors remaining to be a multiple of 4
 if ( 8&(z+n-hhi)){comphc4(&liw,&hiw,VL(hhi),min,po);hhi+=8;}
 if (16&(z+n-hhi)){compc4(&liw,&hiw,squish48(VL(hhi),VL(hhi+8),min),pn);hhi+=16;}
 niter=(z+n-hhi)>>5;
 while(niter--){
  V v0=VL(hhi),v1=VL(hhi+8),v2=VL(hhi+16),v3=VL(hhi+24);
  compc4(&liw,&hiw,squish48(v0,v1,min),pn);
  compc4(&liw,&hiw,squish48(v2,v3,min),pn);
  hhi+=32;}
 I remaining=31&(z+n-hhi);
 if (remaining){
  I *remp=z+n-remaining;
  if(remaining>16){compc4(&liw,&hiw,squish48(VL(remp),VL(remp+8),min),pn);remp+=16;remaining-=16;}
  if(remaining>8){comphc4(&liw,&hiw,VL(remp),min,po);remp+=8;remaining-=8;}
  M1 m=BZHI(0xff,remaining);comphcm4(&liw,&hiw,VLM8(remp,m),min,po,m);}
 //assert(liw==hiw);
 *opivot=ps;
 *omid=liw;
 *ohi=(UI4*)z+n;}

#if 0
// partition [zl zh) according to p, squishing 
static void pivotqs8_squish44(zl,zh,min,p,&m0,&m1);
pivotqs8_squish48(zl,zh,min,p,&m0,&m1);
pivotqs8_squish84(zl,zh,p,&m0,&m1);
#endif

// initial out-of-place 8-byte pivot (computes bounds)
static INLINE void pivotqs8ob(I *z,I *w,I n,I **zm,I *min,I *max,I *pivot){
 I ps=PIVOT(w,n);V p=VBC8(ps);
 I *wl=z,*wh=z+n,*rl=w,*rh=w+n;
 V vmin=VBC8(IMAX),vmax=VBC8(IMIN);
 // advance backwards through the array, so we can save an op by terminating when the index is 0
 // first align rh
 {M1 m;V v=aligndown8n(&rh,&m,&n);
  vmin=VMINMS8(vmin,v,m);vmax=VMAXMS8(vmax,v,m);
  compcm8(&wl,&wh,v,p,m);}
 // there is an additional <=1 vector (with 1+(7&(n-1)) elements) at rl, in case it is misaligned;
 // having accounted for that, force an even iteration count
 if(8&(n-1)){ compc8(&wl,&wh,VL(rh-=8),p); n-=8; }
 I *rlt=rh-(n&~15)-16; // implicitly offset by 16 so we can use a displacement in [0 2048)
 for(I i=n&~15;i;i-=16){ //each iteration handles 2x vector of 8x elements each
  V v1=VL(rlt+i+8),
    v0=VL(rlt+i+0);
  vmin=VMINS8(vmin,VMINS8(v1,v0));
  vmax=VMAXS8(vmax,VMAXS8(v1,v0));
  compc8(&wl,&wh,v1,p);
  compc8(&wl,&wh,v0,p);}
 // handle leading partial vector
 {M1 m=BZHI(0xff,1+(7&(n-1)));
  V v=VLM8(rl,m);vmin=VMINMS8(vmin,v,m);vmax=VMAXMS8(vmax,v,m);
  compcm8(&wl,&wh,v,p,m);}
 //assert(wl==wh);
 *zm=wl; //wl and wh should be the same now
 *min=VMINRS8(vmin);
 *max=VMAXRS8(vmax);
 *pivot=ps;}

#define SORTQNAME s8
#define T I
#define TZ 8
#define TMAXV IMAX
#define TMINV IMIN
#define M M1
#define VMAX VMAXS8
#define VMIN VMINS8
#define VMAXM VMAXMS8
#define VMINM VMINMS8
#define VMINR VMINRS8
#define VMAXR VMAXRS8

#define VBC VBC8
#define COMPCA compca8
#define COMPC compc8
#define COMPCM compcm8
#define ALIGNUP alignup8
#define ALIGNDN aligndown8

#include "vgsortiqavx512.h"

#define SORTQNAME u4
#define T UI4
#define TZ 4
#define TMAXV C4MAX
#define TMINV 0
#define M M2
#define VMAX VMAXU4
#define VMIN VMINU4
#define VMAXM VMAXMU4
#define VMINM VMINMU4
#define VMINR VMINRU4
#define VMAXR VMAXRU4

#define VBC VBC4
#define COMPCA compca4
#define COMPC compc4
#define COMPCM compcm4
#define ALIGNUP alignup4
#define ALIGNDN aligndown4

#include "vgsortiqavx512.h"

// sort u4 in place and then explode to s8 high, descending from zh
// returns zl (useful for recursion)
I *sortqu4iehi(UI4 *wl,UI4 *wh,I *zh,UI4 min,UI4 max,I base){
 if(wh-wl<=10*16){I *r=zh-(wh-wl);sortnu4e(wl,wh-wl,base,r);R r;}
 UI4 *wm,p;
 pivotqu4ip(wl,wh,&wm,&p);
 // must sort high before low, else exploding low may overwrite high
 R sortqu4iehi(wl,wm,sortqu4iehi(wm,wh,zh,p,max,base),min,p,base);}

// ditto, but low and ascending
I *sortqu4ielo(UI4 *wl,UI4 *wh,I *zl,UI4 min,UI4 max,I base){
 if(wh-wl<=10*16){sortnu4e(wl,wh-wl,base,zl);R zl+(wh-wl);}
 UI4 *wm,p;
 pivotqu4ip(wl,wh,&wm,&p);
 R sortqu4ielo(wm,wh,sortqu4ielo(wm,wh,zl,min,p,base),p,max,base);}

static void sort_or_squishlos8(I*,I*,I,I);

void sortqs8i(I *zl,I *zh,I min,I max){I *zm,p;
 if(zh-zl<=10*8){sortns8(zl,zl,zh-zl);R;}
 p=PIVOT(zl,(zh-zl));
#if 0
 if((UI)(p-min)<(1ull<<32)){
  if((UI)(max-p)<(1ull<<32)){
   UI4 *m0,*m1;
   pivotqs8_squish44(zl,zh,min,p,&m0,&m1);
   sortqu4iehi((UI4*)zl,m0,zl+(m0-(UI4*)zl),0,p-min,min);
   sortqu4ielo(m1,(UI4*)zh,zh-((UI4*)zh-m1),0,max-p,p);}
  else{
   UI4 *m0;I *m1;
   pivotqs8_squish48(zl,zh,min,p,&m0,&m1);
   sortqu4iehi((UI4*)zl,m0,m1,0,p-min,min);
   sortqs8i(m1,zh,p,max);}}
 else if((UI)(max-p)<(1ull<<32)){
  I *m0;UI4 *m1;
  pivotqs8_squish84(zl,zh,p,&m0,&m1);
  sortqs8i(zl,m0,min,p);
  sortqu4ielo(m1,(UI4*)zh,zh-((UI4*)zh-m1),0,max-p,p);}
 else{
 }
#endif
 pivotqs8i(zl,zh,&zm,p);
 // should be redundant--given above path, we can squish earlier
 // can also be somewhat cleverer--e.g., if range is 8g, partition on min+4g regardless of sampling.  But if range is 6g and sampling yields min+1g, use min+2g.  Maybe multi partition or some such on eg 16g?  Annoying since, unlike mergesort, can't quantise result to single-vector units, but can maybe still do something nice
 sort_or_squishlos8(zl,zm,min,p);
 sort_or_squishlos8(zm,zh,p,max);
 //sortqs8i(zl,zm,min,p);
 //sortqs8i(zm,zh,p,max);
}

void sortqu4i(UI4 *zl,UI4 *zh,UI4 min,UI4 max){
 if (zh-zl<=16*8){sortnu4(zl,zh-zl);R;}
 UI4 *zm,p;
 pivotqu4ip(zl,zh,&zm,&p);
 sortqu4i(zl,zm,min,p);
 sortqu4i(zm,zh,p,max);}

void sortqu4(UI4 *z,UI4 *w,I n){
 memcpy(z,w,4*n);
 sortqu4i(z,z+n,0,0xffffffff);}

// for any partition, we want to maintain [min max) bounds on it
// but an exclusive maximum isn't representable as I if the partition contains IMAX
// to get around this, partition out imax
// should be rarely taken, so don't bother optimising or trying to do anything else fancy
static void filter_imax(I *wl,I **wh,I *nmax){
 I *mh=*wh;
 I imaxct=0;
 I tnmax=IMIN;
 for(I *zl=wl;wl<mh;wl++){
  if(uncommon(*wl==IMAX)){imaxct++;}
  else{*zl++=*wl,tnmax=MAX(tnmax,*wl);}}
 DO(imaxct,*--mh=IMAX;);
 *wh=mh;
 *nmax=tnmax;}

static void sort_or_squishlos8(I *zl,I *zh,I min,I max){
 if((UI)(max-min)<=(1ull<<32)&&zh-zl>=PIVOTQSQUISHMIN){
  I p;UI4 *wl=(UI4*)zl,*wm,*wh;
  pivotqs8_squishlo4(zl,zh-zl,min,&p,&wm,&wh);
  sortqu4iehi(wl,wm,sortqu4iehi(wm,wh,zh,p-min,max-min,min),0,p-min,min);}
 else{
  sortqs8i(zl,zh,min,max);}}

void vvsortqs8ao(I *zl,I*w,I n){
 if(n<=10*8){sortns8(zl,w,n);R;}
 I min,max,pivot,*zm;
 pivotqs8ob(zl,w,n,&zm,&min,&max,&pivot);
 I *zh=zl+n;
 if(unlikely(max==IMAX))filter_imax(zl,&zh,&max);
 max++;
 sort_or_squishlos8(zl,zm,min,pivot);
 sort_or_squishlos8(zm,zh,pivot,max);
#if 0
 if((UI)(pivot-min)<=(1ull<<32)&&zm-zl>=PIVOTQSQUISHMIN){
  UI4 *wl=(UI4*)zl,*wm,*wh;
  I lp;
  pivotqs8_squishlo4(zl,zm-zl,min,&lp,&wm,&wh);
  sortqu4iehi(wl,wm,sortqu4iehi(wm,wh,zm,lp-min,pivot-min,min),0,lp-min,min);}
 else{
  sortqs8i(zl,zm,min,pivot);}
 if((UI)(max-pivot)<=(1ull<<32)&&zh-zm>=PIVOTQSQUISHMIN){
  UI4 *wl=(UI4*)zm,*wm,*wh;
  I hp;
  pivotqs8_squishlo4(zm,zh-zm,pivot,&hp,&wm,&wh);
  sortqu4iehi(wl,wm,sortqu4iehi(wm,wh,zh,hp-pivot,max-pivot,pivot),0,hp-pivot,pivot);}
 else{
  sortqs8i(zm,zh,pivot,max);}
#endif
}
void vvsortqs8ai(I *zl,I n){
 if(n<=10*8){sortns8(zl,zl,n);R;}
 I min,max,pivot,*zm;
 pivotqs8ib(zl,zl+n,&zm,&min,&max,&pivot);
 I *zh=zl+n;
 if(unlikely(max==IMAX))filter_imax(zl,&zh,&max);
 max++;
 sort_or_squishlos8(zl,zm,min,pivot);
 sort_or_squishlos8(zm,zh,pivot,max);}
#endif //C_AVX512
