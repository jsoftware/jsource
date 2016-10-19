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
// If a==b with ct!=0 the comparisons will have opposite results: if a,b are positive,
//  a is > cct*b and b is not <= cct*a: result ==.  If a,b negative, a is < cct*b and b is
//  <= cct*a: again result ==
// If a==b with ct=0, a will be not > b and b will be <= a: result ==
// If a!=b, finite a,b, there are 6 cases:
//  a positive, b negative: a>cct*b, b <= cct*a: result !=
//  a negative, b positive: a not> cct*b, b not <= cct*a; result !=
//  a and b positive, a>b: a always > b*cct, b <= cct*a only if b is below tolerance: == if b above tolerance
//  a and b positive, a<b: a > b*cct only if a is above tolerance, b never <= cct*a: == if a above tolerance
//  a and b negative, a<b: a never > b*cct, b <= cct*a only if b is below tolerance: == if b below tolerance
//  a and b negative, a>b: a > b*cct only if b is below tolerance, b always <= cct*a: == if b not below tolerance
// If both a&b are same infinity, <= will succeed, > will fail, giving ==
// If only one is infinite, it will dominate both tests, giving !=
// If anything is _., all comparisons fail, giving !=
#define TCMPEQ(cct,a,b) ((((a)>(cct)*(b))?1:0) ^ (((b)<=(cct)*(a))?1:0))
#define TCMPNE(cct,a,b) (1 ^ TCMPEQ(cct,a,b))
#define TCMPLT(cct,a,b) (((a)<(b)) & TCMPNE(cct,a,b))
#define TCMPLE(cct,a,b) (((a)<(b)) | TCMPEQ(cct,a,b))


#define TEQ(u,v)       TCMPEQ(1-jt->ct,(u),(v))
#define TEQXD(u,v)     TEQ((D)u,   v)
#define TEQDX(u,v)     TEQ(   u,(D)v)
#define TNE(u,v)       TCMPNE(1-jt->ct,(u),(v))
#define TNEXD(u,v)     TNE((D)u,   v)
#define TNEDX(u,v)     TNE(   u,(D)v)
#define TLT(u,v)       TCMPLT(1-jt->ct,(u),(v))
#define TLTXD(u,v)     TLT((D)(u),   v )
#define TLTDX(u,v)     TLT(    u,(D)(v))
#define TLE(u,v)       TCMPLE(1-jt->ct,(u),(v))
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
