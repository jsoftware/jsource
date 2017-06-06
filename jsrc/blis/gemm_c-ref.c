/* Copyright 1990-2016, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* gemm micro kernel                                                       */

#include "../blis.h"

#if 1 // BLIS_DRIVER == BLIS_DRIVER_GENERIC

#ifdef TYMES
#undef TYMES
#endif
#ifdef ZRE
#undef ZRE
#endif
#ifdef ZIM
#undef ZIM
#endif
#define TYMES(u,v)      ((u)*(v))   /* let it failed on 0 * _ as in other micro kernel */
#define ZRE(x,y)        ((TYMES((x).real,(y).real))-TYMES((x).imag,(y).imag))
#define ZIM(x,y)        ((TYMES((x).real,(y).imag))+TYMES((x).imag,(y).real))

#define MC  BLIS_DEFAULT_MC_D
#define KC  BLIS_DEFAULT_KC_D
#define NC  BLIS_DEFAULT_NC_D

#define MR  BLIS_DEFAULT_MR_D
#define NR  BLIS_DEFAULT_NR_D


// -----------------------------------------------------------------
// dgemm reference microkernel

//
//  Micro kernel for multiplying panels from A and B.
//
void
bli_dgemm_ukn_ref(dim_t kc,
                  double * restrict alpha_, double * restrict A, double * restrict B,
                  double * restrict beta_,
                  double * restrict C, inc_t rs_c, inc_t cs_c,
                  auxinfo_t* restrict data,
                  cntx_t*    restrict cntx)
{
  double AB[MR*NR];
  double alpha=*alpha_, beta=*beta_;

//
//  Compute AB = A*B
//
  dim_t rl;
  for (rl=0; rl<MR*NR; ++rl) {
    AB[rl] = 0;
  }
  dim_t l;
  for (l=0; l<kc; ++l) {
    dim_t j;
    for (j=0; j<NR; ++j) {
      dim_t i;
      for (i=0; i<MR; ++i) {
        AB[i+j*MR] += A[i]*B[j];
      }
    }
    A += MR;
    B += NR;
  }

//
//  Update C <- beta*C
//
  if (beta==0.0) {
    dim_t j;
    for (j=0; j<NR; ++j) {
      dim_t i;
      for (i=0; i<MR; ++i) {
        C[i*rs_c+j*cs_c] = 0.0;
      }
    }
  } else if (beta!=1.0) {
    dim_t j;
    for (j=0; j<NR; ++j) {
      dim_t i;
      for (i=0; i<MR; ++i) {
        C[i*rs_c+j*cs_c] *= beta;
      }
    }
  }

//
//  Update C <- C + alpha*AB (note: the case alpha==0.0 was already treated in
//                                  the above layer dgemm_macro)
//
  if (alpha==1.0) {
    dim_t j;
    for (j=0; j<NR; ++j) {
      dim_t i;
      for (i=0; i<MR; ++i) {
        C[i*rs_c+j*cs_c] += AB[i+j*MR];
      }
    }
  } else {
    dim_t j;
    for (j=0; j<NR; ++j) {
      dim_t i;
      for (i=0; i<MR; ++i) {
        C[i*rs_c+j*cs_c] += alpha*AB[i+j*MR];
      }
    }
  }
}


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


// -----------------------------------------------------------------
// zgemm reference microkernel

//
//  Micro kernel for multiplying panels from A and B.
//
void
bli_zgemm_ukn_ref(dim_t kc,
                  dcomplex * restrict alpha_, dcomplex * restrict A, dcomplex * restrict B,
                  dcomplex * restrict beta_,
                  dcomplex * restrict C, inc_t rs_c, inc_t cs_c,
                  auxinfo_t* restrict data,
                  cntx_t*    restrict cntx)
{
  dcomplex AB[MR*NR];
  dcomplex alpha=*alpha_, beta=*beta_;

//
//  Compute AB = A*B
//
  dim_t rl;
  for (rl=0; rl<MR*NR; ++rl) {
    AB[rl].real = 0.0;
    AB[rl].imag = 0.0;
  }
  dim_t l;
  for (l=0; l<kc; ++l) {
    dim_t j;
    for (j=0; j<NR; ++j) {
      dim_t i;
      for (i=0; i<MR; ++i) {
        AB[i+j*MR].real += ZRE(A[i],B[j]);
        AB[i+j*MR].imag += ZIM(A[i],B[j]);
      }
    }
    A += MR;
    B += NR;
  }

//
//  Update C <- beta*C
//
  if (beta.real==0.0&&beta.imag==0.0) {
    dim_t j;
    for (j=0; j<NR; ++j) {
      dim_t i;
      for (i=0; i<MR; ++i) {
        C[i*rs_c+j*cs_c].real = 0.0;
        C[i*rs_c+j*cs_c].imag = 0.0;
      }
    }
  } else if (beta.real!=1.0||beta.imag!=0.0) {
    dim_t j;
    for (j=0; j<NR; ++j) {
      dim_t i;
      for (i=0; i<MR; ++i) {
        C[i*rs_c+j*cs_c].real = ZRE(C[i*rs_c+j*cs_c], beta);
        C[i*rs_c+j*cs_c].imag = ZIM(C[i*rs_c+j*cs_c], beta);
      }
    }
  }

//
//  Update C <- C + alpha*AB (note: the case alpha==0.0 was already treated in
//                                  the above layer dgemm_macro)
//
  if (alpha.real==1.0&&alpha.imag==0.0) {
    dim_t j;
    for (j=0; j<NR; ++j) {
      dim_t i;
      for (i=0; i<MR; ++i) {
        C[i*rs_c+j*cs_c].real += AB[i+j*MR].real;
        C[i*rs_c+j*cs_c].imag += AB[i+j*MR].imag;
      }
    }
  } else {
    dim_t j;
    for (j=0; j<NR; ++j) {
      dim_t i;
      for (i=0; i<MR; ++i) {
        C[i*rs_c+j*cs_c].real += ZRE(alpha, AB[i+j*MR]);
        C[i*rs_c+j*cs_c].imag += ZIM(alpha, AB[i+j*MR]);
      }
    }
  }
}

#undef MC
#undef KC
#undef NC
#undef MR
#undef NR

#endif
