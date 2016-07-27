/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Gerunds ` and `:                                          */

#include "j.h"


A jtfxeachv(J jt,I r,A w){A*wv,x,z,*zv;I n,wd;
 RZ(w);
 n=AN(w); wv=AAV(w); wd=(I)w*ARELATIVE(w);
 ASSERT(r>=AR(w),EVRANK);
 ASSERT(n,EVLENGTH);
 ASSERT(BOX&AT(w),EVDOMAIN);
 GATV(z,BOX,n,AR(w),AS(w)); zv=AAV(z);
 DO(n, RZ(zv[i]=x=fx(WVR(i))); ASSERT(VERB&AT(x),EVDOMAIN););
 R z;
}

F1(jtfxeach){R every(w,0L,jtfx);}

static DF1(jtcon1){A h,*hv,*x,z;V*sv;
 PREF1(jtcon1);
 sv=VAV(self); h=sv->h; hv=AAV(h);
 GATV(z,BOX,AN(h),AR(h),AS(h)); x=AAV(z);
 DO(AN(h), RZ(*x++=CALL1(VAV(*hv)->f1,  w,*hv)); ++hv;);
 R ope(z);
}

static DF2(jtcon2){A h,*hv,*x,z;V*sv;
 PREF2(jtcon2);
 sv=VAV(self); h=sv->h; hv=AAV(h);
 GATV(z,BOX,AN(h),AR(h),AS(h)); x=AAV(z);
 DO(AN(h), RZ(*x++=CALL2(VAV(*hv)->f2,a,w,*hv)); ++hv;);
 R ope(z);
}

static DF1(jtinsert){A f,hs,*hv,z;AF*hf;I j,k,m,n,old;
 RZ(w);
 n=IC(w); j=n-1; hs=VAV(self)->h; m=AN(hs); hv=AAV(hs);
 if(!n)R df1(w,iden(*hv));
 GATV(f,INT,m,1,0); hf=(AF*)AV(f); DO(m, hf[i]=VAV(hv[i])->f2;);
 RZ(z=from(num[-1],w));
 old=jt->tnextpushx;
 DO(n-1, k=--j%m; RZ(z=CALL2(hf[k],from(sc(j),w),z,hv[k])); gc(z,old);)
 R z;
}

F2(jtevger){A hs;I k;
 RZ(a&&w);
 RE(k=i0(w)); 
 if(k==GTRAIN)R exg(a);
 RZ(hs=fxeachv(RMAX,a));
 switch(k){
  case GAPPEND:
   R fdef(CGRCO,VERB, jtcon1,jtcon2, a,w,hs, VGERL, RMAX,RMAX,RMAX);
  case GINSERT:
   ASSERT(1>=AR(a),EVRANK);
   R fdef(CGRCO,VERB, jtinsert,0L,   a,w,hs, VGERL, RMAX,0L,0L);
  default:
   ASSERT(0,EVDOMAIN);
}}

F2(jttie){RZ(a&&w); R over(VERB&AT(a)?arep(a):a,VERB&AT(w)?arep(w):w);}


static B jtatomic(J jt,C m,A w){A f,g;B ax,ay,vf,vg;C c,id;V*v;
 static C atomic12[]={CMIN, CLE, CMAX, CGE, CPLUS, CPLUSCO, CSTAR, CSTARCO, CMINUS, CDIV, CROOT, 
     CEXP, CLOG, CSTILE, CBANG, CLEFT, CRIGHT, CJDOT, CCIRCLE, CRDOT, CHGEOM, CFCONS, 0};
 static C atomic1[]={CNOT, CHALVE, 0};
 static C atomic2[]={CEQ, CLT, CGT, CPLUSDOT, CSTARDOT, CNE, 0};
 RZ(w&&VERB&AT(w));
 v=VAV(w); id=v->id;
 if(strchr(atomic12,id)||strchr(1==m?atomic1:atomic2,id))R 1;
 f=v->f; vf=f&&VERB&AT(f); ax=f&&NOUN&AT(f)&&!AR(f);
 g=v->g; vg=g&&VERB&AT(g); ay=g&&NOUN&AT(g)&&!AR(g);
 switch(id){
  case CAT:
  case CATCO:  R atomic(1,f)&&atomic(m,g);
  case CUNDER:
  case CUNDCO: R atomic(m,f)&&atomic(1,g);
  case CAMPCO: R atomic(m,f)&&atomic(1,g);
  case CQQ:    R ax||atomic(m,f);
  case CFORK:  R (CCAP==ID(f)?atomic(1,g):atomic(m,f)&&atomic(2,g))&&atomic(m,v->h);
  case CHOOK:  R atomic(2,f)&&atomic(1,g);
  case CTILDE: R NAME&AT(f)?atomic(m,fix(f)):atomic(2,f);
  case CFIT:   R atomic(m,f);
  case CAMP:   
   if(vf&&vg)R atomic(m,f)&&atomic(1,g);
   if(ax&&atomic(2,g)||ay&&atomic(2,f))R 1;
   if(vg&&1==AR(f)){c=ID(g); R c==CPOLY||c==CBASE;}
 }
 R 0;
}    /* 1 iff verb w is atomic; 1=m monad 2=m dyad */

static A jtgjoin(J jt,C c,A a,A w){A f;
 RZ(a&&w);
 ASSERT(1>=AR(a)&&1>=AR(w),EVRANK);
 ASSERT((!AN(a)||BOX&AT(a))&&(!AN(w)||BOX&AT(w)),EVDOMAIN);
 RZ(f=qq(atop(ds(CBOX),ds(CCOMMA)),zero));
 R df2(box(spellout(c)),df2(a,w,f),f);
}

static DF1(jtcase1a){A g,h,*hv,k,t,u,w0=w,x,y,*yv,z;B b;I r,*xv;V*sv;
 RZ(w);
 r=AR(w);
 if(1<r)RZ(w=gah(1L,w));
 sv=VAV(self); g=sv->g;
 if(atomic(1,g))RZ(k=df1(w,g))
 else{RZ(k=df1(w,qq(g,zero))); ASSERT(AR(k)==AR(w)&&AN(k)==AN(w),EVRANK);}
 if(B01&AT(k)){
  h=sv->h; ASSERT(2<=AN(h),EVINDEX); hv=AAV(h);
  RZ(x=df1(t=repeat(not(k),w),hv[0])); if(!AR(x))RZ(x=reshape(tally(t),x));
  RZ(y=df1(t=repeat(k,     w),hv[1])); if(!AR(y))RZ(y=reshape(tally(t),y));
  RZ(z=!AN(x)?y:!AN(y)?x:from(grade1(grade1(k)),over(x,y)));
 }else{
  RZ(u=nub(k));
  RZ(y=df2(k,w,sldot(gjoin(CATCO,box(scc(CBOX)),from(u,sv->f))))); yv=AAV(y);
  b=0; DO(AN(y), if(b=!AR(yv[i]))break;);
  if(b){
   RZ(x=df2(k,w,sldot(ds(CPOUND)))); xv=AV(x);
   DO(AN(y), if(!AR(yv[i]))RZ(yv[i]=reshape(sc(xv[i]),yv[i])););
  }
  RZ(z=from(grade1(grade1(k)),raze(grade2(y,u))));
 }
 if(1<r){RZ(z=gah(r,z)); ICPY(AS(z),AS(w0),r);}
 R z;
}

static DF1(jtcase1b){A h,u;V*sv;
 sv=VAV(self); h=sv->h;
 RZ(u=from(df1(w,sv->g),h));
 ASSERT(!AR(u),EVRANK);
 R df1(w,*AAV(u));
}

static DF1(jtcase1){A h,*hv;B b;I r,wr;V*sv;
 RZ(w);
 sv=VAV(self);
 wr=AR(w); r=jt->rank?jt->rank[1]:wr; r=MIN(r,sv->mr); jt->rank=0;
 if(b=!r&&wr&&AN(w)){h=sv->h; hv=AAV(h); DO(AN(h), if(!atomic(1,hv[i])){b=0; break;});}
 R b?case1a(w,self):rank1ex(w,self,r,jtcase1b);
}

static DF2(jtcase2){A u;V*sv;
 PREF2(jtcase2);
 sv=VAV(self);
 RZ(u=from(df2(a,w,sv->g),sv->h));
 ASSERT(!AR(u),EVRANK);
 R df2(a,w,*AAV(u));
}

static F2(jtgerfrom){A*av,*v,z;I ad,n;
 RZ(a&&w);  /* 1==AR(w)&&BOX&AT(w) */
 ASSERT(1>=AR(a),EVRANK);
 if(NUMERIC&AT(a))R from(a,w);
 else{
  ASSERT(BOX&AT(a),EVDOMAIN);
  n=AN(a); av=AAV(a); ad=(I)a*ARELATIVE(a);
  GATV(z,BOX,n,1,0); v=AAV(z);
  DO(n, RZ(*v++=gerfrom(AVR(i),w)););
  R z;
}}

F2(jtagenda){
 RZ(a&&w)
 if(NOUN&AT(w))R exg(gerfrom(w,a));
 R fdef(CATDOT,VERB, jtcase1,jtcase2, a,w,fxeachv(1L,a), VGERL, mr(w),lr(w),rr(w));
}

// When u^:gerund is encountered, we replace it with a verb that comes to one of these.
// This creates several names:
// sv->self data; fs=sv->f (the A block for the f operand); f1=f1 in sv->f (0 if sv->f==0); f2=f2 in sv->f (0 if sv->f==0);
//                gs=sv->g (the A block for the g operand); g1=f1 in sv->g (0 if sv->g==0); g2=f2 in sv->g (0 if sv->g==0)


static DF1(jtgcl1){DECLFG;A ff,*hv=AAV(sv->h);I d;
 RE(d=fdep(hv[1])); FDEPINC(d); ff=df2(df1(w,hv[1]),gs,ds(sv->id)); FDEPDEC(d);
 R df1(df1(w,hv[2]),ff);
}

// this is u^:gerund y
// Here, f1 is the original u and g1 is the original v; hs points to the gerund
// First, we run hv[1]->f1 on y (this is gerund v1, the selector/power);
// then we run (sv->id)->f2 on fs (the original u) and the result of selector/power (with self set to (sv->id):
//     sv->id is the original conjunction, executed a second time now that we have the selector/power
//     this is a conjunction execution, executing a u^:n form, and creates a derived verb to perform that function; call that verb ff
// then we execute gerund v2 on y (with self set to v2)
// then we execute ff on the result of (v2 y), with self set to ff
static DF1(jtgcr1){DECLFG;A ff,*hv=AAV(sv->h);I d; 
 RE(d=fdep(hv[1])); FDEPINC(d); ff=df2(fs,df1(w,hv[1]),ds(sv->id)); FDEPDEC(d);
 R df1(df1(w,hv[2]),ff);
}

static DF2(jtgcl2){DECLFG;A ff,*hv=AAV(sv->h);I d; 
 RE(d=fdep(hv[1])); FDEPINC(d); ff=df2(df2(a,w,hv[1]),gs,ds(sv->id)); FDEPDEC(d);
 R df2(df2(a,w,hv[0]),df2(a,w,hv[2]),ff);
}

static DF2(jtgcr2){DECLFG;A ff,*hv=AAV(sv->h);I d; 
 RE(d=fdep(hv[1])); FDEPINC(d); ff=df2(fs,df2(a,w,hv[1]),ds(sv->id)); FDEPDEC(d);
 R df2(df2(a,w,hv[0]),df2(a,w,hv[2]),ff);
}

// called for gerund} or ^:gerund forms.  id is the pseudocharacter for the conjunction (} or ^:)
// Creates a verb that will run jtgc[rl][12] to execute the gerunds on the xy arguments, and
// then  
// a is the original u, w is the original v
A jtgconj(J jt,A a,A w,C id){A hs,y;B na;I n;
 RZ(a&&w);
 ASSERT(VERB&AT(a)&&BOX&AT(w)||BOX&AT(a)&&VERB&AT(w),EVDOMAIN);
 na=1&&BOX&AT(a); y=na?a:w; n=AN(y);
 ASSERT(1>=AR(y),EVRANK);
 ASSERT(2==n||3==n,EVLENGTH);
 ASSERT(BOX&AT(y),EVDOMAIN);
 RZ(hs=fxeach(3==n?y:link(scc(CLEFT),y)));
 R fdef(id,VERB, na?jtgcl1:jtgcr1,na?jtgcl2:jtgcr2, a,w,hs, na?VGERL:VGERR, RMAX,RMAX,RMAX);
}

static DF1(jtgav1){DECLF;A ff,*hv=AAV(sv->h);I d;
 RE(d=fdep(hv[1])); FDEPINC(d); ff=df1(df1(w,hv[1]),ds(sv->id)); FDEPDEC(d);
 R df1(df1(w,hv[2]),ff);
}

static DF2(jtgav2){DECLF;A ff,*hv=AAV(sv->h);I d;
 RE(d=fdep(hv[1])); FDEPINC(d); ff=df1(df2(a,w,hv[1]),ds(sv->id)); FDEPDEC(d);
 R df2(df2(a,w,hv[0]),df2(a,w,hv[2]),ff);
}

A jtgadv(J jt,A w,C id){A hs;I n;
 RZ(w);
 ASSERT(BOX&AT(w),EVDOMAIN);
 n=AN(w);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(n&&n<=3,EVLENGTH);
 ASSERT(BOX&AT(w),EVDOMAIN);
 RZ(hs=fxeach(3==n?w:behead(reshape(num[4],w))));
 R fdef(id,VERB, jtgav1,jtgav2, w,0L,hs, VGERL, RMAX,RMAX,RMAX);
}


static DF1(jtgf1){A h=VAV(self)->h; R df1(  w,*AAV(h));}
static DF2(jtgf2){A h=VAV(self)->h; R df2(a,w,*AAV(h));}

A jtvger2(J jt,C id,A a,A w){A h,*hv,x;V*v;
 RZ(x=a?a:w);
 ASSERT(2==AN(x),EVLENGTH);
 RZ(h=fxeachv(1L,x)); hv=AAV(h); v=VAV(*hv);
 R fdef(id,VERB, jtgf1,jtgf2, x,a?w:0L, h, VGERL, (I)v->mr,(I)v->lr,(I)v->rr);
}    /* verify and define 2-element gerund */
