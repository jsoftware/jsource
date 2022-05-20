/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Fast Fourier Transform and Friends                               */

#include "j.h"


/*
 0 0  8 0     0  0  4  0     0  0  2  0     0  0  1  0  
 1 1  9 0     1  1  5  0     1  1  3  0     1  0  1  8  
 2 2 10 0     2  2  6  0     2  0  2  8     2  2  3  4  
 3 3 11 0     3  3  7  0     3  1  3  8     3  2  3 12  
 4 4 12 0     4  0  4  8     4  4  6  4     4  4  5  2  
 5 5 13 0     5  1  5  8     5  5  7  4     5  4  5 10  
 6 6 14 0     6  2  6  8     6  4  6 12     6  6  7  6  
 7 7 15 0     7  3  7  8     7  5  7 12     7  6  7 14  
 8 0  8 8     8  8 12  4     8  8 10  2     8  8  9  1  
 9 1  9 8     9  9 13  4     9  9 11  2     9  8  9  9  
10 2 10 8    10 10 14  4    10  8 10 10    10 10 11  5  
11 3 11 8    11 11 15  4    11  9 11 10    11 10 11 13  
12 4 12 8    12  8 12 12    12 12 14  6    12 12 13  3  
13 5 13 8    13  9 13 12    13 13 15  6    13 12 13 11  
14 6 14 8    14 10 14 12    14 12 14 14    14 14 15  7  
15 7 15 8    15 11 15 12    15 13 15 14    15 14 15 15  

pow=: * /\ @ }: @ (1&,) @ $
FFT=:      (# pow   pru@#) floop ]
IFT=: # %~ (# pow %@pru@#) floop ]

floop=: 4 : 0
 k=. 2^.n=. #y.
 r=. y.
 p=. ,|:i.k$2                    NB. |."1&.#: i.n
 w=. p{x.                        NB. (pru n)^p
 for_m. <.2^i.-k do.
  i=. (2#i.&.(%&(2*m)) n)+/i.m  
  r=. ,(i{r)+((i+m){r)*(n%m){.w
 end.
 p{r                             NB. ,|:(k$2)$r
)
*/

#if SY_64
#define REVBITS(x,y)
#else
#define REVBITS(x,y)   {y[3]=revb[x[0]]; y[2]=revb[x[1]]; y[1]=revb[x[2]]; y[0]=revb[x[3]];}
#endif

static const UC revb16[16]={0,8,4,12, 2,10,6,14, 1,9,5,13, 3,11,7,15};
static UC revb[256];

static F1(jtravtranspose){I i,n,s;Z t,*v;UC*iv,*jv;UI j;
 n=AN(w); v=ZAV(w);
 j=n; s=SY_64?64:32; while(1<j){j>>=1; --s;} 
 iv=(UC*)&i; jv=(UC*)&j;
 if(128!=revb[1])DO(256, revb[i]=(revb16[i&15]<<4)+revb16[i>>4];);
 for(i=0;i<n;++i){
  REVBITS(iv,jv); j>>=s;
  if(j>(UI)i){t=v[i]; v[i]=v[j]; v[j]=t;}
 }
 R w;
}    /* ravel transpose in place: ,|:(#:&.<:#w)$w  or  (|."1&.#:i.#w){w */

static A jtfloop(J jt,A w,I k,B inv){
R 0;
}
