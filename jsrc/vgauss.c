/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Gaussian Elimination                                             */

#include "j.h"

                    
F1(jtgausselm){A t;C*tv;I c,e,i,j,m,old,r,r1,*s;Q p,*u,*v,*x;
 F1RANK(2,jtgausselm,0);
 ASSERT(RAT&AT(w),EVNONCE);
 ASSERT(2==AR(w),EVRANK);
 s=AS(w); r=s[0]; c=s[1]; r1=MIN(r,c); v=QAV(w); m=c*bp(AT(w));
 GATV(t,LIT,m,1,0); tv=CAV(t);
 old=jt->tnextpushx;
 for(j=0;j<r1;++j){
  e=-1; u=v+c*j+j; DO(r-j, if(XDIG(u->n)){e=i+j; break;} u+=c;);  /* find pivot row */
  ASSERT(0<=e,EVDOMAIN);
  x=v+c*j; 
  if(j!=e){u=v+c*e; MC(tv,x,m); MC(x,u,m); MC(u,tv,m);} /* interchange rows e and j */
  p=x[j]; DO(c, x[i]=qdiv(x[i],p););
  for(i=0;i<r;++i){
   if(i==j)continue;
   u=v+c*i; p=u[j];  /* pivot */
   DO(c, u[i]=qminus(u[i],qtymes(p,x[i])););
  }
  gc(w,old);
 }
 R w;
}    /* Gaussian elimination in place */

static F1(jtdetr){A t,z;C*tv;I c,e,g=1,i,j,k,m,old,r,*s;Q d,p,*u,*v,*x;
 RZ(w);
 s=AS(w); r=s[0]; c=s[1];
 v=QAV(w); 
 m=c*sizeof(Q); GATV(t,LIT,m,1,0); tv=CAV(t);
 old=jt->tnextpushx;
 for(j=0;j<r;++j){
  e=-1; u=v+c*j+j; DO(r-j, if(XDIG(u->n)){e=i+j; break;} u+=c;);  /* find pivot row */
  if(0>e)R cvt(RAT,zero);
  x=v+c*j;
  if(j!=e){u=v+c*e; MC(tv,x,m); MC(x,u,m); MC(u,tv,m); g=-g;}  /* interchange rows e and j */
  i=XDIG(x[j].n); if(i==XPINF||i==XNINF)R mark;
  for(i=j+1;i<r;++i){
   u=v+c*i;
   if(XDIG(u[j].n)){p=qdiv(u[j],x[j]); for(k=j+1;k<r;++k)u[k]=qminus(u[k],qtymes(p,x[k]));}
  }
  gc(w,old);
 }
 d=0<g?*v:qminus(zeroQ,*v); u=v+1+c; DO(r-1, d=qtymes(d,*u); u+=1+c;);
 RE(0);
 GAT(z,RAT,1,0,0); *QAV(z)=d; R z;
}    /* determinant on rational matrix; works in place */

static F1(jtdetd){D g,h,p,q,*u,*v,*x,*y,z=1.0;I c,d,e,i,j,k,r,*s;
 RZ(w);
 s=AS(w); r=s[0]; c=s[1]; v=DAV(w);
 NAN0;
 for(j=0;j<r;++j){
  x=v+c*j; u=x+j; h=0.0; 
  DO(r-j, k=i; DO(c-j, g=ABS(*u); if(h<g){h=g; d=j+k; e=j+i;} ++u;); u+=j;);  /* find pivot, maximum abs element */
  if(h==inf)R mark;
  if(0==h)R scf(0.0);
  if(j!=d){u=v+c*d+j; y=x+j; DO(c-j, q=*u; *u=*y; *y=q; ++u;  ++y; ); z=-z;}  /* interchange rows j and d */
  if(j!=e){u=x+e;     y=x+j; DO(r-j, q=*u; *u=*y; *y=q; u+=c; y+=c;); z=-z;}  /* interchange cols j and e */
  q=x[j]; z*=q; JBREAK0;
  for(i=j+1;i<r;++i){
   u=v+c*i;
   if(u[j]){p=u[j]/q; for(k=j+1;k<r;++k)u[k]-=p*x[k];}
 }}
 NAN1;
 R scf(z);
}    /* determinant on real     matrix; works in place */

#define ZABT(v)         ((v).re*(v).re+(v).im*(v).im)

static F1(jtdetz){A t;D g,h;I c,d,e,i,j,k,r,*s;Z p,q,*u,*v,*x,*y,z;
 RZ(w);
 z.re=1.0; z.im=0.0;
 s=AS(w); r=s[0]; c=s[1]; v=ZAV(w);
 NAN0;
 for(j=0;j<r;++j){
  x=v+c*j; u=x+j; h=0.0; 
  DO(r-j, k=i; DO(c-j, g=ZABT(*u); if(h<g){h=g; d=j+k; e=j+i;} ++u;); u+=j;);  /* find pivot, maximum abs element */
  if(h==inf)R mark;
  if(0==h)R scf(0.0);
  if(j!=d){u=v+c*d;              DO(c-j, q=u[j+i]; u[j+i]=x[j+i]; x[j+i]=q;); z=zminus(zeroZ,z);}  /* interchange rows j and d */
  if(j!=e){u=v+c*j+e; y=v+c*j+j; DO(r-j, q=*u; *u=*y; *y=q; u+=c; y+=c;);     z=zminus(zeroZ,z);}  /* interchange cols j and e */
  q=x[j]; z=ztymes(z,q);
  for(i=j+1;i<r;++i){
   u=v+c*i;
   if(ZNZ(u[j])){p=zdiv(u[j],q); for(k=j+1;k<r;++k)u[k]=zminus(u[k],ztymes(p,x[k]));}
 }}
 NAN1; RE(0);
 GAT(t,CMPX,1,0,0); *ZAV(t)=z; R t;
}    /* determinant on complex  matrix; works in place */

F1(jtgaussdet){A z;I*s;
 RZ(w);
 ASSERT(2==AR(w),EVRANK);
 s=AS(w);
 ASSERT(s[0]==s[1],EVLENGTH);
 switch(CTTZNOFLAG(AT(w))){
  default:   ASSERT(0,EVDOMAIN);
  case B01X:
  case INTX:  R detd(cvt(FL,w));
  case FLX:   z=detd(ca(w));      break;
  case CMPXX: z=detz(ca(w));      break;
  case XNUMX: z=detr(cvt(RAT,w)); break;
  case RATX:  z=detr(ca(w));
 }
 R z==mark?detxm(w,eval("-/ .*")):z;
}    /* determinant on square matrix */
