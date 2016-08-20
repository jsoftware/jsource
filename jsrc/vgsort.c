/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Grade -- dyad /: and \: where a==w                               */

#include "j.h"
#include "vg.h"


#define SF(f)         A f(J jt,I m,I c,I n,A w)

/* m - # cells (# individual grades to do) */
/* c - # atoms in a cell                   */
/* n - length of sort axis                 */
/* w - array to be graded                  */

static SF(jtsortb){A z;B up,*u,*v;I i,s;
 GA(z,AT(w),AN(w),AR(w),AS(w)); v=BAV(z);
 up=1==jt->compgt;  u=BAV(w);
 for(i=0;i<m;++i){
  s=bsum(n,u);
  if(up){memset(v,C0,n-s); memset(v+n-s,C1,s  );}
  else  {memset(v,C1,s  ); memset(v+s,  C0,n-s);}
  u+=n; v+=n;
 }
 R z;
}    /* w grade"1 w on boolean */

static SF(jtsortb2){A z;B up;I i,ii,j,p,yv[4];US*v,*wv,x,zz[4];
 GA(z,AT(w),AN(w),AR(w),AS(w)); v=USAV(z);
 wv=USAV(w); p=4; up=1==jt->compgt;
 DO(p, yv[i]=0;); 
 zz[0]=BS00; zz[1]=BS01; zz[2]=BS10; zz[3]=BS11;
 for(i=0;i<m;++i){
  DO(n, IND2(*wv++); ++yv[ii];);
  if(up){j=0;   DO(p, x=zz[j]; DO(yv[j], *v++=x;); yv[j]=0; ++j;);}
  else  {j=p-1; DO(p, x=zz[j]; DO(yv[j], *v++=x;); yv[j]=0; --j;);}
 }
 R z;
}    /* w grade"r w on 2-byte boolean items */

static SF(jtsortb4){A z;B up;I i,ii,j,p,yv[16];UINT*v,*wv,x,zz[16];
 GA(z,AT(w),AN(w),AR(w),AS(w)); v=(UINT*)AV(z);
 wv=(UINT*)AV(w); p=16; up=1==jt->compgt;
 DO(p, yv[i]=0;); 
 zz[ 0]=B0000; zz[ 1]=B0001; zz[ 2]=B0010; zz[ 3]=B0011;
 zz[ 4]=B0100; zz[ 5]=B0101; zz[ 6]=B0110; zz[ 7]=B0111;
 zz[ 8]=B1000; zz[ 9]=B1001; zz[10]=B1010; zz[11]=B1011;
 zz[12]=B1100; zz[13]=B1101; zz[14]=B1110; zz[15]=B1111;
 for(i=0;i<m;++i){
  DO(n, IND4(*wv++); ++yv[ii];);
  if(up){j=0;   DO(p, x=zz[j]; DO(yv[j], *v++=x;); yv[j]=0; ++j;);}
  else  {j=p-1; DO(p, x=zz[j]; DO(yv[j], *v++=x;); yv[j]=0; --j;);}
 }
 R z;
}    /* w grade"r w on 4-byte boolean items */

static SF(jtsortc){A z;B up;I i,p,yv[256];UC j,*wv,*v;
 GA(z,AT(w),AN(w),AR(w),AS(w)); v=UAV(z);
 wv=UAV(w); p=LIT&AT(w)?256:2; up=1==jt->compgt;
 DO(p, yv[i]=0;);
 for(i=0;i<m;++i){
  DO(n, ++yv[*wv++];);
  if(up){j=0;         DO(p, DO(yv[j], *v++=j;); yv[j]=0; ++j;);}
  else  {j=(UC)(p-1); DO(p, DO(yv[j], *v++=j;); yv[j]=0; --j;);}
 }
 R z;
}    /* w grade"1 w on boolean or character */

static SF(jtsortc2){A y,z;B up;I i,p,*yv;US j,k,*wv,*v;
 GA(z,AT(w),AN(w),AR(w),AS(w)); v=USAV(z);
 wv=USAV(w); p=65536; up=1==jt->compgt;
 GATV(y,INT,p,1,0); yv=AV(y);
 DO(p, yv[i]=0;);
 for(i=0;i<m;++i){
  DO(n, ++yv[*wv++];);
  if(C2T&AT(w)||!liln){
   if(up){j=0;         DO(p,                DO(yv[j], *v++=j;); yv[j]=0;           ++j;);}
   else  {j=(US)(p-1); DO(p,                DO(yv[j], *v++=j;); yv[j]=0;           --j;);}
  }else{
   if(up){k=0;         DO(256, j=k; DO(256, DO(yv[j], *v++=j;); yv[j]=0; j+=256;); ++k;);}
   else  {k=(US)(p-1); DO(256, j=k; DO(256, DO(yv[j], *v++=j;); yv[j]=0; j-=256;); --k;);}
 }}
 R z;
}    /* w grade"1 w on 2-byte character or unicode items */

static SF(jtsorti1);

static SF(jtsorti){A y,z;B up;D p1;I i,j,p,ps,q,s,*wv,*yv,*zv;
 wv=AV(w);
 irange(AN(w),wv,&q,&p); p1=(D)p;
 if(!p||256<p&&0.69*(p1+2*n)>n*log((D)n))R 3000<n?sorti1(m,n,n,w):irs2(gr1(w),w,0L,1L,1L,jtfrom);
 if(0<q&&p1+q<4*n){p+=q; q=0;}
 GATV(y,INT,p,1,0); yv=AV(y); ps=p*SZI; up=1==jt->compgt;
 GA(z,AT(w),AN(w),AR(w),AS(w)); zv=AV(z);
 memset(yv,C0,ps);
 for(i=0;i<m;++i){
  if(q)DO(n, ++yv[*wv++-q];) 
  else DO(n, ++yv[*wv++  ];);
  switch(2*up+(1&&q)){
   case 0: j=p-1; DO(p, s=yv[j]; yv[j]=0; DO(s, *zv++=j  ;); --j;); break;
   case 1: j=p-1; DO(p, s=yv[j]; yv[j]=0; DO(s, *zv++=j+q;); --j;); break;
   case 2: j=0;   DO(p, s=yv[j]; yv[j]=0; DO(s, *zv++=j  ;); ++j;); break;
   case 3: j=0;   DO(p, s=yv[j]; yv[j]=0; DO(s, *zv++=j+q;); ++j;); break;
 }}
 R z;
}    /* w grade"1 w on small-range integers */

static SF(jtsorti1){A x,y,z;I*wv;I d,e,i,p,q,*xv,*yv,*zv;int up;
 GA(z,AT(w),AN(w),AR(w),AS(w)); zv=AV(z);
 p=65536; up=1==jt->compgt; wv=AV(w);
 GATV(y,INT,p,1,0); yv=AV(y);
 GATV(x,INT,n,1,0); xv=AV(x);
 e=SY_64?3:1;
#if C_LE
  d= 1; 
#else
  d=-1;
#endif
 q=e*(-1==d);
 for(i=0;i<m;++i){
  grcol(p,0L,yv,n,wv,xv,sizeof(I)/sizeof(US),    q+(US*)wv,up,0,1);
#if SY_64
  grcol(p,0L,yv,n,xv,zv,sizeof(I)/sizeof(US),1*d+q+(US*)xv,up,0,1);
  grcol(p,0L,yv,n,zv,xv,sizeof(I)/sizeof(US),2*d+q+(US*)zv,up,0,1);
#endif
  grcol(p,0L,yv,n,xv,zv,sizeof(I)/sizeof(US),e*d+q+(US*)xv,up,1,1);
  wv+=c; zv+=n;
 }
 R z;
}    /* w grade"r w on large-range integers */

static SF(jtsortu1);

static SF(jtsortu){A y,z;B up;D p1;I i,p,ps,s,*yv;C4 j,*wv,*zv,q;
 wv=C4AV(w);
 c4range(AN(w),wv,&q,&p); p1=(D)p;
 if(!p||256<p&&0.69*(p1+2*n)>n*log((D)n))R 3000<n?sortu1(m,n,n,w):irs2(gr1(w),w,0L,1L,1L,jtfrom);
 if(0<q&&p1+q<4*n){p+=q; q=0;}
 GATV(y,INT,p,1,0); yv=AV(y); ps=p*SZI; up=1==jt->compgt;
 GA(z,AT(w),AN(w),AR(w),AS(w)); zv=C4AV(z);
 memset(yv,C0,ps);
 for(i=0;i<m;++i){
  if(q)DO(n, ++yv[*wv++-q];) 
  else DO(n, ++yv[*wv++  ];);
  switch(2*up+(1&&q)){
   case 0: j=(C4)p-1; DO(p, s=yv[j]; yv[j]=0; DO(s, *zv++=j  ;); --j;); break;
   case 1: j=(C4)p-1; DO(p, s=yv[j]; yv[j]=0; DO(s, *zv++=j+q;); --j;); break;
   case 2: j=0;   DO(p, s=yv[j]; yv[j]=0; DO(s, *zv++=j  ;); ++j;); break;
   case 3: j=0;   DO(p, s=yv[j]; yv[j]=0; DO(s, *zv++=j+q;); ++j;); break;
 }}
 R z;
}    /* w grade"1 w on small-range literal4 */

static SF(jtsortu1){A x,y,z;B b;C4*g,*h,*xu,*wv,*zu;I d,e,i,k,p;UI *yv;int up;
 GA(z,AT(w),AN(w),AR(w),AS(w));
 p=65536; up=1==jt->compgt; wv=C4AV(w); zu=C4AV(z);
 GATV(y,INT,p,1,0); yv=AV(y);
 GATV(x,C4T,n,1,0); xu=C4AV(x);
#if C_LE
 d= 1; e=0;
#else
 d=-1; e=1;
#endif
 for(i=0;i<m;++i){
  k=0; b=0;
  g=b?xu:zu; h=b?zu:xu;
  grcolu(p, 0L, yv,n,(UI*)wv,(UI*)h,sizeof(C4)/sizeof(US),e+0*d+(US*)wv,k==n?!up:up,0,1);
  grcolu(p, 0L, yv,n,(UI*)h, (UI*)g,sizeof(C4)/sizeof(US),e+1*d+(US*)h ,k==n?!up:up,0,1);
  if(b){
   g=zu;
   if(up){h=n+xu; DO(k,   *g++=*--h;); h=  xu; DO(n-k, *g++=*h++;);}
   else  {h=k+xu; DO(n-k, *g++=*h++;); h=k+xu; DO(k,   *g++=*--h;);}
  }
  wv+=c; zu+=n;
 }
 R z;
}    /* w grade"r w on large-range literal4 */

static SF(jtsortd){A x,y,z;B b;D*g,*h,*xu,*wv,*zu;I d,e,i,k,p,q,*yv;int up;
 if(n<8000)R irs2(gr1(w),w,0L,1L,1L,jtfrom);
 GA(z,AT(w),AN(w),AR(w),AS(w));
 p=65536; q=p/2; up=1==jt->compgt; wv=DAV(w); zu=DAV(z);
 GATV(y,INT,p,1,0); yv=AV(y);
 GATV(x,FL, n,1,0); xu=DAV(x);
#if C_LE
 d= 1; e=0;
#else
 d=-1; e=3;
#endif
 for(i=0;i<m;++i){
  g=wv; k=0; DO(n, if(0>*g++)++k;); b=0<k&&k<n;
  g=b?xu:zu; h=b?zu:xu;
  grcol(p,    0L,      yv,n,(I*)wv,(I*)h,sizeof(D)/sizeof(US),e+0*d+(US*)wv,k==n?!up:up,0,1);
  grcol(p,    0L,      yv,n,(I*)h, (I*)g,sizeof(D)/sizeof(US),e+1*d+(US*)h ,k==n?!up:up,0,1);
  grcol(p,    0L,      yv,n,(I*)g, (I*)h,sizeof(D)/sizeof(US),e+2*d+(US*)g ,k==n?!up:up,0,1);
  grcol(b?p:q,k==n?q:0,yv,n,(I*)h, (I*)g,sizeof(D)/sizeof(US),e+3*d+(US*)h ,k==n?!up:up,0,1);
  if(b){
   g=zu;
   if(up){h=n+xu; DO(k,   *g++=*--h;); h=  xu; DO(n-k, *g++=*h++;);}
   else  {h=k+xu; DO(n-k, *g++=*h++;); h=k+xu; DO(k,   *g++=*--h;);}
  }
  wv+=c; zu+=n;
 }
 R z;
}    /* w grade"1 w on real w */

// x /:"r y
F2(jtgr2){PROLOG(0076);A z=0;I acr,d,f,m,n,*s,t,wcr; 
 RZ(a&&w);
 // ?cr= rank of the cells being sorted; t= type of w
 acr=jt->rank?jt->rank[0]:AR(a); 
 wcr=jt->rank?jt->rank[1]:AR(w); t=AT(w);
 // Handle special in-place cases, when the arguments are identical and the cells are also.  Only if cells have rank>0
 if(a==w&&acr==wcr&&wcr>0){
  // f = length of frame of w; s->shape of w; m=#cells; n=#items in each cell (but invalid if cell has rank 0);
  // d = #bytes in an item of a cell of w (even if cell has rank 0 - then the cell has one item)
  f=AR(w)-wcr; s=AS(w); m=prod(f,s); n=(AR(w))?s[f]:1; d=bp(t)*prod(wcr-1,1+f+s);
  if     (1==d  &&t&B01&&(m==1||0==(n&(SZI-1))))   RZ(z=sortb (m,n,n,w))  // sorting Booleans, when all grades start on a word boundary
  else if(1==d)                      RZ(z=sortc (m,n,n,w))  // sorting single bytes (character or Boolean)
  else if(2==d  &&t&B01)             RZ(z=sortb2(m,n,n,w))  // Booleans with cell-items 2 bytes long
  else if(2==d  &&t&LIT+C2T&&30000<n)RZ(z=sortc2(m,n,n,w))  // long character strings with cell-items 2 bytes long
  else if(1==wcr&&t&C4T)             RZ(z=sortu (m,n,n,w))  // literal4 string lists
  else if(4==d  &&t&B01)             RZ(z=sortb4(m,n,n,w))  // Booleans with cell-items 4 bytes long
  else if(1==wcr&&t&INT)             RZ(z=sorti (m,n,n,w))  // integer lists
  else if(1==wcr&&t&FL )             RZ(z=sortd (m,n,n,w)); // floating-point lists
 }
 // If not a supported in-place case, grade w and then select those values from a
 if(!z)RZ(z=irs2(gr1(w),a,0L,1L,acr,jtfrom));
 EPILOG(z);
}    /* a grade"r w main control for dense w */
