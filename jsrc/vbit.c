/* Copyright 1990-2004, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Bit Type                                                         */

#if 0  // no bit type
/* 1 bit per element                                                       */
/* total # elements <: IMAX                                                */
/* dimension length <: IMAX                                                */
/* last dimension internally padded with 0s up to fullword                 */

#include "j.h"

#define bitandBB(x,y)      jtbitandBB(jt,(x),(y))
#define biteqBB(x,y)       jtbiteqBB(jt,(x),(y))
#define biteqII(x,y)       jtbiteqII(jt,(x),(y))
#define biterror(x,y)      jtbiterror(jt,(x),(y))
#define bitgeBB(x,y)       jtbitgeBB(jt,(x),(y))
#define bitgeII(x,y)       jtbitgeII(jt,(x),(y))
#define bitgtBB(x,y)       jtbitgtBB(jt,(x),(y))
#define bitgtII(x,y)       jtbitgtII(jt,(x),(y))
#define bitleBB(x,y)       jtbitleBB(jt,(x),(y))
#define bitleII(x,y)       jtbitleII(jt,(x),(y))
#define bitltBB(x,y)       jtbitltBB(jt,(x),(y))
#define bitltII(x,y)       jtbitltII(jt,(x),(y))
#define bitnandBB(x,y)     jtbitnandBB(jt,(x),(y))
#define bitneBB(x,y)       jtbitneBB(jt,(x),(y))
#define bitneII(x,y)       jtbitneII(jt,(x),(y))
#define bitnorBB(x,y)      jtbitnorBB(jt,(x),(y))
#define bitorBB(x,y)       jtbitorBB(jt,(x),(y))


F2(jtbitmatch){B*v;I ar,*as,at,c,d,i,q,r,r1,rc,m,wr,*ws,wt;UC k,p,*u;
 RZ(a&&w);
 ar=AR(a); as=AS(a); at=AT(a); 
 wr=AR(w); ws=AS(w); wt=AT(w);
 ASSERTSYS(at&BIT||wt&BIT,"bitmatch");
 ASSERT(at&BIT+B01&&wt&BIT+B01,EVNONCE);
 if(ar!=wr)R zero;
 if(memcmp(as,ws,ar))R zero;
 c=ar?as[ar-1]:1; m=c?AN(a)/c:0; d=(c+BW-1)/BW;
 q=c/BB; r=c%BB; rc=c%BW; r1=rc?(BW-rc)/BB:0;
 if(at&BIT&&wt&BIT)R memcmp(AV(a),AV(w),m*d*SZI)?zero:one;
 if(at&BIT){u=UAV(a); v=BAV(w);}
 else      {u=UAV(w); v=BAV(a);}
 for(i=0;i<m;++i){
  DO(q, k=*u++; p=(UC)128; DO(BB, if((1&&*v++)!=(1&&k&p))R zero; p>>=1;););
  if(r){k=*u++; p=(UC)128; DO(r,  if((1&&*v++)!=(1&&k&p))R zero; p>>=1;);}
  u+=r1;
 }
 R one;
}

static UI bitmask(I c){I rc;UI mask;
 mask=0;
 if(rc=c%BW){
  mask=(~mask)<<(BW-rc);
#if C_LE
  {UC c,*v=(UC*)&mask;I j=SZI; DO(SZI/2, --j; c=v[i]; v[i]=v[j]; v[j]=c;);}
#endif
 }
 R mask;
}    /* bit mask for an array c columns wide */

static F1(jtbitcvt){R cvt(BIT,w);}

static F1(jtbitvfypad){I c,d,i,m,rc,wn,wr,*ws;UI mask,*u;
 RZ(w);
 ASSERT(BIT&AT(w),EVDOMAIN);
 wn=AN(w); wr=AR(w); ws=AS(w);
 c=wr?ws[wr-1]:1; m=c?wn/c:0; rc=c%BW;
 if(!rc)R w;
 d=(c+BW-1)/BW; u=(UI*)AV(w);
 mask=~bitmask(c);
 for(i=0;i<m;++i){
  ASSERTSYS(!(mask&u[d-1]),"bitvfypad"); 
  u+=d;
 }
 R w;
}    /* verify that the pad bits in bit array w are zero */

static I bitsum(I n,UC*b){I z=0;
#if SY_64
 DO((n+BW-1)/BW, z+=bitc[*b++]; z+=bitc[*b++]; z+=bitc[*b++]; z+=bitc[*b++];
                 z+=bitc[*b++]; z+=bitc[*b++]; z+=bitc[*b++]; z+=bitc[*b++];);
#else
 DO((n+BW-1)/BW, z+=bitc[*b++]; z+=bitc[*b++]; z+=bitc[*b++]; z+=bitc[*b++];);
#endif
 R z;
}    /* +/ bit vector */


static F1(jtbitslplus){A t,z;I i,m,mm,n,n1,p,q,*u,wr,*ws,*zv;UC c,*vc,*wv;UI*v,*v0;static UI ptab[256];
 RZ(w);
 ASSERT(BIT&AT(w),EVDOMAIN);
 wr=AR(w);
 if(1>=wr)R sc(bitsum(AN(w),UAV(w)));
 ws=AS(w); m=*ws; if(m)n=AN(w)/m; else PROD(n,wr-1,1+ws); 
 GATV(z,INT,n,wr-1,1+ws); zv=AV(z); memset(zv,C0,SZI*n);
 if(!n)R z;
 if(!m){memset(zv,C0,n*SZI); R z;}
#if SY_64
 mm=255;
 if(!ptab[1]){UC*v;UI x; v=(UC*)&x; DO(256, c=(UC)i; x=0; DO(8, if(c&(UC)1)v[7-i]=1; c>>=1;); ptab[i]=x;);}
#else
 mm=15;
 if(!ptab[1]){UI t[8],x;
  DO(8, t[i]=0; *((i>>1)+(UC*)(i+t))=(i&1)?0x01:0x10;);
  DO(256, c=(UC)i; x=0; DO(8, if(c&(UC)1)x|=t[i]; c>>=1;); ptab[i]=x;);
 }
#endif
 n1=(n+BW-1)/BW; n1*=SZI; q=(n+BB-1)/BB;
 GATV0(t,INT,q,1); v=v0=(UI*)AV(t); wv=UAV(w);
 for(i=(m+mm-1)/mm-1;i>=0;--i){
  v=v0; DO(q, *v++=0;);
  p=i?mm:m%mm;
  DO(p, v=v0; DO(q, *v+++=ptab[*wv++];); wv+=n1-q;);
  vc=(UC*)v0; u=zv; DO(n, c=*vc++; *u+++=c>>4; *u+++=c&(UC)0x0f;);
 }
 R z;
}    /* +/ bit vector */

static F1(jtbitsland){I n;UI ff,*v;
 RZ(w);
 ASSERT(BIT&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVNONCE);
 n=AN(w); v=(UI*)AV(w);
 ff=~(UI)0;
 DO(n/BW, if(ff!=*v++)R zero;);
 if(n%BW)R *v==bitmask(n)?one:zero;
 R one;
}    /* *./ bit vector */

static F1(jtbitslor){I n;UI*v;
 RZ(w);
 ASSERT(BIT&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVNONCE);
 n=AN(w); v=(UI*)AV(w);
 DO(n/BW, if(*v++)R one;);
 if(n%BW)R *v&bitmask(n)?one:zero;
 R zero;
}    /* +./ bit vector */

static F1(jtbitslne){I n;UC c,*v;
 RZ(w);
 ASSERT(BIT&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVNONCE);
 n=AN(w); c=0; v=UAV(w);
#if SY_64
 DO((n+BW-1)/BW, c^=bitc[*v++]; c^=bitc[*v++]; c^=bitc[*v++]; c^=bitc[*v++]; c^=bitc[*v++]; c^=bitc[*v++]; c^=bitc[*v++]; c^=bitc[*v++];);
#else
 DO((n+BW-1)/BW, c^=bitc[*v++]; c^=bitc[*v++]; c^=bitc[*v++]; c^=bitc[*v++];);
#endif
 R c&(UC)1?one:zero;
}    /* ~:/ bit vector */

static F1(jtbitsleq){I n;UC c,*v;
 RZ(w);
 ASSERT(BIT&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVNONCE);
 n=AN(w); c=0; v=UAV(w);
#if SY_64
 DO((n+BW-1)/BW, c^=bitc[*v++]; c^=bitc[*v++]; c^=bitc[*v++]; c^=bitc[*v++]; c^=bitc[*v++]; c^=bitc[*v++]; c^=bitc[*v++]; c^=bitc[*v++];);
#else
 DO((n+BW-1)/BW, c^=bitc[*v++]; c^=bitc[*v++]; c^=bitc[*v++]; c^=bitc[*v++];);
#endif
 R (n-c)&(UC)1?zero:one;
}    /* =/ bit vector */


static F1(jtbitscanand){A z;I c,j,m,n;UC ffc,k,p,*u;UI ff,*v,*zv;
 RZ(w);
 ASSERT(BIT&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVNONCE);
 n=AN(w); v=(UI*)AV(w);
 GA(z,BIT,n,AR(w),AS(w)); zv=(UI*)AV(z);
 m=(n+BW-1)/BW; j=-1; ff=~(UI)0;
 DO(m, if(ff==*v++)*zv++=ff; else{j=i; --v; break;});
 if(0<=j){
  u=(UC*)v; ffc=~(UC)0;
  DO(SZI, if(ffc!=*u++){--u; break;});
  c=u-(UC*)v; k=*u; p=0; 
  DO(BB, if(k&bit[i])p|=bit[i]; else break;); 
  *zv=ff; u=c+(UC*)zv; *u++=p;
  DO((SZI-c)-1, *u++=0;);
  DO((m-j)-1, *++zv=0;);
 }
 R z;
}    /* *./\ bit vector */

static UC nepfx[256];
static UC eqpfx[256];

static F1(jtbitscanne){A z;I n;UC c,d,p,*v,x,*zv;
 RZ(w);
 ASSERT(BIT&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVNONCE);
 if(!nepfx[255])DO(256, p=(UC)i; c=128; x=d=p&c; DO(7, c>>=1; d|=x=p&c?(x?0:c):(x?c:0);); nepfx[i]=d;);
 x=0; n=AN(w); v=UAV(w);
 GA(z,BIT,n,AR(w),AS(w)); zv=UAV(z);
 DO((n+BB-1)/BB, d=nepfx[*v++]; *zv++=x=1&x?~d:d;);
 if(n%BW)*(n/BW+(UI*)AV(z))&=bitmask(n);
 R z;
}    /* ~:/\ bit vector */

static F1(jtbitscaneq){A z;I n;UC c,d,p,*v,x,*zv;
 RZ(w);
 ASSERT(BIT&AT(w),EVDOMAIN);
 ASSERT(1>=AR(w),EVNONCE);
 if(!eqpfx[255])DO(256, p=(UC)i; c=128; x=d=p&c; DO(7, c>>=1; d|=x=p&c?(x?c:0):(x?0:c);); eqpfx[i]=d;);
 x=1; n=AN(w); v=UAV(w);
 GA(z,BIT,n,AR(w),AS(w)); zv=UAV(z);
 DO((n+BB-1)/BB, d=eqpfx[*v++]; *zv++=x=1&x?d:~d;);
 if(n%BW)*(n/BW+(UI*)AV(z))&=bitmask(n);
 R z;
}    /* =/\ bit vector */


static F1(jtbitnot){A z;I c,m,rc,wn,wr,*ws;UI mask,*u,*zv;
 RZ(w);
 ASSERT(BIT&AT(w),EVDOMAIN); 
 wn=AN(w); wr=AR(w); ws=AS(w); u=(UI*)AV(w); 
 c=wr?ws[wr-1]:1; m=c?wn/c:0; rc=c%BW;
 mask=bitmask(c);
 GA(z,BIT,wn,wr,ws); zv=(UI*)AV(z);
 if(rc)DO(m, DO(c/BW, *zv++=~*u++;); *zv++=~*u++&mask;)
 else  DO(m, DO(c/BW, *zv++=~*u++;););
 R z;
}    /* -. bit array */

static F1(jtbitravel){A z;I c,m,rw,wn,wr,*ws;UC*wv,*zv;
 RZ(w);
 ASSERT(BIT&AT(w),EVDOMAIN); 
 wn=AN(w); wr=AR(w); ws=AS(w); wv=UAV(w); 
 c=wr?ws[wr-1]:1; m=c?wn/c:0; rw=c%BW; 
 GA(z,BIT,wn,1,0); zv=UAV(z);
 if(rw){I j,q,r,r1,t,ti;UC k,y;
  q=c/BB; r=c%BB; r1=rw?(BW-rw)/BB:0;
  k=0; t=0; ti=BB-t;
  for(j=0;j<m;++j){
   if(t)DO(q, y=*wv++; k|=y>>t; *zv++=k; k=y<<ti;)
   else if(q){MC(zv,wv,q); zv+=q; wv+=q;}
   if(r){y=*wv++; k|=y>>t; if(BB<=t+r){*zv++=k; k=y<<ti;} t=(t+r)%BB; ti=BB-t;}
   wv+=r1;
  }
  if(r&&t)*zv++=k;
  if(wn%BW)memset(zv,C0,(BW-wn%BW)/BB);
 }else MC(zv,wv,wn/BB);
 R z;
}    /* , bit array */

static F2(jtbitcat){A z;I an,ar,*as,t,ti,wn,wr,*ws;UC*zv;
 RZ(a&&w);
 ASSERT(BIT&AT(a)&&BIT&AT(w),EVDOMAIN);
 an=AN(a); ar=AR(a); as=AS(a);
 wn=AN(w); wr=AR(w); ws=AS(w);
 ASSERT(1>=ar&&1>=wr,EVNONCE);
 GA(z,BIT,an+wn,1,0); zv=UAV(z);
 MC(zv,AV(a),SZI*((an+BW-1)/BW));
 zv+=an/BB;
 if(t=an%BB){UC k,*wv,y;
  ti=BB-t; wv=UAV(w); k=*zv;
  DO((wn+BB-1)/BB, y=*wv++; k|=y>>t; *zv++=k; k=y<<ti;);
  *zv++=k;
 }else MC(zv,AV(w),SZI*((wn+BW-1)/BW));
 R z;
}    /* bit array , bit array */

static F2(jtbitfrom){A z;I an,ar,*as,c,i,j,m,n,q,r,rc,r1,wr,*ws;UC k,*v,*zv;
 RZ(a&&w);
 ASSERT(INT&AT(a),EVNONCE);
 ASSERT(BIT&AT(w),EVDOMAIN);
 an=AN(a); ar=AR(a); as=AS(a); wr=AR(w); ws=AS(w);
 if(1>=wr){I*u;
  c=ar?as[ar-1]:1; m=c?an/c:0; q=c/BB; r=c%BB; rc=c%BW; r1=rc?(BW-rc)/BB:0;
  GA(z,BIT,an,ar,as); zv=UAV(z);
  u=AV(a); v=UAV(w); n=AN(w);
  for(i=0;i<m;++i){
   DO(q, k=0; DO(BB, j=*u++; if(0>j)j+=n; ASSERT(0<=j&&j<n,EVINDEX); if(v[j/BB]&(UC)128>>j%BB)k|=bit[i];); *zv++=k;);
   if(r){k=0; DO(r,  j=*u++; if(0>j)j+=n; ASSERT(0<=j&&j<n,EVINDEX); if(v[j/BB]&(UC)128>>j%BB)k|=bit[i];); *zv++=k;}
   DO(r1, *zv++=0;);
 }}else{A x;I*u,*v,zn,zr,*zv;
  zr=ar+wr-1;
  GATV0(x,INT,zr,1); u=AV(x); 
  ICPY(u,as,ar); ICPY(u+ar,1+ws,wr-1); RE(zn=prod(zr,u));
  GA(z,BIT,zn,zr,u); zv=AV(z);
  n=*ws; c=(ws[wr-1]+BW-1)/BW; RE(m=mult(prod(wr-2,1+ws),c));
  u=AV(a); v=AV(w); 
  DO(an, j=*u++; if(0>j)j+=n; ASSERT(0<=j&&j<n,EVINDEX); ICPY(zv,v+j*m,m); zv+=m;);
 }
 R z;
}    /* integer array { bit array */


static F2(jtbiterror){ASSERT(0,EVNONCE);}

#define BITFBB(f,OP)  \
 F2(f){A z;I c,m,mask,rc,wn,wr,*ws;UI*u,*v,*zv;  \
  RZ(a&&w);                                      \
  wn=AN(w); wr=AR(w); ws=AS(w);                  \
  ASSERT(BIT&AT(a)&&BIT&AT(w),EVDOMAIN);         \
  ASSERT(wr==AR(a),EVRANK);                      \
  ASSERT(!memcmp(ws,AS(a),wr),EVLENGTH);         \
  c=wr?ws[wr-1]:1; m=c?wn/c:0; rc=c%BW;          \
  u=(UI*)AV(a); v=(UI*)AV(w);                    \
  mask=bitmask(c);                               \
  GA(z,BIT,wn,wr,ws); zv=(UI*)AV(z);             \
  if(rc)DO(m, DO(c/BW, *zv++=OP(*u++,*v++);); *zv++=OP(*u++,*v++)&mask;)  \
  else  DO(m, DO(c/BW, *zv++=OP(*u++,*v++);););  \
  R z;                                           \
 }   /* bit array op bit array */

#define BITFII(f,OP)  \
 F2(f){A z;I an,ar,*as,c,i,m,q,r,rc,r1,*u,*v;UC k,*zv;         \
  RZ(a&&w);                                                    \
  an=AN(a); ar=AR(a); as=AS(a);                                \
  ASSERT(INT&AT(a)&&INT&AT(w),EVDOMAIN);                       \
  ASSERT(AR(a)==AR(w),EVRANK);                                 \
  ASSERT(!memcmp(as,AS(w),ar),EVLENGTH);                       \
  c=ar?as[ar-1]:1; m=c?an/c:0; q=c/BB; r=c%BB; rc=c%BW; r1=rc?(BW-rc)/BB:0;  \
  GA(z,BIT,an,ar,as); zv=UAV(z);                               \
  u=AV(a); v=AV(w); zv=UAV(z);                                 \
  for(i=0;i<m;++i){                                            \
   DO(q, k=0; DO(BB, if(OP(*u++,*v++))k|=bit[i];); *zv++=k;);  \
   if(r){k=0; DO(r,  if(OP(*u++,*v++))k|=bit[i];); *zv++=k;}   \
   DO(r1, *zv++=0;);                                           \
  }                                                            \
  R z;                                                         \
 }   /* integer array op integer array */

#define BITF(f,fBB,fII)  \
 F2(f){I at;                                    \
  RZ(a&&w);                                     \
  at=AT(a);                                     \
  ASSERT(TYPESEQ(at,AT(w))&&at&BIT+INT,EVNONCE);        \
  R at&BIT?fBB(a,w):fII(a,w);                   \
 }

#define BBEQ(x,y)   (~(x^y))
#define BBLT(x,y)   (~x&y)
#define BBLE(x,y)   (~x|y)
#define BBGT(x,y)   (x&~y)
#define BBGE(x,y)   (x|~y)
#define BBOR(x,y)   (x|y)
#define BBNOR(x,y)  (~(x|y))
#define BBAND(x,y)  (x&y)
#define BBNAND(x,y) (~(x&y))
#define BBNE(x,y)   (x^y)

#define IILT(x,y)   (x< y)
#define IILE(x,y)   (x<=y)
#define IIEQ(x,y)   (x==y)
#define IIGE(x,y)   (x>=y)
#define IIGT(x,y)   (x> y)
#define IINE(x,y)   (x!=y)

static BITFBB(jtbiteqBB,  BBEQ  )
static BITFBB(jtbitltBB,  BBLT  )
static BITFBB(jtbitleBB,  BBLE  )
static BITFBB(jtbitgtBB,  BBGT  )
static BITFBB(jtbitgeBB,  BBGE  )
static BITFBB(jtbitorBB,  BBOR  )
static BITFBB(jtbitnorBB, BBNOR )
static BITFBB(jtbitandBB, BBAND )
static BITFBB(jtbitnandBB,BBNAND)
static BITFBB(jtbitneBB,  BBNE  )

static BITFII(jtbiteqII,IIEQ)
static BITFII(jtbitltII,IILT)
static BITFII(jtbitleII,IILE)
static BITFII(jtbitgtII,IIGT)
static BITFII(jtbitgeII,IIGE)
static BITFII(jtbitneII,IINE)

static BITF(jtbiteq,  biteqBB,  biteqII  )
static BITF(jtbitlt,  bitltBB,  bitltII  )
static BITF(jtbitle,  bitleBB,  bitleII  )
static BITF(jtbitgt,  bitgtBB,  bitgtII  )
static BITF(jtbitge,  bitgeBB,  bitgeII  )
static BITF(jtbitor,  bitorBB,  biterror )
static BITF(jtbitnor, bitnorBB, biterror )
static BITF(jtbitand, bitandBB, bitandBB )
static BITF(jtbitnand,bitnandBB,bitnandBB)
static BITF(jtbitne,  bitneBB,  bitneII  )

static F1(jtbitgrade){A x;I r,*s;
 RZ(w);
 ASSERT(BIT&AT(w),EVDOMAIN);
 r=AR(w);
 ASSERT(2<=r,EVNONCE);
 RZ(x=gah(r,w));
 AT(x)=LIT; 
 s=AS(x); ICPY(s,AS(w),r); s[r-1]=SZI*((s[r-1]+BW-1)/BW); AN(x)=prod(r,s);
 R grade1(x);
}    /* /: bit array */

static F2(jtbitindexof){I j,n;UC*u,y;UI*v,x;
 RZ(a&&w);
 ASSERT(BIT&AT(a),EVDOMAIN);
 ASSERT(1>=AR(a)&&!AR(w),EVNONCE);
 if(!(AT(w)&BIT+B01))RZ(w=cvt(BIT,w));
 x=*UAV(w)?0:~(UI)0; 
 n=AN(a); v=(UI*)AV(a); j=n;
 DO((n+BW-1)/BW, if(x!=*v++){j=BW*i; --v; break;});
 if(n!=j){
  u=(UC*)v; y=*(UC*)&x; DO(SZI, if(y!=*u++){j+=i*BB; --u; break;});
  y=x?~*u:*u; 
  DO(BB, if(y&bit[i]){j+=i; break;});
 }
 R sc(j);
}    /* bit vector i. 0 or 1 */

#define REPSWITCH(c,exp0,exp1) \
 switch(c){                               \
  case 0:  exp0; exp0; exp0; exp0; break; \
  case 1:  exp0; exp0; exp0; exp1; break; \
  case 2:  exp0; exp0; exp1; exp0; break; \
  case 3:  exp0; exp0; exp1; exp1; break; \
  case 4:  exp0; exp1; exp0; exp0; break; \
  case 5:  exp0; exp1; exp0; exp1; break; \
  case 6:  exp0; exp1; exp1; exp0; break; \
  case 7:  exp0; exp1; exp1; exp1; break; \
  case 8:  exp1; exp0; exp0; exp0; break; \
  case 9:  exp1; exp0; exp0; exp1; break; \
  case 10: exp1; exp0; exp1; exp0; break; \
  case 11: exp1; exp0; exp1; exp1; break; \
  case 12: exp1; exp1; exp0; exp0; break; \
  case 13: exp1; exp1; exp0; exp1; break; \
  case 14: exp1; exp1; exp1; exp0; break; \
  case 15: exp1; exp1; exp1; exp1; break; \
 }

#define REPDO(T,exp0,exp1)  {T*u=(T*)wv,*v=(T*)zv;      \
 DO((n+BB-1)/BB, k=*av++; REPSWITCH(k>>4, exp0, exp1); REPSWITCH(k&15, exp0, exp1););}


static F2(jtbitrepeat){A z;I c,c1,m,n,p,wr,wk,wt;UC*av,k;
 RZ(a&&w);
 ASSERT(BIT&AT(a),EVDOMAIN);
 n=AN(a); av=UAV(a); wr=AR(w); wt=AT(w);
 p=AR(w)?*AS(w):1; m=AN(w)/p; c=wr?*(AS(w)+wr-1):1; c1=(c+BW-1)/BW;
 wk=wt&BIT?SZI*c1*(m/c):m<<bplg(wt);
 ASSERT(n==p,EVLENGTH);
 p=bitsum(n,av);
 GA(z,wt,p*m,AR(w),AS(w)); *AS(z)=p;
 if(BIT&wt&&1>=wr){I j;UC c,d,*wv,*zv;
  wv=UAV(w); zv=UAV(z); j=0;
  DO((n+BB-1)/BB, k=*av++; c=*wv++; d=0; DO(BB, if(k&bit[i]){d|=k&bit[i]&c;}); *zv++=d;);
 }else{I*wv,*zv;
  wv=AV(w); zv=AV(z);
  switch(wk){
   case sizeof(C):   REPDO(C,u++,  *v++=*u++);                       break;
   case sizeof(S):   REPDO(S,u++,  *v++=*u++);                       break;
   case sizeof(C)*3: REPDO(C,u+=3, (*v++=*u++,*v++=*u++,*v++=*u++)); break;
#if SY_64
   case sizeof(int): REPDO(int,u++,*v++=*u++);                       break;
#endif
   case sizeof(I):   REPDO(I,u++,  *v++=*u++);                       break;
   case sizeof(I)*2: REPDO(I,u+=2, (*v++=*u++,*v++=*u++));           break;
   case sizeof(I)*3: REPDO(I,u+=3, (*v++=*u++,*v++=*u++,*v++=*u++)); break;
   case sizeof(I)*4: REPDO(I,u+=4, (*v++=*u++,*v++=*u++,*v++=*u++,*v++=*u++)); break;
   default:          REPDO(C,u+=wk,(MC(v,u,wk),u+=wk,v+=wk));
 }}
 R z;
}    /* bit vector # array */


#define BITDEF(f1,f2)  fdef(0,CIBEAM,VERB, f1,f2, w,0L,0L, VASGSAFE, RMAX,RMAX,RMAX)
#define BITDEF1(f1)    fdef(0,CIBEAM,VERB, f1,0L, w,0L,0L, VASGSAFE, RMAX,RMAX,RMAX)
#define BITDEF2(f2)    fdef(0,CIBEAM,VERB, 0L,f2, w,0L,0L, VASGSAFE, RMAX,RMAX,RMAX)

F1(jtbitadv){A x;V*v;
 RZ(w); v=VAV(w);
 switch(NOUN&AT(w)?i0(w):v->id){
  default:         ASSERT(0,EVDOMAIN);
  case 0:          R BITDEF1(jtbitcvt);
  case 1:          R BITDEF1(jtbitvfypad);
  case CNOT:       R BITDEF1(jtbitnot);
  case CGRADE:     R BITDEF1(jtbitgrade);

  case CCOMMA:     R BITDEF(jtbitravel,jtbitcat);

  case CIOTA:      R BITDEF2(jtbitindexof);
  case CPOUND:     R BITDEF2(jtbitrepeat);
  case CPLUSDOT:   R BITDEF2(jtbitor);
  case CPLUSCO:    R BITDEF2(jtbitnor);
  case CSTAR:
  case CSTARDOT:   R BITDEF2(jtbitand);
  case CSTARCO:    R BITDEF2(jtbitnand);
  case CLT:        R BITDEF2(jtbitlt);
  case CLE:        R BITDEF2(jtbitle);
  case CEQ:        R BITDEF2(jtbiteq);
  case CGT:        R BITDEF2(jtbitgt);
  case CGE:        R BITDEF2(jtbitge);
  case CNE:        R BITDEF2(jtbitne);
  case CFROM:      R BITDEF2(jtbitfrom);
  case CSLASH:
   switch(ID(v->fgh[0])){
    default:       ASSERT(0,EVDOMAIN);
    case CEQ:      R BITDEF1(jtbitsleq);
    case CPLUS:    R BITDEF1(jtbitslplus);
    case CPLUSDOT: R BITDEF1(jtbitslor);
    case CSTARDOT: R BITDEF1(jtbitsland);
    case CNE:      R BITDEF1(jtbitslne);
   }
  case CBSLASH:
   x=v->fgh[0]; ASSERT(CSLASH==ID(x),EVDOMAIN); x=VAV(x)->fgh[0];
   switch(ID(x)){
    default:       ASSERT(0,EVDOMAIN);
    case CSTARDOT: R BITDEF1(jtbitscanand);
    case CNE:      R BITDEF1(jtbitscanne);
    case CEQ:      R BITDEF1(jtbitscaneq);
}}}
#endif