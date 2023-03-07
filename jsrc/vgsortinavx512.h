// avx512 bitonic sorting networks
// present implementations are adapted from vxsort; I think it's probably possible to do better, but other optimisations are more important, so not bothering yet

static INLINE void sort_01v_ascendings8(__m512i *d01) {
    __m512i  min, s;

    s = _mm512_castpd_si512(_mm512_permute_pd(_mm512_castsi512_pd(*d01), _MM_PERM_BBBB));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x00AA, s, *d01);

    s = _mm512_castpd_si512(_mm512_permutex_pd(_mm512_castsi512_pd(*d01), _MM_PERM_ABCD));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x00CC, s, *d01);

    s = _mm512_castpd_si512(_mm512_permute_pd(_mm512_castsi512_pd(*d01), _MM_PERM_BBBB));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x00AA, s, *d01);

    s = _mm512_castpd_si512(_mm512_shuffle_f64x2(_mm512_permutex_pd(_mm512_castsi512_pd(*d01), _MM_PERM_ABCD), _mm512_permutex_pd(_mm512_castsi512_pd(*d01), _MM_PERM_ABCD), _MM_PERM_BADC));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x00F0, s, *d01);

    s = _mm512_castpd_si512(_mm512_permutex_pd(_mm512_castsi512_pd(*d01), _MM_PERM_BADC));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x00CC, s, *d01);

    s = _mm512_castpd_si512(_mm512_permute_pd(_mm512_castsi512_pd(*d01), _MM_PERM_BBBB));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x00AA, s, *d01);
}
static INLINE void sort_01v_merge_ascendings8(__m512i *d01) {
    __m512i  min, s;

    s = _mm512_castpd_si512(_mm512_shuffle_f64x2(_mm512_castsi512_pd(*d01), _mm512_castsi512_pd(*d01), _MM_PERM_BADC));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x00F0, s, *d01);

    s = _mm512_castpd_si512(_mm512_permutex_pd(_mm512_castsi512_pd(*d01), _MM_PERM_BADC));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x00CC, s, *d01);

    s = _mm512_castpd_si512(_mm512_permute_pd(_mm512_castsi512_pd(*d01), _MM_PERM_BBBB));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x00AA, s, *d01);
}
static INLINE void sort_01v_descendings8(__m512i *d01) {
    __m512i  min, s;

    s = _mm512_castpd_si512(_mm512_permute_pd(_mm512_castsi512_pd(*d01), _MM_PERM_BBBB));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x0055, s, *d01);

    s = _mm512_castpd_si512(_mm512_permutex_pd(_mm512_castsi512_pd(*d01), _MM_PERM_ABCD));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x0033, s, *d01);

    s = _mm512_castpd_si512(_mm512_permute_pd(_mm512_castsi512_pd(*d01), _MM_PERM_BBBB));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x0055, s, *d01);

    s = _mm512_castpd_si512(_mm512_shuffle_f64x2(_mm512_permutex_pd(_mm512_castsi512_pd(*d01), _MM_PERM_ABCD), _mm512_permutex_pd(_mm512_castsi512_pd(*d01), _MM_PERM_ABCD), _MM_PERM_BADC));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x000F, s, *d01);

    s = _mm512_castpd_si512(_mm512_permutex_pd(_mm512_castsi512_pd(*d01), _MM_PERM_BADC));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x0033, s, *d01);

    s = _mm512_castpd_si512(_mm512_permute_pd(_mm512_castsi512_pd(*d01), _MM_PERM_BBBB));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x0055, s, *d01);
}
static INLINE void sort_01v_merge_descendings8(__m512i *d01) {
    __m512i  min, s;

    s = _mm512_castpd_si512(_mm512_shuffle_f64x2(_mm512_castsi512_pd(*d01), _mm512_castsi512_pd(*d01), _MM_PERM_BADC));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x000F, s, *d01);

    s = _mm512_castpd_si512(_mm512_permutex_pd(_mm512_castsi512_pd(*d01), _MM_PERM_BADC));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x0033, s, *d01);

    s = _mm512_castpd_si512(_mm512_permute_pd(_mm512_castsi512_pd(*d01), _MM_PERM_BBBB));
    min = _mm512_min_epi64(s, *d01);
    *d01 = _mm512_mask_max_epi64(min, 0x0055, s, *d01);
}
static INLINE void sort_02v_ascendings8(__m512i *d01, __m512i *d02) {
    __m512i  tmp;

    sort_01v_ascendings8(d01);
    sort_01v_descendings8(d02);

    tmp = *d02;
    *d02 = _mm512_max_epi64(*d01, *d02);
    *d01 = _mm512_min_epi64(*d01, tmp);

    sort_01v_merge_ascendings8(d01);
    sort_01v_merge_ascendings8(d02);
}
static INLINE void sort_02v_descendings8(__m512i *d01, __m512i *d02) {
    __m512i  tmp;

    sort_01v_descendings8(d01);
    sort_01v_ascendings8(d02);

    tmp = *d02;
    *d02 = _mm512_max_epi64(*d01, *d02);
    *d01 = _mm512_min_epi64(*d01, tmp);

    sort_01v_merge_descendings8(d01);
    sort_01v_merge_descendings8(d02);
}
static INLINE void sort_02v_merge_ascendings8(__m512i *d01, __m512i *d02) {
    __m512i  tmp;

    tmp = *d01;
    *d01 = _mm512_min_epi64(*d02, *d01);
    *d02 = _mm512_max_epi64(*d02, tmp);

    sort_01v_merge_ascendings8(d01);
    sort_01v_merge_ascendings8(d02);
}
static INLINE void sort_02v_merge_descendings8(__m512i *d01, __m512i *d02) {
    __m512i  tmp;

    tmp = *d01;
    *d01 = _mm512_min_epi64(*d02, *d01);
    *d02 = _mm512_max_epi64(*d02, tmp);

    sort_01v_merge_descendings8(d01);
    sort_01v_merge_descendings8(d02);
}
static INLINE void sort_03v_ascendings8(__m512i *d01, __m512i *d02, __m512i *d03) {
    __m512i  tmp;

    sort_02v_ascendings8(d01, d02);
    sort_01v_descendings8(d03);

    tmp = *d03;
    *d03 = _mm512_max_epi64(*d02, *d03);
    *d02 = _mm512_min_epi64(*d02, tmp);

    sort_02v_merge_ascendings8(d01, d02);
    sort_01v_merge_ascendings8(d03);
}
static INLINE void sort_03v_descendings8(__m512i *d01, __m512i *d02, __m512i *d03) {
    __m512i  tmp;

    sort_02v_descendings8(d01, d02);
    sort_01v_ascendings8(d03);

    tmp = *d03;
    *d03 = _mm512_max_epi64(*d02, *d03);
    *d02 = _mm512_min_epi64(*d02, tmp);

    sort_02v_merge_descendings8(d01, d02);
    sort_01v_merge_descendings8(d03);
}
static INLINE void sort_03v_merge_ascendings8(__m512i *d01, __m512i *d02, __m512i *d03) {
    __m512i  tmp;

    tmp = *d01;
    *d01 = _mm512_min_epi64(*d03, *d01);
    *d03 = _mm512_max_epi64(*d03, tmp);

    sort_02v_merge_ascendings8(d01, d02);
    sort_01v_merge_ascendings8(d03);
}
static INLINE void sort_03v_merge_descendings8(__m512i *d01, __m512i *d02, __m512i *d03) {
    __m512i  tmp;

    tmp = *d01;
    *d01 = _mm512_min_epi64(*d03, *d01);
    *d03 = _mm512_max_epi64(*d03, tmp);

    sort_02v_merge_descendings8(d01, d02);
    sort_01v_merge_descendings8(d03);
}
static INLINE void sort_04v_ascendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04) {
    __m512i  tmp;

    sort_02v_ascendings8(d01, d02);
    sort_02v_descendings8(d03, d04);

    tmp = *d03;
    *d03 = _mm512_max_epi64(*d02, *d03);
    *d02 = _mm512_min_epi64(*d02, tmp);

    tmp = *d04;
    *d04 = _mm512_max_epi64(*d01, *d04);
    *d01 = _mm512_min_epi64(*d01, tmp);

    sort_02v_merge_ascendings8(d01, d02);
    sort_02v_merge_ascendings8(d03, d04);
}
static INLINE void sort_04v_descendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04) {
    __m512i  tmp;

    sort_02v_descendings8(d01, d02);
    sort_02v_ascendings8(d03, d04);

    tmp = *d03;
    *d03 = _mm512_max_epi64(*d02, *d03);
    *d02 = _mm512_min_epi64(*d02, tmp);

    tmp = *d04;
    *d04 = _mm512_max_epi64(*d01, *d04);
    *d01 = _mm512_min_epi64(*d01, tmp);

    sort_02v_merge_descendings8(d01, d02);
    sort_02v_merge_descendings8(d03, d04);
}
static INLINE void sort_04v_merge_ascendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04) {
    __m512i  tmp;

    tmp = *d01;
    *d01 = _mm512_min_epi64(*d03, *d01);
    *d03 = _mm512_max_epi64(*d03, tmp);

    tmp = *d02;
    *d02 = _mm512_min_epi64(*d04, *d02);
    *d04 = _mm512_max_epi64(*d04, tmp);

    sort_02v_merge_ascendings8(d01, d02);
    sort_02v_merge_ascendings8(d03, d04);
}
static INLINE void sort_04v_merge_descendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04) {
    __m512i  tmp;

    tmp = *d01;
    *d01 = _mm512_min_epi64(*d03, *d01);
    *d03 = _mm512_max_epi64(*d03, tmp);

    tmp = *d02;
    *d02 = _mm512_min_epi64(*d04, *d02);
    *d04 = _mm512_max_epi64(*d04, tmp);

    sort_02v_merge_descendings8(d01, d02);
    sort_02v_merge_descendings8(d03, d04);
}
static INLINE void sort_05v_ascendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05) {
    __m512i  tmp;

    sort_04v_ascendings8(d01, d02, d03, d04);
    sort_01v_descendings8(d05);

    tmp = *d05;
    *d05 = _mm512_max_epi64(*d04, *d05);
    *d04 = _mm512_min_epi64(*d04, tmp);

    sort_04v_merge_ascendings8(d01, d02, d03, d04);
    sort_01v_merge_ascendings8(d05);
}
static INLINE void sort_05v_descendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05) {
    __m512i  tmp;

    sort_04v_descendings8(d01, d02, d03, d04);
    sort_01v_ascendings8(d05);

    tmp = *d05;
    *d05 = _mm512_max_epi64(*d04, *d05);
    *d04 = _mm512_min_epi64(*d04, tmp);

    sort_04v_merge_descendings8(d01, d02, d03, d04);
    sort_01v_merge_descendings8(d05);
}
static INLINE void sort_06v_ascendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06) {
    __m512i  tmp;

    sort_04v_ascendings8(d01, d02, d03, d04);
    sort_02v_descendings8(d05, d06);

    tmp = *d05;
    *d05 = _mm512_max_epi64(*d04, *d05);
    *d04 = _mm512_min_epi64(*d04, tmp);

    tmp = *d06;
    *d06 = _mm512_max_epi64(*d03, *d06);
    *d03 = _mm512_min_epi64(*d03, tmp);

    sort_04v_merge_ascendings8(d01, d02, d03, d04);
    sort_02v_merge_ascendings8(d05, d06);
}
static INLINE void sort_06v_descendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06) {
    __m512i  tmp;

    sort_04v_descendings8(d01, d02, d03, d04);
    sort_02v_ascendings8(d05, d06);

    tmp = *d05;
    *d05 = _mm512_max_epi64(*d04, *d05);
    *d04 = _mm512_min_epi64(*d04, tmp);

    tmp = *d06;
    *d06 = _mm512_max_epi64(*d03, *d06);
    *d03 = _mm512_min_epi64(*d03, tmp);

    sort_04v_merge_descendings8(d01, d02, d03, d04);
    sort_02v_merge_descendings8(d05, d06);
}
static INLINE void sort_07v_ascendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07) {
    __m512i  tmp;

    sort_04v_ascendings8(d01, d02, d03, d04);
    sort_03v_descendings8(d05, d06, d07);

    tmp = *d05;
    *d05 = _mm512_max_epi64(*d04, *d05);
    *d04 = _mm512_min_epi64(*d04, tmp);

    tmp = *d06;
    *d06 = _mm512_max_epi64(*d03, *d06);
    *d03 = _mm512_min_epi64(*d03, tmp);

    tmp = *d07;
    *d07 = _mm512_max_epi64(*d02, *d07);
    *d02 = _mm512_min_epi64(*d02, tmp);

    sort_04v_merge_ascendings8(d01, d02, d03, d04);
    sort_03v_merge_ascendings8(d05, d06, d07);
}
static INLINE void sort_07v_descendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07) {
    __m512i  tmp;

    sort_04v_descendings8(d01, d02, d03, d04);
    sort_03v_ascendings8(d05, d06, d07);

    tmp = *d05;
    *d05 = _mm512_max_epi64(*d04, *d05);
    *d04 = _mm512_min_epi64(*d04, tmp);

    tmp = *d06;
    *d06 = _mm512_max_epi64(*d03, *d06);
    *d03 = _mm512_min_epi64(*d03, tmp);

    tmp = *d07;
    *d07 = _mm512_max_epi64(*d02, *d07);
    *d02 = _mm512_min_epi64(*d02, tmp);

    sort_04v_merge_descendings8(d01, d02, d03, d04);
    sort_03v_merge_descendings8(d05, d06, d07);
}
static INLINE void sort_08v_ascendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07, __m512i *d08) {
    __m512i  tmp;

    sort_04v_ascendings8(d01, d02, d03, d04);
    sort_04v_descendings8(d05, d06, d07, d08);

    tmp = *d05;
    *d05 = _mm512_max_epi64(*d04, *d05);
    *d04 = _mm512_min_epi64(*d04, tmp);

    tmp = *d06;
    *d06 = _mm512_max_epi64(*d03, *d06);
    *d03 = _mm512_min_epi64(*d03, tmp);

    tmp = *d07;
    *d07 = _mm512_max_epi64(*d02, *d07);
    *d02 = _mm512_min_epi64(*d02, tmp);

    tmp = *d08;
    *d08 = _mm512_max_epi64(*d01, *d08);
    *d01 = _mm512_min_epi64(*d01, tmp);

    sort_04v_merge_ascendings8(d01, d02, d03, d04);
    sort_04v_merge_ascendings8(d05, d06, d07, d08);
}
static INLINE void sort_08v_descendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07, __m512i *d08) {
    __m512i  tmp;

    sort_04v_descendings8(d01, d02, d03, d04);
    sort_04v_ascendings8(d05, d06, d07, d08);

    tmp = *d05;
    *d05 = _mm512_max_epi64(*d04, *d05);
    *d04 = _mm512_min_epi64(*d04, tmp);

    tmp = *d06;
    *d06 = _mm512_max_epi64(*d03, *d06);
    *d03 = _mm512_min_epi64(*d03, tmp);

    tmp = *d07;
    *d07 = _mm512_max_epi64(*d02, *d07);
    *d02 = _mm512_min_epi64(*d02, tmp);

    tmp = *d08;
    *d08 = _mm512_max_epi64(*d01, *d08);
    *d01 = _mm512_min_epi64(*d01, tmp);

    sort_04v_merge_descendings8(d01, d02, d03, d04);
    sort_04v_merge_descendings8(d05, d06, d07, d08);
}
static INLINE void sort_08v_merge_ascendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07, __m512i *d08) {
 __m512i  tmp;

 tmp = *d01;
 *d01 = _mm512_min_epi64(*d05, *d01);
 *d05 = _mm512_max_epi64(*d05, tmp);

 tmp = *d02;
 *d02 = _mm512_min_epi64(*d06, *d02);
 *d06 = _mm512_max_epi64(*d06, tmp);

 tmp = *d03;
 *d03 = _mm512_min_epi64(*d07, *d03);
 *d07 = _mm512_max_epi64(*d07, tmp);

 tmp = *d04;
 *d04 = _mm512_min_epi64(*d08, *d04);
 *d08 = _mm512_max_epi64(*d08, tmp);

 sort_04v_merge_ascendings8(d01, d02, d03, d04);
 sort_04v_merge_ascendings8(d05, d06, d07, d08);
}
static INLINE void sort_08v_merge_descendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07, __m512i *d08) {
 __m512i  tmp;

 tmp = *d01;
 *d01 = _mm512_min_epi64(*d05, *d01);
 *d05 = _mm512_max_epi64(*d05, tmp);

 tmp = *d02;
 *d02 = _mm512_min_epi64(*d06, *d02);
 *d06 = _mm512_max_epi64(*d06, tmp);

 tmp = *d03;
 *d03 = _mm512_min_epi64(*d07, *d03);
 *d07 = _mm512_max_epi64(*d07, tmp);

 tmp = *d04;
 *d04 = _mm512_min_epi64(*d08, *d04);
 *d08 = _mm512_max_epi64(*d08, tmp);

 sort_04v_merge_descendings8(d01, d02, d03, d04);
 sort_04v_merge_descendings8(d05, d06, d07, d08);
}
static INLINE void sort_09v_ascendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07, __m512i *d08, __m512i *d09) {
 __m512i  tmp;

 sort_08v_ascendings8(d01, d02, d03, d04, d05, d06, d07, d08);
 sort_01v_descendings8(d09);

 tmp = *d09;
 *d09 = _mm512_max_epi64(*d08, *d09);
 *d08 = _mm512_min_epi64(*d08, tmp);

 sort_08v_merge_ascendings8(d01, d02, d03, d04, d05, d06, d07, d08);
 sort_01v_merge_ascendings8(d09);
}
static INLINE void sort_09v_descendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07, __m512i *d08, __m512i *d09) {
 __m512i  tmp;

 sort_08v_descendings8(d01, d02, d03, d04, d05, d06, d07, d08);
 sort_01v_ascendings8(d09);

 tmp = *d09;
 *d09 = _mm512_max_epi64(*d08, *d09);
 *d08 = _mm512_min_epi64(*d08, tmp);

 sort_08v_merge_descendings8(d01, d02, d03, d04, d05, d06, d07, d08);
 sort_01v_merge_descendings8(d09);
}
static INLINE void sort_10v_ascendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07, __m512i *d08, __m512i *d09, __m512i *d10) {
 __m512i  tmp;

 sort_08v_ascendings8(d01, d02, d03, d04, d05, d06, d07, d08);
 sort_02v_descendings8(d09, d10);

 tmp = *d09;
 *d09 = _mm512_max_epi64(*d08, *d09);
 *d08 = _mm512_min_epi64(*d08, tmp);

 tmp = *d10;
 *d10 = _mm512_max_epi64(*d07, *d10);
 *d07 = _mm512_min_epi64(*d07, tmp);

 sort_08v_merge_ascendings8(d01, d02, d03, d04, d05, d06, d07, d08);
 sort_02v_merge_ascendings8(d09, d10);
}
static INLINE void sort_10v_descendings8(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07, __m512i *d08, __m512i *d09, __m512i *d10) {
 __m512i  tmp;

 sort_08v_descendings8(d01, d02, d03, d04, d05, d06, d07, d08);
 sort_02v_ascendings8(d09, d10);

 tmp = *d09;
 *d09 = _mm512_max_epi64(*d08, *d09);
 *d08 = _mm512_min_epi64(*d08, tmp);

 tmp = *d10;
 *d10 = _mm512_max_epi64(*d07, *d10);
 *d07 = _mm512_min_epi64(*d07, tmp);

 sort_08v_merge_descendings8(d01, d02, d03, d04, d05, d06, d07, d08);
 sort_02v_merge_descendings8(d09, d10);
}


static NOINLINE void sort_01v_alts8(I *d, I *s, M1 mask) {

    __m512i d01 = _mm512_mask_loadu_epi64(_mm512_set1_epi64(9223372036854775807ll),
                                          mask,
                                          (I const *) ((__m512i const *) s + 0));
    sort_01v_ascendings8(&d01);
    _mm512_mask_storeu_epi64((__m512i *) d + 0, mask, d01);
}

static NOINLINE void sort_02v_alts8(I *d, I *s, M1 mask) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) s + 0);;
    __m512i d02 = _mm512_mask_loadu_epi64(_mm512_set1_epi64(9223372036854775807ll),
                                          mask,
                                          (I const *) ((__m512i const *) s + 1));
    sort_02v_ascendings8(&d01, &d02);
    _mm512_storeu_si512((__m512i *) d + 0, d01);
    _mm512_mask_storeu_epi64((__m512i *) d + 1, mask, d02);
}

static NOINLINE void sort_03v_alts8(I *d, I *s, M1 mask) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) s + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) s + 1);;
    __m512i d03 = _mm512_mask_loadu_epi64(_mm512_set1_epi64(9223372036854775807ll),
                                          mask,
                                          (I const *) ((__m512i const *) s + 2));
    sort_03v_ascendings8(&d01, &d02, &d03);
    _mm512_storeu_si512((__m512i *) d + 0, d01);
    _mm512_storeu_si512((__m512i *) d + 1, d02);
    _mm512_mask_storeu_epi64((__m512i *) d + 2, mask, d03);
}

static NOINLINE void sort_04v_alts8(I *d, I *s, M1 mask) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) s + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) s + 1);;
    __m512i d03 = _mm512_loadu_si512((__m512i const *) s + 2);;
    __m512i d04 = _mm512_mask_loadu_epi64(_mm512_set1_epi64(9223372036854775807ll),
                                          mask,
                                          (I const *) ((__m512i const *) s + 3));
    sort_04v_ascendings8(&d01, &d02, &d03, &d04);
    _mm512_storeu_si512((__m512i *) d + 0, d01);
    _mm512_storeu_si512((__m512i *) d + 1, d02);
    _mm512_storeu_si512((__m512i *) d + 2, d03);
    _mm512_mask_storeu_epi64((__m512i *) d + 3, mask, d04);
}

static NOINLINE void sort_05v_alts8(I *d, I *s, M1 mask) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) s + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) s + 1);;
    __m512i d03 = _mm512_loadu_si512((__m512i const *) s + 2);;
    __m512i d04 = _mm512_loadu_si512((__m512i const *) s + 3);;
    __m512i d05 = _mm512_mask_loadu_epi64(_mm512_set1_epi64(9223372036854775807ll),
                                          mask,
                                          (I const *) ((__m512i const *) s + 4));
    sort_05v_ascendings8(&d01, &d02, &d03, &d04, &d05);
    _mm512_storeu_si512((__m512i *) d + 0, d01);
    _mm512_storeu_si512((__m512i *) d + 1, d02);
    _mm512_storeu_si512((__m512i *) d + 2, d03);
    _mm512_storeu_si512((__m512i *) d + 3, d04);
    _mm512_mask_storeu_epi64((__m512i *) d + 4, mask, d05);
}

static NOINLINE void sort_06v_alts8(I *d, I *s, M1 mask) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) s + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) s + 1);;
    __m512i d03 = _mm512_loadu_si512((__m512i const *) s + 2);;
    __m512i d04 = _mm512_loadu_si512((__m512i const *) s + 3);;
    __m512i d05 = _mm512_loadu_si512((__m512i const *) s + 4);;
    __m512i d06 = _mm512_mask_loadu_epi64(_mm512_set1_epi64(9223372036854775807ll),
                                          mask,
                                          (I const *) ((__m512i const *) s + 5));
    sort_06v_ascendings8(&d01, &d02, &d03, &d04, &d05, &d06);
    _mm512_storeu_si512((__m512i *) d + 0, d01);
    _mm512_storeu_si512((__m512i *) d + 1, d02);
    _mm512_storeu_si512((__m512i *) d + 2, d03);
    _mm512_storeu_si512((__m512i *) d + 3, d04);
    _mm512_storeu_si512((__m512i *) d + 4, d05);
    _mm512_mask_storeu_epi64((__m512i *) d + 5, mask, d06);
}

static NOINLINE void sort_07v_alts8(I *d, I *s, M1 mask) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) s + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) s + 1);;
    __m512i d03 = _mm512_loadu_si512((__m512i const *) s + 2);;
    __m512i d04 = _mm512_loadu_si512((__m512i const *) s + 3);;
    __m512i d05 = _mm512_loadu_si512((__m512i const *) s + 4);;
    __m512i d06 = _mm512_loadu_si512((__m512i const *) s + 5);;
    __m512i d07 = _mm512_mask_loadu_epi64(_mm512_set1_epi64(9223372036854775807ll),
                                          mask,
                                          (I const *) ((__m512i const *) s + 6));
    sort_07v_ascendings8(&d01, &d02, &d03, &d04, &d05, &d06, &d07);
    _mm512_storeu_si512((__m512i *) d + 0, d01);
    _mm512_storeu_si512((__m512i *) d + 1, d02);
    _mm512_storeu_si512((__m512i *) d + 2, d03);
    _mm512_storeu_si512((__m512i *) d + 3, d04);
    _mm512_storeu_si512((__m512i *) d + 4, d05);
    _mm512_storeu_si512((__m512i *) d + 5, d06);
    _mm512_mask_storeu_epi64((__m512i *) d + 6, mask, d07);
}

static NOINLINE void sort_08v_alts8(I *d, I *s, M1 mask) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) s + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) s + 1);;
    __m512i d03 = _mm512_loadu_si512((__m512i const *) s + 2);;
    __m512i d04 = _mm512_loadu_si512((__m512i const *) s + 3);;
    __m512i d05 = _mm512_loadu_si512((__m512i const *) s + 4);;
    __m512i d06 = _mm512_loadu_si512((__m512i const *) s + 5);;
    __m512i d07 = _mm512_loadu_si512((__m512i const *) s + 6);;
    __m512i d08 = _mm512_mask_loadu_epi64(_mm512_set1_epi64(9223372036854775807ll),
                                          mask,
                                          (I const *) ((__m512i const *) s + 7));
    sort_08v_ascendings8(&d01, &d02, &d03, &d04, &d05, &d06, &d07, &d08);
    _mm512_storeu_si512((__m512i *) d + 0, d01);
    _mm512_storeu_si512((__m512i *) d + 1, d02);
    _mm512_storeu_si512((__m512i *) d + 2, d03);
    _mm512_storeu_si512((__m512i *) d + 3, d04);
    _mm512_storeu_si512((__m512i *) d + 4, d05);
    _mm512_storeu_si512((__m512i *) d + 5, d06);
    _mm512_storeu_si512((__m512i *) d + 6, d07);
    _mm512_mask_storeu_epi64((__m512i *) d + 7, mask, d08);
}
static NOINLINE void sort_09v_alts8(I *d, I *s, M1 mask) {
 __m512i d01 = _mm512_loadu_si512((__m512i const *) s + 0);;
 __m512i d02 = _mm512_loadu_si512((__m512i const *) s + 1);;
 __m512i d03 = _mm512_loadu_si512((__m512i const *) s + 2);;
 __m512i d04 = _mm512_loadu_si512((__m512i const *) s + 3);;
 __m512i d05 = _mm512_loadu_si512((__m512i const *) s + 4);;
 __m512i d06 = _mm512_loadu_si512((__m512i const *) s + 5);;
 __m512i d07 = _mm512_loadu_si512((__m512i const *) s + 6);;
 __m512i d08 = _mm512_loadu_si512((__m512i const *) s + 7);;
 __m512i d09 = _mm512_mask_loadu_epi64(_mm512_set1_epi64(9223372036854775807ll),
   mask,
   (I const *) ((__m512i const *) s + 8));
 sort_09v_ascendings8(&d01, &d02, &d03, &d04, &d05, &d06, &d07, &d08, &d09);
 _mm512_storeu_si512((__m512i *) d + 0, d01);
 _mm512_storeu_si512((__m512i *) d + 1, d02);
 _mm512_storeu_si512((__m512i *) d + 2, d03);
 _mm512_storeu_si512((__m512i *) d + 3, d04);
 _mm512_storeu_si512((__m512i *) d + 4, d05);
 _mm512_storeu_si512((__m512i *) d + 5, d06);
 _mm512_storeu_si512((__m512i *) d + 6, d07);
 _mm512_storeu_si512((__m512i *) d + 7, d08);
 _mm512_mask_storeu_epi64((__m512i *) d + 8, mask, d09);
}

static NOINLINE void sort_10v_alts8(I *d, I *s, M1 mask) {
 __m512i d01 = _mm512_loadu_si512((__m512i const *) s + 0);;
 __m512i d02 = _mm512_loadu_si512((__m512i const *) s + 1);;
 __m512i d03 = _mm512_loadu_si512((__m512i const *) s + 2);;
 __m512i d04 = _mm512_loadu_si512((__m512i const *) s + 3);;
 __m512i d05 = _mm512_loadu_si512((__m512i const *) s + 4);;
 __m512i d06 = _mm512_loadu_si512((__m512i const *) s + 5);;
 __m512i d07 = _mm512_loadu_si512((__m512i const *) s + 6);;
 __m512i d08 = _mm512_loadu_si512((__m512i const *) s + 7);;
 __m512i d09 = _mm512_loadu_si512((__m512i const *) s + 8);;
 __m512i d10 = _mm512_mask_loadu_epi64(_mm512_set1_epi64(9223372036854775807ll),
   mask,
   (I const *) ((__m512i const *) s + 9));
 sort_10v_ascendings8(&d01, &d02, &d03, &d04, &d05, &d06, &d07, &d08, &d09, &d10);
 _mm512_storeu_si512((__m512i *) d + 0, d01);
 _mm512_storeu_si512((__m512i *) d + 1, d02);
 _mm512_storeu_si512((__m512i *) d + 2, d03);
 _mm512_storeu_si512((__m512i *) d + 3, d04);
 _mm512_storeu_si512((__m512i *) d + 4, d05);
 _mm512_storeu_si512((__m512i *) d + 5, d06);
 _mm512_storeu_si512((__m512i *) d + 6, d07);
 _mm512_storeu_si512((__m512i *) d + 7, d08);
 _mm512_storeu_si512((__m512i *) d + 8, d09);
 _mm512_mask_storeu_epi64((__m512i *) d + 9, mask, d10);
}


static INLINE void sort_01v_ascendingu4(__m512i *d01) {
    __m512i  min, s;

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_CDAB);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0xAAAA, s, *d01);

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_ABCD);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0xCCCC, s, *d01);

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_CDAB);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0xAAAA, s, *d01);

    s = _mm512_permutex_epi64(_mm512_shuffle_epi32(*d01, _MM_PERM_ABCD), _MM_PERM_BADC);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0xF0F0, s, *d01);

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_BADC);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0xCCCC, s, *d01);

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_CDAB);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0xAAAA, s, *d01);

    s = _mm512_shuffle_i64x2(_mm512_shuffle_epi32(*d01, _MM_PERM_ABCD), _mm512_shuffle_epi32(*d01, _MM_PERM_ABCD), _MM_PERM_ABCD);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0xFF00, s, *d01);

    s = _mm512_permutex_epi64(*d01, _MM_PERM_BADC);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0xF0F0, s, *d01);

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_BADC);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0xCCCC, s, *d01);

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_CDAB);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0xAAAA, s, *d01);
}
static INLINE void sort_01v_merge_ascendingu4(__m512i *d01) {
    __m512i  min, s;

    s = _mm512_shuffle_i64x2(*d01, *d01, _MM_PERM_BADC);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0xFF00, s, *d01);

    s = _mm512_permutex_epi64(*d01, _MM_PERM_BADC);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0xF0F0, s, *d01);

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_BADC);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0xCCCC, s, *d01);

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_CDAB);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0xAAAA, s, *d01);
}
static INLINE void sort_01v_descendingu4(__m512i *d01) {
    __m512i  min, s;

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_CDAB);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0x5555, s, *d01);

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_ABCD);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0x3333, s, *d01);

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_CDAB);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0x5555, s, *d01);

    s = _mm512_permutex_epi64(_mm512_shuffle_epi32(*d01, _MM_PERM_ABCD), _MM_PERM_BADC);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0x0F0F, s, *d01);

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_BADC);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0x3333, s, *d01);

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_CDAB);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0x5555, s, *d01);

    s = _mm512_shuffle_i64x2(_mm512_shuffle_epi32(*d01, _MM_PERM_ABCD), _mm512_shuffle_epi32(*d01, _MM_PERM_ABCD), _MM_PERM_ABCD);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0x00FF, s, *d01);

    s = _mm512_permutex_epi64(*d01, _MM_PERM_BADC);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0x0F0F, s, *d01);

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_BADC);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0x3333, s, *d01);

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_CDAB);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0x5555, s, *d01);
}
static INLINE void sort_01v_merge_descendingu4(__m512i *d01) {
    __m512i  min, s;

    s = _mm512_shuffle_i64x2(*d01, *d01, _MM_PERM_BADC);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0x00FF, s, *d01);

    s = _mm512_permutex_epi64(*d01, _MM_PERM_BADC);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0x0F0F, s, *d01);

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_BADC);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0x3333, s, *d01);

    s = _mm512_shuffle_epi32(*d01, _MM_PERM_CDAB);
    min = _mm512_min_epu32(s, *d01);
    *d01 = _mm512_mask_max_epu32(min, 0x5555, s, *d01);
}
static INLINE void sort_02v_ascendingu4(__m512i *d01, __m512i *d02) {
    __m512i  tmp;

    sort_01v_ascendingu4(d01);
    sort_01v_descendingu4(d02);

    tmp = *d02;
    *d02 = _mm512_max_epu32(*d01, *d02);
    *d01 = _mm512_min_epu32(*d01, tmp);

    sort_01v_merge_ascendingu4(d01);
    sort_01v_merge_ascendingu4(d02);
}
static INLINE void sort_02v_descendingu4(__m512i *d01, __m512i *d02) {
    __m512i  tmp;

    sort_01v_descendingu4(d01);
    sort_01v_ascendingu4(d02);

    tmp = *d02;
    *d02 = _mm512_max_epu32(*d01, *d02);
    *d01 = _mm512_min_epu32(*d01, tmp);

    sort_01v_merge_descendingu4(d01);
    sort_01v_merge_descendingu4(d02);
}
static INLINE void sort_02v_merge_ascendingu4(__m512i *d01, __m512i *d02) {
    __m512i  tmp;

    tmp = *d01;
    *d01 = _mm512_min_epu32(*d02, *d01);
    *d02 = _mm512_max_epu32(*d02, tmp);

    sort_01v_merge_ascendingu4(d01);
    sort_01v_merge_ascendingu4(d02);
}
static INLINE void sort_02v_merge_descendingu4(__m512i *d01, __m512i *d02) {
    __m512i  tmp;

    tmp = *d01;
    *d01 = _mm512_min_epu32(*d02, *d01);
    *d02 = _mm512_max_epu32(*d02, tmp);

    sort_01v_merge_descendingu4(d01);
    sort_01v_merge_descendingu4(d02);
}
static INLINE void sort_03v_ascendingu4(__m512i *d01, __m512i *d02, __m512i *d03) {
    __m512i  tmp;

    sort_02v_ascendingu4(d01, d02);
    sort_01v_descendingu4(d03);

    tmp = *d03;
    *d03 = _mm512_max_epu32(*d02, *d03);
    *d02 = _mm512_min_epu32(*d02, tmp);

    sort_02v_merge_ascendingu4(d01, d02);
    sort_01v_merge_ascendingu4(d03);
}
static INLINE void sort_03v_descendingu4(__m512i *d01, __m512i *d02, __m512i *d03) {
    __m512i  tmp;

    sort_02v_descendingu4(d01, d02);
    sort_01v_ascendingu4(d03);

    tmp = *d03;
    *d03 = _mm512_max_epu32(*d02, *d03);
    *d02 = _mm512_min_epu32(*d02, tmp);

    sort_02v_merge_descendingu4(d01, d02);
    sort_01v_merge_descendingu4(d03);
}
static INLINE void sort_03v_merge_ascendingu4(__m512i *d01, __m512i *d02, __m512i *d03) {
    __m512i  tmp;

    tmp = *d01;
    *d01 = _mm512_min_epu32(*d03, *d01);
    *d03 = _mm512_max_epu32(*d03, tmp);

    sort_02v_merge_ascendingu4(d01, d02);
    sort_01v_merge_ascendingu4(d03);
}
static INLINE void sort_03v_merge_descendingu4(__m512i *d01, __m512i *d02, __m512i *d03) {
    __m512i  tmp;

    tmp = *d01;
    *d01 = _mm512_min_epu32(*d03, *d01);
    *d03 = _mm512_max_epu32(*d03, tmp);

    sort_02v_merge_descendingu4(d01, d02);
    sort_01v_merge_descendingu4(d03);
}
static INLINE void sort_04v_ascendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04) {
    __m512i  tmp;

    sort_02v_ascendingu4(d01, d02);
    sort_02v_descendingu4(d03, d04);

    tmp = *d03;
    *d03 = _mm512_max_epu32(*d02, *d03);
    *d02 = _mm512_min_epu32(*d02, tmp);

    tmp = *d04;
    *d04 = _mm512_max_epu32(*d01, *d04);
    *d01 = _mm512_min_epu32(*d01, tmp);

    sort_02v_merge_ascendingu4(d01, d02);
    sort_02v_merge_ascendingu4(d03, d04);
}
static INLINE void sort_04v_descendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04) {
    __m512i  tmp;

    sort_02v_descendingu4(d01, d02);
    sort_02v_ascendingu4(d03, d04);

    tmp = *d03;
    *d03 = _mm512_max_epu32(*d02, *d03);
    *d02 = _mm512_min_epu32(*d02, tmp);

    tmp = *d04;
    *d04 = _mm512_max_epu32(*d01, *d04);
    *d01 = _mm512_min_epu32(*d01, tmp);

    sort_02v_merge_descendingu4(d01, d02);
    sort_02v_merge_descendingu4(d03, d04);
}
static INLINE void sort_04v_merge_ascendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04) {
    __m512i  tmp;

    tmp = *d01;
    *d01 = _mm512_min_epu32(*d03, *d01);
    *d03 = _mm512_max_epu32(*d03, tmp);

    tmp = *d02;
    *d02 = _mm512_min_epu32(*d04, *d02);
    *d04 = _mm512_max_epu32(*d04, tmp);

    sort_02v_merge_ascendingu4(d01, d02);
    sort_02v_merge_ascendingu4(d03, d04);
}
static INLINE void sort_04v_merge_descendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04) {
    __m512i  tmp;

    tmp = *d01;
    *d01 = _mm512_min_epu32(*d03, *d01);
    *d03 = _mm512_max_epu32(*d03, tmp);

    tmp = *d02;
    *d02 = _mm512_min_epu32(*d04, *d02);
    *d04 = _mm512_max_epu32(*d04, tmp);

    sort_02v_merge_descendingu4(d01, d02);
    sort_02v_merge_descendingu4(d03, d04);
}
static INLINE void sort_05v_ascendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05) {
    __m512i  tmp;

    sort_04v_ascendingu4(d01, d02, d03, d04);
    sort_01v_descendingu4(d05);

    tmp = *d05;
    *d05 = _mm512_max_epu32(*d04, *d05);
    *d04 = _mm512_min_epu32(*d04, tmp);

    sort_04v_merge_ascendingu4(d01, d02, d03, d04);
    sort_01v_merge_ascendingu4(d05);
}
static INLINE void sort_05v_descendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05) {
    __m512i  tmp;

    sort_04v_descendingu4(d01, d02, d03, d04);
    sort_01v_ascendingu4(d05);

    tmp = *d05;
    *d05 = _mm512_max_epu32(*d04, *d05);
    *d04 = _mm512_min_epu32(*d04, tmp);

    sort_04v_merge_descendingu4(d01, d02, d03, d04);
    sort_01v_merge_descendingu4(d05);
}
static INLINE void sort_06v_ascendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06) {
    __m512i  tmp;

    sort_04v_ascendingu4(d01, d02, d03, d04);
    sort_02v_descendingu4(d05, d06);

    tmp = *d05;
    *d05 = _mm512_max_epu32(*d04, *d05);
    *d04 = _mm512_min_epu32(*d04, tmp);

    tmp = *d06;
    *d06 = _mm512_max_epu32(*d03, *d06);
    *d03 = _mm512_min_epu32(*d03, tmp);

    sort_04v_merge_ascendingu4(d01, d02, d03, d04);
    sort_02v_merge_ascendingu4(d05, d06);
}
static INLINE void sort_06v_descendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06) {
    __m512i  tmp;

    sort_04v_descendingu4(d01, d02, d03, d04);
    sort_02v_ascendingu4(d05, d06);

    tmp = *d05;
    *d05 = _mm512_max_epu32(*d04, *d05);
    *d04 = _mm512_min_epu32(*d04, tmp);

    tmp = *d06;
    *d06 = _mm512_max_epu32(*d03, *d06);
    *d03 = _mm512_min_epu32(*d03, tmp);

    sort_04v_merge_descendingu4(d01, d02, d03, d04);
    sort_02v_merge_descendingu4(d05, d06);
}
static INLINE void sort_07v_ascendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07) {
    __m512i  tmp;

    sort_04v_ascendingu4(d01, d02, d03, d04);
    sort_03v_descendingu4(d05, d06, d07);

    tmp = *d05;
    *d05 = _mm512_max_epu32(*d04, *d05);
    *d04 = _mm512_min_epu32(*d04, tmp);

    tmp = *d06;
    *d06 = _mm512_max_epu32(*d03, *d06);
    *d03 = _mm512_min_epu32(*d03, tmp);

    tmp = *d07;
    *d07 = _mm512_max_epu32(*d02, *d07);
    *d02 = _mm512_min_epu32(*d02, tmp);

    sort_04v_merge_ascendingu4(d01, d02, d03, d04);
    sort_03v_merge_ascendingu4(d05, d06, d07);
}
static INLINE void sort_07v_descendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07) {
    __m512i  tmp;

    sort_04v_descendingu4(d01, d02, d03, d04);
    sort_03v_ascendingu4(d05, d06, d07);

    tmp = *d05;
    *d05 = _mm512_max_epu32(*d04, *d05);
    *d04 = _mm512_min_epu32(*d04, tmp);

    tmp = *d06;
    *d06 = _mm512_max_epu32(*d03, *d06);
    *d03 = _mm512_min_epu32(*d03, tmp);

    tmp = *d07;
    *d07 = _mm512_max_epu32(*d02, *d07);
    *d02 = _mm512_min_epu32(*d02, tmp);

    sort_04v_merge_descendingu4(d01, d02, d03, d04);
    sort_03v_merge_descendingu4(d05, d06, d07);
}
static INLINE void sort_08v_ascendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07, __m512i *d08) {
    __m512i  tmp;

    sort_04v_ascendingu4(d01, d02, d03, d04);
    sort_04v_descendingu4(d05, d06, d07, d08);

    tmp = *d05;
    *d05 = _mm512_max_epu32(*d04, *d05);
    *d04 = _mm512_min_epu32(*d04, tmp);

    tmp = *d06;
    *d06 = _mm512_max_epu32(*d03, *d06);
    *d03 = _mm512_min_epu32(*d03, tmp);

    tmp = *d07;
    *d07 = _mm512_max_epu32(*d02, *d07);
    *d02 = _mm512_min_epu32(*d02, tmp);

    tmp = *d08;
    *d08 = _mm512_max_epu32(*d01, *d08);
    *d01 = _mm512_min_epu32(*d01, tmp);

    sort_04v_merge_ascendingu4(d01, d02, d03, d04);
    sort_04v_merge_ascendingu4(d05, d06, d07, d08);
}
static INLINE void sort_08v_descendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07, __m512i *d08) {
    __m512i  tmp;

    sort_04v_descendingu4(d01, d02, d03, d04);
    sort_04v_ascendingu4(d05, d06, d07, d08);

    tmp = *d05;
    *d05 = _mm512_max_epu32(*d04, *d05);
    *d04 = _mm512_min_epu32(*d04, tmp);

    tmp = *d06;
    *d06 = _mm512_max_epu32(*d03, *d06);
    *d03 = _mm512_min_epu32(*d03, tmp);

    tmp = *d07;
    *d07 = _mm512_max_epu32(*d02, *d07);
    *d02 = _mm512_min_epu32(*d02, tmp);

    tmp = *d08;
    *d08 = _mm512_max_epu32(*d01, *d08);
    *d01 = _mm512_min_epu32(*d01, tmp);

    sort_04v_merge_descendingu4(d01, d02, d03, d04);
    sort_04v_merge_descendingu4(d05, d06, d07, d08);
}
static INLINE void sort_08v_merge_ascendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07, __m512i *d08) {
 __m512i  tmp;

 tmp = *d01;
 *d01 = _mm512_min_epu32(*d05, *d01);
 *d05 = _mm512_max_epu32(*d05, tmp);

 tmp = *d02;
 *d02 = _mm512_min_epu32(*d06, *d02);
 *d06 = _mm512_max_epu32(*d06, tmp);

 tmp = *d03;
 *d03 = _mm512_min_epu32(*d07, *d03);
 *d07 = _mm512_max_epu32(*d07, tmp);

 tmp = *d04;
 *d04 = _mm512_min_epu32(*d08, *d04);
 *d08 = _mm512_max_epu32(*d08, tmp);

 sort_04v_merge_ascendingu4(d01, d02, d03, d04);
 sort_04v_merge_ascendingu4(d05, d06, d07, d08);
}
static INLINE void sort_08v_merge_descendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07, __m512i *d08) {
 __m512i  tmp;

 tmp = *d01;
 *d01 = _mm512_min_epu32(*d05, *d01);
 *d05 = _mm512_max_epu32(*d05, tmp);

 tmp = *d02;
 *d02 = _mm512_min_epu32(*d06, *d02);
 *d06 = _mm512_max_epu32(*d06, tmp);

 tmp = *d03;
 *d03 = _mm512_min_epu32(*d07, *d03);
 *d07 = _mm512_max_epu32(*d07, tmp);

 tmp = *d04;
 *d04 = _mm512_min_epu32(*d08, *d04);
 *d08 = _mm512_max_epu32(*d08, tmp);

 sort_04v_merge_descendingu4(d01, d02, d03, d04);
 sort_04v_merge_descendingu4(d05, d06, d07, d08);
}


static INLINE void sort_09v_ascendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07, __m512i *d08, __m512i *d09) {
 __m512i  tmp;

 sort_08v_ascendingu4(d01, d02, d03, d04, d05, d06, d07, d08);
 sort_01v_descendingu4(d09);

 tmp = *d09;
 *d09 = _mm512_max_epu32(*d08, *d09);
 *d08 = _mm512_min_epu32(*d08, tmp);

 sort_08v_merge_ascendingu4(d01, d02, d03, d04, d05, d06, d07, d08);
 sort_01v_merge_ascendingu4(d09);
}
static INLINE void sort_09v_descendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07, __m512i *d08, __m512i *d09) {
 __m512i  tmp;

 sort_08v_descendingu4(d01, d02, d03, d04, d05, d06, d07, d08);
 sort_01v_ascendingu4(d09);

 tmp = *d09;
 *d09 = _mm512_max_epu32(*d08, *d09);
 *d08 = _mm512_min_epu32(*d08, tmp);

 sort_08v_merge_descendingu4(d01, d02, d03, d04, d05, d06, d07, d08);
 sort_01v_merge_descendingu4(d09);
}
static INLINE void sort_10v_ascendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07, __m512i *d08, __m512i *d09, __m512i *d10) {
 __m512i  tmp;

 sort_08v_ascendingu4(d01, d02, d03, d04, d05, d06, d07, d08);
 sort_02v_descendingu4(d09, d10);

 tmp = *d09;
 *d09 = _mm512_max_epu32(*d08, *d09);
 *d08 = _mm512_min_epu32(*d08, tmp);

 tmp = *d10;
 *d10 = _mm512_max_epu32(*d07, *d10);
 *d07 = _mm512_min_epu32(*d07, tmp);

 sort_08v_merge_ascendingu4(d01, d02, d03, d04, d05, d06, d07, d08);
 sort_02v_merge_ascendingu4(d09, d10);
}
static INLINE void sort_10v_descendingu4(__m512i *d01, __m512i *d02, __m512i *d03, __m512i *d04, __m512i *d05, __m512i *d06, __m512i *d07, __m512i *d08, __m512i *d09, __m512i *d10) {
 __m512i  tmp;

 sort_08v_descendingu4(d01, d02, d03, d04, d05, d06, d07, d08);
 sort_02v_ascendingu4(d09, d10);

 tmp = *d09;
 *d09 = _mm512_max_epu32(*d08, *d09);
 *d08 = _mm512_min_epu32(*d08, tmp);

 tmp = *d10;
 *d10 = _mm512_max_epu32(*d07, *d10);
 *d07 = _mm512_min_epu32(*d07, tmp);

 sort_08v_merge_descendingu4(d01, d02, d03, d04, d05, d06, d07, d08);
 sort_02v_merge_descendingu4(d09, d10);
}


static NOINLINE void sort_01v_altu4(uint32_t *ptr, M2 mask) {

    __m512i d01 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
                                          mask,
                                          (uint32_t const *) ((__m512i const *) ptr + 0));
    sort_01v_ascendingu4(&d01);
    _mm512_mask_storeu_epi32((__m512i *) ptr + 0, mask, d01);
}

static NOINLINE void sort_02v_altu4(uint32_t *ptr, M2 mask) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
    __m512i d02 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
                                          mask,
                                          (uint32_t const *) ((__m512i const *) ptr + 1));
    sort_02v_ascendingu4(&d01, &d02);
    _mm512_storeu_si512((__m512i *) ptr + 0, d01);
    _mm512_mask_storeu_epi32((__m512i *) ptr + 1, mask, d02);
}

static NOINLINE void sort_03v_altu4(uint32_t *ptr, M2 mask) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) ptr + 1);;
    __m512i d03 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
                                          mask,
                                          (uint32_t const *) ((__m512i const *) ptr + 2));
    sort_03v_ascendingu4(&d01, &d02, &d03);
    _mm512_storeu_si512((__m512i *) ptr + 0, d01);
    _mm512_storeu_si512((__m512i *) ptr + 1, d02);
    _mm512_mask_storeu_epi32((__m512i *) ptr + 2, mask, d03);
}

static NOINLINE void sort_04v_altu4(uint32_t *ptr, M2 mask) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) ptr + 1);;
    __m512i d03 = _mm512_loadu_si512((__m512i const *) ptr + 2);;
    __m512i d04 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
                                          mask,
                                          (uint32_t const *) ((__m512i const *) ptr + 3));
    sort_04v_ascendingu4(&d01, &d02, &d03, &d04);
    _mm512_storeu_si512((__m512i *) ptr + 0, d01);
    _mm512_storeu_si512((__m512i *) ptr + 1, d02);
    _mm512_storeu_si512((__m512i *) ptr + 2, d03);
    _mm512_mask_storeu_epi32((__m512i *) ptr + 3, mask, d04);
}

static NOINLINE void sort_05v_altu4(uint32_t *ptr, M2 mask) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) ptr + 1);;
    __m512i d03 = _mm512_loadu_si512((__m512i const *) ptr + 2);;
    __m512i d04 = _mm512_loadu_si512((__m512i const *) ptr + 3);;
    __m512i d05 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
                                          mask,
                                          (uint32_t const *) ((__m512i const *) ptr + 4));
    sort_05v_ascendingu4(&d01, &d02, &d03, &d04, &d05);
    _mm512_storeu_si512((__m512i *) ptr + 0, d01);
    _mm512_storeu_si512((__m512i *) ptr + 1, d02);
    _mm512_storeu_si512((__m512i *) ptr + 2, d03);
    _mm512_storeu_si512((__m512i *) ptr + 3, d04);
    _mm512_mask_storeu_epi32((__m512i *) ptr + 4, mask, d05);
}

static NOINLINE void sort_06v_altu4(uint32_t *ptr, M2 mask) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) ptr + 1);;
    __m512i d03 = _mm512_loadu_si512((__m512i const *) ptr + 2);;
    __m512i d04 = _mm512_loadu_si512((__m512i const *) ptr + 3);;
    __m512i d05 = _mm512_loadu_si512((__m512i const *) ptr + 4);;
    __m512i d06 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
                                          mask,
                                          (uint32_t const *) ((__m512i const *) ptr + 5));
    sort_06v_ascendingu4(&d01, &d02, &d03, &d04, &d05, &d06);
    _mm512_storeu_si512((__m512i *) ptr + 0, d01);
    _mm512_storeu_si512((__m512i *) ptr + 1, d02);
    _mm512_storeu_si512((__m512i *) ptr + 2, d03);
    _mm512_storeu_si512((__m512i *) ptr + 3, d04);
    _mm512_storeu_si512((__m512i *) ptr + 4, d05);
    _mm512_mask_storeu_epi32((__m512i *) ptr + 5, mask, d06);
}

static NOINLINE void sort_07v_altu4(uint32_t *ptr, M2 mask) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) ptr + 1);;
    __m512i d03 = _mm512_loadu_si512((__m512i const *) ptr + 2);;
    __m512i d04 = _mm512_loadu_si512((__m512i const *) ptr + 3);;
    __m512i d05 = _mm512_loadu_si512((__m512i const *) ptr + 4);;
    __m512i d06 = _mm512_loadu_si512((__m512i const *) ptr + 5);;
    __m512i d07 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
                                          mask,
                                          (uint32_t const *) ((__m512i const *) ptr + 6));
    sort_07v_ascendingu4(&d01, &d02, &d03, &d04, &d05, &d06, &d07);
    _mm512_storeu_si512((__m512i *) ptr + 0, d01);
    _mm512_storeu_si512((__m512i *) ptr + 1, d02);
    _mm512_storeu_si512((__m512i *) ptr + 2, d03);
    _mm512_storeu_si512((__m512i *) ptr + 3, d04);
    _mm512_storeu_si512((__m512i *) ptr + 4, d05);
    _mm512_storeu_si512((__m512i *) ptr + 5, d06);
    _mm512_mask_storeu_epi32((__m512i *) ptr + 6, mask, d07);
}

static NOINLINE void sort_08v_altu4(uint32_t *ptr, M2 mask) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) ptr + 1);;
    __m512i d03 = _mm512_loadu_si512((__m512i const *) ptr + 2);;
    __m512i d04 = _mm512_loadu_si512((__m512i const *) ptr + 3);;
    __m512i d05 = _mm512_loadu_si512((__m512i const *) ptr + 4);;
    __m512i d06 = _mm512_loadu_si512((__m512i const *) ptr + 5);;
    __m512i d07 = _mm512_loadu_si512((__m512i const *) ptr + 6);;
    __m512i d08 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
                                          mask,
                                          (uint32_t const *) ((__m512i const *) ptr + 7));
    sort_08v_ascendingu4(&d01, &d02, &d03, &d04, &d05, &d06, &d07, &d08);
    _mm512_storeu_si512((__m512i *) ptr + 0, d01);
    _mm512_storeu_si512((__m512i *) ptr + 1, d02);
    _mm512_storeu_si512((__m512i *) ptr + 2, d03);
    _mm512_storeu_si512((__m512i *) ptr + 3, d04);
    _mm512_storeu_si512((__m512i *) ptr + 4, d05);
    _mm512_storeu_si512((__m512i *) ptr + 5, d06);
    _mm512_storeu_si512((__m512i *) ptr + 6, d07);
    _mm512_mask_storeu_epi32((__m512i *) ptr + 7, mask, d08);
}

static NOINLINE void sort_09v_altu4(uint32_t *ptr, M2 mask) {
 __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
 __m512i d02 = _mm512_loadu_si512((__m512i const *) ptr + 1);;
 __m512i d03 = _mm512_loadu_si512((__m512i const *) ptr + 2);;
 __m512i d04 = _mm512_loadu_si512((__m512i const *) ptr + 3);;
 __m512i d05 = _mm512_loadu_si512((__m512i const *) ptr + 4);;
 __m512i d06 = _mm512_loadu_si512((__m512i const *) ptr + 5);;
 __m512i d07 = _mm512_loadu_si512((__m512i const *) ptr + 6);;
 __m512i d08 = _mm512_loadu_si512((__m512i const *) ptr + 7);;
 __m512i d09 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
   mask,
   (uint32_t const *) ((__m512i const *) ptr + 8));
 sort_09v_ascendingu4(&d01, &d02, &d03, &d04, &d05, &d06, &d07, &d08, &d09);
 _mm512_storeu_si512((__m512i *) ptr + 0, d01);
 _mm512_storeu_si512((__m512i *) ptr + 1, d02);
 _mm512_storeu_si512((__m512i *) ptr + 2, d03);
 _mm512_storeu_si512((__m512i *) ptr + 3, d04);
 _mm512_storeu_si512((__m512i *) ptr + 4, d05);
 _mm512_storeu_si512((__m512i *) ptr + 5, d06);
 _mm512_storeu_si512((__m512i *) ptr + 6, d07);
 _mm512_storeu_si512((__m512i *) ptr + 7, d08);
 _mm512_mask_storeu_epi32((__m512i *) ptr + 8, mask, d09);
}

static NOINLINE void sort_10v_altu4(uint32_t *ptr, M2 mask) {
 __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
 __m512i d02 = _mm512_loadu_si512((__m512i const *) ptr + 1);;
 __m512i d03 = _mm512_loadu_si512((__m512i const *) ptr + 2);;
 __m512i d04 = _mm512_loadu_si512((__m512i const *) ptr + 3);;
 __m512i d05 = _mm512_loadu_si512((__m512i const *) ptr + 4);;
 __m512i d06 = _mm512_loadu_si512((__m512i const *) ptr + 5);;
 __m512i d07 = _mm512_loadu_si512((__m512i const *) ptr + 6);;
 __m512i d08 = _mm512_loadu_si512((__m512i const *) ptr + 7);;
 __m512i d09 = _mm512_loadu_si512((__m512i const *) ptr + 8);;
 __m512i d10 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
   mask,
   (uint32_t const *) ((__m512i const *) ptr + 9));
 sort_10v_ascendingu4(&d01, &d02, &d03, &d04, &d05, &d06, &d07, &d08, &d09, &d10);
 _mm512_storeu_si512((__m512i *) ptr + 0, d01);
 _mm512_storeu_si512((__m512i *) ptr + 1, d02);
 _mm512_storeu_si512((__m512i *) ptr + 2, d03);
 _mm512_storeu_si512((__m512i *) ptr + 3, d04);
 _mm512_storeu_si512((__m512i *) ptr + 4, d05);
 _mm512_storeu_si512((__m512i *) ptr + 5, d06);
 _mm512_storeu_si512((__m512i *) ptr + 6, d07);
 _mm512_storeu_si512((__m512i *) ptr + 7, d08);
 _mm512_storeu_si512((__m512i *) ptr + 8, d09);
 _mm512_mask_storeu_epi32((__m512i *) ptr + 9, mask, d10);
}


static INLINE void write2(I *o,V v,V base){
 V v0=_mm512_add_epi64(base,_mm512_cvtepu32_epi64(_mm512_extracti64x4_epi64(v,0))),
   v1=_mm512_add_epi64(base,_mm512_cvtepu32_epi64(_mm512_extracti64x4_epi64(v,1)));
 _mm512_storeu_si512(o,  v0);
 _mm512_storeu_si512(o+8,v1);}
static INLINE void write2m(I *o,V v,V base,M1 w0,M1 w1){
 V v0=_mm512_add_epi64(base,_mm512_cvtepu32_epi64(_mm512_extracti64x4_epi64(v,0))),
   v1=_mm512_add_epi64(base,_mm512_cvtepu32_epi64(_mm512_extracti64x4_epi64(v,1)));
 _mm512_mask_storeu_epi64(o,  w0,v0);
 _mm512_mask_storeu_epi64(o+8,w1,v1);}

static NOINLINE void sort_01v_altu4e(uint32_t *ptr, I *o, V base, M2 mask,M1 w0,M1 w1) {
    __m512i d01 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
                                          mask,
                                          (uint32_t const *) ((__m512i const *) ptr + 0));
    sort_01v_ascendingu4(&d01);
    write2m(o,d01,base,w0,w1);
}

static NOINLINE void sort_02v_altu4e(uint32_t *ptr,I *o,V base,M2 mask,M1 w0,M1 w1) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
    __m512i d02 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
                                          mask,
                                          (uint32_t const *) ((__m512i const *) ptr + 1));
    sort_02v_ascendingu4(&d01, &d02);
    write2(o,d01,base);
    write2m(o+16,d02,base,w0,w1);
}

static NOINLINE void sort_03v_altu4e(uint32_t *ptr, I *o,V base,M2 mask,M1 w0,M1 w1) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) ptr + 1);;
    __m512i d03 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
                                          mask,
                                          (uint32_t const *) ((__m512i const *) ptr + 2));
    sort_03v_ascendingu4(&d01, &d02, &d03);
    write2(o,d01,base);
    write2(o+16,d02,base);
    write2m(o+32,d03,base,w0,w1);
}

static NOINLINE void sort_04v_altu4e(uint32_t *ptr, I *o,V base,M2 mask,M1 w0,M1 w1) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) ptr + 1);;
    __m512i d03 = _mm512_loadu_si512((__m512i const *) ptr + 2);;
    __m512i d04 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
                                          mask,
                                          (uint32_t const *) ((__m512i const *) ptr + 3));
    sort_04v_ascendingu4(&d01, &d02, &d03, &d04);
    write2(o,d01,base);
    write2(o+16,d02,base);
    write2(o+32,d03,base);
    write2m(o+48,d04,base,w0,w1);
}

static NOINLINE void sort_05v_altu4e(uint32_t *ptr, I *o,V base,M2 mask,M1 w0,M1 w1) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) ptr + 1);;
    __m512i d03 = _mm512_loadu_si512((__m512i const *) ptr + 2);;
    __m512i d04 = _mm512_loadu_si512((__m512i const *) ptr + 3);;
    __m512i d05 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
                                          mask,
                                          (uint32_t const *) ((__m512i const *) ptr + 4));
    sort_05v_ascendingu4(&d01, &d02, &d03, &d04, &d05);
    write2(o,d01,base);
    write2(o+16,d02,base);
    write2(o+32,d03,base);
    write2(o+48,d04,base);
    write2m(o+64,d05,base,w0,w1);
}

static NOINLINE void sort_06v_altu4e(uint32_t *ptr, I *o,V base,M2 mask,M1 w0,M1 w1) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) ptr + 1);;
    __m512i d03 = _mm512_loadu_si512((__m512i const *) ptr + 2);;
    __m512i d04 = _mm512_loadu_si512((__m512i const *) ptr + 3);;
    __m512i d05 = _mm512_loadu_si512((__m512i const *) ptr + 4);;
    __m512i d06 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
                                          mask,
                                          (uint32_t const *) ((__m512i const *) ptr + 5));
    sort_06v_ascendingu4(&d01, &d02, &d03, &d04, &d05, &d06);
    write2(o,d01,base);
    write2(o+16,d02,base);
    write2(o+32,d03,base);
    write2(o+48,d04,base);
    write2(o+64,d05,base);
    write2m(o+80,d06,base,w0,w1);
}

static NOINLINE void sort_07v_altu4e(uint32_t *ptr, I *o,V base,M2 mask,M1 w0,M1 w1) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) ptr + 1);;
    __m512i d03 = _mm512_loadu_si512((__m512i const *) ptr + 2);;
    __m512i d04 = _mm512_loadu_si512((__m512i const *) ptr + 3);;
    __m512i d05 = _mm512_loadu_si512((__m512i const *) ptr + 4);;
    __m512i d06 = _mm512_loadu_si512((__m512i const *) ptr + 5);;
    __m512i d07 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
                                          mask,
                                          (uint32_t const *) ((__m512i const *) ptr + 6));
    sort_07v_ascendingu4(&d01, &d02, &d03, &d04, &d05, &d06, &d07);
    write2(o,d01,base);
    write2(o+16,d02,base);
    write2(o+32,d03,base);
    write2(o+48,d04,base);
    write2(o+64,d05,base);
    write2(o+80,d06,base);
    write2m(o+96,d07,base,w0,w1);
}

static NOINLINE void sort_08v_altu4e(uint32_t *ptr, I *o,V base,M2 mask,M1 w0,M1 w1) {

    __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
    __m512i d02 = _mm512_loadu_si512((__m512i const *) ptr + 1);;
    __m512i d03 = _mm512_loadu_si512((__m512i const *) ptr + 2);;
    __m512i d04 = _mm512_loadu_si512((__m512i const *) ptr + 3);;
    __m512i d05 = _mm512_loadu_si512((__m512i const *) ptr + 4);;
    __m512i d06 = _mm512_loadu_si512((__m512i const *) ptr + 5);;
    __m512i d07 = _mm512_loadu_si512((__m512i const *) ptr + 6);;
    __m512i d08 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
                                          mask,
                                          (uint32_t const *) ((__m512i const *) ptr + 7));
    sort_08v_ascendingu4(&d01, &d02, &d03, &d04, &d05, &d06, &d07, &d08);
    write2(o,d01,base);
    write2(o+16,d02,base);
    write2(o+32,d03,base);
    write2(o+48,d04,base);
    write2(o+64,d05,base);
    write2(o+80,d06,base);
    write2(o+96,d07,base);
    write2m(o+112,d08,base,w0,w1);
}

static NOINLINE void sort_09v_altu4e(uint32_t *ptr, I *o,V base,M2 mask,M1 w0,M1 w1) {
 __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
 __m512i d02 = _mm512_loadu_si512((__m512i const *) ptr + 1);;
 __m512i d03 = _mm512_loadu_si512((__m512i const *) ptr + 2);;
 __m512i d04 = _mm512_loadu_si512((__m512i const *) ptr + 3);;
 __m512i d05 = _mm512_loadu_si512((__m512i const *) ptr + 4);;
 __m512i d06 = _mm512_loadu_si512((__m512i const *) ptr + 5);;
 __m512i d07 = _mm512_loadu_si512((__m512i const *) ptr + 6);;
 __m512i d08 = _mm512_loadu_si512((__m512i const *) ptr + 7);;
 __m512i d09 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
   mask,
   (uint32_t const *) ((__m512i const *) ptr + 8));
 sort_09v_ascendingu4(&d01, &d02, &d03, &d04, &d05, &d06, &d07, &d08, &d09);
 write2(o,d01,base);
 write2(o+ 16,d02,base);
 write2(o+ 32,d03,base);
 write2(o+ 48,d04,base);
 write2(o+ 64,d05,base);
 write2(o+ 80,d06,base);
 write2(o+ 96,d07,base);
 write2(o+112,d08,base);
 write2m(o+128,d09,base,w0,w1);
}

static NOINLINE void sort_10v_altu4e(uint32_t *ptr, I *o,V base,M2 mask,M1 w0,M1 w1) {
 __m512i d01 = _mm512_loadu_si512((__m512i const *) ptr + 0);;
 __m512i d02 = _mm512_loadu_si512((__m512i const *) ptr + 1);;
 __m512i d03 = _mm512_loadu_si512((__m512i const *) ptr + 2);;
 __m512i d04 = _mm512_loadu_si512((__m512i const *) ptr + 3);;
 __m512i d05 = _mm512_loadu_si512((__m512i const *) ptr + 4);;
 __m512i d06 = _mm512_loadu_si512((__m512i const *) ptr + 5);;
 __m512i d07 = _mm512_loadu_si512((__m512i const *) ptr + 6);;
 __m512i d08 = _mm512_loadu_si512((__m512i const *) ptr + 7);;
 __m512i d09 = _mm512_loadu_si512((__m512i const *) ptr + 8);;
 __m512i d10 = _mm512_mask_loadu_epi32(_mm512_set1_epi32(4294967295u),
   mask,
   (uint32_t const *) ((__m512i const *) ptr + 9));
 sort_10v_ascendingu4(&d01, &d02, &d03, &d04, &d05, &d06, &d07, &d08, &d09, &d10);
 write2(o,d01,base);
 write2(o+ 16,d02,base);
 write2(o+ 32,d03,base);
 write2(o+ 48,d04,base);
 write2(o+ 64,d05,base);
 write2(o+ 80,d06,base);
 write2(o+ 96,d07,base);
 write2(o+112,d08,base);
 write2(o+128,d09,base);
 write2m(o+144,d10,base,w0,w1);
}

static void sortns8(I *d,I *s,I n){
 M1 m=BZHI(0xff,1+(7&(n-1)));
 switch((n+7)>>3){
  case  0:                      break;
  case  1: sort_01v_alts8(d,s,m); break;
  case  2: sort_02v_alts8(d,s,m); break;
  case  3: sort_03v_alts8(d,s,m); break;
  case  4: sort_04v_alts8(d,s,m); break;
  case  5: sort_05v_alts8(d,s,m); break;
  case  6: sort_06v_alts8(d,s,m); break;
  case  7: sort_07v_alts8(d,s,m); break;
  case  8: sort_08v_alts8(d,s,m); break;
  case  9: sort_09v_alts8(d,s,m); break;
  case 10: sort_10v_alts8(d,s,m); break;
  default: __builtin_unreachable();
 }
}
static void sortnu4(UI4 *p,I n){
 M2 m=BZHI(0xffff,1+(15&(n-1)));
 switch((n+15)>>4){
  case 0:                      break;
  case 1: sort_01v_altu4(p,m); break;
  case 2: sort_02v_altu4(p,m); break;
  case 3: sort_03v_altu4(p,m); break;
  case 4: sort_04v_altu4(p,m); break;
  case 5: sort_05v_altu4(p,m); break;
  case 6: sort_06v_altu4(p,m); break;
  case 7: sort_07v_altu4(p,m); break;
  case 8: sort_08v_altu4(p,m); break;
  default: __builtin_unreachable();
 }
}
static void sortnu4e(UI4 *p,I n,I bases,I *o){
 V base=VBC8(bases);
 M2 rm=BZHI(0xffff,1+(15&(n-1)));
 M1 wm0=rm,wm1=rm>>8;
 switch((n+15)>>4){
  case  0:                                       break;
  case  1: sort_01v_altu4e(p,o,base,rm,wm0,wm1); break;
  case  2: sort_02v_altu4e(p,o,base,rm,wm0,wm1); break;
  case  3: sort_03v_altu4e(p,o,base,rm,wm0,wm1); break;
  case  4: sort_04v_altu4e(p,o,base,rm,wm0,wm1); break;
  case  5: sort_05v_altu4e(p,o,base,rm,wm0,wm1); break;
  case  6: sort_06v_altu4e(p,o,base,rm,wm0,wm1); break;
  case  7: sort_07v_altu4e(p,o,base,rm,wm0,wm1); break;
  case  8: sort_08v_altu4e(p,o,base,rm,wm0,wm1); break;
  case  9: sort_09v_altu4e(p,o,base,rm,wm0,wm1); break;
  case 10: sort_10v_altu4e(p,o,base,rm,wm0,wm1); break;
  default: __builtin_unreachable();
 }
}
