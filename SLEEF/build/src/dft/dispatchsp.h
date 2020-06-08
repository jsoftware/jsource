#define MAXBUTWIDTH 4

#define ISAMAX 5
#define CONFIGMAX 4
void dft2f_0_purecsp(real *, const real *, const int);
void dft2b_0_purecsp(real *, const real *, const int);
void tbut2f_0_purecsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_0_purecsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_0_purecsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_0_purecsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_0_purecsp(real *, const real *, const int);
void dft4b_0_purecsp(real *, const real *, const int);
void tbut4f_0_purecsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_0_purecsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_0_purecsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_0_purecsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_0_purecsp(real *, const real *, const int);
void dft8b_0_purecsp(real *, const real *, const int);
void tbut8f_0_purecsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_0_purecsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_0_purecsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_0_purecsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_0_purecsp(real *, const real *, const int);
void dft16b_0_purecsp(real *, const real *, const int);
void tbut16f_0_purecsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_0_purecsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_0_purecsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_0_purecsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft2f_2_purecsp(real *, const real *, const int);
void dft2b_2_purecsp(real *, const real *, const int);
void tbut2f_2_purecsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_2_purecsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_2_purecsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_2_purecsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_2_purecsp(real *, const real *, const int);
void dft4b_2_purecsp(real *, const real *, const int);
void tbut4f_2_purecsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_2_purecsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_2_purecsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_2_purecsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_2_purecsp(real *, const real *, const int);
void dft8b_2_purecsp(real *, const real *, const int);
void tbut8f_2_purecsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_2_purecsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_2_purecsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_2_purecsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_2_purecsp(real *, const real *, const int);
void dft16b_2_purecsp(real *, const real *, const int);
void tbut16f_2_purecsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_2_purecsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_2_purecsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_2_purecsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void realSub0_purecsp(real *, const real *, const int, const real *, const real *);
void realSub1_purecsp(real *, const real *, const int, const real *, const real *, const int);
int getInt_purecsp(int);
const void *getPtr_purecsp(int);
void dft2f_0_sse2sp(real *, const real *, const int);
void dft2b_0_sse2sp(real *, const real *, const int);
void tbut2f_0_sse2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_0_sse2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_0_sse2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_0_sse2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_0_sse2sp(real *, const real *, const int);
void dft4b_0_sse2sp(real *, const real *, const int);
void tbut4f_0_sse2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_0_sse2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_0_sse2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_0_sse2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_0_sse2sp(real *, const real *, const int);
void dft8b_0_sse2sp(real *, const real *, const int);
void tbut8f_0_sse2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_0_sse2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_0_sse2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_0_sse2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_0_sse2sp(real *, const real *, const int);
void dft16b_0_sse2sp(real *, const real *, const int);
void tbut16f_0_sse2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_0_sse2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_0_sse2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_0_sse2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft2f_2_sse2sp(real *, const real *, const int);
void dft2b_2_sse2sp(real *, const real *, const int);
void tbut2f_2_sse2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_2_sse2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_2_sse2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_2_sse2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_2_sse2sp(real *, const real *, const int);
void dft4b_2_sse2sp(real *, const real *, const int);
void tbut4f_2_sse2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_2_sse2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_2_sse2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_2_sse2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_2_sse2sp(real *, const real *, const int);
void dft8b_2_sse2sp(real *, const real *, const int);
void tbut8f_2_sse2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_2_sse2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_2_sse2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_2_sse2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_2_sse2sp(real *, const real *, const int);
void dft16b_2_sse2sp(real *, const real *, const int);
void tbut16f_2_sse2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_2_sse2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_2_sse2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_2_sse2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void realSub0_sse2sp(real *, const real *, const int, const real *, const real *);
void realSub1_sse2sp(real *, const real *, const int, const real *, const real *, const int);
int getInt_sse2sp(int);
const void *getPtr_sse2sp(int);
void dft2f_0_avxsp(real *, const real *, const int);
void dft2b_0_avxsp(real *, const real *, const int);
void tbut2f_0_avxsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_0_avxsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_0_avxsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_0_avxsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_0_avxsp(real *, const real *, const int);
void dft4b_0_avxsp(real *, const real *, const int);
void tbut4f_0_avxsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_0_avxsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_0_avxsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_0_avxsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_0_avxsp(real *, const real *, const int);
void dft8b_0_avxsp(real *, const real *, const int);
void tbut8f_0_avxsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_0_avxsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_0_avxsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_0_avxsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_0_avxsp(real *, const real *, const int);
void dft16b_0_avxsp(real *, const real *, const int);
void tbut16f_0_avxsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_0_avxsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_0_avxsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_0_avxsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft2f_2_avxsp(real *, const real *, const int);
void dft2b_2_avxsp(real *, const real *, const int);
void tbut2f_2_avxsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_2_avxsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_2_avxsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_2_avxsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_2_avxsp(real *, const real *, const int);
void dft4b_2_avxsp(real *, const real *, const int);
void tbut4f_2_avxsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_2_avxsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_2_avxsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_2_avxsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_2_avxsp(real *, const real *, const int);
void dft8b_2_avxsp(real *, const real *, const int);
void tbut8f_2_avxsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_2_avxsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_2_avxsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_2_avxsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_2_avxsp(real *, const real *, const int);
void dft16b_2_avxsp(real *, const real *, const int);
void tbut16f_2_avxsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_2_avxsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_2_avxsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_2_avxsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void realSub0_avxsp(real *, const real *, const int, const real *, const real *);
void realSub1_avxsp(real *, const real *, const int, const real *, const real *, const int);
int getInt_avxsp(int);
const void *getPtr_avxsp(int);
void dft2f_0_avx2sp(real *, const real *, const int);
void dft2b_0_avx2sp(real *, const real *, const int);
void tbut2f_0_avx2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_0_avx2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_0_avx2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_0_avx2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_0_avx2sp(real *, const real *, const int);
void dft4b_0_avx2sp(real *, const real *, const int);
void tbut4f_0_avx2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_0_avx2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_0_avx2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_0_avx2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_0_avx2sp(real *, const real *, const int);
void dft8b_0_avx2sp(real *, const real *, const int);
void tbut8f_0_avx2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_0_avx2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_0_avx2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_0_avx2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_0_avx2sp(real *, const real *, const int);
void dft16b_0_avx2sp(real *, const real *, const int);
void tbut16f_0_avx2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_0_avx2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_0_avx2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_0_avx2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft2f_2_avx2sp(real *, const real *, const int);
void dft2b_2_avx2sp(real *, const real *, const int);
void tbut2f_2_avx2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_2_avx2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_2_avx2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_2_avx2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_2_avx2sp(real *, const real *, const int);
void dft4b_2_avx2sp(real *, const real *, const int);
void tbut4f_2_avx2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_2_avx2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_2_avx2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_2_avx2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_2_avx2sp(real *, const real *, const int);
void dft8b_2_avx2sp(real *, const real *, const int);
void tbut8f_2_avx2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_2_avx2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_2_avx2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_2_avx2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_2_avx2sp(real *, const real *, const int);
void dft16b_2_avx2sp(real *, const real *, const int);
void tbut16f_2_avx2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_2_avx2sp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_2_avx2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_2_avx2sp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void realSub0_avx2sp(real *, const real *, const int, const real *, const real *);
void realSub1_avx2sp(real *, const real *, const int, const real *, const real *, const int);
int getInt_avx2sp(int);
const void *getPtr_avx2sp(int);
void dft2f_0_avx512fsp(real *, const real *, const int);
void dft2b_0_avx512fsp(real *, const real *, const int);
void tbut2f_0_avx512fsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_0_avx512fsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_0_avx512fsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_0_avx512fsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_0_avx512fsp(real *, const real *, const int);
void dft4b_0_avx512fsp(real *, const real *, const int);
void tbut4f_0_avx512fsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_0_avx512fsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_0_avx512fsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_0_avx512fsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_0_avx512fsp(real *, const real *, const int);
void dft8b_0_avx512fsp(real *, const real *, const int);
void tbut8f_0_avx512fsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_0_avx512fsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_0_avx512fsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_0_avx512fsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_0_avx512fsp(real *, const real *, const int);
void dft16b_0_avx512fsp(real *, const real *, const int);
void tbut16f_0_avx512fsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_0_avx512fsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_0_avx512fsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_0_avx512fsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft2f_2_avx512fsp(real *, const real *, const int);
void dft2b_2_avx512fsp(real *, const real *, const int);
void tbut2f_2_avx512fsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_2_avx512fsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_2_avx512fsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_2_avx512fsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_2_avx512fsp(real *, const real *, const int);
void dft4b_2_avx512fsp(real *, const real *, const int);
void tbut4f_2_avx512fsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_2_avx512fsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_2_avx512fsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_2_avx512fsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_2_avx512fsp(real *, const real *, const int);
void dft8b_2_avx512fsp(real *, const real *, const int);
void tbut8f_2_avx512fsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_2_avx512fsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_2_avx512fsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_2_avx512fsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_2_avx512fsp(real *, const real *, const int);
void dft16b_2_avx512fsp(real *, const real *, const int);
void tbut16f_2_avx512fsp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_2_avx512fsp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_2_avx512fsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_2_avx512fsp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void realSub0_avx512fsp(real *, const real *, const int, const real *, const real *);
void realSub1_avx512fsp(real *, const real *, const int, const real *, const real *, const int);
int getInt_avx512fsp(int);
const void *getPtr_avx512fsp(int);

void (*dftf_float[CONFIGMAX][ISAMAX][MAXBUTWIDTH+1])(real *, const real *, const int) = {
  {
    {NULL, dft2f_0_purecsp, dft4f_0_purecsp, dft8f_0_purecsp, dft16f_0_purecsp, },
    {NULL, dft2f_0_sse2sp, dft4f_0_sse2sp, dft8f_0_sse2sp, dft16f_0_sse2sp, },
    {NULL, dft2f_0_avxsp, dft4f_0_avxsp, dft8f_0_avxsp, dft16f_0_avxsp, },
    {NULL, dft2f_0_avx2sp, dft4f_0_avx2sp, dft8f_0_avx2sp, dft16f_0_avx2sp, },
    {NULL, dft2f_0_avx512fsp, dft4f_0_avx512fsp, dft8f_0_avx512fsp, dft16f_0_avx512fsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, dft2f_2_purecsp, dft4f_2_purecsp, dft8f_2_purecsp, dft16f_2_purecsp, },
    {NULL, dft2f_2_sse2sp, dft4f_2_sse2sp, dft8f_2_sse2sp, dft16f_2_sse2sp, },
    {NULL, dft2f_2_avxsp, dft4f_2_avxsp, dft8f_2_avxsp, dft16f_2_avxsp, },
    {NULL, dft2f_2_avx2sp, dft4f_2_avx2sp, dft8f_2_avx2sp, dft16f_2_avx2sp, },
    {NULL, dft2f_2_avx512fsp, dft4f_2_avx512fsp, dft8f_2_avx512fsp, dft16f_2_avx512fsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*dftb_float[CONFIGMAX][ISAMAX][MAXBUTWIDTH+1])(real *, const real *, const int) = {
  {
    {NULL, dft2f_0_purecsp, dft4b_0_purecsp, dft8b_0_purecsp, dft16b_0_purecsp, },
    {NULL, dft2f_0_sse2sp, dft4b_0_sse2sp, dft8b_0_sse2sp, dft16b_0_sse2sp, },
    {NULL, dft2f_0_avxsp, dft4b_0_avxsp, dft8b_0_avxsp, dft16b_0_avxsp, },
    {NULL, dft2f_0_avx2sp, dft4b_0_avx2sp, dft8b_0_avx2sp, dft16b_0_avx2sp, },
    {NULL, dft2f_0_avx512fsp, dft4b_0_avx512fsp, dft8b_0_avx512fsp, dft16b_0_avx512fsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, dft2f_2_purecsp, dft4b_2_purecsp, dft8b_2_purecsp, dft16b_2_purecsp, },
    {NULL, dft2f_2_sse2sp, dft4b_2_sse2sp, dft8b_2_sse2sp, dft16b_2_sse2sp, },
    {NULL, dft2f_2_avxsp, dft4b_2_avxsp, dft8b_2_avxsp, dft16b_2_avxsp, },
    {NULL, dft2f_2_avx2sp, dft4b_2_avx2sp, dft8b_2_avx2sp, dft16b_2_avx2sp, },
    {NULL, dft2f_2_avx512fsp, dft4b_2_avx512fsp, dft8b_2_avx512fsp, dft16b_2_avx512fsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*tbutf_float[CONFIGMAX][ISAMAX][MAXBUTWIDTH+1])(real *, uint32_t *, const real *, const int, const real *, const int) = {
  {
    {NULL, tbut2f_0_purecsp, tbut4f_0_purecsp, tbut8f_0_purecsp, tbut16f_0_purecsp, },
    {NULL, tbut2f_0_sse2sp, tbut4f_0_sse2sp, tbut8f_0_sse2sp, tbut16f_0_sse2sp, },
    {NULL, tbut2f_0_avxsp, tbut4f_0_avxsp, tbut8f_0_avxsp, tbut16f_0_avxsp, },
    {NULL, tbut2f_0_avx2sp, tbut4f_0_avx2sp, tbut8f_0_avx2sp, tbut16f_0_avx2sp, },
    {NULL, tbut2f_0_avx512fsp, tbut4f_0_avx512fsp, tbut8f_0_avx512fsp, tbut16f_0_avx512fsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, tbut2f_2_purecsp, tbut4f_2_purecsp, tbut8f_2_purecsp, tbut16f_2_purecsp, },
    {NULL, tbut2f_2_sse2sp, tbut4f_2_sse2sp, tbut8f_2_sse2sp, tbut16f_2_sse2sp, },
    {NULL, tbut2f_2_avxsp, tbut4f_2_avxsp, tbut8f_2_avxsp, tbut16f_2_avxsp, },
    {NULL, tbut2f_2_avx2sp, tbut4f_2_avx2sp, tbut8f_2_avx2sp, tbut16f_2_avx2sp, },
    {NULL, tbut2f_2_avx512fsp, tbut4f_2_avx512fsp, tbut8f_2_avx512fsp, tbut16f_2_avx512fsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*tbutb_float[CONFIGMAX][ISAMAX][MAXBUTWIDTH+1])(real *, uint32_t *, const real *, const int, const real *, const int) = {
  {
    {NULL, tbut2b_0_purecsp, tbut4b_0_purecsp, tbut8b_0_purecsp, tbut16b_0_purecsp, },
    {NULL, tbut2b_0_sse2sp, tbut4b_0_sse2sp, tbut8b_0_sse2sp, tbut16b_0_sse2sp, },
    {NULL, tbut2b_0_avxsp, tbut4b_0_avxsp, tbut8b_0_avxsp, tbut16b_0_avxsp, },
    {NULL, tbut2b_0_avx2sp, tbut4b_0_avx2sp, tbut8b_0_avx2sp, tbut16b_0_avx2sp, },
    {NULL, tbut2b_0_avx512fsp, tbut4b_0_avx512fsp, tbut8b_0_avx512fsp, tbut16b_0_avx512fsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, tbut2b_2_purecsp, tbut4b_2_purecsp, tbut8b_2_purecsp, tbut16b_2_purecsp, },
    {NULL, tbut2b_2_sse2sp, tbut4b_2_sse2sp, tbut8b_2_sse2sp, tbut16b_2_sse2sp, },
    {NULL, tbut2b_2_avxsp, tbut4b_2_avxsp, tbut8b_2_avxsp, tbut16b_2_avxsp, },
    {NULL, tbut2b_2_avx2sp, tbut4b_2_avx2sp, tbut8b_2_avx2sp, tbut16b_2_avx2sp, },
    {NULL, tbut2b_2_avx512fsp, tbut4b_2_avx512fsp, tbut8b_2_avx512fsp, tbut16b_2_avx512fsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*butf_float[CONFIGMAX][ISAMAX][MAXBUTWIDTH+1])(real *, uint32_t *, const int, const real *, const int, const real *, const int) = {
  {
    {NULL, but2f_0_purecsp, but4f_0_purecsp, but8f_0_purecsp, but16f_0_purecsp, },
    {NULL, but2f_0_sse2sp, but4f_0_sse2sp, but8f_0_sse2sp, but16f_0_sse2sp, },
    {NULL, but2f_0_avxsp, but4f_0_avxsp, but8f_0_avxsp, but16f_0_avxsp, },
    {NULL, but2f_0_avx2sp, but4f_0_avx2sp, but8f_0_avx2sp, but16f_0_avx2sp, },
    {NULL, but2f_0_avx512fsp, but4f_0_avx512fsp, but8f_0_avx512fsp, but16f_0_avx512fsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, but2f_2_purecsp, but4f_2_purecsp, but8f_2_purecsp, but16f_2_purecsp, },
    {NULL, but2f_2_sse2sp, but4f_2_sse2sp, but8f_2_sse2sp, but16f_2_sse2sp, },
    {NULL, but2f_2_avxsp, but4f_2_avxsp, but8f_2_avxsp, but16f_2_avxsp, },
    {NULL, but2f_2_avx2sp, but4f_2_avx2sp, but8f_2_avx2sp, but16f_2_avx2sp, },
    {NULL, but2f_2_avx512fsp, but4f_2_avx512fsp, but8f_2_avx512fsp, but16f_2_avx512fsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*butb_float[CONFIGMAX][ISAMAX][MAXBUTWIDTH+1])(real *, uint32_t *, const int, const real *, const int, const real *, const int) = {
  {
    {NULL, but2b_0_purecsp, but4b_0_purecsp, but8b_0_purecsp, but16b_0_purecsp, },
    {NULL, but2b_0_sse2sp, but4b_0_sse2sp, but8b_0_sse2sp, but16b_0_sse2sp, },
    {NULL, but2b_0_avxsp, but4b_0_avxsp, but8b_0_avxsp, but16b_0_avxsp, },
    {NULL, but2b_0_avx2sp, but4b_0_avx2sp, but8b_0_avx2sp, but16b_0_avx2sp, },
    {NULL, but2b_0_avx512fsp, but4b_0_avx512fsp, but8b_0_avx512fsp, but16b_0_avx512fsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, but2b_2_purecsp, but4b_2_purecsp, but8b_2_purecsp, but16b_2_purecsp, },
    {NULL, but2b_2_sse2sp, but4b_2_sse2sp, but8b_2_sse2sp, but16b_2_sse2sp, },
    {NULL, but2b_2_avxsp, but4b_2_avxsp, but8b_2_avxsp, but16b_2_avxsp, },
    {NULL, but2b_2_avx2sp, but4b_2_avx2sp, but8b_2_avx2sp, but16b_2_avx2sp, },
    {NULL, but2b_2_avx512fsp, but4b_2_avx512fsp, but8b_2_avx512fsp, but16b_2_avx512fsp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*realSub0_float[ISAMAX])(real *, const real *, const int, const real *, const real *) = {
  realSub0_purecsp, realSub0_sse2sp, realSub0_avxsp, realSub0_avx2sp, realSub0_avx512fsp, 
};

void (*realSub1_float[ISAMAX])(real *, const real *, const int, const real *, const real *, const int) = {
  realSub1_purecsp, realSub1_sse2sp, realSub1_avxsp, realSub1_avx2sp, realSub1_avx512fsp, 
};

int (*getInt_float[16])(int) = {
  getInt_purecsp, getInt_sse2sp, getInt_avxsp, getInt_avx2sp, getInt_avx512fsp, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
};

const void *(*getPtr_float[16])(int) = {
  getPtr_purecsp, getPtr_sse2sp, getPtr_avxsp, getPtr_avx2sp, getPtr_avx512fsp, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
};

