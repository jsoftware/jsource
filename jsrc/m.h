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
#define REALLOC(a,n) (void*)HeapReAlloc(jt->heap,0,a,n)
#else
#define FREE(a) free(a)
#define MALLOC(n) malloc(n)
#define REALLOC(a,n) realloc(a,n)
#endif

// **** main blocks are managed by allocating 16 bits for all blocks, free or allocated
// when the block is on the free list, AK is a chain pointer

/* layout of the two words before every A array                            */
/* a: ptr to next block (when in free list)                                */
/*    address of SMM array, or 0 (when allocated)                          */
/* j: mfree/msize index                                                    */
/* blkx: distance to go back to get to the first pool buffer from the allocation (0 for first) */


// Memory-allocation info
// PMINL is lg2(size of smallest pool buffer).  It must be big enough to hold at least one I. 
#define PMINL 6   // ((AH*SZI+mhb+SZI)<=64?6:7) smaller buffers possible on 32-bit, but we don't bother
#define PMIN (1L<<PMINL)   // size of smallest block
// PLIML is lg2(size of largest pool buffer), 10 or 11
#define PLIML       (4+PMINL)            // lg2(PLIM)
#define PLIM        (1L<<PLIML)          /* pool allocation is used for blocks <= PLIM   */
// PSIZEL is lg2(size of allocation when pool buffers are allocated), 16 or 17
#define PSIZEL      (10+PMINL)            // lg(PSIZE)
#define PSIZE       (1L<<PSIZEL)      /* size of each pool                    */


// bp(type) returns the number of bytes in an atom of the type
#define bp(i) (jt->typesizes[CTTZ(i)])
// bplg(type) works for NOUN types and returns the lg of the size
#if BW==64
#define bplg(i) (((I)0x008bb6db408dc6c0>>3*CTTZ(i))&(I)7)  // 010 001 011   101 101 101 101 101 101 000 000   100 011 011 100 011 011 000 000 = 0 1000 1011 1011 0110 1101 1011   0100 0000 1000 1101 1100 0110 1100 0000
// bpnoun is like bp but for NOUN types
#define bpnoun(i) ((I)1<<bplg(i))
#else
#define bpnoun(i) (I)bp(i)
#define bplg(i) CTTZ(bpnoun(i))
#endif


