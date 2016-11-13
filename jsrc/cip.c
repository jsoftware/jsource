/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Inner Product                                             */

#include "j.h"
#include "vasm.h"

// Analysis for inner product
// a,w are arguments
// zt is type of result
// *pm is # 1-cells of a
// *pn is # atoms in an item of w
// *pp is number of inner-product muladds
//   (in each, an atom of a multiplies an item of w)
static A jtipprep(J jt,A a,A w,I zt,I*pm,I*pn,I*pp){A z=mark;I*as,ar,ar1,m,mn,n,p,*ws,wr,wr1;
 ar=AR(a); as=AS(a); ar1=ar?ar-1:0; RE(*pm=m=prod(ar1,as));  // m=# 1-cells of a.  It could overflow, if there are no atoms
 wr=AR(w); ws=AS(w); wr1=wr?wr-1:0; RE(*pn=n=prod(wr1,1+ws)); RE(mn=mult(m,n));  // n=#atoms in 1-cell of w; mn = #atoms in result
 *pp=p=ar?*(as+ar1):wr?*ws:1;  // if a is an array, the length of a 1-cell; otherwise, the number of items of w
 ASSERT(!(ar&&wr)||p==*ws,EVLENGTH);
 GA(z,zt,mn,ar1+wr1,0);   // allocate result area
 ICPY(AS(z),      as,ar1);  // Set shape: 1-frame of a followed by shape of item of w
 ICPY(AS(z)+ar1,1+ws,wr1);
 R z;
}    /* argument validation & result for an inner product */

#define IINC(x,y)    {b=0>x; x+=y; BOV(b==0>y&&b!=0>x);}
#define DINC(x,y)    {x+=y;}
#define ZINC(x,y)    {(x).re+=(y).re; (x).im+=(y).im;}

#define PDTBY(Tz,Tw,INC)          \
 {Tw*v,*wv;Tz c,*x,zero,*zv;      \
  v=wv=(Tw*)AV(w); zv=(Tz*)AV(z); memset(&zero,C0,sizeof(Tz));     \
  if(1==n)DO(m, v=wv; c=zero;           DO(p,       if(*u++)INC(c,*v);             ++v;);             *zv++=c;)   \
  else    DO(m, v=wv; memset(zv,C0,zk); DO(p, x=zv; if(*u++)DO(n, INC(*x,*v); ++x; ++v;) else v+=n;); zv+=n;  );  \
 }

#define PDTXB(Tz,Ta,INC,INIT)     \
 {Ta*u;Tz c,*x,zero,*zv;          \
  u=   (Ta*)AV(a); zv=(Tz*)AV(z); memset(&zero,C0,sizeof(Tz));     \
  if(1==n)DO(m, v=wv; c=zero;           DO(p,                   if(*v++)INC(c,*u); ++u;  ); *zv++=c;)   \
  else    DO(m, v=wv; memset(zv,C0,zk); DO(p, x=zv; INIT; DO(n, if(*v++)INC(*x,c); ++x;);); zv+=n;  );  \
 }

static F2(jtpdtby){A z;B b,*u,*v,*wv;C er=0;I at,m,n,p,t,wt,zk;
 at=AT(a); wt=AT(w); t=at&B01?wt:at;
 RZ(z=ipprep(a,w,t,&m,&n,&p)); zk=n*bp(t); u=BAV(a); v=wv=BAV(w);
 NAN0;
 switch(CTTZ(t)){
  default:   ASSERT(0,EVDOMAIN);
  case CMPXX:  if(at&B01)PDTBY(Z,Z,ZINC) else PDTXB(Z,Z,ZINC,c=*u++   ); break;
  case FLX:    if(at&B01)PDTBY(D,D,DINC) else PDTXB(D,D,DINC,c=*u++   ); break;
  case INTX:   if(at&B01)PDTBY(I,I,IINC) else PDTXB(I,I,IINC,c=*u++   ); 
             if(er>=EWOV){
              RZ(z=ipprep(a,w,FL,&m,&n,&p)); zk=n*sizeof(D); u=BAV(a); v=wv=BAV(w);
              if(at&B01)PDTBY(D,I,IINC) else PDTXB(D,I,IINC,c=(D)*u++);
 }}
 NAN1;
 R z;
}    /* x +/ .* y where x or y (but not both) is Boolean */

#define BBLOCK(nn)  \
     d=ti; DO(nw, *d++=0;);                                           \
     DO(nn, if(*u++){vi=(UI*)v; d=ti; DO(nw, *d+++=*vi++;);} v+=n;);  \
     x=zv; c=tc; DO(n, *x+++=*c++;);

#if 0&&C_NA    // scaf  asm function no longer used, and no longer defined in C
/*
*** from asm64noovf.c
C asminnerprodx(I m,I*z,I u,I*y)
{
 I i=-1,t;
l1:
 ++i;
 if(i==m) return 0;
 t= u FTIMES y[i];
 ov(t)
 t= t FPLUS z[i];
 ov(t)
 z[i]=t;
 goto l1;
}
*/

C asminnerprodx(I m,I*z,I u,I*y)
{
 I i=-1,t,p;DI tdi;
l1:
 ++i;
 if(i==m) return 0;
 tdi = u * (DI)y[i];
 t=(I)tdi;
 if (tdi<IMIN||IMAX<tdi) R EWOV;
 p=0>t;
 t= t + z[i];
 if (p==0>z[i]&&p!=0>t) R EWOV;
 z[i]=t;
 goto l1;
}
#endif

// +/ . * support
F2(jtpdt){PROLOG(0038);A z;I ar,at,i,m,n,p,p1,t,wr,wt;
 RZ(a&&w);
 // ?r = rank, ?t = type (but set Boolean type for an empty argument)
 ar=AR(a); at=AN(a)?AT(a):B01;
 wr=AR(w); wt=AN(w)?AT(w):B01;
 if((at|wt)&SPARSE)R pdtsp(a,w);  // Transfer to sparse code if either arg sparse
 if((at|wt)&XNUM+RAT)R df2(a,w,atop(slash(ds(CPLUS)),qq(ds(CSTAR),v2(1L,AR(w)))));  // On indirect numeric, execute as +/@(*"(1,(wr)))
 if(ar&&wr&&AN(a)&&AN(w)&&TYPESNE(at,wt)&&B01&at+wt)R pdtby(a,w);   // If exactly one arg is boolean, handle separately
 t=coerce2(&a,&w,B01);  // convert a/w to common type, using b01 if both empty
 ASSERT(t&NUMERIC,EVDOMAIN);
 // Allocate result area and calculate loop controls
 // m is # 1-cells of a
 // n is # atoms in an item of w
 // p is number of inner-product muladds (length of a row of a)

 RZ(z=ipprep(a,w,t&B01?INT:t&INT&&!SY_64?FL:t,&m,&n,&p));
 if(!p){memset(AV(z),C0,AN(z)*bp(AT(z))); R z;}
 // If either arg is atomic, reshape it to a list
 if(!ar!=!wr){if(ar)RZ(w=reshape(sc(p),w)) else RZ(a=reshape(sc(p),a));}
 p1=p-1;
 // Perform the inner product according to the type
 switch(CTTZNOFLAG(t)){
  case B01X:
   if(0==n%SZI||!SY_ALIGN){A tt;B*u,*v,*wv;I nw,q,r,*x,*zv;UC*c,*tc;UI*d,*ti,*vi;
    q=p/255; r=p%255; nw=(n+SZI-1)/SZI;
    GATV(tt,INT,nw,1,0); ti=(UI*)AV(tt); tc=(UC*)ti;
    u=BAV(a); v=wv=BAV(w); zv=AV(z);
    for(i=0;i<m;++i,v=wv,zv+=n){x=zv; DO(n, *x++=0;); DO(q, BBLOCK(255);); BBLOCK(r);}
   }else{B*u,*v,*wv;I*x,*zv;
    u=BAV(a); v=wv=BAV(w); zv=AV(z);
    for(i=0;i<m;++i,v=wv,zv+=n){
            x=zv; if(*u++)DO(n, *x++ =*v++;) else{v+=n; DO(n, *x++=0;);}
     DO(p1, x=zv; if(*u++)DO(n, *x+++=*v++;) else v+=n;);
   }}
   break;
  case INTX:
#if SY_64
   {I er=0;I c,* RESTRICT u,* RESTRICT v,* RESTRICT wv,* RESTRICT x,* RESTRICT zv;
    u=AV(a); v=wv=AV(w); zv=AV(z);
 /*
   for(i=0;i<m;++i,v=wv,zv+=n){
     x=zv; c=*u++; er=asmtymes1v(n,x,c,v);    if(er)break; v+=n;
     DO(p1, x=zv; c=*u++; er=asminnerprodx(n,x,c,v); if(er)break; v+=n;);

 */
#if C_NA   // non-assembler version
    for(i=0;i<m;++i,v=wv,zv+=n){DPMULDECLS I o,oc,lp;
     x=zv; c=*u++; DQ(n, DPMUL(c,*v, x, ++er); ++v; ++x;)
    DQ(p1, x=zv; c=*u++; DQ(n, DPMULD(c,*v, lp, ++er;) o=*x; oc=(~o)^lp; lp+=o; *x++=lp; o^=lp; ++v; if(XANDY(oc,o)<0)++er;) if(er)break;)  // oflo if signs equal, and different from result sign
    }
#else
    for(i=0;i<m;++i,v=wv,zv+=n){
     x=zv; c=*u++; TYMES1V(n,x,c,v); if(er)break; v+=n;
     DO(p1, x=zv; c=*u++; er=asminnerprodx(n,x,c,v); if(er)break; v+=n;);
     if(er)break;
    }
#endif
    if(er){A z1;D c,* RESTRICT x,* RESTRICT zv;I* RESTRICT u,* RESTRICT v,* RESTRICT wv;
     GATV(z1,FL,AN(z),AR(z),AS(z)); z=z1;
     u=AV(a); v=wv=AV(w); zv=DAV(z);
     for(i=0;i<m;++i,v=wv,zv+=n){
             x=zv; c=(D)*u++; DO(n, *x++ =c**v++;);
      DO(p1, x=zv; c=(D)*u++; DO(n, *x+++=c**v++;););
   }}}
#else
   {D c,*x,*zv;I*u,*v,*wv;
    u=AV(a); v=wv=AV(w); zv=DAV(z);
    if(1==n)DO(m, v=wv; c=0.0; DO(p, c+=*u++*(D)*v++;); *zv++=c;)
    else for(i=0;i<m;++i,v=wv,zv+=n){
            x=zv; c=(D)*u++; DO(n, *x++ =c**v++;);
     DO(p1, x=zv; c=(D)*u++; DO(n, *x+++=c**v++;););
    }
    RZ(z=icvt(z));
   }
#endif
   break;
  case FLX:
#if 1
   {D c,s,t,*u,*v,*wv,*x,*zv;
    u=DAV(a); v=wv=DAV(w); zv=DAV(z);
    NAN0;
    if(1==n){DO(m, v=wv; c=0.0; DO(p, s=*u++; t=*v++; c+=s&&t?s*t:0;); *zv++=c;);}
    else for(i=0;i<m;++i,v=wv,zv+=n){
            x=zv; if(c=*u++){if(INF(c))DO(n, *x++ =*v?c**v:0.0; ++v;)else DO(n, *x++ =c**v++;);}else{v+=n; DO(n, *x++=0.0;);}
     DO(p1, x=zv; if(c=*u++){if(INF(c))DO(n, *x+++=*v?c**v:0.0; ++v;)else DO(n, *x+++=c**v++;);}else v+=n;);
    }
    NAN1;
   }

#else  // this turned out to be slower, at least until the arguments won't fit in cache.  Fast page mode memory saves the day for the original code
#define OPHEIGHT 2  // height of outer-product block
#define OPWIDTH 2  // width of outer-product block
#define WSWATCHWIDTH 16  // number of columns of w in each inner product
#define MAXOPS 4 // maximum number of outer products
   {I remlines, remwcols, reminblock, remw2blocks, numip;
   NAN0;
   // m is # 1-cells of a
   // n is # atoms in an item of w (and result)
   // p is number of inner-product muladds (length of a row of a, and # items of w)
   D* RESTRICT arowhd = DAV(a);  // base of current row-pair of a
   D* RESTRICT zrowhd = DAV(z);  // current result row-pair
   // process a by line-pairs, producing a result line-pair
   for(remlines = m;remlines>0;remlines-=OPHEIGHT,arowhd+=OPHEIGHT*p,zrowhd+=OPHEIGHT*n) {
    D* RESTRICT zcolhd = zrowhd;  // pointer to 2x16 result-block in the current row-pair
    D* RESTRICT wswatchhd = DAV(w);   // pointer to top of 16-column swatch of w, to be processed in 32x2 blocks
    // process w in 16-column swatches.  32x16 is the size of the block we need resident in cache
    for(remwcols = n;remwcols>0;remwcols-=WSWATCHWIDTH,zcolhd+=WSWATCHWIDTH,wswatchhd+=WSWATCHWIDTH) {
     D* RESTRICT ablkst=arowhd;  // pointer to current 2x32 block of a
     D* RESTRICT zblkst = zcolhd;  // pointer to result area, 2x16
     D* RESTRICT wmblkst = wswatchhd;  // pointer to current 32x2 block of w
     // Clear the result area to 0.  We do it now to bring the result block into cache,
     // and not before so we don't sweep through the cache too early
     {D* RESTRICT zp0 = zblkst;D* RESTRICT zp1;
     DO(MIN(OPHEIGHT,remlines), zp1=zp0; DO(MIN(WSWATCHWIDTH,remwcols), *zp1++=0;); zp0 += n;);
     }
     // process the inner product row x col in 32-outer-product macroblocks
     for(reminblock = p;reminblock>0;reminblock-=MAXOPS,ablkst+=MAXOPS,wmblkst+=MAXOPS*n) {
      D* RESTRICT zv = zblkst;  // pointer to result 2x2
      D* RESTRICT wblkst = wmblkst;   // pointer to 1st 32x2 block in w
      // Process each 32x16 as up to 8 32x2 blocks, (less when we get to the right end of w)
      // Meanwhile, prefetch the next 32x16 (32x128 bytes) block of w
      for(remw2blocks=MIN(WSWATCHWIDTH,remwcols);remw2blocks>0;remw2blocks-=OPWIDTH,zv+=OPWIDTH,wblkst+=OPWIDTH) {
       // Accumulate 2x2 outer products into a 2x2 result.
       // We have 2 pointers to a and w.  We always fetch 2 a and 2 w; if we run off the end of either,
       // we fetch the same value twice to avoid an out-of-bounds fetch
_mm_prefetch(,MM_HINT_T0);
       D* RESTRICT at = ablkst;   // top-left of 2x32
       D* RESTRICT ab = ablkst + (remlines>1?p:0);  // lower row
       D* RESTRICT wl = wblkst;   // top-left of 32x2
       D* RESTRICT wr = wblkst + (remw2blocks>1?1:0);  // right column
       D ztl=0.0, ztr=0.0, zbl=0.0, zbr=0.0;
       // Loop through up to 32 2x2 outer products until we run out of inputs
       for(numip=MIN(MAXOPS,reminblock);numip;--numip,++at,++ab,wl+=n,wr+=n) {D t0, t1, t2;
        // Do one 2x2 outer product, accumulate into zxx
        t0 = *at, t1 = *wl;   // fetch at, wl
        ztl += t0 * t1;
        t2 = *ab;  // fetch ab
        zbl += t1 * t2;
        t1 = *wr;  // fetch wr
        ztr += t1 * t0;
        zbr += t1 * t2;
       }
       // Accumulate the sum-of-outer-products into the result area
       zv[0] += ztl;
       if(at!=ab)zv[n] += zbl;  // Don't write outside of valid result area if there is no second row
       if(wl!=wr){   // . or if no right column
        zv[1] += ztr;
        if(at!=ab)zv[n+1] += zbr;  // Don't write outside of valid result area if there is no second row
       }
      }
     }
    }
   }
   }
   NAN1;
#endif
   break;
  case CMPXX:
   {Z c,*u,*v,*wv,*x,*zv;
    u=ZAV(a); v=wv=ZAV(w); zv=ZAV(z);
    if(1==n)DO(m, v=wv; c=zeroZ; DO(p, c.re+=ZRE(*u,*v); c.im+=ZIM(*u,*v); ++u; ++v;); *zv++=c;)
    else for(i=0;i<m;++i,v=wv,zv+=n){
            x=zv; c=*u++; DO(n, x->re =ZRE(c,*v); x->im =ZIM(c,*v); ++x; ++v;);
     DO(p1, x=zv; c=*u++; DO(n, x->re+=ZRE(c,*v); x->im+=ZIM(c,*v); ++x; ++v;););
 }}}
 EPILOG(z);
}

#define IPBX(F)  \
 for(i=0;i<m;++i){                                       \
  MC(zv,*av?v1:v0,n); if(ac)++av;                    \
  for(j=1;j<p;++j){                                      \
   uu=(I*)zv; vv=(I*)(*av?v1+j*wc:v0+j*wc); if(ac)++av;  \
   DO(q, *uu++F=*vv++;);                                 \
   if(r){u=(B*)uu; v=(B*)vv; DO(r, *u++F=*v++;);}        \
  }                                                      \
  zv+=n;                                                 \
 }

#define IPBX0  0
#define IPBX1  1
#define IPBXW  2
#define IPBXNW 3

// a f/ . g w  for boolean a and w
// c is pseudochar for f, d is pseudochar for g
static A jtipbx(J jt,A a,A w,C c,C d){A g=0,x0,x1,z;B*av,*av0,b,*u,*v,*v0,*v1,*zv;C c0,c1;
    I ana,i,j,m,n,p,q,r,*uu,*vv,wc;
 RZ(a&&w);
 RZ(z=ipprep(a,w,B01,&m,&n,&p));
 // m=#1-cells of a, n=# bytes in 1-cell of w, p=length of individual inner product creating an atom
 ana=!!AR(a); wc=AR(w)?n:0; q=n/SZI; r=n%SZI;  // ana = 1 if a is not atomic; wc = stride between items of w; q=#fullwords in an item of w, r=remainder
 // Set c0 & c1 to classify the g operation
 switch(B01&AT(w)?d:0){
  case CEQ:                             c0=IPBXNW; c1=IPBXW;  break;
  case CNE:                             c0=IPBXW;  c1=IPBXNW; break;
  case CPLUSDOT: case CMAX:             c0=IPBXW;  c1=IPBX1;  break;
  case CSTARDOT: case CMIN: case CSTAR: c0=IPBX0;  c1=IPBXW;  break;
  case CLT:                             c0=IPBXW;  c1=IPBX0;  break;
  case CGT:                             c0=IPBX0;  c1=IPBXNW; break; 
  case CLE:                             c0=IPBX1;  c1=IPBXW;  break;
  case CGE:                             c0=IPBXNW; c1=IPBX1;  break;
  case CPLUSCO:                         c0=IPBXNW; c1=IPBX0;  break;
  case CSTARCO:                         c0=IPBX1;  c1=IPBXNW; break;
  default: c0=c1=-1; g=ds(d); RZ(x0=df2(zero,w,g)); RZ(x1=df2(zero,w,g));
 }
 // Set up x0 to be the argument to use for y if the atom of x is 0: 0, 1, y, -.y
 // Set up x1 to be the arg if xatom is 1
 if(!g)RZ(x0=c0==IPBX0?reshape(sc(n),zero):c0==IPBX1?reshape(sc(c==CNE?AN(w):n),one):c0==IPBXW?w:not(w));
 if(!g)RZ(x1=c1==IPBX0?reshape(sc(n),zero):c1==IPBX1?reshape(sc(c==CNE?AN(w):n),one):c1==IPBXW?w:not(w));
 // av->a arg, zv->result, v0->input for 0, v1->input for 1
 av0=BAV(a); zv=BAV(z); v0=BAV(x0); v1=BAV(x1);

 // See if the operations are such that a 0 or 1 from a can skip the innerproduct, or perhaps
 // terminate the entire operation
 I esat=2, eskip=2;  // if byte of a equals esat, the entire result-cell is set to its limit value; if it equals eskip, the innerproduct is skipped
 if(c==CPLUSDOT&&(c0==IPBX1||c1==IPBX1)||c==CSTARDOT&&(c0==IPBX0||c1==IPBX0)){
 // +./ . (+. <: >: *:)   *./ . (*. < > +:)   a byte of a can saturate the entire result-cell: see which value does that
  esat=c==CPLUSDOT?c1==IPBX1:c1==IPBX0;  // esat==1 if +./ . (+. >:)   or *./ . (< +:) where x=1 overrides y (producing 1);  if esat=0, x=0 overrides y (producing 1)
 }else if(c==CPLUSDOT&&(c0==IPBX0||c1==IPBX0)||c==CSTARDOT&&(c0==IPBX1||c1==IPBX1)||
     c==CNE&&(c0==IPBX0||c1==IPBX0)){
  // (+. ~:)/ . (*. < > +:)   *./ . (+. <: >: *:)  a byte of a can guarantee the current innerproduct has no effect
  eskip=c==CSTARDOT?c1==IPBX1:c1==IPBX0;  // eskip==1 if  (+. ~:)/ . (+:)   *./ . (+. >:)  where x=1 has no effect; if esat=0, x=0 has no effect
 }

 switch(c){
  case CPLUSDOT:
#define F |=
#include "cip_t.h"
   break;
  case CSTARDOT:
#define F &=
#include "cip_t.h"
   break;
  case CNE:
#define F ^=
#include "cip_t.h"
   break;
 }
 R z;
}    /* a f/ . g w  where a and w are nonempty and a is boolean */

static DF2(jtdotprod){A fs,gs;C c,d;I r;V*sv;
 RZ(a&&w&&self);
 sv=VAV(self); fs=sv->f; gs=sv->g;
 if(B01&AT(a)&&AN(a)&&AN(w)&&CSLASH==ID(fs)&&(d=vaid(gs))&&
     (c=vaid(VAV(fs)->f),c==CSTARDOT||c==CPLUSDOT||c==CNE))R ipbx(a,w,c,d);
 r=lr(gs);
 R df2(a,w,atop(fs,qq(gs,v2(r==RMAX?r:1+r,RMAX))));
}


static F1(jtminors){A d;
 RZ(d=apv(3L,-1L,1L)); *AV(d)=0;
 R drop(d,df2(one,w,bsdot(ds(CLEFT))));
}

static DF1(jtdet){DECLFG;A h=sv->h;I c,r,*s;
 RZ(w);
 r=AR(w); s=AS(w);
 if(h&&1<r&&2==s[r-1]&&s[r-2]==s[r-1])R df1(w,h);
 F1RANK(2,jtdet,self);
 c=2>r?1:s[1];
 R !c ? df1(mtv,slash(gs)) : 1==c ? CALL1(f1,ravel(w),fs) : h && c==*s ? gaussdet(w) : detxm(w,self); 
}

DF1(jtdetxm){R dotprod(irs1(w,0L,1L,jthead),det(minors(w),self),self);}
     /* determinant via expansion by minors. w is matrix with >1 columns */

F2(jtdot){A f,h=0;AF f2=jtdotprod;C c,d;
 ASSERTVV(a,w);
 if(CSLASH==ID(a)){
  f=VAV(a)->f; c=ID(f); d=ID(w);
  if(d==CSTAR){
   if(c==CPLUS )f2=jtpdt; 
   if(c==CMINUS)RZ(h=eval("[: -/\"1 {.\"2 * |.\"1@:({:\"2)"));
 }}
 R fdef(CDOT,VERB, jtdet,f2, a,w,h, 0L, 2L,RMAX,RMAX);
}
