#include <stdint.h>
#include <string.h> // CBC mode, for memset

#define AES_BLOCKLEN 16 //Block length in bytes AES is 128b block only

#define AES_keyExpSize 240

typedef struct {
  uint8_t RoundKey[AES_keyExpSize];
  uint8_t Iv[AES_BLOCKLEN];
  uint8_t Nk;
  uint8_t Nr;
} AES_ctx;

void AES_init_ctx(AES_ctx* ctx, const uint8_t* key, int keyn);
void AES_init_ctx_iv(AES_ctx* ctx, const uint8_t* key, int keyn, const uint8_t* iv);
void AES_ctx_set_iv(AES_ctx* ctx, const uint8_t* iv);

// buffer size is exactly AES_BLOCKLEN bytes;
// you need only AES_init_ctx as IV is not used in ECB
// ECB is considered insecure for most uses
void AES_ECB_encrypt(AES_ctx* ctx, uint8_t* buf);
void AES_ECB_decrypt(AES_ctx* ctx, uint8_t* buf);

// buffer size MUST be mutiple of AES_BLOCKLEN;
void AES_CBC_encrypt_buffer(AES_ctx* ctx, uint8_t* buf, uintptr_t length);
void AES_CBC_decrypt_buffer(AES_ctx* ctx, uint8_t* buf, uintptr_t length);

// Same function for encrypting as for decrypting.
// IV is incremented for every block, and used after encryption as XOR-compliment for output
void AES_CTR_xcrypt_buffer(AES_ctx* ctx, uint8_t* buf, uintptr_t length);

