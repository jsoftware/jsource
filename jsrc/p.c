/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Parsing; see APL Dictionary, pp. 12-13 & 38.                            */

#include "j.h"
#include "p.h"


#define PARSERSTKALLO (490*sizeof(PSTK))  // number of stack entries to allocate, when we allocate, in bytes

/* NVR - named value reference                                          */
/* a value referenced in the parser which is the value of a name        */
/* (that is, in some symbol table).                                     */
/*                                                                      */
/* jt->nvra      NVR stack a: stack of A values.  LSB is a flag         */
/* jt->nvrav     AAV(jt->nvra)                                          */
/* jt->nvrtop    index of top of stack                                  */
/*                                                                      */
/* Each call of the parser records the current NVR stack top (nvrtop),  */
/* and pop stuff off the stack back to that top on exit       */
/*                                                                      */
// The nvr stack contains pointers to values, added as names are moved
// from the queue to the stack.  The LSB of a pointer is set to indicate that the
// value must have the use count decremented at a safe time.
// nvrpop(otop): pop stuff off the NVR stack back to the old top otop,
//  performing deferred frees
// nvrredef(w):  w is the value of a name about to be redefined
//   (reassigned or erased).  If w is in the nvr stack, the use count of w
//   is incremented.  We expect the caller to immediately decrementing the use-count
//   and we are nullifying that operation

// This design is questionable, because it requires a linear scan of all names extant
// at all levels of function-call (the scan is performed only during nvrredef, which mitigates
// the problem).  Also, names once stacked are considered extant for the duration of the sentence.
// An alternative is to (1) use flags in the block to indicate a block that is extant or has
// a deferred free outstanding, thus avoiding the search; (2) handle the deferred frees at the end of each action routine,
// thus reducing the number of extant names.  This should be revisited when we come to
// dealing with in-place modification.

B jtparseinit(J jt){A x;
 GA(x,INT,20,1,0); ra(x); jt->nvra=x; jt->nvrav=AAV(x);  // Initial stack.  Size is doubled as needed
 R 1;
}

// w is a name about to be redefined.  If it is on the nvr list, at any level, set to complement to indicate
// a deferred decrement for the block.  Return 1 if a deferred decrement was created, 0 if not (including
// the case where there was already a deferred-decrement)
B jtnvrredef(J jt,A w){A*v=jt->nvrav;I s;
 // Scan all the extant names, at all levels.  Unchecked names have LSB clear, and match w.  For them,
 // increment the use count.  If the name has already been decremented, it matches ~w; return quickly
 // then, to make sure we don't increment the same use count twice by continued scanning
 DO(jt->nvrtop, if(0 == (s = (I)w ^ (I)*v)){*v = (A)~(I)w; R 1;}else if(~0==s) break; ++v;);
 R 0;  // if we get through without requesting a decrement, return 0
}    /* stack handling for w which is about to be redefined */

static F2(jtisf){R symbis(onm(a),CALL1(jt->pre,w,0L),jt->symb);} 

static PSTK* jtis(J jt){A f,n,v;B ger=0;C c,*s;PSTK* stack=jt->parserstkend1; 
 n=stack[0].a; v=stack[2].a;   // extract arguments
 if(stack[0].t==1)jt->asgn = 1;  // if the word number of the lhs is 1, it's either (noun)=: or name=: or 'value'=: at the beginning of the line; so indicate
 if(jt->assignsym){symbis(n,v,(A)1);}   // Assign to the known name. 1 is any nonzero
 else {
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
  ASSERT(AN(n)||!IC(v),EVILNAME);  // error if name empty
  // Point to the block for the assignment; fetch the assignment pseudochar (=. or =:); choose the starting symbol table
  // depending on which type of assignment (but if there is no local symbol table, always use the global)
  f=stack[1].a; c=*CAV(f); jt->symb=jt->local&&c==CASGN?jt->local:jt->global;
  // if simple assignment to a name (normal case), do it
  if(NAME&AT(n)) symbis(n,v,jt->symb);
  // otherwise, if it's an assignment to an atomic computed name, convert the string to a name and do the single assignment
  else if(!AR(n))symbis(onm(n),v,jt->symb);
  // otherwise it's multiple assignment (could have just 1 name to assign, if it is AR assignment).
  // Verify rank 1.  For each lhs-rhs pair, do the assignment (in jtisf).
  // if it is AR assignment, apply jtfxx to each assignand, to convert AR to internal form
  // if not AR assignment, just open each box of rhs and assign
  else {ASSERT(1==AR(n),EVRANK); jt->pre=ger?jtfxx:jtope; rank2ex(n,v,0L,-1L,-1L,jtisf);}
 }
 // obsolete jt->symb=0;   // Restore to no symbol table selected
 RNE(stack+2);  // the result is the same value that was assigned
}


#define AVN   (     ADV+VERB+NOUN)
#define CAVN  (CONJ+ADV+VERB+NOUN)
#define EDGE  (MARK+ASGN+LPAR)

// for speed, we encode the stack offset of the token-on-error offset into bits 0-1 of the index of the action routine
#define PMONAD1 1+0
#define PMONAD2 2+0
#define PDYAD 2+4
#define PADV 2+8
#define PCONJ 2+12
#define PTRIDENT 1+4
#define PBIDENT 1+8
#define PASGN 1+12
#define PPAREN 3+0   // error offset immaterial, since never fails
#define PNOMATCH 0

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

// Run parser, creating a new debug frame.  Explicit defs, which don't take the time, go through jtparseas
F1(jtparse){A z;
 RZ(w);
 RZ(deba(DCPARSE,0L,w,0L,0L));
 z=parsea(w);
 debz();
 R z;
}


#define FP {stack = 0; goto exitparse;}   // indicate parse failure
#define EP goto exitparse;   // exit parser, preserving current status
#define EPZ(x) if(!(x)){stack=0;EP}   // exit parser if x==0

// In-place operations
//
// An in-place operation requires an inplaceable argument, which is marked as LSB of AC=0,
// Blocks are born non-inplaceable; they need to be marked inplaceable by the creator.
// Blocks that are assigned are marked not-inplaceable.
// But an in-placeable argument is not enough;
// The key point is that an in-place operation is connected to a FUNCTION CALL, not an DATA BLOCK.
// For example, in (+/ % #) <: y the result of <: y has a usecount of 1 and is
// not used anywhere else, so it is passed into the fork as an inplaceable argument.
// This same block is not inplaceable when it is passed into # (because it will be needed later by +/)
// but it can be inplaceable when passed into +/.
// Thus, it is up to the caller to decide whether it can tolerate having an argument overwritten;
// if so, it marks the argument as inplaceable for that call only.
//
// By setting the inplaceable flag in a result, a verb warrants that the block does not contain and is not contained in
// any other block.  Whether to traverse to do this is an open issue, deferred for the time being because only
// direct blocks are marked inplaceable now.
//
// The 2 LSBs of jt are set to indicate inplaceability of arguments.  The caller sets them when e
// has no further use for the argument AND e knows that the callee can handle in-place arguments.
// An argument is inplaceable only if it is marked as such in the block AND in jt.
//
// Bit 0 of jt is for w, bit 1 for a.
//
// There is one more piece to the inplace system: reassigned names.  When there is an
// assignment to a name, the block being reassigned can be reused for the output if:
//   the usecount is 1
//   the current execution is the only thing on the stack
//   the assignment is to a local name 
// We detect these cases only for local assignments, to avoid worries about aliasing
// with other uses of the names in sentences in execution at higher stack levels; and only when
// the verb about to be executed is the only thing on the stack (but there may have been previous
// executions in the sentence before the final execution whose result is about to be assigned).
// The name jt->assignsym is set to point to the symbol-table entry whenever a local assignment is detected
// (note that some such assignments, coming from ". or (name)=. might go undetected), as a time-saving maneuver
// because such an assignment can complete quickly.
// When a symbol is being reassigned and the data area can be reused, the name jt->zombieval is set
// to point to the data block for the name.
//
// Note that if jt->zombieval is set, any argument that is equal to jt->zombieval will be
// ipso facto inplaceable, and moreover the flag in jt corresponding to such
// an argument WILL NOT be set because it came from evaluating a name.  So, a verb supporting inplace
// operation must check for (jtinplace&1||jt->zombiesym==w) for example
// if it wants to inplace the w argument.

// Note kludge: the stack bits to indicate inplaceability could be replaced by performing
// ra();tpush();  when a word is moved to the stack; but until we
// get nonrecursive usecounts we don't do that.


// Prep for in-place operations.
// If the top-of-stack is ASGN, we will peek into the queue to get the symbol-table entry for the
// name about to be assigned.  jt->zombiesym will point to the symbol-table entry for the assignee
// provided:
//  usecount==1
//  DIRECT type
//  stack will be empty after the current execution
//  The verb supports in-place ops
//  local assignment
//  not assigning to a locative
// It is 0 otherwise.  When zombiesym is set, the data block for
// the assignee may be used as a free block.  zombiesym must be cleared:
//  when the assignment is performed
//  if an error is encountered (preventing the assignment)
// Set zombiesym if current assignment can be in-place.  w is the end of the execution that will produce the result to assign
#define IPSETZOMB(w) if(AT(stack[0].a)==ASGN&&stack[(w)+1].a==mark&&AT(queue[m])==NAME&&CAV(stack[0].a)[0]==CASGN&&jt->local \
   &&AN(queue[m])==NAV(queue[m])->m&&(s=probelocal(queue[m]))){jt->assignsym=s; if(s->val&&AT(s->val)&DIRECT&&AC(s->val)<=ACUC1)jt->zombieval=s->val;}

// In-place operands
// An operand is in-placeable if:
//  usecount==1 with inplaceable bit
//  DIRECT type (implied for now - we don't inplace otherwise)
//  the verb supports in-place ops
// We set the jt bits to indicate inplaceability.  Since the parser never reuses an argument, all bits will
// be set if the callee can handle inplaceing.  The inplaceable bit in the arguments must also be cleared if the
// callee cannot handle inplaceing, to prevent releasing an in-placeable block where it is not understood.
// NOTE that in name =: x i} name, the zombiesym will be set but the name operand will NOT be marked inplace.  The action routine
// should check the operand addresses when zombiesym is set.
#define DFSIP1(v,w) if(VAV(stack[v].a)->flag&VINPLACEOK1){IPSETZOMB(w) y=jtdfs1((J)((I)jt|1),stack[w].a,stack[v].a);}else{ACIPNO(stack[w].a); y=dfs1(stack[w].a,stack[v].a);}
#define DFSIP2(aa,v,w) if(VAV(stack[v].a)->flag&VINPLACEOK2){IPSETZOMB(w) y=jtdfs2((J)((I)jt|3),stack[aa].a,stack[w].a,stack[v].a);}else{ACIPNO(stack[w].a);ACIPNO(stack[aa].a); y=dfs2(stack[aa].a,stack[w].a,stack[v].a);}
// Storing the result
// We store the result into the stack and move the token-number for it.  We set the
// in-place flag to 1 to indicate that the result came from execution, EXCEPT that if
// the result address matches an operand address, we keep the flag from the stack entry that
// matched.  The idea is that if (] y) returns the address of y, we should use the same flags that y had
// we pass in the stack index of the verb, and infer the operands from that
// z=result stack index, a/w=stack index of argument(s), t=source for token number
#define STO1(z,w,tok) {stack[z].t=stack[tok].t; EPZ(stack[z].a=y)}
#define STO2(z,aa,w,tok) {stack[z].t=stack[tok].t; EPZ(stack[z].a=y)}

// Closing up the stack
#define SM(to,from) stack[to]=stack[from]

// Parse a J sentence.  Input is the queue of tokens
F1(jtparsea){PSTK *stack;A *queue,y,z,*v;I es,i,m,otop=jt->nvrtop,maxnvrlen,*dci=&jt->sitop->dci; B jtxdefn=jt->xdefn;L* s;  // symbol-table entry
 // we know what the compiler does not: that jt->sitop and jtxdefn=jt->xdefn are constant even over function calls.
 // So we move those values into local names.
 PSTK *obgn=jt->parserstkbgn, *oend1=jt->parserstkend1;  // push the parser stack
 RZ(w);  // if nothing to do, it is OK to exit before we start pushing

 // This routine has two global responsibilities in addition to parsing.  jt->asgn must be set to 1
 // if the last thing is an assignment, and since this flag is cleared during execution (by ". and
 // others), it must be set at the time the assignment is executed.  We catch it in the action routine,
 // noting when the assignment is to (possibly inherited) word 1 (word 0 is the mark).
 //
 // jt->sitop->dci must be set before executing anything that might fail; it holds the original
 // word number+1 of the token that failed.  jt->sitop->dci is set before dispatching an action routine,
 // so that the information is available for formatting an error display
 m=AN(w); jt->asgn = 0; ++jt->parsercalls;
 if(1>m)R mark;  // exit fast if empty input

 // to simulate the mark at the head of the queue, we set queue to point to the -1 position which
 // is an out-of-bounds entry that must never be referenced.  m=0 corresponds to this mark; otherwise queue[m] is original
 // word m-1, with the number in the stack being one higher than the original word number
 queue=AAV(w)-1;
 ASSERT(!(AT(queue[1])&ASGN),EVSYNTAX);   // make sure there is something before each ASGN

 // We don't refer to w past this point.  It may be overwritten be recursive calls

 // As names are dereferenced, they are added to the nvr queue.  To save time in the loop, we now
 // make sure there is enough room in the nvr queue to handle all the names we will encounter in
 // this sentence.  For simplicity's sake, we just assume the worst, that every word is a name, and
 // make sure there is that much space.  BUT if there were an enormous tacit sentence, that would be
 // very inefficient.  So, if the sentence is too long, we go through and count the number of names,
 // rather than using a poor upper bound.
 if (m < 128)maxnvrlen = m;   // if short enough, assume they're all names
 else {
  maxnvrlen = 0;
  DQ(m, if(NAME==AT(queue[i+1]))++maxnvrlen;)
 }
 // extend the nvr stack, doubling its size each time, till it can hold our names
 while((jt->nvrtop+maxnvrlen) > AN(jt->nvra)){RZ(jt->nvra = ext(1, jt->nvra)); jt->nvrav = AAV(jt->nvra);}

 // allocate the stack.  No need to initialize it, except for the marks at the end, because we
 // never look at a stack location until we have moved from the queue to that position.
 // Each word gets two stack locations: first is the word itself, second the original word number+1
 // to use if there is an error on the word
 // If there is a stack inherited from the previous parse, and it is big enough to hold our queue, just use that.
 // The stack grows down
 if((U)oend1-(U)obgn >= (m+4)*sizeof(PSTK))stack=oend1-3;   // if we can use the previous allocation, start at the end, with 3 marks
 else{
   I allo = MAX((m+4)*sizeof(PSTK),PARSERSTKALLO); // number of bytes to allocate.  Allow 4 marks: 1 at beginning, 3 at end
   GA(y,B01,allo,1,0);
   jt->parserstkbgn=(PSTK*)AV(y);   // save start of data area
   stack=(PSTK*)((U)jt->parserstkbgn+allo)-3;  // point to the ending marks
 }
 // We have the initial stack pointer.  Grow the stack down from there
 stack[0].a = stack[1].a = stack[2].a = mark;  // install initial ending marks.  word numbers are unused

 // Set number of extra words to pull from the queue.  We always need 2 words after the first before a match is possible.
 es = 2;

 // DO NOT RETURN from inside the parser loop.  Stacks must be processed.

 while(1){  // till no more matches possible...
  // Search for a match per the parse table.  Ordered for speed, taking into account that ) and CONJ
  // have been shifted out of the first position
#define ST(i) AT(stack[i].a)
  if(ST(2) & CAVN) { // cases 0-7
   if(ST(0) == NAME)i = PASGN;   // NAME is set only if followed by ASGN
   else {  // cases 0-6
    if (!(ST(0)&(EDGE + AVN)))i = PNOMATCH;
    else if ((ST(1) | ST(2))&(ADV + CONJ)){ /* cases 3, 4, 6, but only if ST(1) is CAVN  */
     i = ST(1)&(ADV + CONJ)
         ? (ST(0)&EDGE ? PBIDENT : PNOMATCH)
         : ST(1)&CAVN   // here ST2 must be adv/conj
           ? (ST(2)==ADV ? PADV : ST(3)&(VERB + NOUN) ? PCONJ : ST(0)&EDGE ? PBIDENT : PNOMATCH)
           : PNOMATCH;
    } else { /* cases 0, 1, 2, 5, 6, , but only if ST(1) is CAVN.  ST(1) is NOUN, VERB, or other nommatching such as (  */
     if (ST(2)&NOUN){
         i = ST(0)&EDGE ? (ST(1) == VERB ? PMONAD1 : ST(1)&NOUN ? PBIDENT : PNOMATCH) : (ST(1) == ASGN && ST(0)&NOUN) ? PASGN : PNOMATCH;
     } else {
      i = ST(3)&NOUN
          ? (ST(1)&NOUN ? PDYAD : ST(1) == VERB ? PMONAD2 : (ST(1) == ASGN && ST(0)&NOUN) ? PASGN : PNOMATCH)
          : ST(1)&CAVN ? (ST(3) == VERB ? PTRIDENT : ST(0)&EDGE ? PBIDENT : PNOMATCH) : (ST(1) == ASGN && ST(0)&NOUN) ? PASGN : PNOMATCH;
     }
    }
   }
  } else i = (ST(0)==LPAR && ST(1)&CAVN && ST(2)==RPAR) ? PPAREN : PNOMATCH;  // case 8.  Test LPAR first, because if LPAR doesn't match, the
                   // sentence will have a syntax error, while RPAR might just be passing through

  // end of match analysis.  i indicates what we matched
  if(i&3){  // if not PNOMATCH... (we test this way to start calculating the token-in-error offset)
   // This is where we execute the action routine.  We give it the stack frame; it is responsible
   // for finding its arguments on the stack, storing the result (if no error) over the last
   // stack entry, then closing up any gap between the front-of-stack and the executed fragment,
   // and finally returning the new front-of-stack pointer
   *dci = stack[(i&3)].t;  // set the token-in-error, using the offset encoded into i
   // Save the current stack pointer for use as end+1 pointer by the next parse.  This is a design issue.  Should we
   // take the time to store the stack pointer on every execution, or just move it back the maximum possible amount
   // before the loop?  The maximum possible amount may be much larger than actual stack usage, making for
   // inefficient use of stack and more frequent allocation; but a store for every execution is expensive.
   // We store it every time here, which is probably not the right answer.
   jt->parserstkend1=stack;   // save stack pointer as parm to action routine AND next level of parse
   switch(i) {
   // Action routines for the parse, when an executable fragment has been detected.  Each routine must:
   // collect the arguments for the action and run it
   // Return failure if the action failed
   // Save the result in the last stack entry for the fragment
   // Set the word-number entry in the last stack entry to the word number that the result will go by
   // Close up any gap between the unexecuted stack elements and the result
   // Return the new stack pointer, which is the relocated beginning-of-stack
   // Set the in-place flags for verb arguments
   case PMONAD1:
   DFSIP1(1,2) STO1(2,2,1) SM(1,0); stack += 1; break;
   case PMONAD2:
   DFSIP1(2,3) STO1(3,3,2) SM(2,1); SM(1,0); stack += 1; break;  //STO(z,w,toksource)
   case PDYAD:
   DFSIP2(1,2,3) STO2(3,1,3,1) SM(2,0); stack += 2; break;  //STO(z,a,w,toksource)
   case PADV:
   EPZ(stack[2].a = dfs1(stack[1].a, stack[2].a)); stack[2].t = stack[1].t; SM(1,0); stack += 1; break;
   case PCONJ:
   EPZ(stack[3].a = dfs2(stack[1].a, stack[3].a, stack[2].a)); stack[3].t = stack[1].t; SM(2,0); stack += 2; break;
   case PTRIDENT:
   EPZ(stack[3].a = folk(stack[1].a, stack[2].a, stack[3].a)); stack[3].t = stack[1].t; SM(2,0); stack += 2; break;
   case PBIDENT:
   EPZ(stack[2].a = hook(stack[1].a, stack[2].a)); stack[2].t = stack[1].t; SM(1,0); stack += 1; break;
   case PPAREN:
   stack[2].a=stack[1].a; stack[2].t=stack[0].t; stack += 2; break; // Can't fail; use value from expr, token # from (
   case PASGN: if(!(stack=jtis(jt)))EP break;
   }

  }else{

   // no executable fragment, pull from the stack.  If we pull ')', there is no way we can execute
   // anything till 2 more words have been pulled, so we pull them here to avoid parse overhead.
   // Likewise, if we pull a CONJ, we can safely pull 1 more here.  And first time through, we should
   // pull 2 words following the first one.
   // es holds the number of extra pulls required.  It may go negative, which means no extra pulls.
   // the only time it is positive here is the initial filling of the queue

   do{
    I at;  // type of the new word (before name resolution)

    stack--;  // back up to new stack frame, where we will store the new word
    stack[0].t = m;  // install the original token number for the word, and clear in-placeable flag

    if(m<=0) {  // Toward the end we have to worry about underrunning the queue, and pulling the virtual mark
      if(m==0) {stack[0].a = mark; --m; break;}  // move in the mark and use it, and pop the stack
      EP       // if there's nothing more to pull, parse is over
    }

    // Move in the new word and check its type.  If it is a name that is not being assigned, resolve its
    // value.
    at=AT(y = queue[m--]);   // fetch the next word from queue; pop the queue; extract the type
    if(at==NAME) {
     if(AT(stack[1].a)!=ASGN) {  // Replace a name with its value, unless to left of ASGN

      // Name, not being assigned
      // Resolve the name.  If the name is x. m. u. etc, always resolve the name to its current value;
      // otherwise resolve nouns to values, and others to 'name~' references
      // original:     if (!(y = jt->xdefn&&NMDOT&NAV(y)->flag ? symbrd(y) : nameref(y))) { stack = 0; goto exitparse; }
      // To save some overhead, we inline this and do the analysis in a different order here
      *dci = m+1;  // syrd can fail, so we have to set the error-word number (before it was decremented) before calling
      if(s=syrd(y,0L)) {   // look up the name in the symbol tables.  0L=Don't bother storing which symbol table was used
        A sv;  // pointer to value block for the name
        
        if(!(sv = s->val))FP  // symbol table entry, but no value.
          // Following the original parser, we assume this is an error that has been reported earlier.  No ASSERT here, since we must pop nvr stack

        // The name is defined.  If it's a noun, use its value (the common & fast case)
        // Or, for special names (x. u. etc) that are always stacked by value, keep the value
        // Otherwise (normal adv/verb/conj name), replace with a 'name~' reference
        if(AT(sv)&NOUN || NMDOT&NAV(y)->flag&&jtxdefn){
         y=sv;    //check NMDOT first, for speed during xdef.  Use value for special name
        } else if (!(y = namerefacv(y, s)))FP   // Replace other acv with reference
      } else {
        // undefined name.  If special x. u. etc, that's fatal; otherwise create a dummy ref to [: (to have a verb)
        if(NMDOT&NAV(y)->flag&&jtxdefn){jsignal(EVVALUE);FP}  // Report error (Musn't ASSERT: need to pop nvr stack) and quit
        if (!(y = namerefacv(y, s)))FP    // this will create a ref to undefined name as verb [:
          // if syrd gave an error, namerefacv may return 0.  This will have previously signaled an error
      }

      // In case the name is assigned during this sentence (including subroutines), remember the data block that the name created
      // NOTE: the nvr stack may have been relocated by action routines, so we must refer to the globals
      jt->nvrav[jt->nvrtop++] = y;
     }

    // If the new word was not a name (whether assigned or not), look to see if it is ) or a conjunction,
    // which allow 2 or 1 more pulls from the queue without checking for an executable fragment.
    // NOTE that we are using the original type for the word, which will be obsolete if the word was a
    // name that was replaced by name resolution.  We don't care - RPAR was never a name to begin with, and CONJ
    // is much more likely to be a primitive; and we don't want to take the time to refetch the resolved type
    } else if(at&RPAR+CONJ){es = (at>>RPARX)+(1/(RPARX>CONJX));}  // 1 for CONJ, 2 for RPAR; the RPARX>CONJX bit is to give a compile-time error if RPARX is not > CONJX

    // y has the resolved value, which is never a NAME unless there is an assignment immediately following
    stack[0].a = y;   // finish setting the stack entry, with the new word
   }while(es-->0);  // Repeat if more pulls required.  We also exit with stack==0 if there is an error
   // words have been pulled from queue

  }
 }  // break with stack==0 on error; main exit is when queue is empty (m<0)
exitparse:

 // Now that the sentence has completed, take care of some cleanup.  Names that were reassigned after
 // their value was moved onto the stack had the decrementing of the use count deferred: we decrement
 // them now.  There may be references to these names in the result (if we are returning a verb/adv/conj),
 // so we don't free the names quite yet: we put them on the tpush stack to be freed after we know
 // we are through with the result
 v=otop+jt->nvrav;  // point to our region of the nvr area
 DO(jt->nvrtop-otop, if(1 & (I)*v)tpush((A)~(I)*v); ++v;);   // schedule deferred frees.  Test with LSBs in case of 32-bit systems
 jt->nvrtop=otop;  // deallocate the region used in this routine

 jt->parserstkbgn=obgn, jt->parserstkend1=oend1;  // pop the parser stack

 // NOW it is OK to return

 if(!stack){jt->assignsym=0; jt->zombieval=0; R 0;}  // If there was an error during execution or name-stacking, exit with failure.  Error has already been signaled.  Remove zombiesym

 // before we exited, we backed the stack to before the initial mark entry.  At this point stack[0] is invalid,
 // stack[1] is the initial mark, stack[2] is the result, and stack[3] had better be the first ending mark
 z=stack[2].a;   // stack[1..2] are the mark; this is the sentence result, if there is no error
 ASSERT(AT(z)&MARK+CAVN&&stack[3].a==mark,(*dci = 0,EVSYNTAX));  // OK if 0 or 1 words left (0 should not occur)
 R z;
}

