/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Grades on Sparse Arrays                                          */

#include "j.h"
#include "vg.h"


/************************************************************************/
/*                                                                      */
/* monad /: and \: on sparse right arguments                            */
/*                                                                      */
/************************************************************************/

static B jtspsscell(J jt,A w,I wf,I wcr,A*zc,A*zt){A c,t,y;B b;
     I cn,*cv,j,k,m,n,p,*s,tn,*tv,*u,*u0,*v,*v0;P*wp;
 wp=PAV(w); s=AS(w); p=3+s[wf];
 y=SPA(wp,i); s=AS(y); m=s[0]; n=s[1];
 u0=AV(y); u=u0+n; 
 v0=u0+wf; v=v0+n;
 if(!m){*zt=*zc=mtv; R 1;}
 GATV0(t,INT,2+2*m,1); tv=AV1(t); tv[0]=tv[1]=0; tn=2;
 GATV0(c,INT,  2*m,2); cv=AV2(c); cv[0]=0;       cn=0; AS(c)[1]=2;
 for(j=1;j<m;++j){
  b=1;
  for(k=0;k<wf;++k)
   if(u0[k]!=u[k]){
    tv[tn++]=j; tv[tn++]=j; cv[1+cn]=tn-cv[cn];
    if(p==tn-cv[cn]){++cv[cn]; cv[1+cn]-=2;} 
    cn+=2;
    cv[cn]=tn-2; u0=u; v0=v; b=0;
    break;
   }
  if(b&&*v0!=*v){tv[tn++]=j; v0=v;}
  u+=n; v+=n;
 }
 tv[tn++]=m; tv[tn++]=m; cv[1+cn]=tn-cv[cn];
 if(p==tn-cv[cn]){++cv[cn]; cv[1+cn]-=2;}
 cn+=2;
 AN(t)=    AS(t)[0]=tn;   *zt=t;  /* cell divisions (row indices in y)            */
 AN(c)=cn; AS(c)[0]=cn>>1; *zc=c;  /* item divisions (indices in t, # of elements) */
 R 1;
}    /* frame: all sparse; cell: 1 or more sparse, then dense */

static A jtgrd1spz(J jt,A w,I wf,I wcr){A z;I*ws,zn;
 ws=AS(w);
 RE(zn=prod(wf+!!wcr,ws)); 
 GATV(z,INT,zn,1+wf,ws); if(!wcr)AS(z)[wf]=1;
 R z;
}    /* allocate result for grd1sp__ */

static void sp1merge0(I n,I n1,I yc,I*zv,I*xv,I*yv,I*tv){I c,d=n1-1,h,i,j,k,p,q,*v=zv;
 DO(n1, if(0==xv[i]){j=i; break;});      /* lower limit for zero items   */
 DO(n1, if(d==xv[i]){k=i; break;});      /* upper limit for zero items   */
 DO(j, *v++=yv[yc*tv[xv[i]]];);          /* items less than zero         */
 p=1+j; c=k>p?yv[yc*tv[xv[p]]]:-1;
 q=1;   h=    yv[yc*tv[q    ]]   ;       /* merge indexed zero items ... */
 for(i=0;i<n;++i)                        /* ... with omitted zero items  */
  if(h==i){++q; if(d==q)break; h=yv[yc*tv[q]];}    
  else{while(k>p&&i>c){*v++=c; ++p; c=k>p?yv[yc*tv[xv[p]]]:-1;} *v++=i;}
 DO(k-p,   *v++=yv[yc*tv[xv[p+  i]]];);  /* rest of indexed zero items   */
 DO(n-h-1, *v++=h+1+i;);                 /* rest of omitted zero items   */
 DO(d-k,   *v++=yv[yc*tv[xv[k+1+i]]];);  /* items greater than zero      */
}    /* merge grade result xv with omitted zero items into zv */

#define ADVANCE(dv)  {I j=wf; while(++dv[--j],dv[j]==ws[j])dv[j]=0;}

static A jtgrd1spss(J jt,A w,I wf,I wcr){F1PREFJT;A c,d,t,x,y,z;I cn,*cv,*dv,i,n,n1,*tv,*u,*ws,wt,*xv,yc,*yv,*zv;P*wp;
 wp=PAV(w); wt=AT(w); ws=AS(w); n=wcr?ws[wf]:1;
 RZ(z=grd1spz(w,wf,wcr)); zv=AV(z);
 SORT sortblok; SORTSP spblok;
 sortblok.f=(CMP)(wt&B01?compspssB:wt&INT?compspssI:wt&FL?compspssD:compspssZ);  // comparison function
 sortblok.jt=jtinplace;  // jt including direction bit
 x=SPA(wp,e); spblok.sev=CAV(x);
 y=SPA(wp,i); spblok.syv=yv=AV(y); spblok.syc=yc=AS(y)[1];
 x=SPA(wp,x); spblok.sxv=CAV(x);   spblok.sxc=aii(x)*(wt&CMPX?2:1);
 spblok.swf=wf;
 sortblok.sp=&spblok;  // chain sparse parms to main sortblok
 RZ(spsscell(w,wf,wcr,&c,&t));
 tv=AV(t); cv=AV(c); cn=AN(c); 
 GATV0(x,INT,2+n,1);   xv=AV1(x);  /* work area for msmerge() */
 RZ(d=apvwr(wf,0L,0L)); dv=AV(d);  /* odometer for frame      */
 for(i=0;i<cn;i+=2){
  spblok.stv=u=tv+cv[i]; n1=cv[1+i]-1;
  while(ICMP(dv,yv+yc**u,wf)){DO(n, zv[i]=i;); zv+=n; ADVANCE(dv);}
  if(u[0]<u[1]){DO(n1, zv[i]=i;); msort(&sortblok,n1,(void**)zv,(void**)xv,(I)&sortblok);}
  else         {DO(n1, xv[i]=i;); msort(&sortblok,n1,(void**)xv,(void**)zv,(I)&sortblok); sp1merge0(n,n1,yc,zv,xv,yv+wf,u);}
  zv+=n; ADVANCE(dv);
 }
 DO(AN(z)/n-(zv-AV(z))/n, DO(n, zv[i]=i;); zv+=n;);
 RE(0);
 R z;
}    /* grade"r w , sparse frame, sparse cell */

static A jtgrd1spsd(J jt,A w,I wf,I wcr){F1PREFJT;A d,t,y,z;I*dv,i,n,p,*tv,yc,*ws,*ys,*yv,*zv;P*wp;
 wp=PAV(w); ws=AS(w); n=wcr?ws[wf]:1; 
 RZ(z=grd1spz(w,wf,wcr)); zv=AV(z);
 A spax=SPA(wp,x); RZ(IRSIP1(spax,0L,wcr,jtgr1,t)); tv=AV(t);  /* grade dense cells              */
 RZ(d=apvwr(wf,0L,0L)); dv=AV(d);                 /* odometer for frame             */
 y=SPA(wp,i); ys=AS(y); p=ys[0]; yc=ys[1]; yv=AV(y);
 for(i=0;i<p;++i){                              /* now merge dense & sparse cells */
  while(ICMP(dv,yv,wf)){DO(n, zv[i]=i;); zv+=n; ADVANCE(dv);}
  DO(n, zv[i]=tv[i];);
  yv+=yc; tv+=n; zv+=n; ADVANCE(dv);
 }
 DO(AN(z)/n-(zv-AV(z))/n, DO(n, zv[i]=i;); zv+=n;);
 R z;
}    /* grade"r w , sparse frame, dense cell */

static B jtspdscell(J jt,A w,I wf,I wcr,A*zc,A*zt){A c,t,y;I*cv,m,n,p,*s,tn,*tv,*v,*v0;P*wp;
 wp=PAV(w); s=AS(w); p=3+s[wf];
 y=SPA(wp,i); s=AS(y); m=s[0]; n=s[1];
 v0=AV(y); v=v0+n; 
 if(!m){*zt=*zc=mtv; R 1;}
 GATV0(t,INT,2+m,1); tv=AV1(t); tv[0]=tv[1]=0; tn=2;
 GAT0(c,INT,2,  2); cv=AV2(c); cv[0]=0;       AS(c)[1]=2;
 DO(m-1, if(*v0!=*v){tv[tn++]=1+i; v0=v;} v+=n;);
 tv[tn++]=m; tv[tn++]=m; cv[1]=tn;
 if(p==tn){++cv[0]; cv[1]-=2;}
 AN(t)=   AS(t)[0]=tn; *zt=t;  /* cell divisions (row indices in y)            */
 AN(c)=2; AS(c)[0]=1;  *zc=c;  /* item divisions (indices in t, # of elements) */
 R 1;
}    /* frame: all dense; cell: 1 or more sparse, then dense */

static A jtgrd1spds(J jt,A w,I wf,I wcr){F1PREFJT;A c,t,x,y,z;I*cv,m,n,n1,p,*tv,*ws,wt,*xv,yc,*yv,*zv;P*wp;
 wp=PAV(w); wt=AT(w); ws=AS(w); n=wcr?ws[wf]:1; RE(m=prod(wf,ws));
 RZ(z=grd1spz(w,wf,wcr)); zv=AV(z);
 SORT sortblok; SORTSP spblok;
 sortblok.f=(CMP)(wt&B01?compspdsB:wt&INT?compspdsI:wt&FL?compspdsD:compspdsZ);  // comparison function
 sortblok.jt=jtinplace;  // jt including direction bit
 x=SPA(wp,e); spblok.sev=CAV(x);
 y=SPA(wp,i); spblok.syv=yv=AV(y); spblok.syc=yc=AS(y)[1]; 
 x=SPA(wp,x); spblok.sxv=CAV(x);   spblok.sxc=p=aii(x)*(wt&CMPX?2:1);
 sortblok.n=p/m;
 spblok.swf=wf;
 sortblok.sp=&spblok;  // chain sparse parms to main sortblok
 RZ(spdscell(w,wf,wcr,&c,&t));
 if(!AN(c)){DO(m, DO(n, zv[i]=i;); zv+=n;); R z;}
 cv=AV(c); n1=cv[1]-1; spblok.stv=tv=cv[0]+AV(t);
 GATV0(x,INT,MAX(n,1+n1),1); xv=AV1(x);  /* work area for msmerge() */
 if(cv[0])DO(m, spblok.si=i; DO(n1, zv[i]=i;); msort(&sortblok,n1,(void**)zv,(void**)xv,(I)&sortblok);                                 zv+=n;)
 else     DO(m, spblok.si=i; DO(n1, xv[i]=i;); msort(&sortblok,n1,(void**)xv,(void**)zv,(I)&sortblok); sp1merge0(n,n1,yc,zv,xv,yv,tv); zv+=n;);
 R z;
}    /* grade"r w , dense frame, sparse cell */

static A jtgrd1spdd(J jt,A w,I wf,I wcr){F1PREFJT;A x,z;I n,*ws;P*wp;
 wp=PAV(w); ws=AS(w); n=wcr?ws[wf]:1;
 x=SPA(wp,x);
 if(AN(x)){RZ(z=from(num(0),x)); R IRSIP1(z,0L,wcr,jtgr1,x);}else{R reshape(vec(INT,1+wf,ws),IX(n));}
}    /* grade"r w , dense frame, dense cell */

/* sparse right argument:                               */
/*  frame axes: all sparse or all dense                 */
/*  cell  axes: 0 or more sparse axes, then dense axes  */

F1(jtgrd1sp){F1PREFJT;PROLOG(0077);A z;B b,c,*wb;I j,m,wcr,wf,wr;P*wp;
 ARGCHK1(w);
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 wp=PAV(w);
 RZ(wb=bfi(wr,SPA(wp,a),1));
 m=0; j=wr; b=c=0; 
 DO(wf, if(wb[i])++m;); if(1<=m&&m<wf){c=1; mvc(wf,wb,1,MEMSET01);}
 DQ(wcr, --j; if(wb[j])b=1; else if(b){c=1; wb[j]=1;});
 if(c)RZ(w=reaxis(ifb(wr,wb),w));
 switch(2*wb[0]+wb[wf]){
  case 0: /* dense  dense  */ z=grd1spdd(w,wf,wcr); break;
  case 1: /* dense  sparse */ z=grd1spds(w,wf,wcr); break;
  case 2: /* sparse dense  */ z=grd1spsd(w,wf,wcr); break;
  case 3: /* sparse sparse */ z=grd1spss(w,wf,wcr); break;
 }
 EPILOG(z);
}    /* grade"r w for sparse w */


/************************************************************************/
/*                                                                      */
/* dyad /: and \: sparse right arguments                                */
/*                                                                      */
/************************************************************************/

#define SP2RENUM(ix,nx,yx,tx)  \
 {I ii,nn=(nx),pp,qq,*tu=(tx),*yu=(yx);    \
  for(ii=(ix);ii<nn;++ii,++m){             \
   pp=xv[ii]; qq=yc*(tu[pp]-1);            \
   DQ(tu[1+pp]-tu[pp], yu[qq+=yc]=m;);     \
 }}

static void sp2merge0(I n,I n1,I yc,I*zyv,I*xv,I*yv,I*tv){I c,d=n1-1,h,i,j,k,m=0,p,q;
 DO(n1, if(0==xv[i]){j=i; break;});  /* lower limit for zero items   */
 DO(n1, if(d==xv[i]){k=i; break;});  /* upper limit for zero items   */
 SP2RENUM(0,j,zyv,tv);               /* items less than zero         */
 p=1+j; c=k>p?yv[yc*tv[xv[p]]]:-1;
 q=1;   h=    yv[yc*tv[q    ]]   ;   /* merge indexed zero items ... */
 for(i=0;i<n;++i)                    /* ... with omitted zero items  */
  if(h==i){++q; if(d==q)break; h=yv[yc*tv[q]];}    
  else{while(k>p&&i>c){SP2RENUM(p,1+p,zyv,tv); ++p; c=k>p?yv[yc*tv[xv[p]]]:-1;} ++m;}
 SP2RENUM(p,  k, zyv,tv);            /* rest of indexed zero items   */
 m+=n-h-1;                           /* rest of omitted zero items   */
 SP2RENUM(1+k,n1,zyv,tv);            /* items greater than zero      */
}    /* merge grade result xv with omitted zero items into zv */

static A jtgrd2spss(J jt,A w,I wf,I wcr){F1PREFJT;A c,t,x,y,z,zy;
     I cn,*cv,i,j,m,n,n1,*tv,*u,*ws,wt,*xu,*xv,yc,*yv,*zyv;P*wp,*zp;
 RZ(z=ca(w)); zp=PAV(z);
 wp=PAV(w); wt=AT(w); ws=AS(w); n=wcr?ws[wf]:1;

 SORT sortblok; SORTSP spblok;
 sortblok.f=(CMP)(wt&B01?compspssB:wt&INT?compspssI:wt&FL?compspssD:compspssZ);  // comparison function
 sortblok.jt=jtinplace;  // jt including direction bit
 x=SPA(wp,e); spblok.sev=CAV(x);
 y=SPA(wp,i); spblok.syv=yv=AV(y); spblok.syc=yc=AS(y)[1];
 x=SPA(wp,x); spblok.sxv=CAV(x);   spblok.sxc=aii(x)*(wt&CMPX?2:1);
 spblok.swf=wf;
 sortblok.sp=&spblok;  // chain sparse parms to main sortblok
 RZ(spsscell(w,wf,wcr,&c,&t));
 tv=AV(t); cv=AV(c); cn=AN(c);
 m=0; j=1; DQ(cn, m=MAX(m,cv[j]); j+=2;);
 GATV0(x,INT,m,1); xu=AV1(x);  /* work area for msmerge() */
 GATV0(x,INT,m,1); xv=AV1(x);  /* work area for msmerge() */
 zy=SPA(zp,i); zyv=AV(zy);
 for(i=0;i<cn;i+=2){
  spblok.stv=u=tv+cv[i]; n1=cv[1+i]-1; m=0;
  DO(n1, xv[i]=i;); msort(&sortblok,n1,(void**)xv,(void**)xu,(I)&sortblok); 
  if(u[0]<u[1])SP2RENUM(0,n1,zyv+wf,u)
  else         sp2merge0(n,n1,yc,zyv+wf,xv,yv+wf,u);
 }
 RZ(x=grade1(zy)); SPB(zp,i,from(x,zy)); SPB(zp,x,from(x,SPA(zp,x)));
 R z;
}    /* sparse frame, sparse cell */

static A jtgrd2spsd(J jt,A w,I wf,I wcr){F1PREFJT;A x,z;P*zp;
 RZ(z=ca(w)); zp=PAV(z);
 x=SPA(zp,x);
 SPB(zp,x,irs2(irs1ip(x,0L,-1L,jtgr1),x,0L,1L,-1L,jtfrom));
 R z;
}    /* sparse frame, dense cell */

F2(jtgrd2sp){F1PREFJT;PROLOG(0078);A z;B b,c,*wb;I acr,af,am,ar,*as,j,m,wcr,wf,wm,wr,*ws;P*wp;
 ARGCHK2(a,w);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr; af=ar-acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 as=AS(a); am=acr?as[af]:1;
 ws=AS(w); wm=wcr?ws[wf]:1;
 ASSERT(am<=wm,EVINDEX);
 wp=PAV(w);
 RZ(wb=bfi(wr,SPA(wp,a),1));
 m=0; j=wr; b=c=0; 
 DO(wf, if(wb[i])++m;); if(1<=m&&m<wf){c=1; mvc(wf,wb,1,MEMSET01);}
 DQ(wcr, --j; if(wb[j])b=1; else if(b){c=1; wb[j]=1;});
 if(c){b=a==w; RZ(w=reaxis(ifb(wr,wb),w)); if(b)a=w;}
 switch((2*wb[0]+wb[wf])*(a==w&&af==wf&&acr==wcr)){
  default: z=irs2(IRS1(w,0L,wcr,(I)jtinplace&JTDESCEND?jtdgrade1:jtgrade1,z),a,VFLAGNONE, RMAX,acr,jtfrom); break;
  case 2: /* sparse dense  */ z=grd2spsd(w,wf,wcr); break;
  case 3: /* sparse sparse */ z=grd2spss(w,wf,wcr); break;
 } 
 EPILOG(z);
}    /* a grade"r w for sparse w */

