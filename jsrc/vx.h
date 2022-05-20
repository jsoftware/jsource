/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Extended Precision                                                      */

#if SY_64
#define XIDIG           5                  /* max # x digits in an integer */
#else
#define XIDIG           3
#endif

#define XBASE           (I)10000
#define XBASEN          (I)4
#define XPINF           (I)99999
#define XNINF           (I)-99999
#define XF1(f)          X f(J jt,    X w)
#define XF2(f)          X f(J jt,X a,X w)
#define XDIG(a)         (AV(a)[AN(a)-1])   /* leading digit              */
#define XMAX(x,y)       (1==xcompare(x,y)?x:y)
#define XMIN(x,y)       (1==xcompare(x,y)?y:x)
#define XDIV(x,y)       xdiv(x,y,jt->xmode)
#define XCUBE(x)        xtymes(x,xtymes(x,x))



/* values for jt->xmode */

#define XMFLR           0                    /* floor,   round down        */
#define XMCEIL          1                    /* ceiling, round up          */
#define XMEXACT         2                    /* exact, error if impossible */
#define XMEXMT          3                    /* exact, empty if impossible */
// obsolete #define XMRND           4                    /* round,   round to nearest  */


extern XF1(jtxfact);
extern XF1(jtxsgn);
extern XF1(jtxsq);
extern XF1(jtxsqrt);
extern XF1(jtxstd);

extern XF2(jtxbin);
extern XF2(jtxgcd);
extern XF2(jtxlcm);
extern XF2(jtxminus);
extern XF2(jtxplus);
extern XF2(jtxpow);
extern XF2(jtxtymes);

extern X   jtxc(J,I);
extern I jtxcompare(J,X,X);
extern X   jtxdiv(J,X,X,I);
extern B   jtxdivrem(J,X,X,X*,X*);
extern X   jtxev1(J,A,C*);
extern X   jtxev2(J,A,A,C*);
extern I   jtxint(J,X);
extern D   jtxlogabs(J,X);
extern X   jtxrem(J,X,X);
