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
A   imax=0;               // IMAX as atom
I   v00[2]={0,0};         // vector value to use for rank 0 0
A   onehalf;      //  0.5
D   pf=0;                 /* performance frequency                */
A   pie=0;                /* o.1                                  */
// obsolete X   iv1=0;               /* extended integer 1                   */
// obsolete X   iv0=0;              /* extended integer 0                   */
// obsolete A   one=0;                /* 1                                    */
// obsolete A   zero=0;               /* 0                                    */
Q   zeroQ={0,0};          /* 0r1                                  */
DX  zeroDX={0,0,0};       /* 0                                    */
Z   zeroZ={0,0};          /* 0j0                                  */
dcomplex zone={1.0,0.0};  // used gy gemm
dcomplex zzero={0.0,0.0};
A   zpath=0;              /* default locale search path           */
uint64_t g_cpuFeatures;   // blis
int hwfma=0;              // blis cpu tuning
A   pst[256]={0};
A   chr[256]={0};         /* scalar for each character, or 0      */
B   testb[256]={0};       /* 1 iff test block follows             */
C   wtype[256]={0};
// obsoletes follow
 // Table of hash-table sizes
// These are primes (to reduce collisions), and big enough to just fit into a power-of-2
// block after leaving 2 words for memory header, AH words for A-block header, 1 for rank (not used for symbol tables),
// and SYMLINFOSIZE for the unused first word, which (for symbol tables) holds general table info.
// symbol tables allocate ptab[]+SYMLINFOSIZE entries, leaving ptab[] entries for symbols.  i.-family
// operations use tables of size ptab[].  i.-family operations have rank 1.  So the prime in the table
// must be no more than (power-of-2)-10.
 // If AH changes, these numbers need to be revisited
// The first row of small values was added to allow for small symbol tables to hold local
// variables.  Hardwired references to ptab in the code have 3 added so that they correspond to
// the correct values.  User locale sizes also refer to the original values, and have 3 added before use.
I ptab[]={
        3,         5,        19,
       53,       113,       241,       497,     1013, 
     2029,      4079,      8179,     16369,    32749, 
    65521,    131059,    262133,    524269,  1048559, 
  2097133,   4194287,   8388593,  16777199, 33554393, 
 67108837, 134217689, 268435399, 536870879
};
// The bucket-size table[i][j] gives the hash-bucket number of argument-name j when the symbol table was
// created with size i.  The argument names supported are ynam and xnam.
// obsolete UI4 yxbuckets[14];    // bucket positions for x/y (x in high part).  Only 14, because we can't hold more than 16 bits here
UC  bitc[256]={0};        /* # 1 bits in each possible byte       */
C   bitdisp[256*16]={0};  /* display for each possible byte       */
A   mdot=0;               /* m.                                   */
A   ndot=0;               /* n.                                   */
A   udot=0;               /* u.                                   */
A   vdot=0;               /* v.                                   */
A   xdot=0;               /* x.                                   */
A   ydot=0;               /* y.                                   */

// globals end

#if SY_64
#if C_AVX || !(defined(_M_X64) || defined(__x86_64__))
#define bits "64"
#else
#define bits "64nonavx"
#endif
#else
#define bits "32"
#endif

char jeversion[]= "je9!:14 j"jversion"/j"bits"/"jplatform"/"jtype"/"jlicense"/"jbuilder"/"__DATE__"T"__TIME__;

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
