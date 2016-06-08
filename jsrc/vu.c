/* Copyright 1990-2014, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Unicode (2-byte unsigned characters)                                    */

#include "j.h"


B jtvc1(J jt,I n,US*v){DO(n, RZ(255>=*v++);); R 1;}
     /* verify that 2-byte chars have high-order 0 bytes */

// allocate new datablock and return the ASCII for the characters in w
// if b is 0, raise error if high byte of unicode is not 0
A jttoc1(J jt,B h,A w){A z;C*wv,*zv;I n;
 RZ(w);
 if(LIT&AT(w))R ca(w);  // if already ASCII, clone it and return the clone
 n=AN(w); wv=CAV(w);    // number of characters, pointer to characters if any
 ASSERT(!n||C2T&AT(w),EVDOMAIN);  // must be empty or unicode
 GA(z,LIT,n,AR(w),AS(w)); zv=CAV(z);  // allocate ASCII area with same data shape
#if C_LE
 if(h)DO(n, *zv++=*wv++; wv++;) else DO(n, *zv++=*wv++; ASSERT(!*wv++,EVDOMAIN);)
 // copy the low byte of the data (if there is any).  if b==0, verify high byte is 0
 // where low and high are depends on endianness
#else
 if(h)DO(n, wv++; *zv++=*wv++;) else DO(n, ASSERT(!*wv++,EVDOMAIN); *zv++=*wv++;)
#endif
 R z;
}    /* convert 2-byte chars to 1-byte chars; 0==h iff high order byte must be 0 */

static F1(jttoc2){F1PREF;A z;C*wv,*zv;I n;
 RZ(w);
 if(C2T&AT(w))R ca(w);
 n=AN(w); wv=CAV(w);
 ASSERT(!n||LIT&AT(w),EVDOMAIN);
 GA(z,C2T,n,AR(w),AS(w)); zv=CAV(z);
#if C_LE
 DO(n, *zv++=*wv++; *zv++=0;);
#else
 DO(n, *zv++=0; *zv++=*wv++;);
#endif
 R z;
}    /* convert 1-byte chars to 2-byte chars */

static F1(jttoc2e){F1PREF;A z;I m,n,r;
 RZ(w);
 n=AN(w); r=AR(w);
 ASSERT(r,EVRANK);
 ASSERT(!n||LIT&AT(w),EVDOMAIN);
 m=*(AS(w)+r-1);
 ASSERT(0==m%2,EVLENGTH);
 GA(z,C2T,n/2,r,AS(w)); *(AS(z)+r-1)=m/2;
 memcpy(AV(z),AV(w),n);
 R z;
}    /* convert pairs of 1-byte chars to 2-byte chars */

static F1(jtifc2){F1PREF;A z;I n,t,*zv;
 RZ(w);
 n=AN(w); t=AT(w);
 ASSERT(!n||t&JCHAR,EVDOMAIN);
 GA(z,INT,n,AR(w),AS(w)); zv=AV(z);
 if(t&LIT){UC*v=(UC*)AV(w); DO(n, *zv++=*v++;);}
 else     {US*v=(US*)AV(w); DO(n, *zv++=*v++;);}
 R z;
}    /* integers from 1- or 2-byte chars */

static F1(jtc2fi){F1PREF;A z;I j,n,*v;US*zv;
 RZ(w=vi(w));
 n=AN(w); v=AV(w);
 GA(z,C2T,n,AR(w),AS(w)); zv=(US*)AV(z);
 DO(n, j=*v++; ASSERT(SMIN<=j&&j<=SMAX,EVINDEX); *zv++=(US)j;);
 R z;
}    /* 2-byte chars from integers */

F1(jtuco1){F1PREF;I t;
 RZ(w);
 t=AT(w);
 ASSERT(!AN(w)||t&JCHAR+NUMERIC,EVDOMAIN);
 R t&NUMERIC?c2fi(w):t&C2T?ca(w):toc2(w);
}    /* copy 2-byte chars; convert 1-byte to 2-byte */


F2(jtuco2){F2PREF;I j;
 RZ(a&&w);
 RE(j=i0(a));
 switch(j){
  case 1: R toc1(1,w);
  case 2: R toc2(w);
  case 3: R ifc2(w);
  case 4: R c2fi(w);
  case 5: R toc1(0,w);
  case 6: R toc2e(w);
  case 7: R toutf16(w);
  case 8: R toutf8(w);
  default: ASSERT(0,EVDOMAIN);
}}
