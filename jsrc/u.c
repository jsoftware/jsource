/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
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
B all0(A w){if(!w)R 0; R !memchr(AV(w),C1,AN(w));}

// w is a boolean array, result is 1 iff all values are 1
B all1(A w){if(!w)R 0; R !memchr(AV(w),C0,AN(w));}

// Number of atoms in an item.
I jtaii(J jt,A w){I m; PROD1(m,AR(w)-1,1+AS(w)); R m;}

// return A-block for b+m*i.n
A jtapv(J jt,I n,I b,I m){A z;
 // see if we can use the canned ascending integers
 if(m==1 && b>=IOTAVECBEGIN && b+n<=IOTAVECLEN+IOTAVECBEGIN) {
  GAT0(z,INT,0,1); AS(z)[0]=n; AN(z)=n; AK(z)=(C*)(iotavec+b-IOTAVECBEGIN)-(C*)z; ACINIT(z,ACUC1) AFLAGINIT(z,AFRO)  // mark block readonly and not inplaceable
  R z;
 }
 R apvwr(n,b,m);
}    /* b+m*i.n */
// same, but never return a readonly block
A jtapvwr(J jt,I n,I b,I m){A z;
 GATV0(z,INT,n,1); I *x=AV(z);
  DO(n, x[i]=b; b+=m;)
 R z;
}    /* b+m*i.n writable */


// w must be 0 or an atom equal to 0 or 1.  Result is its value
B jtb0(J jt,A w){if(!(w))R 0; ASSERT(!AR(w),EVRANK); if(!(B01&AT(w)))RZ(w=cvt(B01,w)); R BAV(w)[0];}

// NOTE: the caller modifies this result inplace, so it must not be shared or readonly
B*jtbfi(J jt,I n,A w,B p){A t;B* RESTRICT b;I* RESTRICT v;
 GATV0(t,B01,n+1,1); b=BAV(t);  // allo n+1 slots
 memset(b,!p,(n|(SZI-1))+1); v=AV(w); DO(AN(w), b[v[i]]=p;);
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

I CTLZI_(UI w, UI4*out){
 UI4 t = 0;
#if BW==64
 if (w & 0xffffffff00000000LL){ w >>= 32; t += 32; }
#endif
 if (w & 0xffff0000LL){ w >>= 16; t += 16; }
 if (w & 0xff00LL){ w >>= 8; t += 8; }
 if (w & 0xf0LL){ w >>= 4; t += 4; }
 if (w & 0xcLL){ w >>= 2; t += 2; }
 *out = t + (UI4)(w >>= 1);
 R 0;
}

I bsum(I n,B*b){I q=(n-1)>>LGSZI,z=0;UI t,*v;
 if(n==0)R z;
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
 R z;
}    /* sum of boolean vector b */

C cf(A w){if(!w)R 0; R CAV(w)[0];}  // first character in a character array

C cl(A w){if(!w)R 0; R CAV(w)[AN(w)-1];}  // last character in a character array

// A block for null-terminated C string, with a trailing NUL (which is not included in the AN of the string)
A jtcstr(J jt,C*s){A z; RZ(z=mkwris(str((I)strlen(s),s))); CAV(z)[AN(z)]=0; R z;}  // ensure writable string returned, since we modify it here.  The string has only the non-NUL, but add a trailing NUL.  There's always room.

// Return 1 iff w is the evocation of a name.  w must be a FUNC
B evoke(A w){V*v=FAV(w); R CTILDE==v->id&&v->fgh[0]&&NAME&AT(v->fgh[0]);}

// Extract the integer value from w, return it.  Set error if non-integral or non-atomic
I jti0(J jt,A w){ARGCHK1(w);
 if(likely(AT(w)&INT+B01)){ASSERT(!AR(w),EVRANK); R BIV0(w);}  // INT/B01 quickly
 if(likely(AT(w)&FL)){D d=DAV(w)[0]; D e=jround(d); I cval=(I)e;  // FL without call to cvt
  // if an atom is tolerantly equal to integer,  there's a good chance it is exactly equal.
  // infinities will always round to themselves
  ASSERT(d==e || FFIEQ(d,e),EVDOMAIN);
  cval=d<(D)-IMAX?-IMAX:cval; cval=d>=FLIMAX?IMAX:cval;
  ASSERT(!AR(w),EVRANK);
  R cval;  // too-large values don't convert, handle separately
 }
 if(!(w=vi(w)))R 0; ASSERT(!AR(w),EVRANK);
 R IAV(w)[0];
}  // can't move the ASSERT earlier without breaking a lot of tests

A jtifb(J jt,I n,B* RESTRICT b){A z;I p,* RESTRICT zv; 
 p=bsum(n,b); 
 if(p==n)R IX(n);
 GATV0(z,INT,p,1); zv=AV(z);
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
 // If values differ and are both nonzero...
 I resultbit = PRIORITYTYPE(MAX(TYPEPRIORITY(s),TYPEPRIORITY(t)));  // Get the higher-priority type
 if(unlikely(((s|t)&SPARSE))!=0){ASSERT(!((s|t)&(C2T|C4T|XNUM|RAT|SBT)),EVDOMAIN); R (I)1 << (resultbit+SB01X-B01X);}  // If either operand sparse, return sparse version
 R (I)1 << resultbit;   // otherwise, return normal version
}

// Copy m bytes from w to z, repeating every n bytes if n<m
// This overfetches from z and w, but does not overstore z
void mvc(I m,void*z,I n,void*w){
 if(unlikely(m==0))R;  // fast return if nothing to copy
#if (C_AVX2&&SY_64) || EMU_AVX2
 // The main use of mvc is memset and fill.
 // The short version has some deficiencies: it calls memcpy repeatedly, which adds overhead, including alignment
 // overhead; it repeatedly reads from memory needlessly, which will erroneously pull the filled area into caches.
 // If the argument can evenly fill up 32 bytes, we can load it into a register here and write repeatedly.
 // This is JMC without the reading.
 if(likely((((n&-n&(2*NPAR*SZI-1))^n)+((m&(n-1))&(REPSGN(-(((I)z|m)&(SZI-1))))))==0)){  // n is power of 2, and not > 32, and (result area is a multiple of cells OR is I-aligned in address and length)
  PREFETCH(w);  /* start bringing in the start of data */ 
  __m256i endmask, wd; UI wdi;  // replicated data
  // replicate the input as needed to word size, then on up to 32-byte size
  if(likely(n<=SZI)){
   wdi=*(I*)w; wdi<<=(SZI-n)<<LGBB; wdi>>=(SZI-n)<<LGBB;  // zero out bits above the size
   I shiftamt=n<<LGBB; wdi|=(wdi<<(shiftamt&(7*BB))); shiftamt<<=1; wdi|=(wdi<<(shiftamt&(7*BB))); shiftamt<<=1; wdi|=(wdi<<(shiftamt&(7*BB)));  // replicate to word size
   wd=_mm256_broadcastq_epi64(_mm_insert_epi64(_mm_setzero_si128(),wdi,0));  // further replicate to 32-byte size
  }else if(n==2*SZI){  // 16 bytes to begin with
   wd=_mm256_broadcastsi128_si256(_mm_loadu_si128((__m128i*)w));  // load em
  }else{  // 32 bytes to begin with
   wd=_mm256_loadu_si256((__m256i*)w);  // load em
  }
  // if the operation is long, move 2 blocks to align the store pointer
  I misalign=(I)z&(NPAR*SZI-1);
  if((UI)m>(((UI)misalign-1)|2*NPAR*SZI)){  // if store address is misaligned and length is > 2 stores...
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
   // it will never have a remnant lees than a full I
   if(likely(n<=SZI)){
    // the cell is SZI or less.  It must end at the end of wdi, but may have multiple repeats.
    STOREBYTES((C*)z+(m&(-SZI)),wdi>>((~m&(SZI-1))<<BB),~m&(SZI-1));  // copy remnant, 1-8 bytes.  Discard discardable bytes from front of wdi
   }else{
    // cell bigger than SZI.  Align data to end of input w
    STOREBYTES((C*)z+(m&(-SZI)),*(I*)((C*)w+n-SZI+(~m&(SZI-1))),~m&(SZI-1));  // copy remnant, 1-8 bytes.  Discard discardable bytes from front of last SZI bytes of wd
   }
   if(unlikely((m&=-SZI)==0))R; --m;  // account for bytes moved; return if we have moved all; keep m as count-1
  }
  /* copy last section, 1-4 Is. ll bits 00->4 bytes, 01->3 bytes, etc  */
  m&=(-NPAR*SZI);  /* m=start of last section, 1-4 Is */
  _mm256_maskstore_epi64((I*)((C*)z+m),endmask,wd);
  /* store 128-byte sections, first one being 0, 4, 8, or 12 Is. There could be 0 to do */
  /* the 2s here are lg2(#duff cases).  With 8 cases we got 8% faster for in-place copy; not worth the extra prefetches normally? */
  UI n128=((m+((((I)1<<2)-1)<<(LGNPAR+LGSZI)))>>(LGNPAR+LGSZI+2));  /* # 128-byte blocks with data, could be 0 */
  if(n128>0){ /* this test is worthwhile for short args */
   switch((m>>(LGNPAR+LGSZI))&(((I)1<<2)-1)){
   lbl: ;
   case 0: _mm256_storeu_si256((__m256i*)z,wd); z=(C*)z+NPAR*SZI;
   case 3: _mm256_storeu_si256((__m256i*)z,wd); z=(C*)z+NPAR*SZI;
   case 2: _mm256_storeu_si256((__m256i*)z,wd); z=(C*)z+NPAR*SZI;
   case 1: _mm256_storeu_si256((__m256i*)z,wd); z=(C*)z+NPAR*SZI;
   if(--n128>0)goto lbl;
   }
  }
  R;
 }
 // if argument doesn't fit into 32 bytes, fall through to slow way
#endif
#if 0  // obsolete 
 I p=n,r;
 // first copy n bytes; thereafter p is the number of bytes we have copied; copy that amount again
 MC(z,w,MIN(p,m)); NOUNROLL while(m>p){r=m-p; MC(p+(C*)z,z,MIN(p,r)); p+=p;}
#else
 C *zz=z;  // running output pointer
 if(n<SZI){
  // if w has < 8 bytes, replicate it to Is, write them out up to an even multiple of Is.  n must be >= 2 here if we went through the bob-copy code above
  //
  I nbitsinw=n<<LGBB; UI wdmsk=(*(UI*)w<<(BW-nbitsinw))>>(BW-nbitsinw); // valid bits, and w data masked to just valid bytes
  I shiftct=nbitsinw; I fullrepbits=nbitsinw; UI wdi=wdmsk;  // running shift count, number of bits in largest full rep of w, full word of data with repeats
  wdi|=wdi<<shiftct; shiftct<<=1; fullrepbits=shiftct<BW?shiftct:fullrepbits; shiftct=shiftct<BW?shiftct:0;  // shift in another rep, remember if it fits, clear shift if not
#if SY_64
  wdi|=wdi<<shiftct; fullrepbits=n==3?6*BB:fullrepbits; // length 2/4 can set length to 2/4 bytes indiscriminately, as long as the data is filled in for the whole word
#endif
#if !((C_AVX2&&SY_64) || EMU_AVX2)
  if(n==1){fullrepbits=BW; if(SY_64)wdi|=wdi<<32;}  // if n=1 possible, check for it
#endif

  // Store even words, for an even # reps (to leave it at an even boundary) unless z ends first
  I nwds=MIN(n<<4,(m-1)>>LGSZI); // # full words to move
  I srct=BW-fullrepbits, slct=fullrepbits;
  DQ(nwds, *(UI*)zz=wdi; zz+=SZI; wdi=(wdi>>srct)|((wdi>>srct)<<slct);)  // store and cycle the bytes
  m-=nwds<<LGSZI;  // account for bytes moved
  if(unlikely(m<=SZI)){
   // nothing left but a remnant <= 1 word, finish it here
   STOREBYTES(zz,wdi,SZI-m);  // # bytes is m, # to leave is SZI-m
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
  JMC(zz,w,movlen,lbl2,0)   // move allowing overstore
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
#endif
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

A jtsc(J jt,I k)     {A z; if((k^REPSGN(k))<=NUMMAX){z=num(k); z=k&~1?z:zeroionei(k); R z;} GAT0(z,INT, 1,0); IAV(z)[0]=k;     RETF(z);}  // always return I
A jtscib(J jt,I k)   {A z; if((k^REPSGN(k))<=NUMMAX)R num(k); GAT0(z,INT, 1,0); IAV(z)[0]=k;     RETF(z);}  // return b if 0 or 1, else I
A jtsc4(J jt,I t,I v){A z; GA(z,t,   1,0,0); IAV(z)[0]=v;     RETF(z);}  // return scalar with a given I-length type (numeric or box)
A jtscb(J jt,B b)    {R num(b);}   // A block for boolean
A jtscc(J jt,C c)    {A z; GAT0(z,LIT, 1,0); CAV(z)[0]=c;     RETF(z);}  // create scalar character
A jtscf(J jt,D x)    {A z; GAT0(z,FL,  1,0); DAV(z)[0]=x;     RETF(z);}   // scalar float
A jtscx(J jt,X x)    {A z; GAT0(z,XNUM,1,0); XAV(z)[0]=ca(x); RETF(z);}  // scalar extended

// return A-block for the string *s with length n
A jtstr(J jt,I n,C*s){A z; GATV0(z,LIT,n,1); MC(AV(z),s,n); RETF(z);}

// return A-block for the string *s with length n, enclosed in quotes and quotes doubled
A jtstrq(J jt,I n,C*s){A z; I qc=2; DO(n, qc+=s[i]=='\'';) GATV0(z,LIT,n+qc,1); C *zv=CAV(z); *zv++='\''; DO(n, C c=s[i]; if(c=='\'')*zv++=c; *zv++=c;) *zv='\''; RETF(z);}

// w is a LIT string; result is a new block with the same string, with terminating NUL added
F1(jtstr0){A z;C*x;I n; ARGCHK1(w); ASSERT(LIT&AT(w),EVDOMAIN); n=AN(w); GATV0(z,LIT,n+1,1); x=CAV(z); MC(x,AV(w),n); x[n]=0; RETF(z);}

// return A-block for a 2-atom integer vector containing a,b
A jtv2(J jt,I a,I b){A z;I*x; GAT0(z,INT,2,1); x=AV(z); *x++=a; *x=b; RETF(z);}

// return A-block for singleton integer list whose value is k
A jtvci(J jt,I k){A z; GAT0(z,INT,1,1); IAV(z)[0]=k; RETF(z);}

// return A-block for list of type t, length n, and values *v
// MUST NOT return virtual or fixed block, because we often modify the returned area
A jtvec(J jt,I t,I n,void*v){A z; GA(z,t,n,1,0); MC(AV(z),v,n<<bplg(t)); RETF(z);}

// return A-block for list of type t, length n, and values *v
// with special handling to coerce boolean type.  We do not overfetch.
#if defined(__GNUC__) && !defined(__clang__)
#pragma push_options
#pragma optimize ("unroll-loops")
#endif
#if (C_AVX2&&SY_64) || EMU_AVX2
A jtvecb01(J jt,I t,I n,void*v){A z;
 GA(z,t,n,1,0);   // allocate buffer
 if(t&B01){C*p=(C*)AV(z),*q=v;
  // for booleans, enforce valid boolean result: convert any nonzero to 0x01
  __m256i zeros=_mm256_setzero_si256();
  __m256i ones=_mm256_castpd_si256(_mm256_broadcast_sd((D*)&Ivalidboolean));
// obsolete  __m256i ffs=_mm256_set1_epi8(0xffu);
  UI n0=n;  // number of bytes to do
#if 0 // obsolete  don't align the source, since that will unalign the dest, which is worse
UI mis=((uintptr_t)q)&31u;
mis=(mis>n0)?n0:mis;
if(mis){
n0-=mis;
#if defined(__clang__)
#pragma clang loop vectorize(enable) interleave_count(4)
#endif
while(mis--)*p++=!!(*q++);
}
#endif
  // move full 32-byte sections
  NOUNROLL while (n0 >= 32) {
// obsolete   _mm256_storeu_si256((__m256i *)p,_mm256_and_si256(_mm256_xor_si256(_mm256_cmpeq_epi8(_mm256_load_si256((__m256i*)q),zeros),ffs),ones));
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
#if 0 // obsolete 
if (n0 >= 16) {
__m128i zeros=_mm_setzero_si128();
__m128i ones=_mm_set1_epi8(1);
__m128i ffs=_mm_set1_epi8(0xffu);
 _mm_storeu_si128((__m128i *)p,_mm_and_si128(_mm_xor_si128(_mm_cmpeq_epi8(_mm_loadu_si128((__m128i*)q),zeros),ffs),ones));
 n0-=16;p+=16;q+=16;
}
#if defined(__clang__)
#pragma clang loop vectorize(enable) interleave_count(4)
#endif
#endif
  // finish by moving individual bytes
  NOUNROLL while(n0--)*p++=!!(*q++);
 }else{MC(AV(z),v,n<<bplg(t));}  // non-boolean, just copy
 RETF(z);
}
#elif __SSE2__
A jtvecb01(J jt,I t,I n,void*v){A z; GA(z,t,n,1,0);if(t&B01){C*p=(C*)AV(z),*q=v; 
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
A jtvecb01(J jt,I t,I n,void*v){A z; I i; GA(z,t,n,1,0);if(t&B01){C*p=(C*)AV(z),*q=v; 
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
F1(jtvi){ARGCHK1(w); R INT&AT(w)?w:cvt(INT,w);}

// Audit w to ensure valid integer value(s).  Error if non-integral.  Result is A block for integer array.  Infinities converted to IMAX/-IMAX
F1(jtvib){A z;D d,e,*wv;I i,n,*zv;
 ARGCHK1(w);
 if(AT(w)&INT)R RETARG(w);  // handle common non-failing cases quickly: INT and boolean
 if(AT(w)&B01){if(!AR(w))R zeroionei(BAV(w)[0]); R cvt(INT,w);}
 if(w==ainf)R imax;  // sentence words of _ always use the same block, so catch that too
 I p=-IMAX,q=IMAX;
 RANK2T oqr=jt->ranks; RESETRANK;
 if(unlikely((AT(w)&SPARSE)!=0))RZ(w=denseit(w));
 switch(AT(w)){
 default:
  if(!(AT(w)&FL))RZ(w=cvt(FL,w));
  n=AN(w); wv=DAV(w);
  GATV(z,INT,n,AR(w),AS(w)); zv=AV(z);
  for(i=0;i<n;++i){
   d=wv[i]; e=jround(d); I cval=(I)e;
   // if an atom is tolerantly equal to integer,  there's a good chance it is exactly equal.
   // infinities will always round to themselves
   ASSERT(d==e || FFIEQ(d,e),EVDOMAIN);
   cval=d<(D)-IMAX?-IMAX:cval; cval=d>=FLIMAX?IMAX:cval; zv[i]=cval;  // too-large values don't convert, handle separately
   }
   break;
  case XNUM:
  case RAT:  z=cvt(INT,maximum(sc(p),minimum(sc(q),w))); break;
 }
 jt->ranks=oqr; RETF(z);
}

// Convert w to integer if needed, and verify every atom is nonnegative
F1(jtvip){I*v; ARGCHK1(w); if(!(INT&AT(w)))RZ(w=cvt(INT,w)); v=AV(w); DQ(AN(w), ASSERT(0<=*v++,EVDOMAIN);); RETF(w);}

// Convert w to string, verify it is a list or atom
F1(jtvs){ARGCHK1(w); ASSERT(1>=AR(w),EVRANK); R LIT&AT(w)?w:cvt(LIT,w);}    
     /* verify string */

// Convert w to utf8 string, verify it is a list or atom
F1(jtvslit){ARGCHK1(w); ASSERT(1>=AR(w),EVRANK); R LIT&AT(w)?w:(C2T+C4T)&AT(w)?toutf8(w):cvt(LIT,w);}    
     /* verify string */
