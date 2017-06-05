/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* gemm micro kernel                                                       */

#include "../blis.h"

#if BLIS_DRIVER == BLIS_DRIVER_SSE2

#include <emmintrin.h>

#define MM_FMADD_PD(a,b,c) _mm_add_pd(_mm_mul_pd(a,b),c)

#define MC  BLIS_DEFAULT_MC_D
#define KC  BLIS_DEFAULT_KC_D
#define NC  BLIS_DEFAULT_NC_D
#define MR  BLIS_DEFAULT_MR_D
#define NR  BLIS_DEFAULT_NR_D

#define NRv (NR/2)              // 2 lanes
#if !((MR==4)&&(NR==6))
#error "invalid MR NR"
#endif

void bli_dgemm_int_d4x6
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

  register __m128d b_re, b_im;
  register __m128d a_0, a_1;

  const double *vb = (const double *)b;

  register __m128d t0_0r, t0_1r, t0_0i, t0_1i;
#if defined(_WIN64)||defined(__LP64__)
  register __m128d t1_0r, t1_1r, t1_0i, t1_1i;
  register __m128d t2_0r, t2_1r, t2_0i, t2_1i;
#else
  __m128d t1_0r, t1_1r, t1_0i, t1_1i;
  __m128d t2_0r, t2_1r, t2_0i, t2_1i;
#endif

  t0_0r = _mm_setzero_pd(); t0_1r = _mm_setzero_pd(); 
  t0_0i = _mm_setzero_pd(); t0_1i = _mm_setzero_pd();
  t1_0r = _mm_setzero_pd(); t1_1r = _mm_setzero_pd(); 
  t1_0i = _mm_setzero_pd(); t1_1i = _mm_setzero_pd();
  t2_0r = _mm_setzero_pd(); t2_1r = _mm_setzero_pd(); 
  t2_0i = _mm_setzero_pd(); t2_1i = _mm_setzero_pd();

  const double *p0,*p1; __m128d pp0[6],pp1[6];

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
    PREFETCH((void*)(a+32)); // PREFETCH((void*)(a+2*4*MR));

    a_0 = _mm_load_pd( (double*)a );
    a_1 = _mm_load_pd( 2+(double*)a );

    b_re = _mm_set1_pd( vb[0] );
    b_im = _mm_set1_pd( vb[1] );

    t0_0r = MM_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm_set1_pd( vb[2] );
    b_im = _mm_set1_pd( vb[3] );

    t1_0r = MM_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm_set1_pd( vb[4] );
    b_im = _mm_set1_pd( vb[5] );

    t2_0r = MM_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM_FMADD_PD(a_1, b_im, t2_1i);

// iter 1
    a_0 = _mm_load_pd( 4+(double*)a );
    a_1 = _mm_load_pd( 6+(double*)a );

    b_re = _mm_set1_pd( vb[6] );
    b_im = _mm_set1_pd( vb[7] );

    t0_0r = MM_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm_set1_pd( vb[8] );
    b_im = _mm_set1_pd( vb[9] );

    t1_0r = MM_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm_set1_pd( vb[10] );
    b_im = _mm_set1_pd( vb[11] );

    t2_0r = MM_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM_FMADD_PD(a_1, b_im, t2_1i);
   
// iter 2
    PREFETCH((void*)(a+36)); // PREFETCH((void*)(a+MR+2*4*MR));

    a_0 = _mm_load_pd( 8+(double*)a );
    a_1 = _mm_load_pd( 10+(double*)a );

    b_re = _mm_set1_pd( vb[12] );
    b_im = _mm_set1_pd( vb[13] );

    t0_0r = MM_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm_set1_pd( vb[14] );
    b_im = _mm_set1_pd( vb[15] );

    t1_0r = MM_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm_set1_pd( vb[16] );
    b_im = _mm_set1_pd( vb[17] );

    t2_0r = MM_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM_FMADD_PD(a_1, b_im, t2_1i);

// iter 3
    a_0 = _mm_load_pd( 12+(double*)a );
    a_1 = _mm_load_pd( 14+(double*)a );

    b_re = _mm_set1_pd( vb[18] );
    b_im = _mm_set1_pd( vb[19] );

    t0_0r = MM_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm_set1_pd( vb[20] );
    b_im = _mm_set1_pd( vb[21] );

    t1_0r = MM_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm_set1_pd( vb[22] );
    b_im = _mm_set1_pd( vb[23] );

    t2_0r = MM_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM_FMADD_PD(a_1, b_im, t2_1i);

    a += 4*MR;        // unroll*MR
    vb += 4*NR;       // unroll*NR  vb is pointer to double
  }

  for (l=0; l<kr; ++l) {

    PREFETCH((void*)(a+32)); // PREFETCH((void*)(a+2*4*MR));

    a_0 = _mm_load_pd( (double*)a );
    a_1 = _mm_load_pd( 2+(double*)a );

    b_re = _mm_set1_pd( vb[0] );
    b_im = _mm_set1_pd( vb[1] );

    t0_0r = MM_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm_set1_pd( vb[2] );
    b_im = _mm_set1_pd( vb[3] );

    t1_0r = MM_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm_set1_pd( vb[4] );
    b_im = _mm_set1_pd( vb[5] );

    t2_0r = MM_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM_FMADD_PD(a_1, b_im, t2_1i);

    a += MR;
    vb += NR;       // vb is pointer to double
  }

  if (alpha!=1.0) {
    b_re = _mm_set1_pd( alpha );

    t0_0r = _mm_mul_pd ( b_re , t0_0r );
    t0_1r = _mm_mul_pd ( b_re , t0_1r );
    t0_0i = _mm_mul_pd ( b_re , t0_0i );
    t0_1i = _mm_mul_pd ( b_re , t0_1i );

    t1_0r = _mm_mul_pd ( b_re , t1_0r );
    t1_1r = _mm_mul_pd ( b_re , t1_1r );
    t1_0i = _mm_mul_pd ( b_re , t1_0i );
    t1_1i = _mm_mul_pd ( b_re , t1_1i );

    t2_0r = _mm_mul_pd ( b_re , t2_0r );
    t2_1r = _mm_mul_pd ( b_re , t2_1r );
    t2_0i = _mm_mul_pd ( b_re , t2_0i );
    t2_1i = _mm_mul_pd ( b_re , t2_1i );

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
  pp0[0] = t0_0r;    // p0 0
  pp0[1] = t1_0r;    //
  pp0[2] = t2_0r;    //
  pp0[3] = t0_0i;    //
  pp0[4] = t1_0i;    //
  pp0[5] = t2_0i;    //
  pp1[0] = t0_1r;    // p1 0
  pp1[1] = t1_1r;    //
  pp1[2] = t2_1r;    //
  pp1[3] = t0_1i;    //
  pp1[4] = t1_1i;    //
  pp1[5] = t2_1i;    //

  gint_t pc;

  pc = 0;
  c[pc      ] += p0[0];
  c[pc+=cs_c] += p0[6];
  c[pc+=cs_c] += p0[2];
  c[pc+=cs_c] += p0[8];
  c[pc+=cs_c] += p0[4];
  c[pc+=cs_c] += p0[10];

  pc = rs_c;
  c[pc      ] += p0[1];
  c[pc+=cs_c] += p0[7];
  c[pc+=cs_c] += p0[3];
  c[pc+=cs_c] += p0[9];
  c[pc+=cs_c] += p0[5];
  c[pc+=cs_c] += p0[11];

  pc = 2*rs_c;
  c[pc      ] += p1[0];
  c[pc+=cs_c] += p1[6];
  c[pc+=cs_c] += p1[2];
  c[pc+=cs_c] += p1[8];
  c[pc+=cs_c] += p1[4];
  c[pc+=cs_c] += p1[10];

  pc = 3*rs_c;
  c[pc      ] += p1[1];
  c[pc+=cs_c] += p1[7];
  c[pc+=cs_c] += p1[3];
  c[pc+=cs_c] += p1[9];
  c[pc+=cs_c] += p1[5];
  c[pc+=cs_c] += p1[11];

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


#define NRv (NR/2)              // 2 lanes
#if !((MR==2)&&(NR==3))
#error "invalid MR NR"
#endif

void bli_zgemm_int_d2x3
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

  register __m128d b_re, b_im;
  register __m128d a_0, a_1;

  const double *vb = (const double *)b;

  register __m128d t0_0r, t0_1r, t0_0i, t0_1i;
#if defined(_WIN64)||defined(__LP64__)
  register __m128d t1_0r, t1_1r, t1_0i, t1_1i;
  register __m128d t2_0r, t2_1r, t2_0i, t2_1i;
#else
  __m128d t1_0r, t1_1r, t1_0i, t1_1i;
  __m128d t2_0r, t2_1r, t2_0i, t2_1i;
#endif

  t0_0r = _mm_setzero_pd(); t0_1r = _mm_setzero_pd(); 
  t0_0i = _mm_setzero_pd(); t0_1i = _mm_setzero_pd();
  t1_0r = _mm_setzero_pd(); t1_1r = _mm_setzero_pd(); 
  t1_0i = _mm_setzero_pd(); t1_1i = _mm_setzero_pd();
  t2_0r = _mm_setzero_pd(); t2_1r = _mm_setzero_pd(); 
  t2_0i = _mm_setzero_pd(); t2_1i = _mm_setzero_pd();

  const double *p0,*p1; __m128d pp0[3],pp1[3];

  PREFETCH((void*)&c[0*rs_c+0*cs_c]);
  PREFETCH((void*)&c[0*rs_c+1*cs_c]);
  PREFETCH((void*)&c[0*rs_c+2*cs_c]);

  gint_t l;
  gint_t ki = k/4;  // unroll
  gint_t kr = k%4;
  for (l=0; l<ki; ++l) {

// iter 0
    PREFETCH((void*)(a+16)); // PREFETCH((void*)(a+2*4*MR));

    a_0 = _mm_load_pd( (double*)a );
    a_1 = _mm_load_pd( 2+(double*)a );

    b_re = _mm_set1_pd( vb[0] );
    b_im = _mm_set1_pd( vb[1] );

    t0_0r = MM_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm_set1_pd( vb[2] );
    b_im = _mm_set1_pd( vb[3] );

    t1_0r = MM_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm_set1_pd( vb[4] );
    b_im = _mm_set1_pd( vb[5] );

    t2_0r = MM_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM_FMADD_PD(a_1, b_im, t2_1i);

// iter 1
    a_0 = _mm_load_pd( 4+(double*)a );
    a_1 = _mm_load_pd( 6+(double*)a );

    b_re = _mm_set1_pd( vb[6] );
    b_im = _mm_set1_pd( vb[7] );

    t0_0r = MM_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm_set1_pd( vb[8] );
    b_im = _mm_set1_pd( vb[9] );

    t1_0r = MM_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm_set1_pd( vb[10] );
    b_im = _mm_set1_pd( vb[11] );

    t2_0r = MM_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM_FMADD_PD(a_1, b_im, t2_1i);
   
// iter 2
    PREFETCH((void*)(a+18)); // PREFETCH((void*)(a+MR+2*4*MR));

    a_0 = _mm_load_pd( 8+(double*)a );
    a_1 = _mm_load_pd( 10+(double*)a );

    b_re = _mm_set1_pd( vb[12] );
    b_im = _mm_set1_pd( vb[13] );

    t0_0r = MM_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm_set1_pd( vb[14] );
    b_im = _mm_set1_pd( vb[15] );

    t1_0r = MM_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm_set1_pd( vb[16] );
    b_im = _mm_set1_pd( vb[17] );

    t2_0r = MM_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM_FMADD_PD(a_1, b_im, t2_1i);

// iter 3
    a_0 = _mm_load_pd( 12+(double*)a );
    a_1 = _mm_load_pd( 14+(double*)a );

    b_re = _mm_set1_pd( vb[18] );
    b_im = _mm_set1_pd( vb[19] );

    t0_0r = MM_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm_set1_pd( vb[20] );
    b_im = _mm_set1_pd( vb[21] );
    t1_0r = MM_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm_set1_pd( vb[22] );
    b_im = _mm_set1_pd( vb[23] );

    t2_0r = MM_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM_FMADD_PD(a_1, b_im, t2_1i);

    a += 4*MR;        // unroll*MR
    vb += 4*NR*2;     // unroll*NR  vb is pointer to double
  }

  for (l=0; l<kr; ++l) {

    PREFETCH((void*)(a+16)); // PREFETCH((void*)(a+2*4*MR));

    a_0 = _mm_load_pd( (double*)a );
    a_1 = _mm_load_pd( 2+(double*)a );

    b_re = _mm_set1_pd( vb[0] );
    b_im = _mm_set1_pd( vb[1] );

    t0_0r = MM_FMADD_PD(a_0, b_re, t0_0r);
    t0_1r = MM_FMADD_PD(a_1, b_re, t0_1r);
    t0_0i = MM_FMADD_PD(a_0, b_im, t0_0i);
    t0_1i = MM_FMADD_PD(a_1, b_im, t0_1i);

    b_re = _mm_set1_pd( vb[2] );
    b_im = _mm_set1_pd( vb[3] );

    t1_0r = MM_FMADD_PD(a_0, b_re, t1_0r);
    t1_1r = MM_FMADD_PD(a_1, b_re, t1_1r);
    t1_0i = MM_FMADD_PD(a_0, b_im, t1_0i);
    t1_1i = MM_FMADD_PD(a_1, b_im, t1_1i);

    b_re = _mm_set1_pd( vb[4] );
    b_im = _mm_set1_pd( vb[5] );

    t2_0r = MM_FMADD_PD(a_0, b_re, t2_0r);
    t2_1r = MM_FMADD_PD(a_1, b_re, t2_1r);
    t2_0i = MM_FMADD_PD(a_0, b_im, t2_0i);
    t2_1i = MM_FMADD_PD(a_1, b_im, t2_1i);

    a += MR;
    vb += NR*2;       // vb is pointer to double
  }

  t0_0i = _mm_shuffle_pd ( t0_0i , t0_0i , 0x1 );
  t0_1i = _mm_shuffle_pd ( t0_1i , t0_1i , 0x1 );

  t1_0i = _mm_shuffle_pd ( t1_0i , t1_0i , 0x1 );
  t1_1i = _mm_shuffle_pd ( t1_1i , t1_1i , 0x1 );

  t2_0i = _mm_shuffle_pd ( t2_0i , t2_0i , 0x1 );
  t2_1i = _mm_shuffle_pd ( t2_1i , t2_1i , 0x1 );

  a_0 = _mm_sub_pd ( t0_0r , t0_0i );
  a_1 = _mm_add_pd ( t0_0r , t0_0i );
  t0_0r = _mm_shuffle_pd ( a_0 , a_1 , 0x2 );

  a_0 = _mm_sub_pd ( t0_1r , t0_1i );
  a_1 = _mm_add_pd ( t0_1r , t0_1i );
  t0_1r = _mm_shuffle_pd ( a_0 , a_1 , 0x2 );

  a_0 = _mm_sub_pd ( t1_0r , t1_0i );
  a_1 = _mm_add_pd ( t1_0r , t1_0i );
  t1_0r = _mm_shuffle_pd ( a_0 , a_1 , 0x2 );

  a_0 = _mm_sub_pd ( t1_1r , t1_1i );
  a_1 = _mm_add_pd ( t1_1r , t1_1i );
  t1_1r = _mm_shuffle_pd ( a_0 , a_1 , 0x2 );

  a_0 = _mm_sub_pd ( t2_0r , t2_0i );
  a_1 = _mm_add_pd ( t2_0r , t2_0i );
  t2_0r = _mm_shuffle_pd ( a_0 , a_1 , 0x2 );

  a_0 = _mm_sub_pd ( t2_1r , t2_1i );
  a_1 = _mm_add_pd ( t2_1r , t2_1i );
  t2_1r = _mm_shuffle_pd ( a_0 , a_1 , 0x2 );

  if (alpha.real!=1.0||alpha.imag!=0.0) {
    b_re = _mm_set1_pd( alpha.real );
    b_im = _mm_set1_pd( alpha.imag );

    t0_0i = _mm_shuffle_pd ( t0_0r , t0_0r , 0x1 );
    t0_0r = _mm_mul_pd ( b_re , t0_0r );
    t0_0i = _mm_mul_pd ( b_im , t0_0i );
    a_0 = _mm_sub_pd ( t0_0r , t0_0i );
    a_1 = _mm_add_pd ( t0_0r , t0_0i );
    t0_0r = _mm_shuffle_pd ( a_0 , a_1 , 0x2 );

    t0_0i = _mm_shuffle_pd ( t0_1r , t0_1r , 0x1 );
    t0_1r = _mm_mul_pd ( b_re , t0_1r );
    t0_0i = _mm_mul_pd ( b_im , t0_0i );
    a_0 = _mm_sub_pd ( t0_1r , t0_1i );
    a_1 = _mm_add_pd ( t0_1r , t0_1i );
    t0_1r = _mm_shuffle_pd ( a_0 , a_1 , 0x2 );

    t1_0i = _mm_shuffle_pd ( t1_0r , t1_0r , 0x1 );
    t1_0r = _mm_mul_pd ( b_re , t1_0r );
    t1_0i = _mm_mul_pd ( b_im , t1_0i );
    a_0 = _mm_sub_pd ( t1_0r , t1_0i );
    a_1 = _mm_add_pd ( t1_0r , t1_0i );
    t1_0r = _mm_shuffle_pd ( a_0 , a_1 , 0x2 );

    t1_0i = _mm_shuffle_pd ( t1_1r , t1_1r , 0x1 );
    t1_1r = _mm_mul_pd ( b_re , t1_1r );
    t1_0i = _mm_mul_pd ( b_im , t1_0i );
    a_0 = _mm_sub_pd ( t1_1r , t1_1i );
    a_1 = _mm_add_pd ( t1_1r , t1_1i );
    t1_1r = _mm_shuffle_pd ( a_0 , a_1 , 0x2 );

    t2_0i = _mm_shuffle_pd ( t2_0r , t2_0r , 0x1 );
    t2_0r = _mm_mul_pd ( b_re , t2_0r );
    t2_0i = _mm_mul_pd ( b_im , t2_0i );
    a_0 = _mm_sub_pd ( t2_0r , t2_0i );
    a_1 = _mm_add_pd ( t2_0r , t2_0i );
    t2_0r = _mm_shuffle_pd ( a_0 , a_1 , 0x2 );

    t2_0i = _mm_shuffle_pd ( t2_1r , t2_1r , 0x1 );
    t2_1r = _mm_mul_pd ( b_re , t2_1r );
    t2_0i = _mm_mul_pd ( b_im , t2_0i );
    a_0 = _mm_sub_pd ( t2_1r , t2_1i );
    a_1 = _mm_add_pd ( t2_1r , t2_1i );
    t2_1r = _mm_shuffle_pd ( a_0 , a_1 , 0x2 );

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
  pp0[0] = t0_0r;    // p0 0
  pp0[1] = t1_0r;    // p0 2
  pp0[2] = t2_0r;    // p0 4
  pp1[0] = t0_1r;    // p1 0
  pp1[1] = t1_1r;    // p1 2
  pp1[2] = t2_1r;    // p1 4

  gint_t pc;

  pc = 0;
  c[pc      ].real += p0[0];
  c[pc      ].imag += p0[1];
  c[pc+=cs_c].real += p0[2];
  c[pc      ].imag += p0[3];
  c[pc+=cs_c].real += p0[4];
  c[pc      ].imag += p0[5];

  pc = rs_c;
  c[pc      ].real += p1[0];
  c[pc      ].imag += p1[1];
  c[pc+=cs_c].real += p1[2];
  c[pc      ].imag += p1[3];
  c[pc+=cs_c].real += p1[4];
  c[pc      ].imag += p1[5];

}


#undef NRv

#undef MC
#undef KC
#undef NC
#undef MR
#undef NR

#endif  /* __SSE2__  */
