/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Interpreter Utilities                                                   */

#include "j.h"


#if SY_64

I jtmult(J jt,I x,I y){B neg;I a,b,c,p,q,qs,r,s,z;static I m=0x00000000ffffffff;
 if(!x||!y)R 0;
 neg=0>x!=0>y;
 if(0>x){x=-x; ASSERT(0<x,EVLIMIT);} p=m&(x>>32); q=m&x;
 if(0>y){y=-y; ASSERT(0<y,EVLIMIT);} r=m&(y>>32); s=m&y;
 ASSERT(!(p&&r),EVLIMIT);
 a=p*s+q*r; qs=q*s; b=m&(qs>>32); c=m&qs;
 ASSERT(2147483648>a+b,EVLIMIT);
 z=c+((a+b)<<32);
 R neg?-z:z;
}

I jtprod(J jt,I n,I*v){I*u,z;
 if(1>n)R 1;
 u=v; DO(n, if(!*u++)R 0;); 
 z=*v++; DO(n-1, z=mult(z,*v++);); 
 R z;
}

#else

I jtmult(J jt,I x,I y){D z=x*(D)y; ASSERT(z<=IMAX,EVLIMIT); R(I)z;}

I jtprod(J jt,I n,I*v){D z=1; DO(n, z*=(D)v[i];); ASSERT(z<=IMAX,EVLIMIT); R(I)z;}

#endif

B all0(A w){RZ(w); R !memchr(AV(w),C1,AN(w));}

B all1(A w){RZ(w); R !memchr(AV(w),C0,AN(w));}

I jtaii(J jt,A w){I m=IC(w); R m&&!(SPARSE&AT(w))?AN(w)/m:prod(AR(w)-1,1+AS(w));}

A jtapv(J jt,I n,I b,I m){A z;I j=b-m,p=b+m*(n-1),*x;
 GA(z,INT,n,1,0); x=AV(z);
 switch(m){
  case  0: DO(n, *x++=b;);      break;
  case -1: while(j!=p)*x++=--j; break;
  case  1: while(j!=p)*x++=++j; break;
  default: while(j!=p)*x++=j+=m;
 }
 R z;
}    /* b+m*i.n */

B jtb0(J jt,A w){RZ(w); ASSERT(!AR(w),EVRANK); if(!(B01&AT(w)))RZ(w=cvt(B01,w)); R*BAV(w);}

B*jtbfi(J jt,I n,A w,B p){A t;B*b;I*v;
 GA(t,B01,n,1,0); b=BAV(t);
 memset(b,!p,n); v=AV(w); DO(AN(w), b[v[i]]=p;);
 R b;
}    /* boolean mask from integers: p=(i.n)e.w */

I bp(I t){
 switch(t){
  case B01:  R sizeof(B);
  case LIT:  case ASGN: case NAME: 
             R sizeof(C);
  case C2T:  R sizeof(C2);
  case INT:  case LPAR: case RPAR: case MARK: case SYMB:
             R sizeof(I);
  case FL:   R sizeof(D);
  case CMPX: R sizeof(Z);
  case BOX:  R sizeof(A);
  case XNUM: R sizeof(X);
  case RAT:  R sizeof(Q);
  case SB01: case SINT: case SFL:  case SCMPX: case SLIT: case SBOX: 
             R sizeof(P);
  case VERB: case ADV:  case CONJ: 
             R sizeof(V);
  case CONW: R sizeof(CW);
  case SBT:  R sizeof(SB);
#ifdef UNDER_CE
  default:   R t&XD?sizeof(DX):t&XZ?sizeof(ZX):-1;
#else
  case XD:   R sizeof(DX);
  case XZ:   R sizeof(ZX);
  default:   R -1;
#endif
}}

I bsum(I n,B*b){I q,z=0;UC*u;UI t,*v;
 v=(UI*)b; u=(UC*)&t; q=n/(255*SZI);
#if SY_64
 DO(q, t=0; DO(255, t+=*v++;); z+=u[0]+u[1]+u[2]+u[3]+u[4]+u[5]+u[6]+u[7];);
#else
 DO(q, t=0; DO(255, t+=*v++;); z+=u[0]+u[1]+u[2]+u[3];);
#endif
 u=(UC*)v; DO(n-q*255*SZI, z+=*u++;);
 R z;
}    /* sum of boolean vector b */

C cf(A w){RZ(w); R*CAV(w);}

C cl(A w){RZ(w); R*(CAV(w)+AN(w)-1);}

I jtcoerce2(J jt,A*a,A*w,I mt){I at,at1,t,wt,wt1;
 RZ(*a&&*w);
 at=AT(*a); at1=AN(*a)?at:0;
 wt=AT(*w); wt1=AN(*w)?wt:0; RE(t=maxtype(at1,wt1)); RE(t=maxtype(t,mt));
 if(!t)RE(t=maxtype(at,wt));
 if(t!=at)RZ(*a=cvt(t,*a));
 if(t!=wt)RZ(*w=cvt(t,*w));
 R t;
}

A jtcstr(J jt,C*s){R str((I)strlen(s),s);}

B evoke(A w){V*v=VAV(w); R CTILDE==v->id&&v->f&&NAME&AT(v->f);}

I jti0(J jt,A w){RZ(w=vi(w)); ASSERT(!AR(w),EVRANK); R*AV(w);}

A jtifb(J jt,I n,B*b){A z;I m,*zv; 
 m=bsum(n,b); 
 if(m==n)R IX(n);
 GA(z,INT,m,1,0); zv=AV(z);
#if !SY_64 && SY_WIN32
 {I i,q=SZI*(n/SZI),*u=(I*)b;
  for(i=0;i<q;i+=SZI)switch(*u++){
    case B0001:                                *zv++=i+3; break;
    case B0010:                     *zv++=i+2;            break;
    case B0011:                     *zv++=i+2; *zv++=i+3; break;
    case B0100:          *zv++=i+1;                       break;
    case B0101:          *zv++=i+1;            *zv++=i+3; break;
    case B0110:          *zv++=i+1; *zv++=i+2;            break;
    case B0111:          *zv++=i+1; *zv++=i+2; *zv++=i+3; break;
    case B1000: *zv++=i;                                  break;
    case B1001: *zv++=i;                       *zv++=i+3; break;
    case B1010: *zv++=i;            *zv++=i+2;            break;
    case B1011: *zv++=i;            *zv++=i+2; *zv++=i+3; break;
    case B1100: *zv++=i; *zv++=i+1;                       break;
    case B1101: *zv++=i; *zv++=i+1;            *zv++=i+3; break;
    case B1110: *zv++=i; *zv++=i+1; *zv++=i+2;            break;
    case B1111: *zv++=i; *zv++=i+1; *zv++=i+2; *zv++=i+3;
  }
  b=(B*)u; DO(n%SZI, if(*b++)*zv++=q+i;);
 }
#else
 DO(n, if(b[i])*zv++=i;);
#endif
 R z;
}    /* integer vector from boolean mask */

F1(jtii){RZ(w); R IX(IC(w));}

I jtmaxtype(J jt,I s,I t){I u;
 u=s|t;
 if(!(u&SPARSE))R u&CMPX?CMPX:u&FL?FL:s<t?t:s;
 if(s){s=s&SPARSE?s:STYPE(s); ASSERT(s,EVDOMAIN);}
 if(t){t=t&SPARSE?t:STYPE(t); ASSERT(t,EVDOMAIN);}
 R s<t?t:s;
}

void mvc(I m,void*z,I n,void*w){I p=n,r;static I k=sizeof(D);
 MC(z,w,MIN(p,m)); while(m>p){r=m-p; MC(p+(C*)z,z,MIN(p,r)); p+=p;}
}

/* // faster but on some compilers runs afoul of things that look like NaNs 
   // exponent bytes are silently changed by one bit
void mvc(I m,void*z,I n,void*w){I p=n,r;static I k=sizeof(D);
 if(m<k||k<n||(I)z%k){MC(z,w,MIN(p,m)); while(m>p){r=m-p; MC(p+(C*)z,z,MIN(p,r)); p+=p;}}
 else{C*e,*s;D d[7],d0,*v;
  p=0==k%n?8:6==n?24:n*k;  // p=lcm(k,n)
  e=(C*)d; s=w; DO(p, *e++=s[i%n];);
  v=(D*)z; d0=*d;
  switch(p){
   case  8: DO(m/p, *v++=d0;); break;
   case 24: DO(m/p, *v++=d0; *v++=d[1]; *v++=d[2];); break;
   case 40: DO(m/p, *v++=d0; *v++=d[1]; *v++=d[2]; *v++=d[3]; *v++=d[4];); break;
   case 56: DO(m/p, *v++=d0; *v++=d[1]; *v++=d[2]; *v++=d[3]; *v++=d[4]; *v++=d[5]; *v++=d[6];);
  }
  if(r=m%p){s=(C*)v; e=(C*)d; DO(r, *s++=e[i];);}
}}
*/

A jtodom(J jt,I r,I n,I*s){A q,z;I j,k,m,*u,*zv;
 RE(m=prod(n,s)); k=n*SZI;
 GA(z,INT,m*n,2==r?2:n,s); zv=AV(z)-n;
 if(2==r){u=AS(z); u[0]=m; u[1]=n;}
 if(!(m&&n))R z;
 if(1==n)DO(m, *++zv=i;)
 else{
  GA(q,INT,n,1,0); u=AV(q); memset(u,C0,k); u[n-1]=-1;
  DO(m, ++u[j=n-1]; DO(n, if(u[j]<s[j])break; u[j]=0; ++u[--j];); MC(zv+=n,u,k););
 }
 R z;
}

F1(jtrankle){R!w||AR(w)?w:ravel(w);}

A jtsc(J jt,I k)     {A z; GA(z,INT, 1,0,0); *IAV(z)=k;     R z;}
A jtsc4(J jt,I t,I v){A z; GA(z,t,   1,0,0); *IAV(z)=v;     R z;}
A jtscb(J jt,B b)    {A z; GA(z,B01, 1,0,0); *BAV(z)=b;     R z;}
A jtscc(J jt,C c)    {A z; GA(z,LIT, 1,0,0); *CAV(z)=c;     R z;}
A jtscf(J jt,D x)    {A z; GA(z,FL,  1,0,0); *DAV(z)=x;     R z;}
A jtscx(J jt,X x)    {A z; GA(z,XNUM,1,0,0); *XAV(z)=ca(x); R z;}

A jtstr(J jt,I n,C*s){A z; GA(z,LIT,n,1,0); MC(AV(z),s,n); R z;}

F1(jtstr0){A z;C*x;I n; RZ(w); n=AN(w); GA(z,LIT,1+n,1,0); x=CAV(z); MC(x,AV(w),n); x[n]=0; R z;}

A jtv2(J jt,I a,I b){A z;I*x; GA(z,INT,2,1,0); x=AV(z); *x++=a; *x=b; R z;}

A jtvci(J jt,I k){A z; GA(z,INT,1,1,0); *IAV(z)=k; R z;}

A jtvec(J jt,I t,I n,void*v){A z; GA(z,t,n,1,0); MC(AV(z),v,n*bp(t)); R z;}

F1(jtvi){RZ(w); R INT&AT(w)?w:cvt(INT,w);}

F1(jtvib){A z;D d,e,*wv;I i,n,*old,p=-IMAX,q=IMAX,*zv;
 RZ(w);
 old=jt->rank; jt->rank=0;
 if(AT(w)&SPARSE)RZ(w=denseit(w));
 switch(AT(w)){
  case INT:  z=w; break;
  case B01:  z=cvt(INT,w); break;
  case XNUM:
  case RAT:  z=cvt(INT,maximum(sc(p),minimum(sc(q),w))); break;
  default:
   if(!(AT(w)&FL))RZ(w=cvt(FL,w));
   n=AN(w); wv=DAV(w);
   GA(z,INT,n,AR(w),AS(w)); zv=AV(z);
   for(i=0;i<n;++i){
    d=wv[i]; e=jfloor(d);
    if     (d==inf )     zv[i]=q;
    else if(d==infm)     zv[i]=p;
    else if(    FEQ(d,e))zv[i]=d<p?p:q<d?q:(I)e;
    else if(++e,FEQ(d,e))zv[i]=d<p?p:q<d?q:(I)e;
    else ASSERT(0,EVDOMAIN);
 }}
 jt->rank=old; R z;
}

F1(jtvip){I*v; RZ(w); if(!(INT&AT(w)))RZ(w=cvt(INT,w)); v=AV(w); DO(AN(w), ASSERT(0<=*v++,EVDOMAIN);); R w;}

F1(jtvs){RZ(w); ASSERT(1>=AR(w),EVRANK); R LIT&AT(w)?w:cvt(LIT,w);}    
     /* verify string */
