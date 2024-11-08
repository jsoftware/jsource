/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Take and Drop                                                    */

#include "j.h"

F1(jtbehead ){F1PREFIP; R jtdrop(jtinplace,zeroionei(1),    w);}
F1(jtcurtail){F1PREFIP; R jtdrop(jtinplace,num(-1),w);}

F1(jtshift1){R drop(num(-1),over(num(1),w));}

static A jttk0(J jt,B b,A a,A w){A z;I k,m=0,n,p,r,*s,*u;
 r=AR(w); n=AN(a); u=AV(a); 
 if(!b){PRODX(m,n,u,1) ASSERT(m>IMIN,EVLIMIT); PRODX(m,r-n,n+AS(w),ABS(m))}
 GA(z,AT(w),m,r,AS(w)); 
 s=AS(z); DO(n, p=u[i]; ASSERT(p>IMIN,EVLIMIT); *s++=ABS(p););
 if(m){k=bpnoun(AT(w)); mvc(k*m,AVn(r,z),k,jt->fillv);}
 R z;
}

static F2(jttks){PROLOG(0092);A a1,q,x,y,z;B b,c;I an,m,r,*s,*u,*v;P*wp,*zp;
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

// general take routine.  a is result frame followed by signed take values i. e. shape of result, w is array
static F2(jttk){PROLOG(0093);A y,z;B b=0;C*yv,*zv;I c,d,dy,dz,e,i,k,m,n,p,q,r,*s,t,*u;
 n=AN(a); u=AV(a); r=AR(w); s=AS(w); t=AT(w);
 if(unlikely(ISSPARSE(t)))R tks(a,w);
 DO(n, if(!u[i]){b=1; break;}); if(!b)DO(r-n, if(!s[n+i]){b=1; break;});  // if empty take, or take from empty cell, set b
 if(((b-1)&AN(w))==0)R tk0(b,a,w);   // this handles empty w, so PROD OK below   b||!AN(w)
 k=bpnoun(t); z=w; c=q=1;  // c will be #cells for this axis
 // process take one axis at a time
 for(i=0;i<n;++i){I itemsize;
  c*=q; p=u[i]; q=ABS(p); m=s[i];  // q=length of take can be IMIN out of this   m=length of axis
  if(q!=m){  // if axis unchanged, skip it.  This includes the first axis
   PROD(itemsize,r-i-1,s+i+1);  // size of item of cell
   DPMULDE(c*itemsize,q,d); GA(y,t,d,r,AS(z)); AS(y)[i]=q;  // this catches q=IMIN: mult error or GA error   d=#cells*itemsize*#taken items
   if(q>m)mvc(k*AN(y),CAVn(r,y),k,jt->fillv);   // overtake - fill the whole area
   itemsize *= k; e=itemsize*MIN(m,q);  //  itemsize=in bytes; e=total bytes moved per item
   dy=itemsize*q; yv=CAV(y);
   dz=itemsize*m; zv=CAV(z);
   m-=q; I yzdiff=dy-dz; yv+=REPSGN(p&m)&yzdiff; zv-=REPSGN(p&-m)&yzdiff;
   DQ(c, MC(yv,zv,e); yv+=dy; zv+=dz;);
   z=y;
  }
 }
 EPILOG(z);
}

F2(jttake){A s;I acr,af,ar,n,*v,wcr,wf,wr;
 F2PREFIP;
 ARGCHK2(a,w); I wt = AT(w);  // wt=type of w
 acr=jt->ranks>>RANKTX; wcr=(RANKT)jt->ranks; RESETRANK;  // save ranks before they are destroyed 
 if(unlikely(ISSPARSE(AT(a))))RZ(a=denseit(a));  //if a is empty this destroys jt->ranks
 if(likely(!ISSPARSE(wt)))RZ(w=jtsetfv1(jt,w,AT(w)));   // pity to do this before we know we need fill
 ar=AR(a); acr=ar<acr?ar:acr; af=ar-acr;  // ?r=rank, ?cr=cell rank, ?f=length of frame
 wr=AR(w); wcr=wr<wcr?wr:wcr; wf=wr-wcr;
 if(((af-1)&(acr-2))>=0){
  s=rank2ex(a,w,DUMMYSELF,MIN(acr,1),wcr,acr,wcr,jttake);  // if multiple x values, loop over them  af>0 or acr>1
  // We extracted from w, so mark it (or its backer if virtual) non-pristine.  There may be replication (if there was fill), so we don't pass pristinity through  We overwrite w because it is no longer in use
  PRISTCLRF(w)
  RETF(s);
 }
 // canonicalize x
 n=AN(a);    // n = #axes in a
 ASSERT(BETWEENC(n,1,wcr),EVLENGTH);  // if y is not atomic, a must not have extra axes  wcr==0 is always true
 I * RESTRICT ws=AS(w);  // ws->shape of w
 RZ(s=vib(a));  // convert input to integer, auditing for illegal values; and convert infinities to IMAX/-IMAX
 // if the input was not INT/bool, we go through and replace any infinities with the length of the axis.  If we do this, we have
 // to clone the area, because vib might return a canned value
 if(!(AT(a)&B01+INT)){
  I i; for(i=0;i<AN(s);++i){I m=IAV(s)[i]; I ms=REPSGN(m); if((m^ms)-ms == IMAX)break;}  // see if there are infinities.  They are IMAX/-IMAX, so take abc & see if result is IMAX
  if(i<AN(s)){
   s=ca(s); if(!ISDENSETYPE(AT(a),FL))RZ(a=ccvt(FL,a,0));  // copy area we are going to change; put a in a form where we can recognize infinity
   for(;i<AN(s);++i){if(DAV(a)[i]==IMIN)IAV(s)[i]=IMIN;else if(INF(DAV(a)[i]))IAV(s)[i]=wcr?ws[wf+i]:1;}  // kludge.  The problem is which huge values to consider infinite.  This is how it was done
  }
 }
 a=s;
 if(!(ar|wf|(((NOUN&~(DIRECT|RECURSIBLE))|SPARSE)&wt)|!wcr|(AFLAG(w)&(AFNJA)))){  // if there is only 1 take axis, w has no frame and is not atomic; and avoid virtualling NJA
  // if the length of take is within the bounds of the first axis
  I tklen = IAV(a)[0];  // get the one number in a, the take amount
  I tkasign = REPSGN(tklen);  // 0 if tklen nonneg, ~0 if neg
  I nitems = ws[0];  // number of items of w
  I tkabs = (tklen^tkasign)-tkasign;  // ABS(tklen)
  if((UI)tkabs<=(UI)nitems) {  // if this is not an overtake...  (unsigned to handle overflow, if tklen=IMIN).
   // calculate offset
   I woffset = tkasign&(tklen + nitems);   // x+#y if x neg, 0 if x pos
   // get length of a cell of w
   I wcellsize; PROD(wcellsize,wr-1,ws+1);  // size of a cell in atoms of w
   I offset = woffset * wcellsize;  // offset in atoms of the virtual data
   // allocate virtual block, passing in the in-place status from w
   RZ(s = virtualip(w,offset,wr));    // allocate block
   // fill in shape.  Note that s and w may be the same block, so ws is destroyed
   I* RESTRICT ss=AS(s); ss[0]=tkabs; DO(wr-1, ss[i+1]=ws[i+1];);  // shape of virtual matches shape of w except for #items
   AN(s)=tkabs*wcellsize;  // install # atoms
   // virtual block does not affect pristinity of w
   RETF(s);
  }
 }
 // full processing for more complex a
 if((-wcr&(wf-1))>=0){   // if y is an atom, or y has multiple cells:
  RZ(s=vec(INT,wf+n,AS(w))); v=wf+AV(s);   // s is a block holding shape of a cell of input to the result: w-frame followed by #$a axes, all taken from w.  vec is never virtual
  if(!wcr){DO(n,v[i]=1;); RZ(w=reshape(s,w));}  // if w is an atom, change it to a singleton of rank #$a
  MCISH(v,AV(a),n);   // whether w was an atom or not, replace the axes of w-cell with values from a.  This leaves s with the final shape of the result
 }
 s=tk(s,w);  // go do the general take/drop
 // We extracted from w, so mark it (or its backer if virtual) non-pristine.  There may be replication (if there was fill), so we don't pass pristinity through  We overwrite w because it is no longer in use
 PRISTCLRF(w)
 RETF(s);
}

F2(jtdrop){A s;I acr,af,ar,d,m,n,*u,*v,wcr,wf,wr;
 F2PREFIP;
 RZ((a=vib(a))&&w);  // convert & audit a
 ar=AR(a); acr=jt->ranks>>RANKTX; acr=ar<acr?ar:acr; af=ar-acr;  // ?r=rank, ?cr=cell rank, ?f=length of frame
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK; I wt=AT(w);
 // special case: if a is atomic 0, and cells of w are not atomic
 if((-wcr&(ar-1))<0&&(IAV(a)[0]==0))R RETARG(w);   // 0 }. y, return y
 if(((af-1)&(acr-2))>=0){
  s=rank2ex(a,w,DUMMYSELF,MIN(acr,1),wcr,acr,wcr,jtdrop);  // if multiple x values, loop over them  af>0 or acr>1
  // We extracted from w, so mark it (or its backer if virtual) non-pristine.  There may be replication, so we don't pass pristinity through  We overwrite w because it is no longer in use
  PRISTCLRF(w)
  RETF(s);
 }
 n=AN(a); u=AV(a);     // n=#axes to drop, u->1st axis
 // virtual case: scalar a
 if(!(ar|wf|(((NOUN&~(DIRECT|RECURSIBLE))|SPARSE)&wt)|!wcr|(AFLAG(w)&(AFNJA)))){  // if there is only 1 take axis, w has no frame and is not atomic; and avoid virtualling NJA
  I * RESTRICT ws=AS(w);  // ws->shape of w
  I droplen = IAV(a)[0];  // get the one number in a, the take amount
  I dropabs = droplen<0?-droplen:droplen;  // ABS(droplen), but may be as high as IMIN
  I remlen = ws[0]-dropabs; remlen=remlen<0?0:remlen;  // length remaining after drop: (#y)-abs(x), 0 if overdrop
  // calculate offset
  I woffset = droplen<0?0:droplen;   // x if x pos, 0 if x neg.  May be out of bounds if overdrop, but there will be no elements
  // get length of a cell of w
  I wcellsize; PROD(wcellsize,wr-1,ws+1);  // size of a cell in atoms of w
  I offset = woffset * wcellsize;  // offset in bytes of the virtual data
  // allocate virtual block.  May use inplace w
  RZ(s = virtualip(w,offset,wr));    // allocate block
  // fill in shape.  s and w may be the same block, so ws is destroyed
  I* RESTRICT ss=AS(s); ss[0]=remlen; MCISH(ss+1,ws+1,MAX(wr-1,0));
  AN(s)=remlen*wcellsize;  // install # atoms
  // Any pristinity adjustment would be in virtualip.  But there isn't any
  RETF(s);
 }

   // length error if too many axes
 fauxblockINT(sfaux,4,1);
 if(wcr){ASSERT(n<=wcr,EVLENGTH);RZ(s=shape(w)); v=wf+AV(s); DO(n, d=u[i]; m=v[i]; m=d<0?m:-m; m+=d; v[i]=m&=REPSGN(m^d););}  // nonatomic w-cell: s is (w frame),(values of a clamped to within size), then convert to equivalent take
 else{fauxINT(s,sfaux,wr+n,1) v=AV(s); MCISH(v,AS(w),wf); v+=wf; DO(n, v[i]=!u[i];); RZ(w=reshape(s,w));}  // atomic w-cell: reshape w-cell  to result-cell shape, with axis length 0 or 1 as will be in result
 RZ(s=tk(s,w));
 // We extracted from w, so mark it (or its backer if virtual) non-pristine.  If w was pristine and inplaceable, transfer its pristine status to the result.  We overwrite w because it is no longer in use
 PRISTXFERF(s,w)
 RETF(s);
}


// create 1 cell of fill when head/tail of an array with no items (at the given rank)
static F1(jtrsh0){A x,y;I wcr,wf,wr,*ws;
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr; RESETRANK;
 ws=AS(w);
 RZ(x=vec(INT,wr-1,ws)); MCISH(wf+AV(x),ws+wf+1,wcr-1);
 RZ(w=jtsetfv1(jt,w,AT(w))); GA00(y,AT(w),1,0); MC(AV0(y),jt->fillv,bpnoun(AT(w)));
 R reshape(x,y);
 // not pristine
}

F1(jthead){I wcr,wf,wr;
 F1PREFIP;
 ARGCHK1(w);
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr;  // no RESETRANK so that we can pass rank into other code
 if(unlikely(!wcr)){RETF(RETARG(w))  // {."0, a NOP
 }else if(likely(AS(w)[wf]!=0)){  // if cell is atom, or cell has items - which means it's safe to calculate the size of a cell
  if(((-wf)|((AT(w)&(DIRECT|RECURSIBLE))-1)|(wr-2))>=0){  // frame=0, and DIRECT|RECURSIBLE, not sparse, and rank>1.  No gain in virtualizing an atom, and it messes up inplacing and allocation-size counting in the tests
   // just one cell (no frame).  Create a virtual block for it, at offset 0
   wcr--; wcr=(wcr<0)?wr:wcr;  // wcr=rank of cell being created
   A z; RZ(z=virtualip(w,0,wcr));  // allocate the cell.  Now fill in shape & #atoms
    // if w is empty we have to worry about overflow when calculating #atoms
   I zn; PROD(zn,wcr,AS(w)+1) MCISH(AS(z),AS(w)+1,wcr) AN(z)=zn;  // Since z and w may be the same, the copy destroys AS(w).  So calc zn first.  copy shape of CELL of w into z
   RETF(z);
  }else{
   // frame not 0, or non-virtualable type, or cell is an atom.  Use from.  Note that jt->ranks is still set, so this may produce multiple cells
   // left rank is garbage, but since zeroionei(0) is an atom it doesn't matter
   RETF(jtfrom(jtinplace,zeroionei(0),w,ds(CFROM)));  // could call jtfromi directly for non-sparse w
  }
 }else{RETF(ISSPARSE(AT(w))?irs2(num(0),take(num( 1),w),0L,0L,wcr,jtfrom):rsh0(w));  // sparse or cell of w is empty - create a cell of fills  jt->ranks is still set for use in take.  Left rank is garbage, but that's OK
 }
 // pristinity from the called verb
}

F1(jttail){I wcr,wf,wr;
 F1PREFIP;
 ARGCHK1(w);
 wr=AR(w); wcr=(RANKT)jt->ranks; wcr=wr<wcr?wr:wcr; wf=wr-wcr;  // no RESETRANK: rank is passed into from/take/rsh0.  Left rank is garbage but that's OK
 if(unlikely(!wcr)){RETF(RETARG(w))  // {:"0, a NOP
 }else if(likely(AS(w)[wf]!=0)){  // if cell is atom, or cell has items - which means it's safe to calculate the size of a cell
  if(((-wf)|((AT(w)&(DIRECT|RECURSIBLE))-1)|(wr-2))>=0){  // frame=0, and DIRECT|RECURSIBLE, not sparse, and rank>1.  No gain in virtualizing an atom, and it messes up inplacing and allocation-size counting in the tests
   // just one cell (no frame).  Create a virtual block for it, at offset of the last item
   wcr--; wcr=(wcr<0)?wr:wcr;  // wcr=rank of cell being created
   I zn; PROD(zn,wcr,AS(w)+1)
   A z; RZ(z=virtualip(w,(AS(w)[wf]-1)*zn,wcr));  // allocate the cell.  Now fill in shape & #atoms
    // if w is empty we have to worry about overflow when calculating #atoms
   MCISH(AS(z),AS(w)+1,wcr) AN(z)=zn;  // Since z and w may be the same, the copy destroys AS(w).  So calc zn first.  copy shape of CELL of w into z
   RETF(z);
  }else{
   // frame not 0, or non-virtualable type, or cell is an atom.  Use from.  Note that jt->ranks is still set, so this may produce multiple cells
   // left rank is garbage, but since num(-1) is an atom it doesn't matter
   RETF(jtfrom(jtinplace,num(-1),w,ds(CFROM)));  // could call jtfromi directly for non-sparse w
  }
 }else{RETF(ISSPARSE(AT(w))?irs2(num(0),take(num(-1),w),0L,0L,wcr,jtfrom):rsh0(w));  // sparse or cell of w is empty - create a cell of fills  jt->ranks is still set for use in take.  Left rank is garbage, but that's OK
 }
 // pristinity from other verbs
}
