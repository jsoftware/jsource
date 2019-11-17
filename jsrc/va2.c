/* Copyright 1990-2011, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Atomic (Scalar) Dyadic                                           */

#include "j.h"
#include "ve.h"
#include "vcomp.h"

// shorthands for inplacing
#define VIP (VIPOKA+VIPOKW)   // inplace everything
#define VIPI0 ((VIPOKA*(sizeof(I)==sizeof(D))))  // inplace left arg if D is same length as I
#define VIP0I ((VIPOKW*(sizeof(I)==sizeof(D))))  // inplace right arg if D is same length as I
#define VIPID (VIPI0+VIPOKW)  // inplace D, and I if I is same length as D: I is left arg
#define VIPDI (VIP0I+VIPOKA)  // inplace D, and I if I is same length as D: I is right arg

static VA va[]={
/* non-atomic functions      */ {
 {{0,0}, {0,0}, {0,0},                                /* BB BI BD              */
  {0,0}, {0,0}, {0,0},                                /* IB II ID              */
  {0,0}, {0,0}, {0,0},                                /* DB DI DD              */
  {0,0}, {0,0}, {0,0}, {0,0}},                        /* ZZ XX QQ Symb         */
 {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},   /* ins: B I D Z X Q Symb */
 {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},   /* pfx: B I D Z X Q Symb */
 {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}} }, /* sfx: B I D Z X Q Symb */

/* 10    */ {
 {{(VF)bw0000II,  VI+VII+VIP}, {(VF)bw0000II,  VI+VII+VIP}, {(VF)bw0000II,  VI+VII+VIP}, 
  {(VF)bw0000II,  VI+VII+VIP}, {(VF)bw0000II,  VI+VIP},     {(VF)bw0000II,  VI+VII+VIP},
  {(VF)bw0000II,  VI+VII+VIP}, {(VF)bw0000II,  VI+VII+VIP}, {(VF)bw0000II,  VI+VII+VIP},
  {(VF)bw0000II,  VI+VII+VIP}, {(VF)bw0000II,  VI+VII+VIP}, {(VF)bw0000II,  VI+VII+VIP}, {0,0}}, 
 {{(VF)bw0000insI,VI+VII}, {(VF)bw0000insI,VI}},
 {{(VF)bw0000pfxI,VI+VII}, {(VF)bw0000pfxI,VI}},
 {{(VF)bw0000sfxI,VI+VII}, {(VF)bw0000sfxI,VI}} },

/* 11    */ {
 {{(VF)bw0001II,  VI+VII+VIP}, {(VF)bw0001II,  VI+VII+VIP}, {(VF)bw0001II,  VI+VII+VIP}, 
  {(VF)bw0001II,  VI+VII+VIP}, {(VF)bw0001II,  VI+VIP},     {(VF)bw0001II,  VI+VII+VIP},
  {(VF)bw0001II,  VI+VII+VIP}, {(VF)bw0001II,  VI+VII+VIP}, {(VF)bw0001II,  VI+VII+VIP},
  {(VF)bw0001II,  VI+VII+VIP}, {(VF)bw0001II,  VI+VII+VIP}, {(VF)bw0001II,  VI+VII+VIP}, {0,0}}, 
 {{(VF)bw0001insI,VI+VII}, {(VF)bw0001insI,VI}},
 {{(VF)bw0001pfxI,VI+VII}, {(VF)bw0001pfxI,VI}},
 {{(VF)bw0001sfxI,VI+VII}, {(VF)bw0001sfxI,VI}} },

/* 12    */ {
 {{(VF)bw0010II,  VI+VII+VIP}, {(VF)bw0010II,  VI+VII+VIP}, {(VF)bw0010II,  VI+VII+VIP}, 
  {(VF)bw0010II,  VI+VII+VIP}, {(VF)bw0010II,  VI+VIP},     {(VF)bw0010II,  VI+VII+VIP},
  {(VF)bw0010II,  VI+VII+VIP}, {(VF)bw0010II,  VI+VII+VIP}, {(VF)bw0010II,  VI+VII+VIP},
  {(VF)bw0010II,  VI+VII+VIP}, {(VF)bw0010II,  VI+VII+VIP}, {(VF)bw0010II,  VI+VII+VIP}, {0,0}}, 
 {{(VF)bw0010insI,VI+VII}, {(VF)bw0010insI,VI}},
 {{0,0}},
 {{(VF)bw0010sfxI,VI+VII}, {(VF)bw0010sfxI,VI}} },

/* 13    */ {
 {{(VF)bw0011II,  VI+VII+VIP}, {(VF)bw0011II,  VI+VII+VIP}, {(VF)bw0011II,  VI+VII+VIP}, 
  {(VF)bw0011II,  VI+VII+VIP}, {(VF)bw0011II,  VI+VIP},     {(VF)bw0011II,  VI+VII+VIP},
  {(VF)bw0011II,  VI+VII+VIP}, {(VF)bw0011II,  VI+VII+VIP}, {(VF)bw0011II,  VI+VII+VIP},
  {(VF)bw0011II,  VI+VII+VIP}, {(VF)bw0011II,  VI+VII+VIP}, {(VF)bw0011II,  VI+VII+VIP}, {0,0}}, 
 {{(VF)bw0011insI,VI+VII}, {(VF)bw0011insI,VI}},
 {{(VF)bw0011pfxI,VI+VII}, {(VF)bw0011pfxI,VI}},
 {{(VF)bw0011sfxI,VI+VII}, {(VF)bw0011sfxI,VI}} },

/* 14    */ {
 {{(VF)bw0100II,  VI+VII+VIP}, {(VF)bw0100II,  VI+VII+VIP}, {(VF)bw0100II,  VI+VII+VIP}, 
  {(VF)bw0100II,  VI+VII+VIP}, {(VF)bw0100II,  VI+VIP},     {(VF)bw0100II,  VI+VII+VIP},
  {(VF)bw0100II,  VI+VII+VIP}, {(VF)bw0100II,  VI+VII+VIP}, {(VF)bw0100II,  VI+VII+VIP},
  {(VF)bw0100II,  VI+VII+VIP}, {(VF)bw0100II,  VI+VII+VIP}, {(VF)bw0100II,  VI+VII+VIP}, {0,0}}, 
 {{(VF)bw0100insI,VI+VII}, {(VF)bw0100insI,VI}},
 {{0,0}},
 {{(VF)bw0100sfxI,VI+VII}, {(VF)bw0100sfxI,VI}} },

/* 15    */ {
 {{(VF)bw0101II,  VI+VII+VIP}, {(VF)bw0101II,  VI+VII+VIP}, {(VF)bw0101II,  VI+VII+VIP}, 
  {(VF)bw0101II,  VI+VII+VIP}, {(VF)bw0101II,  VI+VIP},     {(VF)bw0101II,  VI+VII+VIP},
  {(VF)bw0101II,  VI+VII+VIP}, {(VF)bw0101II,  VI+VII+VIP}, {(VF)bw0101II,  VI+VII+VIP},
  {(VF)bw0101II,  VI+VII+VIP}, {(VF)bw0101II,  VI+VII+VIP}, {(VF)bw0101II,  VI+VII+VIP}, {0,0}}, 
 {{(VF)bw0101insI,VI+VII}, {(VF)bw0101insI,VI}},
 {{(VF)bw0101pfxI,VI+VII}, {(VF)bw0101pfxI,VI}},
 {{(VF)bw0101sfxI,VI+VII}, {(VF)bw0101sfxI,VI}} },

/* 16    */ {
 {{(VF)bw0110II,  VI+VII+VIP}, {(VF)bw0110II,  VI+VII+VIP}, {(VF)bw0110II,  VI+VII+VIP}, 
  {(VF)bw0110II,  VI+VII+VIP}, {(VF)bw0110II,  VI+VIP},     {(VF)bw0110II,  VI+VII+VIP},
  {(VF)bw0110II,  VI+VII+VIP}, {(VF)bw0110II,  VI+VII+VIP}, {(VF)bw0110II,  VI+VII+VIP},
  {(VF)bw0110II,  VI+VII+VIP}, {(VF)bw0110II,  VI+VII+VIP}, {(VF)bw0110II,  VI+VII+VIP}, {0,0}}, 
 {{(VF)bw0110insI,VI+VII}, {(VF)bw0110insI,VI}},
 {{(VF)bw0110pfxI,VI+VII}, {(VF)bw0110pfxI,VI}},
 {{(VF)bw0110sfxI,VI+VII}, {(VF)bw0110sfxI,VI}} },

/* 17    */ {
 {{(VF)bw0111II,  VI+VII+VIP}, {(VF)bw0111II,  VI+VII+VIP}, {(VF)bw0111II,  VI+VII+VIP}, 
  {(VF)bw0111II,  VI+VII+VIP}, {(VF)bw0111II,  VI+VIP},     {(VF)bw0111II,  VI+VII+VIP},
  {(VF)bw0111II,  VI+VII+VIP}, {(VF)bw0111II,  VI+VII+VIP}, {(VF)bw0111II,  VI+VII+VIP},
  {(VF)bw0111II,  VI+VII+VIP}, {(VF)bw0111II,  VI+VII+VIP}, {(VF)bw0111II,  VI+VII+VIP}, {0,0}}, 
 {{(VF)bw0111insI,VI+VII}, {(VF)bw0111insI,VI}},
 {{(VF)bw0111pfxI,VI+VII}, {(VF)bw0111pfxI,VI}},
 {{(VF)bw0111sfxI,VI+VII}, {(VF)bw0111sfxI,VI}} },

/* 18    */ {
 {{(VF)bw1000II,  VI+VII+VIP}, {(VF)bw1000II,  VI+VII+VIP}, {(VF)bw1000II,  VI+VII+VIP}, 
  {(VF)bw1000II,  VI+VII+VIP}, {(VF)bw1000II,  VI+VIP},     {(VF)bw1000II,  VI+VII+VIP},
  {(VF)bw1000II,  VI+VII+VIP}, {(VF)bw1000II,  VI+VII+VIP}, {(VF)bw1000II,  VI+VII+VIP},
  {(VF)bw1000II,  VI+VII+VIP}, {(VF)bw1000II,  VI+VII+VIP}, {(VF)bw1000II,  VI+VII+VIP}, {0,0}}, 
 {{(VF)bw1000insI,VI+VII}, {(VF)bw1000insI,VI}},
 {{0,0}},
 {{(VF)bw1000sfxI,VI+VII}, {(VF)bw1000sfxI,VI}} },

/* 19    */ {
 {{(VF)bw1001II,  VI+VII+VIP}, {(VF)bw1001II,  VI+VII+VIP}, {(VF)bw1001II,  VI+VII+VIP}, 
  {(VF)bw1001II,  VI+VII+VIP}, {(VF)bw1001II,  VI+VIP},     {(VF)bw1001II,  VI+VII+VIP},
  {(VF)bw1001II,  VI+VII+VIP}, {(VF)bw1001II,  VI+VII+VIP}, {(VF)bw1001II,  VI+VII+VIP},
  {(VF)bw1001II,  VI+VII+VIP}, {(VF)bw1001II,  VI+VII+VIP}, {(VF)bw1001II,  VI+VII+VIP}, {0,0}}, 
 {{(VF)bw1001insI,VI+VII}, {(VF)bw1001insI,VI}},
 {{(VF)bw1001pfxI,VI+VII}, {(VF)bw1001pfxI,VI}},
 {{(VF)bw1001sfxI,VI+VII}, {(VF)bw1001sfxI,VI}} },

/* 1a    */ {
 {{(VF)bw1010II,  VI+VII+VIP}, {(VF)bw1010II,  VI+VII+VIP}, {(VF)bw1010II,  VI+VII+VIP}, 
  {(VF)bw1010II,  VI+VII+VIP}, {(VF)bw1010II,  VI+VIP},     {(VF)bw1010II,  VI+VII+VIP},
  {(VF)bw1010II,  VI+VII+VIP}, {(VF)bw1010II,  VI+VII+VIP}, {(VF)bw1010II,  VI+VII+VIP},
  {(VF)bw1010II,  VI+VII+VIP}, {(VF)bw1010II,  VI+VII+VIP}, {(VF)bw1010II,  VI+VII+VIP}, {0,0}}, 
 {{(VF)bw1010insI,VI+VII}, {(VF)bw1010insI,VI}},
 {{0,0}},
 {{(VF)bw1010sfxI,VI+VII}, {(VF)bw1010sfxI,VI}} },

/* 1b    */ {
 {{(VF)bw1011II,  VI+VII+VIP}, {(VF)bw1011II,  VI+VII+VIP}, {(VF)bw1011II,  VI+VII+VIP}, 
  {(VF)bw1011II,  VI+VII+VIP}, {(VF)bw1011II,  VI+VIP},     {(VF)bw1011II,  VI+VII+VIP},
  {(VF)bw1011II,  VI+VII+VIP}, {(VF)bw1011II,  VI+VII+VIP}, {(VF)bw1011II,  VI+VII+VIP},
  {(VF)bw1011II,  VI+VII+VIP}, {(VF)bw1011II,  VI+VII+VIP}, {(VF)bw1011II,  VI+VII+VIP}, {0,0}}, 
 {{(VF)bw1011insI,VI+VII}, {(VF)bw1011insI,VI}},
 {{0,0}},
 {{(VF)bw1011sfxI,VI+VII}, {(VF)bw1011sfxI,VI}} },

/* 1c    */ {
 {{(VF)bw1100II,  VI+VII+VIP}, {(VF)bw1100II,  VI+VII+VIP}, {(VF)bw1100II,  VI+VII+VIP}, 
  {(VF)bw1100II,  VI+VII+VIP}, {(VF)bw1100II,  VI+VIP},     {(VF)bw1100II,  VI+VII+VIP},
  {(VF)bw1100II,  VI+VII+VIP}, {(VF)bw1100II,  VI+VII+VIP}, {(VF)bw1100II,  VI+VII+VIP},
  {(VF)bw1100II,  VI+VII+VIP}, {(VF)bw1100II,  VI+VII+VIP}, {(VF)bw1100II,  VI+VII+VIP}, {0,0}}, 
 {{(VF)bw1100insI,VI+VII}, {(VF)bw1100insI,VI}},
 {{0,0}},
 {{(VF)bw1100sfxI,VI+VII}, {(VF)bw1100sfxI,VI}} },

/* 1d    */ {
 {{(VF)bw1101II,  VI+VII+VIP}, {(VF)bw1101II,  VI+VII+VIP}, {(VF)bw1101II,  VI+VII+VIP}, 
  {(VF)bw1101II,  VI+VII+VIP}, {(VF)bw1101II,  VI+VIP},     {(VF)bw1101II,  VI+VII+VIP},
  {(VF)bw1101II,  VI+VII+VIP}, {(VF)bw1101II,  VI+VII+VIP}, {(VF)bw1101II,  VI+VII+VIP},
  {(VF)bw1101II,  VI+VII+VIP}, {(VF)bw1101II,  VI+VII+VIP}, {(VF)bw1101II,  VI+VII+VIP}, {0,0}}, 
 {{(VF)bw1101insI,VI+VII}, {(VF)bw1101insI,VI}},
 {{0,0}},
 {{(VF)bw1101sfxI,VI+VII}, {(VF)bw1101sfxI,VI}} },

/* 1e    */ {
 {{(VF)bw1110II,  VI+VII+VIP}, {(VF)bw1110II,  VI+VII+VIP}, {(VF)bw1110II,  VI+VII+VIP}, 
  {(VF)bw1110II,  VI+VII+VIP}, {(VF)bw1110II,  VI+VIP},     {(VF)bw1110II,  VI+VII+VIP},
  {(VF)bw1110II,  VI+VII+VIP}, {(VF)bw1110II,  VI+VII+VIP}, {(VF)bw1110II,  VI+VII+VIP},
  {(VF)bw1110II,  VI+VII+VIP}, {(VF)bw1110II,  VI+VII+VIP}, {(VF)bw1110II,  VI+VII+VIP}, {0,0}}, 
 {{(VF)bw1110insI,VI+VII}, {(VF)bw1110insI,VI}},
 {{0,0}},
 {{(VF)bw1110sfxI,VI+VII}, {(VF)bw1110sfxI,VI}} },

/* 1f    */ {
 {{(VF)bw1111II,  VI+VII+VIP}, {(VF)bw1111II,  VI+VII+VIP}, {(VF)bw1111II,  VI+VII+VIP}, 
  {(VF)bw1111II,  VI+VII+VIP}, {(VF)bw1111II,  VI+VIP},     {(VF)bw1111II,  VI+VII+VIP},
  {(VF)bw1111II,  VI+VII+VIP}, {(VF)bw1111II,  VI+VII+VIP}, {(VF)bw1111II,  VI+VII+VIP},
  {(VF)bw1111II,  VI+VII+VIP}, {(VF)bw1111II,  VI+VII+VIP}, {(VF)bw1111II,  VI+VII+VIP}, {0,0}}, 
 {{(VF)bw1111insI,VI+VII}, {(VF)bw1111insI,VI}},
 {{(VF)bw1111pfxI,VI+VII}, {(VF)bw1111pfxI,VI}},
 {{(VF)bw1111sfxI,VI+VII}, {(VF)bw1111sfxI,VI}} },

/* 95 ~: */ {
 {{(VF)neBB,VB+VIP}, {(VF)neBI,VB+VIPOKA}, {(VF)neBD,VB+VIPOKA},
  {(VF)neIB,VB+VIPOKW}, {(VF)neII,VB}, {(VF)neID,VB},
  {(VF)neDB,VB+VIPOKW}, {(VF)neDI,VB}, {(VF)neDD,VB}, 
  {(VF)neZZ,VB+VZZ}, {(VF)neXX,VB+VXEQ}, {(VF)neQQ,VB+VQQ}, {0,0}},
 {{(VF)neinsB,VB}},
 {{(VF)nepfxB,VB}},
 {{(VF)nesfxB,VB}} },

/* 25 %  */ {
 {{(VF)divBB,VD}, {(VF)divBI,VD+VIP0I}, {(VF)divBD,VD+VIPOKW},
  {(VF)divIB,VD+VIPI0}, {(VF)divII,VD+VIPI0+VIP0I}, {(VF)divID,VD+VIPID},
  {(VF)divDB,VD+VIPOKA}, {(VF)divDI,VD+VIPDI}, {(VF)divDD,VD+VIP+VCANHALT}, 
  {(VF)divZZ,VZ+VZZ+VIP}, {(VF)divXX,VX+VXX}, {(VF)divQQ,VQ+VQQ}, {0,0}},
 {{(VF)divinsD,VD+VDD}, {(VF)divinsD,VD+VDD}, {(VF)divinsD,VD}, {(VF)divinsZ,VZ}},
 {{(VF)divpfxD,VD+VDD}, {(VF)divpfxD,VD+VDD}, {(VF)divpfxD,VD}, {(VF)divpfxZ,VZ}},
 {{(VF)divsfxD,VD+VDD}, {(VF)divsfxD,VD+VDD}, {(VF)divsfxD,VD}, {(VF)divsfxZ,VZ}} },

/* 89 +: */ {
 {{(VF)norBB,VB+VIP    }, {(VF)norBB,VB+VBB+VIP}, {(VF)norBB,VB+VBB+VIP},
  {(VF)norBB,VB+VBB+VIP}, {(VF)norBB,VB+VBB+VIP}, {(VF)norBB,VB+VBB+VIP},
  {(VF)norBB,VB+VBB+VIP}, {(VF)norBB,VB+VBB+VIP}, {(VF)norBB,VB+VBB+VIP}, 
  {(VF)norBB,VB+VBB+VIP}, {(VF)norBB,VB+VBB+VIP}, {(VF)norBB,VB+VBB+VIP}, {0,0}},
 {{(VF)norinsB,VB}},
 {{(VF)norpfxB,VB}},
 {{(VF)norsfxB,VB}} },

/* 88 +. */ {
 {{(VF)orBB,VB+VIP     }, {(VF)gcdII,VI+VII}, {(VF)gcdDD,VD+VDD+VIP},
  {(VF)gcdII,VI+VII}, {(VF)gcdII,VI    }, {(VF)gcdDD,VD+VDD+VIP},
  {(VF)gcdDD,VD+VDD+VIP}, {(VF)gcdDD,VD+VDD+VIP}, {(VF)gcdDD,VD+VIP+VCANHALT}, 
  {(VF)gcdZZ,VZ+VZZ}, {(VF)gcdXX,VX+VXX}, {(VF)gcdQQ,VQ+VQQ}, {0,0}},
 {{(VF)orinsB,VB}},
 {{(VF)orpfxB,VB}},
 {{(VF)orsfxB,VB}} },

/* 2d -  */ {
 {{(VF)minusBB,VI    }, {(VF)minusBI,VI+VIPOKW}, {(VF)minusBD,VD+VIPOKW}, 
  {(VF)minusIB,VI+VIPOKA}, {(VF)minusII,VI+VIP}, {(VF)minusID,VD+VIPID},
  {(VF)minusDB,VD+VIPOKA    }, {(VF)minusDI,VD+VIPDI    }, {(VF)minusDD,VD+VIP+VCANHALT}, 
  {(VF)minusZZ,VZ+VZZ+VIP}, {(VF)minusXX,VX+VXX}, {(VF)minusQQ,VQ+VQQ}, {0,0}},
 {{(VF)minusinsB,VI}, {(VF)minusinsI,VI}, {(VF)minusinsD,VD}, {(VF)minusinsZ,VZ}, {0,0},          {0,0},          {0,0}},
 {{(VF)minuspfxB,VI}, {(VF)minuspfxI,VI}, {(VF)minuspfxD,VD+VIPOKW}, {(VF)minuspfxZ,VZ}, {(VF)minuspfxX,VX}, {(VF)minuspfxQ,VQ}, {0,0}},
 {{(VF)minussfxB,VI}, {(VF)minussfxI,VI}, {(VF)minussfxD,VD+VIPOKW}, {(VF)minussfxZ,VZ}, {0,0},          {0,0},          {0,0}} },

   // For Booleans, VIP means 'inplace if rank not specified and there is no frame'
/* 3c <  */ {
 {{(VF)ltBB,VB+VIP}, {(VF)ltBI,VB+VIPOKA}, {(VF)ltBD,VB+VIPOKA},
  {(VF)ltIB,VB+VIPOKW}, {(VF)ltII,VB}, {(VF)ltID,VB},
  {(VF)ltDB,VB+VIPOKW}, {(VF)ltDI,VB}, {(VF)ltDD,VB}, 
  {(VF)ltDD,VB+VDD+VIP}, {(VF)ltXX,VB+VXFC}, {(VF)ltQQ,VB+VQQ}, {(VF)ltSS,VB}},
 {{(VF)ltinsB,VB}},
 {{(VF)ltpfxB,VB}},
 {{(VF)ltsfxB,VB}} },

/* 3d =  */ {
 {{(VF)eqBB,VB+VIP}, {(VF)eqBI,VB+VIPOKA}, {(VF)eqBD,VB+VIPOKA},
  {(VF)eqIB,VB+VIPOKW}, {(VF)eqII,VB}, {(VF)eqID,VB},
  {(VF)eqDB,VB+VIPOKW}, {(VF)eqDI,VB}, {(VF)eqDD,VB}, 
  {(VF)eqZZ,VB+VZZ}, {(VF)eqXX,VB+VXEQ}, {(VF)eqQQ,VB+VQQ}, {(VF)eqII,VB}},
 {{(VF)eqinsB,VB}},
 {{(VF)eqpfxB,VB}},
 {{(VF)eqsfxB,VB}} },

/* 3e >  */ {
 {{(VF)gtBB,VB+VIP}, {(VF)gtBI,VB+VIPOKA}, {(VF)gtBD,VB+VIPOKA},
  {(VF)gtIB,VB+VIPOKW}, {(VF)gtII,VB}, {(VF)gtID,VB},
  {(VF)gtDB,VB+VIPOKW}, {(VF)gtDI,VB}, {(VF)gtDD,VB}, 
  {(VF)gtDD,VB+VDD+VIP}, {(VF)gtXX,VB+VXCF}, {(VF)gtQQ,VB+VQQ}, {(VF)gtSS,VB}},
 {{(VF)gtinsB,VB}},
 {{(VF)gtpfxB,VB}},
 {{(VF)gtsfxB,VB}} },

/* 8a *. */ {
 {{(VF)andBB,VB+VIP    }, {(VF)lcmII,VI+VII}, {(VF)lcmDD,VD+VDD+VIP},
  {(VF)lcmII,VI+VII}, {(VF)lcmII,VI    }, {(VF)lcmDD,VD+VDD+VIP},
  {(VF)lcmDD,VD+VDD+VIP}, {(VF)lcmDD,VD+VDD+VIP}, {(VF)lcmDD,VD+VIP+VCANHALT}, 
  {(VF)lcmZZ,VZ+VZZ}, {(VF)lcmXX,VX+VXX}, {(VF)lcmQQ,VQ+VQQ}, {0,0}},
 {{(VF)andinsB,VB}},
 {{(VF)andpfxB,VB}},
 {{(VF)andsfxB,VB}} },

/* 8b *: */ {
 {{(VF)nandBB,VB+VIP},     {(VF)nandBB,VB+VBB+VIP}, {(VF)nandBB,VB+VBB+VIP},
  {(VF)nandBB,VB+VBB+VIP}, {(VF)nandBB,VB+VBB+VIP}, {(VF)nandBB,VB+VBB+VIP},
  {(VF)nandBB,VB+VBB+VIP}, {(VF)nandBB,VB+VBB+VIP}, {(VF)nandBB,VB+VBB+VIP}, 
  {(VF)nandBB,VB+VBB+VIP}, {(VF)nandBB,VB+VBB+VIP}, {(VF)nandBB,VB+VBB+VIP}, {0,0}},
 {{(VF)nandinsB,VB}},
 {{(VF)nandpfxB,VB}},
 {{(VF)nandsfxB,VB}} },

/* 85 >: */ {
 {{(VF)geBB,VB+VIP}, {(VF)geBI,VB+VIPOKA}, {(VF)geBD,VB+VIPOKA},
  {(VF)geIB,VB+VIPOKW}, {(VF)geII,VB}, {(VF)geID,VB},
  {(VF)geDB,VB+VIPOKW}, {(VF)geDI,VB}, {(VF)geDD,VB}, 
  {(VF)geDD,VB+VDD+VIP}, {(VF)geXX,VB+VXFC}, {(VF)geQQ,VB+VQQ}, {(VF)geSS,VB}},
 {{(VF)geinsB,VB}},
 {{(VF)gepfxB,VB}},
 {{(VF)gesfxB,VB}} },

/* 83 <: */ {
 {{(VF)leBB,VB+VIP}, {(VF)leBI,VB+VIPOKA}, {(VF)leBD,VB+VIPOKA},
  {(VF)leIB,VB+VIPOKW}, {(VF)leII,VB}, {(VF)leID,VB},
  {(VF)leDB,VB+VIPOKW}, {(VF)leDI,VB}, {(VF)leDD,VB}, 
  {(VF)leDD,VB+VDD+VIP}, {(VF)leXX,VB+VXCF}, {(VF)leQQ,VB+VQQ}, {(VF)leSS,VB}},
 {{(VF)leinsB,VB}},
 {{(VF)lepfxB,VB}},
 {{(VF)lesfxB,VB}} },

/* 82 <. */ {
 {{(VF)andBB,VB+VIP}, {(VF)minBI,VI+VIPOKW}, {(VF)minBD,VD+VIPOKW},
  {(VF)minIB,VI+VIPOKA}, {(VF)minII,VI+VIP}, {(VF)minID,VD+VIPID},
  {(VF)minDB,VD+VIPOKA}, {(VF)minDI,VD+VIPDI}, {(VF)minDD,VD+VIP}, 
  {(VF)minDD,VD+VDD+VIP}, {(VF)minXX,VX+VXX}, {(VF)minQQ,VQ+VQQ}, {(VF)minSS,VSB}},  // always VIP a forced conversion
 {{(VF)andinsB,VB}, {(VF)mininsI,VI}, {(VF)mininsD,VD}, {(VF)mininsD,VD+VDD}, {(VF)mininsX,VX}, {0,0},        {(VF)mininsS,VSB}},
 {{(VF)andpfxB,VB}, {(VF)minpfxI,VI+VIPOKW}, {(VF)minpfxD,VD+VIPOKW}, {(VF)minpfxD,VD+VDD}, {(VF)minpfxX,VX}, {(VF)minpfxQ,VQ}, {(VF)minpfxS,VSB}},
 {{(VF)andsfxB,VB}, {(VF)minsfxI,VI+VIPOKW}, {(VF)minsfxD,VD+VIPOKW}, {(VF)minsfxD,VD+VDD}, {(VF)minsfxX,VX}, {(VF)minsfxQ,VQ}, {(VF)minsfxS,VSB}}},

/* 84 >. */ {
 {{(VF)orBB,VB+VIP}, {(VF)maxBI,VI+VIPOKW}, {(VF)maxBD,VD+VIPOKW},
  {(VF)maxIB,VI+VIPOKA}, {(VF)maxII,VI+VIP}, {(VF)maxID,VD+VIPID},
  {(VF)maxDB,VD+VIPOKA}, {(VF)maxDI,VD+VIPDI}, {(VF)maxDD,VD+VIP}, 
  {(VF)maxDD,VD+VDD+VIP}, {(VF)maxXX,VX+VXX}, {(VF)maxQQ,VQ+VQQ}, {(VF)maxSS,VSB}},
 {{(VF)orinsB,VB}, {(VF)maxinsI,VI}, {(VF)maxinsD,VD}, {(VF)maxinsD,VD+VDD}, {(VF)maxinsX,VX}, {0,0},        {(VF)maxinsS,VSB}},
 {{(VF)orpfxB,VB}, {(VF)maxpfxI,VI+VIPOKW}, {(VF)maxpfxD,VD+VIPOKW}, {(VF)maxpfxD,VD+VDD}, {(VF)maxpfxX,VX}, {(VF)maxpfxQ,VQ}, {(VF)maxpfxS,VSB}},
 {{(VF)orsfxB,VB}, {(VF)maxsfxI,VI+VIPOKW}, {(VF)maxsfxD,VD+VIPOKW}, {(VF)maxsfxD,VD+VDD}, {(VF)maxsfxX,VX}, {(VF)maxsfxQ,VQ}, {(VF)maxsfxS,VSB}}},

/* 2b +  */ {
 {{(VF)plusBB,VI    }, {(VF)plusBI,VI+VIPOKW}, {(VF)plusBD,VD+VIPOKW}, 
  {(VF)plusIB,VI+VIPOKA}, {(VF)plusII,VI+VIP}, {(VF)plusID,VD+VIPID}, 
  {(VF)plusDB,VD+VIPOKA    }, {(VF)plusDI,VD+VIPDI    }, {(VF)plusDD,VD+VIP+VCANHALT}, 
  {(VF)plusZZ,VZ+VZZ+VIP}, {(VF)plusXX,VX+VXX}, {(VF)plusQQ,VQ+VQQ}, {0,0}},
 {{(VF)plusinsB,VI}, {(VF)plusinsI,VI}, {(VF)plusinsD,VD}, {(VF)plusinsZ,VZ}, {0,0},         {0,0},         {0,0}},
 {{(VF)pluspfxB,VI}, {(VF)pluspfxI,VI}, {(VF)pluspfxD,VD+VIPOKW}, {(VF)pluspfxZ,VZ}, {(VF)pluspfxX,VX}, {(VF)pluspfxQ,VQ}, {0,0}},
 {{(VF)plussfxB,VI}, {(VF)plussfxI,VI}, {(VF)plussfxD,VD+VIPOKW}, {(VF)plussfxZ,VZ}, {(VF)plussfxX,VX}, {(VF)plussfxQ,VQ}, {0,0}} },

/* 2a *  */ {
 {{(VF)andBB,  VB+VIP}, {(VF)tymesBI,VI+VIPOKW}, {(VF)tymesBD,VD+VIPOKW},
  {(VF)tymesIB,VI+VIPOKA}, {(VF)tymesII,VI+VIP}, {(VF)tymesID,VD+VIPID},
  {(VF)tymesDB,VD+VIPOKA}, {(VF)tymesDI,VD+VIPDI}, {(VF)tymesDD,VD+VIP}, 
  {(VF)tymesZZ,VZ+VZZ+VIP}, {(VF)tymesXX,VX+VXX}, {(VF)tymesQQ,VQ+VQQ}, {0,0}},
 {{(VF)andinsB,VB}, {(VF)tymesinsI,VI}, {(VF)tymesinsD,VD}, {(VF)tymesinsZ,VZ}, {0,0},          {0,0},          {0,0}},
 {{(VF)andpfxB,VB}, {(VF)tymespfxI,VI}, {(VF)tymespfxD,VD+VIPOKW}, {(VF)tymespfxZ,VZ}, {(VF)tymespfxX,VX}, {(VF)tymespfxQ,VQ}, {0,0}},
 {{(VF)andsfxB,VB}, {(VF)tymessfxI,VI}, {(VF)tymessfxD,VD+VIPOKW}, {(VF)tymessfxZ,VZ}, {(VF)tymessfxX,VX}, {(VF)tymessfxQ,VQ}, {0,0}} },

/* 5e ^  */ {   // may produce complex numbers
 {{(VF)geBB, VB+VIP}, {(VF)powBI,VD}, {(VF)powBD,VD},
  {(VF)powIB,VI}, {(VF)powII,VD}, {(VF)powID,VD+VCANHALT},
  {(VF)powDB,VD}, {(VF)powDI,VD}, {(VF)powDD,VD+VCANHALT}, 
  {(VF)powZZ,VZ+VZZ}, {(VF)powXX,VX+VXX}, {(VF)powQQ,VQ+VQQ}, {0,0}},
 {{(VF)geinsB,VB}},
 {{(VF)gepfxB,VB}},
 {{(VF)gesfxB,VB}} },

/* 7c |  */ {
 {{(VF)ltBB, VB+VIP    }, {(VF)remII,VI+VII+VIP}, {(VF)remDD,VD+VDD+VIP},
  {(VF)remII,VI+VII+VIP}, {(VF)remII,VI+VIP    }, {(VF)remID,VI+VCANHALT    },   // remID can 'overflow' if result is nonintegral
  {(VF)remDD,VD+VDD+VIP}, {(VF)remDD,VD+VDD+VIP}, {(VF)remDD,VD+VIP+VCANHALT}, 
  {(VF)remZZ,VZ+VZZ}, {(VF)remXX,VX+VXX}, {(VF)remQQ,VQ+VQQ}, {0,0}},
 {{(VF)ltinsB,VB}},
 {{(VF)ltpfxB,VB}},
 {{(VF)ltsfxB,VB}} },

/* 21 !  */ {
 {{(VF)leBB, VB+VIP            }, {(VF)binDD,VD+VDD+VRI+VIP}, {(VF)binDD,VD+VDD+VIP}, 
  {(VF)binDD,VD+VDD+VRI+VIP}, {(VF)binDD,VD+VDD+VRI+VIP}, {(VF)binDD,VD+VDD+VIP}, 
  {(VF)binDD,VD+VDD+VIP    }, {(VF)binDD,VD+VDD+VIP    }, {(VF)binDD,VD+VIP    }, 
  {(VF)binZZ,VZ+VZZ}, {(VF)binXX,VX+VXX}, {(VF)binQQ,VX+VQQ}, {0,0}}, 
 {{(VF)leinsB,VB}},
 {{(VF)lepfxB,VB}},
 {{(VF)lesfxB,VB}} },

/* d1 o. */ {
 {{(VF)cirDD,VD+VDD}, {(VF)cirDD,VD+VDD}, {(VF)cirBD,VD},
  {(VF)cirDD,VD+VDD}, {(VF)cirDD,VD+VDD}, {(VF)cirID,VD},
  {(VF)cirDD,VD+VDD}, {(VF)cirDD,VD+VDD}, {(VF)cirDD,VD}, 
  {(VF)cirZZ,VZ+VZZ+VRD}, {(VF)cirDD,VD+VDD}, {(VF)cirDD,VD+VDD}, {0,0}},
 {{0,0}},
 {{0,0}},
 {{0,0}} }
};


static UC vaptr[256]={
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* 0 */
// C0  C1          ^D                    TAB LF          CR         
   1,  2,  3,  4,  5,  6,  7,  8,    9, 10, 11, 12, 13, 14, 15, 16, /* 1 */
// <-----------------------bitwise functions -------------------->
   0, VA2OUTOF,  0,  0,  0, VA2DIV,  0,  0,    0,  0, VA2MULT, VA2PLUS,  0, VA2MINUS,  0,  0, /* 2 */
//     !   "   #   $   %   &   '     (   )   *   +   ,   -   .   /  
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0, VA2LT, VA2EQ, VA2GT,  0, /* 3 */
// NOUN    HOOK FK ADVF                      :   ;   <   =   >   ?  
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* 4 */
// @                                                                
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0, VA2POW,  0, /* 5 */
//                                               [   \   ]   ^   _  
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* 6 */
// `                                                                
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0, VA2RESIDUE,  0,  0,  0, /* 7 */
//                                               {   |   }   ~      
   0,  0, VA2MIN, VA2LE, VA2MAX, VA2GE,  0,  0,   VA2GCD, VA2NOR, VA2LCM, VA2NAND,  0,  0,  0,  VA1ROOT, /* 8 */
// =.  =:  <.  <:  >.  >:  _.        +.  +:  *.  *:  -.  -:  %.  %: 
   VA1LOG,  0,  0,  0,  0, VA2NE,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* 9 */
// ^.  ^:  $.  $:  ~.  ~:  |.  |:    ..  .:  :.  ::  ,.  ,:  ;.  ;: 
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* a */
// #.  #:  !.  !:  /.  /:  \.  \:    [.  [:  ].  ]:  {.  {:  }.  }: 
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* b */
// ".  ":  `.  `:  @.  @:  &.  &:    ?.  ?:  a.  A.  a:  b.  c.     
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* c */
// C.  d.  D.  D:  e.  E.  f.  F:    H.  i.  i:  I.  I:  j.  L.  L: 
   0,  0,  0, VA2CIRCLE,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* d */
// m.  M.  n.  o.  p.  p:  Q:  q:    r.  s:  S:  t.  t:  T.  u.  u: 
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* e */
// v.  x.  x:  y.                    0:  }ip }cs {:: {:: }:: &.: p..
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* f */
// ,ip                                                           FF  
  };   /* index in va[] for each ID */
/*   1   2   3   4   5   6   7     8   9   a   b   c   d   e   f   */

// return atomic2 ID for the verb w.  If w is b., look for its u operand and return the appropriate 
C jtvaid(J jt,A w){A x;C c;I k;V*v;
 v=FAV(w); c=v->id;
 if(c==CBDOT){x=v->fgh[1]; if(INT&AT(x)&&!AR(x)&&(k=*AV(x),(k&-16)==16))c=(C)k;}
 R vaptr[(UC)c]?c:0;
}

// prepare a primitive atomic verb for lookups using var()
// if the verb is atomic, we fill in the h field with a pointer to the va row for the verb
void va2primsetup(A w){
 UC xlatedid = vaptr[(UC)FAV(w)->id];  // see which line it is
 // If the id is a comparison operator, turn on the MSB flag bit
 I shiftamt=xlatedid-VA2NE;
 xlatedid += (((((I)0x80<<(VA2LT-VA2NE))|((I)0x80<<(VA2EQ-VA2NE))|((I)0x80<<(VA2GT-VA2NE))|((I)0x80<<(VA2LE-VA2NE))|((I)0x80<<(VA2GE-VA2NE))|((I)0x80<<(VA2NE-VA2NE)))>>shiftamt)&0x80)&((~shiftamt)>>(BW-1));
 FAV(w)->lc=xlatedid;  // save primitive number for use in ssing and monads
 xlatedid=(xlatedid&0x7f)>VA2CIRCLE?0:xlatedid;  // if this op is monad-only, don't set dyad info & flags
 FAV(w)->localuse.lvp[0]=(xlatedid?&va[xlatedid&0x7f]:0);  // point to the line, or 0 if invalid
 if(xlatedid)FAV(w)->flag |= VISATOMIC2;  // indicate that localuse contains AV pointer
}

A jtcvz(J jt,I cv,A w){I t;
 t=AT(w);
 if(cv&VRD&&!(t&FL) )R pcvt(FL,w);  // convert if possible
 if(cv&VRI&&!(t&INT))R icvt(w);  // convert to integer if possible
 R w;
}    /* convert result */

#if 0 // obsolete 
// Routine to lookup function/flags
// ptr is the type of lookup (insert/prefix/suffix) to generate the function for
// In the function, id is the pseudochar for the function to look up
//  t is the argument type
//  action routine is stored in *ado, flags in *cv.  If no action routine, *ado=0
#define VAF(fname,ptr,fp,fm,ft)   \
 void fname(J jt,C id,I t,VF*ado,I*cv){VA2*p;  \
  if(jt->jerr>=EWOV){                          \
   jt->jerr=0;                                 \
   *ado=(id==CPLUS?fp:id==CMINUS?fm:ft);       \
   *cv=VD;                                     \
  }else if(t&=NUMERIC+SBT){                     \
   p=(va+vaptr[(UC)id])->ptr+(t<=FL?(t>>INTX):t<=RAT?(3+(t>>XNUMX)):6);  \
   *ado=p->fgh[0]; *cv=p->cv;                       \
  }else *ado=0;                                \
 }
#endif
// Routine to lookup function/flags for u/ u\ u\.
// ptr is the type of lookup (insert/prefix/suffix) to generate the function for
// In the function, id is the pseudochar for the function to look up
//  t is the argument type
//  action routine is stored in *ado, flags in *cv.  If no action routine, *ado=0
#define VA2F(fname,ptr,fp,fm,ft)   \
 static VA2 fname##EWOV[] = { {ft,VD} , {fp,VD}, {fm,VD}, {0,0} }; \
 VA2 jt##fname(J jt,A self,I t){  \
  if(!(jt->jerr>=EWOV)){  /* normal case, starting the op */                        \
   t&=(NUMERIC+SBT)&(~SPARSE); t=FAV(self)->flag&VISATOMIC2?t:0;   /* t=0 is not numeric or not atomic fn */  \
   I tx=t>>INTX; if(t>FL){tx>>=(XNUMX-INTX); tx=t>RAT?3:tx; tx+=3;}  /* index of type */  \
   VA2 *ra=((VA*)(FAV(self)->localuse.lvp[0]))->ptr+tx; ra=t?ra:fname##EWOV+3;  /* point to return slot */  \
   R *ra;   /* return the routine address/flags */  \
  }else{ /* here to recover from integer overflow */ \
   jt->jerr=0;                                 \
   R fname##EWOV[2*(FAV(self)->id==CMINUS) + (FAV(self)->id==CPLUS)]; \
  }  \
 }

#if 0 // obsolete
   if((t&=(NUMERIC+SBT)&(~SPARSE))&&FAV(self)->flag&VISATOMIC2){  /* numeric input, verb with dataline */        \
    R ((VA*)(FAV(self)->localuse.lvp[0]))->ptr[t<=FL?(t>>INTX):t<=RAT?(3+(t>>XNUMX)):6];  \
   }else R fname##EWOV[3];                                \

#endif

// Lookup the action routine & flags for insert/prefix/suffix
// first name is name of the generated function
// second name (e. g. pins) is the name of the structure element containing the action-routine pointers
// other names are the names of overflow action routines for plus,minus,times respectively

VA2F(vains,pins, plusinsO,minusinsO,tymesinsO)
VA2F(vapfx,ppfx, pluspfxO,minuspfxO,tymespfxO)
VA2F(vasfx,psfx, plussfxO,minussfxO,tymessfxO)

#define VARCASE(e,c) (70*(e)+(c))

// Table converting operand types to slot numbers in a va[] entry
// Employed when one arg is known to be CMPX/XNUM/RAT.  Indexed by
// bitmask of RAT,CMPX,FL.  Entry 9=CMPX; otherwise entry 8=some FL, otherwise
// entry 10 for XNUM, 11 for some RAT 
// static UC xnumpri[] = {10 ,8 ,9 ,9 ,11 ,8 ,9 ,9};
#define xnumpri 0x998B998AU   // we use shift to select 4-bit sections

#if 0 // for debug, to display info about a sparse block
if(AT(a)&SPARSE){
printf("va2a: shape="); A spt=a; DO(AR(spt), printf(" %d",AS(spt)[i]);) printf("\n");
printf("va2a: axes="); spt=SPA(PAV(spt),a); DO(AN(spt), printf(" %d",IAV(spt)[i]);) printf("\n"); 
printf("va2a: indexes="); spt=SPA(PAV(a),i); DO(AN(spt), printf(" %d",IAV(spt)[i]);) printf("\n");
}
#endif

// All dyadic arithmetic verbs f enter here, and also f"n.  a and w are the arguments, id
// is the pseudocharacter indicating what operation is to be performed.  self is the block for this primitive,
// ranks are the ranks of the verb, argranks are the ranks of a and w combined into 1 field
static A jtva2(J jt,AD * RESTRICT a,AD * RESTRICT w,AD * RESTRICT self,RANK2T ranks,UI argranks){A z;I m,
     mf,n,nf,shortr,* RESTRICT s,*sf,zn,awzk[3];VA2 adocv;UI fr;  // fr will eventually be frame/rank  nf (and mf) change roles during execution
 fr=argranks>>RANKTX; shortr=argranks&RANKTMSK;  // fr,shortr = ar,wr to begin with.  Changes later
 F2PREFIP;
 {I at=AT(a);
  I wt=AT(w);
  if(((-(((I)jtinplace&(JTRETRY|JTEMPTY))|((UNSAFE(at|wt))&(NOUN&~(B01|INT|FL))))))>=0){  // no error, bool/int/fl args, no empties
   // Here for the fast and important case, where the arguments are both B01/INT/FL
   VA *vainfo=(VA*)FAV(self)->localuse.lvp[0];  // extract table line from the primitive
   // The index into va is atype*3 + wtype, calculated sneakily.  We test here to avoid the call overhead
   jt->mulofloloc = 0;  // Reinit multiplier-overflow count, in case we hit overflow.  Needed only on integer multiply, but there's no better place
   adocv=vainfo->p2[(UNSAFE(at)>>(INTX-1))+((UNSAFE(at)+UNSAFE(wt))>>INTX)];
  }else{

   // If an operand is empty, turn it to Boolean, and if the OTHER operand is non-numeric, turn that to Boolean too (leaving
   //  rank and shape untouched).  This change to the other operand is notional only - we won't actually convert
   // when there is an empty - but it guarantees that execution on an empty never fails.
   // If we switch a sparse nonnumeric matrix to boolean, that may be a space problem; but we don't
   // support nonnumeric sparse now
   // if an operand is sparse, replace its type with the corresponding non-sparse type, for purposes of testing operand precisions
   if((at|wt)&SPARSE){
    at=(SPARSE&at)?DTYPE(at):at;
    wt=(SPARSE&wt)?DTYPE(wt):wt;
    jtinplace=0;  // We use jtinplace==0 as a flag meaning 'sparse'
   }
   // We could allocate the result block here & avoid the test after the allocation later.  But we would have to check for agreement etc
   if(AN(a)==0){at=B01;if(!(wt&NUMERIC))wt=B01;}  // switch empty arg to Boolean & ensure compatibility with other arg
   if(AN(w)==0){wt=B01;if(!(at&NUMERIC))at=B01;}

   // Figure out the result type.  Don't signal the error from it yet, because domain has lower priority than agreement
   // Extract zt, the type of the result, and cv, the flags indicating the types selected for the arguments and the result
   adocv=var(self,at,wt);
  }
 }

 // finish up the computation of sizes.  We have to defer this till after var() because
 // if we are retrying the operation, we may be in error state until var clears it; and prod and mult can fail,
 // so we have to RE when we call them

// I ar = AR(a);
// I wr = AR(w);
 // Analyze the rank and calculate cell shapes, counts, and sizes.
 // We detect agreement error before domain error
 {//I *as = AS(a); I *ws = AS(w);
  if(ranks==0){ // rank 0 0 means no outer frames, sets up faster
   // No rank specified.  Since all these verbs have rank 0, that simplifies quite a bit.  ak/wk/zk/sf are not needed and are garbage
   // n is not needed for sparse, but we start it early to get it finished
   {A t=a;  // will hold, first the longer-rank argument, then the shorter
    I raminusw=fr-shortr; t=raminusw<0?w:t;  // t->block with longer frame
    s=AS(t); zn=AN(t);     // atoms, shape of larger frame.  shape first.  The dependency chain is s/r/shortr->n->move data
    nf=raminusw>>(BW-1);  // nf=-1 if w has longer frame, means cannot inplace a
    raminusw=-raminusw;   // now aw-ar
    mf=raminusw>>(BW-1);  // mf=-1 if a has longer frame, means cannot inplace w
    raminusw=raminusw&nf; fr+=raminusw; shortr-=raminusw;  // if ar is the longer one, change nothing; otherwise transfer aw-ar from shortr to r
    PROD(n,fr-shortr,s+shortr);  // treat the entire operands as one big cell; get the rest of the values needed
    t=(A)((I)t^((I)a^(I)w)); m=AN(t);  // rank, atoms of shorter frame  m needed for data move
   }
   // notionally we now repurpose fr to be frame/rank, with the frame 0
   if(jtinplace){
    // Non-sparse setup for copy loop, no rank
      // get number of inner cells
    n^=((1-n)>>(BW-1))&nf;  // encode 'w has long frame, so a is repeated' as complementary n; but if n<2, leave it alone.  Since n=1 when frames are equal, nf in that case is N/C
    nf=(adocv.cv>>VIPOKWX) & ((I)(a==w)-1) & (3 + nf*2 + mf);  // set inplaceability here: not if addresses equal (in case of retry); only if op supports; only if nonrepeated cell
    jtinplace = (J)(((I)jtinplace&nf)+4*nf+(adocv.cv&-16));  // bits 0-1 of jtinplace are combined input+local; 2-3 just local; 4+ hold adocv.cv; at least one is set to show non-sparse
    mf=nf=1;  // suppress the outer loop, leaving only the loop over m and n
   }else{
    // Sparse setup: move the block-local variables to longer-lived ones.  We are trying to reduce register pressure
    // repurpose ak/wk/mf/nf to hold acr/wcr/af/wf, which we will pass into vasp.  This allows acr/wcr/af/wf to be block-local
    awzk[0]=argranks>>RANKTX; awzk[1]=argranks&RANKTMSK; mf=0; nf=0;
   }
  }else{I af,wf,acr,wcr,q,ak,wk,zk;
   // Here, a rank was specified.  That means there must be a frame, according to the IRS rules
   acr=ranks>>RANKTX; acr=(I)fr<acr?fr:acr; af=fr-acr; PROD(ak,acr,AS(a)+af);  // r=left rank of verb, acr=effective rank, af=left frame, here ak=left #atoms/cell
   wcr=(RANKT)ranks; wcr=shortr<wcr?shortr:wcr; wf=shortr-wcr; PROD(wk,wcr,AS(w)+wf); // r=right rank of verb, wcr=effective rank, wf=right frame, here wk=right #atoms/cell
       // note: the prod above can never fail, because it gives the actual # cells of an existing noun
   // Now that we have used the rank info, clear jt->ranks.
   // we do this before we generate failures
   // if the frames don't agree, that's always an agreement error
   if(jtinplace){  // If not sparse... This block isn't needed for sparse arguments, and may fail on them.  We move it here to reduce register pressure
    nf=acr<=wcr; zk=acr<=wcr?wk:ak; m=acr<=wcr?ak:wk; fr=acr<=wcr?wcr:acr; shortr=acr<=wcr?acr:wcr; s=AS(acr<=wcr?w:a)+(acr<=wcr?wf:af); PROD(n,fr-shortr,s+shortr);   // nf='w has long frame'; zk=#atoms in cell with larger rank;
    n^=((1-n)>>(BW-1))&-nf;  // encode 'w has long frame, so a is repeated' as complementary n; but if n<2, leave it alone
    // m=#atoms in cell with shorter rank; n=#times shorter-rank cells must be repeated; r=larger of cell-ranks; s->shape of larger-rank cell
    // now shortr has the smaller cell-rank, and acr/wcr are free
    // if the cell-shapes don't match, that's an agreement error UNLESS the frame contains 0; in that case it counts as
    // 'error executing on the cell of fills' and produces a scalar 0 as the result for that cell, which we handle by changing the result-cell rank to 0
    // Nonce: continue giving the error even when frame contains 0 - remove 1|| in the next line to conform to fill-cell rules
// this shows the fix   if(ICMP(as+af,ws+wf,MIN(acr,wcr))){if(1||zn)ASSERT(0,EVLENGTH)else r = 0;}
    ASSERTAGREE(AS(a)+af, AS(w)+wf, shortr)  // now shortr is free
    // if looping required, calculate the strides for input & output.  Needed only if mf or nf>1, but not worth testing, since presumably one will, else why use rank?
    // zk=result-cell size in bytes; ak,wk=left,right arg-cell size in bytes.  Not needed if not looping
    ak*=bp(AT(a)); wk*=bp(AT(w));  // calculate early, using bp, to minimize ALU time & allow time for load/mul to settle.  zt may still be settling
    awzk[0]=ak; awzk[1]=wk;  // get these values out of registers - the compiler thinks they're needed for a loop
    {I f=af<=wf?wf:af; q=af<=wf?af:wf; sf=AS(af<=wf?w:a); mf=af<=wf;   // f=#longer frame; q=#shorter frame; sf->shape of arg with longer frame   mf holds -1 if wf is longer   af/wf free
    nf=(adocv.cv>>VIPOKWX) & ((I)(a==w)-1) & ((I)((argranks>>RANKTX)==f+fr)*2 + (I)((argranks&RANKTMSK)==f+fr));  // set inplaceability here: not if addresses equal (in case of retry); only if op supports; only if nonrepeated cell
    jtinplace = (J)(((I)jtinplace&nf)+4*nf+(adocv.cv&-16));  // bits 0-1 of jtinplace are combined input+local; 2-3 just local; 4+ hold adocv.cv; at least one is set to show non-sparse
    PROD(nf,f-q,q+sf);    // mf=#cells in common frame, nf=#times shorter-frame cell must be repeated.  Not needed if no cells.  First, encode 'wf longer' in sign of nf
    fr+=f<<RANKTX;  // encode f into fr
    }
#ifdef DPMULD
    { DPMULDDECLS DPMULD(nf,zk,zn,{jsignal(EVLIMIT);R 0;}) nf^=((1-nf)>>(BW-1))&-mf; PROD(mf,q,sf); DPMULD(zn,mf,zn,{jsignal(EVLIMIT);R 0;}) }
#else
    RE(zn=mult(nf,zk)); nf^=((1-nf)>>(BW-1))&-mf; PROD(mf,q,sf); RE(zn=mult(mf,zn));  // zn=total # result atoms  (only if non-sparse)
#endif
    zk*=bp(rtype((I)jtinplace));  // now create zk, which is used later than ak/wk.
    awzk[2]=zk;  // move it out of registers
   }else{awzk[0]=acr; awzk[1]=wcr; mf=af; nf=wf;}  // For sparse, repurpose ak/wk/mf/nf to hold acr/wcr/af/wf, which we will pass into vasp.  This allows acr/wcr/af/wf to be block-local
  }
  // TODO: for 64-bit, we could move f and r into upper jtinplace; use bit 4 of jtinplace for testing below; make f/r block-local; extract f/r below as needed
 }

 RESETRANK;  // This is required for xnum/rat/sparse, which call IRS-enabled routines internally.  We could suppress this for mainline types, perhaps in var().  Anyone who sets this must sets it back,
             // so it's OK that we don't clear it if we have error

 // Signal domain error if appropriate. Must do this after agreement tests
 ASSERT(adocv.f,EVDOMAIN);
 if(jtinplace){   // if not sparse...
  // Not sparse.

   // If op specifies forced input conversion AND if both arguments are non-sparse: convert them to the selected type.
   // Incompatible arguments were detected in var().  If there is an empty operand, skip conversions which
   // might fail because the type in t is incompatible with the actual type in a.  t is rare.
   //
   // Because of the priority of errors we mustn't check the type until we have verified agreement above
   if((I)jtinplace&VARGMSK&&zn>0){I t=atype((I)jtinplace);  // t not 0, and the result is not empty
    // Conversions to XNUM use a routine that pushes/sets/pops jt->mode, which controls the
    // type of conversion to XNUM in use.  Any result of the conversion is automatically inplaceable.  If type changes, change the cell-size too
    // bits 2-3 of jtinplace indicate whether inplaceability is allowed by the op, the ranks, and the addresses
    if(TYPESNE(t,AT(a))){awzk[0]>>=bplg(AT(a)); RZ(a=!(t&XNUM)?cvt(t,a):xcvt(((I)jtinplace&VXCVTYPEMSK)>>VXCVTYPEX,a));jtinplace = (J)(intptr_t)((I)jtinplace | (((I)jtinplace>>2)&JTINPLACEA)); awzk[0]<<=bplg(AT(a)); forcetomemory(awzk);}
    if(TYPESNE(t,AT(w))){awzk[1]>>=bplg(AT(w)); RZ(w=!(t&XNUM)?cvt(t,w):xcvt(((I)jtinplace&VXCVTYPEMSK)>>VXCVTYPEX,w));jtinplace = (J)(intptr_t)((I)jtinplace | (((I)jtinplace>>2)&JTINPLACEW)); awzk[1]<<=bplg(AT(w));}
   }  // the function call here inhibits register assignment to temporaries.  It might be better to do the conversion earlier, and defer the error
      // until here.  We will have to look at the generated code when we can use all the registers

   // The call to forcetomemory above guarantees that awzk will not be allocated to registers.  We need them for a/w etc

   // From here on we have possibly changed the address of a and w, but we are still using shape pointers
   // in the original input block.  That's OK.

// less regs, more comp    inplaceallow &= (I)jtinplace;  // allow only blocks originally marked inplaceable
  
   // Allocate a result area of the right type, and copy in its cell-shape after the frame
  // If an argument can be overwritten, use it rather than allocating a new one
  // Argument can be overwritten if: action routine allows it; flagged in jtinplace; usecount 1 or zombie; rank equals (length of longer frame)+(length of longer cell)
  // If the argument has rank that large, and the arguments agree, the argument MUST have the same number of atoms as the result, because all shape is accounted for.
  // rank = rank of result (the rank of the result is the sum of (the longer frame-length) plus (the larger cell-rank))
  // Also, if the operation is one that may abort, we suppress inplacing it if the user can't handle early assignment.
  // Finally, if a==w suppress inplacing, in case the operation must be retried (we could check which ones but they are
  // just not likely to be used reflexively)
  // 
  // If the # atoms is same as result, each atom must be read only once & can be written after it has been read
  // This holds true even if the atoms have different types, as long as they have the same size (this is encoded in the IPOK bits of cv)
  // use the inplacing from the action routine to qualify the caller's inplacing, EXCEPT
  // for Boolean inputs: since we do the operations a word at a time, they may overrun the output area and
  // are thus not inplaceable.  The exception is if there is only one loop through the inputs; that's always inplaceable
// less regs, more comp   {I inplaceallow = (adocv.cv>>VIPOKWX) & (((a==w)|((f!=0)&zt))-1) & ((((zn^an)|(ar^(f+r)))==0)*2 + (((zn^wn)|(wr^(f+r)))==0));
   
      // also turn off inplacing if a==w  or if Boolean with repeated cells   uses B01==1
      // (mf|nf)>1 is a better test than f!=0, because it handles frame of all 1s, but it's slower in the normal case

   // Establish the result area z; if we're reusing an argument, make sure the type is updated to the result type
   // If the operation is one that can fail partway through, don't allow it to overwrite a zombie input unless so enabled by the user
  // The ordering here assumes that jtinplace will usually be set
// alternative  if(((I)jtinplace&1) && (AC(w)<ACUC1 || AC(w)==ACUC1&&jt->assignsym&&jt->assignsym->val==w&&!(adocv.cv&VCANHALT && jt->asgzomblevel<2))){z=w; if(TYPESNE(AT(w),zt))MODBLOCKTYPE(z,zt)  //  Uses JTINPLACEW==1
// alternative  }else if(((I)jtinplace&2) && (AC(a)<ACUC1 || AC(a)==ACUC1&&jt->assignsym&&jt->assignsym->val==a&&!(adocv.cv&VCANHALT && jt->asgzomblevel<2))){z=a; if(TYPESNE(AT(a),zt))MODBLOCKTYPE(z,zt)  //  Uses JTINPLACEA==2
  if( (SGNIF(jtinplace,JTINPLACEWX)&AC(w))<0 || ((SGNIF(jtinplace,JTINPLACEWX)&(AC(w)-2))<0)&&jt->assignsym&&jt->assignsym->val==w/* no longer &&!((I)jtinplace&VCANHALT && jt->asgzomblevel<2)*/){z=w; I zt=rtype((I)jtinplace); if(TYPESNE(AT(w),zt))MODBLOCKTYPE(z,zt)  //  Uses JTINPLACEW==1
  }else if( (SGNIF(jtinplace,JTINPLACEAX)&AC(a))<0 || ((SGNIF(jtinplace,JTINPLACEAX)&(AC(a)-2))<0)&&jt->assignsym&&jt->assignsym->val==a/* no longer &&!((I)jtinplace&VCANHALT && jt->asgzomblevel<2)*/){z=a; I zt=rtype((I)jtinplace); if(TYPESNE(AT(a),zt))MODBLOCKTYPE(z,zt)  //  Uses JTINPLACEA==2
  }else{GA(z,rtype((I)jtinplace),zn,(RANKT)fr+(fr>>RANKTX),0); MCISH(AS(z),sf,fr>>RANKTX); MCISH(AS(z)+(fr>>RANKTX),s,(RANKT)fr);} 
  // s, fr, and sf ARE NOT USED FROM HERE ON in this branch to reduce register pressure.
  if(!zn)RETF(z);  // If the result is empty, the allocated area says it all
  // zn  NOT USED FROM HERE ON

  // End of setup phase.  The execution phase:

  // The compiler thinks that because ak/wk/zk are used in the loop they should reside in registers.  We do better to keep a and w in registers.  So we
  // force the compiler to spill awzk.
  {
   {C *av=CAV(a); C *wv=CAV(w); C *zv=CAV(z);   // point to the data
    // Call the action routines: nf,mf, etc must be preserved in case of repair
    // note: the compiler unrolls these call loops.  Would be nice to suppress that
    if(nf-1==0){I i=mf; do{((AHDR2FN*)adocv.f)(n,m,av,wv,zv,jt); if(!--i)break; av+=awzk[0]; wv+=awzk[1]; zv+=awzk[2];}while(1);}  // if the short cell is not repeated, loop over the frame
    else if(nf-1<0){I im=mf; do{I in=~nf; do{((AHDR2FN*)adocv.f)(n,m,av,wv,zv,jt); wv+=awzk[1]; zv+=awzk[2];}while(--in); av+=awzk[0];}while(--im);} // if right frame is longer, repeat cells of a
    else         {I im=mf; do{I in=nf; do{((AHDR2FN*)adocv.f)(n,m,av,wv,zv,jt); av+=awzk[0]; zv+=awzk[2];}while(--in); wv+=awzk[1];}while(--im);}  // if left frame is longer, repeat cells of w
   }
   // The work has been done.  If there was no error, check for optional conversion-if-possible or -if-necessary
   if(!jt->jerr){RETF(!((I)jtinplace&VRI+VRD)?z:cvz((I)jtinplace,z));  // normal return is here.  The rest is error recovery
   }else if(jt->jerr-EWOVIP>=0){A zz;C *zzv;I zzk;
    // Here for overflow that can be corrected in place
    // If the original result block cannot hold the D result, allocate a separate result area
    if(sizeof(D)==sizeof(I)){zz=z; MODBLOCKTYPE(zz,FL); zzk=awzk[2];   // shape etc are already OK
    }else{GATV(zz,FL,AN(z),AR(z),AS(z)); zzk=awzk[2]*(sizeof(D)/sizeof(I));}
    // restore pointers to beginning of arguments
    zzv=CAV(zz);  // point to new-result data
    // Set up pointers etc for the overflow handling.  Set b=1 if w is taken for the x argument to repair
    if(jt->jerr==EWOVIP+EWOVIPMULII){D *zzvd=(D*)zzv; I *zvi=IAV(z);
     // Multiply repair.  We have to convert all the pre-overflow results to float, and then finish the multiplies
     DQ(jt->mulofloloc, *zzvd++=(D)*zvi++;);  // convert the multiply results to float
     // Now repeat the processing.  Unlike with add/subtract overflow, we have to match up all the argument atoms
     adocv.f=(VF)tymesIIO;  // multiply-repair routine
     {C *av=CAV(a); C *wv=CAV(w);
      I wkm,wkn,akm,akn;
      wkm=awzk[1], akn=awzk[0]; wkn=nf>>(BW-1); nf^=wkn;   // wkn=111..111 iff wk increments with n (and therefore ak with m).  Make nf positive
      akm=akn&wkn; wkn&=wkm; wkm^=wkn; akn^=akm;  // if c, akm=ak/wkn=wk; else akn=ak/wkm=wk.  The other incr is 0
      I im=mf; do{I in=nf; do{((AHDR2FN*)adocv.f)(n,m,av,wv,zzv,jt); zzv+=zzk; av+=akn; wv +=wkn;}while(--in); if(!--im)break; av+=akm; wv +=wkm;}while(1);
     }
    } else {I nipw;   // not multiply repair, but something else to do inplace
     switch(jt->jerr-EWOVIP){
     case EWOVIPPLUSII:
      // choose the non-in-place argument
      adocv.f=(VF)plusIIO; nipw = z!=w; break; // if w not repeated, select it for not-in-place
     case EWOVIPPLUSBI:
      adocv.f=(VF)plusBIO; nipw = 0; break;   // Leave the Boolean argument as a
     case EWOVIPPLUSIB:
      adocv.f=(VF)plusBIO; nipw = 1; break;  // Use w as not-in-place
     case EWOVIPMINUSII:
      adocv.f=(VF)minusIIO; nipw = z!=w; break; // if w not repeated, select it for not-in-place
     case EWOVIPMINUSBI:
      adocv.f=(VF)minusBIO; nipw = 0; break;   // Leave the Boolean argument as a
     case EWOVIPMINUSIB:
      adocv.f=(VF)minusBIO; nipw = 1; break;  // Use w as not-in-place
     }
     // nipw means 'use w as not-in-place'; c means 'repeat cells of a'; so if nipw!=c we repeat cells of not-in-place, if nipw==c we set nf to 1
     // if we are repeating cells of the not-in-place, we leave the repetition count in nf, otherwise subsume it in mf
     // b means 'repeat atoms inside a'; so if nipw!=b we repeat atoms of not-in-place, if nipw==b we set n to 1
     {C *av, *zv=CAV(z);
      I origc=(UI)nf>>(BW-1); I origb=(UI)n>>(BW-1);   // see what the original repeat flags were
      nf^=nf>>(BW-1); n^=n>>(BW-1);  // take abs of n, nf for calculations here
      if(nipw){av=CAV(w), awzk[0]=awzk[1];}else{av=CAV(a);} if(nipw==origc){mf *= nf; nf = 1;} if(nipw==origb){m *= n; n = 1;}
      n^=-nipw;  // install new setting of b flag
     // We have set up ado,nf,mf,nipw,m,n for the conversion.  Now call the repair routine.  n is # times to repeat a for each z, m*n is # atoms of z/zz
      DQ(mf, DQ(nf, ((AHDR2FN*)adocv.f)(n,m,av,zv,zzv,jt); zzv+=zzk; zv+=awzk[2];); av+=awzk[0];)  // use each cell of a (nf) times
     }
    }
    RESETERR
    R zz;  // Return the result after overflow has been corrected
   }
  }
 }else{z=vasp(a,w,FAV(self)->id,adocv.f,adocv.cv,atype(adocv.cv),rtype(adocv.cv),mf,awzk[0],nf,awzk[1],fr>>RANKTX,(RANKT)fr); if(!jt->jerr)R z;}  // handle sparse arrays separately.  at this point ak/wk/mf/nf hold acr/wcr/af/wf
 R 0;  // return to the caller, who will retry any retryable errors
}    /* scalar fn primitive and f"r main control */

/*
 acn wcn zcn  # atoms in a cell
 acr wcr r    cell rank
 af  wf  f    frame
 ak  wk  zk   # bytes in a cell
 an  wn  zn   overall # atoms
 ar  wr  f+r  overall rank

 b    1 iff cell rank of a <= cell rank of w
 c    1 iff frame     of a <= frame of w
 m    # of atoms in the cell with the smaller rank
 mf   agreed # of frames
 n    excess # of cell atoms
 nf   excess # of frames
 f    max of frame of a, frame of w
 q    min of frame of a, frame of w
 r    max of cell rank  of a, cell rank  of w
 s    max of cell shape of a, cell shape of w
 sf   max of frame shape of a, frame shape of w
*/


// 4-nested loop for dot-products.  Handles repeats for inner and outer frame.  oneprod is the code for calculating a single vector inner product *zv++ = *av++ dot *wv++
// If there is inner frame, it is the a arg that is repeated
// LIT is set in it if it is OK to use 2x2 operations (viz a has no inner frame & w has no outer frame)
#define SUMATLOOP2(ti,to,oneprod2,oneprod1) \
  {ti * RESTRICT av=avp,* RESTRICT wv=wvp; to * RESTRICT zv=zvp; \
   __m256i endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-dplen)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
   __m256d acc000; __m256d acc010; __m256d acc100; __m256d acc110; \
   __m256d acc001; __m256d acc011; __m256d acc101; __m256d acc111; \
   _mm256_zeroupper(VOIDARG); \
   DQ(nfro, I jj=nfri; ti *ov0=it&BOX?av:wv; \
    while(1){  \
     DQ(ndpo, I j=ndpi; ti *av0=av; /* i is how many a's are left, j is how many w's*/ \
      while(1){ \
       if(it&LIT&&jj>1){ \
        ti * RESTRICT wv1=wv+dplen; wv1=j==1?wv:wv1; \
        oneprod2  \
        if(j>1){--j; _mm_storeu_pd(zv,_mm256_castpd256_pd128 (acc000)); _mm_storeu_pd(zv+ndpi,_mm256_castpd256_pd128 (acc100)); wv+=dplen; zv +=2;} \
        else{_mm_storel_pd(zv,_mm256_castpd256_pd128 (acc000)); _mm_storel_pd(zv+ndpi,_mm256_castpd256_pd128 (acc100));  zv+=1;} \
       }else{ \
        oneprod1  \
        _mm_storel_pd(zv,_mm256_castpd256_pd128 (acc000)); \
        zv+=1; \
       } \
       if(!--j)break; \
       av=av0;  \
      } \
      if(it&LIT&&jj>1){--i; av+=dplen; zv+=ndpi;} \
     ) \
     if((jj-=(((it&LIT)>>1)+1))<=0)break; \
     if(it&BOX)av=ov0;else wv=ov0; \
    } \
   ) \
  }

#define SUMATLOOP(ti,to,oneprod) \
  {ti * RESTRICT av=avp,* RESTRICT wv=wvp; to * RESTRICT zv=zvp; \
   DQ(nfro, I jj=nfri; ti *ov0=it&BOX?av:wv; \
    while(1){  \
     DQ(ndpo, I j=ndpi; ti *av0=av; /* i is how many a's are left, j is how many w's*/ \
      while(1){oneprod if(!--j)break; av=av0;} \
     ) \
     if(!--jj)break; \
     if(it&BOX)av=ov0;else wv=ov0; } \
   ) \
  }

// 
#if C_AVX&&SY_64
// Do one 2x2 product of length dplen.  Leave results in acc000/010.  dplen must be >0
// av, wv, wv1 are set up
#define ONEPRODAVXD2(label,mid2x2,last2x2) {\
   acc000=_mm256_set1_pd(0.0); acc010=acc000; acc100=acc000; acc110=acc000; \
   acc001=acc000; acc011=acc000; acc101=acc000; acc111=acc000; \
   I rem=dplen; \
   if(rem>8*NPAR)goto label##8; \
   while(rem>NPAR){ \
    if(rem>4*NPAR) \
     {if(rem>6*NPAR){if(rem>7*NPAR)goto label##7;else goto label##6;}else {if(rem>5*NPAR)goto label##5;else goto label##4;}} \
    else{if(rem>2*NPAR){if(rem>3*NPAR)goto label##3;else goto label##2;}else {if(rem>1*NPAR)goto label##1;else break;}} \
    label##8: mid2x2(7,0)  label##7: mid2x2(6,1)  label##6: mid2x2(5,0)  label##5: mid2x2(4,1)  \
    label##4: mid2x2(3,0)  label##3: mid2x2(2,1)  label##2: mid2x2(1,0)  label##1: mid2x2(0,1)  \
    av+=8*NPAR; wv+=8*NPAR; wv1+=8*NPAR; \
    if((rem-=8*NPAR)>8*NPAR)goto label##8;  \
   } \
   av-=(NPAR-rem)&-NPAR; wv-=(NPAR-rem)&-NPAR; wv1-=(NPAR-rem)&-NPAR; \
   last2x2  \
   acc000=_mm256_add_pd(acc000,acc001); acc010=_mm256_add_pd(acc010,acc011); acc100=_mm256_add_pd(acc100,acc101); acc110=_mm256_add_pd(acc110,acc111);  \
   acc000=_mm256_add_pd(acc000,_mm256_permute2f128_pd(acc000,acc000,0x01)); acc010=_mm256_add_pd(acc010,_mm256_permute2f128_pd(acc010,acc010,0x01)); \
    acc100=_mm256_add_pd(acc100,_mm256_permute2f128_pd(acc100,acc100,0x01)); acc110=_mm256_add_pd(acc110,_mm256_permute2f128_pd(acc110,acc110,0x01)); \
   acc000=_mm256_add_pd(acc000,_mm256_permute_pd (acc000,0xf)); acc010=_mm256_add_pd(acc010,_mm256_permute_pd (acc010,0x0));  \
    acc100=_mm256_add_pd(acc100,_mm256_permute_pd (acc100,0xf)); acc110=_mm256_add_pd(acc110,_mm256_permute_pd (acc110,0x0)); \
   acc000=_mm256_blend_pd(acc000,acc010,0xa); acc100=_mm256_blend_pd(acc100,acc110,0xa); \
   av+=((dplen-1)&(NPAR-1))+1;  wv+=((dplen-1)&(NPAR-1))+1; \
   }

// do one 2x2, 4 combinations from offset using accumulator accno.
// av, wv, and wv1 are set
#define CELL2X2M(offset,accno) \
 acc00##accno = MUL_ACC(acc00##accno, _mm256_loadu_pd(&av[offset*NPAR]), _mm256_loadu_pd(&wv[offset*NPAR])); \
 acc01##accno = MUL_ACC(acc01##accno, _mm256_loadu_pd(&av[offset*NPAR]), _mm256_loadu_pd(&wv1[offset*NPAR])); \
 acc10##accno = MUL_ACC(acc10##accno, _mm256_loadu_pd(&av[dplen+offset*NPAR]), _mm256_loadu_pd(&wv[offset*NPAR])); \
 acc11##accno = MUL_ACC(acc11##accno, _mm256_loadu_pd(&av[dplen+offset*NPAR]), _mm256_loadu_pd(&wv1[offset*NPAR]));

// same but with mask, on cell number 0
#define CELL2X2L \
 acc000 = MUL_ACC(acc000, _mm256_maskload_pd(&av[0],endmask), _mm256_maskload_pd(&wv[0],endmask)); \
 acc010 = MUL_ACC(acc010, _mm256_maskload_pd(&av[0],endmask), _mm256_maskload_pd(&wv1[0],endmask)); \
 acc100 = MUL_ACC(acc100, _mm256_maskload_pd(&av[dplen+0],endmask), _mm256_maskload_pd(&wv[0],endmask)); \
 acc110 = MUL_ACC(acc110, _mm256_maskload_pd(&av[dplen+0],endmask), _mm256_maskload_pd(&wv1[0],endmask));

// Do one 1x1 product of length dplen.  Leave results in acc000.  dplen must be >0
// av,  wv, are set up
#define ONEPRODAVXD1(label,mid1x1,last1x1) {\
   acc000=_mm256_set1_pd(0.0); acc010=acc000; acc100=acc000; acc110=acc000; \
   acc001=acc000; acc011=acc000; acc101=acc000; acc111=acc000; \
   I rem=dplen; \
   if(rem>8*NPAR)goto label##8; \
   while(rem>NPAR){ \
    if(rem>4*NPAR) \
     {if(rem>6*NPAR){if(rem>7*NPAR)goto label##7;else goto label##6;}else {if(rem>5*NPAR)goto label##5;else goto label##4;}} \
    else{if(rem>2*NPAR){if(rem>3*NPAR)goto label##3;else goto label##2;}else {if(rem>1*NPAR)goto label##1;else break;}} \
    label##8: mid1x1(7,000)  label##7: mid1x1(6,001)  label##6: mid1x1(5,010)  label##5: mid1x1(4,011)  \
    label##4: mid1x1(3,100)  label##3: mid1x1(2,101)  label##2: mid1x1(1,110)  label##1: mid1x1(0,111)  \
    av+=8*NPAR; wv+=8*NPAR;  \
    if((rem-=8*NPAR)>8*NPAR)goto label##8;  \
   } \
   av-=(NPAR-rem)&-NPAR; wv-=(NPAR-rem)&-NPAR; \
   last1x1  \
   acc000=_mm256_add_pd(acc000,acc001); acc010=_mm256_add_pd(acc010,acc011); acc100=_mm256_add_pd(acc100,acc101); acc110=_mm256_add_pd(acc110,acc111);  \
   acc000=_mm256_add_pd(acc000,acc010); acc100=_mm256_add_pd(acc100,acc110); \
   acc000=_mm256_add_pd(acc000,acc100);  \
   acc000=_mm256_add_pd(acc000,_mm256_permute2f128_pd(acc000,acc000,0x01)); \
   acc000=_mm256_add_pd(acc000,_mm256_permute_pd (acc000,0xf)); \
   av+=((dplen-1)&(NPAR-1))+1;  wv+=((dplen-1)&(NPAR-1))+1; \
   }

// do one 1x1, using accumulator accno.
// av, wv are set
#define CELL1X1M(offset,accno) \
 acc##accno = MUL_ACC(acc##accno, _mm256_loadu_pd(&av[offset*NPAR]), _mm256_loadu_pd(&wv[offset*NPAR]));

// same but with mask, on cell number 0
#define CELL1X1L \
 acc000 = MUL_ACC(acc000, _mm256_maskload_pd(&av[0],endmask), _mm256_maskload_pd(&wv[0],endmask));



#define ONEPRODD \
 __m256i endmask; /* length mask for the last word */ \
 _mm256_zeroupper(VOIDARG); \
 /* +/ vectors */ \
 __m256d idreg=_mm256_set1_pd(0.0); \
 endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-dplen)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
 __m256d acc0=idreg; __m256d acc1=idreg; __m256d acc2=idreg; __m256d acc3=idreg; \
 DQ((dplen-1)>>(2+LGNPAR), \
  acc0=MUL_ACC(acc0,_mm256_loadu_pd(av),_mm256_loadu_pd(wv)); \
  acc1=MUL_ACC(acc1,_mm256_loadu_pd(av+1*NPAR),_mm256_loadu_pd(wv+1*NPAR)); \
  acc2=MUL_ACC(acc2,_mm256_loadu_pd(av+2*NPAR),_mm256_loadu_pd(wv+2*NPAR)); \
  acc3=MUL_ACC(acc3,_mm256_loadu_pd(av+3*NPAR),_mm256_loadu_pd(wv+3*NPAR)); av+=4*NPAR;  wv+=4*NPAR; \
 ) \
 if((dplen-1)&((4-1)<<LGNPAR)){acc0=MUL_ACC(acc0,_mm256_loadu_pd(av),_mm256_loadu_pd(wv)); \
  if(((dplen-1)&((4-1)<<LGNPAR))>=2*NPAR){acc1=MUL_ACC(acc1,_mm256_loadu_pd(av+NPAR),_mm256_loadu_pd(wv+NPAR)); \
   if(((dplen-1)&((4-1)<<LGNPAR))>2*NPAR){acc2=MUL_ACC(acc2,_mm256_loadu_pd(av+2*NPAR),_mm256_loadu_pd(wv+2*NPAR));} \
  } \
  av+=(dplen-1)&((4-1)<<LGNPAR); wv+=(dplen-1)&((4-1)<<LGNPAR);  \
 } \
 acc3=MUL_ACC(acc3,_mm256_maskload_pd(av,endmask),_mm256_maskload_pd(wv,endmask)); av+=((dplen-1)&(NPAR-1))+1;  wv+=((dplen-1)&(NPAR-1))+1; \
 acc0=_mm256_add_pd(acc0,acc1); acc2=_mm256_add_pd(acc2,acc3); acc0=_mm256_add_pd(acc0,acc2); /* combine accumulators vertically */ \
 acc0=_mm256_add_pd(acc0,_mm256_permute2f128_pd(acc0,acc0,0x01)); acc0=_mm256_add_pd(acc0,_mm256_permute_pd (acc0,0xf));   /* combine accumulators horizontally  01+=23, 0+=1 */ \
 _mm_storel_pd(zv++,_mm256_castpd256_pd128 (acc0));/* store the single result */

#else
#define ONEPRODD D total0=0.0; D total1=0.0; if(dplen&1)total1=(D)*av++*(D)*wv++; DQ(dplen>>1, total0+=(D)*av++*(D)*wv++; total1+=(D)*av++*(D)*wv++;); *zv++=total0+total1;
#endif

// routine to do the dot-product calculations.  Brought out to help the compiler allocate registers
// it=type of input, a,w=args dplen=len of each dot-product
// ndpi is the number of times to repeat each list of a within the inner loop, i. e. # dps to repeat it on
// ndpo is */ inner frame of w, i. e. the number of times to repeat the inner loop
// nfri is the number of times to repeat the short-frame operand for the outer loop
// nfri is */ surplus outer frame
// it&BOX is set if a has the shorter outer frame
// w is never repeated in the inner loop (i. e. you can have multiple w but not multiple a; exchange args to ensure this
// if LIT is set in it, it is OK to use 2x2 operations (viz inner frame of a and outer frame of w are empty)
// 'repeata' flag comes from it&BOX
I jtsumattymesprods(J jt,I it,void *avp, void *wvp,I dplen,I nfro,I nfri,I ndpo,I ndpi,void *zvp){
 if(it&FL){
  NAN0;
#if C_AVX
#if 0 // obsolete 
  {D * RESTRICT av=DAV(a),* RESTRICT wv=DAV(w); D * RESTRICT zv=DAV(z); 
   __m256i endmask = _mm256_loadu_si256((__m256i*)(jt->validitymask+((-dplen)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ 
   __m256d acc000; __m256d acc010; __m256d acc100; __m256d acc110; 
   __m256d acc001; __m256d acc011; __m256d acc101; __m256d acc111; 
   _mm256_zeroupper(VOIDARG); 
   {I i=(I)(nfro)-1;    for(;i>=0;--i){ I jj=nfri; D *ov0=repeata?av:wv; 
    while(1){  
     {I i=(I)(ndpo)-1;    for(;i>=0;--i){I j=ndpi; D *av0=av; /* i is how many a's are left, j is how many w's*/ 
      while(1){ 
       if(it&LIT&&jj>1){ 
        D * RESTRICT wv1=wv+dplen; wv1=j==1?wv:wv1; 
   {
   acc000=_mm256_set1_pd(0.0); acc010=acc000; acc100=acc000; acc110=acc000; 
   acc001=acc000; acc011=acc000; acc101=acc000; acc111=acc000; 
   I rem=dplen; 
   if(rem>8*NPAR)goto label8; 
   while(rem>NPAR){ 
    if(rem>4*NPAR) 
     {if(rem>6*NPAR){if(rem>7*NPAR)goto label7;else goto label6;}else {if(rem>5*NPAR)goto label5;else goto label4;}} 
    else{if(rem>2*NPAR){if(rem>3*NPAR)goto label3;else goto label2;}else {if(rem>1*NPAR)goto label1;else break;}} 
    label8: CELL2X2M(7,0)  label7: CELL2X2M(6,1)  label6: CELL2X2M(5,0)  label5: CELL2X2M(4,1)  
    label4: CELL2X2M(3,0)  label3: CELL2X2M(2,1)  label2: CELL2X2M(1,0)  label1: CELL2X2M(0,1)  
    av+=8*NPAR; wv+=8*NPAR; wv1+=8*NPAR; 
    if((rem-=8*NPAR)>8*NPAR)goto label8;  
   } 
   av-=(NPAR-rem)&-NPAR; wv-=(NPAR-rem)&-NPAR; wv1-=(NPAR-rem)&-NPAR; 
   CELL2X2L  
   acc000=_mm256_add_pd(acc000,acc001); acc010=_mm256_add_pd(acc010,acc011); acc100=_mm256_add_pd(acc100,acc101); acc110=_mm256_add_pd(acc110,acc111);  
   acc000=_mm256_add_pd(acc000,_mm256_permute2f128_pd(acc000,acc000,0x01)); acc010=_mm256_add_pd(acc010,_mm256_permute2f128_pd(acc010,acc010,0x01)); 
    acc100=_mm256_add_pd(acc100,_mm256_permute2f128_pd(acc100,acc100,0x01)); acc110=_mm256_add_pd(acc110,_mm256_permute2f128_pd(acc110,acc110,0x01)); 
   acc000=_mm256_add_pd(acc000,_mm256_permute_pd (acc000,0xf)); acc010=_mm256_add_pd(acc010,_mm256_permute_pd (acc010,0x0));  
    acc100=_mm256_add_pd(acc100,_mm256_permute_pd (acc100,0xf)); acc110=_mm256_add_pd(acc110,_mm256_permute_pd (acc110,0x0)); 
   acc000=_mm256_blend_pd(acc000,acc010,0xa); acc100=_mm256_blend_pd(acc100,acc110,0xa); 
   av+=((dplen-1)&(NPAR-1))+1;  wv+=((dplen-1)&(NPAR-1))+1; 
   }
// scaf        ONEPRODAVXD2(D2,CELL2X2M,CELL2X2L)  
        if(j>1){--j; _mm_storeu_pd(zv,_mm256_castpd256_pd128 (acc000)); _mm_storeu_pd(zv+ndpi,_mm256_castpd256_pd128 (acc100)); wv+=dplen; zv +=2;} 
        else{_mm_storel_pd(zv,_mm256_castpd256_pd128 (acc000)); _mm_storel_pd(zv+ndpi,_mm256_castpd256_pd128 (acc100));  zv+=1;} 
       }else{ 
ONEPRODAVXD1(D1,CELL1X1M,CELL1X1L)
        _mm_storel_pd(zv,_mm256_castpd256_pd128 (acc000)); 
        zv+=1; 
       } 
       if(!--j)break; 
       av=av0;  
      } 
      if(it&LIT&&jj>1){--i; av+=dplen; zv+=ndpi;} 
     }}
     if((jj-=(((it&LIT)>>1)+1))<=0)break; 
     if(repeata)av=ov0;else wv=ov0; 
    } 
   }}
  }
#else
  SUMATLOOP2(D,D,ONEPRODAVXD2(D2,CELL2X2M,CELL2X2L),ONEPRODAVXD1(D1,CELL1X1M,CELL1X1L));
#endif
#else
  SUMATLOOP(D,D,ONEPRODD)
#endif
  if(NANTEST){  // if there was an error, it might be 0 * _ which we will turn to 0.  So rerun, checking for that.
   NAN0;
   SUMATLOOP(D,D,D total=0.0; DQ(dplen, D u=*av++; D v=*wv++; if(u&&v)total+=dmul2(u,v);); *zv++=total;)
   NAN1;
  }
 }else if(it&INT){
  SUMATLOOP(I,D,D total0=0.0; D total1=0.0; if(dplen&1)total1=(D)*av++*(D)*wv++; DQ(dplen>>1, total0+=(D)*av++*(D)*wv++; total1+=(D)*av++*(D)*wv++;); *zv++=total0+total1;)
 }else{
  SUMATLOOP(B,I,
   I total=0; I k=dplen; I *avi=(I*)av; I *wvi=(I*)wv;
    while(k>>LGSZI){I kn=MIN(255,k>>LGSZI); k-=kn<<LGSZI; I total2=0; DQ(kn, total2+=*avi++&*wvi++;) ADDBYTESINI(total2); total+=total2;} av=(B*)avi; wv=(B*)wvi; DQ(k, total+=*av++&*wv++;)
   *zv++=total;
  )
 }
 RETF(1);
}



// +/@:*"1 with IRS
DF2(jtsumattymes1){
 RZ(a&&w);
 I ar=AR(a); I wr=AR(w); I acr=jt->ranks>>RANKTX; I wcr=jt->ranks&RMAX;
 // get the cell-ranks to use 
 acr=ar<acr?ar:acr;   // r=left rank of verb, acr=effective rank
 wcr=wr<wcr?wr:wcr;  // r=right rank of verb, wcr=effective rank
     // note: the prod above can never fail, because it gives the actual # cells of an existing noun
   // Now that we have used the rank info, clear jt->ranks.  All verbs start with jt->ranks=RMAXX unless they have "n applied
   // we do this before we generate failures
 RESETRANK;  // This is required if we go to slower code
 // if an argument is empty, sparse, has cell-rank 0, or not a fast arithmetic type, revert to the code for f/@:g atomic
 if(((-((AT(a)|AT(w))&(NOUN&~(B01|INT|FL))))|(AN(a)-1)|(AN(w)-1)|(acr-1)|(wcr-1))<0) { // test for all unusual cases
  R rank2ex(a,w,FAV(self)->fgh[0],MIN(acr,1),MIN(wcr,1),acr,wcr,jtfslashatg);
 }
 // We can handle it here, and both ranks are at least 1.

 // If there is no additional rank (i. e. highest cell-rank is 1), ignore the given rank (which must be 1 1) and use the argument rank
 // This promotes the outer loops to inner loops
 {I rankgiven = (acr|wcr)-1; acr=rankgiven?acr:ar; wcr=rankgiven?wcr:wr;}

 // Exchange if needed to make the cell-rank of a no greater than that of w.  That way we know that w will never repeat in the inner loop
 if(acr>wcr){A t=w; I tr=wr; I tcr=wcr; w=a; wr=ar; wcr=acr; a=t; ar=tr; acr=tcr;}

 // Convert arguments as required
 I it=MAX(AT(a),AT(w));  // if input types are dissimilar, convert to the larger
 if(it!=(AT(w)|AT(a))){
  if(TYPESNE(it,AT(a))){RZ(a=cvt(it,a));}  // convert to common input type
  else if(TYPESNE(it,AT(w))){RZ(w=cvt(it,w));}
 }

 // Verify inner frames match
 ASSERTAGREE(AS(a)+ar-acr, AS(w)+wr-wcr, acr-1) ASSERT(AS(a)[ar-1]==AS(w)[wr-1],EVLENGTH);  // agreement error if not prefix match

 // calculate inner repeat amounts and result shape
 I dplen = AS(a)[ar-1];  // number of atoms in 1 dot-product
 I ndpo; PROD(ndpo,acr-1,AS(w)+wr-wcr);  // number of cells of a = # 2d-level loops
 I ndpi; PROD(ndpi,wcr-acr,AS(w)+wr-wcr+acr-1);  // number of times each cell of a must be repeated (= excess frame of w)
 I zn=ndpo*ndpi;  // number of results from 1 inner cell.  This can't overflow since frames agree and operands are not empty

 A z; 
 // if there is frame, create the outer loop values
 I nfro,nfri;  // outer loop counts, and which arg is repeated
 if(((ar-acr)|(wr-wcr))==0){  // normal case
  nfro=nfri=1;  // no outer loops, repeata immaterial
  GA(z,FL>>(it&B01),ndpo*ndpi,wcr-1,AS(w));  // type is INT if inputs booleans, otherwise FL
 }else{
  // There is frame, analyze and check it
  I af=ar-acr; I wf=wr-wcr; I commonf=wf; I *as=AS(a), *ws=AS(w); I *longs=as;
  it|=(ndpo==1)>wf?LIT:0;  // if there is no inner frame for a, and no outer frame for w, signal OK to use 2x2 multiplies.  Mainly this is +/@:*"1/
  commonf=wf>=af?af:commonf; longs=wf>=af?ws:longs; it|=wf>=af?BOX:0;  // repeat flag, length of common frame, pointer to long shape
  af+=wf; af-=2*commonf;  // repurpose af to be length of surplus frame
  ASSERTAGREE(as,ws,commonf)  // verify common frame
  PROD(nfri,af,longs+commonf); PROD(nfro,commonf,longs);   // number of outer loops, number of repeats
  I zn = ndpo*ndpi*nfro; RE(zn=mult(zn,nfri));  // no error possible till we extend the shape
  GA(z,FL>>(it&B01),zn,af+commonf+wcr-1,0); I *zs=AS(z);  // type is INT if inputs booleans, otherwise FL
  // install the shape
  MCISH(zs,longs,af+commonf); MCISH(zs+af+commonf,ws+wr-wcr,wcr-1);
 }

 RZ(jtsumattymesprods(jt,it,voidAV(a),voidAV(w),dplen,nfro,nfri,ndpo,ndpi,voidAV(z)));  // eval, check for error
 RETF(z);
}


static A jtsumattymes(J jt, A a, A w, I b, I t, I m, I n, I nn, I r, I *s, I zn){A z;
 RZ(a&&w);
 switch(CTTZNOFLAG(t)){
 case B01X:  // the aligned cases are handled elsewhere, a word at a time
  {B*av=BAV(a),u,*wv=BAV(w);I*zu,*zv;
   GATV(z,INT,zn,r-1,1+s); zu=AV(z);
   if(1==n){
             zv=zu; DQ(m,                     *zv++ =*av++**wv++;);
    DQ(nn-1, zv=zu; DQ(m,                     *zv+++=*av++**wv++;););
   }else{if(!b){B* tv=av; av=wv; wv=tv;}
             zv=zu; DQ(m, u=*av++;      DQ(n, *zv++ =u**wv++;););
    DQ(nn-1, zv=zu; DQ(m, u=*av++; if(u)DQ(n, *zv+++=u**wv++;) else wv+=n;););
   }
  }
  break;
#if !SY_64
 case INTX:
  {D u,*zu,*zv;I*av=AV(a),*wv=AV(w);
   GATV(z,FL,zn,r-1,1+s); zu=DAV(z);
   if(1==n){
             zv=zu; DQ(m,                        *zv++ =*av++*(D)*wv++;);
    DQ(nn-1, zv=zu; DQ(m,                        *zv+++=*av++*(D)*wv++;););
   }else{if(!b){I *tv=av; av=wv; wv=tv;}
             zv=zu; DQ(m, u=(D)*av++;      DQ(n, *zv++ =u**wv++;););
    DQ(nn-1, zv=zu; DQ(m, u=(D)*av++; if(u)DQ(n, *zv+++=u**wv++;) else wv+=n;););
   }
   RZ(z=icvt(z));
  }
  break;
#endif
 case FLX:   
  {D*av=DAV(a),u,v,*wv=DAV(w),*zu,*zv;
   GATV(z,FL,zn,r-1,1+s); zu=DAV(z);
   NAN0;
   // First, try without testing for 0*_ .  If we hit it, it will raise NAN
   if(1==n){
             zv=zu; DQ(m, u=*av++;            v=*wv++; *zv++ =u*v; );
    DQ(nn-1, zv=zu; DQ(m, u=*av++;            v=*wv++; *zv+++=u*v; ););
   }else{if(!b){zv=av; av=wv; wv=zv;}
             zv=zu; DQ(m, u=*av++; DQ(n, v=*wv++; *zv++ =u*v;););
    DQ(nn-1, zv=zu; DQ(m, u=*av++; DQ(n, v=*wv++; *zv+++=u*v;)););
   }
   if(NANTEST){av-=m*nn;wv-=m*nn*n; // try again, testing for 0*_
    NAN0;
    if(1==n){
              zv=zu; DQ(m, u=*av++;            v=*wv++; *zv++ =u&&v?dmul2(u,v):0;  );
     DQ(nn-1, zv=zu; DQ(m, u=*av++;            v=*wv++; *zv+++=u&&v?dmul2(u,v):0;  ););
    }else{   // don't swap again
              zv=zu; DQ(m, u=*av++;      DQ(n, v=*wv++; *zv++ =u&&v?dmul2(u,v):0;););
     DQ(nn-1, zv=zu; DQ(m, u=*av++; if(u)DQ(n, v=*wv++; *zv+++=   v?dmul2(u,v):0;) else wv+=n;););
    }
    NAN1;
   }
  }
 }
 RETF(z);
}    /* a +/@:* w for non-scalar a and w */

static C sumbf[]={CSTARDOT,CMIN,CSTAR,CPLUSDOT,CMAX,CEQ,CNE,CSTARCO,CPLUSCO,CLT,CLE,CGT,CGE};  // verbs that support +/@:g

#define SUMBFLOOPW(BF)     \
 {DO(q, memset(tv,C0,p); DO(255, DO(dw,tv[i]+=BF(*u,*v); ++u; ++v;);); DO(zn,zv[i]+=tu[i];));  \
        memset(tv,C0,p); DO(r,   DO(dw,tv[i]+=BF(*u,*v); ++u; ++v;);); DO(zn,zv[i]+=tu[i];) ;  \
 }
#define SUMBFLOOPX(BF)     \
 {DO(q, memset(tv,C0,p); DO(255, DO(dw,tv[i]+=BF(*u,*v); ++u; ++v;);                           \
                               av+=zn; u=(UI*)av; wv+=zn; v=(UI*)wv;); DO(zn,zv[i]+=tu[i];));  \
        memset(tv,C0,p); DO(r,   DO(dw,tv[i]+=BF(*u,*v); ++u; ++v;);                           \
                               av+=zn; u=(UI*)av; wv+=zn; v=(UI*)wv;); DO(zn,zv[i]+=tu[i];) ;  \
 }
#if SY_ALIGN
#define SUMBFLOOP(BF)   SUMBFLOOPW(BF)
#else
#define SUMBFLOOP(BF)   if(zn%SZI)SUMBFLOOPX(BF) else SUMBFLOOPW(BF)
#endif

static A jtsumatgbool(J jt,A a,A w,C id){A t,z;B* RESTRICTI av,* RESTRICTI wv;I dw,n,p,q,r,*s,zn,* RESTRICT zv;UC* RESTRICT tu;UI* RESTRICTI tv,*u,*v;
 RZ(a&&w);
 s=AS(w); n=*s;
 zn=AN(w)/n; dw=(zn+SZI-1)>>LGSZI; p=dw*SZI;
 q=n/255; r=n%255;
 GATV(z,INT,zn,AR(w)-1,1+s); zv=AV(z); memset(zv,C0,zn*SZI);
 GATV0(t,INT,dw,1); tu=UAV(t); tv=(UI*)tu;
 av=BAV(a); u=(UI*)av; 
 wv=BAV(w); v=(UI*)wv;
 switch(id){
  case CMIN: 
  case CSTAR: 
  case CSTARDOT: SUMBFLOOP(AND ); break;
  case CMAX:
  case CPLUSDOT: SUMBFLOOP(OR  ); break;
  case CEQ:      SUMBFLOOP(EQ  ); break;
  case CNE:      SUMBFLOOP(NE  ); break;
  case CSTARCO:  SUMBFLOOP(NAND); break;
  case CPLUSCO:  SUMBFLOOP(NOR ); break;
  case CLT:      SUMBFLOOP(LT  ); break;
  case CLE:      SUMBFLOOP(LE  ); break;
  case CGT:      SUMBFLOOP(GT  ); break;
  case CGE:      SUMBFLOOP(GE  ); break;
 }
 RETF(z);
}    /* a +/@:g w  for boolean a,w where a-:&(* /@$)w; see also plusinsB */

DF2(jtfslashatg){A fs,gs,y,z;B b,bb,sb=0;C*av,c,d,*wv;I ak,an,ar,*as,at,m,
     n,nn,r,rs,*s,t,wk,wn,wr,*ws,wt,yt,zn,zt;VA2 adocv,adocvf;
 RZ(a&&w&&self);
 an=AN(a); ar=AR(a); as=AS(a); at=an?AT(a):B01;
 wn=AN(w); wr=AR(w); ws=AS(w); wt=wn?AT(w):B01;
 b=ar<=wr; r=b?wr:ar; rs=b?ar:wr; s=b?ws:as; nn=r?s[0]:1;  // b='w has higher rank'; r=higher rank rs=lower rank s->longer shape  nn=#items in longer-shape arg
 ASSERTAGREE(as,ws,MIN(ar,wr));
 {I isfork=CFORK==FAV(self)->id; fs=FAV(self)->fgh[0+isfork]; gs=FAV(self)->fgh[1+isfork];}   // b=0 if @:, 1 if fork; take fs,gs accordingly
 if(SPARSE&(at|wt)||!an||!wn||2>nn){ R df1(df2(a,w,gs),fs);}  // if sparse or empty, or just 1 item, do it the old-fashioned way
 rs=MAX(1,rs); PROD(m,rs-1,s+1); PROD(n,r-rs,s+rs); zn=m*n;   // zn=#atoms in _1-cell of longer arg = #atoms in result; m=#atoms in _1-cell of shorter arg  n=#times to repeat shorter arg  (*/ surplus longer shape)
   // if the short-frame arg is an atom, move its rank to 1 so we get the lengths of the _1-cells of the replicated arguments
// obsolete  if(CFORK==sv->id){fs=sv->fgh[1]; gs=sv->fgh[2];}else{fs=sv->fgh[0]; gs=sv->fgh[1];}
 y=FAV(fs)->fgh[0]; c=ID(y); d=ID(gs);
 if(c==CPLUS){
  // +/@:g is special if args are boolean, length is integral number of I, and g is boolean or *
  if((((at&wt&(n==1))>(zn&(SZI-1)))||!SY_ALIGN)&&strchr(sumbf,d))R sumatgbool(a,w,d);  // kludge search is slow   relies on B01==1
  if(d==CSTAR){
   if(!ar||!wr){  // if either argument is atomic, apply the distributive property to save multiplies
    z=!ar?tymes(a,df1(w,fs)):tymes(w,df1(a,fs));
    if(jt->jerr==EVNAN)RESETERR else R z;
   }else if(TYPESEQ(at,wt)&&at&B01+FL+(INT*!SY_64))R jtsumattymes(jt,a,w,b,at,m,n,nn,r,s,zn);  // +/@:*
  }
 }
 adocv=var(gs,at,wt); ASSERT(adocv.f,EVDOMAIN); yt=rtype(adocv.cv ); t=atype(adocv.cv);
 adocvf=var(y,yt,yt); ASSERT(adocvf.f,EVDOMAIN); zt=rtype(adocvf.cv);
 sb=yt&(c==CPLUS);  // +/@:g where g produces Boolean.
 if(!(sb||TYPESEQ(yt,zt)))R df1(df2(a,w,gs),fs);
 if(t){
  bb=1&&t&XNUM;
  if(TYPESNE(t,at))RZ(a=bb?xcvt((adocv.cv&VXCVTYPEMSK)>>VXCVTYPEX,a):cvt(t,a));
  if(TYPESNE(t,wt))RZ(w=bb?xcvt((adocv.cv&VXCVTYPEMSK)>>VXCVTYPEX,w):cvt(t,w));
 }
 ak=b?m:zn; wk=b?zn:m; ak=an<nn?0:ak; wk=wn<nn?0:wk; ak<<=bplg(AT(a));wk<<=bplg(AT(w));
 GA(y,yt,zn,1,0);  // allocate one item for result of g
 GA(z,zt,zn,r-1,1+s);  // allocate main output area for final result from f/
 n^=-b; n=(n==~1)?1:n;  // encode b flag in sign of n
 if(sb){A t;I j,tn,*zv;UC*tc;UI*ti,*yv;  /* +/@:g for boolean-valued g */
  av=CAV(a); wv=CAV(w); yv=(UI*)AV(y); zv=AV(z); memset(zv,C0,zn*SZI);
  tn=(zn+SZI-1)>>LGSZI; GATV0(t,INT,tn,1); tc=UAV(t); ti=(UI*)tc;
  // Run g in batches of up to 255, accumulating the result bytewise.  NOTE: there may be garbage at the end of yv, but because
  // we are supporting littleendian only, it will not affect the result
  for(j=nn;0<j;j-=255/C_LE){
   memset(ti,C0,tn*SZI); 
   DO(MIN(j,255), ((AHDR2FN*)adocv.f)(n,m,av,wv,yv,jt); av+=ak; wv+=wk; DO(tn,ti[i]+=yv[i];););
   DO(zn, zv[i]+=tc[i];);
  }
 }else{A z1;B p=0;C*yv,*zu,*zv;  // general f/@:g.  Do not run g on entire y; instead run one cell at a time
  av=CAV(a)+ak*(nn-1); wv=CAV(w)+wk*(nn-1); yv=CAV(y); zv=CAV(z);
  GA(z1,zt,zn,r-1,1+s); zu=CAV(z1);  // allocate ping-pong output area for f/
  ((AHDR2FN*)adocv.f)(n,m,av,wv,zv,jt);  // create first result-cell of g
  DQ(nn-1, av-=ak; wv-=wk; ((AHDR2FN*)adocv.f)(n,m,av,wv,yv,jt); ((AHDR2FN*)adocvf.f)((I)1,zn,yv,p?zu:zv,p?zv:zu,jt); p^=1;);  // p==1 means result goes to ping buffer zv
  if(NEVM<jt->jerr){jt->jerr=0; z=df1(df2(a,w,gs),fs);}else z=p?z1:z;  // if overflow, revert to old-fashioned way.  If p points to ping, prev result went to pong, make pong the result
 }
 RE(0); RETF(z);
}    /* a f/@:g w where f and g are atomic*/

// Consolidated entry point for ATOMIC2 verbs.  These can be called with self pointing either to a rank block or to the block for
// the atomic.  If the block is a rank block, we will switch self over to the block for the atomic.
// Rank can be passed in via jt->ranks, or in the rank for self.  jt->ranks has priority.
// This entry point supports inplacing
DF2(jtatomic2){A z;
 A realself=FAV(self)->fgh[0];  // if rank operator, this is nonzero and points to the left arg of rank
 RANK2T selfranks=FAV(self)->lrr;  // get left & right rank from rank/primitive
 self=realself?realself:self;  // if this is a rank block, move to the primitive.  u b. or any atomic primitive has f clear
 RZ(a&&w);
 F2PREFIP;
 RANK2T jtranks=jt->ranks;  // fetch IRS ranks if any
 UI ar=AR(a), wr=AR(w), awr=(ar<<RANKTX)+wr; I awm1=(AN(a)-1)|(AN(w)-1);
 selfranks=jtranks==(RANK2T)~0?selfranks:jtranks;
 // check for singletons
 if(!(awm1|((AT(a)|AT(w))&(NOUN&UNSAFE(~(B01+INT+FL)))))){
  z=jtssingleton(jtinplace,a,w,self,(RANK2T)awr,selfranks);
  if(z||jt->jerr<=NEVM)RETF(z);  // normal return, or non-retryable error
  // if retryable error, fall through.  The retry will not be through the singleton code
 }
 // while it's convenient, check for empty result
 jtinplace=(J)((I)jtinplace+((((UI)awm1>>(BW-1)))<<JTEMPTYX));
 // find frame
 I af=(I)(ar-((UI)selfranks>>RANKTX)); af=af<0?0:af;  // framelen of a
 I wf=(I)(wr-((UI)selfranks&RANKTMSK)); wf=wf<0?0:wf;  // framelen of w
 // if there is no frame wrt rank, shift down to working on frame wrt 0.  Set selfranks=0 to signal that case.  It uses simpler setup
 selfranks=af+wf==0?0:selfranks; af=af+wf==0?ar:af; wf=selfranks==0?wr:wf;  // the conditions had to be like this to prevent a jmp
 af=af<wf?af:wf;   // now af is short frame
 ASSERTAGREE(AS(a),AS(w),af);  // outermost (or only) agreement check
 // Run the full dyad, retrying if a retryable error is returned
 while(1){  // run until we get no error
  z=jtva2(jtinplace,a,w,self,selfranks,(RANK2T)awr);  // execute the verb
  if(z||jt->jerr<=NEVM)RETF(z);   // return if no error or error not retryable
  jtinplace=(J)((I)jtinplace|JTRETRY);  // indicate that we are retrying the operation
 }
}

DF2(jtexpn2  ){F2PREFIP; RZ(a&&w); if(((((I)AR(w)-1)&(AT(w)<<(BW-1-FLX)))<0)&&0.5==*DAV(w))R sqroot(a);  R jtatomic2(jtinplace,a,w,self);}  // use sqrt hardware for sqrt.  Only for atomic w. 
DF2(jtresidue){F2PREFIP; RZ(a&&w); I intmod; if(!((AT(a)|AT(w))&(NOUN&~INT)|AR(a))&&(intmod=IAV(a)[0], (intmod&-intmod)+(intmod<=0)==0))R intmod2(w,intmod); R jtatomic2(jtinplace,a,w,self);}


// These are the unary ops that are implemented using a canned argument
// NOTE that they pass through inplaceability

// Shift the w-is-inplaceable flag to a.  Bit 1 is known to be 0 in any call to a monad
#define IPSHIFTWA (jt = (J)(intptr_t)(((I)jt+JTINPLACEW)&-JTINPLACEA))

// We use the right type of singleton so that we engage AVX loops
#define SETCONPTR A* conptr=num; conptr=AT(w)&INT?zeroionei:conptr; conptr=AT(w)&FL?numvr:conptr;  // for 0 or 1 only
#define SETCONPTR2 A* conptr=num; conptr=AT(w)&FL?numvr:conptr;   // used for 2, when the only options are INT/FL

F1(jtnot   ){RZ(w); SETCONPTR R AT(w)&B01+SB01?eq(num[0],w):minus(conptr[1],w);}
F1(jtnegate){RZ(w); SETCONPTR R minus(conptr[0],w);}
F1(jtdecrem){RZ(w); SETCONPTR IPSHIFTWA; R minus(w,conptr[1]);}
F1(jtincrem){RZ(w); SETCONPTR R plus(conptr[1],w);}
F1(jtduble ){RZ(w); SETCONPTR2 R tymes(conptr[2],w);}
F1(jtsquare){RZ(w); R tymes(w,w);}   // leave inplaceable in w only  ?? never inplaces
F1(jtrecip ){RZ(w); SETCONPTR R divide(conptr[1],w);}
F1(jthalve ){RZ(w); if(!(AT(w)&XNUM+RAT))R tymes(onehalf,w); IPSHIFTWA; R divide(w,num[2]);} 

static AHDR2(zeroF,B,void,void){memset(z,C0,m*(n^(n>>(BW-1))));}
static AHDR2(oneF,B,void,void){memset(z,C1,m*(n^(n>>(BW-1))));}

// table of routines to handle = ~:
static VF eqnetbl[2][16] = {
//    11        12        14        BX        21        22        24       x       41        42        44        x      x      x      SB      INHOMO  // char len of aw, or HOMO SB BX
{ (VF)eqCC, (VF)eqCS, (VF)eqCU, (VF)eqAA, (VF)eqSC, (VF)eqSS, (VF)eqSU, (VF)0, (VF)eqUC, (VF)eqUS, (VF)eqUU, (VF)0, (VF)0, (VF)0, (VF)eqII, (VF)zeroF },
{ (VF)neCC, (VF)neCS, (VF)neCU, (VF)neAA, (VF)neSC, (VF)neSS, (VF)neSU, (VF)0, (VF)neUC, (VF)neUS, (VF)neUU, (VF)0, (VF)0, (VF)0, (VF)neII, (VF)oneF },
};

// Analyze the verb and arguments and come up with *ado, address of the routine to handle one
// list of arguments producing a list of results; and *cv, the conversion control which specifies
// the precision inputs must be converted to, and what the result type will be.
// The flags in cv have doubled letters (e.g. VDD) for input precision, single letters (e. g. VD) for result
// result is a VA2 struct  containing ado and cv.  If failure, ado is 0 and the caller should signal domain error

VA2 jtvar(J jt,A self,I at,I wt){I t;
 // If there is a pending error, it might be one that can be cured with a retry; for example, fixed-point
 // overflow, where we will convert to float.  If the error is one of those, get the routine and conversion
 // for it, and return.
 if(!jt->jerr){
  // Normal case where we are not retrying: here for numeric arguments
  // vaptr converts the character pseudocode into an entry in va;
  // that entry contains 34 (ado,cv) pairs, indexed according to verb/argument types.
  // the first 9 entries [0-8] are a 3x3 array of the combinations of the main numeric types
  // B,I,D; then [9] CMPX [10] XINT (but not RAT) [11] RAT [12] SBT (symbol)
  // then [13-19] are for verb/, with precisions B I D Z X Q Symb
  // [20-26] for verb\, and [27-33] for verb\.
  VA *vainfo=(VA*)FAV(self)->localuse.lvp[0];  // extract table line from the primitive
  if(!((t=UNSAFE(at|wt))&(NOUN&~(B01|INT|FL)))){
   // Here for the fast and important case, where the arguments are both B01/INT/FL
   // The index into va is atype*3 + wtype, calculated sneakily
   jt->mulofloloc = 0;  // Reinit multiplier-overflow count, in case we hit overflow
   R vainfo->p2[(UNSAFE(at)>>(INTX-1))+((UNSAFE(at)+UNSAFE(wt))>>INTX)];
  }else if(!(t&(NOUN&~NUMERIC))) {
   // Here one of the arguments is CMPX/RAT/XNUM  (we don't support XD and XZ yet)
   // They are in priority order CMPX, FL, RAT, XNUM.  Extract those bits and look up
   // the type to use
   I  prix=(xnumpri>>(((t&(FL+CMPX))>>(FLX-2))+((t&RAT)>>(RATX-4))))&15; // routine index, FL/CMPX/XNUM/RAT   bits: RAT CMPX FL
   VA2 selva2 = vainfo->p2[prix];  // 
   // Entries specify no input conversion in the (DD,DD) slot, if they can accept FL arguments directly.  But if we select the FL line,
   // one input is FL and the other must be RAT or XNUM, 
   // we'd better specify an input conversion of VDD, unless the verb is one like +. or bitwise that forces conversion to integer/boolean
   if((prix==8)&&!(selva2.cv&(VBB|VII|VDD|VZZ))){selva2.cv = (selva2.cv&(~VARGMSK))|VDD;}   // This is part of where XNUM/RAT is promoted to FL
   R selva2;
  }else{
   // Normal case, but something is nonnumeric.  This will be a domain error except for = and ~:, and a few symbol operations
   VA2 retva2;  retva2.cv=VB; // where we build the return value   cv indicates no input conversion, boolean result
   I opx=(UC)FAV(self)->id==CNE; if(opx|((UC)FAV(self)->id==CEQ)){I opcode;
    // = or ~:, possibly inhomogeneous
    if(HOMO(at,wt)){
     opcode=((at>>(C2TX-2))+(wt>>C2TX))|(3*(5&(((t>>(SBTX-(BOXX+2)))+t)>>BOXX))); // bits are a4 a2 w4 w2 if char, 1100 if symbol, 0011 if box.  symbol is 1110, coming from a and symb shift
    }else opcode=15;  // inhomogeneous line
    retva2.f=eqnetbl[opx][opcode];  // return the comparison
    R retva2;
   }
   // not = ~:, better be a symbol
   if(at&wt&SBT)R vainfo->p2[12];  // symbol on symbol - process it through the optbl
   retva2.f=0; R retva2;  // if not symbol, return not found
  }
 }else{VA2 retva2;
  // Here there was an error in a previous run.  We see if we have a way to retry the operation
  retva2.f=0;  // error if not filled in
  switch((UC)FAV(self)->id){
  case CCIRCLE: if(jt->jerr==EWIMAG){retva2.f=(VF)cirZZ; retva2.cv=VZ+VZZ+VRD;} break;
  case CEXP: if(jt->jerr==EWIMAG){retva2.f=(VF)powZZ; retva2.cv=VZ+VZZ+VRD;}
             else if(jt->jerr==EWRAT){retva2.f=(VF)powQQ; retva2.cv=VQ+VQQ;}
             else if(jt->jerr==EWIRR){retva2.f=(VF)powDD; retva2.cv=VD+VDD;} break;
  case CBANG: if(jt->jerr==EWIRR){retva2.f=(VF)binDD; retva2.cv=VD+VDD;} break;
  case CDIV: if(jt->jerr==EWRAT){retva2.f=(VF)divQQ; retva2.cv=VQ+VQQ;}
             else if(jt->jerr==EWDIV0){retva2.f=(VF)divDD; retva2.cv=VD+VDD;} break;
  case CPLUS: if(jt->jerr==EWOVIP+EWOVIPPLUSII||jt->jerr==EWOVIP+EWOVIPPLUSBI||jt->jerr==EWOVIP+EWOVIPPLUSIB){retva2.f=(VF)plusIO; retva2.cv=VD+VII;} break;
  case CMINUS: if(jt->jerr==EWOVIP+EWOVIPMINUSII||jt->jerr==EWOVIP+EWOVIPMINUSBI||jt->jerr==EWOVIP+EWOVIPMINUSIB){retva2.f=(VF)minusIO; retva2.cv=VD+VII;} break;
  case CSTAR: if(jt->jerr==EWOVIP+EWOVIPMULII){retva2.f=(VF)tymesIO; retva2.cv=VD+VII;} break;
  case CPLUSDOT: if(jt->jerr==EWOV){retva2.f=(VF)gcdIO; retva2.cv=VD+VII;} break;
  case CSTARDOT: if(jt->jerr==EWOV){retva2.f=(VF)lcmIO; retva2.cv=VD+VII;} break;
  case CSTILE: if(jt->jerr==EWOV){retva2.f=(VF)remDD; retva2.cv=VD+VDD+VIP;} break;
  }
  if(retva2.f){RESETERR}else{if(jt->jerr>NEVM){RESETERR jsignal(EVSYSTEM);}}  // system error if unhandled exception.  Otherwise reset error only if we handled it
  R retva2;
 }
}    /* function and control for rank */
