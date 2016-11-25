/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Elementary Functions (Arithmetic, etc.)                          */

#include "j.h"
#include "vasm.h"

#define DIVI(u,v)     (u||v ? u/(D)v : 0.0) 
#define DIVBB(u,v)    (v?u:u?inf:0.0)

#define TYMESBX(u,v)  (u?v:0)
#define TYMESXB(u,v)  (v?u:0)
#define TYMESID(u,v)  (u   ?u*v:0)
#define TYMESDI(u,v)  (   v?u*v:0)
#define TYMESDD(u,v)  (u&&v?u*v:0)

// II add, noting overflow and leaving it, possibly in place
AHDR2(plusII,I,I,I){I u;I v;I w;I oflo=0;
 // overflow is (input signs equal) and (result sign differs from one of them)
 // If u==0, v^=u is always 0 & therefore no overflow
 if(1==n) DQ(m, u=*x; v=*y; w= ~u; u+=v; *z=u; ++x; ++y; ++z; w^=v; v^=u; if(XANDY(w,v)<0)++oflo;)
 else if(b)DO(m, u=*x++; I thresh = IMIN-u; if (u<=0){DO(n, v=*y; if(v<thresh)++oflo; v=u+v; *z++=v; y++;)}else{DO(n, v=*y; if(v>=thresh)++oflo; v=u+v; *z++=v; y++;)})
 else      DO(m, v=*y++; I thresh = IMIN-v; if (v<=0){DO(n, u=*x; if(u<thresh)++oflo; u=u+v; *z++=u; x++;)}else{DO(n, u=*x; if(u>=thresh)++oflo; u=u+v; *z++=u; x++;)})
 if(oflo)jt->jerr=EWOVIP+EWOVIPPLUSII;
}

// BI add, noting overflow and leaving it, possibly in place.  If we add 0, copy the numbers (or leave unchanged, if in place)
AHDR2(plusBI,I,B,I){I u;I v;I oflo=0;
 if(1==n)  DO(m, u=(I)*x; v=*y; if(v==IMAX)oflo+=u; v=u+v; *z++=v; x++; y++; )
 else if(b)DO(m, u=(I)*x++; if(u){DO(n, v=*y; if(v==IMAX)oflo=1; v=v+1; *z++=v; y++;)}else{if(z!=y)MC(z,y,n<<LGSZI); z+=n; y+=n;})
 else      DO(m, v=*y++; DO(n, u=(I)*x; if(v==IMAX)oflo+=u; u=u+v; *z++=u; x++;))
 if(oflo)jt->jerr=EWOVIP+EWOVIPPLUSBI;
}

// IB add, noting overflow and leaving it, possibly in place.  If we add 0, copy the numbers (or leave unchanged, if in place)
AHDR2(plusIB,I,I,B){I u;I v;I oflo=0;
 if(1==n)  DO(m, u=*x; v=(I)*y; if(u==IMAX)oflo+=v; u=u+v; *z++=u; x++; y++; )
 else if(b)DO(m, u=*x++; DO(n, v=(I)*y; if(u==IMAX)oflo+=v; v=u+v; *z++=v; y++;))
 else      DO(m, v=(I)*y++; if(v){DO(n, u=*x; if(u==IMAX)oflo=1; u=u+1; *z++=u; x++;)}else{if(z!=x)MC(z,x,n<<LGSZI); z+=n; x+=n;})
 if(oflo)jt->jerr=EWOVIP+EWOVIPPLUSIB;
}

// BD DB add similarly?

// II subtract, noting overflow and leaving it, possibly in place
AHDR2(minusII,I,I,I){I u;I v;I w;I oflo=0;
 // overflow is (input signs differ) and (result sign differs from minuend sign)
 if(1==n)  {DQ(m, u=*x; v=*y; w=u-v; *z=w; ++x; ++y; ++z; v^=u; u^=w; if(XANDY(u,v)<0)++oflo;)}
// if u<0, oflo if u-v < IMIN => v > u-IMIN; if u >=0, oflo if u-v>IMAX => v < u+IMIN+1 => v <= u+IMIN => v <= u-IMIN
 else if(b)DO(m, u=*x++; I thresh = u-IMIN; if (u<0){DO(n, v=*y; if(v>thresh)++oflo; w=u-v; *z++=w; y++;)}else{DO(n, v=*y; if(v<=thresh)++oflo; w=u-v; *z++=w; y++;)})
 // if v>0, oflo if u-v < IMIN => u < v+IMIN = v-IMIN; if v<=0, oflo if u-v > IMAX => u>v+IMAX => u>v-1-IMIN => u >= v-IMIN
 else      DO(m, v=*y++; I thresh = v-IMIN; if (v<=0){DO(n, u=*x; if(u>=thresh)++oflo; u=u-v; *z++=u; x++;)}else{DO(n, u=*x; if(u<thresh)++oflo; u=u-v; *z++=u; x++;)})
 if(oflo)jt->jerr=EWOVIP+EWOVIPMINUSII;
}

// BI subtract, noting overflow and leaving it, possibly in place.  If we add 0, copy the numbers (or leave unchanged, if in place)
AHDR2(minusBI,I,B,I){I u;I v;I w;I oflo=0;
 if(1==n)  DO(m, u=(I)*x; v=*y; u=u-v; if(v<=IMIN+1&&u<0)++oflo; *z++=u; x++; y++; )
 else if(b)DO(m, u=(I)*x++; DO(n, v=*y; w=u-v; if(v<=IMIN+1&&w<0)++oflo; *z++=w; y++;))
 else      DO(m, v=*y++; DO(n, u=(I)*x; u=u-v; if(v<=IMIN+1&&u<0)++oflo; *z++=u; x++;))
 if(oflo)jt->jerr=EWOVIP+EWOVIPMINUSBI;
}

// IB subtract, noting overflow and leaving it, possibly in place.  If we add 0, copy the numbers (or leave unchanged, if in place)
AHDR2(minusIB,I,I,B){I u;I v;I w;I oflo=0;
 if(1==n)  DO(m, u=*x; v=(I)*y; if(u==IMIN)oflo+=v; u=u-v; *z++=u; x++; y++; )
 else if(b)DO(m, u=*x++; DO(n, v=(I)*y; if(u==IMAX)oflo+=v; w=u-v; *z++=w; y++;))
 else      DO(m, v=(I)*y++; if(v){DO(n, u=*x; if(u==IMAX)oflo=1; u=u-1; *z++=u; x++;)}else{if(z!=x)MC(z,x,n<<LGSZI); z+=n; x+=n;})
 if(oflo)jt->jerr=EWOVIP+EWOVIPMINUSIB;
}

// II multiply, in double precision
AHDR2(tymesII,I,I,I){DPMULDECLS I u;I v;if(jt->jerr)R; I *zi=z;
 if(1==n)  DO(m, u=*x; v=*y; DPMUL(u,v,z, goto oflo); ; z++; x++; y++; )
 else if(b)DO(m, u=*x; DO(n, v=*y; DPMUL(u,v,z, goto oflo); z++; y++;) x++;)
 else      DO(m, v=*y; DO(n, u=*x; DPMUL(u,v,z, goto oflo); z++; x++;) y++;)
exit: jt->mulofloloc += z-zi; R;
oflo: jt->jerr = EWOVIP+EWOVIPMULII; *x=u; *y=v; goto exit;  // back out the last store, in case it's in-place; gcc stores before overflow
}

// BI multiply, using clear/copy
AHDR2(tymesBI,I,B,I){B u;I v;
 if(1==n)  DO(m, u=*x; *z++=u?*y:0; x++; y++; )
 else if(b)DO(m, u=*x++; if(u){if(z!=y)MC(z,y,n<<LGSZI);}else{memset(z,0,n<<LGSZI);} z+=n; y+=n;)
 else      DO(m, v=*y++; DO(n, u=*x; *z++=u?v:0; x++;))
}

// IB multiply, using clear/copy
AHDR2(tymesIB,I,I,B){I u;B v;
 if(1==n)  DO(m, v=*y; *z++=v?*x:0; x++; y++; )
 else if(b)DO(m, u=*x++; DO(n, v=*y; *z++=v?u:0; y++;))
 else      DO(m, v=*y++; if(v){if(z!=x)MC(z,x,n<<LGSZI);}else{memset(z,0,n<<LGSZI);} z+=n; x+=n;)
}

// DI multiply, using clear/copy
AHDR2(tymesBD,D,B,D){B u;D v;
 if(1==n)  DO(m, u=*x; *z++=u?*y:0; x++; y++; )
 else if(b)DO(m, u=*x++; if(u){if(z!=y)MC(z,y,n*sizeof(D));}else{memset(z,0,n*sizeof(D));} z+=n; y+=n;)
 else      DO(m, v=*y++; DO(n, u=*x; *z++=u?v:0; x++;))
}

// ID multiply, using clear/copy
AHDR2(tymesDB,D,D,B){D u;B v;
 if(1==n)  DO(m, v=*y; *z++=v?*x:0; x++; y++; )
 else if(b)DO(m, u=*x++; DO(n, v=*y; *z++=v?u:0; y++;))
 else      DO(m, v=*y++; if(v){if(z!=x)MC(z,x,n*sizeof(D));}else{memset(z,0,n*sizeof(D));} z+=n; x+=n;)
}

// Overflow repair routines
// *x is a non-in-place argument, and n and m advance through it
//  each atom of *x is repeated n times
// *y is the I result of the operation that overflowed
// *z is the D result area (which might be the same as *y)
// b is unused for plus
AHDR2(plusIIO,D,I,I){I u;
 DO(m, u=*x++; DO(n, *z=(D)u + (D)(*y-u); ++y; ++z;));
}
AHDR2(plusBIO,D,B,I){I u;
 DO(m, u=(I)*x++; DO(n, *z=(D)u + (D)(*y-u); ++y; ++z;));
}

// For subtract repair, b is 1 if x was the subtrahend, 0 if the minuend
AHDR2(minusIIO,D,I,I){I u;
 DO(m, u=*x++; DO(n, *z=b?((D)(*y+u)-(D)u):((D)u - (D)(u-*y)); ++y; ++z;));
}
AHDR2(minusBIO,D,B,I){I u;
 DO(m, u=(I)*x++; DO(n, *z=b?((D)(*y+u)-(D)u):((D)u - (D)(u-*y)); ++y; ++z;));
}

// In multiply repair, z points to result, x and y to inputs
// Parts of z before mulofloloc have been filled in already
// We have to track the inputs just as for any other action routine
AHDR2(tymesIIO,D,I,I){I u,v;
 // if all the multiplies are to be skipped, skip them quickly
 I skipct=jt->mulofloloc;
 if(skipct>=m*n){skipct-=m*n;
 }else{
  // There are unskipped multiplies.  Do them.
  if(1==n)  DO(m, u=*x; v=*y; if(skipct){--skipct;}else{*z=(D)u * (D)v;} z++; x++; y++; )
  else if(b)DO(m, u=*x++; DO(n, v=*y; if(skipct){--skipct;}else{*z=(D)u * (D)v;} z++; y++;))
  else      DO(m, v=*y++; DO(n, u=*x; if(skipct){--skipct;}else{*z=(D)u * (D)v;} z++; x++;))
 }
 // Store the new skipct
 jt->mulofloloc=skipct;
}


// All these lines define functions for various operand combinations
// The comments indicate special cases that are defined by verbs that don't follow the
// AOVF/AIFX/ANAN etc. template


// These routines are used by sparse processing, which doesn't do in-place overflow
APFX( plusIO, D,I,I,  PLUSO)
APFX(minusIO, D,I,I, MINUSO)
APFX(tymesIO, D,I,I, TYMESO)

AIFX( plusBB, I,B,B, +     )    /* plusBI */                AIFX( plusBD, D,B,D, +   )
   /* plusIB */                 /* plusII */                AIFX( plusID, D,I,D, +   )
AIFX( plusDB, D,D,B, +     )  AIFX( plusDI, D,D,I, +)       ANAN( plusDD, D,D,D, PLUS)
ANAN( plusZZ, Z,Z,Z, zplus )


AIFX(minusBB, I,B,B, -     )    /* minusBI */               AIFX(minusBD, D,B,D, -    )
  /* minusIB */                 /* minusII */               AIFX(minusID, D,I,D, -    )
AIFX(minusDB, D,D,B, -     )  AIFX(minusDI, D,D,I, -)       ANAN(minusDD, D,D,D, MINUS)
ANAN(minusZZ, Z,Z,Z, zminus)

    /* andBB */                 /* tymesBI */                   /* tymesBD */            
    /* tymesIB */               /* tymesII */               APFX(tymesID, D,I,D, TYMESID)  
    /* tymesDB */             APFX(tymesDI, D,D,I, TYMESDI) APFX(tymesDD, D,D,D, TYMESDD) 
ANAN(tymesZZ, Z,Z,Z, ztymes )

APFX(  divBB, D,B,B, DIVBB)   APFX(  divBI, D,B,I, DIVI)    APFX(  divBD, D,B,D, DIV)
APFX(  divIB, D,I,B, DIVI )   APFX(  divII, D,I,I, DIVI)    APFX(  divID, D,I,D, DIV)
APFX(  divDB, D,D,B, DIVI )   APFX(  divDI, D,D,I, DIVI)    ANAN(  divDD, D,D,D, DIV)
ANAN(  divZZ, Z,Z,Z, zdiv )

     /* orBB */               APFX(  minBI, I,B,I, MIN)     APFX(  minBD, D,B,D, MIN)    
APFX(  minIB, I,I,B, MIN)     APFX(  minII, I,I,I, MIN)     APFX(  minID, D,I,D, MIN)  
APFX(  minDB, D,D,B, MIN)     APFX(  minDI, D,D,I, MIN)     APFX(  minDD, D,D,D, MIN)
APFX(  minSS, SB,SB,SB, SBMIN)

    /* andBB */               APFX(  maxBI, I,B,I, MAX)     APFX(  maxBD, D,B,D, MAX)    
APFX(  maxIB, I,I,B, MAX)     APFX(  maxII, I,I,I, MAX)     APFX(  maxID, D,I,D, MAX)  
APFX(  maxDB, D,D,B, MAX)     APFX(  maxDI, D,D,I, MAX)     APFX(  maxDD, D,D,D, MAX)
APFX(  maxSS, SB,SB,SB, SBMAX)


static D jtremdd(J jt,D a,D b){D q,x,y;
 if(!a)R b;
 ASSERT(!INF(b),EVNAN);
 if(a==inf )R 0<=b?b:a;
 if(a==infm)R 0>=b?b:a;
 q=b/a; x=tfloor(q); y=tceil(q); R teq(x,y)?0:b-a*x;
}

ANAN(remDD, D,D,D, remdd)
ANAN(remZZ, Z,Z,Z, zrem )

static I jtremid(J jt,I a,D b){D r;I k;
 ASSERT(a&&-9e15<b&&b<9e15,EWOV);
 r=b-a*floor(b/a); k=(I)r;
 ASSERT(k==r,EWOV);   // not really overflow - just has a fractional part
 R k;
}

APFX(remID, I,I,D, remid)

static I remii(I a,I b){I r; R!a?b:(r=b%a,0<a?r+a*(0>r):r+a*(0<r));}

AHDR2(remII,I,I,I){I u,v;
 if(1==n)  DO(m,               *z++=remii(*x,*y); x++; y++; )
 else if(b)DO(m, u=*x++; if(0<=u&&!(u&(u-1))){--u; DO(n, *z++=u&*y++;);}
                    else DO(n, *z++=remii( u,*y);      y++;))
 else      DO(m, v=*y++; DO(n, *z++=remii(*x, v); x++;     ));
}


static I igcd1(I a,I b){R a?igcd1(b%a,a):b;}  // Emulate Euclid

I jtigcd(J jt,I a,I b){
 if(a>IMIN&&b>IMIN){a=ABS(a); b=ABS(b);}
 else{
  if(a==b||!a||!b){jt->jerr=EWOV; R 0;}
  if(a==IMIN){b=ABS(b); a=-(a+b);}else{a=ABS(a); b=-(a+b);}
 }
 R a?igcd1(b%a,a):b;
}

D jtdgcd(J jt,D a,D b){D a1,b1,t;B stop = 0;
 a=ABS(a); b=ABS(b); if(a>b){t=a; a=b; b=t;}
 ASSERT(inf!=b,EVNAN);
 if(!a)R b;
 a1=a; b1=b;
 while(remdd(a1/jfloor(0.5+a1/a),b1)){t=a; if((a=remdd(a,b))==0)break; b=t;}  // avoid infinite loop if a goes to 0
 R a;
}    /* D.L. Forkes 1984; E.E. McDonnell 1992 */
// obsolete #if SY_64
// obsolete #if SY_WIN32 && !C_NA
// obsolete I jtilcm(J jt,I a,I b){C er=0;I b1,d,z;
// obsolete  if(a&&b){RZ(d=igcd(a,b)); b1=b/d; TYMESVV(1L,&z,&a,&b1); if(er)jt->jerr=EWOV; R    z;}else R 0;
// obsolete }
// obsolete #else
// obsolete I jtilcm(J jt,I a,I b){LD z;I b1,d;
// obsolete  if(a&&b){RZ(d=igcd(a,b)); b1=b/d; z=a*(LD)b1; if(z<IMIN||IMAX<z)jt->jerr=EWOV; R (I)z;}else R 0;
// obsolete }
// obsolete #endif
// obsolete #else
// obsolete I jtilcm(J jt,I a,I b){D z;I b1,d;
// obsolete  if(a&&b){RZ(d=igcd(a,b)); b1=b/d; z=a*(D)b1; if(z<IMIN||IMAX<z)jt->jerr=EWOV; R (I)z;}else R 0;
// obsolete }
// obsolete #endif
I jtilcm(J jt,I a,I b){I z;I d;
 if(a&&b){RZ(d=igcd(a,b)); if(0==(z=jtmult(0,a,b/d)))jt->jerr=EWOV; R z;}else R 0;
}

#define GCDIO(u,v)      (dgcd((D)u,(D)v))
#define LCMIO(u,v)      (dlcm((D)u,(D)v))

D jtdlcm(J jt,D a,D b){ASSERT(!(INF(a)||INF(b)),EVNAN); R a&&b?a*(b/dgcd(a,b)):0;}

APFX(gcdIO, D,I,I, GCDIO)
APFX(gcdII, I,I,I, igcd )
APFX(gcdDD, D,D,D, dgcd )
APFX(gcdZZ, Z,Z,Z, zgcd )

APFX(lcmII, I,I,I, ilcm )
APFX(lcmIO, D,I,I, LCMIO)
APFX(lcmDD, D,D,D, dlcm )
APFX(lcmZZ, Z,Z,Z, zlcm )


#define GETD          {d=*wv++; if(!d){z=0; break;}}
#define INTDIVF(c,d)  (0>c==0>d?c/d:c%d?c/d-1:c/d)
#define INTDIVC(c,d)  (0>c!=0>d?c/d:c%d?c/d+1:c/d)

F2(jtintdiv){A z;B b,flr;I an,ar,*as,*av,c,d,j,k,m,n,p,p1,r,*s,wn,wr,*ws,*wv,*zv;
 RZ(a&&w);
 an=AN(a); ar=AR(a); as=AS(a); av=AV(a);
 wn=AN(w); wr=AR(w); ws=AS(w); wv=AV(w); b=ar>=wr; r=b?wr:ar; s=b?as:ws;
 ASSERT(!ICMP(as,ws,r),EVLENGTH);
 if(an&&wn){PROD(m,r,s); PROD(n,b?ar-r:wr-r,r+s);}else m=n=0; 
 GATV(z,INT,b?an:wn,b?ar:wr,s); zv=AV(z);
 d=wn?*wv:0; p=0<d?d:-d; p1=d==IMIN?p:p-1; flr=XMFLR==jt->xmode;
 if(!wr&&p&&!(p&p1)){
  k=0; j=1; while(p>j){++k; j<<=1;}
  switch((0<d?0:2)+(flr?0:1)){
   case 0: DO(n,          *zv++=*av++>>k;);                    break;
   case 1: DO(n, c=*av++; *zv++=0< c?1+((c-1)>>k):(c+p1)>>k;); break;
   case 2: DO(n, c=*av++; *zv++=c>IMIN?-c>>k:-(-c>>k););       break;
   case 3: DO(n, c=*av++; *zv++=0<=c?-(c>>k):1+(-(1+c)>>k););
 }}else if(flr){
  if(1==n)    DO(m, c=*av++; GETD;                *zv++=INTDIVF(c,d);  )
  else   if(b)DO(m,          GETD; DO(n, c=*av++; *zv++=INTDIVF(c,d););)
  else        DO(m, c=*av++;       DO(n, GETD;    *zv++=INTDIVF(c,d););)
 }else{
  if(1==n)    DO(m, c=*av++; GETD;                *zv++=INTDIVC(c,d);  )
  else   if(b)DO(m,          GETD; DO(n, c=*av++; *zv++=INTDIVC(c,d););)
  else        DO(m, c=*av++;       DO(n, GETD;    *zv++=INTDIVC(c,d););)
 }
 R z?z:flr?floor1(divide(a,w)):ceil1(divide(a,w));
}    /* <.@% or >.@% on integers */


static F2(jtweight){RZ(a&&w); R df1(behead(over(AR(w)?w:reshape(a,w),one)),bsdot(slash(ds(CSTAR))));}

F1(jtbase1){A z;B*v;I c,d,m,n,p,r,*s,t,*x;
 RZ(w);
 n=AN(w); t=AT(w); r=AR(w); s=AS(w); c=r?*(s+r-1):1;
 ASSERT(t&DENSE,EVNONCE);
 if(c>(SY_64?63:31)||!(t&B01))R pdt(w,weight(sc(c),t&RAT+XNUM?cvt(XNUM,num[2]):num[2]));
 CPROD(n,m,r-1,s);
 GATV(z,INT,m,r?r-1:0,s); x=m+AV(z); v=n+BAV(w);
 if(c)DO(m, p=0; d=1; DO(c, if(*--v)p+=d; d+=d;); *--x=p;)
 else memset(x-m,C0,m*SZI);
 R z;
}

F2(jtbase2){I ar,*as,at,c,t,wr,*ws,wt;
 RZ(a&&w);
 at=AT(a); ar=AR(a); as=AS(a);
 wt=AT(w); wr=AR(w); ws=AS(w); c=wr?*(ws+wr-1):1; t=maxtype(at,wt);
 ASSERT(at&DENSE&&wt&DENSE,EVNONCE);
 if(!(t&at))RZ(a=cvt(t,a));
 if(!(t&wt))RZ(w=cvt(t,w));
 R 1>=ar?pdt(w,weight(sc(c),a)):rank2ex(w,rank2ex(sc(c),a,0L,0L,1L,jtweight),0L,1L,1L,jtpdt);
}

// #: y
F1(jtabase1){A d,z;B*zv;I c,n,p,r,t,*v;UI x;
 RZ(w);
 // n = #atoms, r=rank, t=type
 n=AN(w); r=AR(w); t=AT(w);
 ASSERT(t&DENSE,EVNONCE);
 // Result has rank one more than the input.  If there are no atoms,
 // return (($w),0)($,)w; if Boolean, return (($w),1)($,)w
 if(!n||t&B01)R reshape(over(shape(w),n?one:zero),w);
 if(!(t&INT)){
  // Not integer.  Calculate # digits-1 as d = 2 <.@^. >./ | , w  
  d=df2(num[2],maximum(one,aslash(CMAX,mag(ravel(w)))),atop(ds(CFLOOR),ds(CLOG)));
  // Calculate z = ((1+d)$2) #: w
  RZ(z=abase2(reshape(increm(d),num[2]),w));
  // If not float, result is exact or complex; either way, keep it
  if(!(t&FL))R z;
  // If float, see if we had one digit too many (could happen, if the log was too close to an integer)
  // calculate that as (0 = >./ ({."1 z)).  If so, return }."1 z ,  otherwise z
  // But we can't delete a digit if any of the values were negative - all are significant then
  if(i0(aslash(CPLUSDOT,ravel(lt(w,zero)))))R z;
  if(0==i0(aslash(CMAX,ravel(irs1(z,0L,1L,jthead)))))R irs1(z,0L,1L,jtbehead);
  R z;
 }
 // Integer.  Calculate x=max magnitude encountered (minimum of 1, to leave 1 output value)
 x=1; v=AV(w);
 DO(n, p=*v++; x|=(UI)(p>0?p:-p););  // overflow happens on IMIN, no prob
 for(c=0;x;x>>=1){++c;}  // count # bits in result
 GATV(z,B01,n*c,1+r,AS(w)); *(r+AS(z))=c;  // Allocate result area, install shape
 v=n+AV(w); zv=AN(z)+BAV(z);  // v->last input location (prebiased), zv->last result location (prebiased)
 DO(n, x=*--v; DO(c, *--zv=(B)(x&1); x>>=1;));  // copy in the bits, starting with the LSB
 R z;
}

F2(jtabase2){A z;I an,ar,at,wn,wr,wt,zn;
 RZ(a&&w);
 an=AN(a); ar=AR(a); at=AT(a);
 wn=AN(w); wr=AR(w); wt=AT(w);
 ASSERT(at&DENSE&&wt&DENSE,EVNONCE);
 if(!ar)R residue(a,w);
 if(1==ar&&at&B01+INT&&wt&B01+INT){I*av,d,r,*u,*wv,x,*zv;
  RZ(coerce2(&a,&w,INT));
  RE(zn=mult(an,wn)); GATV(z,INT,zn,1+wr,AS(w)); *(wr+AS(z))=an;
  av=an+AV(a); wv=wn+AV(w); zv=zn+AV(z);
  if(2==an&&!av[-2]&&0<(d=av[-1])){I d1,j,k;
   k=0; j=1; while(d>j){++k; j<<=1;} d1=d-1;  // should use CTTZ
   if(d==j)DO(wn, x=*--wv; *--zv=x&d1; *--zv=x>>k;)
   else    DO(wn, x=*--wv; if(0<=x){*--zv=x%d; *--zv=x/d;}else{*--zv=d+x%d; *--zv=-1+x/d;})  // kludge this line discrepant w/ previous
  }else DO(wn, x=*--wv; u=av; DO(an, d=*--u; *--zv=r=remii(d,x); x=d?(x-r)/d:0;););
  R z;
 }else{PROLOG(0070);A y,*zv;C*u,*yv;I k;
  F2RANK(1,0,jtabase2,0);
  k=bp(at); u=an*k+CAV(a);
  GA(y,at, 1, 0,0); yv=CAV(y);
  GATV(z,BOX,an,1,0); zv=an+AAV(z);
  DO(an, MC(yv,u-=k,k); RZ(w=divide(minus(w,*--zv=residue(y,w)),y)););
  z=ope(z);
  EPILOG(z);
}}

// Compute 2 | w for INT w, leaving Boolean result.   kludge this should shift into mask & store fullwords
F1(jtintmod2){A z;B*b,*v;I k=SZI,mask,m,n,q,r,*u,*wi;
 RZ(w);F1PREFIP;  // allow inplacing but don't use it, since the input is INT and the result is B01
 n=AN(w); q=n/k; r=n%k; v=BAV(w)+!liln*(k-1);
 GATV(z,B01,n,AR(w),AS(w)); u=AV(z);
 b=(B*)&mask; DO(k, b[i]=1;);
 b=(B*)&m; DO(q, DO(k, b[i]=*v; v+=k;); *u++=mask&m;)
 b=(B*)u; wi=AV(w)+q*k; DO(r, *b++=1&*wi++?1:0;);
 R z;
}
