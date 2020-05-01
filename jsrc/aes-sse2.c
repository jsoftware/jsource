#include "j.h"

#if defined(__SSE2__)

#include <stdint.h>
#include <emmintrin.h>

#define BLOCK_SIZE 16

#define ADD _mm_add_epi32
#define XOR _mm_xor_si128

typedef struct {
#ifdef MMSC_VER
  __declspec(align(16)) uint32_t w[60]; //Key Schedule (16byte align)
#else
  uint32_t __attribute__ ((aligned (16))) w[60]; //Key Schedule (16byte align)
#endif
  uint8_t Nk; //For this standard, Nk = 4, 6, or 8. (Also see Sec. 6.3.)
  uint8_t Nr; //For this standard, Nr = 10, 12, or 14. (Also see Sec. 6.3.)
} block_state;

#define  Nb  4u
#define  Nbb  Nb*4

#ifndef MMSC_VER
typedef union __attribute__ ((aligned (16))) s_m128i {
  signed char m128i_i8[16];
  int16_t m128i_i16[8];
  int32_t m128i_i32[4];
  int64_t m128i_i64[2];
  uint8_t m128i_u8[16];
  uint16_t m128i_u16[8];
  uint32_t m128i_u32[4];
  uint64_t m128i_u64[2];
} s_m128i;

#endif

static void block_init(block_state* self, const uint8_t Key[], int cNk);

static __m128i mul(__m128i data, uint8_t n);     //4,2 Multiplication

static void KeyExpansion(block_state* self, char cNK, const uint8_t *key); //5.2 Key Expansion
static uint32_t  RotWord(uint32_t data);    //
static uint32_t  SubWord(uint32_t data);    //
static uint32_t  InvSubWord(uint32_t data);   //

static void Cipher_One(block_state* self, void *ind, void *outd);     //
static __m128i Cipher(block_state* self, __m128i data);        //5.1 Cipher
static __m128i SubBytes(__m128i data);        //5.1.1 SubBytes
static __m128i ShiftRows(__m128i data);       //5.1.2 ShiftRows
static __m128i MixColumns(__m128i data);       //5.1.3 MixColumns
static __m128i AddRoundKey(block_state* self, __m128i data, int i);     //5.1.4 AddRoundKey

static void InvCipher_One(block_state* self, void *ind, void *outd);    //
static __m128i InvCipher(block_state* self, __m128i data);       //5.3 InvCipher
static __m128i InvShiftRows(__m128i data);       //5.3.1 InvShiftRows
static __m128i InvSubBytes(__m128i data);       //5.3.2 InvSubBytes
static __m128i InvMixColumns(__m128i data);      //5.3.3 InvMixColumns
static __m128i InvAddRoundKey(block_state* self, __m128i data, int i);    //5.3.4 InvAddRoundKey

//==============================================================
//   constant
//--------------------------------------------------------------
// input parameters
//   char     cNK  = 4 : 128bit AES
//           = 6 : 192bit AES
//           = 8 : 256bit AES
//   const uint8_t Key[]  : Cipher Key
// return value
//   none
//==============================================================
void block_init(block_state* self, const uint8_t Key[], int cNk)
{
  KeyExpansion(self, (char)(cNk/4), Key);
}
//==============================================================
//   fips-197 4.2  Multiplication
//--------------------------------------------------------------
// input parameters
//   *in[16]  Plain-text
//   *out[16] Cipher-text
// return value
//   out[16]  Cipher-text
//==============================================================
__m128i mul(__m128i data, uint8_t n)
{
#ifdef MMSC_VER
  __declspec(align(16)) static const unsigned __int16 _FF00[8] = {0xFF00,0xFF00,0xFF00,0xFF00, 0xFF00,0xFF00,0xFF00,0xFF00};
  __declspec(align(16)) static const unsigned __int16 _00FF[8] = {0x00FF,0x00FF,0x00FF,0x00FF, 0x00FF,0x00FF,0x00FF,0x00FF};
  __declspec(align(16)) static const unsigned __int16 _011B[8] = {0x011B,0x011B,0x011B,0x011B, 0x011B,0x011B,0x011B,0x011B};
#else
  static const uint16_t __attribute__ ((aligned (16))) _FF00[8] = {0xFF00,0xFF00,0xFF00,0xFF00, 0xFF00,0xFF00,0xFF00,0xFF00};
  static const uint16_t __attribute__ ((aligned (16))) _00FF[8] = {0x00FF,0x00FF,0x00FF,0x00FF, 0x00FF,0x00FF,0x00FF,0x00FF};
  static const uint16_t __attribute__ ((aligned (16))) _011B[8] = {0x011B,0x011B,0x011B,0x011B, 0x011B,0x011B,0x011B,0x011B};
#endif

  const __m128i _mm_FF00 = _mm_load_si128((__m128i*)&_FF00);
  const __m128i _mm_00FF = _mm_load_si128((__m128i*)&_00FF);
  const __m128i _mm_011B = _mm_load_si128((__m128i*)&_011B);

  __m128i data1 = _mm_srli_epi16( _mm_and_si128(data, _mm_FF00), 8);
  __m128i data2 =     _mm_and_si128(data, _mm_00FF);

  __m128i result1 = _mm_setzero_si128();
  __m128i result2 = _mm_setzero_si128();

  uint8_t i = 0x08;

  do {
    result1 = _mm_slli_epi16(result1, 1);
    result2 = _mm_slli_epi16(result2, 1);

    result1 = _mm_xor_si128(result1, _mm_and_si128(_mm_cmpgt_epi16(result1, _mm_00FF), _mm_011B));
    result2 = _mm_xor_si128(result2, _mm_and_si128(_mm_cmpgt_epi16(result2, _mm_00FF), _mm_011B));

    if(i & n) {
      result1 = _mm_xor_si128(result1, data1);
      result2 = _mm_xor_si128(result2, data2);
    }
    i >>= 1;
  } while (i>0);

  return(_mm_or_si128(_mm_slli_epi16(result1, 8), result2));
}
//==============================================================
//   fips-197 5.2  Key Expansion
//--------------------------------------------------------------
// input parameters
//   char    cNK  = 4(0100) : 128bit AES
//          = 6(0110) : 192bit AES
//          = 8(1000) : 256bit AES
//   const uint8_t Key[]    : Cipher Key
// return value
//   none
//==============================================================
void KeyExpansion(block_state* self, char cNk, const uint8_t *key)
{
//The round constant word array.
  static const uint32_t Rcon[10]= {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36};

  uint32_t i;
  int iR;
  int iRm;
  int temp;

  self->Nk = cNk & 0xFE;
  self->Nr = self->Nk + 6;

//high speed memory copy
  i = 0;
  do {
    _mm_storel_epi64((__m128i*)&self->w[i], _mm_loadl_epi64((__m128i*)&key[i*4]));
    i++;
    i++;
  } while (i < self->Nk);

  i  = self->Nk;
  temp = self->w[i-1];
  do {
    iR = i/self->Nk;
    iRm = i%self->Nk;

    if(iRm==0) {
      temp = SubWord(RotWord(temp)) ^ Rcon[(iR)-1];
    } else if((self->Nk > 6) && ( (i%self->Nk) == 4)) {
      temp = SubWord(temp);
    }
    temp ^= self->w[i-self->Nk];
    self->w[i]  = temp;

    i++;

  } while (i < Nb * (self->Nr+1));
}
//==============================================================
//   fips-197 5.2  RotWord()
//--------------------------------------------------------------
// input parameters
//   uint32_t data 4 Byte
// return value
//   uint32_t  ror data,8
//==============================================================
uint32_t RotWord(uint32_t data)
{
  return( (data>>8) | (data<<24) );  // ror data,8
}
//==============================================================
//   fips-197 5.2  Sbox() (4 Byte)
//--------------------------------------------------------------
// input parameters
//   uint32_t data 4 Byte
// return value
//   uint32_t   SBox
//==============================================================
uint32_t SubWord(uint32_t data)
{
// fips-197 Figure 7
  static const uint8_t Sbox[256]= {
    0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
    0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
    0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
    0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
    0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
    0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
    0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
    0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
    0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
    0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
    0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
    0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
    0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
    0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
    0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
    0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
  };

// table transform SIMD command unavailable ?
  uint8_t a0 = Sbox[(data    ) & 0xFF];
  uint8_t a1 = Sbox[(data>>8 ) & 0xFF];
  uint8_t a2 = Sbox[(data>>16) & 0xFF];
  uint8_t a3 = Sbox[(data>>24)];

  return(a0 | a1<<8 | a2<<16 | a3<<24);
}
//==============================================================
//   fips-197 5.2  InvSbox() (4 Byte)
//--------------------------------------------------------------
// input parameters
//   uint32_t data 4 Byte
// return value
//   uint32_t   SBox
//==============================================================
uint32_t InvSubWord(uint32_t data)
{
// fips-197 Figure 14
  static const uint8_t InvSbox[256]= {
    0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
    0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
    0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
    0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
    0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
    0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
    0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
    0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
    0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
    0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
    0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
    0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
    0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
    0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
    0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
    0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
  };

// table transform SIMD command unavailable ?
  uint8_t a0 = InvSbox[(data    ) & 0xFF];
  uint8_t a1 = InvSbox[(data>>8 ) & 0xFF];
  uint8_t a2 = InvSbox[(data>>16) & 0xFF];
  uint8_t a3 = InvSbox[(data>>24)];

  return(a0 | a1<<8 | a2<<16 | a3<<24);
}
//==============================================================
//   fips-197
//--------------------------------------------------------------
// input parameters
//   *in[16]  Plain-text
//   *out[16] Cipher-text
// return value
//   out[16]  Cipher-text
//==============================================================
void Cipher_One(block_state* self, void *ind, void *outd)
{
  _mm_storeu_si128((__m128i*)outd, Cipher(self, _mm_loadu_si128((__m128i*)ind)));

}
//==============================================================
//   fips-197 5.1  Cipher
//--------------------------------------------------------------
// input parameters
//   __m128i data  Plain-text
// return value
//   __m128i    Cipher-text
//==============================================================
__m128i Cipher(block_state* self, __m128i data)
{
// Round [0]
  int i=0;

  data = AddRoundKey(self, data, i);
  i++;

// Round (1) to (Nr-1)
  do {
    data = AddRoundKey(self, MixColumns(ShiftRows(SubBytes(data))),i);
    i++;
  } while(i < self->Nr);

// Round (Nr)
  data = AddRoundKey(self, ShiftRows(SubBytes(data)),i);

  return(data);
}
//==============================================================
//   fips-197 5.1.1
//--------------------------------------------------------------
// input parameters
//   __m128i data  Input
// return value
//   __m128i    Output
//==============================================================
__m128i SubBytes(__m128i data)
{
// table transform SIMD command unavailable ?
#ifdef MMSC_VER
  data.m128i_u32[0] = SubWord(data.m128i_u32[0]);
  data.m128i_u32[1] = SubWord(data.m128i_u32[1]);
  data.m128i_u32[2] = SubWord(data.m128i_u32[2]);
  data.m128i_u32[3] = SubWord(data.m128i_u32[3]);
#else
  (*(s_m128i*)&data).m128i_u32[0] = SubWord((*(s_m128i*)&data).m128i_u32[0]);
  (*(s_m128i*)&data).m128i_u32[1] = SubWord((*(s_m128i*)&data).m128i_u32[1]);
  (*(s_m128i*)&data).m128i_u32[2] = SubWord((*(s_m128i*)&data).m128i_u32[2]);
  (*(s_m128i*)&data).m128i_u32[3] = SubWord((*(s_m128i*)&data).m128i_u32[3]);
#endif

  return(data);
}
//==============================================================
//   fips-197 5.1.2 ShiftRows
//--------------------------------------------------------------
// input parameters
//   __m128i data  Input
// return value
//   __m128i    Output
//==============================================================
__m128i ShiftRows(__m128i data)
{
#ifdef MMSC_VER
  __declspec(align(16)) static const uint32_t mask0[4] = {0x000000FF,0x000000FF,0x000000FF,0x000000FF};
  __declspec(align(16)) static const uint32_t mask1[4] = {0x0000FF00,0x0000FF00,0x0000FF00,0x0000FF00};
  __declspec(align(16)) static const uint32_t mask2[4] = {0x00FF0000,0x00FF0000,0x00FF0000,0x00FF0000};
  __declspec(align(16)) static const uint32_t mask3[4] = {0xFF000000,0xFF000000,0xFF000000,0xFF000000};
#else
  static const uint32_t __attribute__ ((aligned (16))) mask0[4] = {0x000000FF,0x000000FF,0x000000FF,0x000000FF};
  static const uint32_t __attribute__ ((aligned (16))) mask1[4] = {0x0000FF00,0x0000FF00,0x0000FF00,0x0000FF00};
  static const uint32_t __attribute__ ((aligned (16))) mask2[4] = {0x00FF0000,0x00FF0000,0x00FF0000,0x00FF0000};
  static const uint32_t __attribute__ ((aligned (16))) mask3[4] = {0xFF000000,0xFF000000,0xFF000000,0xFF000000};
#endif

  const __m128i _mm_mask0 = _mm_load_si128((__m128i*)&mask0);
  const __m128i _mm_mask1 = _mm_load_si128((__m128i*)&mask1);
  const __m128i _mm_mask2 = _mm_load_si128((__m128i*)&mask2);
  const __m128i _mm_mask3 = _mm_load_si128((__m128i*)&mask3);

  __m128i a0 = _mm_and_si128(_mm_mask0, data);        //rotate vertically
  __m128i a1 = _mm_and_si128(_mm_mask1, _mm_shuffle_epi32(data, 0x39)); //0011 1001 b
  __m128i a2 = _mm_and_si128(_mm_mask2, _mm_shuffle_epi32(data, 0x4E)); //0100 1110 b
  __m128i a3 = _mm_and_si128(_mm_mask3, _mm_shuffle_epi32(data, 0x93)); //1001 0011 b

  return(_mm_or_si128(_mm_or_si128(a0,a1),_mm_or_si128(a2,a3)));
}
//==============================================================
//   fips-197 5.1.3 MixColumns
//--------------------------------------------------------------
// input parameters
//   __m128i data  Input
// return value
//   __m128i    Output
//==============================================================
__m128i MixColumns(__m128i data)
{
  __m128i  a0 = mul(data, 2);
  __m128i  a1 = data;    // = mul(data, 1);
  __m128i  a2 = data;    // = mul(data, 1);
  __m128i  a3 = mul(data, 3);
  // 32-bit rotate right
  a1 = _mm_or_si128(_mm_srli_epi32(a1,24),_mm_slli_epi32(a1, 8)); // prord a1, 24
  a2 = _mm_or_si128(_mm_srli_epi32(a2,16),_mm_slli_epi32(a2,16)); // prord a2, 16
  a3 = _mm_or_si128(_mm_srli_epi32(a3, 8),_mm_slli_epi32(a3,24)); // prord a3, 8

  return(_mm_xor_si128(_mm_xor_si128(a0,a1),_mm_xor_si128(a2,a3)));
}
//==============================================================
//   fips-197 5.1.4
//--------------------------------------------------------------
// input parameters
//   __m128i data  Input
//   int  i   Round
// return value
//   __m128i    Output
//==============================================================
__m128i AddRoundKey(block_state* self, __m128i data, int i)
{
  return(_mm_xor_si128(data, _mm_load_si128((__m128i*)&(self->w[i*4]))));
}
//==============================================================
//   fips-197
//--------------------------------------------------------------
// input parameters
//   *in[16]  Plain-text
//   *out[16] Cipher-text
// return value
//   out[16]  Cipher-text
//==============================================================
void InvCipher_One(block_state* self, void *ind, void *outd)
{
  _mm_storeu_si128((__m128i*)outd, InvCipher(self, _mm_loadu_si128((__m128i*)ind)));
}
//==============================================================
//   fips-197 5.3  InvCipher
//--------------------------------------------------------------
// input parameters
//   __m128i data  Plain-text
// return value
//   __m128i    Cipher-text
//==============================================================
__m128i InvCipher(block_state* self, __m128i data)
{
// Round (Nr)
  int i=self->Nr;

  data = InvAddRoundKey(self, data, i);
  i--;

// Round (Nr-1) to (1)
  do {
    data = InvMixColumns(InvAddRoundKey(self, InvSubBytes(InvShiftRows(data)),i));
    i--;
  } while(i > 0);

// Round (0)
  data = InvAddRoundKey(self, InvSubBytes(InvShiftRows(data)), i);

  return(data);
}
//==============================================================
//   fips-197 5.3.1 InvShiftRows
//--------------------------------------------------------------
// input parameters
//   __m128i data  Input
// return value
//   __m128i    Output
//==============================================================
__m128i InvShiftRows(__m128i data)
{
#ifdef MMSC_VER
  __declspec(align(16)) static const uint32_t mask0[4] = {0x000000FF,0x000000FF,0x000000FF,0x000000FF};
  __declspec(align(16)) static const uint32_t mask1[4] = {0x0000FF00,0x0000FF00,0x0000FF00,0x0000FF00};
  __declspec(align(16)) static const uint32_t mask2[4] = {0x00FF0000,0x00FF0000,0x00FF0000,0x00FF0000};
  __declspec(align(16)) static const uint32_t mask3[4] = {0xFF000000,0xFF000000,0xFF000000,0xFF000000};
#else
  static const uint32_t __attribute__ ((aligned (16))) mask0[4] = {0x000000FF,0x000000FF,0x000000FF,0x000000FF};
  static const uint32_t __attribute__ ((aligned (16))) mask1[4] = {0x0000FF00,0x0000FF00,0x0000FF00,0x0000FF00};
  static const uint32_t __attribute__ ((aligned (16))) mask2[4] = {0x00FF0000,0x00FF0000,0x00FF0000,0x00FF0000};
  static const uint32_t __attribute__ ((aligned (16))) mask3[4] = {0xFF000000,0xFF000000,0xFF000000,0xFF000000};
#endif

  const __m128i _mm_mask0 = _mm_load_si128((__m128i*)&mask0);
  const __m128i _mm_mask1 = _mm_load_si128((__m128i*)&mask1);
  const __m128i _mm_mask2 = _mm_load_si128((__m128i*)&mask2);
  const __m128i _mm_mask3 = _mm_load_si128((__m128i*)&mask3);

  __m128i  a0 = _mm_and_si128(_mm_mask0, data);       //rotate vertically
  __m128i  a1 = _mm_and_si128(_mm_mask3, _mm_shuffle_epi32(data, 0x39)); //0011 1001 b
  __m128i  a2 = _mm_and_si128(_mm_mask2, _mm_shuffle_epi32(data, 0x4E)); //0100 1110 b
  __m128i  a3 = _mm_and_si128(_mm_mask1, _mm_shuffle_epi32(data, 0x93)); //1001 0011 b

  return(_mm_or_si128(_mm_or_si128(a0,a1),_mm_or_si128(a2,a3)));
}
//==============================================================
//   fips-197 5.3.2 InvSubBytes
//--------------------------------------------------------------
// input parameters
//   __m128i data  Input
// return value
//   __m128i    Output
//==============================================================
__m128i InvSubBytes(__m128i data)
{
// table transform SIMD command unavailable ?
#ifdef MMSC_VER
  data.m128i_u32[0] = InvSubWord(data.m128i_u32[0]);
  data.m128i_u32[1] = InvSubWord(data.m128i_u32[1]);
  data.m128i_u32[2] = InvSubWord(data.m128i_u32[2]);
  data.m128i_u32[3] = InvSubWord(data.m128i_u32[3]);
#else
  (*(s_m128i*)&data).m128i_u32[0] = InvSubWord((*(s_m128i*)&data).m128i_u32[0]);
  (*(s_m128i*)&data).m128i_u32[1] = InvSubWord((*(s_m128i*)&data).m128i_u32[1]);
  (*(s_m128i*)&data).m128i_u32[2] = InvSubWord((*(s_m128i*)&data).m128i_u32[2]);
  (*(s_m128i*)&data).m128i_u32[3] = InvSubWord((*(s_m128i*)&data).m128i_u32[3]);
#endif

  return(data);
}
//==============================================================
//   fips-197 5.3.3 InvMixColumns
//--------------------------------------------------------------
// input parameters
//   __m128i data  Input
// return value
//   __m128i    Output
//==============================================================
__m128i InvMixColumns(__m128i data)
{
  __m128i  a0 = mul(data, 0x0E);
  __m128i  a1 = mul(data, 0x09);
  __m128i  a2 = mul(data, 0x0D);
  __m128i  a3 = mul(data, 0x0B);
  // 32-bit rotate right
  a1 = _mm_or_si128(_mm_srli_epi32(a1,24),_mm_slli_epi32(a1, 8)); // prord a1, 24
  a2 = _mm_or_si128(_mm_srli_epi32(a2,16),_mm_slli_epi32(a2,16)); // prord a2, 16
  a3 = _mm_or_si128(_mm_srli_epi32(a3, 8),_mm_slli_epi32(a3,24)); // prord a3, 8

  return(_mm_xor_si128(_mm_xor_si128(a0,a1),_mm_xor_si128(a2,a3)));
}
//==============================================================
//   fips-197 5.3.4 InvAddRoundKey
//--------------------------------------------------------------
// input parameters
//   __m128i data  Input
//   int  i   Round
// return value
//   __m128i    Output
//==============================================================
__m128i InvAddRoundKey(block_state* self, __m128i data, int i)
{
  return(_mm_xor_si128(data, _mm_load_si128((__m128i*)&(self->w[i*4]))));
}

#define block_encrypt Cipher_One
#define block_decrypt InvCipher_One

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
int aes_sse2(I decrypt,I mode,UC *key,I keyn,UC* ivec,UC* out,I len)
{
  block_state self;
  uint8_t *str=out;
  I i;

  switch(mode) {
  case 0:
    block_init(&self, key, (int)keyn);
    if(decrypt) {
      for(i=0; i<len; i+=BLOCK_SIZE) block_decrypt(&self, str+i,out+i);
    } else {
      for(i=0; i<len; i+=BLOCK_SIZE) block_encrypt(&self, str+i,out+i);
    }
    block_finalize(&self);
    break;

  case 1:
    block_init(&self, key, (int)keyn);
    if(decrypt) {
      __m128i iv, temp, storeNextIv;
      iv = _mm_loadu_si128((__m128i*)ivec);
      for(i=0; i<len; i+=BLOCK_SIZE) {
        storeNextIv = _mm_loadu_si128((__m128i*)(str+i));
        block_decrypt(&self, str+i, (uint8_t*)&temp);
        temp = XOR(temp, iv);
        _mm_storeu_si128((__m128i*)(out+i), temp);
        iv = storeNextIv;
      }
    } else {
      __m128i iv, temp;
      iv = _mm_loadu_si128((__m128i*)ivec);
      for(i=0; i<len; i+=BLOCK_SIZE) {
        temp = _mm_loadu_si128((__m128i*)(str+i));
        temp = XOR(temp, iv);
        block_encrypt(&self, (uint8_t*)&temp, out+i);
        iv = _mm_loadu_si128((__m128i*)(out+i));
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
