/* Copyright 1990-2014, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Index-of                                                         */

#if !C_AVX /* see viavx.c */

#include "j.h"
#include "vcomp.h"

// Table of hash-table sizes
// These are primes (to reduce collisions), and big enough to just fit into a power-of-2
// block after leaving 2 words for memory header, AH words for A-block header, 1 for rank (not used for symbol tables),
// and SYMLINFOSIZE for the unused first word, which (for symbol tables) holds general table info.
// symbol tables allocate ptab[]+SYMLINFOSIZE entries, leaving ptab[] entries for symbols.  i.-family
// operations use tables of size ptab[].  i.-family operations have rank 1.  So the prime in the table
// must be no more than (power-of-2)-10.
 // If AH changes, these numbers need to be revisited
// The first row of small values was added to allow for small symbol tables to hold local
// variables.  Hardwired references to ptab in the code have 3 added so that they correspond to
// the correct values.  User locale sizes also refer to the original values, and have 3 added before use.
I ptab[]={
        3,         5,        19,
       53,       113,       241,       499,     1013, 
     2029,      4079,      8179,     16369,    32749, 
    65521,    131059,    262133,    524269,  1048559, 
  2097133,   4194287,   8388593,  16777199, 33554393, 
 67108837, 134217689, 268435399, 536870879
};

I nptab=sizeof(ptab)/SZI;

// The bucket-size table[i][j] gives the hash-bucket number of argument-name j when the symbol table was
// created with size i.  The argument names supported are ynam and xnam.
I yxbuckets[sizeof(ptab)/SZI][2];

// Emulate bucket calculation for local names.
void bucketinit(){I j;
 for(j=0;j<nptab;++j){
  yxbuckets[j][0]=SYMHASH(NAV(ynam)->hash,ptab[j]);
  yxbuckets[j][1]=SYMHASH(NAV(xnam)->hash,ptab[j]);
 }
}

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
// we calculate this using pi as a reference: find pi +- ct, and see which bits are different.  Everything
// above the bits that are changed by ct is considered significant
static void ctmask(J jt){DI p,x,y;UINT c,d,e,m,q;
 p.d=PI;                      /* pi itself                */
 x.d=PI*(1-jt->ct);           /* lower bound              */
 y.d=PI/(1-jt->ct);           /* upper bound              */
 c=p.i[MSW]; d=p.i[LSW]; m=0;
 if(c==x.i[MSW]){e=x.i[LSW]; m =(d&~e)|(~d&e);}
 if(c==y.i[MSW]){e=y.i[LSW]; m|=(d&~e)|(~d&e);}
 q=m;
 while(m){m>>=1; q|=m;}       /* q=:+./\m as a bit vector */
 jt->ctmask=~(UI)q;
}    /* 1 iff significant wrt comparison tolerance */


#if C_HASH
/* hic:  hash a string of length k                                 */
/* hicx: hash the bytes of string v indicated by hin/hiv           */
/* hic2: hash the bytes of a string of length k (k even)           */
/* hic4: hash the bytes of a string of length k (k multiple of 4)  */
/* hicw: hash a word (32 bit or 64 bit depending on CPU)           */

       UI hic (     I k,UC*v){UI z=HASH0;             DO(k,       z=(149*i+1000003)**v++   ^z<<1;      ); R z;}

static UI hicnz(    I k,UC*v){UI z=HASH0;UC c;        DO(k, c=*v++; if(c&&c!=255)z=(149*i+1000003)*c^z<<1;); R z;}

static UI hicx(J jt,I k,UC*v){UI z=HASH0;I*u=jt->hiv; DO(jt->hin, z=(149*i+1000003)*v[*u++]^z<<1;      ); R z;}

#if C_LE
       UI hic2(     I k,UC*v){UI z=HASH0;             DO(k/2,     z=(149*i+1000003)**v     ^z<<1;
                                                       if(*(v+1)){z=(149*i+1000003)**(v+1) ^z<<1;} v+=2;); R z;}
#else
       UI hic2(     I k,UC*v){UI z=HASH0; ++v;        DO(k/2,     z=(149*i+1000003)**v     ^z<<1;
                                                       if(*(v-1)){z=(149*i+1000003)**(v-1) ^z<<1;} v+=2;); R z;}
#endif

#if C_LE
       UI hic4(     I k,UC*v){UI z=HASH0;             DO(k/4,     z=(149*i+1000003)**v     ^z<<1;
                                               if(*(v+2)||*(v+3)){z=(149*i+1000003)**(v+1) ^z<<1;
                                                                  z=(149*i+1000003)**(v+2) ^z<<1;
                                                                  z=(149*i+1000003)**(v+3) ^z<<1;}
                                                  else if(*(v+1)){z=(149*i+1000003)**(v+1) ^z<<1;} v+=4;); R z;}
#else
       UI hic4(     I k,UC*v){UI z=HASH0; v+=3;       DO(k/4,     z=(149*i+1000003)**v     ^z<<1;
                                               if(*(v-2)||*(v-3)){z=(149*i+1000003)**(v-1) ^z<<1;
                                                                  z=(149*i+1000003)**(v-2) ^z<<1;
                                                                  z=(149*i+1000003)**(v-3) ^z<<1;}
                                                  else if(*(v-1)){z=(149*i+1000003)**(v-1) ^z<<1;} v+=4;); R z;}
#endif

#else
/* hic:  hash a string of length k                                 */
/* hicx: hash the bytes of string v indicated by hin/hiv           */
/* hic2: hash the low order bytes of a string of length k (k even) */
/* hic4: hash the low order bytes of a string of length k (k multiple of 4) */
/* hicw: hash a word (32 bit or 64 bit depending on CPU)           */

       UI hic (     I k,UC*v){UI z=0;             DO(k,       z=(i+1000003)**v++   ^z<<1;      ); R z;}

static UI hicnz(    I k,UC*v){UI z=0;UC c;        DO(k, c=*v++; if(c&&c!=255)z=(i+1000003)*c^z<<1;); R z;}

static UI hicx(J jt,I k,UC*v){UI z=0;I*u=jt->hiv; DO(jt->hin, z=(i+1000003)*v[*u++]^z<<1;      ); R z;}

#if C_LE
       UI hic2(     I k,UC*v){UI z=0;             DO(k/2,     z=(i+1000003)**v     ^z<<1; v+=2;); R z;}
#else
       UI hic2(     I k,UC*v){UI z=0; ++v;        DO(k/2,     z=(i+1000003)**v     ^z<<1; v+=2;); R z;}
#endif

#if C_LE
       UI hic4(     I k,UC*v){UI z=0;             DO(k/4,     z=(i+1000003)**v     ^z<<1; v+=4;); R z;}
#else
       UI hic4(     I k,UC*v){UI z=0; v+=3;       DO(k/4,     z=(i+1000003)**v     ^z<<1; v+=4;); R z;}
#endif

#endif

#if SY_64
// Hash a single unsigned INT
#define hicw(v)  (10495464745870458733U**(UI*)(v))
// Hash a single double, using only the bits in ctmask.  -0 is hashed differently than +0.  Should we take the sign bit out of ct?  Only if ct=0?
//  not required for tolerant comparison, but if we tried to do tolerant comparison through the fast code it would help
static UI jthid(J jt,D d){R 10495464745870458733U*(jt->ctmask&*(I*)&d);}
#else
#define hicw(v)  (2838338383U**(U*)(v))
static UI jthid(J jt,D d){DI x; x.d=d; R 888888883U*(x.i[LSW]&jt->ctmask)+2838338383U*x.i[MSW];}
#endif

// Hash the data in the given A.  Comments say this is called only for singletons
// If empty or boxed, hash the shape
// If literal, hash the whole thing
// If numeric, convert first atom to float and hash it
// Q: called only for singletons?  is hct=1 for exact compares?  Seems to be 1.0 always.  Why multiply by hct?
//  is ctmask=~0 for exact compares?  Better be.
static UI jthia(J jt,D hct,A y){UC*yv;D d;I n,t;Q*u;
 n=AN(y); t=AT(y); yv=UAV(y);
 if(!n||t&BOX)R hic(AR(y)*SZI,(UC*)AS(y));
 switch(CTTZ(t)){
  case LITX:  R hic(n,yv);
  case C2TX:  R hic2(2*n,yv);
  case C4TX:  R hic4(4*n,yv);
  case SBTX:  R hic(n*SZI,yv);
  case B01X:  d=*(B*)yv; break;
  case INTX:  d=(D)*(I*)yv; break;
  case FLX: 
  case CMPXX: d=*(D*)yv; break;
  case XNUMX: d=xdouble(*(X*)yv); break;
  case RATX:  u=(Q*)yv; d=xdouble(u->n)/xdouble(u->d);
 }
 R hid(d*hct);
}

// Hash y, which is not a singleton.  Integral types do not hash bytes that equal 0 or 255 (why??).
static UI jthiau(J jt,A y){I m,n;UC*v=UAV(y);UI z=2038074751;X*u,x;
 m=n=AN(y);
 if(!n)R 0;
 switch(CTTZ(AT(y))){
  case RATX:  m+=n;  /* fall thru */
  case XNUMX: u=XAV(y); DO(m, x=*u++; v=UAV(x); z+=hicnz(AN(x)*SZI,UAV(x));); R z;
  case INTX:                                    z =hicnz(n    *SZI,UAV(y));   R z;
  default:   R hic(n*bp(AT(y)),UAV(y));
}}

// Hashes for extended/rational types.  Hash only the numerator of rationals.  These are
// Q and X types (Q is a brace of X types)
static UI hix(X*v){A y=*v;   R hic(AN(y)*SZI,UAV(y));}
static UI hiq(Q*v){A y=v->n; R hic(AN(y)*SZI,UAV(y));}

// Comparisons for extended/rational/float/complex types.  teq should use the macro
static B jteqx(J jt,I n,X*u,X*v){DO(n, if(!equ(*u,*v))R 0; ++u; ++v;); R 1;}
static B jteqq(J jt,I n,Q*u,Q*v){DO(n, if(!QEQ(*u,*v))R 0; ++u; ++v;); R 1;}
static B jteqd(J jt,I n,D*u,D*v){DO(n, if(!teq(*u,*v))R 0; ++u; ++v;); R 1;}
static B jteqz(J jt,I n,Z*u,Z*v){DO(n, if(!zeq(*u,*v))R 0; ++u; ++v;); R 1;}

// test a subset of two boxed arrays for match.  u/v point to pointers to contants, c and d are the relative flags
// We test n subboxes
static B jteqa(J jt,I n,A*u,A*v,I c,I d){DO(n, if(!equ(AADR(c,*u),AADR(d,*v)))R 0; ++u; ++v;); R 1;}

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
 m    target axis length
 n    target item # atoms
 c    # target items in a left-arg cell, which may include multiple right-arg cells
 k    target item # bytes
 acr  left  rank
 wcr  right rank
 ac   # left  arg cells  (cells, NOT items)
 wc   # right arg cells
 ak   # bytes left  arg cells, or 0 if only 1 cell
 wk   # bytes right arg cells, or 0 if only one cell
 a    left  arg
 w    right arg, or mark for m&i. or m&i: or e.&n or -.&n
 hp   pointer to hash table or to 0
 z    result
*/

// should RESTRICT all pointers used for hashtables, inputs, outputs

// should change IOF to return pointer to h; then could just pass in h rather than hp
// should not pass in wcr - not used.  Check other args
#define IOF(f)     A f(J jt,I mode,I m,I n,I c,I k,I acr,I wcr,I ac,I wc,I ak,I wk,A a,A w,A*hp,A z)
// variables used in IOF routines:
// h=A for hashtable, hv->hashtable data, p=#entries in table, pm=unsigned p, used for converting hash to bucket#
// zb,zc,zi are pointer to result area, of different sizes according to the operation
// t1 is 1.0 here; it is the amount that a singleton must be multiplied by to get the value to be hashed
// acn,wcn=#atoms in cell of a,w
// cn = #atoms in target item
// j is the starting bucket number of the hashtable search
// hj is the index of the first empty-or-matching bucket encountered
// zv->result area (as an array of pointers), av->a data, wv->w data 
#define IODECL(T)  A h=*hp;B*zb;C*zc;D t1=1.0;I t=sizeof(T),acn=ak/t,cn=k/t,hj,*hv=AV(h),j,l,p=AN(h),  \
                     wcn=wk/t,*zi,*zv=AV(z);T*av=(T*)AV(a),*v,*wv=(T*)AV(w);UI pm=p
// start searching at index j, and stop when j points to a slot that is empty, or for which exp is false
// (exp is a test for not-equal, normally referring to v (the current element being hashed) and hv (the data field for
// the first block that hashed to this address)
// should init hash to _1 to allow constant compare? not needed if we embed seq# in hash, but useful otherwise
// should init end-of-array to sentinel to move compare against j out of the loop
// should combine hash here, and unroll 3 times to overlap hash, fetch, compare
#define FIND(exp)  while(m>(hj=hv[j])&&(exp)){++j; if(j==p)j=0;}
// define ad and wd, which are bases to be added to boxed addresses
// should use conditional statement
#define RDECL      I ad=(I)a*ARELATIVE(a),wd=(I)w*ARELATIVE(w)
// Misc code to set the shape once we see how many results there are, used for ~. y and x -. y
#define ZISHAPE    *AS(z)=AN(z)=zi-zv
#define ZCSHAPE    *AS(z)=(zc-(C*)zv)/k; AN(z)=n**AS(z)
#define ZUSHAPE(T) *AS(z)= zu-(T*)zv;    AN(z)=n**AS(z)

// Routines to build the hash table from a.  hash calculates the hash function, usually referring to v (the input) and possibly other names.  exp is the comparison routine.  should use _1 for empty?
#define XDOA(hash,exp,inc)         {d=ad; v=av;          DO(m,  j=(hash)%pm; FIND(exp); if(m==hj)hv[j]=i; inc;);}
#define XDQA(hash,exp,dec)         {d=ad; v=av+cn*(m-1); DQ(m,  j=(hash)%pm; FIND(exp); if(m==hj)hv[j]=i; dec;);}

// Routines to look up an item of w.  hash calculates the hash function, usually referring to v (the input) and possibly other names.  exp is the comparison routine.  stmt is executed after the hash lookup
// and must check whether *hj (->prev data) matches the new data in *v
#define XDO(hash,exp,inc,stmt)     {d=wd; v=wv;          DO(cm, j=(hash)%pm; FIND(exp); stmt;             inc;);}
#define XDQ(hash,exp,dec,stmt)     {d=wd; v=wv+cn*(c-1); DQ(cm, j=(hash)%pm; FIND(exp); stmt;             dec;);}
// special lookup routines to move the data rather than store its index, used for nub/match
#define XMV(hash,exp,inc,stmt)      \
 if(k==SZI){XDO(hash,exp,inc,if(m==hj){*zi++=*(I*)v;      stmt;}); zc=(C*)zi;}  \
 else       XDO(hash,exp,inc,if(m==hj){MC(zc,v,k); zc+=k; stmt;});


// Routine to allocate sections of the hash tables
// *hh is the hash table we have selected, p is the number of hash entries we need, m is the maximum+1 value that needs to be stored in an entry
// md gives information about the type of entry, in particular if it is bits or packed bits
// Main result is in *hh, notably currentlo/currentindexofst.  Other fields have been adjusted to account for the allocated space
// The nominal result is the new value for md.  This routine is responsible for setting/clearing IIMODBASE0 appropriately, but only when IIMODBITS is clear
static I hashallo(IH * RESTRICT hh,UI p,UI m,I md){
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
  memset(hh->data.UC,(md&(IIMODPACK+IIOPMSK))<=INUBI,p);
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
 if(!(md&IIMODFORCE0)){
  // Not forced to start over, choose an allocation

  // First of all: it's moot if the allocation won't fit on the right
  I maxn = hh->datasize>>hh->hashelelgsize;  // get max possible index+1
  I selside=0;  // default to allocating on the left (i. e. at 0)
  UI maxindex = (1LL<<(8LL<<hh->hashelelgsize))-1;  // largest possible index for this table
  UI indexceil=maxindex-m;  // max starting index

  // Cost of allocating on the left comes
  // (1) now, as we use (m*currenthi) units of index space
  // (2) in the future, as all future allocations on the right lose index space,
  // in the amount of (new left index end-right index end-future m)*(width-currenthi).  We estimate future values to equal current ones.
  // (3) now, if we have to clear the invalid area (1 store per word cleared)
  // BUT: if the cleared invalid area covers the entire p, we use m rather than left-index+m for calculating the other costs
  // The cost of a unit of index space is 1 store per (width*maxindex)/(m*p) index-space unit, with m and p rounded up if large;
  // we will approximate as 2*(m*p)/(width*maxindex) 

  // Cost of allocating on the right is
  // (p*m) units of index space for the allocated area, plus (currenthi*((right index+m)-left index)) units of index space lost on the left,
  // plus cache expense of p<.currenthi cache words
  // The cost of a cache word is reckoned at 1/2 store per word

  // That's not worth figuring out for every call.  So, we will just allocate on the right unless
  // the invalid area is empty and m<4096 (about the size of L1 cache), to get the caching benefit for small arguments
  if(p <= maxn-hh->currenthi && hh->previousindexend < indexceil && (m>4096 || (hh->invalidlo<p))) {  // the right side is a possibility
   // Allocating right.  Return a region starting at currenthi
   md &= ~(IIMODBASE0|IINOTALLOCATED);  // can't use the fastest code if we didn't clear; but we allocated it
   hh->currentlo=hh->currenthi; hh->currenthi+=p;   // set return value (starting position) and partition
   UI startx=hh->previousindexend;
   hh->currentindexofst=startx; hh->previousindexend=(startx+=m);  // set return value (starting index) and allocated index space
   hh->currentindexend=MAX(startx,hh->currentindexend);  // since currentindexend speaks for the whole left side, we may have to push it up
   // Since the partition (currenthi) is kept to the right of the invalid region, there is no need to clear invalid data
// obsolete UI clrpt = MIN(hh->currenthi, hh->invalidhi);   // get index to clear to
// obsolete  I clrfrom = MAX(hh->currentlo,hh->invalidlo);
// obsolete  I nclrhsh = clrpt-clrfrom;  // get number of hash entries to clear
// obsolete  if(nclrhsh>0){   // if there is something to clear
// obsolete     memset(hh->data.UC+(clrfrom<<hh->hashelelgsize), C0,(nclrhsh<<hh->hashelelgsize));  // clear the region to 0
// obsolete  }
  }else if(hh->currentindexend < indexceil){
   // Allocating left.  Return a region starting at position 0.
   md &= ~(IIMODBASE0|IINOTALLOCATED);  // can't use the fastest code if we didn't clear; but we allocated
   hh->currentlo=0; hh->currenthi=p;   // set return value (starting position) and partition
   UI startx=hh->currentindexend;   // the previous end+1 index becomes the index for this time
   hh->previousindexend=startx;  // bring up the right side to match the left side before we advance
   hh->currentindexofst=startx; hh->currentindexend=(startx+=m);  // set return value (starting index) and allocated index space
   // If (part of) the return area needs to be cleared, clear it.  If we clear the entire left side, reduce the left index
   UI clrpt = MIN(p, hh->invalidhi);   // get index to clear to
   I nclrhsh = clrpt - hh->invalidlo;  // get number of hash entries to clear
   if(nclrhsh>0){   // if there is something to clear
    memset(hh->data.UC+(hh->invalidlo<<hh->hashelelgsize), C0,(nclrhsh<<hh->hashelelgsize));  // clear the region to 0
    if(p<=clrpt){startx=hh->currentindexofst;hh->currentindexend=MAX(m,hh->currentindexofst);}  // If we cleared the whole left side, we can back the left-side pointer to match the right
    hh->invalidlo=clrpt;  // Indicate that we have cleared this region
   }
  }
 }
 if(md&IINOTALLOCATED) {
  // Cannot allocate as is: the region must be initialized.  set BASE0 in this case, and since we initialize, initialize to the most useful value
  md |= IIMODBASE0;  // if we clear the region, mention that so that we get the fastest code
  // Clear the entries of the first allocation to m.  Use fullword stores (should use cache-line stores).  Our allocations are always multiples of fullwords,
  // so it is safe to overfill with fullword stores
  UI storeval=m; if(hh->hashelelgsize==1)storeval |= storeval<<16; if(SZI>4)storeval |= storeval<<(32%BW);  // Pad store value to 64 bits, dropping excess on smaller machines
  I i, nstores=((p<<hh->hashelelgsize)+SZI-1)>>LGSZI;  // get count of partially-filled words
  for(i=0;i<nstores;++i){hh->data.UI[i]=storeval;}  // fill them all
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
    hh->currentindexend=MAX(hh->previousindexend,1+m);  // keep the left-side max index no less than the right-wide
   }else{
    // We didn't clear as far as the partition.  The left side still contains old data.
    if(hh->currentindexend<1+m)hh->currentindexend=1+m;  // Don't lower the index if there is uncleared data on the left
   }
  }else{
   // Not FORCE0: we are clearing because we have to.  Clear everything.  But we can save clearing the right side if it's already clear.
   I clrtopoint=(hh->previousindexend!=1)?hh->datasize:hh->currenthi<<hh->hashelelgsize;  // high+1 entry to clear
   I clrfrompoint=p<<hh->hashelelgsize;  // offset to clear from
   if((clrtopoint-=clrfrompoint)>0){memset(hh->data.UC+clrfrompoint, C0, clrtopoint);}  // clear the region to 0
   hh->currenthi=p;   // set return value (starting position) and partition
   hh->currentindexend=1+m;  // set return value (starting index) and allocated index space.  Leave 0 for 'not found'
   hh->previousindexend=1;  // Init right side unused (but with the 0s representing initialized values)
  }
  hh->currentindexofst=0;  // Indic left-side starting index (return value)
  hh->currentlo=0;    // Indic left-side starting position (return value)
  hh->invalidlo=IMAX; hh->invalidhi=0;  // clear invalidity region
 }
 R md;
}

// *************** first class: intolerant comparisons, unboxed ***********************

// The main search routine, given a, w, mode, etc, for datatypes with no comparison tolerance
// should change IPHOFFSET for ease in calc md
// should change IIDOT etc to make testing run faster
// should change hash so as not to require clearing it - consider leaving the table allocated permanently to reduce cache thrashing and save initialization.
// Could use subsets of it - power of 2??  Choose good size to reduce cache footprint of accessed area.  For prehashing, copy the hash to the
//  prehashed compound.  Could save space by copying only index, not other bytes
// alternatively, could save a multiply by storing address of data rather than index (not as good)
// if we clear the hash, should clear by fast block-copy
// (in all classes) should do self-index in 1 pass: hash and get result without further ado (seems to be done already)

// if there is not a prehashed hashtable, we clear the hashtable and fill it from a, then hash & check each item of w
#define IOFX(T,f,hash,exp,inc,dec)   \
 IOF(f){RDECL;IODECL(T);B b;I cm,d,md,s;UC*u=0;                                      \
  md=mode<IPHOFFSET?mode:mode-IPHOFFSET;                                             \
  b=a==w&&ac==wc&&(mode==IIDOT||mode==IICO||mode==INUBSV||mode==INUB||mode==INUBI);  \
  zb=(B*)zv; zc=(C*)zv; zi=zv; cm=w==mark?0:c;                                       \
  for(l=0;l<ac;++l,av+=acn,wv+=wcn){                                                 \
   if(mode<IPHOFFSET){DO(p,hv[i]=m;); if(!b){if(mode==IICO)XDQA(hash,exp,dec) else XDOA(hash,exp,inc);}}  \
    switch(md){                                                                       \
    case IIDOT:   if(b){          XDO(hash,exp,inc,*zv++=m==hj?(hv[j]=i):hj);}       \
                  else XDO(hash,exp,inc,*zv++=hj);                           break;  \
    case IICO:    if(b){zi=zv+=c; XDQ(hash,exp,dec,*--zi=m==hj?(hv[j]=i):hj);}       \
                  else XDO(hash,exp,inc,*zv++=hj);                           break;  \
    case INUBSV:       XDO(hash,exp,inc,*zb++=m==hj?(hv[j]=i,1):0);          break;  \
    case INUB:         XMV(hash,exp,inc,hv[j]=i);                ZCSHAPE;    break;  \
    case ILESS:        XMV(hash,exp,inc,0      );                ZCSHAPE;    break;  \
    case INUBI:        XDO(hash,exp,inc,if(m==hj)*zi++=hv[j]=i); ZISHAPE;    break;  \
    case IEPS:         XDO(hash,exp,inc,*zb++=m>hj);                         break;  \
    case II0EPS:  s=c; XDO(hash,exp,inc,if(m==hj){s=i; break;}); *zi++=s;    break;  \
    case II1EPS:  s=c; XDO(hash,exp,inc,if(m> hj){s=i; break;}); *zi++=s;    break;  \
    case IJ0EPS:  s=c; XDQ(hash,exp,dec,if(m==hj){s=i; break;}); *zi++=s;    break;  \
    case IJ1EPS:  s=c; XDQ(hash,exp,dec,if(m> hj){s=i; break;}); *zi++=s;    break;  \
    case ISUMEPS: s=0; XDO(hash,exp,inc,if(m> hj)++s;         ); *zi++=s;    break;  \
    case IANYEPS: s=0; XDO(hash,exp,inc,if(m> hj){s=1; break;}); *zb++=1&&s; break;  \
    case IALLEPS: s=1; XDO(hash,exp,inc,if(m==hj){s=0; break;}); *zb++=1&&s; break;  \
    case IIFBEPS: s=c; XDO(hash,exp,inc,if(m> hj)*zi++=i      ); ZISHAPE;    break;  \
  }}                                                                                 \
  R z;                                                                               \
 }

//
static IOFX(A,jtioax1,hia(t1,AADR(d,*v)),!equ(AADR(d,*v),AADR(ad,av[hj])),++v,   --v  )  /* boxed exact 1-element item */   
static IOFX(A,jtioau, hiau(AADR(d,*v)),  !equ(AADR(d,*v),AADR(ad,av[hj])),++v,   --v  )  /* boxed uniform type         */
static IOFX(X,jtiox,  hix(v),            !eqx(n,v,av+n*hj),               v+=cn, v-=cn)  /* extended integer           */   
static IOFX(Q,jtioq,  hiq(v),            !eqq(n,v,av+n*hj),               v+=cn, v-=cn)  /* rational number            */   
static IOFX(C,jtioc,  hic(k,(UC*)v),     memcmp(v,av+k*hj,k),             v+=cn, v-=cn)  /* boolean, char, or integer  */
static IOFX(C,jtiocx, hicx(jt,k,(UC*)v), memcmp(v,av+k*hj,k),             v+=cn, v-=cn)  /* boolean, char, or integer  */
static IOFX(I,jtioi,  hicw(v),           *v!=av[hj],                      ++v,   --v  )

// ********************* second class: tolerant comparisons, possibly boxed **********************

// should have 64-bit versions that use ctmask directly (it should be in machine byte order)
// create hash for a D type
#define HID(y)              (888888883U*y.i[LSW]+2838338383U*y.i[MSW])
#define MASK(dd,xx)         {dd.d=xx; dd.i[LSW]&=jt->ctmask;}

// functions for building the hash table for tolerant comparison.  expa is the function for detecting matches on a values

// hash a single D type.  If complex, hash the real part only (hashing both parts would require 4 hashes for tolerance)
#define THASHA(expa)        {x=*(D*)v; MASK(dx,x); j=HID(dx)%pm; FIND(expa); if(m==hj)hv[j]=i;}
// boxed type.  "hash" the shape only, after performing relative-to-absolute conversion
// should omit the relative, since only shape is hashed
#define THASHBX(expa)       {j=hia(t1,AADR(d,*v))%pm;            FIND(expa); if(m==hj)hv[j]=i;}

// functions for searching the hash table

// find a tolerant match for *v.  Check a threshold below and a threshold above, and set il and ir to the lower/upper buckets matched
#define TFINDXY(expa,expw)  \
 {x=*(D*)v;                                                                             \
  MASK(dl,x*tl);                j=              HID(dl)%pm; FIND(expw); il=ir=hj;       \
  MASK(dr,x*tr); if(dr.d!=dl.d){j=              HID(dr)%pm; FIND(expw);    ir=hj;}      \
 }
// same idea, but also throw in an exact match on the value itself (rounded to a bucket value).  Used for reflexive searches, in which
// we have not initialized the hash table.  We first add an (exact) entry for the current value, and then search for tolerant matches
// We have to add an entry for the current value always, because a hashed value may be tolerantly equal to y but not tequal some
// other value tequal to y.  -0 will always get hashed as +0, and possibly -0 as well
#define TFINDYY(expa,expw)  \
 {x=*(D*)v;                                                                             \
  MASK(dx,x   ); j=jx=HID(dx)%pm; jt->ct=0.0; FIND(expa); jt->ct=ct; if(m==hj)hv[j]=i;  \
  MASK(dl,x*tl);                j=dl.d==dx.d?jx:HID(dl)%pm; FIND(expw); il=ir=hj;       \
  MASK(dr,x*tr); if(dr.d!=dl.d){j=dr.d==dx.d?jx:HID(dr)%pm; FIND(expw);    ir=hj;}      \
 }
// Here the match on the value itself is not exact.
#define TFINDY1(expa,expw)  \
 {x=*(D*)v;                                                                             \
  MASK(dx,x   ); j=jx=HID(dx)%pm;             FIND(expa);            if(m==hj)hv[j]=i;  \
  MASK(dl,x*tl);                j=dl.d==dx.d?jx:HID(dl)%pm; FIND(expw); il=ir=hj;       \
  MASK(dr,x*tr); if(dr.d!=dl.d){j=dr.d==dx.d?jx:HID(dr)%pm; FIND(expw);    ir=hj;}      \
 }
// here comparing boxes
#define TFINDBX(expa,expw)   \
 {jx=j=hia(tl,AADR(d,*v))%pm;           FIND(expw); il=ir=hj;   \
     j=hia(tr,AADR(d,*v))%pm; if(j!=jx){FIND(expw);    ir=hj;}  \
 }

// loop to search the hash table.  b means self-index, bx means boxed
// Fxx is a TFIND macro, charged with setting il and ih; stmt tells what to do with il/ir
// should combine the cases for all ks to save a test?
#define TDO(FXY,FYY,expa,expw,stmt)  \
 switch(4*bx+2*b+(k==sizeof(D))){                       \
  default: DO(c, FXY(expa,expw); stmt; v+=cn;); break;  \
  case 1:  DO(c, FXY(expa,expw); stmt; ++v;  ); break;  \
  case 2:  DO(c, FYY(expa,expw); stmt; v+=cn;); break;  \
  case 3:  DO(c, FYY(expa,expw); stmt; ++v;  );         \
 }
// Same, but search from the end of y backwards (e. i: 0 etc)
#define TDQ(FXY,FYY,expa,expw,stmt)  \
 v+=cn*(c-1);                                           \
 switch(4*bx+2*b+(k==sizeof(D))){                       \
  default: DQ(c, FXY(expa,expw); stmt; v-=cn;); break;  \
  case 1:  DQ(c, FXY(expa,expw); stmt; --v;  ); break;  \
  case 2:  DQ(c, FYY(expa,expw); stmt; v-=cn;); break;  \
  case 3:  DQ(c, FYY(expa,expw); stmt; --v;  );         \
 }
// Version for ~. y and x -. y .  prop is a condition; if true, move the item to *zc++
#define TMV(FXY,FYY,expa,expw,prop)   \
 switch(4*bx+2*b+(k==sizeof(D))){                                  \
  default: DO(c, FXY(expa,expw); if(prop){MC(zc,v,k); zc+=k;}; v+=cn;);            break;  \
  case 1:  DO(c, FXY(expa,expw); if(prop)*zd++=*(D*)v;         ++v;  ); zc=(C*)zd; break;  \
  case 2:  DO(c, FYY(expa,expw); if(prop){MC(zc,v,k); zc+=k;}; v+=cn;);            break;  \
  case 3:  DO(c, FYY(expa,expw); if(prop)*zd++=*(D*)v;         ++v;  ); zc=(C*)zd;         \
 }

// Do the operation.  Build a hash for a except when unboxed self-index
#define IOFT(T,f,FA,FXY,FYY,expa,expw)   \
 IOF(f){RDECL;IODECL(T);B b,bx;D ct=jt->ct,tl=1-jt->ct,tr=1/tl,x,*zd;DI dl,dr,dx;I d,e,il,ir,jx,md,s;  \
  md=mode<IPHOFFSET?mode:mode-IPHOFFSET;                                                         \
  b=a==w&&ac==wc&&(mode==IIDOT||mode==IICO||mode==INUBSV||mode==INUB||mode==INUBI);              \
  zb=(B*)zv; zc=(C*)zv; zd=(D*)zv; zi=zv; e=cn*(m-1); bx=1&&BOX&AT(a);                           \
  jx=0; dl.d=dr.d=dx.d=x=0.0;                                                                    \
  for(l=0;l<ac;++l,av+=acn,wv+=wcn){                                                             \
   if(mode<IPHOFFSET){                                                                           \
    DO(p,hv[i]=m;);                                                                              \
    if(bx||!b){                                                                                  \
     d=ad; v=av; jt->ct=0.0;                                                                     \
     if(IICO==mode){v+=e; DQ(m, FA(expa); v-=cn;);}else DO(m, FA(expa); v+=cn;);                 \
     jt->ct=ct; if(w==mark)break;                                                                \
   }}                                                                                            \
   d=wd; v=wv;                                                                                   \
   switch(md){                                                                                   \
    case IIDOT:        TDO(FXY,FYY,expa,expw,*zv++=MIN(il,ir));                          break;  \
    case IICO:  zv+=c; TDQ(FXY,FYY,expa,expw,*--zv=m==il?ir:m==ir?il:MAX(il,ir)); zv+=c; break;  \
    case INUBSV:       TDO(FXY,FYY,expa,expw,*zb++=i==MIN(il,ir));                       break;  \
    case INUB:         TMV(FXY,FYY,expa,expw,i==MIN(il,ir));                 ZCSHAPE;    break;  \
    case ILESS:        TMV(FXY,FYY,expa,expw,m==il&&m==ir);                  ZCSHAPE;    break;  \
    case INUBI:        TDO(FXY,FYY,expa,expw,if(i==MIN(il,ir))*zi++=i;);     ZISHAPE;    break;  \
    case IEPS:         TDO(FXY,FYY,expa,expw,*zb++=m>il||m>ir             );             break;  \
    case II0EPS:  s=c; TDO(FXY,FYY,expa,expw,if(m==il&&m==ir){s=i; break;}); *zi++=s;    break;  \
    case II1EPS:  s=c; TDO(FXY,FYY,expa,expw,if(m> il||m> ir){s=i; break;}); *zi++=s;    break;  \
    case IJ0EPS:  s=c; TDQ(FXY,FYY,expa,expw,if(m==il&&m==ir){s=i; break;}); *zi++=s;    break;  \
    case IJ1EPS:  s=c; TDQ(FXY,FYY,expa,expw,if(m> il||m> ir){s=i; break;}); *zi++=s;    break;  \
    case ISUMEPS: s=0; TDO(FXY,FYY,expa,expw,if(m> il||m> ir)++s          ); *zi++=s;    break;  \
    case IANYEPS: s=0; TDO(FXY,FYY,expa,expw,if(m> il||m> ir){s=1; break;}); *zb++=1&&s; break;  \
    case IALLEPS: s=1; TDO(FXY,FYY,expa,expw,if(m==il&&m==ir){s=0; break;}); *zb++=1&&s; break;  \
    case IIFBEPS:      TDO(FXY,FYY,expa,expw,if(m> il||m> ir)*zi++=i      ); ZISHAPE;    break;  \
  }}                                                                                             \
  R z;                                                                                           \
 }

// Verbs for the types of inputs

// CMPLX array
static IOFT(Z,jtioz, THASHA, TFINDXY,TFINDYY,memcmp(v,av+n*hj,n*2*sizeof(D)), !eqz(n,v,av+n*hj)               )
// CMPLX list
static IOFT(Z,jtioz1,THASHA, TFINDXY,TFINDYY,memcmp(v,av+n*hj,  2*sizeof(D)), !zeq( *v,av[hj] )               )
// FL array
static IOFT(D,jtiod, THASHA, TFINDXY,TFINDYY,memcmp(v,av+n*hj,n*  sizeof(D)), !eqd(n,v,av+n*hj)               )
// FL list
// should use macro for teq
static IOFT(D,jtiod1,THASHA, TFINDXY,TFINDY1,x!=av[hj],                       !teq(x,av[hj] )                 )
// boxed array with more than 1 box
static IOFT(A,jtioa, THASHBX,TFINDBX,TFINDBX,!eqa(n,v,av+n*hj,d,ad),          !eqa(n,v,av+n*hj,d,ad)          )
// singleton box
static IOFT(A,jtioa1,THASHBX,TFINDBX,TFINDBX,!equ(AADR(d,*v),AADR(ad,av[hj])),!equ(AADR(d,*v),AADR(ad,av[hj])))

// ********************* third class: small-range arguments ****************************

#if 0
// create the value vector

// v0 is the EMPTY value.  Clear to empty, then go through and set TRUE for each value found.  Used for [: u e.  where the position doesn't matter
#define SDO(v0)         if(mode<IPHOFFSET){B v1=!(v0); memset(hv,v0,p); u=av; DO(m, hb[*u++]=v1;);}
// Clear to empty, then go through and remember the index for each value.  Leaves last index, so used for i:
#define SDOA            if(mode<IPHOFFSET){DO(p,hv[i]=m; ); u=av;   DO(m,hu[*u++]=i;);}
// Clear to empty, then go through and remember the index for each value.  Reverse order, so leaves first index, so used for i.
#define SDQA            if(mode<IPHOFFSET){DO(p,hv[i]=m; ); u=av+m; DQ(m,hu[*--u]=i;);}

// Execute stmt on each cell of w.  stmt is responsible for incrementing input and output pointers
#define SDOW(stmt)      {u=wv;   DO(cm, stmt;);}

// loop through the items of w, creating the output.  hh->the value vector

// first, versions for i. i: e.    zz is an lvalue that stores to *zz++, vv is the value to use for not-found
// this version (used for 1- and 2-byte values) we just store the value from the value vector
#define SCOZ(hh,zz,vv)  {u=wv;   DO(cm, zz=hh[*u++];                      );}
// This version for fullword values (which have a partial table); use the table only if the value is represented there
#define SCOZ1(hh,zz,vv) {u=wv;   DO(cm, x=*u++; zz=min<=x&&x<max?hh[x]:vv;);}

// for u@e. - for each item of w, see if it is in the value table, execute stmt if so.  Save w value in x before executing stmt
#define SCOW1(hh,stmt)  {u=wv;   DO(cm, x=*u++; if(min<=x&&x< max&&hh[x     ]){stmt;});}
// reversed: execute stmt when the cell of w is NOT in the value table
#define SCOW0(hh,stmt)  {u=wv;   DO(cm, x=*u++; if(x<min ||max<=x||hh[x     ]){stmt;});}
// faster version of SCOW1 for use when the table contains all possible values
#define SCOWX(hh,stmt)  {u=wv;   DO(cm,         if(                hh[x=*u++]){stmt;});}
// still faster version of SCOWX, don't bother setting x
#define SCOW(hh,stmt)   {u=wv;   DO(cm,         if(                hh[  *u++]){stmt;});}

// just like SCOW1/SCOW0/SCOW, but scanning from the end of w
#define SCQW(hh,stmt)   {u=wv+c; DQ(cm,         if(                hh[  *--u]){stmt;});}
#define SCQW1(hh,stmt)  {u=wv+c; DQ(cm, x=*--u; if(min<=x&&x< max&&hh[x     ]){stmt;});}
#define SCQW0(hh,stmt)  {u=wv+c; DQ(cm, x=*--u; if(x<min ||max<=x||hh[x     ]){stmt;});}

// Do the operation on small-range arguments
// COZ1 is the result loop for i. i: e.
// COW0 is the result loop for (e. i. 0:)  ([: *./ e.)
// COW1 is the result loop for (e. i. 1:)  ([: +./ e.) ([: +/ e.) ([: I. e.)
// COWX is the result loop for -.
// CQW0 is the result loop for (e. i: 0:)
// CQW1 is the result loop for (e. i: 1:)
// cm is the number of cells of w per cell of a 
// clear the value table; 
#define IOFSMALLRANGE(f,T,COZ1,COW0,COW1,COWX,CQW0,CQW1)    \
 IOF(f){A h=*hp;B b,*hb,*zb;I cm,e,*hu,*hv,l,max,md,min,p,s,*v,*zi,*zv;T*av,*u,*wv,x,*zu;       \
  md=mode<IPHOFFSET?mode:mode-IPHOFFSET; b=(mode==IIDOT||mode==IICO)&&a==w&&ac==wc;             \
  av=(T*)AV(a); wv=(T*)AV(w); zv=zi=AV(z); zb=(B*)zv; zu=(T*)zv;                                \
  p=AN(h); min=jt->min; max=p+min; hv=AV(h); hb=(B*)hv-min; hu=hv-min;                          \
  e=1==wc?0:c; cm=w==mark?0:c;                                                                  \
  for(l=0;l<ac;++l,av+=m,wv+=e){                                                                \
   if(b){                                                                                       \
    if(mode==IIDOT){DO(p,hv[i]=-1;); u=wv;          DO(m, v=hu+*u++; if(0>*v)*v=i; *zv++=*v;);       }  \
    else           {DO(p,hv[i]=-1;); u=wv+m; zv+=m; DQ(m, v=hu+*--u; if(0>*v)*v=i; *--zv=*v;); zv+=m;}  \
   }else switch(md){                                                                            \
    case INUBSV:  memset(hv,C1,p); SDOW(if(*zb++=hb[x=*u++])hb[x]=0);                   break;  \
    case INUB:    memset(hv,C1,p); SDOW(if(hb[x=*u++]){*zu++=x; hb[x]=0;}); ZUSHAPE(T); break;  \
    case INUBI:   memset(hv,C1,p); SDOW(if(hb[x=*u++]){*zi++=i; hb[x]=0;}); ZISHAPE;    break;  \
    case IIDOT:   SDQA;         COZ1(hu, *zv++, m);                                     break;  \
    case IICO:    SDOA;         COZ1(hu, *zv++, m);                                     break;  \
    case IEPS:    SDO(C0);      COZ1(hb, *zb++, 0);                                     break;  \
    case ILESS:   SDO(C1);      COWX(hb, *zu++=x   );                       ZUSHAPE(T); break;  \
    case II0EPS:  SDO(C1); s=c; COW0(hb, s=i; break); *zv++=s;                          break;  \
    case II1EPS:  SDO(C0); s=c; COW1(hb, s=i; break); *zv++=s;                          break;  \
    case IJ0EPS:  SDO(C1); s=c; CQW0(hb, s=i; break); *zv++=s;                          break;  \
    case IJ1EPS:  SDO(C0); s=c; CQW1(hb, s=i; break); *zv++=s;                          break;  \
    case IANYEPS: SDO(C0); s=0; COW1(hb, s=1; break); *zb++=1&&s;                       break;  \
    case IALLEPS: SDO(C1); s=1; COW0(hb, s=0; break); *zb++=1&&s;                       break;  \
    case ISUMEPS: SDO(C0); s=0; COW1(hb, ++s       ); *zv++=s;                          break;  \
    case IIFBEPS: SDO(C0);      COW1(hb, *zi++=i   );                       ZISHAPE;    break;  \
  }}                                                                                            \
  R z;                                                                                          \
 }

// The verbs to do the work, for different item lengths
static IOFSMALLRANGE(jtio1,UC,SCOZ, SCOW, SCOW, SCOWX,SCQW, SCQW )  /* 1-byte    items */
static IOFSMALLRANGE(jtio2,US,SCOZ, SCOW, SCOW, SCOWX,SCQW, SCQW )  /* 2-byte    items */
static IOFSMALLRANGE(jtio4,I ,SCOZ1,SCOW0,SCOW1,SCOW0,SCQW0,SCQW1)  /* word size items */

#else  // *********************** new version **************************
// Macros to convert bit index to byte index and bit#.  We address to the byte to avoid rounding problems, as long as the compiler doesn't extend
#define BYTENO(b) ((b)>>3)
#define BITNO(b) ((b)&7)

// nub support, which creates & processes the bit-vector in one pass.  FULL is immaterial.
// T is the type for the result vector, stmt creates the result
// unpacked version expects default of 1
#define SNUB(decl, stmt) I zi=0, zie=m;  decl  UC* RESTRICT hu=hh->data.UC-min; while(zi!=zie){UC v=hu[wv[zi]]; hu[wv[zi]]=0;  stmt  ++zi;}
// packed version.  Default of 0
#define SNUBP(decl, stmt) I zi=0, zie=m;  decl  UC* RESTRICT hu=hh->data.UC-BYTENO(min); while(zi!=zie){UC v=hu[BYTENO(wv[zi])]; hu[BYTENO(wv[zi])]|=1<<BITNO(wv[zi]); v >>=BITNO(wv[zi]); v&=1; v^=1; stmt  ++zi;}

// creation of the value vector

// The bitmask was cleared to 0 by hashalloc
// Boolean/bit hashtables.  Used  where the position doesn't matter, i. e. for all but i./i: 
// Set TRUE for each value found.  hh->currentlo will always be 0.  zi starts at 0, since values don't matter
// The value in the cell is 0 or 1, since we don't care what the original position was
// We init the table to 0 or 1 depending on the primitive; but we always use 0 for PACKed bits, because it's never right to add
// an instruction to building the table
// obsolete #define SDO(T)  I zi=0, zi0=zi, zie=m; UC* RESTRICT hu=hh->data.UC-min; /* biased start,end+1 index, and data pointers */ \
// obsolete      if(!(mode&IPHOFFSET)){T* RESTRICT mav=av; while(zi!=zie){hu[mav[zi]]=1; ++zi;} zi=0;}
#define SDO(T,bitdef)  UC* RESTRICT hu=hh->data.UC-min; if(!(mode&IPHOFFSET)){T* RESTRICT mav=av; DO(m, hu[mav[i]]=1-bitdef;)}
#define SDOP(T)  UC* RESTRICT hu=hh->data.UC-BYTENO(min); if(!(mode&IPHOFFSET)){T* RESTRICT mav=av; DO(m, hu[BYTENO(mav[i])]|=1<<BITNO(mav[i]);)}

// US/UI4 hashtables
// the value in the cell indicates the original input position; the index of the cell indicates the input value
// Go through and remember the index for each value.  Leaves last index, so used for i:.  Leave zi correct at end
#define SDOA(T,Ttype) I zi=hh->currentindexofst, zi0=zi, zie=zi+m; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; /* biased start,end+1 index, and data pointers */ \
                           if(!(mode&IPHOFFSET)){T* RESTRICT mav=av-zi; while(zi!=zie){hu[mav[zi]]=(Ttype)zi; ++zi;} zi=zi0;}
// No faster version for BASE0
// Go through and remember the index for each value.  Reverse order, so leaves first index, so used for i.
#define SDQA(T,Ttype) I zi=hh->currentindexofst, zi0=zi, zie=zi+m; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo;  \
                           if(!(mode&IPHOFFSET)){T* RESTRICT mav=av-zi; do{--zie; hu[mav[zie]]=(Ttype)zie;}while(zie!=zi);}
// This version if we know the area starts at 0
#define SDQA0(T,Ttype) I zie=m-1; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo;  \
                           if(!(mode&IPHOFFSET)){do{hu[av[zie]]=(Ttype)zie;}while(--zie>=0);}

// using the value vector: loop through the items of w, creating the output.
// if FULL is set, there is no need for range-checking the input

// first, versions for i. i:     vv is the value to use for not-found (always m)
// This version for I values (which have a partial table); use the table only if the value is represented there.  vv is the not-found value.  Install it in default position
// Since the table is not FULL, the range scan of must have aborted; we scan forward from the beginning of w
#define SCOZ(T,Ttype,vv) {I * RESTRICT zv=AV(z)+l*c-zi; T* RESTRICT mwv=wv-zi; Ttype def[1]; zie=zi+c; def[0]=(Ttype)(vv+zi0); \
                          while(zi!=zie){T v=mwv[zi]; Ttype *hv=hu+v; if(v<min)hv=def; if(v>max)hv=def; I hvv; if((hvv=(I)*hv-zi0)<0)hvv=vv; zv[zi]=(Ttype)hvv; ++zi;}}
 /* printf("v=%d, hv=0x%p, def=0x%p, zi0=%d, *hv=%d, hvv=%d\n",v,hv,def,zi0,*hv,hvv); */
// this version is used when the result vector is known to be full (out-of-range not possible).  Omit the out-of-bounds check.  Since the range-scan of w ran to completion,
// the end of w is most likely to be in-cache; so scan backwards
#define SCOZF(T,Ttype,vv)  {I * RESTRICT zv=AV(z)+l*c-zi; T* RESTRICT mwv=wv-zi; zie=zi+c; while(zi!=zie){--zie; I hvv; if((hvv=(I)hu[mwv[zie]]-zi0)<0)hvv=vv; zv[zie]=(Ttype)hvv;}}
// these versions are used if the table is known to start at offset 0.  Omit the not-found check, since m was already loaded; and zi is 0
#define SCOZ0(T,Ttype,vv) {I zi = -c; I * RESTRICT zv=AV(z)+l*c-zi; T* RESTRICT mwv=wv-zi; Ttype def[1]; def[0]=(Ttype)vv; \
                          while(zi){T v=mwv[zi]; Ttype *hv=hu+v; if(v<min)hv=def; if(v>max)hv=def; zv[zi]=*hv; ++zi;}}
#define SCOZF0(T,Ttype,vv) {zie=c-1; I * RESTRICT zv=AV(z)+l*c; while(zie>=0){zv[zie]=hu[wv[zie]]; --zie;}}  // backwards scan

// for e. -. u@e. - for each item of w, see if it is in the value table.  Set v to the value read from the table (1 if in table).
// The table is always allocated as a bit vector and therefore is at offset 0 in the table
// wv[i] is the data value read, if that's needed
// should revise loops to count up from -n to 0, saving 1 inst
// Declare the result vector, and prebias it for our loop if needed.  indexed is 1 if zv will be referred to as zv[i], 0 if by *zv
#define DCLZVO(T,indexed) T * RESTRICT zv=T##AV(z)+(l+indexed)*c;
#define SCOW(T,bitdef,stmt)  {UC def[1]; def[0]=bitdef; I i; T *mwv=wv+c; for(i=-c;i<0;++i){T x=mwv[i]; UC *hv=hu+x; if(x<min)hv=def; if(x>max)hv=def; UC v=*hv; stmt}}
// faster version of SCOW for use when the table contains all possible values
#define SCOWF(T,bitdef,stmt)  {I i; T *mwv=wv+c; for(i=-c;i<0;++i){UC v=hu[mwv[i]]; stmt}}
// packed version
#define SCOWP(T,bitdef,stmt)  {UC def[1]; def[0]=0; I i; T *mwv=wv+c; for(i=-c;i<0;++i){T x=mwv[i]; UC *hv=hu+BYTENO(x); if(x<min)hv=def; if(x>max)hv=def; UC v=((*hv>>BITNO(x))&1)^bitdef; stmt}}
// packed full version
#define SCOWPF(T,bitdef,stmt)  {I i; T *mwv=wv+c; for(i=-c;i<0;++i){T x=mwv[i]; UC *hv=hu+BYTENO(x); UC v=((*hv>>BITNO(x))&1)^bitdef; stmt}}

// just like SCOW/SCOWF but scanning from the end of w
#define DCLZVQ(T,unused) T * RESTRICT zv=T##AV(z)+l*c;
#define SCQW(T,bitdef,stmt)  {UC def[1]; def[0]=bitdef; I i; for(i=c-1;i>=0;--i){T x=wv[i]; UC *hv=hu+x; if(x<min)hv=def; if(x>max)hv=def; UC v=*hv; stmt}}
#define SCQWF(T,bitdef,stmt)  {I i; for(i=c-1;i>=0;--i){UC v=hu[wv[i]]; stmt}}
#define SCQWP(T,bitdef,stmt)  {UC def[1]; def[0]=0; I i; for(i=c-1;i>=0;--i){T x=wv[i]; UC *hv=hu+BYTENO(x); if(x<min)hv=def; if(x>max)hv=def; UC v=(*hv>>BITNO(x))&1; stmt}}
#define SCQWPF(T,bitdef,stmt)  {I i; for(i=c-1;i>=0;--i){T x=wv[i]; UC *hv=hu+BYTENO(x); UC v=((*hv>>BITNO(x))&1)^bitdef; stmt}}

// Create basic/FULL/PACK/FULL+PACK versions (used for all boolean maps)
#define SMCASE0(casename, text) case casename: text break;
#define SMCASEF(casename, text) case IIMODFULL+casename: text break;
#define SMCASEP(casename, text) case IIMODPACK+casename: text break;
#define SMCASE1(casename, text) case IIMODFULL+IIMODPACK+casename: text break;
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
 IOF(f){IH *hh=IHAV(*hp);I e,l;T* RESTRICT av,* RESTRICT wv;T max,min; UI p; \
  mode|=((mode&(IIOPMSK&~(IIDOT^IICO)))|((I)a^(I)w)|(ac^wc))?0:IIMODREFLEX; \
  av=(T*)AV(a); wv=(T*)AV(w); \
  min=(T)hh->datamin; p=hh->datarange; max=min+(T)p-1; /* scaf printf("min=%d, max=%d, mode=0x%x\n",min,max,mode);*/\
  e=1==wc?0:c; if(w==mark){c=0; mode|=IPHCALC;} \
  for(l=0;l<ac;++l,av+=m,wv+=e){ \
   if(!(mode&(IPHOFFSET|IPHCALC))){mode = hashallo(hh,p,m,mode);}  /* set parms for this loop - only if not prehashing or using prehashed table, which always start at offset 0 */ \
   switch(mode&(IIOPMSK|IIMODFULL|IIMODPACK|IIMODBASE0)){ /* We know the setting of IIMODBITS without looking */ \
   default: ASSERTSYS(0,"switch failure in i."); \
     /* a lot of the l*c in the cases below could be removed because the verbs lack IRS, but we're leaving them in for now */ \
     /* reflexives, which include nubs, do not require a FULL version.  i.~ and i:~ don't need PACK versions either, since they deal in full indexes only */ \
   case IIMODREFLEX+IIMODFULL+IIDOT: \
   case IIMODREFLEX+IIDOT: \
     {I zi=hh->currentindexofst, zi0=zi, zie=zi+m;  I * RESTRICT zv=AV(z)+l*m-zi; T* RESTRICT mwv=wv-zi; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; /* biased start,end+1 index, and data pointers */ \
      while(zi!=zie){I vv = hu[mwv[zi]]; if(vv<zi0)vv=zi; hu[mwv[zi]]=(Ttype)vv; zv[zi]=vv-zi0;  ++zi;} } break; /* scan sequentially; if prev value present, rewrite it, otherwise write index */ \
   case IIMODBASE0+IIMODREFLEX+IIMODFULL+IIDOT: \
   case IIMODBASE0+IIMODREFLEX+IIDOT: \
     {I zi=0;  I * RESTRICT zv=AV(z)+l*m; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; /* biased start,end+1 index, and data pointers */ \
      while(zi!=m){I vv = hu[wv[zi]]; if(vv==m)vv=zi; hu[wv[zi]]=(Ttype)vv; zv[zi]=vv;  ++zi;} } break; /* scan sequentially; if prev value present, rewrite it, otherwise write index */ \
   case IIMODREFLEX+IIMODFULL+IICO: \
   case IIMODREFLEX+IICO: \
     {I zi=hh->currentindexofst, zi0=zi, zie=zi+m;  I * RESTRICT zv=AV(z)+l*m-zi; T* RESTRICT mwv=wv-zi; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; \
      do{--zie; I vv = hu[mwv[zie]]; if(vv<zi0)vv=zie; hu[mwv[zie]]=(Ttype)vv; zv[zie]=vv-zi0;}while(zie!=zi); } break; /* same in reverse */ \
   case IIMODBASE0+IIMODREFLEX+IIMODFULL+IICO: \
   case IIMODBASE0+IIMODREFLEX+IICO: \
     {I zie=m-1;  I * RESTRICT zv=AV(z)+l*m; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; \
      do{I vv = hu[wv[zie]]; if(vv==m)vv=zie; hu[wv[zie]]=(Ttype)vv; zv[zie]=vv; --zie; }while(zie>=0); } break; /* same in reverse */ \
 \
    /* NUB types use Boolean masks but do not depend on FULL, since they cannot miss.  But they must support PACK. */ \
   case IIMODFULL+INUBSV: \
   case INUBSV:            {SNUB (B * RESTRICT zv=BAV(z)+l*m;  ,  zv[zi]=v;) } break; \
   case IIMODPACK+IIMODFULL+INUBSV: \
   case IIMODPACK+INUBSV:  {SNUBP(B * RESTRICT zv=BAV(z)+l*m;  ,  zv[zi]=v;) } break; \
   case IIMODFULL+INUB: \
   case INUB:              {SNUB (T * RESTRICT zv=(T*)AV(z); T *zv0=zv;   ,  *zv=wv[zi]; zv+=v;) *AS(z)=zv-zv0; AN(z)=n*(zv-zv0); } break;  \
   case IIMODPACK+IIMODFULL+INUB: \
   case IIMODPACK+INUB:    {SNUBP(T * RESTRICT zv=(T*)AV(z); T *zv0=zv;   ,  *zv=wv[zi]; zv+=v;) *AS(z)=zv-zv0; AN(z)=n*(zv-zv0); } break;  \
   case IIMODFULL+INUBI: \
   case INUBI:             {SNUB (I * RESTRICT zv=AV(z); I *zv0=zv;   ,   *zv=zi; zv+=v;) *AS(z)=AN(z)=zv-zv0; } break;  \
   case IIMODPACK+IIMODFULL+INUBI: \
   case IIMODPACK+INUBI:   {SNUBP(I * RESTRICT zv=AV(z); I *zv0=zv;   ,   *zv=zi; zv+=v;) *AS(z)=AN(z)=zv-zv0; } break;  \
     /* non-reflexives can benefit from FULL checking.  IIDOT and IICO need full indexes (and thus use BASE0, but no PACK); everything else is Boolean */\
   case IIDOT:             {SDQA(T,Ttype);  SCOZ(T,Ttype,m);} break;  \
   case IIMODFULL+IIDOT:   {SDQA(T,Ttype);  SCOZF(T,Ttype,m);} break;  \
   case IIMODBASE0+IIDOT:  {SDQA0(T,Ttype);  SCOZ0(T,Ttype,m);} break;  \
   case IIMODBASE0+IIMODFULL+IIDOT: {SDQA0(T,Ttype);  SCOZF0(T,Ttype,m);} break;  \
   case IICO:              {SDOA(T,Ttype);  SCOZ(T,Ttype,m);} break;  \
   case IIMODFULL+IICO:    {SDOA(T,Ttype); SCOZF(T,Ttype,m);} break;  \
   case IIMODBASE0+IICO:   {SDOA(T,Ttype);  SCOZ0(T,Ttype,m);} break;  \
   case IIMODBASE0+IIMODFULL+IICO: {SDOA(T,Ttype); SCOZF0(T,Ttype,m);} break;  \
    /* Boolean indexes from here on.  These must support FULL and PACK */ \
   SMFULLPACKEPS(T,0,IEPS,  (T,0,zv[i]=v;)) /* EPS scans FULL args backwards for cache coherence */ \
   SMFULLPACK(T,1,ILESS,  DCLZVO(T,0) T *zv0=zv; , (T,1,*zv=mwv[i]; zv+=v;); *AS(z)= zv-zv0; AN(z)=n*(zv-zv0);)  \
   SMFULLPACK(T,0,II0EPS, DCLZVO(I,0) I s=c; , (T,0,if(!v){s+=i; break;});*zv++=s;)  \
   SMFULLPACK(T,0,II1EPS, DCLZVO(I,0) I s=c; , (T,0,if(v){s+=i; break;});*zv++=s; )  \
   SMFULLPACQ(T,0,IJ0EPS, DCLZVQ(I,0) I s=c; , (T,0,if(!v){s=i; break;});*zv++=s;)  \
   SMFULLPACQ(T,0,IJ1EPS, DCLZVQ(I,0) I s=c; , (T,0,if(v){s=i; break;}); *zv++=s;)  \
   SMFULLPACK(T,0,IANYEPS,DCLZVO(B,0) B s=0; , (T,0,if(v){s=1; break;}); *zv++=s;)  \
   SMFULLPACK(T,0,IALLEPS,DCLZVO(B,0) B s=1; , (T,0,if(!v){s=0; break;}); *zv++=s;)  \
   SMFULLPACK(T,0,ISUMEPS,DCLZVO(I,0) I s=0; , (T,0,s+=v;); *zv++=s;)  \
   SMFULLPACK(T,0,IIFBEPS,DCLZVO(I,0) I *zv0=zv; , (T,0,*zv=i+c; zv+=v;); *AS(z)=AN(z)=zv-zv0;)  \
   }  \
  }  \
  R z; \
 }
// init to 1 for ~. ~: NUBI LESS   others to 0
#if 0
/* scaf printf("hu[mwv[zi]]=%d ",hu[mwv[zi]]); *//* scaf printf("mwv[zi]=%d, vv=%d\n",mwv[zi],vv); */
/* scaf printf("zi=%d, zie=%d\n",zi,zie); */\
/* scaf   ASSERTSYS(zi0==hh->currentindexofst,"zi0 error"); ASSERTSYS(mwv[zi]>=min,"value too low"); ASSERTSYS(mwv[zi]<=max,"value too high"); ASSERTSYS(vv<(I)hh->currentindexend,"vv too high"); */ \
/* scaf   ASSERTSYS(hh->currentindexend-hh->currentindexofst==m,"range not same as m"); scaf*/ \

#define SCOZ(T,Ttype,vv) {T* RESTRICT mwv=wv-zi; Ttype def[1]; zie=zi+c; def[0]=(Ttype)(vv+zi0); \
                          while(zi!=zie){T v=mwv[zi]; Ttype *hv=hu+v; if(v<min)hv=def; if(v>max)hv=def; I hvv; if((hvv=(I)*hv-zi0)<0)hvv=vv; zv[zi]=(Ttype)hvv; ++zi;}}
 /* printf("v=%d, hv=0x%p, def=0x%p, zi0=%d, *hv=%d, hvv=%d\n",v,hv,def,zi0,*hv,hvv); */
// this version is used when the result vector is known to be full (out-of-range not possible).  Omit the out-of-bounds check.
#define SCOZF(T,Ttype,vv)  {T* RESTRICT mwv=wv-zi; zie=zi+c; while(zi!=zie){I hvv; if((hvv=(I)hu[mwv[zi]]-zi0)<0)hvv=vv; zv[zi]=(Ttype)hvv; ++zi;}}  // zi always starts at 0
// these versions are used if the table is known to start at offset 0.  Omit the not-found check, since m was already loaded; and zi is 0


#define SCQW(T,stmt)  {UC def[1]; def[0]=0; DQ(c, T x=wv[i]; UC *hv=hu+x; if(x<min)hv=def; if(x>max)hv=def; UC v=*hv; stmt)}
#define SCQWF(T,stmt)  {DQ(c, UC v=hu[wv[i]]; stmt)}
#define SCQWP(T,stmt)  {UC def[1]; def[0]=0; DQ(c, T x=wv[i]; UC *hv=hu+BYTENO(x); if(x<min)hv=def; if(x>max)hv=def; UC v=(*hv>>BITNO(x))&1; stmt)}
#define SCQWPF(T,stmt)  {DQ(c, T x=wv[i]; UC *hv=hu+BYTENO(x); UC v=(*hv>>BITNO(x))&1; stmt)}
   SMCASE0(IEPS,   {SDO(T); DCLZVO(B) SCOW (T,zv[i]=v;);}) \
   SMCASEF(IEPS,   {SDO(T); DCLZVO(B) SCOWF(T,zv[i]=v;);}) \
   SMCASE0(ILESS,  {SDO(T); DCLZVO(T) T *zv0=zv; SCOW (T,*zv=wv[i]; v^=1; zv+=v;); *AS(z)= zv-zv0; AN(z)=n*(zv-zv0);})  \
   SMCASEF(ILESS,  {SDO(T); DCLZVO(T) T *zv0=zv; SCOWF(T,*zv=wv[i]; v^=1; zv+=v;); *AS(z)= zv-zv0; AN(z)=n*(zv-zv0);})  \
   SMCASE0(II0EPS, {SDO(T); DCLZVO(I) I s=c; SCOW (T,if(!v){s=i; break;});*zv++=s;})  \
   SMCASEF(II0EPS, {SDO(T); DCLZVO(I) I s=c; SCOWF(T,if(!v){s=i; break;});*zv++=s;})  \
   SMCASE0(II1EPS, {SDO(T); DCLZVO(I) I s=c; SCOW (T,if(v){s=i; break;});*zv++=s; })  \
   SMCASEF(II1EPS, {SDO(T); DCLZVO(I) I s=c; SCOWF(T,if(v){s=i; break;});*zv++=s; })  \
   SMCASE0(IJ0EPS, {SDO(T); DCLZVQ(I) I s=c; SCQW (T,if(!v){s=i; break;});*zv++=s;})  \
   SMCASEF(IJ0EPS, {SDO(T); DCLZVQ(I) I s=c; SCQWF(T,if(!v){s=i; break;});*zv++=s;})  \
   SMCASE0(IJ1EPS, {SDO(T); DCLZVQ(I) I s=c; SCQW (T,if(v){s=i; break;}); *zv++=s;})  \
   SMCASEF(IJ1EPS, {SDO(T); DCLZVQ(I) I s=c; SCQWF(T,if(v){s=i; break;}); *zv++=s;})  \
   SMCASE0(IANYEPS,{SDO(T); DCLZVO(B) B s=0; SCOW (T,if(v){s=1; break;}); *zv++=s;})  \
   SMCASEF(IANYEPS,{SDO(T); DCLZVO(B) B s=0; SCOWF(T,if(v){s=1; break;}); *zv++=s;})  \
   SMCASE0(IALLEPS,{SDO(T); DCLZVO(B) B s=1; SCOW (T,if(!v){s=0; break;}); *zv++=s;})  \
   SMCASEF(IALLEPS,{SDO(T); DCLZVO(B) B s=1; SCOWF(T,if(!v){s=0; break;}); *zv++=s;})  \
   SMCASE0(ISUMEPS,{SDO(T); DCLZVO(I) I s=0; SCOW (T,s+=v;); *zv++=s;})  \
   SMCASEF(ISUMEPS,{SDO(T); DCLZVO(I) I s=0; SCOWF(T,s+=v;); *zv++=s;})  \
   SMCASE0(IIFBEPS,{SDO(T); DCLZVO(I) I *zv0=zv; SCOW (T,*zv=i; zv+=v;); *AS(z)=AN(z)=zv-zv0;})  \
   SMCASEF(IIFBEPS,{SDO(T); DCLZVO(I) I *zv0=zv; SCOWF(T,*zv=i; zv+=v;); *AS(z)=AN(z)=zv-zv0;})  
//   /* case IEPS:               {SDO(T); DCLZVO(B) SCOW(T,zv[i]=v;);}  break; */ \
//   /* case IIMODFULL+IEPS:     {SDO(T); DCLZVO(B) SCOWF(T,zv[i]=v;);}  break; */  \
//   case ILESS:              {SDO(T); DCLZVO(T) T *zv0=zv; SCOW(T,*zv=wv[i]; v^=1; zv+=v;); *AS(z)= zv-zv0; AN(z)=n*(zv-zv0);} break;  \
//   case IIMODFULL+ILESS:    {SDO(T); DCLZVO(T) T *zv0=zv; SCOWF(T,*zv=wv[i]; v^=1; zv+=v;); *AS(z)= zv-zv0; AN(z)=n*(zv-zv0);} break;  \
//   case II0EPS:             {SDO(T); DCLZVO(I) I s=c; SCOW(T,if(!v){s=i; break;});*zv++=s;} break;  \
//   case IIMODFULL+II0EPS:   {SDO(T); DCLZVO(I) I s=c; SCOWF(T,if(!v){s=i; break;});*zv++=s;} break;  \
//   case II1EPS:             {SDO(T); DCLZVO(I) I s=c; SCOW(T,if(v){s=i; break;});*zv++=s; } break;  \
//   case IIMODFULL+II1EPS:   {SDO(T); DCLZVO(I) I s=c; SCOWF(T,if(v){s=i; break;});*zv++=s; } break;  \
//   case IJ0EPS:             {SDO(T); DCLZVQ(I) I s=c; SCQW(T,if(!v){s=i; break;});*zv++=s;} break;  \
//   case IIMODFULL+IJ0EPS:   {SDO(T); DCLZVQ(I) I s=c; SCQWF(T,if(!v){s=i; break;});*zv++=s;} break;  \
//   case IJ1EPS:             {SDO(T); DCLZVQ(I) I s=c; SCQW(T,if(v){s=i; break;}); *zv++=s;} break;  \
//   case IIMODFULL+IJ1EPS:   {SDO(T); DCLZVQ(I) I s=c; SCQWF(T,if(v){s=i; break;}); *zv++=s;} break;  \
//   case IANYEPS:            {SDO(T); DCLZVO(B) B s=0; SCOW(T,if(v){s=1; break;}); *zv++=s;} break;  \
//   case IIMODFULL+IANYEPS:  {SDO(T); DCLZVO(B) B s=0; SCOWF(T,if(v){s=1; break;}); *zv++=s;} break;  \
//   case IALLEPS:            {SDO(T); DCLZVO(B) B s=1; SCOW(T,if(!v){s=0; break;}); *zv++=s;} break;  \
//   case IIMODFULL+IALLEPS:  {SDO(T); DCLZVO(B) B s=1; SCOWF(T,if(!v){s=0; break;}); *zv++=s;} break;  \
//   case ISUMEPS:            {SDO(T); DCLZVO(I) I s=0; SCOW(T,s+=v;       ); *zv++=s;} break;  \
//   case IIMODFULL+ISUMEPS:  {SDO(T); DCLZVO(I) I s=0; SCOWF(T,s+=v;       ); *zv++=s;} break;  \
//   case IIFBEPS:            {SDO(T); DCLZVO(I) I *zv0=zv; SCOW(T,*zv=i; zv+=v;); *AS(z)=AN(z)=zv-zv0;}     break;  \
//  case IIMODFULL+IIFBEPS:  {SDO(T); DCLZVO(I) I *zv0=zv; SCOWF(T,*zv=i; zv+=v;); *AS(z)=AN(z)=zv-zv0;}     break;  \
//   case IEPS:    {SDO(T); B * RESTRICT zv=BAV(z)+l*c; SCOZ(T,UC,0);}  break;  
//   case IIMODFULL+IEPS:    {SDO(T); B * RESTRICT zv=BAV(z)+l*c; SCOZ(T,UC,0);}  break;  
//  {I zi=0, zie=m;  B * RESTRICT zv=BAV(z)+l*m; UC* RESTRICT hu=hh->data.UC-min; /* biased start,end+1 index, and data pointers */ 
//                 while(zi!=zie){UC v=hu[wv[zi]]; hu[wv[zi]]=1; v^=1; zv[zi]=v; ++zi;} } break;  
//    {I zi=0, zie=m;  T * RESTRICT zv=(T*)AV(z), *zv0=zv; UC* RESTRICT hu=hh->data.UC-min; /* biased start,end+1 index, and data pointers */ 
//                 while(zi!=zie){UC v=hu[wv[zi]]; hu[wv[zi]]=1; v^=1; *zv=wv[zi]; zv+=v; ++zi;} *AS(z)=zv-zv0; AN(z)=n*(zv-zv0); } break;  
//   {I zi=0, zie=m;  I * RESTRICT zv=AV(z), *zv0=zv; UC* RESTRICT hu=hh->data.UC-min; /* biased start,end+1 index, and data pointers */ 
//                 while(zi!=zie){UC v=hu[wv[zi]]; hu[wv[zi]]=1; v^=1; *zv=zi; zv+=v; ++zi;} *AS(z)=AN(z)=zv-zv0; } break;
#define f jtio4
#define T UC
#define Ttype US
 IOF(f){IH *hh=IHAV(*hp);I e,l,md;T*av,*wv;T max,min,p; 
  md=(mode&~-IPHOFFSET); md|=(((mode&~((IIDOT^IICO)|IIMODFIELD|IPHOFFSET))|((I)a^(I)w)|(ac^wc))?0:IIMODREFLEX); 
  av=(T*)AV(a); wv=(T*)AV(w); 
  min=(T)hh->currentlo; p=(T)hh->currentindexofst; max=min+p-1; 
  e=1==wc?0:c; if(w==mark)c=0; 
  for(l=0;l<ac;++l,av+=m,wv+=e){ 
   if(!(mode&IPHOFFSET)){hashallo(hh,p,m,md);}  /* set parms for this loop */ 
   switch(md){ 
   case IIMODREFLEX+IIDOT: {I zi=hh->currentindexofst, zi0=zi, zie=zi+m;  I * RESTRICT zv=AV(z)+l*m; T* RESTRICT mwv=wv-zi; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; /* biased start,end+1 index, and data pointers */ 
                           while(zi!=zie){I vv = hu[mwv[zi]]; if(vv<zi0)vv=zi; hu[mwv[zi]]=(Ttype)vv; zv[zi]=vv-zi0; ++zi;} } break; /* scan sequentially; if prev value present, rewrite it, otherwise write index */ 
   case IIMODREFLEX+IICO: {I zi=hh->currentindexofst, zi0=zi, zie=zi+m;  I * RESTRICT zv=AV(z)+l*m; T* RESTRICT mwv=wv-zi; Ttype* RESTRICT hu=hh->data.Ttype-min+hh->currentlo; 
                           do{--zie; I vv = hu[mwv[zie]]; if(vv<zi0)vv=zie; hu[mwv[zie]]=(Ttype)vv; zv[zie]=vv-zi0;}while(zie!=zi); } break; /* same in reverse */ 
   case IIDOT:   {SDQA(T,Ttype);  I * RESTRICT zv=AV(z)+l*m-zi; SCOZ(T,Ttype,m);} break;  
   case IIMODFULL+IIDOT:   {SDQA(T,Ttype);  I * RESTRICT zv=AV(z)+l*m-zi; SCOZ(T,Ttype,m);} break;  
   case IICO:    {SDOA(T,Ttype);  I * RESTRICT zv=AV(z)+l*m-zi; SCOZ(T,Ttype,m);} break;  
   case IIMODFULL+IICO:    {SDOA(T,Ttype);  I * RESTRICT zv=AV(z)+l*m-zi; SCOZ(T,Ttype,m);} break;  
   case IEPS:    {SDO(T); B * RESTRICT zv=BAV(z)+l*m; SCOZ(T,UC,0);}  break;  
   case IIMODFULL+IEPS:    {SDO(T); B * RESTRICT zv=BAV(z)+l*m; SCOZ(T,UC,0);}  break;  
   case IIMODFULL+INUBSV: 
   case INUBSV:  {I zi=0, zie=m;  B * RESTRICT zv=BAV(z)+l*m; T* RESTRICT mwv=wv; UC* RESTRICT hu=hh->data.UC-min; /* biased start,end+1 index, and data pointers */ 
                 while(zi!=zie){UC v=hu[wv[zi]]; hu[wv[zi]]=1; v^=1; zv[zi]=v; ++zi;} } break;  
   case IIMODFULL+INUB: 
   case INUB:    {I zi=0, zie=m;  T * RESTRICT zv=(T*)AV(z)+l*m, *zv0=zv; T* RESTRICT mwv=wv; UC* RESTRICT hu=hh->data.UC-min; /* biased start,end+1 index, and data pointers */ 
                 while(zi!=zie){UC v=hu[wv[zi]]; hu[wv[zi]]=1; *zv=wv[zi]; v^=1; zv+=v; ++zi;} *AS(z)= zv-zv0; AN(z)=n*(zv-zv0); } break;  
   case IIMODFULL+INUBI: 
   case INUBI:   {I zi=0, zie=m;  I * RESTRICT zv=AV(z)+l*m, *zv0=zv; T* RESTRICT mwv=wv; UC* RESTRICT hu=hh->data.UC-min; /* biased start,end+1 index, and data pointers */ 
                 while(zi!=zie){UC v=hu[wv[zi]]; hu[wv[zi]]=1; *zv=zi; v^=1; zv+=v; ++zi;} *AS(z)=AN(z)=zv-zv0; } break;  
   case ILESS:   {SDO(T); T * RESTRICT zv=(T*)AV(z)+l*m, *zv0=zv; SCOW(T,*zv=*mwv; v^=1; zv+=v;); *AS(z)= zv-zv0; AN(z)=n*(zv-zv0);} break;  
   case IIMODFULL+ILESS:   {SDO(T); T * RESTRICT zv=(T*)AV(z)+l*m, *zv0=zv; SCOW(T,*zv=*mwv; v^=1; zv+=v;); *AS(z)= zv-zv0; AN(z)=n*(zv-zv0);} break;  
   case II0EPS:  {SDO(T); I * RESTRICT zv=IAV(z)+l*m; I s=c; SCOW(T,if(!v){s=i; break;});*zv++=s;} break;  
   case IIMODFULL+II0EPS:  {SDO(T); I * RESTRICT zv=IAV(z)+l*m; I s=c; SCOW(T,if(!v){s=i; break;});*zv++=s;} break;  
   case II1EPS:  {SDO(T); I * RESTRICT zv=IAV(z)+l*m; I s=c; SCOW(T,if(v){s=i; break;});*zv++=s; } break;  
   case IIMODFULL+II1EPS:  {SDO(T); I * RESTRICT zv=IAV(z)+l*m; I s=c; SCOW(T,if(v){s=i; break;});*zv++=s; } break;  
   case IJ0EPS:  {SDO(T); I * RESTRICT zv=IAV(z)+l*m; I s=c; SCQW(T,if(!v){s=i; break;});*zv++=s;} break;  
   case IIMODFULL+IJ0EPS:  {SDO(T); I * RESTRICT zv=IAV(z)+l*m; I s=c; SCQW(T,if(!v){s=i; break;});*zv++=s;} break;  
   case IJ1EPS:  {SDO(T); I * RESTRICT zv=IAV(z)+l*m; I s=c; SCQW(T,if(v){s=i; break;}); *zv++=s;} break;  
   case IIMODFULL+IJ1EPS:  {SDO(T); I * RESTRICT zv=IAV(z)+l*m; I s=c; SCQW(T,if(v){s=i; break;}); *zv++=s;} break;  
   case IANYEPS: {SDO(T); B * RESTRICT zv=BAV(z)+l*m; B s=0; SCOW(T,if(v){s=1; break;}); *zv++=s;} break;  
   case IIMODFULL+IANYEPS: {SDO(T); B * RESTRICT zv=BAV(z)+l*m; B s=0; SCOW(T,if(v){s=1; break;}); *zv++=s;} break;  
   case IALLEPS: {SDO(T); B * RESTRICT zv=BAV(z)+l*m; B s=1; SCOW(T,if(!v){s=0; break;}); *zv++=s;} break;  
   case IIMODFULL+IALLEPS: {SDO(T); B * RESTRICT zv=BAV(z)+l*m; B s=1; SCOW(T,if(!v){s=0; break;}); *zv++=s;} break;  
   case ISUMEPS: {SDO(T); I * RESTRICT zv=IAV(z)+l*m; I s=0; SCOW(T,s+=v;       ); *zv++=s;} break;  
   case IIMODFULL+ISUMEPS: {SDO(T); I * RESTRICT zv=IAV(z)+l*m; I s=0; SCOW(T,s+=v;       ); *zv++=s;} break;  
   case IIFBEPS: {SDO(T); I * RESTRICT zv=IAV(z)+l*m, *zv0=zv; SCOW(T,*zv=i; v^=1; zv+=v;); *AS(z)= zv-zv0; AN(z)=n*(zv-zv0);}     break;  
   case IIMODFULL+IIFBEPS: {SDO(T); I * RESTRICT zv=IAV(z)+l*m, *zv0=zv; SCOW(T,*zv=i; v^=1; zv+=v;); *AS(z)= zv-zv0; AN(z)=n*(zv-zv0);}     break;  
   }  
  }  
  R z; 
 }
#else

// The verbs to do the work, for different item lengths and hashtable sizes
static IOFSMALLRANGE(jtio12,UC,US)  static IOFSMALLRANGE(jtio14,UC,UI4)  // 1-byte items, using small/large hashtable
static IOFSMALLRANGE(jtio22,US,US)  static IOFSMALLRANGE(jtio24,US,UI4)  // 2-byte items, using small/large hashtable
static IOFSMALLRANGE(jtio42,I,US)  static IOFSMALLRANGE(jtio44,I,UI4)  // 4-byte items, using small/large hashtable
#endif
#endif
// ******************* fourth class: sequential comparison ***************************************

// we is the expression for reading one comparand, exp is the expression
// loop through storing the index at which a match was found
#define SCDO(T,xe,exp)  \
 {T*v0=(T*)v,*wv=(T*)v,x; \
  switch(mode){                     \
   case IIDOT: {T*av=(T*)u+m; DQ(ac, DQ(c, x=(xe); j=-m;   while(j<0 &&(exp))++j; *zv++=j+m;       wv+=q;); av+=p; if(1==wc)wv=v0;);} break;  \
   case IICO:  {T*av=(T*)u; DQ(ac, DQ(c, x=(xe); j=m-1; while(0<=j&&(exp))--j; *zv++=0>j?m:j; wv+=q;); av+=p; if(1==wc)wv=v0;);} break;  \
   case IEPS:  {T*av=(T*)u+m; DQ(ac, DQ(c, x=(xe); j=-m;   while(j<0 &&(exp))++j; *zb++=j<0;     wv+=q;); av+=p; if(1==wc)wv=v0;);} break;  \
 }}

static void jtiosc(J jt,I mode,I m,I c,I ac,I wc,A a,A w,A z){B*zb;I j,p,q,*u,*v,zn,*zv;
 p=1<ac?m:0; q=1<wc||1<c;
 zn=AN(z); 
 zv=AV(z); zb=(B*)zv; u=AV(a); v=AV(w); 
 switch(CTTZ(AT(a))){
  default:                SCDO(C, *wv,x!=av[j]      ); break;
  case C2TX:               SCDO(S, *wv,x!=av[j]      ); break;
  case C4TX:               SCDO(C4,*wv,x!=av[j]      ); break;
  case CMPXX:              SCDO(Z, *wv,!zeq(x, av[j])); break;
  case XNUMX:              SCDO(A, *wv,!equ(x, av[j])); break;
  case RATX:               SCDO(Q, *wv,!QEQ(x, av[j])); break;
  case INTX:               SCDO(I, *wv,x!=av[j]      ); break;
  case SBTX:               SCDO(SB,*wv,x!=av[j]      ); break;
  printf("before BOXX");
  case BOXX:  {RDECL;      SCDO(A, AADR(wd,*wv),!equ(x,AADR(ad,av[j])));} break;
  printf("after BOXX");
  case FLX:   if(0==jt->ct)SCDO(D, *wv,x!=av[j]) 
             else{D cct=1.0-jt->ct;    SCDO(D, *wv,!TCMPEQ(cct,x,av[j]));} break; 
 }
}    /* right argument cell is scalar; only for modes IIDOT IICO IEPS */


// ***************** fifth class: boxed arguments ************************
// should scrap this and do a recursive hash on the entire box, and use the normal hash code.  would require managing -0 in the hash for float/complex

// return 1 if a is boxed, and ct==0, and a contains a box whose contents are boxed, or complex, or numeric with more than one atom
static B jtusebs(J jt,A a,I ac,I m){A*av,x;I ad,t;
 if(!(BOX&AT(a)&&0==jt->ct))R 0;
 av=AAV(a); ad=(I)a*ARELATIVE(a);
 DO(ac*m, x=AVR(i); t=AT(x); if(t&BOX+CMPX||1<AN(x)&&t&NUMERIC)R 1;);
 R 0;
}    /* n (# elements in a target item) is assumed to be 1 */

// 
// b means self-index and running i. i: ~. ~: (I.@~.)
// bk means i: (e. i: 0:) (e. i: 1:)   or prehashed version thereof, i. e. backwards
// We grade a, producing the ordering permutation.  Then we go through it to discard duplicates
// should not do the duplicate-removal pass, since it requires an entire pass over the a argument; instead, check
//  for duplicates as matches are found, and mark when a check has been made.  Duplicate removal might be reasonable for prehashing
static A jtnodupgrade(J jt,A a,I acr,I ac,I acn,I ad,I n,I m,B b,B bk){A*av,h,*u,*v;I*hi,*hu,*hv,l,m1,q;
 RZ(h=irs1(a,0L,acr,jtgrade1)); hv=AV(h)+bk*(m-1); av=AAV(a);
 // if not self-index, close up the duplicates
 if(!b)for(l=0;l<ac;++l,av+=acn,hv+=m){  // for each item of the overall result
  // hi->next index in the grade result, q is its value, u->A block for that index
  // hu->next output position.  The first result always stays in place
  hi=hv; q=*hi; u=av+n*q;
  // loop through, setting q/v to the index/address of data.  If *v!=*u, accept q/v as a new value and set u=v
  // should not bother with testing equality if q<index of u (for ascending; reverse for descending)
  // if the list was shortened, replace the last position with 1-(length of shortened list).  This will be detected
  // and the sign changed to give (length of list)-1.  0 is OK too, indicating a 1-element list
  if(bk){hu=--hi; DO(m-1, q=*hi--; v=av+n*q; if(!eqa(n,u,v,ad,ad)){u=v; *hu--=q;}); m1=hv-hu; if(m>m1)hv[1-m]=1-m1;}
  else  {hu=++hi; DO(m-1, q=*hi++; v=av+n*q; if(!eqa(n,u,v,ad,ad)){u=v; *hu++=q;}); m1=hu-hv; if(m>m1)hv[m-1]=1-m1;}
 }
 R h;
} 

// hiinc will inc or dec hi
// zstmti initializes the first result index with the index of the smallest element
// zstmt1 is executed if v is a duplicate, and should repeat the previous result
// zstmt0 is executed if v is not a dup, and must advance the p marker to q as well as store the new result
// should not bother testing for equality if ascending order and q<p (since grade would preserve order if equal), or if descending and q>p
// should look into having a grade that discards dups
#define BSLOOPAA(hiinc,zstmti,zstmt1,zstmt0)  \
 {A* RESTRICT u=av,* RESTRICT v;I* RESTRICT hi=hv,p,q;             \
  p=*hiinc; u=av+n*p; zstmti;  /* u->first result value, install result for that value to index itself */      \
  DO(m-1, q=*hiinc; v=av+n*q; if(eqa(n,u,v,ad,ad))zstmt1; else{u=v; zstmt0;}); /* 
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
 {A* RESTRICT u=wv+n*(ii),* RESTRICT v;I i,j,p,q;int t;  \
  for(i=ii;icmp;iinc,uinc){          \
   p=0; q=m1;                        \
   while(p<=q){                      \
    t=0; j=(p+q)/2; v=av+n*hu[j];    \
    DO(n, if(t=compare(AADR(wd,u[i]),AADR(ad,v[i])))break;);  \
    if(0<t)p=j+1; else q=t?j-1:-2;   \
   }                                 \
   zstmt;                            \
 }}

// macros to create ascending or descending binary search, executing zstmt afterwards
#define BSLOOPAW(zstmt)     BSLOOPAWX(0  ,i< c,++i,u+=n,zstmt)
#define BSLOOQAW(zstmt)     BSLOOPAWX(c-1,i>=0,--i,u-=n,zstmt)

// index by sorting a into order, then doing binary search on each item of w.
// Used only when ct=0 and (boxed rank>1 or boxes contain numeric arrays)
// 
static IOF(jtiobs){A*av,h=*hp,*wv,y;B b,bk,*yb,*zb;C*zc;I acn,ad,*hu,*hv,l,m1,md,s,wcn,wd,*zi,*zv;
 bk=mode==IICO||mode==IJ0EPS||mode==IJ1EPS||mode==IPHICO||mode==IPHJ0EPS||mode==IPHJ1EPS;
 b=a==w&&ac==wc&&(mode==IIDOT||mode==IICO||mode==INUB||mode==INUBSV||mode==INUBI); 
 if(mode==INUB||mode==INUBI){GATV(y,B01,m,1,0); yb=BAV(y);}
 md=w==mark?-1:mode<IPHOFFSET?mode:mode-IPHOFFSET;
 av=AAV(a); ad=(I)a*ARELATIVE(a); acn=ak/sizeof(A);
 wv=AAV(w); wd=(I)w*ARELATIVE(w); wcn=wk/sizeof(A);
 zi=zv=AV(z); zb=(B*)zv; zc=(C*)zv;
 // If a has not been sorted already, sort it
 if(mode<IPHOFFSET)RZ(*hp=h=nodupgrade(a,acr,ac,acn,ad,n,m,b,bk));
 if(w==mark)R mark;
 hv=AV(h)+bk*(m-1); jt->complt=-1; jt->compgt=1;
 for(l=0;l<ac;++l,av+=acn,wv+=wcn,hv+=m){  // loop for each result in a
  // m1=index of last item, which may be less than m if there were discarded duplicates (signaled by last index <0)
  s=hv[bk?1-m:m-1]; m1=0>s?-s:m-1; hu=hv-m1*bk;
  if(b)switch(md){  // self-indexes
   case IIDOT:        BSLOOPAA(hi++,zv[p]=p,zv[q]=p,zv[q]=p=q); zv+=m;     break;
   case IICO:         BSLOOPAA(hi--,zv[p]=p,zv[q]=p,zv[q]=p=q); zv+=m;     break;
   case INUBSV:       BSLOOPAA(hi++,zb[p]=1,zb[q]=0,zb[q]=1  ); zb+=m;     break;
   case INUB:         BSLOOPAA(hi++,yb[p]=1,yb[q]=0,yb[q]=1  ); DO(m, if(yb[i]){MC(zc,av+i*n,k); zc+=k;}); ZCSHAPE; break;
   case INUBI:        BSLOOPAA(hi++,yb[p]=1,yb[q]=0,yb[q]=1  ); DO(m, if(yb[i])*zi++=i;);                  ZISHAPE; break;
  }else switch(md){  // searches, by binary search
   case IIDOT:        BSLOOPAW(*zv++=-2==q?hu[j]:m);                       break;
   case IICO:         BSLOOPAW(*zv++=-2==q?hu[j]:m);                       break;
   case IEPS:         BSLOOPAW(*zb++=-2==q);                               break;
   case ILESS:        BSLOOPAW(if(-2< q){MC(zc,u,k); zc+=k;}); ZCSHAPE;    break;
   case II0EPS:  s=m; BSLOOPAW(if(-2< q){s=i; break;});        *zi++=s;    break;
   case IJ0EPS:  s=m; BSLOOQAW(if(-2< q){s=i; break;});        *zi++=s;    break;
   case II1EPS:  s=m; BSLOOPAW(if(-2==q){s=i; break;});        *zi++=s;    break;
   case IJ1EPS:  s=m; BSLOOQAW(if(-2==q){s=i; break;});        *zi++=s;    break;
   case IANYEPS: s=0; BSLOOPAW(if(-2==q){s=1; break;});        *zb++=1&&s; break;
   case IALLEPS: s=1; BSLOOPAW(if(-2< q){s=0; break;});        *zb++=1&&s; break;
   case ISUMEPS: s=0; BSLOOPAW(if(-2==q)++s);                  *zi++=s;    break;
   case IIFBEPS:      BSLOOPAW(if(-2==q)*zi++=i);              ZISHAPE;    break;
 }}
 R z;
}    /* a i.!.0 w on boxed a,w by grading and binary search */

static I jtutype(J jt,A w,I c){A*wv,x;I m,t,wd;
 if(!AN(w))R 1;
 m=AN(w)/c; wv=AAV(w); wd=(I)w*ARELATIVE(w);
 DO(c, t=0; DO(m, x=WVR(i); if(AN(x)){if(t)RZ(TYPESEQ(t,AT(x))) else{t=AT(x); if(t&FL+CMPX+BOX)R 0;}}););
 R t;
}    /* return type if opened atoms of cells of w has uniform type (but not one that may contain -0), else 0. c is # of cells */

I hsize(I m){I q=m+m,*v=ptab+PTO; DO(nptab-PTO, if(q<=*v)break; ++v;); R*v;}
// Routine to find range of an array of I
// The return is a CR struct holding max and range+1.  But if the range+1 is >= maxrange,
// we abort and return 0 range.
// min and max are initial values for min/max
#if 1
// We keep this version, using CMOV, because on Ivy Bridge CMOVs execute at one per clock (latency 2) and thus cannot update a
// single min/max every 2 cycles - and there is no other way to update faster than one per clock.  On Broadwell, CMOVs execute 2
// per clock with latency 1 and thus just can update both min and max each clock with 2 copies.  This is faster than taken branches
// which retire only 0.5 branches per clock
#define ASSESSBLOCKSIZE 64  // every so many inputs, check for range too large
CR condrange(I *s,I n,I min,I max,I maxrange){CR ret;I i,min0,min1,max0,max1;I x;
 // Unroll loop once to keep the compares rolling
 if(!n)goto fail;
 min0=min1=min; max0=max1=max;  // initial values
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
 ret.min=min0; ret.range=max0-min0+1;  // because the tests succeed, this will give the proper range
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

// Types for the prehashed result
#define PREHRESIV 0
#define PREHRESVAR 1
#define PREHRESBV 2
#define PREHRESBAN 3
#define PREHRESIA 4
#define PREHRESIAN 5
#define PREHRESIVN 6

#define MAXBYTEBOOL 65536  // if p exceeds this, we switch over to packed bits

A jtindexofsub(J jt,I mode,A a,A w){PROLOG(0079);A h=0,hi=mtv,z=mtv;B mk=w==mark,th;
    I ac,acr,af,ak,an,ar,*as,at,datamin,f,f1,k,k1,n,r,*s,t,wc,wcr,wf,wk,wn,wr,*ws,wt,zn;UI c,m,p;
 RZ(a&&w);
 // ?r=rank of argument, ?cr=rank the verb is applied at, ?f=length of frame, ?s->shape, ?t=type, ?n=#atoms
 // mk is set if w argument is omitted (we are just prehashing the a arg)
 ar=AR(a); acr=jt->rank?jt->rank[0]:ar; af=ar-acr;
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr; jt->rank=0;  // note: mark is an atom
 as=AS(a); at=AT(a); an=AN(a);
 ws=AS(w); wt=AT(w); wn=AN(w);
 if(mk){f=af; s=as; r=acr-1; f1=wcr-r;}  // if w is omitted (for prehashing), use info from a
 else{  // w is given.  See if we need to abort owing to shapes.
  f=af?af:wf; s=af?as:ws; r=acr?acr-1:0; f1=wcr-r;
  if(0>f1||ICMP(as+af+1,ws+wf+f1,r)){I f0,*v;
   // Dyad where shape of an item of a does not match shape of a cell of w.  Return appropriate not-found
   if((wf-af)>0&&af){f1+=wf-af; wf=af;}  // see below for discussion about long frame in w
   I witems = wr>r?ws[0]:1;  // # items of w, in case we are doing i.&0 eg on result of e., which will have that many items
   m=acr?as[af]:1; f0=MAX(0,f1); RE(zn=mult(prod(f,s),prod(f0,ws+wf)));
   switch(mode){
    case IIDOT:  
    case IICO:    GATV(z,INT,zn,f+f0,s); if(af)ICPY(f+AS(z),ws+wf,f0); v=AV(z); DO(zn, *v++=m;); R z;
    case IEPS:    GATV(z,B01,zn,f+f0,s); if(af)ICPY(f+AS(z),ws+wf,f0); memset(BAV(z),C0,zn); R z;
    case ILESS:                              R ca(w);
    case IIFBEPS:                            R mtv;
    case IANYEPS: case IALLEPS: case II0EPS: R zero;
    case ISUMEPS:                            R sc(0L);
    case II1EPS:  case IJ1EPS:               R sc(witems);
    case IJ0EPS:                             R sc(witems-1);
    case INUBSV:  case INUB:    case INUBI:  ASSERTSYS(0,"indexofsub"); // impossible 
 }}}
 // f is len of frame of a (or of w if a has no frame); s->shape of a (or of w is a has no frame)
 // r is rank of an item of a cell of a (i. e. rank of a target item), f1 is len of frame of A CELL OF w with respect to target cells, in
 // other words the frame of the results each cell of w will produce
 if(at&SPARSE||wt&SPARSE){A z;
  // Handle sparse arguments
  if(1>=acr)R af?sprank2(a,w,0L,acr,RMAX,jtindexof):wt&SPARSE?iovxs(mode,a,w):iovsd(mode,a,w);
  if(af||wf)R sprank2(a,w,0L,acr,wcr,jtindexof);
  switch((at&SPARSE?2:0)+(wt&SPARSE?1:0)){
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
 // k=target item # bytes, hp->hash table or to 0   z=result   p=size of hashtable
 m=acr?as[af]:1; RE(t=mk?at:maxtype(at,wt)); k1=bp(t);   // m=length of target axis; the common type; k1=#bytes/atom of common type
 // Now that we have audited the shape of the cells of a/w to make sure they have commensurate items, we need to revise
 // the frame of w if it has the longer frame.  This can happen only where IRS is supported, namely ~: i. i: e. .
 // For those verbs, we get the effect of repeating a cell of a by having a macrocell of w, which is then broken into target-cell sizes.
 // We do this only if af!=0, because we have already set up to repeat cells of a if af=0
 if((wf-af)>0&&af){f1+=wf-af; wf=af;}
 if(an&&wn){
  // Neither arg is empty.  We can safely count the number of cells
  PROD(n,acr-1,as+af+1); k=n*k1; // n=number of atoms in a target item; k=number of bytes in a target item
  PROD(ac,af,as); PROD(wc,wf,ws); PROD(c,f1,ws+wf);  // ?c=#cells in a & w;  c=#target items (and therefore #result values) in a result-cell
  RE(zn=mult(af?ac:wc,c));   // #results is results/cell * number of cells; number of cells comes from ac if a has frame, otherwise w.  If both have frame, a's must be longer, use it
  ak=(acr?as[af]*k:k)&((1-ac)>>(BW-1)); wk=(c*k)&((1-wc)>>(BW-1));   // # bytes in a cell, but 0 if there are 0 or 1 cells
  if(!af)c=zn;   // if af=0, wc may be >1 if there is w-frame.  In that case, #result/a-cell must include the # w-cells.  This has been included in zn
 }else{
  // An argument is empty.  We must beware of overflow in counting cells.  Just do it the old slow way
  n=acr?prod(acr-1,as+af+1):1; RE(zn=mult(prod(f,s),prod(f1,ws+wf)));
  k=n*k1;
  ac=prod(af,as); ak=ac?k1*an/ac:0;  // ac = #cells of a
  wc=prod(wf,ws); wk=wc?k1*wn/wc:0; c=1<ac?wk/k:zn; wk*=1<wc;
 }

 // Convert dissimilar types
 th=HOMO(at,wt); jt->min=0;  // are args compatible? clear return values from irange
 // Indicate if float args need to be canonicalized for -0.  should do this in the hash
 I cvtsneeded = 0;  // 1 means convert a, 2 means convert w
 if(th&&TYPESNE(t,at))RZ(a=t&XNUM?xcvt(XMEXMT,a):cvt(t,a)) else if(t&FL+CMPX      )cvtsneeded=1;
 if(th&&TYPESNE(t,wt))RZ(w=t&XNUM?xcvt(XMEXMT,w):cvt(t,w)) else if(t&FL+CMPX&&a!=w)cvtsneeded|=2;

 // Allocate the result area
 if(!mk)switch(mode&IIOPMSK){I q;
  case IIDOT: 
  case IICO:    GATV(z,INT,zn,f+f1,     s); if(af)ICPY(f+AS(z),ws+wf,f1); break;
  case INUBSV:  GATV(z,B01,zn,f+f1+!acr,s); if(af)ICPY(f+AS(z),ws+wf,f1); if(!acr)*(AS(z)+AR(z)-1)=1; break;
  case INUB:    q=m+1; GA(z,t,mult(q,aii(a)),MAX(1,wr),ws); *AS(z)=q; break;  // +1 because we speculatively overwrite.  Was MIN(m,p) but we don't have the range yet
  case ILESS:   GA(z,t,AN(w),MAX(1,wr),ws); break;
  case IEPS:    GATV(z,B01,zn,f+f1,     s); if(af)ICPY(f+AS(z),ws+wf,f1); break;
  case INUBI:   q=m+1; GATV(z,INT,q,1,0); break;  // +1 because we speculatively overwrite  Was MIN(m,p) but we don't have the range yet
  // (e. i. 0:) and friends don't do anything useful if e. produces rank > 1.  The search for 0/1 always fails
  case II0EPS: case II1EPS: case IJ0EPS: case IJ1EPS:
                if(wr>MAX(ar,1))R sc(wr>r?ws[0]:1); GAT(z,INT,1,0,0); break;
  // ([: I. e.) ([: +/ e.) ([: +./ e.) ([: *./ e.) work only if e. produces rank 0 or 1.  Nonce error otherwise
  case IIFBEPS: ASSERT(wr<=MAX(ar,1),EVNONCE); GATV(z,INT,c+1,1,0); break;  // +1 because we speculatively overwrite
  case IANYEPS: case IALLEPS:
                ASSERT(wr<=MAX(ar,1),EVNONCE); GAT(z,B01,1,0,0); break;
  case ISUMEPS:
                ASSERT(wr<=MAX(ar,1),EVNONCE); GAT(z,INT,1,0,0); break;
 }

 // Handle empty/inhomogeneous arguments
 if(!(mk||m&&n&&zn&&(th>0))){
  I witems = wr>r?ws[0]:1;  // # items of w, in case we are doing i.&0 eg on result of e., which will have that many items
  switch(mode&IIOPMSK){
  // If empty argument or result, or inhomogeneous arguments, return an appropriate empty or not-found
  // We also handle the case of i.&0@:e. when the rank of w is more than 1 greater than the rank of a cell of a;
  // in that case the search always fails
  case IIDOT:   R reshape(shape(z),sc(n?m:0  ));
  case IICO:    R reshape(shape(z),sc(n?m:m-1));
  case INUBSV:  R reshape(shape(z),take(sc(m),one));
  case INUB:    AN(z)=0; *AS(z)=m?1:0; R z;
  case ILESS:   if(m)AN(z)=*AS(z)=0; else MC(AV(z),AV(w),k1*AN(w)); R z;
  case IEPS:    R reshape(shape(z),m&&(!n||th)?one:zero);
  case INUBI:   R m?iv0:mtv;
  // th<0 means that the result of e. would have rank>1 and would never compare against either 0 or 1
  case II0EPS:  R sc(n&&zn?0L        :witems         );
  case II1EPS:  R sc(n&&zn?witems         :0L        );
  case IJ0EPS:  R sc(n&&zn?MAX(0,witems-1):witems         );
  case IJ1EPS:  R sc(n&&zn?witems         :MAX(0,witems-1));
  case ISUMEPS: R sc(n?0L        :c         );  // must include shape of w
  case IANYEPS: R    n?zero:one;
  case IALLEPS: R c&&n?zero:one;
  case IIFBEPS: R n?mtv :IX(c);
 }}

 // NOTE: from here on we may add modifiers to mode, indicating FULL/BITS/PACK etc.  These flags are needed in the action routine, and must be
 // preserved if the resulting hashtable is saved as part of a prehash.  They are not valid on input to this routine.

 // Choose the function to use for performing the operation
 // See if we should simply do sequential search.    We do this only when the cell of a is a list.
 // The cost of such a search is (4 inst per loop) and the expected number of loops is half of
 // m*number of results.  The cost of small-range hashing is at best 8 cycles per atom added to the table and 5 cycles per lookup.
 // (full hashing is considerably more expensive)
 if(1==acr&&(1==wc||ac==wc)&&a!=w&&!mk&&((D)m*(D)zn<(4*m)+2.5*(D)zn)&&(mode==IIDOT||mode==IICO||mode==IEPS)){
  jtiosc(jt,mode,m,c,ac,wc,a,w,z); // simple sequential search without hashing.
 }else{B b=0==jt->ct;I t1;
  AF fn=0; // we haven't figured it out yet
  UI booladj = (mode&(IIOPMSK&~(IIDOT^IICO)))?5:0; // init table length not found; booladj = 5 if boolean hashvalue is OK, 0 if full index needed
  p = (UI)MIN(IMAX-5,(2.1*MAX(m,c)));  // length we will use for hashtable, if small-range not used.
  if(!b&&t&BOX+FL+CMPX)ctmask(jt);
  if     (t&BOX)          fn=b&&(1<n||usebs(a,ac,m))?jtiobs:1<n?jtioa:b?jtioax1:
                              (t1=utype(a,ac))&&(mk||a==w||TYPESEQ(t1,utype(w,wc)))?jtioau:jtioa1;
  else if(t&XNUM)         fn=jtiox;
  else if(t&RAT )         fn=jtioq;
  else if(1==k)           {p=t&B01?2:256;datamin=0; mode|=IIMODFULL; fn=jtio12;}   // 1-byte ops, just use small-range code: checking takes too much time
  else{
   // We might switch over to small-range mode, if the sizes are right.  See how big the hash table would be for full hashing
   // figure out whether we should use small-range matching or hashing.  We use small-range code if:
   // type is exact and length is 1 or 2 bytes; or
   // type is exact numeric, length is 4 bytes or 8 bytes, and the (range of the data)*(length of rangecell) is less than 2.1*(length of data)*(length of hashcell)
   //  where length of rangecell=4 for i. or i:, 1/8 otherwise, length of hashcell=4
   // result is p (the length of hashtable, as # of entries), datamin (the minimum value found, if small-range)
   // If the allocated range includes all the possible values for the input, set IIMODFULL to indicate that fact
   if(2==k){
    // if the actual range of the data exceeds p, we revert to hashing.  All 2-byte types are exact
    CR crres = condrange2(USAV(a),(AN(a)*k1)/sizeof(US),-1,0,MIN((UI)(IMAX-5)>>booladj,p)<<booladj);   // get the range
    if(crres.range){
      datamin=crres.min;
      // If the range is close to the max, we should consider widening the range to use the faster FULL code.  We do this only for boolean hashes, because
      // in the current allocation going all the way to 65536 kicks us into the longer hashtable (questionable decision).  Otherwise we should just promote
      // any non-Boolean, because the actual cache footprint won't change.
      // The cost of promoting a Boolean is 1 store (1 clock) per word cleared, for (65536-range)>>booladj bytes (if booladj!=0) [or (65536-range) hashtable entries if booladj==0]
      // The savings is 4 ops (2 clocks) per word searched
      if(booladj && ((UI)(65536-crres.range)>>booladj) < (c<<(LGSZI+1))){p=65536; datamin=0;}else{p=crres.range;}  // this underestimates the benefit for prehashes
      if(p==65536)mode|=IIMODFULL;
      fn=jtio22;  // This qualifies for small-range processing
    }else{booladj=0;}   // Turn off booladj if small-range processing not engaged
   }
   if(!fn){  // if we don't have it yet, it will be a hash.  Decide which one
    if(cvtsneeded&1)RZ(a=cvt0(a));  // Convert negative 0 to positive 0. Should do this in the hash
    if(cvtsneeded&2)RZ(w=cvt0(w));
    if(k==SZI&&!(t&FL)){  // non-float, might be INT or SBT
     if(t&INT+SBT){  // same here, for I types
      CR crres = condrange(AV(a),(AN(a)*k1)/SZI,IMAX,IMIN,MIN((UI)(IMAX-5)>>booladj,p)<<booladj);
      if(crres.range){
       datamin=crres.min;
       p=crres.range; fn=jtio42;
      }else{booladj=0; fn=jtioi;}  // leave p as is; clear booladj since not small-range; select integer hashing
     }else{booladj=0; fn=jtioi;}
    }else{                    fn=b||t&B01+JCHAR+INT+SBT?jtioc:1==n?(t&FL?jtiod1:jtioz1):t&FL?jtiod:jtioz;}  // select other hashing
   }
  }



// obsolete // should have an irange that takes the max value allowed, & returns early if range is exceeded
// obsolete  if(AT(a)&INT+SBT&&k==SZI){I r; irange(AN(a)*k1/SZI,AV(a),&r,&ss); if(ss){jt->min=r;}}  //  r=min value,ss=max value+1-min value or 0 if no values or range too big
// obsolete  // compute size of hashtable
// obsolete  p=1==k?(t&B01?2:256):2==k?(t&B01?258:65536):k==SZI&&ss&&ss<2.1*MAX(m,c)?ss:hsize(m);

  // if a hashtable will be needed, allocate it.  It is NOT initialized
  // the hashtable is INT unless we have selected small-range hashing AND we are not looking for the index with i. or i:; then boolean is enough
  if(fn==jtio12||fn==jtio22||fn==jtio42){IH *hh;
   // make sure we have a hashtable of the requisite size.  p has the number of entries, booladj indicates whether they are 1 bit each.
   // if the #entries fits in a US, use the short table.  But bits always use the long table

   // See if the size/range of w allows use of one of the faster loops.  The options are FULL (which saves the 4 instructions per atom of w that would
   // be spent range-checking w) and BASE0 (which clears the hashtable, at a cost of 1 cycle per 2/4 entries, or 4x that if we use fast instructions)
   // First check FULL, which is always the right decision if possible - except for self-classify which assumes FULL, or prehash which doesn't go through w at all
   if(a!=w&&!mk&&!(mode&IIMODFULL)){CR crres;
    I allowrange;  // where we will build the max allowed range of w
    if(h=jt->idothash1){allowrange=IHAV(h)->datasize>>IHAV(h)->hashelelgsize;}else{allowrange=0;}  // current max capacity of large hash
    // always allow a little bit larger than the range of a, to make sure we expand the hashtable if a little more would be enough.
    // but never increase the range if that would exceed the L2 cache - just pay the 4 instructions
    if(k==2){
     allowrange=MIN(MAX(L2CACHESIZE/(LGSZUS),(I)p),MAX(allowrange,(I)(p+(p>>3))));  // allowed range, with expansion
     crres = condrange2(USAV(w),AN(w),datamin,datamin+p-1,allowrange);
    }else{
     allowrange=MIN(MAX(L2CACHESIZE/(LGSZUI4),(I)p),MAX(allowrange,(I)(p+(p>>3))));  // allowed range, with expansion
     crres = condrange(AV(w),AN(w),datamin,datamin+p-1,allowrange);
    }
    if(crres.range){datamin=crres.min; p=crres.range; mode |= IIMODFULL;}
   }  
   if(p<(65536-((AH*SZI+sizeof(IH)+sizeof(MS))/sizeof(US))) && booladj==0 && m<65536){
    // using the short table.  Allocate it if it hasn't been allocated yet, or if this is prehashing, where we will build a separate table.
    // It would be nice to use the main table for m&i. to avoid having to clear a custom table, since m&i. may never get assigned to a name;
    // but if it IS assigned, the main table may be too big, and we don't have any good way to trim it down
    // If the sizes are such that we should clear this table to save 3 clocks per atom of w, say so.  The clearing is done in hashallo
    // Clearing also saves 1 clock per input word
    mode |= ((c*3+m)<(p>>(LGSZI-LGSZUS-1)))<<IIMODFORCE0X;  // 3 cycles per atom of w, 1 cycle per atom of m, versus 2/4 cycle per atom to clear (without wide insts)
    if(mk||!(h=jt->idothash0)){
     GATV(h,INT,((65536*sizeof(US)-((AH*SZI+sizeof(MS))))/SZI),0,0);  // size too big for GAT
     // Fill in the header
     hh=IHAV(h);  // point to the header
     hh->datasize=AM(h)-sizeof(IH);  // number of bytes in data area
     hh->hashelelgsize=1;  // hash entries are 2 bytes long
     if(mk){
      // The table is being used for prehashing.  Clear the data area (only the part we will use), and also the values used as return values from hashallo, to wit
      // the allocated position and index
      mode |= IIMODBASE0|IIMODFORCE0;  // we are surely initializing this table now, & it stays that way on every use
      // It's OK to round the fill up to the length of an I
      UI fillval=m|(m<<16); if(SZI>4)fillval|=fillval<<(32%BW); I fillct=(p+(((1LL<<(LGSZI-LGSZUS))-1)))>>(LGSZI-LGSZUS);
      DO(fillct, hh->data.UI[i]=fillval;)
// obsolete      memset(hh->data.UC,C0,hh->datasize);  // clear the data
      hh->currentlo=0; hh->currentindexofst=0;  // clear the parms.  Leave index 0 for not found
     }else{
      // not prehashing.  Fill in the remaining fields, remember this block for later use, and make its allocation permanent
      jt->idothash0=h;
      hh->invalidlo=IMAX; hh->invalidhi=0;  // none of this is ever used for bits
      hh->currentindexend=hh->previousindexend=(US)-1;  // signal table must be initialized
      // since table is to be initialized, currentlo/currenthi can be left garbage
      ra(h);  // make the table permanent
     }
    }
   }else{
    // using the long table.  Use the current one if it is long enough; otherwise allocate a new one
    // First, make a decision for Boolean tables.  If the table will be Boolean, decide whether to use packed bits
    // or bytes, and represent that information in mode and booladj.
    if(booladj){if(p>MAXBYTEBOOL){mode|=IIMODPACK|IIMODBITS;}else{mode|=IIMODBITS;booladj=5-3;}  // set MODBITS as a flag to hashallo
    }else{
     // If the sizes are such that we should clear this table to save 3 clocks per atom of w, say so.  The clearing is done in hashallo.  Only for non-bits.
     mode |= ((c*3+m)<(p<<(1-(LGSZI-LGSZUI4))))<<IIMODFORCE0X;  // 3 cycles per atom of w, 1 cycle per atom of m, versus 2/2 cycle per atom to clear (without wide insts)
    }
    I psizeinbytes = ((p>>booladj)+4)*sizeof(UI4);   // Get length of table in bytes.  We add 4 to the request:
         // for small-range to round up to an even word of an I, and possibly padding leading/trailing bytes; for hashing, we need a sentinel at the beginning and the end
    if(mk||!((h=jt->idothash1) && IHAV(h)->datasize >= psizeinbytes)){
     // if we have to reallocate, free the old one
     if(!mk&&h){fr(h); jt->idothash1=0;}  // free old, and clear pointer in case of allo error
     // allocate the new one and fill it in
     GATV(h,INT,(psizeinbytes+sizeof(IH)+(SZI-1))>>LGSZI,0,0);
     // Fill in the header
     hh=IHAV(h);  // point to the header
     hh->datasize=AM(h)-sizeof(IH);  // number of bytes in data area
     hh->hashelelgsize=2;  // hash entries are 4 bytes long
     if(mk){
      // The table is being used for prehashing.  Clear the data area (only the part we will use), and also the values used as return values from hashallo, to wit
      // the allocated position and index
      // It's OK to round the fill up to the length of an I
      UI fillval;
      if(booladj){  // convert bit count to words, rounded up
       fillval=(mode&(IIMODPACK+IIOPMSK))<=INUBI; fillval|=fillval<<8; fillval|=fillval<<16;  // fill packed bits with 0, byte-bits with 0 except for ~. ~: I.@~. -.
      }else{  // convert UI4 count to words, rounded up
       mode |= IIMODBASE0|IIMODFORCE0;  // we are surely initializing this table now, & it stays that way on every use.  Only for non-Boolean
       fillval=m; 
      }  // fill bits with 0; fill full hashes with m
      if(SZI>4)fillval|=fillval<<(32%BW);  // fill entire words
      UI fillct=(p+((((1LL<<(LGSZI-LGSZUI4))<<booladj)-1)))>>(booladj+LGSZI-LGSZUI4);  // Round bits/UI4 up to SZI, then convert to count of Is
      DO(fillct, hh->data.UI[i]=fillval;)
// obsolete       memset(hh->data.UC,C0,hh->datasize);  // clear the data
      hh->currentlo=0; hh->currentindexofst=0;  // clear the parms.  This will never go through hashallo, so right-side and upper info not needed
     }else{
      // not prehashing.  Fill in the remaining fields, remember this block for later use, and make its allocation permanent
      jt->idothash1=h;
      hh->invalidlo=IMAX; hh->invalidhi=0;  // none of this is invalid because it held bitmasks
      hh->currentindexend=hh->previousindexend=(UI4)-1;  // signal that table must be initialized
      // since table is to be initialized, currentlo/currenthi can be left garbage
      ra(h);  // make the table permanent
     }
    }
    // switch the routine pointer to the big table
    if(fn==jtio12)fn=jtio14; else if(fn==jtio22)fn=jtio24; else fn=jtio44;
   }
   // Pass the min/range into the action routine, using result values in the hashtable
   hh=IHAV(h); hh->datamin=datamin; hh->datarange=p;  // max will be inferred
  }else{if(fn!=jtiobs)GATV(h,INT,p,1,0);}  // hash allocation old-style, now always INT

  if(fn==jtioc){A x;B*b;C*u,*v;I*d,q;
   // exact types (including intolerant comparison of FL/CMPX)
   // Allocate bitmask (as a B01) for each byte in an item of rimatand, init to true.  This will indicate which bytes need to be indexed
   // should get rid of this - cheaper to hash than to check for need-to-hash
   GATV(x,B01,k,1,0); b=BAV(x); memset(b,C1,k);
   q=k; u=CAV(a); v=u+k;  // q = #bytes that have all identical values.  v point to current item, starting at second
   DO(ac*(m-1), DO(k, if(u[i]!=*v&&b[i]){b[i]=0; --q;} ++v;); if(!q)break;);  // Check for differing byte.  Exit loop if all different.   should reverse b[i] test   error - should be ac*m-1
   // Convert the mask of varying bytes into the list of indexes of varying bytes, and set a pointer to that list for use in the indexing routine
   if(q){jt->hin=k-q; GATV(hi,INT,k-q,1,0); jt->hiv=d=AV(hi); DO(k, if(!b[i])*d++=i;); fn=jtiocx;}
  }

  // Call the routine to perform the operation
  RZ(fn(jt,mode,m,n,c,k,acr,wcr,ac,wc,ak,wk,a,w,&h,z));
  if(mk){A x,*zv;I*xv,ztype;
   // If w was omitted (indicating prehashing), return the information for that special case
   // result is an array of 3 boxes, containing (info vector),(hashtable),(mask of hashed bytes if applicable)
   // The caller must ra() this result to protect it, if it is going to be saved
   GAT(z,BOX,3,1,0); zv=AAV(z);
   GAT(x,INT,6,1,0); xv=AV(x);
// should use a lookup
   switch(mode&IIOPMSK){
    default:                    ztype=PREHRESIV; break;  /* integer vector      */
 // obsolete  case ILESS:                 ztype=PREHRESVAR; break;  /* type/shape from arg */
    case IEPS:                  ztype=PREHRESBV; break;  /* boolean vector      */
    case IANYEPS: case IALLEPS: ztype=PREHRESBAN; break;  /* boolean scalar      */
    case ISUMEPS:               ztype=PREHRESIAN; break; 
    case II0EPS:  case II1EPS:  
    case IJ0EPS:  case IJ1EPS:  ztype=PREHRESIA;  break;         /* integer scalar      */
    case IIFBEPS:               ztype=PREHRESIVN; break; // integer vector with length check
   }
   xv[0]=mode; xv[1]=n; xv[2]=k; xv[3]=jt->min; xv[4]=(I)fn; xv[5]=ztype; 
   zv[0]=x; zv[1]=h; zv[2]=hi;
  }
 }  // end of 'not sequential comparison' which means we need a hashtable
 EPILOG(z);
}    /* a i."r w main control */

// verb to handle compounds like m&i. e.&n .  m/n has already been hashed and the result saved away
A jtindexofprehashed(J jt,A a,A w,A hs){A h,hi,*hv,x,z;AF fn;I ar,*as,at,c,f1,k,m,mode,n,
     r,t,*xv,wr,*ws,wt,ztype;
 RZ(a&&w&&hs);
 // hv is (info vector);(hashtable);(byte index valididty)
 hv=AAV(hs); x=hv[0]; h=hv[1]; hi=hv[2];  
 // get the info from the info vector
 xv=AV(x); mode=xv[0]; n=xv[1]; k=xv[2]; jt->min=xv[3]; fn=(AF)xv[4]; ztype=xv[5]; 
 ar=AR(a); as=AS(a); at=AT(a); t=at; m=ar?*as:1; 
 wr=AR(w); ws=AS(w); wt=AT(w);
// obsolete if(1==ztype)r=wr?wr-1:0;
// obsolete  else        
 r=ar?ar-1:0;
 f1=wr-r;
 RE(c=prod(f1,ws));  // c=#cells of w (and result)
 // audit conformance of input shapes.  If there is an error, pass to the main code to get the error result
 // Use c as an error flag
 if(!(r<=ar&&0<=f1))c=0;   // w must have rank big enough to hold a cell of a
 if(ICMP(as+ar-r,ws+f1,r))c=0;  // and its shape at that rank must match the shape of a cell of a
// obsolete  if(mode==ILESS&&(TYPESNE(t,wt)||AFLAG(w)&AFNJA+AFREL||n!=aii(w)))R less(w,a);
 // If there is any error, switch back to the non-prehashed code.  We must remove any command bits from mode, leaving just the operation type
 if(!(m&&n&&c&&HOMO(t,wt)&&UNSAFE(t)>=UNSAFE(wt)))R indexofsub(mode&IIOPMSK,a,w);

 // allocate enough space for the result, depending on the type of the operation
 switch(ztype){
  // the N endings are types that do not produce correct results if the result of e. has rank >1.  We give nonce error if that happens
  case PREHRESIV: GATV(z,INT,c,    f1, ws); break;
  case PREHRESIVN: ASSERT(wr<=MAX(ar,1),EVNONCE); GATV(z,INT,c,    f1, ws); break;
  case PREHRESVAR: GA(z,wt, AN(w),1+r,ws); break;
  case PREHRESBV: GATV(z,B01,c,    f1, ws); break;
  case PREHRESBAN: ASSERT(wr<=MAX(ar,1),EVNONCE); GAT(z,B01,1,    0,  0 ); break;
  case PREHRESIA: if(wr>MAX(ar,1))R sc(wr>r?ws[0]:1); GAT(z,INT,1,    0,  0 ); break;
  case PREHRESIAN: ASSERT(wr<=MAX(ar,1),EVNONCE); GAT(z,INT,1,    0,  0 ); break;
 }
 // save info used by the routines
 jt->hin=AN(hi); jt->hiv=AV(hi);
 // convert type of w if needed; and if unconverted FL/CMPX, touch to change -0 to 0
 // this is dodgy: if the comparison tolerance doen't change, there should be no need for conversion; but
 // if ct does change, the stored hashtable is invalid.  We should store the ct as part of the hashtable
 if(TYPESNE(t,wt))RZ(w=cvt(t,w)) else if(t&FL+CMPX)RZ(w=cvt0(w));
 // call the action routine
 R fn(jt,mode+IPHOFFSET,m,n,c,k,AR(a),AR(w),(I)1,(I)1,(I)0,(I)0,a,w,&h,z);
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
 RZ(w);
 if(SPARSE&AT(w))R nubsievesp(w); 
 if(jt->rank)jt->rank[0]=jt->rank[1]; 
 R indexofsub(INUBSV,w,w); 
}    /* ~:"r w */

// ~. y  - does not have IRS
F1(jtnub){ 
 RZ(w);
 if(SPARSE&AT(w)||AFLAG(w)&AFNJA+AFREL)R repeat(nubsieve(w),w); 
 R indexofsub(INUB,w,w);
}    /* ~.w */

// x -. y.  does not have IRS
F2(jtless){A x=w;I ar,at,k,r,*s,wr,*ws,wt;
 RZ(a&&w);
 at=AT(a); ar=AR(a); 
 wt=AT(w); wr=AR(w); r=MAX(1,ar);
 if(ar>1+wr)R ca(a);  // if w's rank is smaller than that of a cell of a, nothing can be removed, return a
 // if w's rank is larger than that of a cell of a, reheader w to look like a list of such cells
 if(wr&&r!=wr){RZ(x=gah(r,w)); s=AS(x); ws=AS(w); k=ar>wr?0:1+wr-r; *s=prod(k,ws); ICPY(1+s,k+ws,r-1);}  // bug: should test for error on the prod()
 // if nothing special (like sparse, or incompatible types, or x requires conversion) do the fast way; otherwise (-. x e. y) # y
 R !(at&SPARSE)&&HOMO(at,wt)&&TYPESEQ(at,maxtype(at,wt))&&!(AFLAG(a)&AFNJA+AFREL)?indexofsub(ILESS,x,a):
     repeat(not(eps(a,x)),a);
}    /* a-.w */

// x e. y
F2(jteps){I l,r,rv[2];
 RZ(a&&w);
 rv[0]=r=jt->rank?jt->rank[1]:AR(w);
 rv[1]=l=jt->rank?jt->rank[0]:AR(a); jt->rank=0;
 if(SPARSE&AT(a)+AT(w))R lt(irs2(w,a,0L,r,l,jtindexof),sc(r?*(AS(w)+AR(w)-r):1));
 jt->rank=rv; 
 R indexofsub(IEPS,w,a);
}    /* a e."r w */

// I.@~: y   does not have IRS
F1(jtnubind){
 RZ(w);
 R SPARSE&AT(w)?icap(nubsieve(w)):indexofsub(INUBI,w,w);
}    /* I.@~: w */

// i.@(~:"0) y     does not have IRS
F1(jtnubind0){A z;D oldct=jt->ct;
 RZ(w);
 jt->ct=0.0; z=SPARSE&AT(w)?icap(nubsieve(w)):indexofsub(INUBI,w,w); jt->ct=oldct;
 R z;
}    /* I.@(~:!.0) w */

// = y    
F1(jtsclass){A e,x,xy,y,z;I c,j,m,n,*v;P*p;
 RZ(w);
 // If w is scalar, return 1 1$1
 if(!AR(w))R reshape(v2(1L,1L),one);
 n=IC(w);   // n=#items of y
 RZ(x=indexof(w,w));   // x = i.~ y
 // if w is dense, return ((x = i.n) # x) =/ x
 if(DENSE&AT(w))R atab(CEQ,repeat(eq(IX(n),x),x),x);
 // if x is sparse... ??
 p=PAV(x); e=SPA(p,e); y=SPA(p,i); RZ(xy=stitch(SPA(p,x),y));
 if(n>*AV(e))RZ(xy=over(xy,stitch(e,less(IX(n),y))));
 RZ(xy=grade2(xy,xy)); v=AV(xy);
 c=*AS(xy);
 m=j=-1; DO(c, if(j!=*v){j=*v; ++m;} *v=m; v+=2;);
 GAT(z,SB01,1,2,0);  v=AS(z); v[0]=1+m; v[1]=n;
 p=PAV(z); 
 SPB(p,a,v2(0L,1L));
 SPB(p,e,zero);
 SPB(p,i,xy);
 SPB(p,x,reshape(sc(c),one));
 R z;
}


// support for a i."1 &.|:w or a i:"1 &.|:w

// function definition
#define IOCOLF(f)     void f(J jt,I m,I c,I d,A a,A w,A z,A h)
#define IOCOLDECL(T)  D tl=1-jt->ct,tr=1/tl,x;                           \
                          I hj,*hv=AV(h),i,j,jr,l,p=AN(h),*u,*zv=AV(z);  \
                          T*av=(T*)AV(a),*v,*wv=(T*)AV(w);UI pm=p

// function to search forward
// T is the type of the data
// f is function name
// hasha is the hash to use for the values from a
// hashl and hashr are the hashes to use for one tolerance below & above
// exp is a test for not-equal, returning 1 when values do not match
// For each column, clear the hash table, then hash the items of a, then look up the items of w 
#define IOCOLFT(T,f,hasha,hashl,hashr,exp)  \
 IOCOLF(f){IOCOLDECL(T);                                                   \
  for(l=0;l<c;++l){                                                        \
   DO(p, hv[i]=m;);                                                        \
   v=av;         DO(m, j=(hasha)%pm; FIND(exp); if(m==hj)hv[j]=i; v+=c;);  \
   v=wv; u=zv;                                                             \
   for(i=0;i<d;++i){                                                       \
    x=*(D*)v;                                                              \
    j=jr=(hashl)%pm;           FIND(exp); *u=hj;                           \
    j=   (hashr)%pm; if(j!=jr){FIND(exp); *u=MIN(*u,hj);}                  \
    v+=c; u+=c;                                                            \
   }                                                                       \
   ++av; ++wv; ++zv;                                                       \
 }}

// Similar, but search backward
#define JOCOLFT(T,f,hasha,hashl,hashr,exp)  \
 IOCOLF(f){IOCOLDECL(T);I q;                                               \
  for(l=0;l<c;++l){                                                        \
   DO(p, hv[i]=m;);                                                        \
   v=av+c*(m-1); DQ(m, j=(hasha)%pm; FIND(exp); if(m==hj)hv[j]=i; v-=c;);  \
   v=wv; u=zv;                                                             \
   for(i=0;i<d;++i){                                                       \
    x=*(D*)v;                                                              \
    j=jr=(hashl)%pm;           FIND(exp); *u=q=hj;                         \
    j=   (hashr)%pm; if(j!=jr){FIND(exp); if(m>hj&&(hj>q||q==m))*u=hj;}    \
    v+=c; u+=c;                                                            \
   }                                                                       \
   ++av; ++wv; ++zv;                                                       \
 }}

// create the index-of routines.  These hash just the real part of a complex value
static IOCOLFT(D,jtiocold,hid(*v),    hid(tl*x),hid(tr*x),!teq(*v,av[c*hj]))
static IOCOLFT(Z,jtiocolz,hid(*(D*)v),hid(tl*x),hid(tr*x),!zeq(*v,av[c*hj]))

static JOCOLFT(D,jtjocold,hid(*v),    hid(tl*x),hid(tr*x),!teq(*v,av[c*hj]))
static JOCOLFT(Z,jtjocolz,hid(*(D*)v),hid(tl*x),hid(tr*x),!zeq(*v,av[c*hj]))

// support for a i."1 &.|:w or a i:"1 &.|:w   used only by some sparse-array stuff
A jtiocol(J jt,I mode,A a,A w){A h,z;I ar,at,c,d,m,p,t,wr,*ws,wt;void(*fn)();
 RZ(a&&w);
 // require ct!=0   why??
 ASSERT(0!=jt->ct,EVNONCE);
 at=AT(a); ar=AR(a); m=*AS(a); c=aii(a);  // a: at=type ar=rank m=#items c=#atoms in an item
 wt=AT(w); wr=AR(w); ws=AS(w); 
 d=1; DO(1+wr-ar, d*=ws[i];);
 // convert to common type
 RE(t=maxtype(at,wt));
 if(TYPESNE(t,at))RZ(a=cvt(t,a));
 if(TYPESNE(t,wt))RZ(w=cvt(t,w));
 // allocate hash table and result
 p=hsize(m);
 GATV(h,INT,p,1,0);
 GATV(z,INT,AN(w),wr,ws);
 // call routine based on types.  Only float and CMPX are supported
 switch(CTTZ(t)){
  default:   ASSERT(0,EVNONCE);     
  case FLX:   fn=mode==IICO?jtjocold:jtiocold; ctmask(jt); break;
  case CMPXX: fn=mode==IICO?jtjocolz:jtiocolz; ctmask(jt); break;
 }
 fn(jt,m,c,d,a,w,z,h);
 R z;
}    /* a i."1 &.|:w or a i:"1 &.|:w */

#endif /* !C_AVX */
