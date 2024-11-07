/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Domino                                                           */

#include "j.h"

static F1(jtnorm){R sqroot(pdt(w,conjug(w)));}

// take inverse of upper-triangular w.  We ASSUME w is inplaceable
// n is the size of the nxn matrix w; ncomp codes for special processing
// if n<=ncomp, this is a small FL matrix & we take the inverse inplace
static A jtrinvip(J jt,A w,I n,I ncomp){PROLOG(0066);A ai,bx,di,z;I m;
 ARGCHK1(w);
 if(n<=ncomp){
  NAN0;
  // Handle 2x2 and smaller FL quickly and inplace to avoid recursion and memory-allocation overhead
  // result is 1/w00 w01/(w00*w11)
  //             0     1/w11     for 1x1, only the top-left
  D w00r, w11r;
  w00r=DAV2(w)[0]=1.0/DAV2(w)[0];  // 1/w00
  if(n>1){
   w11r=DAV2(w)[3]=1.0/DAV2(w)[3];  // 1/w11
   DAV2(w)[1]*=-w00r*w11r;  //  w01/(w00*w11)
   // w10 is already 0
  }
  NAN1;
  R w;
  // Don't bother marking so small a matrix as uppertri
 }
 // fall through for other types & shapes
 if(1>=n)R recip(w);  // if an atom, inverse = reciprocal.  Must be CMPX/RAT/QP
 m=n>>1; I tom=(0x01222100>>((n&7)<<2))&3; m=(m+tom<n)?m+tom:m;  // Minimize number of wasted multiply slots, processing in batches of 4
 // construe w as a block-matrix Wij where w00 and w11 are upper-triangular, w10 is 0, and w01 is a full matrix
 RZ(ai=jtrinvip(jt,take(v2(m,m),w),m,ncomp))  // take inverse of w00  kludge could use faux block to avoid take overhead esp for 2x2 FL results
 RZ(di=jtrinvip(jt,drop(v2(m,m),w),n-m,ncomp))  // take inverse of w11
 RZ(bx=negateW(pdt(ai,pdt(take(v2(m,m-n),w),di))));  // -w00^_1 mp w01 mp w11^_1
 if(ISSPARSE(AT(w))){z=over(stitch(ai,bx),take(v2(n-m,-n),di));  // should copy this over w, inplace
 }else{
  // copy in the pieces, line by line, writing over the input area
  I leftlen = m<<bplg(AT(w)); I rightlen=(n-m)<<bplg(AT(w));
  z=w; void *zr=voidAV(z);  // reuse input area, set pointer to output
  // copy top part: ai,.bx
  void *leftr=voidAV(ai), *rightr=voidAV(bx);  // input pointers
  DQ(m, MC(zr,leftr,leftlen); zr=(C*)zr+leftlen; leftr=(C*)leftr+leftlen; MC(zr,rightr,rightlen); zr=(C*)zr+rightlen; rightr=(C*)rightr+rightlen;)
  // copy bottom part: 0,.di
  rightr=voidAV(di);
  DQ(n-m, mvc(leftlen,zr,MEMSET00LEN,MEMSET00); zr=(C*)zr+leftlen; MC(zr,rightr,rightlen); zr=(C*)zr+rightlen; rightr=(C*)rightr+rightlen;)
  AFLAGORLOCAL(z,AFUPPERTRI) // Mark result as upper-triangular in case we multiply a matrix by it
 }
 //  w00^_1     -w00^_1 mp w01 mp w11^_1
 //    0         w11^_1
 EPILOG(z);
}    /* R.K.W. Hui, Uses of { and }, APL87, p. 56 */


// 128!:1 Invert Upper-triangular matrix R
DF1(jtrinv){
 ARGCHK1(w);
 F1RANK(2,jtrinv,self);
 ASSERT(AR(w)==2,EVRANK);  // rank at least 2
 ASSERT(AS(w)[0]==AS(w)[1],EVLENGTH);  // error if not square
 if(!AN(w))R w;  // if empty, return empty
 RZ(w=cvt(ISDENSETYPE(AT(w),B01+INT)?FL:AT(w),w));  // always copy, but convert sparse B01/INT to FL
 R jtrinvip(jt,w,AS(w)[0],ISDENSETYPE(AT(w),FL)?2:0);  // take the inverse.  Since it runs in place, clone w.  For float, reduce overhead at bottom of recursion
}

// recursive subroutine for qr decomposition, returns q;r
static F1(jtqrr){PROLOG(0067);A a1,q,q0,q1,r,r0,r1,t,*tv,t0,t1,y,z;I m,n,p,*s;
 ARGCHK1(w);
 if(2>AR(w)){p=AN(w); n=1;}else{s=AS(w); p=s[0]; n=s[1];}  // p=#rows, n=#columns
 m=n>>1; I tom=(0x01222100>>((n&7)<<2))&3; m=(m+tom<n)?m+tom:m;  // Minimize number of wasted multiply slots, processing in batches of 4
 if(1>=n){  // just 1 col
  t=norm(ravel(w));  // norm of col 
  ASSERT(!AN(w)||!equ(t,num(0)),EVDOMAIN);  // norm must not be 0 unless column is empty
  RZ(q=tymes(w,recip(t)));
  R jlink(2>AR(q)?table(q):q,reshape(v2(n,n),p?t:num(1)));
 }
 // construe w as w0 w1 w0t w1t
 RZ(t0=qrr(take(v2(p,m),w)));  // find QR of w0 pxm   w0t
 tv=AAV(t0); q0=C(tv[0]); r0=C(tv[1]);  // point to Q and R of w0  pxm mxm  w0t    
 RZ(a1=drop(v2(0L,m),w));  // a1=w1  pxn-m  w1t
 RZ(y=pdt(conjug(cant1(q0)),a1));  // q0* w1 mxpxn-m     w1t q0t*   q0t*=/q0      result is mxn-m
 RZ(t1=qrr(minus(a1,pdt(q0,y))));  // pxmxn-m  get QR of w1-(q0 q0* w1)    w1t-(w1t q0t* q0t)    
 tv=AAV(t1); q1=C(tv[0]); r1=C(tv[1]);  
 RZ(q=stitch(q0,q1));  // overall q is q0t    Q of (w1t-(w1t q0t* q0t))
 RZ(r=over(stitch(r0,y),take(v2(n-m,-n),r1)));
 // r is   r0    q0* w1
 //        0     R of w1-(q0 q0* w1)
 // qr is  q0 r0    (q0 q0* w1) + (Q of w1-(q0 q0* w1))(R of w1-(q0 q0* w1))
 // = w0 w1 = w
 z=jlink(q,r); EPILOG(z);
}

#define verifyinplace(to,from) if(to!=from){MC(CAV(to),CAV(from),AN(to)<<bplg(AT(to)));}
// this version operates on rows, inplace.  w is not empty
// q is the ADJOINT of the original q matrix
// result is adjoint of the L in LQ decomp, therefore upper-triangular
static F1(jtltqip){PROLOG(0067);A l0,l1,y,z;
#if C_AVX2 || EMU_AVX2
 D ipa[8], *ipv;
#endif
ARGCHK1(w);
 A q0; fauxblock(virtwq0); D *w0v=DAV(w);  // q0 & q1 data
 I rw=AS(w)[0]; I cl=AS(w)[1];  // # rows, # columns
  // handle case of 2 rows
 if(rw<=2) {
#if C_AVX2 || EMU_AVX2
  if(rw==2 && AT(w)&FL){
   // We calculate the 2-row case rather than recurring to handle the rows individually, because we can keep the multipliers busy
   // Let Pij be the inner product of row i and row j.  Then
   // q0 = w0 / sqrt(P00)
   // q1 =  (w1 - w1.q0 q0) / sqrt ((w1 - w1.q0 q0).(w1 - w1.q0 q0))
   //   = (w1 - (P01/P00)w0) / sqrt((w1 - (P01/P00)w0).(w1 - (P01/P00)w0))
   //   = (w1 - (P01/P00)w0) / sqrt(P11 - 2*(P01^2)/P00) + (P01^2)/P00)
   //   = (w1 - (P01/P00)w0) / sqrt(P11 - (P01^2)/P00)
   // Letting S0=1/sqrt(00) and S1=1/sqrt(P11 - (P01^2)/P00) = sqrt(P00)/sqrt(det)
   // q0 = w0 * S0
   // q1 = w0 * -(P01/P00)S1  +  w1 * S1
   // We could calculate the Ps quicker by noticing the symmetry, i. e. the fact that the operands are the same
   RZ(jtsumattymesprods(jt,FL|LIT,w0v,w0v,cl,1,2,1,2,ipa));  // calculate Pij, 2 at a time
   // The L* result is
   // sqrt(P00)   P01/sqrt(P00)
   // 0           sqrt(P11 - (P01^2)/P00)
   GAT0(y,FL,4,2); AS(y)[0]=AS(y)[1]=2; D *yv=DAV2(y);
   D P00=ipa[0]; D P01=ipa[1]; D P11=ipa[3]; D R00=sqrt(P00); D det=P00*P11 - P01*P01;
   ipa[0]=(1/R00); ipa[1]=0; ipa[2]=-P01*(1/R00)*(1/sqrt(det)); ipa[3]=R00*(1/sqrt(det));  // as above
   yv[0]=R00; yv[1]=P01*(1/R00); yv[2]=0; yv[3]=sqrt(det)*(1/R00);  // as above
   ASSERT(1==cachedmmult(jt,ipa,w0v,w0v,2,cl,2,0),EVNAN); // w1 - (w1 q0*) q0, in place
   RETF(y);
  }
#endif
  // If not FL length 2, handle any length 1
  if(rw<=1){  // just 1 row
   // Use a faux-virtual block to take the norm of w, so that we leave w inplaceable for when we normalize it in place
   fauxvirtual(q0,virtwq0,w,1,ACUC1); AS(q0)[0]=cl; AN(q0)=cl;  // kludge use sumattymesprod to create a table result directly
   A t; RZ(t=norm(q0));  // norm of row
   ASSERT(!equ(t,num(0)),EVDOMAIN);  // norm must not be 0
   A z; RZ(z=tymesA(w,recip(t))); verifyinplace(w,z);
   RZ(t=table(t)); realizeifvirtual(t); R t; // this is real, so it is also the adjoint of L
  }
 }
 // continue for blocks that must be subdivided
 I m=rw>>1; I tom=(0x01222100>>((rw&7)<<2))&3; m=(m+tom<rw)?m+tom:m;  // Minimize number of wasted multiply slots, processing in batches of 4
 // construe w as w0 w1
 fauxvirtual(q0,virtwq0,w,2,ACUC1|ACINPLACE); AS(q0)[0]=m; AS(q0)[1]=cl; AN(q0)=m*cl;
 RZ(l0=jtltqip(jt,q0));  // form q0 in place, return l0
 A q1; fauxblock(virtwq1);  fauxvirtual(q1,virtwq1,w,2,ACUC1|ACINPLACE); AK(q1)+=(m*cl)<<bplg(AT(w)); AS(q1)[0]=rw-m; AS(q1)[1]=cl; AN(q1)=(rw-m)*cl; 
 // calculate w1 - (w1 q0*) q0
#if C_AVX2 || EMU_AVX2
 if(AT(w)&FL && (m<50 || m*m*cl<(64*64*64))){
  // floating-point w that isn't larger than L2 cache.  (1) use inner-products to calculate w1 q0* (2) use blockedmmult to calculate final product
  if((m*(rw-m))>(int)(sizeof(ipa)/sizeof(ipa[0]))){
   GATV0(y,FL,m*(rw-m),2); ipv=DAV2(y);
  }else{  // avoid allocating short y
   ipv=ipa; y=q0; AN(y)=m*(rw-m); AK(y)=(C*)ipa-(C*)y;
  }
  AS(y)[0]=rw-m; AS(y)[1]=m;  // now AS and AN are set in y.  q0 has been repurposed.
  D *w1v=DAV(q1);  // addr of w1, same over calls
  RZ(jtsumattymesprods(jt,FL|LIT,w1v,w0v,cl,1,rw-m,1,m,ipv));  // w1 q0*, into ipv is rw-m x cl x m
  ASSERT(1==cachedmmult(jt,ipv,w0v,w1v,rw-m,cl,m,FLGWMINUSZ),EVNAN); // w1 - (w1 q0*) q0, in place
 }else{
#else
 {
#endif
  // general case for all types
  RZ(y=pdt(q1,conjug(cant1(q0))));  // w1 q0*   n-mxpxm
  RZ(z=minusA(q1,pdt(y,q0))); verifyinplace(q1,z);   // w1 - (w1 q0*) q0   n-mxmxp
 }
 RZ(l1=jtltqip(jt,q1));  //  get QR of   w1 - (w1 q0*) q0  
 // copy in the pieces, line by line
 I leftlen = m<<bplg(AT(w)); I rightlen=(rw-m)<<bplg(AT(w));
 GA(z,AT(w),rw*rw,2,AS(w)); AS(z)[1]=rw; void *zr=voidAV2(z);  // allocate result, set pointer to output
 // copy top part: l0*,. (w1 q0*)*
 void *leftr=voidAV(l0), *rightr=voidAV(conjug(cant1(y)));  // input pointers
 DQ(m, MC(zr,leftr,leftlen); zr=(C*)zr+leftlen; leftr=(C*)leftr+leftlen; MC(zr,rightr,rightlen); zr=(C*)zr+rightlen; rightr=(C*)rightr+rightlen;)
 // copy bottom part: 0,.(L of w1 - (w1 q0*) q0)*
 rightr=voidAV(l1);
 DQ(rw-m, mvc(leftlen,zr,MEMSET00LEN,MEMSET00); zr=(C*)zr+leftlen; MC(zr,rightr,rightlen); zr=(C*)zr+rightlen; rightr=(C*)rightr+rightlen;)
 // q is    q0     (Q of w1 - (w1 q0*) q0)
 // l* is   l0*    (w1 q0*)*
 //         0      (L of w1 - (w1 q0*) q0)*
 // lq is  l0 q0
 //        (w1 q0*) q0 + (L of w1 - (w1 q0*) q0)(Q of w1 - (w1 q0*) q0)
 // = w
 AFLAGORLOCAL(z,AFUPPERTRI) // Mark result as upper-triangular in case we multiply a matrix by it
 EPILOG(z);
}

// qr (?) decomposition of w, returns q;r
DF1(jtqr){A r,z;D c=inf,d=0,x;I n1,n,*s,wr;
 F1RANK(2,jtqr,self);
 ASSERT(!ISSPARSE(AT(w)),EVNONCE);
 ASSERT(AT(w)&B01+INT+FL+CMPX+QP,EVDOMAIN);
 wr=AR(w); s=AS(w);
 ASSERT(2>wr||s[0]>=s[1],EVLENGTH);
 RZ(z=qrr(w)); r=C(AAV(z)[1]); n=AS(r)[0]; n1=1+n;
 if(FL&AT(r)){D*v=DAV(r);  DQ(n, x= ABS(*v); if(x<c)c=x; if(x>d)d=x; v+=n1;);}
 else        {Z*v=ZAV(r);  DQ(n, x=zmag(*v); if(x<c)c=x; if(x>d)d=x; v+=n1;);}
 ASSERT(!n||c>d*FUZZ,EVDOMAIN);
RETF(z);
}

// return inverse of w, calculated by lq applied to adjoint
// result has rank 2
static A jtlq(J jt,A w,D *det){A l;D c=inf,d=0,x;I n1,n,*s,wr;
 F1RANK(2,jtqr,DUMMYSELF);
 ASSERT(!ISSPARSE(AT(w)),EVNONCE);
 ASSERT(AT(w)&B01+INT+FL+CMPX+QP,EVDOMAIN);
 wr=AR(w); s=AS(w);
 ASSERT(2>wr||s[0]>=s[1],EVLENGTH);
 if(ISDENSETYPE(AT(w),B01+INT))RZ(w=cvt(FL,w));  // convert boolean/integer to real
 if(wr==1)w=table(w);  // convert column vector to column matrix
 w=conjug(cant1(w));  // create w*, where the result will be built inplace
 RZ(l=jtltqip(jt,w)); n=AS(l)[0]; n1=1+n;
 // build determinant for integer correction, if that is enabled (i. e. nonzero)
 if(FL&AT(l)){D*v=DAV(l); D determ=*det; DQ(n, x= ABS(*v); if(determ!=0){determ*=x; if(determ>1e20)determ=0.0;} if(x<c)c=x; if(x>d)d=x; v+=n1;); *det=determ;} 
 else        {Z*v=ZAV(l);  DQ(n, x=zmag(*v); if(x<c)c=x; if(x>d)d=x; v+=n1;);}
 ASSERT(!n||c>d*FUZZ,EVDOMAIN);
 RETF(pdt(jtrinvip(jt,l,n,AT(w)&FL?2:0),w));  // engage fast reciprocal for float matrices
}

// Boolean/integer correction.  If the inversand was B01 or INT, we can eliminate some rounding error by forcing the
// determinant to integer and then each value to an integer multiple of the determinant.
// The determinant was calculated when we inverted the matrix
static A jticor(J jt,A  w,D d){D *v;
 ARGCHK1(w);
 if(d==0.0)R w;  // if not enabled or not applicable, return input unchanged
 d=jround(ABS(d));  // force determinant to integer
 D recipd=1/d;
 v=DAV(w); DO(AN(w), v[i]=jround(d*v[i])*recipd;);  // force each value to multiple of recip of determinant, then divide
 R w;
}

static A jtminvdet(J jt,A w,D *det){PROLOG(0068);A q,y,z;I m,n,*s,t,wr;
 F1RANK(2,jtminv,DUMMYSELF);
 t=AT(w); wr=AR(w); s=AS(w); m=wr?s[0]:1; n=1<wr?s[1]:1;
 if(!wr){*det=0.0; R recip(w);}
 if(!AN(w)){ASSERT(1==wr||m>=n,EVLENGTH); *det=0.0; R cant1(w);}
 if(AN(w)&&t&RAT+XNUM){
  ASSERT(m>=n,EVLENGTH);
  if(t&XNUM)RZ(w=cvt(RAT,w));
  if(1<wr&&m==n)y=w; else{q=cant1(w); y=pdt(q,w);}
  z=drop(v2(0L,n),gausselm(stitch(y,reshape(v2(n,n),take(sc(1+n),xco1(scf(1.0)))))));
  if(2>wr)z=tymes(reshape(mtv,z),w); else if(m>n)z=pdt(z,q);
  *det=0.0;  // disable integer correction, in case a % w with a integer
 }else{
  // not RAT/XNUM.  Calculate inverse as R^-1 Q^-1 after taking QR decomp & using Q^-1=Q*
  *det=(t&B01+INT&&2==wr&&m==n)?1.0:0.0;  // if taking inverse of square int, allow setting up for correction afterward
  z=jtlq(jt,w,det);
  z=icor(z,*det);  // if integer correction called for, do it
  z=2==wr?z:reshape(shape(w),z);
 }
 EPILOG(z);
}
F1(jtminv){D detv; R jtminvdet(jt,w,&detv);}

static B jttridiag(J jt,I n,A a,A x){D*av,d,p,*xv;I i,j,n1=n-1;
 av=DAV(a); xv=DAV(x); d=xv[0];
 for(i=j=0;i<n1;++i){
  ASSERT(d!=0,EVDOMAIN);  
  p=xv[j+2]/d;  
  d=xv[j+3]-=p*xv[j+1]; 
  av[i+1]-=p*av[i]; 
  j+=3;
 }
 ASSERT(d!=0,EVDOMAIN); 
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
 v=AV(y); m=AS(y)[0];
 ASSERT(m==3*n-2,EVNONCE);
 DQ(m, d=*v++; d-=*v++; ASSERT(-1<=d&&d<=1,EVNONCE););
 at=AT(a); xt=AT(x); RE(t=maxtype(at,xt)); RE(t=maxtype(t,FL));
 RZ(a=cvt(t,a)); RZ(x=cvt(t,x));
 if(t&CMPX)RZ(ztridiag(n,a,x)) else RZ(tridiag(n,a,x));
 R a;
}    /* currently only handles tridiagonal sparse w */


// a %. w  for all types
DF2(jtmdiv){PROLOG(0069);A z;I t;
 F2RANK(RMAX,2,jtmdiv,self);
 if(ISSPARSE(AT(a)))RZ(a=denseit(a));
 t=AT(w);
 if(ISSPARSE(t))R mdivsp(a,w);
 D detv; // place to build determinant of inverse
 z=jtminvdet(jt,w,&detv);  // take generalized inverse of w, setting up for icor if needed
 z=pdt(2>AR(w)?reshape(shape(w),z):z,a);  // w^-1 mp a
 if(AT(a)&B01+INT)z=icor(z,detv);  // integer correct if a is not float (& correction is possible)
 EPILOG(z);
}

