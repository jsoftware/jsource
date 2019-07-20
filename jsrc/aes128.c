#include <stdint.h>
#include <string.h> // CBC mode, for memset

#define AES128 1

// public interface
#define AES_ctx AES_ctx_128
#define AES_init_ctx AES_init_ctx_128
#define AES_init_ctx_iv AES_init_ctx_iv_128
#define AES_ctx_set_iv AES_ctx_set_iv_128
#define AES_ECB_encrypt AES_ECB_encrypt_128
#define AES_ECB_decrypt AES_ECB_decrypt_128
#define AES_CBC_encrypt_buffer AES_CBC_encrypt_buffer_128
#define AES_CBC_decrypt_buffer AES_CBC_decrypt_buffer_128
#define AES_CTR_xcrypt_buffer AES_CTR_xcrypt_buffer_128

#include "aes-c.h"
