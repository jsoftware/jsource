#if _WIN32
#define __iamcu__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <string.h>
#include "openssl.h"

void OPENSSL_cleanse(void *ptr, size_t len) {
#if defined(_WIN32)
  SecureZeroMemory(ptr, len);
#else
  memset(ptr, 0, len);
#endif
}

