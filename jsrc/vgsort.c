/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Grade -- dyad /: and \: where a==w                               */

#include "j.h"
#include "vg.h"
#pragma intrinsic(memset)

// Places marked TUNE have parameters that must be tuned to the hardware

#define CXCHG2(v0,v1) {void *v2=v0; B t=COMPFN(compn,v0,v1); v0=(!t)?v1:v0; v1=(!t)?v2:v1;}

// Comparison functions.  Do one comparison before the loop for a fast exit if it differs.
// On VS this sequence, where a single byte is returned, creates a CMP/JE/SETL sequence, performing only one (fused) compare
// #define COMPGRADE(T,t) T av=*a, bv=*b; if(av!=bv) R av t bv; while(--n){++a; ++b; av=*a, bv=*b; if(av!=bv) R av t bv;} R a<b;
#define COMPGRADE(T,t) do{T av=*a, bv=*b; if(av!=bv) R av t bv; if(!--n)break; ++a; ++b;}while(1); R 1;
static __forceinline B compiu(I n, I *a, I *b){COMPGRADE(I,<)}
static __forceinline B compid(I n, I *a, I *b){COMPGRADE(I,>)}
static __forceinline B compdu(I n, D *a, D *b){COMPGRADE(D,<)}
static __forceinline B compdd(I n, D *a, D *b){COMPGRADE(D,>)}

// General sort, with comparisons by function call, but may do extra comparisons to avoid mispredicted branches
#define GRADEFNNAME jmsort
#define MERGEFNNAME jmerge
#define COMPFN (*comp)
#define T void
#define PTRADD(p,item) ((void *)((C*)p+(item)*bpi))
#define MVITEMS(dest,src,nitems) {MC(dest,src,(nitems)*bpi); dest=PTRADD(dest,nitems); src=PTRADD(src,nitems);}
#include "vgsort.h"

// Sorts with inline comparisons
#define GRADEFNNAME jmsortiu
#define MERGEFNNAME jmergeiu
#define COMPFN compiu
#define T I
#define PTRADD(p,item) ((p)+(item)*(compn))
#define MVITEMS(dest,src,nitems) {I _i = (nitems)*(compn); do{*dest++=*src++;}while(--_i);}
#include "vgsort.h"

#define GRADEFNNAME jmsortid
#define MERGEFNNAME jmergeid
#define COMPFN compid
#define T I
#define PTRADD(p,item) ((p)+(item)*(compn))
#define MVITEMS(dest,src,nitems) {I _i = (nitems)*(compn); do{*dest++=*src++;}while(--_i);}
#include "vgsort.h"

#define GRADEFNNAME jmsortdu
#define MERGEFNNAME jmergedu
#define COMPFN compdu
#define T D
#define PTRADD(p,item) ((p)+(item)*(compn))
#define MVITEMS(dest,src,nitems) {I _i = (nitems)*(compn); do{*dest++=*src++;}while(--_i);}
#include "vgsort.h"

#define GRADEFNNAME jmsortdd
#define MERGEFNNAME jmergedd
#define COMPFN compdd
#define T D
#define PTRADD(p,item) ((p)+(item)*(compn))
#define MVITEMS(dest,src,nitems) {I _i = (nitems)*(compn); do{*dest++=*src++;}while(--_i);}
#include "vgsort.h"


static struct {
 CMP comproutine;
 void *(*sortfunc)();
} sortroutines[][2] = {  // index is [bitx][up]
[B01X]={{compcd,jmsort},{compcu,jmsort}}, [LITX]={{compcd,jmsort},{compcu,jmsort}}, [INTX]={{0,(void *(*)())jmsortid},{0,(void *(*)())jmsortiu}}, [FLX]={{0,(void *(*)())jmsortdd},{0,(void *(*)())jmsortdu}},
[CMPXX]={{0,(void *(*)())jmsortdd},{0,(void *(*)())jmsortdu}},
[C2TX]={{compud,jmsort},{compuu,jmsort}}, [C4TX]={{comptd,jmsort},{comptu,jmsort}}
};

// sort for direct types, without pointers.  When the operand overflows cache the pointer method is very slow
// We support B01/LIT/C2T/C4T/INT/FL/CMPX
static A jtsortdirect(J jt,I m,I api,I n,A w){A x,z;I t;
 t=AT(w);
 // Create putative output area, same size as input.  If there is more than one cell in result, this will always be the result.
 GA(z,AT(w),AN(w),AR(w),AS(w));
 I cpi=api<<((t>>CMPXX)&1);  // compares per item on a sort
 I bpi=api<<bplg(t);  // bytes per item of a sort
 I bps=bpi*n;  // bytes per sort
 void * RESTRICT wv=voidAV(w); void * RESTRICT zv=voidAV(z);
 CMP cmpfunc=sortroutines[CTTZ(t)][(UI)jt->workareas.compare.complt>>(BW-1)].comproutine;
 void *(*sortfunc)() = sortroutines[CTTZ(t)][(UI)jt->workareas.compare.complt>>(BW-1)].sortfunc;
 // allocate the merge work area, large enough to hold one sort.  In case this turns out to be the final result,
 // make the shape the same as the result shape (if there is more than one sort, this shape will be wrong, but that
 // won't matter, since the shape will never be used elsewhere)
 GA(x,t,n*api,AR(w),AS(w)); void * RESTRICT xv=voidAV(x);  /* work area for msmerge() */
 DO(m,   // sort each cell
  void *sortres=(*sortfunc)(cmpfunc,cpi,n,bpi,(void*)zv,(void*)xv,wv);
  if(m==1){
   // If there is only one cell, it may be either *zv or *xv, but either way it should be the result
   R (sortres==zv)?z:x;
  }else{
   // If there is more than one cell, we have to make sure all the data migrates to *zv, if it's not there already
   if(sortres!=zv)MCL(zv,sortres,bps);
  }
  wv=(void*)((C*)wv+bps); zv=(void*)((C*)zv+bps);
 );
 R z;  // multiple cells - return original z
}    /* w grade"r w for direct types, by moving the data without pointers */


#define SF(f)         A f(J jt,I m,I n,A w)

/* m - # cells (# individual grades to do) */
/* c - # atoms in a cell                   */
/* n - length of sort axis                 */
/* w - array to be graded                  */

static SF(jtsortb){A z;B up,*u,*v;I i,s;
 GA(z,AT(w),AN(w),AR(w),AS(w)); v=BAV(z);
 up=(UI)jt->workareas.compare.complt>>(BW-1);  u=BAV(w);
 for(i=0;i<m;++i){
  s=bsum(n,u);
  if(up){memset(v,C0,n-s); memset(v+n-s,C1,s  );}
  else  {memset(v,C1,s  ); memset(v+s,  C0,n-s);}
  u+=n; v+=n;
 }
 R z;
}    /* w grade"1 w on boolean */

static SF(jtsortb2){A z;B up;I i,ii,j,p,yv[4];US*v,*wv,x,zz[4];
 GA(z,AT(w),AN(w),AR(w),AS(w)); v=USAV(z);
 wv=USAV(w); p=4; up=(UI)jt->workareas.compare.complt>>(BW-1);
 DO(p, yv[i]=0;); 
 zz[0]=BS00; zz[1]=BS01; zz[2]=BS10; zz[3]=BS11;
 for(i=0;i<m;++i){
  DO(n, IND2(*wv++); ++yv[ii];);
  if(up){j=0;   DO(p, x=zz[j]; DO(yv[j], *v++=x;); yv[j]=0; ++j;);}
  else  {j=p-1; DO(p, x=zz[j]; DO(yv[j], *v++=x;); yv[j]=0; --j;);}
 }
 R z;
}    /* w grade"r w on 2-byte boolean items */

static SF(jtsortb4){A z;B up;I i,ii,j,p,yv[16];UINT*v,*wv,x,zz[16];
 GA(z,AT(w),AN(w),AR(w),AS(w)); v=(UINT*)AV(z);
 wv=(UINT*)AV(w); p=16; up=(UI)jt->workareas.compare.complt>>(BW-1);
 DO(p, yv[i]=0;); 
 zz[ 0]=B0000; zz[ 1]=B0001; zz[ 2]=B0010; zz[ 3]=B0011;
 zz[ 4]=B0100; zz[ 5]=B0101; zz[ 6]=B0110; zz[ 7]=B0111;
 zz[ 8]=B1000; zz[ 9]=B1001; zz[10]=B1010; zz[11]=B1011;
 zz[12]=B1100; zz[13]=B1101; zz[14]=B1110; zz[15]=B1111;
 for(i=0;i<m;++i){
  DO(n, IND4(*wv++); ++yv[ii];);
  if(up){j=0;   DO(p, x=zz[j]; DO(yv[j], *v++=x;); yv[j]=0; ++j;);}
  else  {j=p-1; DO(p, x=zz[j]; DO(yv[j], *v++=x;); yv[j]=0; --j;);}
 }
 R z;
}    /* w grade"r w on 4-byte boolean items */

static SF(jtsortc){A z;B up;I i,p,yv[256];UC j,*wv,*v;
 GA(z,AT(w),AN(w),AR(w),AS(w)); v=UAV(z);
 wv=UAV(w); p=LIT&AT(w)?256:2; up=(UI)jt->workareas.compare.complt>>(BW-1);
 DO(p, yv[i]=0;);
 for(i=0;i<m;++i){
  DO(n, ++yv[*wv++];);
  if(up){j=0;         DO(p, DO(yv[j], *v++=j;); yv[j]=0; ++j;);}
  else  {j=(UC)(p-1); DO(p, DO(yv[j], *v++=j;); yv[j]=0; --j;);}
 }
 R z;
}    /* w grade"1 w on boolean or character */

static SF(jtsortc2){A y,z;B up;I i,p,*yv;US j,k,*wv,*v;
 GA(z,AT(w),AN(w),AR(w),AS(w)); v=USAV(z);
 wv=USAV(w); p=65536; up=(UI)jt->workareas.compare.complt>>(BW-1);
 GATV(y,INT,p,1,0); yv=AV(y);
 DO(p, yv[i]=0;);
 for(i=0;i<m;++i){
  DO(n, ++yv[*wv++];);
  if(C2T&AT(w)||!liln){
   if(up){j=0;         DO(p,                DO(yv[j], *v++=j;); yv[j]=0;           ++j;);}
   else  {j=(US)(p-1); DO(p,                DO(yv[j], *v++=j;); yv[j]=0;           --j;);}
  }else{
   if(up){k=0;         DO(256, j=k; DO(256, DO(yv[j], *v++=j;); yv[j]=0; j+=256;); ++k;);}
   else  {k=(US)(p-1); DO(256, j=k; DO(256, DO(yv[j], *v++=j;); yv[j]=0; j-=256;); --k;);}
 }}
 R z;
}    /* w grade"1 w on 2-byte character or unicode items */

static SF(jtsorti1);

static SF(jtsorti){A y,z;I i;UI4 *yv;I j,s,*wv,*zv;
 wv=AV(w);
 // figure out whether we should do small-range processing.  Comments in vg.c
 CR rng = condrange(wv,AN(w),IMAX,IMIN,n<<(n>(L2CACHESIZE>>LGSZI)?2:4));
 // smallrange always wins if applicable; otherwise use radix up to 1300 items, merge thereafter
 if(!rng.range)R n>1300?sorti1(m,n,w):jtsortdirect(jt,m,1,n,w);  // TUNE
 // allocate area for the data, and result area
 GATV(y,C4T,rng.range,1,0); yv=C4AV(y)-rng.min;  // yv->totals area
 GA(z,AT(w),AN(w),AR(w),AS(w)); zv=AV(z);
 // clear all totals to 0, then bias address of area so the data fits
 for(i=0;i<m;++i){  // for each list...
  memset(yv+rng.min,C0,rng.range*sizeof(UI4)); 
  DO(n, ++yv[*wv++];);  // increment total for each input atom
  // run through the totals, copying in the requisite # repetitions of each value
  // We have to disguise the loop to prevent VS from producing a REP STOS, which we don't want because the loop is usually short
  I incr = -jt->workareas.compare.complt; I zincr = (incr&1)*sizeof(*zv); j=rng.min+((incr>>(BW-1))&(rng.range-1));  // jt>complt is 1 or -1
  DQ(rng.range, s=yv[j]; DQ(s, *zv=j; zv=(I*)((C*)zv+zincr);) j+=incr;)  // Don't zv+=zincr, because VS doesn't pull the *8 out
//  if((UI)jt->workareas.compare.complt>>(BW-1)){ j=rng.min; DQ(rng.range, s=(I)yv[j]; DO(s, *zv++=j;); ++j;);}  // generates rep stos, which is slow.  should fix
//  else{j=rng.min+rng.range; DQ(rng.range, --j; s=(I)yv[j]; DO(s, *zv++=j  ;););}
 }
 R z;
}    /* w grade"1 w on small-range integers */


// We are known to have 1 atom per item
static SF(jtsorti1){A x,y,z;I*wv;I i,*xv,*zv;void *yv;
 GA(z,AT(w),AN(w),AR(w),AS(w)); zv=AV(z);
 wv=AV(w);
 // choose bucket table size & function; allocate the bucket area
 I (*grcol)(I,I,void*,I,I*,I*,const I,US*,I);  // prototype for either size of buffer
 { I use4 = n>65535; grcol=use4?(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol4:(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol2; GATV(y,INT,((65536*sizeof(US))>>LGSZI)<<use4,1,0); yv=AV(y);}
 GATV(x,INT,n,1,0); xv=AV(x);
 for(i=0;i<m;++i){I colflags;
  colflags=grcol(65536,0L,yv,n,wv,xv,sizeof(I)/sizeof(US),    INTLSBWDX+(US*)wv,4+1-jt->workareas.compare.complt);  // 'sort', and move 'up' to bit 1
#if SY_64
  colflags=grcol(65536,0L,yv,n,xv,zv,sizeof(I)/sizeof(US),1*WDINC+INTLSBWDX+(US*)xv,colflags);
  colflags=grcol(65536,0L,yv,n,zv,xv,sizeof(I)/sizeof(US),2*WDINC+INTLSBWDX+(US*)zv,colflags);
#endif
  grcol(65536,0L,yv,n,xv,zv,sizeof(I)/sizeof(US),INTMSBWDX+(US*)xv,colflags|1);
  wv+=n; zv+=n;
 }
 R z;
}    /* w grade"r w on large-range integers */

static SF(jtsortu1);

// see jtsorti above
static SF(jtsortu){A y,z;I i;UI4 *yv;C4 j,s,*wv,*zv;
 wv=C4AV(w);
 I maxrange; CR rng;
 if(0<(maxrange=16*(n-32))){rng = condrange4(wv,AN(w),-1,0,maxrange);
 }else rng.range=0;
 if(!rng.range)R n>700?sortu1(m,n,w):jtsortdirect(jt,m,1,n,w);  // TUNE
 GATV(y,C4T,rng.range,1,0); yv=C4AV(y)-rng.min;
 GA(z,AT(w),AN(w),AR(w),AS(w)); zv=C4AV(z);
 for(i=0;i<m;++i){
  memset(yv+rng.min,C0,rng.range*sizeof(UI4)); 
  DO(n, ++yv[*wv++];);
  I incr = -jt->workareas.compare.complt; I zincr = (incr&1)*sizeof(*zv); j=(C4)(rng.min+((incr>>(BW-1))&(rng.range-1)));
  DQ(rng.range, s=yv[j]; DQ(s, *zv=j; zv=(C4*)((C*)zv+zincr);) j+=(C4)incr;)
 }
 R z;
}    /* w grade"1 w on small-range literal4 */

// We are known to have 1 atom per item
static SF(jtsortu1){A x,y,z;C4 *xu,*wv,*zu;I i;void *yv;
 GA(z,AT(w),AN(w),AR(w),AS(w));
 wv=C4AV(w); zu=C4AV(z);
 // choose bucket table size & function; allocate the bucket area
 I (*grcol)(I,I,void*,I,I*,I*,const I,US*,I);  // prototype for either size of buffer
 { I use4 = n>65535; grcol=use4?(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol4:(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol2; GATV(y,INT,((65536*sizeof(US))>>LGSZI)<<use4,1,0); yv=AV(y);}
 GATV(x,C4T,n,1,0); xu=C4AV(x);
 for(i=0;i<m;++i){I colflags;
  colflags=grcol(65536, 0L, yv,n,(UI*)wv,(UI*)xu,sizeof(C4)/sizeof(US),INTLSBWDX+0*WDINC+(US*)wv,4+1-jt->workareas.compare.complt);  // 'sort' + 'up' moved to bit 1
  grcol(65536, 0L, yv,n,(UI*)xu, (UI*)zu,sizeof(C4)/sizeof(US),INTLSBWDX+1*WDINC+(US*)xu ,colflags);
  wv+=n; zu+=n;
 }
 R z;
}    /* w grade"r w on large-range literal4 */

#if 0
#define CXCHG2D(a,b) {t=a; a=(t<=b)?a:b; b=(t<=b)?b:t;}
static void sortdq1short(D *v, I n){D a,b,c,d,e,t;
 switch(n){
 case 2:  // happens only if original input is 2 long
  a=v[0]; b=v[1]; CXCHG2D(a,b);
  goto exitb;
 case 3:
  a=v[0]; b=v[1]; c=v[2]; CXCHG2D(b,c); CXCHG2D(a,b); CXCHG2D(b,c);
  goto exitc;
 case 4:
  a=v[0]; b=v[1]; c=v[2]; d=v[3]; CXCHG2D(a,b); CXCHG2D(c,d); CXCHG2D(a,c); CXCHG2D(b,d); CXCHG2D(b,c);
  goto exitd;
 case 5:
  a=v[0]; b=v[1]; c=v[2]; d=v[3]; e=v[4]; CXCHG2D(b,c); CXCHG2D(d,e); CXCHG2D(b,d); CXCHG2D(a,c); CXCHG2D(a,d); CXCHG2D(c,e); CXCHG2D(a,b); CXCHG2D(c,d); CXCHG2D(b,c);
  v[4]=e;
exitd:
  v[3]=d;
exitc:
  v[2]=c;
exitb:
  v[1]=b; v[0]=a;
 case 0: case 1:;
 }
 R;
}
#else
static UC orderfromcomp3[8] = {
36,24,0,9,33,0,18,6
};

static UC orderfromcomp4[64] = {
228,180,0,120,216,0,156,108,0,0,0,57,0,0,0,45,0,0,0,0,201,0,141,0,0,0,0,0,0,0,78,30,
225,177,0,0,0,0,0,0,0,114,0,54,0,0,0,0,210,0,0,0,198,0,0,0,147,99,0,39,135,0,75,27
};

static US orderfromcomp5[1024] = {
18056,14472,0,10440,17608,0,13576,9992,0,0,0,6352,0,0,0,5904,0,0,0,0,17104,0,13072,0,0,0,0,0,0,0,8984,5400,
18000,14416,0,0,0,0,0,0,0,10328,0,6296,0,0,0,0,17496,0,0,0,17048,0,0,0,13408,9824,0,5792,12960,0,8928,5344,
0,0,0,0,0,0,0,0,0,0,0,2257,0,0,0,1809,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1305,
0,0,0,0,0,0,0,0,0,0,0,2201,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1697,0,0,0,1249,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16593,0,12561,0,0,0,0,0,0,0,8473,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16537,0,0,0,0,0,0,0,12449,0,8417,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4378,794,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4322,738,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
17937,14353,0,0,0,0,0,0,0,10265,0,0,0,0,0,0,17433,0,0,0,0,0,0,0,13345,9761,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,6170,0,2138,0,0,0,0,0,0,0,0,0,0,0,0,0,5666,0,1634,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16922,0,0,0,16474,0,0,0,12834,0,0,0,12386,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8739,5155,0,1123,8291,0,4259,675,
18049,14465,0,10433,17601,0,13569,9985,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,6338,0,0,0,5890,0,0,0,2250,0,0,0,1802,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,17090,0,13058,0,0,0,0,0,0,0,0,0,0,0,0,0,16586,0,12554,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,8963,5379,0,0,0,0,0,0,0,1291,0,0,0,0,0,0,8459,0,0,0,0,0,0,0,4371,787,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
17986,14402,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
17930,14346,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,10307,0,6275,0,0,0,0,0,0,0,2187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,10251,0,0,0,0,0,0,0,6163,0,2131,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
17475,0,0,0,17027,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16523,0,0,0,0,0,0,0,0,0,0,0,
17419,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16915,0,0,0,16467,0,0,0,0,0,0,0,0,0,0,0,
13380,9796,0,5764,12932,0,8900,5316,0,0,0,1676,0,0,0,1228,0,0,0,0,12428,0,8396,0,0,0,0,0,0,0,4308,724,
13324,9740,0,0,0,0,0,0,0,5652,0,1620,0,0,0,0,12820,0,0,0,12372,0,0,0,8732,5148,0,1116,8284,0,4252,668
};

static void sortdq1short(D *v, I n){D a,b,c,d,e;UI f;
 switch(n){
 case 2:
  a=v[0]; b=v[1]; f=a>b; v[f]=a; v[f^1]=b;
  break;
 case 3:
  a=v[0]; b=v[1]; c=v[2];
  f=orderfromcomp3[((a>b)<<2) + ((a>c)<<1) + (b>c)];
  v[f&3]=a; v[(f>>2)&3]=b; v[f>>4]=c;
  break;
 case 4:
  a=v[0]; b=v[1]; c=v[2]; d=v[3];
  f=orderfromcomp4[((a>b)<<5) + ((a>c)<<4) + ((a>d)<<3) + ((b>c)<<2) + ((b>d)<<1) + (c>d)];
  v[f&3]=a; v[(f>>2)&3]=b; v[(f>>4)&3]=c; v[f>>6]=d;
  break;
 case 5:
  // read em in
  a=v[0]; b=v[1]; c=v[2]; d=v[3]; e=v[4];
  // compare each value against each other value, and assemble the bits into f in order of 0-4
  // Then read the ordering value from the table.  This read may miss in cache but we don't care too much, since there is no dependency on the result of the read
  f=orderfromcomp5[((a>b)<<9) + ((a>c)<<8) + ((a>d)<<7) + ((a>e)<<6) + ((b>c)<<5) + ((b>d)<<4) + ((b>e)<<3) + ((c>d)<<2) + ((c>e)<<1) + (d>e)];
  // Store each value in its correct spot  Code to minimize the # instructions held waiting for the fetch to complete
  v[f&7]=a; v[(f>>3)&7]=b; v[(f>>6)&7]=c; v[(f>>9)&7]=d; v[f>>12]=e;
  break;
 case 0: case 1:;
 }
 R;
}
#endif

// sort a single real list using quicksort without misprediction, inplace
// v->first value, n=# values
static void sortdq1(D *v, I n){
 // loop till tail-recursion finished
 I l=0, r=n-1; I stackp=0; I stack[32][2];
 while(1){
  // the batch is short, sort it by fixed comparison; then pick up the next batch
  while(r-l<=4){
    sortdq1short(v+l,r-l+1); // Because our partitioning here is fast, stay in qsort until the batch is very small
    if(--stackp<0)R;  // back up stack; if we're finishing the last call, we're through
    l=stack[stackp][0]; r=stack[stackp][1];  // resume the next 
  }
  // long batch: partition it and recur
  // choose the pivot as the median of first/last/middle, and exchange it with the end of the array
  D pivot;
  {I pivotcomp = 0xc6>>(4*(v[l]>v[(l+r)>>1]) + 2*(v[l]>v[r]) + (v[(l+r)>>1]>v[r]));  // l>m, l>r, m>r 000 lmr  001 lrm  010 -  011 rlm  100 mlr  101 -  110 mrl  111 rml
      // encode l as 00 r as 11 m as 01/10, the sequence is mr-ll-rm 01 11 10 00 00 01 11 10  0 1 1 0 0 0 1 1 0 (LE) -> 011000110  0xc6
   I pivotx=((pivotcomp&1?r:l)+(pivotcomp&2?r:l))>>1; pivot=v[pivotx]; v[pivotx]=v[r];  // pick the median pivot, swap it (notionally) with the last 
  }
  // initialize comparison stacks to empty.  Bits go into the MSB of the comparison stack and are shifted down.  The in pointer is the index of the next value to be compared going in
  // lsb gives the bit # of the lowest valid value in the stack
  UI cstk0=0, cstk1=0; I cstklsb0=BW, cstklsb1=BW;
  // initialize comparison input pointers (input->next value to compare)
  I in0=l; I in1=r-1;  // the last position is notionally the swapped-out pivot
  I xchgx0=0; I xchgx1=r;  // remember the last successful exchange, so that we ignore any step past it
  // loop till partitioning completed
  // At this point we know that if cstk? is 0, cstklsb? has been set to BW.  Also, at least one cstk? is 0
  while(1){
   // add to each comparison stack.  Use the fixed amount (to avoid misbranches), but not much past halfway between comparison output pointers,
   // and never past the opposite output pointer.  This keeps us in the batch and avoids looking for swaps that have already been passed up.  The next output pointer is in0-(BW-cstklsb0)  or  in1+(BW-cstklsb1)
   do{I midpoint=(in0+cstklsb0+in1-cstklsb1)>>1; UI newstkbit=1LL<<(BW-1);
    // If we were unable to add to either of the empty stacks, we are done.  The empty stack(s) will point past the end of the block, but the length will be 0
    I ncmp0=cstklsb0; ncmp0=ncmp0>in1+(BW-cstklsb1)-in0+1?in1+(BW-cstklsb1)-in0+1:ncmp0; ncmp0=ncmp0>midpoint-in0+8?midpoint-in0+8:ncmp0; ncmp0=ncmp0>((BW*2)/3)?(BW*2)/3:ncmp0; ncmp0=ncmp0<0?0:ncmp0; if(!(cstk0|ncmp0))goto partdone;
    I ncmp1=cstklsb1; ncmp1=ncmp1>in1-(in0-(BW-cstklsb0))+1?in1-(in0-(BW-cstklsb0))+1:ncmp1; ncmp1=ncmp1>in1-midpoint+8?in1-midpoint+8:ncmp1; ncmp1=ncmp1>((BW*2)/3)?(BW*2)/3:ncmp1; ncmp1=ncmp1<0?0:ncmp1; if(!(cstk1|ncmp1))goto partdone;
    // look for swappable values and stack them.  At end, note the position of the first swappable.  If there are none, advance LSB to end of word to leave maximum space
    DQ(ncmp0, UI newbit=v[in0]>pivot?newstkbit:0; cstk0=(cstk0>>1)+newbit; ++in0;) cstklsb0=CTTZI(cstk0);cstklsb0=(cstk0==0)?BW:cstklsb0;
    DQ(ncmp1, UI newbit=v[in1]<pivot?newstkbit:0; cstk1=(cstk1>>1)+newbit; --in1;) cstklsb1=CTTZI(cstk1);cstklsb1=(cstk1==0)?BW:cstklsb1;
   }while((cstk0==0)||(cstk1==0));
// obsolete    if((cstklsb0|cstklsb1)&BW)goto partdone;
   // process the comparison stack until one of the stacks is empty.  Perform exchanges
   while(1){
    if(in0-(BW-cstklsb0)>=in1+(BW-cstklsb1))goto partdone;  // if pointers have crossed, we're through
    xchgx0=in0-(BW-cstklsb0); xchgx1=in1+(BW-cstklsb1);  // remember the successful exchange, so that we don't 
    D temp=v[xchgx0]; v[xchgx0]=v[xchgx1]; v[xchgx1]=temp;
// obsolete     cstk0&=cstk0-1; cstk1&=cstk1-1; // remove the bits that were processed
// obsolete     cstklsb0=CTTZI(cstk0); cstklsb1=CTTZI(cstk1);   // advance the pointer to the next swap; garbage if zero, but that's corrected immediately below
    // Remove the bit we processed, and update the lsb for the next swap.  If there is no swap here, advance LSB to end of word to free up stack space
    if(cstk0&=cstk0-1)cstklsb0=CTTZI(cstk0); else{cstklsb0=BW; break;}  // We hope the processor predicts these branches to fail always, so that we will get one misbranch to exit the loop
    if(cstk1&=cstk1-1)cstklsb1=CTTZI(cstk1); else{cstklsb1=BW; goto testcstk1;}
   }
   // Here when cstk0 ended: we have not updated cstk1 yet
   cstk1&=cstk1-1; cstklsb1=CTTZI(cstk1); cstklsb1=cstk1==0?BW:cstklsb1;
testcstk1:
  }
  // At this point we know that indexes BELOW= in0+BW-cstklsb0 are <= the pivot, and those ABOVE= in1-(BW-cstklsb1)  are >= the pivot.  In other words, stklsb gives the location of the exchange that
  // was not made because those values were already in position.  Recursion will be to points including those values.  Any points between those values (exclusive) must be equal to the pivot
partdone:
  // back up the pointers to the last successful exchange if any, since that exchange is known to have stored a value for that side
  xchgx0=MAX(xchgx0,in1+(BW-cstklsb1)); xchgx1=MIN(xchgx1,in0-(BW-cstklsb0));  // Now xchgx0 is the end of the left side, and xchgx1 of the right
  // exchange the end of the array with the start of the right side
  v[r]=v[xchgx1]; v[xchgx1]=pivot;

  // push stack for the larger partition; modify batch pointers for the smaller
  // recursions are l..in1-(BW-cstklsb1) and in0-(BW-cstklsb0)+1..r    the +1 to step over the pivot from this pass
  if(xchgx0-l > r-xchgx1){stack[stackp][0]=l; stack[stackp][1]=xchgx0; l=xchgx1+1;
  }else{stack[stackp][0]=xchgx1+1; stack[stackp][1]=r; r=xchgx0;
  }
  ++stackp;
 }
}


static SF(jtsortdq){  // m=#sorts, n=#items in each sort, w is block
 A z; RZ(z=ca(w));  // until we inplace, we have to copy the input
 D *zv=DAV(z); DQ(m, sortdq1(zv,n); zv+=n;)  // sort each list
 RETF(z);
}

// We are known to have 1 atom per item
static SF(jtsortd){A x,y,z;B b;D*g,*h,*xu,*wv,*zu;I i,nneg;void *yv;
 // Radix sort almost always wins, presumably because filling the radix table is so fast
 R jtsortdq(jt,m,n,w);  // scaf
 if(n&2)R jtsortdirect(jt,m,1,n,w);  // TUNE
 GA(z,AT(w),AN(w),AR(w),AS(w));
 wv=DAV(w); zu=DAV(z);
 // choose bucket table size & function; allocate the bucket area
 I (*grcol)(I,I,void*,I,I*,I*,const I,US*,I);  // prototype for either size of buffer
 { I use4 = n>65535; grcol=use4?(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol4:(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol2; GATV(y,INT,((65536*sizeof(US))>>LGSZI)<<use4,1,0); yv=AV(y);}
 GATV(x,FL, n,1,0); xu=DAV(x);
 for(i=0;i<m;++i){I colflags;  // for each cell to be sorted...
  g=wv; nneg=0; DO(n, nneg+=(0>*g++);); b=0<nneg&&nneg<n;
  g=b?xu:zu; h=b?zu:xu;  // select correct alignment to end with result in zv
  colflags=grcol(65536,    0L,      yv,n,(I*)wv,(I*)h,sizeof(D)/sizeof(US),FPLSBWDX+0*WDINC+(US*)wv,4+(((I )(nneg==n)<<1)^(1-jt->workareas.compare.complt))); // 'sort', plus 'up' in bit 1, but reversed if all neg
  colflags=grcol(65536,    0L,      yv,n,(I*)h, (I*)g,sizeof(D)/sizeof(US),FPLSBWDX+1*WDINC+(US*)h ,colflags);
  colflags=grcol(65536,    0L,      yv,n,(I*)g, (I*)h,sizeof(D)/sizeof(US),FPLSBWDX+2*WDINC+(US*)g ,colflags);
  grcol(32768<<b,(nneg==n)<<15,yv,n,(I*)h, (I*)g,sizeof(D)/sizeof(US),FPLSBWDX+3*WDINC+(US*)h ,colflags);
  if(b){  // if there was not a mix of neg & nonneg, the result is ready now
   g=zu;
   if(colflags&2){h=n+xu; DO(nneg,   *g++=*--h;); h=  xu; DO(n-nneg, *g++=*h++;);}  // up: reverse neg, foll by nonneg
   else  {h=nneg+xu; DO(n-nneg, *g++=*h++;); h=nneg+xu; DO(nneg,   *g++=*--h;);}  // down: nonneg foll by reverse neg
  }
  wv+=n; zu+=n;
 }
 R z;
}    /* w grade"1 w on real w */


// x /:"r y, not sparse
F2(jtgr2){PROLOG(0076);A z=0;I acr,api,d,f,m,n,*s,t,wcr; 
 RZ(a&&w);
 // ?cr= rank of the cells being sorted; t= type of w
 acr=jt->ranks>>RANKTX; acr=AR(a)<acr?AR(a):acr; 
 wcr=(RANKT)jt->ranks; wcr=AR(w)<wcr?AR(w):wcr; t=AT(w);
 // Handle special reflexive cases, when the arguments are identical and the cells are also.  Only if cells have rank>0 and have atoms
 if(a==w&&acr==wcr&&wcr>0&&AN(a)&&t&(B01+LIT+C2T+C4T+INT+FL+CMPX)){
  // f = length of frame of w; s->shape of w; m=#cells; n=#items in each cell;
  // d = #bytes in an item of a cell of w
  f=AR(w)-wcr; s=AS(w); PROD(m,f,s); n=(AR(w))?s[f]:1; PROD(api,wcr-1,1+f+s);
  d=api<<bplg(t);
   // There are special types supported, but for very short sorts we should just skip the checking and go do a sort-in-place.
   // Test that threshold here
  if(n>5){   //  TUNE
   if(t&(C4T+INT+FL)){
    // If this datatype supports smallrange or radix sorting, go try that
    if(1==api)RZ(z=(t&INT?jtsorti:t&FL?jtsortd:jtsortu)(jt,m,n,w))   // Lists of INT/FL/C4T
   else if(d==2){
    // 2-byte types, which must be B01/LIT/C2T.  Use special code, unless strings too short
    if(t&B01)             RZ(z=sortb2(m,n,w))  // Booleans with cell-items 2 bytes long
    if(t&LIT+C2T&&n>4600)RZ(z=sortc2(m,n,w))  // long character strings with cell-items 2 bytes long   TUNE
   }else if(d<2)RZ(z=(t&B01&&(m==1||0==(n&(SZI-1)))?jtsortb:jtsortc)(jt,m,n,w))  // Lists of B01/LIT
   }else if(d==4&&t&B01) RZ(z=sortb4(m,n,w))  // Booleans with cell-items 4 bytes long
  }
   // for direct types, we have the choice of direct/indirect.  For indirect, we do grade followed by from to apply the grading permutation.
   // for direct, we move the data around until we get the final sort.  Direct is better for short items, for which the copy is cheap;
   // and the cutoff rises with the length of the sort, as the indirect sort flags when the items themselves start to fall out of cache.
   // Empirically (Ivy Bridge) we find that the length of the item above which indirect is better depends on sortlength:
   // for 1e6 items - 80 bytes; 1e5 - 64 bytes; 1e4 - 40 bytes - 1e3 - 48 bytes
   // We roughly approximate this curve
  if(!z){  // not a special case
   UI4 lgn; CTLZI(n,lgn);
   if(d<40||(UI4)d<(lgn<<4))RZ(z=jtsortdirect(jt,m,api,n,w))  //  TUNE
  }
 }
 // If not a supported reflexive case, grade w and then select those values from a.  jt->ranks is still set
 if(!z)RZ(z=irs2(gr1(w),a,0L,1L,acr,jtfrom));
 EPILOG(z);
}    /* a grade"r w main control for dense w */
