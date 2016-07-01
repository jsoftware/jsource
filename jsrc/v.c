/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs                                                                   */

#include "j.h"


F1(jttally ){A z; RZ(w); z=sc(IC(w));            R AT(w)&XNUM+RAT?xco1(z):z;}
F1(jtshapex){A z; RZ(w); z=vec(INT,AR(w),AS(w)); R AT(w)&XNUM+RAT?xco1(z):z;}
F1(jtshape ){RZ(w); R vec(INT,AR(w),AS(w));}

F1(jtravel){A a,c,q,x,y,y0,z;B*b,d;I f,j,m,n,r,*u,*v,wr,*ws,wt,*yv;P*wp,*zp;
 RZ(w); 
 n=AN(w); ws=AS(w); wt=AT(w); d=!(wt&SPARSE);
 wr=AR(w); r=jt->rank?jt->rank[1]:wr; f=wr-r; jt->rank=0;
 RE(m=prod(r,f+ws));
 GA(z,wt,d?n:1,1+f,ws); *(f+AS(z))=m; 
 if(d){MC(AV(z),AV(w),n*bp(wt)); R RELOCATE(w,z);}
 wp=PAV(w); zp=PAV(z);
 RZ(b=bfi(wr,SPA(wp,a),1)); 
 if(memchr(b+f,C1,r)){
  if(memchr(b+f,C0,r)){memset(b+f,C1,r); RZ(w=reaxis(ifb(wr,b),w)); wp=PAV(w); x=SPA(wp,x);}
  else RZ(x=ca(SPA(wp,x)));
  RZ(a=ifb(1+f,b)); 
  GA(c,INT,r,1L,0L); v=r+AV(c); j=wr; m=1; DO(r, *--v=m; m*=ws[--j];);
  y0=SPA(wp,i); v=AS(y0); m=v[0]; n=v[1];
  RZ(q=pdt(dropr(n-r,y0),c));
  GA(y,INT,m*(1+n-r),2,0); v=AS(y); v[0]=m; v[1]=1+n-r;
  yv=AV(y); u=AV(y0); v=AV(q); j=n-r;
  DO(m, ICPY(yv,u,j); yv[j]=*v++; yv+=1+j; u+=n;);
 }else{RZ(a=ca(SPA(wp,a))); RZ(x=irs1(SPA(wp,x),0L,r,jtravel)); RZ(y=ca(SPA(wp,i)));}
 SPB(zp,a,a); 
 SPB(zp,e,ca(SPA(wp,e)));
 SPB(zp,x,x);
 SPB(zp,i,y); 
 R z;
}

F1(jttable){A z;I f,r,*s,wr,*ws,wt;
 RZ(w);
 wt=AT(w); ws=AS(w); wr=AR(w); r=jt->rank?jt->rank[1]:wr; f=wr-r; jt->rank=0;
 if(wt&SPARSE){z=irs1(w,0L,r?r-1:0,jtravel); R r?z:irs1(z,0L,0L,jtravel);}
 GA(z,wt,AN(w),2+f,ws); s=f+AS(z);
 if(r)*(1+s)=prod(r-1,1+f+ws); else *s=*(1+s)=1;
 MC(AV(z),AV(w),AN(w)*bp(wt));
 R RELOCATE(w,z);
}

// ] [ and ]"n ["n, dyadic
// length error has already been detected, in irs
static A jtlr2(J jt,B left,A a,A w){A z;C*v;I acr,af,ar,k,n,of,*os,r,*s,t,
  wcr,wf,wr,zn;
 RZ(a&&w);
 // ?r=rank of ? arg; ?cr= verb-rank for that arg; ?f=frame for ?; ?s->shape
 ar=AR(a); acr=jt->rank?jt->rank[0]:ar; af=ar-acr;
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr;
 // Cells of the shorter-frame argument are repeated.  If the shorter- (or equal-)-frame argument
 // is the one being discarded (eg (i. 10 10) ["0 i. 10), the replication doesn't matter, and we
 // simply keep the surviving argument intact, and rat() to increment its use-count and call
 // for a deferred free (why?? this does not constitute a new use).  This is the normal case.
 if(left){if(af>=wf)R a; os=AS(w); r=acr; s=af+AS(a); t=AT(a); v=CAV(a); n=AN(a); of=wf; }
 else    {if(wf>=af)R w; os=AS(a); r=wcr; s=wf+AS(w); t=AT(w); v=CAV(w); n=AN(w); of=af; }
 // If the cells of the surviving arg must be replicated, do so
 // r=cell-rank, s->cell-shape, t=type, v->data, n=#atoms   of surviving arg
 // of=frame os->shape   of non-surviving arg
 // Now get size of cell of survivor, and #cells in the other (necessarily longer) frame.
 // The product of these is the number of atoms of the result
 RE(zn=mult(prod(of,os),prod(r,s)));  // #cells in non-survivor * #atoms in cell of survivor
 GA(z,t,zn,of+r,os); ICPY(of+AS(z),s,r); // allocate result; copy in nonsurviving frame+shape; overwrite cell-shape from survivor
 k=bp(t); mvc(k*zn,AV(z),k*n,v);   // get #bytes/atom, copy&replicate cells
 R z;
} 

F2(jtleft2 ){R lr2(1,a,w);}
F2(jtright2){R lr2(0,a,w);}

F1(jtright1){R rat(w);}

F1(jtiota){A z;I m,n,*v;
 F1RANK(1,jtiota,0);
 if(AT(w)&XNUM+RAT)R cvt(XNUM,iota(vi(w)));
 RZ(w=vi(w)); n=AN(w); v=AV(w);
 if(1==n){m=*v; R 0>m?apv(-m,-m-1,-1L):IX(m);}
 RE(m=prod(n,v)); z=reshape(mag(w),IX(ABS(m)));
 DO(n, if(0>v[i])z=irs1(z,0L,n-i,jtreverse););
 R z;
}

F1(jtjico1){A y,z;B b;D d,*v;I c,m,n; 
 F1RANK(0,jtjico1,0);
 RZ(y=cvt(FL,rect(w))); v=DAV(y); d=*v;
 RE(m=v[1]?i0(cvt(INT,tail(y))):i0(tymes(mag(w),num[2])));
 ASSERT(0<m||!m&&0==d,EVDOMAIN);
 n=(I)jfloor(d+0.1); b=FEQ(d,n); c=(2*ABS(n))/(m?m:1);
 if(b&&m*c==2*ABS(n))z=apv(1+m,-n,0>d?-c:c);
 else                z=plus(scf(0>d?d:-d),tymes(scf(2*ABS(d)/m),apv(1+m,0>d?m:0L,0>d?-1L:1L)));
 if(AT(w)&XNUM+RAT)z=cvt(AT(w)&XNUM||equ(w,floor1(w))?XNUM:RAT,z);
 R z;
}

DF1(jtnum1){RZ(   w&&self); R VAV(self)->h;}
DF2(jtnum2){RZ(a&&w&&self); R VAV(self)->h;}

F2(jtfromr  ){R irs2(a,w,0L,RMAX,1L,jtfrom  );}
F2(jtrepeatr){R irs2(a,w,0L,RMAX,1L,jtrepeat);}

A jttaker(J jt,I n,A w){R irs2(sc(n),w,0L,RMAX,1L,jttake);}
A jtdropr(J jt,I n,A w){R irs2(sc(n),w,0L,RMAX,1L,jtdrop);}

F1(jticap){A a,e;I n;P*p;
 F1RANK(1,jticap,0);
 n=IC(w);
 if(SB01&AT(w)){
  p=PAV(w); a=SPA(p,a); e=SPA(p,e); 
  R *BAV(e)||equ(mtv,a) ? repeat(w,IX(n)) : repeat(SPA(p,x),ravel(SPA(p,i)));
 }
 R B01&AT(w) ? ifb(n,BAV(w)) : repeat(w,IX(n));
}

A jtcharmap(J jt,A w,A x,A y){A z;B bb[256];I k,n,wn;UC c,*u,*v,zz[256];
 RZ(w&&x&&y);
 if(!(LIT&AT(w)))R from(indexof(x,w),y);
 wn=AN(w); n=MIN(AN(x),AN(y)); u=n+UAV(x); v=n+UAV(y);
 k=256; memset(bb,C0,256); if(n<AN(y))memset(zz,*(n+UAV(y)),256);
 DO(n, c=*--u; zz[c]=*--v; if(!bb[c]){--k; bb[c]=1;});
 GA(z,LIT,wn,AR(w),AS(w)); v=UAV(z); u=UAV(w);
 if(k&&n==AN(y))DO(wn, c=*u++; ASSERT(bb[c],EVINDEX); *v++=zz[c];)
 else if(!bitwisecharamp(zz,wn,u,v))DO(wn, *v++=zz[*u++];);
 R z;
}    /* y {~ x i. w */
