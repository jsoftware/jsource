/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* gemm micro kernel                                                       */

#include "../blis.h"

#if BLIS_DRIVER == BLIS_DRIVER_AVX

#include <immintrin.h>

#define MC  BLIS_DEFAULT_MC_D
#define KC  BLIS_DEFAULT_KC_D
#define NC  BLIS_DEFAULT_NC_D
#define MR  BLIS_DEFAULT_MR_D
#define NR  BLIS_DEFAULT_NR_D

#define NRv (NR/4)              // 4 lanes
#if !((MR==4)&&(NR==8))
#error "invalid MR NR"
#endif

void bli_dgemm2_int_4x8
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

  __m256d b0, b1;

  double *vb = b;
  PREFETCH2((void*)vb);
  PREFETCH2((void*)(vb+4));

  __m256d P0_Q0 = _mm256_setzero_pd();
  __m256d P0_Q1 = _mm256_setzero_pd();
  __m256d P1_Q0 = _mm256_setzero_pd();
  __m256d P1_Q1 = _mm256_setzero_pd();
  __m256d P2_Q0 = _mm256_setzero_pd();
  __m256d P2_Q1 = _mm256_setzero_pd();
  __m256d P3_Q0 = _mm256_setzero_pd();
  __m256d P3_Q1 = _mm256_setzero_pd();

  PREFETCH2((void*)&c[0*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+3*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+4*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+5*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+6*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+7*cs_c]);

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

  gint_t l;
  for (l=0; l<k; ++l) {
    PREFETCH((void*)(a+MR));
    __m256d a_0 = _mm256_broadcast_sd( a );
    __m256d a_1 = _mm256_broadcast_sd( a+1 );
    __m256d a_2 = _mm256_broadcast_sd( a+2 );
    __m256d a_3 = _mm256_broadcast_sd( a+3 );

    PREFETCH2((void*)(vb+8));
    b0 =_mm256_loadu_pd(vb);

    P0_Q0 = _mm256_fmadd_pd(a_0, b0, P0_Q0);
    P1_Q0 = _mm256_fmadd_pd(a_1, b0, P1_Q0);
    P2_Q0 = _mm256_fmadd_pd(a_2, b0, P2_Q0);
    P3_Q0 = _mm256_fmadd_pd(a_3, b0, P3_Q0);

    PREFETCH2((void*)(vb+12));
    b1 =_mm256_loadu_pd(vb+4);

    P0_Q1 = _mm256_fmadd_pd(a_0, b1, P0_Q1);
    P1_Q1 = _mm256_fmadd_pd(a_1, b1, P1_Q1);
    P2_Q1 = _mm256_fmadd_pd(a_2, b1, P2_Q1);
    P3_Q1 = _mm256_fmadd_pd(a_3, b1, P3_Q1);

    a += MR;
    vb += 8;    // double
  }

  if (alpha!=1.0) {
    __m256d valpha = _mm256_broadcast_sd( alpha_ );
    P0_Q0 = _mm256_mul_pd(P0_Q0, valpha);
    P0_Q1 = _mm256_mul_pd(P0_Q1, valpha);
    P1_Q0 = _mm256_mul_pd(P1_Q0, valpha);
    P1_Q1 = _mm256_mul_pd(P1_Q1, valpha);
    P2_Q0 = _mm256_mul_pd(P2_Q0, valpha);
    P2_Q1 = _mm256_mul_pd(P2_Q1, valpha);
    P3_Q0 = _mm256_mul_pd(P3_Q0, valpha);
    P3_Q1 = _mm256_mul_pd(P3_Q1, valpha);
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

  const double *p = (const double *) &P0_Q0;
  c[0*rs_c+0*cs_c] += p[0];
  c[0*rs_c+1*cs_c] += p[1];
  c[0*rs_c+2*cs_c] += p[2];
  c[0*rs_c+3*cs_c] += p[3];

  p = (const double *) &P0_Q1;
  c[0*rs_c+4*cs_c] += p[0];
  c[0*rs_c+5*cs_c] += p[1];
  c[0*rs_c+6*cs_c] += p[2];
  c[0*rs_c+7*cs_c] += p[3];

  p = (const double *) &P1_Q0;
  c[1*rs_c+0*cs_c] += p[0];
  c[1*rs_c+1*cs_c] += p[1];
  c[1*rs_c+2*cs_c] += p[2];
  c[1*rs_c+3*cs_c] += p[3];

  p = (const double *) &P1_Q1;
  c[1*rs_c+4*cs_c] += p[0];
  c[1*rs_c+5*cs_c] += p[1];
  c[1*rs_c+6*cs_c] += p[2];
  c[1*rs_c+7*cs_c] += p[3];

  p = (const double *) &P2_Q0;
  c[2*rs_c+0*cs_c] += p[0];
  c[2*rs_c+1*cs_c] += p[1];
  c[2*rs_c+2*cs_c] += p[2];
  c[2*rs_c+3*cs_c] += p[3];

  p = (const double *) &P2_Q1;
  c[2*rs_c+4*cs_c] += p[0];
  c[2*rs_c+5*cs_c] += p[1];
  c[2*rs_c+6*cs_c] += p[2];
  c[2*rs_c+7*cs_c] += p[3];

  p = (const double *) &P3_Q0;
  c[3*rs_c+0*cs_c] += p[0];
  c[3*rs_c+1*cs_c] += p[1];
  c[3*rs_c+2*cs_c] += p[2];
  c[3*rs_c+3*cs_c] += p[3];

  p = (const double *) &P3_Q1;
  c[3*rs_c+4*cs_c] += p[0];
  c[3*rs_c+5*cs_c] += p[1];
  c[3*rs_c+6*cs_c] += p[2];
  c[3*rs_c+7*cs_c] += p[3];

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


#define NRv (NR/4)              // 4 lanes
#if !((MR==4)&&(NR==8))
#error "invalid MR NR"
#endif

void bli_zgemm2_int_4x8
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

  __m256d b0re, b1re, b0im, b1im;
  __m256d bt0,bt2;
  __m256d a_0re,a_0im,a_1re,a_1im,a_2re,a_2im,a_3re,a_3im;

  const double *vb = (const double *)b;
  PREFETCH2((void*)(vb));
  PREFETCH2((void*)(vb+4));
  PREFETCH2((void*)(vb+8));
  PREFETCH2((void*)(vb+12));

  __m256d P0_Q0re = _mm256_setzero_pd();
  __m256d P0_Q1re = _mm256_setzero_pd();
  __m256d P1_Q0re = _mm256_setzero_pd();
  __m256d P1_Q1re = _mm256_setzero_pd();
  __m256d P2_Q0re = _mm256_setzero_pd();
  __m256d P2_Q1re = _mm256_setzero_pd();
  __m256d P3_Q0re = _mm256_setzero_pd();
  __m256d P3_Q1re = _mm256_setzero_pd();

  __m256d P0_Q0im = _mm256_setzero_pd();
  __m256d P0_Q1im = _mm256_setzero_pd();
  __m256d P1_Q0im = _mm256_setzero_pd();
  __m256d P1_Q1im = _mm256_setzero_pd();
  __m256d P2_Q0im = _mm256_setzero_pd();
  __m256d P2_Q1im = _mm256_setzero_pd();
  __m256d P3_Q0im = _mm256_setzero_pd();
  __m256d P3_Q1im = _mm256_setzero_pd();

  PREFETCH2((void*)&c[0*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+3*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+4*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+5*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+6*cs_c]);
  PREFETCH2((void*)&c[0*rs_c+7*cs_c]);

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

  gint_t l;
  for (l=0; l<k; ++l) {
    PREFETCH((void*)(a+MR));
    a_0re = _mm256_broadcast_sd( (double*)a );
    a_0im = _mm256_broadcast_sd( 1+(double*)a );
    a_1re = _mm256_broadcast_sd( (double*)(a+1) );
    a_1im = _mm256_broadcast_sd( 1+(double*)(a+1) );
    a_2re = _mm256_broadcast_sd( (double*)(a+2) );
    a_2im = _mm256_broadcast_sd( 1+(double*)(a+2) );
    a_3re = _mm256_broadcast_sd( (double*)(a+3) );
    a_3im = _mm256_broadcast_sd( 1+(double*)(a+3) );

    PREFETCH2((void*)(vb+16));
    PREFETCH2((void*)(vb+20));
    bt0 = _mm256_loadu_pd(vb);               // [a b c d ]
    bt2 = _mm256_loadu_pd(vb+4);             // [e f g h ]
    b0re = _mm256_unpacklo_pd(bt0, bt2);     // [a e c g ]  in-lane interleave !!
    b0im = _mm256_unpackhi_pd(bt0, bt2);     // [b f d h ]
    // b0re, db0im in-lane interleave !!

    P0_Q0re = _mm256_fmadd_pd(a_0re, b0re, P0_Q0re);
    P0_Q0re = _mm256_fnmadd_pd(a_0im, b0im, P0_Q0re);
    P0_Q0im = _mm256_fmadd_pd(a_0re, b0im, P0_Q0im);
    P0_Q0im = _mm256_fmadd_pd(a_0im, b0re, P0_Q0im);

    P1_Q0re = _mm256_fmadd_pd(a_1re, b0re, P1_Q0re);
    P1_Q0re = _mm256_fnmadd_pd(a_1im, b0im, P1_Q0re);
    P1_Q0im = _mm256_fmadd_pd(a_1re, b0im, P1_Q0im);
    P1_Q0im = _mm256_fmadd_pd(a_1im, b0re, P1_Q0im);

    P2_Q0re = _mm256_fmadd_pd(a_2re, b0re, P2_Q0re);
    P2_Q0re = _mm256_fnmadd_pd(a_2im, b0im, P2_Q0re);
    P2_Q0im = _mm256_fmadd_pd(a_2re, b0im, P2_Q0im);
    P2_Q0im = _mm256_fmadd_pd(a_2im, b0re, P2_Q0im);

    P3_Q0re = _mm256_fmadd_pd(a_3re, b0re, P3_Q0re);
    P3_Q0re = _mm256_fnmadd_pd(a_3im, b0im, P3_Q0re);
    P3_Q0im = _mm256_fmadd_pd(a_3re, b0im, P3_Q0im);
    P3_Q0im = _mm256_fmadd_pd(a_3im, b0re, P3_Q0im);

    PREFETCH2((void*)(vb+24));
    PREFETCH2((void*)(vb+28));
    bt0 = _mm256_loadu_pd(vb+8);
    bt2 = _mm256_loadu_pd(vb+12);
    b1re = _mm256_unpacklo_pd(bt0, bt2);
    b1im = _mm256_unpackhi_pd(bt0, bt2);

    P0_Q1re = _mm256_fmadd_pd(a_0re, b1re, P0_Q1re);
    P0_Q1re = _mm256_fnmadd_pd(a_0im, b1im, P0_Q1re);
    P0_Q1im = _mm256_fmadd_pd(a_0re, b1im, P0_Q1im);
    P0_Q1im = _mm256_fmadd_pd(a_0im, b1re, P0_Q1im);

    P1_Q1re = _mm256_fmadd_pd(a_1re, b1re, P1_Q1re);
    P1_Q1re = _mm256_fnmadd_pd(a_1im, b1im, P1_Q1re);
    P1_Q1im = _mm256_fmadd_pd(a_1re, b1im, P1_Q1im);
    P1_Q1im = _mm256_fmadd_pd(a_1im, b1re, P1_Q1im);

    P2_Q1re = _mm256_fmadd_pd(a_2re, b1re, P2_Q1re);
    P2_Q1re = _mm256_fnmadd_pd(a_2im, b1im, P2_Q1re);
    P2_Q1im = _mm256_fmadd_pd(a_2re, b1im, P2_Q1im);
    P2_Q1im = _mm256_fmadd_pd(a_2im, b1re, P2_Q1im);

    P3_Q1re = _mm256_fmadd_pd(a_3re, b1re, P3_Q1re);
    P3_Q1re = _mm256_fnmadd_pd(a_3im, b1im, P3_Q1re);
    P3_Q1im = _mm256_fmadd_pd(a_3re, b1im, P3_Q1im);
    P3_Q1im = _mm256_fmadd_pd(a_3im, b1re, P3_Q1im);


    a += MR;
    vb += 16;       // vb is pointer to double
  }

  if (alpha.real!=1.0||alpha.imag!=0.0) {
    __m256d valpha_re = _mm256_broadcast_sd( (double*)alpha_ );
    __m256d valpha_im = _mm256_broadcast_sd( 1+(double*)alpha_ );
    P0_Q0re = _mm256_sub_pd(_mm256_mul_pd(P0_Q0re,valpha_re) , _mm256_mul_pd(P0_Q0im,valpha_im));
    P0_Q0im = _mm256_add_pd(_mm256_mul_pd(P0_Q0re,valpha_im) , _mm256_mul_pd(P0_Q0im,valpha_re));
    P0_Q1re = _mm256_sub_pd(_mm256_mul_pd(P0_Q1re,valpha_re) , _mm256_mul_pd(P0_Q1im,valpha_im));
    P0_Q1im = _mm256_add_pd(_mm256_mul_pd(P0_Q1re,valpha_im) , _mm256_mul_pd(P0_Q1im,valpha_re));

    P1_Q0re = _mm256_sub_pd(_mm256_mul_pd(P1_Q0re,valpha_re) , _mm256_mul_pd(P1_Q0im,valpha_im));
    P1_Q0im = _mm256_add_pd(_mm256_mul_pd(P1_Q0re,valpha_im) , _mm256_mul_pd(P1_Q0im,valpha_re));
    P1_Q1re = _mm256_sub_pd(_mm256_mul_pd(P1_Q1re,valpha_re) , _mm256_mul_pd(P1_Q1im,valpha_im));
    P1_Q1im = _mm256_add_pd(_mm256_mul_pd(P1_Q1re,valpha_im) , _mm256_mul_pd(P1_Q1im,valpha_re));

    P2_Q0re = _mm256_sub_pd(_mm256_mul_pd(P2_Q0re,valpha_re) , _mm256_mul_pd(P2_Q0im,valpha_im));
    P2_Q0im = _mm256_add_pd(_mm256_mul_pd(P2_Q0re,valpha_im) , _mm256_mul_pd(P2_Q0im,valpha_re));
    P2_Q1re = _mm256_sub_pd(_mm256_mul_pd(P2_Q1re,valpha_re) , _mm256_mul_pd(P2_Q1im,valpha_im));
    P2_Q1im = _mm256_add_pd(_mm256_mul_pd(P2_Q1re,valpha_im) , _mm256_mul_pd(P2_Q1im,valpha_re));

    P3_Q0re = _mm256_sub_pd(_mm256_mul_pd(P3_Q0re,valpha_re) , _mm256_mul_pd(P3_Q0im,valpha_im));
    P3_Q0im = _mm256_add_pd(_mm256_mul_pd(P3_Q0re,valpha_im) , _mm256_mul_pd(P3_Q0im,valpha_re));
    P3_Q1re = _mm256_sub_pd(_mm256_mul_pd(P3_Q1re,valpha_re) , _mm256_mul_pd(P3_Q1im,valpha_im));
    P3_Q1im = _mm256_add_pd(_mm256_mul_pd(P3_Q1re,valpha_im) , _mm256_mul_pd(P3_Q1im,valpha_re));
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

  const double *p = (const double *) &P0_Q0re;
  c[0*rs_c+0*cs_c].real += p[0];
  c[0*rs_c+1*cs_c].real += p[2];   // compensate in-lane interleave
  c[0*rs_c+2*cs_c].real += p[1];   // ...
  c[0*rs_c+3*cs_c].real += p[3];

  p = (const double *) &P0_Q0im;
  c[0*rs_c+0*cs_c].imag += p[0];
  c[0*rs_c+1*cs_c].imag += p[2];
  c[0*rs_c+2*cs_c].imag += p[1];
  c[0*rs_c+3*cs_c].imag += p[3];

  p = (const double *) &P0_Q1re;
  c[0*rs_c+4*cs_c].real += p[0];
  c[0*rs_c+5*cs_c].real += p[2];
  c[0*rs_c+6*cs_c].real += p[1];
  c[0*rs_c+7*cs_c].real += p[3];

  p = (const double *) &P0_Q1im;
  c[0*rs_c+4*cs_c].imag += p[0];
  c[0*rs_c+5*cs_c].imag += p[2];
  c[0*rs_c+6*cs_c].imag += p[1];
  c[0*rs_c+7*cs_c].imag += p[3];

  p = (const double *) &P1_Q0re;
  c[1*rs_c+0*cs_c].real += p[0];
  c[1*rs_c+1*cs_c].real += p[2];
  c[1*rs_c+2*cs_c].real += p[1];
  c[1*rs_c+3*cs_c].real += p[3];

  p = (const double *) &P1_Q0im;
  c[1*rs_c+0*cs_c].imag += p[0];
  c[1*rs_c+1*cs_c].imag += p[2];
  c[1*rs_c+2*cs_c].imag += p[1];
  c[1*rs_c+3*cs_c].imag += p[3];

  p = (const double *) &P1_Q1re;
  c[1*rs_c+4*cs_c].real += p[0];
  c[1*rs_c+5*cs_c].real += p[2];
  c[1*rs_c+6*cs_c].real += p[1];
  c[1*rs_c+7*cs_c].real += p[3];

  p = (const double *) &P1_Q1im;
  c[1*rs_c+4*cs_c].imag += p[0];
  c[1*rs_c+5*cs_c].imag += p[2];
  c[1*rs_c+6*cs_c].imag += p[1];
  c[1*rs_c+7*cs_c].imag += p[3];

  p = (const double *) &P2_Q0re;
  c[2*rs_c+0*cs_c].real += p[0];
  c[2*rs_c+1*cs_c].real += p[2];
  c[2*rs_c+2*cs_c].real += p[1];
  c[2*rs_c+3*cs_c].real += p[3];

  p = (const double *) &P2_Q0im;
  c[2*rs_c+0*cs_c].imag += p[0];
  c[2*rs_c+1*cs_c].imag += p[2];
  c[2*rs_c+2*cs_c].imag += p[1];
  c[2*rs_c+3*cs_c].imag += p[3];

  p = (const double *) &P2_Q1re;
  c[2*rs_c+4*cs_c].real += p[0];
  c[2*rs_c+5*cs_c].real += p[2];
  c[2*rs_c+6*cs_c].real += p[1];
  c[2*rs_c+7*cs_c].real += p[3];

  p = (const double *) &P2_Q1im;
  c[2*rs_c+4*cs_c].imag += p[0];
  c[2*rs_c+5*cs_c].imag += p[2];
  c[2*rs_c+6*cs_c].imag += p[1];
  c[2*rs_c+7*cs_c].imag += p[3];

  p = (const double *) &P3_Q0re;
  c[3*rs_c+0*cs_c].real += p[0];
  c[3*rs_c+1*cs_c].real += p[2];
  c[3*rs_c+2*cs_c].real += p[1];
  c[3*rs_c+3*cs_c].real += p[3];

  p = (const double *) &P3_Q0im;
  c[3*rs_c+0*cs_c].imag += p[0];
  c[3*rs_c+1*cs_c].imag += p[2];
  c[3*rs_c+2*cs_c].imag += p[1];
  c[3*rs_c+3*cs_c].imag += p[3];

  p = (const double *) &P3_Q1re;
  c[3*rs_c+4*cs_c].real += p[0];
  c[3*rs_c+5*cs_c].real += p[2];
  c[3*rs_c+6*cs_c].real += p[1];
  c[3*rs_c+7*cs_c].real += p[3];

  p = (const double *) &P3_Q1im;
  c[3*rs_c+4*cs_c].imag += p[0];
  c[3*rs_c+5*cs_c].imag += p[2];
  c[3*rs_c+6*cs_c].imag += p[1];
  c[3*rs_c+7*cs_c].imag += p[3];

}


#undef NRv

#undef MC
#undef KC
#undef NC
#undef MR
#undef NR

#endif   /*  C_AVX  */
