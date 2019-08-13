/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Fill-Dependent Verbs                                             */

#include "j.h"

// a and w (which may be the same) are 2 nouns.  We pick the fill based  on the types of a/w (a first, then w, skipping a if empty)
// if jt->fill is set, we use that value instead
// we put one atom of fill into jt->fillv0 and point jt->fillv to that atom
// w is expected to be the reference input; if it is relative, we relocate jt->fillv to be wrt w
F2(jtsetfv){A q=jt->fill;I t;
 RZ(a&&w);
 t=AN(a)?AT(a):AN(w)?AT(w):0;
 if(q&&AN(q)){
  RE(t=t?maxtype(t,AT(q)):AT(q)); 
  if(TYPESNE(t,AT(q)))RZ(q=cvt(t,q));
  jt->fillv=CAV(q);   // jt->fillv points to the fill atom
 }else{if(!t)t=AT(w); fillv(t,1L,jt->fillv0); jt->fillv=jt->fillv0;}    // empty fill.  move 1 std fill atom to fillv0 and point jt->fillv at it
 R TYPESEQ(t,AT(w))?w:cvt(t,w);  // note if w is boxed this won't change it, so relo is still valid
}

F1(jtfiller){A z; RZ(w); GA(z,AT(w),1,0,0); fillv(AT(w),1L,CAV(z)); R z;}

// move n fills of type t to *v
void jtfillv(J jt,I t,I n,C*v){I k=bpnoun(t);
 switch(CTTZ(t)){
 case RATX: mvc(n*k,v,k,&zeroQ); break;
 case XNUMX: mvc(n*k,v,k,&iv0); break;
 case B01X: case BITX: case INTX: case FLX: case CMPXX: case XDX: case XZX: case SB01X: case SFLX: case SCMPXX: case SBTX:
  memset(v,C0,k*n); break;
 case LITX: memset(v,' ',n); break;
 case C2TX: {US x=32; mvc(n*k,v,k,&x); break;}
 case C4TX: {C4 x=32; mvc(n*k,v,k,&x); break;}
 default: mvc(n*k,v,k,&mtv);
 }
}


static F2(jtrotsp){PROLOG(0071);A q,x,y,z;B bx,by;I acr,af,ar,*av,d,k,m,n,p,*qv,*s,*v,wcr,wf,wr;P*wp,*zp;
 RZ(a&&w);
 ASSERT(!jt->fill,EVNONCE);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr; af=ar-acr; p=acr?*(af+AS(a)):1;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 if(1<acr||af)R df2(a,w,qq(qq(ds(CROT),v2(1L,RMAX)),v2(acr,wcr)));
 if(!wcr&&1<p){RZ(w=reshape(over(shape(w),apv(p,1L,0L)),w)); wr=wcr=p;}
 ASSERT(!wcr||p<=wcr,EVLENGTH);
 s=AS(w);
 GATV0(q,INT,wr,1L); qv=AV(q); memset(qv,C0,wr*SZI); 
 RZ(a=vi(a)); v=AV(a); 
 DO(p, k=v[i]; d=s[wf+i]; qv[wf+i]=!d?0:0<k?k%d:k==IMIN?d-(-d-k)%d:d-(-k)%d;);
 wp=PAV(w); a=SPA(wp,a); RZ(y=ca(SPA(wp,i))); m=IC(y);
 n=AN(a); RZ(a=paxis(wr,a)); av=AV(a);
 RZ(q=from(a,q)); qv=AV(q);
 GASPARSE(z,AT(w),1,wr,s); zp=PAV(z);
 by=0; DO(n,    if(qv[  i]){by=1; break;});
 bx=0; DO(wr-n, if(qv[n+i]){bx=1; break;});
 RZ(x=!bx?ca(SPA(wp,x)):irs2(vec(INT,wr-n,n+qv),SPA(wp,x),0L,1L,-1L,jtrotate));
 if(by){
  DO(n, if(k=qv[i]){d=s[av[i]]-k; v=i+AV(y); DQ(m, *v<k?(*v+=d):(*v-=k); v+=n;);});
  RZ(q=grade1(y)); RZ(y=from(q,y)); RZ(x=from(q,x));
 }
 SPB(zp,a,ca(SPA(wp,a))); 
 SPB(zp,e,ca(SPA(wp,e))); 
 SPB(zp,x,x);
 SPB(zp,i,y);
 EPILOG(z);
}    /* a|."r w on sparse arrays */

#define ROF(r) if((I )(r<-n)|(I )(r>n))r=(r<0)?-n:n; x=dk*ABS(r); y=e-x; j=0>r?y:x; k=0>r?x:y;
#define ROT(r) if((I )(r<-n)|(I )(r>n))r=r%n;             x=dk*ABS(r); y=e-x; j=0>r?y:x; k=0>r?x:y;

// m=#cells d=#atoms per item  n=#items per cell
static void jtrot(J jt,I m,I d,I n,I atomsize,I p,I*av,C*u,C*v){I dk,e,k,j,r,x,y;
 e=n*d*atomsize; dk=d*atomsize; // e=#bytes per cell  dk=bytes per item
 if(jt->fill){
  if(p<=1){r=p?*av:0;     ROF(r); DQ(m, if(r<0){mvc(k,v,atomsize,jt->fillv); MC(k+v,u,j);}else{MC(v,j+u,k); mvc(j,k+v,atomsize,jt->fillv);}        u+=e; v+=e;);}
  else{DO(m, r=av[i]; ROF(r);       if(r<0){mvc(k,v,atomsize,jt->fillv); MC(k+v,u,j);}else{MC(v,j+u,k); mvc(j,k+v,atomsize,jt->fillv);}            u+=e; v+=e;);}
 }else{
  if(p<=1){r=p?*av:0;     ROT(r); DQ(m, MC(v,j+u,k); MC(k+v,u,j); u+=e; v+=e;);}
  else{DO(m, r=av[i]; ROT(r);       MC(v,j+u,k); MC(k+v,u,j); u+=e; v+=e;);}
 }
}

/* m   # cells
   c   # atoms in each cell
   n   # items in each cell
   k   # bytes in each atom
   p   length of av
   av  rotation amount(s)
   u   source data area 
   v   target data area      */

F2(jtrotate){A y,z;B b;C*u,*v;I acr,af,ar,*av,d,k,m,n,p,*s,wcr,wf,wn,wr;
 RZ(a&&w);
 if(SPARSE&AT(w))R rotsp(a,w);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr; af=ar-acr; p=acr?*(af+AS(a)):1;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 RZ(a=vi(a));
 // special case: if a is atomic 0, and cells of w are not atomic
 if((wcr!=0)&(((ar|IAV(a)[0])==0)))R RETARG(w);   // 0 |. y, return y
 if(((1-acr)|((-af)&(-acr|(wf-1))))<0)R df2(a,w,qq(qq(ds(CROT),v2(1L,RMAX)),v2(acr,wcr)));  // if multiple a-lists per cell, or a has frame and (a cell is not an atom or w has frame) handle rank by using " for it
 if(((wcr-1)&(1-p))<0){RZ(w=reshape(over(shape(w),apv(p,1L,0L)),w)); wr=wcr=p;}  // if cell is an atom, extend it up to #axes being rotated   !wcr && p>1
 ASSERT(((-wcr)&(wcr-p))>=0,EVLENGTH);    // !wcr||p<=wcr  !(wcr&&p>wcr)
 av=AV(a);
 RZ(w=setfv(w,w)); u=CAV(w); wn=AN(w); s=AS(w); k=bpnoun(AT(w));  // set fill value if given
 GA(z,AT(w),wn,wr,s); v=CAV(z);
 if(!wn)R z;
 PROD(m,wf,s); PROD1(d,wr-wf-1,s+wf+1); n=wcr?s[wf]:1;  // m=#cells of w, n=#items per cell  d=#atoms per item of cell
 rot(m,d,n,k,1>=p?AN(a):1L,av,u,v);
 if(1<p){
  GA(y,AT(w),wn,wr,s); u=CAV(y); 
  b=0; s+=wf;
  DO(p-1, m*=n; n=*++s; PROD(d,wr-wf-i-2,s+1); rot(m,d,n,k,1L,av+i+1,b?u:v,b?v:u); b^=1;);  // s has moved past the frame
  z=b?y:z;
 } 
 RETF(z);
}    /* a|.!.f"r w */


static F1(jtrevsp){A a,q,x,y,z;I c,f,k,m,n,r,*v,wr;P*wp,*zp;
 RZ(w);
 ASSERT(!jt->fill,EVNONCE);
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r; RESETRANK;
 m=*(f+AS(w)); wp=PAV(w);
 GASPARSE(z,AT(w),1,wr,AS(w)); zp=PAV(z);
 a=SPA(wp,a); n=AN(a); RZ(y=ca(SPA(wp,i))); x=SPA(wp,x);
 RZ(q=paxis(wr,a)); v=AV(q); DO(wr, if(f==v[i]){k=i; break;});
 if(!r)       RZ(x=ca(x))
 else if(k>=n)RZ(x=irs2(apv(m,m-1,-1L),x,0L,1L,wr-k,jtfrom))
 else         {v=k+AV(y); c=m-1; DQ(IC(y), *v=c-*v; v+=n;); q=grade1(y); RZ(y=from(q,y)); RZ(x=from(q,x));}
 SPB(zp,a,ca(a)); 
 SPB(zp,e,ca(SPA(wp,e))); 
 SPB(zp,i,y); 
 SPB(zp,x,x);
 R z;
}    /* |."r w on sparse arrays */

F1(jtreverse){A z;C*wv,*zv;I f,k,m,n,nk,r,*v,*ws,wt,wr;
 RZ(w);
 if(SPARSE&AT(w))R revsp(w);
 if(jt->fill)R rotate(num[-1],w);  // rank is set
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r;  // no RESETRANK - we don't call any primitive from here on
 if(!(r&&AN(w))){R RETARG(w);}  // no atoms or reversing atoms - keep input unchanged
 wt=AT(w); ws=AS(w); wv=CAV(w);
 n=ws[f]; 
 m=1; DO(f, m*=ws[i];);
 k=bpnoun(wt); v=1+f+ws; DQ(r-1, k*=*v++;); nk=n*k;
 GA(z,wt,AN(w),wr,ws); zv=CAV(z);
 switch(k){
  default:        {C*s=wv-k,*t; DQ(m, t=s+=nk; DQ(n, MC(zv,t,k); zv+=k; t-=k;););} break;
  case sizeof(C): {C*s=    wv,*t,*u=    zv; DQ(m, t=s+=n; DQ(n, *u++=*--t;););} break;
  case sizeof(S): {S*s=(S*)wv,*t,*u=(S*)zv; DQ(m, t=s+=n; DQ(n, *u++=*--t;););} break;
#if SY_64
  case sizeof(int):{int*s=(int*)wv,*t,*u=(int*)zv; DQ(m, t=s+=n; DQ(n, *u++=*--t;););} break;
#endif
  case sizeof(I): {I*s=(I*)wv,*t,*u=(I*)zv; DQ(m, t=s+=n; DQ(n, *u++=*--t;););} break;
#if !SY_64 && SY_WIN32
  case sizeof(D): {D*s=(D*)wv,*t,*u=(D*)zv; DQ(m, t=s+=n; DQ(n, *u++=*--t;););} break;
#endif
 }
 RETF(z);
}    /* |."r w */


static A jtreshapesp0(J jt,A a,A w,I wf,I wcr){A e,p,x,y,z;B*b,*pv;I c,d,r,*v,wr,*ws;P*wp,*zp;
 wr=AR(w); ws=AS(w);
 wp=PAV(w); RZ(b=bfi(wr,SPA(wp,a),1));
 RZ(e=ca(SPA(wp,e))); x=SPA(wp,x); y=SPA(wp,i);
 v=AS(y); r=v[0]; c=v[1]; d=0; DO(wf, if(b[i])++d;);
 if(!wf){if(r&&c){v=AV(y); DO(c, if(v[i])R e;);} R AN(x)?reshape(mtv,x):e;}
 GASPARSE(z,AT(w),1,wf,ws);
 zp=PAV(z); SPB(zp,e,e); SPB(zp,a,caro(ifb(wf,b)));  // avoid readonly
 GATV0(p,B01,r,1); pv=BAV(p);
 v=AV(y); 
 DO(r, *pv=1; DO(c-d, if(v[d+i]){*pv=0; break;}); ++pv; v+=c;);
 SPB(zp,i,repeat(p,taker(d,y)));
 SPB(zp,x,irs2(mtv,repeat(p,x),0L,1L,wcr-(c-d),jtreshape));
 R z;
}    /* '' ($,)"wcr w for sparse w */

static A jtreshapesp(J jt,A a,A w,I wf,I wcr){A a1,e,t,x,y,z;B az,*b,wz;I an,*av,c,d,j,m,*u,*v,wr,*ws;P*wp,*zp;
 RZ(a=cvt(INT,a)); an=AN(a); av=AV(a); wr=AR(w); ws=AS(w); d=an-wcr;
 az=0; DO(an,  if(!av[   i])az=1;);
 wz=0; DO(wcr, if(!ws[wf+i])wz=1;);
 ASSERT(az||!wz,EVLENGTH);
 if(!an)R reshapesp0(a,w,wf,wcr);
 wp=PAV(w); a1=SPA(wp,a); c=AN(a1); RZ(b=bfi(wr,a1,1));  // b=bitmask, length wr, with 1s for each value in a1
 RZ(e=ca(SPA(wp,e))); x=SPA(wp,x); y=SPA(wp,i);
 u=av+an; v=ws+wr; m=0; DQ(MIN(an,wcr-1), if(*--u!=*--v){m=1; break;});
 if(m||an<wcr) R reshapesp(a,IRS1(w,0L,wcr,jtravel,z),wf,1L);
 ASSERT(!jt->fill,EVDOMAIN);
 GASPARSE(z,AT(w),1,wf+an,ws); MCISH(wf+AS(z),av,an);
 zp=PAV(z); SPB(zp,e,e);  
 GATV0(t,INT,c+d*b[wf],1); v=AV(t); 
 DO(wf, if(b[i])*v++=i;); if(b[wf])DO(d, *v++=wf+i;); j=wf; DQ(wcr, if(b[j])*v++=d+j; ++j;);
 SPB(zp,a,t);
 if(b[wf]){I n,q,r,*v0;   /* sparse */
  if(wf!=*AV(a1))R rank2ex(a,w,0L,MIN(AR(a),1),wcr,MIN(AR(a),1),wcr,jtreshape);
  RE(m=prod(1+d,av)); n=IC(y); if(ws[wf]){q=n*(m/ws[wf]); r=m%ws[wf];} else {q=0; r=0;}
  v=AV(y); DQ(n, if(r<=*v)break; ++q; v+=c;);
  GATV0(t,INT,q,1); u=AV(t); v=v0=AV(y);
  m=j=0; DO(q, u[i]=m+*v; v+=c; ++j; if(j==n){j=0; v=v0; m+=ws[wf];});
  SPB(zp,i,stitch(abase2(vec(INT,1+d,av),t),reitem(sc(q),dropr(1L,y))));
  SPB(zp,x,reitem(sc(q),x));
 }else{                   /* dense  */
  GATV0(t,INT,an,1); v=AV(t); MCISH(v,av,d); m=d; j=wf; DO(wcr, if(!b[j++])v[m++]=av[i+d];);
  SPB(zp,i,ca(y));
  SPB(zp,x,irs2(vec(INT,m,v),x,0L,1L,wcr-(an-m),jtreshape));
 }
 R z;
}    /* a ($,)"wcr w for sparse w and scalar or vector a */

F2(jtreshape){A z;B filling;C*wv,*zv;I acr,ar,c,k,m,n,p,q,r,*s,t,* RESTRICT u,wcr,wf,wn,wr,* RESTRICT ws,zn;
 F2PREFIP;
 RZ(a&&w);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; ws=AS(w); RESETRANK;
 if((I )(1<acr)|(I )(acr<ar))R rank2ex(a,w,0L,MIN(acr,1),wcr,acr,wcr,jtreshape);
 // now a is an atom or a list.  w can have any rank
 RZ(a=vip(a)); r=AN(a); u=AV(a);   // r=length of a   u->values of a
 if(SPARSE&AT(w)){RETF(reshapesp(a,w,wf,wcr));}
 wn=AN(w); RE(m=prod(r,u)); CPROD(wn,c,wf,ws); CPROD(wn,n,wcr,wf+ws);  // m=*/a (#atoms in result)  c=#cells of w  n=#atoms/cell of w
 ASSERT(n||!m||jt->fill,EVLENGTH);  // error if attempt to extend array of no items to some items without fill
 t=AT(w); filling = 0;
 if(m<=n){  // no wraparound
  if(c==1) {  // if there is only 1 cell of w...
   // If no fill required, we can probably use a virtual result, or maybe even an inplace one.  Check for inplace first.  Mustn't inplace an indirect that shortens the data,
   // because then who would free the blocks?  (Actually it would be OK if nonrecursive, but we are trying to exterminate those)
   if(ASGNINPLACESGN(SGNIF((I)jtinplace,JTINPLACEWX)&(r-(wcr+1))&((n-(m+1))|-(t&DIRECT)),w)){  //  inplace allowed, just one cell, result rank (an) <= current rank (so rank fits), usecount is right
    // operation is loosely inplaceable.  Copy in the rank, shape, and atom count.
    AR(w)=(RANKT)(r+wf); AN(w)=m; ws+=wf; MCISH(ws,u,r) RETF(w);   // Start the copy after the (unchanged) frame
   }
   // Not inplaceable.  Create a (noninplace) virtual copy, but not if NJA memory
// correct   if(!(AFLAG(w)&(AFNJA))){RZ(z=virtual(w,0,r+wf)); AN(z)=m; I *zs=AS(z); DO(wf, *zs++=ws[i];); DO(r, zs[i]=u[i];) RETF(z);}
   if((SGNIF(AFLAG(w),AFNJAX)|((t&(DIRECT|RECURSIBLE))-1))>=0){RZ(z=virtual(w,0,r+wf)); AN(z)=m; I * RESTRICT zs=AS(z); MCISH(zs,ws,wf) MCISH(zs+wf,u,r) RETF(z);}
   // for NJA/SMM, fall through to nonvirtual code
  }
 }else if(filling=jt->fill!=0){RZ(w=setfv(w,w)); t=AT(w);}   // if fill required, set fill value.  Remember if we need to fill
 k=bpnoun(t); p=k*m; q=k*n;
 RE(zn=mult(c,m));
 GA(z,t,zn,r+wf,0); s=AS(z); MCISH(s,ws,wf); MCISH(s+wf,u,r);
 if(!zn)R z;
 zv=CAV(z); wv=CAV(w); 
 if(filling)DQ(c, mvc(q,zv,q,wv); mvc(p-q,q+zv,k,jt->fillv); zv+=p; wv+=q;)
 else DQ(c, mvc(p,zv,q,wv); zv+=p; wv+=q;);
 RETF(z);
}    /* a ($,)"r w */

F2(jtreitem){A y,z;I acr,an,ar,r,*v,wcr,wr;
 F2PREFIP;
 RZ(a&&w);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; r=wcr-1; RESETRANK;
 if((I )(1<acr)|(I )(acr<ar))R rank2ex(a,w,0L,MIN(acr,1),wcr,acr,wcr,jtreitem);  // We handle only single operations here, where a has rank<2
 // acr<=ar; ar<=acr; therefore ar==acr here
 fauxblockINT(yfaux,4,1);
 if(1>=wcr)y=a;  // y is atom or list: $ is the same as ($,)
 else{   // rank y > 1: append the shape of an item of y to x
  RZ(a=vi(a)); an=AN(a); acr=1;  // if a was an atom, now it is a list
  fauxINT(y,yfaux,an+r,1) v=AV(y);
  MCISH(v,AV(a),an); MCISH(v+an,AS(w)+wr-r,r);
 }
 R wr==wcr?jtreshape(jtinplace,y,w):IRS2(y,w,0L,acr,wcr,jtreshape,z);  // Since a has no frame, we dont have to check agreement
}    /* a $"r w */

#define EXPAND(T)  \
  {T*u=(T*)wv,*v=(T*)zv,x;                                                \
   mvc(sizeof(T),&x,k,jt->fillv);                                         \
   DQ(an, I abit=*av++; T *uv=abit?u:&x; *v++=*uv; u+=abit;);  \
  }

F2(jtexpand){A z;B*av;C*wv,*wx,*zv;I an,*au,i,k,p,wc,wk,wn,wt,zn;
 RZ(a&&w);
 if(!(B01&AT(a)))RZ(a=cvt(B01,a));
 ASSERT(1==AR(a),EVRANK);
 RZ(w=setfv(w,w)); 
 if(!AR(w))R from(a,take(num[-2],w));  // atomic w, use a { _2 {. w
 av=BAV(a); an=AN(a); au=(I*)av;
 ASSERT(bsum(an,av)==AS(w)[0],EVLENGTH);  // each item of w must be used exactly once
 wv=CAV(w); wn=AN(w); wc=aii(w); wt=AT(w); k=bpnoun(wt); wk=k*wc; wx=wv+wk*AS(w)[0];  // k=bytes/atom, wk=bytes/item, wx=end+1 of area
 RE(zn=mult(an,wc));
 GA(z,wt,zn,AR(w),AS(w)); AS(z)[0]=an; zv=CAV(z);
 switch(wk){
  case sizeof(C): EXPAND(C); break;
  case sizeof(S): EXPAND(S); break;
#if SY_64
  case sizeof(int): EXPAND(int); break;
#endif
  case sizeof(I): EXPAND(I); break;
  default:  
   mvc(k*zn,zv,k,jt->fillv); // here we are trying to minimize calls to MC
   for(i=p=0;i<an;++i)
    if(*av++)p+=wk; 
    else{if(p){MC(zv,wv,p); wv+=p; zv+=p; p=0;} zv+=wk;}
   if(p){MC(zv,wv,p);}
 }
 RETF(z);
}    /* a&#^:_1 w or a&#^:_1!.f w */



