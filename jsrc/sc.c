/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Symbol Table: Named Function Call (unquote)                                   */

#include "j.h"

// This function handles both valences: monad as (w,fs,fs), dyad as (a,w,fs).  self is the name reference
//
// This routine called a 'named' function, which was created by name~ or the equivalent for a stacked verb.
// It also handles pseudo-named functions, which are anonymous entities that need to be given a temporary name
// when they are running under debug.  Pseudo-named functions are created by namerefop.  We need to run them here so they get the debug side-effects of having a name.
DF2(jtunquote){A z;I flgd0cp;  // flgs: 1=pseudofunction 2=cached lookup 8=execution of dyad
 F2PREFIP;  // We understand inplacing.  We check inplaceability of the called function.
 RE(0);
 JATTN;
 flgd0cp=w!=self?8:0; // if we were called with w,fs,fs, we are a monad.  Otherwise (a,w,fs) dyad
 V *v=FAV(self);  // V block for this V/A/C reference
 I callstackx=jt->callstacknext; // Remember where our stack frame starts.  We may add an entry; execution may add more
 A thisname=v->fgh[0]; A fs; A explocale; L *stabent;// the A block for the name of the function (holding an NM) - unless it's a pseudo-name   fs is the 'named' function itself  explocale=explicit locale if any stabent=symbol-table entry if any
 A savname=jt->curname;  // we stack the executing name
 if(likely(thisname!=0)){  // normal names
  jt->curname=thisname;  // set failing name before we have value errors
  // normal path for named functions
  I4 cachedlkp=v->localuse.lu1.cachedref;  // negative if cacheable; positive if cached
  if(cachedlkp>0){
   // There is a lookup for this nameref - use it
   stabent=&JT(jt,sympv)[cachedlkp];  // the symbol block for the cached item
   fs=stabent->val;  // the value of the cached item
   if(likely(fs!=0)){
    // the lookup exists.  If it has a (necessarily direct) locative, we must fetch the locative so we switch to it
    if(unlikely(NAV(thisname)->flag&NMLOC)){RZ(explocale=stfindcre(AN(thisname)-NAV(thisname)->m-2,1+NAV(thisname)->m+NAV(thisname)->s,NAV(thisname)->bucketx));}  //  extract locale string, find/create locale
    else explocale=0;  // if no direct locative, set so
    flgd0cp|=2;  // indicate cached lookup
   }else{cachedlkp=v->localuse.lu1.cachedref=SYMNONPERM; goto valgone;}  // if the value vanished, it must have been erased by hand.  Reenable caching but keep looking
  }else{
valgone: ;
   if(!(NAV(thisname)->flag&(NMLOC|NMILOC|NMIMPLOC))) {  // simple name, and not u./v.
    explocale=0;  // flag no explicit locale
    if(likely(!(stabent = probelocal(thisname,jt->locsyms)))){stabent=jtsyrd1((J)((I)jt+NAV(thisname)->m),NAV(thisname)->s,NAV(thisname)->hash,jt->global);}  // Try local, then look up the name starting in jt->global
   }else{  // locative or u./v.
    if(likely(!(NAV(thisname)->flag&NMIMPLOC))){  // locative
     RZ(explocale=sybaseloc(thisname));  //  get the explicit locale.  0 if erroneous locale
     stabent=jtsyrd1((J)((I)jt+NAV(thisname)->m),NAV(thisname)->s,NAV(thisname)->hash,explocale);  // Look up the name starting in the locale of the locative
    }else{  // u./v.  We have to look at the assigned name/value to know whether this is an implied locative (it usually is)
     if(stabent = probelocal(thisname,jt->locsyms)){
      // u/v, assigned by xdefn.  Implied locative.  Use switching to the local table as a flag for restoring the caller's environment
      explocale=jt->locsyms;  // We have to use this flag trick, rather than stacking the locales here, because errors after the stack is set could corrupt the stack
     }
    }
   }
   // syrd1 returns bit 0 set if the value is from a named locale, i. e. is cachable.  probelocal always returns with that flag off, since local symbols are never cachable
   ASSERT(stabent!=0,EVVALUE);  // name must be defined
   I4 cacheable=(I4)(I)stabent&ARNAMED; stabent=(L*)((I)stabent&~ARNAMED);  // extract cachable flag from stabent & clear it
   fs=stabent->val;  // fetch the value for the name
   ASSERT(fs!=0,EVVALUE); // make sure the name's value is given also
   ASSERT(PARTOFSPEECHEQACV(AT(self),AT(fs)),EVDOMAIN);   // make sure its part of speech has not changed since the name was parsed
   // if this reference allows caching (lI4[0]<0), save the value if it comes from a cachable source, and attach the primitive block to the name
   if(unlikely((cachedlkp&(-cacheable))<0)){
    // point the nameref to the lookup result.  This prevents further changes to the lookup
    v->localuse.lu1.cachedref=stabent-JT(jt,sympv);  // convert symbol address back to index in case symbols are relocated
    stabent->flag|=LCACHED;  // protect the value from changes
    // set the flags in the nameref to what they are in the value.  This will allow compounds using this nameref (created in the parsing of later sentences)
    // to use the flags.  If we do PPPP, this will be too late
    v->flag=FAV(fs)->flag&(VIRS1+VIRS2+VJTFLGOK1+VJTFLGOK2+VASGSAFE);  // combining flags, do not require looking into id
    v->flag2=FAV(fs)->flag2&(VF2WILLOPEN1+VF2USESITEMCOUNT1+VF2WILLOPEN2W+VF2WILLOPEN2A+VF2USESITEMCOUNT2W+VF2USESITEMCOUNT2A);  // combining flags, do not require looking into id
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
     NAV(thisname)->cachedref=self; ra(self); ACSET(thisname,ACUC1);   // exp def is ALWAYS recursive usecount, so we raise self when we store to it.
        //  This wipes out bucket info in self, but that will not be needed since we have cached the lookup
    }
   }
  }
 }else{
  // here for pseudo-named function.  The actual name is in g, and the function itself is pointed to by h.  The verb is an anonymous explicit modifier that has received operands (but not arguments)
  // The name is defined, but it has the value before the modifier operands were given, so ignore fields in it except for the name
  jt->curname=thisname=v->fgh[1];  // get the original name
  explocale=0;  // flag no explicit locale
  fs=v->fgh[2];  // point to the actual executable
  ASSERT(fs!=0,EVVALUE); // make sure the name's value is given also
  ASSERT(TYPESEQ(AT(self),AT(fs)),EVDOMAIN);   // make sure its part of speech has not changed since the name was parsed
  // The pseudo-named function was created under debug mode.  If the same sequence had been parsed outside of debug, it would have been anonymous.  This has
  // implications: anonymous verbs do not push/pop the locale stack.  If bstkreqd is set, ALL functions will push the stack here.  That is bad, because
  // it means that a function that modifies the current locale behaves differently depending on whether debug is on or not.  We set a flag to indicate the case
  flgd0cp|=1;  // indicate pseudofunction
  stabent=0;  // no name lookup was performed
 }
#if NAMETRACK
 // bring out the name, locale, and script into easy-to-display name
 C trackinfo[256];  // will hold name followed by locale
 forcetomemory(&trackinfo);
 mvc(sizeof(trackinfo),trackinfo,1,iotavec-IOTAVECBEGIN+' ');  // clear name & locale
 UI wx=0, wlen;   // index/len we will write to
 wlen=AN(thisname); wlen=wlen+wx>sizeof(trackinfo)-3?sizeof(trackinfo)-3-wx:wlen; MC(trackinfo+wx,NAV(thisname)->s,wlen); wx+=wlen+1;  // copy in the full name
 A locnm=LOCNAME(jt->global);  // name of current global locale
 wlen=AN(locnm); wlen=wlen+wx>sizeof(trackinfo)-2?sizeof(trackinfo)-2-wx:wlen; MC(trackinfo+wx,NAV(locnm)->s,wlen); wx+=wlen+1;  // copy in the locale name
 if(stabent&&stabent->sn>=0){
  wlen=AN(AAV(JT(jt,slist))[stabent->sn]); wlen=wlen+wx>sizeof(trackinfo)-1?sizeof(trackinfo)-1-wx:wlen; MC(trackinfo+wx,CAV(AAV(JT(jt,slist))[stabent->sn]),wlen); wx+=wlen;  // copy in the locale name
 }
 trackinfo[wx]=0;  // null-terminate the info
#endif
 v=FAV(fs);  // repurpose v to point to the resolved verb block
#if !USECSTACK
 I d=v->fdep; if(!d)RE(d=fdep(fs));  // get stack depth of this function, for overrun prevention
 FDEPINC(d);  // verify sufficient stack space - NO ERRORS until FDEPDEC below
#endif
 STACKCHKOFL
 if(unlikely(explocale!=0)){  // there is a locative or implied locative
  // locative. switch to it, stacking the prev value.  If the switch is to the current local symbol table, that means 'switch to caller's environment'
  if(unlikely(explocale==jt->locsyms)){
   pushcallstack1d(CALLSTACKPUSHLOCALSYMS,jt->locsyms);  // save current locsyms
   jt->locsyms=(A)AM(jt->locsyms);  // get the local syms at the time u/v was assigned; make them current.  Leave GST unchanged in it
   explocale=AKGST(jt->locsyms);  // fetch global syms for the caller's environment, so we stack it next
  }
  pushcallstack1d(CALLSTACKPOPLOCALE,jt->global); jt->global=explocale;  // move to new implied locale.  DO NOT change locale in lt->locsyms.  It is set only by explicit action so that on a chain of locatives it stays unchanged
 }
 // ************** no errors till the stack has been popped
 AF actionfn=v->valencefns[flgd0cp>>3];  // index is 'is dyad'.  Load here to allow call address to settle
 w=flgd0cp&8?w:(A)fs;  // set up the bivalent argument with the new self, since fs may have been changed

 // Execute the name.  First check 4 flags at once to see if anything special is afoot: debug, pm, bstk, garbage collection
 if(likely(!(jt->uflags.ui4))) {
  // No special processing. Just run the entity
  // We have to raise the usecount, in case the name is deleted while running.  But that will be very rare.  Plus, we know that the executable type is recursive and non-inplaceable.
  // So, all we have to do is increment the usecount.  If it's a PERMANENT symbol no harm will be done, since we decrement below
  // CODING NOTE: after considerable trial and error I found this ordering, whose purpose is to start the load of the indirect branch address as early as
  // possible before the branch.  Check the generated code on any change of compiler.
  if(unlikely(!(flgd0cp&2)))ACINCR(fs);  // protect the entity ONLY if not cached.  If it is cached it will never be truly deleted
  // Recursion through $: does not go higher than the name it was defined in.  We make this happen by pushing the name onto the $: stack
  A s=jt->parserstackframe.sf; jt->parserstackframe.sf=fs; z=(*actionfn)((J)(((REPSGN(SGNIF(v->flag,(flgd0cp>>3)+VJTFLGOK1X)))|~JTFLAGMSK)&(I)jtinplace),a,w,fs); jt->parserstackframe.sf=s;  // keep all flags in jtinplace
  // Undo the protection.  If, most unusually, the usecount goes to 0, back up and do the full recursive decrement
  if(unlikely(!(flgd0cp&2))){ACDECR(fs); if(unlikely(AC(fs)<=0)){ACINCR(fs); fa(fs);}}
 } else {
  // Extra processing is required.  Check each option individually
  if(jt->uflags.us.cx.cx_c.pmctr)pmrecord(thisname,jt->global?LOCNAME(jt->global):0,-1L,flgd0cp&8?VAL2:VAL1);  // Record the call to the name, if perf monitoring on
  // If we are required to insert a marker for each call, do so (if it hasn't been done already).  But not for pseudo-named functions
  if(!(flgd0cp&1) && jt->uflags.us.uq.uq_c.bstkreqd && callstackx==jt->callstacknext){pushcallstack1d(CALLSTACKPOPLOCALE,jt->global);}  //  If cocurrent is about, make every call visible
  if(jt->uflags.us.cx.cx_c.db&&!(jt->glock||VLOCK&v->flag)&&jt->recurstate<RECSTATEPROMPT){  // The verb is locked if it is marked as locked, or if the script is locked; if recursive JDo, can't enter debug suspension so ignore debug
   z=dbunquote(flgd0cp&8?a:0,flgd0cp&8?w:a,fs,flgd0cp&3?0:stabent);  // if debugging, go do that.  save last sym lookup as debug parm if it is valid (not cached or pseudoname)
  }else{
   if(unlikely(!(flgd0cp&2)))ACINCR(fs);  // protect the entity if not cached
   A s=jt->parserstackframe.sf; jt->parserstackframe.sf=fs; z=(*actionfn)((J)(((REPSGN(SGNIF(v->flag,(flgd0cp>>3)+VJTFLGOK1X)))|~JTFLAGMSK)&(I)jtinplace),a,w,fs); jt->parserstackframe.sf=s;
  if(unlikely(!(flgd0cp&2))){ACDECR(fs); if(unlikely(AC(fs)<=0)){ACINCR(fs); fa(fs);}}
  }
  if(jt->uflags.us.cx.cx_c.pmctr)pmrecord(thisname,jt->global?LOCNAME(jt->global):0,-2L,flgd0cp&8?VAL2:VAL1);  // record the return from call
  if(jt->uflags.us.uq.uq_c.spfreeneeded)spfree();   // if garbage collection required, do it
 }
#if !USECSTACK
 FDEPDEC(d);
#endif

 // Now pop the stack.  Execution may have added entries, but our stack frame always starts in the same place.
 // We may add entries to the end of the caller's stack frame
 jt->curname=savname;  // restore the executing name
 if(unlikely(callstackx!=jt->callstacknext)){  // normal case, with no stack, bypasses all this
  // There are stack entries.  Process them
  if(likely(((jt->callstack[callstackx].type^CALLSTACKPOPLOCALE) | ((callstackx+1)^jt->callstacknext))==0)) {   // jt->callstack[callstackx].type==CALLSTACKPOPLOCALE && callstackx+1==jt->callstacknext
   // The only thing on the stack is a simple POP.  Do the pop.  This & the previous case account for almost all the calls here
   SYMSETGLOBAL(jt->locsyms,jt->callstack[callstackx].value);  // restore global locale
   jt->callstacknext=(I4)callstackx;  // restore stackpointer for caller
  }else{
   // Locales were changed or deleted.  Process the stack fully
   // Find the locale to return to.  This will be the locale of the POP, or jt->global unchanged if there is a POPFROM or there is no POP.
   // There can never be more than one POP.
   I fromfound=0; A earlyloc=0; I i;
   i=jt->callstacknext;  // back to front
   do{
    --i;
    fromfound|=jt->callstack[i].type&CALLSTACKPOPFROM&&!(jt->callstack[callstackx].type&CALLSTACKPUSHLOCALSYMS);  // remember if FROM seen - but if we are processing u/v, always revert global, ignoring any hanging requests
    if(jt->callstack[i].type&(CALLSTACKPOPFROM|CALLSTACKPOPLOCALE|CALLSTACKPOPLOCALEFIRST)){
     earlyloc=jt->callstack[i].value;  // remember earliest POP[FROM]
     // When we remove the earliest POPFROM, we can go back to processing names without requiring stacking the return locale
     if(jt->callstack[i].type&CALLSTACKPOPLOCALEFIRST){jt->uflags.us.uq.uq_c.bstkreqd = 0;}
    }else if(jt->callstack[i].type&CALLSTACKPUSHLOCALSYMS)SYMSETLOCAL((A)jt->callstack[i].value);  // restore locsyms if we stacked it, and restore possibly-changed global value therein
   }while(i!=callstackx);
   // if we encountered u./v., we have now restored the previous local symbols so that it is OK to restore the globals into it
   if(earlyloc&&!fromfound){SYMSETGLOBAL(jt->locsyms,earlyloc);} // If there is a POP to do, do it
   // Delete the deletable locales.  If we encounter the (possibly new) current locale, remember that fact and don't delete it.
   I delcurr=0;  // set if we have to delete jt->global
   i=jt->callstacknext;  // back to front
   do{
    --i;
    if(jt->callstack[i].type&CALLSTACKDELETE){
     if(jt->callstack[i].value==jt->global)delcurr=1;else locdestroy(jt->callstack[i].value);  // destroy or mark for later
    }
   }while(i!=callstackx);
   jt->callstacknext=(I4)callstackx;  // restore stackpointer for caller.  The following pushes are onto the caller's stack
   // NOTE: if there is no higher executing name, these pushes will never get popped.  That would correspond to trying to delete the locale that is running at the console,
   // or typing (cocurrent 'a') into the console, which would leave a POP on the stack, never to be executed because there is no higher function to return to base locale.
   // There is no way to detect this, because names that don't change locales don't leave a trace, and there is no guarantee that the function-call stack will
   // be at 0 when the last name returns, because the name might have been called from the middle of a tacit expression that already had a function-call depth when the
   // name was called.
   // So, we reset the name-stack pointer whenever we call from console level (jt->uflags.us.uq.uq_c.bstkreqd too)

   // If there is a POPFROM, we have to make sure it is undone when the caller returns.  If the caller has a POP already, we can leave it alone; otherwise we have to add one.
   // To make sure we don't overflow the stack because of a sequence of cocurrents, we use jt->uflags.us.uq.uq_c.bstkreqd to indicate that a POPFROM is on the stack and in that case
   // we ensure that there is a POP for every name (but don't create new ones for every cocurrent).  Thus, if jt->uflags.us.cx.cx_c.pmctrbstk is not set, we set it for future calls, and put a POPFIRST onto the caller's stack.
   // When that is found, jt->uflags.us.uq.uq_c.bstkreqd will be reset
   if(fromfound){
    if(!(jt->uflags.us.uq.uq_c.bstkreqd)){pushcallstack1(CALLSTACKPOPLOCALEFIRST,earlyloc); jt->uflags.us.uq.uq_c.bstkreqd=1;}
   }
   // If the current locale was deletable, push an entry to that effect in the caller's stack.  It will be deleted when it becomes un-current (if ever: if
   // the locale is still in use back to console level, it will not be deleted.  Invariant: if a locale is marked for destruction, it appears
   // nowhere earlier on the call stack
   if(delcurr)pushcallstack1(CALLSTACKCHANGELOCALE|CALLSTACKDELETE,jt->global);
  }
 }
 // ************** errors OK now
 R z;
}


// The monad calls the bivalent case with (w,self,self) so that the inputs can pass through to the executed function
static DF1(jtunquote1){R unquote(w,self,self);}  // This just transfers to jtunquote.  It passes jt, with inplacing bits, unmodified

// return ref to adv/conj/verb whose name is a and whose symbol-table entry is w
// if the value is a noun, we just return the value; otherwise we create a 'name~' block
// and return that; the name will be resolved when the name~ is executed.
// If the name is undefined, return a reference to [: (a verb that always fails)
A jtnamerefacv(J jt, A a, L* w){A y;V*v;
 y=w?w->val:ds(CCAP);  // If there is a slot, get the value; if not, treat as [: (verb that creates error)
 if(!y||NOUN&AT(y))R y;  // return if error or it's a noun
 // This reference might escape into another context, either (1) by becoming part of a
 // non-noun result; (2) being assigned to a global name; (3) being passed into an explicit modifier: so we clear the bucket info if we ra() the reference
 v=FAV(y);
 // We cannot be guaranteed that the definition in place when a reference is created is the same value that is there when the reference
 // is used.  Thus, we can't guarantee inplaceability by copying INPLACE bits from f to the result, and we just set INPLACE for everything
 // and let unquote use the up-to-date value.
 // ASGSAFE has a similar problem, and that's more serious, because unquote is too late to stop the inplacing.  We try to ameliorate the
 // problem by making [: unsafe.
 A z=fdef(0,CTILDE,AT(y), jtunquote1,jtunquote, a,0L,0L, (v->flag&VASGSAFE)+(VJTFLGOK1|VJTFLGOK2), v->mr,lrv(v),rrv(v));  // create value of 'name~', with correct rank, part of speech, and safe/inplace bits
 RZ(z);
 // if the nameref is cachable, either because the name is cachable or name caching is enabled now, enable caching in lI4
 // If the nameref is cached, we will fill in the flags in the reference after we first resolve the name
 FAV(z)->localuse.lu1.cachedref=(NAV(a)->flag&NMCACHED || (jt->namecaching && !(NAV(a)->flag&(NMILOC|NMDOT|NMIMPLOC))))<<SYMNONPERMX;  // 0x80.. to enable caching, otherwise 0
 R z;
}

// return reference to the name given in w, used when moving from queue to stack
// For a noun, the reference points to the data, and has rank/shape info
// For other types, we build a function ref to 'name~', and fill in the type, rank, and a pointer to the name;
//  the name will be dereferenced when the function is executed
A jtnameref(J jt,A w,A locsyms){
 ARGCHK1(w);
 R namerefacv(w,syrd(w,locsyms));  // get the symbol-table slot for the name (don't store the locale-name); return its 'value'
}    /* argument assumed to be a NAME */

// Adverb 4!:8 create looked-up cacheable reference to (possibly boxed) literal name a
// The name must be defined.  It supplies the type and rank of the reference.  We require the name to be defined so that
// there will not be a circular reference if a name in a numbered locale is a reference to the same name
F1(jtcreatecachedref){
 A nm; RZ(nm=onm(w)); // create name from arg
 ASSERT(!(NAV(nm)->flag&(NMILOC|NMDOT|NMIMPLOC)),EVDOMAIN) // if special name or indirect locative, error
 L *sym=syrd(nm,JT(jt,emptylocale));  // look up name, but not in local symbols.  We start with the current locale (?? should start with the path?)
 ASSERT(sym!=0,EVVALUE);  // return if error or name not defined
 A z=sym->val; if(unlikely(AT(z)&NOUN))R z;  // if name is a noun, return its value
 RZ(z=fdef(0,CTILDE,AT(z), jtunquote1,jtunquote, nm,0L,0L, (z->flag&VASGSAFE)+(VJTFLGOK1|VJTFLGOK2), FAV(z)->mr,lrv(FAV(z)),rrv(FAV(z))));// create reference
 FAV(z)->localuse.lu1.cachedref=sym-JT(jt,sympv);  // convert symbol address back to index in case symbols are relocated
 sym->flag|=LCACHED;  // protect the value from changes.  We do not chain back from the name
 RETF(z);
}

// Result has type ':' but goes to unquote.  We mark a pseudo-named entity by having f=0, g=name, h=actual entity to execute
F2(jtnamerefop){V*v;
 ARGCHK2(a,w);
 v=FAV(w);
 R fdef(0,CCOLON,VERB,  jtunquote1,jtunquote, 0L,a,w, VXOPCALL|v->flag, v->mr,lrv(v),rrv(v));
}    

/* namerefop() is used by explicit defined operators when: */
/* - debug is on                                           */
/* - operator arguments have been supplied                 */
/* - function arguments have not yet been supplied         */
// w is an anonymous entity that we want to give the name a to for debug purposes


// u./v.
// We process this as 'u'~ where the name is flagged as NMIMPLOC
// Bivalent: called with (a,w,self) or (w,self).  We treat as dyad but turn it into monad if input w is not a noun
DF2(jtimplocref){
 self=AT(w)&NOUN?self:w;
 self=JT(jt,implocref)[FAV(self)->id&1];
 w=AT(w)&NOUN?w:self;
 R unquote(a,w,self); // call as (w,self,self) or (a,w,self)
}
