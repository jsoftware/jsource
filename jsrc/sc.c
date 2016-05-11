/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Symbol Table: Function Call (unquote)                                   */

#include "j.h"


static DF2(jtunquote){A aa,fs,g,ll,oldn,oln,z;B lk;I d,i;L*e;V*v;
 RE(0);
 JATTN;
 v=VAV(self);
 aa=v->f; RE(e=syrd(aa,&g)); 
 fs=v->h?v->h:e?e->val:0;  /* see namerefop() re v->h */
 oldn=jt->curname; jt->curname=aa;
 oln =jt->curlocn; jt->curlocn=ll=g?LOCNAME(g):0;
 ASSERT(fs,EVVALUE); 
 ASSERT(AT(self)==AT(fs),EVDOMAIN);
 d=fdep(fs);
 FDEPINC(d); ASSERT(jt->fcalln>=jt->fcalli,EVSTACK);
 if(0<jt->pmctr)pmrecord(aa,ll,-1L,a?VAL2:VAL1);
 lk=jt->glock||VLOCK&VAV(fs)->flag;
 i=++jt->fcalli; 
 jt->fcallg[i].sw0=jt->stswitched; jt->fcallg[i].og=jt->global; 
 jt->fcallg[i].flag=0; jt->stswitched=0; jt->fcallg[i].g=jt->global=g;
 if(jt->db&&!lk)z=dbunquote(a,w,fs);
 else{ra(fs); z=a?dfs2(a,w,fs):dfs1(w,fs); fa(fs);}  /* beware redefs down the stack */
 if(!jt->stswitched)jt->global=jt->fcallg[i].og;
 jt->stswitched=jt->fcallg[i].sw0;
 if(jt->fcallg[i].flag)locdestroy(i);
 jt->fcallg[i].g=jt->fcallg[i].og=0; jt->stswitched=0; 
 FDEPDEC(d); --jt->fcalli;
 if(0<jt->pmctr)pmrecord(aa,ll,-2L,a?VAL2:VAL1);
 jt->curlocn=oln;
 jt->curname=oldn;
 spfree();
 R z;
}

static DF1(jtunquote1){R unquote(0L,w,self);}

// return ref to adv/conj/verb whose name is a and whose symbol-table entry is w
// if the value is a noun, we just return the value; otherwise we create a 'name~' block
// and return that; the name will be resolved when the name~ is executed.
// If the name is undefined, return a reference to [: (a verb that always fails)
A jtnamerefacv(J jt, A a, L* w){A y;V*v;
 y=w?w->val:ds(CCAP);  // If there is a slot, get the value; if not, treat as [: (verb that creates error)
 if(!y||NOUN&AT(y))R y;  // return if error or it's a noun
 v=VAV(y); 
 R fdef(CTILDE,AT(y), jtunquote1,jtunquote, a,0L,0L, 0L, v->mr,v->lr,v->rr);  // return value of 'name~', with correct rank and part of speech
}


// return reference to the name given in w, used when moving from queue to stack
// For a noun, the reference points to the data, and has rank/shape info
// For other types, we build a function ref to 'name~', and fill in the type, rank, and a pointer to the name;
//  the name will be dereferenced when the function is executed
F1(jtnameref){
 RZ(w);
 R namerefacv(w,syrd(w,0L));  // get the symbol-table slot for the name (don't store the locale-name); return its 'value'
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
