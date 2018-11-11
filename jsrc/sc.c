/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Symbol Table: Named Function Call (unquote)                                   */

#include "j.h"

#if 1
// obsolete // Return last name or errorname entry on the stack; 0 if none.  This will be the name used for error messages
// obsolete A jtcurname(J jt) {
// obsolete  DQ(jt->callstacknext-1, if(jt->callstack[i].type&(CALLSTACKNAME|CALLSTACKERRORNAME))R jt->callstack[i].value;)
// obsolete  R 0;
// obsolete }

// This function handles both valences: monad as (w,fs,fs), dyad as (a,w,fs)
//
// This routine called a 'named' function, which was created by name~ or the equivalent for a stacked verb.
// It also handles pseudo-named functions, which are anonymous entities that need to be given a temporary name
// when they are running under debug.  Pseudo-named functions are created by namerefop.  We need to run them here so they get the debug side-effects of having a name.
static DF2(jtunquote){A z;
 F2PREFIP;  // We understand inplacing.  We check inplaceability of the called function.
 RE(0);
 JATTN;
 V *v=FAV(self);  // V block for this V/A/C reference
 I callstackx=jt->callstacknext; // Remember where our stack frame starts.  We may add an entry; execution may add more
 A thisname=v->fgh[0]; A fs; A explocale; L *stabent;// the A block for the name of the function (holding an NM) - unless it's a pseudo-name   fs is the 'named' function itself  explocale=explicit locale if any stabent=symbol-table entry if any
 if(thisname){
  // normal path for named functions
  if(AM(self)==jt->modifiercounter&&v->localuse){
   // The most recent lookup is still valid, and it is nonzero.  Use it
   fs=v->localuse;
   explocale=0;  // flag no explicit locale
  }else{
   NM* thisnameinfo=NAV(thisname);  // the NM block for the current name
   if(!(thisnameinfo->flag&(NMLOC|NMILOC))) {  // simple name
    explocale=0;  // flag no explicit locale
    if(!(stabent = probelocal(thisname)))stabent=syrd1(thisnameinfo->m,thisnameinfo->s,thisnameinfo->hash,jt->global);  // Try local, then look up the name starting in jt->global
   } else {  // locative
    RZ(explocale=sybaseloc(thisname));  //  get the explicit locale .  0 if erroneous locale
    stabent=syrd1(thisnameinfo->m,thisnameinfo->s,thisnameinfo->hash,explocale);  // Look up the name starting in the locale of the locative
   }
   ASSERT(stabent,EVVALUE);  // name must be defined
   fs=stabent->val;  // fetch the value of the name
   ASSERT(fs,EVVALUE); // make sure the name's value is given also
   // Remember the resolved value and the current modifiercounter, UNLESS the name does not permit remembering the lookup
   if(v->localuse){v->localuse=fs; AM(self)=jt->modifiercounter;}
   ASSERT(TYPESEQ(AT(self),AT(fs)),EVDOMAIN);   // make sure its part of speech has not changed since the name was parsed
  }
 }else{
  // here for pseudo-named function.  The actual name is in g, and the function itself is pointed to by h.  The verb is an anonymous explicit modifier that has received operands (but not arguments)
  // The name is defined, but it has the value before the modifier operands were given, so ignore it except for the name
  stabent=0;  // no symbol table for pseudo-names, since they aren't looked up
  thisname=v->fgh[1];  // get the actual name
  explocale=0;  // flag no explicit locale
  fs=v->fgh[2];  // point to the actual executable
  ASSERT(fs,EVVALUE); // make sure the name's value is given also
  ASSERT(TYPESEQ(AT(self),AT(fs)),EVDOMAIN);   // make sure its part of speech has not changed since the name was parsed
 }
 A savname=jt->curname; jt->curname=thisname;  // stack the name of the previous executing entity, replace it with new
 I dyadex = w!=(A)self;   // if we were called with w,fs,fs, we are a monad.  Otherwise (a,w,fs) dyad
 v=FAV(fs);  // repurpose v to point to the resolved verb block
 I d=v->fdep; if(!d)RE(d=fdep(fs));  // get stack depth of this function, for overrun prevention
 if(explocale){ pushcallstack1(CALLSTACKPOPLOCALE,jt->global); jt->global=explocale;  ++jt->modifiercounter;}  // i if locative, switch to it, stacking the prev valuenvalidate any extant lookups of modifier names -
 // ************** no errors till the stack has been popped
 FDEPINC(d);  // scaf bug this has an ASSERT which can mess up the call stack
 w=dyadex?w:(A)fs;  // set up the bivalent argument with the new self

 // Execute the name.  First check 4 flags at once to see if anything special is afoot
 if(!(jt->uflags.ui4|(v->flag&VLOCK))) {
  // No special processing. Just run the entity
  // We have to raise the usecount, in case the name is deleted while running.  But that will be very rare.  Plus, we know that the executable type is recursive and non-inplaceable.
  // So, all we have to do is increment the usecount.  If it's a PERMANENT symbol no harm will be done, since we decrement below
  // CODING NOTE: after considerable trial and error I found this ordering, whose purpose is to start the load of the indirect branch address as early as
  // possible before the branch.  Check the generated code on any change of compiler.
  AF actionfn=v->valencefns[dyadex];
  ++AC(fs);  // protect the entity
// obsolete  ra(fs);  // protect against reassignment while executing.  Usecount will always be recursive; could ACINCR   scaf should assert recursive, then ACINCR
// obsolete    if(a){if(!(v->flag&VINPLACEOK2))jtinplace=jt; z=dfs2ip(a,w,fs);}else{if(!(v->flag&VINPLACEOK1))jtinplace=jt; z=dfs1ip(w,fs);}
  // Recursion through $: does not go higher than the name it was defined in.  We make this happen by pushing the name onto the $: stack
  A s=jt->sf; jt->sf=fs; z=(*actionfn)((v->flag>>dyadex)&VINPLACEOK1?jtinplace:jt,a,w,fs); jt->sf=s;  // keep all flags in jtinplace
  // Undo the protection.  If, most unusually, the usecount goes to 0, back up and do the full recursive decrement
  if(--AC(fs)<=0){++AC(fs); fa(fs);}
 } else {
  // Extra processing is required.  Check each option individually
  if(0<jt->uflags.us.uq.uq_c.pmctrb)pmrecord(thisname,jt->global?LOCNAME(jt->global):0,-1L,dyadex?VAL1:VAL2);  // Record the call to the name, if perf monitoring on
  if(jt->uflags.us.cx.cx_c.db&&!(jt->uflags.us.cx.cx_c.glock||VLOCK&v->flag)){  // The verb is locked if it is marked as locked, or if the script is locked
   jt->cursymb=stabent; z=dbunquote(dyadex?a:0,dyadex?w:a,fs);  // if debugging, go do that.  save last sym lookup as debug parm
  }else{
   ra(fs);  // should assert recursive usecount
// obsolete    if(a){if(!(v->flag&VINPLACEOK2))jtinplace=jt; z=dfs2ip(a,w,fs);}else{if(!(v->flag&VINPLACEOK1))jtinplace=jt; z=dfs1ip(w,fs);}  // scaf make faster
   A s=jt->sf; jt->sf=fs; z=v->valencefns[dyadex]((v->flag>>dyadex)&VINPLACEOK1?jtinplace:jt,a,w,fs); jt->sf=s;
   fa(fs);
  }
  if(0<jt->uflags.us.uq.uq_c.pmctrb)pmrecord(thisname,jt->global?LOCNAME(jt->global):0,-2L,dyadex?VAL1:VAL2);  // record the return from call
  if(jt->uflags.us.uq.uq_c.spfreeneeded)spfree();   // if garbage collection required, do it
 }
 FDEPDEC(d);

 // Now pop the stack.  Execution may have added entries, but our stack frame always starts in the same place.
 // We may add entries to the end of the caller's stack frame
 jt->curname=savname;  // restore the executing name
 if(callstackx!=jt->callstacknext){  // normal case, with no stack, bypasses all this
  // There are stack entries.  Process them
  if(callstackx+1==jt->callstacknext && jt->callstack[callstackx].type==CALLSTACKPOPLOCALE) {
   // The only thing on the stack is a simple POP.  Do the pop.  This & the previous case account for almost all the calls here
   jt->global=jt->callstack[callstackx].value;  // restore global locale
   jt->callstacknext=(I4)callstackx;  // restore stackpointer for caller
   ++jt->modifiercounter;  // invalidate any extant lookups of modifier names
  } else {
   // Locales were changed or deleted.  Process the stack fully
   // Find the locale to return to.  This will be the locale of the earliest POP, or jt->global unchanged if there is a POPFROM or there is no POP.
   I fromfound=0; A earlyloc=0; I i;
   i=jt->callstacknext;  // back to front
   do{
    --i;
    fromfound|=jt->callstack[i].type&CALLSTACKPOPFROM;  // remember if FROM seen
    if(jt->callstack[i].type&(CALLSTACKPOPFROM|CALLSTACKPOPLOCALE))earlyloc=jt->callstack[i].value;  // remember earliest POP[FROM]
   }while(i!=callstackx);
   if(earlyloc&&!fromfound){jt->global=earlyloc; ++jt->modifiercounter;} // If there is a POP to do, do it; invalidate any extant lookups of modifier names
   // Delete the deletable locales.  If we encounter the current locale, remember that fact and don't delete it.
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
   // So, we reset the name-stack pointer whenever we call from console level.

   // If there is a POPFROM, add a POP in the caller's stack, to the earliest POP[FROM] (which will be the locale in effect when this name started)
   if(fromfound)pushcallstack1(CALLSTACKPOPLOCALE,earlyloc);
   // If the current locale was deletable, push an entry to that effect in the caller's stack.  It will be deleted when it becomes un-current (if ever: if
   // the locale is still in use back to console level, it will not be deleted.  Invariant: if a locale is marked for destruction, it appears
   // nowhere earlier on the call stack
   if(delcurr)pushcallstack1(CALLSTACKCHANGELOCALE|CALLSTACKDELETE,jt->global);
  }
 }
 // ************** errors OK now
 R z;
}

#else

where is call stack reset?
// obsolete aa=v->fgh[0]; RE(e=syrd(aa,&g));
 oldn=jt->curname; jt->curname=aa;
 oln =jt->curlocn; jt->curlocn=ll=g?LOCNAME(g):0;  // should get rid of this
 ASSERT(jt->fcalln > jt->fcalli, EVSTACK);  // We will increment fcalli before use; 1+fcalln elements are allocated, so advancing to number fcalln is the limit
 i=++jt->fcalli; FDEPINC(d);   // No ASSERTs from here till the FDEPDEC below
 jt->fcallg[i].sw0=jt->stswitched; jt->fcallg[i].og=jtg;   // save previous locale
// obsolete  jt->fcallg[i].flag=0; jt->stswitched=0; jt->fcallg[i].g=jt->global=g;
 jt->fcallg[i].flag=0; jt->stswitched=0; jt->fcallg[i].g=jt->global=g;
 // Execute.  ra() to protect against deleting the name while it is running.
 // This will be fast because we know any name has a recursive usecount before it is assigned
 if(!jt->stswitched)jt->global=jt->fcallg[i].og;  // do this better.  Remove stswitched?  og not needed elsewhere - just stack it here
 jt->stswitched=jt->fcallg[i].sw0;
 if(jt->fcallg[i].flag)locdestroy(i);
 jt->fcallg[i].g=jt->fcallg[i].og=0; jt->stswitched=0; // no need for this?
 --jt->fcalli;  // ASSERT OK now
 jt->curlocn=oln;
 jt->curname=oldn;


static DF2(jtunquote){A aa,fs,g,ll,oldn,oln,z;B lk;I d,i;L*e;V*v;
 F2PREFIP;  // We understand inplacing.  We check inplaceability of the called function.
 RE(0);
 JATTN;
 v=FAV(self);
// obsolete aa=v->fgh[0]; RE(e=syrd(aa,&g));
 A jtg = jt->global;  // fetch current locale
 aa=v->fgh[0]; RZ(g=sybaseloc(aa));   // if the name is a locative, get the explicit locale.  0 if erroneous locale, 1 if non-locative
 RE(e=syrdfromloc(aa,g));   // finish looking up name, which can be undefined (0 return) or error (0 return with error set)
 if((I)g&1)g=jtg;  // if not locative, default to current locale
 fs=v->fgh[2]?v->fgh[2]:e?e->val:0;  /* see namerefop() re v->fgh[2] */  // fs is the routine to call; 0 if name undefined
 oldn=jt->curname; jt->curname=aa;
 oln =jt->curlocn; jt->curlocn=ll=g?LOCNAME(g):0;  // should get rid of this
 ASSERT(fs,EVVALUE); 
 ASSERT(TYPESEQ(AT(self),AT(fs)),EVDOMAIN);
 V *fv=FAV(fs);  // point to V struct in fs
 RE(d=fdep(fs));
 ASSERT(jt->fcalln > jt->fcalli, EVSTACK);  // We will increment fcalli before use; 1+fcalln elements are allocated, so advancing to number fcalln is the limit
 if(0<jt->uflags.us.uq.uq_c.pmctrb)pmrecord(aa,ll,-1L,a?VAL2:VAL1);
 lk=jt->uflags.us.cx.cx_c.glock||VLOCK&fv->flag;
 i=++jt->fcalli; FDEPINC(d);   // No ASSERTs from here till the FDEPDEC below
 jt->fcallg[i].sw0=jt->stswitched; jt->fcallg[i].og=jtg;   // save previous locale
// obsolete  jt->fcallg[i].flag=0; jt->stswitched=0; jt->fcallg[i].g=jt->global=g;
 jt->fcallg[i].flag=0; jt->stswitched=0; jt->fcallg[i].g=jt->global=g;
 if(jt->uflags.us.cx.cx_c.db&&!lk){jt->cursymb=e; z=dbunquote(a,w,fs);}  // if debugging, go do that.  save last sym lookup as debug parm
 // Execute.  ra() to protect against deleting the name while it is running.
 // This will be fast because we know any name has a recursive usecount before it is assigned
 else{ra(fs);  // should assert recursive usecount
  if(a){if(!(fv->flag&VINPLACEOK2))jtinplace=jt; z=dfs2ip(a,w,fs);}else{if(!(fv->flag&VINPLACEOK1))jtinplace=jt; z=dfs1ip(w,fs);}
  fa(fs);
 }
 if(!jt->stswitched)jt->global=jt->fcallg[i].og;  // do this better.  Remove stswitched?  og not needed elsewhere - just stack it here
 jt->stswitched=jt->fcallg[i].sw0;
 if(jt->fcallg[i].flag)locdestroy(i);
 jt->fcallg[i].g=jt->fcallg[i].og=0; jt->stswitched=0; // no need for this?
 FDEPDEC(d); --jt->fcalli;  // ASSERT OK now
 if(0<jt->uflags.us.uq.uq_c.pmctrb)pmrecord(aa,ll,-2L,a?VAL2:VAL1);
 jt->curlocn=oln;
 jt->curname=oldn;
 if(jt->uflags.us.uq.uq_c.spfreeneeded)spfree();
 R z;
}

#endif

// The monad calls the bivalent case with (w,self,self) so that the inputs can pass through to the executed function
static DF1(jtunquote1){R unquote(w,(A)self,self);}  // This just transfers to jtunquote.  It passes jt, with inplacing bits, unmodified

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
 A z=fdef(0,CTILDE,AT(y), jtunquote1,jtunquote, a,0L,0L, (v->flag&VASGSAFE)+(VINPLACEOK1|VINPLACEOK2), v->mr,v->lr,v->rr);  // return value of 'name~', with correct rank, part of speech, and safe/inplace bits
 RZ(z); 
 // To prevent having to look up the name every time it is executed, we will remember the address of the block (in localuse), AND the modifier counter at the time of the lookup (in AM).  We can't use
 // old lookups on locatives or canned names like xyuvmn, and we leave localuse 0 as a flag of that condition.
 // If the original name was not defined (w==0), don't set a value so that it will be looked up again to produce value error
 if(w&&!(NAV(a)->flag&(NMLOC|NMILOC|NMDOT))){
  FAV(z)->localuse=y; AM(z)=jt->modifiercounter;
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
 R fdef(0,CCOLON,VERB,  jtunquote1,jtunquote, 0L,a,w, VXOPCALL|v->flag, v->mr,v->lr,v->rr);
}    

/* namerefop() is used by explicit defined operators when: */
/* - debug is on                                           */
/* - operator arguments have been supplied                 */
/* - function arguments have not yet been supplied         */
// w is an anonymous entity that we want to give the name a to for debug purposes
