/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Take and Drop                                                    */

#include "j.h"


F1(jtbehead ){F1PREFIP; R jtdrop(jtinplace,onei,    w);}
F1(jtcurtail){F1PREFIP; R jtdrop(jtinplace,num[-1],w);}

F1(jtshift1){R drop(num[-1],over(one,w));}

static A jttk0(J jt,B b,A a,A w){A z;I k,m=0,n,p,r,*s,*u;
 r=AR(w); n=AN(a); u=AV(a); 
 if(!b){RE(m=prod(n,u)); ASSERT(m>IMIN,EVLIMIT); RE(m=mult(ABS(m),prod(r-n,n+AS(w))));}
 GA(z,AT(w),m,r,AS(w)); 
 s=AS(z); DO(n, p=u[i]; ASSERT(p>IMIN,EVLIMIT); *s++=ABS(p););
 if(m){k=bp(AT(w)); mvc(k*m,AV(z),k,jt->fillv);}
 R z;
}

static F2(jttks){PROLOG(0092);A a1,q,x,y,z;B b,c;I an,m,r,*s,*u,*v;P*wp,*zp;
 an=AN(a); u=AV(a); r=AR(w); s=AS(w); 
 GA(z,AT(w),1,r,s); v=AS(z); DO(an, v[i]=ABS(u[i]););
 zp=PAV(z); wp=PAV(w);
 if(an<=r){RZ(a=vec(INT,r,s)); ICPY(AV(a),u,an);}
 a1=SPA(wp,a); RZ(q=paxis(r,a1)); m=AN(a1);
 RZ(a=from(q,a       )); u=AV(a);
 RZ(y=from(q,shape(w))); s=AV(y);
 b=0; DO(r-m, if(b=u[i+m]!=s[i+m])break;);
 c=0; DO(m,   if(c=u[i  ]!=s[i  ])break;);
 if(b){jt->fill=SPA(wp,e); x=irs2(vec(INT,r-m,m+u),SPA(wp,x),0L,1L,-1L,jttake); jt->fill=0; RZ(x);}  // fill cannot be virtual
 else x=SPA(wp,x);
 if(c){A j;C*xv,*yv;I d,i,*iv,*jv,k,n,t;
  d=0; t=AT(x); k=bp(t)*aii(x);
  q=SPA(wp,i); n=IC(q);
  GATV(j,INT,AN(q),AR(q),AS(q)); jv= AV(j); iv= AV(q);
  GA(y,t,  AN(x),AR(x),AS(x)); yv=CAV(y); xv=CAV(x);
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

// general take routine.  a is take values, w is array
static F2(jttk){PROLOG(0093);A y,z;B b=0;C*yv,*zv;I c,d,dy,dz,e,i,k,m,n,p,q,r,*s,t,*u;
 n=AN(a); u=AV(a); r=AR(w); s=AS(w); t=AT(w);
 if(t&SPARSE)R tks(a,w);
 DO(n, if(!u[i]){b=1; break;}); if(!b)DO(r-n, if(!s[n+i]){b=1; break;});
 if(b||!AN(w))R tk0(b,a,w);
 k=bp(t); z=w; c=q=1;
 // process take one axis at a time
 for(i=0;i<n;++i){
  c*=q; p=u[i]; q=ABS(p); m=s[i];  // q can be IMIN out of this
  if(q!=m){
   RE(d=mult(AN(z)/m,q)); GA(y,t,d,r,AS(z)); *(i+AS(y))=q;  // this catches q=IMIN
   if(q>m)mvc(k*AN(y),CAV(y),k,jt->fillv);
   d=AN(z)/(m*c)*k; e=d*MIN(m,q);
   dy=d*q; yv=CAV(y); if(0>p&&q>m)yv+=d*(q-m);
   dz=d*m; zv=CAV(z); if(0>p&&m>q)zv+=d*(m-q);
   DO(c, MC(yv,zv,e); yv+=dy; zv+=dz;);
   b=1; z=y;
  }
 }
 if(!b)z=ca(w);   // todo kludge no need to ca()
 RELOCATE(w,z);   // if w was relative, rebase the cells on z
 EPILOG(z);
}

F2(jttake){A s,t;D*av,d;I acr,af,ar,n,*tv,*v,wcr,wf,wr;
 F2PREFIP;
 RZ(a&&w); I wt = AT(w);  // wt=type of w
 if(SPARSE&AT(a))RZ(a=denseit(a));
 if(!(SPARSE&wt))RZ(w=setfv(w,w)); 
 ar=AR(a); acr=jt->rank?jt->rank[0]:ar; af=ar-acr;  // ?r=rank, ?cr=cell rank, ?f=length of frame
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr; jt->rank=0;
 if(af||1<acr)R rank2ex(a,w,0L,1L,RMAX,acr,wcr,jttake);  // if multiple x values, loop over them
 // canonicalize x
 n=AN(a);    // n = #axes in a
 ASSERT(!wcr||n<=wcr,EVLENGTH);  // if y is not atomic, a must not have extra axes
 I * RESTRICT ws=AS(w);  // ws->shape of w
 if(AT(a)&B01+INT)RZ(s=a=vi(a))  // convert boolean arg to int
 else{
  RZ(t=vib(a));   // convert to int in new buffer
  if(!(AT(a)&FL))RZ(a=cvt(FL,a));  // convert complex to float
  av=DAV(a); tv=AV(t); v=wf+ws;
  DO(n, d=av[i]; if(d==IMIN)tv[i]=(I)d; else if(INF(d))tv[i]=wcr?v[i]:1;)  // replace infinities in original with high- or low-value
  s=a=t;
 }
 // scaf no virtual if not direct or recursible
// correct if(!(ar|wf|(SPARSE&wt)|!wcr|(AFLAG(w)&(AFNJA|AFSMM)))){  // if there is only 1 take axis, w has no frame and is not atomic
 if(!(ar|wf|((NOUN&~(DIRECT|RECURSIBLE))&wt)|!wcr|(AFLAG(w)&(AFNJA|AFSMM)))){  // if there is only 1 take axis, w has no frame and is not atomic
  // if the length of take is within the bounds of the first axis
  I tklen = IAV(a)[0];  // get the one number in a, the take amount
  I tkasign = tklen>>(BW-1);  // 0 if tklen nonneg, ~0 if neg
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
   // fill in shape
   I* RESTRICT ss=AS(s); ss[0]=tkabs; DO(wr-1, ss[i+1]=ws[i+1];);  // shape of virtual matches shape of w except for #items
   AN(s)=tkabs*wcellsize;  // install # atoms
   RETF(s);
  }
 }
 // full processing for more complex a
 if(!wcr||wf){   // if y is an atom, or y has multiple cells:
  RZ(s=vec(INT,wf+n,AS(w))); v=wf+AV(s);   // s is a block holding shape of a cell of input to the result: w-frame followed by #$a axes, all taken from w
  if(!wcr){DO(n,v[i]=1;); RZ(w=reshape(s,w));}  // if w is an atom, change it to a singleton of rank #$a
  ICPY(v,AV(a),n);   // whether w was an atom or not, replace the axes of w-cell with values from a.  This leaves s with the final shape of the result
 }
 R tk(s,w);  // go do the general take/drop
}

F2(jtdrop){A s;I acr,af,ar,d,m,n,*u,*v,wcr,wf,wr;
 F2PREFIP;
 RZ((a=vib(a))&&w);  // convert & audit a
 ar=AR(a); acr=jt->rank?jt->rank[0]:ar; af=ar-acr;      // ?r=rank, ?cr=cell rank, ?f=length of frame
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr; jt->rank=0; I wt=AT(w);
 // special case: if a is atomic 0, and cells of w are not atomic
 if(wcr&&!ar&&(IAV(a)[0]==0))R RETARG(w);   // 0 }. y, return y
 if(af||1<acr)R rank2ex(a,w,0L,1L,RMAX,acr,wcr,jtdrop);  // if multiple x values, loop over them
 n=AN(a); u=AV(a);     // n=#axes to drop, u->1st axis
 // virtual case: scalar a
// correct if(!(ar|wf|(SPARSE&wt)|!wcr|(AFLAG(w)&(AFNJA|AFSMM)))){  // if there is only 1 take axis, w has no frame and is not atomic
 // scaf not virtual if not direct or recursible
 if(!(ar|wf|((NOUN&~(DIRECT|RECURSIBLE))&wt)|!wcr|(AFLAG(w)&(AFNJA|AFSMM)))){  // if there is only 1 take axis, w has no frame and is not atomic
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
  // fill in shape
  I* RESTRICT ss=AS(s); ss[0]=remlen; DO(wr-1, ss[i+1]=ws[i+1];);  // shape of virtual matches shape of w except for #items
  AN(s)=remlen*wcellsize;  // install # atoms
  RETF(s);
 }

   // length error if too many axes
 // obsolete if(wcr){ASSERT(n<=wcr,EVLENGTH);RZ(s=shape(w)); v=wf+AV(s); DO(n, d=u[i]; m=v[i]; v[i]=d<-m?0:d<0?d+m:d<m?d-m:0;);}  // nonatomic w-cell: s is (w frame),(values of a clamped to within size), then convert to equivalent take
 if(wcr){ASSERT(n<=wcr,EVLENGTH);RZ(s=shape(w)); v=wf+AV(s); DO(n, d=u[i]; m=v[i]; m=d<0?m:-m; m+=d; v[i]=m&=((m^d)>>(BW-1)););}  // nonatomic w-cell: s is (w frame),(values of a clamped to within size), then convert to equivalent take
 else{GATV(s,INT,wr+n,1,0); v=AV(s); ICPY(v,AS(w),wf); v+=wf; DO(n, v[i]=!u[i];); RZ(w=reshape(s,w));}  // atomic w-cell: reshape w-cell  to result-cell shape, with axis length 0 or 1 as will be in result
 R tk(s,w);
}


static F1(jtrsh0){A x,y;I wcr,wf,wr,*ws;
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr; jt->rank=0;
 ws=AS(w);
 RZ(x=vec(INT,wr-1,ws)); ICPY(wf+AV(x),ws+wf+1,wcr-1);
 RZ(w=setfv(w,w)); GA(y,AT(w),1,0,0); MC(AV(y),jt->fillv,bp(AT(w)));
 R reshape(x,y);
}

F1(jthead){I wcr,wf,wr;
 F1PREFIP;
 RZ(w);
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr;
 R !wcr||*(wf+AS(w))? jtfrom(jtinplace,zeroi,w) : 
     SPARSE&AT(w)?irs2(num[0],take(num[ 1],w),0L,0L,wcr,jtfrom):rsh0(w);
}

F1(jttail){I wcr,wf,wr;
 F1PREFIP;
 RZ(w);
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr;
 R !wcr||*(wf+AS(w))?jtfrom(jtinplace,num[-1],w) :
     SPARSE&AT(w)?irs2(num[0],take(num[-1],w),0L,0L,wcr,jtfrom):rsh0(w);
}
