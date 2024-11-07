/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Atomic (Scalar) Dyadic Verbs on Sparse Arrays                    */

#include "j.h"
#include "ve.h"

static A jtvaspc(J jt,A a,A w,C id,VF ado,I cv,I t,I zt,I af,I acr,I wf,I wcr,I f,I r){A q;I*as,*v,*ws;
 as=AS(a);
 ws=AS(w);
 GATV0(q,INT,f+r,1); v=AV1(q);
 if(r>acr){ICPY(v,wf+ws,r); RZ(a=irs2(vec(INT,r-acr,acr+v),a,0L,1L,0L,jtreshape));}
 if(r>wcr){ICPY(v,af+as,r); RZ(w=irs2(vec(INT,r-wcr,wcr+v),w,0L,1L,0L,jtreshape));}
 R vasp(a,w,id,ado,cv,t,zt,af,r,wf,r,f,r);
}    /* prefix agreement on cells */

F1(jtvaspz){A e,x,y;B c,*u,*xu,*xv;I j,n,*v,*yu,*yv,xc,yc;P*wp;
 if(!(B01&AT(w)))R w;
 wp=PAV(w); 
 e=SPA(wp,e); c=!*BAV(e);
 x=SPA(wp,x); xv=xu=u=BAV(x); xc=aii(x); n=AS(x)[0]; j=0;
 y=SPA(wp,i); yv=yu=   AV(y); yc=AS(y)[1];
 if(!(n&&xc&&yc))R w;
 while(u=memchr(xv+xc*j,c,xc*(n-j))){
  j=(u-xv)/xc; v=yv+yc*j;
  if(v==yu){yu+=yc; xu+=xc;}
  else{DQ(yc, *yu++=*v++;); if(1<xc){MC(xu,xv+xc*j,xc); xu+=xc;}}
  ++j;
 }
 n=(yu-yv)/yc;
 if(1==xc)mvc(n,xv,1,iotavec-IOTAVECBEGIN+(c));
 AS(y)[0]=n; AN(y)=n*yc;
 AS(x)[0]=n; AN(x)=n*xc;
 RETF(w);
}    /* post processing on result; modifies argument in place */

static A jtvasp0(J jt,A a,A w,VF ado,I cv,I t,I zt){A e,x,xx,y,z,ze,zx;B b;I n;P*p,*zp;
 if(b=(AR(a)!=0)){xx=a; y=w;}else{xx=w; y=a;}  // xx is sparse arg, y is repeated; b='y is repeated'
 p=PAV(xx); e=SPA(p,e); x=SPA(p,x); n=AN(x);
 if(t){
  if(TYPESNE(t,AT(x))){RZ(x=cvt(t,x)); RZ(e=cvt(t,e));} 
  if(TYPESNE(t,AT(y))) RZ(y=cvt(t,y));
 }
 GA00(ze,zt,1,0); I rc;     ASSERT(((EVOK|EVNOCONV)&(rc=((AHDR2FN*)ado)AH2A_v(1,b?AV(e):AV(y),b?AV(y):AV(e),AV0(ze),jt), rc=rc<0?EWOVIP+EWOVIPMULII:rc)),rc);
 GA(zx,zt,n,AR(x),AS(x)); if(n)ASSERT(((EVOK|EVNOCONV)&(rc=((AHDR2FN*)ado)AH2A(1,2*n+1-b,b?AV(x):AV(y),b?AV(y):AV(x),AV(zx),jt), rc=rc<0?EWOVIP+EWOVIPMULII:rc)),rc);  // was !b
 if(cv&VRI+VRD&&rc!=EVNOCONV){RZ(ze=cvz(cv,ze)); RZ(zx=cvz(cv,zx));}
 GASPARSE(z,STYPE(AT(zx)),1,AR(xx),AS(xx)); zp=PAV(z);
 SPB(zp,a,ca(SPA(p,a)));
 SPB(zp,i,ca(SPA(p,i)));
 SPB(zp,e,ze);
 SPB(zp,x,zx);
 R vaspz(z);
}    /* one argument is sparse and the other is scalar */

static B jtvaspeqprep(J jt,A a,A w,I t,I f,I r,A*ae,A*ay,A*ax,A*we,A*wy,A*wx,A*za){
     A aa,e,q,x,wa;B*b,sa,sw;I n,*v;P*p;
 sa=ISSPARSE(AT(a)); 
 sw=ISSPARSE(AT(w)); n=f+r;
 GATV0(x,B01,n,1); b=BAV1(x); mvc(n,b,MEMSET00LEN,MEMSET00);
 if(sa){p=PAV(a); aa=SPA(p,a); v=AV(aa); DO(AN(aa), b[v[i]]=1;);}
 if(sw){p=PAV(w); wa=SPA(p,a); v=AV(wa); DO(AN(wa), b[v[i]]=1;);}
 q=ifb(n,b); makewritable(q) *za=q;  // avoid readonly result
 if(!sa||!equ(q,aa))RZ(a=reaxis(q,a));
 if(!sw||!equ(q,wa))RZ(w=reaxis(q,w));
 p=PAV(a); *ae=e=SPA(p,e); *ay=SPA(p,i); *ax=x=SPA(p,x); if(t&&TYPESNE(t,AT(x))){RZ(*ae=cvt(t,e)); RZ(*ax=cvt(t,x));}
 p=PAV(w); *we=e=SPA(p,e); *wy=SPA(p,i); *wx=x=SPA(p,x); if(t&&TYPESNE(t,AT(x))){RZ(*we=cvt(t,e)); RZ(*wx=cvt(t,x));}
 R 1;
}

static I zcount(A ay,A wy,B ab,B wb){I c,d,i,j,m,n,*u,*v,yc;
 v=AS(ay); m=v[0]; yc=v[1]; n=AS(wy)[0];
 i=j=d=0; u=AV(ay); v=AV(wy);
 while(m>i&&n>j){
  c=0; DO(yc, if(c=u[i]-v[i])break;);
  if(0>c)   {u+=yc; ++i;             if(wb)++d;}
  else if(c){            v+=yc; ++j; if(ab)++d;}
  else      {u+=yc; ++i; v+=yc; ++j;       ++d;}
 } 
 R d+wb*(m-i)+ab*(n-j);
}    /* item count for sparse result */

#define ADVA   axv+=ak; u+=yc; ++i;
#define ADVW   wxv+=wk; v+=yc; ++j;
#define FLUSH  if(d){c=d*yc; ICPY(zyv,u-c,c); I lrc=((AHDR2FN*)ado)AH2A_v(d*xc,axv-d*ak,wxv-d*wk,zxv,jt); lrc=lrc<0?EWOVIP+EWOVIPMULII:lrc; rc=lrc<rc?lrc:rc; \
                     zxv+=d*zk; zyv+=c; d=0;}

static A jtvaspeq(J jt,A a,A w,C id,VF ado,I cv,I t,I zt,I f,I r){A ae,ax,ay,we,wx,wy,z,za,ze,zx,zy;
     B ab=1,wb=1;C*aev,*axv,*wev,*wxv,*zxv;I ak,c,d,i,j,m,n,*u,*v,wk,xc,yc,zk,*zyv;P*zp;I rc=EVOK;
 RZ(vaspeqprep(a,w,t,f,r,&ae,&ay,&ax,&we,&wy,&wx,&za));
 if(id==CSTAR||id==CSTARDOT){ab=!equ(ae,num(0)); wb=!equ(we,num(0));} 
 v=AS(ay); m=v[0]; yc=v[1]; xc=aii(ax); n=AS(wy)[0];
 aev=CAV(ae); axv=CAV(ax); ak=xc<<bplg(AT(ax));
 wev=CAV(we); wxv=CAV(wx); wk=xc<<bplg(AT(wx));
 d=zcount(ay,wy,ab,wb);
 I zxr=AR(ax); GA(zx,zt, d*xc,AR(ax),AS(ax)); AS(zx)[0]=d; zxv=CAVn(zxr,zx); zk=xc<<bplg(zt);
 GATVR(zy,INT,d*yc,2,AS(ay)); AS(zy)[0]=d; zyv= AV2(zy);
 i=j=d=0; u=AV(ay); v=AV(wy);
 while(m>i&&n>j){
  c=0; DO(yc, if(c=u[i]-v[i])break;);
  if(0>c)   {FLUSH; if(wb){ICPY(zyv,u,yc); I lrc=((AHDR2FN*)ado)AH2A(1,2*xc,axv,wev,zxv,jt); lrc=lrc<0?EWOVIP+EWOVIPMULII:lrc; rc=lrc<rc?lrc:rc; zxv+=zk; zyv+=yc;} ADVA;}
  else if(c){FLUSH; if(ab){ICPY(zyv,v,yc); I lrc=((AHDR2FN*)ado)AH2A(1,2*xc+1,aev,wxv,zxv,jt); lrc=lrc<0?EWOVIP+EWOVIPMULII:lrc; rc=lrc<rc?lrc:rc; zxv+=zk; zyv+=yc;} ADVW;}
  else      {++d; ADVA; ADVW;}
 }
 FLUSH;
 if     (wb&&m>i){c=m-i; ICPY(zyv,u,c*yc); I lrc=((AHDR2FN*)ado)AH2A(1,2*c*xc,axv,wev,zxv,jt); lrc=lrc<0?EWOVIP+EWOVIPMULII:lrc; rc=lrc<rc?lrc:rc;}
 else if(ab&&n>j){c=n-j; ICPY(zyv,v,c*yc); I lrc=((AHDR2FN*)ado)AH2A(1,2*c*xc+1,aev,wxv,zxv,jt); lrc=lrc<0?EWOVIP+EWOVIPMULII:lrc; rc=lrc<rc?lrc:rc;}
 GA00(ze,zt,1,0); I lrc=((AHDR2FN*)ado)AH2A_v(1,aev,wev,AV0(ze),jt); lrc=lrc<0?EWOVIP+EWOVIPMULII:lrc; rc=lrc<rc?lrc:rc;
 ASSERT((rc&(255&~EVNOCONV))==0,rc);
 if(cv&VRI+VRD&&rc!=EVNOCONV){A e,x; RZ(e=cvz(cv,ze)); RZ(x=cvz(cv,zx)); if(TYPESEQ(AT(e),AT(x))){ze=e; zx=x;}}
 GASPARSE(z,STYPE(AT(zx)),1,AR(a),AS(a));
 zp=PAV(z); 
 SPB(zp,a,za); SPB(zp,e,ze); SPB(zp,i,zy); SPB(zp,x,zx);
 R vaspz(z);
}    /* frames and cell ranks equal */

A jtvasp(J jt,A a,A w,C id,VF ado,I cv,I t,I zt,I af,I acr,I wf,I wcr,I f,I r){A fs,z;
 if(!AR(a)||!AR(w))R vasp0(a,w,ado,cv,t,zt);
 if(ISSPARSE(AT(a)|AT(w))&&spmult(&z,a,w,id,af,acr,wf,wcr))R z;
 if(af!=wf){RZ(fs=ds(id)); R sprank2(a,w,fs,acr,wcr,VAV(fs)->valencefns[1]);}
 if(acr!=wcr)R vaspc(a,w,id,ado,cv,t,zt,af,acr,wf,wcr,f,r);
 R vaspeq(a,w,id,ado,cv,t,zt,f,r);
}    /* scalar dyadic fns with one or both arguments sparse */
