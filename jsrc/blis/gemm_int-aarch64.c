/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* gemm micro kernel                                                       */

#include "../blis.h"

#if BLIS_DRIVER == BLIS_DRIVER_AARCH64

#include <arm_neon.h>

/*
 Note:
 armv8 cacheline size = 64 bytes
*/

#define MC  BLIS_DEFAULT_MC_D
#define KC  BLIS_DEFAULT_KC_D
#define NC  BLIS_DEFAULT_NC_D
#define MR  BLIS_DEFAULT_MR_D
#define NR  BLIS_DEFAULT_NR_D

#if !((MR==6)&&(NR==8))
#error "invalid MR NR"
#endif

#define LOOKAHEAD_A   6*24  // multiple of 24 = 4*MR (24*8 bytes)
#define LOOKAHEAD_B   6*32  // multiple of 32 = 4*NR (32*8 bytes)

void bli_dgemm_opt_6x8
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
  double alpha=*alpha_, beta=*beta_;

  register float64x2_t a_re, a_im;
  register float64x2x2_t b_0,b_1;

  const double *va = (const double *)a;

  register float64x2x2_t t0_00, t0_01, t0_02, t0_03;
  register float64x2x2_t t1_00, t1_01, t1_02, t1_03;
  register float64x2x2_t t2_00, t2_01, t2_02, t2_03;

  t0_00.val[0] = t0_01.val[0] = t0_02.val[0] = t0_03.val[0] = \
  t1_00.val[0] = t1_01.val[0] = t1_02.val[0] = t1_03.val[0] = \
  t2_00.val[0] = t2_01.val[0] = t2_02.val[0] = t2_03.val[0] = \
  t0_00.val[1] = t0_01.val[1] = t0_02.val[1] = t0_03.val[1] = \
  t1_00.val[1] = t1_01.val[1] = t1_02.val[1] = t1_03.val[1] = \
  t2_00.val[1] = t2_01.val[1] = t2_02.val[1] = t2_03.val[1] = \
  vdupq_n_f64(0.0);

  const double *p0,*p1,*p2;
  float64x2x2_t pp0[4],pp1[4],pp2[4];

  PREFETCH2((void*)&c[0*cs_c]);
  PREFETCH2((void*)&c[1*cs_c]);
  PREFETCH2((void*)&c[2*cs_c]);
  PREFETCH2((void*)&c[3*cs_c]);
  PREFETCH2((void*)&c[4*cs_c]);
  PREFETCH2((void*)&c[5*cs_c]);
  PREFETCH2((void*)&c[6*cs_c]);
  PREFETCH2((void*)&c[7*cs_c]);

  PREFETCH2((void*)&c[1*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+3*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+4*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+5*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+6*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+7*cs_c]);

  PREFETCH2((void*)&c[2*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+3*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+4*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+5*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+6*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+7*cs_c]);

  PREFETCH2((void*)&c[3*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[3*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[3*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[3*rs_c+3*cs_c]);
  PREFETCH2((void*)&c[3*rs_c+4*cs_c]);
  PREFETCH2((void*)&c[3*rs_c+5*cs_c]);
  PREFETCH2((void*)&c[3*rs_c+6*cs_c]);
  PREFETCH2((void*)&c[3*rs_c+7*cs_c]);

  PREFETCH2((void*)&c[4*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[4*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[4*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[4*rs_c+3*cs_c]);
  PREFETCH2((void*)&c[4*rs_c+4*cs_c]);
  PREFETCH2((void*)&c[4*rs_c+5*cs_c]);
  PREFETCH2((void*)&c[4*rs_c+6*cs_c]);
  PREFETCH2((void*)&c[4*rs_c+7*cs_c]);

  PREFETCH2((void*)&c[5*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[5*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[5*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[5*rs_c+3*cs_c]);
  PREFETCH2((void*)&c[5*rs_c+4*cs_c]);
  PREFETCH2((void*)&c[5*rs_c+5*cs_c]);
  PREFETCH2((void*)&c[5*rs_c+6*cs_c]);
  PREFETCH2((void*)&c[5*rs_c+7*cs_c]);

  gint_t l;
  gint_t ki = k/4;  // unroll
  gint_t kr = k%4;
  for (l=0; l<ki; ++l) {

// iter 0
    PREFETCH((void*)(b+LOOKAHEAD_B ));  //   PREFETCH((void*)(b+2*4*NR));
    PREFETCH((void*)(va+LOOKAHEAD_A ));

    b_0 = vld2q_f64( (double*)b );
    b_1 = vld2q_f64( 4+(double*)b );

    a_re = vld1q_dup_f64( va );
    a_im = vld1q_dup_f64( va+1 );

    t0_00.val[0] = vfmaq_f64( t0_00.val[0], b_0.val[0], a_re );
    t0_01.val[0] = vfmaq_f64( t0_01.val[0], b_0.val[1], a_re );
    t0_00.val[1] = vfmaq_f64( t0_00.val[1], b_1.val[0], a_re );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], b_1.val[1], a_re );
    t0_02.val[0] = vfmaq_f64( t0_02.val[0], b_0.val[0], a_im );
    t0_03.val[0] = vfmaq_f64( t0_03.val[0], b_0.val[1], a_im );
    t0_02.val[1] = vfmaq_f64( t0_02.val[1], b_1.val[0], a_im );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+2 );
    a_im = vld1q_dup_f64( va+3 );

    t1_00.val[0] = vfmaq_f64( t1_00.val[0], b_0.val[0], a_re );
    t1_01.val[0] = vfmaq_f64( t1_01.val[0], b_0.val[1], a_re );
    t1_00.val[1] = vfmaq_f64( t1_00.val[1], b_1.val[0], a_re );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], b_1.val[1], a_re );
    t1_02.val[0] = vfmaq_f64( t1_02.val[0], b_0.val[0], a_im );
    t1_03.val[0] = vfmaq_f64( t1_03.val[0], b_0.val[1], a_im );
    t1_02.val[1] = vfmaq_f64( t1_02.val[1], b_1.val[0], a_im );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+4 );
    a_im = vld1q_dup_f64( va+5 );

    t2_00.val[0] = vfmaq_f64( t2_00.val[0], b_0.val[0], a_re );
    t2_01.val[0] = vfmaq_f64( t2_01.val[0], b_0.val[1], a_re );
    t2_00.val[1] = vfmaq_f64( t2_00.val[1], b_1.val[0], a_re );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], b_1.val[1], a_re );
    t2_02.val[0] = vfmaq_f64( t2_02.val[0], b_0.val[0], a_im );
    t2_03.val[0] = vfmaq_f64( t2_03.val[0], b_0.val[1], a_im );
    t2_02.val[1] = vfmaq_f64( t2_02.val[1], b_1.val[0], a_im );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], b_1.val[1], a_im );
    CCBLOCK;

// iter 1
    PREFETCH((void*)(b+LOOKAHEAD_B +8));
    PREFETCH((void*)(va+LOOKAHEAD_A +8));

    b_0 = vld2q_f64( 8+(double*)b );
    b_1 = vld2q_f64( 12+(double*)b );

    a_re = vld1q_dup_f64( va+6 );
    a_im = vld1q_dup_f64( va+7 );

    t0_00.val[0] = vfmaq_f64( t0_00.val[0], b_0.val[0], a_re );
    t0_01.val[0] = vfmaq_f64( t0_01.val[0], b_0.val[1], a_re );
    t0_00.val[1] = vfmaq_f64( t0_00.val[1], b_1.val[0], a_re );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], b_1.val[1], a_re );
    t0_02.val[0] = vfmaq_f64( t0_02.val[0], b_0.val[0], a_im );
    t0_03.val[0] = vfmaq_f64( t0_03.val[0], b_0.val[1], a_im );
    t0_02.val[1] = vfmaq_f64( t0_02.val[1], b_1.val[0], a_im );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+8 );
    a_im = vld1q_dup_f64( va+9 );

    t1_00.val[0] = vfmaq_f64( t1_00.val[0], b_0.val[0], a_re );
    t1_01.val[0] = vfmaq_f64( t1_01.val[0], b_0.val[1], a_re );
    t1_00.val[1] = vfmaq_f64( t1_00.val[1], b_1.val[0], a_re );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], b_1.val[1], a_re );
    t1_02.val[0] = vfmaq_f64( t1_02.val[0], b_0.val[0], a_im );
    t1_03.val[0] = vfmaq_f64( t1_03.val[0], b_0.val[1], a_im );
    t1_02.val[1] = vfmaq_f64( t1_02.val[1], b_1.val[0], a_im );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+10 );
    a_im = vld1q_dup_f64( va+11 );

    t2_00.val[0] = vfmaq_f64( t2_00.val[0], b_0.val[0], a_re );
    t2_01.val[0] = vfmaq_f64( t2_01.val[0], b_0.val[1], a_re );
    t2_00.val[1] = vfmaq_f64( t2_00.val[1], b_1.val[0], a_re );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], b_1.val[1], a_re );
    t2_02.val[0] = vfmaq_f64( t2_02.val[0], b_0.val[0], a_im );
    t2_03.val[0] = vfmaq_f64( t2_03.val[0], b_0.val[1], a_im );
    t2_02.val[1] = vfmaq_f64( t2_02.val[1], b_1.val[0], a_im );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], b_1.val[1], a_im );
    CCBLOCK;

// iter 2
    PREFETCH((void*)(b+LOOKAHEAD_B +16));
    PREFETCH((void*)(va+LOOKAHEAD_A +16));

    b_0 = vld2q_f64( 16+(double*)b );
    b_1 = vld2q_f64( 20+(double*)b );

    a_re = vld1q_dup_f64( va+12 );
    a_im = vld1q_dup_f64( va+13 );

    t0_00.val[0] = vfmaq_f64( t0_00.val[0], b_0.val[0], a_re );
    t0_01.val[0] = vfmaq_f64( t0_01.val[0], b_0.val[1], a_re );
    t0_00.val[1] = vfmaq_f64( t0_00.val[1], b_1.val[0], a_re );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], b_1.val[1], a_re );
    t0_02.val[0] = vfmaq_f64( t0_02.val[0], b_0.val[0], a_im );
    t0_03.val[0] = vfmaq_f64( t0_03.val[0], b_0.val[1], a_im );
    t0_02.val[1] = vfmaq_f64( t0_02.val[1], b_1.val[0], a_im );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+14 );
    a_im = vld1q_dup_f64( va+15 );

    t1_00.val[0] = vfmaq_f64( t1_00.val[0], b_0.val[0], a_re );
    t1_01.val[0] = vfmaq_f64( t1_01.val[0], b_0.val[1], a_re );
    t1_00.val[1] = vfmaq_f64( t1_00.val[1], b_1.val[0], a_re );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], b_1.val[1], a_re );
    t1_02.val[0] = vfmaq_f64( t1_02.val[0], b_0.val[0], a_im );
    t1_03.val[0] = vfmaq_f64( t1_03.val[0], b_0.val[1], a_im );
    t1_02.val[1] = vfmaq_f64( t1_02.val[1], b_1.val[0], a_im );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+16 );
    a_im = vld1q_dup_f64( va+17 );

    t2_00.val[0] = vfmaq_f64( t2_00.val[0], b_0.val[0], a_re );
    t2_01.val[0] = vfmaq_f64( t2_01.val[0], b_0.val[1], a_re );
    t2_00.val[1] = vfmaq_f64( t2_00.val[1], b_1.val[0], a_re );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], b_1.val[1], a_re );
    t2_02.val[0] = vfmaq_f64( t2_02.val[0], b_0.val[0], a_im );
    t2_03.val[0] = vfmaq_f64( t2_03.val[0], b_0.val[1], a_im );
    t2_02.val[1] = vfmaq_f64( t2_02.val[1], b_1.val[0], a_im );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], b_1.val[1], a_im );
    CCBLOCK;

// iter 3
    PREFETCH((void*)(b+LOOKAHEAD_B +24));

    b_0 = vld2q_f64( 24+(double*)b );
    b_1 = vld2q_f64( 28+(double*)b );

    a_re = vld1q_dup_f64( va+18 );
    a_im = vld1q_dup_f64( va+19 );

    t0_00.val[0] = vfmaq_f64( t0_00.val[0], b_0.val[0], a_re );
    t0_01.val[0] = vfmaq_f64( t0_01.val[0], b_0.val[1], a_re );
    t0_00.val[1] = vfmaq_f64( t0_00.val[1], b_1.val[0], a_re );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], b_1.val[1], a_re );
    t0_02.val[0] = vfmaq_f64( t0_02.val[0], b_0.val[0], a_im );
    t0_03.val[0] = vfmaq_f64( t0_03.val[0], b_0.val[1], a_im );
    t0_02.val[1] = vfmaq_f64( t0_02.val[1], b_1.val[0], a_im );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+20 );
    a_im = vld1q_dup_f64( va+21 );

    t1_00.val[0] = vfmaq_f64( t1_00.val[0], b_0.val[0], a_re );
    t1_01.val[0] = vfmaq_f64( t1_01.val[0], b_0.val[1], a_re );
    t1_00.val[1] = vfmaq_f64( t1_00.val[1], b_1.val[0], a_re );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], b_1.val[1], a_re );
    t1_02.val[0] = vfmaq_f64( t1_02.val[0], b_0.val[0], a_im );
    t1_03.val[0] = vfmaq_f64( t1_03.val[0], b_0.val[1], a_im );
    t1_02.val[1] = vfmaq_f64( t1_02.val[1], b_1.val[0], a_im );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+22 );
    a_im = vld1q_dup_f64( va+23 );

    t2_00.val[0] = vfmaq_f64( t2_00.val[0], b_0.val[0], a_re );
    t2_01.val[0] = vfmaq_f64( t2_01.val[0], b_0.val[1], a_re );
    t2_00.val[1] = vfmaq_f64( t2_00.val[1], b_1.val[0], a_re );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], b_1.val[1], a_re );
    t2_02.val[0] = vfmaq_f64( t2_02.val[0], b_0.val[0], a_im );
    t2_03.val[0] = vfmaq_f64( t2_03.val[0], b_0.val[1], a_im );
    t2_02.val[1] = vfmaq_f64( t2_02.val[1], b_1.val[0], a_im );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], b_1.val[1], a_im );
    CCBLOCK;

    b += 4*NR;      // unroll*NR
    va += 4*MR;     // unroll*MR  va is pointer to double
  }

  for (l=0; l<kr; ++l) {

// look ahead too small
//    PREFETCH((void*)(b+16));   // PREFETCH((void*)(b+2*NR));
//    PREFETCH((void*)(va+12));

    b_0 = vld2q_f64( (double*)b );
    b_1 = vld2q_f64( 4+(double*)b );

    a_re = vld1q_dup_f64( va );
    a_im = vld1q_dup_f64( va+1 );

    t0_00.val[0] = vfmaq_f64( t0_00.val[0], b_0.val[0], a_re );
    t0_01.val[0] = vfmaq_f64( t0_01.val[0], b_0.val[1], a_re );
    t0_00.val[1] = vfmaq_f64( t0_00.val[1], b_1.val[0], a_re );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], b_1.val[1], a_re );
    t0_02.val[0] = vfmaq_f64( t0_02.val[0], b_0.val[0], a_im );
    t0_03.val[0] = vfmaq_f64( t0_03.val[0], b_0.val[1], a_im );
    t0_02.val[1] = vfmaq_f64( t0_02.val[1], b_1.val[0], a_im );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+2 );
    a_im = vld1q_dup_f64( va+3 );

    t1_00.val[0] = vfmaq_f64( t1_00.val[0], b_0.val[0], a_re );
    t1_01.val[0] = vfmaq_f64( t1_01.val[0], b_0.val[1], a_re );
    t1_00.val[1] = vfmaq_f64( t1_00.val[1], b_1.val[0], a_re );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], b_1.val[1], a_re );
    t1_02.val[0] = vfmaq_f64( t1_02.val[0], b_0.val[0], a_im );
    t1_03.val[0] = vfmaq_f64( t1_03.val[0], b_0.val[1], a_im );
    t1_02.val[1] = vfmaq_f64( t1_02.val[1], b_1.val[0], a_im );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+4 );
    a_im = vld1q_dup_f64( va+5 );

    t2_00.val[0] = vfmaq_f64( t2_00.val[0], b_0.val[0], a_re );
    t2_01.val[0] = vfmaq_f64( t2_01.val[0], b_0.val[1], a_re );
    t2_00.val[1] = vfmaq_f64( t2_00.val[1], b_1.val[0], a_re );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], b_1.val[1], a_re );
    t2_02.val[0] = vfmaq_f64( t2_02.val[0], b_0.val[0], a_im );
    t2_03.val[0] = vfmaq_f64( t2_03.val[0], b_0.val[1], a_im );
    t2_02.val[1] = vfmaq_f64( t2_02.val[1], b_1.val[0], a_im );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], b_1.val[1], a_im );
    CCBLOCK;

    b += NR;
    va += MR;       // va is pointer to double
  }

  if (alpha!=1.0) {
    a_re = vld1q_dup_f64( (double*)alpha_ );

    t0_00.val[0] = vmulq_f64 ( t0_00.val[0] , a_re );
    t0_01.val[0] = vmulq_f64 ( t0_01.val[0] , a_re );
    t0_02.val[0] = vmulq_f64 ( t0_02.val[0] , a_re );
    t0_03.val[0] = vmulq_f64 ( t0_03.val[0] , a_re );

    t0_00.val[1] = vmulq_f64 ( t0_00.val[1] , a_re );
    t0_01.val[1] = vmulq_f64 ( t0_01.val[1] , a_re );
    t0_02.val[1] = vmulq_f64 ( t0_02.val[1] , a_re );
    t0_03.val[1] = vmulq_f64 ( t0_03.val[1] , a_re );

    t1_00.val[0] = vmulq_f64 ( t1_00.val[0] , a_re );
    t1_01.val[0] = vmulq_f64 ( t1_01.val[0] , a_re );
    t1_02.val[0] = vmulq_f64 ( t1_02.val[0] , a_re );
    t1_03.val[0] = vmulq_f64 ( t1_03.val[0] , a_re );

    t1_00.val[1] = vmulq_f64 ( t1_00.val[1] , a_re );
    t1_01.val[1] = vmulq_f64 ( t1_01.val[1] , a_re );
    t1_02.val[1] = vmulq_f64 ( t1_02.val[1] , a_re );
    t1_03.val[1] = vmulq_f64 ( t1_03.val[1] , a_re );

    t2_00.val[0] = vmulq_f64 ( t2_00.val[0] , a_re );
    t2_01.val[0] = vmulq_f64 ( t2_01.val[0] , a_re );
    t2_02.val[0] = vmulq_f64 ( t2_02.val[0] , a_re );
    t2_03.val[0] = vmulq_f64 ( t2_03.val[0] , a_re );

    t2_00.val[1] = vmulq_f64 ( t2_00.val[1] , a_re );
    t2_01.val[1] = vmulq_f64 ( t2_01.val[1] , a_re );
    t2_02.val[1] = vmulq_f64 ( t2_02.val[1] , a_re );
    t2_03.val[1] = vmulq_f64 ( t2_03.val[1] , a_re );

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
  c[pc+=cs_c] += p0[4];
  c[pc+=cs_c] += p0[1];
  c[pc+=cs_c] += p0[5];
  c[pc+=cs_c] += p0[2];
  c[pc+=cs_c] += p0[6];
  c[pc+=cs_c] += p0[3];
  c[pc+=cs_c] += p0[7];

  pc = rs_c;
  c[pc      ] += p0[8];
  c[pc+=cs_c] += p0[12];
  c[pc+=cs_c] += p0[9];
  c[pc+=cs_c] += p0[13];
  c[pc+=cs_c] += p0[10];
  c[pc+=cs_c] += p0[14];
  c[pc+=cs_c] += p0[11];
  c[pc+=cs_c] += p0[15];

  pc = 2*rs_c;
  c[pc      ] += p1[0];
  c[pc+=cs_c] += p1[4];
  c[pc+=cs_c] += p1[1];
  c[pc+=cs_c] += p1[5];
  c[pc+=cs_c] += p1[2];
  c[pc+=cs_c] += p1[6];
  c[pc+=cs_c] += p1[3];
  c[pc+=cs_c] += p1[7];

  pc = 3*rs_c;
  c[pc      ] += p1[8];
  c[pc+=cs_c] += p1[12];
  c[pc+=cs_c] += p1[9];
  c[pc+=cs_c] += p1[13];
  c[pc+=cs_c] += p1[10];
  c[pc+=cs_c] += p1[14];
  c[pc+=cs_c] += p1[11];
  c[pc+=cs_c] += p1[15];

  pc = 4*rs_c;
  c[pc      ] += p2[0];
  c[pc+=cs_c] += p2[4];
  c[pc+=cs_c] += p2[1];
  c[pc+=cs_c] += p2[5];
  c[pc+=cs_c] += p2[2];
  c[pc+=cs_c] += p2[6];
  c[pc+=cs_c] += p2[3];
  c[pc+=cs_c] += p2[7];

  pc = 5*rs_c;
  c[pc      ] += p2[8];
  c[pc+=cs_c] += p2[12];
  c[pc+=cs_c] += p2[9];
  c[pc+=cs_c] += p2[13];
  c[pc+=cs_c] += p2[10];
  c[pc+=cs_c] += p2[14];
  c[pc+=cs_c] += p2[11];
  c[pc+=cs_c] += p2[15];

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


#if !((MR==3)&&(NR==4))
#error "invalid MR NR"
#endif

#define LOOKAHEAD_A   3*24 // multiple of 24 = 4*MR*2 (24*8 bytes)
#define LOOKAHEAD_B   3*16 // multiple of 16 = 4*NR   (16*16 bytes)

void bli_zgemm_opt_3x4
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

  register float64x2_t a_re, a_im;
  register float64x2x2_t b_0,b_1;

  const double *va = (const double *)a;

  register float64x2x2_t t0_00, t0_01, t0_02, t0_03;
  register float64x2x2_t t1_00, t1_01, t1_02, t1_03;
  register float64x2x2_t t2_00, t2_01, t2_02, t2_03;

  t0_00.val[0] = t0_01.val[0] = t0_02.val[0] = t0_03.val[0] = \
  t1_00.val[0] = t1_01.val[0] = t1_02.val[0] = t1_03.val[0] = \
  t2_00.val[0] = t2_01.val[0] = t2_02.val[0] = t2_03.val[0] = \
  t0_00.val[1] = t0_01.val[1] = t0_02.val[1] = t0_03.val[1] = \
  t1_00.val[1] = t1_01.val[1] = t1_02.val[1] = t1_03.val[1] = \
  t2_00.val[1] = t2_01.val[1] = t2_02.val[1] = t2_03.val[1] = \
  vdupq_n_f64(0.0);

  const double *p0,*p1,*p2;
  float64x2x2_t pp0[4],pp1[4],pp2[4];

  PREFETCH2((void*)&c[0*cs_c]);
  PREFETCH2((void*)&c[1*cs_c]);
  PREFETCH2((void*)&c[2*cs_c]);
  PREFETCH2((void*)&c[3*cs_c]);

  PREFETCH2((void*)&c[1*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+3*cs_c]);

  PREFETCH2((void*)&c[2*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+3*cs_c]);

  gint_t l;
  gint_t ki = k/4;
  gint_t kr = k%4;
  for (l=0; l<ki; ++l) {

// iter 0
    PREFETCH((void*)(b+LOOKAHEAD_B )); // PREFETCH((void*)(b+2*4*NR));
    PREFETCH((void*)(va+LOOKAHEAD_A ));

    b_0 = vld2q_f64( (double*)b );
    b_1 = vld2q_f64( 4+(double*)b );

    a_re = vld1q_dup_f64( va );
    a_im = vld1q_dup_f64( va+1 );

    t0_00.val[0] = vfmaq_f64( t0_00.val[0], b_0.val[0], a_re );
    t0_00.val[1] = vfmaq_f64( t0_00.val[1], b_0.val[1], a_re );
    t0_01.val[0] = vfmaq_f64( t0_01.val[0], b_1.val[0], a_re );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], b_1.val[1], a_re );

    t0_02.val[0] = vfmaq_f64( t0_02.val[0], b_0.val[0], a_re );
    t0_02.val[1] = vfmaq_f64( t0_02.val[1], b_0.val[1], a_re );
    t0_03.val[0] = vfmaq_f64( t0_03.val[0], b_1.val[0], a_re );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], b_1.val[1], a_re );

    t0_00.val[1] = vfmaq_f64( t0_00.val[1], b_0.val[0], a_im );
    t0_00.val[0] = vfmsq_f64( t0_00.val[0], b_0.val[1], a_im );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], b_1.val[0], a_im );
    t0_01.val[0] = vfmsq_f64( t0_01.val[0], b_1.val[1], a_im );

    t0_02.val[1] = vfmaq_f64( t0_02.val[1], b_0.val[0], a_im );
    t0_02.val[0] = vfmsq_f64( t0_02.val[0], b_0.val[1], a_im );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], b_1.val[0], a_im );
    t0_03.val[0] = vfmsq_f64( t0_03.val[0], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+2 );
    a_im = vld1q_dup_f64( va+3 );

    t1_00.val[0] = vfmaq_f64( t1_00.val[0], b_0.val[0], a_re );
    t1_00.val[1] = vfmaq_f64( t1_00.val[1], b_0.val[1], a_re );
    t1_01.val[0] = vfmaq_f64( t1_01.val[0], b_1.val[0], a_re );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], b_1.val[1], a_re );

    t1_02.val[0] = vfmaq_f64( t1_02.val[0], b_0.val[0], a_re );
    t1_02.val[1] = vfmaq_f64( t1_02.val[1], b_0.val[1], a_re );
    t1_03.val[0] = vfmaq_f64( t1_03.val[0], b_1.val[0], a_re );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], b_1.val[1], a_re );

    t1_00.val[1] = vfmaq_f64( t1_00.val[1], b_0.val[0], a_im );
    t1_00.val[0] = vfmsq_f64( t1_00.val[0], b_0.val[1], a_im );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], b_1.val[0], a_im );
    t1_01.val[0] = vfmsq_f64( t1_01.val[0], b_1.val[1], a_im );

    t1_02.val[1] = vfmaq_f64( t1_02.val[1], b_0.val[0], a_im );
    t1_02.val[0] = vfmsq_f64( t1_02.val[0], b_0.val[1], a_im );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], b_1.val[0], a_im );
    t1_03.val[0] = vfmsq_f64( t1_03.val[0], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+4 );
    a_im = vld1q_dup_f64( va+5 );

    t2_00.val[0] = vfmaq_f64( t2_00.val[0], b_0.val[0], a_re );
    t2_00.val[1] = vfmaq_f64( t2_00.val[1], b_0.val[1], a_re );
    t2_01.val[0] = vfmaq_f64( t2_01.val[0], b_1.val[0], a_re );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], b_1.val[1], a_re );

    t2_02.val[0] = vfmaq_f64( t2_02.val[0], b_0.val[0], a_re );
    t2_02.val[1] = vfmaq_f64( t2_02.val[1], b_0.val[1], a_re );
    t2_03.val[0] = vfmaq_f64( t2_03.val[0], b_1.val[0], a_re );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], b_1.val[1], a_re );

    t2_00.val[1] = vfmaq_f64( t2_00.val[1], b_0.val[0], a_im );
    t2_00.val[0] = vfmsq_f64( t2_00.val[0], b_0.val[1], a_im );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], b_1.val[0], a_im );
    t2_01.val[0] = vfmsq_f64( t2_01.val[0], b_1.val[1], a_im );

    t2_02.val[1] = vfmaq_f64( t2_02.val[1], b_0.val[0], a_im );
    t2_02.val[0] = vfmsq_f64( t2_02.val[0], b_0.val[1], a_im );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], b_1.val[0], a_im );
    t2_03.val[0] = vfmsq_f64( t2_03.val[0], b_1.val[1], a_im );
    CCBLOCK;

// iter 1
    PREFETCH((void*)(b+LOOKAHEAD_B +4));
    PREFETCH((void*)(va+LOOKAHEAD_A +8));

    b_0 = vld2q_f64( 8+(double*)b );
    b_1 = vld2q_f64( 12+(double*)b );

    a_re = vld1q_dup_f64( va+6 );
    a_im = vld1q_dup_f64( va+7 );

    t0_00.val[0] = vfmaq_f64( t0_00.val[0], b_0.val[0], a_re );
    t0_00.val[1] = vfmaq_f64( t0_00.val[1], b_0.val[1], a_re );
    t0_01.val[0] = vfmaq_f64( t0_01.val[0], b_1.val[0], a_re );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], b_1.val[1], a_re );

    t0_02.val[0] = vfmaq_f64( t0_02.val[0], b_0.val[0], a_re );
    t0_02.val[1] = vfmaq_f64( t0_02.val[1], b_0.val[1], a_re );
    t0_03.val[0] = vfmaq_f64( t0_03.val[0], b_1.val[0], a_re );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], b_1.val[1], a_re );

    t0_00.val[1] = vfmaq_f64( t0_00.val[1], b_0.val[0], a_im );
    t0_00.val[0] = vfmsq_f64( t0_00.val[0], b_0.val[1], a_im );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], b_1.val[0], a_im );
    t0_01.val[0] = vfmsq_f64( t0_01.val[0], b_1.val[1], a_im );

    t0_02.val[1] = vfmaq_f64( t0_02.val[1], b_0.val[0], a_im );
    t0_02.val[0] = vfmsq_f64( t0_02.val[0], b_0.val[1], a_im );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], b_1.val[0], a_im );
    t0_03.val[0] = vfmsq_f64( t0_03.val[0], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+8 );
    a_im = vld1q_dup_f64( va+9 );

    t1_00.val[0] = vfmaq_f64( t1_00.val[0], b_0.val[0], a_re );
    t1_00.val[1] = vfmaq_f64( t1_00.val[1], b_0.val[1], a_re );
    t1_01.val[0] = vfmaq_f64( t1_01.val[0], b_1.val[0], a_re );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], b_1.val[1], a_re );

    t1_02.val[0] = vfmaq_f64( t1_02.val[0], b_0.val[0], a_re );
    t1_02.val[1] = vfmaq_f64( t1_02.val[1], b_0.val[1], a_re );
    t1_03.val[0] = vfmaq_f64( t1_03.val[0], b_1.val[0], a_re );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], b_1.val[1], a_re );

    t1_00.val[1] = vfmaq_f64( t1_00.val[1], b_0.val[0], a_im );
    t1_00.val[0] = vfmsq_f64( t1_00.val[0], b_0.val[1], a_im );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], b_1.val[0], a_im );
    t1_01.val[0] = vfmsq_f64( t1_01.val[0], b_1.val[1], a_im );

    t1_02.val[1] = vfmaq_f64( t1_02.val[1], b_0.val[0], a_im );
    t1_02.val[0] = vfmsq_f64( t1_02.val[0], b_0.val[1], a_im );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], b_1.val[0], a_im );
    t1_03.val[0] = vfmsq_f64( t1_03.val[0], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+10 );
    a_im = vld1q_dup_f64( va+11 );

    t2_00.val[0] = vfmaq_f64( t2_00.val[0], b_0.val[0], a_re );
    t2_00.val[1] = vfmaq_f64( t2_00.val[1], b_0.val[1], a_re );
    t2_01.val[0] = vfmaq_f64( t2_01.val[0], b_1.val[0], a_re );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], b_1.val[1], a_re );

    t2_02.val[0] = vfmaq_f64( t2_02.val[0], b_0.val[0], a_re );
    t2_02.val[1] = vfmaq_f64( t2_02.val[1], b_0.val[1], a_re );
    t2_03.val[0] = vfmaq_f64( t2_03.val[0], b_1.val[0], a_re );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], b_1.val[1], a_re );

    t2_00.val[1] = vfmaq_f64( t2_00.val[1], b_0.val[0], a_im );
    t2_00.val[0] = vfmsq_f64( t2_00.val[0], b_0.val[1], a_im );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], b_1.val[0], a_im );
    t2_01.val[0] = vfmsq_f64( t2_01.val[0], b_1.val[1], a_im );

    t2_02.val[1] = vfmaq_f64( t2_02.val[1], b_0.val[0], a_im );
    t2_02.val[0] = vfmsq_f64( t2_02.val[0], b_0.val[1], a_im );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], b_1.val[0], a_im );
    t2_03.val[0] = vfmsq_f64( t2_03.val[0], b_1.val[1], a_im );
    CCBLOCK;

// iter 2
    PREFETCH((void*)(b+LOOKAHEAD_B +8));
    PREFETCH((void*)(va+LOOKAHEAD_A +16));

    b_0 = vld2q_f64( 16+(double*)b );
    b_1 = vld2q_f64( 20+(double*)b );

    a_re = vld1q_dup_f64( va+12 );
    a_im = vld1q_dup_f64( va+13 );

    t0_00.val[0] = vfmaq_f64( t0_00.val[0], b_0.val[0], a_re );
    t0_00.val[1] = vfmaq_f64( t0_00.val[1], b_0.val[1], a_re );
    t0_01.val[0] = vfmaq_f64( t0_01.val[0], b_1.val[0], a_re );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], b_1.val[1], a_re );

    t0_02.val[0] = vfmaq_f64( t0_02.val[0], b_0.val[0], a_re );
    t0_02.val[1] = vfmaq_f64( t0_02.val[1], b_0.val[1], a_re );
    t0_03.val[0] = vfmaq_f64( t0_03.val[0], b_1.val[0], a_re );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], b_1.val[1], a_re );

    t0_00.val[1] = vfmaq_f64( t0_00.val[1], b_0.val[0], a_im );
    t0_00.val[0] = vfmsq_f64( t0_00.val[0], b_0.val[1], a_im );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], b_1.val[0], a_im );
    t0_01.val[0] = vfmsq_f64( t0_01.val[0], b_1.val[1], a_im );

    t0_02.val[1] = vfmaq_f64( t0_02.val[1], b_0.val[0], a_im );
    t0_02.val[0] = vfmsq_f64( t0_02.val[0], b_0.val[1], a_im );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], b_1.val[0], a_im );
    t0_03.val[0] = vfmsq_f64( t0_03.val[0], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+14 );
    a_im = vld1q_dup_f64( va+15 );

    t1_00.val[0] = vfmaq_f64( t1_00.val[0], b_0.val[0], a_re );
    t1_00.val[1] = vfmaq_f64( t1_00.val[1], b_0.val[1], a_re );
    t1_01.val[0] = vfmaq_f64( t1_01.val[0], b_1.val[0], a_re );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], b_1.val[1], a_re );

    t1_02.val[0] = vfmaq_f64( t1_02.val[0], b_0.val[0], a_re );
    t1_02.val[1] = vfmaq_f64( t1_02.val[1], b_0.val[1], a_re );
    t1_03.val[0] = vfmaq_f64( t1_03.val[0], b_1.val[0], a_re );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], b_1.val[1], a_re );

    t1_00.val[1] = vfmaq_f64( t1_00.val[1], b_0.val[0], a_im );
    t1_00.val[0] = vfmsq_f64( t1_00.val[0], b_0.val[1], a_im );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], b_1.val[0], a_im );
    t1_01.val[0] = vfmsq_f64( t1_01.val[0], b_1.val[1], a_im );

    t1_02.val[1] = vfmaq_f64( t1_02.val[1], b_0.val[0], a_im );
    t1_02.val[0] = vfmsq_f64( t1_02.val[0], b_0.val[1], a_im );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], b_1.val[0], a_im );
    t1_03.val[0] = vfmsq_f64( t1_03.val[0], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+16 );
    a_im = vld1q_dup_f64( va+17 );

    t2_00.val[0] = vfmaq_f64( t2_00.val[0], b_0.val[0], a_re );
    t2_00.val[1] = vfmaq_f64( t2_00.val[1], b_0.val[1], a_re );
    t2_01.val[0] = vfmaq_f64( t2_01.val[0], b_1.val[0], a_re );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], b_1.val[1], a_re );

    t2_02.val[0] = vfmaq_f64( t2_02.val[0], b_0.val[0], a_re );
    t2_02.val[1] = vfmaq_f64( t2_02.val[1], b_0.val[1], a_re );
    t2_03.val[0] = vfmaq_f64( t2_03.val[0], b_1.val[0], a_re );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], b_1.val[1], a_re );

    t2_00.val[1] = vfmaq_f64( t2_00.val[1], b_0.val[0], a_im );
    t2_00.val[0] = vfmsq_f64( t2_00.val[0], b_0.val[1], a_im );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], b_1.val[0], a_im );
    t2_01.val[0] = vfmsq_f64( t2_01.val[0], b_1.val[1], a_im );

    t2_02.val[1] = vfmaq_f64( t2_02.val[1], b_0.val[0], a_im );
    t2_02.val[0] = vfmsq_f64( t2_02.val[0], b_0.val[1], a_im );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], b_1.val[0], a_im );
    t2_03.val[0] = vfmsq_f64( t2_03.val[0], b_1.val[1], a_im );
    CCBLOCK;

// iter 3
    PREFETCH((void*)(b+LOOKAHEAD_B +12));

    b_0 = vld2q_f64( 24+(double*)b );
    b_1 = vld2q_f64( 28+(double*)b );

    a_re = vld1q_dup_f64( va+18 );
    a_im = vld1q_dup_f64( va+19 );

    t0_00.val[0] = vfmaq_f64( t0_00.val[0], b_0.val[0], a_re );
    t0_00.val[1] = vfmaq_f64( t0_00.val[1], b_0.val[1], a_re );
    t0_01.val[0] = vfmaq_f64( t0_01.val[0], b_1.val[0], a_re );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], b_1.val[1], a_re );

    t0_02.val[0] = vfmaq_f64( t0_02.val[0], b_0.val[0], a_re );
    t0_02.val[1] = vfmaq_f64( t0_02.val[1], b_0.val[1], a_re );
    t0_03.val[0] = vfmaq_f64( t0_03.val[0], b_1.val[0], a_re );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], b_1.val[1], a_re );

    t0_00.val[1] = vfmaq_f64( t0_00.val[1], b_0.val[0], a_im );
    t0_00.val[0] = vfmsq_f64( t0_00.val[0], b_0.val[1], a_im );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], b_1.val[0], a_im );
    t0_01.val[0] = vfmsq_f64( t0_01.val[0], b_1.val[1], a_im );

    t0_02.val[1] = vfmaq_f64( t0_02.val[1], b_0.val[0], a_im );
    t0_02.val[0] = vfmsq_f64( t0_02.val[0], b_0.val[1], a_im );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], b_1.val[0], a_im );
    t0_03.val[0] = vfmsq_f64( t0_03.val[0], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+20 );
    a_im = vld1q_dup_f64( va+21 );

    t1_00.val[0] = vfmaq_f64( t1_00.val[0], b_0.val[0], a_re );
    t1_00.val[1] = vfmaq_f64( t1_00.val[1], b_0.val[1], a_re );
    t1_01.val[0] = vfmaq_f64( t1_01.val[0], b_1.val[0], a_re );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], b_1.val[1], a_re );

    t1_02.val[0] = vfmaq_f64( t1_02.val[0], b_0.val[0], a_re );
    t1_02.val[1] = vfmaq_f64( t1_02.val[1], b_0.val[1], a_re );
    t1_03.val[0] = vfmaq_f64( t1_03.val[0], b_1.val[0], a_re );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], b_1.val[1], a_re );

    t1_00.val[1] = vfmaq_f64( t1_00.val[1], b_0.val[0], a_im );
    t1_00.val[0] = vfmsq_f64( t1_00.val[0], b_0.val[1], a_im );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], b_1.val[0], a_im );
    t1_01.val[0] = vfmsq_f64( t1_01.val[0], b_1.val[1], a_im );

    t1_02.val[1] = vfmaq_f64( t1_02.val[1], b_0.val[0], a_im );
    t1_02.val[0] = vfmsq_f64( t1_02.val[0], b_0.val[1], a_im );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], b_1.val[0], a_im );
    t1_03.val[0] = vfmsq_f64( t1_03.val[0], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+22 );
    a_im = vld1q_dup_f64( va+23 );

    t2_00.val[0] = vfmaq_f64( t2_00.val[0], b_0.val[0], a_re );
    t2_00.val[1] = vfmaq_f64( t2_00.val[1], b_0.val[1], a_re );
    t2_01.val[0] = vfmaq_f64( t2_01.val[0], b_1.val[0], a_re );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], b_1.val[1], a_re );

    t2_02.val[0] = vfmaq_f64( t2_02.val[0], b_0.val[0], a_re );
    t2_02.val[1] = vfmaq_f64( t2_02.val[1], b_0.val[1], a_re );
    t2_03.val[0] = vfmaq_f64( t2_03.val[0], b_1.val[0], a_re );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], b_1.val[1], a_re );

    t2_00.val[1] = vfmaq_f64( t2_00.val[1], b_0.val[0], a_im );
    t2_00.val[0] = vfmsq_f64( t2_00.val[0], b_0.val[1], a_im );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], b_1.val[0], a_im );
    t2_01.val[0] = vfmsq_f64( t2_01.val[0], b_1.val[1], a_im );

    t2_02.val[1] = vfmaq_f64( t2_02.val[1], b_0.val[0], a_im );
    t2_02.val[0] = vfmsq_f64( t2_02.val[0], b_0.val[1], a_im );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], b_1.val[0], a_im );
    t2_03.val[0] = vfmsq_f64( t2_03.val[0], b_1.val[1], a_im );
    CCBLOCK;

    b += 4*NR;        // unroll*NR
    va += 4*MR*2;     // unroll*MR  va is pointer to double
  }

  for (l=0; l<kr; ++l) {

// look ahead too small
//    PREFETCH((void*)(b+8)); // PREFETCH((void*)(b+2*NR));
//    PREFETCH((void*)(va+12));

    b_0 = vld2q_f64( (double*)b );
    b_1 = vld2q_f64( 4+(double*)b );

    a_re = vld1q_dup_f64( va );
    a_im = vld1q_dup_f64( va+1 );

    t0_00.val[0] = vfmaq_f64( t0_00.val[0], b_0.val[0], a_re );
    t0_00.val[1] = vfmaq_f64( t0_00.val[1], b_0.val[1], a_re );
    t0_01.val[0] = vfmaq_f64( t0_01.val[0], b_1.val[0], a_re );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], b_1.val[1], a_re );

    t0_02.val[0] = vfmaq_f64( t0_02.val[0], b_0.val[0], a_re );
    t0_02.val[1] = vfmaq_f64( t0_02.val[1], b_0.val[1], a_re );
    t0_03.val[0] = vfmaq_f64( t0_03.val[0], b_1.val[0], a_re );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], b_1.val[1], a_re );

    t0_00.val[1] = vfmaq_f64( t0_00.val[1], b_0.val[0], a_im );
    t0_00.val[0] = vfmsq_f64( t0_00.val[0], b_0.val[1], a_im );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], b_1.val[0], a_im );
    t0_01.val[0] = vfmsq_f64( t0_01.val[0], b_1.val[1], a_im );

    t0_02.val[1] = vfmaq_f64( t0_02.val[1], b_0.val[0], a_im );
    t0_02.val[0] = vfmsq_f64( t0_02.val[0], b_0.val[1], a_im );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], b_1.val[0], a_im );
    t0_03.val[0] = vfmsq_f64( t0_03.val[0], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+2 );
    a_im = vld1q_dup_f64( va+3 );

    t1_00.val[0] = vfmaq_f64( t1_00.val[0], b_0.val[0], a_re );
    t1_00.val[1] = vfmaq_f64( t1_00.val[1], b_0.val[1], a_re );
    t1_01.val[0] = vfmaq_f64( t1_01.val[0], b_1.val[0], a_re );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], b_1.val[1], a_re );

    t1_02.val[0] = vfmaq_f64( t1_02.val[0], b_0.val[0], a_re );
    t1_02.val[1] = vfmaq_f64( t1_02.val[1], b_0.val[1], a_re );
    t1_03.val[0] = vfmaq_f64( t1_03.val[0], b_1.val[0], a_re );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], b_1.val[1], a_re );

    t1_00.val[1] = vfmaq_f64( t1_00.val[1], b_0.val[0], a_im );
    t1_00.val[0] = vfmsq_f64( t1_00.val[0], b_0.val[1], a_im );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], b_1.val[0], a_im );
    t1_01.val[0] = vfmsq_f64( t1_01.val[0], b_1.val[1], a_im );

    t1_02.val[1] = vfmaq_f64( t1_02.val[1], b_0.val[0], a_im );
    t1_02.val[0] = vfmsq_f64( t1_02.val[0], b_0.val[1], a_im );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], b_1.val[0], a_im );
    t1_03.val[0] = vfmsq_f64( t1_03.val[0], b_1.val[1], a_im );

    a_re = vld1q_dup_f64( va+4 );
    a_im = vld1q_dup_f64( va+5 );

    t2_00.val[0] = vfmaq_f64( t2_00.val[0], b_0.val[0], a_re );
    t2_00.val[1] = vfmaq_f64( t2_00.val[1], b_0.val[1], a_re );
    t2_01.val[0] = vfmaq_f64( t2_01.val[0], b_1.val[0], a_re );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], b_1.val[1], a_re );

    t2_02.val[0] = vfmaq_f64( t2_02.val[0], b_0.val[0], a_re );
    t2_02.val[1] = vfmaq_f64( t2_02.val[1], b_0.val[1], a_re );
    t2_03.val[0] = vfmaq_f64( t2_03.val[0], b_1.val[0], a_re );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], b_1.val[1], a_re );

    t2_00.val[1] = vfmaq_f64( t2_00.val[1], b_0.val[0], a_im );
    t2_00.val[0] = vfmsq_f64( t2_00.val[0], b_0.val[1], a_im );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], b_1.val[0], a_im );
    t2_01.val[0] = vfmsq_f64( t2_01.val[0], b_1.val[1], a_im );

    t2_02.val[1] = vfmaq_f64( t2_02.val[1], b_0.val[0], a_im );
    t2_02.val[0] = vfmsq_f64( t2_02.val[0], b_0.val[1], a_im );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], b_1.val[0], a_im );
    t2_03.val[0] = vfmsq_f64( t2_03.val[0], b_1.val[1], a_im );
    CCBLOCK;

    b += NR;
    va += MR*2;     // unroll*MR  va is pointer to double
  }

  if (alpha.real!=1.0||alpha.imag!=0.0) {
    a_re = vld1q_dup_f64( (double*)alpha_ );
    a_im = vld1q_dup_f64( 1+(double*)alpha_ );

    t0_00.val[0] = vfmaq_f64( t0_00.val[0], t0_00.val[0], a_re );
    t0_00.val[1] = vfmaq_f64( t0_00.val[1], t0_00.val[1], a_re );
    t0_01.val[0] = vfmaq_f64( t0_01.val[0], t0_01.val[0], a_re );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], t0_01.val[1], a_re );

    t0_02.val[0] = vfmaq_f64( t0_02.val[0], t0_02.val[0], a_re );
    t0_02.val[1] = vfmaq_f64( t0_02.val[1], t0_02.val[1], a_re );
    t0_03.val[0] = vfmaq_f64( t0_03.val[0], t0_03.val[0], a_re );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], t0_03.val[1], a_re );

    t0_00.val[1] = vfmaq_f64( t0_00.val[1], t0_00.val[0], a_im );
    t0_00.val[0] = vfmsq_f64( t0_00.val[0], t0_00.val[1], a_im );
    t0_01.val[1] = vfmaq_f64( t0_01.val[1], t0_01.val[0], a_im );
    t0_01.val[0] = vfmsq_f64( t0_01.val[0], t0_01.val[1], a_im );

    t0_02.val[1] = vfmaq_f64( t0_02.val[1], t0_02.val[0], a_im );
    t0_02.val[0] = vfmsq_f64( t0_02.val[0], t0_02.val[1], a_im );
    t0_03.val[1] = vfmaq_f64( t0_03.val[1], t0_03.val[0], a_im );
    t0_03.val[0] = vfmsq_f64( t0_03.val[0], t0_03.val[1], a_im );

    t1_00.val[0] = vfmaq_f64( t1_00.val[0], t1_00.val[0], a_re );
    t1_00.val[1] = vfmaq_f64( t1_00.val[1], t1_00.val[1], a_re );
    t1_01.val[0] = vfmaq_f64( t1_01.val[0], t1_01.val[0], a_re );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], t1_01.val[1], a_re );

    t1_02.val[0] = vfmaq_f64( t1_02.val[0], t1_02.val[0], a_re );
    t1_02.val[1] = vfmaq_f64( t1_02.val[1], t1_02.val[1], a_re );
    t1_03.val[0] = vfmaq_f64( t1_03.val[0], t1_03.val[0], a_re );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], t1_03.val[1], a_re );

    t1_00.val[1] = vfmaq_f64( t1_00.val[1], t1_00.val[0], a_im );
    t1_00.val[0] = vfmsq_f64( t1_00.val[0], t1_00.val[1], a_im );
    t1_01.val[1] = vfmaq_f64( t1_01.val[1], t1_01.val[0], a_im );
    t1_01.val[0] = vfmsq_f64( t1_01.val[0], t1_01.val[1], a_im );

    t1_02.val[1] = vfmaq_f64( t1_02.val[1], t1_02.val[0], a_im );
    t1_02.val[0] = vfmsq_f64( t1_02.val[0], t1_02.val[1], a_im );
    t1_03.val[1] = vfmaq_f64( t1_03.val[1], t1_03.val[0], a_im );
    t1_03.val[0] = vfmsq_f64( t1_03.val[0], t1_03.val[1], a_im );

    t2_00.val[0] = vfmaq_f64( t2_00.val[0], t2_00.val[0], a_re );
    t2_00.val[1] = vfmaq_f64( t2_00.val[1], t2_00.val[1], a_re );
    t2_01.val[0] = vfmaq_f64( t2_01.val[0], t2_01.val[0], a_re );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], t2_01.val[1], a_re );

    t2_02.val[0] = vfmaq_f64( t2_02.val[0], t2_02.val[0], a_re );
    t2_02.val[1] = vfmaq_f64( t2_02.val[1], t2_02.val[1], a_re );
    t2_03.val[0] = vfmaq_f64( t2_03.val[0], t2_03.val[0], a_re );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], t2_03.val[1], a_re );

    t2_00.val[1] = vfmaq_f64( t2_00.val[1], t2_00.val[0], a_im );
    t2_00.val[0] = vfmsq_f64( t2_00.val[0], t2_00.val[1], a_im );
    t2_01.val[1] = vfmaq_f64( t2_01.val[1], t2_01.val[0], a_im );
    t2_01.val[0] = vfmsq_f64( t2_01.val[0], t2_01.val[1], a_im );

    t2_02.val[1] = vfmaq_f64( t2_02.val[1], t2_02.val[0], a_im );
    t2_02.val[0] = vfmsq_f64( t2_02.val[0], t2_02.val[1], a_im );
    t2_03.val[1] = vfmaq_f64( t2_03.val[1], t2_03.val[0], a_im );
    t2_03.val[0] = vfmsq_f64( t2_03.val[0], t2_03.val[1], a_im );

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
  c[pc      ].real += p0[0];
  c[pc      ].imag += p0[2];
  c[pc+=cs_c].real += p0[1];
  c[pc      ].imag += p0[3];
  c[pc+=cs_c].real += p0[4];
  c[pc      ].imag += p0[6];
  c[pc+=cs_c].real += p0[5];
  c[pc      ].imag += p0[7];

  pc = rs_c;
  c[pc      ].real += p1[0];
  c[pc      ].imag += p1[2];
  c[pc+=cs_c].real += p1[1];
  c[pc      ].imag += p1[3];
  c[pc+=cs_c].real += p1[4];
  c[pc      ].imag += p1[6];
  c[pc+=cs_c].real += p1[5];
  c[pc      ].imag += p1[7];

  pc = 2*rs_c;
  c[pc      ].real += p2[0];
  c[pc      ].imag += p2[2];
  c[pc+=cs_c].real += p2[1];
  c[pc      ].imag += p2[3];
  c[pc+=cs_c].real += p2[4];
  c[pc      ].imag += p2[6];
  c[pc+=cs_c].real += p2[5];
  c[pc      ].imag += p2[7];

}

#undef MC
#undef KC
#undef NC
#undef MR
#undef NR

#undef LOOKAHEAD_A
#undef LOOKAHEAD_B

#endif  /* __aarch64__ */
