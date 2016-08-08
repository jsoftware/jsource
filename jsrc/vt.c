/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Take and Drop                                                    */

#include "j.h"


F1(jtbehead ){R drop(one,    w);}
F1(jtcurtail){R drop(num[-1],w);}

F1(jtshift1){R drop(num[-1],over(one,w));}

static A jttk0(J jt,B b,A a,A w){A z;I k,m=0,n,p,r,*s,*u;
 r=AR(w); n=AN(a); u=AV(a); 
 if(!b){RE(m=prod(n,u)); ASSERT(m>IMIN,EVLIMIT); RE(m=mult(ABS(m),prod(r-n,n+AS(w))));}
 GA(z,AT(w),m,r,AS(w)); 
 s=AS(z); DO(n, p=u[i]; ASSERT(p>IMIN,EVLIMIT); *s++=ABS(p););
 if(m){k=bp(AT(w)); mvc(k*m,AV(z),k,jt->fillv);}
 R z;
}

static F2(jttks){PROLOG(0092);A a1,q,x,y,z;B b,c;I an,m,r,*s,*u,*v;P*wp,*zp;
 an=AN(a); u=AV(a); r=AR(w); s=AS(w); 
 GA(z,AT(w),1,r,s); v=AS(z); DO(an, v[i]=ABS(u[i]););
 zp=PAV(z); wp=PAV(w);
 if(an<=r){RZ(a=vec(INT,r,s)); ICPY(AV(a),u,an);}
 a1=SPA(wp,a); RZ(q=paxis(r,a1)); m=AN(a1);
 RZ(a=from(q,a       )); u=AV(a);
 RZ(y=from(q,shape(w))); s=AV(y);
 b=0; DO(r-m, if(b=u[i+m]!=s[i+m])break;);
 c=0; DO(m,   if(c=u[i  ]!=s[i  ])break;);
 if(b){jt->fill=SPA(wp,e); x=irs2(vec(INT,r-m,m+u),SPA(wp,x),0L,1L,-1L,jttake); jt->fill=0; RZ(x);}
 else x=SPA(wp,x);
 if(c){A j;C*xv,*yv;I d,i,*iv,*jv,k,n,t;
  d=0; t=AT(x); k=bp(t)*aii(x);
  q=SPA(wp,i); n=IC(q);
  GATV(j,INT,AN(q),AR(q),AS(q)); jv= AV(j); iv= AV(q);
  GA(y,t,  AN(x),AR(x),AS(x)); yv=CAV(y); xv=CAV(x);
  for(i=0;i<n;++i){
   c=0; DO(m, t=u[i]; if(c=0>t?iv[i]<t+s[i]:iv[i]>=t)break;);
   if(!c){++d; MC(yv,xv,k); yv+=k; DO(m, t=u[i]; *jv++=0>t?iv[i]-(t+s[i]):iv[i];);}
   iv+=m; xv+=k;
  }
  SPB(zp,i,d<n?take(sc(d),j):j); SPB(zp,x,d<n?take(sc(d),y):y);
 }else{SPB(zp,i,ca(SPA(wp,i))); SPB(zp,x,b?x:ca(x));}
 SPB(zp,a,ca(SPA(wp,a)));
 SPB(zp,e,ca(SPA(wp,e)));
 EPILOG(z);
}    /* take on sparse array w */

static F2(jttk){PROLOG(0093);A y,z;B b=0;C*yv,*zv;I c,d,dy,dz,e,i,k,m,n,p,q,r,*s,t,*u;
 n=AN(a); u=AV(a); r=AR(w); s=AS(w); t=AT(w);
 if(t&SPARSE)R tks(a,w);
 DO(n, if(!u[i]){b=1; break;}); if(!b)DO(r-n, if(!s[n+i]){b=1; break;});
 if(b||!AN(w))R tk0(b,a,w);
 k=bp(t); z=w; c=q=1;
 for(i=0;i<n;++i){
  c*=q; p=u[i]; q=ABS(p); m=s[i];
  if(q!=m){
   RE(d=mult(AN(z)/m,q)); GA(y,t,d,r,AS(z)); *(i+AS(y))=q;
   if(q>m)mvc(k*AN(y),CAV(y),k,jt->fillv);
   d=AN(z)/(m*c)*k; e=d*MIN(m,q);
   dy=d*q; yv=CAV(y); if(0>p&&q>m)yv+=d*(q-m);
   dz=d*m; zv=CAV(z); if(0>p&&m>q)zv+=d*(m-q);
   DO(c, MC(yv,zv,e); yv+=dy; zv+=dz;);
   b=1; z=y;
 }}
 if(!b)z=ca(w); 
 z=RELOCATE(w,z);
 EPILOG(z);
}

F2(jttake){A s,t;D*av,d;I acr,af,ar,n,*tv,*v,wcr,wf,wr;
 RZ(a&&w);
 if(SPARSE&AT(a))RZ(a=denseit(a));
 if(!(SPARSE&AT(w)))RZ(w=setfv(w,w)); 
 ar=AR(a); acr=jt->rank?jt->rank[0]:ar; af=ar-acr;
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr; jt->rank=0;
 if(af||1<acr)R rank2ex(a,w,0L,af?acr:1L,wcr,jttake);
 n=AN(a); 
 ASSERT(!wcr||n<=wcr,EVLENGTH);
 if(AT(a)&B01+INT)RZ(s=a=vi(a))
 else{
  RZ(t=vib(a));
  if(!(AT(a)&FL))RZ(a=cvt(FL,a));
  av=DAV(a); tv=AV(t); v=wf+AS(w);
  DO(n, d=av[i]; if(d==IMIN)tv[i]=(I)d; else if(INF(d))tv[i]=wcr?v[i]:1;)
  s=a=t;
 }
 if(!wcr||wf){
  RZ(s=vec(INT,wf+n,AS(w))); v=wf+AV(s); 
  if(!wcr){DO(n,v[i]=1;); RZ(w=reshape(s,w));}
  ICPY(v,AV(a),n);
 }
 R tk(s,w);
}

F2(jtdrop){A s;I acr,af,ar,d,m,n,*u,*v,wcr,wf,wr;
 RZ((a=vib(a))&&w);
 ar=AR(a); acr=jt->rank?jt->rank[0]:ar; af=ar-acr; 
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr; jt->rank=0;
 if(af||1<acr)R rank2ex(a,w,0L,af?acr:1L,wcr,jtdrop);
 n=AN(a); u=AV(a);
 ASSERT(!wcr||n<=wcr,EVLENGTH);
 if(wcr){RZ(s=shape(w)); v=wf+AV(s); DO(n, d=u[i]; m=v[i]; v[i]=d<-m?0:d<0?d+m:d<m?d-m:0;);}
 else{GATV(s,INT,wr+n,1,0); v=AV(s); ICPY(v,AS(w),wf); v+=wf; DO(n, v[i]=!u[i];); RZ(w=reshape(s,w));}
 R tk(s,w);
}


static F1(jtrsh0){A x,y;I wcr,wf,wr,*ws;
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr; jt->rank=0;
 ws=AS(w);
 RZ(x=vec(INT,wr-1,ws)); ICPY(wf+AV(x),ws+wf+1,wcr-1);
 RZ(w=setfv(w,w)); GA(y,AT(w),1,0,0); MC(AV(y),jt->fillv,bp(AT(w)));
 R reshape(x,y);
}

F1(jthead){I wcr,wf,wr;
 RZ(w);
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr;
 R !wcr||*(wf+AS(w))? from(num[ 0],w) : 
     SPARSE&AT(w)?irs2(num[0],take(num[ 1],w),0L,0L,wcr,jtfrom):rsh0(w);
}

F1(jttail){I wcr,wf,wr;
 RZ(w);
 wr=AR(w); wcr=jt->rank?jt->rank[1]:wr; wf=wr-wcr;
 R !wcr||*(wf+AS(w))?from(num[-1],w) :
     SPARSE&AT(w)?irs2(num[0],take(num[-1],w),0L,0L,wcr,jtfrom):rsh0(w);
}
