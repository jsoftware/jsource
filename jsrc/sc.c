/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
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
DF2(jtunquote){A z;
 A thisname=FAV(self)->fgh[0]; A fs; A explocale;   // the A block for the name of the function (holding an NM) - unless it's a pseudo-name   fs is the 'named' function itself, cached or looked up  explocale=explicit locale if any
 F2PREFIP;  // We understand inplacing.  We check inplaceability of the called function.
 {if(unlikely(JT(jt,adbreakr)[0]>1)){jtjsignal2(jt,EVATTN,thisname); R 0;}}  // this is JBREAK, but we force the compiler to load thisname early
 ARGCHK2(a,w);  // w is w or self always, must be valid
 A savname=jt->curname;  // we stack the executing name
#define FLGPSEUDOX 16  // operation is dyad
#define FLGPSEUDO ((I)1<<FLGPSEUDOX)
#define FLGCACHEDX 17  // operation is dyad
#define FLGCACHED ((I)1<<FLGCACHEDX)
#define FLGDYADX 19  // operation is dyad
#define FLGDYAD ((I)1<<FLGDYADX)
// bits 0-15 hold the currstackx before execution
 I flgd0cpC=w!=self?FLGDYAD:0; // self is right now; if it =w, we must be processing a monad 1=pseudofunction 2=cached lookup 8=execution of dyad bits 8-23=jt->callstacknext at start
 if(likely(thisname!=0)){  // normal names, not pseudo
  jt->curname=thisname;  // set failing name before we have value errors
  // normal path for named functions
  if((fs=FAV(self)->localuse.lu1.cachedref)!=0){
   // There is a cached lookup for this nameref - use it
   // If it has a (necessarily direct named) locative, we must fetch the locative so we switch to it
   if(unlikely((explocale=(A)(I)(NAV(thisname)->flag&NMLOC))!=0)){  // most verbs aren't locatives. if no direct locative, leave explocale=0.  This is a pun on 0
    if(unlikely((explocale=FAV(self)->localuse.lu0.cachedloc)==0)){  // if we have looked it up before, keep the lookup
     RZ(explocale=stfindcre(AN(thisname)-NAV(thisname)->m-2,1+NAV(thisname)->m+NAV(thisname)->s,NAV(thisname)->bucketx));  //  extract locale string, find/create locale
     FAV(self)->localuse.lu0.cachedloc=explocale;  // save named lookup calc for next time  should ra locale or make permanent?
    }
   }
   flgd0cpC|=FLGCACHED;  // indicate cached lookup, which also tells us that we have not ra()d the name
  }else{
   // name was not cached.  Look it up.  The calls to the lookup routines all issue ra() (under lock) on the value if found
   if(likely(!(NAV(thisname)->flag&(NMLOC|NMILOC|NMIMPLOC)))) {  // simple name, and not u./v.
    explocale=0;  // flag no explicit locale
    if(likely((fs=probelocal(thisname,jt->locsyms))==0)){fs=jtsyrd1((J)((I)jt+NAV(thisname)->m),NAV(thisname)->s,NAV(thisname)->hash,jt->global);  // Try local, then look up the name starting in jt->global
      // this is a pun - probelocal returns QCGLOBAL semantics, but we know the value is local, so we treat that as not NAMED
    }else{raposlocal(QCWORD(fs));  // incr usecount to match what syrd1 does.  
    }
   }else{  // locative or u./v.
    if(likely(!(NAV(thisname)->flag&NMIMPLOC))){  // locative
     // see if the locale is cached.  This will help name"n mainly, but also stored xctls like public_z_ =: entry_loc_ where entry_loc will have the locale pointer
     if((explocale=FAV(self)->localuse.lu0.cachedloc)==0){  // use cached locale if there is one
      RZ(explocale=sybaseloc(thisname));  //  get the explicit locale.  0 if erroneous locale
      if(!(NAV(thisname)->flag&NMILOC))FAV(self)->localuse.lu0.cachedloc=explocale;  // save named lookup calc for next time  should ra locale or make permanent?
     }
     fs=jtsyrd1((J)((I)jt+NAV(thisname)->m),NAV(thisname)->s,NAV(thisname)->hash,explocale);  // Look up the name starting in the locale of the locative
    }else{  // u./v.  We have to look at the assigned name/value to know whether this is an implied locative (it usually is)
     if(fs=probelocal(thisname,jt->locsyms)){
      // u/v, assigned by xdefn.  Implied locative.  Use switching to the local table as a flag for restoring the caller's environment
      explocale=jt->locsyms;  // We have to use this flag trick, rather than stacking the locales here, because errors after the stack is set could corrupt the stack
      raposlocal(QCWORD(fs));  // incr usecount to match what syrd1 does
     }
    }
   }
   // fs has QCNAMED semantics
   ASSERT(fs!=0,EVVALUE);  // name must be defined
   I namedloc=(I)fs&QCNAMED; fs=QCWORD(fs);  // extract NAMED flag from fs, clear other flags
   // ** as of here we know there is a value for the name, and it has been ra()d.  We must not take an error exit without fa
   ASSERTSUFF(PARTOFSPEECHEQACV(AT(self),AT(fs)),EVDOMAIN,z=0; goto exitfa;);   // make sure its part of speech has not changed since the name was parsed
   // if this reference allows caching (lI4[0]<0), save the value if it comes from a cachable source, and attach the primitive block to the name
   // we have to wait till here to 
   if(unlikely((FAV(self)->flag2>>(VF2CACHEABLEX-QCNAMEDX))&namedloc)){   // cacheable nameref, and value found in a named locale
    // point the nameref to the lookup result.  This prevents further changes to the lookup
    WRITELOCK(fs->lock);  // we want to cache a name only once
    if(FAV(self)->localuse.lu1.cachedref==0){  // if this is not true, someone else beat us to the cache.  OK, we'll get it next time.  This ensures only one cache calculation
     FAV(self)->localuse.lu1.cachedref=fs;  // store cached address, with FAOWED semantics (not owed)
     ACSETPERM(fs);  // make the cached value immortal
     // set the flags in the nameref to what they are in the value.  This will allow compounds using this nameref (created in the parsing of later sentences)
     // to use the flags.  If we do PPPP, this will be too late
     FAV(self)->flag=FAV(fs)->flag&(VIRS1+VIRS2+VJTFLGOK1+VJTFLGOK2+VASGSAFE);  // combining flags, do not require looking into id
     FAV(self)->flag2=FAV(fs)->flag2&(VF2WILLOPEN1+VF2USESITEMCOUNT1+VF2WILLOPEN2W+VF2WILLOPEN2A+VF2USESITEMCOUNT2W+VF2USESITEMCOUNT2A);  // combining flags, do not require looking into id
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
    }
    WRITEUNLOCK(fs->lock);
   }
  }
 }else{
  // here for pseudo-named function.  The actual name is in g, and the function itself is pointed to by h.  The verb is an anonymous explicit modifier that has received operands (but not arguments)
  // The name is defined, but it has the value before the modifier operands were given, so ignore fields in it except for the name
  jt->curname=thisname=FAV(self)->fgh[1];  // get the original name
  explocale=0;  // flag no explicit locale
  fs=FAV(self)->fgh[2];  // point to the actual executable
  ASSERT(fs!=0,EVVALUE); // make sure the name's value is given also
  ASSERT(TYPESEQ(AT(self),AT(fs)),EVDOMAIN);   // make sure its part of speech has not changed since the name was parsed
  // The pseudo-named function was created under debug mode.  If the same sequence had been parsed outside of debug, it would have been anonymous.  This has
  // implications: anonymous verbs do not push/pop the locale stack.  If bstkreqd is set, ALL functions will push the stack here.  That is bad, because
  // it means that a function that modifies the current locale behaves differently depending on whether debug is on or not.  We set a flag to indicate the case
  flgd0cpC|=FLGPSEUDO;  // indicate pseudofunction, and also that we did not ra() the value of the name (OK since anonymous)
 }
 // value of fs has been ra()d unless it was cached or pseudo.  We must undo that if there is error
#if NAMETRACK
 // bring out the name, locale, and script into easy-to-display name
 C trackinfo[256];  // will hold name followed by locale and scriptname
 forcetomemory(&trackinfo);
 mvc(sizeof(trackinfo),trackinfo,1,iotavec-IOTAVECBEGIN+' ');  // clear name & locale
 UI wx=0, wlen;   // index/len we will write to
 wlen=AN(thisname); wlen=wlen+wx>sizeof(trackinfo)-3?sizeof(trackinfo)-3-wx:wlen; MC(trackinfo+wx,NAV(thisname)->s,wlen); wx+=wlen+1;  // copy in the full name
 A locnm=LOCNAME(jt->global);  // name of current global locale
 wlen=AN(locnm); wlen=wlen+wx>sizeof(trackinfo)-2?sizeof(trackinfo)-2-wx:wlen; MC(trackinfo+wx,NAV(locnm)->s,wlen); wx+=wlen+1;  // copy in the locale name
 if((flgd0cpC&(FLGCACHED|FLGPSEUDO))==0){  // there is a name to look up
  A sna; I snx;
  if((sna=scind(box(sfn(0,thisname))))&&(snx=BIV0(sna))>=0){
   READLOCK(JT(jt,startlock)) wlen=AN(AAV(JT(jt,slist))[snx]); wlen=wlen+wx>sizeof(trackinfo)-1?sizeof(trackinfo)-1-wx:wlen; MC(trackinfo+wx,CAV(AAV(JT(jt,slist))[snx]),wlen); READUNLOCK(JT(jt,startlock)) wx+=wlen;  // copy in the locale name
  }
 }
 trackinfo[wx]=0;  // null-terminate the info
#endif
#if !USECSTACK
 I d=FAV(fs)->fdep; if(!d)RE(d=fdep(fs));  // get stack depth of this function, for overrun prevention
 FDEPINC(d);  // verify sufficient stack space - NO ERRORS until FDEPDEC below
#endif
 STACKCHKOFLSUFF(z=0; goto exitfa;)
 flgd0cpC|=jt->callstacknext; // Remember where our stack frame starts.  We may add an entry or two; execution may add more
 if(unlikely(explocale!=0)){  // there is a locative or implied locative
  // locative. switch to it, stacking the prev value.  If the switch is to the current local symbol table, that means 'switch to caller's environment'
  if(unlikely(explocale==jt->locsyms)){
   pushcallstack1dsuff(CALLSTACKPUSHLOCALSYMS,jt->locsyms,z=0; goto exitfa;);  // save current locsyms
   jt->locsyms=(A)AM(jt->locsyms);  // get the local syms at the time u/v was assigned; make them current.  Leave GST unchanged in it
   explocale=AKGST(jt->locsyms);  // fetch global syms for the caller's environment, so we stack it next
  }
  pushcallstack1dsuff(CALLSTACKPOPLOCALE,jt->global,z=0; goto exitpop;); jt->global=explocale;  // move to new implied locale.  DO NOT change locale in lt->locsyms.  It is set only by explicit action so that on a chain of locatives it stays unchanged
  INCREXECCT(explocale);   // we are starting a new execution in explocale.  Protect the locale while it runs
    // scaf if someone deletes the locale before we start it, we are toast
 }
 // ************** errors must pop the stack and unra() before exiting
 AF actionfn=FAV(fs)->valencefns[flgd0cpC>>FLGDYADX];  // index is 'is dyad'.  Load here to allow call address to settle.  If we move this any higher it gets spilled to memory
 w=flgd0cpC&FLGDYAD?w:fs;  // set up the bivalent argument with the new self, since fs may have been changed

 // Execute the name.  First check 4 flags at once to see if anything special is afoot: debug, pm, bstk, garbage collection
 if(likely(!(jt->uflags.ui4))) {
  // No special processing. Just run the entity
  // We have to raise the usecount, in case the name is deleted while running.  But that will be very rare.  Plus, we know that the executable type is recursive and non-inplaceable.
  // So, all we have to do is increment the usecount.  If it's a PERMANENT symbol no harm will be done, since we decrement below
  // CODING NOTE: after considerable trial and error I found this ordering, whose purpose is to start the load of the indirect branch address as early as
  // possible before the branch.  Check the generated code on any change of compiler.
  // Recursion through $: does not go higher than the name it was defined in.  We make this happen by pushing the name onto the $: stack
  // We preserve the XDEFMODIFIER flag in jtinplace, because the type of the exec must not have been changed by name loookup.  Pass the other inplacing flags through if the call supports inplacing
  A s=jt->parserstackframe.sf; jt->parserstackframe.sf=fs; z=(*actionfn)((J)(((FAV(fs)->flag&(1LL<<((flgd0cpC>>FLGDYADX)+VJTFLGOK1X)))?-1:-JTXDEFMODIFIER)&(I)jtinplace),a,w,fs); jt->parserstackframe.sf=s;  // keep all flags in jtinplace
  // Undo the protection.  If, most unusually, the usecount goes to 0, back up and do the full recursive decrement
 } else {
  // Extra processing is required.  Check each option individually
  DC d=0;  // pointer to debug stack frame, if one is allocated
  if(jt->uflags.us.cx.cx_us){  // debug or pm
   // allocate debug stack frame if we are debugging OR PM'ing.  In PM, we need a way to get the name being executed in an operator
   RZSUFF(d=deba(DCCALL,flgd0cpC&FLGDYAD?a:0,flgd0cpC&FLGDYAD?w:a,fs),z=0; goto exitpop;); d->dcn=(I)fs;   // save executing value for redef checks
  }

  if(jt->uflags.us.cx.cx_c.pmctr)pmrecord(thisname,jt->global?LOCNAME(jt->global):0,-1L,flgd0cpC&FLGDYAD?VAL2:VAL1);  // Record the call to the name, if perf monitoring on
  // If we are required to insert a marker for each call, do so (if it hasn't been done already).  But not for pseudo-named functions
  if(!(flgd0cpC&FLGPSEUDO) && jt->uflags.us.uq.uq_c.bstkreqd && (US)flgd0cpC==jt->callstacknext){
   pushcallstack1dsuff(CALLSTACKPOPLOCALE,jt->global,z=0; goto exitpop;); INCREXECCT(jt->global);  // push the call, and increment the count of the new exec (which is the same locale as the old)
  }  //  If cocurrent is about, make every call visible
  if(jt->uflags.us.cx.cx_c.db&&!(jt->glock||VLOCK&FAV(fs)->flag)&&jt->recurstate<RECSTATEPROMPT){  // The verb is locked if it is marked as locked, or if the script is locked; if recursive JDo, can't enter debug suspension so ignore debug
   z=dbunquote(flgd0cpC&FLGDYAD?a:0,flgd0cpC&FLGDYAD?w:a,fs,d);  // if debugging, go do that. 
  }else{
   A s=jt->parserstackframe.sf; jt->parserstackframe.sf=fs; z=(*actionfn)((J)(((REPSGN(SGNIF(FAV(fs)->flag,(flgd0cpC>>FLGDYADX)+VJTFLGOK1X)))|(JTXDEFMODIFIER|~JTFLAGMSK))&(I)jtinplace),a,w,fs); jt->parserstackframe.sf=s;
  }
  if(jt->uflags.us.cx.cx_c.pmctr)pmrecord(thisname,jt->global?LOCNAME(jt->global):0,-2L,flgd0cpC&FLGDYAD?VAL2:VAL1);  // record the return from call
  if(jt->uflags.us.uq.uq_c.spfreeneeded)spfree();   // if garbage collection required, do it
  if(d)debz();  // release stack frame if allocated
 }
#if !USECSTACK
 FDEPDEC(d);
#endif

 // Now pop the stack.  Execution may have added entries, but our stack frame always starts in the same place.
 // We may add entries to the end of the caller's stack frame
 //
 // The possibilities are:
 // 1. nothing (most likely) - we called a simple name and it returned
 // 2. just a CALLSTACKPOPLOCALE - we called a locative and it returned.  We have to restore the implied locale, which was saved on the stack
 // 3. 18!:4 puts a CALLSTACKPOPFROM entry giving the implied locale before the switch.  We expect that 18!:4 is called through a cover name and if we see it on the stack we
 //   suppress restoring the global locale when the current name (which must be the cover name) completes.  But, we have to make sure that the previous name, which
 //   might not have had any stack entry at all, knows to reset the implied locales when IT finishes.  [The problem is that there might be a string of calls with nothing on the stack,
 //   and then the 18!:4, and you need a marker to indicate which call the 18!:4 applies to.]  So, at the very end, we put a CALLSTACKPOPLOCALEFIRST entry into the
 //   CALLER's stack to notify it of what its starting locale was.  But there is a further mess: there might be oodles of calls to 18!:4 is a row, and if each one pushes CALLSTACKPOPLOCALEFIRST into
 //   its caller, the stack will overflow.  To prevent this, when we push the CALLSTACKPOPLOCALEFIRST onto the caller's stack we enter a new mode, wherein EVERY call starts with
 //   POPLOCALE.  Now there is no ambiguity about where the 18!:4 applies, and we can simply remove them as they come up.
 // 4. u./v. pushes CALLSTACKPUSHLOCALSYMS to save the LOCAL symbol table.  It overrides any other locale resetting above, is always first, and is always followd by CALLSTACKPOPLOCALE
 //
 // We also handle deletion of locales as they leave execution.  Locales cannot be deleted while they are pointed to by paths.  The AC is used to see when there are
 // no references to a locale, as usual; but if the locale has been marked for deletion and is no longer running, it is half-deleted, losing its path and all its symbols,
 // and exists as a zombie until its path references disappear.  To tell is a locale is running, we use a separate 'execution count' stored in AR, which is like a low-order extension
 // of AC.  When a locale is created, it is given usecount of 1 and execution count of 1.  When it is deleted, the execution count is decremented and if it is decremented to 0, that
 // causes the locale to be half-deleted and the AC decremented, possibly resulting in deletion of the locale.  [A flag prevents half-deletion of the locale more than once.]
 // To prevent half-deletion while the locale is running, we increment the execution count when an execution (including the first) switches into the locale, and decrement the
 // execution count when that execution completes (either by a switch back to the previous locale or a successive 18!:4).
 //
 // Locale switches through 18!:4 take advantage of the fact that the cover verb does nothing except the 18!:4 function and thus cannot alter locales itself.
 // The 18!:4 is treated as the start of a new execution for execution-count purposes, and always ensures that the last thing in the caller's stack is either CALLSTACKPOPLOCALEFIRST (for the
 // very first 18!:4) or CALLSTACKCHANGELOCALE (for others).  When 18!:4 is executed, if the caller's stack ends with POPFIRST or CHANGELOCALE, that must mean that the 18!:4 being executed
 // must be the second one in that name, and therefore the execution-count of the current locale is decremented.  Likewise, when a name finishes with POPFIRST or CHANGELOCALE on the top of its own stack,
 // it knows that it executed 18!:4 and must decrement the current locale.  In all cases decrementing the locale count for the locale that the name started with is done only
 // when processing a POP returning from a locative, but if there is a POPFIRST/POPFROM in the frame, the locale that is decremented must be the one in the POPFIRST/CHANGELOCALE (and also
 // the locale in the POPFIRST/CHANGELOCALE must be from the FIRST 18!:4 which will give the starting locale, though we could equally take this value from explocale)
 //
 // Note that the POPFROM after the first propagates back through the stack until it is annihilated by the POPFIRST.  This guarantees that 
exitpop: ;
 jt->curname=savname;  // restore the executing name
 I callstackx=(US)flgd0cpC;  // extract the init call stackx, which we will use to analyze
 if(unlikely(callstackx!=jt->callstacknext)){  // normal case, with no stack, bypasses all this
  // There are stack entries.  Process them
  if(likely(((jt->callstack[callstackx].type^CALLSTACKPOPLOCALE) | ((callstackx+1)^jt->callstacknext))==0)) {   // jt->callstack[callstackx].type==CALLSTACKPOPLOCALE && callstackx+1==jt->callstacknext
   // The only thing on the stack is a simple POP.  Do the pop.  This & the previous case account for almost all the calls here
   DECREXECCT(jt->global);  // end the execution that we started with this locative
   SYMSETGLOBAL(jt->locsyms,jt->callstack[callstackx].value);  // restore global locale
   jt->callstacknext=callstackx;  // restore stackpointer for caller
  }else{
   // Locales were changed.  Process the stack fully
   // PUSHLOCALE (u./v.) will always be first in stack, and we can process it now.  If it is there it is always followed by a POP
   if(jt->callstack[callstackx].type&CALLSTACKPUSHLOCALSYMS){SYMSETLOCAL((A)jt->callstack[callstackx].value); callstackx++;}  // restore locsyms if we stacked it, and restore possibly-changed global value therein
   // The stack is [CALLSTACKPUSHLOCALSYMS] [POPLOCALE] [POPLOCALEFIRST/POPFROM/CHANGELOCALE], no more.  We have advanced callstackx past the CALLSTACKPUSHLOCALSYMS if any
   // It is possible that the stack might be malformed, if the user called 18!:4 and then other calls without return.  Boot does this, alas; in particular stdlib calls 18!:4 directly and then finishes with cocurrent,
   // Which leaves the stack holding POPFROM followed by FIRST.  In this case the intermediate locales did not get their execcts raised/lowered, but that's OK during initialization.
   // It appears that we can get out of boot by just processing the stack back to front, but it would be better for boot to use cocurrent everywhere

   A fromloc=0;  // the locale to POPFIRST, if any.  Comes from POPFROM, but can be overridden by POP
   A popdecrloc=jt->global;  // POP must decrement the execct of the locale it started.  This was put into jt->global and will still be there
      // if there is no POPFROM/POPFIRST/CHANGELOCALE, all of which indicate the the implied locale was changed by the called name.  In those cases, the
      // locale started in this name is in the POPFROM/POPFIRST/CHANGELOCALE, which will be the FIRST 18!:4 encountered.  The POPFIRST/CHANGELOCALE is reponsible for
      // decrementing the usecount of jt->global, which was incremented when the POPFIRST/CHANGELOCALE was stacked
   // Process the stack in reverse order.
   do{
    if(jt->callstack[jt->callstacknext-1].type&CALLSTACKPOPFROM){
     popdecrloc=jt->callstack[jt->callstacknext-1].value;  // recover the starting locale of this execct
     // if TOS is POPFROM, the verb issued 18!:4 (perhaps multiple times, in which case we remember the first).  We must remember
     // the locale as of the 18!:4 as a flag, so we don't reset the global pointer, and also as a value in case we have to add a POPLOCALEFIRST to it.
     // If the verb is u./v., we ignore the POPFROM so that the global value in the extended definition doesn't change
     if((US)flgd0cpC==callstackx){
      // The just-completed name did n>0 18!:4s, and n-1 INCR/DECR pairs.  It is left to us to INCR for the last value, left in jt->global
      INCREXECCT(jt->global);  // signal execution started on the last 18!:4
      fromloc=jt->callstack[jt->callstacknext-1].value;  // remember original locale when calling the cover for 18!:4
     }
    }else if(jt->callstack[jt->callstacknext-1].type&CALLSTACKCHANGELOCALE+CALLSTACKPOPLOCALEFIRST){
     popdecrloc=jt->callstack[jt->callstacknext-1].value;  // recover the starting locale of this execct
     // if TOS is POPLOCALEFIRST/CHANGELOCALE, the execution of this name included a change of locale.  We know this name will be popping, because CHANGELOCALE is created only when we are pushing on every call.
     // Therefore we must terminate the execution of the global locale.
     // EXCEPTION for boot kludge: if this stack frame also includes POPFROM, the execution of the implied locale will be inherited by the caller.  Don't terminate.  Cannot occur if cover used for 18!:4
     if(!(jt->callstacknext-1>callstackx&&jt->callstack[jt->callstacknext-2].type&CALLSTACKPOPFROM))DECREXECCT(jt->global);  // end execution of the last switched locale - unless it persists (kludge)
     if(jt->callstack[jt->callstacknext-1].type&CALLSTACKPOPLOCALEFIRST){SYMSETGLOBAL(jt->locsyms,jt->callstack[jt->callstacknext-1].value); jt->uflags.us.uq.uq_c.bstkreqd = 0;}  // processing FIRST takes us back to fast mode
    }else if(jt->callstack[jt->callstacknext-1].type&CALLSTACKPOPLOCALE){
     // if this name did POPLOCALE, we have to terminate the execution that it started (now in jt->global); and also restore the implied locale EXCEPT when we had POPFROM indicating that the current name
     // is a cover for 18!:4.  In that case we suppress the restore
     DECREXECCT(popdecrloc);  // end execution of the locale this POP moved to
     if(fromloc==0){SYMSETGLOBAL(jt->locsyms,jt->callstack[jt->callstacknext-1].value);  // pop to the caller's value, except when we are the cover for 18!:4
     }else{fromloc=jt->callstack[jt->callstacknext-1].value;}  // if we are the cover for 18!:4, we may be a locative (usually the POP would not change the locale).  In case it does, we must point to the locale we are returning to
    }
   }while(--jt->callstacknext>callstackx);  // process the whole stack in reverse order

   jt->callstacknext=(US)flgd0cpC;  // restore stackpointer for caller.  The following pushes are onto the caller's stack
   // NOTE: if there is no higher executing name, these pushes will never get popped.  That would correspond to trying to delete the locale that is running at the console,
   // or typing (cocurrent 'a') into the console, which would leave a POP on the stack, never to be executed because there is no higher function to return to base locale.
   // There is no way to detect this, because names that don't change locales don't leave a trace, and thus there is no guarantee that the function-call stack will
   // be at 0 when the last name returns, because the name might have been called from the middle of a tacit expression that already had a function-call depth when the
   // name was called.
   // Therefore, we reset the name-stack pointer whenever we call from console level (jt->uflags.us.uq.uq_c.bstkreqd too)

   // We have the implied locale and execution counts right.  If the name we are running did 18!:4 to change the locale in the caller, we have to
   // install a stack entry in the caller to indicate that fact.  BUT we have to keep the stack to just a single POPLOCALEFIRST/POPFROM/CHANGELOCALE, so we may have
   // to look into the caller's stack to see if there is something already there.  If there is, we add nothing, because we need the earliest pop-to locale.
   if(fromloc){  // if we have just called 18!:4...
    if(!jt->uflags.us.uq.uq_c.bstkreqd){
     // We are still in fast mode.  That means that there in no 18!:4 info on the stack, and we can safely add CALLSTACKPOPLOCALEFIRST to the caller's stack.
     pushcallstack1(CALLSTACKPOPLOCALEFIRST,fromloc); jt->uflags.us.uq.uq_c.bstkreqd=1;
    }else{
     // We are in slow mode, which means the caller's stack must contain something.  It can't contain POPFROM, which is erased after it is read
     if(jt->callstack[jt->callstacknext-1].type&CALLSTACKCHANGELOCALE+CALLSTACKPOPLOCALEFIRST){
       // if the caller already has a locale-change token, we don't add another one.  The caller must have called the 18!:4 cover previously.  The previous implied locale, which
       // we expected would be decremented on return from the caller, needs to be decremented now instead.  The new implied locale is retained and will be decremented when
       // the CHANGE/FIRST is executed.
       DECREXECCT(fromloc);   // undo the INCR of the formerly-ending exec, which is now just a middle
     }else{
      // First 18!:4 in the caller.  Add a CHANGELOCALE entry to cause the implied locale to be decremented before the caller returns.
      pushcallstack1(CALLSTACKCHANGELOCALE,fromloc);
     }
    }
   }
  }
 }
 // ************** errors OK now
exitfa:
if(likely(!(flgd0cpC&(FLGCACHED|FLGPSEUDO)))){faacv(fs);}  // unra the name if it was looked up from the symbol tables
R z;
}

// This code needs to be executed before returning to the initial caller (i. e. the keyboard, or at end of task).
// It cleans up any stack entries left by the first named call (since there was no end-of-stack to handle them)
void jtstackepilog(J jt, I4 initcurrstack){
 // the stack may contain POPFIRST or POPFROM if the call changed locales, either properly by cocurrent or illicitly by 18!:4.  In either case we leave the implied locale as the
 // verb left it, i. e. NOT simulating a return to a named call in the FE.  But we do process the stack 
 while(jt->callstacknext>initcurrstack){  // discard the stack for this call.  This largely follows the code at the end of unquote
  if(jt->callstack[jt->callstacknext-1].type&CALLSTACKPOPFROM){
   // if TOS is POPFROM, the user executed 18!:4 from the keyboard.  The new locale has not been started, so we do nothing.
  }else if(jt->callstack[jt->callstacknext-1].type&CALLSTACKCHANGELOCALE+CALLSTACKPOPLOCALEFIRST){
   // The called function switched locales and incremented the count for the new locale.  We must close that execution
   DECREXECCT(jt->global);  // end execution of the last switched locale
   if(jt->callstack[jt->callstacknext-1].type&CALLSTACKPOPLOCALEFIRST){jt->uflags.us.uq.uq_c.bstkreqd = 0;}  // processing FIRST takes us back to fast mode
    // We don't go to fast mode willy-nilly because we could be an interrupt handler and the interrupted function may be in slow mode
  }else if(jt->callstack[jt->callstacknext-1].type&CALLSTACKPOPLOCALE){
   // Since we know we didn't do a POP, there's no need to look for one
  }
  --jt->callstacknext;
 } // process the whole stack in reverse order
}


// return ref to adv/conj/verb whose name is a and whose value/type is val (with QCGLOBAL semantics)
// if the value is a noun, we just return the value; otherwise we create a 'name~' block
// and return that; the name will be resolved when the name~ is executed.
// If the name is undefined, return a reference to [: (a verb that always fails)
// This verb also does some processing designed to reduce register usage in the parser:
//  * if val is not 0, it is freed
//  * the flags from val are transferred to the result (with QCFAOWED semantics)
// The value is used only for flags and rank
A jtnamerefacv(J jt, A a, A val){A y;V*v;
 y=val?QCWORD(val):ds(CCAP);  // If there is a value, use it; if not, treat as [: (verb that creates error)
 if(unlikely((NOUN&AT(y))!=0))R (A)((I)val|QCFAOWED);  // if noun, keep the flags, and indicate we didn't fa() it
 // This reference might escape into another context, either (1) by becoming part of a
 // non-noun result; (2) being assigned to a global name; (3) being passed into an explicit modifier: so we clear the bucket info if we ra() the reference
 v=FAV(y);
 // We cannot be guaranteed that the definition in place when a reference is created is the same value that is there when the reference
 // is used.  Thus, we can't guarantee inplaceability by copying INPLACE bits from f to the result, and we just set INPLACE for everything
 // and let unquote use the up-to-date value.
 // ASGSAFE has a similar problem, and that's more serious, because unquote is too late to stop the inplacing.  We try to ameliorate the
 // problem by making [: unsafe.
 A z=fdef(0,CTILDE,AT(y), jtunquote,jtunquote, a,0L,0L, (v->flag&VASGSAFE)+(VJTFLGOK1|VJTFLGOK2), v->mr,lrv(v),rrv(v));  // create value of 'name~', with correct rank, part of speech, and safe/inplace bits
 if(likely(val!=0))fa(QCWORD(val))else val=(A)QCVERB;  // release the value, now that we don't need it.  If val was 0, get flags to install into reference t indicate [: is a verb
 RZ(z);
 // if the nameref is cachable, either because the name is cachable or name caching is enabled now, mark it cacheable
 // If the nameref is cached, we will fill in the flags in the reference after we first resolve the name
 FAV(z)->flag2|=(NAV(a)->flag&NMCACHED || (jt->namecaching && !(NAV(a)->flag&(NMILOC|NMDOT|NMIMPLOC))))<<VF2CACHEABLEX;  // enable caching if called for
 R (A)((I)z|QCPTYPE(val));  // no FAOWED since we freed val
}

// return reference to the name given in w, used when moving from queue to stack
// For a noun, the reference points to the data, and has rank/shape info
// For other types, we build a function ref to 'name~', and fill in the type, rank, and a pointer to the name;
//  the name will be dereferenced when the function is executed
//  if fa is needed, we do it here
A jtnameref(J jt,A w,A locsyms){A z;
 ARGCHK1(w); z=syrd(w,locsyms);
 RZ(z=namerefacv(w,z));  // make a reference to the name & fa the value
 if(unlikely((I)z&QCFAOWED)){tpush(QCWORD(z));}  // if free is owed, tpush it h & cancel the request
 R QCWORD(z);
}    /* argument assumed to be a NAME */

// Adverb 4!:8 create looked-up cacheable reference to (possibly boxed) literal name a
// The name must be defined.  It supplies the type and rank of the reference.  We require the name to be defined so that
// there will not be a circular reference if a name in a numbered locale is a reference to the same name
F1(jtcreatecachedref){F1PREFIP;A z;
 A nm; RZ(nm=onm(w)); // create name from arg
 ASSERT(!(NAV(nm)->flag&(NMILOC|NMDOT|NMIMPLOC)),EVDOMAIN) // if special name or indirect locative, error
 A val=QCWORD(syrd(nm,(A)(*JT(jt,emptylocale))[THREADID(jt)]));  // look up name, but not in local symbols.  We start with the current locale (?? should start with the path?)
 ASSERT(val!=0,EVVALUE);  // return if error or name not defined
 ASSERT(!(AT(val)&NOUN),EVDOMAIN)
 RZ(z=fdef(0,CTILDE,AT(val), jtunquote,jtunquote, nm,0L,0L, (val->flag&VASGSAFE)+(VJTFLGOK1|VJTFLGOK2), FAV(val)->mr,lrv(FAV(val)),rrv(FAV(val))));// create reference
 FAV(z)->localuse.lu1.cachedref=val;  // install cached address of value
 ACSETPERM(val);  // now that the value is cached, it lives forever
 RETF(z);
}

// Result has type ':' but goes to unquote.  We mark a pseudo-named entity by having f=0, g=name, h=actual entity to execute
F2(jtnamerefop){V*v;
 ARGCHK2(a,w);
 v=FAV(w);
 R fdef(0,CCOLON,VERB,  jtunquote,jtunquote, 0L,a,w, VXOPCALL|v->flag, v->mr,lrv(v),rrv(v));
}    

/* namerefop() is used by explicit defined operators when: */
// - debug is on or PM is on
/* - operator arguments have been supplied                 */
/* - function arguments have not yet been supplied         */
// w is an anonymous entity that we want to give the name a to for debug purposes


// u./v.
// We process this as 'u'~ where the name is flagged as NMIMPLOC
// Bivalent: called with (a,w,self) or (w,self).  We treat as dyad but turn it into monad if input w is not a noun
DF2(jtimplocref){
 self=AT(w)&NOUN?self:w;
 self=JT(jt,implocref)[FAV(self)->id&1];  // namerefs for u and v
 w=AT(w)&NOUN?w:self;  // see whether we're a dyad
 R unquote(a,w,self); // call as (w,self,self) or (a,w,self)
}
