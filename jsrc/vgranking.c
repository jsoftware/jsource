/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
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
 GATV(z,INT,m*n,1+wf,AS(w)); if(!wcr)AS(z)[wf]=1; zv=AV(z);
 if(2==k){US*v;
  for(i=0;i<m;++i){
   memset(yv,C0,p*sizeof(*yv)); 
   for(j=0,v=(US*)wv;j<n;++j)switch(*v++){
    case BS00: ++yv[0]; break;
    case BS01: ++yv[1]; break;
    case BS10: ++yv[2]; break;
    case BS11: ++yv[3]; break;
   }
   RANKINGSUMSCAN(p);
   for(j=0,v=(US*)wv;j<n;++j)switch(*v++){
    case BS00: *zv++=yv[0]++; break;
    case BS01: *zv++=yv[1]++; break;
    case BS10: *zv++=yv[2]++; break;
    case BS11: *zv++=yv[3]++; break;
   }
   wv+=n*k;
 }}else{int*v;
  for(i=0;i<m;++i){
   memset(yv,C0,p*sizeof(*yv)); 
   for(j=0,v=(int*)wv;j<n;++j)switch(*v++){
    case B0000: ++yv[ 0]; break;
    case B0001: ++yv[ 1]; break;
    case B0010: ++yv[ 2]; break;
    case B0011: ++yv[ 3]; break;
    case B0100: ++yv[ 4]; break;
    case B0101: ++yv[ 5]; break;
    case B0110: ++yv[ 6]; break;
    case B0111: ++yv[ 7]; break;
    case B1000: ++yv[ 8]; break;
    case B1001: ++yv[ 9]; break;
    case B1010: ++yv[10]; break;
    case B1011: ++yv[11]; break;
    case B1100: ++yv[12]; break;
    case B1101: ++yv[13]; break;
    case B1110: ++yv[14]; break;
    case B1111: ++yv[15]; break;
   }
   RANKINGSUMSCAN(p);
   for(j=0,v=(int*)wv;j<n;++j)switch(*v++){
    case B0000: *zv++=yv[ 0]++; break;
    case B0001: *zv++=yv[ 1]++; break;
    case B0010: *zv++=yv[ 2]++; break;
    case B0011: *zv++=yv[ 3]++; break;
    case B0100: *zv++=yv[ 4]++; break;
    case B0101: *zv++=yv[ 5]++; break;
    case B0110: *zv++=yv[ 6]++; break;
    case B0111: *zv++=yv[ 7]++; break;
    case B1000: *zv++=yv[ 8]++; break;
    case B1001: *zv++=yv[ 9]++; break;
    case B1010: *zv++=yv[10]++; break;
    case B1011: *zv++=yv[11]++; break;
    case B1100: *zv++=yv[12]++; break;
    case B1101: *zv++=yv[13]++; break;
    case B1110: *zv++=yv[14]++; break;
    case B1111: *zv++=yv[15]++; break;
   }
   wv+=n*k;
 }}
 R z;
}    /* /:@/: w where w is boolean and items have length 2 or 4 */

// /:@/:
F1(jtranking){A y,z;C*wv;I icn,i,k,m,n,t,wcr,wf,wn,wr,*ws,wt,*zv;CR rng;TTYPE *yv,*yu;
 RZ(w);
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 wt=AT(w); wv=CAV(w); wn=AN(w);
 ws=AS(w); n=wcr?ws[wf]:1;  // n=#cells in w; m is number of atoms in w
 if(wn){PROD(m,wf,ws);}  // If there are atoms, calculate result-shape the fast way
 else{RE(m=prod(wf,ws)); R m?reitem(vec(INT,wf,ws),iota(v2(1L,n))):reshape(vec(INT,1+wf,ws),num[0]);}
 PROD1(icn,wcr-1,ws+wf+1); k=icn<<bplg(wt);  // wk=size of atom in bytes; icn=# atoms in an item of a cell  k = *bytes in an item of a CELL of w
 // if Boolean 2- or 4-byte, go off to handle that special case
 if(wt&B01&&(k==2||k==sizeof(int)))R rankingb(w,wf,wcr,m,n,k);
 // See if the values qualify for small-range processing
 if(icn==1&&wt&INT+C4T){  // items are individual INTs or C4Ts
  // Calculate the largest range we can abide.  The cost of a sort is about n*lg(n)*4 cycles; the cost of small-range indexing is
  // range*4.5 (.5 to clear, 2 to read) + n*6 (4 to increment, 2 to write).  So range can be as high as n*lg(n)*4/4.5 - n*6/4.5
  // approximate lg(n) with bit count.  And always use small-range if range is < 256
  UI4 lgn; CTLZI(wn,lgn);
  I maxrange = wn<64?256:(I)((lgn*4-6)*(D)wn/(4.5*(D)icn));
  rng = wt&INT?condrange((I*)wv,wn,IMAX,IMIN,maxrange):condrange4((C4*)wv,wn,-1,0,maxrange);
 }else if(k<=2){rng.range=shortrange[wt&(B01+LIT)][k]; rng.min=0;  // if B01, must be 1 byte; otherwise 2^(8*k)
 }else rng.range=0;
 if(!rng.range){I *yv;
  // small-range not possible.  Do the grade and install each value into its location
  RZ(y=irs1(w,0L,wcr,jtgrade1)); yv=AV(y); 
  GATV(z,INT,m*n,1+wf,ws); if(!wcr)AS(z)[wf]=1; zv=AV(z); 
  DO(m, DO(n, zv[*yv++]=i;); zv+=n;);
  RETF(z);
 }
 // here for small-range ordinals, processed through the ranking loop
 GATV(z,INT,m*n,1+wf,ws); if(!wcr)AS(z)[wf]=1; zv=AV(z);
 GATV0(y,C4T,rng.range,1); yv=C4AV(y); yu=yv-rng.min;
 for(i=0;i<m;++i){
  memset(yv,C0,rng.range*sizeof(*yv));
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
     v=(US*)wv; DO(n, ++yu[*v++];);
     s=0;       DO(256, c=0; d=i; DO(256, u=yv+(c+d); c+=256; if(*u){t=*u; *u=(TTYPE)s; s+=t;}););
     v=(US*)wv; DO(n, *zv++=yu[*v++]++;);
    }else RANKINGLOOP(US);
#else
   case sizeof(S):   RANKINGLOOP(US);
#endif
  }
  wv+=n*k;
 }
 RETF(z);
}
