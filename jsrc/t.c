/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Table of Primitive Symbols                                              */

#include "j.h"
#include "ve.h"
#include "vcomp.h"


C __attribute__((aligned(CACHELINESIZE))) ctype[256]={
CX, CX, CX, CX, CX, CX, CX, CX, CX, CS, CU, CX,  CX, CX,   CX, CX, /*CX       TAB LF DDSEP*/
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX,  CX, CX,   CX, CX, /* 1                  */
CS, CX, CX, CX, CX, CX, CX, CQ, CX, CX, CX, CX,  CX, CX,   CD, CX, /* 2  !"#$%&'()*+,-./ */
C9, C9, C9, C9, C9, C9, C9, C9, C9, C9, CC, CX,  CX, CX,   CX, CX, /* 3 0123456789:;<=>? */
CX, CA, CB, CA, CA, CA, CA, CA, CA, CA, CA, CA,  CA, CA,   CN, CA, /* 4 @ABCDEFGHIJKLMNO */
CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CX,  CX, CX,   CX, C9, /* 5 PQRSTUVWXYZ[\]^_ */
CX, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA,  CA, CA,   CA, CA, /* 6 `abcdefghijklmno */
CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CDD, CX, CDDZ, CX, CX, /* 7 pqrstuvwxyz{|}~  */
// no {}  CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CX, CX, CX, CX, CX, /* 7 pqrstuvwxyz{|}~  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX,  CX, CX,   CX, CX, /* 8                  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX,  CX, CX,   CX, CX, /* 9                  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX,  CX, CX,   CX, CX, /* a                  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX,  CX, CX,   CX, CX, /* b                  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX,  CX, CX,   CX, CX, /* c                  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX,  CX, CX,   CX, CX, /* d                  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX,  CX, CX,   CX, CX, /* e                  */
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX,  CX, CX,   CX, CX, /* f                  */
}; /*
 0   1   2   3   4   5   6   7   8   9   a   b    c   d     e   f   */

static __attribute__((aligned(CACHELINESIZE))) const C alp[256]={
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff};

#define PRIMNOUN(id,t,an,ar,val) [id]={{AKXR(ar),(t)&TRAVERSIBLE,0,(t),ACPERMANENT,(an),(ar)} , {.primint=val} }
#define PRIMNOUNFL(id,t,an,ar,val) [id]={{AKXR(ar),(t)&TRAVERSIBLE,0,(t),ACPERMANENT,(an),(ar)} , {.primfl=val} }
#define PRIMALL(idda,idd,t,f0,f1,f,g,h,initpm1,initpm2,rm,rl,rr,vflg,vflg2,an,ar,lcc) \
 [idda]={{AKXR(ar),(t)&TRAVERSIBLE,0,(t),ACPERMANENT,(an),(ar)},{{.valencefns={f0,f1},.fgh={f,g,h},.localuse={initpm1,initpm2},.flag=(vflg),.flag2=(vflg2),.lrr=(RANK2T)((rl<<RANKTX)+rr),.mr=(RANKT)rm,.id=idd,.lu2.lc=lcc}}}
#define PRIMACV(id,t,f0,f1,rm,rl,rr,vflg,vflg2) PRIMALL(id,id,t,f0,f1,0,0,0,.lu1.uavandx=0,0,rm,rl,rr,vflg,vflg2,0,0,0)
#define PRIMACVPARM(id,t,f0,f1,rm,rl,rr,vflg,vflg2,initpm1,initpm2) PRIMALL(id,id,t,f0,f1,0,0,0,initpm1,initpm2,rm,rl,rr,vflg,vflg2,0,0,0)
#define PRIMATOMIC2(vaid,id,t,f0,f1,rm,rl,rr,vflg,vflg2) PRIMALL(vaid,id,t,f0,f1,0,0,0,.lu1.uavandx=0,VA2##vaid*sizeof(VA),rm,rl,rr,VISATOMIC2|(vflg),vflg2,0,0, \
 VA2##vaid+0x80*(vaid==CLT||id==CGT||id==CLE||id==CGE||id==CEQ||id==CNE))
#define PRIMATOMIC2FGH(vaid,id,t,f0,f1,f,g,h,rm,rl,rr,vflg,vflg2) PRIMALL(vaid,id,t,f0,f1,f,g,h,.lu1.uavandx=0,VA2##vaid*sizeof(VA),rm,rl,rr,VISATOMIC2|(vflg),vflg2,0,0, \
 VA2##vaid+0x80*(vaid==CLT||id==CGT||id==CLE||id==CGE||id==CEQ||id==CNE))
#define PRIMATOMIC2g(id,t,f0,f1,rm,rl,rr,vflg,vflg2,g) PRIMALL(id,id,t,f0,f1,0,g,0,.lu1.uavandx=0,VA2##id*sizeof(VA),rm,rl,rr,VISATOMIC2|(vflg),vflg2,0,0, \
 VA2##id+0x80*(id==CLT||id==CGT||id==CLE||id==CGE||id==CEQ||id==CNE))
// used for -QP to avoid the subtract
#define PRIMATOMIC2UV(vaid,id,uvid,t,f0,f1,rm,rl,rr,vflg,vflg2) PRIMALL(vaid,id,t,f0,f1,0,0,0,.lu1.uavandx=(uvid)*sizeof(UA),VA2##vaid*sizeof(VA),rm,rl,rr,VISATOMIC2|(vflg),vflg2,0,0, \
 VA2##vaid+0x80*(vaid==CLT||id==CGT||id==CLE||id==CGE||id==CEQ||id==CNE))
#define PRIMATOMIC1(id,t,f0,f1,rm,rl,rr,vflg,vflg2) PRIMALL(id,id,t,f0,f1,0,0,0,.lu1.uavandx=(VA1##id-VA1ORIGIN)*sizeof(UA),0,rm,rl,rr,vflg,vflg2,0,0, \
 VA1##id+0x80*(id==CLT||id==CGT||id==CLE||id==CGE||id==CEQ||id==CNE))
#define PRIMATOMIC12(id,t,f0,f1,rm,rl,rr,vflg,vflg2) PRIMALL(id,id,t,f0,f1,0,0,0,.lu1.uavandx=(VA1##id-VA1ORIGIN)*sizeof(UA),VA2##id*sizeof(VA),rm,rl,rr,VISATOMIC2|(vflg),vflg2,0,0, \
 VA2##id+0x80*(id==CLT||id==CGT||id==CLE||id==CGE||id==CEQ||id==CNE))

PRIM primtab[256] = {
 /*  (  */  PRIMNOUN(CLPAR,LPAR,1,0,0),
 /*  )  */  PRIMNOUN(CRPAR,RPAR,1,0,0),
 /*  =. */  PRIMNOUN(CASGN,ASGN+ASGNLOCAL,1,0,CASGN),
 /*  =: */  PRIMNOUN(CGASGN,ASGN,1,0,CGASGN),
 /*  a: */  PRIMNOUN(CACE,BOX,1,0,(I)mtv),
 /*  a. */  PRIMNOUN(CALP,LIT,NALP,1,0),   // this gets modified at init time
 /*  _.  */  PRIMNOUNFL(CUSDOT,FL,1,0,0x7ff8000000000000),  // quiet NaN, since NAN caused trouble in Windows SDKs

 /*  bw */  PRIMATOMIC2(CBW0000,CBW0000, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  bw */  PRIMATOMIC2(CBW0001,CBW0001, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  bw */  PRIMATOMIC2(CBW0010,CBW0010, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  bw */  PRIMATOMIC2(CBW0011,CBW0011, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  bw */  PRIMATOMIC2(CBW0100,CBW0100, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  bw */  PRIMATOMIC2(CBW0101,CBW0101, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  bw */  PRIMATOMIC2(CBW0110,CBW0110, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  bw */  PRIMATOMIC2(CBW0111,CBW0111, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  bw */  PRIMATOMIC2(CBW1000,CBW1000, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  bw */  PRIMATOMIC2(CBW1001,CBW1001, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  bw */  PRIMATOMIC2(CBW1010,CBW1010, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  bw */  PRIMATOMIC2(CBW1011,CBW1011, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  bw */  PRIMATOMIC2(CBW1100,CBW1100, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  bw */  PRIMATOMIC2(CBW1101,CBW1101, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  bw */  PRIMATOMIC2(CBW1110,CBW1110, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  bw */  PRIMATOMIC2(CBW1111,CBW1111, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  =  */  PRIMATOMIC2(CEQ,CEQ,     VERB, jtsclass,  jtatomic2,     RMAX,0,   0   ,VFUSEDOK2|VASGSAFE|VIRS2|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  =. */       /* see above */
 /*  =: */       /* see above */
 /*  <  */  PRIMATOMIC2g(CLT,    VERB, jtbox,     jtatomic2,     RMAX,0,   0   ,VASGSAFE|VFUSEDOK2|VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2,VF2BOXATOP1|VF2PRIM,ds(CRIGHT)),  // alias CLT
 /*  <. */  PRIMATOMIC12(CMIN,  VERB, jtatomic1,  jtatomic2,0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),  // alias CMIN
 /*  <: */  PRIMATOMIC2(CLE,CLE,     VERB, jtdecrem,  jtatomic2,     0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  >  */  PRIMATOMIC2(CGT,CGT,    VERB, jtope,     jtatomic2,     0,   0,   0   ,VFUSEDOK2|VASGSAFE|VIRS2|VJTFLGOK2,VF2WILLOPEN1|VF2PRIM),  // alias CGT
 /*  >. */  PRIMATOMIC12(CMAX,   VERB, jtatomic1,   jtatomic2,0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),  // alias CMAX
 /*  >: */  PRIMATOMIC2(CGE,CGE,     VERB, jtincrem,  jtatomic2,     0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  +  */  PRIMATOMIC12(CPLUS,   VERB, jtatomic1,  jtatomic2,   0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  +. */  PRIMATOMIC2(CPLUSDOT,CPLUSDOT,VERB, jtrect,    jtatomic2,    0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  +: */  PRIMATOMIC2(CPLUSCO,CPLUSCO, VERB, jtduble,   jtatomic2,    0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  *  */  PRIMATOMIC12(CSTAR,   VERB, jtatomic1,  jtatomic2,  0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  *. */  PRIMATOMIC2(CSTARDOT,CSTARDOT,VERB, jtpolar,   jtatomic2,    0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  *: */  PRIMATOMIC2(CSTARCO,CSTARCO, VERB, jtsquare,  jtatomic2,   0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  -  */  PRIMATOMIC2UV(CMINUS,CMINUS,VA1CNEG-VA1ORIGIN,  VERB, jtnegate,  jtatomic2,  0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  -. */  PRIMACV(CNOT,    VERB, jtnot,     jtless,   0,   RMAX,RMAX,VISATOMIC1|VASGSAFE|VJTFLGOK1|((7+(((ILESS-II0EPS)&0xf)<<3))&-SY_64),VF2NONE|VF2PRIM),  // native compound allowing &n - 64-bit only
 /*  -: */  PRIMACV(CHALVE,  VERB, jthalve,   jtmatch,  0,   RMAX,RMAX,VISATOMIC1|VIRS2|VASGSAFE|VJTFLGOK1,VF2NONE|VF2PRIM),  // alias CMATCH
 /*  %  */  PRIMATOMIC2UV(CDIV,CDIV,VA1CRECIP-VA1ORIGIN,    VERB, jtrecip,   jtatomic2, 0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  %. */  PRIMACV(CDOMINO, VERB, jtminv,    jtmdiv,   2,   RMAX,2   ,VASGSAFE,VF2NONE|VF2PRIM),
 /*  %: */  PRIMATOMIC1(CROOT,   VERB, jtatomic1,  jtroot,   0,   0,   0   ,VISATOMIC1|VASGSAFE|VJTFLGOK1,VF2NONE|VF2PRIM),
 /*  ^  */  PRIMATOMIC12(CEXP,    VERB, jtatomic1,   jtexpn2,  0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  ^. */  PRIMATOMIC1(CLOG,    VERB, jtatomic1,  jtlogar2, 0,   0,   0   ,VISATOMIC1|VASGSAFE|VJTFLGOK1,VF2NONE|VF2PRIM),
 /*  ^: */  PRIMACV(CPOWOP,  CONJ, jtvalenceerr, jtpowop,  0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  $  */  PRIMACV(CDOLLAR, VERB, jtshapex,  jtreitem, RMAX,1,   RMAX,VASGSAFE|VIRS2|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  $. */  PRIMACV(CSPARSE, VERB, jtsparse1, jtsparse2,RMAX,RMAX,RMAX,VASGSAFE,VF2NONE|VF2PRIM),
 /*  $: */  PRIMACV(CSELF,   VERB, jtself12,   jtself12,  RMAX,RMAX,RMAX,VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  ~  */  PRIMACV(CTILDE,  ADV,  jtswap,    jtvalenceerr,  0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  ~. */  PRIMACV(CNUB,    VERB, jtnub,     jtvalenceerr,  RMAX,RMAX,RMAX,VASGSAFE|VJTFLGOK1,VF2NONE|VF2PRIM),
 /*  ~: */  PRIMATOMIC2(CNE,CNE,     VERB, jtnubsieve,jtatomic2,     RMAX,0,   0   ,VFUSEDOK2|VASGSAFE|VIRS1|VIRS2|VJTFLGOK2,VF2WILLOPEN1|VF2PRIM),
 /*  |  */  PRIMATOMIC12(CSTILE,  VERB, jtatomic1,     jtresidue,0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  |. */  PRIMACV(CREV,    VERB, jtreverse, jtrotate, RMAX,1,   RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2,VF2WILLOPEN1PROP|VF2PRIM),   // alias CROT
 /*  |: */  PRIMACV(CCANT,   VERB, jtcant1,   jtcant2,  RMAX,1,   RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2,VF2WILLOPEN1PROP|VF2WILLOPEN2A|VF2PRIM),
 /*  .  */  PRIMACV(CDOT,    CONJ, jtvalenceerr, jtdot,    0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  :  */  PRIMACV(CCOLON,  CONJ, jtvalenceerr, jtcolon,  0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  :. */  PRIMACV(COBVERSE,CONJ, jtvalenceerr, jtobverse,0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  :: */  PRIMACV(CADVERSE,CONJ, jtvalenceerr, jtadverse,0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  ,  */  PRIMACV(CCOMMA,  VERB, jtravel,   jtapip,   RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2,VF2WILLOPEN1PROP|VF2PRIM),  // could VF2WILLOPEN2?PROP but sparse code may EPILOG
 /*  ,. */  PRIMACV(CCOMDOT, VERB, jttable,   jtstitch, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  ,: */  PRIMACV(CLAMIN,  VERB, jtlamin1,  jtlamin2, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK1,VF2NONE|VF2PRIM),
 /*  ;  */  PRIMACV(CSEMICO, VERB, jtraze,    jtjlink,  RMAX,RMAX,RMAX,VASGSAFE|VJTFLGOK2,VF2USESITEMCOUNT1|VF2WILLOPEN1|VF2WILLOPEN2WPROP|VF2PRIM),
 /*  ;. */  PRIMACV(CCUT,    CONJ, jtvalenceerr,        jtcut,    0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  ;: */  PRIMACVPARM(CWORDS,  VERB, jtwords,   jtfsm,    1,   RMAX,RMAX,VASGSAFE,VF2NONE|VF2PRIM,.boxcut0=0,0),  // parms are passed through to jtboxcut0
 /*  #  */  PRIMACV(CPOUND,  VERB, jttally,   jtrepeat, RMAX,1,   RMAX,VASGSAFE|VIRS2|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  #. */  PRIMACV(CBASE,   VERB, jtbase1,   jtbase2,  1,   1,   1   ,VASGSAFE,VF2NONE|VF2PRIM),
 /*  #: */  PRIMACV(CABASE,  VERB, jtabase1,  jtabase2, RMAX,1,   0   ,VASGSAFE,VF2NONE|VF2PRIM),
 /*  !  */  PRIMATOMIC12(CBANG,   VERB, jtatomic1, jtatomic2,  0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  !. */  PRIMACV(CFIT,    CONJ, jtvalenceerr, jtfit,    0,   0,   0   ,VASGSAFE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  !: */  PRIMACV(CIBEAM,  CONJ, jtvalenceerr, jtforeign,0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  /  */  PRIMACV(CSLASH,  ADV,  jtslash,   jtvalenceerr, 0,   0,   0   ,VIRS1,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  /. */  PRIMACV(CSLDOT,  ADV,  jtsldot,   jtvalenceerr, 0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  /: */  PRIMACV(CGRADE,  VERB, jtgrade1,  jtgrade2, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  /.. */  PRIMACV(CSLDOTDOT,  ADV,  jtsldotdot,   jtvalenceerr, 0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  \  */  PRIMACV(CBSLASH, ADV,  jtbslash,  jtvalenceerr, 0,   0,   0   ,VIRS1,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  \. */  PRIMACV(CBSDOT,  ADV,  jtbsdot,   jtvalenceerr, 0,   0,   0   ,VIRS1,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  \: */  PRIMACV(CDGRADE, VERB, jtdgrade1, jtdgrade2,RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  [  */  PRIMACV(CLEFT,   VERB, jtright1,  jtleft2,  RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VISATOMIC1|VJTFLGOK1|VJTFLGOK2,VF2WILLOPEN1PROP|VF2WILLOPEN2APROP|VF2PRIM),
 /*  [. */  PRIMACV(CLEV,    CONJ,  0,   jtlev, 0,   0,   0   ,VASGSAFE|VJTFLGOK2,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  [: */  PRIMACV(CCAP,    VERB, jtvalenceerr,jtvalenceerr, RMAX,RMAX,RMAX,VFLAGNONE,VF2NONE|VF2PRIM),   // not ASGSAFE since used for not-yet-defined names
 /*  ]  */  PRIMACV(CRIGHT,  VERB, jtright1,  jtright2, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VISATOMIC1|VJTFLGOK1|VJTFLGOK2,VF2WILLOPEN1PROP|VF2WILLOPEN2WPROP|VF2PRIM),
 /*  ]. */  PRIMACV(CDEX,    CONJ,  0,   jtdex, 0,   0,   0   ,VASGSAFE|VJTFLGOK2,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  ]. */  PRIMACV(CIDENT,  ADV,   jtlev,   0, 0,   0,   0   ,VASGSAFE|VJTFLGOK2,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  {  */  PRIMACV(CLBRACE, VERB, jtcatalog, jtfrom,   1,   0,   RMAX,VASGSAFE|VIRS2|VJTFLGOK2,VF2WILLOPEN2A|VF2PRIM),
 /*  {. */  PRIMACV(CHEAD,   VERB, jthead,    jttake,   RMAX,1,   RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),  // alias CTAKE
 /*  {: */  PRIMACV(CTAIL,   VERB, jttail,    jtvalenceerr,  RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VJTFLGOK1,VF2NONE|VF2PRIM),
 /*  }  */  PRIMACV(CRBRACE, ADV,  jtamend,   jtvalenceerr, 0,   0,   0   ,VASGSAFE|VJTFLGOK2,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  }* */  PRIMACV(CCASEV,  VERB, jtcasev,   jtvalenceerr, RMAX,RMAX,RMAX,VFLAGNONE,VF2NONE|VF2PRIM),   // f2 gets filled in with pointer to a name when this is used
 /*  }. */  PRIMACV(CBEHEAD, VERB, jtbehead,  jtdrop,   RMAX,1,   RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),  // alias CDROP
 /*  }: */  PRIMACV(CCTAIL,  VERB, jtcurtail, jtvalenceerr, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VJTFLGOK1,VF2NONE|VF2PRIM),
 /*  "  */  PRIMACV(CQQ,     CONJ, jtvalenceerr, jtqq,     0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  ". */  PRIMACV(CEXEC,   VERB, jtexec1,   jtexec2,  1,   RMAX,RMAX,VFLAGNONE,VF2NONE|VF2PRIM),
 /*  ": */  PRIMACV(CTHORN,  VERB, jtthorn1,  jtthorn2, RMAX,1,   RMAX,VASGSAFE,VF2NONE|VF2PRIM),
 /*  `  */  PRIMACV(CGRAVE,  CONJ, jtvalenceerr, jttie,    0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  `. */       /* undefined */
 /*  `: */  PRIMACV(CGRCO,   CONJ, jtvalenceerr, jtevger,  0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  @  */  PRIMACV(CAT,     CONJ, jtvalenceerr, jtatop,   0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  @. */  PRIMACV(CATDOT,  CONJ, jtvalenceerr, jtagendai, 0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  @: */  PRIMACV(CATCO,   CONJ, jtvalenceerr, jtatco,   0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  &  */  PRIMACV(CAMP,    CONJ, jtvalenceerr, jtamp,    0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  &. */  PRIMACV(CUNDER,  CONJ, jtvalenceerr, jtunder,  0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  &: */  PRIMACV(CAMPCO,  CONJ, jtvalenceerr,  jtampco,  0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  ?  */  PRIMACV(CQUERY,  VERB, jtroll,    jtdeal,   0,   0,   0   ,VISATOMIC1|VASGSAFE,VF2NONE|VF2PRIM),
 /*  ?. */  PRIMACV(CQRYDOT, VERB, jtrollx,   jtdealx,  RMAX,0,   0   ,VASGSAFE,VF2NONE|VF2PRIM),
 /*  ?: */       /* undefined */
 /* {:: */  PRIMACV(CFETCH,  VERB, jtmap,     jtfetch,  RMAX,1,   RMAX,VASGSAFE|VJTFLGOK2,VF2NONE|VF2PRIM),
// unsupported /* }:: */  PRIMACV(CEMEND,  ADV,  jtemend,   jtvalenceerr,  0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /* &.: */  PRIMACV(CUNDCO,  CONJ, jtvalenceerr, jtundco,  0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  a. */       /* see above */
 /*  a: */       /* see above */
 /*  A. */  PRIMACV(CATOMIC, VERB, jtadot1,   jtadot2,  1,   0,   RMAX,VASGSAFE,VF2NONE|VF2PRIM),
 /*  b. */  PRIMACV(CBDOT,   ADV,  jtbdot,    jtvalenceerr, 0,   0,   0   ,VASGSAFE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  c. */  PRIMACV(CCDOT,   VERB, jtvalenceerr,jtcdot2,   RMAX,RMAX,RMAX,VFLAGNONE,VF2PRIM),
 /*  C. */  PRIMACV(CCYCLE,  VERB, jtcdot1,   jtccapdot2,  1,   1,   RMAX,VASGSAFE,VF2NONE|VF2PRIM),
 /*  e. */  PRIMACV(CEPS,    VERB, jtrazein,  jteps,    RMAX,RMAX,RMAX,VASGSAFE|VIRS2|(7+(((IEPS-II0EPS)&0xf)<<3)),VF2WILLOPEN2A|VF2WILLOPEN2W|VF2PRIM),
 /*  E. */  PRIMACV(CEBAR,   VERB, jtvalenceerr, jtebar,   0,   RMAX,RMAX,VASGSAFE,VF2WILLOPEN2A|VF2WILLOPEN2W|VF2PRIM),
 /*  f. */  PRIMACV(CFIX,    ADV,  jtfix,     jtvalenceerr, 0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  H. */  PRIMACV(CHGEOM,  CONJ, jtvalenceerr, jthgeom,  0,   0,   0   ,VISATOMIC1|VASGSAFE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  i. */  PRIMACV(CIOTA,   VERB, jtiota,    jtindexof,1,   RMAX,RMAX,VASGSAFE|VIRS2,VF2WILLOPEN2A|VF2WILLOPEN2W|VF2PRIM),
 /*  i: */  PRIMACV(CICO,    VERB, jtjico1,   jtjico2,  0,   RMAX,RMAX,VASGSAFE|VIRS2,VF2WILLOPEN2A|VF2WILLOPEN2W|VF2PRIM),
 /*  I. */  PRIMACV(CICAP,   VERB, jticap,    jticap2,  1,   RMAX,RMAX,VASGSAFE,VF2NONE|VF2PRIM),
 /*  j. */  PRIMACV(CJDOT,   VERB, jtjdot1,   jtjdot2,  0,   0,   0   ,VISATOMIC1|VASGSAFE,VF2NONE|VF2PRIM),
 /*  L. */  PRIMACV(CLDOT,   VERB, jtlevel1,  jtvalenceerr, RMAX,RMAX,RMAX,VASGSAFE,VF2WILLOPEN1|VF2PRIM),
 /*  L: */  PRIMACV(CLCAPCO, CONJ, jtvalenceerr, jtlcapco, 0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  m. */  PRIMACV(CMDOT,   CONJ, jtvalenceerr, jtmdot, 0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  M. */  PRIMACV(CMCAP,   ADV,  jtmemo,    jtvalenceerr, 0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  o. */  PRIMATOMIC12(CCIRCLE, VERB, jtpix, jtatomic2, 0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  p. */  PRIMACV(CPOLY,   VERB, jtpoly1,   jtpoly2,  1,   1,   0   ,VASGSAFE|VIRS2|VJTFLGOK2,VF2NONE|VF2PRIM),
 /*  p..*/  PRIMACV(CPDERIV, VERB, jtpderiv1, jtpderiv2,1,   0,   1   ,VASGSAFE,VF2NONE|VF2PRIM),
 /*  p: */  PRIMACV(CPCO,    VERB, jtprime,   jtpco2,   0,   RMAX,RMAX,VISATOMIC1|VIRS1|VASGSAFE,VF2NONE|VF2PRIM),
 /*  q: */  PRIMACV(CQCO,    VERB, jtfactor,  jtqco2,   RMAX,   0,   0   ,VISATOMIC1|VASGSAFE,VF2NONE|VF2PRIM),
 /*  r. */  PRIMACV(CRDOT,   VERB, jtrdot1,   jtrdot2,  0,   0,   0   ,VISATOMIC1|VASGSAFE,VF2NONE|VF2PRIM),
 /*  s: */  PRIMACV(CSCO,    VERB, jtsb1,     jtsb2,    RMAX,RMAX,RMAX,VASGSAFE,VF2NONE|VF2PRIM),
 /*  S: */  PRIMACV(CSCAPCO, CONJ, jtvalenceerr, jtscapco, 0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  t. */  PRIMACV(CTDOT,   CONJ, jtvalenceerr, jttdot, 0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  T. */  PRIMACV(CTCAPDOT, VERB, jttcapdot1, jttcapdot2, RMAX, RMAX,  RMAX   ,VFLAGNONE,VF2NONE|VF2PRIM),
 /*  u. */  PRIMACV(CUDOT,   VERB, jtimplocref,jtimplocref,   RMAX,RMAX,RMAX,VFLAGNONE,VF2IMPLOC|VF2PRIM),
 /*  u: */  PRIMACV(CUCO,    VERB, jtuco1,    jtuco2,   RMAX,RMAX,RMAX,VASGSAFE,VF2NONE|VF2PRIM),
 /*  v. */  PRIMACV(CVDOT,   VERB, jtimplocref, jtimplocref,   RMAX,RMAX,RMAX,VFLAGNONE,VF2IMPLOC|VF2PRIM),
 /*  x: */  PRIMACV(CXCO,    VERB, jtxco1,    jtxco2,   RMAX,RMAX,RMAX,VASGSAFE|VISATOMIC1,VF2NONE|VF2PRIM),
 /*  F.  */  PRIMACV(CFDOT,  CONJ, jtvalenceerr, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  F.. */  PRIMACV(CFDOTDOT,  CONJ, jtvalenceerr, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  F.: */  PRIMACV(CFDOTCO,  CONJ, jtvalenceerr, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  F:  */  PRIMACV(CFCO,  CONJ, jtvalenceerr, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  F:. */  PRIMACV(CFCODOT,  CONJ, jtvalenceerr, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  F:: */  PRIMACV(CFCOCO,  CONJ, jtvalenceerr, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE|VF2PRIM|VF2NAMELESS),
 /*  Z:  */  PRIMACV(CZCO,  VERB,  jtfoldZ1, jtfoldZ2,  RMAX,   RMAX,   RMAX   ,VFLAGNONE,VF2NONE|VF2PRIM),
 // sequences supported as atomics.  The vaid is used only for ss and to reserve a slot in ds

 /*  DUMMY*/ PRIMACV(CDUMMY,   VERB, 0, 0,  0,   0,   0   ,VFLAGNONE,VF2NONE),  // DUMMYSELF, used for flags only
 /*   */     PRIMACV(CENQUEUE,   VERB, 0,    0,  RMAX,RMAX,RMAX,VFLAGNONE,VF2NONE),  // used to create AR to send to eformat for word-formation errors

};

B jtpinit(J jt){
 AS((A)&primtab[CALP])[0]=NALP;  AK((A)&primtab[CALP])=(I)alp-(I)&primtab[CALP];
 R 1;
}
