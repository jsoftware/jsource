/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Macros and Defined Constants for Atomic (Scalar) Verbs           */

typedef struct {VF f;I cv;} VA2;
typedef struct {VA2 p2[13];VA2 pins[7];VA2 ppfx[7];VA2 psfx[7];} VA;
typedef struct {VA2 p1[6];} UA;

                                    /*   cv - control vector               */
#define VBB             B01         /* convert arguments to B              */
#define VII             INT         /* convert arguments to I              */
#define VDD             FL          /* convert arguments to D              */
#define VZZ             CMPX        /* convert arguments to Z              */
#define Vxx             XNUM        /* convert arguments to XNUM           */
#define VQQ             RAT         /* convert arguments to RAT            */
#define VARGMSK         (VBB|VII|VDD|VZZ|Vxx|VQQ)  // mask for argument requested type
#define VRESX           8           // bit position for result flags
#define VB              (B01<<VRESX)/* result type B                       */
#define VI              (INT<<VRESX)/* result type I                       */
#define VD              (FL<<VRESX) /* result type D                       */
#define VZ              (CMPX<<VRESX)/* result type Z                       */
#define VX              (XNUM<<VRESX)/* result type XNUM                    */
#define VQ              (RAT<<VRESX) /* result type RAT                     */
#define VSB             (SBT<<VRESX) /* result type SBT                     */
#define VRESMSK         (VB|VI|VD|VZ|VX|VQ|VSB)  // mask for result-type
#define VRD             (SLIT<<VRESX)// convert result to D if possible - unused code point
#define VRI             (SBOX<<VRESX)// convert result to I if possible - unused code point
#define VXCVTYPEX       25          // bit position for VX conversion type
#define VXCVTYPEMSK     (3<<VXCVTYPEX)  // mask for bit-positions hold XNUM conversion type
#define VXX             (Vxx|(XMEXACT<<VXCVTYPEX))  // exact conversion
#define VXEQ            (Vxx|(XMEXMT<<VXCVTYPEX))   /* convert to XNUM for = ~:            */
#define VXCF            (Vxx|(XMCEIL<<VXCVTYPEX))   /* convert to XNUM ceiling/floor       */
#define VXFC            (Vxx|(XMFLR<<VXCVTYPEX))  /* convert to XNUM floor/ceiling       */
#define VIPOKWX         27      // This routine can put its result over W
#define VIPOKW          (1<<VIPOKWX)
#define VIPOKAX         28      // This routine can put its result over A
#define VIPOKA          (1<<VIPOKAX)
#define VCANHALTX       29    // This routine can generate an error after it has started
#define VCANHALT        (1<<VCANHALTX)

// Extract the argument-conversion type from cv coming from the table
#define atype(x) ((x)&VARGMSK)

// Extract the result type from cv coming from the table
#define rtype(x) (((x)&VRESMSK)>>VRESX)

#if SY_64
#define NOT(v)          ((v)^0x0101010101010101)
#else
#define NOT(v)          ((v)^0x01010101)
#endif

#define SNOT(v)         ((v)^0x0101)
#define INOT(v)         ((v)^0x01010101)

#define AND(u,v)        ( u &  v)
#define GT(u,v)         ( u & ~v)
#define LT(u,v)         (~u &  v)
#define OR(u,v)         ( u |  v)
#define NE(u,v)         ( u ^  v)
#define GE(u,v)         NOT( LT(u,v))
#define LE(u,v)         NOT( GT(u,v))
#define EQ(u,v)         NOT( NE(u,v))
#define NOR(u,v)        NOT( OR(u,v))
#define NAND(u,v)       NOT(AND(u,v))

#define IAND            AND
#define IGT             GT
#define ILT             LT
#define IOR             OR
#define INE             NE
#define IGE(u,v)        INOT(ILT(u,v))
#define ILE(u,v)        INOT(IGT(u,v))
#define IEQ(u,v)        INOT(INE(u,v))
#define INOR(u,v)       INOT(IOR(u,v))
#define INAND(u,v)      INOT(IAND(u,v))

#define SAND            AND
#define SGT             GT
#define SLT             LT
#define SOR             OR
#define SNE             NE
#define SGE(u,v)        SNOT(ILT(u,v))
#define SLE(u,v)        SNOT(IGT(u,v))
#define SEQ(u,v)        SNOT(INE(u,v))
#define SNOR(u,v)       SNOT(IOR(u,v))
#define SNAND(u,v)      SNOT(IAND(u,v))

#define BAND(u,v)       (u && v)
#define BGT(u,v)        (u >  v)
#define BLT(u,v)        (u <  v)
#define BOR(u,v)        (u || v)
#define BNE(u,v)        (u != v)
#define BGE(u,v)        (u >= v)
#define BLE(u,v)        (u <= v)
#define BEQ(u,v)        (u == v)
#define BNOR(u,v)       (!(u||v))
#define BNAND(u,v)      (!(u&&v))

#define PLUS(u,v)       ((u)+   (v))
#define PLUSO(u,v)      ((u)+(D)(v))
#define MINUS(u,v)      ((u)-   (v))
#define MINUSO(u,v)     ((u)-(D)(v))
#define TYMES(u,v)      ((u)&&(v)?(u)*   (v):0)
#define TYMESO(u,v)     ((u)&&(v)?(u)*(D)(v):0)
#define DIV(u,v)        ((u)||(v)?(u)/(v):0)

#define SBORDER(v)      (SBUV(v)->order)

#define SBLT(u,v)       (SBORDER(u)< SBORDER(v))
#define SBLE(u,v)       (SBORDER(u)<=SBORDER(v))
#define SBGT(u,v)       (SBORDER(u)> SBORDER(v))
#define SBGE(u,v)       (SBORDER(u)>=SBORDER(v))

#define SBMIN(u,v)      (SBORDER(u)<=SBORDER(v)?(u):(v))
#define SBMAX(u,v)      (SBORDER(u)>=SBORDER(v)?(u):(v))

#define BOV(exp)        if(exp){er=EWOV; break;}

#define BW0000(x,y)     (0)
#define BW0001(x,y)     (   (x)& (y) )
#define BW0010(x,y)     (   (x)&~(y) )
#define BW0011(x,y)     (x)

#define BW0100(x,y)     (  ~(x)& (y) )
#define BW0101(x,y)     (y)
#define BW0110(x,y)     (   (x)^ (y) )
#define BW0111(x,y)     (   (x)| (y) )

#define BW1000(x,y)     (~( (x)| (y)))
#define BW1001(x,y)     (~( (x)^ (y)))
#define BW1010(x,y)     (       ~(y) )
#define BW1011(x,y)     (   (x)|~(y) )

#define BW1100(x,y)     (  ~(x)      )
#define BW1101(x,y)     (  ~(x)| (y) )
#define BW1110(x,y)     (~( (x)& (y)))
#define BW1111(x,y)     (-1)

#define AHDR1(f,Tz,Tx)          void f(J jt,            I n,Tz* RESTRICT z,Tx* RESTRICT x)
#define AHDR2(f,Tz,Tx,Ty)       void f(J jt,B b,I m,    I n,Tz* RESTRICT z,Tx* RESTRICT x,Ty* RESTRICT y)
#define AHDRP(f,Tz,Tx)          void f(J jt,    I m,I c,I n,Tz* RESTRICT z,Tx* RESTRICT x)
#define AHDRR(f,Tz,Tx)          void f(J jt,    I m,I c,I n,Tz* RESTRICT z,Tx* RESTRICT x)
#define AHDRS(f,Tz,Tx)          void f(J jt,    I m,I c,I n,Tz* RESTRICT z,Tx* RESTRICT x)

/*
 b    1 iff cell rank of a <= cell rank of w
 m    # atoms of in the cell with the smaller rank
 n    excess # of cell atoms
 z    pointer to result atoms
 x    pointer to a      atoms
 y    pointer to w      atoms
*/

#define AMON(f,Tz,Tx,stmt)      AHDR1(f,Tz,Tx){DO(n, {stmt} ++z; ++x;);}

#define AIFX(f,Tz,Tx,Ty,symb)  \
 AHDR2(f,Tz,Tx,Ty){Tx u;Ty v;                            \
  if(1==n)  DO(m,               *z++=*x++ symb *y++; )   \
  else if(b)DO(m, u=*x++; DO(n, *z++=u    symb *y++;))   \
  else      DO(m, v=*y++; DO(n, *z++=*x++ symb v;   ));  \
 }

#define AOVF(f,Tz,Tx,Ty,fvv,f1v,fv1)  \
 AHDR2(f,I,I,I){C er=0;I u,v,*x1,*y1,*z1;                                       \
  if(1==n)  {fvv(m,z,x,y); RER;}                                                \
  else if(b){z1=z; y1=y; DO(m, u=*x++; f1v(n,z,u,y); RER; z=z1+=n; y=y1+=n;);}  \
  else      {z1=z; x1=x; DO(m, v=*y++; fv1(n,z,x,v); RER; z=z1+=n; x=x1+=n;);}  \
 }

#define APFX(f,Tz,Tx,Ty,pfx)   \
 AHDR2(f,Tz,Tx,Ty){Tx u;Ty v;                                  \
  if(1==n)  DO(m,               *z++=pfx(*x,*y); x++; y++; )   \
  else if(b)DO(m, u=*x++; DO(n, *z++=pfx( u,*y);      y++;))   \
  else      DO(m, v=*y++; DO(n, *z++=pfx(*x, v); x++;     ));  \
 }

#define ANAN(f,Tz,Tx,Ty,pfx)   \
 AHDR2(f,Tz,Tx,Ty){Tx u;Ty v;                                  \
  NAN0;                                                        \
  if(1==n)  DO(m,               *z++=pfx(*x,*y); x++; y++; )   \
  else if(b)DO(m, u=*x++; DO(n, *z++=pfx( u,*y);      y++;))   \
  else      DO(m, v=*y++; DO(n, *z++=pfx(*x, v); x++;     ));  \
  NAN1V;                                                       \
 }

#define APFY(f,Tz,Tx,Ty,pfx)   \
 AHDR2(f,Tz,A,A){A u,v;I c,d;                                                                  \
  c=jt->rela;                                                                                  \
  d=jt->relw;                                                                                  \
  switch((c?2:0)+(d?1:0)){                                                                     \
   case 0:                                                                                     \
    if(1==n)  DO(m,                         *z++=pfx(*x,          *y          ); x++; y++; )   \
    else if(b)DO(m, u=         *x++;  DO(n, *z++=pfx(u,           *y          );      y++;))   \
    else      DO(m, v=         *y++;  DO(n, *z++=pfx(*x,          v           ); x++;     ));  \
    R;                                                                                         \
   case 1:                                                                                     \
    if(1==n)  DO(m,                         *z++=pfx(*x,          (A)(d+(I)*y)); x++; y++; )   \
    else if(b)DO(m, u=         *x++;  DO(n, *z++=pfx(u,           (A)(d+(I)*y));      y++;))   \
    else      DO(m, v=(A)(d+(I)*y++); DO(n, *z++=pfx(*x,          v           ); x++;     ));  \
    R;                                                                                         \
   case 2:                                                                                     \
    if(1==n)  DO(m,                         *z++=pfx((A)(c+(I)*x),*y          ); x++; y++; )   \
    else if(b)DO(m, u=(A)(c+(I)*x++); DO(n, *z++=pfx(u,           *y          );      y++;))   \
    else      DO(m, v=         *y++;  DO(n, *z++=pfx((A)(c+(I)*x),v           ); x++;     ));  \
    R;                                                                                         \
   case 3:                                                                                     \
    if(1==n)  DO(m,                         *z++=pfx((A)(c+(I)*x),(A)(d+(I)*y)); x++; y++; )   \
    else if(b)DO(m, u=(A)(c+(I)*x++); DO(n, *z++=pfx(u,           (A)(d+(I)*y));      y++;))   \
    else      DO(m, v=(A)(d+(I)*y++); DO(n, *z++=pfx((A)(c+(I)*x),v           ); x++;     ));  \
 }}


/* Embedded visual tools v3.0 fails perform the z++ on all wince platforms. -KBI */
#if SY_WINCE
#define ACMP(f,Tz,Tx,Ty,pfx)   \
 AHDR2(f,B,Tx,Ty){D u,v;                                          \
  if(1==n)  DO(m, u=(D)*x++;       v=(D)*y++; *z++=pfx(u,v); )    \
  else if(b)DO(m, u=(D)*x++; DO(n, v=(D)*y++; *z++=pfx(u,v);))    \
  else      DO(m, v=(D)*y++; DO(n, u=(D)*x++; *z++=pfx(u,v);));   \
 }
#else
#define ACMP(f,Tz,Tx,Ty,pfx)   \
 AHDR2(f,B,Tx,Ty){D u,v;                                             \
  if(1==n)  DO(m, u=(D)*x++;       v=(D)*y++; *z=pfx(u,v); z++; )    \
  else if(b)DO(m, u=(D)*x++; DO(n, v=(D)*y++; *z=pfx(u,v); z++;))    \
  else      DO(m, v=(D)*y++; DO(n, u=(D)*x++; *z=pfx(u,v); z++;));   \
 }
#endif


#define BFSUB(xb,yi,pfx,bpfx)  \
 {B*a,*p,*yb,*zb;I j,k;                                        \
  a=xb; p=(B*)&u; k=0;                                         \
  if(0==r)for(j=0;j<m;++j){                                    \
   c=*a++; DO(SZI, p[i]=c;); DO(q, v=*yi++; *zz++=pfx(u,v););  \
  }else   for(j=0;j<m;++j){                                    \
   q=(t-k)/SZI; r=(t-k)%SZI;                                   \
   c=*a++; DO(SZI, p[i]=c;); DO(q, v=*yi++; *zz++=pfx(u,v););  \
   if(0==r)k=0;                                                \
   else{                                                       \
    yb=(B*)yi; zb=(B*)zz;                                      \
                   DO(r, d=*yb++; *zb++=bpfx(c,d););           \
    c=*a; k=SZI-r; DO(k, d=*yb++; *zb++=bpfx(c,d););           \
    ++yi; ++zz;                                                \
 }}}

#define BPFX(f,pfx,bpfx,pfyx,bpfyx)  \
 AHDR2(f,B,B,B){B c,d;I dd,q,r,t,u,v,*xx,*yy,*zz;       \
  t=1==n?m:n; q=t/SZI; r=t%SZI;                         \
  xx=(I*)x; yy=(I*)y; zz=(I*)z;                         \
  if(1==n){                                             \
   DO(q, u=*xx++; v=*yy++; *zz++=pfx(u,v););            \
   if(r){u=*xx++; v=*yy++; dd=pfx(u,v); MC(zz,&dd,r);}  \
  }else if(t<SZI){                                      \
   if(b)DO(m, c=*x++; DO(n, v=*y++; *z++=bpfx(c,v);))   \
   else DO(m, d=*y++; DO(n, u=*x++; *z++=bpfx(u,d);));  \
  }else if(b)BFSUB(x,yy,pfx, bpfx)                      \
  else       BFSUB(y,xx,pfyx,bpfyx)                     \
 }
