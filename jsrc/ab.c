/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: b. bitwise functions                                           */

// TODO: rewrite & simplify, and remove divides
// note u b./&.(a.&i.) fails when y is shorter than an int
#include "j.h"
#include "ve.h"
#include "ar.h"


APFX(bw0000II, UI,UI,UI, BW0000)       static APFX(bw0000CC, UC,UC,UC, BW0000)
APFX(bw0001II, UI,UI,UI, BW0001)       static APFX(bw0001CC, UC,UC,UC, BW0001)
APFX(bw0010II, UI,UI,UI, BW0010)       static APFX(bw0010CC, UC,UC,UC, BW0010)
APFX(bw0011II, UI,UI,UI, BW0011)       static APFX(bw0011CC, UC,UC,UC, BW0011)
APFX(bw0100II, UI,UI,UI, BW0100)       static APFX(bw0100CC, UC,UC,UC, BW0100)
APFX(bw0101II, UI,UI,UI, BW0101)       static APFX(bw0101CC, UC,UC,UC, BW0101)
APFX(bw0110II, UI,UI,UI, BW0110)       static APFX(bw0110CC, UC,UC,UC, BW0110)
APFX(bw0111II, UI,UI,UI, BW0111)       static APFX(bw0111CC, UC,UC,UC, BW0111)
APFX(bw1000II, UI,UI,UI, BW1000)       static APFX(bw1000CC, UC,UC,UC, BW1000)
APFX(bw1001II, UI,UI,UI, BW1001)       static APFX(bw1001CC, UC,UC,UC, BW1001)
APFX(bw1010II, UI,UI,UI, BW1010)       static APFX(bw1010CC, UC,UC,UC, BW1010)
APFX(bw1011II, UI,UI,UI, BW1011)       static APFX(bw1011CC, UC,UC,UC, BW1011)
APFX(bw1100II, UI,UI,UI, BW1100)       static APFX(bw1100CC, UC,UC,UC, BW1100)
APFX(bw1101II, UI,UI,UI, BW1101)       static APFX(bw1101CC, UC,UC,UC, BW1101)
APFX(bw1110II, UI,UI,UI, BW1110)       static APFX(bw1110CC, UC,UC,UC, BW1110)
APFX(bw1111II, UI,UI,UI, BW1111)       static APFX(bw1111CC, UC,UC,UC, BW1111)

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

       AHDRR(bw0000insI,UI,UI){I k=SZI*m*d; if(1<n)memset(z,C0 ,k); else MC(z,x,k);}
static AHDRR(bw0000insC,UC,UC){I k=    m*d; if(1<n)memset(z,C0 ,k); else MC(z,x,k);}

       AHDRR(bw1111insI,UI,UI){I k=SZI*m*d; if(1<n)memset(z,CFF,k); else MC(z,x,k);}
static AHDRR(bw1111insC,UC,UC){I k=    m*d; if(1<n)memset(z,CFF,k); else MC(z,x,k);}

       AHDRR(bw0011insI,UI,UI){I k=d*(n-1);                        DO(m, DO(d, *z++=  *x++;); x+=k;);}
static AHDRR(bw0011insC,UC,UC){I k=d*(n-1);                        DO(m, DO(d, *z++=  *x++;); x+=k;);}

       AHDRR(bw1100insI,UI,UI){I k=d*(n-1);                 if(1<n)DO(m, DO(d, *z++= ~*x++;); x+=k;) else MC(z,x,SZI*m*d);}
static AHDRR(bw1100insC,UC,UC){I k=d*(n-1);                 if(1<n)DO(m, DO(d, *z++= ~*x++;); x+=k;) else MC(z,x,    m*d);}

       AHDRR(bw0101insI,UI,UI){I k=d*(n-1);                  x+=k; DO(m, DO(d, *z++=  *x++;); x+=k;);}
static AHDRR(bw0101insC,UC,UC){I k=d*(n-1);                  x+=k; DO(m, DO(d, *z++=  *x++;); x+=k;);}

       AHDRR(bw1010insI,UI,UI){I k=d*(n-1);UI t=     (n&1)-1 ; x+=k; DO(m, DO(d, *z++=t^*x++;); x+=k;);}
static AHDRR(bw1010insC,UC,UC){I k=d*(n-1);UC t=(UC)((n&1)-1); x+=k; DO(m, DO(d, *z++=t^*x++;); x+=k;);}



#define BITWISE(f,T,op)  \
 F2(f){A z;I *av,k=0,x;T*wv,y,*zv;             \
  RZ(a&&w);F2PREFIP;  /* kludge we allow inplace call but we don't honor it yet */ \
  if(!(INT&AT(a)))RZ(a=cvt(INT,a));                                    \
  if(!(INT&AT(w)))RZ(w=cvt(INT,w));                                    \
  av=(I*)AV(a);                          \
  wv=(T*)AV(w);                 \
  ASSERTAGREE(AS(a),AS(w),MIN(AR(a),AR(w)));                      \
  GATV(z,INT,AN(AR(a)>AR(w)?a:w),MAX(AR(a),AR(w)),AS(AR(a)>AR(w)?a:w)); zv=(T*)AV(z);                  \
  if(!AN(z))R z;                                                       \
  if     (AR(a)==AR(w))DO(AN(a), x=*av++;           y=*wv++; *zv++=op(x,y);  )  \
  else if(AR(a)< AR(w))DO(AN(a), x=*av++; DO(AN(w)/AN(a), y=*wv++; *zv++=op(x,y););)  \
  else           DO(AN(w), y=*wv++; DO(AN(a)/AN(w), x=*av++; *zv++=op(x,y););); \
  RE(0); RETF(z);                                                          \
 }

#define BWROT(x,y)      ((y<<(x&(BW-1)))|(y>>(BW-(x&(BW-1)))))
#define BWSHIFT(x,y)    (0>x ? (x<=-BW?0:y>>-x)           : (x>=BW?0:y<<x)            )
#define BWSHIFTA(x,y)   (0>x ? (x<=-BW?(y<0?-1:0):y>>-x)  : (x>=BW?0:y<<x)            )

BITWISE(jtgenbitwiserotate,UI,BWROT   )
BITWISE(jtgenbitwiseshift, UI,BWSHIFT )
BITWISE(jtgenbitwiseshifta,I, BWSHIFTA)

DF1(jtbitwise1){R CALL2(FAV(self)->valencefns[1],zeroionei[0],w,self);}   // inplaceable - don't touch jt


static VF bwC[16]={(VF)bw0000CC,(VF)bw0001CC,(VF)bw0010CC,(VF)bw0011CC, (VF)bw0100CC,(VF)bw0101CC,(VF)bw0110CC,(VF)bw0111CC,
                   (VF)bw1000CC,(VF)bw1001CC,(VF)bw1010CC,(VF)bw1011CC, (VF)bw1100CC,(VF)bw1101CC,(VF)bw1110CC,(VF)bw1111CC};

static VF bwI[16]={(VF)bw0000II,(VF)bw0001II,(VF)bw0010II,(VF)bw0011II, (VF)bw0100II,(VF)bw0101II,(VF)bw0110II,(VF)bw0111II,
                   (VF)bw1000II,(VF)bw1001II,(VF)bw1010II,(VF)bw1011II, (VF)bw1100II,(VF)bw1101II,(VF)bw1110II,(VF)bw1111II};

/* a m b.&.(a.&i.) w */
/* a m b.&.(a.i.]) w */
/* m e. 16+i.16      */

DF2(jtbitwisechar){DECLFG;A*p,x,y,z;B b;I j,m,n,zn;VF f;
 RZ(a&&w);
 x=a;
 y=w;
 if(!(AN(a)&&AN(w)&&(AT(a)&AT(w))&LIT))R from(df2(indexof(alp,a),indexof(alp,w),fs),alp);
 b=AR(a)<=AR(w); zn=AN(b?w:a); m=AN(b?a:w); n=zn/m;
 ASSERTAGREE(AS(a),AS(w),MIN(AR(a),AR(w)));
 j=i0(VAV(fs)->fgh[0])-16;
 GATV(z,LIT,zn,MAX(AR(a),AR(w)),AS(b?w:a));   // d is fixed; was d==SZI?LIT:C2T; would need GA then
 if(1==n)                 {f=bwI[j]; m=(m+SZI-1)>>LGSZI;}
 else if(!AR(a)||!AR(w)||0==(n&(SZI-1))){f=bwI[j]; n=(n+SZI-1)>>LGSZI; p=b?&x:&y; RZ(*p=irs2(sc(SZI),*p,0L,0L,0L,jtrepeat));}
 else                      f=bwC[j];
 n^=-b; n=(n==~1)?1:n;  // encode b flag in sign of n
 f(jt,m,AV(z),AV(x),AV(y),n); 
 *(zn+CAV(z))=0;
 RETF(z);
}

/* compute z=: t{~ a.i.w if t=: c&(m b.) a.                             */
/* http://www.jsoftware.com/jwiki/Essays/Bitwise_Functions_on_Characters */

B jtbitwisecharamp(J jt,UC*t,I n,UC*wv,UC*zv){I p;UC c,i,j,*pv,s[256];VF f;
 i=t[0]; j=t[255];
 if     (i==0    ){c=j; f=(VF)bw0001II;}
 else if(j==i    ){c=i; f=(VF)bw0011II;}
 else if(j==255  ){c=i; f=(VF)bw0111II;}
 else if(j==255-i){c=i; f=(VF)bw0110II;}
 else if(j==0    ){c=i; f=(VF)bw0010II;}
 else if(i==255  ){c=j; f=(VF)bw1011II;}
 else R 0;
 pv=(UC*)&p; DO(SZI, pv[i]=c;);
// obsolete  f(jt,1,1L,256L/SZI,s,pv,AV(alp)); if(memcmp(s,t,256L))R 0;
// obsolete  f(jt,1,1L,(n+SZI-1)>>LGSZI,zv,pv,wv); zv[n]=0;
 f(jt,(I)1,s,AV(alp),pv,(I)(256/SZI)); if(memcmp(s,t,256L))R 0;
 f(jt,(I)1,zv,wv,pv,(n+SZI-1)>>LGSZI); zv[n]=0;
 R 1;
}  // kludge this should be scrapped in favor of wordlong ops


static VF bwinsC[16]={bw0000insC,bw0001insC,bw0010insC,bw0011insC, bw0100insC,bw0101insC,bw0110insC,bw0111insC,
                      bw1000insC,bw1001insC,bw1010insC,bw1011insC, bw1100insC,bw1101insC,bw1110insC,bw1111insC};

static VF bwinsI[16]={bw0000insI,bw0001insI,bw0010insI,bw0011insI, bw0100insI,bw0101insI,bw0110insI,bw0111insI,
                      bw1000insI,bw1001insI,bw1010insI,bw1011insI, bw1100insI,bw1101insI,bw1110insI,bw1111insI};

/* m b./&.(a.&i.) w */
/* m b./&.(a.i.]) w */
/* m e. 16+i.16     */

DF1(jtbitwiseinsertchar){A fs,z;I d,j,n,r,wn,wr,zatoms;UC*u,*v,*wv,x,*zv;VF f;
 RZ(w&&self);
 wr=AR(w); wn=AN(w); n=wr?*AS(w):1; z=VAV(self)->fgh[0]; fs=VAV(z)->fgh[0];
 if(!(wn&&SZI<n&&LIT&AT(w)))R from(df1(indexof(alp,w),fs),alp);
 PROD(d,wr-1,AS(w)+1); zatoms=d; wv=CAV(w); j=i0(VAV(fs)->fgh[0])-16; f=bwinsC[j];  // d=#atoms in an item of a cell.  There is only 1 cell here (rank _)
 if(1==wr)switch(j){   // d==1 here
  case  0: R scc(0);
  case  3: R scc(*wv);
  case  5: R scc(*(wv+wn-1));
  case 10: x=*(wv+wn-1); R scc((UC)(((wn&1)-1))^x);
  case 12: R scc((UC)~*wv);
  case 15: R scc((UC)255);
  case  1: case 6: case 7: case 9: f=bwinsI[j]; n=n>>LGSZI;  // this gets # full words in the list arg.  Handle as ints.  Remnant handled below
 }else if(0==(d&(SZI-1))){f=bwinsI[j]; d>>=LGSZI;}  //if #atoms are a word multiple, switch to handling ints
 GATV(z,LIT,zatoms,wr-1,1+AS(w)); zv=CAV(z);
 f(jt,1L,d,n,zv,wv);
 if(1==wr){
  r=wn-(n<<LGSZI); u=wv+(n<<LGSZI); x=*zv; v=1+zv; 
  switch(j){  // Handle the remnant for fullword ops
   case 1: DO(SZI-1, x=BW0001(x,*v); ++v;); DO(r, x=BW0001(x,*u); ++u;); break;
   case 6: DO(SZI-1, x=BW0110(x,*v); ++v;); DO(r, x=BW0110(x,*u); ++u;); break;
   case 7: DO(SZI-1, x=BW0111(x,*v); ++v;); DO(r, x=BW0111(x,*u); ++u;); break;
   case 9: DO(SZI-1, x=BW1001(x,*v); ++v;); DO(r, x=BW1001(x,*u); ++u;); break;
  }
  *(I*)zv=0; *zv=x;
 }
 R z;
}
