/* Copyright (c) 1990-2026, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Fix                                                            */

#include "j.h"

// if w is a reference (name~), replace it with its value, recursively
A jtunname(J jt,A w,I recurct){A x;V*v;
 ARGCHK1(w);
 if(!(AT(w)&FUNC))RETF(w);   // non-FUNC is an invalid pointer
 if(recurct>100)RETF(w);  // avoid infinite recursion
 v=FAV(w);
 if(CTILDE==v->id&&!jt->glock&&!(VLOCK&v->flag)){x=v->fgh[0]; if(NAME&AT(x)){A nmv; RZ(nmv=symbrd(x)); if(unlikely(AFLAG(nmv)&AFRO))R sfn(0,x); R jtunname(jt,nmv,recurct+1);}}  // if name~ unlocked, keep cocurrent as string, otherwise contents
 RETF(w);
}
// See if there are references to implicit locatives.  Return 1 if so, 0 if not
B jthasimploc(J jt,A w){A hs,*u;V*v;
 ARGCHK1(w);
 if(AT(w)&NOUN+NAME+SYMB)R 0;
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
  if(!(thisnameinfo->flag&NMIMPLOC))R 0; // not u./v.
   if(!probex(NAV(thisname)->m,NAV(thisname)->s,SYMORIGIN,NAV(thisname)->hash,jt->locsyms))R 0;  // assigned value does not exist
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
// FIXASTOPATINV set if we halt at a defined obverse
// a has to be an A type because it goes into every2.  It is always an I type with rank 0 and AN=1 so it can go into every, but it has multiple items.
//   It may be virtual in callbacks from every2 so must not use IAV0
// IAV0(aa)[1-2] are len/addr of the recursion name-list and must be passed to all recursion levels
static A jtfixa(J jtfg,A a,A w){F12JT;A z;
#define REFIXA(a,x) (IAV0(aa)[0]=(aif|(a)), fixa((A)aa,(x)))
#define R255(x) if(unlikely((I)(x)&-256)==0)R x;
 ARGCHK1(w);
 // If we are only interested in replacing locatives, and there aren't any, exit fast
// obsolete  if(NOUN&AT(w)||VNONAME+VNOSELF&FAV(w)->flag)R w;  // if value already fixed (or m : n which gets VNONAME+VNOSELF set), keep it
// obsolete  if(unlikely((NAME&AT(w))!=0)){R sfn(0,w);}
 if(NOUN&AT(w)){R w;}  // return noun value; only way a name gets here is by ".@noun which turns into ".@(name+noun) for execution.  Also in debug, but that's discarded
 if(FAV(w)->flag&VNONAME)R w;  // if value already fixed (or m : n which gets VNONAME+VNOSELF set), keep it.  This prunes all branches with no names, since VNONAME+VNOSELF is propagated from the bottom up during tree creation
 // continuing, there is a name in the branch and we have to plod through the tree to find it
 fauxblock(fauxself); A aa; fauxINT(aa,fauxself,3,0); AN(aa)=1; IAV0(aa)[1]=IAV(a)[1]; IAV0(aa)[2]=IAV(a)[2];  // place to build recursion parm - make the AK field right, and pass the AM field along
 A f=FAV(w)->fgh[0]; A g=FAV(w)->fgh[1]; A h=FAV(w)->fgh[2]; I id=FAV(w)->id;   // fetch fgh of compound, and the type
// obsolete  if(!(((I)f|(I)g)||unlikely((id&-2)==CUDOT)))R w;  // combinations always have f or g; and u./v. must be replaced even though it doesn't
// obsolete  if(unlikely(((id^CFORK)|(I)h)==0)){h=g; g=f; f=ds(CCAP);}  // reconstitute capped fork, which has h=0
 I ai=IAV(a)[0]; I aif=ai&~3; //get value of control input a; extract control flags
 ai^=aif; I na=ai==0?3:ai;  // now ai = state without flags; for levels other than the top, use na to cause replacement of $:
 if(unlikely(aif&FIXALOCSONLY)&&!hasimploc(w))R w;  // if looking for implicit locatives, and there aren't any, nothing to fix
 A wf=ds(id);   // fetch self for w
 A fo=f, go=g, ho=h;  // remember the constituents before they were fixed
 switch(id){  // we know that modifiers have been executed to produce verb/nouns
 // we reexecute the modifiers to use the new values.  But if the constituents were not changed by REFIXA, return the original w without reexecuting
 case CSLASH: 
  f=REFIXA(2,f); R255(f) if(fo==f)R w; R df1(z,f,wf);
 case CSLDOT: case CSLDOTDOT: case CBSLASH: case CBSDOT:
  f=REFIXA(1,f); R255(f) if(fo==f)R w; R df1(z,f,wf);
 case CTDOT:
  f=REFIXA(0,f); R255(f); g=REFIXA(na,g); R255(g); if((((I)fo^(I)f)|((I)go^(I)g))==0)R w; R df2(z,f,g,wf);  // recur and rebuild.  t. starts a new recursion, so don't replace $: in it
 case CATCO:
  if(FAV(g)->id==CTDOT)R REFIXA(na,g);   // t. is internally <@:t. .  Remove the <@: and continue.
// otherwise fall through to...
 case CAT: case CCUT:
  f=REFIXA(1,f); R255(f); g=REFIXA(na,g); R255(g); if((((I)fo^(I)f)|((I)go^(I)g))==0)R w; R df2(z,f,g,wf);  // rerun the compound after fixing the args
 case CAMP: case CAMPCO: case CUNDER: case CUNDCO:
  f=REFIXA(na,f); R255(f) g=REFIXA(1,g); R255(g) if((((I)fo^(I)f)|((I)go^(I)g))==0)R w; R df2(z,f,g,wf);
 case CCOLONE:  // Original m : n had VNONAME+VNOSELF set & never gets here.  This is (1) a nameref for an explicit modifier-plus-args, flagged as VXOP; (2) a namerefop for debug, flagged as PSEUDONAME+VXOPCALL+inherited flags;
                // a namerefop for a modifier locative, which looks like a debug namerefop but has the locative in g.
  if(unlikely(FAV(w)->flag2&VF2PSEUDONAME)){R REFIXA(0,h);}  // If the operator is a pseudo-name, we have to fish the actual operator block out of h
  f=REFIXA(0,f); R255(f) h=REFIXA(0,h); R255(h) if((((I)fo^(I)f)|((I)ho^(I)h))==0)R w; R xop2(f,h?h:g,g);  // here for nameref: xop2 is bivalent; rebuild operator with original self and fixed f/h
 case CCOLON:
  f=REFIXA(1,f); R255(f) g=REFIXA(2,g); R255(g) if((((I)fo^(I)f)|((I)go^(I)g)|((I)ho^(I)h))==0)R w; R df2(z,f,g,wf);  // v : v, similarly
 case CADVF:
  f=REFIXA(3,f); R255(f) g=REFIXA(3,g); R255(g) if(h){h=REFIXA(3,h); R255(h)} if((((I)fo^(I)f)|((I)go^(I)g))==0)R w; R hook(f,g,h);
 case CHOOK:
  f=REFIXA(2,f); R255(f) g=REFIXA(1,g); R255(g) if((((I)fo^(I)f)|((I)go^(I)g))==0)R w; R hook(f,g,0);
 case CFORK:
  if(h==0){ho=h=g; go=g=f; fo=f=ds(CCAP);}  // reconstitute capped fork, which has h=0
  f=REFIXA(na,f); R255(f) g=REFIXA(ID(f)==CCAP?1:2,g); R255(g) h=REFIXA(na,h); R255(h) if((((I)fo^(I)f)|((I)go^(I)g)|((I)ho^(I)h))==0)R w;R folk(f,g,h);  // f first in case it's [:
 case CATDOT:
  // we fix each gerund; for each component that changed, we calculate the AR of the corresponding f
// obsolete   IAV(aa)[0]=(aif|na); RZ(f=every(every2(aa,h,(A)&arofixaself),(A)&arofixaself)); // full A block required for call
  IAV0(aa)[0]=(aif|na); A h1; RZ(h1=every2(aa,h,(A)&arofixaself)); A *h1v=AAV(h1), *hv=AAV(h); // fix each component of gerund h, point to boxes
  g=REFIXA(na,g); R255(g); RZ(f=ca(f)); A *fv=AAV(f);  // fix g, and make a nonrecursive copy of f where we will record changes to h
  I flag=VNOLOCCHG+VNONAME+VNOSELF&FAV(g)->flag;  // we will collect combined NOLOCCHG/VNONAME+VNOSELF over g/h
  DO(AN(h), flag&=FAV(h1v[i])->flag; if(h1v[i]!=hv[i]){A t;RZ(t=aro(h1v[i])) INCORPNV(t) fv[i]=t;})  // update AR in f of any changed component.
// obsolete  R fdef(0,CATDOT,VERB, jtcasei12,jtcasei12, a,w,avb, flag+((VGERL)|(FAV(ds(CATDOT))->flag&~VNONAME+VNOSELF)), RMAX, RMAX, RMAX);
// obsolete   ra00(f,BOX);  // make f recursive, incrementing its descendants
  fdef(0,CATDOT,VERB, FAV(w)->valencefns[0],FAV(w)->valencefns[1], f,g,h1, flag+VGERL, RMAX, RMAX, RMAX);   // create new verb, setting flags
// obsolete   R df2(z,f,g,wf);
 case CGRCO:
  // we fix each gerund, and then convert it to AR and back.  This is needless potentially expensive conversions.  We should
  // fix each gerund, but AR only the ones that changed so that we keep f matching h; and use the fixed ones without un-AR scaf
  IAV0(aa)[0]=(aif|na);
  RZ(f=every(every2(aa,h,(A)&arofixaself),(A)&arofixaself)); // full A block required for call
  g=REFIXA(na,g); R255(g);
  R df2(z,f,g,wf);
 case CIBEAM: R w;  // m, n carried in localuse
 case CUDOT:
  R REFIXA(ai,JT(jt,implocref)[0]);  // u. is equivalent to 'u.'~ for fix purposes
 case CVDOT:
  R REFIXA(ai,JT(jt,implocref)[1]);
 case CTILDE:
  if(f&&NAME&AT(f)){  // f is the name in name~
// obsolete    I initn=AN((A)IAV0(aa)[1]);  // save name depth coming in
   I initn=IAV0(aa)[1];  // save name depth coming in
// obsolete    RZ(y=sfn(0,f));
// obsolete    if(all1(eps(box(y),(A)IAV0(aa)[1])))R w;  // break out of loop if recursive name lookup
   if(unlikely(initn>125)){
    // the number of names processed has gotten large: check for name cycle.  If we find a cycle we return the start of the cycle+1 as the result: always < 256.  If no cycle, it's just too many names
    // this is tricky: the FIRST reference in a cycle must return a reference to itself.  We do it this way to avoid all traversals except when there is a cycle
// obsolete     A *av=AAV1((A)IAV0(aa)[1]);  // pointer to list of names encountered so far
    A *av=(A*)IAV0(aa)[2];  // pointer to list of names encountered so far
    I cyc1;  // start of cycle including last element
    C *cycs=NAV(av[initn-1])->s; I cycn=NAV(av[initn-1])->n; UI4 cych=NAV(av[initn-1])->hash;  // string and hash of the name we are checking for cycle
    DO(initn-2, NM *nm=NAV(av[i]); if(nm->hash==cych && cycn==nm->n && 0==memcmp(cycs,nm->s,cycn)){cyc1=i; goto cycfound;})
    ASSERT(initn<125,EVLIMIT);  // error if too many names in expansion
    ASSERT(0,EVLIMIT)  // no cycle found: too many names, abort
cycfound:;  // cycle found, running from cyci to cycn; now back it down to find its starting position
    I cycl=initn-1-cyc1;  // end-start of cycle
    for(--cyc1;cyc1>=0;--cyc1){NM *nm1=NAV(av[cyc1]),  *nmn=NAV(av[cyc1+cycl]); if(!(nm1->hash==nmn->hash && nm1->n==nmn->n && 0==memcmp(nm1->s,nmn->s,nm1->n)))break;}   // back up to before start of cycle
    ++cyc1;  // advance to start of cycle
    R (A)(cyc1+1);  // return start of cycle+1, which signals abort
   }
   // recursion check finished.  Now replace the name with its value
   A x; RZ(x=symbrdlock(f))   // current value of name; locked name returns a ref to the same name; not found returns error.  If found, value has been ra'd and tpushed.  Error if undefname
   // since the name is supposed to be executable, we have to guard against a type pun on the name
   ASSERT(PARTOFSPEECHEQACV(AT(w),AT(x)),EVTYPECHG);   // make sure its part of speech has not changed since the name was parsed
   if(unlikely(AFLAG(x)&AFRO))R w;  // If name has readonly value (like cocurrent), leave it as a reference
   // if this is an implicit locative, we have to switch the environment before we recur on the name for subsequent lookups
   // The value we get from the lookup must be interpreted in the environment of the higher level
   A savloc=jt->locsyms, savglob=jt->global;  // initial locales
// obsolete     A thisname=v->fgh[0];// the A block for the name of the function (holding an NM) - unless it's a pseudo-name
// obsolete if(thisname!=f)SEGFAULT;  // scaf
// obsolete     if(thisname){ // name given
// obsolete      NM* thisnameinfo=NAV(thisname);  // the NM block for the current name
   if(unlikely(NAV(f)->flag&NMIMPLOC)){     //  implicit locative
// obsolete       if(probex(NAV(thisname)->m,NAV(thisname)->s,SYMORIGIN,NAV(thisname)->hash,jt->locsyms)){  // name is defined
    // If our ONLY mission is to replace implicit locatives, we are finished after replacing this locative IF
    // (1) we want to replace only first-level locatives; (2) there are no more locatives in this branch after the replacement
    if(aif&FIXALOCSONLYLOWEST)R x;  // return looked-up value once we hit one
    // If we have to continue after the replacement, we must do so in the environment of the implicit locative.
    SYMSWITCHTOLOCAL((A)AM(jt->locsyms));
    // NO FAILURES ALLOWED FROM HERE TO RESTORE
// obsolete       }else SEGFAULT;  // scaf
// obsolete      }
   }
   // Before we recur on the contents of the name, add the name we are currently looking up to the list of encountered names
   // so that we abort if we hit a loop.  BUT if the current name is uv defined in a higher level, it can't be part of
   // a loop (since we are advancing the symbol pointer) and the name, which is just 'u', might well come up again; so we don't
   // add the name to the table in that case.  NOTE bug: an indirect locative a__b, if it appeared twice, would be detected as a loop even
   // if it evaluated to different locales
// obsolete     if(savloc==jt->locsyms){AAV1((A)IAV0(aa)[1])[AN((A)IAV0(aa)[1])]=rifvs(y); AN((A)IAV0(aa)[1])++; AS((A)IAV0(aa)[1])[0]++;} // add name-string to list of visited names for recursion check
   if(savloc==jt->locsyms){((A*)IAV0(aa)[2])[initn]=f; IAV0(aa)[1]++;} // add name to list of visited names for recursion check
   z=REFIXA(na,x);  // recur on the name.  We return with success, error, or cycle start depth
// obsolete     AN((A)IAV0(aa)[1])=AS((A)IAV0(aa)[1])[0]=initn;   // restore name count
   IAV0(aa)[1]=initn;   // restore name count for later uses at this level
   SYMRESTORELOCALGLOBAL(savloc,savglob);  // make sure we restore current symbols  THIS IS THE RESTORE
   if(likely(((I)z&~255)!=0)){   // if no error or cycle
    if(unlikely((ai!=0)>(FAV(x)->flag&VNOSELF))){RZ(z=fixrecursive(sc(ai),z))}  // if a lower name contains $:, replace the whole named verb with an explicit equivalent
    ASSERT(PARTOFSPEECHEQ(AT(w),AT(z)),EVTYPECHG);  // if there was a change of part-of-speech during the fix, that's a pun, don't allow it
   }else if(z!=0){  // cycle found
    if(initn>=(I)z)R z;     // we found a cycle, which started at z-1.  If this call is the start of the cycle, have it return w to close the cycle.  Keep the cycle up until that happens
    z=w;
   }else R z;  // error, abort
// obsolete    RZ(z);
// obsolete   }
// obsolete   RE(z);
   R z;
  }else{f=REFIXA(2,f); R255(f) if(fo==f)R w; R df1(z,f,wf);}
// bug ^: and m} should process gerund args
 case COBVERSE:
  if(aif&FIXASTOPATINV)R w;  // stop at obverse if told to
  // otherwise fall through to normal processing
 default:
  ASSERTSYS(((I)f|(I)g),"f and g both 0, but VNONAME not set")     // should not occur.  f and g are both off only in primitives, where VNONAME+VNOSELF should be set (except u./v.)
  if(f){f=REFIXA(na,f); R255(f);}
  if(g){g=REFIXA(na,g); R255(g);}
  if((((I)fo^(I)f)|((I)go^(I)g))==0)R w; R f&&g?df2(z,f,g,wf):f?df1(z,f,wf):w;
 }
}   // fix name, with a containing flags

// On internal calls, self is an integer whose value contains flags.  Otherwise zeroionei is used
// bivalent; called by user as f./f: with w omitted or flags.  self is ds(f./f:)
// called internally with w holding internal flags, and self=0
// w=repl level: 0=top only 1=each name once 2=all names (default) 3=not locatives 4=not globals
DF2(jtfix){F12IP;PROLOG(0005);A z;
 ARGCHK1(a);
 if(LIT&AT(a)){ASSERT(1>=AR(a),EVRANK); RZ(a=nfs(AN(a),CAV(a),0));}   // convert string to name, to allow us to fix modifiers
 // only verbs/noun can get in through the parser, but internally we also vet adv/conj
 ASSERT(AT(a)&NAME+VERB+ADV+CONJ,EVDOMAIN);
 STACKCHKOFL  // make sure we can't recur to a name by removing the name
 w=w==self?num(2):w;   // monad defaults to full replace
 I rqtype=i0(w); RE(0);  // get the requested operation
 if(self!=0){  // if not internal call, convert req to flags
  ASSERT(BETWEENC(rqtype,0,4),EVDOMAIN) rqtype=((I)1<<FIXAFCOX)<<rqtype;  // audit value & convert to one-hot
 } 
// obsolete  self=AT(self)&NOUN?self:zeroionei(0);  // default to 0 if noun not given
 // To avoid infinite recursion ae keep an array of names that we have looked up.  We create that array here, initialized to empty.  To pass it into fixa, we create
 // a faux INT block to hold the value, and use AM in that block to point to the list of names.  The fauxblock has rank 0 but 2 items
// obsolete  A namelist; GAT0(namelist,BOX,248,1); AS(namelist)[0]=AN(namelist)=0;  // allocate 248 slots with rank 1, but initialize to empty
 I namelist[128];  // work area to hold names being fixed.  Only a tiny bit will be used
 fauxblock(fauxself); A augself; fauxINT(augself,fauxself,3,0); IAV0(augself)[0]=rqtype; IAV0(augself)[1]=0; IAV0(augself)[2]=(I)namelist;  // transfer value to writable block; install empty name array
 RZ(z=fixa(augself,AT(a)&VERB+ADV+CONJ?a:symbrdlock(a)));  // name comes from string a
 // the fixed version may still contain a name, if there was a cycle
// obsolete  // Once a node has been fixed, it doesn't need to be looked at ever again.  This applies even if the node itself carries a name.  To indicate this
// obsolete  // we set VNONAME+VNOSELF.  We only do so if the node has descendants (or a name). We can do this only if we are sure the entire tree was traversed, i. e. we were not just looking for implicit locatives or inverses.
// obsolete  if(!(rqtype&(FIXALOCSONLY|FIXALOCSONLYLOWEST|FIXASTOPATINV))&&AT(z)&VERB+ADV+CONJ){V*v=FAV(z); if(v->fgh[0]){v->flag|=VNONAME+VNOSELF;}}  // f is clear for anything in the pst
 EPILOG(z);
}
