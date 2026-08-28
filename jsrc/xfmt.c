/* Copyright (c) 1990-2026, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: 8!:x formatting stuff                                            */

#include "j.h"
#include "x.h"
#include "vcomp.h" /* for TLT & friends */
#include "dtoa.h"
#include "jgmp.h"

#define MAXDVAL 17  // max # decimal places allowed
static const D pnpwrs[MAXDVAL+1][2]={{1e0, 1e-0}, {1e1, 1e-1}, {1e2, 1e-2}, {1e3, 1e-3}, {1e4, 1e-4},
  {1e5, 1e-5}, {1e6, 1e-6}, {1e7, 1e-7}, {1e8, 1e-8}, {1e9, 1e-9}, 
  {1e10, 1e-10}, {1e11, 1e-11}, {1e12, 1e-12}, {1e13, 1e-13}, {1e14, 1e-14},
  {1e15, 1e-15}, {1e16, 1e-16}, {1e17, 1e-17}}; 
static const C*pp="<({[";
static const C*qq=">)}]";

#define SUBe subs[0] /* e */
#define SUBc subs[1] /* , */
#define SUBd subs[2] /* . */
#define SUBm subs[3] /* - */
#define SUBs subs[4] /* * */

/* bit flags vector:  2b0001 1111                                            */
/*                      \|/| |||+-- 1 iff exponential notation               */
/*                       | | ||+--- 1 iff _                                  */
/*                       | | |+---- 1 iff __                                 */
/*                       | | +----- 1 iff _.                                 */
/*                       | +------- 1 iff considered to be zero              */
/*                       +--------- unused, should be 0                      */

#define BITSe  1
#define BITS_  2
#define BITS__ 4
#define BITS_d 8
#define BITSf  14
#define BITSz  16

#define NMODVALS 11

#define mC  (mods&0x1000)
#define mL  (mods&0x0800)
#define mK  (mods&0x0400)
#define mJ  (mods&0x0200)
#define mI  (mods&0x0100)
#define mB  (mods&0x0080)
#define mD  (mods&0x0040)
#define mMN (mods&0x0030)
#define mPQ (mods&0x000c)
#define mR  (mods&0x0002)
#define uK  (u[1])  // unused
#define uJ  (u[2])  // unused
#define uI  (u[3])
#define uB  (u[4])
#define uD  (u[5])
#define uM  (u[6])
#define uN  (u[7])
#define uP  (u[8])
#define uQ  (u[9])
#define uR  (u[10])
#define uS  (u[11])


static F1(jtfmtbfc){F12IP;A*u,z;B t;C c,p,q,*s,*wv;I i,j,m,n;
 ARGCHK1(w); 
 if((C2T+C4T)&AT(w))RZ(w=uco2(num(5),w))
 ASSERT(1>=AR(w),EVDOMAIN);
 n=AN(w); wv=CAV(w); t=0; m=1; j=0;
 for(i=0;i<n;++i){
  c=wv[i]; 
  if(t){ASSERT(c!=p,EVDOMAIN); if(c==q)t=0;}
  else if(c==',')++m; 
  else if(s=strchr(pp,c)){t=1; p=c; q=qq[s-pp];}
 }
 ASSERT(!t,EVDOMAIN);
 GATV0(z,BOX,m,1<m); u=AAVn(1<m,z);
 for(i=0;i<n;++i){
  c=wv[i]; 
  if(t){if(c==q)t=0;}
  else if(c==','){RZ(*u++=incorp(str(i-j,wv+j))); j=i+1;}
  else if(s=strchr(pp,c)){t=1; q=qq[s-pp];}
 }
 RZ(*u=incorp(str(n-j,wv+j)));
 R z;
} /* format phrases: boxed from char */

// install commas into *x, whose length l is big enough.  d is the number of decimal digits.  Discard non-digits (including sign)
static B jtfmtcomma(J jt, C *x, I l, I d, C *subs) {C *v,*u;I j,n,c;
 n=l-(c=(l-!!d-d)>>2); u=x+l-1;  // n=length of input before decimal point (remove 1/4 of the spaces, which are reserved for commas) u=output pointer
 if(v=memchr(x, SUBd, n)){j=n-(v-x); u-=j; memmove(u+1,v,j); v--;} else v=x+n-1;   // v=input pointer, positioned just left of the decimal point
 j=0;
 DQ(v-x+1, if((C)(*v-'0')<=(C)('9'-'0')){if(j==3){*u--=SUBc; j=0;} j++;} *u--=*v--;);
 R 1;
}

// b is the bitflags for the floating-point value w
// result is number of decimal places needed to show all significance
static I jtdpone(J jt, B bits, D w){D t;
 if(bits&BITSf) R 0;   // if field is formatted as f type, 0 decimal places
 w=ABS(w);
 if(bits&BITSe) w/=pow(10,tfloor(log10(w)));   // if exponent form, shift significance to be in [1,10)
 DO(10, t=jround(pnpwrs[i][0]*w)/pnpwrs[i][0]; if(TEQ(t,w)) R i; );  // count number of significant decimal places, return it
 R 9;  // never more than 9
}

// scan a for [www[.ddd]}
// returns 0 if error, and sets *w and *d.  If both omitted, return -1/-1; if only www omitted, error;
//  if no '.', return -1/www; if both given (even if d empty) return www/ddd
static B jtwidthdp(J jt, A a, I *w, I *d){
 ARGCHK1(a); RZ(w); RZ(d);
 #define digdotvalues(w) CCM(w,'0')+CCM(w,'1')+CCM(w,'2')+CCM(w,'3')+CCM(w,'4')+CCM(w,'5')+CCM(w,'6')+CCM(w,'7')+CCM(w,'8')+CCM(w,'9')+CCM(w,'.')
 CCMWDS(digdot)
 I remchars=AN(a); C *v=CAV(a);  // number of chars left, pointer to current
 // advance to first digit/.
 for(;remchars;++v,--remchars){C vv=*v; CCMCAND(digdot,cand,vv) if(CCMTST(cand,vv))break;}
 // if none, exit with neither field
 if(!remchars){*w=*d=-1; R 1;}
 // if ., error
 ASSERT(*v!='.',EVDOMAIN);
 // consume digits, creating w value
 I t; for(t=0;remchars;++v,--remchars){C vv=*v; if(!BETWEENC(vv,'0','9'))break; t=(vv-'0')+10*t;}
 *w=*d=t;  // set integer part into both result fields
 if(remchars&&*v=='.'){
  // . given. skip it and consume the following digits
  ++v,--remchars; for(t=0;remchars;++v,--remchars){C vv=*v; if(!BETWEENC(vv,'0','9'))break; t=(vv-'0')+10*t;}
  *d=t;
 }else *w=-1;   // flag to indicate omitted d (!)
 // verify no remaining digits/. in field
 for(;remchars;++v,--remchars){C vv=*v; CCMCAND(digdot,cand,vv) ASSERT(!CCMTST(cand,vv),EVDOMAIN);}
 ASSERT(BETWEENC(*d,-1,MAXDVAL), EVDOMAIN);  // error if too many decimal places requested
 R 1;
} /* width and decimal places */

/* parse a single boxed format phrase                                        */
/* result:   (width,decimal_places,modifiers);k;j;i;b;d;m;n;p;q;r;s                */
/* where modifiers are 2b1 1111 1111 1111                                        */
/* corresponding to      c lkji bdmn pqrs                                        */
/* k j i b d m n p q r s are the strings to use; s is '' if all defaults           */

static F1(jtfmtparse){F12IP;A x,z,*zv;B ml[2+NMODVALS],mod,t;C c,*cu="srqpnmdbijklc",*cu1="?kjibdmnpqrs",d,*s,*wv;
     I fb,i,j,mi,n,n1,p,q,vals[3]={-1,-1,0};
 ARGCHK1(w);
 w=C(AAV(w)[0]); n=AN(w);
 GAT0(z,BOX,1+NMODVALS,1); zv=AAV1(z); 
 DO(NMODVALS, zv[1+i]=mtv;);
 if(n&&(C2T+C4T)&AT(w))RZ(w=uco2(num(5),w));
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(!n||LIT&AT(w),EVDOMAIN); 
 wv=CAV(w); n1=1+n; t=c=0; fb=0; mi=-1; mvc(sizeof(ml),ml,MEMSET01LEN,MEMSET01);
 for(i=0;i<n1;++i){
  mod=t^1; c=wv[i]; c=i==n?0:c;
  if(i==n)ASSERT(!t,EVDOMAIN)
  else{
   if(t){ASSERT(c!=p,EVDOMAIN); if(c==q)t=0;}
   else if(s=strchr(pp,c)){mod=0; t=1; p=c; q=qq[s-pp];}
  }
  if(!mod)continue;  /* 1==mod iff not in modifier text <xx>; i.e. c is a modifier letter or 0-9 */
  if(0<=mi){
   d=wv[mi];
   ASSERT(s=strchr(cu,d),EVDOMAIN);
   j=s-cu; ASSERT(ml[j],EVDOMAIN); ml[j]=0; fb|=(I)1<<j; 
   if(s=strchr(cu1,d)){if(i-mi>3)RZ(zv[s-cu1]=incorp(str(i-mi-3,wv+mi+2)));}else ASSERT(1==i-mi,EVDOMAIN);
  }
  mi=i;
  if(BETWEENC(c,'0','9')){RZ(widthdp(str(n-i,wv+i),vals,vals+1)); break;} 
 }
 if(mtv!=zv[NMODVALS]){C*cu="e,.-*",*cv,subs[5];
  x=zv[NMODVALS]; n=AN(x); cv=CAV(x); MC(subs,cu,5L); mvc(5L,ml,MEMSET01LEN,MEMSET01);
  ASSERT(0==(n&1)&&10>=n,EVDOMAIN);
  DQ(n>>1, ASSERT(s=strchr(cu,*cv++),EVDOMAIN); j=s-cu; ASSERT(ml[j],EVDOMAIN); ml[j]=0; subs[j]=*cv++;);
  RZ(zv[NMODVALS]=incorp(str(5L,subs)));
 }
 vals[2]=fb; RZ(*zv=incorp(vec(INT,3,vals)));
 R z;
}

typedef union u_DI8_tag { I8 i; D d; } DI8;

// d is #decimal places, y is |value|.  Return value rounded to d places
static D jtroundID(J jt,I d,D y){D f,q,c,h;DI8 f8,q8,c8;
 q=pnpwrs[d][0]*y; if(q<1) h=2; else h=0; q+=h;  // shift significance to left of decimal point.  h is extra significance: set to 2 if no other significance so subsequent compares are tolerant
 f=jfloor(q); c=-jfloor(-q);  // take floor and ceil of q
 if(f==c) R (c-h)/pnpwrs[d][0];   // if they are the same, they are the rounded result
 ASSERTSYS(f<=q&&q<=c, "roundID: fqc");
 f8.d=f;q8.d=q;c8.d=c;
 ASSERTSYS(0<=f8.i&&0<=q8.i&&0<=c8.i, "roundID: sign");  // ?? signs?? y is always positive or +0
 if(q8.i-f8.i >= c8.i-q8.i-1) R (c-h)/pnpwrs[d][0];  // if floor & ceil differ, return the closer
 else                         R (f-h)/pnpwrs[d][0];
} /* round a number in not in exponential notation */

static D jtafzrndID(J jt,I dp,D y){R SGN(y)*roundID(dp,ABS(y));}
         /* round-to-nearest, solve ties by rounding Away From Zero */

static D jtexprndID(J jt, I d, D y){I e,s;D f,q,c,x1,x2;DI8 f8,y8,c8;
 s=SGN(y); e=-(I)jfloor(log10(y=ABS(y)));  // s=sign, e=-1-#digits above decimal point
 // we want to multiply y by 10^-log(y), but that power may overflow.  So we split
 // the power into two halves, and multiply one at a time
 e+=d; x1=pow(2,(D)e); x2=pow(5,(D)e); 
 q=x2*y;
 q*=x1; /* avoid overflow to Infinity */
 f=jfloor( q)/x1; f/=x2;
 c=(-jfloor(-q))/x1; c/=x2;

 if(f==c) R s*c;
 /*ASSERTSYS(f<=y && y<=c, "exprndID: fyc");*/ /* why does this fail? */
 f8.d=f; y8.d=y; c8.d=c;
 if(y8.i-f8.i >= c8.i-y8.i-1) R s*c; else R s*f;
} /* afzrnd for numbers in exponential notation */

// format absolute value of integer iw into the LAST columns of *x which has length m.  dp is number of reserved decimal digits
static B jtsprintfI(J jt, C *x, I m, I dp, I iw, C *subs) {I /*r,*/ g;
 x+=m-1;   // start at last digit
 DQ(dp, *x--='0';); if(dp) *x--=SUBd; /*r = dp + !!dp;*/  // if there are decimal places, make them all 0 and add the decimal point
 g=SGN(iw); UI uiw=ABS(iw);
 while(uiw){ *x--='0'+(C)(uiw%10); uiw/=10; /*r++;*/ }   // format absolute value
 if(g==0) { *x--='0'; /*r++; */ }  // if input 0, give 1 digit of 0
 R 1;
}

static B jtsprintfnD(J jt, C *x, I m, I dp, D dw, C *subs) {I nd;int decpt, sign;
 if(dw==0) { mvc( m,x,1,iotavec-IOTAVECBEGIN+'0'); if(dp) x[1]=SUBd; R 1; }
 if(ABS(dw) < 1) nd=dp; else nd=m-!!dp;
 RZ(ecvt(dw,nd,&decpt,&sign,x));
 if(decpt > 0) {
  memmove(x+decpt+1, x+decpt, dp);
  if(dp) x[decpt]=SUBd;
 } else {
  memmove(x+2-decpt, x, dp+decpt); 
  mvc( 2-decpt,x,1,iotavec-IOTAVECBEGIN+'0');
  if(dp) x[1]=SUBd;
 }
 R 1;
}

static B jtsprintfeD(J jt, C *x, I m, I dp, D dw, C *subs) {I y,y0;int decpt,sign;
 RZ(ecvt(dw,1+dp,&decpt,&sign,x+!!dp));
 if(dp) { x[0]=x[1]; x[1]=SUBd; }
 x += 1+!!dp+dp; *x++=SUBe; 
 if(decpt<0) *x++=SUBm; decpt--; decpt=ABS(decpt);
 ASSERTSYS(1000>decpt, "jtsprintfeD: decpt");
 y0=decpt/100; if(y0)    *x++='0'+(C)y0; decpt%=100;
 y =decpt/10 ; if(y||y0) *x++='0'+(C)y ; decpt%=10;
 y =decpt    ;           *x++='0'+(C)y ; 
 R 1;
}

/* the output of jtfmtprecomp looks like this                                */
/*   z =: base ; strings ; length ; bitflags                                */
/* given the following:                                                      */
/*   nc =: {: $ y           NB. the number of columns                       */
/*   nf is the number of format phrases                                      */
/* if. 1 < nf do.                                                            */
/*  base   =:(nf,4)$width_0, decimal_places_0, modifiers_0, col_width_0, ... */
/* else.                                                                     */    
/*  base   =:(3+nc)$width, dec_places, mods, col_width_0, col_width_1, ...   */
/* end.                                                                      */
/* strings =:(nf,11)$k0;j0;i0;b0;d0;m0;n0;p0;q0;r0;s0; b1;d1; ...                      */
/* length  =:($y)$number_length_0_0, number_length_0_1, ... (in w, not      */
/* bitflags=:($y)$bitflags_0_0, bitflags_0_1, ...            column order)  */

#define mods_coldp 0x40000000 /* applied to modifiers when we're computing this columns # of decimal places */

static F2(jtfmtprecomp) {F12IP;A*as,base,fb,len,strs,*u,z;B*bits,*bw;D dtmp,*dw;
     I d,i,*ib,imod,*iw,*iv,maxl,mods,n,nB,nD,nMN,nPQ,nI,nc,nf,*s,wr,*ws,wt;
 ARGCHK2(a,w); 
 nf=AS(a)[0]; nf=1==AR(a)?1:nf; n=AN(w); wt=AT(w); wr=AR(w); ws=AS(w); SHAPEN(w,wr-1,nc);   // nf=#cells, nc=length of 1-cell (# columns), n=#atoms in w
 ASSERT(ISDENSETYPE(wt,B01+INT+FL+XNUM), EVDOMAIN);
 if(1<nf){GATV0(base,INT,nf*4,2); s=AS(base); *s++=nf; *s=4;}else GATV0(base,INT,3+nc,1);   // allocate result area, either a table or a list
 GATV0(strs,BOX,nf*NMODVALS,2); s=AS(strs); *s++=nf; *s=NMODVALS;    // strings, one per modifier per format phrase
 GATV(len, INT,n,wr,ws);    // len=length in bytes of each number, or # decimal places
 GATV(fb,  B01,n,wr,ws); mvc(n,BAVn(wr,fb),MEMSET00LEN,MEMSET00);   // fb=bitflags for each number, init to 0
 GAT0(z,BOX,4,1); u=AAV1(z); *u++=incorp(base); *u++=incorp(strs); *u++=incorp(len); *u++=incorp(fb);    // allocate result = base ; strs ; len ; fb
 ib=AV(base); as=AAV(strs); u=AAV(a);   // init scan pointers  ib=base, as=strs, u=a (the format phrases)
 if(1==nf){MC(ib,AV(C(*u)),SZI*3); mvc(SZI*nc,ib+3,MEMSET00LEN,MEMSET00); DO(NMODVALS, *as++=incorp(C(u[i+1]));)}  // 1 fp: move it (width, decplaces, mods), set col widths to 0;  then move strs
 else DQ(nf, MC(ib,AV(C(*u)),SZI*3); ib[3]=0; ib+=4; DO(NMODVALS, *as++=incorp(C(u[1])); ++u;) ++u; )  // >1 fp: move each, and move strs
 bits=BAV(fb);  // init bits=ptr to bitflags for each number
 switch(CTTZ(wt)) {
 case B01X:
  bw=BAV(w); ib=AV(base);
  DQ(n, *bits|=BITSz*!*bw; bits++; bw++;);  // BITSe, BITS_, BITS__, and BITS_d are 0; set BITSz if 0
  DQ(nf, if(ib[1]==-1)ib[1]=0; ib+=4;);     /* boolean always has 0 decimal places: */
  break;
 case INTX: case XNUMX:

  iw=AV(w);
  iv=AV(len); /* use len to store dp */
  imod=1;  // row index counter
  for(i=0;i<n;++i){
   ib+=4; --imod; ib=(imod==0)?AV(base):ib; imod=(imod==0)?nf:imod;  // use the entries in base cyclically, repeating every nf loops
   d=ib[1];   // d=#decimal places requested
   /* BITS_, BITS__, and BITS_d are 0 */
   if(wt&INT){
    if(!*iw){*bits|=BITSz; *iv=0;    // set BITSz if value 0, and clear field len
    }else if(d==-1){if(ABS((D)*iw)>2000000000.){*bits|=BITSe; *iv=dpone(*bits,(D)*iw);}else *iv=0;}  // if exponent form allowed, switch to it (by setting BITSe) if |value|>2e9, & save #dps needed as exp form
   }else{X XN=*(X*)iw;  // must be XNUM.  the value, as X
    if(ISX0(XN))*bits|=BITSz;    // set BITSz if value 0
    else if(d==-1){   // exponent form possible, which would give us decimal places
     ASSERT(ABS(AS(XN)[0])<=(1023>>LGBW),EVLIMIT)  // GMP crashes if value exceeds float range
     mpX(XN); D h=jmpz_get_d(mpXN);   // high & low parts as D
     if(ABS(h)>2000000000.){*bits|=BITSe; *iv=dpone(*bits,h);}  // if exponent form allowed, switch to it (by setting BITSe) if |value|>2e9, & save #dps needed as exp form
    }
   }
   bits++; iw++; iv++;
  }
  imod=1;  // reset to beginning of row
  // pass over the numbers again; extend the # fraction bits if needed
  iv=AV(len); 
  for(i=0;i<n;++i){
   ib+=4; --imod; ib=(imod==0)?AV(base):ib; imod=(imod==0)?nf:imod;  // use the entries in base cyclically, repeating every nf loops
   if(ib[1]==-1) ib[2] |= mods_coldp;    // if 'enough' decimal places requested, either as part of the field spec or because exponent form allowed...
   if(ib[2]&mods_coldp && *iv>ib[1]) ib[1]=*iv;    // ...increase the number of decimal places if needed to hold the significance
   iv++; 
  }
  break;
 case FLX:
  dw=DAV(w);
  iv=AV(len); /* use len to store dp */
  imod=1;  // row size counter
  for(i=0;i<n;++i){
   ib+=4; --imod; ib=(imod==0)?AV(base):ib; imod=(imod==0)?nf:imod;
   d=ib[1];
   dtmp=ABS(*dw);    
   if(d==-1) *bits |= BITSe*(TNE(0,dtmp) && (TLT(dtmp,1e-9)||TLT(2e9,dtmp)));
   *bits |= BITS_ *!memcmpne(dw, &inf , SZD)+ 
            BITS__*!memcmpne(dw, &infm, SZD)+
            BITS_d*_isnan(*dw); 
   if(d==-1) *iv = dpone(*bits,*dw);
   else *bits |= BITSz*(TEQ(*dw, 0) || (!(*bits&BITSf+BITSe) && TLT(dtmp, pnpwrs[d][1]/2)));
   bits++; dw++; iv++; 
  }
  iv=AV(len);
  imod=1;  // row size counter
  for(i=0;i<n;++i){
   ib+=4; --imod; ib=(imod==0)?AV(base):ib; imod=(imod==0)?nf:imod;
   if(ib[1]==-1) ib[2] |= mods_coldp;
   if(ib[2]&mods_coldp&&*iv>ib[1]) ib[1]=*iv;
   ASSERTSYS(BETWEENC(ib[1],0,17), "jtfmtprecomp: d oob");
   iv++; 
  }
  bits=BAV(fb); dw=DAV(w);
  imod=1;  // row size counter
  DO(n, 
   ib+=4; --imod; ib=(imod==0)?AV(base):ib; imod=(imod==0)?nf:imod;
   d=ib[1]; 
   if(ib[2]&mods_coldp){
    *bits |= BITSz*(TEQ(*dw, 0) || (!(*bits&BITSf+BITSe) && TLT(ABS(*dw), pnpwrs[d][1]/2)));
   }
   bits++; dw++; 
  );
  break;
 }

 iv=AV(len); bits=BAV(fb);
 iw=AV(w); dw=DAV(w); maxl=-1;
 imod=1;  // row size counter
 I imodc=-1;  // 
 for(i=0;i<n;i++) {  // go through the values figuring the length needed for each value
  ib+=4; u+=NMODVALS; --imod; ib=(imod==0)?AV(base):ib; u=(imod==0)?AAV(strs)-1:u; imod=(imod==0)?nf:imod;
  ++imodc; imodc=(imodc==nc)?0:imodc;  // imodc is i%nc
  nB=AN(C(uB)); nD=AN(C(uD)); nMN=AN(C(uM))+AN(C(uN)); nPQ=AN(C(uP))+AN(C(uQ)); nI=AN(C(uI));
  d=ib[1]; mods=ib[2]; 
  if(*bits&BITSf) { if(mI&&*bits&BITS__)*iv=nI; else if(mD) *iv=nD; else *iv=2-!!(*bits&BITS_); }
  else if(*bits&BITSz) { 
   if(mB) *iv=nB; 
   else {
    *iv=1+d+!!d;
    if(mPQ) (*iv) += nPQ;
   }
  } else if(mI && wt&INT && *iw==IMIN){*iv=nI;  // if we recognize NUL, do so regardless of output precision
  } else if(*bits&BITSe) {  // exponent form
   if(FL&wt) {D ad=ABS(*dw);  // value
    I ep=1+3; ep=ad>=1e-99?1+2:ep; ep=ad>=1?1:ep; ep=ad>=1e10?2:ep; ep=ad>=1e100?3:ep;  // # exponent places needed
    *iv=2+!!d+d+ep;  // store total # columns needed
    if(*dw < 0) { if(mMN) (*iv)+=nMN; else (*iv)++; }
    else if(mPQ) (*iv)+=nPQ;
   } else {D ad, dv;  // exponent form originating from integer (possibly XNUM).  ad is value converted to float
    if(wt&INT)ad=ABS(dv=(D)*iw);
    else{X XN=*(X*)iw;  // must be XNUM.  the value, as X
     ASSERT(ABS(AS(XN)[0])<=(1023>>LGBW),EVLIMIT)  // GMP crashes if value exceeds float range
     mpX(XN); ad=ABS(jmpz_get_d(mpXN));   // as D
    }
    I ep=1+3; ep=ad>=1e-99?1+2:ep; ep=ad>=1?1:ep; ep=ad>=1e10?2:ep; ep=ad>=1e100?3:ep;  // # exponent places needed
    *iv=2+!!d+d+ep;  // store total # columns needed
    if(dv < 0) { if(mMN) (*iv)+=nMN; else (*iv)++; }
    else if(mPQ) (*iv) += nPQ;
   }
  } else {  // must be B01/FL/INT/XNUM type but not exponent form, count number of significant digits
   if(B01&wt) *iv=1+!!d+d;
   else {   // INT/FL/XNUM
    B dneg;  // set if value < 0
    if(!(wt&XNUM)){  // INT/FL
     if(INT&wt) dtmp=(D)*iw; else dtmp=*dw;
     *iv=(I)jfloor(log10(roundID(d,MAX(ABS(dtmp),1))));  // number of digits needed minus 1
     dneg=dtmp<0;
    }else{   // XNUM
     C*s=SgetX(*(A*)iw); // base 10 representation
     dneg=s[0]=='-'; s+=dneg; *iv=strlen(s)-1;  // remember if neg & skip sign; get len of absolute value minus 1
    }
    if(mC) (*iv)+=(*iv)/3;
    (*iv)+=1+!!d+d;
    if(dneg && mMN) (*iv)+=nMN;
    else if(dneg) (*iv)++;
    else if(mPQ) (*iv)+=nPQ;
   }
  }
  ASSERTSYS(0 <= *iv, "jtfmtprecomp: cell length");
  if(*iv > maxl) maxl=*iv;
  if(1<nf && *iv > ib[3]) ib[3]=*iv;
  else if(1==nf && *iv > ib[3+imodc]) ib[3+imodc]=*iv;
  bits++; dw++; iw++; iv++;
 }
 ib=AV(base);
 if(1==nf){if(!ib[0])ib[0]=maxl;}else DQ(nf, if(ib[0]==0)ib[0]=ib[3]; ib+=4;);
 R z;
} /* format: precomputation to separate the group and column concept */

/* a is jtfmtprecomp result */
/* w is argument to format, but with BO1, INT, FL, or XNUM type. */
static A jtfmtallcol(J jt, A a, A w, I mode) {A *a1v,base,fb,len,strs,*u,v,x;
    B *bits,*bv;C*cB,*cD,*cM,*cN,*cP,*cQ,*cR,*cI,*cv,**cvv,*cx,*subs;D dtmp,*dv;
    I coll,d,g,h,i,*ib,imod,*iv,*il,j,k,l,m,mods,nB,nD,nM,nN,nP,nQ,nR,nI,n,nc,nf,t,wr,*ws,y,zs[2];
 ARGCHK1(a); u=AAV(a); base=*u++; strs=*u++; len=*u++; fb=*u++; u=0; subs=0;  // extract components: len->lengths of the values
 ARGCHK1(w); n=AN(w); t=AT(w); wr=AR(w); ws=AS(w); SHAPEN(w,wr-1,nc); 
 ASSERT(ISDENSETYPE(t,B01+INT+FL+XNUM), EVDOMAIN);

 nf=1==AR(base)?1:AS(base)[0];
 switch(mode){
 case 0:
  GATV(x, BOX, n, wr, ws); a1v=AAVn(wr,x); il=AV(len);
  ib=AV(base);
  imod=1;
  DO(n, 
   ib+=4; --imod; ib=(imod==0)?AV(base):ib; imod=(imod==0)?nf:imod;
   if(0<ib[0]) GATV0(*a1v, LIT, ib[0], 1)
   else GATV0(*a1v, LIT, *il, 1) 
   incorp(*a1v); mvc( AN(*a1v),CAV(*a1v),1,iotavec-IOTAVECBEGIN+' '); 
   a1v++; il++; 
  );
  break;
 case 1:
  GATV0(x, BOX, nc, 1); a1v=AAV1(x); ib=AV(base); zs[0]=prod(wr-1,ws);
  GATV0(v, LIT, nc*SZA, 1); cvv=(C**)AV1(v); 
  DO(nc,
   if(0<ib[0]) zs[1]=ib[0]; 
   else zs[1]=ib[3+(1<nf?0:i)]; 
   GATVR(*a1v, LIT, zs[0]*zs[1], 2, zs);
   incorp(*a1v); mvc( AN(*a1v),CAV2(*a1v),1,iotavec-IOTAVECBEGIN+' '); 
   *cvv++=CAV2(*a1v);
   a1v++; if(1<nf) ib+=4; 
  );
  cvv=(C**)AV(v);
  break;
 case 2:
  coll=0; ib=AV(base);
  DO(nc, if(0<ib[0]) coll+=ib[0]; else coll+=ib[3+(1<nf?0:i)];
         if(1<nf) ib+=4; );
  zs[0]=prod(wr-1,ws); zs[1]=coll;
  GATVR(x, LIT, zs[0]*zs[1], 2, zs);
  mvc( AN(x),CAV2(x),1,iotavec-IOTAVECBEGIN+' ');
  break;
 default: ASSERTSYS(0, "jtfmtallcol: mode");
 }
 
 a1v=AAV(x); cx=CAV(x);  il=AV(len); bits=BAV(fb);
 bv=BAV(w); iv=IAV(w); dv=DAV(w);
 
 u=AAV(strs)-1; ib=AV(base); j=h=0;
 for(i=0;i<n;i++) {
  if(1<nf) {
   if(h==nf) h=0;
   if(h) { ib += 4; u += NMODVALS; } 
   else { ib=AV(base); u=AAV(strs)-1; }
  }
  if(j==nc) j=0;
  k=l=ib[0]; d=ib[1]; mods=ib[2]; coll=ib[3+(I )(1==nf)*j];
  nB= AN(uB); nD= AN(uD); nM= AN(uM); nN= AN(uN); nP= AN(uP); nQ= AN(uQ); nR= AN(uR); nI= AN(uI);
  cB=CAV(uB); cD=CAV(uD); cM=CAV(uM); cN=CAV(uN); cP=CAV(uP); cQ=CAV(uQ); cR=CAV(uR); cI=CAV(uI);
  subs=AN(uS)?CAV(uS):(C*)"e,.-*";
  switch(mode) {
  case 0: v=*a1v; cv=CAV(v); break;
  case 1: k=0<l?l:coll; cv=cvv[j]; cvv[j]+=k; break;
  case 2: k=0<l?l:coll; cv=cx; cx+=k; break;
  default: ASSERTSYS(0, "jtfmtallcol: mode");
  }
  if(l>0 && l<*il) mvc(l,cv,1,iotavec-IOTAVECBEGIN+(SUBs));  // can't dereference il if l==0.  If field too short, fill with user's * character
  else {
   // first, install background if r<xx> given; otherwise blanks
   if(0<=l && mL){if(nR)mvc(k,cv,nR,cR); else mvc( l-*il,cv+*il,1,iotavec-IOTAVECBEGIN+' ');}
   else if(0<=l) {if(nR)mvc(k,cv,nR,cR); else mvc( l-*il,cv,1,iotavec-IOTAVECBEGIN+' '); cv+=l-*il;}
   // format the value.
   if(*bits&BITSf) {  // value is _ __ _. (and therefore FL type)
    if(mI && *bits&BITS__)MC(cv, cI, nI);  // NULL has priority i<xx>
    else if(mD) MC(cv, cD, nD);  // if d<xx> given, use it
    else if(*bits&BITS_ ) { cv[0]='_'; }  // otherwise, use the visual representation of the value
    else if(*bits&BITS__) { cv[0]='_'; cv[1]='_'; }
    else                  { cv[0]='_'; cv[1]='.'; }
   } else if(*bits&BITSz) {  // not infinity; is 0?  (could be B01, INT, or FL)
    if(mB) MC(cv, cB, nB);  // if b<xx> given, use it
    else {
     if(mPQ) { MC(cv, cP, nP); MC(cv+*il-nQ, cQ, nQ); }  // if p<xx> or q<xx> given, move in those fields
     RZ(sprintfI(cv+nP,*il-nP-nQ,d,0,subs));  // format 0 into the field, skipping p/q
    }
   } else if(mI && t&INT && *iv==IMIN){MC(cv, cI, nI);  // if we are checking for NULL, that overrides exponential
   } else if(*bits&BITSe) {  // is nonzero to be displayed in exponential notation?  (must be INT or FL)
    dtmp=t&FL?*dv:(D)*iv;  // get value, as a float
    y=dtmp < 0; g=0;    // y will be length of sign prefix; init to 1 if negative.  g is length of sign suffix9
    if(dtmp < 0 && mMN) { y=nM; g=nN; }   // if value is negative and m<xx> or n<xx> given, set pref/suff length from m/n
    else if(dtmp>=0 && mPQ) { y=nP; g=nQ; }   // if value is nonnegative and p<xx> or q<xx> given, set pref/suff length from p/q
    RZ(sprintfeD(cv+y,*il-y-g,d,exprndID(d,dtmp),subs));  // format the number, skipping pref/suff
    if     (dtmp< 0 && mMN) { MC(cv, cM, nM); MC(cv+*il-nN, cN, nN); }  // if negative & pref/suff given, move them in
    else if(dtmp< 0       ) { *cv=SUBm;                              }   // if other negative, use the specified - sign
    else if(dtmp>=0 && mPQ) { MC(cv, cP, nP); MC(cv+*il-nQ, cQ, nQ); }  // if nonnegative & pref/suff given, move them in
   } else {
    if(t&INT+XNUM){  // integer to be displayed in fixed-point
     I sgniv=t&INT?*iv:AS((A)*iv)[0];   // surrogate for value, giving sign & zero info
     y=sgniv < 0; g=0; 
     if(sgniv < 0 && mMN) { y=nM; g=nN; }
     else if(sgniv>=0 && mPQ) { y=nP; g=nQ; }  // pref/suff length as above
     m=*il-y-g; if(mC) m=m-((m-!!d-d)>>2);  // m=length left for value after pref/suff, and any added commas
     if(t&INT){RZ(sprintfI(cv+y, m, d, sgniv, subs))  // format as integer
     }else{C *s=SgetX(*(A*)iv); s+=SGNTO0(sgniv); I sl=strlen(s); MC(cv+y+m-sl,s,sl); }  // format XNUM as integer, skip sign, copy to END of field
     if(mC) RZ(fmtcomma(cv+y, *il-y-g, d, subs));  // insert commas if called for
     if     (sgniv < 0 && mMN) { MC(cv, cM, nM); MC(cv+*il-nN, cN, nN); }  // install pref/suff as above
     else if(sgniv < 0       ) { *cv=SUBm;                              }
     else if(sgniv>= 0 && mPQ) { MC(cv, cP, nP); MC(cv+*il-nQ, cQ, nQ); }
    }else if(t<INT){  // B01
     if(mPQ) { MC(cv, cP, nP); MC(cv+*il-nQ, cQ, nQ); }
     RZ(sprintfI(cv+nP, *il-nP-nQ, d, *bv, subs));
    }else{  // FL to be displayed in fixed point, as above but with decimal places
     y=*dv < 0; g=0;
     if(*dv < 0 && mMN) { y=nM; g=nN; }
     else if(*dv>=0 && mPQ) { y=nP; g=nQ; }
     m=*il-y-g; if(mC) m=m-((m-!!d-d)>>2);
     RZ(sprintfnD(cv+y, m, d, afzrndID(d,*dv), subs));  // round to the display precision
     if(mC) RZ(fmtcomma(cv+y, *il-y-g, d, subs));
     if     (*dv < 0 && mMN) { MC(cv, cM, nM); MC(cv+*il-nN, cN, nN); }
     else if(*dv < 0       ) { *cv=SUBm;                              }
     else if(*dv>= 0 && mPQ) { MC(cv, cP, nP); MC(cv+*il-nQ, cQ, nQ); }
    }
   }
  }
  a1v++; il++; bits++; bv++; iv++; dv++; h++; j++;
 }
 
 R x;
} /* format w */

static A jtfmtxi(J jt, A a, A w, I mode, I *omode){I lvl;
 ARGCHK2(a,w); *omode=0;
 if(unlikely(ISSPARSE(AT(w))))RZ(w=denseit(w));
 if(!AN(w))       RZ(w=reshape(shape(w),chrspace));
 if(JCHAR&AT(w))  R dfv1(a,w,qq(atop(ds(CBOX),ds(CCOMMA)),num(1)));
 ASSERT(1>=AR(a), EVRANK); 
 ASSERT(!AN(a) || JCHAR+BOX&AT(a), EVDOMAIN);
 if(JCHAR&AT(a)||!AN(a)) RZ(a=fmtbfc(a));
 ASSERT(1>=AR(a), EVRANK);
 ASSERT(0==AR(a) || AN(a)==AS(w)[AR(w)-1], EVLENGTH);
 /* catch out-of-memory errors from dtoa.c */
 if(setjmp(((struct dtoa_info*)jt->dtoa)->_env))ASSERTSYS(jt->jerr, "dtoa");
 if(lvl=level(jt,w)){A*wv=AAV(w),x; 
  ASSERT(1>=lvl, EVDOMAIN);
  DO(AN(w), x=C(wv[i]); ASSERT(1>=AR(x),EVRANK); if(AN(x)){ASSERT(AT(x)&JCHAR+NUMERIC,EVDOMAIN);
      ASSERT(!(AR(x)&&AT(x)&NUMERIC),EVRANK);});
  A z; R dfv2(z,reitem(shape(w),a),w,amp(foreign(num(8),num(0)), ds(COPE)));
 } else {
  if(ISDENSETYPE(AT(w),RAT+CMPX))RZ(w=ccvt(FL,w,0));
  *omode=mode;
  R fmtallcol(fmtprecomp(rank1ex0(a,DUMMYSELF,jtfmtparse),w),w,mode);
}} /* 8!:x internals */
  /* mode is the requested mode, *omode is the actual mode computed */
  /* mode is 0, 1, or 2 for 8!:0, 8!:1, or 8!:2                     */
  /* *omode is either 0 or mode                                     */

F2(jtfmt02){F12IP;I mode; R fmtxi(a,w,0,&mode);} /* 8!:0 dyad */

F2(jtfmt12){F12IP;A z;I mode,r,j;
 ARGCHK2(a,w);
 ASSERT(2>=AR(w), EVRANK);
 RZ(z=fmtxi(a,w,1,&mode));
 if(mode==1)R z;
 r=AR(z);
 A t; dfv1(t,cant1(2==r?z:reshape(v2(1L,SETIC(z,j)),z)), qq(atco(ds(CBOX),ds(COPE)),num(1)));
 R ravel(t);
} /* 8!:1 dyad */

F2(jtfmt22){F12IP;A z;I mode,r,j;
 ARGCHK2(a,w);
 ASSERT(2>=AR(w), EVRANK);
 RZ(z=fmtxi(a,w,2,&mode));
 if(mode==2)R z;
 r=AR(z);
 A t; dfv1(t,cant1(2==r?z:reshape(v2(1L,SETIC(z,j)),z)), qq(atco(ds(CBOX),ds(COPE)),num(1)));
 RZ(z=ravel(t));
 R AS(z)[0]?razeh(z):lamin1(z);
} /* 8!:2 dyad */

F1(jtfmt01){F12IP;ARGCHK1(w); RETF(fmt02(AR(w)?reshape(sc(AS(w)[AR(w)-1]),ds(CACE)):ds(CACE),w));} /* 8!:0 monad */
F1(jtfmt11){F12IP;ARGCHK1(w); RETF(fmt12(AR(w)?reshape(sc(AS(w)[AR(w)-1]),ds(CACE)):ds(CACE),w));} /* 8!:1 monad */
F1(jtfmt21){F12IP;ARGCHK1(w); RETF(fmt22(AR(w)?reshape(sc(AS(w)[AR(w)-1]),ds(CACE)):ds(CACE),w));} /* 8!:2 monad */
