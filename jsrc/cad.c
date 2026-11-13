/* Copyright (c) 1990-2026, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Derivative (automatic differentiation)                    */

#include "j.h"

// An outer D. differentiates what the inner D. computes, the derivative code's own arithmetic included.
// When nested, the derivative code computes on values with verbs passed to jtadop, which hands them to the outer level.
// The macros below redefine J's internal calls (e.g. tymes, plus, reshape) to do this: when not nested they call J
// directly at no cost. When nested they call jtadop.
static A jtadop(J,A,A,A);
static A jtadca(J,A);
static A jtadcvt(J,I,A);
static A jtadope(J,A);
#define AO1(f,y) jtadop1(jt,(f),(y))
#define AO2(f,x,y) jtadop2(jt,(f),(x),(y))
// 0 (NULL) argument is an error from the expression that computed it: return 0 passing the error on.
static A jtadop1(J jt,A f,A y){if(!y)R 0; R jtadop(jt,f,0,y);}
static A jtadop2(J jt,A f,A x,A y){if(!x||!y)R 0; R jtadop(jt,f,x,y);}
#undef tymes
#define tymes(x,y) (NESTED?AO2(ds(CSTAR),(x),(y)):jtatomic2(jt,(x),(y),ds(CSTAR)))
#undef plus
#define plus(x,y) (NESTED?AO2(ds(CPLUS),(x),(y)):jtatomic2(jt,(x),(y),ds(CPLUS)))
#undef minus
#define minus(x,y) (NESTED?AO2(ds(CMINUS),(x),(y)):jtatomic2(jt,(x),(y),ds(CMINUS)))
#undef divide
#define divide(x,y) (NESTED?AO2(ds(CDIV),(x),(y)):jtatomic2(jt,(x),(y),ds(CDIV)))
#undef recip
#define recip(x) (NESTED?AO1(ds(CDIV),(x)):jtrecip(jt,(x),ds(CDIV)))
#undef negate
#define negate(x) (NESTED?AO1(ds(CMINUS),(x)):jtnegate(jt,(x),ds(CMINUS)))
#undef expn1
#define expn1(x) (NESTED?AO1(ds(CEXP),(x)):jtatomic1(jt,(x),ds(CEXP)))
#undef expn2
#define expn2(x,y) (NESTED?AO2(ds(CEXP),(x),(y)):jtatomic2(jt,(x),(y),ds(CEXP)))
#undef logar1
#define logar1(x) (NESTED?AO1(ds(CLOG),(x)):jtatomic1(jt,(x),ds(CLOG)))
#undef sqroot
#define sqroot(x) (NESTED?AO1(ds(CSQRT),(x)):jtatomic1(jt,(x),ds(CROOT)))
#undef mag
#define mag(x) (NESTED?AO1(ds(CSTILE),(x)):jtatomic1(jt,(x),ds(CSTILE)))
#undef signum
#define signum(x) (NESTED?AO1(ds(CSTAR),(x)):jtatomic1(jt,(x),ds(CSTAR)))
#undef reshape
#define reshape(x,y) (NESTED?AO2(ds(CDOLLAR),(x),ravel(y)):jtreshape(jt,(x),(y),0))
#undef from
#define from(x,y) (NESTED?AO2(ds(CFROM),(x),(y)):jtfrom(jt,(x),(y),ds(CFROM)))
#undef take
#define take(x,y) (NESTED?AO2(ds(CTAKE),(x),(y)):jttake(jt,(x),(y),0))
#undef drop
#define drop(x,y) (NESTED?AO2(ds(CDROP),(x),(y)):jtdrop(jt,(x),(y),0))
#undef cant1
#define cant1(x) (NESTED?AO1(ds(CCANT),(x)):jtcant1(jt,(x),ds(CCANT)))
#undef cant2
#define cant2(x,y) (NESTED?AO2(ds(CCANT),(x),(y)):jtcant2(jt,(x),(y)))
#undef head
#define head(x) (NESTED?AO1(ds(CHEAD),(x)):jthead(jt,(x),0))
#undef ravel
#define ravel(x) (NESTED?AO1(ds(CCOMMA),(x)):jtravel(jt,(x),0))
#undef ope
#define ope(x) jtadope(jt,(x))
#undef ca
#define ca(x) jtadca(jt,(x))
#undef cvt
#define cvt(t,x) jtadcvt(jt,(t),(x))

#define ADZ mark // the identically-zero tangent
#define ISZ(T) ((T)==ADZ)
#define TR(T) (AR(T)-1) // rank of the primal that tangent T belongs to

static I jtadfwd1(J,A,A,A,I,A*,A*);
static A jtadrec1(J,A,A);
static A jtadrec2(J,A,A,A);
typedef struct {A p,t;} ADVAL; // primal and tangent
static I jtadliftb1(J,B,I,A,ADVAL,I,ADVAL*);
static I jtadliftb2(J,B,I,A,ADVAL,I,ADVAL,I,ADVAL*);
static I jtadfwd2(J,A,A,A,A,A,I,A*,A*);
#define ADF1(f,P,T,pz,tz) RZ(jtadfwd1(jt,(f),(P),(T),k,(pz),(tz)))
#define ADF2(f,x,Tx,y,Ty,pz,tz) RZ(jtadfwd2(jt,(f),(x),(Tx),(y),(Ty),k,(pz),(tz)))

// ********************************************* names *********************************************
// A name inside u is a name reference ('name'~), which J looks up each time it runs it.
// D. needs the definition to choose a rule, so it looks the name up when its walk reaches it.
// A name whose value is an explicit definition is not walked but called through the name,
// so that J does its locale handling, and its sentences are traced.

// A primitive
#define ISPRIM(f) (FAV(f)->flag2&VF2PRIM)
// $:
#define ADISSELF(f) ((f)&&AT(f)&VERB&&ISPRIM(f)&&FAV(f)->id==CSELF)
// m@.v
#define ADISAGENDA(v) ((v)->id==CATDOT&&(v)->fgh[0]&&AT((v)->fgh[0])&BOX&&(v)->fgh[1]&&AT((v)->fgh[1])&VERB)
// A pseudoname: what a modifier called through a locative (adv_loc_) returns, the verb in fgh[2] run in the locale fgh[1]
#define ADPSEUDO(f) (FAV(f)->id==CCOLONE&&FAV(f)->flag2&VF2PSEUDONAME)
// An explicit definition (not a pseudoname)
#define ADEXPLV(g) (FAV(g)->id==CCOLONE&&!(FAV(g)->flag2&VF2PSEUDONAME))
// If f is u./v. then return the reference 'u.'~/'v.'~; otherwise f
#define ADIMPL(f) (FAV(f)->id==CUDOT||FAV(f)->id==CVDOT?JT(jt,implocref)[FAV(f)->id&1]:(f))
// True if the value g of a reference may be walked
#define ADPLAINV(g) (!(AFLAG(g)&AFRO)&&!jt->glock&&!(FAV(g)->flag&VLOCK))

// Is f a reference to another verb?
static B adisref(A f){R f&&AT(f)&VERB&&((FAV(f)->id==CTILDE&&FAV(f)->fgh[0]&&AT(FAV(f)->fgh[0])&NAME)||FAV(f)->id==CUDOT||FAV(f)->id==CVDOT||ADPSEUDO(f));}
// Take a verb f and return the value to walk, f itself, or 0 on error.
static A jtadres(J jt,A f){A x,nm;
 if(!adisref(f))R f;
 STACKCHKOFL // a cycle of names ends in a stack error
 if(ADPSEUDO(f)){ // pseudoname
  x=FAV(f)->fgh[2]; ASSERT(x!=0,EVVALUE); ASSERT(AT(x)&VERB,EVDOMAIN); R ADPLAINV(x)?x:f; // as unquote runs it (sc.c)
 }
 if(FAV(f)->flag2&VF2CACHEABLE&&FAV(f)->flag2&VF2CACHED&&(x=FAV(f)->localuse.lu1.cachedlkp)!=0){ // cached reference (4!:8)
  ASSERT(AT(x)&VERB,EVDOMAIN); R ADPLAINV(x)?x:f; // run the cached value
 }
 A r=ADIMPL(f); nm=FAV(r)->fgh[0];
 if(NAV(nm)->flag&NMIMPLOC){ // u. v.
  x=QCWORD(CLRNAMEDLOC(probex(NAV(nm)->m,NAV(nm)->s,SYMORIGIN,NAV(nm)->hash,jt->locsyms))); // look the name up in the current local symbols only
  ASSERTN(x!=0,EVVALUE,nm); ASSERTN(AT(x)&VERB,EVDOMAIN,nm);
  R ADPLAINV(x)?x:f;
 }
 // ordinary lookup (unquote in sc.c)
 if(!(NAV(nm)->flag&(NMLOC|NMILOC))&&!(AR(jt->locsyms)&ARHASACV)){
  if((x=jtsyrd1((J)((I)jt+NAV(nm)->m),NAV(nm)->s,NAV(nm)->hash,jt->global))!=0)x=SETNAMEDFAOWED(x);
 }else x=syrd(nm,jt->locsyms);
 ASSERTN(QCWORD(x)!=0,EVVALUE,nm);
 if(ISFAOWED(x))tpush(QCWORD(x)); x=QCWORD(x);
 ASSERTN(AT(x)&VERB,EVDOMAIN,nm); // the name's part of speech changed since the reference was made
 R ADPLAINV(x)?x:f;
}
typedef struct {
 A old; // locale before the switch
 A loc; // locale switched to
 A ols; // local symbol table before the switch
 B chg; // the switch went to a different locale that is not permanent
 B sw;  // switch was made
} ADLOC;
// Make the local symbols ls and their locale current, as unquote does for u. v. and name___n
static void jtadswitchls(J jt,ADLOC*s,A ls){A l=AKGST(ls);
 s->old=jt->global; s->ols=jt->locsyms; s->loc=l; s->chg=l!=jt->global&&!(LXAV0(l)[SYMLEXECCT]&EXECCTPERM); s->sw=1;
 jt->locsyms=ls; SYMSETGLOBAL(l); if(s->chg){ACVCACHECLEAR; INCREXECCT(l);} // Switch the current locals and locale.
}
// One step of resolution that also does what J's name call does for a locative or for u. v.
static A jtadresin(J jt,A f,ADLOC*s){A g,nm,l;
 s->sw=0; s->ols=0;
 RZ(g=jtadres(jt,f)); // One step of lookup.
 if(g==f||ADEXPLV(g))R g; // Nothing was resolved or an explicit definition (called through f which does the rest).
 if(ADPSEUDO(f)){l=FAV(f)->fgh[1]; // a pseudoname (what adv_loc_ returns): the locale to run its verb in is in fgh[1]
   if(!l||l==jt->global)R g; // no locale or already the current one: nothing to switch
   s->old=jt->global; s->loc=l; s->chg=!(LXAV0(l)[SYMLEXECCT]&EXECCTPERM); s->sw=1; SYMSETGLOBAL(l); // record the switch
   if(s->chg){ACVCACHECLEAR; INCREXECCT(l);} // a real change
   R g; // the verb to walk, now in its locale
  }
 nm=FAV(ADIMPL(f))->fgh[0];
 if(NAV(nm)->flag&NMIMPLOC){jtadswitchls(jt,s,(A)AM(jt->locsyms)); R g;} // u. v.
 if(!(NAV(nm)->flag&(NMLOC|NMILOC)))R g; // A plain name.
 if(FAV(f)->flag2&VF2CACHEABLE&&FAV(f)->flag2&VF2CACHED&&FAV(f)->localuse.lu1.cachedlkp&&NAV(nm)->flag&NMLOC)l=FAV(f)->localuse.lu0.cachedloc; // cached reference to name_loc_
 else RZ(l=sybaseloc(nm)); // The locale of a locative.
 if(AR(l)&ARLOCALTABLE){if(l!=jt->locsyms)jtadswitchls(jt,s,l); R g;} // name___n
 if(l==jt->global)R g;
 s->old=jt->global; s->loc=l; s->chg=!(LXAV0(l)[SYMLEXECCT]&EXECCTPERM); s->sw=1; // Switch the current locale.
 SYMSETGLOBAL(l); if(s->chg){ACVCACHECLEAR; INCREXECCT(l);}
 R g;
}
static void jtadlocexit(J jt,ADLOC*s){if(!s->sw)R; if(s->ols)jt->locsyms=s->ols; SYMSETGLOBAL(s->old); if(s->chg){ACVCACHECLEAR; DECREXECCT(s->loc);} s->sw=0;}
// Undo switches ls[0..nls-1] made by jtadfollow
static void jtadlocexitn(J jt,ADLOC*ls,I nls){
 DQ(nls, jtadlocexit(jt,&ls[i]);) // innermost first
}
// Quiet version of jtadresin (on any error the reference itself with nothing switched).
// Names are also left unresolved, and the predicates answer cautiously, near J's stack limit or past ADPBUDGET
// bytes of stack below the current trace: a cycle through a compound (f=:>:@g and g=:f) would otherwise let
// every step of the rules walk to the end of the stack.
#define ADPBUDGET 0x40000
static A jtadresinq(J jt,A f,ADLOC*s){A g; s->sw=0; s->ols=0;
 if(STACKPOS<jt->cstackmin+0x10000||(jt->adtrace&&STACKPOS+ADPBUDGET<(uintptr_t)jt->adtrace))R f;
 if(!(g=jtadresin(jt,f,s))){RESETERR; R f;}
 R g;
}
// Follow the chain of names from f (a=:b, b=:c, ...) to the first value that is not a name, each step in the locale
// J would use. Any length is followed; a cycle is detected and ends the walk at a reference.
// A slot of s is used only by a step that switches locale.
// *n is the number used to be undone. *full: walk stopped with all slots used and name unresolved
#define ADMAXLOC 16
static A jtadfollown(J jt,A f,ADLOC*s,I*n,I cap,B*full){A g=f,saved=f; I lam=0,pow=1;
 *n=0; *full=0;
 while(adisref(g)){
  if(*n==cap){*full=1; R g;}
  A h=jtadresinq(jt,g,&s[*n]);
  if(s[*n].sw)++*n; // keep slot only if locale was switched
  if(h==g)R g; // unresolved: the reference itself
  g=h;
  if(g==saved)R g; // back at an earlier value: a cycle
  if(++lam==pow){saved=g; pow<<=1; lam=0;}
 }
 R g;
}
static A jtadfollow(J jt,A f,ADLOC*s,I*n,B*full){R jtadfollown(jt,f,s,n,ADMAXLOC,full);}
// A predicate that sees through names (with the locales switched as J would, and switched back afterwards).
// A cycle, an undefined name, or more than ADMAXLOC locale switches leave a reference.
#define ADLOCWRAP(T,name) static T jt##name(J jt,A f){if(!adisref(f))R jt##name##b(jt,f); \
 ADLOC s[ADMAXLOC]; I n; B full; A g=jtadfollow(jt,f,s,&n,&full); \
 T r=jt##name##b(jt,g); DQ(n, jtadlocexit(jt,&s[i]);) R r;}
// The same for a predicate that recurses over the parts of f, with the answer c when the C stack is nearly used up.
// c is the cautious answer which leaves the work to the rules: they check the stack themselves and report the error
#ifdef NOSTACKCHK
#define ADSTACKLOW(c)
#else
#define ADSTACKLOW(c) if(unlikely(!(STACKPOS>=jt->cstackmin)))R c;
#endif
#define ADLOCWRAPS(T,name,c) static T jt##name(J jt,A f){ADSTACKLOW(c) if(!adisref(f))R jt##name##b(jt,f); \
 ADLOC s[ADMAXLOC]; I n; B full; A g=jtadfollow(jt,f,s,&n,&full); \
 T r=jt##name##b(jt,g); DQ(n, jtadlocexit(jt,&s[i]);) R r;}

#define adlinjoint2(f) jtadlinjoint2(jt,(f))
#define adlinbondm(f) jtadlinbondm(jt,(f))
#define adlinbondn(f) jtadlinbondn(jt,(f))
#define adlinear1(f) jtadlinear1(jt,(f))
#define adconst1(f) jtadconst1(jt,(f))
#define adconst2(f) jtadconst2(jt,(f))
#define adatomic1(f) jtadatomic1(jt,(f))
#define adatomic2(f) jtadatomic2(jt,(f))
#define adstructx(f) jtadstructx(jt,(f))
#define adstructy(f) jtadstructy(jt,(f))
#define adstructj(f) jtadstructj(jt,(f))
#define adstructm(f) jtadstructm(jt,(f))
#define adisplusslash(f) jtadisplusslash(jt,(f))
#define adrankm(f) jtadrankm(jt,(f))
#define adrankl(f) jtadrankl(jt,(f))
#define adrankr(f) jtadrankr(jt,(f))
#define adinnerid(f) jtadinnerid(jt,(f))

static B jtadlinjoint2(J,A);
static B jtadlinbondm(J,A);
static B jtadlinbondn(J,A);
static B jtadlinear1(J,A);
static B jtadconst1(J,A);
static B jtadconst2(J,A);
static B jtadatomic1(J,A);
static B jtadatomic2(J,A);
static B jtadstructx(J,A);
static B jtadstructy(J,A);
static B jtadstructj(J,A);
static B jtadstructm(J,A);
static I jtadrankm(J,A);
static I jtadrankl(J,A);
static I jtadrankr(J,A);
static C jtadinnerid(J,A);

// ************************************* data structures *****************************************
// Every D. sets up a trace for the time it runs with jt->adtrace pointing to it.
enum {ADKPRIM1, ADKPRIM2, ADKSTRUCT1, ADKASSEMBLE, ADKBOXMAP}; // kinds of tape entry
// Tape entry. z is always the entry's result. Other fields are slots filled according to the kind:
//  PRIM1     f verb   x argument                   y 0
//  PRIM2     f verb   x and y arguments
//  STRUCT1   f 0      x argument                   y its index map (atom numbers from 1, 0 for fill) or 0 (identity)
//  ASSEMBLE  f 0      x boxes of the cell results  y 0
//  BOXMAP    f 0      x leaves (list of boxes)     y index map into them
typedef struct {A f,x,y,z; I kind;} ADENT;
typedef struct ADTRACE {
 struct ADTRACE *prev; // the trace of enclosing level (D. whose u contains this one), 0 at the outermost
 I k; // directions
 A *keys, *tans; I n, cap; // hash table: traced arrays (by address) and their tangents
 ADENT *tape; I ntape, captape; // tape
 A zgiven; // forward replay of tape: the entry's recorded result, so the rule computes only the tangent
 A *bkeys; B *bvals; I bn, bcap; // the cache of jtadholds: does a box array hold a traced value
 B rev; // does this level record a tape?
 B fbfail; // verb with no rule was run and its result refused
 B refused; // nonce error is D.'s own refusal: try. and :: pass it on
} ADTRACE;
#define ADT ((ADTRACE*)jt->adtrace)
// D. refuses (e.g verb with no rule): a nonce error marked as D.'s own, so that try. and :: inside u do not take it for an error of J's
// (J would not fail there). The refusal ends the computation.
void jtadrefuse(J jt){if(ADT)ADT->refused=1; jsignal(EVNONCE);}
#define ADASSERT(b) {if(unlikely(!(b))){jtadrefuse(jt); R 0;}}
static void jtadrefusefinal(J jt){if(ADT){ADT->refused=1; ADT->fbfail=1;} jsignal(EVNONCE);} // the verb was run and its result refused (final)
// Is the current error such that try. and :: mustn't catch?
// D.'s own refusal, and also a stack error or a full workspace: D. needs more of both than J, so J's code may not have failed, and catching would
// differentiate catch. instead
B jtadrefusal(J jt){I e=jt->jerr; R jt->adtrace&&((e==EVNONCE&&ADT->refused)||e==EVSTACK||e==EVWSFULL);}

#define ADHASH(x,cap) ((I)(((UI)(x)>>4)*(UI)0x9E3779B97F4A7C15ULL>>20)&((cap)-1)) // Fibonacci hashing.
// Tangent of x or ADZ.
static A jtadtget(J jt,A x){ADTRACE*t=ADT; I h;
 if(!t||!t->n||!x)R ADZ;
 for(h=ADHASH(x,t->cap);t->keys[h];h=(h+1)&(t->cap-1))
  if(t->keys[h]==x)
   R t->tans[h];
 R ADZ;
}
#define adtget(x) jtadtget(jt,(x))
#define TRC(x) (!ISZ(adtget(x))) // x is traced: has a tangent (forward) or is a node (reverse)
#define ADNODE ds(CPLUS) // marker of a node in the table while recording (any permanent non-noun will do)
static A jtadtgetf(J jt,A z){A T=adtget(z); R T==ADNODE?ADZ:T;} // the tangent of z
// Rebuild the table with capacity cap; if sweep, drop entries whose key nothing but the table refers to
static B jtadtrehash(J jt,ADTRACE*t,I cap,B sweep){ // cap is new size, a power of 2. sweep says whether to drop dead entries.
 A*ok=t->keys,*ot=t->tans; I oc=t->cap,h; // ok, ot and oc keep the old keys, tangents and size.
 A*nk=calloc(cap,sizeof(A)),*nt=calloc(cap,sizeof(A)); // Two new zeroed arrays. Zero means an empty slot.
 if(!nk||!nt){free(nk); free(nt); R 0;}
 t->keys=nk; t->tans=nt; t->cap=cap; t->n=0;
 DO(oc, A x=ok[i];
  if(x){
   if(sweep&&AC(x)==ACUC1){fa(x); fa(ot[i]); // free dead entry
   }else{ // insertion with linear probing
    for(h=ADHASH(x,cap);nk[h];h=(h+1)&(cap-1));
    nk[h]=x; nt[h]=ot[i]; ++t->n;
   }
  }
 )
 free(ok); free(ot); R 1;
}
static I jtadtput(J jt,A x,A T){ADTRACE*t=ADT; I h;
 if(2*t->n>=t->cap){ // The table is half full or more.
  ASSERT(jtadtrehash(jt,t,t->cap,1),EVWSFULL); // Drop the entries whose key is held only by the trace (use count 1).
  if(4*t->n>=t->cap)ASSERT(jtadtrehash(jt,t,2*t->cap,0),EVWSFULL); // If the table is still a quarter full or more after the sweep, double it, without sweeping again.
 }
 for(h=ADHASH(x,t->cap);t->keys[h];h=(h+1)&(t->cap-1)); // Find the slot using linear probing.
 realizeifvirtual(T); // a virtual block may not be ra()d
 ra(x); ra(T); t->keys[h]=x; t->tans[h]=T; ++t->n;
 R 1;
}
// enter primal z with tangent T in the trace. Result is the block to use for z (a copy if z is already a key).
static A jtadtreg(J jt,A z,A T){A old;
 if(!z||!T)R 0; // an error computing the value or its tangent
 if(!ADT||ISZ(T)||!(AT(z)&NUMERIC))R z; // text and boxes are never traced
 if(AT(z)&(XNUM|RAT))RZ(z=cvt(FL,z)); // traced values are floating point
 realizeifvirtual(z); // Keys must not be virtual.
 old=adtget(z);
 if(old==T)R z;
 if(!ISZ(old)||ACISPERM(AC(z)))RZ(z=ca(z));
 RZ(jtadtput(jt,z,T)); // Enter the new key (the original z or the copy) with T.
 R z;
}
#define adtreg(z,T) jtadtreg(jt,(z),(T))
// slot of x in the table, or -1 (a slot number is only valid as long as the table isn't rebuilt).
static I jtadslot(J jt,A x){ADTRACE*t=ADT; I h;
 if(!x)R -1;
 for(h=ADHASH(x,t->cap);t->keys[h];h=(h+1)&(t->cap-1))
  if(t->keys[h]==x)R h;
 R -1;
}
// Release everything a trace owns.
static void jtadtfree(J jt,ADTRACE*t){
 DO(t->bcap, if(t->bkeys[i])fa(t->bkeys[i]);) free(t->bkeys); free(t->bvals);
 DO(t->ntape, ADENT*e=&t->tape[i]; if(e->f)fa(e->f); if(e->x)fa(e->x); if(e->y)fa(e->y); fa(e->z);)
 free(t->tape); t->tape=0; t->ntape=t->captape=0;
 DO(t->cap, if(t->keys[i]){fa(t->keys[i]); fa(t->tans[i]);})
 free(t->keys); free(t->tans);
}

// Rebuild the cache of jtadholds with cap slots. Entries held by nothing but the cache are dead and released, the rest kept.
#define ADBOXCACHEMIN 8 // smaller box arrays are walked each time
static B jtadbrehash(J jt,ADTRACE*t,I cap){
 A*ok=t->bkeys; B*ov=t->bvals; I oc=t->bcap,h;
 A*nk=calloc(cap,sizeof(A)); B*nv=calloc(cap,sizeof(B));
 if(!nk||!nv){free(nk); free(nv); R 0;}
 t->bkeys=nk; t->bvals=nv; t->bcap=cap; t->bn=0;
 DO(oc, A x=ok[i]; if(!x)continue;
  if(AC(x)==ACUC1){fa(x); continue;} // held by nothing else: dead
  for(h=ADHASH(x,cap);nk[h];h=(h+1)&(cap-1));
  nk[h]=x; nv[h]=ov[i]; ++t->bn;
 )
 free(ok); free(ov); R 1;
}
#define ADBOXE(x,i) (AT(x)&PYX?(x):AAV(x)[i]) // item i of the boxes x, for C(): a pyx (the result of u t. n) is an atomic box whose content is C() of the pyx itself
// Does x hold a traced value?
static B jtadholds(J jt,A x){ADTRACE*t=ADT; I h; B r=0;
 if(!x||!t||!t->n)R 0;
 STACKCHKOFLSUFF(R 1;) // boxes nested too deep to look through (cautious answer)
 if(!ISZ(adtget(x)))R 1; // If x itself has a tangent in the table, it is traced.
 if(!(AT(x)&BOX))R 0; // Not traced and not boxed.
 B keep=AN(x)>=ADBOXCACHEMIN&&!(AFLAG(x)&AFVIRTUAL)&&!ACISPERM(AC(x)); // should this array use the cache?
 if(keep&&t->bcap) // Look it up.
  for(h=ADHASH(x,t->bcap);t->bkeys[h];h=(h+1)&(t->bcap-1))
   if(t->bkeys[h]==x)
    R t->bvals[h];
 DO(AN(x), if(jtadholds(jt,CCOMMON(ADBOXE(x,i),,R 1))){r=1; break;}) // walk the contents
 if(keep){t=ADT; // store the answer
  if(2*t->bn>=t->bcap){
   if(!jtadbrehash(jt,t,t->bcap?t->bcap:64))R r;
   if(4*t->bn>=t->bcap&&!jtadbrehash(jt,t,2*t->bcap))R r;
  }
  for(h=ADHASH(x,t->bcap);t->bkeys[h];h=(h+1)&(t->bcap-1));
  ra(x); t->bkeys[h]=x; t->bvals[h]=r; ++t->bn;
 }
 R r;
}
static B jtadboxholds(J jt,A x){R x&&AT(x)&BOX&&jtadholds(jt,x);} // boxes holding a traced value
#define ADBOXHOLDS(x) jtadboxholds(jt,(x))
// Does verb f have a noun holding a traced value among its operands (e.g. y&* inside an explicit definition)?
static B jtadhastraced(J,A);
static B jtadhastracedb(J jt,A f){V*v;
 if(!f||!ADT||!ADT->n)R 0;
 if(AT(f)&NOUN)R jtadholds(jt,f); // operand that is a noun: traced or boxes holding a traced value
 if(!(AT(f)&VERB))R 0; // adverb or conjunction isn't a traced value
 if(adisref(f))R 1; // name left unresolved may hold a traced value
 if(ISPRIM(f))R 0;
 v=FAV(f);
 if(v->id==CCOLONE)R !(v->flag2&VF2PSEUDONAME); // An explicit definition may read a traced value (e.g. global) that its arguments do not show.
 if(ADISAGENDA(v)){
  DO(AN(v->fgh[0]), if(jtadhastraced(jt,AAV(v->fgh[0])[i]))R 1;)
  R jtadhastraced(jt,v->fgh[1]);
 }
 R jtadhastraced(jt,v->fgh[0])||jtadhastraced(jt,v->fgh[1])||(v->id==CFORK&&jtadhastraced(jt,v->fgh[2])); // Look at the operands recursively.
}
ADLOCWRAPS(B,adhastraced,1)
// Call an explicit definition with traced arguments. Its sentences propagate the tangents.
static I jtadcallx(J jt,A f,A x,A Tx,A y,A Ty,B dyad,A*pz,A*tz){A z;
 ADASSERT(ADT);
 if(dyad&&x==y&&Tx!=Ty)RZ(x=ca(x)); // x and y are the same array, with different derivatives
 RZ(y=adtreg(y,Ty)); // Register y with its tangent, so that sentences find it.
 if(dyad){
  RZ(x=adtreg(x,Tx)); RZ(df2(z,x,y,f)); // The same for x in the dyad. Then call the definition.
 }else{
  RZ(df1(z,y,f));
 }
 *pz=z; *tz=adtget(z); R 1;
}

// ******************************************* nesting *******************************************
// u D.1 inside u of another D. (second and higher derivatives). The traces form a stack.

static A jtadnorule(J,A,A,A,A);
static A jtadrealarg(J,A);
static A jtadrecord(J,I,A,A,A,A);
static DF1(jtdcap1);
static DF1(jtadscatv);
#define ISDCAP(f) (FAV(f)->id==CDCAPDOT&&FAV(f)->valencefns[0]==jtdcap1) // verb u D.n (id is shared with internal verbs)
#define DCAPU(f) (FAV(f)->fgh[2]?FAV(f)->fgh[2]:FAV(f)->fgh[0]) // verb it differentiates: u or u D.(n-1)
#define NESTED (ADT&&ADT->prev)
// does any level of D. outside the current one have a derivative for x?
static B jtadtracedup(J jt,A x){ADTRACE*t=ADT; B r=0;
 if(!t||!x)R 0;
 for(ADTRACE*s=t->prev;s&&!r;s=s->prev){
  jt->adtrace=s; A T=adtget(x); r=!ISZ(T);
 }
 jt->adtrace=t;
 R r;
}
#define UPTR(x) (NESTED&&jtadtracedup(jt,(x)))

// Is f a primitive that runs J text (e.g. ". 0!:n 6!:2)? We treat such a verb like an explicit definition.
static B jtadrunscode(J jt,A f){V*v; I m,n;
 if(!f||!(AT(f)&VERB))R 0;
 v=FAV(f); if(v->id==CEXEC&&ISPRIM(f))R 1; // ".
 if(v->id!=CIBEAM)R 0;
 m=v->localuse.lu1.foreignmn[0]; n=v->localuse.lu1.foreignmn[1]; // m!:n
 R m==0||(m==6&&n==2); // 0!:n or 6!:2
}
// apply a verb to its arguments while tracing
static A jtadapply(J jt,A f,A x,A y){ADTRACE*t=ADT; A Tx,Ty,z,tz; B dyad=x!=0; I ok;
 t->fbfail=0; t->refused=0; // a new execution
 Tx=dyad?adtget(x):ADZ; Ty=adtget(y);
 if(ISZ(Tx)&&ISZ(Ty)&&!jtadhastraced(jt,f)&&!ADBOXHOLDS(x)&&!ADBOXHOLDS(y)){ // nothing traced here: evaluate
  if(jtadrunscode(jt,f)){A z; // ". or a script: its sentences are traced at this level
   if(dyad)df2(z,x,y,f);
   else df1(z,y,f); R z;
  }
  R jtadop(jt,f,x,y);
 }
 if(t->rev){ // Reverse mode.
  if((z=dyad?jtadrec2(jt,f,x,y):jtadrec1(jt,f,y)))R z;
 }else{ // Forward mode.
  ok=dyad?jtadfwd2(jt,f,x,Tx,y,Ty,t->k,&z,&tz):jtadfwd1(jt,f,y,Ty,t->k,&z,&tz);
  if(ok){
   if(ISZ(tz)&&!ISZ(adtget(z)))RZ(z=ca(z)); // Copy the result with derivative zero that is a block already traced with a different tangent.
   R adtreg(z,tz);
  }
 }
 R 0; // error or D.'s refusal
}
static A jtadop(J jt,A f,A x,A y){A z; ADTRACE*t=ADT;
 if(likely(!t||!t->prev)){if(x)df2(z,x,y,f);else df1(z,y,f); R z;}
 C otr=jt->parserstackframe.adtracing; jt->parserstackframe.adtracing=1; // explicit code run by the outer level is traced there
 jt->adtrace=t->prev; z=jtadapply(jt,f,x,y); jt->adtrace=t;
 jt->parserstackframe.adtracing=otr;
 if(!z&&t->prev->refused&&jt->jerr==EVNONCE){t->refused=1; t->fbfail=1;} // refused by the outer level, so refuse it here too
 R z;
}
// c is a new block with value of z: give it z's derivatives at the levels outside the current one
static A jtadalias(J jt,A c,A z){ADTRACE*t=ADT; A r=c;
 if(!c||c==z||!NESTED)R c;
 for(ADTRACE*s=t->prev;s&&r;s=s->prev){A T; jt->adtrace=s; T=adtget(z);
  if(ISZ(T))continue;
  if(s->rev){
   A rc=jtadrecord(jt,ADKSTRUCT1,0,z,0,c);
   if(!rc)r=0;
   else if(rc!=c){jt->adtrace=t; ASSERT(0,EVSYSTEM);}
  }
  else if(!jtadtput(jt,c,T))r=0;
 }
 jt->adtrace=t; R r;
}
static A jtadca(J jt,A z){A c; RZ(c=jtca(jt,z)); R jtadalias(jt,c,z);}
static A jtadcvt(J jt,I t,A x){A z; if(x&&TYPESEQ(t,AT(x)))R x; RZ(z=jtcvt(jt,t,x)); R jtadalias(jt,z,x);} // convert x to type t.  x itself if it already has type t (sharing x is safe)
// > zb keeping derivatives
static A jtadfwdasm(J jt,A zb,A z);
static A jtadopenat(J jt,A zb){ADTRACE*t=ADT; A z,T; B any=0;
 if(!(AT(zb)&BOX))R jtope(jt,zb);
 DO(AN(zb), A Ti=adtget(C(AAV(zb)[i])); if(!ISZ(Ti))any=1;)
 RZ(z=jtadope(jt,zb));
 if(!any)R z;
 if(t->rev)R jtadrecord(jt,ADKASSEMBLE,0,zb,0,z);
 RZ(T=jtadfwdasm(jt,zb,z));
 R adtreg(z,T);
}
static A jtadope(J jt,A zb){ADTRACE*t=ADT; A z;
 if(likely(!t||!t->prev))R jtope(jt,zb);
 jt->adtrace=t->prev; z=jtadopenat(jt,zb); jt->adtrace=t;
 if(!z&&t->prev->refused&&jt->jerr==EVNONCE){t->refused=1; t->fbfail=1;}
 R z;
}

// ***************************************** tangent arithmetic **********************************
// zeros with shape of tangent of P
static A jtadzeros(J jt,I k,A P){A s; RZ(P); RZ(s=over(sc(k),shape(P))); R reshape(s,scf(0.0));}
#define adzeros(k,P) jtadzeros(jt,(k),(P))
// f"r T
static A jtadmap1(J jt,A f,A T,I r){A z,g; RZ(T); RZ(g=qq(f,sc(r))); R (z=AO1(g,T));}
#define admap1(f,T,r) jtadmap1(jt,(f),(T),(r))
// a f"(ra,rb) b
static A jtadmap2(J jt,A f,A a,I ra,A b,I rb){A z,g; RZ(a); RZ(b); RZ(g=qq(f,v2(ra,rb))); R (z=AO2(g,a,b));}
#define admap2(f,a,ra,b,rb) jtadmap2(jt,(f),(a),(ra),(b),(rb))
// factor F times tangent T
static A jtadscale(J jt,A F,A T){RZ(T); RZ(F); if(ISZ(T))R ADZ; R admap2(ds(CSTAR),F,AR(F),T,TR(T));}
#define adscale(F,T) jtadscale(jt,(F),(T))
static A jtadneg(J jt,A T){RZ(T); if(ISZ(T))R ADZ; R negate(T);}
#define adneg(T) jtadneg(jt,(T))
// extend the cells of T to the shape of P (cells of T have shape that is prefix of $P)
static A jtadexpand(J jt,A T,A P){A Z; RZ(T); RZ(P);
 if(ISZ(T)||(TR(T)==AR(P)))R T;
 RZ(Z=reshape(shape(P),scf(0.0)));
 R admap2(ds(CPLUS),T,TR(T),Z,AR(Z));
}
#define adexpand(T,P) jtadexpand(jt,(T),(P))
// T1+T2 as tangent of P
static A jtadsum(J jt,A T1,A T2,A P){A z; RZ(T1); RZ(T2); RZ(P);
 if(ISZ(T1))R adexpand(T2,P);
 if(ISZ(T2))R adexpand(T1,P);
 RZ(z=admap2(ds(CPLUS),T1,TR(T1),T2,TR(T2)));
 R adexpand(z,P);
}
#define adsum(T1,T2,P) jtadsum(jt,(T1),(T2),(P))
#define admat(T,P) (ISZ(T)?adzeros(k,(P)):(T)) // materialize zero tangent

// ********************************** verb classification *************************************

// monad atomic primitives (whole-array application is correct)
static B adscalarid1(C id){
 switch(id){
 case CPLUS: case CMINUS: case CSTAR: case CDIV: case CEXP: case CLOG: case CSQRT: case CSTARCO: case CPLUSCO:
 case CHALVE: case CSTILE: case CFLOOR: case CCEIL: case CNOT: case CLE: case CGE: case CCIRCLE: R 1; // + - * % ^ ^. %: *: +: -: | <. >. -. <: >: o.
 default: R 0;
 }
}
// dyads that are jointly linear in (x,y)
static B jtadlinjoint2b(J jt,A f){V*v;
 if(!f||!(AT(f)&VERB))R 0; v=FAV(f);
 if(v->id==CQQ)R AT(v->fgh[0])&VERB&&adlinjoint2(v->fgh[0]); // u"r is jointly linear if u is
 if(!ISPRIM(f))R 0;
 switch(v->id){
 case CPLUS: case CMINUS: case CCOMMA: case CCOMDOT: case CLAMIN: case CLEFT: case CRIGHT: R 1; // + - , ,. ,: [ ]
 default: R 0;
 }
}
// m&(+/ . *) or (+/ . *)&n
static B jtadbondmp(J jt,A f){V*v;
 if(!f||!(AT(f)&VERB))R 0; v=FAV(f);
 if(v->id!=CAMP&&v->id!=CAMPCO)R 0;
 R (AT(v->fgh[0])&NOUN&&adinnerid(v->fgh[1]))||(AT(v->fgh[1])&NOUN&&adinnerid(v->fgh[0]));
}
// m&v linear in its argument
static B jtadlinbondmb(J jt,A f){V*v;
 if(!f||!(AT(f)&VERB))R 0; v=FAV(f);
 if(adinnerid(f))R 1; // +/ .*  -/ .*
 if(!ISPRIM(f))R 0;
 switch(v->id){
 case CSTAR: case CFROM: case CTAKE: case CDROP: case CPOUND: case CDOLLAR: case CROT: case CCANT: case CBASE: case CRIGHT: R 1; // * { {. }. # $ |. |: #. ]
 default: R 0;
 }
}
// u&n linear in its argument
static B jtadlinbondnb(J jt,A f){V*v;
 if(!f||!(AT(f)&VERB))R 0; v=FAV(f);
 if(adinnerid(f))R 1; // +/ .*  -/ .*
 if(!ISPRIM(f))R 0;
 switch(v->id){
 case CSTAR: case CDIV: case CGRADE: case CDGRADE: case CLEFT: R 1; // * % /: \: [
 default: R 0;
 }
}
// monad f is linear (so its tangent map is f itself). The shortcut applies f to tangent at rank of P, which
// is only faster than the rules when J runs f on the entire array
static B jtadlinear1b(J jt,A f){V*v; A fs,gs,hs;
 if(!f||!(AT(f)&VERB))R 0; v=FAV(f); fs=v->fgh[0]; gs=v->fgh[1]; hs=v->fgh[2];
 if(v->valencefns[0]==jtadscatv)R 1; // internal verb which is linear by construction
 switch(v->id){
 case CPLUS: case CMINUS: case CPLUSCO: case CHALVE: case CCOMMA: case CCOMDOT: case CLAMIN: case CREV: case CCANT:
 case CHEAD: case CTAIL: case CBEHEAD: case CCTAIL: case CLEFT: case CRIGHT: case CGT: case CBASE: case CPDERIV: case CRAZE: case CCIRCLE: // + - +: -: , ,. ,: |. |: {. {: }. }: [ ] > p.. ; o.
  R !!ISPRIM(f);
 case CSLASH: R AT(fs)&VERB&&adlinjoint2(fs); // u/ folds with jointly linear dyad: +/ -/ ,/ ,./ ,:/ [/ ]/ and u"r of them
 case CBSLASH: case CBSDOT: case CQQ: R AT(fs)&VERB&&adlinear1(fs);
 case CPOWOP: { // u^:n, n must be finite and non-negative
  if(!(AT(fs)&VERB)||!(AT(gs)&NOUN)||AR(gs))R 0;
  if(AT(gs)&FL){D d=DAV(gs)[0]; if(!(d>=0&&d<inf&&d==jround(d)))R 0;}
  else if(AT(gs)&INT){if(IAV(gs)[0]<0)R 0;}
  else if(!(AT(gs)&B01))R 0;
  R adlinear1(fs);}
 case CUNDER: case CUNDCO: {A vi; // u&.v  u&.:v so u, v and J's inverse of v linear
  if(!(AT(fs)&VERB)||!(AT(gs)&VERB)||!adlinear1(fs)||!adlinear1(gs))R 0;
  WITHDEBUGOFF(vi=inv(gs);) if(!vi){RESETERR; R 0;} // quietly (v with no inverse is simply not linear here)
  R adlinear1(vi);}
 case CAT: case CATCO: R adlinear1(fs)&&adlinear1(gs);
 case CAMP: case CAMPCO:
  if(AT(fs)&NOUN)R adlinbondm(gs);
  if(AT(gs)&NOUN)R adlinbondn(fs);
  R adlinear1(fs)&&adlinear1(gs);
 case CFORK:
  if(!hs)R adlinear1(fs)&&adlinear1(gs); // capped fork [: f g is stored as f,g,0
  if(AT(fs)&NOUN)R adlinbondm(gs)&&adlinear1(hs); // (n g h) y = n g (h y)
  R adlinear1(fs)&&adlinear1(hs)&&adlinjoint2(gs);
 case CHOOK: R adlinjoint2(fs)&&adlinear1(gs);
 case CTILDE: R AT(fs)&VERB&&adlinjoint2(fs);
 default: R 0;
 }
}

// is f a verb whose result is always the same numeric atom, whatever the argument?
static B adconstatom(A f){V*v;
 while(f&&AT(f)&VERB&&FAV(f)->id==CQQ&&AT(FAV(f)->fgh[0])&VERB)f=FAV(f)->fgh[0]; // drop "r (it is safe because it changes only the frame never the values)
 if(!f||!(AT(f)&VERB))R 0; v=FAV(f);
 if(v->id==CFCONS)R 1;
 R v->id==CQQ&&AT(v->fgh[0])&(B01|INT|FL)&&!AR(v->fgh[0]);
}
static B jtadatomic1b(J jt,A f){V*v; A fs,gs,hs;
 if(!f||!(AT(f)&VERB))R 0; v=FAV(f); fs=v->fgh[0]; gs=v->fgh[1]; hs=v->fgh[2];
 if(v->flag&VISATOMIC1)R 1;
 if(ISPRIM(f))R adscalarid1(v->id);
 switch(v->id){
 case CAT: case CATCO: case CAMP: case CAMPCO:
  if((v->id==CAT||v->id==CATCO)&&(AT(fs)&NOUN||AT(gs)&NOUN))R 0; // u@n is constant, not elementwise
  if(AT(fs)&NOUN)R (!AR(fs)&&adatomic2(gs))||(AR(fs)<=1&&!(AT(fs)&BOX)&&ISPRIM(gs)&&FAV(gs)->id==CPOLY); // atom&scalar, or coefficients&p.
  if(AT(gs)&NOUN)R !AR(gs)&&adatomic2(fs);
  R adatomic1(fs)&&adatomic1(gs);
 case CFORK: if(!hs)R adatomic1(fs)&&adatomic1(gs);
  if(AT(fs)&NOUN)R !AR(fs)&&adatomic2(gs)&&adatomic1(hs);
  {B cf=adconstatom(fs),ch=adconstatom(hs); // one constant at most: the other gives the result its shape
   R adatomic2(gs)&&(cf?ch?0:adatomic1(hs):adatomic1(fs)&&(ch||adatomic1(hs)));}
 case CHOOK: R adatomic2(fs)&&(adatomic1(gs)||adconstatom(gs)); // y g h y: y gives the shape
 case CQQ: R AT(fs)&VERB&&adatomic1(fs);
 case CTILDE: R AT(fs)&VERB&&adatomic2(fs);
 case CDCAPDOT: R ISDCAP(f)&&v->mr==0&&adatomic1(DCAPU(f)); // the derivative of an elementwise verb is elementwise
 case CATDOT: // m@.v with elementwise verbs, v elementwise or constant atom
  if(!ADISAGENDA(v)||!(adatomic1(gs)||adconstatom(gs)))R 0;
  DO(AN(fs), if(!adatomic1(AAV(fs)[i]))R 0;)
  R 1;
 default: R 0;
 }
}
static B jtadatomic2b(J jt,A f){V*v; A fs,gs,hs;
 if(!f||!(AT(f)&VERB))R 0; v=FAV(f); fs=v->fgh[0]; gs=v->fgh[1]; hs=v->fgh[2];
 if(v->flag&VISATOMIC2)R 1;
 if(ISPRIM(f))switch(v->id){
  case CPLUS: case CMINUS: case CSTAR: case CDIV: case CEXP: case CLOG: case CROOT: case CMIN: case CMAX: case CSTILE: case CCIRCLE:
  case CEQ: case CNE: case CLT: case CLE: case CGE: case CGT: case CPLUSDOT: case CSTARDOT: R 1; // + - * % ^ ^. %: <. >. | o. = ~: < <: >: > +. *.
  default: R 0;
 }
 switch(v->id){
 case CAT: case CATCO: R AT(fs)&VERB&&AT(gs)&VERB&&adatomic1(fs)&&adatomic2(gs);
 case CAMP: case CAMPCO: R AT(fs)&VERB&&AT(gs)&VERB&&adatomic2(fs)&&adatomic1(gs);
 case CFORK: if(!hs)R adatomic1(fs)&&adatomic2(gs);
  if(AT(fs)&NOUN)R !AR(fs)&&adatomic2(gs)&&adatomic2(hs);
  {B cf=adconstatom(fs),ch=adconstatom(hs);
   R adatomic2(gs)&&(cf?ch?0:adatomic2(hs):adatomic2(fs)&&(ch||adatomic2(hs)));}
 case CHOOK: R adatomic2(fs)&&adatomic1(gs);
 case CQQ: R AT(fs)&VERB&&adatomic2(fs)&&v->localuse.lu1.srank[1]==0&&v->localuse.lu1.srank[2]==0; // u"r changes agreement unless r is 0
 case CTILDE: R AT(fs)&VERB&&adatomic2(fs);
 case CDCAPDOT: R ISDCAP(f)&&lr(f)==0&&rr(f)==0&&adatomic2(DCAPU(f));
 default: R 0;
 }
}

// monad f has a zero derivative and is cheap to run on a whole array.
// Used where the rank of f would otherwise make a cell loop: f"r runs once with a zero tangent.
static B jtadconst1b(J jt,A f){V*v;
 if(!f||!(AT(f)&VERB))R 0; v=FAV(f);
 switch(v->id){
 case CSTAR: case CFLOOR: case CCEIL: case CEQ: case CNE: case CPOUND: case CDOLLAR: case CIOTA: case CICO:
 case CGRADE: case CDGRADE: case CICAP: case CEPS: case CLDOT: // * <. >. = ~: # $ i. i: /: \: I. e. L.
  R !!ISPRIM(f);
 case CFCONS: R 1; // 3: etc.
 case CQQ: R AT(v->fgh[0])&NOUN?!(AT(v->fgh[0])&BOX):adconst1(v->fgh[0]); // m"r is the constant m (but boxed m might be a gerund)
 default: R 0;
 }
}
// the same for dyad f with a zero derivative in both arguments
static B jtadconst2b(J jt,A f){V*v;
 if(!f||!(AT(f)&VERB))R 0; v=FAV(f);
 switch(v->id){
 case CEQ: case CNE: case CLT: case CLE: case CGE: case CGT: case CEPS: case CEBAR:
 case CIOTA: case CICO: case CICAP: case CMATCH: // = ~: < <: >: > e. E. i. i: I. -:
  R !!ISPRIM(f);
 case CFCONS: R 1;
 case CQQ: R AT(v->fgh[0])&NOUN?!(AT(v->fgh[0])&BOX):adconst2(v->fgh[0]);
 default: R 0;
 }
}

// Is f  u/ . *  with u = + (matrix product) or - (whose monad is determinant)? Return the id of u, or 0
// the parts may be names (pl/ . tm with pl=: + and tm=: *)
static A jtadpartof(J jt,A g,ADLOC*s,I*n){B full; *n=0; if(!g||!(AT(g)&VERB))R 0; if(!adisref(g))R g; R jtadfollow(jt,g,s,n,&full);}
static C jtadinneridb(J jt,A f){V*v; A g,h,t; C c=0; ADLOC s1[ADMAXLOC],s2[ADMAXLOC],s3[ADMAXLOC]; I n1=0,n2=0,n3=0; v=FAV(f);
 if(v->id!=CDOT)R 0;
 g=jtadpartof(jt,v->fgh[0],s1,&n1); h=jtadpartof(jt,v->fgh[1],s2,&n2); // u/ and *
 if(g&&h&&FAV(g)->id==CSLASH&&ISPRIM(h)&&FAV(h)->id==CSTAR){
  t=jtadpartof(jt,FAV(g)->fgh[0],s3,&n3);   // u
  if(t&&ISPRIM(t)){c=FAV(t)->id; if(c!=CPLUS&&c!=CMINUS)c=0;}
  DQ(n3, jtadlocexit(jt,&s3[i]);)
 }
 DQ(n2, jtadlocexit(jt,&s2[i]);) DQ(n1, jtadlocexit(jt,&s1[i]);)
 R c;
}

// get verb's signed ranks, for the monad and for the left and right arguments of the dyad.
static I jtadrankmb(J jt,A f){V*v=FAV(f);
 if(ISDCAP(f))R jtadrankm(jt,DCAPU(f));
 R v->id==CQQ&&!(v->flag&VGERL)?(I)v->localuse.lu1.srank[0]:(I)v->mr; // gerund"n has rank _
}
static I jtadranklb(J jt,A f){V*v=FAV(f);
 if(ISDCAP(f))R jtadrankl(jt,DCAPU(f));
 R v->id==CQQ&&!(v->flag&VGERL)?(I)v->localuse.lu1.srank[1]:lr(f);
}
static I jtadrankrb(J jt,A f){V*v=FAV(f);
 if(ISDCAP(f))R jtadrankr(jt,DCAPU(f));
 R v->id==CQQ&&!(v->flag&VGERL)?(I)v->localuse.lu1.srank[2]:rr(f);
}

// classifying predicates see through names in the right locale
ADLOCWRAPS(B,adlinjoint2,0)
ADLOCWRAPS(B,adlinbondm,0)
ADLOCWRAPS(B,adlinbondn,0)
ADLOCWRAPS(B,adlinear1,0)
ADLOCWRAPS(B,adconst1,0)
ADLOCWRAPS(B,adconst2,0)
ADLOCWRAPS(B,adatomic1,0)
ADLOCWRAPS(B,adatomic2,0)
ADLOCWRAP(I,adrankm)
ADLOCWRAP(I,adrankl)
ADLOCWRAP(I,adrankr)
ADLOCWRAP(C,adinnerid)

// ***************************** cell loops *****************************
static A jtadmkdcap(J,A,A);
// f with its negative rank removed (v"r with r<0 becomes v"_), also inside u D.n
// so that the cells of an outer D. differentiate u D.n without re-splitting
static A jtadstripneg(J jt,A f,B dyad){V*fv;
 if(!f||!(AT(f)&VERB))R f;
 STACKCHKOFL
 if(ISDCAP(f)){A b=FAV(f)->fgh[0],g;
  RZ(g=jtadstripneg(jt,b,dyad));
  R g==b?f:jtadmkdcap(jt,g,FAV(f)->fgh[1]);
 }
 fv=FAV(f);
 if(fv->id!=CQQ||!(AT(fv->fgh[0])&VERB))R f;
 if(!(dyad?((I)fv->localuse.lu1.srank[1]<0||(I)fv->localuse.lu1.srank[2]<0):(I)fv->localuse.lu1.srank[0]<0))R f;
 R qq(fv->fgh[0],ainf); // v"_
}
// Return the same verb with names in u replaced by what they stand for.
static A jtadfollowdcap(J jt,A g,ADLOC*ls,I*nls){A u,b,c; B full; I n2;
 if(!g||!ISDCAP(g))R g;
 STACKCHKOFL
 u=b=FAV(g)->fgh[0];
 if(adisref(u)){
  b=jtadfollown(jt,u,ls+*nls,&n2,ADMAXLOC-*nls,&full); *nls+=n2;
  ASSERT(!full,EVLIMIT); // more than ADMAXLOC switches in all
 }
 RZ(c=jtadfollowdcap(jt,b,ls,nls)); // b may be v D.m in turn
 R c==u?g:jtadmkdcap(jt,c,FAV(g)->fgh[1]);
}
// Get the verb D. runs on each cell when f has a negative rank.
static A jtadnegv(J jt,A f,B dyad,ADLOC*ls,I*nls){A g,h; B full;
 *nls=0; g=jtadfollow(jt,f,ls,nls,&full);
 if(full){jtadlocexitn(jt,ls,*nls); *nls=0; ASSERT(0,EVLIMIT);} // more than ADMAXLOC locale switches
 if(!(g=jtadfollowdcap(jt,g,ls,nls))){jtadlocexitn(jt,ls,*nls); *nls=0; R 0;}
 if(!(h=jtadstripneg(jt,g,dyad))||h==g){jtadlocexitn(jt,ls,*nls); *nls=0; R h?f:0;}
 R h;
}
// cell loop when f is split at its own ranks: if one is negative the cells run f without it else f itself. T is type of call's result
#define ADNEGCELLS(T,f,neg,dy,call) {if(!(neg)){A g_=(f); R (call);} ADLOC ls_[ADMAXLOC]; I nls_; A g_; T rc_; RZ(g_=jtadnegv(jt,f,dy,ls_,&nls_)); rc_=(call); jtadlocexitn(jt,ls_,nls_); R rc_;}
// |x| for x<0 (IMIN has no negative: any length >= n is the same)
#define ADWINM(x) ((x)==IMIN?IMAX:-(x))
// how many cells x u\ y or x u\. y has when y has n items (the count is the same for infixes and outfixes)
static I adwincount(I x,I n){
 R x>0
  ?MAX(0,n-x+1) // overlapping infixes of length x
  :x<0
   ?(n?1+(n-1)/ADWINM(x):0) // non-overlapping infixes of length |x|
   :n+1; // n+1 empty infixes
}
// For cell i of x u\ y or x u\. y get where the infix starts (*s) and how many items it has (*l). The infix is items s ... s+l-1, the outfix is all the other items.
static void adwin(I x,I n,I i,I*s,I*l){
 if(x>0){*s=i; *l=x;
 }else if(x<0){I m=ADWINM(x); *s=i*m; *l=MIN(m,n-*s);
 }else{*s=i; *l=0;}
}
// item indices of cell i
static A jtadwincell(J jt,B out,I x,I n,I i){A z; I s,l,*zv;
 adwin(x,n,i,&s,&l);
 if(!out)R apv(l,s,1); // infix: s + i. l
 GATV0(z,INT,n-l,1); zv=IAV(z); // outfix: the rest
 DO(s, zv[i]=i;) DO(n-s-l, zv[s+i]=s+l+i;) R z;
}
// item indices of every cell (a table). 0 when the cells differ in length
// or when the table and W { y would hold more than ADWINMAX atoms.
#define ADWINMAX ((I)1<<24)
static A jtadwinidx(J jt,B out,I x,I n,I a){A W; I s,l,L,*wv;
 I c=adwincount(x,n); // number of cells
 if(x<0&&n%ADWINM(x))R 0; // with negative x that doesn't divide n, the last infix is shorter. This goes to cell loop.
 adwin(x,n,0,&s,&l); L=out?n-l:l;
 if(L&&c>ADWINMAX/L/MAX(a,1))R 0; // c*L*a <= ADWINMAX
 GATV0(W,INT,c*L,2); AS(W)[0]=c; AS(W)[1]=L; wv=IAV(W);
 DO(c,
  adwin(x,n,i,&s,&l);
  if(out){DO(s, *wv++=i;) DO(n-s-l, *wv++=s+l+i;)
  }else{DO(l, *wv++=s+i;)}
 )
 R W;
}

// Rank machinery is run over an array of cell indices; each call differentiates one cell and
// returns the boxed pair primal;tangent. The pairs are then opened with fill and the
// direction axis is moved back to the front.

// Monad. Differentiate one cell in forward mode. 0=cell i (P and T split into cells); 1=prefix i+1 (u\); 2=suffix i (u\.); 3=infix i (x u\ y); 4=outfix i (x u\. y). In 1-4 T is directions first.
static DF1(jtadcellm){DF12IP;
 V*v=FAV(self); A f=v->fgh[0],*dv=AAV(v->fgh[1]); A p,t,pz,tz;
 I k=IAV(C(dv[2]))[0], mode=IAV(C(dv[2]))[1], i=IAV(w)[0]; A P=C(dv[0]),T=C(dv[1]);
 B b=!!(AT(P)&BOX); t=ADZ; // boxes have no tangent (their contents carry it)
 switch(mode){
 case 0: RZ(p=from(w,P)); if(!b)RZ(t=from(w,T)); break;
 case 1: RZ(p=take(sc(i+1),P)); if(!b)RZ(t=admap2(ds(CTAKE),sc(i+1),0,T,TR(T))); break;
 case 2: RZ(p=drop(sc(i),P)); if(!b)RZ(t=admap2(ds(CDROP),sc(i),0,T,TR(T))); break;
 default: {A ix; RZ(ix=jtadwincell(jt,mode==4,IAV(C(dv[2]))[2],AS(P)[0],i));
  RZ(p=from(ix,P)); if(!b)RZ(t=admap2(ds(CFROM),ix,1,T,TR(T))); break;}
 }
 RZ(jtadfwd1(jt,f,p,t,k,&pz,&tz));
 RZ(tz=admat(tz,pz)); RZ(tz=adexpand(tz,pz));
 RZ(tz=cant2(num(0),tz));
 R jlink(pz,tz);
}
// Dyad.
static DF2(jtadcelld){DF12IP;V*v=FAV(self); A f=v->fgh[0],*dv=AAV(v->fgh[1]); A x,tx,y,ty,pz,tz;
 I *kv=IAV(C(dv[4])); I k=kv[0];
 RZ(x=from(a,C(dv[0]))); tx=ADZ; if(!(kv[1]&1))RZ(tx=from(a,C(dv[1])));
 RZ(y=from(w,C(dv[2]))); ty=ADZ; if(!(kv[1]&2))RZ(ty=from(w,C(dv[3])));
 RZ(jtadfwd2(jt,f,x,tx,y,ty,k,&pz,&tz));
 RZ(tz=admat(tz,pz)); RZ(tz=adexpand(tz,pz));
 RZ(tz=cant2(num(0),tz));
 R jlink(pz,tz);
}

// flatten the frame of value and of its tangent into a single list of cells, so that cell i and its tangent (all directions) can each be taken with i {
static I jtadsplit(J jt,A P,A T,I fr,I k,A*pr,A*tr){I n=1; A cs;
 DO(fr, n*=AS(P)[i];);
 RZ(cs=drop(sc(fr),shape(P)));
 RZ(*pr=reshape(over(sc(n),cs),P));
 if(ISZ(T)){*tr=mtv; R 1;}
 RZ(*tr=cant2(over(num(0),apv(AR(P)-fr,fr+1,1)),T));
 RZ(*tr=reshape(over(sc(n),over(sc(k),cs)),*tr));
 R 1;
}
// take the array of value;tangent pairs that cell loop returned and open all the values together as the result, and all the tangents together as the result's tangent.
static I jtadjoin(J jt,A z,A*pz,A*tz){A p,t;
 RZ(p=admap2(ds(CFROM),num(0),0,z,1)); RZ(*pz=ope(p));
 if(!(AT(*pz)&NUMERIC)){*tz=ADZ; R 1;} // text or boxes: no tangent
 RZ(t=admap2(ds(CFROM),num(1),0,z,1)); RZ(t=ope(t));
 RZ(*tz=cant2(apv(AR(t)-1,0,1),t)); // direction axis (last) to the front
 R 1;
}
// Compute f"r P and its tangent cell by cell.
static I jtadcells1(J jt,A f,I r,A P,A T,I k,A*pz,A*tz){A pr,tr,d,cv,ix,z,g; I fr=AR(P)-r,n=1;
 DO(fr, n*=AS(P)[i];);
 if(!n){RZ((z=AO1(qq(f,sc(r)),P))); *pz=z; *tz=ADZ; R 1;} // no cells: nothing to differentiate
 if(!(AT(P)&BOX))RZ(T=admat(T,P)); // boxes (holding traced values) have no tangent of their own
 RZ(jtadsplit(jt,P,T,fr,k,&pr,&tr));
 RZ(d=jlink(pr,jlink(tr,v2(k,0))));
 cv=fdef(0,CDCAPDOT,VERB,jtadcellm,jtvalenceerr,f,d,0,VFLAGNONE,RMAX,RMAX,RMAX);
 RZ(ix=reshape(take(sc(fr),shape(P)),apv(n,0,1)));
 RZ(g=qq(cv,num(0))); RZ(df1(z,ix,g));
 R jtadjoin(jt,z,pz,tz);
}
// the same for x f"(lr,rr) y
static I jtadcells2(J jt,A f,I lr,I rr,A x,A Tx,A y,A Ty,I k,A*pz,A*tz){A xr,txr,yr,tyr,d,cv,ix,iy,z,g; I fx=AR(x)-lr,fy=AR(y)-rr,nx=1,ny=1;
 DO(fx, nx*=AS(x)[i];); DO(fy, ny*=AS(y)[i];);
 if(!nx||!ny){ // no cells
  RZ((z=AO2(qq(f,v2(lr,rr)),x,y)));
  *pz=z; *tz=ADZ; R 1;
 }
 RZ(jtadsplit(jt,x,Tx,fx,k,&xr,&txr));
 RZ(jtadsplit(jt,y,Ty,fy,k,&yr,&tyr));
 RZ(d=jlink(xr,jlink(txr,jlink(yr,jlink(tyr,v2(k,(ISZ(Tx)?1:0)+(ISZ(Ty)?2:0)))))));
 cv=fdef(0,CDCAPDOT,VERB,jtvalenceerr,jtadcelld,f,d,0,VFLAGNONE,RMAX,RMAX,RMAX);
 RZ(ix=reshape(take(sc(fx),shape(x)),apv(nx,0,1)));
 RZ(iy=reshape(take(sc(fy),shape(y)),apv(ny,0,1)));
 RZ(g=qq(cv,v2(0,0))); RZ(df2(z,ix,iy,g)); // frame agreement here
 R jtadjoin(jt,z,pz,tz);
}

// ***************************************** helpers ********************************************

// item i of tangent
static A jtaditem(J jt,A T,I i){if(ISZ(T))R ADZ; R admap2(ds(CFROM),sc(i),0,T,TR(T));}
// power count of u^:n or error
static I jtadpowcount(J jt,A n){
 ADASSERT(AT(n)&NOUN&&!AR(n));
 ADASSERT(AT(n)&B01+INT+FL); // boxed n not yet supported
 ADASSERT(!(AT(n)&FL&&(DAV(n)[0]==inf||DAV(n)[0]==infm))); // u^:_ and u^:__ not supported (derivative of a limit)
 ADASSERT(!(AT(n)&FL&&_isnan(DAV(n)[0]))); // u^: _. not supported
 I c=i0(n); RE(0); ADASSERT(c>=0); // negative powers (inverses) not yet supported
 R c;
}
static B jtadhaspowinf(J,A);
static B jtadhaspowinfb(J jt,A f){V*v; A g;
 if(!f||!(AT(f)&VERB)||ISPRIM(f))R 0;
 v=FAV(f); g=v->fgh[1];
 if(v->id==CCOLONE)R 0;
 if(v->id==CPOWOP&&g&&AT(g)&FL&&AN(g)==1&&(DAV(g)[0]==inf||DAV(g)[0]==infm||(_isnan(DAV(g)[0])&&g!=ds(CUSDOT))))R 1;
 if(ADISAGENDA(v)){DO(AN(v->fgh[0]), if(jtadhaspowinf(jt,AAV(v->fgh[0])[i]))R 1;)}
 R jtadhaspowinf(jt,v->fgh[0])||jtadhaspowinf(jt,g)||(v->id==CFORK&&jtadhaspowinf(jt,v->fgh[2]));
}
ADLOCWRAPS(B,adhaspowinf,1)

static A jtadmp(J jt){R dot(slash(ds(CPLUS)),ds(CSTAR));} // +/ .*

static A jtaditem(J,A,I);
// allocate empty list of n boxes
static A jtadboxlist(J jt,I n){A z;
 GATV0(z,BOX,n,1); A*v=AAV1(z); DO(n, v[i]=0;);
 AFLAGORLOCAL(z,BOX&RECURSIBLE); R z;
}
// store x as item i of box list made by jtadboxlist
#define adboxset(z,i,x) {A _x=(x); INCORPRA(_x); AAV1(z)[i]=_x;}

static I jtadfassemble(J jt,A zb,A*ts,I n,I k,A*pz,A*tz){A z,tb,Tz; A*zv=AAV1(zb); I rk=0; B any=0;
 RZ(z=ope(zb)); *pz=z; *tz=ADZ;
 if(!(AT(z)&NUMERIC))R 1;
 DO(n, if(!ISZ(ts[i]))any=1;); if(!any)R 1;
 RZ(tb=jtadboxlist(jt,n));
 DO(n, rk=MAX(rk,AR(zv[i])););
 DO(n, A ti=ts[i],ps,u; RZ(ps=over(sc(k),over(reshape(sc(rk-AR(zv[i])),num(1)),shape(zv[i]))));
  RZ(u=ISZ(ti)?reshape(ps,scf(0.0)):reshape(ps,adexpand(ti,zv[i]))); adboxset(tb,i,u); )
 RZ(Tz=ope(tb));
 RZ(*tz=cant2(over(num(1),over(num(0),apv(AR(Tz)-2,2,1))),Tz)); R 1; // direction axis to the front
}

// Bookkeeping for m@.v
// Return 0 on error, 1 for an atom s (*ps index), 2 for no cells (no derivative),
// 3 for groups: *pgb index lists, *pgk the selectors, *pinv inverse of grade of ,s.
static I jtadagplan(J jt,A f,A x,A y,A*ps,A*pgb,A*pgk,A*pinv){V*v=FAV(f); A s,g,gb,gk,inv; I nar=AN(v->fgh[0]),vr,n,ng,st,b,*gv,*sv,*kv;
 RZ(s=x?AO2(v->fgh[1],x,y):AO1(v->fgh[1],y)); // s =. [x] v y (integer: no derivative)
 if(!(AT(s)&INT))RZ(s=cvt(INT,s));
 vr=AR(s); // rank of selector: frame
 if(!vr){ // atom: one verb, whole arguments
  I j=IAV(s)[0]; j+=REPSGN(j)&nar; ASSERT(BETWEENO(j,0,nar),EVINDEX); *ps=sc(j); R 1;
 }
 ASSERT(MAX(AR(y),x?AR(x):0)>=vr,EVRANK);
 ASSERT(TESTAGREE(AS(s),AS(y),MIN(vr,AR(y))),EVLENGTH);
 if(x){ASSERT(TESTAGREE(AS(s),AS(x),MIN(vr,AR(x))),EVLENGTH);}
 n=AN(s); if(!n)R 2; // no cells
 sv=IAV(s); DO(n, ASSERT(sv[i]>=-nar&&sv[i]<nar,EVINDEX);); // every selector in range
 ADASSERT(!AR(y)||AR(y)>=vr); ADASSERT(!x||!AR(x)||AR(x)>=vr); // shorter frame (not an atom): refused
 RZ(g=grade1(ravel(s))); gv=IAV(g); // cell numbers sorted by selector (stable sort)
 ng=1; DO(n-1, ng+=sv[gv[i+1]]!=sv[gv[i]];); // number of groups: one per distinct selector
 RZ(gb=jtadboxlist(jt,ng)); GATV0(gk,INT,ng,1); kv=IAV(gk); // gb: cell numbers per group; gk: verb per group
 st=0; b=0; // start of the current group; groups done
 DO(n, if(i==n-1||sv[gv[i+1]]!=sv[gv[i]]){A ix; I j=sv[gv[i]];
  GATV0(ix,INT,i+1-st,1); MCISH(IAV(ix),gv+st,i+1-st);
  adboxset(gb,b,ix); kv[b++]=j+(REPSGN(j)&nar); st=i+1;
 });
 GATV0(inv,INT,n,1); DO(n, IAV(inv)[gv[i]]=i;); // restore cell order
 *ps=s; *pgb=gb; *pgk=gk; *pinv=inv; R 3;
}
// New shape for r, a group's result in m@.v (each item raised to rank m-1 by putting 1s in front of its shape)
static A jtadagraise(J jt,A r,I m){
 R over(sc(AS(r)[0]),over(reshape(sc(m-AR(r)),num(1)),behead(shape(r)))); // (# r) , ((m - #$r) $ 1) , }. $ r
}
static A jtadagcells(J jt,A s,A a){
 R over(sc(AN(s)),drop(sc(AR(s)),shape(a))); // (#,s) , (#$s) }. $ a
}
// forward rule for [x] m@.v y
static I jtadagendaf(J jt,A f,A x,A Tx,A y,A Ty,I k,A*pz,A*tz){A s,gb,gk,inv,*fv=AAV(FAV(f)->fgh[0]),cx=x,tcx=Tx,cy=y,tcy=Ty,zr=0,zt=ADZ; I c,ng;
 RZ(c=jtadagplan(jt,f,x,y,&s,&gb,&gk,&inv)); // plan
 if(c==2){ // no cells
  RZ(*pz=x?AO2(f,x,y):AO1(f,y)); *tz=ADZ; R 1;
 }
 if(c==1){ // atom selector
  A g=fv[IAV(s)[0]]; R x?jtadfwd2(jt,g,x,Tx,y,Ty,k,pz,tz):jtadfwd1(jt,g,y,Ty,k,pz,tz);
 }
 ng=AN(gb); // number of groups
 if(AR(s)>1){ // selector with two or more axes needs the arguments flattened
  if(x&&AR(x)){ADF1(ds(CCOMMA),x,Tx,&cx,&tcx); ADF2(ds(CDOLLAR),jtadagcells(jt,s,x),ADZ,cx,tcx,&cx,&tcx);}
  if(AR(y)){ADF1(ds(CCOMMA),y,Ty,&cy,&tcy); ADF2(ds(CDOLLAR),jtadagcells(jt,s,y),ADZ,cy,tcy,&cy,&tcy);}
 }
 A rb,*rs; I rmax=0; GATV0(rb,INT,2*ng,1); rs=(A*)IAV(rb); // each group's result and tangent
 DO(ng, A ix=AAV1(gb)[i],a=cx,ta=tcx,b=cy,tb=tcy,r,tr,g=fv[IAV(gk)[i]];
  if(ng>1){ // more than one group (otherwise cells in order)
   if(x&&AR(x))ADF2(ds(CFROM),ix,ADZ,cx,tcx,&a,&ta);
   if(AR(y))ADF2(ds(CFROM),ix,ADZ,cy,tcy,&b,&tb);
  }
  // differentiate verb on the group's list of cells
  if(x){ADF2(g,a,ta,b,tb,&r,&tr);
  }else{ADF1(g,b,tb,&r,&tr);}
  ASSERT(AR(r)&&AS(r)[0]==AN(ix),EVRANK); // one item per cell
  rs[2*i]=r; rs[2*i+1]=tr; rmax=MAX(rmax,AR(r)); // keep result and its tangent and updates the largest rank
 );
 // join groups in order
 DO(ng, A r=rs[2*i],tr=rs[2*i+1];
  if(AR(r)<rmax){ // result of lower rank
   ADF1(ds(CCOMMA),r,tr,&r,&tr); ADF2(ds(CDOLLAR),jtadagraise(jt,rs[2*i],rmax),ADZ,r,tr,&r,&tr);
  }
  if(zr){ADF2(ds(CCOMMA),zr,zt,r,tr,&zr,&zt); // append this group's items to what has been joined so far
  }else{zr=r; zt=tr;}
 );
 if(ng>1)ADF2(ds(CFROM),inv,ADZ,zr,zt,&zr,&zt); // back in cell order
 if(AR(s)>1)R jtadfwd2(jt,ds(CDOLLAR),shape(s),ADZ,zr,zt,k,pz,tz); // selector of two or more axes
 *pz=zr; *tz=zt; R 1;
}
// u\ y (mode 1) and u\. y (mode 2) for nonlinear u: apply u to each prefix/suffix (the cell loop, jtadcellm)
static I jtadscan(J jt,A f,I mode,A P,A T,I k,A*pz,A*tz){A d,cv,z,g; I n;
 B b=!!(AT(P)&BOX); // boxes: no tangent
 if(!b)RZ(T=admat(T,P));
 if(!AR(P)){RZ(P=ravel(P)); if(!b)RZ(T=reshape(over(sc(k),num(1)),T));}
 n=AS(P)[0];
 if(!n){
  RZ((z=AO1(mode==1?bslash(f):bsdot(f),P)));
  *pz=z; *tz=ADZ; R 1;
 }
 RZ(d=jlink(P,jlink(T,v2(k,mode))));
 cv=fdef(0,CDCAPDOT,VERB,jtadcellm,jtvalenceerr,f,d,0,VFLAGNONE,RMAX,RMAX,RMAX);
 RZ(g=qq(cv,num(0))); RZ(df1(z,apv(n,0,1),g));
 R jtadjoin(jt,z,pz,tz);
}
// forward rule for x u\ y (infixes) and x u\. y (outfixes, when out is set). It returns u on each cell (assembled) and tangent of that.
static I jtadinfixf(J jt,A f,B out,A x,A P,A T,I k,A zg,A*pz,A*tz){A z,W,b,tb; I m,n,c;
 if(ISZ(T)){
  RZ(z=zg?zg:(AO2(out?bsdot(f):bslash(f),x,P)));
  *pz=z; *tz=ADZ; R 1;
 }
 RE(m=i0(x)); // an integer x
 if(!AR(P)){ // atom y
  RZ(P=ravel(P)); RZ(T=reshape(over(sc(k),num(1)),T));
 }
 n=AS(P)[0]; c=adwincount(m,n);
 if(!c){ // no cells
  RZ(z=zg?zg:(AO2(out?bsdot(f):bslash(f),x,P)));
  *pz=z; *tz=ADZ; R 1;
 }
 if((W=jtadwinidx(jt,out,m,n,n?AN(P)/n:1))){ // gather path
  ADF2(ds(CFROM),W,ADZ,P,T,&b,&tb); ADF1(qq(f,sc(-1)),b,tb,&b,&tb);
 }else{A d,cv,g,zc; I pv[3]={k,out?4:3,m}; RE(0); // cell loop for unequal cells or table too big
  RZ(d=jlink(P,jlink(T,vec(INT,3,pv))));
  cv=fdef(0,CDCAPDOT,VERB,jtadcellm,jtvalenceerr,f,d,0,VFLAGNONE,RMAX,RMAX,RMAX);
  RZ(g=qq(cv,num(0))); RZ(df1(zc,apv(c,0,1),g)); RZ(jtadjoin(jt,zc,&b,&tb));
 }
 *pz=zg&&AR(zg)==AR(b)&&!memcmp(AS(zg),AS(b),AR(b)*SZI)?zg:b; *tz=tb; R 1;
}
// true when f is g/ with a verb g, used to recognize g/\. y
#define ADSUFFOLD(f) (AT(f)&VERB&&FAV(f)->id==CSLASH&&FAV(f)->fgh[0]&&AT(FAV(f)->fgh[0])&VERB)
// computee g/\. y and its tangent in forward mode
static I jtadsuffold(J jt,A f,A P,A T,I k,A*pz,A*tz){A g=FAV(f)->fgh[0],zb,a,ta; I n=AS(P)[0];
 if(!(AT(P)&BOX))RZ(T=admat(T,P)); // boxes have no tangent
 A tsb,*ts; RZ(zb=jtadboxlist(jt,n));
 GATV0(tsb,INT,n,1); ts=(A*)IAV(tsb); // the tangents of the steps, until they are boxed
 RZ(a=from(sc(n-1),P)); RZ(ta=jtaditem(jt,T,n-1)); adboxset(zb,n-1,a); ts[n-1]=ta;
 DQ(n-1, A p,t; RZ(p=from(sc(i),P)); RZ(t=jtaditem(jt,T,i)); ADF2(g,p,t,a,ta,&a,&ta); adboxset(zb,i,a); ts[i]=ta;)
 R jtadfassemble(jt,zb,ts,n,k,pz,tz);
}
// The same, recorded: each step on the tape, the results opened by an ASSEMBLE entry
static A jtadrassemble(J,A);
static A jtadrsuffold(J jt,A f,A P){A g=FAV(f)->fgh[0],zb,a; I n=AS(P)[0];
 RZ(zb=jtadboxlist(jt,n));
 RZ(a=jtadrec2(jt,ds(CFROM),sc(n-1),P)); adboxset(zb,n-1,a);
 DQ(n-1, A b; RZ(b=jtadrec2(jt,ds(CFROM),sc(i),P)); RZ(a=jtadrec2(jt,g,b,a)); adboxset(zb,i,a);)
 R jtadrassemble(jt,zb);
}
// u;.0
static B adcut0(A f){A g;
 if(FAV(f)->id!=CCUT)R 0;
 g=FAV(f)->fgh[1];
 R g&&AT(g)&(B01|INT)&&!AR(g)&&!BIV0(g)&&AT(FAV(f)->fgh[0])&VERB;
}
static A jtadcutsel(J jt){R cut(ds(CRIGHT),num(0));} // ];.0
static B adcutsel(A f){R adcut0(f)&&ISPRIM(FAV(f)->fgh[0])&&FAV(FAV(f)->fgh[0])->id==CRIGHT;} // ];.0 itself
static A jtadcutrevx(J jt,A y){I r=AR(y); R reshape(v2(2,r),over(reshape(sc(r),num(0)),negate(shape(y))));} // 0 ,: -$y

// The folds [x] u F. v y  and  F.. F.: F: F:. F::
static A jtadrassemble(J,A); static A jtaduntr(J,A);
static I jtadfoldap(J jt,A f,A x,A Tx,A y,A Ty,I k,B rec,A*pz,A*tz){ // x f y, or f y if x is 0
 if(rec){*tz=ADZ; R !!(*pz=x?jtadrec2(jt,f,x,y):jtadrec1(jt,f,y));}
 R x?jtadfwd2(jt,f,x,Tx,y,Ty,k,pz,tz):jtadfwd1(jt,f,y,Ty,k,pz,tz);
}
static I jtadfolditem(J jt,A y,A Ty,I j,B rec,A*pa,A*pta){ // item j of y (an atom is its own item)
 if(!AR(y)){*pa=y; *pta=Ty; R 1;}
 if(rec){*pta=ADZ; R !!(*pa=jtadrec2(jt,ds(CFROM),sc(j),y));}
 RZ(*pa=from(sc(j),y)); R !!(*pta=jtaditem(jt,Ty,j));
}
static I jtadfoldx(J jt,struct foldstatus*fi,A f,A x,A Tx,A y,A Ty,I k,B rec,A*pz,A*tz){V*v=FAV(f); A u=v->fgh[0],g=v->fgh[1];
 I fl=v->id-CFDOT; B dir=(fl&3)!=0,back=fl&1,mult=(fl&4)!=0,dyad=x!=0,ok;
 I ni=AR(y)?AS(y)[0]:1,nit=ni+dyad,cnt,i=0,nz=0,cap=0,zs=0;
 A a,ta,b,tb=ADZ,c,tc=ADZ,zb=0,tsb,*ts=0,z=0,tzz=ADZ;
 *tz=ADZ;
 if(dir&&nit<2){ // no step of v
  if(!nit){ASSERT(!mult,EVDOMAIN); RZ(z=AO1(f,y)); *pz=rec?jtaduntr(jt,z):z; R 1;} // no items (a monad): u of v's neutral
  if(dyad){a=x; ta=Tx;}else RZ(jtadfolditem(jt,y,Ty,0,rec,&a,&ta)); // one: u of it
  ++fi->exestats[1]; fi->zstatus=0; ok=jtadfoldap(jt,u,0,ADZ,a,ta,k,rec,&z,&tzz);
  if(fi->zstatus&0b01111){RESETERR; ASSERT(0,EVNORESULT);} // Z: ended it before a result
  if(!ok)R 0;
  ++fi->exestats[2];
  if(mult){RZ(z=reshape(over(num(0),shape(z)),z)); if(rec)RZ(z=jtaduntr(jt,z)); tzz=ADZ;} // Multiple: no step, no result
  *pz=z; *tz=tzz; R 1;
 }
 if(dir){
  if(dyad){a=x; ta=Tx; i=back?ni-1:0;}
  else{RZ(jtadfolditem(jt,y,Ty,back?ni-1:0,rec,&a,&ta)); i=back?ni-2:1;}
  cnt=nit-1;
 }else{a=y; ta=Ty; cnt=-1;} // F. F: until Z: ends it
 while(1){
  zs=0; ++fi->exestats[0]; fi->zstatus=0;
  if(dir){RZ(jtadfolditem(jt,y,Ty,i,rec,&c,&tc)); i+=back?-1:1; ok=jtadfoldap(jt,g,c,tc,a,ta,k,rec,&b,&tb);} // item v acc
  else ok=jtadfoldap(jt,g,x,Tx,a,ta,k,rec,&b,&tb); // [x] v acc
  zs|=fi->zstatus;
  if(zs&0b00011)goto zstop;
  if(ok){a=b; ta=tb;}
  if(!(zs&0b00100)){
   if(!ok)R 0;
   ++fi->exestats[1]; fi->zstatus=0; ok=jtadfoldap(jt,u,0,ADZ,a,ta,k,rec,&c,&tc);
   zs|=fi->zstatus;
   if(zs&0b00011)goto zstop;
   if(!(zs&0b00100)){
    if(!ok)R 0;
    if(!(zs&0b01000)){ // the result is kept (0 Z: keeps it out)
     ++fi->exestats[2];
     if(!mult){z=c; tzz=tc; nz=1;}
     else{
      if(nz==cap){A nb; I nc=cap?2*cap:dir?nit-1:16; // room for the results: as many as the steps, or growing (F:)
       RZ(nb=jtadboxlist(jt,nc)); GATV0(tsb,INT,nc,1);
       DO(nz, A e=AAV1(zb)[i]; ra(e); AAV1(nb)[i]=e; ((A*)IAV(tsb))[i]=ts[i];)
       zb=nb; ts=(A*)IAV(tsb); cap=nc;}
      adboxset(zb,nz,c); ts[nz]=tc; ++nz;
     }
    }
   }else RESETERR; // _1 Z: this step gives no result
  }else RESETERR;
  if(zs&0b10000)break; // 1 Z: the last step
  if(cnt>0&&!--cnt)break;
 }
 if(0){zstop: if(zs&0b00001)R 0; RESETERR;} // _3 Z: (the fold limit) is an error  _2 Z: ends the fold with the results so far
 ASSERT(nz,EVNORESULT);
 if(!mult){*pz=z; *tz=tzz; R 1;}
 AN(zb)=AS(zb)[0]=nz;
 if(rec)R !!(*pz=jtadrassemble(jt,zb));
 R jtadfassemble(jt,zb,ts,nz,k,pz,tz);
}
static I jtadfold(J jt,A f,A x,A Tx,A y,A Ty,I k,B rec,A*pz,A*tz){I r; struct foldstatus fi={{0,0,0},0},*s=jt->afoldinfo;
 jt->afoldinfo=&fi; r=jtadfoldx(jt,&fi,f,x,Tx,y,Ty,k,rec,pz,tz); jt->afoldinfo=s; R r;}
#define ADFOLDCASES case CFDOT: case CFDOTCO: case CFDOTDOT: case CFCO: case CFCOCO: case CFCODOT
// Check whether f (the u of u\ or u\.) is g/ with >. or <.
#define ADSCANMM(f) (AT(f)&VERB&&FAV(f)->id==CSLASH&&AT(FAV(f)->fgh[0])&VERB&&ISPRIM(FAV(f)->fgh[0])&&(FAV(FAV(f)->fgh[0])->id==CMAX||FAV(FAV(f)->fgh[0])->id==CMIN))
// Coefficients of derivative of >./\ <./\ >./\. <./\. (linear maps)
static I jtadscanmmc(J jt,B pre,A P,A z,A*pM,A*pW,A*pGn,A*pGs){A M,W,Ln,Ls,E,Q;
 I n=AS(P)[0],m=AN(P)/n; // n rows (items), m columns (atoms per item)
 GATV(E,B01,AN(P),AR(P),AS(P)); // E: the atom equals z in its row
 GATV(Q,B01,AN(P),AR(P),AS(P)); // Q: z equals z of the row above (the run goes on)
 B*ev=BAV(E),*qv=BAV(Q);
 qv[0]=0; // row 0 of Q is never read
 if(AT(P)&(B01|INT)&&AT(z)&(B01|INT)){A zi,pi; // integers compared as integers
  RZ(zi=AT(z)&INT?z:jtcvt(jt,INT,z)); // J's cvt: nothing recorded
  RZ(pi=AT(P)&INT?P:jtcvt(jt,INT,P));
  I*zv=IAV(zi),*pv=IAV(pi);
  DO(AN(P), ev[i]=pv[i]==zv[i];)
  DO(AN(P)-m, qv[i+m]=zv[i+m]==zv[i];)
 }else{A zf,pf;
  RZ(zf=AT(z)&FL?z:jtcvt(jt,FL,z));
  RZ(pf=AT(P)&FL?P:jtcvt(jt,FL,P));
  D*zv=DAV(zf),*pv=DAV(pf);
  DO(AN(P), ev[i]=pv[i]==zv[i];)
  DO(AN(P)-m, qv[i+m]=zv[i+m]==zv[i];)
 }
 GATV(M,FL,AN(P),AR(P),AS(P)); // M: E as floats
 GATV(W,FL,AN(P),AR(P),AS(P)); // W: 1 % the number of atoms sharing the derivative
 GATV(Ln,INT,AN(P),AR(P),AS(P)); // Ln: the row after the run, indexing a cell with zero item appended
 GATV(Ls,INT,AN(P),AR(P),AS(P)); // Ls: the run's first row, indexing a cell with zero item prepended
 D*mv=DAV(M),*wv=DAV(W); I*nv=IAV(Ln),*sv=IAV(Ls);
 DO(AN(P), mv[i]=ev[i];)
 DO(n, I r=i; B*q0=qv+r*m; I*s0=sv+r*m; // Ls, top down
  if(!r)DO(m, s0[i]=i;) // row 0 starts a run: the zero item
  else DO(m, s0[i]=q0[i]?s0[i-m]:r*m+i;)) // the run goes on: the row above's; else this row
 DQ(n, I r=i; B*q1=qv+(r+1)*m; I*n0=nv+r*m; // Ln, bottom up
  if(r==n-1)DO(m, n0[i]=n*m+i;) // the last row: the zero item
  else DO(m, n0[i]=q1[i]?n0[i+m]:(r+1)*m+i;)) // the run goes on: the next row's; else the next row
 if(pre)DO(n, I r=i; B*q0=qv+r*m; D*w0=wv+r*m,*m0=mv+r*m; // prefix: atoms equal to z, run start to row r
  if(!r)DO(m, w0[i]=m0[i];)
  else DO(m, w0[i]=m0[i]+(q0[i]?w0[i-m]:0);))
 else DQ(n, I r=i; B*q1=qv+(r+1)*m; D*w0=wv+r*m,*m0=mv+r*m; // suffix: atoms equal to z, row r to run end
  if(r==n-1)DO(m, w0[i]=m0[i];)
  else DO(m, w0[i]=m0[i]+(q1[i]?w0[i+m]:0);))
 DO(AN(P), wv[i]=wv[i]?1.0/wv[i]:0.0;) // W = % count (a count of 0 only with a NaN)
 *pM=M; *pW=W;
 RZ(*pGn=atop(amp(Ln,ds(CFROM)),ds(CCOMMA))); // Gn = (Ln&{)@,
 RZ(*pGs=atop(amp(Ls,ds(CFROM)),ds(CCOMMA))); // Gs = (Ls&{)@,
 R 1;
}
// Check whether f (u of u\ or u\.) is */
#define ADSCANP(f) (AT(f)&VERB&&FAV(f)->id==CSLASH&&AT(FAV(f)->fgh[0])&VERB&&ISPRIM(FAV(f)->fgh[0])&&FAV(FAV(f)->fgh[0])->id==CSTAR)
// Return Z with Z_0 = X_0, Z_i = X_i + a_i*Z_(i-1) over the n items (pre=0 from the other end) for every direction of X at once. Used by derivatives of */\ and */\.
static A jtadlinrec(J jt,B pre,A a,A X,I r){I n=AS(a)[0];
 if(ISZ(X))R ADZ;
 if(!pre){RZ(a=AO1(ds(CREV),a)); RZ(X=admap1(ds(CREV),X,r));} // suffix recurrence is prefix one reversed
 A *old=jt->tnextpushp;
 for(I s=1;s<n;s<<=1){A bk,bt,bh,at,ah; // doubling: each step joins windows of s items into windows of 2s, so after log2 n steps item i holds Z_i
  RZ(bk=admap1(amp(sc(s),ds(CTAKE)),X,r)); RZ(bt=admap1(amp(sc(s),ds(CDROP)),X,r)); RZ(bh=admap1(amp(sc(-s),ds(CDROP)),X,r));
  RZ(at=AO2(ds(CDROP),sc(s),a)); RZ(ah=AO2(ds(CDROP),sc(-s),a));
  RZ(X=admap2(ds(CCOMMA),bk,r,plus(bt,adscale(at,bh)),r)); // first s items final. For i>=s: X_i + a_i * X_(i-s)
  if(2*s<n)RZ(a=AO2(ds(CCOMMA),AO2(ds(CTAKE),sc(s),a),tymes(at,ah))); // a_i * a_(i-s), for next steps
  RZ(gc3(&X,&a,0,old)); // free this step's temporaries
 }
 if(!pre)RZ(X=admap1(ds(CREV),X,r)); // reverse suffix case back
 R X; // X holds Z
}
// Forward rule for */\ y and */\. y
static I jtadfwdscanp(J jt,A f,B pre,A P,A T,A zg,A*pz,A*tz){A z,c;
 RZ(z=zg?zg:(AO1(f,P))); *pz=z;
 if(ISZ(T)){*tz=ADZ; R 1;}
 RZ(T=jtadexpand(jt,T,P));
 RZ(c=pre
  ?AO2(ds(CCOMMA),num(1),AO1(ds(CCTAIL),z))
  :AO2(ds(CCOMMA),AO1(ds(CBEHEAD),z),num(1)));
 RZ(*tz=jtadlinrec(jt,pre,P,adscale(c,T),AR(P))); R 1;
}
// Forward rule for the scans.
static I jtadfwdscanmm(J jt,A f,B pre,A P,A T,A zg,A*pz,A*tz){A z,M,W,Gn,Gs,Q;
 I r=AR(P);
 RZ(z=zg?zg:(AO1(f,P)));
 *pz=z;
 if(ISZ(T)){*tz=ADZ; R 1;} // no tangent: none
 RZ(jtadscanmmc(jt,pre,P,z,&M,&W,&Gn,&Gs)); // coefficients of linear map
 RZ(Q=adscale(M,T));
 RZ(Q=admap1(pre?bslash(slash(ds(CPLUS))):bsdot(slash(ds(CPLUS))),Q,r)); // running sums: +/\ for a prefix scan, +/\. for a suffix scan
 A G=pre?admap1(Gs,admap2(ds(CCOMMA),scf(0.0),0,Q,r),r) // prefix: the sum before the run (0 , Q at its first row)
        :admap1(Gn,admap2(ds(CCOMMA),Q,r,scf(0.0),0),r); // suffix: the sum after the run (Q , 0 at the row after it)
 RZ(G);
 RZ(*tz=adscale(W,minus(Q,G))); // the sum over the run up to this row (suffix: from it), shared among its atoms equal to z
 R 1;
}

// */ y, >./ y, <./ y without the item-by-item fold: the weight of each item in the derivative, so that
// d(u/ y) = +/ G * dy (over the items).
//  */ : the product of the items before times the product of the items after,  (}: 1 , */\ y) * }. (*/\. y) , 1
//  >./ <./ : the items equal to the maximum (minimum) share the derivative evenly (as x >. y splits a tie):
//        c % ($y) $ +/ c   with  c =. y = ($y) $ >./ y   (atom by atom over the items; exact comparison)
static B adfoldfast(A f){V*v=FAV(f); A g=v->fgh[0];
 R v->id==CSLASH&&AT(g)&VERB&&ISPRIM(g)&&(FAV(g)->id==CSTAR||FAV(g)->id==CMAX||FAV(g)->id==CMIN);}
static A jtadfoldw(J jt,A f,A P,A z){C id=FAV(FAV(f)->fgh[0])->id; A a,b;
 if(!AR(P))R num(1); // u/ of an atom is the atom
 if(id==CSTAR){
  RZ(a=AO1(bslash(slash(ds(CSTAR))),P)); RZ(a=AO2(ds(CCOMMA),num(1),a)); RZ(a=AO1(ds(CCTAIL),a)); // products before
  RZ(b=AO1(bsdot(slash(ds(CSTAR))),P)); RZ(b=AO2(ds(CCOMMA),b,num(1))); RZ(b=AO1(ds(CBEHEAD),b)); // products after
  R tymes(a,b);
 }
 A ri=sc(AR(P)-1); // rank of item: each item against whole of z
 {PUSHCCT(1.0) a=AO2(qq(ds(CEQ),ri),P,z); POPCCT RZ(a);} // c: atoms equal to the maximum (minimum), compared exactly
 if(AT(a)&B01){I n1=0; B*cv=BAV(a);
  DO(AN(a), n1+=cv[i];) // one atom per result atom (no tie): c itself.
  B nan=0; if(AT(z)&FL)DO(AN(z), if(_isnan(DAV(z)[i])){nan=1; break;})
  if(n1==AN(z)&&!nan)R a;
 }
 RZ(b=AO1(slash(ds(CPLUS)),a)); // how many share each atom of the result
 R AO2(qq(ds(CDIV),ri),a,b);
}

// u :: v
// Return 1 (error cleared) to use v, 0 to keep the error
static B jtadadvfails(J jt){I e=jt->jerr;
 if(e==EVTHROW||e==EVEXIT||BETWEENC(e,EVATTN,EVBREAK))R 0;
 if(jtadrefusal(jt))R 0; // D.'s refusal, stack error, full workspace
 RESETERR; R 1; // J's own error: v
}
// reverse mode: drop the tape entries recorded by a branch that failed (from entry n on)
static void jtadtapecut(J jt,I n){ADTRACE*t=ADT;
 for(I i=n;i<t->ntape;++i){ADENT*e=&t->tape[i]; if(e->f)fa(e->f); if(e->x)fa(e->x); if(e->y)fa(e->y); fa(e->z);}
 t->ntape=n;
}

// ******************************************** boxes ********************************************
// The rules normally work with an argument and its tangent. Boxes have no tangent because boxes are never traced.
// A box array that holds traced value would look like a constant to the ordinary rules and its derivatives would be lost.
// So when jtadholds says the argument holds a traced value, the rule sets bx and takes a separate path.

static I jtadscatter(J,A,A,A,A);
static A jtadcot(J,A,B); static I jtadacc(J,A,A); static A jtadmkscat(J,A,A,I);
static A jtadtan(J,A);
static A jtadfwdstruct(J,A,A,A);
static A jtadnorule(J,A,A,A,A);
static B jtadisexpl(J,A);
static B jtadrunsown(J,A);
// Is the box path needed?
#define ADBOXCHK(f,x,y) ((ADBOXHOLDS(x)||ADBOXHOLDS(y))&&!jtadrunsown(jt,(f)))

static B adboxwalk(A f){
 switch(FAV(f)->id){
 case CAT: case CATCO: case CAMP: case CAMPCO: case CFORK: case CHOOK: case CTILDE: case CQQ:
 case CUNDER: case CUNDCO: case CPOWOP: case CSLASH: R 1;
 ADFOLDCASES: R 1;
 case CADVERSE: case CCOLON: case CMCAP: case COBVERSE: R 1;
 case CATDOT: R ADISAGENDA(FAV(f));
 default: R 0;
 }
}
static A jtadrealarg(J jt,A x){
 if(x&&AFLAG(x)&AFVIRTUAL&&ISZ(adtget(x)))R realize(x);
 R x;
}
static A jtadconstbox(J jt,A f,A x,A y){A z;
 if(ISPRIM(f)&&(x?adstructx(f)||adstructy(f)||adstructj(f):adstructm(f))){ // a verb that only moves items
  RZ(z=x?(AO2(f,x,y)):(AO1(f,y)));
  if(AT(z)&BOX)R z; // boxes moved
 }
 if(x)RZ(x=jtadrealarg(jt,x));
 RZ(y=jtadrealarg(jt,y));
 RZ(z=x?(AO2(f,x,y)):(AO1(f,y)));
 R jtadnorule(jt,f,z,x,y);
}

// The box map: y with each numeric leaf replaced by its atom numbers. The box verb applied to it gives z's index map
typedef struct {A*v; I n,cap;} ADLEAVES;
static A jtadleafmap(J jt,A y,I*base,ADLEAVES*l){A z; I n=AN(y); // y with its numeric leaves replaced by atom numbers
 STACKCHKOFL
 GATV(z,BOX,n,AR(y),AS(y)); A*zv=AAV(z); DO(n, zv[i]=mtv;); AFLAGORLOCAL(z,BOX&RECURSIBLE);
 DO(n, A c=C(ADBOXE(y,i)); A m=c;
  if(AT(c)&BOX){
   RZ(m=jtadleafmap(jt,c,base,l));
  }else if(AT(c)&NUMERIC&&AN(c)){
   if(l->n==l->cap){
    I nc=l->cap?2*l->cap:16;
    A*nv=realloc(l->v,nc*sizeof(A));
    ASSERT(nv!=0,EVWSFULL);
    l->v=nv; l->cap=nc;
   }
   l->v[l->n++]=c;
   RZ(m=reshape(shape(c),apv(AN(c),*base+1,1)));
   *base+=AN(c);
  }
  INCORPRA(m); zv[i]=m;
 )
 R z;
}
// The leaf that atom number s (1..ov[n]) of a box map belongs to. ov holds the leaves' offsets (n+1 of them, from 0);
// j is a guess (the previous answer), right when the map runs in order
static I adleafof(I*ov,I n,I s,I j){I lo=0,hi=n-1;
 if(j<n&&ov[j]<s&&s<=ov[j+1])R j;
 while(lo<hi){I md=(lo+hi)>>1; if(s<=ov[md+1])hi=md;else lo=md+1;} // the first leaf ending at or after s
 R lo;
}
// Offsets of the leaves lv in the map: ov[i] atoms before leaf i, ov[n] in all
static A jtadleafoffs(J jt,A lv){A O; I n=AN(lv),*ov;
 GATV0(O,INT,n+1,1); ov=IAV(O); ov[0]=0;
 DO(n, ov[i+1]=ov[i]+AN(C(AAV(lv)[i]));)
 R O;
}
// Compute tangent of a box rule's result in forward mode. A box rule here means > ; {:: or a selection etc. applied to boxes holding traced values.
// Only the leaves that z uses and whose tangent is not zero are gathered, through a map renumbered for them.
static A jtadleafgather(J jt,A lv,A M,A z,B replay){I k=ADT->k,n=AN(lv),nz=AN(M),nu=0; B up=0; A Tall,O,Us,Ts; I*ov,*uv,*mv=IAV(M); A*tv;
 RZ(O=jtadleafoffs(jt,lv)); ov=IAV(O);
 GATV0(Us,INT,n,1); uv=IAV(Us); DO(n, uv[i]=0;) // uv[j]: 1 if z uses leaf j
 GATV0(Ts,INT,n,1); tv=(A*)IAV(Ts); // leaves' tangents
 {I j=0; DO(nz, I s=mv[i]; if(s>0&&s<=ov[n]){j=adleafof(ov,n,s,j); uv[j]=1;})}
 DO(n, A c=C(AAV(lv)[i]); A T=replay?jtadtan(jt,c):adtget(c); tv[i]=T;
  if(uv[i]&&!ISZ(T)){uv[i]=nu; nu+=AN(c); if(!up)up=UPTR(T);}else uv[i]=-1; // uv[j]: now leaf j's offset among those kept, or -1
 )
 if(!nu)R ADZ;
 if(nu<ov[n]){A M2; I*m2; // some leaves dropped: renumber the map (a dropped leaf's atoms become 0)
  GATV(M2,INT,nz,AR(M),AS(M)); m2=IAV(M2);
  {I j=0;
   DO(nz, I s=mv[i]; I r=0;
    if(s>0&&s<=ov[n]){j=adleafof(ov,n,s,j);
     if(uv[j]>=0)r=uv[j]+s-ov[j];
    }
    m2[i]=r;
   )
  }
  M=M2;
 }
 if(up){A Ps; A*pv; I np=0; // an outer level watches
  GATV0(Ps,INT,n,1); pv=(A*)IAV(Ps);
  DO(n, A c=C(AAV(lv)[i]); if(uv[i]<0)continue; RZ(pv[np++]=reshape(v2(k,AN(c)),adexpand(tv[i],c)));)
  for(I w=np;w>1;w=(w+1)>>1){
   DO(w>>1, RZ(pv[i]=AO2(qq(ds(CCOMMA),v2(1,1)),pv[2*i],pv[2*i+1]));) // pv[i] is pv[2i] ,"1 pv[2i+1]
   if(w&1)pv[w>>1]=pv[w-1]; // an odd one left over moves down
  }
  Tall=pv[0];
 }else{D*dv;
  GATV0(Tall,FL,k*nu,2); AS(Tall)[0]=k; AS(Tall)[1]=nu; dv=DAV(Tall);
  DO(n, A c=C(AAV(lv)[i]); A T=tv[i]; I nc=AN(c),off=uv[i];
   if(off<0)continue;
   RZ(T=adexpand(T,c)); RZ(T=cvt(FL,T)); D*sv=DAV(T); DO(k, MC(dv+i*nu+off,sv+i*nc,nc*sizeof(D));))
 }
 R jtadfwdstruct(jt,M,Tall,z);
}
// Check whether z is one of y's contents, by address, at any depth.
static B jtadinside(J jt,A y,A z){
 if(unlikely(!(STACKPOS>=jt->cstackmin)))R 0; // too deep: not found here (the index map that follows reports error)
 DO(AN(y), A c=C(ADBOXE(y,i));
  if(c==z||(AT(c)&BOX&&jtadinside(jt,c,z)))R 1;
 )
 R 0;
}

// x f y (f y when x is 0), f is box verb (> ; {:: ...), y boxes holding traced values.
static I jtadboxmap0(J jt,A f,A x,A y,I k,A*pz,A*tz,B rev){ADLEAVES l={0,0,0}; I base=0; A z,yp,M,lv; I r=0;
 RZ(z=x?(AO2(f,x,y)):(AO1(f,y)));
 *pz=z; if(tz)*tz=ADZ;
 if(!(AT(z)&NUMERIC)||!AN(z))R 1; // boxes, text or empty: nothing to differentiate
 {A o=adtget(z); if(!ISZ(o)){if(tz)*tz=o; R 1;}} // traced content itself, take its own tangent, nothing to record
 if(jtadinside(jt,y,z))R 1; // untraced content itself: a constant
 STACKCHKOFL
 if(!(yp=jtadleafmap(jt,y,&base,&l)))goto exit; // y with each numeric leaf replaced by its atom numbers, leaves in l
 if(x)df2(M,x,yp,f);else df1(M,yp,f); // the same verb on the map: M tells where each atom of z came from
 if(!M)goto exit;
 if(!l.n){r=1; goto exit;} // no numeric leaf: z is fill only, derivative 0
 if(!(M=jtcvt(jt,INT,M))||!(lv=jtadboxlist(jt,l.n)))goto exit; // map as integers; box list for the leaves
 if(!(AR(M)==AR(z)&&!memcmp(AS(M),AS(z),AR(z)*sizeof(I)))){ // map must have z's shape, or gathering through it is wrong
  jsignal(EVSYSTEM); goto exit;
 }
 DO(l.n, adboxset(lv,i,l.v[i]);) // the leaves, owned by lv
 if(rev){ // reverse: record z as gathered from the leaves through M
  B any=0;
  DO(l.n, if(TRC(l.v[i]))any=1;) // is any leaf a node at this level?
  if(any){
   if(!(*pz=jtadrecord(jt,ADKBOXMAP,0,lv,M,z)))goto exit; // a BOXMAP entry
  }
 }else{ // forward: z's tangent is the leaves' tangents gathered through M
  if(!(*tz=jtadleafgather(jt,lv,M,z,0)))goto exit;
 }
 r=1;
exit:
 free(l.v); R r; // the C array of leaf pointers, on every path
}
// The same, but x {:: y with a path of 2 or more steps first follows all steps but the last on the real data
// x {:: y is the last step applied to w =. (}:x) {:: y. Only w is mapped, not all of y.
// The last step on traced list or table w is ({:x) { w with the rule for {.
// A step may also pick from numbers ((0;1;0) {:: <y). Then w is new and all of y is mapped
static I jtadboxmap(J jt,A f,A x,A y,I k,A*pz,A*tz,B rev){A w,p;
 if(!(x&&FAV(f)->id==CFETCH&&AT(x)&BOX&&AR(x)==1&&AN(x)>1))R jtadboxmap0(jt,f,x,y,k,pz,tz,rev);
 RZ(AO2(f,x,y)); // x {:: y first, for J's errors: each step but the last must pick one atom (box, or one number)
 RZ(w=AO2(f,curtail(x),y)); RZ(p=tail(x));
 if(AT(w)&BOX)R jtadboxmap0(jt,f,ravel(p),w,k,pz,tz,rev); // boxes: the last step on them alone
 if(TRC(w)){ // traced leaf
  if(AR(w)){ // last step is p { w
   if(rev)R !!(*pz=jtadrec2(jt,ds(CFROM),p,w));
   R jtadfwd2(jt,ds(CFROM),p,ADZ,w,adtget(w),k,pz,tz);
  }
  RZ(p=over(box(num(0)),p)); R jtadboxmap0(jt,f,p,box(w),k,pz,tz,rev); // atom
 }
 if(jtadinside(jt,y,w)){RZ(*pz=AO2(f,x,y)); if(tz)*tz=ADZ; R 1;} // untraced leaf of y: a constant
 R jtadboxmap0(jt,f,x,y,k,pz,tz,rev); // map all of y
}
// BOXMAP backward: Ct (m,$z) back through map M to each traced leaf, in one pass over M.
static I jtadboxmapbk(J jt,A lv,A M,A z,A Ct){I n=AN(lv),nz=AN(M),m=ADT->k,*mv=IAV(M),*ov; A O,P; D**pv; B up=0;
 RZ(O=jtadleafoffs(jt,lv)); ov=IAV(O); // ov[i]: atoms before leaf i
 if(NESTED){up=UPTR(Ct); DO(n, A c=C(AAV(lv)[i]); if(!up&&TRC(c))up=UPTR(jtadcot(jt,c,0));)} // is Ct or a leaf's cotangent traced outside?
 if(up){A ix,G; I*iv,nt=ov[n]; // yes: J verbs, which the outer level records.
  GATV0(ix,INT,nz,1); iv=IAV(ix); DO(nz, I s=mv[i]; iv[i]=s>0&&s<=nt?s-1:-1;)
  RZ(G=AO1(jtadmkscat(jt,ix,vec(INT,1,&nt),AR(M)),Ct));
  DO(n, A c=C(AAV(lv)[i]); A Gc=G;
   if(!TRC(c))continue; // constant leaf: nothing to pass back
   if(n>1)RZ(Gc=AO2(qq(ds(CFROM),v2(1,1)),apv(AN(c),ov[i],1),G)); // (ov[i]+i.#,c) {"1 G
   RZ(Gc=reshape(over(sc(m),shape(c)),Gc)); RZ(jtadacc(jt,c,Gc)); // added to leaf's cotangent
  )
  R 1;
 }
 RZ(Ct=cvt(FL,Ct)); ASSERT(AN(Ct)==m*nz,EVLENGTH);
 GATV0(P,INT,n,1); pv=(D**)IAV(P); // each leaf's cotangent data, 0 if not a node
 DO(n, A c=C(AAV(lv)[i]); A cb; pv[i]=0;
  if(TRC(c)){RZ(cb=jtadcot(jt,c,1)); pv[i]=DAV(cb);}
 )
 D*cv=DAV(Ct); I j=0; // cv: atom i of z in direction 0  j: the previous leaf
 DO(nz, I s=mv[i];
  if(s>0&&s<=ov[n]){j=adleafof(ov,n,s,j); D*q=pv[j]; // atom s is atom s-1-ov[j] of leaf j
   if(q){I nc=ov[j+1]-ov[j]; q+=s-1-ov[j]; DO(m, q[i*nc]+=cv[i*nz];)} // add it in every direction
  }
  ++cv;
 )
 R 1;
}

// ***************************************** monad rules *****************************************
static I jtadfwd1x(J,A,A,A,I,A*,A*);
static I jtadfwdnorule(J jt,A f,A x,A Tx,A y,A Ty,A*pz,A*tz){A z;
 if(jt->jerr!=EVNONCE||!ADT->refused||ADT->fbfail||jtadhaspowinf(jt,f)||ADISSELF(f))R 0; // only rule's refusal
 RESETERR; ADT->refused=0;
 // the arguments entered with their tangents, so that a result holding one in boxes is seen as holding a traced value
 if(x)RZ(x=ISZ(Tx)?jtadrealarg(jt,x):adtreg(x,Tx)); RZ(y=ISZ(Ty)?jtadrealarg(jt,y):adtreg(y,Ty));
 RZ(z=jtadop(jt,f,x,y)); RZ(z=jtadnorule(jt,f,z,x,y)); // refused: final
 *pz=z; *tz=x&&z==x?Tx:z==y?Ty:jtadtgetf(jt,z);
 R 1;
}
static I jtadfwd1(J jt,A f,A P,A T,I k,A*pz,A*tz){B rp=ADT&&ADT->zgiven;
 if(!jtadfwd1x(jt,f,P,T,k,pz,tz)){if(rp)R 0; RZ(jtadfwdnorule(jt,f,0,ADZ,P,T,pz,tz));}
 if(*pz==P&&ISZ(T)&&!ISZ(*tz)&&ISZ(adtget(P)))RZ(*pz=ca(P));
 R 1;
}
// %. y where it is not a square matrix: % y for an atom, y % +/ *: y for a list, and for matrix pseudo-inverse:
// (%. (|: y) +/ . * y) +/ . * |: y with more rows than columns, (|: y) +/ . * %. y +/ . * |: y with more columns than rows.
static I jtadpinvf(J jt,A P,A T,I k,A*pz,A*tz){A a,ta,b,tb,mp;
 if(!AR(P))R jtadfwd1(jt,ds(CDIV),P,T,k,pz,tz);
 if(AR(P)==1){
  RZ(jtadfwd1(jt,ds(CSTARCO),P,T,k,&a,&ta));
  RZ(jtadfwd1(jt,slash(ds(CPLUS)),a,ta,k,&b,&tb));
  R jtadfwd2(jt,ds(CDIV),P,T,b,tb,k,pz,tz);
 }
 ADASSERT(AR(P)==2);
 RZ(mp=jtadmp(jt));
 RZ(jtadfwd1(jt,ds(CCANT),P,T,k,&a,&ta));
 if(AS(P)[0]<AS(P)[1]){
  RZ(jtadfwd2(jt,mp,P,T,a,ta,k,&b,&tb));
  RZ(jtadfwd1(jt,ds(CDOMINO),b,tb,k,&b,&tb));
  R jtadfwd2(jt,mp,a,ta,b,tb,k,pz,tz);
 }
 RZ(jtadfwd2(jt,mp,a,ta,P,T,k,&b,&tb)); RZ(jtadfwd1(jt,ds(CDOMINO),b,tb,k,&b,&tb));
 R jtadfwd2(jt,mp,b,tb,a,ta,k,pz,tz);
}
static A jtadrec1(J,A,A);
static A jtadrec2(J,A,A,A);
static A jtadpinvr(J jt,A P){A a,b;
 if(!AR(P))R jtadrec1(jt,ds(CDIV),P);
 if(AR(P)==1){
  RZ(a=jtadrec1(jt,ds(CSTARCO),P));
  RZ(a=jtadrec1(jt,slash(ds(CPLUS)),a));
  R jtadrec2(jt,ds(CDIV),P,a);
 }
 ADASSERT(AR(P)==2);
 RZ(a=jtadrec1(jt,ds(CCANT),P));
 if(AS(P)[0]<AS(P)[1]){
  RZ(b=jtadrec2(jt,jtadmp(jt),P,a));
  RZ(b=jtadrec1(jt,ds(CDOMINO),b));
  R jtadrec2(jt,jtadmp(jt),a,b);
 }
 RZ(b=jtadrec2(jt,jtadmp(jt),a,P)); RZ(b=jtadrec1(jt,ds(CDOMINO),b));
 R jtadrec2(jt,jtadmp(jt),b,a);
}

static A jtaddetadj1(J jt,A P,I n,D sh){A mp,u,v,uv,B,Bi,bu,vb,s,d,t; D m=0.0,*pu,*pv;
 RZ(mp=jtadmp(jt));
 if(AT(P)&FL){A pf;
  RZ(pf=cvt(FL,P));
  DO(AN(pf), D e=DAV(pf)[i]; if(e<0)e=-e; if(e>m)m=e;) // the scale of P (its value only)
 }
 if(!(m>0))m=1.0;
 GATV0(u,FL,n,1); GATV0(v,FL,n,1); pu=DAV(u); pv=DAV(v);
 DO(n, pu[i]=m*(0.5+0.5*sin(sh+1.7*(i+1))); pv[i]=m*(0.5+0.5*cos(sh+2.3*(i+1)));) // deterministic, of P's magnitude
 RZ(uv=AO2(slash(ds(CSTAR)),u,v)); RZ(B=plus(P,uv));
 WITHDEBUGOFF(Bi=AO1(ds(CDOMINO),B);)
 if(!Bi){ // B singular too: no answer from this pair
  if(jt->jerr!=EVDOMAIN)R 0;
  RESETERR; R mtv;
 }
 RZ(d=AO1(dot(slash(ds(CMINUS)),ds(CSTAR)),B)); // det B
 RZ(bu=AO2(mp,Bi,u)); RZ(vb=AO2(mp,v,Bi)); RZ(s=AO2(mp,v,bu));
 RZ(t=plus(tymes(minus(num(1),s),Bi),AO2(slash(ds(CSTAR)),bu,vb)));
 R tymes(d,t);
}
static A jtaddetfac(J jt,A P,A z){A a,mp,id,M,c; I n=AS(P)[0];
 if(n&&n<=3){A p; RZ(p=ravel(P)); // up to 3 by 3
  if(n==1)R reshape(v2(1,1),num(1));
  if(n==2){I ix[4]={3,2,1,0},sg[4]={1,-1,-1,1};
   RZ(a=from(vec(INT,4,ix),p)); RZ(a=tymes(vec(INT,4,sg),a)); R reshape(v2(2,2),a);
  }
  {I a1[9],a2[9],b1[9],b2[9]; A s,t,u,w;   // C_ij = P_(i+1)(j+1) P_(i+2)(j+2) - P_(i+1)(j+2) P_(i+2)(j+1), indices mod 3
   DO(9, I r=i/3,q=i%3; a1[i]=3*((r+1)%3)+(q+1)%3; a2[i]=3*((r+2)%3)+(q+2)%3; b1[i]=3*((r+1)%3)+(q+2)%3; b2[i]=3*((r+2)%3)+(q+1)%3;)
   RZ(s=from(vec(INT,9,a1),p)); RZ(t=from(vec(INT,9,a2),p)); RZ(s=tymes(s,t));
   RZ(u=from(vec(INT,9,b1),p)); RZ(w=from(vec(INT,9,b2),p)); RZ(u=tymes(u,w));
   RZ(s=minus(s,u)); R reshape(v2(3,3),s);}
 }
 if((a=AO1(ds(CDOMINO),P)))R tymes(z,cant1(a));
 if(jt->jerr!=EVDOMAIN)R 0;
 RESETERR;
 if(AT(P)&FL){A adj; // floating point: the rank-one update with two pairs u v
  RZ(adj=jtaddetadj1(jt,P,n,0.0)); if(AN(adj)||!n)R cant1(adj);
  RZ(adj=jtaddetadj1(jt,P,n,0.61)); if(AN(adj))R cant1(adj);
  R reshape(v2(n,n),scf(0.0)); // B singular for both pairs
 }
 RZ(mp=jtadmp(jt)); RZ(id=reshape(v2(n,n),take(sc(n+1),num(1))));
 M=0; c=num(1);
 for(I k=1;k<=n;++k){A t,tr;
  {A ci; RZ(ci=tymes(c,id));
   if(M){A pm; RZ(pm=AO2(mp,P,M)); RZ(M=plus(pm,ci));
   }else M=ci;}
  RZ(t=AO2(mp,P,M)); RZ(t=(AO2(ds(CCANT),box(v2(0,1)),t))); RZ(tr=(AO1(slash(ds(CPLUS)),t)));
  RZ(c=negate(divide(tr,sc(k))));
 }
 if(!M)R reshape(v2(0,0),scf(0.0));
 R cant1(n&1?M:negate(M));
}
// u&.v y with v one of the forms J runs as a structural under where v has no inverse:
// u&., y  is  ($y) $ u , y
// u&.v y, v m&{ or (m { ]) or (m { [),  is  (u m{y) m} y
// u&.v y, v m&(];.0) or (m (];.0) ]) or (m (];.0) [) with m an integer 2 1 table (start, positive length),  is  (u v y) i} y,
//   i the items v selects (v i.#y);  u must keep the shape of v y
// Result: 1 for ,  2 for m&{ (*pm = m)  3 for m&(];.0) (*pm = v)  0 otherwise
static I jtadsunder(J jt,A f,A*pm){V*v=FAV(f); A g=v->fgh[1]; V*w; A m;
 if(v->id!=CUNDER||!g||!(AT(g)&VERB))R 0;
 w=FAV(g);
 if(w->id==CCOMMA&&ISPRIM(g))R 1;
 if(w->id==CAMP&&w->fgh[0]&&AT(w->fgh[0])&NOUN&&AT(w->fgh[1])&VERB&&FAV(w->fgh[1])->id==CFROM){
  *pm=w->fgh[0]; R 2;
 }
 if(w->id==CFORK&&w->fgh[0]&&AT(w->fgh[0])&NOUN&&w->fgh[1]&&AT(w->fgh[1])&VERB&&FAV(w->fgh[1])->id==CFROM&&w->fgh[2]&&(ID(w->fgh[2])&~1)==CLEFT){
  *pm=w->fgh[0]; R 2;
 }
 if((w->id==CAMP||(w->id==CFORK&&w->fgh[2]&&(ID(w->fgh[2])&~1)==CLEFT))&&(m=w->fgh[0])&&AT(m)&INT&&AR(m)==2&&AS(m)[0]==2
    &&AS(m)[1]==1&&IAV(m)[1]>0&&w->fgh[1]&&AT(w->fgh[1])&FUNC&&FAV(w->fgh[1])->valencefns[1]==jtrightcut0){
  *pm=g; R 3;
 }
 R 0;
}
static I jtadfwd1x(J jt,A f,A P,A T,I k,A*pz,A*tz){A z,F,a,b,ta,tb; V*v; A fs,gs,hs;
 A zg=0; if(ADT&&ADT->zgiven){zg=ADT->zgiven; ADT->zgiven=0;} // replaying a tape: this entry's result is already recorded, so the rule computes only the tangent; cleared at once, so nested calls compute their own
 ASSERT(AT(f)&VERB,EVDOMAIN);
 B tr=jtadhastraced(jt,f); // f holds a traced noun as an operand (as g =. y&* in explicit code): even with a constant argument it is not constant/linear in its argument
 B bx=0; // P holds a traced value in boxes
 if(ISZ(T)&&!tr){ // constant subexpression: just evaluate
  if(zg||!ADBOXCHK(f,0,P)){ // result may be traced
   RZ(z=zg?zg:(AO1(f,P))); *pz=z; *tz=zg?ADZ:jtadtgetf(jt,z); R 1;
  }
  bx=1;
 }
 ADASSERT(!ISSPARSE(AT(P))); // sparse argument meeting traced value: the rules handle dense arrays only
 STACKCHKOFL
 if(adisref(f)){A g; ADLOC ls; // is f a name reference?
  RZ(g=jtadresin(jt,f,&ls)); // one step lookup of a name: continue with its value (in its locale); an explicit value is called through the name
  if(g!=f){ // lookup gave something
   I r=ADEXPLV(g)
    ?jtadcallx(jt,f,0,ADZ,P,T,0,pz,tz) // explicit
    :jtadfwd1(jt,g,P,T,k,pz,tz); // anything else (tacit, a primitive, another name)
   jtadlocexit(jt,&ls); // restore the locale
   R r;
  }
 }
 if(!ISZ(T)&&AT(P)&(XNUM|RAT))RZ(P=cvt(FL,P));
 ASSERT(ISZ(T)||AT(P)&(B01|INT|FL),EVDOMAIN); // differentiable arguments must be real numbers
 v=FAV(f); fs=v->fgh[0]; gs=v->fgh[1]; hs=v->fgh[2];
 if(bx){ // P: boxes holding a traced value
  if(ISPRIM(f)){
   if(v->id==CGT||v->id==CRAZE)R jtadboxmap(jt,f,0,P,k,pz,tz,0); // > ;
   if(adconst1(f)){RZ(z=AO1(f,P)); *pz=z; *tz=ADZ; R 1;}
   RZ(z=jtadconstbox(jt,f,0,P)); *pz=z; *tz=adtget(z); R 1;
  }
  ADASSERT(adboxwalk(f));
  {I s=adrankm(f),r=efr(AR(P),s); if(r<AR(P))ADNEGCELLS(I,f,s<0,0,jtadcells1(jt,g_,r,P,ADZ,k,pz,tz))}
  goto boxwalk;
 }
 if(!tr&&adlinear1(f)&&!(k>1&&jtadbondmp(jt,f))){ // linear: tangent map is f itself
  RZ(z=zg?zg:(AO1(f,P)));
  *pz=z; RZ(*tz=admap1(f,T,AR(P)));
  R 1;
 }
 if(!ISPRIM(f)){ // compound verb: honor its rank, as whole-array operations where possible
  I r=efr(AR(P),adrankm(f));
  if(r<AR(P)&&!adatomic1(f)){ADVAL l; // l: the result of the lifting, primal and tangent
   if(!tr&&adconst1(f)){RZ(z=zg?zg:(AO1(f,P))); *pz=z; *tz=ADZ; R 1;} // zero derivative: run it on the whole array, no cell loop
   RZ(jtadliftb1(jt,0,k,f,(ADVAL){P,T},0,&l)); // push the rank down through the verb's parts
   *pz=l.p; *tz=l.t;
   R 1;
  }
 }
boxwalk:
 switch(v->id){
 // scalar primitives: tangent = derivative * T
 case CDIV:    RZ(z=zg?zg:recip(P)); RZ(F=negate(tymes(z,z))); break;  // %y
 case CEXP:    RZ(z=zg?zg:expn1(P)); F=z; break;                       // ^y
 case CLOG:    RZ(z=zg?zg:logar1(P)); RZ(F=recip(P)); break;           // ^.y
 case CSQRT:   RZ(z=zg?zg:sqroot(P)); RZ(F=divide(scf(0.5),z)); break; // %:y
 case CSTARCO: RZ(z=zg?zg:tymes(P,P)); RZ(F=tymes(num(2),P)); break;   // *:y
 case CSTILE:  RZ(z=zg?zg:mag(P)); RZ(F=signum(P)); break;             // |y
 case CNOT:    RZ(z=zg?zg:(AO1(f,P))); *pz=z; RZ(*tz=adneg(T)); R 1;   // -.y = 1-y
 case CLE: case CGE: RZ(z=zg?zg:(AO1(f,P))); *pz=z; *tz=T; R 1;        // <:y >:y
 // piecewise-constant or integer-valued: zero tangent
 case CSTAR: case CFLOOR: case CCEIL: case CEQ: case CNE: case CPOUND: case CDOLLAR: case CIOTA: case CICO:
 case CGRADE: case CDGRADE: case CICAP: case CEPS: case CLDOT: case CFCONS: // * <. >. = ~: # $ i. i: /: \: I. e. L. 0: 1: _: etc.
  RZ(z=zg?zg:(AO1(f,P))); *pz=z; *tz=ADZ; R 1;
 case CABASE: // #: y: binary digits, piecewise constant for integer y
  ADASSERT(AT(P)&(B01|INT));
  RZ(z=zg?zg:(AO1(f,P))); *pz=z; *tz=ADZ; R 1;
 case CNUB: {A s; // ~.y = (~:y)#y
  RZ(z=zg?zg:(AO1(f,P))); *pz=z; RZ(s=(AO1(ds(CNE),P)));
  RZ(*tz=admap2(ds(CPOUND),s,1,T,TR(T))); R 1;}
 case CDOMINO: {A mp; // %.y for square y: d(inv y) = - (inv y) mp dy mp (inv y)
  if(AR(P)>2){ADASSERT(!zg); R jtadcells1(jt,f,2,P,T,k,pz,tz);} // %. has rank 2: a matrix at a time
  if(AR(P)!=2||AS(P)[0]!=AS(P)[1]){A p; ADASSERT(!zg); RZ(z=AO1(f,P)); // atom, list, tall matrix
   RZ(jtadpinvf(jt,P,T,k,&p,tz)); *pz=z; R 1;
  }
  RZ(z=zg?zg:(AO1(f,P))); RZ(mp=jtadmp(jt));
  RZ(a=admap2(mp,T,2,z,2)); RZ(a=admap2(mp,z,2,a,2));
  *pz=z; RZ(*tz=negate(a)); R 1;}
 case CDOT: {A s; // -/ . * y is determinant:  d det y = (det y) * +/ , (|: %. y) * dy
  if(adinnerid(f)==CMINUS&&AR(P)>2){ADASSERT(!zg); R jtadcells1(jt,f,2,P,T,k,pz,tz);} // rank 2: a matrix at a time
  if(adinnerid(f)==CMINUS&&AR(P)<2){
   if(!AR(P)){RZ(z=zg?zg:(AO1(f,P))); *pz=z; *tz=T; R 1;}
   ADASSERT(!zg);
   R jtadfwd1(jt,slash(ds(CMINUS)),P,T,k,pz,tz);
  }
  ADASSERT(adinnerid(f)==CMINUS&&AR(P)==2&&AS(P)[0]==AS(P)[1]);
  RZ(z=zg?zg:(AO1(f,P))); *pz=z;
  RZ(a=jtaddetfac(jt,P,z)); // z * |: %. P, or (singular P) |: adj P
  RZ(s=admap2(ds(CSTAR),a,2,T,2)); RZ(s=admap1(ds(CCOMMA),s,2)); RZ(s=admap1(slash(ds(CPLUS)),s,1));
  *tz=s; R 1;}
 // trains and compositions: chain rule
 case CFORK:
  if(!hs){ADF1(gs,P,T,&b,&tb); R jtadfwd1(jt,fs,b,tb,k,pz,tz);} // capped fork [: f g is stored as f,g,0
  ADF1(hs,P,T,&b,&tb);
  if(AT(fs)&NOUN){a=fs; ta=adtget(fs);
  }else ADF1(fs,P,T,&a,&ta);
  R jtadfwd2(jt,gs,a,ta,b,tb,k,pz,tz);
 case CHOOK:
   ADF1(gs,P,T,&b,&tb); R jtadfwd2(jt,fs,P,T,b,tb,k,pz,tz); // y f (g y)
 case CAT: case CATCO:
  if(AT(gs)&NOUN)R jtadfwd1(jt,fs,gs,jtadtgetf(jt,gs),k,pz,tz); // u@n y is u n (n may be traced: *:@y in explicit code)
  ADF1(gs,P,T,&b,&tb); R jtadfwd1(jt,fs,b,tb,k,pz,tz); // u@v y and u@:v y
 case CAMP: case CAMPCO: // u&v y  u&:v y
  if(AT(fs)&NOUN)R jtadfwd2(jt,gs,fs,adtget(fs),P,T,k,pz,tz); // m&v
  if(AT(gs)&NOUN)R jtadfwd2(jt,fs,P,T,gs,adtget(gs),k,pz,tz); // u&n
  ADF1(gs,P,T,&b,&tb); R jtadfwd1(jt,fs,b,tb,k,pz,tz);
 case CQQ:
  if(AT(fs)&NOUN){ // constant verb m"n
   ADASSERT(!(AT(fs)&BOX)); // gerund m not supported
   ADASSERT(ISZ(adtget(fs))); // m traced (explicit code as s"0 with s =. +/ y): not implemented yet
   RZ(z=zg?zg:(AO1(f,P))); *pz=z; *tz=ADZ; R 1;
  }
  R jtadfwd1(jt,fs,P,T,k,pz,tz); // u"n y  u"v y: the ranks were handled above so this is u y
 case CTILDE:
  ADASSERT(AT(fs)&VERB); // a name reference left unresolved above (should not happen)
  R jtadfwd2(jt,fs,P,T,P,T,k,pz,tz); // u~ y = y u y
 case CCOLONE: ADASSERT(!ADPSEUDO(f)); R jtadcallx(jt,f,0,ADZ,P,T,0,pz,tz); // explicit definition: run it, tracing
 case CDCAPDOT: ADASSERT(ISDCAP(f)); R jtadcallx(jt,f,0,ADZ,P,T,0,pz,tz); // u D. n y: the inner D. runs and this level differentiates its arithmetic
 case CADVERSE: // u :: v - differentiate u; if u fails as J's u does, v
  {I ok; WITHDEBUGOFF(ok=jtadfwd1(jt,fs,P,T,k,pz,tz);) if(ok)R 1;} // as J's :: runs u
  if(!jtadadvfails(jt))R 0;
  if(AT(gs)&NOUN){*pz=gs; *tz=adtget(gs); R 1;}
  R jtadfwd1(jt,gs,P,T,k,pz,tz);
 case CCOLON:
  R jtadfwd1(jt,fs,P,T,k,pz,tz); // u : v y
 case CMCAP: R jtadfwd1(jt,fs,P,T,k,pz,tz); // u M. y: u itself
 case COBVERSE: R jtadfwd1(jt,fs,P,T,k,pz,tz); // u :. v y is u y
 case CBSLASH: case CBSDOT: // u\ y  u\. y (non-linear u)
  ADASSERT(AT(fs)&VERB);
  if(ADSCANMM(fs)&&AR(P)&&AS(P)[0])R jtadfwdscanmm(jt,f,v->id==CBSLASH,P,T,zg,pz,tz); // >./\ <./\ >./\. <./\.
  if(ADSCANP(fs)&&AR(P)&&AS(P)[0])R jtadfwdscanp(jt,f,v->id==CBSLASH,P,T,zg,pz,tz); // */\ */\.
  if(v->id==CBSDOT&&AR(P)&&AS(P)[0]&&!zg&&ADSUFFOLD(fs))R jtadsuffold(jt,fs,P,T,k,pz,tz); // g/\. y
  R jtadscan(jt,fs,v->id==CBSLASH?1:2,P,T,k,pz,tz);
 case CATDOT: ADASSERT(ADISAGENDA(v)); R jtadagendaf(jt,f,0,ADZ,P,T,k,pz,tz); // m@.v y
 case CSLASH: {I n; // u/ y (non-linear u)
  if(!AR(P)){*pz=P; *tz=T; R 1;} // u/ of an atom is the atom itself
  n=AS(P)[0];
  if(!n){RZ(z=zg?zg:(AO1(f,P))); *pz=z; *tz=ADZ; R 1;} // u/ '' is u's identity element
  if(!bx&&adfoldfast(f)){A G; // fast path if f is one of */ >./ <./ (not for single boxed item)
   RZ(z=zg?zg:(AO1(f,P))); // u/ P
   RZ(G=jtadfoldw(jt,f,P,z)); // the weights G, shape $P, one weight per atom of P.
   *pz=z;
   RZ(*tz=admap1(slash(ds(CPLUS)),admap2(ds(CSTAR),G,AR(G),T,TR(T)),AR(P))); R 1; // +/"r G *"r T
  }
  // General case: fold item by item.
  RZ(a=from(sc(n-1),P)); RZ(ta=jtaditem(jt,T,n-1));
  DQ(n-1, A p,t; RZ(p=from(sc(i),P)); RZ(t=jtaditem(jt,T,i)); ADF2(fs,p,t,a,ta,&a,&ta);)
  *pz=a; *tz=ta; R 1;}
 case CPOWOP: {I n; // u^:n y
  ADASSERT(AT(gs)&NOUN);
  RE(n=jtadpowcount(jt,gs));
  a=P; ta=T;
  DQ(n, ADF1(fs,a,ta,&a,&ta););
  *pz=a; *tz=ta; R 1;}
 case CUNDER: case CUNDCO: {A vi,m; // u&.v y  u&.:v y
  I su=jtadsunder(jt,f,&m);
  if(su){ADF1(gs,P,T,&a,&ta); ADF1(fs,a,ta,&b,&tb);   // a structural under: v's result put back
   if(su==1){ADF1(ds(CCOMMA),b,tb,&b,&tb); R jtadfwd2(jt,ds(CDOLLAR),shape(P),ADZ,b,tb,k,pz,tz);}
   if(su==3){
    ASSERT(AR(b)==AR(a),EVRANK); ASSERT(!memcmp(AS(b),AS(a),AR(a)*SZI),EVLENGTH); // u keeps the shape
    RZ(m=AO1(m,apv(AS(P)[0],0,1))); // the items v selected: v on their indices (a constant)
   }
   R jtadfwd2(jt,amend(m),b,tb,P,T,k,pz,tz);
  }
  RZ(vi=inv(gs));
  ADF1(gs,P,T,&a,&ta); ADF1(fs,a,ta,&b,&tb);
  R jtadfwd1(jt,vi,b,tb,k,pz,tz);}
 ADFOLDCASES: ADASSERT(!zg); R jtadfold(jt,f,0,ADZ,P,T,k,0,pz,tz); // u F.. v y and the other folds, step by step
 case CCUT: // u;.0 y : u of y reversed on every axis
  ADASSERT(adcut0(f)&&!zg);
  if(!AR(P))R jtadfwd1(jt,fs,P,T,k,pz,tz);
  RZ(a=jtadcutrevx(jt,P)); R jtadfwd2(jt,f,a,ADZ,P,T,k,pz,tz);
 case CBOX: {A q; // < y
  RZ(q=adtreg(P,T)); RZ(z=zg?zg:(AO1(f,q))); *pz=z; *tz=ADZ; R 1;}
 default: ADASSERT(0); // no derivative rule for this verb
 }
 *pz=z; RZ(*tz=adscale(F,T)); R 1;
}

// ****************************************** dyad rules ******************************************

static B adnormal(A w){I n=AN(w);
 if(AT(w)&FL){D*v=DAV(w); DO(n, D a=v[i]<0?-v[i]:v[i]; if(!(a>=DBL_MIN&&a<inf))R 0;)} // NaN fails too
 else if(AT(w)&INT){I*v=IAV(w); DO(n, if(!v[i])R 0;)}
 else if(AT(w)&B01){B*v=BAV(w); DO(n, if(!v[i])R 0;)}
 else R 0;
 R 1;
}
// x %: y = y^(1%x), with respect to y:  (z % y) % x  else  (y^(1%x)-1) % x  (at y=0 the cheap form is 0%0 = 0 in J,
// while the true slope is _ (x>1), 1 (x=1), 0 (0<x<1) or __ (x<0) which 0^p gives).
static A jtadrootfy(J jt,A x,A y,A z){
 if(adnormal(z)&&adnormal(y)&&adnormal(x))R divide(divide(z,y),x);
 R divide(expn2(y,minus(recip(x),num(1))),x);
}
// x ^ y, with respect to x:  y * z % x  else  y * x^(y-1)  (at x=0, z % x is 0%0 or _%0)
static A jtadpowfx(J jt,A x,A y,A z){
 if(adnormal(z)&&adnormal(x))R tymes(y,divide(z,x));
 R tymes(y,expn2(x,minus(y,num(1))));
}

// solve  y mp t = c  for every direction of c (shape k,$z, z the solution of x %. y) with a single %.
// the directions become extra columns of the right side, so y is factored once
static A jtadsolvek(J jt,A c,A y,A z){A t; I n=AS(y)[0],k=AS(c)[0];
 if(!n||!AN(c))R c;
 RZ(t=cant2(num(0),c)); // direction axis last: ($z),k
 RZ(t=reshape(v2(n,AN(t)/n),t));
 RZ(t=AO2(ds(CDOMINO),t,y)); // one solve
 RZ(t=reshape(over(shape(z),sc(k)),t));
 R cant2(apv(AR(z),0,1),t); // direction axis first again
}

// x <. y and x >. y
// The share of the derivative that goes to x: 1 where x is the one selected, 0.5 on a tie (both are
// selected so that x (<. + >.) y has derivative 1 1 like x + y), 0 where y is selected.
// Exact comparison: selected value is the smaller (larger) one exactly.
static A jtadselx(J jt,C id,A x,A y){A c,d;
 PUSHCCT(1.0) c=AO2(ds(id==CMIN?CLE:CGE),x,y); d=c?AO2(ds(id==CMIN?CLT:CGT),x,y):0; POPCCT
 RZ(d);
  B*cv=BAV(c),*dv=BAV(d);
  if(!memcmp(cv,dv,AN(c)))R c; // no tie: c, boolean
  A z; GATV(z,FL,AN(c),AR(c),AS(c)); D*zv=DAV(z);
  DO(AN(c), zv[i]=0.5*(cv[i]+dv[i]);) R z;
}

// x -. y
static A jtadlessmask(J jt,A x,A y){I ar=AR(x),wr=AR(y),r=MAX(1,ar); A m,s;
 if(ar>1+wr||(ar&&(!AS(x)[0]||!(wr?AS(y)[0]:1))))R reshape(sc(AS(x)[0]),num(1)); // nothing removed: keep every item (here ar>0)
 if(ar==wr+1){ // y is one item
  RZ(m=wr?AO2(qq(ds(CMATCH),sc(wr)),x,y):AO2(ds(CEQ),x,y));
  R minus(num(1),m);
 }
 if(wr!=r){I n=1,k=1+wr-r; // y as a list of cells of rank r-1
  DO(k, n*=AS(y)[i];); RZ(s=over(sc(n),drop(sc(k),shape(y)))); RZ(y=reshape(s,ravel(y)));
 }
 RZ(m=AO2(ds(CEPS),x,y)); RZ(m=minus(num(1),m));
 R ar?m:ravel(m);
}

// 1 when u;.n with n = 3 or _3 else 0
static I adcut3(A f){A g;
 if(FAV(f)->id!=CCUT)R 0;
 g=FAV(f)->fgh[1];
 if(!g||!(AT(g)&INT)||AR(g))R 0;
 I n=IAV(g)[0];
 R n==3||n==-3?n:0;
}

// derivative factors of x o. y with respect to y for atomic x.  z = x o. y, used only by codes 0 3 4 7 _4 (may be 0 for the others)
static A jtadcirclefactor(J jt,I c,A y,A z){
#define CIRC(n,y) AO2(ds(CCIRCLE),num(n),(y))
 switch(c){
 case  0: R negate(divide(y,z));              // sqrt(1-y^2)
 case  1: R CIRC(2,y);                        // sin
 case  2: R negate(CIRC(1,y));                // cos
 case  3: R plus(num(1),tymes(z,z));          // tan
 case  4: R divide(y,z);                      // sqrt(1+y^2)
 case  5: R CIRC(6,y);                        // sinh
 case  6: R CIRC(5,y);                        // cosh
 case  7: R minus(num(1),tymes(z,z));         // tanh
 case -1: R recip(CIRC(0,y));                 // arcsin
 case -2: R negate(recip(CIRC(0,y)));         // arccos
 case -3: R recip(plus(num(1),tymes(y,y)));   // arctan
 case -4: R divide(y,z);                      // sqrt(y^2-1)
 case -5: R recip(CIRC(4,y));                 // arcsinh
 case -6: R recip(CIRC(-4,y));                // arccosh
 case -7: R recip(minus(num(1),tymes(y,y)));  // arctanh
 case  9: R num(1);                           // real part
 case -9: case -10: R num(1);                 // y itself, the conjugate: y, for real y
 case 10: R signum(y);                        // magnitude
 case 11: case 12: R num(0);                  // imaginary part, angle (real y)
 case 8: case -8: case -11: case -12: ASSERT(0,EVDOMAIN);
 default: ADASSERT(0);
 }
#undef CIRC
}
// factor dz/dy of z = x o. y for array of constant function codes x (agreement of x and y)
// Each code's factor is computed on its own atoms only. They are gathered from y (and z for the factors written with z),
// the factors of all codes joined and put back.
static A jtadcirclefactorv(J jt,A x,A y,A z){A Z0,xe,ye,yr,zr,O,V,P,G; I n,*xv,*ov,*vv,cnt[25]={0},st[25],pos[25],np=0;
 RZ(Z0=reshape(shape(z),num(0)));
 RZ(xe=jtatomic2(jt,x,Z0,ds(CPLUS))); RZ(xe=cvt(INT,xe)); // code of each result atom
 n=AN(xe); xv=IAV(xe);
 if(!n)R reshape(shape(z),scf(0.0)); // no atoms: no factors
 RZ(ye=plus(y,reshape(shape(z),scf(0.0)))); // y at each result atom (recorded: y may be traced)
 RZ(yr=ravel(ye)); RZ(zr=ravel(z));
 DO(n, I c=xv[i]; ADASSERT(c>=-12&&c<=12); ++cnt[c+12];) // atoms per code
 {I s=0; DO(25, st[i]=s; pos[i]=s; s+=cnt[i]; np+=cnt[i]>0;)}
 if(np==1){I c=xv[0]; A F; // one code (x is list of the same code): its factor on entire array
  if(c==11||c==12)R reshape(shape(z),scf(0.0));
  RZ(F=jtadcirclefactor(jt,c,ye,z)); R AR(F)?F:reshape(shape(z),F);
 }
 GATV0(O,INT,n,1); ov=IAV(O); DO(n, ov[pos[xv[i]+12]++]=i;) // the atom positions grouped by code, in code order
 RZ(P=jtadboxlist(jt,np));
 {I k=0;
  for(I c=-12;c<=12;++c){I m=cnt[c+12]; A ix,yc,zc,fc;
   if(!m)continue; // absent
   RZ(ix=vec(INT,m,ov+st[c+12])); // c's positions
   if(c==11||c==12){RZ(fc=reshape(sc(m),scf(0.0)));} // 0 for real y
   else{RZ(yc=from(ix,yr)); zc=0;
    if(c==0||c==3||c==4||c==7||c==-4)RZ(zc=from(ix,zr)); // for the factors written with z
    RZ(fc=jtadcirclefactor(jt,c,yc,zc));
    if(!AR(fc))RZ(fc=reshape(sc(m),fc));
   }
   adboxset(P,k,fc); ++k;
  }
 }
 RZ(G=AO1(ds(CRAZE),P)); // the factors in code order
 GATV0(V,INT,n,1); vv=IAV(V); DO(n, vv[ov[i]]=i;) // inverse order: atom i's factor is at V[i] in G
 R reshape(shape(z),from(V,G));
}

static I jtadfwd2x(J,A,A,A,A,A,I,A*,A*);
// entry point for differentiating dyad x f y in forward mode
static I jtadfwd2(J jt,A f,A x,A Tx,A y,A Ty,I k,A*pz,A*tz){B rp=ADT&&ADT->zgiven;
 if(!jtadfwd2x(jt,f,x,Tx,y,Ty,k,pz,tz)){
  if(rp)R 0;
  RZ(jtadfwdnorule(jt,f,x,Tx,y,Ty,pz,tz));
 }
 if(!ISZ(*tz)&&((*pz==x&&ISZ(Tx)&&ISZ(adtget(x)))||(*pz==y&&ISZ(Ty)&&ISZ(adtget(y)))))RZ(*pz=ca(*pz));
 R 1;
}
// x #. y for lists or atoms x and y with x traced (and possibly y): +/ y * }. */\. x , 1
static I jtadbasefx(J jt,A x,A Tx,A y,A Ty,I k,A zg,A*pz,A*tz){A z,a,ta,b,tb; I n=AR(y)?AS(y)[0]:1;
 RZ(z=zg?zg:(AO2(ds(CBASE),x,y)));
 // a is bases as list: x itself or n $ x for atom base.
 if(AR(x)){a=x; ta=Tx;
 }else ADF2(ds(CDOLLAR),sc(n),ADZ,x,Tx,&a,&ta);
 // weights
 ADF2(ds(CCOMMA),a,ta,num(1),ADZ,&a,&ta); ADF1(bsdot(slash(ds(CSTAR))),a,ta,&a,&ta); ADF1(ds(CBEHEAD),a,ta,&a,&ta); // }. */\. x , 1
 ADF2(ds(CSTAR),y,Ty,a,ta,&b,&tb); ADF1(slash(ds(CPLUS)),b,tb,&b,&tb); // +/ y * weights
 *pz=z; *tz=tb; R 1;
}
static I jtadfwd2x(J jt,A f,A x,A Tx,A y,A Ty,I k,A*pz,A*tz){A z,F,a,b,c,ta,tb,tc; V*v; A fs,gs,hs; I lrk,rrk;
 A zg=0; if(ADT&&ADT->zgiven){zg=ADT->zgiven; ADT->zgiven=0;}
 ASSERT(AT(f)&VERB,EVDOMAIN);
 B tr=jtadhastraced(jt,f); // f holds a traced noun as an operand: not constant even with constant arguments
 B bx=0; // argument holds a traced value in boxes
 if(ISZ(Tx)&&ISZ(Ty)&&!tr){ // constant subexpression: just evaluate
  if(zg||!ADBOXCHK(f,x,y)){
   RZ(z=zg?zg:(AO2(f,x,y)));
   *pz=z; *tz=zg?ADZ:jtadtgetf(jt,z); R 1;
  }
  bx=1;
 }else if(!zg&&!jtadrunsown(jt,f)&&(ADBOXHOLDS(x)||ADBOXHOLDS(y)))bx=1; // one traced, the other boxes holding a traced value
 ADASSERT(!ISSPARSE(AT(x)|AT(y))); // sparse argument
 STACKCHKOFL
 if(adisref(f)){A g; ADLOC ls; // is f a name reference?
  RZ(g=jtadresin(jt,f,&ls)); // one step lookup of a name
  if(g!=f){ // lookup gave something
   I r=ADEXPLV(g)
    ?jtadcallx(jt,f,x,Tx,y,Ty,1,pz,tz) // explicit
    :jtadfwd2(jt,g,x,Tx,y,Ty,k,pz,tz); // anything else
   jtadlocexit(jt,&ls); // restore the locale
   R r;
  }
 }
 if(!ISZ(Tx)&&AT(x)&(XNUM|RAT))RZ(x=cvt(FL,x)); if(!ISZ(Ty)&&AT(y)&(XNUM|RAT))RZ(y=cvt(FL,y));
 ASSERT((ISZ(Tx)||AT(x)&(B01|INT|FL))&&(ISZ(Ty)||AT(y)&(B01|INT|FL)),EVDOMAIN); // differentiable arguments must be real numbers
 v=FAV(f); fs=v->fgh[0]; gs=v->fgh[1]; hs=v->fgh[2];
 if(bx){
  if(ISPRIM(f)){B by=ADBOXHOLDS(y);
   RZ(x=adtreg(x,Tx)); RZ(y=adtreg(y,Ty)); // a traced argument (the other holds boxes) as a traced value
   if(v->id==CFETCH&&by)R jtadboxmap(jt,f,x,y,k,pz,tz,0); // x {:: y
   if(adconst2(f)||(!by&&ISZ(Ty)&&(v->id==CFETCH||v->id==CFROM))){
    RZ(z=AO2(f,x,y)); *pz=z; *tz=ADZ; R 1;
   }
   RZ(z=jtadconstbox(jt,f,x,y));
   *pz=z; *tz=adtget(z); R 1;
  }
  ADASSERT(adboxwalk(f));
  {I sl=adrankl(f),sr=adrankr(f); lrk=efr(AR(x),sl); rrk=efr(AR(y),sr);
   if(lrk<AR(x)||rrk<AR(y))ADNEGCELLS(I,f,sl<0||sr<0,1,jtadcells2(jt,g_,lrk,rrk,x,Tx,y,Ty,k,pz,tz))
  }
  goto boxwalk2;
 }
 if(!ISPRIM(f)){ // compound verb
  lrk=efr(AR(x),adrankl(f)); rrk=efr(AR(y),adrankr(f));
  if((lrk<AR(x)||rrk<AR(y))&&!adatomic2(f)){ADVAL l; // l: the result of the lifting, primal and tangent
   if(!tr&&adconst2(f)){ // zero derivative: on the whole arrays
    RZ(z=zg?zg:(AO2(f,x,y))); *pz=z; *tz=ADZ; R 1;
   }
   RZ(jtadliftb2(jt,0,k,f,(ADVAL){x,Tx},0,(ADVAL){y,Ty},0,&l));
   *pz=l.p; *tz=l.t;
   R 1;
  }
 }
boxwalk2:
 if(ISPRIM(f)&&adatomic2(f)&&(!AN(x)||!AN(y))){ // a scalar dyad with an empty result
  RZ(z=zg?zg:(AO2(f,x,y))); *pz=z; *tz=ADZ; R 1;
 }
 switch(v->id){
 // scalar primitives
 case CPLUS:  RZ(z=zg?zg:plus(x,y));  *pz=z; RZ(*tz=adsum(Tx,Ty,z)); R 1;
 case CMINUS: RZ(z=zg?zg:minus(x,y)); *pz=z; RZ(*tz=adsum(Tx,adneg(Ty),z)); R 1;
 case CSTAR:  RZ(z=zg?zg:tymes(x,y)); *pz=z; RZ(a=adscale(y,Tx)); RZ(b=adscale(x,Ty)); RZ(*tz=adsum(a,b,z)); R 1;
 case CDIV: // d(x%y) = dx%y - (z%y)*dy
  RZ(z=zg?zg:divide(x,y)); *pz=z;
  a=ADZ; if(!ISZ(Tx))RZ(a=adscale(recip(y),Tx));
  b=ADZ; if(!ISZ(Ty))RZ(b=adscale(negate(divide(z,y)),Ty));
  RZ(*tz=adsum(a,b,z)); R 1;
 case CEXP: // d(x^y) = y*x^(y-1) dx + z*ln(x) dy
  RZ(z=zg?zg:expn2(x,y)); *pz=z;
  a=ADZ; if(!ISZ(Tx))RZ(a=adscale(jtadpowfx(jt,x,y,z),Tx)); // y * z % x or y * x^(y-1)
  b=ADZ; if(!ISZ(Ty))RZ(b=adscale(tymes(z,logar1(x)),Ty));
  RZ(*tz=adsum(a,b,z)); R 1;
 case CLOG: // x^.y = ln y % ln x
  RZ(z=zg?zg:(AO2(f,x,y))); *pz=z;
  a=ADZ; if(!ISZ(Tx))RZ(a=adscale(negate(divide(z,tymes(x,logar1(x)))),Tx));
  b=ADZ; if(!ISZ(Ty))RZ(b=adscale(recip(tymes(y,logar1(x))),Ty));
  RZ(*tz=adsum(a,b,z)); R 1;
 case CROOT: // x%:y = y^(1%x)
  RZ(z=zg?zg:(AO2(f,x,y))); *pz=z;
  a=ADZ; if(!ISZ(Tx))RZ(a=adscale(negate(divide(tymes(z,logar1(y)),tymes(x,x))),Tx));
  b=ADZ; if(!ISZ(Ty))RZ(b=adscale(jtadrootfy(jt,x,y,z),Ty)); // (z % y) % x or (y^(1%x)-1) % x
  RZ(*tz=adsum(a,b,z)); R 1;
 case CMIN: case CMAX: // the tangent of whichever argument was selected
  RZ(z=zg?zg:(AO2(f,x,y))); *pz=z;
  RZ(c=jtadselx(jt,v->id,x,y));
  RZ(a=adscale(c,Tx)); RZ(b=adscale(minus(num(1),c),Ty));
  RZ(*tz=adsum(a,b,z)); R 1;
 case CSTILE: // x|y = y - x * <.y%x
  RZ(z=zg?zg:(AO2(f,x,y))); *pz=z;
  a=ADZ; if(!ISZ(Tx))RZ(a=adscale(divide(minus(z,y),x),Tx)); // d/dx = (z-y)%x
  RZ(*tz=adsum(a,Ty,z)); R 1;
 case CCIRCLE: // x o. y
  ADASSERT(ISZ(Tx)); // function codes must be constant
  if(AR(x)){
   RZ(z=zg?zg:(AO2(f,x,y))); *pz=z;
   RZ(F=jtadcirclefactorv(jt,x,y,z));
   RZ(*tz=adexpand(adscale(F,Ty),z)); R 1;
  }
  {RZ(z=zg?zg:(AO2(f,x,y))); *pz=z;
   if(!AN(z)){*tz=ADZ; R 1;}
   I code=i0(x); RE(0);
   if(code==11||code==12){*tz=ADZ; R 1;} // constant for real y
   RZ(F=jtadcirclefactor(jt,code,y,z));
   RZ(*tz=adexpand(adscale(F,Ty),z)); R 1;}
 // piecewise-constant or integer-valued
 case CEQ: case CNE: case CLT: case CLE: case CGE: case CGT: case CEPS: case CEBAR:
 case CIOTA: case CICO: case CICAP: case CMATCH: case CFCONS: // = ~: < <: >: > e. E. i. i: I. -: 0: 1: _: etc.
  RZ(z=zg?zg:(AO2(f,x,y))); *pz=z; *tz=ADZ; R 1;
 // structural: linear in y, x selects
 case CFROM: case CTAKE: case CDROP: case CPOUND: case CDOLLAR: case CROT: case CCANT: // { {. }. # $ |. |:
  RZ(z=zg?zg:(AO2(f,x,y))); *pz=z;
  *tz=ISZ(Ty)?ADZ:admap2(f,x,AR(x),Ty,TR(Ty)); RZ(*tz); R 1;
 case CBASE: // x #. y: linear in y
  if(!ISZ(Tx)){ // x traced
   if(!AR(x)){A z; RZ(z=zg?zg:(AO2(f,x,y))); // atom base: the polynomial (|."1 y) p. x
    ADF1(qq(ds(CREV),num(1)),y,Ty,&a,&ta); ADF2(ds(CPOLY),a,ta,x,Tx,&b,&tb);
    *pz=z; *tz=tb; R 1;
   }
   if(AR(x)>1||AR(y)>1)R jtadcells2(jt,f,1,MIN(1,AR(y)),x,Tx,y,Ty,k,pz,tz); // cell loop, but cell of y may be an atom
   R jtadbasefx(jt,x,Tx,y,Ty,k,zg,pz,tz); // lists or a list and an atom
  }
  RZ(z=zg?zg:(AO2(f,x,y))); *pz=z; RZ(*tz=admap2(f,x,AR(x),Ty,TR(Ty))); R 1; // only y traced
 // structural: linear in x, y selects
 case CGRADE: case CDGRADE: // x /: y  x \: y  permute x
  RZ(z=zg?zg:(AO2(f,x,y))); *pz=z;
  *tz=ISZ(Tx)?ADZ:admap2(f,Tx,TR(Tx),y,AR(y)); RZ(*tz); R 1;
 case CLESS: {A s; // x -. y
  RZ(z=zg?zg:(AO2(f,x,y))); *pz=z;
  if(ISZ(Tx)){*tz=ADZ; R 1;}
  RZ(s=jtadlessmask(jt,x,y));
  RZ(*tz=admap2(ds(CPOUND),s,1,Tx,TR(Tx))); R 1;}
 // structural: jointly linear
 case CCOMMA: case CCOMDOT: case CLAMIN: // x , y  x ,. y  x ,: y
  RZ(z=zg?zg:(AO2(f,x,y))); *pz=z;
  RZ(*tz=admap2(f,admat(Tx,x),AR(x),admat(Ty,y),AR(y))); R 1;
 case CLEFT:  *pz=x; *tz=Tx; R 1; // x [ y
 case CRIGHT: *pz=y; *tz=Ty; R 1; // x ] y
 // products
 case CDOT:
  ADASSERT(adinnerid(f)); // u/ . * with u = + or - is bilinear, d(x u/ .* y) = (dx u/ .* y) + x u/ .* dy
  RZ(z=zg?zg:(AO2(f,x,y))); *pz=z;
  if(k>1&&AR(x)&&AR(y)){I n=AS(x)[AR(x)-1],na=n?AN(x)/n:0,nb=n?AN(y)/n:0; // k directions: one product for all of them
   if(n&&na&&nb){A X,Y,T; a=b=ADZ; // x as (na,n), y as (n,nb): product contracts last axis of x with first of y
    RZ(X=reshape(v2(na,n),cvt(FL,x))); RZ(Y=reshape(v2(n,nb),cvt(FL,y))); // all floating point.
    if(!ISZ(Tx)){
     RZ(T=cvt(FL,adexpand(Tx,x))); RZ(a=AO2(f,reshape(v2(k*na,n),T),Y)); RZ(a=reshape(over(sc(k),shape(z)),a));
    }
    if(!ISZ(Ty)){
     RZ(T=cvt(FL,adexpand(Ty,y))); RZ(T=cant2(over(num(1),v2(0,2)),reshape(over(sc(k),v2(n,nb)),T)));
     RZ(b=AO2(f,X,reshape(v2(n,k*nb),T))); RZ(b=cant2(over(num(1),v2(0,2)),reshape(over(sc(na),v2(k,nb)),b)));
     RZ(b=reshape(over(sc(k),shape(z)),b));
    }
    RZ(*tz=adsum(a,b,z)); R 1;}
  }
  a=ISZ(Tx)?ADZ:admap2(f,Tx,TR(Tx),y,AR(y)); RZ(a);
  b=ISZ(Ty)?ADZ:admap2(f,x,AR(x),Ty,TR(Ty)); RZ(b);
  RZ(*tz=adsum(a,b,z)); R 1;
 case CDOMINO: {A mp; // x %. y for square y:  y mp z = x, so dz = (dx - dy mp z) %. y
  if(AR(y)>2){ADASSERT(!zg); R jtadcells2(jt,f,AR(x),2,x,Tx,y,Ty,k,pz,tz);} // rank _ 2: each matrix of y with the whole x
  if(AR(y)!=2||AS(y)[0]!=AS(y)[1]){A pi,tpi; ADASSERT(!zg); // y an atom, list or tall matrix: (%. y) +/ . * x
   RZ(z=AO2(f,x,y));
   RZ(jtadpinvf(jt,y,Ty,k,&pi,&tpi)); RZ(jtadfwd2(jt,jtadmp(jt),pi,tpi,x,Tx,k,&pi,tz));
   *pz=z; R 1;
  }
  RZ(z=zg?zg:(AO2(f,x,y))); *pz=z; RZ(mp=jtadmp(jt));
  b=ISZ(Ty)?ADZ:admap2(mp,Ty,2,z,AR(z)); RZ(b);
  RZ(c=adsum(Tx,adneg(b),z));
  RZ(*tz=jtadsolvek(jt,c,y,z)); R 1;} // solved with y, one solve for all directions
 case CPOLY: {A pd; // x p. y with coefficient list x
  ADASSERT(!(AT(x)&BOX));
  RZ(z=zg?zg:(AO2(f,x,y))); *pz=z;
  a=ISZ(Tx)?ADZ:admap2(f,Tx,TR(Tx),y,AR(y)); RZ(a);
  b=ADZ; if(!ISZ(Ty)){RZ((pd=AO1(ds(CPDERIV),x))); RZ(F=(AO2(f,pd,y))); RZ(b=adscale(F,Ty));}
  RZ(*tz=adsum(a,b,z)); R 1;}
 // trains and compositions
 case CFORK:
  if(!hs){ADF2(gs,x,Tx,y,Ty,&b,&tb); R jtadfwd1(jt,fs,b,tb,k,pz,tz);} // capped fork
  ADF2(hs,x,Tx,y,Ty,&b,&tb);
  if(AT(fs)&NOUN){a=fs; ta=adtget(fs);
  }else ADF2(fs,x,Tx,y,Ty,&a,&ta);
  R jtadfwd2(jt,gs,a,ta,b,tb,k,pz,tz);
 case CHOOK: ADF1(gs,y,Ty,&b,&tb); R jtadfwd2(jt,fs,x,Tx,b,tb,k,pz,tz); // x f g y
 case CAT: case CATCO:
  if(AT(gs)&NOUN)R jtadfwd1(jt,fs,gs,jtadtgetf(jt,gs),k,pz,tz); // x u@n y is u n
  ADF2(gs,x,Tx,y,Ty,&b,&tb); R jtadfwd1(jt,fs,b,tb,k,pz,tz); // x u@v y  x u@:v  y
 case CAMP: case CAMPCO: // x u&v y  x u&:v y
  ADASSERT(AT(fs)&VERB&&AT(gs)&VERB);
  ADF1(gs,y,Ty,&b,&tb); ADF1(gs,x,Tx,&a,&ta);
  R jtadfwd2(jt,fs,a,ta,b,tb,k,pz,tz);
 case CQQ:
  if(AT(fs)&NOUN){ // x m"n y
   ADASSERT(!(AT(fs)&BOX)); // gerund: not supported
   ADASSERT(ISZ(adtget(fs))); // m traced: not implemented yet
   RZ(z=zg?zg:(AO2(f,x,y))); *pz=z; *tz=ADZ; R 1;
  }
  R jtadfwd2(jt,fs,x,Tx,y,Ty,k,pz,tz); // x u"n y  x u"v y (ranks were handled above so this is x u y)
 case CTILDE:
  ADASSERT(AT(fs)&VERB); // a name reference left unresolved above (should not happen)
  R jtadfwd2(jt,fs,y,Ty,x,Tx,k,pz,tz); // x u~ y = y u x
 case CCOLONE: ADASSERT(!ADPSEUDO(f)); R jtadcallx(jt,f,x,Tx,y,Ty,1,pz,tz); // explicit definition
 case CDCAPDOT: ADASSERT(ISDCAP(f)); R jtadcallx(jt,f,x,Tx,y,Ty,1,pz,tz); // x u D. n y: the inner D. runs and this level differentiates its arithmetic
 case CCUT: // x u;.0 y subarray selected then u on it.  x u;.3 y and x u;._3 y with u linear: the same cut applied to tangent
  if(adcut0(f)){ // x u;.0 y
   if(adcutsel(f)){
    RZ(z=zg?zg:AO2(f,x,y)); *pz=z; *tz=ISZ(Ty)?ADZ:admap2(f,x,AR(x),Ty,TR(Ty));
    RZ(*tz); R 1;
   }
   ADASSERT(!zg); ADF2(jtadcutsel(jt),x,ADZ,y,Ty,&a,&ta); R jtadfwd1(jt,fs,a,ta,k,pz,tz);
  }
  ADASSERT(adcut3(f)&&ISZ(Tx)&&AT(fs)&VERB&&(adstructm(fs)||adlinear1(fs))&&!jtadhastraced(jt,fs)); // check that u holds no traced operand and only moves atoms or is linear
  RZ(z=zg?zg:AO2(f,x,y)); *pz=z;
  *tz=ISZ(Ty)?ADZ:admap2(f,x,AR(x),Ty,TR(Ty)); RZ(*tz); R 1;
 case CADVERSE: // x u :: v y
  {I ok; WITHDEBUGOFF(ok=jtadfwd2(jt,fs,x,Tx,y,Ty,k,pz,tz);) if(ok)R 1;}
  if(!jtadadvfails(jt))R 0;
  if(AT(gs)&NOUN){*pz=gs; *tz=adtget(gs); R 1;} // u :: n
  R jtadfwd2(jt,gs,x,Tx,y,Ty,k,pz,tz);
 case CRBRACE: // x m} y (linear in (x,y) for fixed m)
  ADASSERT(AT(fs)&NOUN);
  RZ(z=zg?zg:(AO2(f,x,y))); *pz=z;
  RZ(*tz=admap2(f,admat(Tx,x),AR(x),admat(Ty,y),AR(y))); R 1;
 case CCOLON: // x u : v y
  ADASSERT(AT(fs)&VERB&&AT(gs)&VERB);
  R jtadfwd2(jt,gs,x,Tx,y,Ty,k,pz,tz);
 case CMCAP: case COBVERSE: R jtadfwd2(jt,fs,x,Tx,y,Ty,k,pz,tz); // x u M. y  x u :. v y
 case CBSLASH: case CBSDOT: // x u\ y  x u\. y
  ADASSERT(AT(fs)&VERB);
  R jtadinfixf(jt,fs,v->id==CBSDOT,x,y,Ty,k,zg,pz,tz);
 case CATDOT: ADASSERT(ADISAGENDA(v)); R jtadagendaf(jt,f,x,Tx,y,Ty,k,pz,tz); // x m@.v y
 case CSLASH: {A g; // x u/ y = x u"(lu,_) y
  RZ(g=qq(fs,v2(lr(fs),RMAX))); // g = u"(lu,_)
  R jtadfwd2(jt,g,x,Tx,y,Ty,k,pz,tz);}
 case CPOWOP: {I n; // x u^:n y = x&u^:n y
  ADASSERT(AT(gs)&NOUN);
  RE(n=jtadpowcount(jt,gs));
  a=y; ta=Ty;
  DQ(n, ADF2(fs,x,Tx,a,ta,&a,&ta););
  *pz=a; *tz=ta; R 1;}
 case CUNDER: case CUNDCO: {A vi; // x u&.v y  x u&.:v y
  RZ(vi=inv(gs));
  ADF1(gs,y,Ty,&b,&tb); ADF1(gs,x,Tx,&a,&ta);
  ADF2(fs,a,ta,b,tb,&c,&tc);
  R jtadfwd1(jt,vi,c,tc,k,pz,tz);}
 case CSEMICO: {A p,q; // x ; y
  if(!ISPRIM(f))break;
  RZ(p=adtreg(x,Tx)); RZ(q=adtreg(y,Ty)); RZ(z=zg?zg:(AO2(f,p,q))); *pz=z; *tz=ADZ; R 1;}
 ADFOLDCASES: ADASSERT(!zg); R jtadfold(jt,f,x,Tx,y,Ty,k,0,pz,tz); // x u F.. v y and other folds
 }
 ADASSERT(0);
}

// ***************************************** reverse mode *****************************************
// The recording pass runs u once.  Every operation whose result depends on y is appended to the tape
// (verb, arguments, result) and its result becomes a node. The backward pass then walks the tape from the
// end, accumulating cotangents.

#define REC1(f,P) jtadrec1(jt,(f),(P))
#define REC2(f,x,y) jtadrec2(jt,(f),(x),(y))

static I jtadtapepush(J jt,I kind,A f,A x,A y,A z){ADTRACE*t=ADT;
 if(t->ntape==t->captape){
  I nc=t->captape?2*t->captape:256; ADENT*ne=realloc(t->tape,nc*sizeof(ADENT)); ASSERT(ne!=0,EVWSFULL);
  t->tape=ne; t->captape=nc;
 }
 if(x)realizeifvirtual(x); if(y)realizeifvirtual(y); // virtual blocks may not be ra()d
 if(f)ra(f); if(x)ra(x); if(y)ra(y); ra(z);
 t->tape[t->ntape++]=(ADENT){f,x,y,z,kind};
 R 1;
}
static A jtadidx(J jt,A y,I base){R reshape(shape(y),apv(AN(y),base,1));} // ($x) $ base + i. # , x
// z was computed from x (and y) by f: make z a node and record the operation. Result is the block to use for z
static A jtadrecord(J jt,I kind,A f,A x,A y,A z){A old;
 if(!z)R 0;
 if(!(AT(z)&NUMERIC))R z; // text and boxes are never nodes
 if(AT(z)&(XNUM|RAT))RZ(z=jtcvt(jt,FL,z)); // nodes are floating point, the copy is a new block, recorded
 realizeifvirtual(z);
 old=adtget(z);
 if(old==ADNODE&&kind==ADKSTRUCT1)R z; // atom-moving subtree passed an existing node through: nothing happened to it
 // primitive with a derivative rule can return its argument too, as %: on a boolean 1 does: that is recorded on a copy so the backward pass applies its derivative
 if(!ISZ(old)||ACISPERM(AC(z))||z==x||z==y)RZ(z=ca(z));
 RZ(jtadtput(jt,z,ADNODE));
 RZ(jtadtapepush(jt,kind,f,x,y,z));
 R z;
}
#define PRIM1(z) jtadrecord(jt,ADKPRIM1,f,P,0,(z))
#define PRIM2(z) jtadrecord(jt,ADKPRIM2,f,x,y,(z))
// result with zero derivative: if the verb returned a node (e.g. <. y of integer y) then use a copy
static A jtaduntr(J jt,A z){if(!z)R 0; R TRC(z)?ca(z):z;}
#define UNTR(e) ({A _z; RZ(e); jtaduntr(jt,_z);})

// J's own value zj with the derivative of zf recorded from a formula equal to it to rounding: an identity STRUCT1 entry
// So the value is J's as in forward mode and J's evaluation is not wasted
static A jtadrecsame(J jt,A zf,A zj){
 if(!zf||!zj)R 0;
 if(!TRC(zf)||AR(zf)!=AR(zj)||memcmp(AS(zf),AS(zj),AR(zj)*SZI))R zf;
 R jtadrecord(jt,ADKSTRUCT1,0,zf,0,zj);
}

// structural (atom-moving) verbs
static B jtadstructxb(J jt,A f){if(!f||!(AT(f)&VERB))R 0; if(!ISPRIM(f))R 0; switch(FAV(f)->id){case CFROM: case CTAKE: case CDROP: case CPOUND: case CDOLLAR: case CROT: case CCANT: R 1; default: R 0;}} // x selects, y moves
static B jtadstructyb(J jt,A f){if(!f||!(AT(f)&VERB))R 0; if(!ISPRIM(f))R 0; switch(FAV(f)->id){case CGRADE: case CDGRADE: R 1; default: R 0;}} // y selects, x moves
static B jtadstructjb(J jt,A f){if(!f||!(AT(f)&VERB))R 0; if(!ISPRIM(f))R 0; switch(FAV(f)->id){case CCOMMA: case CCOMDOT: case CLAMIN: case CLEFT: case CRIGHT: R 1; default: R 0;}} // both move
// monad f only moves atoms (with fill): the same index map applies whatever the values
static B jtadstructmb(J jt,A f){V*v; A fs,gs,hs; if(!f||!(AT(f)&VERB))R 0; v=FAV(f); fs=v->fgh[0]; gs=v->fgh[1]; hs=v->fgh[2];
 if(ISPRIM(f))switch(v->id){
  case CCOMMA: case CCOMDOT: case CLAMIN: case CREV: case CCANT: case CHEAD: case CTAIL: case CBEHEAD: case CCTAIL: case CRAZE: case CLEFT: case CRIGHT: R 1;
  default: R 0;
 }
 switch(v->id){
 case CAT: case CATCO: R adstructm(fs)&&adstructm(gs);
 case CAMP: case CAMPCO:
  if(AT(fs)&NOUN)R adstructx(gs);
  if(AT(gs)&NOUN)R adstructy(fs);
  R adstructm(fs)&&adstructm(gs);
 case CFORK:
  if(!hs)R adstructm(fs)&&adstructm(gs);
  if(AT(fs)&NOUN)R 0;
  R adstructm(fs)&&adstructm(hs)&&adstructj(gs);
 case CHOOK: R adstructj(fs)&&adstructm(gs);
 case CTILDE: R adstructj(fs);
 case CQQ: R AT(fs)&VERB&&adstructm(fs);
 case CSLASH: R AT(fs)&VERB&&adstructj(fs); // ,/ ,./ [/ fold of a dyad that only moves atoms
 default: R 0;
 }
}
ADLOCWRAP(B,adstructx)
ADLOCWRAP(B,adstructy)
ADLOCWRAP(B,adstructj)
ADLOCWRAPS(B,adstructm,0)

// cell loops
// Monad. Differentiate one cell in reverse mode. 0=cell i; 1=prefix i+1 (u\); 2=suffix i (u\.); 3=infix i (x u\ y); 4=outfix i (x u\. y).
static DF1(jtadrcellm){DF12IP;V*v=FAV(self); A f=v->fgh[0],*dv=AAV(v->fgh[1]); A P=C(dv[1]),c,r; I mode=IAV(C(dv[2]))[0], i=IAV(w)[0];
 switch(mode){
 case 0: RZ(c=REC2(ds(CFROM),w,P)); break;
 case 1: RZ(c=REC2(ds(CTAKE),sc(i+1),P)); break;
 case 2: RZ(c=REC2(ds(CDROP),sc(i),P)); break;
 default: {A ix; RZ(ix=jtadwincell(jt,mode==4,IAV(C(dv[2]))[1],AS(P)[0],i)); RZ(c=REC2(ds(CFROM),ix,P));}
 }
 RZ(r=REC1(f,c)); R box(r);
}
static DF2(jtadrcelld){DF12IP;V*v=FAV(self); A f=v->fgh[0],*dv=AAV(v->fgh[1]); A c1,c2,r;
 RZ(c1=REC2(ds(CFROM),a,C(dv[0]))); RZ(c2=REC2(ds(CFROM),w,C(dv[1])));
 RZ(r=REC2(f,c1,c2)); R box(r);
}
// the results of the cells opened together: recorded if a cell result is a node.
static A jtadrassemble(J jt,A zb){A z; B any=0;
 RZ(z=ope(zb));
 if(!(AT(z)&NUMERIC))R z;
 DO(AN(zb), A Ti=adtget(C(AAV(zb)[i])); if(!ISZ(Ti))any=1;)
 if(!any)R z;
 R jtadrecord(jt,ADKASSEMBLE,0,zb,0,z);
}
static A jtadrsplit(J jt,A P,I fr){I n=1; A cs,r;
 DO(fr, n*=AS(P)[i];);
 RZ(cs=drop(sc(fr),shape(P))); RZ(r=REC1(ds(CCOMMA),P));
 R REC2(ds(CDOLLAR),over(sc(n),cs),r);
}
static A jtadrcells1(J jt,A f,I r,A P){A pr,cv,ix,zb,g; I fr=AR(P)-r,n=1;
 DO(fr, n*=AS(P)[i];);
 if(!n)R UNTR((_z=AO1(qq(f,sc(r)),P)));
 RZ(pr=jtadrsplit(jt,P,fr));
 cv=fdef(0,CDCAPDOT,VERB,jtadrcellm,jtvalenceerr,f,jlink(mtv,jlink(pr,num(0))),0,VFLAGNONE,RMAX,RMAX,RMAX);
 RZ(ix=reshape(take(sc(fr),shape(P)),apv(n,0,1)));
 RZ(g=qq(cv,num(0))); RZ(df1(zb,ix,g));
 R jtadrassemble(jt,zb);
}
static A jtadrcells2(J jt,A f,I lr,I rr,A x,A y){A xr,yr,cv,ix,iy,zb,g; I fx=AR(x)-lr,fy=AR(y)-rr,nx=1,ny=1;
 DO(fx, nx*=AS(x)[i];); DO(fy, ny*=AS(y)[i];);
 if(!nx||!ny)R UNTR((_z=AO2(qq(f,v2(lr,rr)),x,y)));
 RZ(xr=jtadrsplit(jt,x,fx)); RZ(yr=jtadrsplit(jt,y,fy));
 cv=fdef(0,CDCAPDOT,VERB,jtvalenceerr,jtadrcelld,f,jlink(xr,jlink(yr,num(0))),0,VFLAGNONE,RMAX,RMAX,RMAX);
 RZ(ix=reshape(take(sc(fx),shape(x)),apv(nx,0,1)));
 RZ(iy=reshape(take(sc(fy),shape(y)),apv(ny,0,1)));
 RZ(g=qq(cv,v2(0,0))); RZ(df2(zb,ix,iy,g)); // frame agreement here
 R jtadrassemble(jt,zb);
}
static A jtadrscan(J jt,A f,I mode,A P){A cv,zb,g; I n;
 if(!AR(P))RZ(P=REC1(ds(CCOMMA),P));
 n=AS(P)[0];
 if(!n)R UNTR((_z=AO1(mode==1?bslash(f):bsdot(f),P))); // no items
 cv=fdef(0,CDCAPDOT,VERB,jtadrcellm,jtvalenceerr,f,jlink(mtv,jlink(P,sc(mode))),0,VFLAGNONE,RMAX,RMAX,RMAX);
 RZ(g=qq(cv,num(0))); RZ(df1(zb,apv(n,0,1),g));
 R jtadrassemble(jt,zb);
}
// m@.v recorded
static A jtadagendar(J jt,A f,A x,A y){A s,gb,gk,inv,*fv=AAV(FAV(f)->fgh[0]),cx=x,cy=y,zr=0; I c,ng;
 RZ(c=jtadagplan(jt,f,x,y,&s,&gb,&gk,&inv));
 if(c==2)R UNTR((_z=x?AO2(f,x,y):AO1(f,y)));
 if(c==1){A g=fv[IAV(s)[0]]; R x?REC2(g,x,y):REC1(g,y);}
 ng=AN(gb);
 if(AR(s)>1){
  if(x&&AR(x)){RZ(cx=REC1(ds(CCOMMA),x)); RZ(cx=REC2(ds(CDOLLAR),jtadagcells(jt,s,x),cx));}
  if(AR(y)){RZ(cy=REC1(ds(CCOMMA),y)); RZ(cy=REC2(ds(CDOLLAR),jtadagcells(jt,s,y),cy));}
 }
 A rb,*rs; I rmax=0; GATV0(rb,INT,ng,1); rs=(A*)IAV(rb);
 DO(ng, A ix=AAV1(gb)[i],a=cx,b=cy,r,g=fv[IAV(gk)[i]];
  if(ng>1){if(x&&AR(x))RZ(a=REC2(ds(CFROM),ix,cx)); if(AR(y))RZ(b=REC2(ds(CFROM),ix,cy));}
  RZ(r=x?REC2(g,a,b):REC1(g,b));
  ASSERT(AR(r)&&AS(r)[0]==AN(ix),EVRANK);
  rs[i]=r; rmax=MAX(rmax,AR(r));
 );
 DO(ng, A r=rs[i];
  if(AR(r)<rmax){RZ(r=REC1(ds(CCOMMA),r)); RZ(r=REC2(ds(CDOLLAR),jtadagraise(jt,rs[i],rmax),r));}
  RZ(zr=zr?REC2(ds(CCOMMA),zr,r):r);
 );
 if(ng>1)RZ(zr=REC2(ds(CFROM),inv,zr));
 if(AR(s)>1)RZ(zr=REC2(ds(CDOLLAR),shape(s),zr));
 R zr;
}
// x u\ y and x u\. y (out) recorded
static A jtadinfixr(J jt,A f,B out,A x,A P){A W,c; I m,n,cn;
 RE(m=i0(x)); // an integer x
 if(!AR(P))RZ(P=REC1(ds(CCOMMA),P)); // atom y is one-item list
 n=AS(P)[0]; cn=adwincount(m,n);
 if(!cn)R UNTR((_z=AO2(out?bsdot(f):bslash(f),x,P))); // no cells
 if((W=jtadwinidx(jt,out,m,n,n?AN(P)/n:1))){ // gather path
  RZ(c=REC2(ds(CFROM),W,P)); R REC1(qq(f,sc(-1)),c);
 }
 RE(0); // error making the table
 // cell loop
 {A cv,zb,g; cv=fdef(0,CDCAPDOT,VERB,jtadrcellm,jtvalenceerr,f,jlink(mtv,jlink(P,v2(out?4:3,m))),0,VFLAGNONE,RMAX,RMAX,RMAX);
  RZ(g=qq(cv,num(0))); RZ(df1(zb,apv(cn,0,1),g));
  R jtadrassemble(jt,zb);}
}
static B jtadisplusb(J jt,A f){R f&&AT(f)&VERB&&ISPRIM(f)&&FAV(f)->id==CPLUS;} // +
ADLOCWRAP(B,adisplus)
static B jtadisplusslashb(J jt,A f){if(!f||!(AT(f)&VERB)||FAV(f)->id!=CSLASH||!(AT(FAV(f)->fgh[0])&VERB))R 0; R jtadisplus(jt,FAV(f)->fgh[0]);}   // +/
ADLOCWRAP(B,adisplusslash)

// recording, monad
// verb with no rule in reverse mode: run once on the values and checked.
static A jtadrecnorule(J jt,A f,A x,A y){A z;
 if(jt->jerr!=EVNONCE||!ADT->refused||ADT->fbfail||jtadhaspowinf(jt,f)||ADISSELF(f))R 0;
 RESETERR; ADT->refused=0;
 if(x)RZ(x=jtadrealarg(jt,x)); RZ(y=jtadrealarg(jt,y));
 RZ(z=jtadop(jt,f,x,y)); R jtadnorule(jt,f,z,x,y);
}
static A jtadrec1x(J,A,A);
static A jtadrec2x(J,A,A,A);
static A jtadrec1(J jt,A f,A P){A z; if((z=jtadrec1x(jt,f,P)))R z; R jtadrecnorule(jt,f,0,P);}
static A jtadrec2(J jt,A f,A x,A y){A z; if((z=jtadrec2x(jt,f,x,y)))R z; R jtadrecnorule(jt,f,x,y);}
static A jtadrec1x(J jt,A f,A P){A z,a,b; V*v; A fs,gs,hs; B tp,tr,bx=0;
 ASSERT(AT(f)&VERB,EVDOMAIN);
 tp=TRC(P); tr=jtadhastraced(jt,f);
 if(!tp&&!tr){ // nothing traced: evaluate
  if(!ADBOXCHK(f,0,P))R z=AO1(f,P); bx=1;
 }
 ADASSERT(!ISSPARSE(AT(P))); // sparse argument
 STACKCHKOFL
 if(adisref(f)){A g; ADLOC ls;
  RZ(g=jtadresin(jt,f,&ls));
  if(g!=f){A r;
   if(ADEXPLV(g))df1(r,P,f); else r=REC1(g,P);
   jtadlocexit(jt,&ls); R r;
  }
 }
 ASSERT(!tp||AT(P)&(B01|INT|FL),EVDOMAIN);
 v=FAV(f); fs=v->fgh[0]; gs=v->fgh[1]; hs=v->fgh[2];
 if(bx){A pz;
  if(ISPRIM(f)){
   if(v->id==CGT||v->id==CRAZE){RZ(jtadboxmap(jt,f,0,P,0,&pz,0,1)); R pz;} // > ;
   if(adconst1(f))R UNTR((_z=AO1(f,P)));
   R jtadconstbox(jt,f,0,P);
  }
  ADASSERT(adboxwalk(f));
  {I s=adrankm(f),r=efr(AR(P),s);
   if(r<AR(P))ADNEGCELLS(A,f,s<0,0,jtadrcells1(jt,g_,r,P))
  }
  goto boxwalkr1;
 }
 if(tp&&!tr&&!ISPRIM(f)&&adstructm(f)){A m; // whole structural subtree
  RZ((z=AO1(f,P))); RZ(m=(AO1(f,jtadidx(jt,P,1))));
  R jtadrecord(jt,ADKSTRUCT1,0,P,m,z);
 }
 if(!ISPRIM(f)){I r=efr(AR(P),adrankm(f));
  if(r<AR(P)&&!adatomic1(f)){ADVAL l;
   if(!tr&&adconst1(f))R UNTR((_z=AO1(f,P)));
   RZ(jtadliftb1(jt,1,0,f,(ADVAL){P,ADZ},0,&l)); R l.p;
  }
 }
boxwalkr1:
 switch(v->id){
 case CBOX: R AO1(f,P); // < y nothing to record
 // primitives with a backward rule
 case CDIV: case CEXP: case CLOG: case CSQRT: case CSTARCO: case CSTILE: case CNOT: case CLE: case CGE:
 case CMINUS: case CPLUSCO: case CHALVE: case CPLUS: case CCIRCLE:
 case CCOMMA: case CCOMDOT: case CLAMIN: case CREV: case CCANT: case CHEAD: case CTAIL: case CBEHEAD: case CCTAIL: case CRAZE: case CGT:
 case CNUB: case CBASE: case CPDERIV:
  RZ((z=AO1(f,P))); R PRIM1(z);
 case CDOMINO:
  if(AR(P)>2)R jtadrcells1(jt,f,2,P); // rank 2: a matrix at a time
  if(AR(P)!=2||AS(P)[0]!=AS(P)[1]){A zj; RZ(zj=AO1(f,P)); R jtadrecsame(jt,jtadpinvr(jt,P),zj);} // atom, list, non-square matrix: J's value
  RZ((z=AO1(f,P))); R PRIM1(z);
 case CLEFT: case CRIGHT: R P;
 case CDCAPDOT:
  if(v->valencefns[0]==jtadscatv){RZ(z=AO1(f,P)); R PRIM1(z);}
  ADASSERT(ISDCAP(f)); R df1(z,P,f); // u D.1 inside u: the inner D. runs and its arithmetic is recorded here
 case CDOT: // -/ . * y (determinant)
  if(adinnerid(f)==CMINUS&&AR(P)>2)R jtadrcells1(jt,f,2,P); // rank 2: a matrix at a time
  if(adinnerid(f)==CMINUS&&AR(P)<2)R AR(P)?REC1(slash(ds(CMINUS)),P):P;
  ADASSERT(adinnerid(f)==CMINUS&&AR(P)==2&&AS(P)[0]==AS(P)[1]);
  RZ((z=AO1(f,P))); R PRIM1(z);
 // piecewise constant or integer valued
 case CSTAR: case CFLOOR: case CCEIL: case CEQ: case CNE: case CPOUND: case CDOLLAR: case CIOTA: case CICO:
 case CGRADE: case CDGRADE: case CICAP: case CEPS: case CLDOT: case CFCONS:
  R UNTR((_z=AO1(f,P)));
 case CABASE: ADASSERT(AT(P)&(B01|INT)); R UNTR((_z=AO1(f,P))); // #: y
 // compositions
 case CFORK:
  if(!hs){RZ(b=REC1(gs,P)); R REC1(fs,b);}
  RZ(b=REC1(hs,P));
  if(AT(fs)&NOUN)a=fs; else RZ(a=REC1(fs,P));
  R REC2(gs,a,b);
 case CHOOK: RZ(b=REC1(gs,P)); R REC2(fs,P,b);
 case CAT: case CATCO: if(AT(gs)&NOUN)R REC1(fs,gs); // u@n y is u n
  RZ(b=REC1(gs,P)); R REC1(fs,b);
 case CAMP: case CAMPCO:
  if(AT(fs)&NOUN)R REC2(gs,fs,P);
  if(AT(gs)&NOUN)R REC2(fs,P,gs);
  RZ(b=REC1(gs,P)); R REC1(fs,b);
 case CQQ:
  if(AT(fs)&NOUN){ADASSERT(!TRC(fs)&&!(AT(fs)&BOX)); R UNTR((_z=AO1(f,P)));} // m"n
  R REC1(fs,P);
 case CTILDE: ADASSERT(AT(fs)&VERB); R REC2(fs,P,P);
 case CCOLONE: ADASSERT(!ADPSEUDO(f)); R df1(z,P,f); // explicit definition: its sentences are recorded as it runs
 case CADVERSE: // u :: v
  {I nt=ADT->ntape; WITHDEBUGOFF(z=REC1(fs,P);) if(z)R z;
  if(!jtadadvfails(jt))R 0;
  jtadtapecut(jt,nt); R AT(gs)&NOUN?gs:REC1(gs,P);}
 case CCOLON: ADASSERT(AT(fs)&VERB&&AT(gs)&VERB); R REC1(fs,P);
 case CMCAP: case COBVERSE: R REC1(fs,P);
 case CBSLASH: case CBSDOT:
  ADASSERT(AT(fs)&VERB);
  if(adisplusslash(fs)){RZ((z=AO1(f,P))); R PRIM1(z);} // +/\ +/\. linear, with a rule
  if(ADSCANMM(fs)&&AR(P)&&AS(P)[0]){RZ((z=AO1(f,P))); R PRIM1(z);} // >./\ and the like: one entry
  if(ADSCANP(fs)&&AR(P)&&AS(P)[0]){RZ((z=AO1(f,P))); R PRIM1(z);} // */\ */\. one entry
  if(v->id==CBSDOT&&AR(P)&&AS(P)[0]&&ADSUFFOLD(fs))R jtadrsuffold(jt,fs,P); // g/\. y
  R jtadrscan(jt,fs,v->id==CBSLASH?1:2,P);
 case CATDOT: ADASSERT(ADISAGENDA(v)); R jtadagendar(jt,f,0,P);
 case CSLASH: {I n;
  if(!bx&&AT(fs)&VERB&&ISPRIM(fs)&&(FAV(fs)->id==CPLUS||FAV(fs)->id==CMINUS)){ // +/ -/
   if(!AR(P))R P;
   RZ((z=AO1(f,P))); R PRIM1(z);
  }
  if(!AR(P))R P;
  n=AS(P)[0];
  if(!n)R UNTR((_z=AO1(f,P)));
  if(!bx&&adfoldfast(f)){RZ((z=AO1(f,P))); R PRIM1(z);} // */ >./ <./ one entry
  RZ(a=REC2(ds(CFROM),sc(n-1),P));
  DQ(n-1, RZ(b=REC2(ds(CFROM),sc(i),P)); RZ(a=REC2(fs,b,a));)
  R a;}
 case CPOWOP: {I n;
  ADASSERT(AT(gs)&NOUN);
  RE(n=jtadpowcount(jt,gs));
  a=P;
  DQ(n, RZ(a=REC1(fs,a)););
  R a;}
 case CUNDER: case CUNDCO: {A vi,m; I su=jtadsunder(jt,f,&m);
  if(su){
   RZ(a=REC1(gs,P)); RZ(b=REC1(fs,a));
   if(su==1){RZ(b=REC1(ds(CCOMMA),b)); R REC2(ds(CDOLLAR),shape(P),b);}
   if(su==3){ASSERT(AR(b)==AR(a),EVRANK); ASSERT(!memcmp(AS(b),AS(a),AR(a)*SZI),EVLENGTH); RZ(m=AO1(m,apv(AS(P)[0],0,1)));}
   R REC2(amend(m),b,P);
  }
  RZ(vi=inv(gs)); RZ(a=REC1(gs,P)); RZ(b=REC1(fs,a)); R REC1(vi,b);}
 ADFOLDCASES: {A tz; RZ(jtadfold(jt,f,0,ADZ,P,ADZ,0,1,&z,&tz)); R z;} // the folds: each step recorded
 case CCUT: ADASSERT(adcut0(f)); if(!AR(P))R REC1(fs,P); RZ(a=jtadcutrevx(jt,P)); R REC2(f,a,P); // u;.0 y
 default: ADASSERT(0);
 }
}

// recording, dyad
// x #. y recorded for lists or atoms x and y with x traced (and possibly y)
static A jtadbaserx(J jt,A x,A y){A zj,a,b; I n=AR(y)?AS(y)[0]:1;
 RZ(zj=AO2(ds(CBASE),x,y));
 if(AR(x))a=x; else RZ(a=REC2(ds(CDOLLAR),sc(n),x));
 RZ(a=REC2(ds(CCOMMA),a,num(1)));
 RZ(a=REC1(bsdot(slash(ds(CSTAR))),a)); RZ(a=REC1(ds(CBEHEAD),a)); // weights
 RZ(b=REC2(ds(CSTAR),y,a));
 R jtadrecsame(jt,REC1(slash(ds(CPLUS)),b),zj);
}
static A jtadrec2x(J jt,A f,A x,A y){A z,a,b,c; V*v; A fs,gs,hs; B tx,ty,tr,bx=0;
 ASSERT(AT(f)&VERB,EVDOMAIN);
 tx=TRC(x); ty=TRC(y); tr=jtadhastraced(jt,f);
 if(!tx&&!ty&&!tr){ // nothing traced: evaluate
  if(!ADBOXCHK(f,x,y))R z=AO2(f,x,y);
  bx=1;
 }else if(!jtadrunsown(jt,f)&&(ADBOXHOLDS(x)||ADBOXHOLDS(y)))bx=1;
 ADASSERT(!ISSPARSE(AT(x)|AT(y))); // sparse argument
 STACKCHKOFL
 if(adisref(f)){A g; ADLOC ls;
  RZ(g=jtadresin(jt,f,&ls));
  if(g!=f){A r;
   if(ADEXPLV(g))df2(r,x,y,f); else r=REC2(g,x,y);
   jtadlocexit(jt,&ls); R r;
  }
 }
 ASSERT((!tx||AT(x)&(B01|INT|FL))&&(!ty||AT(y)&(B01|INT|FL)),EVDOMAIN);
 v=FAV(f); fs=v->fgh[0]; gs=v->fgh[1]; hs=v->fgh[2];
 if(bx){A pz;
  if(ISPRIM(f)){B by=ADBOXHOLDS(y);
   if(v->id==CFETCH&&by){RZ(jtadboxmap(jt,f,x,y,0,&pz,0,1)); R pz;}
   if(adconst2(f)||(!by&&!ty&&(v->id==CFETCH||v->id==CFROM)))R UNTR((_z=AO2(f,x,y)));
   R jtadconstbox(jt,f,x,y);
  }
  ADASSERT(adboxwalk(f));
  {I sl=adrankl(f),sr=adrankr(f),lrk=efr(AR(x),sl),rrk=efr(AR(y),sr);
   if(lrk<AR(x)||rrk<AR(y))ADNEGCELLS(A,f,sl<0||sr<0,1,jtadrcells2(jt,g_,lrk,rrk,x,y))
  }
  goto boxwalkr2;
 }
 if(!ISPRIM(f)){I lrk=efr(AR(x),adrankl(f)),rrk=efr(AR(y),adrankr(f));
  if((lrk<AR(x)||rrk<AR(y))&&!adatomic2(f)){ADVAL l;
   if(!tr&&adconst2(f))R UNTR((_z=AO2(f,x,y)));
   RZ(jtadliftb2(jt,1,0,f,(ADVAL){x,ADZ},0,(ADVAL){y,ADZ},0,&l)); R l.p;
  }
 }
boxwalkr2:
 switch(v->id){
 case CPLUS: case CMINUS: case CSTAR: case CDIV: case CEXP: case CLOG: case CROOT: case CMIN: case CMAX: case CSTILE:
 case CCOMMA: case CCOMDOT: case CLAMIN:
  RZ((z=AO2(f,x,y))); R PRIM2(z);
 case CCIRCLE: ADASSERT(!tx);
  if(!AR(x)){RZ(z=AO2(f,x,y));
   if(!AN(z))R UNTR((_z=z));
   I code=i0(x); RE(0);
   if(code==11||code==12)R UNTR((_z=z));
   R PRIM2(z);
  }
  RZ((z=AO2(f,x,y))); R PRIM2(z);
 case CEQ: case CNE: case CLT: case CLE: case CGE: case CGT: case CEPS: case CEBAR:
 case CIOTA: case CICO: case CICAP: case CMATCH: case CFCONS:
  R UNTR((_z=AO2(f,x,y)));
 case CFROM: case CTAKE: case CDROP: case CPOUND: case CDOLLAR: case CROT: case CCANT: // x selects
  if(!ty)R UNTR((_z=AO2(f,x,y)));
  RZ((z=AO2(f,x,y))); R PRIM2(z);
 case CBASE: // x #. y
  if(tx){
   if(!AR(x)){ // atom base: (|."1 y) p. x
    RZ(z=AO2(f,x,y)); RZ(a=REC1(qq(ds(CREV),num(1)),y)); R jtadrecsame(jt,REC2(ds(CPOLY),a,x),z);
   }
   if(AR(x)>1||AR(y)>1)R jtadrcells2(jt,f,1,MIN(1,AR(y)),x,y);
   R jtadbaserx(jt,x,y);
  }
  if(AR(x)>1)R jtadrcells2(jt,f,1,MIN(1,AR(y)),x,y);
  RZ((z=AO2(f,x,y))); R PRIM2(z);
 case CGRADE: case CDGRADE: case CLESS: // y selects
  if(!tx)R UNTR((_z=AO2(f,x,y)));
  RZ((z=AO2(f,x,y))); R PRIM2(z);
 case CLEFT: R x;
 case CRIGHT: R y;
 case CDOT: ADASSERT(adinnerid(f));
  if(!AR(x)||!AR(y)){A s=slash(ds(adinnerid(f))); // atom argument: x u/ . * y is u/ x * y (x an atom) or u/"1 x * y (y an atom)
   R REC1(AR(x)?qq(s,num(1)):s,REC2(ds(CSTAR),x,y));
  }
  RZ((z=AO2(f,x,y))); R PRIM2(z);
 case CDOMINO:
  if(AR(y)>2)R jtadrcells2(jt,f,AR(x),2,x,y); // rank _ 2
  if(AR(y)!=2||AS(y)[0]!=AS(y)[1]){A pi,zj; // y an atom, a list or a non-square matrix: (%. y) +/ . * x, with J's value
   RZ(zj=AO2(f,x,y)); RZ(pi=jtadpinvr(jt,y));
   R jtadrecsame(jt,REC2(jtadmp(jt),pi,x),zj);
  }
  if(!AR(x)){A pi,zj; // an atom x: (%. y) +/ . * x, with J's value
   RZ(zj=AO2(f,x,y)); RZ(pi=REC1(ds(CDOMINO),y));
   R jtadrecsame(jt,REC2(jtadmp(jt),pi,x),zj);
  }
  if(AR(x)>2){A x2,z2,zj; I c=1; DO(AR(x)-1, c*=AS(x)[i+1];) // x of rank 3 or more: as a matrix of #x rows, with J's value
   RZ(zj=AO2(f,x,y)); RZ(x2=REC2(ds(CDOLLAR),v2(AS(x)[0],c),REC1(ds(CCOMMA),x))); RZ(z2=REC2(f,x2,y));
   R jtadrecsame(jt,REC2(ds(CDOLLAR),shape(x),REC1(ds(CCOMMA),z2)),zj);
  }
  RZ((z=AO2(f,x,y))); R PRIM2(z);
 case CPOLY: ADASSERT(!(AT(x)&BOX));
  if(AR(x)>1)R jtadrcells2(jt,f,1,0,x,y); // p. has rank 1 0: a table of coefficients is a list of polynomials
  RZ((z=AO2(f,x,y))); R PRIM2(z);
 case CRBRACE: ADASSERT(AT(fs)&NOUN); RZ((z=AO2(f,x,y))); R PRIM2(z);
 case CFORK:
  if(!hs){RZ(b=REC2(gs,x,y)); R REC1(fs,b);}
  RZ(b=REC2(hs,x,y));
  if(AT(fs)&NOUN)a=fs; else RZ(a=REC2(fs,x,y));
  R REC2(gs,a,b);
 case CHOOK: RZ(b=REC1(gs,y)); R REC2(fs,x,b);
 case CAT: case CATCO: if(AT(gs)&NOUN)R REC1(fs,gs); // x u@n y is u n
  RZ(b=REC2(gs,x,y)); R REC1(fs,b);
 case CAMP: case CAMPCO: ADASSERT(AT(fs)&VERB&&AT(gs)&VERB); RZ(b=REC1(gs,y)); RZ(a=REC1(gs,x)); R REC2(fs,a,b);
 case CQQ:
  if(AT(fs)&NOUN){ADASSERT(!TRC(fs)&&!(AT(fs)&BOX)); R UNTR((_z=AO2(f,x,y)));}
  R REC2(fs,x,y);
 case CTILDE: ADASSERT(AT(fs)&VERB); R REC2(fs,y,x);
 case CCOLONE: ADASSERT(!ADPSEUDO(f)); R df2(z,x,y,f);
 case CDCAPDOT: ADASSERT(ISDCAP(f)); R df2(z,x,y,f);
 case CCUT:
  if(adcut0(f)){
   if(adcutsel(f)){
    if(!ty)R UNTR((_z=AO2(f,x,y)));
    RZ((z=AO2(f,x,y))); R PRIM2(z);
   }
   RZ(a=REC2(jtadcutsel(jt),x,y)); R REC1(fs,a);
  }
  ADASSERT(adcut3(f)&&!tx&&AT(fs)&VERB&&adstructm(fs)&&!jtadhastraced(jt,fs));
  if(!ty)R UNTR((_z=AO2(f,x,y)));
  RZ((z=AO2(f,x,y))); R PRIM2(z);
 case CADVERSE: {I nt=ADT->ntape; WITHDEBUGOFF(z=REC2(fs,x,y);) if(z)R z;
  if(!jtadadvfails(jt))R 0;
  jtadtapecut(jt,nt); R AT(gs)&NOUN?gs:REC2(gs,x,y);}
 case CCOLON: ADASSERT(AT(fs)&VERB&&AT(gs)&VERB); R REC2(gs,x,y);
 case CMCAP: case COBVERSE: R REC2(fs,x,y); // x u M. y  or  x u :. v y
 case CBSLASH: case CBSDOT: ADASSERT(AT(fs)&VERB); R jtadinfixr(jt,fs,v->id==CBSDOT,x,y); // x u\ y  x u\. y
 case CATDOT: ADASSERT(ADISAGENDA(v)); R jtadagendar(jt,f,x,y); // x m@.v y
 case CSLASH: {A g; RZ(g=qq(fs,v2(lr(fs),RMAX))); R REC2(g,x,y);}
 case CPOWOP: {I n;
  ADASSERT(AT(gs)&NOUN);
  RE(n=jtadpowcount(jt,gs));
  a=y;
  DQ(n, RZ(a=REC2(fs,x,a)););
  R a;}
 case CUNDER: case CUNDCO: {A vi;
  RZ(vi=inv(gs)); RZ(b=REC1(gs,y)); RZ(a=REC1(gs,x)); RZ(c=REC2(fs,a,b));
  R REC1(vi,c);
 }
 case CSEMICO: if(!ISPRIM(f))break; R AO2(f,x,y); // x ; y
 ADFOLDCASES: {A tz; RZ(jtadfold(jt,f,x,ADZ,y,ADZ,0,1,&z,&tz)); R z;}
 }
 ADASSERT(0);
}

// backward pass

// cotangent of node x
static A jtadcot(J jt,A x,B create){ADTRACE*t=ADT; I h; A cb;
 if(!x)R 0;
 for(h=ADHASH(x,t->cap);t->keys[h];h=(h+1)&(t->cap-1))
  if(t->keys[h]==x){
   cb=t->tans[h];
   if(cb!=ADNODE)R cb;
   if(!create)R 0;
   RZ(cb=reshape(over(sc(t->k),shape(x)),scf(0.0)));
   ra(cb); t->tans[h]=cb; R cb;
  }
 R 0;
}
static void jtadcotdrop(J jt,A x){ADTRACE*t=ADT; I h; // the cotangent of x is no longer needed
 for(h=ADHASH(x,t->cap);t->keys[h];h=(h+1)&(t->cap-1))
  if(t->keys[h]==x){A c=t->tans[h];
   if(c!=ADNODE){fa(c); t->tans[h]=ADNODE;}
   R;
  }
}
// add contribution G to the cotangent of x
static I jtadacc(J jt,A x,A G){A c; D*cv,*gv;
 RZ(G); // error computing the contribution
 if(!x||!TRC(x))R 1;
 I h=jtadslot(jt,x); I m=ADT->k;
 if(ADT->tans[h]==ADNODE&&AN(G)==m*AN(x)&&!UPTR(G)){ // the first contribution: it becomes the cotangent
  if(AT(G)&FL&&!(AFLAG(G)&AFVIRTUAL)&&ACUC(G)<=ACUC1&&!ACISPERM(AC(G))&&AR(G)==AR(x)+1){ // fresh block nobody else holds: take it
   AS(G)[0]=m; DO(AR(x), AS(G)[i+1]=AS(x)[i];)
   ra(G); ADT->tans[h]=G; R 1;
  }
  {I sh[RMAX+1]; sh[0]=m; DO(AR(x), sh[i+1]=AS(x)[i];) // otherwise a copy
   A cc; if(!(AT(G)&FL))RZ(G=jtcvt(jt,FL,G));
   GATV(cc,FL,AN(G),AR(x)+1,sh); MC(DAV(cc),DAV(G),AN(G)*sizeof(D));
   ra(cc); ADT->tans[h]=cc; R 1;}
 }
 RZ(c=jtadcot(jt,x,1));
 RZ(G=cvt(FL,G)); ASSERT(AN(G)==AN(c),EVLENGTH);
 if(UPTR(G)||UPTR(c)){A s; // outer level watches: a new sum, not update in place
  RZ(s=reshape(shape(c),G)); RZ(s=plus(c,s)); RZ(s=cvt(FL,s));
  realizeifvirtual(s); ra(s); fa(ADT->tans[h]); ADT->tans[h]=s; R 1;
 }
 cv=DAV(c); gv=DAV(G); DO(AN(c), cv[i]+=gv[i];)
 R 1;
}
// G summed over the trailing axes that x lacks (prefix agreement)
static A jtadunbc(J jt,A G,A x){I m=ADT->k,nx,ng; A z; D*gv,*zv;
 RZ(G); RZ(x); nx=AN(x);
 RZ(G=cvt(FL,G)); ng=AN(G);
 if(ng==m*nx){ // G already has one value per atom of x
  if(AR(G)==AR(x)+1&&AS(G)[0]==m&&!memcmp(AS(G)+1,AS(x),AR(x)*sizeof(I)))R G;
  R reshape(over(sc(m),shape(x)),G);
 }
 if(UPTR(G)){ // outer level watches G
  RZ(z=reshape(over(sc(m),over(sc(nx),sc(ng/(m*nx)))),G));
  RZ(z=admap1(slash(ds(CPLUS)),z,1));
  R reshape(over(sc(m),shape(x)),z);
 }
 RZ(z=reshape(over(sc(m),shape(x)),scf(0.0)));
 if(!nx)R z;
 I rest=ng/(m*nx); gv=DAV(G); zv=DAV(z);
 DO(m*nx, D s=0.0; D*p=gv+i*rest; DQ(rest, s+=p[i];) zv[i]=s;)
 R z;
}
// contribution Ct*F to node x
static I jtadaccf(J jt,A x,A F,A Ct,A z){A G;
 RZ(F); RZ(Ct);
 if(!TRC(x))R 1;
 RZ(G=admap2(ds(CSTAR),F,AR(F),Ct,AR(z)));
 RZ(G=jtadunbc(jt,G,x));
 R jtadacc(jt,x,G);
}
// scatter-add: it adds atoms of its argument into chosen places of a result. The backward pass uses it only when an outer D. is watching.
static DF1(jtadscatv){DF12IP; A*dv=AAV(FAV(self)->fgh[1]); A ix=C(dv[0]),sx=C(dv[1]),z; I rz=IAV(C(dv[2]))[0],nz=AN(ix),nt=1,L=1,fr;
 ASSERT(AR(w)>=rz,EVRANK); fr=AR(w)-rz;
 DO(AN(sx), nt*=IAV(sx)[i];) DO(fr, L*=AS(w)[i];)
 ASSERT(AN(w)==L*nz,EVLENGTH);
 if(!(AT(w)&FL))RZ(w=jtcvt(jt,FL,w));
 RZ(z=jtreshape(jt,jtover(jt,vec(INT,fr,AS(w)),sx,ds(CCOMMA)),scf(0.0),0));
 I*iv=IAV(ix); D*wv=DAV(w),*zv=DAV(z);
 for(I l=0;l<L;++l){D*zc=zv+l*nt; D*wc=wv+l*nz;
  for(I j=0;j<nz;++j)
   if(iv[j]>=0)zc[iv[j]]+=wc[j];
 }
 R z;
}
static A jtadmkscat(J jt,A ix,A sx,I rz){
 R fdef(0,CDCAPDOT,VERB,jtadscatv,jtvalenceerr,0,jlink(ix,jlink(sx,sc(rz))),0,VFLAGNONE,RMAX,RMAX,RMAX);
}
// jtadscatter when an outer level is watching: the same, by the scatter verb, so that level sees it
static I jtadscatterv(J jt,A M,A Ct,A x,A y){I nz=AN(M),nx=x?AN(x):0,*jv=IAV(M); A ix,G;
 for(I w=0;w<2;++w){A t=w?y:x; I*iv;
  if(!t||!TRC(t))continue;
  GATV0(ix,INT,nz,1); iv=IAV(ix);
  for(I j=0;j<nz;++j){I s=jv[j]; iv[j]=w?(s>nx?s-nx-1:-1):(s>0&&s<=nx?s-1:-1);}
  RZ(G=AO1(jtadmkscat(jt,ix,shape(t),AR(M)),Ct));
  RZ(jtadacc(jt,t,G));
 }
 R 1;
}
// send a result's cotangent Ct back through the index map M to x and y. M has the shape of z: atom j of z came from atom
// M[j]-1 of x (1..nx) or M[j]-nx-1 of y or from fill
static I jtadscatter(J jt,A M,A Ct,A x,A y){I m=ADT->k,nz,nx=x?AN(x):0,ny=y?AN(y):0; I*jv; D*cv,*cx=0,*cy=0; A c;
 RZ(M); RZ(Ct); RZ(M=cvt(INT,M)); RZ(Ct=cvt(FL,Ct)); nz=AN(M);
 ASSERT(AN(Ct)==m*nz,EVLENGTH);
 if(NESTED&&(UPTR(Ct)||(x&&UPTR(jtadcot(jt,x,0)))||(y&&UPTR(jtadcot(jt,y,0)))))R jtadscatterv(jt,M,Ct,x,y);
 if(x&&TRC(x)){RZ(c=jtadcot(jt,x,1)); cx=DAV(c);}
 if(y&&TRC(y)){RZ(c=jtadcot(jt,y,1)); cy=DAV(c);}
 jv=IAV(M); cv=DAV(Ct);
 DO(nz, I s=jv[i];
  if(s>0){
   if(s<=nx){if(cx){D*p=cx+s-1; DO(m, p[i*nx]+=cv[i*nz];)}}
   else if(cy){D*p=cy+s-nx-1; DO(m, p[i*ny]+=cv[i*nz];)}
  }
  ++cv;
 )
 R 1;
}
// ASSEMBLE: z is the opening of the boxes zb (frame F, results r_i). Route each cell's part of Ct to r_i
static I jtadvjpassemble(J jt,A zb,A z,A Ct){I m=ADT->k,fr=AR(zb),n=AN(zb),nz=AN(z),rr=AR(z)-fr,prodR; I*Rs=AS(z)+fr; D*cv;
 RZ(Ct=cvt(FL,Ct));
 B up=UPTR(Ct); DO(n, A ri=C(AAV(zb)[i]); if(!up&&TRC(ri))up=UPTR(jtadcot(jt,ri,0));)
 if(up){A Cr; // an outer level watches
  RZ(Cr=reshape(over(sc(m),over(sc(n),drop(sc(fr),shape(z)))),Ct));
  DO(n, A ri=C(AAV(zb)[i]); A Ci,ps;
   if(!TRC(ri)||!AN(ri))continue;
   RZ(ps=over(reshape(sc(rr-AR(ri)),num(1)),shape(ri))); // shape of r_i promoted to the rank of cells
   RZ(Ci=admap2(ds(CFROM),sc(i),0,Cr,AR(Cr)-1)); RZ(Ci=take(over(sc(m),ps),Ci)); RZ(Ci=reshape(over(sc(m),shape(ri)),Ci));
   RZ(jtadacc(jt,ri,Ci));
  )
  R 1;
 }
 cv=DAV(Ct);
 prodR=n?nz/n:0;
 DO(n, A ri=C(AAV(zb)[i]); A c; D*sv; I ci=i;
  if(!TRC(ri)||!AN(ri))continue;
  RZ(c=jtadcot(jt,ri,1)); sv=DAV(c);
  I nri=AN(ri),rri=AR(ri),ps[RMAX+1],ix[RMAX+1];
  DO(rr, ps[i]=1;) DO(rri, ps[rr-rri+i]=AS(ri)[i];) // shape of r_i promoted to the rank of cells
  DO(rr, ix[i]=0;)
  DO(nri, I off=0; DO(rr, off=off*Rs[i]+ix[i];)
   D*src=cv+ci*prodR+off; D*dst=sv+i; DO(m, dst[i*nri]+=src[i*nz];)
   {I d=rr-1; while(d>=0&&++ix[d]==ps[d]){ix[d]=0; --d;}}
  )
 )
 R 1;
}

// backward step for >./\  <./\  >./\.  <./\. (pre=1 for the prefix scans \, 0 for the suffix scans \.)
static I jtadvjpscanmm(J jt,A f,B pre,A x,A z,A Ct){A M,W,Gn,Gs,S; I r=AR(x);
 RZ(jtadscanmmc(jt,pre,x,z,&M,&W,&Gn,&Gs));
 RZ(S=adscale(W,Ct));
 RZ(S=admap1(pre?bsdot(slash(ds(CPLUS))):bslash(slash(ds(CPLUS))),S,r));
 A G=pre?admap1(Gn,admap2(ds(CCOMMA),S,r,scf(0.0),0),r):admap1(Gs,admap2(ds(CCOMMA),scf(0.0),0,S,r),r); RZ(G);
 R jtadacc(jt,x,adscale(M,minus(S,G)));
}

// backward step for */\ y and */\. y
static I jtadvjpscanp(J jt,B pre,A x,A z,A Ct){A a,c,G;
 RZ(Ct=jtadexpand(jt,Ct,z));
 if(pre){
  RZ(a=AO2(ds(CCOMMA),AO1(ds(CBEHEAD),x),num(1))); RZ(c=AO2(ds(CCOMMA),num(1),AO1(ds(CCTAIL),z)));
 }else{
  RZ(a=AO2(ds(CCOMMA),num(1),AO1(ds(CCTAIL),x))); RZ(c=AO2(ds(CCOMMA),AO1(ds(CBEHEAD),z),num(1)));
 }
 RZ(G=jtadlinrec(jt,!pre,a,Ct,AR(x)));
 R jtadacc(jt,x,adscale(c,G));
}
// backward step for a monad primitive
static I jtadvjp1(J jt,A f,A x,A z,A Ct){V*v=FAV(f); A F,G,mp,a;
 if(v->valencefns[0]==jtadscatv){A*dv=AAV(v->fgh[1]); A ix=C(dv[0]),ixp,R2; I nt=1; // scatter-add: its transpose gathers
  DO(AN(C(dv[1])), nt*=IAV(C(dv[1]))[i];)
  if(!nt||!AN(x))R 1;
  RZ(R2=reshape(v2(AN(Ct)/nt,nt),Ct));
  RZ(R2=admap2(ds(CCOMMA),R2,1,scf(0.0),0)); // zero at index nt, for the dropped atoms
  GATV0(ixp,INT,AN(ix),1); DO(AN(ix), I s=IAV(ix)[i]; IAV(ixp)[i]=s<0?nt:s;)
  RZ(G=admap2(ds(CFROM),ixp,1,R2,1));
  R jtadacc(jt,x,reshape(over(sc(ADT->k),shape(x)),G));
 }
 switch(v->id){
 case CDIV:    RZ(F=negate(tymes(z,z))); break;
 case CEXP:    F=z; break;
 case CLOG:    RZ(F=recip(x)); break;
 case CSQRT:   RZ(F=divide(scf(0.5),z)); break;
 case CSTARCO: RZ(F=tymes(num(2),x)); break;
 case CSTILE:  RZ(F=signum(x)); break;
 case CMINUS: case CNOT: F=num(-1); break;
 case CPLUSCO: F=num(2); break;
 case CHALVE:  F=scf(0.5); break;
 case CPLUS: case CLE: case CGE: F=num(1); break;
 case CCIRCLE: F=pie; break;
 case CCANT: R jtadacc(jt,x,admap1(ds(CCANT),Ct,AR(z)));
 case CCOMMA: // ravel: the same atoms in order
  if(AN(z)==AN(x))R jtadacc(jt,x,reshape(over(sc(ADT->k),shape(x)),Ct));
  /* fall through */
 case CCOMDOT: case CLAMIN: case CREV: case CHEAD: case CTAIL: case CBEHEAD: case CCTAIL: case CRAZE: case CGT:
  R jtadscatter(jt,(a=AO1(f,jtadidx(jt,x,1))),Ct,x,0);
 case CNUB: {A s; // ~.y = (~:y)#y
  RZ(s=(AO1(ds(CNE),x)));
  R jtadscatter(jt,(a=AO2(ds(CPOUND),s,jtadidx(jt,x,1))),Ct,x,0);}
 case CSLASH: {I n=AS(x)[0]; A t; // +/ or -/ along the first axis: each item gets Ct (alternating sign for -/)
  if(adfoldfast(f)){I r=AR(z); // */ >./ <./  item i gets G[i] * Ct, for each direction
   RZ(G=jtadfoldw(jt,f,x,z));
   RZ(G=admap2(qq(ds(CSTAR),v2(r,r)),Ct,r,G,r+1));
   R jtadacc(jt,x,G);
  }
  if(ADT->k==1){RZ(G=reshape(over(num(1),shape(x)),Ct));
  }else{
   RZ(t=admap1(ds(CCOMMA),Ct,AR(z)));
   RZ(G=admap2(ds(CDOLLAR),shape(x),1,t,1));
  }
  if(FAV(v->fgh[0])->id==CMINUS){A s;
   RZ(s=minus(num(1),tymes(num(2),jtatomic2(jt,num(2),apv(n,0,1),ds(CSTILE)))));
   RZ(G=admap2(ds(CSTAR),s,1,G,AR(x)));
  }
  R jtadacc(jt,x,G);}
 case CBSLASH: case CBSDOT: {A g; // u\ y (prefix scan) and u\. y
  if(ADSCANMM(v->fgh[0]))R jtadvjpscanmm(jt,v->fgh[0],v->id==CBSLASH,x,z,Ct); // >./\ and the like
  if(ADSCANP(v->fgh[0]))R jtadvjpscanp(jt,v->id==CBSLASH,x,z,Ct); // */\ */\.
  RZ(g=(AO1(ds(v->id==CBSLASH?CBSDOT:CBSLASH),slash(ds(CPLUS)))));
  RZ(G=admap1(g,Ct,AR(x))); R jtadacc(jt,x,G);}
 case CBASE: {I n; A w; // #. y
  if(!AR(x))R jtadacc(jt,x,Ct);
  n=AS(x)[AR(x)-1]; RZ(w=expn2(num(2),apv(n,n-1,-1)));
  RZ(G=admap2(ds(CSTAR),Ct,0,w,1));
  R jtadacc(jt,x,G);}
 case CPDERIV: {I n=AR(x)?AS(x)[AR(x)-1]:1; // p.. c
  if(n<=1)R 1; // constant polynomial: its derivative is 0 whatever its coefficient
  ADASSERT(AR(z)==AR(x)&&AS(z)[AR(z)-1]==n-1);
  RZ(G=admap2(ds(CSTAR),Ct,1,apv(n-1,1,1),1));
  RZ(G=admap2(ds(CCOMMA),num(0),0,G,1));
  R jtadacc(jt,x,G);}
 case CDOMINO: {A zt;
  RZ(mp=jtadmp(jt)); RZ(zt=cant1(z));
  RZ(G=admap2(mp,zt,2,Ct,2));
  RZ(G=admap2(mp,G,2,zt,2));
  R jtadacc(jt,x,negate(G));}
 case CDOT: {A t;
  RZ(t=jtaddetfac(jt,x,z));
  RZ(G=admap2(ds(CSTAR),Ct,0,t,2));
  R jtadacc(jt,x,G);}
 default: ASSERT(0,EVSYSTEM);
 }
 R jtadaccf(jt,x,F,Ct,z);
}

// pass cotangent back through x {. y or x }. y or x $ y directly using one take or one reshape for all m directions at once.
static A jtadstructt(J jt,C id,A x,A y,A z,A Ct){I m=ADT->k,r=AR(y),n; A s;
 if(id==CDOLLAR){
  if(!AN(y)||AN(z)>AN(y))R mark;
  if(AN(z)==AN(y))R reshape(over(sc(m),shape(y)),Ct);
  R reshape(over(sc(m),shape(y)),take(v2(m,AN(y)),reshape(v2(m,AN(z)),Ct)));
 }
 if(!(AT(x)&INT)||AR(x)>1||(n=AN(x))>r||AR(z)!=r||!r)R mark;
 GATV0(s,INT,r,1);
 DO(r, I d=AS(y)[i]; I xi=i<n?IAV(x)[i]:0; I back=i<n&&(id==CTAKE?xi<0:xi>=0)&&!(id==CDROP&&xi==0); IAV(s)[i]=back?-d:d;) // from the end where z came from the end of y
 if(id==CDROP)DO(n, if(!IAV(x)[i])IAV(s)[i]=AS(y)[i];)
 R take(over(sc(m),s),Ct); // (m,s) {. Ct
}

// backward step for x { y when x is integer index array and no outer D. is watching. It works straight on the cotangent arrays with no index map.
static I jtadgathert(J jt,A x,A y,A z,A Ct){I m=ADT->k,n=AS(y)[0],nx=AN(x),ny=AN(y),nz=AN(z),it; A c; D*cv,*gv; I*xv;
 if(!n||!nx||!ny)R 1;
 it=ny/n;
 RZ(c=jtadcot(jt,y,1)); RZ(Ct=cvt(FL,Ct)); ASSERT(AN(Ct)==m*nz,EVLENGTH);
 cv=DAV(c); gv=DAV(Ct); xv=IAV(x);
 for(I l=0;l<m;++l){D*cl=cv+l*ny; D*gl=gv+l*nz;
  for(I j=0;j<nx;++j){I r=xv[j];
   if(r<0)r+=n;
   D*dst=cl+r*it; D*src=gl+j*it;
   for(I e=0;e<it;++e)dst[e]+=src[e];
  }
 }
 R 1;
}

// backward step for x u;._3 y with u = , or ] and an integer x
// Each window's part of Ct is added back where the window came from (overlapping windows add up).
// x: window sizes (step 1), or steps ,: sizes.  *done is 0 if this does not apply (the caller then uses the index map)
static I jtadcol2im(J jt,A f,A x,A y,A z,A Ct,B*done){I m=ADT->k,r=AR(y),k,mv[RMAX],sz[RMAX],nw[RMAX],st[RMAX],T=1,W=1,A1=1; A fs=FAV(f)->fgh[0],c; I*xv;
 *done=0;
 if(adcut3(f)!=-3||!ISPRIM(fs)||(FAV(fs)->id!=CCOMMA&&FAV(fs)->id!=CRIGHT)||!(AT(x)&INT))R 1;
 if(UPTR(Ct)||UPTR(jtadcot(jt,y,0)))R 1; // outer level watches: index map which it can follow
 xv=IAV(x);
 if(AR(x)<=1){k=AN(x); DO(k, mv[i]=1; sz[i]=xv[i];)} // sizes, step 1
 else if(AR(x)==2&&AS(x)[0]==2){k=AS(x)[1]; DO(k, mv[i]=xv[i]; sz[i]=xv[k+i];)} // steps ,: sizes
 else R 1;
 if(k>r||!k)R 1; // k: number of windowed (leading) axes
 DO(k, if(mv[i]<=0||sz[i]<=0)R 1;)
 DO(r-k, T*=AS(y)[k+i];) // T: atoms in the trailing axes (not windowed)
 DO(k, I p=T; for(I j=i+1;j<k;++j)p*=AS(y)[j]; st[i]=p;) // stride of axis i in atoms
 DO(k, nw[i]=AS(y)[i]>=sz[i]?1+(AS(y)[i]-sz[i])/mv[i]:0; W*=nw[i]; A1*=sz[i];) // nw: complete windows along axis i.  W: all windows
 A1*=T; // atoms per window
 if(AN(z)!=W*A1)R 1; // z must be the windows one after another
 RZ(c=jtadcot(jt,y,1)); RZ(Ct=cvt(FL,Ct)); ASSERT(AN(Ct)==m*AN(z),EVLENGTH);
 D*cv=DAV(c),*gv=DAV(Ct); I ny=AN(y),nz=AN(z),na=A1/T; // na: positions per window
 for(I l=0;l<m;++l){D*cl=cv+l*ny; D*gl=gv+l*nz; I wi[RMAX]; DO(k, wi[i]=0;) // each direction.  wi: the window's index in the grid nw
  for(I w=0;w<W;++w){I base=0; DO(k, base+=wi[i]*mv[i]*st[i];) // base: the window's first atom in y
   I oi[RMAX]; DO(k, oi[i]=0;) // oi: the position within the window
   for(I a=0;a<na;++a){I off=base;
    DO(k, off+=oi[i]*st[i];)
    D*dst=cl+off;
    for(I e=0;e<T;++e)dst[e]+=gl[e]; gl+=T;
    {I d=k-1; while(d>=0&&++oi[d]==sz[d]){oi[d]=0; --d;}} // next position (last axis fastest)
   }
   {I d=k-1; while(d>=0&&++wi[d]==nw[d]){wi[d]=0; --d;}} // next window
  }
 }
 *done=1; R 1;
}

// backward step for a dyad primitive
static I jtadvjp2(J jt,A f,A x,A y,A z,A Ct){V*v=FAV(f); A F,c,mp,a; B tx=TRC(x),ty=TRC(y);
 if(ISPRIM(f)&&adatomic2(f)&&!AN(z))R 1; // atomic dyad with empty result
 switch(v->id){
 case CPLUS:
  if(tx)RZ(jtadaccf(jt,x,num(1),Ct,z));
  if(ty)RZ(jtadaccf(jt,y,num(1),Ct,z));
  R 1;
 case CMINUS:
  if(tx)RZ(jtadaccf(jt,x,num(1),Ct,z));
  if(ty)RZ(jtadaccf(jt,y,num(-1),Ct,z));
  R 1;
 case CSTAR:
  if(tx)RZ(jtadaccf(jt,x,y,Ct,z));
  if(ty)RZ(jtadaccf(jt,y,x,Ct,z));
  R 1;
 case CDIV:
  if(tx)RZ(jtadaccf(jt,x,recip(y),Ct,z));
  if(ty)RZ(jtadaccf(jt,y,negate(divide(z,y)),Ct,z));
  R 1;
 case CEXP:
  if(tx)RZ(jtadaccf(jt,x,jtadpowfx(jt,x,y,z),Ct,z));
  if(ty)RZ(jtadaccf(jt,y,tymes(z,logar1(x)),Ct,z));
  R 1;
 case CLOG:
  if(tx)RZ(jtadaccf(jt,x,negate(divide(z,tymes(x,logar1(x)))),Ct,z));
  if(ty)RZ(jtadaccf(jt,y,recip(tymes(y,logar1(x))),Ct,z));
  R 1;
 case CROOT:
  if(tx)RZ(jtadaccf(jt,x,negate(divide(tymes(z,logar1(y)),tymes(x,x))),Ct,z));
  if(ty)RZ(jtadaccf(jt,y,jtadrootfy(jt,x,y,z),Ct,z));
  R 1;
 case CMIN: case CMAX:
  RZ(c=jtadselx(jt,v->id,x,y));
  if(tx)RZ(jtadaccf(jt,x,c,Ct,z));
  if(ty)RZ(jtadaccf(jt,y,minus(num(1),c),Ct,z));
  R 1;
 case CSTILE:
  if(tx)RZ(jtadaccf(jt,x,divide(minus(z,y),x),Ct,z));
  if(ty)RZ(jtadaccf(jt,y,num(1),Ct,z));
  R 1;
 case CCIRCLE:
   if(AR(x)){RZ(F=jtadcirclefactorv(jt,x,y,z)); R jtadaccf(jt,y,F,Ct,z);}
  {if(!AN(z))R 1;
   I code=i0(x); RE(0);
   RZ(F=jtadcirclefactor(jt,code,y,z));
   R jtadaccf(jt,y,F,Ct,z);
  }
 // atom-moving: apply f to atom indices
 case CFROM: // x { y with an integer index array: each cotangent item is added straight into its source item
  if(AT(x)&INT&&AR(y)>=1&&!UPTR(Ct)&&!UPTR(jtadcot(jt,y,0)))R jtadgathert(jt,x,y,z,Ct);
  R jtadscatter(jt,(a=AO2(f,x,jtadidx(jt,y,1))),Ct,0,y);
 case CTAKE: case CDROP: case CDOLLAR: {A G; // direct transposes where they exist; otherwise the index map below
  RZ(G=jtadstructt(jt,v->id,x,y,z,Ct));
  if(G!=mark)R jtadacc(jt,y,G);}
  /* fall through */
 case CCUT: {B done=0; // windows: direct col2im for complete windows taken whole else the index map
  RZ(jtadcol2im(jt,f,x,y,z,Ct,&done));
  if(done)R 1;
  R jtadscatter(jt,(a=AO2(f,x,jtadidx(jt,y,1))),Ct,0,y);}
 case CPOUND: case CROT: case CCANT: // x selects
  R jtadscatter(jt,(a=AO2(f,x,jtadidx(jt,y,1))),Ct,0,y);
 case CGRADE: case CDGRADE: // y selects
  R jtadscatter(jt,(a=AO2(f,jtadidx(jt,x,1),y)),Ct,x,0);
 case CLESS: {A s; // x -. y
  RZ(s=jtadlessmask(jt,x,y));
  R jtadscatter(jt,(a=AO2(ds(CPOUND),s,jtadidx(jt,x,1))),Ct,x,0);}
 case CCOMMA: {I m=ADT->k; // x , y with items of the same shape as z's: first #x and last #y items of Ct
  B dx=AR(x)==AR(z)&&AR(x)>=1&&!memcmp(AS(x)+1,AS(z)+1,(AR(x)-1)*SZI);
  B dy=AR(y)==AR(z)&&AR(y)>=1&&!memcmp(AS(y)+1,AS(z)+1,(AR(y)-1)*SZI);
  if((dx||!tx)&&(dy||!ty)){
   if(tx)RZ(jtadacc(jt,x,take(v2(m,AS(x)[0]),Ct)));
   if(ty)RZ(jtadacc(jt,y,take(v2(m,-AS(y)[0]),Ct)));
   R 1;
  }}
  /* fall through */
 case CCOMDOT: case CLAMIN: case CRBRACE: // both move
  R jtadscatter(jt,(a=AO2(f,jtadidx(jt,x,1),jtadidx(jt,y,AN(x)+1))),Ct,x,y);
 // linear and bilinear
 case CBASE: {I n; A w; // x #. y
  if(!AR(y)){
   RZ(w=(AO2(ds(CBASE),x,num(1))));
   R jtadaccf(jt,y,w,Ct,z);
  }
  n=AS(y)[AR(y)-1];
  RZ(w=AR(x)?x:reshape(sc(n),x)); RZ(w=AO2(ds(CCOMMA),w,num(1))); // weights
  RZ(w=AO1(bsdot(slash(ds(CSTAR))),w)); RZ(w=AO1(ds(CBEHEAD),w));
  R jtadacc(jt,y,admap2(ds(CSTAR),Ct,0,w,1));}
 case CDOT: {I m=ADT->k,k=AS(x)[AR(x)-1],na,nb; A Cr,X,Y; // z = x +/ . * y contracts the last axis of x with the first of y
  na=k?AN(x)/k:0; nb=k?AN(y)/k:0;
  RZ(mp=jtadmp(jt));
  RZ(Cr=reshape(over(sc(m),v2(na,nb)),cvt(FL,Ct)));
  RZ(X=reshape(v2(na,k),cvt(FL,x)));
  RZ(Y=reshape(v2(k,nb),cvt(FL,y)));
  A sg=0;
  if(adinnerid(f)==CMINUS){
   RZ(sg=minus(num(1),tymes(num(2),jtatomic2(jt,num(2),apv(k,0,1),ds(CSTILE)))));
   RZ(Y=admap2(ds(CSTAR),sg,0,Y,1));
  }
  A Cm1=m==1?reshape(v2(na,nb),Cr):0;
  if(tx){A G;
   RZ(G=!Cm1?AO2(mp,reshape(v2(m*na,nb),Cr),cant1(Y)):k*nb<=na*nb+na*k?AO2(mp,Cm1,cant1(Y)):cant1(AO2(mp,Y,cant1(Cm1))));
   RZ(jtadacc(jt,x,reshape(over(sc(m),shape(x)),G)));}
  if(ty){A G;
   if(!Cm1){
    RZ(G=AO2(mp,cant1(X),reshape(v2(na,m*nb),cant2(over(num(1),v2(0,2)),Cr))));
    RZ(G=cant2(over(num(1),v2(0,2)),reshape(over(sc(k),v2(m,nb)),G)));
   }
   else RZ(G=na*k<=na*nb+nb*k?AO2(mp,cant1(X),Cm1):cant1(AO2(mp,cant1(Cm1),X)));
   if(sg)RZ(G=admap2(ds(CSTAR),sg,1,G,2));
   RZ(jtadacc(jt,y,reshape(over(sc(m),shape(y)),G)));
  }
  R 1;}
 case CDOMINO: {A Gx;
  RZ(mp=jtadmp(jt));
  RZ(Gx=jtadsolvek(jt,Ct,cant1(y),z));
  if(tx)RZ(jtadacc(jt,x,Gx));
  if(ty){A G;
   RZ(G=AR(z)==1?admap2(slash(ds(CSTAR)),Gx,1,z,1):admap2(mp,Gx,2,cant1(z),2));
   RZ(jtadacc(jt,y,negate(G)));
  }
  R 1;}
 case CPOLY: {I m=ADT->k,n=AN(x),ny=AN(y); // c p. y
  if(ty){A pd;
   RZ(pd=(AO1(ds(CPDERIV),x)));
   RZ(F=(AO2(f,pd,y)));
   RZ(jtadaccf(jt,y,F,Ct,z));
  }
  if(tx){A Yp,G;
   RZ(Yp=(AO2(slash(ds(CEXP)),y,apv(n,0,1)))); RZ(Yp=reshape(v2(ny,n),Yp));
   RZ(mp=jtadmp(jt)); RZ(G=(AO2(mp,reshape(v2(m,ny),Ct),Yp)));
   RZ(jtadacc(jt,x,reshape(over(sc(m),shape(x)),G)));
  }
  R 1;}
 default: ASSERT(0,EVSYSTEM);
 }
}

static I jtadbackward(J jt){ADTRACE*t=ADT; A *old=jt->tnextpushp;
 for(I e=t->ntape-1;e>=0;--e){ADENT en=t->tape[e]; A Ct; I ok;
  if(!(Ct=jtadcot(jt,en.z,0)))continue; // nothing depends on this result
  switch(en.kind){
  case ADKPRIM1:    ok=jtadvjp1(jt,en.f,en.x,en.z,Ct); break;
  case ADKPRIM2:    ok=jtadvjp2(jt,en.f,en.x,en.y,en.z,Ct); break;
  case ADKSTRUCT1: {A M=en.y?en.y:jtadidx(jt,en.x,1); ok=jtadscatter(jt,M,Ct,en.x,0); break;}
  case ADKBOXMAP:   ok=jtadboxmapbk(jt,en.x,en.y,en.z,Ct); break;
  default:          ok=jtadvjpassemble(jt,en.x,en.z,Ct); break;
  }
  RZ(ok);
  jtadcotdrop(jt,en.z); // z was produced here, so no earlier entry uses it
  tpop(old);
 }
 R 1;
}

// Forward replay
// When the result has at least as many atoms as y, forward mode is cheaper. Instead of running u again, the
// tangents are pushed through the recorded operations in order. The forward rules are given each recorded result, so nothing is recomputed.

// tangent of a node, ADZ if zero
static A jtadtan(J jt,A x){I h=jtadslot(jt,x);
 if(h<0)R ADZ;
 A c=ADT->tans[h];
 R c==ADNODE?ADZ:c;
}
static void jtadtanset(J jt,A x,A T){I h=jtadslot(jt,x);
 if(h<0)R;
 if(AFLAG(T)&AFVIRTUAL&&!(T=realize(T)))R;
 A*s=&ADT->tans[h];
 if(*s!=ADNODE)fa(*s);
 ra(T); *s=T;
}
static void jtadtandrop(J jt,A x){I h=jtadslot(jt,x); if(h<0)R; A*s=&ADT->tans[h]; if(*s!=ADNODE){fa(*s); *s=ADNODE;}}
// ASSEMBLE: tangent of opening z of boxes zb is made from the tangents of the cell results with fill
static A jtadfwdasm(J jt,A zb,A z){I k=ADT->k,fr=AR(zb),n=AN(zb),nz=AN(z),rr=AR(z)-fr,prodR; I*Rs=AS(z)+fr; A T; D*tv; B any=0,up=0;
 DO(n, A Ti=jtadtan(jt,C(AAV(zb)[i])); if(!ISZ(Ti)){any=1; if(!up)up=UPTR(Ti);})
 if(!any)R ADZ;
 if(up){A bt,Tb; // outer level watches
  bt=0;
  DQ(n, A ri=C(AAV(zb)[i]); A Ti=jtadtan(jt,ri),ps; // list of boxed tangents
   RZ(ps=over(sc(k),over(reshape(sc(rr-AR(ri)),num(1)),shape(ri))));
   RZ(Ti=ISZ(Ti)?reshape(ps,scf(0.0)):reshape(ps,Ti));
   RZ(bt=bt?jlink(Ti,bt):box(Ti));
  )
  RZ(bt=reshape(shape(zb),bt));
  RZ(Tb=ope(bt));
  R cant2(over(apv(fr,0,1),apv(AR(Tb)-fr-1,fr+1,1)),Tb); // direction axis to the front
 }
 RZ(T=reshape(over(sc(k),shape(z)),scf(0.0))); tv=DAV(T);
 prodR=n?nz/n:0;
 DO(n, A ri=C(AAV(zb)[i]); A Ti=jtadtan(jt,ri); I ci=i;
  if(ISZ(Ti)||!AN(ri))continue;
  RZ(Ti=cvt(FL,Ti)); D*sv=DAV(Ti);
  I nri=AN(ri),rri=AR(ri),ps[RMAX+1],ix[RMAX+1];
  DO(rr, ps[i]=1;) DO(rri, ps[rr-rri+i]=AS(ri)[i];)
  DO(rr, ix[i]=0;)
  DO(nri, I off=0; DO(rr, off=off*Rs[i]+ix[i];)
   D*dst=tv+ci*prodR+off; D*src=sv+i; DO(k, dst[i*nz]=src[i*nri];)
   {I d=rr-1; while(d>=0&&++ix[d]==ps[d]){ix[d]=0; --d;}}
  )
 )
 R T;
}
// STRUCT1: tangent of z is gathered from the tangent T of the argument through index map m (atom numbers from 1,
// 0 for a fill). No map: the identity.
static A jtadfwdstruct(J jt,A m,A T,A z){I k=AS(T)[0],nx=k?AN(T)/k:0; A Tf,G;
 if(!m)R T;
 if(!k)R reshape(over(num(0),shape(z)),scf(0.0)); // no directions (empty y), so empty tangent
 RZ(Tf=reshape(v2(k,nx),T)); // Tf =. (k,nx) $ T
 RZ(Tf=(AO2(qq(ds(CCOMMA),v2(1,1)),reshape(v2(k,1),scf(0.0)),Tf))); // Tf =. 0 ,"1 Tf
 RZ(G=(AO2(qq(ds(CFROM),v2(1,1)),ravel(m),Tf))); // G =. (,m) {"1 Tf
 R reshape(over(sc(k),shape(z)),G); // (k,$z) $ G
}
// Forward-mode Jacobian from a tape already recorded: when z has at least as many atoms as y, one
// forward pass with one direction per atom of y is cheaper than one backward pass per atom of z. Only the tangents are computed.
static A jtadreplay(J jt,A y,A zout){ADTRACE*t=ADT; I n=AN(y),*last; A T,Tz,r=0; A *old;
 t->k=n;
 RZ(T=reshape(over(sc(n),shape(y)),take(sc(n+1),scf(1.0)))); // seed: (n,$y) $ (n+1) {. 1.0, the identity
 jtadtanset(jt,y,T);
 ASSERT(last=calloc(t->cap,sizeof(I)),EVWSFULL); // index of the last entry that uses each node
 DO(t->ntape, ADENT*e=&t->tape[i]; I h;
  if(e->kind==ADKASSEMBLE||e->kind==ADKBOXMAP){I ie=i;
   DO(AN(e->x), if((h=jtadslot(jt,C(AAV(e->x)[i])))>=0)last[h]=ie;)
  }else{
   if((h=jtadslot(jt,e->x))>=0)last[h]=i;
   if((h=jtadslot(jt,e->y))>=0)last[h]=i;
  }
 )
 old=jt->tnextpushp;
 for(I e=0;e<t->ntape;++e){ADENT en=t->tape[e]; A pz; I ok=1; Tz=ADZ;
  switch(en.kind){
  case ADKASSEMBLE: ok=!!(Tz=jtadfwdasm(jt,en.x,en.z)); break;
  case ADKBOXMAP: ok=!!(Tz=jtadleafgather(jt,en.x,en.y,en.z,1)); break;
  case ADKPRIM2: {A Tx=jtadtan(jt,en.x),Ty=jtadtan(jt,en.y);
   if(!ISZ(Tx)||!ISZ(Ty)){t->zgiven=en.z; ok=jtadfwd2(jt,en.f,en.x,Tx,en.y,Ty,n,&pz,&Tz); t->zgiven=0;}
   break;}
  case ADKSTRUCT1: {A Tx=jtadtan(jt,en.x);
   if(!ISZ(Tx))ok=!!(Tz=jtadfwdstruct(jt,en.y,Tx,en.z));
   break;}
  default: {A Tx=jtadtan(jt,en.x); // PRIM1
   if(!ISZ(Tx)){t->zgiven=en.z; ok=jtadfwd1(jt,en.f,en.x,Tx,n,&pz,&Tz); t->zgiven=0;}
   break;}
  }
  if(!ok)goto exit;
  if(!ISZ(Tz)){
   if(!(Tz=adexpand(Tz,en.z))||!(Tz=cvt(FL,Tz))){ok=0; goto exit;}
   jtadtanset(jt,en.z,Tz);
  }
  // free the tangents of nodes used for the last time here
  if(en.kind==ADKASSEMBLE||en.kind==ADKBOXMAP){
   DO(AN(en.x), A ri=C(AAV(en.x)[i]); I h=jtadslot(jt,ri);
    if(h>=0&&last[h]==e&&ri!=zout)jtadtandrop(jt,ri);
   )
  }else{I h;
   if((h=jtadslot(jt,en.x))>=0&&last[h]==e&&en.x!=zout)jtadtandrop(jt,en.x);
   if((h=jtadslot(jt,en.y))>=0&&last[h]==e&&en.y!=zout)jtadtandrop(jt,en.y);
  }
  tpop(old);
 }
 Tz=jtadtan(jt,zout);
 if(ISZ(Tz))r=reshape(over(shape(zout),shape(y)),scf(0.0));
 else if((Tz=cant2(num(0),Tz)))r=reshape(over(shape(zout),shape(y)),Tz); // direction axis to the end
exit:
 free(last);
 R r;
}

// Jacobian by reverse mode: record u at y, then one backward pass with one direction per atom of the result.
// *pz is set to the result of u. Returns the Jacobian, shape ($z),$y.
// If chooseok and the result has at least as many atoms as y, forward mode is cheaper: tape is replayed forward.
static A jtadjacrev(J jt,A u,A x,A y,B dyad,A*pz,B chooseok){A z,c,Jy; I m;
 ADT->rev=1; // this level records
 RZ(y=adtreg(y,ADNODE));
 RZ(z=dyad?REC2(u,x,y):REC1(u,y)); // run u with reverse rules (each operation on traced data appends entry to the tape)
 ASSERT((AT(z)&NUMERIC&&!(AT(z)&CMPX))||!AN(z),EVDOMAIN); // u's result must be real
 *pz=z; m=AN(z);
 if(!TRC(z)||!m||!AN(y))R reshape(over(shape(z),shape(y)),scf(0.0)); // z does not depend on y or nothing to differentiate
 if(chooseok&&m>=AN(y)){ // forward mode is cheaper: replay the tape forward
  C otr=jt->parserstackframe.adtracing; jt->parserstackframe.adtracing=0; // rules run J verbs: do not trace them
  A r=jtadreplay(jt,y,z);
  jt->parserstackframe.adtracing=otr;
  R r;
 }
 ADT->k=m;
 RZ(c=jtadcot(jt,z,1)); DO(m, DAV(c)[i*m+i]=1.0;) // seed: identity
 C otr=jt->parserstackframe.adtracing; jt->parserstackframe.adtracing=0; // rules run J verbs: do not trace them
 I ok=jtadbackward(jt);
 jt->parserstackframe.adtracing=otr;
 RZ(ok);
 if(!(Jy=jtadcot(jt,y,0)))R reshape(over(shape(z),shape(y)),scf(0.0));
 R reshape(over(shape(z),shape(y)),Jy);
}

// ***************************************** rank lifting *****************************************
// u"r applied cell by cell would put one set of operations per cell on the tape (reverse mode) or cost one
// loop iteration per cell (forward mode). Instead the rank is pushed down through u, so the work is done by
// whole-array operations.

#define AP1(f,y,z) RZ(jtadap1(jt,rev,k,(f),(y),(z)))
#define AP2(f,x,y,z) RZ(jtadap2(jt,rev,k,(f),(x),(y),(z)))
#define LIFT1(f,y,fr,z) RZ(jtadlift1(jt,rev,k,(f),(y),(fr),(z)))
#define LIFT2(f,x,fx,y,fy,z) RZ(jtadlift2(jt,rev,k,(f),(x),(fx),(y),(fy),(z)))
// apply f to y (no frame), in the current mode. The leaves of rank lifting.
static I jtadap1(J jt,B rev,I k,A f,ADVAL y,ADVAL*z){
 if(rev){z->t=ADZ; z->p=jtadrec1(jt,f,y.p); R z->p!=0;}
 R jtadfwd1(jt,f,y.p,y.t,k,&z->p,&z->t);
}
// the same for x f y
static I jtadap2(J jt,B rev,I k,A f,ADVAL x,ADVAL y,ADVAL*z){
 if(rev){z->t=ADZ; z->p=jtadrec2(jt,f,x.p,y.p); R z->p!=0;}
 R jtadfwd2(jt,f,x.p,x.t,y.p,y.t,k,&z->p,&z->t);
}
// Entry points of rank lifting. Apply f to every cell of framed value and give the assembled result with its derivative.
static I jtadlift1(J jt,B rev,I k,A f,ADVAL y,I fr,ADVAL*z){
 if(!fr)R jtadap1(jt,rev,k,f,y,z);
 R jtadliftb1(jt,rev,k,f,y,fr,z);
}
// the same for x f y, but frames fx and fy in prefix agreement
static I jtadlift2(J jt,B rev,I k,A f,ADVAL x,I fx,ADVAL y,I fy,ADVAL*z){
 if(!fx&&!fy)R jtadap2(jt,rev,k,f,x,y,z);
 R jtadliftb2(jt,rev,k,f,x,fx,y,fy,z);
}

static ADVAL jtadnoun(J jt,A n){ADVAL v={n,ADZ}; v.t=adtget(n); R v;} // a noun operand (traced in explicit code, constant otherwise)
// agreement of frames
static I jtadframes(J jt,B rev,I k,ADVAL*x,I*fx,ADVAL*y,I*fy){I m=MIN(*fx,*fy); ADVAL*s; I fs,fl; A E,ve;
 DO(m, ASSERT(AS(x->p)[i]==AS(y->p)[i],EVLENGTH);); // frames must agree
 if(*fx==*fy)R 1;
 if(*fx<*fy){s=x; fs=*fx; fl=*fy;
  RZ(E=vec(INT,fl-fs,AS(y->p)+fs));
 }else{s=y; fs=*fy; fl=*fx;
  RZ(E=vec(INT,fl-fs,AS(x->p)+fs));
 }
 RZ(ve=qq(atop(amp(E,ds(CDOLLAR)),ds(CLAMIN)),sc(AR(s->p)-fs))); // (E&$)@,:"c
 AP1(ve,*s,s);
 *fx=*fy=fl; R 1;
}
// does verb f contain explicit definition (seen through names)?
static B jtadhasexpl(J,A);
static B jtadhasexplb(J jt,A f){V*v;
 if(!f||!(AT(f)&VERB))R 0;
 if(adisref(f))R 1;
 if(ISPRIM(f))R 0;
 v=FAV(f);
 if(v->id==CCOLONE)R 1;
 if(ADISAGENDA(v)){
  DO(AN(v->fgh[0]), if(jtadhasexpl(jt,AAV(v->fgh[0])[i]))R 1;)
 }
 R jtadhasexpl(jt,v->fgh[0])||jtadhasexpl(jt,v->fgh[1])||(v->id==CFORK&&jtadhasexpl(jt,v->fgh[2]));
}
ADLOCWRAPS(B,adhasexpl,1)

// Is the shape of the result of f determined by shapes of its arguments whatever their values?
static B jtadshape1(J,A);
static B jtadshape2(J,A,B,B);

static B jtadshape1b(J jt,A f){V*v; A fs,gs,hs;
 if(!f||!(AT(f)&VERB)||adisref(f))R 0;
 if(adatomic1(f))R 1;
 v=FAV(f); fs=v->fgh[0]; gs=v->fgh[1]; hs=v->fgh[2];
 if(ISPRIM(f))switch(v->id){
  case CCOMMA: case CCOMDOT: case CLAMIN: case CREV: case CCANT: case CHEAD: case CTAIL: case CBEHEAD: case CCTAIL:
  case CLEFT: case CRIGHT: case CPOUND: case CDOLLAR: case CGRADE: case CDGRADE: case CDOMINO: case CBASE: case CNE:
   R 1; // , ,. ,: |. |: {. {: }. }: [ ] # $ /: \: %. #. ~:
  default: R 0;
 }
 switch(v->id){
 case CPOWOP: // u^:n for  n >= 0
  if(!(AT(fs)&VERB)||!(AT(gs)&B01+INT+FL)||AR(gs))R 0;
  if(AT(gs)&FL){D d=DAV(gs)[0]; if(!(d>=0&&d==jround(d)))R 0;} // _ is fine here: once the shape stops changing it stays
  else if(AT(gs)&INT&&IAV(gs)[0]<0)R 0;
  R jtadshape1(jt,fs);
 case CQQ: case CBSLASH: case CBSDOT: // u"r, u\, u\.
  R AT(fs)&VERB&&jtadshape1(jt,fs);
 case CAT: case CATCO: case CAMP: case CAMPCO:
  if((v->id==CAT||v->id==CATCO)&&(AT(fs)&NOUN||AT(gs)&NOUN))R 0; // u@n
  if(AT(fs)&NOUN)R jtadshape2(jt,gs,0,1); // m&v
  if(AT(gs)&NOUN)R jtadshape2(jt,fs,1,0); // u&n
  R jtadshape1(jt,fs)&&jtadshape1(jt,gs);
 case CFORK:
  if(!hs)R jtadshape1(jt,fs)&&jtadshape1(jt,gs);   // [: f g
  if(AT(fs)&NOUN)R jtadshape1(jt,hs)&&jtadshape2(jt,gs,0,1);
  R jtadshape1(jt,fs)&&jtadshape1(jt,hs)&&jtadshape2(jt,gs,1,1);
 case CHOOK: R jtadshape1(jt,gs)&&jtadshape2(jt,fs,1,1);
 case CTILDE: R AT(fs)&VERB&&jtadshape2(jt,fs,1,1);
 case CSLASH: R AT(fs)&VERB&&jtadshape2(jt,fs,1,1);
 default: R 0;
 }
}
ADLOCWRAPS(B,adshape1,0)

// vy (vx): 1 if the y (x) that f is given can differ from cell to cell
static B jtadshape2b(J jt,A f,B vx,B vy){V*v; A fs,gs,hs;
 if(!f||!(AT(f)&VERB)||adisref(f))R 0;
 if(!vx&&!vy)R 1;
 if(adatomic2(f))R 1;
 v=FAV(f); fs=v->fgh[0]; gs=v->fgh[1]; hs=v->fgh[2];
 if(ISPRIM(f))switch(v->id){
  case CCOMMA: case CCOMDOT: case CLAMIN: case CLEFT: case CRIGHT: case CROT: case CGRADE: case CDGRADE:
  case CEPS: case CIOTA: case CICO: case CMATCH: case CDOMINO: case CBASE: case CEBAR:
   R 1; // , ,. ,: [ ] |. /: \: e. i. i: -: %. #. E. p.
  case CTAKE: case CDROP: case CPOUND: case CDOLLAR: case CCANT: case CFROM: case CPOLY:
   R !vx; // {. }. # $ |: { p.  (the shape depends on the values of x)
  default: R 0;
 }
 switch(v->id){
 case CQQ: R AT(fs)&VERB&&jtadshape2(jt,fs,vx,vy);
 case CDOT: R !!adinnerid(f); // +/ .*  -/ .*
 case CAT: case CATCO:
  if(!(AT(fs)&VERB)||!(AT(gs)&VERB))R 0;
  R jtadshape2(jt,gs,vx,vy)&&jtadshape1(jt,fs);
 case CAMP: case CAMPCO:
  if(!(AT(fs)&VERB)||!(AT(gs)&VERB))R 0;
  R jtadshape1(jt,gs)&&jtadshape2(jt,fs,vx,vy);
 case CFORK:
  if(!hs)R jtadshape2(jt,gs,vx,vy)&&jtadshape1(jt,fs);
  if(AT(fs)&NOUN)R jtadshape2(jt,hs,vx,vy)&&jtadshape2(jt,gs,0,1);
  R jtadshape2(jt,fs,vx,vy)&&jtadshape2(jt,hs,vx,vy)&&jtadshape2(jt,gs,1,1);
 case CHOOK: R jtadshape1(jt,gs)&&jtadshape2(jt,fs,vx,vy);
 case CTILDE: R AT(fs)&VERB&&jtadshape2(jt,fs,vy,vx);
 case CSLASH: R AT(fs)&VERB&&jtadshape2(jt,fs,vx,vy);
 default: R 0;
 }
}
static B jtadshape2(J jt,A f,B vx,B vy){ADSTACKLOW(0)
 if(!adisref(f))R jtadshape2b(jt,f,vx,vy);
 ADLOC s[ADMAXLOC]; I n; B full;
 A g=jtadfollow(jt,f,s,&n,&full);
 B r=jtadshape2b(jt,g,vx,vy);
 DQ(n, jtadlocexit(jt,&s[i]);)
 R r;
}
// Compute f on every cell of y (cells are what follows first fr axes) together with derivative.
// Wherever it can, it does this with whole-array operations instead of one application of f per cell.
static I jtadliftb1(J jt,B rev,I k,A f,ADVAL y,I fr,ADVAL*z){
 if(adisref(f)){A g; ADLOC ls; // If f is a name then lift its value instead.
  RZ(g=jtadresin(jt,f,&ls));
  if(g!=f&&!ADEXPLV(g)){
   I r=jtadliftb1(jt,rev,k,g,y,fr,z);
   jtadlocexit(jt,&ls);
   R r;
  }
 }
 V*v=FAV(f); A fs=v->fgh[0],gs=v->fgh[1],hs=v->fgh[2]; I cr=AR(y.p)-fr; ADVAL a,b;
 if(adatomic1(f))R jtadap1(jt,rev,k,f,y,z); // frame doesn't matter so one application to the whole array
 if(fr&&!jtadhastraced(jt,f)&&(adconst1(f)||(rev?adstructm(f):adlinear1(f))))R jtadap1(jt,rev,k,qq(f,sc(cr)),y,z); // whole array as u"c
 B ex=jtadhasexpl(jt,f)||!AN(y.p); // explicit part or empty argument
 switch(v->id){
 case CQQ:
  if(!(AT(fs)&VERB))break;
  LIFT1(fs,y,fr+cr-efr(cr,adrankm(f)),z); R 1;
 case CAT: case CAMP:
  if(v->id==CAT&&(AT(fs)&NOUN||AT(gs)&NOUN))break; // u@n
  if(AT(fs)&NOUN){LIFT2(gs,jtadnoun(jt,fs),0,y,fr,z); R 1;} // m&v
  if(AT(gs)&NOUN){LIFT2(fs,y,fr,jtadnoun(jt,gs),0,z); R 1;} // u&n
  if(ex||!jtadshape1(jt,gs))break;
  {I nf=fr+cr-efr(cr,(I)v->mr); LIFT1(gs,y,nf,&b); LIFT1(fs,b,nf,z); R 1;} // u@v u&v so u on each result of v.
 case CATCO: case CAMPCO: // u@:v  u&:v infinite rank, u on the whole result of v for each cell
  if(v->id==CATCO&&(AT(fs)&NOUN||AT(gs)&NOUN))break;
  if(AT(fs)&NOUN){LIFT2(gs,jtadnoun(jt,fs),0,y,fr,z); R 1;}
  if(AT(gs)&NOUN){LIFT2(fs,y,fr,jtadnoun(jt,gs),0,z); R 1;}
  if(ex||!jtadshape1(jt,gs))break;
  LIFT1(gs,y,fr,&b); LIFT1(fs,b,fr,z); R 1;
 case CFORK:
  if(ex)break;
  if(!hs){ // [: f g
   if(!jtadshape1(jt,gs))break;
   LIFT1(gs,y,fr,&b); LIFT1(fs,b,fr,z); R 1;
  }
  if(!jtadshape1(jt,hs)||(AT(fs)&VERB&&!jtadshape1(jt,fs)))break;
  {I fa=fr; LIFT1(hs,y,fr,&b);
   if(AT(fs)&NOUN){a=jtadnoun(jt,fs); fa=0;
   }else LIFT1(fs,y,fr,&a);
   LIFT2(gs,a,fa,b,fr,z); R 1;}
 case CHOOK:
   if(ex||!jtadshape1(jt,gs))break;
   LIFT1(gs,y,fr,&b); LIFT2(fs,y,fr,b,fr,z); R 1;
 case CTILDE: if(!(AT(fs)&VERB))break; LIFT2(fs,y,fr,y,fr,z); R 1;
 case CSLASH:
  if(!(AT(fs)&VERB)||!adatomic2(fs)||!fr)break;
  if(!cr){*z=y; R 1;}
  {A perm; RZ(perm=over(sc(fr),over(apv(fr,0,1),apv(cr-1,fr+1,1))));
   AP2(ds(CCANT),jtadnoun(jt,perm),y,&b); AP1(f,b,z); R 1;}
 }
 // cell loop: cells of the frame or (entered with no frame) of the verb's own rank
 I s=fr?0:adrankm(f), c=fr?cr:efr(AR(y.p),s); // with a frame, each cell runs f with its own rank (f"r"c)
 if(rev){z->t=ADZ;
  ADNEGCELLS(I,f,s<0,0,(z->p=jtadrcells1(jt,g_,c,y.p))!=0)
 }
 ADNEGCELLS(I,f,s<0,0,jtadcells1(jt,g_,c,y.p,y.t,k,&z->p,&z->t))
}
static I jtadliftb2(J jt,B rev,I k,A f,ADVAL x,I fx,ADVAL y,I fy,ADVAL*z){
 if(adisref(f)){A g; ADLOC ls;
  RZ(g=jtadresin(jt,f,&ls));
  if(g!=f&&!ADEXPLV(g)){
   I r=jtadliftb2(jt,rev,k,g,x,fx,y,fy,z);
   jtadlocexit(jt,&ls);
   R r;
  }
 }
 V*v=FAV(f); A fs=v->fgh[0],gs=v->fgh[1],hs=v->fgh[2]; ADVAL a,b; I cx,cy,F;
 if(adatomic2(f)){
  if(!((!fx&&!AR(x.p))||(!fy&&!AR(y.p))))RZ(jtadframes(jt,rev,k,&x,&fx,&y,&fy));
  R jtadap2(jt,rev,k,f,x,y,z);
 }
 if((fx||fy)&&!jtadhastraced(jt,f)&&adconst2(f))R jtadap2(jt,rev,k,qq(f,v2(AR(x.p)-fx,AR(y.p)-fy)),x,y,z);
 F=MAX(fx,fy);
 B ex=jtadhasexpl(jt,f)||!AN(x.p)||!AN(y.p); // explicit part or empty argument
 switch(v->id){
 case CQQ:
  if(!(AT(fs)&VERB))break;
  RZ(jtadframes(jt,rev,k,&x,&fx,&y,&fy)); cx=AR(x.p)-fx; cy=AR(y.p)-fy;
  LIFT2(fs,x,fx+cx-efr(cx,adrankl(f)),y,fy+cy-efr(cy,adrankr(f)),z); R 1;
 case CAT:
  if(!(AT(fs)&VERB)||!(AT(gs)&VERB))break;
  if(ex||!jtadshape2(jt,gs,1,1))break;
  RZ(jtadframes(jt,rev,k,&x,&fx,&y,&fy));
  cx=AR(x.p)-fx; cy=AR(y.p)-fy;
  {I nx=fx+cx-efr(cx,(I)lr(f)),ny=fy+cy-efr(cy,(I)rr(f));
   LIFT2(gs,x,nx,y,ny,&b); LIFT1(fs,b,MAX(nx,ny),z); R 1;}
 case CAMP:
  if(!(AT(fs)&VERB)||!(AT(gs)&VERB))break;
  if(ex||!jtadshape1(jt,gs))break;
  RZ(jtadframes(jt,rev,k,&x,&fx,&y,&fy)); cx=AR(x.p)-fx; cy=AR(y.p)-fy;
  {I nx=fx+cx-efr(cx,(I)lr(f)),ny=fy+cy-efr(cy,(I)rr(f));
   LIFT1(gs,y,ny,&b); LIFT1(gs,x,nx,&a); LIFT2(fs,a,nx,b,ny,z); R 1;}
 case CATCO:
  if(!(AT(fs)&VERB)||!(AT(gs)&VERB))break;
  if(ex||!jtadshape2(jt,gs,1,1))break;
  LIFT2(gs,x,fx,y,fy,&b); LIFT1(fs,b,F,z); R 1;
 case CAMPCO:
  if(!(AT(fs)&VERB)||!(AT(gs)&VERB))break;
  if(ex||!jtadshape1(jt,gs))break;
  LIFT1(gs,y,fy,&b); LIFT1(gs,x,fx,&a); LIFT2(fs,a,fx,b,fy,z); R 1;
 case CFORK:
  if(ex)break;
  if(!hs){
   if(!jtadshape2(jt,gs,1,1))break;
   LIFT2(gs,x,fx,y,fy,&b); LIFT1(fs,b,F,z); R 1;
  }
  if(!jtadshape2(jt,hs,1,1)||(AT(fs)&VERB&&!jtadshape2(jt,fs,1,1)))break;
  {I fa=F; LIFT2(hs,x,fx,y,fy,&b);
   if(AT(fs)&NOUN){a=jtadnoun(jt,fs); fa=0;
   }else LIFT2(fs,x,fx,y,fy,&a);
   LIFT2(gs,a,fa,b,F,z); R 1;}
 case CHOOK:
   if(ex||!jtadshape1(jt,gs))break;
   LIFT1(gs,y,fy,&b); LIFT2(fs,x,fx,b,fy,z); R 1;
 case CTILDE:
   if(!(AT(fs)&VERB))break;
   LIFT2(fs,y,fy,x,fx,z); R 1;
 }
 // cell loop
 {I sl=fx||fy?0:adrankl(f), sr=fx||fy?0:adrankr(f);
  cx=fx||fy?AR(x.p)-fx:efr(AR(x.p),sl); cy=fx||fy?AR(y.p)-fy:efr(AR(y.p),sr);
  if(rev){z->t=ADZ;
   ADNEGCELLS(I,f,sl<0||sr<0,1,(z->p=jtadrcells2(jt,g_,cx,cy,x.p,y.p))!=0)
  }
  ADNEGCELLS(I,f,sl<0||sr<0,1,jtadcells2(jt,g_,cx,cy,x.p,x.t,y.p,y.t,k,&z->p,&z->t))}
}

// *************************************** parser hook ***************************************
// Called by the parser instead of the verb's action routine whenever a trace is active.

typedef struct {A*v; I n,cap;} ADPSET; // Set of arguments and noun operands and everything inside them, sorted by address

// Add one block to the set, and if it's boxed, everything inside it, at any depth.
static I jtadpsetadd(J jt,ADPSET*s,A x){
 STACKCHKOFL
 if(s->n==s->cap){I nc=s->cap?2*s->cap:64; A*nv=realloc(s->v,nc*sizeof(A)); if(!nv)R 0; s->v=nv; s->cap=nc;}
 s->v[s->n++]=x;
 if(AT(x)&BOX)DO(AN(x), A c=C(ADBOXE(x,i)); if(c&&!jtadpsetadd(jt,s,c))R 0;)
 R 1;
}
// Add verb f's noun operands to the set, the nouns built into the verb (3 in 3&+). It goes through compound verbs to find operands at any depth, but not through names or explicit definitions.
static I jtadpsetverb(J jt,ADPSET*s,A f){V*v;
 if(!f)R 1;
 if(AT(f)&NOUN)R jtadpsetadd(jt,s,f);
 if(!(AT(f)&VERB+ADV+CONJ)||ISPRIM(f)||adisref(f))R 1;
 v=FAV(f); if(v->id==CCOLONE)R 1;
 if(v->id==CMCAP)R jtadpsetverb(jt,s,v->fgh[0]); // u M. so its h is memo's tables not an operand
 R jtadpsetverb(jt,s,v->fgh[0])&&jtadpsetverb(jt,s,v->fgh[1])&&jtadpsetverb(jt,s,v->fgh[2]);
}
// Comparison function for qsort.
static int adpcmp(const void*a,const void*b){UI x=(UI)*(A*)a,y=(UI)*(A*)b; R x<y?-1:x>y;}
// Binary search: is address x in sorted set?
static B adpsethas(ADPSET*s,A x){I lo=0,hi=s->n-1;
 while(lo<=hi){I m=(lo+hi)>>1;
  if(s->v[m]==x)R 1;
  if((UI)s->v[m]<(UI)x)lo=m+1;
  else hi=m-1;
 }
 R 0;
}
// Check boxed result of verb that D. ran without a rule. It answers: could any box contain a number whose derivative was lost? It returns 1 if the result is safe.
static I jtadboxchk(J jt,A z,ADPSET*s){
 STACKCHKOFL
 DO(AN(z), A c=C(ADBOXE(z,i));
  if(!c||adpsethas(s,c)||ACISPERM(AC(c)))continue; // held in the arguments (traced or not) or a constant
  if(AT(c)&BOX){RZ(jtadboxchk(jt,c,s)); continue;}
  if(AT(c)&NUMERIC&&AN(c)){jtadrefusefinal(jt); R 0;} // number made while f ran is refused
 )
 R 1;
}
// Check behind the no-rule fallback. D. met a verb f it has no rule for, ran it once on the values and got the result z.
// This decides whether z can be used: is it certain no derivative was lost? It returns z if so, otherwise 0 with a final refusal.
static A jtadnorule(J jt,A f,A z,A x,A y){ADPSET s={0,0,0}; I r;
 if(!z)R 0;
 if(z==x||z==y)R z;
 if(AT(z)&BOX?0:!(AT(z)&NUMERIC)||!AN(z))R z; // text (not followed) or empty
 if((x&&!jtadpsetadd(jt,&s,x))||!jtadpsetadd(jt,&s,y)||!jtadpsetverb(jt,&s,f)){free(s.v); RE(0); ASSERT(0,EVWSFULL);}
 qsort(s.v,s.n,sizeof(A),adpcmp);
 if(AT(z)&BOX)r=jtadboxchk(jt,z,&s);
 else{r=adpsethas(&s,z); if(!r)jtadrefusefinal(jt);}
 free(s.v); R r?z:0;
}
// Is f an explicit definition?
static B jtadisexplb(J jt,A f){R f&&AT(f)&VERB&&ADEXPLV(f);}
ADLOCWRAP(B,adisexpl)

// A verb that runs its own computation which this level then differentiates operation by operation:
// an explicit definition or inner D.
static B jtadrunsownb(J jt,A f){R f&&AT(f)&VERB&&(ADEXPLV(f)||ISDCAP(f));}
ADLOCWRAP(B,adrunsown)

// Parser calls it instead of the verb when parser flag adtracing is set.
A jtadtracecall(J jt,I dyad,A a1,A a2,A fs,AF fn){
 if(unlikely(!ADT))R fn(jt,a1,dyad?a2:fs,fs); // flag without trace (cannot happen but if it did then the verb runs without tracing)
 jt->zombieval=0; // no assignment in place while tracing
 if(jtadisexpl(jt,fs))R fn(jt,a1,dyad?a2:fs,fs); // explicit definition: its own sentences are traced. The original call keeps locale handling
 R jtadapply(jt,fs,dyad?a1:0,dyad?a2:a1); // as J: a monad's argument is y
}

// Check whether multiple assignment 'a b' =. w or (names) =. w, can keep the derivatives in a boxed w
I jtadboxasgok(J jt,A w){
 if(!ADT||!(AT(w)&BOX)||AR(w)<2||!jtadholds(jt,w))R 1;
 ADASSERT(0); // multiple assignment of a traced value in boxes of rank 2 or more: not supported
}
// Return item j of the array t (j { t) in a form D. can follow if t is traced.
A jtaditemtraced(J jt,A t,I j){A T,z,Tz;
 if(!ADT)R 0;
 T=adtget(t);
 if(ISZ(T))R jtadtracedup(jt,t)?from(sc(j),t):0; // traced only by an outer level, so the selection is seen there
 if(ADT->rev)R jtadrec2(jt,ds(CFROM),sc(j),t); // reverse mode: record the selection
 RZ(z=from(sc(j),t)); RZ(Tz=jtaditem(jt,T,j));
 R adtreg(z,Tz);
}

// *************************************** conjunction ***************************************

// Compute Jacobian of u at y with x held constant using forward mode
// If u is atomic the Jacobian is diagonal and we return just its diagonal (shape $P), using one seed direction of all 1s.
static A jtadjacobian(J jt,A u,A x,A y,B dyad,B atomic){A T,P,Tz; I k;
 if(atomic){k=1; RZ(T=reshape(over(num(1),shape(y)),scf(1.0)));} // seed T is (1,$y) $ 1.0
 else{k=AN(y); RZ(T=reshape(over(sc(k),shape(y)),take(sc(k+1),num(1))));} // seed T is (k,$y) $ (k+1) {. 1 which is the identity, one seed direction per atom of y
 if(ADT)ADT->k=k; // The number of directions is stored in the trace.
 if(dyad){RZ(jtadfwd2(jt,u,x,ADZ,y,T,k,&P,&Tz));}
 else{RZ(jtadfwd1(jt,u,y,T,k,&P,&Tz));}
 ASSERT((AT(P)&NUMERIC&&!(AT(P)&CMPX))||!AN(P),EVDOMAIN); // u's result must be real
 // Turn tangent into the result.
 if(atomic){
  if(ISZ(Tz))R reshape(shape(P),scf(0.0)); // If the tangent is ADZ then u didn't depend on y, so the result is zeros shaped like P.
  RZ(Tz=adexpand(Tz,P)); // Extend the tangent to the full shape of P.
  R head(Tz); // Remove the one direction axis.
 }
 if(ISZ(Tz))R reshape(over(shape(P),shape(y)),scf(0.0)); // A zero tangent gives a zero Jacobian of shape ($P),$y.
 RZ(Tz=cant2(num(0),Tz)); // Direction axis to the end.
 R reshape(over(shape(P),shape(y)),Tz); // Split that last axis back into the shape of y, since k is */$y. The result has shape ($P),$y.
}

static I jtadtinit(J jt,ADTRACE*t){
 memset(t,0,sizeof(*t)); t->prev=jt->adtrace; t->cap=64;
 t->keys=calloc(t->cap,sizeof(A)); t->tans=calloc(t->cap,sizeof(A));
 if(!t->keys||!t->tans){free(t->keys); free(t->tans); ASSERT(0,EVWSFULL);}
 R 1;
}
// Turn diagonals into full Jacobians. It's the last step of the provably-atomic shortcut.
// d holds the diagonals of per-cell Jacobians: shape F,C (frame F, cells C). Result: shape F,C,C with d on each diagonal.
static A jtaddiag(J jt,A d,I fr){I nf=1,nc=1,rc=AR(d)-fr; A z,s; D*dv,*zv;
 RZ(d=cvt(FL,d));
 DO(fr, nf*=AS(d)[i];);
 DO(rc, nc*=AS(d)[fr+i];);
 RZ(s=over(shape(d),drop(sc(fr),shape(d))));
 if(UPTR(d)){A id,cs; // an outer level watches
  RZ(cs=drop(sc(fr),shape(d)));
  RZ(id=reshape(over(cs,cs),take(sc(nc+1),num(1)))); // (cs,cs) $ (nc+1) {. 1 (the identity on cells of shape cs)
  R cvt(FL,admap2(ds(CSTAR),d,rc,id,2*rc));
 }
 RZ(z=reshape(s,scf(0.0)));
 dv=DAV(d); zv=DAV(z);
 DO(nf, D*zc=zv+i*nc*nc; D*dc=dv+i*nc; DO(nc, zc[i*nc+i]=dc[i];))
 R z;
}
// Take the diagonal out of that full Jacobian (only when forced reverse).
static A jtaddiagof(J jt,A z,A y){I n=AN(y),np,r; A d,sp,ix;
 RZ(z=cvt(FL,z));
 np=n?AN(z)/n:0; r=n?np/n:1; RZ(sp=take(sc(AR(z)-AR(y)),shape(z)));
 if(UPTR(z)){I*iv;
  GATV0(ix,INT,np,1); iv=IAV(ix);
  DO(np, iv[i]=i*n+i/r;)
  R reshape(sp,from(ix,ravel(z)));
 }
 RZ(d=reshape(sp,scf(0.0)));
 DO(np, DAV(d)[i]=DAV(z)[i*n+i/r];)
 R d;
}

// The mode of one D.
enum {ADMAUTO, ADMFWD, ADMREV};

// ADMAUTO chooses: forward when y has one atom or u is atomic; otherwise u is recorded once, and reverse mode
// finishes the job if the result has fewer atoms than y (gradient case), forward mode if not.
// ADMFWD forces forward mode, ADMREV reverse. dg>=0: the result (diagonals) is embedded in Jacobians of cells with frame length dg
static A jtadtraced(J jt,A u,A x,A y,B dyad,B atomic,I mode,I dg){A z=0,P; ADTRACE t; C otr=jt->parserstackframe.adtracing;
 if(!(AT(y)&(B01|INT|FL)))RZ(y=cvt(FL,y));
 RZ(jtadtinit(jt,&t));
 jt->adtrace=&t; jt->parserstackframe.adtracing=1;
 if(!(y=ca(y)))goto exit; // the traced block is a copy of y (the array passed may also be a global or an operand that u reads which must stay constant)
 if(mode==ADMREV||(mode==ADMAUTO&&!atomic&&AN(y)>1)){
  z=jtadjacrev(jt,u,x,y,dyad,&P,mode==ADMAUTO);
  if(z&&atomic)z=jtaddiagof(jt,z,y); // forced reverse on atomic verb
 }else z=jtadjacobian(jt,u,x,y,dyad,atomic);
 if(z)z=cvt(FL,z);
 if(z&&dg>=0)z=jtaddiag(jt,z,dg);
exit:
 jt->adtrace=t.prev; jt->parserstackframe.adtracing=otr; // restore before anything can return
 if(z)ra(z); // the result may be a block the trace holds, so keep it through the cleanup
 jtadtfree(jt,&t);
 if(z)tpush(z);
 R z;
}
// what mode D. should use when it starts: automatic, forward or reverse?
static I jtadmode(J jt){I d=0;
 for(ADTRACE*t=ADT;t;t=t->prev)++d;
 R jt->admodes[MIN(d,(I)sizeof(I)-1)];
}
// 9!:_10 query the switch: one mode per nesting depth, outermost first (the last applies to deeper levels too)
F1(jtadmodeq){F12IP;A z; ASSERTMTV(w);
 GATV0(z,INT,sizeof(I),1);
 DO(sizeof(I), C m=jt->admodes[i]; IAV(z)[i]=m==ADMREV?-1:m==ADMFWD?1:0;)
 R z;
}
// 9!:_11 set it: 0 automatic, 1 forward, _1 reverse; a list gives the outer levels first. Per thread. For testing
F1(jtadmodes){F12IP;I n;
 RZ(w=vi(w)); n=AN(w);
 ASSERT(AR(w)<=1,EVRANK); ASSERT(n<=(I)sizeof(I),EVLENGTH);
 DO(n, ASSERT(BETWEENC(IAV(w)[i],-1,1),EVDOMAIN);)
 DO(sizeof(I), I m=n?IAV(w)[MIN(i,n-1)]:0; jt->admodes[i]=m<0?ADMREV:m>0?ADMFWD:ADMAUTO;)
 R mtm;
}
static DF1(jtdcap1cell){DF12IP;PROLOG(0);A z;
 RZ(z=jtadtraced(jt,DCAPU(self),0,w,0,0,jtadmode(jt),-1));
 EPILOG(z);
}
static DF2(jtdcap2cell){DF12IP;PROLOG(0);A z;
 RZ(z=jtadtraced(jt,DCAPU(self),a,w,1,0,jtadmode(jt),-1));
 EPILOG(z);
}
// Build verb that D. runs on each cell of y.
static A jtadnegcellv(J jt,A self,B dyad,ADLOC*ls,I*nls){A f,v; I n=IAV(FAV(self)->fgh[1])[0]; B full;
 f=jtadfollow(jt,FAV(self)->fgh[0],ls,nls,&full);
 ASSERT(!full,EVLIMIT); // more than ADMAXLOC locale switches to reach u's definition
 if(!(f=jtadfollowdcap(jt,f,ls,nls))){jtadlocexitn(jt,ls,*nls); *nls=0; R 0;}
 if(!(v=jtadstripneg(jt,f,dyad))){jtadlocexitn(jt,ls,*nls); *nls=0; R 0;}
 if(v==f){jtadlocexitn(jt,ls,*nls); *nls=0; R self;}
 if(n>1)RZ(v=jtadmkdcap(jt,v,sc(n-1)));
 R fdef(0,CDCAPDOT,VERB,jtdcap1cell,jtdcap2cell,v,0,0,VFLAGNONE,RMAX,RMAX,RMAX);
}

// u D.n where y is array of boxes. Each box holds array of numbers (any shape, empty of any type allowed).
// The result gives one block per box:
// for n=1 it has the shape of y and box i holds ($u y),$>i{y
// for n=2 it has shape ($y),$y and box (i,j) holds ($u y),($>i{y),$>j{y (Hessian in blocks) and so on.
// D. is called from C, so an error from u or D.'s refusal is reported at the user's sentence as for unboxed y.
static const C adboxpos[]="(((+/\\ - ])&.,)@:(*/&>) +&.> i.&.>)@:($&.>)"; // positions
static const C adboxpack[]="; ,&.> , y"; // list
static const C adboxunp[]="[ {&.> <@]"; // boxes rebuilt
static const C adboxcut[]=":\n({\"(_,m)&y)@<&.> { m # < x"; // blocks
// Check the contents of boxed y
static I jtadboxychk(J jt,A w){A*wv=AAV(w);
 DO(AN(w), A c=C(wv[i]);
  if(!AN(c))continue; // empty (nothing to differentiate)
  ASSERT(!(AT(c)&BOX),EVNONCE);
  ASSERT(AT(c)&NUMERIC,EVDOMAIN);
 )
 R 1;
}
// The boxes of y with their contents in floating point, as an unboxed y is converted.
static A jtadboxyfl(J jt,A w){A*wv=AAV(w),z; I n=AN(w); B any=0;
 DO(n, if(!(AT(C(wv[i]))&FL))any=1;);
 if(!any)R w;
 RZ(z=jtadboxlist(jt,n));
 DO(n, A c=C(wv[i]);
  if(!(AT(c)&FL))RZ(c=cvt(FL,c));
  adboxset(z,i,c);
 );
 R reshape(shape(w),z);
}
// One cell of [x] u D.n y with y boxed. self is per-cell verb made in jtadboxed: fgh[0] is u, fgh[1] is n,
// fgh[2] a list of boxes holding verbs.
static A jtadboxcell(J jt,A a,A w,A self,B dyad){A u=FAV(self)->fgh[0],n=FAV(self)->fgh[1],*hv=AAV(FAV(self)->fgh[2]),p,f,d,dv,z;
 df1(p,w,hv[0]); RZ(p); // positions
 df1(f,w,hv[1]); RZ(f); // list
 RZ(d=jtadmkdcap(jt,atop(dyad?amp(a,u):u,hv[2]),n)); // monad u@unpack D.n or dyad x&u@unpack D.n
 df2(dv,p,f,d); RZ(dv); // derivative: shape ($u y),n#N
 df2(z,p,dv,hv[3]); // blocks
 R z;
}
static DF1(jtadbox1){DF12IP;PROLOG(0);A z; RZ(z=jtadboxcell(jt,0,w,self,0)); EPILOG(z);}
static DF2(jtadbox2){DF12IP;PROLOG(0);A z; RZ(z=jtadboxcell(jt,a,w,self,1)); EPILOG(z);}
// [x] u D.n y with y boxed. self is u D.n. Cells of y are taken at u's rank, as for unboxed y.
static A jtadboxed(J jt,A self,A a,A w,B dyad){A u=FAV(self)->fgh[0],n=FAV(self)->fgh[1],hv,v,b=0,z=0; I lc=0,rc; B neg; ADLOC ls[ADMAXLOC]; I nls=0;
 RZ(jtadboxychk(jt,w)); RZ(w=jtadboxyfl(jt,w));
 if(dyad){
  I sl=adrankl(u),sr=adrankr(u); lc=efr(AR(a),sl); rc=efr(AR(w),sr); neg=sl<0||sr<0;
 }else{
  I sr=adrankm(u); rc=efr(AR(w),sr); neg=sr<0;
 }
 if(neg)RZ(u=jtadnegv(jt,u,dyad,ls,&nls)); // rank is applied below, so cells run u without its negative rank
 if((hv=jtadboxlist(jt,4))){ // J verbs, made once
  if((v=eval((C*)adboxpos)))adboxset(hv,0,v);
  if(v&&(v=colon(num(3),cstr((C*)adboxpack))))adboxset(hv,1,v);
  if(v&&(v=eval((C*)adboxunp)))adboxset(hv,2,v);
  if(v&&(v=colon(num(1),cstr((C*)adboxcut)))&&(df1(v,n,v),v))adboxset(hv,3,v);
  if(v)b=fdef(0,CDCAPDOT,VERB,jtadbox1,jtadbox2,u,n,hv,VFLAGNONE,RMAX,RMAX,RMAX); // one cell
 }
 if(b&&(dyad?lc<AR(a)||rc<AR(w):rc<AR(w)))b=qq(b,dyad?v2(lc,rc):sc(rc)); // several cells: b at u's rank
 if(b){ // run b
  if(dyad)df2(z,a,w,b);
  else df1(z,w,b);
 }
 jtadlocexitn(jt,ls,nls);
 R z;
}
static DF1(jtdcap1){DF12IP;PROLOG(0);A u,z;
 if(ISSPARSE(AT(w)))RZ(w=denseit(w));
 if(AT(w)&BOX){RZ(z=jtadboxed(jt,self,0,w,0)); EPILOG(z);} // y boxed: one block per box
 u=DCAPU(self); // u for n=1, u D.(n-1) for n>1
 I s=adrankm(u), c=efr(AR(w),s); // effective (cell) rank
 if(adatomic1(u)){ // u provably works atom by atom
  RZ(z=jtadtraced(jt,u,0,w,0,1,jtadmode(jt),c?AR(w)-c:-1)); // Each cell's Jacobian is diagonal. One forward pass with a seed of 1s gives all the diagonal entries (shape $w). For cells of rank >0 they are placed on the diagonals of full cell Jacobians.
  EPILOG(z);
 }
 if(c<AR(w)){ADLOC ls[ADMAXLOC]; I nls; // w has more than one cell (c may be 0 when u not provably elementwise); one Jacobian per cell
  A g=jtadnegcellv(jt,self,0,ls,&nls);
  z=!g?0:c?rank1ex(w,g,c,jtdcap1cell):rank1ex0(w,g,jtdcap1cell);
  jtadlocexitn(jt,ls,nls);
  RZ(z);
  EPILOG(z);
 }
 RZ(z=jtadtraced(jt,u,0,w,0,0,jtadmode(jt),-1)); // Cell is the whole of y. Return the full Jacobian of shape ($u y),$y.
 EPILOG(z);
}
static DF2(jtdcap2){DF12IP;PROLOG(0);A u,z;
 if(ISSPARSE(AT(a)))RZ(a=denseit(a)); if(ISSPARSE(AT(w)))RZ(w=denseit(w));
 if(AT(w)&BOX){RZ(z=jtadboxed(jt,self,a,w,1)); EPILOG(z);} // y boxed: one block per box
 u=DCAPU(self);
 I sl=adrankl(u), sr=adrankr(u);
 I lc=efr(AR(a),sl), rc=efr(AR(w),sr); // effective (cell) ranks
 B at2=adatomic2(u); // is u provably an elementwise dyad
 B atomic=at2&&lc==0&&rc==0; // elementwise u with cell ranks 0 0: one pass over the whole arguments
 if(at2&&!atomic){
  // atomic u at higher rank: each cell Jacobian is diagonal when x pairs with y atom for atom,
  // i.e. x is an atom (any y frame), or x and y have the same frame and x's shape is a prefix of y's.
  // Then the whole result is one pass over the whole arguments with the diagonals embedded in the cell Jacobians.
  I fx=AR(a)-lc, fy=AR(w)-rc;
  B ok=AR(a)==0||(fx==fy&&AR(a)<=AR(w));
  if(ok){
   DO(AR(a), if(AS(a)[i]!=AS(w)[i])ok=0;)
  }
  if(ok){
   RZ(z=jtadtraced(jt,u,a,w,1,1,jtadmode(jt),rc?fy:-1));
   EPILOG(z);
  }
 }
 if(!atomic&&(lc<AR(a)||rc<AR(w))){ADLOC ls[ADMAXLOC]; I nls; // several cells and not the one-pass case: one Jacobian per pair of cells
  A g=jtadnegcellv(jt,self,1,ls,&nls);
  z=!g?0:(lc|rc)?rank2ex(a,w,g,lc,rc,lc,rc,jtdcap2cell):rank2ex0(a,w,g,jtdcap2cell);
  jtadlocexitn(jt,ls,nls);
  RZ(z); EPILOG(z);
 }
 RZ(z=jtadtraced(jt,u,a,w,1,atomic,jtadmode(jt),-1));
 EPILOG(z);
}

// u D. n for an integer atom n>=0. Returns the derived verb or u itself for n=0.
static A jtadmkdcap(J jt,A a,A w){I n=IAV(w)[0]; A v=0;
 if(n==0)R a; // u D.0 is u
 if(n>1)RZ(v=jtadmkdcap(jt,a,sc(n-1))); // for n>1, the verb to differentiate is u D.(n-1); for n=1 it is u
 // u D.n has ranks of u
 R fdef(0,CDCAPDOT,VERB,jtdcap1,jtdcap2,a,w,v,VFLAGNONE,mr(a),lr(a),rr(a));
}
#define ADMAXORDER 100 // u D.n is built (and walked) one level per order
F2(jtdcapdot){F12IP;
 ARGCHK2(a,w);
 ASSERT(AT(a)&VERB,EVDOMAIN);
 ASSERT(AT(w)&NOUN,EVDOMAIN);
 ASSERT(!AR(w),EVRANK);
 RZ(w=vib(w));
 ASSERT(IAV(w)[0]>=0,EVDOMAIN);
 ASSERT(IAV(w)[0]<=ADMAXORDER,EVLIMIT);
 R jtadmkdcap(jt,a,w);
}
