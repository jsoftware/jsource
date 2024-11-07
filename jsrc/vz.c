/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Complex-Valued Scalar Functions                                  */

#include "j.h"
#include "vcomp.h"


static const Z zj={0,1};
static const Z z1={1,0};

static D hypoth(D u,D v){D p,q,t; MMM(u,v); R INF(p)?inf:p?(t=q/p,p*sqrt(1+t*t)):0;}

static ZF1(jtzjx){Z z; z.re=-v.im; z.im= v.re; R z;}  // v * 0j1
static ZF1(jtzmj){Z z; z.re= v.im; z.im=-v.re; R z;}  // v * 0j_1 = v/0j1

Z zrj0(D a){Z z; z.re=a; z.im=0.0; R z;}

ZS1(jtzconjug, zr=a; zi=-b;)

ZS2(jtzplus,  zr=a+c; zi=b+d;)

ZS2(jtzminus, zr=a-c; zi=b-d;)

ZF1(jtztrend){D a,b,t;Z z;
 a=v.re; b=v.im;
 if(ZOV(v)){a/=2; b/=2;}
 t=hypoth(a,b); 
 if(t<inf){if(t){z.re=a/t; z.im=b/t;}else{z.re=a; z.im=b;}}  // clang17-18 fail on if(!t)++t..., doing the divide too early
 else
  switch((INF(a)?2:0)+INF(b)){
  case 1: z.re=0.0;    z.im=(D)SGN(b); break;
  case 2: z.re=(D)SGN(a); z.im=0.0;    break;
  case 3: ZASSERT(0,EVNAN);
  }
 R z;
}

ZF2(jtztymes){D a,b,c,d;Z z;
 a=u.re; b=u.im; c=v.re; d=v.im;
 z.re=TYMES(a,c)-TYMES(b,d);
 z.im=TYMES(a,d)+TYMES(b,c);
 R z;
}

ZF2(jtzdiv){ZF2DECL;D t;
 if(ZNZ(v)){
  if(ABS(c)<ABS(d)){t=a; a=-b; b=t;  t=c; c=-d; d=t;}
  a/=c; b/=c; if(likely(ABS(d)!=inf))d/=c; else d=0.0;  t=1+d*d; zr=(a+TYMES(b,d))/t; zi=(b-TYMES(a,d))/t;
 }else if(ZNZ(u)){  // division by 0
  if(a!=0)zr=a/0.0; if(b!=0)zi=b/0.0;  // set any nonzero to infinity
 }
 ZEPILOG;
}

ZF1(jtznegate){R zminus(zeroZ,v);}

D zmag(Z v){R hypoth(v.re,v.im);}

B jtzeq(J jt,Z u,Z v){D a=u.re,b=u.im,c=v.re,d=v.im,p,q;
 if(a==c&&TEQ(b,d))R 1;  // fast check for equality - also picks up cases where one component is infinite
 if(b==d&&TEQ(a,c))R 1;
 if(ZEZ(u)||ZEZ(v)||1.0==jt->cct||(0>a!=0>c&&0>b!=0>d))R 0;
 if(ZOV(u)||ZOV(v)){a/=2; b/=2; c/=2; d/=2;}
 if(ZUN(u)||ZUN(v)){a*=2; b*=2; c*=2; d*=2;}
 p=hypoth(a,b); q=hypoth(c,d);
 R p!=inf && q!=inf && hypoth(a-c,b-d)<=(1.0-jt->cct)*MAX(p,q);
}

ZF1(jtzfloor){D p,q;
 ZF1DECL;
 zr=jfloor(a); p=a-zr;
 zi=jfloor(b); q=b-zi;
 if(1<=p+q+(1.0-jt->cct))if(p>=q)++zr; else ++zi;   // could improve this test
 ZEPILOG;
}

ZF1(jtzceil){R znegate(zfloor(znegate(v)));}

// u | v
ZF2(jtzrem){D a,b,d;Z q;
 if(ZEZ(u))R v;  // if u=0, return v
 ZASSERT(!ZINF(v),EVNAN);
 if(INF(u.re)&&!u.im&&!v.im){
  // infinity found
  if(u.re==inf )R 0<=v.re?v:u;
  if(u.re==infm)R 0>=v.re?v:u;
 }
 ZASSERT(!ZINF(u),EVNONCE);
 // general case, return v - u * <. v % u
 // calculate v % u as (v * +u) % |u
 d=u.re*u.re+u.im*u.im;
 a=u.re*v.re+u.im*v.im;
 b=u.re*v.im-u.im*v.re;
 q.re=a/d; q.im=b/d;
 q=zfloor(q);  // do proper complex floor
 R zminus(v,ztymes(u,q));
}

ZF2(jtzgcd){D a,b;Z t,z;I lim;
 ZASSERT(!(ZINF(u)||ZINF(v)),EVNAN);
 for(lim=2048; lim>0&&ZNZ(u); --lim){t=zrem(u,v); v.re=u.re; v.im=u.im; u.re=t.re; u.im=t.im;}  // max # iters is log(MAXFLOAT)/log(phi)
 if(lim==0)R zeroZ;  // if Euclid failed, return 0j0
 // Move result into first quadrant, and off the real axis
 I RrIi=(((v.re>0)*2+(v.re==0))*2+(v.im>0))*2+(v.im==0);  // classify result signs en bloc
 z.re=ABS(v.re); z.im=ABS(v.im);
 if((0xf1f4>>RrIi)&1){D zt=z.re; z.re=z.im; z.im=zt;}  // swap if re==0, or im!=0&(re>0!=im>0) 1111 0001 1111 0100
 R z;
}

ZF2(jtzlcm){ZASSERT(!(ZINF(u)||ZINF(v)),EVNAN); R ZEZ(u)||ZEZ(v) ? zeroZ : ztymes(u,zdiv(v,zgcd(u,v)));}

ZF1(jtzexp){D a,b,c,s,t;Z z;
 a=v.re; b=v.im;
 if(a<EMIN)z.re=z.im=0.0;
 else{
  ZASSERT(-THMAX<b&&b<THMAX,EVLIMIT); 
  c=cos(b); s=sin(b); 
  if(a<=EMAX){t=exp(a); z.re=t*c; z.im=t*s;}
  else{
   if(!c)z.re=0; else{t=a+log(ABS(c)); t=EMAX<t?inf:exp(t); z.re=0>c?-t:t;}
   if(!s)z.im=0; else{t=a+log(ABS(s)); t=EMAX<t?inf:exp(t); z.im=0>s?-t:t;}
 }}
 R z;
}

ZF1(jtzlog){ZF1DECL;
 zr=b?log(hypoth(a,b)):INF(a)?inf:a?log(hypoth(a,b)):-inf;
 zi=a||b?atan2(b,a):0;
#if SY_WINCE_MIPS && !defined(WIN32_PLATFORM_PSPC)
 if(!b) zi=a<0?PI : 0;  /* atan2(0,v) fails in mips handheld wince - _9^2.5-1.5*/
#endif
 ZEPILOG;
}

ZF2(jtzpow){ZF2DECL;D m;I n;
 if(!a&&!b){z.re=d?0:0>c?inf:!c; z.im=0; R z;}
 if(!d&&IMIN<c&&c<=FLIMAX&&(n=(I)jfloor(c),c==n)){
  if(0>n){u=zdiv(z1,u); n=-n;}
  z=z1;
  while(n){if(1&n)z=ztymes(z,u); u=ztymes(u,u); n>>=1;}
  R z;
 }
 z=zexp(ztymes(v,zlog(u)));
 if(!b&&!d){
  m=jfloor(c);
  if(0>a&&c>m&&c==0.5+m)z.re=0;
  if(c==m)z.im=0;
 }
 R z;
}

ZF1(jtzsqrt){D p,q,t;
 ZF1DECL;
 MMM(a,b);
 if(p){
  t=0.5*q/p; zr=sqrt(ABS(a/2)+p*sqrt(0.25+t*t)); zi=b/(zr+zr);
  if(0>a){t=ABS(zi); zi=0>b?-zr:zr; zr=t;}
 }
 ZEPILOG;
}


/* See Abramowitz & Stegun, Handbook of Mathematical Functions,            */
/*   National Bureau of Standards, 1964 6.                                 */

static ZF1(jtzsin){D a,b,c,s;Z z;
 a=v.re; b=v.im;
 ZASSERT(-THMAX<a&&a<THMAX,EVLIMIT);
 s=sin(a); z.re=s?s*(b<-EMAX2||    EMAX2<b?inf:cosh(b)):0.0; 
 c=cos(a); z.im=c?c*(b<-EMAX2?infm:EMAX2<b?inf:sinh(b)):0.0;
 R z;
}    /* 4.3.55 */

static ZF1(jtzcos){D a,b,c,s;Z z;
 a=v.re; b=v.im;
 ZASSERT(-THMAX<a&&a<THMAX,EVLIMIT);
 c=cos(a); z.re=c? c*(b<-EMAX2||    EMAX2<b?inf:cosh(b)):0.0; 
 s=sin(a); z.im=s?-s*(b<-EMAX2?infm:EMAX2<b?inf:sinh(b)):0.0;
 R z;
}    /* 4.3.56 */

static ZF1(jtztan){R zdiv(zsin(v),zcos(v));}

// bug in some versions of Visual Studio
#pragma auto_inline(off)
static ZF1(jtzp4){R zsqrt(zplus(z1,ztymes(v,v)));}
#pragma auto_inline(on)

static ZF1(jtzm4){R 1e16<hypoth(v.re,v.im)?v:ztymes(zplus(v,z1),zsqrt(zdiv(zminus(v,z1),zplus(v,z1))));}

static ZF1(jtzsinh){R zmj(zsin(zjx(v)));}  /* 4.5.7 */

static ZF1(jtzcosh){R zcos(zjx(v));}       /* 4.5.8 */

static ZF1(jtztanh){R v.re<-TMAX?zrj0(-1.0):TMAX<v.re?z1:zdiv(zsinh(v),zcosh(v));}

static ZF1(jtzp8){R zsqrt(ztymes(zplus(zj,v),zminus(zj,v)));}

static ZF1(jtzasinh){R 0>v.re ? znegate(zasinh(znegate(v))) : zlog(zplus(v,zp4(v)));}

static ZF1(jtzacosh){Z z;
 z=zlog(zplus(v,zm4(v))); 
 if(0>=z.re){z.re=0; z.im=ABS(z.im);} 
 R z;
}

// Here we follow Kahan, Branch Cuts for Complex Elementary Functions, Table 1 which has now-recommended branch cuts
static ZF1(jtzatanh){R ztymes(zrj0((D)0.5),zminus(zlog(zplus(z1,v)),zlog(zminus(z1,v))));}

static ZF1(jtzatan){ZF1DECL;
 if(unlikely(a*a+b*b>1e26))z=zrj0(PI/2.0);  // if either component very large, result is +-pi/2
 else z=zmj(zatanh(zjx(v)));
 z.re=copysign(z.re,a);  // The branch cut is on the imaginary axis, so we just transfer the sign of a to the real part
 if(!b)z.im=0;
 R z;
}    /* 4.4.22 */

// NaN bug in android asin() _1 o. _1
#if defined(ANDROID) && (defined(__aarch32__)||defined(__arm__)||defined(__aarch64__))
static ZF1(jtzasin){R !v.im&&-1<=v.re&&v.re<=1?zrj0(({D _d=asin(v.re);NAN0;_d;})):zmj(zasinh(zjx(v)));}   /* 4.4.20 */
#else
static ZF1(jtzasin){R !v.im&&-1<=v.re&&v.re<=1?zrj0(asin(v.re)):zmj(zasinh(zjx(v)));}   /* 4.4.20 */
#endif

static ZF1(jtzacos){R zminus(zrj0(PI/2.0),zasin(v));}

static ZF1(jtzarc){D x,y;Z t,z;
 z.re=z.im=0;
 t=ztrend(v); x=t.re; y=t.im;
 if(0!=x||0!=y)z.re=atan2(y,x);
 
#if SY_WINCE_MIPS && !defined(WIN32_PLATFORM_PSPC)
 if(!y) z.re=x<0?PI : 0;  /* atan2(0,v) fails in mips handheld wince - 12 o. _3 */
#endif

 R z;
}

ZF2(jtzcir){D r;I x;Z z;
 z=zeroZ;
 r=u.re;
 x=(I)jround(r);
 ZASSERT(BETWEENC(x,-12,12)&&FFEQ(x,r)&&!u.im,EVDOMAIN);  // x must be integer
 switch(x){
 default: ZASSERT(0,EVDOMAIN);
 case  0: R zsqrt(ztymes(zplus(z1,v),zminus(z1,v)));
 case  1: R zsin(v);           case  -1: R zasin(v);
 case  2: R zcos(v);           case  -2: R zacos(v);
 case  3: R ztan(v);           case  -3: R zatan(v);
 case  4: R zp4(v);            case  -4: R zm4(v);
 case  5: R zsinh(v);          case  -5: R zasinh(v);
 case  6: R zcosh(v);          case  -6: R zacosh(v);
 case  7: R ztanh(v);          case  -7: R zatanh(v);
 case  8: R zp8(v);            case  -8: R znegate(zp8(v));
 case  9: z.re=v.re;    R z;   case  -9: R v;
 case 10: z.re=zmag(v); R z;   case -10: R zconjug(v);
 case 11: z.re=v.im;    R z;   case -11: R zjx(v);
 case 12: R zarc(v);           case -12: R zexp(zjx(v));
 }
}

B jtztridiag(J jt,I n,A a,A x){I i,j,n1=n-1;Z*av,d,p,*xv;
 av=ZAV(a); xv=ZAV(x); d=xv[0];
 for(i=j=0;i<n1;++i){
  ASSERT(ZNZ(d),EVDOMAIN);
  p=zdiv(xv[j+2],d); 
  xv[j+3]=d=zminus(xv[j+3],ztymes(p,xv[j+1]));
  av[i+1]=  zminus(av[i+1],ztymes(p,av[i]  ));
  j+=3;
 }
 ASSERT(ZNZ(d),EVDOMAIN);
 i=n-1; j=AN(x)-1; av[i]=zdiv(av[i],d);
 for(i=n-2;i>=0;--i){j-=3; av[i]=zdiv(zminus(av[i],ztymes(xv[j+1],av[i+1])),xv[j]);}
 R 1;
}

DF1(jtexppi){A z;B b;D r,th,y;I k;Z*v,t;
 F1RANK(0,jtexppi,self);
 if(!(CMPX&AT(w)))R expn1(pix(w));   // if not complex, revert
 v=ZAV(w); r=exp(PI*v->re); y=v->im; if(b=0>y)y=-y;  // take exp of real part, set y=imaginary part
 th=y-2*(I)(y/2); k=(I)(2*th); if(k!=2*th)k=-1; else if(b&&k)k=4-k;
 if(!((UI)k<=(UI)3))R expn1(pix(w));   // if k is not an exact multiple of 0.5, revert - what good is that???
 GAT0(z,CMPX,1,0); ZAV0(z)[0].re=ZAV0(z)[0].im=0; DAV0(z)[k&1]=r*(1-(k&2));  // set result to 0, then store into real or imag, possibly changing sign
 R z;
}    /* special code for ^@o. */


ZF1(jtznonce1){ZASSERT(0,EVNONCE);}

ZF2(jtznonce2){ZASSERT(0,EVNONCE);}
