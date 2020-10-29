/* Copyright 1990-2004, Jsoftware Inc.  All rights reserved.               */
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

#define DECLF            V* RESTRICT sv=FAV(self); AD * RESTRICT fs=sv->fgh[0];  \
                        AF f1=fs?FAV(fs)->valencefns[0]:0,f2=fs?FAV(fs)->valencefns[1]:0

#define DECLG           V* RESTRICT sv=FAV(self);AD * RESTRICT gs=sv->fgh[1];  \
                        AF g1=gs?FAV(gs)->valencefns[0]:0,g2=gs?FAV(gs)->valencefns[1]:0

#define DECLFG          DECLF;         AD * RESTRICT gs=sv->fgh[1];  \
                        AF g1=gs?FAV(gs)->valencefns[0]:0,g2=gs?FAV(gs)->valencefns[1]:0

#define DECLFGH         DECLFG;        AD * RESTRICT hs=sv->fgh[2];  \
                        AF h1=hs?FAV(hs)->valencefns[0]:0,h2=hs?FAV(hs)->valencefns[1]:0

// If there are multiple cells, loop over them & call back; otherwise fall through to handle to single cell
#define PREF1(f)        {I m=mr(self);            F1RANK(  m,f,self);}
#define PREF2(f)        {I l=lr(self),r=rr(self); F2RANK(l,r,f,self);}
#define PREF1IP(f)      {I m=mr(self);            F1RANKIP(  m,f,self);}
#define PREF2IP(f)      {I l=lr(self),r=rr(self); F2RANKIP(l,r,f,self);}

#define AS1(f,exp,x)      DF1(f){F1PREFIP;PROLOG(x);DECLF ;A z; PREF1(f); z=(exp); EPILOG(z);}
#define AS2(f,exp,x)      DF2(f){F2PREFIP;PROLOG(x);DECLF ;A z; PREF2(f); z=(exp); EPILOG(z);}
#define CS1(f,exp,x)      DF1(f){PROLOG(x);DECLFG;A z; PREF1(f); z=(exp); EPILOG(z);}
#define CS2(f,exp,x)      DF2(f){PROLOG(x);DECLFG;A z; PREF2(f); z=(exp); EPILOG(z);}
// implied rank loops.  If the arg has frame, loop over the cells.  f##cell operates on a single cell with no rank check
// inplaceability is passed through
#define CS1IP(lpclass,f,exp,x) CS1IPext(lpclass,static,f,exp,x)
#define CS1IPext(lpclass,cellclass,f,exp,x) cellclass DF1(f##cell){F1PREFIP;DECLFG;A z;PROLOG(x); exp; EPILOG(z);} lpclass DF1(f){PREF1(f##cell); R f##cell(jt,w,self);}
#define CS2IP(cellclass,class,f,exp,x) cellclass DF2(f##cell){F2PREFIP;DECLFG;A z;PROLOG(x); exp; EPILOG(z);} class DF2(f){PREF2(f##cell); R f##cell(jt,a,w,self);}



#define ADERIV(id,f1,f2,flag,m,l,r)  fdef(0,id,VERB,(AF)(f1),(AF)(f2),w,0L,0L,(flag),(I)(m),(I)(l),(I)(r))
#define CDERIV(id,f1,f2,flag,m,l,r)  fdef(0,id,VERB,(AF)(f1),(AF)(f2),a,w ,0L,(flag),(I)(m),(I)(l),(I)(r))

#define ASSERTVV(a,w)   ARGCHK2(a,w); ASSERT(VERB&AT(a)&AT(w),EVDOMAIN)
#define ASSERTVVn(a,w)  ARGCHK2(a,w); ASSERT(VERB&AT(a),EVDOMAIN)
#define ASSERTVN(a,w)   ARGCHK2(a,w); ASSERT((VERB&AT(a))>(VERB&AT(w)),EVDOMAIN)
#define ASSERTNN(a,w)   ARGCHK2(a,w); ASSERT(!(VERB&(AT(a)|AT(w))),EVDOMAIN)


// TRUE if v is c!.0.  This is used for detecting use of special code.
#define FIT0(c,v)       (CFIT==v->id&&c==ID(v->fgh[0])&&v->localuse.lD==1.0)
