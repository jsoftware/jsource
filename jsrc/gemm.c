/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* gemm macro kernel                                                       */

#include <stddef.h>
#include <stdint.h>
 
#include "j.h"
#include "gemm.h"
#ifdef MC
#undef MC
#endif
#ifdef ZRE
#undef ZRE
#endif
#ifdef ZIM
#undef ZIM
#endif
#define ZRE(x,y)        ((TYMES((x).real,(y).real))-TYMES((x).imag,(y).imag))
#define ZIM(x,y)        ((TYMES((x).real,(y).imag))+TYMES((x).imag,(y).real))

#include "blis.h"


#define MC  BLIS_DEFAULT_MC_D
#define KC  BLIS_DEFAULT_KC_D
#define NC  BLIS_DEFAULT_NC_D
#define MR  BLIS_DEFAULT_MR_D
#define NR  BLIS_DEFAULT_NR_D


//
//  Packing complete panels from A (i.e. without padding)
//
static void
pack_MRxk(dim_t k, const double *A, inc_t rs_a, inc_t cs_a, double *buffer)
{
    dim_t j;
    for (j=0; j<k; ++j) {
        dim_t i;
        for (i=0; i<MR; ++i) {
            buffer[i] = A[i*rs_a];
        }
        buffer += MR;
        A      += cs_a;
    }
}

//
//  Packing panels from A with padding if required
//
static void
pack_A(dim_t mc, dim_t kc, const double *A, inc_t rs_a, inc_t cs_a, double *buffer)
{
    dim_t mp  = mc / MR;
    dim_t _mr = mc % MR;

    dim_t ri;
    for (ri=0; ri<mp; ++ri) {
        pack_MRxk(kc, A, rs_a, cs_a, buffer);
        buffer += kc*MR;
        A      += MR*rs_a;
    }
    if (_mr>0) {
        dim_t j;
        for (j=0; j<kc; ++j) {
            dim_t i;
            for (i=0; i<_mr; ++i) {
                buffer[i] = A[i*rs_a];
            }
            for (i=_mr; i<MR; ++i) {
                buffer[i] = 0.0;
            }
            buffer += MR;
            A      += cs_a;
        }
    }
}

//
//  Packing complete panels from B (i.e. without padding)
//
static void
pack_kxNR(dim_t k, const double *B, inc_t rs_b, inc_t cs_b, double *buffer)
{
    dim_t i;
    for (i=0; i<k; ++i) {
        dim_t j;
        for (j=0; j<NR; ++j) {
            buffer[j] = B[j*cs_b];
        }
        buffer += NR;
        B      += rs_b;
    }
}

//
//  Packing panels from B with padding if required
//
static void
pack_B(dim_t kc, dim_t nc, const double *B, inc_t rs_b, inc_t cs_b, double *buffer)
{
    dim_t np  = nc / NR;
    dim_t _nr = nc % NR;

    dim_t rj;
    for (rj=0; rj<np; ++rj) {
        pack_kxNR(kc, B, rs_b, cs_b, buffer);
        buffer += kc*NR;
        B      += NR*cs_b;
    }
    if (_nr>0) {
        dim_t i;
        for (i=0; i<kc; ++i) {
            dim_t j;
            for (j=0; j<_nr; ++j) {
                buffer[j] = B[j*cs_b];
            }
            for (j=_nr; j<NR; ++j) {
                buffer[j] = 0.0;
            }
            buffer += NR;
            B      += rs_b;
        }
    }
}


//
//  Compute Y += alpha*X
//
static void
dgeaxpy(dim_t         m,
        dim_t         n,
        double        alpha,
        const double  *X,
        inc_t         rs_x,
        inc_t         cs_x,
        double        *Y,
        inc_t         rs_y,
        inc_t         cs_y)
{
    if (alpha!=1.0) {
        dim_t j;
        for (j=0; j<n; ++j) {
            dim_t i;
            for (i=0; i<m; ++i) {
                Y[i*rs_y+j*cs_y] += alpha*X[i*rs_x+j*cs_x];
            }
        }
    } else {
        dim_t j;
        for (j=0; j<n; ++j) {
            dim_t i;
            for (i=0; i<m; ++i) {
                Y[i*rs_y+j*cs_y] += X[i*rs_x+j*cs_x];
            }
        }
    }
}

//
//  Compute X *= beta
//
static void
dgescal(dim_t   m,
        dim_t   n,
        double  beta,
        double  *X,
        inc_t   rs_x,
        inc_t   cs_x)
{
// since J always set beta=0 and C is zero initialized
// no need to check 0 * inf
    if (beta!=0.0) {
        dim_t j;
        for (j=0; j<n; ++j) {
            dim_t i;
            for (i=0; i<m; ++i) {
                X[i*rs_x+j*cs_x] *= beta;
            }
        }
    } else {
        dim_t j;
        for (j=0; j<n; ++j) {
            dim_t i;
            for (i=0; i<m; ++i) {
                X[i*rs_x+j*cs_x] = 0.0;
            }
        }
    }
}

//
//  Macro Kernel for the multiplication of blocks of A and B.  We assume that
//  these blocks were previously packed to buffers _A and _B.
//
static void
dgemm_macro_kernel(dim_t   mc,
                   dim_t   nc,
                   dim_t   kc,
                   double  alpha,
                   double  beta,
                   double  *_A,
                   double  *_B,
                   double  *C,
                   inc_t   rs_c,
                   inc_t   cs_c)
{
    auxinfo_t auxdata;
    dim_t mp = (mc+MR-1) / MR;
    dim_t np = (nc+NR-1) / NR;

    dim_t _mr = mc % MR;
    dim_t _nr = nc % NR;

    const double *nextA;
    const double *nextB;

    double * _C = aligned_malloc(MR*NR*SZD, alignv);
    memset((void*)_C,0,MR*NR*SZD);  // must initialize memory

    dim_t j;
    for (j=0; j<np; ++j) {
        dim_t nr;
        nr    = (j!=np-1 || _nr==0) ? NR : _nr;
        nextB = &_B[j*kc*NR];
        dim_t i;
        for (i=0; i<mp; ++i) {
            dim_t mr;
            mr    = (i!=mp-1 || _mr==0) ? MR : _mr;
            nextA = &_A[(i+1)*kc*MR];

            if (i==mp-1) {
                nextA = _A;
                nextB = &_B[(j+1)*kc*NR];
                if (j==np-1) {
                    nextB = _B;
                }
            }
            bli_auxinfo_set_next_a(&auxdata, nextA)
            bli_auxinfo_set_next_b(&auxdata, nextB)
            if (mr==MR && nr==NR) {
                ((hwfma)?dgemm2_micro_kernel:dgemm_micro_kernel)(kc, &alpha, &_A[i*kc*MR], &_B[j*kc*NR],
                                   &beta,
                                   &C[i*MR*rs_c+j*NR*cs_c],
                                   rs_c, cs_c,
                                   &auxdata, 0);
            } else {
                ((hwfma)?dgemm2_micro_kernel:dgemm_micro_kernel)(kc, &alpha, &_A[i*kc*MR], &_B[j*kc*NR],
                                   (double*)&dzero,
                                   _C, 1, MR,
                                   &auxdata, 0);
                dgescal(mr, nr, beta,
                        &C[i*MR*rs_c+j*NR*cs_c], rs_c, cs_c);
                dgeaxpy(mr, nr, 1.0, _C, 1, MR,
                        &C[i*MR*rs_c+j*NR*cs_c], rs_c, cs_c);
            }
        }
    }
    aligned_free( _C );
}

//
//  Compute C <- beta*C + alpha*A*B
//
void
dgemm_nn         (I              m,
                  I              n,
                  I              k,
                  double         alpha,
                  double         *A,
                  I              rs_a,
                  I              cs_a,
                  double         *B,
                  I              rs_b,
                  I              cs_b,
                  double         beta,
                  double         *C,
                  I              rs_c,
                  I              cs_c)
{
    I mb = (m+MC-1) / MC;
    I nb = (n+NC-1) / NC;
    I kb = (k+KC-1) / KC;

    I _mc = m % MC;
    I _nc = n % NC;
    I _kc = k % KC;

    double _beta;

    if (alpha==0.0 || k==0) {
        dgescal(m, n, beta, C, rs_c, cs_c);
        return;
    }

// loop 5
    I j;
    for (j=0; j<nb; ++j) {
        I nc;
        nc = (j!=nb-1 || _nc==0) ? NC : _nc;

// loop 4
        I l;
        for (l=0; l<kb; ++l) {
            I kc;
            kc    = (l!=kb-1 || _kc==0) ? KC   : _kc;
            _beta = (l==0) ? beta : 1.0;

            double * _B = aligned_malloc((KC+1)*NC*SZD, alignv);  /* extra bytes for pre-read */
            pack_B(kc, nc,
                   &B[l*KC*rs_b+j*NC*cs_b], rs_b, cs_b,
                   _B);

// loop 3
            I i=0;
#pragma omp parallel for default(none),private(i),shared(j,l,A,C,mb,nc,kc,alpha,_beta,_mc,_B,rs_a,cs_a,rs_c,cs_c)
            for (i=0; i<mb; ++i) {
                I mc;
                mc = (i!=mb-1 || _mc==0) ? MC : _mc;

                double * _A = aligned_malloc(MC*(KC+1)*SZD, alignv);
                pack_A(mc, kc,
                       &A[i*MC*rs_a+l*KC*cs_a], rs_a, cs_a,
                       _A);

                dgemm_macro_kernel(mc, nc, kc, alpha, _beta, _A, _B,
                                   &C[i*MC*rs_c+j*NC*cs_c],
                                   rs_c, cs_c);
                aligned_free( _A );
            }
            aligned_free( _B );
        }
    }
}


// -----------------------------------------------------------------
// INT matrix

/*
#if defined(_WIN64)||defined(__LP64__)
typedef long long I;
#else
typedef long I;
#endif
*/

//
//  Packing complete panels from A (i.e. without padding)
//
static void
ipack_MRxk(dim_t k, const I *A, inc_t rs_a, inc_t cs_a, double *buffer)
{
    dim_t j;
    for (j=0; j<k; ++j) {
        dim_t i;
        for (i=0; i<MR; ++i) {
            buffer[i] = (double)A[i*rs_a];
        }
        buffer += MR;
        A      += cs_a;
    }
}

//
//  Packing panels from A with padding if required
//
static void
ipack_A(dim_t mc, dim_t kc, const I *A, inc_t rs_a, inc_t cs_a, double *buffer)
{
    dim_t mp  = mc / MR;
    dim_t _mr = mc % MR;

    dim_t ri;
    for (ri=0; ri<mp; ++ri) {
        ipack_MRxk(kc, A, rs_a, cs_a, buffer);
        buffer += kc*MR;
        A      += MR*rs_a;
    }
    if (_mr>0) {
        dim_t j;
        for (j=0; j<kc; ++j) {
            dim_t i;
            for (i=0; i<_mr; ++i) {
                buffer[i] = (double)A[i*rs_a];
            }
            for (i=_mr; i<MR; ++i) {
                buffer[i] = 0.0;
            }
            buffer += MR;
            A      += cs_a;
        }
    }
}

//
//  Packing complete panels from B (i.e. without padding)
//
static void
ipack_kxNR(dim_t k, const I *B, inc_t rs_b, inc_t cs_b, double *buffer)
{
    dim_t i;
    for (i=0; i<k; ++i) {
        dim_t j;
        for (j=0; j<NR; ++j) {
            buffer[j] = (double)B[j*cs_b];
        }
        buffer += NR;
        B      += rs_b;
    }
}

//
//  Packing panels from B with padding if required
//
static void
ipack_B(dim_t kc, dim_t nc, const I *B, inc_t rs_b, inc_t cs_b, double *buffer)
{
    dim_t np  = nc / NR;
    dim_t _nr = nc % NR;

    dim_t rj;
    for (rj=0; rj<np; ++rj) {
        ipack_kxNR(kc, B, rs_b, cs_b, buffer);
        buffer += kc*NR;
        B      += NR*cs_b;
    }
    if (_nr>0) {
        dim_t i;
        for (i=0; i<kc; ++i) {
            dim_t j;
            for (j=0; j<_nr; ++j) {
                buffer[j] = (double)B[j*cs_b];
            }
            for (j=_nr; j<NR; ++j) {
                buffer[j] = 0.0;
            }
            buffer += NR;
            B      += rs_b;
        }
    }
}


//
//  Compute C <- beta*C + alpha*A*B
//
void
igemm_nn         (I              m,
                  I              n,
                  I              k,
                  I              alpha,
                  I              *A,
                  I              rs_a,
                  I              cs_a,
                  I              *B,
                  I              rs_b,
                  I              cs_b,
                  I              beta,
                  double         *C,
                  I              rs_c,
                  I              cs_c)
{
    I mb = (m+MC-1) / MC;
    I nb = (n+NC-1) / NC;
    I kb = (k+KC-1) / KC;

    I _mc = m % MC;
    I _nc = n % NC;
    I _kc = k % KC;

    double _beta;

    if (alpha==0 || k==0) {
        dgescal(m, n, (double)beta, C, rs_c, cs_c);
        return;
    }

// loop 5
    I j;
    for (j=0; j<nb; ++j) {
        I nc;
        nc = (j!=nb-1 || _nc==0) ? NC : _nc;

// loop 4
        I l;
        for (l=0; l<kb; ++l) {
            I kc;
            kc    = (l!=kb-1 || _kc==0) ? KC   : _kc;
            _beta = (l==0) ? (double)beta : 1.0;

            double * _B = aligned_malloc((1+KC)*NC*SZD, alignv);
            ipack_B(kc, nc,
                   &B[l*KC*rs_b+j*NC*cs_b], rs_b, cs_b,
                   _B);

// loop 3
            I i=0;
#pragma omp parallel for default(none),private(i),shared(j,l,A,C,mb,nc,kc,alpha,_beta,_mc,_B,rs_a,cs_a,rs_c,cs_c)
            for (i=0; i<mb; ++i) {
                I mc;
                mc = (i!=mb-1 || _mc==0) ? MC : _mc;

                double * _A = aligned_malloc(MC*(1+KC)*SZD, alignv);
                ipack_A(mc, kc,
                       &A[i*MC*rs_a+l*KC*cs_a], rs_a, cs_a,
                       _A);

                dgemm_macro_kernel(mc, nc, kc, (double)alpha, _beta, _A, _B,
                                   &C[i*MC*rs_c+j*NC*cs_c],
                                   rs_c, cs_c);
                aligned_free( _A );
            }
            aligned_free( _B );
        }
    }
}

// -----------------------------------------------------------------
// COMPLEX matrix

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

//
//  Packing complete panels from A (i.e. without padding)
//
static void
zpack_MRxk(dim_t k, const dcomplex *A, inc_t rs_a, inc_t cs_a, dcomplex *buffer)
{
    dim_t j;
    for (j=0; j<k; ++j) {
        dim_t i;
        for (i=0; i<MR; ++i) {
            buffer[i] = A[i*rs_a];
        }
        buffer += MR;
        A      += cs_a;
    }
}

//
//  Packing panels from A with padding if required
//
static void
zpack_A(dim_t mc, dim_t kc, const dcomplex *A, inc_t rs_a, inc_t cs_a, dcomplex *buffer)
{
    dim_t mp  = mc / MR;
    dim_t _mr = mc % MR;

    dim_t ri;
    for (ri=0; ri<mp; ++ri) {
        zpack_MRxk(kc, A, rs_a, cs_a, buffer);
        buffer += kc*MR;
        A      += MR*rs_a;
    }
    if (_mr>0) {
        dim_t j;
        for (j=0; j<kc; ++j) {
            dim_t i;
            for (i=0; i<_mr; ++i) {
                buffer[i] = A[i*rs_a];
            }
            for (i=_mr; i<MR; ++i) {
                buffer[i] = zzero;
            }
            buffer += MR;
            A      += cs_a;
        }
    }
}

//
//  Packing complete panels from B (i.e. without padding)
//
static void
zpack_kxNR(dim_t k, const dcomplex *B, inc_t rs_b, inc_t cs_b, dcomplex *buffer)
{
    dim_t i;
    for (i=0; i<k; ++i) {
        dim_t j;
        for (j=0; j<NR; ++j) {
            buffer[j] = B[j*cs_b];
        }
        buffer += NR;
        B      += rs_b;
    }
}

//
//  Packing panels from B with padding if required
//
static void
zpack_B(dim_t kc, dim_t nc, const dcomplex *B, inc_t rs_b, inc_t cs_b, dcomplex *buffer)
{
    dim_t np  = nc / NR;
    dim_t _nr = nc % NR;

    dim_t rj;
    for (rj=0; rj<np; ++rj) {
        zpack_kxNR(kc, B, rs_b, cs_b, buffer);
        buffer += kc*NR;
        B      += NR*cs_b;
    }
    if (_nr>0) {
        dim_t i;
        for (i=0; i<kc; ++i) {
            dim_t j;
            for (j=0; j<_nr; ++j) {
                buffer[j] = B[j*cs_b];
            }
            for (j=_nr; j<NR; ++j) {
                buffer[j] = zzero;
            }
            buffer += NR;
            B      += rs_b;
        }
    }
}

//
//  Compute Y += alpha*X
//
static void
zgeaxpy(dim_t         m,
        dim_t         n,
        dcomplex      alpha,
        const dcomplex *X,
        inc_t         rs_x,
        inc_t         cs_x,
        dcomplex      *Y,
        inc_t         rs_y,
        inc_t         cs_y)
{
    if (alpha.real!=1.0||alpha.imag!=0.0) {
        dim_t j;
        for (j=0; j<n; ++j) {
            dim_t i;
            for (i=0; i<m; ++i) {
//              Y[i*rs_y+j*cs_y] += alpha*X[i*rs_x+j*cs_x];
                Y[i*rs_y+j*cs_y].real += ZRE(alpha,X[i*rs_x+j*cs_x]);
                Y[i*rs_y+j*cs_y].real += ZIM(alpha,X[i*rs_x+j*cs_x]);
            }
        }
    } else {
        dim_t j;
        for (j=0; j<n; ++j) {
            dim_t i;
            for (i=0; i<m; ++i) {
//              Y[i*rs_y+j*cs_y] += X[i*rs_x+j*cs_x];
                Y[i*rs_y+j*cs_y].real += X[i*rs_x+j*cs_x].real;
                Y[i*rs_y+j*cs_y].imag += X[i*rs_x+j*cs_x].imag;
            }
        }
    }
}

//
//  Compute X *= beta
//
static void
zgescal(dim_t   m,
        dim_t   n,
        dcomplex beta,
        dcomplex *X,
        inc_t   rs_x,
        inc_t   cs_x)
{
// since J always set beta=0 and C is zero initialized
// no need to check 0 * inf
    if (beta.real!=0.0||beta.imag!=0.0) {
        dim_t j;
        for (j=0; j<n; ++j) {
            dim_t i;
            for (i=0; i<m; ++i) {
//              X[i*rs_x+j*cs_x] *= beta;
                X[i*rs_x+j*cs_x].real = ZRE(X[i*rs_x+j*cs_x], beta);
                X[i*rs_x+j*cs_x].imag = ZIM(X[i*rs_x+j*cs_x], beta);
            }
        }
    } else {
        dim_t j;
        for (j=0; j<n; ++j) {
            dim_t i;
            for (i=0; i<m; ++i) {
                X[i*rs_x+j*cs_x] = zzero;
            }
        }
    }
}


//
//  Macro Kernel for the multiplication of blocks of A and B.  We assume that
//  these blocks were previously packed to buffers _A and _B.
//
static void
zgemm_macro_kernel(dim_t   mc,
                   dim_t   nc,
                   dim_t   kc,
                   dcomplex alpha,
                   dcomplex beta,
                   dcomplex *_A,
                   dcomplex *_B,
                   dcomplex *C,
                   inc_t   rs_c,
                   inc_t   cs_c)
{
    auxinfo_t auxdata;
    dim_t mp = (mc+MR-1) / MR;
    dim_t np = (nc+NR-1) / NR;

    dim_t _mr = mc % MR;
    dim_t _nr = nc % NR;

    const dcomplex *nextA;
    const dcomplex *nextB;

    dcomplex * _C = aligned_malloc(2*MR*NR*SZD, alignv);
    memset((void*)_C,0,2*MR*NR*SZD);  // must initialize memory

    dim_t j;
    for (j=0; j<np; ++j) {
        dim_t nr;
        nr    = (j!=np-1 || _nr==0) ? NR : _nr;
        nextB = &_B[j*kc*NR];

        dim_t i;
        for (i=0; i<mp; ++i) {
            dim_t mr;
            mr    = (i!=mp-1 || _mr==0) ? MR : _mr;
            nextA = &_A[(i+1)*kc*MR];

            if (i==mp-1) {
                nextA = _A;
                nextB = &_B[(j+1)*kc*NR];
                if (j==np-1) {
                    nextB = _B;
                }
            }

            bli_auxinfo_set_next_a(&auxdata, nextA)
            bli_auxinfo_set_next_b(&auxdata, nextB)
            if (mr==MR && nr==NR) {
                ((hwfma)?zgemm2_micro_kernel:zgemm_micro_kernel)(kc, &alpha, &_A[i*kc*MR], &_B[j*kc*NR],
                                   &beta,
                                   &C[i*MR*rs_c+j*NR*cs_c],
                                   rs_c, cs_c,
                                   &auxdata, 0);
            } else {
                ((hwfma)?zgemm2_micro_kernel:zgemm_micro_kernel)(kc, &alpha, &_A[i*kc*MR], &_B[j*kc*NR],
                                   (dcomplex*)&zzero,
                                   _C, 1, MR,
                                   &auxdata, 0);
                zgescal(mr, nr, beta,
                        &C[i*MR*rs_c+j*NR*cs_c], rs_c, cs_c);
                zgeaxpy(mr, nr, zone, _C, 1, MR,
                        &C[i*MR*rs_c+j*NR*cs_c], rs_c, cs_c);
            }
        }
    }
    aligned_free( _C );
}

//
//  Compute C <- beta*C + alpha*A*B
//
void
zgemm_nn         (I              m,
                  I              n,
                  I              k,
                  dcomplex       alpha,
                  dcomplex       *A,
                  I              rs_a,
                  I              cs_a,
                  dcomplex       *B,
                  I              rs_b,
                  I              cs_b,
                  dcomplex       beta,
                  dcomplex       *C,
                  I              rs_c,
                  I              cs_c)
{
    I mb = (m+MC-1) / MC;
    I nb = (n+NC-1) / NC;
    I kb = (k+KC-1) / KC;

    I _mc = m % MC;
    I _nc = n % NC;
    I _kc = k % KC;

    dcomplex _beta;

    if ((alpha.real==0.0 && alpha.imag==0.0) || k==0) {
        zgescal(m, n, beta, C, rs_c, cs_c);
        return;
    }

// loop 5
    I j;
    for (j=0; j<nb; ++j) {
        I nc;
        nc = (j!=nb-1 || _nc==0) ? NC : _nc;

// loop 4
        I l;
        for (l=0; l<kb; ++l) {
            I kc;
            kc    = (l!=kb-1 || _kc==0) ? KC   : _kc;
            _beta = (l==0) ? beta : zone;

            dcomplex * _B = aligned_malloc(2*(1+KC)*NC*SZD, alignv);
            zpack_B(kc, nc,
                   &B[l*KC*rs_b+j*NC*cs_b], rs_b, cs_b,
                   _B);

// loop 3
            I i=0;
#pragma omp parallel for default(none),private(i),shared(j,l,A,C,mb,nc,kc,alpha,_beta,_mc,_B,rs_a,cs_a,rs_c,cs_c)
            for (i=0; i<mb; ++i) {
                I mc;
                mc = (i!=mb-1 || _mc==0) ? MC : _mc;

                dcomplex * _A = aligned_malloc(2*MC*(1+KC)*SZD, alignv);
                zpack_A(mc, kc,
                       &A[i*MC*rs_a+l*KC*cs_a], rs_a, cs_a,
                       _A);

                zgemm_macro_kernel(mc, nc, kc, alpha, _beta, _A, _B,
                                   &C[i*MC*rs_c+j*NC*cs_c],
                                   rs_c, cs_c);
                aligned_free( _A );
            }
            aligned_free( _B );
        }
    }
}

