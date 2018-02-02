/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Symbol Table: Function Call (unquote)                                   */

#include "j.h"


static DF2(jtunquote){A aa,fs,g,ll,oldn,oln,z;B lk;I d,i;L*e;V*v;
 F2PREFIP;  // We understand inplacing.  We check inplaceability of the called function.
 RE(0);
 JATTN;
 v=VAV(self);
// obsolete aa=v->f; RE(e=syrd(aa,&g));
 A jtg = jt->global;  // fetch current locale
 aa=v->f; RZ(g=sybaseloc(aa));   // if the name is a locative, get the explicit locale.  0 if erroneous locale, 1 if non-locative
 RE(e=syrdfromloc(aa,g));   // finish looking up name, which can be undefined (0 return) or error (0 return with error set)
 if((I)g&1)g=jtg;  // if not locative, default to current locale
 fs=v->h?v->h:e?e->val:0;  /* see namerefop() re v->h */  // fs is the routine to call; 0 if name undefined
 oldn=jt->curname; jt->curname=aa;
 oln =jt->curlocn; jt->curlocn=ll=g?LOCNAME(g):0;  // should get rid of this
 ASSERT(fs,EVVALUE); 
 ASSERT(TYPESEQ(AT(self),AT(fs)),EVDOMAIN);
 V *fv=VAV(fs);  // point to V struct in fs
 RE(d=fdep(fs));
 ASSERT(jt->fcalln > jt->fcalli, EVSTACK);  // We will increment fcalli before use; 1+fcalln elements are allocated, so advancing to number fcalln is the limit
 if(0<jt->pmctr)pmrecord(aa,ll,-1L,a?VAL2:VAL1);
 lk=jt->glock||VLOCK&fv->flag;
 i=++jt->fcalli; FDEPINC(d);   // No ASSERTs from here till the FDEPDEC below
 jt->fcallg[i].sw0=jt->stswitched; jt->fcallg[i].og=jtg;   // save previous locale
// obsolete  jt->fcallg[i].flag=0; jt->stswitched=0; jt->fcallg[i].g=jt->global=g;
 jt->fcallg[i].flag=0; jt->stswitched=0; jt->fcallg[i].g=jt->global=g;
 if(jt->db&&!lk){jt->cursymb=e; z=dbunquote(a,w,fs);}  // if debugging, go do that.  save last sym lookup as debug parm
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
 if(0<jt->pmctr)pmrecord(aa,ll,-2L,a?VAL2:VAL1);
 jt->curlocn=oln;
 jt->curname=oldn;
 if(jt->spfreeneeded)spfree();
 R z;
}

static DF1(jtunquote1){R unquote(0L,w,self);}  // This just transfers to jtunquote.  It passes jt, with inplacing bits, unmodified

// return ref to adv/conj/verb whose name is a and whose symbol-table entry is w
// if the value is a noun, we just return the value; otherwise we create a 'name~' block
// and return that; the name will be resolved when the name~ is executed.
// If the name is undefined, return a reference to [: (a verb that always fails)
A jtnamerefacv(J jt, A a, L* w){A y;V*v;
 y=w?w->val:ds(CCAP);  // If there is a slot, get the value; if not, treat as [: (verb that creates error)
 if(!y||NOUN&AT(y))R y;  // return if error or it's a noun
 // We are about to create a reference to a name.  Since this reference might escape into another context, either (1) by becoming part of a
 // non-noun result; (2) being assigned to a global name; (3) being passed into an explicit modifier: we have to expunge any reference to local
 // buckets.
 NAV(a)->bucket = 0;  // Clear bucket info so we won't try to look up using local info.  kludge this modifies the original a; not so bad, since it's usually not local; but ugly
 v=VAV(y);
 // We cannot be guaranteed that the definition in place when a reference is created is the same value that is there when the reference
 // is used.  Thus, we can't guarantee inplaceability by copying INPLACE bits from f to the result, and we just set INPLACE for everything
 // and let unquote use the up-to-date value.
 // ASGSAFE has a similar problem, and that's more serious, because unquote is too late to stop the inplacing.  We try to ameliorate the
 // problem by making [: unsafe.
 R fdef(CTILDE,AT(y), jtunquote1,jtunquote, a,0L,0L, (v->flag&VASGSAFE)+(VINPLACEOK1|VINPLACEOK2), v->mr,v->lr,v->rr);  // return value of 'name~', with correct rank, part of speech, and safe/inplace bits
}


// return reference to the name given in w, used when moving from queue to stack
// For a noun, the reference points to the data, and has rank/shape info
// For other types, we build a function ref to 'name~', and fill in the type, rank, and a pointer to the name;
//  the name will be dereferenced when the function is executed
F1(jtnameref){
 RZ(w);
 R namerefacv(w,syrd(w));  // get the symbol-table slot for the name (don't store the locale-name); return its 'value'
}    /* argument assumed to be a NAME */

F2(jtnamerefop){V*v;
 RZ(a&&w);
 v=VAV(w);
 R fdef(CCOLON,VERB,  jtunquote1,jtunquote, a,0L,w, VXOPCALL|v->flag, v->mr,v->lr,v->rr);
}    

/* namerefop() is used by explicit defined operators when: */
/* - debug is on                                           */
/* - operator arguments have been supplied                 */
/* - function arguments have not yet been supplied         */
