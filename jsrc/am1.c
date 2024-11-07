/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: a ind}z for sparse z & ind is box a0;a1;a2;... or integer      */

/*  am1e    a is the sparse element                                        */
/*  am1a    a is an arbitrary dense array                                  */
/*  am1sp   a an arbitrary sparse array                                    */

#include "j.h"


static A jtistd1(J jt,A z,A ind){A*iv,j,*jv,x;I d,i,n,r,*s;
 RZ(z&&ind);
 ASSERT(1>=AR(ind),EVRANK);
 if(AN(ind)&&!(BOX&AT(ind))){ASSERT(NUMERIC&AT(ind),EVINDEX); RZ(ind=every(ind,ds(CRIGHT)));}
 s=AS(z); n=AN(ind); iv=AAV(ind); 
 ASSERT(n<=AR(z),EVINDEX);
 DQ(n, x=C(iv[i]); if(!equ(x,ds(CACE)))break;--n;);
 GATV0(j,BOX,n,1); jv=AAV1(j);
 for(i=0;i<n;++i){
  x=C(iv[i]); d=s[i];
  if(BOX&AT(x)){
   ASSERT(!AR(x),EVINDEX); 
   x=C(AAV(x)[0]); r=AR(x);
   RZ(jv[i]=!AN(x)&&1==r?ds(CACE):less(IX(d),pind(d,x)));
  }else {RZ(x=pind(d,x)); jv[i]=x;}  // INCORP not needed probably, since this use is transient
 }
 R j;
}    /* convert index list ind into standard form */

static A jtastd1(J jt,A a,A z,A ind){A*iv,q,r,s,s1,*sv,x;B b;I ar,*as,d,j,m,n,*rv,zr,*zs;P*zp;
 ar=AR(a); as=AS(a);
 zr=AR(z); zs=AS(z); zp=PAV(z);
 if(!ar)R a;
 n=AN(ind); iv=AAV(ind); 
 GATV0(r,INT,zr,1); rv= AV1(r);
 GATV0(s,BOX,zr,1); sv=AAV1(s);
 m=0; j=n;
 DO(n, x=C(iv[i]); b=x==ds(CACE); m+=rv[i]=b?1:AR(x); RZ(sv[i]=b?sc(zs[i]):shape(x));); 
 DQ(zr-n, rv[j]=1; RZ(sv[j]=sc(zs[j])); ++j;);
 d=m+zr-n; ASSERT(ar<=d,EVRANK);
 RZ(s1=raze(s));
 ASSERT(!ICMP(as,AV(s1)+d-ar,ar),EVLENGTH);
 if(ar<d)RZ(a=reshape(s1,a));
 RZ(q=dgrade1(eps(repeat(r,IX(zr)),SPA(zp,a))));
 R equ(q,IX(d))?a:cant2(q,a);
}    /* convert replacement array a into standard form relative to index list ind */

static A jtssel(J jt,A z,A ind){A a,*iv,p,q,x,y;B*b;I*av,c,i,j,m,n,*u,*v,*yv;P*zp;
 zp=PAV(z);
 y=SPA(zp,i); v=AS(y); m=v[0]; c=v[1]; yv=AV(y); 
 a=SPA(zp,a); n=AN(a); av=AV(a); 
 GATV0(p,B01,m,1); b=BAV1(p); mvc(m,b,1,MEMSET01);
 GATV0(q,INT,m,1); v=AV1(q); iv=AAV(ind);  
 for(i=0;i<n;++i){
  j=av[i]; if(j>=AN(ind))break;
  x=C(iv[j]);
  if(x!=ds(CACE)){
   u=yv+i; DO(m, v[i]=b[i]?*u:-1; u+=c;);
   RZ(p=eps(q,1<AR(x)?ravel(x):x)); b=BAV(p);
 }}
 R p;
}    /* which rows of the index matrix of z are selected by index list ind? */

static B jtipart(J jt,A z,A ind,A*i1,A*i2){A*iv,p,*pv,q,*qv,x;B*b;I c,d,n;P*zp;
 n=AN(ind); iv=AAV(ind);  zp=PAV(z);
 RZ(b=bfi(AR(z),SPA(zp,a),1));
 c=0; DO(n, if(b[i])++c;); d=n-c;
 GATV0(p,BOX,c,1); pv=AAV1(p); *i1=p;
 GATV0(q,BOX,d,1); qv=AAV1(q); *i2=q;
 DO(n, x=C(iv[i]); if(b[i])*pv++=x; else *qv++=x;);
 R 1;
}    /* partition index into sparse and dense parts */

static A jtdcube(J jt,A z,A i2){A*iv,x,y;I i,m,n,*s;P*zp;D rkblk[16];
 n=AN(i2); iv=AAV(i2); 
 zp=PAV(z); x=SPA(zp,x); s=1+AS(x);
 m=1; y=C(iv[n-1]); if(y==ds(CACE))RZ(y=IX(s[n-1]));

 for(i=n-2;0<=i;--i){
  m*=s[1+i]; x=C(iv[i]);
  A t; RZ(t=tymes(sc(m),x==ds(CACE)?IX(s[i]):x));
  RZ(y=ATOMIC2(jt,t,y,rkblk,0L, RMAX,CPLUS));
 }
 R y;
}    /* index cube relative to dense axes */

static A jtscuba(J jt,A z,A i1,B u){A*iv,q=0,x;I c,d,j,n,*s,*v;P*zp;
 n=AN(i1); 
 if(!n)R mtm;
 iv=AAV(i1);  s=AS(z); zp=PAV(z); x=SPA(zp,a); v=AV(x);
 for(j=n-1;0<=j;--j){
  x=C(iv[j]);
  if(x==ds(CACE))RZ(x=IX(s[v[j]]))else{if(1<AR(x))RZ(x=ravel(x)); if(u)RZ(x=nub(x));}
  c=AN(x); 
  if(q){d=AS(q)[0]; RZ(q=stitch(repeat(sc(d),x),reitem(sc(c*d),q)));}
  else RZ(q=reshape(v2(c,1L),x));
 }
 R q;
}    /* index cube relative to sparse axes; 1=u iff unique (remove duplicates) */

static A jtscubb(J jt,A z,A i1){A a,q,x,y;I c,d,h,j,*s,*v,*xv;P*zp;
 RZ(q=scuba(z,i1,1));
 if(!AS(q)[0])R mtm;
 s=AS(z); zp=PAV(z); y=SPA(zp,i); a=SPA(zp,a); v=AV(a);
 c=AS(q)[1]; d=AS(y)[1]; h=d-c;
 if(c==d)R less(q,y);
 RZ(q=less(q,taker(c,y)));
 GATV0(x,INT,h,1); xv=AV1(x); j=c; DO(h, xv[i]=s[v[j++]];);
 RZ(x=odom(2L,h,xv));
 c=AS(q)[0]; d=AS(x)[0];
 R stitch(repeat(sc(d),q),reitem(sc(c*d),x));
}    /* new rows for the index matrix of z for brand new cells */

static A jtscubc(J jt,A z,A i1,A p){A a,q,s,y,y1;B*qv;I c,d,h,j=-1,m,n,*sv,*u,*v;P*zp;
 zp=PAV(z); a=SPA(zp,a); n=AN(i1); h=AN(a)-n;
 if(!h)R mtm;
 GATV0(s,INT,h,1); sv=AV1(s); 
 d=1; u=AS(z); v=AV(a); DO(h, d*=sv[i]=u[v[n+i]];);
 RZ(y=repeat(p,SPA(zp,i))); m=AS(y)[0];
 RZ(y1=take(v2(m,n),y)); v=AV(y1);
 GATV0(q,B01,m,1); qv=BAV1(q);
 if(m){mvc(m,qv,MEMSET00LEN,MEMSET00); DO(m-1, if(ICMP(v,v+n,n)){if(d>i-j)qv[i]=1; j=i;} v+=n;); if(d>(m-1)-j)qv[m-1]=1;}
 RZ(y1=repeat(q,y1)); c=AS(y1)[0];
 if(!c)R mtm;
 R less(stitch(repeat(sc(d),y1),reitem(sc(c*d),odom(2L,h,sv))),y);
}    /* new rows for the index matrix of z for existing cells */

static A jtscube(J jt,A z,A i1,A p){A a,y;P*zp;
 zp=PAV(z); a=SPA(zp,a); y=SPA(zp,i);
 R !AN(a)&&!AS(y)[0]?take(num(1),mtm):over(scubb(z,i1),scubc(z,i1,p));
}    /* new rows for the index matrix of z */

static A jtiindx(J jt,A z,A i1){A q,j,j1,y;I c,d,e,h,i,*jv,m,n,*qv,*v,*yv;P*zp;
 c=AN(i1); zp=PAV(z); y=SPA(zp,i); 
 if(c==AS(y)[1])R indexof(y,scuba(z,i1,0));
 /* when y has excess columns, do progressive indexing */
 RZ(y=taker(c,y)); 
 RZ(j=indexof(y,scuba(z,i1,0)));  /* j: group indices           */
 n= AN(j); jv=AV(j);
 m=AS(y)[0]; yv=AV(y);
 GATV0(q,INT,n,1); qv=AV1(q);          /* q: # members in each group */
 for(i=h=0;i<n;++i){
  e=1; d=jv[i]; v=yv+c*d;
  DQ(m-d-1, if(ICMP(v,v+c,c))break; ++e; v+=c;);
  qv[i]=e; h+=e;
 }
 GATV0(j1,INT,h,1); v=AV1(j1);
 DO(n, e=qv[i]; d=jv[i]; DQ(e, *v++=d++;););
 R j1;
}    /* index of index list in the index matrix of z */

static A jtzpad1(J jt,A z,A t,B ip){A q,s,x,x0,y,y0;I m;P*zp;
 RZ(z&&t);
 if(m=AS(t)[0]){  /* new cells being added */
  zp=PAV(z);  
  y0=SPA(zp,i); RZ(y=over(y0,t)); RZ(q=grade1(y)); RZ(y=from(q,y));
  x0=SPA(zp,x); RZ(s=shape(x0)); *AV(s)=m; RZ(x=from(q,over(x0,reshape(s,SPA(zp,e)))));
  // if z is assigned to a name, the use counts need to be adjusted: the old ones need to be decremented
  // to remove the assignment, and the new ones need to be incremented to prevent them from being freed
  // until the name is freed.  We detect the case from jt->zombieval being set to the address of z
  // (if the block could not be inplaced, z will have been changed)
  if(jt->zombieval==z){RZ(ras(y)); RZ(ras(x)); fa(y0); fa(x0);}
  SPB(zp,i,y); SPB(zp,x,x);
 }
 R z;
}    /* pad z with new rows t for its index matrix */

static B mtind(J jt,A ind){A*iv,x;
 iv=AAV(ind); 
 DO(AN(ind), x=C(iv[i]); if(!AN(x))R 1;); 
 R 0;
}    /* 1 iff standardized index ind is an empty selection */

A jtam1e(J jt,A a,A z,A ind,B ip){A e,i1,i2,p,x;B*pv;C*u,*v;I*iv,k,m,n,r,*s,vk,xk;P*zp;
 RZ(a&&(ind=istd1(z,ind)));
 r=AR(z); zp=PAV(z); x=SPA(zp,x); e=SPA(zp,e);
 RZ(p=ssel(z,ind)); pv=BAV(p);
 RZ(ipart(z,ind,&i1,&i2));
 m=AN(p); n=AN(i2); u=CAV(e); v=CAV(x);
 r=AR(x); s=AS(x); k=bpnoun(AT(x)); xk=k*prod(r-(n+1),s+(n+1)); vk=k*prod(r-1,s+1);
 if(!n)DO(m, if(pv[i])mvc(xk,v,k,u); v+=vk;)
 else{
  RZ(i2=dcube(z,i2)); iv=AV(i2); n=AN(i2);
  DO(m, if(pv[i])DO(n, mvc(xk,v+iv[i]*xk,k,u);); v+=vk;);
 }
 R z;
}    /* a (<ind)}z; sparse z; ind is index list; sparse element a replacement */

A jtam1a(J jt,A a,A z,A ind,B ip){A a0=a,a1,e,i1,i2,t,x;C*u,*v,*xv;I ar,c,*iv,*jv,k,m,n,r,*s,uk,vk,xk;P*zp;
 RZ(a&&(ind=istd1(z,ind)));
 RZ(a=astd1(a,z,ind));
 if(mtind(jt,ind))R z;
 RZ(ipart(z,ind,&i1,&i2));
 RZ(z=zpad1(z,scube(z,i1,ssel(z,ind)),ip));
 zp=PAV(z); x=SPA(zp,x); e=SPA(zp,e);
 ar=AR(a); n=AN(i2); r=AR(x); s=AS(x);
 k=bpnoun(AT(x)); xk=k*prod(r-1-n,s+1+n); vk=k*prod(r-1,s+1); uk=!ar?k:n?xk:vk;
 u=CAV(a); xv=v=CAV(x);
 RZ(t=iindx(z,i1)); iv=AV(t); m=AN(t);
 if(!n&&!m){a1=SPA(zp,a); R ar?sparseit(a0,a1,e):sparseit(reshape(shape(z),a),a1,a);}
 if(n){RZ(t=dcube(z,i2)); jv=AV(t); c=AN(t); v=xv-vk;}
 if(!n)    DO(m,                           mvc(vk,v+vk*iv[i],uk,u); if(ar)u+=uk;  )
 else if(m)DO(m,      v=xv+vk*iv[i]; DO(c, mvc(xk,v+xk*jv[i],uk,u); if(ar)u+=uk;);)
 else      DO(AS(x)[0], v+=vk;         DO(c, mvc(xk,v+xk*jv[i],uk,u); if(ar)u+=uk;););
 R z;
}    /* a (<ind)}z; sparse z; ind is index list; arbitrary dense array a replacement */

A jtam1sp(J jt,A a,A z,A ind,B ip){R amnsp(a,z,ope(catalog(istd1(z,ind))),ip);}
     /* a (<ind)}z; sparse z; ind is index list; arbitrary sparse array a replacement */
