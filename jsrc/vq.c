/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Rational Numbers                                                 */

#include "j.h"
#include "ve.h"


QF1(jtqstd){I d,n;Q z;X g;
 QRZ(w.n&&w.d&&!jt->jerr);
 n=XDIG(w.n); d=XDIG(w.d); z.d=xone;
 if(0>d){QRE(w.n=negate(w.n)); QRE(w.d=negate(w.d)); n=-n; d=-d;}
 if(!n){z.n=xzero; R z;}
 if(!d){z.n=vci(0<n?XPINF:XNINF); R z;}
 if(d==XPINF){QASSERT(n!=XPINF&&n!=XNINF,EVNAN); R zeroQ;}
 if(n==XPINF||n==XNINF){z.n=w.n; R z;}
 QRE(g=xgcd(w.n,w.d));
 if(QX1(g))R w;
 z.n=xdiv(w.n,g,XMEXACT);
 z.d=xdiv(w.d,g,XMEXACT);
 R z;
}

QF2(jtqplus){PROLOG(0083);Q z;
 z.n=xplus(xtymes(a.n,w.d),xtymes(w.n,a.d));
 z.d=xtymes(a.d,w.d);
 QEPILOG(z);
}

QF2(jtqminus){PROLOG(0084);Q z;
 z.n=xminus(xtymes(a.n,w.d),xtymes(w.n,a.d));
 z.d=xtymes(a.d,w.d);
 QEPILOG(z);
}

QF2(jtqtymes){PROLOG(0085);Q z;
 z.n=xtymes(a.n,w.n);
 z.d=xtymes(a.d,w.d);
 QEPILOG(z);
}

QF2(jtqdiv){PROLOG(0086);Q z;
 z.n=xtymes(a.n,w.d); 
 z.d=xtymes(a.d,w.n);
 QEPILOG(z);
}

static QF2(jtqrem){PROLOG(0087);I c,d;Q m,q,z;
 c=XDIG(a.n);
 d=XDIG(w.n);
 if(!c)R w;
 QASSERT(!(d==XPINF||d==XNINF),EVNAN);
 if(c==XPINF)R 0<=d?w:a;
 if(c==XNINF)R 0>=d?w:a;
 q=qdiv(w,a);
 m.n=xtymes(a.n,xdiv(q.n,q.d,XMFLR)); m.d=a.d;
 z=qminus(w,m);
 QEPILOG(z);
}

static QF2(jtqgcd){PROLOG(0088);Q z;
 QRE(z.n=xgcd(a.n,w.n));
 QRE(z.d=xlcm(a.d,w.d));
 QEPILOG(z);
}

static QF2(jtqlcm){R qtymes(a,qdiv(w,qgcd(a,w)));}

static QF2(jtqpow){PROLOG(0089);B c;I p,q,s;Q t,z;X d;
 QRE(1);
 t=a; d=w.n; z.n=z.d=xone;
 p=XDIG(a.n); q=XDIG(w.n); c=QX1(w.d);
 if(p==XPINF||p==XNINF){
  QASSERT(0<p||q!=XPINF,EVDOMAIN); 
  z.n=vci(!q?1L:0>q?0L:0<p?p:1&(*AV(w.n))?XNINF:XPINF);
  R z;
 }
 if(q==XPINF||q==XNINF){
  s=xcompare(mag(a.n),a.d);
  QASSERT(0<=p||0>s&&q==XPINF||0<s&&q==XNINF,EVDOMAIN); 
  z.n=vci(!s?1L:!p&&0>q||0<s&&p&&0<q||0>s&&0>q?XPINF:0L);
  R z;
 }
 QASSERT(c||0<=p,EWIMAG);
 QASSERT(c||QX1(a.d)&&1==AN(a.n)&&(0==p||1==p),EWIRR);
 if(0>XDIG(d)){QRE(d=negate(d)); QRE(t=qdiv(z,t));}
 if(1>xcompare(d,xc(IMAX))){I e;
  QRE(e=xint(d));  
  while(e){if(1&e)QRE(z=qtymes(z,t)); QRE(t=qtymes(t,t)); e>>=1;}
 }else{X e=d,x2;
  QRE(x2=xc(2L));
  while(XDIG(e)){
   if(1&*AV(e))QRE(z=qtymes(z,t)); 
   QRE(t=qtymes(t,t)); 
   QRE(e=xdiv(e,x2,XMFLR));
 }}
 QEPILOG(z);
}

int jtqcompare(J jt,Q a,Q w){R QCOMP(a,w);}

static X jtqbin(J jt,Q a,Q w){ASSERT(QX1(a.d)&&QX1(w.d),EWIRR); R xbin(a.n,w.n);}

static D jtqlogd1(J jt,Q w){ASSERT(0<=XDIG(w.n),EWIMAG); R xlogabs(w.n)-xlogabs(w.d);}

static Z jtqlogz1(J jt,Q w){Z z; z.re=xlogabs(w.n)-xlogabs(w.d); z.im=0>XDIG(w.n)?PI:0.0; R z;}


#define QSQRT(x)    z->n=xsqrt(x->n); z->d=xsqrt(x->d); if(jt->jerr)R;
#define QFACT(x)    ASSERTW(QX1(x->d),EWIRR); *z=xfact(x->n);

AMON(floorQ, X,Q, *z=xdiv(x->n,x->d,XMFLR );)
AMON( ceilQ, X,Q, *z=xdiv(x->n,x->d,XMCEIL);)
AMON(  sgnQ, X,Q, *z=xsgn(x->n);            )
AMON(  absQ, Q,Q, z->n=mag(x->n); z->d=x->d;)
AMON( sqrtQ, Q,Q, QSQRT(x))
AMON( factQ, X,Q, QFACT(x))
AMON( logQD, D,Q, *z=qlogd1(*x);)
AMON( logQZ, Z,Q, *z=qlogz1(*x);)

APFX(  maxQQ, Q,Q,Q, QMAX  )
APFX(  minQQ, Q,Q,Q, QMIN  )
APFX( plusQQ, Q,Q,Q, qplus )
APFX(minusQQ, Q,Q,Q, qminus)
APFX(tymesQQ, Q,Q,Q, qtymes)
APFX(  divQQ, Q,Q,Q, qdiv  )
APFX(  gcdQQ, Q,Q,Q, qgcd  )
APFX(  lcmQQ, Q,Q,Q, qlcm  )
APFX(  remQQ, Q,Q,Q, qrem  )
APFX(  powQQ, Q,Q,Q, qpow  )
APFX(  binQQ, X,Q,Q, qbin  )
