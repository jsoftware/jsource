/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Parsing; see APL Dictionary, pp. 12-13 & 38.                            */

#include "j.h"
#include "p.h"
#include <stdint.h>

#define RECURSIVERESULTSCHECK   // obsolete
//  if(y&&(AT(y)&NOUN)&&!(AFLAG(y)&AFVIRTUAL)&&((AT(y)^AFLAG(y))&RECURSIBLE))SEGFAULT;  // stop if nonrecursive noun result detected


#define PARSERSTKALLO (490*sizeof(PSTK))  // number of stack entries to allocate, when we allocate, in bytes

/* parsing benchmark

parsing & name lookup
9!:5 :: 0: 1
namelkp =: 3 : 0
totl =. 0.
for_i. y do. totl =. totl + 1. + 2. + (+/ % #) i end.
totl
)
9!:5 :: 0: 0
6!:2 'namelkp"2 i. 100 10000 4'

parsing only
namelkp =: 3 : 0
totl =. 0.
for. y do.
 totl =. totl + 1. + 2. + 3.
 totl =. * totl
 totl =. totl - 2
end.
totl
)
(10) 6!:2 'namelkp i. 1e6'

*/
/* NVR - named value reference                                          */
/* a value referenced in the parser which is the value of a name        */
/* (that is, in some symbol table).                                     */
/*                                                                      */
// jt->nvra      A block for NVR stack
// AAV1(jt->nvra)        the stack.  LSB in each is a flag
// AN(jt->nvra)  size of stack
/*                                                                      */
/* Each call of the parser records the current NVR stack top (nvrtop),  */
/* and pop stuff off the stack back to that top on exit       */
/*                                                                      */
// The nvr stack contains pointers to values, added as names are moved
// from the queue to the stack.  Local values are not pushed.

B jtparseinit(JS jjt, I nthreads){A x;
 I threadno; for(threadno=0;threadno<nthreads;++threadno){JJ jt=&jjt->threaddata[threadno];
  GAT0(x,INT,20,1); ACINIT(x,ACUC1) jt->nvra=x;   // Initial stack.  Size is doubled as needed
  // ras not required because this is called during initialization 
 }
 R 1;
}

// w is a block that looks ripe for in-place assignment.  We just have to make sure that it is not in use somewhere up the stack.
// It isn't, if (1) it isn't on the stack at all; (2) if it was put on the stack by the currently-executing sentence.  We call this
// routine only when we are checking inplacing for final assignments, or for virtual extension.  For final assignment the parser stack is guaranteed to be empty; so any
// use of the name that was called for by this sentence must be finished
I jtnotonupperstack(J jt, A w) {
 // w is known nonzero.  In-place assign it only if its NVR count is 1, indicating that only one sentence may have this name stacked
 // This test is not locked because if we inplace we have a race condition anyway; so we have to have higher-level means to avoid that
 if(likely((AM(w)&~AMFREED)==AMNV+AMNVRCT*1)){
  // see if name was stacked (for the only time) in this very sentence
  A *v=jt->parserstackframe.nvrotop+AAV1(jt->nvra);  // point to current-sentence region of the nvr area
  DQ(jt->parserstackframe.nvrtop-jt->parserstackframe.nvrotop, if(*v==w)R 1; ++v;);   // if name stacked in this sentence, that's OK
 }
 // not stacked here, so not reassignable here.  see if name was not stacked at all - that would be OK
 R !(AM(w)&(-(AM(w)&AMNV)<<1));   // return OK if name not using NV semantics or stackcount=0 (rare, since why did we think is might be inplacable?)
}


#define AVN   (     ADV+VERB+NOUN)
#define CAVN  (CONJ+ADV+VERB+NOUN)
#define EDGE  (MARK+ASGN+LPAR)

PT cases[] = {
 EDGE,      VERB,      NOUN, ANY,       0,  jtvmonad, 1,2,1,
 EDGE+AVN,  VERB,      VERB, NOUN,      0,  jtvmonad, 2,3,2,
 EDGE+AVN,  NOUN,      VERB, NOUN,      0,    jtvdyad,  1,3,2,
 EDGE+AVN,  VERB+NOUN, ADV,  ANY,       0,     jtvadv,   1,2,1,
 EDGE+AVN,  VERB+NOUN, CONJ, VERB+NOUN, 0,    jtvconj,  1,3,1,
 EDGE+AVN,  VERB+NOUN, VERB, VERB,      0, jtvfolk,  1,3,1,
 EDGE,      CAVN,      CAVN, ANY,       0,  jtvhook,  1,2,1,
 NAME+NOUN, ASGN,      CAVN, ANY,       0,      jtvis,    0,2,1,
 LPAR,      CAVN,      RPAR, ANY,       0,    jtvpunc,  0,2,0,
};
#define PN 0
#define PA 1
#define PC 2
#define PV 3
#define PM 4  // MARK
#define PNM 5  // NAME
#define PL 6
#define PR 7
#define PS 8  // ASGN without ASGNNAME
#define PSN 9 // ASGN+ASGNNAME
#define PX 255

// Tables to convert parsing type to mask of matching parse-table rows for each of the stack positions
// the AND of these gives the matched row (the end-of-table row is always matched)
// static const US ptcol[4][10] = {
//     PN     PA     PC     PV     PM     PNM    PL     PR     PS     PSN
// { 0x2BE, 0x23E, 0x200, 0x23E, 0x27F, 0x280, 0x37F, 0x200, 0x27F, 0x27F},
// { 0x37C, 0x340, 0x340, 0x37B, 0x200, 0x200, 0x200, 0x200, 0x280, 0x280},
// { 0x2C1, 0x2C8, 0x2D0, 0x2E6, 0x200, 0x200, 0x200, 0x300, 0x200, 0x200},
// { 0x3DF, 0x3C9, 0x3C9, 0x3F9, 0x3C9, 0x3C9, 0x3C9, 0x3C9, 0x3C9, 0x3C9},
// };
// Remove bits 8-9
// Distinguish PSN from PS by not having PSN in stack[3] support line 0 (OK since it must be preceded by NAME and thus will run line 7)
// Put something distictive into LPAR that can be used to create line 8
static const UI4 ptcol[11] = {  // there is a gap at SYMB.  CONW is used to hold ASGNNAME.  
// obsolete [LASTNOUNX-LASTNOUNX] = 0xBE7CC1DF,  // PN
// obsolete [ASGNX-LASTNOUNX] = 0x7F8000C9,  // PS
// obsolete [MARKX-LASTNOUNX] = 0x7F0000C9,  // PM
// obsolete [NAMEX-LASTNOUNX] = 0x800000C9,  // PNM
// obsolete [SYMBX-LASTNOUNX] = 0x7F8000C8,  // PS+NAME
// obsolete // gap [CONWX-LASTNOUNX]
// obsolete [LPARX-LASTNOUNX] = 0x7F000001,  // PL
// obsolete [VERBX-LASTNOUNX] = 0x3E7BE6F9,  // PV
// obsolete [// obsolete ADVX-LASTNOUNX] = 0x3E40C8C9,  // PA
// obsolete [CONJX-LASTNOUNX] = 0x0040D0C9,  // PC
// obsolete [RPARX-LASTNOUNX] = 0x000000C9  // PR
// obsolete // 0x7F8000C8  // PSN
[LASTNOUNX-LASTNOUNX] = 0xDFC17CBE,  // PN
[ASGNX-LASTNOUNX] = 0xC900807F,  // PS
[MARKX-LASTNOUNX] = 0xC900007F,  // PM
[NAMEX-LASTNOUNX] = 0xC9000080,  // PNM
[SYMBX-LASTNOUNX] = 0xC800807F,  // PS+NAME
// gap [CONWX-LASTNOUNX]
[LPARX-LASTNOUNX] = 0x0100007F,  // PL
[VERBX-LASTNOUNX] = 0xF9E67B3E,  // PV
[ADVX-LASTNOUNX] = 0xC9C8403E,  // PA
[CONJX-LASTNOUNX] = 0xC9D04000,  // PC
[RPARX-LASTNOUNX] = 0xC9000000  // PR
};


// tests for pt types
// obsolete #define PTMARK 0x7F0000C9
// obsolete #define PTASGNNAME 0x7F8000C8
// obsolete #define PTISCAVN(pt) (pt&0x4000)
// obsolete #define PTISM(s)  ((s).pt==PTMARK)
// obsolete #define PTOKEND(t2,t3) (((PTISCAVN(~(t2).pt))+((t3).pt^PTMARK))==0)  // t2 is CAVN and t3 is MARK
// obsolete #define PTISASGN(pt)  ((pt)&0x800000)
// obsolete #define PTISNOTASGNNAME(s)  (((s).pt&0x1))
#define PTMARK 0xC900007F
// obsolete #define PTASGNNAME 0xC800807F
#define PTISCAVNX 22
#define PTISCAVN(pt) ((pt)&(1LL<<PTISCAVNX))
#define PTISRPAR(pt) (((pt)&0x7fff)==0)
#define PTISM(s)  ((s).pt==PTMARK)
#define PTOKEND(t2,t3) (((PTISCAVN(~(t2).pt))+((t3).pt^PTMARK))==0)  // t2 is CAVN and t3 is MARK
#define PTISASGN(pt)  (((pt)&0x8000)<<(NAMEX-15))   // we compare against the NAMEX bit
#define PTISNOTASGNNAME(pt)  (((pt)>>24)&1)
#define PTNOTLPARX 27  // this bit is set for NOT LPAR 
#define PTLPARFLG 0x8000000  // this bit is set in pt only if NOT LPAR
// obsolete #define PTISRPAR(s)  ((s).pt<0x100)
// converting type field to pt, store in z
// obsolete #define PTFROMTYPE(z,t) {I pt=CTTZ(t); pt-=(LASTNOUNX+1); pt|=REPSGN(pt); z=ptcol[pt+1];}  // here when we know it's CAVN (not assignment)
#define PTFROMTYPE(z,t) {I pt=CTTZ(t); pt=(t)&(((1LL<<(LASTNOUNX+1))-1))?LASTNOUNX:pt; z=ptcol[pt-LASTNOUNX];}  // here when we know it's CAVN (not assignment)
// obsolete #define PTFROMTYPEASGN(z,t) {I pt=CTTZ(t); pt-=(LASTNOUNX+1); pt|=REPSGN(pt)|((t>>(CONWX-2))&(CONWX-(LASTNOUNX+1))); z=ptcol[pt+1];}  // clear flag bit if ASGN to name, by fetching from unused CONW hole, which is 4 above ASGN
// obsolete  // CONWX-2 moves CONW to its position in the table, which is at CONWX-(LASTNOUNX+1)=4
#define PTFROMTYPEASGN(z,t) {I pt=CTTZ(t); I nt=LASTNOUNX; nt=(t)&CONW?SYMBX:nt; pt=(t)&(CONW|((1LL<<(LASTNOUNX+1))-1))?nt:pt; z=ptcol[pt-LASTNOUNX];}  // clear flag bit if ASGN to name, by fetching from unused SYMB hole

static PSTK* jtpfork(J jt,PSTK *stack){
 A y=folk(stack[1].a,stack[2].a,stack[3].a);  // create the fork
RECURSIVERESULTSCHECK
 RZ(y);  // if error, return 0 stackpointer
 stack[3].t = stack[1].t; stack[3].a = y;  // take err tok from f; save result; no need to set parsertype, since it didn't change
 stack[2]=stack[0]; R stack+2;  // close up stack & return
}

static PSTK* jtphook(J jt,PSTK *stack){
 A y=hook(stack[1].a,stack[2].a);  // create the hook
RECURSIVERESULTSCHECK
 RZ(y);  // if error, return 0 stackpointer
 PTFROMTYPE(stack[2].pt,AT(y)) stack[2].t = stack[1].t; stack[2].a = y;  // take err tok from f; save result
 stack[1]=stack[0]; R stack+1;  // close up stack & return
}

// multiple assignment not to constant names.  self has parms.  ABACK(self) is the symbol table to assign to, valencefns[0] is preconditioning routine to open value or convert it to AR
static DF2(jtisf){RZ(symbis(onm(a),CALL1(FAV(self)->valencefns[0],w,0L),ABACK(self))); R num(0);} 

// assignment, single or multiple
// return sets stack[0] to -1 if this is a final assignment
static PSTK* jtis(J jt,PSTK *stack){
 I asgt=AT(stack[1].a); A v=stack[2].a, n=stack[0].a;  // value and name
 J jtinplace=(J)((I)jt+((stack[0].t==1)<<JTFINALASGNX));   // set JTFINALASGN if this is final assignment
 stack[1+(stack[0].t==1)].t=-1;  // if the word number of the lhs is 1, it's either (noun)=: or name=: or 'value'=: at the beginning of the line;
  // store -1 to the new stack[0].t.  Otherwise, make a harmless store to new stack[-1].t
 if(likely(jt->asginfo.assignsym!=0)){jtsymbis(jtinplace,n,v,(A)asgt);}   // Assign to the known name.  Pass in the type of the ASGN
 else {B ger=0;C *s;
  // Point to the block for the assignment; fetch the assignment pseudochar (=. or =:); choose the starting symbol table
  // depending on which type of assignment (but if there is no local symbol table, always use the global)
  A symtab=jt->locsyms; if(unlikely((SGNIF(asgt,ASGNLOCALX)&(1-AN(jt->locsyms)))>=0))symtab=jt->global;
  if(unlikely((AT(n)&BOXMULTIASSIGN)!=0)){
   // string assignment, where the NAME blocks have already been computed.  Use them.  The fast case is where we are assigning a boxed list
   if(AN(n)==1)n=AAV(n)[0];  // if there is only 1 name, treat this like simple assignment to first box, fall through
   else{
    // True multiple assignment
    ASSERT((-(AR(v))&(-(AN(n)^AS(v)[0])))>=0,EVLENGTH);   // v is atom, or length matches n
    if(((AR(v)^1)+(~AT(v)&BOX))==0){A *nv=AAV(n), *vv=AAV(v); DO(AN(n), jtsymbis(jtinplace,nv[i],vv[i],symtab);)}  // v is boxed list
    else {A *nv=AAV(n); DO(AN(n), jtsymbis(jtinplace,nv[i],ope(AR(v)?from(sc(i),v):v),symtab);)}  // repeat atomic v for each name, otherwise select item.  Open in either case
    goto retstack;
   }
  }
  // single assignment or variable assignment
  if(unlikely((SGNIF(AT(n),LITX)&(AR(n)-2))<0)){
   // lhs is ASCII characters, atom or list.  Convert it to words
   s=CAV(n); ger=CGRAVEC==s[0];   // s->1st character; remember if it is `
   RZ(n=words(ger?str(AN(n)-1,1+s):n));  // convert to words (discarding leading ` if present)
   ASSERT(AN(n)||(AR(v)&&!AS(v)[0]),EVILNAME);  // error if namelist empty or multiple assignment with no values, if there is something to be assigned
   if(1==AN(n)){
    // Only one name in the list.  If one-name AR assignment, leave as a list so we go through the AR-assignment path below
    if(!ger){RZ(n=head(n));}   // One-name normal assignment: make it a scalar, so we go through the name-assignment path & avoid unboxing
   }
  }
  // if simple assignment to a name (normal case), do it
  if(likely((NAME&AT(n))!=0)){
#if FORCEVIRTUALINPUTS
   // When forcing everything virtual, there is a problem with jtcasev, which converts its sentence to an inplace special.
   // The problem is that when the result is set to virtual, its backer does not appear in the NVR stack, and when the reassignment is
   // made the virtual block is dangling.  The workaround is to replace the block on the stack with the final value that was assigned:
   // not allowed in general because of (verb1 x verb2) name =: virtual - if verb2 assigns the name, the value going into verb1 will be freed before use
   stack[2].a=
#endif
   jtsymbis(jtinplace,n,v,symtab);
  }else{
   // computed name(s)
   ASSERT(AN(n)||(AR(v)&&!AS(v)[0]),EVILNAME);  // error if namelist empty or multiple assignment to no names, if there is something to be assigned
   // otherwise, if it's an assignment to an atomic computed name, convert the string to a name and do the single assignment
   if(!AR(n))jtsymbis(jtinplace,onm(n),v,symtab);
   else {
    // otherwise it's multiple assignment (could have just 1 name to assign, if it is AR assignment).
    // Verify rank 1.  For each lhs-rhs pair, do the assignment (in jtisf).
    // if it is AR assignment, apply jtfxx to each assignand, to convert AR to internal form
    // if not AR assignment, just open each box of rhs and assign
    ASSERT(1==AR(n),EVRANK); ASSERT(AT(v)&NOUN,EVDOMAIN);
    // create faux fs to pass args to the multiple-assignment function, in AM and valencefns
    PRIM asgfs; ABACK((A)&asgfs)=symtab; FAV((A)&asgfs)->flag2=0; FAV((A)&asgfs)->valencefns[0]=ger?jtfxx:jtope;   // pass in the symtab to assign, and whether w must be converted from AR.  flag2 must be 0 to satisfy rank2ex
    I rr=AR(v)-1; rr&=~REPSGN(rr); rank2ex(n,v,(A)&asgfs,0,rr,0,rr,jtisf);
   }
  }
 }
retstack:  // return, but 0 if error
 stack+=2; if(unlikely(jt->jerr))stack=0; R stack;  // the result is the same value that was assigned
}


#if AUDITEXECRESULTS
// go through a block to make sure that the descendants of a recursive block are all recursive, and that no descendant is virtual/unincorpable
// and that any block marked PRISTINE, if boxed, has DIRECT descendants with usecount 1
// Initial call has nonrecurok and virtok both set

void auditblock(J jt,A w, I nonrecurok, I virtok) {
 if(!w)R;
 if(AC(w)<0&&AZAPLOC(w)==0)SEGFAULT;
// if(AC(w)<0&&!(AFLAG(w)&AFVIRTUAL)&&AZAPLOC(w)>=jt->tnextpushp)SEGFAULT;  // requires large NTSTACK
 if(AC(w)<0&&!(AFLAG(w)&AFVIRTUAL)&&((I)AZAPLOC(w)<0x100000||(*AZAPLOC(w)!=0&&*AZAPLOC(w)!=w)))SEGFAULT;  // if no zaploc for inplaceable block, error
 I nonrecur = (AT(w)&RECURSIBLE) && ((AT(w)^AFLAG(w))&RECURSIBLE);  // recursible type, but not marked recursive
 if(AFLAG(w)&AFVIRTUAL && !(AFLAG(w)&AFUNINCORPABLE))if(AFLAG(ABACK(w))&AFVIRTUAL)SEGFAULT;  // make sure e real backer is valid and not virtual
 if(nonrecur&&!nonrecurok)SEGFAULT;
 if(AFLAG(w)&(AFVIRTUAL|AFUNINCORPABLE)&&!virtok)SEGFAULT;
 if(AT(w)==(I)0xdeadbeefdeadbeef)SEGFAULT;
 switch(CTTZ(AT(w))){
  case RATX:  
   {A*v=AAV(w); DO(2*AN(w), if(v[i])if(!(((AT(v[i])&NOUN)==INT) && !(AFLAG(v[i])&AFVIRTUAL)))SEGFAULT;);} break;
  case XNUMX:
   {A*v=AAV(w); DO(AN(w), if(v[i])if(!(((AT(v[i])&NOUN)==INT) && !(AFLAG(v[i])&AFVIRTUAL)))SEGFAULT;);} break;
  case BOXX:
   if(!(AFLAG(w)&AFNJA)){A*wv=AAV(w);
   DO(AN(w), if(wv[i]&&(AC(wv[i])<0))SEGFAULT;)
   I acbias=(AFLAG(w)&BOX)!=0;  // subtract 1 if recursive
   if(AFLAG(w)&AFPRISTINE){DO(AN(w), if(!((AT(wv[i])&DIRECT)>0))SEGFAULT;)}  // wv[i]&&(AC(w)-acbias)>1|| can't because other uses may be not deleted yet
   {DO(AN(w), auditblock(jt,wv[i],nonrecur,0););}
   }
   break;
  case VERBX: case ADVX:  case CONJX: 
   {V*v=VAV(w); auditblock(jt,v->fgh[0],nonrecur,0);
    auditblock(jt,v->fgh[1],nonrecur,0);
    auditblock(jt,v->fgh[2],nonrecur,0);} break;
  case B01X: case INTX: case FLX: case CMPXX: case LITX: case C2TX: case C4TX: case SBTX: case NAMEX: case SYMBX: case CONWX:
   if(ISSPARSE(AT(w))){P*v=PAV(w);  A x;
    if(!scheck(w))SEGFAULT;
    x = SPA(v,a); if(!(AT(x)&DIRECT))SEGFAULT; x = SPA(v,e); if(!((AT(x)&DIRECT)>0))SEGFAULT; x = SPA(v,i); if(!(AT(x)&DIRECT))SEGFAULT; x = SPA(v,x); if(!(AT(x)&DIRECT))SEGFAULT;
    auditblock(jt,SPA(v,a),nonrecur,0); auditblock(jt,SPA(v,e),nonrecur,0); auditblock(jt,SPA(v,i),nonrecur,0); auditblock(jt,SPA(v,x),nonrecur,0);
   }else if(NOUN & (AT(w) ^ (AT(w) & -AT(w))))SEGFAULT;
   break;
  case ASGNX: break;
  default: break; SEGFAULT;
 }
}
#endif




// Run parser, creating a new debug frame.  Explicit defs, which make other tests first, then go through jtparsea
// the result has bit 0 set if final assignment
// JT flags indicate whether call comes from ".
F1(jtparse){F1PREFIP;A z;
 ARGCHK1(w);
 A *queue=AAV(w); I m=AN(w);   // addr and length of sentence
 RZ(deba(DCPARSE,queue,(A)m,0L));  // We don't need a new stack frame if there is one already and debug is off
 z=jtparsea(jtinplace,queue,m);
 debz();
 R z;
}


#if FORCEVIRTUALINPUTS
// For wringing out places where virtual blocks are incorporated into results, we make virtual blocks show up all over
// any noun block that is not in-placeable and enabled for inplacing in jt will be replaced by a virtual block.  Then the audit of the
// result will catch any virtual blocks that slipped through into an incorporating entity.  sparse blocks cannot be virtualized.
// if ipok is set, inplaceable blocks WILL NOT be virtualized
A virtifnonip(J jt, I ipok, A buf) {
 RZ(buf);
 if(AT(buf)&NOUN && !(ipok && ACIPISOK(buf)) && !ISSPARSE(AT(buf)) && !(AFLAG(buf)&(AFNJA))) {A oldbuf=buf;
  buf=virtual(buf,0,AR(buf)); if(!buf && jt->jerr!=EVATTN && jt->jerr!=EVBREAK)SEGFAULT;  // replace non-inplaceable w with virtual block; shouldn't fail except for break testing
  I* RESTRICT s=AS(buf); I* RESTRICT os=AS(oldbuf); DO(AR(oldbuf), s[i]=os[i];);  // shape of virtual matches shape of w except for #items
    AN(buf)=AN(oldbuf);  // install # atoms
 }
 R buf;
}

// We intercept all the function calls, for this file only
static A virtdfs1(J jtip, A w, A self){
 J jt = (J)(intptr_t)((I)jtip&-4);  // estab legit jt
 w = virtifnonip(jt,(I)jtip&JTINPLACEW,w);
 R jtdfs1(jtip,w,self);
}
static A virtdfs2(J jtip, A a, A w, A self){
 J jt = (J)(intptr_t)((I)jtip&-4);  // estab legit jt
 a = virtifnonip(jt,(I)jtip&JTINPLACEA,a);
 w = virtifnonip(jt,(I)jtip&JTINPLACEW,w);
 R jtdfs2(jtip,a,w,self);
}
static A virtfolk(J jtip, A f, A g, A h){
 J jt = (J)(intptr_t)((I)jtip&-4);  // estab legit jt
 f = virtifnonip(jt,0,f);
 g = virtifnonip(jt,0,g);
 h = virtifnonip(jt,0,h);
 R jtfolk(jtip,f,g,h);
}
static A virthook(J jtip, A f, A g){
 J jt = (J)(intptr_t)((I)jtip&-4);  // estab legit jt
 f = virtifnonip(jt,0,f);
 g = virtifnonip(jt,0,g);
 R jthook(jtip,f,g);
}

// redefine the names for when they are used below
#define jtdfs1 virtdfs1
#define jtdfs2 virtdfs2
#define jtfolk virtfolk
#define jthook virthook
#endif

// name:: delete the symbol name but not deleting the value.  If the usecount of the value is 1 and it is not on the NVR stack, make it inplaceable.  Replace the nameref with a tpush or NVR free
static I namecoco(J jt, A y, I at, LX *locbuckets, L *s, A sv, L *sympv){F1PREFIP;
 if(((I)jtinplace&JTFROMEXEC))R 1;
 A fndst=UNLXAV0(locbuckets); if(unlikely((at&VERB)!=0))fndst=syrdforlocale(y);  // get locale to use.  This re-looks up global names, but they should be rare in name::
 LX *asymx=LXAV0(fndst)+SYMHASH(NAV(s->name)->hash,AN(fndst)-SYMLINFOSIZE);  // get pointer to index of start of chain; address of previous symbol in chain
 LX nextsymx=*asymx;  // symbol number pointed to by asymx, possibly w/permanent indicator
 while(sympv+SYMNEXT(nextsymx)!=s){asymx=&(sympv+SYMNEXT(nextsymx))->next; nextsymx=*asymx;}
 // asymx points to the chain field that points to s.  Bend the chain around s to delete symbol; clear val to delete name
 if(unlikely(s->flag&LCACHED)){*asymx=s->next;  // cached ref: remove its name, making it unmoored.  This ref can't make such a reference but a different ref could
 }else{
  s->val=0;  // remove the value ref, but don't touch the value itself
  if(likely(!SYMNEXTISPERM(nextsymx))){*asymx=s->next; fa(s->name); s->name=0; s->flag=0; s->sn=0; s->next=sympv[0].next; sympv[0].next=nextsymx;}
 }
 // the name is deleted, leaving the value.  Make the value inplaceable if there are no refs out against it.  If it is a local variable we think any outstanding
 // inplaceable refs will last till the end of the sentence, where we tpop the value
 if(likely((AM(sv)&-AMNVRCT)==0)){
  // value is not on NVR stack.  Schedule a deletion for the end of the sentence; if this name is the ONLY reference to the value,
  // make the value inplaceable (and point AM to the death warrant to allow early free)
  // The difference between this code and jtis() is that we defer the free using the tpop stack rather than the NVR stack
  if(likely(AC(sv)==ACUC1)){ACSET(sv,ACINPLACE+ACUC1); AZAPLOC(sv)=jt->tnextpushp;}
  tpushnr(sv);  // install death warrant for this block after the sentence completes
 }else{
  // value is on NVR stack.  We cannot free it here or even at the end of the sentence.  Mark a deferred free. but if there is already a deferred free, just fa()
  if(AM(sv)&AMFREED){fa(sv)}else{AMNVROR(sv,AMFREED)}
 }
 R 1;  // always good return.  Any error comes from tpush, where it is fatal
}

#define FP goto failparse;   // indicate parse failure and exit
#define EP goto exitparse;   // exit parser, preserving current status
#define EPZ(x) if(unlikely(!(x))){FP}   // exit parser if x==0

#if 0  // keep for commentary
// An in-place operation requires an inplaceable argument, which is marked as AC<0,
// Blocks are born non-inplaceable; they need to be marked inplaceable by the creator.
// Blocks that are assigned are marked not-inplaceable.
// But an in-placeable argument is not enough;
// The key point is that an in-place operation is connected to a FUNCTION CALL as well as a DATA BLOCK.
// For example, in (+/ % #) <: y the result of <: y has a usecount of 1 and is
// not used anywhere else, so it is passed into the fork as an inplaceable argument.
// This same block is not inplaceable when it is passed into # (because it will be needed later by +/)
// but it can be inplaceable when passed into +/.
//
// By setting the inplaceable flag in a result, a verb warrants that the block does not contain and is not contained in
// any other block.
//
// The 2 LSBs of jt are set to indicate inplaceability of arguments.  The caller sets them when e
// has no further use for the argument AND e knows that the callee can handle in-place arguments.
// An argument is inplaceable only if it is marked as such in the block AND in jt.
// A caller should set the bit in jt only if it knows that the argument is inplaceable, which
// will be true if (1) the block was created by the caller; or (2) the block was an argument to the caller with jt
// marked to indicate its inplaceability
//
// Bit 0 of jt is for w, bit 1 for a.
//
// There is one more piece to the inplace system: reassigned names.  When there is an
// assignment to a name, the block being reassigned can be reused for the output if:
//   the usecount is 1
//   the current execution is the only thing on the stack
//   the name can be resolved before the execution
// Resolving the name is vexed, because the execution might change the current locale, or the value
// of an indirect locative.  Moreover, the global name may be on the stack in higher stack frames,
// and assigning the name would change those values before they are executed.  Safest, therefore,
// would be to detect in-place assignment to local names only; but that would not support name =: name , blah
// which currently executes in-place (though with the aliasing problem mentioned above).
//
// The first rule is to have no truck with inplacing unless the execution is known to be locative-safe,
// i. e. will not change locale, path, or any name that might go into a locative.  This is marked by a
// flag ASGSAFE in the verb.

// Any assignment to a name is resolved to an address when the copula is encountered and there
//  is only one execution on the stack.  This resolution will always succeed for a local assignment to a name.
//  For a global assignment to a locative, it may fail, or may resolve to an address that is different from
//  the correct address after the execution.  The address of the L block for the symbol to be assigned is stored in jt->asginfo.assignsym.
//
// [As a time-saving maneuver, we store jt->asginfo.assignsym even if the name is not in-placeable because of its type or usecount.
// We can use jt->asginfo.assignsym to avoid re-looking-up the name.]
//
// If jt->asginfo.assignsym is set, the (necessarily inplaceable) verb may choose to perform an in-place
// operation.  It will check usecounts and addresses to decide whether to do this, and it bears the responsibility
// of worrying about names on the stack.  Note that local names are not put onto the stack, so absence of AFNVR suffices for them.
#endif
// extend NVR stack, returning the A block for it.  stack error on fail, since that's the likely cause
A jtextnvr(J jt){ASSERT(jt->parserstackframe.nvrtop<32000,EVSTACK); RZ(jt->nvra = ext(1, jt->nvra));  R jt->nvra;}

#define BACKMARKS 3   // amount of space to leave for marks at the end.  Because we stack 3 words before we start to parse, we will
 // never see 4 marks on the stack - the most we can have is 1 value + 3 marks.
#define FRONTMARKS 1  // amount of space to leave for front-of-string mark
// Parse a J sentence.  Input is the queue of tokens
// Result has PARSERASGNX (bit 0) set if the last thing is an assignment
// JT flag is used to indicate execution from ". - we can't honor name:: then, or perhaps some assignments
A jtparsea(J jt, A *queue, I nwds){F1PREFIP;PSTK * RESTRICT stack;A z,*v;
 // jt->parsercurrtok must be set before executing anything that might fail; it holds the original
 // word number+1 of the token that failed.  jt->parsercurrtok is set before dispatching an action routine,
 // so that the information is available for formatting an error display
  // Save info for error typeout.  We save sentence info once, and token info for every executed fragment
 PFRAME oframe=jt->parserstackframe;   // save all the stack status
 jt->parserstackframe.parserqueue=queue; jt->parserstackframe.parserqueuelen=(US)nwds;  // addr & length of words being parsed
 if(likely(nwds>1)) {  // normal case where there is a fragment to parse
  // save $: stack.  The recursion point for $: is set on every verb execution here, and there's no need to restore it until the parse completes
  A savfs=jt->sf;  // push $: stack

  // allocate the stack.  No need to initialize it, except for the marks at the end, because we
  // never look at a stack location until we have moved from the queue to that position.
  // Each word gets two stack locations: first is the word itself, second the original word number+1
  // to use if there is an error on the word
  // If there is a stack inherited from the previous parse, and it is big enough to hold our queue, just use that.
  // The stack grows down
  if(unlikely((uintptr_t)jt->parserstackframe.parserstkend1-(uintptr_t)jt->parserstackframe.parserstkbgn < (nwds+BACKMARKS+FRONTMARKS)*sizeof(PSTK))){A y;
   ASSERT(nwds<65000,EVLIMIT);  // To keep the stack frame small, we limit the number of words of a sentence
   I allo = MAX((nwds+BACKMARKS+FRONTMARKS)*sizeof(PSTK),PARSERSTKALLO); // number of bytes to allocate.  Allow 4 marks: 1 at beginning, 3 at end
   GATV0(y,B01,allo,1);
   jt->parserstackframe.parserstkbgn=(PSTK*)AV(y);   // save start of data area
   // We are taking advantage of the fact the NORMAH is 7, and thus a rank-1 array is aligned on a boundary of its size
   jt->parserstackframe.parserstkend1=(PSTK*)((uintptr_t)jt->parserstackframe.parserstkbgn+allo);  // point to the end+1 of the allocation
   // We could worry about hysteresis to avoid reallocation of every call
  }
  stack=jt->parserstackframe.parserstkend1-BACKMARKS;   // start at the end, with 3 marks

  ++jt->parsercalls;  // now we are committed to full parse.  Push stacks.
// obsolete   US nvrotop=jt->parserstackframe.nvrotop=jt->parserstackframe.nvrtop;  // we have to keep the next-to-top nvr value visible for a subroutine.  It remains as we advance nvrtop.  Save in a local too for comp ease
  jt->parserstackframe.nvrotop=jt->parserstackframe.nvrtop;  // we have to keep the next-to-top nvr value visible for a subroutine.  It remains as we advance nvrtop.  Save in a local too for comp ease

  // We don't actually put a mark in the queue at the beginning.  When m goes down to 0, we move in a mark.

  // As names are dereferenced, they are added to the nvr queue.  To save time in the loop, we now
  // make sure there is enough room in the nvr queue to handle all the names we will encounter in
  // this sentence.  For simplicity's sake, we just assume the worst, that every word is a name, and
  // make sure there is that much space.  BUT if there were an enormous tacit sentence, that would be
  // very inefficient.  So, if the sentence is too long, we go through and count the number of names,
  // rather than using a poor upper bound.
  {UI4 maxnvrlen;
   if (likely(nwds < 128))maxnvrlen = (UI4)nwds;   // if short enough, assume they're all names
   else {
    maxnvrlen = 0;
    DQ(nwds, maxnvrlen+=(AT(queue[i])>>NAMEX)&1;)
   }
   // extend the nvr stack, doubling its size each time, till it can hold our names.  Don't let it get too big.  This code duplicated in 4!:55
   if(unlikely((I)(jt->parserstackframe.nvrtop+maxnvrlen) > AN(jt->nvra))){NOUNROLL do{RZ(extnvr());}while((I)(jt->parserstackframe.nvrtop+maxnvrlen) > AN(jt->nvra));}
  }

  queue+=nwds-1; A nexty=*queue--;   // Advance queueptr to last token.  It always points to the next value to fetch.  Then unroll the fetch loop one time

  // Set number of extra words to pull from the queue.  We always need 2 words after the first before a match is possible.
  UI pt0ecam = nwds+(3LL<<CONJX);  // mash into 1 register:  bit 32-63 stack0pt, bit 27,29-30 (VERB,CONJ,RPAR) es delayline, 17-18 AR flags from symtab, 16 set if virtual last token has been processed, 0-15 m (word# in sentence)
#define LOCSYMFLGX (17-ARNAMEADDEDX)
#define PMASKX (LOCSYMFLGX+ARLCLONEDX)  // 8 bits of pmask
  // The es delayline is 27|29,30.  Bit 30 means '2 more', either of 27/29 means 'one more'.  Bit 27 is set by the action routine to stack one extra after an AVN and is cleared by the stacking
  // debugging if(jt->parsercalls==0xdd)
  // debugging  jt->parsercalls=0xdd;
  // DO NOT RETURN from inside the parser loop.  Stacks must be processed.
  LX *locbuckets=LXAV0(jt->locsyms);  // the local symbol table cannot change during the parse
  // Move rank flags from locsyms to locbuckets low bits.  NOTE that locbuckets is always on an 8-byte boundary, even in 32-bit code.  Flag bits are 1-2
  pt0ecam += (AR(jt->locsyms)&(ARLCLONED|ARNAMEADDED))<<LOCSYMFLGX;  // insert clone/added flags into portmanteau vbl

  // We have the initial stack pointer.  Grow the stack down from there
#if SY_64
#define SETSTACK0PT(v) pt0ecam=(UI4)pt0ecam, pt0ecam|=(I)(v)<<32;
#define GETSTACK0PT (pt0ecam>>32)
#else
  UI4 stack0pt;
#define SETSTACK0PT(v) stack0pt=(v);
#define GETSTACK0PT stack0pt
#endif
  SETSTACK0PT(PTMARK);  // will hold the EDGE+AVN value, which doesn't change much and is stored late
  stack[0].pt = stack[1].pt = stack[2].pt = PTMARK;  // install initial ending marks.  word numbers and value pointers are unused

  // One of the bits of locbuckets gets moved to a register here, so we know register pressure isn't great
  while(1){  // till no more matches possible...

    // no executable fragment, pull from the queue.  If we pull ')', there is no way we can execute
    // anything till 2 more words have been pulled, so we pull them here to avoid parse overhead.
    // Likewise, if we pull a CONJ, we can safely pull 1 more here.  And first time through, we should
    // pull 2 words following the first one.
    // es holds the number of extra pulls required.

    // pt0ecam is settling from pt0 and will not be ready for several cycles.  Avoid using it in addresses

   do{I tmpes;
    // pull one value from the queue
    --stack;  // back up to new stack frame, where we will store the new word

    if(likely((pt0ecam&0xffff)!=0)){A y;     // if there is another valid token...
     // Move in the new word and check its type.  If it is a name that is not being assigned, resolve its
     // value.  m has the index of the word we just moved
// obsolete     I at=AT(y = queue[mes>>2]);   // fetch the next word from queue; pop the queue; extract the type, save as at
     I at=AT(nexty);  // get type of next word.  loop was unrolled once.  We can't keep nexty and nextat in regs so we just unroll one - pity; we have to wait for AT
     y=nexty; nexty = *queue--;    // fetch the next word from queue;
     L *sympv=JT(jt,sympv);  // symbol root can change during parse, but not during stacking of a single execution.  But the value spills, so we fetch it here where we have time to fill
      // The last fetch of nexty fetches from queue[-1].  This will not segfault, and we never come back to fetch from inside that bogus block.
     stack[0].t = (US)pt0ecam;  // install the original token number for the word
// obsolete      if(at&NAME){
// obsolete       if(!PTISASGN(stack[1])){L *s;  // Replace a name with its value, unless to left of ASGN.  This test is 'not assignment'
     if((at&NAME)>PTISASGN(GETSTACK0PT)){L *s;A sv;  // Replace a name with its value, unless to left of ASGN.  This test is 'name and not assignment' uses the fact that NAME flag is <= the flag for assignment
      // Name, not being assigned
      // Resolve the name.  If the name is x. m. u. etc, always resolve the name to its current value;
      // otherwise resolve nouns to values, and others to 'name~' references
      // To save some overhead, we inline this and do the analysis in a different order here
      // The important performance case is local names with bucket info.  Pull that out & do it without the call overhead
      // This code is copied from s.c, except for the symx: since that occurs only in explicit definitions we can get to it only through here
// obsolete       I locstflags=AR(UNLXAV0(locbuckets));  // flags from local symbol table
      if((((I)NAV(y)->symx-1)|SGNIF(pt0ecam,LOCSYMFLGX+ARLCLONEDX))>=0){  // if we are using primary table and there is a symbol stored there...
       s=sympv+(I)NAV(y)->symx;  // get address of symbol in primary table
       if(unlikely((sv=s->val)==0))goto rdglob;  // if value has not been assigned, ignore it.  Could just treat as undef
      }else if(likely(NAV(y)->bucket!=0)){I bx;
       if(0 <= (bx = ~NAV(y)->bucketx)){   // negative bucketx (now positive); skip that many items, and then you're at the right place.  This is the path for almost all local symbols
        s=locbuckets[NAV(y)->bucket]+sympv;  // fetch hashchain headptr, point to L for first symbol
        if(unlikely(bx>0)){NOUNROLL do{s = s->next+sympv;}while(--bx);}  // skip the prescribed number, which is usually 1
        if(unlikely((sv=s->val)==0))goto rdglob;  // if value has not been assigned, ignore it
       }else{
        // positive bucketx (now negative); that means skip that many items and then do name search.  This is set for words that were recognized as names but were not detected as assigned-to in the definition.  This is the path for global symbols
        // If no new names have been assigned since the table was created, we can skip this search, since it must fail (this is the path for words in z eg)
        if(likely(!(pt0ecam&(ARNAMEADDED<<LOCSYMFLGX))))goto rdglob;
        // from here on it is rare to find a name - usually they're globals defined elsewhere
        LX lx = locbuckets[NAV(y)->bucket];  // index of first block if any
        I m=NAV(y)->m; C* nm=NAV(y)->s; UI4 hsh=NAV(y)->hash;  // length/addr of name from name block
        if(unlikely(++bx!=0)){NOUNROLL do{lx = sympv[lx].next;}while(++bx);}  // rattle off the permanents, usually 1
        // Now lx is the index of the first name that might match.  Do the compares
        NOUNROLL while(1) {
         if(lx==0)goto rdglob;  // If we run off chain, go read from globals
         lx=SYMNEXT(lx);  // we are now into non-PERMANENT symbols & must clear the flag
         s = lx+sympv;  // symbol entry
         IFCMPNAME(NAV(s->name),nm,m,hsh,{if(unlikely((sv=s->val)==0))goto rdglob; break;})  // if match, we're done looking; could be not found, if no value
         lx = s->next;
        }
        // Here there was a value in the local symbol table.  Skip to use it
       }
      }else{
       // No bucket info.  Usually this is a locative/global, but it could be an explicit modifier, console level, or ".
       // If the name has a cached reference, use it
       if(NAV(y)->cachedref!=0){  // if the user doesn't care enough to turn on caching, performance must not be that important
        // Note: this cannot be a NAMEABANDON, because such a name is never stacked where it can have the cachedref filled in
        A cachead=NAV(y)->cachedref; // use the cached address
        if(unlikely(NAV(y)->flag&NMCACHEDSYM)){cachead=(A)((sympv[(I)cachead]).val); if(unlikely(!cachead)){jsignal(EVVALUE);FP}}  // if it's a symbol index, fetch that.  value error only if cached symbol deleted
        y=cachead; at=AT(y); goto endname; // take its type, proceed
       }
rdglob: ;  // here when we tried the buckets and failed
       jt->parserstackframe.parsercurrtok = (US)pt0ecam;  // syrd can fail, so we have to set the error-word number (before it is decremented) before calling
       s=syrdnobuckets(y);  // do full symbol lookup, knowing that we have checked for buckets already
        // In case the name is assigned during this sentence (including subroutines), remember the data block that the name created
        // NOTE: the nvr stack may have been relocated by action routines, so we must refer to the global value of the base pointer
        // Stack a named value only once.  This is needed only for names whose VALUE is put onto the stack (i. e. a noun); if we stack a REFERENCE
        // (via namerefacv), no special protection is needed.  And, it is not needed for local names, because they are inaccessible to deletion in called
        // functions (that is, the user should not use u. to delete a local name).  If a local name is deleted, we always defer the deletion till the end of the sentence, easier than checking
        // When NVR is set, AM is used to hold the count of NVR stacking, so we can't have NVR and NJA both set.  User manages NJAs separately anyway
       if(likely(s!=0)){
        if(likely((sv=s->val)!=0)){
         if(AT(sv)&NOUN){ 
          // Normally local variables never get close to here because they have bucket info.  But if they are computed assignments,
          // or inside eval strings, they may come through this path.  If one of them is y, it might be virtual.  Thus, we must make sure we don't
          // damage AM in that case.  We don't need NVR then, because locals never need NVR.  Similarly, an LABANDONED name does not have NVR semantics, so leave it alone
          if(likely(!(AFLAG(sv)&AFNJA+AFVIRTUAL)))if(likely((AM(sv)&AMNV)!=0)){
           // NOTE that if the name was deleted in another task s->val will be invalid and we will crash
           AMNVRINCR(sv)  // add 1 to the NVR count, now that we are stacking
           AAV1(jt->nvra)[jt->parserstackframe.nvrtop++] = sv;   // record the place where the value was protected, so we can free it when this sentence completes
          }  // if NJA/virtual, leave NVR alone
         }
        }else goto undefname;  // no val
       }else goto undefname;  // no sym
       at|=VERB;  // indicate that the symbol was not in the local table
      }
      // end of looking at local/global symbol tables
      // s has the symbol for the name.  at&VERB is set if the name was found in a global table
      // since we have called subroutines, we don't use sympv, refetching it instead
// obsolete       if(likely(s!=0)){   // if symbol was defined...
      if(likely(1)){   // if symbol was defined...
// obsolete        A sv = s->val;  // pointer to value block for the name
// obsolete       EPZ(sv)  // symbol table entry, but no value.  Should not occur
       I svt=AT(sv);
         // Following the original parser, we assume this is an error that has been reported earlier.  No ASSERT here, since we must pop nvr stack
       // The name is defined.  If it's a noun, use its value (the common & fast case)
       // Or, for special names (x. u. etc) that are always stacked by value, keep the value
       // If a modifier has no names in its value, we will stack it by value.  The Dictionary says all modifiers are stacked by value, but
       // that will make for tough debugging.  We really want to minimize overhead for each/every/inv.
       // But: if the name is any kind of locative, we have to have a full nameref so unquote can switch locales: can't use the value then
       // Otherwise (normal adv/verb/conj name), replace with a 'name~' reference
       if((svt|at)&(NOUN|NAMEBYVALUE)){   // use value if noun or special name, or name::
        if(unlikely(at&NAMEABANDON))namecoco(jtinplace, y, at, locbuckets, s, sv, JT(jt,sympv));  // if name::, go delete the name, leaving the value to be deleted later
        y=sv; at=svt;
       }else if(unlikely(svt&NAMELESSMOD && !(NAV(y)->flag&NMLOC+NMILOC+NMIMPLOC+NMDOT))){
        // nameless modifier, and not a locative.  Don't create a reference; maybe cache the value
        if(NAV(y)->flag&NMCACHED){
         // cachable and not a locative (and not a noun).  store the value in the name, and flag that it's a symbol index, flag the value as cached in case it gets deleted
         NAV(y)->cachedref=(A)(s-JT(jt,sympv)); NAV(y)->flag|=NMCACHEDSYM; s->flag|=LCACHED; NAV(y)->bucket=0;  // clear bucket info so we will skip that search - this name is forever cached
        }
        y=sv; at=svt;
       }else{  // not a noun/nonlocative-nameless-modifier.  Make a reference
        y = namerefacv(y, s);   // Replace other acv with reference
        EPZ(y)
        at=AT(y);  // refresh the type with the type of the resolved name
       }
      } else {
undefname:
        // undefined name.  If special x. u. etc, that's fatal; otherwise create a dummy ref to [: (to have a verb)
        if(at&NAMEBYVALUE){jsignal(EVVALUE);FP}  // Report error (Musn't ASSERT: need to pop all stacks) and quit
        y = namerefacv(y, s);    // this will create a ref to undefined name as verb [:
        EPZ(y)
          // if syrd gave an error, namerefacv may return 0.  This will have previously signaled an error
        at=AT(y);  // refresh the type with the type of the resolved name
      }
endname: ;
     }
     // names have been resolved
// obsolete      } else {
     // Look to see if it is ) or a conjunction,
     // which allow 2 or 1 more pulls from the queue without checking for an executable fragment.
// obsolete       // NOTE that we are using the original type for the word, which will be stale if the word was a
// obsolete       // name that was replaced by name resolution.  We don't care - RPAR was never a name to begin with, and CONJ
// obsolete       // is much more likely to be a primitive; and we don't want to take the time to refetch the resolved type
// obsolete       I es=mes&3; mes=(mes&-4); es=(at>>CONJX)&3?(at>>CONJX)&3:es; mes+=es;  // calculate pull count es (2 if RPAR, 1 if CONJ, 0 otherwise); use it if not 0, else keep old value
     --pt0ecam;  //  decrement token# for the word we just processed
     pt0ecam|=at&(3LL<<CONJX);  // calculate pull count es (2 if RPAR, 1 if CONJ, 0 otherwise); OR it in: 00= no more, 01=1 more, 1x=2 more
     pt0ecam&=~VERB|-(at&ADV+VERB+NOUN);  // if the action routine left VERB set, it means we should stack another word of we stack an AVN
// obsolete      }
     tmpes=pt0ecam;  // pt0ecam is going to be settling because of stack0pt.  To ratify the branch faster we save the relevant part

     // y has the resolved value, which is never a NAME unless there is an assignment immediately following.
     // Put it onto the stack along with a code indicating part of speech and the token number of the word
     PTFROMTYPEASGN(at,at); stack[0].pt=at; SETSTACK0PT(at)   // stack the internal type too.  We split the ASGN types into with/without name to speed up IPSETZOMB.  Save pt in a register to avoid store forwarding.
     // at has been destroyed
         // and to reduce required initialization of marks.  Here we take advantage of the fact the CONW is set as a flag ONLY in ASGN type, and that PSN-PS is 1
     stack[0].a = y;   // finish setting the stack entry, with the new word
    }else{  // No more tokens.  If m was 0, we are at the (virtual) mark; otherwise we are finished
      if(!(pt0ecam&0x10000)){pt0ecam|=0x10000; stack[0].pt=PTMARK; SETSTACK0PT(PTMARK) break;}  // first time m=0.  realize the virtual mark and use it.  a and pt will not be needed.  e and ca flags immaterial
      EP       // second time.  there's nothing more to pull, parse is over.  This is the normal end-of-parse
      // never fall through here
    }
    if(!(tmpes&(VERB|(3LL<<CONJX))))break;  // exit stack phase when no more to do, leaving es=0
// obsolete     --mes;  // decr es count if not 0
    pt0ecam=(pt0ecam&~(VERB|(3LL<<CONJX)))|((tmpes>>1)&(1LL<<CONJX));  // bits 30/29/27: 1xx->010 others->000
   }while(1);  // Repeat if more pulls required.  We also exit with stack==0 if there is an error
   // words have been pulled from queue.

  // Now execute fragments as long as there is one to execute
   while(1) {
    // This is where we execute the action routine.  We give it the stack frame; it is responsible
    // for finding its arguments on the stack, storing the result (if no error) over the last
    // stack entry, then closing up any gap between the front-of-stack and the executed fragment,
    // and finally returning the new front-of-stack pointer

    // First, create the bitmask of parser lines that are eligible to execute
// obsolete     I pmask=(((~stack0pt)&0x80)*2)+((stack0pt>>24) & (stack[1].pt>>16) & (stack[2].pt>>8) & stack[3].pt);  // bit 8 is set ONLY for LPAR
// obsolete     I pmask=(((~stack0pt)&0x80)*2)+((stack0pt>>24) & (I)((C*)&stack[1].pt)[2] & (I)((C*)&stack[2].pt)[1]  & (I)((C*)&stack[3].pt)[0] );  // bit 8 is set ONLY for LPAR
    I pmask=(I)((C*)&stack[1].pt)[1] & (I)((C*)&stack[2].pt)[2]  & (I)((C*)&stack[3].pt)[3];  // bit 8 is set ONLY for LPAR
// obsolete     pmask=(((~stack0pt)&0x80)*2)+((stack0pt>>24)&pmask);
    pmask=(pmask|PTLPARFLG)&(GETSTACK0PT^PTLPARFLG);  // low 8 bits are lines0-7; LPAR is at some higher noncontiguous location
    if(!pmask)break;  // If all 0, nothing is dispatchable, go push next word
    A stk1a=stack[1].a, stk2a=stack[2].a;  // fetch these as early as possible

    // We are going to execute an action routine.  This will be an indirect branch, and it will mispredict.  To reduce the cost of the misprediction,
    // we want to pile up as many instructions as we can before the branch, preferably getting out of the way as many loads as possible so that they can finish
    // during the pipeline restart.  The perfect scenario would be that the branch restarts while the loads for the stack arguments are still loading.
    // We also have a couple of branches before the indirect branch, and we try to similarly get some computation going before them
    I pline=CTTZ(pmask);  // Get the # of the highest-priority line
// obsolete     pt0ecam&=~(0xff<<PMASKX); pt0ecam|=(pmask&=0xff)<<PMASKX;  // move flags into portmanteau register
    // Save the stackpointer in case there are calls to parse in the names we execute
    jt->parserstackframe.parserstkend1=stack;
    // Fill in the token# (in case of error) based on the line# we are running
    jt->parserstackframe.parsercurrtok = stack[((I)0x056A9>>(pline*2))&3].t;   // in order 9-0: 0 0 1 1 1 2 2 2 2 1->00 00 01 01 01 10 10 10 10 01->0000 0101 0110 1010 1001
    if(pmask&0x1F){
     // Here for lines 0-4, which execute the entity pointed to by fs
     A fs=stk2a; fs=pmask&1?stk1a:fs;  // pointer to the A block for the entity about to be executed
// obsolete      I fsflag=FAV(fs)->flag;  // fetch flags early - we usually need them
     PSTK *stackfs=stack+2-(pmask&1);  // stackpointer for the executing word: 1 2 2 2 2
//     AF actionfn=FAV(fs)->valencefns[(pline&2)>>1];  // the routine we will execute.  It's going to take longer to read this than we can fill before the branch is mispredicted, usually
     // We will be making a bivalent call to the action routine; it will be w,fs,fs for monads and a,w,fs for dyads (with appropriate changes for modifiers).  Fetch those arguments
     // We have fs already.  arg1 will come from position 2 3 1 1 1 depending on stack line; arg2 will come from 1 2 3 2 3
     if(pline<3){A y;  // lines 0 1 2, verb execution
      // Verb execution (in order: V N, V V N, N V N).  We must support inplacing, including assignment in place, and support recursion
// obsolete       pline|=(fsflag>>(pline>>1))&VJTFLGOK1;  // insert VJTFLGOK1 flag if the selected valence is inplaceable
      I dyad=pline>>1;
      pline|=FAV(fs)->flag&VJTFLGOK1+VJTFLGOK2+VASGSAFE;  // insert flags into pline to save reg
      // If it is an inplaceable assignment to a known name that has a value, remember the name and the value
      // We handle =: N V N, =: V N, =: V V N.  In the last case both Vs must be ASGSAFE.  When we set jt->asginfo.assignsym we are warranting
      // that the next assignment will be to the name, and that the reassigned value is available for inplacing.  In the V V N case,
      // this may be over two verbs
      if((UI)(((pline>>dyad)>>VJTFLGOK1X)&1)>(UI)PTISNOTASGNNAME(GETSTACK0PT))if(likely(PTISM(stackfs[2]))){L *s;   // inplaceable assignment to name; nothing in the stack to the right of what we are about to execute; well-behaved function (doesn't change locales)
       if(likely((AT(stack[0].a))&ASGNLOCAL)){
        // local assignment.  To avoid subroutine call overhead, make a quick check for primary symbol
        if(likely((SGNIF(pt0ecam,LOCSYMFLGX+ARLCLONEDX)|((I)NAV(nexty)->symx-1))>=0)){  // if we are using primary table and there is a symbol stored there...
         s=JT(jt,sympv)+(I)NAV(nexty)->symx;  // get address of symbol in primary table.  There may be no value; that's OK
        }else{s=jtprobeislocal(jt,nexty);}
       }else s=jtprobeisquiet(jt,nexty,UNLXAV0(locbuckets));  // global assignment, get slot address
       // Don't remember the assignand if it may change during execution, i. e. if the verb is unsafe.  For line 1 we have to look at BOTH verbs that come after the assignment
// obsolete        s=((FAV(fs)->flag&(FAV(stack[1].a)->flag|((~pmask)<<(VASGSAFEX-1))))&VASGSAFE)?s:0;
       s=((pline&(FAV(stk1a)->flag|((~pline)<<VASGSAFEX)))&VASGSAFE)?s:0;  // pline is 0-2; if not 1, ignore 2nd stkpos
       // It is OK to remember the address of the symbol being assigned, because anything that might conceivably create a new symbol (and thus trigger
       // a relocation of the symbol table) is marked as not ASGSAFE
       jt->asginfo.assignsym=s;  // remember the symbol being assigned.  It may have no value yet, but that's OK - save the lookup
       // to save time in the verbs (which execute more often than this parse), see if the assignment target is suitable for inplacing.  Set zombieval to point to the value if so
       // We require flags indicate not read-only, and usecount==1 (or 2 if NJA block)
       s=s?s:SYMVAL0; A zval=s->val; zval=zval?zval:AFLAG0; zval=AC(zval)==(((AFLAG(zval)&AFRO)-1)&(((AFLAG(zval)&AFNJA)>>1)+1))?zval:0; jt->asginfo.zombieval=zval;  // needs AFRO=1, AFNJA=2
      }
      jt->sf=fs;  // set new recursion point for $:
      I plflg=pline;  // save the flags
      pline&=3;  // remove inplaceable flag bit
      jt=(J)(intptr_t)((I)jt+(REPSGN(SGNIF(plflg,VJTFLGOK1X+(pline>>1)))&(pline|1)));   // set bit 0, and bit 1 if dyadic, if inplacing allowed by the verb
      // jt has been corrupted, now holding inplacing info
      // There is no need to set the token number in the result, since it must be a noun and will never be executed
      // Close up the stack.  For lines 0&2 we don't need two writes, so they are duplicates
      A arg2=stack[pline+1].a;   // 2nd arg, fs or right dyad  1 2 3 (2 3)
      stackfs[0]=stackfs[-1];    // overwrite the verb with the previous cell - 0->1  1->2  1->2
      A arg1=stack[(0x6>>pline)&3].a;   // 1st arg, monad or left dyad  2 3 1 (1 1)   0110  0 1 2 -> 2 3 1   1 11 111
      stack[pline]=stack[0];  // close up the stack  0->0(NOP)  0->1   0->2
      stack+=(pline>>1)+1;   // finish relocating stack   1 1 2 (1 2)
     // Get the branch-to address.  It comes from the appropriate valence of the appropriate stack element.  Stack element is 2 except for line 0; valence is monadic for lines 0 1 4
      AF actionfn=FAVV(fs)->valencefns[pline>>1];  // the routine we will execute.  We have to wait till after the register pressure or the routine address will be written to memory
      // When the args return from the verb, we will check to see if any were inplaceable and unused.  But there is a problem:
      // the arg may be freed by the verb (if it is inplaceable and gets replaced by a virtual reference).  In this case we can't
      // rely on *arg[12].  But if the value is inplaceable, the one thing we CAN count on is that it has a tpop slot.  So we will save
      // the address of the tpop slot IF the arg is inplaceable now.  Then after execution we will pick up again, knowing to quit if the tpop slot
      // has been zapped.  We keep pointers for a/w rather than 1/2 for branch-prediction purposes
      // This calculation should run to completion while the expected misprediction is being processed
      A *tpopw=AZAPLOC(arg2); tpopw=(A*)((I)tpopw&REPSGN(AC(arg2)&((AFLAG(arg2)&(AFVIRTUAL|AFUNINCORPABLE))-1))); tpopw=tpopw?tpopw:ZAPLOC0;  // point to pointer to arg2 (if it is inplace) - only if dyad
      A *tpopa=AZAPLOC(arg1); tpopa=(A*)((I)tpopa&REPSGN(AC(arg1)&((AFLAG(arg1)&(AFVIRTUAL|AFUNINCORPABLE))-1))); tpopa=tpopa?tpopa:ZAPLOC0; tpopw=(pline&2)?tpopw:tpopa; // monad: w fs  dyad: a w   if monad, change to w w  
// obsolete       A *tpopa=AZAPLOC(arg1); tpopa=(AC(arg1)&((AFLAG(arg1)&(AFVIRTUAL|AFUNINCORPABLE))-1))<0?tpopa:ZAPLOC0; tpopw=(pline&2)?tpopw:tpopa; // monad: w fs  dyad: a w   if monad, change to w w  
      y=(*actionfn)(jt,arg1,arg2,fs);
      // expect pipeline break
      jt=(J)(intptr_t)((I)jt&~JTFLAGMSK);
      // jt is OK again
RECURSIVERESULTSCHECK
#if MEMAUDIT&0x10
      auditmemchains();  // trap here while we still point to the action routine
#endif
      EPZ(y);  // fail parse if error
#if AUDITEXECRESULTS
      auditblock(jt,y,1,1);
#endif
#if MEMAUDIT&0x2
      if(AC(y)==0 || (AC(y)<0 && AC(y)!=ACINPLACE+ACUC1))SEGFAULT; 
      audittstack(jt);
#endif
      stackfs[1].a=y;  // save result 2 3 3 2 3; parsetype is unchanged, token# is immaterial
      // free up inputs that are no longer used.  These will be inputs that are still inplaceable and were not themselves returned by the execution.
      // We free them right here, and zap their tpop entry to avoid an extra free later.
      // We free using fanapop, which recurs only on recursive blocks, because that's what the tpop we are replacing does
      // We can free all DIRECT blocks, and PRISTINE also.  We mustn't free non-PRISTINE boxes because the contents are at large
      // and might be freed while in use elsewhere.
      // We mustn't free VIRTUAL blocks because they have to be zapped differently.  When we work that out, we will free them here too
      // NOTE that AZAPLOC may be invalid now, if the block was raised and then lowered for a period.  But if the arg is now abandoned,
      // and it was abandoned on input, and it wasn't returned, it must be safe to zap it using the zaploc BEFORE the call
      {
      if(arg1=*tpopw){  // if the arg has a place on the stack, look at it to see if the block is still around
       I c=(UI)AC(arg1)>>(arg1==y);  // get inplaceability; set off if the arg is the result
       if((c&(-(AT(arg1)&DIRECT)|SGNIF(AFLAG(arg1),AFPRISTINEX)))<0){   // inplaceable and not return value.  Sparse blocks are never inplaceable
// obsolete         if(!(AFLAG(arg1)&AFVIRTUAL)){  // for now, don't handle virtuals (which includes UNINCORPABLEs)
        *tpopw=0; fanapop(arg1,AFLAG(arg1));  // zap the top block; if recursive, fa the contents
// obsolete         }else SEGFAULT;  // scaf
       }
      }
      if(arg2=*tpopa){  // if arg1==arg2 this will never load a value requiring action
       I c=(UI)AC(arg2)>>(arg2==y);
// obsolete  c=arg1==arg2?0:c;
       if((c&(-(AT(arg2)&DIRECT)|SGNIF(AFLAG(arg2),AFPRISTINEX)))<0){  // inplaceable, not return value, not same as arg1, dyad.  Safe to check AC even if freed as arg1
// obsolete         if(!(AFLAG(arg2)&AFVIRTUAL)){  // for now, don't handle virtuals
// obsolete if(arg1==arg2)SEGFAULT;
        *tpopa=0; fanapop(arg2,AFLAG(arg2));
// obsolete         }else SEGFAULT;  // scaf
       }
      }
#if MEMAUDIT&0x2
      audittstack(jt);
#endif
      }
      // Most of the executed fragements are executed right here.  In two cases we can be sure that the stack does not need to be rescanned:
      // 1. pline=2, token 0 is AVN: we have just put a noun in the first position, and if that produced an executable it would have been executed earlier.
      // 2. pline=0 or 2, token 0 is EDGE but not LPAR: similarly can't execute with noun now in slot 1 (if LPAR and line 0/2, the only possible exec is () )
      // we save a pass through the matcher in those cases.  The 8 cycles are worth saving, but more than that it makes the branch prediction tighter
      I iscavn=PTISCAVN(GETSTACK0PT);  // 0x400000 if CAVN (which implies AVN here)
      if(((iscavn>>=(PTISCAVNX-2))&pline)|((GETSTACK0PT>>PTNOTLPARX)&~pline&1)){pt0ecam|=iscavn<<(VERBX-2); break;}  // cavn or ~( & ~line1; set 'stack two if AVN' flag if stack0 was AVN
     }else{
      // Lines 3-4, conj/adv execution.  We must get the parsing type of the result, but we don't need to worry about inplacing or recursion
      AF actionfn=FAVV(fs)->valencefns[pline-3];  // the routine we will execute.  It's going to take longer to read this than we can fill before the branch is mispredicted, usually
      A arg1=stk1a;   // 1st arg, monad or left dyad
      A arg2=stack[pline-1].a;   // 2nd arg, fs or right dyad
      UI4 restok=stack[1].t;  // save token # to use for result
      stack[pline-2]=stack[0]; // close up the stack
      stack=stack+pline-2;  // advance stackpointer to position before result 1 2
      A y=(*actionfn)(jt,arg1,arg2,fs);
RECURSIVERESULTSCHECK
#if MEMAUDIT&0x10
      auditmemchains();  // trap here while we still point to the action routine
#endif
      EPZ(y);  // fail parse if error
#if AUDITEXECRESULTS
      auditblock(jt,y,1,1);
#endif
#if MEMAUDIT&0x2
      if(AC(y)==0 || (AC(y)<0 && AC(y)!=ACINPLACE+ACUC1))SEGFAULT; 
      audittstack(jt);
#endif
      PTFROMTYPE(stack[1].pt,AT(y)) stack[1].t=restok; stack[1].a=y;   // save result, move token#, recalc parsetype
     }
    }else{
     // Here for lines 5-8 (fork/hook/assign/parens), which branch to a canned routine
     // It will run its function, and return the new stackpointer to use, with the stack all filled in.  If there is an error, the returned stackpointer will be 0.
     // We avoid the indirect branch, which is very expensive
     if(pline>6){  // assign/paren
      if(pline==7){
       SETSTACK0PT(stack[2].pt);  // Bottom of stack will be modified, so refresh the type for it
       stack=jtis(jt,stack); // assignment
       EPZ(stack)  // fail if error
      }else{  // paren, for which pline may be anything
       if(likely(PTISCAVN(stack[1].pt)&&PTISRPAR(stack[2].pt))){  // must be [1]=CAVN and [2]=RPAR
        SETSTACK0PT(stack[1].pt); stack[2]=stack[1]; stack[2].t=stack[0].t;  //  Install result over ).  Use value/type from expr, token # from (   Bottom of stack was modified, so refresh the type for it
        stack+=2;  // advance stack pointer to result
       }else{jsignal(EVSYNTAX); FP}  // error if contents of ( not valid
      }
     }else{
      if(pline==5)stack=jtpfork(jt,stack); else stack=jtphook(jt,stack);  // bottom of stack unchanged
      EPZ(stack)  // fail if error
     }
#if MEMAUDIT&0x10
     auditmemchains();  // trap here while we still have the parseline
#endif
#if AUDITEXECRESULTS
     if(pline<=6)auditblock(jt,stack[1].a,1,1);  // () and asgn have already been audited
#endif
#if MEMAUDIT&0x2
      if(m>=0 && (AC(stack[0].a)==0 || (AC(stack[0].a)<0 && AC(stack[0].a)!=ACINPLACE+ACUC1)))SEGFAULT; 
      audittstack(jt);
#endif
    }  // end of classifying fragment
   } // end of loop executing fragments
  }  // break with stack==0 on error; main exit is when queue is empty (m<0)
 exitparse:
   // Prepare the result

  if(likely(stack!=0)){  // if no error yet...
   // before we exited, we backed the stack to before the initial mark entry.  At this point stack[0] is invalid,
   // stack[1] is the initial mark, stack[2] is the result, and stack[3] had better be the first ending mark
   z=stack[2].a;   // stack[0..1] are the mark; this is the sentence result, if there is no error
   if(unlikely(!(PTOKEND(stack[2],stack[3])))){jt->parserstackframe.parsercurrtok = 0; jsignal(EVSYNTAX); z=0;}  // OK if 0 or 1 words left (0 should not occur)
  }else{
failparse:  // If there was an error during execution or name-stacking, exit with failure.  Error has already been signaled.  Remove zombiesym
   CLEARZOMBIE z=0; stack=PSTK2NOTFINALASGN;  // set stack to something that IS NOT a final assignment
  }
#if MEMAUDIT&0x2
  audittstack(jt);
#endif

  // Now that the sentence has completed, take care of some cleanup.  Names that were reassigned after
  // their value was moved onto the stack had the decrementing of the use count deferred: we decrement
  // them now.  There may be references to these names in the result (if we are returning a verb/adv/conj),
  // so we don't free the names quite yet: we put them on the tpush stack to be freed after we know
  // we are through with the result.  If we are returning a noun, free them right away unless they happen to be the very noun we are returning
  // We apply the final free only when the NVR count goes to 0, to make sure we hold off till the last stacked reference has been seen off
  v=AAV1(jt->nvra)+jt->parserstackframe.nvrotop;  // point to our region of the nvr area
  UI zcompval = !z||AT(z)&NOUN?0:-1;  // if z is 0, or a noun, immediately free only values !=z.  Otherwise don't free anything
  DQ(jt->parserstackframe.nvrtop-jt->parserstackframe.nvrotop, A vv = *v;I am;
   // if the NVR count is 1 before we decrement, we have hit the last stacked use & we free the block.
   // if we are performing (or finally deferring) the FINAL free, the value must be a complete zombie and cannot be active anywhere; otherwise we must clear it.  We clear it always
   if(likely((AMNVRDECR(vv,am))<2*AMNVRCT)){if(am&AMFREED){AMNVRAND(vv,~AMFREED) if(((UI)z^(UI)vv)>zcompval){fanano0(vv);}else{tpushna(vv);}}}
   ++v;);   // schedule deferred frees.
    // na so that we don't audit, since audit will relook at this NVR stack

  // Still can't return till frame-stack popped
  jt->parserstackframe = oframe;
#if MEMAUDIT&0x2
  audittstack(jt);
#endif
  jt->sf=savfs;  // pop $: stack
#if MEMAUDIT&0x20
     auditmemchains();  // trap here while we still have the parseline
#endif

  // NOW it is OK to return.  Insert the final-assignment bit (sign of stack[2]) into the return
  R (A)((I)z+SGNTO0US(stack[2].t));  // this is the return point from normal parsing

 }else{A y;  // m<2.  Happens fairly often, and full parse can be omitted
  if(likely(nwds==1)){  // exit fast if empty input.  Happens only during load, but we can't deal with it
   // Only 1 word in the queue.  No need to parse - just evaluate & return.  We do it here to avoid parsing
   // overhead, because it happens enough to notice.
   // No ASSERT - must get to the end to pop stack
   jt->parserstackframe.parsercurrtok=0;  // error token if error found
   I at=AT(y = queue[0]);  // fetch the word
   if((at&NAME)!=0) {L *s;
    if(likely((s=syrd(y,jt->locsyms))!=0)) {     // Resolve the name.
      A sv;  // pointer to value block for the name
      RZ(sv = s->val);  // symbol table entry, but no value.  Must be in an explicit definition, so there is no need to raise an error
      if(likely(((AT(sv)|at)&(NOUN|NAMEBYVALUE))!=0)){   // if noun or special name, use value
       if(unlikely(at&NAMEABANDON))namecoco(jtinplace, y, at, LXAV0(jt->locsyms), s, sv, JT(jt,sympv));  // if name::, go delete the name, leaving the value to be deleted later
       y=sv;
      } else y = namerefacv(y, s);   // Replace other acv with reference.  Could fail.
    } else {
      // undefined name.
      if(at&NAMEBYVALUE){jsignal(EVVALUE); y=0;}  // Error if the unresolved name is x y etc.  Don't ASSERT since we must pop stack
      else y = namerefacv(y, s);    // this will create a ref to undefined name as verb [: .  Could set y to 0 if error
    }
   }
   if(likely(y!=0))if(unlikely(!(AT(y)&CAVN))){jsignal(EVSYNTAX); y=0;}  // if not CAVN result, error
  }else y=mark;  // empty input - return with 'mark' as the value, which means nothing to parse.  This result must not be passed into a sentence
  jt->parserstackframe = oframe;
  R y;
 }

}

