#ifndef _DEFINED_SCOMPLEX
#define _DEFINED_SCOMPLEX
typedef struct
{
	float  real;
	float  imag;
} scomplex;
#endif
  
#ifndef _DEFINED_DCOMPLEX
#define _DEFINED_DCOMPLEX
typedef struct
{
	double real;
	double imag;
} dcomplex;
#endif
  
extern void
dgemm_nn(I              m,
         I              n,
         I              k,
         double         alpha,
         double         *A,
         I              rs_A,
         I              cs_A,
         double         *B,
         I              rs_B,
         I              cs_B,
         double         beta,
         double         *C,
         I              rs_C,
         I              cs_C);

extern void
igemm_nn(I              m,
         I              n,
         I              k,
         I              alpha,
         I              *A,
         I              rs_A,
         I              cs_A,
         I              *B,
         I              rs_B,
         I              cs_B,
         I              beta,
         double         *C,
         I              rs_C,
         I              cs_C);

extern void
zgemm_nn(I              m,
         I              n,
         I              k,
         dcomplex       alpha,
         dcomplex       *A,
         I              rs_A,
         I              cs_A,
         dcomplex       *B,
         I              rs_B,
         I              cs_B,
         dcomplex       beta,
         dcomplex       *C,
         I              rs_C,
         I              cs_C);
