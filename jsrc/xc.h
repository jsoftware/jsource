/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Custom                                                           */


static DF2(jtcblas_dgemm);
static DF2(jtcblas_dscal);
static DF1(jtdgeev);
static DF1(jtdgeqrf);
static DF2(jtdmultmatmat_32x32);
static DF2(jtmatadd);
static DF2(jtseqex);
static DF2(jtseqpow);
static DF2(jtx102d0);
static DF2(jtx102d1);
static DF1(jtx103d0);
static F1(jtwex);
static F2(jttiebox);
static F1(jtbg);
static F1(jtgb);
static DF1(jttconj);
static DF2(jtoa);

#define bg(x)                   jtbg(jt, (x))
#define consfn(x)               jtconsfn(jt,(x))
#define gb(x)                   jtgb(jt, (x))
#define oa(x)                   jtoa(jt,(x))
#define seqdo(x0,x1,x2,x3,x4)   jtseqdo(jt,(x0),(x1),(x2),(x3),(x4))
#define seqlim(x,y,z)           jtseqlim(jt,(x),(y),(z))
#define tconj(x)                jttconj(jt, (x))
#define tiebox(x,y)             jttiebox(jt,(x),(y))
#define wex(x)                  jtwex(jt,(x))
