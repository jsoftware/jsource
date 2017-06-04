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
#if !((MR==4)&&(NR==4))
#error "invalid MR NR"
#endif

void bli_dgemm_opt_4x4
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

  float64x2_t b0, b1;

  const float64x2_t *vb = (const float64x2_t *)b;
  PREFETCH2((void*)vb);
  PREFETCH2((void*)(vb+1));

  float64x2_t P0_Q0, P0_Q1, P1_Q0, P1_Q1, P2_Q0, P2_Q1, P3_Q0, P3_Q1;
  P0_Q0 = P0_Q1 = P1_Q0 = P1_Q1 = P2_Q0 = P2_Q1 = P3_Q0 = P3_Q1 = vdupq_n_f64(0.0);
  const double *p; float64x2_t pp[2];

  PREFETCH2((void*)&c[0*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+3*cs_c]);

  PREFETCH2((void*)&c[1*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+3*cs_c]);

  PREFETCH2((void*)&c[2*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+3*cs_c]);

  PREFETCH2((void*)&c[3*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[3*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[3*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[3*rs_c+3*cs_c]);

  gint_t l;
  for (l=0; l<k; ++l) {
    PREFETCH((void*)(a+MR));
    float64x2_t a_0 = vld1q_dup_f64(a);
    float64x2_t a_1 = vld1q_dup_f64(a+1);
    float64x2_t a_2 = vld1q_dup_f64(a+2);
    float64x2_t a_3 = vld1q_dup_f64(a+3);

    PREFETCH2((void*)(vb+2));
    b0 = vb[0];

    P0_Q0 = vfmaq_f64(P0_Q0 , a_0, b0);
    P1_Q0 = vfmaq_f64(P1_Q0 , a_1, b0);
    P2_Q0 = vfmaq_f64(P2_Q0 , a_2, b0);
    P3_Q0 = vfmaq_f64(P3_Q0 , a_3, b0);

    PREFETCH2((void*)(vb+3));
    b1 = vb[1];

    P0_Q1 = vfmaq_f64(P0_Q1 , a_0, b1);
    P1_Q1 = vfmaq_f64(P1_Q1 , a_1, b1);
    P2_Q1 = vfmaq_f64(P2_Q1 , a_2, b1);
    P3_Q1 = vfmaq_f64(P3_Q1 , a_3, b1);

    a += MR;
    vb += NRv;
  }

  if (alpha!=1.0) {
    float64x2_t valpha = vld1q_dup_f64(alpha_);
    P0_Q0 = vmulq_f64(P0_Q0, valpha);
    P0_Q1 = vmulq_f64(P0_Q1, valpha);
    P1_Q0 = vmulq_f64(P1_Q0, valpha);
    P1_Q1 = vmulq_f64(P1_Q1, valpha);
    P2_Q0 = vmulq_f64(P2_Q0, valpha);
    P2_Q1 = vmulq_f64(P2_Q1, valpha);
    P3_Q0 = vmulq_f64(P3_Q0, valpha);
    P3_Q1 = vmulq_f64(P3_Q1, valpha);
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

  p = (const double *) &pp;
  pp[0] = P0_Q0;
  pp[1] = P0_Q1;

  c[0*rs_c+0*cs_c] += p[0];
  c[0*rs_c+1*cs_c] += p[1];
  c[0*rs_c+2*cs_c] += p[2];
  c[0*rs_c+3*cs_c] += p[3];

  pp[0] = P1_Q0;
  pp[1] = P1_Q1;

  c[1*rs_c+0*cs_c] += p[0];
  c[1*rs_c+1*cs_c] += p[1];
  c[1*rs_c+2*cs_c] += p[2];
  c[1*rs_c+3*cs_c] += p[3];

  pp[0] = P2_Q0;
  pp[1] = P2_Q1;

  c[2*rs_c+0*cs_c] += p[0];
  c[2*rs_c+1*cs_c] += p[1];
  c[2*rs_c+2*cs_c] += p[2];
  c[2*rs_c+3*cs_c] += p[3];

  pp[0] = P3_Q0;
  pp[1] = P3_Q1;

  c[3*rs_c+0*cs_c] += p[0];
  c[3*rs_c+1*cs_c] += p[1];
  c[3*rs_c+2*cs_c] += p[2];
  c[3*rs_c+3*cs_c] += p[3];

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
#if !((MR==4)&&(NR==4))
#error "invalid MR NR"
#endif

void bli_zgemm_opt_4x4
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

  float64x2x2_t b0, b1;
  float64x2x2_t a_0,a_1,a_2,a_3;

  const double *vb = (const double *)b;
  PREFETCH2((void*)vb);
  PREFETCH2((void*)(vb+2));
  PREFETCH2((void*)(vb+4));
  PREFETCH2((void*)(vb+6));

  float64x2x2_t P0_Q0, P0_Q1, P1_Q0, P1_Q1, P2_Q0, P2_Q1, P3_Q0, P3_Q1;

  P0_Q0.val[0] = P0_Q0.val[1] = P1_Q0.val[0] = P1_Q0.val[1] = \
  P2_Q0.val[0] = P2_Q0.val[1] = P3_Q0.val[0] = P3_Q0.val[1] = \
  P0_Q1.val[0] = P0_Q1.val[1] = P1_Q1.val[0] = P1_Q1.val[1] = \
  P2_Q1.val[0] = P2_Q1.val[1] = P3_Q1.val[0] = P3_Q1.val[1] = vdupq_n_f64(0.0);
  const double *pr,*pi; float64x2_t ppr[2],ppi[2];

  PREFETCH2((void*)&c[0*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+3*cs_c]);

  PREFETCH2((void*)&c[1*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+3*cs_c]);

  PREFETCH2((void*)&c[2*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+3*cs_c]);

  PREFETCH2((void*)&c[3*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[3*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[3*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[3*rs_c+3*cs_c]);

  gint_t l;
  for (l=0; l<k; ++l) {
    PREFETCH((void*)(a+MR));
    PREFETCH((void*)(2+a+MR));
    a_0.val[0] = vld1q_dup_f64((double*)&a[0]);  // assume little endian
    a_0.val[1] = vld1q_dup_f64(1+(double*)&a[0]);
    a_1.val[0] = vld1q_dup_f64((double*)&a[1]);
    a_1.val[1] = vld1q_dup_f64(1+(double*)&a[1]);
    a_2.val[0] = vld1q_dup_f64((double*)&a[2]);
    a_2.val[1] = vld1q_dup_f64(1+(double*)&a[2]);
    a_3.val[0] = vld1q_dup_f64((double*)&a[3]);
    a_3.val[1] = vld1q_dup_f64(1+(double*)&a[3]);

    PREFETCH2((void*)(vb+8));
    PREFETCH2((void*)(vb+10));
    b0 = vld2q_f64(vb);

    P0_Q0.val[0] = vfmaq_f64(P0_Q0.val[0] , a_0.val[0], b0.val[0]);
    P1_Q0.val[0] = vfmaq_f64(P1_Q0.val[0] , a_1.val[0], b0.val[0]);
    P2_Q0.val[0] = vfmaq_f64(P2_Q0.val[0] , a_2.val[0], b0.val[0]);
    P3_Q0.val[0] = vfmaq_f64(P3_Q0.val[0] , a_3.val[0], b0.val[0]);

    P0_Q0.val[0] = vfmsq_f64(P0_Q0.val[0] , a_0.val[1], b0.val[1]);
    P1_Q0.val[0] = vfmsq_f64(P1_Q0.val[0] , a_1.val[1], b0.val[1]);
    P2_Q0.val[0] = vfmsq_f64(P2_Q0.val[0] , a_2.val[1], b0.val[1]);
    P3_Q0.val[0] = vfmsq_f64(P3_Q0.val[0] , a_3.val[1], b0.val[1]);

    P0_Q0.val[1] = vfmaq_f64(P0_Q0.val[1] , a_0.val[0], b0.val[1]);
    P1_Q0.val[1] = vfmaq_f64(P1_Q0.val[1] , a_1.val[0], b0.val[1]);
    P2_Q0.val[1] = vfmaq_f64(P2_Q0.val[1] , a_2.val[0], b0.val[1]);
    P3_Q0.val[1] = vfmaq_f64(P3_Q0.val[1] , a_3.val[0], b0.val[1]);

    P0_Q0.val[1] = vfmaq_f64(P0_Q0.val[1] , a_0.val[1], b0.val[0]);
    P1_Q0.val[1] = vfmaq_f64(P1_Q0.val[1] , a_1.val[1], b0.val[0]);
    P2_Q0.val[1] = vfmaq_f64(P2_Q0.val[1] , a_2.val[1], b0.val[0]);
    P3_Q0.val[1] = vfmaq_f64(P3_Q0.val[1] , a_3.val[1], b0.val[0]);

    PREFETCH2((void*)(vb+12));
    PREFETCH2((void*)(vb+14));
    b1 = vld2q_f64(vb+4);

    P0_Q1.val[0] = vfmaq_f64(P0_Q1.val[0] , a_0.val[0], b1.val[0]);
    P1_Q1.val[0] = vfmaq_f64(P1_Q1.val[0] , a_1.val[0], b1.val[0]);
    P2_Q1.val[0] = vfmaq_f64(P2_Q1.val[0] , a_2.val[0], b1.val[0]);
    P3_Q1.val[0] = vfmaq_f64(P3_Q1.val[0] , a_3.val[0], b1.val[0]);

    P0_Q1.val[0] = vfmsq_f64(P0_Q1.val[0] , a_0.val[1], b1.val[1]);
    P1_Q1.val[0] = vfmsq_f64(P1_Q1.val[0] , a_1.val[1], b1.val[1]);
    P2_Q1.val[0] = vfmsq_f64(P2_Q1.val[0] , a_2.val[1], b1.val[1]);
    P3_Q1.val[0] = vfmsq_f64(P3_Q1.val[0] , a_3.val[1], b1.val[1]);

    P0_Q1.val[1] = vfmaq_f64(P0_Q1.val[1] , a_0.val[0], b1.val[1]);
    P1_Q1.val[1] = vfmaq_f64(P1_Q1.val[1] , a_1.val[0], b1.val[1]);
    P2_Q1.val[1] = vfmaq_f64(P2_Q1.val[1] , a_2.val[0], b1.val[1]);
    P3_Q1.val[1] = vfmaq_f64(P3_Q1.val[1] , a_3.val[0], b1.val[1]);

    P0_Q1.val[1] = vfmaq_f64(P0_Q1.val[1] , a_0.val[1], b1.val[0]);
    P1_Q1.val[1] = vfmaq_f64(P1_Q1.val[1] , a_1.val[1], b1.val[0]);
    P2_Q1.val[1] = vfmaq_f64(P2_Q1.val[1] , a_2.val[1], b1.val[0]);
    P3_Q1.val[1] = vfmaq_f64(P3_Q1.val[1] , a_3.val[1], b1.val[0]);

    a += MR;
    vb += 8;       // vb is pointer to double
  }

  if (alpha.real!=1.0||alpha.imag!=0.0) {
    float64x2x2_t valpha;
    valpha.val[0] = vld1q_dup_f64((double*)alpha_);  // assume little endian
    valpha.val[1] = vld1q_dup_f64(1+(double*)alpha_);

    P0_Q0.val[0] = vfmaq_f64(P0_Q0.val[0] , a_0.val[0], valpha.val[0]);
    P1_Q0.val[0] = vfmaq_f64(P1_Q0.val[0] , a_1.val[0], valpha.val[0]);
    P2_Q0.val[0] = vfmaq_f64(P2_Q0.val[0] , a_2.val[0], valpha.val[0]);
    P3_Q0.val[0] = vfmaq_f64(P3_Q0.val[0] , a_3.val[0], valpha.val[0]);

    P0_Q0.val[0] = vfmsq_f64(P0_Q0.val[0] , a_0.val[1], valpha.val[1]);
    P1_Q0.val[0] = vfmsq_f64(P1_Q0.val[0] , a_1.val[1], valpha.val[1]);
    P2_Q0.val[0] = vfmsq_f64(P2_Q0.val[0] , a_2.val[1], valpha.val[1]);
    P3_Q0.val[0] = vfmsq_f64(P3_Q0.val[0] , a_3.val[1], valpha.val[1]);

    P0_Q0.val[1] = vfmaq_f64(P0_Q0.val[1] , a_0.val[0], valpha.val[1]);
    P1_Q0.val[1] = vfmaq_f64(P1_Q0.val[1] , a_1.val[0], valpha.val[1]);
    P2_Q0.val[1] = vfmaq_f64(P2_Q0.val[1] , a_2.val[0], valpha.val[1]);
    P3_Q0.val[1] = vfmaq_f64(P3_Q0.val[1] , a_3.val[0], valpha.val[1]);

    P0_Q0.val[1] = vfmaq_f64(P0_Q0.val[1] , a_0.val[1], valpha.val[0]);
    P1_Q0.val[1] = vfmaq_f64(P1_Q0.val[1] , a_1.val[1], valpha.val[0]);
    P2_Q0.val[1] = vfmaq_f64(P2_Q0.val[1] , a_2.val[1], valpha.val[0]);
    P3_Q0.val[1] = vfmaq_f64(P3_Q0.val[1] , a_3.val[1], valpha.val[0]);

    P0_Q1.val[0] = vfmaq_f64(P0_Q1.val[0] , a_0.val[0], valpha.val[0]);
    P1_Q1.val[0] = vfmaq_f64(P1_Q1.val[0] , a_1.val[0], valpha.val[0]);
    P2_Q1.val[0] = vfmaq_f64(P2_Q1.val[0] , a_2.val[0], valpha.val[0]);
    P3_Q1.val[0] = vfmaq_f64(P3_Q1.val[0] , a_3.val[0], valpha.val[0]);

    P0_Q1.val[0] = vfmsq_f64(P0_Q1.val[0] , a_0.val[1], valpha.val[1]);
    P1_Q1.val[0] = vfmsq_f64(P1_Q1.val[0] , a_1.val[1], valpha.val[1]);
    P2_Q1.val[0] = vfmsq_f64(P2_Q1.val[0] , a_2.val[1], valpha.val[1]);
    P3_Q1.val[0] = vfmsq_f64(P3_Q1.val[0] , a_3.val[1], valpha.val[1]);

    P0_Q1.val[1] = vfmaq_f64(P0_Q1.val[1] , a_0.val[0], valpha.val[1]);
    P1_Q1.val[1] = vfmaq_f64(P1_Q1.val[1] , a_1.val[0], valpha.val[1]);
    P2_Q1.val[1] = vfmaq_f64(P2_Q1.val[1] , a_2.val[0], valpha.val[1]);
    P3_Q1.val[1] = vfmaq_f64(P3_Q1.val[1] , a_3.val[0], valpha.val[1]);

    P0_Q1.val[1] = vfmaq_f64(P0_Q1.val[1] , a_0.val[1], valpha.val[0]);
    P1_Q1.val[1] = vfmaq_f64(P1_Q1.val[1] , a_1.val[1], valpha.val[0]);
    P2_Q1.val[1] = vfmaq_f64(P2_Q1.val[1] , a_2.val[1], valpha.val[0]);
    P3_Q1.val[1] = vfmaq_f64(P3_Q1.val[1] , a_3.val[1], valpha.val[0]);

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

  pr = (const double *) &ppr;
  pi = (const double *) &ppi;
  ppr[0] = P0_Q0.val[0];
  ppi[0] = P0_Q0.val[1];
  ppr[1] = P0_Q1.val[0];
  ppi[1] = P0_Q1.val[1];

  c[0*rs_c+0*cs_c].real += pr[0];
  c[0*rs_c+0*cs_c].imag += pi[0];
  c[0*rs_c+1*cs_c].real += pr[1];
  c[0*rs_c+1*cs_c].imag += pi[1];
  c[0*rs_c+2*cs_c].real += pr[2];
  c[0*rs_c+2*cs_c].imag += pi[2];
  c[0*rs_c+3*cs_c].real += pr[3];
  c[0*rs_c+3*cs_c].imag += pi[3];

  ppr[0] = P1_Q0.val[0];
  ppi[0] = P1_Q0.val[1];
  ppr[1] = P1_Q1.val[0];
  ppi[1] = P1_Q1.val[1];

  c[1*rs_c+0*cs_c].real += pr[0];
  c[1*rs_c+0*cs_c].imag += pi[0];
  c[1*rs_c+1*cs_c].real += pr[1];
  c[1*rs_c+1*cs_c].imag += pi[1];
  c[1*rs_c+2*cs_c].real += pr[2];
  c[1*rs_c+2*cs_c].imag += pi[2];
  c[1*rs_c+3*cs_c].real += pr[3];
  c[1*rs_c+3*cs_c].imag += pi[3];

  ppr[0] = P2_Q0.val[0];
  ppi[0] = P2_Q0.val[1];
  ppr[1] = P2_Q1.val[0];
  ppi[1] = P2_Q1.val[1];

  c[2*rs_c+0*cs_c].real += pr[0];
  c[2*rs_c+0*cs_c].imag += pi[0];
  c[2*rs_c+1*cs_c].real += pr[1];
  c[2*rs_c+1*cs_c].imag += pi[1];
  c[2*rs_c+2*cs_c].real += pr[2];
  c[2*rs_c+2*cs_c].imag += pi[2];
  c[2*rs_c+3*cs_c].real += pr[3];
  c[2*rs_c+3*cs_c].imag += pi[3];

  ppr[0] = P3_Q0.val[0];
  ppi[0] = P3_Q0.val[1];
  ppr[1] = P3_Q1.val[0];
  ppi[1] = P3_Q1.val[1];

  c[3*rs_c+0*cs_c].real += pr[0];
  c[3*rs_c+0*cs_c].imag += pi[0];
  c[3*rs_c+1*cs_c].real += pr[1];
  c[3*rs_c+1*cs_c].imag += pi[1];
  c[3*rs_c+2*cs_c].real += pr[2];
  c[3*rs_c+2*cs_c].imag += pi[2];
  c[3*rs_c+3*cs_c].real += pr[3];
  c[3*rs_c+3*cs_c].imag += pi[3];

}

#undef NRv

#undef MC
#undef KC
#undef NC
#undef MR
#undef NR

#endif  /* __aarch64__ */
