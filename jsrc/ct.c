/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Taylor's Series                                           */

#include "j.h"

#if 0
static F1(jttayatop);

static F1(jtcoeff){V*v;
 ARGCHK1(w);
 v=FAV(w);
 R VTAYFINITE&v->flag ? curtail(FAV(v->fgh[0])->fgh[1]) : mtv;
}    /* coefficents c in {&c@(n&<.), or empty */

static F1(jttpoly){A z;
 ARGCHK1(w);
 RZ(z=atop(amp(ds(CLBRACE),over(AT(w)&CMPX?w:xco1(w),num(0))),amp(tally(w),ds(CMIN))));
 FAV(z)->flag=VTAYFINITE;
 RETF(z);
}

static F1(jtfacit){A c;V*u,*v;
 RZ(c=coeff(w));
 if(AN(c))R tpoly(tymes(c,fact(AT(c)&XNUM+RAT?xco1(IX(IC(c))):IX(IC(c)))));
 v=FAV(w);
 if(CFORK==v->id)switch(ID(v->fgh[1])){
  case CDIV:
   if(CBANG==ID(v->fgh[2]))R v->fgh[0]; 
   break;
  case CSTAR:
   if(CFORK==ID(v->fgh[2])&&(u=FAV(v->fgh[2]),CDIV==ID(u->fgh[1])&&CBANG==ID(u->fgh[2])))R folk(v->fgh[0],v->fgh[1],u->fgh[0]);
   RZ(c=atop(ds(CDIV),ds(CBANG)));
   if(equ(c,v->fgh[0]))R v->fgh[2];
   if(equ(c,v->fgh[2]))R v->fgh[0];
 }
 R folk(ds(CBANG),ds(CSTAR),w);
}

static A jttayamp(J jt,A w,B nf,A x,A h){A y;B ng=nf^1;I j,n;V*v=FAV(h);
 ASSERT(AR(x)<=(nf?lrv(v):rrv(v)),EVRANK);
 switch(v->id){
  case CPLUS:  R tpoly(over(x,num(1)));
  case CMINUS: R tpoly(nf?over(x,num(-1)):over(negate(x),num(1)));
  case CSTAR:  R tpoly(over(num(0),x));
  case CDIV:   ASSERT(ng!=0,EVDOMAIN); R tpoly(over(num(0),recip(x)));
  case CJDOT:  R tpoly(nf?over(x,a0j1):over(jdot1(x),num(1)));
  case CPOLY:  ASSERT(nf!=0,EVDOMAIN); R tpoly(BOX&AT(x)?poly1(x):x);
  case CHGEOM: ASSERT(nf!=0,EVDOMAIN); RE(j=i0(x)); ASSERT(0<=j,EVDOMAIN);
               y=IX(j);
               R tpoly(divide(hgcoeff(y,h),fact(y)));
  case CBANG:  ASSERT(nf!=0,EVDOMAIN); RE(j=i0(x)); ASSERT(0<=j,EVDOMAIN); 
               R tpoly(divide(poly1(box(iota(x))),fact(x)));
  case CEXP:   if(nf)R eva(x,"(^.x)&^ % !");
               RE(n=i0(x));   
               R 0<=n?tpoly(over(reshape(x,num(0)),num(1))):atop(ds(CDIV),amp(h,sc(-n))); 
  case CFIT:   ASSERT(nf&&CPOLY==ID(v->fgh[0]),EVDOMAIN);
               y=over(x,IX(IC(x)));
               R tpoly(mdiv(df2(x,y,h),atab(CEXP,y,IX(IC(x)))));
  case CCIRCLE:
   switch(i0(x)){
    case 1:    R eval("{&0 1 0 _1@(4&|) % !");
    case -3:   R eval("{&0 1 0 _1@(4&|) % ]");
    case 2:    R eval("{&1 0 _1 0@(4&|) % !");
    case 5:    R eval("2&|    % !");
    case -7:   R eval("2&|    % ]");
    case 6:    R eval("2&|@>: % !");
    case -1:   R eval("(2&|              % ]) * ([: */ (1&+ % 2&+)@(i.@<.&.-:))\"0");
    case -5:   R eval("({&0 1 0 _1@(4&|) % ]) * ([: */ (1&+ % 2&+)@(i.@<.&.-:))\"0");
 }}
 ASSERT(0,EVDOMAIN);
}

static A jttcoamp(J jt,A w,B nf,A x,A h){I j;V*v=FAV(h);
 ASSERT(AR(x)<=v->mr,EVRANK);
 switch(v->id){
  case CEXP:   
   if(nf)R amp(logar1(x),ds(CEXP)); break;
  case CHGEOM: 
   ASSERT(nf!=0,EVDOMAIN); RE(j=i0(x)); ASSERT(0<=j,EVDOMAIN);
   R tpoly(hgcoeff(IX(j),h));
 }
 R facit(tayamp(w,nf,x,h));
}

static F2(jttayinv){A y;I m,*v;
 ARGCHK2(a,w);
 RZ(y=vip(w)); v=AV(y);
 m=0; DO(AN(w), m=MAX(m,v[i]);); ++m;
 RZ(y=IX(m)); if(AT(w)&XNUM+RAT)RZ(y=xco1(y));
 R rinv(ev2(apv(m,0L,-1L),df1(y,tdot(a)),"|.!.0\"0 1"));
}

static DF1(jttayrecip){A f=FAV(self)->fgh[0]; R from(w,head(tayinv(FAV(f)->fgh[1],w)));}
     /* %@f t. w */

static DF1(jttaydiv){A c,f,ft,h,ht,y;I j,m,*u;V*v;
 y=FAV(self)->fgh[0]; v=FAV(y);
 RZ(y=vip(w)); u=AV(y);
 m=0; DO(AN(w), m=MAX(m,u[i]);); ++m;
 RZ(y=IX(m)); if(AT(w)&XNUM+RAT)RZ(y=xco1(y));
 RZ(f=df1(y,ft=tdot(v->fgh[0])));
 RZ(h=df1(y,ht=tdot(v->fgh[2])));
 RZ(c=indexof(ne(zeroionei(0),h),num(1)));
 if(j=*AV(c)){
  ASSERT(all1(eq(zeroionei(0),take(c,f))),EVDOMAIN);
  RZ(y=apv(j,m,1L)); if(AT(w)&XNUM+RAT)RZ(y=xco1(y));
  RZ(f=over(drop(c,f),df1(y,ft)));
  RZ(h=over(drop(c,h),df1(y,ht)));
 }
 R from(w,pdt(f,rinv(ev2(apv(m,0L,-1L),h,"|.!.0\"0 1"))));
}    /* (f % h) t. w */

static DF1(jttaysqrt){A f;I m,*v;
 f=FAV(self)->fgh[0];
 RZ(w=vip(w)); v=AV(w);
 m=0; DO(AN(w), m=MAX(m,v[i]);); ++m;
 ASSERT(0,EVNONCE);
  /* R from(w,df2(sc(m),df1(IX(m),tdot(FAV(f)->fgh[1])),taysqrt0)); */
}    /* %:@f t. w */

static F1(jttayfolk){A c,d,f,ft,g,h,ht,pp;B b;V*v=FAV(w);
 h=v->fgh[2];                                                   ht=tdot(h); RZ(d=coeff(ht));
 f=v->fgh[0]; if(NOUN&AT(f))R tayfolk(folk(qq(f,ainf),v->fgh[1],h)); ft=tdot(f); RZ(c=coeff(ft));
 b=AN(c)&&AN(d); g=v->fgh[1]; pp=eval("[: +//. */");
 switch(ID(g)){
  case CPLUS:
  case CMINUS: R b ? tpoly(df1(lamin2(c,d),slash(g))) : folk(ft,g,ht);
  case CSTAR:  R b ? tpoly(df2(c,d,pp)) : eva(folk(ft,pp,ht),"{ x@(i.@>:@(>./)@,)");
  case CDIV:   R ADERIV(CTDOT,jttaydiv,0L,VISATOMIC1,RMAX,RMAX,RMAX);
  case CTILDE: g=FAV(g)->fgh[0]; ASSERT(VERB&AT(f),EVDOMAIN); R tayfolk(folk(h,g,f));
  case CEXP:   ASSERT(1==AN(d),EVDOMAIN); R tdot(atop(amp(g,head(d)),f));
  default:     ASSERT(0,EVDOMAIN);
}}

static F1(jttaysum){I n;V*v=FAV(w);
 switch(ID(w)){
  case CLEFT: case CRIGHT:
   R tpoly(eval("0 _1r2 1r2"));
  case CAMP:  case CAMPCO:
   if(CEXP==ID(v->fgh[0])){
    RE(n=i0(v->fgh[1]));
    ASSERT(0<=n,EVDOMAIN);

 }}
 ASSERT(0,EVDOMAIN);
}

static F1(jttayatop){A c,d,e,f,ft,g,gt,h;I k,m,n;V*v=FAV(w);
 f=v->fgh[0]; g=v->fgh[1];
 switch(ID(f)){
  case CDIV:  R ADERIV(CTDOT,jttayrecip,0L,VISATOMIC1,RMAX,RMAX,RMAX);
  case CSQRT: R ADERIV(CTDOT,jttaysqrt, 0L,VISATOMIC1,RMAX,RMAX,RMAX);
  case CAMP:
   c=FAV(f)->fgh[0]; d=FAV(f)->fgh[1];
   if(CEXP==ID(c)&&INT&AT(d)&&!AR(d)){
    k=*AV(d);
    if(0>k)R tayatop(atop(ds(CDIV),-1==k?g:atop(amp(c,sc(-k)),g)));
 }}
 if(CIOTA==ID(g)){B b;C c;
  c=ID(f); ft=FAV(f)->fgh[0]; gt=FAV(f)->fgh[1];
  if(c==CSLASH&&CPLUS==ID(ft))R taysum(ds(CLEFT));
  b=CSLASH==ID(ft)&&(h=FAV(ft)->fgh[0],CPLUS==ID(h));
  if(b&&(c==CAMPCO||c==CATCO||(c==CAMP||c==CAT)&&1<FAV(gt)->mr))R taysum(gt);
  ASSERT(0,EVDOMAIN);
 }
 ft=tdot(f); RZ(c=coeff(ft)); m=AN(c);
 gt=tdot(g); RZ(d=coeff(gt)); n=AN(d);
 if(n&&all1(eq(zeroionei(0),curtail(d)))){
  e=tail(d);
  if(!equ(zeroionei(1),e))ft=folk(amp(e,ds(CEXP)),ds(CSTAR),ft);
  if(2<n)ft=evc(sc(n-1),ft,"0&=@(x&|) * y@<.@(%&x)");
  R !m?ft:tpoly(1==n?df1(e,f):df1(IX(1+n*(m-1)),ft));
 }
 ASSERT(m!=0,EVDOMAIN);
 /* if(2==m&&equ(num(0),head(c)))R folk(tail(c),ds(CSTAR),gt)); */
 h=eval("4 : '+/x*y([:+//.*/)^:(i.#x) 1'");
 R AN(d)?tpoly(df2(c,d,h)):eva(atco(amp(c,h),gt),"[ { x@:(i.@>:@(>./)@,)");
}

static AS1(jttdot1, df1(w,tdot(fix(fs,zeroionei(0)))),0104)
static AS1(jttco1,  df1(w,tco (fix(fs,zeroionei(0)))),0105)
static CS1(jttcap1, df1(w,tcap(fix(fs,zeroionei(0)),gs)),0106)

static DF1(jttcoh1){R hgcoeff(w,FAV(self)->fgh[0]);}

F1(jttdot){A f,g,h;B nf,ng;C id;V*v;
 ARGCHK1(w);
 if(NOUN&AT(w))R vger2(CTDOT,0L,w);
 if(!nameless(w))R ADERIV(CTDOT,jttdot1,0L,VISATOMIC1,RMAX,RMAX,RMAX);
 v=FAV(w); id=v->id;
 f=v->fgh[0]; nf=f&&NOUN&AT(f);
 g=v->fgh[1]; ng=g&&NOUN&AT(g);
 h=v->fgh[2];
 if(id==CAMP&&nf!=ng)R tayamp(w,nf,nf?f:g,nf?g:f);
 switch(id){
  case CEXP:    R eval("%@!");
  case CLEFT:  
  case CRIGHT:  R tpoly(v2( 0L, 1L));
  case CGE:     R tpoly(v2( 1L, 1L));
  case CLE:     R tpoly(v2(-1L, 1L));
  case CNOT:    R tpoly(v2( 1L,-1L));
  case CMINUS:  R tpoly(v2( 0L,-1L));
  case CPLUSCO: R tpoly(v2( 0L, 2L));
  case CSTARCO: R tpoly(eval("0 0 1"));
  case CHALVE:  R tpoly(eval("0 0.5"));
  case CJDOT:   R tpoly(eval("0 0j1"));
  case CCIRCLE: R tpoly(eval("0 1p1"));
  case CFORK:   R tayfolk(w);
  case CHOOK:   R tayfolk(folk(ds(CRIGHT),f,g));
  case CHGEOM:  R folk(ds(CBANG),swap(ds(CDIV)),tco(w));
  case CUNDER:  R tdot(atop(inv(g),atop(f,g)));
  case CQQ:     if(nf){ASSERT(!AR(f),EVRANK); R tpoly(f);} else R tdot(f);
  case CEVEN:   if(CMINUS==ID(g))R eva(f,"0&=@(2&|) * x t."); break;
  case CODD:    if(CMINUS==ID(g))R eva(f,"     2&|  * x t."); break;
  case CTDOT:   if(nf)R*(1+AAV(h)); break;
  case CDDOT:   R tdot(ddot(f,g));
  case CAMP: case CAMPCO: case CAT:  case CATCO:
   R tayatop(w);
  case CPOWOP:  
   if(!nf&&ng&&!AR(g)){A c,d,ft,h;I m,n;
    RE(n=i0(g));
    switch(SGN(n)){
     case -1:   RZ(h=inv(f)); ASSERT(CPOWOP!=ID(h),EVDOMAIN); R tdot(powop(h,sc(-n),0));
     case  0:   R tpoly(v2(0L,1L));
     case  1:
      ft=tdot(f); RZ(c=d=coeff(ft)); m=AN(c);
      ASSERT(m!=0,EVDOMAIN);
      RZ(h=eval("4 : '+/x*y([:+//.*/)^:(i.#x) 1'"));
      DQ(n-1, RZ(d=df2(c,d,h)););
      R tpoly(d);
 }}}
 ASSERT(id==CFCONS,EVDOMAIN); 
 R tpoly(h);
}

F1(jttco){A f,g;B nf,ng;C id;V*v;
 ARGCHK1(w);
 ASSERT(VERB&AT(w),EVDOMAIN);
 if(!nameless(w))R ADERIV(CTCO,jttco1,0L,VFLAGNONE, RMAX,RMAX,RMAX);
 v=FAV(w); id=v->id;
 f=v->fgh[0]; nf=f&&NOUN&AT(f);
 g=v->fgh[1]; ng=g&&NOUN&AT(g);
 if(id==CAMP&&nf!=ng)R tcoamp(w,nf,nf?f:g,nf?g:f);
 switch(id){
  case CEXP:   R eval("$&1&$");
  case CHGEOM: R ADERIV(CTCO,jttcoh1,0L,VFLAGNONE, RMAX,RMAX,RMAX);
  case CQQ:    if(!nf)R tco(f);
 }
 R facit(tdot(w));
}

F2(jttcap){A c,p,s,t;I n;
 ASSERTVN(a,w);
 ASSERT(!AR(w),EVRANK);
 RZ(t=vib(w)); n=*AV(t);
 ASSERT(0<=n,EVDOMAIN);
 if(!nameless(a))R CDERIV(CTCAP,jttcap1,0L,VFLAGNONE, RMAX,RMAX,RMAX);
 RZ(p=tdot(a));
 if(n<IMAX)R amp(df1(iota(t),p),ds(CPOLY));
 RZ(c=coeff(p));
 if(AN(c))R amp(c,ds(CPOLY));
 RZ(s=cstr("3 : ('g=:p.&y@:(("));
 RZ(s=over(s,over(lrep(a),cstr(") t.)@i.';'g +:^:(g ~: g@+:)^:_ ] 1') \" 0"))));
 R eval(CAV(str0(s)));
}
#endif