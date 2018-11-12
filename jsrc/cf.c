/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Forks                                                     */

#include "j.h"

#define TC1(t)          (t&NOUN?0:t&VERB?3:t&CONJ?2:1)
#define BD(ft,gt)       (4*TC1(ft)+TC1(gt))
#define TDECL           V*sv=FAV(self);A fs=sv->fgh[0],gs=sv->fgh[1],hs=sv->fgh[2]

// handle fork, with support for in-place operations
#define FOLK1 {A fx,hx; PUSHZOMB; A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); RZ(hx=CALL1(h1,  w,hs)); /* the call to h is not inplaceable */ \
/* If any result equals protw, it must not be inplaced: if original w is inplaceable, protw will not match anything */ \
/* the call to f is inplaceable if the caller allowed inplacing, and f is inplaceable, and the hx is NOT the same as y.  Here only the LSB of jtinplace is used */ \
RZ(fx=(f1)(((FAV(fs)->flag>>VJTFLGOK1X)&(hx!=w))?jtinplace:jt,  w,fs)); /* CALL1 with variable jt */ \
/* The call to g is inplaceable if g allows it, UNLESS fx or hx is the same as disallowed y */ \
POPZOMB; RZ(z=(g2)(FAV(gs)->flag&VJTFLGOK2?( (J)(intptr_t)((I)jt|((fx!=protw?JTINPLACEA:0)+(hx!=protw?JTINPLACEW:0))) ):jt,fx,hx,gs));}

#define FOLK2 {A fx,hx; PUSHZOMB; A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); A prota = (A)(intptr_t)((I)a+((I)jtinplace&JTINPLACEA)); \
RZ(hx=(h2)(a!=w&&(FAV(hs)->flag&VJTFLGOK2)?(J)(intptr_t)((I)jtinplace&(sv->flag|~(VFATOPL|VFATOPR))):jt,a,w,hs)); /* inplace h if f is x@], but not if a==w */  \
/* If any result equals protw/prota, it must not be inplaced: if original w/a is inplaceable, protw/prota will not match anything */ \
/* the call to f is inplaceable if the caller allowed inplacing, and f is inplaceable; but only where hx is NOT the same as x or y.  Both flags in jtinplace are used */ \
RZ(fx=(f2)((FAV(fs)->flag&VJTFLGOK2)?((J)(intptr_t)((I)jtinplace&((hx==w?~JTINPLACEW:~0)&(hx==a?~JTINPLACEA:~0)))):jt ,a,w,fs)); \
/* The call to g is inplaceable if g allows it, UNLESS fx or hx is the same as disallowed x/y */ \
POPZOMB; RZ(z=(g2)(FAV(gs)->flag&VJTFLGOK2?( (J)(intptr_t)((I)jt|((fx!=protw&&fx!=prota?JTINPLACEA:0)+(hx!=protw&&hx!=prota?JTINPLACEW:0))) ):jt,fx,hx,gs));}

// similar for cap, but now we can inplace the call to h
#define CAP1 {PUSHZOMB; A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); \
A hx; RZ(hx=(h1)((J)(intptr_t)(((I)jtinplace&(~(JTWILLBEOPENED+JTCOUNTITEMS))) + ((-((FAV(hs)->flag>>VJTFLGOK1X)&JTINPLACEW)) & FAV(gs)->flag2 & JTWILLBEOPENED+JTCOUNTITEMS)),w,hs));  /* inplace g.  jtinplace is set for g */ \
/* inplace gx unless it is protected */ \
POPZOMB; \
jtinplace=(J)(intptr_t)(((I)jtinplace&~(JTINPLACEW))+((hx!=protw)*JTINPLACEW));  \
jtinplace=FAV(gs)->flag&VJTFLGOK1?jtinplace:jt; \
RZ(z=(g1)(jtinplace,hx,gs));}

#define CAP2 {PUSHZOMB; A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); A prota = (A)(intptr_t)((I)a+((I)jtinplace&JTINPLACEA)); \
A hx; RZ(hx=(h2)((J)(intptr_t)(((I)jtinplace&(~(JTWILLBEOPENED+JTCOUNTITEMS))) + ((-((FAV(hs)->flag>>VJTFLGOK1X)&JTINPLACEW)) & FAV(gs)->flag2 & JTWILLBEOPENED+JTCOUNTITEMS)),a,w,hs));  /* inplace g */ \
/* inplace gx unless it is protected */ \
jtinplace=(J)(intptr_t)(((I)jtinplace&~(JTINPLACEW))+((hx!=prota)&(hx!=protw)*JTINPLACEW));  \
jtinplace=FAV(gs)->flag&VJTFLGOK1?jtinplace:jt; \
RZ(z=(g1)(jtinplace,hx,gs));}



DF1(jtcork1){F1PREFIP;DECLFGH;PROLOG(0026);A z;  CAP1; EPILOG(z);}
DF2(jtcork2){F2PREFIP;DECLFGH;PROLOG(0027);A z;  CAP2; EPILOG(z);}

static DF1(jtfolk1){F1PREFIP;DECLFGH;PROLOG(0028);A z; FOLK1; EPILOG(z);}
static DF2(jtfolk2){F2PREFIP;DECLFGH;PROLOG(0029);A z; FOLK2; EPILOG(z);}

// see if f is defined as [:, perhaps through a chain of references
static B jtcap(J jt,A x){V*v;L *l;
 while(v=VAV(x),CTILDE==v->id&&NAME&AT(v->fgh[0])&&(l=syrd(v->fgh[0]))&&(x=l->val));  // go through chain of names
 R CCAP==v->id;
}


// nvv forks.  n must not be inplaced, since the fork may be reused.  hx can be inplaced unless protected by caller.
static DF1(jtnvv1){F1PREFIP;DECLFGH;PROLOG(0032);
 PUSHZOMB; A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); A hx; RZ(hx=(h1)(jtinplace,  w,hs));
 POPZOMB; A z; RZ(z=(g2)((J)(intptr_t)((I)jt + ((FAV(gs)->flag>>VJTFLGOK2X)&(hx!=protw))*JTINPLACEW),fs,hx,gs)); EPILOG(z);}
static DF2(jtnvv2){F1PREFIP;DECLFGH;PROLOG(0033);
 PUSHZOMB; A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); A prota = (A)(intptr_t)((I)a+((I)jtinplace&JTINPLACEA)); A hx; RZ(hx=(h2)(jtinplace,a,w,hs));
 POPZOMB; A z; RZ(z=(g2)((J)(intptr_t)((I)jt + ((FAV(gs)->flag>>VJTFLGOK2X)&(hx!=protw)&(hx!=prota))*JTINPLACEW),fs,hx,gs)); EPILOG(z);}

static DF2(jtfolkcomp){F2PREFIP;DECLFGH;PROLOG(0034);A z;AF f;
 RZ(a&&w);
 if(f=atcompf(a,w,self))z=f(jt,a,w,self); else if(cap(fs))CAP2 else FOLK2;
 EPILOG(z);
}

static DF2(jtfolkcomp0){F2PREFIP;DECLFGH;PROLOG(0035);A z;AF f;D oldct=jt->ct;
 RZ(a&&w);
 jt->ct=0;
 if(f=atcompf(a,w,self))z=f(jt,a,w,self); else if(cap(fs))CAP2 else FOLK2;
 jt->ct=oldct;
 EPILOG(z);
}

static DF1(jtcharmapa){V*v=FAV(self); R charmap(w,FAV(v->fgh[2])->fgh[0],v->fgh[0]);}
static DF1(jtcharmapb){V*v=FAV(self); R charmap(w,FAV(v->fgh[0])->fgh[0],FAV(v->fgh[2])->fgh[0]);}

// Create the derived verb for a fork.  Insert in-placeable flags based on routine, and asgsafe based on fgh
A jtfolk(J jt,A f,A g,A h){A p,q,x,y;AF f1=jtfolk1,f2=jtfolk2;B b;C c,fi,gi,hi;I flag,flag2=0,j,m=-1;V*fv,*gv,*hv,*v;
 RZ(f&&g&&h);
 gv=FAV(g); gi=gv->id;
 hv=FAV(h); hi=hv->id;
 // Start flags with ASGSAFE (if g and h are safe), and with INPLACEOK to match the setting of f1,f2.  Turn off inplacing that neither f nor h can handle
 if(NOUN&AT(f)){  /* nvv, including y {~ x i. ] */
  flag=(hv->flag&(VJTFLGOK1|VJTFLGOK2))+((gv->flag&hv->flag)&VASGSAFE);  // We accumulate the flags for the derived verb.  Start with ASGSAFE if all descendants are.
  // Mark the noun as non-inplaceable.  If the derived verb is used in another sentence, it must first be
  // assigned to a name, which will protects values inside it.
  ACIPNO(f);  // This justifies keeping the result ASGSAFE
  f1=jtnvv1;
  if(LIT&AT(f)&&1==AR(f)&&gi==CTILDE&&CFROM==ID(gv->fgh[0])&&hi==CFORK){
   x=hv->fgh[0];
   if(LIT&AT(x)&&1==AR(x)&&CIOTA==ID(hv->fgh[1])&&CRIGHT==ID(hv->fgh[2])){f1=jtcharmapa;  flag &=~(VJTFLGOK1);}
  }
  R fdef(0,CFORK,VERB, f1,jtnvv2, f,g,h, flag, RMAX,RMAX,RMAX);
 }
 fv=FAV(f); fi=cap(f)?CCAP:fv->id; // if f is a name defined as [:, detect that now & treat it as if capped fork
 if(fi!=CCAP){
  // nvv or vvv fork.  inplace if f or h can handle it, ASGSAFE only if all 3 verbs can
  flag=((fv->flag|hv->flag)&(VJTFLGOK1|VJTFLGOK2))+((fv->flag&gv->flag&hv->flag)&VASGSAFE);  // We accumulate the flags for the derived verb.  Start with ASGSAFE if all descendants are.
  // if g has WILLOPEN, indicate WILLBEOPENED in f/h

 }else{
  // capped fork.  inplace if h can handle it, ASGSAFE if gh are safe
  flag=(hv->flag&(VJTFLGOK1|VJTFLGOK2))+((gv->flag&hv->flag)&VASGSAFE);  // We accumulate the flags for the derived verb.  Start with ASGSAFE if all descendants are.
 }
 switch(fi){
  case CCAP:   if(gi==CBOX)flag2|=VF2BOXATOP1|VF2BOXATOP2|VF2ISCCAP; f1=jtcork1; f2=jtcork2;
               break; /* [: g h */
  case CSLASH: if(gi==CDIV&&hi==CPOUND&&CPLUS==ID(fv->fgh[0])){f1=jtmean; flag|=VIRS1; flag &=~(VJTFLGOK1);} break;  /* +/%# */
  case CAMP:   /* x&i.     { y"_ */
  case CFORK:  /* (x i. ]) { y"_ */
   if(hi==CQQ&&(y=hv->fgh[0],LIT&AT(y)&&1==AR(y))&&equ(ainf,hv->fgh[1])&&
       (x=fv->fgh[0],LIT&AT(x)&&1==AR(x))&&CIOTA==ID(fv->fgh[1])&&
       (fi==CAMP||CRIGHT==ID(fv->fgh[2]))){f1=jtcharmapb; flag &=~(VJTFLGOK1);} break;
  case CAT:    /* <"1@[ { ] */
   if(gi==CLBRACE&&hi==CRIGHT){                                   
    p=fv->fgh[0]; q=fv->fgh[1]; 
    if(CLEFT==ID(q)&&CQQ==ID(p)&&(v=VAV(p),x=v->fgh[0],CLT==ID(x)&&equ(one,v->fgh[1]))){f2=jtsfrom; flag &=~(VJTFLGOK2);}
   }
 }
 switch(fi==CCAP?gi:hi){
  case CQUERY:  if(hi==CDOLLAR||hi==CPOUND){f2=jtrollk; flag &=~(VJTFLGOK2);}  break;
  case CQRYDOT: if(hi==CDOLLAR||hi==CPOUND){f2=jtrollkx; flag &=~(VJTFLGOK2);} break;
  case CICAP:   m=7; if(fi==CCAP){if(hi==CNE)f1=jtnubind; else if(FIT0(CNE,hv)){f1=jtnubind0; flag &=~(VJTFLGOK1);}} break;
  case CSLASH:  c=ID(gv->fgh[0]); m=c==CPLUS?4:c==CPLUSDOT?5:c==CSTARDOT?6:-1; 
                if(fi==CCAP&&vaid(gv->fgh[0])&&vaid(h)){f2=jtfslashatg; flag &=~(VJTFLGOK2);}
                break;
  case CFCONS:  if(hi==CFCONS){x=hv->fgh[2]; j=*BAV(x); m=B01&AT(x)?(gi==CIOTA?j:gi==CICO?2+j:-1):-1;} break;
  case CRAZE:   if(hi==CLBRACE){f2=jtrazefrom; flag &=~(VJTFLGOK2);}
                else if(hi==CCUT){
                 j=i0(hv->fgh[1]);
                 if(CBOX==ID(hv->fgh[0])&&!j){f2=jtrazecut0; flag &=~(VJTFLGOK2);}
                 else if(boxatop(h)){  // h is <@g;.j   detect ;@:(<@(f/\);._2 _1 1 2
                  if((1LL<<(j+3))&0x36) { // fbits are 3 2 1 0 _1 _2 _3; is 1/2-cut?
                   A wf=hv->fgh[0]; V *wfv=FAV(wf); A hg=wfv->fgh[1]; V *hgv=FAV(hg);  // w is <@g;.k  find g
                   if((I)(((hgv->id^CBSLASH)-1)|((hgv->id^CBSDOT)-1))<0) {  // g is gf\ or gf\.
                    A hgf=hgv->fgh[0]; V *hgfv=FAV(hgf);  // find gf
                    if(hgfv->id==CSLASH){  // gf is gff/  .  We will analyze gff later
                     f1=jtrazecut1; f2=jtrazecut2; flag&=~(VJTFLGOK1|VJTFLGOK2);
                    }
                   }
                  }
                 }

                }
 }
 if(0<=m){
  v=4<=m?hv:fv; b=CFIT==v->id&&equ(zero,v->fgh[1]);
  switch(b?ID(v->fgh[0]):v->id){
   case CEQ:   f2=b?jtfolkcomp0:jtfolkcomp; flag|=0+8*m; flag &=~(VJTFLGOK1|VJTFLGOK2); break;
   case CNE:   f2=b?jtfolkcomp0:jtfolkcomp; flag|=1+8*m; flag &=~(VJTFLGOK1|VJTFLGOK2); break;
   case CLT:   f2=b?jtfolkcomp0:jtfolkcomp; flag|=2+8*m; flag &=~(VJTFLGOK1|VJTFLGOK2); break;
   case CLE:   f2=b?jtfolkcomp0:jtfolkcomp; flag|=3+8*m; flag &=~(VJTFLGOK1|VJTFLGOK2); break;
   case CGE:   f2=b?jtfolkcomp0:jtfolkcomp; flag|=4+8*m; flag &=~(VJTFLGOK1|VJTFLGOK2); break;
   case CGT:   f2=b?jtfolkcomp0:jtfolkcomp; flag|=5+8*m; flag &=~(VJTFLGOK1|VJTFLGOK2); break;
   case CEBAR: f2=b?jtfolkcomp0:jtfolkcomp; flag|=6+8*m; flag &=~(VJTFLGOK1|VJTFLGOK2); break;
   case CEPS:  f2=b?jtfolkcomp0:jtfolkcomp; flag|=7+8*m; flag &=~(VJTFLGOK1|VJTFLGOK2); break;
  }
 }

 // If this fork is not a special form, set the flags to indicate whether the f verb does not use an
 // argument.  In that case h can inplace the unused aegument.
 if(f1==jtfolk1 && f2==jtfolk2) flag |= atoplr(f);
 R fdef(flag2,CFORK,VERB, f1,f2, f,g,h, flag, RMAX,RMAX,RMAX);
}

static DF1(taa){TDECL;A t=df1(w,fs); ASSERT(!t||AT(t)&NOUN+VERB,EVSYNTAX); R df1(t,gs);}
static DF1(tvc){TDECL; R df2(fs,w,gs);}  /* also nc */
static DF1(tcv){TDECL; R df2(w,gs,fs);}  /* also cn */

// If the CS? loops, it will be noninplaceable.  If it falls through, we can inplace it.
CS1IP(static,jthook1, \
{PUSHZOMB; A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); A gx; RZ(gx=CALL1(g1,w,gs));  /* Cannot inplace the call to g */ \
/* inplace gx unless it is protected; inplace w (as left arg) if the caller allowed it*/ \
POPZOMB; RZ(z=(f2)(FAV(fs)->flag&VJTFLGOK2?( (J)(intptr_t)((I)jt|(JTINPLACEA*((I)jtinplace&JTINPLACEW)+(gx!=protw?JTINPLACEW:0))) ):jt,w,gx,fs));} \
,0111)
CS2IP(static,jthook2, \
{PUSHZOMB; A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); A gx; RZ(gx=(g1)(a!=w&&(FAV(gs)->flag&VJTFLGOK1)?(J)(intptr_t)((I)jtinplace&~JTINPLACEA):jt,w,gs));  /* Can inplace the call to g for the dyad unless same as a; it has w only */ \
/* inplace gx unless it is protected; inplace a if the caller allowed it*/ \
POPZOMB; RZ(z=(f2)(FAV(fs)->flag&VJTFLGOK2?( (J)(intptr_t)((I)jt|(((I)jtinplace&JTINPLACEA)+(gx!=protw?JTINPLACEW:0))) ):jt,a,gx,fs));} \
,0112)


static DF1(jthkiota){DECLFG;A a,e;I n;P*p;
 RZ(w);
 n=IC(w);
 if(SB01&AT(w)&&1==AR(w)){
  p=PAV(w); a=SPA(p,a); e=SPA(p,e); 
  R *BAV(e)||equ(mtv,a) ? repeat(w,IX(n)) : repeat(SPA(p,x),ravel(SPA(p,i)));
 }
 R B01&AT(w)&&1>=AR(w) ? ifb(n,BAV(w)) : repeat(w,IX(n));
}    /* special code for (# i.@#) */

static DF1(jthkodom){DECLFG;B b=0;I n,*v;
 RZ(w);
 if(INT&AT(w)&&1==AR(w)){n=AN(w); v=AV(w); DO(n, if(b=0>v[i])break;); if(!b)R odom(2L,n,v);}
 R CALL2(f2,w,CALL1(g1,w,gs),fs);
}    /* special code for (#: i.@(* /)) */

static DF2(jthkeps){
 RZ(a&&w);
 if(TYPESEQ(AT(a),AT(w))&&AT(a)&IS1BYTE&&1==AN(a)){A z;
  GATV(z,B01,1,AR(a),AS(a)); 
  *BAV(z)=1&&memchr(CAV(w),*CAV(a),AN(w)); 
  R z;
 }
 R eps(a,gah(1L,w));
}    /* special code for (e.,) */

static DF2(jthkfrom){R from(a,gah(1L,w));}     /* special code for ({ ,) */
static DF2(jthktake){R take(a,gah(1L,w));}     /* special code for ({.,) */
static DF2(jthkdrop){R drop(a,gah(1L,w));}     /* special code for (}.,) */

static DF1(jthkindexofmaxmin){D*du,*dv;I*iu,*iv,n,t,*wv,z=0;V*sv;
 RZ(w&&self);
 n=AN(w); t=AT(w);
 if(!(1==AR(w)&&t&INT+FL))R hook1(w,self);
 sv=FAV(self); wv=AV(w);
 if(n)switch((t&FL?4:0)+(CICO==ID(sv->fgh[0])?2:0)+(CMAX==ID(FAV(sv->fgh[1])->fgh[0]))){
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


F2(jthook){AF f1=0,f2=0;C c,d,e,id;I flag=VFLAGNONE;V*u,*v;
 RZ(a&&w);
 switch(BD(AT(a),AT(w))){
  default:            ASSERT(0,EVSYNTAX);
  case BD(VERB,VERB):
   // This is the (V V) case, producing a verb
   u=FAV(a); c=u->id; f1=jthook1; f2=jthook2;
   v=FAV(w); d=v->id; e=ID(v->fgh[0]);
   // Set flag to use: ASGSAFE if both operands are safe, and INPLACEOK to match f1,f2
   flag=((u->flag&v->flag)&VASGSAFE)+(VJTFLGOK1|VJTFLGOK2);  // start with in-place enabled, as befits hook1/hook2
   if(d==CCOMMA)switch(c){   // all of this except for $, could be handled by virtual blocks
    case CDOLLAR: f2=jtreshape; flag+=VIRS2; break;  // ($,) is inplace
    case CFROM:   f2=jthkfrom; flag &=~VJTFLGOK2;  break;
    case CTAKE:   f2=jthktake; flag &=~VJTFLGOK2;  break;
    case CDROP:   f2=jthkdrop; flag &=~VJTFLGOK2;  break;
    case CEPS:    f2=jthkeps; flag &=~VJTFLGOK2; break;
   }else        switch(c){
    case CSLDOT:  if(COMPOSE(d)&&e==CIOTA&&CPOUND==ID(v->fgh[1])&&CBOX==ID(u->fgh[0])){f1=jtgroup; flag &=~VJTFLGOK1;} break;
    case CPOUND:  if(COMPOSE(d)&&e==CIOTA&&CPOUND==ID(v->fgh[1])){f1=jthkiota; flag &=~VJTFLGOK1;} break;
    case CABASE:  if(COMPOSE(d)&&e==CIOTA&&CSLASH==ID(v->fgh[1])&&CSTAR==ID(FAV(v->fgh[1])->fgh[0])){f1=jthkodom; flag &=~VJTFLGOK1;} break;
    case CIOTA:   
    case CICO:    if(d==CSLASH&&(e==CMAX||e==CMIN)){f1=jthkindexofmaxmin; flag &=~VJTFLGOK1;} break;
    case CFROM:   if(d==CGRADE){f2=jtordstati; flag &=~VJTFLGOK2;} else if(d==CTILDE&&e==CGRADE){f2=jtordstat; flag &=~VJTFLGOK2;}
   }
   // Return the derived verb
   R fdef(0,CHOOK, VERB, f1,f2, a,w,0L, flag, RMAX,RMAX,RMAX);
  // All other cases produce an adverb
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
 R fdef(0,CADVF, ADV, f1,0L, a,w,0L, flag, 0L,0L,0L);
}
