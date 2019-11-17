/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Comparatives                                                     */
/*                                                                         */
/* Special code for the following cases:                                   */
/*    comp i. 0:          i.&0@:comp                                       */
/*    comp i. 1:          i.&1@:comp                                       */
/*    comp i: 0:          i:&0@:comp                                       */
/*    comp i: 1:          i:&1@:comp                                       */
/*    [: + / comp         + /@:comp                                        */
/*    [: +./ comp         +./@:comp   1: e. comp                           */
/*    0: e. comp                                                           */
/*    [: I. comp          I.@:comp       no longer supported               */
/* where comp is one of the following:                                     */
/*    = ~: < <: >: > E. e.                                                 */

#include "j.h"
#include "ve.h"
#include "vcomp.h"


#define INDF(f,T0,T1,F)  \
 static F2(f){I n;T0*av,x;T1*wv,y;                                      \
  av=(T0*)AV(a);                                                    \
  wv=(T1*)AV(w);             \
  if     (!AR(a)){x=*av;        DP(n=AN(w),          y=*wv++; if(F(x,y))R sc(n+i););} \
  else if(!AR(w)){y=*wv;        DP(n=AN(a), x=*av++;          if(F(x,y))R sc(n+i););} \
  else           {              DP(n=AN(w), x=*av++; y=*wv++; if(F(x,y))R sc(n+i););} \
  R sc(n);                                                                    \
 }
#define INDF0(f,T0,T1,F,C)  \
 static F2(f){I n;T0*av,x;T1*wv,y;                                      \
  av=(T0*)AV(a);                                                    \
  wv=(T1*)AV(w);             \
  if     (!AR(a)){x=*av;        if(1.0==jt->cct)DP(n=AN(w),          y=*wv++; if(C(x,y))R sc(n+i);)else DP(n=AN(w),          y=*wv++; if(F(x,y))R sc(n+i);)} \
  else if(!AR(w)){y=*wv;        if(1.0==jt->cct)DP(n=AN(a), x=*av++;          if(C(x,y))R sc(n+i);)else DP(n=AN(a), x=*av++;          if(F(x,y))R sc(n+i);)} \
  else           {              if(1.0==jt->cct)DP(n=AN(w), x=*av++; y=*wv++; if(C(x,y))R sc(n+i);)else DP(n=AN(w), x=*av++; y=*wv++; if(F(x,y))R sc(n+i);)} \
  R sc(n);                                                                    \
 }

#define JNDF(f,T0,T1,F)  \
 static F2(f){I n;T0*av,x;T1*wv,y;                                          \
  av=(T0*)AV(a);                                                        \
  wv=(T1*)AV(w);                  \
  if     (!AR(a)){x=*av; wv+=AN(w); DQ(n=AN(w),          y=*--wv; if(F(x,y))R sc(i););} \
  else if(!AR(w)){y=*wv; av+=AN(a); DQ(n=AN(a), x=*--av;          if(F(x,y))R sc(i););} \
  else           {av+=AN(w); wv+=AN(w); DQ(n=AN(w), x=*--av; y=*--wv; if(F(x,y))R sc(i););} \
  R sc(n);                                                                        \
 }
#define JNDF0(f,T0,T1,F,C)  \
 static F2(f){I n;T0*av,x;T1*wv,y;                                          \
  av=(T0*)AV(a);                                                        \
  wv=(T1*)AV(w);                  \
  if     (!AR(a)){x=*av; wv+=AN(w); if(1.0==jt->cct)DQ(n=AN(w),          y=*--wv; if(C(x,y))R sc(i);)else DQ(n=AN(w),          y=*--wv; if(F(x,y))R sc(i);)} \
  else if(!AR(w)){y=*wv; av+=AN(a); if(1.0==jt->cct)DQ(n=AN(a), x=*--av;          if(C(x,y))R sc(i);)else DQ(n=AN(a), x=*--av;          if(F(x,y))R sc(i);)} \
  else           {av+=AN(w); wv+=AN(w); if(1.0==jt->cct)DQ(n=AN(w), x=*--av; y=*--wv; if(C(x,y))R sc(i);)else DQ(n=AN(w), x=*--av; y=*--wv; if(F(x,y))R sc(i);)} \
  R sc(n);                                                                        \
 }

#define SUMF(f,T0,T1,F)  \
 static F2(f){I m=0,n;T0*av,x;T1*wv,y;                       \
  av=(T0*)AV(a);                                         \
  wv=(T1*)AV(w);   \
  if     (!AR(a)){x=*av; DQ(n=AN(w),          y=*wv++; m+=(F(x,y)););} \
  else if(!AR(w)){y=*wv; DQ(n=AN(a), x=*av++;          m+=(F(x,y)););} \
  else           {       DQ(n=AN(w), x=*av++; y=*wv++; m+=(F(x,y)););} \
  R sc(m);                                                         \
 }
#define SUMF0(f,T0,T1,F,C)  \
 static F2(f){I m=0,n;T0*av,x;T1*wv,y;                       \
  av=(T0*)AV(a);                                         \
  wv=(T1*)AV(w);   \
  if     (!AR(a)){x=*av; if(1.0==jt->cct)DQ(n=AN(w),          y=*wv++; m+=(C(x,y));)else DQ(n=AN(w),          y=*wv++; m+=(F(x,y));)} \
  else if(!AR(w)){y=*wv; if(1.0==jt->cct)DQ(n=AN(a), x=*av++;          m+=(C(x,y));)else DQ(n=AN(a), x=*av++;          m+=(F(x,y));)} \
  else           {       if(1.0==jt->cct)DQ(n=AN(w), x=*av++; y=*wv++; m+=(C(x,y));)else DQ(n=AN(w), x=*av++; y=*wv++; m+=(F(x,y));)} \
  R sc(m);                                                         \
 }


/* Now define byte-parallel (4 bytes at a time) versions of above  */

#define JNDBR(yy)      if(r&&(y=yy))DO(r, if(yv[r-1-i])R sc(n-1-i););

#define ASSIGNX(v)     {x=*(C*)v; x|=x<<8; x|=x<<16; x|=x<<(32&(BW-1)); }
#define INDB3          {n=(UI)n>i*SZI+(CTTZI(y)>>LGBB)?i*SZI+(CTTZI(y)>>LGBB):n; break;}
#define JNDB3          {UI4 bitno; CTLZI(y,bitno); n=(i*SZI+(bitno>>LGBB)); break;}

#define INDB(f,T0,T1,F)  \
 static F2(f){I an,*av,n,q,wn,*wv,x,y;                                 \
  an=AN(a); av=AV(a);                                                             \
  wn=AN(w); wv=AV(w); n=1; n=AR(a)?an:n; n=AR(w)?wn:n;                      \
  q=(n+(SZI-1))>>LGSZI;                                           \
  if     (!AR(a)){ASSIGNX(av); DO(q, if(y=F(x,    *wv++))INDB3;);}  \
  else if(!AR(w)){ASSIGNX(wv); DO(q, if(y=F(*av++,x    ))INDB3;);}  \
  else           {             DO(q, if(y=F(*av++,*wv++))INDB3;);}  \
  R sc(n);                                                                        \
 }
#if 0 // obsolete 
  wn=AN(w); wv=AV(w); n=AR(a)&&AR(w)?MAX(an,wn):AR(a)?an:wn;                      \

#endif

#define JNDB(f,T0,T1,F)  \
 static F2(f){I an,*av,n,q,r,wn,*wv,x,y;                                             \
  an=AN(a); av=AV(a);                                                                         \
  wn=AN(w); wv=AV(w); n=1; n=AR(a)?an:n; n=AR(w)?wn:n;                      \
  if((q=(n-1)>>LGSZI)<0)R zeroionei[0]; r=((n-1)&(SZI-1));  /* # first bytes to do minus 1 */                                                                           \
  I i=q;                                                                       \
  if     (!AR(a)){ASSIGNX(av); wv+=q; y=(F(x,*wv))&(((I)0x100<<(r<<3))-1); while(1){if(y)JNDB3; if(--i<0)break; y=F(x,*--wv);} }  \
  else if(!AR(w)){ASSIGNX(wv); av+=q; y=(F(*av,x))&(((I)0x100<<(r<<3))-1); while(1){if(y)JNDB3; if(--i<0)break; y=F(*--av,x);} }  \
  else           {av+=q;       wv+=q; y=(F(*av,*wv))&(((I)0x100<<(r<<3))-1); while(1){if(y)JNDB3; if(--i<0)break; y=F(*--av,*--wv);} }  \
  R sc(n);                                                                                    \
 }

#define SUMB(f,T0,T1,F)  \
 static F2(f){I an,*av,n,p,r1,wn,*wv,z=0;UI t,x;              \
  an=AN(a); av=AV(a);                                                        \
  wn=AN(w); wv=AV(w); n=1; n=AR(a)?an:n; n=AR(w)?wn:n;                      \
  p=n>>LGSZI; r1=n&(SZI-1);                                       \
  if     (!AR(a)){                                                           \
   ASSIGNX(av);                                                              \
   while((p-=255)>0){t=0; DQ(255, t+=F(x,    *wv++);); ADDBYTESINI(t); z+=t;}              \
         t=0; DQ(p+255,   t+=F(x,    *wv++);); ADDBYTESINI(t); z+=t; x=F(x,  *wv);  \
  }else if(!AR(w)){                                                          \
   ASSIGNX(wv);                                                              \
   while((p-=255)>0){t=0; DQ(255, t+=F(*av++,x    );); ADDBYTESINI(t); z+=t;}              \
         t=0; DQ(p+255,   t+=F(*av++,x    );); ADDBYTESINI(t); z+=t; x=F(*av,x  );  \
  }else{                                                                     \
   while((p-=255)>0){t=0; DQ(255, t+=F(*av++,*wv++);); ADDBYTESINI(t); z+=t;}              \
         t=0; DQ(p+255,   t+=F(*av++,*wv++);); ADDBYTESINI(t); z+=t; x=F(*av,*wv);  \
  }                                                                          \
  x &= ((I)1<<(r1<<LGBB))-1; ADDBYTESINI(x); z+=x;    /* C_LE */                                                       \
  R sc(z);                                                                   \
 }

INDB( i0eqBB,B,B,NE   )  INDF( i0eqBI,B,I,ANE  )  INDF0( i0eqBD,B,D,TNEXD,NEXD0)  /* =  */
INDF( i0eqIB,I,B,ANE  )  INDF( i0eqII,I,I,ANE  )  INDF0( i0eqID,I,D,TNEXD,NEXD0)
INDF0( i0eqDB,D,B,TNEDX,NEDX0)  INDF0( i0eqDI,D,I,TNEDX,NEDX0)  INDF0( i0eqDD,D,D,TNE,NE0  )
JNDB( j0eqBB,B,B,NE   )  JNDF( j0eqBI,B,I,ANE  )  JNDF0( j0eqBD,B,D,TNEXD,NEXD0)
JNDF( j0eqIB,I,B,ANE  )  JNDF( j0eqII,I,I,ANE  )  JNDF0( j0eqID,I,D,TNEXD,NEXD0)
JNDF0( j0eqDB,D,B,TNEDX,NEDX0)  JNDF0( j0eqDI,D,I,TNEDX,NEDX0)  JNDF0( j0eqDD,D,D,TNE,NE0  )

SUMB(sumeqBB,B,B,EQ   )  SUMF(sumeqBI,B,I,AEQ  )  SUMF0(sumeqBD,B,D,TEQXD,EQXD0)
SUMF(sumeqIB,I,B,AEQ  )  SUMF(sumeqII,I,I,AEQ  )  SUMF0(sumeqID,I,D,TEQXD,EQXD0)
SUMF0(sumeqDB,D,B,TEQDX,EQDX0)  SUMF0(sumeqDI,D,I,TEQDX,EQDX0)  SUMF0(sumeqDD,D,D,TEQ,EQ0  )

INDB( i0neBB,B,B,EQ   )  INDF( i0neBI,B,I,AEQ  )  INDF0( i0neBD,B,D,TEQXD,EQXD0)  /* ~: */
INDF( i0neIB,I,B,AEQ  )  INDF( i0neII,I,I,AEQ  )  INDF0( i0neID,I,D,TEQXD,EQXD0)
INDF0( i0neDB,D,B,TEQDX,EQDX0)  INDF0( i0neDI,D,I,TEQDX,EQDX0)  INDF0( i0neDD,D,D,TEQ,EQ0  )

JNDB( j0neBB,B,B,EQ   )  JNDF( j0neBI,B,I,AEQ  )  JNDF0( j0neBD,B,D,TEQXD,EQXD0)
JNDF( j0neIB,I,B,AEQ  )  JNDF( j0neII,I,I,AEQ  )  JNDF0( j0neID,I,D,TEQXD,EQXD0)
JNDF0( j0neDB,D,B,TEQDX,EQDX0)  JNDF0( j0neDI,D,I,TEQDX,EQDX0)  JNDF0( j0neDD,D,D,TEQ,EQ0  )

SUMB(sumneBB,B,B,NE   )  SUMF(sumneBI,B,I,ANE  )  SUMF0(sumneBD,B,D,TNEXD,NEXD0)
SUMF(sumneIB,I,B,ANE  )  SUMF(sumneII,I,I,ANE  )  SUMF0(sumneID,I,D,TNEXD,NEXD0)
SUMF0(sumneDB,D,B,TNEDX,NEDX0)  SUMF0(sumneDI,D,I,TNEDX,NEDX0)  SUMF0(sumneDD,D,D,TNE,NE0  )

INDB( i0ltBB,B,B,GE   )  INDF( i0ltBI,B,I,AGE  )  INDF0( i0ltBD,B,D,TGEXD,GEXD0)  /* <  */
INDF( i0ltIB,I,B,AGE  )  INDF( i0ltII,I,I,AGE  )  INDF0( i0ltID,I,D,TGEXD,GEXD0)
INDF0( i0ltDB,D,B,TGEDX,GEDX0)  INDF0( i0ltDI,D,I,TGEDX,GEDX0)  INDF0( i0ltDD,D,D,TGE,GE0  )

JNDB( j0ltBB,B,B,GE   )  JNDF( j0ltBI,B,I,AGE  )  JNDF0( j0ltBD,B,D,TGEXD,GEXD0)
JNDF( j0ltIB,I,B,AGE  )  JNDF( j0ltII,I,I,AGE  )  JNDF0( j0ltID,I,D,TGEXD,GEXD0)
JNDF0( j0ltDB,D,B,TGEDX,GEDX0)  JNDF0( j0ltDI,D,I,TGEDX,GEDX0)  JNDF0( j0ltDD,D,D,TGE,GE0  )

SUMB(sumltBB,B,B,LT   )  SUMF(sumltBI,B,I,ALT  )  SUMF0(sumltBD,B,D,TLTXD,LTXD0)
SUMF(sumltIB,I,B,ALT  )  SUMF(sumltII,I,I,ALT  )  SUMF0(sumltID,I,D,TLTXD,LTXD0)
SUMF0(sumltDB,D,B,TLTDX,LTDX0)  SUMF0(sumltDI,D,I,TLTDX,LTDX0)  SUMF0(sumltDD,D,D,TLT,LT0  )

INDB( i0leBB,B,B,GT   )  INDF( i0leBI,B,I,AGT  )  INDF0( i0leBD,B,D,TGTXD,GTXD0)  /* <: */
INDF( i0leIB,I,B,AGT  )  INDF( i0leII,I,I,AGT  )  INDF0( i0leID,I,D,TGTXD,GTXD0)
INDF0( i0leDB,D,B,TGTDX,GTDX0)  INDF0( i0leDI,D,I,TGTDX,GTDX0)  INDF0( i0leDD,D,D,TGT,GT0  )

JNDB( j0leBB,B,B,GT   )  JNDF( j0leBI,B,I,AGT  )  JNDF0( j0leBD,B,D,TGTXD,GTXD0)
JNDF( j0leIB,I,B,AGT  )  JNDF( j0leII,I,I,AGT  )  JNDF0( j0leID,I,D,TGTXD,GTXD0)
JNDF0( j0leDB,D,B,TGTDX,GTDX0)  JNDF0( j0leDI,D,I,TGTDX,GTDX0)  JNDF0( j0leDD,D,D,TGT,GT0  )

SUMB(sumleBB,B,B,LE   )  SUMF(sumleBI,B,I,ALE  )  SUMF0(sumleBD,B,D,TLEXD,LEXD0)
SUMF(sumleIB,I,B,ALE  )  SUMF(sumleII,I,I,ALE  )  SUMF0(sumleID,I,D,TLEXD,LEXD0)
SUMF0(sumleDB,D,B,TLEDX,LEDX0)  SUMF0(sumleDI,D,I,TLEDX,LEDX0)  SUMF0(sumleDD,D,D,TLE,LE0  )

INDB( i0geBB,B,B,LT   )  INDF( i0geBI,B,I,ALT  )  INDF0( i0geBD,B,D,TLTXD,LTXD0)  /* >: */
INDF( i0geIB,I,B,ALT  )  INDF( i0geII,I,I,ALT  )  INDF0( i0geID,I,D,TLTXD,LTXD0)
INDF0( i0geDB,D,B,TLTDX,LTDX0)  INDF0( i0geDI,D,I,TLTDX,LTDX0)  INDF0( i0geDD,D,D,TLT ,LT0 )

JNDB( j0geBB,B,B,LT   )  JNDF( j0geBI,B,I,ALT  )  JNDF0( j0geBD,B,D,TLTXD,LTXD0)
JNDF( j0geIB,I,B,ALT  )  JNDF( j0geII,I,I,ALT  )  JNDF0( j0geID,I,D,TLTXD,LTXD0)
JNDF0( j0geDB,D,B,TLTDX,LTDX0)  JNDF0( j0geDI,D,I,TLTDX,LTDX0)  JNDF0( j0geDD,D,D,TLT,LT0  )

SUMB(sumgeBB,B,B,GE   )  SUMF(sumgeBI,B,I,AGE  )  SUMF0(sumgeBD,B,D,TGEXD,GEXD0)
SUMF(sumgeIB,I,B,AGE  )  SUMF(sumgeII,I,I,AGE  )  SUMF0(sumgeID,I,D,TGEXD,GEXD0)
SUMF0(sumgeDB,D,B,TGEDX,GEDX0)  SUMF0(sumgeDI,D,I,TGEDX,GEDX0)  SUMF0(sumgeDD,D,D,TGE,GE0  )

INDB( i0gtBB,B,B,LE   )  INDF( i0gtBI,B,I,ALE  )  INDF0( i0gtBD,B,D,TLEXD,LEXD0)  /* >  */
INDF( i0gtIB,I,B,ALE  )  INDF( i0gtII,I,I,ALE  )  INDF0( i0gtID,I,D,TLEXD,LEXD0)
INDF0( i0gtDB,D,B,TLEDX,LEDX0)  INDF0( i0gtDI,D,I,TLEDX,LEDX0)  INDF0( i0gtDD,D,D,TLE,LE0  )

JNDB( j0gtBB,B,B,LE   )  JNDF( j0gtBI,B,I,ALE  )  JNDF0( j0gtBD,B,D,TLEXD,LEXD0)
JNDF( j0gtIB,I,B,ALE  )  JNDF( j0gtII,I,I,ALE  )  JNDF0( j0gtID,I,D,TLEXD,LEXD0)
JNDF0( j0gtDB,D,B,TLEDX,LEDX0)  JNDF0( j0gtDI,D,I,TLEDX,LEDX0)  JNDF0( j0gtDD,D,D,TLE,LE0  )

SUMB(sumgtBB,B,B,GT   )  SUMF(sumgtBI,B,I,AGT  )  SUMF0(sumgtBD,B,D,TGTXD,GTXD0)
SUMF(sumgtIB,I,B,AGT  )  SUMF(sumgtII,I,I,AGT  )  SUMF0(sumgtID,I,D,TGTXD,GTXD0)
SUMF0(sumgtDB,D,B,TGTDX,GTDX0)  SUMF0(sumgtDI,D,I,TGTDX,GTDX0)  SUMF0(sumgtDD,D,D,TGT,GT0  )


static AF atcompxy[]={  /* table for (B01,INT,FL) vs. (B01,INT,FL) */
 i0eqBB, i0eqBI, i0eqBD,   i0eqIB, i0eqII, i0eqID,   i0eqDB, i0eqDI, i0eqDD,   /* 0 */
 i0neBB, i0neBI, i0neBD,   i0neIB, i0neII, i0neID,   i0neDB, i0neDI, i0neDD,
 i0ltBB, i0ltBI, i0ltBD,   i0ltIB, i0ltII, i0ltID,   i0ltDB, i0ltDI, i0ltDD,
 i0leBB, i0leBI, i0leBD,   i0leIB, i0leII, i0leID,   i0leDB, i0leDI, i0leDD,
 i0geBB, i0geBI, i0geBD,   i0geIB, i0geII, i0geID,   i0geDB, i0geDI, i0geDD,
 i0gtBB, i0gtBI, i0gtBD,   i0gtIB, i0gtII, i0gtID,   i0gtDB, i0gtDI, i0gtDD,
 j0eqBB, j0eqBI, j0eqBD,   j0eqIB, j0eqII, j0eqID,   j0eqDB, j0eqDI, j0eqDD,   /* 2 */
 j0neBB, j0neBI, j0neBD,   j0neIB, j0neII, j0neID,   j0neDB, j0neDI, j0neDD,
 j0ltBB, j0ltBI, j0ltBD,   j0ltIB, j0ltII, j0ltID,   j0ltDB, j0ltDI, j0ltDD,
 j0leBB, j0leBI, j0leBD,   j0leIB, j0leII, j0leID,   j0leDB, j0leDI, j0leDD,
 j0geBB, j0geBI, j0geBD,   j0geIB, j0geII, j0geID,   j0geDB, j0geDI, j0geDD,
 j0gtBB, j0gtBI, j0gtBD,   j0gtIB, j0gtII, j0gtID,   j0gtDB, j0gtDI, j0gtDD,
 sumeqBB,sumeqBI,sumeqBD,  sumeqIB,sumeqII,sumeqID,  sumeqDB,sumeqDI,sumeqDD,  /* 4 */
 sumneBB,sumneBI,sumneBD,  sumneIB,sumneII,sumneID,  sumneDB,sumneDI,sumneDD,
 sumltBB,sumltBI,sumltBD,  sumltIB,sumltII,sumltID,  sumltDB,sumltDI,sumltDD,
 sumleBB,sumleBI,sumleBD,  sumleIB,sumleII,sumleID,  sumleDB,sumleDI,sumleDD,
 sumgeBB,sumgeBI,sumgeBD,  sumgeIB,sumgeII,sumgeID,  sumgeDB,sumgeDI,sumgeDD,
 sumgtBB,sumgtBI,sumgtBD,  sumgtIB,sumgtII,sumgtID,  sumgtDB,sumgtDI,sumgtDD,
};

INDF( i0eqC,C,C,ANE)  INDF( i0neC,C,C,AEQ)
JNDF( j0eqC,C,C,ANE)  JNDF( j0neC,C,C,AEQ)
SUMF(sumeqC,C,C,AEQ)  SUMF(sumneC,C,C,ANE)

static AF atcompC[]={   /* table for LIT vs. LIT */
  i0eqC,  i0neC, 0L,0L,0L,0L,
  j0eqC,  j0neC, 0L,0L,0L,0L,
 sumeqC, sumneC, 0L,0L,0L,0L,
};

INDF( i0eqUS,US,US,ANE)  INDF( i0neUS,US,US,AEQ)
JNDF( j0eqUS,US,US,ANE)  JNDF( j0neUS,US,US,AEQ)
SUMF(sumeqUS,US,US,AEQ)  SUMF(sumneUS,US,US,ANE)

static AF atcompUS[]={   /* table for C2T vs. C2T */
  i0eqUS,  i0neUS, 0L,0L,0L,0L,
  j0eqUS,  j0neUS, 0L,0L,0L,0L,
 sumeqUS, sumneUS, 0L,0L,0L,0L,
};

INDF( i0eqC4,C4,C4,ANE)  INDF( i0neC4,C4,C4,AEQ)
JNDF( j0eqC4,C4,C4,ANE)  JNDF( j0neC4,C4,C4,AEQ)
SUMF(sumeqC4,C4,C4,AEQ)  SUMF(sumneC4,C4,C4,ANE)

static AF atcompC4[]={   /* table for C4T vs. C4T */
  i0eqC4,  i0neC4, 0L,0L,0L,0L,
  j0eqC4,  j0neC4, 0L,0L,0L,0L,
 sumeqC4, sumneC4, 0L,0L,0L,0L,
};

INDF( i0eqS,SB,SB,ANE) INDF( i0neS,SB,SB,AEQ) 
JNDF( j0eqS,SB,SB,ANE) JNDF( j0neS,SB,SB,AEQ) 
SUMF(sumeqS,SB,SB,AEQ) SUMF(sumneS,SB,SB,ANE)

INDF( i0ltS,SB,SB,SBGE) INDF( i0leS,SB,SB,SBGT) INDF( i0geS,SB,SB,SBLT) INDF( i0gtS,SB,SB,SBLE)
JNDF( j0ltS,SB,SB,SBGE) JNDF( j0leS,SB,SB,SBGT) JNDF( j0geS,SB,SB,SBLT) JNDF( j0gtS,SB,SB,SBLE)
SUMF(sumltS,SB,SB,SBLT) SUMF(sumleS,SB,SB,SBLE) SUMF(sumgeS,SB,SB,SBGE) SUMF(sumgtS,SB,SB,SBGT)


static AF atcompSB[]={  /* table for SBT vs. SBT */
  i0eqS, i0neS, i0ltS, i0leS, i0geS, i0gtS,
  j0eqS, j0neS, j0ltS, j0leS, j0geS, j0gtS,
 sumeqS,sumneS,sumltS,sumleS,sumgeS,sumgtS,
};

// the special case for compounds like +/@e.
static F2( jti0eps){R indexofsub( II0EPS,w,a);}
static F2( jti1eps){R indexofsub( II1EPS,w,a);}
static F2( jtj0eps){R indexofsub( IJ0EPS,w,a);}
static F2( jtj1eps){R indexofsub( IJ1EPS,w,a);}
static F2(jtsumeps){R indexofsub(ISUMEPS,w,a);}
static F2(jtanyeps){R indexofsub(IANYEPS,w,a);}
static F2(jtalleps){R indexofsub(IALLEPS,w,a);}

// This table is indexed by m[5 4 3 0]
static AF atcompX[]={   /* table for any vs. any */
          0L,  jti0eps,
    jti1ebar,  jti1eps,
          0L,  jtj0eps,
          0L,  jtj1eps,
   jtsumebar, jtsumeps,
   jtanyebar, jtanyeps,
          0L, jtalleps,
};


// f (the comparison op) is bits 0-2 of m, the combining op is 3-5 of m
/*            f  0   1   2   3   4   5   6   7  */
/* m             =   ~:  <   <:  >:  >   E.  e. */
/* 0 f i. 0:     0   1   2   3   4   5   6   7  */
/* 1 f i. 1:     8   9  10  11  12  13  14  15  */
/* 2 f i: 0:    16  17  18  19  20  21  22  23  */
/* 3 f i: 1:    24  25  26  27  28  29  30  31  */
/* 4 [: + / f   32  33  34  35  36  37  38  39  */
/* 5 [: +./ f   40  41  42  43  44  45  46  47  */
/* 6 [: *./ f   48  49  50  51  52  53  54  55  */
// obsolete /* 7 [: I.  f   56  57  58  59  60  61  62  63  */

// Here to choose the function to use, after the arguments are known.
// We require the ranks to be <2 for processing here except for @e., which requires that the result of e. have rank<2
// If the form is [: f/ g  and g is a simple comparison, use f/@g code for higher ranks
// If no routine found, return 0 to failover to normal path
// compsc.postflags bits 0-1 indicate postprocessing needed: 00=none, 10=+./ (result is binary 0 if search completed), 11=*./ (result is binary 1 if search completed)
AF jtatcompf(J jt,A a,A w,A self){I m;
 RZ(a&&w);
 m=FAV(self)->flag&255;
 if((m&6)!=6){   // normal comparison
  // verify rank is OK, based on operation
  if((AR(a)|AR(w))>1){jt->workareas.compsc.postflags=0; R (m>=(4<<3))?(AF)jtfslashatg:0;}   // If an operand has rank>1, reject it unless it can be turned to f/@g special
  ASSERT(AN(a)==AN(w)||((AR(a)&AR(w))==0),EVLENGTH)   // agreement is same length or one an atom - we know ranks<=1
  // split m into search and comparison
  I search=m>>3; I comp=m&7;
  // Change +./ to i.&1, *./ to i.&0; save flag bits to include in return address
  jt->workareas.compsc.postflags=(0xc0>>search)&3; search=(0x0143210>>(search<<2))&15;  // flags: 00 00 00 00 00 10 11, rev/overlap to 11000000   search: 0 1 2 3 4 1 0
  // Change i.&1@:comp to i.&0@:compx, sim for i:  XOR comp with 000 001 000 110 000 110
  comp^=(0x606010>>(((search&1)+(comp&6))<<2))&7; search>>=1;  // complement comp if search is i&1; then the only search values are 0, 2, 4 so map them to 012.  Could reorder compares to = ~: < >: > <: to save code here
  if(!((AT(a)|AT(w))&(NOUN&~(INT+FL+B01)))){
   // numeric types that we can handle here, for sure
   R atcompxy[6*9*search+9*comp+3*(AT(a)>>INTX)+(AT(w)>>INTX)];
  }
  // Other types have a chance only if they are equal types; fetch from the appropriate table then
  if((AT(a)&AT(w)&(LIT+C2T+C4T+SBT))){R (AT(a)&LIT?atcompC:AT(a)&C2T?atcompUS:AT(a)&C4T?atcompC4:atcompSB)[6*search+comp];}
  R 0;
 }else{  // e./E.
  jt->workareas.compsc.postflags=0;
  if((AR(a)|AR(w))>1){if(!(m&1)||AR(a)>(AR(w)?AR(w):1))R0;}  // some rank > 1, fail if E. or e. returns rank>1
  R atcompX[((m>>2)&~1)+(m&1)];  // choose i.-family routine
 }
}    /* function table look-up for  comp i. 1:  and  i.&1@:comp  etc. */

