/* Copyright 1990-2018, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Global Variables                                                        */

#include "j.h"
#include "jversion.h"
#include "gemm.h"

// globals start - set by globinit at dll initialization
A   a0j1=0;               /* 0j1                                  */
A   aqq=0;                /* ''                                   */
A   asgnlocsimp;          // =. flagged as LOCAL+NAME
A   asgngloname;          // =. flagged as NAME
UC  bit[8]={(UC)0x80, (UC)0x40, (UC)0x20, (UC)0x10, (UC)0x08, (UC)0x04, (UC)0x02, (UC)0x01};
C   breakdata=0;
double dzero=0.0;   // used by gemm
D   inf=0;                /* _                                    */
D   infm=0;               /* __                                   */
A   ace=0;                /* a:                                   */
A   alp=0;                /* a.                                   */
A   ainf=0;               /* _                                    */
A   iv0=0;                /* ,0   also extended integer 0                                */
A   iv1=0;                /* ,1   also extended integer 1                                */
I   liln=0;               /* 1 iff little endian                  */
A   mark=0;               /* parser marker                        */
D   jnan=0;               /* _.                                   */
C   minus0[8]={0};        /* the abominable minus 0               */
A   mtm=0;                /* i. 0 0                               */
A   mtv=0;                /* i.0                                  */
A   mnam=0;               /* m as a name                          */
A   nnam=0;               /* n as a name                          */
A   unam=0;               /* u as a name                          */
A   vnam=0;               /* v as a name                          */
A   xnam=0;               /* x as a name                          */
A   ynam=0;               /* y as a name                          */
A   numv[NUMMAX-NUMMIN+1]={0};
A   zeroionei[2]={0,0};   // integer 0 and 1
A   numvr[3]={0,0};       // float 0,1,2
A   imax=0;               // IMAX as atom
I   v00[2]={0,0};         // vector value to use for rank 0 0
A   onehalf;      //  0.5 (float)
D   pf=0;                 /* performance frequency                */
A   pie=0;                /* o.1                                  */
Q   zeroQ={0,0};          /* 0r1                                  */
DX  zeroDX={0,0,0};       /* 0                                    */
Z   zeroZ={0,0};          /* 0j0                                  */
dcomplex zone={1.0,0.0};  // used gy gemm
dcomplex zzero={0.0,0.0};
A   zpath=0;              /* default locale search path           */
uint64_t g_cpuFeatures;   // blis
A   pst[256]={0};
A   chr[256]={0};         /* scalar for each character, or 0      */
UC  hwfma=0;              // blis cpu tuning
C   wtype[256]={0};

// globals end

#if SY_64
#define bits "64"
#else
#define bits "32"
#endif

#if C_AVX2
#define hw "avx2"
#else
#if C_AVX
#define hw "avx"
#else
#define hw ""
#endif
#endif

char jeversion[]= "je9!:14 j"jversion"/j"bits""hw"/"jplatform"/"jtype"/"jlicense"/"jbuilder"/"__DATE__"T"__TIME__;

F1(jtversq){
 char m[1000];char d[21]; char months[] = "Jan01Feb02Mar03Apr04May05Jun06Jul07Aug08Sep09Oct10Nov11Dec12"; C* p; C* q;
 ASSERTMTV(w);
 strcpy(m,jeversion+8);
 p= m+strlen(m)-20;
 strcpy(d,p);
 *p=0;
 if(' '== d[4]) d[4] = '0';
 strncat(p,d+7,4);
 strcat(p,"-");
 d[3] = 0;
 q= strstr(months,d);
 strncat(p,3 + strstr(months,d),2);
 strcat(p,"-");
 strncat(p,d + 4,2);
 strcat(p,d+11);
 R cstr(m);
}
