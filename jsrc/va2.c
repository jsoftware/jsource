/* Copyright 1990-2011, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Atomic (Scalar) Dyadic                                           */

#include "j.h"
#include "ve.h"


static VA va[]={
/*       */ {
 {{0,0}, {0,0}, {0,0},                                /* BB BI BD              */
  {0,0}, {0,0}, {0,0},                                /* IB II ID              */
  {0,0}, {0,0}, {0,0},                                /* DB DI DD              */
  {0,0}, {0,0}, {0,0}, {0,0}},                        /* ZZ XX QQ Symb         */
 {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},   /* ins: B I D Z X Q Symb */
 {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}},   /* pfx: B I D Z X Q Symb */
 {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}} }, /* sfx: B I D Z X Q Symb */

/* 10    */ {
 {{bw0000II,  VI+VII}, {bw0000II,  VI+VII}, {bw0000II,  VI+VII}, 
  {bw0000II,  VI+VII}, {bw0000II,  VI},     {bw0000II,  VI+VII},
  {bw0000II,  VI+VII}, {bw0000II,  VI+VII}, {bw0000II,  VI+VII},
  {bw0000II,  VI+VII}, {bw0000II,  VI+VII}, {bw0000II,  VI+VII}, {0,0}}, 
 {{bw0000insI,VI+VII}, {bw0000insI,VI}},
 {{bw0000pfxI,VI+VII}, {bw0000pfxI,VI}},
 {{bw0000sfxI,VI+VII}, {bw0000sfxI,VI}} },

/* 11    */ {
 {{bw0001II,  VI+VII}, {bw0001II,  VI+VII}, {bw0001II,  VI+VII}, 
  {bw0001II,  VI+VII}, {bw0001II,  VI},     {bw0001II,  VI+VII},
  {bw0001II,  VI+VII}, {bw0001II,  VI+VII}, {bw0001II,  VI+VII},
  {bw0001II,  VI+VII}, {bw0001II,  VI+VII}, {bw0001II,  VI+VII}, {0,0}}, 
 {{bw0001insI,VI+VII}, {bw0001insI,VI}},
 {{bw0001pfxI,VI+VII}, {bw0001pfxI,VI}},
 {{bw0001sfxI,VI+VII}, {bw0001sfxI,VI}} },

/* 12    */ {
 {{bw0010II,  VI+VII}, {bw0010II,  VI+VII}, {bw0010II,  VI+VII}, 
  {bw0010II,  VI+VII}, {bw0010II,  VI},     {bw0010II,  VI+VII},
  {bw0010II,  VI+VII}, {bw0010II,  VI+VII}, {bw0010II,  VI+VII},
  {bw0010II,  VI+VII}, {bw0010II,  VI+VII}, {bw0010II,  VI+VII}, {0,0}}, 
 {{bw0010insI,VI+VII}, {bw0010insI,VI}},
 {{0,0}},
 {{bw0010sfxI,VI+VII}, {bw0010sfxI,VI}} },

/* 13    */ {
 {{bw0011II,  VI+VII}, {bw0011II,  VI+VII}, {bw0011II,  VI+VII}, 
  {bw0011II,  VI+VII}, {bw0011II,  VI},     {bw0011II,  VI+VII},
  {bw0011II,  VI+VII}, {bw0011II,  VI+VII}, {bw0011II,  VI+VII},
  {bw0011II,  VI+VII}, {bw0011II,  VI+VII}, {bw0011II,  VI+VII}, {0,0}}, 
 {{bw0011insI,VI+VII}, {bw0011insI,VI}},
 {{bw0011pfxI,VI+VII}, {bw0011pfxI,VI}},
 {{bw0011sfxI,VI+VII}, {bw0011sfxI,VI}} },

/* 14    */ {
 {{bw0100II,  VI+VII}, {bw0100II,  VI+VII}, {bw0100II,  VI+VII}, 
  {bw0100II,  VI+VII}, {bw0100II,  VI},     {bw0100II,  VI+VII},
  {bw0100II,  VI+VII}, {bw0100II,  VI+VII}, {bw0100II,  VI+VII},
  {bw0100II,  VI+VII}, {bw0100II,  VI+VII}, {bw0100II,  VI+VII}, {0,0}}, 
 {{bw0100insI,VI+VII}, {bw0100insI,VI}},
 {{0,0}},
 {{bw0100sfxI,VI+VII}, {bw0100sfxI,VI}} },

/* 15    */ {
 {{bw0101II,  VI+VII}, {bw0101II,  VI+VII}, {bw0101II,  VI+VII}, 
  {bw0101II,  VI+VII}, {bw0101II,  VI},     {bw0101II,  VI+VII},
  {bw0101II,  VI+VII}, {bw0101II,  VI+VII}, {bw0101II,  VI+VII},
  {bw0101II,  VI+VII}, {bw0101II,  VI+VII}, {bw0101II,  VI+VII}, {0,0}}, 
 {{bw0101insI,VI+VII}, {bw0101insI,VI}},
 {{bw0101pfxI,VI+VII}, {bw0101pfxI,VI}},
 {{bw0101sfxI,VI+VII}, {bw0101sfxI,VI}} },

/* 16    */ {
 {{bw0110II,  VI+VII}, {bw0110II,  VI+VII}, {bw0110II,  VI+VII}, 
  {bw0110II,  VI+VII}, {bw0110II,  VI},     {bw0110II,  VI+VII},
  {bw0110II,  VI+VII}, {bw0110II,  VI+VII}, {bw0110II,  VI+VII},
  {bw0110II,  VI+VII}, {bw0110II,  VI+VII}, {bw0110II,  VI+VII}, {0,0}}, 
 {{bw0110insI,VI+VII}, {bw0110insI,VI}},
 {{bw0110pfxI,VI+VII}, {bw0110pfxI,VI}},
 {{bw0110sfxI,VI+VII}, {bw0110sfxI,VI}} },

/* 17    */ {
 {{bw0111II,  VI+VII}, {bw0111II,  VI+VII}, {bw0111II,  VI+VII}, 
  {bw0111II,  VI+VII}, {bw0111II,  VI},     {bw0111II,  VI+VII},
  {bw0111II,  VI+VII}, {bw0111II,  VI+VII}, {bw0111II,  VI+VII},
  {bw0111II,  VI+VII}, {bw0111II,  VI+VII}, {bw0111II,  VI+VII}, {0,0}}, 
 {{bw0111insI,VI+VII}, {bw0111insI,VI}},
 {{bw0111pfxI,VI+VII}, {bw0111pfxI,VI}},
 {{bw0111sfxI,VI+VII}, {bw0111sfxI,VI}} },

/* 18    */ {
 {{bw1000II,  VI+VII}, {bw1000II,  VI+VII}, {bw1000II,  VI+VII}, 
  {bw1000II,  VI+VII}, {bw1000II,  VI},     {bw1000II,  VI+VII},
  {bw1000II,  VI+VII}, {bw1000II,  VI+VII}, {bw1000II,  VI+VII},
  {bw1000II,  VI+VII}, {bw1000II,  VI+VII}, {bw1000II,  VI+VII}, {0,0}}, 
 {{bw1000insI,VI+VII}, {bw1000insI,VI}},
 {{0,0}},
 {{bw1000sfxI,VI+VII}, {bw1000sfxI,VI}} },

/* 19    */ {
 {{bw1001II,  VI+VII}, {bw1001II,  VI+VII}, {bw1001II,  VI+VII}, 
  {bw1001II,  VI+VII}, {bw1001II,  VI},     {bw1001II,  VI+VII},
  {bw1001II,  VI+VII}, {bw1001II,  VI+VII}, {bw1001II,  VI+VII},
  {bw1001II,  VI+VII}, {bw1001II,  VI+VII}, {bw1001II,  VI+VII}, {0,0}}, 
 {{bw1001insI,VI+VII}, {bw1001insI,VI}},
 {{bw1001pfxI,VI+VII}, {bw1001pfxI,VI}},
 {{bw1001sfxI,VI+VII}, {bw1001sfxI,VI}} },

/* 1a    */ {
 {{bw1010II,  VI+VII}, {bw1010II,  VI+VII}, {bw1010II,  VI+VII}, 
  {bw1010II,  VI+VII}, {bw1010II,  VI},     {bw1010II,  VI+VII},
  {bw1010II,  VI+VII}, {bw1010II,  VI+VII}, {bw1010II,  VI+VII},
  {bw1010II,  VI+VII}, {bw1010II,  VI+VII}, {bw1010II,  VI+VII}, {0,0}}, 
 {{bw1010insI,VI+VII}, {bw1010insI,VI}},
 {{0,0}},
 {{bw1010sfxI,VI+VII}, {bw1010sfxI,VI}} },

/* 1b    */ {
 {{bw1011II,  VI+VII}, {bw1011II,  VI+VII}, {bw1011II,  VI+VII}, 
  {bw1011II,  VI+VII}, {bw1011II,  VI},     {bw1011II,  VI+VII},
  {bw1011II,  VI+VII}, {bw1011II,  VI+VII}, {bw1011II,  VI+VII},
  {bw1011II,  VI+VII}, {bw1011II,  VI+VII}, {bw1011II,  VI+VII}, {0,0}}, 
 {{bw1011insI,VI+VII}, {bw1011insI,VI}},
 {{0,0}},
 {{bw1011sfxI,VI+VII}, {bw1011sfxI,VI}} },

/* 1c    */ {
 {{bw1100II,  VI+VII}, {bw1100II,  VI+VII}, {bw1100II,  VI+VII}, 
  {bw1100II,  VI+VII}, {bw1100II,  VI},     {bw1100II,  VI+VII},
  {bw1100II,  VI+VII}, {bw1100II,  VI+VII}, {bw1100II,  VI+VII},
  {bw1100II,  VI+VII}, {bw1100II,  VI+VII}, {bw1100II,  VI+VII}, {0,0}}, 
 {{bw1100insI,VI+VII}, {bw1100insI,VI}},
 {{0,0}},
 {{bw1100sfxI,VI+VII}, {bw1100sfxI,VI}} },

/* 1d    */ {
 {{bw1101II,  VI+VII}, {bw1101II,  VI+VII}, {bw1101II,  VI+VII}, 
  {bw1101II,  VI+VII}, {bw1101II,  VI},     {bw1101II,  VI+VII},
  {bw1101II,  VI+VII}, {bw1101II,  VI+VII}, {bw1101II,  VI+VII},
  {bw1101II,  VI+VII}, {bw1101II,  VI+VII}, {bw1101II,  VI+VII}, {0,0}}, 
 {{bw1101insI,VI+VII}, {bw1101insI,VI}},
 {{0,0}},
 {{bw1101sfxI,VI+VII}, {bw1101sfxI,VI}} },

/* 1e    */ {
 {{bw1110II,  VI+VII}, {bw1110II,  VI+VII}, {bw1110II,  VI+VII}, 
  {bw1110II,  VI+VII}, {bw1110II,  VI},     {bw1110II,  VI+VII},
  {bw1110II,  VI+VII}, {bw1110II,  VI+VII}, {bw1110II,  VI+VII},
  {bw1110II,  VI+VII}, {bw1110II,  VI+VII}, {bw1110II,  VI+VII}, {0,0}}, 
 {{bw1110insI,VI+VII}, {bw1110insI,VI}},
 {{0,0}},
 {{bw1110sfxI,VI+VII}, {bw1110sfxI,VI}} },

/* 1f    */ {
 {{bw1111II,  VI+VII}, {bw1111II,  VI+VII}, {bw1111II,  VI+VII}, 
  {bw1111II,  VI+VII}, {bw1111II,  VI},     {bw1111II,  VI+VII},
  {bw1111II,  VI+VII}, {bw1111II,  VI+VII}, {bw1111II,  VI+VII},
  {bw1111II,  VI+VII}, {bw1111II,  VI+VII}, {bw1111II,  VI+VII}, {0,0}}, 
 {{bw1111insI,VI+VII}, {bw1111insI,VI}},
 {{bw1111pfxI,VI+VII}, {bw1111pfxI,VI}},
 {{bw1111sfxI,VI+VII}, {bw1111sfxI,VI}} },

/* 21 !  */ {
 {{leBB, VB        }, {binDD,VD+VDD+VRI}, {binDD,VD+VDD}, 
  {binDD,VD+VDD+VRI}, {binDD,VD+VDD+VRI}, {binDD,VD+VDD}, 
  {binDD,VD+VDD    }, {binDD,VD+VDD    }, {binDD,VD    }, 
  {binZZ,VZ+VZZ}, {binXX,VX+VXX}, {binQQ,VX+VQQ}, {0,0}}, 
 {{leinsB,VB}},
 {{lepfxB,VB}},
 {{lesfxB,VB}} },

/* 25 %  */ {
 {{divBB,VD}, {divBI,VD}, {divBD,VD},
  {divIB,VD}, {divII,VD}, {divID,VD},
  {divDB,VD}, {divDI,VD}, {divDD,VD}, 
  {divZZ,VZ+VZZ}, {divXX,VX+VXX}, {divQQ,VQ+VQQ}, {0,0}},
 {{divinsD,VD+VDD}, {divinsD,VD+VDD}, {divinsD,VD}, {divinsZ,VZ}},
 {{divpfxD,VD+VDD}, {divpfxD,VD+VDD}, {divpfxD,VD}, {divpfxZ,VZ}},
 {{divsfxD,VD+VDD}, {divsfxD,VD+VDD}, {divsfxD,VD}, {divsfxZ,VZ}} },

/* 2a *  */ {
 {{andBB,  VB}, {tymesBI,VI}, {tymesBD,VD},
  {tymesIB,VI}, {tymesII,VI}, {tymesID,VD},
  {tymesDB,VD}, {tymesDI,VD}, {tymesDD,VD}, 
  {tymesZZ,VZ+VZZ}, {tymesXX,VX+VXX}, {tymesQQ,VQ+VQQ}, {0,0}},
 {{andinsB,VB}, {tymesinsI,VI}, {tymesinsD,VD}, {tymesinsZ,VZ}, {0,0},          {0,0},          {0,0}},
 {{andpfxB,VB}, {tymespfxI,VI}, {tymespfxD,VD}, {tymespfxZ,VZ}, {tymespfxX,VX}, {tymespfxQ,VQ}, {0,0}},
 {{andsfxB,VB}, {tymessfxI,VI}, {tymessfxD,VD}, {tymessfxZ,VZ}, {tymessfxX,VX}, {tymessfxQ,VQ}, {0,0}} },

/* 2b +  */ {
 {{plusBB,VI    }, {plusII,VI+VII}, {plusBD,VD}, 
  {plusII,VI+VII}, {plusII,VI    }, {plusID,VD}, 
  {plusDB,VD    }, {plusDI,VD    }, {plusDD,VD}, 
  {plusZZ,VZ+VZZ}, {plusXX,VX+VXX}, {plusQQ,VQ+VQQ}, {0,0}},
 {{plusinsB,VI}, {plusinsI,VI}, {plusinsD,VD}, {plusinsZ,VZ}, {0,0},         {0,0},         {0,0}},
 {{pluspfxB,VI}, {pluspfxI,VI}, {pluspfxD,VD}, {pluspfxZ,VZ}, {pluspfxX,VX}, {pluspfxQ,VQ}, {0,0}},
 {{plussfxB,VI}, {plussfxI,VI}, {plussfxD,VD}, {plussfxZ,VZ}, {plussfxX,VX}, {plussfxQ,VQ}, {0,0}} },

/* 2d -  */ {
 {{minusBB,VI    }, {minusII,VI+VII}, {minusBD,VD}, 
  {minusII,VI+VII}, {minusII,VI    }, {minusID,VD},
  {minusDB,VD    }, {minusDI,VD    }, {minusDD,VD}, 
  {minusZZ,VZ+VZZ}, {minusXX,VX+VXX}, {minusQQ,VQ+VQQ}, {0,0}},
 {{minusinsB,VI}, {minusinsI,VI}, {minusinsD,VD}, {minusinsZ,VZ}, {0,0},          {0,0},          {0,0}},
 {{minuspfxB,VI}, {minuspfxI,VI}, {minuspfxD,VD}, {minuspfxZ,VZ}, {minuspfxX,VX}, {minuspfxQ,VQ}, {0,0}},
 {{minussfxB,VI}, {minussfxI,VI}, {minussfxD,VD}, {minussfxZ,VZ}, {0,0},          {0,0},          {0,0}} },

/* 3c <  */ {
 {{ltBB,VB}, {ltBI,VB}, {ltBD,VB},
  {ltIB,VB}, {ltII,VB}, {ltID,VB},
  {ltDB,VB}, {ltDI,VB}, {ltDD,VB}, 
  {ltDD,VB+VDD}, {ltXX,VB+VXFC}, {ltQQ,VB+VQQ}, {ltSS,VB}},
 {{ltinsB,VB}},
 {{ltpfxB,VB}},
 {{ltsfxB,VB}} },

/* 3d =  */ {
 {{eqBB,VB}, {eqBI,VB}, {eqBD,VB},
  {eqIB,VB}, {eqII,VB}, {eqID,VB},
  {eqDB,VB}, {eqDI,VB}, {eqDD,VB}, 
  {eqZZ,VB+VZZ}, {eqXX,VB+VXEQ}, {eqQQ,VB+VQQ}, {eqII,VB}},
 {{eqinsB,VB}},
 {{eqpfxB,VB}},
 {{eqsfxB,VB}} },

/* 3e >  */ {
 {{gtBB,VB}, {gtBI,VB}, {gtBD,VB},
  {gtIB,VB}, {gtII,VB}, {gtID,VB},
  {gtDB,VB}, {gtDI,VB}, {gtDD,VB}, 
  {gtDD,VB+VDD}, {gtXX,VB+VXCF}, {gtQQ,VB+VQQ}, {gtSS,VB}},
 {{gtinsB,VB}},
 {{gtpfxB,VB}},
 {{gtsfxB,VB}} },

/* 5e ^  */ {
 {{geBB, VB}, {powBI,VD}, {powBD,VD},
  {powIB,VI}, {powII,VD}, {powID,VD},
  {powDB,VD}, {powDI,VD}, {powDD,VD}, 
  {powZZ,VZ+VZZ}, {powXX,VX+VXX}, {powQQ,VQ+VQQ}, {0,0}},
 {{geinsB,VB}},
 {{gepfxB,VB}},
 {{gesfxB,VB}} },

/* 7c |  */ {
 {{ltBB, VB    }, {remII,VI+VII}, {remDD,VD+VDD},
  {remII,VI+VII}, {remII,VI    }, {remID,VI    },
  {remDD,VD+VDD}, {remDD,VD+VDD}, {remDD,VD    }, 
  {remZZ,VZ+VZZ}, {remXX,VX+VXX}, {remQQ,VQ+VQQ}, {0,0}},
 {{ltinsB,VB}},
 {{ltpfxB,VB}},
 {{ltsfxB,VB}} },

/* 82 <. */ {
 {{andBB,VB}, {minBI,VI}, {minBD,VD},
  {minIB,VI}, {minII,VI}, {minID,VD},
  {minDB,VD}, {minDI,VD}, {minDD,VD}, 
  {minDD,VD+VDD}, {minXX,VX+VXX}, {minQQ,VQ+VQQ}, {minSS,VSB}},
 {{andinsB,VB}, {mininsI,VI}, {mininsD,VD}, {mininsD,VD+VDD}, {mininsX,VX}, {0,0},        {mininsS,VSB}},
 {{andpfxB,VB}, {minpfxI,VI}, {minpfxD,VD}, {minpfxD,VD+VDD}, {minpfxX,VX}, {minpfxQ,VQ}, {minpfxS,VSB}},
 {{andsfxB,VB}, {minsfxI,VI}, {minsfxD,VD}, {minsfxD,VD+VDD}, {minsfxX,VX}, {minsfxQ,VQ}, {minsfxS,VSB}}},

/* 83 <: */ {
 {{leBB,VB}, {leBI,VB}, {leBD,VB},
  {leIB,VB}, {leII,VB}, {leID,VB},
  {leDB,VB}, {leDI,VB}, {leDD,VB}, 
  {leDD,VB+VDD}, {leXX,VB+VXCF}, {leQQ,VB+VQQ}, {leSS,VB}},
 {{leinsB,VB}},
 {{lepfxB,VB}},
 {{lesfxB,VB}} },

/* 84 >. */ {
 {{ orBB,VB}, {maxBI,VI}, {maxBD,VD},
  {maxIB,VI}, {maxII,VI}, {maxID,VD},
  {maxDB,VD}, {maxDI,VD}, {maxDD,VD}, 
  {maxDD,VD+VDD}, {maxXX,VX+VXX}, {maxQQ,VQ+VQQ}, {maxSS,VSB}},
 {{orinsB,VB}, {maxinsI,VI}, {maxinsD,VD}, {maxinsD,VD+VDD}, {maxinsX,VX}, {0,0},        {maxinsS,VSB}},
 {{orpfxB,VB}, {maxpfxI,VI}, {maxpfxD,VD}, {maxpfxD,VD+VDD}, {maxpfxX,VX}, {maxpfxQ,VQ}, {maxpfxS,VSB}},
 {{orsfxB,VB}, {maxsfxI,VI}, {maxsfxD,VD}, {maxsfxD,VD+VDD}, {maxsfxX,VX}, {maxsfxQ,VQ}, {maxsfxS,VSB}}},

/* 85 >: */ {
 {{geBB,VB}, {geBI,VB}, {geBD,VB},
  {geIB,VB}, {geII,VB}, {geID,VB},
  {geDB,VB}, {geDI,VB}, {geDD,VB}, 
  {geDD,VB+VDD}, {geXX,VB+VXFC}, {geQQ,VB+VQQ}, {geSS,VB}},
 {{geinsB,VB}},
 {{gepfxB,VB}},
 {{gesfxB,VB}} },

/* 88 +. */ {
 {{ orBB,VB    }, {gcdII,VI+VII}, {gcdDD,VD+VDD},
  {gcdII,VI+VII}, {gcdII,VI    }, {gcdDD,VD+VDD},
  {gcdDD,VD+VDD}, {gcdDD,VD+VDD}, {gcdDD,VD    }, 
  {gcdZZ,VZ+VZZ}, {gcdXX,VX+VXX}, {gcdQQ,VQ+VQQ}, {0,0}},
 {{orinsB,VB}},
 {{orpfxB,VB}},
 {{orsfxB,VB}} },

/* 89 +: */ {
 {{norBB,VB    }, {norBB,VB+VBB}, {norBB,VB+VBB},
  {norBB,VB+VBB}, {norBB,VB+VBB}, {norBB,VB+VBB},
  {norBB,VB+VBB}, {norBB,VB+VBB}, {norBB,VB+VBB}, 
  {norBB,VB+VBB}, {norBB,VB+VBB}, {norBB,VB+VBB}, {0,0}},
 {{norinsB,VB}},
 {{norpfxB,VB}},
 {{norsfxB,VB}} },

/* 8a *. */ {
 {{andBB,VB    }, {lcmII,VI+VII}, {lcmDD,VD+VDD},
  {lcmII,VI+VII}, {lcmII,VI    }, {lcmDD,VD+VDD},
  {lcmDD,VD+VDD}, {lcmDD,VD+VDD}, {lcmDD,VD    }, 
  {lcmZZ,VZ+VZZ}, {lcmXX,VX+VXX}, {lcmQQ,VQ+VQQ}, {0,0}},
 {{andinsB,VB}},
 {{andpfxB,VB}},
 {{andsfxB,VB}} },

/* 8b *: */ {
 {{nandBB,VB},     {nandBB,VB+VBB}, {nandBB,VB+VBB},
  {nandBB,VB+VBB}, {nandBB,VB+VBB}, {nandBB,VB+VBB},
  {nandBB,VB+VBB}, {nandBB,VB+VBB}, {nandBB,VB+VBB}, 
  {nandBB,VB+VBB}, {nandBB,VB+VBB}, {nandBB,VB+VBB}, {0,0}},
 {{nandinsB,VB}},
 {{nandpfxB,VB}},
 {{nandsfxB,VB}} },

/* 95 ~: */ {
 {{neBB,VB}, {neBI,VB}, {neBD,VB},
  {neIB,VB}, {neII,VB}, {neID,VB},
  {neDB,VB}, {neDI,VB}, {neDD,VB}, 
  {neZZ,VB+VZZ}, {neXX,VB+VXEQ}, {neQQ,VB+VQQ}, {0,0}},
 {{neinsB,VB}},
 {{nepfxB,VB}},
 {{nesfxB,VB}} },

/* d1 o. */ {
 {{cirDD,VD+VDD}, {cirDD,VD+VDD}, {cirBD,VD},
  {cirDD,VD+VDD}, {cirDD,VD+VDD}, {cirID,VD},
  {cirDD,VD+VDD}, {cirDD,VD+VDD}, {cirDD,VD}, 
  {cirZZ,VZ+VZZ+VRD}, {cirDD,VD+VDD}, {cirDD,VD+VDD}, {0,0}},
 {{0,0}},
 {{0,0}},
 {{0,0}} }
};


static UC vaptr[256]={
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* 0 */
// C0  C1          ^D                    TAB LF          CR         
   1,  2,  3,  4,  5,  6,  7,  8,    9, 10, 11, 12, 13, 14, 15, 16, /* 1 */
// <-----------------------bitwise functions -------------------->
   0, 17,  0,  0,  0, 18,  0,  0,    0,  0, 19, 20,  0, 21,  0,  0, /* 2 */
//     !   "   #   $   %   &   '     (   )   *   +   ,   -   .   /  
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0, 22, 23, 24,  0, /* 3 */
// NOUN    HOOK FK ADVF                      :   ;   <   =   >   ?  
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* 4 */
// @                                                                
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0, 25,  0, /* 5 */
//                                               [   \   ]   ^   _  
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* 6 */
// `                                                                
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0, 26,  0,  0,  0, /* 7 */
//                                               {   |   }   ~      
   0,  0, 27, 28, 29, 30,  0,  0,   31, 32, 33, 34,  0,  0,  0,  0, /* 8 */
// =.  =:  <.  <:  >.  >:  _.        +.  +:  *.  *:  -.  -:  %.  %: 
   0,  0,  0,  0,  0, 35,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* 9 */
// ^.  ^:  $.  $:  ~.  ~:  |.  |:    ..  .:  :.  ::  ,.  ,:  ;.  ;: 
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* a */
// #.  #:  !.  !:  /.  /:  \.  \:    [.  [:  ].  ]:  {.  {:  }.  }: 
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* b */
// ".  ":  `.  `:  @.  @:  &.  &:    ?.  ?:  a.  A.  a:  b.  c.     
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* c */
// C.  d.  D.  D:  e.  E.  f.  F:    H.  i.  i:  I.  I:  j.  L.  L: 
   0,  0,  0, 36,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* d */
// m.  M.  n.  o.  p.  p:  Q:  q:    r.  s:  S:  t.  t:  T.  u.  u: 
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* e */
// v.  x.  x:  y.                    0:  }ip }cs {:: {:: }:: &.: p..
   0,  0,  0,  0,  0,  0,  0,  0,    0,  0,  0,  0,  0,  0,  0,  0, /* f */
// ,ip                                                           FF  
  };   /* index in va[] for each ID */
/*   1   2   3   4   5   6   7     8   9   a   b   c   d   e   f   */

C jtvaid(J jt,A w){A x;C c;I k;V*v;
 v=VAV(w); c=v->id;
 if(c==CBDOT){x=v->f; if(INT&AT(x)&&!AR(x)&&(k=*AV(x),16<=k&&k<=31))c=(C)k;}
 R vaptr[(UC)c]?c:0;
}

static A jtva2(J,A,A,C);

// These are the entry points for the individual verbs.  They pick up the verb-name
// and transfer to jtva2 which does the work

F2(jtbitwise0000){R va2(a,w,(C)16);}
F2(jtbitwise0001){R va2(a,w,(C)17);}
F2(jtbitwise0010){R va2(a,w,(C)18);}
F2(jtbitwise0011){R va2(a,w,(C)19);}

F2(jtbitwise0100){R va2(a,w,(C)20);}
F2(jtbitwise0101){R va2(a,w,(C)21);}
F2(jtbitwise0110){R va2(a,w,(C)22);}
F2(jtbitwise0111){R va2(a,w,(C)23);}

F2(jtbitwise1000){R va2(a,w,(C)24);}
F2(jtbitwise1001){R va2(a,w,(C)25);}
F2(jtbitwise1010){R va2(a,w,(C)26);}
F2(jtbitwise1011){R va2(a,w,(C)27);}

F2(jtbitwise1100){R va2(a,w,(C)28);}
F2(jtbitwise1101){R va2(a,w,(C)29);}
F2(jtbitwise1110){R va2(a,w,(C)30);}
F2(jtbitwise1111){R va2(a,w,(C)31);}

F2(jteq     ){R va2(a,w,CEQ     );}
F2(jtlt     ){R va2(a,w,CLT     );}
F2(jtminimum){R va2(a,w,CMIN    );}
F2(jtle     ){R va2(a,w,CLE     );}
F2(jtgt     ){R va2(a,w,CGT     );}
F2(jtmaximum){R va2(a,w,CMAX    );}
F2(jtge     ){R va2(a,w,CGE     );}
F2(jtplus   ){R va2(a,w,CPLUS   );}
F2(jtgcd    ){R va2(a,w,CPLUSDOT);}
F2(jtnor    ){R va2(a,w,CPLUSCO );}
F2(jttymes  ){R va2(a,w,CSTAR   );}
F2(jtlcm    ){R va2(a,w,CSTARDOT);}
F2(jtnand   ){R va2(a,w,CSTARCO );}
F2(jtminus  ){R va2(a,w,CMINUS  );}
F2(jtdivide ){R va2(a,w,CDIV    );}
F2(jtexpn2  ){R va2(a,w,CEXP    );}
F2(jtne     ){R va2(a,w,CNE     );}
F2(jtoutof  ){R va2(a,w,CBANG   );}
F2(jtcircle ){R va2(a,w,CCIRCLE );}
F2(jtresidue){RZ(a&&w); R INT&AT(w)&&equ(a,num[2])?intmod2(w):va2(a,w,CSTILE);}

F1(jtnot   ){R w&&AT(w)&B01+SB01?va2(zero,w,CEQ):va2(one,w,CMINUS);}
F1(jtnegate){R va2(zero,  w,     CMINUS);}
F1(jtdecrem){R va2(w,     one,   CMINUS);}
F1(jtincrem){R va2(one,   w,     CPLUS );}
F1(jtduble ){R va2(num[2],w,     CSTAR );}
F1(jtsquare){R va2(w,     w,     CSTAR );}
F1(jtrecip ){R va2(one,   w,     CDIV  );}
F1(jthalve ){R va2(w,     num[2],CDIV  );}

static void zeroF(J jt,B b,I m,I n,B*z,void*x,void*y){memset(z,C0,m*n);}
static void  oneF(J jt,B b,I m,I n,B*z,void*x,void*y){memset(z,C1,m*n);}

A jtcvz(J jt,I cv,A w){I t;
 t=AT(w);
 if(cv&VRD&&t!=FL )R pcvt(FL,w);
 if(cv&VRI&&t!=INT)R icvt(w);
 R w;
}    /* convert result */

I atype(I cv){
 if(!(cv&VBB+VII+VDD+VZZ+VQQ+VXX+VXEQ+VXCF+VXFC))R 0;
 R cv&VBB?B01:cv&VII?INT:cv&VDD?FL:cv&VZZ?CMPX:cv&VQQ?RAT:XNUM;
}    /* argument conversion */
     
I rtype(I cv){R cv&VB?B01:cv&VI?INT:cv&VD?FL:cv&VZ?CMPX:cv&VQ?RAT:cv&VX?XNUM:SBT;}
     /* result type */

     
#define VAF(fname,ptr,fp,fm,ft)   \
 void fname(J jt,C id,I t,VF*ado,I*cv){VA2*p;  \
  if(jt->jerr==EWOV){                          \
   jt->jerr=0;                                 \
   *ado=(id==CPLUS?fp:id==CMINUS?fm:ft);       \
   *cv=VD;                                     \
  }else if(t&NUMERIC+SBT){                     \
   p=(va+vaptr[(UC)id])->ptr+(t&B01?0:t&INT?1:t&FL?2:t&CMPX?3:t&XNUM?4:t&RAT?5:6);  \
   *ado=p->f; *cv=p->cv;                       \
  }else *ado=0;                                \
 }

VAF(jtvains,pins, plusinsO,minusinsO,tymesinsO)
VAF(jtvapfx,ppfx, pluspfxO,minuspfxO,tymespfxO)
VAF(jtvasfx,psfx, plussfxO,minussfxO,tymessfxO)

#define VARCASE(e,c) (70*(e)+(c))

// Analyze the verb and arguments and come up with the *ado, address of the routine to handle one
// list of arguments producing a list of results; and *cv, the conversion control which specifies
// the precision inputs must be converted to, and what the result type will be.
// The flags in cv have doubled letters (e.g. VDD) for input precision, single letters (e. g. VD) for result
// Returned value is 0 for failure, 1 for success
B jtvar(J jt,C id,A a,A w,I at,I wt,VF*ado,I*cv){B b;I j,t,x;VA*q;VA2 p;
 // If there is a pending error, it might be one that can be cured with a retry; for example, fixed-point
 // overflow, where we will convert to float.  If the error is one of those, get the routine and conversion
 // for it, and return.
 if(jt->jerr){
  switch(VARCASE(jt->jerr,id)){
   default:      R 0;
   case VARCASE(EWIMAG,CCIRCLE ): *ado=(VF)cirZZ;   *cv=VZ+VZZ+VRD; break;
   case VARCASE(EWIMAG,CEXP    ): *ado=(VF)powZZ;   *cv=VZ+VZZ+VRD; break;
   case VARCASE(EWIRR ,CBANG   ): *ado=(VF)binDD;   *cv=VD+VDD;     break;
   case VARCASE(EWIRR ,CEXP    ): *ado=(VF)powDD;   *cv=VD+VDD;     break;
   case VARCASE(EWRAT ,CDIV    ): *ado=(VF)divQQ;   *cv=VQ+VQQ;     break;
   case VARCASE(EWRAT ,CEXP    ): *ado=(VF)powQQ;   *cv=VQ+VQQ;     break;
   case VARCASE(EWDIV0,CDIV    ): *ado=(VF)divDD;   *cv=VD+VDD;     break;
   case VARCASE(EWOV  ,CPLUS   ): *ado=(VF)plusIO;  *cv=VD+VII;     break;
   case VARCASE(EWOV  ,CMINUS  ): *ado=(VF)minusIO; *cv=VD+VII;     break;
   case VARCASE(EWOV  ,CSTAR   ): *ado=(VF)tymesIO; *cv=VD+VII;     break;
   case VARCASE(EWOV  ,CPLUSDOT): *ado=(VF)gcdIO;   *cv=VD+VII;     break;
   case VARCASE(EWOV  ,CSTARDOT): *ado=(VF)lcmIO;   *cv=VD+VII;     break;
   case VARCASE(EWOV  ,CSTILE  ): *ado=(VF)remDD;   *cv=VD+VDD;     break;
  }
  RESETERR;
 }else if(at&NUMERIC&&wt&NUMERIC){
  // Normal case where we are not retrying: here for numeric arguments
  t=at|wt; b=1&&t&RAT+XNUM+XD+XZ;  // b = 'indirect precision'
  // vaptr converts the character pseudocode into an entry in va;
  // that entry contains 34 (ado,cv) pairs, indexed according to verb/argument types.
  // the first 9 entries [0-8] are a 3x3 array of the combinations of the main numeric types
  // B,I,D; then [9] CMPLX [10] XINT (but not RAT) [11] RAT [12] SBT (symbol)
  // then [13-19] are for verb/, with precisions B I D Z X Q Symb
  // [20-26] for verb\, and [27-33] for verb\.
  j=t&CMPX ? 9 : b ? (t&XZ?13:t&XD?12:t&FL?8:t&RAT?11:10) : 
      (at&B01?0:at&INT?3:6)+(wt&B01?0:wt&INT?1:2);
  q=va+vaptr[(UC)id];  // get va[] entry for this verb
  p=(q->p2)[j];    // select ado,cv for this precision, copy to result
  *ado=p.f; *cv=x=p.cv; if(b&&t&FL&&!(x&VZZ))*cv+=VDD;  // if one argument is extended precision and the other is float,
  if (b&&t&FL&&(x&VZZ))*(I *)0=0;  // scaf - change VDD fix below ****************************************************************************************************
    // give a float result (unless a lower precision is available, eg for comparisons)
 }else{
  // Normal case, but nonnumeric.  This will be a domain error except for = and ~:, and a few symbol operations
  b=!HOMO(at,wt); *cv=VB;  // b = 'inhoomogeneous types (always ~:)'; cv indicates no input conversion, boolean result
  jt->rela=ARELATIVE(a)*(I)a;   // set flags indicating 'indirect datatype' for use during compare
  jt->relw=ARELATIVE(w)*(I)w;
  switch(id){
    // for =, it's just 0 for inhomogeneous types, or the routines to handle the other comparisons
   case CEQ: *ado=b?(VF)zeroF:at&SBT?(VF)eqII:at&BOX?(VF)eqAA:
                  at&LIT?(wt&LIT?(VF)eqCC:(VF)eqCS):wt&LIT?(VF)eqSC:(VF)eqSS; break;
    // similarly for ~:
   case CNE: *ado=b?(VF) oneF:at&SBT?(VF)neII:at&BOX?(VF)neAA:
                  at&LIT?(wt&LIT?(VF)neCC:(VF)neCS):wt&LIT?(VF)neSC:(VF)neSS; break;
   default:
    // If not = ~:, it has to be a symbol operation.
    ASSERT(at&SBT&&wt&SBT,EVDOMAIN);
    q=va+vaptr[(UC)id]; p=(q->p2)[12];  // fetch the 'symbol' entry
    ASSERT(p.f,EVDOMAIN);   // not all verbs support symbols - fails if this one doesn't
    *ado=p.f; *cv=x=p.cv;  // return the values read
 }}
 R 1;
}    /* function and control for rank */


// All dyadic arithmetic verbs f enter here, and also f"n.  a and w are the arguments, id
// is the pseudocharacter indicating what operation is to be performed
static A jtva2(J jt,A a,A w,C id){A z;B b,c,sp=0;C*av,*wv,*zv;I acn,acr,af,ak,an,ar,*as,at,cv,f,m,
     mf,n,nf,*oq=jt->rank,q,r,*s,*sf,t,wcn,wcr,wf,wk,wn,wr,*ws,wt,zcn,zk,zn,zt;VF ado;
   // oq is used to stack jt->rank in case it is altered by subroutines and we have to retry
 RZ(a&&w);
 an=AN(a); ar=AR(a); as=AS(a); at=AT(a);   // #,x  #$x  address of $x   type of x
 wn=AN(w); wr=AR(w); ws=AS(w); wt=AT(w);   // #,y  #$y  address of $y   type of y
 // If an operand is empty, turn it to Boolean, and if the OTHER operand is non-numeric, turn that to Boolean too (leaving
 //  rank and shape untouched).  This change to the other operand is notional only - we won't actually convert
 // when there is an empty - but it guarantees that execution on n empty never fails.
 // If we switch a sparse nonnumeric matrix to boolean, that may be a space problem; but we don't
 // support nonnumeric sparse now
 if(an==0){at=B01;if(!(wt&NUMERIC))wt=B01;}  // switch empty arg to Boolean & ensure compatibility with other arg
 if(wn==0){wt=B01;if(!(at&NUMERIC))at=B01;}
 // if an operand is sparse, remember that fact and replace its type with the corresponding non-sparse type
 if(SPARSE&at){sp=1; at=DTYPE(at);}
 if(SPARSE&wt){sp=1; wt=DTYPE(wt);}
 // Analyze the rank and calculate cell shapes and counts.  Not byte sizes yet, since there may be conversions
 // We detect agreement error before domain error
 if(jt->rank){
  // Here, a rank was specified.
  r=jt->rank[0]; acr=MIN(ar,r); af=ar-acr; acn=prod(acr,as+af);  // r=left rank of verb, acr=effective rank, af=left frame, acn=left #cells
  r=jt->rank[1]; wcr=MIN(wr,r); wf=wr-wcr; wcn=prod(wcr,ws+wf); // r=right rank of verb, wcr=effective rank, wf=right frame, wcn=left #cells
      // note: the prod above can never fail, because it gives the actual # cells of an existing noun
  // Now that we have used the rank info, clear jt->rank.  All verbs start with jt->rank=0 unless they have "n applied
  // we do this before we generate failures
  jt->rank=0;
  // if the frames don't agree, that's always an agreement error
  ASSERT(!ICMP(as,ws,MIN(af,wf)),EVLENGTH);  // frames must match to the shorter length; agreement error if not
  c=af<=wf; f=c?wf:af; q=c?af:wf; sf=c?ws:as;   // c='right frame is longer'; f=#longer frame; q=#shorter frame; sf->shape of arg with longer frame
  b=acr<=wcr; zcn=b?wcn:acn; m=b?acn:wcn; n=m?zcn/m:0; r=b?wcr:acr; s=b?ws+wf:as+af;   // b='right cell has larger rank'; zcn=#cells in arg with larger rank (=#result cells);
    // m=#cells in arg with shorter rank; n=#times shorter-rank cells must be repeated; r=larger of cell-ranks; s->shape of larger-rank cell
 }else{
  // No rank specified.  Since all these verbs have rank 0, that simplifies quite a bit
  ASSERT(!ICMP(as,ws,MIN(ar,wr)),EVLENGTH);   // agreement error if not prefix match
  af=wf=f=0; acr=ar; wcr=wr; sf=0; mf=nf=1;  // with mf==1, there will be only one call to ado, so most of these names could be left alone except for sparse, and compiler warnings
  b=ar<=wr; zn=b?wn:an; m=b?an:wn; n=m?zn/m:0; r=b?wr:ar; s=b?ws:as;   // treat the entire operands as one big cell; get the rest of the values needed
  c=2;  // flag to indicate 'no rank specified'
 }
 if(id==CEXP&&1==wn&&FL&wt&&0.5==*DAV(w))R sqroot(a);  // Now that we have checked for agreement, switch ^&0.5 to %: to use hardware.  jt->rank is immaterial in that case
// analyze the verb/operands, returning ado, the address of the action routine for the
 // computation, and cv, the flags indicating the types selected for the arguments and the result.
 // Extract zt, the type of the result, and t, the type to use for the arguments
 RZ(var(id,a,w,at,wt,&ado,&cv)); zt=rtype(cv); t=atype(cv);
 // If a single input type has been selected (as it will have been for all verbs except = and ~: which
 // can handle any types); AND if both arguments are non-sparse: convert them to the selected type.
 // Incompatible arguments were detected in var().  If there is an empty operand, skip conversions which
 // might fail because the type in t is incompatible with the actual type in a
 if(t&&!sp&&an&&wn){B xn = !!(t&XNUM);
  // Conversions to XNUM use a routine that pushes/sets/pops jt->mode, which controls the
  // type of conversion to XNUM in use.
  if(t!=at)RZ(a=xn?xcvt(cv&VXEQ?XMEXMT:cv&VXFC?XMFLR:cv&VXCF?XMCEIL:XMEXACT,a):cvt(t,a));
  if(t!=wt)RZ(w=xn?xcvt(cv&VXEQ?XMEXMT:cv&VXCF?XMFLR:cv&VXFC?XMCEIL:XMEXACT,w):cvt(t,w));
 }
 // From here on we have possibly changed the address of a and w, but we are still using shape pointers,
 // rank, type, etc. using the original input block.  That's OK.

 // finish up the computation of sizes.  We have to defer this till after var() because
 // if we are retrying the operation, we may be in error state until var clears it; and prod and mult can fail,
 // so we have to RE when we call them
 if(sp){z=vasp(a,w,id,ado,cv,t,zt,af,acr,wf,wcr,f,r); if(!jt->jerr)R z;}  // handle sparse srrays separately
 else{
   // If rank given, calculate the numbers of loops to do
  if(c!=2) {  // if rank was given initially...
   RE(mf=prod(q,sf)); RE(nf=prod(f-q,q+sf));//else mf=nf=0;  // mf=#cells in common frame, nf=#times shorter-frame cell must be repeated.  Not needed if no cells
   RE(zn=mult(mf,mult(nf,zcn)));  // zn=total # result atoms  (only if non-sparse)
   // if the cell-shapes don't match, that's an agreement error UNLESS the frame contains 0; in that case it counts as
   // 'error executing on the cell of fills' and produces a scalar 0 as the result for that cell, which we handle by changing the result-cell rank to 0
   if(ICMP(as+af,ws+wf,MIN(acr,wcr))){if(zn)ASSERT(0,EVLENGTH)else r = 0;}
   // if looping required, calculate the strides for input & output
   if((mf|nf)>1){zk=zcn*bp(zt); ak=acn*bp(AT(a)); wk=wcn*bp(AT(w));}//else {ak=wk=zk=0;}
   // zk=result-cell size in bytes; ak,wk=left,right arg-cell size in bytes.  Not needed if not looping
  }
  // Not sparse.  Allocate a result area of the right type, and copy in its cell-shape after the frame
  GA(z,zt,zn,f+r,sf); ICPY(f+AS(z),s,r); 
  if(!zn)R z;  // If the result is empty, the allocated area says it all
  av=CAV(a); wv=CAV(w); zv=CAV(z);   // point to the data
  // Call the action routines: 
  if(1==nf) DO(mf,        ado(jt,b,m,n,zv,av,wv); zv+=zk; av+=ak; wv+=wk;)  // if the short cell is not repeated, loop over the frame
  else if(c)DO(mf, DO(nf, ado(jt,b,m,n,zv,av,wv); zv+=zk;         wv+=wk;); av+=ak;)  // if right frame is longer, repeat cells of a
  else      DO(mf, DO(nf, ado(jt,b,m,n,zv,av,wv); zv+=zk; av+=ak;        ); wv+=wk;);  // if left frame is longer, repeat cells of w
  // The work has been done.  If there was no error, check for optional conversion-if-possible
  if(!jt->jerr)R cv&VRI+VRD?cvz(cv,z):z;
 }
 // If we got an internal-only error during execution of the verb, restart to see if it's
 // a recoverable error such an overflow during integer addition.  We have to restore
 // jt->rank, which might have been modified
 R NEVM<jt->jerr?(jt->rank=oq,va2(a,w,id)):0;
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



static DF2(jtsumattymes){A z;B b;I an,ar,*as,at,m,n,nn,r,*s,t,wn,wr,*ws,wt,zn;
 RZ(a&&w&&self);
 an=AN(a); ar=AR(a); as=AS(a); at=an?AT(a):B01;
 wn=AN(w); wr=AR(w); ws=AS(w); wt=wn?AT(w):B01; 
 t=maxtype(at,wt); b=ar<=wr; r=b?wr:ar; s=b?ws:as; nn=*s;
 zn=(b?wn:an)/nn; m=(b?an:wn)/nn; m=m?m:1; n=zn/m;
 switch(t){
  case B01:
   {B*av=BAV(a),u,*wv=BAV(w);I*zu,*zv;
    GA(z,INT,zn,r-1,1+s); zu=AV(z);
    if(1==n){
              zv=zu; DO(m,                     *zv++ =*av++**wv++;);
     DO(nn-1, zv=zu; DO(m,                     *zv+++=*av++**wv++;););
    }else if(b){
              zv=zu; DO(m, u=*av++;      DO(n, *zv++ =u**wv++;););
     DO(nn-1, zv=zu; DO(m, u=*av++; if(u)DO(n, *zv+++=u**wv++;) else wv+=n;););
    }else{
              zv=zu; DO(m, u=*wv++;      DO(n, *zv++ =u**av++;););
     DO(nn-1, zv=zu; DO(m, u=*wv++; if(u)DO(n, *zv+++=u**av++;) else av+=n;););
   }}
   break;
#if !SY_64
  case INT:
   {D u,*zu,*zv;I*av=AV(a),*wv=AV(w);
    GA(z,FL,zn,r-1,1+s); zu=DAV(z);
    if(1==n){
              zv=zu; DO(m,                        *zv++ =*av++*(D)*wv++;);
     DO(nn-1, zv=zu; DO(m,                        *zv+++=*av++*(D)*wv++;););
    }else if(b){
              zv=zu; DO(m, u=(D)*av++;      DO(n, *zv++ =u**wv++;););
     DO(nn-1, zv=zu; DO(m, u=(D)*av++; if(u)DO(n, *zv+++=u**wv++;) else wv+=n;););
    }else{
              zv=zu; DO(m, u=(D)*wv++;      DO(n, *zv++ =u**av++;););
     DO(nn-1, zv=zu; DO(m, u=(D)*wv++; if(u)DO(n, *zv+++=u**av++;) else av+=n;););
    }
    RZ(z=icvt(z));
   }
   break;
#endif
  case FL:   
   {D*av=DAV(a),u,v,*wv=DAV(w),*zu,*zv;
    GA(z,FL,zn,r-1,1+s); zu=DAV(z);
    NAN0;
    if(1==n){
              zv=zu; DO(m, u=*av++;            v=*wv++; *zv++ =u&&v?u*v:0;  );
     DO(nn-1, zv=zu; DO(m, u=*av++;            v=*wv++; *zv+++=u&&v?u*v:0;  ););
    }else if(b){
              zv=zu; DO(m, u=*av++;      DO(n, v=*wv++; *zv++ =u&&v?u*v:0;););
     DO(nn-1, zv=zu; DO(m, u=*av++; if(u)DO(n, v=*wv++; *zv+++=   v?u*v:0;) else wv+=n;););
    }else{
              zv=zu; DO(m, u=*wv++;      DO(n, v=*av++; *zv++ =u&&v?u*v:0;););
     DO(nn-1, zv=zu; DO(m, u=*wv++; if(u)DO(n, v=*av++; *zv+++=   v?u*v:0;) else av+=n;););
    }
    NAN1;
 }}
 R z;
}    /* a +/@:* w for non-scalar a and w */

static C sumbf[]={CSTARDOT,CMIN,CSTAR,CPLUSDOT,CMAX,CEQ,CNE,CSTARCO,CPLUSCO,CLT,CLE,CGT,CGE};

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

static A jtsumatgbool(J jt,A a,A w,C id){A t,z;B*av,*wv;I dw,n,p,q,r,*s,zn,*zv;UC*tu;UI*tv,*u,*v;
 RZ(a&&w);
 s=AS(w); n=*s;
 zn=AN(w)/n; dw=(zn+SZI-1)/SZI; p=dw*SZI;
 q=n/255; r=n%255;
 GA(z,INT,zn,AR(w)-1,1+s); zv=AV(z); memset(zv,C0,zn*SZI);
 GA(t,INT,dw,1,0); tu=(UC*)AV(t); tv=(UI*)tu;
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
 R z;
}    /* a +/@:g w  for boolean a,w where a-:&(* /@$)w; see also plusinsB */

DF2(jtfslashatg){A fs,gs,y,z;B b,bb,sb=0;C*av,c,d,*wv;I ak,an,ar,*as,at,cv,cvf,m,
     n,nn,r,*s,t,wk,wn,wr,*ws,wt,yt,zn,zt;V*sv;VF ado,adof;
 RZ(a&&w&&self);
 an=AN(a); ar=AR(a); as=AS(a); at=an?AT(a):B01; sv=VAV(self); 
 wn=AN(w); wr=AR(w); ws=AS(w); wt=wn?AT(w):B01; b=ar<=wr; r=b?wr:ar; s=b?ws:as; nn=r?*s:1;
 ASSERT(!ICMP(as,ws,MIN(ar,wr)),EVLENGTH);
 if(SPARSE&(at|wt)||!an||!wn||2>nn){b=CFORK==sv->id; R df1(df2(a,w,b?sv->h:sv->g),b?sv->g:sv->f);}
 zn=(b?wn:an)/nn; m=(b?an:wn)/nn; m=m?m:1; n=zn/m;
 if(CFORK==sv->id){fs=sv->g; gs=sv->h;}else{fs=sv->f; gs=sv->g;}
 y=VAV(fs)->f; c=ID(y); d=ID(gs);
 RZ(var(d,mtv,mtv,at,wt,&ado, &cv )); yt=rtype(cv ); t=atype(cv);
 RZ(var(c,mtv,mtv,yt,yt,&adof,&cvf)); zt=rtype(cvf);
 if(c==CPLUS){
  if(at&B01&&wt&B01&&1==n&&(0==zn%SZI||!SY_ALIGN)&&strchr(sumbf,d))R sumatgbool(a,w,d);
  if(d==CSTAR){
   if(ar&&wr&&at==wt&&at&B01+FL+(INT*!SY_64))R sumattymes(a,w,self);
   if(!ar||!wr){
    z=!ar?tymes(a,df1(w,fs)):tymes(w,df1(a,fs));
    if(jt->jerr==EVNAN)RESETERR else R z;
  }}
  sb=1&&yt&B01;
 }
 if(!(sb||yt==zt))R df1(df2(a,w,gs),fs);
 if(t){
  bb=1&&t&XNUM;
  if(t!=at)RZ(a=bb?xcvt(cv&VXEQ?XMEXMT:cv&VXFC?XMFLR:cv&VXCF?XMCEIL:XMEXACT,a):cvt(t,a));
  if(t!=wt)RZ(w=bb?xcvt(cv&VXEQ?XMEXMT:cv&VXCF?XMFLR:cv&VXFC?XMCEIL:XMEXACT,w):cvt(t,w));
 }
 ak=(an/nn)*bp(AT(a)); wk=(wn/nn)*bp(AT(w));  
 GA(y,yt,zn,1,0); 
 GA(z,zt,zn,r-1,1+s);
 if(sb){A t;I j,tn,*zv;UC*tc;UI*ti,*yv;  /* +/@:g for boolean-valued g */
  av=CAV(a); wv=CAV(w); yv=(UI*)AV(y); zv=AV(z); memset(zv,C0,zn*SZI);
  tn=(zn+SZI-1)/SZI; GA(t,INT,tn,1,0); tc=(UC*)AV(t); ti=(UI*)tc;
  for(j=nn;0<j;j-=255){
   memset(ti,C0,tn*SZI); 
   DO(MIN(j,255), ado(jt,b,m,n,yv,av,wv); av+=ak; wv+=wk; DO(tn,ti[i]+=yv[i];););
   DO(zn, zv[i]+=tc[i];);
 }}else{A z1;B p=0;C*yv,*zu,*zv;
  av=CAV(a)+ak*(nn-1); wv=CAV(w)+wk*(nn-1); yv=CAV(y); zv=CAV(z);
  GA(z1,zt,zn,r-1,1+s); zu=CAV(z1);
  ado(jt,b,m,n,zv,av,wv);
  DO(nn-1, av-=ak; wv-=wk; ado(jt,b,m,n,yv,av,wv); adof(jt,b,zn,1L,p?zv:zu,yv,p?zu:zv); p=!p;);
  if(NEVM<jt->jerr){jt->jerr=0; z=df1(df2(a,w,gs),fs);}else if(p)z=z1;
 }
 RE(0); R z;
}    /* a f/@:g w */
