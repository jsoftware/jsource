/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Dyad +/ .* on Sparse Arguments                            */

#include "j.h"


static F1(jtbtreetest){ASSERT(0,EVDOMAIN);}
static F1(jtbtreedft){ASSERT(0,EVDOMAIN);}


/*
vm=: 4 : 0     NB. vector +/ .* sparse matrix
 if. dense x do. yy=.   5$.y [ 'i j'=. |:   4$.y
 else.           yy=. b#5$.y [ 'i j'=. |: b#4$.y [ b=. ({."1 ]4$.y) e. ,4$.x end.
 1 $. (}.$y);0;0;(,.~.j);j +//. (i{x) * yy
)

mv=: 4 : 0     NB. sparse matrix +/ .* vector
 if. dense y do. xx=.   5$.x [ 'i j'=. |:   4$.x
 else.           xx=. b#5$.x [ 'i j'=. |: b#4$.x [ b=. ({:"1 ]4$.x) e. ,4$.y end.
 1 $. (#x);0;0;(,.~.i);i +//. xx*j{y
)
*/

static F2(jtpdtspvv){A x;D*av,s,t,*wv,z;I i,*u,*u0,*uu,*v,*v0,*vv;P*ap,*wp;
 ARGCHK2(a,w);
 ap=PAV(a); x=SPA(ap,i); u=u0=AV(x); uu=u+AN(x); x=SPA(ap,x); av=DAV(x);
 wp=PAV(w); x=SPA(wp,i); v=v0=AV(x); vv=v+AN(x); x=SPA(wp,x); wv=DAV(x);
 z=0.0;
 NAN0;
 while(1){
  i=*u; while(i>*v&&v<vv)++v; if(v==vv)break;
  if(i==*v){s=av[u-u0]; t=wv[v-v0]; z+=s&&t?s*t:0; ++u; ++v; continue;}
  i=*v; while(i>*u&&u<uu)++u; if(u==uu)break;
  if(i==*u){s=av[u-u0]; t=wv[v-v0]; z+=s&&t?s*t:0; ++u; ++v; continue;}
 }
 NAN1;
 R scf(z);
}

static F2(jtpdtspmv){A ax,b,g,x,wx,y,yi,yj,z;B*bv;I m,n,s[2],*u,*v,*yv;P*ap,*wp,*zp;
 ARGCHK2(a,w);
 ap=PAV(a); y=SPA(ap,i); yv=AV(y); s[0]=n=AS(y)[0]; s[1]=1;
 GATVR(yj,INT,n,2,s);
 if(!ISSPARSE(AT(w))){
  GATVR(yi,INT,n,2,s); u=AV2(yi); AR(yj)=1; v=AV(yj);
  DQ(n, *u++=*yv++; *v++=*yv++;);
  ax=SPA(ap,x); RZ(wx=from(yj,w));
 }else{
  v=AV(yj);
  DQ(n, yv++; *v++=*yv++;);
  wp=PAV(w); RZ(b=eps(yj,SPA(wp,i))); bv=BAV(b); 
  AN(yj)=AS(yj)[0]=*s=m=bsum(n,bv); v=AV(yj); yv=AV(y);
  GATVR(yi,INT,m,2,s); u=AV2(yi);
  DQ(n, if(*bv++){*u++=*yv++; *v++=*yv++;}else yv+=2;);
  RZ(ax=repeat(b,SPA(ap,x))); RZ(wx=from(indexof(SPA(wp,i),yj),SPA(wp,x))); 
 }
 RZ(df2(x,yi,tymes(ax,wx),sldot(slash(ds(CPLUS)))));
 RZ(y=nub(yi));
 RZ(g=grade1(y));
 GASPARSE(z,STYPE(AT(x)),1,1,AS(a)); zp=PAV(z);
 SPB(zp,a,iv0);
 SPB(zp,e,scf(0.0));
 SPB(zp,i,from(g,y));
 SPB(zp,x,from(g,x));
 R z;
}    /* (sparse matrix) +/ .* vector; non-complex */

static F2(jtpdtspvm){A ax,b,g,x,wx,y,yi,yj,z;B*bv;D*av,c,d,*wv,*xv;I m,n,s[2],*u,*v,*yv;P*ap,*wp,*zp;
 ARGCHK2(a,w);
 wp=PAV(w); y=SPA(wp,i); yv=AV(y); s[0]=n=AS(y)[0]; s[1]=1;
 if(!ISSPARSE(AT(a))){
  GATVR(yj,INT,n,2,s); v=AV2(yj); 
  av=DAV(a); x=SPA(wp,x); wv=DAV(x);
  GATV0(x,FL,n,1); xv=DAV1(x);
  DQ(n, c=av[*yv++]; *v++=*yv++; d=*wv++; *xv++=c&&d?c*d:0;);
 }else{
  GATVR(yi,INT,n,2,s); u=AV2(yi);
  DQ(n, *u++=*yv++; yv++;);
  ap=PAV(a); RZ(b=eps(yi,SPA(ap,i))); bv=BAV(b); 
  AN(yi)=AS(yi)[0]=*s=m=bsum(n,bv); u=AV(yi); yv=AV(y);
  GATVR(yj,INT,m,2,s); v=AV2(yj);
  DQ(n, if(*bv++){*u++=*yv++; *v++=*yv++;}else yv+=2;);
  RZ(ax=from(indexof(SPA(ap,i),yi),SPA(ap,x))); RZ(wx=repeat(b,SPA(wp,x)));
  RZ(x=tymes(ax,wx));
 }
 RZ(df2(y,yj,x,sldot(slash(ds(CPLUS)))));
 RZ(y=nub(yj));
 RZ(g=grade1(y));
 GASPARSE(z,STYPE(AT(x)),1,1,1+AS(w)); zp=PAV(z);
 SPB(zp,a,iv0);
 SPB(zp,e,scf(0.0));
 SPB(zp,i,from(g,y));
 SPB(zp,x,from(g,x));
 R z;
}    /* vector +/ .* (sparse matrix); non-complex */

/* p  - ptr to sparse array value part   */
/* n  - # elements in index array        */
/* iv - ptr to index array (row,column)  */
/* m  - length of nv                     */
/* nv - row boundaries in iv             */
/* xv - ptr to data values               */
static B jtmmprep(J jt,P*p,I*n,I**iv,I*m,I**nv,D**xv){A x;I j,k,q,*u,*v;
 x=SPA(p,x); if(!ISDENSETYPE(AT(x),FL))RZ(x=cvt(FL,x)); *xv=DAV(x);
 x=SPA(p,i); *iv=u=AV(x); *n=AN(x);
 if(m&&nv){
  q=AS(x)[0]; k=q?2+u[(q-1)<<1]-*u:1;
  GATV0(x,INT,k,1); *nv=v=AV1(x);
  k=-1; DO(q, j=*u++; u++; if(j>k){*v++=i; k=j;});
  *v++=q; AN(x)=AS(x)[0]=k=v-*nv; *m=k-1;
 }
 R 1;
}

/* ii  - row index in result                  */
/* zjn - # temp result values to be harvested */
/* zj  - temp result column indices           */
/* zyv - ptr to temp result values            */
/* n   - # result values so far               */
/* zi  - ptr to result index array            */
/* zx  - ptr to result value array            */
static B jtmmharvest(J jt,I ii,I zjn,A zj,D*zyv,I*n,A*zi,A*zx){A x;D*zxv,*zxv0;I j,m,p,*v,*ziv;
 m=MIN(*AS(*zi),*AS(*zx));
 while(m<*n+zjn){RZ(*zi=ext(0,*zi)); RZ(*zx=ext(0,*zx)); m=MIN(*AS(*zi),*AS(*zx));}
 m=AN(zj); AN(zj)=AS(zj)[0]=zjn; RZ(x=grade2(zj,zj)); AN(zj)=AS(zj)[0]=m; 
 p=-1; v=AV(x); ziv=AV(*zi)+(*n<<1); zxv=zxv0=DAV(*zx)+*n;
 DQ(zjn, if(p<(j=*v++)){p=j; *ziv++=ii; *ziv++=j; *zxv++=zyv[j]; zyv[j]=0;});
 *n+=zxv-zxv0;
 R 1;
}    /* collect accumulated values for row ii */

static F2(jtpdtspmm){A z,zi,zj,zx,zy,*old;D*axv,c,d,*dv,*wxv,*zyv;
     I*aiv,*aivm,i,ii,j,k,k0,m,n=0,p,q,*v,wm,*wiv,*wnv,*zjv,*zjv0;P*zp;
 ARGCHK2(a,w);
 RZ(mmprep(PAV(a),&m,&aiv,0L ,0L  ,&axv)); aivm=m+aiv;
 RZ(mmprep(PAV(w),&m,&wiv,&wm,&wnv,&wxv));
 GATV0(zy,FL,AS(w)[1],1); zyv=DAV1(zy); mvc(AN(zy)*sizeof(D),zyv,MEMSET00LEN,MEMSET00);
 old=jt->tnextpushp; 
 RZ(zj=exta(INT,1L,1L,1000L)); zjv0=AV(zj);
 RZ(zi=exta(INT,2L,2L,1000L));
 RZ(zx=exta(FL, 1L,1L,1000L));
 NAN0;
 if(wm&&aiv<aivm){
  i=*aiv++; zjv=zjv0; k=-1;
  while(aiv<aivm){                 /* run through aiv and axv exactly once */
   j=*aiv++; 
   if(c=*axv++){
    k0=k; p=k+1; q=wm; ii=-1;
    while(p<=q){ii=wiv[wnv[k=(p+q)>>1]<<1]; if(j==ii)break; if(j<ii)q=k-1; else p=k+1;}
    if(j==ii){
     p=wnv[1+k]-wnv[k]; dv=wxv+wnv[k]; v=1+wiv+(wnv[k]<<1);
     q=zjv-zjv0; while(AN(zj)<p+q){RZ(zj=ext(0,zj)); zjv0=AV(zj); zjv=q+zjv0;}
     DQ(p, if(d=*dv++){if(!zyv[*v])*zjv++=*v; zyv[*v++]+=c*d; v++;});
    }else k=k0;
   }
   if(aiv>=aivm||i<(p=*aiv++)){    /* done with row i in a, emit row i in z */
    RZ(mmharvest(i,zjv-zjv0,zj,zyv,&n,&zi,&zx));
    gc3(&zj,&zi,&zx,old);  // these connot be virtual
    zjv=zjv0; k=-1; i=p;
 }}}
 NAN1;
 AS(zx)[0]=AN(zx)=AS(zi)[0]=n; AN(zi)=n<<1;
 GASPARSE(z,FL,1,2,AS(a)); AS(z)[1]=AS(w)[1];
 zp=PAV(z); SPB(zp,a,apvwr(2,0L,1L)); SPB(zp,e,scf(0.0)); SPB(zp,i,zi); SPB(zp,x,zx);
 R z;
}


F2(jtpdtsp){A x;B ab=0,wb=0;P*p;
 ARGCHK2(a,w);
 ASSERT(!AR(a)||!AR(w)||AS(a)[AR(a)-1]==AS(w)[0],EVLENGTH);
 if(AT(a)&FL&&AT(w)&FL){
  if(ISSPARSE(AT(a))){p=PAV(a); x=SPA(p,a); ab=AR(a)==AN(x)&&equ(num(0),SPA(p,e));}
  if(ISSPARSE(AT(w))){p=PAV(w); x=SPA(p,a); wb=AR(w)==AN(x)&&equ(num(0),SPA(p,e));}
 }
 if(ab&&1==AR(a)&&wb&&1==AR(w))R pdtspvv(a,w);
 if(ab&&2==AR(a)&&    1==AR(w))R pdtspmv(a,w);
 if(    1==AR(a)&&wb&&2==AR(w))R pdtspvm(a,w);
 if(ab&&2==AR(a)&&wb&&2==AR(w))R pdtspmm(a,w);
 R df2(x,a,w,atop(slash(ds(CPLUS)),qq(ds(CSTAR),v2(1L,AR(w)))));
}
