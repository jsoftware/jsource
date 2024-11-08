/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Format: ": Monad boxed unicode                                          */

#include "j.h"

// in xu.c
extern I mtousize(UC* src, I srcn);
extern I mtowsize(UC* src, I srcn);
extern I wtomsize(US* src, I srcn);
extern I wtousize(US* src, I srcn);
extern I utomsize(C4* src, I srcn);
extern I utousize(C4* src, I srcn);
extern void mtow(UC* src, I srcn, US* snk);
extern void mtou(UC* src, I srcn, C4* snk);
extern void wtom(US* src, I srcn, UC* snk);
extern void wtou(US* src, I srcn, C4* snk);
extern void utom(C4* src, I srcn, UC* snk);
extern void utou(C4* src, I srcn, C4* snk);

// forward declaration
A RoutineA(J,A,A);
A RoutineB(J,A,A);
A RoutineC(J,A,A);
A RoutineD(J,A,A);
static I wtomnullsize(US* src, I srcn);
static I utomnullsize(C4* src, I srcn);
static void wtomnull(US* src, I srcn, UC* snk);
static void utomnull(C4* src, I srcn, UC* snk);
static A wtownull(J,A);
static A utounull(J,A);

// w is a wide char; result is number of excess display positions needed
// for now, this is just 1 for wide CJK characters
static I extrawidth(C4 w){
// Info taken from //www.unicode.org/Public/UCD/latest/ucd/EastAsianWidth.txt
// These are the characters listed as having East Asian width of F or H.
// Characters with width A (ambiguous) are given the same status as the neighboring characters
// In the array, a number in range 0-10ffff starts an interval; 1xxxxxx ends an interval.  Singletons use 0-10ffff
static const UINT  widechars[] = {
0x1100, 0x100115F, // Hangul - note the first value is wired into jttwidthf16 below
0x231A, 0x100231B, // Hourglass
0x2329, 0x100232A, // Angle Brackets
0x23E9, 0x10023EC, // Double triangles
0x23F0, // Hourglass
0x23F3, // Hourglass
0x25FD, 0x10025FE, // Small square
0x2614, 0x1002615, // Umbrellas
0x2648, 0x1002653, // Astrological
0x267F, // Wheelchair
0x2693, // Anchor
0x26A1, // High Voltage
0x26AA, 0x10026AB, // Medium white circle
0x26BD, 0x10026BE, // Soccer ball, baseball
0x26C4, 0x10026C5, // Snowman
0x26CE, // Ophiuchus
0x26D4, // No entry
0x26EA, // church
0x26F2, 0x10026F3, // fountain
0x26F5, // sailboat
0x26FA, // tent
0x26FD, // fuel pump
0x2705, // check mark
0x270A, 0x100270B, // Victory fist
0x2728, // sparkles
0x274C, // cross mark
0x274E, // negative cross mark
0x2753, 0x1002755, // Question marks
0x2757, // Exclamation mark
0x2795, 0x1002797, // Plus sign
0x27B0, // Curly Loop
0x27BF, // Double Curly loop
0x2B1B, 0x1002B1C, // Black Squre
0x2B50, // square
0x2B55, // Circle
0x2E80, 0x1002E99, // CJK
0x2E9B, 0x1002EF3, // CJK
0x2F00, 0x1002FD5, // CJK
0x2FF0, 0x1002FFB, // CJK
0x3000, 0x100303E, // CJK
0x3041, 0x1004DBF, // CJK
0x4E00, 0x100A4C6, // CJK
0xA960, 0x100A97C, // CJK
0xAC00, 0x100D7A3, // Hangul
0xE000, 0x100FAFF, // CJK
0xFE10, 0x100FE19, // Punctuation
0xFE30, 0x100FE6B, // Punctuation
0xFF01, 0x100FF60, // Punctuation
0xFFE0, 0x100FFE6,  //Symbols
0x16FE0,           // Lm         TANGUT ITERATION MARK
0x17000, 0x10187EC,    // Lo  [6125] TANGUT IDEOGRAPH-17000..TANGUT IDEOGRAPH-187EC
0x18800, 0x1018AF2,    // Lo   [755] TANGUT COMPONENT-001..TANGUT COMPONENT-755
0x1B000, 0x101B001,    // Lo     [2] KATAKANA LETTER ARCHAIC E..HIRAGANA LETTER ARCHAIC YE
0x1F004,           // So         MAHJONG TILE RED DRAGON
0x1F0CF,           // So         PLAYING CARD BLACK JOKER
0x1F18E,           // So         NEGATIVE SQUARED AB
0x1F191, 0x101F19A,    // So    [10] SQUARED CL..SQUARED VS
0x1F200, 0x101F202,    // So     [3] SQUARE HIRAGANA HOKA..SQUARED KATAKANA SA
0x1F210, 0x101F23B,    // So    [44] SQUARED CJK UNIFIED IDEOGRAPH-624B..SQUARED CJK UNIFIED IDEOGRAPH-914D
0x1F240, 0x101F248,    // So     [9] TORTOISE SHELL BRACKETED CJK UNIFIED IDEOGRAPH-672C..TORTOISE SHELL BRACKETED CJK UNIFIED IDEOGRAPH-6557
0x1F250, 0x101F251,    // So     [2] CIRCLED IDEOGRAPH ADVANTAGE..CIRCLED IDEOGRAPH ACCEPT
0x1F300, 0x101F320,    // So    [33] CYCLONE..SHOOTING STAR
0x1F32D, 0x101F335,    // So     [9] HOT DOG..CACTUS
0x1F337, 0x101F37C,    // So    [70] TULIP..BABY BOTTLE
0x1F37E, 0x101F393,    // So    [22] BOTTLE WITH POPPING CORK..GRADUATION CAP
0x1F3A0, 0x101F3CA,    // So    [43] CAROUSEL HORSE..SWIMMER
0x1F3CF, 0x101F3D3,    // So     [5] CRICKET BAT AND BALL..TABLE TENNIS PADDLE AND BALL
0x1F3E0, 0x101F3F0,    // So    [17] HOUSE BUILDING..EUROPEAN CASTLE
0x1F3F4,           // So         WAVING BLACK FLAG
0x1F3F8, 0x101F3FA,    // So     [3] BADMINTON RACQUET AND SHUTTLECOCK..AMPHORA
0x1F3FB, 0x101F3FF,    // Sk     [5] EMOJI MODIFIER FITZPATRICK TYPE-1-2..EMOJI MODIFIER FITZPATRICK TYPE-6
0x1F400, 0x101F43E,    // So    [63] RAT..PAW PRINTS
0x1F440,           // So         EYES
0x1F442, 0x101F4FC,    // So   [187] EAR..VIDEOCASSETTE
0x1F4FF, 0x101F53D,    // So    [63] PRAYER BEADS..DOWN-POINTING SMALL RED TRIANGLE
0x1F54B, 0x101F54E,    // So     [4] KAABA..MENORAH WITH NINE BRANCHES
0x1F550, 0x101F567,    // So    [24] CLOCK FACE ONE OCLOCK..CLOCK FACE TWELVE-THIRTY
0x1F57A,           // So         MAN DANCING
0x1F595, 0x101F596,    // So     [2] REVERSED HAND WITH MIDDLE FINGER EXTENDED..RAISED HAND WITH PART BETWEEN MIDDLE AND RING FINGERS
0x1F5A4,           // So         BLACK HEART
0x1F5FB, 0x101F5FF,    // So     [5] MOUNT FUJI..MOYAI
0x1F600, 0x101F64F,    // So    [80] GRINNING FACE..PERSON WITH FOLDED HANDS
0x1F680, 0x101F6C5,    // So    [70] ROCKET..LEFT LUGGAGE
0x1F6CC,           // So         SLEEPING ACCOMMODATION
0x1F6D0, 0x101F6D2,    // So     [3] PLACE OF WORSHIP..SHOPPING TROLLEY
0x1F6EB, 0x101F6EC,    // So     [2] AIRPLANE DEPARTURE..AIRPLANE ARRIVING
0x1F6F4, 0x101F6F6,    // So     [3] SCOOTER..CANOE
0x1F910, 0x101F91E,    // So    [15] ZIPPER-MOUTH FACE..HAND WITH INDEX AND MIDDLE FINGERS CROSSED
0x1F920, 0x101F927,    // So     [8] FACE WITH COWBOY HAT..SNEEZING FACE
0x1F930,           // So         PREGNANT WOMAN
0x1F933, 0x101F93E,    // So    [12] SELFIE..HANDBALL
0x1F940, 0x101F94B,    // So    [12] WILTED FLOWER..MARTIAL ARTS UNIFORM
0x1F950, 0x101F95E,    // So    [15] CROISSANT..PANCAKES
0x1F980, 0x101F991,    // So    [18] CRAB..SQUID
0x1F9C0,           // So         CHEESE WEDGE
0x20000, 0x102FFFD,    // All undesignated code points in Planes 2 and 3, whether inside or
0x30000, 0x103FFFD     // outside of allocated blocks, default to "W":
};

 if(w<widechars[0] || w>(widechars[sizeof(widechars)/sizeof(widechars[0])-1]&0xffffff))R 0;
 // binary search to find the first entry >= w
 I l=0, r=sizeof(widechars)/sizeof(widechars[0])-1;  // left index, right index.  w is >= widechars[l] and widechars[r]
 while(l<=r){
  I m=(l+r)>>1;
  if(w==(widechars[m]&0xffffff)){l = m; break;}
  if(w > (widechars[m]&0xffffff))l = m+1;
  else if(w < (widechars[m]&0xffffff))r = m-1;
 }
 // widechars[l] is the first entry >= w.  If it is an end-of-interval, or it is equal to w, w is in an interval: return 1 then else 0
 R (widechars[l]&0x1000000) || widechars[l]==w;
}

static I wtomnullsize(US* src, I srcn){ US w,w1;I nignulls = 0; I r=0;
 I eatnull=1;
 while(srcn--)
 {
  w=*src++;
  if(nignulls && (--nignulls,w==0));  // If we should ignore a null, do so, decrement count of ignored nulls
  else if(w<=0x7f)
   r++;
  else if(w<=0x7ff)
  {
   r+=2;
  }
  else if((w>=0x800&&w<=0xd7ff)||(w>=0xe000))
  {
   r+=3;
   if(eatnull)nignulls = extrawidth(w);  // extra nulls only possible in upper pages.  See if there are any
  else
  {
   if(!srcn)
   {
    // isolated surrogate
    r+=3;
   }
   else
   {
    w1=*src;
    if(w>=0xdc00||w1<=0xdbff||w1>=0xe000) // incorrect high/low surrogate
    {
     r+=3;
    }
    else
    {
     r+=4;
     C4 t=(((w&0x3ff)<<10)|(w1&0x3ff))+0x10000;
     if(eatnull)nignulls = extrawidth(t);  // extra nulls only possible in upper pages.  See if there are any
     src++;srcn--;  // next code unit of surrogate pair
    }
   }
   }
  }
 }
 R r;
}

static void wtomnull(US* src, I srcn, UC* snk){ US w,w1;I nignulls = 0;
 I eatnull=1;
 while(srcn--)
 {
  w=*src++;
  if(nignulls && (--nignulls,w==0));  // If we should ignore a null, do so, decrement count of ignored nulls
  else if(w<=0x7f)
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
   if(eatnull)nignulls = extrawidth(w);  // extra nulls only possible in upper pages.  See if there are any
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
     C4 t=(((w&0x3ff)<<10)|(w1&0x3ff))+0x10000;
     *snk++=0xf0|((t>>18)&0x07);
     *snk++=0x80|((t>>12)&0x3f);
     *snk++=0x80|((t>>6)&0x3f);
     *snk++=0x80|(t&0x3f);
     if(eatnull)nignulls = extrawidth(t);  // extra nulls only possible in upper pages.  See if there are any
     src++;srcn--;  // next code unit of surrogate pair
    }
   }
   }
  }
 }
}

// c4v to utf-8 - assume valid data and snk size is ok
static I utomnullsize(C4* src, I srcn){ C4 w;I nignulls = 0; I r=0;
 I eatnull=1;
 while(srcn--)
 {
  w=*src++;
  if(nignulls && (--nignulls,w==0));  // If we should ignore a null, do so, decrement count of ignored nulls
  else if(w<=0x7f)
   r++;
  else if(w<=0x7ff)
  {
   r+=2;
  }
  else if(w<=0xffff)
  {
   r+=3;
   if(eatnull)nignulls = extrawidth(w);  // extra nulls only possible in upper pages.  See if there are any
  }
  else
  {
   r+=4;
   if(eatnull)nignulls = extrawidth(w);  // extra nulls only possible in upper pages.  See if there are any
  }
 }
 R r;
}

// c4v to utf-8 - assume valid data and snk size is ok
static void utomnull(C4* src, I srcn, UC* snk){ C4 w;I nignulls = 0;
 I eatnull=1;
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
  else if(w<=0xffff)
  {
   *snk++=0xe0|w>>12;
   *snk++=0x80|(0x3f&(w>>6));
   *snk++=0x80|(0x3f&w);
   if(eatnull)nignulls = extrawidth(w);  // extra nulls only possible in upper pages.  See if there are any
  }
  else
  {
     *snk++=0xf0|((w>>18)&0x07);
     *snk++=0x80|((w>>12)&0x3f);
     *snk++=0x80|((w>>6)&0x3f);
     *snk++=0x80|(w&0x3f);
   if(eatnull)nignulls = extrawidth(w);  // extra nulls only possible in upper pages.  See if there are any
  }
 }
}

// Pad a wide-char array to make shape match display shape.
// w is a C2T block.  Result is a C2T block with characters wider than
// 1 display position padded with following NUL chars to make the
// width in the array match the display width.  Currently, this is done only for boxed CJK
// codepoints.  To make this function idempotent, we don't add a
// NUL char if a wide char is already followed by a NUL byte.  We assume that
// there will not be NUL bytes in user input (since they were going to be eaten before display anyway)
static A wtownull(J jt,A w){US *wv,*zv;I n,i,nignulls=0,naddednulls=0;A z;
 ARGCHK1(w); n=AN(w);
 if(!n) R w;
 wv=USAV(w);
 // See how many NUL codes we will need to add
 for(i=0;i<n;++i){
  if(nignulls && (--nignulls,wv[i]==0))--naddednulls;  // If an added null is already present, remove it from the count
  else if(wv[i]>=0x1100)naddednulls += nignulls = extrawidth(wv[i]);
 }
 // If no padding is needed, no copy is required
 if(!naddednulls)R w;
 // Allocate result and copy the string, adding null padding where needed
 GATV0(z,C2T,n+naddednulls,1); zv=USAV1(z);
 nignulls = 0;
 for(i=0;i<n;++i){
  if(nignulls && (--nignulls,wv[i]==0));  // If an added null is already present, skip the copy
  else{*zv++ = wv[i]; if(wv[i]>=0x1100&&(nignulls = extrawidth(wv[i]))){*zv++=0;}}  // copy the char; if doublewide, copy the null for it
 }
 R z;
}

static A utounull(J jt,A w){C4 *wv,*zv;I n,i,nignulls=0,naddednulls=0;A z;
 ARGCHK1(w); n=AN(w);
 if(!n) R w;
 wv=C4AV(w);
 // See how many NUL codes we will need to add
 for(i=0;i<n;++i){
  if(nignulls && (--nignulls,wv[i]==0))--naddednulls;  // If an added null is already present, remove it from the count
  else if(wv[i]>=0x1100)naddednulls += nignulls = extrawidth(wv[i]);
 }
 // If no padding is needed, no copy is required
 if(!naddednulls)R w;
 // Allocate result and copy the string, adding null padding where needed
 GATV0(z,C4T,n+naddednulls,1); zv=C4AV1(z);
 nignulls = 0;
 for(i=0;i<n;++i){
  if(nignulls && (--nignulls,wv[i]==0));  // If an added null is already present, skip the copy
  else{*zv++ = wv[i]; if(wv[i]>=0x1100&&(nignulls = extrawidth(wv[i]))){*zv++=0;}}  // copy the char; if doublewide, copy the null for it
 }
 R z;
}


/*
Routine A:
Input: a block of type LIT, rank <=1
Result: a block of type LIT, C2T, or C4T
Process:
 if(block is all ASCII or contains invalid UTF-8)return a copy of the input block;
 if(block contains no codepoints above FFFF){
  UTF-decode to C2T;
  call Routine B and return its result;
 }
 UTF-decode to C4T;
 call Routine C and return its result;

Routine B:
Input: a block of type C2T, rank <=1
Result: a block of type C2T, or C4T
Process:
 if(block contains surrogates){
  Convert block to C4T, one character at a time, ignoring surrogates;
  Call Routine C and return its result;
 }
 if(BAV(prxthornuni)[0]&1)install a NUL C2T character after each CJK fullwidth char that is not followed by NUL;
 return the C2T block;

Routine C:
Input: a block of type C4T, rank <=1
Result: a block of type C4T
Process:
 if(block contains surrogate pairs){
  Join surrogate pairs into one C4T character per pair;
 }
 if(BAV(prxthornuni)[0]&1)install a NUL C4T character after each CJK fullwidth char that is not followed by NUL;
 return the C4T block;


Routine D:
Input: a block of type C2T or C4T, rank <=1
Result: a block of type LIT
Process:
  if(C4T and block contains a character above 10FFFF)domain error;
  if(BAV(prxthornuni)[0]&1)convert to UTF-8 byte string, ignoring the first NUL following a CJK fullwidth character;
  else convert all bytes to UTF-8 byte string;
*/

A RoutineA(J jt,A w,A prxthornuni){A z;I n,t,q,q1,b=0; UC* wv;
 ARGCHK1(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w); wv=UAV(w);
 ASSERT(t&LIT,EVDOMAIN);
 if(!n) {GATV(z,LIT,n,AR(w),AS(w)); R z;}; // empty lit list 
 DQ(n, if(127<*wv++){b=1;break;});
 if(!b)RCA(w);
 q=mtowsize(UAV(w),n);
 if(q<0)RCA(w);
 if(q==(q1=mtousize(UAV(w),n))){
  GATV0(z,C2T,q,1);
  mtow(UAV(w),n,USAV(z));
  R RoutineB(jt,z,prxthornuni);
 }else{
  GATV0(z,C4T,q1,1);
  mtou(UAV(w),n,C4AV(z));
  R RoutineC(jt,z,prxthornuni);
 }
}

A RoutineB(J jt,A w,A prxthornuni){A z;I n,t,q,b=0; US* c2v; C4* c4v;
 ARGCHK1(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w); I wr=AR(w);
 ASSERT(t&C2T,EVDOMAIN);
 if(!n) {GATV(z,C2T,n,wr,AS(w)); R z;}; // empty C2T list 
 q=wtousize(USAV(w),n);
 if(q<0||q!=n){
  GATV(z,C4T,n,wr,AS(w));
  c4v=C4AVn(wr,z);
  c2v=USAV(w);
  DQ(n, *c4v++=(C4)*c2v++;);
  R RoutineC(jt,z,prxthornuni);
 }
 if(BAV(prxthornuni)[0]&1)R wtownull(jt,w);
 R w;
}

A RoutineC(J jt,A w,A prxthornuni){A z;I n,t,q,b=0; C4* wv;
 ARGCHK1(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w); wv=C4AV(w);
 ASSERT(t&C4T,EVDOMAIN);
 if(!n) {GATV(z,C4T,n,AR(w),AS(w)); R z;}; // empty C4T list 
 DQ(n, if((UI4)(*wv-0xd800)<=(UI4)(0xdfff-0xd800)){b=1;break;};wv++;);
 if(b){
 q=utousize(C4AV(w),n);
 GATV0(z,C4T,q,1);
 utou(C4AV(w),n,C4AV1(z));
 if(BAV(prxthornuni)[0]&1)R utounull(jt,z); else R z;
 }
 if(BAV(prxthornuni)[0]&1)R utounull(jt,w);
 R w;
}

A RoutineD(J jt,A w,A prxthornuni){A z;I n,t,q,b=0;C4* c4v;
ARGCHK1(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w);
if(!n) {GATV(z,LIT,n,AR(w),AS(w)); R z;}; // empty lit list
ASSERT(t&(C2T+C4T), EVDOMAIN);
if(t&C4T)
{
 c4v=C4AV(w);
 DQ(n, if(0x10ffff<*c4v++){b=1;break;});
 ASSERT(!b, EVDOMAIN);
 if(BAV(prxthornuni)[0]&1){
 q=utomnullsize(C4AV(w),n);
 GATV0(z,LIT,q,1);
 utomnull(C4AV(w),n,UAV1(z));
 }
 else
 {
 q=utomsize(C4AV(w),n);
 q=(q<0)?(-q):q;
 GATV0(z,LIT,q,1);
 utom(C4AV(w),n,UAV1(z));
 }
}
else
{
 if(BAV(prxthornuni)[0]&1){
 q=wtomnullsize(USAV(w),n);
 GATV0(z,LIT,q,1);
 wtomnull(USAV(w),n,UAV1(z));
 }
 else
 {
 q=wtomsize(USAV(w),n);
 q=(q<0)?(-q):q;
 GATV0(z,LIT,q,1);
 wtom(USAV(w),n,UAV1(z));
 }
}
R z;
}

// display width of a string
//              c2  0:literal  1:literal2   2:literal4
// unit of nsrc in    C          US           C4
// nul are ignored
I stringdisplaywidth(J jt, I c2, void*src, I nsrc){I n=nsrc,q;A c4;C4*u;
 PROLOG(0000);
 switch(c2){
 default:
  q=mtousize(src,nsrc);
  q=(q<0)?-q:q;
  GATV0(c4,C4T,q,1); u=C4AV1(c4);
  mtou(src,nsrc,u);
  n=q; DO(q, if(u[i])n+=extrawidth(u[i]);else n--;);
  break;
 case 1:
  q=wtousize(src,nsrc);
  q=(q<0)?-q:q;
  GATV0(c4,C4T,q,1); u=C4AV1(c4);
  wtou(src,nsrc,u);
  n=q; DO(q, if(u[i])n+=extrawidth(u[i]);else n--;);
  break;
 case 2:
  u=(C4*)src;
  q=nsrc; DO(q, if(u[i])n+=extrawidth(u[i]);else n--;);
  break;
 }
 EPILOG0;
 R n;
}
