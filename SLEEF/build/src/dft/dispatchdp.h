#define MAXBUTWIDTH 4

#define ISAMAX 5
#define CONFIGMAX 4
void dft2f_0_purecdp(real *, const real *, const int);
void dft2b_0_purecdp(real *, const real *, const int);
void tbut2f_0_purecdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_0_purecdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_0_purecdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_0_purecdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_0_purecdp(real *, const real *, const int);
void dft4b_0_purecdp(real *, const real *, const int);
void tbut4f_0_purecdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_0_purecdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_0_purecdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_0_purecdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_0_purecdp(real *, const real *, const int);
void dft8b_0_purecdp(real *, const real *, const int);
void tbut8f_0_purecdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_0_purecdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_0_purecdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_0_purecdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_0_purecdp(real *, const real *, const int);
void dft16b_0_purecdp(real *, const real *, const int);
void tbut16f_0_purecdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_0_purecdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_0_purecdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_0_purecdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft2f_2_purecdp(real *, const real *, const int);
void dft2b_2_purecdp(real *, const real *, const int);
void tbut2f_2_purecdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_2_purecdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_2_purecdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_2_purecdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_2_purecdp(real *, const real *, const int);
void dft4b_2_purecdp(real *, const real *, const int);
void tbut4f_2_purecdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_2_purecdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_2_purecdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_2_purecdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_2_purecdp(real *, const real *, const int);
void dft8b_2_purecdp(real *, const real *, const int);
void tbut8f_2_purecdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_2_purecdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_2_purecdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_2_purecdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_2_purecdp(real *, const real *, const int);
void dft16b_2_purecdp(real *, const real *, const int);
void tbut16f_2_purecdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_2_purecdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_2_purecdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_2_purecdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void realSub0_purecdp(real *, const real *, const int, const real *, const real *);
void realSub1_purecdp(real *, const real *, const int, const real *, const real *, const int);
int getInt_purecdp(int);
const void *getPtr_purecdp(int);
void dft2f_0_sse2dp(real *, const real *, const int);
void dft2b_0_sse2dp(real *, const real *, const int);
void tbut2f_0_sse2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_0_sse2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_0_sse2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_0_sse2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_0_sse2dp(real *, const real *, const int);
void dft4b_0_sse2dp(real *, const real *, const int);
void tbut4f_0_sse2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_0_sse2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_0_sse2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_0_sse2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_0_sse2dp(real *, const real *, const int);
void dft8b_0_sse2dp(real *, const real *, const int);
void tbut8f_0_sse2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_0_sse2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_0_sse2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_0_sse2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_0_sse2dp(real *, const real *, const int);
void dft16b_0_sse2dp(real *, const real *, const int);
void tbut16f_0_sse2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_0_sse2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_0_sse2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_0_sse2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft2f_2_sse2dp(real *, const real *, const int);
void dft2b_2_sse2dp(real *, const real *, const int);
void tbut2f_2_sse2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_2_sse2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_2_sse2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_2_sse2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_2_sse2dp(real *, const real *, const int);
void dft4b_2_sse2dp(real *, const real *, const int);
void tbut4f_2_sse2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_2_sse2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_2_sse2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_2_sse2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_2_sse2dp(real *, const real *, const int);
void dft8b_2_sse2dp(real *, const real *, const int);
void tbut8f_2_sse2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_2_sse2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_2_sse2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_2_sse2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_2_sse2dp(real *, const real *, const int);
void dft16b_2_sse2dp(real *, const real *, const int);
void tbut16f_2_sse2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_2_sse2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_2_sse2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_2_sse2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void realSub0_sse2dp(real *, const real *, const int, const real *, const real *);
void realSub1_sse2dp(real *, const real *, const int, const real *, const real *, const int);
int getInt_sse2dp(int);
const void *getPtr_sse2dp(int);
void dft2f_0_avxdp(real *, const real *, const int);
void dft2b_0_avxdp(real *, const real *, const int);
void tbut2f_0_avxdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_0_avxdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_0_avxdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_0_avxdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_0_avxdp(real *, const real *, const int);
void dft4b_0_avxdp(real *, const real *, const int);
void tbut4f_0_avxdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_0_avxdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_0_avxdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_0_avxdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_0_avxdp(real *, const real *, const int);
void dft8b_0_avxdp(real *, const real *, const int);
void tbut8f_0_avxdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_0_avxdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_0_avxdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_0_avxdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_0_avxdp(real *, const real *, const int);
void dft16b_0_avxdp(real *, const real *, const int);
void tbut16f_0_avxdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_0_avxdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_0_avxdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_0_avxdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft2f_2_avxdp(real *, const real *, const int);
void dft2b_2_avxdp(real *, const real *, const int);
void tbut2f_2_avxdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_2_avxdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_2_avxdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_2_avxdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_2_avxdp(real *, const real *, const int);
void dft4b_2_avxdp(real *, const real *, const int);
void tbut4f_2_avxdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_2_avxdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_2_avxdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_2_avxdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_2_avxdp(real *, const real *, const int);
void dft8b_2_avxdp(real *, const real *, const int);
void tbut8f_2_avxdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_2_avxdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_2_avxdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_2_avxdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_2_avxdp(real *, const real *, const int);
void dft16b_2_avxdp(real *, const real *, const int);
void tbut16f_2_avxdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_2_avxdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_2_avxdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_2_avxdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void realSub0_avxdp(real *, const real *, const int, const real *, const real *);
void realSub1_avxdp(real *, const real *, const int, const real *, const real *, const int);
int getInt_avxdp(int);
const void *getPtr_avxdp(int);
void dft2f_0_avx2dp(real *, const real *, const int);
void dft2b_0_avx2dp(real *, const real *, const int);
void tbut2f_0_avx2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_0_avx2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_0_avx2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_0_avx2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_0_avx2dp(real *, const real *, const int);
void dft4b_0_avx2dp(real *, const real *, const int);
void tbut4f_0_avx2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_0_avx2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_0_avx2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_0_avx2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_0_avx2dp(real *, const real *, const int);
void dft8b_0_avx2dp(real *, const real *, const int);
void tbut8f_0_avx2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_0_avx2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_0_avx2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_0_avx2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_0_avx2dp(real *, const real *, const int);
void dft16b_0_avx2dp(real *, const real *, const int);
void tbut16f_0_avx2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_0_avx2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_0_avx2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_0_avx2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft2f_2_avx2dp(real *, const real *, const int);
void dft2b_2_avx2dp(real *, const real *, const int);
void tbut2f_2_avx2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_2_avx2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_2_avx2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_2_avx2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_2_avx2dp(real *, const real *, const int);
void dft4b_2_avx2dp(real *, const real *, const int);
void tbut4f_2_avx2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_2_avx2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_2_avx2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_2_avx2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_2_avx2dp(real *, const real *, const int);
void dft8b_2_avx2dp(real *, const real *, const int);
void tbut8f_2_avx2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_2_avx2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_2_avx2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_2_avx2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_2_avx2dp(real *, const real *, const int);
void dft16b_2_avx2dp(real *, const real *, const int);
void tbut16f_2_avx2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_2_avx2dp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_2_avx2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_2_avx2dp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void realSub0_avx2dp(real *, const real *, const int, const real *, const real *);
void realSub1_avx2dp(real *, const real *, const int, const real *, const real *, const int);
int getInt_avx2dp(int);
const void *getPtr_avx2dp(int);
void dft2f_0_avx512fdp(real *, const real *, const int);
void dft2b_0_avx512fdp(real *, const real *, const int);
void tbut2f_0_avx512fdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_0_avx512fdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_0_avx512fdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_0_avx512fdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_0_avx512fdp(real *, const real *, const int);
void dft4b_0_avx512fdp(real *, const real *, const int);
void tbut4f_0_avx512fdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_0_avx512fdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_0_avx512fdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_0_avx512fdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_0_avx512fdp(real *, const real *, const int);
void dft8b_0_avx512fdp(real *, const real *, const int);
void tbut8f_0_avx512fdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_0_avx512fdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_0_avx512fdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_0_avx512fdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_0_avx512fdp(real *, const real *, const int);
void dft16b_0_avx512fdp(real *, const real *, const int);
void tbut16f_0_avx512fdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_0_avx512fdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_0_avx512fdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_0_avx512fdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft2f_2_avx512fdp(real *, const real *, const int);
void dft2b_2_avx512fdp(real *, const real *, const int);
void tbut2f_2_avx512fdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut2b_2_avx512fdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but2f_2_avx512fdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but2b_2_avx512fdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft4f_2_avx512fdp(real *, const real *, const int);
void dft4b_2_avx512fdp(real *, const real *, const int);
void tbut4f_2_avx512fdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut4b_2_avx512fdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but4f_2_avx512fdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but4b_2_avx512fdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft8f_2_avx512fdp(real *, const real *, const int);
void dft8b_2_avx512fdp(real *, const real *, const int);
void tbut8f_2_avx512fdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut8b_2_avx512fdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but8f_2_avx512fdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but8b_2_avx512fdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void dft16f_2_avx512fdp(real *, const real *, const int);
void dft16b_2_avx512fdp(real *, const real *, const int);
void tbut16f_2_avx512fdp(real *, uint32_t *, const real *, const int, const real *, const int);
void tbut16b_2_avx512fdp(real *, uint32_t *, const real *, const int, const real *, const int);
void but16f_2_avx512fdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void but16b_2_avx512fdp(real *, uint32_t *, const int, const real *, const int, const real *, const int);
void realSub0_avx512fdp(real *, const real *, const int, const real *, const real *);
void realSub1_avx512fdp(real *, const real *, const int, const real *, const real *, const int);
int getInt_avx512fdp(int);
const void *getPtr_avx512fdp(int);

void (*dftf_double[CONFIGMAX][ISAMAX][MAXBUTWIDTH+1])(real *, const real *, const int) = {
  {
    {NULL, dft2f_0_purecdp, dft4f_0_purecdp, dft8f_0_purecdp, dft16f_0_purecdp, },
    {NULL, dft2f_0_sse2dp, dft4f_0_sse2dp, dft8f_0_sse2dp, dft16f_0_sse2dp, },
    {NULL, dft2f_0_avxdp, dft4f_0_avxdp, dft8f_0_avxdp, dft16f_0_avxdp, },
    {NULL, dft2f_0_avx2dp, dft4f_0_avx2dp, dft8f_0_avx2dp, dft16f_0_avx2dp, },
    {NULL, dft2f_0_avx512fdp, dft4f_0_avx512fdp, dft8f_0_avx512fdp, dft16f_0_avx512fdp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, dft2f_2_purecdp, dft4f_2_purecdp, dft8f_2_purecdp, dft16f_2_purecdp, },
    {NULL, dft2f_2_sse2dp, dft4f_2_sse2dp, dft8f_2_sse2dp, dft16f_2_sse2dp, },
    {NULL, dft2f_2_avxdp, dft4f_2_avxdp, dft8f_2_avxdp, dft16f_2_avxdp, },
    {NULL, dft2f_2_avx2dp, dft4f_2_avx2dp, dft8f_2_avx2dp, dft16f_2_avx2dp, },
    {NULL, dft2f_2_avx512fdp, dft4f_2_avx512fdp, dft8f_2_avx512fdp, dft16f_2_avx512fdp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*dftb_double[CONFIGMAX][ISAMAX][MAXBUTWIDTH+1])(real *, const real *, const int) = {
  {
    {NULL, dft2f_0_purecdp, dft4b_0_purecdp, dft8b_0_purecdp, dft16b_0_purecdp, },
    {NULL, dft2f_0_sse2dp, dft4b_0_sse2dp, dft8b_0_sse2dp, dft16b_0_sse2dp, },
    {NULL, dft2f_0_avxdp, dft4b_0_avxdp, dft8b_0_avxdp, dft16b_0_avxdp, },
    {NULL, dft2f_0_avx2dp, dft4b_0_avx2dp, dft8b_0_avx2dp, dft16b_0_avx2dp, },
    {NULL, dft2f_0_avx512fdp, dft4b_0_avx512fdp, dft8b_0_avx512fdp, dft16b_0_avx512fdp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, dft2f_2_purecdp, dft4b_2_purecdp, dft8b_2_purecdp, dft16b_2_purecdp, },
    {NULL, dft2f_2_sse2dp, dft4b_2_sse2dp, dft8b_2_sse2dp, dft16b_2_sse2dp, },
    {NULL, dft2f_2_avxdp, dft4b_2_avxdp, dft8b_2_avxdp, dft16b_2_avxdp, },
    {NULL, dft2f_2_avx2dp, dft4b_2_avx2dp, dft8b_2_avx2dp, dft16b_2_avx2dp, },
    {NULL, dft2f_2_avx512fdp, dft4b_2_avx512fdp, dft8b_2_avx512fdp, dft16b_2_avx512fdp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*tbutf_double[CONFIGMAX][ISAMAX][MAXBUTWIDTH+1])(real *, uint32_t *, const real *, const int, const real *, const int) = {
  {
    {NULL, tbut2f_0_purecdp, tbut4f_0_purecdp, tbut8f_0_purecdp, tbut16f_0_purecdp, },
    {NULL, tbut2f_0_sse2dp, tbut4f_0_sse2dp, tbut8f_0_sse2dp, tbut16f_0_sse2dp, },
    {NULL, tbut2f_0_avxdp, tbut4f_0_avxdp, tbut8f_0_avxdp, tbut16f_0_avxdp, },
    {NULL, tbut2f_0_avx2dp, tbut4f_0_avx2dp, tbut8f_0_avx2dp, tbut16f_0_avx2dp, },
    {NULL, tbut2f_0_avx512fdp, tbut4f_0_avx512fdp, tbut8f_0_avx512fdp, tbut16f_0_avx512fdp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, tbut2f_2_purecdp, tbut4f_2_purecdp, tbut8f_2_purecdp, tbut16f_2_purecdp, },
    {NULL, tbut2f_2_sse2dp, tbut4f_2_sse2dp, tbut8f_2_sse2dp, tbut16f_2_sse2dp, },
    {NULL, tbut2f_2_avxdp, tbut4f_2_avxdp, tbut8f_2_avxdp, tbut16f_2_avxdp, },
    {NULL, tbut2f_2_avx2dp, tbut4f_2_avx2dp, tbut8f_2_avx2dp, tbut16f_2_avx2dp, },
    {NULL, tbut2f_2_avx512fdp, tbut4f_2_avx512fdp, tbut8f_2_avx512fdp, tbut16f_2_avx512fdp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*tbutb_double[CONFIGMAX][ISAMAX][MAXBUTWIDTH+1])(real *, uint32_t *, const real *, const int, const real *, const int) = {
  {
    {NULL, tbut2b_0_purecdp, tbut4b_0_purecdp, tbut8b_0_purecdp, tbut16b_0_purecdp, },
    {NULL, tbut2b_0_sse2dp, tbut4b_0_sse2dp, tbut8b_0_sse2dp, tbut16b_0_sse2dp, },
    {NULL, tbut2b_0_avxdp, tbut4b_0_avxdp, tbut8b_0_avxdp, tbut16b_0_avxdp, },
    {NULL, tbut2b_0_avx2dp, tbut4b_0_avx2dp, tbut8b_0_avx2dp, tbut16b_0_avx2dp, },
    {NULL, tbut2b_0_avx512fdp, tbut4b_0_avx512fdp, tbut8b_0_avx512fdp, tbut16b_0_avx512fdp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, tbut2b_2_purecdp, tbut4b_2_purecdp, tbut8b_2_purecdp, tbut16b_2_purecdp, },
    {NULL, tbut2b_2_sse2dp, tbut4b_2_sse2dp, tbut8b_2_sse2dp, tbut16b_2_sse2dp, },
    {NULL, tbut2b_2_avxdp, tbut4b_2_avxdp, tbut8b_2_avxdp, tbut16b_2_avxdp, },
    {NULL, tbut2b_2_avx2dp, tbut4b_2_avx2dp, tbut8b_2_avx2dp, tbut16b_2_avx2dp, },
    {NULL, tbut2b_2_avx512fdp, tbut4b_2_avx512fdp, tbut8b_2_avx512fdp, tbut16b_2_avx512fdp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*butf_double[CONFIGMAX][ISAMAX][MAXBUTWIDTH+1])(real *, uint32_t *, const int, const real *, const int, const real *, const int) = {
  {
    {NULL, but2f_0_purecdp, but4f_0_purecdp, but8f_0_purecdp, but16f_0_purecdp, },
    {NULL, but2f_0_sse2dp, but4f_0_sse2dp, but8f_0_sse2dp, but16f_0_sse2dp, },
    {NULL, but2f_0_avxdp, but4f_0_avxdp, but8f_0_avxdp, but16f_0_avxdp, },
    {NULL, but2f_0_avx2dp, but4f_0_avx2dp, but8f_0_avx2dp, but16f_0_avx2dp, },
    {NULL, but2f_0_avx512fdp, but4f_0_avx512fdp, but8f_0_avx512fdp, but16f_0_avx512fdp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, but2f_2_purecdp, but4f_2_purecdp, but8f_2_purecdp, but16f_2_purecdp, },
    {NULL, but2f_2_sse2dp, but4f_2_sse2dp, but8f_2_sse2dp, but16f_2_sse2dp, },
    {NULL, but2f_2_avxdp, but4f_2_avxdp, but8f_2_avxdp, but16f_2_avxdp, },
    {NULL, but2f_2_avx2dp, but4f_2_avx2dp, but8f_2_avx2dp, but16f_2_avx2dp, },
    {NULL, but2f_2_avx512fdp, but4f_2_avx512fdp, but8f_2_avx512fdp, but16f_2_avx512fdp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*butb_double[CONFIGMAX][ISAMAX][MAXBUTWIDTH+1])(real *, uint32_t *, const int, const real *, const int, const real *, const int) = {
  {
    {NULL, but2b_0_purecdp, but4b_0_purecdp, but8b_0_purecdp, but16b_0_purecdp, },
    {NULL, but2b_0_sse2dp, but4b_0_sse2dp, but8b_0_sse2dp, but16b_0_sse2dp, },
    {NULL, but2b_0_avxdp, but4b_0_avxdp, but8b_0_avxdp, but16b_0_avxdp, },
    {NULL, but2b_0_avx2dp, but4b_0_avx2dp, but8b_0_avx2dp, but16b_0_avx2dp, },
    {NULL, but2b_0_avx512fdp, but4b_0_avx512fdp, but8b_0_avx512fdp, but16b_0_avx512fdp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
  {
    {NULL, but2b_2_purecdp, but4b_2_purecdp, but8b_2_purecdp, but16b_2_purecdp, },
    {NULL, but2b_2_sse2dp, but4b_2_sse2dp, but8b_2_sse2dp, but16b_2_sse2dp, },
    {NULL, but2b_2_avxdp, but4b_2_avxdp, but8b_2_avxdp, but16b_2_avxdp, },
    {NULL, but2b_2_avx2dp, but4b_2_avx2dp, but8b_2_avx2dp, but16b_2_avx2dp, },
    {NULL, but2b_2_avx512fdp, but4b_2_avx512fdp, but8b_2_avx512fdp, but16b_2_avx512fdp, },
},
  {
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
    {NULL, NULL, NULL, NULL, NULL, },
},
};

void (*realSub0_double[ISAMAX])(real *, const real *, const int, const real *, const real *) = {
  realSub0_purecdp, realSub0_sse2dp, realSub0_avxdp, realSub0_avx2dp, realSub0_avx512fdp, 
};

void (*realSub1_double[ISAMAX])(real *, const real *, const int, const real *, const real *, const int) = {
  realSub1_purecdp, realSub1_sse2dp, realSub1_avxdp, realSub1_avx2dp, realSub1_avx512fdp, 
};

int (*getInt_double[16])(int) = {
  getInt_purecdp, getInt_sse2dp, getInt_avxdp, getInt_avx2dp, getInt_avx512fdp, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
};

const void *(*getPtr_double[16])(int) = {
  getPtr_purecdp, getPtr_sse2dp, getPtr_avxdp, getPtr_avx2dp, getPtr_avx512fdp, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
};

