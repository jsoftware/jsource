/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Forks                                                     */

#include "j.h"

#define TC1(t)          ((0x20034>>(((t)>>(ADVX-1))&(CONJ+ADV+VERB>>(ADVX-1))))&3)   // C x V A (N) -> 16 x 4 2 0 -> 2 x 3 1 0    10 xx xx xx xx xx 11 01 00   C A V N -> 2 1 3 0
#define BD(ft,gt)       (4*TC1(ft)+TC1(gt))
#define TDECL           V*sv=FAV(self);A fs=sv->fgh[0],gs=sv->fgh[1],hs=sv->fgh[2]


// handle fork, with support for in-place operations
#define FOLK1 {PUSHZOMB; ARGCHK1D(w) A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW));  \
/* If any result equals protw, it must not be inplaced: if original w is inplaceable, protw will not match anything */ \
/* the call to h is not inplaceable, but it may allow WILLOPEN and USESITEMCOUNT */ \
A hx; RZ(hx=(h1)((J)(intptr_t)(((I)jt) + (REPSGN(SGNIF(FAV(hs)->flag,VJTFLGOK1X)) & (FAV(gs)->flag2>>(VF2WILLOPEN2WX-VF2WILLOPEN1X)) & VF2WILLOPEN1+VF2USESITEMCOUNT1)),w,hs)); \
ARGCHK1D(hx) \
/* the call to f is inplaceable if the caller allowed inplacing, and f is inplaceable, and the hx is NOT the same as y.  Here only the LSB of jtinplace is used */ \
A fx; RZ(fx=(f1)((J)(intptr_t)(((I)jt) + (REPSGN(SGNIF(FAV(fs)->flag,VJTFLGOK1X)) & (((I)jtinplace&(I )(hx!=w)) + ((FAV(gs)->flag2>>(VF2WILLOPEN2AX-VF2WILLOPEN1X)) & VF2WILLOPEN1+VF2USESITEMCOUNT1)))),w,fs)); \
ARGCHK2D(fx,hx) \
/* The call to g is inplaceable if g allows it, UNLESS fx or hx is the same as disallowed y.  Pass in WILLOPEN from the input */ \
POPZOMB; RZ(z=(g2)((J)(intptr_t)((((I)jtinplace&(~(JTINPLACEA+JTINPLACEW)))|((I )(fx!=protw)*JTINPLACEA+(I )(hx!=protw)*JTINPLACEW))&(REPSGN(SGNIF(FAV(gs)->flag,VJTFLGOK2X))|~JTFLAGMSK)),fx,hx,gs));}

#define FOLK2 {PUSHZOMB; ARGCHK2D(a,w) A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); A prota = (A)(intptr_t)((I)a+((I)jtinplace&JTINPLACEA)); \
/* the call to h is not inplaceable, but it may allow WILLOPEN and USESITEMCOUNT.  Inplace h if f is x@], but not if a==w  Actually we turn off all flags here if a==w, for comp ease */ \
A hx; RZ(hx=(h2)((J)(intptr_t)(((I)jt) + ((-((FAV(hs)->flag>>VJTFLGOK2X)&(I )(a!=w))) & (((I)jtinplace&sv->flag&(VFATOPL|VFATOPR)) + ((FAV(gs)->flag2>>(VF2WILLOPEN2WX-VF2WILLOPEN1X)) & VF2WILLOPEN1+VF2USESITEMCOUNT1)))),a,w,hs)); \
ARGCHK1D(hx) \
/* If any result equals protw/prota, it must not be inplaced: if original w/a is inplaceable, protw/prota will not match anything */ \
/* the call to f is inplaceable if the caller allowed inplacing, and f is inplaceable; but only where hx is NOT the same as x or y.  Both flags in jtinplace are used */ \
A fx; RZ(fx=(f2)((J)(intptr_t)(((I)jt) + (REPSGN(SGNIF(FAV(fs)->flag,VJTFLGOK2X)) & (((I)jtinplace&(JTINPLACEA*(I )(hx!=a)+(I )(hx!=w))) + ((FAV(gs)->flag2>>(VF2WILLOPEN2AX-VF2WILLOPEN1X)) & VF2WILLOPEN1+VF2USESITEMCOUNT1)))),a,w,fs)); \
ARGCHK2D(fx,hx) \
/* The call to g is inplaceable if g allows it, UNLESS fx or hx is the same as disallowed y.  Pass in WILLOPEN from the input */ \
POPZOMB; RZ(z=(g2)((J)(intptr_t)((((I)jtinplace&(~(JTINPLACEA+JTINPLACEW)))|(((I )(fx!=protw)&(I )(fx!=prota))*JTINPLACEA+((I )(hx!=protw)&(I )(hx!=prota)*JTINPLACEW)))&(REPSGN(SGNIF(FAV(gs)->flag,VJTFLGOK2X))|~JTFLAGMSK)),fx,hx,gs));}

// similar for cap, but now we can inplace the call to h
#define CAP1 {PUSHZOMB; A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); \
A hx; RZ(hx=(h1)((J)(intptr_t)(((I)jtinplace&(~(JTWILLBEOPENED+JTCOUNTITEMS))) + (REPSGN(SGNIF(FAV(hs)->flag,VJTFLGOK1X)) & FAV(gs)->flag2 & JTWILLBEOPENED+JTCOUNTITEMS)),w,hs));  /* inplace g.  jtinplace is set for g */ \
/* inplace gx unless it is protected */ \
POPZOMB; \
jtinplace=(J)(intptr_t)(((I)jtinplace&~(JTINPLACEW))+((I )(hx!=protw)*JTINPLACEW));  \
jtinplace=FAV(gs)->flag&VJTFLGOK1?jtinplace:jt; \
RZ(z=(g1)(jtinplace,hx,gs));}

#define CAP2 {PUSHZOMB; A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); A prota = (A)(intptr_t)((I)a+((I)jtinplace&JTINPLACEA)); \
A hx; RZ(hx=(h2)((J)(intptr_t)(((I)jtinplace&(~(JTWILLBEOPENED+JTCOUNTITEMS))) + (REPSGN(SGNIF(FAV(hs)->flag,VJTFLGOK2X)) & FAV(gs)->flag2 & JTWILLBEOPENED+JTCOUNTITEMS)),a,w,hs));  /* inplace g */ \
/* inplace gx unless it is protected */ \
POPZOMB; jtinplace=(J)(intptr_t)(((I)jtinplace&~(JTINPLACEW))+(((I )(hx!=prota)&(I )(hx!=protw))*JTINPLACEW));  \
jtinplace=FAV(gs)->flag&VJTFLGOK1?jtinplace:jt; \
RZ(z=(g1)(jtinplace,hx,gs));}



DF1(jtcork1){F1PREFIP;DECLFGH;PROLOG(0026);A z;  CAP1; EPILOG(z);}
DF2(jtcork2){F2PREFIP;DECLFGH;PROLOG(0027);A z;  CAP2; EPILOG(z);}

static DF1(jtfolk1){F1PREFIP;DECLFGH;PROLOG(0028);A z; FOLK1; EPILOG(z);}
static DF2(jtfolk2){F2PREFIP;DECLFGH;PROLOG(0029);A z; FOLK2;
 EPILOG(z);}

// see if f is defined as [:, as a single name
static B jtcap(J jt,A x){V*v;L *l;
 if(v=VAV(x),CTILDE==v->id&&NAME&AT(v->fgh[0])&&(l=syrd(v->fgh[0],jt->locsyms))&&(x=l->val))v=VAV(x);  // don't go through chain of names, since it might loop (on u) and it's ugly to chase the chain
 R CCAP==v->id;
}


// nvv forks.  n must not be inplaced, since the fork may be reused.  hx can be inplaced unless protected by caller.
// This generally follows the logic for CAP, but with dyad g
static DF1(jtnvv1){F1PREFIP;DECLFGH;PROLOG(0032);
PUSHZOMB; A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW));
A hx; RZ(hx=(h1)((J)(intptr_t)(((I)jtinplace&(~(JTWILLBEOPENED+JTCOUNTITEMS))) + (REPSGN(SGNIF(FAV(hs)->flag,VJTFLGOK1X)) & (FAV(gs)->flag2>>(VF2WILLOPEN2WX-VF2WILLOPEN1X)) & JTWILLBEOPENED+JTCOUNTITEMS)),w,hs));  /* inplace g.  jtinplace is set for g */
/* inplace gx unless it is protected */
POPZOMB;
jtinplace=(J)(intptr_t)(((I)jtinplace&~(JTINPLACEA+JTINPLACEW))+((I )(hx!=protw)*JTINPLACEW)); 
jtinplace=FAV(gs)->flag&VJTFLGOK2?jtinplace:jt;
A z; RZ(z=(g2)(jtinplace,fs,hx,gs));
EPILOG(z);}
static DF2(jtnvv2){F1PREFIP;DECLFGH;PROLOG(0033);
PUSHZOMB; A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); A prota = (A)(intptr_t)((I)a+((I)jtinplace&JTINPLACEA));
A hx; RZ(hx=(h2)((J)(intptr_t)(((I)jtinplace&(~(JTWILLBEOPENED+JTCOUNTITEMS))) + (REPSGN(SGNIF(FAV(hs)->flag,VJTFLGOK2X)) & (FAV(gs)->flag2>>(VF2WILLOPEN2WX-VF2WILLOPEN1X)) & JTWILLBEOPENED+JTCOUNTITEMS)),a,w,hs));  /* inplace g */
/* inplace gx unless it is protected */
POPZOMB; jtinplace=(J)(intptr_t)(((I)jtinplace&~(JTINPLACEA+JTINPLACEW))+(((I )(hx!=prota)&(I )(hx!=protw))*JTINPLACEW));
jtinplace=FAV(gs)->flag&VJTFLGOK2?jtinplace:jt;
A z;RZ(z=(g2)(jtinplace,fs,hx,gs));
EPILOG(z);}

static DF2(jtfolkcomp){F2PREFIP;DECLFGH;PROLOG(0034);A z;AF f;
 ARGCHK2(a,w);
 f=atcompf(a,w,self);
 if(f){
  I postflags=jt->workareas.compsc.postflags;
  z=f(jt,a,w,self);
  if(z){if(postflags&2){z=num((IAV(z)[0]!=AN(AR(a)>=AR(w)?a:w))^(postflags&1));}}
 }else if(cap(fs))CAP2 else FOLK2;
 EPILOG(z);
}

static DF2(jtfolkcomp0){F2PREFIP;DECLFGH;PROLOG(0035);A z;AF f;
 ARGCHK2(a,w);
 PUSHCCT(1.0)
 f=atcompf(a,w,self);
 if(f){
  I postflags=jt->workareas.compsc.postflags;
  z=f(jt,a,w,self);
  if(z){if(postflags&2){z=num((IAV(z)[0]!=AN(AR(a)>=AR(w)?a:w))^(postflags&1));}}
 }else if(cap(fs))CAP2 else FOLK2;
 POPCCT  //  bug: if we RZd early we leave ct unpopped
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
  if(((AT(f)^B01)|AR(f)|BAV0(f)[0])==0&&BOTHEQ8(gi,hi,CEPS,CDOLLAR))f1=jtisempty;  // 0 e. $, accepting only boolean 0
  if(LIT&AT(f)&&1==AR(f)&&BOTHEQ8(gi,hi,CTILDE,CFORK)&&CFROM==ID(gv->fgh[0])){
   x=hv->fgh[0];
   if(LIT&AT(x)&&1==AR(x)&&CIOTA==ID(hv->fgh[1])&&CRIGHT==ID(hv->fgh[2])){f1=jtcharmapa;  flag &=~(VJTFLGOK1);}  // (N {~ N i, ])
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
 // Copy the open/raze status from v into u@v
  flag2 |= hv->flag2&(VF2WILLOPEN1|VF2WILLOPEN2W|VF2WILLOPEN2A|VF2USESITEMCOUNT1|VF2USESITEMCOUNT2W|VF2USESITEMCOUNT2A);

 }
 switch(fi){
  case CCAP:   if(gi==CBOX)flag2|=VF2BOXATOP1|VF2BOXATOP2|VF2ISCCAP; f1=jtcork1; f2=jtcork2;
   if(BOTHEQ8(gi,hi,CSLASH,CDOLLAR)&&FAV(gv->fgh[0])->id==CSTAR){f1=jtnatoms;}  // [: */ $
   if(gi==CPOUND){f1=hi==CCOMMA?jtnatoms:f1; f1=hi==CDOLLAR?jtrank:f1;}  // [: # ,   [: # $
               break; /* [: g h */
  case CSLASH: if(BOTHEQ8(gi,hi,CDIV,CPOUND)&&CPLUS==ID(fv->fgh[0])){f1=jtmean; flag|=VIRS1; flag &=~(VJTFLGOK1);} break;  /* +/%# */
  case CAMP:   /* x&i.     { y"_ */
  case CFORK:  /* (x i. ]) { y"_ */
   if(hi==CQQ&&(y=hv->fgh[0],LIT&AT(y)&&1==AR(y))&&equ(ainf,hv->fgh[1])&&
       (x=fv->fgh[0],LIT&AT(x)&&1==AR(x))&&CIOTA==ID(fv->fgh[1])&&
       (fi==CAMP||CRIGHT==ID(fv->fgh[2]))){f1=jtcharmapb; flag &=~(VJTFLGOK1);} break;
  case CAT:    /* <"1@[ { ] */
   if(BOTHEQ8(gi,hi,CLBRACE,CRIGHT)){                                   
    p=fv->fgh[0]; q=fv->fgh[1]; 
    if(CLEFT==ID(q)&&CQQ==ID(p)&&(v=VAV(p),x=v->fgh[0],CLT==ID(x)&&v->fgh[1]==num(1))){f2=jtsfrom; flag &=~(VJTFLGOK2);}
   }
   break;
 }
 switch(fi==CCAP?gi:hi){
  case CQUERY:  if((hi&~1)==CPOUND){f2=jtrollk; flag &=~(VJTFLGOK2);}  break;  // # $
  case CQRYDOT: if((hi&~1)==CPOUND){f2=jtrollkx; flag &=~(VJTFLGOK2);} break;  // # $
  case CICAP:   if(fi==CCAP){if(hi==CNE)f1=jtnubind; else if(FIT0(CNE,hv)){f1=jtnubind0; flag &=~(VJTFLGOK1);}}else if(hi==CEBAR){f2=jtifbebar; flag&=~VJTFLGOK2;} break;
  case CSLASH:  c=ID(gv->fgh[0])+1; m=-1;m=BETWEENC(c,CPLUS+1,CSTARDOT+1)?c:m;
                if(fi==CCAP&&FAV(gv->fgh[0])->flag&FAV(h)->flag&VISATOMIC2){f2=jtfslashatg; flag &=~(VJTFLGOK2);}
                break;
  case CFCONS:  if(hi==CFCONS){x=hv->fgh[2]; m=gi+BAV(x)[0]; m=(UI)(B01&AT(x))>((UI)(gi&~2)-CIOTA)?m:-1;} break;  // x-> constant; must be boolean, and i./i:
  case CRAZE:   if(hi==CCUT){
                 j=hv->localuse.lI;
                 if(hv->valencefns[1]==jtboxcut0){f2=jtrazecut0; flag &=~(VJTFLGOK2);}
                 else if(boxatop(h)){  // h is <@g;.j   detect ;@:(<@(f/\);._2 _1 1 2
                  if((((I)1)<<(j+3))&0x36) { // fbits are 3 2 1 0 _1 _2 _3; is 1/2-cut?
                   A wf=hv->fgh[0]; V *wfv=FAV(wf); A hg=wfv->fgh[1]; V *hgv=FAV(hg);  // w is <@g;.k  find g
                   if((I)(((hgv->id^CBSLASH)-1)|((hgv->id^CBSDOT)-1))<0) {  // g is gf\ or gf\.
                    A hgf=hgv->fgh[0]; V *hgfv=FAV(hgf);  // find gf
                    if(hgfv->id==CSLASH){  // gf is gff/  .  We will analyze gff later
                     f1=jtrazecut1; f2=jtrazecut2; flag&=~(VJTFLGOK1|VJTFLGOK2);
                    }
                   }
                  }
                 }

                } break;
 }

 // comparison combinations
 if(0<=m){
  // m has information about the comparison combiner.  See if the right is a comparison
  V *cv=(m&=7)>=4?hv:fv;  // cv point to comp in comp i. 0:  or [: +/ comp
  I d=cv->id;  // comparison op
  I e=d; e=d==CFIT&&cv->localuse.lD==1.0?FAV(cv->fgh[0])->id:e;  // comparison op, possibly from fit
  if(BETWEENC(e,CEQ,CEPS)){
   // valid comparison combination.  m is the combiner, e is the comparison
   f2=d==CFIT?jtfolkcomp0:jtfolkcomp;  // valid comparison type: switch to it
   flag+=(e-CEQ)+8*m; flag &=~(VJTFLGOK1|VJTFLGOK2);;  // set comp type & clear FLGOK1 & 2
  }
 }

 // If this fork is not a special form, set the flags to indicate whether the f verb does not use an
 // argument.  In that case h can inplace the unused argument.
 if(f1==jtfolk1 && f2==jtfolk2) flag |= atoplr(f);
 R fdef(flag2,CFORK,VERB, f1,f2, f,g,h, flag, RMAX,RMAX,RMAX);
}

// Handlers for to  handle w (aa), w (vc), w (cv)
static DF1(taa){TDECL; A z,t; df1(t,w,fs); ASSERT(!t||AT(t)&NOUN+VERB,EVSYNTAX); R df1(z,t,gs);}
static DF1(tvc){TDECL; A z; R df2(z,fs,w,gs);}  /* also nc */
static DF1(tcv){TDECL; A z; R df2(z,w,gs,fs);}  /* also cn */

CS1IPext(static,,jthook1, \
{PUSHZOMB; A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); \
A gx; RZ(gx=(g1)((J)(intptr_t)(((I)jt + ((FAV(fs)->flag2>>(VF2WILLOPEN2WX-VF2WILLOPEN1X)) & (VF2WILLOPEN1+VF2USESITEMCOUNT1) & REPSGN(SGNIF(FAV(gs)->flag,VJTFLGOK1X))))),w,gs));  /* cannot inplace g.  jtinplace is always set */ \
/* inplace gx unless it is protected */ \
POPZOMB; \
jtinplace=(J)(intptr_t)(((I)jtinplace&~(JTINPLACEA+JTINPLACEW)) + (JTINPLACEA*((I)jtinplace&JTINPLACEW)) + ((gx!=protw)*JTINPLACEW)); \
jtinplace=FAV(fs)->flag&VJTFLGOK2?jtinplace:jt; \
RZ(z=(f2)(jtinplace,w,gx,fs)); \
} \
, 0111)

// hook2cell is external
CS2IP(,static,jthook2, \
{PUSHZOMB; A protw = (A)(intptr_t)((I)w+((I)jtinplace&JTINPLACEW)); A prota = (A)(intptr_t)((I)a+((I)jtinplace&JTINPLACEA)); \
A gx; RZ(gx=(g1)((J)(intptr_t)((I)jt + ((((I)jtinplace&((a!=w)<<JTINPLACEWX)) + ((FAV(fs)->flag2>>(VF2WILLOPEN2WX-VF2WILLOPEN1X)) & JTWILLBEOPENED+JTCOUNTITEMS)) & REPSGN(SGNIF(FAV(gs)->flag,VJTFLGOK1X)))),w,gs));  /* inplace g unless a=w.  jtinplace is always set */ \
/* inplace gx unless it is protected */ \
POPZOMB; jtinplace=(J)(intptr_t)(((I)jtinplace&~(JTINPLACEW))+(((I )(gx!=prota)&(I )(gx!=protw))*JTINPLACEW)); \
jtinplace=FAV(fs)->flag&VJTFLGOK2?jtinplace:jt; \
RZ(z=(f2)(jtinplace,a,gx,fs)); \
} \
, 0112)

static DF1(jthkiota){DECLFG;A a,e;I n;P*p;
 ARGCHK1(w);
 SETIC(w,n);\
 if(SB01&AT(w)&&1==AR(w)){
  p=PAV(w); a=SPA(p,a); e=SPA(p,e); 
  R BAV(e)[0]||equ(mtv,a) ? repeat(w,IX(n)) : repeat(SPA(p,x),ravel(SPA(p,i)));
 }
 R B01&AT(w)&&1>=AR(w) ? ifb(n,BAV(w)) : repeat(w,IX(n));
}    /* special code for (# i.@#) */

static DF1(jthkodom){DECLFG;B b=0;I n,*v;
 ARGCHK1(w);
 if(INT&AT(w)&&1==AR(w)){n=AN(w); v=AV(w); DO(n, if(b=0>v[i])break;); if(!b)R odom(2L,n,v);}
 R CALL2(f2,w,CALL1(g1,w,gs),fs);
}    /* special code for (#: i.@(* /)) */

#define IDOTSEARCH(T,comp,compe)  {T *wv=T##AV(w); I optx0=-1; T opt0=wv[0]; I optx1=optx0; T opt1=wv[1]; wv+=2; \
  DO((n>>1)-1, if(wv[0] comp opt0){opt0=wv[0]; optx0=i;} if(wv[1] comp opt1){opt1=wv[1]; optx1=i;} wv+=2;) \
  z=((opt0 comp opt1) || (opt0==opt1&&optx0<=optx1))?2*optx0:2*optx1+1; opt0=opt0 compe opt1?opt0:opt1; z+=2; if(n&1&&wv[0] comp opt0)z=n-1; break;}

#define ICOSEARCH(T,comp,compe)  {T *wv=T##AV(w)+n-1; I optx0=(n>>1)-1; T opt0=wv[0]; I optx1=optx0; T opt1=wv[-1]; wv-=2; \
  DQ(optx0, if(wv[0] comp opt0){opt0=wv[0]; optx0=i;} if(wv[-1] comp opt1){opt1=wv[-1]; optx1=i;} wv-=2;) \
  z=((opt0 comp opt1) || (opt0==opt1&&optx0>=optx1))?2*optx0+1:2*optx1; opt0=opt0  compe opt1?opt0:opt1; z+=n&1; if(n&1&&wv[0] comp opt0)z=0; break;}

static DF1(jthkindexofmaxmin){I z=0;
 ARGCHK2(w,self);
 I n=AN(w);
 if(!(1==AR(w)&&AT(w)&INT+FL))R hook1(w,self);
 if(n>1){
  switch((AT(w)&INT)+(CICO==ID(FAV(self)->fgh[0])?2:0)+(CMAX==ID(FAV(FAV(self)->fgh[1])->fgh[0]))){
  case 0: IDOTSEARCH(D,<,<=)
  case 1: IDOTSEARCH(D,>,>=)
  case 2: ICOSEARCH(D,<,<=)
  case 3: ICOSEARCH(D,>,>=)
  case 4: IDOTSEARCH(I,<,<=)
  case 5: IDOTSEARCH(I,>,>=)
  case 6: ICOSEARCH(I,<,<=)
  case 7: ICOSEARCH(I,>,>=)
  }
 }
 R sc(z);
}    /* special code for (i.<./) (i.>./) (i:<./) (i:>./) */

// (compare L.) dyadic
static DF2(jthklvl2){
 F2RANK(0,RMAX,jthklvl2,self);
 I comparand; RE(comparand=i0(a));  // get value to compare against
 RETF(num(((VAV(self)->flag>>VFHKLVLGTX)&1)^levelle(w,comparand-(VAV(self)->flag&VFHKLVLDEC))));  // decrement for < or >:; complement for > >:
}

F2(jthook){AF f1=0,f2=0;C c,d,e,id;I flag=VFLAGNONE,linktype=0;V*u,*v;
 ARGCHK2(a,w);
 switch(BD(AT(a),AT(w))){
  default:            ASSERT(0,EVSYNTAX);
  case BD(VERB,VERB):
   // This is the (V V) case, producing a verb
   u=FAV(a); c=u->id; f1=jthook1cell; f2=jthook2cell;
   v=FAV(w); d=v->id; e=ID(v->fgh[0]);
   // Set flag to use: ASGSAFE if both operands are safe; and FLGOK init to OK as for hook, but change as needed to match f1,f2
   flag=((u->flag&v->flag)&VASGSAFE)+(VJTFLGOK1|VJTFLGOK2);  // start with in-place enabled, as befits hook1/hook2
   if(d==CCOMMA)switch(c){   // all of this except for ($,) is handled by virtual blocks
    case CDOLLAR: f2=jtreshape; flag+=VIRS2; break;  // ($,) is inplace
   }else if(d==CBOX){
    if(c==CRAZE){f2=jtlink; linktype=ACINPLACE;  // (;<)
    }else if(c==CCOMMA){f2=jtlink; linktype=ACINPLACE+1;  // (,<)
    }
   }else if(d==CLDOT){   // (compare L.)
    I comptype=0; comptype=c==CLT?VFHKLVLGT:comptype; comptype=c==CGT?VFHKLVLDEC:comptype; comptype=c==CLE?VFHKLVLDEC+VFHKLVLGT:comptype; comptype=c==CGE?4:comptype;
    if(comptype){flag|=comptype; f2=jthklvl2; flag &=~VJTFLGOK2;}
   }else        switch(c){
    case CSLDOT:  if(COMPOSE(d)&&e==CIOTA&&CPOUND==ID(v->fgh[1])){  // (f/. i.@#)  or @:
                   if(CBOX==ID(u->fgh[0])){f1=jtkeybox; flag &=~VJTFLGOK1;} // (</. i.@#)
                   else if(u->valencefns[1]==jtkeyheadtally){f1=jtkeyheadtally; flag &=~VJTFLGOK1;} // ((#,{.)/. i.@#) or  (({.,#)/. i.@#)
                  } break;
    case CPOUND:  if(COMPOSE(d)&&e==CIOTA&&CPOUND==ID(v->fgh[1])){f1=jthkiota; flag &=~VJTFLGOK1;} break;  // (# i.@#))
    case CABASE:  if(COMPOSE(d)&&e==CIOTA&&CSLASH==ID(v->fgh[1])&&CSTAR==ID(FAV(v->fgh[1])->fgh[0])){f1=jthkodom; flag &=~VJTFLGOK1;} break;  // (#: i.@(*/))
    case CIOTA:   
    case CICO:    if(BOTHEQ8(d,(e&~1),CSLASH,CMIN)){f1=jthkindexofmaxmin; flag &=~VJTFLGOK1;} break;  // >./ <./
    case CFROM:   if(d==CGRADE){f2=jtordstati; flag &=~VJTFLGOK2;} else if(d==CTILDE&&e==CGRADE){f2=jtordstat; flag &=~VJTFLGOK2;}
   }
   // Return the derived verb
   A z;RZ(z=fdef(0,CHOOK, VERB, f1,f2, a,w,0L, flag, RMAX,RMAX,RMAX));
   FAV(z)->localuse.lclr[0]=linktype; R z;  // if it's a form of ;, install the form
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
