/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* gemm micro kernel                                                       */

#include "../blis.h"

#if BLIS_DRIVER == BLIS_DRIVER_AARCH64

#include <arm_neon.h>

#define MC  BLIS_DEFAULT_MC_D
#define KC  BLIS_DEFAULT_KC_D
#define NC  BLIS_DEFAULT_NC_D
#define MR  BLIS_DEFAULT_MR_D
#define NR  BLIS_DEFAULT_NR_D

#define NRv (NR/2)              // 2 lanes
#if !((MR==8)&&(NR==6))
#error "invalid MR NR"
#endif

void bli_dgemm_opt_8x6
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

  register float64x2_t b_re, b_im;
  register float64x2x2_t a_0,a_1;

  const double *vb = (const double *)b;

  register float64x2x2_t t0_0r, t0_1r, t0_0i, t0_1i;
  register float64x2x2_t t1_0r, t1_1r, t1_0i, t1_1i;
  register float64x2x2_t t2_0r, t2_1r, t2_0i, t2_1i;

  t0_0r.val[0] = t0_1r.val[0] = t0_0i.val[0] = t0_1i.val[0] = \
  t1_0r.val[0] = t1_1r.val[0] = t1_0i.val[0] = t1_1i.val[0] = \
  t2_0r.val[0] = t2_1r.val[0] = t2_0i.val[0] = t2_1i.val[0] = \
  t0_0r.val[1] = t0_1r.val[1] = t0_0i.val[1] = t0_1i.val[1] = \
  t1_0r.val[1] = t1_1r.val[1] = t1_0i.val[1] = t1_1i.val[1] = \
  t2_0r.val[1] = t2_1r.val[1] = t2_0i.val[1] = t2_1i.val[1] = \
  vdupq_n_f64(0.0);

  const double *p0,*p1; float64x2_t pp0[12],pp1[12];

  PREFETCH((void*)&c[0*cs_c]);
  PREFETCH((void*)&c[1*cs_c]);
  PREFETCH((void*)&c[2*cs_c]);
  PREFETCH((void*)&c[3*cs_c]);
  PREFETCH((void*)&c[4*cs_c]);
  PREFETCH((void*)&c[5*cs_c]);

  gint_t l;
  gint_t ki = k/4;  // unroll
  gint_t kr = k%4;
  for (l=0; l<ki; ++l) {
    PREFETCH((void*)(a+64));  //   PREFETCH((void*)(a+2*4*MR));

// iter 0
    a_0 = vld2q_f64( (double*)a );
    a_1 = vld2q_f64( 4+(double*)a );

    b_re = vld1q_dup_f64( vb );
    b_im = vld1q_dup_f64( vb+1 );
    t0_0r.val[0] = vfmaq_f64( t0_0r.val[0], a_0.val[0], b_re );
    t0_1r.val[0] = vfmaq_f64( t0_1r.val[0], a_0.val[1], b_re );
    t0_0i.val[0] = vfmaq_f64( t0_0i.val[0], a_0.val[0], b_im );
    t0_1i.val[0] = vfmaq_f64( t0_1i.val[0], a_0.val[1], b_im );
    t0_0r.val[1] = vfmaq_f64( t0_0r.val[1], a_1.val[0], b_re );
    t0_1r.val[1] = vfmaq_f64( t0_1r.val[1], a_1.val[1], b_re );
    t0_0i.val[1] = vfmaq_f64( t0_0i.val[1], a_1.val[0], b_im );
    t0_1i.val[1] = vfmaq_f64( t0_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+2 );
    b_im = vld1q_dup_f64( vb+3 );
    t1_0r.val[0] = vfmaq_f64( t1_0r.val[0], a_0.val[0], b_re );
    t1_1r.val[0] = vfmaq_f64( t1_1r.val[0], a_0.val[1], b_re );
    t1_0i.val[0] = vfmaq_f64( t1_0i.val[0], a_0.val[0], b_im );
    t1_1i.val[0] = vfmaq_f64( t1_1i.val[0], a_0.val[1], b_im );
    t1_0r.val[1] = vfmaq_f64( t1_0r.val[1], a_1.val[0], b_re );
    t1_1r.val[1] = vfmaq_f64( t1_1r.val[1], a_1.val[1], b_re );
    t1_0i.val[1] = vfmaq_f64( t1_0i.val[1], a_1.val[0], b_im );
    t1_1i.val[1] = vfmaq_f64( t1_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+4 );
    b_im = vld1q_dup_f64( vb+5 );
    t2_0r.val[0] = vfmaq_f64( t2_0r.val[0], a_0.val[0], b_re );
    t2_1r.val[0] = vfmaq_f64( t2_1r.val[0], a_0.val[1], b_re );
    t2_0i.val[0] = vfmaq_f64( t2_0i.val[0], a_0.val[0], b_im );
    t2_1i.val[0] = vfmaq_f64( t2_1i.val[0], a_0.val[1], b_im );
    t2_0r.val[1] = vfmaq_f64( t2_0r.val[1], a_1.val[0], b_re );
    t2_1r.val[1] = vfmaq_f64( t2_1r.val[1], a_1.val[1], b_re );
    t2_0i.val[1] = vfmaq_f64( t2_0i.val[1], a_1.val[0], b_im );
    t2_1i.val[1] = vfmaq_f64( t2_1i.val[1], a_1.val[1], b_im );

// iter 1
    a_0 = vld2q_f64( 8+(double*)a );
    a_1 = vld2q_f64( 12+(double*)a );

    b_re = vld1q_dup_f64( vb+6 );
    b_im = vld1q_dup_f64( vb+7 );
    t0_0r.val[0] = vfmaq_f64( t0_0r.val[0], a_0.val[0], b_re );
    t0_1r.val[0] = vfmaq_f64( t0_1r.val[0], a_0.val[1], b_re );
    t0_0i.val[0] = vfmaq_f64( t0_0i.val[0], a_0.val[0], b_im );
    t0_1i.val[0] = vfmaq_f64( t0_1i.val[0], a_0.val[1], b_im );
    t0_0r.val[1] = vfmaq_f64( t0_0r.val[1], a_1.val[0], b_re );
    t0_1r.val[1] = vfmaq_f64( t0_1r.val[1], a_1.val[1], b_re );
    t0_0i.val[1] = vfmaq_f64( t0_0i.val[1], a_1.val[0], b_im );
    t0_1i.val[1] = vfmaq_f64( t0_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+8 );
    b_im = vld1q_dup_f64( vb+9 );
    t1_0r.val[0] = vfmaq_f64( t1_0r.val[0], a_0.val[0], b_re );
    t1_1r.val[0] = vfmaq_f64( t1_1r.val[0], a_0.val[1], b_re );
    t1_0i.val[0] = vfmaq_f64( t1_0i.val[0], a_0.val[0], b_im );
    t1_1i.val[0] = vfmaq_f64( t1_1i.val[0], a_0.val[1], b_im );
    t1_0r.val[1] = vfmaq_f64( t1_0r.val[1], a_1.val[0], b_re );
    t1_1r.val[1] = vfmaq_f64( t1_1r.val[1], a_1.val[1], b_re );
    t1_0i.val[1] = vfmaq_f64( t1_0i.val[1], a_1.val[0], b_im );
    t1_1i.val[1] = vfmaq_f64( t1_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+10 );
    b_im = vld1q_dup_f64( vb+11 );
    t2_0r.val[0] = vfmaq_f64( t2_0r.val[0], a_0.val[0], b_re );
    t2_1r.val[0] = vfmaq_f64( t2_1r.val[0], a_0.val[1], b_re );
    t2_0i.val[0] = vfmaq_f64( t2_0i.val[0], a_0.val[0], b_im );
    t2_1i.val[0] = vfmaq_f64( t2_1i.val[0], a_0.val[1], b_im );
    t2_0r.val[1] = vfmaq_f64( t2_0r.val[1], a_1.val[0], b_re );
    t2_1r.val[1] = vfmaq_f64( t2_1r.val[1], a_1.val[1], b_re );
    t2_0i.val[1] = vfmaq_f64( t2_0i.val[1], a_1.val[0], b_im );
    t2_1i.val[1] = vfmaq_f64( t2_1i.val[1], a_1.val[1], b_im );

// iter 2
    PREFETCH((void*)(a+72)); // PREFETCH((void*)(a+MR+2*4*MR));

    a_0 = vld2q_f64( 16+(double*)a );
    a_1 = vld2q_f64( 20+(double*)a );

    b_re = vld1q_dup_f64( vb+12 );
    b_im = vld1q_dup_f64( vb+13 );
    t0_0r.val[0] = vfmaq_f64( t0_0r.val[0], a_0.val[0], b_re );
    t0_1r.val[0] = vfmaq_f64( t0_1r.val[0], a_0.val[1], b_re );
    t0_0i.val[0] = vfmaq_f64( t0_0i.val[0], a_0.val[0], b_im );
    t0_1i.val[0] = vfmaq_f64( t0_1i.val[0], a_0.val[1], b_im );
    t0_0r.val[1] = vfmaq_f64( t0_0r.val[1], a_1.val[0], b_re );
    t0_1r.val[1] = vfmaq_f64( t0_1r.val[1], a_1.val[1], b_re );
    t0_0i.val[1] = vfmaq_f64( t0_0i.val[1], a_1.val[0], b_im );
    t0_1i.val[1] = vfmaq_f64( t0_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+14 );
    b_im = vld1q_dup_f64( vb+15 );
    t1_0r.val[0] = vfmaq_f64( t1_0r.val[0], a_0.val[0], b_re );
    t1_1r.val[0] = vfmaq_f64( t1_1r.val[0], a_0.val[1], b_re );
    t1_0i.val[0] = vfmaq_f64( t1_0i.val[0], a_0.val[0], b_im );
    t1_1i.val[0] = vfmaq_f64( t1_1i.val[0], a_0.val[1], b_im );
    t1_0r.val[1] = vfmaq_f64( t1_0r.val[1], a_1.val[0], b_re );
    t1_1r.val[1] = vfmaq_f64( t1_1r.val[1], a_1.val[1], b_re );
    t1_0i.val[1] = vfmaq_f64( t1_0i.val[1], a_1.val[0], b_im );
    t1_1i.val[1] = vfmaq_f64( t1_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+16 );
    b_im = vld1q_dup_f64( vb+17 );
    t2_0r.val[0] = vfmaq_f64( t2_0r.val[0], a_0.val[0], b_re );
    t2_1r.val[0] = vfmaq_f64( t2_1r.val[0], a_0.val[1], b_re );
    t2_0i.val[0] = vfmaq_f64( t2_0i.val[0], a_0.val[0], b_im );
    t2_1i.val[0] = vfmaq_f64( t2_1i.val[0], a_0.val[1], b_im );
    t2_0r.val[1] = vfmaq_f64( t2_0r.val[1], a_1.val[0], b_re );
    t2_1r.val[1] = vfmaq_f64( t2_1r.val[1], a_1.val[1], b_re );
    t2_0i.val[1] = vfmaq_f64( t2_0i.val[1], a_1.val[0], b_im );
    t2_1i.val[1] = vfmaq_f64( t2_1i.val[1], a_1.val[1], b_im );

// iter 3
    a_0 = vld2q_f64( 24+(double*)a );
    a_1 = vld2q_f64( 28+(double*)a );

    b_re = vld1q_dup_f64( vb+18 );
    b_im = vld1q_dup_f64( vb+19 );
    t0_0r.val[0] = vfmaq_f64( t0_0r.val[0], a_0.val[0], b_re );
    t0_1r.val[0] = vfmaq_f64( t0_1r.val[0], a_0.val[1], b_re );
    t0_0i.val[0] = vfmaq_f64( t0_0i.val[0], a_0.val[0], b_im );
    t0_1i.val[0] = vfmaq_f64( t0_1i.val[0], a_0.val[1], b_im );
    t0_0r.val[1] = vfmaq_f64( t0_0r.val[1], a_1.val[0], b_re );
    t0_1r.val[1] = vfmaq_f64( t0_1r.val[1], a_1.val[1], b_re );
    t0_0i.val[1] = vfmaq_f64( t0_0i.val[1], a_1.val[0], b_im );
    t0_1i.val[1] = vfmaq_f64( t0_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+20 );
    b_im = vld1q_dup_f64( vb+21 );
    t1_0r.val[0] = vfmaq_f64( t1_0r.val[0], a_0.val[0], b_re );
    t1_1r.val[0] = vfmaq_f64( t1_1r.val[0], a_0.val[1], b_re );
    t1_0i.val[0] = vfmaq_f64( t1_0i.val[0], a_0.val[0], b_im );
    t1_1i.val[0] = vfmaq_f64( t1_1i.val[0], a_0.val[1], b_im );
    t1_0r.val[1] = vfmaq_f64( t1_0r.val[1], a_1.val[0], b_re );
    t1_1r.val[1] = vfmaq_f64( t1_1r.val[1], a_1.val[1], b_re );
    t1_0i.val[1] = vfmaq_f64( t1_0i.val[1], a_1.val[0], b_im );
    t1_1i.val[1] = vfmaq_f64( t1_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+22 );
    b_im = vld1q_dup_f64( vb+23 );
    t2_0r.val[0] = vfmaq_f64( t2_0r.val[0], a_0.val[0], b_re );
    t2_1r.val[0] = vfmaq_f64( t2_1r.val[0], a_0.val[1], b_re );
    t2_0i.val[0] = vfmaq_f64( t2_0i.val[0], a_0.val[0], b_im );
    t2_1i.val[0] = vfmaq_f64( t2_1i.val[0], a_0.val[1], b_im );
    t2_0r.val[1] = vfmaq_f64( t2_0r.val[1], a_1.val[0], b_re );
    t2_1r.val[1] = vfmaq_f64( t2_1r.val[1], a_1.val[1], b_re );
    t2_0i.val[1] = vfmaq_f64( t2_0i.val[1], a_1.val[0], b_im );
    t2_1i.val[1] = vfmaq_f64( t2_1i.val[1], a_1.val[1], b_im );

    a += 4*MR;      // unroll*MR
    vb += 4*NR;     // unroll*NR  vb is pointer to double
  }

  for (l=0; l<kr; ++l) {
    PREFETCH((void*)(a+64)); // PREFETCH((void*)(a+2*4*MR));

    a_0 = vld2q_f64( (double*)a );
    a_1 = vld2q_f64( 4+(double*)a );

    b_re = vld1q_dup_f64( vb );
    b_im = vld1q_dup_f64( vb+1 );
    t0_0r.val[0] = vfmaq_f64( t0_0r.val[0], a_0.val[0], b_re );
    t0_1r.val[0] = vfmaq_f64( t0_1r.val[0], a_0.val[1], b_re );
    t0_0i.val[0] = vfmaq_f64( t0_0i.val[0], a_0.val[0], b_im );
    t0_1i.val[0] = vfmaq_f64( t0_1i.val[0], a_0.val[1], b_im );
    t0_0r.val[1] = vfmaq_f64( t0_0r.val[1], a_1.val[0], b_re );
    t0_1r.val[1] = vfmaq_f64( t0_1r.val[1], a_1.val[1], b_re );
    t0_0i.val[1] = vfmaq_f64( t0_0i.val[1], a_1.val[0], b_im );
    t0_1i.val[1] = vfmaq_f64( t0_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+2 );
    b_im = vld1q_dup_f64( vb+3 );
    t1_0r.val[0] = vfmaq_f64( t1_0r.val[0], a_0.val[0], b_re );
    t1_1r.val[0] = vfmaq_f64( t1_1r.val[0], a_0.val[1], b_re );
    t1_0i.val[0] = vfmaq_f64( t1_0i.val[0], a_0.val[0], b_im );
    t1_1i.val[0] = vfmaq_f64( t1_1i.val[0], a_0.val[1], b_im );
    t1_0r.val[1] = vfmaq_f64( t1_0r.val[1], a_1.val[0], b_re );
    t1_1r.val[1] = vfmaq_f64( t1_1r.val[1], a_1.val[1], b_re );
    t1_0i.val[1] = vfmaq_f64( t1_0i.val[1], a_1.val[0], b_im );
    t1_1i.val[1] = vfmaq_f64( t1_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+4 );
    b_im = vld1q_dup_f64( vb+5 );
    t2_0r.val[0] = vfmaq_f64( t2_0r.val[0], a_0.val[0], b_re );
    t2_1r.val[0] = vfmaq_f64( t2_1r.val[0], a_0.val[1], b_re );
    t2_0i.val[0] = vfmaq_f64( t2_0i.val[0], a_0.val[0], b_im );
    t2_1i.val[0] = vfmaq_f64( t2_1i.val[0], a_0.val[1], b_im );
    t2_0r.val[1] = vfmaq_f64( t2_0r.val[1], a_1.val[0], b_re );
    t2_1r.val[1] = vfmaq_f64( t2_1r.val[1], a_1.val[1], b_re );
    t2_0i.val[1] = vfmaq_f64( t2_0i.val[1], a_1.val[0], b_im );
    t2_1i.val[1] = vfmaq_f64( t2_1i.val[1], a_1.val[1], b_im );

    a += MR;
    vb += NR;       // vb is pointer to double
  }

  if (alpha!=1.0) {
    b_re = vld1q_dup_f64( (double*)alpha_ );

    t0_0r.val[0] = vmulq_f64 ( t0_0r.val[0] , b_re );
    t0_1r.val[0] = vmulq_f64 ( t0_1r.val[0] , b_re );
    t0_0i.val[0] = vmulq_f64 ( t0_0i.val[0] , b_re );
    t0_1i.val[0] = vmulq_f64 ( t0_1i.val[0] , b_re );

    t0_0r.val[1] = vmulq_f64 ( t0_0r.val[1] , b_re );
    t0_1r.val[1] = vmulq_f64 ( t0_1r.val[1] , b_re );
    t0_0i.val[1] = vmulq_f64 ( t0_0i.val[1] , b_re );
    t0_1i.val[1] = vmulq_f64 ( t0_1i.val[1] , b_re );

    t1_0r.val[0] = vmulq_f64 ( t1_0r.val[0] , b_re );
    t1_1r.val[0] = vmulq_f64 ( t1_1r.val[0] , b_re );
    t1_0i.val[0] = vmulq_f64 ( t1_0i.val[0] , b_re );
    t1_1i.val[0] = vmulq_f64 ( t1_1i.val[0] , b_re );

    t1_0r.val[1] = vmulq_f64 ( t1_0r.val[1] , b_re );
    t1_1r.val[1] = vmulq_f64 ( t1_1r.val[1] , b_re );
    t1_0i.val[1] = vmulq_f64 ( t1_0i.val[1] , b_re );
    t1_1i.val[1] = vmulq_f64 ( t1_1i.val[1] , b_re );

    t2_0r.val[0] = vmulq_f64 ( t2_0r.val[0] , b_re );
    t2_1r.val[0] = vmulq_f64 ( t2_1r.val[0] , b_re );
    t2_0i.val[0] = vmulq_f64 ( t2_0i.val[0] , b_re );
    t2_1i.val[0] = vmulq_f64 ( t2_1i.val[0] , b_re );

    t2_0r.val[1] = vmulq_f64 ( t2_0r.val[1] , b_re );
    t2_1r.val[1] = vmulq_f64 ( t2_1r.val[1] , b_re );
    t2_0i.val[1] = vmulq_f64 ( t2_0i.val[1] , b_re );
    t2_1i.val[1] = vmulq_f64 ( t2_1i.val[1] , b_re );

  }

  if (beta!=1.0) {
    gint_t i;
    for (i=0; i<MR; ++i) {
      gint_t j;
      for (j=0; j<NR; ++j) {               // !!! NOT NRv
        c[i*rs_c+j*cs_c] *= beta;
      }
    }
  }

  p0 = (const double *) &pp0;
  p1 = (const double *) &pp1;

  pp0[0]  = t0_0r.val[0];    // p0 0
  pp0[1]  = t0_0i.val[0];
  pp0[2]  = t1_0r.val[0];
  pp0[3]  = t1_0i.val[0];
  pp0[4]  = t2_0r.val[0];
  pp0[5]  = t2_0i.val[0];

  pp0[6]  = t0_0r.val[1];    // p0 12
  pp0[7]  = t0_0i.val[1];
  pp0[8]  = t1_0r.val[1];
  pp0[9]  = t1_0i.val[1];
  pp0[10] = t2_0r.val[1];
  pp0[11] = t2_0i.val[1];

  pp1[0]  = t0_1r.val[0];    // p1 0
  pp1[1]  = t0_1i.val[0];
  pp1[2]  = t1_1r.val[0];
  pp1[3]  = t1_1i.val[0];
  pp1[4]  = t2_1r.val[0];
  pp1[5]  = t2_1i.val[0];

  pp1[6]  = t0_1r.val[1];    // p1 12
  pp1[7]  = t0_1i.val[1];
  pp1[8]  = t1_1r.val[1];
  pp1[9]  = t1_1i.val[1];
  pp1[10] = t2_1r.val[1];
  pp1[11] = t2_1i.val[1];

  gint_t pc;

  pc = 0;
  c[pc      ] += p0[0];
  c[pc+=cs_c] += p0[2];
  c[pc+=cs_c] += p0[4];
  c[pc+=cs_c] += p0[6];
  c[pc+=cs_c] += p0[8];
  c[pc+=cs_c] += p0[10];

  pc = rs_c;
  c[pc      ] += p1[0];
  c[pc+=cs_c] += p1[2];
  c[pc+=cs_c] += p1[4];
  c[pc+=cs_c] += p1[6];
  c[pc+=cs_c] += p1[8];
  c[pc+=cs_c] += p1[10];

  pc = 2*rs_c;
  c[pc      ] += p0[1];
  c[pc+=cs_c] += p0[3];
  c[pc+=cs_c] += p0[5];
  c[pc+=cs_c] += p0[7];
  c[pc+=cs_c] += p0[9];
  c[pc+=cs_c] += p0[11];

  pc = 3*rs_c;
  c[pc      ] += p1[1];
  c[pc+=cs_c] += p1[3];
  c[pc+=cs_c] += p1[5];
  c[pc+=cs_c] += p1[7];
  c[pc+=cs_c] += p1[9];
  c[pc+=cs_c] += p1[11];

  pc = 4*rs_c;
  c[pc      ] += p0[12];
  c[pc+=cs_c] += p0[14];
  c[pc+=cs_c] += p0[16];
  c[pc+=cs_c] += p0[18];
  c[pc+=cs_c] += p0[20];
  c[pc+=cs_c] += p0[22];

  pc = 5*rs_c;
  c[pc      ] += p1[12];
  c[pc+=cs_c] += p1[14];
  c[pc+=cs_c] += p1[16];
  c[pc+=cs_c] += p1[18];
  c[pc+=cs_c] += p1[20];
  c[pc+=cs_c] += p1[22];

  pc = 6*rs_c;
  c[pc      ] += p0[13];
  c[pc+=cs_c] += p0[15];
  c[pc+=cs_c] += p0[17];
  c[pc+=cs_c] += p0[19];
  c[pc+=cs_c] += p0[21];
  c[pc+=cs_c] += p0[23];

  pc = 7*rs_c;
  c[pc      ] += p1[13];
  c[pc+=cs_c] += p1[15];
  c[pc+=cs_c] += p1[17];
  c[pc+=cs_c] += p1[19];
  c[pc+=cs_c] += p1[21];
  c[pc+=cs_c] += p1[23];

}

#undef NRv

#undef MC
#undef KC
#undef NC
#undef MR
#undef NR

#define MC  BLIS_DEFAULT_MC_Z
#define KC  BLIS_DEFAULT_KC_Z
#define NC  BLIS_DEFAULT_NC_Z
#define MR  BLIS_DEFAULT_MR_Z
#define NR  BLIS_DEFAULT_NR_Z


#define NRv (NR/4)
#if !((MR==4)&&(NR==3))
#error "invalid MR NR"
#endif

void bli_zgemm_opt_4x3
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

  register float64x2_t b_re, b_im;
  register float64x2x2_t a_0,a_1;

  const double *vb = (const double *)b;

  register float64x2x2_t t0_0r, t0_1r, t0_0i, t0_1i;
  register float64x2x2_t t1_0r, t1_1r, t1_0i, t1_1i;
  register float64x2x2_t t2_0r, t2_1r, t2_0i, t2_1i;

  t0_0r.val[0] = t0_1r.val[0] = t0_0i.val[0] = t0_1i.val[0] = \
  t1_0r.val[0] = t1_1r.val[0] = t1_0i.val[0] = t1_1i.val[0] = \
  t2_0r.val[0] = t2_1r.val[0] = t2_0i.val[0] = t2_1i.val[0] = \
  t0_0r.val[1] = t0_1r.val[1] = t0_0i.val[1] = t0_1i.val[1] = \
  t1_0r.val[1] = t1_1r.val[1] = t1_0i.val[1] = t1_1i.val[1] = \
  t2_0r.val[1] = t2_1r.val[1] = t2_0i.val[1] = t2_1i.val[1] = \
  vdupq_n_f64(0.0);

  const double *p0,*p1; float64x2_t pp0[6],pp1[6];

  PREFETCH((void*)&c[0*cs_c]);
  PREFETCH((void*)&c[1*cs_c]);
  PREFETCH((void*)&c[2*cs_c]);

  gint_t l;
  gint_t ki = k/4;
  gint_t kr = k%4;
  for (l=0; l<ki; ++l) {

// iter 0
    PREFETCH((void*)(a+32)); // PREFETCH((void*)(a+2*4*MR));

    a_0 = vld2q_f64( (double*)a );
    a_1 = vld2q_f64( 4+(double*)a );

    b_re = vld1q_dup_f64( vb );
    b_im = vld1q_dup_f64( vb+1 );
    t0_0r.val[0] = vfmaq_f64( t0_0r.val[0], a_0.val[0], b_re );
    t0_1r.val[0] = vfmaq_f64( t0_1r.val[0], a_0.val[1], b_re );
    t0_0i.val[0] = vfmaq_f64( t0_0i.val[0], a_0.val[0], b_im );
    t0_1i.val[0] = vfmaq_f64( t0_1i.val[0], a_0.val[1], b_im );
    t0_0r.val[1] = vfmaq_f64( t0_0r.val[1], a_1.val[0], b_re );
    t0_1r.val[1] = vfmaq_f64( t0_1r.val[1], a_1.val[1], b_re );
    t0_0i.val[1] = vfmaq_f64( t0_0i.val[1], a_1.val[0], b_im );
    t0_1i.val[1] = vfmaq_f64( t0_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+2 );
    b_im = vld1q_dup_f64( vb+3 );
    t1_0r.val[0] = vfmaq_f64( t1_0r.val[0], a_0.val[0], b_re );
    t1_1r.val[0] = vfmaq_f64( t1_1r.val[0], a_0.val[1], b_re );
    t1_0i.val[0] = vfmaq_f64( t1_0i.val[0], a_0.val[0], b_im );
    t1_1i.val[0] = vfmaq_f64( t1_1i.val[0], a_0.val[1], b_im );
    t1_0r.val[1] = vfmaq_f64( t1_0r.val[1], a_1.val[0], b_re );
    t1_1r.val[1] = vfmaq_f64( t1_1r.val[1], a_1.val[1], b_re );
    t1_0i.val[1] = vfmaq_f64( t1_0i.val[1], a_1.val[0], b_im );
    t1_1i.val[1] = vfmaq_f64( t1_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+4 );
    b_im = vld1q_dup_f64( vb+5 );
    t2_0r.val[0] = vfmaq_f64( t2_0r.val[0], a_0.val[0], b_re );
    t2_1r.val[0] = vfmaq_f64( t2_1r.val[0], a_0.val[1], b_re );
    t2_0i.val[0] = vfmaq_f64( t2_0i.val[0], a_0.val[0], b_im );
    t2_1i.val[0] = vfmaq_f64( t2_1i.val[0], a_0.val[1], b_im );
    t2_0r.val[1] = vfmaq_f64( t2_0r.val[1], a_1.val[0], b_re );
    t2_1r.val[1] = vfmaq_f64( t2_1r.val[1], a_1.val[1], b_re );
    t2_0i.val[1] = vfmaq_f64( t2_0i.val[1], a_1.val[0], b_im );
    t2_1i.val[1] = vfmaq_f64( t2_1i.val[1], a_1.val[1], b_im );

// iter 1
    a_0 = vld2q_f64( 8+(double*)a );
    a_1 = vld2q_f64( 12+(double*)a );

    b_re = vld1q_dup_f64( vb+6 );
    b_im = vld1q_dup_f64( vb+7 );
    t0_0r.val[0] = vfmaq_f64( t0_0r.val[0], a_0.val[0], b_re );
    t0_1r.val[0] = vfmaq_f64( t0_1r.val[0], a_0.val[1], b_re );
    t0_0i.val[0] = vfmaq_f64( t0_0i.val[0], a_0.val[0], b_im );
    t0_1i.val[0] = vfmaq_f64( t0_1i.val[0], a_0.val[1], b_im );
    t0_0r.val[1] = vfmaq_f64( t0_0r.val[1], a_1.val[0], b_re );
    t0_1r.val[1] = vfmaq_f64( t0_1r.val[1], a_1.val[1], b_re );
    t0_0i.val[1] = vfmaq_f64( t0_0i.val[1], a_1.val[0], b_im );
    t0_1i.val[1] = vfmaq_f64( t0_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+8 );
    b_im = vld1q_dup_f64( vb+9 );
    t1_0r.val[0] = vfmaq_f64( t1_0r.val[0], a_0.val[0], b_re );
    t1_1r.val[0] = vfmaq_f64( t1_1r.val[0], a_0.val[1], b_re );
    t1_0i.val[0] = vfmaq_f64( t1_0i.val[0], a_0.val[0], b_im );
    t1_1i.val[0] = vfmaq_f64( t1_1i.val[0], a_0.val[1], b_im );
    t1_0r.val[1] = vfmaq_f64( t1_0r.val[1], a_1.val[0], b_re );
    t1_1r.val[1] = vfmaq_f64( t1_1r.val[1], a_1.val[1], b_re );
    t1_0i.val[1] = vfmaq_f64( t1_0i.val[1], a_1.val[0], b_im );
    t1_1i.val[1] = vfmaq_f64( t1_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+10 );
    b_im = vld1q_dup_f64( vb+11 );
    t2_0r.val[0] = vfmaq_f64( t2_0r.val[0], a_0.val[0], b_re );
    t2_1r.val[0] = vfmaq_f64( t2_1r.val[0], a_0.val[1], b_re );
    t2_0i.val[0] = vfmaq_f64( t2_0i.val[0], a_0.val[0], b_im );
    t2_1i.val[0] = vfmaq_f64( t2_1i.val[0], a_0.val[1], b_im );
    t2_0r.val[1] = vfmaq_f64( t2_0r.val[1], a_1.val[0], b_re );
    t2_1r.val[1] = vfmaq_f64( t2_1r.val[1], a_1.val[1], b_re );
    t2_0i.val[1] = vfmaq_f64( t2_0i.val[1], a_1.val[0], b_im );
    t2_1i.val[1] = vfmaq_f64( t2_1i.val[1], a_1.val[1], b_im );

// iter 2
    PREFETCH((void*)(a+36)); // PREFETCH((void*)(a+MR+2*4*MR));

    a_0 = vld2q_f64( 16+(double*)a );
    a_1 = vld2q_f64( 20+(double*)a );

    b_re = vld1q_dup_f64( vb+12 );
    b_im = vld1q_dup_f64( vb+13 );
    t0_0r.val[0] = vfmaq_f64( t0_0r.val[0], a_0.val[0], b_re );
    t0_1r.val[0] = vfmaq_f64( t0_1r.val[0], a_0.val[1], b_re );
    t0_0i.val[0] = vfmaq_f64( t0_0i.val[0], a_0.val[0], b_im );
    t0_1i.val[0] = vfmaq_f64( t0_1i.val[0], a_0.val[1], b_im );
    t0_0r.val[1] = vfmaq_f64( t0_0r.val[1], a_1.val[0], b_re );
    t0_1r.val[1] = vfmaq_f64( t0_1r.val[1], a_1.val[1], b_re );
    t0_0i.val[1] = vfmaq_f64( t0_0i.val[1], a_1.val[0], b_im );
    t0_1i.val[1] = vfmaq_f64( t0_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+14 );
    b_im = vld1q_dup_f64( vb+15 );
    t1_0r.val[0] = vfmaq_f64( t1_0r.val[0], a_0.val[0], b_re );
    t1_1r.val[0] = vfmaq_f64( t1_1r.val[0], a_0.val[1], b_re );
    t1_0i.val[0] = vfmaq_f64( t1_0i.val[0], a_0.val[0], b_im );
    t1_1i.val[0] = vfmaq_f64( t1_1i.val[0], a_0.val[1], b_im );
    t1_0r.val[1] = vfmaq_f64( t1_0r.val[1], a_1.val[0], b_re );
    t1_1r.val[1] = vfmaq_f64( t1_1r.val[1], a_1.val[1], b_re );
    t1_0i.val[1] = vfmaq_f64( t1_0i.val[1], a_1.val[0], b_im );
    t1_1i.val[1] = vfmaq_f64( t1_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+16 );
    b_im = vld1q_dup_f64( vb+17 );
    t2_0r.val[0] = vfmaq_f64( t2_0r.val[0], a_0.val[0], b_re );
    t2_1r.val[0] = vfmaq_f64( t2_1r.val[0], a_0.val[1], b_re );
    t2_0i.val[0] = vfmaq_f64( t2_0i.val[0], a_0.val[0], b_im );
    t2_1i.val[0] = vfmaq_f64( t2_1i.val[0], a_0.val[1], b_im );
    t2_0r.val[1] = vfmaq_f64( t2_0r.val[1], a_1.val[0], b_re );
    t2_1r.val[1] = vfmaq_f64( t2_1r.val[1], a_1.val[1], b_re );
    t2_0i.val[1] = vfmaq_f64( t2_0i.val[1], a_1.val[0], b_im );
    t2_1i.val[1] = vfmaq_f64( t2_1i.val[1], a_1.val[1], b_im );

// iter 3
    a_0 = vld2q_f64( 24+(double*)a );
    a_1 = vld2q_f64( 28+(double*)a );

    b_re = vld1q_dup_f64( vb+18 );
    b_im = vld1q_dup_f64( vb+19 );
    t0_0r.val[0] = vfmaq_f64( t0_0r.val[0], a_0.val[0], b_re );
    t0_1r.val[0] = vfmaq_f64( t0_1r.val[0], a_0.val[1], b_re );
    t0_0i.val[0] = vfmaq_f64( t0_0i.val[0], a_0.val[0], b_im );
    t0_1i.val[0] = vfmaq_f64( t0_1i.val[0], a_0.val[1], b_im );
    t0_0r.val[1] = vfmaq_f64( t0_0r.val[1], a_1.val[0], b_re );
    t0_1r.val[1] = vfmaq_f64( t0_1r.val[1], a_1.val[1], b_re );
    t0_0i.val[1] = vfmaq_f64( t0_0i.val[1], a_1.val[0], b_im );
    t0_1i.val[1] = vfmaq_f64( t0_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+20 );
    b_im = vld1q_dup_f64( vb+21 );
    t1_0r.val[0] = vfmaq_f64( t1_0r.val[0], a_0.val[0], b_re );
    t1_1r.val[0] = vfmaq_f64( t1_1r.val[0], a_0.val[1], b_re );
    t1_0i.val[0] = vfmaq_f64( t1_0i.val[0], a_0.val[0], b_im );
    t1_1i.val[0] = vfmaq_f64( t1_1i.val[0], a_0.val[1], b_im );
    t1_0r.val[1] = vfmaq_f64( t1_0r.val[1], a_1.val[0], b_re );
    t1_1r.val[1] = vfmaq_f64( t1_1r.val[1], a_1.val[1], b_re );
    t1_0i.val[1] = vfmaq_f64( t1_0i.val[1], a_1.val[0], b_im );
    t1_1i.val[1] = vfmaq_f64( t1_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+22 );
    b_im = vld1q_dup_f64( vb+23 );
    t2_0r.val[0] = vfmaq_f64( t2_0r.val[0], a_0.val[0], b_re );
    t2_1r.val[0] = vfmaq_f64( t2_1r.val[0], a_0.val[1], b_re );
    t2_0i.val[0] = vfmaq_f64( t2_0i.val[0], a_0.val[0], b_im );
    t2_1i.val[0] = vfmaq_f64( t2_1i.val[0], a_0.val[1], b_im );
    t2_0r.val[1] = vfmaq_f64( t2_0r.val[1], a_1.val[0], b_re );
    t2_1r.val[1] = vfmaq_f64( t2_1r.val[1], a_1.val[1], b_re );
    t2_0i.val[1] = vfmaq_f64( t2_0i.val[1], a_1.val[0], b_im );
    t2_1i.val[1] = vfmaq_f64( t2_1i.val[1], a_1.val[1], b_im );

    a += 4*MR;        // unroll*MR
    vb += 4*NR*2;     // unroll*NR  vb is pointer to double
  }

  for (l=0; l<kr; ++l) {

    PREFETCH((void*)(a+32)); // PREFETCH((void*)(a+2*4*MR));

    a_0 = vld2q_f64( (double*)a );
    a_1 = vld2q_f64( 4+(double*)a );

    b_re = vld1q_dup_f64( vb );
    b_im = vld1q_dup_f64( vb+1 );
    t0_0r.val[0] = vfmaq_f64( t0_0r.val[0], a_0.val[0], b_re );
    t0_1r.val[0] = vfmaq_f64( t0_1r.val[0], a_0.val[1], b_re );
    t0_0i.val[0] = vfmaq_f64( t0_0i.val[0], a_0.val[0], b_im );
    t0_1i.val[0] = vfmaq_f64( t0_1i.val[0], a_0.val[1], b_im );
    t0_0r.val[1] = vfmaq_f64( t0_0r.val[1], a_1.val[0], b_re );
    t0_1r.val[1] = vfmaq_f64( t0_1r.val[1], a_1.val[1], b_re );
    t0_0i.val[1] = vfmaq_f64( t0_0i.val[1], a_1.val[0], b_im );
    t0_1i.val[1] = vfmaq_f64( t0_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+2 );
    b_im = vld1q_dup_f64( vb+3 );
    t1_0r.val[0] = vfmaq_f64( t1_0r.val[0], a_0.val[0], b_re );
    t1_1r.val[0] = vfmaq_f64( t1_1r.val[0], a_0.val[1], b_re );
    t1_0i.val[0] = vfmaq_f64( t1_0i.val[0], a_0.val[0], b_im );
    t1_1i.val[0] = vfmaq_f64( t1_1i.val[0], a_0.val[1], b_im );
    t1_0r.val[1] = vfmaq_f64( t1_0r.val[1], a_1.val[0], b_re );
    t1_1r.val[1] = vfmaq_f64( t1_1r.val[1], a_1.val[1], b_re );
    t1_0i.val[1] = vfmaq_f64( t1_0i.val[1], a_1.val[0], b_im );
    t1_1i.val[1] = vfmaq_f64( t1_1i.val[1], a_1.val[1], b_im );

    b_re = vld1q_dup_f64( vb+4 );
    b_im = vld1q_dup_f64( vb+5 );
    t2_0r.val[0] = vfmaq_f64( t2_0r.val[0], a_0.val[0], b_re );
    t2_1r.val[0] = vfmaq_f64( t2_1r.val[0], a_0.val[1], b_re );
    t2_0i.val[0] = vfmaq_f64( t2_0i.val[0], a_0.val[0], b_im );
    t2_1i.val[0] = vfmaq_f64( t2_1i.val[0], a_0.val[1], b_im );
    t2_0r.val[1] = vfmaq_f64( t2_0r.val[1], a_1.val[0], b_re );
    t2_1r.val[1] = vfmaq_f64( t2_1r.val[1], a_1.val[1], b_re );
    t2_0i.val[1] = vfmaq_f64( t2_0i.val[1], a_1.val[0], b_im );
    t2_1i.val[1] = vfmaq_f64( t2_1i.val[1], a_1.val[1], b_im );

    a += MR;
    vb += NR*2;       // vb is pointer to double
  }

  t0_0r.val[0] = vsubq_f64( t0_0r.val[0], t0_1i.val[0] );
  t0_1r.val[0] = vaddq_f64( t0_1r.val[0], t0_0i.val[0] );
  t0_0r.val[1] = vsubq_f64( t0_0r.val[1], t0_1i.val[1] );
  t0_1r.val[1] = vaddq_f64( t0_1r.val[1], t0_0i.val[1] );

  t1_0r.val[0] = vsubq_f64( t1_0r.val[0], t1_1i.val[0] );
  t1_1r.val[0] = vaddq_f64( t1_1r.val[0], t1_0i.val[0] );
  t1_0r.val[1] = vsubq_f64( t1_0r.val[1], t1_1i.val[1] );
  t1_1r.val[1] = vaddq_f64( t1_1r.val[1], t1_0i.val[1] );

  t2_0r.val[0] = vsubq_f64( t2_0r.val[0], t2_1i.val[0] );
  t2_1r.val[0] = vaddq_f64( t2_1r.val[0], t2_0i.val[0] );
  t2_0r.val[1] = vsubq_f64( t2_0r.val[1], t2_1i.val[1] );
  t2_1r.val[1] = vaddq_f64( t2_1r.val[1], t2_0i.val[1] );

  if (alpha.real!=1.0||alpha.imag!=0.0) {
    b_re = vld1q_dup_f64( (double*)alpha_ );
    b_im = vld1q_dup_f64( 1+(double*)alpha_ );

    t0_0i.val[0] = vmulq_f64( t0_0r.val[0], b_re );
    t0_1i.val[0] = vmulq_f64( t0_1r.val[0], b_im );
    t0_0r.val[0] = vsubq_f64( t0_0i.val[0], t0_1i.val[0] );
    t0_0i.val[0] = vmulq_f64( t0_0r.val[0], b_im );
    t0_1i.val[0] = vmulq_f64( t0_1r.val[0], b_re );
    t0_1r.val[0] = vaddq_f64( t0_0i.val[0], t0_1i.val[0] );

    t0_0i.val[1] = vmulq_f64( t0_0r.val[1], b_re );
    t0_1i.val[1] = vmulq_f64( t0_1r.val[1], b_im );
    t0_0r.val[1] = vsubq_f64( t0_0i.val[1], t0_1i.val[1] );
    t0_0i.val[1] = vmulq_f64( t0_0r.val[1], b_im );
    t0_1i.val[1] = vmulq_f64( t0_1r.val[1], b_re );
    t0_1r.val[1] = vaddq_f64( t0_0i.val[1], t0_1i.val[1] );

    t1_0i.val[0] = vmulq_f64( t1_0r.val[0], b_re );
    t1_1i.val[0] = vmulq_f64( t1_1r.val[0], b_im );
    t1_0r.val[0] = vsubq_f64( t1_0i.val[0], t1_1i.val[0] );
    t1_0i.val[0] = vmulq_f64( t1_0r.val[0], b_im );
    t1_1i.val[0] = vmulq_f64( t1_1r.val[0], b_re );
    t1_1r.val[0] = vaddq_f64( t1_0i.val[0], t1_1i.val[0] );

    t1_0i.val[1] = vmulq_f64( t1_0r.val[1], b_re );
    t1_1i.val[1] = vmulq_f64( t1_1r.val[1], b_im );
    t1_0r.val[1] = vsubq_f64( t1_0i.val[1], t1_1i.val[1] );
    t1_0i.val[1] = vmulq_f64( t1_0r.val[1], b_im );
    t1_1i.val[1] = vmulq_f64( t1_1r.val[1], b_re );
    t1_1r.val[1] = vaddq_f64( t1_0i.val[1], t1_1i.val[1] );

    t2_0i.val[0] = vmulq_f64( t2_0r.val[0], b_re );
    t2_1i.val[0] = vmulq_f64( t2_1r.val[0], b_im );
    t2_0r.val[0] = vsubq_f64( t2_0i.val[0], t2_1i.val[0] );
    t2_0i.val[0] = vmulq_f64( t2_0r.val[0], b_im );
    t2_1i.val[0] = vmulq_f64( t2_1r.val[0], b_re );
    t2_1r.val[0] = vaddq_f64( t2_0i.val[0], t2_1i.val[0] );

    t2_0i.val[1] = vmulq_f64( t2_0r.val[1], b_re );
    t2_1i.val[1] = vmulq_f64( t2_1r.val[1], b_im );
    t2_0r.val[1] = vsubq_f64( t2_0i.val[1], t2_1i.val[1] );
    t2_0i.val[1] = vmulq_f64( t2_0r.val[1], b_im );
    t2_1i.val[1] = vmulq_f64( t2_1r.val[1], b_re );
    t2_1r.val[1] = vaddq_f64( t2_0i.val[1], t2_1i.val[1] );

  }

  if (beta.real!=1.0||beta.imag!=0.0) {
    gint_t i;
    for (i=0; i<MR; ++i) {
      gint_t j;
      for (j=0; j<NR; ++j) {               // !!! NOT NRv
        c[i*rs_c+j*cs_c].real = c[i*rs_c+j*cs_c].real*beta.real - c[i*rs_c+j*cs_c].imag*beta.imag;
        c[i*rs_c+j*cs_c].imag = c[i*rs_c+j*cs_c].real*beta.imag + c[i*rs_c+j*cs_c].imag*beta.real;
      }
    }
  }

  p0 = (const double *) &pp0;
  p1 = (const double *) &pp1;
// real part
  pp0[0] = t0_0r.val[0];    // p0 0
  pp0[1] = t0_0r.val[1];    // p0 2
  pp0[2] = t1_0r.val[0];    // p0 4
  pp0[3] = t1_0r.val[1];    // p0 6
  pp0[4] = t2_0r.val[0];    // p0 8
  pp0[5] = t2_0r.val[1];    // p0 10
// imag part
  pp1[0] = t0_1r.val[0];    // p1 0
  pp1[1] = t0_1r.val[1];    // p1 2
  pp1[2] = t1_1r.val[0];    // p1 4
  pp1[3] = t1_1r.val[1];    // p1 6
  pp1[4] = t2_1r.val[0];    // p1 8
  pp1[5] = t2_1r.val[1];    // p1 10

  gint_t pc;

  pc = 0;
  c[pc      ].real += p0[0];
  c[pc      ].imag += p1[0];
  c[pc+=cs_c].real += p0[4];
  c[pc      ].imag += p1[4];
  c[pc+=cs_c].real += p0[8];
  c[pc      ].imag += p1[8];

  pc = rs_c;
  c[pc      ].real += p0[1];
  c[pc      ].imag += p1[1];
  c[pc+=cs_c].real += p0[5];
  c[pc      ].imag += p1[5];
  c[pc+=cs_c].real += p0[9];
  c[pc      ].imag += p1[9];

  pc = 2*rs_c;
  c[pc      ].real += p0[2];
  c[pc      ].imag += p1[2];
  c[pc+=cs_c].real += p0[6];
  c[pc      ].imag += p1[6];
  c[pc+=cs_c].real += p0[10];
  c[pc      ].imag += p1[10];

  pc = 3*rs_c;
  c[pc      ].real += p0[3];
  c[pc      ].imag += p1[3];
  c[pc+=cs_c].real += p0[7];
  c[pc      ].imag += p1[7];
  c[pc+=cs_c].real += p0[11];
  c[pc      ].imag += p1[11];

}

#undef NRv

#undef MC
#undef KC
#undef NC
#undef MR
#undef NR

#endif  /* __aarch64__ */
