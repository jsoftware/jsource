#ifndef CPU_INFO_H
#define CPU_INFO_H

#include <stdint.h>

extern void cpuInit(void);
extern uint64_t getCpuFeatures(void);

enum {
    CPU_X86_FEATURE_SSSE3  = (1 << 0),
    CPU_X86_FEATURE_POPCNT = (1 << 1),
    CPU_X86_FEATURE_MOVBE  = (1 << 2),
    CPU_X86_FEATURE_SSE4_1 = (1 << 3),
    CPU_X86_FEATURE_SSE4_2 = (1 << 4),
    CPU_X86_FEATURE_AES_NI = (1 << 5),
    CPU_X86_FEATURE_AVX =    (1 << 6),
    CPU_X86_FEATURE_RDRAND = (1 << 7),
    CPU_X86_FEATURE_AVX2 =   (1 << 8),
    CPU_X86_FEATURE_SHA_NI = (1 << 9),
};

#endif /* CPU_INFO_H */
