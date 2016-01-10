/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Words: Numeric Input Conversion                                         */

#include "j.h"

#if (SYS & SYS_LINUX)
#include <stdlib.h>
#endif

#define NUMH(f)  B f(J jt,I n,C*s,void*vv)

/* numd    floating point number (double)                      */
/* numj    complex number                                      */
/* numx    extended precision integer                          */
/* nume    extended precision floating point number (not used) */
/* numr    rational number                                     */
/* numq    nume or numr                                        */
/* numbpx  3b12 or 3p12 or 3x12 number                         */
/*                                                             */
/* numb    subfunction of numbpx                               */
/*                                                             */
/* converts a single number and assigns into result pointer    */
/* returns 0 if error, 1 if ok                                 */

static NUMH(jtnumd){C c,*t;D*v,x,y;
 RZ(n);
 v=(D*)vv;
 if('-'==*s&&3>n)
  if(1==n){*v=inf; R 1;}
  else{
   c=*(1+s);
   if('-'==c){*v=infm; R 1;}
   else if('.'==c){*v=jnan; R 1;}
  }
 x=strtod(s,&t);
 if(t<s+n-1&&'r'==*t){y=strtod(1+t,&t); x=y?x/y:0<x?inf:0>x?infm:0;}
 R t>=s+n?(*v=x,1):0;
}

static NUMH(jtnumj){C*t,*ta;D x,y;Z*v;
 v=(Z*)vv;
 if(t=memchr(s,'j',n))ta=0; else t=ta=memchr(s,'a',n);
 RZ(numd(t?t-s:n,s,&x));
 if(t){t+=ta?2:1; RZ(numd(n+s-t,t,&y));} else y=0;
 if(ta){C c;
  c=*(1+ta);
  RZ(0<=x&&(c=='d'||c=='r'));
  if(c=='d')y*=PI/180; if(y<=-P2||P2<=y)y-=P2*jfloor(y/P2); if(0>y)y+=P2;
  v->re=y==0.5*PI||y==1.5*PI?0:x*cos(y); v->im=y==PI?0:x*sin(y);
 }else{v->re=x; v->im=y;}
 R 1;
}

static NUMH(jtnumi){B neg;C*t;I j;static C*dig="0123456789";
 if(neg='-'==*s){++s; --n; RZ(n);}
 RZ(19>=n);
 j=0; DO(n, RZ(t=memchr(dig,*s++,10L)); j=10*j+(t-dig););
 RZ(0<=j||neg&&j==IMIN);
 *(I*)vv=0>j||!neg?j:-j;
 R 1;
}     /* called only if SY_64 */

static NUMH(jtnumx){A y;B b,c;C d,*t;I j,k,m,*yv;X*v;static C*dig="0123456789";
 v=(X*)vv;
 d=*(s+n-1); b='-'==*s; c='x'==d||'r'==d; s+=b;
 if('-'==d){RZ(2>=n); RZ(*v=vci(1==n?XPINF:XNINF)); R 1;}
 n-=b+c; RZ(m=(n+XBASEN-1)/XBASEN); k=n-XBASEN*(m-1);
 GA(y,INT,m,1,0); yv=m+AV(y);
 DO(m, j=0; DO(k, RZ(t=memchr(dig,*s++,10L)); j=10*j+(t-dig);); *--yv=b?-j:j; k=XBASEN;);
 RZ(*v=yv[m-1]?y:xstd(y));
 R 1;
}

static X jtx10(J jt,I e){A z;I c,m,r,*zv;
 m=1+e/XBASEN; r=e%XBASEN;
 GA(z,INT,m,1,0); zv=AV(z);
 DO(m-1, *zv++=0;);
 c=1; DO(r, c*=10;); *zv=c;
 R z;
}     /* 10^e as a rational number */

static NUMH(jtnume){C*t,*td,*te;I e,ne,nf,ni;Q f,i,*v,x,y;
 v=(Q*)vv;
 nf=0; i.d=iv1; f.d=iv1;
 if(te=memchr(s,'e',n)){ne=n-(te-s)-1; e=strtoI(1+te,&t,10);  RZ(!*t&&10>ne);}
 if(td=memchr(s,'.',n)){nf=te?(te-td)-1:n-(td-s)-1; if(nf)RZ(numx(nf,td+1,&f.n));}
 ni=td?td-s:te?te-s:n; RZ(numx(ni,s,&i.n));
 x=i;
 if(nf){y.n=iv1; y.d=x10(nf); RE(x='-'==*s?qminus(x,qtymes(f,y)):qplus(x,qtymes(f,y)));}
 if(te){if(0>e){y.n=iv1; y.d=x10(-e);}else{y.n=x10(e); y.d=iv1;} RE(x=qtymes(x,y));}
 *v=x;
 R 1;
}

static NUMH(jtnumr){C c,*t;I m,p,q;Q*v;
 v=(Q*)vv;
 m=(t=memchr(s,'r',n))?t-s:n; RZ(numx(m,s,&v->n)); v->d=iv1;
 if(t){
  c=s[n-1]; RZ('r'!=c&&'x'!=c);
  RZ(numx(n-m-1,s+m+1,&v->d));
  p=*AV(v->n); q=*AV(v->d); 
  RZ(p!=XPINF&&p!=XNINF||q!=XPINF&&q!=XNINF);
  RE(*v=qstd(*v));
 }
 R 1;
}

static NUMH(jtnumq){B b=0;C c,*t;
 t=s; DO(n, c=*t++; if(c=='e'||c=='.'){b=1; break;});
 R b?nume(n,s,vv):numr(n,s,vv);
}

static Z zpi={PI,0};

static B jtnumb(J jt,I n,C*s,Z*v,Z b){A c,d,y;I k;
  static C dig[]="0123456789abcdefghijklmnopqrstuvwxyz";I m=strlen(dig);
 if(!n){*v=zeroZ; R 1;}
 RZ(d=indexof(str(m,dig),str(n,s)));
 RZ(all0(eps(sc(m),d)));
 k=sizeof(Z);
 GA(c,CMPX,1,0,0); MC(AV(c),&b,k); RZ(y=base2(c,d)); MC(v,AV(y),k);
 R 1;
}

static NUMH(jtnumbpx){B ne,ze;C*t,*u;I k,m;Z b,p,q,*v,x,y;
 v=(Z*)vv;
 if(t=memchr(s,'b',n)){
  RZ(numbpx(t-s,s,&b));
  ++t; if(ne='-'==*t)++t;
  m=k=n+s-t; if(u=memchr(t,'.',m))k=u-t;
  RZ(ne||m>(1&&u));
  RZ(numb(k,t,&p,b));
  if(u){
   k=m-(1+k);
   if(ze=!(b.re||b.im))b.re=1;
   RZ(numb(k,1+u,&q,b));
   if(ze){if(q.re)p.re=inf;} else{DO(k,q=zdiv(q,b);); p=zplus(p,q);}
  }
  *v=p; if(ne){v->re=-v->re; v->im=-v->im;}
  R 1;
 }
 if(t=memchr(s,'p',n))u=0; else t=u=memchr(s,'x',n);
 if(!t)R numj(n,s,v);
 RZ(numj(t-s,s,&x)); ++t; RZ(numj(n+s-t,t,&y));
 if(u)*v=ztymes(x,zexp(y)); else *v=ztymes(x,zpow(zpi,y));
 R 1;
}

/* (n,s) string containing the vector constant                */
/* j:  1 iff contains 1j2 or 1ad2 or 1ar2                     */
/* b:  1 iff has 1b1a or 1p2 or 1x2 (note: must handle 1j3b4) */
/* x:  1 iff contains 123x                                    */
/* q:  1 iff contains 3r4                                     */
/* ii: 1 iff integer (but not x)                              */

static void jtnumcase(J jt,I n,C*s,B*b,B*j,B*x,B*q,B*ii){B e;C c;
 *x=*q=*ii=0;
 *j=memchr(s,'j',n)||memchr(s,'a',n);
 *b=memchr(s,'b',n)||memchr(s,'p',n);
 if(!*j&&!*b){
#if SY_64
  *ii=1;
#endif
  if(memchr(s,'x',n)){*b=*x=1; *ii=0;}
  if(memchr(s,'r',n)){*q=1;    *ii=0;}
  if(!*x&&!*q&!*ii)R;
  DO(n, c=s[i]; e=!s[1+i]; if(c=='.'||c=='e'||c=='x'&&!e){*x=*q=*ii=0; R;});
}}

A jtconnum(J jt,I n,C*s){PROLOG;A y,z;B b,(*f)(),ii,j,p=1,q,x;C c,*v;I d=0,e,k,m,t,*yv;
 if(1==n)                {if(k=s[0]-'0',0<=k&&k<=9)R num[ k]; else R ainf;}
 else if(2==n&&CSIGN==*s){if(k=s[1]-'0',0<=k&&k<=9)R num[-k];}
 RZ(y=str(1+n,s)); s=v=CAV(y); s[n]=0;
 GA(y,INT,1+n,1,0); yv=AV(y);
 DO(n, c=*v; *v++=c=c==CSIGN?'-':c==CTAB||c==' '?C0:c; b=C0==c; if(p!=b)yv[d++]=i; p=b;);
 if(d%2)yv[d++]=n; m=d/2;
 numcase(n,s,&b,&j,&x,&q,&ii);
 f=q?jtnumq:x?jtnumx:b||j?jtnumbpx:ii?jtnumi:jtnumd; 
 t=q?RAT   :x?XNUM  :b||j?CMPX    :ii?INT   :FL;     k=bp(t);
 GA(z,t,m,1!=m,0); v=CAV(z);
 if(ii){
  DO(m, d=i+i; e=yv[d]; if(!numi(yv[1+d]-e,e+s,v)){ii=0; break;} v+=k;);
  if(!ii){t=FL; f=jtnumd; GA(z,t,m,1!=m,0); v=CAV(z);}
 }
 if(!ii)DO(m, d=i+i; e=yv[d]; ASSERT(f(jt,yv[1+d]-e,e+s,v),EVILNUM); v+=k;);
 if(t&FL+CMPX)RZ(z=cvt0(z));
 EPILOG(bcvt(0,z));
}


#define EXEC2F(f,f1,t,T) \
 A f(J jt,A a,A w,I n,I m,I c){A z;B b;C d,*u,*uu,*x,*y;I i,j,k,mc,r;T a0,*zv;  \
  i=0; mc=m*c; u=CAV(w); y=u+n; j=c; uu=u+AN(w); if(mc)*(uu-1)=' ';         \
  r=AR(w)-(1==c); r=MAX(0,r);                                               \
  GA(z,t,mc,r,AS(w)); if(1<r&&1!=c)*(AS(z)+r-1)=c; zv=(T*)AV(z);            \
  RZ(a=cvt(t,a)); a0=*(T*)AV(a);                                            \
  while(i<mc){                                                              \
   while(u<uu&&C0==*u)++u;                                                  \
   while(u>=y){while(i<j)zv[i++]=a0; j+=c; y+=n; if(i==mc)R z;}             \
   x=strchr(u,C0); if(x<uu)k=x-u; else{*(uu-1)=C0; k=uu-1-u;}               \
   b=','==u[0]||','==u[k-1];                                                \
   x=u; DO(k, d=u[i]; if(','!=d)*x++=d==CSIGN?'-':d;); *x=C0;               \
   if(b||!f1(x-u,u,i+zv))zv[i]=a0;                                          \
   ++i; u+=1+k;                                                             \
  }                                                                         \
  R z;                                                                      \
 }

static EXEC2F(jtexec2x,numx,  XNUM,X)  /* note: modifies argument w */
static EXEC2F(jtexec2q,numq,  RAT, Q)
static EXEC2F(jtexec2z,numbpx,CMPX,Z)

static A jtexec2r(J jt,A a,A w,I n,I m,I c){A z;B b,e;C d,*u,*uu,*v,*x,*y;D a0,*zv;I i,j,mc,r;
 i=0; mc=m*c; u=CAV(w); y=u+n; j=c; uu=u+AN(w);
 r=AR(w)-(1==c); r=MAX(0,r); 
 GA(z,FL,mc,r,AS(w)); if(1<r&&1!=c)*(AS(z)+r-1)=c; zv=DAV(z); 
 RZ(a=cvt(FL,a)); a0=*DAV(a);
 while(i<mc){
  while(u<uu&&C0==*u)++u; 
  while(u>=y){while(i<j)zv[i++]=a0; j+=c; y+=n; if(i==mc)R z;}
  zv[i]=strtod(u,&v);
  switch(*v){
   case C0:
    i++; u=v; continue;
   case ',':   
    b=u==v; x=v; 
    while(d=*++v)if(','!=d)*x++=d; 
    if(b||','==*(v-1)){zv[i++]=a0; u=v;}else while(v>x)*x++=C0; 
    continue;
   case '-': 
    e=u==v; v++; d=*v++; b=e&&C0==*v;
    if     (e&& C0==d){zv[i++]=inf;  u=v;}
    else if(b&&'-'==d){zv[i++]=infm; u=v;}
    else if(b&&'.'==d){zv[i++]=jnan; u=v;}
    else{zv[i++]=a0; --v; while(C0!=*v++); u=v;}
    continue;
   case 'a':  case 'b':  case 'j':  case 'p':  case 'r':  case 'x':
    if(u!=v)R exec2z(a,w,n,m,c);
   default:  
    zv[i++]=a0; while(C0!=*++v); u=v;
 }}
 R z;
}

F2(jtexec2){A z;B b,ii,j,p,q,x;C d,*v;I at,c,i,k,m,n,r,*s;
 RZ(a&&w);
 ASSERT(!AR(a),EVRANK);
 at=AT(a);
 ASSERT(at&NUMERIC,EVDOMAIN);
 if(!(LIT&AT(w)))RZ(w=toc1(0,w));
 m=n=c=0; r=AR(w);
 if(!r||*(AS(w)+r-1)){
  RZ(w=irs2(w,chr[' '],0L,1L,0L,jtover));  /* will be modified in place */
  v=CAV(w); r=AR(w); s=AS(w); n=s[r-1]; m=prod(r-1,s);
  for(i=0;i<m;++i){I j;
   b=1; k=0; 
   for(j=0;j<n;++j){
    p=b; d=*v; b=' '==d;
    switch(d){
     case ' ':   *v=C0;  break;
     case CSIGN: *v='-'; 
    }
    ++v; if(p>b)++k;
   } 
   if(k>c)c=k;
 }}
 numcase(m*n,CAV(w),&b,&j,&x,&q,&ii);
 if(at&CMPX)                z=cvt0(exec2z(a,w,n,m,c));
 else if(q)                 z=     exec2q(a,w,n,m,c);
 else if(x&&at&B01+INT+XNUM)z=     exec2x(a,w,n,m,c);
 else                       z=cvt0(exec2r(a,w,n,m,c));
 R bcvt(0,z);
}
