/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Rational Numbers                                                        */


#define QF1(f)          Q f(J jt,   Q  w)
#define QF2(f)          Q f(J jt,Q a,Q w)
#define QASSERT(b,e)    {if(!(b)){jsignal(e); R Q0;}}
#define QEPILOG(q)      {Q z9; z9=(qstd(q)); if(!gc3(&z9.n,&z9.d,0L,_ttop))R Q0; R z9;}
#define QRE(exp)        {if((exp),jt->jerr)R Q0;}
#define QRZ(exp)        {if(!(exp))        R Q0;}

#define QEQ(x,y)        (equx((x).n,(y).n)&&equx((x).d,(y).d))
#define QCOMP(x,y)      ({Q Qx= x, Qy= y; ISQinf(Qx) ?(ISQinf(Qy) ?XSGN(Qx.n)-XSGN(Qy.n) :QSGN(Qx)) :ISQinf(Qy) ?-QSGN(Qy) :icmpQQ(Qx,Qy);})
#define QLT(x,y)        (0> QCOMP(x,y))
#define QLE(x,y)        (0>=QCOMP(x,y))
#define QGT(x,y)        (0< QCOMP(x,y))
#define QGE(x,y)        (0<=QCOMP(x,y))
#define QMAX(x,y)       (QGE(x,y)?x:y)
#define QMIN(x,y)       (QLE(x,y)?x:y)

extern QF1(jtqstd);

extern QF2(jtqdiv);
extern QF2(jtqminus);
extern QF2(jtqplus);
extern QF2(jtqtymes);
