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
#define NEVM            34      /* number of event codes       */

/* The following codes are never displayed to the user (but may show up in a 13!:12 query)   */

#define EWOV            50      /* integer overflow            */
#define EWIMAG          51      /* imaginary  result           */
#define EWIRR           52      /* irrational result           */
#define EWRAT           53      /* rational   result           */
#define EWDIV0          54      /* division by zero            */
#define EWTHROW         55      /* throw. executed             */
#define EWOVIP          60      // overflow, but recoverable even though executed in place.  It starts here, but identifies the routine it came from:
#define EWOVIPPLUSII    0
#define EWOVIPPLUSBI    1
#define EWOVIPPLUSIB    2
