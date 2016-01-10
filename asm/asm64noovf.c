/* C source for J asm routines
This source is compiled to create asm source.

The asm source is modified by fix_overflow.ijs
to change the ov(x) marker to return EWOF on overflow.

Other operations must not come between the marker
and the op requiring the test.

The C code must be kept very simple. For example,
   r=*x++**y++; ov(r); //! won't work (tests ++)

Asm shuffles instructions. Index incs/decs get moved
around so keep them away from the test.

It is possible for the test to be incorrectly placed.
The test suite must validate all overflows.
*/

/* fvv  zv=.xv+yv     0<n */
/* fv1  zv=.xv+y      0<n */
/* f1v  zv=.x +yv     0<n */
/* frv  zv=.xv+zv     0<n */
/* fr   z =.+/  xv    1<n */
/* fp   zv=.+/\ xv    1<n */
/* fs   zv=.+/\.xv    1<n */

typedef long long I;
typedef char C;

/* ops that require overflow tests */
#define FPLUS        +
#define FMINUS       -
#define FTIMES       * 
 
#define EWOF 50
#define ov(x)    x=~x;
#define lp       if(m>0) goto l1; return 0;
#define lpm      if(i<m) goto l1; return 0;
#define lpnr     if(m>0) goto l1;

C asmplusvv(I m, I* z, I* x, I* y)
{
 I t;
l1:
 --m;
 t=x[m] FPLUS y[m];
 ov(t)
 z[m]=t;
 lp
}

C asmminusvv(I m, I* z, I* x, I* y)
{
 I t;
l1:
 --m;
 t=x[m] FMINUS y[m];
 ov(t)
 z[m]=t;
 lp
}

C asmtymesvv(I m, I* z, I* x, I* y)
{
 I t;
l1:
 --m;
 t=x[m] FTIMES y[m];
 ov(t)
 z[m]=t;
 lp
}

C asmplus1v(I m, I* z, I u, I* y)
{
 I t;
l1:
 --m;
 t=u FPLUS y[m];
 ov(t)
 z[m]=t;
 lp
}

C asmminus1v(I m, I* z, I u, I* y)
{
 I t;
l1:
 --m;
 t=u FMINUS y[m];
 ov(t)
 z[m]=t;
 lp
}

C asmtymes1v(I m, I* z, I u, I* y)
{
 I t;
l1:
 --m;
 t=u FTIMES y[m];
 ov(t)
 z[m]=t;
 lp
}

C asmminusv1(I m, I* z, I* x, I v)
{
 I t;
l1:
 --m;
 t=x[m] FMINUS v;
 ov(t)
 z[m]=t;
 lp
}

C asmplusrv(I m, I* z, I* x)
{
 I t;
l1:
 --m;
 t=x[m] FPLUS z[m];
 ov(t)
 z[m]=t;
 lp
}

C asmminusrv(I m, I* z, I* x)
{
 I t;
l1:
 --m;
 t=x[m] FMINUS z[m];
 ov(t)
 z[m]=t;
 lp
}

C asmtymesrv(I m, I* z, I* x)
{
 I t;
l1:
 --m;
 t=x[m] FTIMES z[m];
 ov(t)
 z[m]=t;
 lp
}

C asmplusr(I m, I* z, I* x)
{
 I r=0;
l1:
 --m;
 r= r FPLUS x[m];
 ov(r)
 lpnr
 *z=r;
 return 0;
}

C asmminusr(I m, I* z, I* x)
{
 I r=0;
l1:
 --m;
 r=x[m] FMINUS r;
 ov(r)
 lpnr
 *z=r;
 return 0;
}

C asmtymesr(I m, I* z, I* x)
{
 I r=1;
l1:
 --m;
 r= r FTIMES x[m];
 ov(r)
 lpnr
 *z=r;
 return 0;
}

C asmplusp(I m, I* z, I* x)
{
 I r=0;
 I i=0;
l1:
 r= r FPLUS x[i];
 ov(r)
 z[i]=r;
 ++i;
 lpm
}

C asmminusp(I m, I* z, I* x)
{
 I r=0;
 I i=0;
l1:
 r= r FPLUS x[i];
 ov(r)
 z[i]=r;
 ++i;
 if(i==m) return 0;
 r= r FMINUS x[i];
 ov(r)
 z[i]=r;
 ++i;
 lpm
}

C asmtymesp(I m, I* z, I* x)
{
 I r=1;
 I i=0;
l1:
 r= r FTIMES x[i];
 ov(r)
 z[i]=r;
 ++i;
 lpm
}

C asmpluss(I m, I* z, I* x)
{
 I r=0;
l1:
 --m;
 r= r FPLUS x[m];
 ov(r)
 z[m]=r;
 lp
 return 0;
}
 
C asmminuss(I m, I* z, I* x)
{
 I r=0;
l1:
 --m;
 r=x[m] FMINUS r;
 ov(r)
 z[m]=r;
 lp
 return 0;
}

C asmtymess(I m, I* z, I* x)
{
 I r=1;
l1:
 --m;
 r= r FTIMES x[m];
 ov(r)
 z[m]=r;
 lp
 return 0;
}

C asminnerprod(I m, I*z, I*x, I*y, I c)
{
 I t;
 if(m==0) return 0;
 *z=0;
l1:
 --m;
 t=x[m] FTIMES y[m*c];
 ov(t)
 t= *z FPLUS t;
 ov(t)
 *z=t;
 lp
}

/*
C asminnerprod(I n,I*z,I u,I*y){I i;
 for(i=0;i<n;++i) *z++ += u * *y++;
 return ec;
}
*/

C asminnerprodx(I m,I*z,I u,I*y)
{
 I i=-1,t;
l1:
 ++i;
 if(i==m) return 0;
 t= u FTIMES y[i];
 ov(t)
 t= t FPLUS z[i];
 ov(t)
 z[i]=t;
 goto l1;
}
