/* Copyright 1990-2004, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Domino                                                           */

#include "j.h"


static F1(jtnorm){R sqroot(pdt(w,conjug(w)));}

// take inverse of upper-triangular w
F1(jtrinv){PROLOG(0066);A ai,bx,di,z;I m,n,r,*s;
 F1RANK(2,jtrinv,0);
 r=AR(w); s=AS(w); n=2>r?1:s[1]; // n is rank of matrix
// obsolete m=(1+n)>>1; //    m is the matrix splitpoint
 m=n>>1; I tom=(0x01222100>>((n&7)<<2))&3; m=(m+tom<n)?m+tom:m;  // Minimize number of wasted multiply slots, processing in batches of 4
 // construe w as a block-matrix Wij where w00 and w11 are upper-triangular, w10 is 0, and w01 is a full matrix
 ASSERT(!r||n==s[0],EVLENGTH);  // error if not square
 if(1>=n)R recip(w);  // if an atom, inverse = reciprocal
 ai=rinv(take(v2(m,m),w));  // take inverse of w00
 di=rinv(drop(v2(m,m),w));  // take inverse of w11
 bx=negateW(pdt(ai,pdt(take(v2(m,m-n),w),di)));  // -w00^_1 mp w01 mp w11^_1   could have fast multiplier for diagonal mtces
 if(AT(w)&SPARSE){z=over(stitch(ai,bx),take(v2(n-m,-n),di));  // should copy this over w, inplace
 }else{
  // copy in the pieces, line by line
  I leftlen = m<<bplg(AT(w)); I rightlen=(n-m)<<bplg(AT(w));
  GA(z,AT(w),n*n,2,AS(w)); void *zr=voidAV(z);  // allocate result, set pointer to output
  // copy top part: ai,.bx
  void *leftr=voidAV(ai), *rightr=voidAV(bx);  // input pointers
  DQ(m, MC(zr,leftr,leftlen); zr=(C*)zr+leftlen; leftr=(C*)leftr+leftlen; MC(zr,rightr,rightlen); zr=(C*)zr+rightlen; rightr=(C*)rightr+rightlen;)
  // copy bottom part: 0,.di
  rightr=voidAV(di);
  DQ(n-m, memset(zr,C0,leftlen); zr=(C*)zr+leftlen; MC(zr,rightr,rightlen); zr=(C*)zr+rightlen; rightr=(C*)rightr+rightlen;)
 }
 //  w00^_1     -w00^_1 mp w01 mp w11^_1
 //    0         w11^_1
 EPILOG(z);
}    /* R.K.W. Hui, Uses of { and }, APL87, p. 56 */

// recursive subroutine for qr decomposition, returns q;r
static F1(jtqrr){PROLOG(0067);A a1,q,q0,q1,r,r0,r1,t,*tv,t0,t1,y,z;I m,n,p,*s;
 RZ(w);
// obsolete  if(2>AR(w)){p=AN(w); n=m=1;}else{s=AS(w); p=s[0]; n=s[1]; m=(1+n)>>1;}  // p=#rows, n=#columns
 if(2>AR(w)){p=AN(w); n=1;}else{s=AS(w); p=s[0]; n=s[1];}  // p=#rows, n=#columns
 m=n>>1; I tom=(0x01222100>>((n&7)<<2))&3; m=(m+tom<n)?m+tom:m;  // Minimize number of wasted multiply slots, processing in batches of 4
 if(1>=n){  // just 1 col
  t=norm(ravel(w));  // norm of col 
  ASSERT(!AN(w)||!equ(t,num[0]),EVDOMAIN);  // norm must not be 0 unless column is empty
// obsolete  RZ(q=divide(w,t));
  RZ(q=tymes(w,recip(t)));
  R link(2>AR(q)?table(q):q,reshape(v2(n,n),p?t:num[1]));
 }
 // construe w as w0 w1 w0t w1t
 RZ(t0=qrr(take(v2(p,m),w)));  // find QR of w0 pxm   w0t
 tv=AAV(t0); q0=*tv++; r0=*tv;  // point to Q and R of w0  pxm mxm  w0t    
 RZ(a1=drop(v2(0L,m),w));  // a1=w1  pxn-m  w1t
 RZ(y=pdt(conjug(cant1(q0)),a1));  // q0* w1 mxpxn-m     w1t q0t*   q0t*=/q0      result is mxn-m
 RZ(t1=qrr(minus(a1,pdt(q0,y))));  // pxmxn-m  get QR of w1-(q0 q0* w1)    w1t-(w1t q0t* q0t)    
 tv=AAV(t1); q1=*tv++; r1=*tv;  
 RZ(q=stitch(q0,q1));  // overall q is q0t    Q of (w1t-(w1t q0t* q0t))
 RZ(r=over(stitch(r0,y),take(v2(n-m,-n),r1)));
 // r is   r0    q0* w1
 //        0     R of w1-(q0 q0* w1)
 // qr is  q0 r0    (q0 q0* w1) + (Q of w1-(q0 q0* w1))(R of w1-(q0 q0* w1))
 // = w0 w1 = w
 z=link(q,r); EPILOG(z);
}

#define verifyinplace(to,from) if(to!=from){MC(CAV(to),CAV(from),AN(to)<<bplg(AT(to)));}
// this version operates on rows, inplace.  w is not empty
// q is the ADJOINT of the original q matrix
static F1(jtltqip){PROLOG(0067);A l0,l1,y,z;
 RZ(w);
 A q0; fauxblock(virtwq0);
 I rw=AS(w)[0]; I cl=AS(w)[1];  // # rows, # columns
 if(rw<=1){  // just 1 row
  // Use a faux-virtual block to take the norm of w, so that we leave w inplaceable for when we normalize it in place
  fauxvirtual(q0,virtwq0,w,1,ACUC1); AS(q0)[0]=cl; AN(q0)=cl;  // kludge use sumattymesprod to create a table result directly
  A t; RZ(t=norm(q0));  // norm of row
  ASSERT(!equ(t,num[0]),EVDOMAIN);  // norm must not be 0
// obsolete  RZ(q=divide(w,t));
  A z; RZ(z=tymesA(w,recip(t))); verifyinplace(w,z);
  R table(t);  // this is real, so it is also the conjugate of L
 }
 I m=rw>>1; I tom=(0x01222100>>((rw&7)<<2))&3; m=(m+tom<rw)?m+tom:m;  // Minimize number of wasted multiply slots, processing in batches of 4
 // construe w as w0 w1
 fauxvirtual(q0,virtwq0,w,2,ACUC1|ACINPLACE); AS(q0)[0]=m; AS(q0)[1]=cl; AN(q0)=m*cl; 
 RZ(l0=jtltqip(jt,q0));  // form q0 in place, return l0
 A q1; fauxblock(virtwq1);  fauxvirtual(q1,virtwq1,w,2,ACUC1|ACINPLACE); AK(q1)+=(m*cl)<<bplg(AT(w)); AS(q1)[0]=rw-m; AS(q1)[1]=cl; AN(q1)=(rw-m)*cl; 
 RZ(y=pdt(q1,conjug(cant1(q0))));  // w1 q0*   n-mxpxm
 RZ(z=minusA(q1,pdt(y,q0))); verifyinplace(q1,z);   // w1 - (w1 q0*) q0   n-mxmxp
 RZ(l1=jtltqip(jt,q1));  //  get QR of   w1 - (w1 q0*) q0  
// obsolete  RZ(z=over(stitch(l0,conjug(cant1(y))),take(v2(rw-m,-rw),l1)));  // create result as L*, which is upper-tri
 // copy in the pieces, line by line
 I leftlen = m<<bplg(AT(w)); I rightlen=(rw-m)<<bplg(AT(w));
 GA(z,AT(w),rw*rw,2,AS(w)); AS(z)[1]=rw; void *zr=voidAV(z);  // allocate result, set pointer to output
 // copy top part: l0*,. (w1 q0*)*
 void *leftr=voidAV(l0), *rightr=voidAV(conjug(cant1(y)));  // input pointers
 DQ(m, MC(zr,leftr,leftlen); zr=(C*)zr+leftlen; leftr=(C*)leftr+leftlen; MC(zr,rightr,rightlen); zr=(C*)zr+rightlen; rightr=(C*)rightr+rightlen;)
 // copy bottom part: 0,.(L of w1 - (w1 q0*) q0)*
 rightr=voidAV(l1);
 DQ(rw-m, memset(zr,C0,leftlen); zr=(C*)zr+leftlen; MC(zr,rightr,rightlen); zr=(C*)zr+rightlen; rightr=(C*)rightr+rightlen;)
// should build result with copying
 // l* is   l0*    (w1 q0*)*
 //         0      (L of w1 - (w1 q0*) q0)*
 // lq is  l0 q0
//         (w1 q0*) q0 + (L of w1 - (w1 q0*) q0)(Q of w1 - (w1 q0*) q0)
 // = w
 EPILOG(z);
}

// qr (?) decomposition of w, returns q;r
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

// return inverse of w, calculated by lq applied to adjoint
// result has rank 2
static F1(jtlq){A l;D c=inf,d=0,x;I n1,n,*s,wr;
 F1RANK(2,jtqr,0);
 ASSERT(DENSE&AT(w),EVNONCE);
 ASSERT(AT(w)&B01+INT+FL+CMPX,EVDOMAIN);
 wr=AR(w); s=AS(w);
 ASSERT(2>wr||s[0]>=s[1],EVLENGTH);
 if(AT(w)&B01+INT)RZ(w=cvt(FL,w));  // convert boolean/integer to real
 if(wr==1)w=table(w);  // convert column vector to column matrix
 w=conjug(cant1(w));  // create w*, where the result will be built inplace
 RZ(l=jtltqip(jt,w)); n=AS(l)[0]; n1=1+n;
 // build determinant for integer correction, if that is enabled (i. e. nonzero)
 if(FL&AT(l)){D*v=DAV(l); D determ=jt->workareas.minv.determ; DQ(n, x= ABS(*v); if(determ!=0){determ*=x; if(determ>1e20)determ=0.0;} if(x<c)c=x; if(x>d)d=x; v+=n1;); jt->workareas.minv.determ=determ;} 
 else        {Z*v=ZAV(l);  DQ(n, x=zmag(*v); if(x<c)c=x; if(x>d)d=x; v+=n1;);}
 ASSERT(!n||c>d*jt->fuzz,EVDOMAIN);
 RETF(pdt(rinv(l),w));  // kludge could have fast multiplier for diagonal matrices
}

// Boolean/integer correction.  If the inversand was B01 or INT, we can eliminate some rounginr error by forcing the
// determinant to integer and then each value to an integer multiple of the determinant.
// The determinant was calculated when we inverted the matrix
static F1(jticor){D d,*v;
 RZ(w);
 d=jt->workareas.minv.determ;  // fetch flag/determinant
 if(d==0.0)R w;  // if not enabled or not applicable, return input unchanged
 d=jfloor(0.5+ABS(d));  // force determinant to integer
 D recipd=1/d;
 v=DAV(w); DO(AN(w), v[i]=jfloor(0.5+d*v[i])*recipd;);  // force each value to multiple of recip of determinant, then divide
 R w;
}

F1(jtminv){PROLOG(0068);A q,y,z;I m,n,*s,t,wr;
 F1RANK(2,jtminv,0);
 jt->workareas.minv.determ=0.0;
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
  // not RAT/XNUM.  Calculate inverse as R^-1 Q^-1 after taking QR decomp & using Q^-1=Q*
  if(t&B01+INT&&2==wr&&m==n)jt->workareas.minv.determ=1.0;  // if taking inverse of square int, allow setting up for correction afterward
#if 0   // obsolete
  RZ(y=qr(w)); v=AAV(y); q=*v++; r=*v;
  z=pdt(rinv(r),t&CMPX?conjug(cant1(q)):cant1(q));
// obsolete   if(t&B01+INT&&2==wr&&m==n)z=icor(z);
#else
  z=jtlq(jt,w);
#endif
  z=icor(z);  // if integer correction called for, do it
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


// a %. w  for all types
F2(jtmdiv){PROLOG(0069);A z;I t;
 F2RANK(RMAX,2,jtmdiv,0);
 if(AT(a)&SPARSE)RZ(a=denseit(a));
 t=AT(w);
 if(t&SPARSE)R mdivsp(a,w);
#if 0 // obsolete
 if(t&XNUM+RAT)z=minv(w);  // for xnums, take inv of a
 else{
  RZ(y=qr(w)); v=AAV(y); q=*v++; r=*v;
  z=pdt(rinv(r),t&CMPX?conjug(cant1(q)):cant1(q));
  b=t&B01+INT&&2==AR(w)&&*AS(w)==*(1+AS(w));
  if(b)z=icor(r,z);
 }
#endif
 z=minv(w);  // take generalized inverse of w, setting up for icor if needed
 z=pdt(2>AR(w)?reshape(shape(w),z):z,a);  // a * w^-1
 if(AT(a)&B01+INT)z=icor(z);  // integer correct if a is not float (& correction is possible)
 EPILOG(z);
}
