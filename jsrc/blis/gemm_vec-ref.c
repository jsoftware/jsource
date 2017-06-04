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

#if SIMD_REGISTER_WIDTH==128    // 128-bit simd

#if 1 // 128_4x4

#define NRv (NR/2)              // 2 lanes
#if !((MR==4)&&(NR==4))
#error "invalid MR NR"
#endif

void bli_dgemm_128_4x4
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
  typedef double vx __attribute__((vector_size (16)));     // (rwidth/8)

  a = (double*) __builtin_assume_aligned (a, alignv);
  b = (double*) __builtin_assume_aligned (b, alignv);

  vx b0, b1;

  const vx *vb = (const vx *)b;
  b0 = vb[0];

  vx P0_Q0 = {};
  vx P0_Q1 = {};
  vx P1_Q0 = {};
  vx P1_Q1 = {};
  vx P2_Q0 = {};
  vx P2_Q1 = {};
  vx P3_Q0 = {};
  vx P3_Q1 = {};
  const double *p; vx pp[2];

  gint_t l;
  for (l=0; l<k; ++l) {
    vx a_0 =(vx) {
      a[0],a[0]
    };
    vx a_1 =(vx) {
      a[1],a[1]
    };
    vx a_2 =(vx) {
      a[2],a[2]
    };
    vx a_3 =(vx) {
      a[3],a[3]
    };

    b1 = vb[1];

    P0_Q0 += a_0*b0;
    P1_Q0 += a_1*b0;
    P2_Q0 += a_2*b0;
    P3_Q0 += a_3*b0;

    b0 = vb[2];

    P0_Q1 += a_0*b1;
    P1_Q1 += a_1*b1;
    P2_Q1 += a_2*b1;
    P3_Q1 += a_3*b1;

    a += MR;
    vb += NRv;
  }

  if (alpha!=1.0) {
    vx valpha =(vx) {
      alpha,alpha
    };

    P0_Q0 *= valpha;
    P0_Q1 *= valpha;
    P1_Q0 *= valpha;
    P1_Q1 *= valpha;
    P2_Q0 *= valpha;
    P2_Q1 *= valpha;
    P3_Q0 *= valpha;
    P3_Q1 *= valpha;
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

#endif // 128_4x4

#elif SIMD_REGISTER_WIDTH==256    // 256-bit simd

#if 1 // 256_4x8

#define NRv (NR/4)           // 4 lanes
#if !((MR==4)&&(NR==8))
#error "invalid MR NR"
#endif

void bli_dgemm_256_4x8
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
  typedef double vx __attribute__((vector_size (32)));     // (rwidth/8)

  a = (double*) __builtin_assume_aligned (a, alignv);
  b = (double*) __builtin_assume_aligned (b, alignv);

  vx b0, b1;

  const vx *vb = (const vx *)b;
  b0 = vb[0];

  vx P0_Q0 = {};
  vx P0_Q1 = {};
  vx P1_Q0 = {};
  vx P1_Q1 = {};
  vx P2_Q0 = {};
  vx P2_Q1 = {};
  vx P3_Q0 = {};
  vx P3_Q1 = {};
  const double *p; vx pp[2];

  gint_t l;
  for (l=0; l<k; ++l) {
    vx a_0 =(vx) {
      a[0],a[0],a[0],a[0]
    };
    vx a_1 =(vx) {
      a[1],a[1],a[1],a[1]
    };
    vx a_2 =(vx) {
      a[2],a[2],a[2],a[2]
    };
    vx a_3 =(vx) {
      a[3],a[3],a[3],a[3]
    };

    b1 = vb[1];

    P0_Q0 += a_0*b0;
    P1_Q0 += a_1*b0;
    P2_Q0 += a_2*b0;
    P3_Q0 += a_3*b0;

    b0 = vb[2];

    P0_Q1 += a_0*b1;
    P1_Q1 += a_1*b1;
    P2_Q1 += a_2*b1;
    P3_Q1 += a_3*b1;

    a += MR;
    vb += NRv;
  }

  if (alpha!=1.0) {
    vx valpha =(vx) {
      alpha,alpha,alpha,alpha
    };

    P0_Q0 *= valpha;
    P0_Q1 *= valpha;
    P1_Q0 *= valpha;
    P1_Q1 *= valpha;
    P2_Q0 *= valpha;
    P2_Q1 *= valpha;
    P3_Q0 *= valpha;
    P3_Q1 *= valpha;
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
  c[0*rs_c+4*cs_c] += p[4];
  c[0*rs_c+5*cs_c] += p[5];
  c[0*rs_c+6*cs_c] += p[6];
  c[0*rs_c+7*cs_c] += p[7];

  pp[0] = P1_Q0;
  pp[1] = P1_Q1;

  c[1*rs_c+0*cs_c] += p[0];
  c[1*rs_c+1*cs_c] += p[1];
  c[1*rs_c+2*cs_c] += p[2];
  c[1*rs_c+3*cs_c] += p[3];
  c[1*rs_c+4*cs_c] += p[4];
  c[1*rs_c+5*cs_c] += p[5];
  c[1*rs_c+6*cs_c] += p[6];
  c[1*rs_c+7*cs_c] += p[7];

  pp[0] = P2_Q0;
  pp[1] = P2_Q1;

  c[2*rs_c+0*cs_c] += p[0];
  c[2*rs_c+1*cs_c] += p[1];
  c[2*rs_c+2*cs_c] += p[2];
  c[2*rs_c+3*cs_c] += p[3];
  c[2*rs_c+4*cs_c] += p[4];
  c[2*rs_c+5*cs_c] += p[5];
  c[2*rs_c+6*cs_c] += p[6];
  c[2*rs_c+7*cs_c] += p[7];

  pp[0] = P3_Q0;
  pp[1] = P3_Q1;

  c[3*rs_c+0*cs_c] += p[0];
  c[3*rs_c+1*cs_c] += p[1];
  c[3*rs_c+2*cs_c] += p[2];
  c[3*rs_c+3*cs_c] += p[3];
  c[3*rs_c+4*cs_c] += p[4];
  c[3*rs_c+5*cs_c] += p[5];
  c[3*rs_c+6*cs_c] += p[6];
  c[3*rs_c+7*cs_c] += p[7];

}

#endif // 256_4x8

#else

#error "only 128/256 simd are supported"

#endif


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

#if SIMD_REGISTER_WIDTH==128    // 128-bit simd

#if 0 // 128_4x2

#define NRv (NR/2)            // 2 lanes
#if !((MR==4)&&(NR==2))
#error "invalid MR NR"
#endif

void bli_zgemm_128_4x2
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
  a = (dcomplex*) __builtin_assume_aligned (a, alignv);
  b = (dcomplex*) __builtin_assume_aligned (b, alignv);

  vx b0re, b0im, b1re, b1im;
  vx a_0re,a_0im,a_1re,a_1im;

  const double *vb = (const double *)b;

  vx P0_Q0re = {};
  vx P1_Q0re = {};
  vx P2_Q0re = {};
  vx P3_Q0re = {};

  vx P0_Q0im = {};
  vx P1_Q0im = {};
  vx P2_Q0im = {};
  vx P3_Q0im = {};
  const double *pr,*pi; vx ppr[1],ppi[1];

  gint_t l;
  for (l=0; l<k; ++l) {
    b0re = (vx) {
      vb[0],vb[2]
    };
    b0im = (vx) {
      vb[1],vb[3]
    };

    a_0re = (vx) {
      a[0].real,a[0].real
    };
    a_0im = (vx) {
      a[0].imag,a[0].imag
    };
    a_1re = (vx) {
      a[1].real,a[1].real
    };
    a_1im = (vx) {
      a[1].imag,a[1].imag
    };
    a_2re = (vx) {
      a[2].real,a[2].real
    };
    a_2im = (vx) {
      a[2].imag,a[2].imag
    };
    a_3re = (vx) {
      a[3].real,a[3].real
    };
    a_3im = (vx) {
      a[3].imag,a[3].imag
    };

    P0_Q0re += a_0re*b0re;
    P1_Q0re += a_1re*b0re;
    P2_Q0re += a_2re*b0re;
    P3_Q0re += a_3re*b0re;

    P0_Q0re -= a_0im*b0im;
    P1_Q0re -= a_1im*b0im;
    P2_Q0re -= a_2im*b0im;
    P3_Q0re -= a_3im*b0im;

    P0_Q0im += a_0re*b0im;
    P1_Q0im += a_1re*b0im;
    P2_Q0im += a_2re*b0im;
    P3_Q0im += a_3re*b0im;

    P0_Q0im += a_0im*b0re;
    P1_Q0im += a_1im*b0re;
    P2_Q0im += a_2im*b0re;
    P3_Q0im += a_3im*b0re;

    a += MR;
    vb += 4;       // vb is pointer to double
  }

  if (alpha.real!=1.0||alpha.imag!=0.0) {
    vx valpha_re = (vx) {
      alpha.real,alpha.real
    };
    vx valpha_im = (vx) {
      alpha.imag,alpha.imag
    };

    P0_Q0re = P0_Q0re*valpha_re - P0_Q0im*valpha_im;
    P0_Q0im = P0_Q0re*valpha_im + P0_Q0im*valpha_re;

    P1_Q0re = P1_Q0re*valpha_re - P1_Q0im*valpha_im;
    P1_Q0im = P1_Q0re*valpha_im + P1_Q0im*valpha_re;

    P2_Q0re = P2_Q0re*valpha_re - P2_Q0im*valpha_im;
    P2_Q0im = P2_Q0re*valpha_im + P2_Q0im*valpha_re;

    P3_Q0re = P3_Q0re*valpha_re - P3_Q0im*valpha_im;
    P3_Q0im = P3_Q0re*valpha_im + P3_Q0im*valpha_re;
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
  ppr[0] = P0_Q0re;
  ppi[0] = P0_Q0im;

  c[0*rs_c+0*cs_c].real += pr[0];
  c[0*rs_c+0*cs_c].imag += pi[0];
  c[0*rs_c+1*cs_c].real += pr[1];
  c[0*rs_c+1*cs_c].imag += pi[1];

  ppr[0] = P1_Q0re;
  ppi[0] = P1_Q0im;

  c[1*rs_c+0*cs_c].real += pr[0];
  c[1*rs_c+0*cs_c].imag += pi[0];
  c[1*rs_c+1*cs_c].real += pr[1];
  c[1*rs_c+1*cs_c].imag += pi[1];

  ppr[0] = P2_Q0re;
  ppi[0] = P2_Q0im;

  c[2*rs_c+0*cs_c].real += pr[0];
  c[2*rs_c+0*cs_c].imag += pi[0];
  c[2*rs_c+1*cs_c].real += pr[1];
  c[2*rs_c+1*cs_c].imag += pi[1];

  ppr[0] = P3_Q0re;
  ppi[0] = P3_Q0im;

  c[3*rs_c+0*cs_c].real += pr[0];
  c[3*rs_c+0*cs_c].imag += pi[0];
  c[3*rs_c+1*cs_c].real += pr[1];
  c[3*rs_c+1*cs_c].imag += pi[1];

}

#endif // 128_4x2

#if 1 // 128_4x4

#define NRv (NR/4)
#if !((MR==4)&&(NR==4))
#error "invalid MR NR"
#endif

void bli_zgemm_128_4x4
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
  typedef double vx __attribute__((vector_size (16)));     // (rwidth/8)

  a = (dcomplex*) __builtin_assume_aligned (a, alignv);
  b = (dcomplex*) __builtin_assume_aligned (b, alignv);

  vx b0re, b1re, b0im, b1im;
  vx a_0re,a_0im,a_1re,a_1im,a_2re,a_2im,a_3re,a_3im;

  const double *vb = (const double *)b;
  b0re = (vx) {
    vb[0],vb[2]
  };
  b0im = (vx) {
    vb[1],vb[3]
  };

  vx P0_Q0re = {};
  vx P0_Q1re = {};
  vx P1_Q0re = {};
  vx P1_Q1re = {};
  vx P2_Q0re = {};
  vx P2_Q1re = {};
  vx P3_Q0re = {};
  vx P3_Q1re = {};

  vx P0_Q0im = {};
  vx P0_Q1im = {};
  vx P1_Q0im = {};
  vx P1_Q1im = {};
  vx P2_Q0im = {};
  vx P2_Q1im = {};
  vx P3_Q0im = {};
  vx P3_Q1im = {};
  const double *pr,*pi; vx ppr[2],ppi[2];

  gint_t l;
  for (l=0; l<k; ++l) {
    a_0re = (vx) {
      a[0].real,a[0].real
    };
    a_0im = (vx) {
      a[0].imag,a[0].imag
    };
    a_1re = (vx) {
      a[1].real,a[1].real
    };
    a_1im = (vx) {
      a[1].imag,a[1].imag
    };
    a_2re = (vx) {
      a[2].real,a[2].real
    };
    a_2im = (vx) {
      a[2].imag,a[2].imag
    };
    a_3re = (vx) {
      a[3].real,a[3].real
    };
    a_3im = (vx) {
      a[3].imag,a[3].imag
    };

    b1re = (vx) {
      vb[4],vb[6]
    };
    b1im = (vx) {
      vb[5],vb[7]
    };

    P0_Q0re += a_0re*b0re;
    P1_Q0re += a_1re*b0re;
    P2_Q0re += a_2re*b0re;
    P3_Q0re += a_3re*b0re;

    P0_Q0re -= a_0im*b0im;
    P1_Q0re -= a_1im*b0im;
    P2_Q0re -= a_2im*b0im;
    P3_Q0re -= a_3im*b0im;

    P0_Q0im += a_0re*b0im;
    P1_Q0im += a_1re*b0im;
    P2_Q0im += a_2re*b0im;
    P3_Q0im += a_3re*b0im;

    P0_Q0im += a_0im*b0re;
    P1_Q0im += a_1im*b0re;
    P2_Q0im += a_2im*b0re;
    P3_Q0im += a_3im*b0re;

    b0re = (vx) {
      vb[8],vb[10]
    };
    b0im = (vx) {
      vb[9],vb[11]
    };

    P0_Q1re += a_0re*b1re;
    P1_Q1re += a_1re*b1re;
    P2_Q1re += a_2re*b1re;
    P3_Q1re += a_3re*b1re;

    P0_Q1re -= a_0im*b1im;
    P1_Q1re -= a_1im*b1im;
    P2_Q1re -= a_2im*b1im;
    P3_Q1re -= a_3im*b1im;

    P0_Q1im += a_0re*b1im;
    P1_Q1im += a_1re*b1im;
    P2_Q1im += a_2re*b1im;
    P3_Q1im += a_3re*b1im;

    P0_Q1im += a_0im*b1re;
    P1_Q1im += a_1im*b1re;
    P2_Q1im += a_2im*b1re;
    P3_Q1im += a_3im*b1re;

    a += MR;
    vb += 8;       // vb is pointer to double
  }

  if (alpha.real!=1.0||alpha.imag!=0.0) {
    vx valpha_re = (vx) {
      alpha.real,alpha.real
    };
    vx valpha_im = (vx) {
      alpha.imag,alpha.imag
    };

    P0_Q0re = P0_Q0re*valpha_re - P0_Q0im*valpha_im;
    P0_Q0im = P0_Q0re*valpha_im + P0_Q0im*valpha_re;
    P0_Q1re = P0_Q1re*valpha_re - P0_Q1im*valpha_im;
    P0_Q1im = P0_Q1re*valpha_im + P0_Q1im*valpha_re;

    P1_Q0re = P1_Q0re*valpha_re - P1_Q0im*valpha_im;
    P1_Q0im = P1_Q0re*valpha_im + P1_Q0im*valpha_re;
    P1_Q1re = P1_Q1re*valpha_re - P1_Q1im*valpha_im;
    P1_Q1im = P1_Q1re*valpha_im + P1_Q1im*valpha_re;

    P2_Q0re = P2_Q0re*valpha_re - P2_Q0im*valpha_im;
    P2_Q0im = P2_Q0re*valpha_im + P2_Q0im*valpha_re;
    P2_Q1re = P2_Q1re*valpha_re - P2_Q1im*valpha_im;
    P2_Q1im = P2_Q1re*valpha_im + P2_Q1im*valpha_re;

    P3_Q0re = P3_Q0re*valpha_re - P3_Q0im*valpha_im;
    P3_Q0im = P3_Q0re*valpha_im + P3_Q0im*valpha_re;
    P3_Q1re = P3_Q1re*valpha_re - P3_Q1im*valpha_im;
    P3_Q1im = P3_Q1re*valpha_im + P3_Q1im*valpha_re;
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
  ppr[0] = P0_Q0re;
  ppi[0] = P0_Q0im;
  ppr[1] = P0_Q1re;
  ppi[1] = P0_Q1im;

  c[0*rs_c+0*cs_c].real += pr[0];
  c[0*rs_c+0*cs_c].imag += pi[0];
  c[0*rs_c+1*cs_c].real += pr[1];
  c[0*rs_c+1*cs_c].imag += pi[1];
  c[0*rs_c+2*cs_c].real += pr[2];
  c[0*rs_c+2*cs_c].imag += pi[2];
  c[0*rs_c+3*cs_c].real += pr[3];
  c[0*rs_c+3*cs_c].imag += pi[3];

  ppr[0] = P1_Q0re;
  ppi[0] = P1_Q0im;
  ppr[1] = P1_Q1re;
  ppi[1] = P1_Q1im;

  c[1*rs_c+0*cs_c].real += pr[0];
  c[1*rs_c+0*cs_c].imag += pi[0];
  c[1*rs_c+1*cs_c].real += pr[1];
  c[1*rs_c+1*cs_c].imag += pi[1];
  c[1*rs_c+2*cs_c].real += pr[2];
  c[1*rs_c+2*cs_c].imag += pi[2];
  c[1*rs_c+3*cs_c].real += pr[3];
  c[1*rs_c+3*cs_c].imag += pi[3];

  ppr[0] = P2_Q0re;
  ppi[0] = P2_Q0im;
  ppr[1] = P2_Q1re;
  ppi[1] = P2_Q1im;

  c[2*rs_c+0*cs_c].real += pr[0];
  c[2*rs_c+0*cs_c].imag += pi[0];
  c[2*rs_c+1*cs_c].real += pr[1];
  c[2*rs_c+1*cs_c].imag += pi[1];
  c[2*rs_c+2*cs_c].real += pr[2];
  c[2*rs_c+2*cs_c].imag += pi[2];
  c[2*rs_c+3*cs_c].real += pr[3];
  c[2*rs_c+3*cs_c].imag += pi[3];

  ppr[0] = P3_Q0re;
  ppi[0] = P3_Q0im;
  ppr[1] = P3_Q1re;
  ppi[1] = P3_Q1im;

  c[3*rs_c+0*cs_c].real += pr[0];
  c[3*rs_c+0*cs_c].imag += pi[0];
  c[3*rs_c+1*cs_c].real += pr[1];
  c[3*rs_c+1*cs_c].imag += pi[1];
  c[3*rs_c+2*cs_c].real += pr[2];
  c[3*rs_c+2*cs_c].imag += pi[2];
  c[3*rs_c+3*cs_c].real += pr[3];
  c[3*rs_c+3*cs_c].imag += pi[3];

}

#endif // 128_4x4

#elif SIMD_REGISTER_WIDTH==256    // 256-bit simd

#if 0 // 256_4x4

#define NRv (NR/4)          // 4 lanes
#if !((MR==4)&&(NR==4))
#error "invalid MR NR"
#endif

void bli_zgemm_256_4x4
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
  typedef double vx __attribute__((vector_size (32)));     // (rwidth/8)

  a = (dcomplex*) __builtin_assume_aligned (a, alignv);
  b = (dcomplex*) __builtin_assume_aligned (b, alignv);

  vx b0re, b0im;
  vx a_0re,a_0im,a_1re,a_1im,a_2re,a_2im,a_3re,a_3im;

  const double *vb = (const double *)b;

  vx P0_Q0re = {};
  vx P1_Q0re = {};
  vx P2_Q0re = {};
  vx P3_Q0re = {};

  vx P0_Q0im = {};
  vx P1_Q0im = {};
  vx P2_Q0im = {};
  vx P3_Q0im = {};
  const double *pr,*pi; vx ppr[1],ppi[1];

  gint_t l;
  for (l=0; l<k; ++l) {
    a_0re = (vx) {
      a[0].real,a[0].real,a[0].real,a[0].real
    };
    a_0im = (vx) {
      a[0].imag,a[0].imag,a[0].imag,a[0].imag
    };
    a_1re = (vx) {
      a[1].real,a[1].real,a[1].real,a[1].real
    };
    a_1im = (vx) {
      a[1].imag,a[1].imag,a[1].imag,a[1].imag
    };
    a_2re = (vx) {
      a[2].real,a[2].real,a[2].real,a[2].real
    };
    a_2im = (vx) {
      a[2].imag,a[2].imag,a[2].imag,a[2].imag
    };
    a_3re = (vx) {
      a[3].real,a[3].real,a[3].real,a[3].real
    };
    a_3im = (vx) {
      a[3].imag,a[3].imag,a[3].imag,a[3].imag
    };

    b0re = (vx) {
      vb[0],vb[2],vb[4],vb[6]
    };
    b0im = (vx) {
      vb[1],vb[3],vb[5],vb[7]
    };

    P0_Q0re += a_0re*b0re;
    P1_Q0re += a_1re*b0re;
    P2_Q0re += a_2re*b0re;
    P3_Q0re += a_3re*b0re;

    P0_Q0re -= a_0im*b0im;
    P1_Q0re -= a_1im*b0im;
    P2_Q0re -= a_2im*b0im;
    P3_Q0re -= a_3im*b0im;

    P0_Q0im += a_0re*b0im;
    P1_Q0im += a_1re*b0im;
    P2_Q0im += a_2re*b0im;
    P3_Q0im += a_3re*b0im;

    P0_Q0im += a_0im*b0re;
    P1_Q0im += a_1im*b0re;
    P2_Q0im += a_2im*b0re;
    P3_Q0im += a_3im*b0re;

    a += MR;
    vb += 8;       // vb is pointer to double
  }

  if (alpha.real!=1.0||alpha.imag!=0.0) {
    vx valpha_re = (vx) {
      alpha.real,alpha.real,alpha.real,alpha.real
    };
    vx valpha_im = (vx) {
      alpha.imag,alpha.imag,alpha.imag,alpha.imag
    };

    P0_Q0re = P0_Q0re*valpha_re - P0_Q0im*valpha_im;
    P0_Q0im = P0_Q0re*valpha_im + P0_Q0im*valpha_re;

    P1_Q0re = P1_Q0re*valpha_re - P1_Q0im*valpha_im;
    P1_Q0im = P1_Q0re*valpha_im + P1_Q0im*valpha_re;

    P2_Q0re = P2_Q0re*valpha_re - P2_Q0im*valpha_im;
    P2_Q0im = P2_Q0re*valpha_im + P2_Q0im*valpha_re;

    P3_Q0re = P3_Q0re*valpha_re - P3_Q0im*valpha_im;
    P3_Q0im = P3_Q0re*valpha_im + P3_Q0im*valpha_re;
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
  ppr[0] = P0_Q0re;
  ppi[0] = P0_Q0im;

  c[0*rs_c+0*cs_c].real += pr[0];
  c[0*rs_c+0*cs_c].imag += pi[0];
  c[0*rs_c+1*cs_c].real += pr[1];
  c[0*rs_c+1*cs_c].imag += pi[1];

  ppr[0] = P1_Q0re;
  ppi[0] = P1_Q0im;

  c[1*rs_c+0*cs_c].real += pr[0];
  c[1*rs_c+0*cs_c].imag += pi[0];
  c[1*rs_c+1*cs_c].real += pr[1];
  c[1*rs_c+1*cs_c].imag += pi[1];

  ppr[0] = P2_Q0re;
  ppi[0] = P2_Q0im;

  c[2*rs_c+0*cs_c].real += pr[0];
  c[2*rs_c+0*cs_c].imag += pi[0];
  c[2*rs_c+1*cs_c].real += pr[1];
  c[2*rs_c+1*cs_c].imag += pi[1];

  ppr[0] = P3_Q0re;
  ppi[0] = P3_Q0im;

  c[3*rs_c+0*cs_c].real += pr[0];
  c[3*rs_c+0*cs_c].imag += pi[0];
  c[3*rs_c+1*cs_c].real += pr[1];
  c[3*rs_c+1*cs_c].imag += pi[1];

}

#endif // 256_4x4

#if 1 // 256_4x8

#define NRv (NR/4)              // 4 lanes
#if !((MR==4)&&(NR==8))
#error "invalid MR NR"
#endif

void bli_zgemm_256_4x8
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
  typedef double vx __attribute__((vector_size (32)));     // (rwidth/8)

  a = (dcomplex*) __builtin_assume_aligned (a, alignv);
  b = (dcomplex*) __builtin_assume_aligned (b, alignv);

  vx b0re, b1re, b0im, b1im;
  vx a_0re,a_0im,a_1re,a_1im,a_2re,a_2im,a_3re,a_3im;

  const double *vb = (const double *)b;
  b0re = (vx) {
    vb[0],vb[2],vb[4],vb[6]
  };
  b0im = (vx) {
    vb[1],vb[3],vb[5],vb[7]
  };

  vx P0_Q0re = {};
  vx P0_Q1re = {};
  vx P1_Q0re = {};
  vx P1_Q1re = {};
  vx P2_Q0re = {};
  vx P2_Q1re = {};
  vx P3_Q0re = {};
  vx P3_Q1re = {};

  vx P0_Q0im = {};
  vx P0_Q1im = {};
  vx P1_Q0im = {};
  vx P1_Q1im = {};
  vx P2_Q0im = {};
  vx P2_Q1im = {};
  vx P3_Q0im = {};
  vx P3_Q1im = {};
  const double *pr,*pi; vx ppr[2],ppi[2];

  gint_t l;
  for (l=0; l<k; ++l) {
    a_0re = (vx) {
      a[0].real,a[0].real,a[0].real,a[0].real
    };
    a_0im = (vx) {
      a[0].imag,a[0].imag,a[0].imag,a[0].imag
    };
    a_1re = (vx) {
      a[1].real,a[1].real,a[1].real,a[1].real
    };
    a_1im = (vx) {
      a[1].imag,a[1].imag,a[1].imag,a[1].imag
    };
    a_2re = (vx) {
      a[2].real,a[2].real,a[2].real,a[2].real
    };
    a_2im = (vx) {
      a[2].imag,a[2].imag,a[2].imag,a[2].imag
    };
    a_3re = (vx) {
      a[3].real,a[3].real,a[3].real,a[3].real
    };
    a_3im = (vx) {
      a[3].imag,a[3].imag,a[3].imag,a[3].imag
    };

    b1re = (vx) {
      vb[8],vb[10],vb[12],vb[14]
    };
    b1im = (vx) {
      vb[9],vb[11],vb[13],vb[15]
    };

    P0_Q0re += a_0re*b0re;
    P1_Q0re += a_1re*b0re;
    P2_Q0re += a_2re*b0re;
    P3_Q0re += a_3re*b0re;

    P0_Q0re -= a_0im*b0im;
    P1_Q0re -= a_1im*b0im;
    P2_Q0re -= a_2im*b0im;
    P3_Q0re -= a_3im*b0im;

    P0_Q0im += a_0re*b0im;
    P1_Q0im += a_1re*b0im;
    P2_Q0im += a_2re*b0im;
    P3_Q0im += a_3re*b0im;

    P0_Q0im += a_0im*b0re;
    P1_Q0im += a_1im*b0re;
    P2_Q0im += a_2im*b0re;
    P3_Q0im += a_3im*b0re;

    b0re = (vx) {
      vb[16],vb[18],vb[20],vb[22]
    };
    b0im = (vx) {
      vb[17],vb[19],vb[21],vb[23]
    };

    P0_Q1re += a_0re*b1re;
    P1_Q1re += a_1re*b1re;
    P2_Q1re += a_2re*b1re;
    P3_Q1re += a_3re*b1re;

    P0_Q1re -= a_0im*b1im;
    P1_Q1re -= a_1im*b1im;
    P2_Q1re -= a_2im*b1im;
    P3_Q1re -= a_3im*b1im;

    P0_Q1im += a_0re*b1im;
    P1_Q1im += a_1re*b1im;
    P2_Q1im += a_2re*b1im;
    P3_Q1im += a_3re*b1im;

    P0_Q1im += a_0im*b1re;
    P1_Q1im += a_1im*b1re;
    P2_Q1im += a_2im*b1re;
    P3_Q1im += a_3im*b1re;

    a += MR;
    vb += 16;       // vb is pointer to double
  }

  if (alpha.real!=1.0||alpha.imag!=0.0) {
    vx valpha_re = (vx) {
      alpha.real,alpha.real,alpha.real,alpha.real
    };
    vx valpha_im = (vx) {
      alpha.imag,alpha.imag,alpha.imag,alpha.imag
    };

    P0_Q0re = P0_Q0re*valpha_re - P0_Q0im*valpha_im;
    P0_Q0im = P0_Q0re*valpha_im + P0_Q0im*valpha_re;
    P0_Q1re = P0_Q1re*valpha_re - P0_Q1im*valpha_im;
    P0_Q1im = P0_Q1re*valpha_im + P0_Q1im*valpha_re;

    P1_Q0re = P1_Q0re*valpha_re - P1_Q0im*valpha_im;
    P1_Q0im = P1_Q0re*valpha_im + P1_Q0im*valpha_re;
    P1_Q1re = P1_Q1re*valpha_re - P1_Q1im*valpha_im;
    P1_Q1im = P1_Q1re*valpha_im + P1_Q1im*valpha_re;

    P2_Q0re = P2_Q0re*valpha_re - P2_Q0im*valpha_im;
    P2_Q0im = P2_Q0re*valpha_im + P2_Q0im*valpha_re;
    P2_Q1re = P2_Q1re*valpha_re - P2_Q1im*valpha_im;
    P2_Q1im = P2_Q1re*valpha_im + P2_Q1im*valpha_re;

    P3_Q0re = P3_Q0re*valpha_re - P3_Q0im*valpha_im;
    P3_Q0im = P3_Q0re*valpha_im + P3_Q0im*valpha_re;
    P3_Q1re = P3_Q1re*valpha_re - P3_Q1im*valpha_im;
    P3_Q1im = P3_Q1re*valpha_im + P3_Q1im*valpha_re;
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
  ppr[0] = P0_Q0re;
  ppi[0] = P0_Q0im;
  ppr[1] = P0_Q1re;
  ppi[1] = P0_Q1im;

  c[0*rs_c+0*cs_c].real += pr[0];
  c[0*rs_c+0*cs_c].imag += pi[0];
  c[0*rs_c+1*cs_c].real += pr[1];
  c[0*rs_c+1*cs_c].imag += pi[1];
  c[0*rs_c+2*cs_c].real += pr[2];
  c[0*rs_c+2*cs_c].imag += pi[2];
  c[0*rs_c+3*cs_c].real += pr[3];
  c[0*rs_c+3*cs_c].imag += pi[3];

  c[0*rs_c+4*cs_c].real += pr[4];
  c[0*rs_c+4*cs_c].imag += pi[4];
  c[0*rs_c+5*cs_c].real += pr[5];
  c[0*rs_c+5*cs_c].imag += pi[5];
  c[0*rs_c+6*cs_c].real += pr[6];
  c[0*rs_c+6*cs_c].imag += pi[6];
  c[0*rs_c+7*cs_c].real += pr[7];
  c[0*rs_c+7*cs_c].imag += pi[7];

  ppr[0] = P1_Q0re;
  ppi[0] = P1_Q0im;
  ppr[1] = P1_Q1re;
  ppi[1] = P1_Q1im;

  c[1*rs_c+0*cs_c].real += pr[0];
  c[1*rs_c+0*cs_c].imag += pi[0];
  c[1*rs_c+1*cs_c].real += pr[1];
  c[1*rs_c+1*cs_c].imag += pi[1];
  c[1*rs_c+2*cs_c].real += pr[2];
  c[1*rs_c+2*cs_c].imag += pi[2];
  c[1*rs_c+3*cs_c].real += pr[3];
  c[1*rs_c+3*cs_c].imag += pi[3];

  c[1*rs_c+4*cs_c].real += pr[4];
  c[1*rs_c+4*cs_c].imag += pi[4];
  c[1*rs_c+5*cs_c].real += pr[5];
  c[1*rs_c+5*cs_c].imag += pi[5];
  c[1*rs_c+6*cs_c].real += pr[6];
  c[1*rs_c+6*cs_c].imag += pi[6];
  c[1*rs_c+7*cs_c].real += pr[7];
  c[1*rs_c+7*cs_c].imag += pi[7];

  ppr[0] = P2_Q0re;
  ppi[0] = P2_Q0im;
  ppr[1] = P2_Q1re;
  ppi[1] = P2_Q1im;

  c[2*rs_c+0*cs_c].real += pr[0];
  c[2*rs_c+0*cs_c].imag += pi[0];
  c[2*rs_c+1*cs_c].real += pr[1];
  c[2*rs_c+1*cs_c].imag += pi[1];
  c[2*rs_c+2*cs_c].real += pr[2];
  c[2*rs_c+2*cs_c].imag += pi[2];
  c[2*rs_c+3*cs_c].real += pr[3];
  c[2*rs_c+3*cs_c].imag += pi[3];

  c[2*rs_c+4*cs_c].real += pr[4];
  c[2*rs_c+4*cs_c].imag += pi[4];
  c[2*rs_c+5*cs_c].real += pr[5];
  c[2*rs_c+5*cs_c].imag += pi[5];
  c[2*rs_c+6*cs_c].real += pr[6];
  c[2*rs_c+6*cs_c].imag += pi[6];
  c[2*rs_c+7*cs_c].real += pr[7];
  c[2*rs_c+7*cs_c].imag += pi[7];

  ppr[0] = P3_Q0re;
  ppi[0] = P3_Q0im;
  ppr[1] = P3_Q1re;
  ppi[1] = P3_Q1im;

  c[3*rs_c+0*cs_c].real += pr[0];
  c[3*rs_c+0*cs_c].imag += pi[0];
  c[3*rs_c+1*cs_c].real += pr[1];
  c[3*rs_c+1*cs_c].imag += pi[1];
  c[3*rs_c+2*cs_c].real += pr[2];
  c[3*rs_c+2*cs_c].imag += pi[2];
  c[3*rs_c+3*cs_c].real += pr[3];
  c[3*rs_c+3*cs_c].imag += pi[3];

  c[3*rs_c+4*cs_c].real += pr[4];
  c[3*rs_c+4*cs_c].imag += pi[4];
  c[3*rs_c+5*cs_c].real += pr[5];
  c[3*rs_c+5*cs_c].imag += pi[5];
  c[3*rs_c+6*cs_c].real += pr[6];
  c[3*rs_c+6*cs_c].imag += pi[6];
  c[3*rs_c+7*cs_c].real += pr[7];
  c[3*rs_c+7*cs_c].imag += pi[7];

}

#endif // 256_4x8

#else

#error "only 128/256 simd are supported"

#endif

#undef NRv

#undef MC
#undef KC
#undef NC
#undef MR
#undef NR

#endif     /*  __GNUC__  */
