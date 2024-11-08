/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Rational Numbers                                                        */

#include "j.h"
#include "ve.h"

#define DXBODY(exp)  R 0
#define DX1(f,exp)   R 0
#define DX2(f,exp)   R 0
#define XT(w)        R 0

X jtxc(J jt,I n){ // convert n to X
 GEMP0; mpz_t mpz; jmpz_init_set_si(mpz, n);
 R Xmp(z);
}
I jtxint(J jt,X w){ // return w as integer (assuming it fits)
 ASSERT(2>XLIMBLEN(w),EVDOMAIN);
 I sgn=XSGN(w);if(0==sgn)R0;UI limb=XLIMB0(w);
 if(0>sgn){
  if(1+(UI)IMAX>=limb)R-limb; // assume 2s complement
 }else{
  if(IMAX>=limb)R limb;
 }
 ASSERT(0,EVDOMAIN);
}
I jtxcompare(J jt,X a,X w){ // *a-w  NB. J's *
 int z= icmpXX(a, w);
 R z<0 ?-1 :z>0;
}
XF1(jtxsgn){ // *w NB. X w, X result
 R xc(XSGN(w)<0 ?-1 :XSGN(w)>0);
}
XF2(jtxplus){ // a+w  
 ARGCHK2(a,w);
 if (unlikely(ISX0(a))) R w;
 if (unlikely(ISX0(w))) R a;
 // X z= XaddXX(a,w); // XaddXX could become [temporary] syntactic sugar for jtxplus
 mp_size_t an= XLIMBLEN(a), wn= XLIMBLEN(w); // arg sizes
 mp_size_t m= MIN(an, wn), n= MAX(an, wn); // result sizes
 X z; GAX(z, n+1); const mp_ptr zd= voidAV1(z), ad= voidAV1(a), wd= voidAV1(w); // data locations
 B ap= XSGN(a)>0; B wp= XSGN(w)>0; // positive or negative?
 B ax= an>= wn; // when w and a have different lengths, larger should be first arg to mpn_add/mpn_sub
 if (ap==wp) { // work with unsigned magnitudes:
  if (jmpn_add(zd, ax?ad:wd, n, ax?wd:ad, m)) zd[n++]= 1;
  XSGN(z)= ap ?n :-n; // signs matched, result has sign of both args
 } else {
  B zp= ax ?ap :wp;
  if (jmpn_sub(zd, ax?ad:wd, n, ax?wd:ad, m)) {
   zp= 1-zp; // borrow means need to negate result
   if(unlikely(!jmpn_neg(zd, zd, n))) R X0; /* this X0 presumably never happens */
  }
  while (likely(n) && unlikely(!zd[n-1])) n--; /* trim leading zeros */
  if (unlikely(!n)) R X0;
  XSGN(z)= zp ?n :-n;
 }
 R z;
}
XF2(jtxminus){ // a-w  NB. X a, w
 ARGCHK2(a,w);
 R XsubXX(a,w);
}
XF2(jtxtymes){ // a*w  NB. X a, w
 ARGCHK2(a,w); A z;
 ASSERT(1+NBITS(a)+NBITS(w)<GMPMAXBITS, EVWSFULL); // result should fit in emergency gmp memory pool
 R XmulXX(a,w);
}
B jtxdivrem(J jt,X a,X w,X*qz,X*rz){ // a (<.@%,|~) w
 mpX(a); mpX(w); mpz_t mpq; mpz_t mpr; jmpz_init(mpq); jmpz_init(mpr);
 jmpz_fdiv_qr(mpq, mpr, mpa, mpw);
 *qz= Xmp(q); *rz= Xmp(r);
 R 1;
}
X jtxdiv(J jt,X a,X w,I mode){ // a mode@% w NB. X a, w // mode is <. or >. or ] 
 RZ(a&&w&&!jt->jerr); if(ISX0(a))R X0;ASSERT(XSGN(w), EWRAT); // infinity requres RAT
 switch(mode) {
 case XMFLR/* a<.@%w*/: R Xfdiv_qXX(a, w);
 case XMCEIL/*a>.@%w*/: R Xcdiv_qXX(a, w);
 case XMEXACT/*  a%w*/:
  {
   mpX(a); mpX(w); mpX0(z); mpX0(r);
   jmpz_fdiv_qr(mpz, mpr, mpa, mpw); 
   X z= Xmp(z); X r= Xmp(r);
   ASSERT(0==XSGN(r), EWRAT);
   R z;
  }
 }
 SEGFAULT; R0; // this should never happen
}
XF2(jtxrem){ // a | w
 if(!XSGN(a)) R w;
 R Xfdiv_rXX(w,a); /* a on right side here */
}
XF2(jtxgcd){ // a +. w
 R XgcdXX(a,w);
}
XF2(jtxlcm){PROLOG(10100); // a *. w
 X z= XlcmXX(a,w);
 if(0>XSGN(a)*XSGN(w))EPILOG(XsubXX(X0,z));
 EPILOG(z);
}
static X jtxexp(J jt,X w,I mode) {ASSERT(0, EWIRR);} // u ^w NB. u is <. or >. or ]

#define LG2 0.693147180559945286 /* log(2) */
XF2(jtxpow){PROLOG(10101); // a m&|@^ w // FIXME: m should be a parameter rather than jt->xmod
 X m= 0; A xmod= jt->xmod;
 if (xmod) {m= XAV(xmod)[0]; if (!XSGN(m)) m= 0;}
 if (0 == XSGN(w)) { // a^0  NB. a is irrelevant, check for 1=|m
  if (m && 1==XLIMBLEN(m) && 1==XLIMB0(m)) {
   R X0;
  } else {
   R X1;
  }
 }
 if (0 == m) { // a^w
  if (0 == XSGN(a)) { // 0^w
   if (0<XSGN(w)) R X0; 
   ASSERT(0==XSGN(w), EWRAT); R X1; // need RAT for infinity
  } else {
   if (ISX1(a)) R X1; // special case, w doesn't matter
   if (-1==XSGN(a) && 1==XLIMB0(a)) { // _1^w
    R XODDnz(w) ?X_1 :X1;
   }
   ASSERT(1==XLIMBLEN(w), EVLIMIT); // a^w too bulky?
   ASSERT(0<(I)XLIMB0(w), EVLIMIT);
   ASSERT(0<XSGN(w), EWRAT); // want rational result if w is negative
   I wi= IgetX(w);
   ASSERT(GMPMAXBITS > (1+NBITS(a))*wi, EVWSFULL); // GEMP (gmp emergency memory pool) heuristic
   EPILOG(XpowXU(a, wi)); // a^w
  } 
 } else {
  ASSERT(0<XSGN(w), EWRAT); // want rational result if w is negative
  mpX(a); mpX(w); mpX(m); mpz_t mpz;
  jmpz_init(mpz); jmpz_powm(mpz, mpa, mpw, mpm); GEMP0; // m | a ^ w
  if (0>XSGN(m)) jmpz_fdiv_r(mpz, mpz, mpm); // jmpz_powm ignores sign of m
  EPILOG(Xmp(z));
 }
}

XF1(jtxsq){ // *: w
 R XmulXX(w,w);
}
static X jtIrootX(J jt, UI a, X w) { // a %:w NB. optionally with <. or >.
 PROLOG(0114);
 ASSERT(1&a||0<=XSGN(w), EWIRR);
 mpX(w); mpX0(z); I exact= jmpz_root(mpz, mpw, a); GEMP0;
 if(!exact)
  switch(jt->xmode){
  default: ASSERTSYS(0,"xsqrt");
  case XMEXACT: jmpz_clear(mpz); ASSERT(0,EWIRR);
  case XMFLR: break;
  case XMCEIL: jmpz_add_ui(mpz, mpz, 1);
  }
 R Xmp(z);
}

XF1(jtxsqrt){ // %: w (optionally with <. or >.)
 R jtIrootX(jt, 2, w);
}

D jtxlogabs(J jt,X w){ // ^.|w  NB. result is type D
 mpX(w); long exp;
 D frac= jmpz_get_d_2exp(&exp, mpw); // frac=. t*w%2^exp [ exp=. t*##:|w [ t=. *|w
 R log(fabs(frac))+exp*0.693147180559945286; // (^.|frac)+exp*(%2^.^1)
}
static XF1(jtxlog1){ //  u^.|w  NB. u is <. or >. or (if w=1) ]
 ASSERT(0<=XSGN(w), EWIMAG);
 B b= ISX1(w); ASSERT(b||XMEXACT!=jt->xmode,EWIRR);
 R rifvsdebug(xc((I)xlogabs(w)+(I)(!b&&XMCEIL==jt->xmode)));
}
static D jtxlogd1(J jt,X w){ // ^.w NB. result is type D
 ASSERT(0<=XSGN(w),EWIMAG); R xlogabs(w);
}
static Z jtxlogz1(J jt,X w){ // ^.w NB. result is type D
 Z z; z.re=xlogabs(w); z.im=0>XSGN(w)?PI:0.0; R z;
}

static XF2(jtxlog2){
 ASSERT(0<XSGN(a), EVNONCE); ASSERT(!ISX1(a), EVNONCE);
 ASSERT(0<XSGN(w), EVNONCE);
 mpX(a); mpX(w);
 long ae; D am= jmpz_get_d_2exp(&ae, mpa);
 long we; D wm= jmpz_get_d_2exp(&we, mpw);
 D lg= (LG2*we+log(wm))/(LG2*ae+log(am)); // estimate log
 I e= (I)(lg+0.5); // round to nearest integer
 X t= XpowXU(a,e); // test estimate
 I c= icmpXX(t,w); // compare
 if (!c) R XgetI(e);  // exact match, we're done
 ASSERT(XMEXACT!=jt->xmode, EWIRR);
 if (c>0) R XgetI(e-(XMFLR==jt->xmode)); // our estimate was high
 else R XgetI(e+(XMCEIL==jt->xmode)); // our estimate was low
}

F2(jtxlog2a){A z; GAT0(z,XNUM,1L,0L); XAVn(0L,z)[0]=rifvsdebug(xlog2(XAV(a)[0],XAV(w)[0])); RNE(z);}

XF2(jtxroota){ // a %: w (optionally with <. or >.)
 PROLOG(0115);
 X z0, a0= XAV(a)[0], w0= XAV(w)[0];
 if (0==XSGN(a0)) {
  if (0==XSGN(w0)) {
   z0= X0;
  } else if (ISX1(w0)) {
   z0= X1;
  } else ASSERT(0, EWIRR);
 } else {
  // ASSERT(0<XSGN(w0)||1&XLIMB0(a0), EWIMAG);
  ASSERT(0<XSGN(w0), EWIMAG); // test/gx132.ijs wants imaginary cube roots
  I ia0= IgetXor(a0, ASSERT(0, EWIRR)); mpX(w0); mpX0(z0); // really want EWFL here
  I exact= jmpz_root(mpz0, mpw0, ia0); GEMP0;
  if (!exact)
   switch(jt->xmode) {
   case XMEXACT: ASSERT(0, EWIRR);
   case XMFLR: if (mpz0->_mp_size<0) jmpz_sub(mpz0, mpz0, mpX1); break;
   case XMCEIL: if (mpz0->_mp_size>0) jmpz_add(mpz0, mpz0, mpX1); break;
   }
  z0= Xmp(z0);
 }
 A z;GA(z,XNUM,1L,0L,0L); XAVn(0L,z)[0]= z0;
 EPILOG(z);
}
XF1(jtxfact){ // !w
 PROLOG(0116);
 ASSERT(0<=XSGN(w), EWIRR);
 mpX(w); 
 ASSERT(1>jmpz_cmp_si(mpw, 1000000), EVWSFULL); // somewhat arbitrary threshold, roughly matches GEMP limit for result
 mpz_t mpz; jmpz_init(mpz); jmpz_fac_ui(mpz, jmpz_get_si(mpw));
 EPILOG(Xmp(z));
}
F1(jtdigits10){ // "."0@":
 PROLOG(0117);
 ARGCHK1(w);
 // see if a fast case applies
 if(!AR(w))  // must be an atom
  switch(CTTZ(AT(w))){  // must be integral or string type
  case INTX: 
   if(0<=AV(w)[0]){  // must be nonnegative
    A z; GATV0(z,INT,SY_64?19:10,1);  // allocate a max-sized string
    I c= AV(w)[0], *zv= AV1(z), *zv0=zv;
    do{*zv++=c%10;}while(c/=10);  // create digits in reverse order
    I n= AN(z)= AS(z)[0]= zv-zv0;   // install length
    zv=zv0; I*v=zv+n-1; DQ(n>>1, c=*zv; *zv++=*v; *v--=c;); /* reverse in place */
    EPILOG(z);
   }
  case RATX: if (!ISQINT(QAV(w)[0])) break;  // if integral, fall through to continue
  case XNUMX: w= XAV(w)[0];
  case LITX: ASSERT(ISGMP(w), EVDOMAIN);  // LIT must be a GMP-allocated block (can it ever?)
   if(0<=XSGN(w)){
    C*s= SgetX(w);
    I n= strlen(s); // maybe better to use AN(UNvoidAV1(s))-1 ??
    A z; GATV0(z,INT,n,1); I*zv= AV1(z);
    DQ(n, zv[i]= s[i]-'0';);  // convert ASCII to integer
    EPILOG(z);
   }
 }
 // if none of the fast cases applies, do it the long way
 EPILOG(rank1ex0(thorn1(w),DUMMYSELF,jtexec1));
}

static XF2(jtxbinp){PROLOG(0118);
 X d;RZ(d=XsubXX(w,a));X s=0<icmpXX(a,d)?d:a; // s =. a <. w-a
 ASSERT(2>XLIMBLEN(s),EVDOMAIN); // FIXME: do we need additional constraints?
 X z=XbinXU(w,XSGN(s) ?XLIMB0(s) :0);
 EPILOGNOVIRT(z);
}   /* non-negative x,y; x<=y */

XF2(jtxbin){X d,z;PROLOG(0119);
 RZ(d=XsubXX(w,a));
 switch(4*(I)(0>XSGN(a))+2*(I)(0>XSGN(w))+(I)(0>XSGN(d))){
 default:             ASSERTSYS(0,"xbin");
 case 2: /* 0 1 0 */  /* Impossible */
 case 5: /* 1 0 1 */  /* Impossible */
 case 1: /* 0 0 1 */ 
 case 4: /* 1 0 0 */ 
 case 7: /* 1 1 1 */  R X0;
 case 0: /* 0 0 0 */  R rifvsdebug(xbinp(a,w));
 case 3: /* 0 1 1 */  
  z=xbinp(a,XsubXX(a,XaddXX(w,X1))); RZ(z);    EPILOG(rifvsdebug(AV(a)[0]&1?XnegX(z):z));
 case 6: /* 1 1 0 */  
  z=xbinp(XsubXX(X_1,w),XsubXX(X_1,a)); RZ(z); EPILOG(rifvsdebug(AV(d)[0]&1?XnegX(z):z));
 }
}

static A jtpiev(J jt,I n,X b){A e;I ek,i,n1=n-1;X bi,e0,e1,*ev,t;
 GATV0(e,XNUM,n,1); ev=XAV1(e);
 bi=e0=e1=iv1;
 for(i=0,ek=1;i<n1;++i,ek+=3){
  ev[i]=xtymes(e0,xtymes(XCUBE(e1),bi));
  t=xtymes(xc(ek),xtymes(xc(1+ek),xc(2+ek)));
  e0=xtymes(e0,t);        /* e0 = ! 3*i */
  e1=xtymes(e1,xc(1+i));  /* e1 = ! i   */
  bi=xtymes(bi,b);        /* bi = b^i   */
 }
 ev[i]=rifvsdebug(xtymes(e0,xtymes(XCUBE(e1),bi)));
 RE(e); R e;
} /* used in jtxpi (related to the denominator of the chudnovsky algorithm) */

static XF1(jtxpi){
 PROLOG(0120);
 ARGCHK1(w);
 if(!XSGN(w))R X0;
 ASSERT(jt->xmode!=XMEXACT,EVDOMAIN);
 X a;RZ(a=XgetI(545140134L));
 X b;RZ(b=XCUBE(XgetI(640320L)));
 X c;RZ(c=XgetI(13591409L));
 X d;RZ(d=XaddXX(XgetI(541681608L),XmulXX(XgetI(10L),XgetI(600000000L))));
 long bits; mpX(w); jmpz_get_d_2exp(&bits, mpw);
 I n1=1+bits/47, n=1+n1; // 47 = <.2^.b%12^3
 A e;RZ(e=piev(n,b)); X*ev=XAV(e), m=ev[n1];
 X f=X0, s0=X1; I sk=1;
 for(I i=0, p=0;;++i,p^=1){
  X s=XmulXX(s0,XaddXX(c,XmulXX(a,XgetI(i))));
  X t=xdiv(XmulXX(s,m),ev[i],XMEXACT);
  f=p?XsubXX(f,t):XaddXX(f,t);
  if(i==n1)break;
  DO(6, s0=XmulXX(s0,xc(sk++));); RE(s0);  /* s0 = ! 6*i */
 }
 f=XmulXX(d,f);
 X q=XpowXU(XgetI(10L),14*n1);
 X k=XmulXX(XmulXX(a,m),xsqrt(XmulXX(b,xsq(q))));
 EPILOG(rifvsdebug(xdiv(XmulXX(k,w),XmulXX(f,q),jt->xmode)));
}    /* Chudnovsky Bros. */

// what AN(w) would have been, in older versions of J
I jtoldsize(J jt, X w){PROLOG(0121);
 C mode= jt->xmode; jt->xmode= XMCEIL;
 X z= xlog2(XgetI(10000), XabsX(w));
 jt->xmode= mode;
 EPILOGNORET(z);
 I z0= ISX0(z) ?1 :XLIMB0(z);
 R MAX(1,z0);
}

APFX( plusXX, X,X,X, xplus ,,HDR1JERR)
APFX(minusXX, X,X,X, xminus,,HDR1JERR)
APFX(tymesXX, X,X,X, xtymes,,HDR1JERR)
APFX(  divXX, X,X,X, XDIV  ,,HDR1JERR)
APFX(  remXX, X,X,X, xrem  ,,HDR1JERR)
APFX(  gcdXX, X,X,X, xgcd  ,,HDR1JERR)
APFX(  lcmXX, X,X,X, xlcm  ,,HDR1JERR)
APFX(  minXX, X,X,X, XMIN  ,,HDR1JERR)
APFX(  maxXX, X,X,X, XMAX  ,,HDR1JERR)
APFX(  powXX, X,X,X, xpow  ,,HDR1JERR)
APFX(  binXX, X,X,X, xbin  ,,HDR1JERR)

AMON( sgnX, X,X, *z=  rifvsdebug(xsgn(*x));)
AMONPS(sqrtX, X,X, , *z= rifvsdebug(xsqrt(*x)); , HDR1JERR)
AMONPS( expX, X,X, , *z=  rifvsdebug(xexp(*x,jt->xmode)); , HDR1JERR)
AMONPS( logX, X,X, , *z= rifvsdebug(xlog1(*x)); , HDR1JERR)
AMONPS(logXD, D,X, , *z=xlogd1(*x); , HDR1JERR)
AMON(logXZ, Z,X, *z=xlogz1(*x);)
AMONPS( absX, X,X, , *z=   XabsX(*x); , HDR1JERR)
AMONPS(factX, X,X, , *z= rifvsdebug(xfact(*x)); , HDR1JERR)
AMONPS( pixX, X,X, , *z=   rifvsdebug(xpi(*x)); , HDR1JERR)

// Modular functions

#if SY_64
#define XMOD 3037000499    /* <. %: _1+2^63 */
#define doubletype unsigned __int128
#else
#define XMOD 46340      /* <. %: _1+2^31 */
#define doubletype uint64_t
#endif
#define modn(x) ({UI t; doubletype tt; tt=(doubletype)(x)*(doubletype)nrecip; t=tt>>BW; t=(x)-t*n; if(unlikely(t>=n))t-=n; t;})
 // x%m using mrecip.  x*mrecip is truncated, which gives the remainder.  mrecip may be 2^-64 low, so the remainder may be x*m/2^64 high, i. e. m^3/2^64.  This is never more than m too high, so a single correction suffices
 // we expect the correction to be rare so we use a branch

// w is an atom, either INT or XNUM, result is w(mod n) always nonnegative.  n is > 1
static UI modarg(A w, UI n, UI nrecip){UI z;
 I wsign;  // holds sign of w
 if(AT(w)&INT){
  wsign=IAV(w)[0];  // fetch value
  z=ABS(wsign); if(unlikely(z>=n))z=z%n;  // modulus of ABS(w)
 }else{
  // XNUM arg.  First thing is to take its modulus, which we do here since n is small
  X wx=XAV(w)[0]; wsign=XSGN(wx); RZ(wsign)  // sign/size of w, if 0 modulus is 0
  I wlimbs=ABS(wsign); UI *wdig=&XLIMB0(wx);  // get # limbs and a pointer to the least significant
  UI modBW=0-n*nrecip;  // this is 2^BW(mod n)
  z=0;  // modulus inherited from higher digits, and result
  DQ(wlimbs, z=(z*modBW)+wdig[i]%n; z=modn(z);)
 }
 if(unlikely(wsign<0))z=n-z;  // if w neg, correct to complementary modulus
 R z;
} 

// modular inverse of a(mod n).  0<=a<n, and n is a small integer.  nrecip is 2^64/n rounded down
// code from RosettaCode
static I mod_inv(I a, I n){
 I t = 0;  I nt = 1;  I r = n;  I nr = a;
 while(nr!=0) {
  I q = r/nr;
  I tmp = nt;  nt = t - q*nt;  t = tmp;
  tmp = nr;  nr = r - q*nr;  r = tmp;
 }
 if(unlikely(r>1))return -1;  /* No inverse */
 t+=REPSGN(t)&n;  // correct for overshoot
 return t;
}

// modular add on extendeds, one atom
static DF2(jtmodopextadd){A z;PROLOG(000);
 ASSERT(!((AT(a)|AT(w))&(NOUN&~XNUM)),EVDOMAIN)  // must test here if empty args
 X n=XAV(FAV(self)->fgh[2])[0]; X xa=XAV(a)[0], xw=XAV(w)[0];  // modulus (possibly negative) and the atomic ops
 X xz=XmodXX(XaddXX(xa,xw),n);   // perform modular power - using inverse if negative power.  We take the inverse twice, which sucks
 if(unlikely(XSGN(n)<0&&XSGN(xz)>0))xz=XsubXX(xz,n); // mod ignores sign of n; if n negative, move result to range -n.._1
 GAT0(z,XNUM,1,0); XAV0(z)[0]=xz; EPILOG(z);  // return atomic XNUM
}

// modular subtract on extendeds, one atom
static DF2(jtmodopextsub){A z;PROLOG(000);
 ASSERT(!((AT(a)|AT(w))&(NOUN&~XNUM)),EVDOMAIN)  // must test here if empty args
 X n=XAV(FAV(self)->fgh[2])[0]; X xa=XAV(a)[0], xw=XAV(w)[0];  // modulus (possibly negative) and the atomic ops
 X xz=XmodXX(XsubXX(xa,xw),n);   // perform modular power - using inverse if negative power.  We take the inverse twice, which sucks
 if(unlikely(XSGN(n)<0&&XSGN(xz)>0))xz=XsubXX(xz,n); // mod ignores sign of n; if n negative, move result to range -n.._1
 GAT0(z,XNUM,1,0); XAV0(z)[0]=xz; EPILOG(z);  // return atomic XNUM
}

// modular multiply on extendeds, one atom
static DF2(jtmodopexttimes){A z;PROLOG(000);
 ASSERT(!((AT(a)|AT(w))&(NOUN&~XNUM)),EVDOMAIN)  // must test here if empty args
 X n=XAV(FAV(self)->fgh[2])[0]; X xa=XAV(a)[0], xw=XAV(w)[0];  // modulus (possibly negative) and the atomic ops
 if(unlikely(icmpXX(xa,n)>0))xa=XmodXX(xa,n); if(unlikely(icmpXX(xw,n)>0))xw=XmodXX(xw,n);  // take moduli before product.  extra work if x is (unlikely) neg, but we don't want the abs
 X xz=XmodXX(XmulXX(xa,xw),n);   // perform modular multiply
 if(unlikely(XSGN(n)<0&&XSGN(xz)>0))xz=XsubXX(xz,n); // mod ignores sign of n; if n negative, move result to range -n.._1
 GAT0(z,XNUM,1,0); XAV0(z)[0]=xz; EPILOG(z);  // return atomic XNUM
}

// modular reciprocal/divide on extendeds, one atom.  Bivalent. 0%0=0
static DF2(jtmodopextdiv){A z;PROLOG(000);
 ASSERT(!((AT(a)|AT(w))&(NOUN&~XNUM)),EVDOMAIN)  // must test here if empty args
// obsolete  self=AT(w)&VERB?w:self;  // if monad, take self from w
 X xw=XAV(EPMONAD?a:w)[0], n=XAV(FAV(self)->fgh[2])[0];  // divisor and modulus (possibly negative)
 xw=XinvertXX(xw,n); // take inverse of divisor, returning 0 if coprime
 X xz;  // will hold result
 if(!EPMONAD){xz=XAV(a)[0]; if(unlikely(icmpXX(xz,n)>0))xz=XmodXX(xz,n); if(XSGN(xz)!=0){ASSERT(xw!=0,EVDOMAIN) xz=XmodXX(XmulXX(xz,xw),n);}}else{ASSERT(xw!=0,EVDOMAIN) xz=xw;}   // perform multiply and modulus if dyad
 if(unlikely(XSGN(n)<0&&XSGN(xz)>0))xz=XsubXX(xz,n); // mod ignores sign of n; if n negative, move result to range -n.._1
 GAT0(z,XNUM,1,0); XAV0(z)[0]=xz; EPILOG(z);  // return atomic XNUM
}

// modular power on extendeds, one atom
static DF2(jtmodopextexp){A z;PROLOG(000);
 ASSERT(!((AT(a)|AT(w))&(NOUN&~XNUM)),EVDOMAIN)  // must test here if empty args
 X n=XAV(FAV(self)->fgh[2])[0]; X xa=XAV(a)[0], xw=XAV(w)[0];  // modulus (possibly negative) and the atomic ops
 if(unlikely(icmpXX(xa,n)>0))xa=XmodXX(xa,n);  // take modulus of a
 // if power is negative, take the modular inverse of the base
 if(XSGN(xw)<0){ASSERT((xa=XinvertXX(xa,n))!=0,EVDOMAIN) xw=XabsX(xw);}  // verify inverse exists
 X xz=XpowmXXX(xa,xw,n);   // perform modular power - using inverse if negative power.
 if(unlikely(XSGN(n)<0&&XSGN(xz)>0))xz=XsubXX(xz,n); // jmpz_powm ignores sign of n; if n negative, move result to range -n.._1
 GAT0(z,XNUM,1,0); XAV0(z)[0]=xz; EPILOG(z);  // return atomic XNUM
}

// modular operation on extendeds (function for one atom is in self)
// Convert args to XNUM and call rankex to loop over atoms
static DF1(jtmodopext1){if(likely(AN(w)!=0)){if(!(AT(w)&XNUM))RZ(w=cvt(XMODETOCVT(2)|XNUM,w))} R rank1ex0(w,self,FAV(self)->localuse.lu0.modatomfn);}   // (2) means exact conversion only
static DF2(jtmodopext2){if(likely((-AN(a)&-AN(w))<0)){if(!(AT(a)&XNUM))RZ(a=cvt(XMODETOCVT(2)|XNUM,a)) if(!(AT(w)&XNUM))RZ(w=cvt(XMODETOCVT(2)|XNUM,w))} R rank2ex0(a,w,self,FAV(self)->localuse.lu0.modatomfn);}   // (2) means exact conversion only

// modular add with small modulus, one atom.  a and w are either INT or XNUM.  Result is XNUM if modulus is
static DF2(jtmodopintadd){PROLOG(000);
 ASSERT(!((AT(a)|AT(w))&(NOUN&~(INT|XNUM))),EVDOMAIN)  // must test here if empty args
 // fetch n from h and nrecip from self
 X hx0=XAV(FAV(self)->fgh[2])[0];  // the one and only limb of the modulus
 I nsign=XSGN(hx0); UI n=XLIMB0(hx0); UI nrecip=FAV(self)->localuse.lu1.mrecip;
 // fetch x(mod n) and y(mod n)
 UI x=modarg(a,n,nrecip), y=modarg(w,n,nrecip);  // multiplicands
 UI z=x+y; z=modn(z);   // add and take modulus
 // correct for negative n
 A zzz; RZ(zzz=sc(z-(REPSGN(nsign&-z)&n)));  // if m neg, move result to range -m+1..0
 // if modulus was XNUM, make result extended
 if(unlikely(AT(FAV(self)->fgh[1])&XNUM+RAT))zzz=cvt(XNUM,zzz);
 EPILOG(zzz)
}

// modular subtract with small modulus, one atom.  a and w are either INT or XNUM.  Result is XNUM if modulus is
static DF2(jtmodopintsub){PROLOG(000);
 ASSERT(!((AT(a)|AT(w))&(NOUN&~(INT|XNUM))),EVDOMAIN)  // must test here if empty args
 // fetch n from h and nrecip from self
 X hx0=XAV(FAV(self)->fgh[2])[0];  // the one and only limb of the modulus
 I nsign=XSGN(hx0); UI n=XLIMB0(hx0); UI nrecip=FAV(self)->localuse.lu1.mrecip;
 // fetch x(mod n) and y(mod n)
 UI x=modarg(a,n,nrecip), y=modarg(w,n,nrecip);  // multiplicands
 UI z=n+x-y; z=modn(z);   // subtract and take modulus (keep intermediates positive)
 // correct for negative n
 A zzz; RZ(zzz=sc(z-(REPSGN(nsign&-z)&n)));  // if m neg, move result to range -m+1..0
 // if modulus was XNUM, make result extended
 if(unlikely(AT(FAV(self)->fgh[1])&XNUM+RAT))zzz=cvt(XNUM,zzz);
 EPILOG(zzz)
}


// modular multiply with small modulus, one atom.  a and w are either INT or XNUM.  Result is XNUM if modulus is
static DF2(jtmodopinttimes){PROLOG(000);
 ASSERT(!((AT(a)|AT(w))&(NOUN&~(INT|XNUM))),EVDOMAIN)  // must test here if empty args
 // fetch n from h and nrecip from self
 X hx0=XAV(FAV(self)->fgh[2])[0];  // the one and only limb of the modulus
 I nsign=XSGN(hx0); UI n=XLIMB0(hx0); UI nrecip=FAV(self)->localuse.lu1.mrecip;
 // fetch x(mod n) and y(mod n)
 UI x=modarg(a,n,nrecip), y=modarg(w,n,nrecip);  // multiplicands
 UI z=x*y; z=modn(z);   // multiply and take modulus
 // correct for negative n
 A zzz; RZ(zzz=sc(z-(REPSGN(nsign&-z)&n)));  // if m neg, move result to range -m+1..0
 // if modulus was XNUM, make result extended
 if(unlikely(AT(FAV(self)->fgh[1])&XNUM+RAT))zzz=cvt(XNUM,zzz);
 EPILOG(zzz)
}

// modular divide/reciprocal with small modulus, one atom.  a and w are either INT or XNUM.  Result is XNUM if modulus is
// bivalent
static DF2(jtmodopintdiv){PROLOG(000);
 ASSERT(!((AT(a)|AT(w))&(NOUN&~(INT|XNUM))),EVDOMAIN)  // bivalent must test here if empty args
// obsolete  self=AT(w)&VERB?w:self;  // if monad, take self from w
 // fetch n from h and nrecip from self
 X hx0=XAV(FAV(self)->fgh[2])[0];  // the one and only limb of the modulus
 I nsign=XSGN(hx0); UI n=XLIMB0(hx0); UI nrecip=FAV(self)->localuse.lu1.mrecip;
 // fetch [x(mod n)] and y(mod n)
 UI y=modarg(!EPMONAD?w:a,n,nrecip);
 // Take modular inverse of y
 y=mod_inv(y,n); // modular inverse, ~0 if inverse not defined
 UI z;  // will hold result
 if(!EPMONAD){z=modarg(a,n,nrecip); if(z!=0){ASSERT((I)y>=0,EVDOMAIN); z*=y; z=modn(z);}}else{ASSERT((I)y>=0,EVDOMAIN) z=y;}   // multiply and take modulus if dyad 0%0=0
 // correct for negative n
 A zzz; RZ(zzz=sc(z-(REPSGN(nsign&-z)&n)));  // if m neg, move result to range -m+1..0
 // if modulus was XNUM, make result extended
 if(unlikely(AT(FAV(self)->fgh[1])&XNUM+RAT))zzz=cvt(XNUM,zzz);   // cvt is slow
 EPILOG(zzz)
}


// modular power with small integer modulus, one atom.  a and w are either INT or XNUM.  Result is XNUM if modulus is
static DF2(jtmodopintexp){PROLOG(000);
 ASSERT(!((AT(a)|AT(w))&(NOUN&~(INT|XNUM))),EVDOMAIN)  // must test here if empty args
 // if w is XNUM, transfer to extended code
 if(AT(w)&XNUM){if(!(AT(a)&XNUM))RZ(a=cvt(XNUM,a)) R jtmodopextexp(jt,a,w,self);}
 // fetch n from h and nrecip from self
 X hx0=XAV(FAV(self)->fgh[2])[0];  // the one and only limb of the modulus
 I nsign=XSGN(hx0); UI n=XLIMB0(hx0); UI nrecip=FAV(self)->localuse.lu1.mrecip;
 // fetch x(mod n) and y
 UI x=modarg(a,n,nrecip); I y=IAV(w)[0];  // the base and exponent
 if(y<0){
  // Negative y.  Take x=modular inverse x^_1(mod n).
  ASSERT((I)(x=mod_inv(x,n))>=0,EVDOMAIN)
  y=-y;  // use positive power of modular inverse
 }
 // take modular power by repeated squaring
 UI z=1;  // init x^0
 while(y){UI zz=z*x; zz=modn(zz); x*=x; x=modn(x); z=y&1?zz:z; y>>=1;}  //  repeated square/mod
 // correct for negative n
 A zzz; RZ(zzz=sc(z-(REPSGN(nsign&-z)&n)));  // if m neg, move result to range -m+1..0
 // if modulus was XNUM, make result extended
 if(unlikely(AT(FAV(self)->fgh[1])&XNUM+RAT))zzz=cvt(XNUM,zzz);
 EPILOG(zzz)
}

// modular operation with small integer modulus (function for one integer atom is in self)
// convert arg(s) to integral and call the function through the rank loop
// if an arg is empty skip the conversion.  We have to check each atom's type then
static DF1(jtmodopint1){if(likely(AN(w)!=0)){if(!(AT(w)&INT+XNUM))RZ(w=cvt(AT(w)&RAT?XNUM:INT,w))} R rank1ex0(w,self,FAV(self)->localuse.lu0.modatomfn);}
static DF2(jtmodopint2){if(likely((-AN(a)&-AN(w))<0)){if(!(AT(a)&INT+XNUM))RZ(a=cvt(AT(a)&RAT?XNUM:INT,a)) if(!(AT(w)&INT+XNUM))RZ(w=cvt(AT(w)&RAT?XNUM:INT,w))} R rank2ex0(a,w,self,FAV(self)->localuse.lu0.modatomfn);}

// entry point to execute monad/dyad %. m. n after the noun argument(s) are supplied
static DF2(jtmodularexplicitx){F2PREFIP;  // this stands in place of jtxdefn, which inplaces
 // the only reason we need this routine is to reformat any error to avoid exposing internals
 A z=jtxdefn(jt,a,w,self);   // the imp must be in an explicit def
 // if there was an error, save the error code and recreate the error at this level, to cover up details inside the script
 if(unlikely(jt->jerr)){I e=jt->jerr; RESETERR; jsignal(e);}
 R z;
}

// entry point for monad and dyad %. m. n (type=0) and -/ . * m. n (type=1)
static A jtmodularexplicit(J jt,A a,A w,I type){
 // Apply Md_j_ to the input arguments, creating a derived verb to do the work
 A xadv; ASSERT(xadv=jtfindnameinscript(jt,"~addons/dev/modular/modular.ijs",type?"Mdet_j_":"Md_j_",ADV),EVNONCE);
 A derivvb; RZ(derivvb=jtunquote((J)((I)jt|JTXDEFMODIFIER),w,xadv,xadv));
 // If the returned verb has VXOPCALL set, that means we are in debug and a namerefop has been interposed for Foldr_j_.  We don't want that - get the real verb
 if(unlikely(FAV(derivvb)->flag&VXOPCALL))derivvb=FAV(derivvb)->fgh[2];  // the verb is saved in h of the reference
 // Modify the derived verb to go to our preparatory stub.  We require that the continuation be at jtxdefn because we call bivalently and with flags
 ASSERT(FAV(derivvb)->valencefns[1]==jtxdefn,EVSYSTEM);
 FAV(derivvb)->valencefns[0]=jtmodularexplicitx;   // monad always defined
 FAV(derivvb)->valencefns[1]=type?jtvalenceerr:(AF)jtmodularexplicitx;  // dyad defined only for %.
 // For display purposes, give the compound the spelling of the original
 FAV(derivvb)->id=CMDOT;
 R derivvb;
}

// index is (fn#,extended)
static AF modoptbl[][2]={ {jtmodopintexp,jtmodopextexp} , {jtmodopinttimes,jtmodopexttimes} , {jtmodopintdiv,jtmodopextdiv} , {jtmodopintadd,jtmodopextadd} , {jtmodopintsub,jtmodopextsub} };
// Modular arithmetic u m. n
F2(jtmdot){F2PREFIP;A z=0;
 ASSERT(AT(a)&VERB,EVDOMAIN)  // u must be a verb
 // Verify that n is an integer and create a XNUM form for it
 ASSERT(AT(w)&NOUN,EVDOMAIN) ASSERT(AR(w)==0,EVRANK)  // n must be a noun atom
 // Handle the forms that use external libraries
 I type=-1; type=FAV(a)->id==CDOMINO?0:type; type=FAV(a)->valencefns[0]==jtdet?1:type;
 if(type>=0)R jtmodularexplicit(jt,a,w,type);
 A h=w;  // XNUM form of w
 if(!(AT(w)&XNUM))RZ(h=cvt(XNUM,w));   // convert to XNUM
 I nrecip=0;  // will hold reciprocal of abs(n), init to invalid
 UI n=2;  // init integer value of modulus to 'not special'
 // If n is small enough to use integer ops, take its reciprocal
 X hx0=XAV(h)[0];  // the atom as an X
 ASSERT(XSGN(hx0)!=0,EVDOMAIN)  // modulus cannot be 0
 if(ABS(XSGN(hx0))==1){
  n=XLIMB0(hx0);  // absolute value of modulus
  if(BETWEENC(n,2,XMOD)){nrecip=((UI)IMIN/n); nrecip=(nrecip<<1)+((((UI)IMIN-nrecip*n)<<1)>=n);}  // if n small enough, 2^64%m, possibly low by as much as 2^-64
 }
 // Verify that u is a supported verb, and point to its functions
 AF fn1=nrecip==0?jtmodopext1:jtmodopint1, fn2=nrecip==0?jtmodopext2:jtmodopint2;  // top-level verbs monad/dyad, and the verb for atoms
 I fnx=sizeof(modoptbl)/sizeof(modoptbl[0]); fnx=FAV(a)->id==CEXP?0:fnx;  fnx=FAV(a)->id==CSTAR?1:fnx; fnx=FAV(a)->id==CDIV?2:fnx;  // convert verb id to table index
 fnx=FAV(a)->id==CPLUS?3:fnx;  fnx=FAV(a)->id==CMINUS?4:fnx;
 ASSERT(fnx!=sizeof(modoptbl)/sizeof(modoptbl[0]),EVDOMAIN)   // u must be supported
 fn1=(0b100&(1<<fnx))?fn1:jtvalenceerr;  // if u doesn't support monad, take that valence away

 // u is valid.  If n is _1, 0, or 1, modulus is degenerate and we just use a constant result
 if(unlikely(n<2)){R qq(zeroionei(0),zeroionei(0));}  // return 00"0

 // Create the result function.  h points to the original n as XNUM, lu0 points to the function for one atom, lu1 is the reciprocal of n if valid
 fdefallo(z)
 // we mark the compound as NAMELESS to avoid lookup overhead
 fdeffillall(z,VF2NAMELESS,CMDOT,VERB, fn1,fn2, a,w,h,VASGSAFE, 0,0,0,fffv->localuse.lu0.modatomfn=modoptbl[fnx][nrecip==0],FAV(z)->localuse.lu1.mrecip=nrecip);
 R z;
}

B jtxquad(J jt,E *z,X W){
 mpX(W); mpX0(z); D h=jmpz_get_d(mpW); jmpz_set_d(mpz,h); jmpz_sub(mpz,mpW,mpz); D l=jmpz_get_d(mpz);  // high & low parts as D
 D th,tl; TWOSUMBS1(h,l,th,tl) *z=CANONE1(th,tl);   // if jmpz_get_d rounds correctly, h and l will both overlap.  In case not, we make sure they do.  convert to canonical form
 R 1;
}
