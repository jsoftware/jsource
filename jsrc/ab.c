/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: b. bitwise functions                                           */

// TODO: rewrite & simplify, and remove divides
// note u b./&.(a.&i.) fails when y is shorter than an int
#include "j.h"
#include "ve.h"
#include "ar.h"


APFX(bw0000II, UI,UI,UI, BW0000,, R EVOK;)       static APFX(bw0000CC, UC,UC,UC, BW0000,, R EVOK;)
APFX(bw0001II, UI,UI,UI, BW0001,, R EVOK;)       static APFX(bw0001CC, UC,UC,UC, BW0001,, R EVOK;)
APFX(bw0010II, UI,UI,UI, BW0010,, R EVOK;)       static APFX(bw0010CC, UC,UC,UC, BW0010,, R EVOK;)
APFX(bw0011II, UI,UI,UI, BW0011,, R EVOK;)       static APFX(bw0011CC, UC,UC,UC, BW0011,, R EVOK;)
APFX(bw0100II, UI,UI,UI, BW0100,, R EVOK;)       static APFX(bw0100CC, UC,UC,UC, BW0100,, R EVOK;)
APFX(bw0101II, UI,UI,UI, BW0101,, R EVOK;)       static APFX(bw0101CC, UC,UC,UC, BW0101,, R EVOK;)
APFX(bw0110II, UI,UI,UI, BW0110,, R EVOK;)       static APFX(bw0110CC, UC,UC,UC, BW0110,, R EVOK;)
APFX(bw0111II, UI,UI,UI, BW0111,, R EVOK;)       static APFX(bw0111CC, UC,UC,UC, BW0111,, R EVOK;)
APFX(bw1000II, UI,UI,UI, BW1000,, R EVOK;)       static APFX(bw1000CC, UC,UC,UC, BW1000,, R EVOK;)
APFX(bw1001II, UI,UI,UI, BW1001,, R EVOK;)       static APFX(bw1001CC, UC,UC,UC, BW1001,, R EVOK;)
APFX(bw1010II, UI,UI,UI, BW1010,, R EVOK;)       static APFX(bw1010CC, UC,UC,UC, BW1010,, R EVOK;)
APFX(bw1011II, UI,UI,UI, BW1011,, R EVOK;)       static APFX(bw1011CC, UC,UC,UC, BW1011,, R EVOK;)
APFX(bw1100II, UI,UI,UI, BW1100,, R EVOK;)       static APFX(bw1100CC, UC,UC,UC, BW1100,, R EVOK;)
APFX(bw1101II, UI,UI,UI, BW1101,, R EVOK;)       static APFX(bw1101CC, UC,UC,UC, BW1101,, R EVOK;)
APFX(bw1110II, UI,UI,UI, BW1110,, R EVOK;)       static APFX(bw1110CC, UC,UC,UC, BW1110,, R EVOK;)
APFX(bw1111II, UI,UI,UI, BW1111,, R EVOK;)       static APFX(bw1111CC, UC,UC,UC, BW1111,, R EVOK;)

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

       AHDRR(bw0000insI,UI,UI){I k=SZI*m*d; memset(z,C0 ,k); R EVOK;}
static AHDRR(bw0000insC,UC,UC){I k=    m*d; memset(z,C0 ,k); R EVOK;}

       AHDRR(bw1111insI,UI,UI){I k=SZI*m*d; memset(z,CFF,k); R EVOK;}
static AHDRR(bw1111insC,UC,UC){I k=    m*d; memset(z,CFF,k); R EVOK;}

       AHDRR(bw0011insI,UI,UI){I k=d*(n-1);                        DQ(m, DQ(d, *z++=  *x++;); x+=k;);R EVOK;}
static AHDRR(bw0011insC,UC,UC){I k=d*(n-1);                        DQ(m, DQ(d, *z++=  *x++;); x+=k;);R EVOK;}

       AHDRR(bw1100insI,UI,UI){I k=d*(n-1);                 if(1<n)DQ(m, DQ(d, *z++= ~*x++;); x+=k;) else MC(z,x,SZI*m*d);R EVOK;}
static AHDRR(bw1100insC,UC,UC){I k=d*(n-1);                 if(1<n)DQ(m, DQ(d, *z++= ~*x++;); x+=k;) else MC(z,x,    m*d);R EVOK;}

       AHDRR(bw0101insI,UI,UI){I k=d*(n-1);                  x+=k; DQ(m, DQ(d, *z++=  *x++;); x+=k;);R EVOK;}
static AHDRR(bw0101insC,UC,UC){I k=d*(n-1);                  x+=k; DQ(m, DQ(d, *z++=  *x++;); x+=k;);R EVOK;}

       AHDRR(bw1010insI,UI,UI){I k=d*(n-1);UI t=     (n&1)-1 ; x+=k; DQ(m, DQ(d, *z++=t^*x++;); x+=k;);R EVOK;}
static AHDRR(bw1010insC,UC,UC){I k=d*(n-1);UC t=(UC)((n&1)-1); x+=k; DQ(m, DQ(d, *z++=t^*x++;); x+=k;);R EVOK;}



#define BITWISE(f,T,op)  \
 F2(f){A z;I *av,k=0,x;T*wv,y,*zv;             \
  F2PREFIP;ARGCHK2(a,w);  /* kludge we allow inplace call but we don't honor it yet */ \
  if(!(INT&AT(a)))RZ(a=cvt(INT,a));                                    \
  if(!(INT&AT(w)))RZ(w=cvt(INT,w));                                    \
  av=(I*)AV(a);                          \
  wv=(T*)AV(w);                 \
  ASSERTAGREE(AS(a),AS(w),MIN(AR(a),AR(w)));                      \
  GATV(z,INT,AN(AR(a)>AR(w)?a:w),MAX(AR(a),AR(w)),AS(AR(a)>AR(w)?a:w)); zv=(T*)AV(z);                  \
  if(!AN(z))R z;                                                       \
  if     (AR(a)==AR(w))DQ(AN(a), x=*av++;           y=*wv++; *zv++=op(x,y);  )  \
  else if(AR(a)< AR(w))DQ(AN(a), x=*av++; DQ(AN(w)/AN(a), y=*wv++; *zv++=op(x,y););)  \
  else           DQ(AN(w), y=*wv++; DQ(AN(a)/AN(w), x=*av++; *zv++=op(x,y););); \
  RE(0); RETF(z);                                                          \
 }

#define BWROT(x,y)      ((y<<(x&(BW-1)))|(y>>(BW-(x&(BW-1)))))
#define BWSHIFT(x,y)    (0>x ? (x<=-BW?0:y>>-x)           : (x>=BW?0:y<<x)            )
#define BWSHIFTA(x,y)   (0>x ? (x<=-BW?(y<0?-1:0):y>>-x)  : (x>=BW?0:y<<x)            )

BITWISE(jtbitwiserotate,UI,BWROT   )
BITWISE(jtbitwiseshift, UI,BWSHIFT )
BITWISE(jtbitwiseshifta,I, BWSHIFTA)

DF1(jtbitwise1){R CALL2(FAV(self)->valencefns[1],zeroionei(0),w,self);}   // inplaceable - don't touch jt


static AHDR2FN* bwC[16]={(AHDR2FN*)bw0000CC,(AHDR2FN*)bw0001CC,(AHDR2FN*)bw0010CC,(AHDR2FN*)bw0011CC, (AHDR2FN*)bw0100CC,(AHDR2FN*)bw0101CC,(AHDR2FN*)bw0110CC,(AHDR2FN*)bw0111CC,
                   (AHDR2FN*)bw1000CC,(AHDR2FN*)bw1001CC,(AHDR2FN*)bw1010CC,(AHDR2FN*)bw1011CC, (AHDR2FN*)bw1100CC,(AHDR2FN*)bw1101CC,(AHDR2FN*)bw1110CC,(AHDR2FN*)bw1111CC};

static AHDR2FN* bwI[16]={(AHDR2FN*)bw0000II,(AHDR2FN*)bw0001II,(AHDR2FN*)bw0010II,(AHDR2FN*)bw0011II, (AHDR2FN*)bw0100II,(AHDR2FN*)bw0101II,(AHDR2FN*)bw0110II,(AHDR2FN*)bw0111II,
                   (AHDR2FN*)bw1000II,(AHDR2FN*)bw1001II,(AHDR2FN*)bw1010II,(AHDR2FN*)bw1011II, (AHDR2FN*)bw1100II,(AHDR2FN*)bw1101II,(AHDR2FN*)bw1110II,(AHDR2FN*)bw1111II};

/* a m b.&.(a.&i.) w */
/* a m b.&.(a.i.]) w */
/* m e. 16+i.16      */

DF2(jtbitwisechar){DECLFG;A*p,x,y,z;B b;I j,m,n,zn;AHDR2FN* ado;
 ARGCHK2(a,w);
 x=a;
 y=w;
 if((-AN(a)&-AN(w)&-(AT(a)&AT(w))&LIT)>=0)R from(df2(z,indexof(ds(CALP),a),indexof(ds(CALP),w),fs),ds(CALP));  // empty or not LIT
 b=AR(a)<=AR(w); zn=AN(b?w:a); m=AN(b?a:w); n=zn/m;
 ASSERTAGREE(AS(a),AS(w),MIN(AR(a),AR(w)));
 j=i0(VAV(fs)->fgh[1])-16;
 GATV(z,LIT,zn,MAX(AR(a),AR(w)),AS(b?w:a));   // d is fixed; was d==SZI?LIT:C2T; would need GA then
 if(1==n)                 {ado=bwI[j]; m=(m+SZI-1)>>LGSZI;}
 else if((-AR(a)&-AR(w)&-(n&(SZI-1)))>=0){ado=bwI[j]; n=(n+SZI-1)>>LGSZI; p=b?&x:&y; A zz; RZ(*p=IRS2(sc(SZI),*p,0L,0L,0L,jtrepeat,zz));} // a atom, w atom, or multiple of SZI
 else                      ado=bwC[j];
 n^=-b; n=(n==~1)?1:n;  // encode b flag in sign of n
 ado(n,m,AV(x),AV(y),AV(z),jt); 
 *(zn+CAV(z))=0;
 RETF(z);
}

/* compute z=: t{~ a.i.w if t=: c&(m b.) a.                             */
/* http://www.jsoftware.com/jwiki/Essays/Bitwise_Functions_on_Characters */

B jtbitwisecharamp(J jt,UC*t,I n,UC*wv,UC*zv){I p;UC c,i,j,*pv,s[256];AHDR2FN* ado;
 i=t[0]; j=t[255];
 if     (i==0    ){c=j; ado=(AHDR2FN*)bw0001II;}
 else if(j==i    ){c=i; ado=(AHDR2FN*)bw0011II;}
 else if(j==255  ){c=i; ado=(AHDR2FN*)bw0111II;}
 else if(j==255-i){c=i; ado=(AHDR2FN*)bw0110II;}
 else if(j==0    ){c=i; ado=(AHDR2FN*)bw0010II;}
 else if(i==255  ){c=j; ado=(AHDR2FN*)bw1011II;}
 else R 0;
 pv=(UC*)&p; DO(SZI, pv[i]=c;);
 ado((I)(256/SZI),(I)1,AV(ds(CALP)),pv,s,jt); if(memcmpne(s,t,256L))R 0;
 ado((n+SZI-1)>>LGSZI,(I)1,wv,pv,zv,jt); zv[n]=0;
 R 1;
}  // kludge this should be scrapped in favor of wordlong ops


static AHDRRFN* bwinsC[16]={(AHDRRFN*)bw0000insC,(AHDRRFN*)bw0001insC,(AHDRRFN*)bw0010insC,(AHDRRFN*)bw0011insC, (AHDRRFN*)bw0100insC,(AHDRRFN*)bw0101insC,(AHDRRFN*)bw0110insC,(AHDRRFN*)bw0111insC,
                      (AHDRRFN*)bw1000insC,(AHDRRFN*)bw1001insC,(AHDRRFN*)bw1010insC,(AHDRRFN*)bw1011insC, (AHDRRFN*)bw1100insC,(AHDRRFN*)bw1101insC,(AHDRRFN*)bw1110insC,(AHDRRFN*)bw1111insC};

static AHDRRFN* bwinsI[16]={(AHDRRFN*)bw0000insI,(AHDRRFN*)bw0001insI,(AHDRRFN*)bw0010insI,(AHDRRFN*)bw0011insI, (AHDRRFN*)bw0100insI,(AHDRRFN*)bw0101insI,(AHDRRFN*)bw0110insI,(AHDRRFN*)bw0111insI,
                      (AHDRRFN*)bw1000insI,(AHDRRFN*)bw1001insI,(AHDRRFN*)bw1010insI,(AHDRRFN*)bw1011insI, (AHDRRFN*)bw1100insI,(AHDRRFN*)bw1101insI,(AHDRRFN*)bw1110insI,(AHDRRFN*)bw1111insI};

/* m b./&.(a.&i.) w */
/* m b./&.(a.i.]) w */
/* m e. 16+i.16     */

DF1(jtbitwiseinsertchar){A fs,z;I d,j,n,r,wn,wr,zatoms;UC*u,*v,*wv,x,*zv;AHDRRFN* ado;
 ARGCHK2(w,self);
 wr=AR(w); wn=AN(w); SETIC(w,n); z=VAV(self)->fgh[0]; fs=VAV(z)->fgh[0];
 if((-(wn)&(SZI-n)&SGNIF(AT(w),LITX))>=0)R from(df1(z,indexof(ds(CALP),w),fs),ds(CALP));  // revert if not wn!=0 & n>SZI & LIT
 PROD(d,wr-1,AS(w)+1); zatoms=d; wv=CAV(w); j=i0(VAV(fs)->fgh[1])-16; ado=(AHDRRFN*)bwinsC[j];  // d=#atoms in an item of a cell.  There is only 1 cell here (rank _)
 if(1==wr)switch(j){   // d==1 here
  case  0: R scc(0);
  case  3: R scc(*wv);
  case  5: R scc(*(wv+wn-1));
  case 10: x=*(wv+wn-1); R scc((UC)(((wn&1)-1))^x);
  case 12: R scc((UC)~*wv);
  case 15: R scc((UC)255);
  case  1: case 6: case 7: case 9: ado=(AHDRRFN*)bwinsI[j]; n=n>>LGSZI;  // this gets # full words in the list arg.  Handle as ints.  Remnant handled below
 }else if(0==(d&(SZI-1))){ado=(AHDRRFN*)bwinsI[j]; d>>=LGSZI;}  //if #atoms are a word multiple, switch to handling ints
 GATV(z,LIT,zatoms,wr-1,1+AS(w)); zv=CAV(z);
 ado(d,n,1L,wv,zv,jt);
 if(1==wr){
  r=wn-(n<<LGSZI); u=wv+(n<<LGSZI); x=*zv; v=1+zv; 
  switch(j){  // Handle the remnant for fullword ops
   case 1: DQ(SZI-1, x=BW0001(x,*v); ++v;); DQ(r, x=BW0001(x,*u); ++u;); break;
   case 6: DQ(SZI-1, x=BW0110(x,*v); ++v;); DQ(r, x=BW0110(x,*u); ++u;); break;
   case 7: DQ(SZI-1, x=BW0111(x,*v); ++v;); DQ(r, x=BW0111(x,*u); ++u;); break;
   case 9: DQ(SZI-1, x=BW1001(x,*v); ++v;); DQ(r, x=BW1001(x,*u); ++u;); break;
  }
  *(I*)zv=0; *zv=x;
 }
 R z;
}
