/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs                                                                   */

#include "j.h"


F1(jttally ){A z; RZ(w); z=sc(IC(w));            RETF(AT(w)&XNUM+RAT?xco1(z):z);}
F1(jtshapex){A z; RZ(w); z=vec(INT,AR(w),AS(w)); RETF(AT(w)&XNUM+RAT?xco1(z):z);}
F1(jtshape){RZ(w); R vec(INT,AR(w),AS(w));}
F1(jtisempty){RZ(w); if(AT(w)&SPARSE)R eps(zeroionei[0],shape(w)); R num[AN(w)==0];}
F1(jtisnotempty){RZ(w); if(AT(w)&SPARSE)R not(eps(zeroionei[0],shape(w))); R num[AN(w)!=0];}
F1(jtisitems){RZ(w); R num[!AR(w)||AS(w)[0]];}
F1(jtrank){F1PREFIP; RZ(w); R sc(AR(w));}
F1(jtnatoms){F1PREFIP; RZ(w); if(AT(w)&SPARSE)R df1(shape(w),slash(ds(CPLUS))); R sc(AN(w));}

// ,y and ,"r y - producing virtual blocks
F1(jtravel){A a,c,q,x,y,y0,z;B*b;I f,j,m,r,*u,*v,*yv;P*wp,*zp;
 F1PREFIP; RZ(w); 
 r=(RANKT)jt->ranks; r=AR(w)<r?AR(w):r; f=AR(w)-r; // r=effective rank (jt->rank is effective rank from irs1), f=frame
 if(!(AT(w)&SPARSE)){
  if(r==1)R RETARG(w);  // if we are enfiling 1-cells, there's nothing to do, return the input (note: AN of sparse array is always 1)
  CPROD(AN(w),m,r,f+AS(w));   // m=#atoms in cell
  if(ASGNINPLACESGN(SGNIF((I)jtinplace,JTINPLACEWX)&(-r),w) && !(AFLAG(w)&AFUNINCORPABLE)){  // inplace allowed, rank not 0 (so shape will fit), usecount is right
   // operation is loosely inplaceable.  Just shorten the shape to frame,(#atoms in cell).  We do this here rather than relying on
   // the self-virtual-block code in virtual() because we can do it for indirect types also, since we know we are not changing
   // the number of atoms
   AR(w)=(RANKT)(1+f); AS(w)[f]=m; RETF(w);  // if virtual inplace, notify the originator
  }
  // Not inplaceable.  Create a (noninplace) virtual copy, but not if NJA memory
  if(!(AFLAG(w)&(AFNJA))){RZ(z=virtual(w,0,1+f)); AN(z)=AN(w); MCISH(AS(z),AS(w),f) AS(z)[f]=m; RETF(z);}

  // If we have to allocate a new block, do so
  GA(z,AT(w),AN(w),1+f,AS(w)); AS(z)[f]=m;   // allocate result area, shape=frame+1 more to hold size of cell; fill in shape
  MC(AV(z),AV(w),AN(w)<<bplg(AT(w))); RETF(z); // if dense, move the data and relocate it as needed
 }
 // the rest handles sparse matrix enfile
 RESETRANK;   // clear IRS for calls made here
 RE(m=prod(r,f+AS(w)));  // # atoms in cell
 GASPARSE(z,AT(w),1,1+f,AS(w)); AS(z)[f]=m;   // allocate result area, shape=frame+1 more to hold size of cell; fill in shape
 wp=PAV(w); zp=PAV(z);
 RZ(b=bfi(AR(w),SPA(wp,a),1)); 
 if(memchr(b+f,C1,r)){
  if(memchr(b+f,C0,r)){memset(b+f,C1,r); RZ(w=reaxis(ifb(AR(w),b),w)); wp=PAV(w); x=SPA(wp,x);}
  else RZ(x=ca(SPA(wp,x)));
  RZ(a=caro(ifb(1+f,b)));   // avoid readonly block
  GATV0(c,INT,r,1L); v=r+AV(c); j=AR(w); m=1; DQ(r, *--v=m; m*=AS(w)[--j];);
  y0=SPA(wp,i); v=AS(y0); m=v[0]; I n=v[1];
  RZ(q=pdt(dropr(n-r,y0),c));
  GATV0(y,INT,m*(1+n-r),2); v=AS(y); v[0]=m; v[1]=1+n-r;
  yv=AV(y); u=AV(y0); v=AV(q); j=n-r;
  DQ(m, ICPY(yv,u,j); yv[j]=*v++; yv+=1+j; u+=n;);
 }else{RZ(a=ca(SPA(wp,a))); RZ(x=IRS1(SPA(wp,x),0L,r,jtravel,y0)); RZ(y=ca(SPA(wp,i)));}
 SPB(zp,a,a); 
 SPB(zp,e,ca(SPA(wp,e)));
 SPB(zp,x,x);
 SPB(zp,i,y); 
 RETF(z);
}

F1(jttable){A z,zz;I r,wr;
 RZ(w);F1PREFIP;
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r;  // r=rank to use
 RZ(IRSIP1(w,0L,r-1<0?0:r-1,jtravel,z));  // perform ravel on items
 R r?z:IRSIP1(z,0L,0L,jtravel,zz);  // If we are raveling atoms, do it one more time on atoms
} // ,."r y

// ] [ and ]"n ["n, dyadic
// length error has already been detected, in irs
static A jtlr2(J jt,B left,A a,A w){A z;C*v;I acr,af,ar,k,n,of,*os,r,*s,t,
  wcr,wf,wr,zn;
 RZ(a&&w);
 // ?r=rank of ? arg; ?cr= verb-rank for that arg; ?f=frame for ?; ?s->shape
 // We know that jt->rank is nonzero, because the caller checked it
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr; af=ar-acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr;  wf=wr-wcr;  // RESETRANK not required because we call no primitives from here on,. 
 // Cells of the shorter-frame argument are repeated.  If the shorter- (or equal-)-frame argument
 // is the one being discarded (eg (i. 10 10) ["0 i. 10), the replication doesn't matter, and we
 // simply keep the surviving argument intact.  We can do this because we have no PROLOG
 if(left){if(af>=wf){RETF(a);} os=AS(w); r=acr; s=af+AS(a); t=AT(a); v=CAV(a); n=AN(a); of=wf; }
 else    {if(wf>=af){RETF(w);} os=AS(a); r=wcr; s=wf+AS(w); t=AT(w); v=CAV(w); n=AN(w); of=af; }
 // If the cells of the surviving arg must be replicated, do so
 // r=cell-rank, s->cell-shape, t=type, v->data, n=#atoms   of surviving arg
 // of=frame os->shape   of non-surviving arg
 // Now get size of cell of survivor, and #cells in the other (necessarily longer) frame.
 // The product of these is the number of atoms of the result
 RE(zn=mult(prod(of,os),prod(r,s)));  // #cells in non-survivor * #atoms in cell of survivor
 GA(z,t,zn,of+r,os); MCISH(of+AS(z),s,r); // allocate result; copy in nonsurviving frame+shape; overwrite cell-shape from survivor
 k=bpnoun(t); mvc(k*zn,AV(z),k*n,v);   // get #bytes/atom, copy&replicate cells
 RETF(z);
} 

F2(jtleft2 ){F2PREFIP;if(jt->ranks==(RANK2T)~0)RETF(a); RETF(lr2(1,a,w));}
F2(jtright2){F2PREFIP;if(jt->ranks==(RANK2T)~0)RETF(w); RETF(lr2(0,a,w));}

F1(jtright1){RETF(w);}

F1(jtiota){A z;I m,n,*v;
 F1RANK(1,jtiota,0);
 if(AT(w)&XNUM+RAT)R cvt(XNUM,iota(vi(w)));
 RZ(w=vi(w)); n=AN(w); v=AV(w);
 if(1==n){m=*v; R 0>m?apv(-m,-m-1,-1L):IX(m);}
 RE(m=prod(n,v)); z=reshape(mag(w),IX(ABS(m)));
 DO(n, A zz; if(0>v[i])z=IRS1(z,0L,n-i,jtreverse,zz););
 RETF(z);
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
 RETF(z);
}

DF1(jtnum1){RZ(   w&&self); R FAV(self)->fgh[2];}
DF2(jtnum2){RZ(a&&w&&self); R FAV(self)->fgh[2];}

F2(jtfromr  ){RZ(a&&w); A z; R IRS2(a,w,0, RMAX,1L,jtfrom  ,z);} // no agreement check because left rank is infinite - no frame
F2(jtrepeatr){RZ(a&&w); A z; R IRS2(a,w,0, RMAX,1L,jtrepeat,z);}

A jttaker(J jt,I n,A w){RZ(w); A a,z; RZ(a=sc(n)); R IRS2(a,w,0, RMAX,1L,jttake,z);}
A jtdropr(J jt,I n,A w){RZ(w); A a,z; RZ(a=sc(n)); R IRS2(a,w,0, RMAX,1L,jtdrop,z);}

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
 DQ(n, c=*--u; zz[c]=*--v; if(!bb[c]){--k; bb[c]=1;});
 GATV(z,LIT,wn,AR(w),AS(w)); v=UAV(z); u=UAV(w);
 if(k&&n==AN(y))DQ(wn, c=*u++; ASSERT(bb[c],EVINDEX); *v++=zz[c];)
 else if(!bitwisecharamp(zz,wn,u,v))DQ(wn, *v++=zz[*u++];);
 RETF(z);
}    /* y {~ x i. w */
