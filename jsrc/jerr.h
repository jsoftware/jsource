/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
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
#define EVEXIT          15  // set when 2!:55 has requested termination
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
#define EVVALENCE       37      // no defined valence for verb
#define EVINHOMO        38     // inhomogeneous args - converted to EVDOMAIN after eformat
#define EVINDEXDUP      39     // duplicate permutation index - converted to EVINDEX after eformat
#define EVEMPTYT        40    // empty T block for select or for - converted to EVCTRL
#define EVEMPTYDD       41    // empty {{ }} - converted to EVCTRL
#define EVMISSINGGMP    42    // libgmp not installed - converted to EVFACE
#define EVSIDAMAGE      43    // reload of running function - converted to stack error
#define EVDEADLOCK      44    // action would deadlock the system
#define NEVM            44      /* number of event codes       */
#define EVCONCURRENCY   EVDOMAIN      // eg deadlock, unlocked somebody else's lock

/* The following codes are never displayed to the user   */
#define EVFOLDEXIT      59      // abort current execution and return the current result
#define EVFOLDSKIP      60      // abort current execution and resume with the next iteration
#define EWIMAG          61      /* imaginary  result           */
#define EWIRR           62      /* irrational result           */
#define EWRAT           63      /* rational   result           */
#define EWDIV0          64      /* division by zero            */
#define EWOV            66      // integer overflow from the old routines that do not support recovery.  Anything >= EWOV is an overflow.  Leave at 50 until asm routines are retired
#define EWOV1           67      // integer overflow that can be recovered because the float version has already been saved
#define EWOVIP          68      // overflow, but recoverable even though executed in place.  It starts here, but identifies the routine it came from:
#define EWOVIPPLUSII    1  //0b001
#define EWOVIPPLUSBI    0  //0b000  // bit 2=XOR value, bit0=AND value, bits 1:0 indicate function to use
#define EWOVIPPLUSIB    4  //0b100
#define EWOVIPMINUSII   3  //0b011
#define EWOVIPMINUSBI   2  //0b010
#define EWOVIPMINUSIB   6  //0b110
#define EWOVIPMULII     5
#define EWOVFLOOR0      68   // minor oflo in <./>., can be left as INT
#define EWOVFLOOR1      69   // major oflo in <./>., must be converted to FL
#define EVCUTSTACK      76  // set when Cut Stack executed; passed into caller to force its termination; intercepted when terminated line finishes, to revert to suspension
#define EVDEBUGEND      79  // EVEXIT+64 set when debug wants to fail all avoiding all try blocks
#define EVSUPPRESS      255  // turn off error reporting: don't change anything.  Not used
#define EVOK            256  // error code used to mean 'no error' in cases where we have to take the minimum of returned errors
#define EVOKCLEANUP     512  // no error, but there is cleanup work to do

// Exigent errors are those errors that must not be ignored if they happen during execution on a fill-cell
#define EMSK(x) (((I)1)<<((x)-1))
#define EXIGENTERROR (EMSK(EVALLOC) | EMSK(EVATTN) | EMSK(EVEXIT) | EMSK(EVBREAK) | EMSK(EVINPRUPT) | EMSK(EVFACE) | EMSK(EVWSFULL) | EMSK(EVTIME) | EMSK(EVSTACK) | EMSK(EVSYSTEM) )  // errors that always create failure
