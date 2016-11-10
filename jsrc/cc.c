/* Copyright 1990-2010, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Cuts                                                      */

#include "j.h"


static DF1(jtcut01){DECLF;A h,x;
 RZ(x=from(box(every(negate(shape(w)),0L,jtiota)),w));
 if(VGERL&sv->flag){h=sv->h; R df1(x,*AAV(h));}else R CALL1(f1,x,fs);
}    /* f;.0 w */

static F2(jtcut02v){A z;I*av,e,j,k,m,t,wk;
 m=AN(w); t=AT(w); wk=bp(t);
 av=AV(a); j=av[0]; e=av[1]; k=ABS(e);
 ASSERT(!e||-m<=j&&j<m,EVINDEX);
 if(0>j){j+=1+m-k; if(0>j){k+=j; j=0;}}else k=MIN(k,m-j);
 GA(z,t,k,1,0);
 MC(AV(z),CAV(w)+wk*j,wk*k); 
 R 0>e?reverse(z):z;
}    /* a ];.0 vector */

static F2(jtcut02m){A z;C*u,*v;I*av,c,d,e0,e1,j0,j1,k0,k1,m0,m1,*s,t,wk;
 s=AS(w); m0=s[0]; m1=s[1]; t=AT(w); wk=bp(t);
 av=AV(a);    
 if(4==AN(a)){j0=av[0]; e0=av[2]; k0=ABS(e0); j1=av[1]; e1=av[3]; k1=ABS(e1);}
 else        {j0=av[0]; e0=av[1]; k0=ABS(e0); j1=0;     e1=m1;    k1=e1;     }
 ASSERT(!e0||-m0<=j0&&j0<m0,EVINDEX);
 ASSERT(!e1||-m1<=j1&&j1<m1,EVINDEX);
 if(0>j0){j0+=1+m0-k0; if(0>j0){k0+=j0; j0=0;}}else k0=MIN(k0,m0-j0);
 if(0>j1){j1+=1+m1-k1; if(0>j1){k1+=j1; j1=0;}}else k1=MIN(k1,m1-j1);
 GA(z,t,k0*k1,2,0); s=AS(z); s[0]=k0; s[1]=k1;
 u=CAV(z); c=wk*k1;
 if(0>e0){d=-wk*m1; v=CAV(w)+wk*(j0*m1+j1+m1*(k0-1));}
 else    {d= wk*m1; v=CAV(w)+wk*(j0*m1+j1          );}
 DO(k0, MC(u,v,c); u+=c; v+=d;);
 R 0>e1?irs1(z,0L,1L,jtreverse):z;
}    /* a ];.0 matrix */

static DF2(jtcut02){DECLF;A h=0,*hv,q,qq,*qv,y,z,*zv;C id;I*as,c,d,e,hn,i,ii,j,k,m,n,*u,*ws;
 RZ(a&&w);
 if(VGERL&sv->flag){h=sv->h; hv=AAV(h); hn=AN(h);}
 id=h?0:ID(fs); d=h?0:id==CBOX?1:2; 
 if(1>=AR(a))RZ(a=lamin2(zero,a));
 RZ(a=vib(a));
 if(2==AR(a)&&(id==CLEFT||id==CRIGHT)&&AT(w)&DIRECT)
  if     (2==AN(a)&&1==AR(w))R cut02v(a,w);
  else if(4>=AN(a)&&2==AR(w))R cut02m(a,w);
 as=AS(a); m=AR(a)-2; PROD(n,m,as); c=as[1+m]; u=AV(a);
 ASSERT(2==as[m]&&c<=AR(w),EVLENGTH);
 if(!n){  /* empty result; figure out result type */
  switch(d){
   case 0: y=df1(w,*hv); RESETERR; break;
   case 1: y=ace; break;
   case 2: y=CALL1(f1,w,fs); RESETERR; break;
  }
  GA(z,y?AT(y):B01,n,m,as); R z;
 }
 ws=AS(w); 
 GATV(z,BOX,n,m,as); zv=AAV(z);
 GATV(q,BOX,c,1,0); qv=AAV(q);
 GAT(qq,BOX,1,0,0); *AAV(qq)=q;
 for(ii=0;ii<n;++ii){
  for(i=0;i<c;++i){
   m=ws[i]; j=u[i]; e=u[i+c]; k=ABS(e); 
   ASSERT(!e||-m<=j&&j<m,EVINDEX);
   if(0>j){j+=1+m-k; if(0>j){k+=j; j=0;}}else k=MIN(k,m-j);
   RZ(qv[i]=0>e?apv(k,j+k-1,-1L):0==j&&k==m?ace:apv(k,j,1L));
  }
  RZ(y=from(qq,w)); u+=c+c;
  switch(d){
   case 0: RZ(*zv++=df1(y,hv[ii%hn])); break;
   case 1: RZ(*zv++=y);                break;
   case 2: RZ(*zv++=CALL1(f1,y,fs));   break;
 }}
 R 1==d?z:ope(z);
}    /* a f;.0 w */

DF2(jtrazecut0){A z;C*v,*wv,*zu,*zv;I ar,*as,*av,c,d,i,j,k,m,n,q,wt,zn;
 RZ(a&&w);
 n=AN(w); wt=AT(w); wv=CAV(w); 
 ar=AR(a); as=AS(a); m=2==ar?1:*as;
 if(!((2==ar||3==ar)&&wt&IS1BYTE&&1==AR(w)))R raze(df2(a,w,cut(ds(CBOX),zero)));
 ASSERT(2==as[ar-2]&&1==as[ar-1],EVLENGTH);
 RZ(a=vib(a)); av=AV(a);
 RZ(z=exta(wt,1L,1L,n/2)); zn=AN(z); zv=CAV(z); zu=zn+zv;
 for(i=0;i<m;++i){
  j=*av++; k=*av++; 
  ASSERT(-n<=j&&j<n,EVINDEX);
  q=0<=k?k:k==IMIN?IMAX:-k; d=0<=j?MIN(q,n-j):MIN(q,n+1+j);
  while(zu<d+zv){c=zv-CAV(z); RZ(z=ext(0,z)); zn=AN(z); v=CAV(z); zv=c+v; zu=zn+v;}
  switch((0<=j?2:0)+(0<=k)){
   case 0: v=wv+j+n+1;   DO(d, *zv++=*--v;); break;
   case 1: v=wv+j+n+1-d; DO(d, *zv++=*v++;); break;
   case 2: v=wv+j+d;     DO(d, *zv++=*--v;); break;
   case 3: v=wv+j;       DO(d, *zv++=*v++;);
 }}
 AN(z)=*AS(z)=zv-CAV(z);
 R z;
}    /* a ;@:(<;.0) vector */


static DF2(jtcut2bx){A*av,b,t,x,*xv,y,*yv;B*bv;I an,ad,bn,i,j,m,p,q,*u,*v,*ws;V*sv;
 RZ(a&&w&&self);
 sv=VAV(self); q=*AV(sv->g);
 an=AN(a); av=AAV(a); ad=(I)a*ARELATIVE(a); ws=AS(w);
 ASSERT(an<=AR(w),EVLENGTH);
 GATV(x,BOX,an,1,0); xv=AAV(x);
 GATV(y,BOX,an,1,0); yv=AAV(y);
 for(i=0;i<an;++i){
  b=AVR(i); bn=AN(b); m=ws[i];
  ASSERT(1>=AR(b),EVRANK);
  if(!bn&&m){xv[i]=zero; RZ(yv[i]=sc(m));}
  else{
   if(!(B01&AT(b)))RZ(b=cvt(B01,b));
   if(!AR(b)){if(*BAV(b)){RZ(xv[i]=IX(m)); RZ(yv[i]=reshape(sc(m),0<q?one:zero));}else xv[i]=yv[i]=mtv; continue;}
   ASSERT(bn==m,EVLENGTH);
   bv=BAV(b); p=0; DO(bn, p+=bv[i];); 
   GATV(t,INT,p,1,0); u=AV(t); xv[i]=t;
   GATV(t,INT,p,1,0); v=AV(t); yv[i]=t; j=-1;
   if(p)switch(q){
    case  1: DO(bn, if(bv[i]){*u++=i  ; if(0<=j)*v++=i-j  ; j=i;}); *v=bn-j;   break;
    case -1: DO(bn, if(bv[i]){*u++=i+1; if(0<=j)*v++=i-j-1; j=i;}); *v=bn-j-1; break;
    case  2: DO(bn, if(bv[i]){*u++=j+1;         *v++=i-j  ; j=i;}); break;
    case -2: DO(bn, if(bv[i]){*u++=j+1;         *v++=i-j-1; j=i;}); break;
 }}}
 RZ(x=ope(catalog(x)));
 RZ(y=ope(catalog(y)));
 RZ(t=AN(x)?irs2(x,y,0L,1L,1L,jtlamin2):iota(over(shape(x),v2(2L,0L))));
 R cut02(t,w,self);
}    /* a f;.n w for boxed a, with special code for matrix w */


#define MCREL(uu,vv,n)   {A*u=(A*)(uu);A*v=(A*)(vv); DO((n), u[i]=AADR(wd,v[i]););}

#define CUTSWITCH(EACHC)  \
 switch(wd?0:id){A z,*za;C id1,*v1,*zc;I d,i,j,ke,q,*zi,*zs;                 \
  case CPOUND:                                                               \
   GATV(z,INT,m,1,0); zi=AV(z); EACHC(*zi++=d;); R z;                          \
  case CDOLLAR:                                                              \
   GATV(z,INT,m,1,0); zi=AV(z); EACHC(*zi++=d;);                               \
   R irs2(z,vec(INT,MAX(0,r-1),1+s),0L,0L,1L,jtover);                        \
  case CHEAD:                                                                \
   GA(z,t,m*c,r,s); zc=CAV(z); *AS(z)=m;                                     \
   EACHC(ASSERT(d,EVINDEX); MC(zc,v1,k); zc+=k;);                            \
   R z;                                                                      \
  case CTAIL:                                                                \
   GA(z,t,m*c,r,s); zc=CAV(z); *AS(z)=m;                                     \
   EACHC(ASSERT(d,EVINDEX); MC(zc,v1+k*(d-1),k); zc+=k;);                    \
   R z;                                                                      \
  case CCOMMA:                                                               \
  case CLEFT:                                                                \
  case CRIGHT:                                                               \
   e-=e&&neg; RE(d=mult(m*c,e));                                             \
   GA(z,t,d,id==CCOMMA?2:1+r,s-1); zc=CAV(z); fillv(t,d,zc);                 \
   zs=AS(z); zs[0]=m; zs[1]=id==CCOMMA?e*c:e; ke=k*e;                        \
   EACHC(MC(zc,v1,d*k);  zc+=ke;);                                           \
   R z;                                                                      \
  case CBOX:                                                                 \
   GA(z,m?BOX:B01,m,1,0); za=AAV(z);                                         \
   EACHC(GA(y,t,d*c,r,s); *AS(y)=d; MC(AV(y),v1,d*k); *za++=y;);             \
   R z;                                                                      \
  case CAT: case CATCO: case CAMP: case CAMPCO:                              \
   if(CBOX==ID(vf->f)&&(id1=ID(vf->g),id1==CBEHEAD||id1==CCTAIL)){           \
    GA(z,m?BOX:B01,m,1,0); za=AAV(z);                                        \
    EACHC(d=d?d-1:0; GA(y,t,d*c,r,s); *AS(y)=d; MC(AV(y),id1==CBEHEAD?v1+k:v1,d*k); *za++=y;);               \
    R z;                                                                     \
   }                                                                         \
   /* note: fall through */                                                  \
  default:                                                                   \
   if(!m){y=reitem(zero,w); R iota(over(zero,shape(h?df1(y,*hv):CALL1(f1,y,fs))));}                            \
   GATV(z,BOX,m,1,0); za=AAV(z); j=0;                                          \
   switch((wd?2:0)+(h?1:0)){                                                 \
    case 0: EACHC(GA(y,t,d*c,r,s); *AS(y)=d; MC(AV(y),v1,d*k);  RZ(*za++=CALL1(f1,y,fs));          ); break; \
    case 1: EACHC(GA(y,t,d*c,r,s); *AS(y)=d; MC(AV(y),v1,d*k);  RZ(*za++=df1(y,hv[j])); j=(1+j)%hn;); break; \
    case 2: EACHC(GA(y,t,d*c,r,s); *AS(y)=d; MCREL(AV(y),v1,d); RZ(*za++=CALL1(f1,y,fs));          ); break; \
    case 3: EACHC(GA(y,t,d*c,r,s); *AS(y)=d; MCREL(AV(y),v1,d); RZ(*za++=df1(y,hv[j])); j=(1+j)%hn;); break; \
   }                                                                         \
   z=ope(z);                                                                 \
   EPILOG(z);                                                                \
 }

#define EACHCUTSP(stmt)  \
 if(pfx)for(i=m;i>=1;--i){q=yu[i-1]-yu[i  ]; d=q-neg; v1=wv+k*(b+p); stmt; p+=q;}  \
 else   for(i=1;i<=m;++i){q=yu[i  ]-yu[i-1]; d=q-neg; v1=wv+k*(b+p); stmt; p+=q;}

static F1(jtcps){A z;P*wp,*zp;
 GA(z,AT(w),1,AR(w),AS(w)); 
 zp=PAV(z);
 wp=PAV(w); 
 SPB(zp,a,SPA(wp,a)); 
 SPB(zp,e,SPA(wp,e));
 SPB(zp,i,SPA(wp,i));
 R z;
}

static A jtselx(J jt,A x,I r,I i){A z;I c,k;
 c=aii(x); k=c*bp(AT(x));
 GA(z,AT(x),r*c,AR(x),AS(x)); *AS(z)=r;
 MC(CAV(z),CAV(x)+i*k,r*k);
 R z;
}    /* (i+i.r){x */

static A jtsely(J jt,A y,I r,I i,I j){A z;I c,*s,*v;
 c=*(1+AS(y));
 GATV(z,INT,r*c,2,0); s=AS(z); s[0]=r; s[1]=c;
 v=AV(z);
 ICPY(v,AV(y)+i*c,r*c);
 DO(r, *v-=j; v+=c;);
 R z;
}    /* ((i+i.r){y)-"1 ({:$y){.j */

static DF2(jtcut2);

static DF2(jtcut2sx){PROLOG(0024);DECLF;A h=0,*hv,y,yy;B b,neg,pfx,*u,*v;C id;I d,e,hn,m,n,p,t,yn,*yu,*yv;P*ap;V*vf;
 PREF2(jtcut2sx);
 n=IC(w); t=AT(w); m=*AV(sv->g); neg=0>m; pfx=m==1||m==-1; b=neg&&pfx;
 RZ(a=a==mark?eps(w,take(num[pfx?1:-1],w)):DENSE&AT(a)?sparse1(a):a);
 ASSERT(n==*AS(a),EVLENGTH);
 ap=PAV(a);
 if(!(equ(zero,SPA(ap,e))&&AN(SPA(ap,a))))R cut2(cvt(B01,a),w,self); 
 vf=VAV(fs);
 if(VGERL&sv->flag){h=sv->h; hv=AAV(h); hn=AN(h); id=0;}else id=vf->id; 
 y=SPA(ap,i); yn=AN(y); yv=AV(y); u=v=BAV(SPA(ap,x)); e=m=0;
 GATV(yy,INT,1+yn,1,0); yu=AV(yy); *yu++=p=pfx?n:-1;
 switch(pfx+(id==CLEFT||id==CRIGHT||id==CCOMMA?2:0)){
  case 0:          DO(yn, if(*v){++m;      *yu++=  yv[v-u];              } ++v;); break;
  case 1: v+=yn-1; DO(yn, if(*v){++m;      *yu++=  yv[v-u];              } --v;); break;
  case 2:          DO(yn, if(*v){++m; d=p; *yu++=p=yv[v-u]; e=MAX(e,p-d);} ++v;); break;
  case 3: v+=yn-1; DO(yn, if(*v){++m; d=p; *yu++=p=yv[v-u]; e=MAX(e,d-p);} --v;);
 }
 yu=AV(yy); p=pfx?yu[m]:0;
 if(t&DENSE){C*wv;I c,k,r,*s,wd;
  r=MAX(1,AR(w)); s=AS(w); wv=CAV(w); c=aii(w); k=c*bp(t); wd=(I)w*ARELATIVE(w);
  CUTSWITCH(EACHCUTSP)
 }else if(id==CPOUND){A z;I i,*zi; 
  GATV(z,INT,m,1,0); zi=AV(z); 
  if(pfx)for(i=m;i>=1;--i)*zi++=(yu[i-1]-yu[i  ])-neg;
  else   for(i=1;i<=m;++i)*zi++=(yu[i  ]-yu[i-1])-neg;
  EPILOG(z);
 }else{A a,ww,x,y,z,*za,zz;I c,i,j,q,qn,r;P*wp,*wwp;
  wp=PAV(w); a=SPA(wp,a); x=SPA(wp,x); y=SPA(wp,i); yv=AV(y); r=*AS(y); c=*(1+AS(y));
  RZ(ww=cps(w)); wwp=PAV(ww);
  GATV(z,BOX,m,1,0); za=AAV(z);
  switch(AN(a)&&*AV(a)?2+pfx:pfx){
   case 0:
    p=yu[0]; DO(r, if(p<=yv[c*i]){p=i; break;});
    for(i=1;i<=m;++i){
     j=yu[i]; DO(q=r-p, if(j<yv[c*(p+i)]){q=i; break;}); qn=q;
     if(neg)DO(qn=r-p, if(j-1<yv[c*(p+i)]){qn=i; break;});
     *AS(ww)=(yu[i]-yu[i-1])-neg; 
     SPB(wwp,i,sely(y,qn,p,1+yu[i-1]));
     SPB(wwp,x,selx(x,qn,p));
     RZ(*za++=zz=h?df1(ww,hv[(i-1)%hn]):CALL1(f1,ww,fs));
     // reallocate ww if it was used, which we detect by seeing the usecount incremented.  This requires that everything that
     // touches a buffer either copy it or rat().  So that ] doesn't have to rat(), we also detect reuse here if the same buffer
     // is returned to us
     p+=q; if(ww==zz||ACUC1<AC(ww)){RZ(ww=cps(w)); wwp=PAV(ww);}
    }
    break;
   case 1:
    p=yu[m]; DO(r, if(p<=yv[c*i]){p=i; break;});
    for(i=m;i>=1;--i){
     j=yu[i-1]; DO(q=r-p, if(j<=yv[c*(p+i)]){q=i; break;}); qn=q;
     if(neg){j=yu[i]; qn=0; DO(r-p, if(j<yv[c*(p+i)]){qn=q-i; break;});}
     *AS(ww)=(yu[i-1]-yu[i])-neg; 
     SPB(wwp,i,sely(y,qn,p+q-qn,yu[i]+neg));
     SPB(wwp,x,selx(x,qn,p+q-qn));
     RZ(*za++=zz=h?df1(ww,hv[(m-i)%hn]):CALL1(f1,ww,fs));
     p+=q; if(ww==zz||ACUC1<AC(ww)){RZ(ww=cps(w)); wwp=PAV(ww);}
    }
    break;
   case 2:
    for(i=1;i<=m;++i){
     q=yu[i]-yu[i-1]; *AS(ww)=q-neg;
     SPB(wwp,x,irs2(apv(q-neg,p,1L),x,0L,1L,-1L,jtfrom));
     RZ(*za++=zz=h?df1(ww,hv[(i-1)%hn]):CALL1(f1,ww,fs));
     p+=q; if(ww==zz||ACUC1<AC(ww)){RZ(ww=cps(w)); wwp=PAV(ww);}
    }
    break;
   case 3:
    for(i=m;i>=1;--i){
     q=yu[i-1]-yu[i]; *AS(ww)=q-neg;
     SPB(wwp,x,irs2(apv(q-neg,p+neg,1L),x,0L,1L,-1L,jtfrom));
     RZ(*za++=zz=h?df1(ww,hv[(i-1)%hn]):CALL1(f1,ww,fs));
     p+=q; if(ww==zz||ACUC1<AC(ww)){RZ(ww=cps(w)); wwp=PAV(ww);}
    }
    break;
  }
  z=ope(z);
  EPILOG(z);
}}   /* sparse f;.n (dense or sparse) */


static C*jtidenv0(J jt,A a,A w,V*sv,I zt,A*zz){A fs,y;
 *zz=0; 
 fs=sv->f;
 RE(y=df1(zero,iden(VAV(fs)->f)));
 if(TYPESLT(zt,AT(y))){*zz=df1(cut2(a,w,cut(ds(CBOX),sv->g)),amp(fs,ds(COPE))); R 0;}
 if(TYPESGT(zt,AT(y)))RE(y=cvt(zt,y)); 
 R CAV(y);
}    /* pointer to identity element */

#define EACHCUT(stmt)  \
 for(i=1;i<=m;++i){                                 \
  if(pfx&&i==m)q=p;                                 \
  else{u=memchr(v+pfx,sep,p-pfx); u+=!pfx; q=u-v;}  \
  d=q-neg; v1=wv+k*(b+n-p);                         \
  stmt;                                             \
  p-=q; v=u;                                        \
 }
#define EACHCUTG(stmt)  \
 for(i=1;i<=m;++i){                                 \
  if(pfx&&i==m)q=p;                                 \
  else{u=memchr(v+pfx,sep,p-pfx); u+=!pfx; q=u-v;}  \
  d=q-neg; v1=wv+k*(b+n-p);                         \
  old=jt->tnextpushx;                           \
  GA(y,wt,d*c,r,s); *AS(y)=d;                       \
  stmt;                                             \
  if(allbx&&!AR(y)&&BOX&AT(y))*za++=y=*AAV(y);      \
  else if(!allbx)*za++=y;                           \
  else{I ii=i-1;                                    \
   allbx=0;                                         \
   za=AAV(z); DO(ii, RZ(*za++=box(*za));); *za++=y; \
   old=jt->tnextpushx;                          \
  }                                                 \
  gc(y,old);                                        \
  p-=q; v=u;                                        \
 }

/* locals in cut2:                       */
/* b    1 iff _1 cut                     */
/* c    atoms in an item of w            */
/* d    adjusted length of current cut   */
/* e    max width of a cut               */
/* h    gerund                           */
/* hv   gerund                           */
/* id   function code                    */
/* k    # bytes in an item of w          */
/* m    # of cuts                        */
/* n    #a and #w                        */
/* neg  1 iff _1 or _2 cut               */
/* p    remaining length in a            */
/* pfx  1 iff 1 or _1 cut                */
/* q    length of current cut            */
/* sep  the cut character                */
/* u    ptr to a for next    cut         */
/* v    ptr to a for current cut         */
/* v1   ptr to w for current cut         */
/* wd   1 iff w is relative              */

static DF2(jtcut2){PROLOG(0025);DECLF;A h=0,*hv,y,z=0,*za;B b,neg,pfx;C id,id1,sep,*u,*v,*v1,*wv,*zc;
     I c,cv,e=0,d,hn,i,k,ke,m=0,n,old,p,q,r,*s,wd,wt,*zi,*zs;V*vf;VF ado;
 PREF2(jtcut2);
 if(SB01&AT(a)||SPARSE&AT(w))R cut2sx(a,w,self);
 p=n=IC(w); wt=AT(w); k=*AV(sv->g); neg=0>k; pfx=k==1||k==-1; b=neg&&pfx;
 if(a!=mark){
  if(!AN(a)&&n){
   if(VGERL&sv->flag){h=sv->h; ASSERT(AN(h),EVLENGTH); h=*AAV(h); R CALL1(VAV(h)->f1,w,h);}
   else R CALL1(f1,w,fs);
  }
  if(AN(a)&&BOX&AT(a))R cut2bx(a,w,self);
  if(!(B01&AT(a)))RZ(a=cvt(B01,a));
  if(!AR(a))RZ(a=reshape(sc(n),a));
  v=CAV(a); sep=C1;
 }else if(1>=AR(w)&&wt&IS1BYTE){a=w; v=CAV(a); sep=v[pfx?0:n-1];}
 else{RZ(a=n?eps(w,take(num[pfx?1:-1],w)):mtv); v=CAV(a); sep=C1;}
 ASSERT(n==IC(a),EVLENGTH);
 vf=VAV(fs);
 if(VGERL&sv->flag){h=sv->h; hv=AAV(h); hn=AN(h); id=0;}else id=vf->id; 
 r=MAX(1,AR(w)); s=AS(w); wv=CAV(w); c=aii(w); k=c*bp(wt); wd=(I)w*ARELATIVE(w);
 switch(pfx+(id==CLEFT||id==CRIGHT||id==CCOMMA?2:0)){
  case 0: if(AT(a)&B01&&C1==sep)m=bsum(n,v); 
          else{--v;    DO(n, if(sep==*++v) ++m;                    ); v=CAV(a);}    break;
  case 1: if(AT(a)&B01&&C1==*v )m=bsum(n,v);
          else{u=v+=n; DO(n, if(sep==*--v){++m;               u=v;}); p-=u-v; v=u;} break;
  case 2: u=--v;       DO(n, if(sep==*++v){++m; e=MAX(e,v-u); u=v;}); v=CAV(a);     break;
  case 3: u=v+=n;      DO(n, if(sep==*--v){++m; e=MAX(e,u-v); u=v;}); p-=u-v; v=u;
 }
 switch(wd?0:id){
  case CPOUND:
   GATV(z,INT,m,1,0); zi=AV(z); EACHCUT(*zi++=d;); 
   break;
  case CDOLLAR:
   GATV(z,INT,m,1,0); zi=AV(z); EACHCUT(*zi++=d;);
   R irs2(z,vec(INT,MAX(0,r-1),1+s),0L,0L,1L,jtover);
  case CHEAD:
   GA(z,wt,m*c,r,s); zc=CAV(z); *AS(z)=m;
   EACHCUT(if(d)MC(zc,v1,k); else fillv(wt,c,zc); zc+=k;);
   break;
  case CTAIL:
   GA(z,wt,m*c,r,s); zc=CAV(z); *AS(z)=m;
   EACHCUT(if(d)MC(zc,v1+k*(d-1),k); else fillv(wt,c,zc); zc+=k;);
   break;
  case CCOMMA:
  case CLEFT:
  case CRIGHT:
   e-=e&&neg; RE(d=mult(m*c,e));
   GA(z,wt,d,id==CCOMMA?2:1+r,s-1); zc=CAV(z); fillv(wt,d,zc);
   zs=AS(z); zs[0]=m; zs[1]=id==CCOMMA?e*c:e; ke=k*e;
   EACHCUT(MC(zc,v1,d*k);  zc+=ke;);
   break;
  case CBOX:
   GA(z,m?BOX:B01,m,1,0); za=AAV(z);
   EACHCUT(GA(y,wt,d*c,r,s); *AS(y)=d; MC(AV(y),v1,d*k); *za++=y;);
   break;
  case CAT: case CATCO: case CAMP: case CAMPCO:
   if(CBOX==ID(vf->f)&&(id1=ID(vf->g),id1==CBEHEAD||id1==CCTAIL)){
    GA(z,m?BOX:B01,m,1,0); za=AAV(z);
    EACHCUT(d=d?d-1:0; GA(y,wt,d*c,r,s); *AS(y)=d; MC(AV(y),id1==CBEHEAD?v1+k:v1,d*k); *za++=y;);
   }
   break;
  case CSLASH:
   vains(vaid(vf->f),wt,&ado,&cv);
   if(ado){C*z0=0,*zc;I t,zk,zt;
    zt=rtype(cv);
    GA(z,zt,m*c,r,s); *AS(z)=m; 
    if(!AN(z))R z;
    zc=CAV(z); zk=c*bp(zt);
    if((t=atype(cv))&&TYPESNE(t,wt)){RZ(w=cvt(t,w)); wv=CAV(w);}
    EACHCUT(if(d)ado(jt,1L,d*c,d,zc,v1); else{if(!z0){z0=idenv0(a,w,sv,zt,&y); 
        if(!z0){if(y)R y; else break;}} mvc(zk,zc,zk/c,z0);} zc+=zk;);
    if(jt->jerr)R jt->jerr>=EWOV?cut2(a,w,self):0; else R cv&VRI+VRD?cvz(cv,z):z;
 }}
 if(!z){B allbx=1;
  if(!m){y=reitem(zero,w); y=h?df1(y,*hv):CALL1(f1,y,fs); RESETERR; R iota(over(zero,shape(y?y:mtv)));}
  GATV(z,BOX,m,1,0); za=AAV(z);
  switch((wd?2:0)+(h?1:0)){
   case 0: EACHCUTG(MC(AV(y),v1,d*k);  RZ(y=CALL1(f1,y,fs));     ); break;
   case 1: EACHCUTG(MC(AV(y),v1,d*k);  RZ(y=df1(y,hv[(i-1)%hn]));); break;
   case 2: EACHCUTG(MCREL(AV(y),v1,d); RZ(y=CALL1(f1,y,fs));     ); break;
   case 3: EACHCUTG(MCREL(AV(y),v1,d); RZ(y=df1(y,hv[(i-1)%hn])););
  }
  if(!allbx)RZ(z=ope(z));
 }
 EPILOG(z);
}    /* f;.1  f;._1  f;.2  f;._2  monad and dyad */

static DF1(jtcut1){R cut2(mark,w,self);}


#define PSCASE(id,zt,wt)    ((id)+256*(zt)+1024*(wt))
#define PSLOOP(Tz,Tw,F,v0)      \
    {C*u;Tw*wv;Tz s=v0,x,*zv;                   \
     GA(z,zt,n,1,0);                            \
     u=m+av; wv=m+(Tw*)AV(w); zv=m+(Tz*)AV(z);  \
     switch(pfx+2*(id==CBSLASH)){               \
      case 0: DO(n, x=*--wv; if(*--u)s=v0; *--zv=F;              ); break;  /* <@(f/\.);.2 */  \
      case 1: DO(n, x=*--wv;               *--zv=F; if(*--u)s=v0;); break;  /* <@(f/\.);.1 */  \
      case 2: DO(n, x=*wv++;               *zv++=F; if(*u++)s=v0;); break;  /* <@(f/\ );.2 */  \
      case 3: DO(n, x=*wv++; if(*u++)s=v0; *zv++=F;              ); break;  /* <@(f/\ );.1 */  \
    }}

static A jtpartfscan(J jt,A a,A w,I cv,B pfx,C id,C ie){A z=0;B*av;I m,n,zt;
 n=AN(w); m=id==CBSDOT?n:0; zt=rtype(cv); av=BAV(a);
 switch(PSCASE(ie,CTTZ(zt),CTTZ(AT(w)))){
  case PSCASE(CPLUS,   INTX,B01X):       PSLOOP(I,B,s+=x,      0   );       break;
  case PSCASE(CPLUS,   FLX, FLX ): NAN0; PSLOOP(D,D,s+=x,      0.0 ); NAN1; break;
  case PSCASE(CMAX,    INTX,INTX):       PSLOOP(I,I,s=MAX(s,x),IMIN);       break;
  case PSCASE(CMAX,    FLX, FLX ):       PSLOOP(D,D,s=MAX(s,x),-inf);       break;
  case PSCASE(CMIN,    INTX,INTX):       PSLOOP(I,I,s=MIN(s,x),IMAX);       break;
  case PSCASE(CMIN,    FLX, FLX ):       PSLOOP(D,D,s=MIN(s,x),inf );       break;
  case PSCASE(CMAX,    B01X,B01X):
  case PSCASE(CPLUSDOT,B01X,B01X):       PSLOOP(B,B,s|=x,      0   );       break;
  case PSCASE(CMIN,    B01X,B01X):
  case PSCASE(CSTARDOT,B01X,B01X):       PSLOOP(B,B,s&=x,      1   );       break;
  case PSCASE(CNE,     B01X,B01X):       PSLOOP(B,B,s^=x,      0   );       break;
  case PSCASE(CEQ,     B01X,B01X):       PSLOOP(B,B,s=s==x,    1   );       break;
 }
 R z;
}    /* [: ; <@(ie/\);.k  on vector w */

DF2(jtrazecut2){A fs,gs,x,y,z=0;B b,neg,pfx;C id,ie=0,sep,*u,*v,*wv,*zv;I c,cv=0,d,k,m=0,n,p,q,r,*s,wt;
    V*fv,*sv,*vv;VF ado=0;
 RZ(a&&w);
 sv=VAV(self); gs=CFORK==sv->id?sv->h:sv->g; vv=VAV(gs); y=vv->f; fs=VAV(y)->g;
 p=n=IC(w); wt=AT(w); k=*AV(vv->g); neg=0>k; pfx=k==1||k==-1; b=neg&&pfx;
 fv=VAV(fs); id=fv->id;
 if((id==CBSLASH||id==CBSDOT)&&(vv=VAV(fv->f),CSLASH==vv->id)){
  ie=vaid(vv->f);
  if(id==CBSLASH)vapfx(ie,wt,&ado,&cv);  /* [: ; <@(f/\ );.n */
  else           vasfx(ie,wt,&ado,&cv);  /* [: ; <@(f/\.);.n */
 }
 if(SPARSE&AT(w))R raze(cut2(a,w,gs));
 if(a!=mark){
  if(!(AN(a)&&1==AR(a)&&AT(a)&B01+SB01))R raze(cut2(a,w,gs));
  if(AT(a)&SB01)RZ(a=cvt(B01,a));
  v=CAV(a); sep=C1;
 }else if(1>=AR(w)&&wt&IS1BYTE){a=w; v=CAV(a); sep=v[pfx?0:n-1];}
 else{RZ(a=n?eps(w,take(num[pfx?1:-1],w)):mtv); v=CAV(a); sep=C1;}
 ASSERT(n==IC(a),EVLENGTH);
 r=MAX(1,AR(w)); s=AS(w); wv=CAV(w); c=aii(w); k=c*bp(wt);
 if(pfx){u=v+n; while(u>v&&sep!=*v)++v; p=u-v;}
 if(ado){I t,zk,zt;                     /* atomic function f/\ or f/\. */
  if((t=atype(cv))&&TYPESNE(t,wt)){RZ(w=cvt(t,w)); wv=CAV(w);}
  zt=rtype(cv); zk=c*bp(zt);
  if(1==r&&!neg&&B01&AT(a)&&p==n&&v[pfx?0:n-1]){RE(z=partfscan(a,w,cv,pfx,id,ie)); if(z)R z;}
  GA(z,zt,AN(w),r,s); zv=CAV(z);
  while(p){
   if(u=memchr(v+pfx,sep,p-pfx))u+=!pfx; else{if(!pfx)break; u=v+p;}
   q=u-v;
   if(d=q-neg){
    ado(jt,1L,c*d,d,zv,wv+k*(b+n-p));
    if(jt->jerr)R jt->jerr>=EWOV?razecut2(a,w,self):0;
    m+=d; zv+=d*zk; 
   }
   p-=q; v=u;  
 }}else{B b1=0;I old,wc=c,yk,ym,yr,*ys,yt;   /* general f */
  RZ(x=gah(r,w)); ICPY(AS(x),s,r);
  while(p){
   if(u=memchr(v+pfx,sep,p-pfx))u+=!pfx; else{if(!pfx)break; u=v+p;}
   q=u-v; d=q-neg;
   *AS(x)=d; AN(x)=wc*d; AK(x)=(wv+k*(b+n-p))-(C*)x;
   old=jt->tnextpushx;
   RZ(y=df1(x,fs)); ym=IC(y);
   if(!z){yt=AT(y); yr=AR(y); ys=AS(y); c=aii(y); yk=c*bp(yt); GA(z,yt,n*c,MAX(1,yr),ys); *AS(z)=n; zv=CAV(z);}
   if(!(TYPESEQ(yt,AT(y))&&yr==AR(y)&&(1>=yr||!ICMP(1+AS(y),1+ys,yr-1)))){z=0; break;}
   while(IC(z)<=m+ym){RZ(z=ext(0,z)); zv=CAV(z); b1=0;}
   MC(zv+m*yk,CAV(y),ym*yk); 
   if(b1)gc(yt&DIRECT?0:y,old);
   b1=1; m+=ym; p-=q; v=u;
  }
  if(!b1&&ie)GA(z,wt,AN(w),r,s);
 }
 if(z){*AS(z)=m; AN(z)=m*c; R cv&VRI+VRD?cvz(cv,z):z;}
 else R raze(cut2(B01&AT(a)?a:eq(scc(sep),a),w,gs));
}    /* ;@((<@f);.n) or ([: ; <@f;.n) , monad and dyad */

DF1(jtrazecut1){R razecut2(mark,w,self);}


static A jttesos(J jt,A a,A w,I n){A p;I*av,c,k,m,*pv,s,*ws;
 RZ(a&&w);
 c=*(1+AS(a)); av=AV(a); ws=AS(w);
 GATV(p,INT,c,1,0); pv=AV(p);
 if(3==n)DO(c, m=av[i]; s=ws[i]; pv[i]=m?(s+m-1)/m:1&&s;)
 else    DO(c, m=av[i]; k=av[c+i]; s=ws[i]-ABS(k); pv[i]=0>s?0:m?(k||s%m)+s/m:1;);
 R p;
}    /* tesselation result outer shape */

static F2(jttesa){A x;I*av,c,d,k,p=IMAX,r,*s,t,*u,*v;
 RZ(a&&w);
 t=AT(a);
 RZ(a=vib(a)); 
 r=AR(a); s=AS(a); c=r?s[r-1]:1; av=AV(a); d=AR(w);
 ASSERT(d>=c&&(2>r||2==*s),EVLENGTH);
 if(2<=r)DO(c, ASSERT(0<=av[i],EVDOMAIN););
 if(2==r&&c==d&&t&INT)R a;
 GATV(x,INT,2*d,2,0); s=AS(x); s[0]=2; s[1]=d;
 u=AV(x); v=u+d; s=AS(w);
 if(2==r)DO(c,   *u++=av[i]; k=av[i+c]; *v++=k==p?s[i]:k==-p?-s[i]:k;);
 if(2> r)DO(c,   *u++=1;     k=av[i];   *v++=k==p?s[i]:k==-p?-s[i]:k;);
 s+=c;   DO(d-c, *u++=0; *v++=*s++;);
 R x;
}    /* tesselation standardized left argument */

static A jttesmatu(J jt,A a,A w,A self,A p,B e){DECLF;A x,y,z,z0;C*u,*v,*v0,*wv,*yv,*zv;
     I*av,i,k,m,mc,mi,mj,mr,nc,nr,old,*pv,r,s,*s1,sc,sj,sr,t,tc,tr,*ws,yc,yr,zk,zn,zr,*zs,zt;
 ws=AS(w); t=AT(w); k=bp(t); r=k*ws[1];
 av=AV(a); pv=AV(p); wv=CAV(w);
 nr=pv[0]; sr=av[2]; mr=av[0]; mi=r*mr; tr=ws[0];
 nc=pv[1]; sc=av[3]; mc=av[1]; mj=k*mc; sj=k*sc;
 RZ(nr&&nc&&nr>=sr&&nc>=sc);
 GA(y,t,sr*sc,2,2+av); yv=CAV(y);
 u=yv; v=wv; DO(sr, MC(u,v,sj); u+=sj; v+=r;); 
 RZ(z0=CALL1(f1,y,fs)); zt=AT(z0); 
 RZ(zt&DIRECT);
 zn=AN(z0); zr=AR(z0); zs=AS(z0); zk=zn*bp(zt); m=zr*SZI;
 GA(z,zt,zn*nr*nc,2+zr,0); s1=AS(z); ICPY(s1,pv,2); ICPY(2+s1,zs,zr); zv=CAV(z);
 old=jt->tnextpushx;
 if(e) for(i=0;i<nr;++i){  /* f;._3 */
  v=v0=wv+i*mi;
  DO(nc, 
      u=yv; DO(sr, MC(u,v,sj); u+=sj; v+=r;); v=v0+=mj; RZ(x=CALL1(f1,y,fs));
      RZ(TYPESEQ(zt,AT(x))&&zr==AR(x)&&!(m&&memcmp(zs,AS(x),m))); MC(zv,AV(x),zk); zv+=zk; tpop(old););
 }else for(i=0;i<nr;++i){  /* f;. 3 */
  v=v0=wv+i*mi; yr=MIN(tr,sr); tr-=mr; tc=ws[1];
  DO(nc, yc=MIN(tc,sc); tc-=mc; s=yc*k; 
      u=yv; DO(yr, MC(u,v,s ); u+=sj; v+=r;); v=v0+=mj; RZ(x=CALL1(f1,yr<sr||yc<sc?take(v2(yr,yc),y):y,fs));
      RZ(TYPESEQ(zt,AT(x))&&zr==AR(x)&&!(m&&memcmp(zs,AS(x),m))); MC(zv,AV(x),zk); zv+=zk; tpop(old););
 }
 R z;
}    /* f;._3 (1=e) or f;.3 (0=e), matrix w, positive size, uniform f */

static A jttesmat(J jt,A a,A w,A self,A p,B e){DECLF;A y,z,*zv,zz=0;C*u,*v,*v0,*wv,*yv;
     I*av,i,j,k,mc,mi,mj,mr,nc,nr,*pv,r,s,sc,sj,sr,t,tc,tr,*ws,yc,yr;
 ws=AS(w); t=AT(w); k=bp(t); r=k*ws[1];
 av=AV(a); pv=AV(p); wv=CAV(w);
 nr=pv[0]; sr=av[2]; mr=av[0]; mi=r*mr; tr=ws[0];
 nc=pv[1]; sc=av[3]; mc=av[1]; mj=k*mc; sj=k*sc;
 GA(y,t,sr*sc,2,2+av); yv=CAV(y);
 GATV(z,BOX,nr*nc,2,pv); zv=AAV(z);
 for(i=0;i<nr;++i){
  v=v0=wv+i*mi; yr=MIN(tr,sr); tr-=mr; tc=ws[1];
  for(j=0;j<nc;++j){
   yc=MIN(tc,sc); tc-=mc; s=yc*k; 
   if(zz==y||ACUC1<AC(y)){GA(y,t,sr*sc,2,2+av); yv=CAV(y);}  // reallo y is in use, or if returned from ][
   u=yv; DO(yr, MC(u,v,e?sj:s); u+=sj; v+=r;); v=v0+=mj; 
   *zv++=zz=CALL1(f1,e||yr==sr&&yc==sc?y:take(v2(yr,yc),y),fs);
 }}
 RE(0); R ope(z);
}    /* f;._3 (1=e) or f;.3 (0=e), matrix w, positive size */

static DF2(jttess2){A gs,p,y,z;I*av,n,t;
 PREF2(jttess2);
 RZ(a=tesa(a,w)); 
 av=AV(a); gs=VAV(self)->g; n=*AV(gs);
 RZ(p=tesos(a,w,n));
 if(DENSE&AT(w)&&2==AR(w)&&0<=av[2]&&0<=av[3]){
  RE(z=tesmatu(a,w,self,p,(B)(0>n))); 
  if(!z)z=tesmat(a,w,self,p,(B)(0>n));
  if(z&&!AN(z)){
   y=df1(w,VAV(self)->f); RESETERR;
   t=y?AT(y):B01;
   if(TYPESNE(t,AT(z)))GA(z,t,0L,AR(z),AS(z));
  }
  R z;
 }
 R cut02(irs2(cant1(tymes(head(a),cant1(abase2(p,iota(p))))), tail(a),0L,1L,1L,jtlamin2),w,self);
}

static DF1(jttess1){A s;I m,r,*v;
 RZ(w);
 r=AR(w); RZ(s=shape(w)); v=AV(s);
 m=IMAX; DO(r, if(m>v[i])m=v[i];); DO(r, v[i]=m;);
 R tess2(s,w,self);
}


F2(jtcut){A h=0;I flag=0,k;
 RZ(a&&w);
 ASSERT(NOUN&AT(w),EVDOMAIN);
 RZ(w=vi(w));
 RE(k=i0(w));
 if(NOUN&AT(a)){flag=VGERL; RZ(h=fxeachv(1L,a)); ASSERT(3!=k&&-3!=k,EVNONCE);}
 switch(k){
  case 0:          R fdef(CCUT,VERB, jtcut01,jtcut02, a,w,h, flag, RMAX,2L,RMAX);
  case 1: case -1:
  case 2: case -2: R fdef(CCUT,VERB, jtcut1, jtcut2,  a,w,h, flag, RMAX,1L,RMAX);
  case 3: case -3: R fdef(CCUT,VERB, jttess1,jttess2, a,w,h, flag, RMAX,2L,RMAX);
  default:         ASSERT(0,EVDOMAIN);
}}
