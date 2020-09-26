/* Copyright 1990-2011, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Atomic (Scalar) Dyadic                                           */

#include "j.h"
#include "ve.h"
#include "vcomp.h"

// reduce/prefix/suffix routines
// first word is the maximum valid precision bit index, followed by that many+1 routines for reduce, and then for prefix and suffix.
// the last routine is always 0 to indicate invalid
// if there are integer-overflow routine, they comes after the others, in the order rps
VARPSA rpsnull = {0, {0}};

static VARPSA rpsbw0000 = {INTX+1 , { {(VARPSF)bw0000insI,VI+VII}, {0}, {(VARPSF)bw0000insI,VI},
                             {(VARPSF)bw0000pfxI,VI+VII}, {0,0}, {(VARPSF)bw0000pfxI,VI},
                             {(VARPSF)bw0000sfxI,VI+VII}, {0}, {(VARPSF)bw0000sfxI,VI},
                             }};
static VARPSA rpsbw0001 = {INTX+1 , { {(VARPSF)bw0001insI,VI+VII}, {0}, {(VARPSF)bw0001insI,VI},
                             {(VARPSF)bw0001pfxI,VI+VII}, {0,0}, {(VARPSF)bw0001pfxI,VI},
                             {(VARPSF)bw0001sfxI,VI+VII}, {0}, {(VARPSF)bw0001sfxI,VI},
                             }};
static VARPSA rpsbw0010 = {INTX+1 , { {(VARPSF)bw0010insI,VI+VII}, {0}, {(VARPSF)bw0010insI,VI},
                             {0,0}, {0,0}, {0,0},
                             {(VARPSF)bw0010sfxI,VI+VII}, {0}, {(VARPSF)bw0010sfxI,VI},
                             }};
static VARPSA rpsbw0011 = {INTX+1 , { {(VARPSF)bw0011insI,VI+VII}, {0}, {(VARPSF)bw0011insI,VI},
                             {(VARPSF)bw0011pfxI,VI+VII}, {0,0}, {(VARPSF)bw0011pfxI,VI},
                             {(VARPSF)bw0011sfxI,VI+VII}, {0}, {(VARPSF)bw0011sfxI,VI},
                             }};
static VARPSA rpsbw0100 = {INTX+1 , { {(VARPSF)bw0100insI,VI+VII}, {0}, {(VARPSF)bw0100insI,VI},
                             {0,0}, {0,0}, {0,0},
                             {(VARPSF)bw0100sfxI,VI+VII}, {0}, {(VARPSF)bw0100sfxI,VI},
                             }};
static VARPSA rpsbw0101 = {INTX+1 , { {(VARPSF)bw0101insI,VI+VII}, {0}, {(VARPSF)bw0101insI,VI},
                             {(VARPSF)bw0101pfxI,VI+VII}, {0,0}, {(VARPSF)bw0101pfxI,VI},
                             {(VARPSF)bw0101sfxI,VI+VII}, {0}, {(VARPSF)bw0101sfxI,VI},
                             }};
static VARPSA rpsbw0110 = {INTX+1 , { {(VARPSF)bw0110insI,VI+VII}, {0}, {(VARPSF)bw0110insI,VI},
                             {(VARPSF)bw0110pfxI,VI+VII}, {0,0}, {(VARPSF)bw0110pfxI,VI},
                             {(VARPSF)bw0110sfxI,VI+VII}, {0}, {(VARPSF)bw0110sfxI,VI},
                             }};
static VARPSA rpsbw0111 = {INTX+1 , { {(VARPSF)bw0111insI,VI+VII}, {0}, {(VARPSF)bw0111insI,VI},
                             {(VARPSF)bw0111pfxI,VI+VII}, {0,0}, {(VARPSF)bw0111pfxI,VI},
                             {(VARPSF)bw0111sfxI,VI+VII}, {0}, {(VARPSF)bw0111sfxI,VI},
                             }};
static VARPSA rpsbw1000 = {INTX+1 , { {(VARPSF)bw1000insI,VI+VII}, {0}, {(VARPSF)bw1000insI,VI},
                             {0,0}, {0,0}, {0,0},
                             {(VARPSF)bw1000sfxI,VI+VII}, {0}, {(VARPSF)bw1000sfxI,VI},
                             }};
static VARPSA rpsbw1001 = {INTX+1 , { {(VARPSF)bw1001insI,VI+VII}, {0}, {(VARPSF)bw1001insI,VI},
                             {(VARPSF)bw1001pfxI,VI+VII}, {0,0}, {(VARPSF)bw1001pfxI,VI},
                             {(VARPSF)bw1001sfxI,VI+VII}, {0}, {(VARPSF)bw1001sfxI,VI},
                             }};
static VARPSA rpsbw1010 = {INTX+1 , { {(VARPSF)bw1010insI,VI+VII}, {0}, {(VARPSF)bw1010insI,VI},
                             {0,0}, {0,0}, {0,0},
                             {(VARPSF)bw1010sfxI,VI+VII}, {0}, {(VARPSF)bw1010sfxI,VI},
                             }};
static VARPSA rpsbw1011 = {INTX+1 , { {(VARPSF)bw1011insI,VI+VII}, {0}, {(VARPSF)bw1011insI,VI},
                             {0,0}, {0,0}, {0,0},
                             {(VARPSF)bw1011sfxI,VI+VII}, {0}, {(VARPSF)bw1011sfxI,VI},
                             }};
static VARPSA rpsbw1100 = {INTX+1 , { {(VARPSF)bw1100insI,VI+VII}, {0}, {(VARPSF)bw1100insI,VI},
                             {0,0}, {0,0}, {0,0},
                             {(VARPSF)bw1100sfxI,VI+VII}, {0}, {(VARPSF)bw1100sfxI,VI},
                             }};
static VARPSA rpsbw1101 = {INTX+1 , { {(VARPSF)bw1101insI,VI+VII}, {0}, {(VARPSF)bw1101insI,VI},
                             {0,0}, {0,0}, {0,0},
                             {(VARPSF)bw1101sfxI,VI+VII}, {0}, {(VARPSF)bw1101sfxI,VI},
                             }};
static VARPSA rpsbw1110 = {INTX+1 , { {(VARPSF)bw1110insI,VI+VII}, {0}, {(VARPSF)bw1110insI,VI},
                             {0,0}, {0,0}, {0,0},
                             {(VARPSF)bw1110sfxI,VI+VII}, {0}, {(VARPSF)bw1110sfxI,VI},
                             }};
static VARPSA rpsbw1111 = {INTX+1 , { {(VARPSF)bw1111insI,VI+VII}, {0}, {(VARPSF)bw1111insI,VI},
                             {(VARPSF)bw1111pfxI,VI+VII}, {0,0}, {(VARPSF)bw1111pfxI,VI},
                             {(VARPSF)bw1111sfxI,VI+VII}, {0}, {(VARPSF)bw1111sfxI,VI},
                             }};

static VARPSA rpsne = {B01X+1 , { {(VARPSF)neinsB,VB},
                         {(VARPSF)nepfxB,VB},
                         {(VARPSF)nesfxB,VB},
                         }};
static VARPSA rpsnor = {B01X+1 , { {(VARPSF)norinsB,VB},
                         {(VARPSF)norpfxB,VB},
                         {(VARPSF)norsfxB,VB},
                         }};
static VARPSA rpsor = {B01X+1 , { {(VARPSF)orinsB,VB},
                         {(VARPSF)orpfxB,VB},
                         {(VARPSF)orsfxB,VB},
                         }};
static VARPSA rpseq = {B01X+1 , { {(VARPSF)eqinsB,VB},
                         {(VARPSF)eqpfxB,VB},
                         {(VARPSF)eqsfxB,VB},
                         }};
static VARPSA rpsand = {B01X+1 , { {(VARPSF)andinsB,VB},
                         {(VARPSF)andpfxB,VB},
                         {(VARPSF)andsfxB,VB},
                         }};
static VARPSA rpsnand = {B01X+1 , { {(VARPSF)nandinsB,VB},
                         {(VARPSF)nandpfxB,VB},
                         {(VARPSF)nandsfxB,VB},
                         }};
static VARPSA rpsge = {B01X+1 , { {(VARPSF)geinsB,VB},
                         {(VARPSF)gepfxB,VB},
                         {(VARPSF)gesfxB,VB},
                         }};
static VARPSA rpsle = {B01X+1 , { {(VARPSF)leinsB,VB},
                         {(VARPSF)lepfxB,VB},
                         {(VARPSF)lesfxB,VB},
                         }};
static VARPSA rpsgt = {B01X+1 , { {(VARPSF)gtinsB,VB},
                         {(VARPSF)gtpfxB,VB},
                         {(VARPSF)gtsfxB,VB},
                         }};
static VARPSA rpslt = {B01X+1 , { {(VARPSF)ltinsB,VB},
                         {(VARPSF)ltpfxB,VB},
                         {(VARPSF)ltsfxB,VB},
                         }};

static VARPSA rpsdiv = {CMPXX+1 , { {(VARPSF)divinsD,VD+VDD}, {0}, {(VARPSF)divinsD,VD+VDD}, {(VARPSF)divinsD,VD}, {(VARPSF)divinsZ,VZ},
                          {(VARPSF)divpfxD,VD+VDD}, {0}, {(VARPSF)divpfxD,VD+VDD}, {(VARPSF)divpfxD,VD}, {(VARPSF)divpfxZ,VZ},
                          {(VARPSF)divsfxD,VD+VDD}, {0}, {(VARPSF)divsfxD,VD+VDD}, {(VARPSF)divsfxD,VD}, {(VARPSF)divsfxZ,VZ},
                         }};

static VARPSA rpsminus = {RATX+1 , {
{(VARPSF)minusinsB,VI}, {0}, {(VARPSF)minusinsI,VI}, {(VARPSF)minusinsD,VD}, {(VARPSF)minusinsZ,VZ},        {0}, {0}, {0},
{(VARPSF)minuspfxB,VI}, {0}, {(VARPSF)minuspfxI,VI}, {(VARPSF)minuspfxD,VD+VIPOKW}, {(VARPSF)minuspfxZ,VZ}, {0}, {(VARPSF)minuspfxX,VX}, {(VARPSF)minuspfxQ,VQ},
{(VARPSF)minussfxB,VI}, {0}, {(VARPSF)minussfxI,VI}, {(VARPSF)minussfxD,VD+VIPOKW}, {(VARPSF)minussfxZ,VZ},  {0}, {0}, {0},
{(VARPSF)minusinsO,VD},{(VARPSF)minuspfxO,VD},{(VARPSF)minussfxO,VD},  // integer-overflow routines
}};
static VARPSA rpsplus = {RATX+1 , {
{(VARPSF)plusinsB,VI}, {0}, {(VARPSF)plusinsI,VI}, {(VARPSF)plusinsD,VD}, {(VARPSF)plusinsZ,VZ},        {0}, {0}, {0},
{(VARPSF)pluspfxB,VI}, {0}, {(VARPSF)pluspfxI,VI}, {(VARPSF)pluspfxD,VD+VIPOKW}, {(VARPSF)pluspfxZ,VZ}, {0}, {(VARPSF)pluspfxX,VX}, {(VARPSF)pluspfxQ,VQ},
{(VARPSF)plussfxB,VI}, {0}, {(VARPSF)plussfxI,VI}, {(VARPSF)plussfxD,VD+VIPOKW}, {(VARPSF)plussfxZ,VZ}, {0}, {(VARPSF)plussfxX,VX}, {(VARPSF)plussfxQ,VQ},
{(VARPSF)plusinsO,VD},{(VARPSF)pluspfxO,VD},{(VARPSF)plussfxO,VD},  // integer-overflow routines
}};
static VARPSA rpstymes = {RATX+1 , {
{(VARPSF)andinsB,VB}, {0}, {(VARPSF)tymesinsI,VI}, {(VARPSF)tymesinsD,VD}, {(VARPSF)tymesinsZ,VZ},        {0}, {0}, {0},
{(VARPSF)andpfxB,VB}, {0}, {(VARPSF)tymespfxI,VI}, {(VARPSF)tymespfxD,VD+VIPOKW}, {(VARPSF)tymespfxZ,VZ}, {0}, {(VARPSF)tymespfxX,VX}, {(VARPSF)tymespfxQ,VQ},
{(VARPSF)andsfxB,VB}, {0}, {(VARPSF)tymessfxI,VI}, {(VARPSF)tymessfxD,VD+VIPOKW}, {(VARPSF)tymessfxZ,VZ}, {0}, {(VARPSF)tymessfxX,VX}, {(VARPSF)tymessfxQ,VQ},
{(VARPSF)tymesinsO,VD},{(VARPSF)tymespfxO,VD},{(VARPSF)tymessfxO,VD},  // integer-overflow routines
}};

static VARPSA rpsmin = {SBTX+1 , {
{(VARPSF)andinsB,VB}, {0}, {(VARPSF)mininsI,VI}, {(VARPSF)mininsD,VD}, {(VARPSF)mininsD,VD+VDD}, {0}, {(VARPSF)mininsX,VX}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {(VARPSF)mininsS,VSB},
{(VARPSF)andpfxB,VB}, {0}, {(VARPSF)minpfxI,VI+VIPOKW}, {(VARPSF)minpfxD,VD+VIPOKW}, {(VARPSF)minpfxD,VD+VDD}, {0}, {(VARPSF)minpfxX,VX}, {(VARPSF)minpfxQ,VQ}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {(VARPSF)minpfxS,VSB},
{(VARPSF)andsfxB,VB}, {0}, {(VARPSF)minsfxI,VI+VIPOKW}, {(VARPSF)minsfxD,VD+VIPOKW}, {(VARPSF)minsfxD,VD+VDD}, {0}, {(VARPSF)minsfxX,VX}, {(VARPSF)minsfxQ,VQ}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {(VARPSF)minsfxS,VSB},
}};
static VARPSA rpsmax = {SBTX+1 , {
{(VARPSF)orinsB,VB}, {0}, {(VARPSF)maxinsI,VI}, {(VARPSF)maxinsD,VD}, {(VARPSF)maxinsD,VD+VDD}, {0}, {(VARPSF)maxinsX,VX}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {(VARPSF)maxinsS,VSB},
{(VARPSF)orpfxB,VB}, {0}, {(VARPSF)maxpfxI,VI+VIPOKW}, {(VARPSF)maxpfxD,VD+VIPOKW}, {(VARPSF)maxpfxD,VD+VDD}, {0}, {(VARPSF)maxpfxX,VX}, {(VARPSF)maxpfxQ,VQ}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {(VARPSF)maxpfxS,VSB},
{(VARPSF)orsfxB,VB}, {0}, {(VARPSF)maxsfxI,VI+VIPOKW}, {(VARPSF)maxsfxD,VD+VIPOKW}, {(VARPSF)maxsfxD,VD+VDD}, {0}, {(VARPSF)maxsfxX,VX}, {(VARPSF)maxsfxQ,VQ}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {(VARPSF)maxsfxS,VSB},
}};


VA va[]={
/* non-atomic functions      */ {
 {{0,0}, {0,0}, {0,0},                                /* BB BI BD              */
  {0,0}, {0,0}, {0,0},                                /* IB II ID              */
  {0,0}, {0,0}, {0,0},                                /* DB DI DD              */
  {0,0}, {0,0}, {0,0}, {0,0}},                        /* ZZ XX QQ Symb         */
  &rpsnull},

/* 10    */ {
 {{(VF)bw0000II,  VI+VII+VIP}, {(VF)bw0000II,  VI+VII+VIP}, {(VF)bw0000II,  VI+VII+VIP}, 
  {(VF)bw0000II,  VI+VII+VIP}, {(VF)bw0000II,  VI+VIP},     {(VF)bw0000II,  VI+VII+VIP},
  {(VF)bw0000II,  VI+VII+VIP}, {(VF)bw0000II,  VI+VII+VIP}, {(VF)bw0000II,  VI+VII+VIP},
  {(VF)bw0000II,  VI+VII+VIP}, {(VF)bw0000II,  VI+VII+VIP}, {(VF)bw0000II,  VI+VII+VIP}, {0,0}}, 
  &rpsbw0000},

/* 11    */ {
 {{(VF)bw0001II,  VI+VII+VIP}, {(VF)bw0001II,  VI+VII+VIP}, {(VF)bw0001II,  VI+VII+VIP}, 
  {(VF)bw0001II,  VI+VII+VIP}, {(VF)bw0001II,  VI+VIP},     {(VF)bw0001II,  VI+VII+VIP},
  {(VF)bw0001II,  VI+VII+VIP}, {(VF)bw0001II,  VI+VII+VIP}, {(VF)bw0001II,  VI+VII+VIP},
  {(VF)bw0001II,  VI+VII+VIP}, {(VF)bw0001II,  VI+VII+VIP}, {(VF)bw0001II,  VI+VII+VIP}, {0,0}}, 
  &rpsbw0001},

/* 12    */ {
 {{(VF)bw0010II,  VI+VII+VIP}, {(VF)bw0010II,  VI+VII+VIP}, {(VF)bw0010II,  VI+VII+VIP}, 
  {(VF)bw0010II,  VI+VII+VIP}, {(VF)bw0010II,  VI+VIP},     {(VF)bw0010II,  VI+VII+VIP},
  {(VF)bw0010II,  VI+VII+VIP}, {(VF)bw0010II,  VI+VII+VIP}, {(VF)bw0010II,  VI+VII+VIP},
  {(VF)bw0010II,  VI+VII+VIP}, {(VF)bw0010II,  VI+VII+VIP}, {(VF)bw0010II,  VI+VII+VIP}, {0,0}}, 
  &rpsbw0010},

/* 13    */ {
 {{(VF)bw0011II,  VI+VII+VIP}, {(VF)bw0011II,  VI+VII+VIP}, {(VF)bw0011II,  VI+VII+VIP}, 
  {(VF)bw0011II,  VI+VII+VIP}, {(VF)bw0011II,  VI+VIP},     {(VF)bw0011II,  VI+VII+VIP},
  {(VF)bw0011II,  VI+VII+VIP}, {(VF)bw0011II,  VI+VII+VIP}, {(VF)bw0011II,  VI+VII+VIP},
  {(VF)bw0011II,  VI+VII+VIP}, {(VF)bw0011II,  VI+VII+VIP}, {(VF)bw0011II,  VI+VII+VIP}, {0,0}}, 
  &rpsbw0011},

/* 14    */ {
 {{(VF)bw0100II,  VI+VII+VIP}, {(VF)bw0100II,  VI+VII+VIP}, {(VF)bw0100II,  VI+VII+VIP}, 
  {(VF)bw0100II,  VI+VII+VIP}, {(VF)bw0100II,  VI+VIP},     {(VF)bw0100II,  VI+VII+VIP},
  {(VF)bw0100II,  VI+VII+VIP}, {(VF)bw0100II,  VI+VII+VIP}, {(VF)bw0100II,  VI+VII+VIP},
  {(VF)bw0100II,  VI+VII+VIP}, {(VF)bw0100II,  VI+VII+VIP}, {(VF)bw0100II,  VI+VII+VIP}, {0,0}}, 
  &rpsbw0100},

/* 15    */ {
 {{(VF)bw0101II,  VI+VII+VIP}, {(VF)bw0101II,  VI+VII+VIP}, {(VF)bw0101II,  VI+VII+VIP}, 
  {(VF)bw0101II,  VI+VII+VIP}, {(VF)bw0101II,  VI+VIP},     {(VF)bw0101II,  VI+VII+VIP},
  {(VF)bw0101II,  VI+VII+VIP}, {(VF)bw0101II,  VI+VII+VIP}, {(VF)bw0101II,  VI+VII+VIP},
  {(VF)bw0101II,  VI+VII+VIP}, {(VF)bw0101II,  VI+VII+VIP}, {(VF)bw0101II,  VI+VII+VIP}, {0,0}}, 
  &rpsbw0101},

/* 16    */ {
 {{(VF)bw0110II,  VI+VII+VIP}, {(VF)bw0110II,  VI+VII+VIP}, {(VF)bw0110II,  VI+VII+VIP}, 
  {(VF)bw0110II,  VI+VII+VIP}, {(VF)bw0110II,  VI+VIP},     {(VF)bw0110II,  VI+VII+VIP},
  {(VF)bw0110II,  VI+VII+VIP}, {(VF)bw0110II,  VI+VII+VIP}, {(VF)bw0110II,  VI+VII+VIP},
  {(VF)bw0110II,  VI+VII+VIP}, {(VF)bw0110II,  VI+VII+VIP}, {(VF)bw0110II,  VI+VII+VIP}, {0,0}}, 
  &rpsbw0110},

/* 17    */ {
 {{(VF)bw0111II,  VI+VII+VIP}, {(VF)bw0111II,  VI+VII+VIP}, {(VF)bw0111II,  VI+VII+VIP}, 
  {(VF)bw0111II,  VI+VII+VIP}, {(VF)bw0111II,  VI+VIP},     {(VF)bw0111II,  VI+VII+VIP},
  {(VF)bw0111II,  VI+VII+VIP}, {(VF)bw0111II,  VI+VII+VIP}, {(VF)bw0111II,  VI+VII+VIP},
  {(VF)bw0111II,  VI+VII+VIP}, {(VF)bw0111II,  VI+VII+VIP}, {(VF)bw0111II,  VI+VII+VIP}, {0,0}}, 
  &rpsbw0111},

/* 18    */ {
 {{(VF)bw1000II,  VI+VII+VIP}, {(VF)bw1000II,  VI+VII+VIP}, {(VF)bw1000II,  VI+VII+VIP}, 
  {(VF)bw1000II,  VI+VII+VIP}, {(VF)bw1000II,  VI+VIP},     {(VF)bw1000II,  VI+VII+VIP},
  {(VF)bw1000II,  VI+VII+VIP}, {(VF)bw1000II,  VI+VII+VIP}, {(VF)bw1000II,  VI+VII+VIP},
  {(VF)bw1000II,  VI+VII+VIP}, {(VF)bw1000II,  VI+VII+VIP}, {(VF)bw1000II,  VI+VII+VIP}, {0,0}}, 
  &rpsbw1000},

/* 19    */ {
 {{(VF)bw1001II,  VI+VII+VIP}, {(VF)bw1001II,  VI+VII+VIP}, {(VF)bw1001II,  VI+VII+VIP}, 
  {(VF)bw1001II,  VI+VII+VIP}, {(VF)bw1001II,  VI+VIP},     {(VF)bw1001II,  VI+VII+VIP},
  {(VF)bw1001II,  VI+VII+VIP}, {(VF)bw1001II,  VI+VII+VIP}, {(VF)bw1001II,  VI+VII+VIP},
  {(VF)bw1001II,  VI+VII+VIP}, {(VF)bw1001II,  VI+VII+VIP}, {(VF)bw1001II,  VI+VII+VIP}, {0,0}}, 
  &rpsbw1001},

/* 1a    */ {
 {{(VF)bw1010II,  VI+VII+VIP}, {(VF)bw1010II,  VI+VII+VIP}, {(VF)bw1010II,  VI+VII+VIP}, 
  {(VF)bw1010II,  VI+VII+VIP}, {(VF)bw1010II,  VI+VIP},     {(VF)bw1010II,  VI+VII+VIP},
  {(VF)bw1010II,  VI+VII+VIP}, {(VF)bw1010II,  VI+VII+VIP}, {(VF)bw1010II,  VI+VII+VIP},
  {(VF)bw1010II,  VI+VII+VIP}, {(VF)bw1010II,  VI+VII+VIP}, {(VF)bw1010II,  VI+VII+VIP}, {0,0}}, 
  &rpsbw1010},

/* 1b    */ {
 {{(VF)bw1011II,  VI+VII+VIP}, {(VF)bw1011II,  VI+VII+VIP}, {(VF)bw1011II,  VI+VII+VIP}, 
  {(VF)bw1011II,  VI+VII+VIP}, {(VF)bw1011II,  VI+VIP},     {(VF)bw1011II,  VI+VII+VIP},
  {(VF)bw1011II,  VI+VII+VIP}, {(VF)bw1011II,  VI+VII+VIP}, {(VF)bw1011II,  VI+VII+VIP},
  {(VF)bw1011II,  VI+VII+VIP}, {(VF)bw1011II,  VI+VII+VIP}, {(VF)bw1011II,  VI+VII+VIP}, {0,0}}, 
  &rpsbw1011},

/* 1c    */ {
 {{(VF)bw1100II,  VI+VII+VIP}, {(VF)bw1100II,  VI+VII+VIP}, {(VF)bw1100II,  VI+VII+VIP}, 
  {(VF)bw1100II,  VI+VII+VIP}, {(VF)bw1100II,  VI+VIP},     {(VF)bw1100II,  VI+VII+VIP},
  {(VF)bw1100II,  VI+VII+VIP}, {(VF)bw1100II,  VI+VII+VIP}, {(VF)bw1100II,  VI+VII+VIP},
  {(VF)bw1100II,  VI+VII+VIP}, {(VF)bw1100II,  VI+VII+VIP}, {(VF)bw1100II,  VI+VII+VIP}, {0,0}}, 
  &rpsbw1100},

/* 1d    */ {
 {{(VF)bw1101II,  VI+VII+VIP}, {(VF)bw1101II,  VI+VII+VIP}, {(VF)bw1101II,  VI+VII+VIP}, 
  {(VF)bw1101II,  VI+VII+VIP}, {(VF)bw1101II,  VI+VIP},     {(VF)bw1101II,  VI+VII+VIP},
  {(VF)bw1101II,  VI+VII+VIP}, {(VF)bw1101II,  VI+VII+VIP}, {(VF)bw1101II,  VI+VII+VIP},
  {(VF)bw1101II,  VI+VII+VIP}, {(VF)bw1101II,  VI+VII+VIP}, {(VF)bw1101II,  VI+VII+VIP}, {0,0}}, 
  &rpsbw1101},

/* 1e    */ {
 {{(VF)bw1110II,  VI+VII+VIP}, {(VF)bw1110II,  VI+VII+VIP}, {(VF)bw1110II,  VI+VII+VIP}, 
  {(VF)bw1110II,  VI+VII+VIP}, {(VF)bw1110II,  VI+VIP},     {(VF)bw1110II,  VI+VII+VIP},
  {(VF)bw1110II,  VI+VII+VIP}, {(VF)bw1110II,  VI+VII+VIP}, {(VF)bw1110II,  VI+VII+VIP},
  {(VF)bw1110II,  VI+VII+VIP}, {(VF)bw1110II,  VI+VII+VIP}, {(VF)bw1110II,  VI+VII+VIP}, {0,0}}, 
  &rpsbw1110},

/* 1f    */ {
 {{(VF)bw1111II,  VI+VII+VIP}, {(VF)bw1111II,  VI+VII+VIP}, {(VF)bw1111II,  VI+VII+VIP}, 
  {(VF)bw1111II,  VI+VII+VIP}, {(VF)bw1111II,  VI+VIP},     {(VF)bw1111II,  VI+VII+VIP},
  {(VF)bw1111II,  VI+VII+VIP}, {(VF)bw1111II,  VI+VII+VIP}, {(VF)bw1111II,  VI+VII+VIP},
  {(VF)bw1111II,  VI+VII+VIP}, {(VF)bw1111II,  VI+VII+VIP}, {(VF)bw1111II,  VI+VII+VIP}, {0,0}}, 
  &rpsbw1111},

/* 95 ~: */ {
 {{(VF)neBB,VB+VIP}, {(VF)neBI,VB+VIPOKA}, {(VF)neBD,VB+VIPOKA},
  {(VF)neIB,VB+VIPOKW}, {(VF)neII,VB}, {(VF)neID,VB},
  {(VF)neDB,VB+VIPOKW}, {(VF)neDI,VB}, {(VF)neDD,VB}, 
  {(VF)neZZ,VB+VZZ}, {(VF)neXX,VB+VXEQ}, {(VF)neQQ,VB+VQQ}, {0,0}},
  &rpsne},

/* 25 %  */ {
 {{(VF)divBB,VD}, {(VF)divBI,VD+VIP0I}, {(VF)divBD,VD+VIPOKW},
  {(VF)divIB,VD+VIPI0}, {(VF)divII,VD+VIPI0+VIP0I}, {(VF)divID,VD+VIPID},
  {(VF)divDB,VD+VIPOKA}, {(VF)divDI,VD+VIPDI}, {(VF)divDD,VD+VIP+VCANHALT}, 
  {(VF)divZZ,VZ+VZZ+VIP}, {(VF)divXX,VX+VXX}, {(VF)divQQ,VQ+VQQ}, {0,0}},
  &rpsdiv},

/* 89 +: */ {
 {{(VF)norBB,VB+VIP    }, {(VF)norBB,VB+VBB+VIP}, {(VF)norBB,VB+VBB+VIP},
  {(VF)norBB,VB+VBB+VIP}, {(VF)norBB,VB+VBB+VIP}, {(VF)norBB,VB+VBB+VIP},
  {(VF)norBB,VB+VBB+VIP}, {(VF)norBB,VB+VBB+VIP}, {(VF)norBB,VB+VBB+VIP}, 
  {(VF)norBB,VB+VBB+VIP}, {(VF)norBB,VB+VBB+VIP}, {(VF)norBB,VB+VBB+VIP}, {0,0}},
  &rpsnor},

/* 88 +. */ {
 {{(VF)orBB,VB+VIP     }, {(VF)gcdII,VI+VII}, {(VF)gcdDD,VD+VDD+VIP},
  {(VF)gcdII,VI+VII}, {(VF)gcdII,VI    }, {(VF)gcdDD,VD+VDD+VIP},
  {(VF)gcdDD,VD+VDD+VIP}, {(VF)gcdDD,VD+VDD+VIP}, {(VF)gcdDD,VD+VIP+VCANHALT}, 
  {(VF)gcdZZ,VZ+VZZ}, {(VF)gcdXX,VX+VXX}, {(VF)gcdQQ,VQ+VQQ}, {0,0}},
  &rpsor},

/* 2d -  */ {
 {{(VF)minusBB,VI    }, {(VF)minusBI,VI+VIPOKW}, {(VF)minusBD,VD+VIPOKW}, 
  {(VF)minusIB,VI+VIPOKA}, {(VF)minusII,VI+VIP}, {(VF)minusID,VD+VIPID},
  {(VF)minusDB,VD+VIPOKA    }, {(VF)minusDI,VD+VIPDI    }, {(VF)minusDD,VD+VIP+VCANHALT}, 
  {(VF)minusZZ,VZ+VZZ+VIP}, {(VF)minusXX,VX+VXX}, {(VF)minusQQ,VQ+VQQ}, {0,0}},
  &rpsminus},

   // For Booleans, VIP means 'inplace if rank not specified and there is no frame'
/* 3c <  */ {
 {{(VF)ltBB,VB+VIP}, {(VF)ltBI,VB+VIPOKA}, {(VF)ltBD,VB+VIPOKA},
  {(VF)ltIB,VB+VIPOKW}, {(VF)ltII,VB}, {(VF)ltID,VB},
  {(VF)ltDB,VB+VIPOKW}, {(VF)ltDI,VB}, {(VF)ltDD,VB}, 
  {(VF)ltDD,VB+VDD+VIP}, {(VF)ltXX,VB+VXFC}, {(VF)ltQQ,VB+VQQ}, {(VF)ltSS,VB}},
  &rpslt},

/* 3d =  */ {
 {{(VF)eqBB,VB+VIP}, {(VF)eqBI,VB+VIPOKA}, {(VF)eqBD,VB+VIPOKA},
  {(VF)eqIB,VB+VIPOKW}, {(VF)eqII,VB}, {(VF)eqID,VB},
  {(VF)eqDB,VB+VIPOKW}, {(VF)eqDI,VB}, {(VF)eqDD,VB}, 
  {(VF)eqZZ,VB+VZZ}, {(VF)eqXX,VB+VXEQ}, {(VF)eqQQ,VB+VQQ}, {(VF)eqII,VB}},
  &rpseq},

/* 3e >  */ {
 {{(VF)gtBB,VB+VIP}, {(VF)gtBI,VB+VIPOKA}, {(VF)gtBD,VB+VIPOKA},
  {(VF)gtIB,VB+VIPOKW}, {(VF)gtII,VB}, {(VF)gtID,VB},
  {(VF)gtDB,VB+VIPOKW}, {(VF)gtDI,VB}, {(VF)gtDD,VB}, 
  {(VF)gtDD,VB+VDD+VIP}, {(VF)gtXX,VB+VXCF}, {(VF)gtQQ,VB+VQQ}, {(VF)gtSS,VB}},
  &rpsgt},

/* 8a *. */ {
 {{(VF)andBB,VB+VIP    }, {(VF)lcmII,VI+VII}, {(VF)lcmDD,VD+VDD+VIP},
  {(VF)lcmII,VI+VII}, {(VF)lcmII,VI    }, {(VF)lcmDD,VD+VDD+VIP},
  {(VF)lcmDD,VD+VDD+VIP}, {(VF)lcmDD,VD+VDD+VIP}, {(VF)lcmDD,VD+VIP+VCANHALT}, 
  {(VF)lcmZZ,VZ+VZZ}, {(VF)lcmXX,VX+VXX}, {(VF)lcmQQ,VQ+VQQ}, {0,0}},
  &rpsand},

/* 8b *: */ {
 {{(VF)nandBB,VB+VIP},     {(VF)nandBB,VB+VBB+VIP}, {(VF)nandBB,VB+VBB+VIP},
  {(VF)nandBB,VB+VBB+VIP}, {(VF)nandBB,VB+VBB+VIP}, {(VF)nandBB,VB+VBB+VIP},
  {(VF)nandBB,VB+VBB+VIP}, {(VF)nandBB,VB+VBB+VIP}, {(VF)nandBB,VB+VBB+VIP}, 
  {(VF)nandBB,VB+VBB+VIP}, {(VF)nandBB,VB+VBB+VIP}, {(VF)nandBB,VB+VBB+VIP}, {0,0}},
  &rpsnand},

/* 85 >: */ {
 {{(VF)geBB,VB+VIP}, {(VF)geBI,VB+VIPOKA}, {(VF)geBD,VB+VIPOKA},
  {(VF)geIB,VB+VIPOKW}, {(VF)geII,VB}, {(VF)geID,VB},
  {(VF)geDB,VB+VIPOKW}, {(VF)geDI,VB}, {(VF)geDD,VB}, 
  {(VF)geDD,VB+VDD+VIP}, {(VF)geXX,VB+VXFC}, {(VF)geQQ,VB+VQQ}, {(VF)geSS,VB}},
  &rpsge},

/* 83 <: */ {
 {{(VF)leBB,VB+VIP}, {(VF)leBI,VB+VIPOKA}, {(VF)leBD,VB+VIPOKA},
  {(VF)leIB,VB+VIPOKW}, {(VF)leII,VB}, {(VF)leID,VB},
  {(VF)leDB,VB+VIPOKW}, {(VF)leDI,VB}, {(VF)leDD,VB}, 
  {(VF)leDD,VB+VDD+VIP}, {(VF)leXX,VB+VXCF}, {(VF)leQQ,VB+VQQ}, {(VF)leSS,VB}},
  &rpsle},

/* 82 <. */ {
 {{(VF)andBB,VB+VIP}, {(VF)minBI,VI+VIPOKW}, {(VF)minBD,VD+VIPOKW},
  {(VF)minIB,VI+VIPOKA}, {(VF)minII,VI+VIP}, {(VF)minID,VD+VIPID},
  {(VF)minDB,VD+VIPOKA}, {(VF)minDI,VD+VIPDI}, {(VF)minDD,VD+VIP}, 
  {(VF)minDD,VD+VDD+VIP}, {(VF)minXX,VX+VXX}, {(VF)minQQ,VQ+VQQ}, {(VF)minSS,VSB}},  // always VIP a forced conversion
  &rpsmin},

/* 84 >. */ {
 {{(VF)orBB,VB+VIP}, {(VF)maxBI,VI+VIPOKW}, {(VF)maxBD,VD+VIPOKW},
  {(VF)maxIB,VI+VIPOKA}, {(VF)maxII,VI+VIP}, {(VF)maxID,VD+VIPID},
  {(VF)maxDB,VD+VIPOKA}, {(VF)maxDI,VD+VIPDI}, {(VF)maxDD,VD+VIP}, 
  {(VF)maxDD,VD+VDD+VIP}, {(VF)maxXX,VX+VXX}, {(VF)maxQQ,VQ+VQQ}, {(VF)maxSS,VSB}},
  &rpsmax},

/* 2b +  */ {
 {{(VF)plusBB,VI    }, {(VF)plusBI,VI+VIPOKW}, {(VF)plusBD,VD+VIPOKW}, 
  {(VF)plusIB,VI+VIPOKA}, {(VF)plusII,VI+VIP}, {(VF)plusID,VD+VIPID}, 
  {(VF)plusDB,VD+VIPOKA    }, {(VF)plusDI,VD+VIPDI    }, {(VF)plusDD,VD+VIP+VCANHALT}, 
  {(VF)plusZZ,VZ+VZZ+VIP}, {(VF)plusXX,VX+VXX}, {(VF)plusQQ,VQ+VQQ}, {0,0}},
  &rpsplus},

/* 2a *  */ {
 {{(VF)andBB,  VB+VIP}, {(VF)tymesBI,VI+VIPOKW}, {(VF)tymesBD,VD+VIPOKW},
  {(VF)tymesIB,VI+VIPOKA}, {(VF)tymesII,VI+VIP}, {(VF)tymesID,VD+VIPID},
  {(VF)tymesDB,VD+VIPOKA}, {(VF)tymesDI,VD+VIPDI}, {(VF)tymesDD,VD+VIP}, 
  {(VF)tymesZZ,VZ+VZZ+VIP}, {(VF)tymesXX,VX+VXX}, {(VF)tymesQQ,VQ+VQQ}, {0,0}},
  &rpstymes},

/* 5e ^  */ {   // may produce complex numbers
 {{(VF)geBB, VB+VIP}, {(VF)powBI,VD}, {(VF)powBD,VD},
  {(VF)powIB,VI}, {(VF)powII,VD}, {(VF)powID,VD+VCANHALT},
  {(VF)powDB,VD}, {(VF)powDI,VD}, {(VF)powDD,VD+VCANHALT}, 
  {(VF)powZZ,VZ+VZZ}, {(VF)powXX,VX+VXX}, {(VF)powQQ,VQ+VQQ}, {0,0}},
  &rpsge},

/* 7c |  */ {
 {{(VF)ltBB, VB+VIP    }, {(VF)remII,VI+VII+VIP}, {(VF)remDD,VD+VDD+VIP},
  {(VF)remII,VI+VII+VIP}, {(VF)remII,VI+VIP    }, {(VF)remID,VI+VCANHALT    },   // remID can 'overflow' if result is nonintegral
  {(VF)remDD,VD+VDD+VIP}, {(VF)remDD,VD+VDD+VIP}, {(VF)remDD,VD+VIP+VCANHALT}, 
  {(VF)remZZ,VZ+VZZ}, {(VF)remXX,VX+VXX}, {(VF)remQQ,VQ+VQQ}, {0,0}},
  &rpslt},

/* 21 !  */ {
 {{(VF)leBB, VB+VIP            }, {(VF)binDD,VD+VDD+VRI+VIP}, {(VF)binDD,VD+VDD+VIP}, 
  {(VF)binDD,VD+VDD+VRI+VIP}, {(VF)binDD,VD+VDD+VRI+VIP}, {(VF)binDD,VD+VDD+VIP}, 
  {(VF)binDD,VD+VDD+VIP    }, {(VF)binDD,VD+VDD+VIP    }, {(VF)binDD,VD+VIP    }, 
  {(VF)binZZ,VZ+VZZ}, {(VF)binXX,VX+VXX}, {(VF)binQQ,VX+VQQ}, {0,0}}, 
  &rpsle},

/* d1 o. */ {
 {{(VF)cirDD,VD+VDD}, {(VF)cirDD,VD+VDD}, {(VF)cirBD,VD},
  {(VF)cirDD,VD+VDD}, {(VF)cirDD,VD+VDD}, {(VF)cirID,VD},
  {(VF)cirDD,VD+VDD}, {(VF)cirDD,VD+VDD}, {(VF)cirDD,VD}, 
  {(VF)cirZZ,VZ+VZZ+VRD}, {(VF)cirDD,VD+VDD}, {(VF)cirDD,VD+VDD}, {0,0}},
  &rpsnull},
};

A jtcvz(J jt,I cv,A w){I t;
 t=AT(w);
 if(cv&VRD&&!(t&FL) )R pcvt(FL,w);  // convert if possible
 if(cv&VRI&&!(t&INT))R icvt(w);  // convert to integer if possible
 R w;
}    /* convert result */

#if 0 // obsolete
// Routine to lookup function/flags for u/ u\ u\.
// ptr is the type of lookup (insert/prefix/suffix) to generate the function for
// In the function, id is the pseudochar for the function to look up
//  t is the argument type
//  action routine is stored in *ado, flags in *cv.  If no action routine, *ado=0
#define VA2F(fname,ptr,fp,fm,ft)   \
 static VARPS fname##EWOV[] = { {ft,VD} , {fp,VD}, {fm,VD}, {0,0} }; \
 VARPS jt##fname(J jt,A self,I t){  \
  if(!(jt->jerr>=EWOV)){  /* normal case, starting the op */                        \
   t&=(NUMERIC+SBT)&(~SPARSE); t=FAV(self)->flag&VISATOMIC2?t:0;   /* t=0 is not numeric or not atomic fn */  \
   I tx=t>>INTX; if(t>FL){tx>>=(XNUMX-INTX); tx=t>RAT?3:tx; tx+=3;}  /* index of type */  \
   VARPS *ra=((VA*)(FAV(self)->localuse.lvp[0]))->ptr+tx; ra=t?ra:fname##EWOV+3;  /* point to return slot */  \
   R *ra;   /* return the routine address/flags */  \
  }else{ /* here to recover from integer overflow */ \
   jt->jerr=0;                                 \
   R fname##EWOV[2*(FAV(self)->id==CMINUS) + (FAV(self)->id==CPLUS)]; \
  }  \
 }
// Lookup the action routine & flags for insert/prefix/suffix
// first name is name of the generated function
// second name (e. g. pins) is the name of the structure element containing the action-routine pointers
// other names are the names of overflow action routines for plus,minus,times respectively

VA2F(vains,pins, plusinsO,minusinsO,tymesinsO)
VA2F(vapfx,ppfx, pluspfxO,minuspfxO,tymespfxO)
VA2F(vasfx,psfx, plussfxO,minussfxO,tymessfxO)
#endif

// obsolete #define VARCASE(e,c) (70*(e)+(c))

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
// repair routines for 
static VF repairip[4] = {plusBIO, plusIIO, minusBIO, minusIIO};

#if 0
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
#endif

// All dyadic arithmetic verbs f enter here, and also f"n.  a and w are the arguments, id
// is the pseudocharacter indicating what operation is to be performed.  self is the block for this primitive,
// ranks are the ranks of the verb, argranks are the ranks of a and w combined into 1 field
static A jtva2(J jt,AD * RESTRICT a,AD * RESTRICT w,AD * RESTRICT self,RANK2T ranks,UI argranks){A z;I m,
     mf,n,nf,shortr,* RESTRICT s,*sf,zn;VA2 adocv;UI fr;  // fr will eventually be frame/rank  nf (and mf) change roles during execution
 I aawwzk[5];  // a outer/only, a inner, w outer/only, w inner, z
 fr=argranks>>RANKTX; shortr=argranks&RANKTMSK;  // fr,shortr = ar,wr to begin with.  Changes later
 F2PREFIP;
 {I at=AT(a);
  I wt=AT(w);
  if(!(((I)jtinplace&(JTRETRY|JTEMPTY))+((UNSAFE(at|wt))&(NOUN&~(B01|INT|FL))))){  // no error, bool/int/fl args, no empties
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
   if(unlikely((at|wt)&SPARSE)){
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
    nf=REPSGN(raminusw);  // nf=-1 if w has longer frame, means cannot inplace a
    raminusw=-raminusw;   // now aw-ar
    mf=REPSGN(raminusw);  // mf=-1 if a has longer frame, means cannot inplace w
    raminusw=raminusw&nf; fr+=raminusw; shortr-=raminusw;  // if ar is the longer one, change nothing; otherwise transfer aw-ar from shortr to r
    PROD(n,fr-shortr,s+shortr);  // treat the entire operands as one big cell; get the rest of the values needed
    t=(A)((I)t^((I)a^(I)w)); m=AN(t);  // rank, atoms of shorter frame  m needed for data move
   }
   // notionally we now repurpose fr to be frame/rank, with the frame 0
   if(jtinplace){
    // Non-sparse setup for copy loop, no rank
      // get number of inner cells
    n^=REPSGN(1-n)&nf;  // encode 'w has long frame, so a is repeated' as complementary n; but if n<2, leave it alone.  Since n=1 when frames are equal, nf in that case is N/C
    nf=(adocv.cv>>VIPOKWX) & ((I)(a==w)-1) & (3 + nf*2 + mf);  // set inplaceability here: not if addresses equal (in case of retry); only if op supports; only if nonrepeated cell
    jtinplace = (J)(((I)jtinplace&nf)+4*nf+(adocv.cv&-16));  // bits 0-1 of jtinplace are combined input+local; 2-3 just local; 4+ hold adocv.cv; at least one is set to show non-sparse
    mf=/*nf= obsolete*/1;  // suppress the outer loop, leaving only the loop over m and n
   }else{
    // Sparse setup: move the block-local variables to longer-lived ones.  We are trying to reduce register pressure
    // repurpose ak/wk/mf/nf to hold acr/wcr/af/wf, which we will pass into vasp.  This allows acr/wcr/af/wf to be block-local
    aawwzk[0]=argranks>>RANKTX; aawwzk[1]=argranks&RANKTMSK; mf=0; nf=0;
   }
  }else{I af,wf,acr,wcr,q,ak,wk,zk;  // fr, shortr are left/right verb rank here
   // Here, a rank was specified.  That means there must be a frame, according to the IRS rules
   acr=ranks>>RANKTX; acr=(I)fr<acr?fr:acr; af=fr-acr; PROD(ak,acr,AS(a)+af);  // acr=effective rank, af=left frame, here ak=left #atoms/cell
   wcr=(RANKT)ranks; wcr=shortr<wcr?shortr:wcr; wf=shortr-wcr; PROD(wk,wcr,AS(w)+wf); // r=right rank of verb, wcr=effective rank, wf=right frame, here wk=right #atoms/cell
       // note: the prod above can never fail, because it gives the actual # cells of an existing noun
   // Now that we have used the rank info, clear jt->ranks.
   // we do this before we generate failures
   // if the frames don't agree, that's always an agreement error
   if(jtinplace){  // If not sparse... This block isn't needed for sparse arguments, and may fail on them.  We move it here to reduce register pressure
    nf=acr<=wcr; zk=acr<=wcr?wk:ak; m=acr<=wcr?ak:wk; fr=acr<=wcr?wcr:acr; shortr=acr<=wcr?acr:wcr; s=AS(acr<=wcr?w:a)+(acr<=wcr?wf:af); PROD(n,fr-shortr,s+shortr);   // nf='w has long frame'; zk=#atoms in cell with larger rank;
    n^=REPSGN(1-n)&-nf;  // encode 'w has long frame, so a is repeated' as complementary n; but if n<2, leave it alone
    // m=#atoms in cell with shorter rank; n=#times shorter-rank cells must be repeated; r=larger of cell-ranks; s->shape of larger-rank cell
    // now shortr has the smaller cell-rank, and acr/wcr are free.  fr has the longer cell-rank
    // if looping required, calculate the strides for input & output.  Needed only if mf or nf>1, but not worth testing, since presumably one will, else why use rank?
    // zk=result-cell size in bytes; ak,wk=left,right arg-cell size in bytes.  Not needed if not looping
    ak*=bp(AT(a)); wk*=bp(AT(w));  // calculate early, using bp, to minimize ALU time & allow time for load/mul to settle.  zt may still be settling
    // if the cell-shapes don't match, that's an agreement error UNLESS the frame contains 0; in that case it counts as
    // 'error executing on the cell of fills' and produces a scalar 0 as the result for that cell, which we handle by changing the result-cell rank to 0
    // Nonce: continue giving the error even when frame contains 0 - remove 1|| in the next line to conform to fill-cell rules
// this shows the fix   if(ICMP(as+af,ws+wf,MIN(acr,wcr))){if(1||zn)ASSERT(0,EVLENGTH)else r = 0;}
    ASSERTAGREE(AS(a)+af, AS(w)+wf, shortr)  // now shortr is free
// obsolete     awzk[0]=ak; awzk[1]=wk;  // get these values out of registers - the compiler thinks they're needed for a loop
    {aawwzk[0]=ak; ak=af<=wf?0:ak; aawwzk[1]=ak; aawwzk[2]=wk; wk=af<=wf?wk:0; aawwzk[3]=wk;  // set inner cell size for last followed by non-last.  Last is 0 fror a repeated cell
    I f=af<=wf?wf:af; q=af<=wf?af:wf; sf=AS(af<=wf?w:a); /* obsolete mf=af<=wf;*/   // f=#longer frame; q=#shorter frame; sf->shape of arg with longer frame   mf holds -1 if wf is longer   af/wf free
    nf=(adocv.cv>>VIPOKWX) & ((I)(a==w)-1) & ((I)((argranks>>RANKTX)==f+fr)*2 + (I)((argranks&RANKTMSK)==f+fr));  // set inplaceability here: not if addresses equal (in case of retry); only if op supports; only if nonrepeated cell
    jtinplace = (J)(((I)jtinplace&nf)+4*nf+(adocv.cv&-16));  // bits 0-1 of jtinplace are combined input+local; 2-3 just local; 4+ hold adocv.cv; at least one is set to show non-sparse
    PROD(nf,f-q,q+sf);    // mf=#cells in common frame, nf=#times shorter-frame cell must be repeated.
    fr+=f<<RANKTX;  // encode f into fr
    }
// obsolete #ifdef DPMULD
    { DPMULDE(nf,zk,zn) /* obsolete nf^=REPSGN(1-nf)&-mf;*/ PROD(mf,q,sf); DPMULDE(zn,mf,zn) }
// obsolete #else
// obsolete     RE(zn=mult(nf,zk)); /* obsolete nf^=REPSGN(1-nf)&-mf; */ PROD(mf,q,sf); RE(zn=mult(mf,zn));  // zn=total # result atoms  (only if non-sparse)
// obsolete #endif
    // Now nf=outer repeated frame  mf=outer common frame  n=inner repeated frame  m=inner common frame
    //    leading axes --------------------------------------------------------------> trailing axes
    // loop migration: if the outer loop can be subsumed into the inner loop do that to make the faster inner loops more effective
    // the cases we see are:
    // 1: m=1 and n=1: move mf->m, nf->n, and complement n if ak==0 (i. e. x arg is repeated) example:  list *"0 _ atom
    // 2: n=1 and nf=1: multiply m by mf  example: (shape 4 5) *"1 (shape 4 5 or 1 4 5)
    if(unlikely(n==SGNTO0(((m-2)|(nf-2))))){  // n==1 && (m==1||nf==1) - ignore 0 in shape, which aborts early
     // migration is possible
     m*=mf; n=nf^REPSGN((ak-1)&(1-nf));   // propagate mf and nf down; if n is not 1, complement if ak is 0
     mf=1;  // no outer loops.  nf immaterial
    }else{
     // normal case
     mf*=nf; --nf; // mf is total # iterations; nf is outer loop repeat count-1
    }
    zk*=bp(rtype((I)jtinplace));  // now create zk, which is used later than ak/wk.
    aawwzk[4]=zk;  // move it out of registers
   }else{aawwzk[0]=acr; aawwzk[1]=wcr; mf=af; nf=wf;}  // For sparse, repurpose ak/wk/mf/nf to hold acr/wcr/af/wf, which we will pass into vasp.  This allows acr/wcr/af/wf to be block-local
  }
  // TODO: for 64-bit, we could move f and r into upper jtinplace; use bit 4 of jtinplace for testing below; make f/r block-local; extract f/r below as needed
 }

 RESETRANK;  // This is required for xnum/rat/sparse, which call IRS-enabled routines internally.  We could suppress this for mainline types, perhaps in var().  Anyone who sets this must set it back,
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
   if((I)jtinplace&VARGMSK&&zn>0){I t=atype((I)jtinplace);  // input conversion required (rare), and the result is not empty
    // Conversions to XNUM use a routine that pushes/sets/pops jt->mode, which controls the
    // type of conversion to XNUM in use.  Any result of the conversion is automatically inplaceable.  If type changes, change the cell-size too, possibly larger or smaller
    // bits 2-3 of jtinplace indicate whether inplaceability is allowed by the op, the ranks, and the addresses
// obsolete     if(TYPESNE(t,AT(a))){aawwzk[0]=(aawwzk[0]>>bplg(AT(a)))<<bplg(t); aawwzk[1]=(aawwzk[1]>>bplg(AT(a)))<<bplg(t); RZ(a=!(t&XNUM)?cvt(t,a):xcvt(((I)jtinplace&VXCVTYPEMSK)>>VXCVTYPEX,a));jtinplace = (J)(intptr_t)((I)jtinplace | (((I)jtinplace>>2)&JTINPLACEA));}
// obsolete     if(TYPESNE(t,AT(w))){aawwzk[2]=(aawwzk[2]>>bplg(AT(w)))<<bplg(t); aawwzk[3]=(aawwzk[3]>>bplg(AT(w)))<<bplg(t); RZ(w=!(t&XNUM)?cvt(t,w):xcvt(((I)jtinplace&VXCVTYPEMSK)>>VXCVTYPEX,w));jtinplace = (J)(intptr_t)((I)jtinplace | (((I)jtinplace>>2)&JTINPLACEW));}
    if(TYPESNE(t,AT(a))){aawwzk[0]=(aawwzk[0]>>bplg(AT(a)))<<bplg(t); aawwzk[1]=(aawwzk[1]>>bplg(AT(a)))<<bplg(t); RZ(a=cvt(t|((I)jtinplace&VARGCVTMSKF),a)); jtinplace = (J)(intptr_t)((I)jtinplace | (((I)jtinplace>>2)&JTINPLACEA));}
    if(TYPESNE(t,AT(w))){aawwzk[2]=(aawwzk[2]>>bplg(AT(w)))<<bplg(t); aawwzk[3]=(aawwzk[3]>>bplg(AT(w)))<<bplg(t); RZ(w=cvt(t|((I)jtinplace&VARGCVTMSKF),w)); jtinplace = (J)(intptr_t)((I)jtinplace | (((I)jtinplace>>2)&JTINPLACEW));}
   }  // It might be better to do the conversion earlier, and defer the error
      // until here.  We will have to look at the generated code when we can use all the registers

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
// obsolete  if( (SGNIF(jtinplace,JTINPLACEWX)&AC(w))<0 || ((SGNIF(jtinplace,JTINPLACEWX)&(AC(w)-2))<0)&&jt->assignsym&&jt->assignsym->val==w/* no longer &&!((I)jtinplace&VCANHALT && jt->asgzomblevel<2)*/){z=w; I zt=rtype((I)jtinplace); if(TYPESNE(AT(w),zt))MODBLOCKTYPE(z,zt)  //  Uses JTINPLACEW==1
// obsolete   }else if( (SGNIF(jtinplace,JTINPLACEAX)&AC(a))<0 || ((SGNIF(jtinplace,JTINPLACEAX)&(AC(a)-2))<0)&&jt->assignsym&&jt->assignsym->val==a/* no longer &&!((I)jtinplace&VCANHALT && jt->asgzomblevel<2)*/){z=a; I zt=rtype((I)jtinplace); if(TYPESNE(AT(a),zt))MODBLOCKTYPE(z,zt)  //  Uses JTINPLACEA==2
  if(ASGNINPLACESGN(SGNIF(jtinplace,JTINPLACEWX),w)){z=w; I zt=rtype((I)jtinplace); if(TYPESNE(AT(w),zt))MODBLOCKTYPE(z,zt)  //  Uses JTINPLACEW==1
  }else if(ASGNINPLACESGN(SGNIF(jtinplace,JTINPLACEAX),a)){z=a; I zt=rtype((I)jtinplace); if(TYPESNE(AT(a),zt))MODBLOCKTYPE(z,zt)  //  Uses JTINPLACEA==2
  }else{GA(z,rtype((I)jtinplace),zn,(RANKT)fr+(fr>>RANKTX),0); MCISH(AS(z),sf,fr>>RANKTX); MCISH(AS(z)+(fr>>RANKTX),s,(RANKT)fr);} 
  // s, fr, and sf ARE NOT USED FROM HERE ON in this branch to reduce register pressure.
  if(!zn)RETF(z);  // If the result is empty, the allocated area says it all
  // zn  NOT USED FROM HERE ON

  // End of setup phase.  The execution phase:

  // The compiler thinks that because ak/wk/zk are used in the loop they should reside in registers.  We do better to keep a and w in registers.  So we
  // force the compiler to spill aawwzk by using address arithmetic.
  {I rc=EVOK;
   {  // lowest rc from the executed sections
    C *av=CAV(a); C *wv=CAV(w); C *zv=CAV(z);  // point to the data
    // Call the action routines: nf,mf, etc must be preserved in case of repair
#if 0   // obsolete
    if(nf-1==0){I i=mf; do{((AHDR2FN*)adocv.f)(n,m,av,wv,zv,jt); if(!--i)break; av+=awzk[0]; wv+=awzk[1]; zv+=awzk[2];}while(1);}  // if the short cell is not repeated, loop over the frame
    else if(nf-1<0){I im=mf; do{I in=~nf; do{((AHDR2FN*)adocv.f)(n,m,av,wv,zv,jt); wv+=awzk[1]; zv+=awzk[2];}while(--in); av+=awzk[0];}while(--im);} // if right frame is longer, repeat cells of a
    else         {I im=mf; do{I in=nf; do{((AHDR2FN*)adocv.f)(n,m,av,wv,zv,jt); av+=awzk[0]; zv+=awzk[2];}while(--in); wv+=awzk[1];}while(--im);}  // if left frame is longer, repeat cells of w
#else
     // mf has the total number of calls.  nf is 1 less than the number of calls with a repeated cell.  aawwzk[0,1] are the cell-size of a for the outer loop, aawwzk[2,3] are for w;
     // but aawwzk[1,3] have 0 in a repeated argument.  aawwzk[1,3] are added for each inner iteration, aawwzk[0,2] at the end of an inner cycle
     // m is the number of outer loops the caller will run
     // n is the number of times the inner-loop atom is repeated for each outer loop: n=1 means no inner loop needed; n>1 means each atom of y is repeated n times; n<0 means each atom of x is repeated ~n times.  n*m cannot=0. 
   I i=mf; I jj=nf; while(1){I lrc=((AHDR2FN*)adocv.f)(n,m,av,wv,zv,jt); rc=lrc<rc?lrc:rc; if(!--i)break; zv+=aawwzk[4]; I jj1=--jj; jj=jj<0?nf:jj; av+=aawwzk[1+REPSGN(jj1)]; wv+=aawwzk[3+REPSGN(jj1)];}  // jj1 is -1 on the last inner iter, where we use outer incr
#endif
   }
   // The work has been done.  If there was no error, check for optional conversion-if-possible or -if-necessary
   if(rc==EVOK){RETF(!((I)jtinplace&VRI+VRD)?z:cvz((I)jtinplace,z));  // normal return is here.  The rest is error recovery
   }else if(rc-EWOVIP>=0){A zz;C *zzv;I zzk;
    // Here for overflow that can be corrected in place
// not yet    if(rc==EVOKCLEANUP){jt->mulofloloc=0; RETF(z);}  // if multiply that did not overflow, clear the oflo position for next time, and return
    // If the original result block cannot hold the D result, allocate a separate result area
    if(sizeof(D)==sizeof(I)){zz=z; MODBLOCKTYPE(zz,FL); zzk=aawwzk[4];   // shape etc are already OK
    }else{GATV(zz,FL,AN(z),AR(z),AS(z)); zzk=aawwzk[4]*(sizeof(D)/sizeof(I));}
    // restore pointers to beginning of arguments
    zzv=CAV(zz);  // point to new-result data
    // Set up pointers etc for the overflow handling.  Set b=1 if w is taken for the x argument to repair
    if(rc==EWOVIP+EWOVIPMULII){D *zzvd=(D*)zzv; I *zvi=IAV(z);
     // Multiply repair.  We have to convert all the pre-overflow results to float, and then finish the multiplies
     jt->mulofloloc = ~jt->mulofloloc;  // make length positive
     DQ(jt->mulofloloc, *zzvd++=(D)*zvi++;);  // convert the multiply results to float.  mulofloloc is known negative, and must be complemented
     // Now repeat the processing.  Unlike with add/subtract overflow, we have to match up all the argument atoms
     {C *av=CAV(a); C *wv=CAV(w);
#if 0
     adocv.f=(VF)tymesIIO;  // multiply-repair routine
      I wkm,wkn,akm,akn;
      wkm=awzk[1], akn=awzk[0]; wkn=REPSGN(nf); nf^=wkn;   // wkn=111..111 iff wk increments with n (and therefore ak with m).  Make nf positive
      akm=akn&wkn; wkn&=wkm; wkm^=wkn; akn^=akm;  // if c, akm=ak/wkn=wk; else akn=ak/wkm=wk.  The other incr is 0
      I im=mf; do{I in=nf; do{((AHDR2FN*)adocv.f)(n,m,av,wv,zzv,jt); zzv+=zzk; av+=akn; wv +=wkn;}while(--in); if(!--im)break; av+=akm; wv +=wkm;}while(1);
#else
      I i=mf; I jj=nf; while(1){tymesIIO(n,m,(I*)av,(I*)wv,(D*)zzv,jt); if(!--i)break; zzv+=zzk; I jj1=--jj; jj=jj<0?nf:jj; av+=aawwzk[1+REPSGN(jj1)]; wv+=aawwzk[3+REPSGN(jj1)];}  // jj1 is -1 on the last inner iter, where we use outer incr
#endif
     }
// not yet     jt->mulofloloc=0;  // reinit for next time
    } else {   // not multiply repair, but something else to do inplace
     adocv.f = repairip[(rc-EWOVIP)&3];   // fetch ep from table
     I nipw = ((z!=w) & (rc-EWOVIP)) ^ (((rc-EWOVIP)>>2) & 1);  // nipw from z!=w if bits2,0==01; 1 if 10; 0 if 00
// obsolete      switch(rc-EWOVIP){
// obsolete      case EWOVIPPLUSII:
// obsolete       // choose the non-in-place argument
// obsolete       adocv.f=(VF)plusIIO; nipw = z!=w; break; // if w not repeated, select it for not-in-place
// obsolete      case EWOVIPPLUSBI:
// obsolete       adocv.f=(VF)plusBIO; nipw = 0; break;   // Leave the Boolean argument as a
// obsolete      case EWOVIPPLUSIB:
// obsolete       adocv.f=(VF)plusBIO; nipw = 1; break;  // Use w as not-in-place
// obsolete      case EWOVIPMINUSII:
// obsolete       adocv.f=(VF)minusIIO; nipw = z!=w; break; // if w not repeated, select it for not-in-place
// obsolete      case EWOVIPMINUSBI:
// obsolete       adocv.f=(VF)minusBIO; nipw = 0; break;   // Leave the Boolean argument as a
// obsolete      case EWOVIPMINUSIB:
// obsolete       adocv.f=(VF)minusBIO; nipw = 1; break;  // Use w as not-in-place
// obsolete      }
     // nipw means 'use w as not-in-place'; c means 'repeat cells of a'; so if nipw!=c we repeat cells of not-in-place, if nipw==c we set nf to 1
     // if we are repeating cells of the not-in-place, we leave the repetition count in nf, otherwise subsume it in mf
     // b means 'repeat atoms inside a'; so if nipw!=b we repeat atoms of not-in-place, if nipw==b we set n to 1
     {C *av, *zv=CAV(z);
#if 0
      I origc=SGNTO0(nf); I origb=SGNTO0(n);   // see what the original repeat flags were
      nf^=REPSGN(nf); n^=REPSGN(n);  // take abs of n, nf for calculations here
      if(nipw){av=CAV(w), awzk[0]=awzk[1];}else{av=CAV(a);} if(nipw==origc){mf *= nf; nf = 1;} if(nipw==origb){m *= n; n = 1;}
      n^=-nipw;  // install new setting of b flag
     // We have set up ado,nf,mf,nipw,m,n for the conversion.  Now call the repair routine.  n is # times to repeat a for each z, m*n is # atoms of z/zz
      DQ(mf, DQ(nf, ((AHDR2FN*)adocv.f)(n,m,av,zv,zzv,jt); zzv+=zzk; zv+=awzk[2];); av+=awzk[0];)  // use each cell of a (nf) times
#else
      // zv and zzv process without repeats; they contain all the information for the in-place argument (if any).
      // av may have repeats.  Repeats before the function call are handled exactly as the first time through, by using aawwzk.
      // repeats inside the function call (from n) must appear only on a, i. e. n<0 to repeat a, or n==1 for no repeat
      // if the original internal repeat was on the argument that is now z (e. g. nipw is set, meaning original a is going to z, and n is negative, meaning repeat inside a)
      // we must multiply out the repeat to leave n=1.
      av=CAV(nipw?w:a);  // point to the not-in-place argument
      I nsgn=SGNTO0(n); n^=REPSGN(n); if(nipw==nsgn){m *= n; n = 1;} n^=-nipw;  // force n to <=1; make n flag indicate whether args were switched
      I i=mf; I jj=nf; while(1){((AHDR2FN*)adocv.f)(n,m,av,zv,zzv,jt); if(!--i)break; zv+=aawwzk[4]; zzv+=zzk; I jj1=--jj; jj=jj<0?nf:jj; av+=aawwzk[2*nipw+1+REPSGN(jj1)];}  // jj1 is -1 on the last inner iter, where we use outer incr
#endif
     }
    }
    R zz;  // Return the result after overflow has been corrected
   }
   // retry required, not inplaceable.  Signal the error code to the caller.  If the error is not retryable, set the error message
   if(rc<=NEVM)jsignal(rc);else jt->jerr=(UC)rc;
  }
 }else{z=vasp(a,w,FAV(self)->id,adocv.f,adocv.cv,atype(adocv.cv),rtype(adocv.cv),mf,aawwzk[0],nf,aawwzk[1],fr>>RANKTX,(RANKT)fr); if(!jt->jerr)R z;}  // handle sparse arrays separately.  at this point ak/wk/mf/nf hold acr/wcr/af/wf
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
   _mm256_zeroupper(VOIDARG); \
   __m256i endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-dplen)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
   __m256d acc000; __m256d acc010; __m256d acc100; __m256d acc110; \
   __m256d acc001; __m256d acc011; __m256d acc101; __m256d acc111; \
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
#if (C_AVX&&SY_64) || EMU_AVX
// Do one 2x2 product of length dplen.  Leave results in acc000/010.  dplen must be >0
// av, wv, wv1 are set up
#define ONEPRODAVXD2(label,mid2x2,last2x2) {\
   acc000=_mm256_setzero_pd(); acc010=acc000; acc100=acc000; acc110=acc000; \
   if(dplen<=NPAR)goto label##9; \
   acc001=acc000; acc011=acc000; acc101=acc000; acc111=acc000; \
   I rem=dplen; if(rem>8*NPAR)goto label##8; \
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
   acc000=_mm256_add_pd(acc000,acc001); acc010=_mm256_add_pd(acc010,acc011); acc100=_mm256_add_pd(acc100,acc101); acc110=_mm256_add_pd(acc110,acc111);  \
   label##9: last2x2  \
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
   acc000=_mm256_setzero_pd(); if(dplen<=NPAR)goto label##9; \
   acc010=acc000; acc100=acc000; acc110=acc000; \
   acc001=acc000; acc011=acc000; acc101=acc000; acc111=acc000; \
   I rem=dplen; if(rem>8*NPAR)goto label##8; \
   while(rem>NPAR){ \
    if(rem>4*NPAR) \
     {if(rem>6*NPAR){if(rem>7*NPAR)goto label##7;else goto label##6;}else {if(rem>5*NPAR)goto label##5;else goto label##4;}} \
    else{if(rem>2*NPAR){if(rem>3*NPAR)goto label##3;else goto label##2;}else {if(rem>1*NPAR)goto label##1;else break;}} \
    label##8: mid1x1(7,000)  label##7: mid1x1(6,001)  label##6: mid1x1(5,010)  label##5: mid1x1(4,011)  \
    label##4: mid1x1(3,100)  label##3: mid1x1(2,101)  label##2: mid1x1(1,110)  label##1: mid1x1(0,111)  \
    av+=8*NPAR; wv+=8*NPAR;  \
    if((rem-=8*NPAR)>8*NPAR)goto label##8;  \
   } \
   acc000=_mm256_add_pd(acc000,acc001); acc010=_mm256_add_pd(acc010,acc011); acc100=_mm256_add_pd(acc100,acc101); acc110=_mm256_add_pd(acc110,acc111);  \
   acc000=_mm256_add_pd(acc000,acc010); acc100=_mm256_add_pd(acc100,acc110); \
   acc000=_mm256_add_pd(acc000,acc100);  \
   av-=(NPAR-rem)&-NPAR; wv-=(NPAR-rem)&-NPAR; \
   label##9: last1x1  \
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
 __m256d idreg=_mm256_setzero_pd(); \
 endmask = _mm256_loadu_si256((__m256i*)(validitymask+((-dplen)&(NPAR-1))));  /* mask for 00=1111, 01=1000, 10=1100, 11=1110 */ \
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
#if C_AVX || EMU_AVX
  SUMATLOOP2(D,D,ONEPRODAVXD2(D2,CELL2X2M,CELL2X2L),ONEPRODAVXD1(D1,CELL1X1M,CELL1X1L));
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
 switch(UNSAFE(t)){
 case B01:  // the aligned cases are handled elsewhere, a word at a time
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
 case INT:
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
 case FL:   
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

// obsolete static C sumbf[]={CSTARDOT,CMIN,CSTAR,CPLUSDOT,CMAX,CEQ,CNE,CSTARCO,CPLUSCO,CLT,CLE,CGT,CGE};  // verbs that support +/@:g

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

DF2(jtfslashatg){A fs,gs,y,z;B b,sb=0;C*av,c,d,*wv;I ak,an,ar,*as,at,m,
     n,nn,r,rs,*s,t,wk,wn,wr,*ws,wt,yt,zn,zt;VA2 adocv,adocvf;
 RZ(a&&w&&self);
 an=AN(a); ar=AR(a); as=AS(a); at=AT(a); at=an?at:B01;
 wn=AN(w); wr=AR(w); ws=AS(w); wt=AT(w); wt=wn?wt:B01;
 b=ar<=wr; r=b?wr:ar; rs=b?ar:wr; s=b?ws:as; nn=s[0]; nn=r?nn:1;  // b='w has higher rank'; r=higher rank rs=lower rank s->longer shape  nn=#items in longer-shape arg
 ASSERTAGREE(as,ws,MIN(ar,wr));
 {I isfork=CFORK==FAV(self)->id; fs=FAV(self)->fgh[0+isfork]; gs=FAV(self)->fgh[1+isfork];}   // b=0 if @:, 1 if fork; take fs,gs accordingly
 if(SPARSE&(at|wt)||!an||!wn||2>nn){ R df1(z,df2(y,a,w,gs),fs);}  // if sparse or empty, or just 1 item, do it the old-fashioned way
 rs=MAX(1,rs); PROD(m,rs-1,s+1); PROD(n,r-rs,s+rs); zn=m*n;   // zn=#atoms in _1-cell of longer arg = #atoms in result; m=#atoms in _1-cell of shorter arg  n=#times to repeat shorter arg  (*/ surplus longer shape)
   // if the short-frame arg is an atom, move its rank to 1 so we get the lengths of the _1-cells of the replicated arguments
 y=FAV(fs)->fgh[0]; c=ID(y); d=ID(gs);
 if(c==CPLUS){
  // +/@:g is special if args are boolean, length is integral number of I, and g is boolean or *
  if((((at&wt&(n==1))>(zn&(SZI-1)))||!SY_ALIGN)/* obsolete &&strchr(sumbf,d)*/){   //  relies on B01==1
#define sumbfvalues(w) CCM(w,CGE)+CCM(w,CLE)+CCM(w,CGT)+CCM(w,CLT)+CCM(w,CPLUSCO)+CCM(w,CSTARCO)+CCM(w,CNE)+CCM(w,CEQ)+ \
 CCM(w,CSTARDOT)+CCM(w,CPLUSDOT)+CCM(w,CMIN)+CCM(w,CMAX)+CCM(w,CSTAR)
   CCMWDS(sumbf) CCMCAND(sumbf,cand,d) if(CCMTST(cand,d))R sumatgbool(a,w,d);   // quickly handle verbs that have primitive inverses
// obsolete  if(s=strchr(invf[0],id))R ds(invf[1][s-invf[0]]);     kludge scaf faster
  }
  if(d==CSTAR){
   if(!ar||!wr){  // if either argument is atomic, apply the distributive property to save multiplies
    A z0; z=!ar?tymes(a,df1(z0,w,fs)):tymes(w,df1(z0,a,fs));
    if(jt->jerr==EVNAN)RESETERR else R z;
   }else if(TYPESEQ(at,wt)&&at&B01+FL+(INT*!SY_64))R jtsumattymes(jt,a,w,b,at,m,n,nn,r,s,zn);  // +/@:*
  }
 }
 adocv=var(gs,at,wt); ASSERT(adocv.f,EVDOMAIN); yt=rtype(adocv.cv ); t=atype(adocv.cv);
 adocvf=var(y,yt,yt); ASSERT(adocvf.f,EVDOMAIN); zt=rtype(adocvf.cv);
 sb=yt&(c==CPLUS);  // +/@:g where g produces Boolean.
 if(!(sb||TYPESEQ(yt,zt)))R df1(z,df2(y,a,w,gs),fs);
 if(t){
// obsolete   I bb=t&XNUM;
// obsolete   if(TYPESNE(t,at))RZ(a=bb?xcvt((adocv.cv&VXCVTYPEMSK)>>VXCVTYPEX,a):cvt(t,a));
// obsolete   if(TYPESNE(t,wt))RZ(w=bb?xcvt((adocv.cv&VXCVTYPEMSK)>>VXCVTYPEX,w):cvt(t,w));
  if(TYPESNE(t,at))RZ(a=cvt(t|(adocv.cv&VARGCVTMSKF),a));
  if(TYPESNE(t,wt))RZ(w=cvt(t|(adocv.cv&VARGCVTMSKF),w));
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
  I rc;  // accumulate error returns
  rc=((AHDR2FN*)adocv.f)(n,m,av,wv,zv,jt);  // create first result-cell of g
  DQ(nn-1, av-=ak; wv-=wk; I lrc; lrc=((AHDR2FN*)adocv.f)(n,m,av,wv,yv,jt); rc=lrc<rc?lrc:rc; lrc=((AHDR2FN*)adocvf.f)((I)1,zn,yv,p?zu:zv,p?zv:zu,jt); rc=lrc<rc?lrc:rc; p^=1;);  // p==1 means result goes to ping buffer zv
  if(NEVM<(rc&255)){df1(z,df2(y,a,w,gs),fs);}else{if(rc&255)jsignal(rc); z=p?z1:z;}  // if overflow, revert to old-fashioned way.  If p points to ping, prev result went to pong, make pong the result
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
  jtinplace=(J)((I)jtinplace|JTRETRY);  // indicate that we are retrying the operation.  We must, because jt->jerr is set with the retry code
 }
 // while it's convenient, check for empty result
 jtinplace=(J)((I)jtinplace+(((SGNTO0(awm1)))<<JTEMPTYX));
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

DF2(jtexpn2  ){F2PREFIP; RZ(a&&w); if(unlikely(((((I)AR(w)-1)&SGNIF(AT(w),FLX))<0)))if(unlikely(0.5==DAV(w)[0]))R sqroot(a);  R jtatomic2(jtinplace,a,w,self);}  // use sqrt hardware for sqrt.  Only for atomic w. 
DF2(jtresidue){F2PREFIP; RZ(a&&w); I intmod; if(!((AT(a)|AT(w))&(NOUN&~INT)|AR(a))&&(intmod=IAV(a)[0], (intmod&-intmod)+(intmod<=0)==0))R intmod2(w,intmod); R jtatomic2(jtinplace,a,w,self);}


// These are the unary ops that are implemented using a canned argument
// NOTE that they pass through inplaceability

// Shift the w-is-inplaceable flag to a.  Bit 1 is known to be 0 in any call to a monad
#define IPSHIFTWA (jt = (J)(intptr_t)(((I)jt+JTINPLACEW)&-JTINPLACEA))

// We use the right type of singleton so that we engage AVX loops
#define SETCONPTR(n) A conptr=num(n); A conptr2=zeroionei(n); A conptr3=numvr(n); conptr=AT(w)&INT?conptr2:conptr; conptr=AT(w)&FL?conptr3:conptr;  // for 0 or 1 only
#define SETCONPTR2(n) A conptr=num(n); A conptr3=numvr(n); conptr=AT(w)&FL?conptr3:conptr;   // used for 2, when the only options are INT/FL

F1(jtnot   ){RZ(w); SETCONPTR(1) R AT(w)&B01+SB01?eq(num(0),w):minus(conptr,w);}
F1(jtnegate){RZ(w); SETCONPTR(0) R minus(conptr,w);}
F1(jtdecrem){RZ(w); SETCONPTR(1) IPSHIFTWA; R minus(w,conptr);}
F1(jtincrem){RZ(w); SETCONPTR(1) R plus(conptr,w);}
F1(jtduble ){RZ(w); SETCONPTR2(2) R tymes(conptr,w);}
F1(jtsquare){RZ(w); R tymes(w,w);}   // leave inplaceable in w only  ?? never inplaces
F1(jtrecip ){RZ(w); SETCONPTR(1) R divide(conptr,w);}
F1(jthalve ){RZ(w); if(!(AT(w)&XNUM+RAT))R tymes(onehalf,w); IPSHIFTWA; R divide(w,num(2));} 

static AHDR2(zeroF,B,void,void){memset(z,C0,m*(n^REPSGN(n)));R EVOK;}
static AHDR2(oneF,B,void,void){memset(z,C1,m*(n^REPSGN(n)));R EVOK;}

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
