/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Box & Open                                                       */

#include "j.h"


I level(A w){A*wv;I d,j,wd;
 if(!(AN(w)&&AT(w)&BOX+SBOX))R 0;
 d=0; wv=AAV(w); wd=(I)w*ARELATIVE(w);
 DO(AN(w), j=level(WVR(i)); if(d<j)d=j;);
 R 1+d;
}

F1(jtlevel1){RZ(w); R sc(level(w));}

F1(jtbox0){R irs1(w,0L,0L,jtbox);}

F1(jtbox){A y,z,*zv;C*wv,*yv;I f,k,m,n,r,wr,*ws; 
 RZ(w);
 ASSERT(!(SPARSE&AT(w)),EVNONCE);
 ws=AS(w); wr=AR(w); r=jt->rank?jt->rank[1]:wr; f=wr-r; 
 RE(n=prod(f,ws)); if(n)m=AN(w)/n; else RE(m=prod(r,f+ws)); 
 k=m*bp(AT(w)); wv=CAV(w);
 GA(z,BOX,n,f,ws); zv=AAV(z);
 if(f){
  GA(y,AT(w),m,r,f+ws); yv=CAV(y);
  if(ARELATIVE(w)){A*v=(A*)wv;A1*u=(A1*)yv; DO(n, DO(m, u[i]=AABS(*v++,w);); RZ(zv[i]=ca(y)););}
  else DO(n, MC(yv,wv,k); wv+=k; RZ(zv[i]=ca(y)););
 }else RZ(*zv=rat(w));
 R z;
}    /* <"r w */

F1(jtboxopen){RZ(w); R AN(w)&&BOX&AT(w)?rat(w):box(w);}

F2(jtlink){RZ(a&&w); R over(box(a),AN(w)&&AT(w)&BOX?rat(w):box(w));}

static B povtake(A a,A w,C*x){B b;C*v;I d,i,j,k,m,n,p,q,r,*s,*ss,*u,*uu,y;
 RZ(w);
 r=AR(w); n=AN(w); k=bp(AT(w)); v=CAV(w);
 if(1>=r){MC(x,v,k*n); R 1;}
 m=AN(a); u=AV(a); s=AS(w);
 p=0; d=1; DO(r, if(u[m-1-i]==s[r-1-i]){d*=s[r-1-i]; ++p;}else break;);
 b=0; DO(r-p, if(b=1<s[i])break;);
 if(!b){MC(x,v,k*n); R 1;}
 k*=d; n/=d; ss=s+r-p; uu=u+m-p;
 for(i=0;i<n;++i){
  y=0; d=1; q=i; /* y=.a#.((-$a){.(($a)$1),$w)#:i */
  s=ss; u=uu; DO(r-p, j=*--s; y+=q%j*d; d*=*--u; q/=j;);
  MC(x+y*k,v,k); v+=k;
 }
 R 1;
}

static B jtopes1(J jt,B**zb,A*za,A*ze,I*zm,A cs,A w){A a,e=0,q,*wv,x;B*b;I i,k,m=0,n,*v,wcr;P*p;
 n=AN(w); wcr=AN(cs); wv=AAV(w);
 GA(x,B01,wcr,1,0); b=BAV(x); memset(b,C0,wcr);
 for(i=0;i<n;++i)
  if(q=wv[i],SPARSE&AT(q)){
   p=PAV(q); x=SPA(p,x); m+=*AS(x);
   if(!e)e=SPA(p,e); else ASSERT(equ(e,SPA(p,e)),EVSPARSE);
   k=wcr-AR(q); DO(k, b[i]=1;); a=SPA(p,a); v=AV(a); DO(AN(a), b[k+*v++]=1;);
  }
 RZ(*za=ifb(wcr,b));    /* union of sparse axes           */
 *zb=b;                 /* mask corresp. to sparse axes   */
 *ze=e?e:zero;          /* sparse element                 */
 *zm=m;                 /* estimate # of non-sparse cells */
 R 1;
}

static B jtopes2(J jt,A*zx,A*zy,B*b,A a,A e,A q,I wcr){A x;B*c;I dt,k,r,*s,t;P*p;
 dt=AT(e); r=AR(q); k=wcr-r; t=AT(q);
 if(t&SPARSE){
  p=PAV(q);
  RZ(c=bfi(r,SPA(p,a),1));
  DO(r, if(b[k+i]!=c[i]){RZ(q=reaxis(ifb(r,k+b),q)); break;});
 }else{
  if(k){
   GA(x,t,AN(q),wcr,0); s=AS(x); DO(k, *s++=1;); ICPY(s,AS(q),r); 
   MC(AV(x),AV(q),AN(q)*bp(t)); q=x;
  }
  RZ(q=sparseit(t&dt?q:cvt(dt,q),a,e));
 }
 p=PAV(q);
 x=SPA(p,x); if(!(dt&AT(x)))RZ(x=cvt(dt,x));
 *zx=x;         /* data cells              */
 *zy=SPA(p,i);  /* corresp. index matrix   */
 R 1;
}

static A jtopes(J jt,I zt,A cs,A w){A a,d,e,sh,t,*wv,x,x1,y,y1,z;B*b;C*xv;I an,*av,c,dk,dt,*dv,i,j,k,m,m1,n,
     p,*s,*v,wcr,wr,xc,xk,yc,*yv,*zs;P*zp;
 n=AN(w); wr=AR(w); wv=AAV(w); wcr=AN(cs); dt=DTYPE(zt); dk=bp(dt);
 RZ(opes1(&b,&a,&e,&m,cs,w)); an=AN(a); av=AV(a);
 GA(z,zt,1L,wr+wcr,0); zs=AS(z); ICPY(zs,AS(w),wr); ICPY(zs+wr,AV(cs),wcr);
 zp=PAV(z); c=wcr-an; yc=wr+an;
 SPB(zp,e,e=cvt(dt,e));
 GA(t,INT,yc, 1L,0L); v=AV(t); DO(wr, v[i]=i;); DO(an, v[wr+i]=wr+av[i];); SPB(zp,a,t);
 GA(sh,INT,1+c,1L,0L); s=AV(sh); s[0]=m; j=1; DO(wcr, if(!b[i])s[j++]=zs[wr+i];); 
 RE(xc=prod(c,1+s)); xk=xc*dk;
 GA(d,INT,wr,1,0); dv=AV(d); memset(dv,C0,wr*SZI);
 RE(i=mult(m,xc)); GA(x,dt, i,1+c,s); xv=CAV(x); mvc(m*xk,xv,dk,AV(e));
 RE(i=mult(m,yc)); GA(y,INT,i,2L, 0L); v=AS(y); *v=m; v[1]=yc; yv=AV(y); memset(yv,C0,SZI*i);
 for(i=p=0;i<n;++i){
  RZ(opes2(&x1,&y1,b,a,e,wv[i],wcr)); v=AS(y1); m1=v[0]; k=v[1];
  if(m<p+m1){
   j=m; m=(i<n-1?m+m:0)+p+m1;
   RZ(x=take(sc(m),x)); xv=CAV(x)+p*xk; mvc(xk*(m-j),xv,dk,AV(e));
   RZ(y=take(sc(m),y)); yv= AV(y)+p*yc;
  }
  for(j=wr-1;j;--j)if(dv[j]==zs[j]){dv[j]=0; ++dv[j-1];}else break;
  v=AV(y1); DO(m1, ICPY(yv,dv,wr); ICPY(yv+yc-k,v,k); yv+=yc; v+=k;); 
  if(memcmp(1+AS(x1),1+s,SZI*c)){*s=m1; povtake(sh,x1,xv);} else MC(xv,AV(x1),m1*xk);
  ++dv[wr-1]; xv+=m1*xk; p+=m1;
 }
 SPB(zp,x,p==m?x:take(sc(p),x));
 SPB(zp,i,p==m?y:take(sc(p),y));
 R z;
}

F1(jtope){PROLOG;A cs,*v,y,z;B b,c,h=1;C*x;I d,i,k,m,n,*p,q=RMAX,r=0,*s,t=0,*u,zn;
 RZ(w);
 n=AN(w); v=AAV(w); b=ARELATIVE(w);
 if(!(n&&BOX&AT(w)))R ca(w); /* {GA(z,B01,0L,1+AR(w),AS(w)); *(AR(w)+AS(w))=0; R z;} */
 if(!AR(w))R b?(A)AABS(*v,w):*v;
 for(i=0;i<n;++i){
  y=b?(A)AABS(v[i],w):v[i]; 
  q=MIN(q,AR(y)); 
  r=MAX(r,AR(y)); 
  if(AN(y)){
   k=AT(y); t=t?t:k; m=t|k;
   if(t!=k){h=0; ASSERT(HOMO(t,k)&&!(m&SPARSE&&m&XNUM+RAT),EVDOMAIN); t=maxtype(t,k);}
 }}
 if(!t)DO(n, y=b?(A)AABS(v[i],w):v[i]; k=AT(y); RE(t=maxtype(t,k)););
 GA(cs,INT,r,1,0); u=AV(cs); DO(r-q, u[i]=1;); p=u+r-q; DO(q, p[i]=0;);
 DO(n, y=b?(A)AABS(v[i],w):v[i]; s=AS(y); p=u+r-AR(y); DO(AR(y),p[i]=MAX(p[i],s[i]);););
 if(t&SPARSE)RZ(z=opes(t,cs,w))
 else{
  RE(m=prod(r,u)); RE(zn=mult(n,m)); k=bp(t); q=m*k; 
  GA(z,t,zn,r+AR(w),AS(w)); ICPY(AS(z)+AR(w),u,r); x=CAV(z);
  c=b&&t&BOX;
  if(c){AFLAG(z)=AFREL; p=AV(z); d=AREL(mtv,z); DO(zn, *p++=d;);} else fillv(t,zn,x);
  for(i=0;i<n;++i){
   y=b?(A)AABS(v[i],w):v[i];
   if(ARELATIVE(y))RZ(y=relocate((I)y-c*(I)z,ca(y)));
   if(h&&1>=r)                MC(x,AV(y),k*AN(y));
   else if(t==AT(y)&&m==AN(y))MC(x,AV(y),q); 
   else if(AN(y))             RZ(povtake(cs,t==AT(y)?y:cvt(t,y),x)); 
   x+=q;
 }}
 EPILOG(z);
}

static A jtrazeg(J jt,A w,I t,I n,I r,A*v,B zb){A h,h1,x,y,*yv,z,*zv;B b;C*zu;I c=0,d,i,j,k,m,p,q,*s,*v1,yr,*ys;
 k=bp(t); d=SZI*(r-1); b=ARELATIVE(w);
 GA(h,INT,r,1,0); s=AV(h); memset(s,C0,r*SZI);
 for(i=0;i<n;++i){
  y=b?(A)AABS(v[i],w):v[i]; yr=AR(y); ys=AS(y); c+=r==yr?*ys:1; ASSERT(0<=c,EVLIMIT); j=r-yr; 
  if(!yr)continue;
  DO(j,  s[i]=MAX(1,    s[i]);     ); 
  DO(yr, s[j]=MAX(ys[i],s[j]); ++j;);
 }
 *s=c; m=prod(r,s); p=c?k*m/c:0; 
 GA(h1,INT,r,1,0); v1=AV(h1);
 GA(z,t,m,r,s); if(zb)AFLAG(z)=AFREL; 
 zu=CAV(z); zv=AAV(z);
 for(i=0;i<n;++i){
  y=b?(A)AABS(v[i],w):v[i];
  if(t!=AT(y))RZ(y=cvt(t,y)); 
  yr=AR(y); ys=AS(y);
  if(!yr){
   if(t&BOX){x=(A)(*AV(y)+ARELATIVE(y)*(I)y-zb*(I)z); DO(p/SZA, *zv++=x;);}
   else     {mvc(p,zu,k,AV(y)); zu+=p;} 
   continue;
  }
  if(j=r-yr){DO(j,v1[i]=1;); ICPY(j+v1,ys,yr); RZ(y=reshape(h1,y));}
  if(memcmp(1+s,1+AS(y),d)){*s=IC(y); RZ(y=take(h,y));}
  if(t&BOX){yv=AAV(y); q=ARELATIVE(y)*(I)y-zb*(I)z; DO(AN(y), *zv++=(A)((I)yv[i]+q););}
  else     {j=k*AN(y); MC(zu,AV(y),j); zu+=j;}
 }
 R z;
}    /* raze general case */

F1(jtraze){A*v,y,*yv,z,*zv;B b,zb;C*zu;I d,i,k,m=0,n,q,r=1,*s=0,t=0,yt;
 RZ(w);
 n=AN(w); v=AAV(w); b=ARELATIVE(w); zb=b;
 if(!n)R mtv;
 if(!(BOX&AT(w)))R ravel(w);
 if(1==n){RZ(z=b?(A)AABS(*v,w):*v); R AR(z)?z:ravel(z);}
 for(i=0;i<n;++i){
  y=b?(A)AABS(v[i],w):v[i]; m+=d=AN(y); r=MAX(r,AR(y)); 
  if(d){
   yt=AT(y); 
   if(t){ASSERT(HOMO(t,yt),EVDOMAIN); t=maxtype(t,yt);}else t=yt;
   zb=zb||ARELATIVE(y);
 }}
 if(!t)DO(n, y=b?(A)AABS(v[i],w):v[i]; t=MAX(t,AT(y)););
 k=bp(t); 
 if(1<r)R razeg(w,t,n,r,v,zb);
 GA(z,t,m,r,0); if(zb&&!(t&DIRECT))AFLAG(z)=AFREL;
 zu=CAV(z); zv=AAV(z);
 for(i=0;i<n;++i){
  y=b?(A)AABS(v[i],w):v[i]; 
  if(AN(y)){
   if(t&BOX){yv=AAV(y); q=ARELATIVE(y)*(I)y-zb*(I)z; DO(AN(y), *zv++=(A)((I)yv[i]+q););}
   else     {if(t!=AT(y))RZ(y=cvt(t,y)); d=k*AN(y); MC(zu,AV(y),d); zu+=d;}
 }}
 R z;
}

F1(jtrazeh){A*wv,y,z;C*xv,*yv,*zv;I c=0,ck,dk,i,k,n,p,r,*s,t,wd;
 RZ(w);
 ASSERT(BOX&AT(w),EVDOMAIN);
 if(!AR(w))R ope(w);
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w); y=WVR(0); p=IC(y); t=AT(y); k=bp(t);
 DO(n, y=WVR(i); r=AR(y); ASSERT(p==IC(y),EVLENGTH); ASSERT(r&&r<=2&&t==AT(y),EVNONCE); c+=1==r?1:*(1+AS(y)););
 GA(z,t,p*c,2,0); s=AS(z); *s=p; *(1+s)=c; 
 zv=CAV(z); ck=c*k;
 for(i=0;i<n;++i){
  y=WVR(i); dk=1==AR(y)?k:k**(1+AS(y)); xv=zv; zv+=dk;
  if(!dk)continue;
  if(wd&&t&BOX)RZ(y=car(y));
  yv=CAV(y);
  switch(0==(I)xv%dk&&0==ck%dk?dk:0){
   case sizeof(I): {I*u,*v=(I*)yv; DO(p, u=(I*)xv; *u=*v++;    xv+=ck;);} break;
   case sizeof(S): {S*u,*v=(S*)yv; DO(p, u=(S*)xv; *u=*v++;    xv+=ck;);} break;
   case sizeof(C):                 DO(p, *xv=*yv++;            xv+=ck;);  break;
   default:                        DO(p, MC(xv,yv,dk); yv+=dk; xv+=ck;); 
 }}
 R z;
}    /* >,.&.>/,w */


#define EXTZ    if(vv<=d+v){m=v-CAV(z); RZ(z=ext(0,z)); v=m+CAV(z); vv=CAV(z)+k*AN(z);}

F2(jtrazefrom){A*wv,y,z;B b;C*v,*vv;I an,c,d,i,j,k,m,n,r,*s,t,*u,wn;
 RZ(a&&w);
 an=AN(a); wn=AN(w);
 if(b=NUMERIC&AT(a)&&1==AR(a)&&BOX&AT(w)&&!ARELATIVE(w)&&1==AR(w)&&1<wn&&an>10*wn){
  wv=AAV(w); y=*wv; r=AR(y); s=1+AS(y); n=B01&AT(a)?2:wn;
  for(i=m=t=0;b&&i<n;++i){
   y=wv[i]; b=r==AR(y)&&!(1<r&&ICMP(s,1+AS(y),r-1));
   if(AN(y)){m+=AN(y); if(t)b=b&&t==AT(y); else t=AT(y);}
 }}
 if(!(b&&t&DIRECT))R raze(from(a,w));
 c=aii(y); k=bp(t); 
 RZ(z=exta(t,r,c,(I)((1.2*an*m)/(n*c)))); u=AS(z); *u++=AN(z)/c; DO(r-1, *u++=*s++;);
 v=CAV(z); vv=v+k*AN(z);
 if(B01&AT(a)){B*av=BAV(a);        
  for(i=0;i<an;++i){
   y=wv[*av++]; d=k*AN(y); EXTZ; MC(v,AV(y),d); v+=d;
 }}else{I*av; 
  RZ(a=vi(a)); av=AV(a); 
  for(i=0;i<an;++i){
   j=*av++; if(0>j){j+=wn; ASSERT(0<=j,EVINDEX);}else ASSERT(j<wn,EVINDEX);
   y=wv[j];     d=k*AN(y); EXTZ; MC(v,AV(y),d); v+=d;
 }}
 AN(z)=(v-CAV(z))/k; *AS(z)=AN(z)/c;     
 R z;
}    /* a ;@:{ w */
