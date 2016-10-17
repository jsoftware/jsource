/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Comparatives                                                     */

/* See  R. Bernecky, Comparison Tolerance, SATN-23, IPSA, 1977-06-10       */
/* For finite numbers, the following defns hold:                           */
/*                                                                         */
/*   teq(u,v)   ABS(u-v)<=ct*MAX(ABS(u),ABS(v))                            */
/*   tne(u,v)   !teq(u,v)                                                  */
/*   tlt(u,v)   (u< v)&&tne(u,v)                                           */
/*   tle(u,v)   (u<=v)||teq(u,v)                                           */
/*   tge(u,v)   (u>=v)||teq(u,v)                                           */
/*   tgt(u,v)   (u> v)&&tne(u,v)                                           */
/*   tfloor(v)  x=floor(0.5+v), x-tgt(x,v)                                 */
/*   tceil(v)   x=floor(0.5+v), x+tlt(x,v)                                 */

// cct is complementary comp tolerance (1-jt->ct).  Result is 1 if tolerantly equal
// If a==b the comparisons will have opposite results, i. e. 1
// If a!=b, finite a,b, the larger will compare greater and the other will compare le as long
// as the tolerance holds.
// If both a&b are same infinity, <= will succeed, > will fail, giving ==
// If only one is infinite, it will dominate both tests, giving !=
// If anything is _., all comparisons fail, giving !=
#define TCMPEQ(cct,a,b) ((((a)>(cct)*(b))?1:0) ^ (((b)<=(cct)*(a))?1:0))

#define TEQ            teq
#define TEQXD(u,v)     teq((D)u,   v)
#define TEQDX(u,v)     teq(   u,(D)v)
#define TNE            !teq
#define TNEXD          !TEQXD
#define TNEDX          !TEQDX
#define TLT            tlt
#define TLTXD(u,v)     tlt((D)(u),   v )
#define TLTDX(u,v)     tlt(    u,(D)(v))
#define TLE(u,v)       (tlt(u,v) || teq(u,v))
#define TLEXD(u,v)     TLE((D)(u),    v )
#define TLEDX(u,v)     TLE(    u ,(D)(v))
#define TGT(u,v)       TLT(    v ,    u )
#define TGTXD(u,v)     TLT(    v ,(D)(u))
#define TGTDX(u,v)     TLT((D)(v),    u )
#define TGE(u,v)       TLE(    v ,    u )
#define TGEXD(u,v)     TLE(    v ,(D)(u))
#define TGEDX(u,v)     TLE((D)(v),    u )

#define ALT(x,y)       ((x)< (y))
#define ALE(x,y)       ((x)<=(y))
#define AEQ(x,y)       ((x)==(y))
#define ANE(x,y)       ((x)!=(y))
#define AGE(x,y)       ((x)>=(y))
#define AGT(x,y)       ((x)> (y))
