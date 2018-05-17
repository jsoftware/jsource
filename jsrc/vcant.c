/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Transpose                                                        */

#include "j.h"

static F2(jtcanta);

static A jtcants(J jt,A a,A w,A z){A a1,q,y;B*b,*c;I*u,wr,zr;P*wp,*zp;
 RZ(a&&w&&z);
 RZ(a=grade1(a));
 wr=AR(w); wp=PAV(w); a1=SPA(wp,a);
 zr=AR(z); zp=PAV(z);
 ASSERT(wr==zr,EVNONCE);
 RZ(b=bfi(wr,a1,1));
 GATV(q,B01,wr,1,0); c=BAV(q); u=AV(a); DO(wr, c[i]=b[u[i]];);
 SPB(zp,a,ifb(wr,c));
 SPB(zp,e,ca(SPA(wp,e)));
 RZ(y=fromr(grade1(indexof(a,a1)),SPA(wp,i)));
 RZ(q=grade1(y));
 SPB(zp,i,from(q,y));
 SPB(zp,x,from(q,canta(over(zero,increm(grade1(less(a,a1)))),SPA(wp,x))));
 R z;
}    /* w is sparse */

#if 0
#define CANTA(T,exp)  \
 {T*u=(T*)zv,*v=(T*)wv;                                                  \
  DO(zn, exp; j=r-1; ++tv[j]; d+=mv[j];                                  \
  while(j&&sv[j]==tv[j]){d+=mv[j-1]-mv[j]*sv[j]; tv[j]=0; ++tv[--j];});  \
 }
#else
// do the innermost loop fast.  r must be >= 2 (no way for it to be 1)
#define CANTA(T,exp)  \
 {T*u=(T*)zv,*v=(T*)wv;                                                  \
  do{j = r-1; I mvr1=mv[j]; DQ(sv[j], exp; d+=mvr1;)                        \
   do{d-=mv[j]*sv[j]; tv[j]=0; --j; if(j<0)break; d+=mv[j]; ++tv[j];}while(sv[j]==tv[j]);  \
  }while(j>=0); \
 }
#endif

static F2(jtcanta){A m,s,t,z;B b;C*wv,*zv;I*av,c,d,j,k,*mv,r,*sv,*tv,wf,wr,*ws,zn,zr;
 RZ(a&&w);
 av=AV(a); ws=AS(w); wr=AR(w); r=jt->rank?jt->rank[1]:wr; jt->rank=0;
 ASSERT(r==AN(a),EVLENGTH);
 if(wf=wr-r){
  GATV(a,INT,wr,1,0); tv=AV(a); 
  DO(wf, tv[i]=i;); DO(r, tv[wf+i]=wf+av[i];);
  av=tv;
 }
 zr=0; DO(wr, zr=MAX(zr,av[i]);); zr+=0<wr;
 GATV(m,INT,zr,1,0); mv=AV(m);
 GATV(s,INT,zr,1,0); sv=AV(s);
 GATV(t,INT,wr,1,0); tv=AV(t);
 d=1; j=wr; DO(wr, --j; tv[j]=d; d*=ws[j];);
 for(j=0,zn=1;j<zr;++j){
  c=IMAX; d=k=0;
  DO(wr, if(j==av[i]){k=1; d+=tv[i]; c=MIN(c,ws[i]);});
  ASSERT(k,EVINDEX);
  zn*=c; sv[j]=c; mv[j]=d;
 }
 b=1&&SPARSE&AT(w);
 GA(z,AT(w),b?1:zn,zr,sv);
 if(b)R cants(a,w,z); if(!zn)R z;  
 d=1; r=zr; j=wr; DO(wr, --j; if(j!=av[j])break; d*=sv[j]; --r;);  // collect trailing unmodified axes into the cell-size
 if(1<d)DO(r, mv[i]/=d;);  // if cell-size has increased, reduce movement vector accordingly, since it counts in cells (questionable decision)
 k=bp(AT(w)); zv=CAV(z); wv=CAV(w);  // k = size of atom, in bytes
 if(r){
  k *= d;  // repurpose k to be the size of a cell, in bytes
  d=0;  // inside the loop d will hold the input pointer
  memset(tv,C0,r*SZI);  // clear initial rep of the input pointer
  switch(k){
  case sizeof(C): CANTA(C, *u++=v[d];); break;
  case sizeof(S): CANTA(S, *u++=v[d];); break;
#if SY_64
  case sizeof(int): CANTA(int, *u++=v[d];); break;
#endif
  case sizeof(I): CANTA(I, *u++=v[d];); break;
#if !SY_64 && SY_WIN32
  case sizeof(D): if(AT(w)&FL){CANTA(D, *u++=v[d];); break;}
    // move as D type only if echt floats - otherwise they get corrupted.  If not float, fall through to...
#endif
  default:        CANTA(C, MC(u,v+d*k,k); u+=k;); break;
  }     
 }else{MC(zv,wv,k*zn);}  // could return w
 RELOCATE(w,z); RETF(z);  // should EPILOG?
}    /* dyadic transpose in APL\360, a f"(1,r) w where 1>:#$a  */

F1(jtcant1){I r; 
 RZ(w); 
 if(jt->rank){jt->rank[0]=1; r=jt->rank[1];}else r=AR(w); 
 A z=canta(apv(r,r-1,-1L),w);
 RZ(z);  INHERITNOREL(z,w); RETF(z);
}    /* |:"r w */

F2(jtcant2){A*av,p,t,y;I j,k,m,n,*pv,q,r,*v;
 RZ(a&&w);
 q=jt->rank?jt->rank[0]:AR(a); 
 r=jt->rank?jt->rank[1]:AR(w); jt->rank=0;
 if(1<q||q<AR(a))R rank2ex(a,w,0L,1,RMAX,q,r,jtcant2);
 if(BOX&AT(a)){
  RZ(y=pfill(r,t=raze(a))); v=AV(y);
  GATV(p,INT,AN(y),1,0); pv=AV(p);
  m=AN(a); n=AN(t); av=AAV(a); RELBASEASGN(a,a);
  j=0; DO(r-n,pv[*v++]=j++;); DO(m, k=AN(AVR(i)); DO(k,pv[*v++]=j;); if(k)++j;);
 }else p=pinv(pfill(r,a));
 A z= r<AR(w) ? irs2(p,w,0L,1L,r,jtcanta) : canta(p,w);
 RZ(z);  INHERITNOREL(z,w); RETF(z);
}    /* a|:"r w main control */ 
