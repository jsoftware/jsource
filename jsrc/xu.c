/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: u: conversions                                                   */

#include <wchar.h>

#include "j.h"
#include "x.h"
#include "cpuinfo.h"

#if C_AVX512
typedef short int char16_t;
extern size_t utf16le_to_utf8_avx512(unsigned char out[restrict], const char16_t in[restrict], size_t len, size_t *outlen);
extern size_t utf8_to_utf16le_avx512(char16_t out[restrict], const unsigned char in[restrict], size_t len, size_t *outlen);
#endif

// utf-8 to c2v - assumes valid utf-8 data and snk of right size
void mtow(UC* src, I srcn, US* snk){ US c,c1,c2,c3; UINT t;
  while (srcn--)
  {
    c=*src++;
    if(c<0x80)
    {
      *snk++=c;
    }
    else if(c<=0xc1||c>=0xf5)
    {
      /* ignore */
      *snk++=c;
    }
    else if(!srcn)
    {
      /* ignore */
      *snk++=c;
    }
    else if(c<0xe0)
    {
// 2 utf8
      c1=*src;
      if((c1&0xc0)==0x80)
      {
        t=((c&0x1f)<<6)|(c1&0x3f);
        *snk++=(US)t;
        src++;srcn--;
      }
      else
      {
        /* ignore */
        *snk++=c;
      }
    }
    else if(srcn<2)
    {
      /* ignore */
      *snk++=c;
    }
    else if(c<0xf0)
    {
// 3 utf8
      c1=src[0];
      c2=src[1];
      if((c1&0xc0)==0x80&&(c2&0xc0)==0x80&&(c>=0xe1||c1>=0xa0))
      {
        t=((c&0x0f)<<12)|((c1&0x3f)<<6)|(c2&0x3f);
        *snk++=(US)t;
        src+=2;srcn-=2;
      }
      else
      {
        /* ignore */
        *snk++=c;
      }
    }
    else if(srcn<3)
    {
      /* ignore */
      *snk++=c;
    }
    else if(c<0xf5)
    {
// 4 utf8
      c1=*src;
      c2=src[1];
      c3=src[2];
      if((c1&0xc0)==0x80&&(c2&0xc0)==0x80&&(c3&0xc0)==0x80&&(c>=0xf1||c1>=0x90))
      {
        t=((c&0x07)<<18)|((c1&0x3f)<<12)|((c2&0x3f)<<6)|(c3&0x3f);
        if(t>=0x110000)
        {
          /* ignore */
          *snk++=c;
        }
        else
        {
          if(t>=0x10000)
          {
            t-=0x10000;
            *snk++=0xd800|((t>>10)&0x3ff);
            t=0xdc00|(t&0x3ff);
          }
          *snk++=(US)t;
          src+=3;srcn-=3;
        }
      }
      else
      {
        /* ignore */
        *snk++=c;
      }
    }
    else
    {
      /* ignore */
      *snk++=c;
    }
  }
}

// get size of conversion from utf-8 to c2v
// return negative if utf-8 invalid
I mtowsize(UC* src, I srcn){ US c,c1,c2,c3;UINT t;I r=0;int invalid=0;
  while (srcn--)
  {
    c=*src++;
    if(c<0x80)
    {
      r++;
    }
    else if(c<=0xc1||c>=0xf5)
    {
      /* ignore */
      r++;invalid=1;
    }
    else if(!srcn)
    {
      /* ignore */
      r++;invalid=1;
    }
    else if(c<0xe0)
    {
// 2 utf8
      c1=*src;
      if((c1&0xc0)==0x80)
      {
        src++;srcn--;
        r++;
      }
      else
      {
        /* ignore */
        r++;invalid=1;
      }
    }
    else if(srcn<2)
    {
      /* ignore */
      r++;invalid=1;
    }
    else if(c<0xf0)
    {
// 3 utf8
      c1=src[0];
      c2=src[1];
      if((c1&0xc0)==0x80&&(c2&0xc0)==0x80&&(c>=0xe1||c1>=0xa0))
      {
        src+=2;srcn-=2;
        r++;
      }
      else
      {
        /* ignore */
        r++;invalid=1;
      }
    }
    else if(srcn<3)
    {
      /* ignore */
      r++;invalid=1;
    }
    else if(c<0xf5)
    {
// 4 utf8
      c1=src[0];
      c2=src[1];
      c3=src[2];
      if((c1&0xc0)==0x80&&(c2&0xc0)==0x80&&(c3&0xc0)==0x80&&(c>=0xf1||c1>=0x90))
      {
        t=((c&0x07)<<18)|((c1&0x3f)<<12)|((c2&0x3f)<<6)|(c3&0x3f);
        if(t>=0x110000)
        {
         /* ignore */
         r++;invalid=1;
        }
        else
        {
         if(t>=0x10000)
         {
           r++;
         }
         src+=3;srcn-=3;
         r++;
        }
      }
      else
      {
        /* ignore */
        r++;invalid=1;
      }
    }
    else
    {
      /* ignore */
      r++;invalid=1;
    }
  }
 R (invalid)?(-r):r;
}

// utf-8 to c4v - assumes valid utf-8 data and snk of right size
void mtou(UC* src, I srcn, C4* snk){ US c,c1,c2,c3; UINT t;
  while (srcn--)
  {
    c=*src++;
    if(c<0x80)
    {
      *snk++=c;
    }
    else if(c<=0xc1||c>=0xf5)
    {
      /* ignore */
      *snk++=c;
    }
    else if(!srcn)
    {
      /* ignore */
      *snk++=c;
    }
    else if(c<0xe0)
    {
// 2 utf8
      c1=*src;
      if((c1&0xc0)==0x80)
      {
        t=((c&0x1f)<<6)|(c1&0x3f);
        *snk++=(C4)t;
        src++;srcn--;
      }
      else
      {
        /* ignore */
        *snk++=c;
      }
    }
    else if(srcn<2)
    {
      /* ignore */
      *snk++=c;
    }
    else if(c<0xf0)
    {
// 3 utf8
      c1=src[0];
      c2=src[1];
      if((c1&0xc0)==0x80&&(c2&0xc0)==0x80&&(c>=0xe1||c1>=0xa0))
      {
        t=((c&0x0f)<<12)|((c1&0x3f)<<6)|(c2&0x3f);
        *snk++=(C4)t;
        src+=2;srcn-=2;
      }
      else
      {
        /* ignore */
        *snk++=c;
      }
    }
    else if(srcn<3)
    {
      /* ignore */
      *snk++=c;
    }
    else if(c<0xf5)
    {
// 4 utf8
      c1=src[0];
      c2=src[1];
      c3=src[2];
      if((c1&0xc0)==0x80&&(c2&0xc0)==0x80&&(c3&0xc0)==0x80&&(c>=0xf1||c1>=0x90))
      {
        t=((c&0x07)<<18)|((c1&0x3f)<<12)|((c2&0x3f)<<6)|(c3&0x3f);
        if(t>=0x110000)
        {
          /* ignore */
          *snk++=c;
        }
        else
        {
          *snk++=(C4)t;
          src+=3;srcn-=3;
        }
      }
      else
      {
        /* ignore */
        *snk++=c;
      }
    }
    else
    {
      /* ignore */
      *snk++=c;
    }
  }
}

// get size of conversion from utf-8 to c4v
// return negative if utf-8 invalid
I mtousize(UC* src, I srcn){ US c,c1,c2,c3;UINT t;I r=0;int invalid=0;
  while (srcn--)
  {
    c=*src++;
    if(c<0x80)
    {
      r++;
    }
    else if(c<=0xc1||c>=0xf5)
    {
      /* ignore */
      r++;invalid=1;
    }
    else if(!srcn)
    {
      /* ignore */
      r++;invalid=1;
    }
    else if(c<0xe0)
    {
// 2 utf8
      c1=*src;
      if((c1&0xc0)==0x80)
      {
        src++;srcn--;
        r++;
      }
      else
      {
        /* ignore */
        r++;invalid=1;
      }
    }
    else if(srcn<2)
    {
      /* ignore */
      r++;invalid=1;
    }
    else if(c<0xf0)
    {
// 3 utf8
      c1=src[0];
      c2=src[1];
      if((c1&0xc0)==0x80&&(c2&0xc0)==0x80&&(c>=0xe1||c1>=0xa0))
      {
        src+=2;srcn-=2;
        r++;
      }
      else
      {
        /* ignore */
        r++;invalid=1;
      }
    }
    else if(srcn<3)
    {
      /* ignore */
      r++;invalid=1;
    }
    else if(c<0xf5)
    {
// 4 utf8
      c1=src[0];
      c2=src[1];
      c3=src[2];
      if((c1&0xc0)==0x80&&(c2&0xc0)==0x80&&(c3&0xc0)==0x80&&(c>=0xf1||c1>=0x90))
      {
        t=((c&0x07)<<18)|((c1&0x3f)<<12)|((c2&0x3f)<<6)|(c3&0x3f);
        if(t>=0x110000)
        {
         /* ignore */
         r++;invalid=1;
        }
        else
        {
         src+=3;srcn-=3;
         r++;
        }
      }
      else
      {
        /* ignore */
        r++;invalid=1;
      }
    }
    else
    {
      /* ignore */
      r++;invalid=1;
    }
  }
 R (invalid)?(-r):r;
}

// c2v to utf-8 - assume valid data and snk size is ok
// This version seems to be referred to externally, but I can't see where from
void wtom(US* src, I srcn, UC* snk){ US w,w1; UINT t;
 while(srcn--)
 {
  w=*src++;
  if(w<=0x7f)
   *snk++=(UC)w;
  else if(w<=0x7ff)
  {
   *snk++=0xc0|(w>>6);
   *snk++=0x80|(0x3f&w);
  }
  else if((w>=0x800&&w<=0xd7ff)||(w>=0xe000))
  {
   *snk++=0xe0|w>>12;
   *snk++=0x80|(0x3f&(w>>6));
   *snk++=0x80|(0x3f&w);
  }
  else
  {
   if(!srcn)
   {
    // isolated surrogate
    *snk++=0xe0|w>>12;
    *snk++=0x80|(0x3f&(w>>6));
    *snk++=0x80|(0x3f&w);
   }
   else
   {
    w1=*src;
    if(w>=0xdc00||w1<=0xdbff||w1>=0xe000) // incorrect high/low surrogate
    {
     *snk++=0xe0|(0x0f&(w>>12));
     *snk++=0x80|(0x3f&(w>>6));
     *snk++=0x80|(0x3f&w);
    }
    else
    {
     t=(((w&0x3ff)<<10)|(w1&0x3ff))+0x10000;
     *snk++=0xf0|((t>>18)&0x07);
     *snk++=0x80|((t>>12)&0x3f);
     *snk++=0x80|((t>>6)&0x3f);
     *snk++=0x80|(t&0x3f);
     src++;srcn--;  // next code unit of surrogate pair
    }
   }
  }
 }
}

// get size of conversion from c2v to utf-8
// return negative if utf-16 invalid
I wtomsize(US* src, I srcn){ US w,w1;I r=0;int invalid=0;
 while(srcn--)
 {
  w=*src++;
  if(w<=0x7f)
   ++r;
  else if(w<=0x7ff)
   r+=2;
  else if((w>=0x800&&w<=0xd7ff)||(w>=0xe000))
   r+=3;
  else
  {
   if(!srcn)
   {
    // isolated surrogate
    r+=3;invalid=1;
   }
   else
   {
    w1=*src;
    if(w>=0xdc00||w1<=0xdbff||w1>=0xe000)
    {
     // incorrect high/low surrogate
     r+=3;invalid=1;
    }
    else
    {
     r+=4;
     src++;srcn--;  // skip the next code unit of surrogate pair
    }
   }
  }
 }
 R (invalid)?(-r):r;
}

// c2v to utf-32 - assume valid data and snk size is ok
// This version seems to be referred to externally, but I can't see where from
void wtou(US* src, I srcn, C4* snk){ US w,w1;
 while(srcn--)
 {
  w=*src++;
  if(w<=0xd7ff||(w>=0xe000))
  {
   *snk++=(C4)w;
  }
  else
  {
   if(!srcn)
   {
    // isolated surrogate
   *snk++=(C4)w;
   }
   else
   {
    w1=*src;
    if(w>=0xdc00||w1<=0xdbff||w1>=0xe000) // incorrect high/low surrogate
    {
     *snk++=(C4)w;
    }
    else
    {
     *snk++=(C4)(((w&0x3ff)<<10)|(w1&0x3ff))+0x10000;
     src++;srcn--;  // next code unit of surrogate pair
    }
   }
  }
 }
}

// get size of conversion from c2v to utf-32
// return negative if utf-16 invalid
I wtousize(US* src, I srcn){ US w,w1;I r=0;int invalid=0;
 while(srcn--)
 {
  w=*src++;
  if(w<=0xd7ff||(w>=0xe000))
   ++r;
  else
  {
   if(!srcn)
   {
    // isolated surrogate
    r+=1;invalid=1;
   }
   else
   {
    w1=*src;
    if(w>=0xdc00||w1<=0xdbff||w1>=0xe000)
    {
     // incorrect high/low surrogate
     r+=1;invalid=1;
    }
    else
    {
     r+=1;
     src++;srcn--;  // skip the next code unit of surrogate pair
    }
   }
  }
 }
 R (invalid)?(-r):r;
}

// c4v to utf-8 - assume valid data and snk size is ok
// This version seems to be referred to externally, but I can't see where from
void utom(C4* src, I srcn, UC* snk){ C4 w;
 while(srcn--)
 {
  w=*src++;
  if(w<=0x7f)
   *snk++=(UC)w;
  else if(w<=0x7ff)
  {
   *snk++=0xc0|(w>>6);
   *snk++=0x80|(0x3f&w);
  }
  else if(BETWEENC(w,0x800,0xffff)&&!BETWEENC(w,0xd800,0xdfff))
  {
   *snk++=0xe0|w>>12;
   *snk++=0x80|(0x3f&(w>>6));
   *snk++=0x80|(0x3f&w);
  }
  else if(BETWEENC(w,0xd800,0xdfff)) // utf-16 surrogate invalid in utf-32
  {
   *snk++=0xe0|w>>12;
   *snk++=0x80|(0x3f&(w>>6));
   *snk++=0x80|(0x3f&w);
  }
  else if(w>0x10ffff)          // invalid range
  {
   // demoted to 1-byte. utf8 atmost 4 bytes
   *snk++=(UC)w;
  }
  else
  {
     *snk++=0xf0|((w>>18)&0x07);
     *snk++=0x80|((w>>12)&0x3f);
     *snk++=0x80|((w>>6)&0x3f);
     *snk++=0x80|(w&0x3f);
  }
 }
}

// get size of conversion from c4v to utf-8
// return negative if utf-32 invalid
I utomsize(C4* src, I srcn){ C4 w;I r=0;int invalid=0;
 while(srcn--)
 {
  w=*src++;
  if(w<=0x7f)
   ++r;
  else if(w<=0x7ff)
   r+=2;
  else if(BETWEENC(w,0x800,0xffff)&&!BETWEENC(w,0xd800,0xdfff))
   r+=3;
  else if(BETWEENC(w,0xd800,0xdfff)) // utf-16 surrogate invalid in utf-32
  {r+=3;invalid=1;}
  else if(w>0x10ffff)          // invalid range
  {++r;invalid=1;}             // demoted to 1-byte. utf8 atmost 4 bytes
  else
   r+=4;
 }
 R (invalid)?(-r):r;
}

// c4v to utf-16 - assume valid data and snk size is ok
// This version seems to be referred to externally, but I can't see where from
void utow(C4* src, I srcn, US* snk){ C4 w;
 while(srcn--)
 {
  w=*src++;
  if(w>0x10ffff) // invalid range
  {
   // demoted to 2-byte.
   *snk++=(US)w;
  }
  else
  {
   if(w>=0x10000)
   {
    w-=0x10000;
    *snk++=0xd800|((w>>10)&0x3ff);
    w=0xdc00|(w&0x3ff);
   }
   *snk++=(US)w;
  }
 }
}

// get size of conversion from c4v to utf-16
// return negative if utf-32 invalid
I utowsize(C4* src, I srcn){ C4 w;I r=0;int invalid=0;
 while(srcn--)
 {
  w=*src++;
  if(w>0x10ffff) // invalid range
  {
    r+=1;
    invalid=1; // demoted to 2-byte.
  }
  else
  {
   if(w>=0x10000)
    r+=2;
   else if(BETWEENC(w,0xd800,0xdfff)) // utf-16 surrogate invalid in utf-32
    {r+=1; invalid=1;}
   else
    r+=1;
  }
 }
 R (invalid)?(-r):r;
}

// convert surrogate pair in C4T
void utou(C4* src, I srcn, C4* snk){ C4 w,w1;
 while(srcn--)
 {
  w=*src++;
  if(!BETWEENC(w,0xd800,0xdfff))
  {
   *snk++=(C4)w;
  }
  else
  {
   if(!srcn)
   {
    // isolated surrogate
   *snk++=(C4)w;
   }
   else
   {
    w1=*src;
    if(w>=0xdc00||!BETWEENC(w1,0xdc00,0xdfff)) // incorrect high/low surrogate
    {
     *snk++=(C4)w;
    }
    else
    {
     *snk++=(C4)(((w&0x3ff)<<10)|(w1&0x3ff))+0x10000;
     src++;srcn--;  // next code unit of surrogate pair
    }
   }
  }
 }
}

// convert surrogate pair in C4T
I utousize(C4* src, I srcn){ C4 w,w1; I r=0;
 while(srcn--)
 {
  w=*src++;
  if(w<=0xd7ff||(w>=0xe000))
  {
   r++;
  }
  else
  {
   if(!srcn)
   {
    // isolated surrogate
   r++;
   }
   else
   {
    w1=*src;
    if(w>=0xdc00||!BETWEENC(w1,0xdc00,0xdfff)) // incorrect high/low surrogate
    {
      r++;
    }
    else
    {
     r++;  // NOT  r+=2
     src++;srcn--;  // next code unit of surrogate pair
    }
   }
  }
 }
 R r;
}

F1(jttoutf16){A z;I n,n1,t,q,b=0,j; UC* wv; US* c2v; C4* c4v; A c4; I *v;
 PROLOG(0000);
 ARGCHK1(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w); wv=UAV(w);
 if(!n) {GATV(z,LIT,n,AR(w),AS(w)); R z;}; // empty lit list
 ASSERT(t&(NUMERIC+JCHAR), EVDOMAIN);
 if(NUMERIC&t) // u16 from int
 {
  RZ(w=vi(w));
  n=AN(w); v=(I*)AV(w);
  GATV0(c4,C4T,n,1); c4v=C4AV1(c4);
  DQ(n, j=*v++; ASSERT(BETWEENC(j,0,0x10ffff),EVINDEX); *c4v++=(C4)j;);
  q=utowsize(C4AV(c4),AN(c4));
  q=(q<0)?(-q):q;   // allow unpaired surrogate as in 10&u:
  GATV0(z,C2T,q,1);
  utow(C4AV(c4),AN(c4),USAV1(z));
 }
 else if(LIT&t) // u16 from u8
 {
  DQ(n, if(127<*wv++){b=1;break;});
  if(!b)RCA(w);  // ascii list unchanged ascii scalar as list
  q=mtowsize(UAV(w),n);
  ASSERT(q>=0,EVDOMAIN);
  GATV0(z,C2T,q,1);
#if C_AVX512
  if(getCpuFeatures()&CPU_X86_FEATURE_AVX512VBMI2)
   utf8_to_utf16le_avx512(USAV1(z),UAV(w),(size_t)n,(size_t*)&n1);
  else
   mtow(UAV(w),n,USAV1(z));
#else
  mtow(UAV(w),n,USAV1(z));
#endif
 }
 else if(C2T&t)
 {
  c2v=USAV(w);
  DQ(n, if(127<*c2v++){b=1;break;});
  if(b)RCA(w); // u16 unchanged
  GATV0(z,LIT,n,1);
  wv=UAV1(z);
  c2v=USAV(w);
  DQ(n, *wv++=(UC)*c2v++;);   // fallback to ascii
 }
 else
 {
  c4v=C4AV(w);
  DQ(n, if(127<*c4v++){b=1;break;});
  if(b){
  q=utowsize(C4AV(w),n);
  ASSERT(q>=0,EVDOMAIN);
  GATV0(z,C2T,q,1);
  utow(C4AV(w),n,USAV1(z));
  }
  else
  {
  GATV0(z,LIT,n,1);
  wv=UAV1(z);
  c4v=C4AV(w);
  DQ(n, *wv++=(char)*c4v++;);   // fallback to ascii
  }
 }
 EPILOG(z);
}    // 7 u: x - utf16 from LIT or C2T C4T

// Similar to jttoutf8, but allow invalid unicode
// w is C2T C4T or LIT.  Result is U8 string
A jttoutf8a(J jt,A w,A prxthornuni){A z;I n,n1,t,q;  // prxthornuni is unused
PROLOG(0000);
ARGCHK1(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w);
if(!n) {GATV(z,LIT,n,AR(w),AS(w)); R z;}; // empty lit list
if(t&LIT)RCA(w); // char unchanged
ASSERT(t&(C2T+C4T), EVDOMAIN);
if(t&C2T)
{
q=wtomsize(USAV(w),n);
q=(q<0)?(-q):q;
GATV0(z,LIT,q,1);
#if C_AVX512
if(getCpuFeatures()&CPU_X86_FEATURE_AVX512VBMI2)
 utf16le_to_utf8_avx512(UAV1(z),USAV(w),(size_t)n,(size_t*)&n1);
else
 wtom(USAV(w),n,UAV1(z));
#else
wtom(USAV(w),n,UAV1(z));
#endif
}
else
{
q=utomsize(C4AV(w),n);
q=(q<0)?(-q):q;
GATV0(z,LIT,q,1);
utom(C4AV(w),n,UAV1(z));
}
EPILOG(z);
}    // called by monad ":

// w is C2T C4T or LIT INT.  Result is U8 string
F1(jttoutf8){A z;I n,n1,t,q,j; A c4; C4* c4v; I *v;
PROLOG(0000);
ARGCHK1(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w);
if(!n) {GATV(z,LIT,n,AR(w),AS(w)); R z;}; // empty lit list
if(t&LIT)RCA(w); // char unchanged
ASSERT(t&(NUMERIC+JCHAR), EVDOMAIN);
if(NUMERIC&t)
{
RZ(w=vi(w));
n=AN(w); v=(I*)AV(w);
GATV0(c4,C4T,n,1); c4v=C4AV1(c4);
DQ(n, j=*v++; ASSERT((UI)j<=(UI)0x10ffff,EVINDEX); *c4v++=(C4)j;);
q=utomsize(C4AV(c4),AN(c4));
q=(q<0)?(-q):q;   // allow unpaired surrogate as in 10&u:
GATV0(z,LIT,q,1);
utom(C4AV(c4),AN(c4),UAV1(z));
}
else if(t&C4T)
{
q=utomsize(C4AV(w),n);
ASSERT(q>=0,EVDOMAIN);
GATV0(z,LIT,q,1);
utom(C4AV(w),n,UAV1(z));
}
else
{
q=wtomsize(USAV(w),n);
ASSERT(q>=0,EVDOMAIN);
GATV0(z,LIT,q,1);
#if C_AVX512
if(getCpuFeatures()&CPU_X86_FEATURE_AVX512VBMI2)
 utf16le_to_utf8_avx512(UAV1(z),USAV(w),(size_t)n,(size_t*)&n1);
else
 wtom(USAV(w),n,UAV1(z));
#else
wtom(USAV(w),n,UAV1(z));
#endif
}
EPILOG(z);
}    // 8 u: x - utf8 from LIT or C2T C4T

// Similar to jttoutf16, but unlike 7&u: this one always returns unicode
F1(jttoutf16x){A z;I n,n1,t,q;
PROLOG(0000);
ARGCHK1(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w);
if(!n) {GATV(z,C2T,n,AR(w),AS(w)); R z;}; // empty list
if(t&C2T)RCA(w);
ASSERT(t&LIT+C4T,EVDOMAIN);
if(t&C4T)
{
q=utowsize(C4AV(w),n);
ASSERT(q>=0,EVDOMAIN);
GATV0(z,C2T,q,1);
utow(C4AV(w),n,USAV1(z));
}
else // u16 from u8
{
q=mtowsize(UAV(w),n);
ASSERT(q>=0,EVDOMAIN);
GATV0(z,C2T,q,1);
#if C_AVX512
if(getCpuFeatures()&CPU_X86_FEATURE_AVX512VBMI2)
 utf8_to_utf16le_avx512(USAV1(z),UAV(w),(size_t)n,(size_t*)&n1);
else
 mtow(UAV(w),n,USAV1(z));
#else
mtow(UAV(w),n,USAV1(z));
#endif
}
EPILOG(z);
}

// Similar to jttoutf8a, allow invalid unicode
// w is C2T C4T or LIT.  Result is U8 string and null-terminate
F1(jttoutf8x){
R str0(toutf8a(w,0));
}

// External function - just convert wide-char fw[] to U8 in f[], and null-terminate
void jttoutf8w(J jt,C* f, I n, US* fw){I q;I n1;
q=wtomsize(fw,wcslen((wchar_t*)fw));
q=(q<0)?(-q):q;
#if C_AVX512
if(getCpuFeatures()&CPU_X86_FEATURE_AVX512VBMI2)
 utf16le_to_utf8_avx512(f,fw,(size_t)wcslen((wchar_t*)fw),(size_t*)&n1);
else
 wtom(fw,wcslen((wchar_t*)fw),f);
#else
wtom(fw,wcslen((wchar_t*)fw),f);
#endif
f[q]=0;
}

F1(jttoutf32){A z;I n,t,q,b=0,j; UC* wv; US* c2v; C4* c4v; I* v;
 PROLOG(0000);
 ARGCHK1(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w); wv=UAV(w);
 if(!n) {GATV(z,LIT,n,AR(w),AS(w)); R z;}; // empty lit list
 ASSERT(t&(NUMERIC+JCHAR), EVDOMAIN);
 if(NUMERIC&t) // u32 from int
 {
  RZ(w=vi(w));
  n=AN(w); v=(I*)AV(w);
  I zr=AR(w); GATV(z,C4T,n,AR(w),AS(w)); c4v=C4AVn(zr,z);
  DQ(n, j=*v++; *c4v++=(C4)(UI)j;);
 }
 else if(LIT&t) // u32 from u8
 {
  DQ(n, if(127<*wv++){b=1;break;});
  if(!b)RCA(w);  // ascii list unchanged ascii scalar as list
  q=mtousize(UAV(w),n);
  ASSERT(q>=0,EVDOMAIN);
  GATV0(z,C4T,q,1);
  mtou(UAV(w),n,C4AV1(z));
 }
 else if(C2T&t)
 {
  c2v=USAV(w);
  DQ(n, if(127<*c2v++){b=1;break;});
  if(b){
  q=wtousize(USAV(w),n);
  ASSERT(q>=0,EVDOMAIN);
  GATV0(z,C4T,q,1);
  wtou(USAV(w),n,C4AV1(z));
  }
  else
  {
  GATV0(z,LIT,n,1);
  wv=UAV1(z);
  c2v=USAV(w);
  DQ(n, *wv++=(char)*c2v++;);
  }
 }
 else
 {
  c4v=C4AV(w);
  DQ(n, if(127<*c4v++){b=1;break;});
  if(b){
   q=utousize(C4AV(w),n);
   GATV0(z,C4T,q,1);
   utou(C4AV(w),n,C4AV1(z));
  }
  else
  {
  GATV0(z,LIT,n,AR(w));
  wv=UAV(z);
  c4v=C4AV(w);
  DQ(n, *wv++=(UC)*c4v++;);
  }
 }
 EPILOG(z);
}    // 9 u: x - utf32 from INT UTF-8 or UTF-16


// similar to jttoutf32 but result is always unicode and
// illegal utf-16 is allowed
// literal is NOT interpreted as utf-8.
// rank is infinit
F1(jttou32){A z;I n,t,b=0,j; US* c2v; C4* c4v; I* v; UC* c1v;
 PROLOG(0000);
 ARGCHK1(w); n=AN(w); t=AT(w);
 if(C4T&AT(w))RCA(w);  // if already C4T, return
 ASSERT(!n||(NUMERIC+JCHAR)&AT(w),EVDOMAIN);  // must be empty or unicode
 if(!n) {GATV(z,C4T,n,AR(w),AS(w)); R z;}; // empty list
 if(NUMERIC&t)
 {
  RZ(w=vi(w));
  n=AN(w); v=(I*)AV(w);
  I zr=AR(w); GATV(z,C4T,n,AR(w),AS(w)); c4v=C4AVn(zr,z);
  DQ(n, j=*v++; *c4v++=(C4)(UI)j;);
 }
 else if(LIT&t)
 {
  GATV(z,C4T,n,AR(w),AS(w));
  c4v=C4AV(z);
  c1v=(UC*)UAV(w);
  DQ(n, *c4v++=(C4)*c1v++;);
 }
 else
 {
  GATV(z,C4T,n,AR(w),AS(w));
  c4v=C4AV(z);
  c2v=USAV(w);
  DQ(n, *c4v++=(C4)*c2v++;);
 }
 EPILOG(z);
}    // 10 u: x - literal4 similar to monad u: for whcar

// cnull 0: cesu-8  1: modified utf-8
static A tocesu8a(J jt, A w, I cnul) {A z,z1; UC* wv=UAV(w); I n,n1,t,q; C4* c4v; US* c2v;
PROLOG(0000);
ARGCHK1(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w);
if(!n) {GATV(z,LIT,n,AR(w),AS(w)); R z;}; // empty list
ASSERT(t&LIT,EVDOMAIN);
// convert to utf-16
q=mtowsize(wv,n);
if(q<0)RCA(w);
GATV0(z,C2T,q,1);
c2v=USAV1(z);
#if C_AVX512
if(getCpuFeatures()&CPU_X86_FEATURE_AVX512VBMI2)
 utf8_to_utf16le_avx512(c2v,wv,(size_t)n,(size_t*)&n1);
else
 mtow(wv,n,c2v);
#else
mtow(wv,n,c2v);
#endif
// promote wchar to literal4
// change 0 to its over long version
GATV0(z1,C4T,q,1);
c4v=C4AV1(z1);
if(cnul){
 DQ(q, if(*c2v)*c4v++=(C4)*c2v++;else{*c4v++=(C4)0xc080;c2v++;});
}else{
 DQ(q, *c4v++=(C4)*c2v++;);
}
// convert to utf-8
q=utomsize(C4AV1(z1),AN(z1));
q=(q<0)?(-q):q;
GATV0(z,LIT,q,1);
utom(C4AV1(z1),AN(z1),UAV1(z));
EPILOG(z);
}

// to modified utf-8 assume input is rank-1 LIT
// nul converted 0xc080
F1(jttomutf8){R tocesu8a(jt,w,1);}

// to cesu-8 assume input is rank-1 LIT
// to convert backwards, first to utf-16 then to utf-8
F1(jttocesu8){R tocesu8a(jt,w,0);}

