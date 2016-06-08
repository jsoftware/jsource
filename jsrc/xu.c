/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: u: conversions                                                   */

#include "j.h"
#include "x.h"

// utf-8 to c2v - assumes valid utf-8 data and snk of right size
static void mtow(UC* src, I srcn, US* snk){ US c,c1,c2;
 while(srcn--)
 {
  c=*src++;
  if(c<=0x7f)
   *snk++=c;
  else if((c&0xE0)==0xC0)
  {
   c1=*src++;--srcn;
   *snk++=((c&0x1F)<<6)|(c1&0x3f);
  }
  else if((c&0xF0)==0xE0)
  {
    c1=*src++;--srcn;
    c2=*src++;--srcn;
    *snk++=((c&0x0F)<<12)|((c1&0x3F)<<6)|(c2&0x3f);
  }
 }
}

// get size of conversion from utf-8 to c2v
// return -1 if utf-8 invalid
static I mtowsize(UC* src, I srcn){ US c,c1,c2,cf; I r=0;
 while(srcn--)
 {
  c=*src++;
  if(c<=0x7f)
   ++r;
  else if((c&0xE0)==0xC0)
  {
   if(!srcn) R -1;
   c1=*src++;--srcn;
   if((c1&0xc0)!=0x80) R -1;
   cf=((c&0x1F)<<6)|(c1&0x3f);
   if(cf<0x80) R -1; // overlong
   ++r;
  }
  else if((c&0xF0)==0xE0)
  {
   if(!srcn) R -1;
   c1=*src++;--srcn;
   if((c1&0xc0)!=0x80) R -1;
   if(!srcn) R -1;
   c2=*src++;--srcn;
   if((c2&0xc0)!=0x80) R -1;
   cf=((c&0x0F)<<12)|((c1&0x3F)<<6)|(c2&0x3f);
   if(cf<0x800) R -1; // overlong
   ++r;
  }
  else
   R -1;
 }
 R r;
}

// c2v to utf-8 - assume valid data and snk size is ok
void wtom(US* src, I srcn, UC* snk){ US w;
 while(srcn--)
 {
  w=*src++;
  if(w<=0x7f)
   *snk++=(C)w;
  else if(w<=0x7ff)
  {
   *snk++=0xc0|(w>>6);
   *snk++=0x80|(0x3f&w);
  }
  else
  {
   *snk++=0xe0|w>>12;
   *snk++=0x80|(0x3f&(w>>6));
   *snk++=0x80|(0x3f&w);
  }
 }
}

// get size of conversion from c2v to utf-8
static I wtomsize(US* src, I srcn){ US w;I r=0;
 while(srcn--)
 {
  w=*src++;
  if(w<=0x7f)
   ++r;
  else if(w<=0x7ff)
   r+=2;
  else
   r+=3;
 }
 R r;
}

F1(jttoutf16){F1PREF;A z;I n,t,q,b=0; C* wv; US* c2v; 
 RZ(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w); wv=CAV(w);
 if(!n) {GA(z,LIT,n,1,0); R z;}; // empty lit list 
 if(LIT&t)
 {
  DO(n, if(0>*wv++){b=1;break;});
  if(!b){ if(1==AR(w)) {R ca(w);}; GA(z,LIT,1,1,0); *CAV(z)=*CAV(w); R z;} // ascii list unchanged ascii scalar as list
  q=mtowsize(CAV(w),n);
  ASSERT(q>=0,EVDOMAIN);
  GA(z,C2T,q,1,0);
  mtow(CAV(w),n,(US*)CAV(z));
  R z; // u16 from u8
 }
 else if(C2T&t)
 {
  c2v=(US*)AV(w);
  DO(n, if(127<*c2v++){b=1;break;});
  if(b) R ca(w); // u16 unchanged
  GA(z,LIT,n,AR(w),AS(w));
  wv=CAV(z);
  c2v=(US*)AV(w);
  DO(n, *wv++=(char)*c2v++;);
  R z;
 }
 else
  ASSERT(0, EVDOMAIN);
}    // 7 u: x - utf16 from LIT or C2T

F1(jttoutf8){F1PREF;A z;I n,t,q;
RZ(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w);
if(!n) {GA(z,LIT,n,AR(w),AS(w)); R z;}; // empty lit 
if(t&LIT) R ca(w); // char unchanged
ASSERT(t&C2T, EVDOMAIN);
q=wtomsize((US*)CAV(w),n);
GA(z,LIT,q,1,0);
wtom((US*)CAV(w),n,CAV(z));
R z;
}    // 8 u: x - utf8 from LIT or C2T

F1(jttoutf16x){F1PREF;I q;A z;
ASSERT(LIT&AT(w),EVDOMAIN);
q=mtowsize(CAV(w),AN(w));
ASSERT(q>=0,EVDOMAIN);
GA(z,C2T,q,1,0);
mtow(CAV(w),AN(w),(US*)CAV(z));
R z; // u16 from u8
}

void jttoutf8x(J jt,C* f, I n, US* fw){I q;
q=wtomsize(fw,wcslen(fw));
wtom(fw,wcslen(fw),f);
f[q]=0;
}
