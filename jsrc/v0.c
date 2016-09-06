/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Polynomial Roots & Polynomial Evaluation                         */

#include "j.h"


#define EPS            (jt->fuzz)

#define dplus(x,y)     (x+y)
#define dtymes(x,y)    (x*y)
#define dnegate(x)     (-x)
#define QNEGATE(x)     (qminus(zeroQ,x))

#define CFR(f,T,TYPE,fplus,ftymes,fnegate)  \
 F2(f){PROLOG(0060);A z;I j,n;T d,*t,*u,*v;            \
  n=AN(w); u=(T*)AV(w);                          \
  GATVS(z,TYPE,1+n,1,0,TYPE##SIZE); v=(T*)AV(z); *v=*(T*)AV(a);  \
  for(j=0;j<n;++j){                              \
   d=fnegate(u[j]); t=j+v; *(1+t)=*t;            \
   DO(j, *t=fplus(*(t-1),ftymes(d,*t)); --t;);   \
   *v=ftymes(d,*v);                              \
  }                                              \
  RE(z); EPILOG(z);                              \
 }

static CFR(jtcfrd,D,FL,  dplus,dtymes,dnegate)
static CFR(jtcfrx,X,XNUM,xplus,xtymes, negate)
static CFR(jtcfrq,Q,RAT, qplus,qtymes,QNEGATE)

static F1(jtrsort){A t,z;D d=jt->ct;
 RZ(w);
 jt->ct=jt->fuzz;
 t=over(mag(w),cant1(rect(w)));
 z=dgrade2(w,cant1(irs2(irs2(t,t,0L,1L,1L,jtindexof),t,0L,1L,1L,jtfrom)));
 jt->ct=d;
 R z;
}

static F2(jtcfrz){A z;B b=0,p;I j,n;Z c,d,*t,*u,*v;
 RZ(w=rsort(w)); 
 n=AN(w); u=ZAV(w); 
 GATV(z,CMPX,1+n,1,0); v=ZAV(z); *v=c=*ZAV(a); p=!c.im;
 for(j=0;j<n;++j){
  d=znegate(u[j]); t=j+v; *(1+t)=*t; 
  DO(j, *t=zplus(*(t-1),ztymes(d,*t)); --t;); 
  *v=ztymes(d,*v);
  if(p&&d.im)if(b=!b)c=u[j]; else if(p=ZCJ(c,u[j])){t=v; DO(2+j, t++->im=0.0;);}
 }
 R p>b?cvt(FL,z):z;
}

static F1(jtcfr){A c,r,*wv;I t,wd;
 ASSERT(!AR(w)||2==AN(w),EVLENGTH);
 wv=AAV(w); wd=(I)w*ARELATIVE(w);
 if(AR(w)){c=WVR(0); r=WVR(1);}else{c=one; r=WVR(0);}
 ASSERT(!AR(c)&&1>=AR(r),EVRANK);
 ASSERT(NUMERIC&AT(c)&&(!AN(r)||NUMERIC&AT(r)),EVDOMAIN);
 t=AN(r)?AT(r):B01; if(t&B01+INT)t=XNUM; RE(t=maxtype(t,AT(c)));
 if(TYPESNE(t,AT(c)))RZ(c=cvt(t,c));
 if(TYPESNE(t,AT(r)))RZ(r=cvt(t,r));
 R t&RAT?cfrq(c,r):t&XNUM?cfrx(c,r):t&CMPX?cfrz(c,r):cfrd(c,r);
}    /* coefficients from roots */


static D jtsummag(J jt,A w){A t=aslash(CPLUS,mag(w)); R t?*DAV(t):0.0;}

/* a is a poly of degree m and x is a root estimate             */
/* improve root estimate x by applying Newton iteration n times */
/* x - (a p. x) % (p.. a) p. x                                  */

static Z jtnewt(J jt,I m,Z*a,Z x,I n){I i,j;D e=EPS/1024.0;Z c,p,q,*v;
 c.im=0.0;
 for(i=0;i<n;++i){
  p=q=zeroZ; v=a+m; j=m;
  DO(m, p=zplus(*v,ztymes(x,p)); c.re=(D)j--; q=zplus(ztymes(c,*v),ztymes(x,q)); --v;);
  p=zplus(*a,ztymes(x,p));
  if(e>zmag(p)||e>zmag(q))break;
  x=zminus(x,zdiv(p,q));
 }
 R x;
}    

static B jtdeflateq(J jt,B k,I m,Q*v,Q x){Q q,r,*u;
 u=v+m; q=*u--; DO(m, r=*u--;       q=qplus(r,qtymes(q,x)););
 RE(0); RZ(QEQ(q,zeroQ));
 u=v+m; q=*u--; DO(m, r=*u; *u--=q; q=qplus(r,qtymes(q,x)););
 R 1;
}    /* deflate by x which may or may not be a root. result is 1 iff x is a root. k is ignored. */

static void jtdeflate(J jt,B k,I m,Z*v,Z x){
 if(k){Z q,r; v+=m; q=*v--; DO(m, r=*v; *v--=q; q=zplus(r,ztymes(q,x)););}
 else{D a,b,d,p,q,r;
  a=2*x.re; b=-(x.re*x.re+x.im*x.im);
  v+=m; p=v--->re; q=v--->re;
  DO(m-1, r=v->re; v->re=d=p; v->im=0.0; --v; p=q+d*a; q=r+d*b;);
}}   /* deflate by single root (1=k) or by conjugates (0=k) */

#define CSZ1 11
static Z jtlaguerre(J jt,I m,Z*a,Z x){D ax,e;I i,j;Z b,c,d,dx,g,g2,h,p,q,s,sq,y,zm,zm1;
 static D cyclefracs[8] = {0.5,0.25,0.75,0.13,0.87,0.63,0.37,0.95};
 zm=zrj0((D)m); zm1=zrj0((D)m-1);
 for(i=0;;++i){
  ZASSERT(i<400,EVLIMIT);
  c=d=zeroZ; b=a[m]; e=zmag(b); ax=zmag(x);
  for(j=0;j<m;++j){
   d=zplus(ztymes(x,d),c);         /* 2*d is 2nd derivative */
   c=zplus(ztymes(x,c),b);         /* c   is 1st derivative */
   b=zplus(ztymes(x,b),a[m-j-1]);  /* b   is poly at x      */
   e=zmag(b)+ax*e;
  }
  if(zmag(b)<=EPS*e)R x;
  g=zdiv(c,b);
  g2=ztymes(g,g);
  h=zminus(g2,zdiv(zplus(d,d),b));
  sq=zsqrt(ztymes(zm1,zminus(ztymes(zm,h),g2)));
  p=zplus(g,sq); q=zminus(g,sq); s=zmag(p)>zmag(q)?p:q; 
  y=x;
  dx=ZNZ(s)?zdiv(zm,s):zpow(znegate(zdiv(a[0],a[m])),zrj0(1.0/(D)m));  // Normal step if s!=0; random step if s=0
  x=zminus(x,dx);  // advance to new position
  if(zmag(zminus(x,y))<=EPS*zmag(x))R x;  // if we didn't move much, call it converged.  We hope it's a root.
  // This algorithm is subject to hitting limit cycles (_48 1 0 0 0 1 is an example)
  // To prevent that, every so often we make a partial move
  if(((i-1)%CSZ1)==0)x=zplus(x,ztymes(dx,zrj0(cyclefracs[(i>>3)&8])));
}}   // Press et al., "Numerical Recipes in C" with additions from 2d edition

static Q jtmultiple(J jt,D x,Q m){A y;Q q1,q2,q1r2;
 q1r2.n=xone; q1r2.d=xplus(xone,xone);
 QRE(y=cvt(RAT,scf(x)));
 QRE(q1=qplus(q1r2,qtymes(m,*QAV(y))));
 QRE(q2.n=xdiv(q1.n,q1.d,XMFLR)); q2.d=xone;
 R qdiv(q2,m);
}    /* nearest multiple of m to x */

static Q jtmaxdenom(J jt,I n,Q*v){Q z;X*u,x,y;
 u=1+(X*)v; x=*u;  // u-> &1st denominator, x=&1st denominator
 DO(n-1, u+=2; y=*u; if(-1==xcompare(x,y))x=y;);  // x=&largest denominator
 z.n=x; z.d=xone; R z;  // set denominator as a rational number
}    /* maximum denominator in rational vector v */

/* find all exact rational roots of a rational polynomial w or degree m; return: */
/*  *zz: list of what rational roots are found                       */
/*  *ww: list of complex coefficients of deflated polynomial         */

static B jtrfcq(J jt,I m,A w,A*zz,A*ww){A q,x,y,z;B b;I i,j,wt;Q*qv,rdx,rq,*wv,*zv;Z r,*xv,*yv;
 wt=AT(w);
 ASSERTSYS(wt&B01+INT+FL+XNUM+RAT,"rfcq");
 // Convert w to rational; wv->1st rational coeff
 if(!(wt&RAT))RZ(w=cvt(RAT,w)); wv=QAV(w);
 rdx=maxdenom(1+m,wv);  // rdx = max denominator in the polynomial, in rational form
 RZ(x=cvt(CMPX,w)); xv=ZAV(x); // set x = complex form of w, xv->first complex coeff
 RZ(y=take(sc(1+m),x)); yv=ZAV(y);  // y = complex form with degree m, yv->first coeff
 RZ(q=take(sc(1+m),w)); qv=QAV(q);  // q = rational form with degree m, qv->first coeff
 GATV(z,RAT,m,1,0); zv=QAV(z);        // allocate space for exact rational roots, zv->first result location
 i=j=0;
 // loop to find each root by Laguerre's method
 while(i<m){
  // find one (complex) root.  Exit if error
  r=laguerre(m,xv,laguerre(m-i,yv,zeroZ));
  if(jt->jerr){RESETERR; break;}
  // Get a rational form, with denominator equal to the largest denominator in the polynomial,
  // that is close to the real part of the root
  RE(rq=multiple(r.re,rdx));
  b=0;  // set 'no rational root found'
  // If the value found IS a root, deivide it from the polynomial repeatedly, and move a copy
  // to the result for each repetition
  while(deflateq(1,m-j,qv,rq)){*zv++=rq; ++j; b=1;}
  // Laguerre came up empty.  Try something else
  if(!b){Q q1;
   // Try adjusting the numerator of the Laguerre result by +-1, and see if they work
   q1=rq; q1.n=xone;
   rq=qplus (rq,q1); while(deflateq(1,m-j,qv,rq)){*zv++=rq; ++j; b=1;}
   rq=qminus(rq,q1); while(deflateq(1,m-j,qv,rq)){*zv++=rq; ++j; b=1;}
  }
  // If we found a root, refresh the copies of the complex versions, and account for the roots we have found
  if(b){AN(q)=*AS(q)=1+m-j; rdx=maxdenom(1+m-j,qv); RZ(y=cvt(CMPX,q)); yv=ZAV(y); i=j;}
  else{D c,d;
   // No root found!  Turn over the table and shoot out the lights.  If r is near an axis, push it to the axis.
   c=ABS(r.re); d=ABS(r.im); if(d<EPS*c)r.im=0; if(c<EPS*d)r.re=0;
   // Use Newton's method to find a root (we hope)
   r=newt(m,xv,r,10L); b=!r.im||i==m-1;
   // Divide out the root - or the pair of them, if they are complex.
   deflate(b,m-i,yv,r); i+=2-b;
   // We don't use te roots we find here - we just have to make some progress
   // before the next iteration
 }}
 AN(z)=*AS(z)=j; *zz=z; RZ(*ww=cvt(FL,q));
 R 1;
}    /* roots from coefficients, degree m is 2 or more */

static A jtrfcz(J jt,I m,A w){A x,y,z;B bb=0,real;D c,d;I i;Z r,*xv,*yv,*zv;
 real=!(CMPX&AT(w)); RZ(x=cvt(CMPX,w)); xv=ZAV(x); 
 GATV(y,CMPX,1+m,1,0); yv=ZAV(y); MC(yv,xv,(1+m)*sizeof(Z));
 GATV(z,CMPX,  m,1,0); zv=ZAV(z);
 if(2==m){Z a2,b,c,d,z2={2,0};
  a2=ztymes(z2,xv[2]); b=znegate(xv[1]); c=xv[0]; 
  d=zsqrt(zminus(ztymes(b,b),ztymes(z2,ztymes(a2,c))));
  r=zdiv(zplus (b,d),a2); zv[0]=newt(m,xv,r,10L);
  r=zdiv(zminus(b,d),a2); zv[1]=newt(m,xv,r,10L);
 }else{
  for(i=0;i<m;++i){
   r=laguerre(m,xv,laguerre(m-i,yv,zeroZ));
   if(jt->jerr){RESETERR; bb=1; break;}
   if(real){c=ABS(r.im); d=ABS(r.re); if(c<EPS*d)r.im=0; else if(d<EPS*c)r.re=0;}
   zv[i]=r=newt(m,xv,r,10L); 
   if(real&&r.im&&i<m-1){r.im=-r.im; zv[1+i]=r; deflate(0,m-i,yv,r); ++i;}
   else deflate(1,m-i,yv,r);
  }
  if(bb){A x1;D*u;
   // If we failed on an iteration, perturb the highest coefficient a little bit and see if we can solve that instead.
   if(real){RZ(x1=cvt(FL,vec(CMPX,1+m,xv))); u=  DAV(x1)+m-1;      if(*u)*u*=1+1e-12; else *u=1e-12;}
   else    {RZ(x1=       vec(CMPX,1+m,xv) ); u=&(ZAV(x1)+m-1)->re; if(*u)*u*=1+1e-12; else *u=1e-12;}
   RZ(z=rfcz(m,x1)); zv=ZAV(z);
   DO(m, zv[i]=newt(m,xv,zv[i],10L););
 }}
 if(real){B b=1; DO(m, if(zv[i].im){b=0; break;}); if(b)z=cvt(FL,z);}
 R z;
}    /* roots from coefficients, degree m is 2 or more */

// roots from coefficients.  w is (possibly empty) list of coefficients
static F1(jtrfc){A r,w1;I m=0,n,t;
 n=AN(w); t=AT(w);  // n=#coeffs, t=type
 if(n){
  ASSERT(t&DENSE&&t&NUMERIC,EVDOMAIN);  // coeffs must be dense numeric
  RZ(r=jico2(ne(w,zero),one)); m=*AV(r)%n;  // r=block for index of last nonzero; m=degree of polynomial (but 0 if all zeros)
  ASSERT(m||equ(zero,head(w)),EVDOMAIN);  // error if unsolvable constant polynomial
 }
 // switch based on degree of polynomial
 switch(m){
  case 0:  R link(zero,mtv);  // degree 0 - return 0;''
  case 1:  r=ravel(negate(aslash(CDIV,take(num[2],w)))); break;  // linear - return solution, whatever its type
  default: if(t&CMPX)r=rfcz(m,w);  // higher order - if complex, go straight to complex solutions
           else{RZ(rfcq(m,w,&r,&w1)); if(m>AN(r))r=over(r,rfcz(m-AN(r),w1));} // otherwise, find rational solutions in r, and residual polynomial in w1.
            // if there are residual (complex) solutions, go find them
 }
 // Return result, which is leading nonzero coeff;roots
 R link(from(sc(m),w),rsort(r));
}

// entry point for p. y
F1(jtpoly1){A c,e,x;
 F1RANK(1L,jtpoly1,0L);
 // If y is not boxed, it's a list of coefficients.  Get the roots
 if(!(AN(w)&&BOX&AT(w)))R rfc(w);
 x=AAV0(w);
 // If there is more than one box, or the first box has rank <= 1, it's scale;roots form - go handle it
 if(1<AN(w)||1>=AR(x))R cfr(w);
 // Must be exponent form: a single box containing a table with 2-atom rows
 ASSERT(2==AR(x),EVRANK);
 ASSERT(2==*(1+AS(x)),EVLENGTH);
 RZ(c=irs1(x,0L,1L,jthead));  // c = {."1>y = list of coefficients
 RZ(e=irs1(x,0L,1L,jttail));  // e = {:"1>y = list of exponents
 ASSERT(equ(e,floor1(e))&&all1(le(zero,e)),EVDOMAIN);  // insist on nonnegative integral exponents
 R evc(c,e,"x y}(1+>./y)$0");  // evaluate c 2 : 'x y}(1+>./y)$0' e
}


static A jtmnomx(J jt,I m,A w){A s,*wv,x,z=w,*zv;I i,n,r,wd;
 RZ(w);
 if(BOX&AT(w)){
  n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w); RZ(s=sc(m));
  GATV(z,BOX,n,AR(w),AS(w)); zv=AAV(z);
  for(i=0;i<n;++i){
   x=WVR(i); r=AR(x); 
   ASSERT(1>=r,EVRANK); 
   ASSERT(!r||m==AN(x),EVLENGTH); 
   zv[i]=1<m==1<r?x:1<m?reshape(s,x):head(x);
  }
  RE(z); RZ(z=ope(z));
 }
 ASSERT(NUMERIC&AT(z),EVDOMAIN);
 R z;
}    /* standardize multinomial right arg */

static F2(jtpoly2a){A c,e,x;I m;
 RZ(a&&w);
 m=*(1+AS(a))-1;
 ASSERT(AT(a)&NUMERIC,EVDOMAIN);
 ASSERT(2==AR(a),EVRANK);
 ASSERT(0<m,EVLENGTH);
 RZ(c=      irs1(a,0L,1L,jthead  ) ); 
 RZ(e=cant1(irs1(a,0L,1L,jtbehead)));
 RZ(x=mnomx(m,w));
 R 1==m?pdt(irs2(x,ravel(e),0L,0L,2L,jtexpn2),c):pdt(df2(x,e,dot(slash(ds(CSTAR)),ds(CEXP))),c);
}    /* multinomial: (<c,.e0,.e1,.e2) p. <x0,x1,x2, left argument opened */

F2(jtpoly2){A c,z;B b;D*ad,d,p,*wd,x,*zd;I an,at,j,t,wn,wt;Z*az,e,q,*wz,y,*zz;
 RZ(a&&w);
 if(1<AR(a))R rank2ex(a,w,0L,1L,0L,jtpoly2);
 an=AN(a); at=AT(a); b=1&&BOX&at;
 wn=AN(w); wt=AT(w);
 ASSERT(!an||at&NUMERIC+BOX,EVDOMAIN);
 ASSERT(!wn||wt&NUMERIC+BOX,EVDOMAIN);
 if(!an)R reshape(shape(w),zero);
 if(b){A*av=AAV(a);I ad=(I)a*ARELATIVE(a);
  ASSERT(2>=an,EVLENGTH);
  c=1==an?one:AVR(0); a=AVR(1!=an); 
  if(1==an&&2==AR(a))R poly2a(a,w);
  an=AN(a); at=AT(a);
  ASSERT(NUMERIC&(at|AT(c)),EVDOMAIN);
  ASSERT(!AR(c),EVRANK);
  ASSERT(1>=AR(a),EVRANK); if(!AR(a))RZ(a=ravel(a));
 }
 d=0.0; e=zeroZ;
 RE(t=maxtype(at,wt)); if(b)RE(t=maxtype(t,AT(c))); if(!(t&XNUM+RAT))RE(t=maxtype(t,FL));
 if(b){RZ(c=cvt(t,c)); d=*DAV(c); e=*ZAV(c);}
 if(TYPESNE(t,at))RZ(a=cvt(t,a)); ad=DAV(a); az=ZAV(a);
 if(TYPESNE(t,wt))RZ(w=cvt(t,w)); wd=DAV(w); wz=ZAV(w);
 j=0;
 if(t&FL+CMPX){
        DO(t&FL?an:an+an, x=ad[i]; if(x==inf||x==infm){j=1; break;}); 
  if(!j)DO(t&FL?wn:wn+wn, x=wd[i]; if(x==inf||x==infm){j=1; break;});
 }
 if(!j&&!(t&XNUM+RAT)){GA(z,t,AN(w),AR(w),AS(w)); zd=DAV(z); zz=ZAV(z);}
 switch((b?0:3)+(j||t&XNUM+RAT?0:t&FL?1:2)){
  case 0: R tymes(c,df2(negate(a),w,eval("*/@(+/)")));
  case 1: DO(wn, p=d; x=*wd++; DO(an,p*=x-ad[i];); *zd++=p;);                  break;
  case 2: DO(wn, q=e; y=*wz++; DO(an,q=ztymes(q,zminus(y,az[i]));); *zz++=q;); break;
  case 3: R df2(w,a,eval("(^/i.@#) +/ .* ]"));
  case 4: DO(wn, p=d; x=*wd++; j=an; DO(an,p=ad[--j]+x*p;); *zd++=p;);         break;
  case 5: DO(wn, q=e; y=*wz++; j=an; DO(an,q=zplus(az[--j],ztymes(y,q));); *zz++=q;);
 }
 R z;
}    /* a p. w */


F1(jtpderiv1){
 F1RANK(1,jtpderiv1,0);
 if(AN(w)&&!(NUMERIC&AT(w)))RZ(w=poly1(w));
 R 1>=AN(w) ? apv(1L,0L,0L) : tymes(behead(w),apv(AN(w)-1,1L,1L));
}    /* p.. w */

F2(jtpderiv2){
 F2RANK(0,1,jtpderiv2,0);
 if(!(NUMERIC&AT(w)))RZ(w=poly1(w));
 ASSERT(NUMERIC&AT(a),EVDOMAIN);
 R over(a,divide(w,apv(AN(w),1L,1L)));
}    /* a p.. w */



