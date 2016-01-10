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

extern void jtmsort(J,I,I*,I*);

#define IND2(x)  \
  switch(x){                \
   case BS00: ii=0; break;  \
   case BS01: ii=1; break;  \
   case BS10: ii=2; break;  \
   case BS11: ii=3;         \
  }

#define IND4(x)  \
  switch(x){                                            \
   case B0000: ii=0; break;  case B1000: ii= 8; break;  \
   case B0001: ii=1; break;  case B1001: ii= 9; break;  \
   case B0010: ii=2; break;  case B1010: ii=10; break;  \
   case B0011: ii=3; break;  case B1011: ii=11; break;  \
   case B0100: ii=4; break;  case B1100: ii=12; break;  \
   case B0101: ii=5; break;  case B1101: ii=13; break;  \
   case B0110: ii=6; break;  case B1110: ii=14; break;  \
   case B0111: ii=7; break;  case B1111: ii=15;         \
  }
