/* Copyright 1990-2014, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: crc32c                                                           */

#include "crc32c.h"
#include "crc32ctables.h"

uint32_t crc32csb4(uint32_t crc, uint32_t value)
{
  crc ^= value;
  uint32_t term1 = crc_tableil8_o56[crc & 0x000000FF] ^
                   crc_tableil8_o48[(crc >> 8) & 0x000000FF];
  uint32_t term2 = crc >> 16;
  crc = term1 ^
        crc_tableil8_o40[term2 & 0x000000FF] ^
        crc_tableil8_o32[(term2 >> 8) & 0x000000FF];
  return crc;
}

#if defined(_WIN64)||defined(__LP64__)
uint64_t crc32csb8(uint64_t crci, uint64_t value)
#else
uint32_t crc32csb8(uint32_t crci, uint64_t value)
#endif
{
  uint32_t crc = (uint32_t)crci ^ value & 0xFFFFFFFFULL;
  uint32_t val = value >> 32;
  uint32_t term1 = crc_tableil8_o88[crc & 0x000000FF] ^
                   crc_tableil8_o80[(crc >> 8) & 0x000000FF];
  uint32_t term2 = crc >> 16;
  crc = term1 ^
        crc_tableil8_o72[term2 & 0x000000FF] ^
        crc_tableil8_o64[(term2 >> 8) & 0x000000FF];
  term1 = crc_tableil8_o56[val & 0x000000FF] ^
          crc_tableil8_o48[(val >> 8) & 0x000000FF];

  term2 = val >> 16;
  crc = crc ^ term1 ^
        crc_tableil8_o40[term2  & 0x000000FF] ^
        crc_tableil8_o32[(term2 >> 8) & 0x000000FF];
  return crc;
}
