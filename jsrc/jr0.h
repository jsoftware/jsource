/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* R0 and ASSERT (used twice in vq.c)                                      */

#ifndef FAIL
#define FAIL 0
#endif


#undef R0

#if FINDNULLRET // When we return 0, we should always have an error code set.  trap if not
#define R0 {if(!jt->jerr)SEGFAULT; R FAIL;}
#else
#define R0 R FAIL;
#endif

#undef ASSERT

#define ASSERT(b,e) {if(unlikely(!(b))){jsignal(e); R0;}}
// version for debugging
// #define ASSERT(b,e)     {if(unlikely(!(b))){fprintf(stderr,"error code: %i : file %s line %d\n",(int)(e),__FILE__,__LINE__); jsignal(e); R0;}}
// #define ASSERT(b,e)     {if(unlikely(!(b))){if(!(jt->emsgstate&EMSGSTATETRAPPING)){char ermsg[2000];sprintf(ermsg,"error code: %i : file %s line %d\n",(int)(e),__FILE__,__LINE__);fprintf(stderr,"%s",ermsg);jsto(JJTOJ(jt),MTYOER,ermsg);jsignal(e);}R0;}}
