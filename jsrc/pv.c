/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Parsing: Tacit Verb Translator (13 : )                                  */

#include "j.h"
#include "p.h"

#define WTA         2     /* sizeof(TA)/sizeof(A) */
#define NTTAB       20
#define TC          5485900

#define CHK1        (!(stack[b].t)                          )
#define CHK2        (!(stack[b].t||stack[e].t)              )
#define CHK3        (!(stack[b].t||stack[1+b].t||stack[e].t))
#define CP          ds(CCAP)
#define DCASE(x,y)  (6*(x)+(y))
#define FGL(v)      folknohfn(v->fgh[2]?v->fgh[0]:CP,v->fgh[2]?v->fgh[1]:v->fgh[1-1],ds(CLEFT ))
#define FGR(v)      folknohfn(v->fgh[2]?v->fgh[0]:CP,v->fgh[2]?v->fgh[1]:v->fgh[1-1],ds(CRIGHT))
#define LF          ds(CLEFT )
#define RT          ds(CRIGHT)
#define RZZ(exp)    {if(!(exp))R zz;}


static const TA zz={0,0};

// Returns AM(z) 1 if monad, 0 if dyad
static F1(jtvtokens){A t,*y,z;I n,*s;TA*x;
 RZ(t=tokens(vs(w),0)); n=AN(t); y=AAV(t);
 I tmonad=1; 
 GATV0(z,BOX,WTA*(5+n),2); s=AS(z); *s++=5+n; *s=WTA;
 x=(TA*)AV2(z);
 x->a=mark; x->t=0; ++x;
 DQ(n, x->a=t=QCWORD(*y++); x->t=0; ++x; if(AT(t)&NAME&&NAV(t)->flag&NMDOT&&NAV(t)->s[0]=='x')tmonad=0;);  // clear monad flag if x seen
 x->a=mark; x->t=0; ++x;
 x->a=mark; x->t=0; ++x;
 x->a=mark; x->t=0; ++x;
 x->a=mark; x->t=0; ++x;
 AM(z)=tmonad;
 R z;
}    /* build string sentence into queue suitable for parsing */

static F1(jtcfn){I j; R !AR(w)&&INT&AT(w)&&(j=AV(w)[0],-9<=j&&j<=9)?FCONS(w):qq(w,ainf);}
     /* constant function with value w */

static A jttine(J jt,A w,I tmonad){V*v; R w&&AT(w)&VERB&&tmonad&&(v=FAV(w),v->id==CFORK&&(v->fgh[2]?v->fgh[0]:CP)==CP&&RT==(v->fgh[2]?v->fgh[2]:v->fgh[1]))?(v->fgh[2]?v->fgh[1]:v->fgh[0]):w;}
     /* if monad and w = [: g ], then g; else just w itself */

// type of special verb coded as n"_1 where n>TC; returns n-TC if special, otherwise _1
static I tvi(A w){A x;I i,z=-1;V*v;
 if(w&&VERB&AT(w)){
  v=FAV(w);
  if(CQQ==v->id&&num(-1)==v->fgh[1]){
   x=v->fgh[0]; 
   if(!AR(x)&&INT&AT(x)){i=AV(x)[0]-TC; z=0<=i&&i<NTTAB?i:-1;}
  }
 }
 R z;
}

static const C ctab[]={CEQ,CMIN,CMAX,CPLUS,CPLUSDOT,CPLUSCO,CSTAR,CSTARDOT,CSTARCO,CMATCH,CNE,0};

static F1(jtswapc){C c;
 if(!primitive(w))R swap(w);
 c=ID(w);
 R strchr(ctab,c)?w:c==CLT?ds(CGT):c==CGT?ds(CLT):c==CLE?ds(CGE):c==CGE?ds(CLE):swap(w);
}    /* w~ or equivalent */

TACT(jtvmonad){A fs;TA y,z={num(1),0};V*v;
 y=stack[e]; fs=stack[b].a;
 if(!y.t)df1(z.a,y.a,fs);
 else{
  v=FAV(y.t);
  if(!(CFORK==v->id&&0<=tvi(v->fgh[2]?v->fgh[2]:v->fgh[1])))z.t=folknohfn(CP,fs,tine(y.t));
  else if(NOUN&AT(v->fgh[0]))           z.t=folknohfn(CP,folknohfn(CP,fs,folknohfn(v->fgh[0],v->fgh[1],RT)),tine(v->fgh[2]));
  else                             z.t=folknohfn(tine(v->fgh[2]?v->fgh[0]:CP),folknohfn(CP,fs,v->fgh[2]?v->fgh[1]:v->fgh[0]),tine(v->fgh[2]?v->fgh[2]:v->fgh[1]));
 }
 R z;
}

static I jtdcase(J jt,I xi,V*v){
 R !v ? 0 : 0>xi ? 1 : CFORK!=v->id ? 2 : NOUN&AT(v->fgh[0]) ? 3 : v->fgh[2]==0 ? 4 : 5;
}    
/* 0   x        */
/* 1      f     */
/* 2         t  */
/* 3   x  f  t  */
/* 4   [: f  t  */
/* 5   s  f  t  */

TACT(jtvdyad){A fs,sf,xt,yt;B xl,xr,yl,yr;I xi=-1,yi=-1;TA x,y,z={num(1),0};V*u=0,*v=0;
 fs=stack[e-1].a; x=stack[b]; y=stack[e]; sf=swapc(fs);
 if(xt=tine(x.t)){xi=tvi(x.t); u=FAV(xt); if(0>xi&&CFORK==u->id){xi=tvi(u->fgh[2]?u->fgh[0]:CP); if(0>xi)xi=tvi(u->fgh[2]?u->fgh[2]:u->fgh[2-1]);}}
 if(yt=tine(y.t)){yi=tvi(y.t); v=FAV(yt); if(0>yi&&CFORK==v->id){yi=tvi(v->fgh[2]?v->fgh[0]:CP); if(0>yi)yi=tvi(v->fgh[2]?v->fgh[2]:v->fgh[2-1]);}}
 if(fs==ds(CLEFT)){if(xt)z.t=xt; else z.a=x.a; R z;}
 if(0>xi&&0>yi)switch((xt?2:0)+(yt?1:0)){
 case 0: df2(z.a,x.a,y.a,fs); break;
 case 1: z.t=folknohfn(x.a,fs,yt); break;
 case 2: z.t=folknohfn(y.a,sf,xt); break;
 case 3:
  xl=xt==LF; xr=xt==RT;
  yl=yt==LF; yr=yt==RT;
  if     (xl&&yr)            z.t=fs;
  else if(xr&&yl)            z.t=sf;
  else if(xr&&yr&&tmonad)z.t=swap(fs);
  else z.t=CFORK==u->id&&primitive(yt)?folknohfn(yt,sf,xt):folknohfn(xt,fs,yt);
 }else{B b,c;I i,j,xj,yj;
  i=dcase(xi,u); if(u&&CFORK==u->id){xi=tvi(u->fgh[2]?u->fgh[0]:CP); xj=tvi(u->fgh[2]?u->fgh[2]:u->fgh[2-1]);}else{xi=-1; xj=tvi(xt);}
  j=dcase(yi,v); if(v&&CFORK==v->id){yi=tvi(v->fgh[2]?v->fgh[0]:CP); yj=tvi(v->fgh[2]?v->fgh[2]:v->fgh[2-1]);}else{yi=-1; yj=tvi(yt);}
  z.t=0; b=xj==yj; c=xj==yi;
  switch(DCASE(i,j)){
  case DCASE(0,2): z.t=folknohfn(x.a,fs,yt); break;
  case DCASE(2,0): z.t=folknohfn(y.a,sf,xt); break;
  case DCASE(0,3): z.t=folknohfn(CP,folknohfn(x.a,fs,FGR(v)),v->fgh[2]?v->fgh[2]:v->fgh[2-1]); break;
  case DCASE(0,4): z.t=folknohfn(CP,folknohfn(x.a,fs,v->fgh[2]?v->fgh[1]:v->fgh[1-1]  ),v->fgh[2]?v->fgh[2]:v->fgh[2-1]); break;
  case DCASE(1,2): z.t=folknohfn(xt,fs,yt); break;
  case DCASE(1,3): 
  case DCASE(1,4): z.t=folknohfn(xt,folknohfn(LF,fs,FGR(v)),v->fgh[2]?v->fgh[2]:v->fgh[2-1]); break;
  case DCASE(2,1): z.t=folknohfn(xt,fs,yt); break;
  case DCASE(3,1): z.t=folknohfn(xt,fs,yt); break;
  case DCASE(4,1): z.t=folknohfn(xt,fs,yt); break;
  case DCASE(2,2): z.t=folknohfn(xt,fs,yt); break;
  case DCASE(2,3): z.t=b?folknohfn(CP,folknohfn(RT,    fs,FGR(v)),v->fgh[2]?v->fgh[2]:v->fgh[2-1]):folknohfn(xt,  folknohfn(LF,    fs,FGR(v)),v->fgh[2]?v->fgh[2]:v->fgh[2-1]); break;
  case DCASE(2,4): z.t=b?folknohfn(CP,folknohfn(RT,    fs,v->fgh[2]?v->fgh[1]:v->fgh[1-1]  ),v->fgh[2]?v->fgh[2]:v->fgh[2-1]):folknohfn(xt,  folknohfn(LF,    fs,FGR(v)),v->fgh[2]?v->fgh[2]:v->fgh[2-1]); break;
  case DCASE(3,2): z.t=b?folknohfn(CP,folknohfn(FGR(u),fs,RT    ),yt  ):folknohfn(u->fgh[2]?u->fgh[2]:u->fgh[2-1],folknohfn(FGL(u),fs,RT    ),yt  ); break;
  case DCASE(3,3): z.t=b?folknohfn(CP,folknohfn(FGR(u),fs,FGR(v)),v->fgh[2]?v->fgh[2]:v->fgh[2-1]):folknohfn(u->fgh[2]?u->fgh[2]:u->fgh[2-1],folknohfn(FGL(u),fs,FGR(v)),v->fgh[2]?v->fgh[2]:v->fgh[2-1]); break;
  case DCASE(3,4): z.t=b?folknohfn(CP,folknohfn(FGR(u),fs,v->fgh[2]?v->fgh[1]:v->fgh[1-1]  ),v->fgh[2]?v->fgh[2]:v->fgh[2-1]):folknohfn(u->fgh[2]?u->fgh[2]:u->fgh[2-1],folknohfn(FGL(u),fs,FGR(v)),v->fgh[2]?v->fgh[2]:v->fgh[2-1]); break;
  case DCASE(4,2): z.t=b?folknohfn(CP,folknohfn(u->fgh[2]?u->fgh[1]:u->fgh[1-1],  fs,RT    ),yt  ):folknohfn(u->fgh[2]?u->fgh[2]:u->fgh[2-1],folknohfn(FGL(u),fs,RT    ),yt  ); break;
  case DCASE(4,3): z.t=b?folknohfn(CP,folknohfn(u->fgh[2]?u->fgh[1]:u->fgh[1-1],  fs,FGR(v)),v->fgh[2]?v->fgh[2]:v->fgh[2-1]):folknohfn(u->fgh[2]?u->fgh[2]:u->fgh[2-1],folknohfn(FGL(u),fs,FGR(v)),v->fgh[2]?v->fgh[2]:v->fgh[2-1]); break;
  case DCASE(4,4): z.t=b?folknohfn(CP,folknohfn(u->fgh[2]?u->fgh[1]:u->fgh[1-1],  fs,v->fgh[2]?v->fgh[1]:v->fgh[1-1]  ),v->fgh[2]?v->fgh[2]:v->fgh[2-1]):folknohfn(u->fgh[2]?u->fgh[2]:u->fgh[2-1],folknohfn(FGL(u),fs,FGR(v)),v->fgh[2]?v->fgh[2]:v->fgh[2-1]); break;
  case DCASE(0,5):         z.t=folknohfn(v->fgh[2]?v->fgh[0]:CP,folknohfn(x.a,            fs,v->fgh[2]?v->fgh[1]:v->fgh[1-1]),v->fgh[2]?v->fgh[2]:v->fgh[2-1]); break; 
  case DCASE(2,5): if(b||c)z.t=folknohfn(v->fgh[2]?v->fgh[0]:CP,folknohfn(b?RT:LF,        fs,v->fgh[2]?v->fgh[1]:v->fgh[1-1]),v->fgh[2]?v->fgh[2]:v->fgh[2-1]); break; 
  case DCASE(3,5):
  case DCASE(4,5): if(b||c)z.t=folknohfn(v->fgh[2]?v->fgh[0]:CP,folknohfn(b?FGR(u):FGL(u),fs,v->fgh[2]?v->fgh[1]:v->fgh[1-1]),v->fgh[2]?v->fgh[2]:v->fgh[2-1]); break; 
  case DCASE(5,0):         z.t=folknohfn(u->fgh[2]?u->fgh[0]:CP,folknohfn(y.a,            sf,u->fgh[2]?u->fgh[1]:u->fgh[1-1]),u->fgh[2]?u->fgh[2]:u->fgh[2-1]); break; 
  case DCASE(5,2): if(b||c)z.t=folknohfn(u->fgh[2]?u->fgh[0]:CP,folknohfn(u->fgh[2]?u->fgh[1]:u->fgh[1-1],           fs,b?RT    :LF        ),yt  ); break; 
  case DCASE(5,3):
  case DCASE(5,4): if(b||c)z.t=folknohfn(u->fgh[2]?u->fgh[0]:CP,folknohfn(u->fgh[2]?u->fgh[1]:u->fgh[1-1],           fs,b?FGR(v):FGL(v)    ),v->fgh[2]?v->fgh[2]:v->fgh[2-1]); break; 
  case DCASE(5,5): if(xi==yi&&xj==yj||xi==yj&&xj==yi)
                     if(b||  (v->fgh[2]?v->fgh[1]:v->fgh[1-1])==swapc(v->fgh[2]?v->fgh[1]:v->fgh[1-1]))z.t=folknohfn(u->fgh[2]?u->fgh[0]:CP,folknohfn(u->fgh[2]?u->fgh[1]:u->fgh[1-1],fs,     v->fgh[2]?v->fgh[1]:v->fgh[1-1] ),u->fgh[2]?u->fgh[2]:u->fgh[2-1]);
                     else if((u->fgh[2]?u->fgh[1]:u->fgh[1-1])==swapc(u->fgh[2]?u->fgh[1]:u->fgh[1-1]))z.t=folknohfn(v->fgh[2]?v->fgh[0]:CP,folknohfn(u->fgh[2]?u->fgh[1]:u->fgh[1-1],fs,     v->fgh[2]?v->fgh[1]:v->fgh[1-1] ),v->fgh[2]?v->fgh[2]:v->fgh[2-1]);
                     else                      z.t=folknohfn(u->fgh[2]?u->fgh[0]:CP,folknohfn(u->fgh[2]?u->fgh[1]:u->fgh[1-1],fs,swap(v->fgh[2]?v->fgh[1]:v->fgh[1-1])),u->fgh[2]?u->fgh[2]:u->fgh[2-1]);
  }
  RZZ(z.t);
 }
 R z;
}
TACT(jtvadv ){TA z={0,0}; if(CHK1)df1(z.a,stack[b].a,stack[e].a); R z;}

TACT(jtvconj){TA z={0,0}; if(CHK2)df2(z.a,stack[b].a,stack[e].a,stack[e-1].a); R z;}

TACT(jtvfolk){TA z={0,0}; if(CHK3)z.a=folknohfn(stack[b].a,stack[1+b].a,stack[e].a); R z;}

TACT(jtvhook){TA z={0,0}; if(CHK2)z.a=hook(stack[b].a,stack[e].a,mark); R z;}

TACT(jtvpunc){R stack[e-1];}

TACT(jtvis){A ea,et,n,t;I j;TA*u,z={0,0};
 n=stack[b].a;
 if(!(NAME&AT(n)&&CASGN==CAV(stack[1+b].a)[0]))R z;
 t=sfn(0,n); j=*ttabi; u=ttab;
 if(!t||NTTAB==*ttabi)R z;
 DQ(j, if(equ(t,u->a))R z; ++u;);
 ea=stack[e].a; et=stack[e].t;
 symbisdel(n,ea,locsyms);
 ++*ttabi; u->a=t; u->t=et?et:cfn(ea);
 z.a=ea;  z.t=tsubst?qq(sc(TC+j),num(-1)):et;
 R z;
}

static TACT(jtvmove){A t;TA*u,x,z;
 x=stack[MAX(0,e)];
 if(!(NAME&AT(x.a))||ASGN&AT(stack[b].a))R x;
 z.a=nameref(x.a,locsyms); z.t=0; t=sfn(0,x.a); u=ttab;
 DO(*ttabi, if(equ(t,u->a)){z.t=tsubst&&ttabi0<=i?qq(sc(TC+i),num(-1)):u->t; break;} ++u;);
 R z;
}

/* final translator result                             */
/* modifies argument in place                          */
/* a. replaces 880i functions by ttab[i].t entries */
/* b. replaces n0"_ v1 v2 by n0 v1 v2                  */
/* c. replaces [: g ] by g, if monad                   */

#define ASGRCOMMON(w,x,s,test) ({A val=(s); test if(AFLAG(w)&RECURSIBLE){if(val)ra(val); if(FAV(w)->fgh[x]!=0){A t=FAV(w)->fgh[x]; FAV(w)->fgh[x]=0; fa(t);} } FAV(w)->fgh[x]=val; FAV(w)->fgh[x];})  // t for audits
#define ASGR(w,x,s) ASGRCOMMON(w,x,s,RZ(val))
#define ASGRZ(w,x,s) ASGRCOMMON(w,x,s,)  // no error

static A jtvfinal(J jt,A w,I tmonad,I tsubst,TA *ttab,I *ttabi,I ttabi0){I i;V*u,*v;
 ARGCHK1(w);
 if(!(VERB&AT(w)))R w;
 v=FAV(w);
 if(CFORK!=v->id){i=tvi(w); R 0<=i?jtvfinal(jt,ttab[i].t,tmonad,tsubst,ttab,ttabi,ttabi0):w;}
 I f=!!v->fgh[2];
 ASGR(w,2,incorp(tine(jtvfinal(jt,f?v->fgh[2]:v->fgh[2-1],tmonad,tsubst,ttab,ttabi,ttabi0))));
 ASGR(w,1,incorp(tine(jtvfinal(jt,f?v->fgh[1]:v->fgh[1-1],tmonad,tsubst,ttab,ttabi,ttabi0))));
 ASGR(w,0,incorp(tine(jtvfinal(jt,f?v->fgh[0]:CP,tmonad,tsubst,ttab,ttabi,ttabi0))));
 if(VERB&AT(v->fgh[2]?v->fgh[0]:CP)){
  u=FAV(v->fgh[2]?v->fgh[0]:CP); 
  if(CFCONS==u->id)ASGR(w,0,u->fgh[2]);  // must be incorped already
  else if(CQQ==u->id&&NOUN&AT(u->fgh[0])&&equ(ainf,u->fgh[1]))ASGR(w,0,u->fgh[0]);  // must be incorped already
  if(NOUN&AT(v->fgh[2]?v->fgh[0]:CP))RZ(w=folknohfn(v->fgh[2]?v->fgh[0]:CP,v->fgh[2]?v->fgh[1]:v->fgh[1-1],v->fgh[2]?v->fgh[2]:v->fgh[2-1]));
 }
 RZ(w=tine(w));
 if(FAV(w)->id==CFORK&&AT(FAV(w)->fgh[0])&VERB&&FAV(FAV(w)->fgh[0])->id==CCAP){ASGR(w,0,FAV(w)->fgh[1]); ASGR(w,1,FAV(w)->fgh[2]); ASGRZ(w,2,0);}
 R w;
}    

static A jttparse(J jt,A w,A locsyms,I tmonad,I tsubst,TA *ttab,I *ttabi,I ttabi0){A*s,t,x;C d;I b,*c,e,i,j,k,m,n;TA*stack;
 ARGCHK1(w);
 stack=(TA*)AV(w); n=m=AS(w)[0]-4;
 do{
  for(i=0;i<NCASES;i++){
   c=cases[i].c; s=(A*)(n+stack); d=1;
   d=d&&*c++&(AT(*s)&-AT(*s)); s+=WTA;
   d=d&&*c++&(AT(*s)&-AT(*s)); s+=WTA;
   d=d&&*c++&(AT(*s)&-AT(*s)); s+=WTA;
   d=d&&*c++&(AT(*s)&-AT(*s));
   if(d)break;
  }
  if(i<NCASES){
   b=cases[i].b; j=n+b;
   e=cases[i].e; k=n+e;
   stack[k]=(cases[i].vf)(jt,j,k,stack,locsyms,tmonad,tsubst,ttab,ttabi,ttabi0);
   if(!(stack[k].a))R A0;
   DQ(b,stack[--k]=stack[--j];); n=k;
  } else {stack[n-1]=jtvmove(jt,n,m-1,stack,locsyms,tmonad,tsubst,ttab,ttabi,ttabi0); RE(0); n-=0<m--;}
 } while(0<=m);
 x=stack[1+n].a; t=stack[1+n].t;
 ASSERT(NOUN&AT(x)&&MARK&AT(stack[2+n].a),EVSYNTAX);
 R t?jtvfinal(jt,t,tmonad,tsubst,ttab,ttabi,ttabi0):cfn(x);
}

F1(jtvtrans){PROLOG(0053);A locsyms,y,z=0;I c,i,ttabi;TA ttab[NTTAB];
 RZ(ttab[0].a=cstr("x")); ttab[0].t=ds(CLEFT);
 RZ(ttab[1].a=cstr("y")); ttab[1].t=RT; c=2;
 for(i=0;!z&&2>i;++i){
  RZ(y=vtokens(w));  // return AM bit0=monad
  I tmonad=AM(y);
  ttabi=c;
  RZ(locsyms=stcreate(2,40,0L,0L)); AR(locsyms)&=~ARLOCALTABLE;  // not necessary to set global pointers; flag table so we don't switch to locsyms during assignment
  symbis(mnuvxynam[5],num(1),locsyms); if(!tmonad)symbis(mnuvxynam[4],num(1),locsyms); 
  WITHMSGSOFF(z=jttparse(jt,y,locsyms,tmonad,0==i,ttab,&ttabi,c);)
// obsolete  RESETERR;
  if(i&&!z)z=colon(num(4-tmonad),w);
 }
 EPILOG(z);
}
