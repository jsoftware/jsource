/* Copyright 1990-2014, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Reduce (Insert), Outer Product, and Fold                              */

#include "j.h"
#include "vasm.h"
#include "ve.h"
#include "vcomp.h"
#include "ar.h"

static DF1(jtreduce);


#define PARITY2         u=(UC*)&s; b=0; b^=*u++; b^=*u++;
#define PARITY4         u=(UC*)&s; b=0; b^=*u++; b^=*u++; b^=*u++; b^=*u++; 
#define PARITY8         u=(UC*)&s; b=0; b^=*u++; b^=*u++; b^=*u++; b^=*u++; b^=*u++; b^=*u++; b^=*u++; b^=*u++;

#if SY_64
#define PARITYW         PARITY8
#else
#define PARITYW         PARITY4
#endif  

#if SY_ALIGN
#define VDONE(T,PAR)  \
 {I q=n/sizeof(T);T s,*y=(T*)x; DQ(m, s=0; DQ(q, s^=*y++;); PAR; *z++=b==pc;);}

static void vdone(I m,I n,B*x,B*z,B pc){B b,*u;
 if(1==m){UI s,*xi;
  s=0; b=0;
  xi=(I*)x; DQ(n>>LGSZI, s^=*xi++;); 
  u=(B*)xi; DQ(n&(SZI-1), b^=*u++;);
  u=(B*)&s; DQ(SZI,   b^=*u++;);
  *z=b==pc;
 }else if(0==(n&(sizeof(UI  )-1)))VDONE(UI,  PARITYW)
 else  if(0==(n&(sizeof(UINT)-1)))VDONE(UINT,PARITY4)
 else  if(0==(n&(sizeof(US  )-1)))VDONE(US,  PARITY2)
 else  DQ(m, b=0; DQ(n, b^=*x++;); *z++=b==pc;);
}
#else
static void vdone(I m,I n,B*x,B*z,B pc){B b;I q,r;UC*u;UI s,*y;
 q=n>>LGSZI; r=n&(SZI-1); y=(UI*)x;
 switch((r?2:0)+pc){
  case 0: DQ(m, s=0; DQ(q, s^=*y++;); PARITYW;                            *z++=!b;); break;
  case 1: DQ(m, s=0; DQ(q, s^=*y++;); PARITYW;                            *z++= b;); break;
  case 2: DQ(m, s=0; DQ(q, s^=*y++;); PARITYW; u=(UC*)y; DQ(r, b^=*u++;); *z++=!b; x+=n; y=(UI*)x;); break;
  case 3: DQ(m, s=0; DQ(q, s^=*y++;); PARITYW; u=(UC*)y; DQ(r, b^=*u++;); *z++= b; x+=n; y=(UI*)x;); break;
}}
#endif

#define RBFXLOOP(T,pfx)  \
 {T* RESTRICT xx=(T*)x,* RESTRICT yy,*z0,* RESTRICT zz=(T*)z;   \
  q=d/sizeof(T);                  \
  for(j=0;j<m;++j){               \
   yy=xx; xx-=q; z0=zz; DQ(q, --xx; --yy; --zz; *zz=pfx(*xx,*yy););    \
   DQ(n-2,       zz=z0; DQ(q, --xx;       --zz; *zz=pfx(*xx,*zz);););  \
 }}  /* non-commutative */

#define RCFXLOOP(T,pfx)  \
 {T* RESTRICT xx=(T*)x,* RESTRICT yy,*z0,* RESTRICT zz=(T*)z;   \
  q=d/sizeof(T);                  \
  for(j=0;j<m;++j){               \
   yy=xx; xx+=q; z0=zz; DQ(q, *zz++=pfx(*yy,*xx); ++xx; ++yy;);    \
   DQ(n-2,       zz=z0; DQ(q, *zz++=pfx(*zz,*xx); ++xx;      ););  \
 }}  /* commutative */

#if SY_ALIGN
#define RBFXODDSIZE(pfx,bpfx)  RBFXLOOP(C,bpfx)
#define REDUCECFX              REDUCEBFX
#else
#define RBFXODDSIZE(pfx,bpfx)  \
 {B*zz;I r,t,*xi,*yi,*zi;                                                       \
  q=d>>LGSZI; r=d&(SZI-1); xi=(I*)x; zz=z;                                             \
  for(j=0;j<m;++j,zz-=d){                                                       \
   yi=xi; xi=(I*)((B*)xi-d); zi=(I*)zz;                                         \
   DQ(q, --xi; --yi; *--zi=pfx(*xi,*yi););                                      \
    xi=(I*)((B*)xi-r); yi=(I*)((B*)yi-r); t=pfx(*xi,*yi); MC((B*)zi-r,&t,r);    \
   DQ(n-2, zi=(I*)zz; DQ(q, --xi; --zi; *zi=pfx(*xi,*zi););                     \
    xi=(I*)((B*)xi-r); zi=(I*)((B*)zi-r); t=pfx(*xi,*zi); MC(    zi,  &t,r););  \
 }}  /* non-commutative */

#define RCFXODDSIZE(pfx,bpfx)  \
 {I r,t,*xi,*yi,*zi;                                                            \
  q=d>>LGSZI; r=d&(SZI-1);                                                             \
  for(j=0;j<m;++j,x+=d,z+=d){                                                   \
   yi=(I*)x; x+=d; xi=(I*)x; zi=(I*)z; DQ(q, *zi++=pfx(*yi,*xi); ++xi; ++yi;); t=pfx(*yi,*xi); MC(zi,&t,r);    \
   DQ(n-2,   x+=d; xi=(I*)x; zi=(I*)z; DQ(q, *zi++=pfx(*zi,*xi); ++xi;      ); t=pfx(*zi,*xi); MC(zi,&t,r););  \
 }}  /* commutative */

#define REDUCECFX(f,pfx,ipfx,spfx,bpfx,vdo)  \
 AHDRP(f,B,B){B*y=0;I j,q;                       \
  if(d==1){vdo; R;}                                \
  if(1==n)DQ(d, *z++=*x++;)                        \
  else if(0==d%sizeof(UI  ))RCFXLOOP(UI,   pfx)    \
  else if(0==d%sizeof(UINT))RCFXLOOP(UINT,ipfx)    \
  else if(0==d%sizeof(US  ))RCFXLOOP(US,  spfx)    \
  else                      RCFXODDSIZE(pfx,bpfx)  \
 }  /* commutative */

#endif

#define REDUCEBFX(f,pfx,ipfx,spfx,bpfx,vdo)  \
 AHDRP(f,B,B){B*y=0;I j,q;                       \
  if(d==1){vdo; R;}                                \
  x+=m*d*n; z+=m*d;                           \
  if(1==n)DQ(d, *--z=*--x;)                        \
  else if(0==d%sizeof(UI  ))RBFXLOOP(UI,   pfx)    \
  else if(0==d%sizeof(UINT))RBFXLOOP(UINT,ipfx)    \
  else if(0==d%sizeof(US  ))RBFXLOOP(US,  spfx)    \
  else                      RBFXODDSIZE(pfx,bpfx)  \
 }  /* non-commutative */

REDUCECFX(  eqinsB, EQ,  IEQ,  SEQ,  BEQ,  vdone(m,n,x,z,(B)(n&1)))
REDUCECFX(  neinsB, NE,  INE,  SNE,  BNE,  vdone(m,n,x,z,1       ))
REDUCECFX(  orinsB, OR,  IOR,  SOR,  BOR,  {I c=d*n; DQ(m, *z++=1&&memchr(x,C1,n);                         x+=c;)}) 
REDUCECFX( andinsB, AND, IAND, SAND, BAND, {I c=d*n; DQ(m, *z++=!  memchr(x,C0,n);                         x+=c;}))
REDUCEBFX(  ltinsB, LT,  ILT,  SLT,  BLT,  {I c=d*n; DQ(m, *z++= *(x+n-1)&&!memchr(x,C1,n-1)?1:0;          x+=c;)})
REDUCEBFX(  leinsB, LE,  ILE,  SLE,  BLE,  {I c=d*n; DQ(m, *z++=!*(x+n-1)&&!memchr(x,C0,n-1)?0:1;          x+=c;)})
REDUCEBFX(  gtinsB, GT,  IGT,  SGT,  BGT,  {I c=d*n; DQ(m, y=memchr(x,C0,n); *z++=1&&(y?1&(y-x):1&n);      x+=c;)})
REDUCEBFX(  geinsB, GE,  IGE,  SGE,  BGE,  {I c=d*n; DQ(m, y=memchr(x,C1,n); *z++=!  (y?1&(y-x):1&n);      x+=c;)})
REDUCEBFX( norinsB, NOR, INOR, SNOR, BNOR, {I c=d*n; DQ(m, y=memchr(x,C1,n); d=y?y-x:n; *z++=(1&d)==d<n-1; x+=c;)})
REDUCEBFX(nandinsB, NAND,INAND,SNAND,BNAND,{I c=d*n; DQ(m, y=memchr(x,C0,n); d=y?y-x:n; *z++=(1&d)!=d<n-1; x+=c;)})


#if SY_ALIGN
REDUCEPFX(plusinsB,I,B,PLUS, plusBB, plusBI)
#else
AHDRR(plusinsB,I,B){I dw,i,p,q,r,r1,s;UC*tu;UI*v;
 if(d==1&n<SZI)DQ(m, s=0; DQ(n, s+=*x++;); *z++=s;)
 else if(d==1){UI t;
  p=n>>LGSZI; q=p/255; r=p%255; r1=n&(SZI-1); tu=(UC*)&t;
  for(i=0;i<m;++i){
   s=0; v=(UI*)x; 
   DO(q, t=0; DO(255, t+=*v++;); DO(SZI, s+=tu[i];));
         t=0; DO(r,   t+=*v++;); DO(SZI, s+=tu[i];);
   x=(B*)v; DQ(r1, s+=*x++;); 
   *z++=s;
 }}else{A t;UI*tv;
  dw=(d+SZI-1)>>LGSZI; p=dw*SZI; memset(z,C0,m*d*SZI);
  q=n/255; r=n%255;
  t=ga(INT,dw,1,0); if(!t)R;
  tu=UAV(t); tv=(UI*)tu; v=(UI*)x;
  for(i=0;i<m;++i,z+=d){
   DO(q, memset(tv,C0,p); DO(255, DO(dw,tv[i]+=v[i];); x+=d; v=(UI*)x;); DO(d,z[i]+=tu[i];));
         memset(tv,C0,p); DO(r,   DO(dw,tv[i]+=v[i];); x+=d; v=(UI*)x;); DO(d,z[i]+=tu[i];) ;
}}}  /* +/"r w on boolean w, originally by Roger Moore */
#endif


REDUCEOVF( plusinsI, I, I,  PLUSR, PLUSVV, PLUSRV) 
REDUCEOVF(minusinsI, I, I, MINUSR,MINUSVV,MINUSRV) 
REDUCEOVF(tymesinsI, I, I, TYMESR,TYMESVV,TYMESRV)

REDUCCPFX( plusinsO, D, I,  PLUSO)
REDUCCPFX(minusinsO, D, I, MINUSO) 
REDUCCPFX(tymesinsO, D, I, TYMESO) 

#define redprim256rk1(prim,identity) \
 __m256i endmask; /* length mask for the last word */ \
 _mm256_zeroupper(VOIDARG); \
  /* prim/ vectors */ \
  __m256d idreg=_mm256_set1_pd(identity); \
  endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-n)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
  DQ(m, __m256d acc0=idreg; __m256d acc1=idreg; __m256d acc2=idreg; __m256d acc3=idreg; \
   DQ((n-1)>>(2+LGNPAR), acc0=prim(acc0,_mm256_loadu_pd(x)); acc1=prim(acc1,_mm256_loadu_pd(x+NPAR)); acc2=prim(acc2,_mm256_loadu_pd(x+2*NPAR)); acc3=prim(acc3,_mm256_loadu_pd(x+3*NPAR)); x+=4*NPAR; ) \
   if((n-1)&((4-1)<<LGNPAR)){acc0=prim(acc0,_mm256_loadu_pd(x));\
    if(((n-1)&((4-1)<<LGNPAR))>=2*NPAR){acc1=prim(acc1,_mm256_loadu_pd(x+NPAR)); \
     if(((n-1)&((4-1)<<LGNPAR))>2*NPAR){acc2=prim(acc2,_mm256_loadu_pd(x+2*NPAR));} \
    } \
    x += (n-1)&((4-1)<<LGNPAR); \
   } \
   acc3=prim(acc3,_mm256_blendv_pd(idreg,_mm256_maskload_pd(x,endmask),_mm256_castsi256_pd(endmask))); x+=((n-1)&(NPAR-1))+1; \
   acc0=prim(acc0,acc1); acc2=prim(acc2,acc3); acc0=prim(acc0,acc2); /* combine accumulators vertically */ \
   acc0=prim(acc0,_mm256_permute2f128_pd(acc0,acc0,0x01)); acc0=prim(acc0,_mm256_permute_pd(acc0,0xf));   /* combine accumulators horizontally  01+=23, 0+=1 */ \
   _mm_storel_pd(z++,_mm256_castpd256_pd128 (acc0)); /* store the single result */ \
  )

AHDRR(plusinsD,D,D){I i;D* RESTRICT y;
  NAN0;
  // latency of add is 4, so use 4 accumulators
  if(d==1){
#if C_AVX&&SY_64
   redprim256rk1(_mm256_add_pd,0.0)
#else
   x += m*n; z+=m; DQ(m, D v0=0.0; D v1=0.0; if(((n+1)&3)==0)v1=*--x; D v2=0.0; if(n&2)v2=*--x; D v3=0.0; if(n&3)v3=*--x;
                       DQ(n>>2, v0=PLUS(*--x,v0); v1=PLUS(*--x,v1); v2=PLUS(*--x,v2); v3=PLUS(*--x,v3);); v0+=v1; v2+=v3;*--z=v0+v2;)
#endif
  }
  else if(1==n){if(sizeof(D)!=sizeof(D)){DQ(n, *z++=    *x++;)}else{MC((C*)z,(C*)x,d*sizeof(D));}}
  else{z+=(m-1)*d; x+=(m*n-1)*d;
   for(i=0;i<m;++i,z-=d){
    y=x; x-=d; plusDD(1,d,x,y,z,jt); x-=d;
    DQ(n-2,    plusDD(1,d,x,z,z,jt); x-=d; );
   }
  }
  NAN1V;
}

REDUCENAN( plusinsZ, Z, Z, zplus, plusZZ )
REDUCEPFX( plusinsX, X, X, xplus, plusXX, plusXX )

REDUCEPFX(minusinsB, I, B, MINUS, minusBB, minusBI ) 
REDUCENAN(minusinsD, D, D, MINUS, minusDD ) 
REDUCENAN(minusinsZ, Z, Z, zminus, minusZZ) 

REDUCEPFX(tymesinsD, D, D, TYMES, tymesDD, tymesDD ) 
REDUCEPFX(tymesinsZ, Z, Z, ztymes, tymesZZ, tymesZZ) 

REDUCENAN(  divinsD, D, D, DIV, divDD   )
REDUCENAN(  divinsZ, Z, Z, zdiv, divZZ  )

REDUCEPFXIDEM2(  maxinsI, I, I, MAX, maxII   )
REDUCEPFXIDEM2PRIM256(  maxinsD, D, D, MAX, maxDD, _mm256_max_pd, infm  )
REDUCEPFX(  maxinsX, X, X, XMAX, maxXX , maxXX )
REDUCEPFX(  maxinsS, SB,SB,SBMAX, maxSS, maxSS )

REDUCEPFXIDEM2(  mininsI, I, I, MIN, minII   )
REDUCEPFXIDEM2PRIM256(  mininsD, D, D, MIN, minDD, _mm256_min_pd, inf   )
REDUCEPFX(  mininsX, X, X, XMIN, minXX, minXX  )
REDUCEPFX(  mininsS, SB,SB,SBMIN, minSS, minSS )


static DF1(jtred0){DECLF;A x;I f,r,wr,*s;
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r; RESETRANK; s=AS(w);
 if(AT(w)&DENSE){GA(x,AT(w),0L,r,f+s);}else{GASPARSE(x,AT(w),1,r,f+s);}
 R reitem(vec(INT,f,s),lamin1(df1(x,(AT(w)&SBT)?idensb(fs):iden(fs))));
}    /* f/"r w identity case */

// general reduce.  We inplace the results into the next iteration.  This routine cannot inplace its inputs.
static DF1(jtredg){F1PREFIP;PROLOG(0020);DECLF;AD * RESTRICT a;I i,k,n,r,wr;A *old;
 RZ(w);
 ASSERT(DENSE&AT(w),EVNONCE);
 // loop over rank
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; RESETRANK;
 if(r<wr)R rank1ex(w,self,r,jtredg);
 // From here on we are doing a single reduction
 // TODO: should detect &.> and avoid overhead
 n=AS(w)[0]; // n=#cells
 // Allocate virtual block for the running x argument.
 A origw=w; I origwc=AC(w);  // save inplaceability of the original w
 fauxblock(virtafaux); fauxvirtual(a,virtafaux,w,r-1,ACUC1|ACINPLACE);  // allocate UNINCORPORABLE block, allow inplacing (only if jtinplace determines that the backer is inplaceable)
 old=jt->tnextpushp; // save stack mark for subsequent frees.  We keep the a argument over the calls, but allow the w to be deleted
 // w will hold the result from the iterations.  Init to value of last cell
 // Since there are multiple cells, w may be in a virtual block; but we don't rely on that.
 RZ(w=tail(w)); k=AN(w)<<bplg(AT(w)); // k=length of input cell in bytes
 // Calculate inplaceability for most of the run.  We can inplace the left arg, which is always virtual, if w is direct inplaceable.
 // We can inplace the right arg the first time if it is direct inplaceable, and always after that.  This is subject to approval by the verb u
 // and the input jtinplace.  We turn off WILLBEOPENED status in jtinplace for the callee.
 I inplacelaterw = (FAV(fs)->flag>>(VJTFLGOK2X-JTINPLACEWX)) & JTINPLACEW;  // JTINPLACEW if the verb can handle inplacing
 jtinplace = (J)(intptr_t)(((I)jt) + (JTINPLACEW+JTINPLACEA)*(inplacelaterw&(I)jtinplace&((AT(w)&TYPEVIPOK)!=0)&(origwc>>(BW-1))));  // inplace left arg, and first right arg, only if w is direct inplaceable, enabled, and verb can take it
 // fill in the shape, offset, and item-count of the virtual block
 AN(a)=AN(w); AK(a)+=(n-2)*k; MCISH(AS(a),AS(w),r-1);  // make the virtual block look like the tail, except for the offset
 // Mark the blocks as inplaceable.  They won't be used as inplaceable unless permitted by jtinplace
 // We need to free memory in case the called routine leaves it unfreed (that's bad form & we shouldn't expect it), and also to free the result of the
 // previous iteration.  We don't want to free every time, though, because that does ra() on w which could be a costly traversal if it's a nonrecusive recursible type.
 // As a compromise we free every few iterations: at least one per 8 iterations, and at least 8 times through the process
#define LGMINGCS 3  // lg2 of minimum number of times we call gc
#define MINGCINTERVAL 8  // max spacing between frees
 I freedist=MIN((n+((1<<LGMINGCS)-1))>>LGMINGCS,MINGCINTERVAL); I freephase=freedist;
 for(i=1;i<n;++i){   // loop through items
  AC(a)=ACUC1|ACINPLACE;   // in case we created a virtual block from it, restore inplaceability to the UNINCORPABLE block
  RZ(w=CALL2IP(f2,a,w,fs));
  if(--freephase==0){w=gc(w,old); freephase=freedist;}   // free the buffers we allocated, except for the result
  // if w happens to be the same virtual block that we passed in, we have to clone it before we change the pointer
  if(a==w){RZ(w=virtual(w,0,AR(a))); AN(w)=AN(a); MCISH(AS(w),AS(a),AR(a));}
  // move to next input cell
  AK(a) -= k;
  // set larger inplaceability for iterations after the first
  jtinplace = (J)(intptr_t)((I)jtinplace|inplacelaterw);
 }
 EPILOG(w);  // this frees the virtual block, at the least
}    /* f/"r w for general f and 1<(-r){$w */


static C fca[]={CSTAR, CPLUS, CEQ, CMIN, CMAX, CPLUSDOT, CSTARDOT, CNE, 0};  /* commutative & associative */

static A jtredsp1a(J jt,C id,A z,A e,I n,I r,I*s){A t;B b,p=0;D d=1;
 switch(id){
  default:       ASSERT(0,EVNONCE);
  case CPLUSDOT: R n?gcd(z,e):ca(e);
  case CSTARDOT: R n?lcm(z,e):ca(e);
  case CMIN:     R n?minimum(z,e):ca(e);
  case CMAX:     R n?maximum(z,e):ca(e);
  case CPLUS:    if(n&&equ(e,num[0]))R z; DO(r, d*=s[i];); t=tymes(e,d>IMAX?scf(d-n):sc((I)d-n)); R n?plus (z,t):t;
  case CSTAR:    if(n&&equ(e,num[1] ))R z; DO(r, d*=s[i];); t=expn2(e,d>IMAX?scf(d-n):sc((I)d-n)); R n?tymes(z,t):t;
  case CEQ:      p=1;  /* fall thru */
  case CNE:
   ASSERT(B01&AT(e),EVNONCE); 
   if(!n)*BAV(z)=p; 
   b=1; DO(r, if(!(s[i]&1)){b=0; break;}); 
   R !p==*BAV(e)&&b!=(n&1)?not(z):z;
}}   /* f/w on sparse vector w, post processing */

static A jtredsp1(J jt,A w,A self,C id,VF ado,I cv,I f,I r,I zt){A e,x,z;I m,n;P*wp;
 RZ(w);
 wp=PAV(w); e=SPA(wp,e); x=SPA(wp,x); n=AN(x); m=*AS(w);
 GA(z,zt,1,0,0);
 if(n){((AHDRRFN*)ado)(1L,n,1L,AV(x),AV(z),jt); RE(0); if(m==n)R z;}
 R redsp1a(id,z,e,n,AR(w),AS(w));
}    /* f/"r w for sparse vector w */

DF1(jtredravel){A f,x,z;I n;P*wp;
 F1PREFIP;
 RZ(w);
 f=FAV(self)->fgh[0];  // f from f/
 if(!(SPARSE&AT(w)))R reduce(jtravel(jtinplace,w),f);
 wp=PAV(w); x=SPA(wp,x); n=AN(x);
 while(1){  // Loop to handle restart on overflow
  VA2 adocv = vains(FAV(f)->fgh[0],AT(x));
  ASSERT(adocv.f,EVNONCE);
  GA(z,rtype(adocv.cv),1,0,0);
  if(n)((AHDRRFN*)adocv.f)((I)1,n,(I)1,AV(x),AV(z),jt);  // mustn't adocv on empty
  if(jt->jerr<EWOV){RE(0); R redsp1a(vaid(FAV(f)->fgh[0]),z,SPA(wp,e),n,AR(w),AS(w));}
}}  /* f/@, w */

static A jtredspd(J jt,A w,A self,C id,VF ado,I cv,I f,I r,I zt){A a,e,x,z,zx;I c,m,n,*s,t,*v,wr,*ws,xf,xr;P*wp,*zp;
 RZ(w);
 ASSERT(strchr(fca,id),EVNONCE);
 wp=PAV(w); a=SPA(wp,a); e=SPA(wp,e); x=SPA(wp,x); s=AS(x);
 xr=r; v=AV(a); DO(AN(a), if(f<v[i])--xr;); xf=AR(x)-xr;
 m=prod(xf,s); c=m?AN(x)/m:0; n=s[xf];
 GA(zx,zt,AN(x)/n,AR(x)-1,s); MCISH(xf+AS(zx),1+xf+s,xr-1); 
 ((AHDRRFN*)ado)(c/n,n,m,AV(x),AV(zx),jt); RE(0);
 switch(id){
  case CPLUS: if(!equ(e,num[0]))RZ(e=tymes(e,sc(n))); break; 
  case CSTAR: if(!equ(e,num[1] )&&!equ(e,num[0]))RZ(e=expn2(e,sc(n))); break;
  case CEQ:   ASSERT(B01&AT(x),EVNONCE); if(!*BAV(e)&&0==(n&1))e=num[1]; break;
  case CNE:   ASSERT(B01&AT(x),EVNONCE); if( *BAV(e)&&1==(n&1))e=num[0];
 }
 if(TYPESNE(AT(e),AT(zx))){t=maxtypene(AT(e),AT(zx)); if(TYPESNE(t,AT(zx)))RZ(zx=cvt(t,zx));}
 wr=AR(w); ws=AS(w);
 GASPARSE(z,STYPE(AT(zx)),1,wr-1,ws); if(1<wr)MCISH(f+AS(z),f+1+ws,wr-1);
 zp=PAV(z);
 RZ(a=ca(a)); v=AV(a); DO(AN(a), if(f<v[i])--v[i];);
 SPB(zp,a,a);
 SPB(zp,e,cvt(AT(zx),e));
 SPB(zp,i,SPA(wp,i));
 SPB(zp,x,zx);
 R z;
}    /* f/"r w for sparse w, rank > 1, dense axis */

static B jtredspsprep(J jt,C id,I f,I zt,A a,A e,A x,A y,I*zm,I**zdv,B**zpv,I**zqv,C**zxxv,A*zsn){
     A d,p,q,sn=0,xx;B*pv;C*xxv;I*dv,j,k,m,mm,*qv=0,*u,*v,yc,yr,yr1,*yv;
 v=AS(y); yr=v[0]; yc=v[1]; yr1=yr-1;
 RZ(d=grade1(eq(a,sc(f)))); dv=AV(d); 
 DO(AN(a), if(i!=dv[i]){RZ(q=grade1p(d,y)); qv=AV(q); break;});
 GATV0(p,B01,yr,1); pv=BAV(p); memset(pv,C0,yr);
 u=yv=AV(y); m=mm=0; j=-1; if(qv)v=yv+yc*qv[0];
 for(k=0;k<yr1;++k){
  if(qv){u=v; v=yv+yc*qv[1+k];}else v=u+yc;
  DO(yc-1, if(u[dv[i]]!=v[dv[i]]){++m; pv[k]=1; mm=MAX(mm,k-j); j=k; break;});
  if(!qv)u+=yc;
 }
 if(yr){++m; pv[yr1]=1; mm=MAX(mm,yr1-j);}
 if(qv){j=mm*aii(x); GA(xx,AT(x),j,1,0); xxv=CAV(xx);}
 switch(id){
  case CPLUS: case CPLUSDOT: j=!equ(e,num[0]); break;
  case CSTAR: case CSTARDOT: j=!equ(e,num[1]);  break;
  case CMIN:                 j=!equ(e,zt&B01?num[1] :zt&INT?sc(IMAX):ainf     ); break;
  case CMAX:                 j=!equ(e,zt&B01?num[0]:zt&INT?sc(IMIN):scf(infm)); break;
  case CEQ:                  j=!*BAV(e);     break;
  case CNE:                  j= *BAV(e);     break;
 }
 if(j)GATV0(sn,INT,m,1);
 *zm=m; *zdv=dv; *zpv=pv; *zqv=qv; *zxxv=xxv; *zsn=sn;
 R 1;
}

static B jtredspse(J jt,C id,I wm,I xt,A e,A zx,A sn,A*ze,A*zzx){A b;B nz;I t,zt;
 RZ(b=ne(num[0],sn)); nz=!all0(b); zt=AT(zx);
 switch(id){
  case CPLUS:    if(nz)RZ(zx=plus (zx,       tymes(e,sn) )); RZ(e=       tymes(e,sc(wm)) ); break; 
  case CSTAR:    if(nz)RZ(zx=tymes(zx,bcvt(1,expn2(e,sn)))); RZ(e=bcvt(1,expn2(e,sc(wm)))); break;
  case CPLUSDOT: if(nz)RZ(zx=gcd(zx,from(b,over(num[0],e))));                 break;
  case CSTARDOT: if(nz)RZ(zx=lcm(zx,from(b,over(num[1] ,e))));                 break;
  case CMIN:     if(nz)RZ(zx=minimum(zx,from(b,over(zt&B01?num[1]: zt&INT?sc(IMAX):ainf,     e)))); break;
  case CMAX:     if(nz)RZ(zx=maximum(zx,from(b,over(zt&B01?num[0]:zt&INT?sc(IMIN):scf(infm),e)))); break;
  case CEQ:      ASSERT(B01&xt,EVNONCE); if(nz)RZ(zx=eq(zx,eq(num[0],residue(num[2],sn)))); if(!(wm&1))e=num[1];  break;
  case CNE:      ASSERT(B01&xt,EVNONCE); if(nz)RZ(zx=ne(zx,eq(num[1], residue(num[2],sn)))); if(!(wm&1))e=num[0]; break;
 }
 if(TYPESNE(AT(e),AT(zx))){t=maxtypene(AT(e),AT(zx)); if(TYPESNE(t,AT(zx)))RZ(zx=cvt(t,zx));}
 *ze=e; *zzx=zx;
 R 1;
}

static A jtredsps(J jt,A w,A self,C id,VF ado,I cv,I f,I r,I zt){A a,a1,e,sn,x,x1=0,y,z,zx,zy;B*pv;
     C*xv,*xxv,*zv;I*dv,i,m,n,*qv,*sv,*v,wr,xk,xt,wm,*ws,xc,yc,yr,*yu,*yv,zk;P*wp,*zp;
 RZ(w);
 ASSERT(strchr(fca,id),EVNONCE);
 wr=AR(w); ws=AS(w); wm=ws[f];
 wp=PAV(w); a=SPA(wp,a); e=SPA(wp,e); 
 y=SPA(wp,i); v=AS(y); yr=v[0]; yc=v[1]; yv=AV(y);
 x=SPA(wp,x); xt=AT(x); xc=aii(x);
 RZ(redspsprep(id,f,zt,a,e,x,y,&m,&dv,&pv,&qv,&xxv,&sn));
 xv=CAV(x); xk=xc<<bplg(xt);
 GA(zx,zt,m*xc,AR(x),AS(x)); AS(zx)[0]=m; zv=CAV(zx); zk=xc<<bplg(zt);
 GATV0(zy,INT,m*(yc-1),2); v=AS(zy); v[0]=m; v[1]=yc-1; yu=AV(zy);
 v=qv; if(sn)sv=AV(sn);
 for(i=0;i<m;++i){A y;B*p1;C*u;I*vv;
  p1=1+(B*)memchr(pv,C1,yr); n=p1-pv; if(sn)sv[i]=wm-n; pv=p1;
  vv=qv?yv+yc**v:yv; DO(yc-1, *yu++=vv[dv[i]];);
  if(1<n){if(qv){u=xxv; DO(n, MC(u,xv+xk*v[i],xk); u+=xk;);} ((AHDRRFN*)ado)(xc,n,1L,qv?xxv:xv,zv,jt); RE(0);}
  else   if(zk==xk)MC(zv,qv?xv+xk**v:xv,xk);
  else   {if(!x1)GA(x1,xt,xc,1,0); MC(AV(x1),qv?xv+xk**v:xv,xk); RZ(y=cvt(zt,x1)); MC(zv,AV(y),zk);}
  zv+=zk; if(qv)v+=n; else{xv+=n*xk; yv+=n*yc;}
 }
 if(sn)RZ(redspse(id,wm,xt,e,zx,sn,&e,&zx));
 RZ(a1=ca(a)); v=AV(a1); n=0; DO(AN(a), if(f!=v[i])v[n++]=v[i]-(I )(f<v[i]););
 GASPARSE(z,STYPE(AT(zx)),1,wr-1,ws); if(1<r)MCISH(f+AS(z),f+1+ws,r-1);
 zp=PAV(z);
 SPB(zp,a,vec(INT,n,v)); 
 SPB(zp,e,cvt(AT(zx),e));
 SPB(zp,x,zx); 
 SPB(zp,i,zy);
 R z;
}    /* f/"r w for sparse w, rank > 1, sparse axis */

static DF1(jtreducesp){A a,g,z;B b;I f,n,r,*v,wn,wr,*ws,wt,zt;P*wp;
 RZ(w);J jtinplace=jt;
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r;  // no RESETRANK
 wn=AN(w); ws=AS(w); n=r?ws[f]:1;
 wt=AT(w); wt=wn?DTYPE(wt):B01;
 g=VAV(self)->fgh[0];
 if(!n)R red0(w,self);  // red0 uses ranks, and resets them
 C id=vaid(g);
 VA2 adocv = vains(g,wt);
 if(2==n&&!(adocv.f&&strchr(fca,id))){
  A x; IRS2(num[0],w,0L,0,r,jtfrom,x); A y; IRS2(num[1],w,0L,0,r,jtfrom,y);
  R df2(x,y,g);  // rank has been reset for this call
 }
 // original rank still set
 if(!adocv.f)R redg(w,self);
 if(1==n)R tail(w);
 zt=rtype(adocv.cv);
 RESETRANK;
 if(1==wr)z=redsp1(w,self,id,adocv.f,adocv.cv,f,r,zt);
 else{
  wp=PAV(w); a=SPA(wp,a); v=AV(a);
  b=0; DO(AN(a), if(f==v[i]){b=1; break;});
  z=b?redsps(w,self,id,adocv.f,adocv.cv,f,r,zt):redspd(w,self,id,adocv.f,adocv.cv,f,r,zt);
 }
 R jt->jerr>=EWOV?IRS1(w,self,r,jtreducesp,z):z;
}    /* f/"r for sparse w */

#define BR2IFX(T,F)     {T*u=(T*)wv,*v=u+d,x,y;                                           \
                         GATV(z,B01,wn>>1,wr-1,ws); zv=BAV(z);                               \
                         if(1<d)DQ(m, DQ(d, x=*u++; y=*v++; *zv++=x F y; ); u+=d; v+=d;)  \
                         else   DQ(m,       x=*u++; y=*u++; *zv++=x F y;               ); \
                        }
#define BR2PFX(T,F)     {T*u=(T*)wv,*v=u+d,x,y;                                           \
                         GATV(z,B01,wn>>1,wr-1,ws); zv=BAV(z);                               \
                         if(1<d)DQ(m, DQ(d, x=*u++; y=*v++; *zv++=F(x,y);); u+=d; v+=d;)  \
                         else   DQ(m,       x=*u++; y=*u++; *zv++=F(x,y);              ); \
                        }
#define BTABIFX(F)      {btab[0                        ]=0 F 0;  \
                         btab[C_LE?256:  1]=0 F 1;  \
                         btab[C_LE?  1:256]=1 F 0;  \
                         btab[257                      ]=1 F 1;  \
                        }
#define BTABPFX(F)      {btab[0                        ]=F(0,0); \
                         btab[C_LE?256:  1]=F(0,1); \
                         btab[C_LE?  1:256]=F(1,0); \
                         btab[257                      ]=F(1,1); \
                        }
#define BR2CASE(t,id)   ((id)+256*(t))

static B jtreduce2(J jt,A w,C id,I f,I r,A*zz){A z=0;B b=0,btab[258],*zv;I c,d,m,wn,wr,*ws,*wv;
 wn=AN(w); wr=AR(w); ws=AS(w); wv=AV(w);
 PROD(m,f,ws); PROD(c,r,f+ws); d=c>>1;
 switch(BR2CASE(CTTZ(AT(w)),id)){
  case BR2CASE(B01X,CEQ     ): if(b=1==r)BTABIFX(==   ); break;
  case BR2CASE(B01X,CNE     ): if(b=1==r)BTABIFX(!=   ); break;
  case BR2CASE(B01X,CLT     ): if(b=1==r)BTABIFX(<    ); break;
  case BR2CASE(B01X,CLE     ): if(b=1==r)BTABIFX(<=   ); break;
  case BR2CASE(B01X,CGT     ): if(b=1==r)BTABIFX(>    ); break;
  case BR2CASE(B01X,CGE     ): if(b=1==r)BTABIFX(>=   ); break;
  case BR2CASE(B01X,CMAX    ):
  case BR2CASE(B01X,CPLUSDOT): if(b=1==r)BTABIFX(||   ); break;
  case BR2CASE(B01X,CPLUSCO ): if(b=1==r)BTABPFX(BNOR ); break;
  case BR2CASE(B01X,CMIN    ):
  case BR2CASE(B01X,CSTAR   ):
  case BR2CASE(B01X,CSTARDOT): if(b=1==r)BTABIFX(&&   ); break;
  case BR2CASE(B01X,CSTARCO ): if(b=1==r)BTABPFX(BNAND); break;
  case BR2CASE(LITX,CEQ     ): BR2IFX(C,== ); break;
  case BR2CASE(LITX,CNE     ): BR2IFX(C,!= ); break;
  case BR2CASE(C2TX,CEQ     ): BR2IFX(US,== ); break;
  case BR2CASE(C2TX,CNE     ): BR2IFX(US,!= ); break;
  case BR2CASE(C4TX,CEQ     ): BR2IFX(C4,== ); break;
  case BR2CASE(C4TX,CNE     ): BR2IFX(C4,!= ); break;
  case BR2CASE(SBTX,CEQ     ): BR2IFX(SB,== ); break;
  case BR2CASE(SBTX,CLT     ): BR2PFX(SB,SBLT); break;
  case BR2CASE(SBTX,CLE     ): BR2PFX(SB,SBLE); break;
  case BR2CASE(SBTX,CGT     ): BR2PFX(SB,SBGT); break;
  case BR2CASE(SBTX,CGE     ): BR2PFX(SB,SBGE); break;
  case BR2CASE(SBTX,CNE     ): BR2IFX(SB,!= ); break;
  case BR2CASE(INTX,CEQ     ): BR2IFX(I,== ); break;
  case BR2CASE(INTX,CLT     ): BR2IFX(I,<  ); break;
  case BR2CASE(INTX,CLE     ): BR2IFX(I,<= ); break;
  case BR2CASE(INTX,CGT     ): BR2IFX(I,>  ); break;
  case BR2CASE(INTX,CGE     ): BR2IFX(I,>= ); break;
  case BR2CASE(INTX,CNE     ): BR2IFX(I,!= ); break;
  case BR2CASE(FLX, CEQ     ): BR2PFX(D,TEQ); break;
  case BR2CASE(FLX, CLT     ): BR2PFX(D,TLT); break;
  case BR2CASE(FLX, CLE     ): BR2PFX(D,TLE); break;
  case BR2CASE(FLX, CGT     ): BR2PFX(D,TGT); break;
  case BR2CASE(FLX, CGE     ): BR2PFX(D,TGE); break;
  case BR2CASE(FLX, CNE     ): BR2PFX(D,TNE); break;
 }
 if(b){S*u=(S*)wv; GATV(z,B01,wn>>1,wr-1,ws); zv=BAV(z); DQ(m, *zv++=btab[*u++];);}
 if(z&&1<r){I*u=f+AS(z),*v=f+1+ws; DQ(r-1, *u++=*v++;);}
 *zz=z;
 R 1;
}    /* f/"r for dense w over an axis of length 2 */

static DF1(jtreduce){A z;I d,f,m,n,r,t,wn,wr,*ws,wt,zt;
 RZ(w);F1PREFIP;
 wn=AN(w); wt=wn?AT(w):B01;   // Treat empty as Boolean type
 if(SPARSE&AT(w))R reducesp(w,self);  // If sparse, go handle it
 wr=AR(w); ws=AS(w);
 // Create  r: the effective rank; f: length of frame; n: # items in a CELL of w
 r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r; n=r?ws[f]:1;  // no RESETRANK
 // Handle the special cases: neutrals, single items
 if(n>1){
  // Normal case, >1 item.
  // The case of empty w is interesting, because the #cells, and the #atoms in an item of a cell, may both overflow if
  // n=0.  But we have handled that case above.  If n is not 0, there may be other zeros in the shape that allow
  // an overflow when an infix of the shape is multiplied; but that won't matter because the other 0 will guarantee that there
  // are no atoms written

  // Normal processing for multiple items.  Get the routine & flags to process it
  VA2 adocv = vains(FAV(self)->fgh[0],wt);
  // If there is no special routine, go perform general reduce
  if(!adocv.f)R redg(w,self);  // jt->ranks is still set.  redg will clear the ranks
  // Here for primitive reduce handled by special code.
  // Calculate m: #cells of w to operate on; d: #atoms in an item of a cell of w cell of w (a cell to which u is applied);
  // zn: #atoms in result
  PROD(d,r-1,f+ws+1);  //  */ }. $ cell
  // m=*/ frame (i. e. #cells to operate on)
  // r cannot be 0 (would be handled above).  Calculate low part of zn first
  PROD(m,f,ws);
  RESETRANK;   // clear rank now that we've used it - not really required here?
  // Allocate the result area
  zt=rtype(adocv.cv);
  GA(z,zt,m*d,MAX(0,wr-1),ws); if(1<r)MCISH(f+AS(z),f+1+ws,r-1);  // allocate, and install shape
  if(m*d==0)RETF(z);  // mustn't call the function on an empty argument!
  // Convert inputs if needed 
  if((t=atype(adocv.cv))&&TYPESNE(t,wt))RZ(w=cvt(t,w));
  // call the selected reduce routine.
  ((AHDRRFN*)adocv.f)(d,n,m,AV(w),AV(z),jt);
  // if return is EWOV, it's an integer overflow and we must restart, after restoring the ranks
  // EWOV1 means that there was an overflow on a single result, which was calculated accurately and stored as a D.  So in that case all we
  // have to do is change the type of the result.
  if(jt->jerr)if(jt->jerr==EWOV1){RESETERR;AT(z)=FL;RETF(z);}else {RETF(jt->jerr>=EWOV?IRS1(w,self,r,jtreduce,z):0);} else {RETF(adocv.cv&VRI+VRD?cvz(adocv.cv,z):z);}

  // special cases:
 }else if(n==1)R head(w);    // reduce on single items - ranks are still set
 else R red0(w,self);    // 0 items - neutrals - ranks are still set



}    /* f/"r w main control */

static A jtredcatsp(J jt,A w,A z,I r){A a,q,x,y;B*b;I c,d,e,f,j,k,m,n,n1,p,*u,*v,wr,*ws,xr;P*wp,*zp;
 ws=AS(w); wr=AR(w); f=wr-r; p=ws[1+f];
 wp=PAV(w); x=SPA(wp,x); y=SPA(wp,i); a=SPA(wp,a); v=AV(a); 
 m=*AS(y); n=AN(a); n1=n-1; xr=AR(x);
 RZ(b=bfi(wr,a,1));
 c=b[f]; d=b[1+f]; if(c&&d)b[f]=0; e=f+!c;
 j=0; DO(n, if(e==v[i]){j=i; break;}); 
 k=1; DO(f, if(!b[i])++k;);
 zp=PAV(z); SPB(zp,e,ca(SPA(wp,e)));
 GATV0(q,INT,n-(I )(c&&d),1); v=AV(q); DO(wr, if(b[i])*v++=i-(I )(i>f);); SPB(zp,a,q);
 if(c&&d){          /* sparse sparse */
  SPB(zp,x,ca(x));
  SPB(zp,i,repeatr(ne(a,sc(f)),y)); q=SPA(zp,i);  // allow for virtualization of SPB
  v=j+AV(q); u=j+AV(y);
  DQ(m, *v=p*u[0]+u[1]; v+=n1; u+=n;);
 }else if(!c&&!d){  /* dense dense */
  u=AS(x); GA(q,AT(x),AN(x),xr-1,u); v=k+AS(q); *v=u[k]*u[1+k]; MCISH(1+v,2+k+u,xr-k-2);
  MC(AV(q),AV(x),AN(x)<<bplg(AT(x)));
  SPB(zp,x,q); SPB(zp,i,ca(y));
 }else{             /* other */
  GATV0(q,INT,xr,1); v=AV(q); 
  if(1!=k){*v++=0; *v++=k; e=0; DQ(xr-1, ++e; if(e!=k)*v++=e;); RZ(x=cant2(q,x));}
  v=AV(q); u=AS(x); *v=u[0]*u[1]; MCISH(1+v,2+u,xr-1); RZ(x=reshape(vec(INT,xr-1,v),x));
  e=ws[f+c]; RZ(y=repeat(sc(e),y)); v=j+AV(y);
  if(c)DO(m, k=p**v; DO(e, *v=k+  i; v+=n;);)
  else DO(m, k=  *v; DO(e, *v=k+p*i; v+=n;);); 
  RZ(q=grade1(y)); RZ(y=from(q,y)); RZ(x=from(q,x));
  SPB(zp,x,x); SPB(zp,i,y);
 }
 R z;
}    /* ,/"r w for sparse w, 2<r */

DF1(jtredcat){A z;B b;I f,r,*s,*v,wr;
 RZ(w);F1PREFIP;
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r; s=AS(w); RESETRANK;
 b=1==r&&1==s[f];  // special case: ,/ on last axis which has length 1: in that case, the rules say the axis disappears (because of the way ,/ works on length-1 lists)
 if(2>r&&!b)RCA(w);  // in all OTHER cases, result=input for ranks<2
 // use virtual block (possibly self-virtual) for all cases except sparse
 if(!(SPARSE&AT(w))){
  RZ(z=jtvirtual(jtinplace,w,0,wr-1)); AN(z)=AN(w); // Allocate the block.  Then move in AN and shape
  I *zs=AS(z); MCISH(zs,s,f); if(!b){RE(zs[f]=mult(s[f],s[f+1])); MCISH(zs+f+1,s+f+2,r-2);}
  R z;
 }else{
  GASPARSE(z,AT(w),AN(w),wr-1,s); 
  if(!b){v=f+AS(z); RE(*v=mult(s[f],s[1+f])); MCISH(1+v,2+f+s,r-2);}
  R redcatsp(w,z,r);
 }
}    /* ,/"r w */

static DF1(jtredsemi){I f,n,r,*s,wr;
 RZ(w);
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r; s=AS(w); n=r?s[f]:1;  // let the rank run into tail   n=#items  in a cell of w
 if(2>n){ASSERT(n,EVDOMAIN); R tail(w);}  // rank still set
 if(BOX&AT(w))R jtredg(jt,w,self);  // the old way failed because it did not mimic scalar replication; revert to the long way.  ranks are still set
 else{A z; R IRS1(w,0L,r-1,jtbox,z);}  // unboxed, just box the cells
}    /* ;/"r w */

static DF1(jtredstitch){A c,y;I f,n,r,*s,*v,wr;
 RZ(w);
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r; RESETRANK;
 s=AS(w); n=r?s[f]:1;
 ASSERT(n,EVDOMAIN);
 if(1==n)R IRS1(w,0L,r,jthead,y);
 if(1==r){if(2==n)R RETARG(w); A z1,z2,z3; RZ(IRS2(num[-2],w,0L,0L,1L,jtdrop,z1)); RZ(IRS2(num[-2],w,0L,0L,1L,jttake,z2)); R IRS2(z1,z2,0L,1L,0L,jtover,z3);}
 if(2==r)R IRS1(w,0L,2L,jtcant1,y);
 RZ(c=apvwr(wr,0L,1L)); v=AV(c); v[f]=f+1; v[f+1]=f; RZ(y=cant2(c,w));  // transpose last 2 axes
 if(SPARSE&AT(w)){A x;
  GATV0(x,INT,f+r-1,1); v=AV(x); MCISH(v,AS(y),f+1);
  RE(v[f+1]=mult(s[f],s[f+2])); MCISH(v+f+2,s+3+f,r-3);
  RETF(reshape(x,y));
 }else{
  v=AS(y); 
  RE(v[f+1]=mult(s[f],s[f+2])); MCISH(v+f+2,s+3+f,r-3);
  --AR(y); 
  RETF(y);
}}   /* ,./"r w */

static DF1(jtredstiteach){A*wv,y;I n,p,r,t;
 RZ(w);
 n=AN(w);
 if(!(2<n&&1==AR(w)&&BOX&AT(w)))R reduce(w,self);
 wv=AAV(w);  y=wv[0]; p=IC(y); t=AT(y);
 DO(n, y=wv[i]; r=AR(y); if(!((((r-1)&-2)==0)&&p==IC(y)&&TYPESEQ(t,AT(y))))R reduce(w,self););  // rank 1 or 2, rows match, equal types
 R box(razeh(w));
}    /* ,.&.>/ w */

static DF1(jtredcateach){A*u,*v,*wv,x,*xv,z,*zv;I f,m,mn,n,r,wr,*ws,zm,zn;I n1=0,n2=0;
 RZ(w);
 wr=AR(w); ws=AS(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r; RESETRANK;
 n=r?ws[f]:1;
 if(!r||1>=n)R reshape(repeat(ne(sc(f),IX(wr)),shape(w)),n?w:ace);
 if(!(BOX&AT(w)))R df1(cant2(sc(f),w),qq(ds(CBOX),zeroionei[1]));
// bug: ,&.>/ y does scalar replication wrong
// wv=AN(w)+AAV(w); DQ(AN(w), if(AN(*--wv)&&AR(*wv)&&n1&&n2) ASSERT(0,EVNONCE); if((!AR(*wv))&&n1)n2=1; if(AN(*wv)&&1<AR(*wv))n1=1;);
 zn=AN(w)/n; PROD(zm,f,ws); PROD(m,r-1,ws+f+1); mn=m*n;
 GATV(z,BOX,zn,wr-1,ws); MCISH(AS(z)+f,ws+f+1,r-1);
 GATV0(x,BOX,n,1); xv=AAV(x);
 zv=AAV(z); wv=AAV(w); 
 DO(zm, u=wv; DO(m, v=u++; DO(n, xv[i]=*v; v+=m;); A Zz; RZ(Zz=raze(x)); rifv(Zz); *zv++ = Zz;); wv+=mn;);
 RETF(z);
}    /* ,&.>/"r w */

static DF2(jtoprod){R df2(a,w,FAV(self)->fgh[2]);}  // x u/ y - transfer to the u"lr,_ verb (precalculated)


F1(jtslash){A h;AF f1;C c;V*v;I flag=0;
 RZ(w);
 if(NOUN&AT(w))R evger(w,sc(GINSERT));  // treat m/ as m;.6.  This means that a node with CSLASH never contains gerund u
 v=FAV(w); 
 switch(v->id){  // select the monadic case
  case CCOMMA:  f1=jtredcat; flag=VJTFLGOK1;   break;
  case CCOMDOT: f1=jtredstitch; flag=0; break;
  case CSEMICO: f1=jtredsemi; flag=0; break;
  case CUNDER:  f1=jtreduce; if(COPE==ID(v->fgh[1])){c=ID(v->fgh[0]); if(c==CCOMMA)f1=jtredcateach; else if(c==CCOMDOT)f1=jtredstiteach;} flag=0; break;
  default: f1=jtreduce; flag=(v->flag&VJTFLGOK2)>>(VJTFLGOK2X-VJTFLGOK1X); break;  // monad is inplaceable if the dyad for u is
 }
 RZ(h=qq(w,v2(lr(w),RMAX)));  // create the rank compound to use if dyad
 R fdef(0,CSLASH,VERB, f1,jtoprod, w,0L,h, flag|FAV(ds(CSLASH))->flag, RMAX,RMAX,RMAX);
}

A jtaslash (J jt,C c,    A w){RZ(   w); R df1(  w,   slash(ds(c))     );}
A jtaslash1(J jt,C c,    A w){RZ(   w); R df1(  w,qq(slash(ds(c)),zeroionei[1]));}
A jtatab   (J jt,C c,A a,A w){RZ(a&&w); R df2(a,w,   slash(ds(c))     );}

DF1(jtmean){
 RZ(w);
 I wr=AR(w); I r=(RANKT)jt->ranks; r=wr<r?wr:r;
 I n=r?AS(w)[wr-r]:1;
 // leave jt->ranks unchanged to pass into +/
A sum=reduce(w,FAV(self)->fgh[0]);  // calculate +/"r
 RESETRANK;  // back to infinite rank for the divide
 RZ(sum);
 RZ(w=jtatomic2(JTIPA,sum,sc(n),ds(CDIV)));  // take quotient inplace and return it
 RETF(w);
}    // (+/%#)"r w, implemented as +/"r % cell-length

// entry point to execute monad/dyad Fold after the noun arguments are supplied
static DF2(jtfoldx){
 // see if this is monad or dyad
 I foldflag=(~AT(w)&VERB)>>(VERBX-3);  // flags: dyad mult fwd rev  if w is not conj, this must be a dyad call
 self=foldflag&8?self:w; w=foldflag&8?w:a; a=foldflag&8?a:mtv; // if monad, it's w self garbage,  move to '' w self
 // get the rest of the flags from the original ID byte, which was moved to lc
 foldflag|=FAV(self)->lc-CFDOT;  // this sets mult fwd rev
 // define the flags as the special global
 RZ(symbis(nfs(11,"Foldtype_j_"),sc(foldflag),jt->locsyms));
 // execute the Fold.  While it is running, set the flag to allow Z:
 B foldrunning=jt->foldrunning; jt->foldrunning=1; A z=(*(FAV(self)->localuse.lfns[1]))(jt,a,w,self); jt->foldrunning=foldrunning;
 // if there was an error, save the error code and recreate the error at this level, to cover up details inside the script
 if(jt->jerr){I e=jt->jerr; RESETERR; jsignal(e);}
 R z;
}

// entry point for monad and dyad F. F.. F.: F: F:. F::
DF2(jtfold){
 // The name Fold_j_ should have been loaded at startup.  If not, try loading its script.  If that still fails, quit
 A foldconj; I step;
 for(step=0;step<2;++step){
  switch(step){  // try the startup, from the bottom up
  case 1: eval("load'dev/fold'");  // fall through
  case 0: if((foldconj=nameref(nfs(7,"Fold_j_")))&&AT(foldconj)&CONJ)goto found;  // there is always a ref, but it may be to [:
  }
  RESETERR;  // if we loop back, clear errors
 }
 ASSERT(0,EVNONCE);  // not found or not conjunction - error
found: ;

 // Apply Fold_j_ to the input arguments, creating a derived verb to do the work
 A derivvb; RZ(derivvb=unquote(a,w,foldconj));
 // Modify the derived verb to go to our preparatory stub.  Save the dyadic entry point for the derived verb so the stub can call it
 FAV(derivvb)->localuse.lfns[1]=FAV(derivvb)->valencefns[1];
 FAV(derivvb)->valencefns[0]=FAV(derivvb)->valencefns[1]=jtfoldx;
 // Tell the stub what the original fold type was
 FAV(derivvb)->lc=FAV(self)->id;
 R derivvb;
}

// x Z: y
DF2(jtfoldZ){
 ASSERT(jt->foldrunning,EVSYNTAX);  // If fold not running, fail.  Should be a semantic error rather than syntax
 // The name FoldZ_j_ should have been loaded at startup.  If not, fail
 A foldvb; RZ(foldvb=nameref(nfs(8,"FoldZ_j_"))); ASSERT((AT(foldvb)&VERB),EVNONCE);   // error if undefined or not verb
 // Apply FoldZ_j_ to the input arguments, creating a derived verb to do the work
 A z=unquote(a,w,foldvb);
 // if there was an error, save the error code and recreate the error at this level, to cover up details inside the script
 if(jt->jerr){I e=jt->jerr; RESETERR; jsignal(e);}
 R z;
}