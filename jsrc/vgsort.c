/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Grade -- dyad /: and \: where a==w                               */

#include "j.h"
#include "vg.h"
#pragma intrinsic(memset)

// Places marked TUNE have parameters that must be tuned to the hardware

#define CXCHG2(v0,v1) {void *v2=v0; B t=COMPFN(compn,v0,v1); v0=(!t)?v1:v0; v1=(!t)?v2:v1;}

static const UC orderfromcomp3[8] = {
36,24,0,9,33,0,18,6
};

static const UC orderfromcomp4[64] = {
228,180,0,120,216,0,156,108,0,0,0,57,0,0,0,45,0,0,0,0,201,0,141,0,0,0,0,0,0,0,78,30,
225,177,0,0,0,0,0,0,0,114,0,54,0,0,0,0,210,0,0,0,198,0,0,0,147,99,0,39,135,0,75,27
};

static const US orderfromcomp5[1024] = {
18056,14472,0,10440,17608,0,13576,9992,0,0,0,6352,0,0,0,5904,0,0,0,0,17104,0,13072,0,0,0,0,0,0,0,8984,5400,
18000,14416,0,0,0,0,0,0,0,10328,0,6296,0,0,0,0,17496,0,0,0,17048,0,0,0,13408,9824,0,5792,12960,0,8928,5344,
0,0,0,0,0,0,0,0,0,0,0,2257,0,0,0,1809,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1305,
0,0,0,0,0,0,0,0,0,0,0,2201,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1697,0,0,0,1249,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16593,0,12561,0,0,0,0,0,0,0,8473,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16537,0,0,0,0,0,0,0,12449,0,8417,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4378,794,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4322,738,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
17937,14353,0,0,0,0,0,0,0,10265,0,0,0,0,0,0,17433,0,0,0,0,0,0,0,13345,9761,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,6170,0,2138,0,0,0,0,0,0,0,0,0,0,0,0,0,5666,0,1634,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16922,0,0,0,16474,0,0,0,12834,0,0,0,12386,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8739,5155,0,1123,8291,0,4259,675,
18049,14465,0,10433,17601,0,13569,9985,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,6338,0,0,0,5890,0,0,0,2250,0,0,0,1802,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,17090,0,13058,0,0,0,0,0,0,0,0,0,0,0,0,0,16586,0,12554,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,8963,5379,0,0,0,0,0,0,0,1291,0,0,0,0,0,0,8459,0,0,0,0,0,0,0,4371,787,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
17986,14402,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
17930,14346,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,10307,0,6275,0,0,0,0,0,0,0,2187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,10251,0,0,0,0,0,0,0,6163,0,2131,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
17475,0,0,0,17027,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16523,0,0,0,0,0,0,0,0,0,0,0,
17419,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16915,0,0,0,16467,0,0,0,0,0,0,0,0,0,0,0,
13380,9796,0,5764,12932,0,8900,5316,0,0,0,1676,0,0,0,1228,0,0,0,0,12428,0,8396,0,0,0,0,0,0,0,4308,724,
13324,9740,0,0,0,0,0,0,0,5652,0,1620,0,0,0,0,12820,0,0,0,12372,0,0,0,8732,5148,0,1116,8284,0,4252,668
};

// Comparison functions.  Do one comparison before the loop for a fast exit if it differs.
// On VS this sequence, where a single byte is returned, creates a CMP/JE/SETL sequence, performing only one (fused) compare
#define COMPGRADE(T,t) do{T av=*a, bv=*b; if(av!=bv) R av t bv; if(!--n)break; ++a; ++b;}while(1); R 1;
static INLINE B compiu(I n, I *a, I *b){COMPGRADE(I,<)}
static INLINE B compid(I n, I *a, I *b){COMPGRADE(I,>)}
static INLINE B compdu(I n, D *a, D *b){COMPGRADE(D,<)}
static INLINE B compdd(I n, D *a, D *b){COMPGRADE(D,>)}

// General sort, with comparisons by function call, but may do extra comparisons to avoid mispredicted branches
#define GRADEFNNAME jmsort
#define MERGEFNNAME jmerge
#define COMPFN (*comp)
#define T void
#define PTRADD(p,item) ((void *)((C*)p+(item)*bpi))
#define MVITEMS(dest,src,nitems) {MC(dest,src,(nitems)*bpi); dest=PTRADD(dest,nitems); src=PTRADD(src,nitems);}
#include "vgsort.h"

// Sorts with inline comparisons
#define GRADEFNNAME jmsortiu
#define MERGEFNNAME jmergeiu
#define COMPFN compiu
#define T I
#define PTRADD(p,item) ((p)+(item)*(compn))
#define MVITEMS(dest,src,nitems) {I _i = (nitems)*(compn); do{*dest++=*src++;}while(--_i);}
#include "vgsort.h"

#define GRADEFNNAME jmsortid
#define MERGEFNNAME jmergeid
#define COMPFN compid
#define T I
#define PTRADD(p,item) ((p)+(item)*(compn))
#define MVITEMS(dest,src,nitems) {I _i = (nitems)*(compn); do{*dest++=*src++;}while(--_i);}
#include "vgsort.h"

#define GRADEFNNAME jmsortdu
#define MERGEFNNAME jmergedu
#define COMPFN compdu
#define T D
#define PTRADD(p,item) ((p)+(item)*(compn))
#define MVITEMS(dest,src,nitems) {I _i = (nitems)*(compn); do{*dest++=*src++;}while(--_i);}
#include "vgsort.h"

#define GRADEFNNAME jmsortdd
#define MERGEFNNAME jmergedd
#define COMPFN compdd
#define T D
#define PTRADD(p,item) ((p)+(item)*(compn))
#define MVITEMS(dest,src,nitems) {I _i = (nitems)*(compn); do{*dest++=*src++;}while(--_i);}
#include "vgsort.h"


static struct {
 CMP comproutine;
 void *(*sortfunc)();
} sortroutines[][2] = {  // index is [bitx][up]
[B01X]={{compcd,jmsort},{compcu,jmsort}}, [LITX]={{compcd,jmsort},{compcu,jmsort}}, [INTX]={{0,(void *(*)())jmsortid},{0,(void *(*)())jmsortiu}}, [FLX]={{0,(void *(*)())jmsortdd},{0,(void *(*)())jmsortdu}},
[CMPXX]={{0,(void *(*)())jmsortdd},{0,(void *(*)())jmsortdu}},
[QPX]={{0,(void *(*)())jmsortdd},{0,(void *(*)())jmsortdu}},
[C2TX]={{compud,jmsort},{compuu,jmsort}}, [C4TX]={{comptd,jmsort},{comptu,jmsort}},
[INT2X]={{compsd,jmsort},{compsu,jmsort}}, [INT4X]={{compld,jmsort},{complu,jmsort}},
};

// sort for direct types, without pointers.  When the operand overflows cache the pointer method is very slow
// We support B01/LIT/C2T/C4T/INT/INT2/INT4/FL/CMPX/QP
// jt has the JTDESCEND flag
static A jtsortdirect(J jt,I m,I api,I n,A w){F1PREFJT;A x,z;I t;
 t=AT(w);
 // Create putative output area, same size as input.  If there is more than one cell in result, this will always be the result.
 I zr=AR(w); GA(z,AT(w),AN(w),zr,AS(w)); void * RESTRICT zv=voidAVn(zr,z);
 I cpi=api+(t&CMPX+QP?api:0);  // compares per item on a sort
 I bpi=api<<bplg(t);  // bytes per item of a sort
 I bps=bpi*n;  // bytes per sort
 void * RESTRICT wv=voidAV(w);
 CMP cmpfunc=sortroutines[CTTZ(t)][(~(I)jtinplace>>JTDESCENDX)&1].comproutine;
 void *(*sortfunc)() = sortroutines[CTTZ(t)][(~(I)jtinplace>>JTDESCENDX)&1].sortfunc;
 // allocate the merge work area, large enough to hold one sort.  In case this turns out to be the final result,
 // make the shape the same as the result shape (if there is more than one sort, this shape will be wrong, but that
 // won't matter, since the shape will never be used elsewhere)
 GA(x,t,n*api,zr,AS(w)); void * RESTRICT xv=voidAVn(zr,x);  /* work area for msmerge() */
 DO(m,   // sort each cell
  void *sortres=(*sortfunc)(cmpfunc,cpi,n,bpi,(void*)zv,(void*)xv,wv);
  if(m==1){
   // If there is only one cell, it may be either *zv or *xv, but either way it should be the result
   R (sortres==zv)?z:x;
  }else{
   // If there is more than one cell, we have to make sure all the data migrates to *zv, if it's not there already
   if(sortres!=zv)MCL(zv,sortres,bps);
  }
  wv=(void*)((C*)wv+bps); zv=(void*)((C*)zv+bps);
 );
 R z;  // multiple cells - return original z
}    /* w grade"r w for direct types, by moving the data without pointers */


// JTDESCEND in jt is set for descending
#define SF(f)         A f(J jt,I m,I n,A w)

/* m - # cells (# individual grades to do) */
/* c - # atoms in a cell                   */
/* n - length of sort axis                 */
/* w - array to be graded                  */

static SF(jtsortb){F1PREFJT;A z;B up,*u,*v;I i,s;
 I zr=AR(w); GA(z,AT(w),AN(w),AR(w),AS(w)); v=BAVn(zr,z);
 up=(~(I)jtinplace>>JTDESCENDX)&1;  u=BAV(w);
 for(i=0;i<m;++i){
  s=bsum(n,u);
  if(up){mvc(n-s,v,MEMSET00LEN,MEMSET00); mvc(s  ,v+n-s,1,MEMSET01);}
  else  {mvc(s  ,v,1,MEMSET01); mvc(n-s,v+s,MEMSET00LEN,MEMSET00);}
  u+=n; v+=n;
 }
 R z;
}    /* w grade"1 w on boolean */

static SF(jtsortb2){F1PREFJT;A z;B up;I i,ii,yv[4];US*v,*wv;
 I zr=AR(w); GA(z,AT(w),AN(w),AR(w),AS(w)); v=USAVn(zr,z);
 wv=USAV(w); up=(~(I)jtinplace>>JTDESCENDX)&1;
 I startfill=0;
 for(i=0;i<m;++i){
  mvc(sizeof(yv),yv,MEMSET00LEN,MEMSET00);
  DQ(n, IND2(*wv++); ++yv[ii];);
  DO(4, ii=i^((up-1)&3); US b2=(ii+(ii<<9)); b2=(b2>>1)&0x0101; mvc(yv[ii]*2,v+startfill,2,&b2); startfill+=yv[ii];)
 }
 R z;
}    /* w grade"r w on 2-byte boolean items */

static SF(jtsortb4){F1PREFJT;A z;B up;I i,ii,yv[16];UINT*v,*wv;
 I zr=AR(w); GA(z,AT(w),AN(w),AR(w),AS(w)); v=(UINT*)AVn(zr,z);
 wv=(UINT*)AV(w); up=(~(I)jtinplace>>JTDESCENDX)&1;
 I startfill=0;
 for(i=0;i<m;++i){
  mvc(sizeof(yv),yv,MEMSET00LEN,MEMSET00);
  DQ(n, IND4(*wv++); ++yv[ii];);
  DO(16, ii=i^((up-1)&15); UINT b4=(ii+(ii<<9)); b4=(b4+(b4<<18)); b4=(b4>>3)&0x01010101; mvc(yv[ii]*4,v+startfill,4,&b4); startfill+=yv[ii];)
 }
 R z;
}    /* w grade"r w on 4-byte boolean items */

static SF(jtsortc){F1PREFJT;A z;B up;I i,ii,yv[256];UC*v,*wv;
 I zr=AR(w); GA(z,AT(w),AN(w),AR(w),AS(w)); v=UAVn(zr,z);
 wv=UAV(w); up=(~(I)jtinplace>>JTDESCENDX)&1; I p=LIT&AT(w)?256:2; 
 I startfill=0;
 for(i=0;i<m;++i){
  mvc(p*SZI,yv,MEMSET00LEN,MEMSET00);
  DQ(n, ++yv[*wv++];);
  DO(p, ii=i^((up-1)&(p-1)); mvc(yv[ii],v+startfill,1,&ii); startfill+=yv[ii];)
 }
 R z;
}    /* w grade"1 w on boolean or character */

static SF(jtsortc2){F1PREFJT;A z;B up;I i,yv[65536];US*v,*wv;
 I zr=AR(w); GA(z,AT(w),AN(w),AR(w),AS(w)); v=USAVn(zr,z);
 wv=USAV(w); up=(~(I)jtinplace>>JTDESCENDX)&1;
 I startfill=0; I sct=AT(w)&C2T?0:8;  // C2T is littleendian, 2 chars are bigendian
 for(i=0;i<m;++i){
  mvc(65536*SZI,yv,MEMSET00LEN,MEMSET00);
  DQ(n, ++yv[*wv++];);
  DO(65536, US ii=i^((up-1)&(65536-1)); ii=(ii<<sct)|(ii>>sct); mvc(yv[ii]*2,v+startfill,2,&ii); startfill+=yv[ii];)  // scaf could exit early when startfill goes to end
 }
 R z;
}    /* w grade"1 w on 2-byte character or unicode items */


// We are known to have 1 atom per item
static SF(jtsorti1){F1PREFJT;A x,y,z;I*wv;I i,*xv,*zv;void *yv;
 I zr=AR(w); GA(z,AT(w),AN(w),AR(w),AS(w)); zv=AVn(zr,z);
 wv=AV(w);
 // choose bucket table size & function; allocate the bucket area
 I (*grcol)(I,I,void*,I,I*,I*,const I,US*,I);  // prototype for either size of buffer
 { I use4 = n>65535; grcol=use4?(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol4:(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol2; GATV0(y,INT,((65536*sizeof(US))>>LGSZI)<<use4,1); yv=AV1(y);}
 GATV0(x,INT,n,1); xv=AV1(x);
 for(i=0;i<m;++i){I colflags;
  colflags=grcol(65536,0L,yv,n,wv,xv,sizeof(I)/sizeof(US),    INTLSBWDX+(US*)wv,4+((~(I)jtinplace>>(JTDESCENDX-1))&2));  // 'sort', and move 'up' to bit 1
#if SY_64
  colflags=grcol(65536,0L,yv,n,xv,zv,sizeof(I)/sizeof(US),1*WDINC+INTLSBWDX+(US*)xv,colflags);
  colflags=grcol(65536,0L,yv,n,zv,xv,sizeof(I)/sizeof(US),2*WDINC+INTLSBWDX+(US*)zv,colflags);
#endif
  grcol(65536,0L,yv,n,xv,zv,sizeof(I)/sizeof(US),INTMSBWDX+(US*)xv,colflags|1);
  wv+=n; zv+=n;
 }
 R z;
}    /* w grade"r w on large-range integers */

//todo should use for avx2 too
//maybe even swar??
#if !(C_AVX512 && C_FSGSBASE)
// sort a single integer list using quicksort without misprediction, inplace
#define SORTQCOND (C_AVX2 || EMU_AVX2)
#define SORTQNAME vvsortqs8ai
#define SORTQTYPE IL
#define SORTQSCOPE
#define SORTQSET256 _mm256_set_epi64x
#define SORTQTYPE256 __m256i
#define SORTQCASTTOPD _mm256_castsi256_pd
#define SORTQCMP256 _mm256_cmpgt_epi64
#define SORTQCMPTYPE 
#define SORTQMASKLOAD _mm256_maskload_epi64
#define SORTQULOAD _mm256_loadu_si256
#define SORTQULOADTYPE __m256i*
#include "vgsortq.h"

#define SORTQCOND 0 //todo--avx2 vectorised version wants 8-byte quantities so disable for now.  Fine as should switch to 
#define SORTQNAME vvsortqs4ai
#define SORTQTYPE I4
#define SORTQSCOPE
#define SORTQSET256 _mm256_set_epi32
#define SORTQTYPE256 __m256i
#define SORTQCASTTOPD _mm256_castsi256_pd
#define SORTQCMP256 _mm256_cmpgt_epi32
#define SORTQCMPTYPE 
#define SORTQMASKLOAD _mm256_maskload_epi32
#define SORTQULOAD _mm256_loadu_si256
#define SORTQULOADTYPE __m256i*
#include "vgsortq.h"
// scaf
void vvsortqs8ao(IL *z,IL *w,I n){ memcpy(z,w,8*n); vvsortqs8ai(z,n); }
void vvsortqs4ao(I4 *z,I4 *w,I n){ memcpy(z,w,4*n); vvsortqs4ai(z,n); }
#endif


// JTDESCEND set in jt
static SF(jtsortiq){F1PREFIP;  // m=#sorts, n=#items in each sort, w is block
 A z;I inplace=ASGNINPLACESGN(SGNIF(jtinplace,JTINPLACEWX),w); //inplace sort?
 if(inplace)z=w;
 else GA(z,AT(w),AN(w),AR(w),AS(w));
 I *zv=IAV(z),*wv=IAV(w);
 DQ(m,
  if(inplace)vvsortqiai(zv,n);
  else vvsortqiao(zv,wv,n),wv+=n;
  if((I)jtinplace&JTDESCEND){I *zv1=zv; I *zv2=zv+n; DQ(n>>1, I t=*zv1; *zv1++=*--zv2; *zv2=t;)} //scaf strawman reverse if desc
  zv+=n;)
 RETF(z);}

static SF(jtsorti){F1PREFIP;A y,z;I i;UI4 *yv;I j,s,*wv,*zv;
 wv=AV(w);
 // figure out whether we should do small-range processing.  Comments in vg.c
 // First, decide, based on the length of the list, what the threshold for small-range sorting will be.
#if 1  // set to 0 to TUNE
 I nrange;   // range allowed, as multiple of size
 if(BETWEENC(n,1024,100000000)){
  I lzn=CTLZI(n); lzn=((((UI)n<<1)<<((BW-1)-lzn))>>(BW-8))+(lzn<<8);  // approx lg(n), binary point 8
  lzn=lzn*709-1006633;  // 0.0108333 2^.x - 0.06, binary point at 24
  nrange=16777216/lzn;  // 1/lzn, binary point back to 0
 }else{nrange=n<=1023?25:2;}  // for small or large, use limit.  On the high end we don't want to run out of memory
 CR rng = condrange(wv,AN(w),IMAX,IMIN,n*nrange); // see if smallrange allowed
 // smallrange always wins if applicable
 if(!rng.range){  // range was too large
  R jtsortiq(jtinplace,m,n,w);  // qsort always wins as of 2/2023 Alder Lake
//  if(n<100000)R jtsortdirect(jtinplace,m,1,n,w);  // 800-99999, mergesort   TUNE
//  R sorti1(m,n,w);  // 100000+, radix  TUNE
 }
#else
  CR rng = condrange(wv,AN(w),IMAX,IMIN,n*50); // testing on Alder Lake 2/2023
  if(!rng.range){  // range was too large
   if((n&3)==0b00)R jtsortiq(jtinplace,m,n,w);  // qsort  TUNE
   if((n&3)==0b01)R jtsortdirect(jtinplace,m,1,n,w);  //  mergesort   TUNE
   if((n&2)==0b10)R sorti1(m,n,w);  // radix  TUNE
// {{y , 0 _3 _2 {{ 6!:2 '/:~ nn' [ nn =. x }. y }}"0 _  y ?@$ 1e9}}"0 <.&.(%&4) 10000 * 10 ^ 10 %~ i. 41  NB. tuning not including smallrange: qsort merge radix.  range up to 1e9
// {{y , 0 _3 _2 {{ 6!:2 '/:~ nn' [ nn =. x }. y }}"0 _  y ?@$ 1e9}}"0 <.&.(%&4) 1e9 1e10 1e11  NB. tuning not including smallrange: qsort merge radix, big arrays, range to 1e9
// (] , ] {{ 6!:2 '/:~ nn' [ nn =. x ?@$ y }}"0 (1e18) , *&(>:2 * i. 10))"0 (3) + <.&.(%&4) 10000 * 10 ^ 10 %~ i. 41  NB. smallrange, buckets of length 2
   // n&3==0b11 for smallrange
   // smallrange results 
   // 2e3 25x 3e3 20x 6e3 11x 8e3 8x 1e4 7x 1e5 7x 1.6e6 6x 6e6 5x 1e8 5x
   // we approximate this by the line in (lg x,%y space) from (12,.07) to (24,0.2)
   // %y-.07 % 2^.x-12 = .13%12
   // %y = 0.0108333 2^.x - 0.06
  }
#endif
 // allocate area for the data, and result area
 GATV0(y,C4T,rng.range,1); yv=C4AV1(y)-rng.min;  // yv->totals area
 if(ASGNINPLACESGN(SGNIF(jtinplace,JTINPLACEWX),w))z=w;else GA(z,AT(w),AN(w),AR(w),AS(w));
 zv=AV(z);
 // clear all totals to 0, then bias address of area so the data fits
 for(i=0;i<m;++i){  // for each list...
  mvc(rng.range*sizeof(UI4),yv+rng.min,MEMSET00LEN,MEMSET00); 
  DQ(n, ++yv[*wv++];);  // increment total for each input atom
  // run through the totals, copying in the requisite # repetitions of each value
  // We have to disguise the loop to prevent VS from producing a REP STOS, which we don't want because the loop is usually short
  I incr = 1-(((I)jtinplace>>(JTDESCENDX-1))&2); j=rng.min+(REPSGN(incr)&(rng.range-1));  // jt>complt is 1 or -1
  DQ(rng.range, s=yv[j]; DQ(s, *zv++=j;) j+=incr;)  // Don't zv+=zincr, because VS doesn't pull the *8 out
//  if((UI)jt->workareas.compare.complt>>(BW-1)){ j=rng.min; DQ(rng.range, s=(I)yv[j]; DQ(s, *zv++=j;); ++j;);}  // generates rep stos, which is slow.  should fix
//  else{j=rng.min+rng.range; DQ(rng.range, --j; s=(I)yv[j]; DQ(s, *zv++=j  ;););}
 }
 R z;
}    /* w grade"1 w on small-range integers */


static SF(jtsortu1);

// see jtsorti above
static SF(jtsortu){F1PREFIP;A y,z;I i;UI4 *yv;C4 j,s,*wv,*zv;
 wv=C4AV(w);
 I maxrange; CR rng;
 if(0<(maxrange=16*(n-32))){rng = condrange4(wv,AN(w),-1,0,maxrange);
 }else rng.range=0;
 if(!rng.range)R n>700?sortu1(m,n,w):jtsortdirect(jtinplace,m,1,n,w);  // TUNE
 GATV0(y,C4T,rng.range,1); yv=C4AV1(y)-rng.min;
 I zr=AR(w); GA(z,AT(w),AN(w),AR(w),AS(w)); zv=C4AVn(zr,z);
 for(i=0;i<m;++i){
  mvc(rng.range*sizeof(UI4),yv+rng.min,MEMSET00LEN,MEMSET00); 
  DQ(n, ++yv[*wv++];);
  I incr = 1-(((I)jtinplace>>(JTDESCENDX-1))&2); j=(C4)(rng.min+(REPSGN(incr)&(rng.range-1)));
  DQ(rng.range, s=yv[j]; DQ(s, *zv++=j;) j+=(C4)incr;)
 }
 R z;
}    /* w grade"1 w on small-range literal4 */

// We are known to have 1 atom per item
static SF(jtsortu1){F1PREFJT;A x,y,z;C4 *xu,*wv,*zu;I i;void *yv;
 I zr=AR(w); GA(z,AT(w),AN(w),zr,AS(w)); zu=C4AVn(zr,z);
 wv=C4AV(w);
 // choose bucket table size & function; allocate the bucket area
 I (*grcol)(I,I,void*,I,I*,I*,const I,US*,I);  // prototype for either size of buffer
 { I use4 = n>65535; grcol=use4?(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol4:(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol2; GATV0(y,INT,((65536*sizeof(US))>>LGSZI)<<use4,1); yv=AV1(y);}
 GATV0(x,C4T,n,1); xu=C4AV1(x);
 for(i=0;i<m;++i){I colflags;
  colflags=grcol(65536, 0L, yv,n,(UI*)wv,(UI*)xu,sizeof(C4)/sizeof(US),INTLSBWDX+0*WDINC+(US*)wv,4+((~(I)jtinplace>>(JTDESCENDX-1))&2));  // 'sort' + 'up' moved to bit 1
  grcol(65536, 0L, yv,n,(UI*)xu, (UI*)zu,sizeof(C4)/sizeof(US),INTLSBWDX+1*WDINC+(US*)xu ,colflags);
  wv+=n; zu+=n;
 }
 R z;
}    /* w grade"r w on large-range literal4 */


// sort a single real list using quicksort without misprediction, inplace
#define SORTQCOND (C_AVX2 || EMU_AVX2)
#define SORTQNAME sortdq1
#define SORTQTYPE D
#define SORTQSCOPE static
#define SORTQCASTTOPD 
#define SORTQSET256 _mm256_set_pd
#define SORTQTYPE256 __m256d
#define SORTQCMP256(a,b) _mm256_cmp_pd(a,b)
#define SORTQCMPTYPE ,_CMP_GT_OQ
#define SORTQMASKLOAD _mm256_maskload_pd
#define SORTQULOAD _mm256_loadu_pd
#define SORTQULOADTYPE D*
#include "vgsortq.h"

static SF(jtsortdq){F1PREFIP;  // m=#sorts, n=#items in each sort, w is block
 A z; 
 if(ASGNINPLACESGN(SGNIF(jtinplace,JTINPLACEWX),w))z=w; else RZ(z=ca(w));   // output area, possibly the same as the input
 D *zv=DAV(z); DQ(m, sortdq1(zv,n); if((I)jtinplace&JTDESCEND){D *zv1=zv; D *zv2=zv+n; DQ(n>>1, D t=*zv1; *zv1++=*--zv2; *zv2=t;)} zv+=n;)  // sort each list (ascending); reverse if descending
 RETF(z);
}

// We are known to have 1 atom per item
static SF(jtsortd){F1PREFIP;A x,y,z;B b;D*g,*h,*xu,*wv,*zu;I i,nneg;void *yv;
#if 1  // normal case
 // Use quicksort always
 R jtsortdq(jtinplace,m,n,w);  // TUNE
#else
 if((n&3)==0b00)R jtsortdq(jtinplace,m,n,w);
 if((n&3)==0b01)R jtsortdirect(jtinplace,m,1,n,w);  // TUNE
// {{y , 0 _3 _2 {{ 6!:2 '/:~ nn' [ nn =. x }. y }}"0 _  (1.e8) * y ?@$ 0}}"0 <.&.(%&4) 100 * 10 ^ 10 %~ i. 41  NB. tuning not including smallrange: qsort merge radix.  range up to 1e9
#endif
 // falling through for radix sort
 I zr=AR(w); GA(z,AT(w),AN(w),zr,AS(w)); zu=DAVn(zr,z);
 wv=DAV(w);
 // choose bucket table size & function; allocate the bucket area
 I (*grcol)(I,I,void*,I,I*,I*,const I,US*,I);  // prototype for either size of buffer
 { I use4 = n>65535; grcol=use4?(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol4:(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol2; GATV0(y,INT,((65536*sizeof(US))>>LGSZI)<<use4,1); yv=AV1(y);}
 GATV0(x,FL, n,1); xu=DAV1(x);
 for(i=0;i<m;++i){I colflags;  // for each cell to be sorted...
  g=wv; nneg=0; DQ(n, nneg+=(0>*g++);); b=0<nneg&&nneg<n;
  g=b?xu:zu; h=b?zu:xu;  // select correct alignment to end with result in zv
  colflags=grcol(65536,    0L,      yv,n,(I*)wv,(I*)h,sizeof(D)/sizeof(US),FPLSBWDX+0*WDINC+(US*)wv,4+(((I )(nneg==n)<<1)^((~(I)jtinplace>>(JTDESCENDX-1))&2))); // 'sort', plus 'up' in bit 1, but reversed if all neg
  colflags=grcol(65536,    0L,      yv,n,(I*)h, (I*)g,sizeof(D)/sizeof(US),FPLSBWDX+1*WDINC+(US*)h ,colflags);
  colflags=grcol(65536,    0L,      yv,n,(I*)g, (I*)h,sizeof(D)/sizeof(US),FPLSBWDX+2*WDINC+(US*)g ,colflags);
  grcol(32768<<b,(nneg==n)<<15,yv,n,(I*)h, (I*)g,sizeof(D)/sizeof(US),FPLSBWDX+3*WDINC+(US*)h ,colflags);
  if(b){  // if there was not a mix of neg & nonneg, the result is ready now
   g=zu;
   if(colflags&2){h=n+xu; DQ(nneg,   *g++=*--h;); h=  xu; DQ(n-nneg, *g++=*h++;);}  // up: reverse neg, foll by nonneg
   else  {h=nneg+xu; DQ(n-nneg, *g++=*h++;); h=nneg+xu; DQ(nneg,   *g++=*--h;);}  // down: nonneg foll by reverse neg
  }
  wv+=n; zu+=n;
 }
 R z;
}    /* w grade"1 w on real w */


// x /:"r y, not sparse
// jt has the JTDESCEND flag, plus inplaceability
F2(jtgr2){F2PREFIP;PROLOG(0076);A z=0;I acr,api,d,f,m,n,*s,t,wcr; 
 ARGCHK2(a,w);
 // ?cr= rank of the cells being sorted; t= type of w
 acr=jt->ranks>>RANKTX; acr=AR(a)<acr?AR(a):acr; 
 wcr=(RANKT)jt->ranks; wcr=AR(w)<wcr?AR(w):wcr; t=AT(w);
 // Handle special reflexive cases, when the arguments are identical and the cells are also.  Only if cells have rank>0 and have atoms
 if(a==w&&acr==wcr&&wcr>0&&AN(a)&&t&(B01+LIT+C2T+C4T+INT+INT2+INT4+FL+CMPX+QP)){  // tests after the first almost always succeed
  // f = length of frame of w; s->shape of w; m=#cells; n=#items in each cell;
  // d = #bytes in an item of a cell of w
  f=AR(w)-wcr; s=AS(w); PROD(m,f,s); SETICFR(w,f,AR(w),n);  PROD(api,wcr-1,1+f+s);
  d=api<<bplg(t);
   // There are special types supported, but for very short sorts we should just skip the checking and go do a sort-in-place.
   // Test that threshold here
  if(n>5){   //  TUNE
   if(t&(C4T+INT+FL)){
    // If this datatype supports smallrange or radix sorting, go try that
    if(1==api)RZ(z=(t&INT?jtsorti:t&FL?jtsortd:jtsortu)(jtinplace,m,n,w))   // Lists of INT/FL/C4T
   }else if(((d^2)+(t&INT2))==0){
    // 2-byte types (not INT2), which must be B01/LIT/C2T.  Use special code, unless strings too short
    if(t&B01)             RZ(z=sortb2(m,n,w))  // Booleans with cell-items 2 bytes long
    else if(n>4600)RZ(z=sortc2(m,n,w))  // long character strings with cell-items 2 bytes long   TUNE
   }else if(d<2){RZ(z=(t&B01&&(m==1||0==(n&(SZI-1)))?jtsortb:jtsortc)(jtinplace,m,n,w))  // Lists of B01/LIT
   }else if(d==4&&t&B01) RZ(z=sortb4(m,n,w))  // Booleans with cell-items 4 bytes long
  }
   // for direct types, we have the choice of direct/indirect.  For indirect, we do grade followed by from to apply the grading permutation.
   // for direct, we move the data around until we get the final sort.  Direct is better for short items, for which the copy is cheap;
   // and the cutoff rises with the length of the sort, as the indirect sort flags when the items themselves start to fall out of cache.
   // Empirically (Ivy Bridge) we find that the length of the item above which indirect is better depends on sortlength:
   // for 1e6 items - 80 bytes; 1e5 - 64 bytes; 1e4 - 40 bytes - 1e3 - 48 bytes
   // We roughly approximate this curve
  if(!z){  // not a special case
   UI4 lgn=CTLZI(n);
   if(d<40||(UI4)d<(lgn<<4))RZ(z=jtsortdirect(jtinplace,m,api,n,w))  //  TUNE
  }
 }
 // If not a supported reflexive case, grade w and then select those values from a.  jt->ranks is still set
 if(!z){A t;
  I awflg=AFLAG(a);   // Remember original pristinity of a, before calling from which will clear it
  RZ(t=gr1(w)); IRS2(t,a,0L,1L,acr,jtfrom,z); RZ(z);
  // Boxed args will come through here.  Because no cell of a is repeated in the result, we know that if a is pristine,
  // the result will be too, as long as the frames have equal length (and thus must be equal).  If from chose to return virtual z,
  // a will now be non-inplaceable and z will be virtual but not inplaceable.
  AFLAGORLOCAL(z,awflg&((SGNTO0(AC(a))&((I)jtinplace>>JTINPLACEAX)&(((acr-AR(a))^(wcr-AR(w)))-1))<<AFPRISTINEX))
  // But the original a certainly loses pristinity.  We rely on from to have done that, if it didn't create a virtual block
// not needed  if(unlikely(awflg&AFVIRTUAL)){a=ABACK(a); awflg=AFLAG(a);} AFLAG(a)=awflg&~AFPRISTINE;
 }  // if inputs agreed, they will agree with w replaced by /:w and rank by 1
 EPILOG(z);
}    /* a grade"r w main control for dense w */
