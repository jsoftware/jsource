/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* gemm micro kernel                                                       */

#include "../blis.h"

#if BLIS_DRIVER == BLIS_DRIVER_SSE2

#include <emmintrin.h>

/*
 Note:
 intel cacheline size = 64 bytes
*/

#define MM_FMADD_PD(a,b,c) _mm_add_pd(_mm_mul_pd(a,b),c)

#define MC  BLIS_DEFAULT_MC_D
#define KC  BLIS_DEFAULT_KC_D
#define NC  BLIS_DEFAULT_NC_D
#define MR  BLIS_DEFAULT_MR_D
#define NR  BLIS_DEFAULT_NR_D

#if !((MR==4)&&(NR==4))
#error "invalid MR NR"
#endif

#define LOOKAHEAD_A   4*16  // multiple of 16 = 4*MR (16*8 bytes)
#define LOOKAHEAD_B   4*16  // multiple of 16 = 4*NR (16*8 bytes)

void bli_dgemm_int_d4x4
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

#if defined(_WIN64)||defined(__LP64__)
  register __m128d a_re, a_im, d_re, d_im;
  register __m128d b_0, b_1, b_2, b_3;
#else
  __m128d a_re, a_im, d_re, d_im;
  __m128d b_0, b_1, b_2, b_3;
#endif

  const double *va = (const double *)a;

  register __m128d t0_00, t0_01, t0_02, t0_03;
  register __m128d t1_00, t1_01, t1_02, t1_03;

  t0_00 = _mm_setzero_pd();
  t0_01 = _mm_setzero_pd();
  t0_02 = _mm_setzero_pd();
  t0_03 = _mm_setzero_pd();
  t1_00 = _mm_setzero_pd();
  t1_01 = _mm_setzero_pd();
  t1_02 = _mm_setzero_pd();
  t1_03 = _mm_setzero_pd();

  const double *p0,*p1;
  __m128d pp0[4],pp1[4];

  a_re = _mm_set1_pd( va[0] );
  a_im = _mm_set1_pd( va[1] );

  b_0 = _mm_load_pd( (double*)b );
  b_1 = _mm_load_pd( 2+(double*)b );

  PREFETCH2((void*)&c[0*cs_c]);
  PREFETCH2((void*)&c[1*cs_c]);
  PREFETCH2((void*)&c[2*cs_c]);
  PREFETCH2((void*)&c[3*cs_c]);

  PREFETCH2((void*)&c[1*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+3*cs_c]);

  gint_t l;
  gint_t ki = k>>2;  // unroll
  gint_t kr = k&3;
  for (l=0; l<ki; ++l) {

// iter 0

    b_2 = _mm_load_pd( 4+(double*)b );
    PREFETCH((void*)(b+LOOKAHEAD_B )); // PREFETCH((void*)(a+2*4*MR));
    b_3 = _mm_load_pd( 6+(double*)b );
    PREFETCH((void*)(va+LOOKAHEAD_A ));

    d_re = _mm_set1_pd( va[2] );
    d_im = _mm_set1_pd( va[3] );

    t0_00 = MM_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_1, a_im, t0_03);

    a_re = _mm_set1_pd( va[4] );
    a_im = _mm_set1_pd( va[5] );

    t1_00 = MM_FMADD_PD(b_0, d_re, t1_00);
    t1_01 = MM_FMADD_PD(b_1, d_re, t1_01);
    t1_02 = MM_FMADD_PD(b_0, d_im, t1_02);
    t1_03 = MM_FMADD_PD(b_1, d_im, t1_03);
    CCBLOCK;

// iter 1

    b_0 = _mm_load_pd( 8+(double*)b );
    b_1 = _mm_load_pd( 10+(double*)b );

    d_re = _mm_set1_pd( va[6] );
    d_im = _mm_set1_pd( va[7] );

    t0_00 = MM_FMADD_PD(b_2, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_3, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_2, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_3, a_im, t0_03);

    a_re = _mm_set1_pd( va[8] );
    a_im = _mm_set1_pd( va[9] );

    t1_00 = MM_FMADD_PD(b_2, d_re, t1_00);
    t1_01 = MM_FMADD_PD(b_3, d_re, t1_01);
    t1_02 = MM_FMADD_PD(b_2, d_im, t1_02);
    t1_03 = MM_FMADD_PD(b_3, d_im, t1_03);
    CCBLOCK;

// iter 2

    b_2 = _mm_load_pd( 12+(double*)b );
    PREFETCH((void*)(b+LOOKAHEAD_B +8));
    b_3 = _mm_load_pd( 14+(double*)b );
    PREFETCH((void*)(va+LOOKAHEAD_A +8));

    d_re = _mm_set1_pd( va[10] );
    d_im = _mm_set1_pd( va[11] );

    t0_00 = MM_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_1, a_im, t0_03);

    a_re = _mm_set1_pd( va[12] );
    a_im = _mm_set1_pd( va[13] );

    t1_00 = MM_FMADD_PD(b_0, d_re, t1_00);
    t1_01 = MM_FMADD_PD(b_1, d_re, t1_01);
    t1_02 = MM_FMADD_PD(b_0, d_im, t1_02);
    t1_03 = MM_FMADD_PD(b_1, d_im, t1_03);
    CCBLOCK;

// iter 3

    b_0 = _mm_load_pd( 16+(double*)b );
    b_1 = _mm_load_pd( 18+(double*)b );

    d_re = _mm_set1_pd( va[14] );
    d_im = _mm_set1_pd( va[15] );

    t0_00 = MM_FMADD_PD(b_2, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_3, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_2, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_3, a_im, t0_03);

    a_re = _mm_set1_pd( va[16] );
    a_im = _mm_set1_pd( va[17] );

    t1_00 = MM_FMADD_PD(b_2, d_re, t1_00);
    t1_01 = MM_FMADD_PD(b_3, d_re, t1_01);
    t1_02 = MM_FMADD_PD(b_2, d_im, t1_02);
    t1_03 = MM_FMADD_PD(b_3, d_im, t1_03);
    CCBLOCK;

    b += 4*NR;        // unroll*NR
    va += 4*MR;       // unroll*MR  va is pointer to double
  }

  PREFETCH2((void*)&c[2*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+3*cs_c]);

  PREFETCH2((void*)&c[3*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[3*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[3*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[3*rs_c+3*cs_c]);

  for (l=0; l<kr; ++l) {

    d_re = _mm_set1_pd( va[2] );
    d_im = _mm_set1_pd( va[3] );

    t0_00 = MM_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_1, a_im, t0_03);
    CCBLOCK;

    a_re = _mm_set1_pd( va[4] );
    a_im = _mm_set1_pd( va[5] );

    t1_00 = MM_FMADD_PD(b_0, d_re, t1_00);
    t1_01 = MM_FMADD_PD(b_1, d_re, t1_01);
    t1_02 = MM_FMADD_PD(b_0, d_im, t1_02);
    t1_03 = MM_FMADD_PD(b_1, d_im, t1_03);

    b_0 = _mm_load_pd( 4+(double*)b );
    b_1 = _mm_load_pd( 6+(double*)b );
    CCBLOCK;

    b += NR;
    va += MR;       // va is pointer to double
  }

  if (alpha!=1.0) {
    a_re = _mm_set1_pd( alpha );

    t0_00 = _mm_mul_pd ( a_re , t0_00 );
    t0_01 = _mm_mul_pd ( a_re , t0_01 );
    t0_02 = _mm_mul_pd ( a_re , t0_02 );
    t0_03 = _mm_mul_pd ( a_re , t0_03 );

    t1_00 = _mm_mul_pd ( a_re , t1_00 );
    t1_01 = _mm_mul_pd ( a_re , t1_01 );
    t1_02 = _mm_mul_pd ( a_re , t1_02 );
    t1_03 = _mm_mul_pd ( a_re , t1_03 );

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
  pp0[0] = t0_00;
  pp0[1] = t0_01;
  pp0[2] = t0_02;
  pp0[3] = t0_03;
  pp1[0] = t1_00;
  pp1[1] = t1_01;
  pp1[2] = t1_02;
  pp1[3] = t1_03;

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


#if !((MR==2)&&(NR==2))
#error "invalid MR NR"
#endif

#define LOOKAHEAD_A   4*16  // multiple of 16 = 4*MR*2 (16*8 bytes)
#define LOOKAHEAD_B   4*8   // multiple of  8 = 4*NR   (8*16 bytes)

void bli_zgemm_int_d2x2
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

#if defined(_WIN64)||defined(__LP64__)
  register __m128d a_re, a_im, d_re, d_im;
  register __m128d b_0, b_1, b_2, b_3;
#else
  __m128d a_re, a_im, d_re, d_im;
  __m128d b_0, b_1, b_2, b_3;
#endif

  const double *va = (const double *)a;

  register __m128d t0_00, t0_01, t0_02, t0_03;
  register __m128d t1_00, t1_01, t1_02, t1_03;

  t0_00 = _mm_setzero_pd();
  t0_01 = _mm_setzero_pd();
  t0_02 = _mm_setzero_pd();
  t0_03 = _mm_setzero_pd();
  t1_00 = _mm_setzero_pd();
  t1_01 = _mm_setzero_pd();
  t1_02 = _mm_setzero_pd();
  t1_03 = _mm_setzero_pd();

  const double *p0,*p1;
  __m128d pp0[2],pp1[2];

  a_re = _mm_set1_pd( va[0] );
  a_im = _mm_set1_pd( va[1] );

  b_0 = _mm_load_pd( (double*)b );
  b_1 = _mm_load_pd( 2+(double*)b );

  PREFETCH2((void*)&c[0*cs_c]);
  PREFETCH2((void*)&c[1*cs_c]);

  gint_t l;
  gint_t ki = k>>2;  // unroll
  gint_t kr = k&3;
  for (l=0; l<ki; ++l) {

// iter 0

    b_2 = _mm_load_pd( 4+(double*)b );
    PREFETCH((void*)(b+LOOKAHEAD_B )); // PREFETCH((void*)(b+2*4*NR));
    b_3 = _mm_load_pd( 6+(double*)b );
    PREFETCH((void*)(va+LOOKAHEAD_A ));

    d_re = _mm_set1_pd( va[2] );
    d_im = _mm_set1_pd( va[3] );

    t0_00 = MM_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_1, a_im, t0_03);

    a_re = _mm_set1_pd( va[4] );
    a_im = _mm_set1_pd( va[5] );

    t1_00 = MM_FMADD_PD(b_0, d_re, t1_00);
    t1_01 = MM_FMADD_PD(b_1, d_re, t1_01);
    t1_02 = MM_FMADD_PD(b_0, d_im, t1_02);
    t1_03 = MM_FMADD_PD(b_1, d_im, t1_03);
    CCBLOCK;

// iter 1

    b_0 = _mm_load_pd( 8+(double*)b );
    b_1 = _mm_load_pd( 10+(double*)b );

    d_re = _mm_set1_pd( va[6] );
    d_im = _mm_set1_pd( va[7] );

    t0_00 = MM_FMADD_PD(b_2, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_3, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_2, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_3, a_im, t0_03);

    a_re = _mm_set1_pd( va[8] );
    a_im = _mm_set1_pd( va[9] );

    t1_00 = MM_FMADD_PD(b_2, d_re, t1_00);
    t1_01 = MM_FMADD_PD(b_3, d_re, t1_01);
    t1_02 = MM_FMADD_PD(b_2, d_im, t1_02);
    t1_03 = MM_FMADD_PD(b_3, d_im, t1_03);
    CCBLOCK;

// iter 2

    b_2 = _mm_load_pd( 12+(double*)b );
    PREFETCH((void*)(b+LOOKAHEAD_B +4));
    b_3 = _mm_load_pd( 14+(double*)b );
    PREFETCH((void*)(va+LOOKAHEAD_A +8));

    d_re = _mm_set1_pd( va[10] );
    d_im = _mm_set1_pd( va[11] );

    t0_00 = MM_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_1, a_im, t0_03);

    a_re = _mm_set1_pd( va[12] );
    a_im = _mm_set1_pd( va[13] );

    t1_00 = MM_FMADD_PD(b_0, d_re, t1_00);
    t1_01 = MM_FMADD_PD(b_1, d_re, t1_01);
    t1_02 = MM_FMADD_PD(b_0, d_im, t1_02);
    t1_03 = MM_FMADD_PD(b_1, d_im, t1_03);
    CCBLOCK;

// iter 3

    b_0 = _mm_load_pd( 16+(double*)b );
    b_1 = _mm_load_pd( 18+(double*)b );

    d_re = _mm_set1_pd( va[14] );
    d_im = _mm_set1_pd( va[15] );

    t0_00 = MM_FMADD_PD(b_2, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_3, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_2, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_3, a_im, t0_03);

    a_re = _mm_set1_pd( va[16] );
    a_im = _mm_set1_pd( va[17] );

    t1_00 = MM_FMADD_PD(b_2, d_re, t1_00);
    t1_01 = MM_FMADD_PD(b_3, d_re, t1_01);
    t1_02 = MM_FMADD_PD(b_2, d_im, t1_02);
    t1_03 = MM_FMADD_PD(b_3, d_im, t1_03);
    CCBLOCK;

    b += 4*NR;        // unroll*NR
    va += 4*MR*2;     // unroll*MR  va is pointer to double
  }

  PREFETCH2((void*)&c[1*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+1*cs_c]);

  for (l=0; l<kr; ++l) {

    d_re = _mm_set1_pd( va[2] );
    d_im = _mm_set1_pd( va[3] );

    t0_00 = MM_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM_FMADD_PD(b_1, a_im, t0_03);

    a_re = _mm_set1_pd( va[4] );
    a_im = _mm_set1_pd( va[5] );

    t1_00 = MM_FMADD_PD(b_0, d_re, t1_00);
    t1_01 = MM_FMADD_PD(b_1, d_re, t1_01);
    t1_02 = MM_FMADD_PD(b_0, d_im, t1_02);
    t1_03 = MM_FMADD_PD(b_1, d_im, t1_03);

    b_0 = _mm_load_pd( 4+(double*)b );
    b_1 = _mm_load_pd( 6+(double*)b );
    CCBLOCK;

    b += NR;
    va += MR*2;       // va is pointer to double
  }

  t0_02 = _mm_shuffle_pd ( t0_02 , t0_02 , 0x1 );
  t0_03 = _mm_shuffle_pd ( t0_03 , t0_03 , 0x1 );
  t1_02 = _mm_shuffle_pd ( t1_02 , t1_02 , 0x1 );
  t1_03 = _mm_shuffle_pd ( t1_03 , t1_03 , 0x1 );

#if 0
// if sse3 available
// but apparently no speed improvement
#include <pmmintrin.h>

  t0_00 = _mm_addsub_pd ( t0_00, t0_02 );
  t0_01 = _mm_addsub_pd ( t0_01, t0_03 );

  t1_00 = _mm_addsub_pd ( t1_00, t1_02 );
  t1_01 = _mm_addsub_pd ( t1_01, t1_03 );

#else
  b_0 = _mm_sub_pd ( t0_00 , t0_02 );
  b_1 = _mm_add_pd ( t0_00 , t0_02 );

  b_2 = _mm_sub_pd ( t0_01 , t0_03 );
  b_3 = _mm_add_pd ( t0_01 , t0_03 );

  t0_00 = _mm_shuffle_pd ( b_0 , b_1 , 0x2 );
  t0_01 = _mm_shuffle_pd ( b_2 , b_3 , 0x2 );

  b_0 = _mm_sub_pd ( t1_00 , t1_02 );
  b_1 = _mm_add_pd ( t1_00 , t1_02 );

  b_2 = _mm_sub_pd ( t1_01 , t1_03 );
  b_3 = _mm_add_pd ( t1_01 , t1_03 );

  t1_00 = _mm_shuffle_pd ( b_0 , b_1 , 0x2 );
  t1_01 = _mm_shuffle_pd ( b_2 , b_3 , 0x2 );

#endif

  if (alpha.real!=1.0||alpha.imag!=0.0) {
    a_re = _mm_set1_pd( alpha.real );
    a_im = _mm_set1_pd( alpha.imag );

    t0_02 = _mm_shuffle_pd ( t0_00 , t0_00 , 0x1 );
    t0_00 = _mm_mul_pd ( a_re , t0_00 );
    t0_02 = _mm_mul_pd ( a_im , t0_02 );
    b_0 = _mm_sub_pd ( t0_00 , t0_02 );
    b_1 = _mm_add_pd ( t0_00 , t0_02 );
    t0_00 = _mm_shuffle_pd ( b_0 , b_1 , 0x2 );

    t0_02 = _mm_shuffle_pd ( t0_01 , t0_01 , 0x1 );
    t0_01 = _mm_mul_pd ( a_re , t0_01 );
    t0_02 = _mm_mul_pd ( a_im , t0_02 );
    b_0 = _mm_sub_pd ( t0_01 , t0_03 );
    b_1 = _mm_add_pd ( t0_01 , t0_03 );
    t0_01 = _mm_shuffle_pd ( b_0 , b_1 , 0x2 );

    t1_02 = _mm_shuffle_pd ( t1_00 , t1_00 , 0x1 );
    t1_00 = _mm_mul_pd ( a_re , t1_00 );
    t1_02 = _mm_mul_pd ( a_im , t1_02 );
    b_0 = _mm_sub_pd ( t1_00 , t1_02 );
    b_1 = _mm_add_pd ( t1_00 , t1_02 );
    t1_00 = _mm_shuffle_pd ( b_0 , b_1 , 0x2 );

    t1_02 = _mm_shuffle_pd ( t1_01 , t1_01 , 0x1 );
    t1_01 = _mm_mul_pd ( a_re , t1_01 );
    t1_02 = _mm_mul_pd ( a_im , t1_02 );
    b_0 = _mm_sub_pd ( t1_01 , t1_03 );
    b_1 = _mm_add_pd ( t1_01 , t1_03 );
    t1_01 = _mm_shuffle_pd ( b_0 , b_1 , 0x2 );

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
  pp0[0] = t0_00;
  pp0[1] = t0_01;
  pp1[0] = t1_00;
  pp1[1] = t1_01;

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

}

#undef MC
#undef KC
#undef NC
#undef MR
#undef NR

#undef LOOKAHEAD_A
#undef LOOKAHEAD_B

#endif  /* __SSE2__  */
