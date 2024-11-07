/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Symbol Table                                                            */

#include "j.h"


/* a symbol table aka locale is a type LX vector rank 0                         */
/* zero elements mean unused entry                                         */
/* non-zero elements are indices in the global symbol pool and             */
/*     are head pointers to a linked list                                  */
//     In local symbol tables, the MSB of a root/chain pointer is set if the next element is PERMANENT
// SYMLINFO chains at the beginning are reserved for table info

/* the global symbol pool is a type INT matrix                             */
/* the number of columns is symcol=ceiling(sizeof(L)/sizeof(I))            */
/* elements are interpreted per type L (see jtype.h)                       */
/* a linked list of free entries is kept using the next pointer                  */
// JT(jt,symp):     symbol pool array fixed rank 0, holding L entries (each 3 Is (4 in 32-bit))
// SYMORIGIN    symbol pool array base.  First ele is base of free chain
// AM(JT(jt,symp)) symbol table index (monotonically increasing) - not really needed


/* numbered locales:                                                       */
// JT(jt,stnum)   A block, data is hashtable holding symtab pointer or 0.  Fixed rank 0
// AN(JT(jt,stnum)) size of hashtable in entries (each 1 L*)
// AS(JT(jt,stnum))[1] next loc# to allocate
// AM(JT(jt,stnum)) number of entries in use in table

/* named locales:                                                          */
/* JT(jt,stloc):  locales symbol table                                        */

// within private locales, AK points to the global symbol table when the entity started, and AM is a chain field
// to the local table in the calling explicit definition
//
// within named/numbered locales, AK points to the path (a list of boxes, using AAV0, pointing to the SYMB blocks of the path)
// and AM is the Bloom filter for the locale.


#define symcol ((sizeof(L)+SZI-1)/SZI)

// extend global symbol table.  Called under system lock
A jtsymext(J jt){A x,y;I j,m,n,*v,xn,yn;L*u;
 if(SYMORIGIN!=0){y=(A)((I)SYMORIGIN-AKXR(0)); j=allosize(y)+NORMAH*SZI; yn=AN(y); n=yn/symcol;}  // .  Get header addr by backing off offset of LAV0; extract allo size from header (approx)  yn=#Is in old allo
 else {            j=((I)1)<<12;                  yn=0; n=1;   }  // n is # rows in chain base + old values
 m=j<<1;                     // new size in bytes - 2 * old size
 m-=AKXR(0);                // m is now amount to allo to keep total byte size indicated by j<<1
 m/=symcol*SZI;              // round m to # LX entries we can fit
 xn=m*symcol;             // xn=#Is to allo
 GATV0(x,INT,xn,0); v=(I*)LAV0(x);    // allo the array; v->new symbol 0
 if(SYMORIGIN!=0)ICPY(v,LAV0(y),yn);             // if extension, copy old data to new block
 mvc(SZI*(xn-yn),v+yn,MEMSET00LEN,MEMSET00);               /* 0 unused area for safety  kludge  */
 // dice the added area into symbols, chain them together, add to free chain
 u=n+(L*)v; j=1+n;    // u->start of new area  j=sym# of (1st new sym+1), will always chain each symbol to the next
DQ(m-n-1, u++->next=(LX)(j++););    // for each new symbol except the last, install chain.  Leave last chain 0
 if(SYMORIGIN!=0){u->next=SYMGLOBALROOT; fa(y);}   // if there is an old chain, transfer it to the end of the new chain, then free the old area
 ACINITZAP(x); SYMORIGIN=LAV0(x);           // preserve new array and switch to it
 SYMGLOBALROOT=(LX)n;  // start the new free chain with the first added ele
 R (A)1;
}    /* 0: initialize (no old array); 1: extend old array */

// Make sure there are n symbols available for allocation.  Extend the symbol table if not.
// This must be called outside of any lock and only when the local free-symbol queue has fewer than n values
I jtreservesym(J jt,I n){L *sympv=SYMORIGIN;// start of symbol block
 // add the overflow chain to the main chain, if it is not empty
 if(SYMNEXT(jt->symfreehead[1])!=0){
  sympv[SYMNEXT(jt->symfreetail1)].next=SYMNEXT(jt->symfreehead[0]); jt->symfreehead[0]=jt->symfreehead[1];  // install overflow at start of main
  jt->symfreect[0]+=jt->symfreect[1]; jt->symfreehead[1]=0; jt->symfreect[1]=0;  // move symbol count to main, mark overflow empty
 }
 // loop till we get the number of symbols required
 if(jt->symfreect[0]>=n)R 1;  // if the overflow gave us enough, we are done
 while(1){
  // count off symbols from the global area, up to a fair number (we need to get enough to justify the lock overhead)
  I ninlock; I nwanted=MAX(100,n-jt->symfreect[0]);  // number taken fro shared, number we would like to get
  WRITELOCK(JT(jt,symlock))
  LX sprev;    // sym# of a symbol in the chain, starts at the symbol holding SYMGLOBALROOT
  NOUNROLL for(ninlock=0, sprev=0;ninlock<nwanted&&SYMNEXT(sympv[sprev].next);++ninlock,sprev=SYMNEXT(sympv[sprev].next));  // ninlock counts symbols; at end sprev points to a valid one (unless chain is empty)
  if(likely(ninlock!=0)){  // if the global chain is not empty...
   // transfer what we got to our local table
   LX localhead=SYMNEXT(jt->symfreehead[0]);   // start of the local chain
   jt->symfreehead[0]=SYMGLOBALROOT;   // make the new symbols the head of the local chain
   SYMGLOBALROOT=SYMNEXT(sympv[sprev].next);  // restore the rest of the global symbols to the global chain
   sympv[sprev].next=localhead;  // restore our preexisting local symbols to the local chain
  }
  WRITEUNLOCK(JT(jt,symlock))
  // if we didn't get enough, call a system lock and extend/relocate the table
  if((jt->symfreect[0]+=ninlock)>=n)break;  // incr total symbols in main; success if we got enough
  RZ(jtsystemlock(jt,LOCKPRISYM,jtsymext))  // 
  sympv=SYMORIGIN;  // refresh pointer after system lock
 }
 R 1;
}

#define SYMTABMAINFULL 100   // when the main table has this many frees, put new frees on overflow
#define SYMTABOFLOFULL 200   // when the overflow has this many frees, return them to the shared area
// free the chain of symbols starting with h, ending with t, which contans n symbols.  The chain of t is garbage.
// we put them on the main chain unless that is already overfull; in that case we add to the overflow.  If the overflow
// becomes too big, move it to the shared symbol table (under lock)
void jtsymreturn(J jt, LX h, LX t, I n){L *sympv=SYMORIGIN;  // base of symbol table
 I qno=(jt->symfreect[0]>SYMTABMAINFULL);  // chain number to add to
 I qhead=SYMNEXT(jt->symfreehead[qno]); I qct=jt->symfreect[qno]+=n;  // fetch selected queue head pointer, and the updated count in the selected queue
 sympv[SYMNEXT(t)].next=qhead; jt->symfreehead[qno]=SYMNEXT(h);  // install the blocks-to-free at the head of the selected queue
 if(unlikely(qno==1)){  // if we are adding to the overflow...
  if(unlikely(qhead==0))jt->symfreetail1=SYMNEXT(t);  // if the overflow was empty, remember where it ends
  if(qct>SYMTABOFLOFULL){
   // the overflow filled up.  Transfer it to the shared area.  The caller might have a lock on a hashchain but never on the symbol table
   WRITELOCK(JT(jt,symlock))
   sympv[jt->symfreetail1].next=SYMGLOBALROOT; SYMGLOBALROOT=SYMNEXT(h);  // we must have added to overflow; move the entire overflow to head of shared area
   jt->symfreehead[1]=0; jt->symfreect[1]=0;  // mark the overflow as empty
   WRITEUNLOCK(JT(jt,symlock))
  }
 }
}

// hv->hashtable slot; allocate new symbol, install as head/tail of hash chain, with previous chain appended
// if SYMNEXT(tailx)==0, append at head (immediately after *hv); if SYMNEXT(tailx)!=0, append after tailx.  If queue is empty, tailx is always 0
// The stored chain pointer to the new record is given the non-PERMANENT status from the sign of tailx
// result is new symbol
// Caller must ensure, by prior use of SYMRESERVE, that the symbol is available in the main chain. This routine takes no locks
L* jtsymnew(J jt,LX*hv, LX tailx){LX j;L*u,*v;
 j=SYMNEXT(jt->symfreehead[0]); jt->symfreehead[0]=SYMORIGIN[j].next; --jt->symfreect[0];  // remove symbol from list & account for it
 u=j+SYMORIGIN;  // the new symbol.  u points to it, j is its index
 if(likely(SYMNEXT(tailx)!=0)) {L *t=SYMNEXT(tailx)+SYMORIGIN;
  // appending to tail, must be a symbol.  Queue is known to be nonempty
  u->next=t->next;t->next=j|(tailx&SYMNONPERM);  // it's always the end: point to next & prev, and chain from prev.  Everything added here is non-PERMANENT
 }else{
  // appending to head.
  u->next=*hv;  // chain old queue to u
  *hv=j|(tailx);   // set new head, flagged as NONPERM unless suppressed
 }
 R u;
}    /* allocate a new pool entry and insert into hash table entry hv */

// free all the symbols in symbol table w.  As long as the symbols are PERMANENT, delete the values but not the name.
// For non-PERMANENT, delete name and value.
// Reset the fields in the deleted blocks.
// This is used only for freeing local symbol tables, thus does not need to clear the name/path or worry about CACHED values
extern void jtsymfreeha(J jt, A w){I j,wn=AN(w); LX k,* RESTRICT wv=LXAV0(w);
 LX freeroot=0; L *freetailchn=(L*)((I)jt->shapesink-offsetof(L,next));  // sym index of first freed ele; addr of chain field in last freed ele
 L *jtsympv=SYMORIGIN;  // Move base of symbol block to a register.  Block 0 is the base of the free chain.  MUST NOT move the base of the free queue to a register,
  // because when we free an explicit locale it frees its symbols here, and one of them might be a verb that contains a nested SYMB, giving recursion.  It is safe to move sympv to a register because
  // we know there will be no allocations during the free process.
 // loop through each hash chain, clearing the blocks in the chain.  Do not clear chain 0, which holds x/y bucket numbers
 I nfreed=0;  // count of # blocks freed
 LX lastk;  // last block freed, valid only if something was freed
 for(j=SYMLINFOSIZE;j<wn;++j){
  LX *aprev=&wv[j];  // this points to the predecessor of the last block we processed
  // process the chain
  if(k=*aprev){  // process only chains with values
   // first, free the PERMANENT values (if any), but not the names
   NOUNROLL do{
    if(!SYMNEXTISPERM(k))break;  // we are about to free k.  exit if it is not permanent
    I nextk=jtsympv[k].next;  // unroll loop 1 time
    aprev=&jtsympv[k].next;  // save last item we processed here
    if(jtsympv[k].val){
     // if the value was abandoned to an explicit definition, we took usecount 8..1  -> 1 ; revert that.  Can't change an ACPERMANENT!
     // otherwise decrement the usecount
     SYMVALFA(jtsympv[k]);
     jtsympv[k].val=0;jtsympv[k].valtype=0;  // clear value - don't clear name
    }
    k=nextk;
   }while(k);
   // clear chain in last PERMANENT block
   *aprev=0;  // only the PERMANENT survive
   // We are now pointing at the first non-permanent, if any.  Erase them all, deleting the name and value
   if(k){
    LX k1=SYMNEXT(k);  // remember first non-PERMANENT
    freeroot=freeroot?freeroot:k1;  // remember overall first value
    NOUNROLL do{
     k=SYMNEXT(k);  // remove address flagging
     I nextk=jtsympv[k].next;  // unroll loop once
     fa(jtsympv[k].name);fa(jtsympv[k].val);jtsympv[k].name=0;jtsympv[k].valtype=0;jtsympv[k].val=0;jtsympv[k].sn=0;jtsympv[k].flag=0;
     lastk=k;  // remember index of last block
     ++nfreed;  // ince count of block in chain-to-free
     k=nextk;
    }while(k);
    // make the non-PERMANENTs the base of the free pool & chain previous pool from them
    freetailchn->next=k1; freetailchn=&jtsympv[lastk];  // free chain may have permanent flags
   }
  }
 }
 if(likely(freeroot!=0)){jtsymreturn(jt,freeroot,lastk,nfreed);}  // put all blocks freed here onto the free chain
}

static SYMWALK(jtsympoola, I,INT,100,1, 1, *zv++=j;)

F1(jtsympool){A aa,q,x,y,*yv,z,zz=0,*zv;I i,n,*u,*xv;L*pv;LX j,*v;
 ARGCHK1(w); 
 ASSERT(1==AR(w),EVRANK); 
 ASSERT(!AN(w),EVLENGTH);
 READLOCK(JT(jt,stlock)) READLOCK(JT(jt,stloc)->lock) READLOCK(JT(jt,symlock))
 GAT0E(z,BOX,4,1,goto exit;); zv=AAV1(z);
 n=AN((A)((I)SYMORIGIN-AKXR(0)))/symcol; pv=SYMORIGIN;
 GATV0E(x,INT,n*6,2,goto exit;); AS(x)[0]=n; AS(x)[1]=6; xv= AV2(x); zv[0]=incorp(x);  // box 0: sym info
 GATV0E(y,BOX,n,  1,goto exit;);                         yv=AAV1(y); zv[1]=incorp(y);  // box 1: 
 for(i=0;i<n;++i,++pv){         /* per pool entry       */
  *xv++=i;   // sym number
  *xv++=(!(pv->flag&LINFO)&&pv->val)?LOWESTBIT(AT(pv->val)):0;  // type: only the lowest bit.  In LINFO, val may be locale#.  Must allow SYMB through
  *xv++=pv->flag+(pv->name?LHASNAME:0)+(!(pv->flag&LINFO)&&pv->val?LHASVALUE:0);  // flag
  *xv++=pv->sn;    // script index
  *xv++=SYMNEXT(pv->next);  // chain
  *xv++=0;  // for debug, the thread# that allocated the symbol
  RZGOTO(*yv++=(q=pv->name)?incorp(sfn(SFNSIMPLEONLY,q)):mtv,exit);  // simple name
 }
 // Allocate box 2: locale name
 GATV0E(y,BOX,n,1,goto exit;); yv=AAV1(y); zv[2]=incorp(y);
 DO(n, yv[i]=mtv;);
 n=AN(JT(jt,stloc)); v=LXAV0(JT(jt,stloc));   // v->locale chains
 for(i=0;i<n;++i){  // for each chain-base in locales pool
  for(j=v[i];j=SYMNEXT(j),j;j=SYMORIGIN[j].next){      // j is index to named local entry; process the chain
   x=SYMORIGIN[j].val;  // x->symbol table for locale
   RZGOTO(yv[j]=yv[LXAV0(x)[0]]=aa=incorp(sfn(SFNSIMPLEONLY,LOCNAME(x))),exit);  // install name in the entry for the locale
   RZGOTO(q=sympoola(x),exit); u=AV(q); DO(AN(q), yv[u[i]]=aa;);
  }
 }
 n=jtcountnl(jt);
 for(i=0;i<n;++i)if(x=jtindexnl(jt,i)){   /* per numbered locales */
  RZGOTO(      yv[LXAV0(x)[0]]=aa=incorp(sfn(SFNSIMPLEONLY,LOCNAME(x))),exit);
  RZGOTO(q=sympoola(x),exit); u=AV(q); DO(AN(q), yv[u[i]]=aa;);
 }
 if(EXPLICITRUNNING){               /* per local table      */
  RZGOTO(aa=incorp(cstr("**local**")),exit);
  RZGOTO(q=sympoola(jt->locsyms),exit); u=AV(q); DO(AN(q), yv[u[i]]=aa;);
 }
 // box 3: # free symbols for each thread
 GATV0E(x,INT,JT(jt,wthreadhwmk)+1,1,goto exit;); xv=AV1(x); zv[3]=incorp(x);  // box 0: sym info
 DO(JT(jt,wthreadhwmk)+1, J jt0=JTFORTHREAD(jt,i); I nfreesym=0; DO(2, for(j=jt0->symfreehead[i];j=SYMNEXT(j),j;j=SYMORIGIN[j].next)++nfreesym;) xv[i]=nfreesym;)
 zz=z;
exit: ;
 READUNLOCK(JT(jt,stlock)) READUNLOCK(JT(jt,stloc)->lock) READUNLOCK(JT(jt,symlock))
 RETF(zz);
}    /* 18!:_2 symbol pool */

// l/string are length/addr of name, hash is hash of the name, g is symbol table  l is encoded in low bits of jt
// the symbol is deleted if found.  Return 1 if the value was an ACV
// if the symbol is PERMANENT, it is not deleted but its value is removed
// if the symbol is CACHED, it is removed from the chain but otherwise untouched, leaving the symbol abandoned.  It is the caller's responsibility to handle the name
// We take no locks on g.  They are necessary, but are the user's responsibility
B jtprobedel(J jt,C*string,UI4 hash,A g){B ret;
 F1PREFIP;
 L *sympv=SYMORIGIN;  // base of symbol pool
 LX *asymx=LXAV0(g)+SYMHASH(hash,AN(g)-SYMLINFOSIZE);  // get pointer to index of start of chain; address of previous symbol in chain
 LX delblockx=*asymx;
 while(1){
  delblockx=SYMNEXT(delblockx);
  if(!delblockx){ret=0; break;}  // if chain empty or ended, not found
  L *sym=sympv+delblockx;  // address of next in chain, before we delete it
  LX nextdelblockx=sym->next;  // unroll loop once
  if(likely(!(AFLAG(sym->name)&AFRO))){   // ignore request to delete readonly name (cocurrent)
   IFCMPNAME(NAV(sym->name),string,(I)jtinplace&0xff,hash,     // (1) exact match - if there is a value, use this slot, else say not found
     {
      ret=sym->val==0?0:~(I)sym->valtype&QCNOUN;  // return value: value was defined & not a noun
      SYMVALFA(*sym); sym->val=0; sym->valtype=0;  // decr usecount in value; remove value from symbol
      if(!(sym->flag&LPERMANENT)){  // if PERMANENT, we delete only the value
       *asymx=sym->next; fa(sym->name); sym->name=0; sym->flag=0; sym->sn=0;    // unhook symbol from hashchain, free the name, clear the symbol
       jtsymreturn(jt,delblockx,delblockx,1);  // return symbol to free chains
      }  // add to symbol free list
      break;  // name match - return
     }
   // if match, bend predecessor around deleted block, return address of match (now deleted but still points to value)
   )
  } 
  asymx=&sym->next;   // mismatch - step to next
  delblockx=nextdelblockx;
 }
 R ret;
}

// l/string are length/addr of name, hash is hash of the name, g is symbol table.  l is encoded in low bits of jt
// result is addr/global/flags for name (i. e. QCGLOBAL semantics), or 0 if not found
// locking is the responsibility of the caller
A jtprobe(J jt,C*string,UI4 hash,A g){
 RZ(g);
 F2PREFIP;
 LX symx=LXAV0(g)[SYMHASH(hash,AN(g)-SYMLINFOSIZE)];  // get index of start of chain
 L *sympv=SYMORIGIN;  // base of symbol table
 L *symnext, *sym=sympv+SYMNEXT(symx);  // first symbol address - might be the free root if symx is 0
 NOUNROLL while(symx){  // loop is unrolled 1 time
  // sym is the symbol to process, symx is its index.  Start by reading next in chain.  One overread is OK, will be symbol 0 (the root of the freequeue)
  symnext=sympv+(symx=SYMNEXT(sym->next));
  IFCMPNAME(NAV(sym->name),string,(I)jtinplace&0xff,hash,R (A)((I)sym->val+sym->valtype);)     // (1) exact match - if there is a value, return it.  valtype has QCGLOBAL semantics
  sym=symnext;  // advance to value we read
 }
 R 0;  // not found
}

// a is A for name; result is addr/0/flags for name (i. e. QCGLOBAL semantics), or 0 if not found
// If the value is empty, return 0 for not found
// We know that there are buckets and that we should search them
// Take no locks
A probelocalbuckets(L *sympv,A a,LX lx,I bx){NM*u;   // lx is LXAV0(locsyms)[bucket#], bx is index within bucket
 // There is always a local symbol table, but it may be empty (performance is immaterial then)
 RZQ(a);u=NAV(a);  // u->NM block
 if(0 > (bx = ~bx)){
  // positive bucketx (now negative); that means skip that many items and then do name search.  This is set for words that were recognized as names but were not detected as assigned-to in the definition
  // we know we have tested for NAMEADDED; the normal path here is probably a failed search for a global.  recursive tables also come here
  I m=u->m; C* s=u->s; UI4 hsh=u->hash; // length/addr of name from name block, and hash
  if(unlikely(++bx!=0)){NOUNROLL do{lx = sympv[lx].next;}while(++bx);}  // rattle off the permanents, usually 1
  // Now lx is the index of the first name that might match.  Do the compares
  NOUNROLL while(lx=SYMNEXT(lx)) {L* l = lx+sympv;  // symbol entry
   IFCMPNAME(NAV(l->name),s,m,hsh, R (A)((I)l->val+l->valtype);)
   lx = l->next;
  }
  R 0;  // no match.
 } else {
  L* l = lx+sympv;  // fetch hashchain headptr, point to L for first symbol
  // negative bucketx (now positive); skip that many items, and then you're at the right place
  if(unlikely(bx>0)){NOUNROLL do{l = l->next+sympv;}while(--bx);}  // skip the prescribed number, which is usually 1
  R (A)((I)l->val+l->valtype);
 }
}

// a is A for name; result is addr/0/flags for name (i. e. QCGLOBAL semantics), or 0 if not found
// If the value is empty, return 0 for not found
// Take no locks because local
// Use buckets if present: was are called only if buckets will be valid, i. e. 
A jtprobelocal(J jt,A a,A locsyms){NM*u;I b,bx;
 // There is always a local symbol table, but it may be empty
 ARGCHK1(a);u=NAV(a);  // u->NM block
 if(likely((b = u->bucket)!=0)){
  R probelocalbuckets(SYMORIGIN,a,LXAV0(locsyms)[b],u->bucketx);  // look up using bucket info
 }else{
  // No bucket information, do full search.  This includes names that don't come straight from words in an explicit definition
  R jtprobe((J)((I)jt+NAV(a)->m),NAV(a)->s,NAV(a)->hash,locsyms);
 }
}

// a is A for name; result is L* address of the symbol-table entry in the local symbol table lsym (which must exist)
// If not found, one is created
// We know that the name block DOES NOT have a direct symbol number, because we have checked that if there was a chance (if the name block was synthetic there is no chance)
// May call probeis which takes a lock on the local table; if so we release the lock
L *jtprobeislocal(J jt,A a,A lsym){NM*u;I bx;L *sympv=SYMORIGIN;
 // If there is bucket information, there must be a local symbol table, so search it
 ARGCHK1(a);u=NAV(a);  // u->NM block
 I4 b=u->bucket;
 if((likely(b!=0))){
  LX lx = LXAV0(lsym)[b];  // index of first block if any
  if(unlikely(0 > (bx = ~u->bucketx))){
   // positive bucketx (now negative); that means skip that many items and then do name search
   // Even if we know there have been no names assigned we have to spin to the end of the chain (for insertion purposes)
   // We don't unroll these loops because there is usually one symbol per bucket
   I m=u->m; C* s=u->s; UI4 hsh=u->hash;  // length/addr of name from name block, and hash
   LX tx = lx;  // tx will hold the address of the last item in the chain, in case we have to add a new symbol
   L* l;
   NOUNROLL while(0>++bx){tx = lx; lx = sympv[lx].next;}  // all permanent
   // Now lx is the index of the first name that might match.  Do the compares
   NOUNROLL while(lx=SYMNEXT(lx)) {
    l = lx+sympv;  // symbol entry
    IFCMPNAME(NAV(l->name),s,m,hsh,R l;)
    tx = lx; lx = l->next;
   }
   // not found, create new symbol.  If tx is 0, the queue is empty, so adding at the head is OK; otherwise add after tx.  Make it non-PERMANENT
   SYMRESERVE(1) l=symnew(&LXAV0(lsym)[b],tx|SYMNONPERM); 
   ra(a); l->name=a;  // point symbol table to the name block, and increment its use count accordingly
   AR(lsym)|=ARNAMEADDED;  // Mark that a name has been added beyond what was known at preprocessing time
   R l;
  } else {L* l = lx+sympv;  // fetch hashchain headptr, point to L for first symbol
   // negative bucketx (now positive); skip that many items, and then you're at the right place
   NOUNROLL while(bx--){l = l->next+sympv;}  // all permanent
   R l;  // return 
  }
 } else {
  // No bucket information, do full search. We do have to reserve a symbol in case the name is new
  // We don't need a lock, because this is a local table; but this path is rare - only for computed names, and for assignments
  // during creation of the local symbol tables, where we will keep the lock once we take it
  SYMRESERVE(1) L *l=probeis(a,lsym); WRITEUNLOCK(lsym->lock);  // release the unneeded lock
  RZ(l);
  AR(lsym)|=((~l->flag)&LPERMANENT)<<(ARNAMEADDEDX-LPERMANENTX);  // Mark that a name has been added beyond what was known at preprocessing time, if the added name is not PERMANENT
  R l;
 }
}

// Acquire a symbol and then xctl to probeis.  Suitable when the caller needs a symbol AND has no locks
// May fail if symbol cannot be allocated.  takes and releases a write lock on the symbol table
L* jtprobeisres(J jt,A a,A g){SYMRESERVE(1) L *z=probeis(a,g); WRITEUNLOCK(g->lock); R z;}

// a is A for name
// g is symbol table to use
// result is L* symbol-table entry to use; cannot fail, because symbol has been reserved
// if not found, one is created.  Caller must ensure that a symbol is reserved
// Takes a write lock on g and returns holding that lock
L*jtprobeis(J jt,A a,A g){C*s;LX tx;I m;L*v;NM*u;L *sympv=SYMORIGIN;
 u=NAV(a); m=u->m; s=u->s; UI4 hsh=u->hash;  // m=length of name  s->name  hsh=hash of name
 LX *hv=LXAV0(g)+SYMHASH(hsh,AN(g)-SYMLINFOSIZE);  // get hashchain base among the hash tables
 WRITELOCK(g->lock);  // write-lock the table before we access it
 if(tx=SYMNEXT(*hv)){                                 /* !*hv means (0) empty slot    */
  v=tx+sympv;
  NOUNROLL while(1){
   LX lxnext=v->next;  // unroll loop once
   u=NAV(v->name);  // name cannot be 0
   IFCMPNAME(u,s,m,hsh,R v;)    // (1) exact match - may or may not have value
   if(!lxnext)break;                                /* (2) link list end */
   v=(tx=SYMNEXT(lxnext))+sympv;
  }
 }
 // not found, create new symbol.  If tx is 0, the queue is empty, so adding at the head is OK; otherwise add after tx
 v=symnew(hv,tx|SYMNONPERM);   // symbol is non-PERMANENT and known to be available
 ra(a); v->name=a;  // point symbol table to the name block, and increment its use count accordingly
 R v;
}    /* probe for assignment */

// look up a non-locative name using the locale path
// g is the current locale, l/string=length/name, hash is the hash for it (l is carried in the low 8 bits of jt)
// result is  result is addr/named/flags for name (i. e. QCNAMED semantics), or 0 if not found
// Bit QCNAMED of the result is set iff the name was found in a named locale
// We must have no locks coming in; we take a read lock on each symbol table we have to search
// if we find a name, we ra() it under lock.  All we have to do is increment the name since it is known to be recursive if possible
A jtsyrd1(J jt,C *string,UI4 hash,A g){A*v,x,y;
 RZ(g);  // make sure there is a locale...
 // we store an extra 0 at the end of the path to allow us to unroll this loop once
 I bloom=BLOOMMASK(hash); v=LOCPATH(g);
 // This function is called after local symbols have been found wanting.  Usually g will be the base
 // of the implied path.  But if the value is a locative, g is the locative locale to start in, and
 // that might be a local table if name___1 is used.  We hereby define that ___1 searches only in
 // the local table, not the path; and we have to disable the Bloom filter because local tables don't have
 // one.
 if(unlikely(AR(g)&ARLOCALTABLE)){bloom=0; v=(A*)&iotavec-IOTAVECBEGIN+0;}  // no bloom, empty path
 NOUNROLL do{A gn=*v--; if((bloom&~LOCBLOOM(g))==0){READLOCK(g->lock) A res=jtprobe(jt,string,hash,g);
                         if(res){raposgblqcgsv(QCWORD(res),QCPTYPE(res),res);
#ifdef PDEP
                         res=(A)(((I)res&~QCNAMED)+PDEP((I)AR(g)>>ARNAMEDX,(I)1<<(QCNAMEDX-ARNAMEDX)));
#else
                         res=(A)(((I)res&~QCNAMED)+(((I)AR(g)&ARNAMED)<<(QCNAMEDX-ARNAMEDX)));
#endif
                         READUNLOCK(g->lock) R res;}  // change QCGLOBAL semantics to QCNAMED
                         READUNLOCK(g->lock)} g=gn;
            }while(g);  // return when name found.
 R 0;  // fall through: not found
}    /* find name a where the current locale is g */ 
// same, but return the locale in which the name is found, and no ra().  Takes readlock on searched locales.  Return 0 if not found
A jtsyrd1forlocale(J jt,C *string,UI4 hash,A g){
 RZ(g);  // make sure there is a locale...
 I bloom=BLOOMMASK(hash); A *v=LOCPATH(g); NOUNROLL do{A gn=*v--; A y; if((bloom&~LOCBLOOM(g))==0){READLOCK(g->lock) y=jtprobe(jt,string,hash,g); READUNLOCK(g->lock) if(y){break;}} g=gn;}while(g);  // return when name found.
 R g;
}


// u is address of indirect locative: in a__b__c, it points to the b (the direct part of the name is not used)
// n is the length of the entire locative (4 in this example)
// result is address of symbol table to use for name lookup (if not found, it is created)
static A jtlocindirect(J jt,I n,C*u,I hash){A x;C*s,*v,*xv;I k,xn;
 A g=0;  // the locale we are looking in, as we go right to left through the a__b__c... chain. 0 means 'locals then globals', otherwise exact locale to search in
 A y;  //  resolved A block for __x.  This is the name of a locale which will be found & put into g
 s=n+u;   // s->end+1 of name, past the last locative
 while(u<s){
  v=s; NOUNROLL while('_'!=*--v); ++v;  // v->start of last indirect locative
  k=s-v; s=v-2;    // k=length of indirect locative; s->end+1 of next name if any
  ASSERT(k<256,EVLIMIT);
  if(likely(!BETWEENC(v[0],'0','9'))){  // is normal name?
   if(likely(g==0)){  // first time through
    y=QCWORD(jtprobe((J)((I)jt+k),v,(UI4)hash,jt->locsyms));  // look up local first.
    if(y==0)y=QCWORD(jtsyrd1((J)((I)jt+k),v,(UI4)hash,jt->global));else{rapos(y,y);}  // if not local, start in implied locale.  ra to match syrd
   }else y=QCWORD(jtsyrd1((J)((I)jt+k),v,(UI4)nmhash(k,v),g));   // look up later indirect locatives, yielding an A block for a locative
   ASSERTN(y,EVVALUE,nfs(k,v));  // verify found.  If y was found, it has been ra()d
   ASSERTNGOTO(!AR(y),EVRANK,nfs(k,v),exitfa);   // verify atomic
   if(AT(y)&(INT|B01)){  // atomic integer, numbered or debug locale
    hash=BIV0(y);   // fetch locale number, overwriting the input parameter as needed below
    if(hash<0){ASSERT(g==0,EVLOCALE) goto neglocnum;}   // negative locale number (debug frame) is valid only as last locale.  Transfer to the code to handle it
    g=findnl(hash); ASSERTGOTO(g!=0,EVLOCALE,exitfa);  // nonnegative locale#, use it for the numbered locale
   }else{
    ASSERTNGOTO(BOX&AT(y),EVDOMAIN,nfs(k,v),exitfa);  // verify box
    x=C(AAV(y)[0]); if((((I)AR(x)-1)&-(AT(x)&(INT|B01)))<0) {
     // Boxed integer - use that as bucketx, the locale number
     g=findnl(BIV0(x)); ASSERTGOTO(g!=0,EVLOCALE,exitfa);  // boxed integer, look it up
    }else{
     xn=AN(x); xv=CAV(x);   // x->boxed contents, xn=length, xv->string
     ASSERTNGOTO(1>=AR(x),EVRANK,nfs(k,v),exitfa);   // verify list (or atom)
     ASSERTNGOTO(xn,EVLENGTH,nfs(k,v),exitfa);   // verify not empty
     ASSERTNGOTO(LIT&AT(x),EVDOMAIN,nfs(k,v),exitfa);  // verify string
     ASSERTNGOTO(vlocnm(xn,xv),EVILNAME,nfs(k,v),exitfa);  // verify legal name
     I bucketx=BUCKETXLOC(xn,xv);
     RZGOTO(g=stfindcre(xn,xv,bucketx),exitfa);  // find st for the name
    }
   }
  }else{DC s; 
   // the 'name' is a number (it must be the last name).  It refers to debug stack frames, the first of which is numbered _1.  hash has the value of the number
   // To avoid uncertainty as new frames are created, frames before the top suspended frame are ignored
   ASSERT(hash<0,EVLOCALE)    // if index not now negative, it was too high
neglocnum:;
   I issusp;  //   issusp='we have hit a suspension frame'
   for(s=jt->sitop,issusp=0;s;s=s->dclnk){issusp|=s->dcsusp; if(issusp&&s->dctype==DCCALL&&hash==-1)break; hash+=issusp&&s->dctype==DCCALL;} ASSERT(s,EVLOCALE); // skip to suspension; step to requested stack frame; error if # too low
   g=s->dcloc;  // fetch locale to use for the lookup
  }
 }
 R g;
exitfa: ;
 fa(y);
 R 0;
}

// a is a locative NAME block; result is the starting locale, or 0 if error
// if the locative is direct we just look up/create the locale with that name; if indirect we find the value, then look up that locale
A jtsybaseloc(J jt,A a) {I m,n;NM*v;
 n=AN(a); v=NAV(a); m=v->m;
 // Locative: find the indirect locale to start on, or the named locale, creating the locale if not found
 if(likely(!(NMILOC&v->flag)))R stfindcre(n-m-2,1+m+v->s,v->bucketx);
 R locindirect(n-m-2,2+m+v->s,v->bucketx);
}

// look up a name (either simple or locative) using the full name resolution
// result is addr/global/flags for name (i. e. QCGLOBAL semantics), or 0 if not found (no error on undefname)
// If the block is found, the value has been ra()d
A jtsyrd(J jt,A a,A locsyms){A g;
 ARGCHK1(a);
 if(likely(!(NAV(a)->flag&(NMLOC|NMILOC)))){A val;
  // If there is a local symbol table, search it first
  if(val=jtprobe((J)((I)jt+NAV(a)->m),NAV(a)->s,NAV(a)->hash,locsyms)){rapos(QCWORD(val),val); R val;}  // return flagging the result if local.  Value pointers in symbols have QCGLOBAL semantics
  g=jt->global;  // Continue with the current locale
 }else{A val;  // locative
  RZ(g=sybaseloc(a));  // find the starting locale for the name lookup
  if(unlikely(AR(g)&ARLOCALTABLE)){if(val=jtprobe((J)((I)jt+NAV(a)->m),NAV(a)->s,NAV(a)->hash,g)){rapos(QCWORD(val),val); R val;} g=AKGST(g);}  // if locative ended with a local table, it must be from debug locative __nn.  Search as local first to avoid Bloom filter, then pick up with that frame's globals
 }
 A res=jtsyrd1((J)((I)jt+NAV(a)->m),NAV(a)->s,NAV(a)->hash,g);  // Not local: look up the name starting in locale g
 if(likely(res!=0))res=SETGLOBAL(res);  // mark found in global, if found
 R res;
}
// same, but return locale in which found.  No ra(), 0 if symbol not found.  Takes & releases readlock on searched locales
A jtsyrdforlocale(J jt,A a){A g;
 ARGCHK1(a);
 if(likely(!(NAV(a)->flag&(NMLOC|NMILOC)))){
  // If there is a local symbol table, search it first
  if(jtprobe((J)((I)jt+NAV(a)->m),NAV(a)->s,NAV(a)->hash,jt->locsyms)){R jt->locsyms;}  // return flagging the result if local
  g=jt->global;  // Start with the current locale
 } else RZ(g=sybaseloc(a));
 R jtsyrd1forlocale((J)((I)jt+NAV(a)->m),NAV(a)->s,NAV(a)->hash,g);  // Not local: look up the name starting in locale g
}
// same as syrd, but we have already checked for buckets
// look up a name (either simple or locative) using the full name resolution
// result is addr/global/flags for name (i. e. QCGLOBAL semantics), or 0 if not found
// If the name/value are found, ra() the value
A jtsyrdnobuckets(J jt,A a){A g,res;
 ARGCHK1(a);
 I fndtblflg;  //  in the rare case of n___1, we have to look to see if the name was local
 if(likely(!(NAV(a)->flag&(NMLOC|NMILOC)))){
  // If there is a local symbol table, search it first - but only if there is no bucket info.  If there is bucket info we have checked already
  if(unlikely(!NAV(a)->bucket))if(res=jtprobe((J)((I)jt+NAV(a)->m),NAV(a)->s,NAV(a)->hash,jt->locsyms)){rapos(QCWORD(res),res); R res;}  // return if found locally from name
  g=jt->global;  // Start with the current locale
  fndtblflg=0;  // through this path only global tables are found
 } else{RZ(g=sybaseloc(a)); fndtblflg=AR(g);}  // if locative, start in locative locale & remember table type
 res=jtsyrd1((J)((I)jt+NAV(a)->m),NAV(a)->s,NAV(a)->hash,g);  // Not local: look up the name starting in locale g
 if(likely(res!=0))if(likely(!(fndtblflg&ARLOCALTABLE)))res=SETGLOBAL(res);  // mark found in global, if found, which switches to QCGLOBAL semantics
 R res;
}

// return symbol address for name, or 0 if not found
static L *jtprobeforsym(J jt,C*string,UI4 hash,A g){
 RZ(g);
 F2PREFIP;
 LX symx=LXAV0(g)[SYMHASH(hash,AN(g)-SYMLINFOSIZE)];  // get index of start of chain
 L *sympv=SYMORIGIN;  // base of symbol table
 L *symnext, *sym=sympv+SYMNEXT(symx);  // first symbol address - might be the free root if symx is 0
 NOUNROLL while(symx){  // loop is unrolled 1 time
  // sym is the symbol to process, symx is its index.  Start by reading next in chain.  One overread is OK, will be symbol 0 (the root of the freequeue)
  symnext=sympv+SYMNEXT(symx=sym->next);
  IFCMPNAME(NAV(sym->name),string,(I)jtinplace&0xff,hash,R sym->val!=0?sym:0;)     // (1) exact match - if there is a value, return the symbol
  sym=symnext;  // advance to value we read
 }
 R 0;  // not found
}

// a is a NAME block.  Look up the name and return the requested component as an I
// component is: 0=&symbol (deprecated) 1=&value data area 2=&value header 3=script number+1.  Result is 0 if name not found or error in locative name
static I jtsyrdinternal(J jt, A a, I component){A g=0;L *l;
 ARGCHK1(a);
 I stringlen=NAV(a)->m; C *string=NAV(a)->s; UI4 hash=NAV(a)->hash;
 if(likely(!(NAV(a)->flag&(NMLOC|NMILOC)))){
  // If there is a local symbol table, search it first
  if(l=jtprobeforsym((J)((I)jt+stringlen),string,hash,jt->locsyms)){goto gotval;}  // return flagging the result if local.  Stored names have QCGLOBAL semantics
  g=jt->global;  // Continue with the current locale
 } else RZ(g=sybaseloc(a));  // look up locative; error possible in name, return 0
 // we store an extra 0 at the end of the path to allow us to unroll this loop once
 I bloom=BLOOMMASK(hash); A *v=LOCPATH(g);
 NOUNROLL do{A gn=*v--; if((bloom&~LOCBLOOM(g))==0){READLOCK(g->lock) l=jtprobeforsym((J)((I)jt+stringlen),string,hash,g); if(l){goto gotval;} READUNLOCK(g->lock)} g=gn;}while(g);  // exit loop when found
 R 0;  // not found, locks released
gotval: ;
 // found: l points to the symbol.  We hold a lock on g, if it is nonzero
 I res=0;
 if(component==0){ASSERTGOTO(NOUN&AT(l->val),EVDOMAIN,exitlock) res=(I)l;}  // 15!:6, symbol address
 else if(component==1){ASSERTGOTO(NOUN&AT(l->val),EVDOMAIN,exitlock) res=(I)voidAV(l->val);}  // 15!:14, data address
 else if(component==2){ASSERTGOTO(NOUN&AT(l->val),EVDOMAIN,exitlock) res=(I)(l->val);}  // 15!:12, header address
 else{res=l->sn+1;}  // 4!:4, script index
exitlock:
 if(g)READUNLOCK(g->lock)
 R res;
}


// w is boxed names, result is integer array of values in the symbol
// component selects the internal variable wanted.  We loop through the names
static A jtdllsymaddr(J jt,A w,C component){A*wv,x,y,z;I i,n,*zv;
 ARGCHK1(w);
 n=AN(w); wv=AAV(w); 
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 I zr=AR(w); GATV(z,INT,n,AR(w),AS(w)); zv=AVn(zr,z); 
 NOUNROLL for(i=0;i<n;++i){
  x=C(wv[i]);
  I val;  // value of requested component
  I nmlen=AN(x);  // length of valid part of name
  if(unlikely((AR(x)&~1)+(component^3)+nmlen==0))val=0;  // special case of script lookup with empty name: could be unnamed stack entry in 13!:13, so give not found rather than name error
  else{
   if(unlikely((AR(x)&~1)+(component^3)==0))RZ(x=take(indexof(x,scc('>')),x));  // script lookup: name =. (name i. '>') {. name
   RE(y=stdnm(x)); ASSERTN(y,EVILNAME,nfs(nmlen,CAV(x))); RESETERR; 
   val=jtsyrdinternal(jt,y,component);
  }
  if(component==3)RESETERR; RE(0);  // if the name lookup failed, exit; but 4!:4 never fails, because used in 13!:13
  ASSERT(component==3||val!=0,EVVALUE);  // error if name not found, for symbol or data address
  zv[i]=val-(component==3);  // undo the increment of script number.  Could use >>1
 }
 RETF(z);
}

F1(jtdllsymget){ASSERT(!JT(jt,seclev),EVSECURE) R dllsymaddr(w,0);}  // 15!:6
F1(jtdllsymdat){ASSERT(!JT(jt,seclev),EVSECURE) R dllsymaddr(w,1);}  // 15!:14
F1(jtdllsymhdr){ASSERT(!JT(jt,seclev),EVSECURE) R dllsymaddr(w,2);}  // 15!:12
F1(jtscind){R dllsymaddr(w,3);}  // 4!:4 script index
F1(jtdllvaladdr){ASSERT(!JT(jt,seclev),EVSECURE) R sc((I)w);}  // 15!:19, return address of header of noun w

// look up the name w using full name resolution.  Return the value if found, abort if not found or invalid name
F1(jtsymbrd){A z; ARGCHK1(w); ASSERTN(z=QCWORD(syrd(w,jt->locsyms)),EVVALUE,w); tpush(z); R z;}   // undo the ra() in syrd

// look up name w, return value unless locked or undefined; then return just the name
F1(jtsymbrdlocknovalerr){A y;
 if(!(y=QCWORD(syrd(w,jt->locsyms)))){
  // no value.  Could be undefined name (no error) or some other error including value error, which means error looking up an indirect locative
  // If error, abort with it; if undefined, return a reference to the undefined name
  RE(0);   // if not simple undefined, error
  R nameref(w,jt->locsyms);  // return reference to undefined name
 }
 // no error.  Return the value unless locked function
 tpush(y);  // undo the ra() in syrd
 R FUNC&AT(y)&&(jt->glock||VLOCK&FAV(y)->flag)?nameref(w,jt->locsyms):y;
}

// Same, but value error if name not defined
F1(jtsymbrdlock){A y;
 RZ(y=symbrd(w));
 R FUNC&AT(y)&&(jt->glock||VLOCK&FAV(y)->flag)?nameref(w,jt->locsyms):y;
}


// w is a value to be assigned, v is the incumbent value in that name
// Called only in debug mode.  If we redefine an executing name with a different value, it would invalidate
// the debug stack.  It is OK to reassign if the value doesn't change.  Fail if any redefinition would change part of speech or the id of the executing function.
// If the currently-executing definition is reloaded, mark the stack entry: xdefn will pick it up when debug is on
// as modified - xdefn will try to hot-swap to the new definition between lines
// If the modified name is executing higher on the stack, fail
// returns v for OK to allow the assignment to proceed, 0 if error
A jtredef(J jt,A w,A v){A f;DC c,d;
 d=jt->sitop; NOUNROLL while(d&&!(DCCALL==d->dctype&&d->dcj))d=d->dclnk; if(!(d&&DCCALL==d->dctype&&d->dcj))R v;   // find the most recent DCCALL with error, exit if none
 // Now d->stack entry of executing entity, which is in suspension at an error
 if(v==(A)d->dcn){  // if we reassign any name whose value equals the executing value, we treat it as a reassignment of the executing name.  This is for comp ease
  // attempted reassignment of the executing name
  // insist that the redefinition have the same type, and the same explicitness
  f=d->dcf;  // the executing function
  if(unlikely(!(TYPESEQ(AT(f),AT(w))&&(FAV(f)->id==CCOLONE)==(FAV(w)->id==CCOLONE)&&(FAV(f)->flag&VXOP+VXOPR)==(FAV(w)->flag&VXOP+VXOPR))))JT(jt,sidamage)=1;  // the executing value MUST have a name, otherwise we couldn't modify it.  If type/id changed, pull the plug
  d->dcf=w; d->dcn=(I)w;  // dcf is used by redef code in xdefn; dcn is the stacked addr of executing fn, which we must now update so we can see if it is changed again later
  // If we are redefining the executing explicit definition during debug, remember that.
  // debug will switch over to the new definition before the next line is executed.
  // Reassignment outside of debug continues executing the old definition
  if(FAV(w)->id==CCOLONE&&!jteqf(jt,w,v)){d->dcredef=1;}  // don't call for redef if value doesn't change in explicit defn
  // Erase any stack entries after the redefined call, except for SCRIPT type which must be preserved for linf
  c=jt->sitop; NOUNROLL while(c&&DCCALL!=c->dctype){if(DCSCRIPT!=c->dctype)c->dctype=DCJUNK; c=c->dclnk;}
 }

 // Don't allow redefinition of a name that is suspended higher up on the stack, possibly many times.   If the definition doesn't change, that's OK but we must point the stack to the new value
 c=d; NOUNROLL while(c=c->dclnk){if(DCCALL==c->dctype&&v==(A)c->dcn){c->dcn=(I)w; if(!jteqf(jt,w,v))JT(jt,sidamage)=1;}}
 R v;   // good return: recycle v to save a register
}    /* check for changes to stack */

// find symbol entry for name a in global symbol table g; this is known to be in service of global assignment
// the name a may require lookup through the path; once we find the locale, we search only in it
// Result is &value for symbol, or 0, with no low-order flags
// We are called for purposes of setting zombieval for inplace assignments.  We do not create the symbol because multiple threads may assign a name
// We try to create the symbol table (not really needed).  This routine must not modify fillv, which is in use in parsing
A jtprobequiet(J jt,A a){A g;
 I n=AN(a); NM* v=NAV(a); I m=v->m;  // n is length of name, v points to string value of name, m is length of non-locale part of name
 if(likely(n==m)){g=jt->global;}   // if not locative, define in default locale
 else{C* s=1+m+v->s; if(!(g=NMILOC&v->flag?locindirect(n-m-2,1+s,v->bucketx):stfindcre(n-m-2,s,v->bucketx))){RESETERR; R 0;}}  // if locative, find the locale for the assignment; error is not fatal
 READLOCK(g->lock) A res=jtprobe((J)((I)jt+NAV(a)->m),NAV(a)->s,NAV(a)->hash,g); READUNLOCK(g->lock)   // return pointer to value, if found
 R res;
}

// assign symbol: assign name a in symbol table g to the value w (but g is ignored if a is a locative)
// Result is 0 if error, otherwise low 2 bits are x1 = final assignment, 1x = local assignment, others garbage
// flags set in jt: bit 0=this is a final assignment;
I jtsymbis(J jt,A a,A w,A g){F2PREFIP;
 ARGCHK2(a,w);
 I anmf=NAV(a)->flag; // fetch flags for the name
 // Before we take a lock on the symbol table, realize any virtual w, and convert w to recursive usecount.  These will be unnecessary if the
 // name is NJA or is a reassignment, but since NJAs cannot be non-DIRECT little is lost.  We will be doing an unneeded realize if a virtual [x]y from
 // xdefn is reassigned to itself.  Too bad: we need to make sure we don't hold the lock through an expensive operation.
 // It is safe to do the recursive-usecount change here as local, because the value cannot have been released to any other core.  Similarly for
 // virtuals.
 // Find the internal code for the name to be assigned.  Do this before we take the lock.
 I wt=AT(w);
 rifv(w); // must realize any virtual
 if(unlikely(((wt^AFLAG(w))&RECURSIBLE)!=0)){AFLAGORLOCAL(w,wt&RECURSIBLE) wt=(I)jtra(w,wt,(A)wt);}  // make the block recursive (incr children if was nonrecursive).  This does not affect the usecount of w itself.

 if(unlikely((anmf&(NMLOC|NMILOC))!=0)){I n=AN(a); I m=NAV(a)->m;
  // locative: n is length of name, v points to string value of name, m is length of non-locale part of name
  // Find the symbol table to use, creating one if none found.  Unfortunately zombieval doesn't give us the symbol table
  C*s=1+m+NAV(a)->s; if(unlikely(anmf&NMILOC))g=locindirect(n-m-2,1+s,NAV(a)->bucketx);else g=stfindcre(n-m-2,s,NAV(a)->bucketx);
 }else{  // not locative assignment

  if(g==jt->global){  // global assignment.
   // check for non-locative global assignment to a locally-defined name.  Give domain error and immediately eformat, since no one has a self for assignment
   // this test will usually have a positive bucketx and will not call probelocal.  Unlikely that symx is present
   I localnexist=REPSGN(NAV(a)->bucketx|SGNIF(AR(jt->locsyms),ARNAMEADDEDX));   // 0 if bucketx positive (meaning name known but not locally assigned) AND no unknown name has been assigned: i. e. no local def ~0 otherwise
   localnexist=~localnexist&(I)NAV(a)->bucket;  // the previous calc is valid only if bucket info exists (i. e. processed for explicit def); now non0 if valid & known to have no assignment
   ASSERTSUFF(localnexist||!jtprobe((J)((I)jt+NAV(a)->m),NAV(a)->s,NAV(a)->hash,jt->locsyms),EVDOMAIN,R (I)jteformat(jt,0,str(strlen("public assignment to a name with a private value"),"public assignment to a name with a private value"),0,0);)
  }
 }
 RZ(g)  // g has the locale we are writing to
 I valtype=ATYPETOVALTYPE(wt);  // value to install.  It will have QCGLOBAL semantics

 if(unlikely((wt&NOUN)==0)){  // writing to ACV
  ACVCACHECLEAR; // invalidate all previous ACV lookups
  // if the value we are assigning is marked as NAMELESS, and the name is not a locative, flag this name as NAMELESS.  Only ACVs are NAMELESS
  // NOTE that the value may be in use elsewhere; may even be a primitive.

  if(unlikely((((NAV(a)->flag&NMLOC+NMILOC+NMIMPLOC)-1)&SGNIF(FAV(w)->flag2,VF2NAMELESSX))<0))valtype=VALTYPENAMELESS;   // nameless & non-locative, so indicate
  if(unlikely(jt->glock!=0))if(likely(FAV(w)->fgh[0]!=0)){FAV(w)->flag|=VLOCK;}  // fn created in locked function is also locked
  if((AR(g)&ARLOCALTABLE)!=0)AR(g)|=ARHASACV;  // if we assign a non-noun to a local table, note the fact so we will look them up
 }

 L *e;  // the symbol we will use
 // we don't have e, look it up.  NOTE: this temporarily undefines the name, which will have a null value pointer.  We accept this, because any reference to
 // the name was invalid anyway and is subject to having the value removed
 // We reserve 1 symbol for the new name, in case the name is not defined.  If the name is not new we won't need the symbol.
 if((AR(g)&ARLOCALTABLE)!=0){  // if assignment to a local table (which might not be jt->locsyms)
  I4 symx=NAV(a)->symx;
  e=likely((SGNIF(AR(g),ARLCLONEDX)|(symx-1))>=0)?SYMORIGIN+(I)symx:probeislocal(a,g);  // local symbol given and we are using the original table: use the symbol.  Otherwise, look up and reserve 1 symbol
  g=0;   // indicate we have no lock to clear
 }else{  // global table
  SYMRESERVE(1)
  I bloom=BLOOMMASK(NAV(a)->hash);  // calculate Bloom mask outside of lock
  valtype|=QCGLOBAL;  // must flag local/global type in symbol
  e=probeis(a, g);  // get the symbol address to use, old or new.  This returns holding a lock on the table
  // if we are writing to a non-local table, update the table's Bloom filter.
  BLOOMOR(g,bloom);  // requires writelock on g
 }
 // ****** if g is a global table, we have a write lock on the locale, which we must release in any error paths.  g=0 otherwise *******

 // if we are debugging, we have to make sure that the value being replaced is not in execution on the stack.  Of course, it would have to have an executable type
 A x=e->val;   // if x is 0, this name has not been assigned yet; if nonzero, x points to the incumbent value
 if(unlikely(jt->uflags.trace&TRACEDB))if(x!=0&&((e->valtype&QCNOUN)==0))x=redef(w,x);  // check for SI damage (handled later).  could move outside of lock, but it's only for debug
 // *** this is the last subroutine call till the end - registers available ***

 I xaf;  // holder for nvr/free flags
 {A aaf=AFLAG0; aaf=x?x:aaf; xaf=AFLAG(aaf);}  // flags from x, or 0 if there is no x
 e->sn=jt->currslistx;  // Save the script in which this name was defined
 ASSERTGOTO(!(e->flag&LREADONLY),EVRO,exitlock)  // if writing read-only name (xxx_index), fail
 if(unlikely(JT(jt,stch)!=0))e->flag|=LCH;  // update 'changed' flag if enabled - no harm in marking locals too

 if(likely(!(AFNJA&xaf))){
  // Normal case of non-memory-mapped assignment.
  // If we are assigning the same data block that's already there, don't bother with changing use counts or anything else (assignment-in-place)
  if(likely(x!=w)){
   e->valtype=valtype;  // set the value type of the new value
   e->val=w;  // store the new value to free w before ra()
   SYMVALFA1(*e,x);  // fa the value unless it was never ra()d to begin with, and handle AC for the caller in that case; repurpose x to point to any residual value to be fa()d later
                   // It is OK to do the first half of this operation early, since it doesn't change the usecount.  But we must keep the lock until we have protected w
                   // SYMVALFA1 does not call a subroutine
   // Increment the use count of the value being assigned, to reflect the fact that the assigned name will refer to it.
   // Virtual values were realized earlier, and usecounts guaranteed recursive
   // If the value is abandoned inplaceable, we can just zap it and set its usecount to 1
   // SPARSE nouns must never be inplaceable, because their components are not 
   if((SGNIF(jtinplace,JTFINALASGNX)&AC(w))<0){  // if final assignment of abandoned value
    // We can zap abandoned nouns.  But only if they are final assignment: something like nm:_ [ nm=. 4+4 would free the active block if we zapped, when the FAOWED was applied
    //  also, if the name is global it may vanish, along with the value, at any time, so we 
    AFLAGORLOCAL(w,AFKNOWNNAMED);   // indicate the value is in a name.  We do this to allow virtual extension.
    // very often a final assignment will assign the abandoned result of the last computation.  These values accumulate on the tstack and have to
    // be popped off every now & then in jtxdefn, since there is nothing else to pop them.  We detect this important case, in which the zaploc of the
    // abandoned value is the very last thing on the tstack: then we back up tpushp, removing all trace of the block
    ZAPTSTACKEND(w) ACRESET(w,ACUC1)  // make it non-abandoned.  Like raczap(1).  Context must ensure that taking w off tstack will not invalidate any pending tpop
    // It would be a disaster to back the tstack to in front of a valid 'old' pointer held somewhere.  The subsequent tpop would never end.  The case cannot occur, because we set 'old'
    // only before sentence execution, and there is no way for an anonymous abandoned value to come from a higher level (name:_ pushes a stack entry at the current level; assignment to y clears inplaceability).
    // NOTE: NJA can't zap either, but it never has AC<0
   }else{   // not abandoned final assignment
    if(likely(!ACISPERM(AC(w))))AFLAGSETKNOWN(w);   // indicate the value is in a name.  We do this to allow virtual extension.  Is it worth it?.  Probably, since we have to lock AC anyway
    rarecur(w);  // if zap not allowed, just ra() w, known recursive-if-recursible so this is quick.  Subroutine call.  w may be inplaceable but not zappable so no rapos; may be sparse so we must allow 1 small recursion then
   }

  }else x=0;  // repurpose x to be the value needing fa
 }else{  // x exists, and is either read-only or memory-mapped
  ASSERTGOTO(!(AFRO&xaf),EVRO,exitlock);   // error if read-only value
  if(x!=w){  // replacing name with different mapped data.  If data is the same, just leave it alone
   // no need to store valtype - that can't change from noun (because must be DIRECT below)
   I wt=AT(w); I wn=AN(w); I wr=AR(w); I m=wn<<bplg(wt);  // we will move the flags/data from w to the preallocated area x
   ASSERTGOTO((wt&DIRECT)>0,EVDOMAIN,exitlock);  // boxed, extended, etc can't be assigned to memory-mapped array
   ASSERTGOTO(AM(x)>=m,EVALLOC,exitlock);  // ensure the file area can hold the data.  AM of NJA is allosize
   AT(x)=wt; AN(x)=wn; AR(x)=(RANKT)wr; MCISH(AS(x),AS(w),wr); MC(AV(x),AV(w),m);  // copy in the data.  Can't release the lock while we are copying data in.
  }
  x=0;  // indicate no further fa needed
 }
 // x here is the value that needs to be freed
 if(g!=0)WRITEUNLOCK(g->lock)else jtinplace=(J)((I)jtinplace|JTASGNWASLOCAL);  // if global, release lock; else indic local in return 
 // ************* we have released the write lock
 // If this is a reassignment, we need to decrement the use count in the old value, since that value is no longer used.  Do so after the new value is raised,
 // in case the new value was being protected by the old (ex: n =. >n).
 // It is the responsibility of parse to keep the usecount of a named value raised until it has come out of execution
 SYMVALFA2(x);  // if the old value needs to be traversed in detail, do it now outside of lock (subroutine call)
 R (I)jtinplace;   // good return, with bit 0 set if final assignment, bit 1 if local
exitlock:  // error exit
 if(g!=0)WRITEUNLOCK(g->lock)
 R 0;
}    /* a: name; w: value; g: symbol table */

// assign symbol and free values immediately
I jtsymbisdel(J jt,A a,A w,A g){
 // All we have to do is mark the assignment as final.
 R jtsymbis((J)((I)jt|JTFINALASGN),a,w,g);
}
