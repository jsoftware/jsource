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
DF2(jtunquote){A z;
 F2PREFIP;  // We understand inplacing.  We check inplaceability of the called function.
 RE(0);
 JATTN;
 V *v=FAV(self);  // V block for this V/A/C reference
 I callstackx=jt->callstacknext; // Remember where our stack frame starts.  We may add an entry; execution may add more
 A thisname=v->fgh[0]; A fs; A explocale; L *stabent;// the A block for the name of the function (holding an NM) - unless it's a pseudo-name   fs is the 'named' function itself  explocale=explicit locale if any stabent=symbol-table entry if any
 A savname=jt->curname;  // we stack the executing name
 if(thisname){
  jt->curname=thisname;  // set failing name before we have value errors
  // normal path for named functions
  if(AM(self)==jt->modifiercounter&&v->localuse.lvp[0]){
   // The most recent lookup is still valid, and it is nonzero.  Use it
   fs=v->localuse.lvp[0];
   explocale=0;  // flag no explicit locale
   stabent=(L*)fs;  // set stabent to NONZERO to indicate not a pseudofunction
  }else{
   NM* thisnameinfo=NAV(thisname);  // the NM block for the current name
   if(!(thisnameinfo->flag&(NMLOC|NMILOC|NMIMPLOC))) {  // simple name, and not u./v.
    explocale=0;  // flag no explicit locale
    if(!(stabent = probelocal(thisname)))stabent=syrd1(thisnameinfo->m,thisnameinfo->s,thisnameinfo->hash,jt->global);  // Try local, then look up the name starting in jt->global
   } else {  // locative or u/v
    if(!(thisnameinfo->flag&NMIMPLOC)){  // locative
     RZ(explocale=sybaseloc(thisname));  //  get the explicit locale.  0 if erroneous locale
     stabent=syrd1(thisnameinfo->m,thisnameinfo->s,thisnameinfo->hash,explocale);  // Look up the name starting in the locale of the locative
    }else{  // u./v.  We have to look at the assigned name/value to know whether this is an implied locative (it usually is)
     if(stabent = probelocal(thisname)){
      // u/v, assigned by xdefn.  Implied locative.  Use switching to the local table as a flag for restoring the caller's environment
      explocale=jt->locsyms;  // We have to use this flag trick, rather than stacking the locales here, because errors after the stack is set could corrupt the stack
     }
    }
   }
   ASSERT(stabent,EVVALUE);  // name must be defined
   fs=stabent->val;  // fetch the value of the name
   ASSERT(fs,EVVALUE); // make sure the name's value is given also
   // Remember the resolved value and the current modifiercounter, UNLESS the name does not permit remembering the lookup
   if(v->localuse.lvp[0]){v->localuse.lvp[0]=fs; AM(self)=jt->modifiercounter;}
   ASSERT(TYPESEQ(AT(self),AT(fs)),EVDOMAIN);   // make sure its part of speech has not changed since the name was parsed
  }
 }else{
  // here for pseudo-named function.  The actual name is in g, and the function itself is pointed to by h.  The verb is an anonymous explicit modifier that has received operands (but not arguments)
  // The name is defined, but it has the value before the modifier operands were given, so ignore fields in it except for the name
  jt->curname=thisname=v->fgh[1];  // get the original name
  explocale=0;  // flag no explicit locale
  fs=v->fgh[2];  // point to the actual executable
  ASSERT(fs,EVVALUE); // make sure the name's value is given also
  ASSERT(TYPESEQ(AT(self),AT(fs)),EVDOMAIN);   // make sure its part of speech has not changed since the name was parsed
  // The pseudo-named function was created under debug mode.  If the same sequence had been parsed outside of debug, it would have been anonymous.  This has
  // implications: anonymous verbs do not push/pop the locale stack.  If PMCTRBSTKREQD is set, ALL functions will push the stack here.  That is bad, because
  // it means that a function that modifies the current locale behaves differently depending on whether debug is on or not.  We use stabent=0 to indicate that
  // the forced-push should be omitted.
  stabent=0;  // no symbol table for pseudo-names, since they aren't looked up
 }
 I dyadex = w!=self;   // if we were called with w,fs,fs, we are a monad.  Otherwise (a,w,fs) dyad
 v=FAV(fs);  // repurpose v to point to the resolved verb block
#if !USECSTACK
 I d=v->fdep; if(!d)RE(d=fdep(fs));  // get stack depth of this function, for overrun prevention
 FDEPINC(d);  // verify sufficient stack space - NO ERRORS until FDEPDEC below
#endif
 STACKCHKOFL
 if(explocale){  // there is a locative or implied locative
  // locative. switch to it, stacking the prev value.  If the switch is to the current local symbol table, that means 'switch to caller's environment'
  if(explocale==jt->locsyms){
   pushcallstack1d(CALLSTACKPUSHLOCALSYMS,jt->locsyms);  // save current locsyms
   jt->locsyms=(A)AM(jt->locsyms);  // get the local syms at the time u/v was assigned; make them current.  Leave GST unchanged in it
   explocale=AKGST(jt->locsyms);  // fetch global syms for the caller's environment, so we stack it next
  }
  pushcallstack1d(CALLSTACKPOPLOCALE,jt->global); jt->global=explocale;  // move to new implied locale.  DO NOT change locale it lt->locsyms.  It is set only by explicit action so that on a chain of locatives it stays unchanged
// obsolete   pushcallstack1d(CALLSTACKPOPLOCALE,jt->global); SYMSETGLOBAL(jt->locsyms,explocale);  // move to new implied locale
  ++jt->modifiercounter;  // invalidate any extant lookups of modifier names
 }
 // ************** no errors till the stack has been popped
 w=dyadex?w:(A)fs;  // set up the bivalent argument with the new self, since fs may have been changed

 // Execute the name.  First check 4 flags at once to see if anything special is afoot
 if(!(jt->uflags.ui4|(v->flag&VLOCK))) {
  // No special processing. Just run the entity
  // We have to raise the usecount, in case the name is deleted while running.  But that will be very rare.  Plus, we know that the executable type is recursive and non-inplaceable.
  // So, all we have to do is increment the usecount.  If it's a PERMANENT symbol no harm will be done, since we decrement below
  // CODING NOTE: after considerable trial and error I found this ordering, whose purpose is to start the load of the indirect branch address as early as
  // possible before the branch.  Check the generated code on any change of compiler.
  AF actionfn=v->valencefns[dyadex];
  ++AC(fs);  // protect the entity
  // Recursion through $: does not go higher than the name it was defined in.  We make this happen by pushing the name onto the $: stack
  A s=jt->sf; jt->sf=fs; z=(*actionfn)(SGNIF(v->flag,dyadex+VJTFLGOK1X)<0?jtinplace:jt,a,w,fs); jt->sf=s;  // keep all flags in jtinplace
  // Undo the protection.  If, most unusually, the usecount goes to 0, back up and do the full recursive decrement
  if(--AC(fs)<=0){++AC(fs); fa(fs);}
 } else {
  // Extra processing is required.  Check each option individually
  if(PMCTRBPMON&jt->uflags.us.uq.uq_c.pmctrbstk)pmrecord(thisname,jt->global?LOCNAME(jt->global):0,-1L,dyadex?VAL2:VAL1);  // Record the call to the name, if perf monitoring on
  // If we are required to insert a marker for each call, do so (if it hasn't been done already).  But not for pseudo-named functions
  if(stabent!=0 && jt->uflags.us.uq.uq_c.pmctrbstk&PMCTRBSTKREQD && callstackx==jt->callstacknext){pushcallstack1d(CALLSTACKPOPLOCALE,jt->global);}  //  If cocurrent is about, make every call visible
  if(jt->uflags.us.cx.cx_c.db&&!(jt->uflags.us.cx.cx_c.glock||VLOCK&v->flag)){  // The verb is locked if it is marked as locked, or if the script is locked
   jt->cursymb=stabent; z=dbunquote(dyadex?a:0,dyadex?w:a,fs);  // if debugging, go do that.  save last sym lookup as debug parm
  }else{
   ra(fs);  // should assert recursive usecount
   A s=jt->sf; jt->sf=fs; z=v->valencefns[dyadex]((v->flag>>dyadex)&VJTFLGOK1?jtinplace:jt,a,w,fs); jt->sf=s;
   fa(fs);
  }
  if(PMCTRBPMON&jt->uflags.us.uq.uq_c.pmctrbstk)pmrecord(thisname,jt->global?LOCNAME(jt->global):0,-2L,dyadex?VAL2:VAL1);  // record the return from call
  if(jt->uflags.us.uq.uq_c.spfreeneeded)spfree();   // if garbage collection required, do it
 }
#if !USECSTACK
 FDEPDEC(d);
#endif

 // Now pop the stack.  Execution may have added entries, but our stack frame always starts in the same place.
 // We may add entries to the end of the caller's stack frame
 jt->curname=savname;  // restore the executing name
 if(callstackx!=jt->callstacknext){  // normal case, with no stack, bypasses all this
  // There are stack entries.  Process them
  if(jt->callstack[callstackx].type==CALLSTACKPOPLOCALE && callstackx+1==jt->callstacknext) {
   // The only thing on the stack is a simple POP.  Do the pop.  This & the previous case account for almost all the calls here
// obsolete    jt->global=jt->callstack[callstackx].value;  // restore global locale
   SYMSETGLOBAL(jt->locsyms,jt->callstack[callstackx].value);  // restore global locale
   jt->callstacknext=(I4)callstackx;  // restore stackpointer for caller
   ++jt->modifiercounter;  // invalidate any extant lookups of modifier names
  } else {
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
     if(jt->callstack[i].type&CALLSTACKPOPLOCALEFIRST){jt->uflags.us.uq.uq_c.pmctrbstk &= ~PMCTRBSTKREQD;}
// obsolete     }else if(jt->callstack[i].type&CALLSTACKPUSHLOCALSYMS)jt->locsyms=jt->callstack[i].value;  // restore locsyms if we stacked it
    }else if(jt->callstack[i].type&CALLSTACKPUSHLOCALSYMS)SYMSETLOCAL((A)jt->callstack[i].value);  // restore locsyms if we stacked it, and restore possibly-changed global value therein
   }while(i!=callstackx);
   // if we encountered u./v., we have now restored the previous local symbols so that it is OK to restore the globals into it
// obsolete    if(earlyloc&&!fromfound){jt->global=earlyloc; ++jt->modifiercounter;} // If there is a POP to do, do it; invalidate any extant lookups of modifier names
   if(earlyloc&&!fromfound){SYMSETGLOBAL(jt->locsyms,earlyloc); ++jt->modifiercounter;} // If there is a POP to do, do it; invalidate any extant lookups of modifier names
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
   // So, we reset the name-stack pointer whenever we call from console level (jt->uflags.us.uq.uq_c.pmctrbstk too)

   // If there is a POPFROM, we have to make sure it is undone when the caller returns.  If the caller has a POP already, we can leave it alone; otherwise we have to add one.
   // To make sure we don't overflow the stack because of a sequence of cocurrents, we use jt->uflags.us.uq.uq_c.pmctrbstk to indicate that a POPFROM is on the stack and in that case
   // we ensure that there is a POP for every name (but don't create new ones for every cocurrent).  Thus, if jt->uflags.us.uq.uq_c.pmctrbstk is not set, we set it for future calls, and put a POPFIRST onto the caller's stack.
   // When that is found, jt->uflags.us.uq.uq_c.pmctrbstk will be reset
   if(fromfound){
    if(!(jt->uflags.us.uq.uq_c.pmctrbstk&PMCTRBSTKREQD)){pushcallstack1(CALLSTACKPOPLOCALEFIRST,earlyloc); jt->uflags.us.uq.uq_c.pmctrbstk |= PMCTRBSTKREQD;}
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
 // We are about to create a reference to a name.  Since this reference might escape into another context, either (1) by becoming part of a
 // non-noun result; (2) being assigned to a global name; (3) being passed into an explicit modifier: we have to expunge any reference to local
 // buckets.  Tolerable because local ACVs are rare
 NAV(a)->bucket = 0;  // Clear bucket info so we won't try to look up using local info.  kludge this modifies the original a; not so bad, since it's usually not local; but ugly
 v=FAV(y);
 // We cannot be guaranteed that the definition in place when a reference is created is the same value that is there when the reference
 // is used.  Thus, we can't guarantee inplaceability by copying INPLACE bits from f to the result, and we just set INPLACE for everything
 // and let unquote use the up-to-date value.
 // ASGSAFE has a similar problem, and that's more serious, because unquote is too late to stop the inplacing.  We try to ameliorate the
 // problem by making [: unsafe.
 A z=fdef(0,CTILDE,AT(y), jtunquote1,jtunquote, a,0L,0L, (v->flag&VASGSAFE)+(VJTFLGOK1|VJTFLGOK2), v->mr,lrv(v),rrv(v));  // return value of 'name~', with correct rank, part of speech, and safe/inplace bits
 RZ(z); 
 // To prevent having to look up the name every time it is executed, we will remember the address of the block (in localuse), AND the modifier counter at the time of the lookup (in AM).  We can't use
 // old lookups on locatives or canned names like xyuvmn, and we leave localuse 0 as a flag of that condition to help logic in unquote
 // If the original name was not defined (w==0), don't set a value so that it will be looked up again to produce value error
 if(w&&!(NAV(a)->flag&(NMLOC|NMILOC|NMDOT))){
  FAV(z)->localuse.lvp[0]=y; AM(z)=jt->modifiercounter;
 }
 R z;
}


// return reference to the name given in w, used when moving from queue to stack
// For a noun, the reference points to the data, and has rank/shape info
// For other types, we build a function ref to 'name~', and fill in the type, rank, and a pointer to the name;
//  the name will be dereferenced when the function is executed
F1(jtnameref){
 RZ(w);
 R namerefacv(w,syrd(w));  // get the symbol-table slot for the name (don't store the locale-name); return its 'value'
}    /* argument assumed to be a NAME */

// Create a pseudo-named entity.  a is the name, w is the actual entity
// Result will go to unquote.  We mark a pseudo-named entity by having f=0, g=name, h=actual entity to execute
F2(jtnamerefop){V*v;
 RZ(a&&w);
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
 self=jt->implocref[FAV(self)->id&1];
 w=AT(w)&NOUN?w:self;
 R unquote(a,w,self); // call as (w,self,self) or (a,w,self)
}