/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Macros and Defined-Constants (for Adverbs and Conjunctions)    */


#define GAPPEND         (I)0
#define GINSERT         (I)3
#define GTRAIN          (I)6

#define COMPOSE(c)  (BETWEENC((c),CAMPCO,CAT))   // &: & @: @ 

#define CONJCASET(at,wt) ((((at)>>(VERBX-1))+((wt)>>VERBX))&3)
#define CONJCASE(a,w)   CONJCASET(AT(a),AT(w))
#define NN              CONJCASET(NOUN,NOUN)               /* NOUN NOUN                       */
#define NV              CONJCASET(NOUN,VERB)               /* NOUN VERB                       */
#define VN              CONJCASET(VERB,NOUN)               /* VERB NOUN                       */
#define VV              CONJCASET(VERB,VERB)               /* VERB VERB                       */

// If there are multiple cells, loop over them & call back; otherwise fall through to handle to single cell
// Use PREF[12] when you don't know the rank; otherwise F[12]RANK(IP) directly
#define PREF1(f)        {I m=mr(self);            F1RANK(  m,f,self);}
#define PREF2(f)        {I l=lr(self),r=rr(self); F2RANK(l,r,f,self);}
#define PREF1IP(f)      {I m=mr(self);            F1RANKIP(  m,f,self);}
#define PREF2IP(f)      {I l=lr(self),r=rr(self); F2RANKIP(l,r,f,self);}


#define CDERIVF(id,f1,f2,flag,flag2,m,l,r)  fdef(flag2,id,VERB,(f1)?(AF)(f1):(AF)jtvalenceerr,(f2)?(AF)(f2):(AF)jtvalenceerr,a,w ,0L,(flag),(I)(m),(I)(l),(I)(r))
#define CDERIV(id,f1,f2,flag,m,l,r)  CDERIVF(id,f1,f2,flag,0,m,l,r)

// Assertions in conjunctions, to check operand types
#define ASSERTVV(a,w)   ARGCHK2(a,w); ASSERT(VERB&AT(a)&AT(w),EVDOMAIN)
#define ASSERTVVn(a,w)  ARGCHK2(a,w); ASSERT(VERB&AT(a),EVDOMAIN)
#define ASSERTVN(a,w)   ARGCHK2(a,w); ASSERT((VERB&AT(a))>(VERB&AT(w)),EVDOMAIN)
#define ASSERTNN(a,w)   ARGCHK2(a,w); ASSERT(!(VERB&(AT(a)|AT(w))),EVDOMAIN)


// TRUE if v is c!.0.  This is used for detecting use of special code.
#define FIT0(c,v)       (CFIT==v->id&&c==FAV(v->fgh[0])->id&&v->localuse.lu1.cct==1.0)
