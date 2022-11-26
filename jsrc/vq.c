#include "j.h"
#include "ve.h"

#define QSQRT(x)    z->n=rifvsdebug(xsqrt(x->n)); z->d=rifvsdebug(xsqrt(x->d)); {I rc; if(rc=jt->jerr){RESETERR; R rc;}}
#define QFACT(x)    if(ISQinf(*x)&&0<QSGN(*x)) *z=Q_; else {ASSERTWR(ISQINT(*x),EWIRR); z->n=xfact(x->n); z->d=X1;}

#undef R0
#define R0 RQ0

QF1(jtqstd){// canonical form for w: 1155r210 -> 11r2
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
 XPERSIST(w.n);
 XPERSIST(w.d);
 R w;
}
QF2(jtqplus){PROLOG(0083); // a+w
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
 QEPILOG(qstd(QaddQQ(a,w)));
}
QF2(jtqminus){PROLOG(0084); // a-w
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
QF2(jtqtymes){PROLOG(0085); // a*w
 QEPILOG(QmulQQ(a, w));
}
QF2(jtqdiv){PROLOG(0086); // a%w
 QASSERT(!(ISQinf(a)&&ISQinf(w)), EVNAN);
 if (ISQ0(a)) R Q0;
 if (0==QSGN(w)) R QSGN(a) ?(0<QSGN(a) ?Q_ :Q__) :Q1;
 QEPILOG(QdivQQ(a,w));
}
static QF2(jtqrem){PROLOG(0087); // a|w
 if (ISQ0(a)) R w;
 QASSERT(!ISQinf(w), EVNAN);
 if (ISQinf(a)) {
  if(0>QSGN(a)*QSGN(w))R a;
  R w;
 }
 X num= Xfdiv_rXX(XmulXX(w.n,a.d), XmulXX(a.n,w.d));
 X den= XmulXX(a.d, w.d);
 Q z= {num, den};
 QEPILOG(z);
}

static QF2(jtqgcd){PROLOG(0088); // a+.w
 QASSERT(XSGN(a.d)&&XSGN(w.d), EVNAN)
 X num= XgcdXX(XmulXX(a.n, w.d), XmulXX(w.n, a.d));
 X den= XmulXX(a.d, w.d);
 Q z= {num, den};
 QEPILOG(z);
}

// a*.w
static QF2(jtqlcm){
 QASSERT(XSGN(a.d)&&XSGN(w.d), EVNAN)
 Q z= {xlcm(a.n, w.n), X1};
 R z;
}

/* a^w is complicated by need to shield from libgmp design decisions
 * if w is too large, libgmp would exit j
 * if a or w is infinite, libgmp would throw a floating point exception
 *
 * Note also that jt->xmod is a hidden influence here
 * this needs careful testing
 */
static QF2(jtqpow){PROLOG(0089); // a^w
 if (0==QSGN(w)) R Q1; // a^0
 if (ISQinf(a)) { // a is _ or __
  if (0<QSGN(a)) {
   R 0>QSGN(w) ?Q0 :Q_; // _^w
  } else { // a is __
   if (0>QSGN(w)) RQ0; // __^w where w is negative
   QASSERT(!ISQinf(w), EVDOMAIN) // sign unknown for __^_
   R XODDnz(w.n) ?Q__ :Q_; // __^w for odd or even w
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
 if (1==W) R a;            // a^1
 QASSERT(XLIMBLIM>XLIMBLEN(a.n)*DW, EVWSFULL) // a^w would be too large
 if (1==XLIMBLEN(a.n)) { // approximate result to check size
  D est= (log2(fabs(DgetX(a.n)))+log2(DgetX(a.d)))*DW/(8*sizeof (UI));
  QASSERT(XLIMBLIM > est, EVWSFULL); // a^w would be too bulky
 }
 if (0>W) {
  X p= XabsX(w.n); // we could simplify this, using XpowXU, when 0==jt->xmod
  Q z= {xpow(a.d, p), xpow(a.n, p)}; // use reciprocal of a and |w
  QEPILOG(z);
 } else {
  Q z= {xpow(a.n, w.n), xpow(a.d, w.n)}; 
  QEPILOG(z);
 } // don't actually need qstd, because a was in canonical form
}

I jtqcompare(J jt,Q a,Q w){R QCOMP(a,w);}

#undef R0
#define R0 R 0

static X jtqbin(J jt,Q a,Q w){ASSERT(ISX1(a.d)&&ISX1(w.d),EWIRR); R rifvsdebug(xbin(a.n,w.n));}

static D jtqlogd1(J jt,Q w){ASSERT(0<=XSGN(w.n),EWIMAG); R xlogabs(w.n)-xlogabs(w.d);}

static Z jtqlogz1(J jt,Q w){Z z; z.re=xlogabs(w.n)-xlogabs(w.d); z.im=0>XSGN(w.n)?PI:0.0; R z;}

AMON(floorQ, X,Q, *z=rifvsdebug(xdiv(x->n,x->d,XMFLR ));)
AMON( ceilQ, X,Q, *z=rifvsdebug(xdiv(x->n,x->d,XMCEIL));)
AMON(  sgnQ, X,Q, *z=rifvsdebug(xsgn(x->n));            )
AMON(  absQ, Q,Q, z->n=XabsX(x->n); z->d=x->d;)
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
