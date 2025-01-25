/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Memory Management                                                       */

/* ANSI C already has malloc by j.h include of stdlib.h                    */
#if (SYS & SYS_PCWIN+SYS_PC386+SYS_UNIX) && !(SYS+SYS_ANSI)
#include <malloc.h>
#endif

#if 0 && SY_WIN32 && !SY_WINCE
#define FREE(a)     HeapFree(JT(jt,heap),0,a)
#define MALLOC(n)   (void*)HeapAlloc(JT(jt,heap),0,n)
#define REALLOC(a,n) (void*)HeapReAlloc(JT(jt,heap),0,a,n)
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


#define LEAKSNIFF 0
#define SHOWALLALLOC 0 // to display log of allo/free
/*  to analyze
;@:(<@({.~  2 | #)/.)~ (#~ ('0000' -: 4&{.)@>) {.@;:;._2 wd 'clippaste'
*/

//NOTE: alignment to cache is now required because of LSB flags in enqueue()
#define ALIGNTOCACHE 1   // set to 1 to align each OS-allocated block block to cache-line boundary.  Will reduce cache usage for headers
#define ALIGNPOOLTOCACHE 1   // set to 1 to align each pool block to cache-line boundary.  Will reduce cache usage for headers
#define TAILPAD (32)  // we must ensure that a 32-byte masked op fetch to the last byte doesn't run off into unallocated memory

#define MEMJMASK 0xf   // these bits of j contain subpool #; higher bits used for computation for subpool entries
#define SBFREEBLG (14+PMINL)   // lg2(SBFREEB)
#define SBFREEB (1L<<SBFREEBLG)   // number of bytes that need to be freed before we rescan
#define MFREEBCOUNTING 1   // When this bit is set in mfreeb[], we keep track of max space usage

// Format of h, the 16-bit workarea for free and allocated blocks in main memory.  This is used for in-memory headers for NJS blocks, but not for NJA blocks all on disk
#define AFCHAIN(a) ((a)->kchain.chain)  // the chain field, when the block is not allocated
#define AFPROXYCHAIN(a) ((a)->tproxy.proxychain)  // chain field for base proxies during garbage collection
#define FHRHROOTX 15
#define FHRHROOT (((I)1)<<FHRHROOTX)  // set if the current block is the root (the first of the consecutive blocks making up its allocation)
#define FHRHROOTFREE ((I)2<<(PSIZEL-PMINL))   // If this bit is set at the end of garbage-collection, the whole allocation can be freed.  LSB (precisely, lowest 1-bit) is the size indicator
//
// the lower bits encode the size of the block, by the position of the lowest 1 bit, and in the upper bits either (1) the full size of the block for large allocations
// (2) the offset of the block from the root, for pool allocations.  The following macros define the field
// For GMP allocations, h has a special value and we free them through mfgmp
#define FHRHISGMP 0x4000  // this block was allocated by GMP
#define FHRHBINISGMP 14  // this block was allocated by GMP
#define FHRHPOOLBIN(h) CTTZ(h)     // pool bin# for free (0 means allo of size PMIN, etc).  If this gives PLIML-PMINL+1, the allocation is a system allo
#define FHRHBINISPOOL(h) ((h)&((2LL<<(PLIML-PMINL))-1))      // true is this is a pool allo, false if system (h is mask from block)
#define FHRHBININPOOL(bin) ((bin)<PLIML-PMINL+1)      // true is this is a pool allo, false if system or GMP (h is bin#)
#define ALLOJISPOOL(j) ((j)<=PLIML)     // true if pool allo, false if system (j is lg2(requested size))
#define ALLOJBIN(j) ((j)-PMINL)   // convert j (=lg2(size)) to pool bin#
#define FHRHPOOLBINSIZE(h) (LOWESTBIT(h)<<PMINL)        // convert hmask to size for pool bin#
#define FHRHPOOLBINTOSIZE(bin) (PMIN<<(bin))        // convert hmask to size for pool bin#
#define FHRHSYSSIZE(h) (((I)1)<<((h)>>(PLIML-PMINL+2)))        // convert h to size for system alloc
#define FHRHSIZE(h) ({I lbit=LOWESTBIT(h)&((2LL<<(PLIML-PMINL))-1); I hbit=lbit==0; (lbit<<PMINL)+((hbit<<((h)>>(PLIML-PMINL+2)))); })
#define FHRHSYSJHDR(j) ((2*(j)+1)<<(PLIML-PMINL+1))        // convert j (=lg(size)) to h format for a system allo
#define FHRHBININCR(b) ((I)2<<(b))      // when garbage-collecting bin b, add this much to the root for each free block encountered.  This is also the amount by which the h values of successive blocks in an allocation differ
#define FHRHBLOCKOFFSETMASK(b) (FHRHROOTFREE - FHRHBININCR(b))  // for blocks in pool b, mask to use to extract offset to root
#define FHRHBLOCKOFFSET(h,m) ((((h)>>(FHRHROOTX-0))-1) & (((h)&(m))<<(PMINL-1)))     // how far the pool block with h is offset from its root.  0 if FHRHROOT is set.  m is FHRHBLOCKOFFSETMASK
#define FHRHISROOTALLOFREE(h) ((h)&FHRHROOTFREE)   // given the root's h after garbage collection, is the entire allocation free?
#define FHRHROOTADDR(a,m) ((A)((C*)(a) - FHRHBLOCKOFFSET(AFHRH(a),(m))))   // address of root for block a.  m is FHRHBLOCKOFFSETMASK
#define FHRHISALLOFREE(a,m) FHRHISROOTALLOFREE(AFHRH(FHRHROOTADDR((a),(m))))      // is the given block a free after garbage collection? m is FHRHBLOCKOFFSETMASK
#define FHRHRESETROOT(b) (FHRHROOT + (((I)1)<<(b)))     // value to set root to after garbage-collection if the allocation was NOT freed
#define FHRHENDVALUE(b) (FHRHROOTFREE + (((I)1)<<(b)))     // value representing last+1 block in allo.  Subtract FHRHBININCR to get to previous

// the size of the total allocation of the block for w, always a power of 2
#define alloroundsize(w)  (ISGMP(w) ?XHSZ+AN(w) :FHRHSIZE(AFHRH(w)))
