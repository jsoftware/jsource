#if defined(__aarch64__)

#include <stdint.h>
#include <string.h>
#if defined(__ARM_NEON) || defined(_WIN32)
#include <arm_neon.h>
#endif
/* GCC and LLVM Clang, but not Apple Clang */
# if defined(__GNUC__) && !defined(__apple_build_version__)
#  if defined(__ARM_ACLE) || defined(__ARM_FEATURE_CRYPTO)
#   include <arm_acle.h>
#  endif
# endif

#include "j.h"
#include "aes-arm_table.h"

#define AES_ENCRYPT     1
#define AES_DECRYPT     0

#define BLOCK_SIZE 16

#define AES_RKSIZE      272

typedef struct {
#ifdef _WIN32
  __declspec(align(16)) uint8_t  rk[AES_RKSIZE];
#else
  uint8_t __attribute__ ((aligned (16))) rk[AES_RKSIZE];
#endif
  uint8_t Nk; //For this standard, Nk = 4, 6, or 8. (Also see Sec. 6.3.)
  uint8_t Nr; //For this standard, Nr = 10, 12, or 14. (Also see Sec. 6.3.)
} block_state;

#define  Nb  4u
#define  Nbb  Nb*4

#ifndef GET_UINT32_LE
#define GET_UINT32_LE(n,b,i)                            \
{                                                       \
    (n) = ( (uint32_t) (b)[(i)    ]       )             \
        | ( (uint32_t) (b)[(i) + 1] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 2] << 16 )             \
        | ( (uint32_t) (b)[(i) + 3] << 24 );            \
}
#endif

static void aes_setkey_enc(block_state* self, const uint8_t *key, int keyn)
{
  unsigned int i;
  uint32_t *RK;

// keyn        16  24  32
// AES_KEYSIZE 128 192 256  *&8     << 3
// Nk          4   6   8    %&      >> 2
// Nr          10  12  14   6+%&4   6 + >>2

  uint32_t AES_KEYSIZE = keyn << 3;
  self->Nk = keyn >> 2;
  self->Nr = (self->Nk + 6);

  RK = (uint32_t *) self->rk;

  for( i = 0; i < (self->Nk); i++ ) {
    GET_UINT32_LE( RK[i], key, i << 2 );
  }

  switch( self->Nr ) {
  case 10:

    for( i = 0; i < 10; i++, RK += 4 ) {
      RK[4]  = RK[0] ^ RCON[i] ^
               ( (uint32_t) FSb[ ( RK[3] >>  8 ) & 0xFF ]       ) ^
               ( (uint32_t) FSb[ ( RK[3] >> 16 ) & 0xFF ] <<  8 ) ^
               ( (uint32_t) FSb[ ( RK[3] >> 24 ) & 0xFF ] << 16 ) ^
               ( (uint32_t) FSb[ ( RK[3]       ) & 0xFF ] << 24 );

      RK[5]  = RK[1] ^ RK[4];
      RK[6]  = RK[2] ^ RK[5];
      RK[7]  = RK[3] ^ RK[6];
    }
    break;

  case 12:

    for( i = 0; i < 8; i++, RK += 6 ) {
      RK[6]  = RK[0] ^ RCON[i] ^
               ( (uint32_t) FSb[ ( RK[5] >>  8 ) & 0xFF ]       ) ^
               ( (uint32_t) FSb[ ( RK[5] >> 16 ) & 0xFF ] <<  8 ) ^
               ( (uint32_t) FSb[ ( RK[5] >> 24 ) & 0xFF ] << 16 ) ^
               ( (uint32_t) FSb[ ( RK[5]       ) & 0xFF ] << 24 );

      RK[7]  = RK[1] ^ RK[6];
      RK[8]  = RK[2] ^ RK[7];
      RK[9]  = RK[3] ^ RK[8];
      RK[10] = RK[4] ^ RK[9];
      RK[11] = RK[5] ^ RK[10];
    }
    break;

  case 14:

    for( i = 0; i < 7; i++, RK += 8 ) {
      RK[8]  = RK[0] ^ RCON[i] ^
               ( (uint32_t) FSb[ ( RK[7] >>  8 ) & 0xFF ]       ) ^
               ( (uint32_t) FSb[ ( RK[7] >> 16 ) & 0xFF ] <<  8 ) ^
               ( (uint32_t) FSb[ ( RK[7] >> 24 ) & 0xFF ] << 16 ) ^
               ( (uint32_t) FSb[ ( RK[7]       ) & 0xFF ] << 24 );

      RK[9]  = RK[1] ^ RK[8];
      RK[10] = RK[2] ^ RK[9];
      RK[11] = RK[3] ^ RK[10];

      RK[12] = RK[4] ^
               ( (uint32_t) FSb[ ( RK[11]       ) & 0xFF ]       ) ^
               ( (uint32_t) FSb[ ( RK[11] >>  8 ) & 0xFF ] <<  8 ) ^
               ( (uint32_t) FSb[ ( RK[11] >> 16 ) & 0xFF ] << 16 ) ^
               ( (uint32_t) FSb[ ( RK[11] >> 24 ) & 0xFF ] << 24 );

      RK[13] = RK[5] ^ RK[12];
      RK[14] = RK[6] ^ RK[13];
      RK[15] = RK[7] ^ RK[14];
    }
    break;
  }
}

static void aes_inverse_key(block_state* self, const uint8_t *fwdkey)
{
  int i, j;
  uint32_t *RK;
  uint32_t *SK;

#if (defined(__clang__) && ( (__clang_major__ > 3) || ((__clang_major__ == 3) && ((__clang_minor__ > 5) || !(defined(__aarch32__)||defined(__arm__)||defined(__aarch64__)) )))) || __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 6))
  fwdkey = (const uint8_t *) __builtin_assume_aligned (fwdkey, 16);
#endif
  RK = (uint32_t *) self->rk;
  SK = ((uint32_t *) fwdkey) + self->Nr * 4;

  *RK++ = *SK++;
  *RK++ = *SK++;
  *RK++ = *SK++;
  *RK++ = *SK++;

  for( i = self->Nr - 1, SK -= 8; i > 0; i--, SK -= 8 ) {
    for( j = 0; j < 4; j++, SK++ ) {
      *RK++ = RT0[ FSb[ ( *SK       ) & 0xFF ] ] ^
              RT1[ FSb[ ( *SK >>  8 ) & 0xFF ] ] ^
              RT2[ FSb[ ( *SK >> 16 ) & 0xFF ] ] ^
              RT3[ FSb[ ( *SK >> 24 ) & 0xFF ] ];
    }
  }

  *RK++ = *SK++;
  *RK++ = *SK++;
  *RK++ = *SK++;
  *RK++ = *SK++;
}

/*
 * ARMv8a AES-ECB block en(de)cryption
 */
static void aes_crypt_ecb( block_state* self,
                           int mode,
                           const unsigned char input[16],
                           unsigned char output[16] )
{
  int i;
  uint8x16_t state_vec, roundkey_vec;
  uint8_t *RK = (uint8_t *) self->rk;

  // Load input and round key into into their vectors
  state_vec = vld1q_u8( input );

  if ( mode == AES_ENCRYPT ) {
    // Initial AddRoundKey is in the loop due to AES instruction always doing AddRoundKey first
    for( i = 0; i < self->Nr - 1; i++ ) {
      // Load Round Key
      roundkey_vec = vld1q_u8( RK );
      // Forward (AESE) round (AddRoundKey, SubBytes and ShiftRows)
      state_vec = vaeseq_u8( state_vec, roundkey_vec );
      // Mix Columns (AESMC)
      state_vec = vaesmcq_u8( state_vec );
      // Move pointer ready to load next round key
      RK += 16;
    }

    // Final Forward (AESE) round (AddRoundKey, SubBytes and ShiftRows). No Mix columns
    roundkey_vec = vld1q_u8( RK ); /* RK already moved in loop */
    state_vec = vaeseq_u8( state_vec, roundkey_vec );
  } else {
    // Initial AddRoundKey is in the loop due to AES instruction always doing AddRoundKey first
    for( i = 0; i < self->Nr - 1; i++ ) {
      // Load Round Key
      roundkey_vec = vld1q_u8( RK );
      // Reverse (AESD) round (AddRoundKey, SubBytes and ShiftRows)
      state_vec = vaesdq_u8( state_vec, roundkey_vec );
      // Inverse Mix Columns (AESIMC)
      state_vec = vaesimcq_u8( state_vec );
      // Move pointer ready to load next round key
      RK += 16;
    }

    // Final Reverse (AESD) round (AddRoundKey, SubBytes and ShiftRows). No Mix columns
    roundkey_vec = vld1q_u8( RK ); /* RK already moved in loop */
    state_vec = vaesdq_u8( state_vec, roundkey_vec );
  }

  // Manually apply final Add RoundKey step (EOR)
  RK += 16;
  roundkey_vec = vld1q_u8( RK );
  state_vec = veorq_u8( state_vec, roundkey_vec );

  // Write results back to output array
  vst1q_u8( output, state_vec );
}

#define block_init aes_setkey_enc
#define block_encrypt(self, input, output) aes_crypt_ecb(self, AES_ENCRYPT, input, output)
#define block_decrypt(self, input, output) aes_crypt_ecb(self, AES_DECRYPT, input, output)

static void block_finalize(block_state* self)
{
}

/*
  mode
  0    ECB
  1    CBC
  2    CTR
 */
// iv must be 16-byte wide
// out buffer of n bytes and n must be 16-byte block
// out buffer will be overwritten
int aes_arm(I decrypt,I mode,UC *key,I keyn,UC* ivec,UC* out,I len)
{
  block_state self;
#ifdef _WIN32
  __declspec(align(16)) uint8_t rk_tmp[AES_RKSIZE];
#else
  uint8_t __attribute__ ((aligned (16))) rk_tmp[AES_RKSIZE];
#endif
  uint8_t *str=out;
  I i;

  switch(mode) {
  case 0:
    block_init(&self, key, (int)keyn);
    if(decrypt) {
      memcpy(rk_tmp, self.rk, AES_RKSIZE);
      aes_inverse_key(&self, rk_tmp);
      for(i=0; i<len; i+=BLOCK_SIZE) block_decrypt(&self, str+i,out+i);
    } else {
      for(i=0; i<len; i+=BLOCK_SIZE) block_encrypt(&self, str+i,out+i);
    }
    block_finalize(&self);
    break;

  case 1:
    block_init(&self, key, (int)keyn);
    if(decrypt) {
      memcpy(rk_tmp, self.rk, AES_RKSIZE);
      aes_inverse_key(&self, rk_tmp);
      uint8x16_t iv, temp, storeNextIv;
      iv = vld1q_u8(ivec);
      for(i=0; i<len; i+=BLOCK_SIZE) {
        storeNextIv = vld1q_u8((str+i));
        block_decrypt(&self, str+i, (uint8_t*)&temp);
        temp = veorq_u8(temp, iv);
        vst1q_u8((out+i), temp);
        iv = storeNextIv;
      }
    } else {
      uint8x16_t iv, temp;
      iv = vld1q_u8(ivec);
      for(i=0; i<len; i+=BLOCK_SIZE) {
        temp = vld1q_u8((str+i));
        temp = veorq_u8(temp, iv);
        block_encrypt(&self, (uint8_t*)&temp, out+i);
        iv = vld1q_u8((out+i));
      }
    }
    block_finalize(&self);
    break;

  case 2: {
    uint8_t iv[BLOCK_SIZE];
    uint8_t buffer[BLOCK_SIZE];

    block_init(&self, key, (int)keyn);
    memcpy(iv, ivec, BLOCK_SIZE);

    uintptr_t i;
    int bi;
    for (i = 0, bi = BLOCK_SIZE; i < (uintptr_t)len; ++i, ++bi) {
      if (bi == BLOCK_SIZE) { /* we need to regen xor compliment in buffer */

        memcpy(buffer, iv, BLOCK_SIZE);
        block_encrypt(&self, buffer,buffer);
        /* Increment Iv and handle overflow */
        for (bi = (BLOCK_SIZE - 1); bi >= 0; --bi) {
          /* inc will overflow */
          if (iv[bi] == 255) {
            iv[bi] = 0;
            continue;
          }
          iv[bi] += 1;
          break;
        }
        bi = 0;
      }
      out[i] = (out[i] ^ buffer[bi]);
    }
  }
  block_finalize(&self);
  break;

  default:
    R 1;

  }

  R 0;  // success
}

#endif

