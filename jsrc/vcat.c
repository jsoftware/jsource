/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Catenate and Friends                                             */

#include "j.h"


static A jtovs0(J jt,B p,I r,A a,A w){A a1,e,q,x,y,z;B*b;I at,*av,c,d,j,k,f,m,n,t,*v,wr,*ws,wt,zr;P*wp,*zp;
 ws=AS(w); wr=AR(w); f=wr-r; zr=wr+!r;
 ASSERT(IMAX>ws[f],EVLIMIT);
 wp=PAV(w); e=SPA(wp,e); x=SPA(wp,x); y=SPA(wp,i); m=*AS(y); 
 a1=SPA(wp,a); c=AN(a1); av=AV(a1); RZ(b=bfi(zr,a1,1));
 at=AT(a); wt=AT(x);
 ASSERT(HOMO(at,wt),EVDOMAIN);
 t=maxtype(at,wt);
 ASSERT(t&(B01|INT|FL|CMPX),EVDOMAIN);  // verify supported sparse type
 if(t!=at)RZ(a=cvt(t,a));
 if(t!=wt){RZ(x=cvt(t,x)); RZ(e=cvt(t,e));}
 j=k=0; DO(f, if(b[i])++j; else ++k;);
 switch(2*b[f]+!equ(a,e)){
  case 0:  /* dense and a equal e */
   RZ(y=ca(y)); 
   RZ(x=p?irs2(x,a,0L,AR(x)-(1+k),0L,jtover):irs2(a,x,0L,0L,AR(x)-(1+k),jtover)); 
   break;
  case 1:  /* dense and a not equal to e */
   GA(q,INT,c,1,0); v=AV(q); DO(c, v[i]=ws[av[i]];); RZ(q=odom(2L,c,v));
   if(AN(q)>=AN(y)){
    RZ(z=shape(x)); *AV(z)=*AS(q); 
    RZ(x=from(grade1(over(y,less(q,y))),over(x,reshape(z,e))));
    y=q;
   }
   RZ(x=p?irs2(x,a,0L,AR(x)-(1+k),0L,jtover):irs2(a,x,0L,0L,AR(x)-(1+k),jtover));
   break;
  case 2:  /* sparse and a equals e */
   RZ(y=ca(y)); 
   if(!p){v=j+AV(y); DO(m, ++*v; v+=c;);} 
   break;
  case 3:  /* sparse and a not equal to e */
   GA(q,INT,c,1,0); v=AV(q); DO(c, v[i]=ws[av[i]];); v[j]=1; RZ(q=odom(2L,c,v)); n=*AS(q);
   if(p){RZ(y=over(y,q)); v=AV(y)+j+m*c; d=ws[f]; DO(n, *v=d; v+=c;);}
   else {RZ(y=over(q,y)); v=AV(y)+j+n*c;          DO(m, ++*v; v+=c;);}
   RZ(q=shape(x)); *AV(q)=n; RZ(q=reshape(q,a)); RZ(x=p?over(x,q):over(q,x));
   if(f){RZ(q=grade1(y)); RZ(y=from(q,y)); RZ(x=from(q,x));}
 }
 GA(z,STYPE(t),1,zr,ws); 
 if(r)++*(f+AS(z)); else *(wr+AS(z))=2;
 zp=PAV(z); SPB(zp,a,ifb(zr,b)); SPB(zp,e,e); SPB(zp,i,y); SPB(zp,x,x);
 R z;
}    /* a,"r w (0=p) or w,"r a (1=p) where a is scalar */

static F2(jtovs){A ae,ax,ay,q,we,wx,wy,x,y,z,za,ze;B*ab,*wb,*zb;I acr,ar,*as,at,c,m,n,r,t,*v,wcr,wr,*ws,wt,*zs;P*ap,*wp,*zp;
 RZ(a&&w);
 at=AT(a); ar=AR(a); acr=jt->rank?jt->rank[0]:ar;
 wt=AT(w); wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; jt->rank=0;
 if(!ar)R ovs0(0,wcr,a,w);
 if(!wr)R ovs0(1,acr,w,a);
 if(ar>acr||wr>wcr)R sprank2(a,w,0L,acr,wcr,jtover);
 r=MAX(ar,wr);
 if(r>ar)RZ(a=reshape(over(apv(r-ar,1L,0L),shape(a)),a)); as=AS(a);
 if(r>wr)RZ(w=reshape(over(apv(r-wr,1L,0L),shape(w)),w)); ws=AS(w);
 ASSERT(*as<IMAX-*ws,EVLIMIT);
 if(!(at&SPARSE)){wp=PAV(w); RZ(a=sparseit(a,SPA(wp,a),SPA(wp,e)));}
 if(!(wt&SPARSE)){ap=PAV(a); RZ(w=sparseit(w,SPA(ap,a),SPA(ap,e)));}
 ap=PAV(a); RZ(ab=bfi(r,SPA(ap,a),1)); ae=SPA(ap,e); at=AT(ae);
 wp=PAV(w); RZ(wb=bfi(r,SPA(wp,a),1)); we=SPA(wp,e); wt=AT(we);
 ASSERT(equ(ae,we),EVNONCE);
 GA(q,B01,r,1,0); zb=BAV(q); DO(r, zb[i]=ab[i]||wb[i];); RZ(za=ifb(r,zb)); c=AN(za);
 GA(q,INT,r,1,0); zs= AV(q); DO(r, zs[i]=MAX(as[i],ws[i]););
 DO(r, if(zb[i]>ab[i]){RZ(a=reaxis(za,a)); break;});
 DO(r, if(zb[i]>wb[i]){RZ(w=reaxis(za,w)); break;});
 *zs=*as; DO(r, if(zs[i]>as[i]){RZ(a=take(q,a)); break;});
 *zs=*ws; DO(r, if(zs[i]>ws[i]){RZ(w=take(q,w)); break;});
 *zs=*as+*ws; t=maxtype(at,wt);
 ap=PAV(a); ay=SPA(ap,i); ax=SPA(ap,x); if(t!=at)RZ(ax=cvt(t,ax));
 wp=PAV(w); wy=SPA(wp,i); wx=SPA(wp,x); if(t!=at)RZ(wx=cvt(t,wx));
 GA(z,STYPE(t),1,r,zs); zp=PAV(z);
 SPB(zp,a,za); SPB(zp,e,ze=ca(t==at?ae:we));
 if(*zb){
  SPB(zp,x,  over(ax,wx));
  SPB(zp,i,y=over(ay,wy)); v=AV(y)+AN(ay); m=*as; DO(*AS(wy), *v+=m; v+=c;);
 }else{C*av,*wv,*xv;I am,ak,i,j,k,mn,p,*u,wk,wm,xk,*yv;
  i=j=p=0; k=bp(t); 
  m=*AS(ay); u=AV(ay); av=CAV(ax); am=aii(ax); ak=k*am;
  n=*AS(wy); v=AV(wy); wv=CAV(wx); wm=aii(wx); wk=k*wm; mn=m+n; xk=k*(am+wm);
  GA(y,INT,mn*c,      2,     AS(ay)); yv= AV(y); *AS(y)=mn;                 
  GA(x,t,  mn*(am+wm),AR(ax),AS(ax)); xv=CAV(x); *AS(x)=mn; *(1+AS(x))=*zs; mvc(k*AN(x),xv,k,AV(ze));
  while(i<m||j<n){I cmp;
   if     (i==m)cmp= 1; 
   else if(j==n)cmp=-1;
   else         {cmp=0; DO(c, if(u[i]!=v[i]){cmp=u[i]<v[i]?-1:1; break;});}
   switch(cmp){
    case -1: ICPY(yv,u,c); u+=c; ++i; memcpy(xv,   av,ak); av+=ak; break;
    case  0: ICPY(yv,u,c); u+=c; ++i; memcpy(xv,   av,ak); av+=ak; ++p;  /* fall thru */
    case  1: ICPY(yv,v,c); v+=c; ++j; memcpy(xv+ak,wv,wk); wv+=wk;
   }
   yv+=c; xv+=xk;
  }
  SPB(zp,i,p?take(sc(mn-p),y):y); SPB(zp,x,p?take(sc(mn-p),x):x);
 }
 R z;
}    /* a,"r w where a or w or both are sparse */


static C*jtovgmove(J jt,I k,I c,I m,A s,A w,C*x,A z){B b;I d,n,p=c*m,q,*u,*v;
 b=ARELATIVE(z);
 if(AR(w)){
  n=AN(w); d=AN(s)-AR(w);
  if((!n||d)&&!b)mvc(k*p,x,k,jt->fillv);
  if(n&&n<p){v=AV(s); *v=m; RZ(w=take(d?vec(INT,AR(w),d+v):s,w));}
  if(n){
   if(b){q=ARELATIVE(w)*(I)w-(I)z; u=(I*)x; v=AV(w); DO(AN(w), *u++=q+*v++;);}
   else MC(x,AV(w),k*AN(w));
  }
 }else{
  if(b){q=*AV(w)+ARELATIVE(w)*(I)w-(I)z; u=(I*)x; DO(p, *u++=q;);} 
  else mvc(k*p,x,k,AV(w));
 }
 R x+k*p;
}    /* move an argument into the result area */

static F2(jtovg){A s,z;C*x;I ar,*as,c,k,m,n,q,r,*sv,wr,*ws,zn;
 RZ(a&&w);
 RZ(w=setfv(a,w)); RZ(coerce2(&a,&w,0L));
 ar=AR(a); wr=AR(w); r=ar+wr?MAX(ar,wr):1;
 RZ(s=r?vec(INT,r,r==ar?AS(a):AS(w)):num[2]); sv=AV(s);
 if(m=MIN(ar,wr)){
  as=ar+AS(a); ws=wr+AS(w); k=r;
  DO(m, --as; --ws; sv[--k]=MAX(*as,*ws);); 
  DO(r-m, sv[i]=MAX(1,sv[i]););
 }
 RE(c=prod(r-1,1+sv)); m=r>ar?1:IC(a); n=r>wr?1:IC(w); 
 RE(zn=mult(c,m+n)); ASSERT(0<=m+n,EVLIMIT);
 GA(z,AT(a),zn,r,sv); *AS(z)=m+n; x=CAV(z); k=bp(AT(a));
 if(ARELATIVE(a)||ARELATIVE(w)){AFLAG(z)=AFREL; q=(I)jt->fillv+(I)w-(I)z; mvc(k*zn,x,k,&q);}
 RZ(x=ovgmove(k,c,m,s,a,x,z));
 RZ(x=ovgmove(k,c,n,s,w,x,z));
 R z;
}    /* a,w general case for array with the same type; jt->rank=0 */

static F2(jtovv){A z;I m,t;
 t=AT(a); 
 GA(z,t,AN(a)+AN(w),1,0);  
 if(t&BOX){A1*u,*v;B p,q,r;
  p=ARELATIVE(a); q=ARELATIVE(w); r=p||q; if(r)AFLAG(z)=AFREL; v=A1AV(z);
  u=A1AV(a); m=p*(I)a-r*(I)z; DO(AN(a), *v++=m+*u++;);
  u=A1AV(w); m=q*(I)w-r*(I)z; DO(AN(w), *v++=m+*u++;);
 }else{C*x;I k;
  k=bp(t); m=k*AN(a); x=CAV(z); 
  MC(x,  AV(a),m      ); 
  MC(x+m,AV(w),k*AN(w));
 }
 R z;
}    /* a,w for vectors/scalars with the same type */

static void om(I k,I c,I d,I m,I m1,I n,I r,C*u,C*v){I e,km,km1,kn;
 e=c/d; km=k*m; km1=k*m1; kn=k*n;
 if(!r&&m1!=n)DO(c, mvc(km1,u,kn,v); u+=km;)
 else if(1<e){
  if(m1>n)DO(c/e, DO(e, mvc(km1,u,kn,v); u+=km;); v+=kn;)
  else    DO(c/e, DO(e, MC(u,v,kn);      u+=km;); v+=kn;);
 }else{
  if(m1>n)DO(c,         mvc(km1,u,kn,v); u+=km;   v+=kn;)
  else    DO(c,         MC(u,v,kn);      u+=km;   v+=kn;); 
}}   /* move an argument into the result area */


F2(jtover){A z;B b;C*zv;I acn,acr,af,ar,*as,c,f,k,m,ma,mw,p,q,r,*s,t,wcn,wcr,wf,wr,*ws,zn;
 RZ(a&&w);
 if(SPARSE&AT(a)||SPARSE&AT(w))R ovs(a,w);  // if either arg is sparse, switch to sparse code
 RZ(t=coerce2(&a,&w,0L));  // convert args to compatible precisions, changing a and w if needed
 ar=AR(a); wr=AR(w);
 if(!jt->rank&&2>ar&&2>wr)R ovv(a,w);
 acr=jt->rank?jt->rank[0]:ar; af=ar-acr; as=AS(a); p=acr?as[ar-1]:1;
 wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr; ws=AS(w); q=wcr?ws[wr-1]:1;
 r=acr+wcr?MAX(acr,wcr):1;
 if(2<r||!AN(a)||!AN(w)||2<acr+wcr&&p!=q||ARELATIVE(a)||ARELATIVE(w)){
  jt->rank=0; R rank2ex(a,w,0L,acr,wcr,jtovg);
 }
 acn=1>=acr?p:p*as[af+acr-2]; ma=!acr&&2==wcr?q:acn;
 wcn=1>=wcr?q:q*ws[wf+wcr-2]; mw=!wcr&&2==acr?p:wcn; m=ma+mw; 
 b=af<=wf; f=b?wf:af; s=b?ws:as; RE(c=prod(f,s)); RE(zn=mult(c,m));
 GA(z,t,zn,f+r,s); zv=CAV(z); s=AS(z)+AR(z)-1; 
 if(2>r)*s=m; else{*s=acr?p:q; *(s-1)=(1<acr?as[ar-2]:1)+(1<wcr?ws[wr-2]:1);}
 k=bp(t);
 om(k,c,prod(af,as),m,ma,acn,ar,zv,     CAV(a)); 
 om(k,c,prod(wf,ws),m,mw,wcn,wr,zv+k*ma,CAV(w));
 R z;
}    /* overall control, and a,w and a,"r w for cell rank <: 2 */

F2(jtstitch){B sp2;I ar,wr;
 RZ(a&&w);
 ar=AR(a); wr=AR(w); sp2=(SPARSE&AT(a)||SPARSE&AT(w))&&2>=ar&&2>=wr;
 ASSERT(!ar||!wr||*AS(a)==*AS(w),EVLENGTH);
 R sp2 ? stitchsp2(a,w) : irs2(a,w,0L,ar?ar-1:0,wr?wr-1:0,jtover);
}

F1(jtlamin1){A x;I*s,*v,wcr,wf,wr; 
 RZ(w);
 wr=wcr=AR(w); if(jt->rank){wcr=MIN(wr,jt->rank[1]); jt->rank=0;} wf=wr-wcr;
 GA(x,INT,1+wr,1,0); v=AV(x);
 s=AS(w); ICPY(v,s,wf); *(v+wf)=1; ICPY(v+1+wf,s+wf,wcr);
 R reshape(x,w);
}    /* ,:"r w */

F2(jtlamin2){A z;I ar,p,q,wr;
 RZ(a&&w); 
 ar=AR(a); p=jt->rank?jt->rank[0]:ar; 
 wr=AR(w); q=jt->rank?jt->rank[1]:wr; 
 if(p)a=irs1(a,0L,p,jtlamin1);
 if(q)w=irs1(w,0L,q,jtlamin1);
 z=irs2(a,w,0L,p+!!p,q+!!q,jtover);
 if(!p&&!q)z=irs1(z,0L,0L,jtlamin1);
 R z;
}    /* a,:"r w */

// Return 0 if it's ok to in-place this name, 1 if not.  It's not if the name in h
// exists (meaning it is target is local assignment) but is not defined
I jtpiplocalerr(J jt, A self){
  A nm = VAV(self)->h;
  if(nm&&jt->local)R !probe(nm,jt->local);  // if name defined, and there are local names, see if it's defined
  R 0;  // OK otherwise
}

// append-in-place.  We can only append if the buffer is not in use more than once, and if the
// datatype is direct.  Also, we can't append if the name was a local name that is not defined,
// since that would append-in-place to the global value
DF2(jtapip){RZ(a&&w);R AC(a)>(AFNJA&AFLAG(a)?2:1)||!(DIRECT&AT(a))||jtpiplocalerr(jt,self)?over(a,w):apipx(a,w);}

F2(jtapipx){A h;C*av,*wv;I ak,at,ar,*as,k,p,*u,*v,wk,wm,wn,wt,wr,*ws;
 RZ(a&&w);
 at=AT(a); ar=AR(a); as=AS(a);
 wt=AT(w); wr=AR(w); ws=AS(w); p=-1;
 if(AN(a)&&ar&&ar>=wr&&at>=wt&&5e8>AC(a)){
  p=0; u=as+ar-wr; v=ws; if(ar==wr){++u; ++v;}
  DO(wr-(ar==wr), k=*u++-*v++; if(0<k)p=1; else if(0>k){p=-1; break;});
  k=bp(at); ak=k*AN(a); wm=ar==wr?*ws:1; wn=wm*aii(a); wk=k*wn;
 }
 if(0<=p&&AM(a)>=ak+wk+(1&&at&LAST0)){
  if(at>wt)RZ(w=cvt(at,w));
  if(p){RZ(h=vec(INT,wr,as+ar-wr)); if(ar==wr)*AV(h)=*ws; RZ(w=take(h,w));}
  av=ak+CAV(a); wv=CAV(w); 
  if(wr&&ar>1+wr){RZ(setfv(a,w)); mvc(wk,av,k,jt->fillv);}
  if(wr)MC(av,wv,k*AN(w)); else mvc(wk,av,k,wv);
  *as+=wm; AN(a)+=wn; if(at&LAST0)*(av+wk)=0;
 }else RZ(a=over(a,w));
 R a;
}    /* append in place if possible */
