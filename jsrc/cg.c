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

// m@.v general (non-atomic)case, like dyad case below
static DF1(jtcase1b){A h,u;V*sv;
 F1PREFIP;sv=VAV(self); h=sv->h;
 RZ(u=from(df1(w,sv->g),h));  // not inplace
 ASSERT(!AR(u),EVRANK);
 R jtdf1(VAV(*AAV(u))->flag&VINPLACEOK1?jtinplace:jt,w,*AAV(u));  // inplace if the verb can handle it
}

// m@.v y.  First check for atomic verbs (special case case1a); if not, process through case1b.  Pass inplacing to case1b only if there is 1 cell
static DF1(jtcase1){A h,*hv;B b;I r,wr;V*sv;
 RZ(w);
 F1PREFIP; sv=VAV(self);
 wr=AR(w); r=jt->rank?jt->rank[1]:wr; r=MIN(r,sv->mr); jt->rank=0;
 if(b=!r&&wr&&AN(w)){h=sv->h; hv=AAV(h); DO(AN(h), if(!atomic(1,hv[i])){b=0; break;});}
 if(b){R case1a(w,self);}   // atomic: go there
 if(r>=wr){R jtcase1b(jtinplace,w,self);}  // If there is only 1 cell, execute on it, keeping inplaceability
 R rank1ex(w,self,r,jtcase1b);  // Otherwise loop over the cells (perforce not inplace)
}

// x m@.v y.  Run v (not inplace); select the "BOX" containing the verb to run; run it, inplacing as called for in the input
static DF2(jtcase2){A u;V*sv;
 F2PREFIP;PREF2(jtcase2);
 sv=VAV(self);
 RZ(u=from(df2(a,w,sv->g),sv->h));
 ASSERT(!AR(u),EVRANK);
 R jtdf2(VAV(*AAV(u))->flag&VINPLACEOK2?jtinplace:jt,a,w,*AAV(u));  // inplace if the verb can handle it
}

// @.n
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

F2(jtagenda){I flag;
 RZ(a&&w)
 if(NOUN&AT(w))R exg(gerfrom(w,a));
 // verb v.  Create a "BOX" type holding the verb form of each gerund
 A avb; RZ(avb = fxeachv(1L,a));
  // Calculate ASGSAFE from all of the verbs (both a and w), provided the user can handle it
 flag = VASGSAFE&VAV(w)->flag; A* avbv = AAV(avb); DO(AN(avb), flag &= VAV(*avbv)->flag; ++avbv;);  // Don't increment inside VAV!
 R fdef(CATDOT,VERB, jtcase1,jtcase2, a,w,avb, flag+((VGERL|VINPLACEOK1|VINPLACEOK2)|VAV(ds(CATDOT))->flag), mr(w),lr(w),rr(w));
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

// called for gerund} or ^:gerund forms.  id is the pseudocharacter for the modifier (} or ^:)
// Creates a verb that will run jtgc[rl][12] to execute the gerunds on the xy arguments, and
// then execute the operation
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

// verb executed for x v0`v1`v2} y
static DF2(jtgav2){F2PREFIP;DECLF;A ff,ffm,ffx,ffy,*hv=AAV(sv->h);I d;   // hv->gerunds
A protw = (A)((I)w+((I)jtinplace&JTINPLACEW)); A prota = (A)((I)a+((I)jtinplace&JTINPLACEA)); // protected addresses
 // first, get the indexes to use.  Since this is going to call m} again, we protect against
 // stack overflow - but why do we stop with the execution of the }, which is not inside the
 // potential loop?
 RE(d=fdep(hv[1])); FDEPINC(d);
 ffm = df2(a,w,hv[1]);  // x v1 y - no inplacing
 ff=df1(ffm,ds(sv->id)); FDEPDEC(d);   // now ff represents (x v1 y)}
 RZ(ffm);  // OK to fail after FDEPDEC
 // Protect any input that was returned by v1 (must be ][)
 if(a==ffm)jtinplace = (J)((I)jtinplace&~JTINPLACEA); if(w==ffm)jtinplace = (J)((I)jtinplace&~JTINPLACEW);
 PUSHZOMB
 // execute the gerunds that will give the arguments to ff
 // x v2 y - can inplace an argument that v0 is not going to use, except if a==w
 RZ(ffy = (VAV(hv[2])->f2)(a!=w&&(VAV(hv[2])->flag&VINPLACEOK2)?(J)((I)jtinplace&(sv->flag|~(VFATOPL|VFATOPR))):jt ,a,w,hv[2]));  // flag self about f, since flags may be needed in f
 // x v0 y - can inplace any unprotected argument
 RZ(ffx = (VAV(hv[0])->f2)((VAV(hv[0])->flag&VINPLACEOK2)?((J)((I)jtinplace&((ffm==w||ffy==w?~JTINPLACEW:~0)&(ffm==a||ffy==a?~JTINPLACEA:~0)))):jt ,a,w,hv[0]));
 // execute ff, i. e.  (x v1 y)} .  Allow inplacing xy unless protected by the caller
 POPZOMB; R (VAV(ff)->f2)(VAV(ff)->flag&VINPLACEOK2?( (J)((I)jt|((ffx!=protw&&ffx!=prota?JTINPLACEA:0)+(ffy!=protw&&ffy!=prota?JTINPLACEW:0))) ):jt,ffx,ffy,ff);
}

// handle v0`v1[`v2]} to create the verb to process it when [x] and y arrive
// The id is the pseudocharacter for the function, which is passed in as the pchar for the derived verb
A jtgadv(J jt,A w,C id){A hs;I n;
 RZ(w);
 ASSERT(BOX&AT(w),EVDOMAIN);
 n=AN(w);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(n&&n<=3,EVLENGTH);  // verify 1-3 gerunds
 ASSERT(BOX&AT(w),EVDOMAIN);
 RZ(hs=fxeach(3==n?w:behead(reshape(num[4],w))));   // convert to v0`v0`v0, v1`v0`v1, or v0`v1`v2; convert each gerund to verb
 // hs is a BOX array, but its elements are VERBs
 // The derived verb is ASGSAFE if all the components are; it has gerund left-operand; and it supports inplace operation on the dyad
 // Also set the LSB flags to indicate whether v0 is u@[ or u@]
 I flag=(VAV(AAV(hs)[0])->flag&VAV(AAV(hs)[1])->flag&VAV(AAV(hs)[2])->flag&VASGSAFE)+(VGERL|VINPLACEOK2)+atoplr(AAV(hs)[0]);
 R fdef(id,VERB, jtgav1,jtgav2, w,0L,hs,flag, RMAX,RMAX,RMAX);  // create the derived verb
}


static DF1(jtgf1){A h=VAV(self)->h; R df1(  w,*AAV(h));}
static DF2(jtgf2){A h=VAV(self)->h; R df2(a,w,*AAV(h));}

A jtvger2(J jt,C id,A a,A w){A h,*hv,x;V*v;
 RZ(x=a?a:w);
 ASSERT(2==AN(x),EVLENGTH);
 RZ(h=fxeachv(1L,x)); hv=AAV(h); v=VAV(*hv);
 R fdef(id,VERB, jtgf1,jtgf2, x,a?w:0L, h, VGERL, (I)v->mr,(I)v->lr,(I)v->rr);
}    /* verify and define 2-element gerund */
