/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Grade -- monad /:@/:"r  on dense arguments                       */

#include "j.h"
#include "vg.h"

#define TTYPE UI4
// calculate +/\ *yv
#define RANKINGSUMSCAN(p)   \
 {I s=0; TTYPE *u=yv; TTYPE *uend=u+(p); while(u!=uend){t=*u; *u=(TTYPE)s; s+=t; ++u;}}
// create totals of occurrences in *yv (which is yv biased by min); +/\; then for each input value, copy in ordinal
// and then increment the ordinal for next time 
#define RANKINGLOOP(T)   \
 {T*v=(T*)wv;  DQ(n, ++yu[*v++];); RANKINGSUMSCAN(rng.range); v=(T*)wv;  DQ(n, *zv++=yu[*v++]++;);}

static A jtrankingb(J jt,A w,I wf,I wcr,I m,I n,I k){A z;C*wv;I i,j,p,t,*zv;TTYPE yv[16];
 p=2==k?4:16; wv=CAV(w);
 GATV0(z,INT,m*n,1+wf); MCISH(AS(z),AS(w),wf+1-!wcr) if(!wcr)AS(z)[wf]=1; zv=AVn(1+wf,z);
 if(2==k){US*v;
  for(i=0;i<m;++i){
   mvc(p*sizeof(*yv),yv,MEMSET00LEN,MEMSET00); 
   for(j=0,v=(US*)wv;j<n;++j){I bb=*v++; bb|=bb<<9; bb>>=8; ++yv[bb&3];} // convert 2 bits to 1 value, big-endian
   RANKINGSUMSCAN(p);
   for(j=0,v=(US*)wv;j<n;++j){I bb=*v++; bb|=bb<<9; bb>>=8; *zv++=yv[bb&3]++;}  // fetch the value
   wv+=n*k;
  }
 }else{int*v;
  for(i=0;i<m;++i){
   mvc(p*sizeof(*yv),yv,MEMSET00LEN,MEMSET00); 
   for(j=0,v=(int*)wv;j<n;++j){I bb=*v++; bb|=bb<<9; bb|=bb<<18; bb>>=24; ++yv[bb&0xf];}  // big-endian
   RANKINGSUMSCAN(p);
   for(j=0,v=(int*)wv;j<n;++j){I bb=*v++; bb|=bb<<9; bb|=bb<<18; bb>>=24; *zv++=yv[bb&0xf]++;}
   wv+=n*k;
  }
 }
 R z;
}    /* /:@/: w where w is boolean and items have length 2 or 4 */

// /:@/:
F1(jtranking){A y,z;C*wv;I icn,i,k,m,n,t,wcr,wf,wn,wr,*ws,wt,*zv;CR rng;TTYPE *yv,*yu;
 ARGCHK1(w);
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 wt=AT(w); wv=CAV(w); wn=AN(w);
 ws=AS(w); SETICFR(w,wf,wcr,n);   // n=#cells in w; m is number of atoms in w
 if(likely(wn!=0)){PROD(m,wf,ws);}  // If there are atoms, calculate result-shape the fast way
 else{RE(m=prod(wf,ws)); R m?reitem(vec(INT,wf,ws),iota(v2(1L,n))):reshape(vec(INT,1+wf,ws),num(0));}
 PROD(icn,wcr-1,ws+wf+1); k=icn<<bplg(wt);  // wk=size of atom in bytes; icn=# atoms in an item of a cell  k = *bytes in an item of a CELL of w
 k&=REPSGN(SGNIFDENSE(wt));  // if sparse w, set k so as to disable all special cases
 // if Boolean 2- or 4-byte, go off to handle that special case
 if(ISDENSETYPE(wt,B01)&&(k==2||k==sizeof(int)))R rankingb(w,wf,wcr,m,n,k);
 // See if the values qualify for small-range processing
 if(icn==1&&ISDENSETYPE(wt,INT+C4T)){  // items are individual INTs or C4Ts
  // Calculate the largest range we can abide.  The cost of a sort is about n*lg(n)*4 cycles; the cost of small-range indexing is
  // range*4.5 (.5 to clear, 2 to read) + n*6 (4 to increment, 2 to write).  So range can be as high as n*lg(n)*4/4.5 - n*6/4.5
  // approximate lg(n) with bit count.  And always use small-range if range is < 256
  UI4 lgn=CTLZI(wn);
  I maxrange = wn<64?256:(I)((lgn*4-6)*(D)wn/(4.5*(D)icn));
  rng = ISDENSETYPE(wt,INT)?condrange((I*)wv,wn,IMAX,IMIN,maxrange):condrange4((C4*)wv,wn,-1,0,maxrange);
 }else if(unlikely(((k-1)&-2)==0)){
  // special case of k==2.  We assume the worst on range, i. e. 8 bits/byte, EXCEPT that for B01 type we know that the value can never exceed
  // 1 so we add 2 instead of the last 8 bits
  rng.min=0; rng.range=(1LL<<((k-(wt&B01))<<3))+((wt&B01)<<(k-1));  // 2, 256, 258, or 65536
 }else rng.range=0;
 if(!rng.range){I *yv;
  // small-range not possible.  Do the grade and install each value into its location
  RZ(IRS1(w,0L,wcr,jtgrade1,y)); yv=AV(y); 
  GATV0(z,INT,m*n,1+wf); MCISH(AS(z),ws,wf+1-!wcr) if(!wcr)AS(z)[wf]=1; zv=AVn(1+wf,z); 
  DO(m, DO(n, zv[*yv++]=i;); zv+=n;);
  RETF(z);
 }
 // here for small-range ordinals, processed through the ranking loop
 GATV0(z,INT,m*n,1+wf); MCISH(AS(z),ws,wf+1-!wcr) if(!wcr)AS(z)[wf]=1; zv=AVn(1+wf,z);
 GATV0(y,C4T,rng.range,1); yv=C4AV1(y); yu=yv-rng.min;
 for(i=0;i<m;++i){
  mvc(rng.range*sizeof(*yv),yv,MEMSET00LEN,MEMSET00);
  switch(k){
  case sizeof(I): if(wt&INT){RANKINGLOOP(I); break;}
#if SY_64
  case sizeof(C4):
#endif
   RANKINGLOOP(C4); break;
  case sizeof(C):   RANKINGLOOP(UC); break;
#if C_LE
  case sizeof(S):
   if(wt&IS1BYTE){I c,d,s,t;US*v;TTYPE *u;
    v=(US*)wv; DQ(n, ++yu[*v++];);
    s=0;       DO(256, c=0; d=i; DQ(256, u=yv+(c+d); c+=256; if(*u){t=*u; *u=(TTYPE)s; s+=t;}););
    v=(US*)wv; DQ(n, *zv++=yu[*v++]++;);
   }else RANKINGLOOP(US);
#else
  case sizeof(S):   RANKINGLOOP(US);
#endif
  }
  wv+=n*k;
 }
 RETF(z);
}
