/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Table of Primitive Symbols                                              */

#include "j.h"
#include "ve.h"
#include "vcomp.h"


C ctype[256]={
CX, CX, CX, CX, CX, CX, CX, CX, CX, CS, CX, CX, CX, CX, CX, CX, /*CX                  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, /* 1                  */
CS, CX, CX, CX, CX, CX, CX, CQ, CX, CX, CX, CX, CX, CX, CD, CX, /* 2  !"#$%&'()*+,-./ */
C9, C9, C9, C9, C9, C9, C9, C9, C9, C9, CC, CX, CX, CX, CX, CX, /* 3CX123456789:;<=>? */
CX, CA, CB, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CN, CA, /* 4 @ABCDEFGHIJKLMNO */
CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CX, CX, CX, CX, C9, /* 5 PQRSTUVWXYZ[\]^_ */
CX, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, /* 6 `abcdefghijklmno */
CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CX, CX, CX, CX, CX, /* 7 pqrstuvwxyz{|}~  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, /* 8                  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, /* 9                  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, /* a                  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, /* b                  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, /* c                  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, /* d                  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, /* e                  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, /* f                  */
};
/*   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f   */

extern const UA va1tab[];
extern VA va[];
#if 0  // obsolete 
static const UC vaptr[256]={
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* 0 */
// C0  C1          ^D                    TAB LF          CR         
   1,  2,  3,  4,  5,  6,  7,  8,    9, 10, 11, 12, 13, 14, 15, 16, /* 1 */
// <-----------------------bitwise functions -------------------->
   0, VA2CBANG,  0,  0,  0, VA2CDIV,  0,  0,    0,  0, VA2CSTAR, VA2CPLUS,  0, VA2CMINUS,  0,  0, /* 2 */
//     !   "   #   $   %   &   '     (   )   *   +   ,   -   .   /  
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0, VA2CLT, VA2CEQ, VA2CGT,  0, /* 3 */
// NOUN    HOOK FK ADVF                      :   ;   <   =   >   ?  
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* 4 */
// @                                                                
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0, VA2CEXP,  0, /* 5 */
//                                               [   \   ]   ^   _  
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* 6 */
// `                                                                
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0, VA2CSTILE,  0,  0,  0, /* 7 */
//                                               {   |   }   ~      
   0,  0, VA2CMIN, VA2CLE, VA2CMAX, VA2CGE,  0,  0,   VA2CPLUSDOT, VA2CPLUSCO, VA2CSTARDOT, VA2CSTARCO,  0,  0,  0,  VA1CROOT, /* 8 */
// =.  =:  <.  <:  >.  >:  _.        +.  +:  *.  *:  -.  -:  %.  %: 
   VA1CLOG,  0,  0,  0,  0, VA2CNE,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* 9 */
// ^.  ^:  $.  $:  ~.  ~:  |.  |:    ..  .:  :.  ::  ,.  ,:  ;.  ;: 
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* a */
// #.  #:  !.  !:  /.  /:  \.  \:    [.  [:  ].  ]:  {.  {:  }.  }: 
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* b */
// ".  ":  `.  `:  @.  @:  &.  &:    ?.  ?:  a.  A.  a:  b.  c.     
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* c */
// C.  d.  D.  D:  e.  E.  f.  F:    H.  i.  i:  I.  I:  j.  L.  L: 
   0,  0,  0, VA2CCIRCLE,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* d */
// m.  M.  n.  o.  p.  p:  Q:  q:    r.  s:  S:  t.  t:  T.  u.  u: 
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* e */
// v.  x.  x:  y.                    0:  }ip }cs {:: {:: }:: &.: p..
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* f */
// ,ip                                                           FF  
  };   /* index in va[] for each ID */
/*   1   2   3   4   5   6   7     8   9   a   b   c   d   e   f   */

// return atomic2 ID for the verb w.  If w is b., look for its u operand and return the appropriate 
static C jtvaid(J jt,A w){A x;C c;I k;V*v;
 v=FAV(w); c=v->id;
 if(c==CBDOT){x=v->fgh[1]; if(INT&AT(x)&&!AR(x)&&(k=*AV(x),(k&-16)==16))c=(C)k;}
 R vaptr[(UC)c]?c:0;
}

// prepare a primitive atomic verb for lookups using var()
// if the verb is atomic, we fill in the h field with a pointer to the va row for the verb
void va2primsetup(A w){
 UC xlatedid = vaptr[(UC)FAV(w)->id];  // see which line it is
 // If the id is a comparison operator, turn on the MSB flag bit
 I shiftamt=xlatedid-VA2CNE;
 xlatedid += (((((I)0x80<<(VA2CLT-VA2CNE))|((I)0x80<<(VA2CEQ-VA2CNE))|((I)0x80<<(VA2CGT-VA2CNE))|((I)0x80<<(VA2CLE-VA2CNE))|((I)0x80<<(VA2CGE-VA2CNE))|((I)0x80<<(VA2CNE-VA2CNE)))>>shiftamt)&0x80)&REPSGN(~shiftamt);
 FAV(w)->lc=xlatedid;  // save primitive number for use in ssing and monads
 xlatedid=(xlatedid&0x7f)>VA2CCIRCLE?0:xlatedid;  // if this op is monad-only, don't set dyad info & flags
 FAV(w)->localuse.lvp[0]=(xlatedid?(VA*)&va[xlatedid&0x7f]:0);  // point to the line, or 0 if invalid
 if(xlatedid)FAV(w)->flag |= VISATOMIC2;  // indicate that localuse contains AV pointer
}


// prepare a primitive atomic verb for lookups using var()
// if the verb is atomic, we fill in the lc field with the index to the va row for the verb
void va1primsetup(A w){
 UC xlatedid = (UC)FAV(w)->lc&0x7f;  // see which VA2 type it is
 if(xlatedid>=VA2CMIN)FAV(w)->localuse.lvp[1]=(UA*)&va1tab[xlatedid-VA2CMIN];  // if there is a va1 function, install it
}


static B jtpdef(J jt,I id,I t,AF f1,AF f2,I m,I l,I r,I flag){A z;V*v;
// obsolete  GAT0(z,BOX,(VERBSIZE+SZI-1)>>LGSZI,0);   // use BOX so it will all be cleared
 z=ds(id);  // get address of block
 AK(z)=AKXR(0);  // point to the data
 AT(z)=t;  // install actual type
 ACX(z); v=FAV(z);
 AN(z)=(VERBSIZE+SZI-1)>>LGSZI;  // Install atom count so we can clone
 AFLAG(z) |= AT(z)&TRAVERSIBLE;  // ensure that traversible types in pst are marked traversible, so tpush/ra/fa will not recur on them
 // AM is available for flags, and AR except for actual rank
 if(t&VERB+ADV+CONJ){
 v->valencefns[0]=f1?f1:jtdomainerr1;  /* monad C function */
 v->valencefns[1]=f2?f2:jtdomainerr2;  /* dyad  C function */
 v->mr=(RANKT)m;                   /* monadic rank     */
 v->lrr=(RANK2T)((l<<RANKTX)+r);  // left and right rank
#if !USECSTACK
 v->fdep=1;                 /* function depth   */
#endif
 v->id=(C)id;                  /* spelling         */
 v->flag=(UI4)flag;              // flags
 v->flag2=(UI4)(id>>8);
 va2primsetup(z);  // install info, if any, needed by va2
 va1primsetup(z);  // install info, if any, needed by va1 must call va2setup first
 }
// obsolete  ds(id)=z;             /* other fields are zeroed in ga() */
 R 1;
}
#endif
static const C alp[256]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff};

#if 1

#define PRIMNOUN(id,t,an,ar,val) [id]={{AKXR(ar),(t)&TRAVERSIBLE,0,(t),ACPERMANENT,(an),(ar)} , {.primint=val} }
#define PRIMALL(id,t,f0,f1,f,g,h,initpm1,initpm2,rm,rl,rr,vflg,vflg2,an,ar,lc) \
 [id]={{AKXR(ar),(t)&TRAVERSIBLE,0,(t),ACPERMANENT,(an),(ar)},{{{f0,f1},{f,g,h},{initpm1,initpm2},(vflg),(vflg2),(RANK2T)((rl<<RANKTX)+rr),(RANKT)rm,id,lc}}}
#define PRIMACV(id,t,f0,f1,rm,rl,rr,vflg,vflg2) PRIMALL(id,t,f0,f1,0,0,0,.lfns=(AF)0,(AF)0,rm,rl,rr,vflg,vflg2,(VERBSIZE+SZI-1)>>LGSZI,0,0)
#define PRIMACVPARM(id,t,f0,f1,rm,rl,rr,vflg,vflg2,initpm1,initpm2) PRIMALL(id,t,f0,f1,0,0,0,initpm1,initpm2,rm,rl,rr,vflg,vflg2,(VERBSIZE+SZI-1)>>LGSZI,0,0)
#define PRIMATOMIC2(id,t,f0,f1,rm,rl,rr,vflg,vflg2) PRIMALL(id,t,f0,f1,0,0,0,.lfns=(AF)(va+VA2##id),(AF)0,rm,rl,rr,VISATOMIC2|(vflg),vflg2,(VERBSIZE+SZI-1)>>LGSZI,0, \
 VA2##id+0x80*(id==CLT||id==CGT||id==CLE||id==CGE||id==CEQ||id==CNE))
#define PRIMATOMIC2g(id,t,f0,f1,rm,rl,rr,vflg,vflg2,g) PRIMALL(id,t,f0,f1,0,g,0,.lfns=(AF)(va+VA2##id),(AF)0,rm,rl,rr,VISATOMIC2|(vflg),vflg2,(VERBSIZE+SZI-1)>>LGSZI,0, \
 VA2##id+0x80*(id==CLT||id==CGT||id==CLE||id==CGE||id==CEQ||id==CNE))
#define PRIMATOMIC1(id,t,f0,f1,rm,rl,rr,vflg,vflg2) PRIMALL(id,t,f0,f1,0,0,0,.lfns=(AF)0,(AF)(va1tab+VA1##id-VA1CMIN),rm,rl,rr,vflg,vflg2,(VERBSIZE+SZI-1)>>LGSZI,0, \
 VA1##id+0x80*(id==CLT||id==CGT||id==CLE||id==CGE||id==CEQ||id==CNE))
#define PRIMATOMIC12(id,t,f0,f1,rm,rl,rr,vflg,vflg2) PRIMALL(id,t,f0,f1,0,0,0,.lfns=(AF)(va+VA2##id),(AF)(va1tab+VA1##id-VA1CMIN),rm,rl,rr,VISATOMIC2|(vflg),vflg2,(VERBSIZE+SZI-1)>>LGSZI,0, \
 VA2##id+0x80*(id==CLT||id==CGT||id==CLE||id==CGE||id==CEQ||id==CNE))

PRIM primtab[256] = {
 /*  (  */  PRIMNOUN(CLPAR,LPAR,1,0,0),
 /*  )  */  PRIMNOUN(CRPAR,RPAR,1,0,0),
 /*  =. */  PRIMNOUN(CASGN,ASGN+ASGNLOCAL,1,0,CASGN),
 /*  =: */  PRIMNOUN(CGASGN,ASGN,1,0,CGASGN),
 /*  a: */  PRIMNOUN(CACE,BOX,1,0,(I)mtv),
 /*  a. */  PRIMNOUN(CALP,LIT,NALP,1,0),   // this gets modified at init time

 /*  bw */  PRIMATOMIC2(CBW0000, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW0001, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW0010, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW0011, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW0100, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW0101, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW0110, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW0111, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW1000, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW1001, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW1010, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW1011, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW1100, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW1101, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW1110, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW1111, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  =  */  PRIMATOMIC2(CEQ,     VERB, jtsclass,  jtatomic2,     RMAX,0,   0   ,VFUSEDOK2|VASGSAFE|VIRS2|VJTFLGOK2,VF2NONE),
 /*  =. */       /* see above */
 /*  =: */       /* see above */
 /*  <  */  PRIMATOMIC2g(CLT,    VERB, jtbox,     jtatomic2,     RMAX,0,   0   ,VASGSAFE|VFUSEDOK2|VIRS1|VIRS2|VJTFLGOK2,VF2BOXATOP1,ds(CRIGHT)),  // alias CLT
 /*  <. */  PRIMATOMIC12(CMIN,  VERB, jtatomic1,  jtatomic2,0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),  // alias CMIN
 /*  <: */  PRIMATOMIC2(CLE,     VERB, jtdecrem,  jtatomic2,     0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  >  */  PRIMATOMIC2(CGT,    VERB, jtope,     jtatomic2,     0,   0,   0   ,VFUSEDOK2|VASGSAFE|VIRS2|VJTFLGOK2,VF2WILLOPEN1),  // alias CGT
 /*  >. */  PRIMATOMIC12(CMAX,   VERB, jtatomic1,   jtatomic2,0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),  // alias CMAX
 /*  >: */  PRIMATOMIC2(CGE,     VERB, jtincrem,  jtatomic2,     0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  +  */  PRIMATOMIC12(CPLUS,   VERB, jtatomic1,  jtatomic2,   0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK2,VF2NONE),
 /*  +. */  PRIMATOMIC2(CPLUSDOT,VERB, jtrect,    jtatomic2,    0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK2,VF2NONE),
 /*  +: */  PRIMATOMIC2(CPLUSCO, VERB, jtduble,   jtatomic2,    0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  *  */  PRIMATOMIC12(CSTAR,   VERB, jtatomic1,  jtatomic2,  0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  *. */  PRIMATOMIC2(CSTARDOT,VERB, jtpolar,   jtatomic2,    0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK2,VF2NONE),
 /*  *: */  PRIMATOMIC2(CSTARCO, VERB, jtsquare,  jtatomic2,   0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  -  */  PRIMATOMIC2(CMINUS,  VERB, jtnegate,  jtatomic2,  0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  -. */  PRIMACV(CNOT,    VERB, jtnot,     jtless,   0,   RMAX,RMAX,VISATOMIC1|VASGSAFE|VJTFLGOK1,VF2NONE),
 /*  -: */  PRIMACV(CHALVE,  VERB, jthalve,   jtmatch,  0,   RMAX,RMAX,VISATOMIC1|VIRS2|VASGSAFE|VJTFLGOK1,VF2NONE),  // alias CMATCH
 /*  %  */  PRIMATOMIC2(CDIV,    VERB, jtrecip,   jtatomic2, 0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  %. */  PRIMACV(CDOMINO, VERB, jtminv,    jtmdiv,   2,   RMAX,2   ,VASGSAFE,VF2NONE),
 /*  %: */  PRIMATOMIC1(CROOT,   VERB, jtatomic1,  jtroot,   0,   0,   0   ,VISATOMIC1|VASGSAFE|VJTFLGOK1,VF2NONE),
 /*  ^  */  PRIMATOMIC12(CEXP,    VERB, jtatomic1,   jtexpn2,  0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  ^. */  PRIMATOMIC1(CLOG,    VERB, jtatomic1,  jtlogar2, 0,   0,   0   ,VISATOMIC1|VASGSAFE|VJTFLGOK1,VF2NONE),
 /*  ^: */  PRIMACV(CPOWOP,  CONJ, jtdomainerr1, jtpowop,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  $  */  PRIMACV(CDOLLAR, VERB, jtshapex,  jtreitem, RMAX,1,   RMAX,VASGSAFE|VIRS2|VJTFLGOK2,VF2NONE),
 /*  $. */  PRIMACV(CSPARSE, VERB, jtsparse1, jtsparse2,RMAX,RMAX,RMAX,VASGSAFE,VF2NONE),
 /*  $: */  PRIMACV(CSELF,   VERB, jtself1,   jtself2,  RMAX,RMAX,RMAX,VFLAGNONE,VF2NONE),
 /*  ~  */  PRIMACV(CTILDE,  ADV,  jtswap,    jtdomainerr2,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  ~. */  PRIMACV(CNUB,    VERB, jtnub,     jtdomainerr2,  RMAX,RMAX,RMAX,VASGSAFE,VF2NONE),
 /*  ~: */  PRIMATOMIC2(CNE,     VERB, jtnubsieve,jtatomic2,     RMAX,0,   0   ,VFUSEDOK2|VASGSAFE|VIRS1|VIRS2|VJTFLGOK2,VF2NONE),
 /*  |  */  PRIMATOMIC12(CSTILE,  VERB, jtatomic1,     jtresidue,0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  |. */  PRIMACV(CREV,    VERB, jtreverse, jtrotate, RMAX,1,   RMAX,VASGSAFE|VIRS1|VIRS2,VF2NONE),   // alias CROT
 /*  |: */  PRIMACV(CCANT,   VERB, jtcant1,   jtcant2,  RMAX,1,   RMAX,VASGSAFE|VIRS1|VIRS2,VF2NONE),
 /*  .  */  PRIMACV(CDOT,    CONJ, jtdomainerr1, jtdot,    0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  :  */  PRIMACV(CCOLON,  CONJ, jtdomainerr1, jtcolon,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  :. */  PRIMACV(COBVERSE,CONJ, jtdomainerr1, jtobverse,0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  :: */  PRIMACV(CADVERSE,CONJ, jtdomainerr1, jtadverse,0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  ,  */  PRIMACV(CCOMMA,  VERB, jtravel,   jtapip,   RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  ,. */  PRIMACV(CCOMDOT, VERB, jttable,   jtstitch, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VJTFLGOK1,VF2NONE),
 /*  ,: */  PRIMACV(CLAMIN,  VERB, jtlamin1,  jtlamin2, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2,VF2NONE),
 /*  ;  */  PRIMACV(CSEMICO, VERB, jtraze,    jtlink,   RMAX,RMAX,RMAX,VASGSAFE,VF2WILLOPEN1|VF2USESITEMCOUNT1),
 /*  ;. */  PRIMACV(CCUT,    CONJ, jtdomainerr1,        jtcut,    0,   0,   0   ,VFLAGNONE,VF2NONE),
// obsolete /*  ;: */  PRIMACV(CWORDS,  VERB, jtwords,   jtfsm,    1,   RMAX,RMAX,VASGSAFE,VF2NONE),
 /*  ;: */  PRIMACVPARM(CWORDS,  VERB, jtwords,   jtfsm,    1,   RMAX,RMAX,VASGSAFE,VF2NONE,.lpf=~0,0),  // parms are passed through to jtboxcut0
 /*  #  */  PRIMACV(CPOUND,  VERB, jttally,   jtrepeat, RMAX,1,   RMAX,VASGSAFE|VIRS2|VJTFLGOK2,VF2NONE),
 /*  #. */  PRIMACV(CBASE,   VERB, jtbase1,   jtbase2,  1,   1,   1   ,VASGSAFE,VF2NONE),
 /*  #: */  PRIMACV(CABASE,  VERB, jtabase1,  jtabase2, RMAX,1,   0   ,VASGSAFE,VF2NONE),
 /*  !  */  PRIMATOMIC12(CBANG,   VERB, jtatomic1, jtatomic2,  0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  !. */  PRIMACV(CFIT,    CONJ, jtdomainerr1, jtfit,    0,   0,   0   ,VASGSAFE,VF2NONE),
 /*  !: */  PRIMACV(CIBEAM,  CONJ, jtdomainerr1, jtforeign,0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  /  */  PRIMACV(CSLASH,  ADV,  jtslash,   jtdomainerr2, 0,   0,   0   ,VIRS1,VF2NONE),
 /*  /. */  PRIMACV(CSLDOT,  ADV,  jtsldot,   jtdomainerr2, 0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  /: */  PRIMACV(CGRADE,  VERB, jtgrade1,  jtgrade2, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK2,VF2NONE),
 /*  \  */  PRIMACV(CBSLASH, ADV,  jtbslash,  jtdomainerr2, 0,   0,   0   ,VIRS1,VF2NONE),
 /*  \. */  PRIMACV(CBSDOT,  ADV,  jtbsdot,   jtdomainerr2, 0,   0,   0   ,VIRS1,VF2NONE),
 /*  \: */  PRIMACV(CDGRADE, VERB, jtdgrade1, jtdgrade2,RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK2,VF2NONE),
 /*  [  */  PRIMACV(CLEFT,   VERB, jtright1,  jtleft2,  RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VISATOMIC1|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  [: */  PRIMACV(CCAP,    VERB, jtdomainerr1,jtdomainerr2, RMAX,RMAX,RMAX,VFLAGNONE,VF2NONE),   // not ASGSAFE since used for not-yet-defined names
 /*  ]  */  PRIMACV(CRIGHT,  VERB, jtright1,  jtright2, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VISATOMIC1|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  {  */  PRIMACV(CLBRACE, VERB, jtcatalog, jtfrom,   1,   0,   RMAX,VASGSAFE|VIRS2|VJTFLGOK2,VF2NONE),
 /*  {. */  PRIMACV(CHEAD,   VERB, jthead,    jttake,   RMAX,1,   RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2,VF2NONE),  // alias CTAKE
 /*  {: */  PRIMACV(CTAIL,   VERB, jttail,    jtdomainerr2,  RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VJTFLGOK1,VF2NONE),
 /*  }  */  PRIMACV(CRBRACE, ADV,  jtamend,   jtdomainerr2, 0,   0,   0   ,VASGSAFE|VJTFLGOK2,VF2NONE),
 /*  }* */  PRIMACV(CCASEV,  VERB, jtcasev,   jtdomainerr2, RMAX,RMAX,RMAX,VFLAGNONE,VF2NONE),   // f2 gets filled in with pointer to a name when this is used
 /*  }. */  PRIMACV(CBEHEAD, VERB, jtbehead,  jtdrop,   RMAX,1,   RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2,VF2NONE),  // alias CDROP
 /*  }: */  PRIMACV(CCTAIL,  VERB, jtcurtail, jtdomainerr2, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VJTFLGOK1,VF2NONE),
 /*  "  */  PRIMACV(CQQ,     CONJ, jtdomainerr1, jtqq,     0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  ". */  PRIMACV(CEXEC,   VERB, jtexec1,   jtexec2,  1,   RMAX,RMAX,VFLAGNONE,VF2NONE),
 /*  ": */  PRIMACV(CTHORN,  VERB, jtthorn1,  jtthorn2, RMAX,1,   RMAX,VASGSAFE,VF2NONE),
 /*  `  */  PRIMACV(CGRAVE,  CONJ, jtdomainerr1, jttie,    0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  `. */       /* undefined */
 /*  `: */  PRIMACV(CGRCO,   CONJ, jtdomainerr1, jtevger,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  @  */  PRIMACV(CAT,     CONJ, jtdomainerr1, jtatop,   0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  @. */  PRIMACV(CATDOT,  CONJ, jtdomainerr1, jtagendai, 0,   0,   0   ,VFLAGNONE,VF2NONE),
// obsolete  /*  @.: */  PRIMACV(CATDOTCO, CONJ, jtdomainerr1, jtagendai, 0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  @: */  PRIMACV(CATCO,   CONJ, jtdomainerr1, jtatco,   0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  &  */  PRIMACV(CAMP,    CONJ, jtdomainerr1, jtamp,    0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  &. */  PRIMACV(CUNDER,  CONJ, jtdomainerr1, jtunder,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  &: */  PRIMACV(CAMPCO,  CONJ, jtdomainerr1,  jtampco,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  ?  */  PRIMACV(CQUERY,  VERB, jtroll,    jtdeal,   0,   0,   0   ,VISATOMIC1|VASGSAFE,VF2NONE),
 /*  ?. */  PRIMACV(CQRYDOT, VERB, jtrollx,   jtdealx,  RMAX,0,   0   ,VASGSAFE,VF2NONE),
 /*  ?: */       /* undefined */
 /* {:: */  PRIMACV(CFETCH,  VERB, jtmap,     jtfetch,  RMAX,1,   RMAX,VASGSAFE|VJTFLGOK2,VF2NONE),
 /* }:: */  PRIMACV(CEMEND,  ADV,  jtemend,   jtdomainerr2,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /* &.: */  PRIMACV(CUNDCO,  CONJ, jtdomainerr1, jtundco,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  a. */       /* see above */
 /*  a: */       /* see above */
 /*  A. */  PRIMACV(CATOMIC, VERB, jtadot1,   jtadot2,  1,   0,   RMAX,VASGSAFE,VF2NONE),
 /*  b. */  PRIMACV(CBDOT,   ADV,  jtbdot,    jtdomainerr2, 0,   0,   0   ,VASGSAFE,VF2NONE),
 /*  C. */  PRIMACV(CCYCLE,  VERB, jtcdot1,   jtcdot2,  1,   1,   RMAX,VASGSAFE,VF2NONE),
 /*  e. */  PRIMACV(CEPS,    VERB, jtrazein,  jteps,    RMAX,RMAX,RMAX,VASGSAFE|VIRS2,VF2NONE),
 /*  E. */  PRIMACV(CEBAR,   VERB, jtdomainerr1, jtebar,   0,   RMAX,RMAX,VASGSAFE,VF2NONE),
 /*  f. */  PRIMACV(CFIX,    ADV,  jtfix,     jtdomainerr2, 0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  H. */  PRIMACV(CHGEOM,  CONJ, jtdomainerr1, jthgeom,  0,   0,   0   ,VISATOMIC1|VASGSAFE,VF2NONE),
 /*  i. */  PRIMACV(CIOTA,   VERB, jtiota,    jtindexof,1,   RMAX,RMAX,VASGSAFE|VIRS2,VF2NONE),
 /*  i: */  PRIMACV(CICO,    VERB, jtjico1,   jtjico2,  0,   RMAX,RMAX,VASGSAFE|VIRS2,VF2NONE),
 /*  I. */  PRIMACV(CICAP,   VERB, jticap,    jticap2,  1,   RMAX,RMAX,VASGSAFE,VF2NONE),
 /*  j. */  PRIMACV(CJDOT,   VERB, jtjdot1,   jtjdot2,  0,   0,   0   ,VISATOMIC1|VASGSAFE,VF2NONE),
 /*  L. */  PRIMACV(CLDOT,   VERB, jtlevel1,  jtdomainerr2, RMAX,RMAX,RMAX,VASGSAFE,VF2NONE),
 /*  L: */  PRIMACV(CLCAPCO, CONJ, jtdomainerr1, jtlcapco, 0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  m. */       /* see above */
 /*  M. */  PRIMACV(CMCAP,   ADV,  jtmemo,    jtdomainerr2, 0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  n. */       /* see above */
 /*  o. */  PRIMATOMIC12(CCIRCLE, VERB, jtpix,     jtatomic2, 0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  p. */  PRIMACV(CPOLY,   VERB, jtpoly1,   jtpoly2,  1,   1,   0   ,VASGSAFE|VIRS2|VJTFLGOK2,VF2NONE),
 /*  p..*/  PRIMACV(CPDERIV, VERB, jtpderiv1, jtpderiv2,1,   0,   1   ,VASGSAFE,VF2NONE),
 /*  p: */  PRIMACV(CPCO,    VERB, jtprime,   jtpco2,   0,   RMAX,RMAX,VISATOMIC1|VIRS1|VASGSAFE,VF2NONE),
 /*  q: */  PRIMACV(CQCO,    VERB, jtfactor,  jtqco2,   0,   0,   0   ,VISATOMIC1|VASGSAFE,VF2NONE),
 /*  r. */  PRIMACV(CRDOT,   VERB, jtrdot1,   jtrdot2,  0,   0,   0   ,VISATOMIC1|VASGSAFE,VF2NONE),
 /*  s: */  PRIMACV(CSCO,    VERB, jtsb1,     jtsb2,    RMAX,RMAX,RMAX,VASGSAFE,VF2NONE),
 /*  S: */  PRIMACV(CSCAPCO, CONJ, jtdomainerr1, jtscapco, 0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  u. */  PRIMACV(CUDOT,   VERB, jtimplocref,jtimplocref,   RMAX,RMAX,RMAX,VFLAGNONE,VF2NONE),
 /*  u: */  PRIMACV(CUCO,    VERB, jtuco1,    jtuco2,   RMAX,RMAX,RMAX,VASGSAFE,VF2NONE),
 /*  v. */  PRIMACV(CVDOT,   VERB, jtimplocref, jtimplocref,   RMAX,RMAX,RMAX,VFLAGNONE,VF2NONE),
/*  x. */       /* see above */
 /*  x: */  PRIMACV(CXCO,    VERB, jtxco1,    jtxco2,   RMAX,RMAX,RMAX,VASGSAFE|VISATOMIC1,VF2NONE),
 /*  y. */       /* see above */
 /*  F.  */  PRIMACV(CFDOT,  CONJ, jtdomainerr1, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  F.. */  PRIMACV(CFDOTDOT,  CONJ, jtdomainerr1, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  F.: */  PRIMACV(CFDOTCO,  CONJ, jtdomainerr1, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  F:  */  PRIMACV(CFCO,  CONJ, jtdomainerr1, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  F:. */  PRIMACV(CFCODOT,  CONJ, jtdomainerr1, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  F:: */  PRIMACV(CFCOCO,  CONJ, jtdomainerr1, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  Z:  */  PRIMACV(CZCO,  VERB,  jtdomainerr1, jtfoldZ,  RMAX,   RMAX,   RMAX   ,VFLAGNONE,VF2NONE),
};
#endif

B jtpinit(J jt){
#if 0 // obsolete 
// obsolete  MC(wtype,ctype,256L); wtype['N']=CN; wtype['B']=CB;
// obsolete  GAT0(alp,LIT,NALP,1); s=CAV(ds(CALP)); DO(NALP,*s++=(C)i;); ACX(ds(CALP));
// obsolete  /*  a. */  pst[(UC)CALP ]=t=ds(CALP);                 ACX(t);
// obsolete  /*  a: */  pst[(UC)CACE ]=t=ds(CACE)=sc4(BOX,(I)mtv); ACX(t);  AFLAG(t) |= (AT(t)&TRAVERSIBLE);  // ensure that traversible types in pst are marked traversible, so tpush/ra/fa will not recur on them
// obsolete  /*  (  */  pst[(UC)CLPAR]=t=sc4(LPAR,0L);        ACX(t);
// obsolete  /*  )  */  pst[(UC)CRPAR]=t=sc4(RPAR,0L);        ACX(t);
// obsolete  /*  =. */  GA(t,ASGN+ASGNLOCAL,1,0,0); ACX(t); *CAV(t)=CASGN;  pst[(UC)CASGN ]=t;
// obsolete  /*  =: */  GAT0(t,ASGN,1,0); ACX(t); *CAV(t)=CGASGN; pst[(UC)CGASGN]=t;
 /*  a. */  pdef(CALP,LIT,0,0,0,0,0,0); AR(ds(CALP))=1; AS(ds(CALP))[0]=AN(ds(CALP))=NALP; AK(ds(CALP))=alp-(C*)ds(CALP);  // remove the verb settings for the non-verbs
 /*  a: */  pdef(CACE,BOX,0,0,0,0,0,0); AN(ds(CACE))=1; AAV(ds(CACE))[0]=mtv;
 /*  (  */  pdef(CLPAR,LPAR,0,0,0,0,0,0); AN(ds(CLPAR))=1;
 /*  )  */  pdef(CRPAR,RPAR,0,0,0,0,0,0); AN(ds(CRPAR))=1;
 /*  =. */  pdef(CASGN,ASGN+ASGNLOCAL,0,0,0,0,0,0); CAV(ds(CASGN))[0]=CASGN; AN(ds(CASGN))=1;
 /*  =: */  pdef(CGASGN,ASGN,0,0,0,0,0,0); CAV(ds(CGASGN))[0]=CGASGN; AN(ds(CGASGN))=1;

 /*  bw */  pdef(CBW0000, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  bw */  pdef(CBW0001, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  bw */  pdef(CBW0010, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  bw */  pdef(CBW0011, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  bw */  pdef(CBW0100, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  bw */  pdef(CBW0101, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  bw */  pdef(CBW0110, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  bw */  pdef(CBW0111, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  bw */  pdef(CBW1000, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  bw */  pdef(CBW1001, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  bw */  pdef(CBW1010, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  bw */  pdef(CBW1011, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  bw */  pdef(CBW1100, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  bw */  pdef(CBW1101, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  bw */  pdef(CBW1110, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  bw */  pdef(CBW1111, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  =  */  pdef(CEQ,     VERB, jtsclass,  jtatomic2,     RMAX,0,   0   ,VFUSEDOK2|VASGSAFE|VIRS2|VJTFLGOK2);
 /*  =. */       /* see above */
 /*  =: */       /* see above */
 /*  <  */  pdef(CBOX,    VERB, jtbox,     jtatomic2,     RMAX,0,   0   ,VASGSAFE|VFUSEDOK2|VIRS1|VIRS2|VJTFLGOK2);  // alias CLT
 /*  <. */  pdef(CFLOOR,  VERB, jtatomic1,  jtatomic2,0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);  // alias CMIN
 /*  <: */  pdef(CLE,     VERB, jtdecrem,  jtatomic2,     0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  >  */  pdef(COPE,    VERB, jtope,     jtatomic2,     0,   0,   0   ,VFUSEDOK2|VASGSAFE|VIRS2|VJTFLGOK2);  // alias CGT
 /*  >. */  pdef(CCEIL,   VERB, jtatomic1,   jtatomic2,0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);  // alias CMAX
 /*  >: */  pdef(CGE,     VERB, jtincrem,  jtatomic2,     0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  +  */  pdef(CPLUS,   VERB, jtatomic1,  jtatomic2,   0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK2);
 /*  +. */  pdef(CPLUSDOT,VERB, jtrect,    jtatomic2,    0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK2);
 /*  +: */  pdef(CPLUSCO, VERB, jtduble,   jtatomic2,    0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  *  */  pdef(CSTAR,   VERB, jtatomic1,  jtatomic2,  0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  *. */  pdef(CSTARDOT,VERB, jtpolar,   jtatomic2,    0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK2);
 /*  *: */  pdef(CSTARCO, VERB, jtsquare,  jtatomic2,   0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  -  */  pdef(CMINUS,  VERB, jtnegate,  jtatomic2,  0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  -. */  pdef(CNOT,    VERB, jtnot,     jtless,   0,   RMAX,RMAX,VISATOMIC1|VASGSAFE|VJTFLGOK1);
 /*  -: */  pdef(CHALVE,  VERB, jthalve,   jtmatch,  0,   RMAX,RMAX,VISATOMIC1|VIRS2|VASGSAFE|VJTFLGOK1);  // alias CMATCH
 /*  %  */  pdef(CDIV,    VERB, jtrecip,   jtatomic2, 0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  %. */  pdef(CDOMINO, VERB, jtminv,    jtmdiv,   2,   RMAX,2   ,VASGSAFE);
 /*  %: */  pdef(CSQRT,   VERB, jtatomic1,  jtroot,   0,   0,   0   ,VISATOMIC1|VASGSAFE|VJTFLGOK1);
 /*  ^  */  pdef(CEXP,    VERB, jtatomic1,   jtexpn2,  0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  ^. */  pdef(CLOG,    VERB, jtatomic1,  jtlogar2, 0,   0,   0   ,VISATOMIC1|VASGSAFE|VJTFLGOK1);
 /*  ^: */  pdef(CPOWOP,  CONJ, 0L,        jtpowop,  0,   0,   0   ,VFLAGNONE);
 /*  $  */  pdef(CDOLLAR, VERB, jtshapex,  jtreitem, RMAX,1,   RMAX,VASGSAFE|VIRS2|VJTFLGOK2);
 /*  $. */  pdef(CSPARSE, VERB, jtsparse1, jtsparse2,RMAX,RMAX,RMAX,VASGSAFE);
 /*  $: */  pdef(CSELF,   VERB, jtself1,   jtself2,  RMAX,RMAX,RMAX,VFLAGNONE);
 /*  ~  */  pdef(CTILDE,  ADV,  jtswap,    0L,       0,   0,   0   ,VFLAGNONE);
 /*  ~. */  pdef(CNUB,    VERB, jtnub,     0L,       RMAX,RMAX,RMAX,VASGSAFE);
 /*  ~: */  pdef(CNE,     VERB, jtnubsieve,jtatomic2,     RMAX,0,   0   ,VFUSEDOK2|VASGSAFE|VIRS1|VIRS2|VJTFLGOK2);
 /*  |  */  pdef(CSTILE,  VERB, jtatomic1,     jtresidue,0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  |. */  pdef(CREV,    VERB, jtreverse, jtrotate, RMAX,1,   RMAX,VASGSAFE|VIRS1|VIRS2);   // alias CROT
 /*  |: */  pdef(CCANT,   VERB, jtcant1,   jtcant2,  RMAX,1,   RMAX,VASGSAFE|VIRS1|VIRS2);
 /*  .  */  pdef(CDOT,    CONJ, 0L,        jtdot,    0,   0,   0   ,VFLAGNONE);
 /*  :  */  pdef(CCOLON,  CONJ, 0L,        jtcolon,  0,   0,   0   ,VFLAGNONE);
 /*  :. */  pdef(COBVERSE,CONJ, 0L,        jtobverse,0,   0,   0   ,VFLAGNONE);
 /*  :: */  pdef(CADVERSE,CONJ, 0L,        jtadverse,0,   0,   0   ,VFLAGNONE);
 /*  ,  */  pdef(CCOMMA,  VERB, jtravel,   jtapip,   RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2);
 /*  ,. */  pdef(CCOMDOT, VERB, jttable,   jtstitch, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VJTFLGOK1);
 /*  ,: */  pdef(CLAMIN,  VERB, jtlamin1,  jtlamin2, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2);
 /*  ;  */  pdef(CSEMICO, VERB, jtraze,    jtlink,   RMAX,RMAX,RMAX,VASGSAFE);
 /*  ;. */  pdef(CCUT,    CONJ, 0L,        jtcut,    0,   0,   0   ,VFLAGNONE);
 /*  ;: */  pdef(CWORDS,  VERB, jtwords,   jtfsm,    1,   RMAX,RMAX,VASGSAFE);
 /*  #  */  pdef(CPOUND,  VERB, jttally,   jtrepeat, RMAX,1,   RMAX,VASGSAFE|VIRS2|VJTFLGOK2);
 /*  #. */  pdef(CBASE,   VERB, jtbase1,   jtbase2,  1,   1,   1   ,VASGSAFE);
 /*  #: */  pdef(CABASE,  VERB, jtabase1,  jtabase2, RMAX,1,   0   ,VASGSAFE);
 /*  !  */  pdef(CBANG,   VERB, jtatomic1, jtatomic2,  0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  !. */  pdef(CFIT,    CONJ, 0L,        jtfit,    0,   0,   0   ,VASGSAFE);
 /*  !: */  pdef(CIBEAM,  CONJ, 0L,        jtforeign,0,   0,   0   ,VFLAGNONE);
 /*  /  */  pdef(CSLASH,  ADV,  jtslash,   0L,       0,   0,   0   ,VIRS1);
 /*  /. */  pdef(CSLDOT,  ADV,  jtsldot,   0L,       0,   0,   0   ,VFLAGNONE);
 /*  /: */  pdef(CGRADE,  VERB, jtgrade1,  jtgrade2, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK2);
 /*  \  */  pdef(CBSLASH, ADV,  jtbslash,  0L,       0,   0,   0   ,VIRS1);
 /*  \. */  pdef(CBSDOT,  ADV,  jtbsdot,   0L,       0,   0,   0   ,VIRS1);
 /*  \: */  pdef(CDGRADE, VERB, jtdgrade1, jtdgrade2,RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK2);
 /*  [  */  pdef(CLEFT,   VERB, jtright1,  jtleft2,  RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VISATOMIC1|VJTFLGOK1|VJTFLGOK2);
 /*  [: */  pdef(CCAP,    VERB, 0L,        0L,       RMAX,RMAX,RMAX,VFLAGNONE);   // not ASGSAFE since used for not-yet-defined names
 /*  ]  */  pdef(CRIGHT,  VERB, jtright1,  jtright2, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VISATOMIC1|VJTFLGOK1|VJTFLGOK2);
 /*  {  */  pdef(CLBRACE, VERB, jtcatalog, jtfrom,   1,   0,   RMAX,VASGSAFE|VIRS2|VJTFLGOK2);
 /*  {. */  pdef(CHEAD,   VERB, jthead,    jttake,   RMAX,1,   RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2);  // alias CTAKE
 /*  {: */  pdef(CTAIL,   VERB, jttail,    0L,       RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VJTFLGOK1);
 /*  }  */  pdef(CRBRACE, ADV,  jtamend,   0L,       0,   0,   0   ,VASGSAFE|VJTFLGOK2);
 /*  }* */  pdef(CCASEV,  VERB, jtcasev,   0L,       RMAX,RMAX,RMAX,VFLAGNONE);   // f2 gets filled in with pointer to a name when this is used
 /*  }. */  pdef(CBEHEAD, VERB, jtbehead,  jtdrop,   RMAX,1,   RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2);  // alias CDROP
 /*  }: */  pdef(CCTAIL,  VERB, jtcurtail, 0L,       RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VJTFLGOK1);
 /*  "  */  pdef(CQQ,     CONJ, 0L,        jtqq,     0,   0,   0   ,VFLAGNONE);
 /*  ". */  pdef(CEXEC,   VERB, jtexec1,   jtexec2,  1,   RMAX,RMAX,VFLAGNONE);
 /*  ": */  pdef(CTHORN,  VERB, jtthorn1,  jtthorn2, RMAX,1,   RMAX,VASGSAFE);
 /*  `  */  pdef(CGRAVE,  CONJ, 0L,        jttie,    0,   0,   0   ,VFLAGNONE);
 /*  `. */       /* undefined */
 /*  `: */  pdef(CGRCO,   CONJ, 0L,        jtevger,  0,   0,   0   ,VFLAGNONE);
 /*  @  */  pdef(CAT,     CONJ, 0L,        jtatop,   0,   0,   0   ,VFLAGNONE);
 /*  @. */  pdef(CATDOT,  CONJ, 0L,        jtagenda, 0,   0,   0   ,VIRS1);
 /*  @: */  pdef(CATCO,   CONJ, 0L,        jtatco,   0,   0,   0   ,VFLAGNONE);
 /*  &  */  pdef(CAMP,    CONJ, 0L,        jtamp,    0,   0,   0   ,VFLAGNONE);
 /*  &. */  pdef(CUNDER,  CONJ, 0L,        jtunder,  0,   0,   0   ,VFLAGNONE);
 /*  &: */  pdef(CAMPCO,  CONJ, 0L,        jtampco,  0,   0,   0   ,VFLAGNONE);
 /*  ?  */  pdef(CQUERY,  VERB, jtroll,    jtdeal,   0,   0,   0   ,VISATOMIC1|VASGSAFE);
 /*  ?. */  pdef(CQRYDOT, VERB, jtrollx,   jtdealx,  RMAX,0,   0   ,VASGSAFE);
 /*  ?: */       /* undefined */
 /* {:: */  pdef(CFETCH,  VERB, jtmap,     jtfetch,  RMAX,1,   RMAX,VASGSAFE|VJTFLGOK2);
 /* }:: */  pdef(CEMEND,  ADV,  jtemend,   0L,       0,   0,   0   ,VFLAGNONE);
 /* &.: */  pdef(CUNDCO,  CONJ, 0L,        jtundco,  0,   0,   0   ,VFLAGNONE);
 /*  a. */       /* see above */
 /*  a: */       /* see above */
 /*  A. */  pdef(CATOMIC, VERB, jtadot1,   jtadot2,  1,   0,   RMAX,VASGSAFE);
 /*  b. */  pdef(CBDOT,   ADV,  jtbdot,    0L,       0,   0,   0   ,VASGSAFE);
 /*  C. */  pdef(CCYCLE,  VERB, jtcdot1,   jtcdot2,  1,   1,   RMAX,VASGSAFE);
 /*  e. */  pdef(CEPS,    VERB, jtrazein,  jteps,    RMAX,RMAX,RMAX,VASGSAFE|VIRS2);
 /*  E. */  pdef(CEBAR,   VERB, 0L,        jtebar,   0,   RMAX,RMAX,VASGSAFE);
 /*  f. */  pdef(CFIX,    ADV,  jtfix,     0L,       0,   0,   0   ,VFLAGNONE);
 /*  H. */  pdef(CHGEOM,  CONJ, 0L,        jthgeom,  0,   0,   0   ,VISATOMIC1|VASGSAFE);
 /*  i. */  pdef(CIOTA,   VERB, jtiota,    jtindexof,1,   RMAX,RMAX,VASGSAFE|VIRS2);
 /*  i: */  pdef(CICO,    VERB, jtjico1,   jtjico2,  0,   RMAX,RMAX,VASGSAFE|VIRS2);
 /*  I. */  pdef(CICAP,   VERB, jticap,    jticap2,  1,   RMAX,RMAX,VASGSAFE);
 /*  j. */  pdef(CJDOT,   VERB, jtjdot1,   jtjdot2,  0,   0,   0   ,VISATOMIC1|VASGSAFE);
 /*  L. */  pdef(CLDOT,   VERB, jtlevel1,  0L,       RMAX,RMAX,RMAX,VASGSAFE);
 /*  L: */  pdef(CLCAPCO, CONJ, 0L,        jtlcapco, 0,   0,   0   ,VFLAGNONE);
 /*  m. */       /* see above */
 /*  M. */  pdef(CMCAP,   ADV,  jtmemo,    0L,       0,   0,   0   ,VFLAGNONE);
 /*  n. */       /* see above */
 /*  o. */  pdef(CCIRCLE, VERB, jtpix,     jtatomic2, 0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2);
 /*  p. */  pdef(CPOLY,   VERB, jtpoly1,   jtpoly2,  1,   1,   0   ,VASGSAFE|VIRS2|VJTFLGOK2);
 /*  p..*/  pdef(CPDERIV, VERB, jtpderiv1, jtpderiv2,1,   0,   1   ,VASGSAFE);
 /*  p: */  pdef(CPCO,    VERB, jtprime,   jtpco2,   0,   RMAX,RMAX,VISATOMIC1|VIRS1|VASGSAFE);
 /*  q: */  pdef(CQCO,    VERB, jtfactor,  jtqco2,   0,   0,   0   ,VISATOMIC1|VASGSAFE);
 /*  r. */  pdef(CRDOT,   VERB, jtrdot1,   jtrdot2,  0,   0,   0   ,VISATOMIC1|VASGSAFE);
 /*  s: */  pdef(CSCO,    VERB, jtsb1,     jtsb2,    RMAX,RMAX,RMAX,VASGSAFE);
 /*  S: */  pdef(CSCAPCO, CONJ, 0L,        jtscapco, 0,   0,   0   ,VFLAGNONE);
 /*  u. */  pdef(CUDOT,   VERB, jtimplocref,    jtimplocref,   RMAX,RMAX,RMAX,VFLAGNONE);
 /*  u: */  pdef(CUCO,    VERB, jtuco1,    jtuco2,   RMAX,RMAX,RMAX,VASGSAFE);
 /*  v. */  pdef(CVDOT,   VERB, jtimplocref,    jtimplocref,   RMAX,RMAX,RMAX,VFLAGNONE);
/*  x. */       /* see above */
 /*  x: */  pdef(CXCO,    VERB, jtxco1,    jtxco2,   RMAX,RMAX,RMAX,VASGSAFE|VISATOMIC1);
 /*  y. */       /* see above */
 /*  F.  */  pdef(CFDOT,  CONJ, 0,    jtfold,  0,   0,   0   ,VFLAGNONE);
 /*  F.. */  pdef(CFDOTDOT,  CONJ, 0,    jtfold,  0,   0,   0   ,VFLAGNONE);
 /*  F.: */  pdef(CFDOTCO,  CONJ, 0,    jtfold,  0,   0,   0   ,VFLAGNONE);
 /*  F:  */  pdef(CFCO,  CONJ, 0,    jtfold,  0,   0,   0   ,VFLAGNONE);
 /*  F:. */  pdef(CFCODOT,  CONJ, 0,    jtfold,  0,   0,   0   ,VFLAGNONE);
 /*  F:: */  pdef(CFCOCO,  CONJ, 0,    jtfold,  0,   0,   0   ,VFLAGNONE);
 /*  Z:  */  pdef(CZCO,  VERB,  0,    jtfoldZ,  RMAX,   RMAX,   RMAX   ,VFLAGNONE);

// modify the BOX verb so that it is flagged BOXATOP (for result purposes), with a g field of ].  Result verbs will
// treat it as <@], but normal processing as <
FAV(ds(CBOX))->flag2 |= VF2BOXATOP1; FAV(ds(CBOX))->fgh[1]=ds(CRIGHT);
// the verbs (all monads) that open their operand are flagged so that their compounds can also be flagged
FAV(ds(CSEMICO))->flag2 |= VF2WILLOPEN1|VF2USESITEMCOUNT1;
FAV(ds(COPE))->flag2 |= VF2WILLOPEN1;
 if(jt->jerr){printf("pinit failed; error %hhi\n", jt->jerr); R 0;} else R 1;
#endif
 AS((A)&primtab[CALP])[0]=NALP;  AK((A)&primtab[CALP])=(I)alp-(I)&primtab[CALP];
 R 1;
}
