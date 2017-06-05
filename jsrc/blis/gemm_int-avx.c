/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* gemm micro kernel                                                       */

#include "../blis.h"

#if BLIS_DRIVER == BLIS_DRIVER_AVX

#include <immintrin.h>

#define NO_FMA 1

#if NO_FMA
#define MM256_FMADD_PD(a,b,c) _mm256_add_pd(_mm256_mul_pd(a,b),c)
#define BLI_DGEMM bli_dgemm_int_8x6
#define BLI_ZGEMM bli_zgemm_int_4x3
#else
#define MM256_FMADD_PD(a,b,c) _mm256_fmadd_pd(a,b,c)
#define BLI_DGEMM bli_dgemm2_int_8x6
#define BLI_ZGEMM bli_zgemm2_int_4x3
#endif

#define MC  BLIS_DEFAULT_MC_D
#define KC  BLIS_DEFAULT_KC_D
#define NC  BLIS_DEFAULT_NC_D
#define MR  BLIS_DEFAULT_MR_D
#define NR  BLIS_DEFAULT_NR_D

#define NRv (NR/4)              // 4 lanes
#if !((MR==8)&&(NR==6))
#error "invalid MR NR"
#endif

void BLI_DGEMM
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

  register __m256d b_re, b_im;
  register __m256d a_0, a_1;

  const double *vb = (const double *)b;

  _mm256_zeroupper();
  register __m256d t0_0r, t0_1r, t0_0i, t0_1i;
  register __m256d t1_0r, t1_1r, t1_0i, t1_1i;
  register __m256d t2_0r, t2_1r, t2_0i, t2_1i;

  t0_0r = _mm256_setzero_pd(); t0_1r = _mm256_setzero_pd(); t0_0i = _mm256_setzero_pd(); t0_1i = _mm256_setzero_pd();
  t1_0r = _mm256_setzero_pd(); t1_1r = _mm256_setzero_pd(); t1_0i = _mm256_setzero_pd(); t1_1i = _mm256_setzero_pd();
  t2_0r = _mm256_setzero_pd(); t2_1r = _mm256_setzero_pd(); t2_0i = _mm256_setzero_pd(); t2_1i = _mm256_setzero_pd();

  const double *p0,*p1; __m256d pp0[6],pp1[6];

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

// iter 0
    PREFETCH((void*)(a+64));  //   PREFETCH((void*)(a+2*4*MR));

    a_0 = _mm256_load_pd( (double*)a );
    a_1 = _mm256_load_pd( 4+(double*)a );

    b_re = _mm256_broadcast_sd( vb );
    b_im = _mm256_broadcast_sd( vb+1 );

    t0_0r = MM256_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM256_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM256_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM256_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm256_broadcast_sd( vb+2 );
    b_im = _mm256_broadcast_sd( vb+3 );

    t1_0r = MM256_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM256_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM256_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM256_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm256_broadcast_sd( vb+4 );
    b_im = _mm256_broadcast_sd( vb+5 );

    t2_0r = MM256_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM256_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM256_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM256_FMADD_PD(a_1, b_im, t2_1i);

// iter 1
    a_0 = _mm256_load_pd( 8+(double*)a );
    a_1 = _mm256_load_pd( 12+(double*)a );

    b_re = _mm256_broadcast_sd( vb+6 );
    b_im = _mm256_broadcast_sd( vb+7 );

    t0_0r = MM256_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM256_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM256_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM256_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm256_broadcast_sd( vb+8 );
    b_im = _mm256_broadcast_sd( vb+9 );

    t1_0r = MM256_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM256_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM256_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM256_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm256_broadcast_sd( vb+10 );
    b_im = _mm256_broadcast_sd( vb+11 );

    t2_0r = MM256_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM256_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM256_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM256_FMADD_PD(a_1, b_im, t2_1i);

// iter 2
    PREFETCH((void*)(a+72)); // PREFETCH((void*)(a+MR+2*4*MR));

    a_0 = _mm256_load_pd( 16+(double*)a );
    a_1 = _mm256_load_pd( 20+(double*)a );

    b_re = _mm256_broadcast_sd( vb+12 );
    b_im = _mm256_broadcast_sd( vb+13 );

    t0_0r = MM256_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM256_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM256_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM256_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm256_broadcast_sd( vb+14 );
    b_im = _mm256_broadcast_sd( vb+15 );

    t1_0r = MM256_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM256_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM256_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM256_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm256_broadcast_sd( vb+16 );
    b_im = _mm256_broadcast_sd( vb+17 );

    t2_0r = MM256_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM256_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM256_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM256_FMADD_PD(a_1, b_im, t2_1i);

// iter 3
    a_0 = _mm256_load_pd( 24+(double*)a );
    a_1 = _mm256_load_pd( 28+(double*)a );

    b_re = _mm256_broadcast_sd( vb+18 );
    b_im = _mm256_broadcast_sd( vb+19 );

    t0_0r = MM256_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM256_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM256_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM256_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm256_broadcast_sd( vb+20 );
    b_im = _mm256_broadcast_sd( vb+21 );

    t1_0r = MM256_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM256_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM256_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM256_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm256_broadcast_sd( vb+22 );
    b_im = _mm256_broadcast_sd( vb+23 );

    t2_0r = MM256_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM256_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM256_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM256_FMADD_PD(a_1, b_im, t2_1i);

    a += 4*MR;      // unroll*MR
    vb += 4*NR;     // unroll*NR  vb is pointer to double
  }

  for (l=0; l<kr; ++l) {
    PREFETCH((void*)(a+64)); // PREFETCH((void*)(a+2*4*MR));

    a_0 = _mm256_load_pd( (double*)a );
    a_1 = _mm256_load_pd( 4+(double*)a );

    b_re = _mm256_broadcast_sd( vb );
    b_im = _mm256_broadcast_sd( vb+1 );

    t0_0r = MM256_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM256_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM256_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM256_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm256_broadcast_sd( vb+2 );
    b_im = _mm256_broadcast_sd( vb+3 );

    t1_0r = MM256_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM256_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM256_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM256_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm256_broadcast_sd( vb+4 );
    b_im = _mm256_broadcast_sd( vb+5 );

    t2_0r = MM256_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM256_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM256_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM256_FMADD_PD(a_1, b_im, t2_1i);

    a += MR;
    vb += NR;       // vb is pointer to double
  }

  if (alpha!=1.0) {
    b_re = _mm256_broadcast_sd( (double*)alpha_ );

    t0_0r = _mm256_mul_pd ( b_re , t0_0r );
    t0_1r = _mm256_mul_pd ( b_re , t0_1r );
    t0_0i = _mm256_mul_pd ( b_re , t0_0i );
    t0_1i = _mm256_mul_pd ( b_re , t0_1i );

    t1_0r = _mm256_mul_pd ( b_re , t1_0r );
    t1_1r = _mm256_mul_pd ( b_re , t1_1r );
    t1_0i = _mm256_mul_pd ( b_re , t1_0i );
    t1_1i = _mm256_mul_pd ( b_re , t1_1i );

    t2_0r = _mm256_mul_pd ( b_re , t2_0r );
    t2_1r = _mm256_mul_pd ( b_re , t2_1r );
    t2_0i = _mm256_mul_pd ( b_re , t2_0i );
    t2_1i = _mm256_mul_pd ( b_re , t2_1i );

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

//  t0_0r t0_0i t0_1r t0_1i
//  t1_0r t1_0i t1_1r t1_1i
//  t2_0r t2_0i t2_1r t2_1i
//  row order in rr ri ir ii
//  col order in t0 t1 t2

  p0 = (const double *) &pp0;
  p1 = (const double *) &pp1;
  pp0[0] = t0_0r;    // p0 0
  pp0[1] = t1_0r;    // p0 4
  pp0[2] = t2_0r;    // p0 8
  pp0[3] = t0_0i;    // p0 12
  pp0[4] = t1_0i;    // p0 16
  pp0[5] = t2_0i;    // p0 20
  pp1[0] = t0_1r;    // p1 0
  pp1[1] = t1_1r;    // p1 4
  pp1[2] = t2_1r;    // p1 8
  pp1[3] = t0_1i;    // p1 12
  pp1[4] = t1_1i;    // p1 16
  pp1[5] = t2_1i;    // p1 20

  gint_t pc;

  pc = 0;
  c[pc      ] += p0[0];
  c[pc+=cs_c] += p0[12];
  c[pc+=cs_c] += p0[4];
  c[pc+=cs_c] += p0[16];
  c[pc+=cs_c] += p0[8];
  c[pc+=cs_c] += p0[20];

  pc = rs_c;
  c[pc      ] += p0[1];
  c[pc+=cs_c] += p0[13];
  c[pc+=cs_c] += p0[5];
  c[pc+=cs_c] += p0[17];
  c[pc+=cs_c] += p0[9];
  c[pc+=cs_c] += p0[21];

  pc = 2*rs_c;
  c[pc      ] += p0[2];
  c[pc+=cs_c] += p0[14];
  c[pc+=cs_c] += p0[6];
  c[pc+=cs_c] += p0[18];
  c[pc+=cs_c] += p0[10];
  c[pc+=cs_c] += p0[22];

  pc = 3*rs_c;
  c[pc      ] += p0[3];
  c[pc+=cs_c] += p0[15];
  c[pc+=cs_c] += p0[7];
  c[pc+=cs_c] += p0[19];
  c[pc+=cs_c] += p0[11];
  c[pc+=cs_c] += p0[23];

  pc = 4*rs_c;
  c[pc      ] += p1[0];
  c[pc+=cs_c] += p1[12];
  c[pc+=cs_c] += p1[4];
  c[pc+=cs_c] += p1[16];
  c[pc+=cs_c] += p1[8];
  c[pc+=cs_c] += p1[20];

  pc = 5*rs_c;
  c[pc      ] += p1[1];
  c[pc+=cs_c] += p1[13];
  c[pc+=cs_c] += p1[5];
  c[pc+=cs_c] += p1[17];
  c[pc+=cs_c] += p1[9];
  c[pc+=cs_c] += p1[21];

  pc = 6*rs_c;
  c[pc      ] += p1[2];
  c[pc+=cs_c] += p1[14];
  c[pc+=cs_c] += p1[6];
  c[pc+=cs_c] += p1[18];
  c[pc+=cs_c] += p1[10];
  c[pc+=cs_c] += p1[22];

  pc = 7*rs_c;
  c[pc      ] += p1[3];
  c[pc+=cs_c] += p1[15];
  c[pc+=cs_c] += p1[7];
  c[pc+=cs_c] += p1[19];
  c[pc+=cs_c] += p1[11];
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


#define NRv (NR/4)              // 4 lanes
#if !((MR==4)&&(NR==3))
#error "invalid MR NR"
#endif

void BLI_ZGEMM
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

  register __m256d b_re, b_im;
  register __m256d a_0, a_1;

  const double *vb = (const double *)b;

  _mm256_zeroupper();
  register __m256d t0_0r, t0_1r, t0_0i, t0_1i;
  register __m256d t1_0r, t1_1r, t1_0i, t1_1i;
  register __m256d t2_0r, t2_1r, t2_0i, t2_1i;

  t0_0r = _mm256_setzero_pd(); t0_1r = _mm256_setzero_pd(); t0_0i = _mm256_setzero_pd(); t0_1i = _mm256_setzero_pd();
  t1_0r = _mm256_setzero_pd(); t1_1r = _mm256_setzero_pd(); t1_0i = _mm256_setzero_pd(); t1_1i = _mm256_setzero_pd();
  t2_0r = _mm256_setzero_pd(); t2_1r = _mm256_setzero_pd(); t2_0i = _mm256_setzero_pd(); t2_1i = _mm256_setzero_pd();

  const double *p0,*p1; __m256d pp0[3],pp1[3];

  PREFETCH((void*)&c[0*cs_c]);
  PREFETCH((void*)&c[1*cs_c]);
  PREFETCH((void*)&c[2*cs_c]);

  gint_t l;
  gint_t ki = k/4;  // unroll
  gint_t kr = k%4;
  for (l=0; l<ki; ++l) {

// iter 0
    PREFETCH((void*)(a+32)); // PREFETCH((void*)(a+2*4*MR));

    a_0 = _mm256_load_pd( (double*)a );
    a_1 = _mm256_load_pd( 4+(double*)a );

    b_re = _mm256_broadcast_sd( vb );
    b_im = _mm256_broadcast_sd( vb+1 );

    t0_0r = MM256_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM256_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM256_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM256_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm256_broadcast_sd( vb+2 );
    b_im = _mm256_broadcast_sd( vb+3 );

    t1_0r = MM256_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM256_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM256_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM256_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm256_broadcast_sd( vb+4 );
    b_im = _mm256_broadcast_sd( vb+5 );

    t2_0r = MM256_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM256_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM256_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM256_FMADD_PD(a_1, b_im, t2_1i);

// iter 1
    a_0 = _mm256_load_pd( 8+(double*)a );
    a_1 = _mm256_load_pd( 12+(double*)a );

    b_re = _mm256_broadcast_sd( vb+6 );
    b_im = _mm256_broadcast_sd( vb+7 );

    t0_0r = MM256_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM256_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM256_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM256_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm256_broadcast_sd( vb+8 );
    b_im = _mm256_broadcast_sd( vb+9 );

    t1_0r = MM256_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM256_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM256_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM256_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm256_broadcast_sd( vb+10 );
    b_im = _mm256_broadcast_sd( vb+11 );

    t2_0r = MM256_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM256_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM256_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM256_FMADD_PD(a_1, b_im, t2_1i);

// iter 2
    PREFETCH((void*)(a+36)); // PREFETCH((void*)(a+MR+2*4*MR));

    a_0 = _mm256_load_pd( 16+(double*)a );
    a_1 = _mm256_load_pd( 20+(double*)a );

    b_re = _mm256_broadcast_sd( vb+12 );
    b_im = _mm256_broadcast_sd( vb+13 );

    t0_0r = MM256_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM256_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM256_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM256_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm256_broadcast_sd( vb+14 );
    b_im = _mm256_broadcast_sd( vb+15 );

    t1_0r = MM256_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM256_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM256_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM256_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm256_broadcast_sd( vb+16 );
    b_im = _mm256_broadcast_sd( vb+17 );

    t2_0r = MM256_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM256_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM256_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM256_FMADD_PD(a_1, b_im, t2_1i);

// iter 3
    a_0 = _mm256_load_pd( 24+(double*)a );
    a_1 = _mm256_load_pd( 28+(double*)a );

    b_re = _mm256_broadcast_sd( vb+18 );
    b_im = _mm256_broadcast_sd( vb+19 );

    t0_0r = MM256_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM256_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM256_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM256_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm256_broadcast_sd( vb+20 );
    b_im = _mm256_broadcast_sd( vb+21 );

    t1_0r = MM256_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM256_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM256_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM256_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm256_broadcast_sd( vb+22 );
    b_im = _mm256_broadcast_sd( vb+23 );

    t2_0r = MM256_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM256_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM256_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM256_FMADD_PD(a_1, b_im, t2_1i);

    a += 4*MR;        // unroll*MR
    vb += 4*NR*2;     // unroll*NR  vb is pointer to double
  }

  for (l=0; l<kr; ++l) {

    PREFETCH((void*)(a+32)); // PREFETCH((void*)(a+2*4*MR));

    a_0 = _mm256_load_pd( (double*)a );
    a_1 = _mm256_load_pd( 4+(double*)a );

    b_re = _mm256_broadcast_sd( vb );
    b_im = _mm256_broadcast_sd( vb+1 );

    t0_0r = MM256_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM256_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM256_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM256_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm256_broadcast_sd( vb+2 );
    b_im = _mm256_broadcast_sd( vb+3 );

    t1_0r = MM256_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM256_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM256_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM256_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm256_broadcast_sd( vb+4 );
    b_im = _mm256_broadcast_sd( vb+5 );

    t2_0r = MM256_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM256_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM256_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM256_FMADD_PD(a_1, b_im, t2_1i);

    a += MR;
    vb += NR*2;       // vb is pointer to double
  }

  t0_0i = _mm256_permute_pd ( t0_0i , 0x5 );
  t0_1i = _mm256_permute_pd ( t0_1i , 0x5 );

  t1_0i = _mm256_permute_pd ( t1_0i , 0x5 );
  t1_1i = _mm256_permute_pd ( t1_1i , 0x5 );

  t2_0i = _mm256_permute_pd ( t2_0i , 0x5 );
  t2_1i = _mm256_permute_pd ( t2_1i , 0x5 );

  t0_0r = _mm256_addsub_pd ( t0_0r , t0_0i );
  t0_1r = _mm256_addsub_pd ( t0_1r , t0_1i );

  t1_0r = _mm256_addsub_pd ( t1_0r , t1_0i );
  t1_1r = _mm256_addsub_pd ( t1_1r , t1_1i );

  t2_0r = _mm256_addsub_pd ( t2_0r , t2_0i );
  t2_1r = _mm256_addsub_pd ( t2_1r , t2_1i );

  if (alpha.real!=1.0||alpha.imag!=0.0) {
    b_re = _mm256_broadcast_sd( (double*)alpha_ );
    b_im = _mm256_broadcast_sd( 1+(double*)alpha_ );

    t0_0i = _mm256_permute_pd ( t0_0r , 0x5 );
    t0_0r = _mm256_mul_pd ( b_re , t0_0r );
    t0_0i = _mm256_mul_pd ( b_im , t0_0i );
    t0_0r = _mm256_addsub_pd ( t0_0r, t0_0i );

    t0_0i = _mm256_permute_pd ( t0_1r , 0x5 );
    t0_1r = _mm256_mul_pd ( b_re , t0_1r );
    t0_0i = _mm256_mul_pd ( b_im , t0_0i );
    t0_1r = _mm256_addsub_pd ( t0_1r, t0_0i );

    t1_0i = _mm256_permute_pd ( t1_0r , 0x5 );
    t1_0r = _mm256_mul_pd ( b_re , t1_0r );
    t1_0i = _mm256_mul_pd ( b_im , t1_0i );
    t1_0r = _mm256_addsub_pd ( t1_0r, t1_0i );

    t1_0i = _mm256_permute_pd ( t1_1r , 0x5 );
    t1_1r = _mm256_mul_pd ( b_re , t1_1r );
    t1_0i = _mm256_mul_pd ( b_im , t1_0i );
    t1_1r = _mm256_addsub_pd ( t1_1r, t1_0i );

    t2_0i = _mm256_permute_pd ( t2_0r , 0x5 );
    t2_0r = _mm256_mul_pd ( b_re , t2_0r );
    t2_0i = _mm256_mul_pd ( b_im , t2_0i );
    t2_0r = _mm256_addsub_pd ( t2_0r, t2_0i );

    t2_0i = _mm256_permute_pd ( t2_1r , 0x5 );
    t2_1r = _mm256_mul_pd ( b_re , t2_1r );
    t2_0i = _mm256_mul_pd ( b_im , t2_0i );
    t2_1r = _mm256_addsub_pd ( t2_1r, t2_0i );

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

//  each row 3 pairs of re im
//  t0_0r t1_0r t2_0r : row 0 and row 1
//  t0_1r t1_1r t2_1r : row 2 and row 3
//
//  t0_0r a b c d
//  t1_0r e f g h
//  t2_0r i j k l
//         re im re im re im
//  row 0: a  b  e  f  i  j
//  row 1: c  d  g  h  k  l

  p0 = (const double *) &pp0;
  p1 = (const double *) &pp1;
  pp0[0] = t0_0r;    // p0 0
  pp0[1] = t1_0r;    // p0 4
  pp0[2] = t2_0r;    // p0 8
  pp1[0] = t0_1r;    // p1 0
  pp1[1] = t1_1r;    // p1 4
  pp1[2] = t2_1r;    // p1 8

  gint_t pc;

  pc = 0;
  c[pc      ].real += p0[0];
  c[pc      ].imag += p0[1];
  c[pc+=cs_c].real += p0[4];
  c[pc      ].imag += p0[5];
  c[pc+=cs_c].real += p0[8];
  c[pc      ].imag += p0[9];

  pc = rs_c;
  c[pc      ].real += p0[2];
  c[pc      ].imag += p0[3];
  c[pc+=cs_c].real += p0[6];
  c[pc      ].imag += p0[7];
  c[pc+=cs_c].real += p0[10];
  c[pc      ].imag += p0[11];

  pc = 2*rs_c;
  c[pc      ].real += p1[0];
  c[pc      ].imag += p1[1];
  c[pc+=cs_c].real += p1[4];
  c[pc      ].imag += p1[5];
  c[pc+=cs_c].real += p1[8];
  c[pc      ].imag += p1[9];

  pc = 3*rs_c;
  c[pc      ].real += p1[2];
  c[pc      ].imag += p1[3];
  c[pc+=cs_c].real += p1[6];
  c[pc      ].imag += p1[7];
  c[pc+=cs_c].real += p1[10];
  c[pc      ].imag += p1[11];

}


#undef NRv

#undef MC
#undef KC
#undef NC
#undef MR
#undef NR

#endif   /*  C_AVX  */
