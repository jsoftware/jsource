/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: a ind}z where z is sparse and ind is <"1 integers              */

/*  amne    a is the sparse element                                        */
/*  amna    a is an arbitrary dense array                                  */
/*  amnsp   a an arbitrary sparse array                                    */

#include "j.h"


static I jtcsize(J jt,A z,A ind){B*b;I h,j,m,r,*s;P*zp;
 r=AR(z); s=AS(z); zp=PAV(z);
 RZ(b=bfi(r,SPA(zp,a),0));
 m=1; j=h=*(AR(ind)+AS(ind)-1); DQ(r-h, if(b[j])m*=s[j]; ++j;);
 R m;
}    /* data cell size in z for each element of index array ind */

static B jtiaddr(J jt,A z,A ind,A*i1,A*i2){A a,ai,as,ii,jj,q,t,x,y;I c,d,e,h,i,*iv,*jv,m,n,p,*qv,*s,*u,*v,*yv;P*zp;
 zp=PAV(z); a=SPA(zp,a); x=SPA(zp,x); s=1+AS(x); 
 h=*(AS(ind)+AR(ind)-1);                               /* # axes indexed              */
 RZ(q=gt(sc(h),a)); 
 y=SPA(zp,i); if(!all1(q))RZ(y=repeatr(q,y));          /* indexed cols of index mat   */
 m=*AS(y); yv=AV(y);      
 RZ(ai=IX(h));
 RZ(as=less(IX(AR(z)),a)); u=AV(as); n=AN(as);         /* dense axes                  */
 GATV0(t,INT,n,1); v=AV(t);                             /* shape of indexed dense axes */
 e=0; d=1; DO(n, if(h>u[i])v[e++]=s[i]; else d*=s[i];);
 RZ(*i2=jj=tymes(sc(d),base2(vec(INT,e,v),repeatr(eps(ai,as),ind))));
 c=*(1+AS(y));
 if(!c){
  n=AN(jj);
  RZ(*i1=repeat(sc(n),IX(m)));
  RZ(*i2=reshape(sc(m*n),jj));
  R 1;
 }
 RZ(*i1=ii=indexof(y,repeatr(eps(ai,a),ind)));         /* group indices in index mat  */
 if(c==AN(a))R 1;
 n=AN(ii); iv=AV(ii); jv=AV(jj);                       /* do progressive iota         */
 GATV0(q,INT,n,1); qv=AV(q);                            /* group sizes                 */
 for(i=h=0;i<n;++i){
  e=1; d=iv[i]; v=yv+c*d;
  DQ(m-d-1, if(ICMP(v,v+c,c))break; ++e; v+=c;);
  qv[i]=e; h+=e;                                       /* # elements in group i       */
 }
 GATV0(t,INT,h,1); u=AV(t); *i1=t;
 GATV0(t,INT,h,1); v=AV(t); *i2=t;
 DO(n, e=qv[i]; d=iv[i]; p=jv[i]; DQ(e, *u++=d++; *v++=p;););
 R 1;
}    /* index i1 (in index matrix) and address i2 (in data array) from index array */

static A jtzpadn(J jt,A z,A ind,B ip){A a,ai,i1,p,p1,q,t,x,x0,y,y0,y1;B*b;I c,d,h,m,n;P*zp;
 // put sparse axi into names a,x,y; remember x&y as x0,y0
 zp=PAV(z); a=SPA(zp,a); x=x0=SPA(zp,x); y=y0=SPA(zp,i);
 n=1; h=*(AS(ind)+AR(ind)-1);
 RZ(ai=IX(h));
 RZ(t=eps(ai,a)); b=BAV(t); d=0; DO(h, if(b[i])++d;);
 RZ(i1=d<h?repeatr(t,ind):ind); if(2!=AR(ind))RZ(i1=d?reshape(v2(AN(i1)/d,d),i1):mtm);
 RZ(t=gt(sc(h),a)); RZ(y1=all1(t)?y:repeatr(t,y));
 RZ(p=nub(less(i1,y1)));
 if(c=AN(a)-d){
  RZ(t=from(less(a,ai),shape(z))); RZ(p1=odom(2L,c,AV(t))); n=*AS(p1);
  if(m=*AS(p))RZ(p=stitch(repeat(sc(n),p),reshape(v2(n*m,c),p1)));
  RZ(t=nub(repeat(eps(y1,i1),y1)));
  RZ(t=stitch(repeat(sc(n),t),reshape(v2(n**AS(t),c),p1)));
  RZ(t=less(t,y));
  if(AN(t))RZ(p=over(p,t));
 }
 if(m=*AS(p)){  /* new cells being added */
  RZ(y=over(y,p)); RZ(q=grade1(y)); RZ(y=from(q,y));
  RZ(t=shape(x)); *AV(t)=m; RZ(x=from(q,over(x,reshape(t,SPA(zp,e)))));
  // if z is assigned to a name, the use counts need to be adjusted: the old ones need to be decremented
  // to remove the assignment, and the new ones need to be incremented to prevent them from being freed
  // until the name is freed.  We detect the case from jt->assignsym being set to the address of z
  // (if the block could not be inplaced, z will have been changed)
  if(jt->assignsym&&jt->assignsym->val==z){RZ(ras(y)); RZ(ras(x)); fa(y0); fa(x0);}
  SPB(zp,i,y); SPB(zp,x,x);
 }
 R z;
}    /* create new indexed cells in z as necessary */

static A jtastdn(J jt,A a,A z,A ind){A a1,q,r,s;B*b;I ar,*as,*av,d,ir,n,n1,*v,zr,*zs;P*zp;
 ar=AR(a); as=AS(a);
 zr=AR(z); zs=AS(z); 
 if(!ar)R a;
 ir=AR(ind); n=*(AS(ind)+ir-1); d=(ir-1)+(zr-n); ASSERT(ar<=d,EVRANK);  // n=shape of item of i; d is # unindexed axes
 GATV0(s,INT,d,1); v=AV(s); MCISH(v,AS(ind),ir-1); MCISH(v+ir-1,zs+n,zr-n);
 ASSERTAGREE(as,AV(s)+d-ar,ar);
 if(ar<d)RZ(a=reshape(s,a));
 zp=PAV(z); a1=SPA(zp,a); av=AV(a1); n1=n-1;
 GATV0(s,B01,zr,1); b=BAV(s); 
 memset(b,C0,zr); DO(AN(a1), b[av[i]]=1;); memset(b,!memchr(b,C1,n)?C0:C1,n);
 GATV0(r,INT,zr-n1,1); v=AV(r); *v++=ar-(zr-n); DQ(zr-n, *v++=1;);
 RZ(q=dgrade1(repeat(r,vec(B01,zr-n1,b+n1))));
 R equ(q,IX(ar))?a:cant2(q,a);
}    /* convert replacement array a into standard form relative to index array ind */

A jtamne(J jt,A a,A z,A ind,B ip){A i1,i2,x,y;C*u,*v;I*iv,*jv,k,m,n,vk,xk;P*zp;
 RZ(a&&z&&ind);
 RZ(iaddr(z,ind,&i1,&i2));
 zp=PAV(z); x=SPA(zp,x); y=SPA(zp,i);
 m=*AS(y); n=AN(i1);
 k=bpnoun(AT(x)); xk=k*aii(x); vk=k*csize(z,ind);
 u=CAV(a); v=CAV(x); iv=AV(i1); jv=AV(i2);
 DO(n, if(m>iv[i])mvc(vk,v+xk*iv[i]+k*jv[i],k,u););
 R z;
}    /* a (<"1 ind)}z, sparse z, integer array ind, sparse element a replacement */

A jtamna(J jt,A a,A z,A ind,B ip){A i1,i2,x;C*u,*v;I*iv,*jv,k,n,vk,xk;P*zp;
 RZ(a&&z&&ind);
 RZ(z=zpadn(z,ind,ip));
 RZ(a=astdn(a,z,ind));
 RZ(iaddr(z,ind,&i1,&i2));
 zp=PAV(z); x=SPA(zp,x); n=AN(i1);
 k=bpnoun(AT(x)); xk=k*aii(x); vk=k*csize(z,ind);
 u=CAV(a); v=CAV(x); iv=AV(i1); jv=AV(i2); 
 if(AR(a))DO(n, mvc(vk,v+xk*iv[i]+k*jv[i],vk,u); u+=vk;)
 else     DO(n, mvc(vk,v+xk*iv[i]+k*jv[i],k,u););
 R z;
}    /* a (<"1 ind)}z, dense a,integer array ind, sparse z */

A jtamnsp(J jt,A a,A z,A ind,B ip){A i1,i2,t;C*ev,*u,*v,*vv;I c,*dv,i,*iv,j,*jv,k,m,n,p,q,r,*s,*yv,zk;P*ap,*zp;
 RZ(a&&z&&ind);
 r=AR(a); ap=PAV(a); t=SPA(ap,a); if(r>AN(t))RZ(a=reaxis(IX(r),a));
 RZ(a=astdn(a,z,ind));  ap=PAV(a);
 RZ(z=zpadn(z,ind,ip)); zp=PAV(z);
 RZ(iaddr(z,ind,&i1,&i2));
 s=AS(a); n=AN(i1); c=csize(z,ind); iv=AV(i1); jv=AV(i2);
 t=SPA(ap,i); yv= AV(t); m=*AS(t); p=0;
 t=SPA(ap,e); ev=CAV(t);
 t=SPA(ap,x); u =CAV(t);
 t=SPA(zp,x); v =CAV(t); k=bpnoun(AT(t)); zk=k*aii(t);
 GATV0(t,INT,r,1); dv=AV(t); memset(dv,C0,SZI*r); dv[r-1]=-1;
 for(i=0;i<n;++i){
  vv=v+zk*iv[i]+k*jv[i];
  for(j=0;j<c;++j){
   q=r; DQ(r, --q; ++dv[q]; if(dv[q]<s[q])break; dv[q]=0;);
   q=1; while(p<m){DO(r, if(q=yv[i]-dv[i])break;); if(0<=q)break; ++p; yv+=r;}
   MC(vv,q?ev:u+k*p,k); 
   vv+=k;
 }}
 R z;
}    /* a (<"1 ind)}z, sparse a, integer array ind, sparse z */
