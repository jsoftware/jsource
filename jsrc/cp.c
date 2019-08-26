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
 if(1==wn){   // kludge having separate cases for wn==1 seems overkill
  *zv++=c=*wv; d=1+c;
  while(c!=d){
   if(zv==zz){i=zv-AV(z); RZ(z=ext(0,z)); zv=AV(z)+i; zz=AV(z)+AN(z);}
   d=c; if((UI)c>=(UI)an){c+=an; ASSERT((UI)c<(UI)an,EVINDEX);} *zv++=c=av[c];
  }
  d=(zv-AV(z))-1;
 }else{
  ICPY(zv,wv,wn); zu=zv; zv+=wn;
  while(1){
   if(zv==zz){i=zv-AV(z); RZ(z=ext(0,z)); zv=AV(z)+i; zz=AV(z)+AN(z); zu=zv-wn;}
   b=1; DQ(wn, d=c=*zu++; if((UI)c>=(UI)an){c+=an; ASSERT((UI)c<(UI)an,EVINDEX);} *zv++=c=av[c]; if(c!=d)b=0;);
   if(b)break;
  }
  d=(zv-AV(z))/wn-1;
 }
 AS(z)[0]=d; AN(z)=d*wn; MCISH(1+AS(z),AS(w),wr); 
 RETF(z);
}    /* {&a^:(<_) w */

static DF1(jtindexseqlim1){A fs;
 RZ(w); 
 fs=FAV(self)->fgh[0];  // {&x
 R AT(w)&B01+INT?tclosure(FAV(fs)->fgh[1],w):powseqlim(w,fs);
}    /* {&x^:(<_) w */

static DF2(jtindexseqlim2){
 RZ(a&&w);
 R 1==AR(a)&&AT(a)&INT&&AT(w)&B01+INT?tclosure(a,w):powseqlim(w,amp(ds(CFROM),a));
}    /* a {~^:(<_) w */

static DF1(jtpowseq){A fs,gs,x;I n=IMAX;V*sv;
 RZ(w);
 sv=FAV(self); fs=sv->fgh[0]; gs=sv->fgh[1];
 ASSERT(!AR(gs),EVRANK);
 ASSERT(BOX&AT(gs),EVDOMAIN);
 x=*AAV(gs); if(!AR(x))RE(n=i0(vib(x)));
 if(0>n){RZ(fs=inv(fs)); n=-n;}
 if(n==IMAX||1==AR(x)&&!AN(x))R powseqlim(w,fs);
 R df1(w,powop(fs,IX(n),0));
}    /* f^:(<n) w */

// u^:n w where n is nonnegative integer atom (but never 0 or 1, which are handled as special cases)
static DF1(jtfpown){A fs,z;AF f1;I n;V*sv;A *old;
 RZ(w);
 F1PREFIP;
 sv=FAV(self);
 n=*AV(sv->fgh[2]);
// obsolete if(n==0||n==1)SEGFAULT  // scaf
// switch(n=*AV(sv->fgh[2])){
//  case 0:  RCA(w);
//  case 1:  fs=sv->fgh[0]; R CALL1(FAV(fs)->valencefns[0],w,fs);
//  default: 
 fs=sv->fgh[0]; f1=FAV(fs)->valencefns[0];
 z=w; 
 old=jt->tnextpushp; 
 DQ(n, JATTN; RZ(z=CALL1IP(f1,z,fs)); z=gc(z,old);); 
 RETF(z);
// }
}

// u^:n w where n is any array or scalar infinity or negative
static DF1(jtply1){PROLOG(0040);DECLFG;A b,hs,j,*xv,y,z;B*bv,q;I i,k,m,n,*nv,p=0;AD * RESTRICT x;A *old;  // RESTRICT on x fails in VS2013
 hs=sv->fgh[2]; m=AN(hs); 
 RZ(y=ravel(hs)); RZ(y=from(j=grade1(y),y)); nv=AV(y);  // j is grading permutation of y; y is sorted powers
 GATV0(x,BOX,m,1); xv=AAV(x);  // cannot be virtual
 while(p<m&&0>nv[p])p++;  // find first positive power
 if(p<m){  // if there is a positive power...
  z=w;
  n=nv[m-1]; k=p;
  while(k<m&&!nv[k]){INSTALLBOX(x,xv,k,z); ++k;}  // install the input as the result for any 0 powers
  RZ(b=eq(ainf,from(j,ravel(gs)))); bv=BAV(b); q=k<m?bv[k]:0;
  old=jt->tnextpushp;  // should move this up
  for(i=1;i<=n;++i){
   RZ(z=CALL1(f1,y=z,fs));  // z=next power, y=previous power
   if(q&&equ(y,z)){DQ(m-k, INSTALLBOX(x,xv,k,z); ++k;); break;}  // if there is an infinity, check for repetition; if any, use it for all higher powers, & be done
   while(k<m&&i==nv[k]){INSTALLBOX(x,xv,k,z); ++k; q=k<m?bv[k]:0;}  // otherwise use result for all equal powers
   if(!(i&7)){JATTN; if(!gc3((A*)&x,&z,0L,old))R0;}
 }}
 if(0<p){  // if there was a negative power...
  RZ(fs=inv(fs)); f1=FAV(fs)->valencefns[0];
  z=w;
  n=nv[0]; k=p-1;
  RZ(b=eq(scf(-inf),from(j,ravel(gs)))); bv=BAV(b); q=bv[k];
  old=jt->tnextpushp;
  for(i=-1;i>=n;--i){
   RZ(z=CALL1(f1,y=z,fs));
   if(q&&equ(y,z)){DQ(1+k, INSTALLBOX(x,xv,k,z); --k;); break;}
   while(0<=k&&i==nv[k]){INSTALLBOX(x,xv,k,z); --k; q=0<=k?bv[k]:0;}
   if(!(i&7)){JATTN; if(!gc3((A*)&x,&z,0L,old))R0;}
 }}
 z=ope(reshape(shape(hs),from(grade1(j),x))); EPILOG(z);
}

// u^:_ w
static DF1(jtpinf1){DECLFG;PROLOG(0340);A z;
 I i=0;
 while(1){
  RZ(z=CALL1(f1,w,fs));  // call the fn
  I isend=equ(z,w);  // remember if it is the same as last time
  if(!((isend-1)&++i&7)) {  // every so often, but always when we leave...
   JATTN;   // check for user interrupt, in case the function doesn't allocate memory
   EPILOGNORET(z);  // free up allocated blocks, but keep z
   if(isend)RETF(z);  // return at end
  }
  w=z;  // make the new result the starting value for next loop
 }
}

#define DIST(i,x)  if(i==e){v=CAV(x); \
                     while(k<m&&i==(e=nv[jv[k]])){MC(zv+c*jv[k],v,c); ++k;}}

// u^:n w where n is not empty and is either an array or a negative atom
static DF1(jtply1s){DECLFG;A hs,j,y,y1,z;C*v,*zv;I c,e,i,*jv,k,m,n,*nv,r,*s,t,zn;
 RZ(w);
 hs=sv->fgh[2]; m=AN(hs); nv=AV(hs); 
 RZ(j=grade1(ravel(hs))); jv=AV(j); e=nv[*jv];  // e=lowest power
 if(!e&&!nv[jv[m-1]])R reshape(over(shape(hs),shape(w)),w);  // all powers 0
 RZ(y=y1=CALL1(f1,w,fs)); t=AT(y); r=AR(y);
 if(0>e||t&BOX)R ply1(w,self);
 if(!e){
  if(HOMO(t,AT(w)))RZ(w=pcvt(t,w));
  if(!(TYPESEQ(t,AT(w))&&AN(y)==AN(w)&&(r==AR(w)||1>=r&&1>=AR(w))))R ply1(w,self);
 }
 k=AR(hs); RE(zn=mult(m,AN(y)));
 GA(z,AT(y),zn,k+AR(y),0); zv=CAV(z);
 s=AS(z); MCISH(s,AS(hs),k); MCISH(s+k,AS(y),r);
 n=nv[jv[m-1]]; c=AN(y)<<bplg(t); s=AS(y);
 k=0; DIST(0,w); DIST(1,y);
 for(i=2;i<=n;++i){
  RZ(y=CALL1(f1,y,fs));
  if(TYPESNE(t,AT(y))||r!=AR(y)||ICMP(AS(y),s,r))R ply1(w,self);
  DIST(i,y);
 }
 RETF(z);
}    /* f^:n w, non-negative finite n, well-behaved f */

static DF1(jtinv1){F1PREFIP;DECLFG;A z; RZ(w);A i; RZ(i=inv((fs))); FDEPINC(1);  z=(FAV(i)->valencefns[0])(FAV(i)->flag&VJTFLGOK1?jtinplace:jt,w,i);       FDEPDEC(1); RETF(z);}  // was invrecur(fix(fs))
static DF1(jtinvh1){F1PREFIP;DECLFGH;A z; RZ(w);    FDEPINC(1); z=(FAV(hs)->valencefns[0])(jtinplace,w,hs);        FDEPDEC(1); RETF(z);}
static DF2(jtinv2){DECLFG;A z; RZ(a&&w); FDEPINC(1); z=df1(w,inv(amp(a,fs))); FDEPDEC(1); STACKCHKOFL RETF(z);}  // the CHKOFL is to avoid tail recursion, which prevents a recursion loop from being broken
static DF1(jtinverr){F1PREFIP;ASSERT(0,EVDOMAIN);}  // used for uninvertible monads

static CS2(jtply2,  df1(w,powop(amp(a,fs),gs,0)),0107)  // dyad adds x to make x&u, and then reinterpret the compound.  We could interpret u differently now that it has been changed

static DF1(jtpowg1){A h=FAV(self)->fgh[2]; R df1(  w,*AAV(h));}
static DF2(jtpowg2){A h=FAV(self)->fgh[2]; R df2(a,w,*AAV(h));}

// When u^:v is encountered, we replace it with a verb that comes to one of these.
// This creates a verb, jtpowxx, which calls jtdf1 within a PROLOG/EPILOG pair, after creating several names:
// sv->self data; fs=sv->fgh[0] (the A block for the f operand); f1=f1 in sv->fgh[0] (0 if sv->fgh[0]==0); f2=f2 in sv->fgh[0] (0 if sv->fgh[0]==0);
//                gs=sv->fgh[1] (the A block for the g operand); g1=f1 in sv->fgh[1] (0 if sv->fgh[1]==0); g2=f2 in sv->fgh[1] (0 if sv->fgh[1]==0)
// Here, f1 is the original u and g1 is the original v
// We call g1 (=original v), passing in y (and gs as self).  This returns v y
// We then call powop(original u,result of v y), which is the VN case for u^:(v y) and creates a derived verb to perform that function 
// Finally df1 treats the powop result as self, calling self/powop->valencefns[0] (the appropriate power case based on v y)
//   with the y arg as the w operand (and self/powop included to provide access to the original u)
// We allow v to create a gerund, but we do not allow a gerund to create a gerund.

// We catch the special cases 0  & 1 here, mostly for branch-prediction purposes.  All results of g1/g2 will be nouns, while
// most instances of u^:v (run through powop) have v as verb

// here for u^:v y
CS1IP(static,jtpowv1, \
A u; A v; RZ(u=CALL1(g1,  w,gs));  /* execute v */ \
if(!AR(u) && (v=vib(u)) && !(IAV(v)[0]&~1)){z=IAV(v)[0]?(FAV(fs)->valencefns[0])(FAV(fs)->flag&VJTFLGOK1?jtinplace:jt,w,fs):w;} \
else{RESETERR; RZ(u = powop(fs,u,(A)1));  \
z=(FAV(u)->valencefns[0])(FAV(u)->flag&VJTFLGOK1?jtinplace:jt,w,u);} \
,0108)
// here for x u^:v y 
CS2IP(static,jtpowv2, \
A u; A v; RZ(u=CALL2(g2,a,w,gs));  /* execute v */ \
if(!AR(u) && (v=vib(u)) && !(IAV(v)[0]&~1)){z=IAV(v)[0]?(FAV(fs)->valencefns[1])(FAV(fs)->flag&VJTFLGOK2?jtinplace:jt,a,w,fs):w;} \
else{RESETERR; RZ(u = powop(fs,u,(A)1));  \
z=(FAV(u)->valencefns[1])(FAV(u)->flag&VJTFLGOK2?jtinplace:jt,a,w,u);} \
,0109)
// here for x u@:]^:v y and x u@]^:v y
CS2IP(static,jtpowv2a, \
A u; A v; fs=FAV(fs)->fgh[0]; RZ(u=CALL2(g2,a,w,gs));  /* execute v */ \
if(!AR(u) && (v=vib(u)) && !(IAV(v)[0]&~1)){z=IAV(v)[0]?(FAV(fs)->valencefns[0])(FAV(fs)->flag&VJTFLGOK2?jtinplace:jt,w,fs):w;} \
else{RESETERR; RZ(u = powop(fs,u,(A)1));  \
z=(FAV(u)->valencefns[0])(FAV(u)->flag&VJTFLGOK1?jtinplace:jt,w,u);} \
,0110)

// This executes the conjunction u^:v to produce a derived verb.  If the derived verb
// contains verb v or gerund v, it executes v on the xy arguments and then calls jtpowop
// to recreate a new derived verb with the function specified by the result of v.
// The result of v could be a gerund, which would cause the process to continue, so we put the
// kibosh on it by setting self (otherwise unused, and set to nonzero in the initial invocation
// from parse) to 0 in all calls resulting from execution of gerund v.  Then we fail any gerund
// if self is 0.
DF2(jtpowop){A hs;B b;V*v;
 RZ(a&&w);
 ASSERT(AT(a)&VERB,EVDOMAIN);  // u must be a verb
 if(AT(w)&VERB){
  // u^:v.  Create derived verb to handle it.
  v=FAV(a); b=(v->id==CAT||v->id==CATCO)&&ID(v->fgh[1])==CRIGHT;  // detect u@]^:v
  // The action routines are inplaceable; take ASGSAFE from u and v, inplaceability from u
  R CDERIV(CPOWOP,jtpowv1cell,b?jtpowv2acell:jtpowv2cell,(v->flag&FAV(w)->flag&VASGSAFE)+(v->flag&(VJTFLGOK1|VJTFLGOK2)), RMAX,RMAX,RMAX);
 }
 // u^:n.  Check for special types.
 if(BOX&AT(w)){A x,y;AF f1,f2;
  // Boxed v.  It could be <n or [v0`]v1`v2 or <''.
  if(!AR(w)&&(x=*AAV(w),!AR(x)&&NUMERIC&AT(x)||1==AR(x)&&!AN(x))){
   // here for <n or <''.  That will be handled by special code.
   f1=jtpowseq; f2=jtply2; v=FAV(a);
   // if u is {&n or {~, and n is <_ or <'', do the tclosure trick
   if((!AN(x)||FL&AT(x)&&inf==*DAV(x))){
    if(CAMP==v->id&&(CFROM==ID(v->fgh[0])&&(y=v->fgh[1],INT&AT(y)&&1==AR(y)))){f1=jtindexseqlim1;}  // {&b^:_ y
    else if(CTILDE==v->id&&CFROM==ID(v->fgh[0])){f2=jtindexseqlim2;}   // x {~^:_ y
   }
   R CDERIV(CPOWOP,f1,f2,VFLAGNONE, RMAX,RMAX,RMAX);  // create the derived verb for <n
  }
//    ASSERT(self,EVDOMAIN);  // If gerund returns gerund, error.  This check is removed pending further design
  R gconj(a,w,CPOWOP);  // create the derived verb for [v0`]v1`v2
 }
 // unboxed n.
 RZ(hs=vib(w));   // hs=n coerced to integer
 if(!AR(w)){  // input is an atom
  // Handle the 4 important cases: atomic _1 (inverse), 0 (nop), 1 (execute u), and _ (converge/do while)
  if(!(IAV(hs)[0]&~1))R a=IAV(hs)[0]?a:ds(CRIGHT);  //  u^:0 is like ],  u^:1 is like u 
  if((IAV(hs)[0]<<1)==-2){  //  u^:_1 or u^:_
   if(IAV(hs)[0]<0){  // u^:_1
    // if there are no names, calculate the monadic inverse and save it in h.  Inverse of the dyad, or the monad if there are names,
    // must wait until we get arguments
    A h=0; AF f1=jtinv1; if(nameless(a)){if(h=inv(a)){f1=jtinvh1;}else{f1=jtinverr; RESETERR}} // h must be valid for free.  If no names in w, take the inverse.  If it doesn't exist, fail the monad but keep the dyad going
    I flag = (FAV(a)->flag&VASGSAFE) + (h?FAV(h)->flag&VJTFLGOK1:VJTFLGOK1);  // inv1 inplaces and calculates ip for next step; invh has ip from inverse
    R fdef(0,CPOWOP,VERB,(AF)(f1),jtinv2,a,w,h,flag,RMAX,RMAX,RMAX);
   }else{  // u^:_
    R fdef(0,CPOWOP,VERB,jtpinf1,jtply2,a,w,0,VFLAGNONE,RMAX,RMAX,RMAX);
   }
  }
 }
 I m=AN(hs); // m=#atoms of n; n=1st atom; r=n has rank>0
 // If not special case, fall through to handle general case
 b=0; if(m&&AT(w)&FL+CMPX)RE(b=!all0(eps(w,over(ainf,scf(infm)))));   // set b if n is nonempty FL or CMPX array containing _ or __ kludge should just use hs
 b|=!m; B nonnegatom=!AR(w)&&0<=IAV(hs)[0]; I flag=FAV(a)->flag&((~b&nonnegatom)<<VJTFLGOK1X);  // flags for (empty or contains _/__), (scalar n>=0); if the latter, keep the inplace flag
// obsolete  R fdef(0,CPOWOP,VERB, b||!m?jtply1:!AR(w)&&0<=IAV(hs)[0]?jtfpown:jtply1s,jtply2, a,w,hs,   // Create derived verb: special cases for , 
 R fdef(0,CPOWOP,VERB, b?jtply1:nonnegatom?jtfpown:jtply1s,jtply2, a,w,hs,   // Create derived verb: special cases for , 
    flag|VFLAGNONE, RMAX,RMAX,RMAX);
 // no reason to inplace this, since it has to keep the old value to check for changes
}
