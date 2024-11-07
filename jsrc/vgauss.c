/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Gaussian Elimination                                             */

#include "j.h"

extern A jtthq1(J jt, Q y);
// w is a rational matrix
DF1(jtgausselm){I c,e,i,j,r,r1,*s;Q p,*u,*v,*x;
 F1RANK(2,jtgausselm,self);
 ASSERT(RAT&AT(w),EVNONCE);
 // This routine modifies w in place.  If w is virtual, that causes an error, because the blocks referred to in
 // w are actually in the backer, and the backer has had ra() applied; so blocks in the backer are going to be freed
 // twice: storing a newly-allocated block will produce a double-free.  So we have to realize any virtual block coming in.
 w=rifvsdebug(w);  // must realize before in-place operations, as above
 ASSERT(2==AR(w),EVRANK);
 s=AS(w); r=s[0]; c=s[1]; r1=MIN(r,c);
 A *old=jt->tnextpushp;
 for(j=0;j<r1;++j){
  v=QAV(w);
  e=-1; u=v+c*j+j; DO(r-j, if(XSGN(u->n)){e=i+j; break;} u+=c;);  /* find pivot row */
  ASSERT(0<=e,EVDOMAIN);
  x=v+c*j; 
  if(j!=e){u=v+c*e; DO(c, Q t1=u[i]; u[i]=x[i]; x[i]=t1;);} /* interchange rows e and j */
  p=x[j]; ra(p.n); ra(p.d); DO(c, Q z=qdiv(x[i],p); CHECKZAP(z.n); CHECKZAP(z.d); INSTALLRAT(w,x,i,z);); fa(p.n); fa(p.d);
  for(i=0;i<r;++i){
   if(i==j)continue;
   u=v+c*i; p=u[j];  /* pivot */
   ra(p.n); ra(p.d); DO(c, Q z=qminus(u[i],qtymes(p,x[i])); INSTALLRAT(w,u,i,z);); fa(p.n); fa(p.d);
  }
  if(!gc3(&w,0L,0L,old))R 0;  // use simple gc3 to ensure all changes use the stack, since w is modified inplace.  Alternatively could turn off inplacing here
 }
 R w;
}    /* Gaussian elimination in place */

static F1(jtdetr){A z;I c,e,g=1,i,j,k,r,*s;Q d,p,*u,*v,*x;
 ARGCHK1(w);
 w=rifvsdebug(w);  // must realize before in-place operations, as above
 s=AS(w); r=s[0]; c=s[1];
 A *old=jt->tnextpushp;
 for(j=0;j<r;++j){
  v=QAV(w); 
  e=-1; u=v+c*j+j; DO(r-j, if(XSGN(u->n)){e=i+j; break;} u+=c;);  /* find pivot row */
  if(0>e)R cvt(RAT,num(0));
  x=v+c*j;
  if(j!=e){u=v+c*e; DO(c, Q t1=u[i]; u[i]=x[i]; x[i]=t1;); g=-g;}  /* interchange rows e and j */
  if(0==XSGN(x[j].d))R mark;
  for(i=j+1;i<r;++i){
   u=v+c*i;
   if(XSGN(u[j].n)){p=qdiv(u[j],x[j]); ra(p.n); ra(p.d); for(k=j+1;k<r;++k){Q z=qminus(u[k],qtymes(p,x[k]));INSTALLRAT(w,u,k,z);} fa(p.n); fa(p.d);}
  }
  if(!gc3(&w,0L,0L,old))R 0;  // use simple gc3 to ensure all changes use the stack, since w is modified inplace.  Alternatively could turn off inplacing here
 }
 d=0<g?*v:qminus(Q0,*v); u=v+1+c; DQ(r-1, d=qtymes(d,*u); u+=1+c;);
 RE(0);
 GAT0(z,RAT,1,0); QAV0(z)[0]=d; R z;
}    /* determinant on rational matrix; works in place */

static F1(jtdetd){D g,h,p,q,*u,*v,*x,*y,z=1.0;I c,d,e,i,j,k,r,*s;
 ARGCHK1(w);
 s=AS(w); r=s[0]; c=s[1]; v=DAV(w);
 NAN0;
 for(j=0;j<r;++j){
  x=v+c*j; u=x+j; h=0.0; 
  DO(r-j, k=i; DO(c-j, g=ABS(*u); if(h<g){h=g; d=j+k; e=j+i;} ++u;); u+=j;);  /* find pivot, maximum abs element */
  if(h==inf)R mark;
  if(0==h)R scf(0.0);
  if(j!=d){u=v+c*d+j; y=x+j; DQ(c-j, q=*u; *u=*y; *y=q; ++u;  ++y; ); z=-z;}  /* interchange rows j and d */
  if(j!=e){u=x+e;     y=x+j; DQ(r-j, q=*u; *u=*y; *y=q; u+=c; y+=c;); z=-z;}  /* interchange cols j and e */
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
 ARGCHK1(w);
 z.re=1.0; z.im=0.0;
 s=AS(w); r=s[0]; c=s[1]; v=ZAV(w);
 NAN0;
 for(j=0;j<r;++j){
  x=v+c*j; u=x+j; h=0.0; 
  DO(r-j, k=i; DO(c-j, g=ZABT(*u); if(h<g){h=g; d=j+k; e=j+i;} ++u;); u+=j;);  /* find pivot, maximum abs element */
  if(h==inf)R mark;
  if(0==h)R scf(0.0);
  if(j!=d){u=v+c*d;              DO(c-j, q=u[j+i]; u[j+i]=x[j+i]; x[j+i]=q;); z=zminus(zeroZ,z);}  /* interchange rows j and d */
  if(j!=e){u=v+c*j+e; y=v+c*j+j; DQ(r-j, q=*u; *u=*y; *y=q; u+=c; y+=c;);     z=zminus(zeroZ,z);}  /* interchange cols j and e */
  q=x[j]; z=ztymes(z,q);
  for(i=j+1;i<r;++i){
   u=v+c*i;
   if(ZNZ(u[j])){p=zdiv(u[j],q); for(k=j+1;k<r;++k)u[k]=zminus(u[k],ztymes(p,x[k]));}
 }}
 NAN1; RE(0);
 GAT0(t,CMPX,1,0); ZAV0(t)[0]=z; R t;
}    /* determinant on complex  matrix; works in place */

F1(jtgaussdet){A z;I*s;
 ARGCHK1(w);
 ASSERT(2==AR(w),EVRANK);
 s=AS(w);
 ASSERT(s[0]==s[1],EVLENGTH);
 if(!ISSPARSE(AT(w)))
  switch(CTTZNOFLAG(AT(w))){
  case FLX:   z=detd(ca(w));      break;
  default:   ASSERT(0,EVDOMAIN);
  case B01X:
  case INTX:  R detd(ccvt(FL,w,0));
  case CMPXX: z=detz(ca(w));      break;
  case XNUMX: z=detr(cvt(RAT,w)); break;
  case RATX:  z=detr(ca(w));
  }
 R z==mark?detxm(w,eval("-/ .*")):z;
}    /* determinant on square matrix */
