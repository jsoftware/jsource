/* Copyright (c) 1990-2026, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Domino                                                           */

#include "j.h"

#ifdef IMPORTCBLASLIB
#undef jcblas_dtrsm
#undef jcblas_ztrsm

#undef jdgecon_
#undef jdgels_
#undef jdgeqrf_
#undef jdgetrf_
#undef jdgetri_
#undef jdgetrs_
#undef jdorgqr_
#undef jdtrcon_

#undef jzgecon_
#undef jzgels_
#undef jzgeqrf_
#undef jzgetrf_
#undef jzgetri_
#undef jzgetrs_
#undef jztrcon_
#undef jzungqr_

#define jcblas_dtrsm cblas_dtrsm
#define jcblas_ztrsm cblas_ztrsm

#define jdgecon_ dgecon_
#define jdgels_ dgels_
#define jdgeqrf_ dgeqrf_
#define jdgetrf_ dgetrf_
#define jdgetri_ dgetri_
#define jdgetrs_ dgetrs_
#define jdorgqr_ dorgqr_
#define jdtrcon_ dtrcon_

#define jzgecon_ zgecon_
#define jzgels_ zgels_
#define jzgeqrf_ zgeqrf_
#define jzgetrf_ zgetrf_
#define jzgetri_ zgetri_
#define jzgetrs_ zgetrs_
#define jztrcon_ ztrcon_
#define jzungqr_ zungqr_
#else
#define jcblas_dtrsm(...) EXTERNCALL((jcblas_dtrsm)(__VA_ARGS__))
#define jcblas_ztrsm(...) EXTERNCALL((jcblas_ztrsm)(__VA_ARGS__))

#define jdgecon_(...) EXTERNCALL((jdgecon_)(__VA_ARGS__))
#define jdgels_(...) EXTERNCALL((jdgels_)(__VA_ARGS__))
#define jdgeqrf_(...) EXTERNCALL((jdgeqrf_)(__VA_ARGS__))
#define jdgetrf_(...) EXTERNCALL((jdgetrf_)(__VA_ARGS__))
#define jdgetri_(...) EXTERNCALL((jdgetri_)(__VA_ARGS__))
#define jdgetrs_(...) EXTERNCALL((jdgetrs_)(__VA_ARGS__))
#define jdorgqr_(...) EXTERNCALL((jdorgqr_)(__VA_ARGS__))
#define jdtrcon_(...) EXTERNCALL((jdtrcon_)(__VA_ARGS__))

#define jzgecon_(...) EXTERNCALL((jzgecon_)(__VA_ARGS__))
#define jzgels_(...) EXTERNCALL((jzgels_)(__VA_ARGS__))
#define jzgeqrf_(...) EXTERNCALL((jzgeqrf_)(__VA_ARGS__))
#define jzgetrf_(...) EXTERNCALL((jzgetrf_)(__VA_ARGS__))
#define jzgetri_(...) EXTERNCALL((jzgetri_)(__VA_ARGS__))
#define jzgetrs_(...) EXTERNCALL((jzgetrs_)(__VA_ARGS__))
#define jztrcon_(...) EXTERNCALL((jztrcon_)(__VA_ARGS__))
#define jzungqr_(...) EXTERNCALL((jzungqr_)(__VA_ARGS__))
#endif

static F1(jtnorm){F12IP;R sqroot(pdt(w,conjug(w)));}

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
 m=n>>1; I tom=SHMSK(0x01222100,(n&7)<<2,3); m=(m+tom<n)?m+tom:m;  // Minimize number of wasted multiply slots, processing in batches of 4
 // construe w as a block-matrix Wij where w00 and w11 are upper-triangular, w10 is 0, and w01 is a full matrix
 RZ(ai=jtrinvip(jt,take(v2(m,m),w),m,ncomp))  // take inverse of w00  kludge could use faux block to avoid take overhead esp for 2x2 FL results
 RZ(di=jtrinvip(jt,drop(v2(m,m),w),n-m,ncomp))  // take inverse of w11
 RZ(bx=negateW(pdt(ai,pdt(take(v2(m,m-n),w),di))));  // -w00^_1 mp w01 mp w11^_1
 if(ISSPARSE(AT(w)) || AT(w)&RAT+XNUM){z=over(stitch(ai,bx),take(v2(n-m,-n),di));  // should copy this over w, inplace
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
DF1(jtrinv){DF12IP;
 ARGCHK1(w);
 F1RANK(2,jtrinv,self);
 ASSERT(AR(w)==2,EVRANK);  // rank at least 2
 ASSERT(AS(w)[0]==AS(w)[1],EVLENGTH);  // error if not square
 if(!AN(w))R w;  // if empty, return empty
 RZ(w=cvt(ISDENSETYPE(AT(w),B01+INT2+INT4+INT)?FL:AT(w),w));  // always copy, convert dense boolean/integer to FL
 R jtrinvip(jt,w,AS(w)[0],ISDENSETYPE(AT(w),FL)?2:0);  // take the inverse.  Since it runs in place, clone w.  For float, reduce overhead at bottom of recursion
}

// recursive subroutine for qr decomposition, returns q;r
static F1(jtqrr){F12IP;PROLOG(0067);A a1,q,q0,q1,r,r0,r1,t,*tv,t0,t1,y,z;I m,n,p,*s;
 ARGCHK1(w);
 if(2>AR(w)){p=AN(w); n=1;}else{s=AS(w); p=s[0]; n=s[1];}  // p=#rows, n=#columns
 m=n>>1; I tom=SHMSK(0x01222100,(n&7)<<2,3); m=(m+tom<n)?m+tom:m;  // Minimize number of wasted multiply slots, processing in batches of 4
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
static F1(jtltqip){F12IP;PROLOG(0067);A l0,l1,y,z;
#if C_AVX2 || EMU_AVX2
 D ipa[8], *ipv;
#endif
 ARGCHK1(w);
 fauxblock(virtwq0); A q0; D *w0v=DAV(w);  // q0 & q1 data
 I rw=AS(w)[0]; I cl=AS(w)[1];  // # rows, # columns
  // handle case of 2 rows
 if(rw<=2) {
// this condition must be the same as that in cip.c
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
   ASSERT(det>0,EVDOMAIN);
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
 I m=rw>>1; I tom=SHMSK(0x01222100,(rw&7)<<2,3); m=(m+tom<rw)?m+tom:m;  // Minimize number of wasted multiply slots, processing in batches of 4
 // construe w as w0 w1
 fauxvirtual(q0,virtwq0,w,2,ACUC1|ACINPLACE); AS(q0)[0]=m; AS(q0)[1]=cl; AN(q0)=m*cl;
 RZ(l0=jtltqip(jt,q0));  // form q0 in place, return l0
 fauxblock(virtwq1); A q1; fauxvirtual(q1,virtwq1,w,2,ACUC1|ACINPLACE); AK(q1)+=(m*cl)<<bplg(AT(w)); AS(q1)[0]=rw-m; AS(q1)[1]=cl; AN(q1)=(rw-m)*cl;
 // calculate w1 - (w1 q0*) q0
// this condition must be the same as that in cip.c
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

// 128!:0 for a dense FL/CMPX matrix w (m>=n>=1) by Householder QR: xGEQRF, then xORGQR/xUNGQR to form Q.
// Each column of Q and row of R is rescaled by a unit factor so the diagonal of R is real and >=0,
// Returns q;r like jtqrr.
static A jtqrlapack(J jt,A w){PROLOG(0067);A q,r,tau,sg,wk,z;
 I t=AT(w)&CMPX?CMPX:FL, im=AS(w)[0], in=AS(w)[1]; int m=(int)im, n=(int)in, info=0, lwork=-1;
 if(AT(w)!=t)RZ(w=cvt(t,w));
 RZ(q=cant1(w));
 GA10(tau,t,in); GA10(sg,t,in); GA00(r,t,in*in,2); AS(r)[0]=AS(r)[1]=in;
 if(t==FL){D wsz,wsz2; D *qv=DAV(q), *rv=DAV(r), *sv=DAV(sg);
  jdgeqrf_(&m,&n,qv,&m,DAV(tau),&wsz,&lwork,&info); jdorgqr_(&m,&n,&n,qv,&m,DAV(tau),&wsz2,&lwork,&info); // workspace queries
  lwork=(int)MAX(wsz,wsz2); GATV0(wk,FL,lwork,1); // allocate workspace
  jdgeqrf_(&m,&n,qv,&m,DAV(tau),DAV(wk),&lwork,&info); // Householder QR
  ASSERT(!info,EVDOMAIN);
  // Fixing R's diagonal to be positive
  for(I i=0;i<in;++i){ // for each row i of R
   D s=qv[i+i*im]<0?-1.0:1.0; // s is sign of the diagonal element R(i,i)
   sv[i]=s; // remember it, to fix Q's column i later
   for(I j=0;j<in;++j) rv[i*in+j]=j<i?0.0:s*qv[i+j*im]; // for each column j of that row  below the diagonal: 0, on/above: R(i,j) times the sign
  }
  jdorgqr_(&m,&n,&n,qv,&m,DAV(tau),DAV(wk),&lwork,&info); // form Q explicitly
  ASSERT(!info,EVDOMAIN);
  // match sign on each column of Q
  for(I j=0;j<in;++j)
   if(sv[j]<0){
    D *c=qv+j*im;
    for(I i=0;i<im;++i)c[i]=-c[i];
   }
 }else{dcomplex wsz,wsz2; Z *qv=ZAV(q), *rv=ZAV(r), *sv=ZAV(sg);
  jzgeqrf_(&m,&n,(dcomplex*)qv,&m,(dcomplex*)ZAV(tau),&wsz,&lwork,&info); jzungqr_(&m,&n,&n,(dcomplex*)qv,&m,(dcomplex*)ZAV(tau),&wsz2,&lwork,&info); // workspace queries
  lwork=(int)MAX(wsz.real,wsz2.real); GATV0(wk,CMPX,lwork,1); // allocate workspace
  jzgeqrf_(&m,&n,(dcomplex*)qv,&m,(dcomplex*)ZAV(tau),(dcomplex*)ZAV(wk),&lwork,&info); // Householder QR
  ASSERT(!info,EVDOMAIN);
  for(I i=0;i<in;++i){ // for each row i of R
   Z d=qv[i+i*im]; // d is R(i,i)
   D a=zmag(d); // a = |d|, its magnitude
   Z s=a>0?(Z){d.re/a,d.im/a}:(Z){1.0,0.0}; sv[i]=s; // s = d/|d|, the phase of d; 1 if d is 0
   for(I j=0;j<in;++j){ // for each column j of row i
    Z e=qv[i+j*im];
    rv[i*in+j]=j<i?zeroZ:j==i?(Z){a,0.0}:(Z){s.re*e.re+s.im*e.im, s.re*e.im-s.im*e.re}; // below the diagonal: 0, above: conj(s)*e, diagonal: |d| set exactly (conj(s)*d could round to a tiny imaginary part)
   }
  }
  jzungqr_(&m,&n,&n,(dcomplex*)qv,&m,(dcomplex*)ZAV(tau),(dcomplex*)ZAV(wk),&lwork,&info); // form Q explicitly
  ASSERT(!info,EVDOMAIN);
  // multiply each column j of Q by the phase s that was divided out of row j of R, so that R's diagonal is real and positive.
  for(I j=0;j<in;++j){
   Z s=sv[j];
   Z *c=qv+j*im;
   for(I i=0;i<im;++i){
    Z e=c[i];
    c[i]=(Z){e.re*s.re-e.im*s.im, e.re*s.im+e.im*s.re};
   }
  }
 }
 RZ(q=cant1(q));
 z=jlink(q,r);
 EPILOG(z);
}

// 1 if every element of the FL or CMPX array w is finite: x*0 is 0 for finite x but NaN for an infinity or NaN.
static B allfinite(A w){D *v=DAV(w); I n=AN(w)<<((AT(w)&CMPX)!=0);
 DQ(n, if(unlikely(*v++*0.0!=0.))R 0;);
 R 1;
}

// qr decomposition of w, returns q;r
DF1(jtqr){F12IP;A r,z;D c=inf,d=0,x;I n1,n,*s,wr;
 F1RANK(2,jtqr,self);
 ASSERT(!ISSPARSE(AT(w)),EVNONCE);
 ASSERT(AT(w)&B01+INT2+INT4+INT+FL+CMPX+QP,EVDOMAIN);
 wr=AR(w); s=AS(w);
 ASSERT(2>wr||s[0]>=s[1],EVLENGTH);
 ASSERT(!(AT(w)&FL+CMPX) || allfinite(w), EVDOMAIN);
 if(hascblas && wr==2 && AN(w) && AT(w)&(B01+INT2+INT4+INT+FL+CMPX) && s[0]*s[1]<=INT_MAX
  && (AT(w)&CMPX ? jzgeqrf_&&jzungqr_ : jdgeqrf_&&jdorgqr_)){
  RZ(z=jtqrlapack(jt,w));
 } else{
  if(AT(w)&INT2+INT4)RZ(w=cvt(FL,w));
  z=qrr(w);
  if(!z&&jt->jerr==EVNAN){RESETERR; ASSERT(0,EVDOMAIN);} // NaN error -> domain error
  RZ(z);
 }
 r=C(AAV(z)[1]); n=AS(r)[0]; n1=1+n;
 if(FL&AT(r)){D*v=DAV(r);  DQ(n, x= ABS(*v); if(x<c)c=x; if(x>d)d=x; v+=n1;);}
 else        {Z*v=ZAV(r);  DQ(n, x=zmag(*v); if(x<c)c=x; if(x>d)d=x; v+=n1;);}
 ASSERT(!n||c>d*FUZZ,EVDOMAIN);
 RETF(z);
}

// Computes determinant from LU factors (or of a triangular w, with ipiv=0) for integer correction.
// Returns 0.0 if the determinant magnitude exceeds 1e20 or is NaN.
static D ludet(A w,int *ipiv){D d=1.0;
 I n=AR(w)==2?AS(w)[0]:0, swaps=0;
 DQ(n,{
  d*=DAV(w)[i+i*n];
  if(!(fabs(d)<=1e20)) return 0.0;
 });
 // Each row interchange flips the determinant's sign.
 if(ipiv)DQ(n, if(ipiv[i]!=i+1)++swaps;);
 if(swaps&1)d=-d;
 return d;
}

// Boolean/integer correction. We can eliminate some rounding error by forcing the
// determinant to integer and then each value to an integer multiple of the determinant.
// The determinant was calculated when we inverted the matrix
static A jticor(J jt,A w,D d){D *v;
 ARGCHK1(w);
 if(d==0.0)R w;  // if not enabled or not applicable, return input unchanged
 d=jround(ABS(d));  // force determinant to integer
 D recipd=1/d;
 v=DAV(w);
 DQ(AN(w), D p=d*v[i]; if(ABS(p-jround(p))>0.25)R w;); // not close to multiples of 1/d: correction would be a guess
 DQ(AN(w), v[i]=jround(d*v[i])*recipd;);  // force each value to multiple of recip of determinant, then divide
 R w;
}

// anorm = ||w||, the largest absolute row sum of w (the 1-norm of the transpose LAPACK sees)
// It must be computed before xGETRF overwrites w, because condition = ||w||*||w^-1|| needs it to reject nearly singular w
// (xGECON estimates ||w^-1||, or jtlq measures the computed inverse).
static D luanorm(A w,I n){D r=0;
 for(I i=0;i<n;++i){D s=0;
  if(AT(w)&FL){
   D* v=DAV(w)+i*n;
   for(I j=0;j<n;++j)s+=ABS(v[j]);
  }else{
   Z* v=ZAV(w)+i*n;
   for(I j=0;j<n;++j)s+=sqrt(v[j].re*v[j].re+v[j].im*v[j].im); // fast, but the squares can overflow or underflow
   if(!(s>1e-150&&s<1e150)){s=0; for(I j=0;j<n;++j)s+=zmag(v[j]);} // so redo an extreme (or NaN) row with zmag, which scales
  }
  if(s!=s)R s; // NaN
  if(s>r)r=s;
 }
 R r;
}

// jtlucondok, luinvcondok and jttrcondok return 1 = OK, 0 = numerically singular (or inf/NaN), -1 = allocation failed.

// Used by jtgesv, which never forms the inverse (jtlq computes the exact condition number from the inverse instead).
// After xGETRF has factored square w, decide whether w is invertible using xGECON.
static I jtlucondok(J jt,A w,int n,D anorm){int info=0; D rcond=0; C norm='1'; A wk; D *wv;
 if(!(anorm<inf))R 0;  // inf or NaN in w
 if(AT(w)&FL){
  GATV0E(wk,FL,5*(I)n,1,R -1); wv=DAV(wk); // allocate workspace: work 4n D, iwork n int
  jdgecon_(&norm,&n,DAV(w),&n,&anorm,&rcond,wv,(int*)(wv+4*(I)n),&info);
 }else{
  GATV0E(wk,FL,6*(I)n,1,R -1); wv=DAV(wk); // allocate workspace: work 2n Z, rwork 2n D
  jzgecon_(&norm,&n,(dcomplex*)ZAV(w),&n,&anorm,&rcond,(dcomplex*)wv,wv+4*(I)n,&info);
 }
 R info==0 && rcond>FUZZ; // false if rcond is NaN
}

// Used by jtgesv instead of jtlucondok when n<=GESVEXACT. After xGETRF has factored square w, compute the inverse of the
// matrix LAPACK sees one column at a time from the factors, and measure its 1-norm exactly. For small n this is cheaper than xGECON.
#define GESVEXACT 12
static I luinvcondok(I t,A w,int n,int *ipiv,D anorm){D inorm=0;
 if(!(anorm<inf))R 0; // inf or NaN in w
 if(t&FL){D *a=DAV(w), x[GESVEXACT];
  for(I j=0;j<n;++j){
   DO(n, x[i]=0.0;); x[j]=1.0; // start with e_j
   DO(n, I p=ipiv[i]-1; if(p!=i){D s=x[i]; x[i]=x[p]; x[p]=s;}); // apply the row interchanges
   for(I k=0;k<n;++k){D xk=x[k]; // forward substitution with L (solve L y = x)
    for(I i=k+1;i<n;++i)x[i]-=a[i+k*n]*xk;
   }
   for(I k=n-1;k>=0;--k){D xk=x[k]/=a[k+k*n]; // back substitution with U (solve U z = y)
    for(I i=0;i<k;++i)x[i]-=a[i+k*n]*xk;
   }
   D s=0; DO(n, s+=ABS(x[i]);); // take the column sum and keep the largest
   if(!(s<inf))R 0; if(s>inorm)inorm=s; // inf or NaN: singular
  }
 }else{Z *a=ZAV(w), x[GESVEXACT];
  for(I j=0;j<n;++j){
   DO(n, x[i].re=x[i].im=0.0;); x[j].re=1.0;
   DO(n, I p=ipiv[i]-1; if(p!=i){Z s=x[i]; x[i]=x[p]; x[p]=s;});
   for(I k=0;k<n;++k){Z xk=x[k];
    for(I i=k+1;i<n;++i){Z l=a[i+k*n];
     x[i].re-=l.re*xk.re-l.im*xk.im; x[i].im-=l.re*xk.im+l.im*xk.re;
    }
   }
   for(I k=n-1;k>=0;--k){
    // x[k]/u = x[k]*conj(u)/|u|^2.  Scale u by r first so |u|^2 cannot underflow or overflow (u is not 0: xGETRF info was 0).
    Z u=a[k+k*n]; D r=MAX(ABS(u.re),ABS(u.im)), ur=u.re/r, ui=u.im/r, m=(ur*ur+ui*ui)*r;
    Z xk={(x[k].re*ur+x[k].im*ui)/m, (x[k].im*ur-x[k].re*ui)/m}; x[k]=xk;
    for(I i=0;i<k;++i){Z l=a[i+k*n];
     x[i].re-=l.re*xk.re-l.im*xk.im; x[i].im-=l.re*xk.im+l.im*xk.re;
    }
   }
   D s=0; DO(n, s+=zmag(x[i]););
   if(!(s<inf))R 0; if(s>inorm)inorm=s;
  }
 }
 R 1.0/(anorm*inorm)>FUZZ;
}

// Singularity test for triangular solve, least squares, minimum norm, pseudo-inverse.
// Those paths factor w = Q*R (or L*Q) with Q orthonormal, so w and its factor R have the same
// condition number. xTRCON estimates rcond ~ 1/(||R||*||R^-1||);
// t     FL or CMPX
// uplo  'U' if the factor is upper triangular (R of a QR), 'L' if lower (L of an LQ)
// fac   pointer to the triangle: the R (or L); lda is the column stride of that storage
static I jttrcondok(J jt,I t,C uplo,int n,void *fac,int lda){int info=0; D rcond=0; C norm='1', diag='N'; A wk; D *wv;
 if(t&FL){
  GATV0E(wk,FL,4*(I)n,1,R -1); wv=DAV(wk); // allocate workspace: work 3n D, iwork n int
  jdtrcon_(&norm,&uplo,&diag,&n,(D*)fac,&lda,&rcond,wv,(int*)(wv+3*(I)n),&info);
 }else{
  GATV0E(wk,FL,5*(I)n,1,R -1); wv=DAV(wk); // allocate workspace: work 2n Z, rwork n D
  jztrcon_(&norm,&uplo,&diag,&n,(dcomplex*)fac,&lda,&rcond,(dcomplex*)wv,wv+4*(I)n,&info);
 }
 R info==0 && rcond>FUZZ; // false if rcond is NaN
}

// return inverse of w
// If jtfg allows w to be inplaced, the LAPACK path for square w overwrites it and returns it as the result.
static A jtlq(J jtfg,A w,D *det){F12IP;A l;D c=inf,d=0,x;I n1,n,*ws,wr;
 ASSERT(!ISSPARSE(AT(w)),EVNONCE);
 ASSERT(AT(w)&B01+INT2+INT4+INT+FL+CMPX+QP,EVDOMAIN);
 wr=AR(w); ws=AS(w);
 ASSERT(2>wr||ws[0]>=ws[1],EVLENGTH); // square or tall only: jtminvdet turns wide w into its tall conjugate transpose first
 // calling LAPACK to compute LU factorization and then inverse of square matrix
 if(hascblas && AT(w)&(B01+INT2+INT4+INT+FL+CMPX)
  && wr==2 && ws[0]==ws[1] && ws[0]>1 && ws[0]*ws[0]<=INT_MAX
  && (AT(w)&CMPX ? jzgetrf_&&jzgetri_ : jdgetrf_&&jdgetri_)){
  // Work on w itself only if it is already FL/CMPX and the caller lets us inplace it; otherwise copy.
  if(!(AT(w)&FL+CMPX) || (AC(w)&SGNIF(jtfg,JTINPLACEWX))>=0) RZ(w=cvt(AT(w)&CMPX?CMPX:FL,w));
  int info, ni=(int)AS(w)[0], lwork=-1; A ipa,wk; GATV0(ipa,INT4,ni,1); int *ipiv=I4AV1(ipa);
  D anorm=luanorm(w,ni); // needed for the condition test below; take it before xGETRF overwrites w
  if(AT(w)&FL) jdgetrf_(&ni,&ni,DAV(w),&ni,ipiv,&info);
  else jzgetrf_(&ni,&ni,(dcomplex*)ZAV(w),&ni,ipiv,&info);
  ASSERT(!info,EVDOMAIN); // exactly singular
  if(AT(w)&FL){D worksize;
   if(*det!=0.0){*det=ludet(w,ipiv);} // boolean/integer w: save determinant for integer correction of the result
   jdgetri_(&ni,DAV(w),&ni,ipiv,&worksize,&lwork,&info); // workspace query
   ASSERT(!info,EVDOMAIN);
   lwork=(int)worksize; GATV0(wk,FL,lwork,1);
   jdgetri_(&ni,DAV(w),&ni,ipiv,DAV(wk),&lwork,&info); // matrix inversion
  }else{dcomplex worksize;
   jzgetri_(&ni,(dcomplex*)ZAV(w),&ni,ipiv,&worksize,&lwork,&info); // workspace query
   ASSERT(!info,EVDOMAIN);
   lwork=(int)worksize.real; GATV0(wk,CMPX,lwork,1);
   jzgetri_(&ni,(dcomplex*)ZAV(w),&ni,ipiv,(dcomplex*)ZAV(wk),&lwork,&info); // matrix inversion
  }
  ASSERT(!info, EVDOMAIN);
  // w is now the inverse, so the reciprocal condition number is exactly 1/(|w|*|w^_1|) in the 1-norm.
  ASSERT(anorm*luanorm(w,ni)<1.0/FUZZ,EVDOMAIN);
  RETF(w);
 }
 // Tall matrix: pseudo-inverse from Householder QR
 if(hascblas && AT(w)&(B01+INT2+INT4+INT+FL+CMPX) && wr==2 && ws[0]>ws[1] && ws[0]*ws[1]<=INT_MAX
  && (AT(w)&CMPX ? jzgeqrf_&&jztrcon_&&jzungqr_&&jcblas_ztrsm : jdgeqrf_&&jdtrcon_&&jdorgqr_&&jcblas_dtrsm)){
  I t=AT(w)&CMPX?CMPX:FL, im=ws[0], in=ws[1]; int m=(int)im, n=(int)in, info=0, lwork, lw2; A q,r,tau,wk; I ok;
  if(AT(w)!=t)RZ(w=cvt(t,w));
  RZ(q=cant1(w));
  GA10(tau,t,in); GA00(r,t,in*in,2); AS(r)[0]=AS(r)[1]=in;
  // 1. QR of w.  2. copy R out (4. overwrites it).  3. rank test on R.  4. form Q in place.  5. solve q R^T = conj(Q).
  if(t==FL){D wsz; D *qv=DAV(q), *rv=DAV(r);
   lwork=-1;
   jdgeqrf_(&m,&n,qv,&m,DAV(tau),&wsz,&lwork,&info);
   lw2=(int)wsz; lwork=-1;
   jdorgqr_(&m,&n,&n,qv,&m,DAV(tau),&wsz,&lwork,&info);
   lwork=MAX(lw2,(int)wsz); // one workspace big enough for both
   GATV0(wk,FL,lwork,1);
   jdgeqrf_(&m,&n,qv,&m,DAV(tau),DAV(wk),&lwork,&info); // 1.
   ASSERT(!info,EVDOMAIN);
   for(I j=0;j<in;++j) // 2.
    for(I i=0;i<in;++i)
     rv[i+j*in]=i<=j?qv[i+j*im]:0.0;
   ok=jttrcondok(jt,t,'U',n,rv,n); // 3.
   if(ok<0) R 0; // allocation failed
   ASSERT(ok,EVDOMAIN);
   jdorgqr_(&m,&n,&n,qv,&m,DAV(tau),DAV(wk),&lwork,&info); // 4.
   ASSERT(!info,EVDOMAIN);
   jcblas_dtrsm(CblasColMajor,CblasRight,CblasUpper,CblasTrans,CblasNonUnit,m,n,1.0,rv,n,qv,m); // 5.
  }else{dcomplex wsz; Z *qv=ZAV(q), *rv=ZAV(r); Z one={1.0,0.0};
   lwork=-1;
   jzgeqrf_(&m,&n,(dcomplex*)qv,&m,(dcomplex*)ZAV(tau),&wsz,&lwork,&info);
   lw2=(int)wsz.real; lwork=-1;
   jzungqr_(&m,&n,&n,(dcomplex*)qv,&m,(dcomplex*)ZAV(tau),&wsz,&lwork,&info);
   lwork=MAX(lw2,(int)wsz.real);
   GATV0(wk,CMPX,lwork,1);
   jzgeqrf_(&m,&n,(dcomplex*)qv,&m,(dcomplex*)ZAV(tau),(dcomplex*)ZAV(wk),&lwork,&info); // 1.
   ASSERT(!info,EVDOMAIN);
   for(I j=0;j<in;++j) // 2.
    for(I i=0;i<in;++i)
     rv[i+j*in]=i<=j?qv[i+j*im]:zeroZ;
   ok=jttrcondok(jt,t,'U',n,rv,n); // 3.
   if(ok<0) R 0;
   ASSERT(ok,EVDOMAIN);
   jzungqr_(&m,&n,&n,(dcomplex*)qv,&m,(dcomplex*)ZAV(tau),(dcomplex*)ZAV(wk),&lwork,&info); // 4.
   ASSERT(!info,EVDOMAIN);
   DQ(im*in, qv[i].im=-qv[i].im;); // conj(Q)
   jcblas_ztrsm(CblasColMajor,CblasRight,CblasUpper,CblasTrans,CblasNonUnit,m,n,&one,rv,n,qv,m); // 5.
  }
  RETF(q);
 }
 // when LAPACK is not applicable.
 if(AT(w)&B01+INT2+INT4+INT){
  RZ(w=cvt(FL,w));
 }else if(AT(w)&FL+CMPX){
  ASSERT(allfinite(w),EVDOMAIN);
 }
 if(wr==1)RZ(w=table(w)); // convert column vector to column matrix
 RZ(w=conjug(cant1(w))); // create w*, where the result will be built inplace
 l=jtltqip(jt,w);
 if(!l&&jt->jerr==EVNAN){RESETERR; ASSERT(0,EVDOMAIN);} // NaN error -> domain error
 RZ(l); n=AS(l)[0]; n1=1+n;
 // build determinant for integer correction, if that is enabled (i. e. nonzero)
 if(FL&AT(l)){D*v=DAV(l); D determ=*det; DQ(n, x= ABS(*v); if(determ!=0){determ*=x; if(determ>1e20)determ=0.0;} if(x<c)c=x; if(x>d)d=x; v+=n1;); *det=determ;}
 else        {Z*v=ZAV(l);  DQ(n, x=zmag(*v); if(x<c)c=x; if(x>d)d=x; v+=n1;);}
 ASSERT(!n||c>d*FUZZ,EVDOMAIN);
 A z=pdt(jtrinvip(jt,l,n,AT(w)&FL?2:0),w);  // engage fast reciprocal for float matrices
 if(!z&&jt->jerr==EVNAN){RESETERR; ASSERT(0,EVDOMAIN);}
 RETF(z);
}

static A jtminvdet(J jtfg,A w,D *det){F12IP;PROLOG(0068);A q,y,z;I m,n,*s,t,wr;
 F1RANK(2,jtminv,DUMMYSELF);
 t=AT(w); wr=AR(w); s=AS(w); m=wr?s[0]:1; n=1<wr?s[1]:1;
 if(!wr){ASSERT(t&NUMERIC,EVDOMAIN); *det=0.0; R recip(w);}
 if(!AN(w)){*det=0.0; R cant1(w);}  // empty: the pseudo-inverse is the empty transpose, also for wide w
 if(wr==2 && m<n){  // For a wide w (m<n), %. w takes the pseudo-inverse of its transpose, which is tall, and transposes the result back.
  RZ(z=jtminvdet(jt,cant1(w),det));
  RZ(z=cant1(z));
  EPILOG(z);
 }
 if(AN(w)&&t&RAT+XNUM){
  if(t&XNUM)RZ(w=cvt(RAT,w));
  if(1<wr&&m==n)y=w; else{q=cant1(w); y=pdt(q,w);}
  z=drop(v2(0L,n),gausselm(stitch(y,reshape(v2(n,n),take(sc(1+n),xco1(scf(1.0)))))));
  if(2>wr)z=tymes(reshape(mtv,z),w); else if(m>n)z=pdt(z,q);
  *det=0.0;  // disable integer correction, in case a % w with a integer
 }else{
  // not RAT/XNUM.  Calculate inverse as R^-1 Q^-1 after taking QR decomp & using Q^-1=Q*
  *det=(t&B01+INT2+INT4+INT&&2==wr&&m==n)?1.0:0.0; // if taking inverse of square int, allow setting up for correction afterward
  RZ(z=jtlq(jtfg,w,det));
  RZ(z=icor(z,*det)); // if integer correction called for, do it
  z=2==wr?z:reshape(shape(w),z);
 }
 EPILOG(z);
}

F1(jtminv){F12IP;D detv; R jtminvdet(jtfg,w,&detv);}  // pass jtfg so an inplaceable w can be overwritten by LAPACK

// Is the nxn FL/CMPX w triangular? Returns the uplo LAPACK must be given (it sees the transpose of our row-major w):
// 'L' if w is upper triangular, 'U' if it is lower triangular, 0 if neither. A diagonal w counts as upper.
// Each scan stops at the first nonzero, so a general matrix costs O(1).
static C trikind(A w,I n){B up=1, lo=1;
 if(AT(w)&FL){D *v=DAV(w);
  for(I i=1;i<n&&up;++i)
   for(I j=0;j<i;++j)
    if(v[i*n+j]!=0){ // below the diagonal
     up=0;break;
    }
  for(I i=0;i<n-1&&lo;++i)
   for(I j=i+1;j<n;++j)
    if(v[i*n+j]!=0){ // above the diagonal
     lo=0;break;
    }
 }else{Z *v=ZAV(w);
  for(I i=1;i<n&&up;++i)
   for(I j=0;j<i;++j)
    if(v[i*n+j].re!=0||v[i*n+j].im!=0){
     up=0;break;
    }
  for(I i=0;i<n-1&&lo;++i)
   for(I j=i+1;j<n;++j)
    if(v[i*n+j].re!=0||v[i*n+j].im!=0){
     lo=0;break;
    }
 }
 R up?'L':(lo?'U':0); // LAPACK sees the transpose: our upper is its lower, our lower is its upper
}
// Solve (w+/ .*z)-:a using LAPACK for dense non-empty a and w where w is square matrix.  a is not an atom (jtmdiv extends an atom to a vector).
static A jtgesv(J jtfg,A a,A w){F12IP;A z;
 int n=(int)AS(w)[0],nrhs,info,*ipiv=0; C trans='T', uplo=0; A ipa; I ok;
 I at=AT(a), wt=AT(w), t=((at|wt)&CMPX)?CMPX:FL;
 // Check inplaceability.
 I ipw=AC(w)&SGNIF(jtfg,JTINPLACEWX);
 if(a==w)ipw&=SGNIF(jtfg,JTINPLACEAX);
 if(wt!=t) RZ(w=cvt(t,w)); // Promote w to the common FL/CMPX type
 if(t==FL?jcblas_dtrsm&&jdtrcon_:jcblas_ztrsm&&jztrcon_) uplo=trikind(w,n); // Triangular w?
 if(!uplo && wt==t && ipw>=0) RZ(w=cvt(t,w)); // If not triangular and can't inplace then copy.
 // z is a fresh array LAPACK may overwrite, even when a==w.
 if(AR(a)<=1){
  RZ(z=reshape(v2(1,n),cvt(t,a)));
 }else{
  if(at!=t)RZ(a=cvt(t,a));
  RZ(z=cant1(reshape(v2(n,AN(a)/n),a)));
 }
 nrhs=(int)AS(z)[0];
 if(uplo){ // Triangular
  ok=jttrcondok(jt,t,uplo,n,voidAV(w),n);
  if(ok<0)R 0;
  ASSERT(ok,EVDOMAIN);
  I ul=uplo=='L'?CblasLower:CblasUpper; // column-major
  if(t==FL){
   jcblas_dtrsm(CblasColMajor,CblasLeft,ul,CblasTrans,CblasNonUnit,n,nrhs,1.0,DAV(w),n,DAV(z),n);
  }else {Z one={1.0,0.0};
   jcblas_ztrsm(CblasColMajor,CblasLeft,ul,CblasTrans,CblasNonUnit,n,nrhs,&one,ZAV(w),n,ZAV(z),n);
  }
 }else{ // General
  GATV0(ipa,INT4,n,1); ipiv=I4AV1(ipa);
  // Factor, check conditioning, solve
  D anorm=luanorm(w,n); // take the norm before xGETRF overwrites w
  if(t==FL)jdgetrf_(&n,&n,DAV(w),&n,ipiv,&info); else jzgetrf_(&n,&n,(dcomplex*)ZAV(w),&n,ipiv,&info);
  ASSERT(!info,EVDOMAIN); // exactly singular
  ok=n<=GESVEXACT?luinvcondok(t,w,n,ipiv,anorm):jtlucondok(jt,w,n,anorm); // small n: exact test, without xGECON's overhead
  if(ok<0) R 0; // allocation failure
  ASSERT(ok,EVDOMAIN); // numerically singular
  if(t==FL) jdgetrs_(&trans,&n,&nrhs,DAV(w),&n,ipiv,DAV(z),&n,&info);
  else      jzgetrs_(&trans,&n,&nrhs,(dcomplex*)ZAV(w),&n,ipiv,(dcomplex*)ZAV(z),&n,&info);
  ASSERT(!info,EVDOMAIN);
 }
 if(at&(B01+INT2+INT4+INT) && wt&(B01+INT2+INT4+INT)) RZ(z=icor(z,ludet(w,ipiv)));
 RZ(z=cant1(z)); // transpose z
 RZ(z=reshape(shape(a),z)); // result has the shape of a: (}.$w),}.$a with #w = #a
 R z;
}

// Solve (w+/ .*z)-:a for non-square w using LAPACK.
//  - overdetermined (tall w or w vector): the least-squares solution
//  - underdetermined (wide w): the minimum-norm solution
// w is matrix mxn or vector (treated as mx1 matrix)
// (#a) = #w  (a is not an atom)
// Result has shape (}.$w),}.$a
// Domain error if w is rank-deficient (matrix rank of w < min(m,n) where (m,n) is shape of w).
static A jtgels(J jtfg,A a,A w){F12IP;A b,z,zs;
 int m,n,r,c,nrhs,lda,ldb,lwork=-1,info; A wk;
 I t=((AT(a)|AT(w))&CMPX)?CMPX:FL; C trans=t==FL?'T':'C';
 I ar=AR(a), wr=AR(w), im=AS(w)[0], in=wr==2?AS(w)[1]:1, inrhs=AN(a)/im, ildb=MAX(im,in);
 c=m=(int)im; lda=r=n=(int)in; nrhs=(int)inrhs; ldb=(int)ildb;
 B tall=m>n;
 I ipw=AC(w)&SGNIF(jtfg,JTINPLACEWX);
 if(a==w)ipw&=SGNIF(jtfg,JTINPLACEAX);
 if(tall && n>1){ // when tall, always copy
  RZ(w=cant1(AT(w)!=t?cvt(t,w):w));
 }else if(AT(w)!=t || ipw>=0){ // when wide, copy only if we can't inplace.
  RZ(w=cvt(t,w));
 }
 // Create b that holds a on input and the solution on output: LAPACK's column-major ldb x nrhs (nrhs rows of length ldb=max(m,n)).
 // Row j = column j of a (m entries) on input, column j of the solution (first n entries) on output
 // ldb>m only when w is wide, the extra entries are output space.
 if(ar==1 && ldb==m){ // a is vector, w is tall (or square)
  RZ(b=reshape(v2(1,im),cvt(t,a)));
 }else{ // a has several right-hand sides or w is wide so the columns need padding.
  if(AT(a)!=t) RZ(a=cvt(t,a));
  // b is array of nrhs rows of length ldb=max(m,n): row j holds column j of a (zero-padded) on input and column j of the solution on output.
  GA00(b,t,inrhs*ildb,2); AS(b)[0]=inrhs; AS(b)[1]=ildb;
  if(t==FL){D *bv=DAV(b), *av=DAV(a);
   for(I j=0;j<inrhs;++j){D *rhs=bv+j*ildb;
    for(I i=0;i<im;++i)rhs[i]=av[i*inrhs+j]; // transpose
    for(I i=im;i<ildb;++i)rhs[i]=0.0; // padding
   }
  }else{Z *bv=ZAV(b), *av=ZAV(a);
   for(I j=0;j<inrhs;++j){Z *rhs=bv+j*ildb;
    for(I i=0;i<im;++i)rhs[i]=av[i*inrhs+j]; // transpose
    for(I i=im;i<ildb;++i)rhs[i]=zeroZ; // padding
   }
  }
 }
 // LAPACK's view of w: rxc with leading dimension lda.
 // Tall: LAPACK sees w itself, trans='N'.
 // Wide: LAPACK sees the nxm transpose of our row-major w, so solve with trans='T'.
 if(tall){trans='N'; r=m; c=n; lda=m;}
 else if(t==CMPX){Z*v=ZAV(w);
  DQ(AN(w), v->im=-v->im; ++v;); // zgels accepts 'C' not 'T', so for CMPX we first conjugate w. LAPACK then sees conj(w)^T = w^H.
 }
 if(t==FL){D wsz;
  jdgels_(&trans,&r,&c,&nrhs,DAV(w),&lda,DAV(b),&ldb,&wsz,&lwork,&info); // workspace query
  ASSERT(!info,EVDOMAIN);
  lwork=(int)wsz; GATV0(wk,FL,lwork,1);
  jdgels_(&trans,&r,&c,&nrhs,DAV(w),&lda,DAV(b),&ldb,DAV(wk),&lwork,&info); // solve
 }else{dcomplex wsz;
  jzgels_(&trans,&r,&c,&nrhs,(dcomplex*)ZAV(w),&lda,(dcomplex*)ZAV(b),&ldb,&wsz,&lwork,&info); // workspace query
  ASSERT(!info,EVDOMAIN);
  lwork=(int)wsz.real; GATV0(wk,CMPX,lwork,1);
  jzgels_(&trans,&r,&c,&nrhs,(dcomplex*)ZAV(w),&lda,(dcomplex*)ZAV(b),&ldb,(dcomplex*)ZAV(wk),&lwork,&info); // solve
 }
 ASSERT(!info,EVDOMAIN);
 // Rank test on the triangular factor left in w. Its condition number is that of w.
 I ok=tall?jttrcondok(jt,t,'U',n,voidAV(w),m):jttrcondok(jt,t,'U',m,voidAV(w),n);
 if(ok<0)R 0;
 ASSERT(ok,EVDOMAIN);
 RZ(z=take(v2(inrhs,in),b)); // (inrhs,in) {. b
 if(inrhs>1)RZ(z=cant1(z)); // Transpose back to n x nrhs (not needed for one column).
 // reshape to (}.$w),}.$a
 GATV0(zs,INT,(wr==2)+ar-1,1); I *zsv=IAV1(zs);
 if(wr==2)*zsv++=in; // }.$w
 DO(ar-1, zsv[i]=AS(a)[i+1];) // }.$a
 R reshape(zs,z);
}

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

static F2(jtmdivsp){F12IP;A a1,x,y;I at,d,m,n,t,*v,xt;P*wp;
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
DF2(jtmdiv){F12IP;PROLOG(0069);A z;I at=AT(a), wt=AT(w);
 F2RANK(RMAX,2,jtmdiv,self);
 if(ISSPARSE(at))RZ(a=denseit(a));
 if(ISSPARSE(wt))R mdivsp(a,w);
 // An atom a is extended to a vector of length #w, on every path, so the result's shape is always ((}.$w),}.$a) -: }.$w
 if(!AR(a)&&AR(w))RZ(a=reshape(sc(AS(w)[0]),a));
 // Try LAPACK
 I wm=AR(w)?AS(w)[0]:1, wn=AR(w)==2?AS(w)[1]:1, wmx=MAX(wm,wn); // rows, cols of w
 if(hascblas
  && at&(B01+INT2+INT4+INT+FL+CMPX) && wt&(B01+INT2+INT4+INT+FL+CMPX)
  && AN(w)>1 && AN(a)>0 // a, w must be non-empty and w must have at least two elements, so in particular w mustn't be an atom.
  && AR(a) && AS(a)[0]==wm // leading axes of a and w must have the same length
  && wm*wn<=INT_MAX && wmx*(AN(a)/wm)<=INT_MAX){ // LAPACK indexes with int: w and the right-hand-side workspace must fit.
  if(AR(w)==2 && AS(w)[0]==AS(w)[1] && (((at|wt)&CMPX) ? jzgetrf_&&jzgetrs_&&jzgecon_ : jdgetrf_&&jdgetrs_&&jdgecon_)){ // square w: solve systems of linear equations
   z=jtgesv(jtfg,a,w);
   EPILOG(z);
  }else if(((at|wt)&CMPX)? jzgels_&&jztrcon_ : jdgels_&&jdtrcon_){ // non-square (or vector) w: least squares if tall, minimum norm if wide.
   z=jtgels(jtfg,a,w);
   EPILOG(z);
  }
 }
 D detv; // place to build determinant of inverse
 if(at&(INT2+INT4))RZ(a=cvt(FL,a)); // we later use +/ .* which cannot mix INT2/INT4 with FL.
 RZ(z=jtminvdet(jt,w,&detv));
 RZ(z=pdt(2>AR(w)?reshape(shape(w),z):z,a)); // w^-1 mp a
 if(at&B01+INT2+INT4+INT) RZ(z=icor(z,detv)); // integer correct if a is not float (& correction is possible)
 EPILOG(z);
}
