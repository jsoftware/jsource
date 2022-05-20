/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Table of Primitive Symbols                                              */

#include "j.h"
#include "ve.h"
#include "vcomp.h"


C __attribute__((aligned(CACHELINESIZE))) ctype[256]={
CX, CX, CX, CX, CX, CX, CX, CX, CX, CS, CU, CX, CX, CX, CX, CX, /*CX       TAB      DDSEP*/
CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, CX, /* 1                  */
CS, CX, CX, CX, CX, CX, CX, CQ, CX, CX, CX, CX, CX, CX, CD, CX, /* 2  !"#$%&'()*+,-./ */
C9, C9, C9, C9, C9, C9, C9, C9, C9, C9, CC, CX, CX, CX, CX, CX, /* 3 0123456789:;<=>? */
CX, CA, CB, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CN, CA, /* 4 @ABCDEFGHIJKLMNO */
CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CX, CX, CX, CX, C9, /* 5 PQRSTUVWXYZ[\]^_ */
CX, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, /* 6 `abcdefghijklmno */
CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CDD, CX, CDDZ, CX, CX, /* 7 pqrstuvwxyz{|}~  */
// no {}  CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CA, CX, CX, CX, CX, CX, /* 7 pqrstuvwxyz{|}~  */
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

static __attribute__((aligned(CACHELINESIZE))) const C alp[256]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff};

#define PRIMNOUN(id,t,an,ar,val) [id]={{AKXR(ar),(t)&TRAVERSIBLE,0,(t),ACPERMANENT,(an),(ar)} , {.primint=val} }
#define PRIMALL(idda,idd,t,f0,f1,f,g,h,initpm1,initpm2,rm,rl,rr,vflg,vflg2,an,ar,lcc) \
 [idda]={{AKXR(ar),(t)&TRAVERSIBLE,0,(t),ACPERMANENT,(an),(ar)},{{.valencefns={f0,f1},.fgh={f,g,h},.localuse={initpm1,initpm2},.flag=(vflg),.flag2=(vflg2),.lrr=(RANK2T)((rl<<RANKTX)+rr),.mr=(RANKT)rm,.id=idd,.lc=lcc}}}
#define PRIMACV(id,t,f0,f1,rm,rl,rr,vflg,vflg2) PRIMALL(id,id,t,f0,f1,0,0,0,.lu1.uavandx=0,0,rm,rl,rr,vflg,vflg2,0,0,0)
#define PRIMACVPARM(id,t,f0,f1,rm,rl,rr,vflg,vflg2,initpm1,initpm2) PRIMALL(id,id,t,f0,f1,0,0,0,initpm1,initpm2,rm,rl,rr,vflg,vflg2,0,0,0)
#define PRIMATOMIC2(vaid,id,t,f0,f1,rm,rl,rr,vflg,vflg2) PRIMALL(vaid,id,t,f0,f1,0,0,0,.lu1.uavandx=0,VA2##vaid*sizeof(VA),rm,rl,rr,VISATOMIC2|(vflg),vflg2,0,0, \
 VA2##vaid+0x80*(vaid==CLT||id==CGT||id==CLE||id==CGE||id==CEQ||id==CNE))
#define PRIMATOMIC2FGH(vaid,id,t,f0,f1,f,g,h,rm,rl,rr,vflg,vflg2) PRIMALL(vaid,id,t,f0,f1,f,g,h,.lu1.uavandx=0,VA2##vaid*sizeof(VA),rm,rl,rr,VISATOMIC2|(vflg),vflg2,0,0, \
 VA2##vaid+0x80*(vaid==CLT||id==CGT||id==CLE||id==CGE||id==CEQ||id==CNE))
#define PRIMATOMIC2g(id,t,f0,f1,rm,rl,rr,vflg,vflg2,g) PRIMALL(id,id,t,f0,f1,0,g,0,.lu1.uavandx=0,VA2##id*sizeof(VA),rm,rl,rr,VISATOMIC2|(vflg),vflg2,0,0, \
 VA2##id+0x80*(id==CLT||id==CGT||id==CLE||id==CGE||id==CEQ||id==CNE))
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

 /*  bw */  PRIMATOMIC2(CBW0000,CBW0000, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW0001,CBW0001, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW0010,CBW0010, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW0011,CBW0011, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW0100,CBW0100, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW0101,CBW0101, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW0110,CBW0110, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW0111,CBW0111, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW1000,CBW1000, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW1001,CBW1001, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW1010,CBW1010, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW1011,CBW1011, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW1100,CBW1100, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW1101,CBW1101, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW1110,CBW1110, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  bw */  PRIMATOMIC2(CBW1111,CBW1111, VERB, jtbitwise1,  jtatomic2,   RMAX,0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  =  */  PRIMATOMIC2(CEQ,CEQ,     VERB, jtsclass,  jtatomic2,     RMAX,0,   0   ,VFUSEDOK2|VASGSAFE|VIRS2|VJTFLGOK2,VF2NONE),
 /*  =. */       /* see above */
 /*  =: */       /* see above */
 /*  <  */  PRIMATOMIC2g(CLT,    VERB, jtbox,     jtatomic2,     RMAX,0,   0   ,VASGSAFE|VFUSEDOK2|VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2,VF2BOXATOP1,ds(CRIGHT)),  // alias CLT
 /*  <. */  PRIMATOMIC12(CMIN,  VERB, jtatomic1,  jtatomic2,0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),  // alias CMIN
 /*  <: */  PRIMATOMIC2(CLE,CLE,     VERB, jtdecrem,  jtatomic2,     0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  >  */  PRIMATOMIC2(CGT,CGT,    VERB, jtope,     jtatomic2,     0,   0,   0   ,VFUSEDOK2|VASGSAFE|VIRS2|VJTFLGOK2,VF2WILLOPEN1),  // alias CGT
 /*  >. */  PRIMATOMIC12(CMAX,   VERB, jtatomic1,   jtatomic2,0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),  // alias CMAX
 /*  >: */  PRIMATOMIC2(CGE,CGE,     VERB, jtincrem,  jtatomic2,     0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  +  */  PRIMATOMIC12(CPLUS,   VERB, jtatomic1,  jtatomic2,   0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  +. */  PRIMATOMIC2(CPLUSDOT,CPLUSDOT,VERB, jtrect,    jtatomic2,    0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK2,VF2NONE),
 /*  +: */  PRIMATOMIC2(CPLUSCO,CPLUSCO, VERB, jtduble,   jtatomic2,    0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  *  */  PRIMATOMIC12(CSTAR,   VERB, jtatomic1,  jtatomic2,  0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  *. */  PRIMATOMIC2(CSTARDOT,CSTARDOT,VERB, jtpolar,   jtatomic2,    0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK2,VF2NONE),
 /*  *: */  PRIMATOMIC2(CSTARCO,CSTARCO, VERB, jtsquare,  jtatomic2,   0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  -  */  PRIMATOMIC2(CMINUS,CMINUS,  VERB, jtnegate,  jtatomic2,  0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  -. */  PRIMACV(CNOT,    VERB, jtnot,     jtless,   0,   RMAX,RMAX,VISATOMIC1|VASGSAFE|VJTFLGOK1|((7+(((ILESS-II0EPS)&0xf)<<3))&-SY_64),VF2NONE),  // native compound allowing &n - 64-bit only
 /*  -: */  PRIMACV(CHALVE,  VERB, jthalve,   jtmatch,  0,   RMAX,RMAX,VISATOMIC1|VIRS2|VASGSAFE|VJTFLGOK1,VF2NONE),  // alias CMATCH
 /*  %  */  PRIMATOMIC2(CDIV,CDIV,    VERB, jtrecip,   jtatomic2, 0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  %. */  PRIMACV(CDOMINO, VERB, jtminv,    jtmdiv,   2,   RMAX,2   ,VASGSAFE,VF2NONE),
 /*  %: */  PRIMATOMIC1(CROOT,   VERB, jtatomic1,  jtroot,   0,   0,   0   ,VISATOMIC1|VASGSAFE|VJTFLGOK1,VF2NONE),
 /*  ^  */  PRIMATOMIC12(CEXP,    VERB, jtatomic1,   jtexpn2,  0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  ^. */  PRIMATOMIC1(CLOG,    VERB, jtatomic1,  jtlogar2, 0,   0,   0   ,VISATOMIC1|VASGSAFE|VJTFLGOK1,VF2NONE),
 /*  ^: */  PRIMACV(CPOWOP,  CONJ, jtdomainerr1, jtpowop,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  $  */  PRIMACV(CDOLLAR, VERB, jtshapex,  jtreitem, RMAX,1,   RMAX,VASGSAFE|VIRS2|VJTFLGOK2,VF2NONE),
 /*  $. */  PRIMACV(CSPARSE, VERB, jtsparse1, jtsparse2,RMAX,RMAX,RMAX,VASGSAFE,VF2NONE),
 /*  $: */  PRIMACV(CSELF,   VERB, jtself1,   jtself2,  RMAX,RMAX,RMAX,VFLAGNONE,VF2NONE),
 /*  ~  */  PRIMACV(CTILDE,  ADV,  jtswap,    jtdomainerr2,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  ~. */  PRIMACV(CNUB,    VERB, jtnub,     jtdomainerr2,  RMAX,RMAX,RMAX,VASGSAFE|VJTFLGOK1,VF2NONE),
 /*  ~: */  PRIMATOMIC2(CNE,CNE,     VERB, jtnubsieve,jtatomic2,     RMAX,0,   0   ,VFUSEDOK2|VASGSAFE|VIRS1|VIRS2|VJTFLGOK2,VF2WILLOPEN1),
 /*  |  */  PRIMATOMIC12(CSTILE,  VERB, jtatomic1,     jtresidue,0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  |. */  PRIMACV(CREV,    VERB, jtreverse, jtrotate, RMAX,1,   RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2,VF2WILLOPEN1PROP),   // alias CROT
 /*  |: */  PRIMACV(CCANT,   VERB, jtcant1,   jtcant2,  RMAX,1,   RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2,VF2WILLOPEN1PROP|VF2WILLOPEN2A),
 /*  .  */  PRIMACV(CDOT,    CONJ, jtdomainerr1, jtdot,    0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  :  */  PRIMACV(CCOLON,  CONJ, jtdomainerr1, jtcolon,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  :. */  PRIMACV(COBVERSE,CONJ, jtdomainerr1, jtobverse,0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  :: */  PRIMACV(CADVERSE,CONJ, jtdomainerr1, jtadverse,0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  ,  */  PRIMACV(CCOMMA,  VERB, jtravel,   jtapip,   RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK1|VJTFLGOK2,VF2WILLOPEN1PROP),  // could VF2WILLOPEN2?PROP but sparse code may EPILOG
 /*  ,. */  PRIMACV(CCOMDOT, VERB, jttable,   jtstitch, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  ,: */  PRIMACV(CLAMIN,  VERB, jtlamin1,  jtlamin2, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VJTFLGOK1,VF2NONE),
 /*  ;  */  PRIMACV(CSEMICO, VERB, jtraze,    jtjlink,  RMAX,RMAX,RMAX,VASGSAFE|VJTFLGOK2,VF2USESITEMCOUNT1|VF2WILLOPEN1|VF2WILLOPEN2WPROP),
 /*  ;. */  PRIMACV(CCUT,    CONJ, jtdomainerr1,        jtcut,    0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  ;: */  PRIMACVPARM(CWORDS,  VERB, jtwords,   jtfsm,    1,   RMAX,RMAX,VASGSAFE,VF2NONE,.boxcut0=0,0),  // parms are passed through to jtboxcut0
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
 /*  [  */  PRIMACV(CLEFT,   VERB, jtright1,  jtleft2,  RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VISATOMIC1|VJTFLGOK1|VJTFLGOK2,VF2WILLOPEN1PROP|VF2WILLOPEN2APROP),
 /*  [. */  PRIMACV(CLEV,    CONJ,  0,   jtlev, 0,   0,   0   ,VASGSAFE|VJTFLGOK2,VF2NONE),
 /*  [: */  PRIMACV(CCAP,    VERB, jtdomainerr1,jtdomainerr2, RMAX,RMAX,RMAX,VFLAGNONE,VF2NONE),   // not ASGSAFE since used for not-yet-defined names
 /*  ]  */  PRIMACV(CRIGHT,  VERB, jtright1,  jtright2, RMAX,RMAX,RMAX,VASGSAFE|VIRS1|VIRS2|VISATOMIC1|VJTFLGOK1|VJTFLGOK2,VF2WILLOPEN1PROP|VF2WILLOPEN2WPROP),
 /*  ]. */  PRIMACV(CDEX,    CONJ,  0,   jtdex, 0,   0,   0   ,VASGSAFE|VJTFLGOK2,VF2NONE),
 /*  ]. */  PRIMACV(CIDENT,  ADV,   jtlev,   0, 0,   0,   0   ,VASGSAFE|VJTFLGOK2,VF2NONE),
 /*  {  */  PRIMACV(CLBRACE, VERB, jtcatalog, jtfrom,   1,   0,   RMAX,VASGSAFE|VIRS2|VJTFLGOK2,VF2WILLOPEN2A),
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
 /*  e. */  PRIMACV(CEPS,    VERB, jtrazein,  jteps,    RMAX,RMAX,RMAX,VASGSAFE|VIRS2|(7+(((IEPS-II0EPS)&0xf)<<3)),VF2WILLOPEN2A|VF2WILLOPEN2W),
 /*  E. */  PRIMACV(CEBAR,   VERB, jtdomainerr1, jtebar,   0,   RMAX,RMAX,VASGSAFE,VF2WILLOPEN2A|VF2WILLOPEN2W),
 /*  f. */  PRIMACV(CFIX,    ADV,  jtfix,     jtdomainerr2, 0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  H. */  PRIMACV(CHGEOM,  CONJ, jtdomainerr1, jthgeom,  0,   0,   0   ,VISATOMIC1|VASGSAFE,VF2NONE),
 /*  i. */  PRIMACV(CIOTA,   VERB, jtiota,    jtindexof,1,   RMAX,RMAX,VASGSAFE|VIRS2,VF2WILLOPEN2A|VF2WILLOPEN2W),
 /*  i: */  PRIMACV(CICO,    VERB, jtjico1,   jtjico2,  0,   RMAX,RMAX,VASGSAFE|VIRS2,VF2WILLOPEN2A|VF2WILLOPEN2W),
 /*  I. */  PRIMACV(CICAP,   VERB, jticap,    jticap2,  1,   RMAX,RMAX,VASGSAFE,VF2NONE),
 /*  j. */  PRIMACV(CJDOT,   VERB, jtjdot1,   jtjdot2,  0,   0,   0   ,VISATOMIC1|VASGSAFE,VF2NONE),
 /*  L. */  PRIMACV(CLDOT,   VERB, jtlevel1,  jtdomainerr2, RMAX,RMAX,RMAX,VASGSAFE,VF2WILLOPEN1),
 /*  L: */  PRIMACV(CLCAPCO, CONJ, jtdomainerr1, jtlcapco, 0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  M. */  PRIMACV(CMCAP,   ADV,  jtmemo,    jtdomainerr2, 0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  o. */  PRIMATOMIC12(CCIRCLE, VERB, jtpix, jtatomic2, 0,   0,   0   ,VISATOMIC1|VFUSEDOK2|VIRS2|VASGSAFE|VJTFLGOK1|VJTFLGOK2,VF2NONE),
 /*  p. */  PRIMACV(CPOLY,   VERB, jtpoly1,   jtpoly2,  1,   1,   0   ,VASGSAFE|VIRS2|VJTFLGOK2,VF2NONE),
 /*  p..*/  PRIMACV(CPDERIV, VERB, jtpderiv1, jtpderiv2,1,   0,   1   ,VASGSAFE,VF2NONE),
 /*  p: */  PRIMACV(CPCO,    VERB, jtprime,   jtpco2,   0,   RMAX,RMAX,VISATOMIC1|VIRS1|VASGSAFE,VF2NONE),
 /*  q: */  PRIMACV(CQCO,    VERB, jtfactor,  jtqco2,   0,   0,   0   ,VISATOMIC1|VASGSAFE,VF2NONE),
 /*  r. */  PRIMACV(CRDOT,   VERB, jtrdot1,   jtrdot2,  0,   0,   0   ,VISATOMIC1|VASGSAFE,VF2NONE),
 /*  s: */  PRIMACV(CSCO,    VERB, jtsb1,     jtsb2,    RMAX,RMAX,RMAX,VASGSAFE,VF2NONE),
 /*  S: */  PRIMACV(CSCAPCO, CONJ, jtdomainerr1, jtscapco, 0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  t. */  PRIMACV(CTDOT,   CONJ, jtdomainerr1, jttdot, 0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  T. */  PRIMACV(CTCAPDOT, VERB, jttcapdot1, jttcapdot2, 0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  u. */  PRIMACV(CUDOT,   VERB, jtimplocref,jtimplocref,   RMAX,RMAX,RMAX,VFLAGNONE,VF2IMPLOC),
 /*  u: */  PRIMACV(CUCO,    VERB, jtuco1,    jtuco2,   RMAX,RMAX,RMAX,VASGSAFE,VF2NONE),
 /*  v. */  PRIMACV(CVDOT,   VERB, jtimplocref, jtimplocref,   RMAX,RMAX,RMAX,VFLAGNONE,VF2IMPLOC),
 /*  x: */  PRIMACV(CXCO,    VERB, jtxco1,    jtxco2,   RMAX,RMAX,RMAX,VASGSAFE|VISATOMIC1,VF2NONE),
 /*  F.  */  PRIMACV(CFDOT,  CONJ, jtdomainerr1, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  F.. */  PRIMACV(CFDOTDOT,  CONJ, jtdomainerr1, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  F.: */  PRIMACV(CFDOTCO,  CONJ, jtdomainerr1, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  F:  */  PRIMACV(CFCO,  CONJ, jtdomainerr1, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  F:. */  PRIMACV(CFCODOT,  CONJ, jtdomainerr1, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  F:: */  PRIMACV(CFCOCO,  CONJ, jtdomainerr1, jtfold,  0,   0,   0   ,VFLAGNONE,VF2NONE),
 /*  Z:  */  PRIMACV(CZCO,  VERB,  jtdomainerr1, jtfoldZ,  RMAX,   RMAX,   RMAX   ,VFLAGNONE,VF2NONE),
 // sequences supported as atomics.  The vaid is used only for ss and to reserve a slot in ds

};

B jtpinit(J jt){
 AS((A)&primtab[CALP])[0]=NALP;  AK((A)&primtab[CALP])=(I)alp-(I)&primtab[CALP];
 R 1;
}
