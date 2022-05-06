/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Fix                                                            */

#include "j.h"


F1(jtunname){A x;V*v;
 ARGCHK1(w); 
 v=VAV(w);
 if(CTILDE==v->id&&!jt->glock&&!(VLOCK&v->flag)){x=v->fgh[0]; if(NAME&AT(x))R symbrd(x);}
 RETF(w);
}

static B jtselfq(J jt,A w){A hs,*u;V*v;
 ARGCHK1(w);
 if(AT(w)&NOUN+NAME)R 0;
 v=FAV(w);
 if(unlikely(BETWEENC(v->id,CATDOT,CSELF))){
  if(v->id==CSELF)R 1;  // found!
  if(hs=v->fgh[2]){u=AAV(hs); DO(AN(hs), if(selfq(u[i]))R 1;);}  // @. or `: - traverse the gerund
 }else{
  DO(3, if(v->fgh[i]&&selfq(v->fgh[i]))R 1;)  // anything else - traverse
 }
 R 0;
}    /* 1 iff w contains $: */

// See if there are references to implicit locatives.  Return 1 if so, 0 if not
B jthasimploc(J jt,A w){A hs,*u;V*v;
 ARGCHK1(w);
 if(AT(w)&NOUN+NAME)R 0;
 v=FAV(w);
 switch(v->id){
  default:     
   DO(3, if(v->fgh[i]&&hasimploc(v->fgh[i]))R 1;) break;
  case CUDOT: case CVDOT:
   R 1;  // these are always implicit locatives
  case CTILDE: ;
   A thisname=v->fgh[0];  // the A block for the name of the function (holding an NM) - unless it's a pseudo-name
   if(!thisname)R 0;  // no name
   if(AT(thisname)&VERB)R hasimploc(thisname);  // if v~, go look at v
   if(AT(thisname)&NOUN)R 0;   // if noun~, leave as is
   NM* thisnameinfo=NAV(thisname);  // the NM block for the current name
   if(!(thisnameinfo->flag&NMIMPLOC))R 0; // not NMDOT
   if(!(probelocal(thisname,jt->locsyms)))R 0;  // assigned value does not exist
   R 1;
  case CATDOT:
  case CGRCO:
   if(hs=v->fgh[2]){u=AAV(hs); DO(AN(hs), if(hasimploc(u[i]))R 1;);}
   R 0;
 }
 R 0;
}

static A jtfixa(J,A,A);
EVERYFS(arofixaself,jtaro,jtfixa,0,VFLAGNONE)  // create A block to be used in every to call jtfixa (dyad) or jtaro (monad)

// fix the verb w (could be a name if name+noun, or during debug).
// a is flags to handle lower recursions:
//  0 to begin with
//  1, 2, or 3 to indicate that a piece of the original input has been processed, and any $: found in a name must
//   be replaced by an explicit equivalent with the indicated valence(s)
// Higher flag bits restrict the search:
// FIXALOCSONLY set if we will replace only implicit locatives.  We don't go down a branch that doesn't contain one
// FIXALOCSONLYLOWEST set if we replace only lowest-level locatives (suitable for function return).  We stop in a branch when we hit a locative reference
// FIXASTOPATINV set if we halt at a defined oberse
// a has to be an A type because it goes into every2.  It is always an I type with rank 0 so it can go into every, but it has multiple items.
//   It may be virtual in callbacks from every2 so must not use IAV0
// IAV0(aa)[1] points to the recursion name-list and must be passed to all recursion levels
static A jtfixa(J jt,A a,A w){A f,g,h,wf,x,y,z=w;V*v;fauxblock(fauxself); A aa; fauxINT(aa,fauxself,2,0); IAV0(aa)[1]=IAV(a)[1];  // place to build recursion parm - make the AK field right, and pass the AM field along
#define REFIXA(a,x) (IAV0(aa)[0]=(aif|(a)), fixa((A)aa,(x)))
 ARGCHK1(w);
 I ai=IAV(a)[0];  // value of a
 I aif=ai&(FIXALOCSONLY|FIXALOCSONLYLOWEST|FIXASTOPATINV); // extract control flags
 ai^=aif;   // now ai = state without flags
 // If we are only interested in replacing locatives, and there aren't any, exit fast
 if(aif&FIXALOCSONLY&&!hasimploc(w))R w;  // nothing to fix
 if(NAME&AT(w)){R sfn(0,w);}  // only way a name gets here is by ".@noun which turns into ".@(name+noun) for execution.  Also in debug, but that's discarded
 if(NOUN&AT(w)||VFIX&VAV(w)->flag)R w;
 v=VAV(w); f=v->fgh[0]; g=v->fgh[1]; h=v->fgh[2]; wf=ds(v->id); I na=ai==0?3:ai;  // for levels other than the top, use na to cuase replacement of $:
 if(v->id==CFORK&&h==0){h=g; g=f; f=ds(CCAP);}  // reconstitute capped fork
 if(!(((I)f|(I)g)||((v->id&-2)==CUDOT)))R w;  // combinations always have f or g; and u./v. must be replaced even though it doesn't
 switch(v->id){  // we know that modifiers have been executed to produce verb/nouns
 case CSLASH: 
  R df1(z,REFIXA(2,f),wf);
 case CSLDOT: case CBSLASH: case CBSDOT:
  R df1(z,REFIXA(1,f),wf);
 case CTDOT:
  f=REFIXA(0,f); g=REFIXA(na,g); R df2(z,f,g,wf);  // recur and rebuild.  t. starts a new recursion, so don't replace $: in it
 case CATCO:
  if(FAV(g)->id==CTDOT)R REFIXA(na,g);   // t. is internally <@:t. .  Remove the <@; and continue.
// otherwise fall through to...
 case CAT: case CCUT:
  f=REFIXA(1,f); g=REFIXA(na,g); R df2(z,f,g,wf);  // rerun the compound after fixing the args
 case CAMP: case CAMPCO: case CUNDER: case CUNDCO:
  f=REFIXA(na,f); g=REFIXA(1,g); R df2(z,f,g,wf);
 case CCOLON:
  // n : n had VFIX set & never gets here
  if(v->flag&VXOPR){
   // operator: fix the operands and rebuild.  If the operator is a pseudo-name, we have to fish the actual operator block out of h
   if(!f){v=VAV(h); f=v->fgh[0]; g=v->fgh[1]; h=v->fgh[2]; wf=ds(v->id);}
   f=REFIXA(0,f); h=REFIXA(0,h); R xop2(f,h?h:g,g);  // xop2 is bivalent 
  }
  else{f=REFIXA(1,f); g=REFIXA(2,g); R df2(z,f,g,wf);}  // v : v, similarly
 case CADVF:
  f=REFIXA(3,f); g=REFIXA(3,g); if(h)h=REFIXA(3,h); else h=mark; R hook(f,g,h);
 case CHOOK:
  f=REFIXA(2,f); g=REFIXA(1,g); R hook(f,g,mark);
 case CFORK:
  f=REFIXA(na,f); g=REFIXA(ID(f)==CCAP?1:2,g); h=REFIXA(na,h); R folk(f,g,h);  // f first in case it's [:
 case CATDOT: case CGRCO:
  IAV(aa)[0]=(aif|na);
  RZ(f=every(every2(aa,h,(A)&arofixaself),(A)&arofixaself)); // full A block required for call
  RZ(g=REFIXA(na,g));
  R df2(z,f,g,wf);
 case CIBEAM:
  if(f)RZ(f=REFIXA(na,f));
  if(g)RZ(g=REFIXA(na,g));
  R f&&g ? (VDDOP&v->flag?df2(z,f,g,df2(x,head(h),tail(h),wf)):df2(z,f,g,wf)) : 
           (VDDOP&v->flag?df1(z,f,  df2(x,head(h),tail(h),wf)):df1(z,f,  wf)) ;
 case CUDOT:
  R REFIXA(ai,JT(jt,implocref)[0]);  // u. is equivalent to 'u.'~ for fix purposes
 case CVDOT:
  R REFIXA(ai,JT(jt,implocref)[1]);
 case CTILDE:
  if(f&&NAME&AT(f)){
   RZ(y=sfn(0,f));
   if(all1(eps(box(y),(A)IAV0(aa)[1])))R w;  // break out of loop if recursive name lookup
   ASSERT(AN((A)IAV0(aa)[1])<248,EVLIMIT);  // error if too many names in expansion
   // recursion check finished.  Now replace the name with its value
   if(x=symbrdlock(f)){   // locked returns a ref to the same name
    // if this is an implicit locative, we have to switch the environment before we recur on the name for subsequent lookups
    // The value we get from the lookup must be interpreted in the environment of the higher level
    A savloc=jt->locsyms;  // initial locales
    A thisname=v->fgh[0];// the A block for the name of the function (holding an NM) - unless it's a pseudo-name
    if(thisname){ // name given
     NM* thisnameinfo=NAV(thisname);  // the NM block for the current name
     if(thisnameinfo->flag&NMIMPLOC){     //  implicit locative
      if((probelocal(thisname,jt->locsyms))){  // name is defined
       // If our ONLY mission is to replace implicit locatives, we are finished after replacing this locative IF
       // (1) we want to replace only first-level locatives; (2) there are no more locatives in this branch after the replacement
       if(aif&FIXALOCSONLYLOWEST)R x;  // return looked-up value once we hit one
       // If we have to continue after the replacement, we must do so in the environment of the implicit locative.
       SYMRESTOREFROMLOCAL((A)AM(jt->locsyms));
       // NO FAILURES ALLOWED FROM HERE TO RESTORE
      }
     }
    }
    // Before we recur on the contents of the name, add the name we are currently looking up to the list of encountered names
    // so that we abort if we hit a loop.  BUT if the current name is uv defined in a higher level, it can't be part of
    // a loop (since we are advancing the symbol pointer) and the name, which is just 'u', might well come up again; so we don't
    // add the name to the table in that case.  NOTE bug: an indirect locative a__b, if it appeared twice, would be detected as a loop even
    // if it evaluated to different locales
    I initn=AN((A)IAV0(aa)[1]);  // save name depth coming in
    if(savloc==jt->locsyms){AAV1((A)IAV0(aa)[1])[AN((A)IAV0(aa)[1])]=rifvs(y); AN((A)IAV0(aa)[1])++; AS((A)IAV0(aa)[1])[0]++;} // add name-string to list of visited names for recursion check
    if(z=REFIXA(na,x)){
     if(ai!=0&&selfq(x))z=fixrecursive(sc(ai),z);  // if a lower name contains $:, replace it with explicit equivalent
    }
    SYMRESTOREFROMLOCAL(savloc);  // make sure we restore current symbols
    AN((A)IAV0(aa)[1])=AS((A)IAV0(aa)[1])[0]=initn;   // restore name count
    RZ(z);
   }
   RE(z);
   ASSERT(PARTOFSPEECHEQ(AT(w),AT(z)),EVDOMAIN);  // if there was a change of part-of-speech during the fix, that's a pun, don't allow it
   R z;
  }else R df1(z,REFIXA(2,f),wf);
// bug ^: and m} should process gerund args
 case COBVERSE:
  if(aif&FIXASTOPATINV)R w;  // stop at obverse if told to
  // otherwise fall through to normal processing
 default:
  if(f)RZ(f=REFIXA(na,f));
  if(g)RZ(g=REFIXA(na,g));
  R f&&g?df2(z,f,g,wf):f?df1(z,f,wf):w;
 }
}   /* 0=a if fix names; 1=a if fix names only if does not contain $: */

// On internal calls, self is an integer whose value contains flags.  Otherwise zeroionei is used
DF1(jtfix){F1PREFIP;PROLOG(0005);A z;
 ARGCHK1(w);
 if(LIT&AT(w)){ASSERT(1>=AR(w),EVRANK); RZ(w=nfs(AN(w),CAV(w)));}
 // only verbs/noun can get in through the parser, but internally we also vet adv/conj
 ASSERT(AT(w)&NAME+VERB+ADV+CONJ,EVDOMAIN);
 STACKCHKOFL  // make sure we can't recur to a name by removing the name
 self=AT(self)&NOUN?self:zeroionei(0);  // default to 0 if noun not given
 // To avoid infinite recursion we keep an array of names that we have looked up.  We create that array here, initialized to empty.  To pass it into fixa, we create
 // a faux INT block to hold the value, and use AM in that block to point to the list of names.  The fauxblock has rank 0 but 2 items
 A namelist; GAT0(namelist,BOX,248,1); AS(namelist)[0]=AN(namelist)=0;  // allocate 248 slots with rank 1, but initialize to empty
 fauxblock(fauxself); A augself; fauxINT(augself,fauxself,2,0); IAV0(augself)[0]=IAV(self)[0]; IAV0(augself)[1]=(I)namelist;  // transfer value to writable block; install empty name array
 RZ(z=fixa(augself,AT(w)&VERB+ADV+CONJ?w:symbrdlock(w)));  // name comes from string a
 // Once a node has been fixed, it doesn't need to be looked at ever again.  This applies even if the node itself carries a name.  To indicate this
 // we set VFIX.  We only do so if the node has descendants (or a name). We can do this only if we are sure the entire tree was traversed, i. e. we were not just looking for implicit locatives or inverses.
 if(!(IAV(self)[0]&(FIXALOCSONLY|FIXALOCSONLYLOWEST|FIXASTOPATINV))&&AT(z)&VERB+ADV+CONJ){V*v=FAV(z); if(v->fgh[0]){v->flag|=VFIX;}}  // f is clear for anything in the pst
 EPILOG(z);
}
