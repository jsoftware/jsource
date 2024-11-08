/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Private Jsoftware encode/decode routines - license keys and ijl         */

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

#include "j.h"

#define max(a,b)    (((a) > (b)) ? (a) : (b))

#define SK    133
#define LOCKEXTRA  5
#define SERIALNUMSIZE 32
#define lobyte(w)  ((UC)(w))

#if !(SYS & SYS_LILENDIAN)
int swapint(int n){C* p,c;
 p=(C*)&n;
 c=p[3];p[3]=p[0];p[0]=c;
 c=p[2];p[2]=p[1];p[1]=c;
 R n;
}
#endif

F1(jtlock1){A z; C* p; C* src;
 UC c,c1,c2,k1[SK],k2[SK];    
 int r,len,xlen,maxc1,maxc2,i,j,k;

 ARGCHK1(w);
 ASSERT(1==AR(w),EVRANK);
 ASSERT(AT(w)&LIT,EVDOMAIN);
 src=UAV(w);
 len=(int)AN(w);
 xlen=len;
#if SY_WINCE
 {SYSTEMTIME st;GetLocalTime(&st);srand((UC)(st.wMilliseconds)); }
#else
 srand((UC)time(NULL));
#endif
 c1=lobyte(rand());
 c2=lobyte(rand());
 maxc1=max(33,c1);
 maxc2=max(33,c2);
 r=4+4+SK+LOCKEXTRA+maxc1+len+SK+maxc2+2*SERIALNUMSIZE; /* result len */
 GATV0(z,LIT,r,1);
 p=CAV1(z);
#if !(SYS & SYS_LILENDIAN)
 xlen=swapint(len);
#endif
 for(i=0;i<SK;++i) k1[i] = lobyte(rand());
 for(i=0;i<SK;++i) k2[i] = lobyte(rand());
 *p++=(UC)255; *p++=0; *p++=c1; *p++=c2;
 for(i=0;i<4;++i) *p++= k1[i] ^ *(i+(UC*)&xlen);
 MC(p,k1,SK);
 p+=SK;
 k=LOCKEXTRA+maxc1;
 for(i=0;i<k;++i) *p++= lobyte(rand());
 j=0;
 for(i=0;i<len;++i)
 {
  c=*src++;
  c= c^k1[j];
  if(++j==SK) j=0;
  *p++=c;
 }
 MC(p,k2,SK);
 p+=SK;
 for(i=0;i<maxc2;++i) *p++= lobyte(rand());
 for(i=0;i<2*SERIALNUMSIZE;++i) *p++= lobyte(rand());
 R z;
}

F2(jtlock2){ASSERT(0,EVDOMAIN);}

F1(jtunlock1){R unlock2(mtv,w);}

F2(jtunlock2){int i,j,len,tlen;UC c1,c2,k1[SK],*lp,*sp,*d;
 ARGCHK2(a,w);  
 d=UAV(w);
 tlen=(int)AN(w);
 if(!tlen || 255!=d[0] || 0 != d[1] || tlen<8+SK) return w; /* not jl */
 MC(k1, d+8, SK);
 for(i=0;i<(int)sizeof(int);++i) *(i+(UC*)&len) = k1[i] ^ d[4+i];
#if !(SYS & SYS_LILENDIAN)
 len=swapint(len);
#endif
 c1 = max(33,d[2]);
 c2 = max(33,d[3]);
 if(!tlen == (8+LOCKEXTRA+c1+c2+len+ 2*SK + 2*SERIALNUMSIZE)) return w; /* not jl */
 lp = d+8+SK+c1+LOCKEXTRA;
 sp = d;
 j=0;
 for(i=0;i<len;++i)
 {
  *sp++ = *lp++ ^ k1[j];
  if(++j==SK) j=0;
 }
 while(sp<d+tlen)*sp++=' ';
 R w;
}    /* ignores left argument */
