/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Parsing; see APL Dictionary, pp. 12-13 & 38.                            */

#include "j.h"
#include "p.h"


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
// a deferred decrement for the block.  We expect that this call will be followed by a fa() call,
// so we increment this use count id we find a match, to nullify the subsequent fa().
void jtnvrredef(J jt,A w){A*v=jt->nvrav;I s;
 // Scan all the extant names, at all levels.  Unchecked names have LSB clear, and match w.  For them,
 // increment the use count.  If the name has already been decremented, it matches ~w; return quickly
 // then, to make sure we don't increment the same use count twice by continued scanning
 DO(jt->nvrtop, if(0 == (s = (I)w ^ (I)*v)){ra(w); *v = (A)~(I)w; break;}else if(0==~s) break; ++v;);
}    /* stack handling for w which is about to be redefined */

// Action routines for the parse, when an executable fragment has been detected.  Each routine must:
// collect the arguments for the action and run it
// Return failure if the action failed
// Save the result in the last stack entry for the fragment
// Set the word-number entry in the last stack entry to the word number that the result will go by
// Close up any gap between the unexecuted stack elements and the result
// Return the new stack pointer, which is the relocated beginning-of-stack
ACTION(jtmonad1 ){RZ (stack[4] = dfs1(stack[4],stack[2])); stack[5] = stack[3]; stack[3] = stack[1]; stack[2] = stack[0]; R stack+2;}
ACTION(jtmonad2 ){RZ (stack[6] = dfs1(stack[6],stack[4])); stack[7] = stack[5]; stack[4] = stack[2]; stack[2] = stack[0]; stack[5] = stack[3]; stack[3] = stack[1]; R stack+2;}
ACTION(jtdyad   ){RZ(stack[6] = dfs2(stack[2], stack[6], stack[4])); stack[7] = stack[5]; stack[5] = stack[1]; stack[4] = stack[0]; R stack + 4; }
ACTION(jtadv    ){RZ(stack[4] = dfs1(stack[2], stack[4])); stack[5] = stack[3]; stack[3] = stack[1]; stack[2] = stack[0]; R stack + 2; }
ACTION(jtconj   ){RZ(stack[6] = dfs2(stack[2], stack[6], stack[4])); stack[7] = stack[3]; stack[5] = stack[1]; stack[4] = stack[0]; R stack + 4; }
ACTION(jttrident){RZ(stack[6] = folk(stack[2], stack[4], stack[6])); stack[7] = stack[3]; stack[5] = stack[1]; stack[4] = stack[0]; R stack + 4; }
ACTION(jtbident ){RZ(stack[4] = hook(stack[2], stack[4])); stack[5] = stack[3]; stack[3] = stack[1]; stack[2] = stack[0]; R stack + 2; }
ACTION(jtpunc   ){stack[5] = stack[1]; stack[4] = stack[2]; R stack+4;}  // Can't fail; use value from expr, token # from (

static F2(jtisf){R symbis(onm(a),CALL1(jt->pre,w,0L),jt->symb);} 

ACTION(jtis){A f,n,v;B ger=0;C c,*s;
 n=stack[0]; v=stack[4];   // extract arguments
 if((I)stack[1]==1)jt->asgn = 1;  // if the word number of the lhs is 1, it's either (noun)=: or name=: or 'value'=: at the beginning of the line; so indicate
 if(LIT&AT(n)&&1>=AR(n)){
  //ASSERT(1>=AR(n),EVRANK); must be true
  s=CAV(n); ger=CGRAVE==*s; 
  RZ(n=words(ger?str(AN(n)-1,1+s):n));
  if(1==AN(n))RZ(n=head(n));
 }
 ASSERT(AN(n)||!IC(v),EVILNAME);
 f=stack[2]; c=*CAV(f); jt->symb=jt->local&&c==CASGN?jt->local:jt->global;
 if(NAME&AT(n)) symbis(n,v,jt->symb);
 else if(!AR(n))symbis(onm(n),v,jt->symb);
 else {ASSERT(1==AR(n),EVRANK); jt->pre=ger?jtfxx:jtope; rank2ex(n,v,0L,-1L,-1L,jtisf);}
 jt->symb=0;
 stack[5]=stack[3];  // clean up stack after execution
 RNE(stack+4);  // the result is the same value that was assigned
}


#define AVN   (     ADV+VERB+NOUN)
#define CAVN  (CONJ+ADV+VERB+NOUN)
#define EDGE  (MARK+ASGN+LPAR)

// for speed, we encode the stack offset of the token-on-error offset into bits 1-2 of the index of the action routine
#define PMONAD1 2+0
#define PMONAD2 4+0
#define PDYAD 4+1
#define PADV 4+8
#define PCONJ 4+9
#define PTRIDENT 2+1
#define PBIDENT 2+8
#define PASGN 2+9
#define PPAREN 6+0   // error offset immaterial, since never fails
#define PNOMATCH 0

ACTF casefuncs[] = {0, 0, jtmonad1, jttrident, jtmonad2, jtdyad, jtpunc, 0, 0, 0, jtbident, jtis, jtadv, jtconj};

PT cases[] = {
 EDGE,      VERB,      NOUN, ANY,       jtmonad1,  jtvmonad, 1,2,1,
 EDGE+AVN,  VERB,      VERB, NOUN,      jtmonad2,  jtvmonad, 2,3,2,
 EDGE+AVN,  NOUN,      VERB, NOUN,      jtdyad,    jtvdyad,  1,3,2,
 EDGE+AVN,  VERB+NOUN, ADV,  ANY,       jtadv,     jtvadv,   1,2,1,
 EDGE+AVN,  VERB+NOUN, CONJ, VERB+NOUN, jtconj,    jtvconj,  1,3,1,
 EDGE+AVN,  VERB+NOUN, VERB, VERB,      jttrident, jtvfolk,  1,3,1,
 EDGE,      CAVN,      CAVN, ANY,       jtbident,  jtvhook,  1,2,1,
 NAME+NOUN, ASGN,      CAVN, ANY,       jtis,      jtvis,    0,2,1,
 LPAR,      CAVN,      RPAR, ANY,       jtpunc,    jtvpunc,  0,2,0,
};

// Run parser, creating a new debug frame.  Explicit defs, which don't take th etime, go through jtparseas
F1(jtparse){A z;
 RZ(w);
 RZ(deba(DCPARSE,0L,w,0L));
 z=parsea(w);
 debz();
 R z;
}

#define FP {stack = 0; goto exitparse;}   // indicate parse failure
#define EP goto exitparse;   // exit parser, preserving current status

// Parse a J sentence.  Input is the queue of tokens
F1(jtparsea){A *stack,*queue,y,z,*v;I es,i,m,otop=jt->nvrtop,maxnvrlen,*dci=&jt->sitop->dci; B jtxdefn=jt->xdefn;
 // we know what the compiler does not: that jt->sitop and jtxdefn=jt->xdefn are constant even over function calls.
 // So we move those values into local names.
 RZ(w);

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
 GA(y,BOX,2*(m+4),1,0); stack=(2*(m+1))+AAV(y);  // 4 marks: 1 at beginning, 3 at end
 stack[0] = stack[2] = stack[4] = mark;  // install initial marks.  word numbers are unused

 // Set number of extra words to pull from the queue.  We always need 2 words after the first before a match is possible.
 es = 2;

 while(1){  // till no more matches possible...
  // Search for a match per the parse table.  Ordered for speed, taking into account that ) and CONJ
  // have been shifted out of the first position
#define ST(i) AT(stack[2*i])
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
  if(i&6){  // if not PNOMATCH... (we test this way to start calculating the token-in-error offset)
   // This is where we execute the action routine.  We give it the stack frame; it is responsible
   // for finding its arguments on the stack, storing the result (if no error) over the last
   // stack entry, then closing up any gap between the front-of-stack and the executed fragment,
   // and finally returning the new front-of-stack pointer
   *dci = (I)stack[(i&6)|1];  // set the token-in-error, using the offset encoded into i
   if(!(stack = casefuncs[i](jt,stack)))EP  // stop parsing in case of error

  }else{

   // no executable fragment, pull from the stack.  If we pull ')', there is no way we can execute
   // anything till 2 more words have been pulled, so we pull them here to avoid parse overhead.
   // Likewise, if we pull a CONJ, we can safely pull 1 more here.  And first time through, we should
   // pull 2 words following the first one.
   // es holds the number of extra pulls required.  It may go negative, which means no extra pulls.
   // the only time it is positive here is the initial filling of the queue

   do{
    I at;  // type of the new word (before name resolution)

    stack -= 2;  // back up to new stack frame, where we will store the new word
    stack[1] = (A)m;  // install the original token number for the word

    if(m<=0) {  // Toward the end we have to worry about underrunning the queue, and pulling the virtual mark
      if(m==0) {stack[0] = mark; --m; break;}  // move in the mark and use it, and pop the stack
      EP       // if there's nothing more to pull, parse is over
    }

    // Move in the new word and check its type.  If it is a name that is not being assigned, resolve its
    // value.
    at=AT(y = queue[m--]);   // fetch the next word from queue; pop the queue; extract the type
    if(at==NAME) {
     if(AT(stack[2])!=ASGN) {  // Replace a name with its value, unless to left of ASGN
      L* s;  // symbol-table entry for name

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
        if(AT(sv)&NOUN || NMDOT&NAV(y)->flag&&jtxdefn)y=sv;    //check NMDOT first, for speed during xdef.  Use value for special name
        else if (!(y = namerefacv(y, s)))FP   // Replace other acv with reference
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
    stack[0] = y;   // finish setting the stack entry, with the new word
   }while(es-->0);  // Repeat if more pulls required.  We also exit with stack==0 if there is an error
   // words have been pulled from queue

  }
 }  // break with stack==0 on error; main exit is when queue is empty (m<0)
exitparse:

 // Now that the sentence has completed, take care of some cleanup.  Names that were reassigned after
 // their value was moved onto the stack had the decrementing of the use count deferred: we decrement
 // them now (and possibly free them).  This is a bit of a kludge, because the names fall out of circulation
 // before the end of the sentence, and we should free them ASAP; but it is convenient to wait
 // till the end of the sentence, as we do here.
 v=otop+jt->nvrav;  // point to our region of the nvr area
 DO(jt->nvrtop-otop, if(1 & (I)*v)fa((A)~(I)*v); ++v;);   // perform deferred frees.  Test with LSBs in case of 32-bit systems
 jt->nvrtop=otop;  // deallocate the region used in this routine

 RZ(stack);  // If there was an error during execution or name-stacking, exit with failure.  Error has already been signaled

 // before we exited, we backed the stack to before the initial mark entry.  At this point stack[0] is invalid,
 // stack[2] is the initial mark, stack[4] is the result, and stack[6] had better be the first ending mark
 z=stack[4];   // stack[1..2] are the mark; this is the sentence result, if there is no error
 ASSERT(AT(z)&MARK+CAVN&&AT(stack[6])==MARK,(*dci = 0,EVSYNTAX));  // OK if 0 or 1 words left (0 should not occur)
 R z;
}

/* locals in parsea                                             */
/* i:     index in cases[] of matching 4-pattern                */
/* m:     current # of non-mark words in the queue              */
/* otop:  old value of jt->nvrtop                               */
/* queue: queue, taken from the input                           */
/* stack: parser stack, (word,count) pairs                      */
/* w:     argument containing the queue                         */
/* y:     array temp                                            */
/* z:     result                                                */
