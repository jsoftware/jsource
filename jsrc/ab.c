/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: b. bitwise functions                                           */

// TODO: rewrite & simplify, and remove divides
// note u b./&.(a.&i.) fails when y is shorter than an int
#include "j.h"
#include "ve.h"
#include "ar.h"

// commutative functions
AHDR2(bw0100II,BID,BID,BID){R bw0010II(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(bw0101II,BID,BID,BID){R bw0011II(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(bw1100II,BID,BID,BID){R bw1010II(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(bw1101II,BID,BID,BID){R bw1011II(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(bw0100I2I2,UI2,UI2,UI2){R bw0010I2I2(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(bw0101I2I2,UI2,UI2,UI2){R bw0011I2I2(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(bw1100I2I2,UI2,UI2,UI2){R bw1010I2I2(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(bw1101I2I2,UI2,UI2,UI2){R bw1011I2I2(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(bw0100I4I4,UI4,UI4,UI4){R bw0010I4I4(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(bw0101I4I4,UI4,UI4,UI4){R bw0011I4I4(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(bw1100I4I4,UI4,UI4,UI4){R bw1010I4I4(m^1^SGNTO0(m),z,y,x,n,jt);}
AHDR2(bw1101I4I4,UI4,UI4,UI4){R bw1011I4I4(m^1^SGNTO0(m),z,y,x,n,jt);}

#if C_AVX2 || EMU_AVX2
primop256(bw0000II,1,,zz=_mm256_setzero_pd(),R EVOK;)
primop256(bw0001II,1,,zz=_mm256_and_pd(xx,yy),R EVOK;)
primop256(bw0010II,0,,zz=_mm256_andnot_pd(yy,xx),R EVOK;)
primop256(bw0011II,0,,zz=xx,R EVOK;)
// commutative primop256(bw0100II,0,,zz=_mm256_andnot_pd(xx,yy),R EVOK;)
// commutative primop256(bw0101II,0,,zz=yy,R EVOK;)
primop256(bw0110II,1,,zz=_mm256_xor_pd(xx,yy),R EVOK;)
primop256(bw0111II,1,,zz=_mm256_or_pd(xx,yy),R EVOK;)
primop256(bw1000II,1,__m256d ones=_mm256_setzero_pd(); ones=_mm256_castsi256_pd(_mm256_cmpeq_epi32(_mm256_castpd_si256(ones),_mm256_castpd_si256(ones)));,zz=_mm256_xor_pd(ones,_mm256_or_pd(xx,yy)),R EVOK;)
primop256(bw1001II,1,__m256d ones=_mm256_setzero_pd(); ones=_mm256_castsi256_pd(_mm256_cmpeq_epi32(_mm256_castpd_si256(ones),_mm256_castpd_si256(ones)));,zz=_mm256_xor_pd(ones,_mm256_xor_pd(xx,yy)),R EVOK;)
primop256(bw1010II,0,__m256d ones=_mm256_setzero_pd(); ones=_mm256_castsi256_pd(_mm256_cmpeq_epi32(_mm256_castpd_si256(ones),_mm256_castpd_si256(ones)));,zz=_mm256_xor_pd(ones,yy),R EVOK;)
primop256(bw1011II,0,__m256d ones=_mm256_setzero_pd(); ones=_mm256_castsi256_pd(_mm256_cmpeq_epi32(_mm256_castpd_si256(ones),_mm256_castpd_si256(ones)));,zz=_mm256_xor_pd(ones,_mm256_andnot_pd(xx,yy)),R EVOK;)
// commutative primop256(bw1100II,0,__m256d ones=_mm256_setzero_pd(); ones=_mm256_castsi256_pd(_mm256_cmpeq_epi32(_mm256_castpd_si256(ones),_mm256_castpd_si256(ones)));,zz=_mm256_xor_pd(ones,xx),R EVOK;)
// commutative primop256(bw1101II,0,__m256d ones=_mm256_setzero_pd(); ones=_mm256_castsi256_pd(_mm256_cmpeq_epi32(_mm256_castpd_si256(ones),_mm256_castpd_si256(ones)));,zz=_mm256_xor_pd(ones,_mm256_andnot_pd(yy,xx)),R EVOK;)
primop256(bw1110II,1,__m256d ones=_mm256_setzero_pd(); ones=_mm256_castsi256_pd(_mm256_cmpeq_epi32(_mm256_castpd_si256(ones),_mm256_castpd_si256(ones)));,zz=_mm256_xor_pd(ones,_mm256_and_pd(xx,yy)),R EVOK;)
primop256(bw1111II,1,__m256d ones=_mm256_setzero_pd(); ones=_mm256_castsi256_pd(_mm256_cmpeq_epi32(_mm256_castpd_si256(ones),_mm256_castpd_si256(ones)));,zz=ones,R EVOK;)
#else
APFX(bw0000II, UI,UI,UI, BW0000,, R EVOK;)
APFX(bw0001II, UI,UI,UI, BW0001,, R EVOK;) 
APFX(bw0010II, UI,UI,UI, BW0010,, R EVOK;)
APFX(bw0011II, UI,UI,UI, BW0011,, R EVOK;)
// commutative APFX(bw0100II, UI,UI,UI, BW0100,, R EVOK;)
// commutative APFX(bw0101II, UI,UI,UI, BW0101,, R EVOK;)
APFX(bw0110II, UI,UI,UI, BW0110,, R EVOK;)
APFX(bw0111II, UI,UI,UI, BW0111,, R EVOK;)
APFX(bw1000II, UI,UI,UI, BW1000,, R EVOK;)
APFX(bw1001II, UI,UI,UI, BW1001,, R EVOK;)
APFX(bw1010II, UI,UI,UI, BW1010,, R EVOK;)
APFX(bw1011II, UI,UI,UI, BW1011,, R EVOK;)
// commutative APFX(bw1100II, UI,UI,UI, BW1100,, R EVOK;)
// commutative APFX(bw1101II, UI,UI,UI, BW1101,, R EVOK;)
APFX(bw1110II, UI,UI,UI, BW1110,, R EVOK;)
APFX(bw1111II, UI,UI,UI, BW1111,, R EVOK;)
#endif
APFX(bw10000II, UI,I,UI, BW10000,, R EVOK;)
APFX(bw10001II, UI,I,UI, BW10001,, R EVOK;)
APFX(bw10010II, UI,I,I, BW10010,, R EVOK;)
APFX(bw10000I2I2, UI2,I2,UI2, BW10000,, R EVOK;)
APFX(bw10001I2I2, UI2,I2,UI2, BW10001,, R EVOK;)
APFX(bw10010I2I2, UI2,I2,I2, BW10010,, R EVOK;)
APFX(bw10000I4I4, UI4,I4,UI4, BW10000,, R EVOK;)
APFX(bw10001I4I4, UI4,I4,UI4, BW10001,, R EVOK;)
APFX(bw10010I4I4, UI4,I4,I4, BW10010,, R EVOK;)
APFX(bw0000I2I2, UI2,UI2,UI2, BW0000,, R EVOK;)
APFX(bw0001I2I2, UI2,UI2,UI2, BW0001,, R EVOK;) 
APFX(bw0010I2I2, UI2,UI2,UI2, BW0010,, R EVOK;)
APFX(bw0011I2I2, UI2,UI2,UI2, BW0011,, R EVOK;)
// commutative APFX(bw0100I2I2, UI2,UI2,UI2, BW0100,, R EVOK;)
// commutative APFX(bw0101I2I2, UI2,UI2,UI2, BW0101,, R EVOK;)
APFX(bw0110I2I2, UI2,UI2,UI2, BW0110,, R EVOK;)
APFX(bw0111I2I2, UI2,UI2,UI2, BW0111,, R EVOK;)
APFX(bw1000I2I2, UI2,UI2,UI2, BW1000,, R EVOK;)
APFX(bw1001I2I2, UI2,UI2,UI2, BW1001,, R EVOK;)
APFX(bw1010I2I2, UI2,UI2,UI2, BW1010,, R EVOK;)
APFX(bw1011I2I2, UI2,UI2,UI2, BW1011,, R EVOK;)
// commutative APFX(bw1100I2I2, UI2,UI2,UI2, BW1100,, R EVOK;)
// commutative APFX(bw1101I2I2, UI2,UI2,UI2, BW1101,, R EVOK;)
APFX(bw1110I2I2, UI2,UI2,UI2, BW1110,, R EVOK;)
APFX(bw1111I2I2, UI2,UI2,UI2, BW1111,, R EVOK;)

APFX(bw0000I4I4, UI4,UI4,UI4, BW0000,, R EVOK;)
APFX(bw0001I4I4, UI4,UI4,UI4, BW0001,, R EVOK;) 
APFX(bw0010I4I4, UI4,UI4,UI4, BW0010,, R EVOK;)
APFX(bw0011I4I4, UI4,UI4,UI4, BW0011,, R EVOK;)
// commutative APFX(bw0100I4I4, UI4,UI4,UI4, BW0100,, R EVOK;)
// commutative APFX(bw0101I4I4, UI4,UI4,UI4, BW0101,, R EVOK;)
APFX(bw0110I4I4, UI4,UI4,UI4, BW0110,, R EVOK;)
APFX(bw0111I4I4, UI4,UI4,UI4, BW0111,, R EVOK;)
APFX(bw1000I4I4, UI4,UI4,UI4, BW1000,, R EVOK;)
APFX(bw1001I4I4, UI4,UI4,UI4, BW1001,, R EVOK;)
APFX(bw1010I4I4, UI4,UI4,UI4, BW1010,, R EVOK;)
APFX(bw1011I4I4, UI4,UI4,UI4, BW1011,, R EVOK;)
// commutative APFX(bw1100I4I4, UI4,UI4,UI4, BW1100,, R EVOK;)
// commutative APFX(bw1101I4I4, UI4,UI4,UI4, BW1101,, R EVOK;)
APFX(bw1110I4I4, UI4,UI4,UI4, BW1110,, R EVOK;)
APFX(bw1111I4I4, UI4,UI4,UI4, BW1111,, R EVOK;)

// scaf rewrite all these and I2/I4 as well
       static APFX(bw0000CC, UC,UC,UC, BW0000,, R EVOK;)
       static APFX(bw0001CC, UC,UC,UC, BW0001,, R EVOK;)
       static APFX(bw0010CC, UC,UC,UC, BW0010,, R EVOK;)
       static APFX(bw0011CC, UC,UC,UC, BW0011,, R EVOK;)
       static APFX(bw0100CC, UC,UC,UC, BW0100,, R EVOK;)
       static APFX(bw0101CC, UC,UC,UC, BW0101,, R EVOK;)
       static APFX(bw0110CC, UC,UC,UC, BW0110,, R EVOK;)
       static APFX(bw0111CC, UC,UC,UC, BW0111,, R EVOK;)
       static APFX(bw1000CC, UC,UC,UC, BW1000,, R EVOK;)
       static APFX(bw1001CC, UC,UC,UC, BW1001,, R EVOK;)
       static APFX(bw1010CC, UC,UC,UC, BW1010,, R EVOK;)
       static APFX(bw1011CC, UC,UC,UC, BW1011,, R EVOK;)
       static APFX(bw1100CC, UC,UC,UC, BW1100,, R EVOK;)
       static APFX(bw1101CC, UC,UC,UC, BW1101,, R EVOK;)
       static APFX(bw1110CC, UC,UC,UC, BW1110,, R EVOK;)
       static APFX(bw1111CC, UC,UC,UC, BW1111,, R EVOK;)

/* see below */                        /* see below */
REDUCEPFX(bw0001insI, UI,UI, BW0001, bw0001II, bw0001II)   static REDUCEPFX(bw0001insC, UC,UC, BW0001, bw0001CC, bw0001CC)
REDUCEPFX(bw0010insI, UI,UI, BW0010, bw0010II, bw0010II)   static REDUCEPFX(bw0010insC, UC,UC, BW0010, bw0010CC, bw0010CC)
/* see below */                        /* see below */
REDUCEPFX(bw0100insI, UI,UI, BW0100, bw0100II, bw0100II)   static REDUCEPFX(bw0100insC, UC,UC, BW0100, bw0100CC, bw0100CC)  
/* see below */                        /* see below */
REDUCEPFX(bw0110insI, UI,UI, BW0110, bw0110II, bw0110II)   static REDUCEPFX(bw0110insC, UC,UC, BW0110, bw0110CC, bw0110CC)
REDUCEPFX(bw0111insI, UI,UI, BW0111, bw0111II, bw0111II)   static REDUCEPFX(bw0111insC, UC,UC, BW0111, bw0111CC, bw0111CC)
REDUCEPFX(bw1000insI, UI,UI, BW1000, bw1000II, bw1000II)   static REDUCEPFX(bw1000insC, UC,UC, BW1000, bw1000CC, bw1000CC)
REDUCEPFX(bw1001insI, UI,UI, BW1001, bw1001II, bw1001II)   static REDUCEPFX(bw1001insC, UC,UC, BW1001, bw1001CC, bw1001CC)
/* see below */                        /* see below */
REDUCEPFX(bw1011insI, UI,UI, BW1011, bw1011II, bw1011II)   static REDUCEPFX(bw1011insC, UC,UC, BW1011, bw1011CC, bw1011CC)
/* see below */                        /* see below */
REDUCEPFX(bw1101insI, UI,UI, BW1101, bw1101II, bw1101II)   static REDUCEPFX(bw1101insC, UC,UC, BW1101, bw1101CC, bw1101CC)
REDUCEPFX(bw1110insI, UI,UI, BW1110, bw1110II, bw1110II)   static REDUCEPFX(bw1110insC, UC,UC, BW1110, bw1110CC, bw1110CC)
/* see below */                        /* see below */

       AHDRR(bw0000insI,UI,UI){I k=SZI*m*d; mvc(k,z,MEMSET00LEN,MEMSET00); R EVOK;}
static AHDRR(bw0000insC,UC,UC){I k=    m*d; mvc(k,z,MEMSET00LEN,MEMSET00); R EVOK;}

       AHDRR(bw1111insI,UI,UI){I k=SZI*m*d; mvc(k,z,1,MEMSETFF); R EVOK;}
static AHDRR(bw1111insC,UC,UC){I k=    m*d; mvc(k,z,1,MEMSETFF); R EVOK;}

       AHDRR(bw0011insI,UI,UI){I k=d*(n-1);                        DQ(m, DQ(d, *z++=  *x++;); x+=k;);R EVOK;}
static AHDRR(bw0011insC,UC,UC){I k=d*(n-1);                        DQ(m, DQ(d, *z++=  *x++;); x+=k;);R EVOK;}

       AHDRR(bw1100insI,UI,UI){I k=d*(n-1);                 if(1<n)DQ(m, DQ(d, *z++= ~*x++;); x+=k;) else MC(z,x,SZI*m*d);R EVOK;}
static AHDRR(bw1100insC,UC,UC){I k=d*(n-1);                 if(1<n)DQ(m, DQ(d, *z++= ~*x++;); x+=k;) else MC(z,x,    m*d);R EVOK;}

       AHDRR(bw0101insI,UI,UI){I k=d*(n-1);                  x+=k; DQ(m, DQ(d, *z++=  *x++;); x+=k;);R EVOK;}
static AHDRR(bw0101insC,UC,UC){I k=d*(n-1);                  x+=k; DQ(m, DQ(d, *z++=  *x++;); x+=k;);R EVOK;}

       AHDRR(bw1010insI,UI,UI){I k=d*(n-1);UI t=     (n&1)-1 ; x+=k; DQ(m, DQ(d, *z++=t^*x++;); x+=k;);R EVOK;}
static AHDRR(bw1010insC,UC,UC){I k=d*(n-1);UC t=(UC)((n&1)-1); x+=k; DQ(m, DQ(d, *z++=t^*x++;); x+=k;);R EVOK;}



// obsolete #define BITWISE(f,T,op)  \
// obsolete  DF2(f){F12IP;A z;I *av,k=0,x;T*wv,y,*zv;             \
// obsolete   ARGCHK2(a,w);  /* kludge we allow inplace call but we don't honor it yet */ \
// obsolete   if(!ISDENSETYPE(AT(a),INT))RZ(a=cvt(INT,a));                                    \
// obsolete   if(!ISDENSETYPE(AT(w),INT))RZ(w=cvt(INT,w));                                    \
// obsolete   av=(I*)AV(a);                          \
// obsolete   wv=(T*)AV(w);                 \
// obsolete   ASSERTAGREE(AS(a),AS(w),MIN(AR(a),AR(w)));                      \
// obsolete   I zr=MAX(AR(a),AR(w)); GATV(z,INT,AN(AR(a)>AR(w)?a:w),zr,AS(AR(a)>AR(w)?a:w)); zv=(T*)AVn(zr,z);                  \
// obsolete   if(!AN(z))R z;                                                       \
// obsolete   if     (AR(a)==AR(w))DQ(AN(a), x=*av++;           y=*wv++; *zv++=op(x,y);  )  \
// obsolete   else if(AR(a)< AR(w))DQ(AN(a), x=*av++; DQ(AN(w)/AN(a), y=*wv++; *zv++=op(x,y););)  \
// obsolete   else           DQ(AN(w), y=*wv++; DQ(AN(a)/AN(w), x=*av++; *zv++=op(x,y););); \
// obsolete   RE(0); RETF(z);                                                          \
// obsolete  }
// obsolete 
// obsolete #define BWROT(x,y)      ((y<<(x&(BW-1)))|(y>>(BW-(x&(BW-1)))))
// obsolete #define BWSHIFT(x,y)    (0>x ? (x<=-BW?0:y>>-x)           : (x>=BW?0:y<<x)            )
// obsolete #define BWSHIFTA(x,y)   (0>x ? (x<=-BW?(y<0?-1:0):y>>-x)  : (x>=BW?0:y<<x)            )
// obsolete 
// obsolete BITWISE(jtbitwiserotate,UI,BWROT   )
// obsolete BITWISE(jtbitwiseshift, UI,BWSHIFT )
// obsolete BITWISE(jtbitwiseshifta,I, BWSHIFTA)

DF1(jtbitwise1){F12IP;R CALL2IP(FAV(self)->valencefns[1],zeroionei(0),w,self);}


static AHDR2FN* bwC[16]={(AHDR2FN*)bw0000CC,(AHDR2FN*)bw0001CC,(AHDR2FN*)bw0010CC,(AHDR2FN*)bw0011CC, (AHDR2FN*)bw0100CC,(AHDR2FN*)bw0101CC,(AHDR2FN*)bw0110CC,(AHDR2FN*)bw0111CC,
                   (AHDR2FN*)bw1000CC,(AHDR2FN*)bw1001CC,(AHDR2FN*)bw1010CC,(AHDR2FN*)bw1011CC, (AHDR2FN*)bw1100CC,(AHDR2FN*)bw1101CC,(AHDR2FN*)bw1110CC,(AHDR2FN*)bw1111CC};

static AHDR2FN* bwI[16]={(AHDR2FN*)bw0000II,(AHDR2FN*)bw0001II,(AHDR2FN*)bw0010II,(AHDR2FN*)bw0011II, (AHDR2FN*)bw0100II,(AHDR2FN*)bw0101II,(AHDR2FN*)bw0110II,(AHDR2FN*)bw0111II,
                   (AHDR2FN*)bw1000II,(AHDR2FN*)bw1001II,(AHDR2FN*)bw1010II,(AHDR2FN*)bw1011II, (AHDR2FN*)bw1100II,(AHDR2FN*)bw1101II,(AHDR2FN*)bw1110II,(AHDR2FN*)bw1111II};

/* a m b.&.(a.&i.) w */
/* a m b.&.(a.i.]) w */
/* m e. 16+i.16      */
DF2(jtunderh2);
DF2(jtbitwisechar){F12IP;A fs=FAV(self)->fgh[0]; A gs=FAV(self)->fgh[1]; A p,z;I b;I j,m,n,zn;AHDR2FN* ado;
 ARGCHK2(a,w);
 A x=a, y=w; I an=AN(a), wn=AN(w);
 if((-an&-wn&-(AT(a)&AT(w))&LIT)>=0)R jtunderh2(jt,a,w,self);  // empty or not LIT, revert
 b=AR(a)<=AR(w); zn=b?wn:an; m=b?an:wn; n=zn/m;  // b = 'x is repeated'  m=length of low-rank arg n=#repeats of low-rank arg 
 ASSERTAGREE(AS(a),AS(w),MIN(AR(a),AR(w)));
 j=i0(VAV(fs)->fgh[1])-16;  // fetch boolean fn #
 GATV(z,LIT,zn,MAX(AR(a),AR(w)),AS(b?w:a));   // d is fixed; was d==SZI?LIT:C2T; would need GA then
 if(1==n)                 {ado=bwI[j]; m=(m+SZI-1)>>LGSZI;}  // for single loop we overwrite.  This means no inplacing
 else if((-AR(a)&-AR(w)&-(n&(SZI-1)))>=0){ado=bwI[j]; n=(n+SZI-1)>>LGSZI; A zz; RZ(p=IRS2(num(SZI),b?x:y,0L,0L,0L,jtrepeat,zz)); x=b?p:x; y=b?y:p;} // a atom or w atom, or multiple of SZI.  Replicate bytes to words in repeated arg
 else                      ado=bwC[j];
 ado AH2A(m,n==1?~m:2*(n^-b)+b,AV(x),AV(y),AV(z),jt);  // convert m to encoded length/repct
 RETF(z);
}

static AHDRRFN* bwinsC[16]={(AHDRRFN*)bw0000insC,(AHDRRFN*)bw0001insC,(AHDRRFN*)bw0010insC,(AHDRRFN*)bw0011insC, (AHDRRFN*)bw0100insC,(AHDRRFN*)bw0101insC,(AHDRRFN*)bw0110insC,(AHDRRFN*)bw0111insC,
                      (AHDRRFN*)bw1000insC,(AHDRRFN*)bw1001insC,(AHDRRFN*)bw1010insC,(AHDRRFN*)bw1011insC, (AHDRRFN*)bw1100insC,(AHDRRFN*)bw1101insC,(AHDRRFN*)bw1110insC,(AHDRRFN*)bw1111insC};

static AHDRRFN* bwinsI[16]={(AHDRRFN*)bw0000insI,(AHDRRFN*)bw0001insI,(AHDRRFN*)bw0010insI,(AHDRRFN*)bw0011insI, (AHDRRFN*)bw0100insI,(AHDRRFN*)bw0101insI,(AHDRRFN*)bw0110insI,(AHDRRFN*)bw0111insI,
                      (AHDRRFN*)bw1000insI,(AHDRRFN*)bw1001insI,(AHDRRFN*)bw1010insI,(AHDRRFN*)bw1011insI, (AHDRRFN*)bw1100insI,(AHDRRFN*)bw1101insI,(AHDRRFN*)bw1110insI,(AHDRRFN*)bw1111insI};

/* m b./&.(a.&i.) w */
/* m b./&.(a.i.]) w */
/* m e. 16+i.16     */

DF1(jtunderh1);
DF1(jtbitwiseinsertchar){F12IP;A fs,z;I d,j,n,r,wn,wr,zatoms;UC*u,*v,*wv,x,*zv;AHDRRFN* ado;
 ARGCHK2(w,self);
 wr=AR(w); wn=AN(w); SETIC(w,n); z=VAV(self)->fgh[0]; fs=VAV(z)->fgh[0];
 if((-(wn)&(SZI-n)&SGNIF(AT(w),LITX))>=0)R jtunderh1(jt,w,self);  // revert if not wn!=0 & n>SZI & LIT
 PROD(d,wr-1,AS(w)+1); zatoms=d; wv=CAV(w); j=i0(VAV(fs)->fgh[1])-16; ado=(AHDRRFN*)bwinsC[j];  // d=#atoms in an item of a cell.  There is only 1 cell here (rank _)
 if(1==wr)
  switch(j){   // d==1 here
  case  0: R scc(0);
  case  3: R scc(wv[0]);
  case  5: R scc(wv[wn-1]);
  case 10: x=wv[wn-1]; R scc((UC)(((wn&1)-1))^x);
  case 12: R scc((UC)~wv[0]);
  case 15: R scc((UC)255);
  case  1: case 6: case 7: case 9: ado=(AHDRRFN*)bwinsI[j]; n=n>>LGSZI;  // this gets # full words in the list arg.  Handle as ints.  Remnant handled below
  }
 else if(0==(d&(SZI-1))){ado=(AHDRRFN*)bwinsI[j]; d>>=LGSZI;}  //if #atoms are a word multiple, switch to handling ints
 GATV(z,LIT,zatoms,wr-1,1+AS(w)); zv=CAVn(wr-1,z);
 ado(d,n,1L,wv,zv,jt);
 if(1==wr){
  r=wn-(n<<LGSZI); u=wv+(n<<LGSZI); x=*zv; v=1+zv; 
  switch(j){  // Handle the remnant for fullword ops
  case 1: DQ(SZI-1, x=BW0001(x,*v); ++v;); DQ(r, x=BW0001(x,*u); ++u;); break;
  case 6: DQ(SZI-1, x=BW0110(x,*v); ++v;); DQ(r, x=BW0110(x,*u); ++u;); break;
  case 7: DQ(SZI-1, x=BW0111(x,*v); ++v;); DQ(r, x=BW0111(x,*u); ++u;); break;
  case 9: DQ(SZI-1, x=BW1001(x,*v); ++v;); DQ(r, x=BW1001(x,*u); ++u;); break;
  }
  *zv=x;
 }
 R z;
}
