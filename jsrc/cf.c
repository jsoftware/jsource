/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Forks                                                     */

#include "j.h"

#define TC1(t)          (t&NOUN?0:t&VERB?3:t&CONJ?2:1)
#define BD(ft,gt)       (4*TC1(ft)+TC1(gt))
#define TDECL           V*sv=VAV(self);A fs=sv->f,gs=sv->g,hs=sv->h

#define FOLK1           {A fx,hx; hx=CALL1(h1,  w,hs); fx=CALL1(f1,  w,fs); z=CALL2(g2,fx,hx,gs);}
#define FOLK2           {A fx,hx; hx=CALL2(h2,a,w,hs); fx=CALL2(f2,a,w,fs); z=CALL2(g2,fx,hx,gs);}
#define CAP1            {z=CALL1(g1,CALL1(h1,  w,hs),gs);}
#define CAP2            {z=CALL1(g1,CALL2(h2,a,w,hs),gs);}

static DF1(jtcork1){F1PREF;DECLFGH;PROLOG;A z;  CAP1; EPILOG(z);}
static DF2(jtcork2){F2PREF;DECLFGH;PROLOG;A z;  CAP2; EPILOG(z);}
static DF1(jtfolk1){F1PREF;DECLFGH;PROLOG;A z; FOLK1; EPILOG(z);}
static DF2(jtfolk2){F2PREF;DECLFGH;PROLOG;A z; FOLK2; EPILOG(z);}

static B jtcap(J jt,A x){V*v;
 while(v=VAV(x),CTILDE==v->id&&NAME&AT(v->f)&&(x=symbrd(v->f)));
 R CCAP==v->id;
}

static DF1(jtcorx1){F1PREF;DECLFGH;PROLOG;A z; if(cap(fs))RZ(z=df1(  w,folk(ds(CCAP),gs,hs))) else FOLK1; EPILOG(z);}
static DF2(jtcorx2){F2PREF;DECLFGH;PROLOG;A z; if(cap(fs))RZ(z=df2(a,w,folk(ds(CCAP),gs,hs))) else FOLK2; EPILOG(z);}
     /* f g h where f may be [: */

static DF1(jtnvv1){F1PREF;DECLFGH;PROLOG; EPILOG(CALL2(g2,fs,CALL1(h1,  w,hs),gs));}
static DF2(jtnvv2){F2PREF;DECLFGH;PROLOG; EPILOG(CALL2(g2,fs,CALL2(h2,a,w,hs),gs));}

static DF2(jtfolkcomp){F2PREF;DECLFGH;PROLOG;A z;AF f;
 RZ(a&&w);
 if(f=atcompf(a,w,self))z=f(jt,a,w,self); else if(cap(fs))CAP2 else FOLK2;
 EPILOG(z);
}

static DF2(jtfolkcomp0){F2PREF;DECLFGH;PROLOG;A z;AF f;D oldct=jt->ct;
 RZ(a&&w);
 jt->ct=0;
 if(f=atcompf(a,w,self))z=f(jt,a,w,self); else if(cap(fs))CAP2 else FOLK2;
 jt->ct=oldct;
 EPILOG(z);
}

static DF1(jtcharmapa){F1PREF;V*v=VAV(self); R charmap(w,VAV(v->h)->f,v->f);}
static DF1(jtcharmapb){F1PREF;V*v=VAV(self); R charmap(w,VAV(v->f)->f,VAV(v->h)->f);}

A jtfolk(J jt,A f,A g,A h){F2PREF;A p,q,x,y;AF f1=jtfolk1,f2=jtfolk2;B b;C c,fi,gi,hi;I flag=0,j,m=-1;V*fv,*gv,*hv,*v;
 RZ(f&&g&&h);
 gv=VAV(g); gi=gv->id; 
 hv=VAV(h); hi=hv->id; 
 if(NOUN&AT(f)){  /* y {~ x i. ] */
  f1=jtnvv1;
  if(LIT&AT(f)&&1==AR(f)&&gi==CTILDE&&CFROM==ID(gv->f)&&hi==CFORK){
   x=hv->f;
   if(LIT&AT(x)&&1==AR(x)&&CIOTA==ID(hv->g)&&CRIGHT==ID(hv->h))f1=jtcharmapa;  
  }
  R fdef(CFORK,VERB, f1,jtnvv2, f,g,h, flag, RMAX,RMAX,RMAX);
 }
 fv=VAV(f); fi=fv->id;
 switch(fi){
  case CCAP:                      f1=jtcork1; f2=jtcork2;  break; /* [: g h */
  case CTILDE: if(NAME&AT(fv->f)){f1=jtcorx1; f2=jtcorx2;} break; /* f  g h */
  case CSLASH: if(gi==CDIV&&hi==CPOUND&&CPLUS==ID(fv->f))f1=jtmean; break;  /* +/%# */
  case CAMP:   /* x&i.     { y"_ */
  case CFORK:  /* (x i. ]) { y"_ */
   if(hi==CQQ&&(y=hv->f,LIT&AT(y)&&1==AR(y))&&equ(ainf,hv->g)&&
       (x=fv->f,LIT&AT(x)&&1==AR(x))&&CIOTA==ID(fv->g)&&
       (fi==CAMP||CRIGHT==ID(fv->h)))f1=jtcharmapb; break;
  case CAT:    /* <"1@[ { ] */
   if(gi==CLBRACE&&hi==CRIGHT){                                   
    p=fv->f; q=fv->g; 
    if(CLEFT==ID(q)&&CQQ==ID(p)&&(v=VAV(p),x=v->f,CLT==ID(x)&&equ(one,v->g)))f2=jtsfrom;
 }}
 switch(fi==CCAP?gi:hi){
  case CQUERY:  if(hi==CDOLLAR||hi==CPOUND)f2=jtrollk;  break;
  case CQRYDOT: if(hi==CDOLLAR||hi==CPOUND)f2=jtrollkx; break;
  case CICAP:   m=7; if(fi==CCAP){if(hi==CNE)f1=jtnubind; else if(FIT0(CNE,hv))f1=jtnubind0;} break;
  case CSLASH:  c=ID(gv->f); m=c==CPLUS?4:c==CPLUSDOT?5:c==CSTARDOT?6:-1; 
                if(fi==CCAP&&vaid(gv->f)&&vaid(h))f2=jtfslashatg;
                break;
  case CFCONS:  if(hi==CFCONS){x=hv->h; j=*BAV(x); m=B01&AT(x)?(gi==CIOTA?j:gi==CICO?2+j:-1):-1;} break;
  case CRAZE:   if(hi==CLBRACE)f2=jtrazefrom;
                else if(hi==CCUT){
                 j=i0(hv->g);
                 if(CBOX==ID(hv->f)&&!j)f2=jtrazecut0;
                 else if(boxatop(h)&&j&&-2<=j&&j<=2){f1=jtrazecut1; f2=jtrazecut2;}
 }}
 if(0<=m){
  v=4<=m?hv:fv; b=CFIT==v->id&&equ(zero,v->g);
  switch(b?ID(v->f):v->id){
   case CEQ:   f2=b?jtfolkcomp0:jtfolkcomp; flag=0+8*m; break;
   case CNE:   f2=b?jtfolkcomp0:jtfolkcomp; flag=1+8*m; break;
   case CLT:   f2=b?jtfolkcomp0:jtfolkcomp; flag=2+8*m; break;
   case CLE:   f2=b?jtfolkcomp0:jtfolkcomp; flag=3+8*m; break;
   case CGE:   f2=b?jtfolkcomp0:jtfolkcomp; flag=4+8*m; break;
   case CGT:   f2=b?jtfolkcomp0:jtfolkcomp; flag=5+8*m; break;
   case CEBAR: f2=b?jtfolkcomp0:jtfolkcomp; flag=6+8*m; break;
   case CEPS:  f2=b?jtfolkcomp0:jtfolkcomp; flag=7+8*m; break;
 }}
 R fdef(CFORK,VERB, f1,f2, f,g,h, flag, RMAX,RMAX,RMAX);
}


static DF1(taa){F1PREF;TDECL;A t=df1(w,fs); ASSERT(!t||AT(t)&NOUN+VERB,EVSYNTAX); R df1(t,gs);}
static DF1(tvc){F1PREF;TDECL; R df2(fs,w,gs);}  /* also nc */
static DF1(tcv){F1PREF;TDECL; R df2(w,gs,fs);}  /* also cn */


static CS1(jthook1, CALL2(f2,w,CALL1(g1,w,gs),fs))
static CS2(jthook2, CALL2(f2,a,CALL1(g1,w,gs),fs))

static DF1(jthkiota){F1PREF;DECLFG;A a,e;I n;P*p;
 RZ(w);
 n=IC(w);
 if(SB01&AT(w)&&1==AR(w)){
  p=PAV(w); a=SPA(p,a); e=SPA(p,e); 
  R *BAV(e)||equ(mtv,a) ? repeat(w,IX(n)) : repeat(SPA(p,x),ravel(SPA(p,i)));
 }
 R B01&AT(w)&&1>=AR(w) ? ifb(n,BAV(w)) : repeat(w,IX(n));
}    /* special code for (# i.@#) */

static DF1(jthkodom){F1PREF;DECLFG;B b=0;I n,*v;
 RZ(w);
 if(INT&AT(w)&&1==AR(w)){n=AN(w); v=AV(w); DO(n, if(b=0>v[i])break;); if(!b)R odom(2L,n,v);}
 R CALL2(f2,w,CALL1(g1,w,gs),fs);
}    /* special code for (#: i.@(* /)) */

static DF2(jthkeps){F2PREF;
 RZ(a&&w);
 if(AT(a)==AT(w)&&AT(a)&IS1BYTE&&1==AN(a)){A z;
  GA(z,B01,1,AR(a),AS(a)); 
  *BAV(z)=1&&memchr(CAV(w),*CAV(a),AN(w)); 
  R z;
 }
 R eps(a,gah(1L,w));
}    /* special code for (e.,) */

static DF2(jthkfrom){F2PREF;R from(a,gah(1L,w));}     /* special code for ({ ,) */
static DF2(jthktake){F2PREF;R take(a,gah(1L,w));}     /* special code for ({.,) */
static DF2(jthkdrop){F2PREF;R drop(a,gah(1L,w));}     /* special code for (}.,) */

static DF1(jthkindexofmaxmin){F1PREF;D*du,*dv;I*iu,*iv,n,t,*wv,z=0;V*sv;
 RZ(w&&self);
 n=AN(w); t=AT(w);
 if(!(1==AR(w)&&t&INT+FL))R hook1(w,self);
 sv=VAV(self); wv=AV(w);
 if(n)switch((t&FL?4:0)+(CICO==ID(sv->f)?2:0)+(CMAX==ID(VAV(sv->g)->f))){
  case  0: iu=iv=    wv;     DO(n, if(*iv<*iu)iu=iv; ++iv;); z=iu-    wv; break;
  case  1: iu=iv=    wv;     DO(n, if(*iv>*iu)iu=iv; ++iv;); z=iu-    wv; break;
  case  2: iu=iv=    wv+n-1; DO(n, if(*iv<*iu)iu=iv; --iv;); z=iu-    wv; break;
  case  3: iu=iv=    wv+n-1; DO(n, if(*iv>*iu)iu=iv; --iv;); z=iu-    wv; break;
  case  4: du=dv=(D*)wv;     DO(n, if(*dv<*du)du=dv; ++dv;); z=du-(D*)wv; break;
  case  5: du=dv=(D*)wv;     DO(n, if(*dv>*du)du=dv; ++dv;); z=du-(D*)wv; break;
  case  6: du=dv=(D*)wv+n-1; DO(n, if(*dv<*du)du=dv; --dv;); z=du-(D*)wv; break;
  case  7: du=dv=(D*)wv+n-1; DO(n, if(*dv>*du)du=dv; --dv;); z=du-(D*)wv;
 }
 R sc(z);
}    /* special code for (i.<./) (i.>./) (i:<./) (i:>./) */


F2(jthook){F2PREF;AF f1=0,f2=0;C c,d,e,id;I flag=0;V*u,*v;
 RZ(a&&w);
 switch(BD(AT(a),AT(w))){
  default:            ASSERT(0,EVSYNTAX);
  case BD(VERB,VERB):
   u=VAV(a); c=u->id; f1=jthook1; f2=jthook2;
   v=VAV(w); d=v->id; e=ID(v->f);
   if(d==CCOMMA)switch(c){
    case CDOLLAR: f2=jtreshape; flag+=VIRS2; break;
    case CFROM:   f2=jthkfrom;  break;
    case CTAKE:   f2=jthktake;  break;
    case CDROP:   f2=jthkdrop;  break;
    case CEPS:    f2=jthkeps;
   }else        switch(c){
    case CSLDOT:  if(COMPOSE(d)&&e==CIOTA&&CPOUND==ID(v->g)&&CBOX==ID(u->f))f1=jtgroup; break;
    case CPOUND:  if(COMPOSE(d)&&e==CIOTA&&CPOUND==ID(v->g))f1=jthkiota; break;
    case CABASE:  if(COMPOSE(d)&&e==CIOTA&&CSLASH==ID(v->g)&&CSTAR==ID(VAV(v->g)->f))f1=jthkodom; break;
    case CIOTA:   
    case CICO:    if(d==CSLASH&&(e==CMAX||e==CMIN))f1=jthkindexofmaxmin; break;
    case CFROM:   if(d==CGRADE)f2=jtordstati; else if(d==CTILDE&&e==CGRADE)f2=jtordstat;
   }
   R fdef(CHOOK, VERB, f1,f2, a,w,0L, flag, RMAX,RMAX,RMAX);
  case BD(ADV, ADV ): f1=taa; break;
  case BD(NOUN,CONJ):
  case BD(VERB,CONJ):
   f1=tvc; id=ID(w);
   if(BOX&AT(a)&&(id==CATDOT||id==CGRAVE||id==CGRCO)&&gerexact(a))flag+=VGERL;
   break;
  case BD(CONJ,NOUN):
  case BD(CONJ,VERB):
   f1=tcv; id=ID(a);
   if(BOX&AT(w)&&(id==CGRAVE||id==CPOWOP&&1<AN(w))&&gerexact(w))flag+=VGERR;
 }
 R fdef(CADVF, ADV, f1,0L, a,w,0L, flag, 0L,0L,0L);
}
