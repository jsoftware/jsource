/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: u: conversions                                                   */

#include <wchar.h>

#include "j.h"
#include "x.h"

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
      c1=*src;
      c2=*(src+1);
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
      c2=*(src+1);
      c3=*(src+2);
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
      c1=*src;
      c2=*(src+1);
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
      c1=*src;
      c2=*(src+1);
      c3=*(src+2);
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
      c1=*src;
      c2=*(src+1);
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
      c1=*src;
      c2=*(src+1);
      c3=*(src+2);
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
      c1=*src;
      c2=*(src+1);
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
      c1=*src;
      c2=*(src+1);
      c3=*(src+2);
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
  else if((w>=0x800&&w<=0xd7ff)||(w>=0xe000&&w<=0xffff))
  {
   *snk++=0xe0|w>>12;
   *snk++=0x80|(0x3f&(w>>6));
   *snk++=0x80|(0x3f&w);
  }
  else if(w>=0xd800&&w<=0xdfff) // utf-16 surrogate invalid in utf-32
  {
   *snk++=0xe0|w>>12;
   *snk++=0x80|(0x3f&(w>>6));
   *snk++=0x80|(0x3f&w);
  }
  else if(w>0x10ffff)          // invalid range
  {
     // ignored, utf8 atmost 4 bytes
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
  else if((w>=0x800&&w<=0xd7ff)||(w>=0xe000&&w<=0xffff))
   r+=3;
  else if(w>=0xd800&&w<=0xdfff) // utf-16 surrogate invalid in utf-32
  {r+=3;invalid=1;}
  else if(w>0x10ffff)          // invalid range
  {invalid=1;}                 // ignored, utf8 atmost 4 bytes
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
   // ignored
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
   invalid=1; // ignored
  else
  {
   if(w>=0x10000)
    r+=2;
   else if(w>=0xd800&&w<=0xdfff) // utf-16 surrogate invalid in utf-32
    {r+=1; invalid=1;}
   else
    r+=1;
  }
 }
 R (invalid)?(-r):r;
}

F1(jttoutf16){A z;I n,t,q,b=0,j; C* wv; US* c2v; C4* c4v; A c4; I *v;
 RZ(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w); wv=CAV(w);
 if(!n) {GATV(z,LIT,n,1,0); R z;}; // empty lit list 
 if(t&(B01+INT))
 {
  RZ(w=vi(w));
  n=AN(w); v=(I*)AV(w);
  GATV(c4,C4T,n,AR(w),0); c4v=(C4*)AV(c4);
  DO(n, j=*v++; ASSERT(0<=j&&j<=0x10ffff,EVINDEX); *c4v++=(C4)j;);
  q=utowsize((C4*)CAV(c4),AN(c4));
  q=(q<0)?(-q):q;   // allow unpaired surrogate as in 10&u:
  GATV(z,C2T,q,1,0);
  utow((C4*)CAV(c4),AN(c4),(US*)CAV(z));
  R z; // u32 from u8
 }
 else if(LIT&t)
 {
  DO(n, if(0>*wv++){b=1;break;});
  if(!b){ if(1==AR(w)) {R ca(w);}; GAT(z,LIT,1,1,0); *CAV(z)=*CAV(w); R z;} // ascii list unchanged ascii scalar as list
  q=mtowsize(CAV(w),n);
  ASSERT(q>=0,EVDOMAIN);
  GATV(z,C2T,q,1,0);
  mtow(CAV(w),n,(US*)CAV(z));
  R z; // u16 from u8
 }
 else if(C2T&t)
 {
  c2v=(US*)AV(w);
  DO(n, if(127<*c2v++){b=1;break;});
  if(b) R ca(w); // u16 unchanged
  GATV(z,LIT,n,AR(w),0);
  wv=CAV(z);
  c2v=(US*)AV(w);
  DO(n, *wv++=(char)*c2v++;);
  R z;
 }
 else if(C4T&t)
 {
  c4v=(C4*)AV(w);
  DO(n, if(127<*c4v++){b=1;break;});
  if(b){
  q=utowsize((C4*)CAV(w),n);
  ASSERT(q>=0,EVDOMAIN);
  GATV(z,C2T,q,1,0);
  utow((C4*)CAV(w),n,(US*)CAV(z));
  R z;
  }
  else
  {
  GATV(z,LIT,n,AR(w),0);
  wv=CAV(z);
  c4v=(C4*)CAV(w);
  DO(n, *wv++=(char)*c4v++;);
  R z;
  }
 }
 else
  ASSERT(0, EVDOMAIN);
}    // 7 u: x - utf16 from LIT or C2T C4T

// Similar to jttoutf8, but allow invalid unicode
// w is C2T C4T or LIT.  Result is U8 string
F1(jttoutf8a){A z;I n,t,q;
RZ(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w);
if(!n) {GATV(z,LIT,n,AR(w),AS(w)); R z;}; // empty lit list
if(t&LIT) R ca(w); // char unchanged
ASSERT(t&(C2T+C4T), EVDOMAIN);
if(t&C2T)
{
q=wtomsize((US*)CAV(w),n);
q=(q<0)?(-q):q;
GATV(z,LIT,q,1,0);
// wtomnull((US*)CAV(w),n,CAV(z),jt->thornuni);  // If we inserted nulls after CJK, remove them.  This is only when original data was boxed
wtom((US*)CAV(w),n,CAV(z));
}
else
{
q=utomsize((C4*)CAV(w),n);
q=(q<0)?(-q):q;
GATV(z,LIT,q,1,0);
utom((C4*)CAV(w),n,CAV(z));
}
R z;
}    // called by monad ":

// w is C2T C4T or LIT INT.  Result is U8 string
F1(jttoutf8){A z;I n,t,q,j; A c4; C4* c4v; I *v;
RZ(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w);
if(!n) {GATV(z,LIT,n,AR(w),AS(w)); R z;}; // empty lit list
if(t&LIT) R ca(w); // char unchanged
ASSERT(t&(B01+INT+C2T+C4T), EVDOMAIN);
if(t&(B01+INT))
{
RZ(w=vi(w));
n=AN(w); v=(I*)AV(w);
GATV(c4,C4T,n,AR(w),0); c4v=(C4*)AV(c4);
DO(n, j=*v++; ASSERT(0<=j&&j<=0x10ffff,EVINDEX); *c4v++=(C4)j;);
q=utomsize((C4*)CAV(c4),AN(c4));
q=(q<0)?(-q):q;   // allow unpaired surrogate as in 10&u:
GATV(z,LIT,q,1,0);
utom((C4*)CAV(c4),AN(c4),CAV(z));
}
else if(t&C4T)
{
q=utomsize((C4*)CAV(w),n);
ASSERT(q>=0,EVDOMAIN);
GATV(z,LIT,q,1,0);
utom((C4*)CAV(w),n,CAV(z));
}
else
{
q=wtomsize((US*)CAV(w),n);
ASSERT(q>=0,EVDOMAIN);
GATV(z,LIT,q,1,0);
wtom((US*)CAV(w),n,CAV(z));
}
R z;
}    // 8 u: x - utf8 from LIT or C2T C4T

// Similar to jttoutf16, but unlike 7&u: this one always returns unicode
F1(jttoutf16x){I q;A z;
ASSERT((LIT+C4T)&AT(w),EVDOMAIN);
if(AT(w)&C4T)
{
q=utowsize((C4*)CAV(w),AN(w));
ASSERT(q>=0,EVDOMAIN);
GATV(z,C2T,q,1,0);
utow((C4*)CAV(w),AN(w),(US*)CAV(z));
}
else
{
q=mtowsize(CAV(w),AN(w));
ASSERT(q>=0,EVDOMAIN);
GATV(z,C2T,q,1,0);
mtow(CAV(w),AN(w),(US*)CAV(z));
}
R z; // u16 from u8
}

// External function - just convert wide-char fw[] to U8 in f[], and null-terminate 
void jttoutf8x(J jt,C* f, I n, US* fw){I q;
q=wtomsize(fw,wcslen((wchar_t*)fw));
q=(q<0)?(-q):q;
wtom(fw,wcslen((wchar_t*)fw),f);
f[q]=0;
}

F1(jttoutf32){A z;I n,t,q,b=0,j; C* wv; US* c2v; C4* c4v; I* v;
 RZ(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w); wv=CAV(w);
 if(!n) {GATV(z,LIT,n,1,0); R z;}; // empty lit list 
 if((B01+INT)&t)
 {
  RZ(w=vi(w));
  n=AN(w); v=(I*)AV(w);
  GATV(z,C4T,n,AR(w),0); c4v=(C4*)AV(z);
  DO(n, j=*v++; *c4v++=(C4)(UI)j;);
  R z; // u32 from int
 }
 else if(LIT&t)
 {
  DO(n, if(0>*wv++){b=1;break;});
  if(!b){ if(1==AR(w)) {R ca(w);}; GAT(z,LIT,1,1,0); *CAV(z)=*CAV(w); R z;} // ascii list unchanged ascii scalar as list
  q=mtousize(CAV(w),n);
  ASSERT(q>=0,EVDOMAIN);
  GATV(z,C4T,q,1,0);
  mtou(CAV(w),n,(C4*)CAV(z));
  R z; // u32 from u8
 }
 else if(C2T&t)
 {
  c2v=(US*)AV(w);
  DO(n, if(127<*c2v++){b=1;break;});
  if(b){
  q=wtousize((US*)CAV(w),n);
  ASSERT(q>=0,EVDOMAIN);
  GATV(z,C4T,q,1,0);
  wtou((US*)CAV(w),n,(C4*)CAV(z));
  R z;
  }
  else
  {
  GATV(z,LIT,n,AR(w),0);
  wv=CAV(z);
  c2v=(US*)AV(w);
  DO(n, *wv++=(char)*c2v++;);
  R z;
  }
 }
 else if(C4T&t)
 {
  c4v=(C4*)AV(w);
  DO(n, if(127<*c4v++){b=1;break;});
  if(b){
   R ca(w); // u32 unchanged
  }
  else
  {
  GATV(z,LIT,n,AR(w),0);
  wv=CAV(z);
  c4v=(C4*)CAV(w);
  DO(n, *wv++=(char)*c4v++;);
  R z;
  }
 }
 else
  ASSERT(0, EVDOMAIN);
}    // 9 u: x - utf32 from INT UTF-8 or UTF-16


// similar to jttoutf32 but result is always unicode and
// illegal utf-16 is allowed
// literal is NOT interpreted as utf-8.
// rank is infinit
F1(jttou32){A z;I n,t,b=0,j; C* wv; US* c2v; C4* c4v; I* v; UC* c1v;
 RZ(w); n=AN(w); t=AT(w); wv=CAV(w);
 if(C4T&AT(w))R ca(w);  // if already C4T, clone it and return the clone
 ASSERT(!n||(B01+INT+LIT+C2T)&AT(w),EVDOMAIN);  // must be empty or unicode
 if((B01+INT)&t)
 {
  RZ(w=vi(w));
  n=AN(w); v=(I*)AV(w);
  GATV(z,C4T,n,AR(w),AS(w)); c4v=(C4*)AV(z);
  DO(n, j=*v++; *c4v++=(C4)(UI)j;);
 }
 else if(LIT&t)
 {
  GATV(z,C4T,n,AR(w),AS(w));
  c4v=(C4*)CAV(z);
  c1v=(UC*)UAV(w);
  DO(n, *c4v++=(C4)*c1v++;);
 }
 else
 {
  GATV(z,C4T,n,AR(w),AS(w));
  c4v=(C4*)AV(z);
  c2v=(US*)CAV(w);
  DO(n, *c4v++=(C4)*c2v++;);
 }
 R z;
}    // 10 u: x - literal4 similar to monad u: for whcar

