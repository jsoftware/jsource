/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: u: conversions                                                   */

#include <wchar.h>

#include "j.h"
#include "x.h"

// utf-8 to c2v - assumes valid utf-8 data and snk of right size
static void mtow(UC* src, I srcn, US* snk){ US c,c1,c2,c3; UINT t;
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
static I mtowsize(UC* src, I srcn){ US c,c1,c2,c3;UINT t;I r=0;int invalid=0;
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
static void mtou(UC* src, I srcn, C4* snk){ US c,c1,c2,c3; UINT t;
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
static I mtousize(UC* src, I srcn){ US c,c1,c2,c3;UINT t;I r=0;int invalid=0;
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

// w is a wide char; result is number of excess display positions needed
// for now, this is just 1 for wide CJK characters
static I extrawidth(C4 w){
// Info taken from //www.unicode.org/Public/UCD/latest/ucd/EastAsianWidth.txt
// These are the characters listed as having East Asian width of F or H.
// Characters with width A (ambiguous) are given the same status as the neighboring characters
// In the array, a number in range 0-10ffff starts an interval; 1xxxxxx ends an interval.  Singletons use 0-10ffff
static UINT  widechars[] = {
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

// c4v to utf-8 - assume valid data and snk size is ok
static void utomnull(C4* src, I srcn, UC* snk, I eatnull){ C4 w;I nignulls = 0;
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
static I wtomsize(US* src, I srcn){ US w,w1;I r=0;int invalid=0;
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
static I wtousize(US* src, I srcn){ US w,w1;I r=0;int invalid=0;
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
static I utomsize(C4* src, I srcn){ C4 w;I r=0;int invalid=0;
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
static I utowsize(C4* src, I srcn){ C4 w;I r=0;int invalid=0;
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

// Similar to out16, but called only for byte inputs at rank 1.
// We don't abort on invalid U8, just keep the original input
// We convert a byte string to wide-char
F1(jttoutf16r){A z;I n,t,q,b=0; C* wv; US* c2v;
 RZ(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w); wv=CAV(w);
 if(!n) {GATV(z,LIT,n,1,0); R z;}; // empty lit list 
 q=mtousize(CAV(w),n);
// if(q<0 || q==n)R ca(w);  // If invalid or no U8, keep as byte
 if(q==n)R ca(w);
 q=(q<0)?(-q):q;
 GATV(z,C2T,q,1,0);  // allocate result - long enough to hold the C2T
 c2v=(US*)CAV(z);
 mtow(CAV(w),n,c2v);
// obsolete for(;q<n;++q)c2v[q]=32;  no need to pad with spaces now
 R twidthf16(z); // check the C2T result for wide chars
}

// Similar to out32, but called only for byte inputs at rank 1.
// We don't abort on invalid U8, just keep the original input
// We convert a byte string to wide-char
F1(jttoutf32r){A z;I n,t,q,b=0; C* wv; C4* c4v;
 RZ(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w); wv=CAV(w);
 if(!n) {GATV(z,LIT,n,1,0); R z;}; // empty lit list 
 if(C2T&t)
 {
 q=wtousize((US*)CAV(w),n);
 q=(q<0)?(-q):q;
 GATV(z,C4T,q,1,0);  // allocate result - long enough to hold the C4T
 c4v=(C4*)CAV(z);
 wtou((US*)CAV(w),n,c4v);
 }
 else
 {
 q=mtousize(CAV(w),n);
// if(q<0 || q==n)R ca(w);  // If invalid or no U8, keep as byte
 if(q==n)R ca(w);
 q=(q<0)?(-q):q;
 GATV(z,C4T,q,1,0);  // allocate result - long enough to hold the C4T
 c4v=(C4*)CAV(z);
 mtou(CAV(w),n,c4v);
// obsolete for(;q<n;++q)c4v[q]=32;  no need to pad with spaces now
 }
 R twidthf32(z); // check the C4T result for wide chars
}

// Pad a wide-char array to make shape match display shape.
// w is a C2T block.  Result is a C2T block with characters wider than
// 1 display position padded with following NUL chars to make the
// width in the array match the display width.  Currently, this is done for CJK
// codepoints.  To make this function idempotent, we don't add a
// NUL char if a wide char is already followed by a NUL byte.  We assume that
// there will not be NUL bytes in user input (since they were going to be eaten before display anyway)
F1(jttwidthf16){US *wv,*zv;I n,i,nignulls=0,naddednulls=0;A z;
 RZ(w); ASSERT(1>=AR(w),EVRANK); n=AN(w);
 if(!n) R w;
 if(AT(w)&C4T)
 {
  I q=utowsize((C4*)CAV(w),AN(w));
  q=(q<0)?(-q):q;
  A zc;
  GATV(zc,C2T,q,1,0);
  utow((C4*)CAV(w),AN(w),(US*)CAV(zc));
  wv=USAV(zc); n=AN(w);
 }
 else
  wv=USAV(w);
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

// Pad a wide-char array to make shape match display shape.
// w is a C4T block.  Result is a C4T block with characters wider than
// 1 display position padded with following NUL chars to make the
// width in the array match the display width.  Currently, this is done for CJK
// codepoints.  To make this function idempotent, we don't add a
// NUL char if a wide char is already followed by a NUL byte.  We assume that
// there will not be NUL bytes in user input (since they were going to be eaten before display anyway)
F1(jttwidthf32){C4 *wv,*zv;US *w2v;I n,i,nignulls=0,naddednulls=0;A z;
 RZ(w); ASSERT(1>=AR(w),EVRANK); n=AN(w);
 if(!n) R w;
 if(AT(w)&C4T)
 {
 wv=(C4*)AV(w);
 // See how many NUL codes we will need to add
 for(i=0;i<n;++i){
  if(nignulls && (--nignulls,wv[i]==0))--naddednulls;  // If an added null is already present, remove it from the count
  else if(wv[i]>=0x1100)naddednulls += nignulls = extrawidth(wv[i]);
 }
 }
 else
 {
 w2v=(US*)AV(w);
 // See how many NUL codes we will need to add
 for(i=0;i<n;++i){
  if(nignulls && (--nignulls,w2v[i]==0))--naddednulls;  // If an added null is already present, remove it from the count
  else if(w2v[i]>=0x1100)naddednulls += nignulls = extrawidth((C4)w2v[i]);
 }
 }
 // If no padding is needed, no copy is required
 if(!naddednulls)R w;
 // Allocate result and copy the string, adding null padding where needed
 GATV(z,C4T,n+naddednulls,1,0); zv=(C4*)AV(z);
 nignulls = 0;
 if(AT(w)&C4T)
 {
 for(i=0;i<n;++i){
  if(nignulls && (--nignulls,wv[i]==0));  // If an added null is already present, skip the copy
  else{*zv++ = wv[i]; if(wv[i]>=0x1100&&(nignulls = extrawidth(wv[i]))){*zv++=0;}}  // copy the char; if doublewide, copy the null for it
 }
 }
 else
 {
 for(i=0;i<n;++i){
  if(nignulls && (--nignulls,w2v[i]==0));  // If an added null is already present, skip the copy
  else{*zv++ = (C4)w2v[i]; if(w2v[i]>=0x1100&&(nignulls = extrawidth((C4)w2v[i]))){*zv++=0;}}  // copy the char; if doublewide, copy the null for it
 }
 }
 R z;
}

F1(jttoutf16){A z;I n,t,q,b=0,j; C* wv; US* c2v; C4* c4v; A c4; I *v;
 RZ(w); ASSERT(1>=AR(w),EVRANK); n=AN(w); t=AT(w); wv=CAV(w);
 if(!n) {GATV(z,LIT,n,1,0); R z;}; // empty lit list 
 if(t&(B01+INT))
 {
  RZ(w=vi(w));
  n=AN(w); v=(I*)AV(w);
  GATV(c4,C4T,n,AR(w),AS(w)); c4v=(C4*)AV(c4);
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
  GATV(z,LIT,n,AR(w),AS(w));
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
  GATV(z,LIT,n,AR(w),AS(w));
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
GATV(c4,C4T,n,AR(w),AS(w)); c4v=(C4*)AV(c4);
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
  GATV(z,C4T,n,AR(w),AS(w)); c4v=(C4*)AV(z);
//  DO(n, j=*v++; ASSERT(0<=j&&j<=0x10ffff,EVINDEX); *c4v++=(C4)j;);
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
  GATV(z,LIT,n,AR(w),AS(w));
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
  GATV(z,LIT,n,AR(w),AS(w));
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
//  DO(n, j=*v++; ASSERT(0<=j&&j<=0x10ffff,EVINDEX); *c4v++=(C4)j;);
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

