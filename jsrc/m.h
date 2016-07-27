/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Memory Management                                                       */

/* ANSI C already has malloc by j.h include of stdlib.h                    */
#if (SYS & SYS_PCWIN+SYS_PC386+SYS_UNIX) && !(SYS+SYS_ANSI)
#include <malloc.h>
#endif

#if SY_WIN32 && !SY_WINCE
#define FREE(a)     HeapFree(jt->heap,0,a)
#define MALLOC(n)   (void*)HeapAlloc(jt->heap,0,n)
#else
#define FREE(a) free(a)
#define MALLOC(n) malloc(n)
#endif

typedef struct {I*a;US j;US blkx;} MS;

/* layout of the two words before every A array                            */
/* a: ptr to next block (when in free list)                                */
/*    address of SMM array, or 0 (when allocated)                          */
/* j: mfree/msize index                                                    */
/* blkx: distance to go back to get to the first pool buffer from the allocation (0 for first) */

#define mhb sizeof(MS)                  /* # bytes in memory header             */
#define mhw (sizeof(MS) / SZI)              /* # words in memory header             */

// Memory-allocation info
// PMINL is lg2(size of smallest pool buffer).  It must be big enough to hold at least one I. 
#define PMINL ((AH*SZI+mhb+SZI)<=64?6:7)
#define PMIN (1L<<PMINL)   // size of smallest block
// PLIML is lg2(size of largest pool buffer), 10 or 11
#define PLIML       (4+PMINL)            // lg2(PLIM)
#define PLIM        (1L<<PLIML)          /* pool allocation is used for blocks <= PLIM   */
// PSIZEL is lg2(size of allocation when pool buffers are allocated), 16 or 17
#define PSIZEL      (10+PMINL)            // lg(PSIZE)
#define PSIZE       (1L<<PSIZEL)      /* size of each pool                    */




// bp(type) returns the number of bytes
#define bp(i) typesizes[CTTZ(i)]
