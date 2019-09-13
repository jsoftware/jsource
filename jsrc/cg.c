/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Gerunds ` and `:                                          */

#include "j.h"


// Bivalent entry point.
// self is a cyclic iterator
// we extract the pointer to the verb to be executed, advance the cycle pointer with wraparound, and call the verb
// passes inplacing through
static DF2(jtexeccyclicgerund){  // call is w,self or a,w,self
 // find the real self, valence-dependent
 RZ(w);
 F2PREFIP;
 I ismonad=(AT(w)>>VERBX)&1; self=ismonad?w:self;
 I nexttoexec=FAV(self)->localuse.lI; A vbtoexec=AAV(FAV(self)->fgh[2])[nexttoexec]; AF fntoexec=FAV(vbtoexec)->valencefns[1-ismonad]; ASSERT(fntoexec,EVDOMAIN); // get fn to exec
 ++nexttoexec; nexttoexec=AN(FAV(self)->fgh[2])==nexttoexec?0:nexttoexec; FAV(self)->localuse.lI=nexttoexec; // cyclically advance exec pointer
 w=ismonad?vbtoexec:w; R (*fntoexec)(jtinplace,a,w,vbtoexec);  // vector to the function, as a,vbtoexec or a,w,vbtoexec as appropriate
}

// w is a verb that refers to a cyclic gerund which is stored in h.
// Result is a clone that is used to hold which gerund is next to execute
// the gerund must not be empty
A jtcreatecycliciterator(J jt, A w){A z;
 // Create the clone, point it to come to the execution point, set the next-verb number to 0
 RZ(z=ca(w)); FAV(z)->valencefns[0]=FAV(z)->valencefns[1]=jtexeccyclicgerund; FAV(z)->localuse.lI=0;
 // clear the BOXATOP flags for this iterator
 FAV(z)->flag2=0;
 R z;
}

// w is a gerund whose max rank is r.  Result is a boxed array of VERBs, one for each gerund, if they are well forrmed
A jtfxeachv(J jt,I r,A w){A*wv,x,z,*zv;I n;
 RZ(w);
 n=AN(w); wv=AAV(w); 
 ASSERT(r>=AR(w),EVRANK);  // max rank allowed
 ASSERT(n,EVLENGTH);  // gerund must not be empty
 ASSERT(BOX&AT(w),EVDOMAIN);   // must be boxed
 GATV(z,BOX,n,AR(w),AS(w)); zv=AAV(z);  // allocate one box per gerund
 DO(n, RZ(zv[i]=x=fx(wv[i])); ASSERT(VERB&AT(x),EVDOMAIN););   // create verb, verify it is a verb
 R z;
}

// run jtfx on each box in w, turning AR into an A block
F1(jtfxeach){RETF(every(w,0L,jtfx));}
// run jtfx on each box in w, EXCEPT on nouns, which we return as is
F1(jtfxeachacv){RETF(every(w,w,jtfx));}  // the second w is just any nonzero

static DF1(jtcon1){A h,*hv,*x,z;V*sv;
 PREF1(jtcon1);
 sv=FAV(self); h=sv->fgh[2]; hv=AAV(h);
 GATV(z,BOX,AN(h),AR(h),AS(h)); x=AAV(z);
 DQ(AN(h), RZ(*x++=CALL1(FAV(*hv)->valencefns[0],  w,*hv)); ++hv;);
 R ope(z);
}

static DF2(jtcon2){A h,*hv,*x,z;V*sv;
 PREF2(jtcon2);
 sv=FAV(self); h=sv->fgh[2]; hv=AAV(h);
 GATV(z,BOX,AN(h),AR(h),AS(h)); x=AAV(z);
 DQ(AN(h), RZ(*x++=CALL2(FAV(*hv)->valencefns[1],a,w,*hv)); ++hv;);
 R ope(z);
}

// u`:3 insert 
static DF1(jtinsert){A hs,*hv,z;I hfx,j,m,n;A *old;
 RZ(w);
 n=IC(w); j=n-1; hs=FAV(self)->fgh[2]; m=AN(hs); hfx=j%m; hv=AAV(hs);  // m cannot be 0
 if(!n)R df1(w,iden(*hv));
 RZ(z=from(num[-1],w));
 old=jt->tnextpushp;
 --m; DQ(n-1, --j; --hfx; hfx=(hfx<0)?m:hfx; RZ(z=CALL2(FAV(hv[hfx])->valencefns[1],from(sc(j),w),z,hv[hfx])); z=gc(z,old);)
 RETF(z);
}

// u`:m
F2(jtevger){A hs;I k;
 RZ(a&&w);
 RE(k=i0(w)); 
 if(k==GTRAIN)R exg(a);
 RZ(hs=fxeachv(RMAX,a));
 switch(k){
  case GAPPEND:
   R fdef(0,CGRCO,VERB, jtcon1,jtcon2, a,w,hs, VGERL, RMAX,RMAX,RMAX);
  case GINSERT:
   ASSERT(1>=AR(a),EVRANK);
   R fdef(0,CGRCO,VERB, jtinsert,0L,   a,w,hs, VGERL, RMAX,0L,0L);
  default:
   ASSERT(0,EVDOMAIN);
}}

F2(jttie){RZ(a&&w); R over(VERB&AT(a)?arep(a):a,VERB&AT(w)?arep(w):w);}


static B jtatomic(J jt,C m,A w){A f,g;B ax,ay,vf,vg;C c,id;V*v;
 // char types that are atomic both dyad and monad
 // CFCONS should not be atomic.  But who would use it by itself?  It must be part of
 // some compound, so we will pretend it's atomic.  It's too much trouble to add an 'atomic result'
 // type, and tests rely on 1: being treated as atomic.  It's been this way a long time.
 static C atomic12[]={CMIN, CLE, CMAX, CGE, CPLUS, CPLUSCO, CSTAR, CSTARCO, CMINUS, CDIV, CROOT, 
     CEXP, CLOG, CSTILE, CBANG, CLEFT, CRIGHT, CJDOT, CCIRCLE, CRDOT, CHGEOM, CFCONS, 0};
 // atomic monad-only
 static C atomic1[]={CNOT, CHALVE, 0};
 // atomic dyad-only
 static C atomic2[]={CEQ, CLT, CGT, CPLUSDOT, CSTARDOT, CNE, 0};
 RZ(w&&VERB&AT(w));
 v=FAV(w); id=v->id;
 if(strchr(atomic12,id)||strchr(1==m?atomic1:atomic2,id))R 1;
 f=v->fgh[0]; vf=f&&VERB&AT(f); ax=f&&NOUN&AT(f)&&!AR(f);
 g=v->fgh[1]; vg=g&&VERB&AT(g); ay=g&&NOUN&AT(g)&&!AR(g);
 switch(id){  // should handle @. ?
  case CAT:
  case CATCO:  R atomic(1,f)&&atomic(m,g);
  case CUNDER:
  case CUNDCO: R atomic(m,f)&&atomic(1,g);
  case CAMPCO: R atomic(m,f)&&atomic(1,g);
  case CQQ:    R ax||atomic(m,f);
  case CFORK:  R (CCAP==ID(f)?atomic(1,g):atomic(m,f)&&atomic(2,g))&&atomic(m,v->fgh[2]);
  case CHOOK:  R atomic(2,f)&&atomic(1,g);
  case CTILDE: R NAME&AT(f)?atomic(m,fix(f,zeroionei[0])):atomic(2,f);
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
 ASSERT(1>=AR(a)&&1>=AR(w),EVRANK);  // both ranks<2
 ASSERT((!AN(a)||BOX&AT(a))&&(!AN(w)||BOX&AT(w)),EVDOMAIN);  // both boxed or empty
 RZ(f=qq(atop(ds(CBOX),ds(CCOMMA)),zeroionei[0]));  // f = <@,"0
 R df2(box(spellout(c)),df2(a,w,f),f);   // gerund: (<c) <@,"0 a <@,"0 w
}

// u@.v y atomic.  Operate on the nub of y and then rearrange the results
// kludge TODO: Don't do this if the nub is almost as big as y; also find a way to expose this logic
// to general verbs
static DF1(jtcase1a){F1PREFIP;A g,h,*hv,k,t,u,w0=w,x,y,*yv,z;B b;I r,*xv;V*sv;
 RZ(w);
 r=AR(w);
 if(1<r)RZ(w=ravel(w));  // will return virtual block
 sv=FAV(self); g=sv->fgh[1];
 // Calculate v y.  If v is atomic, apply v y, else v"0 y
 if(atomic(1,g))RZ(k=df1(w,g))
 else{RZ(k=df1(w,qq(g,zeroionei[0]))); ASSERT(AR(k)==AR(w)&&AN(k)==AN(w),EVRANK);}
 if(B01&AT(k)){
  // v produced a binary list.  Pull out the operands for u[0] and u[1], operate on them individually,
  // and interleave the results
  h=sv->fgh[2]; ASSERT(2<=AN(h),EVINDEX); hv=AAV(h);
  RZ(x=df1(t=repeat(not(k),w),hv[0])); if(!AR(x))RZ(x=reshape(tally(t),x));
  RZ(y=df1(t=repeat(k,     w),hv[1])); if(!AR(y))RZ(y=reshape(tally(t),y));
  RZ(z=!AN(x)?y:!AN(y)?x:from(grade1(grade1(k)),over(x,y)));
 }else{
  // v produced non-binary. apply k u/. y and shuffle the results to their proper positions
  RZ(u=nub(k));
  RZ(y=df2(k,w,sldot(gjoin(CATCO,box(scc(CBOX)),from(u,sv->fgh[0]))))); yv=AAV(y);
  b=0; DO(AN(y), if(b=!AR(yv[i]))break;);
  if(b){
   RZ(x=df2(k,w,sldot(ds(CPOUND)))); xv=AV(x);
   y=rifvs(y); yv=AAV(y);   // mustn't install into virtual
   DO(AN(y), if(!AR(yv[i])){RZ(z=reshape(sc(xv[i]),yv[i])); INSTALLBOXNF(y,yv,i,z);});
  }
  RZ(z=from(grade1(grade1(k)),raze(grade2(y,u))));
 }
 if(1<r){RZ(z=virtualip(z,0,r)); AN(z)=AN(w0); MCISH(AS(z),AS(w0),r);}
 R z;
}

// m@.v general (non-atomic)case, like dyad case below
static DF1(jtcase1b){A h,u;V*sv;
 F1PREFIP;sv=FAV(self); h=sv->fgh[2];
 RZ(u=from(df1(w,sv->fgh[1]),h));  // not inplace
 ASSERT(!AR(u),EVRANK);
 R jtdf1(FAV(*AAV(u))->flag&VJTFLGOK1?jtinplace:jt,w,*AAV(u));  // inplace if the verb can handle it
}

// m@.v y.  First check for atomic verbs (special case case1a); if not, process through case1b.  Pass inplacing to case1b only if there is 1 cell
static DF1(jtcase1){A h,*hv;B b;I r,wr;V*sv;
 RZ(w);
 F1PREFIP; sv=FAV(self);
 wr=AR(w); r=(RANKT)jt->ranks; r=wr<r?wr:r; r=MIN(r,sv->mr); RESETRANK;
 if(b=!r&&wr&&AN(w)){h=sv->fgh[2]; hv=AAV(h); DO(AN(h), if(!atomic(1,hv[i])){b=0; break;});}
 if(b){R case1a(w,self);}   // atomic: go there
 if(r>=wr){R jtcase1b(jtinplace,w,self);}  // If there is only 1 cell, execute on it, keeping inplaceability
 R rank1ex(w,self,r,jtcase1b);  // Otherwise loop over the cells (perforce not inplace)
}

// x m@.v y.  Run v (not inplace); select the "BOX" containing the verb to run; run it, inplacing as called for in the input
static DF2(jtcase2){A u;V*sv;
 F2PREFIP;PREF2(jtcase2);
 sv=FAV(self);
 RZ(u=from(df2(a,w,sv->fgh[1]),sv->fgh[2]));
 ASSERT(!AR(u),EVRANK);
 R jtdf2(FAV(*AAV(u))->flag&VJTFLGOK2?jtinplace:jt,a,w,*AAV(u));  // inplace if the verb can handle it
}

// @.n
static F2(jtgerfrom){A*av,*v,z;I n;
 RZ(a&&w);  /* 1==AR(w)&&BOX&AT(w) */
 ASSERT(1>=AR(a),EVRANK);
 if(NUMERIC&AT(a))R from(a,w);
 else{
  ASSERT(BOX&AT(a),EVDOMAIN);
  n=AN(a); av=AAV(a); 
  GATV0(z,BOX,n,1); v=AAV(z);
  DO(n, RZ(*v++=gerfrom(av[i],w)););
  R z;
}}

F2(jtagenda){I flag;
 RZ(a&&w)
 if(NOUN&AT(w))R exg(gerfrom(w,a));
 // verb v.  Create a "BOX" type holding the verb form of each gerund
 A avb; RZ(avb = fxeachv(1L,a));
  // Calculate ASGSAFE from all of the verbs (both a and w), provided the user can handle it
 flag = VASGSAFE&FAV(w)->flag; A* avbv = AAV(avb); DQ(AN(avb), flag &= FAV(*avbv)->flag; ++avbv;);  // Don't increment inside FAV!
 R fdef(0,CATDOT,VERB, jtcase1,jtcase2, a,w,avb, flag+((VGERL|VJTFLGOK1|VJTFLGOK2)|FAV(ds(CATDOT))->flag), mr(w),lr(w),rr(w));
}

// When u^:gerund is encountered, we replace it with a verb that comes to one of these.
// This creates several names:
// sv->self data; fs=sv->fgh[0] (the A block for the f operand); f1=f1 in sv->fgh[0] (0 if sv->fgh[0]==0); f2=f2 in sv->fgh[0] (0 if sv->fgh[0]==0);
//                gs=sv->fgh[1] (the A block for the g operand); g1=f1 in sv->fgh[1] (0 if sv->fgh[1]==0); g2=f2 in sv->fgh[1] (0 if sv->fgh[1]==0)


static DF1(jtgcl1){DECLFG;A ff,*hv=AAV(sv->fgh[2]);
 STACKCHKOFL FDEPINC(d=fdep(hv[1])); ff=df2(df1(w,hv[1]),gs,ds(sv->id)); FDEPDEC(d);
 R df1(df1(w,hv[2]),ff);
}

// this is u^:gerund y
// Here, f1 is the original u and g1 is the original v; hs points to the gerund
// First, we run hv[1]->valencefns[0] on y (this is gerund v1, the selector/power);
// then we run (sv->id)->valencefns[1] on fs (the original u) and the result of selector/power (with self set to (sv->id):
//     sv->id is the original conjunction, executed a second time now that we have the selector/power
//     this is a conjunction execution, executing a u^:n form, and creates a derived verb to perform that function; call that verb ff
// then we execute gerund v2 on y (with self set to v2)
// then we execute ff on the result of (v2 y), with self set to ff
static DF1(jtgcr1){DECLFG;A ff,*hv=AAV(sv->fgh[2]);
 STACKCHKOFL FDEPINC(d=fdep(hv[1])); ff=df2(fs,df1(w,hv[1]),ds(sv->id)); FDEPDEC(d);
 R df1(df1(w,hv[2]),ff);
}

static DF2(jtgcl2){DECLFG;A ff,*hv=AAV(sv->fgh[2]);
 STACKCHKOFL FDEPINC(d=fdep(hv[1])); ff=df2(df2(a,w,hv[1]),gs,ds(sv->id)); FDEPDEC(d);
 R df2(df2(a,w,hv[0]),df2(a,w,hv[2]),ff);
}

static DF2(jtgcr2){DECLFG;A ff,*hv=AAV(sv->fgh[2]);
 STACKCHKOFL FDEPINC(d=fdep(hv[1])); ff=df2(fs,df2(a,w,hv[1]),ds(sv->id)); FDEPDEC(d);
 R df2(df2(a,w,hv[0]),df2(a,w,hv[2]),ff);
}

// called for gerund} or ^:gerund forms.  id is the pseudocharacter for the modifier (} or ^:)
// Creates a verb that will run jtgc[rl][12] to execute the gerunds on the xy arguments, and
// then execute the operation
// a is the original u, w is the original v
A jtgconj(J jt,A a,A w,C id){A hs,y;B na;I n;
 RZ(a&&w);
 ASSERT(VERB&AT(a)&&BOX&AT(w)||BOX&AT(a)&&VERB&AT(w),EVDOMAIN);
 na=1&&BOX&AT(a); y=na?a:w; n=AN(y);  // na is 1 for gerund}; y is the gerund
 ASSERT(1>=AR(y),EVRANK);
 ASSERT(2==n||3==n,EVLENGTH);
 ASSERT(BOX&AT(y),EVDOMAIN);
 RZ(hs=fxeach(3==n?y:link(scc(CLEFT),y)));
 R fdef(0,id,VERB, na?jtgcl1:jtgcr1,na?jtgcl2:jtgcr2, a,w,hs, na?VGERL:VGERR, RMAX,RMAX,RMAX);
}

// verb executed for v0`v1`v2} y
static DF1(jtgav1){DECLF;A ff,ffm,ffx,*hv=AAV(sv->fgh[2]);
 // first, get the indexes to use.  Since this is going to call m} again, we protect against
 // stack overflow in the loop in case the generated ff generates a recursive call to }
 // If the AR is a noun, just leave it as is
 FDEPINC(d=fdep(hv[1]));
 ffm = df1(w,hv[1]);  // x v1 y - no inplacing
 FDEPDEC(d);
 RZ(ffm);  // OK to fail after FDEPDEC
 RZ(ff=df1(ffm,ds(sv->id)));   // now ff represents (v1 y)}
 if(AT(hv[2])&NOUN){ffx=hv[2];}else{RZ(ffx=df1(w,hv[2]))}
 R df1(ffx,ff);
}

static DF2(jtgav2){F2PREFIP;DECLF;A ff,ffm,ffx,ffy,*hv=AAV(sv->fgh[2]);  // hv->gerunds
A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); A prota = (A)(intptr_t)((I)a+((I)jtinplace&JTINPLACEA)); // protected addresses
 // first, get the indexes to use.  Since this is going to call m} again, we protect against
 // stack overflow in the loop in case the generated ff generates a recursive call to }
 // If the AR is a noun, just leave it as is
 FDEPINC(d=fdep(hv[1]));
 ffm = df2(a,w,hv[1]);  // x v1 y - no inplacing.
 FDEPDEC(d);
 RZ(ffm);  // OK to fail after FDEPDEC
 RZ(ff=df1(ffm,ds(sv->id)));   // now ff represents (x v1 y)}  .  Alas, ffm can no longer be virtual
 // Protect any input that was returned by v1 (must be ][)
 if(a==ffm)jtinplace = (J)(intptr_t)((I)jtinplace&~JTINPLACEA); if(w==ffm)jtinplace = (J)(intptr_t)((I)jtinplace&~JTINPLACEW);
 PUSHZOMB
 // execute the gerunds that will give the arguments to ff.  But if they are nouns, leave as is
 // x v2 y - can inplace an argument that v0 is not going to use, except if a==w
 RZ(ffy = (FAV(hv[2])->valencefns[1])(a!=w&&(FAV(hv[2])->flag&VJTFLGOK2)?(J)(intptr_t)((I)jtinplace&(sv->flag|~(VFATOPL|VFATOPR))):jt ,a,w,hv[2]));  // flag self about f, since flags may be needed in f
 // x v0 y - can inplace any unprotected argument
 RZ(ffx = (FAV(hv[0])->valencefns[1])((FAV(hv[0])->flag&VJTFLGOK2)?((J)(intptr_t)((I)jtinplace&((ffm==w||ffy==w?~JTINPLACEW:~0)&(ffm==a||ffy==a?~JTINPLACEA:~0)))):jt ,a,w,hv[0]));
 // execute ff, i. e.  (x v1 y)} .  Allow inplacing xy unless protected by the caller.  No need to pass WILLOPEN status, since the verb can't use it.  ff is needed to give access to m
 POPZOMB; R (FAV(ff)->valencefns[1])(FAV(ff)->flag&VJTFLGOK2?( (J)(intptr_t)((I)jt|((ffx!=protw&&ffx!=prota?JTINPLACEA:0)+(ffy!=protw&&ffy!=prota?JTINPLACEW:0))) ):jt,ffx,ffy,ff);
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
 // hs is a BOX array, but its elements are ARs
 // The derived verb is ASGSAFE if all the components are; it has gerund left-operand; and it supports inplace operation on the dyad
 // Also set the LSB flags to indicate whether v0 is u@[ or u@]
 ASSERT(AT(AAV(hs)[0])&VERB&&AT(AAV(hs)[1])&VERB&&AT(AAV(hs)[2])&VERB,EVDOMAIN);
 I flag=(FAV(AAV(hs)[0])->flag&FAV(AAV(hs)[1])->flag&FAV(AAV(hs)[2])->flag&VASGSAFE)+(VGERL|VJTFLGOK2)+atoplr(AAV(hs)[0]);
 R fdef(0,id,VERB, jtgav1,jtgav2, w,0L,hs,flag, RMAX,RMAX,RMAX);  // create the derived verb
}


static DF1(jtgf1){A h=FAV(self)->fgh[2]; R df1(  w,*AAV(h));}
static DF2(jtgf2){A h=FAV(self)->fgh[2]; R df2(a,w,*AAV(h));}

A jtvger2(J jt,C id,A a,A w){A h,*hv,x;V*v;
 RZ(x=a?a:w);
 ASSERT(2==AN(x),EVLENGTH);
 RZ(h=fxeachv(1L,x)); hv=AAV(h); v=VAV(*hv);
 R fdef(0,id,VERB, jtgf1,jtgf2, x,a?w:0L, h, VGERL, (I)v->mr,lrv(v),rrv(v));
}    /* verify and define 2-element gerund */
