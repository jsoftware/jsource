/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Rational Numbers                                                        */

#include "j.h"
#include "ve.h"

#define QSQRT(x)    z->n=rifvsdebug(xsqrt(x->n)); z->d=rifvsdebug(xsqrt(x->d)); {I rc; if(rc=jt->jerr){RESETERR; R rc;}}
#define QFACT(x)    if(ISQinf(*x)&&0<QSGN(*x)) *z=Q_; else {ASSERTWR(ISQINT(*x),EWIRR); z->n=xfact(x->n); z->d=X1;}

#undef FAIL
#define FAIL Q0
#include "jr0.h"
#ifdef ANDROID
#if __ANDROID_API__ < 15
#define log2(x) (log(x)/0.6931471805599453)
#endif
#endif

// qstd is invoked in QEPILOG
QF1(jtqstd){ // canonical form for w: 1155r210 -> 11r2
 if (unlikely(!w.n || !w.d)) RQ0; // quick exit for jsignal from an xnum routine
 if (ISQ0(w)) RQ0;
 if (ISQinf(w)) switch (QSGN(w)) { // libgmp does not handle infinity
   case -1: w.n= X_1; R w;
   case 0: jsignal(EVNAN); RQ0;
   case 1: w.n= X1; R w;
   default: SEGFAULT; // numerator of infinity must be magnitude 1
 }
 if (!ISQINT(w)) {
  X gcd= XgcdXX(w.n, w.d);
  if (0>XSGN(w.d)) XSGN(gcd)= -XSGN(gcd); // gcd is not shared
  if (!ISX1(gcd)) {
   w.n= Xfdiv_qXX(w.n, gcd);
   w.d= Xfdiv_qXX(w.d, gcd);
  }
 }
 R w;
}
QF2(jtqplus){PROLOG(10001); // a+w
 if (ISQinf(a)) {
  if (ISQinf(w)) {
   QASSERT(0<QSGN(a)*QSGN(w), EVNAN);
   R a;
  } else {
   R a;
  }
 } else if (ISQinf(w)) {
  R w;
 }
 QEPILOG(QaddQQ(a,w));
}
QF2(jtqminus){PROLOG(10002); // a-w
 if (ISQinf(a)) {
  if (ISQinf(w)) {
   QASSERT(0>QSGN(a)*QSGN(w), EVNAN);
   R a;
  } else {
   R a;
  }
 } else if (ISQinf(w)) {
  R 0>QSGN(w) ?Q_ :Q__;
 }
 QEPILOG(QsubQQ(a,w));
}
QF2(jtqtymes){PROLOG(10003); // a*w
 QEPILOG(QmulQQ(a, w));
}
QF2(jtqdiv){PROLOG(10004); // a%w
 QASSERT(!(ISQinf(a)&&ISQinf(w)), EVNAN);
 if (ISQ0(a)) RQ0;
 if (0==QSGN(w)) R QSGN(a) ?(0<QSGN(a) ?Q_ :Q__) :Q1;
 QEPILOG(QdivQQ(a,w));
}
static QF2(jtqrem){PROLOG(10005); // a|w
 if (ISQ0(a)) R w;
 QASSERT(!ISQinf(w), EVNAN);
 if (ISQinf(a)) {
  if(0>QSGN(a)*QSGN(w)) QEPILOG(a);
  R w;
 }
 X num= Xfdiv_rXX(XmulXX(w.n,a.d), XmulXX(a.n,w.d));
 X den= XmulXX(a.d, w.d);
 Q z= {num, den};
 QEPILOG(z);
}

static QF2(jtqgcd){PROLOG(10006); // a+.w
 QASSERT(XSGN(a.d)&&XSGN(w.d), EVNAN)
 X num= XgcdXX(XmulXX(a.n, w.d), XmulXX(w.n, a.d));
 X den= XmulXX(a.d, w.d);
 Q z= {num, den};
 QEPILOG(z);
}

// a*.w
static QF2(jtqlcm){PROLOG(10007);
 QASSERT(XSGN(a.d)&&XSGN(w.d), EVNAN)
 Q z= {xlcm(a.n, w.n), X1};
 QEPILOG(z);
}

/* a^w is complicated by need to shield from libgmp design decisions
 * if w is too large, libgmp would exit j
 * if a or w is infinite, libgmp would throw a floating point exception
 *
 * Note also that jt->xmod is a hidden influence here
 * this needs careful testing
 */
static QF2(jtqpow){PROLOG(10008); // a^w
 if (0==QSGN(w)) R Q1; // a^0
 if (ISQinf(a)) { // a is _ or __
  if (0<QSGN(a)) {
   R 0>QSGN(w) ?Q0 :Q_; // _^w
  } else { // a is __
   if (0>QSGN(w)) RQ0; // __^w where w is negative
   QASSERT(!ISQinf(w), EVDOMAIN) // sign unknown for __^_
   QEPILOG(XODDnz(w.n) ?Q__ :Q_); // __^w for odd or even w
  }
 }
 if (ISQinf(w)) { // w is _ or __
  int am1= icmpQX(a, X1);         // am has sign of a-1
  if (!am1) R Q1; // 1^w
  int ap1= icmpQX(a, X_1);        // ap1 has sign of a-_1
  QASSERT(ap1, EVDOMAIN); // NaN if a = _1
  if (0<QSGN(w)) { // a^_
   if (0<am1) R Q_; // 1<a
   QASSERT(0<ap1, EVDOMAIN); // NaN if a less than _1
   RQ0; // a between _1 and 1
  } else { // a^__
   if (0<am1 || 0>ap1) RQ0; // 1 < |a
   if (!QSGN(a)) R Q_; // 0^__
   QASSERT(0<QSGN(a), EVDOMAIN); // NaN if a between _1 and 0
   R Q_; // a between 0 and 1
  }
 } // now w is not zero and both a and w are finite (so result must be finite)
 if (0==QSGN(a)&&0<QSGN(w)) R Q0;
 if (ISQ1(a)) R Q1;
 QASSERT(ISQINT(w), 0<QSGN(a) ?EWIRR :EWIMAG);  // use float or complex for roots (when w.d is larger than 1) // might make sense here to special case exact roots?
 QASSERT(1==XLIMBLEN(w.n) && INT_MAX>XLIMB0(w.n), EVWSFULL); // w is too large
 I W= QSGN(w)*XLIMB0(w.n); D DW= llabs(W); // DW=: 0.0+|W=: w
 if (1==W) QEPILOG(a);                     // a^1
	if (-1==W) {Q z= {a.d, a.n}; QEPILOG(z);} // a^_1
 // handled in xpow: QASSERT(XLIMBLIM*8*sizeof (UI) > (NBITS(a.n)+NBITS(a.d))*DW, EVWSFULL); // a^w would be too bulky?
 if (0>W) {
  X p= XabsX(w.n); // we could simplify this, using XpowXU, when 0==jt->xmod
  Q z= {xpow(a.d, p), xpow(a.n, p)}; // use reciprocal of a and |w
  QEPILOG(z);
 } else {
  Q z= {xpow(a.n, w.n), xpow(a.d, w.n)}; 
  QEPILOG(z);
 }
}

I jtqcompare(J jt,Q a,Q w){R QCOMP(a,w);}

#undef FAIL
#include "jr0.h"

B jtqquad(J jt,E *z,Q W){
  if (ISQinf(W)){*z=(E){.hi=0<QSGN(W) ?inf :infm,.lo=0.};}
  else{
   mpQ(W); mpQ0(z); D h=jmpq_get_d(mpW); jmpq_set_d(mpz,h); jmpq_sub(mpz,mpW,mpz); D l=jmpq_get_d(mpz);  // high & low parts as D
   // if jmpq_get_d rounds correctly, h and l will both overlap.  In case not, we make sure they do
   D th=h+l; D tl=h-th; h=th; l+=tl;
   *z=CANONE1(h,l);   // convert to canonical form
  }
  R 1;
}

static X jtqbin(J jt,Q a,Q w){ASSERT(ISX1(a.d)&&ISX1(w.d),EWIRR); R rifvsdebug(xbin(a.n,w.n));}

static D jtqlogd1(J jt,Q w){ASSERT(0<=XSGN(w.n),EWIMAG); R xlogabs(w.n)-xlogabs(w.d);}

static Z jtqlogz1(J jt,Q w){Z z; z.re=xlogabs(w.n)-xlogabs(w.d); z.im=0>XSGN(w.n)?PI:0.0; R z;}

AMON(floorQQ, Q,Q, *z= ISQinf(*x) ?*x :QgetX(xdiv(x->n,x->d,XMFLR ));)
AMON( ceilQQ, Q,Q, *z= ISQinf(*x) ?*x :QgetX(xdiv(x->n,x->d,XMCEIL));)
AMON(  sgnQ, X,Q, *z=rifvsdebug(xsgn(x->n));            )
AMON(  absQ, Q,Q, z->n=XabsX(x->n); z->d=x->d;)
AMONPS(floorQ, X,Q, , *z=xdiv(x->n,x->d,XMFLR ); , HDR1JERR)
AMONPS( ceilQ, X,Q, , *z=xdiv(x->n,x->d,XMCEIL); , HDR1JERR)
AMONPS( sqrtQ, Q,Q, , QSQRT(x) , HDR1JERR)
AMONPS( factQ, Q,Q, , QFACT(x) , HDR1JERR)
AMONPS( logQD, D,Q, , *z=qlogd1(*x); , HDR1JERR)
AMONPS( logQZ, Z,Q, , *z=qlogz1(*x); , HDR1JERR)

APFX(  maxQQ, Q,Q,Q, QMAX  ,,R EVOK;)
APFX(  minQQ, Q,Q,Q, QMIN  ,,R EVOK;)
APFX( plusQQ, Q,Q,Q, qplus ,,HDR1JERR)
APFX(minusQQ, Q,Q,Q, qminus,,HDR1JERR)
APFX(tymesQQ, Q,Q,Q, qtymes,,HDR1JERR)
APFX(  divQQ, Q,Q,Q, qdiv  ,,HDR1JERR)
APFX(  gcdQQ, Q,Q,Q, qgcd  ,,HDR1JERR)
APFX(  lcmQQ, Q,Q,Q, qlcm  ,,HDR1JERR)
APFX(  remQQ, Q,Q,Q, qrem  ,,HDR1JERR)
APFX(  powQQ, Q,Q,Q, qpow  ,,HDR1JERR)
APFX(  binQQ, X,Q,Q, qbin  ,,HDR1JERR)
