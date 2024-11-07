/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Fill-Dependent Verbs                                             */

#include "j.h"

// a and w (which may be the same) are 2 nouns.  We pick the fill based on the types of a/w (a first, then w, skipping a if empty)
// if jt->fill is set, we use that value instead
// we put one atom of fill into jt->fillv0 and point jt->fillv to that atom
// if w is not the same type as the fill, convert it.  The user has to handle a.
// if w and a have the same type, w will not be converted
// Result is possibly-converted w
F2(jtsetfv){A q=jt->fill;I t;
 ARGCHK2(a,w);
 q=q?q:mtv;  // if no fill given, use empty vector
 I t2=REPSGN(-AN(w))&AT(w); t=REPSGN(-AN(a))&AT(a); t=t?t:t2;  // ignoring empties, use type of a then w
 if(unlikely(AN(q)!=0)){ // fill specified
  RE(t=t?maxtype(t,AT(q)):AT(q)); // get type needed for fill
  if(TYPESNE(t,AT(q))){if((q=cvt(t,q))==0){if(jt->jerr==EVDOMAIN)jt->jerr=EVINHOMO; R 0;}}  // convert the user's type if needed; call it INHOMO if incompatible
  jt->fillv=CAV(q); jt->fillvlen=bpnoun(t);  // jt->fillv points to the fill atom
 }else{if(!t)t=AT(w); fillv0(t);}    // empty or no fill.  create std fill in fillv0 and point jt->fillv at it
 w=TYPESEQ(t,AT(w))?w:cvt(t,w);  // note if w is boxed and nonempty this won't change it
 if(w==0&&jt->jerr==EVDOMAIN)jt->jerr=EVINHOMO; // if we got an error here (always called DOMAIN), show it as EVHOMO when we eformat
 R w;
}
// simpler version when there is only w.  We make AT(w) an argument to save a fetch
A jtsetfv1(J jt, A w, I t){A q=jt->fill;
 ARGCHK1(w);
 q=q?q:mtv;  // if no fill given, use empty vector.  No fill type specified
 if(unlikely(AN(q)!=0)){ // fill specified
  t=AN(w)?t:B01; // if w empty, give it the lowest priority
  RE(t=maxtype(t,AT(q))); // get type needed for fill
  if(TYPESNE(t,AT(q))){if(unlikely((q=cvt(t,q))==0)){if(jt->jerr==EVDOMAIN)jt->jerr=EVINHOMO; R 0;}}  // convert the user's type if needed; call it INHOMO if incompatible
  else if(unlikely(TYPESNE(t,AT(w)))){if(unlikely((w=cvt(t,w))==0)){if(jt->jerr==EVDOMAIN)jt->jerr=EVINHOMO; R 0;}}  // note if w is boxed and nonempty this won't change it
  jt->fillv=CAV(q); jt->fillvlen=bpnoun(t);   // jt->fillv points to the fill atom, which may have been allocated here & will be freed when the caller exits
 }else{fillv0(t);}    // default fill.   point jt->fillv at it
 R w;
}

// Create a cell of fill for empty execution, type wt, rank r, shape *s.  Copy in the data.  The cell is NOT recursive
A jtfiller(J jt, I wt, I r, I* s){A z; I n,klg; CPROD(1,n,r,s); klg=bplg(wt); fillv0(wt); GA(z,wt,n,r,s); mvc(n<<klg,CAVn(r,z),jt->fillvlen,jt->fillv); R z;}

// Point jt->fillv to fill(s) of type t, and the # bytes of them into jt->fillv0len
void jtfillv0(J jt,I t){
 if(likely(t&B01+INT+INT2+INT4+FL+CMPX+HP+SP+QP+SBT)){jt->fillv=NUMERIC0; jt->fillvlen=4*SZI;}  // numeric & symbols that fill with 0
 else{
  jt->fillvlen=bpnoun(t);  // for others, just 1 atom of fill
  void *fill=&charfill; fill=t&BOX?voidAV0(ds(CACE)):fill; fill=t&XNUM+RAT?&Q0:fill; jt->fillv=fill;  // point to 1 atom of fill for the type
 }
}


static F2(jtrotsp){PROLOG(0071);A q,x,y,z;B bx,by;I acr,af,ar,*av,d,k,m,n,p,*qv,*s,*v,wcr,wf,wr;P*wp,*zp;
 ARGCHK2(a,w);
 ASSERT(!jt->fill,EVNONCE);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr; af=ar-acr; p=acr?AS(a)[af]:1;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 if(1<acr||af)R df2(z,a,w,qq(qq(ds(CROT),v2(1L,wcr)),v2(acr,wcr)));  // wcr to force qq to create loop, as sparse needs
 if(!wcr&&1<p){RZ(w=reshape(over(shape(w),apv(p,1L,0L)),w)); wr=wcr=p;}
 ASSERT(!wcr||p<=wcr,EVLENGTH);
 s=AS(w);
 GATV0(q,INT,wr,1L); qv=AVn(1L,q); mvc(wr*SZI,qv,MEMSET00LEN,MEMSET00); 
 RZ(a=vi(a)); v=AV(a); 
 DO(p, k=v[i]; d=s[wf+i]; qv[wf+i]=!d?0:0<k?k%d:k==IMIN?d-(-d-k)%d:d-(-k)%d;);
 wp=PAV(w); a=SPA(wp,a); RZ(y=ca(SPA(wp,i))); SETIC(y,m);
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


/* m   # cells
   d   # atoms in each cell
   n   # items in each cell
   k   # bytes in each atom
   p   length of av
   av  rotation amount(s)
   u   source data area 
   v   target data area      */
// set k=length that wraps (dk * shift), ks=offset to it in source, kd=offset to it in dest, js=source offset to part that doesn't wrap, kd=offset to it in dest
// for left shift (ar positive) ks=0, js=dk*shift, kd=e-k, jd=0
// for right shift              ks=e-k, js=0, kd=0, jd=dk*shift
#define ROTF(r) {I ar=ABS(r); if(unlikely((UI)ar>(UI)n)){if(jt->fill)ar=n; else{r=r%n; ar=ABS(r);}} k=dk*ar; kd=e-k; ks=r<0?kd:0; jd=r<0?k:0; kd-=ks; js=k-jd;}   // UI in case ABS(IMIN)

F2(jtrotate){A origw=w,z;C *u,*v;I acr,af,ar,d,k,m,n,p,*s,wcr,wf,wn,wr;
 F2PREFIP;ARGCHK2(a,w);
 if(unlikely(ISSPARSE(AT(w))))R rotsp(a,w);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr; af=ar-acr; p=acr?AS(a)[af]:1;  // p=#axes to rotate
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 RZ(a=vi(a));
 // We support IRS in a limited way.  We revert to the rank loop if:
 // 1 cell-rank of a>1  (we have to replicate w)
 // 2 a has frame, if: cell-rank of a > 0  (we have to match cells of a)
 //                OR  frame of w does not equal frame of a (agreement has already been checked in the caller, if IRS)
 //                   (in the case where a and w frames are equal, we apply each atom of a to one cell of w since a cell-rank is 0)
 if(((1-acr)|((-af)&(-acr|-(af^wf))))<0)R rank2ex(a,w,DUMMYSELF,MIN(acr,1),wcr,acr,wcr,jtrotate);  // revert if we can't match a and w easily
 if(unlikely(((wcr-1)&(1-p))<0)){RZ(w=reshape(apip(shape(w),apv(p,1L,0L)),w)); wr=wcr=p;}  // if cell is an atom, extend it up to #axes being rotated   !wcr && p>1
 if(unlikely(AN(a)==0))R RETARG(w);  // no axes to rotate, return fast (helps later loop)
 // special case: if a is atomic 0, and cells of w are not atomic
 I *av=IAV(a); I av0=*av;  // scan pointer through rotation counts; first or only rotation count
 if((wcr!=0)&(((ar|av0)==0)))R RETARG(w);   // 0 |. y, return y
 ASSERT(((-wcr)&(wcr-p))>=0,EVLENGTH);    // !wcr||p<=wcr  !(wcr&&p>wcr)
 wn=AN(w); if(!wn)R RETARG(w);  // if empty w, no need to rotate
 I negifragged=(p-2)&(1-AN(a));  // neg if p<=1 and AN(a)>1, meaning different rotations for each cell
 z=0;   // init no result allocated
 I notrightfill=-1;  // sign set if this is not right shift with fill
 if(jt->fill){RZ(w=jtsetfv1(jt,w,AT(w))); notrightfill=~av0;}  // set fill value if given (and convert w if needed); remember if right shift
 u=CAV(w); wn=AN(w); s=AS(w); I klg=bplg(AT(w));
 PROD(m,wf,s); PROD(d,wr-wf-1,s+wf+1); SETICFR(w,wf,wcr,n);   // m=#cells of w, n=#items per cell  d=#atoms per item of cell
 I e=(n*d)<<klg; I dk=d<<klg; // e=#bytes per cell  dk=bytes per item
 I kd,ks,jd,js; ROTF(av0)
 I yztotal=0;  // sum of ping-pong buffer A addresses
 if((((AFLAG(w)&(AFVIRTUAL|AFNJA|AFRO|RECURSIBLE))-1)&ASGNINPLACENEG(SGNIF(jtinplace,JTINPLACEWX),w))<0){  // w allows inplacing, header size is valid
  if((notrightfill&~negifragged)<0){  // a does not differ between cells and is not right fill
   // Check for inplacing.  We can if the rotate, after converted to positive shift value, fits in the slack
   I backslack=(FHRHSIZE(AFHRH(w))-AK(w)-(AN(w)<<klg))&-SZI;  //  allocated size of w, which is known to be allocated by J, MINUS the offset to the data and the length of the data
   if(ks+js<backslack){   // can the front section fit in the slack?
    // we can inplace the first axis.  Advance AK(w), which shifts left.  We will do the copies from back to front
    // since cells overlap.  u=v will signal that the first block doesn't get copied
    AK(w)+=ks+js; k=av0<0?ks:k;  // for <<, len is len of wrap; for >>, len of nonwrap
    ks=0; kd=e; u+=(m-1)*e; v=u; e=-e;  // fix up to move only k-part
    z=w;
   }
  }
  yztotal=(I)w;  // if w was inplaceable but we don't inplace because of size, remember that w is available as a ping-pong buffer
 }
 if(z==0){GA(z,AT(w),wn,wr,s); v=CAVn(wr,z); yztotal+=(I)z;}   // allocate result area, unless we are inplacing into w
 I ii=m; while(1){if(u!=v)MC(v+jd,u+js,e-k); if(!jt->fill)MC(v+kd,u+ks,k); else mvc(k,v+kd,(I)1<<klg,jt->fillv); if(--ii<=0)break; if(withprob(negifragged<0,0.1)){av0=*++av; ROTF(av0)} u+=e; v+=e;}

 if(1<p){I i;
  // more than 1 axis: we ping-pong between buffers as we go down the axes.
  //   Start here with input in z
  s+=wf;   // skip over w frame to get to the cell.  We will start 1 axis in
  for(i=0;i<p-1;++i){
   m*=n; n=*++s; PROD(d,wr-wf-i-2,s+1); e=(n*d)<<klg; dk=d<<klg;  // update cell sizes
   ROTF(av[i+1])  // calculate offsets
   u=CAV(z);   // we saved where the previous output went; it is the input
   // z here is always inplaceable.  See if it has extra room.  Since we might have added front slack on the first axis, we have the option of backing up
   I backslack=(FHRHSIZE(AFHRH(z))-(AK(z)+(AN(z)<<klg)))&-SZI;  // slack space at end
   I frontslack=AK(z)-AKXR(AR(z));  // slack space at front
   backslack=(ks+js-backslack)&~(jt->fill?REPSGN(av[i+1]):0);  // backslack is neg if we can move AK right, EXCEPT when right-shift with fill
   frontslack=(e-(ks+js)-frontslack)&(jt->fill?REPSGN(av[i+1]):-1);  // frontslack is neg if we can move AK left, EXCEPT when left-shift with fill
   if((backslack|frontslack)<0){   // there is room for inplacing, front or back
    if(((e-(ks+js))&REPSGN(backslack))>((ks+js)&REPSGN(frontslack))){  // choose the allowed direction that has the smaller wrapped area.  The unwrapped area will stay in place
     // inplace, adding to AK.  Still copy back to front
     AK(z)+=ks+js; k=av[i+1]<0?ks:k;  // move AK right, which shifts the buffer left.  If k was calculated from <<, the wrap length is correct.  If from >>, take other side
     ks=0; kd=e; u+=(m-1)*e; e=-e;  // adjust offsets, copy back to front
     // leave z unchanged for next axis
    }else{  // inplace subtracting from AK.  copy front to back
     AK(z)-=e-(ks+js); k=av[i+1]<0?k:kd;    // move AK left, which shifts the buffer right,  If k was calculated from >>, the wrap length is correct.  If from <<, take other side
     u-=e-(ks+js); ks=e; kd=0;  // adjust offsets; back u to start of wrapped area
    }
    v=u;   // this suppresses the copy of j
   }else{  // can't inplace
    if((I)z==yztotal){A y; GA(y,AT(z),AN(z),AR(z),AS(z)); yztotal+=(I)y;} // if there is only 1 buffer allocate a second
    z=(A)(yztotal-(I)z); v=CAV(z);  // ping-pong
   }
   // we have the buffer pointers, move the data
   I ii=m; while(1){if(u!=v)MC(v+jd,u+js,e-k); if(!jt->fill)MC(v+kd,u+ks,k); else mvc(k,v+kd,(I)1<<klg,jt->fillv); if(--ii<=0)break; u+=e; v+=e;}
  }  // do axes, with ping-pong, leaving result in z/v
 } 
 // w is going to be replaced.  That makes it non-pristine; but if it is inplaceable it can pass its pristinity to the result, as long as there is no fill
 PRISTXFERFIF(z,origw,jt->fill==0)  // transfer pristinity if there is no fill
 RETF(z);
}    /* a|.!.f"r w */

static F1(jtrevsp){A a,q,x,y,z;I c,f,k,m,n,r,*v,wr;P*wp,*zp;
 ARGCHK1(w);
 ASSERT(!jt->fill,EVNONCE);
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r; RESETRANK;
 m=AS(w)[f]; wp=PAV(w);
 GASPARSE(z,AT(w),1,wr,AS(w)); zp=PAV(z);
 a=SPA(wp,a); n=AN(a); RZ(y=ca(SPA(wp,i))); x=SPA(wp,x);
 RZ(q=paxis(wr,a)); v=AV(q); DO(wr, if(f==v[i]){k=i; break;});
 if(!r)       RZ(x=ca(x))
 else if(k>=n)RZ(x=irs2(apv(m,m-1,-1L),x,0L,1L,wr-k,jtfrom))
 else         {v=k+AV(y); c=m-1; DQ(SETIC(y,r), *v=c-*v; v+=n;); q=grade1(y); RZ(y=from(q,y)); RZ(x=from(q,x));}
 SPB(zp,a,ca(a)); 
 SPB(zp,e,ca(SPA(wp,e))); 
 SPB(zp,i,y); 
 SPB(zp,x,x);
 R z;
}    /* |."r w on sparse arrays */

F1(jtreverse){A z;C*wv,*zv;I f,k,m,n,nk,r,*v,*ws,wt,wr;
 F1PREFIP;ARGCHK1(w);
 if(unlikely(ISSPARSE(AT(w))))R revsp(w);
 if(unlikely(jt->fill!=0))R rotate(num(-1),w);  // rank is set - not inplaceable because it uses fill
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; f=wr-r;  // no RESETRANK - we don't call any primitive from here on  wr=rank of arg r=eff rank f=len of frame
 ws=AS(w); I *an=ws+f; an=r?an:&oneone[0]; n=*an;    // n=number of subitems of the cell to be reversed
 if(unlikely(((-r)&(1-n))>=0)){R RETARG(w);}  // rank 0 or 0-1 atoms in item - keep input unchanged
 wt=AT(w); wv=CAV(w);  // wv->source data
 PROD(m,f,ws); PROD(k,r-1,ws+f+1);  // m=# argument cells k=#atoms in one subitem
 k<<=bplg(wt); nk=n*k;  // k=#bytes in subitem  nk=#bytes in cell
 if((AC(w)&SGNIF(jtinplace,JTINPLACEWX))<0){z=w;}  // inplace: leave pristinity of w alone
 else{GA(z,wt,AN(w),wr,ws); PRISTCLRF(w)}  // new copy: allocate new area, make w non-pristine since it escapes
 // w has been destroyed
 zv=CAV(z);  // zv->target data
// macro to copy from both ends, swapping.  one last copy at the end, if needed (otherwise discard it)
#define COPY2ENDS(T) {DQ(m, T *s1=(T*)wv; T *t1=(T*)zv; T *sn=(T*)(wv+nk)-1; T *tn=(T*)(zv+nk)-1; \
                      DQ(n>>1, T x0=*s1; T xn=*sn; *t1=xn; *tn=x0; ++s1; --sn; ++t1; --tn;) t1=n&1?t1:(T*)&jt->shapesink; *t1=*s1;  wv+=nk; zv+=nk;)}
 switch(k){
 case sizeof(I): COPY2ENDS(I) break;
 case sizeof(C): COPY2ENDS(C) break;
 case sizeof(S): COPY2ENDS(S) break;
#if SY_64
 case sizeof(I4):COPY2ENDS(I4) break;
#endif
 default:
  if(zv==wv){  // reverse in place
   I nI=(k-1)>>LGSZI; I nB=(-k)&(SZI-1);  // number of words to copy (may be 0), number of bytes NOT to copy, 0-7.  Overfetch allowed but not overstore
   {DQ(m, C *s1=wv; C *sn=(wv+nk)-k; \
    DQ(n>>1, I *i1=(I*)s1; I *in=(I*)sn; I xx; DQ(nI, xx=*i1; *i1=*in; *in=xx; ++ i1; ++in;) xx=*i1; I yy=*in; STOREBYTES(i1,yy,nB) STOREBYTES(in,xx,nB) s1+=k; sn-=k;) wv+=nk;)}  // center item stays in place
  }else{C*s=wv-k,*t; DQ(m, t=s+=nk; DQ(n, MC(zv,t,k); zv+=k; t-=k;););}  // one-ended copy, not in place
  break;
 }
 RETF(z);  // This verb propagates WILLOPEN and must not perform EPILOG
}    /* |."r w */


static A jtreshapesp0(J jt,A a,A w,I wf,I wcr){A e,p,x,y,z;B*b,*pv;I c,d,r,*v,wr,*ws;P*wp,*zp;
 wr=AR(w); ws=AS(w);
 wp=PAV(w); RZ(b=bfi(wr,SPA(wp,a),1));
 RZ(e=ca(SPA(wp,e))); x=SPA(wp,x); y=SPA(wp,i);
 v=AS(y); r=v[0]; c=v[1]; d=0; DO(wf, if(b[i])++d;);
 if(!wf){if(r&&c){v=AV(y); DO(c, if(v[i])R e;);} R AN(x)?reshape(mtv,x):e;}
 GASPARSE(z,AT(w),1,wf,ws);
 zp=PAV(z); SPB(zp,e,e); A bvec=ifb(wf,b); makewritable(bvec) SPB(zp,a,bvec);  // avoid readonly
 GATV0(p,B01,r,1); pv=BAV1(p);
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
 GATV0(t,INT,c+d*b[wf],1); v=AV1(t); 
 DO(wf, if(b[i])*v++=i;); if(b[wf])DO(d, *v++=wf+i;); j=wf; DQ(wcr, if(b[j])*v++=d+j; ++j;);
 SPB(zp,a,t);
 if(b[wf]){I n,q,r,*v0;   /* sparse */
  if(wf!=*AV(a1))R rank2ex(a,w,DUMMYSELF,MIN(AR(a),1),wcr,MIN(AR(a),1),wcr,jtreshape);
  RE(m=prod(1+d,av)); SETIC(y,n); if(ws[wf]){q=n*(m/ws[wf]); r=m%ws[wf];} else {q=0; r=0;}
  v=AV(y); DQ(n, if(r<=*v)break; ++q; v+=c;);
  GATV0(t,INT,q,1); u=AV1(t); v=v0=AV(y);
  m=j=0; DO(q, u[i]=m+*v; v+=c; ++j; if(j==n){j=0; v=v0; m+=ws[wf];});
  SPB(zp,i,stitch(abase2(vec(INT,1+d,av),t),reitem(sc(q),dropr(1L,y))));
  SPB(zp,x,reitem(sc(q),x));
 }else{                   /* dense  */
  GATV0(t,INT,an,1); v=AV1(t); MCISH(v,av,d); m=d; j=wf; DO(wcr, if(!b[j++])v[m++]=av[i+d];);
  SPB(zp,i,ca(y));
  SPB(zp,x,irs2(vec(INT,m,v),x,0L,1L,wcr-(an-m),jtreshape));
 }
 R z;
}    /* a ($,)"wcr w for sparse w and scalar or vector a */

// x is floating-point left side of x $ y, w is right side.  Result is INT x, but if x contains _, replace _ with the number needed to use up y exactly.
// Give rndfn a chance to adjust the number.  nlens is #axes before inner  cell: wcr for ($,) 1 for $   wcr is cell-rank of w, if "r given
static A jtreshapeblank(J jt, A a, A w, A rndfn, I nlens, I wcr){
 RZ(a=vib(a)); RZ(a=mkwris(a)) // convert to int, converting _ to IMAX; make it writable since we will change _
 I upos=IMAX, xprod=1, oflo=0;  // index of _ if any, */ x -. _, oflo flag
 DO(AN(a), DPMULDDECLS I xval=IAV(a)[i]; oflo=xval==0?xval:oflo; I nupos=xval==IMAX?i:upos; xval=xval==IMAX?1:xval; ASSERT(nupos<=upos,EVDOMAIN) upos=nupos; DPMULD(xprod,xval,xprod,oflo=1;);)  // multiple non-_, error if overflow of >1 _
 if(upos==IMAX)R a;  // if x didn't contain _, just use it as is
 ASSERT(!oflo,EVLIMIT) ASSERT(xprod!=0,EVDOMAIN);  // detect overflow; if result is empty, we cannot calculate a value
 I nw; PRODX(nw,nlens,AS(w)+AR(w)-wcr,1)   // calculate # cells in a wcr-cell of w: use all axes or just the first
 A nblank; dfv1(nblank,divide(sc(nw),sc(xprod)),rndfn); RZ(nblank);  // rndfn */ (x-._) % */ $`#@.isitem y
 ASSERT(AR(nblank)==0,EVRANK) if(!(AT(nblank)&INT))RZ(nblank=cvt(INT,nblank))  // nblank must be stomic
 IAV(a)[upos]=IAV(nblank)[0]; R a;  // replace _ with calculated value and return new x
}

F2(jtreshape){A z;B filling;C*wv,*zv;I acr,ar,c,k,m,n,p,q,r,*s,t,* RESTRICT u,wcr,wf,wr,* RESTRICT ws,zn;
 F2PREFIP;
 ARGCHK2(a,w);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; ws=AS(w); RESETRANK;
 if((I )(1<acr)|(I )(acr<ar)){z=rank2ex(a,w,DUMMYSELF,MIN(acr,1),wcr,acr,wcr,jtreshape); PRISTCLRF(w) RETF(z);}  // multiple cells - must lose pristinity
 // now a is an atom or a list.  w can have any rank
 if(unlikely(AT(a)&FL))RZ(a=jtreshapeblank(jt,a,w,ds(CRIGHT),wcr,wcr)) else RZ(a=vip(a));  // convert a to integer & audit; if FL, also check for _ and handle
 r=AN(a); u=AV(a);   // r=length of a   u->values of a
 if(unlikely(ISSPARSE(AT(w)))){RETF(reshapesp(a,w,wf,wcr));}
 PRODX(m,r,u,1)  // m=*/a (#atoms in result)  c=#cells of w  n=#atoms/cell of w
 CPROD(,c,wf,ws); CPROD(,n,wcr,wf+ws);
 ASSERT(likely(n!=0)||!m||jt->fill,EVLENGTH);  // error if attempt to extend array of no items to some items without fill
 t=AT(w); filling = 0;
 if(m<=n){  // no wraparound
  if(c==1) {  // if there is only 1 cell of w...
   // If no fill required, we can probably use a virtual result, or maybe even an inplace one.  Check for inplace first.  Mustn't inplace an indirect that shortens the data,
   // because then who would free the blocks?  (Actually it would be OK if nonrecursive, but we are trying to exterminate those).  Since it must be DIRECT, there's no question about PRISTINE, but that would be OK to transfer if inplaceable
   if(ASGNINPLACESGN(SGNIF(jtinplace,JTINPLACEWX)&(r-(wcr+1))&((n-(m+1))|-(t&DIRECT)),w)){  //  inplace allowed, just one cell, result rank (an) <= current rank (so rank fits), usecount is right, equal atom count if not DIRECT
    // operation is loosely inplaceable.  Copy in the rank, shape, and atom count.
    AR(w)=(RANKT)(r+wf); AN(w)=m; ws+=wf; MCISH(ws,u,r) RETF(w);   // Start the copy after the (unchanged) frame
   }
   // Not inplaceable.  Create a (noninplace) virtual copy, but not if NJA memory (to avoid making the virtual NJA backer unmodifiable).  Don't virtual unless m >= MINVIRTSIZE
   if((SGNIF(AFLAG(w),AFNJAX)|((t&(DIRECT|RECURSIBLE))-1)|(m-MINVIRTSIZE))>=0){RZ(z=virtual(w,0,r+wf)); AN(z)=m; I * RESTRICT zs=AS(z); MCISH(zs,ws,wf) MCISH(zs+wf,u,r) RETF(z);}
   // for NJA/SMM, fall through to nonvirtual code
  }
 }else if(filling=jt->fill!=0){RZ(w=jtsetfv1(jt,w,AT(w))); t=AT(w);}   // if fill required, set fill value.  Remember if we need to fill
 k=bpnoun(t); p=k*m; q=k*n;
 DPMULDE(c,m,zn);
 GA00(z,t,zn,r+wf); s=AS(z); MCISH(s,ws,wf); MCISH(s+wf,u,r);
 if(!zn)R z;
 zv=CAV(z); wv=CAV(w); 
 // We extracted from w, so mark it (or its backer if virtual) non-pristine.  Note that w was not changed above if it was boxed nonempty.  z is never pristine, since it may have repeats
 PRISTCLRF(w)
 // w has been destroyed
 if(filling)DQ(c, mvc(q,zv,q,wv); mvc(p-q,q+zv,k,jt->fillv); zv+=p; wv+=q;)
 else DQ(c, mvc(p,zv,q,wv); zv+=p; wv+=q;);
 RETF(z);
}    /* a ($,)"r w */

F2(jtreitem){A y,z;I acr,an,ar,r,*v,wcr,wr;
 F2PREFIP;
 ARGCHK2(a,w);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; r=wcr-1; RESETRANK;
 if((I)(1<acr)|(I)(acr<ar)){z=rank2ex(a,w,DUMMYSELF,MIN(acr,1),wcr,acr,wcr,jtreitem); PRISTCLRF(w) RETF(z);}  // multiple cells - must lose pristinity  // We handle only single operations here, where a has rank<2
 // acr<=ar; ar<=acr; therefore ar==acr here
 fauxblockINT(yfaux,4,1);
 if(1>=wcr)y=a;  // y is atom or list: $ is the same as ($,)
 else{   // rank y > 1: append the shape of an item of y to x
  if(unlikely(AT(a)&FL))RZ(a=jtreshapeblank(jt,a,w,ds(CRIGHT),MIN(1,wcr),wcr)) else RZ(a=vip(a));  // convert a to integer & audit; if FL, also check for _ and handle
  an=AN(a); acr=1;  // if a was an atom, now it is a list
  fauxINT(y,yfaux,an+r,1) v=AV(y);
  MCISH(v,AV(a),an); MCISH(v+an,AS(w)+wr-r,r);
 }
 R wr==wcr?jtreshape(jtinplace,y,w):IRS2(y,w,0L,acr,wcr,jtreshape,z);  // Since a has no frame, we don't have to check agreement
}    /* a $"r w */

// x $[!.n]!.v y or x ($,)[!.n]!.v y which uses fn v if needed to resolve _ in x
DF2(jtreshapeblankfn){I acr,ar,r,wcr,wr;
 F2PREFIP;
 ARGCHK2(a,w);
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr;   RESETRANK;
 if((I )(1<acr)|(I )(acr<ar)){A z=rank2ex(a,w,DUMMYSELF,MIN(acr,1),wcr,acr,wcr,jtreshapeblankfn); PRISTCLRF(w) RETF(z);}  // multiple cells - must lose pristinity
 A fs=FAV(self)->fgh[0]; AF reshapefn=FAV(fs)->valencefns[1];  // next routine to call, $ ($,) or !.n
 if(likely(AT(a)&FL)){  // if there might be _, check for it
  I nlens=FAV(self)->localuse.lu1.fittype?1:wcr; nlens=wcr<nlens?wcr:nlens;  // # axes to use for counting result cells: the w-cell, or 1 item thereof
  RZ(a=jtreshapeblank(jt,a,w,FAV(self)->fgh[1],nlens,wcr))  // replace the blank, applying function in g
 }
 RETF((*reshapefn)(jtinplace,a,w,fs))   // do the reshape, with _ replaced
}

#define EXPAND(T)  \
  {T*u=(T*)wv,*v=(T*)zv,x;                                                \
   mvc(sizeof(T),&x,k,jt->fillv);                                         \
   DQ(an, I abit=*av++; T *uv=abit?u:&x; *v++=*uv; u+=abit;);  \
  }

F2(jtexpand){A z;B*av;C*wv,*zv;I an,i,k,p,wc,wk,wt,zn;
 ARGCHK2(a,w);
 if(!ISDENSETYPE(AT(a),B01))RZ(a=cvt(B01,a));
 ASSERT(1>=AR(a),EVRANK);  // x must be atom or list
 RZ(w=jtsetfv1(jt,w,AT(w)));   // surely fill is needed; maybe cvt w
 if(!AR(w))R from(a,take(num(-2),w));  // atomic w, use a { _2 {. w
 av=BAV(a); an=AN(a);
 ASSERT(bsum(an,av)==AS(w)[0],EVLENGTH);  // each item of w must be used exactly once
 wv=CAV(w); PROD(wc,AR(w)-1,AS(w)+1) wt=AT(w); k=bpnoun(wt); wk=k*wc;  // k=bytes/atom, wk=bytes/item, wx=end+1 of area
 DPMULDE(an,wc,zn);
 I zr=AR(w); GA(z,wt,zn,AR(w),AS(w)); AS(z)[0]=an; zv=CAVn(zr,z);
 // We extracted from w, so mark it (or its backer if virtual) non-pristine.  Note that w was not changed above if it was boxed nonempty.  z is never pristine, since it may have repeats
 PRISTCLRF(w)   // this destroys w
 switch(wk){
 case sizeof(I): EXPAND(I); break;
 case sizeof(C): EXPAND(C); break;
 case sizeof(S): EXPAND(S); break;
#if SY_64
 case sizeof(int): EXPAND(int); break;
#endif
 default:  
  mvc(k*zn,zv,k,jt->fillv); // here we are trying to minimize calls to MC
  for(i=p=0;i<an;++i)
   if(*av++)p+=wk; 
   else{if(p){MC(zv,wv,p); wv+=p; zv+=p; p=0;} zv+=wk;}
  if(p){MC(zv,wv,p);}
 }
 RETF(z);
}    /* a&#^:_1 w or a&#^:_1!.f w */



