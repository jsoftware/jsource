#ifndef CRC32C_H
#define CRC32C_H

#include <stdint.h>

#if defined(_WIN64)||defined(__LP64__)
extern uint64_t crc32csb8(uint64_t crc, uint64_t value);
#else
extern uint32_t crc32csb8(uint32_t crc, uint64_t value);
#endif
extern uint32_t crc32csb4(uint32_t crc, uint32_t value);

#endif

