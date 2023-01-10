/* Copyright (c) 1990-2023, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* R0 and ASSERT (used twice in vq.c)                                      */

#ifndef FAIL
#define FAIL 0
#endif


#ifdef R0
#undef R0
#endif

#if FINDNULLRET // When we return 0, we should always have an error code set.  trap if not
#define R0 {if(!jt->jerr)SEGFAULT; R FAIL;}
#else
#define R0 R FAIL;
#endif

#ifdef ASSERT
#undef ASSERT
#endif

#define ASSERT(b,e) {if(unlikely(!(b))){jsignal(e); R0;}}
