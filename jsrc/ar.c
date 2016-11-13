/* Copyright 1990-2014, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Reduce (Insert) and Outer Product                              */

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
 {I q=n/sizeof(T);T s,*y=(T*)x; DO(m, s=0; DO(q, s^=*y++;); PAR; *z++=b==pc;);}

static void vdone(I m,I n,B*x,B*z,B pc){B b,*u;
 if(1==m){UI s,*xi;
  s=0; b=0;
  xi=(I*)x; DO(n/SZI, s^=*xi++;); 
  u=(B*)xi; DO(n%SZI, b^=*u++;);
  u=(B*)&s; DO(SZI,   b^=*u++;);
  *z=b==pc;
 }else if(0==n%sizeof(UI  ))VDONE(UI,  PARITYW)
 else  if(0==n%sizeof(UINT))VDONE(UINT,PARITY4)
 else  if(0==n%sizeof(US  ))VDONE(US,  PARITY2)
 else  DO(m, b=0; DO(n, b^=*x++;); *z++=b==pc;);
}
#else
static void vdone(I m,I n,B*x,B*z,B pc){B b;I q,r;UC*u;UI s,*y;
 q=n/SZI; r=n%SZI; y=(UI*)x;
 switch((r?2:0)+pc){
  case 0: DO(m, s=0; DO(q, s^=*y++;); PARITYW;                            *z++=!b;); break;
  case 1: DO(m, s=0; DO(q, s^=*y++;); PARITYW;                            *z++= b;); break;
  case 2: DO(m, s=0; DO(q, s^=*y++;); PARITYW; u=(UC*)y; DO(r, b^=*u++;); *z++=!b; x+=n; y=(UI*)x;); break;
  case 3: DO(m, s=0; DO(q, s^=*y++;); PARITYW; u=(UC*)y; DO(r, b^=*u++;); *z++= b; x+=n; y=(UI*)x;); break;
}}
#endif

#define RBFXLOOP(T,pfx)  \
 {T*xx=(T*)x,*yy,*z0,*zz=(T*)z;   \
  q=d/sizeof(T);                  \
  for(j=0;j<m;++j){               \
   yy=xx; xx-=q; z0=zz; DO(q, --xx; --yy; --zz; *zz=pfx(*xx,*yy););    \
   DO(n-2,       zz=z0; DO(q, --xx;       --zz; *zz=pfx(*xx,*zz);););  \
 }}  /* non-commutative */

#define RCFXLOOP(T,pfx)  \
 {T*xx=(T*)x,*yy,*z0,*zz=(T*)z;   \
  q=d/sizeof(T);                  \
  for(j=0;j<m;++j){               \
   yy=xx; xx+=q; z0=zz; DO(q, *zz++=pfx(*yy,*xx); ++xx; ++yy;);    \
   DO(n-2,       zz=z0; DO(q, *zz++=pfx(*zz,*xx); ++xx;      ););  \
 }}  /* commutative */

#if SY_ALIGN
#define RBFXODDSIZE(pfx,bpfx)  RBFXLOOP(C,bpfx)
#define REDUCECFX              REDUCEBFX
#else
#define RBFXODDSIZE(pfx,bpfx)  \
 {B*zz;I r,t,*xi,*yi,*zi;                                                       \
  q=d/SZI; r=d%SZI; xi=(I*)x; zz=z;                                             \
  for(j=0;j<m;++j,zz-=d){                                                       \
   yi=xi; xi=(I*)((B*)xi-d); zi=(I*)zz;                                         \
   DO(q, --xi; --yi; *--zi=pfx(*xi,*yi););                                      \
    xi=(I*)((B*)xi-r); yi=(I*)((B*)yi-r); t=pfx(*xi,*yi); MC((B*)zi-r,&t,r);    \
   DO(n-2, zi=(I*)zz; DO(q, --xi; --zi; *zi=pfx(*xi,*zi););                     \
    xi=(I*)((B*)xi-r); zi=(I*)((B*)zi-r); t=pfx(*xi,*zi); MC(    zi,  &t,r););  \
 }}  /* non-commutative */

#define RCFXODDSIZE(pfx,bpfx)  \
 {I r,t,*xi,*yi,*zi;                                                            \
  q=d/SZI; r=d%SZI;                                                             \
  for(j=0;j<m;++j,x+=d,z+=d){                                                   \
   yi=(I*)x; x+=d; xi=(I*)x; zi=(I*)z; DO(q, *zi++=pfx(*yi,*xi); ++xi; ++yi;); t=pfx(*yi,*xi); MC(zi,&t,r);    \
   DO(n-2,   x+=d; xi=(I*)x; zi=(I*)z; DO(q, *zi++=pfx(*zi,*xi); ++xi;      ); t=pfx(*zi,*xi); MC(zi,&t,r););  \
 }}  /* commutative */

#define REDUCECFX(f,pfx,ipfx,spfx,bpfx,vdo)  \
 AHDRP(f,B,B){B*y=0;I d,j,q;                       \
  if(c==n){vdo; R;}                                \
  d=c/n;                                           \
  if(1==n)DO(d, *z++=*x++;)                        \
  else if(0==d%sizeof(UI  ))RCFXLOOP(UI,   pfx)    \
  else if(0==d%sizeof(UINT))RCFXLOOP(UINT,ipfx)    \
  else if(0==d%sizeof(US  ))RCFXLOOP(US,  spfx)    \
  else                      RCFXODDSIZE(pfx,bpfx)  \
 }  /* commutative */

#endif

#define REDUCEBFX(f,pfx,ipfx,spfx,bpfx,vdo)  \
 AHDRP(f,B,B){B*y=0;I d,j,q;                       \
  if(c==n){vdo; R;}                                \
  d=c/n; x+=m*c; z+=m*d;                           \
  if(1==n)DO(d, *--z=*--x;)                        \
  else if(0==d%sizeof(UI  ))RBFXLOOP(UI,   pfx)    \
  else if(0==d%sizeof(UINT))RBFXLOOP(UINT,ipfx)    \
  else if(0==d%sizeof(US  ))RBFXLOOP(US,  spfx)    \
  else                      RBFXODDSIZE(pfx,bpfx)  \
 }  /* non-commutative */

REDUCECFX(  eqinsB, EQ,  IEQ,  SEQ,  BEQ,  vdone(m,n,x,z,(B)(n%2)))
REDUCECFX(  neinsB, NE,  INE,  SNE,  BNE,  vdone(m,n,x,z,1       ))
REDUCECFX(  orinsB, OR,  IOR,  SOR,  BOR,  DO(m, *z++=1&&memchr(x,C1,n);                         x+=c;)) 
REDUCECFX( andinsB, AND, IAND, SAND, BAND, DO(m, *z++=!  memchr(x,C0,n);                         x+=c;))
REDUCEBFX(  ltinsB, LT,  ILT,  SLT,  BLT,  DO(m, *z++= *(x+n-1)&&!memchr(x,C1,n-1)?1:0;          x+=c;))
REDUCEBFX(  leinsB, LE,  ILE,  SLE,  BLE,  DO(m, *z++=!*(x+n-1)&&!memchr(x,C0,n-1)?0:1;          x+=c;))
REDUCEBFX(  gtinsB, GT,  IGT,  SGT,  BGT,  DO(m, y=memchr(x,C0,n); *z++=1&&(y?1&(y-x):1&n);      x+=c;))
REDUCEBFX(  geinsB, GE,  IGE,  SGE,  BGE,  DO(m, y=memchr(x,C1,n); *z++=!  (y?1&(y-x):1&n);      x+=c;))
REDUCEBFX( norinsB, NOR, INOR, SNOR, BNOR, DO(m, y=memchr(x,C1,n); d=y?y-x:n; *z++=(1&d)==d<n-1; x+=c;))
REDUCEBFX(nandinsB, NAND,INAND,SNAND,BNAND,DO(m, y=memchr(x,C0,n); d=y?y-x:n; *z++=(1&d)!=d<n-1; x+=c;))


#if SY_ALIGN
REDUCEPFX(plusinsB,I,B,PLUS)
#else
AHDRR(plusinsB,I,B){I d,dw,i,p,q,r,r1,s;UC*tu;UI*v;
 if(c==n&&n<SZI)DO(m, s=0; DO(n, s+=*x++;); *z++=s;)
 else if(c==n){UI t;
  p=n/SZI; q=p/255; r=p%255; r1=n%SZI; tu=(UC*)&t;
  for(i=0;i<m;++i){
   s=0; v=(UI*)x; 
   DO(q, t=0; DO(255, t+=*v++;); DO(SZI, s+=tu[i];));
         t=0; DO(r,   t+=*v++;); DO(SZI, s+=tu[i];);
   x=(B*)v; DO(r1, s+=*x++;); 
   *z++=s;
 }}else{A t;UI*tv;
  d=c/n; dw=(d+SZI-1)/SZI; p=dw*SZI; memset(z,C0,m*d*SZI);
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

REDUCENAN( plusinsD, D, D, PLUS  ) 
REDUCENAN( plusinsZ, Z, Z, zplus )
REDUCEPFX( plusinsX, X, X, xplus )

REDUCEPFX(minusinsB, I, B, MINUS ) 
REDUCENAN(minusinsD, D, D, MINUS ) 
REDUCENAN(minusinsZ, Z, Z, zminus) 

REDUCEPFX(tymesinsD, D, D, TYMES ) 
REDUCEPFX(tymesinsZ, Z, Z, ztymes) 

REDUCENAN(  divinsD, D, D, DIV   )
REDUCENAN(  divinsZ, Z, Z, zdiv  )

REDUCEPFX(  maxinsI, I, I, MAX   )
REDUCEPFX(  maxinsD, D, D, MAX   )
REDUCEPFX(  maxinsX, X, X, XMAX  )
REDUCEPFX(  maxinsS, SB,SB,SBMAX )

REDUCEPFX(  mininsI, I, I, MIN   )
REDUCEPFX(  mininsD, D, D, MIN   )
REDUCEPFX(  mininsX, X, X, XMIN  )
REDUCEPFX(  mininsS, SB,SB,SBMIN )


static DF1(jtred0){DECLF;A x;I f,r,wr,*s;
 wr=AR(w); r=jt->rank?jt->rank[1]:wr; f=wr-r; s=AS(w);
 jt->rank=0;
 GA(x,AT(w),0L,r,f+s);
 R reitem(vec(INT,f,s),lamin1(df1(x,(AT(w)&SBT)?idensb(fs):iden(fs))));
}    /* f/"r w identity case */

// general reduce.  We inplace the results into the next iteration.  This routine cannot inplace its inputs.
static DF1(jtredg){PROLOG(0020);DECLF;A y,z;B p;C*u,*v;I i,k,n,old,r,wr,yn,yr,*ys,yt;
 RZ(w);
 ASSERT(DENSE&AT(w),EVNONCE);
 // loop over rank
 wr=AR(w); r=jt->rank?jt->rank[1]:wr; jt->rank=0;
 if(r<wr)R rank1ex(w,self,r,jtredg);
 // From here on we are doing a single reduction
 n=IC(w); p=ARELATIVE(w);  // n=#items of cell, p=REL flag
 J jtip = jt; if(VAV(fs)->flag&VINPLACEOK2)jtip=(J)((I)jtip+(JTINPLACEW+JTINPLACEA));  // if f supports inplacing, so do we
 // z will hold the result from the iterations.  Init to value of last cell
 // yt=type, yn=#atoms, yr=rank, ys->shape of input cell
 // Since there are multiple cells, z mst be in a new block and therefore inplaceable
 RZ(z=tail(w)); yt=AT(z); yn=AN(z); yr=AR(z); ys=1+AS(w);
 // k=length of input cell in bytes, v->last cell data
 k=yn*bp(yt); v=CAV(w)+k*(n-1);
 old=jt->tnextpushx; // save stack mark for subsequent frees
 for(i=1;i<n;++i){   // loop through items
  v-=k;    // v-> next item to apply
  GA(y,yt,yn,yr,ys); u=CAV(y);   // allocate block for item, u->data area
  // copy the item into the allocated block
  if(p){A1*wv=(A1*)v,*yv=(A1*)u;I d=(I)w-(I)y; AFLAG(y)=AFREL; DO(yn, yv[i]=d+wv[i];);}else MC(u,v,k); 
  RZ(z=(f2)(jtip,y,z,fs));   // apply the verb to the arguments
  gc(z,old);   // free the buffers we allocated, except for the result
 }
 EPILOG(z);
}    /* f/"r w for general f and 1<(-r){$w */


static C fca[]={CSTAR, CPLUS, CEQ, CMIN, CMAX, CPLUSDOT, CSTARDOT, CNE, 0};  /* commutative & associative */

static A jtredsp1a(J jt,C id,A z,A e,I n,I r,I*s){A t;B b,p=0;D d=1;
 switch(id){
  default:       ASSERT(0,EVNONCE);
  case CPLUSDOT: R n?gcd(z,e):ca(e);
  case CSTARDOT: R n?lcm(z,e):ca(e);
  case CMIN:     R n?minimum(z,e):ca(e);
  case CMAX:     R n?maximum(z,e):ca(e);
  case CPLUS:    if(n&&equ(e,zero))R z; DO(r, d*=s[i];); t=tymes(e,d>IMAX?scf(d-n):sc((I)d-n)); R n?plus (z,t):t;
  case CSTAR:    if(n&&equ(e,one ))R z; DO(r, d*=s[i];); t=expn2(e,d>IMAX?scf(d-n):sc((I)d-n)); R n?tymes(z,t):t;
  case CEQ:      p=1;  /* fall thru */
  case CNE:
   ASSERT(B01&AT(e),EVNONCE); 
   if(!n)*BAV(z)=p; 
   b=1; DO(r, if(!(s[i]%2)){b=0; break;}); 
   R !p==*BAV(e)&&b!=n%2?not(z):z;
}}   /* f/w on sparse vector w, post processing */

static A jtredsp1(J jt,A w,A self,C id,VF ado,I cv,I f,I r,I zt){A e,x,z;I m,n;P*wp;
 RZ(w);
 wp=PAV(w); e=SPA(wp,e); x=SPA(wp,x); n=AN(x); m=*AS(w);
 GA(z,zt,1,0,0);
 if(n){ado(jt,1L,n,n,AV(z),AV(x)); RE(0); if(m==n)R z;}
 R redsp1a(id,z,e,n,AR(w),AS(w));
}    /* f/"r w for sparse vector w */

DF1(jtredravel){A f,x,z;C id;I cv,n;P*wp;VF ado;
 RZ(w);
 f=VAV(self)->f;
 if(!(SPARSE&AT(w)))R reduce(AN(w)?gah(1L,w):mtv,f);
 wp=PAV(w); x=SPA(wp,x); n=AN(x);
 id=vaid(VAV(f)->f);
 while(1){  // Loop to handle restart on overflow
  vains(id,AT(x),&ado,&cv);
  ASSERT(ado,EVNONCE);
  GA(z,rtype(cv),1,0,0);
  if(n)ado(jt,1L,n,n,AV(z),AV(x));
  if(jt->jerr<EWOV)R redsp1a(id,z,SPA(wp,e),n,AR(w),AS(w));;
}}  /* f/@, w */

static A jtredspd(J jt,A w,A self,C id,VF ado,I cv,I f,I r,I zt){A a,e,x,z,zx;I c,m,n,*s,t,*v,wr,*ws,xf,xr;P*wp,*zp;
 RZ(w);
 ASSERT(strchr(fca,id),EVNONCE);
 wp=PAV(w); a=SPA(wp,a); e=SPA(wp,e); x=SPA(wp,x); s=AS(x);
 xr=r; v=AV(a); DO(AN(a), if(f<v[i])--xr;); xf=AR(x)-xr;
 m=prod(xf,s); c=m?AN(x)/m:0; n=s[xf];
 GA(zx,zt,AN(x)/n,AR(x)-1,s); ICPY(xf+AS(zx),1+xf+s,xr-1); 
 ado(jt,m,c,n,AV(zx),AV(x)); RE(0);
 switch(id){
  case CPLUS: if(!equ(e,zero))RZ(e=tymes(e,sc(n))); break; 
  case CSTAR: if(!equ(e,one )&&!equ(e,zero))RZ(e=expn2(e,sc(n))); break;
  case CEQ:   ASSERT(B01&AT(x),EVNONCE); if(!*BAV(e)&&0==n%2)e=one; break;
  case CNE:   ASSERT(B01&AT(x),EVNONCE); if( *BAV(e)&&1==n%2)e=zero;
 }
 if(TYPESNE(AT(e),AT(zx))){t=maxtype(AT(e),AT(zx)); if(TYPESNE(t,AT(zx)))RZ(zx=cvt(t,zx));}
 wr=AR(w); ws=AS(w);
 GA(z,STYPE(AT(zx)),1,wr-1,ws); if(1<wr)ICPY(f+AS(z),f+1+ws,wr-1);
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
 GATV(p,B01,yr,1,0); pv=BAV(p); memset(pv,C0,yr);
 u=yv=AV(y); m=mm=0; j=-1; if(qv)v=yv+yc*qv[0];
 for(k=0;k<yr1;++k){
  if(qv){u=v; v=yv+yc*qv[1+k];}else v=u+yc;
  DO(yc-1, if(u[dv[i]]!=v[dv[i]]){++m; pv[k]=1; mm=MAX(mm,k-j); j=k; break;});
  if(!qv)u+=yc;
 }
 if(yr){++m; pv[yr1]=1; mm=MAX(mm,yr1-j);}
 if(qv){j=mm*aii(x); GA(xx,AT(x),j,1,0); xxv=CAV(xx);}
 switch(id){
  case CPLUS: case CPLUSDOT: j=!equ(e,zero); break;
  case CSTAR: case CSTARDOT: j=!equ(e,one);  break;
  case CMIN:                 j=!equ(e,zt&B01?one :zt&INT?sc(IMAX):ainf     ); break;
  case CMAX:                 j=!equ(e,zt&B01?zero:zt&INT?sc(IMIN):scf(infm)); break;
  case CEQ:                  j=!*BAV(e);     break;
  case CNE:                  j= *BAV(e);     break;
 }
 if(j)GATV(sn,INT,m,1,0);
 *zm=m; *zdv=dv; *zpv=pv; *zqv=qv; *zxxv=xxv; *zsn=sn;
 R 1;
}

static B jtredspse(J jt,C id,I wm,I xt,A e,A zx,A sn,A*ze,A*zzx){A b;B nz;I t,zt;
 RZ(b=ne(zero,sn)); nz=!all0(b); zt=AT(zx);
 switch(id){
  case CPLUS:    if(nz)RZ(zx=plus (zx,       tymes(e,sn) )); RZ(e=       tymes(e,sc(wm)) ); break; 
  case CSTAR:    if(nz)RZ(zx=tymes(zx,bcvt(1,expn2(e,sn)))); RZ(e=bcvt(1,expn2(e,sc(wm)))); break;
  case CPLUSDOT: if(nz)RZ(zx=gcd(zx,from(b,over(zero,e))));                 break;
  case CSTARDOT: if(nz)RZ(zx=lcm(zx,from(b,over(one ,e))));                 break;
  case CMIN:     if(nz)RZ(zx=minimum(zx,from(b,over(zt&B01?one: zt&INT?sc(IMAX):ainf,     e)))); break;
  case CMAX:     if(nz)RZ(zx=maximum(zx,from(b,over(zt&B01?zero:zt&INT?sc(IMIN):scf(infm),e)))); break;
  case CEQ:      ASSERT(B01&xt,EVNONCE); if(nz)RZ(zx=eq(zx,eq(zero,residue(num[2],sn)))); if(!(wm%2))e=one;  break;
  case CNE:      ASSERT(B01&xt,EVNONCE); if(nz)RZ(zx=ne(zx,eq(one, residue(num[2],sn)))); if(!(wm%2))e=zero; break;
 }
 if(TYPESNE(AT(e),AT(zx))){t=maxtype(AT(e),AT(zx)); if(TYPESNE(t,AT(zx)))RZ(zx=cvt(t,zx));}
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
 xv=CAV(x); xk=xc*bp(xt);
 GA(zx,zt,m*xc,AR(x),AS(x)); *AS(zx)=m; zv=CAV(zx); zk=xc*bp(zt);
 GATV(zy,INT,m*(yc-1),2,0); v=AS(zy); v[0]=m; v[1]=yc-1; yu=AV(zy);
 v=qv; if(sn)sv=AV(sn);
 for(i=0;i<m;++i){A y;B*p1;C*u;I*vv;
  p1=1+(B*)memchr(pv,C1,yr); n=p1-pv; if(sn)sv[i]=wm-n; pv=p1;
  vv=qv?yv+yc**v:yv; DO(yc-1, *yu++=vv[dv[i]];);
  if(1<n){if(qv){u=xxv; DO(n, MC(u,xv+xk*v[i],xk); u+=xk;);} ado(jt,1L,n*xc,n,zv,qv?xxv:xv); RE(0);}
  else   if(zk==xk)MC(zv,qv?xv+xk**v:xv,xk);
  else   {if(!x1)GA(x1,xt,xc,1,0); MC(AV(x1),qv?xv+xk**v:xv,xk); RZ(y=cvt(zt,x1)); MC(zv,AV(y),zk);}
  zv+=zk; if(qv)v+=n; else{xv+=n*xk; yv+=n*yc;}
 }
 if(sn)RZ(redspse(id,wm,xt,e,zx,sn,&e,&zx));
 RZ(a1=ca(a)); v=AV(a1); n=0; DO(AN(a), if(f!=v[i])v[n++]=v[i]-(f<v[i]););
 GA(z,STYPE(AT(zx)),1,wr-1,ws); if(1<r)ICPY(f+AS(z),f+1+ws,r-1);
 zp=PAV(z);
 SPB(zp,a,vec(INT,n,v)); 
 SPB(zp,e,cvt(AT(zx),e));
 SPB(zp,x,zx); 
 SPB(zp,i,zy);
 R z;
}    /* f/"r w for sparse w, rank > 1, sparse axis */

static DF1(jtreducesp){A a,g,x,y,z;B b;C id;I cv,f,n,r,rr[2],*v,wn,wr,*ws,wt,zt;P*wp;VF ado;
 RZ(w);
 wr=AR(w); r=jt->rank?jt->rank[1]:wr; f=wr-r;
 wn=AN(w); ws=AS(w); n=r?ws[f]:1;
 wt=AT(w); wt=wn?DTYPE(wt):B01;
 g=VAV(self)->f; id=vaid(g);
 if(!n)R red0(w,self);
 vains(id,wt,&ado,&cv);
 if(2==n&&!(ado&&strchr(fca,id))){
  rr[0]=0; rr[1]=r;
  jt->rank=rr; x=from(zero,w); 
  jt->rank=rr; y=from(one, w); 
  R df2(x,y,g);
 }
 if(!ado)R redg(w,self);
 if(1==n)R tail(w);
 zt=rtype(cv);
 jt->rank=0;
 if(1==wr)z=redsp1(w,self,id,ado,cv,f,r,zt);
 else{
  wp=PAV(w); a=SPA(wp,a); v=AV(a);
  b=0; DO(AN(a), if(f==v[i]){b=1; break;});
  z=b?redsps(w,self,id,ado,cv,f,r,zt):redspd(w,self,id,ado,cv,f,r,zt);
 }
 R jt->jerr>=EWOV?(rr[1]=r,jt->rank=rr,reducesp(w,self)):z;
}    /* f/"r for sparse w */

#define BR2IFX(T,F)     {T*u=(T*)wv,*v=u+d,x,y;                                           \
                         GATV(z,B01,wn/2,wr-1,ws); zv=BAV(z);                               \
                         if(1<d)DO(m, DO(d, x=*u++; y=*v++; *zv++=x F y; ); u+=d; v+=d;)  \
                         else   DO(m,       x=*u++; y=*u++; *zv++=x F y;               ); \
                        }
#define BR2PFX(T,F)     {T*u=(T*)wv,*v=u+d,x,y;                                           \
                         GATV(z,B01,wn/2,wr-1,ws); zv=BAV(z);                               \
                         if(1<d)DO(m, DO(d, x=*u++; y=*v++; *zv++=F(x,y);); u+=d; v+=d;)  \
                         else   DO(m,       x=*u++; y=*u++; *zv++=F(x,y);              ); \
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
 PROD(m,f,ws); PROD(c,r,f+ws); d=c/2;
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
 if(b){S*u=(S*)wv; GATV(z,B01,wn/2,wr-1,ws); zv=BAV(z); DO(m, *zv++=btab[*u++];);}
 if(z&&1<r){I*u=f+AS(z),*v=f+1+ws; DO(r-1, *u++=*v++;);}
 *zz=z;
 R 1;
}    /* f/"r for dense w over an axis of length 2 */

static DF1(jtreduce){A z;C id;I c,cv,f,*jtr,m,n,r,rr[2],t,wn,wr,*ws,wt,zn,zt;VF ado;
 RZ(w);
 wn=AN(w); wt=wn?AT(w):B01;   // Treat empty as Boolean type
 if(SPARSE&AT(w))R reducesp(w,self);  // If sparse, go handle it
 wr=AR(w); ws=AS(w); n=1;
 // Create  r: the effective rank; f: length of frame; n: # items in a CELL of w
 if(jtr=jt->rank) {
  // Rank specified (it must be less than the rank of w).  Set values accordingly.
  r=jtr[1]; f=wr-r; if(r)n=ws[f];
 }else{r=wr; f=0; if(r)n=ws[0];
 }
 id=vaid(VAV(self)->f);
 // Handle the special cases: neutrals, single items, reduce on 2 items
 switch(n){
  case 0: R red0(w,self);    // neutrals
  case 1: R tail(w);    // reduce on single items - note that jt->rank is still set
  case 2: RZ(reduce2(w,id,f,r,&z)); if(z)R z;   // reduce on 2 items.  If there is no special code for the verb, fall through to...
 }
 // The case of empty w is interesting, because the #cells, and the #atoms in an item of a cell, may both overflow if
 // n=0.  But we have handled that case above.  If n is not 0, there may be other zeros in the shape that allow
 // an overflow when an infix of the shape is multiplied; but that won't matter because the other 0 will guarantee that there
 // are no atoms written

 // Normal processing for multiple items.  Get the routine & flags to process it
 vains(id,wt,&ado,&cv);
 // If there is no special routine, go perform general reduce
 if(!ado)R redg(w,self);
 // Here for primitive reduce handled by special code.
 // Calculate m: #cells of w to operate on; c: #atoms in a cell of w (NOT a cell to which u is applied);
 // zn: #atoms in result
 PROD(zn,r-1,f+ws+1);  //  */ }. $ cell
 if(jtr){
  // Rank specified.  m=*/ frame (i. e. #cells to operate on), c=*/ $ cell of w (#atoms in cell), zn=m * */ }. $ cell (# result atoms).
  // r cannot be 0 (would be handled above).  Calculate low part of zn first
  PROD(m,f,ws); c=zn*ws[f]; zn*=m; jt->rank=0;   // clear rank now that we've used it
 }else{
  // No rank, just operate on whole of w
  m=1; c=wn;   // one cell, containing all the atoms
 }
 // Allocate the result area
 zt=rtype(cv);
 GA(z,zt,zn,MAX(0,wr-1),ws); if(1<r)ICPY(f+AS(z),f+1+ws,r-1);  // allocate, and install shape
 // Convert inputs if needed 
 if((t=atype(cv))&&TYPESNE(t,wt))RZ(w=cvt(t,w));
 // call the selected reduce routine.
 ado(jt,m,c,n,AV(z),AV(w));
 // if return is EWOV, it's an integer overflow and we must restart.
 // EWOV1 means that there was an overflow on a single result, which was calculated accurately and stored as a D.  So in that case all we
 // have to do is change the type of the result
 if(jt->jerr)if(jt->jerr==EWOV1){RESETERR;AT(z)=FL;R z;}else R jt->jerr>=EWOV?(rr[1]=r,jt->rank=rr,reduce(w,self)):0; else R cv&VRI+VRD?cvz(cv,z):z;
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
 GATV(q,INT,n-(c&&d),1,0); v=AV(q); DO(wr, if(b[i])*v++=i-(i>f);); SPB(zp,a,q);
 if(c&&d){          /* sparse sparse */
  SPB(zp,x,ca(x));
  SPB(zp,i,q=repeatr(ne(a,sc(f)),y));
  v=j+AV(q); u=j+AV(y);
  DO(m, *v=p*u[0]+u[1]; v+=n1; u+=n;);
 }else if(!c&&!d){  /* dense dense */
  u=AS(x); GA(q,AT(x),AN(x),xr-1,u); v=k+AS(q); *v=u[k]*u[1+k]; ICPY(1+v,2+k+u,xr-k-2);
  MC(AV(q),AV(x),AN(x)*bp(AT(x)));
  SPB(zp,x,q); SPB(zp,i,ca(y));
 }else{             /* other */
  GATV(q,INT,xr,1,0); v=AV(q); 
  if(1!=k){*v++=0; *v++=k; e=0; DO(xr-1, ++e; if(e!=k)*v++=e;); RZ(x=cant2(q,x));}
  v=AV(q); u=AS(x); *v=u[0]*u[1]; ICPY(1+v,2+u,xr-1); RZ(x=reshape(vec(INT,xr-1,v),x));
  e=ws[f+c]; RZ(y=repeat(sc(e),y)); v=j+AV(y);
  if(c)DO(m, k=p**v; DO(e, *v=k+  i; v+=n;);)
  else DO(m, k=  *v; DO(e, *v=k+p*i; v+=n;);); 
  RZ(q=grade1(y)); RZ(y=from(q,y)); RZ(x=from(q,x));
  SPB(zp,x,x); SPB(zp,i,y);
 }
 R z;
}    /* ,/"r w for sparse w, 2<r */

static DF1(jtredcat){A z;B b;I f,r,*s,*v,wr;
 RZ(w);
 wr=AR(w); r=jt->rank?jt->rank[1]:wr; f=wr-r; s=AS(w); jt->rank=0;
 b=1==r&&1==s[f];
 if(2>r&&!b)R ca(w);
 GA(z,AT(w),AN(w),wr-1,s); 
 if(!b){v=f+AS(z); RE(*v=mult(s[f],s[1+f])); ICPY(1+v,2+f+s,r-2);}
 if(SPARSE&AT(w))R redcatsp(w,z,r);
 MC(AV(z),AV(w),AN(w)*bp(AT(w)));
 if(ARELATIVE(w)){AFLAG(z)=AFREL; z=relocate((I)w-(I)z,z);}
 R z;
}    /* ,/"r w */

static DF1(jtredsemi){I f,n,r,*s,wr;
 RZ(w);
 wr=AR(w); r=jt->rank?jt->rank[1]:wr; f=wr-r; s=AS(w); n=r?s[f]:1;
 if(2>n){ASSERT(n,EVDOMAIN); R tail(w);}
 if(BOX&AT(w))R irs2(rank1ex(curtail(w),0L,r-1,jtbox),tail(w),0L,r,r-1,jtover);
 else R irs1(w,0L,r-1,jtbox);
}    /* ;/"r w */

static DF1(jtredstitch){A c,y;I f,n,r,*s,*v,wr;
 RZ(w);
 wr=AR(w); r=jt->rank?jt->rank[1]:wr; f=wr-r; jt->rank=0;
 s=AS(w); n=r?s[f]:1;
 ASSERT(n,EVDOMAIN);
 if(1==n)R irs1(w,0L,r,jthead);
 if(1==r)R 2==n?ca(w):irs2(irs2(num[-2],w,0L,0L,1L,jtdrop),irs2(num[-2],w,0L,0L,1L,jttake),0L,1L,0L,jtover);
 if(2==r)R irs1(w,0L,2L,jtcant1);
 RZ(c=IX(wr)); v=AV(c); v[f]=f+1; v[f+1]=f; RZ(y=cant2(c,w));
 if(SPARSE&AT(w)){A x;
  GATV(x,INT,f+r-1,1,0); v=AV(x); ICPY(v,AS(y),f+1);
  RE(v[f+1]=mult(s[f],s[f+2])); ICPY(v+f+2,s+3+f,r-3);
  R reshape(x,y);
 }else{
  v=AS(y); 
  RE(v[f+1]=mult(s[f],s[f+2])); ICPY(v+f+2,s+3+f,r-3);
  --AR(y); 
  R y;
}}   /* ,./"r w */

static DF1(jtredstiteach){A*wv,y;I n,p,r,t,wd;
 RZ(w);
 n=AN(w);
 if(!(2<n&&1==AR(w)&&BOX&AT(w)))R reduce(w,self);
 wv=AAV(w); wd=(I)w*ARELATIVE(w); y=WVR(0); p=IC(y); t=AT(y);
 DO(n, y=WVR(i); r=AR(y); if(!(r&&r<=2&&p==IC(y)&&TYPESEQ(t,AT(y))))R reduce(w,self););
 R box(razeh(w));
}    /* ,.&.>/ w */

static DF1(jtredcateach){A*u,*v,*wv,x,*xv,z,*zv;I f,m,mn,n,r,wd,wr,*ws,zm,zn;I n1=0,n2=0;
 RZ(w);
 wr=AR(w); ws=AS(w); r=jt->rank?jt->rank[1]:wr; f=wr-r; jt->rank=0;
 n=r?ws[f]:1;
 if(!r||1>=n)R reshape(repeat(ne(sc(f),IX(wr)),shape(w)),n?w:ace);
 if(!(BOX&AT(w)))R df1(cant2(sc(f),w),qq(ds(CBOX),one));
// bug: ,&.>/ y does scalar replication wrong
// wv=AN(w)+AAV(w); DO(AN(w), if(AN(*--wv)&&AR(*wv)&&n1&&n2) ASSERT(0,EVNONCE); if((!AR(*wv))&&n1)n2=1; if(AN(*wv)&&1<AR(*wv))n1=1;);
 zn=AN(w)/n; PROD(zm,f,ws); PROD(m,r-1,ws+f+1); mn=m*n;
 GATV(z,BOX,zn,wr-1,ws); ICPY(AS(z)+f,ws+f+1,r-1);
 GATV(x,BOX,n,1,0); xv=AAV(x);
 zv=AAV(z); wv=AAV(w); wd=(I)w*ARELATIVE(w);
 DO(zm, u=wv; DO(m, v=u++; DO(n, xv[i]=AADR(wd,*v); v+=m;); RZ(*zv++=raze(x));); wv+=mn;);
 R z;
}    /* ,&.>/"r w */

static DF2(jtoprod){R df2(a,w,VAV(self)->h);}


F1(jtslash){A h;AF f1=jtreduce;C c;V*v;
 RZ(w);
 if(NOUN&AT(w))R evger(w,sc(GINSERT));
 v=VAV(w); 
 switch(v->id){
  case CCOMMA:  f1=jtredcat;    break;
  case CCOMDOT: f1=jtredstitch; break;
  case CSEMICO: f1=jtredsemi;   break;
  case CUNDER:  if(COPE==ID(v->g)){c=ID(v->f); if(c==CCOMMA)f1=jtredcateach; else if(c==CCOMDOT)f1=jtredstiteach;}
 }
 RZ(h=qq(w,v2(lr(w),RMAX)));
 R fdef(CSLASH,VERB, f1,jtoprod, w,0L,h, VAV(ds(CSLASH))->flag, RMAX,RMAX,RMAX);
}

A jtaslash (J jt,C c,    A w){RZ(   w); R df1(  w,   slash(ds(c))     );}
A jtaslash1(J jt,C c,    A w){RZ(   w); R df1(  w,qq(slash(ds(c)),one));}
A jtatab   (J jt,C c,A a,A w){RZ(a&&w); R df2(a,w,   slash(ds(c))     );}


static AHDRR(jtmeanD,D,D){I d,i;D*y;D v,*zz;
 d=c/n;
 NAN0;
 if(1==d)DO(m, v=   *x++; DO(n-1, v+=*x++;); *z++=v/n;)
 else for(i=0;i<m;++i){
  y=x; x+=d; zz=z; DO(d, *z++ =*x+++   *y++;);
  DO(n-3,    z=zz; DO(d, *z+++=*x++;        ));
             z=zz; DO(d, *z   =(*z+*x++)/n; ++z;);
 }
 NAN1V;
}    /* based on REDUCEPFX; 2<n */

static AHDRR(jtmeanI,D,I){I d,i;I*y;D v,*zz;
 d=c/n;
 if(1==d)DO(m, v=(D)*x++; DO(n-1, v+=*x++;); *z++=v/n;)
 else for(i=0;i<m;++i){
  y=x; x+=d; zz=z; DO(d, *z++ =*x+++(D)*y++;);
  DO(n-3,    z=zz; DO(d, *z+++=*x++;        ));
             z=zz; DO(d, *z   =(*z+*x++)/n; ++z;);
}}   /* based on REDUCEPFX; 2<n */

DF1(jtmean){A z;I c,f,m,n,r,wn,wr,*ws,wt;
 RZ(w);
 wr=AR(w); r=jt->rank?jt->rank[1]:wr; f=wr-r; jt->rank=0;
 wt=AT(w); wn=AN(w); ws=AS(w); n=r?ws[f]:1;
 if(!(wn&&2<n&&wt&INT+FL))R divide(df1(w,qq(slash(ds(CPLUS)),sc(r))),sc(n));
 // there must be atoms, so it's OK to PROD infixes of shape
 GATV(z,FL,wn/n,MAX(0,wr-1),ws); if(1<r)ICPY(f+AS(z),f+1+ws,r-1);
 PROD(m,f,ws); PROD(c,r,f+ws);
 if(wt&INT)meanI(m,c,n,DAV(z), AV(w)); 
 else      meanD(m,c,n,DAV(z),DAV(w));
 RE(0); R z;
}    /* (+/%#)"r w */
