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
 I bpi=api*bp(t);  // bytes per item of a sort
 I bps=bpi*n;  // bytes per sort
 void * RESTRICT wv=voidAV(w); void * RESTRICT zv=voidAV(z);
 CMP cmpfunc=sortroutines[CTTZ(t)][jt->compgt==1].comproutine;
 void *(*sortfunc)() = sortroutines[CTTZ(t)][jt->compgt==1].sortfunc;
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
 up=1==jt->compgt;  u=BAV(w);
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
 wv=USAV(w); p=4; up=1==jt->compgt;
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
 wv=(UINT*)AV(w); p=16; up=1==jt->compgt;
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
 wv=UAV(w); p=LIT&AT(w)?256:2; up=1==jt->compgt;
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
 wv=USAV(w); p=65536; up=1==jt->compgt;
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
  I incr = jt->compgt; I zincr = (incr&1)*sizeof(*zv); j=rng.min+((incr>>(BW-1))&(rng.range-1));  // jt>compgt is 1 or -1
  DQ(rng.range, s=yv[j]; DQ(s, *zv=j; zv=(I*)((C*)zv+zincr);) j+=incr;)  // Don't zv+=zincr, because VS doesn't pull the *8 out
//  if(jt->compgt==1){ j=rng.min; DQ(rng.range, s=(I)yv[j]; DO(s, *zv++=j;); ++j;);}  // generates rep stos, which is slow.  should fix
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
  colflags=grcol(65536,0L,yv,n,wv,xv,sizeof(I)/sizeof(US),    INTLSBWDX+(US*)wv,4+(jt->compgt+1));  // 'sort', and move 'up' to bit 1
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
  I incr = jt->compgt; I zincr = (incr&1)*sizeof(*zv); j=(C4)(rng.min+((incr>>(BW-1))&(rng.range-1)));
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
  colflags=grcol(65536, 0L, yv,n,(UI*)wv,(UI*)xu,sizeof(C4)/sizeof(US),INTLSBWDX+0*WDINC+(US*)wv,4+(jt->compgt+1));  // 'sort' + 'up' moved to bit 1
  grcol(65536, 0L, yv,n,(UI*)xu, (UI*)zu,sizeof(C4)/sizeof(US),INTLSBWDX+1*WDINC+(US*)xu ,colflags);
  wv+=n; zu+=n;
 }
 R z;
}    /* w grade"r w on large-range literal4 */

// We are known to have 1 atom per item
static SF(jtsortd){A x,y,z;B b;D*g,*h,*xu,*wv,*zu;I i,nneg;void *yv;
 // Radix sort almost always wins, presumably because filling the radix table is so fast
 if(n<50)jtsortdirect(jt,m,1,n,w);  // TUNE
 GA(z,AT(w),AN(w),AR(w),AS(w));
 wv=DAV(w); zu=DAV(z);
 // choose bucket table size & function; allocate the bucket area
 I (*grcol)(I,I,void*,I,I*,I*,const I,US*,I);  // prototype for either size of buffer
 { I use4 = n>65535; grcol=use4?(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol4:(I (*)(I,I,void*,I,I*,I*,const I,US*,I))grcol2; GATV(y,INT,((65536*sizeof(US))>>LGSZI)<<use4,1,0); yv=AV(y);}
 GATV(x,FL, n,1,0); xu=DAV(x);
 for(i=0;i<m;++i){I colflags;
  g=wv; nneg=0; DO(n, nneg+=(0>*g++);); b=0<nneg&&nneg<n;
  g=b?xu:zu; h=b?zu:xu;  // select correct alignment to end with result in zv
  colflags=grcol(65536,    0L,      yv,n,(I*)wv,(I*)h,sizeof(D)/sizeof(US),FPLSBWDX+0*WDINC+(US*)wv,4+(((nneg==n)<<1)^(jt->compgt+1))); // 'sort', plus 'up' in bit 1, but reversed if all neg
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


// x /:"r y
F2(jtgr2){PROLOG(0076);A z=0;I acr,api,d,f,m,n,*s,t,wcr; 
 RZ(a&&w);
 // ?cr= rank of the cells being sorted; t= type of w
 acr=jt->rank?jt->rank[0]:AR(a); 
 wcr=jt->rank?jt->rank[1]:AR(w); t=AT(w);
 // Handle special reflexive cases, when the arguments are identical and the cells are also.  Only if cells have rank>0 and have atoms
 if(a==w&&acr==wcr&&wcr>0&&AN(a)&&t&(B01+LIT+C2T+C4T+INT+FL+CMPX)){
  // f = length of frame of w; s->shape of w; m=#cells; n=#items in each cell;
  // d = #bytes in an item of a cell of w
  f=AR(w)-wcr; s=AS(w); PROD(m,f,s); n=(AR(w))?s[f]:1; PROD(api,wcr-1,1+f+s);
  d=api*bp(t);
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
 // If not a supported reflexive case, grade w and then select those values from a
 if(!z)RZ(z=irs2(gr1(w),a,0L,1L,acr,jtfrom));
 EPILOG(z);
}    /* a grade"r w main control for dense w */
