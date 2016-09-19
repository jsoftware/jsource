/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Grade -- monad /:@/:"r  on dense arguments                       */

#include "j.h"
#include "vg.h"


#define RANKINGSUMSCAN   \
 {I s,*u; s=0; u=yv; DO(p, if(*u){t=*u; *u=s; s+=t;} ++u;);}
#define RANKINGLOOP(T)   \
 {T*v=(T*)wv;  DO(n, ++yu[*v++];); RANKINGSUMSCAN; v=(T*)wv;  DO(n, *zv++=yu[*v++]++;);}

static A jtrankingb(J jt,A w,I wf,I wcr,I m,I n,I k){A z;C*wv;I i,j,p,t,yv[16],*zv;
 p=2==k?4:16; wv=CAV(w);
 GATV(z,INT,m*n,1+wf,AS(w)); if(!wcr)*(AS(z)+wf)=1; zv=AV(z);
 if(2==k){US*v;
  for(i=0;i<m;++i){
   memset(yv,C0,p*SZI); 
   for(j=0,v=(US*)wv;j<n;++j)switch(*v++){
    case BS00: ++yv[0]; break;
    case BS01: ++yv[1]; break;
    case BS10: ++yv[2]; break;
    case BS11: ++yv[3]; break;
   }
   RANKINGSUMSCAN;
   for(j=0,v=(US*)wv;j<n;++j)switch(*v++){
    case BS00: *zv++=yv[0]++; break;
    case BS01: *zv++=yv[1]++; break;
    case BS10: *zv++=yv[2]++; break;
    case BS11: *zv++=yv[3]++; break;
   }
   wv+=n*k;
 }}else{int*v;
  for(i=0;i<m;++i){
   memset(yv,C0,p*SZI); 
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
   RANKINGSUMSCAN;
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

F1(jtranking){A y,z;C*wv;I i,k,m,n,p=0,q=0,t,wcr,wf,wk,wr,*ws,wt,*yu,*yv,*zv;
 RZ(w);
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr; jt->rank=0;
 wt=AT(w); wv=CAV(w);
 ws=AS(w); n=wcr?ws[wf]:1; RE(m=prod(wf,ws));
 if(!AN(w))R m?reitem(vec(INT,wf,ws),iota(v2(1L,n))):reshape(vec(INT,1+wf,ws),zero);
 wk=bp(wt); k=wk*(wcr?prod(wcr-1,ws+wf+1):1);  
 if(wt&B01&&(k==2||k==sizeof(int)))R rankingb(w,wf,wcr,m,n,k);
 else switch(k){
  case 1:   p=wt&B01?2:256; break;
  case 2:   p=65536; break;
  case sizeof(int): 
   if(wt&INT){irange(AN(w)/(k/wk),(I*)wv,&q,&p); if(!(65536>p||0.69*(p+2*n)<n*log((D)n)))p=0;}
   else if(wt&C4T){C4 cq=(C4)q; c4range(AN(w)/(k/wk),(C4*)wv,&cq,&p); q=cq; if(!(65536>p||0.69*(p+2*n)<n*log((D)n)))p=0;}
 }
 if(!p){
  RZ(y=irs1(w,0L,wcr,jtgrade1)); yv=AV(y); 
  GATV(z,INT,m*n,1+wf,ws); if(!wcr)*(AS(z)+wf)=1; zv=AV(z); 
  DO(m, DO(n, zv[*yv++]=i;); zv+=n;);
  R z;
 }
 GATV(z,INT,m*n,1+wf,ws); if(!wcr)*(AS(z)+wf)=1; zv=AV(z);
 GATV(y,INT,p,1,0); yv=AV(y); yu=yv-q;
 for(i=0;i<m;++i){
  memset(yv,C0,p*SZI);
  switch(k){
   case sizeof(int): if(wt&C4T){RANKINGLOOP(C4);} else {RANKINGLOOP(int);} break;
   case sizeof(C):   RANKINGLOOP(UC); break;
#if C_LE
   case sizeof(S):
    if(wt&IS1BYTE){I c,d,s,t,*u;US*v;
     v=(US*)wv; DO(n, ++yu[*v++];);
     s=0;       DO(256, c=0; d=i; DO(256, u=yv+(c+d); c+=256; if(*u){t=*u; *u=s; s+=t;}););
     v=(US*)wv; DO(n, *zv++=yu[*v++]++;);
    }else RANKINGLOOP(US);
#else
   case sizeof(S):   RANKINGLOOP(US);
#endif
  }
  wv+=n*k;
 }
 R z;
}
