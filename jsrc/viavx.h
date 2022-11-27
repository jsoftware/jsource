// Common routines for i.-family verbs

#include "vcomp.h"

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
#define HASHSLOT(hash) j=((hash)*p)>>32;

// Misc code to set the shape once we see how many results there are, used for ~. y and x -. y
#define ZISHAPE    AS(z)[0]=AN(z)=zi-zv
#define ZCSHAPE    AS(z)[0]=(zc-(C*)zv)/k; AN(z)=n*AS(z)[0]
#define ZUSHAPE(T) AS(z)[0]= zu-(T*)zv;    AN(z)=n*AS(z)[0]

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

// b is a bit index; BYTENO is byte number, BITNO is bit within byte
#define BYTENO(b) ((b)>>3)
#define BITNO(b) ((b)&7)

I hashallo(IH *RESTRICT,UI,UI,I);
UI cthia(UIL,D,A);
UI jthiau(J,A);
B jteqnx(J,I,X*,X*),jteqnq(J,I,Q*,Q*),jteqa(J,I,A*,A*);

// Hash a single INT-sized atom
#define hici1(x) CRC32L(-1LL,*(x))

// obsolete #define RETCRC3 R CRC32L(crc0,CRC32L(crc1,crc2))
#define RETCRC3 R CRC32L(crc0,(UI)rol32(crc1,9)+((UI)rol32(crc2,21)<<32))

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

// Hash a pair of INTs.  Not compatible with hici.  Based on wyhash.
static __forceinline UI hici2(UI x, UI y){
 x^=0xe7037ed1a0b428db;
 y^=0xa0761d6478bd642f;
 UI l,h;DPUMULU(x,y,l,h);
 R (UI4)(l^h);}

// Hash a single FL atom, with check for -0 and +0
#define hic01(x) ((*(UIL*)(x)!=(UIL)NEGATIVE0)?CRC32LL(-1L,*(UIL*)(x)):CRC32LL(-1L,0))

// Hashes for extended/rational types.  Hash only the numerator of rationals.  These are
// Q and X types (Q is a pair of X types)
static UI hix(X*v){A y=*v;   R hici(XLIMBLEN(y),UIAV(y));}
static UI hiq(Q*v){A y=v->n; R hici(XLIMBLEN(y),UIAV(y));}

// Hash a single double, using only the bits in ctmask.
//  not required for tolerant comparison, but if we tried to do tolerant comparison through the fast code it would help
static UI cthid(UIL ctmask,D d){R likely(*(UIL*)&d!=(UIL)NEGATIVE0)?CRC32LL(-1L,*(UIL*)&d&ctmask):CRC32LL(-1L,0);}

// compare floats, not distinguishing -0 from +0.  Return 0 if equal, 1 if not equal
static __forceinline I fcmp0(D* a, D* w, I n){
 DQ(n, if(a[i]!=w[i])R 1;);
 R 0;
}


#if C_AVX512
#define COMPSETUP \
 __mmask8 endmask = BZHI(0xff,1+(n-1)%8);
#define COMPCALL(a) icmpeq(v,(a)+n*hj,n,endmask)
static __forceinline I icmpeq(I *x, I *y, I n, __mmask8 em) {
 if(common(n<=8))R !!_mm512_cmpneq_epi64_mask(_mm512_maskz_loadu_epi64(em,x),_mm512_maskz_loadu_epi64(em,y));
 __m512i u,v;
 DQ((n-1)/8,
  u=_mm512_loadu_si512(x);
  v=_mm512_loadu_si512(y);
  x+=8;y+=8;
  if(_mm512_cmpneq_epi64_mask(u,v))R 1;)
 R !!_mm512_cmpneq_epi64_mask(_mm512_maskz_loadu_epi64(em,x),_mm512_maskz_loadu_epi64(em,y));}
#elif C_AVX2 || EMU_AVX2
#define COMPSETUP \
 __m256i endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-n)&(NPAR-1))));  // mask for 0 1 2 3 4 5 is xxxx 0001 0011 0111 1111 0001
#define COMPCALL(a) icmpeq(v,(a)+n*hj,n,endmask)
static __forceinline I icmpeq(I *x, I *y, I n, __m256i endmask) {
 __m256i u,v; __m256d ones=_mm256_castsi256_pd(_mm256_cmpeq_epi64(endmask,endmask));
 I i=(n-1)>>LGNPAR;  /* # loops for 0 1 2 3 4 5 is x 0 0 0 0 1 */
 while(--i>=0){
  u=_mm256_loadu_si256((__m256i*)x); v=_mm256_loadu_si256((__m256i*)y); x+=NPAR;y+=NPAR; if(!_mm256_testc_pd(_mm256_castsi256_pd(_mm256_cmpeq_epi64(u,v)),ones))R 1; // abort on any nonequal
  }
 u=_mm256_maskload_epi64(x,endmask); v=_mm256_maskload_epi64(y,endmask); 
 R !_mm256_testc_pd(_mm256_castsi256_pd(_mm256_cmpeq_epi64(u,v)),ones);  // no miscompares, compare equal (= 0 result)
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

// I flip-flopped for a while between this and a pair of scalar compares.  Decided to go with this: lower reg pressure, better predictability
#if C_AVX512
#define cmpi2(x,y) ({ !!_mm_cmpneq_epi64_mask(_mm_loadu_si128((__m128i*)(x)),_mm_loadu_si128((__m128i*)(y))); })
#else
#define cmpi2(x,y) ({ __m128i _cmpres=_mm_cmpeq_epi32(_mm_loadu_si128((__m128i*)(x)),_mm_loadu_si128((__m128i*)(y))); !_mm_testc_si128(_cmpres,_mm_setone_si128()); })
#endif

static inline B jeqd(I n,D*u,D*v,D cct){DQ(n, if(!TCMPEQ(cct,*u,*v))R 0; ++u; ++v;); R 1;}
static inline B jteqz(J jt,I n,Z*u,Z*v){DQ(n, if(!zeq(*u,*v))R 0; ++u; ++v;); R 1;}


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


// prototypes for implementations in viavx?.c
IOF(jtioax1);  IOF(jtioau);  IOF(jtiox);  IOF(jtioq);  IOF(jtioc);  IOF(jtioi);  IOF(jtioC2);  IOF(jtioC4);  IOF(jtioi1);  IOF(jtio16); IOF(jtioc01);  IOF(jtioz01);  IOF(jtioc0);  IOF(jtioz0);
IOF(jtioax12); IOF(jtioau2); IOF(jtiox2); IOF(jtioq2); IOF(jtioc2); IOF(jtioi2); IOF(jtioC22); IOF(jtioC42); IOF(jtioi12); IOF(jtio162); IOF(jtioc012); IOF(jtioz012); IOF(jtioc02); IOF(jtioz02);
IOF(jtioz);  IOF(jtioz1);  IOF(jtiod);  IOF(jtiod1);  IOF(jtioa);  IOF(jtioa1);
IOF(jtioz2); IOF(jtioz12); IOF(jtiod2); IOF(jtiod12); IOF(jtioa2); IOF(jtioa12);
IOF(jtio12); IOF(jtio22); IOF(jtio42); IOF(jtio82);
IOF(jtio14); IOF(jtio24); IOF(jtio44); IOF(jtio84);
A jtiosc(J jt,I mode,I n,I asct,I wsct,I ac,I wc,A a,A w,A z);
IOF(jtiobs);
IOF(jtiowax1);  IOF(jtiowau);  IOF(jtiowx);  IOF(jtiowq);  IOF(jtiowc);  IOF(jtiowi);  IOF(jtiow21);  IOF(jtiow41);  IOF(jtiowi1);  IOF(jtiow161);  IOF(jtiowc01);  IOF(jtiowz01);  IOF(jtiowc0);  IOF(jtiowz0);
IOF(jtiowax12); IOF(jtiowau2); IOF(jtiowx2); IOF(jtiowq2); IOF(jtiowc2); IOF(jtiowi2); IOF(jtiow212); IOF(jtiow412); IOF(jtiowi12); IOF(jtiow1612); IOF(jtiowc012); IOF(jtiowz012); IOF(jtiowc02); IOF(jtiowz02);
