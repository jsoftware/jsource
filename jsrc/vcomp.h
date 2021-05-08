/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Comparatives                                                     */

/* See  R. Bernecky, Comparison Tolerance, SATN-23, IPSA, 1977-06-10       */
/* For finite numbers, the following defns hold:                           */
/*                                                                         */
/*   TEQ(u,v)   ABS(u-v)<=ct*MAX(ABS(u),ABS(v))                            */
/*   tne(u,v)   !TEQ(u,v)                                                  */
/*   tlt(u,v)   (u< v)&&tne(u,v)                                           */
/*   tle(u,v)   (u<=v)||TEQ(u,v)                                           */
/*   tge(u,v)   (u>=v)||TEQ(u,v)                                           */
/*   tgt(u,v)   (u> v)&&tne(u,v)                                           */
/*   tfloor(v)  x=floor(0.5+v), x-tgt(x,v)                                 */
/*   tceil(v)   x=floor(0.5+v), x+tlt(x,v)                                 */

// cct is complementary comp tolerance (jt->cct).  Result is 1 if tolerantly equal
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
// These are designed to be used in IF statements where they can produce fused comparisons
#define TCMPEQ(cct,a,b) ((((a)>(cct)*(b))?1:0) != (((b)<=(cct)*(a))?1:0))
#define TCMPNE(cct,a,b) ((((a)>(cct)*(b))?1:0) == (((b)<=(cct)*(a))?1:0))
#define TCMPLT(cct,a,b) (((a)<(b)) & ((((a)>(cct)*(b))?1:0) ^ (((b)>(cct)*(a))?1:0)))
#define TCMPLE(cct,a,b) (((a)<(b)) | ((((a)>(cct)*(b))?1:0) ^ (((b)<=(cct)*(a))?1:0)))  // must use | to make _. always compare false
// This version to produce a boolean result
#define TCMPEQB(cct,a,b) ((((a)>(cct)*(b))?1:0) ^ (((b)<=(cct)*(a))?1:0))

#define TEQ(u,v)       TCMPEQ(jt->cct,(u),(v))
#define TEQXD(u,v)     TEQ((D)u,   v)
#define TEQDX(u,v)     TEQ(   u,(D)v)
#define TNE(u,v)       TCMPNE(jt->cct,(u),(v))
#define TNEXD(u,v)     TNE((D)u,   v)
#define TNEDX(u,v)     TNE(   u,(D)v)
#define TLT(u,v)       TCMPLT(jt->cct,(u),(v))
#define TLTXD(u,v)     TLT((D)(u),   v )
#define TLTDX(u,v)     TLT(    u,(D)(v))
#define TLE(u,v)       TCMPLE(jt->cct,(u),(v))
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

// bytewise comparisons of characters.  Leaves a bit set if the condition is true 
#define ACNE(x,y)      ((x)^(y))
#define ACEQ(x,y)      (((~((x)^(y))|BOOLEANSIGN)-~BOOLEANSIGN)&~((x)^(y))&BOOLEANSIGN)  // low 7 bits 0, and high bit 0, after XOR
#define ACEQB(x,y)     ((UI)ACEQ(x,y)>>(BB-1))  // same, as boolean in bit 0 of byte
#define ACNEB(x,y)     ((UI)((((((x)^(y))&~BOOLEANSIGN)+~BOOLEANSIGN)|((x)^(y)))&BOOLEANSIGN)>>(BB-1))

// intolerant versions of the same form
#define EQ0(u,v)       ((u)==(v))
#define EQXD0(u,v)     EQ0((D)u,   v)
#define EQDX0(u,v)     EQ0(   u,(D)v)
#define NE0(u,v)       ((u)!=(v))
#define NEXD0(u,v)     NE0((D)u,   v)
#define NEDX0(u,v)     NE0(   u,(D)v)
#define LT0(u,v)       ((u)<(v))
#define LTXD0(u,v)     LT0((D)(u),   v )
#define LTDX0(u,v)     LT0(    u,(D)(v))
#define LE0(u,v)       ((u)<=(v))
#define LEXD0(u,v)     LE0((D)(u),    v )
#define LEDX0(u,v)     LE0(    u ,(D)(v))
#define GT0(u,v)       ((u)>(v))
#define GTXD0(u,v)     GT0((D)(u),    v )
#define GTDX0(u,v)     GT0((D)(u),    v )
#define GE0(u,v)       ((u)>=(v))
#define GEXD0(u,v)     GE0((D)(u),    v )
#define GEDX0(u,v)     GE0((D)(u),    v )
