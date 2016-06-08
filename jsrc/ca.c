/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Atop and Ampersand                                        */

#include "j.h"


static DF1(jtonf1){F1PREF;PROLOG;DECLFG;A z;I flag=sv->flag,m=jt->xmode;
 PREF1(jtonf1);
 if(primitive(gs))if(flag&VFLR)jt->xmode=XMFLR; else if(flag&VCEIL)jt->xmode=XMCEIL;
 if(RAT&AT(w))RZ(w=pcvt(XNUM,w));
 z=CALL1(f1,CALL1(g1,w,gs),fs);
 jt->xmode=m;
 EPILOG(z);
}

static DF2(jtuponf2){F2PREF;PROLOG;DECLFG;A z;I flag=sv->flag,m=jt->xmode;
 RZ(a&&w);
 if(primitive(gs))if(flag&VFLR)jt->xmode=XMFLR; else if(flag&VCEIL)jt->xmode=XMCEIL;
 if(RAT&AT(a))RZ(a=pcvt(XNUM,a));
 if(RAT&AT(w))RZ(w=pcvt(XNUM,w));
 z=INT&AT(a)&&INT&AT(w)&&CDIV==ID(gs)?intdiv(a,w):CALL1(f1,CALL2(g2,a,w,gs),fs);
 jt->xmode=m;
 EPILOG(z);
}

static X jtxmodpow(J jt,A a,A w,A h){A ox,z;
 if(!(XNUM&AT(a)))RZ(a=cvt(XNUM,a));
 if(!(XNUM&AT(w)))RZ(w=cvt(XNUM,w));
 if(!(XNUM&AT(h)))RZ(h=cvt(XNUM,h));
 ox=jt->xmod; jt->xmod=h;
 GA(z,XNUM,1,0,0); *XAV(z)=xpow(*XAV(a),*XAV(w));
 jt->xmod=ox;
 RNE(z);
}

#define DMOD 46340         /* <. %: _1+2^31 */

#if SY_64
#define XMOD 3037000499    /* <. %: _1+2^63 */
#else
#define XMOD 94906265      /* <. %: _1+2^53 */
static I dmodpow(D x,I n,D m){D z=1; while(n){if(1&n)z=fmod(z*x,m); x=fmod(x*x,m); n>>=1;} R(I)z;}
#endif

static I imodpow(I x,I n,I m){I z=1; while(n){if(1&n)z=(z*x)%m;     x=(x*x)%m;     n>>=1;} R   z;}

static DF2(jtmodpow2){F2PREF;A h;B b,c;I at,m,n,wt,x,z;
 PREF2(jtmodpow2);
 h=VAV(self)->h; 
 if(RAT&AT(a))RZ(a=pcvt(XNUM,a)) else if(!(AT(a)&INT+XNUM))RZ(a=pcvt(INT,a)); 
 if(RAT&AT(w))RZ(w=pcvt(XNUM,w)) else if(!(AT(w)&INT+XNUM))RZ(w=pcvt(INT,w));
 at=AT(a); wt=AT(w);
 if((AT(h)&XNUM||at&XNUM||wt&XNUM)&&at&XNUM+INT&&wt&INT+XNUM){A z;
  z=xmodpow(a,w,h); if(!jt->jerr)R z; RESETERR; R residue(h,expn2(a,w)); 
 }
 n=*AV(w);
 if(!(INT&at&&INT&wt&&0<=n))R residue(h,expn2(a,w));
 m=*AV(h); x=*AV(a);
 if(!m)R expn2(a,w);
 if(XMOD<m||XMOD<-m||m==IMIN||x==IMIN)R cvt(INT,xmodpow(a,w,h));
 if(b=0>m)m=-m;
 if(c=0>x)x=-x; x=x%m; if(c)x=m-x;
#if SY_64
 z=imodpow(x,n,m);
#else
 z=m>DMOD?dmodpow((D)x,n,(D)m):imodpow(x,n,m);
#endif
 R sc(b?z-m:z);
}    /* a m&|@^ w ; m guaranteed to be INT or XNUM */

static DF1(jtmodpow1){F1PREF;A g=VAV(self)->g; R rank2ex(VAV(g)->f,w,self,0L,0L,jtmodpow2);}
     /* m&|@(n&^) w ; m guaranteed to be INT or XNUM */

static CS1(on1,  CALL1(f1,CALL1(g1,w,gs),fs))
static CS2(jtupon2,CALL1(f1,CALL2(g2,a,w,gs),fs))

static DF2(on2){F2PREF;PROLOG;DECLFG;A ga,gw,z; 
 PREF2(on2); 
 gw=CALL1(g1,w,gs);
 ga=CALL1(g1,a,gs);
 z=CALL2(f2,ga,gw,fs); 
 EPILOG(z);
}

static DF2(atcomp){F2PREF;AF f;
 RZ(a&&w); 
 f=atcompf(a,w,self); 
 R f?f(jt,a,w,self):upon2(a,w,self);
}

static DF2(atcomp0){F2PREF;A z;AF f;D oldct=jt->ct;
 RZ(a&&w);
 f=atcompf(a,w,self);
 jt->ct=0; z=f?f(jt,a,w,self):upon2(a,w,self); jt->ct=oldct; 
 R z;
}

F2(jtatop){F2PREF;A f,g,h=0,x;AF f1=on1,f2=jtupon2;B b=0,j;C c,d,e;I flag=0,m=-1;V*av,*wv;
 ASSERTVV(a,w);
 av=VAV(a); c=av->id;
 wv=VAV(w); d=wv->id;
 switch(c){
  case CNOT:    if(d==CMATCH){f2=jtnotmatch; flag+=VIRS2;} break;
  case CGRADE:  if(d==CGRADE){f1=jtranking; flag+=VIRS1;} break;
  case CSLASH:  if(d==CCOMMA)f1=jtredravel; break;
  case CCEIL:   f1=jtonf1; f2=jtuponf2; flag=VCEIL; break;
  case CFLOOR:  f1=jtonf1; f2=jtuponf2; flag=VFLR;  break;
  case CICAP:   if(d==CNE)f1=jtnubind; else if(FIT0(CNE,wv))f1=jtnubind0; break;
  case CQUERY:  if(d==CDOLLAR||d==CPOUND)f2=jtrollk;  break;
  case CQRYDOT: if(d==CDOLLAR||d==CPOUND)f2=jtrollkx; break;
  case CRAZE:   if(d==CCUT&&boxatop(w)){f1=jtrazecut1; f2=jtrazecut2;} break;
  case CSLDOT:  if(d==CSLASH&&CSLASH==ID(av->f))f2=jtpolymult; break;
  case CQQ:     if(d==CTHORN&&CEXEC==ID(av->f)&&equ(zero,av->g))f1=jtdigits10; break;
  case CEXP:    if(d==CCIRCLE)f1=jtexppi; break;
  case CAMP:
   x=av->f; if(RAT&AT(x))RZ(x=pcvt(XNUM,x));
   if((d==CEXP||d==CAMP&&CEXP==ID(wv->g))&&AT(x)&INT+XNUM&&!AR(x)&&CSTILE==ID(av->g)){
    h=x; flag=VMOD; 
    if(d==CEXP)f2=jtmodpow2; else f1=jtmodpow1;
 }}
 if(d==CEBAR||d==CEPS||(b=FIT0(CEPS,wv))){
  f=av->f; g=av->g; e=ID(f); if(b)d=ID(wv->f);
  if(c==CICAP)m=7;
  else if(c==CSLASH)m=e==CPLUS?4:e==CPLUSDOT?5:e==CSTARDOT?6:-1;
  else if(c==CAMP&&(g==zero||g==one)){j=*BAV(g); m=e==CIOTA?j:e==CICO?2+j:-1;}
  switch(0<=m?d:-1){
   case CEBAR: f2=b?atcomp0:atcomp; flag=6+8*m; break;
   case CEPS:  f2=b?atcomp0:atcomp; flag=7+8*m; break;
 }}
 R fdef(CAT,VERB, f1,f2, a,w,h, flag, (I)wv->mr,(I)wv->lr,(I)wv->rr);
}

F2(jtatco){F2PREF;A f,g;AF f1=on1,f2=jtupon2;B b=0;C c,d,e;I flag=0,j,m=-1;V*av,*wv;
 ASSERTVV(a,w);
 av=VAV(a); c=av->id; f=av->f; g=av->g; e=ID(f); 
 wv=VAV(w); d=wv->id;
 switch(c){
  case CNOT:    if(d==CMATCH){f2=jtnotmatch; flag+=VIRS2;} break;
  case CGRADE:  if(d==CGRADE){f1=jtranking; flag+=VIRS1;} break;
  case CCEIL:   f1=jtonf1; f2=jtuponf2; flag=VCEIL; break;
  case CFLOOR:  f1=jtonf1; f2=jtuponf2; flag=VFLR;  break;
  case CQUERY:  if(d==CDOLLAR||d==CPOUND)f2=jtrollk;  break;
  case CQRYDOT: if(d==CDOLLAR||d==CPOUND)f2=jtrollkx; break;
  case CICAP:   m=7; if(d==CNE)f1=jtnubind; else if(FIT0(CNE,wv))f1=jtnubind0; break;
  case CAMP:    if(g==zero||g==one){j=*BAV(g); m=e==CIOTA?j:e==CICO?2+j:-1;} break;
  case CSLASH:  
   if(vaid(f)&&vaid(w))f2=jtfslashatg;
   if(d==CCOMMA)f1=jtredravel; else m=e==CPLUS?4:e==CPLUSDOT?5:e==CSTARDOT?6:-1;
   break;
  case CSEMICO: 
   if(d==CLBRACE)f2=jtrazefrom; 
   else if(d==CCUT){
    j=i0(wv->g);
    if(CBOX==ID(wv->f)&&!j)f2=jtrazecut0;    
    else if(boxatop(w)&&j&&-2<=j&&j<=2){f1=jtrazecut1; f2=jtrazecut2;}
 }}
 if(0<=m){
  b=d==CFIT&&equ(zero,wv->g);
  switch(b?ID(wv->f):d){
   case CEQ:   f2=b?atcomp0:atcomp; flag=0+8*m; break;
   case CNE:   f2=b?atcomp0:atcomp; flag=1+8*m; break;
   case CLT:   f2=b?atcomp0:atcomp; flag=2+8*m; break;
   case CLE:   f2=b?atcomp0:atcomp; flag=3+8*m; break;
   case CGE:   f2=b?atcomp0:atcomp; flag=4+8*m; break;
   case CGT:   f2=b?atcomp0:atcomp; flag=5+8*m; break;
   case CEBAR: f2=b?atcomp0:atcomp; flag=6+8*m; break;
   case CEPS:  f2=b?atcomp0:atcomp; flag=7+8*m; break;
 }}
 R fdef(CATCO,VERB, f1,f2, a,w,0L, flag, RMAX,RMAX,RMAX);
}

F2(jtampco){F2PREF;AF f1=on1;C c,d;I flag=0;V*wv;
 ASSERTVV(a,w);
 c=ID(a); wv=VAV(w); d=wv->id;
 if     (c==CSLASH&&d==CCOMMA)         f1=jtredravel;
 else if(c==CRAZE&&d==CCUT&&boxatop(w))f1=jtrazecut1;
 else if(c==CGRADE&&d==CGRADE)         {f1=jtranking; flag+=VIRS1;}
 R fdef(CAMPCO,VERB, f1,on2, a,w,0L, flag, RMAX,RMAX,RMAX);
}

static DF1(withl){F1PREF;DECLFG; R jt->rank?irs2(fs,w,gs,AR(fs),jt->rank[1],g2):CALL2(g2,fs,w,gs);}
static DF1(withr){F1PREF;DECLFG; R jt->rank?irs2(w,gs,fs,jt->rank[1],AR(gs),f2):CALL2(f2,w,gs,fs);}

static DF1(ixfixedleft  ){F1PREF;V*v=VAV(self); R indexofprehashed(v->f,w,v->h);}
static DF1(ixfixedright ){F1PREF;V*v=VAV(self); R indexofprehashed(v->g,w,v->h);}

static DF1(ixfixedleft0 ){F1PREF;A z;D old=jt->ct;V*v=VAV(self); 
 jt->ct=0.0; z=indexofprehashed(v->f,w,v->h); jt->ct=old; 
 R z;
}

static DF1(ixfixedright0){F1PREF;A z;D old=jt->ct;V*v=VAV(self); 
 jt->ct=0.0; z=indexofprehashed(v->g,w,v->h); jt->ct=old; 
 R z;
}

static DF2(with2){F2PREF;R df1(w,powop(self,a,0));}

F2(jtamp){F2PREF;A h=0;AF f1=on1,f2=on2;B b;C c,d=0;D old=jt->ct;I flag=0,mode=-1,p,r;V*u,*v;
 RZ(a&&w);
 switch(CONJCASE(a,w)){
  default: ASSERTSYS(0,"amp");
  case NN: ASSERT(0,EVDOMAIN);
  case NV:
   f1=withl; v=VAV(w); c=v->id;
   if(AN(a)&&AR(a)){
    if(b=c==CFIT&&equ(zero,v->g))c=ID(v->f); 
    mode=c==CIOTA?IIDOT:c==CICO?IICO:-1;
   }
   if(0<=mode){
    if(b){jt->ct=0.0; h=indexofsub(mode,a,mark); jt->ct=old; f1=ixfixedleft0;}
    else {            h=indexofsub(mode,a,mark);             f1=ixfixedleft ;}
   }else switch(c){
    case CWORDS: RZ(a=fsmvfya(a)); f1=jtfsmfx; break;
    case CIBEAM: if(v->f&&v->g&&128==i0(v->f)&&3==i0(v->g)){RZ(h=crccompile(a)); f1=jtcrcfixedleft;}
   }
   R fdef(CAMP,VERB, f1,with2, a,w,h, flag, RMAX,RMAX,RMAX);
  case VN: 
   f1=withr;
   if(AN(w)&&AR(w)){
    v=VAV(a); c=v->id; p=v->flag%256; if(b=c==CFIT&&equ(zero,v->g))c=ID(v->f);
    if(7==p%8)mode=II0EPS+p/8;  /* (e.i.0:)  etc. */
    else      mode=c==CEPS?IEPS:c==CLESS?ILESS:-1;
   }
   if(0<=mode){
    if(b){jt->ct=0.0; h=indexofsub(mode,w,mark); jt->ct=old; f1=ixfixedright0;}
    else {            h=indexofsub(mode,w,mark);             f1=ixfixedright ;}
   }
   R fdef(CAMP,VERB, f1,with2, a,w,h, flag, RMAX,RMAX,RMAX);
  case VV:
   v=VAV(w); c=v->id; r=v->mr;
   if(c==CFORK||c==CAMP){
    if(c==CFORK)d=ID(v->h);
    if(CIOTA==ID(v->g)&&(!d||d==CLEFT||d==CRIGHT)&&equ(alp,v->f)){
     u=VAV(a); d=u->id;
     if(d==CLT||d==CLE||d==CEQ||d==CNE||d==CGE||d==CGT)f2=jtcharfn2;
   }}else switch(ID(a)){
    case CGRADE: if(c==CGRADE){f1=jtranking; flag+=VIRS1;} break;
    case CSLASH: if(c==CCOMMA)f1=jtredravel; break;
    case CCEIL:  f1=jtonf1; flag=VCEIL; break;
    case CFLOOR: f1=jtonf1; flag=VFLR;  break;
    case CRAZE:  if(c==CCUT&&boxatop(w))f1=jtrazecut1;
   }
   R fdef(CAMP,VERB, f1,f2, a,w,0L, flag, r,r,r);
}}
