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
/*    [: I. comp          I.@:comp                                         */
/* where comp is one of the following:                                     */
/*    = ~: < <: >: > E. e.                                                 */

#include "j.h"
#include "ve.h"
#include "vcomp.h"


#define INDF(f,T0,T1,F)  \
 static F2(f){I an,n,wn;T0*av,x;T1*wv,y;                                      \
  an=AN(a); av=(T0*)AV(a);                                                    \
  wn=AN(w); wv=(T1*)AV(w); n=AR(a)&&AR(w)?MAX(an,wn):AR(a)?an:wn;             \
  if     (!AR(a)){x=*av;        DO(n,          y=*wv++; if(F(x,y))R sc(i););} \
  else if(!AR(w)){y=*wv;        DO(n, x=*av++;          if(F(x,y))R sc(i););} \
  else           {              DO(n, x=*av++; y=*wv++; if(F(x,y))R sc(i););} \
  R sc(n);                                                                    \
 }

#define JNDF(f,T0,T1,F)  \
 static F2(f){I an,n,wn;T0*av,x;T1*wv,y;                                          \
  an=AN(a); av=(T0*)AV(a);                                                        \
  wn=AN(w); wv=(T1*)AV(w); n=AR(a)&&AR(w)?MAX(an,wn):AR(a)?an:wn;                 \
  if     (!AR(a)){x=*av; wv+=n; DO(n,          y=*--wv; if(F(x,y))R sc(n-1-i););} \
  else if(!AR(w)){y=*wv; av+=n; DO(n, x=*--av;          if(F(x,y))R sc(n-1-i););} \
  else           {av+=n; wv+=n; DO(n, x=*--av; y=*--wv; if(F(x,y))R sc(n-1-i););} \
  R sc(n);                                                                        \
 }

#define SUMF(f,T0,T1,F)  \
 static F2(f){I an,m=0,n,wn;T0*av,x;T1*wv,y;                       \
  an=AN(a); av=(T0*)AV(a);                                         \
  wn=AN(w); wv=(T1*)AV(w); n=AR(a)&&AR(w)?MAX(an,wn):AR(a)?an:wn;  \
  if     (!AR(a)){x=*av; DO(n,          y=*wv++; if(F(x,y))++m;);} \
  else if(!AR(w)){y=*wv; DO(n, x=*av++;          if(F(x,y))++m;);} \
  else           {       DO(n, x=*av++; y=*wv++; if(F(x,y))++m;);} \
  R sc(m);                                                         \
 }

#define ANYF(f,T0,T1,F)  \
 static F2(f){I an,n,wn;T0*av,x;T1*wv,y;                             \
  an=AN(a); av=(T0*)AV(a);                                           \
  wn=AN(w); wv=(T1*)AV(w); n=AR(a)&&AR(w)?MAX(an,wn):AR(a)?an:wn;    \
  if     (!AR(a)){x=*av; DO(n,          y=*wv++; if(F(x,y))R one;);} \
  else if(!AR(w)){y=*wv; DO(n, x=*av++;          if(F(x,y))R one;);} \
  else           {       DO(n, x=*av++; y=*wv++; if(F(x,y))R one;);} \
  R zero;                                                            \
 }

#define ALLF(f,T0,T1,F)  \
 static F2(f){I an,n,wn;T0*av,x;T1*wv,y;                               \
  an=AN(a); av=(T0*)AV(a);                                             \
  wn=AN(w); wv=(T1*)AV(w); n=AR(a)&&AR(w)?MAX(an,wn):AR(a)?an:wn;      \
  if     (!AR(a)){x=*av; DO(n,          y=*wv++; if(!F(x,y))R zero;);} \
  else if(!AR(w)){y=*wv; DO(n, x=*av++;          if(!F(x,y))R zero;);} \
  else           {       DO(n, x=*av++; y=*wv++; if(!F(x,y))R zero;);} \
  R one;                                                               \
 }

#define IFB1  \
  {if(zu==zv){I m=zv-AV(z); RZ(z=ext(0,z)); zv=m+AV(z); zu=AN(z)+AV(z);} *zv++=i;}

#define IFBF(f,T0,T1,F)  \
 static F2(f){A z;I an,n,wn,*zu,*zv;T0*av,x;T1*wv,y;                \
  an=AN(a); av=(T0*)AV(a);                                          \
  wn=AN(w); wv=(T1*)AV(w); n=AR(a)&&AR(w)?MAX(an,wn):AR(a)?an:wn;   \
  RZ(z=exta(INT,1L,1L,MAX(22,n/8))); zv=AV(z); zu=zv+AN(z);         \
  if     (!AR(a)){x=*av; DO(n,          y=*wv++; if(F(x,y))IFB1;);} \
  else if(!AR(w)){y=*wv; DO(n, x=*av++;          if(F(x,y))IFB1;);} \
  else           {       DO(n, x=*av++; y=*wv++; if(F(x,y))IFB1;);} \
  AN(z)=*AS(z)=zv-AV(z); R z;                                       \
 }


/* Now define byte-parallel (4 bytes at a time) versions of above  */

#define JNDBR(yy)      if(r&&(y=yy))DO(r, if(yv[r-1-i])R sc(n-1-i););

#if SY_64
#define ASSIGNX(v)     {x=*v; xv[1]=xv[2]=xv[3]=xv[4]=xv[5]=xv[6]=xv[7]=xv[0];}
#define INDB3          R sc(       i*SZI+(yv[0]?0:yv[1]?1:yv[2]?2:yv[3]?3:yv[4]?4:yv[5]?5:yv[6]?6:7) );
#define JNDB3          R sc(n-1-(r+i*SZI+(yv[7]?0:yv[6]?1:yv[5]?2:yv[4]?3:yv[3]?4:yv[2]?5:yv[1]?6:7)));
#else
#define ASSIGNX(v)     {x=*v; xv[1]=xv[2]=xv[3]=xv[0];}
#define INDB3          R sc(       i*SZI+(yv[0]?0:yv[1]?1:yv[2]?2:3) );
#define JNDB3          R sc(n-1-(r+i*SZI+(yv[3]?0:yv[2]?1:yv[1]?2:3)));
#endif

#define INDB(f,T0,T1,F)  \
 static F2(f){B*xv,*yv;I an,*av,n,q,r,wn,*wv,x,y;                                 \
  an=AN(a); av=AV(a);                                                             \
  wn=AN(w); wv=AV(w); n=AR(a)&&AR(w)?MAX(an,wn):AR(a)?an:wn;                      \
  q=n/SZI; r=n%SZI;                                                               \
  xv=(B*)&x; yv=(B*)&y;                                                           \
  if     (!AR(a)){ASSIGNX(av); DO(q, if(y=F(x,    *wv++))INDB3;); y=F(x,  *wv);}  \
  else if(!AR(w)){ASSIGNX(wv); DO(q, if(y=F(*av++,x    ))INDB3;); y=F(*av,x  );}  \
  else           {             DO(q, if(y=F(*av++,*wv++))INDB3;); y=F(*av,*wv);}  \
  if(y)DO(r, if(yv[i])R sc(i+q*SZI););                                            \
  R sc(n);                                                                        \
 }

#define JNDB(f,T0,T1,F)  \
 static F2(f){B*xv,*yv;I an,*av,n,q,r,wn,*wv,x,y;                                             \
  an=AN(a); av=AV(a);                                                                         \
  wn=AN(w); wv=AV(w); n=AR(a)&&AR(w)?MAX(an,wn):AR(a)?an:wn;                                  \
  q=n/SZI; r=n%SZI;                                                                           \
  xv=(B*)&x; yv=(B*)&y;                                                                       \
  if     (!AR(a)){ASSIGNX(av); wv+=q; JNDBR(F(x,  *wv)); DO(q, if(y=F(x,    *--wv))JNDB3;);}  \
  else if(!AR(w)){ASSIGNX(wv); av+=q; JNDBR(F(*av,x  )); DO(q, if(y=F(*--av,x    ))JNDB3;);}  \
  else           {av+=q;       wv+=q; JNDBR(F(*av,*wv)); DO(q, if(y=F(*--av,*--wv))JNDB3;);}  \
  R sc(n);                                                                                    \
 }

#define SUMB(f,T0,T1,F)  \
 static F2(f){B*xv;I an,*av,n,p,q,r,r1,wn,*wv,x,z=0;UC*tu;UI t;              \
  an=AN(a); av=AV(a);                                                        \
  wn=AN(w); wv=AV(w); n=AR(a)&&AR(w)?MAX(an,wn):AR(a)?an:wn;                 \
  p=n/SZI; q=p/255; r=p%255; r1=n%SZI;                                       \
  xv=(B*)&x; tu=(UC*)&t;                                                     \
  if     (!AR(a)){                                                           \
   ASSIGNX(av);                                                              \
   DO(q, t=0; DO(255, t+=F(x,    *wv++);); DO(SZI,z+=tu[i];););              \
         t=0; DO(r,   t+=F(x,    *wv++);); DO(SZI,z+=tu[i];); x=F(x,  *wv);  \
  }else if(!AR(w)){                                                          \
   ASSIGNX(wv);                                                              \
   DO(q, t=0; DO(255, t+=F(*av++,x    );); DO(SZI,z+=tu[i];););              \
         t=0; DO(r,   t+=F(*av++,x    );); DO(SZI,z+=tu[i];); x=F(*av,x  );  \
  }else{                                                                     \
   DO(q, t=0; DO(255, t+=F(*av++,*wv++);); DO(SZI,z+=tu[i];););              \
         t=0; DO(r,   t+=F(*av++,*wv++);); DO(SZI,z+=tu[i];); x=F(*av,*wv);  \
  }                                                                          \
  DO(r1, z+=xv[i];);                                                         \
  R sc(z);                                                                   \
 }

#define ANYB(f,T0,T1,F)  \
 static F2(f){B*xv;     I an,*av,n,p,r1,  wn,*wv,x;                                 \
  an=AN(a); av=AV(a);                                                               \
  wn=AN(w); wv=AV(w); n=AR(a)&&AR(w)?MAX(an,wn):AR(a)?an:wn;                        \
  p=n/SZI; r1=n%SZI;;                                                               \
  xv=(B*)&x;                                                                        \
  if     (!AR(a)){ASSIGNX(av); DO(p, if(   F(x,    *wv++))R one; ); x=F(x,  *wv);}  \
  else if(!AR(w)){ASSIGNX(wv); DO(p, if(   F(*av++,x    ))R one; ); x=F(*av,x  );}  \
  else           {             DO(p, if(   F(*av++,*wv++))R one; ); x=F(*av,*wv);}  \
  DO(r1, if(xv[i])R one;);                                                          \
  R zero;                                                                           \
 }

#define ALLB(f,T0,T1,F)  \
 static F2(f){B*xv;C*tv;I an,*av,n,p,r1,t,wn,*wv,x;                                 \
  an=AN(a); av=AV(a);                                                               \
  wn=AN(w); wv=AV(w); n=AR(a)&&AR(w)?MAX(an,wn):AR(a)?an:wn;                        \
  p=n/SZI; r1=n%SZI;;                                                               \
  xv=(B*)&x; tv=(C*)&t; DO(SZI, tv[i]=1;);                                          \
  if     (!AR(a)){ASSIGNX(av); DO(p, if(t!=F(x,    *wv++))R zero;); x=F(x,  *wv);}  \
  else if(!AR(w)){ASSIGNX(wv); DO(p, if(t!=F(*av++,x    ))R zero;); x=F(*av,x  );}  \
  else           {             DO(p, if(t!=F(*av++,*wv++))R zero;); x=F(*av,*wv);}  \
  DO(r1, if(!xv[i])R zero;);                                                        \
  R one;                                                                            \
 }

#if SY_64
#define IFB3           \
  {if(zu<zv){I c=zv-AV(z); RZ(z=ext(0,z)); zv=c+AV(z); zu=AV(z)+AN(z)-SZI;}  \
   if(yv[0])*zv++=  m;      \
   if(yv[1])*zv++=1+m;      \
   if(yv[2])*zv++=2+m;      \
   if(yv[3])*zv++=3+m;      \
   if(yv[4])*zv++=4+m;      \
   if(yv[5])*zv++=5+m;      \
   if(yv[6])*zv++=6+m;      \
   if(yv[7])*zv++=7+m;      \
  }
#else
#define IFB3           \
  {if(zu<zv){I c=zv-AV(z); RZ(z=ext(0,z)); zv=c+AV(z); zu=AV(z)+AN(z)-SZI;}  \
   switch(y){                                                                \
    case B0001:                                *zv++=3+m; break;             \
    case B0010:                     *zv++=2+m;            break;             \
    case B0011:                     *zv++=2+m; *zv++=3+m; break;             \
    case B0100:          *zv++=1+m;                       break;             \
    case B0101:          *zv++=1+m;            *zv++=3+m; break;             \
    case B0110:          *zv++=1+m; *zv++=2+m;            break;             \
    case B0111:          *zv++=1+m; *zv++=2+m; *zv++=3+m; break;             \
    case B1000: *zv++=m;                                  break;             \
    case B1001: *zv++=m;                       *zv++=3+m; break;             \
    case B1010: *zv++=m;            *zv++=2+m;            break;             \
    case B1011: *zv++=m;            *zv++=2+m; *zv++=3+m; break;             \
    case B1100: *zv++=m; *zv++=1+m;                       break;             \
    case B1101: *zv++=m; *zv++=1+m;            *zv++=3+m; break;             \
    case B1110: *zv++=m; *zv++=1+m; *zv++=2+m;            break;             \
    case B1111: *zv++=m; *zv++=1+m; *zv++=2+m; *zv++=3+m; break;             \
  }}
#endif

#define IFBB(f,T0,T1,F)  \
 static F2(f){A z;B*xv,*yv;I an,*av,m=0,n,q,r,wn,*wv,x,y,*zu,*zv;                        \
  an=AN(a); av=AV(a);                                                                    \
  wn=AN(w); wv=AV(w); n=AR(a)&&AR(w)?MAX(an,wn):AR(a)?an:wn;                             \
  q=n/SZI; r=n%SZI;                                                                      \
  xv=(B*)&x; yv=(B*)&y;                                                                  \
  RZ(z=exta(INT,1L,1L,MAX(22,n/SZI))); zv=AV(z); zu=zv+AN(z)-SZI;                        \
  if     (!AR(a)){ASSIGNX(av); DO(q, if(y=F(x,    *wv++))IFB3; m+=SZI;); y=F(x,  *wv);}  \
  else if(!AR(w)){ASSIGNX(wv); DO(q, if(y=F(*av++,x    ))IFB3; m+=SZI;); y=F(*av,x  );}  \
  else           {             DO(q, if(y=F(*av++,*wv++))IFB3; m+=SZI;); y=F(*av,*wv);}  \
  if(r&&y){DO(SZI-r, yv[r+i]=0;); IFB3;};                                                \
  AN(z)=*AS(z)=zv-AV(z); R z;                                                            \
 }

INDB( i0eqBB,B,B,NE   )  INDF( i0eqBI,B,I,ANE  )  INDF( i0eqBD,B,D,TNEXD)  /* =  */
INDF( i0eqIB,I,B,ANE  )  INDF( i0eqII,I,I,ANE  )  INDF( i0eqID,I,D,TNEXD)
INDF( i0eqDB,D,B,TNEDX)  INDF( i0eqDI,D,I,TNEDX)  INDF( i0eqDD,D,D,TNE  )

INDB( i1eqBB,B,B,EQ   )  INDF( i1eqBI,B,I,AEQ  )  INDF( i1eqBD,B,D,TEQXD)
INDF( i1eqIB,I,B,AEQ  )  INDF( i1eqII,I,I,AEQ  )  INDF( i1eqID,I,D,TEQXD)
INDF( i1eqDB,D,B,TEQDX)  INDF( i1eqDI,D,I,TEQDX)  INDF( i1eqDD,D,D,TEQ  )

JNDB( j0eqBB,B,B,NE   )  JNDF( j0eqBI,B,I,ANE  )  JNDF( j0eqBD,B,D,TNEXD)
JNDF( j0eqIB,I,B,ANE  )  JNDF( j0eqII,I,I,ANE  )  JNDF( j0eqID,I,D,TNEXD)
JNDF( j0eqDB,D,B,TNEDX)  JNDF( j0eqDI,D,I,TNEDX)  JNDF( j0eqDD,D,D,TNE  )

JNDB( j1eqBB,B,B,EQ   )  JNDF( j1eqBI,B,I,AEQ  )  JNDF( j1eqBD,B,D,TEQXD)
JNDF( j1eqIB,I,B,AEQ  )  JNDF( j1eqII,I,I,AEQ  )  JNDF( j1eqID,I,D,TEQXD)
JNDF( j1eqDB,D,B,TEQDX)  JNDF( j1eqDI,D,I,TEQDX)  JNDF( j1eqDD,D,D,TEQ  )

SUMB(sumeqBB,B,B,EQ   )  SUMF(sumeqBI,B,I,AEQ  )  SUMF(sumeqBD,B,D,TEQXD)
SUMF(sumeqIB,I,B,AEQ  )  SUMF(sumeqII,I,I,AEQ  )  SUMF(sumeqID,I,D,TEQXD)
SUMF(sumeqDB,D,B,TEQDX)  SUMF(sumeqDI,D,I,TEQDX)  SUMF(sumeqDD,D,D,TEQ  )

ANYB(anyeqBB,B,B,EQ   )  ANYF(anyeqBI,B,I,AEQ  )  ANYF(anyeqBD,B,D,TEQXD)
ANYF(anyeqIB,I,B,AEQ  )  ANYF(anyeqII,I,I,AEQ  )  ANYF(anyeqID,I,D,TEQXD)
ANYF(anyeqDB,D,B,TEQDX)  ANYF(anyeqDI,D,I,TEQDX)  ANYF(anyeqDD,D,D,TEQ  )

ALLB(alleqBB,B,B,EQ   )  ALLF(alleqBI,B,I,AEQ  )  ALLF(alleqBD,B,D,TEQXD)
ALLF(alleqIB,I,B,AEQ  )  ALLF(alleqII,I,I,AEQ  )  ALLF(alleqID,I,D,TEQXD)
ALLF(alleqDB,D,B,TEQDX)  ALLF(alleqDI,D,I,TEQDX)  ALLF(alleqDD,D,D,TEQ  )

IFBB(ifbeqBB,B,B,EQ   )  IFBF(ifbeqBI,B,I,AEQ  )  IFBF(ifbeqBD,B,D,TEQXD)
IFBF(ifbeqIB,I,B,AEQ  )  IFBF(ifbeqII,I,I,AEQ  )  IFBF(ifbeqID,I,D,TEQXD)
IFBF(ifbeqDB,D,B,TEQDX)  IFBF(ifbeqDI,D,I,TEQDX)  IFBF(ifbeqDD,D,D,TEQ  )

INDB( i0neBB,B,B,EQ   )  INDF( i0neBI,B,I,AEQ  )  INDF( i0neBD,B,D,TEQXD)  /* ~: */
INDF( i0neIB,I,B,AEQ  )  INDF( i0neII,I,I,AEQ  )  INDF( i0neID,I,D,TEQXD)
INDF( i0neDB,D,B,TEQDX)  INDF( i0neDI,D,I,TEQDX)  INDF( i0neDD,D,D,TEQ  )

INDB( i1neBB,B,B,NE   )  INDF( i1neBI,B,I,ANE  )  INDF( i1neBD,B,D,TNEXD)
INDF( i1neIB,I,B,ANE  )  INDF( i1neII,I,I,ANE  )  INDF( i1neID,I,D,TNEXD)
INDF( i1neDB,D,B,TNEDX)  INDF( i1neDI,D,I,TNEDX)  INDF( i1neDD,D,D,TNE  )

JNDB( j0neBB,B,B,EQ   )  JNDF( j0neBI,B,I,AEQ  )  JNDF( j0neBD,B,D,TEQXD)
JNDF( j0neIB,I,B,AEQ  )  JNDF( j0neII,I,I,AEQ  )  JNDF( j0neID,I,D,TEQXD)
JNDF( j0neDB,D,B,TEQDX)  JNDF( j0neDI,D,I,TEQDX)  JNDF( j0neDD,D,D,TEQ  )

JNDB( j1neBB,B,B,NE   )  JNDF( j1neBI,B,I,ANE  )  JNDF( j1neBD,B,D,TNEXD)
JNDF( j1neIB,I,B,ANE  )  JNDF( j1neII,I,I,ANE  )  JNDF( j1neID,I,D,TNEXD)
JNDF( j1neDB,D,B,TNEDX)  JNDF( j1neDI,D,I,TNEDX)  JNDF( j1neDD,D,D,TNE  )

SUMB(sumneBB,B,B,NE   )  SUMF(sumneBI,B,I,ANE  )  SUMF(sumneBD,B,D,TNEXD)
SUMF(sumneIB,I,B,ANE  )  SUMF(sumneII,I,I,ANE  )  SUMF(sumneID,I,D,TNEXD)
SUMF(sumneDB,D,B,TNEDX)  SUMF(sumneDI,D,I,TNEDX)  SUMF(sumneDD,D,D,TNE  )

ANYB(anyneBB,B,B,NE   )  ANYF(anyneBI,B,I,ANE  )  ANYF(anyneBD,B,D,TNEXD)
ANYF(anyneIB,I,B,ANE  )  ANYF(anyneII,I,I,ANE  )  ANYF(anyneID,I,D,TNEXD)
ANYF(anyneDB,D,B,TNEDX)  ANYF(anyneDI,D,I,TNEDX)  ANYF(anyneDD,D,D,TNE  )

ALLB(allneBB,B,B,NE   )  ALLF(allneBI,B,I,ANE  )  ALLF(allneBD,B,D,TNEXD)
ALLF(allneIB,I,B,ANE  )  ALLF(allneII,I,I,ANE  )  ALLF(allneID,I,D,TNEXD)
ALLF(allneDB,D,B,TNEDX)  ALLF(allneDI,D,I,TNEDX)  ALLF(allneDD,D,D,TNE  )

IFBB(ifbneBB,B,B,NE   )  IFBF(ifbneBI,B,I,ANE  )  IFBF(ifbneBD,B,D,TNEXD)
IFBF(ifbneIB,I,B,ANE  )  IFBF(ifbneII,I,I,ANE  )  IFBF(ifbneID,I,D,TNEXD)
IFBF(ifbneDB,D,B,TNEDX)  IFBF(ifbneDI,D,I,TNEDX)  IFBF(ifbneDD,D,D,TNE  )

INDB( i0ltBB,B,B,GE   )  INDF( i0ltBI,B,I,AGE  )  INDF( i0ltBD,B,D,TGEXD)  /* <  */
INDF( i0ltIB,I,B,AGE  )  INDF( i0ltII,I,I,AGE  )  INDF( i0ltID,I,D,TGEXD)
INDF( i0ltDB,D,B,TGEDX)  INDF( i0ltDI,D,I,TGEDX)  INDF( i0ltDD,D,D,TGE  )

INDB( i1ltBB,B,B,LT   )  INDF( i1ltBI,B,I,ALT  )  INDF( i1ltBD,B,D,TLTXD)
INDF( i1ltIB,I,B,ALT  )  INDF( i1ltII,I,I,ALT  )  INDF( i1ltID,I,D,TLTXD)
INDF( i1ltDB,D,B,TLTDX)  INDF( i1ltDI,D,I,TLTDX)  INDF( i1ltDD,D,D,TLT  )

JNDB( j0ltBB,B,B,GE   )  JNDF( j0ltBI,B,I,AGE  )  JNDF( j0ltBD,B,D,TGEXD)
JNDF( j0ltIB,I,B,AGE  )  JNDF( j0ltII,I,I,AGE  )  JNDF( j0ltID,I,D,TGEXD)
JNDF( j0ltDB,D,B,TGEDX)  JNDF( j0ltDI,D,I,TGEDX)  JNDF( j0ltDD,D,D,TGE  )

JNDB( j1ltBB,B,B,LT   )  JNDF( j1ltBI,B,I,ALT  )  JNDF( j1ltBD,B,D,TLTXD)
JNDF( j1ltIB,I,B,ALT  )  JNDF( j1ltII,I,I,ALT  )  JNDF( j1ltID,I,D,TLTXD)
JNDF( j1ltDB,D,B,TLTDX)  JNDF( j1ltDI,D,I,TLTDX)  JNDF( j1ltDD,D,D,TLT  )

SUMB(sumltBB,B,B,LT   )  SUMF(sumltBI,B,I,ALT  )  SUMF(sumltBD,B,D,TLTXD)
SUMF(sumltIB,I,B,ALT  )  SUMF(sumltII,I,I,ALT  )  SUMF(sumltID,I,D,TLTXD)
SUMF(sumltDB,D,B,TLTDX)  SUMF(sumltDI,D,I,TLTDX)  SUMF(sumltDD,D,D,TLT  )

ANYB(anyltBB,B,B,LT   )  ANYF(anyltBI,B,I,ALT  )  ANYF(anyltBD,B,D,TLTXD)
ANYF(anyltIB,I,B,ALT  )  ANYF(anyltII,I,I,ALT  )  ANYF(anyltID,I,D,TLTXD)
ANYF(anyltDB,D,B,TLTDX)  ANYF(anyltDI,D,I,TLTDX)  ANYF(anyltDD,D,D,TLT  )

ALLB(allltBB,B,B,LT   )  ALLF(allltBI,B,I,ALT  )  ALLF(allltBD,B,D,TLTXD)
ALLF(allltIB,I,B,ALT  )  ALLF(allltII,I,I,ALT  )  ALLF(allltID,I,D,TLTXD)
ALLF(allltDB,D,B,TLTDX)  ALLF(allltDI,D,I,TLTDX)  ALLF(allltDD,D,D,TLT  )

IFBB(ifbltBB,B,B,LT   )  IFBF(ifbltBI,B,I,ALT  )  IFBF(ifbltBD,B,D,TLTXD)
IFBF(ifbltIB,I,B,ALT  )  IFBF(ifbltII,I,I,ALT  )  IFBF(ifbltID,I,D,TLTXD)
IFBF(ifbltDB,D,B,TLTDX)  IFBF(ifbltDI,D,I,TLTDX)  IFBF(ifbltDD,D,D,TLT  )

INDB( i0leBB,B,B,GT   )  INDF( i0leBI,B,I,AGT  )  INDF( i0leBD,B,D,TGTXD)  /* <: */
INDF( i0leIB,I,B,AGT  )  INDF( i0leII,I,I,AGT  )  INDF( i0leID,I,D,TGTXD)
INDF( i0leDB,D,B,TGTDX)  INDF( i0leDI,D,I,TGTDX)  INDF( i0leDD,D,D,TGT  )

INDB( i1leBB,B,B,LE   )  INDF( i1leBI,B,I,ALE  )  INDF( i1leBD,B,D,TLEXD)
INDF( i1leIB,I,B,ALE  )  INDF( i1leII,I,I,ALE  )  INDF( i1leID,I,D,TLEXD)
INDF( i1leDB,D,B,TLEDX)  INDF( i1leDI,D,I,TLEDX)  INDF( i1leDD,D,D,TLE  )

JNDB( j0leBB,B,B,GT   )  JNDF( j0leBI,B,I,AGT  )  JNDF( j0leBD,B,D,TGTXD)
JNDF( j0leIB,I,B,AGT  )  JNDF( j0leII,I,I,AGT  )  JNDF( j0leID,I,D,TGTXD)
JNDF( j0leDB,D,B,TGTDX)  JNDF( j0leDI,D,I,TGTDX)  JNDF( j0leDD,D,D,TGT  )

JNDB( j1leBB,B,B,LE   )  JNDF( j1leBI,B,I,ALE  )  JNDF( j1leBD,B,D,TLEXD)
JNDF( j1leIB,I,B,ALE  )  JNDF( j1leII,I,I,ALE  )  JNDF( j1leID,I,D,TLEXD)
JNDF( j1leDB,D,B,TLEDX)  JNDF( j1leDI,D,I,TLEDX)  JNDF( j1leDD,D,D,TLE  )

SUMB(sumleBB,B,B,LE   )  SUMF(sumleBI,B,I,ALE  )  SUMF(sumleBD,B,D,TLEXD)
SUMF(sumleIB,I,B,ALE  )  SUMF(sumleII,I,I,ALE  )  SUMF(sumleID,I,D,TLEXD)
SUMF(sumleDB,D,B,TLEDX)  SUMF(sumleDI,D,I,TLEDX)  SUMF(sumleDD,D,D,TLE  )

ANYB(anyleBB,B,B,LE   )  ANYF(anyleBI,B,I,ALE  )  ANYF(anyleBD,B,D,TLEXD)
ANYF(anyleIB,I,B,ALE  )  ANYF(anyleII,I,I,ALE  )  ANYF(anyleID,I,D,TLEXD)
ANYF(anyleDB,D,B,TLEDX)  ANYF(anyleDI,D,I,TLEDX)  ANYF(anyleDD,D,D,TLE  )

ALLB(allleBB,B,B,LE   )  ALLF(allleBI,B,I,ALE  )  ALLF(allleBD,B,D,TLEXD)
ALLF(allleIB,I,B,ALE  )  ALLF(allleII,I,I,ALE  )  ALLF(allleID,I,D,TLEXD)
ALLF(allleDB,D,B,TLEDX)  ALLF(allleDI,D,I,TLEDX)  ALLF(allleDD,D,D,TLE  )

IFBB(ifbleBB,B,B,LE   )  IFBF(ifbleBI,B,I,ALE  )  IFBF(ifbleBD,B,D,TLEXD)
IFBF(ifbleIB,I,B,ALE  )  IFBF(ifbleII,I,I,ALE  )  IFBF(ifbleID,I,D,TLEXD)
IFBF(ifbleDB,D,B,TLEDX)  IFBF(ifbleDI,D,I,TLEDX)  IFBF(ifbleDD,D,D,TLE  )

INDB( i0geBB,B,B,LT   )  INDF( i0geBI,B,I,ALT  )  INDF( i0geBD,B,D,TLTXD)  /* >: */
INDF( i0geIB,I,B,ALT  )  INDF( i0geII,I,I,ALT  )  INDF( i0geID,I,D,TLTXD)
INDF( i0geDB,D,B,TLTDX)  INDF( i0geDI,D,I,TLTDX)  INDF( i0geDD,D,D,TLT  )

INDB( i1geBB,B,B,GE   )  INDF( i1geBI,B,I,AGE  )  INDF( i1geBD,B,D,TGEXD)
INDF( i1geIB,I,B,AGE  )  INDF( i1geII,I,I,AGE  )  INDF( i1geID,I,D,TGEXD)
INDF( i1geDB,D,B,TGEDX)  INDF( i1geDI,D,I,TGEDX)  INDF( i1geDD,D,D,TGE  )

JNDB( j0geBB,B,B,LT   )  JNDF( j0geBI,B,I,ALT  )  JNDF( j0geBD,B,D,TLTXD)
JNDF( j0geIB,I,B,ALT  )  JNDF( j0geII,I,I,ALT  )  JNDF( j0geID,I,D,TLTXD)
JNDF( j0geDB,D,B,TLTDX)  JNDF( j0geDI,D,I,TLTDX)  JNDF( j0geDD,D,D,TLT  )

JNDB( j1geBB,B,B,GE   )  JNDF( j1geBI,B,I,AGE  )  JNDF( j1geBD,B,D,TGEXD)
JNDF( j1geIB,I,B,AGE  )  JNDF( j1geII,I,I,AGE  )  JNDF( j1geID,I,D,TGEXD)
JNDF( j1geDB,D,B,TGEDX)  JNDF( j1geDI,D,I,TGEDX)  JNDF( j1geDD,D,D,TGE  )

SUMB(sumgeBB,B,B,GE   )  SUMF(sumgeBI,B,I,AGE  )  SUMF(sumgeBD,B,D,TGEXD)
SUMF(sumgeIB,I,B,AGE  )  SUMF(sumgeII,I,I,AGE  )  SUMF(sumgeID,I,D,TGEXD)
SUMF(sumgeDB,D,B,TGEDX)  SUMF(sumgeDI,D,I,TGEDX)  SUMF(sumgeDD,D,D,TGE  )

ANYB(anygeBB,B,B,GE   )  ANYF(anygeBI,B,I,AGE  )  ANYF(anygeBD,B,D,TGEXD)
ANYF(anygeIB,I,B,AGE  )  ANYF(anygeII,I,I,AGE  )  ANYF(anygeID,I,D,TGEXD)
ANYF(anygeDB,D,B,TGEDX)  ANYF(anygeDI,D,I,TGEDX)  ANYF(anygeDD,D,D,TGE  )

ALLB(allgeBB,B,B,GE   )  ALLF(allgeBI,B,I,AGE  )  ALLF(allgeBD,B,D,TGEXD)
ALLF(allgeIB,I,B,AGE  )  ALLF(allgeII,I,I,AGE  )  ALLF(allgeID,I,D,TGEXD)
ALLF(allgeDB,D,B,TGEDX)  ALLF(allgeDI,D,I,TGEDX)  ALLF(allgeDD,D,D,TGE  )

IFBB(ifbgeBB,B,B,GE   )  IFBF(ifbgeBI,B,I,AGE  )  IFBF(ifbgeBD,B,D,TGEXD)
IFBF(ifbgeIB,I,B,AGE  )  IFBF(ifbgeII,I,I,AGE  )  IFBF(ifbgeID,I,D,TGEXD)
IFBF(ifbgeDB,D,B,TGEDX)  IFBF(ifbgeDI,D,I,TGEDX)  IFBF(ifbgeDD,D,D,TGE  )

INDB( i0gtBB,B,B,LE   )  INDF( i0gtBI,B,I,ALE  )  INDF( i0gtBD,B,D,TLEXD)  /* >  */
INDF( i0gtIB,I,B,ALE  )  INDF( i0gtII,I,I,ALE  )  INDF( i0gtID,I,D,TLEXD)
INDF( i0gtDB,D,B,TLEDX)  INDF( i0gtDI,D,I,TLEDX)  INDF( i0gtDD,D,D,TLE  )

INDB( i1gtBB,B,B,GT   )  INDF( i1gtBI,B,I,AGT  )  INDF( i1gtBD,B,D,TGTXD)
INDF( i1gtIB,I,B,AGT  )  INDF( i1gtII,I,I,AGT  )  INDF( i1gtID,I,D,TGTXD)
INDF( i1gtDB,D,B,TGTDX)  INDF( i1gtDI,D,I,TGTDX)  INDF( i1gtDD,D,D,TGT  )

JNDB( j0gtBB,B,B,LE   )  JNDF( j0gtBI,B,I,ALE  )  JNDF( j0gtBD,B,D,TLEXD)
JNDF( j0gtIB,I,B,ALE  )  JNDF( j0gtII,I,I,ALE  )  JNDF( j0gtID,I,D,TLEXD)
JNDF( j0gtDB,D,B,TLEDX)  JNDF( j0gtDI,D,I,TLEDX)  JNDF( j0gtDD,D,D,TLE  )

JNDB( j1gtBB,B,B,GT   )  JNDF( j1gtBI,B,I,AGT  )  JNDF( j1gtBD,B,D,TGTXD)
JNDF( j1gtIB,I,B,AGT  )  JNDF( j1gtII,I,I,AGT  )  JNDF( j1gtID,I,D,TGTXD)
JNDF( j1gtDB,D,B,TGTDX)  JNDF( j1gtDI,D,I,TGTDX)  JNDF( j1gtDD,D,D,TGT  )

SUMB(sumgtBB,B,B,GT   )  SUMF(sumgtBI,B,I,AGT  )  SUMF(sumgtBD,B,D,TGTXD)
SUMF(sumgtIB,I,B,AGT  )  SUMF(sumgtII,I,I,AGT  )  SUMF(sumgtID,I,D,TGTXD)
SUMF(sumgtDB,D,B,TGTDX)  SUMF(sumgtDI,D,I,TGTDX)  SUMF(sumgtDD,D,D,TGT  )

ALLB(allgtBB,B,B,GT   )  ALLF(allgtBI,B,I,AGT  )  ALLF(allgtBD,B,D,TGTXD)
ALLF(allgtIB,I,B,AGT  )  ALLF(allgtII,I,I,AGT  )  ALLF(allgtID,I,D,TGTXD)
ALLF(allgtDB,D,B,TGTDX)  ALLF(allgtDI,D,I,TGTDX)  ALLF(allgtDD,D,D,TGT  )

ANYB(anygtBB,B,B,GT   )  ANYF(anygtBI,B,I,AGT  )  ANYF(anygtBD,B,D,TGTXD)
ANYF(anygtIB,I,B,AGT  )  ANYF(anygtII,I,I,AGT  )  ANYF(anygtID,I,D,TGTXD)
ANYF(anygtDB,D,B,TGTDX)  ANYF(anygtDI,D,I,TGTDX)  ANYF(anygtDD,D,D,TGT  )

IFBB(ifbgtBB,B,B,GT   )  IFBF(ifbgtBI,B,I,AGT  )  IFBF(ifbgtBD,B,D,TGTXD)
IFBF(ifbgtIB,I,B,AGT  )  IFBF(ifbgtII,I,I,AGT  )  IFBF(ifbgtID,I,D,TGTXD)
IFBF(ifbgtDB,D,B,TGTDX)  IFBF(ifbgtDI,D,I,TGTDX)  IFBF(ifbgtDD,D,D,TGT  )


static AF atcompxy[]={  /* table for (B01,INT,FL) vs. (B01,INT,FL) */
 i0eqBB, i0eqBI, i0eqBD,   i0eqIB, i0eqII, i0eqID,   i0eqDB, i0eqDI, i0eqDD,   /* 0 */
 i0neBB, i0neBI, i0neBD,   i0neIB, i0neII, i0neID,   i0neDB, i0neDI, i0neDD,
 i0ltBB, i0ltBI, i0ltBD,   i0ltIB, i0ltII, i0ltID,   i0ltDB, i0ltDI, i0ltDD,
 i0leBB, i0leBI, i0leBD,   i0leIB, i0leII, i0leID,   i0leDB, i0leDI, i0leDD,
 i0geBB, i0geBI, i0geBD,   i0geIB, i0geII, i0geID,   i0geDB, i0geDI, i0geDD,
 i0gtBB, i0gtBI, i0gtBD,   i0gtIB, i0gtII, i0gtID,   i0gtDB, i0gtDI, i0gtDD,
 0L,     0L,     0L,       0L,     0L,     0L,       0L,     0L,     0L,
 0L,     0L,     0L,       0L,     0L,     0L,       0L,     0L,     0L,

 i1eqBB, i1eqBI, i1eqBD,   i1eqIB, i1eqII, i1eqID,   i1eqDB, i1eqDI, i1eqDD,   /* 1 */
 i1neBB, i1neBI, i1neBD,   i1neIB, i1neII, i1neID,   i1neDB, i1neDI, i1neDD,
 i1ltBB, i1ltBI, i1ltBD,   i1ltIB, i1ltII, i1ltID,   i1ltDB, i1ltDI, i1ltDD,
 i1leBB, i1leBI, i1leBD,   i1leIB, i1leII, i1leID,   i1leDB, i1leDI, i1leDD,
 i1geBB, i1geBI, i1geBD,   i1geIB, i1geII, i1geID,   i1geDB, i1geDI, i1geDD,
 i1gtBB, i1gtBI, i1gtBD,   i1gtIB, i1gtII, i1gtID,   i1gtDB, i1gtDI, i1gtDD,
 0L,     0L,     0L,       0L,     0L,     0L,       0L,     0L,     0L,
 0L,     0L,     0L,       0L,     0L,     0L,       0L,     0L,     0L,

 j0eqBB, j0eqBI, j0eqBD,   j0eqIB, j0eqII, j0eqID,   j0eqDB, j0eqDI, j0eqDD,   /* 2 */
 j0neBB, j0neBI, j0neBD,   j0neIB, j0neII, j0neID,   j0neDB, j0neDI, j0neDD,
 j0ltBB, j0ltBI, j0ltBD,   j0ltIB, j0ltII, j0ltID,   j0ltDB, j0ltDI, j0ltDD,
 j0leBB, j0leBI, j0leBD,   j0leIB, j0leII, j0leID,   j0leDB, j0leDI, j0leDD,
 j0geBB, j0geBI, j0geBD,   j0geIB, j0geII, j0geID,   j0geDB, j0geDI, j0geDD,
 j0gtBB, j0gtBI, j0gtBD,   j0gtIB, j0gtII, j0gtID,   j0gtDB, j0gtDI, j0gtDD,
 0L,     0L,     0L,       0L,     0L,     0L,       0L,     0L,     0L,
 0L,     0L,     0L,       0L,     0L,     0L,       0L,     0L,     0L,

 j1eqBB, j1eqBI, j1eqBD,   j1eqIB, j1eqII, j1eqID,   j1eqDB, j1eqDI, j1eqDD,   /* 3 */
 j1neBB, j1neBI, j1neBD,   j1neIB, j1neII, j1neID,   j1neDB, j1neDI, j1neDD,
 j1ltBB, j1ltBI, j1ltBD,   j1ltIB, j1ltII, j1ltID,   j1ltDB, j1ltDI, j1ltDD,
 j1leBB, j1leBI, j1leBD,   j1leIB, j1leII, j1leID,   j1leDB, j1leDI, j1leDD,
 j1geBB, j1geBI, j1geBD,   j1geIB, j1geII, j1geID,   j1geDB, j1geDI, j1geDD,
 j1gtBB, j1gtBI, j1gtBD,   j1gtIB, j1gtII, j1gtID,   j1gtDB, j1gtDI, j1gtDD,
 0L,     0L,     0L,       0L,     0L,     0L,       0L,     0L,     0L,
 0L,     0L,     0L,       0L,     0L,     0L,       0L,     0L,     0L,

 sumeqBB,sumeqBI,sumeqBD,  sumeqIB,sumeqII,sumeqID,  sumeqDB,sumeqDI,sumeqDD,  /* 4 */
 sumneBB,sumneBI,sumneBD,  sumneIB,sumneII,sumneID,  sumneDB,sumneDI,sumneDD,
 sumltBB,sumltBI,sumltBD,  sumltIB,sumltII,sumltID,  sumltDB,sumltDI,sumltDD,
 sumleBB,sumleBI,sumleBD,  sumleIB,sumleII,sumleID,  sumleDB,sumleDI,sumleDD,
 sumgeBB,sumgeBI,sumgeBD,  sumgeIB,sumgeII,sumgeID,  sumgeDB,sumgeDI,sumgeDD,
 sumgtBB,sumgtBI,sumgtBD,  sumgtIB,sumgtII,sumgtID,  sumgtDB,sumgtDI,sumgtDD,
 0L,     0L,     0L,       0L,     0L,     0L,       0L,     0L,     0L,
 0L,     0L,     0L,       0L,     0L,     0L,       0L,     0L,     0L,

 anyeqBB,anyeqBI,anyeqBD,  anyeqIB,anyeqII,anyeqID,  anyeqDB,anyeqDI,anyeqDD,  /* 5 */
 anyneBB,anyneBI,anyneBD,  anyneIB,anyneII,anyneID,  anyneDB,anyneDI,anyneDD,
 anyltBB,anyltBI,anyltBD,  anyltIB,anyltII,anyltID,  anyltDB,anyltDI,anyltDD,
 anyleBB,anyleBI,anyleBD,  anyleIB,anyleII,anyleID,  anyleDB,anyleDI,anyleDD,
 anygeBB,anygeBI,anygeBD,  anygeIB,anygeII,anygeID,  anygeDB,anygeDI,anygeDD,
 anygtBB,anygtBI,anygtBD,  anygtIB,anygtII,anygtID,  anygtDB,anygtDI,anygtDD,
 0L,     0L,     0L,       0L,     0L,     0L,       0L,     0L,     0L,
 0L,     0L,     0L,       0L,     0L,     0L,       0L,     0L,     0L,

 alleqBB,alleqBI,alleqBD,  alleqIB,alleqII,alleqID,  alleqDB,alleqDI,alleqDD,  /* 6 */
 allneBB,allneBI,allneBD,  allneIB,allneII,allneID,  allneDB,allneDI,allneDD,
 allltBB,allltBI,allltBD,  allltIB,allltII,allltID,  allltDB,allltDI,allltDD,
 allleBB,allleBI,allleBD,  allleIB,allleII,allleID,  allleDB,allleDI,allleDD,
 allgeBB,allgeBI,allgeBD,  allgeIB,allgeII,allgeID,  allgeDB,allgeDI,allgeDD,
 allgtBB,allgtBI,allgtBD,  allgtIB,allgtII,allgtID,  allgtDB,allgtDI,allgtDD,
 0L,     0L,     0L,       0L,     0L,     0L,       0L,     0L,     0L,
 0L,     0L,     0L,       0L,     0L,     0L,       0L,     0L,     0L,

 ifbeqBB,ifbeqBI,ifbeqBD,  ifbeqIB,ifbeqII,ifbeqID,  ifbeqDB,ifbeqDI,ifbeqDD,  /* 7 */
 ifbneBB,ifbneBI,ifbneBD,  ifbneIB,ifbneII,ifbneID,  ifbneDB,ifbneDI,ifbneDD,
 ifbltBB,ifbltBI,ifbltBD,  ifbltIB,ifbltII,ifbltID,  ifbltDB,ifbltDI,ifbltDD,
 ifbleBB,ifbleBI,ifbleBD,  ifbleIB,ifbleII,ifbleID,  ifbleDB,ifbleDI,ifbleDD,
 ifbgeBB,ifbgeBI,ifbgeBD,  ifbgeIB,ifbgeII,ifbgeID,  ifbgeDB,ifbgeDI,ifbgeDD,
 ifbgtBB,ifbgtBI,ifbgtBD,  ifbgtIB,ifbgtII,ifbgtID,  ifbgtDB,ifbgtDI,ifbgtDD,
 0L,     0L,     0L,       0L,     0L,     0L,       0L,     0L,     0L,
 0L,     0L,     0L,       0L,     0L,     0L,       0L,     0L,     0L,
};

INDF( i0eqC,C,C,ANE)  INDF( i0neC,C,C,AEQ)
INDF( i1eqC,C,C,AEQ)  INDF( i1neC,C,C,ANE)
JNDF( j0eqC,C,C,ANE)  JNDF( j0neC,C,C,AEQ)
JNDF( j1eqC,C,C,AEQ)  JNDF( j1neC,C,C,ANE)
SUMF(sumeqC,C,C,AEQ)  SUMF(sumneC,C,C,ANE)
ALLF(alleqC,C,C,AEQ)  ALLF(allneC,C,C,ANE)
ANYF(anyeqC,C,C,AEQ)  ANYF(anyneC,C,C,ANE)
IFBF(ifbeqC,C,C,AEQ)  IFBF(ifbneC,C,C,ANE)

static AF atcompC[]={   /* table for LIT vs. LIT */
  i0eqC,  i0neC, 0L,0L,0L,0L,0L,0L,
  i1eqC,  i1neC, 0L,0L,0L,0L,0L,0L,
  j0eqC,  j0neC, 0L,0L,0L,0L,0L,0L,
  j1eqC,  j1neC, 0L,0L,0L,0L,0L,0L,
 sumeqC, sumneC, 0L,0L,0L,0L,0L,0L,
 anyeqC, anyneC, 0L,0L,0L,0L,0L,0L,
 alleqC, allneC, 0L,0L,0L,0L,0L,0L,
 ifbeqC, ifbneC, 0L,0L,0L,0L,0L,0L,
};

INDF( i0eqUS,US,US,ANE)  INDF( i0neUS,US,US,AEQ)
INDF( i1eqUS,US,US,AEQ)  INDF( i1neUS,US,US,ANE)
JNDF( j0eqUS,US,US,ANE)  JNDF( j0neUS,US,US,AEQ)
JNDF( j1eqUS,US,US,AEQ)  JNDF( j1neUS,US,US,ANE)
SUMF(sumeqUS,US,US,AEQ)  SUMF(sumneUS,US,US,ANE)
ALLF(alleqUS,US,US,AEQ)  ALLF(allneUS,US,US,ANE)
ANYF(anyeqUS,US,US,AEQ)  ANYF(anyneUS,US,US,ANE)
IFBF(ifbeqUS,US,US,AEQ)  IFBF(ifbneUS,US,US,ANE)

static AF atcompUS[]={   /* table for C2T vs. C2T */
  i0eqUS,  i0neUS, 0L,0L,0L,0L,0L,0L,
  i1eqUS,  i1neUS, 0L,0L,0L,0L,0L,0L,
  j0eqUS,  j0neUS, 0L,0L,0L,0L,0L,0L,
  j1eqUS,  j1neUS, 0L,0L,0L,0L,0L,0L,
 sumeqUS, sumneUS, 0L,0L,0L,0L,0L,0L,
 anyeqUS, anyneUS, 0L,0L,0L,0L,0L,0L,
 alleqUS, allneUS, 0L,0L,0L,0L,0L,0L,
 ifbeqUS, ifbneUS, 0L,0L,0L,0L,0L,0L,
};

INDF( i0eqC4,C4,C4,ANE)  INDF( i0neC4,C4,C4,AEQ)
INDF( i1eqC4,C4,C4,AEQ)  INDF( i1neC4,C4,C4,ANE)
JNDF( j0eqC4,C4,C4,ANE)  JNDF( j0neC4,C4,C4,AEQ)
JNDF( j1eqC4,C4,C4,AEQ)  JNDF( j1neC4,C4,C4,ANE)
SUMF(sumeqC4,C4,C4,AEQ)  SUMF(sumneC4,C4,C4,ANE)
ALLF(alleqC4,C4,C4,AEQ)  ALLF(allneC4,C4,C4,ANE)
ANYF(anyeqC4,C4,C4,AEQ)  ANYF(anyneC4,C4,C4,ANE)
IFBF(ifbeqC4,C4,C4,AEQ)  IFBF(ifbneC4,C4,C4,ANE)

static AF atcompC4[]={   /* table for C4T vs. C4T */
  i0eqC4,  i0neC4, 0L,0L,0L,0L,0L,0L,
  i1eqC4,  i1neC4, 0L,0L,0L,0L,0L,0L,
  j0eqC4,  j0neC4, 0L,0L,0L,0L,0L,0L,
  j1eqC4,  j1neC4, 0L,0L,0L,0L,0L,0L,
 sumeqC4, sumneC4, 0L,0L,0L,0L,0L,0L,
 anyeqC4, anyneC4, 0L,0L,0L,0L,0L,0L,
 alleqC4, allneC4, 0L,0L,0L,0L,0L,0L,
 ifbeqC4, ifbneC4, 0L,0L,0L,0L,0L,0L,
};

INDF( i0eqS,SB,SB,ANE) INDF( i0neS,SB,SB,AEQ) 
INDF( i1eqS,SB,SB,AEQ) INDF( i1neS,SB,SB,ANE) 
JNDF( j0eqS,SB,SB,ANE) JNDF( j0neS,SB,SB,AEQ) 
JNDF( j1eqS,SB,SB,AEQ) JNDF( j1neS,SB,SB,ANE) 
SUMF(sumeqS,SB,SB,AEQ) SUMF(sumneS,SB,SB,ANE)
ALLF(alleqS,SB,SB,AEQ) ALLF(allneS,SB,SB,ANE) 
ANYF(anyeqS,SB,SB,AEQ) ANYF(anyneS,SB,SB,ANE)
IFBF(ifbeqS,SB,SB,AEQ) IFBF(ifbneS,SB,SB,ANE) 

INDF( i0ltS,SB,SB,SBGE) INDF( i0leS,SB,SB,SBGT) INDF( i0geS,SB,SB,SBLT) INDF( i0gtS,SB,SB,SBLE)
INDF( i1ltS,SB,SB,SBLT) INDF( i1leS,SB,SB,SBLE) INDF( i1geS,SB,SB,SBGE) INDF( i1gtS,SB,SB,SBGT)
JNDF( j0ltS,SB,SB,SBGE) JNDF( j0leS,SB,SB,SBGT) JNDF( j0geS,SB,SB,SBLT) JNDF( j0gtS,SB,SB,SBLE)
JNDF( j1ltS,SB,SB,SBLT) JNDF( j1leS,SB,SB,SBLE) JNDF( j1geS,SB,SB,SBGE) JNDF( j1gtS,SB,SB,SBGT)
SUMF(sumltS,SB,SB,SBLT) SUMF(sumleS,SB,SB,SBLE) SUMF(sumgeS,SB,SB,SBGE) SUMF(sumgtS,SB,SB,SBGT)
ALLF(allltS,SB,SB,SBLT) ALLF(allleS,SB,SB,SBLE) ALLF(allgeS,SB,SB,SBGE) ALLF(allgtS,SB,SB,SBGT)
ANYF(anyltS,SB,SB,SBLT) ANYF(anyleS,SB,SB,SBLE) ANYF(anygeS,SB,SB,SBGE) ANYF(anygtS,SB,SB,SBGT)
IFBF(ifbltS,SB,SB,SBLT) IFBF(ifbleS,SB,SB,SBLE) IFBF(ifbgeS,SB,SB,SBGE) IFBF(ifbgtS,SB,SB,SBGT)


static AF atcompSB[]={  /* table for SBT vs. SBT */
  i0eqS, i0neS, i0ltS, i0leS, i0geS, i0gtS, 0L,0L,
  i1eqS, i1neS, i1ltS, i1leS, i1geS, i1gtS, 0L,0L,
  j0eqS, j0neS, j0ltS, j0leS, j0geS, j0gtS, 0L,0L,
  j1eqS, j1neS, j1ltS, j1leS, j1geS, j1gtS, 0L,0L,
 sumeqS,sumneS,sumltS,sumleS,sumgeS,sumgtS, 0L,0L,
 anyeqS,anyneS,anyltS,anyleS,anygeS,anygtS, 0L,0L,
 alleqS,allneS,allltS,allleS,allgeS,allgtS, 0L,0L,
 ifbeqS,ifbneS,ifbltS,ifbleS,ifbgeS,ifbgtS, 0L,0L,
};


static F2( jti0eps){R indexofsub( II0EPS,w,a);}
static F2( jti1eps){R indexofsub( II1EPS,w,a);}
static F2( jtj0eps){R indexofsub( IJ0EPS,w,a);}
static F2( jtj1eps){R indexofsub( IJ1EPS,w,a);}
static F2(jtsumeps){R indexofsub(ISUMEPS,w,a);}
static F2(jtanyeps){R indexofsub(IANYEPS,w,a);}
static F2(jtalleps){R indexofsub(IALLEPS,w,a);}
static F2(jtifbeps){R indexofsub(IIFBEPS,w,a);}

static AF atcompX[]={   /* table for any vs. any */
  0L,0L,0L,0L,0L,0L,        0L,  jti0eps,
  0L,0L,0L,0L,0L,0L,  jti1ebar,  jti1eps,
  0L,0L,0L,0L,0L,0L,        0L,  jtj0eps,
  0L,0L,0L,0L,0L,0L,        0L,  jtj1eps,
  0L,0L,0L,0L,0L,0L, jtsumebar, jtsumeps,
  0L,0L,0L,0L,0L,0L, jtanyebar, jtanyeps,
  0L,0L,0L,0L,0L,0L,        0L, jtalleps,
  0L,0L,0L,0L,0L,0L, jtifbebar, jtifbeps,
};


/*            f  0   1   2   3   4   5   6   7  */
/* m             =   ~:  <   <:  >:  >   E.  e. */
/* 0 f i. 0:     0   1   2   3   4   5   6   7  */
/* 1 f i. 1:     8   9  10  11  12  13  14  15  */
/* 2 f i: 0:    16  17  18  19  20  21  22  23  */
/* 3 f i: 1:    24  25  26  27  28  29  30  31  */
/* 4 [: + / f   32  33  34  35  36  37  38  39  */
/* 5 [: +./ f   40  41  42  43  44  45  46  47  */
/* 6 [: *./ f   48  49  50  51  52  53  54  55  */
/* 7 [: I.  f   56  57  58  59  60  61  62  63  */

AF jtatcompf(J jt,A a,A w,A self){AF f;I ar,at,m,wr,wt;
 RZ(a&&w);
 at=AT(a); ar=AR(a);
 wt=AT(w); wr=AR(w);
 m=VAV(self)->flag%256;
 if(1<ar||1<wr){if(32<=m&&m<=37||40<=m&&m<=45||48<=m&&m<=53)R(AF)jtfslashatg; RZ(7==m%8);}
 ASSERT(AN(a)==AN(w)||!ar||!wr||5<m%8,EVLENGTH);
 f=atcompX[m];
 if(!f){
  if(at&B01+INT+FL&&wt&B01+INT+FL)f=atcompxy[9*m+3*(at&B01?0:at&INT?1:2)+(wt&B01?0:wt&INT?1:2)];
  else if(at&LIT&&wt&LIT)         f=atcompC[m];
  else if(at&C2T&&wt&C2T)         f=atcompUS[m];
  else if(at&C4T&&wt&C4T)         f=atcompC4[m];
  else if(at&SBT&&wt&SBT)         f=atcompSB[m];
 }
 R f;
}    /* function table look-up for  comp i. 1:  and  i.&1@:comp  etc. */

