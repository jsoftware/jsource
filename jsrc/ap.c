/* Copyright 1990-2011, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Prefix and Infix                                               */

#include "j.h"
#include "vasm.h"
#include "ve.h"


#define MINUSPA(b,r,u,v)  r=b?u-v:u+v;
#define MINUSPZ(b,r,u,v)  if(b)r=zminus(u,v); else r=zplus(u,v);
#define MINUSPX(b,r,u,v)  if(b)r=xminus(u,v); else r=xplus(u,v);
#define MINUSPQ(b,r,u,v)  if(b)r=qminus(u,v); else r=qplus(u,v);

#define DIVPA(b,r,u,v)    r=b?(!u&&!v?0:u/(D)v):u*v;
#define DIVPZ(b,r,u,v)    if(b)r=zdiv(u,v); else r=ztymes(u,v);

#define PREFIXPFX(f,Tz,Tx,pfx)  \
 AHDRP(f,Tz,Tx){I i;Tz v,*y;                                    \
  if(c==n)DO(m, *z++=v=    *x++; DO(n-1, *z=v=pfx(v,*x); ++z; ++x;))  \
  else{I d=c/n; for(i=0;i<m;++i){                                              \
   y=z; DO(d, *z++=    *x++;);                                        \
   DO(n-1, DO(d, *z=pfx(*y,*x); ++z; ++x; ++y;));                     \
 }}}  /* for associative functions only */

#define PREFIXNAN(f,Tz,Tx,pfx)  \
 AHDRP(f,Tz,Tx){I i;Tz v,*y;                                    \
  NAN0;                                                               \
  if(c==n)DO(m, *z++=v=    *x++; DO(n-1, *z=v=pfx(v,*x); ++z; ++x;))  \
  else{I d=c/n; for(i=0;i<m;++i){                                              \
   y=z; DO(d, *z++=    *x++;);                                        \
   DO(n-1, DO(d, *z=pfx(*y,*x); ++z; ++x; ++y;));                     \
  }}                                                                   \
  NAN1V;                                                              \
 }   /* for associative functions only */

#define PREFICPFX(f,Tz,Tx,pfx)  \
 AHDRP(f,Tz,Tx){I i;Tz v,*y;                                    \
  if(c==n)DO(m, *z++=v=(Tz)*x++; DO(n-1, *z=v=pfx(v,*x); ++z; ++x;))  \
  else{I d=c/n; for(i=0;i<m;++i){                                              \
   y=z; DO(d, *z++=(Tz)*x++;);                                        \
   DO(n-1, DO(d, *z=pfx(*y,*x); ++z; ++x; ++y;));                     \
 }}}  /* for associative functions only */

#define PREFIXALT(f,Tz,Tx,pfx)  \
 AHDRP(f,Tz,Tx){B b;I i;Tz v,*y;                                                 \
  if(c==n)DO(m, *z++=v=    *x++; b=0; DO(n-1, b=!b; pfx(b,*z,v,*x); v=*z; ++z; ++x;))  \
  else{I d=c/n; for(i=0;i<m;++i){                                                               \
   y=z; DO(d, *z++=    *x++;); b=0;                                                    \
   DO(n-1, b=!b; DO(d, pfx(b,*z,*y,*x); ++z; ++x; ++y;));                              \
 }}}

#define PREALTNAN(f,Tz,Tx,pfx)  \
 AHDRP(f,Tz,Tx){B b;I i;Tz v,*y;                                                 \
  NAN0;                                                                                \
  if(c==n)DO(m, *z++=v=    *x++; b=0; DO(n-1, b=!b; pfx(b,*z,v,*x); v=*z; ++z; ++x;))  \
  else{I d=c/n; for(i=0;i<m;++i){                                                               \
   y=z; DO(d, *z++=    *x++;); b=0;                                                    \
   DO(n-1, b=!b; DO(d, pfx(b,*z,*y,*x); ++z; ++x; ++y;));                              \
  }}                                                                                    \
  NAN1V;                                                                               \
 }

#define PREFICALT(f,Tz,Tx,pfx)  \
 AHDRP(f,Tz,Tx){B b;I i;Tz v,*y;                                                 \
  if(c==n)DO(m, *z++=v=(Tz)*x++; b=0; DO(n-1, b=!b; pfx(b,*z,v,*x); v=*z; ++z; ++x;))  \
  else{I d=c/n; for(i=0;i<m;++i){                                                               \
   y=z; DO(d, *z++=(Tz)*x++;); b=0;                                                    \
   DO(n-1, b=!b; DO(d, pfx(b,*z,*y,*x); ++z; ++x; ++y;));                              \
 }}}

#define PREFIXOVF(f,Tz,Tx,fp1,fvv)  \
 AHDRP(f,I,I){C er=0;I i,*xx=x,*y,*zz=z;                      \
  if(c==n){                                                         \
   if(1==n)DO(m, *z++=*x++;)                                        \
   else    DO(m, fp1(n,z,x); RER; z=zz+=c; x=xx+=c;)                \
  }else{I d=c/n; for(i=0;i<m;++i){                                           \
   y=z; DO(d, *z++=*x++;); zz=z; xx=x;                              \
   DO(n-1, fvv(d,z,y,x); RER; x=xx+=d; y=zz; z=zz+=d;);             \
 }}}

  
#if SY_ALIGN
#define PREFIXBFXLOOP(T,pfx)  \
 {T*xx=(T*)x,*yy,*zz=(T*)z;  \
  q=d/sizeof(T);             \
  DO(m, yy=zz; DO(q, *zz++=*xx++;); DO(n-1, DO(q, *zz++=pfx(*yy,*xx); ++xx; ++yy;)));  \
 }

#define PREFIXBFX(f,pfx,ipfx,spfx,bpfx,vexp)          \
 AHDRP(f,B,B){B*y;I d=c/n,j,q;                        \
  if(1==d)for(j=0;j<m;++j){vexp}                      \
  else if(0==d%sizeof(UI  ))PREFIXBFXLOOP(UI,   pfx)  \
  else if(0==d%sizeof(UINT))PREFIXBFXLOOP(UINT,ipfx)  \
  else if(0==d%sizeof(US  ))PREFIXBFXLOOP(US,  spfx)  \
  else DO(m, y=z; DO(d, *z++=*x++;); DO(n-1, DO(d, *z++=bpfx(*y,*x); ++x; ++y;)));  \
 }    /* f/\"r z for boolean associative atomic function f */
#else
#define PREFIXBFX(f,pfx,ipfx,spfx,bpfx,vexp)          \
 AHDRP(f,B,B){B*tv;I d,i,q,r,t,*xi,*yi,*zi;                      \
  d=c/n; q=d/SZI; r=d%SZI; xi=(I*)x; zi=(I*)z; tv=(B*)&t;        \
  if(1==d)   for(r=0;r<m;++r){vexp}                              \
  else if(!r)for(i=0;i<m;++i){                                   \
   yi=zi; DO(q, *zi++=*xi++;);                                   \
   DO(n-1, DO(q, *zi=pfx(*yi,*xi); ++zi; ++xi; ++yi;));          \
  }else for(i=0;i<m;++i){                                        \
   yi=zi; MC(zi,xi,d);                                           \
   xi=(I*)((B*)xi+d);                                            \
   zi=(I*)((B*)zi+d);                                            \
   DO(n-1, DO(q, *zi=pfx(*yi,*xi); ++zi; ++xi; ++yi;);           \
    t=pfx(*yi,*xi); z=(B*)zi; DO(r, z[i]=tv[i];);                \
    xi=(I*)(r+(B*)xi);                                           \
    yi=(I*)(r+(B*)yi);                                           \
    zi=(I*)(r+(B*)zi); );                                        \
 }}   /* f/\"r z for boolean associative atomic function f */
#endif

#define BFXANDOR(c0,c1)  \
 {B*y=memchr(x,c0,n); if(y){q=y-x; memset(z,c1,q); memset(z+q,c0,n-q);}else memset(z,c1,n); x+=c; z+=c;}

PREFIXBFX( orpfxB, OR, IOR, SOR, BOR,  BFXANDOR(C1,C0))
PREFIXBFX(andpfxB, AND,IAND,SAND,BAND, BFXANDOR(C0,C1))
PREFIXBFX( nepfxB, NE, INE, SNE, BNE, {B b=0; DO(n, *z++=b^=  *x++;);})
PREFIXBFX( eqpfxB, EQ, IEQ, SEQ, BEQ, {B b=1; DO(n, *z++=b=b==*x++;);})


static B jtpscanlt(J jt,I m,I c,I n,B*z,B*x,B p){A t;B*v;I d,i;
 d=c/n; memset(z,!p,m*c); 
 if(1==d)DO(m, if(v=memchr(x,p,n))*(z+(v-x))=p; z+=c; x+=c;)
 else{
  GATV(t,B01,d,1,0); v=BAV(t);
  for(i=0;i<m;++i){
   memset(v,C1,d);
   DO(n, DO(d, if(v[i]&&p==x[i]){v[i]=0; z[i]=p;};); z+=d; x+=d;); 
 }}
 R 1;
}    /* f/\"1 w for < and <: */

AHDRP(ltpfxB,B,B){pscanlt(m,c,n,z,x,C1);}
AHDRP(lepfxB,B,B){pscanlt(m,c,n,z,x,C0);}


static B jtpscangt(J jt,I m,I c,I n,B*z,B*x,B a,B pp,B pa,B ps){
  A t;B b,*cc="\000\001\000",e,*p=cc+pp,*v;C*u;I d,i,j;
 if(c==n)for(i=0;i<m;++i){
  if(v=memchr(x,a,n)){
   j=v-x; b=1&&j%2; 
   mvc(j,z,2L,p); memset(z+j,b!=ps,n-j); *(z+j)=b!=pa;
  }else mvc(n,z,2L,p);
  z+=c; x+=c;
 }else{
  d=c/n; GATV(t,B01,d,1,0); u=BAV(t);
  for(i=0;i<m;++i){
   e=pp; memset(u,C0,d);
   DO(n, j=i; DO(d, if(u[i])z[i]='1'==u[i]; else 
     if(a==x[i]){b=1&&j%2; z[i]=b!=pa; u[i]=b!=ps?'1':'0';}else z[i]=e;);
    e=!e; z+=d; x+=d;); 
 }}
 R 1;
}    /* f/\"1 w for > >: +: *: */

AHDRP(  gtpfxB,B,B){pscangt(m,c,n,z,x,C0,C1,C0,C0);}
AHDRP(  gepfxB,B,B){pscangt(m,c,n,z,x,C1,C0,C1,C1);}
AHDRP( norpfxB,B,B){pscangt(m,c,n,z,x,C1,C0,C1,C0);}
AHDRP(nandpfxB,B,B){pscangt(m,c,n,z,x,C0,C1,C0,C1);}


PREFIXOVF( pluspfxI, I, I,  PLUSP, PLUSVV)
PREFIXOVF(tymespfxI, I, I, TYMESP,TYMESVV)

AHDRP(minuspfxI,I,I){C er=0;I d=c/n,i,j,n1=n-1,*xx=x,*y,*zz=z;
 if(1==d){
  if(1==n)DO(m, *z++=*x++;)
  else    DO(m, MINUSP(n,z,x); RER; z=zz+=c; x=xx+=c;);
 }else for(i=0;i<m;++i){                               
  y=z; DO(d, *z++=*x++;); zz=z; xx=x; j=0;
  DO(n1, MINUSVV(d,z,y,x); RER; x=xx+=d; y=zz; z=zz+=d; if(n1<=++j)break;
          PLUSVV(d,z,y,x); RER; x=xx+=d; y=zz; z=zz+=d; if(n1<=++j)break;);
}}

PREFICPFX( pluspfxO, D, I,  PLUS   )
PREFICPFX(tymespfxO, D, I,  TYMES  )
PREFICALT(minuspfxO, D, I,  MINUSPA)

PREFIXPFX( pluspfxB, I, B,  PLUS   )
PREFIXNAN( pluspfxD, D, D,  PLUS   )
PREFIXNAN( pluspfxZ, Z, Z,  zplus  )
PREFIXPFX( pluspfxX, X, X,  xplus  )
PREFIXPFX( pluspfxQ, Q, Q,  qplus  )

PREFIXPFX(tymespfxD, D, D,  TYMES  )
PREFIXPFX(tymespfxZ, Z, Z,  ztymes )
PREFIXPFX(tymespfxX, X, X,  xtymes )
PREFIXPFX(tymespfxQ, Q, Q,  qtymes )

PREFIXALT(minuspfxB, I, B,  MINUSPA)
PREALTNAN(minuspfxD, D, D,  MINUSPA)
PREALTNAN(minuspfxZ, Z, Z,  MINUSPZ)
PREFIXALT(minuspfxX, X, X,  MINUSPX)
PREFIXALT(minuspfxQ, Q, Q,  MINUSPQ)

PREALTNAN(  divpfxD, D, D,  DIVPA  )
PREALTNAN(  divpfxZ, Z, Z,  DIVPZ  )

PREFIXPFX(  maxpfxI, I, I,  MAX    )
PREFIXPFX(  maxpfxD, D, D,  MAX    )
PREFIXPFX(  maxpfxX, X, X,  XMAX   )
PREFIXPFX(  maxpfxQ, Q, Q,  QMAX   )
PREFIXPFX(  maxpfxS, SB,SB, SBMAX  )

PREFIXPFX(  minpfxI, I, I,  MIN    )
PREFIXPFX(  minpfxD, D, D,  MIN    )
PREFIXPFX(  minpfxX, X, X,  XMIN   )
PREFIXPFX(  minpfxQ, Q, Q,  QMIN   )
PREFIXPFX(  minpfxS, SB,SB, SBMIN  )

PREFIXPFX(bw0000pfxI, UI,UI, BW0000)
PREFIXPFX(bw0001pfxI, UI,UI, BW0001)
PREFIXPFX(bw0011pfxI, UI,UI, BW0011)
PREFIXPFX(bw0101pfxI, UI,UI, BW0101)
PREFIXPFX(bw0110pfxI, UI,UI, BW0110)
PREFIXPFX(bw0111pfxI, UI,UI, BW0111)
PREFIXPFX(bw1001pfxI, UI,UI, BW1001)
PREFIXPFX(bw1111pfxI, UI,UI, BW1111)


static DF1(jtprefix){DECLF;I r;
 RZ(w);
 if(jt->rank&&jt->rank[1]<AR(w)){r=jt->rank[1]; jt->rank=0; R rank1ex(w,self,r,jtprefix);}
 jt->rank=0;
 R eachl(apv(IC(w),1L,1L),w,atop(fs,ds(CTAKE)));
}    /* f\"r w for general f */

static DF1(jtgprefix){A h,*hv,z,*zv;I m,n,r;
 RZ(w);
 ASSERT(DENSE&AT(w),EVNONCE);
 if(jt->rank&&jt->rank[1]<AR(w)){r=jt->rank[1]; jt->rank=0; R rank1ex(w,self,r,jtgprefix);}
 jt->rank=0;
 n=IC(w); 
 h=VAV(self)->h; hv=AAV(h); m=AN(h);
 GATV(z,BOX,n,1,0); zv=AAV(z);
 DO(n, RZ(zv[i]=df1(take(sc(1+i),w),hv[i%m])););
 R ope(z);
}    /* g\"r w for gerund g */

//  f/\"r y    w is y, fs is in self
static DF1(jtpscan){A y,z;C id;I c,cv,f,m,n,r,rr[2],t,wn,wr,*ws,wt,zt;VF ado;
 RZ(w);
 wt=AT(w);   // get type of w
 if(SPARSE&wt)R scansp(w,self,jtpscan);  // if sparse, go do it separately
 // wn = #atoms in w, wr=rank of w, r=effective rank, f=length of frame, ws->shape of w
 wn=AN(w); wr=AR(w); r=jt->rank?jt->rank[1]:wr; f=wr-r; ws=AS(w);
 // m = #cells, c=#atoms/cell, n = #items per cell
 PROD(m,f,ws); c=m?wn/m:prod(r,f+ws); n=r?ws[f]:1;  // wn=0 doesn't matter
 // y is the verb u; id=pseudocharacter for it
 y=VAV(self)->f; id=vaid(VAV(y)->f);
 // If there are 0 or 1 items, return the input unchanged, except: if rank 0, return (($w),1)($,)w
 if(2>n||!wn){if(id){jt->rank=0; R r?ca(w):reshape(over(shape(w),one),w);}else R prefix(w,self);}
 vapfx(id,wt,&ado,&cv);
 if(!ado)R prefix(w,self);
 if((t=atype(cv))&&TYPESNE(t,wt))RZ(w=cvt(t,w));
 zt=rtype(cv); jt->rank=0;
 GA(z,zt,wn,wr,ws);
 ado(jt,m,c,n,AV(z),AV(w));
 if(jt->jerr)R (jt->jerr>=EWOV)?(rr[1]=r,jt->rank=rr,pscan(w,self)):0; else R cv&VRI+VRD?cvz(cv,z):z;
}    /* f/\"r w atomic f main control */

// block a contains (start,length) of infix.  w is the A for the data.
// Result is new block containing the extracted infix
static F2(jtseg){A z;I c,k,m,n,*u,zn;
 RZ(a&&w);
 // The (start,length) had better be integers.  Extract them into m,n
 if(INT&AT(a)){u=AV(a); m=*u; n=*(1+u);} else m=n=0;
 c=aii(w); k=c*bp(AT(w)); RE(zn=mult(n,c));  // c=#atoms per item, k=#bytes/item, zn=atoms/infix
 GA(z,AT(w),zn,MAX(1,AR(w)),AS(w)); *AS(z)=n;  // Allocate array of items, move in shape, override # items
 // Copy the selected items to the new block and return the new block
 if(ARELATIVE(w)){A*u=AAV(z),*v=AAV(w)+m; DO(n, *u++=(A)AABS(w,*v++););}
 else MC(AV(z),CAV(w)+m*k,n*k);
 R z;
}

// m is the infix length (x), w is the array (y)
// Result is A for an nx2 table of (starting item#,length) for each infix
static A jtifxi(J jt,I m,A w){A z;I d,j,k,n,p,*x;
 RZ(w);
 // p=|m, n=#items of w, d=#applications of u (depending on overlapping/nonoverlapping)
 p=ABS(m); n=IC(w); d=0>m?(I)((n+(D)p-1)/p):MAX(0,1+n-m);
 // Allocate result, a dx2 table; install shape
 GATV(z,INT,2*d,2,0); *AS(z)=d; *(1+AS(z))=2;
 // x->result area; k=stride between infixes; j=starting index (prebiased); copy (index,length) for each infix;
 // repair last length if it runs off the end
 x=AV(z); k=0>m?p:1; j=-k; DO(d, *x++=j+=k; *x++=p;); if(d)*--x=MIN(p,n-j);
 R z;
}

// Entry point for infix.  a is x, w is y, fs points to u
static DF2(jtinfix){PROLOG(0018);DECLF;A x,z;I m; 
 PREF2(jtinfix); // Handle looping over rank.  This returns here for each cell (including this test)
 // The rest of this verb handles a single cell
 // If length is infinite, convert to large integer
 // kludge - test for ==ainf should be replaced with a test for value; will fail if _ is result of expression like {._
 if(a==ainf)m=IMAX;
 else RE(m=i0(vib(a))); // get infix length as an integer
 // Create table of infix positions
 RZ(x=ifxi(m,w));
 // If there are infixes, apply fs@:jtseg (ac2 creates an A verb for jtseg)
 if(*AS(x))z=eachl(x,w,atop(fs,ac2(jtseg)));
 else{A s;I r, rr;
  // No infixes.  Create a cell of fills, apply fs to it, add a leading axis of 0 to the result
  // create a block containing the shape of the fill-cell.  The fill-cell is a list of items of y,
  // with the number of items being the infix-size if positive, or 0 if negative
  // r = rank of w, rr=rank of list of items of w, s is block for list of length rr; copy shape of r; override #items of infix
  r=AR(w); rr=MAX(1,r); GATV(s,INT,rr,1,0); if(r)ICPY(AV(s),AS(w),r); *AV(s)=0>m?0:m==IMAX?1+IC(w):m;
  // Create fill-cell of shape s; apply u to it
  RZ(x=df1(reshape(s,filler(w)),fs));
  // Prepend leading axis of 0 to the result
  z=reshape(over(zero,shape(x)),x);
 } 
 EPILOG(z);
}

static DF2(jtinfix2){PROLOG(0019);A f,x,y;B af,d;I c,m,n,n1,r,*s,t; 
 PREF2(jtinfix); 
 RE(m=i0(vib(a))); t=AT(w); n=IC(w); 
 if(!(2==m&&2<=n&&t&DENSE))R infix(a,w,self);
 f=VAV(self)->f; f=VAV(f)->f;
 af=vaid(f); d=af&&t&DIRECT;
 c=AN(w)/n; r=AR(w); s=AS(w); n1=n-1;
 if(d             ){RZ(x=gah(r,w)); ICPY(AS(x),s,r); *AS(x)=n1; AN(x)=c*n1;} 
 else RZ(x=curtail(w));
 if(d&!(t&IS1BYTE)){RZ(y=gah(r,w)); ICPY(AS(y),s,r); *AS(y)=n1; AN(y)=c*n1; AK(y)=AK(w)+(I)w+c*bp(t)-(I)y;}
 else RZ(y= behead(w));
 A z=df2(x,y,af?f:qq(f,num[-1]));
 EPILOG(z);
}    /* 2 f/\w */

static DF2(jtginfix){A h,*hv,x,z,*zv;I d,m,n;
 RE(m=i0(vib(a))); 
 RZ(x=ifxi(m,w));
 h=VAV(self)->h; hv=AAV(h); d=AN(h);
 if(n=IC(x)){
  GATV(z,BOX,n,1,0); zv=AAV(z);
  DO(n, RZ(zv[i]=df1(seg(from(sc(i),x),w),hv[i%d])););
  R ope(z);
 }else{A s;
  RZ(s=AR(w)?shape(w):ca(iv0)); *AV(s)=ABS(m);
  RZ(x=df1(reshape(s,filler(w)),*hv));
  R reshape(over(zero,shape(x)),x);
}}

#define MCREL(uu,vv,n)  {A*u=(A*)(uu),*v=(A*)(vv); DO((n), *u++=(A)AABS(wd,*v++););}

static DF2(jtinfixd){A fs,z;C*x,*y;I c=0,d,k,m,n,p,q,r,*s,wd,wr,*ws,wt,zc; 
 F2RANK(0,RMAX,jtinfixd,self);
 wr=AR(w); ws=AS(w); wt=AT(w); n=IC(w);
 RE(m=i0(vib(a))); if(m==IMAX){m=n+1;} p=m==IMIN?IMAX:ABS(m);
 if(0>m){p=MIN(p,n); d=p?(n+p-1)/p:0;}else{ASSERT(IMAX-1>n-m,EVDOMAIN); d=MAX(0,1+n-m);}
 if(fs=VAV(self)->f,CCOMMA==ID(fs)){RE(c=aii(w)); RE(zc=mult(p,c)); r=2;}
 else{if(n)RE(c=aii(w)); zc=p; r=wr?1+wr:2;}
 GA(z,wt,d*p*c,r,0); x=CAV(z); y=CAV(w);
 s=AS(z); *s++=d; *s++=zc; ICPY(s,1+ws,r-2);
 k=c*bp(wt); wd=(I)w*ARELATIVE(w);
 if(AN(z))switch((0>m?2:0)+(wd?1:0)){
  case 0: q=p*k; DO(d, MC(x,y,q);    x+=q; y+=k;);      break;
  case 1: q=p*k; DO(d, MCREL(x,y,p); x+=q; y+=k;);      break;  
  case 2: MC(x,y,n*k);  if(q=d*p-n)fillv(wt,q*c,x+n*k); break;
  case 3: MCREL(x,y,n); if(q=d*p-n)fillv(wt,q*c,x+n*k); break;
 }
 R z;
}    /* a[\w and a]\w and a,\w */


#define SETZ    {s=yv; DO(c, *zv++=*s++;  );}
#define SETZD   {s=yv; DO(c, *zv++=*s++/d;);}

#define MOVSUMAVG(Tw,Ty,ty,Tz,tz,xd,SET)  \
 {Tw*u,*v;Ty*s,x=0,*yv;Tz*zv;                                  \
  GATVS(z,tz,c*(1+p),AR(w),AS(w),tz##SIZE); *AS(z)=1+p;                    \
  zv=(Tz*)AV(z); u=v=(Tw*)AV(w);                               \
  if(1==c){                                                    \
   DO(m, x+=*v++;); *zv++=xd;                                  \
   DO(p, x+=(Ty)*v++-(Ty)*u++; *zv++=xd;);                     \
  }else{                                                       \
   GATVS(y,ty,c,1,0,ty##SIZE); s=yv=(Ty*)AV(y); DO(c, *s++=0;);            \
   DO(m, s=yv; DO(c, *s+++=*v++;);); SET;                      \
   DO(p, s=yv; DO(c, x=*s+++=(Ty)*v++-(Ty)*u++; *zv++=xd;););  \
 }}

static A jtmovsumavg1(J jt,I m,A w,A fs,B avg){A y,z;D d=(D)m;I c,p,s,t,wt;
 p=IC(w)-m; wt=AT(w); c=aii(w);
 switch((wt&B01?0:wt&INT?2:4)+avg){
  case 0:       MOVSUMAVG(B,I,INT,I,INT,x,  SETZ ); break;
  case 1:       MOVSUMAVG(B,I,INT,D,FL, x/d,SETZD); break;
  case 2: 
   irange(AN(w),AV(w),&s,&t); t=0<t&&IMAX>=d*((D)s+(D)t);
   if(t)        MOVSUMAVG(I,I,INT,I,INT,x,  SETZ )
   else         MOVSUMAVG(I,D,FL, D,FL, x,  SETZ ); break;
  case 3:       MOVSUMAVG(I,D,FL, D,FL, x/d,SETZD); break;
  case 4: NAN0; MOVSUMAVG(D,D,FL, D,FL, x,  SETZ ); NAN1; break;
  case 5: NAN0; MOVSUMAVG(D,D,FL, D,FL, x/d,SETZD); NAN1; break;
 }
 R z;
}    /* m +/\w or (if 0=avg) m (+/%#)\w (if 1=avg); bool or integer or float; 0<m */

static A jtmovsumavg(J jt,I m,A w,A fs,B avg){A z;
 z=movsumavg1(m,w,fs,avg);
 if(jt->jerr==EVNAN)RESETERR else R z;
 R infix(sc(m),w,fs);
}

static DF2(jtmovavg){I m;
 PREF2(jtmovavg);
 RE(m=i0(vib(a)));
 if(0<m&&m<=IC(w)&&AT(w)&B01+FL+INT)R movsumavg(m,w,self,1); 
 R infix(a,w,self);
}    /* a (+/ % #)\w */

#define MOVMINMAX(T,type,ie,CMP)    \
 {T d,e,*s,*t,*u,*v,x=ie,*yv,*zv;                              \
  zv=(T*)AV(z); u=v=(T*)AV(w);                                 \
  if(1==c){                                                    \
   DO(m, d=*v++; if(d CMP x)x=d;); *zv++=x;                    \
   for(i=0;i<p;++i){                                           \
    d=*v++; e=*u++;                                            \
    if(d CMP x)x=d; else if(e==x){x=d; t=u; DO(m-1, e=*t++; if(e CMP x)x=e;);}  \
    *zv++=x;                                                   \
  }}else{                                                      \
   GATVS(y,type,c,1,0,type##SIZE); s=yv=(T*)AV(y); DO(c, *s++=ie;);          \
   DO(m, s=yv; DO(c, d=*v++; if(d CMP *s)*s=d; ++s;);); SETZ;  \
   for(i=0;i<p;++i){                                           \
    for(j=0,s=yv;j<c;++j,++s){                                 \
     d=*v++; e=*u++; x=*s;                                     \
     if(d CMP x)x=d; else if(e==x){x=d; t=c+u-1; DO(m-1, e=*t; t+=c; if(e CMP x)x=e;);}  \
     *s=x;                                                     \
    }                                                          \
    SETZ;                                                      \
 }}}

#define MOVMINMAXS(T,type,ie,CMP)    \
 {T d,e,*s,*t,*u,*v,x=ie,*yv,*zv;                              \
  zv=(T*)AV(z); u=v=(T*)AV(w);                                 \
  if(1==c){                                                    \
   DO(m, d=*v++; if(CMP(d,x))x=d;); *zv++=x;                    \
   for(i=0;i<p;++i){                                           \
    d=*v++; e=*u++;                                            \
    if(CMP(d,x))x=d; else if(e==x){x=d; t=u; DO(m-1, e=*t++; if(CMP(e,x))x=e;);}  \
    *zv++=x;                                                   \
  }}else{                                                      \
   GATVS(y,type,c,1,0,type##SIZE); s=yv=(T*)AV(y); DO(c, *s++=ie;);          \
   DO(m, s=yv; DO(c, d=*v++; if(CMP(d,*s))*s=d; ++s;);); SETZ;  \
   for(i=0;i<p;++i){                                           \
    for(j=0,s=yv;j<c;++j,++s){                                 \
     d=*v++; e=*u++; x=*s;                                     \
     if(CMP(d,x))x=d; else if(e==x){x=d; t=c+u-1; DO(m-1, e=*t; t+=c; if(CMP(e,x))x=e;);}  \
     *s=x;                                                     \
    }                                                          \
    SETZ;                                                      \
 }}}

static A jtmovminmax(J jt,I m,A w,A fs,B max){A y,z;I c,i,j,p,wt;
 p=IC(w)-m; wt=AT(w); c=aii(w);
 GA(z,AT(w),c*(1+p),AR(w),AS(w)); *AS(z)=1+p;
 switch(max+(wt&SBT?0:wt&INT?2:4)){
  case 0: MOVMINMAXS(SB,SBT,jt->sbuv[0].down,SBLE); break;
  case 1: MOVMINMAXS(SB,SBT,0,SBGE); break;
  case 2: MOVMINMAX(I,INT,IMAX,<=); break;
  case 3: MOVMINMAX(I,INT,IMIN,>=); break;
  case 4: MOVMINMAX(D,FL, inf ,<=); break;
  case 5: MOVMINMAX(D,FL, infm,>=); break;
 }
 R z;
}    /* a <./\w (0=max) or a >./\ (1=max); vector w; integer or float; 0<m */

static A jtmovandor(J jt,I m,A w,A fs,B or){A y,z;B b0,b1,d,e,*s,*t,*u,*v,x,*yv,*zv;I c,i,j,p;
 p=IC(w)-m; c=aii(w); x=b0=!or; b1=or;
 GATV(z,B01,c*(1+p),AR(w),AS(w)); *AS(z)=1+p;
 zv=BAV(z); u=v=BAV(w);
 if(1==c){
  DO(m, if(b1==*v++){x=b1; break;}); *zv++=x; v=u+m;
  for(i=0;i<p;++i){
   d=*v++; e=*u++;
   if(d==b1)x=d; else if(e==b1){x=d; t=u; DO(m-1, if(b1==*t++){x=b1; break;});}
   *zv++=x;
 }}else{
  GATV(y,B01,c,1,0); s=yv=BAV(y); DO(c, *s++=b0;);
  DO(m, s=yv; DO(c, if(b1==*v++)*s=b1; ++s;);); SETZ;
  for(i=0;i<p;++i){
   for(j=0,s=yv;j<c;++j,++s){
    d=*v++; e=*u++; x=*s;
    if(d==b1)x=d; else if(e==b1){x=d; t=c+u-1; DO(m-1, e=*t; t+=c; if(b1==e){x=b1; break;});}
    *s=x;
   }
   SETZ;
 }}
 R z;
}    /* a *./\w (0=or) or a +./\ (1=or); boolean w; 0<m */

static A jtmovbwandor(J jt,I m,A w,A fs,B or){A z;I c,p,*s,*t,*u,x,*zv;
 p=IC(w)-m; c=aii(w);
 GATV(z,INT,c*(1+p),AR(w),AS(w)); *AS(z)=1+p;
 zv=AV(z); u=AV(w);
 if(c)switch(or+(1==c?0:2)){
  case 0: DO(1+p, x=*u++; t=u; DO(m-1, x&=*t++;); *zv++=x;); break;
  case 1: DO(1+p, x=*u++; t=u; DO(m-1, x|=*t++;); *zv++=x;); break;
  case 2: DO(1+p, ICPY(zv,u,c); t=u+=c; DO(m-1, s=zv; DO(c, *s++&=*t++;);); zv+=c;); break;
  case 3: DO(1+p, ICPY(zv,u,c); t=u+=c; DO(m-1, s=zv; DO(c, *s++|=*t++;);); zv+=c;); break;
 }
 R z;
}    /* a 17 b./\w (0=or) or a 23 b./\ (1=or); integer w; 0<m */

static A jtmovneeq(J jt,I m,A w,A fs,B eq){A y,z;B*s,*u,*v,x,*yv,*zv;I c,p;
 p=IC(w)-m; c=aii(w); x=eq;
 GATV(z,B01,c*(1+p),AR(w),AS(w)); *AS(z)=1+p;
 zv=BAV(z); u=v=BAV(w);
 if(1<c){GATV(y,B01,c,1,0); s=yv=BAV(y); DO(c, *s++=eq;);}
 switch(eq+(1<c?2:0)){
  case 0: DO(m,                   x   ^=   *v++;  ); *zv++=x; DO(p,                   *zv++=x   ^=   *u++^ *v++;  ); break;
  case 1: DO(m,                   x    =x==*v++;  ); *zv++=x; DO(p,                   *zv++=x    =x==*u++==*v++;  ); break;
  case 2: DO(m, s=yv; DO(c,       *s++^=   *v++;);); SETZ;    DO(p, s=yv; DO(c,       *zv++=*s++^=   *u++^ *v++;);); break;
  case 3: DO(m, s=yv; DO(c, x=*s; *s++ =x==*v++;);); SETZ;    DO(p, s=yv; DO(c, x=*s; *zv++=*s++ =x==*u++==*v++;););
 }
 R z;
}    /* m ~:/\w (0=eq) or m =/\ (1=eq); boolean w; 0<m */

static A jtmovbwneeq(J jt,I m,A w,A fs,B eq){A y,z;I c,p,*s,*u,*v,x,*yv,*zv;
 p=IC(w)-m; c=aii(w); x=eq?-1:0;
 GATV(z,INT,c*(1+p),AR(w),AS(w)); *AS(z)=1+p;
 zv=AV(z); u=v=AV(w);
 if(1<c){GATV(y,INT,c,1,0); s=yv=AV(y); DO(c, *s++=x;);}
 switch(eq+(1<c?2:0)){
  case 0: DO(m,                   x   ^=    *v++ ;  ); *zv++=x; DO(p,                   *zv++=x   ^=      *u++^*v++  ;  ); break;
  case 1: DO(m,                   x    =~(x^*v++);  ); *zv++=x; DO(p,                   *zv++=x    =~(x^~(*u++^*v++));  ); break;
  case 2: DO(m, s=yv; DO(c,       *s++^=    *v++ ;);); SETZ;    DO(p, s=yv; DO(c,       *zv++=*s++^=      *u++^*v++  ;);); break;
  case 3: DO(m, s=yv; DO(c, x=*s; *s++ =~(x^*v++););); SETZ;    DO(p, s=yv; DO(c, x=*s; *zv++=*s++ =~(x^~(*u++^*v++));););
 }
 R z;
}    /* m 22 b./\w (0=eq) or m 25 b./\ (1=eq); integer w; 0<m */

static DF2(jtmovfslash){A x,z;B b;C id,*wv,*zv;I c,cm,cv,d,m,m0,p,t,wk,wt,zk,zt;VF ado;
 PREF2(jtmovfslash);
 p=IC(w); wt=AT(w);
 RE(m0=i0(vib(a))); m=0<=m0?m0:m0==IMIN?p:MIN(p,-m0); 
 if(2==m0)R infix2(a,w,self);
 x=VAV(self)->f; x=VAV(x)->f; id=ID(x); 
 if(wt&B01)id=id==CMIN?CSTARDOT:id==CMAX?CPLUSDOT:id; 
 if(id==CBDOT&&(x=VAV(x)->f,INT&AT(x)&&!AR(x)))id=(C)*AV(x);
 switch(AR(w)&&0<m0&&m0<=*AS(w)?id:0){
  case CPLUS:    if(wt&B01+INT+FL)R movsumavg(m,w,self,0); break;
  case CMIN:     if(wt&SBT+INT+FL)R movminmax(m,w,self,0); break;
  case CMAX:     if(wt&SBT+INT+FL)R movminmax(m,w,self,1); break;
  case CSTARDOT: if(wt&B01       )R  movandor(m,w,self,0); break;
  case CPLUSDOT: if(wt&B01       )R  movandor(m,w,self,1); break;
  case CNE:      if(wt&B01       )R   movneeq(m,w,self,0); break;
  case CEQ:      if(wt&B01       )R   movneeq(m,w,self,1); break;
  case CBW1001:  if(wt&    INT   )R movbwneeq(m,w,self,1); break;
  case CBW0110:  if(wt&    INT   )R movbwneeq(m,w,self,0); break;
 }
 vains(id,wt,&ado,&cv);
 if(!ado||!m||m>p)R infix(a,w,self);
 d=0<=m0?1+p-m:(p+m-1)/m; c=aii(w); cm=c*m; b=0>m0&&0<p%m;
 zt=rtype(cv); jt->rank=0; 
 GA(z,zt,c*d,MAX(1,AR(w)),AS(w)); *AS(z)=d;
 if((t=atype(cv))&&TYPESNE(t,wt)){RZ(w=cvt(t,w)); wt=AT(w);}
 zv=CAV(z); zk=bp(zt)*c; 
 wv=CAV(w); wk=bp(wt)*(0<=m0?c:c*m);
 DO(d-b, ado(jt,1L,cm,m,zv,wv); zv+=zk; wv+=wk;);
 if(b)ado(jt,1L,c*(p%m),p%m,zv,wv);
 if(jt->jerr>=EWOV){RESETERR; R movfslash(a,cvt(FL,w),self);}else R z;
}    /* a f/\w */

static DF1(jtiota1){R apv(IC(w),1L,1L);}

F1(jtbslash){A f;AF f1=jtprefix,f2=jtinfix;V*v;
 RZ(w);
 if(NOUN&AT(w))R fdef(CBSLASH,VERB, jtgprefix,jtginfix, w,0L,fxeachv(1L,w), VGERL|VAV(ds(CBSLASH))->flag, RMAX,0L,RMAX);
 v=VAV(w); f=VAV(w)->f;
 switch(v->id){
  case CPOUND:
   f1=jtiota1; break;
  case CLEFT: case CRIGHT: case CCOMMA:   
   f2=jtinfixd; break;
  case CFORK:  
   if(v->f1==(AF)jtmean)f2=jtmovavg; break;
  case CSLASH: 
   f2=jtmovfslash; if(vaid(f))f1=jtpscan;
 }
 R ADERIV(CBSLASH,f1,f2,VAV(ds(CBSLASH))->flag,RMAX,0L,RMAX);
}

A jtascan(J jt,C c,A w){RZ(w); R df1(w,bslash(slash(ds(c))));}
