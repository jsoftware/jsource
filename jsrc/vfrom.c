/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: From & Associates. See Hui, Some Uses of { and }, APL87.         */

#include "j.h"


F1(jtcatalog){PROLOG(0072);A b,*wv,x,z,*zv;C*bu,*bv,**pv;I*cv,i,j,k,m=1,n,p,*qv,r=0,*s,t=0,*u,wd;
 F1RANK(1,jtcatalog,0);
 if(!(AN(w)&&AT(w)&BOX+SBOX))R box(w);
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w);
 DO(n, x=WVR(i); if(AN(x)){p=AT(x); t=t?t:p; ASSERT(HOMO(t,p),EVDOMAIN); RE(t=maxtype(t,p));});
 RE(t=maxtype(B01,t)); k=bp(t);
 GA(b,t,n,1,0);      bv=CAV(b);
 GATV(x,INT,n,1,0);    qv=AV(x);
 GATV(x,BOX,n,1,0);    pv=(C**)AV(x);
 RZ(x=apv(n,0L,0L)); cv=AV(x);
 DO(n, x=WVR(i); if(TYPESNE(t,AT(x)))RZ(x=cvt(t,x)); r+=AR(x); qv[i]=p=AN(x); RE(m=mult(m,p)); pv[i]=CAV(x););
 GATV(z,BOX,m,r,0);    zv=AAV(z); s=AS(z); 
 DO(n, x=WVR(i); u=AS(x); DO(AR(x),*s++=*u++;););
 for(i=0;i<m;i++){
  bu=bv-k;
  DO(n, MC(bu+=k,pv[i]+k*cv[i],k););
  DO(n, j=n-1-i; if(qv[j]>++cv[j])break; cv[j]=0;);
  RZ(*zv++=ca(b));
 }
 EPILOG(z);
}

#define SETJ(jexp)    {j=(jexp); if(0<=j)ASSERT(j<p,EVINDEX) else{j+=p; ASSERT(0<=j,EVINDEX);}}

#define IFROMLOOP(T)        \
 {T   *v=(T*)wv,*x=(T*)zv;  \
  if(1==an){v+=j;   DO(m,                                    *x++=*v;       v+=p; );}  \
  else              DO(m, DO(an, SETJ(av[i]);                *x++=v[j];);   v+=p; );   \
 }
#define IFROMLOOP2(T,qexp)  \
 {T*u,*v=(T*)wv,*x=(T*)zv;  \
  q=(qexp); pq=p*q;         \
  if(1==an){v+=j*q; DO(m,                     u=v;     DO(q, *x++=*u++;);   v+=pq;);}  \
  else              DO(m, DO(an, SETJ(av[i]); u=v+j*q; DO(q, *x++=*u++;);); v+=pq;);   \
 }

F2(jtifrom){A z;C*wv,*zv;I acr,an,ar,*av,j,k,m,p,pq,q,*s,wcn,wcr,wf,wk,wn,wr,*ws,zn;
 RZ(a&&w);
 // This routine is implemented as if it had infinite rank: if no rank is specified, it operates on the entire
 // a (and w).  This has implications for empty arguments.
 ar=AR(a); acr=jt->rank?jt->rank[0]:ar;
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr; jt->rank=0;
 if(ar>acr)R rank2ex(a,w,0L,acr,wcr,jtifrom);  // split a into cells if needed
 // From here on, execution on a single cell of a (on a single cell of w)
 an=AN(a); wn=AN(w); ws=AS(w);
 if(!(INT&AT(a)))RZ(a=cvt(INT,a));
 // If a is empty, it needs to simulate execution on a cell of fills.  But that might produce domain error, if w has no
 // items, where 0 { empty is an index error!  In that case, we set wr to 0, in effect making it an atom (since failing exec on fill-cell produces atomic result)
// if(an==0 && wn==0 && ws[wf]==0)wcr=wr=0;
 p=j=wcr?*(ws+wf):1; j=j?j:1;  // j is #items in a cell of w.  m is #cells of w
 m=prod(wf,ws); RE(zn=mult(an,wn/j)); wcn=wn/(m?m:1); k=bp(AT(w))*wcn/j; wk=j*k;
 GA(z,AT(w),zn,ar+wr-(0<wcr),ws);  // result-shape is shape of a followed by shape of cell of w; start with w-shape, which gets the frame
 s=AS(z); ICPY(s+wf,AS(a),ar); if(wcr)ICPY(s+wf+ar,1+wf+ws,wcr-1);
 av=AV(a); wv=CAV(w); zv=CAV(z); if(an)SETJ(*av);
 if(AT(w)&FL+CMPX){if(k==sizeof(D))IFROMLOOP(D) else IFROMLOOP2(D,k/sizeof(D));}
 else switch(k){
  case sizeof(C): IFROMLOOP(C); break; 
  case sizeof(S): IFROMLOOP(S); break;  
#if SY_64
  case sizeof(int):IFROMLOOP(int); break;
#endif
  case sizeof(I): IFROMLOOP(I); break;
  default:
   if     (0==k%sizeof(I))IFROMLOOP2(I,k/sizeof(I))
#if SY_64
   else if(0==k%sizeof(int))IFROMLOOP2(int,k/sizeof(int))
#endif
   else if(0==k%sizeof(S))IFROMLOOP2(S,k/sizeof(S))
   else{S*x,*u;
    q=1+k/sizeof(S);
    if(1==an){wv+=k*j; DO(m,                     x=(S*)zv; u=(S*) wv;      DO(q, *x++=*u++;); zv+=k;   wv+=wk;);}
    else               DO(m, DO(an, SETJ(av[i]); x=(S*)zv; u=(S*)(wv+k*j); DO(q, *x++=*u++;); zv+=k;); wv+=wk;);
 }}
 R RELOCATE(w,z);
}    /* a{"r w for numeric a */

#define BSET(x,y0,y1,y2,y3)     *x++=y0; *x++=y1; *x++=y2; *x++=y3;
#define BSETV(b)                MC(v,wv+b*k,k); v+=k;

#if !SY_64 && SY_WIN32
#define BNNERN(T)   \
 {B*au=av;T*v=(T*)wv,v0,v1,*x=(T*)zv;                                               \
  DO(m, v0=v[0]; v1=v[1]; u=(I*)av; DO(q, switch(*u++){                             \
   case B0000: BSET(x,v0,v0,v0,v0); break;  case B0001: BSET(x,v0,v0,v0,v1); break; \
   case B0010: BSET(x,v0,v0,v1,v0); break;  case B0011: BSET(x,v0,v0,v1,v1); break; \
   case B0100: BSET(x,v0,v1,v0,v0); break;  case B0101: BSET(x,v0,v1,v0,v1); break; \
   case B0110: BSET(x,v0,v1,v1,v0); break;  case B0111: BSET(x,v0,v1,v1,v1); break; \
   case B1000: BSET(x,v1,v0,v0,v0); break;  case B1001: BSET(x,v1,v0,v0,v1); break; \
   case B1010: BSET(x,v1,v0,v1,v0); break;  case B1011: BSET(x,v1,v0,v1,v1); break; \
   case B1100: BSET(x,v1,v1,v0,v0); break;  case B1101: BSET(x,v1,v1,v0,v1); break; \
   case B1110: BSET(x,v1,v1,v1,v0); break;  case B1111: BSET(x,v1,v1,v1,v1); break; \
  });                                                                               \
  b=(B*)u; DO(r, *x++=*b++?v1:v0;); v+=p;);                                         \
 }
#define BNNERM(T,T1)   \
 {B*au=av;T*c,*v=(T*)wv,v0,v1,*x=(T*)zv;T1 vv[16],*y;                                  \
  DO(m, v0=v[0]; v1=v[1]; c=(T*)vv; y=(T1*)x; u=(I*)av;                                \
   BSET(c,v0,v0,v0,v0); BSET(c,v0,v0,v0,v1); BSET(c,v0,v0,v1,v0); BSET(c,v0,v0,v1,v1); \
   BSET(c,v0,v1,v0,v0); BSET(c,v0,v1,v0,v1); BSET(c,v0,v1,v1,v0); BSET(c,v0,v1,v1,v1); \
   BSET(c,v1,v0,v0,v0); BSET(c,v1,v0,v0,v1); BSET(c,v1,v0,v1,v0); BSET(c,v1,v0,v1,v1); \
   BSET(c,v1,v1,v0,v0); BSET(c,v1,v1,v0,v1); BSET(c,v1,v1,v1,v0); BSET(c,v1,v1,v1,v1); \
   DO(q, switch(*u++){                                                                 \
    case B0000: *y++=vv[ 0]; break;  case B0001: *y++=vv[ 1]; break;                   \
    case B0010: *y++=vv[ 2]; break;  case B0011: *y++=vv[ 3]; break;                   \
    case B0100: *y++=vv[ 4]; break;  case B0101: *y++=vv[ 5]; break;                   \
    case B0110: *y++=vv[ 6]; break;  case B0111: *y++=vv[ 7]; break;                   \
    case B1000: *y++=vv[ 8]; break;  case B1001: *y++=vv[ 9]; break;                   \
    case B1010: *y++=vv[10]; break;  case B1011: *y++=vv[11]; break;                   \
    case B1100: *y++=vv[12]; break;  case B1101: *y++=vv[13]; break;                   \
    case B1110: *y++=vv[14]; break;  case B1111: *y++=vv[15]; break;                   \
   });                                                                                 \
   b=(B*)u; x=(T*)y; DO(r, *x++=*b++?v1:v0;); v+=p;);                                  \
 }
#else
#define BNNERN(T)       {T*v=(T*)wv,*x=(T*)zv; DO(m, b=av; DO(an, *x++=*(v+*b++);); v+=p;);}
#define BNNERM(T,T1)    BNNERN(T)
#endif

#define INNER1B(T)  {T*v=(T*)wv,*x=(T*)zv; v+=*av; DO(m, *x++=*v; v+=p;);}

static F2(jtbfrom){A z;B*av,*b;C*wv,*zv;I acr,an,ar,k,m,p,q,r,*s,*u=0,wcn,wcr,wf,wk,wn,wr,*ws,zn;
 RZ(a&&w);
 ar=AR(a); acr=jt->rank?jt->rank[0]:ar;
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr; jt->rank=0;
 if(ar>acr)R rank2ex(a,w,0L,acr,wcr,jtbfrom);
 an=AN(a); wn=AN(w); ws=AS(w);
 // If a is empty, it needs to simulate execution on a cell of fills.  But that might produce domain error, if w has no
 // items, where 0 { empty is an index error!  In that case, we set wr to 0, in effect making it an atom (since failing exec on fill-cell produces atomic result)
// if(an==0 && wn==0 && ws[wf]==0)wcr=wr=0;
 p=wcr?*(ws+wf):1; q=an/SZI; r=an%SZI;
 ASSERT(2<=p||1==p&&all0(a)||!p&&!an,EVINDEX);
 p=p?p:1; RE(m=prod(wf,ws)); RE(zn=mult(an,wn/p)); wcn=wn/(m?m:1); k=bp(AT(w))*wcn/p; wk=p*k;
 GA(z,AT(w),zn,ar+wr-(0<wcr),ws); 
 s=AS(z); ICPY(s+wf,AS(a),ar); if(wcr)ICPY(s+wf+ar,1+wf+ws,wcr-1);
 av=BAV(a); wv=CAV(w); zv=CAV(z);
 switch(k+k+(1==an)){
  case   2*sizeof(I): BNNERN(I);   break;
  case   2*sizeof(C): BNNERM(C,I); break; 
  case 1+2*sizeof(C): INNER1B(C);  break;
  case 1+2*sizeof(S): INNER1B(S);  break;
#if SY_64
  case 1+2*sizeof(int): INNER1B(int);  break;
#endif
  case 1+2*sizeof(I): INNER1B(I);  break;
  default:
   if(1==an){wv+=k**av; DO(m, MC(zv,wv,k); zv+=k; wv+=wk;);}
#if !SY_64 && SY_WIN32
   else{A x;C*v,*xv,*xv00,*xv01,*xv02,*xv03,*xv04,*xv05,*xv06,*xv07,*xv08,*xv09,*xv10,*xv11,
         *xv12,*xv13,*xv14,*xv15;I i,j,k4=k*4;
    GATV(x,LIT,16*k4,1,0); xv=CAV(x);
    xv00=xv;       xv01=xv+   k4; xv02=xv+ 2*k4; xv03=xv+ 3*k4;
    xv04=xv+ 4*k4; xv05=xv+ 5*k4; xv06=xv+ 6*k4; xv07=xv+ 7*k4;
    xv08=xv+ 8*k4; xv09=xv+ 9*k4; xv10=xv+10*k4; xv11=xv+11*k4;
    xv12=xv+12*k4; xv13=xv+13*k4; xv14=xv+14*k4; xv15=xv+15*k4;
    for(i=0;i<m;++i){
     u=(I*)av; v=xv;
     BSETV(0); BSETV(0); BSETV(0); BSETV(0);   BSETV(0); BSETV(0); BSETV(0); BSETV(1);
     BSETV(0); BSETV(0); BSETV(1); BSETV(0);   BSETV(0); BSETV(0); BSETV(1); BSETV(1);
     BSETV(0); BSETV(1); BSETV(0); BSETV(0);   BSETV(0); BSETV(1); BSETV(0); BSETV(1);
     BSETV(0); BSETV(1); BSETV(1); BSETV(0);   BSETV(0); BSETV(1); BSETV(1); BSETV(1);
     BSETV(1); BSETV(0); BSETV(0); BSETV(0);   BSETV(1); BSETV(0); BSETV(0); BSETV(1);
     BSETV(1); BSETV(0); BSETV(1); BSETV(0);   BSETV(1); BSETV(0); BSETV(1); BSETV(1);
     BSETV(1); BSETV(1); BSETV(0); BSETV(0);   BSETV(1); BSETV(1); BSETV(0); BSETV(1);
     BSETV(1); BSETV(1); BSETV(1); BSETV(0);   BSETV(1); BSETV(1); BSETV(1); BSETV(1);
     for(j=0;j<q;++j,zv+=k4)switch(*u++){
      case B0000: MC(zv,xv00,k4); break;   case B0001: MC(zv,xv01,k4); break;
      case B0010: MC(zv,xv02,k4); break;   case B0011: MC(zv,xv03,k4); break;  
      case B0100: MC(zv,xv04,k4); break;   case B0101: MC(zv,xv05,k4); break;
      case B0110: MC(zv,xv06,k4); break;   case B0111: MC(zv,xv07,k4); break;
      case B1000: MC(zv,xv08,k4); break;   case B1001: MC(zv,xv09,k4); break;
      case B1010: MC(zv,xv10,k4); break;   case B1011: MC(zv,xv11,k4); break;
      case B1100: MC(zv,xv12,k4); break;   case B1101: MC(zv,xv13,k4); break;
      case B1110: MC(zv,xv14,k4); break;   case B1111: MC(zv,xv15,k4); break;
     }
     b=(B*)u; DO(r, MC(zv,wv+k**b++,k); zv+=k;); wv+=wk;
   }}
#else
   else DO(m, b=av; DO(an, MC(zv,wv+k**b++,k); zv+=k;); wv+=wk;);
#endif
 }
 R RELOCATE(w,z);
}    /* a{"r w for boolean a */

A jtfrombu(J jt,A a,A w,I wf){A p,q,z;B b=0;I ar,*as,h,m,r,*u,*v,wcr,wr,*ws;
 ar=AR(a); as=AS(a); h=as[ar-1];
 wr=AR(w); ws=AS(w); wcr=wr-wf;
 DO(ar, if(!as[i]){b=1; break;});
 DO(wr, if(!ws[i]){b=1; break;});
 if(b){
  GA(z,AT(w),0,wf+(wcr-h)+(ar-1),0); u=AS(z);
  v=ws;      DO(wf,    *u++=*v++;);
  v=as;      DO(ar-1,  *u++=*v++;);
  v=ws+wf+h; DO(wcr-h, *u++=*v++;);
  R z;
 } 
 GATV(p,INT,h,1,0); v=AV(p)+h; u=ws+wf+h; m=1; DO(h, *--v=m; m*=*--u;);
 r=wr+1-h;
 if(r==wr)
  z=irs2(pdt(a,p),w,VFLAGNONE, RMAX,wcr+1-h,jtifrom);
 else if(ARELATIVE(w)){
  GATV(q,INT,r,1,0); 
  v=AV(q); ICPY(v,ws,wf); *(v+wf)=m; ICPY(v+wf+1,ws+wf+h,wcr-h); RZ(q=reshape(q,w));
  z=irs2(pdt(a,p),q,VFLAGNONE, RMAX,wcr+1-h,jtifrom);
 }else{
  RZ(q=gah(r,w)); v=AS(q); ICPY(v,ws,wf); *(v+wf)=m; ICPY(v+wf+1,ws+wf+h,wcr-h);  /* q is reshape(.,w) */
  z=irs2(pdt(a,p),q,VFLAGNONE, RMAX,wcr+1-h,jtifrom);
 }
 R z;
}    /* (<"1 a){"r w, dense w, integer array a */

B jtaindex(J jt,A a,A w,I wf,A*ind){A*av,q,z;I ad,an,ar,c,j,k,t,*u,*v,*ws;
 RZ(a&&w);
 an=AN(a); *ind=0;
 if(!an)R 0;
 ws=wf+AS(w); ar=AR(a); av=AAV(a); ad=(I)a*ARELATIVE(a); q=AVR(0); c=AN(q);
 if(!c)R 0;
 ASSERT(c<=AR(w)-wf,EVLENGTH);
 GATV(z,INT,an*c,1+ar,AS(a)); *(ar+AS(z))=c; v=AV(z);
 for(j=0;j<an;++j){
  q=AVR(j); t=AT(q);
  if(t&BOX)R 0;
  if(!(t&INT))RZ(q=cvt(INT,q));
  if(!(c==AN(q)&&1>=AR(q)))R 0; 
  u=AV(q);
  DO(c, k=u[i]; if(0>k)k+=ws[i]; ASSERT(0<=k&&k<ws[i],EVINDEX); *v++=k;);
 }
 *ind=z;
 R 1;
}    /* <"1 a to a where a is an integer index array */

static B jtaindex1(J jt,A a,A w,I wf,A*ind){A z;I c,k,n,t,*v,*ws;
 RZ(a&&w);
 n=AN(a); t=AT(a); *ind=0;
 ws=wf+AS(w); c=*(AS(a)+AR(a)-1);
 if(!n||!c||t&BOX)R 0;
 ASSERT(c<=AR(w)-wf,EVLENGTH);
 RZ(z=t&INT?ca(a):cvt(INT,a)); v=AV(z);
 DO(n/c, DO(c, k=*v; if(0>k)*v=k+=ws[i]; ASSERT(0<=k&&k<ws[i],EVINDEX); ++v;););
 *ind=z;
 R 1;
}    /* verify that <"1 a is valid for (<"1 a){w */

static A jtafrom2(J jt,A p,A q,A w,I r){A z;C*wv,*zv;I c,d,e,j,k,m,pn,pr,*pv,
  qn,qr,*qv,*s,wf,wk,wr,*ws,zn;
 wr=AR(w); ws=AS(w); wf=wr-r; wk=bp(AT(w));
 pn=AN(p); pr=AR(p); pv=AV(p);
 qn=AN(q); qr=AR(q); qv=AV(q);
 RE(m=prod(wf,ws)); RE(c=prod(r,ws+wf)); e=ws[1+wf]; d=c?c/(e*ws[wf]):0;
 RE(zn=mult(m,mult(pn,mult(qn,d))));
 GA(z,AT(w),zn,wf+pr+qr+r-2,ws);
 s=AS(z)+wf; ICPY(s,AS(p),pr); 
 s+=pr;      ICPY(s,AS(q),qr);
 s+=qr;      ICPY(s,ws+wf+2,r-2);
 wv=CAV(w); zv=CAV(z); 
 switch(k=d*wk){
  default:        {C*v=wv,*x=zv-k;I n=c*wk;
   DO(m, DO(pn, j=e*pv[i]; DO(qn, MC(x+=k,v+k*(j+qv[i]),k);)); v+=n;); R z;}
#define INNER2(T) {T*v=(T*)wv,*x=(T*)zv;I n=c/d; \
   DO(m, DO(pn, j=e*pv[i]; DO(qn, *x++=v[j+qv[i]];         )); v+=n;); R z;}
  case sizeof(C): INNER2(C);
  case sizeof(S): INNER2(S);
#if SY_64
  case sizeof(int): INNER2(int);
#endif
  case sizeof(I): INNER2(I);
#if !SY_64 && SY_WIN32
  case sizeof(D): INNER2(D);
#endif
}}   /* (<p;q){"r w  for positive integer arrays p,q */

static A jtafi(J jt,I n,A w){A x;
 if(!(AN(w)&&BOX&AT(w)))R pind(n,w);
 ASSERT(!AR(w),EVINDEX);
 x=AAV0(w);
 R AN(x)?less(IX(n),pind(n,x)):ace; 
}

static F2(jtafrom){PROLOG(0073);A c,ind,p=0,q,*v,x,y=w;B b=1,bb=1;I acr,ar,cd,i=0,j,k,m,n,pr,r,*s,t,wcr,wf,wr;
 RZ(a&&w);
 ar=AR(a); acr=  jt->rank?jt->rank[0]:ar;
 wr=AR(w); wcr=r=jt->rank?jt->rank[1]:wr; wf=wr-wcr; jt->rank=0;
 if(ar){
  if(ar==acr&&wr==wcr){RE(aindex(a,w,wf,&ind)); if(ind)R frombu(ind,w,wf);}
  R wr==wcr?rank2ex(a,w,0L,0L,wcr,jtafrom):
      df2(rank1ex(a,0L,acr,jtbox),rank1ex(w,0L,wcr,jtbox),amp(ds(CLBRACE),ds(COPE)));
 }
 c=AAV0(a); t=AT(c); n=IC(c); v=AAV(c); cd=(I)c*ARELATIVE(c); 
 k=bp(AT(w)); s=AS(w)+wr-r;
 ASSERT(1>=AR(c),EVRANK);
 ASSERT(n<=r,EVLENGTH);
 if(n&&!(t&BOX)){RE(aindex(a,w,wf,&ind)); if(ind)R frombu(ind,w,wf);}
 if(r==wr)for(i=m=pr=0;i<n;++i){
  p=afi(s[i],AADR(cd,v[i]));
  if(!(p&&1==AN(p)&&INT&AT(p)))break;
  pr+=AR(p); 
  m+=*AV(p)*prod(r-i-1,1+i+s);
 }
 if(i){I*ys;
  RZ(y=gah(pr+r-i,w)); ys=AS(y); DO(pr, *ys++=1;); ICPY(ys,s+i,r-i);
  AM(y)=AN(y)=prod(AR(y),AS(y));
  AK(y)=k*m+CAV(w)-(C*)y;
 }
 for(;i<n;i+=2){
  j=1+i; if(!p)p=afi(s[i],AADR(cd,v[i])); q=j<n?afi(s[j],AADR(cd,v[j])):ace; if(!(p&&q))break;
  if(p!=ace&&q!=ace){b=0; y=afrom2(p,q,y,r-i);}
  else if(p!=ace)   {b=0; y=irs2(p,y,0L,AR(p),r-i,jtifrom);}
  else if(q!=ace)   {b=0; y=irs2(q,y,0L,AR(q),r-j,jtifrom);}
  p=0;
 }
 RE(y); if(b){RZ(y=ca(x=y)); RZ(y=RELOCATE(x,y));} EPILOG(y);
}    /* a{"r w for boxed index a */

F2(jtfrom){I at;
 RZ(a&&w);
 at=AT(a);
 switch((at&SPARSE?2:0)+(AT(w)&SPARSE?1:0)){
  case 0:  R at&BOX?afrom(a,w)  :at&B01?bfrom(a,w):ifrom(a,w);
  case 1:  R at&BOX?frombs(a,w) :                  fromis(a,w);
  case 2:  R fromsd(a,w);
  default: R fromss(a,w);
}}   /* a{"r w main control */

F2(jtsfrom){A ind;
 RE(aindex1(a,w,0L,&ind));
 R !ind?from(irs1(a,0L,1L,jtbox),w):SPARSE&AT(w)?frombsn(ind,w,0L):frombu(ind,w,0L);
}    /* (<"1 a){w */

static F2(jtmapx){
 RZ(a&&w);
 if(!(BOX&AT(w)))R ope(a);
 R every2(box0(every2(a,box0(catalog(every(shape(w),0L,jtiota))),0L,jtover)),w,0L,jtmapx);
}

F1(jtmap){R mapx(ace,w);}


F2(jtfetch){A*av,t,x=w;I ad,n;
 F2RANK(1,RMAX,jtfetch,0);
 if(!(BOX&AT(a)))RZ(a=box(a));
 n=AN(a); av=AAV(a); ad=(I)a*ARELATIVE(a);
 if(!n)R w;
 DO(n-1, RZ(t=afrom(box(AVR(i)),x)); ASSERT(!AR(t),EVRANK); RZ(x=ope(t)););
 RZ(t=afrom(box(AVR(n-1)),x));
 R !AR(t)&&AT(t)&BOX?ope(t):t;
}

