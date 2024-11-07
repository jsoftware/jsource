/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Interpreter Utilities                                                   */

#include "j.h"
#include "vasm.h"


#if SY_64

// If jt is 0, don't call jsignal when there is an error
// Returns x*y, or 0 if there is an error (and in that case jsignal might have been called)
#ifdef DPMULD
I jtmult(J jt, I x, I y){I z; DPMULDDECLS DPMULD(x,y,z,{if(jt)jsignal(EVLIMIT);R 0;}) R z;}
#else
I jtmult(J jt, I x, I y){I z;I const lm = 0x00000000ffffffffLL; I const hm = 0xffffffff00000000LL;
  I const lmsb = 0x0000000080000000LL; I const hlsb = 0x0000000100000000;
 // if each argument fits in unsigned-32, do unsigned multiply (normal case); make sure result doesn't overflow
 if(!(hm &(x|y))){if(0 > (z = (I)((UI)x * (UI)y))){if(jt)jsignal(EVLIMIT);R 0;}}
 // if each argument fits in signed-32, do signed multiply; no overflow possible
 else if (!(hm &((x+0x80000000LL)|(y+0x80000000LL))))z = x * y;
 else {
   // if x and y BOTH have signed significance in high 32 bits, that's too big; and
   // MAXNEG32*MAXNEG32 is as well, because it needs 65 bits to be represented.
   // all the low 32 bits of x/y are unsigned data
  if(((hm & (x + (x & hlsb))) && (hm & (y + (y & hlsb)))) || ((x == hm) && (y == hm))){if(jt)jsignal(EVLIMIT); R 0;}

  // otherwise, do full 64-bit multiply.  Don't convert neg to pos, because the audit for positive fails on MAXNEG * 1
  I xh = x>>32, xl = x&lm, yh = y>>32, yl = y&lm;
  I xlyl = xl*yl;  // partial products
  // Because only one argument can have signed significance in high 32 bits, the
  // sum of lower partial products must fit in 96 bits.  Get bits 32..95 of that,
  // and fail if there is significance in 64..95
  // We need all 4 partial products, but we can calculate xh*yh and xh*yl at the same time,
  // by multiplying xh by the entire y.
  z = xh*y+yh*xl+((UI)xlyl>>32);
  if ((hm & (z + lmsb))){if(jt)jsignal(EVLIMIT); R 0;}
  // Combine bits 32..63 with 0..31 to produce the result
  z = (z<<32) + (xlyl&lm);
 }
 R z;
}
#endif

// take the product of n values starting at *v.  Set error if overflow, but NOT if any of the values is 0
I jtprod(J jt,I n,I*v){I z;
 DPMULDDECLS
 // We generally don't call here unless n>2, so we optimize for that case
 // We want to make sure that if any of the numbers being multiplied is 0, we return 0 with no error.
 // So we check each number as it is read, and exit early if 0 is encountered.  When we multiply, we suppress
 // the error assertion.  Then, at the end, if the product is 0, it must mean that there was an error, and
 // we report it then.  This way we don't need a separate pass to check for 0.
 z=1;
 DQ(n, if(unlikely(*v==0))R 0; DPMULDZ(z,*v,z) ++v;)
 ASSERT(z!=0,EVLIMIT)
 R z;
}
#else

I jtmult(J jt,I x,I y){D z=x*(D)y; ASSERT(((z<=IMAX)&&(z>=IMIN))||(z=0,!jt),EVLIMIT); R(I)z;}  // If jt==0, return quiet 0

I jtprod(J jt,I n,I*v){D z=1; DO(n, z*=(D)v[i];); ASSERT(z<=IMAX,EVLIMIT); R(I)z;}

#endif

// w is a boolean array, result is 1 iff all values are 0
B all0(A w){if(!w)R 0; I *c8=IAV(w); I n=AN(w); while((n-=SZI)>=0)if(*c8++!=0)R 0; R ((*c8<<BB)<<((~n)<<LGBB))==0;}

// w is a boolean array, result is 1 iff all values are 1
B all1(A w){if(!w)R 0; I *c8=IAV(w); I n=AN(w); while((n-=SZI)>=0)if(*c8++!=VALIDBOOLEAN)R 0; R (((*c8^VALIDBOOLEAN)<<BB)<<((~n)<<LGBB))==0;}

// Number of atoms in an item.
I jtaii(J jt,A w){I m; PROD(m,AR(w)-1,1+AS(w)); R m;}

// return A-block for b+m*i.n
A jtapv(J jt,I n,I b,I m){A z;
 // see if we can use the canned ascending integers
 if((m==1) & (b>=IOTAVECBEGIN) & (b+n<=IOTAVECLEN+IOTAVECBEGIN)) {
  if(unlikely((b|(n&~1))==0)){z=n?iv0:mtvi; R z;}  // i. 0/1, INT type
  GAT0(z,INT,0,1); AS(z)[0]=n; AN(z)=n; AK(z)=(C*)(iotavec+b-IOTAVECBEGIN)-(C*)z; ACINIT(z,ACUC1) AFLAGINIT(z,AFRO)  // mark block readonly and not inplaceable
  R z;
 }
 R apvwr(n,b,m);
}    /* b+m*i.n */
// same, but never return a readonly block
A jtapvwr(J jt,I n,I b,I m){A z;
 GATV0(z,INT,n,1); I *x=AV1(z);
  DO(n, x[i]=b; b+=m;)
 R z;
}    /* b+m*i.n writable */


// w must be 0 or an atom equal to 0 or 1.  Result is its value
B jtb0(J jt,A w){if(!(w))R 0; ASSERT(!AR(w),EVRANK); if(likely(ISDENSETYPE(AT(w),INT+B01))){I z; ASSERT(!((z=BIV0(w))&~1),EVDOMAIN); R z;} RZ(w=cvt(B01,w)); R BAV(w)[0];}
   // INT/B01 quickly

// NOTE: the caller modifies this result inplace, so it must not be shared or readonly
B*jtbfi(J jt,I n,A w,B p){A t;B* RESTRICT b;I* RESTRICT v;
 GATV0(t,B01,n+1,1); b=BAV1(t);  // allo n+1 slots
 mvc((n|(SZI-1))+1,b,1,iotavec-IOTAVECBEGIN+(p^1)); v=AV(w); DO(AN(w), b[v[i]]=p;);
 R b;
}    // boolean mask from integers: p=(i.>:n)e.w  where *./w<n

// default CTTZ to use if there is no compiler intrinsic
#if !defined(CTTZ)
// Return bit #of lowest bit set in w (considering only low 32 bits)
// if no bit set, result is undefined (1 here)
I CTTZ(I w){
    I t = 1;
    if (0 == (w & 0xffff)){ w >>= 16; t += 16; }
    if (0 == (w & 0xff)){ w >>= 8; t += 8; }
    if (0 == (w & 0xf)){ w >>= 4; t += 4; }
    if (0 == (w & 0x3)){ w >>= 2; t += 2; }
    R t - (w & 1);
}
// same, except returns 32 if no bit set
I CTTZZ(I w){ R w & 0xffffffff ? CTTZ(w) : 32; }
// Same, but works on full length of an I argument (32 or 64 bits)
#if SY_64
I CTTZI(I w){
    I t = 1;
    if (0 == (w & 0xffffffffLL)){ w >>= 32; t += 32; }
    if (0 == (w & 0xffff)){ w >>= 16; t += 16; }
    if (0 == (w & 0xff)){ w >>= 8; t += 8; }
    if (0 == (w & 0xf)){ w >>= 4; t += 4; }
    if (0 == (w & 0x3)){ w >>= 2; t += 2; }
    R t - (w & 1);
}
#else
// #define CTTZI CTTZ   // On 32-bit machines, I is same as long
// j.h declares CTTZI as extern function
I CTTZI(I w){R CTTZ(w);}
#endif
#endif

UI4 CTLZI_(UI w){
 UI4 t = 0;
#if BW==64
 if (w & 0xffffffff00000000LL){ w >>= 32; t += 32; }
#endif
 if (w & 0xffff0000LL){ w >>= 16; t += 16; }
 if (w & 0xff00LL){ w >>= 8; t += 8; }
 if (w & 0xf0LL){ w >>= 4; t += 4; }
 if (w & 0xcLL){ w >>= 2; t += 2; }
 R t + (UI4)(w >>= 1);
}

I bsum(I n,B*b){I q=(n-1)>>LGSZI,z=0;UI t,*v;
 if(n==0)R z;
#if C_AVX2 || EMU_AVX2
 // do 64 bits at a time, ending with <32 to finish
 UI b64; __m256i b0,b1,zero=_mm256_setzero_si256(),baltb=_mm256_set1_epi16(0x00ff),balt16=_mm256_set1_epi32(0x0000ffff),balt1664=_mm256_set1_epi64x(0x000000000000ffff),btotal=zero; I zbase;
 B *bx=b;
 b0=zero; b1=zero; 
 if(n>SZI*NPAR)while(1){
  // do a section of up to 126 adds in each accumulator
  I n127=n; n127=n<254*SZI*NPAR*2?n127:254*SZI*NPAR*2;  // limit is 254 loops in each of 2 accumulators

#define BSUM64(offset,acc) b##acc=_mm256_add_epi8(b##acc,_mm256_loadu_si256((__m256i *)(bx+(2*offset+acc)*(SZI*NPAR))));

  UI n2=DUFFLPCTV(n127-1,3,LGSZI+LGNPAR);  // # turns through duff loop - each one 64 bytes
  if(n2>0){
   I backoff=DUFFBACKOFFV(n127-1,3,LGSZI+LGNPAR);
   bx += (backoff+1)*(SZI*NPAR);
   n-=n2*8*(SZI*NPAR)+(backoff+1)*(SZI*NPAR);  // reduct count of # bytes left
   switch(backoff){
   do{
   case -1: BSUM64(0,0) case -2: BSUM64(0,1) case -3: BSUM64(1,0) case -4: BSUM64(1,1)
   case -5: BSUM64(2,0) case -6: BSUM64(2,1) case -7: BSUM64(3,0) case -8: BSUM64(3,1)
   bx +=8*(SZI*NPAR);
   }while(--n2!=0);
   }
  }
  // if this is the last loop, break out to handle the last batch
  if(n<=SZI*NPAR)break;
  // join accumulators and funnel them down a single total of 4 64-bit values
  b0=_mm256_add_epi16(_mm256_srli_epi16(b0,8),_mm256_and_si256(b0,baltb));  // 00TT00TT00TT00TT in b0
  b1=_mm256_add_epi16(_mm256_srli_epi16(b1,8),_mm256_and_si256(b1,baltb));   // and b1
  b1=_mm256_add_epi16(b0,b1);  // now b1 has the total, in 16 16-bit lanes
  b1=_mm256_add_epi32(_mm256_srli_epi32(b1,16),b1); b1=_mm256_add_epi64(_mm256_srli_epi64(b1,32),b1);  // now have xxxxxxxxxxxxTTTT in each 64-bit section
  btotal=_mm256_add_epi64(_mm256_and_si256(b1,balt1664),btotal);
  b0=zero; b1=zero; 
 }
 // handle last batch of 32.  Create mask, read the batch, add it into b1.  There are n bytes left to read, n=1 to 32
 __m256i endmask=_mm256_loadu_si256((__m256i*)((C*)validitymask+32-n));
 b1=_mm256_add_epi8(b1,_mm256_and_si256(_mm256_loadu_si256((__m256i*)((C*)validitymask+32-n)),_mm256_maskload_epi64((I*)bx, _mm256_loadu_si256((__m256i*)(((I)validitymask+32-n)&-SZI)))));
   // load the 8-byte section for any valid byte
 // join accumulators and funnel them down a single total of 4 64-bit values
 b0=_mm256_add_epi16(_mm256_srli_epi16(b0,8),_mm256_and_si256(b0,baltb));  // 00TT00TT00TT00TT in b0
 b1=_mm256_add_epi16(_mm256_srli_epi16(b1,8),_mm256_and_si256(b1,baltb));   // and b1
 b1=_mm256_add_epi16(b0,b1);  // now b1 has the total, in 16 16-bit lanes 00TT...
 b1=_mm256_add_epi32(_mm256_srli_epi32(b1,16),b1); b1=_mm256_add_epi64(_mm256_srli_epi64(b1,32),b1);  // now have xxxxxxxxxxxxTTTT in each 64-bit section
 btotal=_mm256_add_epi64(_mm256_and_si256(b1,balt1664),btotal);
 // add 64-bit sections to get final result
 btotal=_mm256_add_epi64(btotal,_mm256_castpd_si256(_mm256_permute_pd(_mm256_castsi256_pd(btotal),0xf)));
 z=_mm256_extract_epi64(btotal,0)+_mm256_extract_epi64(btotal,2);
 #else
 v=(UI*)b;
 // Do word-size sections, max 255 at a time, till all finished
 NOUNROLL while(1){
  t=0; DQ(MIN(q,254), t+=*v++;); q-=254;  // sig in ffffffffffffffff
  if(q<=0)break;   // exit after last section, always with room for 1 more
  ADDBYTESINI(t);  // convert byte-lane total to single total
  z+=t;   // add sig
 }
// finish up any remnant, 1-8 bytes
 t+=*v&((UI)~(I)0 >> (((-n)&(SZI-1))<<3)); ADDBYTESINI(t); z+=t;
#endif
 R z;
}    /* sum of boolean vector b */

// A block for null-terminated C string, with a trailing NUL (which is not included in the AN of the string)
A jtcstr(J jt,C*s){A z; RZ(z=mkwris(str((I)strlen(s),s))); CAV(z)[AN(z)]=0; R z;}  // ensure writable string returned, since we modify it here.  The string has only the non-NUL, but add a trailing NUL.  There's always room.

// Return 1 iff w is the evocation of a name.  w must be a FUNC
B evoke(A w){V*v=FAV(w); R CTILDE==v->id&&v->fgh[0]&&NAME&AT(v->fgh[0]);}

// return 1 if (,w) -: i. # ,w
// if there is an error, we just return 0
I jtisravelix(J jt,A w){
 I wt=AT(w); if(unlikely(!(wt&B01+INT))){RZ(w=cvt(INT,w)) wt=AT(w);}  // get type, force to numeric
 if(wt&INT){I *wv=IAV(w); DO(AN(w), if(wv[i]!=i)R 0;)}
 else{C *wv=BAV(w); DO(AN(w), if(wv[i]!=i)R 0;)}
 R 1;  // return 1 if all match
}

// Extract the integer value from w, return it.  Set error if non-integral or non-atomic (and return 0).  Values whose abs > IMAX are converted to IMAX/-IMAX
I jti0(J jt,A w){ARGCHK1(w);
 if(likely(ISDENSETYPE(AT(w),INT+B01))){ASSERT(!AR(w),EVRANK); R BIV0(w);}  // INT/B01 quickly
 if(likely(ISDENSETYPE(AT(w),FL))){I cval;  // FL also "quickly"
  D d=DAV(w)[0];  // fetch value
  if(ABS(d)<-(D)IMIN){
   D e=jround(d); cval=(I)e;  // FL without call to cvt
   // if an atom is tolerantly equal to integer,  there's a good chance it is exactly equal.
   // infinities will always round to themselves
   ASSERT(d==e || FFIEQ(d,e),EVDOMAIN);
  }else{
   cval=d>0?IMAX:-IMAX;  // if beyond integral, treat as infinity
  }
  R cval;  // too-large values don't convert, handle separately
 }
 if (AT(w)&XNUM) {
  X x= XAV(w)[0];
  if (ifits_slong_pX(x)) {
   I r= IgetX(x);
   if (unlikely(IMIN==r)) R -IMAX;
   R r;
  }
  if (XSGN(x)>0) R IMAX; else R -IMAX;
 }
 if(!(w=vi(w)))R 0; ASSERT(!AR(w),EVRANK);
 if (ISGMP(w)||AT(w)&RAT) SEGFAULT; // this should never happen
 R IAV(w)[0];
}  // can't move the ASSERT earlier without breaking a lot of tests

A jtifb(J jt,I n,B* RESTRICT b){A z;I p,* RESTRICT zv; 
 p=bsum(n,b); 
 if(p==n)R IX(n);
 GATV0(z,INT,p,1); zv=AV1(z);
#if C_AVX2 || EMU_AVX2
 if(unlikely(p==0))R z;
 // do 64 bits at a time
 UI b64; __m256i b0,b1,bor,zero=_mm256_setzero_si256(); I zbase;
 B *bx=b;

#define IFB64(offset) b0=_mm256_loadu_si256((__m256i *)(bx+offset*(SZI*NPAR*2))); b1=_mm256_loadu_si256((__m256i *)(bx+offset*(SZI*NPAR*2)+NPAR*SZI)); bor=_mm256_or_si256(b0,b1); if(!_mm256_testz_si256(bor,bor)){ \
  b64=((UI)(UI4)_mm256_movemask_epi8(_mm256_cmpgt_epi8(b1,zero))<<32)+(UI)(UI4)_mm256_movemask_epi8(_mm256_cmpgt_epi8(b0,zero)); zbase=bx-b+offset*(SZI*NPAR*2); do{*zv++=zbase+CTTZI(b64); b64&=b64-1;}while(b64);}

 UI n2=DUFFLPCTV(n-1,2,LGSZI+LGNPAR+1);  // # turns through duff loop - each one 64 bytes
 if(n2>0){
  I backoff=DUFFBACKOFFV(n-1,2,LGSZI+LGNPAR+1);
  bx += (backoff+1)*(SZI*NPAR*2);
  switch(backoff){
  do{
  case -1: IFB64(0) case -2: IFB64(1) case -3: IFB64(2) case -4: IFB64(3)
  bx +=4*(SZI*NPAR*2);
  }while(--n2!=0);
  }
 }
 // handle last batch of 64.  Create masks
 n=(n-1)&(2*SZI*NPAR-1);  // get n-1 of remnant
 I n1=(n>>LGSZI)+1; n1=n1>=NPAR?NPAR:n1; bor=_mm256_loadu_si256((__m256i*)(validitymask+((-n1)&(NPAR-1))));
 b64=(UI)(UI4)_mm256_movemask_epi8(_mm256_cmpgt_epi8(_mm256_maskload_epi64((I*)bx,bor),zero));
 // read the second batch.  If n<32, this must be NOPd by repeating the address
 zbase=bx-b; bx+=n&(SZI*NPAR);  // advance bx only if there is something to read

 n1=(n>>LGSZI)-3; n1=n1<0?0:n1; bor=_mm256_loadu_si256((__m256i*)(validitymask+(4-n1)));
 b64=b64+((UI)(UI4)_mm256_movemask_epi8(_mm256_cmpgt_epi8(_mm256_maskload_epi64((I*)bx,bor),zero))<<32);
 // discard invalid bits and write the rest out
 b64<<=(BW-1)-n; b64>>=(BW-1)-n; while(b64){*zv++=zbase+CTTZI(b64); b64&=b64-1;};
#else
 n+=(n&(SZI-1))?SZI:0; I zbase=0; UI *wvv=(UI*)b; UI bits=*wvv++;  // prime the pipeline for top of loop
 while(n>0){    // where we load bits SZI at a time
  // skip empty words, to get best speed on near-zero a.  This exits with the first unskipped word in bits
  while(bits==0 && n>=(2*SZI)){zbase+=SZI; bits=*wvv++; n-=SZI;}  // fast-forward over zeros.  Always leave 1 word so we have a batch to process
  I batchsize=n>>LGSZI; batchsize=MIN(BB,batchsize);
  UI bitstack=0; while(--batchsize>0){I bits2=*wvv++; PACKBITSINTO(bits,bitstack); bits=bits2;};  // keep read pipe ahead.  Each loop gets SZI bits, * BB=BW
  // Handle the last word of the batch.  It might have non-Boolean data at the end, AFTER the Boolean padding.  Just clear the non-boolean part in this line
  bits&=VALIDBOOLEAN; PACKBITSINTO(bits,bitstack);
  // Now handle the last batch, by discarding garbage bits at the end and then shifting the lead bit down to bit 0
  if(n>=BW+SZI)bits=*wvv++;else {n-=n&(SZI-1)?SZI:0; bitstack<<=(BW-n)&(SZI-1); bitstack>>=BW-n;}  // discard invalid trailing bits; shift leading byte to position 0.  For non-last batches, start on next batch
  NOUNROLL while(bitstack){I bitx=CTTZI(bitstack); *zv++=zbase+bitx; bitstack&=bitstack-1;}
  zbase+=BW;  // advance base to next batch of 64
  n-=BW;  // decr count left
 }
#endif
 R z;
}    /* integer vector from boolean mask */

// i. # w
static F1(jtii){ARGCHK1(w); I j; RETF(IX(SETIC(w,j)));}

// Return the higher-priority of the types s and t.  s and t are known to be not equal.
// If either is sparse, convert the result to sparse.
// Error if one argument is sparse and the other is non-sparsable
// s and t may not be 0
// Result is always an UNSAFE type
// this code is repeated in result.h
I jtmaxtype(J jt,I s,I t){
 I resultbit = PRIORITYTYPE(MAX(TYPEPRIORITY(s),TYPEPRIORITY(t)));  // Get the higher-priority type
 if(unlikely(ISSPARSE(s|t))){ASSERT(!((s|t)&(C2T|C4T|XNUM|RAT|SBT)),EVDOMAIN); R SPARSE|((I)1 << resultbit);}  // If either operand sparse, return sparse version
 R (I)1 << resultbit;   // otherwise, return normal version
}

// Copy m bytes from w to z, repeating every n bytes if n<m
// This overfetches from z and w, but does not overstore z
void mvc(I m,void*z,I n,void*w){
 if(unlikely(m==0))R;  // fast return if nothing to copy
#if C_AVX2 || EMU_AVX2
 PREFETCH(w);  /* start bringing in the start of data */ 
 // The main use of mvc is memset and fill.
 // The short version has some deficiencies: it calls memcpy repeatedly, which adds overhead, including alignment
 // overhead; it repeatedly reads from memory needlessly, which will erroneously pull the filled area into caches.
 // If the argument can evenly fill up 32 bytes, we can load it into a register here and write repeatedly.
 // This is JMC without the reading.
 if((((n&-n&(2*NPAR*SZI-1))^n)+((m&(n-1))&(REPSGN(-(((I)z|m)&(SZI-1))))))==0){  // n is power of 2, and not > 32, and (result area is a multiple of cells OR is I-aligned in address and length)
  __m256i endmask, wd; UI wdi;  // replicated data
  // replicate the input as needed to word size, then on up to 32-byte size
  if(n==4*SZI){wd=_mm256_loadu_si256((__m256i*)w);  // 32 bytes, load em.  First place because of MEMSET and all numeric fill
  }else if(n<=SZI){  // <=8 bytes to begin with: LIT C2T C4T
   wdi=*(UI*)w&((UI)-1LL>>((SZI-n)<<LGBB));  // fetch fill and mask to valid bytes
   wdi*=(UI)((((0x0101010101010101<<1)+(0x0001000100010001<<2)+(0x0000000100000001<<4)+(0x1<<8))>>n)&VALIDBOOLEAN);  // replicate based on size
   wd=_mm256_set1_epi64x(wdi);  // further replicate to 32-byte size
  }else{wd=_mm256_castpd_si256(_mm256_broadcast_pd((__m128d*)w));  // 16 bytes to begin with, load em - only CMPX/RAT
  }
  // if the operation is long, move 2 blocks to align the store pointer
  I misalign=(I)z&(NPAR*SZI-1);
  if((UI)m>(((UI)misalign-1)|4*NPAR*SZI)){  // if store address is misaligned and length is big enough   TUNE
   _mm256_storeu_si256((__m256i*)z,wd); z=(C*)z+NPAR*SZI; _mm256_storeu_si256((__m256i*)z,wd);   // do the 2 stores
   z=(C*)z+NPAR*SZI-misalign; m-=2*NPAR*SZI-misalign;
   // if the misalignment was not a multiple of the size of the atoms, refetch the store value
   if(unlikely(misalign&(n-1)))wd=_mm256_loadu_si256((__m256i*)((I)z-NPAR*SZI));
  }
  endmask=_mm256_loadu_si256((__m256i*)(validitymask+((-(m>>LGSZI))&(NPAR-1)))); // here byte-remnant is OPTIONAL so 00xxx->0->1111, 01xxx->3->1000, 10xxx->2->1100
  /* copy the remnants at the end - bytes and Is.  Do this early because they have address conflicts that will take 20 */
  /* cycles to sort out, and we can put that time into the switch and loop branch overhead */
  /* First, the odd bytes if any */
  /* if there is 1 byte to do low bits of ll are 0, which means protect 7 bytes, thus 0->7, 1->6, 7->0 */
  if(unlikely(m--&(SZI-1))){  // convert m to len-1, but first see if there is a remnant
   // the last word is partially filled.  This could be the end of a long value, if the initial address was
   // misaligned; but it will not wrap over a cell boundary of an I or larger (since the input was an even multiple of cells of w)
   // If target is a word multiple on a word boundary, it might not be an even multiple of cells but it can never come here either, since
   // it will never have a remnant less than a full I
   if(likely(n<=SZI)){
    // the cell is SZI or less.  It must end at the end of wdi, but may have multiple repeats.
    STOREBYTES((C*)z+(m&(-SZI)),wdi>>((~m&(SZI-1))<<BB),~m&(SZI-1));  // copy remnant, 1-8 bytes.  Discard discardable bytes from front of wdi
   }else{
    // cell bigger than SZI.  Align data to end of input w
    STOREBYTES((C*)z+(m&(-SZI)),*(I*)((C*)w+n-SZI+(~m&(SZI-1))),~m&(SZI-1));  // copy remnant, 1-8 bytes.  Discard discardable bytes from front of last SZI bytes of wd
   }
   if(unlikely((m&=-SZI)==0))R; --m;  // account for bytes moved; return if we have moved all; keep m as count-1
  }
  // store 128-byte sections, first one being 0, 4, 8, or 12 Is. There could be 0 to do
  UI n2=DUFFLPCT(m>>LGSZI,2);  /* # turns through duff loop */
  if(n2>0){
   UI backoff=DUFFBACKOFF(m>>LGSZI,2);
   z=(C*)z+(backoff+1)*NPAR*SZI;
   if(likely(m<L3CACHESIZE)){
    // Normal case: not a huge copy
    switch(backoff){
    do{ ;
    case -1: _mm256_storeu_si256((__m256i*)z,wd);
    case -2: _mm256_storeu_si256((__m256i*)((C*)z+1*NPAR*SZI),wd);
    case -3: _mm256_storeu_si256((__m256i*)((C*)z+2*NPAR*SZI),wd);
    case -4: _mm256_storeu_si256((__m256i*)((C*)z+3*NPAR*SZI),wd);
    z=(C*)z+4*NPAR*SZI;
    }while(--n2>0);
    }
   }else{
   // The copy length is bigger than L3 cache: use non-temporal stores to avoid excessive cache traffic
    switch(backoff){
    do{ ;
    case -1: _mm256_stream_si256((__m256i*)z,wd);
    case -2: _mm256_stream_si256((__m256i*)((C*)z+1*NPAR*SZI),wd);
    case -3: _mm256_stream_si256((__m256i*)((C*)z+2*NPAR*SZI),wd);
    case -4: _mm256_stream_si256((__m256i*)((C*)z+3*NPAR*SZI),wd);
    z=(C*)z+4*NPAR*SZI;
    }while(--n2>0);
    }
   }
  }
  // copy last section, 1-4 Is. ll bits 00->4 bytes, 01->3 bytes, etc
  // do it here so that the previous loop doesn't have back-to-back branches in the small loop, trying to save one lousy update of z
  _mm256_maskstore_epi64((I*)z,endmask,wd);
  R;
 }
 // if argument doesn't fit into 32 bytes, fall through to slow way
#endif
 C *zz=z;  // running output pointer
 if(n<SZI){
  // if w has < 8 bytes, replicate it to Is, write them out up to an even multiple of Is.  n must be >= 2 here if we went through the non-copy code above
  //
  I nbitsinw=n<<LGBB; UI wdmsk=(*(UI*)w<<(BW-nbitsinw))>>(BW-nbitsinw); // valid bits, and w data masked to just valid bytes
  I shiftct=nbitsinw; I fullrepbits=nbitsinw; UI wdi=wdmsk;  // running shift count, number of bits in largest full rep of w, full word of data with repeats
  wdi|=wdi<<shiftct; shiftct<<=1; fullrepbits=shiftct<BW?shiftct:fullrepbits; shiftct=shiftct<BW?shiftct:0;  // shift in another rep, remember if it fits, clear shift if not
#if SY_64
  wdi|=wdi<<shiftct; fullrepbits=n==3?6*BB:fullrepbits; // length 2/4 can set length to 2/4 bytes indiscriminately, as long as the data is filled in for the whole word
#endif
#if !(C_AVX2 || EMU_AVX2)
  if(n==1){fullrepbits=BW; wdi|=wdi<<(BW/2);}  // if n=1 possible, check for it
#endif

  // Store full words, for an aliquot # reps (to leave it at an even boundary) unless z ends first
  I nwds=MIN(n<<4,(m-1)>>LGSZI); // # full words to move
  I srct=BW-fullrepbits, slct=fullrepbits;
  DQ(nwds, *(UI*)zz=wdi; zz+=SZI; wdi=(wdi>>srct)|((wdi>>srct)<<slct);)  // store and cycle the bytes
  m-=nwds<<LGSZI;  // account for bytes moved
  if(unlikely(m<=SZI)){
   // nothing left but a remnant <= 1 word, finish it here
   if(m)STOREBYTES(zz,wdi,SZI-m);  // # bytes is m, # to leave is SZI-m
   R;
  }
  // there is more to do.  Housekeep the variables to indicate that we have a larger area in w
  w=z;  // after this copy we will be copying from the beginning of the result area
  n=nwds<<LGSZI;  // we moved w above; now set source size to len of repeated w data
 }
 I nn=n;  // length of current source buffer, as it expands.  MUST BE >= SZI now
 I movlen=0;   // length moved from w in the final copy in the loop
 // copy as long as there is extra space in the output (or destination is an even word length), allowing overstore
 if(m>SZI)while(1){
  // if m and z are both I-aligned, we can move fullwords without actually getting an overstore
  // Otherwise, we must limit the move to less than m to ensure overstore does not actually exceed the buffer boundary
  I storelimit=m-(REPSGN(-((m|(I)zz)&(SZI-1)))&(SZI-1));  // m, backed up by SZI-1 if necessary
  movlen=MIN(storelimit,nn);  // # bytes we can safely move
  JMC(zz,w,movlen,0)   // move allowing overstore
  zz+=movlen;  // advance output pointer
  m-=movlen;   // decrement length
  if(m<=SZI)break;  // if we can't copy more, exit while we still point to the last buffer copied
  I addlen=movlen; addlen=w==z?addlen:0; nn+=addlen;   // increase size of doubled buffer, except first time when we are not appending to previous move
  w=z;  // from now on we move from the beginning of the output area and append to it
 }

 // copy final remnant if any, 0-SZI bytes
 if(unlikely(m>0)){
  // there is a remnant to insert without overstore.  It starts at offset movlen into w and runs for m bytes.  It may wrap the end of the buffer, or even start outside it
  movlen=(movlen==nn)?0:movlen;  // if we fully copied w, wrap to 0 so we don't overfetch after w
  UI wdi=*(UI*)((C*)w+movlen);  // pick up the next bytes to move, possibly with overfetch
  if(unlikely(movlen+m>nn)){  // if the next move wraps the end of the buffer...
   UI wrapi=*(UI*)w; I wrappos=(nn-movlen)<<LGBB; wdi=((wdi<<(BW-wrappos))>>(BW-wrappos))|(wrapi<<wrappos);  // join up the full value to store
  }
  STOREBYTES(zz,wdi,SZI-m);    // # bytes is m, # to leave is SZI-m
 }
}

// odometer, up to the n numbers s[]
A jtodom(J jt,I r,I n,I* RESTRICT s){A z;I m,mn,*u,*zv;
 PRODX(m,n,s,1) DPMULDE(m,n,mn);
 GATV(z,INT,mn,2==r?2:n,s);
 if(2==r){u=AS(z); u[0]=m; u[1]=n;}
 if(!mn)R z;
 zv=IAV(z);
 if(1==n)DO(m, zv[i]=i;)
 else{
  I *zvo=zv-1; DQ(n, *zv++=0;)   // init first row, point zvo to before the first value
  DQ(m-1, I dontadd=0; zv=zvo+n; zvo=zv+n; DQ(n, I out=*zv + 1 + dontadd; dontadd=REPSGN(out-s[i]); *zvo = out&dontadd; --zv; --zvo;))  // add 1 first time, & continue as long as there is carry.  Once dontadd goes to -1 it stays there
 }
 R z;
}

F1(jtrankle){R!w||AR(w)?w:ravel(w);}

A jtsc(J jt,I k)     {A z; if(BETWEENC(k,NUMMIN,NUMMAX)){z=num(k); z=k&~1?z:zeroionei(k); R z;} GAT0(z,INT, 1,0); IAV0(z)[0]=k;     RETF(z);}  // always return I
A jtscib(J jt,I k)   {A z; if(BETWEENC(k,NUMMIN,NUMMAX))R num(k); GAT0(z,INT, 1,0); IAV0(z)[0]=k;     RETF(z);}  // return b if 0 or 1, else I
A jtsc4(J jt,I t,I v){A z; GA00(z,t,1,0); IAV0(z)[0]=v;     RETF(z);}  // return scalar with a given I-length type (numeric or box)
A jtscb(J jt,B b)    {R num(b);}   // A block for boolean
A jtscc(J jt,C c)    {A z; GAT0(z,LIT, 1,0); CAV0(z)[0]=c;     RETF(z);}  // create scalar character
A jtscf(J jt,D x)    {A z; GAT0(z,FL,  1,0); DAV0(z)[0]=x;     RETF(z);}   // scalar float
A jtscx(J jt,X x)    {A z; GAT0(z,XNUM,1,0); XAV0(z)[0]=ca(x); RETF(z);}  // scalar extended

// return A-block for the string *s with length n
A jtstr(J jt,I n,C*s){A z; GATV0(z,LIT,n,1); MC(AV1(z),s,n); RETF(z);}

// return A-block for the string *s with length n, enclosed in quotes and quotes doubled
A jtstrq(J jt,I n,C*s){A z; I qc=2; DO(n, qc+=s[i]=='\'';) GATV0(z,LIT,n+qc,1); C *zv=CAV1(z); *zv++='\''; DO(n, C c=s[i]; if(c=='\'')*zv++=c; *zv++=c;) *zv='\''; RETF(z);}

// w is a LIT string; result is a new block with the same string, with terminating NUL added
F1(jtstr0){A z;C*x;I n; ARGCHK1(w); ASSERT(LIT&AT(w),EVDOMAIN); n=AN(w); GATV0(z,LIT,n+1,1); x=CAV1(z); MC(x,AV(w),n); x[n]=0; RETF(z);}

// return A-block for a 2-atom integer vector containing a,b
A jtv2(J jt,I a,I b){A z;I*x; GAT0(z,INT,2,1); x=AV1(z); *x++=a; *x=b; RETF(z);}

// return A-block for singleton integer list whose value is k
A jtvci(J jt,I k){A z; GAT0(z,INT,1,1); IAV1(z)[0]=k; RETF(z);}

// return A-block for list of type t, length n, and values *v
// MUST NOT return virtual or fixed block, because we often modify the returned area
A jtvec(J jt,I t,I n,void*v){A z; GA10(z,t,n); MC(AV(z),v,n<<bplg(t)); RETF(z);}

// return A-block for list of type t, length n, and values *v
// with special handling to coerce boolean type.  We do not overfetch.
#if defined(__GNUC__) && !defined(__clang__)
#pragma push_options
#pragma optimize ("unroll-loops")
#endif
#if C_AVX2 || EMU_AVX2
A jtvecb01(J jt,I t,I n,void*v){A z;
 GA10(z,t,n);   // allocate buffer
 if(t&B01){C*p=(C*)AV(z),*q=v;
  // for booleans, enforce valid boolean result: convert any nonzero to 0x01
  __m256i zeros=_mm256_setzero_si256();
  __m256i ones=_mm256_castpd_si256(_mm256_broadcast_sd((D*)&Ivalidboolean));
  UI n0=n;  // number of bytes to do
  // move full 32-byte sections
  NOUNROLL while (n0 >= 32) {
  _mm256_storeu_si256((__m256i *)p,_mm256_add_epi8(_mm256_cmpeq_epi8(_mm256_loadu_si256((__m256i*)q),zeros),ones));  // 0->ff->0  non0->0->1
   n0-=32;p+=32;q+=32;
  }
  // move full 8-byte sections
  NOUNROLL while(n0>=SZI){
   I b8=*(I*)q;  // fetch bytes
   b8=(b8|((((UI)(b8&~VALIDBOOLEAN)>>1)+((UI)~(VALIDBOOLEAN<<(BB-1))))>>(BB-1)))&VALIDBOOLEAN;  // set bit 7 if bits 1-7 not 0; shift to bit 0; OR; mask to boolean
   *(I*)p=b8;  // store the result
   n0-=SZI;p+=SZI;q+=SZI;
  }
  // finish by moving individual bytes
  NOUNROLL while(n0--)*p++=!!(*q++);
 }else{MC(AV(z),v,n<<bplg(t));}  // non-boolean, just copy
 RETF(z);
}
#elif __SSE2__
A jtvecb01(J jt,I t,I n,void*v){A z; GA10(z,t,n);if(t&B01){C*p=(C*)AV(z),*q=v; 
__m128i zeros=_mm_setzero_si128();
__m128i ones=_mm_set1_epi8(1);
__m128i ffs=_mm_set1_epi8(0xffu);
UI n0=n<<bplg(t);
UI mis=((uintptr_t)q)&15u;
mis=(mis>n0)?n0:mis;
if(mis){
n0-=mis;
#if defined(__clang__)
#pragma clang loop vectorize(enable) interleave_count(4)
#endif
while(mis--)*p++=!!(*q++);
}
while (n0 >= 16) {
 _mm_storeu_si128((__m128i *)p,_mm_and_si128(_mm_xor_si128(_mm_cmpeq_epi8(_mm_loadu_si128((__m128i*)q),zeros),ffs),ones));
 n0-=16;p+=16;q+=16;
}
#if defined(__clang__)
#pragma clang loop vectorize(enable) interleave_count(4)
#endif
while(n0--)*p++=!!(*q++);
}else MC(AV(z),v,n<<bplg(t)); RETF(z);}
#else
A jtvecb01(J jt,I t,I n,void*v){A z; I i; GA10(z,t,n);if(t&B01){C*p=(C*)AV(z),*q=v; 
#if defined(__clang__)
#pragma clang loop vectorize(enable) interleave_count(4)
#endif
for(i=0;i<n<<bplg(t);i++)*p++=!!(*q++);
}else MC(AV(z),v,n<<bplg(t)); RETF(z);}
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma pop_options
#endif

// Convert w to integer if it isn't integer already (the usual conversion errors apply)
F1(jtvi){ARGCHK1(w);
 if(ISDENSETYPE(AT(w),INT))R RETARG(w);  // handle common non-failing cases quickly: INT and boolean
 if((AT(w)&SPARSE+B01)>AR(w))R zeroionei(BAV(w)[0]);   // non-sparse Boolean atom, take atomic integer
 R cvt(INT,w);
}

// Audit w to ensure valid integer value(s).  Error (i. e. 0 return) if non-integral.  Result is A block for integer array.  Infinities converted to IMAX/-IMAX.  Non-infinities greater than integer precision give error
F1(jtvib){A z;I i,n,*zv;
 ARGCHK1(w);
 if(ISDENSETYPE(AT(w),INT))R RETARG(w);  // handle common non-failing cases quickly: INT and boolean
 if(ISDENSETYPE(AT(w),B01)){if(!AR(w))R zeroionei(BAV(w)[0]); R cvt(INT,w);}
 if(w==ainf)R imax;  // sentence words of _ always use the same block, so catch that too
 I p=-IMAX,q=IMAX;
 RANK2T oqr=jt->ranks; RESETRANK;
 if(unlikely(ISSPARSE(AT(w))!=0))RZ(w=denseit(w));
 switch(AT(w)){
 default:
  if(!ISDENSETYPE(AT(w),FL))RZ(w=ccvt(FL,w,0));
  n=AN(w);
  I zr=AR(w); GATV(z,INT,n,AR(w),AS(w)); zv=AVn(zr,z);
  for(i=0;i<n;++i){I cval;
   D d=DAV(w)[i];  // fetch value
   if(ABS(d)<-(D)IMIN){
    D e=jround(d); cval=(I)e;  // FL without call to cvt
    // if an atom is tolerantly equal to integer,  there's a good chance it is exactly equal.
    // infinities will always round to themselves
    ASSERT(d==e || FFIEQ(d,e),EVDOMAIN);
   }else{
    ASSERT(ABS(d)==inf,EVDOMAIN);  // if beyond int, must be infinite
    cval=d>0?IMAX:-IMAX;  // if beyond integral, treat as infinity
   }
   zv[i]=cval;
   }
   break;
  case XNUM:
  case RAT:  z=cvt(INT,maximum(sc(p),minimum(sc(q),w))); break;
 }
 jt->ranks=oqr; RETF(z);
}

// Convert w to integer if needed, and verify every atom is nonnegative
F1(jtvip){I*v; ARGCHK1(w); if(!ISDENSETYPE(AT(w),INT))RZ(w=cvt(INT,w)); v=AV(w); DQ(AN(w), ASSERT(0<=*v++,EVDOMAIN);); RETF(w);}

// Convert w to string, verify it is a list or atom
F1(jtvs){ARGCHK1(w); ASSERT(1>=AR(w),EVRANK); if(!ISDENSETYPE(AT(w),LIT))w=cvt(LIT,w); R w;}    
     /* verify string */

// Convert w to utf8 string, verify it is a list or atom
F1(jtvslit){ARGCHK1(w); ASSERT(1>=AR(w),EVRANK); if(!ISDENSETYPE(AT(w),LIT)){w=AT(w)&(C2T+C4T)?toutf8(w):cvt(LIT,w);} R w;}    
     /* verify string */

// w is a boxed noun whose contents may include virtual blocks.  Realize any such virtual blocks.
// The blocks are realized in place in w, which is OK because only nonrecursive nouns can contains virtual blocks.
A jtrealizeboxedvirtuals(J jt,A w){
A *a=AAV(w);   // address of boxed value
DQ(AN(w), if(*a)realizeifvirtual(*a); ++a;);
R (A)1;
}

// set up to call name in script.  Result is nameref to the name, which must have the part of speech pos.  Return 0 if not found
A jtfindnameinscript(J jt,C *script, C *name, I pos){
 A target; UI step;
 for(step=0;step<2;++step){
  switch(step){  // try the startup, from the bottom up
  case 1: ; C buf[100]; eval(strcat(strcat(strcpy(buf,"load'"),script),"'"));  // load script and fall through
  case 0: ; A w=nfs(strlen(name),name); A val; if((val=syrd(w,jt->locsyms))!=0){target=QCWORD(namerefacv(w,val)); if(target&&LOWESTBIT(AT(target))&pos)R target;}  // there is always a ref, but it may be to [:.  Undo ra() in syrd
  }
  RESETERR;  // if we loop back, clear errors
 }
 R 0;  // not found or wrong part of speech - error
}
