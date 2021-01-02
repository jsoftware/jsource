/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Differentiation and Integration                           */
#if 0
#include "j.h"

// returns TRUE if w is a constant verb m"r
static B jtiscons(J jt,A w){A x;V*v; 
 if(!w)R 0;
 v=VAV(w); x=v->fgh[0];
 R CQQ==v->id&&NOUN&AT(x)&&!AR(x);
}

static const C ispoly1[]={CLEFT,CRIGHT,CLE,CGE,CNOT,CMINUS,CPLUSCO,CHALVE,CCIRCLE,CJDOT,0};

static I jtispoly(J jt,A w){A e,f,g,h,x,y;B nf,ng,vf,vg;C c,id;I k,m,n,t;V*v;
 if(!w)R 0;
 v=VAV(w); id=v->id;
 if(id==CFCONS||iscons(w))R 1;
 if(strchr(ispoly1,id))R 2;
 if(id==CSTARCO)R 3;
 f=v->fgh[0]; nf=f&&NOUN&AT(f); vf=!nf; 
 g=v->fgh[1]; ng=g&&NOUN&AT(g); vg=!ng; x=nf?f:g; t=x?AT(x):0; h=nf?g:f; c=h?ID(h):0;
 if(id==CFORK){
  if(!(vf&&vg))R 0;
  m=ispoly(f); n=ispoly(v->fgh[2]);
  switch(m&&n?ID(g):0){
   case CPLUS: R MAX(m,n);
   case CSTAR: R m+n-1;
 }}
 if(vf&&vg&&(id==CAT||id==CATCO||id==CAMP||id==CAMPCO)){m=ispoly(f); n=ispoly(g); if(m&&n)R 1+(m-1)*(n-1);}
 if(!(id==CAMP&&(t&NUMERIC||c==CPOLY)))R 0;
 if(nf&&1>=AR(x)&&c==CPOLY){
  if(!(t&BOX+NUMERIC))R 0;
  SETIC(x,k);
  if(t&NUMERIC)R k;
  y=*(AAV(x)+k-1); if(!(2>=AR(y)))R 0;
  if(1>=AR(y))R 1+IC(y);
  if(!(2==AS(y)[1]))R 0;
  RZ(e=irs1(y,0L,1L,jttail));
  RZ(equ(e,floor1(e))&&all1(le(zeroionei(0),e)));
  RZ(y=aslash(CMAX,cvt(INT,e)));
  R 1+*AV(y);
 }
 if(nf==ng||AR(x))R 0;
 if(c==CPLUS||c==CMINUS||c==CSTAR||c==CDIV&&ng)R 2;
 RZ(x=pcvt(INT,x)); 
 if(!(INT&AT(x)))R 0;
 k=*AV(x);
 R 0<=k&&(c==CBANG&&nf||c==CEXP&&ng)?1+k:0;
}    /* 1 + degree of polynomial (0 if not poly) */

static F1(jtfpolyc){A b;B*bv;I m,n;
 RZ(b=ne(w,zeroionei(0))); bv=BAV(b);
 m=n=AN(w); DQ(n, if(bv[--m])break;); ++m;
 if(m<n)RZ(w=take(sc(m),w)); n=m;
 switch(n){
  case 1: R qq(head(w),zeroionei(0));
  case 3: if(equ(w,over(v2(0L,0L),zeroionei(1))))R ds(CSTARCO); break;
  case 2: 
   if(equ(w,v2( 0L,-1L)))           R ds(CMINUS);
   if(equ(w,v2( 1L,-1L)))           R ds(CNOT);
   if(equ(w,v2(-1L, 1L)))           R ds(CLE);
   if(equ(w,v2( 0L, 1L)))           R ds(CLEFT);
   if(equ(w,v2( 1L, 1L)))           R ds(CGE);
   if(equ(w,v2( 0L, 2L)))           R ds(CPLUSCO);
   if(equ(w,over(num(0),onehalf)))R ds(CHALVE);
   if(equ(w,over(num(0),pie)))R ds(CCIRCLE);
   if(equ(w,over(num(0),a0j1       )))R ds(CJDOT);
 }
 R amp(w,ds(CPOLY));
}

static A jtfpoly(J jt,I n,A f){I m=0>n?1:1+n; RZ(f); R fpolyc(df1(IX(m),tdot(f)));}

static F1(jtfnegate){V*v; ARGCHK1(w); v=VAV(w); R CAT==v->id&&CMINUS==ID(v->fgh[0])?v->fgh[1]:atop(ds(CMINUS),w);}

static F2(jtfplus){
 ARGCHK2(a,w);
 if(iscons(a)&&equ(VAV(a)->fgh[0],zeroionei(0)))R w;
 if(iscons(w)&&equ(VAV(w)->fgh[0],zeroionei(0)))R a;
 R folk(a,ds(CPLUS),w);
}

static F2(jtfminus){
 ARGCHK2(a,w);
 if(iscons(a)&&equ(VAV(a)->fgh[0],zeroionei(0)))R fnegate(w);
 if(iscons(w)&&equ(VAV(w)->fgh[0],zeroionei(0)))R a;
 R folk(a,ds(CMINUS),w);
}

static F2(jtftymes){A x,y;B b,c;I k;
 ARGCHK2(a,w);
 b=iscons(a); x=VAV(a)->fgh[0];
 c=iscons(w); y=VAV(w)->fgh[0];
 if(CFORK==ID(w)&&NOUN&AT(y))R ftymes(a,folk(qq(y,ainf),VAV(w)->fgh[1],VAV(w)->fgh[2]));
 if(b&&AT(x)&B01+INT){k=i0(x); if(-1<=k&&k<=1)R !k?a:0<k?w:fnegate(w);}
 if(c&&AT(y)&B01+INT){k=i0(y); if(-1<=k&&k<=1)R !k?w:0<k?a:fnegate(a);}
 if(b&&CFORK==ID(w)&&iscons(y))R ftymes(qq(tymes(x,VAV(y)->fgh[0]),zeroionei(0)),VAV(w)->fgh[2]);
 R c?folk(w,ds(CSTAR),a):folk(a,ds(CSTAR),w);
}

static F1(jtdpoly){A c,e,x;I n,t;
 ARGCHK1(w);
 n=AN(w); t=AT(w);
 ASSERT(!n||t&NUMERIC+BOX,EVDOMAIN);
 if(!n||t&NUMERIC)R 2>=n?qq(2==n?tail(w):cvt(n?t:B01,num(0)),zeroionei(0)):fpolyc(behead(tymes(w,IX(n))));
 x=AAV(w)[0];
 if(1<n||1>=AR(x))R dpoly(poly1(w));
 ASSERT(2==AR(x)&&2==AS(x)[1],EVDOMAIN);
 c=irs1(x,0L,1L,jthead);
 e=irs1(x,0L,1L,jttail);
 R amp(box(stitch(tymes(c,e),minus(e,zeroionei(1)))),ds(CPOLY));
}

static F1(jtipoly){A b,c,e,p=0,q=0,x;I n,t;
 ARGCHK1(w);
 n=AN(w); t=AT(w);
 ASSERT(!n||t&NUMERIC+BOX,EVDOMAIN);
 if(!n||t&NUMERIC)R fpolyc(over(num(0),divide(w,apv(n,1L,1L))));
 x=AAV(w)[0];
 if(1<n||1>=AR(x))R ipoly(poly1(w));
 ASSERT(2==AR(x)&&2==AS(x)[1],EVDOMAIN);
 RZ(c=irs1(x,0L,1L,jthead));
 RZ(e=plus(zeroionei(1),irs1(x,0L,1L,jttail)));
 RZ(b=ne(e,zeroionei(0)));
 if(!all0(b))RZ(p=amp(box(repeat(b,stitch(divide(c,e),e))),ds(CPOLY)));
 if(!all1(b))RZ(q=evc(not(b),c,"(+/x#y)&*@^."));
 R p&&q?folk(p,ds(CPLUS),q):p?p:q;
}

static F1(jticube){R atco(eval("* =/~@(i.@$)"),w);}

static F1(jtdiffamp0){A f,g,h,x,y;B nf,ng;C id;V*v;
 ARGCHK1(w);
 v=VAV(w);
 f=v->fgh[0]; nf=1&&NOUN&AT(f);
 g=v->fgh[1]; ng=1&&NOUN&AT(g);
 h=nf?g:f; id=ID(h); x=nf?f:g;   // h is the verb, id is its id; x is the noun
 if(!(!AR(x)||id==CPOLY))R 0;
 switch(id){
  case CPLUS:  R qq(num(1),zeroionei(0));
  case CSTAR:  R qq(x,zeroionei(0));
  case CMINUS: R qq(num(nf?-1:1),zeroionei(0));
  case CDIV:   R nf?eva(x,"(-x)&%@*:"):qq(recip(x),zeroionei(0));
  case CPOLY:  if(nf)R dpoly(x); break;
  case CBANG:  if(nf&&!AR(x))R dpoly(df1(iota(increm(x)),tdot(w))); break;
  case CROOT:  if(nf&&!AR(x))R atop(amp(recip(x),ds(CSTAR)),amp(ds(CEXP),decrem(recip(x)))); break;
  case CLOG:   R eva(logar1(x),nf?"(%x)&%":"(-x)&%@(* *:@^.)");
  case CEXP:
   if(nf)R evc(x,w,"(^.x)&*@y");
   RZ(y=pcvt(INT,x));
   if(INT&AT(y))switch(*AV(y)){
    case 0:    R qq(num(0),zeroionei(0));
    case 1:    R qq(num(1),zeroionei(0));
    case 2:    R ds(CPLUSCO);
   }
   R eva(x,"x&*@(^&(x-1))");
  case CCIRCLE:
   if(nf){
    RZ(x=vi(x));
    switch(*AV(x)){
     case 0:   R folk(ds(CMINUS),ds(CDIV),w);
     case 1:   R amp(num(2),h);
     case 2:   R atop(ds(CMINUS),amp(num(1),h));
     case 3:   R atop(atop(ds(CDIV),ds(CSTARCO)),amp(num(2),h));
     case 5:   R amp(num[6],h);
     case 6:   R amp(num(5),h);
     case 7:   R atop(atop(ds(CDIV),ds(CSTARCO)),amp(num[6],h));
 }}}
 R A0;
}

static F1(jtdiff0){A df,dg,dh,f,g,h,x,y,z;B b,nf,ng,vf,vg;C id;I m,p,q;V*v;
 ARGCHK1(w);
 v=VAV(w); id=v->id;
 f=v->fgh[0]; nf=f&&NOUN&AT(f); vf=f&&!nf;  // nf=f is a noun vf=f is a verb same for ng, vg
 g=v->fgh[1]; ng=g&&NOUN&AT(g); vg=g&&!ng;
 if(id==CAMP&&nf!=ng)R diffamp0(w);
 switch(id){
  case CLE:
  case CGE:
  case CLEFT:
  case CRIGHT:    R qq(num(1),zeroionei(0));
  case CPLUSCO:   R qq(num(2),zeroionei(0));
  case CNOT:
  case CMINUS:    R qq(num(-1),zeroionei(0));
  case CFCONS:    R qq(num(0),zeroionei(0));
  case CSTARCO:   R ds(CPLUSCO);
  case CHALVE:    R qq(connum(3L,"1r2"),zeroionei(0));
  case CCIRCLE:   R qq(pie,zeroionei(0));
  case CDIV:      R eval("- @%@*:");
  case CSQRT:     R eval("-:@%@%:");
  case CEXP:      R w;
  case CLOG:      R ds(CDIV);
  case CJDOT:     R qq(a0j1,zeroionei(0));
  case CRDOT:     R atop(ds(CJDOT),w);
  case CDDOT:     if(vf&&ng)R ddot(f,increm(g)); break;
  case CPOWOP:    
   if(vf&&ng&&!AR(g))switch(p=i0(g)){
    case -1: R diff0(inv(f));
    case  0: RE(0); R qq(num(1),zeroionei(0));
    case  1: R diff0(f);
    default:
     if(0>p){RZ(f=inv(f)); p=-p;}
     if(q=ispoly(f)){RE(m=i0(vib(expn2(sc(q-1),g)))); R dpoly(df1(IX(1+m),tdot(w)));}
     R diff0(atop(powop(f,sc(p-1),0),f));
   }
   break;
  case CQQ:
   if(!AR(f)&&NUMERIC&AT(f)&&ng&&equ(g,zeroionei(0)))R qq(num(0),zeroionei(0));
   if(vf&&ng)R qq(diff0(f),g);
   break;
  case CAT:
  case CATCO:
  case CAMP:
  case CAMPCO:
   if(vf&&vg){
    p=ispoly(f); q=ispoly(g);
    if(p&&q)R dpoly(df1(IX(1+(p-1)*(q-1)),tdot(w)));
    if(!(dg=diff0(g)))R A0; if(!(df=diff0(f)))R A0; v=VAV(df); x=v->fgh[0]; 
    if(CQQ!=v->id)R ftymes(dg,atop(df,g));
    switch(CQQ==v->id&&AT(x)&B01+INT?i0(x):9){
     case 0:      R df;
     case 1:      R dg;
     case 2:      R atop(ds(CPLUSCO),dg);
     case -1:     R fnegate(dg);
     default:     R ftymes(df,dg);
   }}
   break;
  case CTILDE:
   if(vf)switch(ID(f)){
    case CPLUS:   R qq(num(2),zeroionei(0));
    case CSTAR:   R ds(CPLUSCO);
    case CMINUS:
    case CLOG:
    case CDIV:    R qq(num(0),zeroionei(0));
    case CEXP:    R eva(w,"x * >:@^.");
   }
   break;
  case CFORK:
   h=v->fgh[2];
   if(NOUN&AT(f))R diff0(folk(qq(f,zeroionei(0)),g,h));
   if(CCAP==ID(f))R diff0(atco(g,h));
   p=ispoly(f); if(!(df=diff0(f)))R A0;
   q=ispoly(h); if(!(dh=diff0(h)))R A0; b=p&&q;
   switch(ID(g)){
    case CPLUS:   z=fplus (df,dh); R b?fpoly(MAX(p,q)-1,z):z;
    case CMINUS:  z=fminus(df,dh); R b?fpoly(MAX(p,q)-1,z):z;
    case CSTAR:   z=fplus(ftymes(df,h),ftymes(f,dh)); R b?fpoly(p+q,z):z;
    case CCOMMA:  R folk(df,g,dh);
    case CDIV:    x=fminus(ftymes(df,h),ftymes(f,dh));
                  y=atop(ds(CSTARCO),h);
                  R folk(b?fpoly(p+q-1-(I )(p==q),x):x,ds(CDIV),q?fpoly(q+q,y):y);
    case CEXP:    if(1==q){A k;
                   RZ(k=df1(num(0),h));
                   if(equ(k,zeroionei(0)))R qq(num(0),zeroionei(0));
                   if(equ(k,zeroionei(1)))R df;
                   if(equ(k,num(2)))R ftymes(df,ftymes(h,f));
                   R ftymes(df,ftymes(h,folk(f,g,qq(decrem(k),zeroionei(0)))));
                  }
 }}
 R A0;
}

// integrals of f&g, where exactly one of f,g is a noun
static F1(jtintgamp0){A f,g,h,x,y;B nf,ng;C id;V*v;
 ARGCHK1(w);
 v=VAV(w);
 f=v->fgh[0]; nf=1&&NOUN&AT(f);  // nf means f is a noun, ng means g is a noun
 g=v->fgh[1]; ng=1&&NOUN&AT(g);
 h=nf?g:f; id=ID(h); x=nf?f:g; // h is the verb operand, x is the noun
 if(!(!AR(x)||id==CPOLY))R A0;  // give up if noun is not an atom, except for p.
 switch(id){
  case CPLUS:  R ipoly(over(x,zeroionei(1)));
  case CSTAR:  R ipoly(over(zeroionei(0),x));
  case CMINUS: R nf?ipoly(over(x,num(-1))):ipoly(over(negate(x),zeroionei(1)));
  case CDIV:   R nf?eva(x,"x&*@^."):ipoly(over(num(0),recip(x)));
  case CPOLY:  if(nf)R ipoly(x); break;
  case CBANG:  if(nf&&AT(x))R ipoly(df1(iota(increm(x)),tdot(w))); break;
  case CEXP:
   if(ng){  //  ^&x  x must be an atom here
    if(equ(x,num(-1)))R ds(CLOG);   // ^_1 => log
    RZ(y=pcvt(INT,x));
    // if y is an INT and value > 0, integrate as polynomial (ipoly), else do y^(a+1) / (a+1).
    // For non-positive integer case, J representation is %&(a+1)@(^&(a+1))
    // We could just use y^(a+1) / (a+1) for all cases, but the ipoly form is better to feed into further symbolic processing
    R ((INT&AT(y))&&i0(y)>0)? ipoly(take(sc(-1-i0(y)),num(1))) : atop(amp(ds(CDIV),increm(y)),amp(ds(CEXP),increm(y)));
   } else {  // x&^
    /* e.g. a^y for some y (i.e. an exponential without base e). 
       Integrate as exp(log(a) * y). If a is negative
       the result will be complex.

       J example: 
          (2&^) d. _1 
       %&(^.2)@(2&^)
     
       For the case where a=0, this will cause an error.*/
    if(equ(x,zeroionei(0)))R A0;  // 0^h gives domain error
    R atop(amp(ds(CDIV),logar1(x)), amp(x,ds(CEXP))); // J: %&(^. a)@(a&^)
   }
  case CCIRCLE:
   if(nf){
    RZ(x=vi(x));
    switch(*AV(x)){
     case 1:   R atop(ds(CMINUS),amp(num(2),h));
     case 2:   R amp(num(1),h);
     case 3:   R eval("-@^.@(2&o.)");
     case 5:   R amp(num[6],h);
     case 6:   R amp(num(5),h);
     case 7:   R atop(ds(CLOG),amp(num[6],h));
 }}}
 R A0;
}

static F1(jtintg0); 

static F2(jtintgatop){A df,f=a,g=w,q,x,y;I m,n;V*v;
 ARGCHK2(a,w);
 m=ispoly(f);
 n=ispoly(g);
 if(m&&n)R ipoly(df1(IX(1+(m-1)*(n-1)),tdot(atop(a,w))));
 if(2==m){
  RZ(q=v2(0L,1L));
  RZ(x=df1(q,tdot(f)));
  RZ(y=equ(zeroionei(1), tail(x))?intg0(g):atop(fpolyc(tymes(q,x)),intg0(g)));
  R    equ(zeroionei(0),head(x))?y       :folk(y,ds(CPLUS),amp(head(x),ds(CSTAR)));
 }
 if(1==n||2==n){
  df=atop(intg0(f),g); 
  if(1==n)R df;
  RZ(x=df1(num(1),tdot(g)));
  R equ(x,zeroionei(1))?df:atop(amp(ds(CDIV),x),df);
 }
 v=VAV(g);
 if(m&&equ(take(sc(-m),num(1)),df1(IX(m),tdot(f)))){  /* ^&m @ g */
  if(CLOG==v->id)R 1==m?ds(CRIGHT):2==m?intg0(g):eva(sc(m-1),"(] * ^&x@^.) - x&* @(^&(x-1)@^. d. _1)");
  if(CAMP==v->id&&CCIRCLE==ID(v->fgh[1])&&(y=v->fgh[0],!AR(y)&&equ(y,floor1(y)))){
   if(2>=m)R 1==m?ds(CRIGHT):intgamp0(g);
   switch(i0(y)){
    case 1: R eva(sc(m-1),"%&(-x )@(^&(x-1)@(1&o.) * 2&o.) + ((x-1)%x)&*@(^&(x-2)@(1&o.) d. _1)");
    case 2: R eva(sc(m-1),"%&x    @(^&(x-1)@(2&o.) * 1&o.) + ((x-1)%x)&*@(^&(x-2)@(2&o.) d. _1)");
    case 3: R eva(sc(m-1),"%&(x-1)@(^&(x-1)@(3&o.)       ) -              ^&(x-2)@(3&o.) d. _1 ");
    case 7: R eva(sc(m-1),"%&(1-x)@(^&(x-1)@(7&o.)       ) +              ^&(x-2)@(7&o.) d. _1 ");
 }}}
 R 0;
}    /* integral of a @ w */

static F2(jtintgtymes){A f=a,g=w;
 ARGCHK2(a,w);
 R A0;
}    /* integral of a * w */

static F1(jtintg0){A df,dh,f,g,h;B nf,ng,vf,vg;C id;I m,n,p,q;V*fv,*gv,*v;
 ARGCHK1(w);
 id=ID(w); v=VAV(w);
 f=v->fgh[0]; nf=f&&NOUN&AT(f); if(vf=f&&!nf)fv=VAV(f);
 g=v->fgh[1]; ng=g&&NOUN&AT(g); if(vg=g&&!ng)gv=VAV(g);
 if(id==CAMP&&nf!=ng)R intgamp0(w);
 switch(id){
  case CLE:       R ipoly(v2(-1L, 1L));
  case CGE:       R ipoly(v2( 1L, 1L));
  case CLEFT:     
  case CRIGHT:    R ipoly(v2( 0L, 1L));
  case CNOT:      R ipoly(v2( 1L,-1L));
  case CMINUS:    R ipoly(v2( 0L,-1L));
  case CPLUSCO:   R ds(CSTARCO);
  case CFCONS:    R amp(v->fgh[2],ds(CSTAR));
  case CSTARCO:   R ipoly(over(v2(0L,0L),zeroionei(1)));
  case CHALVE:    R ipoly(over(num(0),scf((D)0.5)));
  case CCIRCLE:   R ipoly(over(num(0),scf(PI    )));
  case CDIV:      R ds(CLOG);
  case CSQRT:     R eval("%: * (0 2%3)&p.");
  case CEXP:      R w;
  case CLOG:      R eval("(]*^.) - ]");
  case CJDOT:     R ipoly(over(num(0),a0j1));
  case CRDOT:     R eval("-@j.@r.");
  case CDDOT:     if(vf&&ng)R ddot(f,decrem(g)); break;
  case CPOWOP:    
   if(vf&&ng&&!AR(g))switch(p=i0(g)){
    case -1: R intg0(inv(f));
    case  0: RE(0); R ipoly(v2(0L,1L));
    case  1: R intg0(f);
    default:
     if(0>p){RZ(f=inv(f)); p=-p;}
     if(q=ispoly(f)){RE(m=i0(vib(expn2(sc(q-1),g)))); R ipoly(df1(IX(1+m),tdot(w)));}
     R intg0(atop(powop(f,sc(p-1),0),f));
   }
   break;
  case CQQ:
   if(!AR(f)&&NUMERIC&AT(f)&&ng&&equ(g,zeroionei(0)))R amp(f,ds(CSTAR));
   if(vf&&ng)R qq(intg0(f),g);
   break;
  case CAT:
  case CATCO:
  case CAMP:
  case CAMPCO:
   if(vf&&vg)R intgatop(f,g);
   break;
  case CTILDE:
   if(vf)switch(ID(f)){
    case CPLUS:   R ipoly(v2(0L,2L));
    case CSTAR:   R ipoly(over(v2(0L,0L),zeroionei(1)));
    case CMINUS:  R FCONS(num(0));
    case CLOG:
    case CDIV:    R ds(CRIGHT);
   }
   break;
  case CFORK:
   h=v->fgh[2]; 
   if(NOUN&AT(f))R intg0(folk(qq(f,zeroionei(0)),g,h));
   dh=intg0(h); n=ispoly(h);
   df=intg0(f); m=ispoly(f);
   switch(ID(g)){
    case CPLUS:   R m&&n ? (p=MAX(m,n),ipoly(df1(IX(p),tdot(w)))) :  fplus(df,dh);
    case CMINUS:  R m&&n ? (p=MAX(m,n),ipoly(df1(IX(p),tdot(w)))) : fminus(df,dh);
    case CSTAR:   if(m&&n){p=2+(m-1)*(n-1); R ipoly(df1(IX(p),tdot(w)));}
                  R intgtymes(f,h);
 }}
 R 0;
}

static DF1(jtddot1){V*v=VAV(self); R df1(w,ddot(fix(v->fgh[0],zeroionei(0)),v->fgh[1]));}

F2(jtddot){A x,*xv,y,z;AF f;I j,n,p,q,r,*wv;
 ARGCHK2(a,w);
 ASSERT(NOUN&AT(w),EVDOMAIN);
 RZ(w=vi(w)); r=AR(w); n=AN(w); wv=AV(w);
 if(NOUN&AT(a)){ASSERT(0,EVNONCE);}
 if(!nameless(a)||1<r)R CDERIV(CDDOT, jtddot1,0L, 0L, 0L,0L,0L);
 CR rng = condrange(wv,n,IMAX,IMIN,IMAX-1); p=rng.min; q=rng.range;
 if(!r){
  if(!p){V*v=VAV(a); R v->mr||v->lrr?qq(a,zeroionei(0)):a;}
  f=0<=p?jtdiff0:jtintg0; y=a; DQ(ABS(p), ASSERT(y=CALL1(f,y,0L),EVDOMAIN);); R y;
 }
 q+=p-1; p=0>p?p:0; q=0<q?q:0;
 GATV0(x,BOX,1+q-p,1); xv=AAV(x); xv[-p]=incorp(a);
 if(0>p){y=a; j=-p; DQ(-p, ASSERT(y=intg0(y),EVDOMAIN); xv[--j]=incorp(y););}
 if(0<q){y=a; j=-p; DQ( q, ASSERT(y=diff0(y),EVDOMAIN); xv[++j]=incorp(y););}
 j=n; z=xv[wv[--j]-p]; DQ(n-1, RZ(z=folk(xv[wv[--j]-p],ds(CCOMMA),z)););
 R qq(z,zeroionei(0));
}


static F1(jtdiffamp){A f,g,h,x,y;B nf,ng;V*v;
 ARGCHK1(w);
 v=VAV(w);
 f=v->fgh[0]; nf=1&&NOUN&AT(f);
 g=v->fgh[1]; ng=1&&NOUN&AT(g);
 h=nf?g:f; x=nf?f:g;
 switch(ID(h)){
  case CROT:
  case CCANT:
  case CLBRACE:
  case CATOMIC:
  case CCYCLE:  
   if(nf)R atop(hook(eval("=/"),w),eval("i.@$"));
   break;
  case CPOLY:
   if(nf&&1>=AR(x))R dpoly(NUMERIC&AT(x)?x:poly1(x));
   break;
  case CBANG:
   if(!AR(x)&&(x=pcvt(INT,x),INT&AT(x)))R dpoly(df1(IX(1+*AV(x)),tdot(w)));
   break;
  case CFIT:
   if(nf&&1>=AR(x)&&(y=VAV(h)->fgh[0],CPOLY==ID(y)))R dpoly(df1(IX(IC(x)),tdot(w)));
  }
  R A0;
}

static F1(jtdiff){A df,dh,f,g,h,z;B nf,ng,vf,vg;C id;I r;V*v;
 ARGCHK1(w);
 ASSERT(VERB&AT(w),EVDOMAIN);
 v=VAV(w); id=v->id; r=v->mr;
 f=v->fgh[0]; nf=f&&NOUN&AT(f); vf=f&&!nf;
 g=v->fgh[1]; ng=g&&NOUN&AT(g); vg=g&&!ng;
 if(nf&&id==CFORK)R diff(folk(qq(f,ainf),g,v->fgh[2]));
 if(z=diff0(w))R id==CQQ&&ng&&equ(g,zeroionei(0))?z:icube(z);
 if(id==CAMP&&nf!=ng)R diffamp(w);
 switch(id){
  case CREV:     R eval("(|.=/])@(i.@$)");
  case CCANT:    R eval("(|:=/])@(i.@$)");
  case CHGEOM:   R hgdiff(w);
  case CSLASH:
   switch(ID(f)){
    case CPLUS:  R eval("({. =/ */@}.@$ | ])@(i.@$)");
   }
   break;
  /* ----- commented out because it is incorrect  
  case CBSLASH:
  case CBSDOT:
   if(CSLASH==ID(f)&&(ff=VAV(f)->fgh[0],ff&&VERB&AT(ff))){
    b=id==CBSDOT;
    switch(ID(ff)){
     case CPLUS: R eval(b ? "<:/~@(i.@$)" : ">:/~@(i.@$)");
   }}
   break;
  ----- */
  case CFCONS:
   R atop(amp(ds(CDOLLAR),num(0)),ds(CDOLLAR));
  case CQQ:
   if(NUMERIC&AT(f)&&ng){
    z=atop(amp(ds(CDOLLAR),num(0)),ds(CDOLLAR));
    R RMAX<mr(w)?z:qq(z,g);
   }
   if(vf&&ng)R qq(diff(f),g);
   break;
  case CAT:
  case CAMP:
   if(vf&&ng)R qq(df1(g,f),ainf);
   if(vf&&vg)R folk(diff(g),eval("+/ .*"),atop(diff(f),g));
   break;
  case CFORK:
   df=diff(f); h=v->fgh[2]; dh=diff(h);
   switch(ID(g)){
    case CPLUS:  R fplus(df,dh);
    case CMINUS: R fminus(df,dh);
    case CSTAR:  R fplus(ftymes(df,h),ftymes(f,dh));
    case CDIV:   R folk(fminus(ftymes(df,h),ftymes(f,dh)), ds(CDIV), atop(ds(CSTARCO),h));
 }}
 R A0;
}

static F1(jtintg){ASSERT(0,EVNONCE);}

static A jtdtab(J jt,A a,I d){A h;V*v;
 if(!a)R A0;  // return 0 if not found in table
 if(CDCAP==ID(a)&&(v=VAV(a),NOUN&AT(v->fgh[0])&&d==i0(v->fgh[1]))){h=VAV(a)->fgh[2]; R*(1+AAV(h));}
 switch(SGN(d)){
  default: ASSERTSYS(0,"dtab");
  case -1: R dtab(intg(a),d+1);
  case  0: R a;
  case  1: R dtab(diff(a),d-1);
}}


static DF2(jtsslope){A fs,f0,p,y,z,*zv;I m,n,r,t;V*sv=VAV(self);
 PREF2(jtsslope);
 fs=sv->fgh[0]; m=*AV(sv->fgh[1]);
 RZ(fs=1<m?dcapco(fs,sc(m-1)):atop(fs,ds(CRIGHT)));
 r=AR(a); n=AN(w);
 ASSERT(!r||r==AR(w)&&!memcmp(AS(a),AS(w),r*SZI),EVNONCE);
 RZ(f0=df2(a,w,fs));
 GATV(z,BOX,n,AR(w),AS(w)); zv=AAV(z);
 t=CMPX&AT(a)||CMPX&AT(w)?CMPX:FL;
 RZ(a=cvt(t,a)); RZ(y=cvt(t,w)); GA(p,t,1,0,0);
 if(t&CMPX){Z*av=ZAV(a),e,*pv=ZAV(p),*v=ZAV(y),x;
  e.re=1e-7; e.im=0.0; *pv=ZNZ(*av)?*av:e; 
  DO(n, if(r)*pv=ZNZ(av[i])?av[i]:e; x=v[i]; v[i]=zplus(v[i],*pv); RZ(zv[i]=incorp(divide(minus(df2(p,y,fs),f0),p))); v[i]=x;);
 }else     {D*av=DAV(a),e,*pv=DAV(p),*v=DAV(y),x;
  e=1e-7;              *pv=    *av ?*av:e;
  DO(n, if(r)*pv=    av[i] ?av[i]:e; x=v[i]; v[i]+=*pv;            RZ(zv[i]=incorp(divide(minus(df2(p,y,fs),f0),p))); v[i]=x;);
 }
 R ope(z); /* cant2(IX(AR(w)),ope(z)); */
}

static DF1(jtderiv1){A e,ff,fs,gs,s,t,z,*zv;I*gv,d,n,*tv;V*v;
 PREF1(jtderiv1);
 v=VAV(self); RZ(fs=fix(v->fgh[0],zeroionei(0))); gs=v->fgh[1]; n=AN(gs); gv=AV(gs); 
 if(!(AT(w)&FL+CMPX))RZ(w=cvt(FL,w));
 RZ(e=scf((D)1e-7));
 GAT0(t,INT,1,0); tv=AV(t);   // no need to INCORP t, since no one cares and it's not virtual
 RZ(s=ca(self)); v=VAV(s); v->fgh[1]=t; v->lrr=(v->lrr&RMAX)+(v->mr<<RANKTX);
 GATV(z,BOX,n,AR(gs),AS(gs)); zv=AAV(z);
 DO(n, *tv=d=gv[i]; zv[i]=incorp((ff=dtab(fs,d))?df1(w,ff):sslope(tymes(e,w),w,s)););
 RE(0); R ope(z);
}

F2(jtdcap){A z;I r,*v;
 ARGCHK2(a,w);
 ASSERT(NOUN&AT(w),EVDOMAIN);
 RZ(w=vi(w)); v=AV(w); DO(AN(w), ASSERT(0<=v[i],EVNONCE););
 if(NOUN&AT(a))R vger2(CDCAP,a,w);
 r=mr(a);
 R !AR(w)&&nameless(a)&&(z=dtab(a,*v))?z:CDERIV(CDCAP,jtderiv1,0L,VFLAGNONE,r,0L,r);
}

F2(jtdcapco){I r,*v;
 ASSERTVN(a,w);
 RZ(w=vi(w)); v=AV(w); DO(AN(w), ASSERT(0<=v[i],EVNONCE););
 r=mr(a);
 R CDERIV(CDCAPCO,0L,jtsslope,VFLAGNONE,0L,r,r);
}
#endif
