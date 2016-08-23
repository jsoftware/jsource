/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Inner Product                                             */

#include "j.h"
#include "vasm.h"

static A jtipprep(J jt,A a,A w,I zt,I*pm,I*pn,I*pp){A z=mark;I*as,ar,ar1,m,mn,n,p,*ws,wr,wr1;
 ar=AR(a); as=AS(a); ar1=ar?ar-1:0; RE(*pm=m=prod(ar1,  as));
 wr=AR(w); ws=AS(w); wr1=wr?wr-1:0; RE(*pn=n=prod(wr1,1+ws)); RE(mn=mult(m,n));
 *pp=p=ar?*(as+ar1):wr?*ws:1;
 ASSERT(!(ar&&wr)||p==*ws,EVLENGTH);
 GA(z,zt,mn,ar1+wr1,0);
 ICPY(AS(z),      as,ar1);
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
 switch(t){
  default:   ASSERT(0,EVDOMAIN);
  case CMPX:  if(at&B01)PDTBY(Z,Z,ZINC) else PDTXB(Z,Z,ZINC,c=*u++   ); break;
  case FL:    if(at&B01)PDTBY(D,D,DINC) else PDTXB(D,D,DINC,c=*u++   ); break;
  case INT:   if(at&B01)PDTBY(I,I,IINC) else PDTXB(I,I,IINC,c=*u++   ); 
             if(er==EWOV){
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

#if C_NA
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

F2(jtpdt){PROLOG(0038);A z;I ar,at,i,m,n,p,p1,t,wr,wt;
 RZ(a&&w);
 ar=AR(a); at=AN(a)?AT(a):B01;
 wr=AR(w); wt=AN(w)?AT(w):B01;
 if((at|wt)&SPARSE)R pdtsp(a,w);
 if((at|wt)&XNUM+RAT)R df2(a,w,atop(slash(ds(CPLUS)),qq(ds(CSTAR),v2(1L,AR(w)))));
 if(ar&&wr&&AN(a)&&AN(w)&&TYPESNE(at,wt)&&B01&at+wt)R pdtby(a,w);   // prefer at^wt
 t=coerce2(&a,&w,B01);
 ASSERT(t&NUMERIC,EVDOMAIN);
 RZ(z=ipprep(a,w,t&B01?INT:t&INT&&!SY_64?FL:t,&m,&n,&p));
 if(!p){memset(AV(z),C0,AN(z)*bp(AT(z))); R z;}
 if(!ar!=!wr){if(ar)RZ(w=reshape(sc(p),w)) else RZ(a=reshape(sc(p),a));}
 p1=p-1;
 switch(t){
  case B01:
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
  case INT:
#if SY_64
   {C er=0;I c,*u,*v,*wv,*x,*zv;
    u=AV(a); v=wv=AV(w); zv=AV(z);
 /*
   for(i=0;i<m;++i,v=wv,zv+=n){
     x=zv; c=*u++; er=asmtymes1v(n,x,c,v);    if(er)break; v+=n;
     DO(p1, x=zv; c=*u++; er=asminnerprodx(n,x,c,v); if(er)break; v+=n;);

 */
    for(i=0;i<m;++i,v=wv,zv+=n){
     x=zv; c=*u++; TYMES1V(n,x,c,v); if(er)break; v+=n;
     DO(p1, x=zv; c=*u++; er=asminnerprodx(n,x,c,v); if(er)break; v+=n;);
     if(er)break;
    }
    if(er){A z1;D c,*x,*zv;I*u,*v,*wv;
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
  case FL:
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
   break;
  case CMPX:
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
  memcpy(zv,*av?v1:v0,n); if(ac)++av;                    \
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

static A jtipbx(J jt,A a,A w,C c,C d){A g=0,x0,x1,z;B*av,b,e,*u,*v,*v0,*v1,*zv;C c0,c1;
    I ac,i,j,m,n,p,q,r,*uu,*vv,wc;
 RZ(a&&w);
 RZ(z=ipprep(a,w,B01,&m,&n,&p));
 ac=AR(a)?1:0; wc=AR(w)?n:0; q=n/SZI; r=n%SZI;
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
 if(!g)RZ(x0=c0==IPBX0?reshape(sc(n),zero):c0==IPBX1?reshape(sc(c==CNE?AN(w):n),one):c0==IPBXW?w:not(w));
 if(!g)RZ(x1=c1==IPBX0?reshape(sc(n),zero):c1==IPBX1?reshape(sc(c==CNE?AN(w):n),one):c1==IPBXW?w:not(w));
 av=BAV(a); zv=BAV(z); v0=BAV(x0); v1=BAV(x1);
 switch(c){
  case CPLUSDOT:
#define F(x,y) *x++|=*y++
#include "cip_t.h"
   break;
  case CSTARDOT:
#define F(x,y) *x++&=*y++
#include "cip_t.h"
   break;
  case CNE:
#define F(x,y) *x++^=*y++
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


F1(jtminors){A d;
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
