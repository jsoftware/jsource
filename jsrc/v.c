/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs                                                                   */

#include "j.h"


F1(jttally ){A z; I k; ARGCHK1(w); z=sc(SETIC(w,k)); RETF(AT(w)&XNUM+RAT?xco1(z):z);}  //  # y
F1(jtshapex){A z; ARGCHK1(w); VECI(z,AR(w),AS(w)); if(unlikely(((AT(w)&XNUM+RAT)!=0)))z=xco1(z); RETF(z);}
F1(jtshape){ARGCHK1(w); A z; VECI(z,AR(w),AS(w)); RETF(z);}  // $ y
F1(jtisempty){ARGCHK1(w); if(unlikely(ISSPARSE(AT(w))))R eps(zeroionei(0),shape(w)); R num(AN(w)==0);}  // 0 e. $
F1(jtisnotempty){ARGCHK1(w); if(unlikely(ISSPARSE(AT(w))))R not(eps(zeroionei(0),shape(w))); R num(AN(w)!=0);}  // *@#@,
F1(jtisitems){ARGCHK1(w); R num(!AR(w)|!!AS(w)[0]);}   // *@#   *@:#
F1(jtrank){F1PREFIP; ARGCHK1(w); R sc(AR(w));}  // #@$
F1(jtnatoms){F1PREFIP; A z; ARGCHK1(w); if(unlikely(ISSPARSE(AT(w))))R dfv1(z,shape(w),slash(ds(CSTAR))); R sc(AN(w));}   // */@$  #@,

// ,y and ,"r y - producing virtual blocks
F1(jtravel){A a,c,q,x,y,y0,z;B*b;I f,j,m,r,*u,*v,*yv;P*wp,*zp;
 F1PREFIP; ARGCHK1(w); 
 r=(RANKT)jt->ranks; r=AR(w)<r?AR(w):r; f=AR(w)-r; // r=effective rank (jt->rank is effective rank from irs1), f=frame
 if(likely(!ISSPARSE(AT(w)))){
  if(r==1)R RETARG(w);  // if we are enfiling 1-cells, there's nothing to do, return the input (note: AN of sparse array is always 1)
  CPROD(AN(w),m,r,f+AS(w));   // m=#atoms in cell
  if(ASGNINPLACESGN(SGNIF(jtinplace,JTINPLACEWX)&(-r),w) && !(AFLAG(w)&AFUNINCORPABLE)){  // inplace allowed, rank not 0 (so shape will fit), usecount is right
   // operation is loosely inplaceable.  Just shorten the shape to frame,(#atoms in cell).  We do this here rather than relying on
   // the self-virtual-block code in virtual() because we can do it for indirect types also, since we know we are not changing
   // the number of atoms
   // We preserve pristinity between the input and the output
   AR(w)=(RANKT)(1+f); AS(w)[f]=m; RETF(w);
  }
  // Not self-virtual.  Create a (noninplace) virtual copy, but not if NJA memory (because we avoid virtualing from NJA, which makes the NJA unmodifiable)
  // Transfer pristinity from w to z, if w is abandoned.  Taking the virtual block will clear prist from w
  if(!(AFLAG(w)&(AFNJA))){I wprist=PRISTFROMW(w); RZ(z=virtual(w,0,1+f)); AN(z)=AN(w); MCISH(AS(z),AS(w),f) AS(z)[f]=m; AFLAGORLOCAL(z,wprist) RETF(z);}

  // If we have to allocate a new block, do so.  In that rare case, revoke pristinity of w
  GA00(z,AT(w),AN(w),1+f); MCISH(AS(z),AS(w),f) AS(z)[f]=m;   // allocate result area, shape=frame+1 more to hold size of cell; fill in shape (don't overfetch AS(w)
  MC(AV(z),AV(w),AN(w)<<bplg(AT(w)));
  PRISTCLRF(w)
  RETF(z);   // This verb propagates WILLOPEN and must not perform EPILOG
 }
 // the rest handles sparse matrix enfile
 RESETRANK;   // clear IRS for calls made here
 RE(m=prod(r,f+AS(w)));  // # atoms in cell
 GASPARSE(z,AT(w),1,1+f,AS(w)); AS(z)[f]=m;   // allocate result area, shape=frame+1 more to hold size of cell; fill in shape
 wp=PAV(w); zp=PAV(z);
 RZ(b=bfi(AR(w),SPA(wp,a),1)); 
 if(memchr(b+f,C1,r)){
  if(memchr(b+f,C0,r)){mvc(r,b+f,1,MEMSET01); RZ(w=reaxis(ifb(AR(w),b),w)); wp=PAV(w); x=SPA(wp,x);}
  else RZ(x=ca(SPA(wp,x)));
  a=ifb(1+f,b); makewritable(a)   // avoid readonly block
  GATV0(c,INT,r,1L); v=r+AVn(1L,c); j=AR(w); m=1; DQ(r, *--v=m; m*=AS(w)[--j];);
  y0=SPA(wp,i); v=AS(y0); m=v[0]; I n=v[1];
  RZ(q=pdt(dropr(n-r,y0),c));
  GATV0(y,INT,m*(1+n-r),2); v=AS(y); v[0]=m; v[1]=1+n-r;
  yv=AV(y); u=AV(y0); v=AV(q); j=n-r;
  DQ(m, ICPY(yv,u,j); yv[j]=*v++; yv+=1+j; u+=n;);
 }else{RZ(a=ca(SPA(wp,a))); A spax=SPA(wp,x); RZ(x=IRS1(spax,0L,r,jtravel,y0)); RZ(y=ca(SPA(wp,i)));}
 SPB(zp,a,a); 
 SPB(zp,e,ca(SPA(wp,e)));
 SPB(zp,x,x);
 SPB(zp,i,y); 
 RETF(z);
}

F1(jttable){A z,zz;I r,wr;
 F1PREFIP;ARGCHK1(w);
 // We accept the pristine calculations from ravel
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r;  // r=rank to use
 RZ(IRSIP1(w,0L,r-((UI)r>0),jtravel,z));  // perform ravel on items
 R r?z:IRSIP1(z,0L,0L,jtravel,zz);  // If we are raveling atoms, do it one more time on atoms
} // ,."r y

// ]"n, dyadic - also ["n, implemented as ] with args switched
// length error has already been detected, in irs
static A jtlr2(J jt,RANK2T ranks,A a,A w){I acr,af,ar,wcr,wf,wr;
 ARGCHK2(a,w);
 // ?r=rank of ? arg; ?cr= verb-rank for that arg; ?f=frame for ?; ?s->shape
 // We know that jt->rank is nonzero, because the caller checked it
 ar=AR(a); acr=ranks>>RANKTX; acr=ar<acr?ar:acr; af=ar-acr;
 wr=AR(w); wcr=(RANKT)ranks; wcr=wr<wcr?wr:wcr;  wf=wr-wcr;
 // Cells of the shorter-frame argument are repeated.  If the shorter- (or equal-)-frame argument
 // is the one being discarded (eg (i. 10 10) ["0 i. 10), the replication doesn't matter, and we
 // simply keep the surviving argument intact.
 if(wf>=af){RETF(w);}  // no replication - quick out
 RESETRANK; a=apip(drop(sc(wf),take(sc(af),shape(a))),drop(sc(wf),shape(w))); A z; IRS2(a,w,0L,RMAX,wcr,jtreshape,z); RETF(z);  // ((wf }. af {. $a) , wf }. $w) ($,)"(_,wcr) w
} 

// ][.  Must not call EPILOG because the verb propagates WILLOPEN.  When rank is specified ]"n does not propagate
F2(jtleft2 ){F2PREFIP;RANK2T jtr=jt->ranks; if(likely(jtr==R2MAX))RETF(RETARG(a)); RETF(lr2((jtr<<RANKTX)|(jtr>>RANKTX),w,a));}  // swap a & w, and their ranks
F2(jtright2){F2PREFIP;RANK2T jtr=jt->ranks; if(likely(jtr==R2MAX))RETF(RETARG(w)); RETF(lr2(jtr,a,w));}

F1(jtright1){RETF(RETARG(w));}
// lev, dex, and ident - identity adverb/conjunction  (ident uses the same code as lev)
F2(jtlev){F2PREFIP;RETF(RETARG(a));}  F2(jtdex){F2PREFIP;RETF(RETARG(w));}



// i. y
DF1(jtiota){A z;I m,n,*v;
 F1RANK(1,jtiota,self);
 if(AT(w)&XNUM+RAT)R cvt(XNUM,iota(vi(w)));  // if extended, take integer and convert
 RZ(w=vi(w)); n=AN(w); v=AV(w);
 if(1==n){m=*v; R 0>m?apv(-m,-m-1,-1L):IX(m);}  // if list required, create it (ascending or descending) and return it
 A mg; RZ(mg=mag(w)); PRODX(m,n,IAV(mg),1); RZ(z=IX(m)); RZ(z=reshape(mag(w),z));  // rank>1.  Take */|y, create index vector, reshape to (|y) i. */|y
 DO(n, A zz; if(0>v[i])z=IRS1(z,0L,n-i,jtreverse,zz););  // reverse negative axes in input
 RETF(z);
}

// i: w
DF1(jtjico1){A y,z;B b;D d,*v;I c,m,n; 
 F1RANK(0,jtjico1,self);
 RZ(y=ccvt(FL,rect(w),0)); v=DAV(y); d=*v;  // convert to complex, d=real part of value
 RE(m=v[1]?i0(cvt(INT,tail(y))):i0(tymes(mag(w),num(2))));  // m=#steps: imaginary part if nonzero; otherwise 2*|w
 ASSERT(0<m||!m&&0==d,EVDOMAIN);  // error if imag part was negative, or 0 unless d is also 0
 n=(I)jround(d); b=FFIEQ(d,n); c=(2*ABS(n))/(m?m:1);   // try as integer
 if(b&&m*c==2*ABS(n))z=apv(1+m,-n,0>d?-c:c);  // if integer works, use it
 else                z=plusW(scf(0>d?d:-d),tymesW(scf(2*ABS(d)/m),apv(1+m,0>d?m:0L,0>d?-1L:1L)));  // otherwise FL
 if(AT(w)&XNUM+RAT)z=cvt(AT(w)&XNUM||equ(w,floor1(w))?XNUM:RAT,z);  // cvrt to XNUM as needed
 RETF(z);
}

// _9: to 9: and _:, return the saved value.  If we can inplace the operation (i. e. 0:"0), do so for DIRECT types, preserving the existing precision
DF1(jtnum1){F1PREFIP;A z=0;
 ARGCHK2(w,self); RANKT rank=(RANKT)jt->ranks; rank=rank>AR(w)?AR(w):rank; A a=FAV(self)->fgh[2];  // fetch value to store: always an INT/boolean, but if boolean the high-order bytes are 0, so 0 is valid INT/FL and 1 a valid INT
 if(rank==AR(w))R a;  // at infinite rank, just return the value.  Because VFATOP[LR] puns with comparison flags,
   // it is possible that inplacing flags are set; so we must handle infinite rank before looking at inplacing
 // rank given, must replicate the value.  if rank 0, we can do it inplace
 I natoms;  // number of atoms to allocate
 I k=bplg(AT(a));  // lg2 of size of atoms moved
 if(rank==0){
  natoms=AN(w);  // result has same # atoms as input
  if(ASGNINPLACESGN(SGNIF(jtinplace,JTINPLACEWX)&-(AT(w)&B01+INT+FL)&~((AT(w)&B01+INT+FL)-(AT(a)&B01+INT+FL)),w)){  // inplaceable, and direct numeric and type of a is not bigger than that of w
   // inplace: we will cast the atom of a to the (never smaller) size of w.  This is OK because a is never a bigger type
   // 0 can be used for any w; boolean 1 can be used as is for boolean/INT w; other values must be converted in FL.  So, if result is FL, nonzero values must be converted
   if((AT(w)>>FLX)&(IAV0(a)[0]!=0))RZ(a=ccvt(FL,a,0));  // make value conform to existing type
   z=w; k=bplg(AT(w));  // copy to the existing output area, 
  }
 }else{
  // not inplace: count the atoms of the result
  PROD(natoms,AR(w)-rank,AS(w));  // # atoms in result: 1 per cell
 }
 if(!z)GA(z,AT(a),natoms,AR(w)-rank,AS(w));  // allocate result if not inplace
 // We now have the result area, with the right type.  Fill it
 mvc(natoms<<k,voidAV(z),1LL<<k,voidAV(a));  // could use voidAV0(a)
 R z;
}
DF2(jtnum2){ARGCHK3(a,w,self); R FAV(self)->fgh[2];}

F2(jtfromr  ){ARGCHK2(a,w); A z; R IRS2(a,w,0, RMAX,1L,jtfrom  ,z);} // no agreement check because left rank is infinite - no frame  {"_ 1
F2(jtrepeatr){ARGCHK2(a,w); A z; R IRS2(a,w,0, RMAX,1L,jtrepeat,z);}  // #"_ 1

A jttaker(J jt,I n,A w){ARGCHK1(w); A a,z; RZ(a=sc(n)); R IRS2(a,w,0, RMAX,1L,jttake,z);}  // n {."1 w
A jtdropr(J jt,I n,A w){ARGCHK1(w); A a,z; RZ(a=sc(n)); R IRS2(a,w,0, RMAX,1L,jtdrop,z);}  // n }."1 w

// I. y
DF1(jticap){A a,e;I n;P*p;
 F1RANK(1,jticap,self);
 SETIC(w,n);
 if(unlikely((AT(w)&SPARSE+B01)==SPARSE+B01)){
  p=PAV(w); a=SPA(p,a); e=SPA(p,e); 
  R BAV(e)[0]||equ(mtv,a) ? repeat(w,IX(n)) : repeat(SPA(p,x),ravel(SPA(p,i)));
 }
 R likely((B01&AT(w))!=0) ? ifb(n,BAV(w)) : repeat(w,IX(n));
}

DF1(jtcharmap){F1PREFIP; A z;B bb[256];I k,n,wn;UC c,*u,*v,zz[256];
 A x=FAV(FAV(self)->fgh[2])->fgh[0], y=FAV(self)->fgh[0];  // extract translation tables
 RZ(w&&x&&y);
 if(!(LIT&AT(w)))R from(indexof(x,w),y);
 I yn=AN(y); wn=AN(w); n=MIN(AN(x),yn); u=n+UAV(x); v=n+UAV(y);
 k=256; mvc(256,bb,MEMSET00LEN,MEMSET00); if(n<yn) mvc(256,zz,1,iotavec-IOTAVECBEGIN+UAV(y)[n]);   // bb is array telling which input chars are in x; zz is result char to map for given input byte.  If not exact mapping, init z to the 'not found' char
 DQ(n, c=*--u; zz[c]=*--v; k-=(I)bb[c]^1; bb[c]=1;);   // mark characters in x, and count down to see if we hit all 256.  Note earliest mapped character for each.  Surplus chars of x ignored
 if(ASGNINPLACESGN(SGNIF(jtinplace,JTINPLACEWX),w))z=w; else{GATV(z,LIT,wn,AR(w),AS(w));} v=UAV(z); u=UAV(w);  // alloc block unless inplace and no possible error; point to input & output strings
 if(unlikely(((k-1)|(n-yn))>=0)){   // NOT(all codes mapped OR #x<#y, meaning no error possible): index error possible on {
  DO(wn, if(!bb[u[i]])R from(indexof(x,w),y);)} // Check for index error.  If error, abort through the { path to generate the right error message--don't bother trying to make this fast
 DO(wn, *v++=zz[u[i]];)  // no index error, do the translate, possibly inplace
  // Roger's code first checked to see if the translation exactly represented a bitwise op.  That seems like a lot of work for an unlikely case.  If the user wants a bitwise op, he can use
  // m b. &.(a.&i.) where we catch the case
 RETF(z);
}    /* (y {~ x i. ]) w */
