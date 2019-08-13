/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: "Mathematical" Functions (Irrational, Transcendental, etc.)      */

#include "j.h"
#include "ve.h"

D jtintpow(J jt,D x,I n){D r=1;
 if(0>n){x=1/x; if(n==IMIN){r=x; n=IMAX;} else n=-n;}  // kludge use r=x; n=-1-n;
 while(n){if(1&n)r*=x; x*=x; n>>=1;}
 R r;
}    /* x^n where x is real and n is integral */

D jtpospow(J jt,D x,D y){
 if(0==y)R 1.0;
 if(0==x)R 0<y?0.0:inf;
 if(0<x){
  if(y== inf)R 1<x?inf:1>x?0.0:1.0;
  if(y==-inf)R 1<x?0.0:1>x?inf:1.0;
  R exp(y*log(x));
 }
 if(y==-inf){ASSERT(-1>x,EVDOMAIN); R 0.0;}
 if(y== inf){ASSERT(-1<x,EVDOMAIN); R 0.0;}
 jt->jerr=EWIMAG;
 R 0;
}    /* x^y where x and y are real and x is non-negative */

#define POWXB(u,v)  (v?u:1)
#define POWBX(u,v)  (u?1.0:v<0?inf:!v)
#define POWII(u,v)  intpow((D)u,v)
#define POWID(u,v)  pospow((D)u,v)

APFX(powBI, D,B,I, POWBX )
APFX(powBD, D,B,D, POWBX )
APFX(powIB, I,I,B, POWXB )
APFX(powII, D,I,I, POWII )
APFX(powID, D,I,D, POWID )
APFX(powDB, D,D,B, POWXB )
APFX(powDI, D,D,I, intpow)
APFX(powDD, D,D,D, pospow)
APFX(powZZ, Z,Z,Z, zpow  )

ANAN(cirZZ, Z,Z,Z, zcir  )

static void jtcirx(J jt,I n,I k,D*z,D*y){D p,t;
 NAN0;
 switch(k){
  default: ASSERTW(0,EWIMAG);
  case  0: DQ(n, t=*y++; ASSERTW( -1.0<=t&&t<=1.0, EWIMAG ); *z++=sqrt(1.0-t*t);); break;
  case  1: DQ(n, t=*y++; ASSERTW(-THMAX<t&&t<THMAX,EVLIMIT); *z++=sin(t););        break;
  case  2: DQ(n, t=*y++; ASSERTW(-THMAX<t&&t<THMAX,EVLIMIT); *z++=cos(t););        break;
  case  3: DQ(n, t=*y++; ASSERTW(-THMAX<t&&t<THMAX,EVLIMIT); *z++=tan(t););        break;
  case  4: DQ(n, t=*y++;                                     *z++=t<-1e8?-t:1e8<t?t:sqrt(t*t+1.0););       break;
  case  5: DQ(n, t=*y++;                                     *z++=t<-EMAX2?infm:EMAX2<t?inf:sinh(t););     break;
  case  6: DQ(n, t=*y++;                                     *z++=t<-EMAX2||    EMAX2<t?inf:cosh(t););     break;
// math library tanh is slooooow  case  7: DQ(n, t=*y++;                                     *z++=t<-TMAX?-1:TMAX<t?1:tanh(t););           break;
  case  7: DQ(n, t=*y++;                                     *z++=t<-TMAX?-1:TMAX<t?1:(1.0-exp(-2*t))/(1.0+exp(-2*t)););           break;
  case -1: DQ(n, t=*y++; ASSERTW( -1.0<=t&&t<=1.0, EWIMAG ); *z++=asin(t););       break;
  case -2: DQ(n, t=*y++; ASSERTW( -1.0<=t&&t<=1.0, EWIMAG ); *z++=acos(t););       break;
  case -3: DQ(n,                                             *z++=atan(*y++););    break;
  case -4: DQ(n, t=*y++; ASSERTW(t<=-1.0||1.0<=t,  EWIMAG ); *z++=t<-1e8||1e8<t?t:t==-1?0:(t+1)*sqrt((t-1)/(t+1));); break;
  case -5: p=log(2.0); 
           DQ(n, t=*y++; *z++=1.0e8<t?p+log(t):-7.8e3>t?-(p+log(-t)):log(t+sqrt(t*t+1.0)););               break;
  case -6: p=log(2.0); 
           DQ(n, t=*y++; ASSERTW(          1.0<=t, EWIMAG ); *z++=1.0e8<t?p+log(t):log(t+sqrt(t*t-1.0));); break;
  case -7: DQ(n, t=*y++; ASSERTW( -1.0<=t&&t<=1.0, EWIMAG ); *z++=0.5*log((1.0+t)/(1.0-t)););              break;
  case  9: DQ(n,         *z++=*y++;);           break;    
  case 10: DQ(n, t=*y++; *z++=ABS(t););         break;
  case 11: DQ(n,         *z++=0.0;);            break;
  case 12: DQ(n,         *z++=0<=*y++?0.0:PI;); break;
 }
 NAN1V;
}

AHDR2(cirBD,D,B,D){ASSERTW(n<=1&&1==m,EWIMAG); n^=n>>(BW-1); cirx(n,   (I)*x,z,y);}
AHDR2(cirID,D,I,D){ASSERTW(n<=1&&1==m,EWIMAG); n^=n>>(BW-1); cirx(n,   *x,z,y);}

AHDR2(cirDD,D,D,D){I k=(I)jfloor(0.5+*x);
 ASSERTW(k==*x,EVDOMAIN); 
 ASSERTW(n<=1&&1==m,EWIMAG); // if more than one value, 
 n^=n>>(BW-1);   // convert complementary n to nonneg
 cirx(n,k,z,y);
}


F2(jtlogar2){A z;I t;
 RZ(a&&w); 
 RE(t=maxtype(AT(a),AT(w)));
 if(!(t&XNUM)||jt->xmode==XMEXACT){jt->xmode=XMEXACT; R jtatomic2(JTIPAW,logar1(w),logar1(a),ds(CDIV));}  // better to multiply by recip, but not much, & it makes 0 ^. 0 not fail
 z=rank2ex0(cvt(XNUM,a),cvt(XNUM,w),0L,jtxlog2a); 
 if(z)R z;
 if(jt->jerr==EWIMAG||jt->jerr==EWIRR){RESETERR; jt->xmode=XMEXACT; R divide(logar1(w),logar1(a));}
 R 0;
}
    
F2(jtroot){A z;I t;
 RZ(a&&w);
 RE(t=maxtype(AT(a),AT(w)));
 if(!(t&XNUM))R expn2(cvt(t,w),recip(cvt(t,a)));
 z=rank2ex0(cvt(XNUM,a),cvt(XNUM,w),0L,jtxroota);
 switch(jt->jerr){
  case EWIMAG: RESETERR; R expn2(cvt(CMPX,w),recip(cvt(CMPX,a)));
  case EWRAT: 
  case EWIRR:  RESETERR; R expn2(cvt(FL,  w),recip(cvt(FL,  a)));
  default:     R z;
}}

F1(jtjdot1){R tymes(a0j1,w);}
F2(jtjdot2){R plus(a,tymes(a0j1,w));}
F1(jtrdot1){R expn1(jdot1(w));}
F2(jtrdot2){R tymes(a,rdot1(w));}


F1(jtpolar){RZ(w); R cvt(SPARSE&AT(w)?SFL:FL,df2(v2(10L,12L),w,qq(ds(CCIRCLE),v2(1L,0L))));}

F1(jtrect){A e,z;B b;I r,t;P*wp,*zp;Z c;
 RZ(w); 
 t=AT(w); r=AR(w); RESETRANK;   // Run as infinite rank
 ASSERT(!AN(w)||t&NUMERIC,EVDOMAIN);
 if(t&CMPX){GATV(z,FL,2*AN(w),1+r,AS(w)); AS(z)[r]=2; MC(AV(z),AV(w),AN(z)*sizeof(D)); R z;}
 else if(t&SPARSE){
  b=1&&t&SCMPX;
  GASPARSE(z,b?SFL:t,1,1+r,AS(w)); AS(z)[r]=2;
  wp=PAV(w); zp=PAV(z);
  if(b){e=SPA(wp,e); c=*ZAV(e); ASSERT(FEQ(c.re,c.im),EVSPARSE); SPB(zp,e,scf(c.re));}
  else SPB(zp,e,ca(SPA(wp,e)));
  SPB(zp,a,ca(SPA(wp,a)));
  SPB(zp,i,ca(SPA(wp,i)));
  SPB(zp,x,rect(SPA(wp,x)));
  R z;
 }else R df2(w,num[0],qq(ds(CCOMMA),zeroionei[0]));
}
