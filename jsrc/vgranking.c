/* Copyright (c) 1990-2026, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Grade -- monad /:@/:"r  on dense arguments                       */

#include "j.h"
#include "vg.h"

#define TTYPE UI4
// obsolete #define RANKINGSUMSCAN(p)   \
// obsolete  {I s=0; TTYPE *u=yv; TTYPE *uend=u+(p); while(u!=uend){t=*u; *u=(TTYPE)s; s+=t; ++u;}}
// calculate +/\ *yv
extern AHDRP(  pluspfxI4,I4, I4 );
#define RANKINGSUMSCAN(p) pluspfxI4(1,(p),1,(I4*)yv,(I4*)yv,jt)

// create totals of occurrences in *yv (which is yv biased by min); +/\; then for each input value, copy in ordinal
// and then increment the ordinal for next time
#define RANKINGLOOP(T,exp)   \
 {T*v=(T*)wv;  DQ(n, ++yu[(exp)+1]; ++v;); RANKINGSUMSCAN(rng.range); v=(T*)wv;  DQ(n, *zv++=yu[exp]++; ++v;);}

// ranking booleans of k bytes, k=2 or 4
static A jtrankingb(J jt,A w,I wf,I wcr,I m,I n,I k){A z;C*wv;I i,j,p,t,*zv;TTYPE yv[17];
 p=(I)1<<k; wv=CAV(w);  // p=# code points in input, wv points to input
// obsolete  p=2==k?4:16; wv=CAV(w);
 GATV0(z,INT,m*n,1+wf); MCISH(AS(z),AS(w),wf+1-!wcr) if(!wcr)AS(z)[wf]=1; zv=AVn(1+wf,z);
// obsolete if(2==k){US*v;
// obsolete   for(i=0;i<m;++i){
// obsolete    mvc(p*sizeof(*yv),yv,MEMSET00LEN,MEMSET00); 
// obsolete    for(j=0,v=(US*)wv;j<n;++j){I bb=*v++; bb|=bb<<9; bb>>=8; ++yv[(bb&3)+1];} // convert 2 bits to 1 value, big-endian
// obsolete    RANKINGSUMSCAN(p);
// obsolete    for(j=0,v=(US*)wv;j<n;++j){I bb=*v++; bb|=bb<<9; bb>>=8; *zv++=yv[bb&3]++;}  // fetch the value
// obsolete    wv+=n*k;
// obsolete   }
// obsolete  }else{int*v;
// obsolete   for(i=0;i<m;++i){
// obsolete    mvc(p*sizeof(*yv),yv,MEMSET00LEN,MEMSET00); 
// obsolete    for(j=0,v=(int*)wv;j<n;++j){I bb=*v++; bb|=bb<<9; bb|=bb<<18; bb>>=24; ++yv[(bb&0xf)+1];}  // big-endian
// obsolete       RANKINGSUMSCAN(p);
// obsolete       for(j=0,v=(int*)wv;j<n;++j){I bb=*v++; bb|=bb<<9; bb|=bb<<18; bb>>=24; *zv++=yv[bb&0xf]++;}
// obsolete       wv+=n*k;
// obsolete      }
 unsigned int*v;
 for(i=0;i<m;++i){  // loop for each cell
  mvc(p*sizeof(*yv),yv,MEMSET00LEN,MEMSET00);  // clear histogram 
  for(j=0,v=(unsigned int*)wv;j<n;++j){UI4 bb=*v&(UI4)VALIDBOOLEAN; v=(unsigned int*)((I)v+k); bb|=bb<<9; bb|=bb<<18; bb>>=(28-k); ++yv[bb+1];}  // boolean histogram bigendian 
  RANKINGSUMSCAN(p);  // prefix scan  
  for(j=0,v=(int*)wv;j<n;++j){UI4 bb=*v&(UI4)VALIDBOOLEAN; v=(unsigned int*)((I)v+k); bb|=bb<<9; bb|=bb<<18; bb>>=(28-k); *zv++=yv[bb]++;}  // use histo
  wv+=n*k;
 }
 R z;
}    /* /:@/: w where w is boolean and items have length 2 or 4 */

// /:@/:
F1(jtranking){F12IP;A y,z;C*wv;I icn,i,k,m,n,t,wcr,wf,wn,wr,*ws,wt,*zv;CR rng;TTYPE *yv,*yu;
 ARGCHK1(w);
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 wt=AT(w); wv=CAV(w); wn=AN(w);
 ws=AS(w); SETICFR(w,wf,wcr,n);   // n=#cells in w; m is number of atoms in w
 if(likely(wn!=0)){PROD(m,wf,ws);}  // If there are atoms, calculate result-shape the fast way
 else{RE(m=prod(wf,ws)); R m?reitem(vec(INT,wf,ws),iota(v2(1L,n))):reshape(vec(INT,1+wf,ws),num(0));}  // if no items, return index vector
 PROD(icn,wcr-1,ws+wf+1); k=icn<<bplg(wt);  // wk=size of atom in bytes; icn=# atoms in an item of a cell  k = *bytes in an item of a CELL of w
 k&=REPSGN(SGNIFDENSE(wt));  // if sparse w, set k so as to disable all special cases
 if(ISDENSETYPE(wt,B01)&&(k==2||k==sizeof(int)))R rankingb(w,wf,wcr,m,n,k);  // if Boolean 2- or 4-byte, go off to handle that special case
 // Calculate the largest range we can abide.  The cost of a sort is about n*lg(n)*4 cycles; the cost of small-range indexing is
 // range*4.5 (.5 to clear, 2 to read) + n*6 (4 to increment, 2 to write).  So range can be as high as n*lg(n)*4/4.5 - n*6/4.5
 // approximate lg(n) with bit count.  And always use small-range if range is < 256
 UI4 lgn=CTLZI(wn); I maxrange = wn<64?256:(I)((lgn*4-6)*(D)wn/(4.5*(D)icn));  // range below this qualifies as small
 // See if the values qualify for small-range processing
 if(icn==1&&ISDENSETYPE(wt,INT+C4T)){  // items are individual INTs or C4Ts
  rng=ISDENSETYPE(wt,INT)?condrange((I*)wv,wn,IMAX,IMIN,maxrange):condrange4((C4*)wv,wn,-1,0,maxrange);
 }else if(unlikely(((k-1)&-2)==0)){
  // special case of k==1 or 2. Could be C2T/INT2 or a pair of B01/LIT/INT1.  INT2 is signed & cannot use small-range.  The pair is bigendian and cannot use condrange, but we can swap bytes
  // and we just assume the max range.
  if(wt&C2T){rng=condrange2((C2*)wv,wn,-1,0,maxrange);  // 2-byte char: get range
  }else if(wt&IS1BYTE){rng.min=0; rng.range=(1LL<<((k-(wt&B01))<<3))+((wt&B01)<<(k-1)); if(rng.range>=maxrange)rng.range=0;  // 1 or 2 B01/LIT: 2, 256, 258, or 65536, see if too big
  }else rng.range=0;  // Must be I2 or 2 I1s, can't do smallrange
// obsolete   We assume the worst on range, i. e. 8 bits/byte, EXCEPT that for B01 type we know that the value can never exceed
// obsolete   // 1 so we add 2 instead of the last 8 bits
// obsolete   rng.min=0; rng.range=(1LL<<((k-(wt&B01))<<3))+((wt&B01)<<(k-1));  // 2, 256, 258, or 65536
 }else rng.range=0;
 if(!rng.range){I *yv;
  // small-range not possible.  Do the grade and install each value into its location
  RZ(IRS1(w,0L,wcr,jtgrade1,y)); yv=AV(y); 
  GATV0(z,INT,m*n,1+wf); MCISH(AS(z),ws,wf+1-!wcr) if(!wcr)AS(z)[wf]=1; zv=AVn(1+wf,z); 
  DO(m, DO(n, zv[*yv++]=i;); zv+=n;);
  RETF(z);
 }
 // here for small-range ordinals, processed through the ranking loop
 GATV0(z,INT,m*n,1+wf); MCISH(AS(z),ws,wf+1-!wcr) if(!wcr)AS(z)[wf]=1; zv=AVn(1+wf,z);  // allocate result
 GATV0(y,C4T,rng.range+1,1); yv=C4AV1(y); yu=yv-rng.min;  // allocate workarea where we count frequencies
 for(i=0;i<m;++i){
  mvc(rng.range*sizeof(*yv),yv,MEMSET00LEN,MEMSET00);
  switch(k){
  case sizeof(I): if(wt&INT){RANKINGLOOP(I,*v); break;}
#if SY_64
  case sizeof(C4):
#endif
   RANKINGLOOP(C4,*v); break;
  case sizeof(C):   RANKINGLOOP(UC,*v); break;
// obsolete #if C_LE
  case sizeof(S):
   if(wt&IS1BYTE){RANKINGLOOP(US,(US)(*v<<8)+(US)(*v>>8))  // if 1-byte, swap to bigendian
// obsolete I c,d,s,t;US*v;TTYPE *u;
// obsolete     {US *v=(US*)wv;  DQ(n, ++yu[()+1]; ++v;); RANKINGSUMSCAN(rng.range); v=(US*)wv;  DQ(n, *zv++=yu[(*v<<8)+(*v>>8)]++;); ++v;}
// obsolete     v=(US*)wv; DQ(n, ++yu[*v++];);
// obsolete     s=0;       DO(256, c=0; d=i; DQ(256, u=yv+(c+d); c+=256; if(*u){t=*u; *u=(TTYPE)s; s+=t;}););
// obsolete     v=(US*)wv; DQ(n, *zv++=yu[*v++]++;);
   }else RANKINGLOOP(US,*v);  // keep 2-byte as is
   break;
// obsolete #else
// obsolete   case sizeof(S):   RANKINGLOOP(US);
// obsolete #endif
  }
  wv+=n*k;
 }
 RETF(z);
}
