/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Match Associates                                                 */

#include "j.h"
#include "vcomp.h"


static B jtmatchsub(J,I,I,I,I,A,A,B*,B,B);
static F2(jtmatchs);

#define MCS(q,af,wf)  ((1<q?16:q?8:0)+(af?2:0)+(wf?1:0))
#define QLOOP         b=b1; DO(q, if(u[i]!=v[i]){b=b0; break;}); *x++=b;
#define EQV(T)        \
 {T h,*u=(T*)av,*v=(T*)wv;                                                   \
  q=k/sizeof(T);                                                             \
  switch(MCS(q,af,wf)){                                                      \
   case MCS(1,0,0):              *x++=*u  ==*v?b1:b0; break;                 \
   case MCS(1,0,1): h=*u; if(b1)DO(mn, *x++=h   ==*v++;) else DO(mn, *x++=h   !=*v++;)  break;  \
   case MCS(1,1,0): h=*v; if(b1)DO(mn, *x++=*u++==h;   ) else DO(mn, *x++=*u++!=h;   ); break;  \
   case MCS(1,1,1): if(b1){                                                  \
                     if(1==n)      DO(m,               *x++=*u++==*v++;  )   \
                     else if(af<wf)DO(m, h=*u++; DO(n, *x++=h   ==*v++;);)   \
                     else          DO(m, h=*v++; DO(n, *x++=*u++==h;   ););  \
                    }else{                                                   \
                     if(1==n)      DO(m,               *x++=*u++!=*v++;  )   \
                     else if(af<wf)DO(m, h=*u++; DO(n, *x++=h   !=*v++;);)   \
                     else          DO(m, h=*v++; DO(n, *x++=*u++!=h;   ););  \
                    } break;                                                 \
   case MCS(2,0,0):        QLOOP;               break;                       \
   case MCS(2,0,1): DO(mn, QLOOP;       v+=q;); break;                       \
   case MCS(2,1,0): DO(mn, QLOOP; u+=q;      ); break;                       \
   case MCS(2,1,1): if(1==n)      DO(m,       QLOOP; u+=q;   v+=q;)          \
                    else if(af<wf)DO(m, DO(n, QLOOP; v+=q;); u+=q;)          \
                    else          DO(m, DO(n, QLOOP; u+=q;); v+=q;); break;  \
 }}

static B eqv(I af,I wf,I m,I n,I k,C*av,C*wv,B*x,B b0,B b1){B b,*xx=x;I c,d,mn=m*n,q;
 if     (0==k%sizeof(I)  )EQV(I)
#if SY_64
 else if(0==k%sizeof(int))EQV(int)
#endif
 else if(0==k%sizeof(S)  )EQV(S)
 else if(1==k)            EQV(C)
 else{
  c=af?k:0; d=wf?k:0;
  if(1==n)      DO(m,       *x++=!!memcmp(av,wv,k)?b0:b1; av+=c;   wv+=d;)
  else if(af<wf)DO(m, DO(n, *x++=!!memcmp(av,wv,k)?b0:b1; wv+=k;); av+=k;)
  else          DO(m, DO(n, *x++=!!memcmp(av,wv,k)?b0:b1; av+=k;); wv+=k;);
 }
 R mn?xx[mn-1]:b1;
}    /* what memcmp should have been */

// Return 1 if a and w match, 0 if not
B jtequ(J jt,A a,A w){A x;B b; 
 RZ(a&&w);F2PREFIP;  // allow inplace request - it has no effect
 if(a==w)R 1;
 if(SPARSE&(AT(a)|AT(w))&&AR(a)&&AR(w)){RZ(x=matchs(a,w)); R*BAV(x);}
 R level(a)==level(w)&&matchsub(0L,0L,1L,1L,a,w,&b,C0,C1);
}

static B jteqf(J jt,A a,A w){A p,q;B b;V*u=VAV(a),*v=VAV(w);
 if(!(TYPESEQ(AT(a),AT(w))&&u->id==v->id))R 0;
 p=u->f; q=v->f; if(!(!p&&!q||p&&q&&matchsub(0L,0L,1L,1L,p,q,&b,C0,C1)))R 0;
 p=u->g; q=v->g; if(!(!p&&!q||p&&q&&matchsub(0L,0L,1L,1L,p,q,&b,C0,C1)))R 0;
 p=u->h; q=v->h;    R !p&&!q||p&&q&&matchsub(0L,0L,1L,1L,p,q,&b,C0,C1);
}

#define EQA(a,w)  matchsub(0L,0L,1L,1L,a,w,&b,C0,C1)
#define EQQ(a,w)  (equ(a.n,w.n)&&equ(a.d,w.d))

#define INNERT(T,f)                  \
 {T* RESTRICT u=(T*)av,* RESTRICT v=(T*)wv;              \
  if(1==n)      DO(m,       x[j]=b1; DO(c, if(!f(u[i],v[i])){x[j]=b0; break;}); u+=p; v+=q; ++j;  )  \
  else if(af>wf)DO(m, DO(n, x[j]=b1; DO(c, if(!f(u[i],v[i])){x[j]=b0; break;}); u+=p; ++j;); v+=q;)  \
  else          DO(m, DO(n, x[j]=b1; DO(c, if(!f(u[i],v[i])){x[j]=b0; break;}); v+=q; ++j;); u+=p;)  \
 }

#define INNERT2(aa,ww,f)             \
 {A1* RESTRICT u=(A1*)av,* RESTRICT v=(A1*)wv;           \
  if(1==n)      DO(m,       x[j]=b1; DO(c, if(!f((A)AABS(u[i],aa),(A)AABS(v[i],ww))){x[j]=b0; break;}); u+=p; v+=q; ++j;  )  \
  else if(af>wf)DO(m, DO(n, x[j]=b1; DO(c, if(!f((A)AABS(u[i],aa),(A)AABS(v[i],ww))){x[j]=b0; break;}); u+=p; ++j;); v+=q;)  \
  else          DO(m, DO(n, x[j]=b1; DO(c, if(!f((A)AABS(u[i],aa),(A)AABS(v[i],ww))){x[j]=b0; break;}); v+=q; ++j;); u+=p;)  \
 }

static B jtmatchsub(J jt,I af,I wf,I m,I n,A a,A w,B*x,B b0,B b1){B b,c0;C*av,*wv;I at,c,j=0,mn,p,q,t,wt;
 p=AR(a)-af; at=UNSAFE(AT(a)); mn=m&&n?m*n:0;
 q=AR(w)-wf; wt=UNSAFE(AT(w)); RE(t=maxtype(at,wt)); c0=!jt->ct&&t&FL+CMPX;
 c=(af>wf?AN(a):AN(w))/(mn?mn:1); b=p!=q||ICMP(af+AS(a),wf+AS(w),p)||c&&!HOMO(at,wt);
 if(b||!c||a==w){memset(x,b?b0:b1,mn); R b?b0:b1;}
 if(t&FUNC)R eqf(a,w)?b1:b0;
 if(TYPESNE(t,at))RZ(a=t&XNUM?xcvt(XMEXMT,a):cvt(t,a)) else if(c0)RZ(a=cvt0(a)); 
 if(TYPESNE(t,wt))RZ(w=t&XNUM?xcvt(XMEXMT,w):cvt(t,w)) else if(c0)RZ(w=cvt0(w)); 
 p=af?c:0; av=CAV(a);
 q=wf?c:0; wv=CAV(w);
 switch(c0?0:CTTZ(t)){
  default:   R eqv(af,wf,m,n,c*bp(t),av,wv,x,b0,b1);
  case FLX:   INNERT(D,TEQ);    R mn?x[mn-1]:b1;
  case CMPXX: INNERT(Z,zeq);    R mn?x[mn-1]:b1;
  case XNUMX: INNERT(X,equ);    R mn?x[mn-1]:b1;
  case RATX:  INNERT(Q,EQQ);    R mn?x[mn-1]:b1;
  case BOXX:  switch(2*ARELATIVE(a)+ARELATIVE(w)){
   default:  INNERT(A,EQA);    R mn?x[mn-1]:b1;
   case 1:   INNERT2(0,w,EQA); R mn?x[mn-1]:b1;
   case 2:   INNERT2(a,0,EQA); R mn?x[mn-1]:b1;
   case 3:   INNERT2(a,w,EQA); R mn?x[mn-1]:b1;
}}}

static F2(jtmatchs){A ae,ax,p,q,we,wx,x;B*b,*pv,*qv;D d;I acr,an=0,ar,c,j,k,m,n,r,*s,*v,wcr,wn=0,wr;P*ap,*wp;
 RZ(a&&w);
 ar=AR(a); acr=jt->rank?jt->rank[0]:ar; r=ar;
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; jt->rank=0;
 if(ar>acr||wr>wcr)R rank2ex(a,w,0L,acr,wcr,jtmatchs);
 if(ar!=wr||memcmp(AS(a),AS(w),r*SZI)||!HOMO(AT(a),AT(w)))R zero;
 GATV(x,B01,r,1L,0L); b=BAV(x); memset(b,C0,r);
 if(SPARSE&AT(a)){ap=PAV(a); x=SPA(ap,a); v=AV(x); an=AN(x); DO(an, b[v[i]]=1;);}
 if(SPARSE&AT(w)){wp=PAV(w); x=SPA(wp,a); v=AV(x); wn=AN(x); DO(wn, b[v[i]]=1;);} 
 c=0; DO(r, c+=b[i];);
 if(an<c||DENSE&AT(a))RZ(a=reaxis(ifb(r,b),a)); ap=PAV(a); ae=SPA(ap,e); ax=SPA(ap,x); m=*AS(ax);
 if(wn<c||DENSE&AT(w))RZ(w=reaxis(ifb(r,b),w)); wp=PAV(w); we=SPA(wp,e); wx=SPA(wp,x); n=*AS(wx);
 RZ(x=indexof(SPA(ap,i),SPA(wp,i))); v=AV(x);
 GATV(p,B01,m,1,0); pv=BAV(p);
 GATV(q,B01,n,1,0); qv=BAV(q); 
 memset(pv,C1,m); DO(n, j=*v++; if(j<m)pv[j]=qv[i]=0; else qv[i]=1;);
 if(memchr(pv,C1,m)&&!all1(eq(we,repeat(p,ax))))R zero;
 if(memchr(qv,C1,n)&&!all1(eq(ae,repeat(q,wx))))R zero;
 j=0; DO(m, if(pv[i])++j;);
 k=0; DO(n, if(qv[i])++k; qv[i]=!qv[i];);
 if(!equ(from(repeat(q,x),ax),repeat(q,wx)))R zero;
 x=SPA(ap,a); v=AV(x); s=AS(a); d=1.0; DO(AN(x), d*=s[v[i]];);
 R d==m+k&&d==n+j||equ(ae,we)?one:zero;
}    /* a -:"r w on sparse arrays */


F2(jtmatch){A z;I af,f,m,n,mn,*s,wf;
 RZ(a&&w);
 if(SPARSE&(AT(a)|AT(w)))R matchs(a,w);
 af=jt->rank?AR(a)-jt->rank[0]:0;
 wf=jt->rank?AR(w)-jt->rank[1]:0; jt->rank=0;
 if(af>wf){f=af; s=AS(a); RE(m=prod(wf,s)); RE(n=prod(af-wf,wf+s));}
 else     {f=wf; s=AS(w); RE(m=prod(af,s)); RE(n=prod(wf-af,af+s));}
 RE(mn=mult(m,n));
 GATV(z,B01,mn,f,s); matchsub(af,wf,m,n,a,w,BAV(z),C0,C1);
 R z;
}    /* a -:"r w */

F2(jtnotmatch){A z;I af,f,m,n,mn,*s,wf;
 RZ(a&&w);
 if(SPARSE&(AT(a)|AT(w)))R not(matchs(a,w));
 af=jt->rank?AR(a)-jt->rank[0]:0;
 wf=jt->rank?AR(w)-jt->rank[1]:0; jt->rank=0;
 if(af>wf){f=af; s=AS(a); RE(m=prod(wf,s)); RE(n=prod(af-wf,wf+s));}
 else     {f=wf; s=AS(w); RE(m=prod(af,s)); RE(n=prod(wf-af,af+s));}
 RE(mn=mult(m,n));
 GATV(z,B01,mn,f,s); matchsub(af,wf,m,n,a,w,BAV(z),C1,C0);
 R z;
}    /* a -.@-:"r w */
