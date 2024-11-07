/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Index-of on Sparse Arrays                                        */

#include "j.h"


static I jtioev(J jt,I mode,A a){A ae,ax,ay,p;B*pv;I j,k,m,n,*yv;P*ap;
 ap=PAV(a);
 ae=SPA(ap,e);
 ay=SPA(ap,i); yv=AV(ay);
 ax=SPA(ap,x); m=k=AN(ax); n=j=AS(a)[0]; 
 RZ(p=eq(ax,ae)); pv=BAV(p);
 switch((AN(ay)?2:0)+(I )(1==mode)){
 case 0:  DO(m,           if(          pv[i])R i;); R m;
 case 1:  DQ(m,      --k; if(          pv[k])R k;); R n-!m;
 case 2:  DO(m,           if(i!=yv[i]||pv[i])R i;); R m;
 default: DQ(m, --j; --k; if(j!=yv[k]||pv[k])R j;); R m==n?n:n-m-1;
 }
}   /* index of sparse element */

A jtiovxs(J jt,I mode,A a,A w){A e,x,z;B h;I at,t,wt;P*ap=0,*wp,*zp;
 at=AT(a); if(ISSPARSE(at)){at=DTYPE(at); ap=PAV(a);}
 wt=DTYPE(AT(w)); wp=PAV(w); 
 if(h=HOMO(at,wt))t=maxtype(at,wt);
 GASPARSE(z,INT,1,AR(w),AS(w)); zp=PAV(z);
 SPB(zp,a,SPA(wp,a));
 SPB(zp,i,SPA(wp,i));
 e=SPA(wp,e); if(h&&TYPESNE(t,wt))RZ(e=cvt(t,e));
 x=SPA(wp,x); if(h&&TYPESNE(t,wt))RZ(x=cvt(t,x));
 if(ap){A ae,ax,ay,p,q;B b=0,*pv;I j,k,m,n,*v,*yv;
  ay=SPA(ap,i); yv=AV(ay); 
  ae=SPA(ap,e); if(h&&TYPESNE(t,at))RZ(ae=cvt(t,ae));
  ax=SPA(ap,x); if(h&&TYPESNE(t,at))RZ(ax=cvt(t,ax)); if(!AN(ay))RZ(ax=ravel(ax));
  m=AN(ax); n=AS(a)[0];
  j=ioev(mode,a);
  if(equ(ae,e))SPB(zp,e,sc(j))
  else{RE(k=i0(indexofsub(mode,ax,e))); SPB(zp,e,sc(AN(ay)?(m>k?yv[k]:n):k));}
  RZ(q=indexofsub(mode,ax,x)); v=AV(q);
  if(AN(ay)||AN(SPA(ap,a))){
   DQ(AN(x), k=*v; *v++=m>k?yv[k]:n;);
   if(j<n){RZ(p=eq(ae,x)); pv=BAV(p); v=AV(q); DO(AN(x), if(pv[i])*v=j; ++v;);}
  }
  SPB(zp,x,q);
 }else{
  if(h&&TYPESNE(t,at))RZ(a=cvt(t,a));
  SPB(zp,e,indexofsub(mode,a,e));
  SPB(zp,x,indexofsub(mode,a,x));
 }
 R z;
}    /* vector i. sparse */

A jtiovsd(J jt,I mode,A a,A w){A ae,ax,ay,p,z;B h,*pv;I at,j,m,n,t,wt,*v,*yv;P*ap;
 ap=PAV(a); ax=SPA(ap,x); ay=SPA(ap,i);
 if(!AS(ay)[1])R indexofsub(mode,ravel(ax),w);// special case if no sparse axes: just like dense i.  
 m=AN(ax); n=AS(a)[0]; yv=AV(ay); ae=SPA(ap,e);
 at=DTYPE(AT(a)); wt=AT(w); if(h=HOMO(at,wt))t=maxtype(at,wt);
 if(h&&TYPESNE(t,wt))RZ(w=cvt(t,w));
 j=ioev(mode,a); 
 RZ(z=mkwris(indexofsub(mode,ax,w))); v=AV(z);  // ensure z writable since we may modify it here
 RZ(p=eq(ae,w)); pv=BAV(p); 
 DO(AN(w), *v=pv[i]?j:m>*v?yv[*v]:n; ++v;);
 R z;
}    /* (sparse vector) i. dense */


A jtindexofxx(J jt,I mode,A a,A w){A x;B*b,*c,s;I ar,d,j,m,n,wr;P*p;
 ARGCHK2(a,w);
 s=ISSPARSE(AT(a)); ar=AR(a); wr=AR(w); d=wr-ar;
 if(s){p=PAV(a); m=ar; n=wr;}
 else {p=PAV(w); m=wr; n=ar;}
 RZ(b=bfi(m,SPA(p,a),1)); b[0]=1;
 GATV0(x,B01,n,1); c=BAV1(x);
 DO(ABS(d), c[i]=s;);  // initialize unfilled part of c
 j=0; DQ(MIN(ar,wr), ++j; c[n-j]=b[m-j];);
 R indexofss(mode,s?a:reaxis(ifb(n,c),a),s?reaxis(ifb(n,c),w):w);
}    /* dense i. sparse   or   sparse i. dense;  1<AR(a) */ 

static F1(jtifdz){I m;
 ARGCHK1(w);
 m=bplg(AT(w))-LGSZI; AN(w)<<=m; AS(w)[1]<<=m;
 AT(w)=INT;
 R w;
}    /* INT from FL or CMPX, in place */

static A jtioe(J jt,I mode,A w){A b,j,p,y;I c,jn,*jv,k,n;P*wp;
 wp=PAV(w);
 n=AS(w)[0]; y=SPA(wp,i);
 if(!AN(y))R sc(1==mode?(n?n-1:0):0);
 RZ(b=eq(SPA(wp,e),SPA(wp,x)));
 if(2<AR(b)){AS(b)[1]=aii(b); AR(b)=2;}
 if(1<AR(b))RZ(b=aslash1(CSTARDOT,b));  /* b=. *./@,"_1 (3$.w)=5$.w */
 RZ(y=irs2(num(0),y,0L,0L,1L,jtfrom));
 RZ(df2(p,y,b,sldot(slash(ds(CSTARDOT)))));
 RZ(j=repeat(not(p),repeat(ne(y,curtail(over(num(-1),y))),y)));
 jn=AN(j); jv=AV(j);
 if(n==jn)k=n;
 else{
  if(1==mode){k=*jv-1; jv+=jn-1; c=n; DO(jn, --c; if(c!=*jv--){k=c; break;});}  /* i: */
  else       {k=1+jv[jn-1];           DO(jn,      if(i!=*jv++){k=i; break;});}  /* i. */
 }
 R sc(k);
}    /* index of sparse item; leading axis is sparse */

static B jtioresparse(J jt,B aw,A*za,A*zw){A a,e,w;B*ab,ac=0,*wb,wc=0;I ar,j,wr;P*ap,*wp;
 a=*za; ar=AR(a); ap=PAV(a); RZ(ab=bfi(ar,SPA(ap,a),1)); 
 if(!*ab)*ab=ac=1;
 if(aw){
  w=*zw; wr=AR(w); wp=PAV(w);
  e=SPA(ap,e); if(!equ(e,SPA(wp,e))){RZ(w=rezero(e,w)); wp=PAV(w);}
  RZ(wb=bfi(wr,SPA(wp,a),1));  
  j=wr-ar; DO(ar-1, ++j; if(ab[1+i]<wb[j])ab[1+i]=ac=1; else if(ab[1+i]>wb[j])wb[j]=wc=1;);
  DO(1+wr-ar, if(!wb[i])wb[i]=wc=1;);
 }
 if(    ac)RZ(*za=reaxis(ifb(ar,ab),a));
 if(aw&&wc)RZ(*zw=reaxis(ifb(wr,wb),w));
 R 1;
}    /* harmonize sparse elements and sparse axes for a and w */

static B jtiopart(J jt,A w,I r,I mm,I*zc,A*zi,A*zj,A*zx){A b,f,wx,x,wy,y;B*bv;
     I c=*zc,d,i,j,k,m,n,nd,p,q,wr,*v,*xv;P*wp;
 wr=AR(w); d=wr-r;
 wp=PAV(w); wy=SPA(wp,i); wx=SPA(wp,x); n=AR(wx)-1;
 RZ(b=not(irs2(wx,reshape(vec(INT,n,1+AS(wx)),SPA(wp,e)),0L,n,n,jtmatch)));
 if(!all1(b)){RZ(wx=repeat(b,wx)); RZ(wy=repeat(b,wy));}
 v=AV(wy); m=AS(wy)[0]; n=AS(wy)[1]; nd=n-d;
 GATV0(b,B01,m,1); bv=BAV1(b);
 if     (0==d){mvc(m,bv,MEMSET00LEN,MEMSET00); if(m)*bv=1;}
 else if(1==d){j=-1; DO(m, bv[i]=j!=*v; j=*v; v+=n;);}
 else{
  GATV0(x,INT,d,1); xv=AV1(x); *xv=-1;
  DO(m, bv[i]=0; DO(d, if(xv[i]!=v[i]){bv[i]=1; j=i; DQ(d-j, xv[j]=v[j]; ++j;); break;}); v+=n;)
 }
 if(m){RZ(f=cut(ds(CCOMMA),num(1))); RZ(df2(y,b,dropr(d,wy),f)); RZ(df2(x,b,wx,f));}
 else{y=mtm; RZ(x=reshape(v2(0L,prod(r,AS(w)+wr-r)),wx));}
 if(0>c)*zc=c=AS(y)[1]; 
 else if(c!=AS(y)[1]){RZ(y=taker(c,y)); RZ(x=taker((c/(n-d))*aii(wx),x));}
 v=AV(y); k=0; q=AS(y)[0];
 for(i=0;i<q;++i){
  j=k; k=1+j; while(k<m&&!bv[k])++k; p=nd*(k-j); 
  if(c<p)*v=mm; else DO(c-p, v[p+i]=mm;); 
  v+=c;
 }
 RZ(*zi=repeat(b,taker(d,wy)));
 *zj=y;
 *zx=x;
 R 1;
}

A jtindexofss(J jt,I mode,A a,A w){A ai,aj,ax,wi,wj,wx,x,y,z;B aw=a!=w;I ar,c,m,mm,n,r,*u,*v,wr;P*zp;
 ARGCHK2(a,w);
 ar=AR(a);
 wr=AR(w); r=1+wr-ar; 
 RZ(ioresparse(aw,&a,&w));
 v=AS(a); n=*v++; mm=-1; DO(ar-1, mm=MAX(mm,v[i]););
 c=-1; RZ(iopart(a,ar-1,mm,&c,&ai,&aj,&ax));
 if(aw)RZ(iopart(w,ar-1,mm,&c,&wi,&wj,&wx));
 switch(aw?(FL+CMPX&maxtype(AT(ax),AT(wx))?3:1):FL+CMPX&AT(ax)?2:0){
 case 0: x=stitch(aj,ax);                           break;
 case 1: x=stitch(aj,ax); y=stitch(wj,wx);          break;
 case 2: x=stitch(aj,1.0!=jt->cct?iocol(mode,ax,ax):ifdz(ax)); break;
 case 3: x=stitch(aj,1.0!=jt->cct?iocol(mode,ax,ax):ifdz(ax)); 
         y=stitch(wj,1.0!=jt->cct?iocol(mode,ax,wx):ifdz(wx));
 }
 RZ(x=indexofsub(mode,x,aw?y:x)); u=AV(x);
 m=AS(ai)[0]; v=AV(ai); 
 if(aw)DO(AN(x), u[i]=m>u[i]?v[u[i]]:n;)
 else  DO(AN(x), u[i]=v[u[i]];);
 if(!r)R AN(x)?sc(*u):ioe(mode,a);
 GASPARSE(z,INT,1,r,AS(w)); zp=PAV(z);
 SPB(zp,a,apvwr(r,0L,1L));
 SPB(zp,e,ioe(mode,a));
 SPB(zp,i,aw?wi:ai);
 SPB(zp,x,x);
 R z;
}    /* sparse i. sparse */

F1(jtnubsievesp){A e,x,y,z;I c,j,m,n,r,*s,*u,*v,*vv,wr,*yv;P*p;D rkblk[16];
 ARGCHK1(w);
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; RESETRANK;
 n=r?AS(w)[wr-r]:1;
 if(r<wr)R ATOMIC2(jt,IX(n),irs2(w,w,0L,r,r,jtindexof),rkblk,1L,r?1L:0L,CEQ);  // seems to fail
 RZ(x=indexof(w,w)); p=PAV(x);
 y=SPA(p,i); u=AV(y); c=AS(y)[0];
 x=SPA(p,x); v=AV(x);
 e=SPA(p,e); j=*AV(e); m=j<n;
 DO(c, m+=u[i]==v[i];);
 GATV0(y,INT,m,2); s=AS(y); s[0]=m; s[1]=1; vv=yv=AV2(y);
 if(c)DO(c, if(u[i]==v[i]){if(j<u[i]){*vv++=j; j=n;} *vv++=u[i];})
 if(m&&vv<yv+m)*vv=j;
 GASPARSE(z,B01,1,1,&n); p=PAV(z);
 SPB(p,a,iv0);
 SPB(p,e,num(0));
 SPB(p,i,y);
 SPB(p,x,reshape(sc(m),num(1)));
 R z;
}
