/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: !                                                                */

#include "j.h"


static const Z z1={1, 0};

static D coeff[]={
 0.0, 1.0,            0.5772156649015329, -0.6558780715202538, 
 -0.0420026350340952, 0.1665386113822915, -0.0421977345555443,
 -0.009621971527877,  0.007218943246663,  -0.0011651675918591, 
 -0.0002152416741149, 0.0001280502823882, -0.0000201348547807,
 -0.0000012504934821, 0.000001133027232,  -0.0000002056338417,
 6.116095e-9,         5.0020075e-9,       -1.1812746e-9,
 1.043427e-10,        7.7823e-12,         -3.6968e-12,
 5.1e-13,             -2.06e-14,          -5.4e-15,
 1.4e-15,             1.0e-16
};

static I terms=sizeof(coeff)/sizeof(D);

static Z jtzhorner(J jt,I n,D*c,Z v){Z s;D*d=n+c;
 s=zeroZ;
 DQ(n, s=zplus(zrj0(*--d),ztymes(v,s));); 
 R s;
}

static D dgps(D v){D*d=terms+coeff,s=0.0; DQ(terms, s=*--d+v*s;); R 1/s;}
     /* Abramowitz & Stegun, 6.1.34 */

static Z jtzgps(J jt,Z z){R zdiv(z1,zhorner(terms,coeff,z));}

D jtdgamma(J jt,D x){B b;D t;
 t=1.0; b=x==jfloor(x);
 if(b&&0>=x){ASSERT(x>x-1,EVLIMIT); R x==2*jfloor(x/2)?inf:infm;}
 if(0<=x) while(1<x){t*=--x; if(t==inf)R inf;}
 else    {while(0>x){t*=x++; if(t==inf)R 0.0;} t=1.0/t;}
 R b?t:t*dgps(x);
}    /* gamma(x) using recurrence formula */

static Z jtzgrecur(J jt,Z z){Z t;
 t=z1;
 if(0<=z.re) while( 0.5<z.re){--z.re; t=ztymes(t,z); if(t.re==inf)R t;    }
 else       {while(-0.5>z.re){t=ztymes(t,z); ++z.re; if(t.re==inf)R zeroZ;} t=zdiv(z1,t);}
 R ztymes(t,zgps(z));
}    /* gamma(z) using recurrence formula */

static Z jtzgauss(J jt,D n,Z z){D d=1/n;Z p,t;
 if(1>=n)R zgrecur(z);
 p=ztymes(zpow(zrj0(2*PI),zrj0((1-n)/2)),zpow(zrj0(n),zminus(z,zrj0(0.5))));
 t=zdiv(z,zrj0(n));
 DQ((I)n, p=ztymes(p,zgrecur(t)); t.re+=d;);
 R p;
}    /* Abramowitz & Stegun, 6.1.20 */
 
static D c[]={1.0, 1.0/12, 1.0/288, -139.0/51840, -571.0/2488320};
static Z jtzstirling(J jt,Z z){Z p,q;
 p=ztymes(zsqrt(zdiv(zrj0(2*PI),z)),zpow(zdiv(z,zrj0(2.718281828459045235360287)),z));
 q=zhorner(5L,c,zdiv(z1,z));
 R ztymes(p,q);
}    /* Abramowitz & Stegun, 6.1.37 */

static Z jtzgamma(J jt,Z z){D y=ABS(z.im);
 R !y?zrj0(dgamma(z.re)):20<y?zstirling(z):zgauss(jceil(y/0.8660254),z);
}

AMONPS(factI,  D,I, , *z=dgamma(1.0+(D)*x); , HDR1JERR)
AMONPS(factD,  D,D, , *z=_isnan(*x)?*x:dgamma(1.0+*x); , HDR1JERR)
AMONPS(factZ,  Z,Z, , *z=zgamma(zplus(z1,*x)); , HDR1JERR)


#define PQLOOP(expr) while(n&&h&&h!=inf&&h!=infm){h*=expr; --n;}

static D pq(D h,D m,D*c,D*d){D x=*c,y=*d;I n=(I)MIN(m,FLIMAX);
 if(0>=m)R h;
 switch(2*(I )(0>x)+(I )(0>y)){
  case 0: if(x!= y)PQLOOP(x--/y--); break;
  case 1: if(x!=-y)PQLOOP(x--/y++)else if(m>2*jfloor(0.5*m))h=-h; break;
  case 2: if(x!=-y)PQLOOP(x++/y--)else if(m>2*jfloor(0.5*m))h=-h; break;
  case 3: if(x!= y)PQLOOP(x++/y++); break;
 }
 if(0>=*c)*c+=m; else *c-=m;
 if(0>=*d)*d+=m; else *d-=m;
 R h;
}

static I signf(D x){R 0<=x||1<=x-2*jfloor(0.5*x)?1:-1;}
     /* sign of !x */

static D jtdbin(J jt,D x,D y){D c,d,e,h=1.0,p,q,r;I k=0;
 c=y;   if(0<=c)p=jfloor(c); else{k+=4; ++c; p=jfloor(-c);}
 d=y-x; if(0<=d)q=jfloor(d); else{k+=2; ++d; q=jfloor(-d);}
 e=x;   if(0<=e)r=jfloor(e); else{k+=1; ++e; r=jfloor(-e);}
 switch(k){
  case 0: h=pq(h,q,&c,&d); h=pq(h,r,&c,&e);                break;
  case 1: h=pq(h,p,&c,&d); h=pq(h,r,&e,&d);           --e; break;
  case 2: h=pq(h,p,&c,&e); h=pq(h,q,&d,&e);      --d;      break;
  case 5: h=pq(h,p,&e,&c); h=pq(h,q,&e,&d); --c;      --e; break;
  case 6: h=pq(h,p,&d,&c); h=pq(h,r,&d,&e); --c; --d;      break;
  case 7: h=pq(h,q,&d,&c); h=pq(h,r,&e,&c); --c; --d; --e; break;
 }
 if(!h)R 0; 
 if(h==inf||h==infm)R inf*signf(x)*signf(y)*signf(y-x);
 R h*dgamma(1+c)/(dgamma(1+d)*dgamma(1+e));
}    /* x and y-x are not negative integers */

static D ibin(D x,D y){D d=MIN(x,y-x),p=1;
 // if x and y are _, d is NaN.  Conversion to int is undefined then.  Test for it, but we're gonna get a NaN error when we finish
 if(_isnan(d))R 0.0;  // avoid looping if d is invalid now
 DQ((I)d, p*=y--/d--; if(p==inf)R p;);
 R jround(p);
}    /* x and y are non-negative integers; x<=y */

static Z jtzbin(J jt,Z x,Z y){Z a,b,c;
 a=zgamma(zplus(z1,y));
 b=zgamma(zplus(z1,x));
 c=zgamma(zplus(z1,zminus(y,x)));
 R zdiv(a,ztymes(b,c));
}

#define MOD2(x) ((x)-2*jfloor(0.5*(x)))

D jtbindd(J jt,D x,D y){B id,ix,iy;D d;
 if(_isnan(x))R x; else if(_isnan(y))R y;
 d=y-x; 
 id=d==jfloor(d);
 ix=x==jfloor(x); 
 iy=y==jfloor(y);
 switch(4*(I )(ix&&0>x)+2*(I )(iy&&0>y)+(I )(id&&0>d)){
  default: ASSERTSYS(0,"bindd");
  case 5: /* 1 0 1 */  /* Impossible */
  case 0: /* 0 0 0 */
  case 2: /* 0 1 0 */  R ix&&iy?ibin(x,y):dbin(x,y);
  case 3: /* 0 1 1 */  R (MOD2(x)?-1:1)*ibin(x,x-y-1);
  case 6: /* 1 1 0 */  R (MOD2(d)?-1:1)*ibin(-1-y,-1-x);
  case 1: /* 0 0 1 */ 
  case 4: /* 1 0 0 */ 
  case 7: /* 1 1 1 */  R 0;
}}   /* P.C. Berry, Sharp APL Reference Manual, 1979, p. 132 */

static Z jtbinzz(J jt,Z x,Z y){B id,ix,iy;D rd,rx,ry;Z d;
 if(!x.im&&!y.im)R zrj0(bindd(x.re,y.re));
 d=zminus(y,x);
 rd=d.re; id=rd==jfloor(rd)&&0==d.im;
 rx=x.re; ix=rx==jfloor(rx)&&0==x.im; 
 ry=y.re; iy=ry==jfloor(ry)&&0==y.im;
 switch(4*(I )(ix&&0>rx)+2*(I )(iy&&0>ry)+(I )(id&&0>rd)){
  default: ZASSERT(0,EVSYSTEM);
  case 5: /* 1 0 1 */  /* Impossible */
  case 0: /* 0 0 0 */
  case 2: /* 0 1 0 */  R zbin(x,y);
  case 3: /* 0 1 1 */  R zrj0((MOD2(rx)?-1:1)*ibin(rx,rx-ry-1));
  case 6: /* 1 1 0 */  R zrj0((MOD2(rd)?-1:1)*ibin(-1-ry,-1-rx));
  case 1: /* 0 0 1 */ 
  case 4: /* 1 0 0 */ 
  case 7: /* 1 1 1 */  R zeroZ;
}}


APFX(binDD, D,D,D, bindd,NAN0;,HDR1JERRNAN)
APFX(binZZ, Z,Z,Z, binzz,NAN0;,HDR1JERRNAN)
