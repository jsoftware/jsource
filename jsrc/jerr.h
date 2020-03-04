/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Error Codes                                                             */


#define EVATTN          1       /* See i.c for error texts     */
#define EVBREAK         2
#define EVDOMAIN        3
#define EVILNAME        4
#define EVILNUM         5
#define EVINDEX         6
#define EVFACE          7
#define EVINPRUPT       8
#define EVLENGTH        9
#define EVLIMIT         10
#define EVNONCE         11
#define EVASSERT        12
#define EVOPENQ         13
#define EVRANK          14
#define EVSPELL         16
#define EVSTACK         17
#define EVSTOP          18
#define EVSYNTAX        19
#define EVSYSTEM        20
#define EVVALUE         21
#define EVWSFULL        22
#define EVCTRL          23
#define EVFACCESS       24
#define EVFNAME         25
#define EVFNUM          26
#define EVTIME          27
#define EVSECURE        28
#define EVSPARSE        29
#define EVLOCALE        30
#define EVRO            31
#define EVALLOC         32
#define EVNAN           33
#define EVNONNOUN       34
#define EVTHROW         35      // uncaught throw.
#define EVFOLDLIMIT     36      // abort current execution and return the current result
#define NEVM            36      /* number of event codes       */

/* The following codes are never displayed to the user (but may show up in a 13!:12 query)   */
#define EVFOLDEXIT      43      // abort current execution and return the current result
#define EVFOLDSKIP      44      // abort current execution and resume with the next iteration
#define EWIMAG          45      /* imaginary  result           */
#define EWIRR           46      /* irrational result           */
#define EWRAT           47      /* rational   result           */
#define EWDIV0          48      /* division by zero            */
#define EWOV            50      // integer overflow from the old routines that do not support recovery.  Anything >= EWOV is an overflow.  Leave at 50 until asm routines are retired
#define EWOV1           51      // integer overflow that can be recovered because the float version has already been saved
#define EWOVIP          52      // overflow, but recoverable even though executed in place.  It starts here, but identifies the routine it came from:
#define EWOVIPPLUSII    1  //0b001
#define EWOVIPPLUSBI    0  //0b000  // bit 2=XOR value, bit0=AND value, bits 1:0 indicate function to use
#define EWOVIPPLUSIB    4  //0b100
#define EWOVIPMINUSII   3  //0b011
#define EWOVIPMINUSBI   2  //0b010
#define EWOVIPMINUSIB   6  //0b110
#define EWOVIPMULII     5
#define EWOVFLOOR0      52   // minor oflo in <./>., can be left as INT
#define EWOVFLOOR1      53   // major oflo in <./>., must be converted to FL
#define EVOK            256  // error code used to mean 'no error' in cases where we have to take the minimum of returned errors
#define EVOKCLEANUP     512  // no error, but there is cleanup work to do

// Exigent errors are those errors that must not be ignored if they happen during execution on a fill-cell
#define EMSK(x) (((I)1)<<((x)-1))
#define EXIGENTERROR (EMSK(EVALLOC) | EMSK(EVATTN) | EMSK(EVBREAK) | EMSK(EVINPRUPT) | EMSK(EVFACE) | EMSK(EVWSFULL) | EMSK(EVTIME) | EMSK(EVSTACK) | EMSK(EVSYSTEM) )  // errors that always create failure
