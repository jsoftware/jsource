/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Power Operator ^: and Associates                          */

#include "j.h"


static DF1(jtpowseqlim){PROLOG(0039);A x,y,z,*zv;I i,n;
 RZ(w);
 RZ(z=exta(BOX,1L,1L,20L)); zv=AAV(z); *zv++=x=w;
 i=1; n=AN(z);
 while(1){
  if(n==i){RZ(z=ext(0,z)); zv=i+AAV(z); n=AN(z);}
  RZ(*zv++=x=df1(y=x,self));
  if(equ(x,y)){AN(z)=*AS(z)=i; break;}
  ++i;
 }
 z=ope(z);
 EPILOG(z);
}    /* f^:(<_) w */

static F2(jttclosure){A z;B b;I an,*av,c,d,i,wn,wr,wt,*wv,*zu,*zv,*zz;
 RZ(a&&w);
 wt=AT(w); wn=AN(w); wr=AR(w);
 if(B01&wt)RZ(w=cvt(INT,w)); wv=AV(w);
 av=AV(a); an=AN(a);
 RZ(z=exta(INT,1+wr,wn,20L)); 
 zv=AV(z); zz=zv+AN(z);
 if(1==wn){
  *zv++=c=*wv; d=1+c;
  while(c!=d){
   if(zv==zz){i=zv-AV(z); RZ(z=ext(0,z)); zv=AV(z)+i; zz=AV(z)+AN(z);}
   d=c; if(0>c)c+=an; ASSERT(0<=c&&c<an,EVINDEX); *zv++=c=av[c];
 }}else{
  ICPY(zv,wv,wn); zu=zv; zv+=wn;
  while(1){
   if(zv==zz){i=zv-AV(z); RZ(z=ext(0,z)); zv=AV(z)+i; zz=AV(z)+AN(z); zu=zv-wn;}
   b=1; DO(wn, d=c=*zu++; if(0>c)c+=an; ASSERT(0<=c&&c<an,EVINDEX); *zv++=c=av[c]; if(c!=d)b=0;);
   if(b)break;
 }}
 i=zv-AV(z); *AS(z)=d=i/wn-1; AN(z)=d*wn; ICPY(1+AS(z),AS(w),wr); 
 R z;
}    /* {&a^:(<_) w */

static DF1(jtindexseqlim1){A fs;
 RZ(w); 
 fs=VAV(self)->f;
 R AT(w)&B01+INT?tclosure(VAV(fs)->g,w):powseqlim(w,fs);
}    /* {&x^:(<_) w */

static DF2(jtindexseqlim2){
 RZ(a&&w);
 R 1==AR(a)&&AT(a)&INT&&AT(w)&B01+INT?tclosure(a,w):powseqlim(w,amp(ds(CFROM),a));
}    /* a {~^:(<_) w */

static DF1(jtpowseq){A fs,gs,x;I n=IMAX;V*sv;
 RZ(w);
 sv=VAV(self); fs=sv->f; gs=sv->g;
 ASSERT(!AR(gs),EVRANK);
 ASSERT(BOX&AT(gs),EVDOMAIN);
 x=*AAV(gs); if(!AR(x))RE(n=i0(vib(x)));
 if(0>n){RZ(fs=inv(fs)); n=-n;}
 if(n==IMAX||1==AR(x)&&!AN(x))R powseqlim(w,fs);
 R df1(w,powop(fs,IX(n),0));
}    /* f^:(<n) w */

static DF1(jtfpown){A fs,z;AF f1;I n,old;V*sv;
 RZ(w);
 sv=VAV(self); 
 switch(n=*AV(sv->h)){
  case 0:  R ca(w);
  case 1:  fs=sv->f; R CALL1(VAV(fs)->f1,w,fs);
  default: 
   fs=sv->f; f1=VAV(fs)->f1;
   z=w; 
   old=jt->tnextpushx; 
   DO(n, RZ(z=CALL1(f1,z,fs)); gc(z,old);); 
   R z;
}}   /* single positive finite exponent */

static DF1(jtply1){PROLOG(0040);DECLFG;A b,hs,j,x,*xv,y,z;B*bv,q;I i,k,m,n,*nv,old,p=0;
 hs=sv->h; m=AN(hs); 
 RZ(x=ravel(hs)); RZ(y=from(j=grade1(x),x)); nv=AV(y);
 GATV(x,BOX,m,1,0); xv=AAV(x);
 while(p<m&&0>nv[p])p++;
 if(p<m){
  RZ(z=ca(w));
  n=nv[m-1]; k=p;
  while(k<m&&!nv[k]){xv[k]=z; ++k;}
  RZ(b=eq(ainf,from(j,ravel(gs)))); bv=BAV(b); q=k<m?bv[k]:0;
  old=jt->tnextpushx;
  for(i=1;i<=n;++i){
   RZ(z=CALL1(f1,y=z,fs));
   if(q&&equ(y,z)){DO(m-k, xv[k]=z; ++k;); break;}
   while(k<m&&i==nv[k]){xv[k]=z; ++k; q=k<m?bv[k]:0;}
   if(!(i%10))gc3(x,z,0L,old);
 }}
 if(0<p){
  RZ(fs=inv(fs)); f1=VAV(fs)->f1;
  RZ(z=ca(w));
  n=nv[0]; k=p-1;
  RZ(b=eq(scf(-inf),from(j,ravel(gs)))); bv=BAV(b); q=bv[k];
  old=jt->tnextpushx;
  for(i=-1;i>=n;--i){
   RZ(z=CALL1(f1,y=z,fs));
   if(q&&equ(y,z)){DO(1+k, xv[k]=z; --k;); break;}
   while(0<=k&&i==nv[k]){xv[k]=z; --k; q=0<=k?bv[k]:0;}
   if(!(i%10))gc3(x,z,0L,old);
 }}
 z=ope(reshape(shape(hs),from(grade1(j),x))); EPILOG(z);
}

#define DIST(i,x)  if(i==e){v=CAV(x); \
                     while(k<m&&i==(e=nv[jv[k]])){MC(zv+c*jv[k],v,c); ++k;}}

static DF1(jtply1s){DECLFG;A hs,j,y,y1,z;C*v,*zv;I c,e,i,*jv,k,m,n,*nv,r,*s,t,zn;
 RZ(w);
 hs=sv->h; m=AN(hs); nv=AV(hs); 
 RZ(j=grade1(ravel(hs))); jv=AV(j); e=nv[*jv];
 if(!e&&!nv[jv[m-1]])R reshape(over(shape(hs),shape(w)),w);
 RZ(y=y1=CALL1(f1,w,fs)); t=AT(y); r=AR(y);
 if(0>e||t&BOX)R ply1(w,self);
 if(!e){
  if(HOMO(t,AT(w)))RZ(w=pcvt(t,w));
  if(!(TYPESEQ(t,AT(w))&&AN(y)==AN(w)&&(r==AR(w)||1>=r&&1>=AR(w))))R ply1(w,self);
 }
 k=AR(hs); RE(zn=mult(m,AN(y)));
 GA(z,AT(y),zn,k+AR(y),0); zv=CAV(z);
 s=AS(z); ICPY(s,AS(hs),k); ICPY(k+s,AS(y),r);
 n=nv[jv[m-1]]; c=AN(y)*bp(t); s=AS(y);
 k=0; DIST(0,w); DIST(1,y);
 for(i=2;i<=n;++i){
  RZ(y=CALL1(f1,y,fs));
  if(TYPESNE(t,AT(y))||r!=AR(y)||ICMP(AS(y),s,r))R ply1(w,self);
  DIST(i,y);
 }
 R z;
}    /* f^:n w, non-negative finite n, well-behaved f */

static DF1(jtinv1){DECLFG;A z; RZ(w);    FDEPINC(1); z=df1(w,inv(fs));        FDEPDEC(1); R z;}
static DF2(jtinv2){DECLFG;A z; RZ(a&&w); FDEPINC(1); z=df1(w,inv(amp(a,fs))); FDEPDEC(1); R z;}

static CS2(jtply2,  df1(w,powop(amp(a,fs),gs,0)),0107)

static DF1(jtpowg1){A h=VAV(self)->h; R df1(  w,*AAV(h));}
static DF2(jtpowg2){A h=VAV(self)->h; R df2(a,w,*AAV(h));}

// When u^:v is encountered, we replace it with a verb that comes to one of these.
// This creates a verb, jtpowxx, which calls jtdf1 within a PROLOG/EPILOG pair, after creating several names:
// sv->self data; fs=sv->f (the A block for the f operand); f1=f1 in sv->f (0 if sv->f==0); f2=f2 in sv->f (0 if sv->f==0);
//                gs=sv->g (the A block for the g operand); g1=f1 in sv->g (0 if sv->g==0); g2=f2 in sv->g (0 if sv->g==0)
// Here, f1 is the original u and g1 is the original v
// We call g1 (=original v), passing in y (and gs as self).  This returns v y
// We then call powop(original u,result of v y), which is the VN case for u^:(v y) and creates a derived verb to perform that function 
// Finally df1 treats the powop result as self, calling self/powop->f1 (the appropriate power case based on v y)
//   with the y arg as the w operand (and self/powop included to provide access to the original u)
// We allow v to create a gerund, but we do not allow a gerund to create a gerund.

// We could handle the special cases 0 & 1 here, but we don't.  We go ahead and call powop to return the verb (] or a).
// That way we only have to check for 0&1 in one place, and it picks up u^:v and u^:n

// here for u^:v y
static CS1IP(jtpowv1, \
A u; RZ(u = powop(fs,        CALL1(g1,  w,gs),(A)1));  \
z=(VAV(u)->f1)(VAV(u)->flag&VINPLACEOK1?jtinplace:jt,w,u) \
,0108)
// here for x u^:v y 
static CS2IP(jtpowv2, \
A u; RZ(u = powop(fs,        CALL2(g2,a,w,gs),(A)1)); \
z=(VAV(u)->f2)(VAV(u)->flag&VINPLACEOK2?jtinplace:jt,a,w,u); \
,0109)
// here for x u@:]^:v y and x u@]v y
static CS2IP(jtpowv2a, \
A u; RZ(u = powop(VAV(fs)->f,CALL2(g2,a,w,gs),(A)1)); \
z=(VAV(u)->f1)(VAV(u)->flag&VINPLACEOK1?jtinplace:jt,w,u); \
,0110)

// This executes the conjunction u^:v to produce a derived verb.  If the derived verb
// contains verb v or gerund v, it executes v on the xy arguments and then calls jtpowop
// to recreate a new derived verb with the function specified by the result of v.
// The result of v could be a gerund, which would cause the process to continue, so we put the
// kibosh on it by setting self (otherwise unused, and set to nonzero in the initial invocation
// from parse) to 0 in all calls resulting from execution of gerund v.  Then we fail any gerund
// if self is 0.
DF2(jtpowop){A hs;B b,r;I m,n;V*v;
 RZ(a&&w);
 switch(CONJCASE(a,w)){
  default: ASSERTSYS(0,"powop");
  case NV: ASSERT(0,EVDOMAIN);
  case NN: ASSERT(-1==i0(w),EVDOMAIN); R vger2(CPOWOP,a,w);
  case VV:
   // u^:v.  Create derived verb to handle it.
   v=VAV(a); b=(v->id==CAT||v->id==CATCO)&&ID(v->g)==CRIGHT;
   // The action routines are inplaceable; take ASGSAFE from u and v
   R CDERIV(CPOWOP,jtpowv1,b?jtpowv2a:jtpowv2,(v->flag&VAV(w)->flag&VASGSAFE)+(VINPLACEOK1|VINPLACEOK2), RMAX,RMAX,RMAX);
  case VN:
   // u^:n.  Check for special types.
   if(BOX&AT(w)){A x,y;AF f1,f2;
    // Boxed v.  It could be <n or [v0`]v1`v2.
    if(ARELATIVE(w))RZ(w=car(w));   // Resolve relative values if present
    if(!AR(w)&&(x=*AAV(w),!AR(x)&&NUMERIC&AT(x)||1==AR(x)&&!AN(x))){
     // here for <n or <''.  That will be handled by special code.
     f1=jtpowseq; f2=jtply2; v=VAV(a);
     // if u is {&n or {~, and n is <_ or <'', do the tclosure trick
     if((!AN(x)||FL&AT(x)&&inf==*DAV(x))){
      if(CAMP==v->id&&(CFROM==ID(v->f)&&(y=v->g,INT&AT(y)&&1==AR(y)))){f1=jtindexseqlim1;}  // {&b^:_ y
      else if(CTILDE==v->id&&CFROM==ID(v->f)){f2=jtindexseqlim2;}   // x {~^:_ y
     }
     R CDERIV(CPOWOP,f1,f2,VFLAGNONE, RMAX,RMAX,RMAX);  // create the derived verb for <n
    }
//    ASSERT(self,EVDOMAIN);  // If gerund returns gerund, error.  This check is removed pending further design
    R gconj(a,w,CPOWOP);  // create the derived verb for [v0`]v1`v2
   }
   // unboxed n.
   RZ(hs=vib(w));   // hs=n coerced to integer
   m=AN(hs); n=m?*AV(hs):0; r=0<AR(hs);  // m=#atoms of n; n=1st atom; r=n has rank>0
   if(!r){
    // Handle the 3 important cases: atomic _1 (inverse), 0 (nop), and 1 (execute u)
    switch(n){
    case 0: R ds(CRIGHT);   //  u^:0 is like ]
    case 1: R a;  // u^:1 is like u 
    case -1: R CDERIV(CPOWOP,jtinv1,jtinv2,VFLAGNONE, RMAX,RMAX,RMAX);  // create verb to calculate the inverse
    }
   }
   // If not special case, fall through to handle general case
   b=0; if(m&&AT(w)&FL+CMPX)RE(b=!all0(eps(w,over(ainf,scf(infm)))));   // set b if n is nonempty FL or CMPX array containing _ or __ 
   R fdef(CPOWOP,VERB, b||!m?jtply1:!r&&0<=n?jtfpown:jtply1s,jtply2, a,w,hs,   // Create derived verb: special cases for (empty or contains _/__), (scalar n>=0)
      VFLAGNONE, RMAX,RMAX,RMAX);
   // no reason to inplace this, since it has to keep the old value to check for changes
}}
