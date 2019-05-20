/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: f/ defns                                                       */

// n=#items in a CELL of w
// m=#cells to operate on
// c=#atoms in a cell
// d=#atoms in an item of a cell of w (thus c=d*n)

#if 0 // obsolete
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
#define REDUCEPFX(f,Tz,Tx,pfx,vecfn1,vecfnn)  \
 AHDRR(f,Tz,Tx){I i;Tz v;                              \
  if(d==1){x += m*n; z+=m; DQ(m, v=*--x; DQ(n-1, --x; v=pfx(*x,v);); *--z=v;)}  \
  else if(1==n){if(sizeof(Tz)!=sizeof(Tx)){DQ(d, *z++=    *x++;)}else{MC((C*)z,(C*)x,d*sizeof(Tz));}}          \
  else{z+=(m-1)*d; x+=(m*n-1)*d;                                        \
   for(i=0;i<m;++i,z-=d){                                    \
    Tx* RESTRICT y=x; x-=d; vecfn1(jt,d,z,x,y,1); x-=d;        \
    DQ(n-2,    vecfnn(jt,d,z,x,z,1); x-=d;);        \
  }}}

// used on idempotent verbs, using 2 accumulators
#define REDUCEPFXIDEM2(f,Tz,Tx,pfx,vecfn)  \
 AHDRR(f,Tz,Tx){I i;                              \
  if(d==1){x += m*n; z+=m; DQ(m, Tz v0=(Tz)*--x; Tz v1=v0; if(!(n&1))v1=*--x;  DQ((n-1)>>1, x-=2; v0=pfx(x[0],v0); v1=pfx(x[1],v1); ); v0=pfx(v0,v1); *--z=v0;)}  \
  else if(1==n){if(sizeof(Tz)!=sizeof(Tx)){DQ(d, *z++=    *x++;)}else{MC((C*)z,(C*)x,d*sizeof(Tz));}}          \
  else{z+=(m-1)*d; x+=(m*n-1)*d;                                        \
   for(i=0;i<m;++i,z-=d){                                    \
    Tx* RESTRICT y=x; x-=d; vecfn(jt,d,z,x,y,1); x-=d;        \
    DQ(n-2,    vecfn(jt,d,z,x,z,1); x-=d;);        \
  }}}

// used on idempotent verbs, using 4 accumulators but using the 256-bit instructions if available
#if C_AVX&&SY_64
#define REDUCEPFXIDEM2PRIM256(f,Tz,Tx,pfx,vecfn,prim,identity)  \
 AHDRR(f,Tz,Tx){I i;                              \
  if(d==1){redprim256rk1(prim,identity)}  \
  else if(1==n){if(sizeof(Tz)!=sizeof(Tx)){DQ(d, *z++=    *x++;)}else{MC((C*)z,(C*)x,d*sizeof(Tz));}}          \
  else{z+=(m-1)*d; x+=(m*n-1)*d;                                        \
   for(i=0;i<m;++i,z-=d){                                    \
    Tx* RESTRICT y=x; x-=d; vecfn(jt,d,z,x,y,1); x-=d;        \
    DQ(n-2,    vecfn(jt,d,z,x,z,1); x-=d;);        \
  }}}
#else
#define REDUCEPFXIDEM2PRIM256(f,Tz,Tx,pfx,vecfn,prim,identity) REDUCEPFXIDEM2(f,Tz,Tx,pfx,vecfn)
#endif

#if 0 // obsolete
#define REDUCENAN(f,Tz,Tx,pfx,vecfn)  \
 AHDRR(f,Tz,Tx){I i;Tx* RESTRICT y;Tz v,* RESTRICT zz;                              \
  NAN0;                                                           \
  if(d==1){x += m*n; z+=m; DQ(m, v=*--x; DQ(n-1, --x; v=pfx(*x,v);); *--z=v;)}  \
  else if(1==n){if(sizeof(Tz)!=sizeof(Tx)){DQ(n, *z++=    *x++;)}else{MC((C*)z,(C*)x,d*sizeof(Tz));}}          \
  else{zz=z+=m*d; x+=m*d*n;                                        \
   for(i=0;i<m;++i,zz-=d){                                    \
    y=x; x-=d; z=zz; DQ(d, --z; --x; --y; *z=pfx(*x,*y););         \
    DQ(n-2,    z=zz; DQ(d, --z; --x;      *z=pfx(*x,*z);));        \
  }}                                                               \
  NAN1V;                                                          \
}
#else
#define REDUCENAN(f,Tz,Tx,pfx,vecfn)  \
 AHDRR(f,Tz,Tx){I i;Tz v;                              \
  NAN0; if(d*m*n==0)SEGFAULT; /* scaf*/                                                          \
  if(d==1){x += m*n; z+=m; DQ(m, v=*--x; DQ(n-1, --x; v=pfx(*x,v);); *--z=v;)}  \
  else if(1==n){if(sizeof(Tz)!=sizeof(Tx)){DQ(n, *z++=    *x++;)}else{MC((C*)z,(C*)x,d*sizeof(Tz));}}          \
  else{z+=(m-1)*d; x+=(m*n-1)*d;                                        \
   for(i=0;i<m;++i,z-=d){                                    \
    Tx* RESTRICT y=x; x-=d; vecfn(jt,d,z,x,y,1); x-=d;        \
    DQ(n-2,    vecfn(jt,d,z,x,z,1); x-=d;);        \
  }}                                                               \
  NAN1V;                                                          \
}
#endif
#endif

#define REDUCCPFX(f,Tz,Tx,pfx)  \
 AHDRR(f,Tz,Tx){I i;Tx* RESTRICT y;Tz v,* RESTRICT zz;                              \
  if(d==1){x += m*n; z+=m; DQ(m, v=(Tz)*--x; DQ(n-1, --x; v=pfx(*x,v);); *--z=v;)}  \
  else if(1==n){DQ(d, *z++= (Tz)*x++;)}          \
  else{zz=z+=m*d; x+=m*d*n;                                       \
   for(i=0;i<m;++i,zz-=d){                                    \
    y=x; x-=d; z=zz; DQ(d, --z; --x; --y; *z=pfx(*x,*y););         \
    DQ(n-2,    z=zz; DQ(d, --z; --x;      *z=pfx(*x,*z);));        \
  }}}





#define REDUCEOVF(f,Tz,Tx,fr1,fvv,frn)  \
 AHDRR(f,I,I){C er=0;I i,* RESTRICT xx,*y,* RESTRICT zz;                          \
  if(d==1){xx=x; zz=z; DQ(m, z=zz++; x=xx; fr1(n,z,x); RER; xx += n;); R;}        \
  if(1==n){if(sizeof(Tz)!=sizeof(Tx)){DQ(d, *z++=*x++;)}else{MC((C*)z,(C*)x,d*sizeof(Tz));} R;}   \
  zz=z+=m*d; xx=x+=m*d*n;                                  \
  xx-=d; zz-=d;                                                 \
  for(i=0;i<m;++i,xx-=d,zz-=d){                                 \
   y=xx;   x=xx-=d; z=zz; fvv(d,z,x,y); RER;                    \
   DO(n-2, x=xx-=d; z=zz; frn(d,z,x);   RER;);                  \
 }}
