/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Hypergeometric Series                                     */

#include "j.h"


static A jthparm(J jt,A j,A f,A h){A z;
 if(!(VERB&AT(f)))R shift1(aslash(CSTAR,atab(CPLUS,h,j)));
 RZ(z=CALL1(FAV(f)->valencefns[0],j,f));
 ASSERT(1>=AR(z),EVRANK); 
 ASSERT(!AR(z)||AN(j)==AN(z),EVLENGTH);
 R z;
}

static A jthgv(J jt,B b,I n,A w,A self){A c,d,e,h,*hv,j,y;V*sv=FAV(self);
 RZ(j=IX(n)); h=sv->fgh[2]; hv=AAV(h);
 RZ(c=hparm(j,sv->fgh[0],C(hv[0])));
 RZ(d=hparm(j,sv->fgh[1],C(hv[1])));
 e=shift1(divide(w,apv(n,1L,1L)));
 switch((VERB&AT(sv->fgh[0])?2:0)+(VERB&AT(sv->fgh[1])?1:0)){
 case 0: y=ascan(CSTAR,divide(tymes(c,e),d)); break;
 case 1: y=divide(ascan(CSTAR,tymes(c,e)),d); break;
 case 2: y=divide(tymes(c,ascan(CSTAR,e)),ascan(CSTAR,d)); break;
 case 3: y=divide(tymes(c,ascan(CSTAR,e)),d);
 }
 R b?over(num(0),ascan(CPLUS,y)):aslash(CPLUS,y);
}    /* verb or complex cases */

static A jthgd(J jt,B b,I n,A w,A p,A q){A c,d,e,z;D r,s,t,*u,*v,x,*zv;I j,pn,qn;
 RZ(c=ccvt(FL,p,0)); u=DAV(c); pn=AN(c);
 RZ(d=ccvt(FL,q,0)); v=DAV(d); qn=AN(d);
 RZ(e=ccvt(FL,w,0)); x=DAV(e)[0]; r=s=1; t=0; z=0;
 if(b&&2000>n){GATV0(z,FL,1+n,1); zv=DAV1(z); *zv++=0; *zv++=1;}
 NAN0;
 for(j=1;j<n&&t!=s&&!_isnan(s);++j){
  DO(pn, r*=u[i]; ++u[i];);  /* r*=u[i]++; compiler error on 3B1 */
  DO(qn, r/=v[i]; ++v[i];); 
  r*=x/j; t=s; s+=r; if(z)*zv++=s; JBREAK0;
 }
 NAN1;
 R !b?scf(s):z?take(sc(1+j),z):hgd(b,j,w,p,q);
}    /* real vector p,q; real scalar w; all terms (1=b) or last term (0=b) */

static DF2(jthgeom2){PROLOG(0036);A h,*hv,t,z;B b;I an,*av,j,n;V*sv=FAV(self);
 ARGCHK2(a,w);
 if(AR(w))R rank2ex0(a,w,self,jthgeom2);
 RZ(a=AT(a)&FL+CMPX+QP?vib(a):vi(a));  // kludge just call vib?
 an=AN(a); av=AV(a); n=0; DO(an, j=av[i]; ASSERT(0<=j,EVDOMAIN); if(n<j)n=j;);
 if(!n)R tymes(zeroionei(0),a);
 h=sv->fgh[2]; hv=AAV(h);
 b=VERB&(AT(sv->fgh[0])|AT(sv->fgh[1]))||CMPX&(AT(w)|AT(C(hv[0]))|AT(C(hv[1])));
 if(!b)z=hgd((B)(1<an),n,w,C(hv[0]),C(hv[1]));
 else if(2000>n)z=hgv((B)(1<an),n,w,self);
 else{
  j=10; t=mtv; z=zeroionei(1);
  while(z&&!equ(z,t)){t=z; z=hgv(0,j,w,self); j+=j;} 
  RZ(z); if(1<an)z=hgv(1,j,w,self);
 }
 if(1<an)z=from(minimum(a,sc(SETIC(z,an)-1)),z);
 EPILOG(z);
}

static DF1(jthgeom1){R hgeom2(sc(IMAX),w,self);}

static F2(jtcancel){A c,d,f,x,y;
 a=ravel(a); x=nub(a); c=keytally(a,a);  // nub of a, & # times values appear
 w=ravel(w); y=nub(w); d=keytally(w,w);  // same for w
 a=repeat(maximum(num(0),minus(c,from(indexof(y,x),over(d,zeroionei(0))))),x);
 w=repeat(maximum(num(0),minus(d,from(indexof(x,y),over(c,zeroionei(0))))),y);
 R jlink(a,w);
}

F2(jthgeom){F2PREFIP;A c,d,h=0;B p,q;I at,wt;
 ARGCHK2(a,w);
 at=AT(a); p=1&&at&NOUN; c=d=mtv;
 wt=AT(w); q=1&&wt&NOUN;
 if(p){c=a; ASSERT(!AN(a)||at&NUMERIC,EVDOMAIN); ASSERT(1>=AR(a),EVRANK);}
 if(q){d=w; ASSERT(!AN(w)||wt&NUMERIC,EVDOMAIN); ASSERT(1>=AR(w),EVRANK);}
 RZ(h=cancel(c,d));
 R fdef(0,CHGEOM,VERB, jthgeom1,jthgeom2, a,w,h, 0L, 0L,0L,0L);
}    /* a H. w */

DF1(jthgcoeff){PROLOG(0037);A c,d,h,*hv,y,z;B b;I j,n,pn,qn,*v;V*sv=FAV(self);
 RZ(w=vi(w)); v=AV(w); 
 n=0; DO(AN(w), j=v[i]; ASSERT(0<=j,EVDOMAIN); if(n<j)n=j;);
 if(!n)R eq(w,w);
 h=sv->fgh[2]; hv=AAV(h); A hv0=C(hv[0]); A hv1=C(hv[1]);
 b=VERB&(AT(sv->fgh[0])|AT(sv->fgh[1]))||CMPX&(AT(w)|AT(hv0)|AT(hv1));
 if(!b){D r=1.0,*u,*v,*yv;
  RZ(c=ccvt(FL,hv0,0)); u=DAV(c); pn=AN(c);
  RZ(d=ccvt(FL,hv1,0)); v=DAV(d); qn=AN(d);
  GATV0(y,FL,n,1); yv=DAV1(y);
  DO(n, DO(pn, r*=u[i]; ++u[i];); DO(qn, r/=v[i]; ++v[i];); yv[i]=r;); 
 }else{A j;
  RZ(j=IX(n));
  c=hparm(j,sv->fgh[0],hv0);
  d=hparm(j,sv->fgh[1],hv1);
  switch((VERB&AT(sv->fgh[0])?2:0)+(VERB&AT(sv->fgh[1])?1:0)){
  case 0: y=ascan(CSTAR,divide(c,d)); break;
  case 1: y=divide(ascan(CSTAR,c),d); break;
  case 2: y=divide(c,ascan(CSTAR,d)); break;
  case 3: y=divide(c,d);
  }
 }
 RZ(z=from(w,over(zeroionei(1),y)));
 EPILOG(z);
}    /* coefficients indexed by w excluding !j */
