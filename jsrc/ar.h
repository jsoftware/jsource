/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: f/ defns                                                       */

// n=#items in a CELL of w
// m=#cells to operate on
// c=#atoms in a cell

#if 0
#define REDUCEPFX(f,Tz,Tx,pfx)  \
AHDRR(f,Tz,Tx){I d,i;Tx* RESTRICT y;Tz v,* RESTRICT zz;                              \
d=c/n; x+=m*c; zz=z+=m*d;                              \
if(1==d)DO(m, v=    *--x; DO(n-1, --x; v=pfx(*x,v);); *--z=v;)  \
else if(1==n)DO(d, *--z=    *--x;)                              \
else for(i=0;i<m;++i,zz-=d){                                    \
y=x; x-=d; z=zz; DO(d, --z; --x; --y; *z=pfx(*x,*y););         \
DO(n-2,    z=zz; DO(d, --z; --x;      *z=pfx(*x,*z);));        \
}}

#define REDUCENAN(f,Tz,Tx,pfx)  \
 AHDRR(f,Tz,Tx){I d,i;Tx* RESTRICT y;Tz v,* RESTRICT zz;                              \
  NAN0;                                                           \
  d=c/n; x+=m*c; zz=z+=m*d;                                       \
  if(1==d)DO(m, v=    *--x; DO(n-1, --x; v=pfx(*x,v);); *--z=v;)  \
  else if(1==n)DO(d, *--z=    *--x;)                              \
  else for(i=0;i<m;++i,zz-=d){                                    \
   y=x; x-=d; z=zz; DO(d, --z; --x; --y; *z=pfx(*x,*y););         \
   DO(n-2,    z=zz; DO(d, --z; --x;      *z=pfx(*x,*z);));        \
  }                                                               \
  NAN1V;                                                          \
 }

#define REDUCCPFX(f,Tz,Tx,pfx)  \
 AHDRR(f,Tz,Tx){I d,i;Tx* RESTRICT y;Tz v,* RESTRICT zz;                              \
  d=c/n; x+=m*c; zz=z+=m*d;                                       \
  if(1==d)DO(m, v=(Tz)*--x; DO(n-1, --x; v=pfx(*x,v);); *--z=v;)  \
  else if(1==n)DO(d, *--z=(Tz)*--x;)                              \
  else for(i=0;i<m;++i,zz-=d){                                    \
   y=x; x-=d; z=zz; DO(d, --z; --x; --y; *z=pfx(*x,*y););         \
   DO(n-2,    z=zz; DO(d, --z; --x;      *z=pfx(*x,*z);));        \
 }}

#define REDUCEOVF(f,Tz,Tx,fr1,fvv,frn)  \
 AHDRR(f,I,I){C er=0;I d,i,* RESTRICT xx,*y,* RESTRICT zz;                          \
  d=c/n; xx=x+=m*c; zz=z+=m*d;                                  \
  if(1==d){DO(m, z=--zz; x=xx-=c; fr1(n,z,x); RER;); R;}        \
  if(1==n){DO(d, *--z=*--x;); R;}                               \
  xx-=d; zz-=d;                                                 \
  for(i=0;i<m;++i,xx-=d,zz-=d){                                 \
   y=xx;   x=xx-=d; z=zz; fvv(d,z,x,y); RER;                    \
   DO(n-2, x=xx-=d; z=zz; frn(d,z,x);   RER;);                  \
 }}

#else
#define REDUCEPFX(f,Tz,Tx,pfx)  \
 AHDRR(f,Tz,Tx){I d,i;Tx* RESTRICT y;Tz v,* RESTRICT zz;                              \
  if(c==n){x += m*c; z+=m; DQ(m, v=*--x; DQ(n-1, --x; v=pfx(*x,v);); *--z=v;)}  \
  else if(1==n){if(sizeof(Tz)!=sizeof(Tx)){DQ(c, *z++=    *x++;)}else{MC((C*)z,(C*)x,c*sizeof(Tz));}}          \
  else{d=c/n; x+=m*c; zz=z+=m*d;                                       \
   for(i=0;i<m;++i,zz-=d){                                    \
    y=x; x-=d; z=zz; DQ(d, --z; --x; --y; *z=pfx(*x,*y););         \
    DQ(n-2,    z=zz; DQ(d, --z; --x;      *z=pfx(*x,*z);));        \
  }}}

#define REDUCENAN(f,Tz,Tx,pfx)  \
 AHDRR(f,Tz,Tx){I d,i;Tx* RESTRICT y;Tz v,* RESTRICT zz;                              \
  NAN0;                                                           \
  if(c==n){x += m*c; z+=m; DQ(m, v=*--x; DQ(n-1, --x; v=pfx(*x,v);); *--z=v;)}  \
  else if(1==n){if(sizeof(Tz)!=sizeof(Tx)){DQ(c, *z++=    *x++;)}else{MC((C*)z,(C*)x,c*sizeof(Tz));}}          \
  else{d=c/n; x+=m*c; zz=z+=m*d;                                       \
   for(i=0;i<m;++i,zz-=d){                                    \
    y=x; x-=d; z=zz; DQ(d, --z; --x; --y; *z=pfx(*x,*y););         \
    DQ(n-2,    z=zz; DQ(d, --z; --x;      *z=pfx(*x,*z);));        \
  }}                                                               \
  NAN1V;                                                          \
}
#endif

#define REDUCCPFX(f,Tz,Tx,pfx)  \
 AHDRR(f,Tz,Tx){I d,i;Tx* RESTRICT y;Tz v,* RESTRICT zz;                              \
  if(c==n){x += m*c; z+=m; DQ(m, v=(Tz)*--x; DQ(n-1, --x; v=pfx(*x,v);); *--z=v;)}  \
  else if(1==n){DQ(c, *z++= (Tz)*x++;)}          \
  else{d=c/n; x+=m*c; zz=z+=m*d;                                       \
   for(i=0;i<m;++i,zz-=d){                                    \
    y=x; x-=d; z=zz; DQ(d, --z; --x; --y; *z=pfx(*x,*y););         \
    DQ(n-2,    z=zz; DQ(d, --z; --x;      *z=pfx(*x,*z);));        \
  }}}




#define REDUCEOVF(f,Tz,Tx,fr1,fvv,frn)  \
 AHDRR(f,I,I){C er=0;I d,i,* RESTRICT xx,*y,* RESTRICT zz;                          \
  if(c==n){xx=x; zz=z; DQ(m, z=zz++; x=xx; fr1(n,z,x); RER; xx += c;); R;}        \
  if(1==n){if(sizeof(Tz)!=sizeof(Tx)){DQ(c, *z++=*x++;)}else{MC((C*)z,(C*)x,c*sizeof(Tz));} R;}   \
  d=c/n; xx=x+=m*c; zz=z+=m*d;                                  \
  xx-=d; zz-=d;                                                 \
  for(i=0;i<m;++i,xx-=d,zz-=d){                                 \
   y=xx;   x=xx-=d; z=zz; fvv(d,z,x,y); RER;                    \
   DO(n-2, x=xx-=d; z=zz; frn(d,z,x);   RER;);                  \
 }}
