#include <stdint.h>

struct AES_ctx_128 {
  uint8_t RoundKey[176];
  uint8_t Iv[16];
};

struct AES_ctx_192 {
  uint8_t RoundKey[208];
  uint8_t Iv[16];
};

struct AES_ctx_256 {
  uint8_t RoundKey[240];
  uint8_t Iv[16];
};

void AES_init_ctx_128(struct AES_ctx_128* ctx, const uint8_t* key);
void AES_init_ctx_iv_128(struct AES_ctx_128* ctx, const uint8_t* key, const uint8_t* iv);
void AES_ctx_set_iv_128(struct AES_ctx_128* ctx, const uint8_t* iv);
void AES_ECB_encrypt_128(const struct AES_ctx_128* ctx, uint8_t* buf);
void AES_ECB_decrypt_128(const struct AES_ctx_128* ctx, uint8_t* buf);
void AES_CBC_encrypt_buffer_128(struct AES_ctx_128* ctx, uint8_t* buf, uintptr_t length);
void AES_CBC_decrypt_buffer_128(struct AES_ctx_128* ctx, uint8_t* buf, uintptr_t length);
void AES_CTR_xcrypt_buffer_128(struct AES_ctx_128* ctx, uint8_t* buf, uintptr_t length);

void AES_init_ctx_192(struct AES_ctx_192* ctx, const uint8_t* key);
void AES_init_ctx_iv_192(struct AES_ctx_192* ctx, const uint8_t* key, const uint8_t* iv);
void AES_ctx_set_iv_192(struct AES_ctx_192* ctx, const uint8_t* iv);
void AES_ECB_encrypt_192(const struct AES_ctx_192* ctx, uint8_t* buf);
void AES_ECB_decrypt_192(const struct AES_ctx_192* ctx, uint8_t* buf);
void AES_CBC_encrypt_buffer_192(struct AES_ctx_192* ctx, uint8_t* buf, uintptr_t length);
void AES_CBC_decrypt_buffer_192(struct AES_ctx_192* ctx, uint8_t* buf, uintptr_t length);
void AES_CTR_xcrypt_buffer_192(struct AES_ctx_192* ctx, uint8_t* buf, uintptr_t length);

void AES_init_ctx_256(struct AES_ctx_256* ctx, const uint8_t* key);
void AES_init_ctx_iv_256(struct AES_ctx_256* ctx, const uint8_t* key, const uint8_t* iv);
void AES_ctx_set_iv_256(struct AES_ctx_256* ctx, const uint8_t* iv);
void AES_ECB_encrypt_256(const struct AES_ctx_256* ctx, uint8_t* buf);
void AES_ECB_decrypt_256(const struct AES_ctx_256* ctx, uint8_t* buf);
void AES_CBC_encrypt_buffer_256(struct AES_ctx_256* ctx, uint8_t* buf, uintptr_t length);
void AES_CBC_decrypt_buffer_256(struct AES_ctx_256* ctx, uint8_t* buf, uintptr_t length);
void AES_CTR_xcrypt_buffer_256(struct AES_ctx_256* ctx, uint8_t* buf, uintptr_t length);
