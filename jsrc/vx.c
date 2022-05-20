/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Extended Precision Integers                                      */

#include "j.h"
#include "ve.h"


X jtxev1(J jt,A w,C*s){A y; 
 RZ(df1(y,cvt(XNUM,w),eval(s)));
 ASSERTSYS(!AR(y),"xev1");
 if(!(XNUM&AT(y)))RZ(y=cvt(XNUM,y)); 
 R XAV(y)[0];
}

X jtxev2(J jt,A a,A w,C*s){A y; 
 RZ(df2(y,cvt(XNUM,a),cvt(XNUM,w),eval(s))); 
 ASSERTSYS(!AR(y),"xev2");
 if(!(XNUM&AT(y)))RZ(y=cvt(XNUM,y)); 
 R XAV(y)[0];
}

X jtxc(J jt,I n){I m=1,p,*zv;X z; 
 p=n; NOUNROLL while(p/=XBASE)++m;
 GATV0(z,INT,m,1); zv=AV(z);
 p=n; DO(m, zv[i]=p%XBASE; p/=XBASE;);
 R z;
}    /* n is non-negative */

D xdouble(X w){D z=0;I c,n,*v;
 n=AN(w); v=n+AV(w); c=*--v;
 if(c==XPINF)R inf; else if(c==XNINF)R infm;
 DQ(n, z=*v--+z*XBASE;);
 R z;   
}

I jtxint(J jt,X w){I c,n,*v,z;
 n=AN(w); v=AV(w); v+=n; c=z=*--v;
 ASSERT(n<=XIDIG&&c!=XPINF&&c!=XNINF,EVDOMAIN);
 DQ(n-1, z=*--v+z*XBASE;);
 ASSERT((c^z)>=0,EVDOMAIN);
 R z;
}

XF1(jtxstd){A z;B b;I c=0,d,i,j,k,m=XBASE,n,*zv;
 ARGCHK1(w);
 n=AN(w); RZ(z=ca(w)); zv=AV(z);
 b=0; j=n; DQ(n, --j; if(zv[j]){b=0<zv[j]; break;});
 if(b) for(i=0;i<n;++i){
  k=zv[i]+=c; 
  if     (0>  k){c=-1-(-k)/m; zv[i]=d=m-(-k)%m; if(d== m){zv[i]=0; ++c;}}
  else if(m<= k){c=k/m;       zv[i]=k%m;}
  else          c=0;
 }else for(i=0;i<n;++i){
  k=zv[i]+=c; 
  if     (0<  k){c=1+k/m;     zv[i]=d=(k%m)-m;  if(d==-m){zv[i]=0; --c;}}
  else if(m<=-k){c=-(-k)/m;   zv[i]=-(-k)%m;}
  else          c=0;
 }
 if(c)R apip(z,sc(c));
 j=n-1; NOUNROLL while(j&&!zv[j])--j; ++j;
 R j==n?z:vec(INT,j,zv);
}    /* convert to standard form */

I jtxcompare(J jt,X a,X w){I*av,j,m,n,x,y,*wv;int s,t;
 RE(1);
 m=AN(a); av=AV(a); x=av[m-1]; s=SGN(x);
 n=AN(w); wv=AV(w); y=wv[n-1]; t=SGN(y);
 if(s!=t)R s?s:-t;
 if(1==m&&(x==XPINF||x==XNINF))R 0<x? !(1==n&&y==XPINF):-!(1==n&&y==XNINF);
 if(1==n&&(y==XPINF||y==XNINF))R 0<y?-!(1==m&&x==XPINF): !(1==m&&x==XNINF);
 if(m!=n)R m>n?s:-s;
 j=m; DQ(m, --j; if(av[j]!=wv[j])R av[j]>wv[j]?1:-1;);
 R 0;
}    /* _1 0 1 depending on whether a<w, a=w, a>w */


XF1(jtxsgn){I x=XDIG(w); R xc(SGN(x));}

XF2(jtxplus){PROLOG(0094);A z;I an,*av,c,d,m,n,wn,*wv,*zv;
 ARGCHK2(a,w);
 an=AN(a); av=AV(a); c=av[an-1];
 wn=AN(w); wv=AV(w); d=wv[wn-1];
 if(c==XPINF||c==XNINF||d==XPINF||d==XNINF){
  ASSERT(!(c==XPINF&&d==XNINF||c==XNINF&&d==XPINF),EVNAN);
  R rifvsdebug(vci(c==XPINF||d==XPINF?XPINF:XNINF));
 }
 m=MAX(an,wn); n=MIN(an,wn);
 GATV0(z,INT,m,1); zv=AV(z);
 DQ(n, *zv++=*av+++*wv++;);
 if(m>n)ICPY(zv,an>wn?av:wv,m-n);
 z=xstd(z);
 EPILOGNOVIRT(z);
}

XF2(jtxminus){PROLOG(0095);A z;I an,*av,c,d,m,n,wn,*wv,*zv;
 ARGCHK2(a,w);
 an=AN(a); av=AV(a); c=av[an-1];
 wn=AN(w); wv=AV(w); d=wv[wn-1];
 if(c==XPINF||c==XNINF||d==XPINF||d==XNINF){
  ASSERT(c!=d,EVNAN);
  R rifvsdebug(vci(c==XPINF||d==XNINF?XPINF:XNINF));
 }
 m=MAX(an,wn); n=MIN(an,wn);
 GATV0(z,INT,m,1); zv=AV(z);
 DQ(n, *zv++=*av++-*wv++;);
 if(m>n){if(an>wn)ICPY(zv,av,m-n); else DQ(m-n, *zv++=-*wv++;);}
 z=xstd(z);
 EPILOGNOVIRT(z);
}

XF2(jtxtymes){A z;I an,*av,c,d,e,i,j,m=XBASE,n,*v,wn,*wv,*zv;
 ARGCHK2(a,w);
 an=AN(a); av=AV(a); c=av[an-1];
 wn=AN(w); wv=AV(w); d=wv[wn-1];
 if(!c||!d)R iv0;
 if(c==XPINF||c==XNINF||d==XPINF||d==XNINF)R rifvsdebug(vci(0<c*d?XPINF:XNINF));
 n=an+wn; GATV0(z,INT,n,1); zv=v=AV(z); mvc(n*SZI,zv,1,MEMSET00);
 for(i=0;i<an;++i,++zv){
  if(c=av[i])for(j=0;j<wn;++j){
   d=zv[j]+=c*wv[j];
   if     (m<= d){e=  d /m; zv[j]-=e*m; zv[1+j]+=e;}
   else if(m<=-d){e=(-d)/m; zv[j]+=e*m; zv[1+j]-=e;}
 }}
 R rifvsdebug(v[n-1]?z:vec(INT,v[n-2]?n-1:1,v));
}

static X jtshift10(J jt,I e,X w){A z;I c,d,k,m,n,q,r,*wv,*zv;
 n=AN(w); wv=AV(w); c=wv[n-1];
 q=e/XBASEN; r=e%XBASEN; d=0==r?1:1==r?10:2==r?100:1000;
 m=n+q+(I )(XBASE<=c*d);
 GATV0(z,INT,m,1); zv=AV(z);
 DQ(q, *zv++=0;);
 if(r){c=0; DQ(n, k=c+d**wv++; *zv++=k%XBASE; c=k/XBASE;); if(c)*zv=c;}
 else DQ(n, *zv++=*wv++;);
 R z;
}    /* w*10^e, positive w */

B jtxdivrem(J jt,X a,X w,X*qz,X*rz){B b,c;I*av,d,j,n,*qv,r,y;X q;
 j=n=AN(a); av=AV(a); b=0<=av[n-1];
 y=AV(w)[0]; c=0<=y; if(!c)y=-y; r=0;
 GATV0(q,INT,n,1); qv=AV(q);
 switch(2*b+c){
  case 0: DQ(n, --j; d=r*XBASE-av[j]; r=d%y; qv[j]=  d/y ;); r=-r;      break;
  case 1: DQ(n, --j; d=r*XBASE-av[j]; r=d%y; qv[j]=-(d/y);); r=r?y-r:0; break;
  case 2: DQ(n, --j; d=r*XBASE+av[j]; r=d%y; qv[j]=-(d/y);); r=r?r-y:0; break;
  case 3: DQ(n, --j; d=r*XBASE+av[j]; r=d%y; qv[j]=  d/y ;);            break;
 }
 if(r&&b!=c){--qv[0]; DO(n-1, if(qv[i]>-XBASE)break; qv[i]=0; --qv[1+i];);}
 if(1<n&&!qv[n-1])AN(q)=AS(q)[0]=n-1;
 *qz=q; *rz=rifvsdebug(vec(INT,1L,&r)); R 1;
}    /* (<.a%w),(w|a) where w has a single "digit" and is nonzero */
 
X jtxdiv(J jt,X a,X w,I mode){PROLOG(0096);B di;I an,*av,c,c0,d,e,k,s,u[2],u1,wn,*wv,yn;X q,r,y;
 RZ(a&&w&&!jt->jerr);
 an=AN(a); av=AV(a); c=c0=av[an-1];
 wn=AN(w); wv=AV(w); d=   wv[wn-1]; di=d==XPINF||d==XNINF;
 if(c&&!d)R rifvsdebug(vci(0<c?XPINF:XNINF));
 if(c==XPINF||c==XNINF){ASSERT(!di,EVNAN); R rifvsdebug(vci(0<c*d?XPINF:XNINF));}
 if(di)R iv0;
 if(1==wn&&d){I*v;
  RZ(xdivrem(a,w,&q,&r));  // must not return virtual
  if(!AV(r)[0]||mode==XMFLR)R q;
  ASSERT(mode==XMCEIL,EWRAT);
  v=AV(q); ++*v; 
  R rifvsdebug(XBASE>*v?q:xstd(q));
 }
 switch((0<=c?2:0)+(I )(0<=d)){
  case 0: R rifvsdebug(xdiv(negate(a),negate(w),mode));
  case 1: R rifvsdebug(negate(xdiv(negate(a),w,mode==XMFLR?XMCEIL:mode==XMCEIL?XMFLR:mode)));
  case 2: R rifvsdebug(negate(xdiv(a,negate(w),mode==XMFLR?XMCEIL:mode==XMCEIL?XMFLR:mode)));
  default:
   if(1!=(e=xcompare(a,w))){
    ASSERT(!(c&&e&&mode==XMEXACT),EWRAT); 
    d=c&&(mode||!e); 
    R rifvsdebug(vec(INT,1L,&d));
   }
   if(1<an)c=av[an-2]+c*XBASE;
   if(1<wn)d=wv[wn-2]+d*XBASE;
   e=c>=d?c/d:(I)((XBASE*(D)c)/d); u[0]=e%XBASE; u[1]=u1=e/XBASE; 
   RZ(q=vec(INT,u1?2L:1L,u)); 
   RZ(y=xtymes(w,q)); yn=AN(y); e=AV(y)[yn-1];
   k=c0>=e?c0/e:e/c0; 
   k=(k>3)+(k>32)+(k>316)+(k>3162);
   s=XBASEN*(an-yn)+(c0>=e?k:-k); 
   if(s){q=shift10(s,q); y=shift10(s,y);}
   A z=xplus(q,xdiv(xminus(a,y),w,mode));
   EPILOGNOVIRT(z);
  }
}   /* <.a%w (mode=XMFLR) or >.a%w (mode=XMCEIL) or a%w (mode=XMEXACT) */

XF2(jtxrem){I c,d,e;X q,r,y;
 ARGCHK2(a,w);
 c=XDIG(a); d=XDIG(w);
 if(!c)R rifvs(w);
 ASSERT(!(d==XPINF||d==XNINF),EVNAN);
 if(c==XPINF)R 0<=d?w:a;
 if(c==XNINF)R 0>=d?w:a;
 if(1==AN(a)&&c){RZ(xdivrem(w,a,&q,&r)); R rifvsdebug(r);}
 switch((0<=c?2:0)+(I )(0<=d)){
  case 0:  R rifvsdebug(negate(xrem(negate(a),negate(w))));
  case 1:  y=xrem(negate(a),w); R rifvsdebug(xcompare(y,iv0)? xplus(a,y):y);
  case 2:  y=xrem(a,negate(w)); R rifvsdebug(xcompare(y,iv0)?xminus(a,y):y);
  default: R rifvsdebug(0<=(e=xcompare(a,w)) ? (e?w:iv0) : xminus(w,xtymes(a,xdiv(w,a,XMFLR))));
}}
                                             
XF2(jtxgcd){I c,d;X p,q,t;
 ARGCHK2(a,w);
 c=XDIG(a); if(0>c)RZ(a=negate(a)); 
 d=XDIG(w); if(0>d)RZ(w=negate(w));
 ASSERT(!(c==XPINF||c==XNINF||d==XPINF||d==XNINF),EVNAN);
 if(!c)R rifvsdebug(w);
 if(!d)R rifvsdebug(a);
 p=a; q=w; A *old=jt->tnextpushp;
 while(XDIG(p)){
  t=p;
  RZ(p=xrem(p,q));
  q=t;
  if(!gc3(&p,&q,0L,old))R0;
 }
 R rifvsdebug(q);
}

XF2(jtxlcm){R rifvsdebug(xtymes(a,xdiv(w,xgcd(a,w),XMEXACT)));}

static X jtxexp(J jt,X w,I mode){I k,m;X s,y;
 ARGCHK1(w);
 k=XDIG(w);
 ASSERT(!k||mode!=XMEXACT,EWIRR);
 if(0>k)R rifvsdebug(xc(mode));
 m=(I)(2.718281828*xint(w)); k=2; s=xplus(iv1,w); y=w;
 DQ(m, y=xtymes(y,w); s=xplus(xtymes(s,xc(k)),y); ++k;);
 R rifvsdebug(xdiv(s,xev1(apv(1+m,1L,1L),"*/"),mode));
}

XF2(jtxpow){PROLOG(0097);I c,d,e,r;X m,t,z;
 ARGCHK2(a,w);
 c=XDIG(a); d=XDIG(w); e=AV(w)[0];
 if(c==XPINF||c==XNINF){
  ASSERT(0<c||d!=XPINF,EVDOMAIN); 
  R rifvsdebug(vci(!d?1L:0>d?0L:0<c?c:1&e?XNINF:XPINF));
 }
 if(d==XPINF||d==XNINF){
  ASSERT(0<=c||d==XNINF,EVDOMAIN); 
  R rifvsdebug(vci(1==c&&1==AN(a)?1L:!c&&0>d||c&&0<d?XPINF:0L));
 }
 if(1==AN(a)&&(1==c||-1==c))R 1==c||0==(e&1)?iv1:xc(-1L); 
 if(!c){ASSERT(0<=d,EWRAT); R d?iv0:iv1;}
 if(0>d){
  ASSERT(!jt->xmod,EVDOMAIN); 
  ASSERT(jt->xmode!=XMEXACT,EWRAT); 
  r=jt->xmode==XMCEIL; R rifvsdebug(xc(0<c?r:1&e?r-1:r));
 }
 t=a; z=iv1; m=jt->xmod?XAV(jt->xmod)[0]:0;
 if(!m||1>xcompare(w,xc(IMAX))){
  ASSERT(m||2>=AN(w),EVLIMIT);
  RE(e=xint(w));
  if(m)while(e){if(1&e)RZ(z=xrem(m,xtymes(z,t))); RZ(t=xrem(m,xsq(t))); e>>=1;}
  else while(e){if(1&e)RZ(z=       xtymes(z,t) ); RZ(t=       xsq(t) ); e>>=1;}
 }else{B b;I n,*u,*v;X e;
  RZ(e=ca(w)); n=AN(e); v=AV(e);
  while(n){
   if(1&*v)RZ(z=xrem(m,xtymes(z,t))); 
   RZ(t=xrem(m,xtymes(t,t))); 
   b=1; c=0; u=v+n;
   DQ(n, d=c+*--u; c=1&d?XBASE:0; *u=d>>1; if(b&=!*u)--n;);  /* e=.<.e%2 */
 }}
 EPILOGNOVIRT(z);
}

XF1(jtxsq){R xtymes(w,w);}

XF1(jtxsqrt){I c,m,n,p,q,*wv;X e,x;
 ARGCHK1(w);
 n=AN(w); wv=AV(w); c=wv[n-1];
 ASSERT(0<=c,EWIMAG);
 if(!(1&n))c=wv[n-2]+c*XBASE;
 m=(1+n)>>1; RZ(x=apvwr(m,0L,0L)); AV(x)[m-1]=(I)sqrt((D)c);
 RZ(e=xc(2L));
 p=m*XBASEN; q=0; while(p){++q; p>>=1;} 
 DQ(1+q, RZ(x=xdiv(xplus(x,xdiv(w,x,XMFLR)),e,XMFLR)););
 p=xcompare(w,xsq(x));
 switch(jt->xmode){
  case XMEXACT: 
   if(!p)R rifvsdebug(x); 
   AV(x)[0]+=p; RZ(x=xstd(x));
   ASSERT(!xcompare(w,xsq(x)),EWIRR);
   R rifvsdebug(x);
  default:     ASSERTSYS(0,"xsqrt");
  case XMFLR:  if(-1==p){--AV(x)[0]; R xstd(x);}else R rifvsdebug(x);
  case XMCEIL: if( 1==p){++AV(x)[0]; R xstd(x);}else R rifvsdebug(x);
}}

static XF2(jtxroot){A q;D x;I an,*av,c,d,r,wn,*wv;X n,n1,p,t,z;
 an=AN(a); av=AV(a); c=av[an-1];
 wn=AN(w); wv=AV(w); d=wv[wn-1]; 
 ASSERT(0<=d,EWIMAG);
 if(1==wn&&((d&~1)==0))R 1==d?iv1:0<=c?iv0:vci(XPINF);
 if(!c&&0<d)R rifvsdebug(vci(XPINF));
 r=xint(a); if(jt->jerr){RESETERR; R iv1;}
 if(2==r)R xsqrt(w);
 x=xlogabs(w)/r;
 if(x<709.78){RZ(q=ceil1(cvt(RAT,scf(exp(x))))); z=XAV(q)[0];}
 else        {RZ(q=cvt(XNUM,scf(jceil(x)))); z=xexp(XAV(q)[0],XMCEIL);}
 RZ(n=xc(r)); RZ(n1=xc(r-1));
 RZ(t=xdiv(w,p=xpow(z,n1),XMFLR));
 RZ(z=xdiv(xplus(t,xtymes(z,n1)),n,XMFLR))
 NOUNROLL while(1){
  RZ(t=xdiv(w,p=xpow(z,n1),XMFLR));
  if(1>xcompare(z,t))break;
  RZ(z=xdiv(xplus(t,xtymes(z,n1)),n,XMFLR))
 }
 if(XMFLR==jt->xmode||!xcompare(w,xtymes(z,p)))R rifvsdebug(z);
 if(XMCEIL==jt->xmode)R rifvsdebug(xplus(z,iv1));
 ASSERT(0,EWIRR);
}

D jtxlogabs(J jt,X w){D c;I m,n,*v;
 n=AN(w); m=MIN(n,20/XBASEN); v=n+AV(w);
 c=0.0; DQ(m, c=c*XBASE+(D)*--v;);
 R log(ABS(c))+XBASEN*(n-m)*2.3025850929940457;
}

static XF1(jtxlog1){B b;I c;
 c=XDIG(w); b=1==c&&1==AN(w);
 ASSERT(0<=c,EWIMAG);
 ASSERT(b||jt->xmode!=XMEXACT,EWIRR);
 R rifvsdebug(xc((I)xlogabs(w)+(I )(!b&&jt->xmode==XMCEIL)));
}

static D jtxlogd1(J jt,X w){ASSERT(0<=XDIG(w),EWIMAG); R xlogabs(w);}

static Z jtxlogz1(J jt,X w){Z z; z.re=xlogabs(w); z.im=0>XDIG(w)?PI:0.0; R z;}


static XF2(jtxlog2sub){ASSERT(0,EVNONCE);}

static XF2(jtxlog2){D c,d,x,y;I an,*av,j,k,m,n,wn,*wv;X p,q;
 ARGCHK2(a,w);
 an=AN(a); av=AV(a); c=(D)av[an-1]; if(1<an)c=av[an-2]+c*XBASE;
 wn=AN(w); wv=AV(w); d=(D)wv[wn-1]; if(1<wn)d=wv[wn-2]+d*XBASE;
 if(2<an)R rifvsdebug(xlog2sub(a,w));
 ASSERT(0<=c,EWIMAG);
 if(!c){ASSERT(d!=0,EVDOMAIN); R iv0;}
 if(!d){ASSERT(0<c,EVDOMAIN); R rifvsdebug(vci(XNINF));}
 ASSERT(0<d,EVDOMAIN);
 if(1==c)R rifvsdebug(1==d?iv0:vci(XPINF));
 x=log(c)+XBASEN*(2<an?an-2:0)*2.3025850929940457;
 y=log(d)+XBASEN*(2<wn?wn-2:0)*2.3025850929940457;
 m=n=(I)(y/x+(I )(an<wn));
 RZ(p=q=xpow(a,xc(m))); j=k=xcompare(p,w);
 if     (0<j){--m; RZ(p=xdiv(p,a,XMEXACT)); j=xcompare(p,w); if(0<j)R rifvsdebug(xlog2sub(a,w));}
 else if(0>j){++n; RZ(q=xtymes(p,a));       k=xcompare(q,w); if(0>k)R rifvsdebug(xlog2sub(a,w));}
 ASSERT(jt->xmode!=XMEXACT||!j||!k,EWIRR); 
 R rifvsdebug(xc(!j?m:!k?n:jt->xmode==XMCEIL?n:m));
}

F2(jtxlog2a){A z; GAT0(z,XNUM,1L,0L); XAV(z)[0]=rifvsdebug(xlog2(XAV(a)[0],XAV(w)[0])); RNE(z);}
F2(jtxroota){A z; GAT0(z,XNUM,1L,0L); XAV(z)[0]=rifvsdebug(xroot(XAV(a)[0],XAV(w)[0])); RNE(z);}

XF1(jtxfact){I n;
 n=AV(w)[0];
 if(n==XPINF||n==XNINF)R vci(XPINF);
 RE(n=xint(w)); 
 if(0>n)R rifvsdebug(vci(n&1?XPINF:XNINF));
 R rifvsdebug(xev1(apv(n,1L,1L),"*/"));
}

static XF2(jtxbinp){PROLOG(0098);D m;I i,n;X c,d,p,q,r,s;
 RZ(d=xminus(w,a)); s=1==xcompare(a,d)?d:a; RE(n=xint(s));
 m=xdouble(w);
 if(m<=FLIMAX){
  RZ(p=less(ravel(factor(apv(n,(I)m,-1L))),zeroionei(0)));
  RZ(q=less(ravel(factor(apv(n,1L,   1L))),zeroionei(0)));
  c=over(p,q);
  d=repeat(v2(AN(p),AN(q)),v2(1L,-1L));
  A z=xev1(repeat(ev2(c,d,"+//."),nub(c)),"*/");
  EPILOGNOVIRT(z);
 }else{
  p=q=iv1; r=w;  
  for(i=0;i<n;++i){
   p=xtymes(p,r); r=xminus(r,iv1);  
   q=xtymes(q,s); s=xminus(s,iv1);
   d=xgcd(p,q); p=xdiv(p,d,XMEXACT); q=xdiv(q,d,XMEXACT);
   if(jt->jerr)R 0;
  }
  EPILOGNOVIRT(p);
}}   /* non-negative x,y; x<=y */

XF2(jtxbin){X d,z;
 RZ(d=xminus(w,a));
 switch(4*(I )(0>XDIG(a))+2*(I )(0>XDIG(w))+(I )(0>XDIG(d))){
  default:             ASSERTSYS(0,"xbin");
  case 2: /* 0 1 0 */  /* Impossible */
  case 5: /* 1 0 1 */  /* Impossible */
  case 1: /* 0 0 1 */ 
  case 4: /* 1 0 0 */ 
  case 7: /* 1 1 1 */  R iv0;
  case 0: /* 0 0 0 */  R rifvsdebug(xbinp(a,w));
  case 3: /* 0 1 1 */  
   z=xbinp(a,xminus(a,xplus(w,iv1)));           R rifvsdebug(AV(a)[0]&1?negate(z):z);
  case 6: /* 1 1 0 */  
   z=xbinp(xminus(xc(-1L),w),xminus(xc(-1L),a)); R rifvsdebug(AV(d)[0]&1?negate(z):z);
 }
}

static A jtpiev(J jt,I n,X b){A e;I ek,i,n1=n-1;X bi,e0,e1,*ev,t;
 GATV0(e,XNUM,n,1); ev=XAV(e);
 bi=e0=e1=iv1;
 for(i=0,ek=1;i<n1;++i,ek+=3){
  ev[i]=xtymes(e0,xtymes(XCUBE(e1),bi));
  t=xtymes(xc(ek),xtymes(xc(1+ek),xc(2+ek)));
  e0=xtymes(e0,t);        /* e0 = ! 3*i */
  e1=xtymes(e1,xc(1+i));  /* e1 = ! i   */
  bi=xtymes(bi,b);        /* bi = b^i   */
 }
 ev[i]=rifvsdebug(xtymes(e0,xtymes(XCUBE(e1),bi)));
 RE(e); R e;
}

static XF1(jtxpi){A e;B p;I i,n,n1,sk;X a,b,c,d,*ev,k,f,m,q,s,s0,t;
 ARGCHK1(w);
 if(!XDIG(w))R iv0;
 ASSERT(jt->xmode!=XMEXACT,EVDOMAIN);
 RZ(a=xc(545140134L));
 RZ(b=XCUBE(xc(640320L)));
 RZ(c=xc(13591409L));
 RZ(d=xplus(xc(541681608L),xtymes(xc(10L),xc(600000000L))));
 n1=(13+AN(w)*XBASEN)/14; n=1+n1;
 RZ(e=piev(n,b)); ev=XAV(e); m=ev[n1];
 f=iv0; s0=iv1; sk=1;
 for(i=p=0;;++i,p^=1){
  s=xtymes(s0,xplus(c,xtymes(a,xc(i))));
  t=xdiv(xtymes(s,m),ev[i],XMEXACT);
  f=p?xminus(f,t):xplus(f,t);
  if(i==n1)break;
  DO(6, s0=xtymes(s0,xc(sk++));); RE(s0);  /* s0 = ! 6*i */
 }
 f=xtymes(d,f);
 q=xpow(xc(10L),xc(14*n1));
 k=xtymes(xtymes(a,m),xsqrt(xtymes(b,xsq(q))));
 R rifvsdebug(xdiv(xtymes(k,w),xtymes(f,q),jt->xmode));
}    /* Chudnovsky Bros. */

APFX( plusXX, X,X,X, xplus ,,HDR1JERR)
APFX(minusXX, X,X,X, xminus,,HDR1JERR)
APFX(tymesXX, X,X,X, xtymes,,HDR1JERR)
APFX(  divXX, X,X,X, XDIV  ,,HDR1JERR)
APFX(  remXX, X,X,X, xrem  ,,HDR1JERR)
APFX(  gcdXX, X,X,X, xgcd  ,,HDR1JERR)
APFX(  lcmXX, X,X,X, xlcm  ,,HDR1JERR)
APFX(  minXX, X,X,X, XMIN  ,,HDR1JERR)
APFX(  maxXX, X,X,X, XMAX  ,,HDR1JERR)
APFX(  powXX, X,X,X, xpow  ,,HDR1JERR)
APFX(  binXX, X,X,X, xbin  ,,HDR1JERR)

AMON( sgnX, X,X, *z=  rifvsdebug(xsgn(*x));)
AMONPS(sqrtX, X,X, , *z= rifvsdebug(xsqrt(*x)); , HDR1JERR)
AMONPS( expX, X,X, , *z=  rifvsdebug(xexp(*x,jt->xmode)); , HDR1JERR)
AMONPS( logX, X,X, , *z= rifvsdebug(xlog1(*x)); , HDR1JERR)
AMONPS(logXD, D,X, , *z=xlogd1(*x); , HDR1JERR)
AMON(logXZ, Z,X, *z=xlogz1(*x);)
AMONPS( absX, X,X, , *z=   rifvs(mag(*x)); , HDR1JERR)
AMONPS(factX, X,X, , *z= rifvsdebug(xfact(*x)); , HDR1JERR)
AMONPS( pixX, X,X, , *z=   rifvsdebug(xpi(*x)); , HDR1JERR)


F1(jtdigits10){A z;B b=0;I c,m,n,*v,*zv,*zv0;X x;
 ARGCHK1(w);
 if(!AR(w))switch(CTTZ(AT(w))){
  case INTX:  b=0<=AV(w)[0]; break;
  case XNUMX: x=XAV(w)[0]; n=AN(x); v=AV(x); b=0<=v[n-1]; break;
  case RATX:  x=XAV(w)[0]; n=AN(x); v=AV(x); b=0<=v[n-1]&&equ(iv1,QAV(w)->d);
 }
 if(!b)R rank1ex0(thorn1(w),DUMMYSELF,jtexec1);
 m=INT&AT(w)?(SY_64?19:10):XBASEN*AN(x);
 GATV0(z,INT,m,1); zv=zv0=AV(z);
 if(INT&AT(w)){c=AV(w)[0]; *zv++=c%10; while(c/=10)*zv++=c%10;}
 else{
  DQ(n-1, c=*v++; DQ(XBASEN, *zv++=c%10; c/=10;););
  c=*v++; if(c||1==n)*zv++=c%10; while(c/=10)*zv++=c%10;
 }
 AN(z)=AS(z)[0]=n=zv-zv0; 
 zv=zv0; v=zv0+n-1; DQ(n>>1, c=*zv; *zv++=*v; *v--=c;); /* reverse in place */
 RETF(z);
}    /* "."0@": w */


#define DXBODY(exp)  DECLG;A y=sv->fgh[2],z;I m=jt->xmode; jt->xmode=XMFLR; z=exp; jt->xmode=m; R z
#define DX1(f,exp)   DF1(f){DXBODY(exp);} 
#define DX2(f,exp)   DF2(f){DXBODY(exp);}
#define XT(w)        tymes(y,w)

static DX1(postmult1, XT(CALL1(g1,  w,gs)))
static DX2(postmult2, XT(CALL2(g2,a,w,gs)))

static DX1(premult1, CALL1(g1,      XT(w),gs))
static DX2(premult2, CALL2(g2,XT(a),XT(w),gs))

static DX1(ydiv1, CALL2(g2,y,    w,gs))
static DX2(ydiv2, CALL2(g2,XT(a),w,gs))

static DX1(ysqrt, CALL1(g1,tymes(w,XT(y)),gs))
