/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: u: conversions                                                   */

#include <wchar.h>

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

// w is a wide char; result is number of excess display positions needed
// for now, this is just 1 for wide CJK characters
static I extrawidth(US w){
// Info taken from //www.unicode.org/Public/UCD/latest/ucd/EastAsianWidth.txt
// These are the characters listed as having East Asian width of F or H.
// Characters with width A (ambiguous) are given the same status as the neighboring characters
// In the array, a number in range 0-ffff starts an interval; 1xxxx ends an interval.  Singletons use 0-ffff
static I4  widechars[] = {
0x1100, 0x1115F, // Hangul - note the first value is wired into jttwidthf16 below
0x231A, 0x1231B, // Hourglass
0x2329, 0x1232A, // Angle Brackets
0x23E9, 0x123EC, // Double triangles
0x23F0, // Hourglass
0x23F3, // Hourglass
0x25FD, 0x125FE, // Small square
0x2614, 0x12615, // Umbrellas
0x2648, 0x12653, // Astrological
0x267F, // Wheelchair
0x2693, // Anchor
0x26A1, // High Voltage
0x26AA, 0x126AB, // Medium white circle
0x26BD, 0x126BE, // Soccer ball, baseball
0x26C4, 0x126C5, // Snowman
0x26CE, // Ophiuchus
0x26D4, // No entry
0x26EA, // church
0x26F2, 0x126F3, // fountain
0x26F5, // sailboat
0x26FA, // tent
0x26FD, // fuel pump
0x2705, // check mark
0x270A, 0x1270B, // Victory fist
0x2728, // sparkles
0x274C, // cross mark
0x274E, // negative cross mark
0x2753, 0x12755, // Question marks
0x2757, // Exclamation mark
0x2795, 0x12797, // Plus sign
0x27B0, // Curly Loop
0x27BF, // Double Curly loop
0x2B1B, 0x12B1C, // Black Squre
0x2B50, // square
0x2B55, // Circle
0x2E80, 0x12E99, // CJK
0x2E9B, 0x12EF3, // CJK
0x2F00, 0x12FD5, // CJK
0x2FF0, 0x12FFB, // CJK
0x3000, 0x1303E, // CJK
0x3041, 0x14DBF, // CJK
0x4E00, 0x1A4C6, // CJK
0xA960, 0x1A97C, // CJK
0xAC00, 0x1D7A3, // Hangul
0xE000, 0x1FAFF, // CJK
0xFE10, 0x1FE19, // Punctuation
0xFE30, 0x1FE6B, // Punctuation
0xFF01, 0x1FF60, // Punctuation
0xFFE0, 0x1FFE6  //Symbols
};
 if(w<widechars[0] || w>(widechars[sizeof(widechars)/sizeof(widechars[0])-1]&0xffff))R 0;
 // binary search to find the first entry >= w
 I l=0, r=sizeof(widechars)/sizeof(widechars[0])-1;  // left index, right index.  w is >= widechars[l] and widechars[r]
 while(l<=r){
  I m=(l+r)>>1;
  if(w==(widechars[m]&0xffff)){l = m; break;}
  if(w > (widechars[m]&0xffff))l = m+1;
  else if(w < (widechars[m]&0xffff))r = m-1;
 }
 // widechars[l] is the first entry >= w.  If it is an end-of-interval, or it is equal to w, w is in an interval: return 1 then else 0
 R (widechars[l]&0x10000) || widechars[l]==w;
}

// c2v to utf-8 - assume valid data and snk size is ok
static void wtomnull(US* src, I srcn, UC* snk, I eatnull){ US w;I nignulls = 0;
 while(srcn--)
 {
  w=*src++;
  if(nignulls && (--nignulls,w==0));  // If we should ignore a null, do so, decrement count of ignored nulls
  else if(w<=0x7f)
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
   if(eatnull)nignulls = extrawidth(w);  // extra nulls only possible in upper pages.  See if there are any
  }
 }
}

// c2v to utf-8 - assume valid data and snk size is ok
// This version seems to be referred to externally, but I can't see where from
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
// If eatnull is true, discard NUL bytes following a wide-display char
static I wtomsize(US* src, I srcn, I eatnull){ US w;I r=0;I nignulls = 0;
 while(srcn--)
 {
  w=*src++;
  if(nignulls && (--nignulls,w==0));  // If we should ignore a null, do so, decrement count of ignored nulls
  else if(w<=0x7f)
   ++r;
  else if(w<=0x7ff)
   r+=2;
  else
   r+=3;
   if(eatnull)nignulls = extrawidth(w);  // count the extra null; may rescind if already there
 }
 R r;
}

// Similar to out16, but called only for byte inputs at rank 1.
// We don't abort on invalid U8, just keep the original input
// We convert a byte string to wide-char
F1(jttoutf16r){A z;I n,t,q,b=0; C* wv; US* c2v;
 RZ(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w); wv=CAV(w);
 if(!n) {GATV(z,LIT,n,1,0); R z;}; // empty lit list 
 q=mtowsize(CAV(w),n);
 if(q<0 || q==n)R ca(w);  // If invalid or no U8, keep as byte
 GATV(z,C2T,q,1,0);  // allocate result - long enough to hold the C@Ts
 c2v=(US*)CAV(z);
 mtow(CAV(w),n,c2v);
// obsolete for(;q<n;++q)c2v[q]=32;  no need to pad with spaces now
 R twidthf16(z); // check the C2T result for wide chars
}

// Pad a wide-char array to make shape match display shape.
// w is a C2T block.  Result is a C2T block with characters wider than
// 1 display position padded with following NUL chars to make the
// width in the array match the display width.  Currently, this is done for CJK
// codepoints.  To make this function idempotent, we don't add a
// NUL char if a wide char is already followed by a NUL byte.  We assume that
// there will not be NUL bytes in user input (since they were going to be eaten before display anyway)
F1(jttwidthf16){US *wv,*zv;I n,i,nignulls=0,naddednulls=0;A z;
 RZ(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); wv=USAV(w);
 if(!n) R w;
 // See how many NUL codes we will need to add
 for(i=0;i<n;++i){
  if(nignulls && (--nignulls,wv[i]==0))--naddednulls;  // If an added null is already present, remove it from the count
  else if(wv[i]>=0x1100)naddednulls += nignulls = extrawidth(wv[i]);
 }
 // If no padding is needed, no copy is required
 if(!naddednulls)R w;
 // Allocate result and copy the string, adding null padding where needed
 GATV(z,C2T,n+naddednulls,1,0); zv=USAV(z);
 nignulls = 0;
 for(i=0;i<n;++i){
  if(nignulls && (--nignulls,wv[i]==0));  // If an added null is already present, skip the copy
  else{*zv++ = wv[i]; if(wv[i]>=0x1100&&(nignulls = extrawidth(wv[i]))){*zv++=0;}}  // copy the char; if doublewide, copy the null for it
 }
 R z;
}

F1(jttoutf16){A z;I n,t,q,b=0; C* wv; US* c2v; 
 RZ(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w); wv=CAV(w);
 if(!n) {GATV(z,LIT,n,1,0); R z;}; // empty lit list 
 if(LIT&t)
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
  GATV(z,LIT,n,AR(w),AS(w));
  wv=CAV(z);
  c2v=(US*)AV(w);
  DO(n, *wv++=(char)*c2v++;);
  R z;
 }
 else
  ASSERT(0, EVDOMAIN);
}    // 7 u: x - utf16 from LIT or C2T

// w is C2T or LIT.  Result is U8 string
// If a wide-display C2T is followed by NULs, remove the NULs
F1(jttoutf8){A z;I n,t,q;
RZ(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w);
if(!n) {GATV(z,LIT,n,AR(w),AS(w)); R z;}; // empty lit 
if(t&LIT) R ca(w); // char unchanged
ASSERT(t&C2T, EVDOMAIN);
q=wtomsize((US*)CAV(w),n,1);
 GATV(z,LIT,q,1,0);
wtomnull((US*)CAV(w),n,CAV(z),1);
R z;
}    // 8 u: x - utf8 from LIT or C2T

F1(jttoutf16x){I q;A z;
ASSERT(LIT&AT(w),EVDOMAIN);
q=mtowsize(CAV(w),AN(w));
ASSERT(q>=0,EVDOMAIN);
 GATV(z,C2T,q,1,0);
mtow(CAV(w),AN(w),(US*)CAV(z));
R z; // u16 from u8
}

// External function - just convert wide-char fw[] to U8 in f[], and null-terminate 
void jttoutf8x(J jt,C* f, I n, US* fw){I q;
q=wtomsize(fw,wcslen((wchar_t*)fw),0);
wtom(fw,wcslen((wchar_t*)fw),f);
f[q]=0;
}
