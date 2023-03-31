/* Copyright (c) 1990-2023, Jsoftware Inc.  All rights reserved.           */
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
 X z= XaddXX(a,w);
 R z;
}
XF2(jtxminus){ // a-w  NB. X a, w
 ARGCHK2(a,w);
 R XsubXX(a,w);
}
XF2(jtxtymes){ // a*w  NB. X a, w
 ARGCHK2(a,w); A z;
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
  case XMEXACT/*  a%w*/: {
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
 if (0 == XSGN(w)) { // a^0  NB. m is irrelevant, a is irrelevant
  R X1;
 }
 X m= 0; A xmod= jt->xmod;
 if (xmod) {m= XAV(xmod)[0]; if (!XSGN(m)) m= 0;}
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
   D estimate= log(fabs(DgetX(a)));
   if (isinf(estimate)) estimate= XLIMBLEN(w)*SZI*LG2*8;
   ASSERT(LG2*8*GMPMAXSZ > estimate*wi, EVWSFULL); // GEMP (gmp emergency memory pool) heuristic
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

// https://en.wikipedia.org/wiki/Modular_multiplicative_inverse
XF2(jtxpowmodinv){PROLOG(0113); // special case when xmod is extended (and w is negative)
 X m= 0; A xmod= jt->xmod;
 if (xmod) {m= XAV(xmod)[0]; if (!XSGN(m)) m= 0;}
 if (0 == m) R xpow(a, w);
 mpX(a); mpX(w); mpX(m); mpz_t mpz;
 jmpz_init(mpz); jmpz_powm(mpz, mpa, mpw, mpm); GEMP0; // m | a ^ w
 if (0>XSGN(m)) jmpz_fdiv_r(mpz, mpz, mpm); // jmpz_powm ignores sign of m
 EPILOG(Xmp(z));
}

XF1(jtxsq){ // *: w
 R XmulXX(w,w);
}
static X jtIrootX(J jt, UI a, X w) { // a %:w NB. optionally with <. or >.
 PROLOG(0114);
 ASSERT(1&a||0<=XSGN(w), EWIRR);
 mpX(w); mpX0(z); I exact= jmpz_root(mpz, mpw, a); GEMP0;
 if (!exact) switch(jt->xmode){
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

F2(jtxlog2a){A z; GAT0(z,XNUM,1L,0L); XAV(z)[0]=rifvsdebug(xlog2(XAV(a)[0],XAV(w)[0])); RNE(z);}

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
  if (!exact) switch(jt->xmode) {
    case XMEXACT: ASSERT(0, EWIRR);
    case XMFLR: if (mpz0->_mp_size<0) jmpz_sub(mpz0, mpz0, mpX1); break;
    case XMCEIL: if (mpz0->_mp_size>0) jmpz_add(mpz0, mpz0, mpX1); break;
  }
  z0= Xmp(z0);
 }
 A z;GA(z,XNUM,1L,0L,0L); *XAV(z)= z0;
 EPILOG(z);
}
XF1(jtxfact){ // !w
 PROLOG(0116);
 ASSERT(0<=XSGN(w), EVDOMAIN);
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
    I c= AV(w)[0], *zv= AV(z), *zv0=zv;
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
    A z; GATV0(z,INT,n,1); I*zv= AV(z);
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
 GATV0(e,XNUM,n,1); ev=XAV(e);
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
