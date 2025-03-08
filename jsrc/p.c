/* Copyright (c) 1990-2025, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: parsing                                                          */

// Parsing follows the description in Dictionary Chapter E. Parsing
// with the following implementation details:
// 1. words must have gone through enqueue, which puts type information into the low 6 bits of each pointer with QCTYPE flags
// 2. the leading mark is not written before the sentence, but is implied
// 3. During each symbol lookup the value read (which has QCNAMELKP semantics) is ra()d iff global and the type flags are converted to QCFAOWED semantics.
// 4. the stack contains a pointer to the word (converted to STKNAMED semantics), the token number in the original sentence that will
//     be blamed in case of error, and a binary mask indicating which parsing lines the word is recognized in, in
//     each of the 4 stack popsitions.  The 'search of the parse table' is done by ANDing 4 bytes together.
// 5. all executions from parsing lines 0-2 are inplaceable
// 6. if the result of an execution in lines 0-2 is assigned to a name, jt->zombieval is set to point to the value if that value can be modified in place
// 7. after execution in lines 0-2, abandoned arguments that are still abandoned are freed immediately
// 8. After an execution, any stacked value that came from a symbol lookup is fa()d
// 9. forks are processed through line 5.  All other bidents/tridents are processed through line 6, which calls jthook to do the work
// 10. minimal error information is preserved.  When an error occurs, jtjsignal calls back to infererrtok to figure out where the error happened
// 11. end-of-parse is often detected early, especially after an assignment
// 12. In certain cases the parse knows it can stack 2 or even 3 words before checking for an executable fragment
//
// There is a separate path for one-word sentences.  If you add any feature, check to see if you have to add it to the one-word path also.
//

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

#define AVN   (     ADV+VERB+NOUN)
#define CAVN  (CONJ+ADV+VERB+NOUN)
#define EDGE  (MARK+ASGN+LPAR)

// the parsing table, used by the tacit translator only
PT cases[] = {
 EDGE,      VERB,      NOUN, ANY,       0,  jtvmonad, 1,2,1,  // 0
 EDGE+AVN,  VERB,      VERB, NOUN,      0,  jtvmonad, 2,3,2,  // 1
 EDGE+AVN,  NOUN,      VERB, NOUN,      0,  jtvdyad,  1,3,2,  // 2
 EDGE+AVN,  VERB+NOUN, ADV,  ANY,       0,  jtvadv,   1,2,1,  // 3
 EDGE+AVN,  VERB+NOUN, CONJ, VERB+NOUN, 0,  jtvconj,  1,3,1,  // 4
 EDGE+AVN,  VERB+NOUN, VERB, VERB,      0,  jtvfolk,  1,3,1,  // 5
 EDGE,      CAVN,      CAVN, ANY,       0,  jtvhook,  1,2,1,  // 6
 NAME+NOUN, ASGN,      CAVN, ANY,       0,  jtvis,    0,2,1,  // 7
 LPAR,      CAVN,      RPAR, ANY,       0,  jtvpunc,  0,2,0,  // 8
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
// The AND of these gives the matched row (the end-of-table row is always matched)
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
// We distinguish local and global assignments with a flag; the local version enables line 6 (hook) erroneously, which must be rejected in jthook
// in all these bit 24 must be set in anything that isn't a name, and bit 27 in anything but LPAR
#define PTNOUN 0xDFC17CBE
#define PTMARKFRONT 0x0900007F  // matches only pos 0 - bit 24 set to not look like assign to name, 27 for not LPAR
#define PTMARKBACK (UI4)0xC9000040  // matches only pos 3, but with a flag set to not match PTISRPAR0 - in pos 0, which this can never be in
#define PTVERB 0xF9E67B3E
#define PTLPAR 0x0100007F  // flagged to be not a name
#define PTRPAR (UI4)0xC9000000
// table for translating AT type to QC type flags
static const __attribute__((aligned(CACHELINESIZE))) UI4 ptcol[16] = {
[LASTNOUNX-LASTNOUNX] = PTNOUN,  // PN
[NAMEX-LASTNOUNX] = 0xC9200080,  // PNM assigned: high 16 bits immaterial because this MUST match line 7 which will delete it
[MARKX-LASTNOUNX] = 0x0,  // PM cannot occur in a sentence
[ADVX-LASTNOUNX] = 0xC9C8403E,  // PA
// gap[ASGNX-LASTNOUNX]   must be left open in enqueue() tokens to avoid messing up the pull queue
[VALTYPENAMELESS-1] = 0xC9C8403E, // gap [SYMBX-LASTNOUNX]  only in syrd() results, i. e. from symbol
[VALTYPESPARSE-1] = PTNOUN,  // gap [CONWX-LASTNOUNX]  only in syrd() results, i. e. from symbol
[VERBX-LASTNOUNX] = PTVERB,  // PV
[LPARX-LASTNOUNX] = PTLPAR,  // PL
[CONJX-LASTNOUNX] = 0xC9D04000,  // PC
[RPARX-LASTNOUNX] = PTRPAR,  // PR
// the LOCAL and ASGNTONAME flags are passed into PT.  They can cause line 6 to be matched when stack[2] is ASGN 
[QCASGN-1] = 0xC900807F,
[QCASGN+QCASGNISLOCAL-1] = 0xC940807F,  // this has PTASGNLOCAL set
[QCASGN+QCASGNISTONAME-1] = 0xC800807F,
[QCASGN+QCASGNISLOCAL+QCASGNISTONAME-1] = 0xC840807F  // this has PTASGNLOCAL set
};

// tests for pt types
#define PTISCAVNX 23  // this flag used in a register here
#define PTISCAVN(pt) ((pt)&(1LL<<PTISCAVNX))
#define PTISRPAR0(pt) ((pt)&0x7fff)
// clang17 wastes an instruction #define PTISMARKBACKORRPAR(s)  (((s).pt>>16)==(PTRPAR>>16))  // s.pt is ) or MARK.
#define PTISMARKBACKORRPAR(s)  ((((US*)&(s).pt)[1])==(PTRPAR>>16))  // s.pt is ) or MARK.
_Static_assert((PTRPAR^PTMARKBACK&0xffff0000)==0,"MARKBACK must equal RPAR for end purposes");
#define PTISMARKFRONT(pt)  (((pt)&0xff000000)==(PTMARKFRONT&0xff000000))  // pt is MARKFRONT
#define PTISNOTASGNNAME(pt)  ((pt)&0x1000000)  // bit must be >= VJTFLGOK2X because we compare them > to find valid named assignments
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
static DF2(jtisf){F12IP;A am, mw; RZ(am=onm(a)); RZ(mw=CALL1(FAV(self)->valencefns[0],w,self)); RZ(symbisdel(am,mw,ABACK(self))); R num(0);} 

// assignment other than name =[.:], single or multiple
// jt has flag set for final assignment (passed into symbis)
// The return must be 0 for bad, otherwise good with bit 0=final assignment, bit 1 = local assignment (never set)
static I NOINLINE jtis(J jtinplace,A n,A v,A symtab){F12IP;
 B ger=0;C *s;
 if(unlikely(AT(n)==BOX+BOXMULTIASSIGN)){   // test both bits, since BOXMULTIASSIGN has multiple uses
  // string assignment, where the NAME blocks have already been computed.  Use them.  The fast case is where we are assigning a boxed list
  if(AN(n)==1)n=AAV(n)[0];  // if there is only 1 name, treat this like simple assignment to first box, fall through
  else{
   // multiple assignment to fixed names
   ASSERT((-(AR(v))&(-(AN(n)^AS(v)[0])))>=0,EVLENGTH);   // v is atom, or length matches n
   if(((AR(v)^1)+(~AT(v)&BOX))==0){A *nv=AAV(n), *vv=AAV(v); DO(AN(n), jtsymbis(jtinplace,nv[i],C(vv[i]),symtab);)}  // v is boxed list
   else {A *nv=AAV(n); DO(AN(n), A  vval; RZ(vval=ope(AR(v)?from(sc(i),v):v)); jtsymbis((J)((I)jtinplace|JTFINALASGN),nv[i],vval,symtab);)}  // repeat atomic v for each name, otherwise select item.  Open in either case; always final assignment since value is not passed on
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
  // computed name(s), now boxed character strings
  ASSERT(AN(n)||(AR(v)&&!AS(v)[0]),EVILNAME);  // error if namelist empty or multiple assignment to no names, if there is something to be assigned
  // otherwise, if it's an assignment to an atomic computed name, convert the string to a name and do the single assignment
  if(!AR(n)){A nnm; RZ(nnm=onm(n)); jtsymbis(jtinplace,nnm,v,symtab);  // just one name
  }else{
   // otherwise it's multiple assignment (could have just 1 name to assign, if it is AR assignment).
   // Verify rank 1.  For each lhs-rhs pair, do the assignment (in jtisf).
   // if it is AR assignment, apply jtfxx to each assignand, to convert AR to internal form
   // if not AR assignment, just open each box of rhs and assign
   ASSERT(1==AR(n),EVRANK); ASSERT(AT(v)&NOUN,EVDOMAIN);
   // create faux fs to pass args to the multiple-assignment function, in AM and valencefns.  AT must be 0 for eformat, too
   PRIM asgfs; ABACK((A)&asgfs)=symtab; AT((A)&asgfs)=0; FAV((A)&asgfs)->flag2=0; FAV((A)&asgfs)->valencefns[0]=ger?jtfxx:jtope;   // pass in the symtab to assign, and whether w must be converted from AR.  flag2 must be 0 to satisfy rank2ex
   I rr=AR(v)-((UI)AR(v)>0); rank2ex(n,v,(A)&asgfs,0,rr,0,rr,jtisf); 
   if(unlikely(jt->jerr!=0)){
    // If the assignment failed, try to explain why, since there is nowhere else to do so.  jtis may have formatted a message already (for invalid =:); if so, it will survive.  Otherwise
    // we infer an agreement error from EVLENGTH, or an error in opening from EVDOMAIN
    char* emsg=0; I msglen;
    emsg=jt->jerr==EVLENGTH?"number of assigned names does not match number of values":emsg;  // infer agreement error from EVLENGTH
    emsg=(((jt->jerr==EVDOMAIN)|(jt->jerr==EVSPELL)|(jt->jerr==EVINHOMO)))&ger?"error evaluating atomic representation for assignment":emsg;   // if error evaluating gerund
    emsg=(jt->jerr==EVINHOMO)&!ger?"mixed types encountered while opening an item for assignment":emsg;  // otherwise must be domain error on open
    emsg=(jt->jerr==EVDOMAIN)&!ger?"error opening an item for assignment":emsg;  // otherwise must be domain error on open
    if(emsg)jteformat(jt,0,str(strlen(emsg),emsg),0,0);
   }
  }
 }
retstack:  // return, but 0 if error
 R unlikely(jt->jerr!=0)?0:(I)jtinplace;  // return 0 if error, otherwise a nonzero with final assignment flag correct and local assignment clear
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
 case B01X: case INTX: case FLX: case CMPXX: case QPX: case LITX: case C2TX: case C4TX: case SBTX: case NAMEX: case SYMBX: case CONWX: case INT2X: case INT4X:  // direct forms, but possibly sparse
  if(ISSPARSE(AT(w))){P*v=PAV(w);  A x;
   if(!scheck(w))SEGFAULT;
   x = SPA(v,a); if(!(AT(x)&DIRECT))SEGFAULT; x = SPA(v,e); if(!((AT(x)&DIRECT)>0))SEGFAULT; x = SPA(v,i); if(!(AT(x)&DIRECT))SEGFAULT; x = SPA(v,x); if(!(AT(x)&DIRECT))SEGFAULT;
   auditblock(jt,SPA(v,a),nonrecur,0); auditblock(jt,SPA(v,e),nonrecur,0); auditblock(jt,SPA(v,i),nonrecur,0); auditblock(jt,SPA(v,x),nonrecur,0);
  }else if(NOUN & (AT(w) ^ (AT(w) & -AT(w))))SEGFAULT;
  break;
 case ASGNX: break;
 default: break; SEGFAULT;
}

#endif

#if 0  // for debugging
static SYMWALK(jtchkval0k, I,INT,1,1, AT(QCWORD(d->fval))&NOUN&&AK(QCWORD(d->fval))==0?SEGFAULT:0 , ;)
#endif

// Run parser, creating a new debug frame.  Explicit defs, which make other tests first, go through jtparsea except during debug/pm
// the result has bit 0 set if final assignment
// JT flags indicate whether call comes from ".
F1(jtparse){F12IP;A z;I stackallo=0;
 ARGCHK1(w);
 A *queue=AAV(w); I m=AN(w);   // addr and length of sentence
 // Get a new stack frame if needed.  This is a holdover from early days when a frame was allocated for every sentence.  Now that we can
 // parse a sentence in the time it takes to allocate a frame, that is unacceptable overhead.  We come through here for keyboard, script, and ". .
 // We need a DCPARSE frame to get error messages displayed, but we use the dcm field in it to indicate how many levels of ". are active
 if(unlikely(jt->uflags.trace&TRACEDB1)||unlikely(jt->sitop==0)||unlikely(jt->sitop->dctype!=DCPARSE)){RZ(deba(DCPARSE,queue,(A)m,0L)); stackallo=1;}
 I oexct=jt->sitop->dcm; jt->sitop->dcm+=((I)jtinplace>>JTFROMEXECX)&1;   // if this is "., raise the exec count in the PARSE frame
 z=jtparsea(jtinplace,queue,m);
 jt->sitop->dcm=oexct; if(unlikely(stackallo))debz();   // remove the exec count, pop stack if pushed
 // It is vital that we NOT issue EPILOG here, in case there are deleted values on the stack that need protection
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
#define jtfolk virtfolk
#define jthook virthook
#endif

// name_: delete the symbol name but not deleting the value.  The value has been ra()d.  Undo the pending fa: If usecount goes to 1, make it abandoned inplaceable and tpush
// Incoming y is the value attached to the symbol & has QCFAOWED semantics, result is same value with QCFAOWED semantics.
// result is the value, possibly with FAOWED set
static A nameundco(J jtinplace, A name, A y){F12IP;
 A locfound;
 if(unlikely(((I)y&QCFAOWED)!=0))locfound=syrdforlocale(name);  // get locale to use.  This re-looks up global names, but they should be rare in name_:
 else{locfound=jt->locsyms;  // if not FAOWED, it must be local, no lookup needed
 }
 if(((I)jtinplace&JTFROMEXEC))R SETFAOWED(y);   // in "., the result value from ". has not been protected by FAOWED, and might be prematurely freed.  So we don't free here, set FAOWED and return indicating indic that we need to fa
 WRITELOCK(locfound->lock)
 jtprobedel((J)((I)jt+NAV(name)->m),NAV(name)->s,NAV(name)->hash,locfound);  // delete the symbol (incl name and value) in the locale in which it is defined
 WRITEUNLOCK(locfound->lock)
 // The name has been deleted.  The value is still protected by the ra() made in syrd or before we were called.
 // convert here (notionally) to QCFAOWED semantics, with the knowledge that every block has had a ra() at this point.  We explicitly set FAOWED status here
 if(likely(!ISSPARSE(AT(QCWORD(y))))&&likely(AC(QCWORD(y))==ACUC1)){
  // The usecount has gone down to 1 (including the ra from syrd): the block can become inplaceable again.  We have to give it a valid zaploc in case anyone wants to use it.
  // In this case the tpush undoes the ra() from syrd and we do not set FAOWED.
  ACSET(QCWORD(y),ACINPLACE+ACUC1); AZAPLOC(QCWORD(y))=jt->tnextpushp; tpushna(QCWORD(y)); R CLRFAOWED(y);
 }
 R SETFAOWED(y);  // if we don't tpush, we need to tell the caller that fa is owed
}

#define FP goto failparse;   // indicate parse failure and exit
#define FPS goto failparsestack;  // indicate parse failure during stacking or (
#define FPE goto failparseeformat;  // call eformat after parse failure
#define EP(x) {pt0ecam=(x); goto exitparse;}   // exit parser with success; x = 1 if final assignment
#define FPZSUFF(x,suff) if(unlikely(!(x))){suff FP}   // exit parser w/failure if x==0, doing suff on the way out
#define FPSZSUFF(x,suff) if(unlikely(!(x))){suff FPS}   // exit parser w/failure if x==0, doing suff on the way out
#define FPZ(x) FPZSUFF(x,)   // exit parser w/failure if x==0
#define FPSZ(x) FPSZSUFF(x,)   // exit parser w/failure if x==0


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
//   the usecount is 2 (1 for the name, 1 for the ra() in the lookup)
//   the current execution is the only thing on the stack
//   the name can be resolved before the execution
//
// The first rule is to have no truck with inplacing unless the execution is known to be locative-safe,
// i. e. will not change locale, path, or any name that might go into a locative.  This is marked by a
// flag ASGSAFE in the verb.
//
// If jt->zombieval is set, the (necessarily inplaceable) verb may choose to perform an in-place
// operation.  It will check usecounts and addresses to decide whether to do this

#if 0 // for stats gathering
I statwfaowed=0, statwfainh=0, statwfafa=0, statwpop=0, statwpopfa=0, statwpopnull=0, statwnull=0, statwpoprecur=0;
I statafaowed=0, statafainh=0, statafafa=0, statapop=0, statapopfa=0, statapopnull=0, statanull=0, statapoprecur=0;
I statffaowed=0;
#define INCRSTAT(type) {__atomic_fetch_add(&stat##type,1,__ATOMIC_ACQUIRE);}
#else
#define INCRSTAT(x)
#endif

// Given that we took an error executing the block starting at jt->parserstackframe.parserstkend1, figure out what the error-token number(s) should be
// There may be 2 numbers: the location of a mismatched parenthesis and the location of the error itself.
I infererrtok(J jt){I errtok;
 jt->emsgstate&=~EMSGSTATEPAREN;  // set no mismatched () found   (cannot have one for pee, and not really needed for a single word)
 // if the sentence had only one token, that token is the error token.  This also picks up undefined names that are being signaled to console after the last sentence has finished.  The
 // stack will have popped to the initial value, which has 0 words: this could never fail on its own, so we say the failing token is 1.
 if(jt->parserstackframe.parserstkbgn[-1].t<=1)R 1;  // just one token, return it
 // if the error was pee or preexecution error, use the error set then
 if(jt->parserstackframe.parseroridetok!=0xffff){errtok=jt->parserstackframe.parseroridetok;  // Will always be 0 if not -1
 }else{
  // error detected during execution - reparse the fragment to get the line# that was executed
  I pmask=(I)((C*)&jt->parserstackframe.parserstkend1[0].pt)[0] & (I)((C*)&jt->parserstackframe.parserstkend1[1].pt)[1] &(I)((C*)&jt->parserstackframe.parserstkend1[2].pt)[2];  // emulate parse
  if(pmask!=0)pmask&=(I)((C*)&jt->parserstackframe.parserstkend1[3].pt)[3];  // if we fail when the stack is empty the only thing on the stack is 3 marks, so we'd better not fetch the fourth.  marks are not executable
  // see if ( was executable
  pmask|=jt->parserstackframe.parserstkend1[0].pt&PTNOTLPAR?0:0x100;  // ( any any any is line 8 - but it couldn't fail
  // Get the number that was executed.  Take the token# from entry (9-0): 2 1 1 1 1 2 2 2 2 1 (line 9 is no match, must be ending syntax error)
  errtok=jt->parserstackframe.parserstkend1[((0x21e>>CTTZI(pmask|0x200))&1)+1].t;  // get failing token#
 }
 // see if the sentence had unbalanced parens.
 I lastlwd, lastrwd, nesting=0;   // 1-origin wd# of leftwost unmatched ( and rightmost unmatched ) found so far, and nesting level
 A *wds=(A*)jt->parserstackframe.parserstkbgn[-1].a; I nwds=jt->parserstackframe.parserstkbgn[-1].t;  // address/count of the words of the sentence
 for(;nwds!=0;--nwds){if(nesting<0)break; if(QCSENTTYPE(wds[nwds-1])==LPARX-LASTNOUNX+1){lastlwd=nwds; --nesting;} if(QCSENTTYPE(wds[nwds-1])==RPARX-LASTNOUNX+1){if(nesting==0)lastrwd=nwds; ++nesting;}}  // count (); include LKPNAME in the test
 if(nesting!=0){   // if unbalanced parens...
  I ptok=(nesting<0)?lastlwd:lastrwd;  // select the error location depending on which failed
  jt->emsgstate|=((nesting>0)?EMSGSTATEPARENTYPE:0)|((ptok<=errtok)?EMSGSTATEPARENPOSL:0)|((ptok>=errtok)?EMSGSTATEPARENPOSR:0);  // remember type and location
  if(errtok==0)jt->emsgstate|=EMSGSTATEPARENPOSL|EMSGSTATEPARENPOSR;  // if the ONLY error is from paren, indicate the error is at the paren
  errtok|=ptok<<16;   // install second error number 
 }
 R errtok;  // return the error token(s)
}

// Go through the stack issuing ra() for each name that has not been ra()d already.  We do this when we are about to do something that might delete a local name that is in execution, to wit
// (1) expunging a name; (2) executing ".; (3) assigning a name (with a non-final assignment) when the stack has more than just the value being assigned
// This must be called before pushing the new stack frame.  ofst tells how many leading stack entries to skip; if 0, we assume that we are executing a verb from lines 1-3.
// If we raise a value that is an argument to the current verb running from parse (which may be a large tacit function), we have to tell parse about it so that it can
// flag tpopa/tpopw to fa the value when it leaves execution.  We do so by marking the verb in the top entry of the execution stack
void protectlocals(J jt, I ofst){PSTK *stk=jt->parserstackframe.parserstkend1; A y=0;  // pointer to execution stack
 I verbpos=(stk[2].pt==PTNOUN)?1:2, ranoun=0; verbpos=ofst?0:verbpos; // verb is in slot 1 if x V N, 2 if x N V N or x V V N; but 0 (no verb) for assignments; flag that we changed tpop[aw]
 for(;stk[ofst].pt!=PTMARKBACK;++ofst){
  if(((I)stk[ofst].a&STKNAMED+STKFAOWED)==STKNAMED&&stk[ofst].pt!=PTMARKFRONT){   // if named and not FAOWED - but .a is garbage in a front mark, so not then
   if(!ACISPERM(AC(QCWORD(stk[ofst].a)))&&!(AFLAG(QCWORD(stk[ofst].a))&AFSENTENCEWORD)){   // leave PERMANENT or SENTENCEWORD block always showing NAMED+~FAOWED which will suppress frees quickly
         // to speed parse we mark known-untouchable blocks with NAMED+~FAOWED, with the knowledge that we will not treat them as NAMED here
    rapos(QCWORD(stk[ofst].a),y); stk[ofst].a=SETSTKFAOWED(stk[ofst].a); // if named & unprotected, protect & mark
    ranoun|=BETWEENO(ofst,1,verbpos+1)&&(stk[ofst].pt^PTNOUN);  // remember if we are protecting a noun argument 
   }
  }
 }
 if(verbpos&&ranoun)stk[verbpos].a=SETSTKREFRESHRQD(stk[verbpos].a);  // if this is a line 1-3 exec and we modified tpop[aw], set flag to indicate that
}

#define BACKMARKS 3   // amount of space to leave for marks at the end.  Because we stack 3 words before we start to parse, we will
#define FRONTMARKS 1  // amount of space to leave for front-of-string mark
#define PSTACKRSV 1  // number of stack frames used for holding sentence error info
// Parse a J sentence.  Input is the queue of tokens
// Result has PARSERASGNX (bit 0) set if the last thing is an assignment
// JT flag is used to indicate execution from ". - we can't honor name_: then, or perhaps some assignments
A jtparsea(J jtinplace, A *queue, I nwds){F12IP;PSTK *stack;A z,*v;
 // whenever we execute a fragment, parserstkend1 must be set to the execution stack of the fragment; the stack will be analyzed
 // to get the error token.  Errors during the stacking phase will be located from this routine
 if(likely(nwds>1)) {  // normal case where there is a fragment to parse
  // Save info for error typeout.  We save pointers to the sentence, and the executing parser-stack frame for each execution, from which we infer error position
  PFRAME oframe=jt->parserstackframe;   // save all the stack status

  // allocate the stack.  No need to initialize it, except for the marks at the end, because we
  // never look at a stack location until we have moved from the queue to that position.
  // If there is a stack inherited from the previous parse, and it is big enough to hold our queue, just use that.
  // It goes from stkbgn to stkend1.  During execution we update stkend1 before each execution, and stkbgn at the beginning of each sentence
  // The stack grows down
  PSTK *currstk=jt->parserstackframe.parserstkbgn;  // current stack entry
  if(unlikely(((intptr_t)jt->parserstackframe.parserstkend1-((intptr_t)jt->parserstackframe.parserstkbgn+(nwds+BACKMARKS+FRONTMARKS+PSTACKRSV+1+1)*(intptr_t)sizeof(PSTK)))<0)){A y;
   ASSERT(nwds<65000,EVLIMIT);  // To keep the stack frame small, we limit the number of words of a sentence
   I allo = MAX((nwds+BACKMARKS+FRONTMARKS+PSTACKRSV+1)*sizeof(PSTK),PARSERSTKALLO); // number of bytes to allocate.  Allow 4 marks: 1 at beginning, 3 at end
   GATV0(y,B01,allo,1);
   currstk=(PSTK*)CAV1(y);   // save start of data area, leaving space for all the marks
   // We are taking advantage of the fact the NORMAH is 7, and thus a rank-1 array is aligned on a boundary of its size
   jt->parserstackframe.parserstkend1=(PSTK*)(CAV(y)+allo);  // point to the end+1 of the allocation
  }  // We could worry about hysteresis to avoid reallocation of every call

  // the first element of the parser stack is where we save unchanging error info for the sentence
  currstk->a=(A)queue; currstk->t=(US)nwds;  // addr & length of words being parsed
  jt->parserstackframe.parserstkbgn=currstk+PSTACKRSV;  // advance over the original-sentence info, creating an upward-growing stack at the bottom of the area. jt->parserstackframe.parserstkbgn[-1] has the error info

  // mash into 1 register:  bit 32-63 stack0pt, bit 29-31 (from CONJX) es delayline pull 3/2/1 after current word,
  //  (exec) 23,26 VJTFLGOK1/2+VASGSAFE from verb flags 27 PTNOTLPARX set if stack[0] is not (
  //         25 set if first stack word AFTER the executing fragment is NOT MARK/RPAR (i. e. there are executions remaining on the stack) 
  //  (name resolution) 23-26  free
  //  (exec) 20-22 savearea for pmask for lines 0-2  (stack) 17,20 flags from at NAMEBYVALUE/NAMEABANDON
  //  19 free
  //  18   AR flag from symtab
  //  16 free
  //  0-15 m (word# in sentence)
#define NOTFINALEXECX 25  // bit must be >= VJTFLGOK2X and less than 32+PTISCAVNX, because we test values 
#define NOTFINALEXEC (1LL<<NOTFINALEXECX)
#define LOCSYMFLGX (18-ARLCLONEDX)  // add LOCSYMFLGX to AR*X to get to the flag bit in pt0ecam
#define PMASKSAVEX 20  // 3 bits of pline
#define NAMEFLAGSX 17  // 17 and 20
  // STACK0PT needs only enough info to decode from position 0.  It persists into the execution phase
#if SY_64
#define SETSTACK0PT(v) pt0ecam=(UI4)pt0ecam, pt0ecam|=(I)(v)<<32;
#define GETSTACK0PT (pt0ecam>>32)
#define STACK0PTISCAVN (pt0ecam&(1LL<<(32+PTISCAVNX)))  // the bit must be > NOTFINALX
#else
  UI4 stack0pt;
#define SETSTACK0PT(v) stack0pt=(v);
#define GETSTACK0PT stack0pt
#define STACK0PTISCAVN (PTISCAVN(stack0pt)<<(NOTFINALEXECX+1-PTISCAVNX))  // move the bit to above NOTFINALX
#endif
  jt->parserstackframe.parseroridetok=0xffff;  // indicate no pee/syrd error has occurred
  UI pt0ecam = (AR(jt->locsyms)&ARLCLONED)<<LOCSYMFLGX;  // insert clone/added flags into portmanteau vbl.  locsyms cannot change during this execution

  queue+=nwds-1;  // Advance queueptr to last token.  It always points to the next value to fetch.
#define ASGNEDGE ((0xfLL<<QCASGN)|(0x1LL<<QCLPAR)|(0xfLL<<(QCNAMED+QCASGN))|(0x1LL<<(QCNAMED+QCLPAR)))  // ASGN+EDGE, ignoring frontmark.  We use the full QCTYPE to ensure we ignore QCNAMED, but we don't let LKPNAME slip through
  // If words -1 & -2 exist, we can pull 4 words initially unless word -1 is ASGN or word -2 is EDGE or word 0 is ADV.  Unfortunately the ADV may come from a name so we also have to check in that branch
  // It has long latency so we start early.  The actual computation is about 3 cycles, much faster than a table search+misbranch  LPAR in [-1] doesn't hurt anything
  I pull4=((~(UI8)ASGNEDGE>>QCTYPE(queue[-1])) & (~(UI8)ASGNEDGE>>QCTYPE(queue[-2])))&1;  // NOTE: if nwds<3 (must be 2 then) this will fetch 1 word from before the beginning
                         // of queue.  That's OK, because the fetch will always be legal, as the data always has a word of legal data (usually the block header; in pppp an explicit pad word)

  // Set starting word#, and number of extra words to pull from the queue.  We always need 2 words after the first before a match is possible, or maybe 3 as calculated above
  pt0ecam += nwds+((0b11LL<<CONJX)<<pull4);

  A y;  // y will be the word+flags for the next queue value to process.  We reload it as so that it never has to be saved over a call
  y=*(volatile A*)queue;  // unroll y once

  ++jt->parsercalls;  // now we are committed to full parse.  Push stacks.
// debug if(jt->parsercalls == 0x4)
// debug jt->parsercalls = 0x4;
  PSTK *stackend1=stack=jt->parserstackframe.parserstkend1-BACKMARKS;   // start at the end, pointing to first of 3 marks

  // We don't actually put a mark in the queue at the beginning.  When m goes down to 0, we infer a mark.
  // DO NOT RETURN from inside the parser loop.  Stacks must be processed.

  // We have the initial stack pointer.  Grow the stack down from there
  stack[0].pt = PTMARKBACK;  // install 1 mark, whose 0s will protect the other 2 mark-slots
  while(1){  // till no more matches possible...

    // no executable fragment, pull from the queue.  Bits 31-29 of pt0ecam indicate how many we can safely pull before we
    // check for executability.  If we pull ')', there is no way we can execute
    // anything till 2 more words have been pulled, so we pull them here to avoid parse overhead.
    // Likewise, if we pull a CONJ, we can safely pull 1 more here.  And first time through, we should
    // pull 2 words following the first one.  Sometimes we can pull 3, if there are no adverbs about.

    // pt0ecam is settling from pt0 but it will be ready soon
   
   do{

    if(likely((US)pt0ecam!=0)){  //      if there is another valid token...
     // Move in the new word and check its type.  If it is a name that is not being assigned, resolve its
     // value.  m has the index of the word we just moved.  y = *queue

     // We have the value/typeclass of the next word (QCSENTENCE semantics).  If it is an unassigned name, we have to resolve it and perhaps use the new name/type
     if(((I)y&QCISLKPNAME)){
      // Replace a name (not to left of ASGN) with its value
      // Name, not being assigned
      // Resolve the name.  If the name is x. m. u. etc, always resolve the name to its current value;
      // otherwise resolve nouns to values, and others to 'name~' references
      // The important performance case is local names with symbol numbers.  Pull that out & do it without the call overhead
      // Registers are very tight here.  Nothing survives over a subroutine call - refetch y if necessary  If we have anything to
      // keep over a subroutine call, we have to store it in pt0ecam or some other saved name
      I4 symx, buck;
      symx=NAV(QCWORD(y))->symx;  // see if there is a primary symbol, which trumps everything else
      L *sympv=SYMORIGIN;  // fetch the base of the symbol table.  This can't change between executions but there's no benefit in fetching earlier
      pt0ecam&=~(NAMEBYVALUE+NAMEABANDON)>>(NAMEBYVALUEX-NAMEFLAGSX);  // install name-status flags from y
      pt0ecam|=((I)y&(QCNAMEABANDON+QCNAMEBYVALUE))<<NAMEFLAGSX;
      y=QCWORD(y);  // back y up to the NAME block
      if((symx&~REPSGN4(SGNIF4(pt0ecam,LOCSYMFLGX+ARLCLONEDX)))!=0){  // if we are using an uncloned table and there is a symbol stored there...
       // this is the path for all local names as long as we can use the uncloned table
       L *s=sympv+(I)symx;  // get address of symbol from the primary table
       if(unlikely((s->fval)==0))goto rdglob;  // if value has not been assigned, ignore it.  y has QCSYMVAL semantics
       if(unlikely(ISRAREQD(y=s->fval)))raposlocalqcgsv(QCWORD(y),QCPTYPE(y),y);  // ra the block if needed - rare for locals (only sparse).  Now we call it QCFAOWED semantics
      }else if(likely((buck=NAV(QCWORD(y))->bucket)>0)){  // buckets but no symbol - must be global, or recursive symtab - but not synthetic new name.  We would fetch symx&buck together if we could hand-code it.
       // public names come through here (with positive pucketx) or 
       I bx=NAVV(y)->bucketx;  // get an early fetch in case we don't have a symbol but we do have buckets - globals, mainly
       if(likely((bx|(I)(I1)AR(jt->locsyms))>=0))goto rdglob;  // if positive bucketx and no name has been added, skip the search - the usual case if not recursive symtab
       // negative bucket (indicating exactly where the name is) or some name has been added to this symtab.  We have to probe the local table.  Added name is pretty rare -
       // should we loop through the buckets on a local name?  Probably - saves a call for every name in a non-primary table
       if(likely(bx<0)){L* l; for(l=LXAV0(jt->locsyms)[buck]+sympv;++bx<0;l=l->next+sympv); y=l->fval;}  // local name in cloned table.  Get to it without subroutine call
       else if((y=probelocalbuckets(sympv,y,LXAV0(jt->locsyms)[buck],bx))==0){y=QCWORD(*(volatile A*)queue);goto rdglob;}  // must be unassigned name and there has been a surprise write to the cloned symtab.  Rare indeed.  If not found, it's global - restore y
       if(unlikely(ISRAREQD(y)))raposlocalqcgsv(QCWORD(y),QCPTYPE(y),y);  // ra the block if needed - rare for locals (only sparse).  Now we call it QCFAOWED semantics
      }else{
       // No bucket info.  Usually this is a locative/global, but it could be an explicit modifier, console level, or ".
rdglob: ;  // here when we tried the buckets and failed
       // If the name has a cached reference, use it
       if(NAV(y)->cachedref!=0){  // if the user doesn't care enough to turn on caching, performance must not be that important
        // Note: this cannot be a NAMEABANDON, because such a name is never stacked where it can have the cachedref filled in
        y=NAV(y)->cachedref; // use the cached address/flags, which has QCFAOWED semantics
        goto endname; // take its type, proceed.  We skip the FAOWED issues (FAOWED must be clear in the cached addr)
       }
       y=syrdnobuckets(y);    // do full symbol lookup, knowing that we have checked for buckets already.  Error if not defined.  Result has QCFAOWED semantics
       if(unlikely(y==0))goto undefname;
      }
      // end of looking at local/global symbol tables.
      // y has QCFAOWED semantics here, and has the type flags.  When we finish y must have QCFAOWED semantics, still with the type flags
      // *queue still has the original y, We access it as volatile so the compiler will not put it into a register
      //
      // ra() has been issued for the value if global.  If we don't undo that, we must set FAOWED in y to indicate that fact.  The value will stay protected until
      // the execution that uses this y has completed and the result has been made recursive, so that we know the input value has been RA()d if it has been used
      if(likely(1)){
       // The name is defined with QCFAOWED semantics.  If it's a noun, use its value (the common & fast case)
       // Or, for special names (x u etc) that are always stacked by value, keep the value
       // If a modifier has no names in its value, we will stack it by value.  The Dictionary says all modifiers are stacked by value, but
       // that will make for tough debugging.  We really want to minimize overhead for each/every/inv.  nameless was detected on assignment.
       // But: if the name is any kind of locative, we have to have a full nameref so unquote can switch locales: can't use the value then
       // Otherwise (normal adv/verb/conj name), replace with a 'name~' reference
       //
       // When we finish classifying the name it must have correct QCFAOWED semantics.  Each path must ensure that.  The stored symbol value had QCNAMED set, and QCRAREQD set
       // if the value was to be fa()d by the lookup (i. e. if it was public or sparse) - these flags are set now, and usually we leave them unchanged, the main exception
       // being that if we replace a value with a reference, we mark the reference unnamed because it represents a future lookup rather than a looked-up value
       if((pt0ecam&(NAMEBYVALUE>>(NAMEBYVALUEX-NAMEFLAGSX)))|((I)y&QCNOUN)){   // use value if noun or special name, or name_:
        if(unlikely((pt0ecam&(NAMEABANDON>>(NAMEBYVALUEX-NAMEFLAGSX))))){  // is name_:?
         // if name_:, go delete the name, leaving the value to be deleted later.
         // If the value is local, we must ra it and any other local pointers on the stack
         if(!ISFAOWED(y)&&!ACISPERM(AC(QCWORD(y)))&&!(AFLAG(QCWORD(y))&AFSENTENCEWORD)){  // if the name has been raised already, all stacked copies have been protected.  If PERMANENT or from the executing sentence, needs no protecting
          rapos(QCWORD(y),y);  // ra() the new value first so that all args to undco have been ra()d
          PSTK *sk;
          // scan the stack to see if the new value is at large in the stack, setting FAOWED when it is.  We don't call protectlocals because we need to protect just the one value and speed might matter
          // the stacked value might have been an unflagged non-NAMED result, but after we ra() it we must call it STKNAMED because only STKNAMED values get fa()d when they leave execution
          for(sk=stack;sk!=stackend1;++sk){if(QCWORD(y)==QCWORD(sk->a) && !ISSTKFAOWED(sk->a)){rapos(QCWORD(y),y); sk->a=SETSTKNAMEDFAOWED(sk->a);}}  // if the value we want to use is stacked already, it must be marked non-abondoned
         }
         FPSZSUFF(y=nameundco(jtinplace, QCWORD(*(volatile A*)queue), y), fa(QCWORD(y));)
        }else y=SYMVALTOFAOWED(y) ;  // if global, mark to free later
       }else if(unlikely(QCPTYPE(y)==VALTYPENAMELESS)){
        // nameless modifier, and not a locative.  This handles 'each' and the like.  Don't create a reference; maybe cache the value
        A origy=QCWORD(*(volatile A*)queue);  // refetch name so we can look at its flags
        NAMELESSQCTOTYPEDQC(y)  // convert type to normal adverb, which the parser looks for.  Leaves semantics intact
        y=SYMVALTOFAOWED(y) ;  // if global, mark to free later
        if(NAV(origy)->flag&NMCACHED){  // nameless mod is cachable - replace it by its value in the name
         // cachable and not a locative (and not a noun).  store the value in the name, make the value permanent
         NAV(origy)->cachedref=CLRFAOWED(y); NAV(origy)->bucket=0; ACSETPERM(QCWORD(y)); // clear bucket info so we will skip that search - this name is forever cached with QCFAOWED semantics.  Make the cached value immortal
        }
       }else{  // not a noun/nonlocative-nameless-modifier.  We have to stack a reference to the ACV.
        A origname=QCWORD(*(volatile A*)queue);  // refetch the name
        y=namerefacv(origname,y);   // Replace acv with reference, and fa() looked-up y value if global.  y starts with QCFAOWED semantics, returns with QCFAOWED
        FPSZ(y)
       }
      }else{
undefname:
       // undefined name, possibly because malformed (in which case error is already set).  If a_:, or special u v x. y. in an explicit definition etc, that's fatal; otherwise try creating a dummy ref to [: (to have a verb)
       if(pt0ecam&(NAMEBYVALUE>>(NAMEBYVALUEX-NAMEFLAGSX))&&(pt0ecam&(NAMEABANDON>>(NAMEBYVALUEX-NAMEFLAGSX))||EXPLICITRUNNING)){jt->jerr=EVVALUE;FPS}  // Report error (Musn't ASSERT: need to pop all stacks) and quit (call signal later)
       y=likely(jt->jerr==0)?namerefacv(QCWORD(*(volatile A*)queue), 0):0;    // create a ref to undefined name as verb [:, including flags; if syrd gave an error (malformed name), leave the error in place
       FPSZ(y)   // abort if failure allocating reference, or malformed name
      }
endname: ;
     // else
      // not a name requiring lookup.   enqueue() set the QC flags, which we will use below.  We have just checked the NAMED flag, off here.  Now we notionally switch to
      // QCFAOWED semantics, in which QCISLKPNAME is repurposed to QCFAOWED (& known to be 0).  enqueue() sets QCNAMED+~FAOWED in blocks that are known to need no pretection from deletion:
      // those are PERMANENT blocks and sentence words (together these amount to the entire sentence except for NAMEs).  With FAOWED off we will know that the block needs no fa(), and the flags
      // guarantee that the block is never protected from deletion.
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
     // stack the value, changing it to STKNAMED semantics
     stack[0].a = (A)(((I)y&~QCMASK)+(((I)y>>(QCNAMEDX-STKNAMEDX))&STKNAMEDMSK));   // finish setting the stack entry, with the new word.  The stack entry has STKNAMED/STKFAOWED with the rest of the address valid (no type flags)
     y=*(volatile A*)queue;   // fetch next value as early as possible
     pt0ecam|=((1LL<<(LASTNOUNX-1))<<tx)&(3LL<<CONJX);   /// install pull count es  OR it in: 000= no more, other 001=1 more (CONJ), 01x=2 more (RPAR).  
     UI4 tmpes=pt0ecam;  // pt0ecam is going to be settling because of stack0pt.  To ratify the branch faster we save the relevant part (the pull queue)
     pt0ecam&=(I)(UI4)~((0b111LL<<CONJX));  // clear the pull queue, and all of the stackpt0 field if any.  This is to save 2 fetches in executing lines 0-2 for =:
     // Put new word onto the stack along with a code indicating part of speech and the token number of the word
     SETSTACK0PT(it) stack[0].pt=it;   // stack the internal type too.  We split the ASGN types into with/without name to speed up IPSETZOMB.  Save pt in a register to avoid store forwarding.  Only parts have to be valid; we use the rest as flags
         // and to reduce required initialization of marks.  Here we take advantage of the fact the CONW is set as a flag ONLY in ASGN type
     // *** here is where we exit stacking to do execution ***
     if(!(tmpes&(0b111LL<<CONJX)))break;  // exit stack phase when no more to do, leaving es=0
     // we are looping back for another stacked word before executing.  Restore the pull queue to pt0ecam, after shifting it down and shortening it if we pulled ADV
     pt0ecam|=((tmpes>>=(CONJX+1))&(~(1LL<<(QCADV+1))>>tx))<<CONJX;  // bits 31-29: 1xx->010 01x->001 others->000.  But if ADV, change request for 2 more to request for 1 more by clearing bit 30.
         // We shift the code down to clear the LSB, leaving 0xx.  Then we AND away bit 1 if ADV.  tx is never QCADV+1, because ASGN is mapped to 12-15, so we never AND away bit 0
    }else{  // No more tokens.  If have not yet signaled the (virtual) mark, do so; otherwise we are finished
     --stack;  // back up to new stack frame, where we will store the new word
     if(!PTISMARKFRONT(GETSTACK0PT)){SETSTACK0PT(PTMARKFRONT) stack[0].pt=PTMARKFRONT; break;}  // first time, realize the virtual mark and use it.  e and ca flags immaterial.  We store the mark only so infererrtok can look at it
     EP(0)       // second time.  there's nothing more to pull, parse is over.  This is the normal end-of-parse (except for after assignment)
     // never fall through here
    }
   }while(1);  // Repeat if more pulls required.  We also exit with stack==0 if there is an error
   // words have been pulled from queue.

   pt0ecam|=(((((UI8)1<<QCADV)|((UI8)1<<QCVERB)|((UI8)1<<QCNOUN)|((UI8)1<<QCISLKPNAME)|((UI8)1<<(QCISLKPNAME+QCNAMEBYVALUE))|((UI8)1<<(QCISLKPNAME+QCNAMEBYVALUE+QCNAMEABANDON)))
      >>(QCTYPE(y)))&(UI8)1)<<CONJX;  // save next-is-CAVN status in CONJ (which will become the request for a 2nd pull).

#define NEXTY queue[0]

  // Now execute fragments as long as there is one to execute.
   while(1) {

    // This is where we execute the action routine.  We give it the stack frame; it is responsible
    // for finding its arguments on the stack, storing the result (if no error) over the last
    // stack entry, then closing up any gap between the front-of-stack and the executed fragment,
    // and finally returning the new front-of-stack pointer
    // First, create the bitmask of parser lines that are eligible to execute
    // register pressure is severe where we do subroutine calls below
    I pmask=(I)((C*)&stack[1].pt)[1] & (I)((C*)&stack[2].pt)[2];  // stkpos 2 is enough to detect bit 0 if result is 0-4
    PSTK *fsa=(PSTK*)((I)stack+((2*sizeof(PSTK))>>((I)((C*)&stack[2].pt)[2]&1)));  // pointer to stack slot for the CAV to be executed, for lines 0-4   1 2 2 (2 2)
    A fs=QCWORD(__atomic_load_n(&fsa->a,__ATOMIC_ACQUIRE));  // the action to be executed if lines 0-4.  Must read early: dependency is pmask[0]->fsa->fs->fsflag to settle before we check assignments.  Could be garbage
    pmask&=GETSTACK0PT;  // finish 1st 3 columns of parse
    pmask&=(I)((C*)&stack[3].pt)[3];  // finish 4th column of parse
    A fs1=__atomic_load_n(&stack[1].a,__ATOMIC_ACQUIRE);  // in case of line 1 V0 V1 N2, we will need the flags from V1.  Could be garbage
    pt0ecam&=~(VJTFLGOK1+VJTFLGOK2+VASGSAFE+PTNOTLPAR+NOTFINALEXEC+(7LL<<PMASKSAVEX));   // clear all the flags we will use
    
    if(withprob(pmask!=0,0.8)){  // If all 0, nothing is dispatchable, go push next word after checking for ( .  likely is an overstatement but it gives better register usage
     fs1=QCWORD(fs1);  // clear flags from address
     // We are going to execute an action routine.  This will be an indirect branch, and it will mispredict.  To reduce the cost of the misprediction,
     // we want to pile up as many instructions as we can before the branch, preferably getting out of the way as many loads as possible so that they can finish
     // during the pipeline restart.  The perfect scenario would be that the branch restarts while the loads for the stack arguments are still loading.
     jt->parserstackframe.parserstkend1=stack;    // Save the stackpointer in case there are calls to parse in the names we execute
     I pmask567=pmask;  // save before we mask high bits
     if(withprob(pmask&=0x1F,0.7)){  // if lines 0-4: decodes are mutually exclusive (i. e. one hot)
      I fs1flag=FAV(fs1=pmask&2?fs1:fs)->flag;  // if line 1, fetch V0 flags; otherwise harmless refetch of fs flags.  If line 1 matches, line 0 cannot
      I fsflag=FAV(fs)->flag;  // fetch flags early - we always need them in lines 0-2
      // Here for lines 0-4, which execute the entity pointed to by fs
      // We will be making a bivalent call to the action routine; it will be w,fs,fs for monads and a,w,fs for dyads (with appropriate changes for modifiers).  Fetch those arguments
      // We have fs already.  arg1 will come from position 2 3 1 1 1 depending on stack line; arg2 will come from 1 2 3 2 3
      if(pmask&7){  // lines 0 1 2, verb execution
       // Verb execution (in order: V N, V V N, N V N).  We must support inplacing, including assignment in place, and support recursion
       if(unlikely((!PTISMARKBACKORRPAR(fsa[2]))))pt0ecam|=NOTFINALEXEC;  // remember if this exec is final in its branch.   Wait till we know not fail, so we don't have to wait for (.  Used after execution
       pt0ecam|=pmask<<PMASKSAVEX;  // save pmask over the subroutine calls - also used after the verb execution
       // Get the branch-to address.  It comes from the appropriate valence of the appropriate stack element.  Stack element is 2 except for line 0; valence is monadic for lines 0 1 4
       pmask>>=2;        // now pmask is only 1 bit, indicating 'dyad'
       jt->parserstackframe.sf=fs;  // set new recursion point for $:
       AF actionfn=__atomic_load_n(&FAV(fs)->valencefns[pmask],__ATOMIC_RELAXED);  // frees fs the routine we will execute.  We put the atomic_load here to encourage early load of notfinalexec.  clang17 keeps this in a reg till the call
       fs1flag&=fsflag;  // include ASGSAFE from V0 (if applicable, otherwise just a duplicate of fsflag)
       fsflag>>=pmask; fsflag&=VJTFLGOK1;  // select the monad/dyad bit indicating inplaceability, store it in the monad bit pf flags
       pt0ecam|=fsflag;  // insert flag into portmanteau reg.  Used in the execution
       // If it is an inplaceable assignment to a known name that has a value, remember the value (the name will of necessity be the one thing pointing to the value)
       // We handle =: N V N, =: V N, =: V V N.  In the last case both Vs must be ASGSAFE.  When we set jt->zombieval we are warranting
       // that the next assignment will overwrite the value, and that the reassigned value is available for inplacing.  In the V V N case,
       // this may be over two verbs
       // Consider the case name =. name , 3 + +
       // the name =. name , 3 will come to execution.  Can it inplace?  Yes, because the modified value of name will be on the stack after
       // execution.  That will then execute as (name' + +) creating a fork that will assign to name.  So we can inplace any execution, because
       // it always produces a noun and the only things executable from the stack are tridents
       if(withprob((UI)fsflag>(UI)(PTISNOTASGNNAME(GETSTACK0PT)+(~fs1flag&VASGSAFE)),0.1)){A zval; I targc;  // targc is # refs in the assigned symbol that is allowed for inplaceables
          // The values on the left are good: function that understands inplacing.
          // The values on the right are bad, and all bits > the good bits.  They are: not assignment to name;
          // ill-behaved function (may change locales).  The > means 'good and no bads', that is, inplaceable assignment
        // Here we have an assignment to check.  We will call subroutines, thus losing all volatile registers
        if(likely(GETSTACK0PT&PTASGNLOCAL)){L *s;   // only sentences from explicit defns have ASGNLOCAL set
         // local assignment.  First check for primary symbol.  We expect this to succeed.  We fetch the unflagged address of the value
         if(likely((s=(L*)(I)(NAV(QCWORD(*(volatile A*)queue))->symx&~REPSGN4(SGNIF4(pt0ecam,LOCSYMFLGX+ARLCLONEDX))))!=0)){
          zval=QCWORD((SYMORIGIN+(I)s)->fval);  // get value of symbol in primary table.  There may be no value; that's OK
         }else{zval=QCWORD(jtprobelocal(jt,QCWORD(*(volatile A*)queue),jt->locsyms));}
         targc=ACUC1;  // since local values are not ra()d, they will have AC=1 if inplaceable.  This will miss sparse values (which have been ra()d. which is OK
        }else{
          // global assignment, get slot address.  Global names have been ra()d and have AC=2
         zval=QCWORD(probequiet(QCWORD(*(volatile A*)queue)));
          // We have to handle the case of name =: 5 + > <  name,  i. e where the usercount of 2 came from external boxing rather than FAOWED.
          // we require that the zvalue be a FAOWED argument
          targc=fsa[1].a==(A)((I)zval+STKNAMED+STKFAOWED)?ACUC2:0; targc=stack[1].a==(A)((I)zval+STKNAMED+STKFAOWED)?ACUC2:targc;
        }
        // to save time in the verbs (which execute more often than this assignment-parse), see if the assignment target is suitable for inplacing.  Set zombieval to point to the value if so
        // We require flags indicate not read-only, and correct usecount: 1 if local, 2 if global since we have raised the count of this block already if it is named and to be operated on inplace; +1 if NJA to account for the mapping reference.
        // The block can be virtual, if it is x/y to xdefn, but we must never inplace to a virtual block or to readonly (xxx_index_ =: xxx_index_ + 1)
        // It might seem sound to take a branch on zval since initialization assignment tend to come in batches.  This is an incomplete analysis.  The most likely
        // path to here is to mispredict the assignment and then correctly predict the local path.  In that path we have loaded the symbol number followed by zval, and it will
        // not settle for 10 clocks.  We very much want to keep executing during the settlement so we don't want to risk a misprediction.  We should be executing
        // well into tpop* before zval settles.
        zval=zval?zval:AFLAG0; zval=AC(zval)==(REPSGN((AFLAG(zval)&(AFRO|AFVIRTUAL))-1)&(((AFLAG(zval)>>AFNJAX)&(AFNJA>>AFNJAX))+targc))?zval:0; jt->zombieval=zval;  // compiler should generate BT+ADC
        pmask=(pt0ecam>>(PMASKSAVEX+2))&1;  // restore dyad bit after calls
       }
       PSTK *arga=fsa; arga=pmask?stack:arga; A arg1=arga[1].a;// 1st arg, monad or left dyad  2 3 1
         // this requires fsa to survive over the assignment block, but it's faster than the alternative
       arga=pmask?&stack[3]:arga; A arg2=arga[0].a;   // 2nd arg, fs or right dyad  1 2 3 (2 3)
       // Create what we need to free arguments after the execution.  We keep the information needed to two registers so they can persist over the call as they are needed right away on return
       // (1) When the args return from the verb, we will check to see if any were abandoned.  Those can be freed right away, returning them to the
//     // pool and allowing their cache space to be reused.  But there is a problem:
       // the arg may be freed by the verb (if it is abandoned and gets replaced by a virtual reference).  In this case we can't
       // rely on *arg[12].  But if the value is abandoned, the one thing we CAN count on is that it has a tpop slot.  So we will save
       // the address of the tpop slot IF the arg is abandoned now.  Then after execution we will pick up again, knowing to quit if the tpop slot
       // has been zapped.  If the tpop[aw] is not abandoned, we switch it to look like STKNAMED/~STKFAOWED because that is the fastest path out
       // (2) If either arg is STKNAMED, it can't be abandoned & we use tpop[aw] to hold it, preserving the STKNAMED/STKFAOWED flags.  After exec if FAOWED we must either fa() the arg, paying the debt, or flag the result as STKFAOWED
       // The calculation of tpopa/w will run to completion while the expected indirect-branch misprediction is being processed
       A *tpopa=AZAPLOC(QCWORD(arg1)); tpopa=(A*)((I)tpopa&REPSGN(AC(QCWORD(arg1))&((AFLAG(QCWORD(arg1))&(AFVIRTUAL|AFUNINCORPABLE))-1))); tpopa=tpopa?tpopa:(A*)STKNAMED; tpopa=ISSTKNAMED(arg1)?(A*)arg1:tpopa;
        // Note: this line must come before the next one, to free up the reg holding STKNAMED
       A *tpopw=AZAPLOC(QCWORD(arg2)); tpopw=(A*)((I)tpopw&REPSGN(AC(QCWORD(arg2))&((AFLAG(QCWORD(arg2))&(AFVIRTUAL|AFUNINCORPABLE))-1))); tpopw=tpopw?tpopw:(A*)STKNAMED; tpopw=(I)arg2&(pmask>>-STKNAMEDX)?(A*)arg2:tpopw;
              // point to pointer to arg2 (if it is inplace) - only if dyad
              // tpopa/tpopw are:  monad: w fs  dyad: a w
        // tpopw may point to fs, but who cares?  If it's zappable, best to zap it now
       J jti=(J)((I)jt+(2*pmask)+1); jti=(pt0ecam&VJTFLGOK1)?jti:jt;  // pmask now means 'dyad execution'.  Set args as inplaceable if verb supports jtflags in the current valence
       y=(*actionfn)(jti,QCWORD(arg1),QCWORD(arg2),jt->parserstackframe.sf);   // set bit 0, and bit 1 if dyadic, if inplacing allowed by the verb
         // use jt->parserstackframe.sf to free fs earlier; we are about to break the pipeline.  When we don't break we lose time waiting for jt->fs to settle, but very little.
       // expect pipeline break.  The tpopw/tpopa calculation will still be waiting in the pipeline.  The important thing is to get the instructions ISSUED so that the
       // indirect branch can mispredict and start fetching from the new address.  That is, minimize total # instructions from loading actionfn until the call, with no concern for latency.  In the normal case the
       // load of actionfn will be a single load, completing in 5 cycles, so every instruction after that is money.  The shifting of pmask in place in the code above is needed to prevent the
       // compiler from spilling actionfn (the compiler doesn't realize that this is going to be a pipeline break, so if it feels moved to spill actionfn, it will end with
       // call [actionfn] which is as bad as the code can be).
       // The generated code is maddeningly brittle.  As of 20220802 actionfn is loaded early and kept in a register, while jt->parserstackframe.sf resides in r9 for the duration of the function.  But if you do an atomic_load of jt->parserstackframe.sf
       // in the function call, which frees up r9, the whole allocation changes and actionfn gets dumped to memory.  All would be better (including switch statements) if the compiler recognized the need to load indirect-branch targets early.
RECURSIVERESULTSCHECK
#if MEMAUDIT&0x10
       auditmemchains();  // trap here while we still point to the action routine
#endif
       if(unlikely(y==0)){  // fail parse if error.  All FAOWED names must stay on the stack until we know it is safe to delete them
        pmask=(pt0ecam>>PMASKSAVEX)&7;  // restore after calls
        // if there was an error, try to localize it to this primitive
        jteformat(jt,jt->parserstackframe.sf,QCWORD(stack[(pmask+1)&3].a),QCWORD(pmask&4?stack[3].a:0),0);
        FP
       }
#if AUDITEXECRESULTS
       auditblock(jt,y,1,1);
#endif
#if MEMAUDIT&0x2
       audittstack(jt);
       if(AC(y)==0 || (AC(y)<0 && AC(y)!=ACINPLACE+ACUC1))SEGFAULT; 
#endif           
       // Make sure the result is recursive.  We need this to guarantee that any named value that has been incorporated has its usecount increased,
       //  so that it is safe to remove its protection
       ramkrecursv(y);  // force recursive y
       A freea,freep;  // reg to hold arg we are possibly freeing
       // start with fs, which we extract from the stack to get the FAOWED flag
       PSTK *fsa2=&stack[((pt0ecam&((I)1<<PMASKSAVEX))?1:2)];   // point to verb slot  1 2 2
       freep=fsa2[0].a;  // fetch pointer to verb
       if(unlikely(ISSTKREFRESHRQD(freep))){
        // here the execution of this verb required going through the stack to raise the usecount of local values on the stack.  We have to refresh the tpop[aw] values
        freea=stack[1].a; tpopa=(pt0ecam>>(PMASKSAVEX+2))&(I)freea&STKNAMED?(A*)freea:tpopa;  // refresh tpopa if dyad and the x arg was STKNAMED
        freea=fsa2[1].a;   // fetch y arg of verb
        tpopw=(pt0ecam>>(PMASKSAVEX+2))&(I)freea&STKNAMED?(A*)freea:tpopw;  // refresh tpopw if dyad and the y arg was STKNAMED
        tpopa=~(pt0ecam>>(PMASKSAVEX+2))&(I)freea&STKNAMED?(A*)freea:tpopa;  // refresh tpopa if monad and the y arg was STKNAMED
       }
       fsa2[0]=fsa2[-1];    // overwrite the verb with the previous cell - 0->1  1->2  1->2(NOP)
       PSTK *fsa0=fsa2-1; fsa0=(pt0ecam&(4<<PMASKSAVEX))?fsa2:fsa0; *fsa0=stack[0];  // fsa0=0 1 1->0 1 2 close up the stack  0->0(NOP)  0->1   0->2  scaf could be better
       stack=fsa2;  // move stack to verb slot for now 1 2 2
       if(unlikely(ISSTKFAOWED(freep))){INCRSTAT(ffaowed)/* 0.0 */; faowed(QCWORD(freep),AC(QCWORD(freep)),AT(QCWORD(freep)));}

       // (1) free up inputs that are no longer used.  These will be inputs that are still abandoned and were not themselves returned by the execution.
       // We free them right here, and zap their tpop entry to avoid an extra free later.
       // We free using fanapop, which recurs only on recursive blocks, because that's what the tpop we are replacing does.  All blocks are recursive here.
       // We can free all DIRECT blocks, and PRISTINE also.  We mustn't free non-PRISTINE boxes because the contents are at large
       // and might be freed while in use elsewhere.
       // We mustn't free VIRTUAL blocks because they have to be zapped differently.  When we work that out, we will free them here too
       // NOTE that AZAPLOC may be invalid now, if the block was raised and then lowered for a period.  But if the arg is now abandoned,
       // and it was abandoned on input, and it wasn't returned, it must be safe to zap it using the zaploc from BEFORE the call
       // (2) if any of args/fs is FAOWED, the value is now out of execution and must be fa()d.  BUT if the value is y, it remains in execution and we inherit the
       // FAOWED status into y (but only once per value, because the first such inheritance modifies y). 
       // first the w arg
       // We schedule the loads according to the measured frequencies indicated, which come from the test suite (not including gtdot*.ijs) measured 20241224 on Alder Lake (Skylake compilation)
       // The rule is: loads that contribute to mispredictable test A should be scheduled before the LAST likely-mispredicted branch leading to A.
       // Those loads will settle during misprediction, reducing the time through A (including its misprediction, if it mispredicts)
       if(ISSTKNAMED(tpopw)){INCRSTAT(wfaowed/*.7*/) if(unlikely((A)QCWORD(tpopw)==y)){INCRSTAT(wfainh/*.02*/) y=(A)tpopw;}else if(withprob(ISSTKFAOWED(tpopw),0.2)){INCRSTAT(wfafa/*.08*/) faowed((A)QCWORD(tpopw),AC((A)QCWORD(tpopw)),AT((A)QCWORD(tpopw)));}}
                // if the input is the result, keep the flags from the input, which are set to indicate required frees.  The presence of one of these will suppress matching a also
       else{ /*0.3*/  // tpopw points to the tpop stack, not the argument
        freea=*tpopw;   // get the tstack pointer, which points back to the arg if it has not been zapped
        if(likely(freea!=0)){INCRSTAT(wpop/*.99*/)  // if the arg has a place on the tstack, look at it to see if the block is still around
         I zapok=(AC(freea)&(-(AT(freea)&DIRECT)|SGNIF(AFLAG(freea),AFPRISTINEX))); zapok=freea==y?0:zapok; // (not return value) and abandoned.  Sparse blocks are never abandoned
         if(zapok<0){INCRSTAT(wpopfa/*0.45*/) *tpopw=0; fanapop(freea,AFLAG(freea));  // zap the top block; if recursive, fa the contents.  We free tpopw before subroutine
         }else{INCRSTAT(wpopnull/*0.55*/)}
        }else{INCRSTAT(wnull/*.01*/)}
       }
       // repeat for a if any
       if(ISSTKNAMED(tpopa)){INCRSTAT(afaowed/*.8*/) if(unlikely((A)QCWORD(tpopa)==y)){INCRSTAT(afainh/*.02*/) y=(A)tpopa;}else if(withprob(ISSTKFAOWED(tpopa),0.2)){INCRSTAT(wfafa/*.98*/) faowed((A)QCWORD(tpopa),AC((A)QCWORD(tpopa)),AT((A)QCWORD(tpopa)));}}
       else{ /*.2*/
        freea=*tpopa;   // get the tstack pointer, which points back to the arg if it has not been zapped
        if(likely(freea!=0)){INCRSTAT(apop/*.95*/)  // if the arg has a place on the tstack, look at it to see if the block is still around
         I zapok=(AC(freea)&(-(AT(freea)&DIRECT)|SGNIF(AFLAG(freea),AFPRISTINEX))); zapok=freea==y?0:zapok; // (not return value) and abandoned.  Sparse blocks are never abandoned
         if(zapok<0){INCRSTAT(apopfa/*0.4*/) *tpopa=0; fanapop(freea,AFLAG(freea));  // zap the top block; if recursive, fa the contents.  We free tpopa before subroutine
         }else{INCRSTAT(apopnull/*0.6*/)}
        }else{INCRSTAT(anull/*.05*/)}
       }

       // close up the stack (now 1 2 2) and store the result.  We have to wait till here because y may have inherited FAOWED status
       stack[1].a=y;  // save result 2 3 3; parsetype (noun) is unchanged, token# is immaterial
       y=NEXTY;  // refetch next-word to save regs
       fsa2=stack-1; stack=(pt0ecam&(2<<PMASKSAVEX))?fsa2:stack;  // stack is now 1 1 2


       // Most of the executed fragments are executed right here.  In two cases we can be sure that the stack does not need to be rescanned:
       // 1. pline=2, token 0 is AVN: we have just put a noun in the first position, and if that produced an executable it would have been executed earlier.
       // 2. pline=0 or 2, token 0 not LPAR (might be EDGE): similarly can't execute with noun now in slot 1 (if LPAR and line 0/2, the only possible exec is () )
       // Since if pline is 0 token 0 must be EDGE, this is equivalent to pline!=1 and word 0 not LPAR
       // we save a pass through the matcher in those cases.  The 8 cycles are worth saving, but more than that it makes the branch prediction tighter
       // further, if word 0 is (C)AVN, we can pull 2 tokens if the next token is AVN: we have a flag for that

       // Handle early exits from exec loop: (1) line (0, impossible)/1/2 with AVN in pos 0; (2) (line 0/2, not LPAR in pos 0, finalexec).
       // If line 02 and the current word is (C)AVN and the next is also, stack 2
       // the likelys on the next 2 lines are to get the compiler to avoid spilling queue or nextat
       if(likely((GETSTACK0PT&PTNOTLPAR)!=0)){
        if(likely(STACK0PTISCAVN>=(pt0ecam&NOTFINALEXEC+(1LL<<(PMASKSAVEX+1))))){   // test is AVN or (NOTFINAL and pmask[1] both 0)
         // not ( and (AVN or !line1 & finalexec)): OK to skip the executable check
         pt0ecam&=(((GETSTACK0PT<<(CONJX-PTISCAVNX))&~(pt0ecam<<(CONJX-(PMASKSAVEX+1))))|~CONJ);  // Optionally stack one more.  CONJ comes in as (next is CAVN).  Stack 2 if also (curr is CAVN) and (line 02)
         break;  // Go stack.
        }
       }else{
        // if LPAR, the usual next thing is ( CAVN ) and we will catch that here, to avoid going through fragment search (questionable)
        // the only other plausible case is ( V N ) resulting from line 1, and we can do no better than execute it
        if(PTISRPAR0(stack[2].pt)==0){stack[2]=stack[1]; stack[2].t=stack[0].t; SETSTACK0PT(PTNOUN); stack+=2;}  // ( CAVN ).  Handle it
       }
       // If line 1, we must rescan in case it was V V V N which is now x V V N
      }else{
       // Lines 3-4, adv/conj execution.  We must get the parsing type of the result, but we don't need to worry about recursion
       pmask>>=3; // 1 for adj, 2 for conj
       AF actionfn=__atomic_load_n(&FAV(fs)->valencefns[pmask-1],__ATOMIC_RELAXED);  // refetch the routine address early.  This may chain 2 fetches, which finishes about when the indirect branch is executed
       A arg1=stack[1].a;   // 1st arg, monad or left dyad
       A arg2=stack[pmask+1].a;   // 2nd arg, fs or right dyad
       A arg3=__atomic_load_n(&stack[2].a,__ATOMIC_RELAXED); arg3=pmask&2?arg3:0;  // fs, if this is a conjunction, for FAOWED testing.  If not conj, set to 0 which has FAOWED clear.  Atomic to avoid branch
       UI4 restok=stack[1].t;  // save token # to use for result
       // We set the MODIFIER flag in the call so that jtxdefn/unquote can know that they are modifiers
       // We mark the inputs inplaceable (the first modifier to support that is u`v) - both always, even for adverbs
       A yy=(*actionfn)((J)((I)jt|JTXDEFMODIFIER+JTINPLACEA+JTINPLACEW),QCWORD(arg1),QCWORD(arg2),fs);
RECURSIVERESULTSCHECK
#if MEMAUDIT&0x10
       auditmemchains();  // trap here while we still point to the action routine
#endif
       if(unlikely(yy==0)){  // fail parse if error.  All FAOWED names must stay on the stack until we know it is safe to delete them
        // if there was an error, try to localize it to this primitive
        jteformat(jt,fs,QCWORD(stack[1].a),QCWORD(pmask&2?stack[3].a:0),0);
        FP
       }
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

       while(1){       // for each stacked value, free the value unless it survives to the result, in which case it inherits the FAOWED.  Only one FAOWED can be passed on this way
        if(unlikely(ISSTKFAOWED(arg1))){if(unlikely(QCWORD(arg1)==yy))yy=arg1;else faowed(QCWORD(arg1),__atomic_load_n(&AC(QCWORD(arg1)),__ATOMIC_RELAXED),__atomic_load_n(&AT(QCWORD(arg1)),__ATOMIC_RELAXED));}
            // unlikely because modifiers apply usually to primitives
        if(arg2==0)break; arg1=arg2; arg2=arg3; arg3=0;
       };

       y=NEXTY;  // refetch next-word to save regs
       stack[pmask]=stack[0]; // close up the stack
       stack=stack+pmask;  // advance stackpointer to position before result 1 2
       PTFROMTYPE(stack[1].pt,AT(QCWORD(yy))) stack[1].t=restok; stack[1].a=yy;   // save result, move token#, recalc parsetype
       // quite often there is another execution so we don't try to avoid it
      }
     }else{
      // Here for lines 5-7 (fork/hook/assign), which branch to a canned routine
      // It will run its function, and return the new stackpointer to use, with the stack all filled in.  If there is an error, the returned stackpointer will be 0.
      // We avoid the indirect branch, which is very expensive
      if(pmask567&0b10000000){  // assign - can't be fork/hook
       // Point to the block for the assignment; fetch the assignmenttype; choose the starting symbol table
       // depending on which type of assignment (but if there is no local symbol table, always use the global)
       A symtab=jt->locsyms; {A gsyms=jt->global; symtab=!EXPLICITRUNNING?gsyms:symtab; symtab=!(stack[1].pt&PTASGNLOCAL)?gsyms:symtab;}  // use global table if  =: used, or symbol table is the short one, meaning 'no symbols'
       I rc;
       if(unlikely(stack[3].pt!=PTMARKBACK))protectlocals(jt,3);  // if there are stacked values after the value to be assigned, protect them in case we are about to reassign the value.  This should be rare.  The value itself needs no protection
             // this is not required on a final assignment, because then the sentence mst be in error - not worth testing
       if(likely(GETSTACK0PT&PTNAME0))rc=jtsymbis((J)((I)jt|(((US)pt0ecam==0)<<JTFINALASGNX)),QCWORD(stack[0].a),QCWORD(stack[2].a),symtab);   // Assign to the known name.  If ASSIGNSYM is set, PTNAME0 must also be set
       else rc=jtis((J)((I)jt|(((US)pt0ecam==0)<<JTFINALASGNX)),QCWORD(stack[0].a),QCWORD(stack[2].a),symtab);  // unknown or multiple name, process
#if MEMAUDIT&0x10
       auditmemchains();
#endif
       CLEARZOMBIE   // in case zombieval was set, clear it until next use
       FPZ(rc)
       // it impossible for the stack to be executable.  If there are no more words, the sentence is finished.
       // If FAOWED was in the value, the result needs to inherit it.  But since we retain the same stack position as the result of the assignment, nothing more is needed.
       if(likely((US)pt0ecam==0))EP(rc)  // In the normal sentence name =: ..., we are done after the assignment.  Ending stack must be  (x x result) normally (x MARK result), i. e. leave stackptr unchanged
         // rc has bits 0-1 set to indicate if final assignment, and single assignment to a local name
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
       if(pmask567&0b100000){  // fork NVV or VVV
        A arg1=stack[1].a, arg2=stack[2].a, arg3=stack[3].a;
        A yy=folk(QCWORD(arg1),QCWORD(arg2),QCWORD(arg3));  // create the fork
        // Make sure the result is recursive.  We need this to guarantee that any named value that has been incorporated has its usecount increased,
        //  so that it is safe to remove its protection
        FPZ(yy);    // fail parse if error.  All FAOWED names must stay on the stack until we know it is safe to delete them
        // errors during fork execution are formatted there.  The only error on the fork itself is syntax, for which the terse error is enough
        RECURSIVERESULTSCHECK
        ramkrecursv(yy);  // force recursive y
        y=NEXTY;  // refetch next-word to save regs
        while(1){        // for each stacked value, free the value unless it survives to the result, in which case it inherits the FAOWED.  Only one FAOWED can be passed on this way
         if(ISSTKFAOWED(arg1)){if(unlikely(QCWORD(arg1)==yy))yy=arg1;else faowed(QCWORD(arg1),__atomic_load_n(&AC(QCWORD(arg1)),__ATOMIC_RELAXED),__atomic_load_n(&AT(QCWORD(arg1)),__ATOMIC_RELAXED));}
         if(arg2==0)break; arg1=arg2; arg2=arg3; arg3=0;
        };

        stack[3].t = stack[1].t; stack[3].a = yy;  // take err tok from f; save result; no need to set parsertype, since it didn't change
        stack[2]=stack[0]; stack+=2;  // close up stack
       }else{
        // hook and non-fork tridents
        A arg1=stack[1].a, arg2=stack[2].a, arg3=stack[3].a;
        // Because we use some bits in the PT flags to distinguish assignment types, those bits indicate valid-parse on some invalid combinations.  They come to here with an ASGN in stack[2].  Catch it and reject the fragment
        // We could defer the check until later (in hook) but this seems tolerable
        if(unlikely(AT(QCWORD(arg2))&ASGN))goto rejectfrag;
        I trident=PTISCAVN(stack[3].pt)?3:2; arg3=PTISCAVN(stack[3].pt)?arg3:mark;  // beginning of stack after execution; a is invalid in end-of-stack.  mark suffices to show not FAOWED
        A yy=hook(QCWORD(arg1),QCWORD(arg2),QCWORD(arg3));  // create the hook
        // Make sure the result is recursive.  We need this to guarantee that any named value that has been incorporated has its usecount increased,
        //  so that it is safe to remove its protection
        FPZ(yy);    // fail parse if error.  All FAOWED names must stay on the stack until we know it is safe to delete them
        // errors inside hook are formatted there.  The only error on the hook itself is syntax, for which the terse error is enough
        RECURSIVERESULTSCHECK
        ramkrecursv(yy);  // force recursive y
        while(1){       // for each stacked value, free the value unless it survives to the result, in which case it inherits the FAOWED.  Only one FAOWED can be passed on this way
         if(ISSTKFAOWED(arg1)){if(unlikely(QCWORD(arg1)==yy))yy=arg1;else faowed(QCWORD(arg1),__atomic_load_n(&AC(QCWORD(arg1)),__ATOMIC_RELAXED),__atomic_load_n(&AT(QCWORD(arg1)),__ATOMIC_RELAXED));}
         if(arg2==mark)break; arg1=arg2; arg2=arg3; arg3=mark;
        };
        y=NEXTY;  // refetch next-word to save regs
        PTFROMTYPE(stack[trident].pt,AT(QCWORD(yy))) stack[trident].t = stack[trident-1].t; stack[trident].a = yy;  // take err tok from f of hook, g of trident; save result.  Must store new type because this line takes adverb hooks also
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
    }else{  // no executable fragment (in lines 0-7)
rejectfrag:;
     // LPAR misses the main parse table, which is just as well because it would miss later branches anyway.  We pick it up here so as not to add
     // a couple of cycles to the main parse test.  Whether we stack or execute, y is still set with the next word+type
     if(!(GETSTACK0PT&PTNOTLPAR)){  // ( with no other line.  Better be ( CAVN )
      if(likely(PTISCAVN(~stack[1].pt)==PTISRPAR0(stack[2].pt))){  // must be [1]=CAVN and [2]=RPAR.  To be equal, !CAVN and RPAR-if-0 must both be 0 
       SETSTACK0PT(stack[1].pt); stack[2]=stack[1]; stack[2].t=stack[0].t;  //  Install result over ).  Use value/type from expr, token # from (   Bottom of stack was modified, so refresh the type for it
       stack+=2;  // advance stack pointer to result
      }else{jt->parserstackframe.parserstkend1=stack; jsignal(EVSYNTAX); FPS}  // error if contents of ( not valid.  Set stackpointer so we see the failing exec
      // we fall through to rescan after ( )
     }else{pt0ecam&=~CONJ;  break;}   // parse failed, return to stack next word.  Must clear 'stack 2' flag
    }  // end 'there was a fragment'
   } // end of loop executing fragments
   
  }  // break with stack==0 on error to failparse; main exit is when queue is empty (m<0), to exitparse

 exitparse:
  // *** pt0ecam has been repurposed to bit 0=final assignment, bit 1=assignment to local, others garbage
#if MEMAUDIT&0x2
  audittstack(jt);
#endif
   // Prepare the result
  if(likely(1)){  // no error on this branch
   // before we exited, we backed the stack to before the initial mark entry.  At this point stack[0] is invalid,
   // stack[1] is the initial mark (not written out), stack[2] is the result, and stack[3] had better be the first ending mark
   if(unlikely(!PTISCAVN(stack[2].pt))){jt->parserstackframe.parseroridetok=0; jsignal(EVSYNTAX); FPE}
   if(unlikely(&stack[3]!=stackend1)){jt->parserstackframe.parseroridetok=0; jsignal(EVSYNTAX); FPE}
   z=stack[2].a;   // stack[0..1] are the mark; this is the sentence result, if there is no error.  STKNAMED semantics
   // normal end, but we have to handle the case where the result has FAOWED.   (ex: ([ 4!:55@(<'x')) x   or   a =: >: a   or a =. b).
   // If the value has not been assigned, we might be about to print it or use it as a result.  In that case we tpush to protect it till the next gc,
   // If the value was assigned, we can count on the assigned name to protect it, and we could just fa here.  BUT THAT COULD CRASH if the name is expunged
   // by another thread while we are using the value.  The case is very rare but we test for it, so we have to make it work.
   // If final assignment was local this can't happen, and we do the fa
   jt->parserstackframe = oframe;  // pop the parser frame-stack before tpushna, which may fail
   if(unlikely(ISSTKFAOWED(z))){if(pt0ecam&JTASGNWASLOCAL){faowed(QCWORD(z),AC(QCWORD(z)),AT(QCWORD(z)))} else tpushna(QCWORD(z));}   // if the result needs a free, do it, possibly deferred via tpush
  }else{  // If there was an error during execution or name-stacking, exit with failure.  Error has already been signaled.  Remove zombiesym.  Repurpose pt0ecam
failparsestack: // here we encountered an error during stacking.  The error was processed using an old stack, so its spacing is wrong.
                // we set the error word# for the failing word and then resignal the error to get the spacing right and call eformat to annotate it
                // Here pt0ecam has NOT been repurposed to final assignment yet; we must do so
                // we call eformat with empty a to indicate 'stacking error'
   {C olderr=jt->jerr; RESETERR jt->parserstackframe.parseroridetok=(US)pt0ecam; jsignal(olderr);}
failparseeformat:
   jt->parserstackframe.parserstkend1=stack;  // move stack-end down so eformat doesn't overwrite it
   jteformat(jt,ds(CENQUEUE),mtv,zeroionei(0),0);  // recreate the error with the correct spacing
failparse:
   // the special error code EVABORTEMPTY causes the sentence to be aborted with no error and an empty result (not used)
   // if m=0, the stack contains a virtual mark and perhaps one garbage entry.  Skip the possible garbage first, and also the virtual since it has no flags
   stack+=((US)pt0ecam==0); CLEARZOMBIE z=0; pt0ecam=0;  // indicate not final assignment on error
   // fa() any blocks left on the stack that have FAOWED - but not the mark, which has a garbage address
   for(;stack!=stackend1;++stack)if(!PTISMARKFRONT(stack->pt)&&ISSTKFAOWED(stack->a)){faowed(QCWORD(stack->a),AC(QCWORD(stack->a)),AT(QCWORD(stack->a)))};  // issue deferred fa for items ra()d and not finished
   jt->parserstackframe = oframe;  // pop the parser frame-stack
   if(unlikely(jt->jerr==EVABORTEMPTY)){RESETERR; z=mtv;}  // if the sentence was aborted without error, revert to empty result after cleanup
  }
#if MEMAUDIT&0x2
  audittstack(jt);
#endif
#if MEMAUDIT&0x20
     auditmemchains();  // trap here while we still have the parseline
#endif

  // NOW it is OK to return.  Insert the final-assignment bit (pt0ecam) into the return
  R (A)((I)QCWORD(z)+(pt0ecam&JTFINALASGN));  // this is the return point from normal parsing

 }else{A y;  // m<2.  Happens fairly often, and full parse can be omitted
  if(likely(nwds==1)){A sv=0;  // exit fast if empty input.  Happens only during load, but we can't deal with it
   // 1-word sentence:
   I yflags=(I)queue[0];  // fetch the word, with QCNAMELKP semantics
   // Only 1 word in the queue.  No need to parse - just evaluate & return.  We do it here to avoid parsing overhead, because it happens enough to notice (conditions & function results)
   // No ASSERT - must get to the end to pop stack
   y=QCWORD(yflags);  // point y to the start of block
   if(likely((yflags&QCISLKPNAME))){  // y is a name to be looked up
    if(likely((((I)NAV(y)->symx-1)|SGNIF(AR(jt->locsyms),ARLCLONEDX))>=0)){  // if we are using primary table and there is a symbol stored there...
     L *s=SYMORIGIN+(I)NAV(y)->symx;  // get address of symbol in primary table
     if(likely((sv=s->fval)!=0)){  // value has been assigned
      // the very likely case of a local name.  This value needs no protection because there is nothing more to happen in the sentence and the local symbol table is sufficient protection.  Skip the ra and the tpush
      I svt=QCTYPE(sv); sv=QCWORD(sv);  // type of stored value
      if(likely(svt&QCNOUN)||unlikely(yflags&QCNAMEBYVALUE)){   // if noun or special name, use value
       if(unlikely(yflags&QCNAMEABANDON))goto abandname;  // if abandoned, it loses the symbol-table protection and we have to protect it with ra.  Since rare (especially for a single word!), do so by re-looking up the name
       y=sv; // we will use the value we read
       }else{y=QCWORD(namerefacv(y, sv));}   // Replace other acv with reference.  Could fail.  Flags in sv=s->val=0 to prevent fa() and ensure flags=0 in return value
      goto gotlocalval;   // y has the unprotected value read.  We can use that.
     }
    }
    // falling through here, the name was not fast-local or not assigned.  Resolve it through a full search.  Since errors may happen we have to have a parser stackframe
    //  Since we know that a single word can never execute, we don't need to push the stack: we simply switch to using a stack for this word that points to the sentence
abandname:;
    PSTK* ostk=jt->parserstackframe.parserstkbgn;  // save caller's stack - rest of parserstackframe is untouched and unused
    PSTK localstack[PSTACKRSV]={{.a=(A)queue, .t=1}};  // stack to use, containing pointer to the sentence
    jt->parserstackframe.parserstkbgn=&localstack[PSTACKRSV];  // point to originfo to use, +1
    if(likely((sv=syrd(y,jt->locsyms))!=0)){     // Resolve the name and ra() it if global - undefname gives 0 without error
     // The name was found, not in a static local table.  sv has QCFAOWED semantics
     if(likely((I)sv&QCNOUN)||unlikely(yflags&QCNAMEBYVALUE)){   // if noun or special name, use value
      if(unlikely(yflags&QCNAMEABANDON)){
       // If the value is local, we must ra it because undco() expects it (questionable)
       if(unlikely(!ISRAREQD(sv)))rapos(QCWORD(sv),sv);  // ra() the new value first
       sv=nameundco(jtinplace, y, sv);  // if name_:, go delete the name, leaving the value to be deleted later.  returned sv has QCFAOWED semantics
       y=QCWORD(sv); sv=(A)ISFAOWED(sv);  // undco will set FAOWED if it didn't fa() the value; transfer that to sv
      }else{y=QCWORD(sv); sv=(A)ISFAOWED(sv);}  // not name_:, just use the value.  sv=non0 if it needs free
     }else{y=QCWORD(namerefacv(y, sv)); sv=0;}   // Replace other acv with reference.  Could fail.  Undo the ra from syrd if global, so clear sv to indicate no further fa
    }else{
     // undefined name.  This is very subtle.  We will return a reference to [: as required by the rules (User might execute ".'undefname' which should return empty with no error).
     // This will be formatted for error, if ever, only when returning the value to console level - but at that point the failing sentence has been lost.  That will be OK because ASSERTN
     // suppresses display of the failing line.
     if(yflags&QCNAMEBYVALUE){jsignal(EVVALUE); y=0;}  // Error right away if the unresolved name is x y etc.  Don't ASSERT since we must pop stack
     else{y=unlikely(jt->jerr)?0:QCWORD(namerefacv(y, 0)); sv=0;}   // this will create a ref to undefined name as verb [: .  Could set y to 0 if error; if error already, just keep it
    }
    jt->parserstackframe.parserstkbgn=ostk;  // restore pointer to caller's stack frame before the tpush, which might fail
    // If sv!=0, we found the value and ra()d it.  Match the ra with a tpush so that the value stays protected during further execution
    if(likely(sv!=0)){if(likely(y!=0)){tpush(y);}else fa(sv);}  // undo the ra() in syrd.  In case someone else deletes the value, protect it on the tpop stack till it can be displayed
   }  // any single-word sentence from enqueue must be a name or a CAVN
  }else y=mark;  // empty input - return with 'mark' as the value, which means nothing to parse.  This result must not be passed into a sentence
gotlocalval:;  // local fetches can skip the tpush.  Value in y
  R y;  // indicate not a final assignment - and may be 0
 }

}

