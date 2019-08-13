/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Parsing; see APL Dictionary, pp. 12-13 & 38.                            */

#include "j.h"
#include "p.h"
#include <stdint.h>


#define PARSERSTKALLO (490*sizeof(PSTK))  // number of stack entries to allocate, when we allocate, in bytes

/* NVR - named value reference                                          */
/* a value referenced in the parser which is the value of a name        */
/* (that is, in some symbol table).                                     */
/*                                                                      */
/* jt->nvra      NVR stack a: stack of A values.  LSB is a flag         */
/* jt->nvrav     AAV(jt->nvra)                                          */
// jt->nvran     AN(jt->nvra)
/* jt->nvrtop    index of top of stack.  stack grows up                 */
/*                                                                      */
/* Each call of the parser records the current NVR stack top (nvrtop),  */
/* and pop stuff off the stack back to that top on exit       */
/*                                                                      */
// The nvr stack contains pointers to values, added as names are moved
// from the queue to the stack.  Local values are not pushed.

B jtparseinit(J jt){A x;
 GAT0(x,INT,20,1); ras(x); jt->nvra=x; jt->nvrav=AAV(x); jt->nvran=(UI4)AN(x);  // Initial stack.  Size is doubled as needed
 R 1;
}

// w is a block that looks ripe for in-place assignment.  We just have to make sure that it is not in use somewhere up the stack.
// It isn't, if (1) it isn't on the stack at all; (2) if it was put on the stack by the currently-executing sentence.  We call this
// routine only when we are checking inplacing for final assignments, for which the parser stack is guaranteed to be empty; so any
// use of the name that was called for by this sentence must be finished
I jtnotonupperstack(J jt, A w) {
  // w is known nonzero
  // see if name was stacked (for the first time) in this very sentence
  A *v=jt->parserstackframe.nvrotop+jt->nvrav;  // point to current-sentence region of the nvr area
  DQ(jt->parserstackframe.nvrtop-jt->parserstackframe.nvrotop, if(*v==w)R 1; ++v;);   // if name stacked in this sentence, that's OK
  // see if name was not stacked at all
  R !(AFLAG(w)&AFNVR);   // return OK if name not stacked (rare, because if it wasn't stacked in the current sentence why would we think we can inplace it?)
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
// static US ptcol[4][10] = {
//     PN     PA     PC     PV     PM     PNM    PL     PR     PS     PSN
// { 0x2BE, 0x23E, 0x200, 0x23E, 0x27F, 0x280, 0x37F, 0x200, 0x27F, 0x27F},
// { 0x37C, 0x340, 0x340, 0x37B, 0x200, 0x200, 0x200, 0x200, 0x280, 0x280},
// { 0x2C1, 0x2C8, 0x2D0, 0x2E6, 0x200, 0x200, 0x200, 0x300, 0x200, 0x200},
// { 0x3DF, 0x3C9, 0x3C9, 0x3F9, 0x3C9, 0x3C9, 0x3C9, 0x3C9, 0x3C9, 0x3C9},
// };
// Remove bits 8-9
// Distinguish PSN from PS by not having PSN in stack[3] support line 0 (OK since it must be preceded by NAME and thus will run line 7)
// Put something distictive into LPAR that can be used to create line 8
static UI4 ptcol[] = {
0xBE7CC1DF,  // PN
0x7F8000C9,  // PS
0x7F0000C9,  // PM
0x800000C9,  // PNM
0,    // PX
0,    // PX
0x7F000001,  // PL
0x3E7BE6F9,  // PV
0x3E40C8C9,  // PA
0x0040D0C9,  // PC
0x000000C9  // PR
// 0x7F8000C8  // PSN
};

// tests for pt types
#define PTMARK 0x7F0000C9
#define PTASGNNAME 0x7F8000C8
#define PTISCAVN(s) ((s).pt&0x4000)
#define PTISM(s)  ((s).pt==PTMARK)
#define PTISASGN(s)  ((s).pt&0x800000)
#define PTISASGNNAME(s)  (!((s).pt&0x1))
#define PTISRPAR(s)  ((s).pt<0x100)
// converting type field to pt, store in z
#define PTFROMTYPE(z,t) {I pt=CTTZ(t); pt-=LASTNOUNX; pt=pt<0?0:pt; z=ptcol[pt];}
#define PTFROMTYPEASGN(z,t) {I pt=CTTZ(t); pt-=LASTNOUNX; pt=pt<0?0:pt; pt=ptcol[pt]; pt=((t)&CONW)?PTASGNNAME:pt; z=(UI4)pt;}  // clear flag bit if ASGN to name

static PSTK* jtpfork(J jt,PSTK *stack){
 A y=folk(stack[1].a,stack[2].a,stack[3].a);  // create the fork
 RZ(y);  // if error, return 0 stackpointer
 stack[3].t = stack[1].t; stack[3].a = y;  // take err tok from f; save result; no need to set parsertype, since it didn't change
 stack[2]=stack[0]; R stack+2;  // close up stack & return
}

static PSTK* jtphook(J jt,PSTK *stack){
 A y=hook(stack[1].a,stack[2].a);  // create the hook
 RZ(y);  // if error, return 0 stackpointer
 PTFROMTYPE(stack[2].pt,AT(y)) stack[2].t = stack[1].t; stack[2].a = y;  // take err tok from f; save result
 stack[1]=stack[0]; R stack+1;  // close up stack & return
}

static PSTK* jtpparen(J jt,PSTK *stack){
 ASSERT(PTISCAVN(stack[1])&&PTISRPAR(stack[2]),EVSYNTAX);  // if error, signal so with 0 stack.  Look only at pt since MARK doesn't have an a
 stack[2].pt=stack[1].pt; stack[2].t=stack[0].t; stack[2].a = stack[1].a;  //  Install result over ).  Use value from expr, token # from (
 R stack+2;  // advance stack pointer to result
}

static F2(jtisf){RZ(symbis(onm(a),CALL1(jt->pre,w,0L),jt->symb)); R num[0];} 

static PSTK* jtis(J jt,PSTK *stack){B ger=0;C *s;
  A asgblk=stack[1].a; I asgt=AT(asgblk); A v=stack[2].a, n=stack[0].a;  // value and name
 jt->asgn = stack[0].t==1;  // if the word number of the lhs is 1, it's either (noun)=: or name=: or 'value'=: at the beginning of the line; so indicate
 if(jt->assignsym){symbis(n,v,(A)asgt);}   // Assign to the known name.  Pass in the type of the ASGN
 else {
  // Point to the block for the assignment; fetch the assignment pseudochar (=. or =:); choose the starting symbol table
  // depending on which type of assignment (but if there is no local symbol table, always use the global)
  A symtab=jt->locsyms; if(!(asgt&ASGNLOCAL)||AN(jt->locsyms)==1)symtab=jt->global;
  if(AT(n)&BOXMULTIASSIGN){
   // string assignment, where the NAME blocks have already been computed.  Use them.  The fast case is where we are assigning a boxed list
   if(AN(n)==1)n=AAV(n)[0];  // if there is only 1 name, treat this like simple assignment to first box, fall through
   else{
    // True multiple assignment
    ASSERT(!AR(v)||AN(n)==AS(v)[0],EVLENGTH);   // v is atom, or length matches n
    if(((AR(v)^1)+(~AT(v)&BOX))==0){A *nv=AAV(n), *vv=AAV(v); DO(AN(n), symbis(nv[i],vv[i],symtab);)}  // v is boxed list
    else {A *nv=AAV(n); DO(AN(n), symbis(nv[i],ope(AR(v)?from(sc(i),v):v),symtab);)}  // repeat atomic v for each name, otherwise select item.  Open in either case
    goto retstack;
   }
  }
  if(LIT&AT(n)&&1>=AR(n)){
   // lhs is ASCII characters, atom or list.  Convert it to words
   //ASSERT(1>=AR(n),EVRANK); must be true
   s=CAV(n); ger=CGRAVE==*s;   // s->1st character; remember if it is `
   RZ(n=words(ger?str(AN(n)-1,1+s):n));  // convert to words (discarding leading ` if present)
   if(1==AN(n)){
    // Only one name in the list.  If one-name AR assignment, leave as a list so we go through the AR-assignment path below
    if(!ger){RZ(n=head(n));}   // One-name normal assignment: make it a scalar, so we go through the name-assignment path & avoid unboxing
   }
  }
  ASSERT(AN(n)||!IC(v),EVILNAME);  // error if name empty or multiple assignment to no names, if there is something to be assigned
  // if simple assignment to a name (normal case), do it
  if(NAME&AT(n)){
#if FORCEVIRTUALINPUTS
   // When forcing everything virtual, there is a problem with jtcasev, which converts its sentence to an inplace special.
   // The problem is that when the result is set to virtual, its backer does not appear in the NVR stack, and when the reassignment is
   // made the virtual block is dangling.  The workaround is to replace the block on the stack with the final value that was assigned:
   // not allowed in general because of (verb1 x verb2) name =: virtual - if verb2 assigns the name, the value going into verb1 will be freed before use
   stack[2].a=
#endif
   symbis(n,v,symtab);
  }
  // otherwise, if it's an assignment to an atomic computed name, convert the string to a name and do the single assignment
  else if(!AR(n))symbis(onm(n),v,symtab);
  // otherwise it's multiple assignment (could have just 1 name to assign, if it is AR assignment).
  // Verify rank 1.  For each lhs-rhs pair, do the assignment (in jtisf).
  // if it is AR assignment, apply jtfxx to each assignand, to convert AR to internal form
  // if not AR assignment, just open each box of rhs and assign
  else {ASSERT(1==AR(n),EVRANK); ASSERT(AT(v)&NOUN,EVDOMAIN); jt->symb=symtab; jt->pre=ger?jtfxx:jtope; rank2ex(n,v,0L,0,AR(v)-1<0?0:AR(v)-1,0,AR(v)-1<0?0:AR(v)-1,jtisf);}
 }
retstack:  // return, but 0 if error
 stack+=2; stack=jt->jerr?0:stack; R stack;  // the result is the same value that was assigned
}

static PSTK * (*(lines58[]))() = {jtpfork,jtphook,jtis,jtpparen};  // handlers for parse lines 5-8

#if AUDITEXECRESULTS
// go through a block to make sure that the descendants of a recursive block are all recursive, and that no descendant is virtual.
// Initial call has nonrecurok and virtok both set
void auditblock(A w, I nonrecurok, I virtok) {
 if(!w)R;
 I nonrecur = (AT(w)&RECURSIBLE) && ((AT(w)^AFLAG(w))&RECURSIBLE);  // recursible type, but not marked recursive
 I virt = (AFLAG(w)&AFVIRTUAL)!=0;  // any virtual
 if(virt)if(AFLAG(ABACK(w))&AFVIRTUAL)SEGFAULT  // make sure backer is valid and not virtual
 if(nonrecur&&!nonrecurok)SEGFAULT
 if(virt&&!virtok)SEGFAULT
 if(AT(w)==0xdeadbeefdeadbeef)SEGFAULT
 switch(CTTZ(AT(w))){
  case RATX:  
   {A*v=AAV(w); DO(2*AN(w), if(v[i])if(!(((AT(v[i])&NOUN)==INT) && !(AFLAG(v[i])&AFVIRTUAL)))SEGFAULT);} break;
  case XNUMX:
   {A*v=AAV(w); DO(AN(w), if(v[i])if(!(((AT(v[i])&NOUN)==INT) && !(AFLAG(v[i])&AFVIRTUAL)))SEGFAULT);} break;
  case BOXX:
   if(!(AFLAG(w)&AFNJA)){A*wv=AAV(w); 
   {DO(AN(w), auditblock(wv[i],nonrecur,0););}
   }
   break;
  case VERBX: case ADVX:  case CONJX: 
   {V*v=VAV(w); auditblock(v->fgh[0],nonrecur,0); auditblock(v->fgh[1],nonrecur,0); auditblock(v->fgh[2],nonrecur,0);} break;
  case SB01X: case SINTX: case SFLX: case SCMPXX: case SLITX: case SBOXX:
   {P*v=PAV(w);  A x;
   x = SPA(v,a); if(!(AT(x)&DIRECT))SEGFAULT x = SPA(v,e); if(!(AT(x)&DIRECT))SEGFAULT x = SPA(v,i); if(!(AT(x)&DIRECT))SEGFAULT x = SPA(v,x); if(!(AT(x)&DIRECT))SEGFAULT
   auditblock(SPA(v,a),nonrecur,0); auditblock(SPA(v,e),nonrecur,0); auditblock(SPA(v,i),nonrecur,0); auditblock(SPA(v,x),nonrecur,0);} break;
  case B01X: case INTX: case FLX: case CMPXX: case LITX: case C2TX: case C4TX: case SBTX: case NAMEX: case SYMBX: case CONWX: if(NOUN & (AT(w) ^ (AT(w) & -AT(w))))SEGFAULT break;
  case ASGNX: break;
  default: break; SEGFAULT
 }
}
#endif




// Run parser, creating a new debug frame.  Explicit defs, which make other tests first, then go through jtparsea
F1(jtparse){A z;
 RZ(w);
 A *queue=AAV(w); I m=AN(w);   // addr and length of sentence
 RZ(deba(DCPARSE,queue,(A)m,0L));  // We don't need a new stack frame if there is one already and debug is off
 z=parsea(queue,m);
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
 if(AT(buf)&NOUN && !(ipok && ACIPISOK(buf)) && !(AT(buf)&SPARSE) && !(AFLAG(buf)&(AFNJA))) {A oldbuf=buf;
  buf=virtual(buf,0,AR(buf)); if(!buf && jt->jerr!=EVATTN && jt->jerr!=EVBREAK)SEGFAULT  // replace non-inplaceable w with virtual block; shouldn't fail except for break testing
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

#define FP goto failparse;   // indicate parse failure and exit
#define EP goto exitparse;   // exit parser, preserving current status
#define EPZ(x) if(!(x)){FP}   // exit parser if x==0

#if 0  // keep for commentary
// In-place operations
//
// An in-place operation requires an inplaceable argument, which is marked as LSB of AC=0,
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
// any other block.  Whether to traverse to do this is an open issue, deferred for the time being because only
// direct blocks are marked inplaceable now.
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
// flag in the verb.  For the nonce, we use the in-placeable flag as a proxy for this.

// To accommodate the issues, we take a two-step approach:
// 1. Any assignment to a name is resolved to an address when the copula is encountered and there
//  is only one execution on the stack.  This resolution will always succeed for a local assignment to a name.
//  For a global assignment to a locative, it may fail, or may resolve to an address that is different from
//  the correct address after the execution.  The address of the L block for the symbol to be assigned is stored in jt->assignsym.
// 2. For local assignment that qualifies for in-place operation (see below), the address of the A block
//  for the value of the reassignable argument is stored in jt->zombieval.
//
// [As a time-saving maneuver, we store jt->assignsym even if the name is not in-placeable because of its type or usecount.
// We can use jt->assignsym to avoid re-looking-up the name.]
//
// This gives two levels of inplace operations:
// 1. loose - if jt->assignsym is set, the (necessarily inplaceable) verb may choose to perform an in-place
// operation.  It will check usecounts and addresses to decide whether to do this, and it bears the responsibility
// of worrying about names on the stack.
// 2. strict - if jt->zombieval is set, the caller warrants that it is safe to overwrite jt->zombieval and
// assign it to the result.  This will be set only for local assignments to a simplename.

// Note that if jt->zombieval is set, any argument that is equal to jt->zombieval will be
// ipso facto inplaceable, and moreover the usecount in such
// an argument WILL NOT be set because it came from evaluating a name.  So, a verb supporting inplace
// operation must check for (jtinplace&1 && ACIPISOK(w)||jt->zombiesym==w) for example
// if it wants to inplace the w argument.

// Prep for in-place assignments.
// If the top-of-stack is ASGN, we will peek into the queue to get the symbol-table entry for the
// name about to be assigned.  jt->zombieval will point to the A block for the assignee
// provided:
//  usecount==1
//  DIRECT type
//  stack will be empty after the current execution
//  The verb supports in-place ops
//  local assignment
//  not assigning to a locative
// It is 0 otherwise.  When zombieval is set, the data block for
// the assignee may be used as a free block.  zombieval must be cleared:
//  when the assignment is performed
//  if an error is encountered (preventing the assignment)
// Set zombieval if current assignment can be in-place.  w is the end of the execution that will produce the result to assign
// There must be a local symbol table if we have the ASGN indicating local/simple
// Set assignsym for any final execution that assigns to a name.  queue[m-1] is the name to be assigned.  We use this only if the verb is known to be locale-safe, so it's OK
// to precalculate the assignment target.  The probe for a locale name will never fail, because we will have preallocated the name.
//  A verb, such as a derived hook, might support inplaceability but one of its components could be locale-unsafe; example (, unsafeverb h)
// We set zombieval whenever the local value is to be reassigned, regardless of usecount.  Users of zombieval must check AC==1 before using it.  zombieval
// is used only as an alternative way of detecting inplaceable usecount, not as a way of reusing a non-argument block during final assignment.
#define IPSETZOMB(w,v) {L *s; if(PTISASGNNAME(stack[0])&&PTISM(stack[(w)+1])&&(FAV(stack[v].a)->flag&VASGSAFE) \
   &&(s=AT(stack[0].a)&ASGNLOCAL?probelocal(queue[m-1]):probeisquiet(queue[m-1]))){jt->assignsym=s; if(s->val&&AT(stack[0].a)&ASGNLOCAL)jt->zombieval=s->val;}}

// In-place operands
// An operand is in-placeable if:
//  usecount==1 with inplaceable bit
//  DIRECT type (implied for now - we don't inplace otherwise)
//  the verb supports in-place ops
// We set the jt bits to indicate inplaceability.  Since the parser never reuses an argument, all bits will
// be set if the callee can handle inplaceing.
// NOTE that in name =: x i} name, the zombieval will be set but the name operand will NOT have an inplaceable usecount.  The action routine
// should check the operand addresses when zombieval/assignsym is set.  Inplaceable arguments are ignored by functions that do
// not handle inplace operations.

// w here is the index of the last word of the execution. 
// aa  is the index of the left argument.  v is the verb.  zomb is 1 if it is OK to set assignsym/zombieval
#define DFSIP1(v,w,zomb) if(FAV(stack[v].a)->flag&VJTFLGOK1){if(zomb)IPSETZOMB(w,v) y=jtdfs1((J)(intptr_t)((I)jt|JTINPLACEW),stack[w].a,stack[v].a);}else{y=dfs1(stack[w].a,stack[v].a);}
#define DFSIP2(aa,v,w) if(FAV(stack[v].a)->flag&VJTFLGOK2){IPSETZOMB(w,v) y=jtdfs2((J)(intptr_t)((I)jt|(JTINPLACEW+JTINPLACEA)),stack[aa].a,stack[w].a,stack[v].a);}else{y=dfs2(stack[aa].a,stack[w].a,stack[v].a);}
// Storing the result
// We store the result into the stack and move the token-number for it.  
// we pass in the stack index of the verb, and infer the operands from that
// z=result stack index, a/w=stack index of argument(s), t=source for token number
#define STOY(z,tok) {stack[z].t=stack[tok].t; EPZ(stack[z].a=y)}

// Closing up the stack
#define SM(to,from) stack[to]=stack[from]
#endif
A* jtextnvr(J jt){ASSERT(jt->parserstackframe.nvrtop<32000,EVLIMIT); RZ(jt->nvra = ext(1, jt->nvra)); jt->nvran=(UI4)AN(jt->nvra); jt->nvrav = AAV(jt->nvra); R jt->nvrav;}

#define BACKMARKS 3   // amount of space to leave for marks at the end.  Because we stack 3 words before we start to parse, we will
 // never see 4 marks on the stack - the most we can have is 1 value + 3 marks.
#define FRONTMARKS 1  // amount of space to leave for front-of-string mark
// Parse a J sentence.  Input is the queue of tokens
A jtparsea(J jt, A *queue, I m){PSTK *stack;A z,*v;I es;

 // This routine has two global responsibilities in addition to parsing.  jt->asgn must be set to 1
 // if the last thing is an assignment, and since this flag is cleared during execution (by ". and
 // others), it must be set at the time the assignment is executed.  We catch it in the action routine,
 // noting when the assignment is to (possibly inherited) word 1 (word 0 is the mark).
 //
 // jt->parsercurrtok must be set before executing anything that might fail; it holds the original
 // word number+1 of the token that failed.  jt->parsercurrtok is set before dispatching an action routine,
 // so that the information is available for formatting an error display
  // Save info for error typeout.  We save sentence info once, and token info for every executed fragment
 PFRAME oframe=jt->parserstackframe;   // save all the stack status
 jt->parserstackframe.parserqueue=queue; jt->parserstackframe.parserqueuelen=(US)m;  // addr & length of words being parsed
 jt->asgn = 0;
 if(m>1) {  // normal case where there is a fragment to parse
  // allocate the stack.  No need to initialize it, except for the marks at the end, because we
  // never look at a stack location until we have moved from the queue to that position.
  // Each word gets two stack locations: first is the word itself, second the original word number+1
  // to use if there is an error on the word
  // If there is a stack inherited from the previous parse, and it is big enough to hold our queue, just use that.
  // The stack grows down
  if((uintptr_t)jt->parserstackframe.parserstkend1-(uintptr_t)jt->parserstackframe.parserstkbgn < (m+BACKMARKS+FRONTMARKS)*sizeof(PSTK)){A y;   // we hope this gets predicted right - should branch
   ASSERT(m<65000,EVLIMIT);  // To keep the stack frame small, we limit the length of a sentence
   I allo = MAX((m+BACKMARKS+FRONTMARKS)*sizeof(PSTK),PARSERSTKALLO); // number of bytes to allocate.  Allow 4 marks: 1 at beginning, 3 at end
   GATV0(y,B01,allo,1);
   jt->parserstackframe.parserstkbgn=(PSTK*)AV(y);   // save start of data area
   // We are taking advantage of the fact the NORMAH is 7, and thus a rank-1 array is aligned on a boundary of its size
   jt->parserstackframe.parserstkend1=(PSTK*)((uintptr_t)jt->parserstackframe.parserstkbgn+allo);  // point to the end+1 of the allocation
   // We could worry about hysteresis to avoid reallocation of every call
  }
  stack=jt->parserstackframe.parserstkend1-BACKMARKS;   // start at the end, with 3 marks

  ++jt->parsercalls;  // now we are committed to full parse.  Push stacks.
  I nvrotop=jt->parserstackframe.nvrotop=jt->parserstackframe.nvrtop;  // we have to keep the next-to-top nvr value visible for a subroutine.  It remains as we advance nvrtop.  Save in a local too for comp ease

  // We don't actually put a mark in the queue at the beginning.  When m goes down to 0, we move in a mark.

  // As names are dereferenced, they are added to the nvr queue.  To save time in the loop, we now
  // make sure there is enough room in the nvr queue to handle all the names we will encounter in
  // this sentence.  For simplicity's sake, we just assume the worst, that every word is a name, and
  // make sure there is that much space.  BUT if there were an enormous tacit sentence, that would be
  // very inefficient.  So, if the sentence is too long, we go through and count the number of names,
  // rather than using a poor upper bound.
  {UI4 maxnvrlen;
   if (m < 128)maxnvrlen = (UI4)m;   // if short enough, assume they're all names
   else {
    maxnvrlen = 0;
    DQ(m, maxnvrlen+=(AT(queue[i])>>NAMEX)&1;)
   }
   // extend the nvr stack, doubling its size each time, till it can hold our names.  Don't let it get too big.  This code duplicated in 4!:55
   while((jt->parserstackframe.nvrtop+maxnvrlen) > jt->nvran)RZ(extnvr());
  }

  // We have the initial stack pointer.  Grow the stack down from there
  stack[0].pt = stack[1].pt = stack[2].pt = PTMARK;  // install initial ending marks.  word numbers and value pointers are unused

  // Set number of extra words to pull from the queue.  We always need 2 words after the first before a match is possible.
  es = 2;

  // DO NOT RETURN from inside the parser loop.  Stacks must be processed.

  while(1){  // till no more matches possible...
    UI4 stack0pt;  // will hold the EDGE+AVN value, which doesn't change much and is stored late

    // no executable fragment, pull from the queue.  If we pull ')', there is no way we can execute
    // anything till 2 more words have been pulled, so we pull them here to avoid parse overhead.
    // Likewise, if we pull a CONJ, we can safely pull 1 more here.  And first time through, we should
    // pull 2 words following the first one.
    // es holds the number of extra pulls required.  It may go negative, which means no extra pulls.
    // the only time it is positive here is the initial filling of the queue

   do{
    stack--;  // back up to new stack frame, where we will store the new word

    if(--m>=0) {A y;     // if there is another valid token...
     // Move in the new word and check its type.  If it is a name that is not being assigned, resolve its
     // value.  m has the index of the word we just moved
     I at=AT(y = queue[m]);   // fetch the next word from queue; pop the queue; extract the type, save as at
     if(at&NAME) {
      if(!PTISASGN(stack[1])) {L *s;  // Replace a name with its value, unless to left of ASGN.  This test is 'not assignment'

       // Name, not being assigned
       // Resolve the name.  If the name is x. m. u. etc, always resolve the name to its current value;
       // otherwise resolve nouns to values, and others to 'name~' references
       // To save some overhead, we inline this and do the analysis in a different order here
       // The important performance case is local names with bucket info.  Pull that out & do it without the call overhead
       // This code is copied from s.c
       if(NAV(y)->bucket){I bx;
        if(0 <= (bx = ~NAV(y)->bucketx)){   // negative bucketx (now positive); skip that many items, and then you're at the right place.  This is the path for almost all local symbols
         s = LXAV0(jt->locsyms)[NAV(y)->bucket]+jt->sympv;  // fetch hashchain headptr, point to L for first symbol
         while(bx--){s = s->next+jt->sympv;}  // skip the prescribed number
         if(s->val==0)goto rdglob;  // if value has not been assigned, ignore it
        }else{
         // positive bucketx (now negative); that means skip that many items and then do name search.  This is set for words that were recognized as names but were not detected as assigned-to in the definition.  This is the path for global symbols
         // If no new names have been assigned since the table was created, we can skip this search, since it must fail (this is the path for words in z eg)
         if(!(AR(jt->locsyms)&LNAMEADDED))goto rdglob;
         // from here on it is rare to find a name - usually they're globals defined elsewhere
         LX lx = LXAV0(jt->locsyms)[NAV(y)->bucket];  // index of first block if any
         I m=NAV(y)->m; C* nm=NAV(y)->s;  // length/addr of name from name block
         while(0>++bx){lx = jt->sympv[lx].next;}
         // Now lx is the index of the first name that might match.  Do the compares
         while(1) {
          if(lx==0)goto rdglob;  // If we run off chain, go read from globals
          s = lx+jt->sympv;  // symbol entry
          IFCMPNAME(NAV(s->name),nm,m,{if(s->val==0)goto rdglob; break;})  // if match, we're done looking; could be not found, if no value
          lx = s->next;
         }
         // Here there was a value in the local symbol table
        }
       }else{
        // No bucket info.  Usually this is a locative, but it could be an explicit modifier, console level, or ".
rdglob: ;
        jt->parserstackframe.parsercurrtok = (I4)(m+1);  // syrd can fail, so we have to set the error-word number (before it was decremented) before calling
        s=syrdnobuckets(y);  // do full symbol lookup, knowing that we have checked for buckets already
         // In case the name is assigned during this sentence (including subroutines), remember the data block that the name created
         // NOTE: the nvr stack may have been relocated by action routines, so we must refer to the global value of the base pointer
         // Stack a named value only once.  This is needed only for names whose VALUE is put onto the stack (i. e. a noun); if we stack a REFERENCE
         // (via namerefacv), no special protection is needed.  And, it is not needed for local names, because they are inaccessible to deletion in called
         // functions (that is, the user should not use u. to delete a local name).  If a local name is deleted, we always defer the deletion till the end of the sentence, easier than checking
        if(s&&s->val&&AT(s->val)&NOUN&&!(AFLAG(s->val)&AFNVR)){ 
         jt->nvrav[jt->parserstackframe.nvrtop++] = s->val;   // record the place where the value was protected, so we can free it when this sentence complaetes
         AFLAG(s->val) |= AFNVR|AFNVRUNFREED;  // mark the value as protected and not yet deferred-freed
        }
       }
       // end of looking at local/global symbol tables
       // s has the symbol for the name
       if(s){   // if symbol was defined...
        A sv;  // pointer to value block for the name
        
        if(!(sv = s->val))FP  // symbol table entry, but no value.
          // Following the original parser, we assume this is an error that has been reported earlier.  No ASSERT here, since we must pop nvr stack
        // The name is defined.  If it's a noun, use its value (the common & fast case)
        // Or, for special names (x. u. etc) that are always stacked by value, keep the value
        // Otherwise (normal adv/verb/conj name), replace with a 'name~' reference
        if((AT(sv)|at)&(NOUN|NAMEBYVALUE)){   // use value if noun or special name
         y=sv;
        } else {
         if (!(y = namerefacv(y, s)))FP   // Replace other acv with reference
        }
       } else {
         // undefined name.  If special x. u. etc, that's fatal; otherwise create a dummy ref to [: (to have a verb)
         if(at&NAMEBYVALUE){jsignal(EVVALUE);FP}  // Report error (Musn't ASSERT: need to pop nvr stack) and quit
         if (!(y = namerefacv(y, s)))FP    // this will create a ref to undefined name as verb [:
           // if syrd gave an error, namerefacv may return 0.  This will have previously signaled an error
       }
       at=AT(y);  // refresh the type with the type of the resolved name

      }

     // If the new word was not a name (whether assigned or not), look to see if it is ) or a conjunction,
     // which allow 2 or 1 more pulls from the queue without checking for an executable fragment.
     // NOTE that we are using the original type for the word, which will be stale if the word was a
     // name that was replaced by name resolution.  We don't care - RPAR was never a name to begin with, and CONJ
     // is much more likely to be a primitive; and we don't want to take the time to refetch the resolved type
     } else es = (at>>CONJX)?at>>CONJX:es;  // 1 for CONJ, 2 for RPAR, 0 otherwise

     // y has the resolved value, which is never a NAME unless there is an assignment immediately following.
     // Put it onto the stack along with a code indicating part of speech and the token number of the word
     PTFROMTYPEASGN(stack[0].pt=stack0pt,at);   // stack the internal type too.  We split the ASGN types into with/without name to speed up IPSETZOMB.  Save pt in a register to avoid store forwarding
     stack[0].t = (UI4)(m+1);  // install the original token number for the word
     stack[0].a = y;   // finish setting the stack entry, with the new word
         // and to reduce required initialization of marks.  Here we take advantage of the fact the CONW is set as a flag ONLY in ASGN type, and that PSN-PS is 1
    }else{  // No more tokens.  If m was 0, we are at the (virtual) mark; otherwise we are finished
      if(m==-1) {stack[0].pt=stack0pt=PTMARK; break;}  // realize the virtual mark and use it.  a and pt will not be needed
      EP       // if there's nothing more to pull, parse is over.  This is the normal end-of-parse
    }
   }while(es-->0);  // Repeat if more pulls required.  We also exit with stack==0 if there is an error
   // words have been pulled from queue

  // Now execute fragments as long as there is one to execute
   while(1) {
    // This is where we execute the action routine.  We give it the stack frame; it is responsible
    // for finding its arguments on the stack, storing the result (if no error) over the last
    // stack entry, then closing up any gap between the front-of-stack and the executed fragment,
    // and finally returning the new front-of-stack pointer

    // First, create the bitmask of parser lines that are eligible to execute
    I pmask=(((~stack0pt)&0x80)*2)+((stack0pt>>24) & (stack[1].pt>>16) & (stack[2].pt>>8) & stack[3].pt);  // bit 8 is set ONLY for LPAR
    if(!pmask)break;  // If all 0, nothing is dispatchable, go push next word

    // We are going to execute an action routine.  This will be an indirect branch, and it will mispredict.  To reduce the cost of the misprediction,
    // we want to pile up as many instructions as we can before the branch, preferably getting out of the way as many loads as possible so that they can finish
    // during the pipeline restart.  The perfect scenario would be that the branch restarts while the loads for the stack arguments are still loading.
    // We also have a couple of branches before the indirect branch, and we try to similarly get some computation going before them
    I pline=CTTZ(pmask);  // Get the # of the highest-priority line
    // Save the stackpointer in case there are calls to parse in the names we execute
    jt->parserstackframe.parserstkend1=stack;
    // Fill in the token# (in case of error) based on the line# we are running
    jt->parserstackframe.parsercurrtok = stack[((I)0x056A9>>(pline*2))&3].t;   // in order 9-0: 0 0 1 1 1 2 2 2 2 1->00 00 01 01 01 10 10 10 10 01->0000 0101 0110 1010 1001
    if(pmask&0x1F){
     // Here for lines 0-4, which execute the routine pointed to by fs
     // Get the branch-to address ASAP.  It comes from the appropriate valence of the appropriate stack element.  Stack element is 2 except for line 0; valence is monadic for lines 0 1 4
     PSTK *stackfs=stack+2-(pmask&1);  // stackpointer for the executing word: 1 2 2 2 2
     A fs=stackfs->a;  // pointer to the A block for the entity about to be executed
//     AF actionfn=FAV(fs)->valencefns[(pline&2)>>1];  // the routine we will execute.  It's going to take longer to read this than we can fill before the branch is mispredicted, usually
     // We will be making a bivalent call to the action routine; it will be w,fs,fs for monads and a,w,fs for dyads (with appropriate changes for modifiers).  Fetch those arguments
     // We have fs already.  arg1 will come from position 2 3 1 1 1 depending on stack line; arg2 will come from 1 2 3 2 3
     if(pmask&0x7){A y;
      // Verb execution.  We must support inplacing, including assignment in place, and support recursion
      jt->sf=fs;  // push $: stack
      // While we are waiting for the branch address, work on inplacing.  See if the primitive being executed is inplaceable
      if((FAV(fs)->flag>>(pline>>1))&VJTFLGOK1){L *s;
       // Inplaceable.  If it is an assignment to a known name that has a value, remember the name and the value
       if(PTISASGNNAME(stack[0])&&PTISM(stackfs[2])&&(FAV(fs)->flag&VASGSAFE)   // assignment to name; nothing in the stack to the right of what we are about to execute; well-behaved function (doesn't change locales)
          &&(s=((AT(stack[0].a))&ASGNLOCAL?jtprobelocal:jtprobeisquiet)(jt,queue[m-1])) ){
        // It is OK to remember the address of the symbol being assigned, because anything that might conceivably create a new symbol (and thus trigger
        // a relocation of the symbol table) is marked as not ASGSAFE
        jt->assignsym=s;  // remember the symbol being assigned.  It may have no value yet, but that's OK - save the lookup
        if(s->val&&AT(stack[0].a)&ASGNLOCAL)jt->zombieval=s->val;  // if the value is being assigned locally & it exists, remember the value.  We have to avoid private/public puns
       }
       jt=(J)(intptr_t)((I)jt+(pline|1));   // set bit 0, and bit 1 if dyadic
      }
      // jt has been corrupted, now holding inplacing info
      // CODING NOTE: after considerable trial and error I found this ordering, whose purpose is to start the load of the indirect branch address as early as
      // possible before the branch.  Check the generated code on any change of compiler.
      // Since we have half a dozen or so cycles to fill, push the $: stack and close up the execution stack BEFORE we execute the verb.  If we didn't close up the stack, we
      // could avoid having the $: stack by having $: look into the execution stack to find the verb that is being executed.  But overall it is faster to pay the expense of the $:
      // stack in exchange for being able to fill the time before the misprediction
      AF actionfn=FAV(fs)->valencefns[pline>>1];  // the routine we will execute.  It's going to take longer to read this than we can fill before the branch is mispredicted, usually
      // There is no need to set the token number in the result, since it must be a noun and will never be executed
      // Close up the stack.  For lines 0&2 we don't need two writes, so they are duplicates
      A arg2=stack[pline+1].a;   // 2nd arg, fs or right dyad  1 2 3 (2 3)
      stackfs[0]=stackfs[-1];    // overwrite the verb with the previous cell - 0->1  1->2  1->2
      A arg1=stack[(0x6>>pline)&3].a;   // 1st arg, monad or left dyad  2 3 1 (1 1)   0110  0 1 2 -> 2 3 1   1 11 111
      stack[pline]=stack[0];  // close up the stack  0->0(NOP)  0->1   0->2
      stack+=(pline>>1)+1;   // finish relocating stack   1 1 2 (1 2)
      y=(*actionfn)(jt,arg1,arg2,fs);
      jt=(J)(intptr_t)((I)jt&~JTFLAGMSK);
      // jt is OK again
      EPZ(y);  // fail parse if error
      stackfs[1].a=y;  // save result 2 3 3 2 3; parsetype is unchanged, token# is immaterial
     }else{
      // Lines 3-4, conj/adv execution.  We must get the parsing type of the result, but we don't need to worry about inplacing or recursion
      AF actionfn=FAV(fs)->valencefns[pline-3];  // the routine we will execute.  It's going to take longer to read this than we can fill before the branch is mispredicted, usually
      A arg1=stack[1].a;   // 1st arg, monad or left dyad
      A arg2=stack[pline-1].a;   // 2nd arg, fs or right dyad
      UI4 restok=stack[1].t;  // save token # to use for result
      stack[pline-2]=stack[0]; // close up the stack
      stack=stack+pline-2;  // advance stackpointer to position before result 1 2
      A y=(*actionfn)(jt,arg1,arg2,fs);
      EPZ(y);  // fail parse if error
      PTFROMTYPE(stack[1].pt,AT(y)) stack[1].t=restok; stack[1].a=y;   // save result, move token#, recalc parsetype
     }
    }else{
     // Here for lines 5-8 (fork/hook/assign/parens), which branch to a canned routine
     // It will run its function, and return the new stackpointer to use, with the stack all filled in.  If there is an error, the returned stackpointer will be 0.
     stack=(*lines58[pline-5])(jt,stack);  // run it
     if(!stack)FP  // fail if error
     stack0pt=stack[0].pt;  // bottom of stack was modified, so refresh the type for it (lines 0-6 don't change it)
    }
   }
  }  // break with stack==0 on error; main exit is when queue is empty (m<0)
 exitparse:
   // Prepare the result

  if(stack){  // if no error yet...
   // before we exited, we backed the stack to before the initial mark entry.  At this point stack[0] is invalid,
   // stack[1] is the initial mark, stack[2] is the result, and stack[3] had better be the first ending mark
   z=stack[2].a;   // stack[1..2] are the mark; this is the sentence result, if there is no error
   if(!(PTISCAVN(stack[2])&&PTISM(stack[3]))){jt->parserstackframe.parsercurrtok = 0; jsignal(EVSYNTAX); z=0;}  // OK if 0 or 1 words left (0 should not occur)
  }else{
failparse:  // If there was an error during execution or name-stacking, exit with failure.  Error has already been signaled.  Remove zombiesym
   CLEARZOMBIE z=0;
  }

  // Now that the sentence has completed, take care of some cleanup.  Names that were reassigned after
  // their value was moved onto the stack had the decrementing of the use count deferred: we decrement
  // them now.  There may be references to these names in the result (if we are returning a verb/adv/conj),
  // so we don't free the names quite yet: we put them on the tpush stack to be freed after we know
  // we are through with the result.  If we are returning a noun, free them right away
  v=jt->nvrav+nvrotop;  // point to our region of the nvr area
  DQ(jt->parserstackframe.nvrtop-nvrotop, A vv = *v; I vf = AFLAG(vv); AFLAG(vv) = vf & ~(AFNVR|AFNVRUNFREED); if(!(vf&AFNVRUNFREED))if(!z||AT(z)&NOUN){fa(vv);}else{tpush(vv);} ++v;);   // schedule deferred frees.
  // Still can't return till frame-stack popped

  jt->parserstackframe = oframe;
  // NOW it is OK to return
  R z;  // this is the return point from normal parsing

 }else{A y;  // m<2.  Happens fairly often, and full parse can be omitted
  if(m==1){  // exit fast if empty input.  Happens only during load, but we can't deal with it
   // Only 1 word in the queue.  No need to parse - just evaluate & return.  We do it here to avoid parsing
   // overhead, because it happens enough to notice
   jt->parserstackframe.parsercurrtok=0;  // error token if error found
   I at=AT(y = queue[0]);  // fetch the word
   if(at&NAME) {L *s;
    if(s=syrd(y)) {     // Resolve the name.
      A sv;  // pointer to value block for the name
      RZ(sv = s->val);  // symbol table entry, but no value.  Must be in an explicit definition, so there is no need to raise an error
      if((AT(sv)|at)&(NOUN|NAMEBYVALUE)){   // in noun or special name, use value
       y=sv;
      } else RZ(y = namerefacv(y, s));   // Replace other acv with reference
    } else {
      // undefined name.
      ASSERT(!(at&NAMEBYVALUE),EVVALUE)  // Error if the unresolved name is x y etc
      RZ(y = namerefacv(y, s));    // this will create a ref to undefined name as verb [:
    }
   }
   ASSERT(AT(y)&CAVN,EVSYNTAX);
  }else y=mark;  // empty input - return with 'mark' as the value, which means nothing to parse.  This result must not be passed into a sentence
  jt->parserstackframe = oframe;
  R y;
 }

}

