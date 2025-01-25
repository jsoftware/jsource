/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Parsing: Macros and Defined Constants                                   */


#define NCASES          9L           /* # of rows in cases parses table    */
#define ACTION(f)       A*  f(J jt)
#define TACT(f)         TA f(J jt,I b,I e,TA*stack,A locsyms,I tmonad,I tsubst,TA *ttab,I *ttabi,I ttabi0)

typedef TA(*TAF)();
typedef A*(*ACTF)();
typedef struct{ I c[4];ACTF f; TAF vf; I b, e, k; } PT;

/* c:  4-patterns for AT(x) of top 4 parser stack elements      */
/* f:  action                                                   */
/* vf: action for tacit verb translator                         */
/* b:  beginning index in stack that action applies to          */
/* e:  ending    index in stack that action applies to          */
/* k:  index in stack of error indicator                        */
// this is now used only for tacit translator

extern PT       cases[];

extern TACT(jtvadv);
extern TACT(jtvconj);
extern TACT(jtvdyad);
extern TACT(jtvfolk);
extern TACT(jtvhook);
extern TACT(jtvis);
extern TACT(jtvmonad);
extern TACT(jtvpunc);

