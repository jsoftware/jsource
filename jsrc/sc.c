/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Symbol Table: Named Function Call (unquote)                                   */

#include "j.h"

// This function handles both valences: monad as (w,self,self), dyad as (a,w,self).  self is the name reference.  Never called without 3 args
// JTXDEFMODIFIER is set if the name is a modifier: not used here, but passed on - xdefn uses it
//
// This routine calls a 'named' function, which was created by name~ or the equivalent for a stacked verb.
// It also handles pseudo-named functions, which are anonymous entities that need to be given a temporary name
// when they are running under debug.  Pseudo-named functions are created by namerefop.  We need to run them here so they get the debug side-effects of having a name.
DF2(jtunquote){F12IP;A z;
 // we stack sf ($:), curname, and local/global symbols, restoring them all on return (except perhaps globals which
 // is changed by cocurrent).  The layout in JTT must match the layout here
 struct unqstack {
  A sf;  // $:
  A curname;  // the executing name  sf/curname must stay together for stacking
  A global;  //  implied locale
  A locsyms;  // local symbol table
 } stack;

#define FLGDYAD 1   // conjunction or dyadic verb
#define FLGLOCATIVEX JTXDEFMODIFIERX  // 8 name is a direct or indirect locative (not u./v.)
#define FLGLOCATIVE ((I)1<<FLGLOCATIVEX)
#define FLGNMFLGX 9   // place where we store flags from name
    // bits 9-16 hold name flags
#define FLGPSEUDOX 17  // operation is pseudo-named function
#define FLGPSEUDO ((I)1<<FLGPSEUDOX)
#define FLGCACHEDX 18  // operation is cached, not requiring lookup
#define FLGCACHED ((I)1<<FLGCACHEDX)
#define FLGANONX 19  // operation is pseudo-named function but only because of debug/pm.  The name should not be used to change locales
#define FLGANON ((I)1<<FLGANONX)
#define FLGLOCINCRDECRX 20  // this call is to a new locale & must incr/decr execcts
#define FLGLOCINCRDECR ((I)1<<FLGLOCINCRDECRX)
#define FLGLOCCHANGEDX 21  // the caller of this function has previously encountered cocurrent
#define FLGLOCCHANGED ((I)1<<FLGLOCCHANGEDX)
   // bits 22-28 must be 0 - they are stored into bstkreqd
#define FLGFLAG2X 32  // location of FLAG2 flags

 UI8 flgvbnmgen=(UI8)__atomic_load_n(&FAV(self)->flag2,__ATOMIC_RELAXED);   // extract self flags, which we will need quickly, init dyad flag
 A thisname=__atomic_load_n(&FAV(self)->fgh[0],__ATOMIC_RELAXED); A fs;   // the A block for the name of the function (holding an NM) - unless it's a pseudo-name   fs is the 'named' function itself, cached or looked up
 C flagsfromname=__atomic_load_n(&NAV(thisname)->flag,__ATOMIC_RELAXED);  // get flags from name
// obsolete  I flgvbnmgen=w!=self;  // init DYAD flag
   // We check inplaceability of the called function.  jtfg unused
 A explocale;  // locale to execute in.  Not used unless LOCINCRDECR set
 {if(unlikely(JT(jt,adbreakr)[0]>1)){jtjsignal2(jt,EVBREAK,thisname); R 0;}}  // this is JBREAK, but we force the compiler to load thisname early
 ARGCHK2(a,w);  // w is w or self always, must be valid
 flgvbnmgen=(flgvbnmgen<<FLGFLAG2X)+(w!=self);  // move self flags into position, install dyad flag
#if C_AVX2 || EMU_AVX2
 _mm256_storeu_si256((__m256i *)&stack,_mm256_loadu_si256((__m256i *)&jt->parserstackframe.sf));
#else
 memcpy(&stack,&jt->parserstackframe.sf,sizeof(stack));  // push sf/globals/curname/locals
#endif
 // *** errors must to go the error exit to restore stacked values
 jt->curname=thisname;  // set executing name before we have value errors.  We will refresh thisname as needed after calls so the compiler won't have to save/restore it
 // trace explicit routine
 if(unlikely(traceexplicit && jt->curname)) {
  A g=jt->global; NM*v; v=NAV(LOCNAME(g)); // v->n,v->s  current locale name
#ifdef ANDROID
  if(traceexplicit==1||traceexplicit==2) __android_log_print(ANDROID_LOG_DEBUG, (const char*)"jtrace", "jtrace i %.*s %.*s\n", NAV(jt->curname)->n, NAV(jt->curname)->s, v->n, v->s);
#else
  if(traceexplicit==1) fprintf(stdout, "jtrace i %.*s %.*s\n", NAV(jt->curname)->n, NAV(jt->curname)->s, v->n, v->s);
  else if(traceexplicit==2) fprintf(stderr, "jtrace i %.*s %.*s\n", NAV(jt->curname)->n, NAV(jt->curname)->s, v->n, v->s);
#endif
  else if(traceexplicit==3) { ASSERT(traceexpfile,EVFACE)
   FILE * p; p= fopen (traceexpfile, "a"); ASSERT(p, EVFACE)
   fprintf(p, "jtrace i %.*s %.*s\n", NAV(jt->curname)->n, NAV(jt->curname)->s, v->n, v->s); fclose(p);
  }
 }
 if(likely(!(flgvbnmgen&((UI8)VF2PSEUDONAME<<FLGFLAG2X)))){  // normal names, not pseudo
  // normal path for named functions
  if(likely((fs=FAV(self)->localuse.lu1.cachedlkp)!=0)){  // fetch the most recent lookup, which may be reused. no QC
   if(flgvbnmgen&((UI8)VF2CACHEABLE<<FLGFLAG2X)){  // long cacheable?  (test first because test suite expects it)
    if(likely(flgvbnmgen&((UI8)VF2CACHED<<FLGFLAG2X))){  // cached already?
     // the reference is cached.  Switch to it immediately.
     // If it has a (necessarily direct named) locative, we must fetch the locative so we switch to it
     if(((A)(I)(flagsfromname&NMLOC)!=0)){  // most verbs aren't locatives. if no direct locative, leave global unchanged
      explocale=FAV(self)->localuse.lu0.cachedloc;  // it's a direct locative, so we must have looked it up when we cached the ref
      flgvbnmgen+=((explocale!=jt->global)&~(LXAV0(explocale)[SYMLEXECCT]>>EXECCTPERMX))<<FLGLOCINCRDECRX;  // remember that there is a change of locale, but not if it is permanent
      flgvbnmgen+=FLGLOCATIVE;  // remember if this call is a locative
      SYMSETGLOBAL(explocale);   // switch to the (possibly new) locale.
     }
     flgvbnmgen+=FLGCACHED;  // indicate cached lookup, which also tells us that we have not ra()d the name
     goto finlookup;     // NM flags have not been inserted into flagsfromname, not needed
    }
   }else{  // if long cacheable, don't allow short caching, else long cache would seldom get used (and it's faster)
    if(likely(FAV(self)->lu2.refvalidtime==ACVCACHEREADLOCK)){  // is previous lookup still valid
     // Short caching: the previous lookup can be reused because there have been no assignments to/from ACV
     flgvbnmgen+=flagsfromname<<FLGNMFLGX;  // insert name flags into flag reg.  They should have settled from the load
     raposgblqcgsv(fs,0,fs); // ra to match syrd1.  The 0 guarantees no recursion, i. e. no subroutine call, OK because the value must not be sparse
     ACVCACHEREADUNLOCK   // must keep the lock till the cached value is protected
     if(unlikely(flgvbnmgen&(NMLOC<<FLGNMFLGX))){  // is this a (necessarily direct) locative?
// obsolete       // see if the locale is cached.   public_z_ =: entry_loc_ where entry_loc will have the locale pointer
// obsolete       if(unlikely((explocale=cachedlocale)==0)){  // use cached locale if there is one (there will be, except first time through)  If not...
// obsolete        RZSUFF(explocale=sybaseloc(thisname),z=0; goto exitname;);  //  get the explicit locale.  0 if erroneous locale
// obsolete           // if the name is deleted before sybaseloc runs, we might get a starting locale that doesn't match the definition in fs.  That will be confusing but will not crash.  It is
// obsolete           // very unlikely and occurs only when names are being deleted during execution, which is confusing to begin with
// obsolete        FAV(self)->localuse.lu0.cachedloc=explocale;  // save named lookup calc for next time  should ra locale or make permanent?
// obsolete        thisname=jt->curname;  // refresh thisname
// obsolete       }
      explocale=FAV(self)->localuse.lu0.cachedloc;  // get the locale of the lookup
      flgvbnmgen+=((explocale!=jt->global)&~(LXAV0(explocale)[SYMLEXECCT]>>EXECCTPERMX))<<FLGLOCINCRDECRX;  // remember if there is a change of locale to non-PERMANENT, requiring putting the new locale into execution
      flgvbnmgen+=FLGLOCATIVE;  // remember this call is a locative
      SYMSETGLOBAL(explocale);   // switch to the (possibly new) locale.  We DO NOT change AKGST because if we are calling a non-operator modifier we need the old locale if the modifier calls u./v. .
         // AKGST changes only from cocurrent.  This leads to a divergence between jt->global and AKGST if a non-operator modifier calls an anonymous explicit that issues cocurrent.  BFD.  The divergence
         // is removed by the next named call
     } // simple names just use the cached value
     goto finlookup; // all caches bypass the check for change of type since that would be the least of our worries
    }else{ACVCACHEREADUNLOCK FAV(self)->localuse.lu1.cachedlkp=0;}  // timestamp changed, removed cacheability to avoid another miss
   }
  }
  // here the address was not cached, or the cache was stale.  For namerefs caching we go through here to refetch the value because we need QCNAMEDLOC semantics
  {
   // name was not cached.  Look it up, returning QCNAMEDLOC semantics.  The calls to the lookup routines all issue ra() (under lock) on the value if found
// obsolete    I nmflgs=NAV(thisname)->flag;   // flags from name, before we call subroutines
   flgvbnmgen+=flagsfromname<<FLGNMFLGX;  // insert name flags into flag reg.  They should have settled from the load
   UI4 localts=ACVCACHEREAD;  // record timestamp of lookup.  This will be used only if we cache the lookup.  We must do this BEFORE ra/syrd protects the value
   if(likely(!(flagsfromname&(NMLOC|NMILOC|NMIMPLOC)))) {  // simple name, and not u./v.
    // We must not use bucket info for the local lookup, because the reference may have been created in a different context
    J jtx=(J)((I)jt+NAV(thisname)->m); C *sx=NAV(thisname)->s; UI4 hashx=NAV(thisname)->hash;
    if(unlikely(AR(jt->locsyms)&ARHASACV)&&unlikely((fs=CLRNAMEDLOC(probex(NAV(thisname)->m,sx,SYMORIGIN,hashx,jt->locsyms)))!=0)){  // check local syms only if they have an ACV
     raposlocal(QCWORD(fs),fs); // if found locally, protect it, as if found by syrd1.  Set not-NAMEDLOC.
    }else{fs=jtsyrd1(jtx,sx,hashx,jt->global);  // not found in local, search global
    }
    // leave LOCINCRDECR unset and jt->global unchanged
   }else{  // locative or u./v.
    // there are subroutine calls here; we refresh thisname sometimes so the compiler won't carry it in a register
    if(likely(!(flgvbnmgen&(NMIMPLOC<<FLGNMFLGX)))){  // not implicit locative...
     // see if the locale is cached.  This will help name"n mainly, but also stored xctls like public_z_ =: entry_loc_ where entry_loc will have the locale pointer
     if((explocale=FAV(self)->localuse.lu0.cachedloc)==0){  // use cached locale if there is one.  If not...
      RZSUFF(explocale=sybaseloc(thisname),z=0; goto exitname;);  //  get the explicit locale.  0 if erroneous locale
      thisname=jt->curname;  // refresh thisname
     }
     if(unlikely(AR(explocale)&ARLOCALTABLE)){
      // the locative specifies a local table, which must have originated from name___n.  This requires extra work,
      // like u./v.  We switch both global & local tables, and then look up the name locally.  If not found, carry on
      jt->locsyms=explocale; explocale=AKGST(jt->locsyms);  // move to new locals and their globals (up the local-sym chain)
      if((fs=CLRNAMEDLOC(probex(NAV(thisname)->m,NAV(thisname)->s,SYMORIGIN,NAV(thisname)->hash,jt->locsyms)))!=0)goto fslocal;  // look only in local symbols.  Set not-NAMEDLOC. if found, continue as for u./v.
      thisname=jt->curname;  // refresh thisname
      // falling through, we have switched the symbol tables and we didn't find the name locally.  Continue looking through the path
     }else flgvbnmgen+=FLGLOCATIVE;  // remember if this call is a locative - but not a local table 
     fs=jtsyrd1((J)((I)jt+NAV(thisname)->m),NAV(thisname)->s,NAV(thisname)->hash,explocale);  // Look up the name starting in the locale of the locative

    }else{  // u./v.  We have to look at the assigned name/value to know whether this is an implied locative (it usually is)
     if(likely((fs=CLRNAMEDLOC(probex(NAV(thisname)->m,NAV(thisname)->s,SYMORIGIN,NAV(thisname)->hash,jt->locsyms)))!=0)){  // look only in local symbols.  Set not-NAMEDLOC
      // u/v, assigned by xdefn.  Implied locative.  Switch locals and globals to caller's environment
      jt->locsyms=(A)AM(jt->locsyms); explocale=AKGST(jt->locsyms);  // move to new locals and their globals (up the local-sym chain)
fslocal:;  // come here when the name we are about to execute was found in a local symbol table.  We must ra() to protect the name, because syrd1() ra()s all other names
           // and the local case is rare: we match the ra() so that every non-cached executing name has been ra()d explicitly
      raposlocal(QCWORD(fs),fs);   // incr usecount to protect fs during execution (needed only for local names)
     }else explocale=jt->global;  // explocale must be defined for flgvbnmgen
    }
    flgvbnmgen|=((explocale!=jt->global)&~(LXAV0(explocale)[SYMLEXECCT]>>EXECCTPERMX))<<FLGLOCINCRDECRX;  // remember that there is a change of locale to non-PERMANENT
    SYMSETGLOBAL(explocale);   // switch to the (possibly new) locale.  We DO NOT change AKGST because if we are calling a non-operator modifier we need the old locale if the modifier calls u./v. .
        // AKGST changes only from cocurrent.  This leads to a divergence between jt->global and AKGST if a non-operator modifier calls an anonymous explicit that issues cocurrent.  BFD.  The divergence
        // is removed by the next named call
   }
   // Common path for named functions that we looked up here.  fs has QCNAMEDLOC semantics.  short-term caches bypass this.  The rare local names have been ra()d
   // explocale is the locale we are calling into
   ASSERTSUFF(fs!=0,EVVALUE,z=0; goto exitname;);  // name must be defined
   I namedloc=(I)fs&QCNAMEDLOC; fs=QCWORD(fs);  // extract NAMED flag from fs, clear other flags
   // ** as of here we know there is a value for the name, and it has been ra()d.  We must not take an error exit without fa.  fs has noQC semantics
   ASSERTSUFF(PARTOFSPEECHEQACV(AT(self),AT(fs)),EVDOMAIN,z=0; fa(fs); goto exitname;);   // make sure its part of speech has not changed since the name was parsed; if error must use general fa
   // at this point we could short-circuit names that call names (e. g. public =: name_loc_) by checking that the call is to jtunquote and is not a locative.  This would save about half
   // of this routine.  It's not clear that the frequency is worth the test

   // *** now that we know the lookup was valid, save it for next time, including locale.
   // This is a little different between short- and long-term caches, because of the possibility that the
   // locale is numbered/private.  Such locales are unsuitable for long-term caches since the locale
   // may disappear.  They are OK for short-term caches because deleting the locale invalidates the timestamp
   if(likely(!(flgvbnmgen&((UI8)VF2CACHEABLE<<FLGFLAG2X)))){  // if only short-term cache is possible...
    // for short-term cache, save the lookup, and the locale too if it is a direct locale (either named or numbered).  We stored the timestamp earlier, before the value was protected
    if(!(flgvbnmgen&((NMILOC+NMIMPLOC)<<FLGNMFLGX))){  // Never cache anything for indirect or implicit locatives
     // The tricky part with this is that the reference may be in use in multiple threads while we are trying to cache it.  Because the pair fs/timestamp must be updated atomically, we
     // have to take a write lock.  We would like to avoid a read lock when checking the timestamp.  We do so at the cost of setting the timestamp to 'uncacheable' for a moment.  The
     // worst this will do is cause a very rare cache miss.  Our rule is, we must never expose a situation where reading the fs followed by the timestamp gives a timestamp that is
     // later than the correct one for the fs.
     WRITELOCK(self->lock)  // take a lock on the reference
     __atomic_store_n(&FAV(self)->lu2.refvalidtime,(UI4)0,__ATOMIC_RELEASE);  // set timestamp invalid in case it starts after the new stamp
     __atomic_store_n(&FAV(self)->localuse.lu1.cachedlkp,fs,__ATOMIC_RELEASE);     // save named lookup calc for next time.  This makes the lookup cached.  no QC
// obsolete      if(flgvbnmgen&(NMLOC<<FLGNMFLGX))
     __atomic_store_n(&FAV(self)->localuse.lu0.cachedloc,explocale,__ATOMIC_RELEASE);   // including locale in case it was looked up (used only for NMLOC references)
     __atomic_store_n(&FAV(self)->lu2.refvalidtime,localts,__ATOMIC_RELEASE);  // set timestamp for the lookup.  It may already be out of date, which is OK - it will be recreated
     WRITEUNLOCK(self->lock)  // release lock on the reference
    }
   }else if(namedloc && (!(flgvbnmgen&(NMLOC<<FLGNMFLGX)) || (LXAV0(explocale)[SYMLEXECCT]&EXECCTPERM))){   // cacheable nameref, and value found in a permanent named locale
    // ************* the nameref is long-term cachable.  Fill it in.  Happens the first time a cachable reference is encountered.
    thisname=jt->curname;  // refresh thisname
    // point the nameref to the lookup result.
    WRITELOCK(fs->lock);  // we want to cache a name only once
    if(!(flgvbnmgen&((UI8)VF2CACHED<<FLGFLAG2X))){  // if this is not true, someone else beat us to the cache.  OK, we'll get it next time.  This ensures only one cache calculation
     ACSETPERM(fs);  // make the cached value immortal
     // set the flags in the nameref to what they are in the value.  This will allow compounds using this nameref (created in the parsing of later sentences)
     // to use the flags.  If we do PPPP, this will be too late
     FAV(self)->flag=FAV(fs)->flag&(VIRS1+VIRS2+VASGSAFE);  // combining flags, do not require looking into id
     FAV(self)->flag2=(FAV(fs)->flag2&(VF2WILLOPEN1+VF2USESITEMCOUNT1+VF2WILLOPEN2W+VF2WILLOPEN2A+VF2USESITEMCOUNT2W+VF2USESITEMCOUNT2A))|(VF2CACHED+VF2CACHEABLE);  // combining flags, do not require looking into id
         // indicate cached has been filled in, preventing any further changes to the caching
// flag2: if we look through name(s) when replacing f[12] and fs, we could support VF2BOXATOP1+VF2BOXATOP2+VF2ATOPOPEN1+VF2ATOPOPEN2W+VF2ATOPOPEN2A+
//  and we might be able to use VF2RANKATOP1+VF2RANKATOP2+VF2RANKONLY1+VF2RANKONLY2+  but we haven't checked the loops yet
      // we could mark the lookup as cached, but if debug is on we want to display the lookup value first time through    ???
      // If the NM block is cachable, point it to the nameref.  The NM block must be marked cachable AND still be pointed to by the explicit definition, which
      // means that its usecount must be more than what comes from the nameref.  If the explicit definition has been deleted, we must ensure that we don't put a loop
      // in the chains, because there will never be a free from the non-nameref side to break the loop
     if(NAV(thisname)->flag&NMCACHED && AC(thisname)>((AFLAG(self)&TRAVERSIBLE)!=0)){  // name from explicit definition, and definition still active
      // This leads to a loop in the inclusion graph, as nameref and name point to each other.  We have special code in fa() for names to break the loop.
      // We must ensure that raising the nameref does not raise the usecount of the name, as it would if the nameref is not yet recursive.  If the usecount of the
      // name were raised, it would never go to 0 when the explicit definition is freed, and the block would leak.  Likewise we must undo the situation where the
      // nameref was raised before this caching: that would set the name usecount to 2 and freeing the explicit verb would not trigger revisiting the link to the
      // nameref.  In short, when there is a cached ref from the name, the count of the name is always 1, and the nameref has been incremented: so the name will
      // not go away until the explicit does, and when that happens, the link will be removed in fa().
      NAV(thisname)->cachedref=(A)((I)self+ATYPETOVALTYPE(AT(self)));   // cache the reference, with QCFAOWED semantics.  FAOWED is off
      ra(self); ACSET(thisname,ACUC1);   // exp def is ALWAYS recursive usecount, so we raise self when we store to it.
         //  This wipes out bucket info in self, but that will not be needed since we have cached the lookup
     }
     FAV(self)->localuse.lu1.cachedlkp=fs;     // save named lookup calc for next time  should ra locale or make permanent?
     if(flgvbnmgen&(NMLOC<<FLGNMFLGX))FAV(self)->localuse.lu0.cachedloc=explocale;   // including locale it is was looked up in
    }
    WRITEUNLOCK(fs->lock);
   }
  }

 }else{
  // here for pseudo-named function.  The actual name is in f, and the function itself is pointed to by h.  The verb is an anonymous explicit modifier that has received operands (but not arguments)
  // The name is defined, but it has the value before the modifier operands were given, so ignore fields in it except for the name
// obsolete   jt->curname=thisname;   // use the original name, which may have locatives that we will ignore
// obsolete =FAV(self)->fgh[1];  // get the original name
  fs=FAV(self)->fgh[2];  // point to the actual executable
  ASSERTSUFF(fs!=0,EVVALUE,z=0; goto exitname;); // make sure the name's value is given also
  flgvbnmgen+=FLGPSEUDO;  // indicate pseudofunction, and also that we did not ra() the value of the name (OK since anonymous)
  ASSERTSUFF(TYPESEQ(AT(self),AT(fs)),EVTYPECHG,z=0; goto exitfa;);   // make sure its part of speech has not changed since the name was parsed - that's a pun in type
  if(FAV(self)->fgh[1]!=0){  // if there is a locale specified, we must inherit that locale for this execution

   explocale=FAV(self)->fgh[1];  // explocale must be defined for flgvbnmgen
   flgvbnmgen|=((explocale!=jt->global)&~(LXAV0(explocale)[SYMLEXECCT]>>EXECCTPERMX))<<FLGLOCINCRDECRX;  // remember if there is a change of locale to non-PERMANENT
   SYMSETGLOBAL(explocale);   // switch to the (possibly new) locale.  We DO NOT change AKGST because if we are calling a non-operator modifier we need the old locale if the modifier calls u./v. .
  }else{
   // The pseudo-named function was created under debug/pm mode.  If the same sequence had been parsed outside of debug, it would have been anonymous.  This has
   // implications: anonymous verbs do not restore locales.  To preserve that behavior, we divert the call to the slow path and then skip restoring the locale
   // at the return
   flgvbnmgen+=FLGANON; jt->uflags.spfreeneeded|=0x80;  //  indicate anonymous call, force this call through the long path
  }
  // NOTE: we will update the $: recursion point during this call, because recursion doesn't go through a name.  But what about a pseudo-name?  If the name comes from a locative, we treat it like any other
  // named ref and recur to fs.  If it is truly anonymous, it can only be a call to xdefn during pm/debug: xdefn will start a new parse and the recursion point is immaterial.
 }
finlookup:;  // here when short- or long-term cache hits.  We know that no pun is possible on either cache hit   NOTE: NM flags have not been inserted into flg reg if we branch to here, thus must not be used below
 // value of fs has been ra()d unless it was refcached or pseudo.  We must undo that if there is error

#if NAMETRACK
 // bring out the name, locale, and script into easy-to-display name
 C trackinfo[256];  // will hold name followed by locale and scriptname
 forcetomemory(&trackinfo);
 mvc(sizeof(trackinfo),trackinfo,1,iotavec-IOTAVECBEGIN+' ');  // clear name & locale
 UI wx=0, wlen;   // index/len we will write to
 wlen=AN(thisname); wlen=wlen+wx>sizeof(trackinfo)-3?sizeof(trackinfo)-3-wx:wlen; MC(trackinfo+wx,NAV(thisname)->s,wlen); wx+=wlen+1;  // copy in the full name
 A locnm=LOCNAME(stack.global);  // name of global locale before call
 wlen=AN(locnm); wlen=wlen+wx>sizeof(trackinfo)-2?sizeof(trackinfo)-2-wx:wlen; MC(trackinfo+wx,NAV(locnm)->s,wlen); wx+=wlen+1;  // copy in the locale name
 if((flgvbnmgen&(FLGCACHED|FLGPSEUDO))==0){  // there is a name to look up
  A sna; I snx;
  if((sna=scind(box(sfn(0,thisname))))&&(snx=BIV0(sna))>=0){
   READLOCK(JT(jt,startlock)) wlen=AN(AAV(JT(jt,slist))[snx]); wlen=wlen+wx>sizeof(trackinfo)-1?sizeof(trackinfo)-1-wx:wlen; MC(trackinfo+wx,CAV(AAV(JT(jt,slist))[snx]),wlen); READUNLOCK(JT(jt,startlock)) wx+=wlen;  // copy in the locale name
  }
 }
 trackinfo[wx]=0;  // null-terminate the info
#endif
 AF actionfn=FAV(fs)->valencefns[flgvbnmgen&FLGDYAD];  // index is 'is dyad'.  Load here to allow call address to settle.  There are no calls from here to fn dispatch
 w=flgvbnmgen&FLGDYAD?w:fs;  // set up the bivalent argument with the new self, since fs may have been changed (if pseudo-named function)
 STACKCHKOFLSUFF(z=0; goto exitfa;)  // this could be in an infinite-reursion loop; check
 // Move to the destination locale.  The question is, What to do when the locale doesn't change?  The INCREXECCT is a single
 // lock inc instruction, but suppose all the cores are executing names in base?  The contention for the base locale will
 // be horrid.  To avoid that, we update execct only during a change.
 // At this point jt->global is the new locale to use (possibly inherited).  If LOCINCRDECR is set,
 // explocale also holds that value.  LOCINCRDECR is set if we should incr/decr explocale, which is true if it changes
 // to a non-permanent locale (we don't want executions in z/base/... to hammer the z count)
 if(flgvbnmgen&FLGLOCINCRDECR){ACVCACHECLEAR; INCREXECCT(explocale);}  // incr execct in newly-starting locale.  Invalidate name lookups on locale change
 // scaf if someone deletes the locale before we start it, we are toast
 // ************** from here on errors must (optionally) decr explocale  and unra() before exiting
 // Recursion through $: does not go higher than the name it was defined in.  We make this happen by pushing the name onto the $: stack
 jt->parserstackframe.sf=fs; // as part of starting the name we set the new recursion point
 // ******** Execute the name.  First check 4 flags at once to see if anything special is afoot: debug, pm, bstk, garbage collection
 if(likely(!(jt->uflags.ui4))) {
  // No special processing. Just run the entity as (a,w,self) or (w,self,self)
  // We preserve the XDEFMODIFIER flag in jtfg, because the type of the exec must not have been changed by name lookup.  Pass the other inplacing flags through
  z=(*actionfn)(jtfg,a,w,fs);  // keep jt flags incl MODIFIER 
  if(unlikely(z==0)){jteformat(jt,jt->parserstackframe.sf,a,w,0);}  // make this a format point
 }else{
  // Extra processing is required.  Check each option individually
  DC d=0;  // pointer to debug stack frame, if one is allocated
  if(jt->uflags.trace){  // debug or pm
   // allocate debug stack frame if we are debugging OR PM'ing.  In PM, we need a way to get the name being executed in an operator
   RZSUFF(d=deba(DCCALL,flgvbnmgen&FLGDYAD?a:0,flgvbnmgen&FLGDYAD?w:a,fs),z=0; goto exitpop;); fs=jt->parserstackframe.sf; d->dcn=(I)fs; d->dcloc=jt->locsyms;   // save executing value for redef checks; init dcloc in case it's a tacit definition; refresh fs
  }

  A execlocname=LOCNAME(jt->global);  // locale name for logging, known not to change since we haven't popped the executing locale yet
  if(jt->uflags.trace&TRACEPM){pmrecord(jt->curname,execlocname,-1L,(flgvbnmgen&FLGDYAD)+1); fs=jt->parserstackframe.sf;}  // Record the call to the name, if perf monitoring on
  // transfer the bstkreqd flag to our internal flags so we can continue it on after the return, and clear it for the called function.  The idea is that bstkreqd has info about
  // the current caller, and is reset for the next level.
  // But: if this is a anonymous function, we need to skip the save/restore of locales, so we skip the update
  flgvbnmgen|=jt->uflags.bstkreqd<<FLGLOCCHANGEDX;  // remember the flag.  If anon, this will be unused
  jt->uflags.bstkreqd&=flgvbnmgen>>FLGANONX;  // clear bstk for next level (if not anonymous)
  // call the function, as above
  if((jt->uflags.trace&TRACEDB)&&!(jt->glock||VLOCK&FAV(fs)->flag)&&!(jt->recurstate&RECSTATERENT)){  // The verb is locked if it is marked as locked, or if the script is locked; don't debug/pm any recursive entry
   z=jtdbunquote(jtfg,a,w,fs);  // if debugging, go do that.  Uses jt->sitop as stack frame
  }else{
   z=(*actionfn)(jtfg,a,w,fs);
   if(unlikely(z==0)){jteformat(jt,jt->parserstackframe.sf,a,w,0);}  // make this a format point
  }
  if(jt->uflags.trace&TRACEPM)pmrecord(jt->curname,execlocname,-2L,(flgvbnmgen&FLGDYAD)+1);  // record the return from call
  if(d)debz();  // release stack frame if allocated
  if(unlikely(flgvbnmgen&FLGANON)){jt->uflags.spfreeneeded&=~0x80; goto exitanon;}  // if this was an anonymous explicit verb, skip over locale restoration (it can't be cocurrent)
  if(jt->uflags.spflag){                        // Need to do some form of space reclamation?
   if(jt->uflags.sprepatneeded)jtrepatrecv(jt); // repatriate first, because we might reclaim enough memory to need to gc
   if(jt->uflags.spfreeneeded&1)spfree();
  }        // if garbage collection required, do it
  fs=jt->parserstackframe.sf;  // restore fs
 }
 // the call has returned, long way or short way

 if(unlikely((I)jtfg&flgvbnmgen&FLGLOCATIVE) && likely(z!=0) && !(AT(z)&NOUN)){  // can't be cocurrent
  // a locative modifier returned a non-noun.  We create a pseudoname to remember the name and locale that were used.
  if(!(FAV(z)->flag2&VF2PSEUDONAME) || (FAV(z)->fgh[1]!=0))RZSUFF(z=namerefop(jt->curname,z), goto exitpop;);  // If we can't use the locale address in an existing pseudoname, allocate a new one
  A currloc=jt->global; ra(currloc); FAV(z)->fgh[1]=currloc;  // remember locale for this pseudoname - must ra since z is recursive
 }

 // We also handle deletion of locales as they leave execution.  Locales cannot be deleted while they are pointed to by paths.  The AC is used to see when there are
 // no references to a locale, as usual; but if the locale has been marked for deletion and is no longer running, it is half-deleted, losing its path and all its symbols,
 // and exists as a zombie until its path references disappear.  To tell if a locale is running, we use a separate 'execution count' stored in AR, which is like a low-order extension
 // of AC.  When a locale is created, it is given usecount of 1 and execution count of 1.  When it is deleted, the execution count is decremented and if it is decremented to 0, that
 // causes the locale to be half-deleted and the AC decremented, possibly resulting in deletion of the locale.  [A flag prevents half-deletion of the locale more than once.]
 // To prevent half-deletion while the locale is running, we increment the execution count when an execution (including the first) switches into the locale, and decrement the
 // execution count when that execution completes (either by a switch back to the previous locale or a successive 18!:4).
exitpop: ;
 // LSB of z is set in the return iff what we just called was cocurrent
 // jt->locsyms here is always the same as before the call (but may have been changed previously for u./v.)
 // jt->global may have changed, if the called function executed cocurrent
 if(unlikely(((C)(I)z|jt->uflags.bstkreqd)&1)){  // cocurrent OR return when called function changed locale?
  ACVCACHECLEAR;  // either way we have to clear ACV cache
  if((C)(I)z&1){  // was successful call to cocurrent?
   // here the name we called was cocurrent, possibly through a locative.  This is where we change jt->global in the caller
   // The incr/decrs here are on behalf of the caller of cocurrent.  We always incr the locale we move to; we decr a locale
   // only if a previous cocurrent moved to it; the first time we don't know whether it was incrd, and we leave
   // the decr to the caller of cocurrent
   z=(A)((I)z&~1); INCREXECCTIF(z);  // z has the locale to switch to.  clear flag bit; indicate new execution starting in the locale we switch to
   // we temporarily started an execution for the locative (now jt->global).  We will close that below in common code
   if(flgvbnmgen&FLGLOCCHANGED)DECREXECCTIF(stack.global)   // if the caller's locale was moved to by cocurrent (i.e. this is the second+ cocurrent in the calling function), it must be replaced by z.  No DECR is performed for the FIRST cocurrent, so if that was entered by a locative
         // it will be owed a DECR.  We can't DECR the first call because it might be in execution higher in the stack and already have a delete outstanding
   stack.global=z;  // ... and into the area we will pop from, thus storing through to the caller
   z=mtm;  // we have switched; this will be the result of cocurrent
   flgvbnmgen|=FLGLOCCHANGED; // leave bstkreqd set as a flag indicating next function's caller has encountered cocurrent
  }else if(jt->uflags.bstkreqd)DECREXECCTIF(jt->global)   // we are returning from this function, which called cocurrent.  Close the final change of locale
 }
 jt->uflags.bstkreqd=(C)(flgvbnmgen>>FLGLOCCHANGEDX);  // bstkreqd is set after the return if the CALLER OF THE EXITING ROUTINE has seen cocurrent.  This was passed into the exiting routine as FLGLOCCHANGED.  bstkreqd is set to be used by either the next call or the return from this caller
 if(unlikely(flgvbnmgen&FLGLOCINCRDECR)){ACVCACHECLEAR; DECREXECCT(explocale)}  // If we used a locative, undo its incr.  If there were cocurrents, the incr was a while back
 // ************** errors OK now
exitfa:;  // error point for errors after symbol res. 
 // this is an RFO cycle that will cause trouble if there are many cores running the same names without cached lookups
if(likely(!(flgvbnmgen&(FLGCACHED|FLGPSEUDO)))){fanamedacv(fs);}  // unra the name if it was looked up from the symbol tables
exitname:; // error point for name errors.
 SYMSETGLOBALINLOCAL(stack.locsyms,stack.global);   // we will restore jt->global, which might have changed early or as late as the deletion; make sure locsyms matches.  global and AKGST always match for the named explicit routine that is running.
    // if an explicit routine calls a tacit name via locative, globals and AKGST will diverge while the tacit verb runs; if the tacit verb then calls a (possibly named) explicit, the explicit's u. will be from the earlier explicit, not the intervening tacit.
 // trace explicit routine
 if(unlikely(traceexplicit && jt->curname)) {
  A g=jt->global; NM*v; v=NAV(LOCNAME(g)); // v->n,v->s  current locale name
#ifdef ANDROID
  if(traceexplicit==1||traceexplicit==2) __android_log_print(ANDROID_LOG_DEBUG, (const char*)"jtrace", "jtrace o %.*s %.*s\n", NAV(jt->curname)->n, NAV(jt->curname)->s, v->n, v->s);
#else
  if(traceexplicit==1) fprintf(stdout, "jtrace o %.*s %.*s\n", NAV(jt->curname)->n, NAV(jt->curname)->s, v->n, v->s);
  else if(traceexplicit==2) fprintf(stderr, "jtrace o %.*s %.*s\n", NAV(jt->curname)->n, NAV(jt->curname)->s, v->n, v->s);
#endif
  else if(traceexplicit==3) { ASSERT(traceexpfile,EVFACE)
   FILE * p; p= fopen (traceexpfile, "a"); ASSERT(p, EVFACE)
   fprintf(p, "jtrace o %.*s %.*s\n", NAV(jt->curname)->n, NAV(jt->curname)->s, v->n, v->s); fclose(p);
  }
 }
#if C_AVX2 || EMU_AVX2
 _mm256_storeu_si256((__m256i *)&jt->parserstackframe.sf,_mm256_loadu_si256((__m256i *)&stack));
#else
 memcpy(&jt->parserstackframe.sf,&stack,sizeof(stack));  // restore sf/curname/globals/locals
#endif
RETF(z);
exitanon:;  // exit point for anonymous pseudo-named verbs, which must not restore locales so that they act like other anonymous verbs
memcpy(&jt->parserstackframe.sf,&stack,2*sizeof(A));  // restore sf/curname only
RETF(z);
}


// return ref to adv/conj/verb whose name is a and whose value/type is val (with QCSYMVAL semantics if it is not 0; but typeflags may be 0 in a NONzero value if the result should be value/0)
// if the value is a noun, we just return the value with its flags; otherwise we create a 'name~' block
// and return that with the part of speech of the value; the name will be resolved when the name~ is executed.
// If the name is undefined, return a reference to [: (a verb that always fails)
// This verb also does some processing designed to reduce register usage in the parser:
//  * if val is not 0, it is freed if it is not local
//  * the flags from val are converted to QCFAOWED semantics, and if a reference is created it is NOT NAMED
// If value is NMCACHED and eligible, we store away a pointer to the original reference, which will be a sentence word.  When the reference is looked
// up it will add a pointer to the reference in the sentence word, which will establish the caching.  Otherwise the value is used only for flags and rank
// The reference lookup is initialized with QCFAOWED (not named, no fa owed) semantics (0/QCVERB if undefined ref) and the current asngct
A jtnamerefacv(J jt, A a, A val){A y;V*v;
 y=likely(val!=0)?QCWORD(val):ds(CCAP);  // If there is a value, use it; if not, treat as [: (verb that creates error)
 if(unlikely(((I)val&QCNOUN)!=0))R SYMVALTOFAOWED(val);  // if noun, keep the flags: if global, indicate it needs eventual fa()
 // This reference might escape into another context, either (1) by becoming part of a
 // non-noun result; (2) being assigned to a global name; (3) being passed into an explicit modifier: so we clear the bucket info if we ra() the reference
 v=FAV(y);
 // We cannot be guaranteed that the definition in place when a reference is created is the same value that is there when the reference
 // is used.  Thus, we can't guarantee inplaceability by copying INPLACE bits from f to the result, and we just set INPLACE for everything
 // and let unquote use the up-to-date value.
 // ASGSAFE has a similar problem, and that's more serious, because unquote is too late to stop the inplacing.  We try to ameliorate the
 // problem by making [: unsafe.
 // if there is an error at this point, we must be working on an undefined mnuvxy.  In that case, keep the error and don't allocate a block
 // if the nameref is cachable, either because the name is cachable or name caching is enabled now, mark it cacheable
 // If the nameref is cached, we will fill in the flags in the reference after we first resolve the name
 // In any case we install the current asgnct and the looked-up value (unflagged).  We will know that the cached value is not a pun in type
 I flag2=(!!(NAV(a)->flag&NMCACHED) | (jt->namecaching & !(NAV(a)->flag&(NMILOC|NMMNUVXY|NMIMPLOC))))<<VF2CACHEABLEX;  // enable caching if called for
 A z=fdefnoerr(flag2,CTILDE,AT(y), jtunquote,jtunquote, a,0L,0L, (v->flag&VASGSAFE), v->mr,lrv(v),rrv(v));  // create value of 'name~', with correct rank, part of speech, and safe/inplace bits
 if(likely(val!=0)){if(ISFAOWED(val))fa(QCWORD(val))}else val=(A)QCVERB;  // release the value, now that we don't need it (if global).  If val was 0, get flags to install into reference to indicate [: is a verb
 RZ(z);  // abort if reference not allocated
 // We would like to give a start to short-caching by remembering the lookup and time, but the time must be BEFORE the point when the lookup is protected by its syrd(), and we don't have that value
// obsolete  if(likely(!(NAV(a)->flag&(NMILOC|NMIMPLOC)))){FAV(z)->localuse.lu1.cachedlkp=QCWORD(val);} // install cache of lookup, but never if indirect locative.  No QC
 if(flag2&VF2CACHEABLE){FAV(z)->localuse.lu1.cachedlkp=QCWORD(val);} // install cache of lookup, but never if indirect locative.  No QC
// obsolete  FAV(z)->lu2.refvalidtime=ACVCACHEREAD;} 
 R (A)((I)z+QCPTYPE(val));  // Give the result the part of speech of the input.  no FAOWED since we freed val; no NAMED since a reference is not a named value
}

// return reference to the name given in w, used when moving from queue to stack
// For a noun, the reference points to the data, and has rank/shape info
// For other types, we build a function ref to 'name~', and fill in the type, rank, and a pointer to the name;
//  the name will be dereferenced when the function is executed
//  if fa is needed, we do it here
A jtnameref(J jt,A w,A locsyms){A z;
 ARGCHK1(w); z=syrd(w,locsyms);
 RZ(z=namerefacv(w,z));  // make a reference to the name, with QCFAOWED semantics
 if(unlikely((I)z&QCFAOWED)){tpush(QCWORD(z));}  // if free is owed (must be noun), tpush it & cancel the request
 R QCWORD(z);
}    /* argument assumed to be a NAME */

// Adverb 4!:8 create looked-up cacheable reference to (possibly boxed) literal name a
// The name must be defined.  It supplies the type and rank of the reference.  We require the name to be defined so that
// there will not be a circular reference if a name in a numbered locale is a reference to the same name
F1(jtcreatecachedref){F12IP;A z;
 A nm; RZ(nm=onm(w)); // create name from arg
 ASSERT(!(NAV(nm)->flag&(NMILOC|NMMNUVXY|NMIMPLOC)),EVDOMAIN) // if special name or indirect locative, error
 A val=QCWORD(syrd(nm,(A)(*JT(jt,emptylocale))[THREADID(jt)]));  // look up name, but not in local symbols.  We start with the current locale (?? should start with the path?)
 ASSERT(val!=0,EVVALUE);  // return if error or name not defined
 ASSERT(!(AT(val)&NOUN),EVDOMAIN)
 z=fdef(VF2CACHED+VF2CACHEABLE,CTILDE,AT(val), jtunquote,jtunquote, nm,0L,0L, (val->flag&VASGSAFE), FAV(val)->mr,lrv(FAV(val)),rrv(FAV(val)));// create reference
 FAV(z)->localuse.lu1.cachedlkp=val;  // install cached address of value, no QC
 ACSETPERM(val);  // now that the value is cached, it lives forever
 RETF(z);
}

// Result has type ':' but goes to unquote.  We mark a pseudo-named entity by having f=name, h=actual entity to execute. g can be filled in with a locale for modifier locative
F2(jtnamerefop){F12IP;V*v;
 ARGCHK2(a,w);
 v=FAV(w);
 R fdef(VF2PSEUDONAME,CCOLONE,VERB,  jtunquote,jtunquote, a,0L,w, VXOPCALL|v->flag, v->mr,lrv(v),rrv(v));
   // VXOPCALL is checked by representations; VF2PSEUDONAME is checked by unquote.  They could be merged
}    

/* namerefop() is used by explicit defined operators when: */
// - debug is on or PM is on
/* - operator arguments have been supplied                 */
/* - function arguments have not yet been supplied         */
// w is an anonymous entity that we want to give the name a to for debug purposes


// u./v.
// We process this as 'u'~ where the name is flagged as NMIMPLOC
// Bivalent: called with (a,w,self) or (w,self,self).  We treat as dyad but turn it into monad if input w is not a noun
DF2(jtimplocref){F12IP;
 A childself=JT(jt,implocref)[FAV(self)->id&1];  // pull from namerefs for u and v
 R unquote(a,EPDYAD?w:childself,childself); // call as (w,self,self) or (a,w,self)
}

// 9!:30 query current name, '' if none
F1(jtcurnameq){F12IP;ASSERTMTV(w); if(jt->curname==0)R mtv; R str(NAV(jt->curname)->n,NAV(jt->curname)->s);}
