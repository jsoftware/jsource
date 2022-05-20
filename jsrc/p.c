/* Copyright (c) 1990-2022, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Parsing; see APL Dictionary, pp. 12-13 & 38.                            */

#include "j.h"
#include "p.h"
#include <stdint.h>

#define RECURSIVERESULTSCHECK   //  if(y&&(AT(y)&NOUN)&&!(AFLAG(y)&AFVIRTUAL)&&((AT(y)^AFLAG(y))&RECURSIBLE))SEGFAULT;  // stop if nonrecursive noun result detected


#define PARSERSTKALLO (490*sizeof(PSTK))  // number of stack entries to allocate, when we allocate, in bytes

/* parsing benchmark

parsing & name lookup + loop index
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
totl =. 0. [ a =. 2. [ ab =. 3.
for. y do.
 totl =. totl + +: 1. - +: a * ab + 1.
 totl =. * totl - ab + - a
 totl =. totl - 2.
end.
totl
)
(10) 6!:2 'namelkp i. 1e6'

parsing+loop
namelkp =: 3 : 0
for. y do.
y
end.
0
)
(10) 6!:2 'namelkp i. 1e6'

namelkp =: 3 : 0
for. y do.
end.
0
)
(10) 6!:2 'namelkp i. 1e6'

*/
B jtparseinit(JS jjt, I nthreads){
 R 1;
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
// Put something distinctive into LPAR that can be used to create line 8
// We distinguish local and global assignments by having local assignments enable line 6 (hook) which must be rejected in jthook
#define PTNOUN 0xDFC17CBE
#define PTMARK 0xC900007F
static const __attribute__((aligned(CACHELINESIZE))) UI4 ptcol[16] = {
[LASTNOUNX-LASTNOUNX] = PTNOUN,  // PN
[NAMEX-LASTNOUNX] = 0xC9200080,  // PNM assigned: high 16 bits immaterial because this MUST match line 7 which will delete it
[MARKX-LASTNOUNX] = PTMARK,  // PM
[ADVX-LASTNOUNX] = 0xC9C8403E,  // PA
// gap[ASGNX-LASTNOUNX]   must be left open in enqueue() tokens to avoid messing up the pull queue
[VALTYPENAMELESSADV-1] = 0xC9C8403E, // gap [SYMBX-LASTNOUNX]  only in syrd() results, i. e. from symbol
[VALTYPESPARSE-1] = PTNOUN,  // gap [CONWX-LASTNOUNX]  only in syrd() results, i. e. from symbol
[VERBX-LASTNOUNX] = 0xF9E67B3E,  // PV
[LPARX-LASTNOUNX] = 0x0100007F,  // PL
[CONJX-LASTNOUNX] = 0xC9D04000,  // PC
[RPARX-LASTNOUNX] = 0xC9000000,  // PR
// the LOCAL and ASGNTONAME flags are passed into PT
[QCASGN-1] = 0xC900807F,
[QCASGN+QCASGNISLOCAL-1] = 0xC940807F,
[QCASGN+QCASGNISTONAME-1] = 0xC800807F,
[QCASGN+QCASGNISLOCAL+QCASGNISTONAME-1] = 0xC840807F
};

// tests for pt types
#define PTISCAVNX 23  // this flag used in a register here
#define PTISCAVN(pt) ((pt)&(1LL<<PTISCAVNX))
#define PTISRPAR0(pt) ((pt)&0x7fff)
#define PTISM(s)  ((s).pt==PTMARK)
#define PTOKEND(t2,t3) (((PTISCAVN(~(t2).pt))+((t3).pt^PTMARK))==0)  // t2 is CAVN and t3 is MARK
#define PTNAMEIFASGN(pt)  ((pt)<<(NAMEX-15))   // we compare against the NAMEX bit
#define PTISNOTASGNNAME(pt)  ((pt)&0x1000000)
#define PTASGNLOCALX 22  // set in the type-code for local assignment
#define PTASGNLOCAL (1LL<<PTASGNLOCALX)
#define PTNOTLPARX 27  // this bit is set for NOT LPAR    used in a register here
#define PTNOTLPAR (1LL<<PTNOTLPARX)  // this bit is set in pt only if NOT LPAR
#define PTNAME0X 21  // when pt[0] is known to be NAME or NOUN, this bit is set if NAME
#define PTNAME0 (1LL<<PTNAME0X)
// converting type field to pt, store in z
#define PTFROMTYPE(z,t) {I pt=CTTZ(t); pt=(t)&(((1LL<<(LASTNOUNX+1))-1))?LASTNOUNX:pt; z=ptcol[pt-LASTNOUNX];}  // here when we know it's CAVN (not assignment)


// multiple assignment not to constant names.  self has parms.  ABACK(self) is the symbol table to assign to, valencefns[0] is preconditioning routine to open value or convert it to AR
// We flag all multiple assignments as final because the value is protected in the source
static DF2(jtisf){RZ(symbisdel(onm(a),CALL1(FAV(self)->valencefns[0],w,0L),ABACK(self))); R num(0);} 

// assignment, single or multiple
// pt0 i the PT code for the left-hand side, pt0 is the PT code for the assigptnment, m is the token number to be assigned next (0 if the next thing is MASK)
// jt has flag set for final assignment (passed into symbis)
// The return must be 0 for bad, anything else for good
static I NOINLINE jtis(J jt,A n,A v,A symtab){F1PREFIP;
 B ger=0;C *s;
 if(unlikely(AT(n)==BOX+BOXMULTIASSIGN)){   // test both bits, since BOXMULTIASSIGN has multiple uses
  // string assignment, where the NAME blocks have already been computed.  Use them.  The fast case is where we are assigning a boxed list
  if(AN(n)==1)n=AAV(n)[0];  // if there is only 1 name, treat this like simple assignment to first box, fall through
  else{
   // True multiple assignment
   ASSERT((-(AR(v))&(-(AN(n)^AS(v)[0])))>=0,EVLENGTH);   // v is atom, or length matches n
   if(((AR(v)^1)+(~AT(v)&BOX))==0){A *nv=AAV(n), *vv=AAV(v); DO(AN(n), jtsymbis(jtinplace,nv[i],C(vv[i]),symtab);)}  // v is boxed list
   else {A *nv=AAV(n); DO(AN(n), jtsymbis((J)((I)jtinplace|JTFINALASGN),nv[i],ope(AR(v)?from(sc(i),v):v),symtab);)}  // repeat atomic v for each name, otherwise select item.  Open in either case; always final assignment
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
 // if simple assignment to a name (normal case), do it.  To get here it must have been a length-1 list of names
 if(unlikely((NAME&AT(n))!=0)){
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
retstack:  // return, but 0 if error
 R !jt->jerr;  // return 0 if error
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
   DO(AN(w), if(wv[i]&&(AC(C(wv[i]))<0))SEGFAULT;)
   I acbias=(AFLAG(w)&BOX)!=0;  // subtract 1 if recursive
   if(AFLAG(w)&AFPRISTINE){DO(AN(w), if(!((AT(C(wv[i]))&DIRECT)>0))SEGFAULT;)}  // wv[i]&&(AC(w)-acbias)>1|| can't because other uses may be not deleted yet
   {DO(AN(w), auditblock(jt,C(wv[i]),nonrecur,0););}
   }
   break;
  case VERBX: case ADVX:  case CONJX: 
   {V*v=VAV(w); auditblock(jt,C(v->fgh[0]),nonrecur,0);
    auditblock(jt,C(v->fgh[1]),nonrecur,0);
    auditblock(jt,C(v->fgh[2]),nonrecur,0);} break;
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




// Run parser, creating a new debug frame.  Explicit defs, which make other tests first, go through jtparsea except during debug/pm
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

// name:: delete the symbol name but not deleting the value.  Undo the pending fa: If usecount goes to 1, make it abandoned inplaceable and tpush them
// Incoming y is the value attached to the symbol & has QCGLOBAL semantics, result is same value with QCFAOWED semantics
// result is the value, possibly with FAOWED set
static A namecoco(J jt, A name, A y){F1PREFIP;
 if(((I)jtinplace&JTFROMEXEC))R SETFAOWED(y);   // in "., we can't do this because the value is not protected by FAOWED, so indic that we need to fa
 A locfound=jt->locsyms; if(unlikely(((I)y&QCGLOBAL)!=0))locfound=syrdforlocale(name);  // get locale to use.  This re-looks up global names, but they should be rare in name::
 WRITELOCK(locfound->lock)
 jtprobedel((J)((I)jt+NAV(name)->m),NAV(name)->s,NAV(name)->hash,locfound);  // delete the symbol (incl name and value) in the locale in which it is defined
 WRITEUNLOCK(locfound->lock)
 // The name has been deleted.  The value is still protected by the fa() made in syrd.
 if(likely(!ISSPARSE(AT(QCWORD(y)))))if(likely(AC(QCWORD(y))==ACUC1)){
  // The usecount has gone down to 1 (including the fa from syrd), the block can become inplaceable again.  We have to give it a valid zaploc in case anyone wants to use it.
  // In this case the tpush undoes the ra() from syrd and we do not set FAOWED
  ACSET(QCWORD(y),ACINPLACE+ACUC1); AZAPLOC(QCWORD(y))=jt->tnextpushp; tpushna(QCWORD(y)); R CLRFAOWED(y);
 }
 R SETFAOWED(y);
}

#define FP goto failparse;   // indicate parse failure and exit
#define EP(x) {pt0ecam=(x); goto exitparse;}   // exit parser with success; x = 1 if final assignment
#define FPZSUFF(x,suff) if(unlikely(!(x))){suff FP}   // exit parser w/failure if x==0, doing suff on the way out
#define FPZ(x) FPZSUFF(x,)   // exit parser w/failure if x==0

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
//  the correct address after the execution.  The address of the block that is to be assigned is in jt->zombieval
/
// If jt->zombieval is set, the (necessarily inplaceable) verb may choose to perform an in-place
// operation.  It will check usecounts and addresses to decide whether to do this, and it bears the responsibility
// of worrying about names on the stack.  Note that local names are not put onto the stack, so absence of AFNVR suffices for them.
#endif

#if 0  // for this to work we would have to rearrange the stack AFTER execution
// Given that we took an error executing the block starting at jt->parserstackframe.parserstkend1, figure out what the error-token number should be
I infererrtok(J jt){
 // if the sentence had only one token, that token is the error token
 if(jt->parserstackframe.parserqueuelen==1)R 1;  // lust one token, return it
 // reparse the fragment to get the line# that was executed
 I pmask=(I)((C*)&jt->parserstackframe.parserstkend1[0].pt)[0] & (I)((C*)&jt->parserstackframe.parserstkend1[1].pt)[1] &(I)((C*)&jt->parserstackframe.parserstkend1[2].pt)[2] &(I)((C*)&jt->parserstackframe.parserstkend1[3].pt)[3];  // parse
 // see if ( was executable
 pmask|=jt->parserstackframe.parserstkend1[0].pt&PTNOTLPAR?0:0x100;  // ( any any any is line 8
 // Get the number that was executed.  Take the token# from entry (9-0): 2 1 1 1 1 2 2 2 2 1 (line 9 is no match, must be ending syntax error)
 R jt->parserstackframe.parserstkend1[((0x21e>>CTTZI(pmask|0x200))&1)+1].t;  // return failing token#
}
#endif

#define BACKMARKS 3   // amount of space to leave for marks at the end.  Because we stack 3 words before we start to parse, we will
 // never see 4 marks on the stack - the most we can have is 1 value + 3 marks.
#define FRONTMARKS 1  // amount of space to leave for front-of-string mark
#define PSTACKRSV 1  // number of stack frames used for holding sentence error info
// Parse a J sentence.  Input is the queue of tokens
// Result has PARSERASGNX (bit 0) set if the last thing is an assignment
// JT flag is used to indicate execution from ". - we can't honor name:: then, or perhaps some assignments
A jtparsea(J jt, A *queue, I nwds){F1PREFIP;PSTK * stack;A z,*v;
 // jt->parsercurrtok must be set before executing anything that might fail; it holds the original
 // word number+1 of the token that failed.  jt->parsercurrtok is set before dispatching an action routine,
 // so that the information is available for formatting an error display

 // Save info for error typeout.  We save sentence info once, and token info for every executed fragment
 PFRAME oframe=jt->parserstackframe;   // save all the stack status

 // allocate the stack.  No need to initialize it, except for the marks at the end, because we
 // never look at a stack location until we have moved from the queue to that position.
 // If there is a stack inherited from the previous parse, and it is big enough to hold our queue, just use that.
 // It goes from stkbgn to stkend1.  During execution we update stkend1 before each execution, and stkbgn at the beginning of each execution
 // The stack grows down
 PSTK *currstk=jt->parserstackframe.parserstkbgn;  // current stack entry
 if(unlikely(((intptr_t)jt->parserstackframe.parserstkend1-((intptr_t)jt->parserstackframe.parserstkbgn+(nwds+BACKMARKS+FRONTMARKS+PSTACKRSV+1)*(intptr_t)sizeof(PSTK)))<0)){A y;
  ASSERT(nwds<65000,EVLIMIT);  // To keep the stack frame small, we limit the number of words of a sentence
  I allo = MAX((nwds+BACKMARKS+FRONTMARKS+PSTACKRSV+1)*sizeof(PSTK),PARSERSTKALLO); // number of bytes to allocate.  Allow 4 marks: 1 at beginning, 3 at end
  GATV0(y,B01,allo,1);
  currstk=(PSTK*)CAV(y);   // save start of data area, leaving space for all the marks
  // We are taking advantage of the fact the NORMAH is 7, and thus a rank-1 array is aligned on a boundary of its size
  jt->parserstackframe.parserstkend1=(PSTK*)(CAV(y)+allo);  // point to the end+1 of the allocation
 }  // We could worry about hysteresis to avoid reallocation of every call

 // the first element of the parser stack is where we save unchanging error info for the sentence
 currstk->a=(A)queue; currstk->t=(US)nwds;  // addr & length of words being parsed
 jt->parserstackframe.parserstkbgn=currstk+PSTACKRSV;  // advance over the error info, creating an upward-growing stack at the bottom of the area jt->parserstackframe.parserstkbgn[-1] has the error info

 if(likely(nwds>1)) {  // normal case where there is a fragment to parse
  // mash into 1 register:  bit 32-63 stack0pt, bit 29-31 (from CONJX) es delayline pull 3/2/1 after current word,
  //  (exec) 23-24,26 VJTFLGOK1+VJTFLGOK2+VASGSAFE from verb flags 27 PTNOTLPARX set if stack[0] is not (  17 set if first stack word AFTER the executing fragment is NOT MARK (i. e. there are executions remaining on the stack) 
  //  (name resolution) 23-26  free
  //  (exec) 20-22 savearea for pmask for lines 0-2  (stack) 17,20 flags from at NAMEBYVALUE/NAMEABANDON, 21 flag to indicate global symbol table used
  //  18-19 AR flags from symtab
  //  16 set if value has been added to NVR stack
  //  0-15 m (word# in sentence)
#define LOCSYMFLGX (18-ARNAMEADDEDX)
#define PLINESAVEX 20  // 3 bits of pline
// above #define USEDGLOBALX 21
// above #define USEDGLOBAL (1LL<<USEDGLOBALX)
#define NOTFINALEXECX 17
#define NOTFINALEXEC (1LL<<NOTFINALEXECX)
#define NAMEFLAGSX 17  // 17 and 20
#define NVRSTACKEDX 16
#define NVRSTACKED (1LL<<NVRSTACKEDX)
  // STACK0PT needs only enough info to decode from position 0.  It persists into the execution phase
#if SY_64
#define SETSTACK0PT(v) pt0ecam=(UI4)pt0ecam, pt0ecam|=(I)(v)<<32;
#define GETSTACK0PT (pt0ecam>>32)
#define STACK0PTISCAVN (pt0ecam&(1LL<<(32+PTISCAVNX)))
#else
  UI4 stack0pt;
#define SETSTACK0PT(v) stack0pt=(v);
#define GETSTACK0PT stack0pt
#define STACK0PTISCAVN PTISCAVN(stack0pt)
#endif
  UI pt0ecam = (AR(jt->locsyms)&(ARLCLONED|ARNAMEADDED))<<LOCSYMFLGX;  // insert clone/added flags into portmanteau vbl.  locsyms cannot change during this execution

  queue+=nwds-1;  // Advance queueptr to last token.  It always points to the next value to fetch.

  // If words -1 & -2 exist, we can pull 4 words initially unless word -1 is ASGN or word -2 is EDGE or word 0 is ADV.  Unfortunately the ADV may come from a name so we also have to check in that branch
  // It has long latency so we start early.  The actual computation is about 3 cycles, much faster than a table search+misbranch
  I pull4=0; pull4=((~((0xfLL<<QCASGN)|(0x1LL<<QCLPAR))>>QCTYPE(queue[-1])) & (~((0xfLL<<QCASGN)|(0x1LL<<QCLPAR))>>QCTYPE(queue[-2])))&1;

  // Set number of extra words to pull from the queue.  We always need 2 words after the first before a match is possible.  If neither of the last words is EDGE, we can take 3
  pt0ecam += nwds+((0b11LL<<CONJX)<<pull4);
// debug if(jt->parsercalls==0x9d0)
// debug jt->parsercalls=0x9d0;


  A y;  // y will be the word+flags for the next queue value to process.  We reload it as so that it never has to be saved over a call
  y=*(volatile A*)queue;  // unroll y once

  ++jt->parsercalls;  // now we are committed to full parse.  Push stacks.
  PSTK *stackend1=stack=jt->parserstackframe.parserstkend1-BACKMARKS;   // start at the end, with 3 marks

  // We don't actually put a mark in the queue at the beginning.  When m goes down to 0, we infer a mark.
  // DO NOT RETURN from inside the parser loop.  Stacks must be processed.

  // We have the initial stack pointer.  Grow the stack down from there
  stack[0].pt = stack[1].pt = stack[2].pt = PTMARK;  // install initial ending marks.  word numbers and value pointers are unused
  while(1){  // till no more matches possible...

    // no executable fragment, pull from the queue.  If we pull ')', there is no way we can execute
    // anything till 2 more words have been pulled, so we pull them here to avoid parse overhead.
    // Likewise, if we pull a CONJ, we can safely pull 1 more here.  And first time through, we should
    // pull 2 words following the first one.
    // es holds the number of extra pulls required.

    // pt0ecam is settling from pt0 but it will be ready soon
   
   do{
      // to make the compiler keep queue in regs, you have to convince it that the path back to this loop is common enough
      // to give it priority.  likelys at the end of the line 0-2 code are key

    if(likely((US)pt0ecam!=0)){  //      if there is another valid token...
     // Move in the new word and check its type.  If it is a name that is not being assigned, resolve its
     // value.  m has the index of the word we just moved.  y = *queue

     // We have the value/typeclass of the next word.  If it is an unassigned name, we have to resolve it and perhaps use the new name/type
     if(!((I)y&QCISLKPNAME)){
      // not a name requiring lookup; turn off FAOWED
      y=CLRFAOWED(y);  // y is now addr/0/type index
     }else{  // Replace a name (not to left of ASGN) with its value
      // Name, not being assigned
      // Resolve the name.  If the name is x. m. u. etc, always resolve the name to its current value;
      // otherwise resolve nouns to values, and others to 'name~' references
      // The important performance case is local names with symbol numbers.  Pull that out & do it without the call overhead
      // Registers are very tight here.  Nothing survives over a subroutine call - refetch y if necessary  If we have anything to
      // pass over a subroutine call, we have to store it pt0ecam or some other saved name
      I4 symx, buck;
      symx=NAV(QCWORD(y))->symx; buck=NAV(QCWORD(y))->bucket;
      L *sympv=SYMORIGIN;  // fetch the base of the symbol table.  This can't change between executions but there's no benefit in fetching earlier
      I bx=NAVV(QCWORD(y))->bucketx;  // get an early fetch in case we don't have a symbol but we do have buckets - globals, mainly
      pt0ecam&=~(NAMEBYVALUE+NAMEABANDON)>>(NAMEBYVALUEX-NAMEFLAGSX);
      pt0ecam|=((I)y&(QCNAMEABANDON+QCNAMEBYVALUE))<<NAMEFLAGSX;
      y=QCWORD(y);  // back y up to the NAME block
      if((symx&~REPSGN4(SGNIF4(pt0ecam,LOCSYMFLGX+ARLCLONEDX)))!=0){  // if we are using primary table and there is a symbol stored there...
       L *s=sympv+(I)symx;  // get address of symbol in primary table
       if(unlikely(s->valtype==0))goto rdglob;  // if value has not been assigned, ignore it.  Could just treat as undef
       y=(A)((I)s->val+s->valtype);  //  combine the type and value.  type has QCGLOBAL semantics, as y does.
       raposlocalqcgsv(s->val,s->valtype,y);  // ra() the value to match syrd.  type has global flag clear
      }else if(likely(buck!=0)){  // buckets but no symbol - must be global, or recursive symtab - but not synthetic name
       if((bx|SGNIF(pt0ecam,ARNAMEADDEDX+LOCSYMFLGX))>=0)goto rdglob;  // if positive bucketx and no name has been added, skip the search - the usual case if not recursive symtab
       if((y=probelocalbuckets(sympv,y,LXAV0(jt->locsyms)[buck],bx))==0){y=QCWORD(*(volatile A*)queue);goto rdglob;}  // see if there is a local symbol, using the buckets.  If not, restore y
       raposlocalqcgsv(QCWORD(y),QCPTYPE(y),y);  // ra() the value to match syrd
      }else{
       // No bucket info.  Usually this is a locative/global, but it could be an explicit modifier, console level, or ".
rdglob: ;  // here when we tried the buckets and failed
       // If the name has a cached reference, use it
       if(NAV(y)->cachedref!=0){  // if the user doesn't care enough to turn on caching, performance must not be that important
        // Note: this cannot be a NAMEABANDON, because such a name is never stacked where it can have the cachedref filled in
        y=NAV(y)->cachedref; // use the cached address/flags, which has QCFAOWED semantics
        goto endname; // take its type, proceed.  We skip the FAOWED issues (FAOWED must be clear in the cached addr)
       }
       jt->parserstackframe.parsercurrtok = (US)pt0ecam;  // syrd can fail, so we have to set the error-word number (before it is decremented) before calling
       if(unlikely((y=syrdnobuckets(y))==0))goto undefname;  // do full symbol lookup, knowing that we have checked for buckets already.  Error if not defined
      }
      // end of looking at local/global symbol tables.
      // y has QCGLOBAL semantics here, and has the type flags.  When we finish y must have QCFAOWED semantics, still with the type flags
      // *queue still has the original y, We access it as volatile so the compiler will not put it into a register
      //
      // ra() has been issued for the value.  If we don't undo that, we must set FAOWED in y to indicate that fact.  The value will stay protected until
      // the execution that uses this y has completed and the result has been made recursive, so that we know the input value has been RA()d if it has been used
      if(likely(1)){
       // The name is defined.  If it's a noun, use its value (the common & fast case)
       // Or, for special names (x. u. etc) that are always stacked by value, keep the value
       // If a modifier has no names in its value, we will stack it by value.  The Dictionary says all modifiers are stacked by value, but
       // that will make for tough debugging.  We really want to minimize overhead for each/every/inv.  nameless was detected on assignment.
       // But: if the name is any kind of locative, we have to have a full nameref so unquote can switch locales: can't use the value then
       // Otherwise (normal adv/verb/conj name), replace with a 'name~' reference
       if((pt0ecam&(NAMEBYVALUE>>(NAMEBYVALUEX-NAMEFLAGSX)))|((I)y&QCNOUN)){   // use value if noun or special name, or name::
        if(unlikely((pt0ecam&(NAMEABANDON>>(NAMEBYVALUEX-NAMEFLAGSX))))){FPZSUFF(y=namecoco(jtinplace, QCWORD(*(volatile A*)queue), y), fa(QCWORD(y));)}  // if name::, go delete the name, leaving the value to be deleted later
        else y=SETFAOWED(y);
       }else if(unlikely(QCPTYPE(y)==VALTYPENAMELESSADV)){
        // nameless modifier, and not a locative.  This handles 'each'.  Don't create a reference; maybe cache the value
        A origy=QCWORD(*(volatile A*)queue);  // refetch y so we can look at its flags
        y=(A)((I)y+QCADV-VALTYPENAMELESSADV);  // convert type to normal adverb, which the parser looks for
        if(NAV(origy)->flag&NMCACHED){  // nameless mod is cachable - replace it by its value in the name
         // cachable and not a locative (and not a noun).  store the value in the name, make the value permanent
         NAV(origy)->cachedref=CLRFAOWED(y); NAV(origy)->bucket=0; ACSETPERM(y); // clear bucket info so we will skip that search - this name is forever cached with QCFAOWED semantics.  Make the cached value immortal
        }
        y=SETFAOWED(y);
       }else{  // not a noun/nonlocative-nameless-modifier.  We have to stack a reference to the name.  But if the value IS a reference, use the value if possible to avoid the extra lookup
        A origname=QCWORD(*(volatile A*)queue);  // refetch the name
        if(unlikely(FAV(y)->valencefns[0]==jtunquote && !(NAV(origname)->flag&(NMLOC|NMILOC|NMIMPLOC)))){  // reference is as reference does
         // the value is a non-locative reference to another reference.  It is safe to skip over it.  Leave y holding the value
        }else{y=namerefacv(origname, y);}   // Replace other acv with reference, and fa() looked-up y value
        FPZ(y)
       }
      }else{
undefname:
       // undefined name.  If special x. u. etc, that's fatal; otherwise create a dummy ref to [: (to have a verb)
       // Following the original parser, we assume this is an error that has been reported earlier.  No ASSERT here, since we must pop nvr stack
       if(pt0ecam&(NAMEBYVALUE>>(NAMEBYVALUEX-NAMEFLAGSX))){jsignal(EVVALUE);FP}  // Report error (Musn't ASSERT: need to pop all stacks) and quit
       y=namerefacv(QCWORD(*(volatile A*)queue), 0);    // this will create a ref to undefined name as verb [:, including flags
       FPZ(y)   // if syrd gave an error, namerefacv will return 0 (from fdef).  This will have previously signaled an error
      }
endname: ;
     }

     // names have been resolved
     // y has the resolved value, which has the 4-bit QC type with QCFAOWED semantics (QCFAOWED set if a fa is needed on the value)
     // Look to see if it is ) or a conjunction,
     // which allow 2 or 1 more pulls from the queue without checking for an executable fragment.
     // Also, dyad executions sometimes allow two pulls if the first one is AVN.
     I tx=QCPTYPE(y);  // extract the 4-bit parsing-type index
     I it=ptcol[tx-1];   // convert type to internal code
     --stack;  // back up to new stack frame, where we will store the new word
     stack[0].t = (US)pt0ecam;  // install the original token number for the word
     --pt0ecam;  //  decrement token# for the word we just processed
     queue--;  // OK to fetch queue[-1]
     // stack the value, changing it to STKFAOWED semantics
     stack[0].a = (A)(((I)y&~STKFAOWED)+(((I)y>>(QCFAOWEDX-STKFAOWEDX))&STKFAOWED));   // finish setting the stack entry, with the new word.  The stack entry has STKFAOWED with garbage in other type flags
     y=*(volatile A*)queue;   // fetch next value as early as possible
     pt0ecam|=((1LL<<(LASTNOUNX-1))<<tx)&(3LL<<CONJX);   /// install pull count es  OR it in: 000= no more, other 001=1 more (CONJ), 01x=2 more (RPAR).  
     UI4 tmpes=pt0ecam;  // pt0ecam is going to be settling because of stack0pt.  To ratify the branch faster we save the relevant part (the pull queue)
     pt0ecam&=(I)(UI4)~((0b111LL<<CONJX));  // clear the pull queue, and all of the stackpt0 field if any.  This is to save 2 fetches in executing lines 0-2 for =:
     // Put new word onto the stack along with a code indicating part of speech and the token number of the word
     SETSTACK0PT(it) stack[0].pt=it;   // stack the internal type too.  We split the ASGN types into with/without name to speed up IPSETZOMB.  Save pt in a register to avoid store forwarding.  Only parts have to be valid; we use the rest as flags
         // and to reduce required initialization of marks.  Here we take advantage of the fact the CONW is set as a flag ONLY in ASGN type
     // *** here is where we exit stacking to do execution ***
     if(!(tmpes&(0b111LL<<CONJX)))break;  // exit stack phase when no more to do, leaving es=0
     // we are looping back for another stacked word before executing.  Restore the pull queue to pt0ecam, after shiting it down and shortening it if we pulled ADV
     pt0ecam|=((tmpes>>=(CONJX+1))&(~(1LL<<(QCADV+1))>>tx))<<CONJX;  // bits 31-29: 1xx->010 01x->001 others->000.  But if ADV, change request for 2 more to request for 1 more by clearing bit 30.
         // We shift the code down to clear the LSB, leaving 0xx.  Then we AND away bit 1 if ADV.  tx is never QCADV+1, because ASGN is mapped to 12-15, so we never AND away bit 0
    }else{  // No more tokens.  If m was 0, we are at the (virtual) mark; otherwise we are finished
     --stack;  // back up to new stack frame, where we will store the new word
     if(GETSTACK0PT!=PTMARK){SETSTACK0PT(PTMARK) break;}  // first time m=0.  realize the virtual mark and use it.  e and ca flags immaterial
     EP(0)       // second time.  there's nothing more to pull, parse is over.  This is the normal end-of-parse (except for after assignment)
     // never fall through here
    }
   }while(1);  // Repeat if more pulls required.  We also exit with stack==0 if there is an error
   // words have been pulled from queue.

   pt0ecam|=(((CONJ>>QCADV)|(CONJ>>QCVERB)|(CONJ>>QCNOUN)|(CONJ>>QCISLKPNAME)|(CONJ>>(QCISLKPNAME+QCNAMEBYVALUE))|(CONJ>>(QCISLKPNAME+QCNAMEBYVALUE+QCNAMEABANDON)))
      <<(QCTYPE(y)))&CONJ;  // save next-is-CAVN status in CONJ (which will become the request for a 2nd pull).

#define NEXTY ((volatile A *)queue)[0]

  // Now execute fragments as long as there is one to execute.
   while(1) {
    // This is where we execute the action routine.  We give it the stack frame; it is responsible
    // for finding its arguments on the stack, storing the result (if no error) over the last
    // stack entry, then closing up any gap between the front-of-stack and the executed fragment,
    // and finally returning the new front-of-stack pointer
    // First, create the bitmask of parser lines that are eligible to execute
    // register pressure is severe
    I pmask=(I)((C*)&stack[1].pt)[1] & (I)((C*)&stack[2].pt)[2];  // stkpos 0-2 are enough to detect a match on line 0
    pmask&=(I)((C*)&stack[3].pt)[3];  // fetch early
    pt0ecam&=~(VJTFLGOK1+VJTFLGOK2+VASGSAFE+PTNOTLPAR+NOTFINALEXEC+(7LL<<PLINESAVEX));   // clear all the flags we will use
    pmask&=GETSTACK0PT;  // finish 1st 3 columns, whiich are enough to decide bit 0
// too many regs   A fs=*(volatile A *)&stack[2].a; {A fs1=*(volatile A *)&stack[1].a;// Read both possibilities to reduce latency
// even when forced to calculate   jt->shapesink[0]=(I)(fs=pmask&1?fs1:fs);} // choose correct values of fs
    PSTK *fsa=&stack[2-(pmask&1)];  // pointer to stack slot for the CAV to be executed, for lines 0-4
    // We have a long chain of updates to pt0ecam; start them now.  Also, we need fs and its flags; get them as early as possible
    
    if(pmask){  // If all 0, nothing is dispatchable, go push next word
     A fs=QCWORD(fsa->a);  // the action to be executed if lines 0-4
     pt0ecam|=(!PTISM(fsa[2]))<<NOTFINALEXECX;  // remember if there is something on the stack after the result of this exec.   Wait till we know not fail, so we don't have to wait for (
     jt->parserstackframe.parsercurrtok = fsa[0].t;   // in order 4-0: 2 2 2 2 1
     // We are going to execute an action routine.  This will be an indirect branch, and it will mispredict.  To reduce the cost of the misprediction,
     // we want to pile up as many instructions as we can before the branch, preferably getting out of the way as many loads as possible so that they can finish
     // during the pipeline restart.  The perfect scenario would be that the branch restarts while the loads for the stack arguments are still loading.
     // We also have a couple of branches before the indirect branch, and we try to similarly get some computation going before them
     // Save the stackpointer in case there are calls to parse in the names we execute
     jt->parserstackframe.parserstkend1=stack;
     // Fill in the token# (in case of error) based on the line# we are running
     if(pmask&0x1F){
      I fsflag=FAVV(fs)->flag;  // fetch flags early - we always need them in lines 0-2
      pmask=LOWESTBIT(pmask);   // leave only one bit
      // Here for lines 0-4, which execute the entity pointed to by fs
      // We will be making a bivalent call to the action routine; it will be w,fs,fs for monads and a,w,fs for dyads (with appropriate changes for modifiers).  Fetch those arguments
      // We have fs already.  arg1 will come from position 2 3 1 1 1 depending on stack line; arg2 will come from 1 2 3 2 3
      if(pmask&7){  // lines 0 1 2, verb execution
       // Verb execution (in order: V N, V V N, N V N).  We must support inplacing, including assignment in place, and support recursion
       // Most of the executed fragements are executed right here.  In two cases we can be sure that the stack does not need to be rescanned:
       // 1. pline=2, token 0 is AVN: we have just put a noun in the first position, and if that produced an executable it would have been executed earlier.
       // 2. pline=0 or 2, token 0 not LPAR (might be EDGE): similarly can't execute with noun now in slot 1 (if LPAR and line 0/2, the only possible exec is () )
       // Since if pline is 0 token 0 must be EDGE, this is equivalent to pline!=1 and word 0 not LPAR
       // we save a pass through the matcher in those cases.  The 8 cycles are worth saving, but more than that it makes the branch prediction tighter
       // further, if word 0 is (C)AVN, we can pull 2 tokens if the next token is AVN: we have a flag for that
       pt0ecam|=pmask<<PLINESAVEX;  // lose pline over the subroutine calls to try to prevent a register spill
       pt0ecam|=fsflag&VJTFLGOK1+VJTFLGOK2+VASGSAFE;  // insert flags into portmanteau reg.  This ties up the reg while flags settle, but it's mostly used for predictions
       // If it is an inplaceable assignment to a known name that has a value, remember the name and the value
       // We handle =: N V N, =: V N, =: V V N.  In the last case both Vs must be ASGSAFE.  When we set jt->zombie we are warranting
       // that the next assignment will be to the name, and that the reassigned value is available for inplacing.  In the V V N case,
       // this may be over two verbs
       jt->parserstackframe.sf=fs;  // set new recursion point for $:
       if(((UI)(fsflag&(VJTFLGOK1<<(pmask>>2)))>(UI)PTISNOTASGNNAME(GETSTACK0PT)))if(likely(!(pt0ecam&NOTFINALEXEC))){A zval;   // inplaceable assignment to name; nothing in the stack to the right of what we are about to execute; well-behaved function (doesn't change locales)
        // We have many fetches to do and they will delay the execution of the code in this block.  We will rejoin the non-assignment block with a large slug of
        // instructions that have to wait.  Probably the frontend will still be emitting blocked instructions even after all the unblocked ones have been executed.  Pity.
         // Don't remember the assignand if it may change during execution, i. e. if the verb is unsafe.  For line 1 we have to look at BOTH verbs that come after the assignment
        if(pt0ecam&VASGSAFE&&(!(pt0ecam&(2<<PLINESAVEX))||FAVV(QCWORD(stack[1].a))->flag&VASGSAFE)){  // if executing line 1, make sure stack[1] is also ASGSAFE
         if(likely(GETSTACK0PT&PTASGNLOCAL)){L *s;
          // local assignment.  First check for primary symbol.  We expect this to succeed
          if(likely((s=(L*)(I)(NAV(QCWORD(*(volatile A*)queue))->symx&~REPSGN4(SGNIF4(pt0ecam,LOCSYMFLGX+ARLCLONEDX))))!=0)){
           zval=(SYMORIGIN+(I)s)->val;  // get value of symbol in primary table.  There may be no value; that's OK
          }else{zval=QCWORD(jtprobelocal(jt,QCWORD(*(volatile A*)queue),jt->locsyms));}
         }else zval=QCWORD(probequiet(QCWORD(*(volatile A*)queue)));  // global assignment, get slot address
         // to save time in the verbs (which execute more often than this assignment-parse), see if the assignment target is suitable for inplacing.  Set zombieval to point to the value if so
         // We require flags indicate not read-only, and usecount==2 (or 3 if NJA block) since we have raised the count of this block already if it is to be operated on inplace.
         // The block can be virtual, if it is x/y to xdefn.  We must never inplace to a virtual block
         zval=zval?zval:AFLAG0; zval=AC(zval)==(REPSGN((AFLAG(zval)&(AFRO|AFVIRTUAL))-1)&(((AFLAG(zval)>>AFNJAX)&(AFNJA>>AFNJAX))+ACUC2))?zval:0; jt->zombieval=zval;  // compiler should generate BT+ADC
         pmask=(pt0ecam>>PLINESAVEX)&7;  // restore after calls
        }
       }
       // There is no need to set the token number in the result, since it must be a noun and will never be executed
       // Close up the stack.  For lines 0&2 we don't need two writes, so they are duplicates
       // Get the branch-to address.  It comes from the appropriate valence of the appropriate stack element.  Stack element is 2 except for line 0; valence is monadic for lines 0 1 4
       AF actionfn=FAVV(fs)->valencefns[pmask>>2];  // the routine we will execute.  We have to wait till after the register pressure or the routine address will be written to memory
       A arg2=stack[(pmask>>1)+1].a;   // 2nd arg, fs or right dyad  1 2 3 (2 3)
       A arg1=stack[(pmask&3)+1].a;   // 1st arg, monad or left dyad  2 3 1 (1 1)     0 1 2 -> 1 2 3 + 1 1 2 -> 2 3 5 -> 2 3 1
       // (1) When the args return from the verb, we will check to see if any were inplaceable and unused.  But there is a problem:
       // the arg may be freed by the verb (if it is inplaceable and gets replaced by a virtual reference).  In this case we can't
       // rely on *arg[12].  But if the value is inplaceable, the one thing we CAN count on is that it has a tpop slot.  So we will save
       // the address of the tpop slot IF the arg is inplaceable now.  Then after execution we will pick up again, knowing to quit if the tpop slot
       // has been zapped.  We keep pointers for a/w rather than 1/2 for branch-prediction purposes
       // (2) If either arg is FAOWED, it can't be inplaceable & we use tpop[aw] to hold the FAOWED arg, flagged as FAOWED.  We must not duplicate an arg in this case
       // This calculation should run to completion while the expected misprediction is being processed
       A *tpopw=AZAPLOC(QCWORD(arg2)); tpopw=(A*)((I)tpopw&REPSGN(AC(QCWORD(arg2))&((AFLAG(QCWORD(arg2))&(AFVIRTUAL|AFUNINCORPABLE))-1))); tpopw=tpopw?tpopw:ZAPLOC0; tpopw=ISSTKFAOWED((I)arg2&(pmask>>2))?(A*)arg2:tpopw;
              // point to pointer to arg2 (if it is inplace) - only if dyad
       A *tpopa=AZAPLOC(QCWORD(arg1)); tpopa=(A*)((I)tpopa&REPSGN(AC(QCWORD(arg1))&((AFLAG(QCWORD(arg1))&(AFVIRTUAL|AFUNINCORPABLE))-1))); tpopa=tpopa?tpopa:ZAPLOC0; tpopa=ISSTKFAOWED(arg1)?(A*)arg1:tpopa;
              // tpopa/tpopw are:  monad: w fs  dyad: a w
        // tpopw may point to fs, but who cares?  If it's zappable, best to zap it now
       y=(*actionfn)((J)((I)jt+(REPSGN(SGNIF(pt0ecam,VJTFLGOK1X+(pmask>>2)))&((pmask>>1)|1))),QCWORD(arg1),QCWORD(arg2),fs);   // set bit 0, and bit 1 if dyadic, if inplacing allowed by the verb
         // could use jt->parserstackframe.sf to free fs earlier; we are about to break the pipeline.  But when we don't break we lose time waiting for jt->fs to settle
       // expect pipeline break
RECURSIVERESULTSCHECK
#if MEMAUDIT&0x10
       auditmemchains();  // trap here while we still point to the action routine
#endif
       FPZ(y);  // fail parse if error.  All FAOWED names must stay on the stack until we know it is safe to delete them
#if AUDITEXECRESULTS
       auditblock(jt,y,1,1);
#endif
#if MEMAUDIT&0x2
       if(AC(y)==0 || (AC(y)<0 && AC(y)!=ACINPLACE+ACUC1))SEGFAULT; 
#endif
       ramkrecursv(y);  // force recursive y
       pmask=(pt0ecam>>PLINESAVEX)&7;  // restore after calls
       // Make sure the result is recursive.  We need this to guarantee that any named value that has been incorporated has its usecount increased,
       //  so that it is safe to remove its protection
       // (1) if any of args/fs is FAOWED, the value is now out of execution and can be fa()d.  BUT if the value is y, it remains in execution and we inherit the
       // FAOWED status into y (but only once per value).  This is mutually exclusive with
       // (2) free up inputs that are no longer used.  These will be inputs that are still inplaceable and were not themselves returned by the execution.
       // We free them right here, and zap their tpop entry to avoid an extra free later.
       // We free using fanapop, which recurs only on recursive blocks, because that's what the tpop we are replacing does
       // We can free all DIRECT blocks, and PRISTINE also.  We mustn't free non-PRISTINE boxes because the contents are at large
       // and might be freed while in use elsewhere.
       // We mustn't free VIRTUAL blocks because they have to be zapped differently.  When we work that out, we will free them here too
       // NOTE that AZAPLOC may be invalid now, if the block was raised and then lowered for a period.  But if the arg is now abandoned,
       // and it was abandoned on input, and it wasn't returned, it must be safe to zap it using the zaploc BEFORE the call
       // first the w arg
       if(ISSTKFAOWED(tpopw)){arg1=QCWORD(tpopw); tpopw=tpopa; if(unlikely(arg1==y))y=SETSTKFAOWED(y);else fa(arg1);}
       else if(arg1=*tpopw){  // if the arg has a place on the stack, look at it to see if the block is still around
        I c=(UI)AC(arg1)>>(arg1==y);  // get inplaceability; set off if the arg is the result
        if((c&(-(AT(arg1)&DIRECT)|SGNIF(AFLAG(arg1),AFPRISTINEX)))<0){   // inplaceable and not return value.  Sparse blocks are never inplaceable
         *tpopw=0; tpopw=tpopa; fanapop(arg1,AFLAG(arg1));  // zap the top block; if recursive, fa the contents.  We free tpopa before subroutine
        }else tpopw=tpopa;
       }else tpopw=tpopa;
       // repeat for a if any
       if(ISSTKFAOWED(tpopw)){arg1=QCWORD(tpopw); if(unlikely(arg1==y))y=SETSTKFAOWED(y);else fa(arg1);}
       else if(arg1=*tpopw){  // if arg1==arg2 this will never load a value requiring action
        I c=(UI)AC(arg1)>>(arg1==y);   // can remove y here, see above
        if((c&(-(AT(arg1)&DIRECT)|SGNIF(AFLAG(arg1),AFPRISTINEX)))<0){  // inplaceable, not return value, not same as arg1, dyad.  Safe to check AC even if freed as arg1
         *tpopw=0; fanapop(arg1,AFLAG(arg1));
        }
       }
       // repeat for fs
       if(ISSTKFAOWED(arg1=stack[2-(pmask&1)].a)){fa(QCWORD(arg1));}   // 1 2 2
       // close up the stack and store the result
       stack[3-(pmask&1)].a=y;  // save result 2 3 2; parsetype is unchanged, token# is immaterial
       stack[((pmask&3)>>1)+1]=stack[pmask>>1];    // overwrite the verb with the previous cell - 0->1  1->2  2->1(NOP)  need 0->1     1->2 0->1    0->2  after relo  -1->0    0->1 -1->0   -2->0
       stack[pmask>>1]=stack[0];  // close up the stack  0->0(NOP)  0->1   0->2
       stack+=(pmask>>2)+1;   // finish relocating stack   1 1 2 (1 2)
       y=NEXTY;  // refetch next-word to save regs

       // Handle early exits: AVN on line (0)12 or (not LPAR on line 02 and finalexec).
       // If line 02 and the current word is (C)AVN and the next is also, stack 2
       // the likelys on the next 2 lines are required to get the compiler to avoid spilling queue or nextat
       if(likely((GETSTACK0PT&PTNOTLPAR)!=0)){
        if(likely(STACK0PTISCAVN>=(pt0ecam&NOTFINALEXEC+(1LL<<(PLINESAVEX+1))))){   // test is AVN or (NOTFINAL and pmask[1] both 0)
         // not ( and (AVN or !line1 & finalexec)): OK to skip the executable check
         pt0ecam&=(((GETSTACK0PT<<(CONJX-PTISCAVNX))&~(pt0ecam<<(CONJX-(PLINESAVEX+1))))|~CONJ);  // Optionally stack one more.  CONJ comes in as (next is CAVN).  Stack 2 if also (curr is CAVN) and (line 02)
         break;  // Go stack.
        }
       }else{
        // if LPAR, the usual next thing is ( CAVN ) and we will catch that here, to avoid going through fragment search (questionable)
        if(PTISRPAR0(stack[2].pt)==0){stack[2]=stack[1]; stack[2].t=stack[0].t; SETSTACK0PT(PTNOUN); stack+=2;}  // ( CAVN ).  Handle it
       }
       // If EDGE on line 1, we must rescan for EDGE V V N
      }else{
       // Lines 3-4, adv/conj execution.  We must get the parsing type of the result, but we don't need to worry about inplacing or recursion
       pmask>>=3; // 1 for adj, 2 for conj
       AF actionfn=FAVV(fs)->valencefns[pmask-1];  // the routine we will execute.  It's going to take longer to read this than we can fill before the branch is mispredicted, usually
       A arg1=stack[1].a;   // 1st arg, monad or left dyad
       A arg2=stack[pmask+1].a;   // 2nd arg, fs or right dyad
       A arg3=stack[2].a; arg3=pmask&2?arg3:0;  // fs, if this is a conjunction, for FAOWED testing
       UI4 restok=stack[1].t;  // save token # to use for result
       // We set the MODIFIER flag in the call so that jtxdefn/unquote can know that they are modifiers
       A yy=(*actionfn)((J)((I)jt|JTXDEFMODIFIER),QCWORD(arg1),QCWORD(arg2),fs);
       y=NEXTY;  // refetch next-word to save regs
RECURSIVERESULTSCHECK
#if MEMAUDIT&0x10
       auditmemchains();  // trap here while we still point to the action routine
#endif
       FPZ(yy);    // fail parse if error.  All FAOWED names must stay on the stack until we know it is safe to delete them
#if AUDITEXECRESULTS
       auditblock(jt,yy,1,1);
#endif
#if MEMAUDIT&0x2
       if(AC(yy)==0 || (AC(yy)<0 && AC(yy)!=ACINPLACE+ACUC1))SEGFAULT; 
       audittstack(jt);
#endif
       // Make sure the result is recursive.  We need this to guarantee that any named value that has been incorporated has its usecount increased,
       //  so that it is safe to remove its protection
       ramkrecursv(yy);  // force recursive y
       if(ISSTKFAOWED(arg1=stack[1].a)){arg1=QCWORD(arg1);if(unlikely(arg1==yy))yy=SETSTKFAOWED(yy);else fa(arg1);}
       if(ISSTKFAOWED(arg1=stack[pmask+1].a)){arg1=QCWORD(arg1);if(unlikely(arg1==yy))yy=SETSTKFAOWED(yy);else fa(arg1);}
       if(ISSTKFAOWED(arg3)){arg1=QCWORD(arg3);if(unlikely(arg1==yy))yy=SETSTKFAOWED(yy);else fa(arg1);}
       stack[pmask]=stack[0]; // close up the stack
       stack=stack+pmask;  // advance stackpointer to position before result 1 2
       PTFROMTYPE(stack[1].pt,AT(yy)) stack[1].t=restok; stack[1].a=yy;   // save result, move token#, recalc parsetype
      }
     }else{
      // Here for lines 5-7 (fork/hook/assign), which branch to a canned routine
      // It will run its function, and return the new stackpointer to use, with the stack all filled in.  If there is an error, the returned stackpointer will be 0.
      // We avoid the indirect branch, which is very expensive
      jt->parserstackframe.parsercurrtok = stack[1].t;   // 1 for hook/fork/assign
      if(pmask&0b10000000){  // assign - can't be fork/hook
       // Point to the block for the assignment; fetch the assignmenttype; choose the starting symbol table
       // depending on which type of assignment (but if there is no local symbol table, always use the global)
       A symtab=jt->locsyms; {A gsyms=jt->global; symtab=!EXPLICITRUNNING?gsyms:symtab; symtab=!(stack[1].pt&PTASGNLOCAL)?gsyms:symtab;}  // use global table if  =: used, or symbol table is the short one, meaning 'no symbols'
       I rc;
       if(likely(GETSTACK0PT&PTNAME0))rc=jtsymbis((J)((I)jt|(((US)pt0ecam==0)<<JTFINALASGNX)),QCWORD(stack[0].a),QCWORD(stack[2].a),symtab);   // Assign to the known name.  If ASSIGNSYM is set, PTNAME0 must also be set
       else rc=jtis((J)((I)jt|(((US)pt0ecam==0)<<JTFINALASGNX)),QCWORD(stack[0].a),QCWORD(stack[2].a),symtab);
#if MEMAUDIT&0x10
       auditmemchains();
#endif
       CLEARZOMBIE   // in case assignsym was set, clear it until next use
       // it impossible for the stack to be executable.  If there are no more words, the sentence is finished.
       FPZ(rc)  // fail if error
       // If FAOWED was in the value, the result needs to inherit it.  But since we retain the same stack position as the result of the assignment, nothing more is needed.
       if(likely((US)pt0ecam==0))EP(1)  // In the normal sentence name =: ..., we are done after the assignment.  Ending stack must be  (x x result) normally (x MARK result), i. e. leave stackptr unchanged
       stack+=2;  // if we have to keep going, advance stack to the assigned value
       // here we are dealing with the uncommon case of non-final assignment.
       // the newly-assigned name might have been ra()d, if it couldn't be zapped.  If so, indicate that fact in the stacked address

       // If the next word is not LPAR, we can fetch another word after.
       // if the 2d-next word exists, and it is (C)AVN, and the current top-of-stack is not ADV, and the next word is not ASGN, we can pull a third word.  (Only ADV can become executable in stack[2]
       // if it was not executable next to ASGN).  We go to the trouble (1) because the case is the usual one and we are saving a little time; (2) by eliminating
       // failing calls to the parser we strengthen its branch prediction
       // At this point pt0ecam&CONJ is set to (nextat is CAVN).  For comp ease we use this instead of checking for LPAR and ASGN.  This means that for a =: b =: c we will miss after assigning b.
       y=NEXTY;  // refetch next-word to save regs
       if(likely((pt0ecam&(1LL-(I)(US)pt0ecam)&CONJ)!=0)){pt0ecam|=-(AT(QCWORD(queue[-1]))&ADV+VERB+NOUN+NAME)&~(AT(stack[0].a)<<(CONJX+1-ADVX))&(CONJ<<1);}  // we start with CONJ set to 'next is CAVN'
       break;  // go pull the next word(s)
      }else{
       if(pmask&0b100000){
        A arg1=stack[1].a, arg2=stack[2].a, arg3=stack[3].a;
        A yy=folk(QCWORD(arg1),QCWORD(arg2),QCWORD(arg3));  // create the fork
        // Make sure the result is recursive.  We need this to guarantee that any named value that has been incorporated has its usecount increased,
        //  so that it is safe to remove its protection
        FPZ(yy);    // fail parse if error.  All FAOWED names must stay on the stack until we know it is safe to delete them
        RECURSIVERESULTSCHECK
        ramkrecursv(yy);  // force recursive y
        y=NEXTY;  // refetch next-word to save regs
        if(ISSTKFAOWED(arg1=stack[1].a)){arg1=QCWORD(arg1);if(unlikely(arg1==yy))yy=SETSTKFAOWED(yy);else faacv(arg1);}
        if(ISSTKFAOWED(arg1=stack[2].a)){arg1=QCWORD(arg1);if(unlikely(arg1==yy))yy=SETSTKFAOWED(yy);else faacv(arg1);}
        if(ISSTKFAOWED(arg1=stack[3].a)){arg1=QCWORD(arg1);if(unlikely(arg1==yy))yy=SETSTKFAOWED(yy);else faacv(arg1);}
        stack[3].t = stack[1].t; stack[3].a = yy;  // take err tok from f; save result; no need to set parsertype, since it didn't change
        stack[2]=stack[0]; stack+=2;  // close up stack
       }else{
        // hook and non-fork tridents
        A arg1=stack[1].a, arg2=stack[2].a, arg3=stack[3].a;
        I trident=PTISCAVN(stack[3].pt)?3:2; arg3=PTISCAVN(stack[3].pt)?arg3:mark;  // beginning of stack after execution; a is invalid in end-of-stack
        A yy=hook(QCWORD(arg1),QCWORD(arg2),QCWORD(arg3));  // create the hook
        // Make sure the result is recursive.  We need this to guarantee that any named value that has been incorporated has its usecount increased,
        //  so that it is safe to remove its protection
        FPZ(yy);    // fail parse if error.  All FAOWED names must stay on the stack until we know it is safe to delete them
        RECURSIVERESULTSCHECK
        ramkrecursv(yy);  // force recursive y
        y=NEXTY;  // refetch next-word to save regs
        if(ISSTKFAOWED(arg1=stack[1].a)){arg1=QCWORD(arg1);if(unlikely(arg1==yy))yy=SETSTKFAOWED(yy);else fa(arg1);}
        if(ISSTKFAOWED(arg1=stack[2].a)){arg1=QCWORD(arg1);if(unlikely(arg1==yy))yy=SETSTKFAOWED(yy);else fa(arg1);}
        if(ISSTKFAOWED(arg3)){arg1=QCWORD(arg3);if(unlikely(arg1==yy))yy=SETSTKFAOWED(yy);else fa(arg1);}
        PTFROMTYPE(stack[trident].pt,AT(yy)) stack[trident].t = stack[trident-1].t; stack[trident].a = yy;  // take err tok from f of hook, g of trident; save result.  Must store new type because this line takes adverb hooks also
        stack[trident-1]=stack[0]; stack+=trident-1;  // close up stack
       }
      }
#if MEMAUDIT&0x10
      auditmemchains();  // trap here while we still have the parseline
#endif
#if AUDITEXECRESULTS
      if(pline<=6)auditblock(jt,stack[1].a,1,1);  // () and asgn have already been audited
#endif
#if MEMAUDIT&0x2
      if((US)pt0ecam!=0 && (AC(QCWORD(stack[0].a))==0 || (AC(QCWORD(stack[0].a))<0 && AC(QCWORD(stack[0].a))!=ACINPLACE+ACUC1)))SEGFAULT; 
      audittstack(jt);
#endif
     }  // end of classifying fragment
    // the compiler doesn't handle the combination of likely and break.  If we don't put something here, the fail-parse branch will go backwards
    // and will predict that way, which is wrong.
    }else{
     // LPAR misses the main parse table, which is just as well because it would miss later branches anyway.  We pick it up here so as not to add
     // a couple of cycles to the main parse test.  Whether we stack or execute, y is still set with the next word+type
     if(!(GETSTACK0PT&PTNOTLPAR)){  // ( with no other line.  Better be ( CAVN )
      if(likely(PTISCAVN(~stack[1].pt)==PTISRPAR0(stack[2].pt))){  // must be [1]=CAVN and [2]=RPAR.  To be equal, !CAVN and RPAR-if-0 must both be 0 
       SETSTACK0PT(stack[1].pt); stack[2]=stack[1]; stack[2].t=stack[0].t;  //  Install result over ).  Use value/type from expr, token # from (   Bottom of stack was modified, so refresh the type for it
       stack+=2;  // advance stack pointer to result
      }else{jt->parserstackframe.parsercurrtok = stack[1].t; jsignal(EVSYNTAX); FP}  // error if contents of ( not valid
      // we fall through to rescan after ( )
     }else{pt0ecam&=~CONJ;  break;}   // parse failed, return to stack next word.  Must clear 'stack 2' flag
    }  // end 'there was a fragment'
   } // end of loop executing fragments
   
  }  // break with stack==0 on error to failparse; main exit is when queue is empty (m<0), to exitparse

 exitparse:
  // *** pt0ecam has been repurposed to 1 for final assignment, 0 0therwise
#if MEMAUDIT&0x2
  audittstack(jt);
#endif
   // Prepare the result
  if(1){  // no error on this branch
   // before we exited, we backed the stack to before the initial mark entry.  At this point stack[0] is invalid,
   // stack[1] is the initial mark (not written out), stack[2] is the result, and stack[3] had better be the first ending mark
   if(unlikely(!PTISCAVN(stack[2].pt))){jt->parserstackframe.parsercurrtok = 0; jsignal(EVSYNTAX); FP}
   if(unlikely(&stack[3]!=stackend1)){jt->parserstackframe.parsercurrtok = 0; jsignal(EVSYNTAX); FP}
   z=stack[2].a;   // stack[0..1] are the mark; this is the sentence result, if there is no error.  STKFAOWED semantics
   // normal end, but we have to handle the case where the result has FAOWED.   (ex: ([ 4!:55@(<'x')) x).  The fa must be deferred till after the result has been used.  We turn it into a tpush
   if(unlikely(ISSTKFAOWED(z))){tpushna(QCWORD(z));}  // if the result needs a free, do it via tpush
  }else{
failparse:  // If there was an error during execution or name-stacking, exit with failure.  Error has already been signaled.  Remove zombiesym.  Repurpose pt0ecam
   // if m=0, the stack contains a virtual mark.  We must step over that so we don't free the garbage mask
   stack+=((US)pt0ecam==0); CLEARZOMBIE z=0; pt0ecam=0;  // indicate not final assignment
   // fa() any blocks left on the stack that have FAOWED
   for(;stack!=stackend1;++stack)if(ISSTKFAOWED(stack->a)){fa(QCWORD(stack->a))};  // issue deferred fa for items ra()d and not finished
  }
#if MEMAUDIT&0x2
  audittstack(jt);
#endif

  // Still can't return till frame-stack popped
  jt->parserstackframe = oframe;
#if MEMAUDIT&0x2
  audittstack(jt);
#endif
#if MEMAUDIT&0x20
     auditmemchains();  // trap here while we still have the parseline
#endif

  // NOW it is OK to return.  Insert the final-assignment bit (pt0ecam) into the return
  R (A)((I)QCWORD(z)+pt0ecam);  // this is the return point from normal parsing

 }else{A y;A sv=0;  // m<2.  Happens fairly often, and full parse can be omitted
  if(likely(nwds==1)){  // exit fast if empty input.  Happens only during load, but we can't deal with it
   // Only 1 word in the queue.  No need to parse - just evaluate & return.  We do it here to avoid parsing
   // overhead, because it happens enough to notice.
   // No ASSERT - must get to the end to pop stack
   jt->parserstackframe.parsercurrtok=0;  // error token if error found
   I at=AT(y = QCWORD(queue[0]));  // fetch the word
   if((at&NAME)!=0) {
    if(likely((((I)NAV(y)->symx-1)|SGNIF(AR(jt->locsyms),ARLCLONEDX))>=0)){  // if we are using primary table and there is a symbol stored there...
     L *s=SYMORIGIN+(I)NAV(y)->symx;  // get address of symbol in primary table
     if(likely((sv=s->val)!=0)){raposlocal(s->val); goto got1val;}  // if value has not been assigned, ignore it.  Could just treat as undef.  Must ra to match syrd.  sv has QCGLOBAL semantics
    }
    if(likely((sv=syrd(y,jt->locsyms))!=0)){     // Resolve the name and ra() it
got1val:;
     // sv has QCGLOBAL semantics
     if(likely(((AT(QCWORD(sv))|at)&(NOUN|NAMEBYVALUE))!=0)){   // if noun or special name, use value
      if(unlikely(at&NAMEABANDON)){
       sv=namecoco(jtinplace, y, sv);  // if name::, go delete the name, leaving the value to be deleted later.  sv has QCFAOWED semantics
       y=QCWORD(sv); sv=(A)ISFAOWED(sv);  // coco will set FAOWED if it didn't fa() the value; transfer that to sv
      }else y=QCWORD(sv);  // not name::, just use the value
     } else {y=QCWORD(namerefacv(y, sv)); sv=0;}   // Replace other acv with reference.  Could fail.  Undo the ra from syrd
    } else {
     // undefined name.
     if(at&NAMEBYVALUE){jsignal(EVVALUE); y=0;}  // Error if the unresolved name is x y etc.  Don't ASSERT since we must pop stack
     else y = QCWORD(namerefacv(y, 0));    // this will create a ref to undefined name as verb [: .  Could set y to 0 if error
    }
   }
   if(likely(y!=0))if(unlikely(!(AT(y)&CAVN))){jsignal(EVSYNTAX); y=0;}  // if not CAVN result, error
   // If sv!=0, we found the value and ra()d it.  Match the ra with a tpush so that the value stays protected during further execution
   if(likely(sv!=0)){if(likely(y!=0)){tpush(y);}else fa(sv);}  // undo the ra() in syrd.  In case someone else deletes the value, protect it on the tpop stack till it can be displayed
  }else y=mark;  // empty input - return with 'mark' as the value, which means nothing to parse.  This result must not be passed into a sentence
  jt->parserstackframe = oframe;
  R y;  // indicate not a final assignment
 }

}

