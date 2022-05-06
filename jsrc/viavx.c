/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Index-of                                                         */

//TODO: don't EPILOG in indexofsub if there's not much to free.  That will save traversal of large boxed result

#include "j.h"
#include "vcomp.h"

// platforms with hardware crc32c
#if C_CRC32C && SY_64

#if !(C_AVX || EMU_AVX)
#error need avx or emulation to work
#endif

/* Floating point (type D) byte order:               */
/* Archimedes              3 2 1 0 7 6 5 4           */
/* VAX                     1 0 3 2 5 4 7 6           */
/* little endian           7 6 5 4 3 2 1 0           */
/* ThinkC MAC universal    0 1 0 1 2 3 4 5 6 7 8 9   */
/* ThinkC MAC 6888x        0 1 _ _ 2 3 4 5 6 7 8 9   */
/* normal                  0 1 2 3 4 5 6 7 ...       */

#if C_LE
#define MSW 1   /* most  significant word */
#define LSW 0   /* least significant word */
#else
#define MSW 0
#define LSW 1
#endif

// create a mask of bits in which a difference is considered significant for floating-point purposes.
// cct is complementary comparison tolerance; return 
static UIL calcctmask(D cct){
 // New version: we have to find the max maskvalue such that x+tolerance and x-tolerance are not separated by more than one
 // maskpoint, for any x.  The worst-case x occurs where the mantissa of x is as big as it can be, e. g. 1.1111111111...
 // At that point the tolerance band above is t/(t+1) times the approx value (2 in the example).  The tolerance band below is about the same,
 // and the sum of the two must not exceed the mask size.  We calculate the mask in floating point as 2 - 2 * 2*(t/(t+1)) which will
 // give its floating-point representation.  We then adjust the mask by forcing the exponent to be masked, and clearing any bits below the
 // highest clear bit
 if(likely(cct==1.0-FUZZ))R (UIL)0xfffffffffffffc00LL;  // default ct
 if(likely(cct==1.0))R ~0LL;  // intolerant ct
 // user specified ct.  Calculate the mask for it
 D p=2.0 - (4.0*(1.0-cct))/(2.0-cct);
 UIL q=(~*(UIL*)&p)<<16;  // shift exponent away & complement
 I zeropos; CTLZI(q,zeropos);   // get bit# of highest 0 bit (+ 16, because of shift) - 15 if all bits were 1
 R ((UIL)~0LL)<<(zeropos-15);
}

// Routine to allocate sections of the hash tables
// *hh is the hash table we have selected, p is the number of hash entries we need, asct is the maximum+1 value that needs to be stored in an entry
// md gives information about the type of entry, in particular if it is bits or packed bits
// Main result is in *hh, notably currentlo/currentindexofst.  Other fields have been adjusted to account for the allocated space
// The nominal result is the new value for md.  This routine is responsible for setting/clearing IIMODBASE0 appropriately, but only when IIMODBITS is clear
static I hashallo(IH * RESTRICT hh,UI p,UI asct,I md){
 // If the request is for bits, allocate them starting at the beginning of the table.
 if(md&IIMODBITS){
  hh->currentlo=0; hh->currentindexofst=0;  // Bits always start at 0
  // Since bits invalidate the memory ranges, record what parts are invalid
  hh->invalidlo=0;  // invalid area starts at 0
  if(md&IIMODPACK)p=(p+15)>>3;  // if bits are packed, convert to byte count.  Pad first and last bytes
  // Round p up to multiple of I, then convert to hashtable entries. Must do this for endian reasons, to avoid a hole in the invalid region
  p = (p+(SZI-1))&-SZI;  // round up to number of bytes that need to be cleared to clear Is
  // Clear the bits before returning.  We init packed bits to 0, but byte-bits to a value that depends on the function being performed.
  // ~. ~: I.@~. -.   all prefer the table to be complemented and thus initialized to 1.
  // REVERSED types always initialize to 1, whether packed or not
  // this is a kludge - the initialization value should be passed in by the caller, in asct
  UI fillval = md&IREVERSED?(md&IIMODPACK?255:1):((md&(IIMODPACK+IIOPMSK))<=INUBI); fillval|=fillval<<8; fillval|=fillval<<16;  // mvc overfetches, so need full UI
  mvc(p,hh->data.UI,4,&fillval);  // fill with repeated copies of fillval
  // If the invalid area grows, update the invalid hwmk, and also the partition
  p >>= hh->hashelelgsize;  // convert p to hash index 
  if(p>hh->invalidhi){
   hh->currenthi = MAX(hh->currenthi,p);  // adjust partition to be after the invalid area
   hh->invalidhi=p;  // adjust high-water mark
  }
  R md;
 }
 // Not bits. We have a choice of allocating on the left or the right.  Allocating on the left creates cache coherence
 // but tends to waste address points.  Calculate the cost of each and choose.

 // if we are forced to start over, do so
 md |= IINOTALLOCATED;   // indicate not allocated yet
 if(!(md&IIMODFORCE0)){   // this is never true for AVX code, which always clears the region
  // Not forced to start over, choose an allocation

  // First of all: it's moot if the allocation won't fit on the right
  I maxn = hh->datasize>>hh->hashelelgsize;  // get max possible index+1
  I selside=0;  // default to allocating on the left (i. e. at 0)
  UI maxindex = (((I)1)<<(((UIL)SZI)<<hh->hashelelgsize))-1;  // largest possible index for this table
  UI indexceil=maxindex-asct;  // max starting index

  // Cost of allocating on the left comes
  // (1) now, as we use (asct*currenthi) units of index space
  // (2) in the future, as all pyx allocations on the right lose index space,
  // in the amount of (new left index end-right index end-pyx asct)*(width-currenthi).  We estimate future values to equal current ones.
  // (3) now, if we have to clear the invalid area (1 store per word cleared)
  // BUT: if the cleared invalid area covers the entire p, we use asct rather than left-index+asct for calculating the other costs
  // The cost of a unit of index space is 1 store per (width*maxindex)/(asct*p) index-space unit, with asct and p rounded up if large;
  // we will approximate as 2*(asct*p)/(width*maxindex) 

  // Cost of allocating on the right is
  // (p*asct) units of index space for the allocated area, plus (currenthi*((right index+asct)-left index)) units of index space lost on the left,
  // plus cache expense of p<.currenthi cache words
  // The cost of a cache word is reckoned at 1/2 store per word

  // That's not worth figuring out for every call.  So, we will just allocate on the right unless
  // the invalid area is empty and asct<4096 (about the size of L1 cache), to get the caching benefit for small arguments
  if(p <= maxn-hh->currenthi && hh->previousindexend < indexceil && hh->invalidhi<hh->currenthi && !(asct<4096 && (hh->invalidlo>=p))) {  // the right side is a possibility
   // Allocating right.  Return a region starting at currenthi
   md &= ~(IIMODBASE0|IINOTALLOCATED);  // can't use the fastest code if we didn't clear; but we allocated it
   hh->currentlo=hh->currenthi; hh->currenthi+=p;   // set return value (starting position) and partition
   UI startx=hh->previousindexend;
   hh->currentindexofst=startx; hh->previousindexend=(startx+=asct);  // set return value (starting index) and allocated index space
   hh->currentindexend=MAX(startx,hh->currentindexend);  // since currentindexend speaks for the whole left side, we may have to push it up
   // Since the partition (currenthi) is kept to the right of the invalid region, there is no need to clear invalid data
  }else if(hh->currentindexend < indexceil){
   // Allocating left.  Return a region starting at position 0.
   md &= ~(IIMODBASE0|IINOTALLOCATED);  // can't use the fastest code if we didn't clear; but we allocated
   hh->currentlo=0; hh->currenthi=p;   // set return value (starting position) and partition
   UI startx=hh->currentindexend;   // the previous end+1 index becomes the index for this time
   hh->previousindexend=startx;  // bring up the right side to match the left side before we advance
   hh->currentindexofst=startx; hh->currentindexend=(startx+=asct);  // set return value (starting index) and allocated index space
   // If (part of) the return area needs to be cleared, clear it.  If we clear the entire left side, reduce the left index
   UI clrpt = MIN(p, hh->invalidhi);   // get index to clear to
   I nclrhsh = clrpt - hh->invalidlo;  // get number of hash entries to clear
   if(nclrhsh>0){   // if there is something to clear
    mvc((nclrhsh<<hh->hashelelgsize),hh->data.UC+(hh->invalidlo<<hh->hashelelgsize),1,MEMSET00);  // clear the region to 0
    if(p<=clrpt){startx=hh->currentindexofst;hh->currentindexend=MAX(asct,hh->currentindexofst);}  // If we cleared the whole left side, we can back the left-side pointer to match the right
    hh->invalidlo=clrpt;  // Indicate that we have cleared this region
   }
  }
 }
 if(md&IINOTALLOCATED) {
  // Cannot allocate as is: the region must be initialized.  set BASE0 in this case, and since we initialize, initialize to the most useful value
  md |= IIMODBASE0;  // if we clear the region, mention that so that we get the fastest code
  // Clear the entries of the first allocation to asct.  Use fullword stores or cache-line stores.  Our allocations are always multiples of fullwords,
  // so it is safe to overfill with fullword stores
  UI storeval=asct; if(hh->hashelelgsize==1)storeval |= storeval<<16;  // Pad store value to 64 bits, dropping excess on smaller machines.  mvc overfetches
  mvc(((p<<hh->hashelelgsize)+SZI-1)&-SZI,hh->data.UI,4,&storeval);  // fill with repeated copies of fillval
  // Clear everything past the first allocation to 0, indicating 'not touched yet'.  But we can elide this if it is already 0, which we can tell by
  // examining the partition pointer and the right-hand index.  This is important if FORCE0 was set for i."r: we will repeatedly reset the base, and
  // we need to avoid clearing the whole buffer for any time after the first.
  // We also don't want to clear the whole buffer if we were trying to save a little time by setting BASE0 in an argument that uses only a little
  // of the table.
  // We have to make sure we are not moving the partition to the left: that might expose some uninitialized values on the right side
  if(md&IIMODFORCE0){
   // Putting that together: if FORCE0 is set, we just clear the left side and leave the right alone, except that we may move the partition right.
   if(p>=hh->currenthi){
    // Moving the partition right (or not at all).  That doesn't uncover anything
    hh->currenthi=p;   // set the new partition pointer
    hh->currentindexend=MAX(hh->previousindexend,1+asct);  // keep the left-side max index no less than the right-wide
   }else{
    // We didn't clear as far as the partition.  The left side still contains old data.
    if(hh->currentindexend<1+asct)hh->currentindexend=1+asct;  // Don't lower the index if there is uncleared data on the left
   }
  }else{
   // Not FORCE0: we are clearing because we have to.  Clear everything.  But we can save clearing the right side if it's already clear.
   I clrtopoint=(hh->previousindexend!=1)?hh->datasize:hh->currenthi<<hh->hashelelgsize;  // high+1 entry to clear
   I clrfrompoint=p<<hh->hashelelgsize;  // offset to clear from
   if((clrtopoint-=clrfrompoint)>0){mvc( clrtopoint,hh->data.UC+clrfrompoint,1,MEMSET00);}  // clear the region to 0
   hh->currenthi=p;   // set return value (starting position) and partition
   hh->currentindexend=1+asct;  // set return value (starting index) and allocated index space.  Leave 0 for 'not found'
   hh->previousindexend=1;  // Init right side unused (but with the 0s representing initialized values)
  }
  hh->currentindexofst=0;  // Indic left-side starting index (return value)
  hh->currentlo=0;    // Indic left-side starting position (return value)
  hh->invalidlo=IMAX; hh->invalidhi=0;  // clear invalidity region
 }
 R md;
}

// All hashes must return a CRC result, because that is evenly distributed throughout the lower 32 bits
// CRC32L  takes UI  (4 or 8 bytes)
// CRC32LL takes UIL (8 bytes)

#define RETCRC3 R CRC32L(crc0,CRC32L(crc1,crc2))
// Create CRC32 of the k bytes in *v.  Uses CRC32L to process 8 bytes at a time
// We may fetch past the end of the input, but only up to the next SZI-byte block
UI hic(I k, UC *v) {
 // Do 3 CRCs in parallel because the latency of the CRC instruction is 3 clocks.
 // This is executed repeatedly so we expect all the branches to predict correctly
 UI crc0=-1, crc1=crc0, crc2=crc0;  // init all CRCs
 for(;k>=3*SZI;v+=3*SZI,k-=3*SZI){  // Do blocks of 24 bytes
  crc0=CRC32L(crc0,((UI*)v)[0]); crc1=CRC32L(crc1,((UI*)v)[1]); crc2=CRC32L(crc2,((UI*)v)[2]);
 }
 // The order of this runout is replicated in the other character routines
 if(k>=SZI){crc0=CRC32L(crc0,((UI*)v)[0]); v+=SZI;}  // finish the remnant
 if(k>=2*SZI){crc1=CRC32L(crc1,((UI*)v)[0]); v+=SZI;}
 if(k&=(SZI-1)){  // last few bytes
  crc2=CRC32L(crc2,((UI*)v)[0]&~((UI)-((I)1)<<(k<<3)));  // mask out invalid bytes - must use 64-bit shift!
 }
 RETCRC3;
}

#define hicnz hic  // hicnz omits 0/255 bytes.  If this is just for performance, get rid of it

// collect count LS sections of size lgsize (in bits) from v, where sections are separated by lgspacing bytes and we start with section number index 
static UI fetchspread(UC *v, I lgspacing, I lgsize, I count, I index){UI ret = 0;
 DO(count, ret |= (UI)*(US*)(v+((index+i)<<lgspacing))<<(i<<lgsize););  // read 2 bytes to avoid overrun; expand to 64, shift
 R ret;
}
// Hash a C4T to be compatible with C2T and LIT
// Scan to see if the C4T can be represented exactly as a C2T or LIT; if so,
// hash only those bytes
// k is the number of BYTES (for compatibility with previously-existing interfaces)
UI hic4(I k, UC* v){I cct=k>>LGSZI4;
 // Scan to find precision needed.  Since this will probably kick out quickly, we don't bother
 // unrolling the loop
 I max = 0;
 DQ(cct, max |= ((UI4*)v)[i]; if(((UI4*)v)[i]>65535)R hic(k,v););  // If significance > C2T, hash as C4T - 4 bytes per char
 UI crc0=-1, crc1=crc0, crc2=crc0;
 if(max<=255){  // if nothing bigger than LIT, hash as LIT as a canonical form
  // hash as if LIT
#if SY_64
  for(;cct>=24;v+=24*sizeof(UI4),cct-=24){  // Do blocks of 24 bytes
   crc0=CRC32L(crc0,fetchspread(v,2,3,8,0)); crc1=CRC32L(crc1,fetchspread(v,2,3,8,8)); crc2=CRC32L(crc2,fetchspread(v,2,3,8,16));
#else
  for(;cct>=12;v+=12*sizeof(UI4),cct-=12){  // Do blocks of 12 bytes
   crc0=CRC32L(crc0,fetchspread(v,2,3,4,0)); crc1=CRC32L(crc1,fetchspread(v,2,3,4,4)); crc2=CRC32L(crc2,fetchspread(v,2,3,4,8));
#endif
  }
  // The order of this runout exactly matches hic(): crc0 gets first full 8 if any, crc1 gets next full 8; crc2 takes any shard
#if SY_64
  if(cct>=8){crc0=CRC32L(crc0,fetchspread(v,2,3,8,0)); v+=32;}  // finish the remnant
  if(cct>=16){crc1=CRC32L(crc1,fetchspread(v,2,3,8,0)); v+=32;}
  if(cct&=7){  // last few bytes
   crc2=CRC32L(crc2,fetchspread(v,2,3,cct,0));  // mask out invalid bytes
#else
  if(cct>=4){crc0=CRC32L(crc0,fetchspread(v,2,3,4,0)); v+=16;}  // finish the remnant
  if(cct>=8){crc1=CRC32L(crc1,fetchspread(v,2,3,4,0)); v+=16;}
  if(cct&=3){  // last few bytes
   crc2=CRC32L(crc2,fetchspread(v,2,3,cct,0));  // mask out invalid bytes
#endif
  }
 } else {
  // there was a non-ASCII character, so hash as if C2T
#if SY_64
  for(;cct>=12;v+=12*sizeof(UI4),cct-=12){  // Do blocks of 24 bytes
   crc0=CRC32L(crc0,fetchspread(v,2,4,4,0)); crc1=CRC32L(crc1,fetchspread(v,2,4,4,4)); crc2=CRC32L(crc2,fetchspread(v,2,4,4,8));
#else
  for(;cct>=6;v+=6*sizeof(UI4),cct-=6){  // Do blocks of 12 bytes
   crc0=CRC32L(crc0,fetchspread(v,2,4,2,0)); crc1=CRC32L(crc1,fetchspread(v,2,4,2,2)); crc2=CRC32L(crc2,fetchspread(v,2,4,2,4));
#endif
  }
#if SY_64
  if(cct>=4){crc0=CRC32L(crc0,fetchspread(v,2,4,4,0)); v+=16;}  // finish the remnant
  if(cct>=8){crc1=CRC32L(crc1,fetchspread(v,2,4,4,0)); v+=16;}
  if(cct&=3){  // last few bytes
   crc2=CRC32L(crc2,fetchspread(v,2,4,cct,0));  // mask out invalid bytes
#else
  if(cct>=2){crc0=CRC32L(crc0,fetchspread(v,2,4,2,0)); v+=8;}  // finish the remnant
  if(cct>=4){crc1=CRC32L(crc1,fetchspread(v,2,4,2,0)); v+=8;}
  if(cct&=1){  // last few bytes
   crc2=CRC32L(crc2,fetchspread(v,2,4,cct,0));  // mask out invalid bytes
#endif
  }
 }
 RETCRC3;

}

// Similarly, hash a C2T for compatibility with LIT
UI hic2(I k, UC* v){I cct=k>>LGSZS;
 // Scan to find precision needed.  Since this will probably kick out quickly, we don't bother
 // unrolling the loop
 DQ(cct, if(((US*)v)[i]>255)R hic(k,v);); // if upper significance, hash C2T: 2 bytes per char
 // hash as if LIT, low bytes only
 UI crc0=-1, crc1=crc0, crc2=crc0;
#if SY_64
 for(;cct>=24;v+=24*sizeof(US),cct-=24){  // Do blocks of 24 bytes
  crc0=CRC32L(crc0,fetchspread(v,1,3,8,0)); crc1=CRC32L(crc1,fetchspread(v,1,3,8,8)); crc2=CRC32L(crc2,fetchspread(v,1,3,8,16));
#else
 for(;cct>=12;v+=12*sizeof(US),cct-=12){  // Do blocks of 12 bytes
  crc0=CRC32L(crc0,fetchspread(v,1,3,4,0)); crc1=CRC32L(crc1,fetchspread(v,1,3,4,4)); crc2=CRC32L(crc2,fetchspread(v,1,3,4,8));
#endif
 }
#if SY_64
 if(cct>=8){crc0=CRC32L(crc0,fetchspread(v,1,3,8,0)); v+=16;}  // finish the remnant
 if(cct>=16){crc1=CRC32L(crc1,fetchspread(v,1,3,8,0)); v+=16;}
 if(cct&=7){  // last few bytes
  crc2=CRC32L(crc2,fetchspread(v,1,3,cct,0));  // mask out invalid bytes
#else
 if(cct>=4){crc0=CRC32L(crc0,fetchspread(v,1,3,4,0)); v+=8;}  // finish the remnant
 if(cct>=8){crc1=CRC32L(crc1,fetchspread(v,1,3,4,0)); v+=8;}
 if(cct&=3){  // last few bytes
  crc2=CRC32L(crc2,fetchspread(v,1,3,cct,0));  // mask out invalid bytes
#endif
 }
 RETCRC3;
}

// Hash a single INT-sized atom
#define hici1(x) CRC32L(-1LL,*(x))

// Hash an INT list
static __forceinline UI hici(I k, UI* v){
 // Owing to latency, hash 3 inputs at a time; but not if short.  Length is never 0 but can be 1.
 if((k-=3)<=0){  // fast path for len<=3.  We think all these branches will predict correctly
  UI crc; crc=CRC32L(-1LL,v[0]); if(k>=-1){crc=CRC32L(crc,v[1]); if(k==0)crc=CRC32L(crc,v[2]);} R crc;
 }
 UI crc0=-1, crc1=crc0, crc2=crc0;
 NOUNROLL do{
  crc0=CRC32L(crc0,v[0]); crc1=CRC32L(crc1,v[1]); crc2=CRC32L(crc2,v[2]);
  v+=3, k-=3;
 }while(k>=0);  // at end k is negative, and we have gone through the loop origk%3 times
 if(k>-2){crc1=CRC32L(crc1,v[1]); crc0=CRC32L(crc0,v[0]);}else if(k==-2){crc0=CRC32L(crc0,v[0]);}
 RETCRC3;
}

// Hash a single FL atom, with check for -0 and +0
#define hic01(x) ((*(UIL*)(x)!=(UIL)NEGATIVE0)?CRC32LL(-1L,*(UIL*)(x)):CRC32LL(-1L,0))

// Hash a FL list, with check for -0 and +0
static UI hic0(I k, UIL* v){
 // Owing to latency, hash pairs of inputs.  Check each for -0
 UI crc0=-1, crc1=crc0;
 for(k-=2;k>=0;v+=2, k-=2){
  if(likely(v[0]!=(UIL)NEGATIVE0)){crc0=CRC32LL(crc0,v[0]);}else{crc0=CRC32LL(crc0,0);}
  if(likely(v[1]!=(UIL)NEGATIVE0)){crc1=CRC32LL(crc1,v[1]);}else{crc1=CRC32LL(crc1,0);}
 }
 if(k>-2){if(likely(v[0]!=(UIL)NEGATIVE0)){crc0=CRC32LL(crc0,v[0]);}else{crc0=CRC32LL(crc0,0);}}
 R CRC32L(crc0,crc1);
}


// Hashes for extended/rational types.  Hash only the numerator of rationals.  These are
// Q and X types (Q is a brace of X types)
static UI hix(X*v){A y=*v;   R hici(AN(y),UIAV(y));}
static UI hiq(Q*v){A y=v->n; R hici(AN(y),UIAV(y));}


// Hash a single double, using only the bits in ctmask.
//  not required for tolerant comparison, but if we tried to do tolerant comparison through the fast code it would help
static UI cthid(UIL ctmask,D d){R likely(*(UIL*)&d!=(UIL)NEGATIVE0)?CRC32LL(-1L,*(UIL*)&d&ctmask):CRC32LL(-1L,0);}

// Hash the data in the given A, which is an element of the box we are hashing
// If empty or boxed, hash the shape
// If literal, hash the whole thing
// If numeric, convert first atom to float and hash it after multiplying by hct.  hct will change to give low/high values
// Q: called only for singletons?
static UI cthia(UIL ctmask,D hct,A y){UC*yv;D d;I n,t;Q*u;
 n=AN(y); t=AT(y); yv=UAV(y);
 if(((n-1)|SGNIF(t,BOXX))<0)R hic((I)AR(y)*SZI,(UC*)AS(y));  // boxed or empty
 switch(CTTZ(t)){
 case INTX:  d=(D)*(I*)yv; break;
 case LITX:  R hic(n,yv);
 case C2TX:  R hic2(2*n,yv);
 case C4TX:  R hic4(4*n,yv);
 case SBTX:  R hic(n*SZI,yv);
 case B01X:  d=*(B*)yv; break;
 case FLX: 
 case CMPXX: d=*(D*)yv; break;
 case XNUMX: d=xdouble(*(X*)yv); break;
 case RATX:  u=(Q*)yv; d=xdouble(u->n)/xdouble(u->d); break;
 }
 R cthid(ctmask,d*hct);
}

// Hash y, which is not a singleton.
static UI jthiau(J jt,A y){I m,n;UI z;X*u,x;
 m=n=AN(y); UC*v=UAV(y);
 if(!n)R 0;
 switch(AT(y)){
 case INT:  R hici(n,AV(y));
 case RAT:  m+=n;  /* fall thru */
 case XNUM: z=-1LL; u=XAV(y); DQ(m, x=*u++; v=UAV(x); z=CRC32((UI4)z,(UI4)hicnz(AN(x)*SZI,UAV(x)));); R z;
 default:   R hic(n<<bplg(AT(y)),UAV(y));
 }
}

// Comparisons for extended/rational/float/complex types.
static B jteqx(J jt,I n,X*u,X*v){DQ(n, if(!equ(C(*u),C(*v)))R 0; ++u; ++v;); R 1;}
static B jteqq(J jt,I n,Q*u,Q*v){DQ(n, if(!QEQ(*u,*v))R 0; ++u; ++v;); R 1;}
static B jeqd(I n,D*u,D*v,D cct){DQ(n, if(!TCMPEQ(cct,*u,*v))R 0; ++u; ++v;); R 1;}
static B jteqz(J jt,I n,Z*u,Z*v){DQ(n, if(!zeq(*u,*v))R 0; ++u; ++v;); R 1;}

// test a subset of two boxed arrays for match.  u/v point to pointers to contents
// We test n subboxes
static B jteqa(J jt,I n,A*u,A*v){DQ(n, if(!equ(C(*u),C(*v)))R 0; ++u; ++v;); R 1;}
// same but intolerant (used for write probes)
static B jteqa0(J jt,I n,A*u,A*v){PUSHCCT(1.0) B res=1; DQ(n, if(!equ(C(*u),C(*v))){res=0;break;}; ++u; ++v;); POPCCT R res;}


/*
 mode one of the following:
       0  IIDOT      i.      a w rank
       1  IICO       i:      a w rank
       2  INUBSV     ~:        w rank
       3  INUB       ~.        w
       4  ILESS      -.      a w
       5  INUBI      I.@~:     w
       6  IEPS       e.      a w rank
       7  II0EPS     e.i.0:  a w   
       8  II1EPS     e.i.1:  a w
       9  IJ0EPS     e.i:0:  a w  
       10 IJ1EPS     e.i:1:  a w
       11 ISUMEPS    [:+ /e. a w     
       12 IANYEPS    [:+./e. a w    
       13 IALLEPS    [:*./e. a w    
       14 IIFBEPS    I.@e.   a w    
       30 IPHIDOT    i.      a w     prehashed
       31 IPHICO     i:      a w     prehashed
       34 IPHLESS    -.      a w     prehashed  no longer supported
       36 IPHEPS     e.      a w     prehashed
       37 IPHI0EPS   e.i.0:  a w     prehashed
       38 IPHI1EPS   e.i.1:  a w     prehashed
       39 IPHJ0EPS   e.i:0:  a w     prehashed
       40 IPHJ1EPS   e.i:1:  a w     prehashed
       41 IPHSUMEPS  [:+ /e. a w     prehashed
       42 IPHANYEPS  [:+./e. a w     prehashed
       43 IPHALLEPS  [:*./e. a w     prehashed
       44 IPHIFBEPS  I.@e.   a w     prehashed
 asct    target axis length (number of things to be searched from in a single pass)
 n    target item # atoms
 wsct    # target items in a left-arg cell, which may include multiple right-arg cells (number of searches)
 k    target item # bytes
 ac   # left  arg cells  (cells, NOT items)
 wc   # right arg cells
 ak   # bytes left  arg cells, or 0 if only 1 cell   scaf unnecessary?  not used in smallrange
 wk   # bytes right arg cells, or 0 if only one cell
 a    left  arg
 w    right arg, or mark for m&i. or m&i: or e.&n or -.&n
 h   pointer to hash table or to 0
 z    result
*/

#define IOF(f)     A f(J jt,I mode,I n,I asct,I wsct,I ac,I wc,A a,A w,A z,I k,I ak,I wk,A h)  // asct/wsct go by m and c
//J jt,I mode,I n,I asct,I wsct,I ac,I wc,A a,A w,A z
// variables used in IOF routines:
// h=A for hashtable, hv->hashtable data, p=#entries in table, pm=unsigned p, used for converting hash to bucket#
// acn,wcn=#atoms in cell of a,w
// cn = #atoms in target item
// j is the starting bucket number of the hashtable search
// hj is the index of the first empty-or-matching bucket encountered
// zv->result area (as an array of pointers), av->a data, wv->w data 



// get hash slot and set up for the search.  j holds the slot
#if SY_64
#define HASHSLOT(hash) j=((hash)*p)>>32;
#else
#define HASHSLOT(hash) j=((hash)*(UIL)(p))>>32;
#endif

// Misc code to set the shape once we see how many results there are, used for ~. y and x -. y
#define ZISHAPE    AS(z)[0]=AN(z)=zi-zv
#define ZCSHAPE    AS(z)[0]=(zc-(C*)zv)/k; AN(z)=n*AS(z)[0]
#define ZUSHAPE(T) AS(z)[0]= zu-(T*)zv;    AN(z)=n*AS(z)[0]


// *************** first class: intolerant comparisons, unboxed or boxed ***********************

// Routines to build the hash table from a.  hash calculates the hash function, usually referring to v (the input) and possibly other names.  exp is the comparison routine.

// Calculate the hash slot.  The hash calculation (input parm) relies on the name v and produces the name j.  We have moved v to an xmm register to reduce register pressure
// here, so extract its parts for use as needed
#define HASHSLOTP(T,hash) v=(T*)_mm_extract_epi64(vp,0); j=((hash)*(UIL)(p))>>32;
// Conditionally insert a new value into the hash table.  The initial value of hj (the table scan pointer) has been fetched.  name is the name holding the slot to be added
// (it will be j, j1, or j2 depending on where we are in the processing pipeline),
// exp is a comparison expression meaning 'mismatch' that returns 0 if the data indexed by the slot is equal to *v (the expression will use *v as the new value, and hj as the index into the
// original input av to point to the value represented in the hashtable).  hj advances through the hash until an empty slot or a match is found.
// The v value is stored in an xmm register and brought out into v for use by (exp); it is a delayed version of the v that was used for HASHSLOT.
// The table search goes in descending order, and wraps around to p-1 (the last entry in the table).
// What happens after the search depends on the last 3 parameters:
// If (store) is 1, the value of i (which is the loop index giving the position within a of the item being processed) is stored into the empty hash slot,
// only if the hash search does not find a match.  If (store) is 2, the entry that we found is cleared, by setting it to maxcount+1, when we find a match.
// When (store)=2, we also ignore hash entries containing maxcount+1, treating them as failed compares
// Independent of (store), (fstmt) is executed if the item is found in the hash table, and (nfstmt) is executed if it is not found.
#define FINDP(T,TH,hsrc,name,exp,fstmt,nfstmt,store) NOUNROLL do{if(hj==hsrc##sct){ \
  if(store==1)hv[name]=(TH)i; nfstmt break;}  /* this is the not-found case */ \
  if((store!=2||hj<hsrc##sct)&&(v=(T*)_mm_extract_epi64(vp,1),!(exp))){if(store==2)hv[name]=(TH)(hsrc##sct+1); fstmt break;} /* found */ \
  if(unlikely(--name<0))name+=p; hj=hv[name]; /* miscompare, nust continue search */ \
  }while(1);

// Traverse the hash table for one argument.  (src) indicates which argument, a or w, we are looping through; (hsrc) indicates which argument provided the hash table.
// For each item we do HASHSLOT folowed by FINDP, and adjust the (v) values (both stored in xmm variable vp) to keep track
// of which input item is being operated on.  This loop is triple-unrolled, so that after we HASHSLOT for entry q, we FINDP for entry q-2.  As soon as we HASHSLOT for entry
// q, we prefetch it into L1 cache; then as soon as we have finished the last store for entry q-1, we do the fetch for entry q (which will be fetching while the hash for entry
// q+2 is being calculated).
// The (fstmt,nfstmt,store) arguments indicate what to do when a match/notmatch is resolved.
// (loopctl) give the stride through the input array, the control for the main loop, and the index of the last value.  These values differ for forward and reverse scans through the input.
#define XSEARCH(T,TH,src,hsrc,hash,exp,stride,fstmt,nfstmt,store,vpofst,loopctl,finali) \
 {I i, j, hj; T *v; vp=_mm_insert_epi64(vp,(I)(src##v+vpofst),0); vpstride = _mm_insert_epi64(vp,(stride)*(I)sizeof(T),0); vp=_mm_shuffle_epi32(vp,0x44); vpstride=_mm_insert_epi64(vpstride,0LL,1); \
 HASHSLOTP(T,hash) if(src##sct>1){I j1,j2; vp=_mm_add_epi64(vp,vpstride); j1=j; HASHSLOTP(T,hash) hj=hv[j1]; vp=_mm_add_epi64(vp,vpstride); vpstride=_mm_shuffle_epi32(vpstride,0x44); \
 for loopctl {j2=j1; j1=j; HASHSLOTP(T,hash) PREFETCH((C*)&hv[j]); FINDP(T,TH,hsrc,j2,exp,fstmt,nfstmt,store); vp=_mm_add_epi64(vp,vpstride); hj=hv[j1];} \
 FINDP(T,TH,hsrc,j1,exp,fstmt,nfstmt,store); vp=_mm_add_epi64(vp,vpstride);} hj=hv[j]; i=finali; FINDP(T,TH,hsrc,j,exp,fstmt,nfstmt,store); }

// Traverse a in forward direction, adding values to the hash table
#define XDOAP(T,TH,hash,exp,stride) XSEARCH(T,TH,a,a,hash,exp,stride,{},{},1,0, (i=0;i<asct-2;++i) ,asct-1)
// Traverse w in forward direction, executing fstmt/nfstmt depending on found/notfound; and adding to the hash if (reflex) is 1, indicating a reflexive operation
#define XDOP(T,TH,hash,exp,stride,fstmt,nfstmt,reflex) XSEARCH(T,TH,w,a,hash,exp,stride,fstmt,nfstmt,reflex,0, (i=0;i<wsct-2;++i) ,wsct-1)
// same for traversing a/w in reverse
#define XDQAP(T,TH,hash,exp,stride) XSEARCH(T,TH,a,a,hash,exp,(-(stride)),{},{},1,cn*(asct-1), (i=asct-1;i>1;--i) ,0)
#define XDQP(T,TH,hash,exp,stride,fstmt,nfstmt,reflex) XSEARCH(T,TH,w,a,hash,exp,(-(stride)),fstmt,nfstmt,reflex,cn*(wsct-1), (i=wsct-1;i>1;--i) ,0)

// special lookup routines to move the data rather than store its index, used for nub/less
#define XMVP(T,TH,hash,exp,stride,reflex)      \
 if(k==SZI){XDOP(T,TH,hash,exp,stride,{},{*(I*)zc=*(I*)_mm_extract_epi64(vp,1); zc+=SZI;},reflex); }  \
 else      {XDOP(T,TH,hash,exp,stride,{},{MC(zc,(C*)_mm_extract_epi64(vp,1),k); zc+=k;},reflex); }
// version for ([ -. -.)
#define XMVPI(T,TH,hash,exp,stride,reflex)      \
 if(k==SZI){XDOP(T,TH,hash,exp,stride,{*(I*)zc=*(I*)_mm_extract_epi64(vp,1); zc+=SZI;},{},reflex); }  \
 else      {XDOP(T,TH,hash,exp,stride,{MC(zc,(C*)_mm_extract_epi64(vp,1),k); zc+=k;},{},reflex); }

// The main search routine, given a, w, mode, etc, for datatypes with no comparison tolerance

// if there is not a prehashed hashtable, we clear the hashtable and fill it from a, then hash & check each item of w
#define IOFX(T,TH,f,setup,hash,exp,stride)   \
 IOF(f){I acn=ak/sizeof(T),cn=k/sizeof(T),l,p,  \
        wcn=wk/sizeof(T),*zv=AV(z);T* RESTRICT av=(T*)AV(a),* RESTRICT wv=(T*)AV(w);I md; TH * RESTRICT hv; \
        IH *hh=IHAV(h); p=hh->datarange;  hv=hh->data.TH;  \
 \
  __m128i vp, vpstride;   /* v for hash/v for search; stride for each */ \
  _mm256_zeroupperx(VOIDARG)  \
  setup \
  vp=_mm_setzero_si128();  /* to avoid warnings */ \
  md=mode&IIOPMSK;   /* clear upper flags including REFLEX bit */                                            \
    /* look for IIDOT/IICO/INUBSV/INUB/INUBI/IFORKEY - we set IIMODREFLEX if one of those is set */ \
  if(!(((uintptr_t)a^(uintptr_t)w)|(ac^wc)))md|=IIMODREFLEX&((((1<<IIDOT)|(1<<IICO)|(1<<INUBSV)|(1<<INUB)|(1<<INUBI)|(1<<IFORKEY))<<IIMODREFLEXX)>>md);  /* remember if this is reflexive, which doesn't prehash */  \
  if(w==mark){wsct=0;}   /* if prehashing, turn off the second half */                          \
  I wrepssv=0; if(1==wc){wrepssv=ac;}else if(unlikely(ac>wc))wrepssv=(ac/wc)-1; I wreps=wrepssv;  \
  for(l=0;l<ac;++l,av+=acn,wv+=--wreps<0?wcn:0,wreps=wreps<0?wrepssv:wreps){                                                 \
   /* zv progresses through the result - for those versions that support IRS */ \
   if(!(mode&IPHOFFSET)){hashallo(hh,p,asct,mode); if(!(md&IIMODREFLEX)){if(md==IICO)XDQAP(T,TH,hash,exp,stride) else XDOAP(T,TH,hash,exp,stride);} if(wsct==0)break;}  \
   switch(md){ \
    /* i.~ - one-pass operation.  Fill in the table and result as we go */ \
   case IIDOT|IIMODREFLEX: { XDOP(T,TH,hash,exp,stride,{*zv++=hj;},{*zv++=i;},1);} break;      \
   case IFORKEY|IIMODREFLEX: { I nuniq=0; XDOP(T,TH,hash,exp,stride,{++zv[hj-i]; *zv++=hj;},{++nuniq; *zv++=i+1;},1); AM(h)=nuniq;} break;      \
   case IICO|IIMODREFLEX: {I *zi=zv+=wsct; XDQP(T,TH,hash,exp,stride,{*--zi=hj;},{*--zi=i;},1) } break;      \
    /* normal i./i: - use the table */ \
   case IICO: \
   case IIDOT: { XDOP(T,TH,hash,exp,stride,{*zv++=hj;},{*zv++=hj;},0); }                          break;  \
   case INUBSV|IIMODREFLEX: { B *zb=(B*)zv; XDOP(T,TH,hash,exp,stride,{*zb++=0;},{*zb++=1;},1) zv=(I*)zb;} /* IRS - keep zv running */  break;  \
   case INUB|IIMODREFLEX: { C *zc=(C*)zv;       XMVP(T,TH,hash,exp,stride,1);                ZCSHAPE; }   break;  \
   case ILESS: { C *zc=(C*)zv; XMVP(T,TH,hash,exp,stride,0);                ZCSHAPE; }   break;  \
   case IINTER: { C *zc=(C*)zv; XMVPI(T,TH,hash,exp,stride,0);                ZCSHAPE; }   break;  \
   case INUBI|IIMODREFLEX: {I *zi=zv;  XDOP(T,TH,hash,exp,stride,{},{*zi++=i;},1) ZISHAPE; }   break;  \
   case IEPS: { B *zb=(B*)zv;  XDOP(T,TH,hash,exp,stride,{*zb++=1;},{*zb++=0;},0) zv=(I*)zb;} /* this has IRS, so zv must be kept right */                       break;  \
    /* the rest are f@:e., none of which have IRS */ \
   case II0EPS: { I s; XDOP(T,TH,hash,exp,stride,{},{s=i; goto exit0;},0); s=wsct; exit0: *zv=s; }   break; /* i.&0@:e. */   \
   case II1EPS: { I s; XDOP(T,TH,hash,exp,stride,{s=i; goto exit1;},{},0); s=wsct; exit1: *zv=s; }  break; /* i.&1@:e. */  \
   case IJ0EPS: { I s; XDQP(T,TH,hash,exp,stride,{},{s=i; goto exit2;},0); s=wsct; exit2: *zv=s; }   break; /* i:&0@:e. */  \
   case IJ1EPS: { I s; XDQP(T,TH,hash,exp,stride,{s=i; goto exit3;},{},0); s=wsct; exit3: *zv=s; }   break; /* i:&1@:e. */  \
   case ISUMEPS: { I s=0; XDOP(T,TH,hash,exp,stride,{++s;},{},0); *zv=s; }   break; /* +/@:e. */  \
   case IANYEPS: { B s; XDOP(T,TH,hash,exp,stride,{s=1; goto exit5;},{},0); s=0; exit5: *(B*)zv=s; } break; /* +./@:e. */  \
   case IALLEPS: { B s; XDOP(T,TH,hash,exp,stride,{},{s=0; goto exit6;},0); s=1; exit6: *(B*)zv=s; } break; /* *./@:e. */  \
   case IIFBEPS: { I *zi=zv; XDOP(T,TH,hash,exp,stride,{*zi++=i;},{},0); ZISHAPE; }   break; /* I.@:e. */  \
   } \
  }                                                                                 \
  R h;                                                                               \
 }

// compare floats, not distinguishing -0 from +0.  Return 0 if equal, 1 if not equal
static __forceinline I fcmp0(D* a, D* w, I n){
 DQ(n, if(a[i]!=w[i])R 1;);
 R 0;
}
#if C_AVX2 || EMU_AVX2
#define COMPSETUP \
 __m256i endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-n)&(NPAR-1))));  // mask for 0 1 2 3 4 5 is xxxx 0001 0011 0111 1111 0001
#define COMPCALL(a) icmpeq(v,(a)+n*hj,n,endmask)
static __forceinline I icmpeq(I *x, I *y, I n, __m256i endmask) {
 __m256i u,v;
 I i=(n-1)>>LGNPAR;  /* # loops for 0 1 2 3 4 5 is x 0 0 0 0 1 */
 while(--i>=0){
  u=_mm256_loadu_si256 ((__m256i*)x); v=_mm256_loadu_si256 ((__m256i*)y); if(~_mm256_movemask_epi8(_mm256_cmpeq_epi8(u,v)))R 1; x+=NPAR; y+=NPAR;
  }
 u=_mm256_maskload_epi64(x,endmask); v=_mm256_maskload_epi64(y,endmask); 
 R 0!=~_mm256_movemask_epi8(_mm256_cmpeq_epi8(u,v));  // no miscompares, compare equal
}
#else
#define COMPSETUP
#define COMPCALL(a) icmpeq(v,(a)+n*hj,n)
// Return nonzero if *a!=*w
static __forceinline I icmpeq(I *a, I *w, I n) {
 if(n)do{
  if(*a!=*w)R n;
  if(--n)++a,++w; else R n;
 }while(1);
 R n;
}
#endif

// jtioa* BOX
// jtiox  XNUM
// jtioq  RAT
// jtioi1 k==SZI, INT/SBT/char/bool not small-range
// jtioi  list of >1 INT
// jtioc  k!=SZI, bool (must be list of em)/char/INT/SBT
// jtioc01 intolerant FL atom
// jtioc0 intolerant FL array
// jtioz01 intolerant CMPX atom
// jtioz0 intolerant CMPX array

static IOFX(A,US,jtioax1,,cthia(~0LL,1.0,C(*v)),!equ(C(*v),C(av[hj])),1  )  /* boxed exact 1-element item */   
static IOFX(A,US,jtioau,, hiau(C(*v)),  !equ(C(*v),C(av[hj])),1  )  /* boxed uniform type         */
static IOFX(X,US,jtiox,,  hix(v),            !eqx(n,v,av+n*hj),               cn)  /* extended integer           */   
static IOFX(Q,US,jtioq,,  hiq(v),            !eqq(n,v,av+n*hj),               cn)  /* rational number            */   
static IOFX(C,US,jtioc,,  hic(k,(UC*)v),     memcmpne(v,av+k*hj,k),             cn)  /* boolean, char, or integer  */
static IOFX(I,US,jtioi,COMPSETUP,  hici(n,v),            COMPCALL(av),          cn  )  // INT array, not float
static IOFX(I,US,jtioi1,,  hici1(v),           *v!=av[hj],                    1 )  // len=8, not float
static IOFX(D,US,jtioc01,, hic01((UIL*)v),    *v!=av[hj],                      1) // float atom
static IOFX(Z,US,jtioz01,, hic0(2,(UIL*)v),    (v[0].re!=av[hj].re)||(v[0].im!=av[hj].im), 1) // complex atom
static IOFX(D,US,jtioc0,, hic0(n,(UIL*)v),    fcmp0(v,&av[n*hj],n),           cn) // float array
static IOFX(Z,US,jtioz0,, hic0(2*n,(UIL*)v),    fcmp0((D*)v,(D*)&av[n*hj],2*n),  cn) // complex array

static IOFX(A,UI4,jtioax12,,cthia(~0LL,1.0,C(*v)),!equ(C(*v),C(av[hj])),1  )  /* boxed exact 1-element item */   
static IOFX(A,UI4,jtioau2,, hiau(C(*v)),  !equ(C(*v),C(av[hj])),1  )  /* boxed uniform type         */
static IOFX(X,UI4,jtiox2,,  hix(v),            !eqx(n,v,av+n*hj),               cn)  /* extended integer           */   
static IOFX(Q,UI4,jtioq2,,  hiq(v),            !eqq(n,v,av+n*hj),               cn)  /* rational number            */   
static IOFX(C,UI4,jtioc2,,  hic(k,(UC*)v),     memcmpne(v,av+k*hj,k),             cn)  /* boolean, char, or integer  */
static IOFX(I,UI4,jtioi2,COMPSETUP,  hici(n,v),            COMPCALL(av),          cn  )  // INT array, not float
static IOFX(I,UI4,jtioi12,,  hici1(v),           *v!=av[hj],                    1 )  // len=8, not float
static IOFX(D,UI4,jtioc012,, hic01((UIL*)v),    *v!=av[hj],                      1) // float atom
static IOFX(Z,UI4,jtioz012,, hic0(2,(UIL*)v),    (v[0].re!=av[hj].re)||(v[0].im!=av[hj].im), 1) // complex atom
static IOFX(D,UI4,jtioc02,, hic0(n,(UIL*)v),    fcmp0(v,&av[n*hj],n),           cn) // float array
static IOFX(Z,UI4,jtioz02,, hic0(2*n,(UIL*)v),    fcmp0((D*)v,(D*)&av[n*hj],2*n),  cn) // complex array


// ********************* second class: tolerant comparisons, possibly boxed **********************

// create tolerant hash for a single D
// Note: the masking may cause a nonzero to hash to negative zero.  This is OK, because any nonzero will not be
// tequal to +0 or -0.  But we must ensure that -0 hashes to the same value as +0, since those two numbers are equal.
#define HIDMSK(v) (likely(*(UIL*)v!=(UIL)NEGATIVE0)?CRC32LL(-1L,*(UIL*)v&ctmask):CRC32LL(-1L,0))
// save the mask result in m
#define HIDMSKSV(m,v) ((m=*(UIL*)v&ctmask), (likely(*(UIL*)v!=(UIL)NEGATIVE0)?CRC32LL(-1L,m):CRC32LL(-1L,0)) )
// save the unmasked D value (as bits) in m
#define HIDUMSKSV(m,v) ((m=*(UIL*)v), (likely(m!=(UIL)NEGATIVE0)?CRC32LL(-1L,m&ctmask):CRC32LL(-1L,0)) )
// create hash for a D type
#define HID(y)              CRC32LL(-1L,(y))
#define MASK(dd,xx)         {D dv=(xx); if(likely(*(UIL*)&dv!=(UIL)NEGATIVE0)){dd=*(UIL*)&dv&ctmask;}else{dd=0;} }

// FIND for read.  Stop loop if endtest is true; execute fstmt if match ((exp) is false).  hj holds the index of the value being tested
#define FINDRD(exp,hindex,endtest,fstmt) do{hj=hv[hindex]; if(endtest)break;if(!(exp)){fstmt break;}if(unlikely(--hindex<0))hindex+=p;}while(1);

// FIND for write.  j is the scan pointer through the hashtable, and has been initialized to the starting bucket
// store i into the hashtable if not found.  The test should be intolerant except for NUB/KEY operation
#define FINDWR(TH,exp) NOUNROLL do{if(asct==(hj=hv[j])){hv[j]=(TH)i; break;}if(!(exp))break;if(unlikely(--j<0))j+=p;}while(1);

// functions for building the hash table for tolerant comparison.  expa is the function for detecting matches on a values

// find a tolerant match for *v.  The hashtable has already been created.
// We have to look in two buckets: for the interval containing the value, and for the neighboring interval
// containing tolerantly equal values.  Because the interval is always at least as wide as the
// tolerant diameter (i. e. 2 tolerances wide), only one neighboring interval need be checked.
// For the default tolerance, the interval width is just a smidgen above the diameter for intervals
// near 1.1111; it is twice the diameter for intervals near 1.00000.  This means that a random
// value will have its tolerance circle spilling into the neighboring interval about
// 3/4 of the time.  because of that, we don't bother to check whether the circle actually
// does overlap [that would require calculating the upper and lower bounds and then doing bit fiddling on the result];
// we simply probe in the neighboring interval closer to the given value.
// We stack up as much calculation as possible before the searches to reduce the cost of a misprediction
//
// We calculate a half-interval above and below the value; one of those is in the neighboring interval,
// the other is in the same interval; we XOR to preserve the neighbor
//
// At the end of this calculation il contains the index of a match, or asct if no match.

// set dx to the 'other' interval from *v.
#define SETXVAL  xval=_mm256_castpd256_pd128(_mm256_broadcast_sd((D*)v)); xnew=_mm_mul_pd(xval,tltr); xrot=_mm_permute_pd(xnew,0x1); xnew=_mm_xor_pd(xnew,xval); xnew=_mm_xor_pd(xnew,xrot); dx=_mm_extract_epi64(_mm_castpd_si128(xnew),0);
#define TFINDXYT(TH,expa,expw,fstmt0,endtest1,fstmt1)  \
 {UIL dx; x=*(D*)v;                                                                            \
  HASHSLOT(HIDUMSKSV(dx,v)) jx=j; \
  SETXVAL \
  HASHSLOT(HID(dx&=ctmask)) FINDRD(expw,jx,asct==hj,fstmt0); il=hj; \
  FINDRD(expw,j,endtest1,fstmt1); \
 }
// Same idea, but used for reflexives, where the table has not been built yet.  We save replicating the hash calculation, and also
// we know that there will be a match in the first search, which simplifies that search.
// For this routine expa MUST be an intolerant comparison
#define TFINDY1T(TH,expa,expw,fstmt0,endtest1,fstmt1)  \
 {UIL dx; x=*(D*)v;                                                                             \
  HASHSLOT(HIDUMSKSV(dx,v)) jx=j; \
  SETXVAL \
  FINDWR(TH,expa);  \
  HASHSLOT(HID(dx&=ctmask)) FINDRD(expw,jx,0,fstmt0); il=hj; \
  FINDRD(expw,j,endtest1,fstmt1); \
 }
// This version for nub/key.  We read first, and add to the table only if the value was not found
#define TFINDY1TKEY(TH,expa,expw,fstmt0,endtest1,fstmt1)  \
 {UIL dx; x=*(D*)v;                                                                             \
  HASHSLOT(HIDUMSKSV(dx,v)) jx=j; /* j=jx=main interval */ \
  SETXVAL \
  FINDRD(expw,jx,asct==hj,fstmt0); il=hj; /* read main interval */ \
  HASHSLOT(HID(dx&=ctmask)) /* j=other interval*/ \
  FINDRD(expw,j,endtest1,fstmt1); /* read other interval, set il to place found if any */ \
  if(il>=i){j=jx; FINDWR(TH,expa); il=i;} /* if new value, add to hash */ \
 }


// here comparing boxes.  Consider modifying hia to take a self/neighbor flag rather than a tolerance
#define TFINDBX(TH,expa,expw,fstmt0,endtest1,fstmt1)   \
 {HASHSLOT(cthia(ctmask,tl,C(*v))) jx=j; FINDRD(expw,j,asct==hj,fstmt0); il=hj;   \
 HASHSLOT(cthia(ctmask,tr,C(*v))) if(j!=jx){FINDRD(expw,j,endtest1,fstmt1);}  \
 }
// reflexive.  Because the compare for expa does not force intolerance, we must do so here.  This is required only for boxes, since the other expas are intolerant
#define TFINDBY(TH,expa,expw,fstmt0,endtest1,fstmt1)   \
 {HASHSLOT(cthia(ctmask,1.0,C(*v)))  PUSHCCT(1.0) FINDWR(TH,expa); POPCCT TFINDBX(TH,expa,expw,fstmt0,endtest1,fstmt1)}

// reflexive for nub/key.  First search the table, then insert only if no match found.
// Each FINDRD sets hj, leaving il and hj at the end; set il to be the lower.
// if not found, set il=i (that is, match on the newly-added element), and add that element
#define TFINDBYKEY(TH,expa,expw,fstmt0,endtest1,fstmt1)   \
 {HASHSLOT(cthia(ctmask,tl,C(*v))) jx=j; FINDRD(expw,j,asct==hj,fstmt0); il=hj;   \
 HASHSLOT(cthia(ctmask,tr,C(*v))) if(j!=jx){FINDRD(expw,j,endtest1,fstmt1); }  \
 if(il>=i){HASHSLOT(cthia(ctmask,1.0,C(*v)))  FINDWR(TH,expa); il=i;} \
 }

// ******* macros to process each item of w *******
// FXY is a TFIND macro, charged with setting il.
// Set il, execute the statement, advance to the next item
#define TDOXY(T,TH,FXY,expa,expw,fstmt0,endtest1,fstmt1,stmt)  \
 DO(wsct, FXY(TH,expa,expw,fstmt0,endtest1,fstmt1); stmt; v=(T*)((C*)v+k);)

// Same, but search from the end of y backwards (e. i: 0 etc)
#define TDQXY(T,TH,FXY,expa,expw,fstmt0,endtest1,fstmt1,stmt)  \
 v=(T*)((C*)v+k*(wsct-1)); DQ(wsct, FXY(TH,expa,expw,fstmt0,endtest1,fstmt1); stmt; v=(T*)((C*)v-k);)

// Version for ~. y and x -. y .  move the item to *zc++
// If we know the item is a single value we store it always and ratify the value by incrementing the pointer, using conditional instructions
// If the item might be long we move it only if it is valid
// For -.
#define TMVX(T,TH,FXY,expa,expw)   \
  {if(k==sizeof(T)){DQ(wsct, FXY(TH,expa,expw,goto found3;,hj==asct,il=hj;); *(T*)zc=*(T*)v; zc+=(il==asct)*sizeof(T); found3: v=(T*)((C*)v+k); );  \
            }else{DQ(wsct, FXY(TH,expa,expw,goto found2;,hj==asct,goto found2;); {MC(zc,v,k); zc+=k;}; found2: v=(T*)((C*)v+k); );}  \
 }
// For ([ -. -.)
#define TMVXI(T,TH,FXY,expa,expw)   \
  {if(k==sizeof(T)){DQ(wsct, FXY(TH,expa,expw,{},hj==asct,il=hj;); *(T*)zc=*(T*)v; zc+=(il!=asct)*sizeof(T); v=(T*)((C*)v+k); );  \
            }else{DQ(wsct, FXY(TH,expa,expw,{},hj==asct,il=hj;); if(il!=asct){MC(zc,v,k); zc+=k;}; v=(T*)((C*)v+k); );}  \
 }
// for ~.  Same idea, but reflexive.  FYY is always TFINDBYKEY
#define TMVY(T,TH,FYY,expa,expw)   \
  {if(k==sizeof(T)){DO(wsct, FYY(TH,expa,expw,{},hj==asct,il=hj;); *(T*)zc=*(T*)v; zc+=(i==il)*sizeof(T); v=(T*)((C*)v+k); );     \
             }else{DO(wsct, FYY(TH,expa,expw,{},hj==asct,il=hj;); if(il==i){MC(zc,v,k); zc+=k;}; v=(T*)((C*)v+k); );}   \
 }

#define SETXNEW  __m128d tltr; tltr=_mm_set_pd(tl,tr); xnew=xrot=xval=_mm_setzero_pd();
// Do the operation.  Build a hash for a except when self-index
// FXY is a TFIND macro, charged with setting il.
// FYY similar, for reflexive searches
// FYYKEY similar for nub/key - they add to the hash only if noun found
// expa - test for matches during hashing.  expa is true for NO match.  n v av hj are available as parameters
// expw - test for matches during lookup, i. e. reading hashtable.  expw is true for NO match.  n v av hj are available as parameters
#define IOFT(T,TH,f,hash,FXY,FYY,FYYKEY,expa,expw)   \
 IOF(f){I acn=ak/sizeof(T),  \
        wcn=wk/sizeof(T),* zv=AV(z);T* RESTRICT av=(T*)AV(a),* wv=(T*)AV(w);I md; \
        D tl=jt->cct,tr=1/tl;I il,jx; D x=0.0;  /* =0.0 to stifle warning */    \
        IH *hh=IHAV(h); I p=hh->datarange; TH * RESTRICT hv=hh->data.TH; UIL ctmask=calcctmask(jt->cct);   \
  __m128i vp, vpstride;   /* v for hash/v for search; stride for each */ \
  _mm256_zeroupperx(VOIDARG)  \
  __m128d xval, xnew, xrot; SETXNEW \
  vp=_mm_setzero_si128();  /* to avoid warnings */ \
  md=mode&IIOPMSK;   /* clear upper flags including REFLEX bit */                            \
  if(!(((uintptr_t)a^(uintptr_t)w)|(ac^wc)))md|=(IIMODREFLEX&((((1<<IIDOT)|(1<<IICO)|(1<<INUBSV)|(1<<INUB)|(1<<INUBI)|(1<<IFORKEY))<<IIMODREFLEXX)>>md));  /* remember if this is reflexive, which doesn't prehash */  \
  jx=0;                                                                     \
  I wrepssv=0; if(1==wc){wrepssv=ac;}else if(unlikely(ac>wc))wrepssv=(ac/wc)-1; I wreps=wrepssv; if(w==mark){wsct=0;} \
  for(;ac>0;av+=acn,--ac,wv+=--wreps<0?wcn:0,wreps=wreps<0?wrepssv:wreps){                   \
   if(!(mode&IPHOFFSET)){  /* if we are not using a prehashed table */                                        \
    hashallo(hh,p,asct,mode);                                                           \
    if(!(IIMODREFLEX&md)){I cn= k/sizeof(T);  /* not reflexive */                                            \
     PUSHCCT(1.0) if(md==IICO)XDQAP(T,TH,hash,expa,cn) else XDOAP(T,TH,hash,expa,cn); POPCCT  /* all writes to hash must use intolerant compare */                \
     if(w==mark)break;                                                                \
    } \
   }                                                                                            \
   switch(md){                                                                                   \
   /* when we are searching up, we can stop the second search when it gets past the index found in the first search */ \
   case IIDOT: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,*zi++=il;); zv=zi; } break;  \
   case IIDOT|IIMODREFLEX: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; TDOXY(T,TH,FYY,expa,expw,{},hj>=il,il=hj;,*zi++=il;); zv=zi; } break;  \
   case IFORKEY|IIMODREFLEX: {T * RESTRICT v=wv; I nuniq=0; I j, hj; I * RESTRICT zi=zv; TDOXY(T,TH,FYYKEY,expw,expw,{},hj>=il,il=hj;,nuniq+=i==il;*zi=il;zv[il]++;++zi;); AM(h)=nuniq; zv=zi; } break;  \
   case IICO:  {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi; zi=zv+=wsct; TDQXY(T,TH,FXY,expa,expw,{},hj==asct,il=il==asct?hj:il;il=hj>il?hj:il;,*--zi=il;);} break;  \
   case IICO|IIMODREFLEX:  {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi; zi=zv+=wsct; TDQXY(T,TH,FYY,expa,expw,{},hj==asct,il=il==asct?hj:il;il=hj>il?hj:il;,*--zi=il;);} break;  \
   case INUBSV|IIMODREFLEX:{T * RESTRICT v=wv; I j, hj; B * RESTRICT zb=(B*)zv; TDOXY(T,TH,FYYKEY,expw,expw,{},hj>=il,il=hj;,*zb++=i==il;); zv=(I*)zb;} break;  /* zv must keep running */  \
   case INUB|IIMODREFLEX:  {T * RESTRICT v=wv; I j, hj; D * RESTRICT zd=(D*)zv; C * RESTRICT zc=(C*)zv; TMVY(T,TH,FYYKEY,expw,expw); ZCSHAPE; }    break;  \
   case ILESS: {T * RESTRICT v=wv; I j, hj; D * RESTRICT zd=(D*)zv; C * RESTRICT zc=(C*)zv; TMVX(T,TH,FXY,expa,expw); ZCSHAPE; }    break;  \
   case IINTER: {T * RESTRICT v=wv; I j, hj; D * RESTRICT zd=(D*)zv; C * RESTRICT zc=(C*)zv; TMVXI(T,TH,FXY,expa,expw); ZCSHAPE; }    break;  \
   case INUBI|IIMODREFLEX: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; TDOXY(T,TH,FYYKEY,expw,expw,{},hj>=il,il=hj;,*zi=i; zi+=(i==il);); ZISHAPE;} break;  \
   case IEPS:  {T * RESTRICT v=wv; I j, hj; B * RESTRICT zb=(B*)zv; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,*zb++=(il!=asct);); zv=(I*)zb;} break;   /* zv must keep running */ \
   case II0EPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; I s=wsct; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct==il){s=i; break;}); *zi=s;} break;  \
   case II1EPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; I s=wsct; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct!=il){s=i; break;}); *zi=s;} break;  \
   case IJ0EPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; I s=wsct; TDQXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct==il){s=i; break;}); *zi=s;} break;  \
   case IJ1EPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; I s=wsct; TDQXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct!=il){s=i; break;}); *zi=s;} break;  \
   case ISUMEPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; I s=0; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,s+=(asct>il);); *zi=s;}  break;  \
   case IANYEPS: {T * RESTRICT v=wv; I j, hj; B * RESTRICT zb=(B*)zv; I s=0; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct>il){s=1; break;}); *zb=(B)s;} break;  \
   case IALLEPS: {T * RESTRICT v=wv; I j, hj; B * RESTRICT zb=(B*)zv; I s=1; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,if(asct==il){s=0; break;}); *zb=(B)s;} break;  \
   case IIFBEPS: {T * RESTRICT v=wv; I j, hj; I * RESTRICT zi=zv; TDOXY(T,TH,FXY,expa,expw,{},hj>=il,il=hj;,*zi=i; zi+=(asct>il);); ZISHAPE;} break;  \
   } \
  }                                                                                             \
  R h;                                                                                           \
 }

// Verbs for the types of inputs
// jtiod  tolerant FL
// jtiod1 tolerant FL atom
// jtioz  tolerant CMPX
// jtoiz1 tolerant CMPX atom

// hashes using the 2-byte hashtable
// CMPLX array
static IOFT(Z,US,jtioz, HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),n*2), !eqz(n,v,av+n*hj)               )
// CMPLX atom
static IOFT(Z,US,jtioz1,HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),  2), !zeq( *v,av[hj] )               )
// FL array
static IOFT(D,US,jtiod, HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0(v,av+n*hj,n  ), !jeqd(n,v,av+n*hj,tl)               )
// FL atom
static IOFT(D,US,jtiod1,HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,*v!=av[hj],                       !TCMPEQ(tl,x,av[hj] )                 )
// boxed array with more than 1 box
static IOFT(A,US,jtioa, cthia(ctmask,1.0,C(*v)),TFINDBX,TFINDBY,TFINDBYKEY,!eqa(n,v,av+n*hj),          !eqa(n,v,av+n*hj)          )
// singleton box
static IOFT(A,US,jtioa1,cthia(ctmask,1.0,C(*v)),TFINDBX,TFINDBY,TFINDBYKEY,!equ(C(*v),C(av[hj])),!equ(C(*v),C(av[hj])))

// hashes using the 4-byte hashtable
// CMPLX array
static IOFT(Z,UI4,jtioz2, HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),n*2), !eqz(n,v,av+n*hj)               )
// CMPLX atom
static IOFT(Z,UI4,jtioz12,HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0((D*)v,(D*)(av+n*hj),  2), !zeq( *v,av[hj] )               )
// FL array
static IOFT(D,UI4,jtiod2, HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,fcmp0(v,av+n*hj,n  ), !jeqd(n,v,av+n*hj,tl)               )
// FL atom
static IOFT(D,UI4,jtiod12,HIDMSK(v), TFINDXYT,TFINDY1T,TFINDY1TKEY,*v!=av[hj],                       !TCMPEQ(tl,x,av[hj] )                 )
// boxed array with more than 1 box
static IOFT(A,UI4,jtioa2, cthia(ctmask,1.0,C(*v)),TFINDBX,TFINDBY,TFINDBYKEY,!eqa(n,v,av+n*hj),          !eqa(n,v,av+n*hj)          )
// singleton box
static IOFT(A,UI4,jtioa12,cthia(ctmask,1.0,C(*v)),TFINDBX,TFINDBY,TFINDBYKEY,!equ(C(*v),C(av[hj])),!equ(C(*v),C(av[hj])))

// ********************* third class: small-range arguments ****************************

// b is a bit index; BYTENO is byte number, BITNO is bit within byte
#define BYTENO(b) ((b)>>3)
#define BITNO(b) ((b)&7)

// nub support, which creates & processes the bit-vector in one pass.  FULL is immaterial.
// T is the type for the result vector, stmt creates the result
// unpacked version expects default of 1
#define SNUB(decl, stmt) I zi=0, zie=asct;  decl  UC* RESTRICT hu=hh->data.UC-min; while(zi!=zie){UC v=hu[wv[zi]]; hu[wv[zi]]=0;  stmt  ++zi;}
// packed version.  Default of 0
#define SNUBP(decl, stmt) I zi=0, zie=asct;  decl  UC* RESTRICT hu=hh->data.UC-BYTENO(min); while(zi!=zie){UC v=hu[BYTENO(wv[zi])]; hu[BYTENO(wv[zi])]|=1<<BITNO(wv[zi]); v >>=BITNO(wv[zi]); v&=1; v^=1; stmt  ++zi;}

// creation of the value vector

// The bitmask was cleared to 0/1 by hashalloc depending on function
// Boolean/bit hashtables.  Used  where the position doesn't matter, i. e. for all but i./i: 
// Set TRUE for each value found.  hh->currentlo will always be 0.  zi starts at 0, since values don't matter
// The value in the cell is 0 or 1, since we don't care what the original position was
// We init the table to 0 or 1 depending on the primitive; but we always use 0 for PACKed bits, because it's never right to add
// an instruction to building the table
#define SDO(T,bitdef)  UC* RESTRICT hu=hh->data.UC-min; if(!(mode&IPHOFFSET)){T* RESTRICT mav=av; DO(asct, hu[mav[i]]=1-bitdef;)}
#define SDOP(T)  UC* RESTRICT hu=hh->data.UC-BYTENO(min); if(!(mode&IPHOFFSET)){T* RESTRICT mav=av; DO(asct, hu[BYTENO(mav[i])]|=1<<BITNO(mav[i]);)}  // packed version

// US/UI4 hashtables
// the value in the cell indicates the original input position; the index of the cell indicates the input value
// Go through and remember the index for each value.  Leaves last index, so used for i:.  Leave zi correct at end
#define SDOA(T,Ttype) I zi=hh->currentindexofst, zi0=zi, zie=zi+asct; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; /* biased start,end+1 index, and data pointers */ \
                           if(!(mode&IPHOFFSET)){T* RESTRICT mav=av-zi; while(zi!=zie){hu[mav[zi]]=(Ttype)zi; ++zi;} zi=zi0;}
// No faster version for BASE0
// Go through and remember the index for each value.  Reverse order, so leaves first index, so used for i.
#define SDQA(T,Ttype) I zi=hh->currentindexofst, zi0=zi, zie=zi+asct; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo;  \
                           if(!(mode&IPHOFFSET)){T* RESTRICT mav=av-zi; do{--zie; hu[mav[zie]]=(Ttype)zie;}while(zie!=zi);}
// This version if we know the area starts at 0
#define SDQA0(T,Ttype) I zie=asct-1; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo;  \
                           if(!(mode&IPHOFFSET)){do{hu[av[zie]]=(Ttype)zie;}while(--zie>=0);}

// using the value vector: loop through the items of w, creating the output.
// if FULL is set, there is no need for range-checking the input

// first, versions for i. i:     vv is the value to use for not-found (always asct)
// This version for I values (which have a partial table); use the table only if the value is represented there.  vv is the not-found value.  Install it in default position
// Since the table is not FULL, the range scan of must have aborted; we scan forward from the beginning of w
#define SCOZ(T,Ttype,vv) {I * RESTRICT zv=AV(z)+l*wsct-zi; T* RESTRICT mwv=wv-zi; Ttype def[1]; zie=zi+wsct; def[0]=(Ttype)(vv+zi0); \
                          while(zi!=zie){T v=mwv[zi]; Ttype *hv=hu+v; if(v<min)hv=def; if(v>max)hv=def; I hvv; if((hvv=(I)*hv-zi0)<0)hvv=vv; zv[zi]=(Ttype)hvv; ++zi;}}
// this version is used when the result vector is known to be full (out-of-range not possible).  Omit the out-of-bounds check.  Since the range-scan of w ran to completion,
// the end of w is most likely to be in-cache; so scan backwards
#define SCOZF(T,Ttype,vv)  {I * RESTRICT zv=AV(z)+l*wsct-zi; T* RESTRICT mwv=wv-zi; zie=zi+wsct; while(zi!=zie){--zie; I hvv; if((hvv=(I)hu[mwv[zie]]-zi0)<0)hvv=vv; zv[zie]=(Ttype)hvv;}}
// these versions are used if the table is known to start at offset 0.  Omit the not-found check, since asct was already loaded; and zi is 0
#define SCOZ0(T,Ttype,vv) {I zi = -wsct; I * RESTRICT zv=AV(z)+l*wsct-zi; T* RESTRICT mwv=wv-zi; Ttype def[1]; def[0]=(Ttype)vv; \
                          while(zi){T v=mwv[zi]; Ttype *hv=hu+v; if(v<min)hv=def; if(v>max)hv=def; zv[zi]=*hv; ++zi;}}
#define SCOZF0(T,Ttype,vv) {zie=wsct-1; I * RESTRICT zv=AV(z)+l*wsct; while(zie>=0){zv[zie]=hu[wv[zie]]; --zie;}}  // backwards scan

// for e. -. u@e. ([ -. -.) - for each item of w, see if it is in the value table.  Set v to the value read from the table (1 if in table).
// The table is always allocated as a bit vector and therefore is at offset 0 in the table
// wv[i] is the data value read, if that's needed
// Declare the result vector, and prebias it for our loop if needed.  indexed is 1 if zv will be referred to as zv[i], 0 if by *zv
#define DCLZVO(T,indexed) T * RESTRICT zv=T##AV(z)+(l+indexed)*wsct;
#define SCOW(T,bitdef,stmt)  {UC def[1]; def[0]=bitdef; I i; T *mwv=wv+wsct; for(i=-wsct;i<0;++i){T x=mwv[i]; UC *hv=hu+x; if(x<min)hv=def; if(x>max)hv=def; UC v=*hv; stmt}}
// faster version of SCOW for use when the table contains all possible values
#define SCOWF(T,bitdef,stmt)  {I i; T *mwv=wv+wsct; for(i=-wsct;i<0;++i){UC v=hu[mwv[i]]; stmt}}
// packed version
#define SCOWP(T,bitdef,stmt)  {UC def[1]; def[0]=0; I i; T *mwv=wv+wsct; for(i=-wsct;i<0;++i){T x=mwv[i]; UC *hv=hu+BYTENO(x); if(x<min)hv=def; if(x>max)hv=def; UC v=((*hv>>BITNO(x))&1)^bitdef; stmt}}
// packed full version
#define SCOWPF(T,bitdef,stmt)  {I i; T *mwv=wv+wsct; for(i=-wsct;i<0;++i){T x=mwv[i]; UC *hv=hu+BYTENO(x); UC v=((*hv>>BITNO(x))&1)^bitdef; stmt}}

// just like SCOW/SCOWF but scanning from the end of w
#define DCLZVQ(T,unused) T * RESTRICT zv=T##AV(z)+l*wsct;
#define SCQW(T,bitdef,stmt)  {UC def[1]; def[0]=bitdef; I i; for(i=wsct-1;i>=0;--i){T x=wv[i]; UC *hv=hu+x; if(x<min)hv=def; if(x>max)hv=def; UC v=*hv; stmt}}
#define SCQWF(T,bitdef,stmt)  {I i; for(i=wsct-1;i>=0;--i){UC v=hu[wv[i]]; stmt}}
#define SCQWP(T,bitdef,stmt)  {UC def[1]; def[0]=0; I i; for(i=wsct-1;i>=0;--i){T x=wv[i]; UC *hv=hu+BYTENO(x); if(x<min)hv=def; if(x>max)hv=def; UC v=(*hv>>BITNO(x))&1; stmt}}
#define SCQWPF(T,bitdef,stmt)  {I i; for(i=wsct-1;i>=0;--i){T x=wv[i]; UC *hv=hu+BYTENO(x); UC v=((*hv>>BITNO(x))&1)^bitdef; stmt}}

// Create basic/FULL/PACK/FULL+PACK versions (used for all boolean maps)
#define SMCASE0(casename, text) case casename: text break;
#define SMCASEF(casename, text) case IIMODFULL+casename: text break;
#define SMCASEP(casename, text) case IIMODPACK+casename: text break;
#define SMCASE1(casename, text) case IIMODFULL+IIMODPACK+casename: text break;
// action here is (parms to SCO*) cleanup  - the cleanup comes after the SCO* loop
#define SMFULLPACK(T,bitdef,casename,decl,action) SMCASE0(casename, {SDO(T,bitdef) decl SCOW action}) SMCASEF(casename, {SDO(T,bitdef) decl SCOWF action}) SMCASEP(casename, {SDOP(T) decl SCOWP action}) SMCASE1(casename, {SDOP(T) decl SCOWPF action})
#define SMFULLPACQ(T,bitdef,casename,decl,action) SMCASE0(casename, {SDO(T,bitdef) decl SCQW action}) SMCASEF(casename, {SDO(T,bitdef) decl SCQWF action}) SMCASEP(casename, {SDOP(T) decl SCQWP action}) SMCASE1(casename, {SDOP(T) decl SCQWPF action})
// EPS builds a full-size result and thus can scan FULLs in either order; we choose backwards
#define SMFULLPACKEPS(T,bitdef,casename,action) SMCASE0(casename, {SDO(T,bitdef) DCLZVO(B,1) SCOW action}) SMCASEF(casename, {SDO(T,bitdef) DCLZVQ(B,1) SCQWF action}) SMCASEP(casename, {SDOP(T) DCLZVO(B,1) SCOWP action}) SMCASE1(casename, {SDOP(T) DCLZVQ(B,1) SCQWPF action})

// Do the operation on small-range arguments
// COZ1 is the result loop for i. i:  we will have to have a bit version for e.
// COW is the result loop for (e. i. 1:)  ([: +./ e.) ([: +/ e.) ([: I. e.) (e. i. 0:) ([: *./ e.) -.
// CQW is the result loop for (e. i: 1:) (e. i: 0:)
// cm is the number of cells of w per cell of a 
#define IOFSMALLRANGE(f,T,Ttype)    \
 IOF(f){IH *hh=IHAV(h);I l;T* RESTRICT av,* RESTRICT wv;T max,min; UI p; \
  mode|=((mode&(IIOPMSK&~(IIDOT^IICO)))|((I)a^(I)w)|(ac^wc))?0:IIMODREFLEX; \
  _mm256_zeroupperx(VOIDARG)  \
  av=(T*)AV(a); wv=(T*)AV(w); \
  min=(T)hh->datamin; p=hh->datarange; max=min+(T)p-1; \
  I wrepssv=0; if(1==wc){wrepssv=ac;}else if(unlikely(ac>wc))wrepssv=(ac/wc)-1; I wreps=wrepssv; if(w==mark){wsct=0;} \
  for(l=0;l<ac;++l,av+=asct,wv+=--wreps<0?wsct:0,wreps=wreps<0?wrepssv:wreps){ \
   if(!(mode&(IPHOFFSET))){mode = hashallo(hh,p,asct,mode);}  /* clear table & set parms for this loop - only if not using prehashed table, which always start at offset 0 */ \
   switch(mode&(IIOPMSK|IIMODFULL|IIMODPACK|IIMODBASE0)){ /* We know the setting of IIMODBITS without looking */ \
   default: ASSERTSYS(0,"switch failure in i."); \
     /* a lot of the l*wsct in the cases below could be removed because the verbs lack IRS, but we're leaving them in for now */ \
     /* reflexives, which include nubs, do not require a FULL version.  i.~ and i:~ don't need PACK versions either, since they deal in full indexes only */ \
   case IIMODREFLEX+IIMODFULL+IIDOT: \
   case IIMODREFLEX+IIDOT: \
     {I zi=hh->currentindexofst, zi0=zi, zie=zi+asct;  I * RESTRICT zv=AV(z)+l*asct-zi; T* RESTRICT mwv=wv-zi; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; /* biased start,end+1 index, and data pointers */ \
      while(zi!=zie){I vv = hu[mwv[zi]]; vv=(vv<zi0)?zi:vv; hu[mwv[zi]]=(Ttype)vv; zv[zi]=vv-zi0;  ++zi;} } break; /* scan sequentially; if prev value present, rewrite it, otherwise write index */ \
   case IIMODBASE0+IIMODREFLEX+IIMODFULL+IIDOT: \
   case IIMODBASE0+IIMODREFLEX+IIDOT: \
     {I zi=0;  I * RESTRICT zv=AV(z)+l*asct; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; /* biased start,end+1 index, and data pointers */ \
      while(zi!=asct){I vv = hu[wv[zi]]; vv=(vv==asct)?zi:vv; hu[wv[zi]]=(Ttype)vv; zv[zi]=vv;  ++zi;} } break; /* scan sequentially; if prev value present, rewrite it, otherwise write index */ \
   case IIMODREFLEX+IIMODFULL+IICO: \
   case IIMODREFLEX+IICO: \
     {I zi=hh->currentindexofst, zi0=zi, zie=zi+asct;  I * RESTRICT zv=AV(z)+l*asct-zi; T* RESTRICT mwv=wv-zi; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; \
      do{--zie; I vv = hu[mwv[zie]]; vv=(vv<zi0)?zie:vv; hu[mwv[zie]]=(Ttype)vv; zv[zie]=vv-zi0;}while(zie!=zi); } break; /* same in reverse */ \
   case IIMODBASE0+IIMODREFLEX+IIMODFULL+IICO: \
   case IIMODBASE0+IIMODREFLEX+IICO: \
     {I zie=asct-1;  I * RESTRICT zv=AV(z)+l*asct; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; \
      do{I vv = hu[wv[zie]]; vv=(vv==asct)?zie:vv; hu[wv[zie]]=(Ttype)vv; zv[zie]=vv; --zie; }while(zie>=0); } break; /* same in reverse */ \
 \
    /* NUB types use Boolean masks but do not depend on FULL, since they cannot miss.  But they must support PACK. */ \
   case IIMODFULL+INUBSV: \
   case INUBSV:            {SNUB (B * RESTRICT zv=BAV(z)+l*asct;  ,  zv[zi]=v;) } break; \
   case IIMODPACK+IIMODFULL+INUBSV: \
   case IIMODPACK+INUBSV:  {SNUBP(B * RESTRICT zv=BAV(z)+l*asct;  ,  zv[zi]=v;) } break; \
   case IIMODFULL+INUB: \
   case INUB:              {SNUB (T * RESTRICT zv=(T*)AV(z); T *zv0=zv;   ,  *zv=wv[zi]; zv+=v;) AS(z)[0]=zv-zv0; AN(z)=n*(zv-zv0); } break;  \
   case IIMODPACK+IIMODFULL+INUB: \
   case IIMODPACK+INUB:    {SNUBP(T * RESTRICT zv=(T*)AV(z); T *zv0=zv;   ,  *zv=wv[zi]; zv+=v;) AS(z)[0]=zv-zv0; AN(z)=n*(zv-zv0); } break;  \
   case IIMODFULL+INUBI: \
   case INUBI:             {SNUB (I * RESTRICT zv=AV(z); I *zv0=zv;   ,   *zv=zi; zv+=v;) AS(z)[0]=AN(z)=zv-zv0; } break;  \
   case IIMODPACK+IIMODFULL+INUBI: \
   case IIMODPACK+INUBI:   {SNUBP(I * RESTRICT zv=AV(z); I *zv0=zv;   ,   *zv=zi; zv+=v;) AS(z)[0]=AN(z)=zv-zv0; } break;  \
     /* non-reflexives can benefit from FULL checking.  IIDOT and IICO need full indexes (and thus use BASE0, but no PACK); everything else is Boolean */\
   case IIDOT:             {SDQA(T,Ttype);  SCOZ(T,Ttype,asct);} break;  \
   case IIMODFULL+IIDOT:   {SDQA(T,Ttype);  SCOZF(T,Ttype,asct);} break;  \
   case IIMODBASE0+IIDOT:  {SDQA0(T,Ttype);  SCOZ0(T,Ttype,asct);} break;  \
   case IIMODBASE0+IIMODFULL+IIDOT: {SDQA0(T,Ttype);  SCOZF0(T,Ttype,asct);} break;  \
   case IICO:              {SDOA(T,Ttype);  SCOZ(T,Ttype,asct);} break;  \
   case IIMODFULL+IICO:    {SDOA(T,Ttype); SCOZF(T,Ttype,asct);} break;  \
   case IIMODBASE0+IICO:   {SDOA(T,Ttype);  SCOZ0(T,Ttype,asct);} break;  \
   case IIMODBASE0+IIMODFULL+IICO: {SDOA(T,Ttype); SCOZF0(T,Ttype,asct);} break;  \
    /* Boolean indexes from here on.  These must support FULL and PACK.  bitdef appears twice */ \
   SMFULLPACKEPS(T,0,IEPS,  (T,0,zv[i]=v;)) /* EPS scans FULL args backwards for cache coherence */ \
   SMFULLPACK(T,1,ILESS,  DCLZVO(T,0) T *zv0=zv; , (T,1,*zv=mwv[i]; zv+=v;); AS(z)[0]= zv-zv0; AN(z)=n*(zv-zv0);)  \
   SMFULLPACK(T,0,IINTER,  DCLZVO(T,0) T *zv0=zv; , (T,0,*zv=mwv[i]; zv+=v;); AS(z)[0]= zv-zv0; AN(z)=n*(zv-zv0);)  \
   SMFULLPACK(T,0,II0EPS, DCLZVO(I,0) I s=wsct; , (T,0,if(!v){s+=i; break;});*zv++=s;)  \
   SMFULLPACK(T,0,II1EPS, DCLZVO(I,0) I s=wsct; , (T,0,if(v){s+=i; break;});*zv++=s; )  \
   SMFULLPACQ(T,0,IJ0EPS, DCLZVQ(I,0) I s=wsct; , (T,0,if(!v){s=i; break;});*zv++=s;)  \
   SMFULLPACQ(T,0,IJ1EPS, DCLZVQ(I,0) I s=wsct; , (T,0,if(v){s=i; break;}); *zv++=s;)  \
   SMFULLPACK(T,0,IANYEPS,DCLZVO(B,0) B s=0; , (T,0,if(v){s=1; break;}); *zv++=s;)  \
   SMFULLPACK(T,0,IALLEPS,DCLZVO(B,0) B s=1; , (T,0,if(!v){s=0; break;}); *zv++=s;)  \
   SMFULLPACK(T,0,ISUMEPS,DCLZVO(I,0) I s=0; , (T,0,s+=v;); *zv++=s;)  \
   SMFULLPACK(T,0,IIFBEPS,DCLZVO(I,0) I *zv0=zv; , (T,0,*zv=i+wsct; zv+=v;); AS(z)[0]=AN(z)=zv-zv0;)  \
   }  \
  }  \
  R h; \
 }

// The verbs to do the work, for different item lengths and hashtable sizes
static IOFSMALLRANGE(jtio12,UC,US)  static IOFSMALLRANGE(jtio14,UC,UI4)  // 1-byte items, using small/large hashtable
static IOFSMALLRANGE(jtio22,US,US)  static IOFSMALLRANGE(jtio24,US,UI4)  // 2-byte items, using small/large hashtable
static IOFSMALLRANGE(jtio42,I,US)  static IOFSMALLRANGE(jtio44,I,UI4)  // 4/8-byte items, using small/large hashtable

// ******************* fourth class: sequential comparison ***************************************
// implemented only for i. i: e. u/.   - perhaps should revert for other compounds


// fz=bit0 = commutative, bit1 set if incomplete y must be filled with 0 (to avoid isub oflo), bit2 set if incomplete x must be filled with i (for fdiv NaN),
// bit3 set for int-to-float on x, bit4 for int-to-float on y
// bit5 set to suppress loop-unrolling
// bit6 set for bool-to-int on x, bit7 for bool-to-int on y
// bit8 set for bool-to-float on x, bit9 for bool-to-float on y
// bit12 this is a reverse-index loop
// bit13 not used
// bit14 this is u./
// bit15 this is e.

// do one computation. xy bit 0 means fetch/incr y, bit 1 means fetch/incr x.  bit 2 is never used.  lineno is the offset to the row being worked on
#define CPRMDO(name,zzop,xy,fz,lineno,inv)  if((xy)&2)LDBID(xx,OFFSETBID(x,((fz)&0x1000?-1:1)*lineno*NPAR,fz,0x8,0x40,0x100),fz,0x8,0x40,0x100) if((xy)&1)LDBID(yy,OFFSETBID(y,((fz)&0x1000?-1:1)*lineno*NPAR,fz,0x10,0x80,0x200),fz,0x10,0x80,0x200)  \
     if((xy)&2)CVTBID(xx,xx,fz,0x8,0x40,0x100) if((xy)&1)CVTBID(yy,yy,fz,0x10,0x80,0x200)  \
     zzop; if(unlikely((maskatend=_mm256_movemask_pd(zz))!=inv*15)){orign2+=lineno; goto name##out;}

#define CPRMINCR(xy,fz,ct) if((xy)&2)INCRBID(x,((fz)&0x1000?-1:1)*(ct),fz,0x8,0x40,0x100) if((xy)&1)INCRBID(y,((fz)&0x1000?-1:1)*(ct),fz,0x10,0x80,0x200)

// set mask for last group.  Also advance pointer to last block if reversed scan
#define CPRMALIGN(zzop,xy,fz,len)  \
  if((fz)&0x1000){CPRMINCR(xy,fz,NPAR-(len)) endmask = _mm256_loadu_si256((__m256i*)(validitymask+5+(((len)-1)&(NPAR-1))));} else endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-(len))&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */

// the short loop is never used here
#define CPRMDUFF(name,zzop,xy,fz,len,lpmsk,inv) \
     if(!((fz)&(lpmsk))){ \
      orign2=n2=DUFFLPCT((len)-1,3);  /* # turns through duff loop */ \
      orign2<<=3; \
      if(n2>0){ \
       backoff=DUFFBACKOFF((len)-1,3); \
       orign2+=backoff+1;  \
       CPRMINCR(xy,fz,(backoff+1)*NPAR) \
       switch(backoff){ \
       do{ \
       case -1: CPRMDO(name,zzop,xy,fz,0,inv) case -2: CPRMDO(name,zzop,xy,fz,1,inv) case -3: CPRMDO(name,zzop,xy,fz,2,inv) case -4: CPRMDO(name,zzop,xy,fz,3,inv) \
       case -5: CPRMDO(name,zzop,xy,fz,4,inv) case -6: CPRMDO(name,zzop,xy,fz,5,inv) case -7: CPRMDO(name,zzop,xy,fz,6,inv) case -8: CPRMDO(name,zzop,xy,fz,7,inv) \
       CPRMINCR(xy,fz,8*NPAR) \
       }while(--n2!=0); \
       } \
      } \
     }else{orign2=n2=(len-1)>>LGNPAR; \
      if(n2!=0){NOUNROLL do{CPRMDO(name,zzop,4+(xy),fz,0,inv) CPRMINCR(xy,fz,NPAR)}while(--n2!=0);}  \
     } \

#define CPRMMASK(zzop,xy,fz,inv) if((xy)&2)LDBIDM(xx,x,fz,0x8,0x40,0x100,endmask) if((xy)&1)LDBIDM(yy,y,fz,0x10,0x80,0x200,endmask)  \
  if((xy)&2)CVTBID(xx,xx,fz,0x8,0x40,0x100) if((xy)&1)CVTBID(yy,yy,fz,0x10,0x80,0x200)  \
  zzop; maskatend=(!!inv^!!((fz)&0x8000))?_mm256_movemask_pd(_mm256_and_pd(_mm256_castsi256_pd(endmask),zz)):_mm256_movemask_pd(_mm256_or_pd(_mm256_xor_pd(ones,_mm256_castsi256_pd(endmask)),zz));  // shift in match for i., nomatch for e.

#define seqsch256(name,fz,pref,zzop,inv) \
{ \
 __m256d xx,yy,zz; void *x,*y; \
 __m256i endmask; /* length mask for the last word */ \
 __m256d ones=_mm256_setzero_pd(); ones=_mm256_castsi256_pd(_mm256_cmpeq_epi32(_mm256_castpd_si256(ones),_mm256_castpd_si256(ones))); \
 __m256i acc0=_mm256_setzero_si256(), acc1=acc0, acc2=acc0, acc3=acc0, acc4=acc0, acc5=acc0, acc6=acc0, acc7=acc0; \
 I maskatend, nuniq=0; \
 UI backoff; UI n2; I orign2; /* orign2 goes -1 during rev search */ \
 _mm256_zeroupperx(VOIDARG) \
   /* will be removed except for divide */ \
 CVTEPI64DECLS pref \
 y=v; void *x0=u; \
 for(;ac;--ac){I nw=wsct; \
  for(;nw;--nw){x=x0; \
   I n0=asct; \
   LDBID1(yy,y,fz,0x10,0x80,0x200) CVTBID1(yy,yy,fz,0x10,0x80,0x200) \
   CPRMALIGN(zzop,2,fz,n0) \
   CPRMDUFF(name,zzop,2,fz,n0,32+8,inv) \
   CPRMMASK(zzop,2,fz,inv) /* runout, using mask */ \
name##out: \
   if((fz)&0x8000){*(C*)zv=maskatend!=inv*15; zv=(C*)zv+1; \
   }else{ \
    n2<<=3; \
    orign2-=n2; orign2<<=LGNPAR; \
    orign2+=(((fz)&0x1000?inv?0x4322111100000000:0x0000000011112234:inv?0x4010201030102010:0x0102010301020104)>>(maskatend<<2))&7; if((fz)&0x1000){orign2=n0-1-orign2; orign2=orign2<0?n0:orign2;} \
    *(I*)zv=orign2; if((fz)&0x4000){nuniq+=(orign2-wsct+nw)==0; ((I*)zv)[orign2-wsct+nw]++;} zv=(I*)zv+1;  /* if key, look back to incr the first in class */ \
   } \
   INCRBID(y,q,fz,0x10,0x80,0x200) \
  } \
  INCRBID(x0,p,fz,0x08,0x40,0x100) \
  y=(1==wc)?v:y; \
 } \
 if((fz)&0x4000)AM(z)=nuniq; \
}


#define IOSCCASE(bit,multi,mode) ((8*(multi)+(bit))*4+((mode)&3))

// xe is the expression for reading one comparand, exp is the expression for 'no match between x and av[j]')
// loop through storing the index at which a match was found
#define SCDO(bit,T,exp)  \
   case IOSCCASE(bit,0,IIDOT): {T*v0=(T*)v,*wv=(T*)v,x; T*av=(T*)u+asct; DQ(ac, DQ(wsct, x=*wv; j=-asct;   while(j<0 &&(exp))++j; *(I*)zv=j+=asct; zv=(I*)zv+1;       wv+=q;); av+=p; if(1==wc)wv=v0;);} break;  \
   case IOSCCASE(bit,0,IFORKEY): {T*v0=(T*)v,*wv=(T*)v,x; I nuniq=0; T*av=(T*)u+asct; DQ(ac, DO(wsct, x=*wv; j=-asct;   while(j<0 &&(exp))++j; *(I*)zv=j+=asct; nuniq+=(j-i)==0; ((I*)zv)[j-i]++; zv=(I*)zv+1;       wv+=q;); AM(z)=nuniq; av+=p; if(1==wc)wv=v0;);} break;  \
   case IOSCCASE(bit,0,IICO):  {T*v0=(T*)v,*wv=(T*)v,x; T*av=(T*)u; DQ(ac, DQ(wsct, x=*wv; j=asct-1; while(0<=j&&(exp))--j; *(I*)zv=(j=0>j?asct:j); zv=(I*)zv+1; wv+=q;); av+=p; if(1==wc)wv=v0;);} break;  \
   case IOSCCASE(bit,0,IEPS):  {T*v0=(T*)v,*wv=(T*)v,x; T*av=(T*)u+asct; DQ(ac, DQ(wsct, x=*wv; j=-asct;   while(j<0 &&(exp))++j; *(C*)zv=SGNTO0(j); zv=(C*)zv+1;     wv+=q;); av+=p; if(1==wc)wv=v0;);} break;

// same but the cells have n atoms, each of which is compared.  comparands are wv[jj] and avv[jj]
#define SCDON(bit,T,exp)  \
   case IOSCCASE(bit,1,IIDOT): {T*v0=(T*)v,*wv=(T*)v; T*av=(T*)u; DQ(ac, DQ(wsct, j=-asct;   T*avv=av; do{I jj=n-1; T* wvv=wv; do{if(exp)break;}while(--jj>=0); if(jj<0)break; avv+=n;}while(++j<0);   *(I*)zv=j+=asct; zv=(I*)zv+1;       wv+=q;); av+=p; if(1==wc)wv=v0;);} break;  \
   case IOSCCASE(bit,1,IFORKEY): {T*v0=(T*)v,*wv=(T*)v; I nuniq=0; T*av=(T*)u; DQ(ac, DO(wsct, j=-asct;   T*avv=av; do{I jj=n-1; T* wvv=wv; do{if(exp)break;}while(--jj>=0); if(jj<0)break; avv+=n;}while(++j<0);  *(I*)zv=j+=asct; nuniq+=(j-i)==0; ((I*)zv)[j-i]++; zv=(I*)zv+1;       wv+=q;); AM(z)=nuniq; av+=p; if(1==wc)wv=v0;);} break;  \
   case IOSCCASE(bit,1,IICO):  {T*v0=(T*)v,*wv=(T*)v; T*av=(T*)u; DQ(ac, DQ(wsct, j=asct-1;  T*avv=av+asct*n; do{avv-=n; I jj=n-1; T* wvv=wv; do{if(exp)break;}while(--jj>=0); if(jj<0)break;}while(--j>=0);     *(I*)zv=(j=0>j?asct:j); zv=(I*)zv+1; wv+=q;); av+=p; if(1==wc)wv=v0;);} break;  \
   case IOSCCASE(bit,1,IEPS):  {T*v0=(T*)v,*wv=(T*)v; T*av=(T*)u; DQ(ac, DQ(wsct, j=-asct;   T*avv=av; do{I jj=n-1; T* wvv=wv; do{if(exp)break;}while(--jj>=0); if(jj<0)break; avv+=n;}while(++j<0);    *(C*)zv=SGNTO0(j); zv=(C*)zv+1;     wv+=q;); av+=p; if(1==wc)wv=v0;);} break;

// ac is # outer cells of a, asct=#items in 1 inner cell, wc is #outer search cells, wsct is #items to search for per outer cell
// n is #atoms in a cell
static A jtiosc(J jt,I mode,I n,I asct,I wsct,I ac,I wc,A a,A w,A z){I j,p,q; void *u,*v,*zv;
 p=ac>1?asct:0; q=REPSGN(1-(wc|wsct)); p*=n; q&=n;  // q=1<wc||1<wsct; number of atoms to move between repeats p=*atoms of a to move between repeats
 zv=voidAV(z); u=voidAV(a); v=voidAV(w);
 // Create a pseudotype 19 (=XDX) for intolerant comparison.  This puns on XDX-FLX==16
 I bit=CTTZ(AT(a)); bit+=((AT(a)>>FLX)&(jt->cct==1.0))<<4;
 switch(IOSCCASE(bit,n>1,mode)){
  SCDO(B01X,C,x!=av[j]      );
  SCDO(LITX,C,x!=av[j]      );
  SCDO(C2TX,S,x!=av[j]      );
  SCDO(C4TX,C4,x!=av[j]      );
  SCDO(CMPXX,Z,!zeq(x, av[j]));
  SCDO(XNUMX,A,!equ(x, av[j]));
  SCDO(RATX,Q,!QEQ(x, av[j]));
  SCDO(SBTX,SB,x!=av[j]      );
  SCDO(BOXX,A,!equ(C(x),C(av[j])));
#if (C_AVX&&SY_64) || EMU_AVX
  // The instruction set is too quirky to do this with macros
   case IOSCCASE(XDX,0,IIDOT): seqsch256(seqschidotDD0,0x0000,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0) break; 
   case IOSCCASE(XDX,0,IFORKEY): seqsch256(seqschkeyDD0,0x4000,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0) break; 
   case IOSCCASE(XDX,0,IICO): seqsch256(seqschicoDD0,0x1000,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0) break; 
   case IOSCCASE(XDX,0,IEPS): seqsch256(seqschepsDD0,0x8000,,zz=_mm256_cmp_pd(xx,yy,_CMP_EQ_OQ),0) break; 
#define ZZTEQ zz=_mm256_xor_pd(_mm256_cmp_pd(xx,_mm256_mul_pd(yy,cct),_CMP_GT_OQ),_mm256_cmp_pd(yy,_mm256_mul_pd(xx,cct),_CMP_LE_OQ));  // tolerant =
   case IOSCCASE(FLX,0,IIDOT): seqsch256(seqschidotDD,0x0000,__m256d cct=_mm256_broadcast_sd(&jt->cct);,ZZTEQ,0) break; 
   case IOSCCASE(FLX,0,IFORKEY): seqsch256(seqschkeyDD,0x4000,__m256d cct=_mm256_broadcast_sd(&jt->cct);,ZZTEQ,0) break; 
   case IOSCCASE(FLX,0,IICO): seqsch256(seqschicoDD,0x1000,__m256d cct=_mm256_broadcast_sd(&jt->cct);,ZZTEQ,0) break; 
   case IOSCCASE(FLX,0,IEPS): seqsch256(seqschepsDD,0x8000,__m256d cct=_mm256_broadcast_sd(&jt->cct);,ZZTEQ,0) break; 
#else
  SCDO(XDX,D,x!=av[j])
  SCDO(FLX,D,!TCMPEQ(jt->cct,x,av[j]));
#endif


#if (C_AVX2&&SY_64) || EMU_AVX2
   case IOSCCASE(INTX,0,IIDOT): seqsch256(seqschidotII,0x0000,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0) break; 
   case IOSCCASE(INTX,0,IFORKEY): seqsch256(seqschkeyII,0x4000,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0) break; 
   case IOSCCASE(INTX,0,IICO): seqsch256(seqschicoII,0x1000,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0) break; 
   case IOSCCASE(INTX,0,IEPS): seqsch256(seqschepsII,0x8000,,zz=_mm256_castsi256_pd(_mm256_cmpeq_epi64(_mm256_castpd_si256(xx),_mm256_castpd_si256(yy)));,0) break; 
#else
  SCDO(INTX,I,x!=av[j]      );
#endif


  SCDON(B01X,C,wvv[jj]!=avv[jj]      );
  SCDON(LITX,C,wvv[jj]!=avv[jj]      );
  SCDON(C2TX,S, wvv[jj]!=avv[jj]      );
  SCDON(C4TX,C4,wvv[jj]!=avv[jj]      );
  SCDON(CMPXX,Z, !zeq(wvv[jj], avv[jj]));
  SCDON(XNUMX,A, !equ(wvv[jj], avv[jj]));
  SCDON(RATX,Q, !QEQ(wvv[jj], avv[jj]));
  SCDON(INTX,I, wvv[jj]!=avv[jj]      );
  SCDON(SBTX,SB,wvv[jj]!=avv[jj]      );
  SCDON(BOXX,A, !equ(C(wvv[jj]),C(avv[jj])));
  SCDON(XDX,D, wvv[jj]!=avv[jj]) ;
  SCDON(FLX,D, !TCMPEQ(jt->cct,wvv[jj],avv[jj]));
  default:  jsignal(EVSYSTEM);
 }
 R z;  // return non-error indic, but also where the partition count for IFORKEY is stored
}    /* right argument cell is scalar or list; only for modes IIDOT IICO IEPS, and intolerant IFORKEY */


// ***************** fifth class: boxed arguments ************************

// return 1 if a is boxed, and ct==0, and a contains a box whose contents are boxed, or complex, or numeric with more than one atom
static B jtusebs(J jt,A a,I ac,I asct){A*av,x;I t;
 if(!(BOX&AT(a)&&1.0==jt->cct))R 0;
 av=AAV(a); 
 DO(ac*asct, x=C(av[i]); t=AT(x); if(t&BOX+CMPX||1<AN(x)&&t&NUMERIC)R 1;);
 R 0;
}    /* n (# elements in a target item) is assumed to be 1 */

// 
// b means self-index and running i. i: ~. ~: (I.@~.)
// bk means i: (e. i: 0:) (e. i: 1:)   or prehashed version thereof, i. e. backwards
// We grade a, producing the ordering permutation.  Then we go through it to discard duplicates
static A jtnodupgrade(J jt,A a,I acr,I ac,I acn,I ad,I n,I asct,I md,I bk){A*av,h,*u,*v;I*hi,*hu,*hv,l,m1,q;
 RZ(IRS1(a,0L,acr,jtgrade1,h)); hv=AV(h)+bk*(asct-1); av=AAV(a);
 // if not self-index, close up the duplicates
 if(!(md&IIMODREFLEX))for(l=0;l<ac;++l,av+=acn,hv+=asct){  // for each item of the overall result
  // hi->next index in the grade result, q is its value, u->A block for that index
  // hu->next output position.  The first result always stays in place
  hi=hv; q=*hi; u=av+n*q;
  // loop through, setting q/v to the index/address of data.  If *v!=*u, accept q/v as a new value and set u=v
  // don't bother with testing equality if q<index of u (for ascending; reverse for descending)
  // if the list was shortened, replace the last position with -(length of shortened list).  This will be detected
  // and complemented to give (length of list)-1.  0 is OK too, indicating a 1-element list
  if(bk){hu=--hi; DQ(asct-1, q=*hi--; v=av+n*q; if((u<v)||!eqa(n,u,v)){u=v; *hu--=q;}); m1=hv-hu; if(asct>m1)hv[1-asct]=-m1;}
  else  {hu=++hi; DQ(asct-1, q=*hi++; v=av+n*q; if((v<u)||!eqa(n,u,v)){u=v; *hu++=q;}); m1=hu-hv; if(asct>m1)hv[asct-1]=-m1;}
 }
 R h;
} 

// hiinc will inc or dec hi
// zstmti initializes the first result index with the index of the smallest element
// zstmt1 is executed if v is a duplicate, and should repeat the previous result
// zstmt0 is executed if v is not a dup, and must advance the p marker to q as well as store the new result
// don't bother testing for equality if ascending order and q<p (since grade would preserve order if equal), or if descending and q>p
#define BSLOOPAA(hiinc,zstmti,zstmt1,zstmt0)  \
 {A* RESTRICT u=av,* RESTRICT v;I* RESTRICT hi=hv,p,q;             \
  p=*hi; hi+=(hiinc); u=av+n*p; zstmti;  /* u->first result value, install result for that value to index itself */      \
  DQ(asct-1, q=*hi; hi+=(hiinc); v=av+n*q; if(((hiinc>0&&v>u)||(hiinc<0&&v<u))&&eqa(n,u,v)){zstmt1;} else{u=v; zstmt0;}); /* 
   q is input element# that will have result index i, v->it; if *u=*v, v is a duplicate: map the result back to u (index=p)
   if *u!=*p, advance u/p to v/q and use q as the result index */ \
 }

// binary search through the list hu[]
// m1 is the number of items-1(= index of last item)
// p/q are left/right indexes for the binary search
// ii is the item number we start working on
// i is the item number of w we are working on
// icmp is the loop exit condition, finding the last i (depends on direction - I don't see why)
// iinc advances to the next item of w (depends on direction - I don't see why)
// uinc advances the pointer to the next item of w (depends on direction - I don't see why)
// zstmt creates the result when a match has been found.  At that point q=-2 if there was no match, otherwise
//  it holds the index of the match
#define BSLOOPAWX(ii,icmp,iinc,uinc,zstmt)  \
 {A* RESTRICT u=wv+n*(ii),* RESTRICT v;I i,j,p,q;I t;  \
  for(i=ii;icmp;iinc,uinc){          \
   p=0; q=m1;                        \
   NOUNROLL while(p<=q){                      \
    t=0; j=(p+q)>>1; v=av+n*hu[j];    \
    DO(n, if(t=compare(C(u[i]),C(v[i])))break;);  \
    if(0<t)p=j+1; else q=t?j-1:-2;   \
   }                                 \
   zstmt;                            \
  }  \
 }

// macros to create ascending or descending binary search, executing zstmt afterwards
#define BSLOOPAW(zstmt)     BSLOOPAWX(0  ,i< wsct,++i,u+=n,zstmt)
#define BSLOOQAW(zstmt)     BSLOOPAWX(wsct-1,i>=0,--i,u-=n,zstmt)

// index by sorting a into order, then doing binary search on each item of w.
// Used only when ct=0 and (boxed rank>1 or boxes contain numeric arrays)

// This function needs acr for the sort.  We pass it in throuigh AM(h), since h is otherwise unused (except to hold the return value from IFORKEY)
// (if prehashing, we pass in acr only for the prehash, and h thereafter)
static IOF(jtiobs){A*av,*wv,y;B *yb,*zb;C*zc;I acn,*hu,*hv,l,m1,md,s,wcn,*zi,*zv;
 md=mode&IIOPMSK;  // just the operation bits
 I bk=1&(((1<<IICO)|(1<<IJ0EPS)|(1<<IJ1EPS))>>md);  // set if the dup-scan is reverse direction
 if(mode==INUB||mode==INUBI){GATV0(y,B01,asct,1); yb=BAV(y);}
 av=AAV(a);  acn=ak>>LGSZI;
 wv=AAV(w);  wcn=wk>>LGSZI;
 zi=zv=AV(z); zb=(B*)zv; zc=(C*)zv;
 // If a has not been sorted already, sort it
 if(!(mode&IPHOFFSET)){  // if we are not using a presorted table...
  // look for IIDOT/IICO/INUBSV/INUB/INUBI/IFORKEY - we set IIMODREFLEX if one of those is set.  They don't remove dups.
  // we don't set REFLEX if there is a prehash, because the prehash always removes dups, and we would be left missing some values
  if(!(((uintptr_t)a^(uintptr_t)w)|(ac^wc)))md+=IIMODREFLEX&((((1<<IIDOT)|(1<<IICO)|(1<<INUBSV)|(1<<INUB)|(1<<INUBI)|(1<<IFORKEY))<<IIMODREFLEXX)>>md);
  RZ(h=nodupgrade(a,(I)h,ac,acn,0,n,asct,md,bk));   // h is used to pass in acr
 }
 if(w==mark)R h;
 hv=AV(h)+bk*(asct-1);
 for(l=0;l<ac;++l,av+=acn,wv+=wcn,hv+=asct){  // loop for each result in a
  // m1=index of last item-1, which may be less than m-1 if there were discarded duplicates (signaled by last index <0)
  s=hv[bk?1-asct:asct-1]; m1=0>s?~s:asct-1; hu=hv-m1*bk;
  switch(md){
   // self-indexes
   case IIDOT|IIMODREFLEX:        BSLOOPAA(1,zv[p]=p,zv[q]=p,zv[q]=p=q); zv+=asct;     break;
   case IFORKEY|IIMODREFLEX: {I nuniq=0;BSLOOPAA(1,++nuniq;zv[p]=p+1,zv[q]=p;zv[p]++,++nuniq;zv[q]=(p=q)+1); zv+=asct; AM(h)=nuniq;     break;}
   case IICO|IIMODREFLEX:         BSLOOPAA(-1,zv[p]=p,zv[q]=p,zv[q]=p=q); zv+=asct;     break;
   case INUBSV|IIMODREFLEX:       BSLOOPAA(1,zb[p]=1,zb[q]=0,zb[q]=1  ); zb+=asct;     break;
   case INUB|IIMODREFLEX:         BSLOOPAA(1,yb[p]=1,yb[q]=0,yb[q]=1  ); DO(asct, if(yb[i]){MC(zc,av+i*n,k); zc+=k;}); ZCSHAPE; break;
   case INUBI|IIMODREFLEX:        BSLOOPAA(1,yb[p]=1,yb[q]=0,yb[q]=1  ); DO(asct, if(yb[i])*zi++=i;);                  ZISHAPE; break;
   // searches, by binary search
   case IIDOT:        BSLOOPAW(*zv++=-2==q?hu[j]:asct);                       break;
   case IICO:         BSLOOPAW(*zv++=-2==q?hu[j]:asct);                       break;
   case IEPS:         BSLOOPAW(*zb++=-2==q);                               break;
   case ILESS:        BSLOOPAW(if(-2< q){MC(zc,u,k); zc+=k;}); ZCSHAPE;    break;
   case IINTER:       BSLOOPAW(if(!(-2< q)){MC(zc,u,k); zc+=k;}); ZCSHAPE;    break;
   case II0EPS:  s=asct; BSLOOPAW(if(-2< q){s=i; break;});        *zi++=s;    break;
   case IJ0EPS:  s=asct; BSLOOQAW(if(-2< q){s=i; break;});        *zi++=s;    break;
   case II1EPS:  s=asct; BSLOOPAW(if(-2==q){s=i; break;});        *zi++=s;    break;
   case IJ1EPS:  s=asct; BSLOOQAW(if(-2==q){s=i; break;});        *zi++=s;    break;
   case IANYEPS: s=0; BSLOOPAW(if(-2==q){s=1; break;});        *zb++=(B)s; break;
   case IALLEPS: s=1; BSLOOPAW(if(-2< q){s=0; break;});        *zb++=(B)s; break;
   case ISUMEPS: s=0; BSLOOPAW(if(-2==q)++s);                  *zi++=s;    break;
   case IIFBEPS:      BSLOOPAW(if(-2==q)*zi++=i);              ZISHAPE;    break;
 }}
 R h;
}    /* a i.!.0 w on boxed a,w by grading and binary search */

static I jtutype(J jt,A w,I c){A*wv,x;I m,t;
 if(!AN(w))R 1;
 m=AN(w); wv=AAV(w); 
 DO(c, t=0; DOSTEP(m,c, x=C(wv[i]); if(AN(x)){if(t){if(!(TYPESEQ(t,AT(x))))R 0;} else{t=AT(x); if(t&FL+CMPX+BOX)R 0;}}););
 R t;
}    /* return type if opened atoms of cells of w has uniform type (but not one that may contain -0), else 0. c is # of cells */

// *************************** sixth class: hashing w ***********************
// used when w is shorter than a and thus more likely to fit into cache.  Also allows early exit when all results found.
//  Intolerant comparisons only.   Used for i./i:/e. only, and never reflexive

// start a new class.  The index is i.  It does not need to be translated, since it will be where the data for its class is.
// The root of the class is the first index found (i).  Init to 'not found' (could be faster to init them all at once)
#define XDOWNEWCLASS(TH,TZ,zptr,nfval) {++chainct; indtdd[i]=(TH)i; zptr[i]=(TZ)nfval;}
// add to an old class.  The index is i; hj is the root of the class (the smallest index for the class).
#define XDOWOLDCLASS(TH) {indtdd[i]=(TH)hj;}

// A value from a matched the hashtable.  The hashtable entry will be erased; here we store the index of the match (i) into
// the result hashchain (anchored at hj)
#define XDOWFOUND(TZ,zptr,zvalue,earlyexit) {zptr[hj]=(TZ)zvalue; if(--chainct==0)goto earlyexit;}

// After the results have been calculated, go through the indirect table and copy the result for any value that is not start-of-class
#define XDOWINDCLASS(zptr) {I zx; DO(wsct, if((zx=indtdd[i])!=i){zptr[i]=zptr[zx];});}

// Traverse w in forward direction, adding values to the hash table
#define XDOWP(T,TH,TZ,zptr,hash,exp,stride,nfval) XSEARCH(T,TH,w,w,hash,exp,stride,XDOWOLDCLASS(TH),XDOWNEWCLASS(TH,TZ,zptr,nfval),1,0, (i=0;i<wsct-2;++i) ,wsct-1)

// Traverse a in forward direction, executing fstmt/nfstmt depending on found/notfound; clearing the hash if a match is found
#define XDOWA(T,TH,TZ,zptr,hash,exp,stride,zvalue,earlyexit) XSEARCH(T,TH,a,w,hash,exp,stride,XDOWFOUND(TZ,zptr,zvalue,earlyexit),{},2,0, (i=0;i<asct-2;++i) ,asct-1)
// same, traversing a from back to front
#define XDQWA(T,TH,TZ,zptr,hash,exp,stride,zvalue,earlyexit) XSEARCH(T,TH,a,w,hash,exp,(-(stride)),XDOWFOUND(TZ,zptr,zvalue,earlyexit),{},2,cn*(asct-1), (i=asct-1;i>1;--i) ,0)

#define IOFXW(T,TH,f,setup,hash,exp,stride)   \
 IOF(f){I acn=ak/sizeof(T),cn=k/sizeof(T),l,p,  \
        wcn=wk/sizeof(T);T* RESTRICT av=(T*)AV(a),* RESTRICT wv=(T*)AV(w);I md; TH * RESTRICT hv; \
        IH *hh=IHAV(h); p=hh->datarange;  hv=hh->data.TH;  \
 \
  __m128i vp, vpstride;   /* v for hash/v for search; stride for each */ \
  _mm256_zeroupperx(VOIDARG)  \
  setup \
  vp=_mm_setzero_si128();  /* to avoid warnings */ \
  md=mode&IIOPMSK;   /* clear upper flags including REFLEX bit */  \
  A indtbl; GATV0(indtbl,INT,((asct*sizeof(TH)+SZI)>>LGSZI),0); TH * RESTRICT indtdd=TH##AV(indtbl); \
  for(l=0;l<ac;++l,av+=acn,wv+=wcn){I chainct=0;  /* number of chains in w */   \
   /* zv progresses through the result - for those versions that support IRS */ \
   hashallo(hh,p,wsct,mode); \
     \
   switch(md){                                                                       \
   case IICO:\
   case IIDOT: {I * RESTRICT zv=AV(z)+l*wsct; XDOWP(T,TH,I,zv,hash,exp,stride,asct); \
     if(md==IIDOT)XDOWA(T,TH,I,zv,hash,exp,stride,i,allfound1)else XDQWA(T,TH,I,zv,hash,exp,stride,i,allfound1)  allfound1: XDOWINDCLASS(zv); } break;  \
   case IEPS: {B * RESTRICT zb=BAV(z)+l*wsct; XDOWP(T,TH,B,zb,hash,exp,stride,0); XDOWA(T,TH,B,zb,hash,exp,stride,1,allfound2); allfound2: XDOWINDCLASS(zb); } break; \
   } \
  }                                                                                  \
  R h;                                                                               \
 }

static IOFXW(A,US,jtiowax1,,cthia(~0LL,1.0,C(*v)),!equ(C(*v),C(wv[hj])),1  )  /* boxed exact 1-element item */   
static IOFXW(A,US,jtiowau,, hiau(C(*v)),  !equ(C(*v),C(wv[hj])),1  )  /* boxed uniform type         */
static IOFXW(X,US,jtiowx,,  hix(v),            !eqx(n,v,wv+n*hj),               cn)  /* extended integer           */   
static IOFXW(Q,US,jtiowq,,  hiq(v),            !eqq(n,v,wv+n*hj),               cn)  /* rational number            */   
static IOFXW(C,US,jtiowc,,  hic(k,(UC*)v),     memcmpne(v,wv+k*hj,k),             cn)  /* boolean, char, or integer  */
static IOFXW(I,US,jtiowi,COMPSETUP,  hici(n,v),            COMPCALL(wv),          cn  )  // INT array, not float
static IOFXW(I,US,jtiowi1,,  hici1(v),           *v!=wv[hj],                    1 )  // len=8, not float
static IOFXW(D,US,jtiowc01,, hic01((UIL*)v),    *v!=wv[hj],                      1) // float atom
static IOFXW(Z,US,jtiowz01,, hic0(2,(UIL*)v),    (v[0].re!=wv[hj].re)||(v[0].im!=wv[hj].im), 1) // complex atom
static IOFXW(D,US,jtiowc0,, hic0(n,(UIL*)v),    fcmp0(v,&wv[n*hj],n),           cn) // float array
static IOFXW(Z,US,jtiowz0,, hic0(2*n,(UIL*)v),    fcmp0((D*)v,(D*)&wv[n*hj],2*n),  cn) // complex array

static IOFXW(A,UI4,jtiowax12,,cthia(~0LL,1.0,C(*v)),!equ(C(*v),C(wv[hj])),1  )  /* boxed exact 1-element item */   
static IOFXW(A,UI4,jtiowau2,, hiau(C(*v)),  !equ(C(*v),C(wv[hj])),1  )  /* boxed uniform type         */
static IOFXW(X,UI4,jtiowx2,,  hix(v),            !eqx(n,v,wv+n*hj),               cn)  /* extended integer           */   
static IOFXW(Q,UI4,jtiowq2,,  hiq(v),            !eqq(n,v,wv+n*hj),               cn)  /* rational number            */   
static IOFXW(C,UI4,jtiowc2,,  hic(k,(UC*)v),     memcmpne(v,wv+k*hj,k),             cn)  /* boolean, char, or integer  */
static IOFXW(I,UI4,jtiowi2,COMPSETUP,  hici(n,v),            COMPCALL(wv),          cn  )  // INT array, not float
static IOFXW(I,UI4,jtiowi12,,  hici1(v),           *v!=wv[hj],                    1 )  // len=8, not float
static IOFXW(D,UI4,jtiowc012,, hic01((UIL*)v),    *v!=wv[hj],                      1) // float atom
static IOFXW(Z,UI4,jtiowz012,, hic0(2,(UIL*)v),    (v[0].re!=wv[hj].re)||(v[0].im!=wv[hj].im), 1) // complex atom
static IOFXW(D,UI4,jtiowc02,, hic0(n,(UIL*)v),    fcmp0(v,&wv[n*hj],n),           cn) // float array
static IOFXW(Z,UI4,jtiowz02,, hic0(2*n,(UIL*)v),    fcmp0((D*)v,(D*)&wv[n*hj],2*n),  cn) // complex array


// *************************** seventh class: small-range processing of w ***********************
// used when w is shorter than a and thus more likely to fit into cache.  Also allows early exit when all results found.
//  Intolerant comparisons only.   Used for i./i:/e. only, and never reflexive


// Get the next value (j) from position i, and look at the hash (hv[j], called hj) to see if this is a new class.  It is, if the hashtable still contains
// its initial wsct value.  If new class, remember it in the hashtable and (complemented) in the result.  If old class, the hash value is the class; remember it.
// When we are done, each result value has the complement of the first matching index in w
#define XDOWSP(T,TH) {I j, hj; DO(wsct, j=wv[i]; hj=hv[j]; if(hj==wsct){++chainct;hv[j]=(TH)i;zv[i]=~i;}else{zv[i]=~hj;})}
// Scan forward through a.  Read value[i]; if out of bounds, divert to 'not-found' location; read from table; if table has a value, store the result value and set so we only do that once
#define XDOWSA(T,TH,earlyexit) {I j, hj; DO(asct, \
  j=av[i]; j=(j<minimum)?maximum:j; j=(j>maximum)?maximum:j; hj=hv[j]; if(hj<wsct){hv[j]=(TH)wsct; zv[hj]=i; if(--chainct==0)goto earlyexit;})}
// Same but reverse scan through a
#define XDQWSA(T,TH,earlyexit) {I j, hj; DQ(asct, \
  j=av[i]; j=(j<minimum)?maximum:j; j=(j>maximum)?maximum:j; hj=hv[j]; if(hj<wsct){hv[j]=(TH)wsct; zv[hj]=i; if(--chainct==0)goto earlyexit;})}
// Go through the result table.  If the value is positive, it is a valid found value.  If negative, its complement is the self-classify index in w.  If this
// self-classify index matches the result index, that means the item was never found; store 'not found' result.  Otherwise copy from the earlier index.
#define XDOWGETRESULT {I zvv; DO(wsct, if((zvv=zv[i])<0){zv[i]=(zvv=~zvv)==i?asct:zv[zvv];})}

// same for bit table, which doesn't need to self-classify w
#define XDOWSB(T,TH) {I j, hj; DO(wsct, j=wv[i]; hj=hv[j]; if(hj>0){++chainct;hv[j]=(TH)0;})}
// After the results have been calculated, go through the indirect table and copy the result from the table
// Scan forward through a.  Read value[i]; if out of bounds, divert to 'not-found' location; read from table; if table has a value, store the result value
#define XDOWSAB(T,TH,earlyexit) {I j, hj; DO(asct, \
  j=av[i]; j=(j<minimum)?maximum:j; j=(j>maximum)?maximum:j; hj=hv[j]; if(hj==0){hv[j]=(TH)1; if(--chainct==0)goto earlyexit;})}
#define XDOWGETRESULTB(T,TH,zptr) {DO(wsct, zptr[i]=(TH)hv[wv[i]];)}

// same, but for packed bits.  We know that bits are inited to 1, set to 0 when found in w, and set back to 1 when found in a
#define XDOWSPB(T,TH) {I j, hj; DO(wsct, j=wv[i]; UC bitmsk=1<<BITNO(j); hj=hv[BYTENO(j)]; if(hj&bitmsk){++chainct;hv[BYTENO(j)]=(TH)(hj^=bitmsk);})}
// Scan forward through a.  Read value[i]; if out of bounds, divert to 'not-found' location; read from table; if table has a value, store the result value
#define XDOWSAPB(T,TH,earlyexit) {I j, hj; DO(asct, \
  j=av[i]; j=(j<minimum)?maximum:j; j=(j>maximum)?maximum:j; UC bitmsk=1<<BITNO(j); hj=hv[BYTENO(j)]; if(!(hj&bitmsk)){hv[BYTENO(j)]=(TH)(hj^=bitmsk); if(--chainct==0)goto earlyexit;})}
// After the results have been calculated, go through the indirect table and copy the result for any value that is not start-of-class
#define XDOWGETRESULTPB(T,TZ,zptr) {DO(wsct, zptr[i]=(TZ)((hv[BYTENO(wv[i])]>>BITNO(wv[i]))&1);)}

// convert the hashtable in *hv to a packed-bit hashtable in *hvp, preserving the validity limits
#define XDOWREPACK(T,TH) {I *hvpw = (I*)hvp+((maximum+1)>>LGBW); TH *hv2=hv+maximum; I q = 0; DQ((maximum+1)&(BW-1), q=q+q+(*hv2-->=(TH)wsct);) *hvpw--=q; \
  DQ(((hv2-hv)-minimum+1)>>LGBW, DQ(BW, q=q+q+(*hv2-->=(TH)wsct);) *hvpw--=q;) I fct=(hv2-hv)-minimum+1; DQ(fct, q=q+q+(*hv2-->=(TH)wsct);) q<<=(BW-fct); *hvpw=q; \
 }
// Do forward scan of a packed bit hashtable.  When a new value is found, use *hv to give the result location to remember it in
#define XDOWSARPB(T,TH,earlyexit) {I j, hj; DO(asct, \
  j=av[i]; j=(j<minimum)?maximum:j; j=(j>maximum)?maximum:j; I bitmsk=((I)1)<<BITNO(j); hj=hvp[BYTENO(j)]; if(!(hj&bitmsk)){hvp[BYTENO(j)]=(UC)(hj^=bitmsk); zv[hv[j]]=i; if(--chainct==0)goto earlyexit;})}
// Same for reverse scan
#define XDQWSARPB(T,TH,earlyexit) {I j, hj; DQ(asct, \
  j=av[i]; j=(j<minimum)?maximum:j; j=(j>maximum)?maximum:j; I bitmsk=((I)1)<<BITNO(j); hj=hvp[BYTENO(j)]; if(!(hj&bitmsk)){hvp[BYTENO(j)]=(UC)(hj^=bitmsk); zv[hv[j]]=i; if(--chainct==0)goto earlyexit;})}

#define IOFXWS(f,T,TH)   \
 IOF(f){I acn=ak/sizeof(T),cn=k/sizeof(T),l,p,  \
  wcn=wk/sizeof(T);T* RESTRICT av=(T*)AV(a),* RESTRICT wv=(T*)AV(w);I md; \
  /* establish min/max of data; create biased pointer to table area;  */ \
  /* When we allocated the table we left room for the extra entry */ \
  IH *hh=IHAV(h); I minimum=hh->datamin; p=hh->datarange; I maximum=minimum+p; \
  /* if the hashtable for i./i: exceeds the cache size, allocate a packed-bit version of it. \
  We will compress the hashtable after self-classifying w.  We compare against L2 size; there is value in staying in L1 too */ \
  UC *hvp; if((p<(L2CACHESIZE>>hh->hashelelgsize))||(mode&(IIOPMSK^(IICO|IIDOT)))){hvp=0;}else{A hvpa; GATV0(hvpa,INT,3+(p>>LGBW),0); hvp=UCAV(hvpa)-BYTENO(minimum)+SZI;} \
  \
  _mm256_zeroupperx(VOIDARG)  \
  md=mode&(IIOPMSK|IIMODPACK);   /* clear upper flags including REFLEX bit */  \
  for(l=0;l<ac;++l,av+=acn,wv+=wcn){I chainct=0;  /* number of chains in w */   \
   /* zv progresses through the result - for those versions that support IRS */ \
   hashallo(hh,p,wsct,mode); \
     \
   switch(md){                                                                       \
    /* start by adding an 'empty' entry after the end of the table.  This will be referred to by out-of-bounds values of a */ \
   case IICO:\
   case IIDOT: {TH * RESTRICT hv=hh->data.TH-minimum; hv[maximum]=(TH)wsct; I * RESTRICT zv=AV(z)+l*wsct; XDOWSP(T,TH); \
     if(hvp==0){if(md==IIDOT)XDOWSA(T,TH,allfound3)else XDQWSA(T,TH,allfound3)} \
     else{XDOWREPACK(T,TH) if(md==IIDOT)XDOWSARPB(T,TH,allfound3)else XDQWSARPB(T,TH,allfound3) } \
     allfound3: XDOWGETRESULT } break; \
   case IEPS: {UC * RESTRICT hv=hh->data.UC-minimum; hv[maximum]=1; B * RESTRICT zb=BAV(z)+l*wsct; XDOWSB(T,B); XDOWSAB(T,B,allfound4); allfound4: XDOWGETRESULTB(T,B,zb); } break; \
   case IEPS|IIMODPACK: {UC * RESTRICT hv=hh->data.UC-BYTENO(minimum); hv[BYTENO(maximum)]|=1<<BITNO(maximum); B * RESTRICT zb=BAV(z)+l*wsct; XDOWSPB(T,B); XDOWSAPB(T,B,allfound5); allfound5: XDOWGETRESULTPB(T,B,zb); } break; \
   } \
  }                                                                                  \
  R h;                                                                               \
 }

// *************************** eighth class: x i. y where a and w are both sorted ***********************
// process as for sequential file update.  Items of a are integer atoms only for now
/*
 asct    target axis length (number of things to be searched from in a single pass)
 n    target item # atoms
 wsct    # target items in a left-arg cell, which may include multiple right-arg cells (number of searches)
 k    target item # bytes
 ac   # left  arg cells  (cells, NOT items)
 wc   # right arg cells
 ak   # bytes left  arg cells, or 0 if only 1 cell
 wk   # bytes right arg cells, or 0 if only 1 cell
 a    left  arg
 w    right arg, or mark for m&i. or m&i: or e.&n or -.&n
 h   pointer to hash table or to 0
 z    result
*/
static IOF(jtiosfu){I i;
 I acn=ak>>LGSZI; I wcn=wk>>LGSZI; I * RESTRICT zv=IAV(z);
 I *RESTRICT av=IAV(a), *wv=IAV(w);
 for(;ac>0;av+=acn,wv+=wcn,--ac){  // loop for each cell of i."r
  if(wsct<(asct>>4)){
   I *zend=zv+wsct;  // end+1 of output area
   I zvwvofst=(I)wv-(I)zv;  // distance from zv to corresponding wv
   // here when y is much shorter than x.  We will use binary search to avoid having to touch every cacheline of x.
   // We use the same search order for every starting value of y so that we ensure that the path to the leaf is
   // mostly in cache.  And, we don't try to avoid branches since they will mostly predict correctly, until the
   // last few
   //
   // we could try to start the search from a saved position, as long as we don't change the search path
   I wval, aliml, alimr, zval;
   // find a starting point within a stride of the beginning of the correct point, put it into aliml
   aliml=0; alimr=asct-1; wval=*(I*)((I)zv+zvwvofst);
   zval=asct;
   NOUNROLL while(1){
    // av is the start of the a vector
    // zv points to the next output location
    // zv+zvwvofst points to the data we will read into wval
    // aval is the value read from a
    // ax is the index of aval
    // wval is the w value to compare aval against, i. e. the value corresponding to the next output.
    // aliml is the index of the smallest value that is not known to be less than wval
    // alimr is the index of the largest value that is not known to be >= wval
    // zval is the result value to write
    // asct and zend are used to find end of a/w

    I ax=(aliml+alimr)>>1;  // get midpoint address
    I aval=av[ax];  // read the value
    if(aval<wval){aliml=ax+1;}else{alimr=ax-1; zval=aval==wval?ax:zval;}  // update endpoints
    if(alimr<aliml){ // if end of search...
     *zv++=zval;  // write out the match status
     if(unlikely(zv==zend))break;   // exit if zv is past the end
     wval=*(I*)((I)zv+zvwvofst);  // read next value, always in L1 cache
     zval=asct; alimr=asct-1; aliml=0; // we know there is at least one value to read
    }
   }
   NOUNROLL while(zv!=zend)*zv++=asct;  // all the rest not found
  }else{
   // y is almost as big as x.  There is no gain from trying to skip through the cache, so we will process x and y sequentially
   I *zend=zv+wsct-1;  // end of output area
   I ax=0, *av0=av, *wv0=wv; 
   I a0=*av0++, w0=*wv0++;
   if(unlikely(wsct==1)){wv0=jt->shapesink; ++zend;}  // special processing needed when we fill next-last output.   Here we start there
   while(1){
    if(unlikely(ax==asct-1))break;
    I a1=*av0, w1=*wv0;
    I adva=a0<w0?SZI:0;
    av0=(I*)((I)av0+adva); ax+=adva>>LGSZI; adva^=SZI;
    wv0=(I*)((I)wv0+adva);
    I zval=asct; zval=a0==w0?ax:zval; *zv=zval;
    zv=(I*)((I)zv+adva);
    if(unlikely(zv==zend)){if(wv0==jt->shapesink+1)break; wv0=jt->shapesink; ++zend;}  // if we are about to fill last z, we are about to overfetch w.  Point wv0 harmlessly.  Second time, ewe're through
    a0=adva?a0:a1; w0=adva?w1:w0;
   }
   // at the end we stopped before handling the last element of a (because we would have overfetched a if we continued)
   // put out all the rest of w.  We have incremented wv0 once 
   if(unlikely(zv!=zend)){  // more output needed
    if(likely(wv0!=jt->shapesink))++zend;  // if we stopped before the very last result, we have an extra one to do
    NOUNROLL while(1){*zv++=w0==a0?ax:asct; if(zv==zend)break; w0=*wv0++;}  // handle all the remaining ws, checking against the last a
   }
   
  }
 }
 R z;   // harmless nonzero return that we can fetch AM() from
}

static IOFXWS(jtio42w,I,US)  static IOFXWS(jtio44w,I,UI4)  // INT-sized items, using small/large hashtable

#define ASSESSBLOCKSIZE 64  // every so many inputs, check for range too large
// Routine to find range of an array of I
// The return is a CR struct holding max and range+1.  But if the range+1 is > maxrange,
// we abort and return 0 range.
// min and max are initial values for min/max
#if C_AVX2 || EMU_AVX2
CR condrange(I *s,I n,I min,I max,I maxrange){CR ret;
 if(unlikely(n==0))goto fail;
 I nqw=(n-1)>>LGNPAR;  // number of full-or-partial widewords-1
 __m256i min0=_mm256_set1_epi64x(min);  // atoms between cells
 __m256i max0=_mm256_set1_epi64x(max);  // atoms between cells
 __m256i min1, max1, min2=min0, max2=max0, min3=min0, max3=max0;
 __m256i ones = _mm256_cmpeq_epi8(min0,min0);  // all ones for switching max to complement form
 __m256i endmask= _mm256_loadu_si256((__m256i const *)(validitymask+((-n)&(NPAR-1))));
 if(nqw--){  // if there are at least 2 blocks...
  // nqw is now #blocks-2, because we are going to read the last one in here, leaving one less to process 
  // read the last batch into min/max1.  It might well hold the biggest or smallest values
  min1 = max1 = _mm256_loadu_si256((__m256i const *)(s+(nqw<<LGNPAR)));
  // since we just read the last full block, nqw is now the number yet to do-1 (including the ending masked block), i. e. nqw is # full blocks to do
  I batchsize = 16;  // number of batches to read before checking for range exceeded
  while(nqw>0){  // at least 2 blocks left - we will always leave the last one for the masked load
   I compn=batchsize; compn=compn+120>nqw?nqw:compn;  // number to read before combining values.  A break+compare costs the same as 30 batches
   nqw-=compn;  // decr for all the reads we will make
#define TAKEMINOF(z,x)  z = _mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(z),_mm256_castsi256_pd(x),_mm256_castsi256_pd(_mm256_cmpgt_epi64(z,x))));  // if min>temp, take temp
#define TAKEMAXOF(z,x)  z = _mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(x),_mm256_castsi256_pd(z),_mm256_castsi256_pd(_mm256_cmpgt_epi64(z,x))));  // if max>temp, take max
   UI n2=DUFFLPCT(compn<<LGNPAR,2);  // # turns through duff loop - always at least 1 because compn>=1.  Input to DUFF* is # elements-1
   __m256i temp;
   UI backoff=DUFFBACKOFF(compn<<LGNPAR,2);
   s+=(backoff+1)*NPAR;
   switch(backoff){
   do{
   case -1: temp = _mm256_loadu_si256((__m256i const *)s); TAKEMINOF(min0,temp) TAKEMAXOF(max0,temp)
   case -2: temp = _mm256_loadu_si256((__m256i const *)(s+1*NPAR)); TAKEMINOF(min1,temp) TAKEMAXOF(max1,temp)
   case -3: temp = _mm256_loadu_si256((__m256i const *)(s+2*NPAR)); TAKEMINOF(min2,temp) TAKEMAXOF(max2,temp)
   case -4: temp = _mm256_loadu_si256((__m256i const *)(s+3*NPAR)); TAKEMINOF(min3,temp) TAKEMAXOF(max3,temp)
   s+=4*NPAR;
   }while(--n2>0);
   }
   if(nqw==0)break;  // all batches read, read the remnant and finish
   // more batches to read.  Check for early exit
   // combine the mins into one, and the maxes.  Take the difference & see if it exceeds max allowable range
   // first combine the wide accumulators
   TAKEMINOF(min0,min1) TAKEMAXOF(max0,max1) TAKEMINOF(min2,min3) TAKEMAXOF(max2,max3) TAKEMINOF(min0,min2) TAKEMAXOF(max0,max2)
   // Convert max to complement so we can do min/max simultaneously
   min1=_mm256_xor_si256(ones,max0);  //  min1 destroyed! min1 now complement form X X X X
   max1=_mm256_castps_si256(_mm256_blend_ps(_mm256_castsi256_ps(min0),_mm256_castsi256_ps(min1),0x33));  // max1 destroyed! now has X n X n
   min1=_mm256_castps_si256(_mm256_blend_ps(_mm256_castsi256_ps(min0),_mm256_castsi256_ps(min1),0xcc));  // min1 now has n X n X  (X=compl max, n=min)
   max1=_mm256_castpd_si256(_mm256_permute_pd(_mm256_castsi256_pd(max1),0x5));  // max1 has n X n X
   TAKEMINOF(min1,max1);  // min1 is n X n X
   max1=_mm256_permute4x64_epi64(min1,0b11111110);  // max1 has n X - -  from upper min1
   TAKEMINOF(min1,max1);  // min1 is n X - -, the overall min & compl max
   min = _mm256_extract_epi64(min1,0); max = _mm256_extract_epi64(min1,1);  // max is still complement
   if(unlikely((UI)((~max)-min)>=(UI)maxrange))goto fail;  // abort if range limit exceeded
   // the check above takes 20 cycles.  Increase the batchsize up to ~1000 cycles
   //  Now the values in min/max 0/1 are all scrambled, but the min is somewhere in min0 and the max is somewhere in max0, and min1/max1 contain all valid values
   batchsize<<=2; batchsize=batchsize>1024?1024:batchsize;   // after the first compare, which includes the end batch, grow batch size rapidly
   min1=min0; max1=max0;  // restore the complement registers
  }
  s+=NPAR;  // skip over the last batch, previously read
 }else{min1=min0; max1=max0;}
 // append the last section, 1-4 longs
 TAKEMINOF(min2,min3) TAKEMAXOF(max2,max3)  // free min3/max3
 __m256i temp = _mm256_maskload_epi64(s,endmask);  // read valid values
 min0 = _mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(min0),_mm256_castsi256_pd(temp),_mm256_castsi256_pd(_mm256_and_si256(_mm256_cmpgt_epi64(min0,temp),endmask))));  // masked TAKEMINOF
 max0 = _mm256_castpd_si256(_mm256_blendv_pd(_mm256_castsi256_pd(max0),_mm256_castsi256_pd(temp),_mm256_castsi256_pd(_mm256_and_si256(_mm256_cmpgt_epi64(temp,max0),endmask))));
 // make final combination
   // combine the mins into one, and the maxes.  Take the difference & see if it exceeds max allowable range
 TAKEMINOF(min0,min1) TAKEMAXOF(max0,max1) TAKEMINOF(min0,min2) TAKEMAXOF(max0,max2)
 // Convert max to complement so we can do min/max simultaneously
 min1=_mm256_xor_si256(ones,max0);  //  min1 destroyed! min1 now complement form X X X X
 max1=_mm256_castps_si256(_mm256_blend_ps(_mm256_castsi256_ps(min0),_mm256_castsi256_ps(min1),0x33));  // max1 destroyed! now has X n X n
 min1=_mm256_castps_si256(_mm256_blend_ps(_mm256_castsi256_ps(min0),_mm256_castsi256_ps(min1),0xcc));  // min1 now has n X n X  (X=compl max, n=min)
 max1=_mm256_castpd_si256(_mm256_permute_pd(_mm256_castsi256_pd(max1),0x5));  // max1 has n X n X
 TAKEMINOF(min1,max1);  // min1 is n X n X, which includes everything
 max1=_mm256_permute4x64_epi64(min1,0b11111110);  // max1 has n X - -  from upper min1
 TAKEMINOF(min1,max1);  // min1 is n X - -, the overall min & compl max
 min = _mm256_extract_epi64(min1,0); max = _mm256_extract_epi64(min1,1);  // max is still complement
 if(unlikely((UI)((~max)-min)>=(UI)maxrange))goto fail; ret.min=min; ret.range=(~max)-min+1;  // because the tests succeed, this will give the proper range
 R ret;
fail: ret.min=ret.range=0; R ret;
}
// Same for 4-bytes types, such as C4T
CR condrange4(C4 *s,I n,I min,I max,I maxrange){CR ret;I i; C4 min0,min1,max0,max1;C4 x;
 // Unroll loop once to keep the compares rolling
 if(!n)goto fail;
 min0=min1=(C4)min; max0=max1=(C4)max;  // initial values
 if(n&1)min1=max1=*s++;  // if odd number of words, take first word to even it up
 if(n>>=1){  // n=#pairs of words left
  --n; i=n&(ASSESSBLOCKSIZE/2-1); n>>=5;  // do a block of compares to get on boundary; n=#64-words blocks left, i=size-1 of this block
  do{  // We keep this short loop separate because it always finishes with a misprediction.
   x=s[0]; max0=(x>max0)?x:max0; min0=(x<min0)?x:min0; // this generates CMOVcc in VS
   x=s[1]; max1=(x>max1)?x:max1; min1=(x<min1)?x:min1;
   s+=2;  // advance to next set
  }while(--i>=0);
  while(n--){  // Do the remaining 64-word blocks
   // Every so often, coalesce the results & see if input maxrange has been exceeded
   max0=(max1>max0)?max1:max0; min0=(min1<min0)?min1:min0; if((UI)(max0-min0)>=(UI)maxrange)goto fail;
   i=(ASSESSBLOCKSIZE/4-1);  // # 4-groups-1
   do{  // This loop, which is always 64 words, will never mispredict
    x=s[0]; max0=(x>max0)?x:max0; min0=(x<min0)?x:min0; // this generates CMOVcc in VS
    x=s[1]; max1=(x>max1)?x:max1; min1=(x<min1)?x:min1;
    x=s[2]; max0=(x>max0)?x:max0; min0=(x<min0)?x:min0;
    x=s[3]; max1=(x>max1)?x:max1; min1=(x<min1)?x:min1;
    s+=4;  // advance to next set
   }while(--i>=0);
  }
 }
 // combine last results
 max0=(max1>max0)?max1:max0; min0=(min1<min0)?min1:min0; if((UI)(max0-min0)>=(UI)maxrange)goto fail;
 ret.min=min0; ret.range=(I)((UI)(max0-min0)+1);  // because the tests succeed, this will give the proper range
 R ret;
fail: ret.min=ret.range=0; R ret;
}
// Same for US types
CR condrange2(US *s,I n,I min,I max,I maxrange){CR ret;I i; US min0,min1,max0,max1;US x;
 // Unroll loop once to keep the compares rolling
 if(!n)goto fail;
 min0=min1=(US)min; max0=max1=(US)max;  // initial values
 if(n&1)min1=max1=*s++;  // if odd number of words, take first word to even it up
 if(n>>=1){  // n=#pairs of words left
  --n; i=n&31; n>>=5;  // do a block of compares to get on boundary; n=#64-words blocks left, i=size-1 of this block
  do{  // We keep this short loop separate because it always finishes with a misprediction.
   x=s[0]; max0=(x>max0)?x:max0; min0=(x<min0)?x:min0; // this generates CMOVcc in VS
   x=s[1]; max1=(x>max1)?x:max1; min1=(x<min1)?x:min1;
   s+=2;  // advance to next set
  }while(--i>=0);
  while(n--){  // Do the remaining 64-word blocks
   // Every so often, coalesce the results & see if input maxrange has been exceeded
   max0=(max1>max0)?max1:max0; min0=(min1<min0)?min1:min0; if((UI)(max0-min0)>=(UI)maxrange)goto fail;
   i=15;  // # 4-groups-1
   do{  // This loop, which is always 64 words, will never mispredict
    x=s[0]; max0=(x>max0)?x:max0; min0=(x<min0)?x:min0; // this generates CMOVcc in VS
    x=s[1]; max1=(x>max1)?x:max1; min1=(x<min1)?x:min1;
    x=s[2]; max0=(x>max0)?x:max0; min0=(x<min0)?x:min0;
    x=s[3]; max1=(x>max1)?x:max1; min1=(x<min1)?x:min1;
    s+=4;  // advance to next set
   }while(--i>=0);
  }
 }
 // combine last results
 max0=(max1>max0)?max1:max0; min0=(min1<min0)?min1:min0; if((UI)(max0-min0)>=(UI)maxrange)goto fail;
 ret.min=min0; ret.range=(I)((UI)(max0-min0)+1);  // because the tests succeed, this will give the proper range
 R ret;
fail: ret.min=ret.range=0; R ret;
}
#else  // the simpler non-unrolled version
static CR condrange(I *s,I n,I min,I max,I maxrange){CR ret;I i;I x;
 if(!n){ret.range=0; R ret;}; // return failure if no data
 if(min>max){min=max=*s++; --n;}  // init min & max to valid, so that only one changes at a time
 while(n){
  --n;i=n&63; n&=~63;  // do a block of compares
  do{x=*s++;
   // Use Conditional ops, which have latency of 2 but throughput of 1, so should result in 1 compare per cycle, which
   // is the best we can do, and doesn't rely on branch prediction.  Reassess if conditional ops get 2 ports (more likely,
   // use wide instructions)
   min=(x<min)?x:min; max=(x>max)?x:max;
  }while(--i>=0);
  if((UI)(max-min)>=(UI)maxrange){ret.range=0; R ret;}
 }
 ret.min=min; ret.range=1+max-min;  // 1+p-q can never be < 0, from the previous test
 R ret;
}
// Same for US types
static CR condrange2(US *s,I n,I min,I max,I maxrange){CR ret;I i;US x;
 if(!n){ret.range=0; R ret;}; // return failure if no data
 if(min>max){min=max=*s++; --n;}  // init min & max to valid, so that only one changes at a time
 while(n){
  --n;i=n&63; n&=~63;  // do a block of compares
  do{x=*s++;
   // Use Conditional ops, which have latency of 2 but throughout of 1, so should result in 1 compare per cycle, which
   // is the best we can do, and doesn't rely on barch prediction.  Reassess if conditional ops get 2 ports (more likely,
   // use wide instructions)
   min=(x<min)?x:min; max=(x>max)?x:max;
  }while(--i>=0);
  if((UI)(max-min)>=(UI)maxrange){ret.range=0; R ret;}
 }
 ret.min=min; ret.range=1+max-min;  // 1+p-q can never be < 0, from the previous test
 R ret;
}
#endif


// This is the routine that analyzes the input, allocates result area and hashtable, and vectors to the correct action routine
// jtioa* BOX
// jtiox  XNUM
// jtioq  RAT
// jtioi1 k==SZI, INT/SBT/char/bool not small-range
// jtioi  list of >1 INT
// jtioc  k!=SZI, bool (must be list of em)/char/INT/SBT
// jtioc01 intolerant FL atom
// jtioc0 intolerant FL array
// jtioz01 intolerant CMPX atom
// jtioz0 intolerant CMPX array
// jtiod  tolerant FL
// jtiod1 tolerant FL atom
// jtioz  tolerant CMPX
// jtioz1 tolerant CMPX atom
// Table to look up routine from index
// 0-11, 16-19 are hashes for various types, calculated by bit-twisting
// 32-51 are reverse hashes (i. e hash w, look up items of a)
#define FNTABLEPREFIX 2  // number of leading entries used for special types
#define FNTBLSMALL1 12  // small-range, 1-byte items
#define FNTBLSMALL2 13  // small-range, 2-byte items
#define FNTBLSMALL4 14  // small-range, 4-byte items
#define FNTBLONEINT 15  // hash of single INT-sized exact value
#define FNTBLBOXARRAY 20  // array of boxes, tolerant or not (we just hash on shape)
#define FNTBLBOXINTOLERANT 21  // single box but intolerant
#define FNTBLBOXUNIFORM 22  // single box, but a and w have uniform contents
#define FNTBLBOXUNKNOWN 23  // single box, but we can't do much with it.  Try hashing the first atom of contents
#define FNTBLXNUM 24   // hashed xnum
#define FNTBLRAT 25   // hashed rat
#define FNTBLBOXSSORT 26  // boxes, handled by sorting and binary search
#define FNTBLREVERSE 27  // where the reversed hashes start
#define FNTBLSIZE 54  // number of functions - before the second half
static const AF fntbl[]={
// prefix: routines used without hashtables, flags, etc
 jtiosc,  // sequential comparison (-2) - we pass in extra args
 jtiosfu,   // i.!.1 - sequential file update (-1)
// US tables
 jtioc,jtioc,jtioc,jtioc,jtioi,jtioi,jtioi,jtioi,  // bool, INT
 jtiod,jtioc0,jtiod1,jtioc01,jtio12,jtio22,jtio42,jtioi1,   // FL (then small-range, then ONEINT)
 jtioz,jtioz0,jtioz1,jtioz01,   // CMPX

 jtioa,jtioax1,jtioau,jtioa1,  // atomic types
 jtiox,jtioq,
 jtiobs,
 
 jtiowc,jtiowc,jtiowc,jtiowc,jtiowi,jtiowi,jtiowi,jtiowi,  // bool, INT
 0,jtiowc0,0,jtiowc01,0,0,jtio42w,jtiowi1,   // FL (then small-range, then ONEINT)
 0,jtiowz0,0,jtiowz01,   // CMPX
 0,0,0,0,
 0,0,
 0,

// UI4 tables
 jtioc2,jtioc2,jtioc2,jtioc2,jtioi2,jtioi2,jtioi2,jtioi2,  // bool, INT
 jtiod2,jtioc02,jtiod12,jtioc012,jtio14,jtio24,jtio44,jtioi12,   // FL (then small-range, then ONEINT)
 jtioz2,jtioz02,jtioz12,jtioz012,   // CMPX

 jtioa2,jtioax12,jtioau2,jtioa12,  // atomic types
 jtiox2,jtioq2,
 jtiobs,

 jtiowc2,jtiowc2,jtiowc2,jtiowc2,jtiowi2,jtiowi2,jtiowi2,jtiowi2,  // bool, INT
 0,jtiowc02,0,jtiowc012,0,0,jtio44w,jtiowi12,   // FL (then small-range, then ONEINT)
 0,jtiowz02,0,jtiowz012   // CMPX

};
static const S fnflags[]={  // 0 values reserved for small-range.  They turn off booladj
 IIMODFULL,IIMODFULL,IIMODFULL,IIMODFULL,IIMODFULL,IIMODFULL,IIMODFULL,IIMODFULL,  // bool, INT
 IIMODFULL,IIMODFULL,IIMODFULL,IIMODFULL,0,0,0,IIMODFULL,   // FL (then small-range, then ONEINT)
 IIMODFULL,IIMODFULL,IIMODFULL,IIMODFULL,   // CMPX

 IIMODFULL,IIMODFULL,IIMODFULL,IIMODFULL,  // atomic types
 IIMODFULL,IIMODFULL,
 -2,  // 'no hashing' (for box search)
 
// Reversed hashes, where supported.  IIMODFULL is not needed by the reversed-hash code so we continue its use, started above, as a flag to turn off booleans
 IREVERSED|IIMODFULL,IREVERSED|IIMODFULL,IREVERSED|IIMODFULL,IREVERSED|IIMODFULL,IREVERSED|IIMODFULL,IREVERSED|IIMODFULL,IREVERSED|IIMODFULL,IREVERSED|IIMODFULL,  // bool, INT
 IIMODFULL,IREVERSED|IIMODFULL,IIMODFULL,IREVERSED|IIMODFULL,IREVERSED,IREVERSED,IREVERSED,IIMODFULL,   // FL (then small-range, then ONEINT)
 IIMODFULL,IREVERSED|IIMODFULL,IIMODFULL,IREVERSED|IIMODFULL   // CMPX

};

#define MAXBYTEBOOL 65536  // if p exceeds this, we switch over to packed bits
#define COMPARESPERHASHWRITE 10  // writing to hash+range test cost is like this many compares
#define COMPARESPERHASHREAD 8   // consulting hash is like this many compares
#define OVERHEADHASHALLO 100  // clearing hash, calculating fnx costs this many compares
#define OVERHEADSHAPES 100  // checking shapes, types, etc costs this many compares

// mode indicates the type of operation, defined in j.h
A jtindexofsub(J jt,I mode,A a,A w){F2PREFIP;PROLOG(0079);A h=0;fauxblockINT(zfaux,1,0);
    I ac,acr,af,ak,an,ar,*as,at,datamin,f,f1,k,klg,n,r,*s,t,wc,wcr,wf,wk,wn,wr,*ws,wt,zn;UI c,m,p;
 ARGCHK2(a,w);

 // ?r=rank of argument, ?cr=rank the verb is applied at, ?f=length of frame, ?s->shape, ?t=type, ?n=#atoms
 // prehash is set if w argument is omitted (we are just prehashing the a arg)
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr; af=ar-acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;  // note: mark is an atom
 as=AS(a); at=AT(a); an=AN(a);
 ws=AS(w); wt=AT(w); wn=AN(w);
 // NOTE: from here on we may add modifiers to mode, indicating FULL/BITS/PACK etc.  These flags are needed in the action routine, and must be
 // preserved if the resulting hashtable is saved as part of a prehash.  They are not valid on input to this routine.
 if(unlikely(w==mark)){mode |= IPHCALC; f=af; s=as; r=acr-1; f1=wcr-r;}  // if w is omitted (for prehashing), use info from a
 else{  // w is given.  (i. e. not prehash)
  // IIOREPS indicates i./i:/e./key, which are candidates for reversed search and sequential search.  key will never cause a reversed search, but
  // it must not use sequential search if the comparison is inexact, because then it would conflict with nub and just generally fail because values not in
  // the nub could match later values leaving omitted values
  mode |= IIOREPS&((((((I)1)<<IIDOT)|(((I)1)<<IICO)|(((I)1)<<IEPS)|(((0x100000&((UI4*)&jt->cct)[1])>=(UI)((at)&(FL|CMPX|BOX)))<<IFORKEY))<<IIOREPSX)>>mode);  // remember if i./i:/e./key (and not prehash) /. is OK if a is not float
  // The comparison uses the fact that cct can never go above 1.0, which is 0x3ff0000000000000 in double precision.  To avoid integer-float conversions, we just strip out the bit that signifies
  // 1.0.  The expression then means 'tolerance=1.0 or intolerant comparison'
  // If the problem is small, use sequential search to save analysis and hashing time
  // TUNE  From testing 5/2021 on SkylakeX.  Hard to tell, since the data is brought into cache
#define SEQSEARCHIFALT 100  // use sequential search if a shorter than this
#define SEQSEARCHIFWLT 12  // use sequential search if w shorter than this
#define SEQSEARCHIFAWLT 200  // use sequential search if a+w shorter than this
  if((((an-SEQSEARCHIFALT)|(wn-SEQSEARCHIFWLT)|(an+wn-SEQSEARCHIFAWLT))<0)&&((ar^1)+TYPESXOR(at,wt))==0&&(((1-wr)|SGNIF(mode,ISFUX)|SGNIFNOT(mode,IIOREPSX)|SGNIFSPARSE(at|wt)|(-((acr^1)|(wr^wcr)))|(an-1)|(wn-1))>=0)){
   // Fast path for (vector i./i:/e./key atom or short vector) - if not prehashing.  Do sequential search
   I zt=((mode&IIOPMSK)==IEPS)?B01:INT;  // the result type depends on the operation.
   A z; GA(z,zt,wn,wr,ws);
   jtiosc(jt,mode,1,an,wn,1,1,a,w,z); // simple sequential search without hashing.
   RETF(z);
  }

  //  See if we need to abort owing to shapes.
  // ?r=rank of argument, ?cr=rank the verb is applied at, ?f=length of frame, ?s->shape, ?t=type, ?n=#atoms
  // prehash is set if w argument is omitted (we are just prehashing the a arg)
  f=af?af:wf; s=af?as:ws; r=acr?acr-1:0; f1=wcr-r;  // see below.  f1<0 here means cell of w has rank smaller than the item of a-cell, thus there are no matches possible
        // f1 is length of frame in w
  I f1clamp=REPSGN(f1); I disagree; TESTDISAGREE(disagree,as+af+1,ws+wf+(~f1clamp&f1),r)
  if(unlikely(f1clamp<disagree)){I f0,*v;A z;   // true if f1<0 OR disagree!=0
   // Dyad where shape of an item of a does not match shape of a cell of w.  Return appropriate not-found
   if(((af-wf)&-af)<0){f1+=wf-af; wf=af;}  // see below for discussion about long frame in w
   I witems=ws[0]; witems=wr>r?witems:1;  // # items of w, in case we are doing i.&0 eg on result of e., which will have that many items
   SETICFR(a,af,acr,m);  f0=MAX(0,f1); DPMULDE(prod(f,s),prod(f0,ws+wf),zn)
   switch(mode&IIOPMSK){
    case IIDOT:  
    case IICO:    GATV0(z,INT,zn,f+f0); MCISH(AS(z),s,f) MCISH(f+AS(z),ws+wf,f0); v=AV(z); DQ(zn, *v++=m;); R z;  // mustn't overfetch s
    case IEPS:    GATV0(z,B01,zn,f+f0); MCISH(AS(z),s,f) MCISH(f+AS(z),ws+wf,f0); mvc(zn,BAV(z),1,MEMSET00); R z;  // mustn't overfetch s
    case ILESS:                              RCA(w);
    case IINTER:                             R take(zeroionei(0),w);
    case IIFBEPS:                            R mtv;
    case IANYEPS: case IALLEPS: case II0EPS: R num(0);
    case ISUMEPS:                            R sc(0L);
    case II1EPS:  case IJ1EPS:               R sc(witems);
    case IJ0EPS:                             R sc(witems-1);
    case INUBSV:  case INUB:    case INUBI:  ASSERTSYS(0,"indexofsub"); // impossible 
   }
  }
 }
 // f is len of frame of a (or of w if a has no frame); s->shape of a (or of w is a has no frame)
 // r is rank of an item of a cell of a (i. e. rank of a target item), f1 is len of frame of A CELL OF w with respect to target cells, in
 // other words the frame of the results each cell of w will produce
 if(unlikely(ISSPARSE(at|wt))){A z;
  // Handle sparse arguments
  mode &= IIOPMSK;  // remove flags before going to sparse code
  if(1>=acr)R af?sprank2(a,w,0L,acr,RMAX,jtindexof):ISSPARSE(wt)?iovxs(mode,a,w):iovsd(mode,a,w);
  if(af|wf)R sprank2(a,w,0L,acr,wcr,jtindexof);
  switch((ISSPARSE(at)?2:0)+(ISSPARSE(wt)?1:0)){
   case 1: z=indexofxx(mode,a,w); break;
   case 2: z=indexofxx(mode,a,w); break;
   case 3: z=indexofss(mode,a,w); break;
  }
  EPILOG(z);
 }
 // Not sparse.
 // Calculate size of result.
 // m=target axis length, n=target item # atoms
 // c # target items in a left-arg cell, which may include multiple right-arg cells
 // k=target item # bytes, h->hash table or to 0   z=result   p=size of hashtable
 SETICFR(a,af,acr,m);  t=(mode&IPHCALC)?at:maxtyped(at,wt); klg=bplg(t);   // m=length of target axis; the common type; klg=lg of #bytes/atom of common type
 // Now that we have audited the shape of the cells of a/w to make sure they have commensurate items, we need to revise
 // the frame of w if it has the longer frame.  This can happen only where IRS is supported, namely ~: i. i: e. .
 // For those verbs, we get the effect of repeating a cell of a by having a macrocell of w, which is then broken into target-cell sizes.
 // We do this only if af!=0, because we have already set up to repeat cells of a if af=0
 if(unlikely(((af-wf)&-af)<0)){f1+=wf-af; wf=af;}  // wf>af & af>0
 if(likely(((an-1)|(wn-1))>=0)){
  // Neither arg is empty.  We can safely count the number of cells
  PROD(n,acr-1,as+af+1); k=n<<klg; // n=number of atoms in a target item; k=number of bytes in a target item
  PROD(ac,af,as); PROD(wc,wf,ws); PROD(c,MAX(f1,-1),ws+wf);  // ?c=#cells in a & w;  c=#target items (and therefore #result values) in a result-cell.  -1 so we don't fetch outside the shape
  DPMULDE(af?ac:wc,c,zn);   // #results is results/cell * number of cells; number of cells comes from ac if a has frame, otherwise w.  If both have frame, a's must be longer, use it
  ak=(m*k)&REPSGN(1-ac); wk=(c*k)&REPSGN(1-wc);   // # bytes in a cell, but 0 if there are 0 or 1 cells
  if(!af)c=zn;   // if af=0, wc may be >1 if there is w-frame.  In that case, #result/a-cell must include the # w-cells.  This has been included in zn
 }else{
  // An argument is empty.  We must beware of overflow in counting cells.  Just do it the old slow way
  n=acr?prod(acr-1,as+af+1):1; DPMULDE(prod(f,s),prod(f1,ws+wf),zn)
  k=n<<klg;
  ac=prod(af,as); ak=ac?(an<<klg)/ac:0;  // ac = #cells of a
  wc=prod(wf,ws); wk=wc?(wn<<klg)/wc:0; c=1<ac?wk/k:zn; wk*=1<wc;
 }

 // Convert dissimilar types
 I fnx;  // function to use, set below
 if(likely(TYPESEQ(at,wt))){fnx=0;
 }else{
  fnx=HOMONE(at,wt)?0:-2; /* noavx jt->min=0; */  // are args compatible?  -2 if not.  MARK is inhomo
  if((fnx|(TYPESXOR(t,at)-1))>=0)RZ(a=cvt(t|VFRCEXMT,a))  // convert if homo and TYPESXOR both nonzero
  if((fnx|(TYPESXOR(t,wt)-1))>=0)RZ(w=cvt(t|VFRCEXMT,w))
  fnx=mode&IPHCALC?0:fnx;  // if prehash, ignore the inhomo
 }

 // fnx is -2 if inhomo args, 0 otherwise
 fnx|=REPSGN(SGNIFNOT(mode,IPHCALCX)&(((I)m-1)|(n-1)|(zn-1)));
 // fnx is -2 if inhomo and not empty, -1 if any arg or result empty, 0 otherwise  but if precalc, we leave fnx at 0
 // Choose the function to use for performing the operation, in 'fnx'
 // fnx starts negative and is set to nonnegative when we know what function to use.
 // While fnx is negative, bit 0 means 'intolerant comparison'
 // If fnx is still negative at the end of the function search, it means either 'sequential search' (-1),
 // 'empty' (-2), or 'inhomo' (-3)
 // See if we should simply do sequential search.    We do this only for i./i:/e./key, only when w-cells match with a-cells or w has a single repeated cell
 // The cost of such a search is (4 inst per loop, at 1/2 cycle each) and the expected number of loops is half of
 // m*number of results.  The cost of small-range hashing is at best 10 cycles per atom added to the table and 8 cycles per lookup.
 // (full hashing is considerably more expensive); also a fair amount of time for range-checking and table-clearing, and further testing here
 // Here we just use the empirical observations that worked for atoms  TUNE
 if(unlikely(mode&ISFU)){fnx=fnx<0?fnx:1; fnx-=2;    // i.!.0 (qualified earlier for rank & type) - use special function, no hashtable.  But preserve codes -3 & -4 for inhomo/empty
 }else if((((((I)m-SEQSEARCHIFALT)|(zn-SEQSEARCHIFWLT)|((I)m+zn-SEQSEARCHIFAWLT)|fnx)<0)) && (((((-(wc^1))&(-(wc^ac)))|SGNIFNOT(mode,IIOREPSX))&~fnx)>=0)){   // wc==1 or ac and IOREPS, or empty/inhomo
          //  small enough operation, or  empty/inhomo   test size first because partially checked already & failed TUNE
    // this will not choose sequential search enough when the cells are large (comparisons then are cheap because of early exit)
  fnx-=2;  // now fnx is -4 for inhomo, -3 for empty, -1 for SFU, -2 for sequential.  This is ready for lookup.
//  jtiosc(jt,mode,n,m,c,ac,wc,a,w,z); // simple sequential search without hashing
 }else{
// jtioa* BOX
// jtiox  XNUM
// jtioq  RAT
// jtioi1  k=SZI, INT/SBT/char/bool not small-range
// jtioi  INT array
// jtioc  k=any, bool (must be list of em)/char/INT/SBT
// jtioc01 intolerant FL atom
// jtioc0 intolerant FL array
// jtioz01 intolerant CMPX atom
// jtioz0 intolerant CMPX array
// jtiod  tolerant FL
// jtiod1 tolerant FL atom
// jtioz  tolerant CMPX
// jtoiz1 tolerant CMPX atom

#define SMALLHASHCACHE L2CACHESIZE/sizeof(US)  // number of US entries that fit in L2
#define SMALLHASHMAX (131072-2-(2*SZI/sizeof(US))-((NORMAH*SZI+sizeof(IH))/sizeof(US)))  // max # US entries allowed in small hash.  More than 2^16 entries, to allow small-range expansion.
  // we allocate 4 extra entries to make sure we can write a quadword at the end, and to ensure there are sentinels

// testing#define HASHFACTOR 6.0  // multiple of p over m, found empirically
  fnx=-2 + (1.0==jt->cct); // we haven't figured it out yet.  Change meaning of fnx: fnx is -2 for tolerant, -1 for intolerant
  // p>>booladj is the number of hashtable entries we need.  booladj is 0 for full hash, 2 if we just need one byte-encoded boolean per input value (i. e. 4 bits per I4 hash entry), 5 if just one bit per input value (32 bits per hash entry)
  UI booladj=(mode&(IIOPMSK&~(IIDOT^IICO)))?5:0;  // boolean allowed when not i./i:
  p=0;  // indicate we haven't come up with the table size yet.  It depends on reverse and small-range decisions

  if(unlikely((t&BOX+XNUM+RAT)!=0)){
   if(t&BOX){I t1; fnx=(fnx&1)&&(1<n||usebs(a,ac,m))?FNTBLBOXSSORT:1<n?FNTBLBOXARRAY:(fnx&1)?FNTBLBOXINTOLERANT:
             (t1=utype(a,ac))&&((mode&IPHCALC)||a==w||TYPESEQ(t1,utype(w,wc)))?FNTBLBOXUNIFORM:FNTBLBOXUNKNOWN;
   }else fnx=CTTZ(t)+(FNTBLXNUM-XNUMX);
  }else if(unlikely(1==k)){p=t&B01?2:256;datamin=0; mode|=IIMODFULL; fnx=FNTBLSMALL1;}   // 1-byte ops, just use small-range code: checking takes too much time
  else{
   // We might switch over to small-range mode, if the sizes are right.  See how big the hash table would be for full hashing
   // figure out whether we should use small-range matching or hashing.  We use small-range code if:
   // type is exact and length is 1 or 2 bytes; or
   // type is exact numeric, length is 4 bytes or 8 bytes, and the (range of the data)*(length of rangecell) is less than 2.1*(length of data)*(length of hashcell)
   //  where length of rangecell=4 for i. or i:, 1/8 otherwise, length of hashcell=4
   // result is p (the length of hashtable, as # of entries), datamin (the minimum value found, if small-range)
   // If the allocated range includes all the possible values for the input, set IIMODFULL to indicate that fact
   if(unlikely(2==k)){
    // if the actual range of the data exceeds p, we revert to hashing.  All 2-byte types are exact
    CR crres = condrange2(USAV(a),(AN(a)<<klg)>>LGSZS,-1,0,MIN((UI)(IMAX-5)>>booladj,3*m)<<booladj);   // get the range
    if(crres.range){
      datamin=crres.min;
      // If the range is close to the max, we should consider widening the range to use the faster FULL code.  We do this only for boolean hashes, because
      // in the current allocation going all the way to 65536 kicks us into the longer hashtable (questionable decision).  Otherwise we should just promote
      // any non-Boolean, because the actual cache footprint won't change.
      // The cost of promoting a Boolean is 1 store (1 clock) per word cleared, for (65536-range)>>booladj bytes (if booladj!=0) [or (65536-range) hashtable entries if booladj==0]
      // The savings is 4 ops (2 clocks) per word searched
      if(booladj && ((UI)(65536-crres.range)>>booladj) < (c<<(LGSZI+1))){p=65536; datamin=0;}else{p=crres.range;}  // this underestimates the benefit for prehashes
      if(p==65536)mode|=IIMODFULL;
      fnx=FNTBLSMALL2;  // This qualifies for small-range processing
    }
   }
   if(likely(fnx<0)){  // if we don't have it yet, it will be a hash or small-range integers.  Decide which one
    if((k&~(t&FL))==SZI){  // non-float, might be INT or SBT, or characters.  FL has -0 problem   requires SZI==FL
     if(likely((t&INT+SBT)!=0)){I fnprov;A rangearg; UI rangearglen;  // same here, for I types
      // small-range processing is a possibility, but we need to decide whether we are going to do a reversed hash, so we will
      // know which range to check.  For i./i:, we reverse if c is much shorter than m; for e., we have to consider whether
      // the forward hash will benefit from bits mode, so we have to estimate the size of each hash table
      // if IIOREPS is not set, we have no support for the reversed-hash small-range, and we always look for small-range hash of a
      // otherwise (a candidate for reversed hash), if i./i: is set, meaning a full hashtable is needed, reverse if a is twice as long as w
      // otherwise (e., which uses bitmasks in the forward hash) calculate length of bitmask and reverse if the full table for w is shorter
      // than the bitmask for a.  Note that FORKEY will never cause a reversed hash
      rangearg=a; rangearglen=m; fnprov=FNTBLSMALL4;   // values for forward check
      if(mode&IIOREPS){  // if reverse check is possible, see if it is desired
       if((m>>(1))>c){rangearg=w; rangearglen=c; fnprov=FNTBLSMALL4+FNTBLREVERSE; }  // booladj?(m>MAXBYTEBOOL?5:2): omitted now
      }
      // we make the small-range decision mostly on length; if the range table would be bigger than the hashtable, we use the hash.  Here
      // we invert the calculation to see how big a range we can tolerate without exceeding the table size.  The length of the hash, whether small-range
      // or not, depends only on the number of items being hashed.  We limit the small-range to 3 times the # items;
      // but if this is a prehash, we expect that the hash will be used multiple times and we increase the size to weight the scales in favor of
      // small-range hash.  The full hash spends more time in lookup than in creation, because misses become more likely the
      // fuller the table.  This makes small-range much more valuable when the hashes are repeated
      I maxsizemult=mode&IPHCALC?6:4;  // # slots/item to allow in small-range table.  More if prehash
      CR crres = condrange(AV(rangearg),((AN(rangearg)<<klg))>>LGSZI,IMAX,IMIN,MIN((UI)(IMAX-5)>>booladj,maxsizemult*rangearglen)<<booladj);
      if(crres.range){datamin=crres.min; p=crres.range; fnx=fnprov;  // use the selected orientation
      }else{fnx=FNTBLONEINT;}  // select integer hashing if range too big...
     }else{fnx=FNTBLONEINT;}   // ... or some other 8-byte length (not float, though)
    }else{  // it's a hash
     fnx=((t&CMPX+FL+INT))+((n==1)?2:0)+fnx+2;  // index: CMPX/FL/n==1/intolerant (~fnx is 1 for tolerant, 0 for intolerant; fnx+2 is the reverse)
    }
   }
  }
  I fmods = fnflags[fnx];  // fetch flags for this type. -2 means no hash
  if(!p){  // If we selected small-range hashing, we have decided everything by analyzing the input; keep that.  Otherwise choose m or c depending on fwd/reverse hash
   // We know which type of hashing to use, and now we need to decide between forward and reverse hashing.  We use reverse hashing if the
   // type we have selected supports it, and if w is much shorter than a.  The operation must be i./i:/e., and we must not be prehashing
   p=m;
   if(((m>>1)>c) && (mode&IIOREPS) && fntbl[FNTABLEPREFIX+fnx+FNTBLREVERSE]){
    p=c; fnx+=FNTBLREVERSE;
    fmods = fnflags[fnx];  // refetch flags
   }
   // set p based on the length of the argument being hashed
   if(unlikely((SGNIF(t,B01X)&(k-(BW-1)))<0)){p=MIN(p,(UI)((I)1)<<k);}  // Get max # different possible values to hash; the number of items, but less than that for short booleans
   // Find the best hash size, based on empirical studies.  Allow at least 3x hashentries per input value; if that's less than the size of the small hash, go to the limit of
   // the small hash.  But not more than 10 hashtable entries per input (to save time clearing)
   {UI op=p*10; op=p>=SMALLHASHMAX/10?IMAX-5:op; p=p>(IMAX-5)/3?(IMAX-5)/3:p; p*=3; p=p<SMALLHASHMAX?SMALLHASHMAX:p; p=p>op?op:p;}
  }else if(unlikely((mode&IIOPMSK)==IFORKEY)){
   // We are processing on behalf of key, and we decided to do small-range processing.  Key can do better by
   // creating and processing the small-range table itself, so we will let it do that.  We return a special short block (LSB=1)
   // that indicates the length of the key (AN) and the start and range of the keys (AK and AM)
   A z; GAT0(z,INT,1,0); AN(z)=k; AK(z)=datamin; AM(z)=p;  // allocate and return
   RETF((A)((I)z+1));  // return the tagged address
  }
// testing  p = (UI)MIN(IMAX-5,(HASHFACTOR*p));  // length we will use for hashtable, if small-range not used

  // if a hashtable will be needed, allocate it.  It is NOT initialized
  // the hashtable is INT unless we have selected small-range hashing AND we are not looking for the index with i. or i:; then boolean is enough
  if(fmods>=0){IH * RESTRICT hh;
   if(unlikely(fmods!=0)){mode |= fmods; if(fmods&IIMODFULL)booladj=0;}   // If IMODFULL is required, bring it in; if not small-range, turn off bit mode

   // make sure we have a hashtable of the requisite size.  p has the number of entries, booladj indicates whether they are 1 bit each.
   // if the #entries fits in a US, use the short table.  But bits always use the long table

   // See if the size/range of w allows use of one of the faster loops.  The options are FULL (which saves the 4 instructions per atom of w that would
   // be spent range-checking w) and BASE0 (which clears the hashtable, at a cost of 1 cycle per 2/4 entries, or 4x that if we use fast instructions)
   // First check FULL, which is always the right decision if possible - except for self-classify which assumes FULL, or prehash which doesn't go through w at all
   // Don't bother to check if the decision has already been made (this will be set for full hashes, which ignore this bit and require FORCE0)
   if(unlikely(a!=w&&!(mode&(IIMODFULL|IPHCALC|IREVERSED)))){CR crres;
    // We can get here only if IIMODFULL is off, which happens only if fmods is 0, which means we are doing a forward small-range hash.  In addition, the
    // number of bytes in an item will be 2 or SZI.  We must convert the # atoms to a # of 2- or SZI-sized pieces
    I allowrange;  // where we will build the max allowed range of w
    if(likely((h=jt->idothash1)!=0)){allowrange=IHAV(h)->datasize>>IHAV(h)->hashelelgsize;}else{allowrange=0;}  // current max capacity of large hash
    // always allow a little bit larger than the range of a, to make sure we expand the hashtable if a little more would be enough.
    // but never increase the range if that would exceed the L2 cache - just pay the 4 instructions
    if(unlikely(k==2)){
     allowrange=MIN(MAX(L2CACHESIZE>>LGSZUS,(I)p),MAX(allowrange,(I)(p+(p>>3))));  // allowed range, with expansion
     crres = condrange2(USAV(w),(AN(w)<<klg)>>LGSZS,datamin,datamin+p-1,allowrange);
    }else{
     allowrange=MIN(MAX(L2CACHESIZE>>LGSZUI4,(I)p),MAX(allowrange,(I)(p+(p>>3))));  // allowed range, with expansion
     crres = condrange(AV(w),(AN(w)<<klg)>>LGSZI,datamin,datamin+p-1,allowrange);
    }
    if(crres.range){datamin=crres.min; p=crres.range; mode |= IIMODFULL;}
   }  
    if((((I)p-(I)SMALLHASHMAX) & ((I)booladj-1) & ((I)((mode&IREVERSED)?c:m)-65535))<0){  // range must fit into address; hash index+1 must fit into the 16-bit entry
      // (the +1 is in case we do reversed hash where we invalidate by writing m+1; shouldn't have small hash if m=65535, but take no chances)
    // using the short table.  Allocate it if it hasn't been allocated yet, or if this is prehashing, where we will build a separate table.
    // It would be nice to use the main table for m&i. to avoid having to clear a custom table, since m&i. may never get assigned to a name;
    // but if it IS assigned, the main table may be too big, and we don't have any good way to trim it down
    // If the sizes are such that we should clear this table to save 3 clocks per atom of w, say so.  The clearing is done in hashallo (or on allocation here, for prehashing)
    // Clearing also saves 1 clock per input word.
    // The -1 is to make sure we have 1 entry at the end of the small-range table to store wsct
#if 0  // now that we have wide instructions, it always makes sense to clear the allocated area
    mode |= ((c*3+m)<(p>>(LGSZI-LGSZUS-1)))<<IIMODFORCE0X;  // 3 cycles per atom of w, 1 cycle per atom of m, versus 2/4 cycle per atom to clear (without wide insts)
#else
    mode |= IIMODFORCE0;
#endif
    // we can use the small table.  See if there already is one we can use, otherwise allocate one
    if(unlikely((REPSGN(SGNIFNOT(mode,IPHCALCX))&(I)(h=jt->idothash0))==0)){   // precalc, or  small table doesn't exist
     GATV0(h,INT,((SMALLHASHMAX*sizeof(US)+SZI+(SZI-1))>>LGSZI),0);  // size too big for GAT
     // Fill in the header
     hh=IHAV(h);  // point to the header
     hh->datasize=allosize(h)-sizeof(IH);  // number of bytes in data area
     hh->hashelelgsize=1;  // hash entries are 2 bytes long
     hh->currenthi = hh->previousindexend = 0;  // This is the minimum need to initialize when FORCE0 is set
     if(likely(!(mode&IPHCALC))){ACINITZAP(h); jt->idothash0=h;}  // If not prehashing a table, save this and protect against removal.
    }
   }else{
    // using the long table.  Use the current one if it is long enough; otherwise allocate a new one
    // First, make a decision for Boolean tables.  If the table will be Boolean, decide whether to use packed bits
    // or bytes, and represent that information in mode and booladj.
    ASSERT((UI)m<=(UI)(UI4)-1,EVLIMIT);  // there must not be more items than we can distinguish with different hash indexes
    if(unlikely(booladj!=0)){if(unlikely(p>MAXBYTEBOOL)){mode|=IIMODPACK|IIMODBITS;}else{mode|=IIMODBITS;booladj=5-3;}  // set MODBITS as a flag to hashallo   unlikely fails on single word
    }else{
#if 0  // now that we have wide instructions, it always makes sense to clear the allocated area
     // If the sizes are such that we should clear this table to save 3 clocks per atom of w, say so.  The clearing is done in hashallo.  Only for non-bits.
     mode |= ((c*3+m)<(p<<(1-(LGSZI-LGSZUI4))))<<IIMODFORCE0X;  // 3 cycles per atom of w, 1 cycle per atom of m, versus 2/2 cycle per atom to clear (without wide insts)
#else
     mode |= IIMODFORCE0;
#endif     
    }
    I psizeinbytes = ((p>>booladj)+4)*sizeof(UI4);   // Get length of table in bytes.  We add 4 to the request:
         // for small-range to round up to an even word of an I, and possibly padding leading/trailing bytes; for hashing, we need a sentinel at the beginning and the end
    if(unlikely(!(h=jt->idothash1) || ((SGNIF(mode,IPHCALCX)|(IHAV(h)->datasize-psizeinbytes))<0))){   // no old table, or we have to create one for prehash, or old table not big enough
     // if we have to reallocate, free the old one
     if(unlikely((REPSGN(SGNIFNOT(mode,IPHCALCX))&(I)h)!=0)){fr(h); jt->idothash1=0;}  // free old, and clear pointer in case of allo error
     // allocate the new one and fill it in
     GATV0(h,INT,(psizeinbytes+sizeof(IH)+(SZI-1))>>LGSZI,0);
     // Fill in the header
     hh=IHAV(h);  // point to the header
     hh->datasize=allosize(h)-sizeof(IH);  // number of bytes in data area
     hh->hashelelgsize=2;  // hash entries are 4 bytes long
     hh->currenthi = hh->previousindexend = 0;  // This is the minimum need to initialize when FORCE0 is set
     // If the hash size is moderate, there is a gain to be had by preserving it between searches (it will already be in cache).  On the other hand,
     // it would be a shame to tie up vast amounts of memory waiting for a large search.  To compromise, we keep the buffer unless it is much bigger than the L3 cache
     if(likely((SGNIFNOT(mode,IPHCALCX)&(hh->datasize-5*L3CACHESIZE))<0)){ACINITZAP(h); jt->idothash1=h;}  // If not prehashing a table, save this and protect against removal if not prehash and table not huge
    }
    // switch the routine pointer to the big table
    fnx+=FNTBLSIZE;
   }
   // Pass the min/range into the action routine, using result values in the hashtable
   hh=IHAV(h); hh->datamin=datamin; hh->datarange=p;  // max will be inferred
  }else{h = (A)acr;}  // if there is no hashtable, pass in acr using the h field.  This is for sorted boxes only
 }  // end of 'not sequential comparison' which means we may need a hashtable

 AF ifn=fntbl[FNTABLEPREFIX+fnx];  // get an early start fetching the function we will call

 // Allocate the result area.  NOTE that some of the routines, like small-range, always store at least one result; so we have to point z somewhere harmless before launching them. If we are prehashing we skip this.
 // If the conditions are right, perform the operation inplace
 A z;
 switch(mode&(IPHCALC|IIOPMSK)){
 default:      fauxINT(z,zfaux,1,0) break;   // if prehashed, we must create an area that can hold at least one stored result
 case IIDOT: case IFORKEY:
 case IICO:    GATV0(z,INT,zn,f+f1); MCISH(AS(z),s,f) MCISH(f+AS(z),ws+wf,f1); break;  // mustn't overfetch s
 case INUBSV:  GATV0(z,B01,zn,f+f1+!acr); MCISH(AS(z),s,f) MCISH(f+AS(z),ws+wf,f1); if(!acr)AS(z)[AR(z)-1]=1; break;  // mustn't overfetch s
 case INUB:    {I q; PRODX(q,AR(a)-1,AS(a)+1,MIN(m,p)+1) GA(z,t,q,MAX(1,wr),ws); break;}  // +1 because we speculatively overwrite.
 case ILESS: case IINTER:   ws=wr==0?&AN(w):ws; GA(z,AT(w),AN(w),MAX(1,wr),ws); break;  // if wr is an atom, use 1 for the shape
 case IEPS:    GATV0(z,B01,zn,f+f1); MCISH(AS(z),s,f) MCISH(f+AS(z),ws+wf,f1); break;
 case INUBI:   GATV0(z,INT,MIN(m,p)+1,1); break;  // +1 because we speculatively overwrite
 // (e. i. 0:) and friends don't do anything useful if e. produces rank > 1.  The search for 0/1 always fails
 case II0EPS: case II1EPS: case IJ0EPS: case IJ1EPS:
               if(wr>MAX(ar,1))R sc(wr>r?ws[0]:1); GAT0(z,INT,1,0); break;
 // ([: I. e.) ([: +/ e.) ([: +./ e.) ([: *./ e.) come here only if e. produces rank 0 or 1.
 case IIFBEPS: GATV0(z,INT,c+1,1); break;  // +1 because we speculatively overwrite
 case IANYEPS: case IALLEPS:
               GAT0(z,B01,1,0); break;
 case ISUMEPS:
               GAT0(z,INT,1,0); break;
 }

 // ************* ws has been destroyed *******************

 // Create result for empty/inhomogeneous arguments, & return
 if(unlikely(fnx<-2)){  // if something empty (-3) or inhomo (-4), create the result immediately
  I witems; SETICFR(w,0,wr>r,witems);   // # items of w, in case we are doing i.&0 eg on result of e., which will have that many items
  switch(mode&(IIOPMSK)){  // if PHCALC, we never got here
  // If empty argument or result, or inhomogeneous arguments, return an appropriate empty or not-found
  // We also handle the case of i.&0@:e. when the rank of w is more than 1 greater than the rank of a cell of a;
  // in that case the search always fails
  case IIDOT:   R reshape(shape(z),sc(n?m:0  ));
  case IFORKEY: {z=reshape(shape(z),take(sc(m),sc(m))); RZ(z=mkwris(z)); AM(z)=!!m; R z;}  // all 0 but the first has the total count.  Must install # partitions=1 if #items>0
  case IICO:    R reshape(shape(z),sc(n?m:m-1));
  case INUBSV:  R reshape(shape(z),take(sc(m),num(1)));
  case INUB:    AN(z)=0; AS(z)[0]=m?1:0; R z;
  case ILESS:   if(m&&fnx==-3)AN(z)=AS(z)[0]=0; else MC(AV(z),AV(w),AN(w)<<bplg(AT(w))); R z;
  case IINTER:  if(!(m&&fnx==-3))AN(z)=AS(z)[0]=0; R z;  // y has atoms or something is empty, return all of w; otherwise empty
  case IEPS:    R reshape(shape(z),num(m&&(!n||(fnx&1))));  // fnx&1 is true if homo
  case INUBI:   R m?iv0:mtv;
  // th<0 means that the result of e. would have rank>1 and would never compare against either 0 or 1
  case II0EPS:  R sc(n&&zn?0L        :witems         );
  case II1EPS:  R sc(n&&zn?witems         :0L        );
  case IJ0EPS:  R sc(n&&zn?MAX(0,witems-1):witems         );
  case IJ1EPS:  R sc(n&&zn?witems         :MAX(0,witems-1));
  case ISUMEPS: R sc(n?0L        :c         );  // must include shape of w
  case IANYEPS: R num(!n);
  case IALLEPS: R num(!(c&&n));
  case IIFBEPS: R n?mtv :IX(c);
  }
 }

 // Call the routine to perform the operation
 RZ(h=ifn(jt,mode,n,m,c,ac,wc,a,w,z,k,ak,wk,h));
//  RZ(h=fntbl[fnx](jt,mode,n,m,c,ac,wc,a,w,z,k,ak,wk,h));
 // If the call was IFORKEY, the number of partitions was stored in AM(h).  Move it to AM(z) whence it will be returned.
 I forkeyresult=AM(h);  // save # partitions

 if(unlikely((mode&IPHCALC)!=0)){A x,*zv;I*xv;
  // If w was omitted (indicating prehashing), return the information for that special case
  // result is an array of 3 boxes, containing (info vector),(hashtable),(mask of hashed bytes if applicable)
  // The caller must ras() this result to protect it, if it is going to be saved
  GAT0(z,BOX,2,1); zv=AAV(z);
  GAT0(x,INT,6,1); xv=AV(x);
  xv[0]=mode; xv[1]=n; xv[2]=k; /* noavx xv[3]=jt->min; */ xv[4]=(I)fntbl[FNTABLEPREFIX+fnx]; /* xv[5]=ztypefromitype[mode&IIOPMSK]; */
  zv[0]=incorp(x); zv[1]=incorp(h);
 }
 RZ(z=EPILOGNORET(z));
 // Since EPILOG may have rewritten AM, and IFORKEY never returns to the parser, we can store the FORKEY result in AM.
 *((mode&IIOPMSK)==IFORKEY?(I*)&AM(z):(I*)&jt->shapesink)=forkeyresult;
 RETF(z);
}    /* a i."r w main control */

// verb to vector combine@e. compounds.  The i. code is in the self
// because these are e. compounds we swap a and w
DF2(jtcombineeps){ARGCHK3(a,w,self);R indexofsub(II0EPS+((FAV(self)->flag>>3)&7),w,a);}

// verb to execute compounds like m&i. e.&n .  m/n has already been hashed and the result saved away
// a is the arg that was indexed, used only if we have to revert to rerunning the operation
// w is the arg to be applied to the index
// hs is the hashtable
A jtindexofprehashed(J jt,A a,A w,A hs,A self){A h,*hv,x,z;AF fn;I ar,*as,at,c,f1,k,m,mode,n,
     r,t,*xv,wr,*ws,wt;
 ARGCHK3(a,w,hs);
 // hv is (info vector);(hashtable);(byte index validity)
 hv=AAV(hs); x=hv[0]; h=hv[1]; 
 // get the info from the info vector
 xv=AV(x); mode=xv[0]; n=xv[1]; k=xv[2]; /* noavx jt->min=xv[3]; */ fn=(AF)xv[4];  // n=#atoms in item of table, k=#bytes
 ar=AR(a); as=AS(a); at=AT(a); t=at; SETIC(a,m); 
 wr=AR(w); ws=AS(w); wt=AT(w);
 r=ar?ar-1:0;   // r=rank of a hashed item
 f1=wr-r;   // frame of w wrt items of a.  Negative if w's cells are too big
 PRODX(c,f1,ws,1);  // c=#cells of w (and result)
 // audit conformance of input shapes.  If there is an error, pass to the main code to get the error result
 // Use c=0 as an error flag
 // for e.-compounds that do arithmetic, w must have the shape of an item in the hash, or a list of them (i. e. framelen 0 or 1); otherwise nonce error (below)
 // for e.-compounds that merely search, w must have the shape of an item in the hash, or a list of them (i. e. framelen 0 or 1); otherwise not found (below)
 // for LESS/INTER types, an item of w must have the shape of an item in the hash; otherwise revert
 // for other types (which can be only IEPS/IIDOT/IICO), an item of w can have larger rank
 if(likely((c&=REPSGN(~f1))>0)){  // revert if w has higher rank than a cell of a
  c=ICMP(as+ar-r,ws+f1,r)?0:c;  // verify agreement in cell-shape, set c=0 if not
  if(((I)1<<(mode&IIOPMSK))&(((I)1<<ILESS)|((I)1<<IINTER))){
   if(f1<(wr!=0)||f1>1){
    // LESS/INTER where the hashtable has the wrong cell-rank.  Revert
    // LESS/INTER cannot revert simply by calling indexofsub, because a has to be reshaped to make the cell-rank match the item of w.
    R (mode&IIOPMSK)==ILESS?less(w,a):jtintersect(jt,w,a,self);
   }
  }
 }
 c=typeged(t,wt)?c:0;   // OK to use hash if w doesn't have higher precision
 // If there is any error, switch back to the non-prehashed code.  We must remove any command bits from mode, leaving just the operation type
 if(unlikely((-m&-n&-c&NEGIFHOMO(t,wt))>=0))R indexofsub(mode&IIOPMSK,a,w);  // empty in a or w, or inhomo, or w has higher precision than the hash

 // convert type of w if needed
 if(TYPESNE(t,wt))RZ(w=cvt(t,w))
 // call the action routine

 // allocate enough space for the result, depending on the type of the operation
 switch(mode&IIOPMSK){
  // Some types that do not produce correct results if the result of e. has rank >1.  We give nonce error if that happens
  default: GATV(z,INT,c,f1,ws); break;
  case ILESS: case IINTER: GA(z,t,AN(w),MAX(1,wr),wr==0?&AN(w):ws); break;
  case IIFBEPS: ASSERT(wr<=MAX(ar,1),EVNONCE); GATV(z,INT,c,f1,ws); break;
  case IEPS: GATV(z,B01,c,f1,ws); break;
  case IANYEPS: case IALLEPS: ASSERT(wr<=MAX(ar,1),EVNONCE); GAT0(z,B01,1,0); break;
  case II0EPS:  case II1EPS: case IJ0EPS:  case IJ1EPS: if(wr>MAX(ar,1))R sc(wr>r?ws[0]:1); GAT0(z,INT,1,0); break;
  case ISUMEPS: ASSERT(wr<=MAX(ar,1),EVNONCE); GAT0(z,INT,1,0); break;
 }
 // save info used by the routines
 // noavx jt->hin=AN(hi); jt->hiv=AV(hi);
 RZ(fn(jt,mode+IPHOFFSET,n,m,c,(I)1,(I)1,a,w,z,k,(I)0,(I)0,h))
 R z;
}

// Now, support for the primitives that use indexof

// x i. y
F2(jtindexof){R indexofsub(IIDOT,a,w);}
     /* a i."r w */

// x i: y
F2(jtjico2){R indexofsub(IICO,a,w);}
     /* a i:"r w */

// ~: y
F1(jtnubsieve){
 ARGCHK1(w);
 if(unlikely(ISSPARSE(AT(w))))R nubsievesp(w); 
 jt->ranks=(RANKT)jt->ranks + ((RANKT)jt->ranks<<RANKTX);  // we process as if dyad; make left rank=right rank
 R indexofsub(INUBSV,w,w); 
}    /* ~:"r w */

// ~. y  - does not have IRS
F1(jtnub){ 
 F1PREFIP;ARGCHK1(w);
 if(unlikely((SGNIFSPARSE(AT(w))|SGNIF(AFLAG(w),AFNJAX))<0))R repeat(nubsieve(w),w);    // sparse or NJA
 A z; RZ(z=indexofsub(INUB,w,w));
 // We extracted from w, so mark it (or its backer if virtual) non-pristine.  If w was pristine and inplaceable, transfer its pristine status to the result.  We overwrite w because it is no longer in use
 PRISTXFERF(z,w)
 RETF(z);
}    /* ~.w */

// x -. y.  does not have IRS
F2(jtless){A x=w;I ar,at,k,r,*s,wr,*ws,wt;
 F2PREFIP;ARGCHK2(a,w);
 at=AT(a); ar=AR(a); 
 wt=AT(w); wr=AR(w); r=MAX(1,ar);
 if(unlikely(ar>1+wr))RCA(a);  // if w's rank is smaller than that of a cell of a, nothing can be removed, return a
 // if w's rank is larger than that of a cell of a, reheader w to look like a list of such cells
 if(unlikely((-wr&-(r^wr))<0)){RZ(x=virtual(w,0,r)); AN(x)=AN(w); s=AS(x); ws=AS(w); k=ar>wr?0:1+wr-r; I s0; PRODX(s0,k,ws,1) s[0]=s0; MCISH(1+s,k+ws,r-1);}  //  use fauxvirtual here
 // if nothing special (like sparse, or incompatible types, or x requires conversion) do the fast way; otherwise (-. x e. y) # x 
 // because LESS allocates a large array to hold all the values, we use the slower, less memory-intensive, version if a is mapped
 RZ(x=(SGNIFSPARSE(at)|SGNIF(AFLAG(a),AFNJAX))>=0?indexofsub(ILESS,x,a):
     repeat(not(eps(a,x)),a));
 // We extracted from a, so mark it (or its backer if virtual) non-pristine.  If a was pristine and inplaceable, transfer its pristine status to the result
 PRISTXFERAF(x,a)
 RETF(x);
}    /* a-.w */

// x ([ -. -.[!.f]) y.  does not have IRS
DF2(jtintersect){A x=w;I ar,at,k,r,*s,wr,*ws,wt;
 F2PREFIP;ARGCHK2(a,w);
 at=AT(a); ar=AR(a); 
 wt=AT(w); wr=AR(w); r=MAX(1,ar);
 if(unlikely(ar>1+wr))R take(zeroionei(0),a);  // if w's rank is smaller than that of a cell of a, nothing can be common, return no items
 // if w's rank is larger than that of a cell of a, reheader w to look like a list of such cells
 if(unlikely((-wr&-(r^wr))<0)){RZ(x=virtual(w,0,r)); AN(x)=AN(w); s=AS(x); ws=AS(w); k=ar>wr?0:1+wr-r; I s0; PRODX(s0,k,ws,1) s[0]=s0; MCISH(1+s,k+ws,r-1);}  //  use fauxvirtual here
 // comparison tolerance may be encoded in h - apply it if so
 D savcct = jt->cct;
 PUSHCCTIF(FAV(self)->localuse.lu1.cct,FAV(self)->localuse.lu1.cct!=0)   // if there is a CT, use it
 // if nothing special (like sparse, or incompatible types, or x requires conversion) do the fast way; otherwise (-. x e. y) # x 
 // because LESS allocates a large array to hold all the values, we use the slower, less memory-intensive, version if a is mapped
 // Don't revert to fork!  localuse.lu1.fork2hfn is not set
 x=(SGNIFSPARSE(at)|SGNIF(AFLAG(a),AFNJAX))>=0?indexofsub(IINTER,x,a):
     repeat(eps(a,x),a);
 POPCCT
 RZ(x);
 // We extracted from a, so mark it (or its backer if virtual) non-pristine.  If a was pristine and inplaceable, transfer its pristine status to the result
 PRISTXFERAF(x,a)
 RETF(x);
}    /* a-.w */

// x e. y
F2(jteps){I l,r;
 ARGCHK2(a,w);
 l=jt->ranks>>RANKTX; l=AR(a)<l?AR(a):l;
 r=(RANKT)jt->ranks; r=AR(w)<r?AR(w):r; RESETRANK;
 if(unlikely(ISSPARSE(AT(a)|AT(w))))R lt(irs2(w,a,0L,r,l,jtindexof),sc(r?AS(w)[AR(w)-r]:1));  // for sparse, implement as (# cell of y) > y i. x
 jt->ranks=(RANK2T)((r<<RANKTX)+l);  // swap ranks for subroutine.  Subroutine will reset ranks
 R indexofsub(IEPS,w,a);
}    /* a e."r w */

// I.@~: y   does not have IRS
F1(jtnubind){
 ARGCHK1(w);
 R ISSPARSE(AT(w))?icap(nubsieve(w)):indexofsub(INUBI,w,w);
}    /* I.@~: w */

// i.@(~:!.0) y     does not have IRS
F1(jtnubind0){A z;
 ARGCHK1(w);
 PUSHCCT(1.0) z=ISSPARSE(AT(w))?icap(nubsieve(w)):indexofsub(INUBI,w,w); POPCCT
 R z;
}    /* I.@(~:!.0) w */

// x i.!.1 y - assumes xy -: /:~ xy (integer atoms only for now)
F2(jtsfu){
 ARGCHK2(a,w);
 I type=ISFU+IIDOT; type=((NOUN|SPARSE)&~(INT))&(AT(a)|AT(w))?IIDOT:type;
 I l=jt->ranks>>RANKTX; l=AR(a)<l?AR(a):l; type=l!=1?IIDOT:type; // If the cells of a are not atoms, we revert to standard methods
 R indexofsub(type,a,w);
}    /* a i.!.1"r w */

// = y    
F1(jtsclass){A e,x,xy,y,z;I c,j,m,n,*v;P*p;
 ARGCHK1(w);
 // If w is scalar, return 1 1$1
 if(!AR(w))R reshape(v2(1L,1L),num(1));
 SETIC(w,n);   // n=#items of y
 RZ(x=indexof(w,w));   // x = i.~ y
 // if w is dense, return ((x = i.n) # x) =/ x
 if(!ISSPARSE(AT(w)))R atab(CEQ,repeat(eq(IX(n),x),x),x);
 // if x is sparse... ??
 p=PAV(x); e=SPA(p,e); y=SPA(p,i); RZ(xy=stitch(SPA(p,x),y));
 if(n>AV(e)[0])RZ(xy=over(xy,stitch(e,less(IX(n),y))));
 RZ(xy=grade2(xy,xy)); v=AV(xy);
 c=AS(xy)[0];
 m=j=-1; DQ(c, if(j!=*v){j=*v; ++m;} *v=m; v+=2;);
 GASPARSE0(z,B01,1,2); v=AS(z); v[0]=1+m; v[1]=n;
 p=PAV(z); 
 SPB(p,a,v2(0L,1L));
 SPB(p,e,num(0));
 SPB(p,i,xy);
 SPB(p,x,reshape(sc(c),num(1)));
 R z;
}


// support for a i."1 &.|:w or a i:"1 &.|:w

// function definition
#define IOCOLF(f)     void f(J jt,I asct,I wsct,I d,A a,A w,A z,A h,UIL ctmask)
#define IOCOLDECL(T)  D tl=jt->cct,tr=1/tl,x;                           \
                          I hj,*hv=AV(h),i,j,jr,l,p=AN(h),*u,*zv=AV(z);  \
                          T*av=(T*)AV(a),*v,*wv=(T*)AV(w);UI pm=p

// start searching at index j, and stop when j points to a slot that is empty, or for which exp is false
// (exp is a test for not-equal, normally referring to v (the current element being hashed) and hv (the data field for
// the first block that hashed to this address)
#define FIND(exp) NOUNROLL while(asct>(hj=hv[j])&&(exp)){if(unlikely(--j<0))j+=p;}

// function to search forward
// T is the type of the data
// f is function name
// hasha is the hash to use for the values from a
// hashl and hashr are the hashes to use for one tolerance below & above
// exp is a test for not-equal, returning 1 when values do not match
// For each column, clear the hash table, then hash the items of a, then look up the items of w 
#define IOCOLFT(T,f,hasha,hashl,hashr,exp)  \
 IOCOLF(f){IOCOLDECL(T);                                                   \
  for(l=0;l<wsct;++l){                                                        \
   DO(p, hv[i]=asct;);                                                        \
   v=av;         DO(asct, j=(hasha)%pm; FIND(exp); if(asct==hj)hv[j]=i; v+=wsct;);  \
   v=wv; u=zv;                                                             \
   for(i=0;i<d;++i){                                                       \
    x=*(D*)v;                                                              \
    j=jr=(hashl)%pm;           FIND(exp); *u=hj;                           \
    j=   (hashr)%pm; if(j!=jr){FIND(exp); *u=MIN(*u,hj);}                  \
    v+=wsct; u+=wsct;                                                            \
   }                                                                       \
   ++av; ++wv; ++zv;                                                       \
  } \
 }

// Similar, but search backward
#define JOCOLFT(T,f,hasha,hashl,hashr,exp)  \
 IOCOLF(f){IOCOLDECL(T);I q;                                               \
  for(l=0;l<wsct;++l){                                                        \
   DO(p, hv[i]=asct;);                                                        \
   v=av+wsct*(asct-1); DQ(asct, j=(hasha)%pm; FIND(exp); if(asct==hj)hv[j]=i; v-=wsct;);  \
   v=wv; u=zv;                                                             \
   for(i=0;i<d;++i){                                                       \
    x=*(D*)v;                                                              \
    j=jr=(hashl)%pm;           FIND(exp); *u=q=hj;                         \
    j=   (hashr)%pm; if(j!=jr){FIND(exp); if(asct>hj&&(hj>q||q==asct))*u=hj;}    \
    v+=wsct; u+=wsct;                                                            \
   }                                                                       \
   ++av; ++wv; ++zv;                                                       \
  } \
 }

// create the index-of routines.  These hash just the real part of a complex value
static IOCOLFT(D,jtiocold,cthid(ctmask,*v),    cthid(ctmask,tl*x),cthid(ctmask,tr*x),!TEQ(*v,av[wsct*hj]))
static IOCOLFT(Z,jtiocolz,cthid(ctmask,*(D*)v),cthid(ctmask,tl*x),cthid(ctmask,tr*x),!zeq(*v,av[wsct*hj]))

static JOCOLFT(D,jtjocold,cthid(ctmask,*v),    cthid(ctmask,tl*x),cthid(ctmask,tr*x),!TEQ(*v,av[wsct*hj]))
static JOCOLFT(Z,jtjocolz,cthid(ctmask,*(D*)v),cthid(ctmask,tl*x),cthid(ctmask,tr*x),!zeq(*v,av[wsct*hj]))

// support for a i."1 &.|:w or a i:"1 &.|:w   used only by some sparse-array stuff
A jtiocol(J jt,I mode,A a,A w){A h,z;I ar,at,c,d,m,p,t,wr,*ws,wt;void(*fn)();
 ARGCHK2(a,w);
 // require ct!=0   why??
 ASSERT(1.0!=jt->cct,EVNONCE);
 at=AT(a); ar=AR(a); m=AS(a)[0]; c=aii(a);  // a: at=type ar=rank m=#items c=#atoms in an item
 wt=AT(w); wr=AR(w); ws=AS(w); 
 d=1; DO(1+wr-ar, d*=ws[i];);
 // convert to common type
 RE(t=maxtype(at,wt));
 if(TYPESNE(t,at))RZ(a=cvt(t,a));
 if(TYPESNE(t,wt))RZ(w=cvt(t,w));
 // allocate hash table and result
 FULLHASHSIZE(m+m,INTSIZE,1,0,p);
 GATV0(h,INT,p,1);
 GATV(z,INT,AN(w),wr,ws);
 // call routine based on types.  Only float and CMPX are supported
 UIL ctmask;
 switch(t){
  default:   ASSERT(0,EVNONCE);     
  case FL:   fn=mode==IICO?jtjocold:jtiocold; ctmask=calcctmask(jt->cct); break;
  case CMPX: fn=mode==IICO?jtjocolz:jtiocolz; ctmask=calcctmask(jt->cct); break;
 }
 fn(jt,m,c,d,a,w,z,h,ctmask);
 R z;
}    /* a i."1 &.|:w or a i:"1 &.|:w */
#endif // C_AVX
