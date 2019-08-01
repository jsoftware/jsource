/* Copyright 1990-2004, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Grades                                                                  */


extern F1(jtgr1);
extern F2(jtgr2);
extern F1(jtgrd1sp);
extern F2(jtgrd2sp);

extern B compcu(I,UC*,UC*);
extern B compcd(I,UC*,UC*);
extern B compuu(I,US*,US*);
extern B compud(I,US*,US*);
extern B comptu(I,C4*,C4*);
extern B comptd(I,C4*,C4*);
extern B compr(I,A*,A*);
extern B comppu(I,D*,D*);
extern B comppd(I,D*,D*);
extern B compxu(I,X*,X*);
extern B compxd(I,X*,X*);
extern B compqu(I,Q*,Q*);
extern B compqd(I,Q*,Q*);
extern B compp(I,I*,I*);

extern int  compspdsB(J,I,I);
extern int  compspdsD(J,I,I);
extern int  compspdsI(J,I,I);
extern int  compspdsZ(J,I,I);

extern int  compspssB(J,I,I);
extern int  compspssD(J,I,I);
extern int  compspssI(J,I,I);
extern int  compspssZ(J,I,I);

extern I grcol4(I,I,UI4*,I,I*,I*,const I,US*,I);
extern I grcol2(I,I,US*,I,I*,I*,const I,US*,I);

extern void jtmsort(J,I,void**,void**);

// Convert 2 Booleans to a code 0-3
#if C_LE
#define IND2(x) {US xx = (x); ii = ((xx<<9)|xx)>>8;}
#else
#define IND2(x) {US xx = (x); ii = 0x3&((xx>>7)|xx);}
#endif

// Convert 4 Booleans to a code 0-15
#if C_LE
#define IND4(x) {UINT xx = (x); xx|=xx<<9; xx|=xx<<18; ii = xx>>24;}  // first byte (bit 0) is the MSB when a word is loaded
#else
#define IND4(x) {UINT xx = (x); ii = 0xf&((xx>>21)|(xx>>14)|(xx>>7)|xx);}
#endif

// endian constants for 16-bit radix sorts
#define FPLSBWDX (C_LE?0:3)
#define FPMSBWDX (C_LE?3:0)
#define INTLSBWDX (C_LE?0:(SZI/2-1))
#define INTMSBWDX (C_LE?(SZI/2-1):0)
#define WDINC (C_LE?1:-1)
