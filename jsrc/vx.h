/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Extended Precision                                                      */

#define XF1(f)          X f(J jt,    X w)
#define XF2(f)          X f(J jt,X a,X w)
#define XMAX(x,y)       (0<icmpXX(x,y)?x:y)
#define XMIN(x,y)       (0<icmpXX(x,y)?y:x)
#define XDIV(x,y)       xdiv(x,y,jt->xmode)
#define XCUBE(x)        xtymes(x,xtymes(x,x))

/* values for jt->xmode */
#define XMFLR           0                    /* floor,   round down        */
#define XMCEIL          1                    /* ceiling, round up          */
#define XMEXACT         2                    /* exact, error if impossible */
#define XMEXMT          3                    /* exact, empty if impossible */

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
extern XF2(jtxpowmodinv);
extern XF2(jtxtymes);

extern X   jtxc(J,I);
extern I   jtxcompare(J,X,X);
extern X   jtxdiv(J,X,X,I);
extern B   jtxdivrem(J,X,X,X*,X*);
extern X   jtxev1(J,A,C*);
extern X   jtxev2(J,A,A,C*);
extern I   jtxint(J,X);
extern D   jtxlogabs(J,X);
extern X   jtxrem(J,X,X);
