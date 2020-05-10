/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* gemm micro kernel                                                       */

#include "../blis.h"

#if BLIS_DRIVER == BLIS_DRIVER_AVX

#include <immintrin.h>

/*
 Note:
 intel avx recommend 32 bytes alignment
 intel cacheline alignment = 32 bytes
 intel cacheline size = 64 bytes
*/

#define NO_FMA 0

#if NO_FMA
#define MM256_FMADD_PD(a,b,c) _mm256_add_pd(_mm256_mul_pd(a,b),c)
#define BLI_DGEMM bli_dgemm_int_6x8
#define BLI_ZGEMM bli_zgemm_int_3x4
#else
/* clang-cl need /Arch:AVX2 to build */
#if C_AVX2 || !(defined(_WIN32) && defined(__clang__))
#define MM256_FMADD_PD(a,b,c) _mm256_fmadd_pd(a,b,c)
#else
#define MM256_FMADD_PD(a,b,c) _mm256_add_pd(_mm256_mul_pd(a,b),c)
#endif
#define BLI_DGEMM bli_dgemm2_int_6x8
#define BLI_ZGEMM bli_zgemm2_int_3x4
#endif

#define MC  BLIS_DEFAULT_MC_D
#define KC  BLIS_DEFAULT_KC_D
#define NC  BLIS_DEFAULT_NC_D
#define MR  BLIS_DEFAULT_MR_D
#define NR  BLIS_DEFAULT_NR_D

#if !((MR==6)&&(NR==8))
#error "invalid MR NR"
#endif

#define LOOKAHEAD_A   6*24  // multiple of 24 = 4*MR (24*8 bytes)
#define LOOKAHEAD_B   4*32  // multiple of 32 = 4*NR (32*8 bytes)

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

  register __m256d a_re, a_im;
  register __m256d b_0, b_1;

  const double *va = (const double *)a;

  _mm256_zeroupper();
  register __m256d t0_00, t0_01, t0_02, t0_03;
  register __m256d t1_00, t1_01, t1_02, t1_03;
  register __m256d t2_00, t2_01, t2_02, t2_03;

  t0_00 = _mm256_setzero_pd();
  t0_01 = _mm256_setzero_pd();
  t0_02 = _mm256_setzero_pd();
  t0_03 = _mm256_setzero_pd();
  t1_00 = _mm256_setzero_pd();
  t1_01 = _mm256_setzero_pd();
  t1_02 = _mm256_setzero_pd();
  t1_03 = _mm256_setzero_pd();
  t2_00 = _mm256_setzero_pd();
  t2_01 = _mm256_setzero_pd();
  t2_02 = _mm256_setzero_pd();
  t2_03 = _mm256_setzero_pd();

  const double *p0,*p1,*p2;
  __m256d pp0[4],pp1[4],pp2[4];

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

  gint_t l;
  gint_t ki = k>>2;  // unroll
  gint_t kr = k&3;
  for (l=0; l<ki; ++l) {

// iter 0

    b_0 = _mm256_load_pd( (double*)b );
    PREFETCH((void*)(b+LOOKAHEAD_B ));  //   PREFETCH((void*)(b+2*4*NR));
    b_1 = _mm256_load_pd( 4+(double*)b );
    PREFETCH((void*)(va+LOOKAHEAD_A ));

    a_re = _mm256_broadcast_sd( va );
    a_im = _mm256_broadcast_sd( va+1 );

    t0_00 = MM256_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM256_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM256_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM256_FMADD_PD(b_1, a_im, t0_03);

    a_re = _mm256_broadcast_sd( va+2 );
    a_im = _mm256_broadcast_sd( va+3 );

    t1_00 = MM256_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM256_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM256_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM256_FMADD_PD(b_1, a_im, t1_03);

    a_re = _mm256_broadcast_sd( va+4 );
    a_im = _mm256_broadcast_sd( va+5 );

    t2_00 = MM256_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM256_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM256_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM256_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

// iter 1

    b_0 = _mm256_load_pd( 8+(double*)b );
    PREFETCH((void*)(b+LOOKAHEAD_B +8));
    b_1 = _mm256_load_pd( 12+(double*)b );
    PREFETCH((void*)(va+LOOKAHEAD_A +8));

    a_re = _mm256_broadcast_sd( va+6 );
    a_im = _mm256_broadcast_sd( va+7 );

    t0_00 = MM256_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM256_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM256_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM256_FMADD_PD(b_1, a_im, t0_03);

    a_re = _mm256_broadcast_sd( va+8 );
    a_im = _mm256_broadcast_sd( va+9 );

    t1_00 = MM256_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM256_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM256_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM256_FMADD_PD(b_1, a_im, t1_03);

    a_re = _mm256_broadcast_sd( va+10 );
    a_im = _mm256_broadcast_sd( va+11 );

    t2_00 = MM256_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM256_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM256_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM256_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

// iter 2

    b_0 = _mm256_load_pd( 16+(double*)b );
    PREFETCH((void*)(b+LOOKAHEAD_B +16));
    b_1 = _mm256_load_pd( 20+(double*)b );
    PREFETCH((void*)(va+LOOKAHEAD_A +16));

    a_re = _mm256_broadcast_sd( va+12 );
    a_im = _mm256_broadcast_sd( va+13 );

    t0_00 = MM256_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM256_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM256_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM256_FMADD_PD(b_1, a_im, t0_03);

    a_re = _mm256_broadcast_sd( va+14 );
    a_im = _mm256_broadcast_sd( va+15 );

    t1_00 = MM256_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM256_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM256_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM256_FMADD_PD(b_1, a_im, t1_03);

    a_re = _mm256_broadcast_sd( va+16 );
    a_im = _mm256_broadcast_sd( va+17 );

    t2_00 = MM256_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM256_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM256_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM256_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

// iter 3

    b_0 = _mm256_load_pd( 24+(double*)b );
    PREFETCH((void*)(b+LOOKAHEAD_B +24));
    b_1 = _mm256_load_pd( 28+(double*)b );

    a_re = _mm256_broadcast_sd( va+18 );
    a_im = _mm256_broadcast_sd( va+19 );

    t0_00 = MM256_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM256_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM256_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM256_FMADD_PD(b_1, a_im, t0_03);

    a_re = _mm256_broadcast_sd( va+20 );
    a_im = _mm256_broadcast_sd( va+21 );

    t1_00 = MM256_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM256_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM256_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM256_FMADD_PD(b_1, a_im, t1_03);

    a_re = _mm256_broadcast_sd( va+22 );
    a_im = _mm256_broadcast_sd( va+23 );

    t2_00 = MM256_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM256_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM256_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM256_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

    b += 4*NR;      // unroll*NR
    va += 4*MR;     // unroll*MR  va is pointer to double
  }

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

  for (l=0; l<kr; ++l) {

// look ahead too small
//    PREFETCH((void*)(b+16));   // PREFETCH((void*)(a+2*NR));
//    PREFETCH((void*)(va+12));

    b_0 = _mm256_load_pd( (double*)b );
    b_1 = _mm256_load_pd( 4+(double*)b );

    a_re = _mm256_broadcast_sd( va );
    a_im = _mm256_broadcast_sd( va+1 );

    t0_00 = MM256_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM256_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM256_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM256_FMADD_PD(b_1, a_im, t0_03);

    a_re = _mm256_broadcast_sd( va+2 );
    a_im = _mm256_broadcast_sd( va+3 );

    t1_00 = MM256_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM256_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM256_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM256_FMADD_PD(b_1, a_im, t1_03);

    a_re = _mm256_broadcast_sd( va+4 );
    a_im = _mm256_broadcast_sd( va+5 );

    t2_00 = MM256_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM256_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM256_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM256_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

    b += NR;
    va += MR;       // va is pointer to double
  }

  if (alpha!=1.0) {
    a_re = _mm256_broadcast_sd( (double*)alpha_ );

    t0_00 = _mm256_mul_pd ( a_re , t0_00 );
    t0_01 = _mm256_mul_pd ( a_re , t0_01 );
    t0_02 = _mm256_mul_pd ( a_re , t0_02 );
    t0_03 = _mm256_mul_pd ( a_re , t0_03 );

    t1_00 = _mm256_mul_pd ( a_re , t1_00 );
    t1_01 = _mm256_mul_pd ( a_re , t1_01 );
    t1_02 = _mm256_mul_pd ( a_re , t1_02 );
    t1_03 = _mm256_mul_pd ( a_re , t1_03 );

    t2_00 = _mm256_mul_pd ( a_re , t2_00 );
    t2_01 = _mm256_mul_pd ( a_re , t2_01 );
    t2_02 = _mm256_mul_pd ( a_re , t2_02 );
    t2_03 = _mm256_mul_pd ( a_re , t2_03 );

  }

  PREFETCH2((void*)&c[5*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[5*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[5*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[5*rs_c+3*cs_c]);
  PREFETCH2((void*)&c[5*rs_c+4*cs_c]);
  PREFETCH2((void*)&c[5*rs_c+5*cs_c]);
  PREFETCH2((void*)&c[5*rs_c+6*cs_c]);
  PREFETCH2((void*)&c[5*rs_c+7*cs_c]);

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
  c[pc+=cs_c] += p0[4];
  c[pc+=cs_c] += p0[5];
  c[pc+=cs_c] += p0[6];
  c[pc+=cs_c] += p0[7];

  pc = rs_c;
  c[pc      ] += p0[8];
  c[pc+=cs_c] += p0[9];
  c[pc+=cs_c] += p0[10];
  c[pc+=cs_c] += p0[11];
  c[pc+=cs_c] += p0[12];
  c[pc+=cs_c] += p0[13];
  c[pc+=cs_c] += p0[14];
  c[pc+=cs_c] += p0[15];

  pc = 2*rs_c;
  c[pc      ] += p1[0];
  c[pc+=cs_c] += p1[1];
  c[pc+=cs_c] += p1[2];
  c[pc+=cs_c] += p1[3];
  c[pc+=cs_c] += p1[4];
  c[pc+=cs_c] += p1[5];
  c[pc+=cs_c] += p1[6];
  c[pc+=cs_c] += p1[7];

  pc = 3*rs_c;
  c[pc      ] += p1[8];
  c[pc+=cs_c] += p1[9];
  c[pc+=cs_c] += p1[10];
  c[pc+=cs_c] += p1[11];
  c[pc+=cs_c] += p1[12];
  c[pc+=cs_c] += p1[13];
  c[pc+=cs_c] += p1[14];
  c[pc+=cs_c] += p1[15];

  pc = 4*rs_c;
  c[pc      ] += p2[0];
  c[pc+=cs_c] += p2[1];
  c[pc+=cs_c] += p2[2];
  c[pc+=cs_c] += p2[3];
  c[pc+=cs_c] += p2[4];
  c[pc+=cs_c] += p2[5];
  c[pc+=cs_c] += p2[6];
  c[pc+=cs_c] += p2[7];

  pc = 5*rs_c;
  c[pc      ] += p2[8];
  c[pc+=cs_c] += p2[9];
  c[pc+=cs_c] += p2[10];
  c[pc+=cs_c] += p2[11];
  c[pc+=cs_c] += p2[12];
  c[pc+=cs_c] += p2[13];
  c[pc+=cs_c] += p2[14];
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

  register __m256d a_re, a_im;
  register __m256d b_0, b_1;

  const double *va = (const double *)a;

  _mm256_zeroupper();
  register __m256d t0_00, t0_01, t0_02, t0_03;
  register __m256d t1_00, t1_01, t1_02, t1_03;
  register __m256d t2_00, t2_01, t2_02, t2_03;

  t0_00 = _mm256_setzero_pd();
  t0_01 = _mm256_setzero_pd();
  t0_02 = _mm256_setzero_pd();
  t0_03 = _mm256_setzero_pd();
  t1_00 = _mm256_setzero_pd();
  t1_01 = _mm256_setzero_pd();
  t1_02 = _mm256_setzero_pd();
  t1_03 = _mm256_setzero_pd();
  t2_00 = _mm256_setzero_pd();
  t2_01 = _mm256_setzero_pd();
  t2_02 = _mm256_setzero_pd();
  t2_03 = _mm256_setzero_pd();

  const double *p0,*p1,*p2;
  __m256d pp0[2],pp1[2],pp2[2];

  PREFETCH2((void*)&c[0*cs_c]);
  PREFETCH2((void*)&c[1*cs_c]);
  PREFETCH2((void*)&c[2*cs_c]);
  PREFETCH2((void*)&c[3*cs_c]);

  gint_t l;
  gint_t ki = k>>2;  // unroll
  gint_t kr = k&3;
  for (l=0; l<ki; ++l) {

// iter 0

    b_0 = _mm256_load_pd( (double*)b );
    PREFETCH((void*)(b+LOOKAHEAD_B )); // PREFETCH((void*)(b+2*4*NR));
    b_1 = _mm256_load_pd( 4+(double*)b );
    PREFETCH((void*)(va+LOOKAHEAD_A ));

    a_re = _mm256_broadcast_sd( va );
    a_im = _mm256_broadcast_sd( va+1 );

    t0_00 = MM256_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM256_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM256_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM256_FMADD_PD(b_1, a_im, t0_03);

    a_re = _mm256_broadcast_sd( va+2 );
    a_im = _mm256_broadcast_sd( va+3 );

    t1_00 = MM256_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM256_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM256_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM256_FMADD_PD(b_1, a_im, t1_03);

    a_re = _mm256_broadcast_sd( va+4 );
    a_im = _mm256_broadcast_sd( va+5 );

    t2_00 = MM256_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM256_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM256_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM256_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

// iter 1

    b_0 = _mm256_load_pd( 8+(double*)b );
    PREFETCH((void*)(b+LOOKAHEAD_B +4));
    b_1 = _mm256_load_pd( 12+(double*)b );
    PREFETCH((void*)(va+LOOKAHEAD_A +8));

    a_re = _mm256_broadcast_sd( va+6 );
    a_im = _mm256_broadcast_sd( va+7 );

    t0_00 = MM256_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM256_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM256_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM256_FMADD_PD(b_1, a_im, t0_03);

    a_re = _mm256_broadcast_sd( va+8 );
    a_im = _mm256_broadcast_sd( va+9 );

    t1_00 = MM256_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM256_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM256_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM256_FMADD_PD(b_1, a_im, t1_03);

    a_re = _mm256_broadcast_sd( va+10 );
    a_im = _mm256_broadcast_sd( va+11 );

    t2_00 = MM256_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM256_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM256_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM256_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

// iter 2

    b_0 = _mm256_load_pd( 16+(double*)b );
    PREFETCH((void*)(b+LOOKAHEAD_B +8));
    b_1 = _mm256_load_pd( 20+(double*)b );
    PREFETCH((void*)(va+LOOKAHEAD_A +16));

    a_re = _mm256_broadcast_sd( va+12 );
    a_im = _mm256_broadcast_sd( va+13 );

    t0_00 = MM256_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM256_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM256_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM256_FMADD_PD(b_1, a_im, t0_03);

    a_re = _mm256_broadcast_sd( va+14 );
    a_im = _mm256_broadcast_sd( va+15 );

    t1_00 = MM256_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM256_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM256_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM256_FMADD_PD(b_1, a_im, t1_03);

    a_re = _mm256_broadcast_sd( va+16 );
    a_im = _mm256_broadcast_sd( va+17 );

    t2_00 = MM256_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM256_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM256_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM256_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

// iter 3

    b_0 = _mm256_load_pd( 24+(double*)b );
    PREFETCH((void*)(b+LOOKAHEAD_B +12));
    b_1 = _mm256_load_pd( 28+(double*)b );

    a_re = _mm256_broadcast_sd( va+18 );
    a_im = _mm256_broadcast_sd( va+19 );

    t0_00 = MM256_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM256_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM256_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM256_FMADD_PD(b_1, a_im, t0_03);

    a_re = _mm256_broadcast_sd( va+20 );
    a_im = _mm256_broadcast_sd( va+21 );

    t1_00 = MM256_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM256_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM256_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM256_FMADD_PD(b_1, a_im, t1_03);

    a_re = _mm256_broadcast_sd( va+22 );
    a_im = _mm256_broadcast_sd( va+23 );

    t2_00 = MM256_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM256_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM256_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM256_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

    b += 4*NR;        // unroll*NR
    va += 4*MR*2;     // unroll*MR  va is pointer to double
  }

  PREFETCH2((void*)&c[1*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[1*rs_c+3*cs_c]);

  for (l=0; l<kr; ++l) {

// look ahead too small
//    PREFETCH((void*)(b+8)); // PREFETCH((void*)(b+2*NR));
//    PREFETCH((void*)(va+12));

    b_0 = _mm256_load_pd( (double*)b );
    b_1 = _mm256_load_pd( 4+(double*)b );

    a_re = _mm256_broadcast_sd( va );
    a_im = _mm256_broadcast_sd( va+1 );

    t0_00 = MM256_FMADD_PD(b_0, a_re, t0_00);
    t0_01 = MM256_FMADD_PD(b_1, a_re, t0_01);
    t0_02 = MM256_FMADD_PD(b_0, a_im, t0_02);
    t0_03 = MM256_FMADD_PD(b_1, a_im, t0_03);

    a_re = _mm256_broadcast_sd( va+2 );
    a_im = _mm256_broadcast_sd( va+3 );

    t1_00 = MM256_FMADD_PD(b_0, a_re, t1_00);
    t1_01 = MM256_FMADD_PD(b_1, a_re, t1_01);
    t1_02 = MM256_FMADD_PD(b_0, a_im, t1_02);
    t1_03 = MM256_FMADD_PD(b_1, a_im, t1_03);

    a_re = _mm256_broadcast_sd( va+4 );
    a_im = _mm256_broadcast_sd( va+5 );

    t2_00 = MM256_FMADD_PD(b_0, a_re, t2_00);
    t2_01 = MM256_FMADD_PD(b_1, a_re, t2_01);
    t2_02 = MM256_FMADD_PD(b_0, a_im, t2_02);
    t2_03 = MM256_FMADD_PD(b_1, a_im, t2_03);
    CCBLOCK;

    b += NR;
    va += MR*2;     // unroll*MR  va is pointer to double
  }

  PREFETCH2((void*)&c[2*rs_c+0*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+1*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+2*cs_c]);
  PREFETCH2((void*)&c[2*rs_c+3*cs_c]);

  t0_02 = _mm256_permute_pd ( t0_02 , 0x5 );
  t0_03 = _mm256_permute_pd ( t0_03 , 0x5 );

  t1_02 = _mm256_permute_pd ( t1_02 , 0x5 );
  t1_03 = _mm256_permute_pd ( t1_03 , 0x5 );

  t2_02 = _mm256_permute_pd ( t2_02 , 0x5 );
  t2_03 = _mm256_permute_pd ( t2_03 , 0x5 );

  t0_00 = _mm256_addsub_pd ( t0_00 , t0_02 );
  t0_01 = _mm256_addsub_pd ( t0_01 , t0_03 );

  t1_00 = _mm256_addsub_pd ( t1_00 , t1_02 );
  t1_01 = _mm256_addsub_pd ( t1_01 , t1_03 );

  t2_00 = _mm256_addsub_pd ( t2_00 , t2_02 );
  t2_01 = _mm256_addsub_pd ( t2_01 , t2_03 );

  if (alpha.real!=1.0||alpha.imag!=0.0) {
    a_re = _mm256_broadcast_sd( (double*)alpha_ );
    a_im = _mm256_broadcast_sd( 1+(double*)alpha_ );

    t0_02 = _mm256_permute_pd ( t0_00 , 0x5 );
    t0_03 = _mm256_permute_pd ( t0_01 , 0x5 );

    t1_02 = _mm256_permute_pd ( t1_00 , 0x5 );
    t1_03 = _mm256_permute_pd ( t1_01 , 0x5 );

    t2_02 = _mm256_permute_pd ( t2_00 , 0x5 );
    t2_03 = _mm256_permute_pd ( t2_01 , 0x5 );

    t0_00 = _mm256_mul_pd ( a_re , t0_00 );
    t0_01 = _mm256_mul_pd ( a_re , t0_01 );
    t1_00 = _mm256_mul_pd ( a_re , t1_00 );
    t1_01 = _mm256_mul_pd ( a_re , t1_01 );
    t2_00 = _mm256_mul_pd ( a_re , t2_00 );
    t2_01 = _mm256_mul_pd ( a_re , t2_01 );

    t0_02 = _mm256_mul_pd ( a_im , t0_02 );
    t0_03 = _mm256_mul_pd ( a_im , t0_03 );
    t1_02 = _mm256_mul_pd ( a_im , t1_02 );
    t1_03 = _mm256_mul_pd ( a_im , t1_03 );
    t2_02 = _mm256_mul_pd ( a_im , t2_02 );
    t2_03 = _mm256_mul_pd ( a_im , t2_03 );

    t0_00 = _mm256_addsub_pd ( t0_00, t0_02 );
    t0_01 = _mm256_addsub_pd ( t0_01, t0_03 );
    t1_00 = _mm256_addsub_pd ( t1_00, t1_02 );
    t1_01 = _mm256_addsub_pd ( t1_01, t1_03 );
    t2_00 = _mm256_addsub_pd ( t2_00, t2_02 );
    t2_01 = _mm256_addsub_pd ( t2_01, t2_03 );

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
  c[pc+=cs_c].real += p0[4];
  c[pc      ].imag += p0[5];
  c[pc+=cs_c].real += p0[6];
  c[pc      ].imag += p0[7];

  pc = rs_c;
  c[pc      ].real += p1[0];
  c[pc      ].imag += p1[1];
  c[pc+=cs_c].real += p1[2];
  c[pc      ].imag += p1[3];
  c[pc+=cs_c].real += p1[4];
  c[pc      ].imag += p1[5];
  c[pc+=cs_c].real += p1[6];
  c[pc      ].imag += p1[7];

  pc = 2*rs_c;
  c[pc      ].real += p2[0];
  c[pc      ].imag += p2[1];
  c[pc+=cs_c].real += p2[2];
  c[pc      ].imag += p2[3];
  c[pc+=cs_c].real += p2[4];
  c[pc      ].imag += p2[5];
  c[pc+=cs_c].real += p2[6];
  c[pc      ].imag += p2[7];

}

#undef MC
#undef KC
#undef NC
#undef MR
#undef NR

#undef LOOKAHEAD_A
#undef LOOKAHEAD_B

#endif   /*  C_AVX  */
