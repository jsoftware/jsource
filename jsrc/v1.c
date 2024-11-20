/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Match Associates                                                 */

#include "j.h"
#include "vcomp.h"
#include "ve.h"

#ifdef BOXEDSPARSE
extern UC fboxedsparse;
#endif

#ifdef MMSC_VER
#pragma warning(disable: 4244)
#endif

#define MATCHSUBDEFAULTS ,0,0,1,1,1   // parms not needed if x==0.  We have to pass them anyway because the called function may use them as a workarea, expected to be initialized by the caller
static B jtmatchsub(J,A,A,B* RESTRICT,I,I,I,I,I);
static F2(jtmatchs);

#if !C_AVX2 && !EMU_AVX2

#define MCS(q,af,wf)  ((((q>1)+(q>0))<<2)+(af?2:0)+(wf?1:0))
// set *x++ to b1 if *u=*v, b0 otherwise
#define QLOOP         b=b1; DO(q, if(u[i]!=v[i]){b^=1; break;}); *x++=b;
// comparison, with special cases for 1/more than 1, and looping over repeated cells
// we know there is a frame somewhere.  This is for plain byte comparison - no tolerance
#define EQV(T)        \
 {T h,* RESTRICT u=(T*)av,* RESTRICT v=(T*)wv;                                                   \
  q=k/sizeof(T);                                                             \
  switch(MCS(q,af,wf)){                                                      \
  case MCS(1,0,1): h=*u; if(b1)DQ(mn, *x++=h   ==*v++;) else DQ(mn, *x++=h   !=*v++;)  break;  \
  case MCS(1,1,0): h=*v; if(b1)DQ(mn, *x++=*u++==h;   ) else DQ(mn, *x++=*u++!=h;   ); break;  \
  case MCS(1,1,1): if(b1){                                                  \
                    if(1==n)      DQ(m,               *x++=*u++==*v++;  )   \
                    else if(af<wf)DQ(m, h=*u++; DQ(n, *x++=h   ==*v++;);)   \
                    else          DQ(m, h=*v++; DQ(n, *x++=*u++==h;   ););  \
                   }else{                                                   \
                    if(1==n)      DQ(m,               *x++=*u++!=*v++;  )   \
                    else if(af<wf)DQ(m, h=*u++; DQ(n, *x++=h   !=*v++;);)   \
                    else          DQ(m, h=*v++; DQ(n, *x++=*u++!=h;   ););  \
                   } break;                                                 \
  case MCS(2,0,1): DQ(mn, QLOOP;       v+=q;); break;                       \
  case MCS(2,1,0): DQ(mn, QLOOP; u+=q;      ); break;                       \
  case MCS(2,1,1): if(1==n)      DQ(m,       QLOOP; u+=q;   v+=q;)          \
                   else if(af<wf)DQ(m, DQ(n, QLOOP; v+=q;); u+=q;)          \
                   else          DQ(m, DQ(n, QLOOP; u+=q;); v+=q;); break;  \
  } \
 }

// comparison function for non-float arrays, in chunks of size k, storing match results into *x and
// returning the first one as the result (nothing special about first, could be any one)
// This is a generalized version of the INNERT macro below
// If we get here, we know that at least one of the arguments has a frame
// m=#cells of shorter frame, n=#times a cell of shorter frame must be repeated
static B eqv(I af,I wf,I m,I n,I k,C*av,C*wv,B* RESTRICT x,B b1){B b,* RESTRICT xx=x;I mn=m*n,q;
  // select a comparison loop based on the size of the data area.  It's all about the fastest way to compare bytes
 if     (0==(k&(SZI-1))  )EQV(I)
#if SY_64
 else if(0==(k&(SZI4-1)))EQV(int)
#endif
 else if(0==(k&(SZS-1))  )EQV(S)
 else if(1==k)            EQV(C)
 else{
  if(af<wf)DQ(m, DQ(n, *x++=(!!memcmp(av,wv,k))^b1; wv+=k;); av+=k;)
  else          DQ(m, DQ(n, *x++=(!!memcmp(av,wv,k))^b1; av+=k;); wv+=k;);
 }
 R xx[0];
}    /* what memcmp should have been */

#else
static AHDR2FN* compfn[4][2] = {{(AHDR2FN*)neCC, (AHDR2FN*)eqCC} , {(AHDR2FN*)neSS, (AHDR2FN*)eqSS} , {(AHDR2FN*)neUU, (AHDR2FN*)eqUU} , {(AHDR2FN*)neII, (AHDR2FN*)eqII} };
static B eqv(I af,I wf,I m,I n,I k,C* RESTRICT av,C* RESTRICT wv,B* RESTRICT z,B b1){
 // Handle the special cases where the number of bytes to compare is a standard type.  Vector to the routine for comparing the type
 if(k==((k&-k)&(2*SZI-1))){
  // length is 1, 2, 4, or 8.  There's a routine for that, for = or ~:
  compfn[CTTZI(k)][b1]AH2A_nm(n,m,wv,av,z,0);  // no jt for these routines.  Swap wv/av because n is positive, which will mean repeat the first arg
  R 0;  // return value immaterial
}
 I n0=(k-1)>>LGSZI;  // number of Is to process (after we skip the first 1-8 chars); later, # repeat count in inner loop
 __m256i u,v;
 // prep for each compare loop
 __m256i endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-n0)&(NPAR-1))));  /* mask for 0 1 2 3 4 5 is xxxx 0001 0011 0111 1111 0001 */
 UI n2=DUFFLPCT(n0-1,3);  /* # turns through duff loop */
 UI backoff=DUFFBACKOFF(n0-1,3);
 b1^=1;  // change success value to failure value
 // loop for each result
 // loop promotion: if cells of a are not repeated, modify loop counts to do outer loop once and inner loop for each rep
 I ka;  // amount to add to av in inner loop: k if the cells of a are not repeated, 0 if they are
 n0=m; ka=n==1?k:0; m=n==1?1:m; n=n==1?n0:n;  // move outer loop info to inner if inner loop not needed
 do{  // outer loop, for each unique cell of a
  n0=n;
  do{B b;  // inner loop, for each repeat of a
   // compare up to 8 bytes.  This checks for early failure & allows us to operate on full qwords hereafter
   {I ll=SZI; ll=k<ll?k:ll; I comp=(*(I*)av^*(I*)wv)<<(((-ll)&(SZI-1))<<LGBB); comp=-((I*)comp==0); I l8=8-k; if((l8&comp)>=0){b = b1^1^(comp-REPSGN(l8-8)); goto fail;}}  // if mismatch in 1st 8 bytes, or len <=8, return mismatch status  -(comp==0)) is ~0 if comp==0
  // fetch the load mask for the last block: the words to load, including any trailing fragment
   // step up to qword boundary
   I *x=(I*)((C*)av+((k-1)&(SZI-1))+1), *y=(I*)((C*)wv+((k-1)&(SZI-1))+1);  // access the arguments as Is
   __m256i allmatches =_mm256_cmpeq_epi8(endmask,endmask); __m256d ones=_mm256_castsi256_pd(allmatches); // accumuland for compares init to all 1
   b=b1;  // init store value to compare failure
   if(n2>0){
    UI i = n2;  // inner loop size
    x+=(backoff+1)*NPAR; y+=(backoff+1)*NPAR;
    switch(backoff){
    do{
    case -1: u=_mm256_loadu_si256 ((__m256i*)x); v=_mm256_loadu_si256 ((__m256i*)y); allmatches=_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v));
    case -2: u=_mm256_loadu_si256 ((__m256i*)(x+1*NPAR)); v=_mm256_loadu_si256 ((__m256i*)(y+1*NPAR)); allmatches=_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v));
    case -3: u=_mm256_loadu_si256 ((__m256i*)(x+2*NPAR)); v=_mm256_loadu_si256 ((__m256i*)(y+2*NPAR)); allmatches=_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v));
    case -4: u=_mm256_loadu_si256 ((__m256i*)(x+3*NPAR)); v=_mm256_loadu_si256 ((__m256i*)(y+3*NPAR)); allmatches=_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v));
    case -5: u=_mm256_loadu_si256 ((__m256i*)(x+4*NPAR)); v=_mm256_loadu_si256 ((__m256i*)(y+4*NPAR)); allmatches=_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v));
    case -6: u=_mm256_loadu_si256 ((__m256i*)(x+5*NPAR)); v=_mm256_loadu_si256 ((__m256i*)(y+5*NPAR)); allmatches=_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v));
    case -7: u=_mm256_loadu_si256 ((__m256i*)(x+6*NPAR)); v=_mm256_loadu_si256 ((__m256i*)(y+6*NPAR)); allmatches=_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v));
    case -8: u=_mm256_loadu_si256 ((__m256i*)(x+7*NPAR)); v=_mm256_loadu_si256 ((__m256i*)(y+7*NPAR)); allmatches=_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v));
    x+=8*NPAR; y+=8*NPAR;
    if(n2==1)goto oneloop;  // if we don't have to loop here, avoid the data-dependent branch and fold the comparisons into the last batch 
     if(!_mm256_testc_pd(_mm256_castsi256_pd(allmatches),ones))goto fail;  // if searches are long, kick out when there is a miscompare.  test is '!(all sign bits of allmatches =1)'
    }while(--i>0);
    }
oneloop:;
   }
   u=_mm256_maskload_epi64(x,endmask); v=_mm256_maskload_epi64(y,endmask); 
   b ^= _mm256_testc_pd(_mm256_castsi256_pd(_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v))),ones);  // no miscompares, switch failure value to success.  test 1=good
fail:
   *z++=b;  // store one result
   wv += k; av+= ka;  // advance w always, and a if original m was 1
  }while(--n0);
  av += k;  // advance a, only needed if a was repeated
 }while(--m);

 R 0;  // return value not used - results stored in *z
}
// memcmpne: test for inequality, not caring about order, for exact inputs
// We use AVX2 instructions always, so this might be a little slower for repeat matches on short inputs; but it avoids misbranches
I memcmpne(void *s, void *t, I l){
 if(unlikely(l==0))R 0; // loops require nonempty arrays - empties compare equal.  If there are no atoms we can't safely fetch anything from memory
 // If the first fetch miscompares, we can avoid the setup overhead.  This will be worthwhile on long compares, and not too
 // expensive on short ones.  We roll arg-length testing and value testing into one
 {I ll=SZI; ll=l<ll?l:ll; I comp=(*(I*)s^*(I*)t)<<(((-ll)&(SZI-1))<<LGBB); comp=-((I*)comp==0); I l8=8-l; if((l8&comp)>=0)R comp-REPSGN(l8-8);}  // if mismatch in 1st 8 bytes, or len <=8, return mismatch status  -(comp==0)) is ~0 if comp==0
 // fetch the load mask for the last block: the words to load, including any trailing fragment
 // step up to qword boundary
 I *x=(I*)((C*)s+((l-1)&(SZI-1))+1), *y=(I*)((C*)t+((l-1)&(SZI-1))+1);  // access the arguments as Is
 I n=(l-1)>>LGSZI;  // number of Ds to process - cannot be 0
 __m256i u,v;
 __m256i endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-n)&(NPAR-1))));  // mask for 0 1 2 3 4 5 is xxxx 0001 0011 0111 1111 0001
 __m256d ones=_mm256_castsi256_pd(_mm256_cmpeq_epi8(endmask,endmask));

 UI n2=DUFFLPCT(n-1,3);  /* # turns through duff loop */
 if(n2>0){
  __m256i allmatches =_mm256_castpd_si256(ones); // accumuland for compares init to all 1
  UI backoff=DUFFBACKOFF(n-1,3);
  x+=(backoff+1)*NPAR; y+=(backoff+1)*NPAR;
  switch(backoff){
  do{
  case -1: u=_mm256_loadu_si256 ((__m256i*)x); v=_mm256_loadu_si256 ((__m256i*)y); allmatches=_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v));
  case -2: u=_mm256_loadu_si256 ((__m256i*)(x+1*NPAR)); v=_mm256_loadu_si256 ((__m256i*)(y+1*NPAR)); allmatches=_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v));
  case -3: u=_mm256_loadu_si256 ((__m256i*)(x+2*NPAR)); v=_mm256_loadu_si256 ((__m256i*)(y+2*NPAR)); allmatches=_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v));
  case -4: u=_mm256_loadu_si256 ((__m256i*)(x+3*NPAR)); v=_mm256_loadu_si256 ((__m256i*)(y+3*NPAR)); allmatches=_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v));
  case -5: u=_mm256_loadu_si256 ((__m256i*)(x+4*NPAR)); v=_mm256_loadu_si256 ((__m256i*)(y+4*NPAR)); allmatches=_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v));
  case -6: u=_mm256_loadu_si256 ((__m256i*)(x+5*NPAR)); v=_mm256_loadu_si256 ((__m256i*)(y+5*NPAR)); allmatches=_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v));
  case -7: u=_mm256_loadu_si256 ((__m256i*)(x+6*NPAR)); v=_mm256_loadu_si256 ((__m256i*)(y+6*NPAR)); allmatches=_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v));
  case -8: u=_mm256_loadu_si256 ((__m256i*)(x+7*NPAR)); v=_mm256_loadu_si256 ((__m256i*)(y+7*NPAR)); allmatches=_mm256_and_si256(allmatches,_mm256_cmpeq_epi64(u,v));
  x+=8*NPAR; y+=8*NPAR;
   if(!_mm256_testc_pd(_mm256_castsi256_pd(allmatches),ones))R 1;  // test is '!(all sign bits of allmatches=1)'
  }while(--n2>0);
  }
 }

 u=_mm256_maskload_epi64(x,endmask); v=_mm256_maskload_epi64(y,endmask); 
 R !_mm256_testc_pd(_mm256_castsi256_pd(_mm256_cmpeq_epi64(u,v)),ones);  // return 1 if any mismatch
}

// memcmpnefl: test for inequality, not caring about order, for float inputs, possibly with tolerance
// We use AVX2 instructions always, so this might be a little slower for repeat matches on short inputs; but it avoids misbranches
// l is # atoms (D) to process
I memcmpnefl(void *s, void *t, I l, J jt){
 if(l==0)R 0;  // loops require nonempty arrays
 // fetch the load mask for the last block: the words to load, including any trailing fragment
 // fetch the mask of valid bytes in the last batch fetched, maybe less than the load mask
 D *x=s, *y=t;  // access the arguments as doubles
 __m256d u,v;
 __m256i endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-l)&(NPAR-1))));  // mask for 0 1 2 3 4 5 is xxxx 0001 0011 0111 1111 0001
 __m256d ones=_mm256_castsi256_pd(_mm256_cmpeq_epi8(endmask,endmask));
 if(jt->cct==1.0){
  // intolerant comparison
  UI n2=DUFFLPCT(l-1,3);  /* # turns through duff loop */
  if(n2>0){
   __m256d allmatches =_mm256_castsi256_pd(_mm256_cmpeq_epi8(endmask,endmask)); // accumuland for compares init to all 1
   UI backoff=DUFFBACKOFF(l-1,3);
   x+=(backoff+1)*NPAR; y+=(backoff+1)*NPAR;
   switch(backoff){
   do{
   case -1: u=_mm256_loadu_pd(x); v=_mm256_loadu_pd(y); allmatches=_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ));
   case -2: u=_mm256_loadu_pd(x+1*NPAR); v=_mm256_loadu_pd(y+1*NPAR); allmatches=_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ));
   case -3: u=_mm256_loadu_pd(x+2*NPAR); v=_mm256_loadu_pd(y+2*NPAR); allmatches=_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ));
   case -4: u=_mm256_loadu_pd(x+3*NPAR); v=_mm256_loadu_pd(y+3*NPAR); allmatches=_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ));
   case -5: u=_mm256_loadu_pd(x+4*NPAR); v=_mm256_loadu_pd(y+4*NPAR); allmatches=_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ));
   case -6: u=_mm256_loadu_pd(x+5*NPAR); v=_mm256_loadu_pd(y+5*NPAR); allmatches=_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ));
   case -7: u=_mm256_loadu_pd(x+6*NPAR); v=_mm256_loadu_pd(y+6*NPAR); allmatches=_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ));
   case -8: u=_mm256_loadu_pd(x+7*NPAR); v=_mm256_loadu_pd(y+7*NPAR); allmatches=_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ));
   x+=8*NPAR; y+=8*NPAR;
    if(!_mm256_testc_pd(allmatches,ones))R 1;  // test is '!(all bits of allmatches=1)'
   }while(--n2>0);
   }
  }
  u=_mm256_maskload_pd(x,endmask); v=_mm256_maskload_pd(y,endmask); 
  R !_mm256_testc_pd(_mm256_cmp_pd(u,v,_CMP_EQ_OQ),ones);   // return 1 if any mismatch
 }
 UI i=(l-1)>>LGNPAR;  /* # loops for 0 1 2 3 4 5 is x 0 0 0 0 1 */
 // tolerant comparison
 __m256d cct=_mm256_broadcast_sd(&jt->cct);
 if(i){
  do{
   u=_mm256_loadu_pd(x); v=_mm256_loadu_pd(y); x+=NPAR; y+=NPAR;
   if(!_mm256_testc_pd(_mm256_xor_pd(_mm256_cmp_pd(u,_mm256_mul_pd(v,cct),_CMP_GT_OQ),_mm256_cmp_pd(v,_mm256_mul_pd(u,cct),_CMP_LE_OQ)),ones))R 1;
  }while(--i>0);
 }
 u=_mm256_maskload_pd(x,endmask); v=_mm256_maskload_pd(y,endmask); 
 R !_mm256_testc_pd(_mm256_xor_pd(_mm256_cmp_pd(u,_mm256_mul_pd(v,cct),_CMP_GT_OQ),_mm256_cmp_pd(v,_mm256_mul_pd(u,cct),_CMP_LE_OQ)),ones);
}

static B eqvfl(I af,I wf,I m,I n,I k,D* RESTRICT av,D* RESTRICT wv,B* RESTRICT z,B b1,J jt){
 // Handle the special cases where the number of bytes to compare is a standard type.  Vector to the routine for comparing the type
 if(k==1){
  // individual floats.  call the routine for that
  (b1?eqDD:neDD)AH2A_nm(n,m,wv,av,z,jt);  // Swap wv/av because n is positive, which will mean repeat the first arg
  R 0;  // return value immaterial
 }
 __m256d u,v;
 // prep for each compare loop
 __m256i endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-k)&(NPAR-1))));  // mask for 0 1 2 3 4 5 is xxxx 0001 0011 0111 1111 0001
 __m256d ones=_mm256_castsi256_pd(_mm256_cmpeq_epi8(endmask,endmask));
 UI n2=DUFFLPCT(k-1,3);  /* # turns through duff loop */
 UI backoff=DUFFBACKOFF(k-1,3);
 UI i0=(k-1)>>LGNPAR;  /* # loops for 0 1 2 3 4 5 is x 0 0 0 0 1 used for tolerant */
 b1^=1;  // change success value to failure value
 // loop for each result
 // loop promotion: if cells of a are not repeated, modify loop counts to do outer loop once and inner loop for each rep
 I ka;  // amount to add to av in inner loop: k if the cells of a are not repeated, 0 if they are
 I n0=m; ka=n==1?k:0; m=n==1?1:m; n=n==1?n0:n;  // move outer loop info to inner if inner loop not needed
 do{  // outer loop, for each unique cell of a
  n0=n;  // inner loop count
  do{  // inner loop, for each repeat of a
   D *x=av, *y=wv;  // init arg pointers to start of cell
   B b=b1;  // init store value to compare failure

   if(jt->cct==1.0){
    // intolerant comparison
    __m256d allmatches =_mm256_castsi256_pd(_mm256_cmpeq_epi8(endmask,endmask)); // accumuland for compares init to all 1
    if(n2>0){
     UI i = n2;  // inner loop size
     x+=(backoff+1)*NPAR; y+=(backoff+1)*NPAR;
     switch(backoff){
     do{
     case -1: u=_mm256_loadu_pd(x); v=_mm256_loadu_pd(y); allmatches=_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ));
     case -2: u=_mm256_loadu_pd(x+1*NPAR); v=_mm256_loadu_pd(y+1*NPAR); allmatches=_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ));
     case -3: u=_mm256_loadu_pd(x+2*NPAR); v=_mm256_loadu_pd(y+2*NPAR); allmatches=_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ));
     case -4: u=_mm256_loadu_pd(x+3*NPAR); v=_mm256_loadu_pd(y+3*NPAR); allmatches=_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ));
     case -5: u=_mm256_loadu_pd(x+4*NPAR); v=_mm256_loadu_pd(y+4*NPAR); allmatches=_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ));
     case -6: u=_mm256_loadu_pd(x+5*NPAR); v=_mm256_loadu_pd(y+5*NPAR); allmatches=_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ));
     case -7: u=_mm256_loadu_pd(x+6*NPAR); v=_mm256_loadu_pd(y+6*NPAR); allmatches=_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ));
     case -8: u=_mm256_loadu_pd(x+7*NPAR); v=_mm256_loadu_pd(y+7*NPAR); allmatches=_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ));
     x+=8*NPAR; y+=8*NPAR;
     if(n2==1)goto oneloop;  // if we don't have to loop here, avoid the data-dependent branch and fold the comparisons into the last batch 
      if(!_mm256_testc_pd(allmatches,ones))goto fail;  // test is '!(all sign bits of allmatches=1)'
     }while(--i>0);
     }
    }
oneloop:
    u=_mm256_maskload_pd(x,endmask); v=_mm256_maskload_pd(y,endmask); 
    b ^= _mm256_testc_pd(_mm256_and_pd(allmatches,_mm256_cmp_pd(u,v,_CMP_EQ_OQ)),ones);  // no miscompares, switch failure value to success.  test 1=good
   }else{
    // tolerant comparison
    __m256d cct=_mm256_broadcast_sd(&jt->cct);
    if(i0){
     UI i = i0;  // inner loop size
     do{  // unfortunately it's probably not worth checking for lengths 5-8 & we will have a misbranch whenever length > 4
      u=_mm256_loadu_pd(x); v=_mm256_loadu_pd(y); x+=NPAR; y+=NPAR;
      if(!_mm256_testc_pd(_mm256_xor_pd(_mm256_cmp_pd(u,_mm256_mul_pd(v,cct),_CMP_GT_OQ),_mm256_cmp_pd(v,_mm256_mul_pd(u,cct),_CMP_LE_OQ)),ones))goto fail;
     }while(--i>0);
    }
    u=_mm256_maskload_pd(x,endmask); v=_mm256_maskload_pd(y,endmask); 
    b ^= _mm256_testc_pd(_mm256_xor_pd(_mm256_cmp_pd(u,_mm256_mul_pd(v,cct),_CMP_GT_OQ),_mm256_cmp_pd(v,_mm256_mul_pd(u,cct),_CMP_LE_OQ)),ones);
   }

fail:
   *z++=b;  // store one result
   wv += k; av+= ka;  // advance w always, and a if original m was 1
  }while(--n0);
  av += k;  // advance a, only needed if a was repeated
 }while(--m);

 R 0;  // return value not used - results stored in *z
}

#endif

// Return 1 if a and w match, 0 if not
B jtequ(J jt,A a,A w){A x;
 F2PREFIP;ARGCHK2(a,w);  // allow inplace request - it has no effect
 if(a==w)R 1;
 if(unlikely(ISSPARSE(AT(a)|AT(w))))if(AR(a)&&AR(w)){RZ(x=matchs(a,w)); R BAV(x)[0];}
 R ((B (*)())jtmatchsub)(jt,a,w,0   MATCHSUBDEFAULTS);  // don't check level - it takes too long for big arrays
}

// Return 1 if a and w match, 0 if not
B jtequx(J jt,X a,X w){R 0==icmpXX(a,w);}

// Return 1 if a and w match, 0 if not
B jteqx(J jt,A a,A w){A x;
 F2PREFIP;ARGCHK2(a,w);  // allow inplace request - it has no effect
 if(a==w)R 1;
 if(unlikely(ISSPARSE(AT(a)|AT(w))))if(AR(a)&&AR(w)){RZ(x=matchs(a,w)); R BAV(x)[0];}
 R ((B (*)())jtmatchsub)(jt,a,w,0   MATCHSUBDEFAULTS);  // don't check level - it takes too long for big arrays
}

// Return 1 if a and w match intolerantly, 0 if not
B jtequ0(J jt,A a,A w){
 F2PREFIP;  // allow inplace request - it has no effect
 PUSHCCT(1.0) B res=equ(a,w); POPCCT R res;
}

// Test name equality, looking only at name strings
static B eqname(A a,A w){
 if(TYPESXOR(AT(a),AT(w)))R 0;   // both must be names
 if(AN(a)!=AN(w))R 0;  // full namelen matches
 if(NAV(a)->m!=NAV(w)->m)R 0;  // len of simplename
 R !memcmp(NAV(a)->s,NAV(w)->s,AN(a));  // compare name itself
}

// Test for equality of functions, 1 if they match.  To match, the functions must have the same pseudocharacter and fgh
B jteqf(J jt,A a,A w){A p,q;V*u=FAV(a),*v=FAV(w);
 if(TYPESXOR(AT(a),AT(w))+(u->id^v->id))R 0;   // must match on type and id
 p=u->fgh[0]; q=v->fgh[0]; if(!((p==q||(p==0)==(q==0)&&((B (*)())jtmatchsub)(jt,p,q,0   MATCHSUBDEFAULTS))))R 0;
 p=u->fgh[1]; q=v->fgh[1]; if(!((p==q||(p==0)==(q==0)&&((B (*)())jtmatchsub)(jt,p,q,0   MATCHSUBDEFAULTS))))R 0;
 if(u->id==CCOLONE&&!(u->flag&VXOP)&&u->fgh[0]!=0){
  // explicit definition (original, not filled in with operands, not nameless).  Compare equal if the text matches exactly (including comments) - tolerance and type made it hard to match based on h[0]
  if(!(v->id==CCOLONE&&!(v->flag&VXOP)&&v->fgh[0]!=0))R 0;  // both must be explicit
  DO(2, p=AAV1(u->fgh[2])[HN*i+2]; q=AAV1(v->fgh[2])[HN*i+2];
   if(p!=q){if((p==0)!=(q==0))R 0; if(!((B (*)())jtmatchsub)(jt,p,q,0  MATCHSUBDEFAULTS))R 0;}  // check source text
  )
  R 1;  // if no differences, call them equal
 }else {p=u->fgh[2]; q=v->fgh[2];    R (p==q||(p==0)==(q==0)&&((B (*)())jtmatchsub)(jt,p,q,0   MATCHSUBDEFAULTS));}
}

// compare function for boxes.  Do a test on the single contents of the box.  Reset comparison direction to normal.
// In case the boxes have QC flags (possible if we are comparing functions), mask off the flags
#ifndef BOXEDSPARSE
#define EQA(a,w) \
 ({ A ma=QCWORD(C(a)), mw=QCWORD(C(w)); ((-(ma!=mw)&((AN(ma)^AN(mw))-1))>=0?(ma==mw):((B (*)())jtmatchsub)(jt,ma,mw,0   MATCHSUBDEFAULTS)); })
#else
#define EQA(a,w) \
 ({ A ma=C(QCWORD(a)), mw=C(QCWORD(w)); (((!ISSPARSE(AT(ma)|AT(mw)))&&((-(ma!=mw)&((AN(ma)^AN(mw))-1))>=0))?(ma==mw):((B (*)())jtmatchsub)(jt,ma,mw,0   MATCHSUBDEFAULTS)); })
#endif
// compare rationals
#define EQQ(a,w)  (equx(a.n,w.n)&&equx(a.d,w.d))

// compare arrays for equality of all values.  f is the compare function
// m=#cells of shorter frame, n=#times a cell of shorter frame must be repeated
// x[] is result array.  This can be 0 if we are doing a comparison inside a box, in which case
// we don't store the result.  In any case, b holds the result of the last comparison
#define INNERT(T,f)                  \
 {T* RESTRICT u=(T*)av,* RESTRICT v=(T*)wv; I m0=m, n0=n; m0=x==0?1:m0; n0=x==0?1:n0;  /* u->a data, v->w data */           \
  if(1==n0){      DO(m0,       B b=b1; DO(c, if(!f(u[i],v[i])){b^=1; break;}); if(!x)R b^b1^1; x[j++]=b; u+=c; v+=c;)}  \
  else if(af>wf)DO(m0, DO(n0, B b=b1; DO(c, if(!f(u[i],v[i])){b^=1; break;}); x[j++]=b; u+=c;); v+=c;)  \
  else          DO(m0, DO(n0, B b=b1; DO(c, if(!f(u[i],v[i])){b^=1; break;}); x[j++]=b; v+=c;); u+=c;)  \
 }

// compare functions for float/complex intolerant comparison
#define DEQCT0(a,w) ((a)==(w))
#define ZEQCT0(a,w) ( ((a).re==(w).re) && ((a).im==(w).im) )

// match two values, returning b1 if match, 1^b1 if no match.  If the values are functions, that's all we return.  If the values are nouns, we
// store the match value(s) in *x.  x may be 0, if af and wf are 0 and m and n are 1.  In this case we don't store anything.
// but return the match status.  We use this when comparing boxed arrays or functions
// For recursive calls, x=0, af=wf=0, and m=n=1.  If there is frame, arguments have been exchanged so that a has the shorter frame, i. e. af<=wf, and a will be repeated
// b1 is the value to use for 'match' - 1 normally, but 0 for top level of -.@-:
// m=#cells of shorter frame, n=#times a cell of shorter frame must be repeated
// the comparands may not be sparse
// arguments after x may be omitted if x==0, and are then assumed to be 0 0 1 1 1
static B jtmatchsub(J jt,A a,A w,B* RESTRICT x,I af,I wf,I m,I n,I b1){C*av,*wv;I at,c,j=0,t,wt;
 // we tested for a==w before the call, to save on call overhead (usually)
 // m*n cannot be 0.  If this is a recursive call, m=n=1; while if it is the first call, empty m/n were handled at the top level
 {
 af&=(-(x!=0)); wf&=(-(x!=0));   // if x not given, use defaults for af, wf
 I p=AR(a)-af;
 I q=AR(w)-wf; 
 // p=cell-rank of a; q=cell-rank of w; ?t=type;
 // c=#atoms in a cell, b is 1 if rank or cell-shape mismatches, or if cells are not empty and types are incompatible
 // We know that either there is no frame or both arguments are nonempty (Empty arguments with frame can happen only at the top level
 // and were handled there).
 // Do all the tests for miscompare and combine the results at the end
 // We assume the caller checked for a==w and handled it, so we don't.
 I shapediff=p;  // see if ranks differ
 p=q<p?q:p; q^=shapediff; TESTDISAGREE(shapediff,af+AS(a),wf+AS(w),p); // now p is smaller rank; q=ranks differ; shapediff=shapes differ
 shapediff|=q;  // shapes or ranks differ
 PROD(c,p,af+AS(a));  // get c=length of a cell in atoms
 at=AT(a); wt=AT(w);   // save types, now that register pressure is over
 p=NEGIFHOMO(at,wt);  // now p= neg if homogeneous args
 if(((shapediff-1)&(-c)&p)>=0){  // skip compare if rank differ, or if shapes differ, or if inhomo, or if empty; not checking for identical args
  // create result, !b1 if there was a difference in shape or inhomo, b1 otherwise
  p=1^SGNTO0((shapediff-1)&((c-1)|p));  // p=1 if error, =no match  ignore inhomo error if empty
  if(x)mvc(m*n,x,1,iotavec-IOTAVECBEGIN+(p^b1));else b1=1; R p^b1;  // write 'error' if writing enabled; return false
 }
 }

 // If we're comparing functions, return that result
 t=at;  //  in case types identical, pick one
 if(unlikely(t&FUNC))R (!eqf(a,w))^(x==0?1:b1);  // true value, but switch if return is not 'match'
 if(unlikely(t&NAME))R (!eqname(a,w))^(x==0?1:b1);  // true value, but switch if return is not 'match'
#ifdef BOXEDSPARSE
 if(fboxedsparse) if(unlikely(ISSPARSE(at|wt)))R num(1)==matchs(a,w);
#endif
 // If the types mismatch, convert as needed to the common (unsafe) type calculated earlier
 if(at!=wt) {
  t=maxtypedne(at,wt);
  if(at!=t)RZ(a=cvt(t|VFRCEXMT,a));
  if(wt!=t)RZ(w=cvt(t|VFRCEXMT,w));
 }
 // If a has no frame, it might be the shorter frame and therefore repeated; but in that case
 // m will be 1 (1 cell in shorter frame).  So it is safe to increment each address by c in the compare loops
 av=CAV(a);
 wv=CAV(w);
 // do the comparison, leaving the last result in b
 switch(CTTZ(t)){
  // Take the case of no frame quickly, because it happens on each recursion and also in much user code
 default:   // all direct exact types
  c <<= bplg(t);
  if(!x){
#if C_AVX2 || EMU_AVX2
   R memcmpne(av,wv,c)^1;   // single call, thus not stored - return it immediately
#else
  
  R !memcmp(av,wv,c);   // single call, thus not stored - return it immediately
#endif
  }else{
   R eqv(af,wf,m,n,c,av,wv,x,b1);  // stored version loops & stores
  }
  break;
 case CMPXX: if(1.0!=jt->cct){INNERT(Z,zeq) break;}  // tolerant, must use complex distance
#if C_AVX2 || EMU_AVX2
  c*=2;   // intolerant: treat as 2 floats, fall through
#else
  INNERT(Z,ZEQCT0) break;
#endif
 case FLX:
#if C_AVX2 || EMU_AVX2
  if(!x){
   R memcmpnefl(av,wv,c,jt)^1;   // single call, thus not stored - return it immediately
  }else{
   R eqvfl(af,wf,m,n,c,(D*)av,(D*)wv,x,b1,jt);  // stored version loops & stores
  }
#else
   if(1.0!=jt->cct)INNERT(D,TEQ)else INNERT(D,DEQCT0)
#endif
  break;
 case QPX: INNERT(E,EQE); break;
 case XNUMX: INNERT(X,equx); break;
 case RATX:  INNERT(Q,EQQ); break;
 case BOXX:  INNERT(A,EQA); break;
 }
 R 0;  // Return value matters only for single compare (x=0); we have returned already in that case
}

static F2(jtmatchs){A ae,ax,p,q,we,wx,x;B*b,*pv,*qv;D d;I acr,an=0,ar,c,j,k,m,n,r,*s,*v,wcr,wn=0,wr;P*ap,*wp;
 ARGCHK2(a,w);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr; r=ar;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; RESETRANK;
 if(ar>acr||wr>wcr)R rank2ex(a,w,DUMMYSELF,acr,wcr,acr,wcr,jtmatchs);
 if(ar!=wr||memcmpne(AS(a),AS(w),r*SZI)||!HOMO(AT(a),AT(w)))R num(0);
 GATV0(x,B01,r,1L); b=BAVn(1L,x); mvc(r,b,MEMSET00LEN,MEMSET00);
 if(ISSPARSE(AT(a))){ap=PAV(a); x=SPA(ap,a); v=AV(x); an=AN(x); DO(an, b[v[i]]=1;);}
 if(ISSPARSE(AT(w))){wp=PAV(w); x=SPA(wp,a); v=AV(x); wn=AN(x); DO(wn, b[v[i]]=1;);} 
 c=0; DO(r, c+=b[i];);
 if(an<c||!ISSPARSE(AT(a)))RZ(a=reaxis(ifb(r,b),a)); ap=PAV(a); ae=SPA(ap,e); ax=SPA(ap,x); m=AS(ax)[0];
 if(wn<c||!ISSPARSE(AT(w)))RZ(w=reaxis(ifb(r,b),w)); wp=PAV(w); we=SPA(wp,e); wx=SPA(wp,x); n=AS(wx)[0];
 RZ(x=indexof(SPA(ap,i),SPA(wp,i))); v=AV(x);
 GATV0(p,B01,m,1); pv=BAV1(p);
 GATV0(q,B01,n,1); qv=BAV1(q); 
 mvc(m,pv,1,MEMSET01); DO(n, j=*v++; if(j<m)pv[j]=qv[i]=0; else qv[i]=1;);
 if(memchr(pv,C1,m)&&!all1(eq(we,repeat(p,ax))))R num(0);
 if(memchr(qv,C1,n)&&!all1(eq(ae,repeat(q,wx))))R num(0);
 j=0; DO(m, if(pv[i])++j;);
 k=0; DO(n, if(qv[i])++k; qv[i]=!qv[i];);
 if(!equ(from(repeat(q,x),ax),repeat(q,wx)))R num(0);
 x=SPA(ap,a); v=AV(x); s=AS(a); d=1.0; DO(AN(x), d*=s[v[i]];);
 R d==m+k&&d==n+j||equ(ae,we)?num(1):num(0);
}    /* a -:"r w on sparse arrays */


// x -:"r y or x -.@-:"r y depending on LSB of jt
F2(jtmatch){A z;I af,m,n,mn,wf;
 I eqis0 = (I)jt&1; jt=(J)((I)jt&~1);
 ARGCHK2(a,w);
 I isatoms = (-AN(a))&(-AN(w));  // neg if both args have atoms
 if(unlikely(ISSPARSE(AT(a)|AT(w))))R ne(num(eqis0),matchs(a,w));
 af=AR(a)-(I)(jt->ranks>>RANKTX); af=af<0?0:af; wf=AR(w)-(I)((RANKT)jt->ranks); wf=wf<0?0:wf; RESETRANK;
 // exchange a and w as needed to ensure a has the shorter frame, i. e. is the repeated argument
 {A ta=a; I ti=af; I afhi=af-wf; a=afhi>=0?w:a; w=afhi>=0?ta:w; af=afhi>=0?wf:af; wf=afhi>=0?ti:wf;} 
 // If either operand is empty return without any comparisons.  In this case we have to worry that the
 // number of cells may overflow, even if there are no atoms
 if(isatoms>=0){B b; I p;  // AN(a) is 0 or AN(w) is 0
  // no atoms.  The shape of the result is the length of the longer frame.  See how many cells that is
  PRODX(mn,wf,AS(w),1)
  // The compare for each cell is 1 if the cell-shapes are the same
  p=AR(a)-af; b=p==(AR(w)-wf)&&!ICMP(af+AS(a),wf+AS(w),p);   // b =  shapes are the same
  // Allocate & return result
  GATV(z,B01,mn,wf,AS(w)); mvc(mn,BAVn(wf,z),1,iotavec-IOTAVECBEGIN+(b^eqis0)); R z;
 }
 // There are atoms.  If there is only 1 cell to compare, do it quickly
 if(wf==0){
 I nocall = (-(a!=w)&((AN(a)^AN(w))-1));
 R num((nocall>=0?SGNTO0(nocall)+(a==w):((B (*)())jtmatchsub)(jt,a,w,0   MATCHSUBDEFAULTS))^eqis0);  // SGNTO0 to prevent misbranch
 }
 // Otherwise we are doing match with rank.  Set up for the repetition in matchsub
 // Create m: #cells in shorter (i. e. common) frame  n: # times cell of shorter frame is repeated
 PROD(m,af,AS(w)); PROD(n,wf-af,AS(w)+af);
 mn=m*n;  // total number of matches to do, i. e. # results
 GATV(z,B01,mn,wf,AS(w)); matchsub(a,w,BAVn(wf,z),af,wf,m,n,eqis0^1);  // matchsub stores, and we ignore the result
 // We do not check for a==w here & thus will compare them
 RETF(z);
}    /* a -:"r w */

F2(jtnotmatch){R jtmatch((J)((I)jt+1),a,w);}   /* a -.@-:"r w */
