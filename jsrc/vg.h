/* Copyright 1990-2004, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Grades                                                                  */


extern F1(jtgr1);
extern F2(jtgr2);
extern F1(jtgrd1sp);
extern F2(jtgrd2sp);

extern int  compa(J,I,I);
extern int  compc(J,I,I);
extern int  compd(J,I,I);
extern int  compd1(J,I,I);
extern int  compi(J,I,I);
extern int  compi1(J,I,I);
extern int  compp(J,I,I);
extern int  compq(J,I,I);
extern int  compr(J,I,I);
extern int  compu(J,I,I);
extern int  compt(J,I,I);
extern int  compt1(J,I,I);
extern int  compx(J,I,I);

extern int  compspdsB(J,I,I);
extern int  compspdsD(J,I,I);
extern int  compspdsI(J,I,I);
extern int  compspdsZ(J,I,I);

extern int  compspssB(J,I,I);
extern int  compspssD(J,I,I);
extern int  compspssI(J,I,I);
extern int  compspssZ(J,I,I);

extern void grcol(I,I,I*,I,I*,I*,const I,US*,int,int,int);
extern void grcolu(I,I,UI*,I,UI*,UI*,const I,US*,int,int,int);

extern void jtmsort(J,I,I*,I*);

// Convert 2 Booleans to a code 0-3
#if C_LE
#define IND2(x) {US xx = (x); ii = ((xx<<9)|xx)>>8;}
#else
#define IND2(x) {US xx = (x); ii = 0x3&((xx>>7)|xx);}
#endif

// Convert 4 Booleans to a code 0-15
#if C_LE
#define IND4(x) {UINT xx = (x); ii = ((xx<<27)|(xx<<18)|(xx<<9)|xx)>>24;}  // first byte (bit 0) is the MSB when a word is loaded
#else
#define IND4(x) {UINT xx = (x); ii = 0xf&((xx>>21)|(xx>>14)|(xx>>7)|xx);}
#endif
