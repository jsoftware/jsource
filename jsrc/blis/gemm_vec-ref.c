/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* gemm micro kernel                                                       */

#include "../blis.h"

#if BLIS_DRIVER == BLIS_DRIVER_GNUVEC

#define MC  BLIS_DEFAULT_MC_D
#define KC  BLIS_DEFAULT_KC_D
#define NC  BLIS_DEFAULT_NC_D
#define MR  BLIS_DEFAULT_MR_D
#define NR  BLIS_DEFAULT_NR_D

#if !((MR==6)&&(NR==4))
#error "invalid MR NR"
#endif

#define LOOKAHEAD_A   6*24  // multiple of 24 = 4*MR (24*8 bytes)
#define LOOKAHEAD_B   6*16  // multiple of 16 = 4*NR (16*8 bytes)

typedef double vx __attribute__((vector_size (16)));     // (rwidth/8)

#define MM_SETZERO_PD( )      (vx){}
#define MM_LOAD_PD( a )       (vx){(a)[0], (a)[1]}
#define MM_SET1_PD( a )       (vx){(a), (a)}
#define MM_FMADD_PD( a, b, c) (a)*(b)+(c)
#define MM_MUL_PD( a, b)      (a)*(b)
#define MM_SUB_PD( a, b)      (a)-(b)
#define MM_ADD_PD( a, b)      (a)+(b)
#define MM_SWAP_PD( a, b)     (vx){(a)[1],(b)[0]}
#define MM_SHUFFLE_PD( a, b)  (vx){(a)[0],(b)[1]}

void bli_dgemm_128_6x4
(
  dim_t               k,
  double*    restrict alpha_,
  double*    restrict a,
  double*    restrict b,
  double*    restrict beta_,
  double*    restrict c, inc_t rs_c, inc_t cs_c,
  auxinfo_t* restrict data,
  cntx_t*    restrict cntx
)
{
#if (defined(__clang__) && ( (__clang_major__ > 3) || ((__clang_major__ == 3) && ((__clang_minor__ > 5) || !(defined(__aarch32__)||defined(__arm__)||defined(__aarch64__)) )))) || __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 6))
  a = (double*) __builtin_assume_aligned (a, alignv);
  b = (double*) __builtin_assume_aligned (b, alignv);
#endif

  double alpha=*alpha_, beta=*beta_;

  vx a_re, a_im;
  vx b_0, b_1;

  const double *va = (const double *)a;

  vx t0_00, t0_01, t0_02, t0_03;
  vx t1_00, t1_01, t1_02, t1_03;
  vx t2_00, t2_01, t2_02, t2_03;

  t0_00 = MM_SETZERO_PD( );
  t0_01 = MM_SETZERO_PD( );
  t0_02 = MM_SETZERO_PD( );
  t0_03 = MM_SETZERO_PD( );
  t1_00 = MM_SETZERO_PD( );
  t1_01 = MM_SETZERO_PD( );
  t1_02 = MM_SETZERO_PD( );
  t1_03 = MM_SETZERO_PD( );
  t2_00 = MM_SETZERO_PD( );
  t2_01 = MM_SETZERO_PD( );
  t2_02 = MM_SETZERO_PD( );
  t2_03 = MM_SETZERO_PD( );

  const double *p0,*p1,*p2;
  vx pp0[4],pp1[4],pp2[4];

  PREFETCH((void*)&c[0*cs_c]);
  PREFETCH((void*)&c[1*cs_c]);
  PREFETCH((void*)&c[2*cs_c]);
  PREFETCH((void*)&c[3*cs_c]);
  PREFETCH((void*)&c[4*cs_c]);
  PREFETCH((void*)&c[5*cs_c]);

  gint_t l;
  gint_t ki = k>>2;  // unroll
  gint_t kr = k&3;
  for (l=0; l<ki; ++l) {

// iter 0
    PREFETCH((void*)(b+LOOKAHEAD_B )); // PREFETCH((void*)(b+2*4*NR));
    PREFETCH((void*)(va+LOOKAHEAD_A ));

    b_0 = MM_LOAD_PD( (double*)b );
    b_1 = MM_LOAD_PD( 2+(double*)b );

    a_re = MM_SET1_PD( va[0] );
    a_im = MM_SET1_PD( va[1] );

    t0_00 = MM_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_1, a_im, t0_03);

    a_re = MM_SET1_PD( va[2] );
    a_im = MM_SET1_PD( va[3] );

    t1_00 = MM_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM_FMADD_PD(b_1, a_im, t1_03);

    a_re = MM_SET1_PD( va[4] );
    a_im = MM_SET1_PD( va[5] );

    t2_00 = MM_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

// iter 1
    PREFETCH((void*)(b+LOOKAHEAD_B +8));
    PREFETCH((void*)(va+LOOKAHEAD_A +8));

    b_0 = MM_LOAD_PD( 4+(double*)b );
    b_1 = MM_LOAD_PD( 6+(double*)b );

    a_re = MM_SET1_PD( va[6] );
    a_im = MM_SET1_PD( va[7] );

    t0_00 = MM_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_1, a_im, t0_03);

    a_re = MM_SET1_PD( va[8] );
    a_im = MM_SET1_PD( va[9] );

    t1_00 = MM_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM_FMADD_PD(b_1, a_im, t1_03);

    a_re = MM_SET1_PD( va[10] );
    a_im = MM_SET1_PD( va[11] );

    t2_00 = MM_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

// iter 2
    PREFETCH((void*)(b+LOOKAHEAD_B +16));
    PREFETCH((void*)(va+LOOKAHEAD_A +16));

    b_0 = MM_LOAD_PD( 8+(double*)b );
    b_1 = MM_LOAD_PD( 10+(double*)b );

    a_re = MM_SET1_PD( va[12] );
    a_im = MM_SET1_PD( va[13] );

    t0_00 = MM_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_1, a_im, t0_03);

    a_re = MM_SET1_PD( va[14] );
    a_im = MM_SET1_PD( va[15] );

    t1_00 = MM_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM_FMADD_PD(b_1, a_im, t1_03);

    a_re = MM_SET1_PD( va[16] );
    a_im = MM_SET1_PD( va[17] );

    t2_00 = MM_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

// iter 3
    PREFETCH((void*)(b+LOOKAHEAD_B +24));

    b_0 = MM_LOAD_PD( 12+(double*)b );
    b_1 = MM_LOAD_PD( 14+(double*)b );

    a_re = MM_SET1_PD( va[18] );
    a_im = MM_SET1_PD( va[19] );

    t0_00 = MM_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_1, a_im, t0_03);

    a_re = MM_SET1_PD( va[20] );
    a_im = MM_SET1_PD( va[21] );
    t1_00 = MM_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM_FMADD_PD(b_1, a_im, t1_03);

    a_re = MM_SET1_PD( va[22] );
    a_im = MM_SET1_PD( va[23] );

    t2_00 = MM_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

    b += 4*NR;        // unroll*NR
    va += 4*MR;       // unroll*MR  va is pointer to double
  }

  for (l=0; l<kr; ++l) {

// look ahead too small
//    PREFETCH((void*)(b+8));   // PREFETCH((void*)(b+2*NR));
//    PREFETCH((void*)(va+12));

    b_0 = MM_LOAD_PD( (double*)b );
    b_1 = MM_LOAD_PD( 2+(double*)b );

    a_re = MM_SET1_PD( va[0] );
    a_im = MM_SET1_PD( va[1] );

    t0_00 = MM_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_1, a_im, t0_03);

    a_re = MM_SET1_PD( va[2] );
    a_im = MM_SET1_PD( va[3] );

    t1_00 = MM_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM_FMADD_PD(b_1, a_im, t1_03);

    a_re = MM_SET1_PD( va[4] );
    a_im = MM_SET1_PD( va[5] );

    t2_00 = MM_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

    b += NR;
    va += MR;       // va is pointer to double
  }

  if (alpha!=1.0) {
    a_re = MM_SET1_PD( alpha );

    t0_00 = MM_MUL_PD ( a_re , t0_00 );
    t0_01 = MM_MUL_PD ( a_re , t0_01 );
    t0_02 = MM_MUL_PD ( a_re , t0_02 );
    t0_03 = MM_MUL_PD ( a_re , t0_03 );

    t1_00 = MM_MUL_PD ( a_re , t1_00 );
    t1_01 = MM_MUL_PD ( a_re , t1_01 );
    t1_02 = MM_MUL_PD ( a_re , t1_02 );
    t1_03 = MM_MUL_PD ( a_re , t1_03 );

    t2_00 = MM_MUL_PD ( a_re , t2_00 );
    t2_01 = MM_MUL_PD ( a_re , t2_01 );
    t2_02 = MM_MUL_PD ( a_re , t2_02 );
    t2_03 = MM_MUL_PD ( a_re , t2_03 );

  }

  if (beta!=1.0) {
    gint_t i;
    for (i=0; i<MR; ++i) {
      gint_t j;
      for (j=0; j<NR; ++j) {
        c[i*rs_c+j*cs_c] *= beta;
      }
    }
  }

  p0 = (const double *) &pp0;
  p1 = (const double *) &pp1;
  p2 = (const double *) &pp2;
  pp0[0] = t0_00;
  pp0[1] = t0_01;
  pp0[2] = t0_02;
  pp0[3] = t0_03;
  pp1[0] = t1_00;
  pp1[1] = t1_01;
  pp1[2] = t1_02;
  pp1[3] = t1_03;
  pp2[0] = t2_00;
  pp2[1] = t2_01;
  pp2[2] = t2_02;
  pp2[3] = t2_03;

  gint_t pc;

  pc = 0;
  c[pc      ] += p0[0];
  c[pc+=cs_c] += p0[1];
  c[pc+=cs_c] += p0[2];
  c[pc+=cs_c] += p0[3];

  pc = 1*rs_c;
  c[pc      ] += p0[4];
  c[pc+=cs_c] += p0[5];
  c[pc+=cs_c] += p0[6];
  c[pc+=cs_c] += p0[7];

  pc = 2*rs_c;
  c[pc      ] += p1[0];
  c[pc+=cs_c] += p1[1];
  c[pc+=cs_c] += p1[2];
  c[pc+=cs_c] += p1[3];

  pc = 3*rs_c;
  c[pc      ] += p1[4];
  c[pc+=cs_c] += p1[5];
  c[pc+=cs_c] += p1[6];
  c[pc+=cs_c] += p1[7];

  pc = 4*rs_c;
  c[pc      ] += p2[0];
  c[pc+=cs_c] += p2[1];
  c[pc+=cs_c] += p2[2];
  c[pc+=cs_c] += p2[3];

  pc = 5*rs_c;
  c[pc      ] += p2[4];
  c[pc+=cs_c] += p2[5];
  c[pc+=cs_c] += p2[6];
  c[pc+=cs_c] += p2[7];

}

#undef MC
#undef KC
#undef NC
#undef MR
#undef NR

#undef LOOKAHEAD_A
#undef LOOKAHEAD_B

#define MC  BLIS_DEFAULT_MC_Z
#define KC  BLIS_DEFAULT_KC_Z
#define NC  BLIS_DEFAULT_NC_Z
#define MR  BLIS_DEFAULT_MR_Z
#define NR  BLIS_DEFAULT_NR_Z


#if !((MR==3)&&(NR==2))
#error "invalid MR NR"
#endif

#define LOOKAHEAD_A   3*24  // multiple of 24 = 4*MR*2 (24*8 bytes)
#define LOOKAHEAD_B   3*8   // multiple of  8 = 4*NR   (8*16 bytes)

void bli_zgemm_128_3x2
(
  dim_t               k,
  dcomplex*  restrict alpha_,
  dcomplex*  restrict a,
  dcomplex*  restrict b,
  dcomplex*  restrict beta_,
  dcomplex*  restrict c, inc_t rs_c, inc_t cs_c,
  auxinfo_t* restrict data,
  cntx_t*    restrict cntx
)
{
  dcomplex alpha=*alpha_, beta=*beta_;

  vx a_re, a_im;
  vx b_0, b_1;

  const double *va = (const double *)a;

  vx t0_00, t0_01, t0_02, t0_03;
  vx t1_00, t1_01, t1_02, t1_03;
  vx t2_00, t2_01, t2_02, t2_03;

  t0_00 = MM_SETZERO_PD( );
  t0_01 = MM_SETZERO_PD( );
  t0_02 = MM_SETZERO_PD( );
  t0_03 = MM_SETZERO_PD( );
  t1_00 = MM_SETZERO_PD( );
  t1_01 = MM_SETZERO_PD( );
  t1_02 = MM_SETZERO_PD( );
  t1_03 = MM_SETZERO_PD( );
  t2_00 = MM_SETZERO_PD( );
  t2_01 = MM_SETZERO_PD( );
  t2_02 = MM_SETZERO_PD( );
  t2_03 = MM_SETZERO_PD( );

  const double *p0,*p1,*p2;
  vx pp0[2],pp1[2],pp2[2];

  PREFETCH((void*)&c[0*rs_c+0*cs_c]);
  PREFETCH((void*)&c[0*rs_c+1*cs_c]);
  PREFETCH((void*)&c[0*rs_c+2*cs_c]);

  gint_t l;
  gint_t ki = k>>2;  // unroll
  gint_t kr = k&3;
  for (l=0; l<ki; ++l) {

// iter 0
    PREFETCH((void*)(b+LOOKAHEAD_B )); // PREFETCH((void*)(b+2*4*NR));
    PREFETCH((void*)(va+LOOKAHEAD_A ));

    b_0 = MM_LOAD_PD( (double*)b );
    b_1 = MM_LOAD_PD( 2+(double*)b );

    a_re = MM_SET1_PD( va[0] );
    a_im = MM_SET1_PD( va[1] );

    t0_00 = MM_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_1, a_im, t0_03);

    a_re = MM_SET1_PD( va[2] );
    a_im = MM_SET1_PD( va[3] );

    t1_00 = MM_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM_FMADD_PD(b_1, a_im, t1_03);

    a_re = MM_SET1_PD( va[4] );
    a_im = MM_SET1_PD( va[5] );

    t2_00 = MM_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

// iter 1
    PREFETCH((void*)(b+LOOKAHEAD_B +4));
    PREFETCH((void*)(va+LOOKAHEAD_A +8));

    b_0 = MM_LOAD_PD( 4+(double*)b );
    b_1 = MM_LOAD_PD( 6+(double*)b );

    a_re = MM_SET1_PD( va[6] );
    a_im = MM_SET1_PD( va[7] );

    t0_00 = MM_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_1, a_im, t0_03);

    a_re = MM_SET1_PD( va[8] );
    a_im = MM_SET1_PD( va[9] );

    t1_00 = MM_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM_FMADD_PD(b_1, a_im, t1_03);

    a_re = MM_SET1_PD( va[10] );
    a_im = MM_SET1_PD( va[11] );

    t2_00 = MM_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

// iter 2
    PREFETCH((void*)(b+LOOKAHEAD_B +8));
    PREFETCH((void*)(va+LOOKAHEAD_A +16));

    b_0 = MM_LOAD_PD( 8+(double*)b );
    b_1 = MM_LOAD_PD( 10+(double*)b );

    a_re = MM_SET1_PD( va[12] );
    a_im = MM_SET1_PD( va[13] );

    t0_00 = MM_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_1, a_im, t0_03);

    a_re = MM_SET1_PD( va[14] );
    a_im = MM_SET1_PD( va[15] );

    t1_00 = MM_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM_FMADD_PD(b_1, a_im, t1_03);

    a_re = MM_SET1_PD( va[16] );
    a_im = MM_SET1_PD( va[17] );

    t2_00 = MM_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

// iter 3
    PREFETCH((void*)(b+LOOKAHEAD_B +12));

    b_0 = MM_LOAD_PD( 12+(double*)b );
    b_1 = MM_LOAD_PD( 14+(double*)b );

    a_re = MM_SET1_PD( va[18] );
    a_im = MM_SET1_PD( va[19] );

    t0_00 = MM_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_1, a_im, t0_03);

    a_re = MM_SET1_PD( va[20] );
    a_im = MM_SET1_PD( va[21] );
    t1_00 = MM_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM_FMADD_PD(b_1, a_im, t1_03);

    a_re = MM_SET1_PD( va[22] );
    a_im = MM_SET1_PD( va[23] );

    t2_00 = MM_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

    b += 4*NR;        // unroll*NR
    va += 4*MR*2;     // unroll*MR  va is pointer to double
  }

  for (l=0; l<kr; ++l) {

// look ahead too small
//    PREFETCH((void*)(b+4)); // PREFETCH((void*)(b+2*NR));
//    PREFETCH((void*)(va+12));

    b_0 = MM_LOAD_PD( (double*)b );
    b_1 = MM_LOAD_PD( 2+(double*)b );

    a_re = MM_SET1_PD( va[0] );
    a_im = MM_SET1_PD( va[1] );

    t0_00 = MM_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_1, a_im, t0_03);

    a_re = MM_SET1_PD( va[2] );
    a_im = MM_SET1_PD( va[3] );

    t1_00 = MM_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM_FMADD_PD(b_1, a_im, t1_03);

    a_re = MM_SET1_PD( va[4] );
    a_im = MM_SET1_PD( va[5] );

    t2_00 = MM_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

    b += NR;
    va += MR*2;       // va is pointer to double
  }

  t0_02 = MM_SWAP_PD ( t0_02 , t0_02 );
  t0_03 = MM_SWAP_PD ( t0_03 , t0_03 );

  t1_02 = MM_SWAP_PD ( t1_02 , t1_02 );
  t1_03 = MM_SWAP_PD ( t1_03 , t1_03 );

  t2_02 = MM_SWAP_PD ( t2_02 , t2_02 );
  t2_03 = MM_SWAP_PD ( t2_03 , t2_03 );

  b_0 = MM_SUB_PD ( t0_00 , t0_02 );
  b_1 = MM_ADD_PD ( t0_00 , t0_02 );
  t0_00 = MM_SHUFFLE_PD ( b_0 , b_1 );

  b_0 = MM_SUB_PD ( t0_01 , t0_03 );
  b_1 = MM_ADD_PD ( t0_01 , t0_03 );
  t0_01 = MM_SHUFFLE_PD ( b_0 , b_1 );

  b_0 = MM_SUB_PD ( t1_00 , t1_02 );
  b_1 = MM_ADD_PD ( t1_00 , t1_02 );
  t1_00 = MM_SHUFFLE_PD ( b_0 , b_1 );

  b_0 = MM_SUB_PD ( t1_01 , t1_03 );
  b_1 = MM_ADD_PD ( t1_01 , t1_03 );
  t1_01 = MM_SHUFFLE_PD ( b_0 , b_1 );

  b_0 = MM_SUB_PD ( t2_00 , t2_02 );
  b_1 = MM_ADD_PD ( t2_00 , t2_02 );
  t2_00 = MM_SHUFFLE_PD ( b_0 , b_1 );

  b_0 = MM_SUB_PD ( t2_01 , t2_03 );
  b_1 = MM_ADD_PD ( t2_01 , t2_03 );
  t2_01 = MM_SHUFFLE_PD ( b_0 , b_1 );

  if (alpha.real!=1.0||alpha.imag!=0.0) {
    a_re = MM_SET1_PD( alpha.real );
    a_im = MM_SET1_PD( alpha.imag );

    t0_02 = MM_SWAP_PD ( t0_00 , t0_00 );
    t0_00 = MM_MUL_PD ( a_re , t0_00 );
    t0_02 = MM_MUL_PD ( a_im , t0_02 );
    b_0 = MM_SUB_PD ( t0_00 , t0_02 );
    b_1 = MM_ADD_PD ( t0_00 , t0_02 );
    t0_00 = MM_SHUFFLE_PD ( b_0 , b_1 );

    t0_02 = MM_SWAP_PD ( t0_01 , t0_01 );
    t0_01 = MM_MUL_PD ( a_re , t0_01 );
    t0_02 = MM_MUL_PD ( a_im , t0_02 );
    b_0 = MM_SUB_PD ( t0_01 , t0_03 );
    b_1 = MM_ADD_PD ( t0_01 , t0_03 );
    t0_01 = MM_SHUFFLE_PD ( b_0 , b_1 );

    t1_02 = MM_SWAP_PD ( t1_00 , t1_00 );
    t1_00 = MM_MUL_PD ( a_re , t1_00 );
    t1_02 = MM_MUL_PD ( a_im , t1_02 );
    b_0 = MM_SUB_PD ( t1_00 , t1_02 );
    b_1 = MM_ADD_PD ( t1_00 , t1_02 );
    t1_00 = MM_SHUFFLE_PD ( b_0 , b_1 );

    t1_02 = MM_SWAP_PD ( t1_01 , t1_01 );
    t1_01 = MM_MUL_PD ( a_re , t1_01 );
    t1_02 = MM_MUL_PD ( a_im , t1_02 );
    b_0 = MM_SUB_PD ( t1_01 , t1_03 );
    b_1 = MM_ADD_PD ( t1_01 , t1_03 );
    t1_01 = MM_SHUFFLE_PD ( b_0 , b_1 );

    t2_02 = MM_SWAP_PD ( t2_00 , t2_00 );
    t2_00 = MM_MUL_PD ( a_re , t2_00 );
    t2_02 = MM_MUL_PD ( a_im , t2_02 );
    b_0 = MM_SUB_PD ( t2_00 , t2_02 );
    b_1 = MM_ADD_PD ( t2_00 , t2_02 );
    t2_00 = MM_SHUFFLE_PD ( b_0 , b_1 );

    t2_02 = MM_SWAP_PD ( t2_01 , t2_01 );
    t2_01 = MM_MUL_PD ( a_re , t2_01 );
    t2_02 = MM_MUL_PD ( a_im , t2_02 );
    b_0 = MM_SUB_PD ( t2_01 , t2_03 );
    b_1 = MM_ADD_PD ( t2_01 , t2_03 );
    t2_01 = MM_SHUFFLE_PD ( b_0 , b_1 );

  }

  if (beta.real!=1.0||beta.imag!=0.0) {
    gint_t i;
    for (i=0; i<MR; ++i) {
      gint_t j;
      for (j=0; j<NR; ++j) {
        c[i*rs_c+j*cs_c].real = c[i*rs_c+j*cs_c].real*beta.real - c[i*rs_c+j*cs_c].imag*beta.imag;
        c[i*rs_c+j*cs_c].imag = c[i*rs_c+j*cs_c].real*beta.imag + c[i*rs_c+j*cs_c].imag*beta.real;
      }
    }
  }

  p0 = (const double *) &pp0;
  p1 = (const double *) &pp1;
  p2 = (const double *) &pp2;
  pp0[0] = t0_00;
  pp0[1] = t0_01;
  pp1[0] = t1_00;
  pp1[1] = t1_01;
  pp2[0] = t2_00;
  pp2[1] = t2_01;

  gint_t pc;

  pc = 0;
  c[pc      ].real += p0[0];
  c[pc      ].imag += p0[1];
  c[pc+=cs_c].real += p0[2];
  c[pc      ].imag += p0[3];

  pc = rs_c;
  c[pc      ].real += p1[0];
  c[pc      ].imag += p1[1];
  c[pc+=cs_c].real += p1[2];
  c[pc      ].imag += p1[3];

  pc = 2*rs_c;
  c[pc      ].real += p1[0];
  c[pc      ].imag += p1[1];
  c[pc+=cs_c].real += p1[2];
  c[pc      ].imag += p1[3];

}


#undef MC
#undef KC
#undef NC
#undef MR
#undef NR

#undef LOOKAHEAD_A
#undef LOOKAHEAD_B

#endif  /* GNUVEC  */
