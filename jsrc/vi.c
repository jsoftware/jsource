/* Copyright 1990-2014, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Index-of                                                         */

#include "j.h"

// Table of hash-table sizes
// These are primes (to reduce collisions), and big enough to just fit into a power-of-2
// block after leaving 2 words for memory header, AH words for A-block header, 1 for rank (not used for symbol tables),
// and SYMLINFOSIZE for the unused first word, which (for symbol tables) holds general table info.
// symbol tables allocate ptab[]+SYMLINFOSIZE entries, leaving ptab[] entries for symbols.  i.-family
// operations use tables of size ptab[].  i.-family operations have rank 1.  So the prime in the table
// must be no more than (power-of-2)-10.
 // If AH changes, these numbers need to be revisited
// The first row of small values was added to allow for small symbol tables to hold local
// variables.  Hardwired references to ptab in the code have 3 added so that they correspond to
// the correct values.  User locale sizes also refer to the original values, and have 3 added before use.
I ptab[]={
        3,         5,        19,
       53,       113,       241,       499,     1013, 
     2029,      4079,      8179,     16369,    32749, 
    65521,    131059,    262133,    524269,  1048559, 
  2097133,   4194287,   8388593,  16777199, 33554393, 
 67108837, 134217689, 268435399, 536870879
};

I nptab=sizeof(ptab)/SZI;

// The bucket-size table[i][j] gives the hash-bucket number of argument-name j when the symbol table was
// created with size i.  The argument names supported are ynam and xnam.
I yxbuckets[sizeof(ptab)/SZI][2];

// Emulate bucket calculation for local names.
void bucketinit(){I j;
 for(j=0;j<nptab;++j){
  yxbuckets[j][0]=SYMHASH(NAV(ynam)->hash,ptab[j]);
  yxbuckets[j][1]=SYMHASH(NAV(xnam)->hash,ptab[j]);
 }
}

/* Floating point (type D) byte order:               */
/* Archimedes              3 2 1 0 7 6 5 4           */
/* VAX                     1 0 3 2 5 4 7 6           */
/* little endian           7 6 5 4 3 2 1 0           */
/* ThinkC MAC universal    0 1 0 1 2 3 4 5 6 7 8 9   */
/* ThinkC MAC 6888x        0 1 _ _ 2 3 4 5 6 7 8 9   */
/* normal                  0 1 2 3 4 5 6 7 ...       */

#if C_LE
#define MSW 1   /* most  significant word */
#define LSW 0   /* least significant word */
#else
#define MSW 0
#define LSW 1
#endif

static void ctmask(J jt){DI p,x,y;UINT c,d,e,m,q;
 p.d=PI;                      /* pi itself                */
 x.d=PI*(1-jt->ct);           /* lower bound              */
 y.d=PI/(1-jt->ct);           /* upper bound              */
 c=p.i[MSW]; d=p.i[LSW]; m=0;
 if(c==x.i[MSW]){e=x.i[LSW]; m =(d&~e)|(~d&e);}
 if(c==y.i[MSW]){e=y.i[LSW]; m|=(d&~e)|(~d&e);}
 q=m;
 while(m){m>>=1; q|=m;}       /* q=:+./\m as a bit vector */
 jt->ctmask=~(UI)q;
}    /* 1 iff significant wrt comparison tolerance */


#if C_HASH
/* hic:  hash a string of length k                                 */
/* hicx: hash the bytes of string v indicated by hin/hiv           */
/* hic2: hash the bytes of a string of length k (k even)           */
/* hic4: hash the bytes of a string of length k (k multiple of 4)  */
/* hicw: hash a word (32 bit or 64 bit depending on CPU)           */

       UI hic (     I k,UC*v){UI z=HASH0;             DO(k,       z=(149*i+1000003)**v++   ^z<<1;      ); R z;}

static UI hicnz(    I k,UC*v){UI z=HASH0;UC c;        DO(k, c=*v++; if(c&&c!=255)z=(149*i+1000003)*c^z<<1;); R z;}

static UI hicx(J jt,I k,UC*v){UI z=HASH0;I*u=jt->hiv; DO(jt->hin, z=(149*i+1000003)*v[*u++]^z<<1;      ); R z;}

#if C_LE
       UI hic2(     I k,UC*v){UI z=HASH0;             DO(k/2,     z=(149*i+1000003)**v     ^z<<1;
                                                       if(*(v+1)){z=(149*i+1000003)**(v+1) ^z<<1;} v+=2;); R z;}
#else
       UI hic2(     I k,UC*v){UI z=HASH0; ++v;        DO(k/2,     z=(149*i+1000003)**v     ^z<<1;
                                                       if(*(v-1)){z=(149*i+1000003)**(v-1) ^z<<1;} v+=2;); R z;}
#endif

#if C_LE
       UI hic4(     I k,UC*v){UI z=HASH0;             DO(k/4,     z=(149*i+1000003)**v     ^z<<1;
                                               if(*(v+2)||*(v+3)){z=(149*i+1000003)**(v+1) ^z<<1;
                                                                  z=(149*i+1000003)**(v+2) ^z<<1;
                                                                  z=(149*i+1000003)**(v+3) ^z<<1;}
                                                  else if(*(v+1)){z=(149*i+1000003)**(v+1) ^z<<1;} v+=4;); R z;}
#else
       UI hic4(     I k,UC*v){UI z=HASH0; v+=3;       DO(k/4,     z=(149*i+1000003)**v     ^z<<1;
                                               if(*(v-2)||*(v-3)){z=(149*i+1000003)**(v-1) ^z<<1;
                                                                  z=(149*i+1000003)**(v-2) ^z<<1;
                                                                  z=(149*i+1000003)**(v-3) ^z<<1;}
                                                  else if(*(v-1)){z=(149*i+1000003)**(v-1) ^z<<1;} v+=4;); R z;}
#endif

#else
/* hic:  hash a string of length k                                 */
/* hicx: hash the bytes of string v indicated by hin/hiv           */
/* hic2: hash the low order bytes of a string of length k (k even) */
/* hic4: hash the low order bytes of a string of length k (k multiple of 4) */
/* hicw: hash a word (32 bit or 64 bit depending on CPU)           */

       UI hic (     I k,UC*v){UI z=0;             DO(k,       z=(i+1000003)**v++   ^z<<1;      ); R z;}

static UI hicnz(    I k,UC*v){UI z=0;UC c;        DO(k, c=*v++; if(c&&c!=255)z=(i+1000003)*c^z<<1;); R z;}

static UI hicx(J jt,I k,UC*v){UI z=0;I*u=jt->hiv; DO(jt->hin, z=(i+1000003)*v[*u++]^z<<1;      ); R z;}

#if C_LE
       UI hic2(     I k,UC*v){UI z=0;             DO(k/2,     z=(i+1000003)**v     ^z<<1; v+=2;); R z;}
#else
       UI hic2(     I k,UC*v){UI z=0; ++v;        DO(k/2,     z=(i+1000003)**v     ^z<<1; v+=2;); R z;}
#endif

#if C_LE
       UI hic4(     I k,UC*v){UI z=0;             DO(k/4,     z=(i+1000003)**v     ^z<<1; v+=4;); R z;}
#else
       UI hic4(     I k,UC*v){UI z=0; v+=3;       DO(k/4,     z=(i+1000003)**v     ^z<<1; v+=4;); R z;}
#endif

#endif

#if SY_64
#define hicw(v)  (10495464745870458733U**(UI*)(v))
static UI jthid(J jt,D d){R 10495464745870458733U*(jt->ctmask&*(I*)&d);}
#else
#define hicw(v)  (2838338383U**(U*)(v))
static UI jthid(J jt,D d){DI x; x.d=d; R 888888883U*(x.i[LSW]&jt->ctmask)+2838338383U*x.i[MSW];}
#endif


static UI jthia(J jt,D hct,A y){UC*yv;D d;I n,t;Q*u;
 n=AN(y); t=AT(y); yv=UAV(y);
 if(!n||t&BOX)R hic(AR(y)*SZI,(UC*)AS(y));
 switch(CTTZ(t)){
  case LITX:  R hic(n,yv);
  case C2TX:  R hic2(2*n,yv);
  case C4TX:  R hic4(4*n,yv);
  case SBTX:  R hic(n*SZI,yv);
  case B01X:  d=*(B*)yv; break;
  case INTX:  d=(D)*(I*)yv; break;
  case FLX: 
  case CMPXX: d=*(D*)yv; break;
  case XNUMX: d=xdouble(*(X*)yv); break;
  case RATX:  u=(Q*)yv; d=xdouble(u->n)/xdouble(u->d);
 }
 R hid(d*hct);
}

static UI jthiau(J jt,A y){I m,n;UC*v=UAV(y);UI z=2038074751;X*u,x;
 m=n=AN(y);
 if(!n)R 0;
 switch(CTTZ(AT(y))){
  case RATX:  m+=n;  /* fall thru */
  case XNUMX: u=XAV(y); DO(m, x=*u++; v=UAV(x); z+=hicnz(AN(x)*SZI,UAV(x));); R z;
  case INTX:                                    z =hicnz(n    *SZI,UAV(y));   R z;
  default:   R hic(n*bp(AT(y)),UAV(y));
}}

static UI hix(X*v){A y=*v;   R hic(AN(y)*SZI,UAV(y));}
static UI hiq(Q*v){A y=v->n; R hic(AN(y)*SZI,UAV(y));}


static B jteqx(J jt,I n,X*u,X*v){DO(n, if(!equ(*u,*v))R 0; ++u; ++v;); R 1;}
static B jteqq(J jt,I n,Q*u,Q*v){DO(n, if(!QEQ(*u,*v))R 0; ++u; ++v;); R 1;}
static B jteqd(J jt,I n,D*u,D*v){DO(n, if(!teq(*u,*v))R 0; ++u; ++v;); R 1;}
static B jteqz(J jt,I n,Z*u,Z*v){DO(n, if(!zeq(*u,*v))R 0; ++u; ++v;); R 1;}

static B jteqa(J jt,I n,A*u,A*v,I c,I d){DO(n, if(!equ(AADR(c,*u),AADR(d,*v)))R 0; ++u; ++v;); R 1;}

/*
 mode one of the following:
       0  IIDOT      i.      a w rank
       1  IICO       i:      a w rank
       2  INUBSV     ~:        w rank
       3  INUB       ~.        w
       4  ILESS      -.      a w
       5  INUBI      I.@~:     w
       6  IEPS       e.      a w rank
       7  II0EPS     e.i.0:  a w   
       8  II1EPS     e.i.1:  a w
       9  IJ0EPS     e.i:0:  a w  
       10 IJ1EPS     e.i:1:  a w
       11 ISUMEPS    [:+ /e. a w     
       12 IANYEPS    [:+./e. a w    
       13 IALLEPS    [:*./e. a w    
       14 IIFBEPS    I.@e.   a w    
       30 IPHIDOT    i.      a w     prehashed
       31 IPHICO     i:      a w     prehashed
       34 IPHLESS    -.      a w     prehashed
       36 IPHEPS     e.      a w     prehashed
       37 IPHI0EPS   e.i.0:  a w     prehashed
       38 IPHI1EPS   e.i.1:  a w     prehashed
       39 IPHJ0EPS   e.i:0:  a w     prehashed
       40 IPHJ1EPS   e.i:1:  a w     prehashed
       41 IPHSUMEPS  [:+ /e. a w     prehashed
       42 IPHANYEPS  [:+./e. a w     prehashed
       43 IPHALLEPS  [:*./e. a w     prehashed
       44 IPHIFBEPS  I.@e.   a w     prehashed
 m    target axis length
 n    target item # elements
 c    # target items in a right arg cell
 k    target item # bytes
 acr  left  rank
 wcr  right rank
 ac   # left  arg cells
 wc   # right arg cells
 ak   # bytes left  arg cells, or 0 if only one cell
 wk   # bytes right arg cells, or 0 if only one cell
 a    left  arg
 w    right arg, or mark for m&i. or m&i: or e.&n or -.&n
 hp   pointer to hash table or to 0
 z    result
*/

#define IOF(f)     A f(J jt,I mode,I m,I n,I c,I k,I acr,I wcr,I ac,I wc,I ak,I wk,A a,A w,A*hp,A z)
#define IODECL(T)  A h=*hp;B*zb;C*zc;D t1=1.0;I t=sizeof(T),acn=ak/t,cn=k/t,hj,*hv=AV(h),j,l,p=AN(h),  \
                     wcn=wk/t,*zi,*zv=AV(z);T*av=(T*)AV(a),*v,*wv=(T*)AV(w);UI pm=p
#define FIND(exp)  while(m>(hj=hv[j])&&(exp)){++j; if(j==p)j=0;}
#define RDECL      I ad=(I)a*ARELATIVE(a),wd=(I)w*ARELATIVE(w)
#define ZISHAPE    *AS(z)=AN(z)=zi-zv
#define ZCSHAPE    *AS(z)=(zc-(C*)zv)/k; AN(z)=n**AS(z)
#define ZUSHAPE(T) *AS(z)= zu-(T*)zv;    AN(z)=n**AS(z)


#define XDOA(hash,exp,inc)         {d=ad; v=av;          DO(m,  j=(hash)%pm; FIND(exp); if(m==hj)hv[j]=i; inc;);}
#define XDQA(hash,exp,dec)         {d=ad; v=av+cn*(m-1); DQ(m,  j=(hash)%pm; FIND(exp); if(m==hj)hv[j]=i; dec;);}
#define XDO(hash,exp,inc,stmt)     {d=wd; v=wv;          DO(cm, j=(hash)%pm; FIND(exp); stmt;             inc;);}
#define XDQ(hash,exp,dec,stmt)     {d=wd; v=wv+cn*(c-1); DQ(cm, j=(hash)%pm; FIND(exp); stmt;             dec;);}

#define XMV(hash,exp,inc,stmt)      \
 if(k==SZI){XDO(hash,exp,inc,if(m==hj){*zi++=*(I*)v;      stmt;}); zc=(C*)zi;}  \
 else       XDO(hash,exp,inc,if(m==hj){MC(zc,v,k); zc+=k; stmt;});

#define IOFX(T,f,hash,exp,inc,dec)   \
 IOF(f){RDECL;IODECL(T);B b;I cm,d,md,s;UC*u=0;                                      \
  md=mode<IPHOFFSET?mode:mode-IPHOFFSET;                                             \
  b=a==w&&ac==wc&&(mode==IIDOT||mode==IICO||mode==INUBSV||mode==INUB||mode==INUBI);  \
  zb=(B*)zv; zc=(C*)zv; zi=zv; cm=w==mark?0:c;                                       \
  for(l=0;l<ac;++l,av+=acn,wv+=wcn){                                                 \
   if(mode<IPHOFFSET){DO(p,hv[i]=m;); if(!b){if(mode==IICO)XDQA(hash,exp,dec) else XDOA(hash,exp,inc);}}  \
   switch(md){                                                                       \
    case IIDOT:   if(b){          XDO(hash,exp,inc,*zv++=m==hj?(hv[j]=i):hj);}       \
                  else XDO(hash,exp,inc,*zv++=hj);                           break;  \
    case IICO:    if(b){zi=zv+=c; XDQ(hash,exp,dec,*--zi=m==hj?(hv[j]=i):hj);}       \
                  else XDO(hash,exp,inc,*zv++=hj);                           break;  \
    case INUBSV:       XDO(hash,exp,inc,*zb++=m==hj?(hv[j]=i,1):0);          break;  \
    case INUB:         XMV(hash,exp,inc,hv[j]=i);                ZCSHAPE;    break;  \
    case ILESS:        XMV(hash,exp,inc,0      );                ZCSHAPE;    break;  \
    case INUBI:        XDO(hash,exp,inc,if(m==hj)*zi++=hv[j]=i); ZISHAPE;    break;  \
    case IEPS:         XDO(hash,exp,inc,*zb++=m>hj);                         break;  \
    case II0EPS:  s=c; XDO(hash,exp,inc,if(m==hj){s=i; break;}); *zi++=s;    break;  \
    case II1EPS:  s=c; XDO(hash,exp,inc,if(m> hj){s=i; break;}); *zi++=s;    break;  \
    case IJ0EPS:  s=c; XDQ(hash,exp,dec,if(m==hj){s=i; break;}); *zi++=s;    break;  \
    case IJ1EPS:  s=c; XDQ(hash,exp,dec,if(m> hj){s=i; break;}); *zi++=s;    break;  \
    case ISUMEPS: s=0; XDO(hash,exp,inc,if(m> hj)++s;         ); *zi++=s;    break;  \
    case IANYEPS: s=0; XDO(hash,exp,inc,if(m> hj){s=1; break;}); *zb++=1&&s; break;  \
    case IALLEPS: s=1; XDO(hash,exp,inc,if(m==hj){s=0; break;}); *zb++=1&&s; break;  \
    case IIFBEPS: s=c; XDO(hash,exp,inc,if(m> hj)*zi++=i      ); ZISHAPE;    break;  \
  }}                                                                                 \
  R z;                                                                               \
 }

static IOFX(A,jtioax1,hia(t1,AADR(d,*v)),!equ(AADR(d,*v),AADR(ad,av[hj])),++v,   --v  )  /* boxed exact 1-element item */   
static IOFX(A,jtioau, hiau(AADR(d,*v)),  !equ(AADR(d,*v),AADR(ad,av[hj])),++v,   --v  )  /* boxed uniform type         */
static IOFX(X,jtiox,  hix(v),            !eqx(n,v,av+n*hj),               v+=cn, v-=cn)  /* extended integer           */   
static IOFX(Q,jtioq,  hiq(v),            !eqq(n,v,av+n*hj),               v+=cn, v-=cn)  /* rational number            */   
static IOFX(C,jtioc,  hic(k,(UC*)v),     memcmp(v,av+k*hj,k),             v+=cn, v-=cn)  /* boolean, char, or integer  */
static IOFX(C,jtiocx, hicx(jt,k,(UC*)v), memcmp(v,av+k*hj,k),             v+=cn, v-=cn)  /* boolean, char, or integer  */
static IOFX(I,jtioi,  hicw(v),           *v!=av[hj],                      ++v,   --v  )


#define HID(y)              (888888883U*y.i[LSW]+2838338383U*y.i[MSW])
#define MASK(dd,xx)         {dd.d=xx; dd.i[LSW]&=jt->ctmask;}
#define THASHA(expa)        {x=*(D*)v; MASK(dx,x); j=HID(dx)%pm; FIND(expa); if(m==hj)hv[j]=i;}
#define THASHBX(expa)       {j=hia(t1,AADR(d,*v))%pm;            FIND(expa); if(m==hj)hv[j]=i;}

#define TFINDXY(expa,expw)  \
 {x=*(D*)v;                                                                             \
  MASK(dl,x*tl);                j=              HID(dl)%pm; FIND(expw); il=ir=hj;       \
  MASK(dr,x*tr); if(dr.d!=dl.d){j=              HID(dr)%pm; FIND(expw);    ir=hj;}      \
 } 
#define TFINDYY(expa,expw)  \
 {x=*(D*)v;                                                                             \
  MASK(dx,x   ); j=jx=HID(dx)%pm; jt->ct=0.0; FIND(expa); jt->ct=ct; if(m==hj)hv[j]=i;  \
  MASK(dl,x*tl);                j=dl.d==dx.d?jx:HID(dl)%pm; FIND(expw); il=ir=hj;       \
  MASK(dr,x*tr); if(dr.d!=dl.d){j=dr.d==dx.d?jx:HID(dr)%pm; FIND(expw);    ir=hj;}      \
 }
#define TFINDY1(expa,expw)  \
 {x=*(D*)v;                                                                             \
  MASK(dx,x   ); j=jx=HID(dx)%pm;             FIND(expa);            if(m==hj)hv[j]=i;  \
  MASK(dl,x*tl);                j=dl.d==dx.d?jx:HID(dl)%pm; FIND(expw); il=ir=hj;       \
  MASK(dr,x*tr); if(dr.d!=dl.d){j=dr.d==dx.d?jx:HID(dr)%pm; FIND(expw);    ir=hj;}      \
 }
#define TFINDBX(expa,expw)   \
 {jx=j=hia(tl,AADR(d,*v))%pm;           FIND(expw); il=ir=hj;   \
     j=hia(tr,AADR(d,*v))%pm; if(j!=jx){FIND(expw);    ir=hj;}  \
 }
#define TDO(FXY,FYY,expa,expw,stmt)  \
 switch(4*bx+2*b+(k==sizeof(D))){                       \
  default: DO(c, FXY(expa,expw); stmt; v+=cn;); break;  \
  case 1:  DO(c, FXY(expa,expw); stmt; ++v;  ); break;  \
  case 2:  DO(c, FYY(expa,expw); stmt; v+=cn;); break;  \
  case 3:  DO(c, FYY(expa,expw); stmt; ++v;  );         \
 }
#define TDQ(FXY,FYY,expa,expw,stmt)  \
 v+=cn*(c-1);                                           \
 switch(4*bx+2*b+(k==sizeof(D))){                       \
  default: DQ(c, FXY(expa,expw); stmt; v-=cn;); break;  \
  case 1:  DQ(c, FXY(expa,expw); stmt; --v;  ); break;  \
  case 2:  DQ(c, FYY(expa,expw); stmt; v-=cn;); break;  \
  case 3:  DQ(c, FYY(expa,expw); stmt; --v;  );         \
 }
#define TMV(FXY,FYY,expa,expw,prop)   \
 switch(4*bx+2*b+(k==sizeof(D))){                                  \
  default: DO(c, FXY(expa,expw); if(prop){MC(zc,v,k); zc+=k;}; v+=cn;);            break;  \
  case 1:  DO(c, FXY(expa,expw); if(prop)*zd++=*(D*)v;         ++v;  ); zc=(C*)zd; break;  \
  case 2:  DO(c, FYY(expa,expw); if(prop){MC(zc,v,k); zc+=k;}; v+=cn;);            break;  \
  case 3:  DO(c, FYY(expa,expw); if(prop)*zd++=*(D*)v;         ++v;  ); zc=(C*)zd;         \
 }


#define IOFT(T,f,FA,FXY,FYY,expa,expw)   \
 IOF(f){RDECL;IODECL(T);B b,bx;D ct=jt->ct,tl=1-jt->ct,tr=1/tl,x,*zd;DI dl,dr,dx;I d,e,il,ir,jx,md,s;  \
  md=mode<IPHOFFSET?mode:mode-IPHOFFSET;                                                         \
  b=a==w&&ac==wc&&(mode==IIDOT||mode==IICO||mode==INUBSV||mode==INUB||mode==INUBI);              \
  zb=(B*)zv; zc=(C*)zv; zd=(D*)zv; zi=zv; e=cn*(m-1); bx=1&&BOX&AT(a);                           \
  jx=0; dl.d=dr.d=dx.d=x=0.0;                                                                    \
  for(l=0;l<ac;++l,av+=acn,wv+=wcn){                                                             \
   if(mode<IPHOFFSET){                                                                           \
    DO(p,hv[i]=m;);                                                                              \
    if(bx||!b){                                                                                  \
     d=ad; v=av; jt->ct=0.0;                                                                     \
     if(IICO==mode){v+=e; DQ(m, FA(expa); v-=cn;);}else DO(m, FA(expa); v+=cn;);                 \
     jt->ct=ct; if(w==mark)break;                                                                \
   }}                                                                                            \
   d=wd; v=wv;                                                                                   \
   switch(md){                                                                                   \
    case IIDOT:        TDO(FXY,FYY,expa,expw,*zv++=MIN(il,ir));                          break;  \
    case IICO:  zv+=c; TDQ(FXY,FYY,expa,expw,*--zv=m==il?ir:m==ir?il:MAX(il,ir)); zv+=c; break;  \
    case INUBSV:       TDO(FXY,FYY,expa,expw,*zb++=i==MIN(il,ir));                       break;  \
    case INUB:         TMV(FXY,FYY,expa,expw,i==MIN(il,ir));                 ZCSHAPE;    break;  \
    case ILESS:        TMV(FXY,FYY,expa,expw,m==il&&m==ir);                  ZCSHAPE;    break;  \
    case INUBI:        TDO(FXY,FYY,expa,expw,if(i==MIN(il,ir))*zi++=i;);     ZISHAPE;    break;  \
    case IEPS:         TDO(FXY,FYY,expa,expw,*zb++=m>il||m>ir             );             break;  \
    case II0EPS:  s=c; TDO(FXY,FYY,expa,expw,if(m==il&&m==ir){s=i; break;}); *zi++=s;    break;  \
    case II1EPS:  s=c; TDO(FXY,FYY,expa,expw,if(m> il||m> ir){s=i; break;}); *zi++=s;    break;  \
    case IJ0EPS:  s=c; TDQ(FXY,FYY,expa,expw,if(m==il&&m==ir){s=i; break;}); *zi++=s;    break;  \
    case IJ1EPS:  s=c; TDQ(FXY,FYY,expa,expw,if(m> il||m> ir){s=i; break;}); *zi++=s;    break;  \
    case ISUMEPS: s=0; TDO(FXY,FYY,expa,expw,if(m> il||m> ir)++s          ); *zi++=s;    break;  \
    case IANYEPS: s=0; TDO(FXY,FYY,expa,expw,if(m> il||m> ir){s=1; break;}); *zb++=1&&s; break;  \
    case IALLEPS: s=1; TDO(FXY,FYY,expa,expw,if(m==il&&m==ir){s=0; break;}); *zb++=1&&s; break;  \
    case IIFBEPS:      TDO(FXY,FYY,expa,expw,if(m> il||m> ir)*zi++=i      ); ZISHAPE;    break;  \
  }}                                                                                             \
  R z;                                                                                           \
 }

static IOFT(Z,jtioz, THASHA, TFINDXY,TFINDYY,memcmp(v,av+n*hj,n*2*sizeof(D)), !eqz(n,v,av+n*hj)               )
static IOFT(Z,jtioz1,THASHA, TFINDXY,TFINDYY,memcmp(v,av+n*hj,  2*sizeof(D)), !zeq( *v,av[hj] )               )
static IOFT(D,jtiod, THASHA, TFINDXY,TFINDYY,memcmp(v,av+n*hj,n*  sizeof(D)), !eqd(n,v,av+n*hj)               )
static IOFT(D,jtiod1,THASHA, TFINDXY,TFINDY1,x!=av[hj],                       !teq(x,av[hj] )                 )
static IOFT(A,jtioa, THASHBX,TFINDBX,TFINDBX,!eqa(n,v,av+n*hj,d,ad),          !eqa(n,v,av+n*hj,d,ad)          )
static IOFT(A,jtioa1,THASHBX,TFINDBX,TFINDBX,!equ(AADR(d,*v),AADR(ad,av[hj])),!equ(AADR(d,*v),AADR(ad,av[hj])))


#define SDO(v0)         if(mode<IPHOFFSET){B v1=!(v0); memset(hv,v0,p); u=av; DO(m, hb[*u++]=v1;);}
#define SDOA            if(mode<IPHOFFSET){DO(p,hv[i]=m; ); u=av;   DO(m,hu[*u++]=i;);}
#define SDQA            if(mode<IPHOFFSET){DO(p,hv[i]=m; ); u=av+m; DQ(m,hu[*--u]=i;);}

#define SDOW(stmt)      {u=wv;   DO(cm, stmt;);}

#define SCOZ(hh,zz,vv)  {u=wv;   DO(cm, zz=hh[*u++];                      );}
#define SCOZ1(hh,zz,vv) {u=wv;   DO(cm, x=*u++; zz=min<=x&&x<max?hh[x]:vv;);}

#define SCOW(hh,stmt)   {u=wv;   DO(cm,         if(                hh[  *u++]){stmt;});}
#define SCOWX(hh,stmt)  {u=wv;   DO(cm,         if(                hh[x=*u++]){stmt;});}
#define SCOW1(hh,stmt)  {u=wv;   DO(cm, x=*u++; if(min<=x&&x< max&&hh[x     ]){stmt;});}
#define SCOW0(hh,stmt)  {u=wv;   DO(cm, x=*u++; if(x<min ||max<=x||hh[x     ]){stmt;});}

#define SCQW(hh,stmt)   {u=wv+c; DQ(cm,         if(                hh[  *--u]){stmt;});}
#define SCQW1(hh,stmt)  {u=wv+c; DQ(cm, x=*--u; if(min<=x&&x< max&&hh[x     ]){stmt;});}
#define SCQW0(hh,stmt)  {u=wv+c; DQ(cm, x=*--u; if(x<min ||max<=x||hh[x     ]){stmt;});}

#define IOFSMALLRANGE(f,T,COZ1,COW0,COW1,COWX,CQW0,CQW1)    \
 IOF(f){A h=*hp;B b,*hb,*zb;I cm,e,*hu,*hv,l,max,md,min,p,s,*v,*zi,*zv;T*av,*u,*wv,x,*zu;       \
  md=mode<IPHOFFSET?mode:mode-IPHOFFSET; b=(mode==IIDOT||mode==IICO)&&a==w&&ac==wc;             \
  av=(T*)AV(a); wv=(T*)AV(w); zv=zi=AV(z); zb=(B*)zv; zu=(T*)zv;                                \
  p=AN(h); min=jt->min; max=p+min; hv=AV(h); hb=(B*)hv-min; hu=hv-min;                          \
  e=1==wc?0:c; cm=w==mark?0:c;                                                                  \
  for(l=0;l<ac;++l,av+=m,wv+=e){                                                                \
   if(b){                                                                                       \
    if(mode==IIDOT){DO(p,hv[i]=-1;); u=wv;          DO(m, v=hu+*u++; if(0>*v)*v=i; *zv++=*v;);       }  \
    else           {DO(p,hv[i]=-1;); u=wv+m; zv+=m; DQ(m, v=hu+*--u; if(0>*v)*v=i; *--zv=*v;); zv+=m;}  \
   }else switch(md){                                                                            \
    case INUBSV:  memset(hv,C1,p); SDOW(if(*zb++=hb[x=*u++])hb[x]=0);                   break;  \
    case INUB:    memset(hv,C1,p); SDOW(if(hb[x=*u++]){*zu++=x; hb[x]=0;}); ZUSHAPE(T); break;  \
    case INUBI:   memset(hv,C1,p); SDOW(if(hb[x=*u++]){*zi++=i; hb[x]=0;}); ZISHAPE;    break;  \
    case IIDOT:   SDQA;         COZ1(hu, *zv++, m);                                     break;  \
    case IICO:    SDOA;         COZ1(hu, *zv++, m);                                     break;  \
    case IEPS:    SDO(C0);      COZ1(hb, *zb++, 0);                                     break;  \
    case ILESS:   SDO(C1);      COWX(hb, *zu++=x   );                       ZUSHAPE(T); break;  \
    case II0EPS:  SDO(C1); s=c; COW0(hb, s=i; break); *zv++=s;                          break;  \
    case II1EPS:  SDO(C0); s=c; COW1(hb, s=i; break); *zv++=s;                          break;  \
    case IJ0EPS:  SDO(C1); s=c; CQW0(hb, s=i; break); *zv++=s;                          break;  \
    case IJ1EPS:  SDO(C0); s=c; CQW1(hb, s=i; break); *zv++=s;                          break;  \
    case IANYEPS: SDO(C0); s=0; COW1(hb, s=1; break); *zb++=1&&s;                       break;  \
    case IALLEPS: SDO(C1); s=1; COW0(hb, s=0; break); *zb++=1&&s;                       break;  \
    case ISUMEPS: SDO(C0); s=0; COW1(hb, ++s       ); *zv++=s;                          break;  \
    case IIFBEPS: SDO(C0);      COW1(hb, *zi++=i   );                       ZISHAPE;    break;  \
  }}                                                                                            \
  R z;                                                                                          \
 }

static IOFSMALLRANGE(jtio1,UC,SCOZ, SCOW, SCOW, SCOWX,SCQW, SCQW )  /* 1-byte    items */
static IOFSMALLRANGE(jtio2,US,SCOZ, SCOW, SCOW, SCOWX,SCQW, SCQW )  /* 2-byte    items */
static IOFSMALLRANGE(jtio4,I ,SCOZ1,SCOW0,SCOW1,SCOW0,SCQW0,SCQW1)  /* word size items */


#define SCDO(T,xe,exp)  \
 {T*av=(T*)u,*v0=(T*)v,*wv=(T*)v,x; \
  switch(mode){                     \
   case IIDOT: DO(ac, DO(c, x=(xe); j=0;   while(m>j &&(exp))++j; *zv++=j;       wv+=q;); av+=p; if(1==wc)wv=v0;); break;  \
   case IICO:  DO(ac, DO(c, x=(xe); j=m-1; while(0<=j&&(exp))--j; *zv++=0>j?m:j; wv+=q;); av+=p; if(1==wc)wv=v0;); break;  \
   case IEPS:  DO(ac, DO(c, x=(xe); j=0;   while(m>j &&(exp))++j; *zb++=m>j;     wv+=q;); av+=p; if(1==wc)wv=v0;); break;  \
 }}

static IOF(jtiosc){B*zb;I j,p,q,*u,*v,zn,*zv;
 p=1<ac?m:0; q=1<wc||1<c;
 zn=AN(z); 
 zv=AV(z); zb=(B*)zv; u=AV(a); v=AV(w); 
 switch(CTTZ(AT(a))){
  default:                SCDO(C, *wv,x!=av[j]      ); break;
  case C2TX:               SCDO(S, *wv,x!=av[j]      ); break;
  case C4TX:               SCDO(C4,*wv,x!=av[j]      ); break;
  case CMPXX:              SCDO(Z, *wv,!zeq(x, av[j])); break;
  case XNUMX:              SCDO(A, *wv,!equ(x, av[j])); break;
  case RATX:               SCDO(Q, *wv,!QEQ(x, av[j])); break;
  case INTX:               SCDO(I, *wv,x!=av[j]      ); break;
  case SBTX:               SCDO(SB,*wv,x!=av[j]      ); break;
  case BOXX:  {RDECL;      SCDO(A, AADR(wd,*wv),!equ(x,AADR(ad,av[j])));} break;
  case FLX:   if(0==jt->ct)SCDO(D, *wv,x!=av[j]) 
             else         SCDO(D, *wv,!teq(x,av[j]));
 }
 R z;
}    /* right argument cell is scalar; only for modes IIDOT IICO IEPS */


static B jtusebs(J jt,A a,I ac,I m){A*av,x;I ad,t;
 if(!(BOX&AT(a)&&0==jt->ct))R 0;
 av=AAV(a); ad=(I)a*ARELATIVE(a);
 DO(ac*m, x=AVR(i); t=AT(x); if(t&BOX+CMPX||1<AN(x)&&t&NUMERIC)R 1;);
 R 0;
}    /* n (# elements in a target item) is assumed to be 1 */

static A jtnodupgrade(J jt,A a,I acr,I ac,I acn,I ad,I n,I m,B b,B bk){A*av,h,*u,*v;I*hi,*hu,*hv,l,m1,q;
 RZ(h=irs1(a,0L,acr,jtgrade1)); hv=AV(h)+bk*(m-1); av=AAV(a);
 if(!b)for(l=0;l<ac;++l,av+=acn,hv+=m){
  hi=hv; q=*hi; u=av+n*q;
  if(bk){hu=--hi; DO(m-1, q=*hi--; v=av+n*q; if(!eqa(n,u,v,ad,ad)){u=v; *hu--=q;}); m1=hv-hu; if(m>m1)hv[1-m]=1-m1;}
  else  {hu=++hi; DO(m-1, q=*hi++; v=av+n*q; if(!eqa(n,u,v,ad,ad)){u=v; *hu++=q;}); m1=hu-hv; if(m>m1)hv[m-1]=1-m1;}
 }
 R h;
} 

#define BSLOOPAA(hiinc,zstmti,zstmt1,zstmt0)  \
 {A*u=av,*v;I*hi=hv,p,q;             \
  p=*hiinc; u=av+n*p; zstmti;        \
  DO(m-1, q=*hiinc; v=av+n*q; if(eqa(n,u,v,ad,ad))zstmt1; else{u=v; zstmt0;});  \
 }

#define BSLOOPAWX(ii,icmp,iinc,uinc,zstmt)  \
 {A*u=wv+n*(ii),*v;I i,j,p,q;int t;  \
  for(i=ii;icmp;iinc,uinc){          \
   p=0; q=m1;                        \
   while(p<=q){                      \
    t=0; j=(p+q)/2; v=av+n*hu[j];    \
    DO(n, if(t=compare(AADR(wd,u[i]),AADR(ad,v[i])))break;);  \
    if(0<t)p=j+1; else q=t?j-1:-2;   \
   }                                 \
   zstmt;                            \
 }}

#define BSLOOPAW(zstmt)     BSLOOPAWX(0  ,i< c,++i,u+=n,zstmt)
#define BSLOOQAW(zstmt)     BSLOOPAWX(c-1,i>=0,--i,u-=n,zstmt)

static IOF(jtiobs){A*av,h=*hp,*wv,y;B b,bk,*yb,*zb;C*zc;I acn,ad,*hu,*hv,l,m1,md,s,wcn,wd,*zi,*zv;
 bk=mode==IICO||mode==IJ0EPS||mode==IJ1EPS||mode==IPHICO||mode==IPHJ0EPS||mode==IPHJ1EPS;
 b=a==w&&ac==wc&&(mode==IIDOT||mode==IICO||mode==INUB||mode==INUBSV||mode==INUBI); 
 if(mode==INUB||mode==INUBI){GATV(y,B01,m,1,0); yb=BAV(y);}
 md=w==mark?-1:mode<IPHOFFSET?mode:mode-IPHOFFSET;
 av=AAV(a); ad=(I)a*ARELATIVE(a); acn=ak/sizeof(A);
 wv=AAV(w); wd=(I)w*ARELATIVE(w); wcn=wk/sizeof(A);
 zi=zv=AV(z); zb=(B*)zv; zc=(C*)zv;
 if(mode<IPHOFFSET)RZ(*hp=h=nodupgrade(a,acr,ac,acn,ad,n,m,b,bk));
 if(w==mark)R mark;
 hv=AV(h)+bk*(m-1); jt->complt=-1; jt->compgt=1;
 for(l=0;l<ac;++l,av+=acn,wv+=wcn,hv+=m){
  s=hv[bk?1-m:m-1]; m1=0>s?-s:m-1; hu=hv-m1*bk;
  if(b)switch(md){
   case IIDOT:        BSLOOPAA(hi++,zv[p]=p,zv[q]=p,zv[q]=p=q); zv+=m;     break;
   case IICO:         BSLOOPAA(hi--,zv[p]=p,zv[q]=p,zv[q]=p=q); zv+=m;     break;
   case INUBSV:       BSLOOPAA(hi++,zb[p]=1,zb[q]=0,zb[q]=1  ); zb+=m;     break;
   case INUB:         BSLOOPAA(hi++,yb[p]=1,yb[q]=0,yb[q]=1  ); DO(m, if(yb[i]){MC(zc,av+i*n,k); zc+=k;}); ZCSHAPE; break;
   case INUBI:        BSLOOPAA(hi++,yb[p]=1,yb[q]=0,yb[q]=1  ); DO(m, if(yb[i])*zi++=i;);                  ZISHAPE; break;
  }else switch(md){
   case IIDOT:        BSLOOPAW(*zv++=-2==q?hu[j]:m);                       break;
   case IICO:         BSLOOPAW(*zv++=-2==q?hu[j]:m);                       break;
   case IEPS:         BSLOOPAW(*zb++=-2==q);                               break;
   case ILESS:        BSLOOPAW(if(-2< q){MC(zc,u,k); zc+=k;}); ZCSHAPE;    break;
   case II0EPS:  s=m; BSLOOPAW(if(-2< q){s=i; break;});        *zi++=s;    break;
   case IJ0EPS:  s=m; BSLOOQAW(if(-2< q){s=i; break;});        *zi++=s;    break;
   case II1EPS:  s=m; BSLOOPAW(if(-2==q){s=i; break;});        *zi++=s;    break;
   case IJ1EPS:  s=m; BSLOOQAW(if(-2==q){s=i; break;});        *zi++=s;    break;
   case IANYEPS: s=0; BSLOOPAW(if(-2==q){s=1; break;});        *zb++=1&&s; break;
   case IALLEPS: s=1; BSLOOPAW(if(-2< q){s=0; break;});        *zb++=1&&s; break;
   case ISUMEPS: s=0; BSLOOPAW(if(-2==q)++s);                  *zi++=s;    break;
   case IIFBEPS:      BSLOOPAW(if(-2==q)*zi++=i);              ZISHAPE;    break;
 }}
 R z;
}    /* a i.!.0 w on boxed a,w by grading and binary search */

static I jtutype(J jt,A w,I c){A*wv,x;I m,t,wd;
 if(!AN(w))R 1;
 m=AN(w)/c; wv=AAV(w); wd=(I)w*ARELATIVE(w);
 DO(c, t=0; DO(m, x=WVR(i); if(AN(x)){if(t)RZ(TYPESEQ(t,AT(x))) else{t=AT(x); if(t&FL+CMPX+BOX)R 0;}}););
 R t;
}    /* return type if opened atoms of cells of w has uniform type, else 0. c is # of cells */

I hsize(I m){I q=m+m,*v=ptab+PTO; DO(nptab-PTO, if(q<=*v)break; ++v;); R*v;}


A jtindexofsub(J jt,I mode,A a,A w){PROLOG(0079);A h=0,hi=mtv,z=mtv;AF fn;B mk=w==mark,th;
    I ac,acr,af,ak,ar,*as,at,c,f,f1,k,k1,m,n,p,r,*s,ss,t,wc,wcr,wf,wk,wr,*ws,wt,zn;
 RZ(a&&w);
 ar=AR(a); acr=jt->rank?jt->rank[0]:ar; af=ar-acr;
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr; jt->rank=0;
 as=AS(a); at=AT(a);
 ws=AS(w); wt=AT(w);
 if(mk){f=af; s=as; r=acr-1; f1=wcr-r;}
 else{
  f=af?af:wf; s=af?as:ws; r=acr?acr-1:0; f1=wcr-r;
  if(0>f1||ICMP(as+af+1,ws+wf+f1,r)){I f0,*v;
   m=acr?as[af]:1; f0=MAX(0,f1); RE(zn=mult(prod(f,s),prod(f0,ws+wf)));
   switch(mode){
    case IIDOT:  
    case IICO:    GATV(z,INT,zn,f+f0,s); if(af)ICPY(f+AS(z),ws+wf,f0); v=AV(z); DO(zn, *v++=m;); R z;
    case IEPS:    GATV(z,B01,zn,f+f0,s); if(af)ICPY(f+AS(z),ws+wf,f0); memset(BAV(z),C0,zn); R z;
    case ILESS:                              R ca(w);
    case IIFBEPS:                            R mtv;
    case IANYEPS: case IALLEPS: case II0EPS: R zero;
    case ISUMEPS:                            R sc(0L);
    case II1EPS:  case IJ1EPS:               R sc(zn);
    case IJ0EPS:                             R sc(zn-1);
    case INUBSV:  case INUB:    case INUBI:  ASSERTSYS(0,"indexofsub"); // impossible 
 }}}
 if(at&SPARSE||wt&SPARSE){A z;
  if(1>=acr)R af?sprank2(a,w,0L,acr,RMAX,jtindexof):wt&SPARSE?iovxs(mode,a,w):iovsd(mode,a,w);
  if(af||wf)R sprank2(a,w,0L,acr,wcr,jtindexof);
  switch((at&SPARSE?2:0)+(wt&SPARSE?1:0)){
   case 1: z=indexofxx(mode,a,w); break;
   case 2: z=indexofxx(mode,a,w); break;
   case 3: z=indexofss(mode,a,w); break;
  }
  EPILOG(z);
 }
 m=acr?as[af]:1; n=acr?prod(acr-1,as+af+1):1; RE(zn=mult(prod(f,s),prod(f1,ws+wf)));
 RE(t=mk?at:maxtype(at,wt)); k1=bp(t); k=n*k1; th=HOMO(at,wt); jt->min=ss=0;
 ac=prod(af,as); ak=ac?k1*AN(a)/ac:0;  
 wc=prod(wf,ws); wk=wc?k1*AN(w)/wc:0; c=1<ac?wk/k:zn; wk*=1<wc;
 if(th&&TYPESNE(t,at))RZ(a=t&XNUM?xcvt(XMEXMT,a):cvt(t,a)) else if(t&FL+CMPX      )RZ(a=cvt0(a));
 if(th&&TYPESNE(t,wt))RZ(w=t&XNUM?xcvt(XMEXMT,w):cvt(t,w)) else if(t&FL+CMPX&&a!=w)RZ(w=cvt0(w));
 if(AT(a)&INT+SBT&&k==SZI){I r; irange(AN(a)*k1/SZI,AV(a),&r,&ss); if(ss){jt->min=r;}}
 p=1==k?(t&B01?2:256):2==k?(t&B01?258:65536):k==SZI&&ss&&ss<2.1*MAX(m,c)?ss:hsize(m);
 if(!mk)switch(mode){I q;
  case IIDOT: 
  case IICO:    GATV(z,INT,zn,f+f1,     s); if(af)ICPY(f+AS(z),ws+wf,f1); break;
  case INUBSV:  GATV(z,B01,zn,f+f1+!acr,s); if(af)ICPY(f+AS(z),ws+wf,f1); if(!acr)*(AS(z)+AR(z)-1)=1; break;
  case INUB:    q=MIN(m,p); GA(z,t,mult(q,aii(a)),MAX(1,wr),ws); *AS(z)=q; break;
  case ILESS:   GA(z,t,AN(w),MAX(1,wr),ws); break;
  case IEPS:    GATV(z,B01,zn,f+f1,     s); if(af)ICPY(f+AS(z),ws+wf,f1); break;
  case INUBI:   q=MIN(m,p); GATV(z,INT,q,1,0); break;
  case IIFBEPS: GATV(z,INT,c,1,0); break;
  case IANYEPS: case IALLEPS:
                GAT(z,B01,1,0,0); break;
  case II0EPS: case II1EPS: case IJ0EPS: case IJ1EPS: case ISUMEPS:
                GAT(z,INT,1,0,0); break;
 }
 if(!(mk||m&&n&&zn&&th))switch(mode){
  case IIDOT:   R reshape(shape(z),sc(n?m:0  ));
  case IICO:    R reshape(shape(z),sc(n?m:m-1));
  case INUBSV:  R reshape(shape(z),take(sc(m),one));
  case INUB:    AN(z)=0; *AS(z)=m?1:0; R z;
  case ILESS:   if(m)AN(z)=*AS(z)=0; else MC(AV(z),AV(w),k1*AN(w)); R z;
  case IEPS:    R reshape(shape(z),m&&(!n||th)?one:zero);
  case INUBI:   R m?iv0:mtv;
  case II0EPS:  R sc(n?0L        :c         );
  case II1EPS:  R sc(n?c         :0L        );
  case IJ0EPS:  R sc(n?MAX(0,c-1):c         );
  case IJ1EPS:  R sc(n?c         :MAX(0,c-1));
  case ISUMEPS: R sc(n?0L        :c         );
  case IANYEPS: R    n?zero:one;
  case IALLEPS: R c&&n?zero:one;
  case IIFBEPS: R n?mtv :IX(c);
 }
 if(a!=w&&!mk&&1==acr&&(1==wc||ac==wc)&&(D)m*n*zn<13*((D)m*n+zn)&&(mode==IIDOT||mode==IICO||mode==IEPS)){
  fn=jtiosc;
 }else{B b=0==jt->ct;I ht=INT,t1;
  if(!b&&t&BOX+FL+CMPX)ctmask(jt);
  if     (t&BOX)          fn=b&&(1<n||usebs(a,ac,m))?jtiobs:1<n?jtioa:b?jtioax1:
                              (t1=utype(a,ac))&&(mk||a==w||TYPESEQ(t1,utype(w,wc)))?jtioau:jtioa1;
  else if(t&XNUM)         fn=jtiox;
  else if(t&RAT )         fn=jtioq;
  else if(1==k)           {fn=jtio1; if(!(mode==IIDOT||mode==IICO))ht=B01;}
  else if(2==k)           {fn=jtio2; if(!(mode==IIDOT||mode==IICO))ht=B01;}
  else if(k==SZI&&!(t&FL)){if(p==ss){fn=jtio4; if(!(mode==IIDOT||mode==IICO))ht=B01;}else fn=jtioi;}
  else                    fn=b||t&B01+JCHAR+INT+SBT?jtioc:1==n?(t&FL?jtiod1:jtioz1):t&FL?jtiod:jtioz;
  if(fn!=jtiobs)GA(h,ht,p,1,0);
 }
 if(fn==jtioc){A x;B*b;C*u,*v;I*d,q;
  GATV(x,B01,k,1,0); b=BAV(x); memset(b,C1,k);
  q=k; u=CAV(a); v=u+k;
  DO(ac*(m-1), DO(k, if(u[i]!=*v&&b[i]){b[i]=0; --q;} ++v;); if(!q)break;);
  if(q){jt->hin=k-q; GATV(hi,INT,k-q,1,0); jt->hiv=d=AV(hi); DO(k, if(!b[i])*d++=i;); fn=jtiocx;}
 }
 RZ(fn(jt,mode,m,n,c,k,acr,wcr,ac,wc,ak,wk,a,w,&h,z));
 if(mk){A x,*zv;I*xv,ztype;
  GAT(z,BOX,3,1,0); zv=AAV(z);
  GAT(x,INT,6,1,0); xv=AV(x);
  switch(mode){
   default:                    ztype=0; break;  /* integer vector      */
   case ILESS:                 ztype=1; break;  /* type/shape from arg */
   case IEPS:                  ztype=2; break;  /* boolean vector      */
   case IANYEPS: case IALLEPS: ztype=3; break;  /* boolean scalar      */
   case ISUMEPS:
   case II0EPS:  case II1EPS:  
   case IJ0EPS:  case IJ1EPS:  ztype=4;         /* integer scalar      */
  }
  xv[0]=mode; xv[1]=n; xv[2]=k; xv[3]=jt->min; xv[4]=(I)fn; xv[5]=ztype; 
  zv[0]=x; zv[1]=h; zv[2]=hi;
 }
 EPILOG(z);
}    /* a i."r w main control */

A jtindexofprehashed(J jt,A a,A w,A hs){A h,hi,*hv,x,z;AF fn;I ar,*as,at,c,f1,k,m,mode,n,
     r,t,*xv,wr,*ws,wt,ztype;
 RZ(a&&w&&hs);
 hv=AAV(hs); x=hv[0]; h=hv[1]; hi=hv[2];  
 xv=AV(x); mode=xv[0]; n=xv[1]; k=xv[2]; jt->min=xv[3]; fn=(AF)xv[4]; ztype=xv[5]; 
 ar=AR(a); as=AS(a); at=AT(a); t=at; m=ar?*as:1; 
 wr=AR(w); ws=AS(w); wt=AT(w);
 if(1==ztype)r=wr?wr-1:0;
 else        r=ar?ar-1:0;
 f1=wr-r;
 ASSERT(r<=ar&&0<=f1,EVRANK); 
 ASSERT(!ICMP(as+ar-r,ws+f1,r),EVLENGTH);
 RE(c=prod(f1,ws));
 if(mode==ILESS&&(TYPESNE(t,wt)||AFLAG(w)&AFNJA+AFREL||n!=aii(w)))R less(w,a);
 if(!(m&&n&&c&&HOMO(t,wt)&&UNSAFE(t)>=UNSAFE(wt)))R indexofsub(mode,a,w);
 switch(ztype){
  case 0: GATV(z,INT,c,    f1, ws); break;
  case 1: GA(z,wt, AN(w),1+r,ws); break;
  case 2: GATV(z,B01,c,    f1, ws); break;
  case 3: GAT(z,B01,1,    0,  0 ); break;
  case 4: GAT(z,INT,1,    0,  0 ); break;
 }
 jt->hin=AN(hi); jt->hiv=AV(hi);
 if(TYPESNE(t,wt))RZ(w=cvt(t,w)) else if(t&FL+CMPX)RZ(w=cvt0(w));
 R fn(jt,mode+IPHOFFSET,m,n,c,k,AR(a),AR(w),(I)1,(I)1,(I)0,(I)0,a,w,&h,z);
}

F2(jtindexof){R indexofsub(IIDOT,a,w);}
     /* a i."r w */

F2(jtjico2){R indexofsub(IICO,a,w);}
     /* a i:"r w */

F1(jtnubsieve){
 RZ(w);
 if(SPARSE&AT(w))R nubsievesp(w); 
 if(jt->rank)jt->rank[0]=jt->rank[1]; 
 R indexofsub(INUBSV,w,w); 
}    /* ~:"r w */

F1(jtnub){ 
 RZ(w);
 if(SPARSE&AT(w)||AFLAG(w)&AFNJA+AFREL)R repeat(nubsieve(w),w); 
 R indexofsub(INUB,w,w);
}    /* ~.w */

F2(jtless){A x=w;I ar,at,k,r,*s,wr,*ws,wt;
 RZ(a&&w);
 at=AT(a); ar=AR(a); 
 wt=AT(w); wr=AR(w); r=MAX(1,ar);
 if(ar>1+wr)R ca(a);
 if(wr&&r!=wr){RZ(x=gah(r,w)); s=AS(x); ws=AS(w); k=ar>wr?0:1+wr-r; *s=prod(k,ws); ICPY(1+s,k+ws,r-1);}
 R !(at&SPARSE)&&HOMO(at,wt)&&TYPESEQ(at,maxtype(at,wt))&&!(AFLAG(a)&AFNJA+AFREL)?indexofsub(ILESS,x,a):
     repeat(not(eps(a,x)),a);
}    /* a-.w */

F2(jteps){I l,r,rv[2];
 RZ(a&&w);
 rv[0]=r=jt->rank?jt->rank[1]:AR(w);
 rv[1]=l=jt->rank?jt->rank[0]:AR(a); jt->rank=0;
 if(SPARSE&AT(a)+AT(w))R lt(irs2(w,a,0L,r,l,jtindexof),sc(r?*(AS(w)+AR(w)-r):1));
 jt->rank=rv; 
 R indexofsub(IEPS,w,a);
}    /* a e."r w */

F1(jtnubind){
 RZ(w);
 R SPARSE&AT(w)?icap(nubsieve(w)):indexofsub(INUBI,w,w);
}    /* I.@~: w */

F1(jtnubind0){A z;D oldct=jt->ct;
 RZ(w);
 jt->ct=0.0; z=SPARSE&AT(w)?icap(nubsieve(w)):indexofsub(INUBI,w,w); jt->ct=oldct;
 R z;
}    /* I.@(~:!.0) w */


F1(jtsclass){A e,x,xy,y,z;I c,j,m,n,*v;P*p;
 RZ(w);
 if(!AR(w))R reshape(v2(1L,1L),one);
 n=IC(w);
 RZ(x=indexof(w,w));
 if(DENSE&AT(w))R atab(CEQ,repeat(eq(IX(n),x),x),x);
 p=PAV(x); e=SPA(p,e); y=SPA(p,i); RZ(xy=stitch(SPA(p,x),y));
 if(n>*AV(e))RZ(xy=over(xy,stitch(e,less(IX(n),y))));
 RZ(xy=grade2(xy,xy)); v=AV(xy);
 c=*AS(xy);
 m=j=-1; DO(c, if(j!=*v){j=*v; ++m;} *v=m; v+=2;);
 GAT(z,SB01,1,2,0);  v=AS(z); v[0]=1+m; v[1]=n;
 p=PAV(z); 
 SPB(p,a,v2(0L,1L));
 SPB(p,e,zero);
 SPB(p,i,xy);
 SPB(p,x,reshape(sc(c),one));
 R z;
}


#define IOCOLF(f)     void f(J jt,I m,I c,I d,A a,A w,A z,A h)
#define IOCOLDECL(T)  D tl=1-jt->ct,tr=1/tl,x;                           \
                          I hj,*hv=AV(h),i,j,jr,l,p=AN(h),*u,*zv=AV(z);  \
                          T*av=(T*)AV(a),*v,*wv=(T*)AV(w);UI pm=p

#define IOCOLFT(T,f,hasha,hashl,hashr,exp)  \
 IOCOLF(f){IOCOLDECL(T);                                                   \
  for(l=0;l<c;++l){                                                        \
   DO(p, hv[i]=m;);                                                        \
   v=av;         DO(m, j=(hasha)%pm; FIND(exp); if(m==hj)hv[j]=i; v+=c;);  \
   v=wv; u=zv;                                                             \
   for(i=0;i<d;++i){                                                       \
    x=*(D*)v;                                                              \
    j=jr=(hashl)%pm;           FIND(exp); *u=hj;                           \
    j=   (hashr)%pm; if(j!=jr){FIND(exp); *u=MIN(*u,hj);}                  \
    v+=c; u+=c;                                                            \
   }                                                                       \
   ++av; ++wv; ++zv;                                                       \
 }}

#define JOCOLFT(T,f,hasha,hashl,hashr,exp)  \
 IOCOLF(f){IOCOLDECL(T);I q;                                               \
  for(l=0;l<c;++l){                                                        \
   DO(p, hv[i]=m;);                                                        \
   v=av+c*(m-1); DQ(m, j=(hasha)%pm; FIND(exp); if(m==hj)hv[j]=i; v-=c;);  \
   v=wv; u=zv;                                                             \
   for(i=0;i<d;++i){                                                       \
    x=*(D*)v;                                                              \
    j=jr=(hashl)%pm;           FIND(exp); *u=q=hj;                         \
    j=   (hashr)%pm; if(j!=jr){FIND(exp); if(m>hj&&(hj>q||q==m))*u=hj;}    \
    v+=c; u+=c;                                                            \
   }                                                                       \
   ++av; ++wv; ++zv;                                                       \
 }}

static IOCOLFT(D,jtiocold,hid(*v),    hid(tl*x),hid(tr*x),!teq(*v,av[c*hj]))
static IOCOLFT(Z,jtiocolz,hid(*(D*)v),hid(tl*x),hid(tr*x),!zeq(*v,av[c*hj]))

static JOCOLFT(D,jtjocold,hid(*v),    hid(tl*x),hid(tr*x),!teq(*v,av[c*hj]))
static JOCOLFT(Z,jtjocolz,hid(*(D*)v),hid(tl*x),hid(tr*x),!zeq(*v,av[c*hj]))

A jtiocol(J jt,I mode,A a,A w){A h,z;I ar,at,c,d,m,p,t,wr,*ws,wt;void(*fn)();
 RZ(a&&w);
 ASSERT(0!=jt->ct,EVNONCE);
 at=AT(a); ar=AR(a); m=*AS(a); c=aii(a);
 wt=AT(w); wr=AR(w); ws=AS(w); 
 d=1; DO(1+wr-ar, d*=ws[i];);
 RE(t=maxtype(at,wt));
 if(TYPESNE(t,at))RZ(a=cvt(t,a));
 if(TYPESNE(t,wt))RZ(w=cvt(t,w));
 p=hsize(m);
 GATV(h,INT,p,1,0);
 GATV(z,INT,AN(w),wr,ws);
 switch(CTTZ(t)){
  default:   ASSERT(0,EVNONCE);     
  case FLX:   fn=mode==IICO?jtjocold:jtiocold; ctmask(jt); break;
  case CMPXX: fn=mode==IICO?jtjocolz:jtiocolz; ctmask(jt); break;
 }
 fn(jt,m,c,d,a,w,z,h);
 R z;
}    /* a i."1 &.|:w or a i:"1 &.|:w */
