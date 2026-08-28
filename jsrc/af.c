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
// IAV(a)[0] is flags to handle lower recursions:
//  0 to begin with
//  1, 2, or 3 to indicate that a piece of the original input has been processed, and any $: found in a name must end with $::
// Higher flag bits restrict the search:
// FIXAFRECUR,FIXAFCOPUBLIC,FIXAFCOLOCATIVE   call for replacement of that class of names
// FIXAFIRSTIMPLOCONLY 256 // replace only the first occurrence of u./v. in each branch (IMPLOC)
// FIXASTOPATINV 512  // stop fixing a branch when it gets to a an explicit obverse 

// a has to be an A type because it goes into every2.  It is always an I type with rank 0 and AN=1 so it can go into every, but it has multiple items.
//   It may be virtual in callbacks from every2 so must not use IAV0
// IAV0(aa)[1-2] are len/addr of the recursion name-list and must be passed to all recursion levels
static A jtfixa(J jtfg,A a,A w){F12JT;A z;
#define REFIXA(a,x) (IAV0(aa)[0]=(aif|(a)), fixa((A)aa,(x)))  // a indicates which side of the input may need $::
#define R255(x) if(unlikely((I)(x)&-256)==0)R x;  // return values of 1-255 indicate cycles; return cycle position in that case.  We return the cycle position up the line till we find the start of cycle, which becomes the result
 ARGCHK1(w);
 // If we are only interested in replacing locatives, and there aren't any, exit fast
 if(NOUN&AT(w)){R w;}  // return noun value; only way a name gets here is by ".@noun which turns into ".@(name+noun) for execution.  Also in debug, but that's discarded
 if(FAV(w)->flag&VNONAME)R w;  // if value already fixed (or m : n which gets VNONAME+VNOSELF set), keep it.  This prunes all branches with no names, since VNONAME+VNOSELF is propagated from the bottom up during tree creation
 // continuing, there is a name in the branch and we have to plod through the tree to find it
 fauxblock(fauxself); A aa; fauxINT(aa,fauxself,3,0); AN(aa)=1; IAV0(aa)[1]=IAV(a)[1]; IAV0(aa)[2]=IAV(a)[2];  // place to build recursion parm - copy the recursion namelist and length
 A f=FAV(w)->fgh[0]; A g=FAV(w)->fgh[1]; A h=FAV(w)->fgh[2]; I id=FAV(w)->id;   // fetch fgh of compound, and the type
 I ai=IAV(a)[0]; I aif=ai&~3; //get value of control input a; extract control flags
 ai^=aif; I na=ai==0?3:ai;  // now ai = state without flags; for levels other than the top, use na to cause replacement of $:
 if(unlikely(aif&FIXAFIRSTIMPLOCONLY)&&!hasimploc(w))R w;  // if looking for implicit locatives, and there aren't any, nothing to fix
 A wf=ds(id);   // fetch self for w
 switch(id){  // we know that modifiers have been executed to produce verb/nouns
 // we reexecute the modifiers to use the new values.
 case CSLASH: 
  f=REFIXA(2,f); R255(f) R df1(z,f,wf);
 case CSLDOT: case CSLDOTDOT: case CBSLASH: case CBSDOT:
  f=REFIXA(1,f); R255(f) R df1(z,f,wf);
 case CTDOT:
  f=REFIXA(0,f); R255(f); g=REFIXA(na,g); R255(g); R df2(z,f,g,wf);  // recur and rebuild.  t. starts a new recursion, so don't replace $: in it
 case CATCO:
  if(FAV(g)->id==CTDOT)R REFIXA(na,g);   // t. is internally <@:t. .  Remove the <@: and continue.
// otherwise fall through to...
 case CAT: case CCUT:
  f=REFIXA(1,f); R255(f); g=REFIXA(na,g); R255(g); R df2(z,f,g,wf);  // rerun the compound after fixing the args
 case CAMP: case CAMPCO: case CUNDER: case CUNDCO:
  f=REFIXA(na,f); R255(f) g=REFIXA(1,g); R255(g) R df2(z,f,g,wf);
 case CCOLONE:  // Original m : n had VNONAME+VNOSELF set & never gets here.  This is (1) a nameref for an explicit modifier-plus-args, flagged as VXOP; (2) a namerefop for debug, flagged as PSEUDONAME+VXOPCALL+inherited flags;
                // a namerefop for a modifier locative, which looks like a debug namerefop but has the locative in g.
  if(unlikely(FAV(w)->flag2&VF2PSEUDONAME)){R REFIXA(0,h);}  // If the operator is a pseudo-name, we have to fish the actual operator block out of h
  f=REFIXA(0,f); R255(f) if(h){h=REFIXA(0,h); R255(h)} R xop2(f,h?h:g,g);  // here for nameref: xop2 is bivalent; rebuild operator with original self and fixed f/h
 case CCOLON:
  f=REFIXA(1,f); R255(f) g=REFIXA(2,g); R255(g) R df2(z,f,g,wf);  // v : v, similarly
 case CADVF:
  f=REFIXA(3,f); R255(f) g=REFIXA(3,g); R255(g) if(h){h=REFIXA(3,h); R255(h)} R hook(f,g,h);
 case CHOOK:
  f=REFIXA(2,f); R255(f) g=REFIXA(1,g); R255(g) R hook(f,g,0);
 case CFORK:
  if(h==0){h=g; g=f; f=ds(CCAP);}  // reconstitute capped fork, which has h=0
  f=REFIXA(na,f); R255(f) g=REFIXA(ID(f)==CCAP?1:2,g); R255(g) h=REFIXA(na,h); R255(h) R folk(f,g,h);  // f first in case it's [:
 case CATDOT:
  // we fix each gerund; for each component that changed, we calculate the AR of the corresponding f
  IAV0(aa)[0]=(aif|na); A h1; RZ(h1=every2(aa,h,(A)&arofixaself)); A *h1v=AAV(h1), *hv=AAV(h); // fix each component of gerund h, point to boxes
  g=REFIXA(na,g); R255(g); RZ(f=ca(f)); A *fv=AAV(f);  // fix g, and make a nonrecursive copy of f where we will record changes to h
  I flag=VNOLOCCHG+VNONAME+VNOSELF&FAV(g)->flag;  // we will collect combined NOLOCCHG/VNONAME+VNOSELF over g/h
  DO(AN(h), flag&=FAV(h1v[i])->flag; if(h1v[i]!=hv[i]){A t;RZ(t=aro(h1v[i])) INCORPNV(t) fv[i]=t;})  // update AR in f of any changed component.
  fdef(0,CATDOT,VERB, FAV(w)->valencefns[0],FAV(w)->valencefns[1], f,g,h1, flag+VGERL, RMAX, RMAX, RMAX);   // create new verb, setting flags
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
   I initn=IAV0(aa)[1];  // save name depth coming in
   if(unlikely(initn>125)){
    // the number of names processed has gotten large: check for name cycle.  If we find a cycle we return the start of the cycle+1 as the result: always < 256.  If no cycle, it's just too many names, EVLIMIT
    // this is tricky: the FIRST reference in a cycle must return a reference to itself.  We do it this way to avoid all traversals except when there is a cycle
    A *av=(A*)IAV0(aa)[2];  // pointer to list of names encountered so far
    I cyc1;  // start of cycle including last element
    C *cycs=NAV(av[initn-1])->s; I cycn=NAV(av[initn-1])->n; UI4 cych=NAV(av[initn-1])->hash;  // string and hash of the name we are checking for cycle
    DO(initn-2, NM *nm=NAV(av[i]); if(nm->hash==cych && cycn==nm->n && 0==memcmp(cycs,nm->s,cycn)){cyc1=i; goto cycfound;})
    ASSERT(0,EVLIMIT)  // no cycle found: too many names, abort
cycfound:;  // cycle found, running from cyci to cycn; now back it down to find its starting position
    I cycl=initn-1-cyc1;  // end-start of cycle
    for(--cyc1;cyc1>=0;--cyc1){NM *nm1=NAV(av[cyc1]),  *nmn=NAV(av[cyc1+cycl]); if(!(nm1->hash==nmn->hash && nm1->n==nmn->n && 0==memcmp(nm1->s,nmn->s,nm1->n)))break;}   // back up to before start of cycle
    ++cyc1;  // advance to start of cycle
    R (A)(cyc1+1);  // return start of cycle+1, which signals abort
   }
   // self-recursion check finished.  Now replace the name with its value
   A x; ASSERTN(QCWORD(x=syrd(f,jt->locsyms))!=0,EVVALUE,f);  // read name, returning QCFAOWED semantics.  Error if not defined
   I isglobal; if(isglobal=ISFAOWED(x))tpush(QCWORD(x)); x=QCWORD(x);  // remember if name was public (it can't be sparse, since it is not a noun).  If fa owed, use the tstack for it; remove QC flags
   if(unlikely(FUNC&AT(x)&&(jt->glock||FAV(x)->flag&VLOCK)))R w;  // if value is locked, don't replace it - leave the original name
   // since the name is supposed to be executable, we have to guard against a type pun on the name
   ASSERT(PARTOFSPEECHEQACV(AT(w),AT(x)),EVTYPECHG);   // make sure its part of speech has not changed since the name was parsed
   if(unlikely(AFLAG(x)&AFRO))R w;  // If name has readonly value (like cocurrent), leave it as a reference
   // See if we should replace the name with its value, based on operation type; and see if we should stop
   if(unlikely(aif&FIXAFIRSTIMPLOCONLY)&&NAV(f)->flag&NMIMPLOC)R x;  // In the special mode where we are looking for 1 locative, stop after we have found it
   if(!isglobal){
     // private name (including u./v.) is always replaced
   }else if((NAV(f)->flag&NMLOC+NMILOC+NMIMPLOC)==0){  // name was global but not locative
    if(!(aif&FIXAFCOPUBLIC))R w;  // if private name and we are not replacing private names, leave unreplaced
   }else{   // some kind of locative
    if(NAV(f)->flag&NMILOC){  // name was indirect locative
     if(!(aif&FIXAFCOLOCATIVE)){C wkarea[258];  // if not fixing locatives...
      // we have name__ind, but we are not replacing locatives.  Turn the locative into a direct locative, replacing ind with its value (if it is replaceable)
      if(!(aif&FIXAFCOPUBLIC)){   // if we are not replacing public names, we must check the status of ind
       C *ind=&NAV(f)->s[NAV(f)->n]; while(ind[-1]!='_')--ind; if(!jtprobelocal(SYMORIGIN,nfs(&NAV(f)->s[NAV(f)->n]-ind,ind,0),jt->locsyms))R w;  // convert ind to name; if not private, leave the locative unchanged
      }
      // ind is a replaceable name.  Turn the reference into name_valofind_.  There may be many inds
      A baseloc; RZ(baseloc=jtsybaseloc(jt,f))   // resolve the indirects to the defining locale
      I stglen=snprintf(wkarea,sizeof(wkarea),"%.*s_%.*s_",(int)NAV(f)->m,NAV(f)->s,(int)NAV(LOCNAME(baseloc))->n,NAV(LOCNAME(baseloc))->s);  // format the direct locative
      R namerefacv(nfs(stglen,wkarea,0),x);  // a reference to that name becomes the result
     }
    }
    if(!(aif&FIXAFCOLOCATIVE))R w;  // if name is locative and we are not replacing locatives, leave unreplaced
   }
   if(!(aif&FIXAFCORECUR))R x;  // if we are replacing only the top name, stop after we have its value
   // here we decided to keep looking for names, in the new value x
   // if this is an implicit locative, we have to switch the environment before we recur on the name for subsequent lookups.
   A savloc=jt->locsyms, savglob=jt->global;  // push the current locales, whether we change them or not
   if(unlikely(NAV(f)->flag&NMIMPLOC))SYMSWITCHTOLOCAL((A)AM(jt->locsyms));     //  on implicit locative, switch to locative's context  NO FAILURES ALLOWED FROM HERE TO RESTORE
   // Before we recur on the contents of the name, add the name we are currently looking up to the list of encountered names
   // so that we abort if we hit a loop.  BUT if the current name is uv defined in a higher level, it can't be part of
   // a loop (since we are advancing the symbol pointer) and the name, which is just 'u', might well come up again; so we don't
   // add the name to the table in that case.  NOTE bug: an indirect locative a__b, if it appeared twice, would be detected as a loop even
   // if it evaluated to different locales
   if(savloc==jt->locsyms){((A*)IAV0(aa)[2])[initn]=f; IAV0(aa)[1]++;} // add name to list of visited names for recursion check
   z=REFIXA(na,x);  // recur on the name.  We return with success, error, or cycle start depth
   IAV0(aa)[1]=initn;   // restore name count for later uses at this level
   SYMRESTORELOCALGLOBAL(savloc,savglob);  // make sure we restore current symbols  THIS IS THE RESTORE
   if(likely(((I)z&~255)!=0)){   // if no error or cycle
    if(unlikely((ai!=0)>(FAV(x)->flag&VNOSELF))){RZ(z=jtdolcoco(jt,z))}  // if a name not at top level contains $:, replace the verb with verb $:: to set a recursion point
    ASSERT(PARTOFSPEECHEQ(AT(w),AT(z)),EVTYPECHG);  // if there was a change of part-of-speech during the fix, that's a pun, don't allow it
   }else if(z!=0){  // cycle found
    if(initn>=(I)z)R z;     // we found a cycle, which started at z-1.  If this call is the start of the cycle, have it return w to close the cycle.  Keep the cycle up until that happens
    z=w;
   }else R z;  // error, abort
   R z;  // good return
  }else{f=REFIXA(2,f); R255(f)  R df1(z,f,wf);}
// bug ^: and m} should process gerund args
 case COBVERSE:
  if(aif&FIXASTOPATINV)R w;  // stop at obverse if told to
  // otherwise fall through to normal processing
 default:
  ASSERTSYS(((I)f|(I)g),"f and g both 0, but VNONAME not set")     // should not occur.  f and g are both off only in primitives, where VNONAME+VNOSELF should be set (except u./v.)
  if(f){f=REFIXA(na,f); R255(f);}
  if(g){g=REFIXA(na,g); R255(g);}
  R f&&g?df2(z,f,g,wf):f?df1(z,f,wf):w;
 }
}   // fix name, with a containing flags

// On internal calls, self is an integer whose value contains flags.  Otherwise zeroionei is used
// bivalent; called by user as f./f: with w omitted or flags.  self is ds(f./f:)
// called internally with w holding internal flags, and self=0
// w=string list of keywords (or boxed list), 'recur', 'public', or 'all'.  'public' means non-locatives
// if self=0 (internal call), bits 0-1 of w are passed through to the flags into fixa, and 'recur all' is implied
DF2(jtfix){F12IP;PROLOG(0005);A z;
 ARGCHK1(a);
 if(unlikely(LIT&AT(a))){ASSERT(1>=AR(a),EVRANK); RZ(a=nfs(AN(a),CAV(a),0));}   // convert string to name, to allow us to fix modifiers.  Only verbs/noun can get in through the parser, but internally we also vet adv/conj
 ASSERT(AT(a)&NAME+VERB+ADV+CONJ,EVDOMAIN);   // other nouns are errors
 if(unlikely(AT(a)&NAME))RZ(a=jtnameref(jt,a,jt->locsyms))  // convert a name to a reference (must have come from string - other verb names are already converted to references)
 STACKCHKOFL  // make sure we can't recur to a name by removing the name
 I reqmask;
 if(self!=0){  // not internal call
  if(w==self)reqmask=FIXAFCORECUR+FIXAFCOPUBLIC+FIXAFCOLOCATIVE;   // default for f. // recur all
  else{   // w has string args for keywords.
   if(AT(w)&LIT)RZ(w=words(w)) ASSERT(AT(w)&BOX,EVDOMAIN) ASSERT(AR(w)<2,EVRANK)   // convert to list of boxed strings
   A wx; RZ(wx=indexof(words(str(25,"single public all private")),w));   // look up each word in w among the keywords
   I *wxv=IAV(wx); reqmask=0; DQ(AN(wx), I bit=(I)1<<wxv[i]; ASSERT((reqmask&bit)==0,EVDOMAIN) reqmask|=bit;) ASSERT((reqmask&16)==0,EVDOMAIN) ASSERT(LOWESTBIT(reqmask>>1)==(reqmask>>1),EVDOMAIN)  // no misspelled or repeated words, and only one of public|all
   reqmask&=7; reqmask^=1; reqmask|=(reqmask&4)>>1; reqmask<<=FIXAFCOX;  // all implies public; shift into position
  }
 }else{reqmask=(IAV(w)[0]<<FIXAFINTX)+FIXAFCORECUR+FIXAFCOPUBLIC+FIXAFCOLOCATIVE;}   // internal call has upper flags bits only, lower implied (so we can use num() for them)

 if(unlikely(!(reqmask&FIXAFCORECUR))&&!(FAV(a)->id==CTILDE&&(AT(FAV(a)->fgh[0])&NAME)))z=a;  // if user wants to fix only the top, the top must be a reference or we leave it unfixed
 else{
  // To avoid infinite recursion ae keep an array of names that we have looked up.  We create that array here, initialized to empty.  To pass it into fixa, we create
  // a faux INT block to hold the value, and use AM in that block to point to the list of names.  The fauxblock has rank 0 but 2 items
  I namelist[128];  // work area to hold names being fixed.  Only a tiny bit will be used
  fauxblock(fauxself); A augself; fauxINT(augself,fauxself,3,0); IAV0(augself)[0]=reqmask; IAV0(augself)[1]=0; IAV0(augself)[2]=(I)namelist;  // transfer value to writable block; install empty name array
  RZ(z=fixa(augself,a));  // fix as requested.  a must be an ACV now
  // the fixed version may still contain a name, if there was a cycle
 }
 EPILOG(z);
}
