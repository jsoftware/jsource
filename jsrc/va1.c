/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Monadic Atomic                                                   */

#include "j.h"
#include "ve.h"


#if BW==64
static AMONPS(floorDI,I,D,
 I rc=0; UI fbits; D mplrs[2]; mplrs[0]=2.0-jt->cct; mplrs[1]=jt->cct-0.00000000000000011; ,
 {if(((fbits=*(UI*)x)&0x7fffffffffffffff)<0x43c0000000000000){I neg=SGNTO0((*(UI*)x)-SGNTO0(*(UI*)x)); *z=(I)(*x*mplrs[neg])-neg;}  // -0 is NOT neg; take everything up to +-2^61
  // if there is a value above 2^61, encode it by setting bit 62 to the opposite of bit 63 (we know bit 62 was 1 originally).  Remember the fact that we need a correction pass.
  // See if the value must be promoted to floating-point in the correction pass.  Return value of EWOVFLOOR0 if there are values all of which fit in an integer, EWOVFLOOR1 if float is required
  else{rc|=EWOVFLOOR0; D d=tfloor(*x); *z=fbits^(SGNTO0(fbits)<<(BW-2)); if(d!=(I)d)rc|=EWOVFLOOR1;} } ,  // we use DQ; i is n-1-reali, ~i = (reali-n+1)-1 = i-n
  R rc?rc:EVOK;
 ; )  // x100 0011 1100 =>2^61
#else
static AMON(floorDI,I,D, {D d=tfloor(*x); *z=(I)d; ASSERTWR(d==*z,EWOV);})
#endif
static AMON(floorD, D,D, *z=tfloor(*x);)
static AMON(floorZ, Z,Z, *z=zfloor(*x);)

#if BW==64
static AMONPS(ceilDI,I,D,
 I rc=0; UI fbits; D mplrs[2]; mplrs[0]=2.0-jt->cct; mplrs[1]=jt->cct-0.00000000000000011; ,
 {if(((fbits=*(UI*)x)&0x7fffffffffffffff)<0x43c0000000000000){I pos=SGNTO0((0-*(UI*)x)-SGNTO0(0-*(UI*)x)); *z=(I)(*x*mplrs[pos])+pos;}  // 0 is NOT pos; take everything up to +-2^61
  // if there is a value above 2^61, encode it by setting bit 62 to the opposite of bit 63 (we know bit 62 was 1 originally).  Remember the fact that we need a correction pass.
  // See if the value must be promoted to floating-point in the correction pass.  Return value of -2 if there are values all of which fit in an integer, -3 if float is required
  else{rc|=EWOVFLOOR0; D d=tceil(*x); *z=fbits^(SGNTO0(fbits)<<(BW-2)); if(d!=(I)d)rc|=EWOVFLOOR1;} } ,  // we use DQ; i is n-1-reali, ~i = (reali-n+1)-1 = i-n
  R rc?rc:EVOK;
 ; )  // x100 0011 1100 =>2^61
#else
static AMON(ceilDI, I,D, {D d=tceil(*x);  *z=(I)d; ASSERTWR(d==*z,EWOV);})
#endif
static AMON(ceilD,  D,D, *z=tceil(*x);)
static AMON(ceilZ,  Z,Z, *z=zceil(*x);)

static AMON(cjugZ,  Z,Z, *z=zconjug(*x);)

static AMON(sgnI,   I,I, I xx=*x; *z=REPSGN(xx)|SGNTO0(-xx);)
static AMON(sgnD,   I,D, *z=((1.0-jt->cct)<=*x) - (-(1.0-jt->cct)>=*x);)
static AMONPS(sgnZ,   Z,Z, , if((1.0-jt->cct)>zmag(*x))*z=zeroZ; else *z=ztrend(*x); , HDR1JERR)

static AMON(sqrtI,  D,I, ASSERTWR(0<=*x,EWIMAG); *z=sqrt((D)*x);)

#if (C_AVX&&SY_64) || EMU_AVX
AHDR1(sqrtD,D,D){
 AVXATOMLOOP(
 __m256d zero; zero=_mm256_setzero_pd();
 __m256d neg; __m256d comp; __m256d anyneg; anyneg=zero;

,
  neg=_mm256_cmp_pd(u,zero,_CMP_LT_OQ); comp=_mm256_sub_pd(zero,u); u=_mm256_blendv_pd(u,comp,neg); // convert to positive; then back to negative
  anyneg=_mm256_or_pd(anyneg,neg);
  u=_mm256_sqrt_pd(_mm256_blendv_pd(u,comp,neg)); comp=_mm256_sub_pd(zero,u); u=_mm256_blendv_pd(u,comp,neg);  // store sqrt, with sign of the original value

 ,
 R (_mm256_movemask_pd(anyneg)&0xf)?EWIMAG:EVOK;  // if there are any negative values, call for a postpass
 )
}

AHDR1(absD,D,D){
 AVXATOMLOOP(
  __m256d absmask; absmask=_mm256_castsi256_pd(_mm256_set1_epi64x (0x7fffffffffffffff));
 ,
  u=_mm256_and_pd(u,absmask);
 ,
  R EVOK;
 )
}

#else
static AMONPS(sqrtD,  D,D, I ret=EVOK; , if(*x>=0)*z=sqrt(*x);else{*z=-sqrt(-*x); ret=EWIMAG;}, R ret;)  // if input is negative, leave sqrt as negative
#if BW==64
static AMON(absD,   I,I, *z= *x&0x7fffffffffffffff;)
#else
static AMON(absD,   D,D, *z= ABS(*x);)
#endif
#endif
static AMON(sqrtZ,  Z,Z, *z=zsqrt(*x);)

static AMON(expB,   D,B, *z=*x?2.71828182845904523536:1;)
static AMONPS(expZ, Z,Z, , *z=zexp(*x); , HDR1JERR)

static AMON(logB,   D,B, *z=*x?0:infm;)
static AMON(logZ,   Z,Z, *z=zlog(*x);)

static AMONPS(absI,   I,I, I vtot=0; , I val=*x; val=(val^REPSGN(val))-REPSGN(val); vtot |= val; *z=val; , R vtot<0?EWOV:EVOK;)
static AMONPS(absZ,   D,Z, , *z=zmag(*x); , HDR1JERR)

static AHDR1(oneB,C,C){memset(z,C1,n); R EVOK;}

extern AHDR1FN expI, expD, logI, logD;

const UA va1tab[]={
 /* <. */ {{{ 0,VB}, {  0,VI}, {floorDI,VI+VIP64}, {floorZ,VZ}, {  0,VX}, {floorQ,VX}}},
 /* >. */ {{{ 0,VB}, {  0,VI}, { ceilDI,VI+VIP64}, { ceilZ,VZ}, {  0,VX}, { ceilQ,VX}}},
 /* +  */ {{{ 0,VB}, {  0,VI}, {    0,VD}, { cjugZ,VZ}, {  0,VX}, {   0,VQ}}},
 /* *  */ {{{ 0,VB}, { sgnI,VI+VIPW}, {   sgnD,VI+VIP64}, {  sgnZ,VZ}, { sgnX,VX}, {  sgnQ,VX}}},
 /* ^  */ {{{expB,VD}, { expI,VD}, {   expD,VD+VIPW}, {  expZ,VZ}, { expX,VX}, {  expD,VD+VDD}}},
 /* |  */ {{{ 0,VB}, { absI,VI+VIPW}, {   absD,VD+VIPW}, {  absZ,VD}, { absX,VX}, {  absQ,VQ}}},
 /* !  */ {{{oneB,VB}, {factI,VD}, {  factD,VD}, { factZ,VZ}, {factX,VX}, { factQ,VX}}},
 /* o. */ {{{  0L,0L}, {   0L,0L}, {     0L,0L}, {    0L,0L}, { pixX,VX}, {    0L,0L}}}, // others handled as dyads
 /* %: */ {{{ 0,VB}, {sqrtI,VD}, {  sqrtD,VD+VIPW}, { sqrtZ,VZ}, {sqrtX,VX}, { sqrtQ,VQ}}},
 /* ^. */ {{{logB,VD}, { logI,VD}, {   logD,VD}, {  logZ,VZ}, { logX,VX}, { logQD,VD}}},
};


static A jtva1(J,A,A);

static A jtva1s(J jt,A w,A self,I cv,VA1F ado){A e,x,z,ze,zx;B c;I n,oprc,t,zt;P*wp,*zp;
 t=atype(cv); zt=rtype(cv);
 wp=PAV(w); e=SPA(wp,e); x=SPA(wp,x); c=t&&TYPESNE(t,AT(e));
 if(c)RZ(e=cvt(t,e));          GA(ze,zt,1,0,    0    ); oprc=((AHDR1FN*)ado)(jt,1L,AV(ze),AV(e));
 if(c)RZ(e=cvt(t,x)); n=AN(x); if(oprc==EVOK){GA(zx,zt,n,AR(x),AS(x)); if(n)oprc=((AHDR1FN*)ado)(jt,n, AV(zx),AV(x));}
 // sparse does not do inplace repair.  Always retry, and never inplace.
 oprc=oprc<0?EWOV:oprc;  //   If a restart is required, turn the result to EWOV (must be floor/ceil)
 if(oprc!=EVOK){
  jt->jerr=(UC)oprc;  // signal error to the retry code, or to the system
  if(jt->jerr<=NEVM)R 0;
  J jtinplace=(J)((I)jt+JTRETRY);  // tell va1 it's a retry
  RZ(ze=jtva1(jtinplace,e,self)); 
  jt->jerr=(UC)oprc; RZ(zx=jtva1(jtinplace,x,self));   // restore restart signal for the main data too
 }else if(cv&VRI+VRD){RZ(ze=cvz(cv,ze)); RZ(zx=cvz(cv,zx));}
 GASPARSE(z,STYPE(AT(ze)),1,AR(w),AS(w)); zp=PAV(z);
 SPB(zp,a,ca(SPA(wp,a)));
 SPB(zp,i,ca(SPA(wp,i)));
 SPB(zp,e,ze);
 SPB(zp,x,zx);
 R z;
}

#define VA1CASE(e,f) (10*(e)+(f))

static A jtva1(J jt,A w,A self){A z;I cv,n,t,wt,zt;VA1F ado;
 UA *u=(UA *)FAV(self)->localuse.lvp[1];
 F1PREFIP;ARGCHK1(w);
 wt=AT(w); n=AN(w); wt=(I)jtinplace&JTEMPTY?B01:wt;
#if SY_64
 VA1 *p=&u->p1[(0x0321000054032100>>(CTTZ(wt)<<2))&7];  // from MSB, we need xxx 011 010 001 xxx 000 xxx xxx   101 100 xxx 011 010 001 xxx 000
#else
 if(wt&SPARSE){wt=AT(SPA(PAV(w),e));}
 VA1 *p=&u->p1[(0x54032100>>(CTTZ(wt)<<2))&7];  // from MSB, we need 101 100 xxx 011 010 001 xxx 000
#endif
 ASSERT(wt&NUMERIC,EVDOMAIN);
 if(!((I)jtinplace&JTRETRY)){
  ado=p->f; cv=p->cv;
 }else{
  I m=REPSGN((wt&XNUM+RAT)-1);   // -1 if not XNUM/RAT
  switch(VA1CASE(jt->jerr,FAV(self)->lc-VA2CMIN)){
   default:     R 0;  // unknown type - error must have come from previous verb
   // all these cases are needed because sparse code may fail over to them
   case VA1CASE(EWOV,  VA2CMIN-VA2CMIN): cv=VD;       ado=floorD;               break;
   case VA1CASE(EWOV,  VA2CMAX-VA2CMIN): cv=VD;       ado=ceilD;                break;
   case VA1CASE(EWOV,  VA2CSTILE-VA2CMIN): cv=VD+VDD;   ado=absD;                 break;
   case VA1CASE(EWIRR, VA1CROOT-VA2CMIN): cv=VD+VDD;   ado=sqrtD;                break;
   case VA1CASE(EWIRR, VA2CEXP-VA2CMIN): cv=VD+VDD;   ado=expD;                 break;
   case VA1CASE(EWIRR, VA2CBANG-VA2CMIN): cv=VD+VDD;   ado=factD;                break;
   case VA1CASE(EWIRR, VA1CLOG-VA2CMIN): cv=VD+(VDD&m); ado=m?(VA1F)logD:(VA1F)logXD; break;
   case VA1CASE(EWIMAG,VA1CROOT-VA2CMIN): cv=VZ+VZZ;   ado=sqrtZ;                break;  // this case remains because singleton code fails over to it
   case VA1CASE(EWIMAG,VA1CLOG-VA2CMIN): cv=VZ+(VZZ&m); ado=m?(VA1F)logZ:wt&XNUM?(VA1F)logXZ:(VA1F)logQZ; break;   // singleton code fails over to this too
  }
  RESETERR;
 }
 if(ado==0)R w;  // if function is identity, return arg
 if(unlikely((-(AT(w)&SPARSE)&-n)<0))R va1s(w,self,cv,ado);  // branch off to do sparse
 // from here on is dense va1
 t=atype(cv); zt=rtype(cv);  // extract required type of input and result
 if(UNSAFE(t&~wt)){RZ(w=cvt(t,w)); jtinplace=(J)((I)jtinplace|JTINPLACEW);}  // convert input if necessary; if we converted, converted result is ipso facto inplaceable.  t is usually 0
 if(ASGNINPLACESGN(SGNIF((I)jtinplace,JTINPLACEWX)&SGNIF(cv,VIPOKWX),w)){z=w; if(TYPESNE(AT(w),zt))MODBLOCKTYPE(z,zt)}else{GA(z,zt,n,AR(w),AS(w));}
 if(!n){RETF(z);} 
 I oprc = ((AHDR1FN*)ado)(jt,n,AV(z),AV(w));  // perform the operation on all the atoms, save result status.  If an error was signaled it will be reported here, but not necessarily vice versa
 if(oprc==EVOK){RETF(cv&VRI+VRD?cvz(cv,z):z);}  // Normal return point: if no error, convert the result if necessary (rare)
 else{
  // There was an error.  If it is recoverable in place, handle the cases here
  // positive result gives error type to use for retrying the operation; negative is 1's complement of the restart point (first value NOT stored)
  // integer abs: convert everything to float, changing IMIN to IMAX+1
  if(ado==absI){A zz=z; if(VIP64){MODBLOCKTYPE(zz,FL)}else{GATV(zz,FL,n,AR(z),AS(z))}; I *zv=IAV(z); D *zzv=DAV(zz); DQ(n, if(*zv<0)*zzv=-(D)*zv;else*zzv=(D)*zv; ++zv; ++zzv;) RETF(zz);}
  // float sqrt: reallocate as complex, scan to make positive results real and negative ones imaginary
  if(ado==sqrtD){A zz; GATV(zz,CMPX,n,AR(z),AS(z)); D *zv=DAV(z); Z *zzv=ZAV(zz); DQ(n, if(*zv>=0){zzv->re=*zv;zzv->im=0.0;}else{zzv->im=-*zv;zzv->re=0.0;} ++zv; ++zzv;) RETF(zz);}
  // float floor: unconvertable cases are stored with bit 63 and bit 62 unlike; restore the float value by setting bit 62.
  // if bit 0 of oprc is 1, values must be converted to float; if 0, they can be left as int
  if(VIP64&&ado==floorDI){A zz=z;
   if(!(oprc&1)){I *zv=IAV(z); DQ(n, I bits=*(I*)zv; if((bits^SGNIF(bits,BW-2))<0){bits|=0x4000000000000000; *zv=(I)tfloor(*(D*)&bits);} ++zv;)}  // convert overflows, turn back to integer
   else{MODBLOCKTYPE(zz,FL) I *zv=IAV(z); D *zzv=DAV(zz); DQ(n, I bits=*(I*)zv++; if((bits^SGNIF(bits,BW-2))>=0)*zzv=(D)bits;else{bits|=0x4000000000000000; *zzv=tfloor(*(D*)&bits);} ++zzv;)}   // force float conversion
   RETF(zz);
  }
  // float ceil, similarly
  if(VIP64&&ado==ceilDI){A zz=z;
   if(!(oprc&1)){I *zv=IAV(z); DQ(n, I bits=*(I*)zv; if((bits^SGNIF(bits,BW-2))<0){bits|=0x4000000000000000; *zv=(I)tceil(*(D*)&bits);} ++zv;)}  // convert overflows, turn back to integer
   else{MODBLOCKTYPE(zz,FL) I *zv=IAV(z); D *zzv=DAV(zz); DQ(n, I bits=*(I*)zv++; if((bits^SGNIF(bits,BW-2))>=0)*zzv=(D)bits;else{bits|=0x4000000000000000; *zzv=tceil(*(D*)&bits);} ++zzv;)}   // force float conversion
   RETF(zz);
  }

  // not recoverable in place.  If recoverable with a retry, do the retry; otherwise fail.  Caller will decide; we return error indic
  // we set the error code from the value given by the routine, except that if it involves a restart it must have been ceil/floor that we couldn't restart - that's a EWOV
  oprc=oprc<0?EWOV:oprc; if(oprc>NEVM)RESETERR; jt->jerr=(UC)oprc;  // if this is going to retry, clear the old error text; but leave the error value
  R 0;
 }
}

// Consolidated entry point for ATOMIC1 verbs.
// This entry point supports inplacing
DF1(jtatomic1){A z;
 F1PREFIP;ARGCHK1(w);
 I awm1=AN(w)-1;
 // check for singletons
 if(!(awm1|(AT(w)&(NOUN&UNSAFE(~(B01+INT+FL)))))){  // len=1 andbool/int/float
  z=jtssingleton1(jtinplace,w,self);
  if(z||jt->jerr<=NEVM){RETF(z);}  // normal return, or non-retryable error
  // if retryable error, fall through.  The retry will not be through the singleton code
  jtinplace=(J)((I)jtinplace|JTRETRY);  // indicate that we are retrying the operation
 }
 // while it's convenient, check for empty result
 jtinplace=(J)((I)jtinplace+(((SGNTO0(awm1)))<<JTEMPTYX));
 // Run the full dyad, retrying if a retryable error is returned
 while(1){  // run until we get no error
  z=jtva1(jtinplace,w,self);  // execute the verb
  if(z||jt->jerr<=NEVM){RETF(z);}   // return if no error or error not retryable
  jtinplace=(J)((I)jtinplace|JTRETRY);  // indicate that we are retrying the operation
 }
}

DF1(jtpix   ){F1PREFIP; ARGCHK1(w); if(XNUM&AT(w)&&(jt->xmode==XMFLR||jt->xmode==XMCEIL))R jtatomic1(jtinplace,w,self); R jtatomic2(jtinplace,pie,w,ds(CSTAR));}
