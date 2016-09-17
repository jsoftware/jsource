/* Copyright 1990-2009, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Conjunctions: Rank Associates                                           */

#include "j.h"

#define DR(r)           (0>r?RMAX:r)


I mr(A w){R VAV(w)->mr;}
I lr(A w){R VAV(w)->lr;}
I rr(A w){R VAV(w)->rr;}

// effective rank: ar is rank of argument, r is rank of verb (may be negative)
// result is rank of argument cell
I efr(I ar,I r){R 0>r?MAX(0,r+ar):MIN(r,ar);}

#define NEWYA   {GA(ya,at,acn,acr,as+af); uu=CAV(ya);}
#define NEWYW   {GA(yw,wt,wcn,wcr,ws+wf); vv=CAV(yw);}
#define MOVEYA  {MC(uu,u+=ak,ak); if(ab)RZ(ya=relocate((I)a-(I)ya,ya));}
#define MOVEYW  {MC(vv,v+=wk,wk); if(wb)RZ(yw=relocate((I)w-(I)yw,yw));}

A jtrank1ex(J jt,A w,A fs,I mr,AF f1){PROLOG(0041);A y,y0,yw,z;B wb;C*v,*vv;
    I k,mn,n=1,p,*s,wcn,wcr,wf,wk,wr,*ws,wt,yn,yr,*ys,yt;
 RZ(w);
 wt=AT(w);
 if(wt&SPARSE)R sprank1(w,fs,mr,f1);
 wr=AR(w); ws=AS(w); wcr=efr(wr,mr); wf=wr-wcr; wb=ARELATIVE(w);
 if(!wf)R CALL1(f1,w,fs);
 RE(wcn=prod(wcr,wf+ws)); wk=wcn*bp(wt); v=CAV(w)-wk; NEWYW;
 p=wf; s=ws; RE(mn=prod(wf,ws));
 if(AN(w))MOVEYW else RZ(yw=reshape(vec(INT,wcr,ws+wf),filler(w)));
#define VALENCE   1
#define TEMPLATE  0
#include "cr_t.h"
}

// General setup for verbs with IRS that do not go through jtirs[12]
// A verb u["n] using this function checks to see whether it has multiple cells; if so,
// it calls here, giving a callback; we split the arguents into cells and call the callback,
// which is often the same original function that called here.
A jtrank2ex(J jt,A a,A w,A fs,I lr,I rr,AF f2){PROLOG(0042);A y,y0,ya,yw,z;B ab,b,wb;
   C*u,*uu,*v,*vv;I acn,acr,af,ak,ar,*as,at,k,mn,n=1,p,q,*s,wcn,wcr,wf,wk,wr,*ws,wt,yn,yr,*ys,yt;
 RZ(a&&w);
 at=AT(a); wt=AT(w);
 if(at&SPARSE||wt&SPARSE)R sprank2(a,w,fs,lr,rr,f2);
 // ?r=rank, ?s->shape, ?cr=effective rank, ?f=#frame, ?b=relative flag, for each argument
 ar=AR(a); as=AS(a); acr=efr(ar,lr); af=ar-acr; ab=ARELATIVE(a);
 wr=AR(w); ws=AS(w); wcr=efr(wr,rr); wf=wr-wcr; wb=ARELATIVE(w);
 if(!af&&!wf)R CALL2(f2,a,w,fs);  // if there's only one cell, run on it, that's the result
 // multiple cells.  Loop through them.
 // ?cn=number of atoms in a cell, ?k=#bytes in a cell, uv point to one cell before aw data
 // Allocate y? to hold one cell of ?, with uu,vv pointing to the data of y?
 RE(acn=prod(acr,as+af)); ak=acn*bp(at); u=CAV(a)-ak; NEWYA;
 RE(wcn=prod(wcr,ws+wf)); wk=wcn*bp(wt); v=CAV(w)-wk; NEWYW;
 // b means 'w frame is larger'; p=#larger frame; q=#shorter frame; s->larger frame
 // mn=#cells in larger frame (& therefore #cells in result); n=# times to repeat each cell
 //  from shorter-frame argument
 b=af<=wf; p=b?wf:af; q=b?af:wf; s=b?ws:as; RE(mn=prod(p,s)); RE(n=prod(p-q,s+q));
 ASSERT(!ICMP(as,ws,q),EVLENGTH);  // error if frames are not same as prefix
 // Initialize y? to hold data for the first cell; but if ? is empty, set y? to a cell of fills
 if(AN(a))MOVEYA else RZ(ya=reshape(vec(INT,acr,as+af),filler(a)));
 if(AN(w))MOVEYW else RZ(yw=reshape(vec(INT,wcr,ws+wf),filler(w)));
#define VALENCE  2
#define TEMPLATE 0
#include "cr_t.h"
}

/* Integrated Rank Support                              */
/* f knows how to compute f"r                           */
/* jt->rank points to a 2-element vector of             */
/* (left, right (monadic)) ranks                        */
/* 0=jt->rank means f is not being called from rank     */
/* jt->rank is guaranteed positive                      */
/* jt->rank is guaranteed <: argument ranks             */
/* frame agreement is verified before invoking f        */
/* frames either match, or one is empty                 */
/* (i.e. prefix agreement invokes general case)         */

A jtirs1(J jt,A w,A fs,I m,AF f1){A z;I*old=jt->rank,rv[2],wr; 
 RZ(w);
 wr=AR(w); rv[1]=m=efr(wr,m);
 if(m>=wr)R CALL1(f1,w,fs);
 rv[0]=0;
 jt->rank=rv; z=CALL1(f1,w,fs); jt->rank=old; 
 R z;
}

// IRS setup for dyads x op y
// a is x, w is y
// fs is the f field of the verb (the verb to be applied repeatedly) - or 0 if none
// l, r are nominal ranks of fs
// f2 is a setup verb (jtover, jtreshape, etc)
A jtirs2(J jt,A a,A w,A fs,I l,I r,AF f2){A z;I af,ar,*old=jt->rank,rv[2],wf,wr;
 // push the jt->rank (pointer to ranks) stack.  push/pop may not match, no problem
 RZ(a&&w);
 ar=AR(a); rv[0]=l=efr(ar,l); af=ar-l;  // get rank, effective rank, length of frame...
 wr=AR(w); rv[1]=r=efr(wr,r); wf=wr-r;     // ...for both args
 if(!(af||wf))R CALL2(f2,a,w,fs);   // if no frame, call setup verb and return result
 ASSERT(!ICMP(AS(a),AS(w),MIN(af,wf)),EVLENGTH);   // verify agreement
 /* if(af&&wf&&af!=wf)R rank2ex(a,w,fs,l,r,f2); */
 jt->rank=rv; z=CALL2(f2,a,w,fs); jt->rank=old;   // save ranks, call setup verb, pop rank stack
  // Not all setup verbs (*f2)() use the fs argument.  
 R z;
}


static DF1(cons1a){R VAV(self)->f;}

static DF2(cons2a){R VAV(self)->f;}

static DF1(cons1){V*sv=VAV(self);
 RZ(w);
 R rank1ex(w,self,efr(AR(w),*AV(sv->h)),cons1a);
}

static DF2(cons2){V*sv=VAV(self);I*v=AV(sv->h);
 RZ(a&&w);
 R rank2ex(a,w,self,efr(AR(a),v[1]),efr(AR(w),v[2]),cons2a);
}

static DF1(rank1i){DECLF;A h=sv->h;I*v=AV(h); R irs1(w,fs,*v,f1);}

static DF2(rank2i){DECLF;A h=sv->h;I*v=AV(h); R irs2(a,w,fs,v[1],v[2],f2);}

static DF1(rank1){DECLF;A h=sv->h;I m,*v=AV(h),wr;
 RZ(w);
 wr=AR(w); m=efr(wr,v[0]);
 R m<wr?rank1ex(w,fs,m,f1):CALL1(f1,w,fs);
}

static DF2(rank2){DECLF;A h=sv->h;I ar,l,r,*v=AV(h),wr;
 RZ(a&&w);
 ar=AR(a); l=efr(ar,v[1]);
 wr=AR(w); r=efr(wr,v[2]);
 R l<ar||r<wr?rank2ex(a,w,fs,l,r,f2):CALL2(f2,a,w,fs);
}

/* obsolete
// Select action routines and flags for u"n based on the verb u and the ranks n.  r0 is rank being installed in the monad
static void qqset(A a,I r0,AF*f1,AF*f2,I*flag){A f,g;C c,d,e,p,q;I m=0;V*v;
 static C at1[]={CFLOOR,CLE,CCEIL,CGE,CPLUS,CPLUSDOT,CPLUSCO, 
   CSTAR,CSTARDOT,CSTARCO,CMINUS,CNOT,CHALVE,CDIV,CSQRT,CEXP,CLOG, 
   CSTILE,CBANG,CLEFT,CRIGHT,CQUERY,CHGEOM,CJDOT,CCIRCLE, 
   CPCO,CQCO,CRDOT,CTDOT,CXCO,0};  // f monad <-> f"r monad
 static C ir1[]={CCOMMA,CLAMIN,CLEFT,CRIGHT,CCANT,CROT,CTAKE,CDROP,CGRADE,CDGRADE,
   CBOX,CNE,CTAIL,CCTAIL,CSLASH,CBSLASH,CBSDOT,CCOMDOT,CPCO,CATDOT,0};
 static C ir2[]={CCOMMA,CLAMIN,CLEFT,CRIGHT,CCANT,CROT,CTAKE,CDROP,CGRADE,CDGRADE,
   CDOLLAR,CPOUND,CIOTA,CICO,CEPS,CLBRACE,CMATCH,
   CEQ,CLT,CMIN,CLE,CGT,CMAX,CGE,CPLUS,CPLUSDOT,CPLUSCO,CSTAR,CSTARDOT,CSTARCO,
   CMINUS,CDIV,CEXP,CNE,CSTILE,CBANG,CCIRCLE,0};
 // For noun u, set flags to empty and use the constant routines
 if(NOUN&AT(a)){*f1=cons1; *f2=cons2; *flag=0; R;}
 // Verb u.  Calculate m, the flags to use for u.  We set VIRSx if the verb is a primitive with known
 // IRS, or if u has a IRS flags set (except when that flag came from execution of ")
 v=VAV(a); c=v->id;
 if(strchr(ir1,c))m+=VIRS1;
 if(strchr(ir2,c))m+=VIRS2;
 if(!(m&VIRS1)&&v->flag&VIRS1&&c!=CQQ)m+=VIRS1;
 if(!(m&VIRS2)&&v->flag&VIRS2&&c!=CQQ)m+=VIRS2;
 // If u does not have intrinsic IRS, see if it is a combination that has IRS
 if(!m){
  p=0; if(f=v->f){d=ID(f);p=VERB&AT(f)&&strchr(ir2,d);};  // p=1 if f has IRS2
  q=0; if(g=v->g){e=ID(g);q=VERB&AT(g)&&strchr(ir2,e);};  // q=1 if g has IRS2
  switch(c){
   case CFIT:   if(p&&d!=CEXP)m+=VIRS2; if(d==CNE)m+=VIRS1; break;  // u!.n, preserve IRS2 except for ^!.n; preserve IRS1 for ~:!.n (nub sieve)
   case CTILDE: if(p)m+=VIRS1+VIRS2; break;   // u~, set IRS if dyad u has IRS2
   case CAMP:   if(p&&NOUN&AT(g)||q&&NOUN&AT(f))m+=VIRS1; break;   // u&n or m&v, preserve IRS2 of the verb 
   case CFORK:  if(v->f1==(AF)jtmean)m+=VIRS1;   // mean is also supported by IRS1  (kludge - should set flag when mean detected)
 }}

 // Set m to the verb flags, which we will use to select the action routine, based on whether u supports IRS.
 // VISATOMIC1 overrides IRS1.
 I nm = v->flag&(VIRS1|VIRS2|VISATOMIC1);
 if(nm&VISATOMIC1)nm|=VIRS1;  // scaf
 if((~nm)&(m&(VIRS1|VIRS2)))
  *((I*)0)=0;  // scaf
 
 // We have m.  Now decide the return routines.  If the rank is nugatory, skip it & preserve the original routine pointer.
 // Otherwise, use the driver routine that executes irs? or rank?ex for the verb
 // I think this can be improved, by detecting the nugatory case by looking at the verb ranks.  This code may go back to
 // the time when arithmetic verbs had infinite rank.
 // Preserve the INPLACE flags from u - but only when irs etc can handle them!
 // not yet m |= v->flag&(VINPLACEOK1|VINPLACEOK2);
// obsolete *f1=strchr(at1,c) ? v->f1 : m&VIRS1 ? rank1i : rank1;
 *f1=nm&VISATOMIC1 ? v->f1 : nm&VIRS1 ? rank1i : rank1;
 *f2=                        nm&VIRS2 ? rank2i : rank2; 
 *flag=0;  // u"n itself does not support IRS - it IS IRS, and requires explicit rank is rank is applied to it
}
*/

// a"w; result is a verb
F2(jtqq){A h,t;AF f1,f2;D*d;I *hv,n,r[3],vf,*v;
 RZ(a&&w);
 // The h value in the function will hold the ranks.  Allocate it
 GAT(h,INT,3,1,0); hv=AV(h);  // hv->rank[0]
 if(VERB&AT(w)){
  // verb v.  Extract the ranks into a floating-point list
  GAT(t,FL,3,1,0); d=DAV(t);
  n=r[0]=hv[0]=mr(w); d[0]=n<=-RMAX?-inf:RMAX<=n?inf:n;
  n=r[1]=hv[1]=lr(w); d[1]=n<=-RMAX?-inf:RMAX<=n?inf:n;
  n=r[2]=hv[2]=rr(w); d[2]=n<=-RMAX?-inf:RMAX<=n?inf:n;
  // The floating-list is what we will call the v operand
  // h is the integer version
  w=t;
 }else{
  // Noun v. Extract and turn into 3 values, stored in h
  n=AN(w);
  ASSERT(1>=AR(w),EVRANK);
  ASSERT(0<n&&n<4,EVLENGTH);
  RZ(t=vib(w)); v=AV(t);
  hv[0]=v[2==n]; r[0]=DR(hv[0]);
  hv[1]=v[3==n]; r[1]=DR(hv[1]);
  hv[2]=v[n-1];  r[2]=DR(hv[2]);
 }

 // Get the action routines and flags to use for the derived verb
 if(NOUN&AT(a)){f1=cons1; f2=cons2; ACIPNO(a); vf=VFLAGNONE; // use the constant routines for nouns; mark the constant non-inplaceable since it may be reused
 }else{
  // The flags for u indicate its IRS and atomic status.  If atomic (for monads only), ignore the rank, just point to
  // the action routine for the verb.  Otherwise, choose the appropriate rank routine, depending on whether the verb
  // supports IRS.
  V* av=VAV(a);   // point to verb info
  f1=av->flag&VISATOMIC1 ? av->f1 : av->flag&VIRS1 ? rank1i : rank1;
  f2=av->flag&VIRS2 ? rank2i : rank2; 
  vf=av->flag&VASGSAFE;  // inherit ASGSAFE from u
 }

 // Create the derived verb.  The derived verb (u"n) NEVER supports IRS; not does it (yet) support inplace ops
 R fdef(CQQ,VERB, f1,f2, a,w,h, vf, r[0],r[1],r[2]);
}
