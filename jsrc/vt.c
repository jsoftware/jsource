/* Copyright (c) 1990-2026, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Take and Drop                                                    */

#include "j.h"

F1(jtbehead ){F12IP; R jtdrop(jtfg,zeroionei(1),w);}
F1(jtcurtail){F12IP; R jtdrop(jtfg,num(-1),w);}

F1(jtshift1){F12IP;R drop(num(-1),over(zeroionei(1),w));}

static I shape1[RMAX+1]={[0 ... RMAX]=1};  // use as cell-shape for atomic cell, to allow extension to any rank

// take empty or from empty, a=take shape, w=takefrom. empty=1 if result is empty (i. e. an axis in u, or an axis in w outside of u, is 0).  istake is 1 if take, 0 if drop
static A jttk0(J jt,B empty,I *u, I n,A w, I wf, I istake){A z;I m,r,*s;DPMULDDECLS
 r=AR(w); I *ws=r==wf?shape1:AS(w);  // ws points to shape of w, or all 1s if cell of w is an atom
 if(!empty){  // w was empty but the result is nonempty.  Can happen only for take.
  I p=1; DO(n, I m=ABS(u[i]); if(unlikely(m==IMAX))m=ws[wf+i]; DPMULDE(p,m,p)) PRODX(p,r-n,&AS(w)[n],ABS(p)) PRODX(m,wf,AS(w),p)  // count atoms, applying infinities.  We have values from a, then tail of w, then frame of w
  RZ(w=jtsetfv1(jt,w,AT(w)));  // if cell is not empty, count its atoms in m.  Handle axes taken in full  Set the fill value
 }else{m=0;}  // cell is empty and thus has 0 components.
 if(unlikely(r==wf))r+=n;  // if cell is an atom, extend its shape with 1s for each axis in u
 GA(z,AT(w),m,r,AS(w));   // allocate result; init shape to w shape
 s=&AS(z)[wf]; DO(n, I m=ABS(u[i]); if(istake){if(unlikely(m==IMAX))m=ws[wf+i];}else{m=ws[wf+i]-m; m=m<0?0:m;} s[i]=ABS(m););  // install cell-shape from u; handle axes taken in full, convert drop counts to take counts
 if(m){I k=bpnoun(AT(w)); mvc(k*m,AVn(r,z),k,jt->fillv);}
 R z;
}


static F2(jttks){F12IP;PROLOG(0092);A a1,q,x,y,z;B b,c;I an,m,r,*s,*u,*v;P*wp,*zp;
 an=AN(a); u=AV(a); r=AR(w); s=AS(w); 
 GASPARSE(z,AT(w),1,r,s); v=AS(z); DO(an, v[i]=ABS(u[i]););
 zp=PAV(z); wp=PAV(w);
 if(an<=r){RZ(a=vec(INT,r,s)); MCISH(AV(a),u,an);}  // vec is not virtual
 a1=SPA(wp,a); RZ(q=paxis(r,a1)); m=AN(a1);
 RZ(a=from(q,a       )); u=AV(a);
 RZ(y=from(q,shape(w))); s=AV(y);
 b=0; DO(r-m, if(b=u[i+m]!=s[i+m])break;);
 c=0; DO(m,   if(c=u[i  ]!=s[i  ])break;);
 if(b){jt->fill=SPA(wp,e); x=irs2(vec(INT,r-m,m+u),SPA(wp,x),0L,1L,-1L,jttake); jt->fill=0; RZ(x);}  // fill cannot be virtual
 else x=SPA(wp,x);
 if(c){A j;C*xv,*yv;I d,i,*iv,*jv,k,n,t;
  d=0; t=AT(x); k=aii(x)<<bplg(t);
  q=SPA(wp,i); SETIC(q,n);
  I jr=AR(q); GATV(j,INT,AN(q),AR(q),AS(q)); jv= AVn(jr,j); iv= AV(q);
  I yr=AR(x); GA(y,t,  AN(x),AR(x),AS(x)); yv=CAVn(yr,y); xv=CAV(x);
  for(i=0;i<n;++i){
   c=0; DO(m, t=u[i]; if(c=0>t?iv[i]<t+s[i]:iv[i]>=t)break;);
   if(!c){++d; MC(yv,xv,k); yv+=k; DO(m, t=u[i]; *jv++=0>t?iv[i]-(t+s[i]):iv[i];);}
   iv+=m; xv+=k;
  }
  SPB(zp,i,d<n?take(sc(d),j):j); SPB(zp,x,d<n?take(sc(d),y):y);
 }else{SPB(zp,i,ca(SPA(wp,i))); SPB(zp,x,b?x:ca(x));}
 SPB(zp,a,ca(SPA(wp,a)));
 SPB(zp,e,ca(SPA(wp,e)));
 EPILOG(z);
}    /* take on sparse array w */

// general take/drop routine.  *u (length n) is the signed take values, w is array, wf is frame of operation.
// istake is 1 for take, 0 for drop
static INLINE A jttk(A w, I *u, I n, I wf, J jtfg, I istake, I wcr, A a, I wt){F12IP;PROLOG(0093);A y,z;C*yv,*zv;I c,dy,dz,e,i,k,m,p,q,r;
 I wr=AR(w), t=AT(w), *ws=wr==wf?shape1:AS(w);  // ws points to shape of w, or all 1s if cell of w is an atom
 if(unlikely(ISSPARSE(t))){
  // sparse: reconstruct old parameters
  A s; RZ(s=vec(INT,wf+n,AS(w))); I *v=IAV(s);  // create safe place to build axes.  This is a kludge and overfetches from w, but that's how it was done
  if(istake){
   DO(n, I m=u[i]; I ms=REPSGN(m); if(unlikely((m^ms)-ms==IMAX))m=ws[wf+i]; v[wf+i]=m;)  // see if there are infinities.  They are IMAX/-IMAX, so take abc & see if result is IMAX
  }else{  // drop
   DO(n, I m=u[i]; I ut=ws[wf+i]; ut-=ABS(m); ut=ut<0?0:ut; m=~REPSGN(m); ut=(ut^m)-m; v[wf+i]=ut;)   // convert the drops to takes
  }
  if(wf==wr){A z;IRS2(vec(INT,n,shape1),w,0,1,0,jtreshape,z);RZ(z);w=z;}  // if w is an atom, change it to a singleton of rank #$a
  R tks(s,w);
 }

 // see if a virtual can be returned
 if(!((AN(a)^1)|wf|!wcr)&&likely(AN(w)>0)&&likely(!(AFLAG(w)&(AFNJA)))){  // if there is only 1 take axis, w has no frame and is not atomic; and avoid virtualling NJA
  // if the length of take is within the bounds of the first axis
  I tdlen=IAV(a)[0];  // get the one number in a, the take/drop amount
  I nitems=ws[0];  // number of items of w
  I tkasign=REPSGN(tdlen), tkabs;  // 0 if tklen nonneg, ~0 if neg; |len|
  if(!istake){tkabs=ABS(tdlen); tkabs=nitems-tkabs; tkabs=tkabs<0?0:tkabs; tkasign=~tkasign; tdlen=(tkabs^tkasign)-tkasign;} else tkabs = (tdlen^tkasign)-tkasign;  // convert dropct to takect; set (UI)ABS(tdlen)
  I minvirt=MINVIRTSIZE(wt); minvirt=minvirt>AN(w)?AN(w):minvirt;  // minimum len to create v virtual of this type, never more than the # items we have
  I wcellsize; PROD(wcellsize,wr-1,ws+1);  // size of a cell in atoms of w
  if(BETWEENO(tkabs*wcellsize,minvirt,AN(w))){  // if this is not an overtake or take in full, and big enough to make virtual...  (unsigned to handle overflow, if tklen=IMIN).
   I woffset=tkasign&(tdlen+nitems);   // offset to data if w, in cells: x+#y if x neg, 0 if x pos
   I offset = woffset*wcellsize;  // offset in atoms of the virtual data
   // allocate virtual block, passing in the in-place status from w
   RZ(z=virtualip(w,offset,wr));    // allocate block
   // fill in shape.  Note that z and w may be the same block, so ws is destroyed
   I* RESTRICT zs=AS(z); zs[0]=tkabs; MCISH(&zs[1],&ws[1],wr-1)  // shape of virtual matches shape of w except for #items
   AN(z)=tkabs*wcellsize;  // install # atoms
   // creating a virtual does not affect the pristinity of the backer
   RETF(z);
  }else if(unlikely(tkabs==nitems))R w;  // if w taken in full, return w unchanged
 }

 if(unlikely(wf==wr)){ASSERT(wf+n<=RMAX,EVLIMIT) ws=shape1;}   // if cell of y is an atom, give it 1s as rank extension; make sure the extension doesn't exceed rank max

 I dlen[RMAX+1], flen[RMAX+1];  // shape ptr into w; length in atoms of data to be copied for each fill section, length in atoms of each fill to be copied after the data. dlen<0 means scalar replication
 I cn, zcn, ux, zfx=0; PROD(zcn,wr-(wf+n),&ws[wf+n]) cn=zcn; // size of next-lower cell of w; size of next-lower cell of z; index to shape, running backwards; index to coalesced fill section, running forwards from 0; size of largest undivided cell
 I lowerreps=zcn; // # inherited repeats of lower axis, incremented by axes taken in full.  First time, must include the size of the inner cell
 I ifill=0, iskip=0, anyfill=0;  // init # atoms of initial fill and initial skip; flag is there is any fill
 for(ux=n-1;ux>=0;--ux){  // for each active axis (starting at the last)
  I ui=u[ux], wsi=ws[ux+wf], absui=ABS(ui);  // fetch take/drop count and axis length for the next axis, and |ui|
  // put count into take form, in range [0,axislen]
  if(istake){if(unlikely((UI)absui>=(UI)IMAX))absui=wsi;  // take: replace infinite length with length of axis
  }else{absui=wsi-absui; absui=absui<0?0:absui; ui=(absui^~REPSGN(ui))-~REPSGN(ui);} // drop: convert drop count to equivalent take count
  if(unlikely(absui==0))goto emptytake;   // if empty take axis, go create empty result
  // move into flen/dlen
  if(unlikely(absui==wsi)){lowerreps*=absui;  // if axis is taken in full, do not copy - we will simply run the previous level more times
  }else{  // normal case with a change in axis length
   dlen[zfx]=MIN(absui,wsi)*lowerreps; // dlen=#repeats of next-lower cell (at bottom, # atoms to copy)
   if(istake){flen[zfx]=absui-wsi>0?zcn:cn; flen[zfx]*=absui-wsi; anyfill|=-flen[zfx]; ifill+=REPSGN(ui&-flen[zfx])&flen[zfx]; // (take) flen=fill amount if + (z-cells), skip amount if - (w cells); remember of fill positive
   }else{flen[zfx]=absui-wsi>0?0:cn; flen[zfx]*=absui-wsi;}   // (drop) no fill possible
     // any fill needed?
   iskip+=REPSGN(ui&flen[zfx])&flen[zfx];   // if ui neg, add to initial fill/skip counts
   lowerreps=1; ++zfx;  // this has fill or skip: write out a record for it and reinit the lower-cell count
  }
  DPMULDE(zcn,absui,zcn); cn*=wsi;  // update total size of z cell
 }
 if(unlikely(zfx==0)){if(wf!=wr)R w; else {dlen[0]=1, flen[0]=0, zfx=1;}}  // if all axes taken in full, return input unchanged EXCEPT when taking from an atom; continue then with 1 atomic axis, to get the shape
 if(unlikely(AN(w)==0)){if(istake){DONOUNROLL(wf, if(!AS(w)[i])goto emptytake;) DONOUNROLL(wr-(wf+n), if(!AS(w)[wf+n+i])goto emptytake;)} goto emptyovertake;}  // w is empty: if it has a 0 among the surviving axes, call it an empty take, otherwise empty w.  Use original AS(w)
 dlen[zfx]=IMAX;  // in case there is only 1 axis, give it an unlimited second axis for loop reduction

 PRODX(zcn,wf,AS(w),zcn)  // include all leading axes taken in full and the frame
 if(istake&&anyfill<0){RZ(w=jtsetfv1(jt,w,AT(w))); t=AT(w);}  // if fill is required, set the fill atom and convert it & w to common type
 GA0(z,t,zcn,MAX(wr,wf+n)); MCISH(AS(z),AS(w),wr) if(unlikely(wf==wr))MCISH(&AS(z)[wr],shape1,n) // allocate result, copy w shape, which includes trailing 1s if atomic cell
 DO(n, I absui=ABS(u[i]); if(istake){if(unlikely((UI)absui>=(UI)IMAX))absui=ws[wf+i];}else{absui=ws[wf+i]-absui;} AS(z)[wf+i]=absui;)   // install the shape given by u.  Final absui must be >0 here
 I klg=bplg(t);
 C *s=CAV(w), *d=CAV(z), *endd=d+(zcn<<klg);  // pointer to source, pointer to start/end of filled area
 if(ifill!=0){mvc(ifill<<klg,d,1LL<<klg,jt->fillv); d+=ifill<<klg;} s-=iskip<<klg;  // install initial fill, take initial skip

 I dl=dlen[0]<<klg, fl=flen[0]<<klg, dl1=dlen[1];  // len of first level of data and fill; number of repeats at second level
 I rx, ndx[RMAX];  // odometer into copy sections
 for(rx=0;rx<zfx;++rx)ndx[rx]=0;   // init all levels (topmost omitted since we test for exit using the address)
 while(1){
  I dl1i=dl1; do{MC(d,s,dl); d+=dl; if(d==endd)goto endcopy; s+=dl; if(fl>0){mvc(fl,d,1LL<<klg,jt->fillv); d+=fl; if(d==endd)goto endcopy;}else s-=fl;}while(--dl1i);   // copy data+fill for lowest cell.  This is the only place data comes from; the rest is fill
  rx=1;  // start looking at higher axes
  while(1){  // go up through the axes, adding fill if we get to the end of the axis.  We enter this loop in the logical middle, since we have already half-processed level 1
   if(istake&&flen[rx]>0){I l=flen[rx]<<klg; if(unlikely((I)((endd-d)-l)<0))l=endd-d; mvc(l,d,(I)1<<klg,jt->fillv); d+=l; if(d==endd)goto endcopy;}  // copy fill if there is any.  Fill after the first axis may overrun the end; check for that
   else s-=flen[rx]<<klg;  // if no fill, skip over unused input if any
   ++rx;  // we have moved fill for this axis; restart the axis & check the next
   if(rx==zfx)break;  // when we fill out the first axis, we have copied the whole cell
   if(++ndx[rx]<dlen[rx])break;  // until we hit end, just go back to move another cell.  This increments the odometer
   ndx[rx]=0;  // if odometer rolled over to end value, continue to next wheel after resetting the one that rolled
  }
 }
endcopy:;
 // We extracted from w, so mark it (or its backer if virtual) non-pristine.  There may be replication (if there was fill), so we don't pass pristinity through  We overwrite w because it is no longer in use
 PRISTCLRF(w)  // scaf if w was abandoned, mark z pristine
 EPILOG(z);
 I empty; if(1){emptytake: empty=1;} else{emptyovertake: empty=0;} R jttk0(jt,empty,u,n,w,wf,istake);  // return empty cell. b=0: nonempty take from empty cell; b=1: empty take (any u or w-frame 0), i. e. no cells
}

// x {."r y, which allows infinities in x
F2(jttake){F12IP;A z;I acr,af,ar,n,*v,wcr,wf,wr;
 ARGCHK2(a,w); I wt=AT(w);  // wt=type of w
 acr=jt->ranks>>RANKTX; wcr=(RANKT)jt->ranks; RESETRANK;  // save ranks before they are destroyed 
 if(unlikely(ISSPARSE(AT(a))))RZ(a=denseit(a));  //if a is empty this destroys jt->ranks
 ar=AR(a); acr=ar<acr?ar:acr; af=ar-acr;  // ?r=rank, ?cr=cell rank, ?f=length of frame
 wr=AR(w); wcr=wr<wcr?wr:wcr; wf=wr-wcr;
 if(((af-1)&(acr-2))>=0){  // af>0 || acr>1   a has frame, or cell of a has rank > 1
  z=rank2ex(a,w,DUMMYSELF,MIN(acr,1),wcr,acr,wcr,jttake);  // if multiple x values, loop over them  af>0 or acr>1
  // We extracted from w, so mark it (or its backer if virtual) non-pristine.  There may be replication (if there was fill), so we don't pass pristinity through  We overwrite w because it is no longer in use
  PRISTCLRF(w)
  RETF(z);
 }
 // canonicalize x
 n=AN(a);    // n = #axes in a
 ASSERT(BETWEENC(n,1,wcr),EVLENGTH);  // if y is not atomic, a must not have extra axes  wcr==0 always tests true
 I * RESTRICT ws=AS(w);  // ws->shape of w
 RZ(a=vib(a));  // convert input to integer, auditing for illegal values; and convert infinities to IMAX/-IMAX
 // full processing for more complex a
 z=jttk(w,IAV(a),AN(a),wf,jtfg,1,wcr,a,wt);  // go do the general take/drop
 RETF(z);
}

F2(jtdrop){F12IP;A z;I acr,af,ar,d,m,n,*u,*v,wcr,wf,wr;
 
 RZ((a=vib(a))&&w);  // convert & audit a
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr; af=ar-acr;  // ?r=rank, ?cr=cell rank, ?f=length of frame
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK; I wt=AT(w);
 // special case: if a is atomic 0, and cells of w are not atomic
 if((-wcr&(ar-1))<0&&(IAV(a)[0]==0))R RETARG(w);   // 0 }. y, return y
 if(((af-1)&(acr-2))>=0){
  z=rank2ex(a,w,DUMMYSELF,MIN(acr,1),wcr,acr,wcr,jtdrop);  // if multiple x values, loop over them  af>0 or acr>1
  // We extracted from w, so mark it (or its backer if virtual) non-pristine.  There may be replication, so we don't pass pristinity through  We overwrite w because it is no longer in use
  PRISTCLRF(w)
  RETF(z);
 }
 n=AN(a); u=AV(a);     // n=#axes to drop, u->1st axis
 ASSERT(BETWEENC(n,1,wcr),EVLENGTH);  // if y is not atomic, a must not have extra axes  wcr==0 always tests true
 z=jttk(w,IAV(a),n,wf,jtfg,0,wcr,a,wt);  // go do the general take/drop
 RETF(z);
}


// create 1 cell of fill when head/tail of an array with no items (at the given rank)
static F1(jtrsh0){F12IP;A x,y;I wcr,wf,wr,*ws;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 ws=AS(w);
 RZ(x=vec(INT,wr-1,ws)); MCISH(wf+AV(x),ws+wf+1,wcr-1);
 RZ(w=jtsetfv1(jt,w,AT(w))); GA00(y,AT(w),1,0); MC(AV0(y),jt->fillv,bpnoun(AT(w)));
 R reshape(x,y);
 // not pristine
}

F1(jthead){F12IP;I wcr,wf,wr;
 ARGCHK1(w);
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr;  // no RESETRANK so that we can pass rank into other code
 if(unlikely(!wcr)){RETF(RETARG(w))  // {."0, a NOP
 }else if(likely(AS(w)[wf]!=0)){  // if cell is atom, or cell has items - which means it's safe to calculate the size of a cell
  // Use from.  Note that jt->ranks is still set, so this may produce multiple cells
  // left rank is garbage, but since zeroionei(0) is an atom it doesn't matter
  RETF(jtfrom(jtfg,zeroionei(0),w,ds(CFROM)));  // scaf could call jtfromi directly for non-sparse w
 }else{RETF(ISSPARSE(AT(w))?irs2(zeroionei(0),take(zeroionei( 1),w),0L,0L,wcr,jtfrom):rsh0(w));  // sparse or cell of w is empty - create a cell of fills  jt->ranks is still set for use in take.  Left rank is garbage, but that's OK
 }
 // pristinity from the called verb
}

F1(jttail){F12IP;I wcr,wf,wr;
 ARGCHK1(w);
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr;  // no RESETRANK: rank is passed into from/take/rsh0.  Left rank is garbage but that's OK
 if(unlikely(!wcr)){RETF(RETARG(w))  // {:"0, a NOP
 }else if(likely(AS(w)[wf]!=0)){  // if cell is atom, or cell has items - which means it's safe to calculate the size of a cell
  // Use from.  Note that jt->ranks is still set, so this may produce multiple cells
  // left rank is garbage, but since num(-1) is an atom it doesn't matter
  RETF(jtfrom(jtfg,num(-1),w,ds(CFROM)));  // scaf could call jtfromi directly for non-sparse w
 }else{RETF(ISSPARSE(AT(w))?irs2(zeroionei(0),take(num(-1),w),0L,0L,wcr,jtfrom):rsh0(w));  // sparse or cell of w is empty - create a cell of fills  jt->ranks is still set for use in take.  Left rank is garbage, but that's OK
 }
 // pristinity from other verbs
}
