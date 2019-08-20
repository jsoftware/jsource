#if _WIN32
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

