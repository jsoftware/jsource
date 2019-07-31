/* Copyright 1990-2004, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Domino                                                           */

#include "j.h"


static F1(jtnorm){R sqroot(pdt(w,conjug(w)));}

F1(jtrinv){PROLOG(0066);A ai,bx,di,z;I m,n,r,*s;
 F1RANK(2,jtrinv,0);
 r=AR(w); s=AS(w); n=2>r?1:s[1]; m=(1+n)>>1;
 ASSERT(!r||n==s[0],EVLENGTH);
 if(1>=n)R recip(w);
 ai=rinv(take(v2(m,m),w));
 di=rinv(drop(v2(m,m),w));
 bx=negate(pdt(ai,pdt(take(v2(m,m-n),w),di)));
 z=over(stitch(ai,bx),take(v2(n-m,-n),di));
 EPILOG(z);
}    /* R.K.W. Hui, Uses of { and }, APL87, p. 56 */

static F1(jtqrr){PROLOG(0067);A a1,q,q0,q1,r,r0,r1,t,*tv,t0,t1,y,z;I m,n,p,*s;
 RZ(w);
 if(2>AR(w)){p=AN(w); n=m=1;}else{s=AS(w); p=s[0]; n=s[1]; m=(1+n)>>1;} 
 if(1>=n){
  t=norm(ravel(w));
  ASSERT(!AN(w)||!equ(t,num[0]),EVDOMAIN);
  RZ(q=divide(w,t));
  R link(2>AR(q)?table(q):q,reshape(v2(n,n),p?t:num[1]));
 }
 RZ(t0=qrr(take(v2(p,m),w)));
 tv=AAV(t0); q0=*tv++; r0=*tv;
 RZ(a1=drop(v2(0L,m),w));
 RZ(y=pdt(conjug(cant1(q0)),a1));
 RZ(t1=qrr(minus(a1,pdt(q0,y))));
 tv=AAV(t1); q1=*tv++; r1=*tv;
 RZ(q=stitch(q0,q1));
 RZ(r=over(stitch(r0,y),take(v2(n-m,-n),r1)));
 z=link(q,r); EPILOG(z);
}

F1(jtqr){A r,z;D c=inf,d=0,x;I n1,n,*s,wr;
 F1RANK(2,jtqr,0);
 ASSERT(DENSE&AT(w),EVNONCE);
 ASSERT(AT(w)&B01+INT+FL+CMPX,EVDOMAIN);
 wr=AR(w); s=AS(w);
 ASSERT(2>wr||s[0]>=s[1],EVLENGTH);
 RZ(z=qrr(w)); r=*(1+AAV(z)); n=*AS(r); n1=1+n;
 if(FL&AT(r)){D*v=DAV(r);  DQ(n, x= ABS(*v); if(x<c)c=x; if(x>d)d=x; v+=n1;);} 
 else        {Z*v=ZAV(r);  DQ(n, x=zmag(*v); if(x<c)c=x; if(x>d)d=x; v+=n1;);}
 ASSERT(!n||c>d*jt->fuzz,EVDOMAIN);
 RETF(z);
}

static F2(jticor){D d,*v;I n;
 RZ(a&&w);
 d=1; n=1+*AS(a);
 v=DAV(a); DQ(n-1, d*=*v; v+=n;); d=jfloor(0.5+ABS(d));
 if(!d||d>1e20)R w;
 v=DAV(w); DO(AN(w), v[i]=jfloor(0.5+d*v[i])/d;);
 R w;
}

F1(jtminv){PROLOG(0068);A q,r,*v,y,z;I m,n,*s,t,wr;
 F1RANK(2,jtminv,0);
 t=AT(w); wr=AR(w); s=AS(w); m=wr?s[0]:1; n=1<wr?s[1]:1;
 if(!wr)R recip(w);
 if(!AN(w)){ASSERT(1==wr||m>=n,EVLENGTH); R cant1(w);}
 if(AN(w)&&t&RAT+XNUM){
  ASSERT(m>=n,EVLENGTH);
  if(t&XNUM)RZ(w=cvt(RAT,w));
  if(1<wr&&m==n)y=w; else{q=cant1(w); y=pdt(q,w);}
  z=drop(v2(0L,n),gausselm(stitch(y,reshape(v2(n,n),take(sc(1+n),xco1(scf(1.0)))))));
  if(2>wr)z=tymes(reshape(mtv,z),w); else if(m>n)z=pdt(z,q);
 }else{
  RZ(y=qr(w)); v=AAV(y); q=*v++; r=*v;
  z=pdt(rinv(r),t&CMPX?conjug(cant1(q)):cant1(q));
  if(t&B01+INT&&2==wr&&m==n)z=icor(r,z);
  z=2==wr?z:reshape(shape(w),z);
 }
 EPILOG(z);
}

static B jttridiag(J jt,I n,A a,A x){D*av,d,p,*xv;I i,j,n1=n-1;
 av=DAV(a); xv=DAV(x); d=xv[0];
 for(i=j=0;i<n1;++i){
  ASSERT(d,EVDOMAIN);  
  p=xv[j+2]/d;  
  d=xv[j+3]-=p*xv[j+1]; 
  av[i+1]-=p*av[i]; 
  j+=3;
 }
 ASSERT(d,EVDOMAIN); 
 i=n-1; j=AN(x)-1; av[i]/=d;
 for(i=n-2;i>=0;--i){j-=3; av[i]=(av[i]-xv[j+1]*av[i+1])/xv[j];}
 R 1;
}

static F2(jtmdivsp){A a1,x,y;I at,d,m,n,t,*v,xt;P*wp;
 ASSERT(2==AR(w),EVRANK);
 v=AS(w); n=v[0]; 
 ASSERT(n>=v[1]&&n==AN(a),EVLENGTH); 
 ASSERT(n==v[1],EVNONCE);
 wp=PAV(w); x=SPA(wp,x); y=SPA(wp,i); a1=SPA(wp,a);
 ASSERT(2==AN(a1),EVNONCE);
 v=AV(y); m=*AS(y);
 ASSERT(m==3*n-2,EVNONCE);
 DQ(m, d=*v++; d-=*v++; ASSERT(-1<=d&&d<=1,EVNONCE););
 at=AT(a); xt=AT(x); RE(t=maxtype(at,xt)); RE(t=maxtype(t,FL));
 RZ(a=cvt(t,a)); RZ(x=cvt(t,x));
 if(t&CMPX)RZ(ztridiag(n,a,x)) else RZ(tridiag(n,a,x));
 R a;
}    /* currently only handles tridiagonal sparse w */


F2(jtmdiv){PROLOG(0069);A q,r,*v,y,z;B b=0;I t;
 F2RANK(RMAX,2,jtmdiv,0);
 if(AT(a)&SPARSE)RZ(a=denseit(a));
 t=AT(w);
 if(t&SPARSE)R mdivsp(a,w);
 if(t&XNUM+RAT)z=minv(w);
 else{
  RZ(y=qr(w)); v=AAV(y); q=*v++; r=*v;
  z=pdt(rinv(r),t&CMPX?conjug(cant1(q)):cant1(q));
  b=t&B01+INT&&2==AR(w)&&*AS(w)==*(1+AS(w));
  if(b)z=icor(r,z);
 }
 z=pdt(2>AR(w)?reshape(shape(w),z):z,a);
 if(b&&AT(a)&B01+INT)z=icor(r,z);
 EPILOG(z);
}
