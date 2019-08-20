/* sha1-arm.c - ARMv8 SHA extensions using C intrinsics       */
/*   Written and placed in public domain by Jeffrey Walton    */
/*   Based on code from ARM, and by Johannes Schneiders, Skip */
/*   Hovsmith and Barry O'Rourke for the mbedTLS project.     */

/* For some reason we need to use the C++ compiler. Otherwise   */
/* all the intrinsics functions, like vsha1h_u32, are missing.  */
/* GCC118 on the compile farm with GCC 4.8.5 suffers the issue. */
/* g++ -DTEST_MAIN -march=armv8-a+crypto sha1-arm.c -o sha1.exe */

#if defined(__aarch64__)

/* Visual Studio 2017 and above supports ARMv8, but its not clear how to detect */
/* it or use it at the moment. Also see http://stackoverflow.com/q/37244202,    */
/* http://stackoverflow.com/q/41646026, and http://stackoverflow.com/q/41688101 */
#if defined(__arm__) || defined(__aarch32__) || defined(__arm64__) || defined(__aarch64__) || defined(_M_ARM)
# if defined(__GNUC__)
#  include <stdint.h>
# endif
# if defined(__ARM_NEON)|| defined(_MSC_VER) || defined(__GNUC__)
#  include <arm_neon.h>
# endif
/* GCC and LLVM Clang, but not Apple Clang */
# if defined(__GNUC__) && !defined(__apple_build_version__)
#  if defined(__ARM_ACLE) || defined(__ARM_FEATURE_CRYPTO)
#   include <arm_acle.h>
#  endif
# endif
#endif  /* ARM Headers */

/* Process multiple blocks. The caller is responsible for setting the initial */
/*  state, and the caller is responsible for padding the final block.        */
void sha1_process_arm(uint32_t state[5], const uint8_t data[], uintptr_t length)
{
#ifdef _MSC_VER
  __declspec(align(16)) uint32x4_t ABCD, ABCD_SAVED;
  __declspec(align(16)) uint32x4_t TMP0, TMP1;
  __declspec(align(16)) uint32x4_t MSG0, MSG1, MSG2, MSG3;
#else
  uint32x4_t __attribute__ ((aligned (16))) ABCD, ABCD_SAVED;
  uint32x4_t __attribute__ ((aligned (16))) TMP0, TMP1;
  uint32x4_t __attribute__ ((aligned (16))) MSG0, MSG1, MSG2, MSG3;
#endif
  uint32_t   E0, E0_SAVED, E1;

  /* Load state */
  ABCD = vld1q_u32(&state[0]);
  E0 = state[4];

  while (length >= 64) {
    /* Save state */
    ABCD_SAVED = ABCD;
    E0_SAVED = E0;

    /* Load message */
    MSG0 = vld1q_u32((const uint32_t*)(data));
    MSG1 = vld1q_u32((const uint32_t*)(data + 16));
    MSG2 = vld1q_u32((const uint32_t*)(data + 32));
    MSG3 = vld1q_u32((const uint32_t*)(data + 48));

    /* Reverse for little endian */
    MSG0 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG0)));
    MSG1 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG1)));
    MSG2 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG2)));
    MSG3 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(MSG3)));

    TMP0 = vaddq_u32(MSG0, vdupq_n_u32(0x5A827999));
    TMP1 = vaddq_u32(MSG1, vdupq_n_u32(0x5A827999));

    /* Rounds 0-3 */
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1cq_u32(ABCD, E0, TMP0);
    TMP0 = vaddq_u32(MSG2, vdupq_n_u32(0x5A827999));
    MSG0 = vsha1su0q_u32(MSG0, MSG1, MSG2);

    /* Rounds 4-7 */
    E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1cq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG3, vdupq_n_u32(0x5A827999));
    MSG0 = vsha1su1q_u32(MSG0, MSG3);
    MSG1 = vsha1su0q_u32(MSG1, MSG2, MSG3);

    /* Rounds 8-11 */
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1cq_u32(ABCD, E0, TMP0);
    TMP0 = vaddq_u32(MSG0, vdupq_n_u32(0x5A827999));
    MSG1 = vsha1su1q_u32(MSG1, MSG0);
    MSG2 = vsha1su0q_u32(MSG2, MSG3, MSG0);

    /* Rounds 12-15 */
    E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1cq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG1, vdupq_n_u32(0x6ED9EBA1));
    MSG2 = vsha1su1q_u32(MSG2, MSG1);
    MSG3 = vsha1su0q_u32(MSG3, MSG0, MSG1);

    /* Rounds 16-19 */
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1cq_u32(ABCD, E0, TMP0);
    TMP0 = vaddq_u32(MSG2, vdupq_n_u32(0x6ED9EBA1));
    MSG3 = vsha1su1q_u32(MSG3, MSG2);
    MSG0 = vsha1su0q_u32(MSG0, MSG1, MSG2);

    /* Rounds 20-23 */
    E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG3, vdupq_n_u32(0x6ED9EBA1));
    MSG0 = vsha1su1q_u32(MSG0, MSG3);
    MSG1 = vsha1su0q_u32(MSG1, MSG2, MSG3);

    /* Rounds 24-27 */
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E0, TMP0);
    TMP0 = vaddq_u32(MSG0, vdupq_n_u32(0x6ED9EBA1));
    MSG1 = vsha1su1q_u32(MSG1, MSG0);
    MSG2 = vsha1su0q_u32(MSG2, MSG3, MSG0);

    /* Rounds 28-31 */
    E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG1, vdupq_n_u32(0x6ED9EBA1));
    MSG2 = vsha1su1q_u32(MSG2, MSG1);
    MSG3 = vsha1su0q_u32(MSG3, MSG0, MSG1);

    /* Rounds 32-35 */
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E0, TMP0);
    TMP0 = vaddq_u32(MSG2, vdupq_n_u32(0x8F1BBCDC));
    MSG3 = vsha1su1q_u32(MSG3, MSG2);
    MSG0 = vsha1su0q_u32(MSG0, MSG1, MSG2);

    /* Rounds 36-39 */
    E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG3, vdupq_n_u32(0x8F1BBCDC));
    MSG0 = vsha1su1q_u32(MSG0, MSG3);
    MSG1 = vsha1su0q_u32(MSG1, MSG2, MSG3);

    /* Rounds 40-43 */
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1mq_u32(ABCD, E0, TMP0);
    TMP0 = vaddq_u32(MSG0, vdupq_n_u32(0x8F1BBCDC));
    MSG1 = vsha1su1q_u32(MSG1, MSG0);
    MSG2 = vsha1su0q_u32(MSG2, MSG3, MSG0);

    /* Rounds 44-47 */
    E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1mq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG1, vdupq_n_u32(0x8F1BBCDC));
    MSG2 = vsha1su1q_u32(MSG2, MSG1);
    MSG3 = vsha1su0q_u32(MSG3, MSG0, MSG1);

    /* Rounds 48-51 */
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1mq_u32(ABCD, E0, TMP0);
    TMP0 = vaddq_u32(MSG2, vdupq_n_u32(0x8F1BBCDC));
    MSG3 = vsha1su1q_u32(MSG3, MSG2);
    MSG0 = vsha1su0q_u32(MSG0, MSG1, MSG2);

    /* Rounds 52-55 */
    E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1mq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG3, vdupq_n_u32(0xCA62C1D6));
    MSG0 = vsha1su1q_u32(MSG0, MSG3);
    MSG1 = vsha1su0q_u32(MSG1, MSG2, MSG3);

    /* Rounds 56-59 */
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1mq_u32(ABCD, E0, TMP0);
    TMP0 = vaddq_u32(MSG0, vdupq_n_u32(0xCA62C1D6));
    MSG1 = vsha1su1q_u32(MSG1, MSG0);
    MSG2 = vsha1su0q_u32(MSG2, MSG3, MSG0);

    /* Rounds 60-63 */
    E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG1, vdupq_n_u32(0xCA62C1D6));
    MSG2 = vsha1su1q_u32(MSG2, MSG1);
    MSG3 = vsha1su0q_u32(MSG3, MSG0, MSG1);

    /* Rounds 64-67 */
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E0, TMP0);
    TMP0 = vaddq_u32(MSG2, vdupq_n_u32(0xCA62C1D6));
    MSG3 = vsha1su1q_u32(MSG3, MSG2);
    MSG0 = vsha1su0q_u32(MSG0, MSG1, MSG2);

    /* Rounds 68-71 */
    E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E1, TMP1);
    TMP1 = vaddq_u32(MSG3, vdupq_n_u32(0xCA62C1D6));
    MSG0 = vsha1su1q_u32(MSG0, MSG3);

    /* Rounds 72-75 */
    E1 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E0, TMP0);

    /* Rounds 76-79 */
    E0 = vsha1h_u32(vgetq_lane_u32(ABCD, 0));
    ABCD = vsha1pq_u32(ABCD, E1, TMP1);

    /* Combine state */
    E0 += E0_SAVED;
    ABCD = vaddq_u32(ABCD_SAVED, ABCD);

    data += 64;
    length -= 64;
  }

  /* Save state */
  vst1q_u32(&state[0], ABCD);
  state[4] = E0;
}

#if defined(TEST_MAIN)

#include <stdio.h>
#include <string.h>
int main(int argc, char* argv[])
{
  /* empty message with padding */
  uint8_t message[64];
  memset(message, 0x00, sizeof(message));
  message[0] = 0x80;

  /* initial state */
  uint32_t state[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};

  sha1_process_arm(state, message, sizeof(message));

  const uint8_t b1 = (uint8_t)(state[0] >> 24);
  const uint8_t b2 = (uint8_t)(state[0] >> 16);
  const uint8_t b3 = (uint8_t)(state[0] >>  8);
  const uint8_t b4 = (uint8_t)(state[0] >>  0);
  const uint8_t b5 = (uint8_t)(state[1] >> 24);
  const uint8_t b6 = (uint8_t)(state[1] >> 16);
  const uint8_t b7 = (uint8_t)(state[1] >>  8);
  const uint8_t b8 = (uint8_t)(state[1] >>  0);

  /* DA39A3EE5E6B4B0D... */
  printf("SHA1 hash of empty message: ");
  printf("%02X%02X%02X%02X%02X%02X%02X%02X...\n",
         b1, b2, b3, b4, b5, b6, b7, b8);

  int success = ((b1 == 0xDA) && (b2 == 0x39) && (b3 == 0xA3) && (b4 == 0xEE) &&
                 (b5 == 0x5E) && (b6 == 0x6B) && (b7 == 0x4B) && (b8 == 0x0D));

  if (success)
    printf("Success!\n");
  else
    printf("Failure!\n");

  return (success != 0 ? 0 : 1);
}

#endif

#endif // defined(__aarch64__)
