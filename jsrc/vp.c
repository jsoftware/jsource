/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Permutations                                                     */

#include "j.h"


static I jtord(J jt,A w){I j,n,*v,z;
 RZ(w);
 n=AN(w); z=-n;
 if(n){if(!(INT&AT(w)))RZ(w=cvt(INT,w)); v=AV(w); DO(n, j=*v++; if(z<j)z=j;); ++z;}
 R z;
}

F1(jtpinv){I m=0,n,*v;
 F1RANK(1,jtpinv,0);
 RZ(w=vi(w));
 n=AN(w); v=AV(w);
 DO(n, m=0>v[i]?MAX(m,-1-v[i]):MAX(m,v[i]);); m+=0<n;
 R indexof(pfill(m,w),IX(m));
}    /* permutation inverse */

A jtpind(J jt,I n,A w){A z;I j,m,*v;
 RE(n); RZ(w);
 m=-n;
 RZ(z=ca(vi(w))); v=AV(z);
 DO(AN(z), j=*v; ASSERT(m<=j&&j<n,EVINDEX); *v++=0>j?j+n:j;);
 R z;
}    /* positive indices */

A jtpfill(J jt,I n,A w){PROLOG(0081);A b,z;B*bv,*v;I*wv,*zv;
 RZ(w=pind(n,w));  wv=AV(w);
 GATV(z,INT,n,1,0); zv=AV(z);
 GATV(b,B01,n,1,0); bv=BAV(b); memset(bv,C1,n);
 DO(AN(w), v=bv+wv[i]; ASSERT(*v,EVINDEX); *v=0;);
 DO(n, if(bv[i])*zv++=i;); ICPY(zv,wv,AN(w));
 EPILOG(z);
}

static F1(jtcfd){A b,q,x,z,*zv;B*bv;I c,i,j,n,*qv,*u,*v,zn;
 RZ(w);
 if(c=1&&INT&AT(w)){
  n=AN(w); v=AV(w);
  GATV(b,B01,1+n,1,0); bv=BAV(b); memset(bv,C0,n);
  DO(n, j=v[i]; if(j<0||n<=j||bv[j]){c=0; break;} bv[j]=1;);
 }
 if(!c){n=ord(w); RZ(w=pfill(n,w)); v=AV(w); GATV(b,B01,1+n,1,0);}
 bv=BAV(b); memset(bv,C0,1+n); ++bv;
 i=0; j=n-1; zn=(I)(log((D)n)+1.6); 
 GATV(q,INT,n, 1,0); qv= AV(q);
 GATV(z,BOX,zn,1,0); zv=AAV(z);
 while(1){
  while(bv[j])--j; if(0>j)break;
  u=qv; c=j;
  do{bv[c]=1; *u++=c; c=v[c];}while(c!=j);
  if(i==zn){RZ(z=ext(0,z)); zv=AAV(z); zn=AN(z);}
  RZ(zv[i++]=vec(INT,u-qv,qv));
 }
 AN(z)=*AS(z)=zn=i; j=zn-1; DO(zn/2, x=zv[i]; zv[i]=zv[j]; zv[j]=x; --j;);
 R z;
}    /* cycle from direct */

static A jtdfc(J jt,I n,A w){PROLOG(0082);A b,q,*wv,z;B*bv;I c,j,qn,*qv,*x,wd;
 RE(n); RZ(w);
 ASSERT(0<=n,EVINDEX);
 GATV(b,B01,n,1,0); bv=BAV(b); memset(bv,C1,n);
 RZ(z=IX(n)); x=AV(z);
 wv=AAV(w); wd=(I)w*ARELATIVE(w);
 for(j=AN(w)-1;0<=j;j--){
  RZ(q=pind(n,WVR(j))); qv=AV(q); qn=AN(q);
  if(!qn)continue;
  DO(qn, ASSERT(bv[qv[i]],EVINDEX); bv[qv[i]]=0;); DO(qn,bv[qv[i]]=1;);
  c=x[qv[0]]; DO(qn-1,x[qv[i]]=x[qv[i+1]];); x[qv[qn-1]]=c;
 }
 EPILOG(z);
}    /* direct from cycle */

F1(jtcdot1){F1RANK(1,jtcdot1,0); R BOX&AT(w)?dfc(ord(raze(w)),w):cfd(w);}

F2(jtcdot2){A p;
 F2RANK(1,RMAX,jtcdot2,0);
 RZ(p=BOX&AT(a)?dfc(IC(w),a):pfill(IC(w),a));
 R AR(w)?from(p,w):w;
}

F1(jtpparity){A x,y,z;B p,*u;I i,j,k,m,n,r,*s,*v,*zv;
 RZ(x=cvt(INT,w));
 r=AR(x); s=AS(x); n=r?*(s+r-1):1; RE(m=prod(r-1,s)); v=AV(x);
 GATV(y,B01,n,1,0); u=BAV(y);
 GATV(z,INT,m,r?r-1:0,s); zv=AV(z);
 for(i=0;i<m;++i){
  j=p=0; memset(u,C1,n);
  DO(n, k=v[i]; if(0>k)v[i]=k+=n; if(0<=k&&k<n&&u[k])u[k]=0; else{j=1+n; break;});
  for(;j<n;++j)if(j!=v[j]){k=j; DO(n-j-1, ++k; if(j==v[k]){v[k]=v[j]; p=!p; break;});}
  zv[i]=p?-1:j==n; 
  v+=n;
 }
 R z;
}    /* permutation parity; # interchanges to get i.n */

static F1(jtdfr){A z;I c,d,i,j,m,n,*v,*x;
 RZ(w);
 n=*(AS(w)+AR(w)-1); m=n?AN(w)/n:0; v=AV(w);
 GATV(z,INT,AN(w),AR(w),AS(w)); x=AV(z);
 for(i=0;i<m;++i){
  DO(n, x[i]=i;);
  DO(n-1, j=i; c=x[j+v[j]]; DO(1+v[j], d=x[j+i]; x[j+i]=c; c=d;););
  x+=n; v+=n;
 }
 R z;
}    /* direct from reduced */

static F1(jtrfd){A z;I j,k,n,r,*s,*x;
 RZ(z=ca(w)); x=AV(z);
 r=AR(w); s=AS(w); 
 if(n=s[r-1])DO(AN(w)/n, j=n-1; ++x; DO(n-1, k=0; DO(j--, k+=*x>x[i];); *x++=k;););
 R z;
}    /* reduced from direct */

F1(jtadot1){A y;I n;
 F1RANK(1,jtadot1,0);
 RZ(y=BOX&AT(w)?cdot1(w):pfill(ord(w),w));
 n=IC(y);
 R base2(cvt(XNUM,apv(n,n,-1L)),rfd(y));
}

F2(jtadot2){A m,p;I n;
 RZ(a&&w);
 n=IC(w); p=sc(n); if(XNUM&AT(a))p=cvt(XNUM,p); RZ(m=fact(p));
 ASSERT(all1(le(negate(m),a))&&all1(lt(a,m)),EVINDEX);
 if(!AR(w)){RZ(vi(a)); R ca(w);}
 RZ(p=dfr(vi(abase2(apv(n,n,-1L),a))));
 R equ(w,IX(n))?p:from(p,w);
}

