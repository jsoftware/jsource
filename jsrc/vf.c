/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Fill-Dependent Verbs                                             */

#include "j.h"


F2(jtsetfv){A q=jt->fill;I t;
 RZ(a&&w);
 t=AN(a)?AT(a):AN(w)?AT(w):0;
 if(q&&AN(q)){
  RE(t=maxtype(t,AT(q))); 
  if(TYPESNE(t,AT(q)))RZ(q=cvt(t,q));
  if(ARELATIVE(q))RZ(q=cpa(1,q));
  jt->fillv=CAV(q);
 }else{if(!t)t=AT(w); fillv(t,1L,jt->fillv0); jt->fillv=jt->fillv0;}
 if(ARELATIVE(w)){*(I*)(jt->fillv0)=AREL(*(A*)jt->fillv,w); jt->fillv=jt->fillv0;}
 R TYPESEQ(t,AT(w))?w:cvt(t,w);
}

F1(jtfiller){A z; RZ(w); GA(z,AT(w),1,0,0); fillv(AT(w),1L,CAV(z)); R z;}

// move n fills of type t to *v
void fillv(I t,I n,C*v){I k=bp(t);
 switch(CTTZ(t)){
 case RATX: mvc(n*k,v,k,&zeroQ); break;
 case XNUMX: mvc(n*k,v,k,&xzero); break;
 case B01X: case BITX: case INTX: case FLX: case CMPXX: case XDX: case XZX: case SB01X: case SFLX: case SCMPXX: case SBTX:
  memset(v,C0,k*n); break;
 case LITX: memset(v,' ',n); break;
 case C2TX: {US x=32; mvc(n*k,v,k,&x); break;}
 case C4TX: {C4 x=32; mvc(n*k,v,k,&x); break;}
 default: mvc(n*k,v,k,&mtv);
 }
}


static F2(jtrotsp){PROLOG(0071);A q,x,y,z;B bx,by;I acr,af,ar,*av,d,k,m,n,p,*qv,*s,*v,wcr,wf,wr;P*wp,*zp;
 RZ(a&&w);
 ASSERT(!jt->fill,EVNONCE);
 ar=AR(a); acr=jt->rank?jt->rank[0]:ar; af=ar-acr; p=acr?*(af+AS(a)):1;
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr; jt->rank=0; 
 if(1<acr||af)R df2(a,w,qq(qq(ds(CROT),v2(1L,RMAX)),v2(acr,wcr)));
 if(!wcr&&1<p){RZ(w=reshape(over(shape(w),apv(p,1L,0L)),w)); wr=wcr=p;}
 ASSERT(!wcr||p<=wcr,EVLENGTH);
 s=AS(w);
 GATV(q,INT,wr,1L,0); qv=AV(q); memset(qv,C0,wr*SZI); 
 RZ(a=vi(a)); v=AV(a); 
 DO(p, k=v[i]; d=s[wf+i]; qv[wf+i]=!d?0:0<k?k%d:k==IMIN?d-(-d-k)%d:d-(-k)%d;);
 wp=PAV(w); a=SPA(wp,a); RZ(y=ca(SPA(wp,i))); m=IC(y);
 n=AN(a); RZ(a=paxis(wr,a)); av=AV(a);
 RZ(q=from(a,q)); qv=AV(q);
 GA(z,AT(w),1,wr,s); zp=PAV(z);
 by=0; DO(n,    if(qv[  i]){by=1; break;});
 bx=0; DO(wr-n, if(qv[n+i]){bx=1; break;});
 RZ(x=!bx?ca(SPA(wp,x)):irs2(vec(INT,wr-n,n+qv),SPA(wp,x),0L,1L,-1L,jtrotate));
 if(by){
  DO(n, if(k=qv[i]){d=s[av[i]]-k; v=i+AV(y); DO(m, *v<k?(*v+=d):(*v-=k); v+=n;);});
  RZ(q=grade1(y)); RZ(y=from(q,y)); RZ(x=from(q,x));
 }
 SPB(zp,a,ca(SPA(wp,a))); 
 SPB(zp,e,ca(SPA(wp,e))); 
 SPB(zp,x,x);
 SPB(zp,i,y);
 EPILOG(z);
}    /* a|."r w on sparse arrays */

#define ROF(r) r=r<-n?-n:n<r?n:r; x=dk*ABS(r); y=e-x; j=0>r?x:0; k=0>r?0:x;
#define ROT(r) r=r%n;             x=dk*ABS(r); y=e-x; j=0>r?y:x; k=0>r?x:y;

static void jtrot(J jt,I m,I c,I n,I k,I p,I*av,C*u,C*v){I dk,e,j,r,x,y;
 e=c*k; dk=e/n; if(jt->fill)mvc(m*e,v,k,jt->fillv);   
 switch((jt->fill?0:2)+(1<p)){
  case 0: r=p?*av:0;     ROF(r); DO(m, MC(j+v,k+u,y);            u+=e; v+=e;); break;
  case 1: DO(m, r=av[i]; ROF(r);       MC(j+v,k+u,y);            u+=e; v+=e;); break;
  case 2: r=p?*av:0;     ROT(r); DO(m, MC(v,j+u,k); MC(k+v,u,j); u+=e; v+=e;); break;
  case 3: DO(m, r=av[i]; ROT(r);       MC(v,j+u,k); MC(k+v,u,j); u+=e; v+=e;);
}}

/* m   # cells
   c   # atoms in each cell
   n   # items in each cell
   k   # bytes in each atom
   p   length of av
   av  rotation amount(s)
   u   source data area 
   v   target data area      */

F2(jtrotate){A y,z;B b;C*u,*v;I acr,af,ar,*av,k,m,n,p,*s,wcr,wf,wn,wr;
 RZ(a&&w);
 if(SPARSE&AT(w))R rotsp(a,w);
 ar=AR(a); acr=jt->rank?jt->rank[0]:ar; af=ar-acr; p=acr?*(af+AS(a)):1;
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr; jt->rank=0;
 if(1<acr||af&&acr||af&&!wf)R df2(a,w,qq(qq(ds(CROT),v2(1L,RMAX)),v2(acr,wcr)));
 if(!wcr&&1<p){RZ(w=reshape(over(shape(w),apv(p,1L,0L)),w)); wr=wcr=p;}
 ASSERT(!wcr||p<=wcr,EVLENGTH);
 RZ(a=vi(a)); av=AV(a);
 RZ(w=setfv(w,w)); u=CAV(w); wn=AN(w); s=AS(w); k=bp(AT(w));
 GA(z,AT(w),wn,wr,s); v=CAV(z);
 if(!wn)R z;
 PROD(m,wf,s); n=wcr?s[wf]:1;
 rot(m,wn/m,n,k,1>=p?AN(a):1L,av,u,v);
 if(1<p){
  GA(y,AT(w),wn,wr,s); u=CAV(y); 
  b=0; s+=wf;
  DO(p-1, m*=n; n=*++s; rot(m,wn/m,n,k,1L,av+i+1,b?u:v,b?v:u); b=!b;);
  z=b?y:z;
 } 
 R RELOCATE(w,z);
}    /* a|.!.f"r w */


static F1(jtrevsp){A a,q,x,y,z;I c,f,k,m,n,r,*v,wr;P*wp,*zp;
 RZ(w);
 ASSERT(!jt->fill,EVNONCE);
 wr=AR(w); r=jt->rank?jt->rank[1]:wr; f=wr-r; jt->rank=0;
 m=*(f+AS(w)); wp=PAV(w);
 GA(z,AT(w),1,wr,AS(w)); zp=PAV(z);
 a=SPA(wp,a); n=AN(a); RZ(y=ca(SPA(wp,i))); x=SPA(wp,x);
 RZ(q=paxis(wr,a)); v=AV(q); DO(wr, if(f==v[i]){k=i; break;});
 if(!r)       RZ(x=ca(x))
 else if(k>=n)RZ(x=irs2(apv(m,m-1,-1L),x,0L,1L,wr-k,jtfrom))
 else         {v=k+AV(y); c=m-1; DO(IC(y), *v=c-*v; v+=n;); q=grade1(y); RZ(y=from(q,y)); RZ(x=from(q,x));}
 SPB(zp,a,ca(a)); 
 SPB(zp,e,ca(SPA(wp,e))); 
 SPB(zp,i,y); 
 SPB(zp,x,x);
 R z;
}    /* |."r w on sparse arrays */

F1(jtreverse){A z;C*wv,*zv;I f,k,m,n,nk,r,*v,*ws,wt,wr;
 RZ(w);
 if(SPARSE&AT(w))R revsp(w);
 if(jt->fill)R rotate(num[-1],w);
 wr=AR(w); r=jt->rank?jt->rank[1]:wr; f=wr-r;
 if(!(r&&AN(w))){RZ(z=ca(w)); R ARELATIVE(w)?relocate((I)w-(I)z,z):z;}
 wt=AT(w); ws=AS(w); wv=CAV(w);
 n=ws[f]; 
 m=1; DO(f, m*=ws[i];);
 k=bp(wt); v=1+f+ws; DO(r-1, k*=*v++;); nk=n*k;
 GA(z,wt,AN(w),wr,ws); zv=CAV(z);
 switch(k){
  default:        {C*s=wv-k,*t; DO(m, t=s+=nk; DO(n, MC(zv,t,k); zv+=k; t-=k;););} break;
  case sizeof(C): {C*s=    wv,*t,*u=    zv; DO(m, t=s+=n; DO(n, *u++=*--t;););} break;
  case sizeof(S): {S*s=(S*)wv,*t,*u=(S*)zv; DO(m, t=s+=n; DO(n, *u++=*--t;););} break;
#if SY_64
  case sizeof(int):{int*s=(int*)wv,*t,*u=(int*)zv; DO(m, t=s+=n; DO(n, *u++=*--t;););} break;
#endif
  case sizeof(I): {I*s=(I*)wv,*t,*u=(I*)zv; DO(m, t=s+=n; DO(n, *u++=*--t;););} break;
#if !SY_64 && SY_WIN32
  case sizeof(D): {D*s=(D*)wv,*t,*u=(D*)zv; DO(m, t=s+=n; DO(n, *u++=*--t;););} break;
#endif
 }
 R RELOCATE(w,z);
}    /* |."r w */


static A jtreshapesp0(J jt,A a,A w,I wf,I wcr){A e,p,x,y,z;B*b,*pv;I c,d,r,*v,wr,*ws;P*wp,*zp;
 wr=AR(w); ws=AS(w);
 wp=PAV(w); RZ(b=bfi(wr,SPA(wp,a),1));
 RZ(e=ca(SPA(wp,e))); x=SPA(wp,x); y=SPA(wp,i);
 v=AS(y); r=v[0]; c=v[1]; d=0; DO(wf, if(b[i])++d;);
 if(!wf){if(r&&c){v=AV(y); DO(c, if(v[i])R e;);} R AN(x)?reshape(mtv,x):e;}
 GA(z,AT(w),1,wf,ws);
 zp=PAV(z); SPB(zp,e,e); SPB(zp,a,ifb(wf,b));
 GATV(p,B01,r,1,0); pv=BAV(p);
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
 wp=PAV(w); a1=SPA(wp,a); c=AN(a1); RZ(b=bfi(wr,a1,1));
 RZ(e=ca(SPA(wp,e))); x=SPA(wp,x); y=SPA(wp,i);
 u=av+an; v=ws+wr; m=0; DO(MIN(an,wcr-1), if(*--u!=*--v){m=1; break;});
 if(m||an<wcr) R reshapesp(a,irs1(w,0L,wcr,jtravel),wf,1L);
 ASSERT(!jt->fill,EVDOMAIN);
 GA(z,AT(w),1,wf+an,ws); ICPY(wf+AS(z),av,an);
 zp=PAV(z); SPB(zp,e,e);  
 GATV(t,INT,c+d*b[wf],1,0); v=AV(t); 
 DO(wf, if(b[i])*v++=i;); if(b[wf])DO(d, *v++=wf+i;); j=wf; DO(wcr, if(b[j])*v++=d+j; ++j;);
 SPB(zp,a,t);
 if(b[wf]){I n,q,r,*v0;   /* sparse */
  if(wf!=*AV(a1))R rank2ex(a,w,0L,1L,wcr,jtreshape);
  RE(m=prod(1+d,av)); n=IC(y); q=n*(m/ws[wf]); r=m%ws[wf];
  v=AV(y); DO(n, if(r<=*v)break; ++q; v+=c;);
  GATV(t,INT,q,1,0); u=AV(t); v=v0=AV(y);
  m=j=0; DO(q, u[i]=m+*v; v+=c; ++j; if(j==n){j=0; v=v0; m+=ws[wf];});
  SPB(zp,i,stitch(abase2(vec(INT,1+d,av),t),reitem(sc(q),dropr(1L,y))));
  SPB(zp,x,reitem(sc(q),x));
 }else{                   /* dense  */
  GATV(t,INT,an,1,0); v=AV(t); ICPY(v,av,d); m=d; j=wf; DO(wcr, if(!b[j++])v[m++]=av[i+d];);
  SPB(zp,i,ca(y));
  SPB(zp,x,irs2(vec(INT,m,v),x,0L,1L,wcr-(an-m),jtreshape));
 }
 R z;
}    /* a ($,)"wcr w for sparse w and scalar or vector a */

F2(jtreshape){A z;B b;C*wv,*zv;I acr,ar,c,k,m,n,p,q,r,*s,t,*u,wcr,wf,wn,wr,*ws,zn;
 RZ(a&&w);
 ar=AR(a); acr=jt->rank?jt->rank[0]:ar;
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr; ws=AS(w); jt->rank=0;
 if(1<acr||acr<ar)R rank2ex(a,w,0L,MIN(1,acr),wcr,jtreshape);
 RZ(a=vip(a)); r=AN(a); u=AV(a);
 if(SPARSE&AT(w))R reshapesp(a,w,wf,wcr);
 wn=AN(w); RE(m=prod(r,u)); CPROD(wn,c,wf,ws); CPROD(wn,n,wcr,wf+ws);
 ASSERT(n||!m||jt->fill,EVLENGTH);
 b=jt->fill&&m>n; if(b)RZ(w=setfv(w,w)); 
 t=AT(w); k=bp(t); p=k*m; q=k*n;
 RE(zn=mult(c,m));
 GA(z,t,zn,r+wf,0); s=AS(z); ICPY(s,ws,wf); ICPY(wf+s,u,r);
 if(!zn)R z;
 zv=CAV(z); wv=CAV(w); 
 if(b)DO(c, mvc(q,zv,q,wv); mvc(p-q,q+zv,k,jt->fillv); zv+=p; wv+=q;)
 else DO(c, mvc(p,zv,q,wv); zv+=p; wv+=q;);
 R RELOCATE(w,z);
}    /* a ($,)"r w */

F2(jtreitem){A y;I acr,an,ar,m,r,*v,wcr,wr;
 RZ(a&&w);
 ar=AR(a); acr=jt->rank?jt->rank[0]:ar; m=MIN(1,acr);
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; r=wcr-1; jt->rank=0;
 if(1<acr||acr<ar)R rank2ex(a,w,0L,m,wcr,jtreitem);
 if(1>=wcr)y=a;
 else{ 
  RZ(a=vi(a)); an=AN(a); m=1;
  GATV(y,INT,an+r,1,0); v=AV(y);
  ICPY(v,AV(a),an); ICPY(v+an,AS(w)+wr-r,r);
 }
 R ar==acr&&wr==wcr?reshape(y,w):irs2(y,w,0L,m,wcr,jtreshape);
}    /* a $"r w */

#if SY_64
#define EXPAND(T)  \
  {T*u=(T*)wv,*v=(T*)zv,x;                                                \
   mvc(sizeof(T),&x,k,jt->fillv);                                         \
   DO(an, if(*av++){ASSERT(wx>(C*)u,EVLENGTH); *v++=*u++;}else *v++=x;);  \
   wv=(C*)u;                                                              \
  }
#else
#define EXPAND(T)  \
  {T*u=(T*)wv,*v=(T*)zv,x;                                                                       \
   mvc(sizeof(T),&x,k,jt->fillv);                                                                \
   for(i=0;i<q;++i)switch(*au++){                                                                \
    case B0000:                              *v++=x;    *v++=x;    *v++=x;    *v++=x;    break;  \
    case B0001: ASSERT(wx>  (C*)u,EVLENGTH); *v++=x;    *v++=x;    *v++=x;    *v++=*u++; break;  \
    case B0010: ASSERT(wx>  (C*)u,EVLENGTH); *v++=x;    *v++=x;    *v++=*u++; *v++=x;    break;  \
    case B0011: ASSERT(wx>1+(C*)u,EVLENGTH); *v++=x;    *v++=x;    *v++=*u++; *v++=*u++; break;  \
    case B0100: ASSERT(wx>  (C*)u,EVLENGTH); *v++=x;    *v++=*u++; *v++=x;    *v++=x;    break;  \
    case B0101: ASSERT(wx>1+(C*)u,EVLENGTH); *v++=x;    *v++=*u++; *v++=x;    *v++=*u++; break;  \
    case B0110: ASSERT(wx>1+(C*)u,EVLENGTH); *v++=x;    *v++=*u++; *v++=*u++; *v++=x;    break;  \
    case B0111: ASSERT(wx>2+(C*)u,EVLENGTH); *v++=x;    *v++=*u++; *v++=*u++; *v++=*u++; break;  \
    case B1000: ASSERT(wx>  (C*)u,EVLENGTH); *v++=*u++; *v++=x;    *v++=x;    *v++=x;    break;  \
    case B1001: ASSERT(wx>1+(C*)u,EVLENGTH); *v++=*u++; *v++=x;    *v++=x;    *v++=*u++; break;  \
    case B1010: ASSERT(wx>1+(C*)u,EVLENGTH); *v++=*u++; *v++=x;    *v++=*u++; *v++=x;    break;  \
    case B1011: ASSERT(wx>2+(C*)u,EVLENGTH); *v++=*u++; *v++=x;    *v++=*u++; *v++=*u++; break;  \
    case B1100: ASSERT(wx>1+(C*)u,EVLENGTH); *v++=*u++; *v++=*u++; *v++=x;    *v++=x;    break;  \
    case B1101: ASSERT(wx>2+(C*)u,EVLENGTH); *v++=*u++; *v++=*u++; *v++=x;    *v++=*u++; break;  \
    case B1110: ASSERT(wx>2+(C*)u,EVLENGTH); *v++=*u++; *v++=*u++; *v++=*u++; *v++=x;    break;  \
    case B1111: ASSERT(wx>3+(C*)u,EVLENGTH); *v++=*u++; *v++=*u++; *v++=*u++; *v++=*u++; break;  \
   }                                                                                             \
   if(r){av=(B*)au; DO(r, if(*av++){ASSERT(wx>(C*)u,EVLENGTH); *v++=*u++;}else *v++=x;);}        \
   wv=(C*)u;                                                                                     \
  }
#endif

F2(jtexpand){A z;B*av;C*wv,*wx,*zv;I an,*au,i,k,p,q,r,wc,wk,wn,wt,zn;
 RZ(a&&w);
 if(!(B01&AT(a)))RZ(a=cvt(B01,a));
 ASSERT(1==AR(a),EVRANK);
 RZ(w=setfv(w,w)); 
 if(!AR(w))R from(a,take(num[-2],w));
 av=BAV(a); an=AN(a); q=an/SZI; r=an%SZI; au=(I*)av;
 wv=CAV(w); wn=AN(w); wc=aii(w); wt=AT(w); k=bp(wt); wk=k*wc; wx=wv+wk**AS(w);
 RE(zn=mult(an,wc));
 GA(z,wt,zn,AR(w),AS(w)); *AS(z)=an; zv=CAV(z);
 switch(wk){
  case sizeof(C): EXPAND(C); break;
  case sizeof(S): EXPAND(S); break;
#if SY_64
  case sizeof(int): EXPAND(int); break;
#endif
  case sizeof(I): EXPAND(I); break;
  default:  
   mvc(k*zn,zv,k,jt->fillv); 
   for(i=p=0;i<an;++i)
    if(*av++)p+=wk; 
    else{if(p){ASSERT(wx>=wv+p,EVLENGTH); MC(zv,wv,p); wv+=p; zv+=p; p=0;} zv+=wk;}
   if(p){ASSERT(wx>=wv+p,EVLENGTH); MC(zv,wv,p); wv+=p;}
 }
 ASSERT(wx==wv,EVLENGTH);
 R z;
}    /* a&#^:_1 w or a&#^:_1!.f w */



