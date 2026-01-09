/* Copyright (c) 1990-2026, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Index-of                                                         */

#include "j.h"
#if C_CRC32C && SY_64 && (C_AVX2 || EMU_AVX2)
#define suff 2
#define TH UI4
#include "viavx1.h"
#endif
