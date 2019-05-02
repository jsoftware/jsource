/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
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
#define FGL(v)      folk(v->fgh[0],v->fgh[1],ds(CLEFT ))
#define FGR(v)      folk(v->fgh[0],v->fgh[1],ds(CRIGHT))
#define LF          ds(CLEFT )
#define RT          ds(CRIGHT)
#define RZZ(exp)    {if(!(exp))R zz;}


static TA zz={0,0};

static F1(jtvtokens){A t,*y,z;I n,*s;TA*x;
 RZ(t=tokens(vs(w),0)); n=AN(t); y=AAV(t);
 jt->tmonad=1; 
 GATV0(z,BOX,WTA*(5+n),2); s=AS(z); *s++=5+n; *s=WTA;
 x=(TA*)AV(z);
 x->a=mark; x->t=0; ++x;
 DO(n, x->a=t=*y++; x->t=0; ++x; if(AT(t)&NAME&&NAV(t)->flag&NMDOT&&NAV(t)->s[0]=='x')jt->tmonad=0;);
 x->a=mark; x->t=0; ++x;
 x->a=mark; x->t=0; ++x;
 x->a=mark; x->t=0; ++x;
 x->a=mark; x->t=0; ++x;
 R z;
}    /* build string sentence into queue suitable for parsing */

static F1(jtcfn){I j; R !AR(w)&&INT&AT(w)&&(j=*AV(w),-9<=j&&j<=9)?FCONS(w):qq(w,ainf);}
     /* constant function with value w */

static F1(jttine){V*v; R w&&jt->tmonad&&(v=FAV(w),CP==v->fgh[0]&&RT==v->fgh[2])?v->fgh[1]:w;}
     /* if monad and w = [: g ], then g; else just w itself */

static I tvi(A w){A x;I i,z=-1;V*v;
 if(w&&VERB&AT(w)){
  v=FAV(w);
  if(CQQ==v->id&&num[-1]==v->fgh[1]){
   x=v->fgh[0]; 
   if(!AR(x)&&INT&AT(x)){i=*AV(x)-TC; z=0<=i&&i<NTTAB?i:-1;}
 }}
 R z;
}

static C ctab[]={CEQ,CMIN,CMAX,CPLUS,CPLUSDOT,CPLUSCO,CSTAR,CSTARDOT,CSTARCO,CMATCH,CNE,0};

static F1(jtswapc){C c;
 if(!primitive(w))R swap(w);
 c=ID(w);
 R strchr(ctab,c)?w:c==CLT?ds(CGT):c==CGT?ds(CLT):c==CLE?ds(CGE):c==CGE?ds(CLE):swap(w);
}    /* w~ or equivalent */

TACT(jtvmonad){A fs;TA y,z={num[1],0};V*v;
 y=stack[e]; fs=stack[b].a;
 if(!y.t)z.a=df1(y.a,fs);
 else{
  v=FAV(y.t);
  if(!(CFORK==v->id&&0<=tvi(v->fgh[2])))z.t=folk(CP,fs,tine(y.t));
  else if(NOUN&AT(v->fgh[0]))           z.t=folk(CP,folk(CP,fs,folk(v->fgh[0],v->fgh[1],RT)),tine(v->fgh[2]));
  else                             z.t=folk(tine(v->fgh[0]),folk(CP,fs,v->fgh[1]),tine(v->fgh[2]));
 }
 R z;
}

static I jtdcase(J jt,I xi,V*v){
 R !v ? 0 : 0>xi ? 1 : CFORK!=v->id ? 2 : NOUN&AT(v->fgh[0]) ? 3 : CP==v->fgh[0] ? 4 : 5;
}    
/* 0   x        */
/* 1      f     */
/* 2         t  */
/* 3   x  f  t  */
/* 4   [: f  t  */
/* 5   s  f  t  */

TACT(jtvdyad){A fs,sf,xt,yt;B xl,xr,yl,yr;I xi=-1,yi=-1;TA x,y,z={num[1],0};V*u=0,*v=0;
 fs=stack[e-1].a; x=stack[b]; y=stack[e]; sf=swapc(fs);
 if(xt=tine(x.t)){xi=tvi(x.t); u=FAV(xt); if(0>xi&&CFORK==u->id){xi=tvi(u->fgh[0]); if(0>xi)xi=tvi(u->fgh[2]);}}
 if(yt=tine(y.t)){yi=tvi(y.t); v=FAV(yt); if(0>yi&&CFORK==v->id){yi=tvi(v->fgh[0]); if(0>yi)yi=tvi(v->fgh[2]);}}
 if(fs==ds(CLEFT)){if(xt)z.t=xt; else z.a=x.a; R z;}
 if(0>xi&&0>yi)switch((xt?2:0)+(yt?1:0)){
  case 0: z.a=df2(x.a,y.a,fs); break;
  case 1: z.t=folk(x.a,fs,yt); break;
  case 2: z.t=folk(y.a,sf,xt); break;
  case 3:
   xl=xt==LF; xr=xt==RT;
   yl=yt==LF; yr=yt==RT;
   if     (xl&&yr)            z.t=fs;
   else if(xr&&yl)            z.t=sf;
   else if(xr&&yr&&jt->tmonad)z.t=swap(fs);
   else z.t=CFORK==u->id&&primitive(yt)?folk(yt,sf,xt):folk(xt,fs,yt);
 }else{B b,c;I i,j,xj,yj;
  i=dcase(xi,u); if(u&&CFORK==u->id){xi=tvi(u->fgh[0]); xj=tvi(u->fgh[2]);}else{xi=-1; xj=tvi(xt);}
  j=dcase(yi,v); if(v&&CFORK==v->id){yi=tvi(v->fgh[0]); yj=tvi(v->fgh[2]);}else{yi=-1; yj=tvi(yt);}
  z.t=0; b=xj==yj; c=xj==yi;
  switch(DCASE(i,j)){
   case DCASE(0,2): z.t=folk(x.a,fs,yt); break;
   case DCASE(2,0): z.t=folk(y.a,sf,xt); break;
   case DCASE(0,3): z.t=folk(CP,folk(x.a,fs,FGR(v)),v->fgh[2]); break;
   case DCASE(0,4): z.t=folk(CP,folk(x.a,fs,v->fgh[1]  ),v->fgh[2]); break;
   case DCASE(1,2): z.t=folk(xt,fs,yt); break;
   case DCASE(1,3): 
   case DCASE(1,4): z.t=folk(xt,folk(LF,fs,FGR(v)),v->fgh[2]); break;
   case DCASE(2,1): z.t=folk(xt,fs,yt); break;
   case DCASE(3,1): z.t=folk(xt,fs,yt); break;
   case DCASE(4,1): z.t=folk(xt,fs,yt); break;
   case DCASE(2,2): z.t=folk(xt,fs,yt); break;
   case DCASE(2,3): z.t=b?folk(CP,folk(RT,    fs,FGR(v)),v->fgh[2]):folk(xt,  folk(LF,    fs,FGR(v)),v->fgh[2]); break;
   case DCASE(2,4): z.t=b?folk(CP,folk(RT,    fs,v->fgh[1]  ),v->fgh[2]):folk(xt,  folk(LF,    fs,FGR(v)),v->fgh[2]); break;
   case DCASE(3,2): z.t=b?folk(CP,folk(FGR(u),fs,RT    ),yt  ):folk(u->fgh[2],folk(FGL(u),fs,RT    ),yt  ); break;
   case DCASE(3,3): z.t=b?folk(CP,folk(FGR(u),fs,FGR(v)),v->fgh[2]):folk(u->fgh[2],folk(FGL(u),fs,FGR(v)),v->fgh[2]); break;
   case DCASE(3,4): z.t=b?folk(CP,folk(FGR(u),fs,v->fgh[1]  ),v->fgh[2]):folk(u->fgh[2],folk(FGL(u),fs,FGR(v)),v->fgh[2]); break;
   case DCASE(4,2): z.t=b?folk(CP,folk(u->fgh[1],  fs,RT    ),yt  ):folk(u->fgh[2],folk(FGL(u),fs,RT    ),yt  ); break;
   case DCASE(4,3): z.t=b?folk(CP,folk(u->fgh[1],  fs,FGR(v)),v->fgh[2]):folk(u->fgh[2],folk(FGL(u),fs,FGR(v)),v->fgh[2]); break;
   case DCASE(4,4): z.t=b?folk(CP,folk(u->fgh[1],  fs,v->fgh[1]  ),v->fgh[2]):folk(u->fgh[2],folk(FGL(u),fs,FGR(v)),v->fgh[2]); break;
   case DCASE(0,5):         z.t=folk(v->fgh[0],folk(x.a,            fs,v->fgh[1]),v->fgh[2]); break; 
   case DCASE(2,5): if(b||c)z.t=folk(v->fgh[0],folk(b?RT:LF,        fs,v->fgh[1]),v->fgh[2]); break; 
   case DCASE(3,5):
   case DCASE(4,5): if(b||c)z.t=folk(v->fgh[0],folk(b?FGR(u):FGL(u),fs,v->fgh[1]),v->fgh[2]); break; 
   case DCASE(5,0):         z.t=folk(u->fgh[0],folk(y.a,            sf,u->fgh[1]),u->fgh[2]); break; 
   case DCASE(5,2): if(b||c)z.t=folk(u->fgh[0],folk(u->fgh[1],           fs,b?RT    :LF        ),yt  ); break; 
   case DCASE(5,3):
   case DCASE(5,4): if(b||c)z.t=folk(u->fgh[0],folk(u->fgh[1],           fs,b?FGR(v):FGL(v)    ),v->fgh[2]); break; 
   case DCASE(5,5): if(xi==yi&&xj==yj||xi==yj&&xj==yi)
                     if(b||  v->fgh[1]==swapc(v->fgh[1]))z.t=folk(u->fgh[0],folk(u->fgh[1],fs,     v->fgh[1] ),u->fgh[2]);
                     else if(u->fgh[1]==swapc(u->fgh[1]))z.t=folk(v->fgh[0],folk(u->fgh[1],fs,     v->fgh[1] ),v->fgh[2]);
                     else                      z.t=folk(u->fgh[0],folk(u->fgh[1],fs,swap(v->fgh[1])),u->fgh[2]);
  }
  RZZ(z.t);
 }
 R z;
}

TACT(jtvadv ){TA z={0,0}; if(CHK1)z.a=df1(stack[b].a,stack[e].a); R z;}

TACT(jtvconj){TA z={0,0}; if(CHK2)z.a=df2(stack[b].a,stack[e].a,stack[e-1].a); R z;}

TACT(jtvfolk){TA z={0,0}; if(CHK3)z.a=folk(stack[b].a,stack[1+b].a,stack[e].a); R z;}

TACT(jtvhook){TA z={0,0}; if(CHK2)z.a=hook(stack[b].a,stack[e].a); R z;}

TACT(jtvpunc){R stack[e-1];}

TACT(jtvis){A ea,et,n,t;I j;TA*u,z={0,0};
 n=stack[b].a;
 if(!(NAME&AT(n)&&CASGN==*CAV(stack[1+b].a)))R z;
 t=sfn(0,n); j=jt->ttabi; u=jt->ttab;
 if(!t||NTTAB==jt->ttabi)R z;
 DO(j, if(equ(t,u->a))R z; ++u;);
 ea=stack[e].a; et=stack[e].t;
 symbis(n,ea,jt->local);
 ++jt->ttabi; u->a=t; u->t=et?et:cfn(ea);
 z.a=ea;  z.t=jt->tsubst?qq(sc(TC+j),num[-1]):et;
 R z;
}

static TACT(jtvmove){A t;TA*u,x,z;
 x=stack[MAX(0,e)];
 if(!(NAME&AT(x.a))||ASGN&AT(stack[b].a))R x;
 z.a=nameref(x.a); z.t=0; t=sfn(0,x.a); u=jt->ttab;
 DO(jt->ttabi, if(equ(t,u->a)){z.t=jt->tsubst&&jt->ttabi0<=i?qq(sc(TC+i),num[-1]):u->t; break;} ++u;);
 R z;
}

/* final translator result                             */
/* modifies argument in place                          */
/* a. replaces 880i functions by jt->ttab[i].t entries */
/* b. replaces n0"_ v1 v2 by n0 v1 v2                  */
/* c. replaces [: g ] by g, if monad                   */

static F1(jtvfinal){I i;V*u,*v;
 RZ(w);
 if(!(VERB&AT(w)))R w;
 v=FAV(w);
 if(CFORK!=v->id){i=tvi(w); R 0<=i?vfinal(jt->ttab[i].t):w;}
 RZ(v->fgh[0]=incorp(tine(vfinal(v->fgh[0]))));
 RZ(v->fgh[1]=incorp(tine(vfinal(v->fgh[1]))));
 RZ(v->fgh[2]=incorp(tine(vfinal(v->fgh[2]))));
 if(VERB&AT(v->fgh[0])){
  u=FAV(v->fgh[0]); 
  if(CFCONS==u->id)v->fgh[0]=u->fgh[2];  // must be incorped already
  else if(CQQ==u->id&&NOUN&AT(u->fgh[0])&&equ(ainf,u->fgh[1]))v->fgh[0]=u->fgh[0];  // must be incorped already
  if(NOUN&AT(v->fgh[0]))RZ(w=folk(v->fgh[0],v->fgh[1],v->fgh[2]));
 }
 R tine(w);
}    

F1(jttparse){A*s,t,x;C d;I b,*c,e,i,j,k,m,n;TA*stack;
 RZ(w);
 stack=(TA*)AV(w); n=m=*AS(w)-4;
 do{
  for(i=0;i<NCASES;i++){
   c=cases[i].c; s=(A*)(n+stack); d=1;
   d=d&&*c++&AT(*s); s+=WTA;
   d=d&&*c++&AT(*s); s+=WTA;
   d=d&&*c++&AT(*s); s+=WTA;
   d=d&&*c++&AT(*s);
   if(d)break;
  }
  if(i<NCASES){
   b=cases[i].b; j=n+b;
   e=cases[i].e; k=n+e;
   stack[k]=(cases[i].vf)(jt,j,k,stack);
   if(!(stack[k].a))R A0;
   DO(b,stack[--k]=stack[--j];); n=k;
  } else {stack[n-1]=vmove(n,m-1,stack); RE(0); n-=0<m--;}
 } while(0<=m);
 x=stack[1+n].a; t=stack[1+n].t;
 ASSERT(NOUN&AT(x)&&MARK&AT(stack[2+n].a),EVSYNTAX);
 R t?vfinal(t):cfn(x);
}

F1(jtvtrans){PROLOG(0053);A local,y,z=0;B tmonad,tsubst;I c,i;TA ttab[NTTAB],*ttab0;
 local=jt->local; tmonad=jt->tmonad; ttab0=jt->ttab; tsubst=jt->tsubst;
 RZ(ttab[0].a=cstr("x")); ttab[0].t=ds(CLEFT);
 RZ(ttab[1].a=cstr("y")); ttab[1].t=RT; c=2;
 for(i=0;!z&&2>i;++i){
  RZ(y=vtokens(w));
  jt->ttab=ttab; jt->ttabi=jt->ttabi0=c;
  RZ(jt->local=stcreate(2,40,0L,0L));
  IS(ynam,num[1]); if(!jt->tmonad)IS(xnam,num[1]); 
  jt->tsubst=0==i;
  z=tparse(y); RESETERR;
  if(i&&!z)z=colon(num[4-jt->tmonad],w);
 }
 jt->local=local; jt->tmonad=tmonad; jt->ttab=ttab0; jt->tsubst=tsubst;
 EPILOG(z);
}
