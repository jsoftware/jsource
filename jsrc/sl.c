/* Copyright (c) 1990-2024, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Symbol Table: Locales                                                   */

#include "j.h"

// Interfaces for numbered locales
#if 0   // direct locale numbering
 #define DELAYBEFOREREUSE 5000  // min number of locales to have before we start reusing
 // Initialize the numbered-locale system.  Called during initialization, so no need for ras()
 static A jtinitnl(J jt){A q;
  GATV0(q,INT,1,1);
  JT(jt,stnum)=q;  // save address of block
  jt->numloctbl=IAV1(q);  // address of locale vector
  jt->numlocsize=1;  // length of locale vector
  jt->numlocdelqh=IAV1(q);  // pointer to first locale number to allocate
  jt->numlocdelqn=1;  // init 1 value to allocate
  jt->numlocdelqt=IAV1(q);  // pointer to end of queue (queue is never empty - it starts at 1 and never allocates unless there are DELAYBEFOREREUSE empties
  *jt->numlocdelqh=(I)IAV1(q);  // Make the sole locale the end-of-chain by looping its chain to itself.  This is never needed as end-of-chain but it does ensure relocation
  R q;  // return no error
 }
 
 // Get the locale number to use for the next numbered locale.  (0 if error, with error code set)
 // This call just gets the number to use, it does not allocate the number.  The locale will be installed later, unless an error intervenes
 static I jtgetnl(J jt){
  if(jt->numlocdelqn<DELAYBEFOREREUSE){
   // There are not enough blocks in the delq to guarantee that the next free will lie fallow long enough.  Increase the allocation
   A x; UI oldsize=jt->numlocsize; do{RZ(x=ext(1,JT(jt,stnum))) JT(jt,stnum)=x;}NOUNROLL while(jt->numlocdelqn+AN(x)-oldsize<DELAYBEFOREREUSE);   // extend & save the allocation.  ext handles the usecount.  Stop when we have enough
   jt->numlocdelqn += AN(x)-oldsize;  // add the new allocation into the size of the delq
   I relodist=(I)IAV(x)-(I)jt->numloctbl;  // relocation factor
   I oldbase=(I)jt->numloctbl; I *reloptr=IAV(x);  // treat the locale pointer as integers for arithmetic here
   DQ(oldsize, if((UI)(*reloptr-oldbase)<(oldsize<<LGSZI))*reloptr+=relodist; ++reloptr;)  // if the pointer points to within the old block, relocate it to the new.  reloptr ends pointing to the first new block
   jt->numlocdelqh=(I*)((I)jt->numlocdelqh+relodist); jt->numlocdelqt=(I*)((I)jt->numlocdelqt+relodist);  // relocate delq head/tail
   // we could make the new block the head of the alloq, but then block 1 would go out before block 0 and Chris would complain.  So we chain them off the tail
   *jt->numlocdelqt=(I)reloptr; // chain the upcoming blocks at end of queue
   DQ(AN(x)-oldsize-1, *reloptr=(I)(reloptr+1); reloptr=reloptr+1;)  // chain each block to the next, except for the last.  end pointing to the last
   *reloptr=(I)reloptr; jt->numlocdelqt=reloptr; // chain added blocks as allocable, in ascending order.  The last one loops to self to indicate end, and becomes the end of the delq
   jt->numloctbl=IAV(x); jt->numlocsize=(UI4)AN(x); // set address and length of new table
  }
  R jt->numlocdelqh-jt->numloctbl;  // return index of next allocation
 }
 
 // Install locale l in the numbered-locale table, at the number returned by the previous jtgetnl.  No error is possible
 static void jtinstallnl(J jt, A l){
  I *nextallo=(I*)*jt->numlocdelqh;  // save new head of the allo chain
  ras(l); *jt->numlocdelqh=(I)l;  // protect l and store it as the new locale for its index
  jt->numlocdelqh=nextallo;  // set next block as new head of chain
  // we can't be allocating the end-of-chain unless DELAYBEFOREREUSE is 1
  --jt->numlocdelqn;  // reduce number-of-items in delq by the 1 we have just released
 }
 
 // return the address of the locale block for number n, or 0 if not found
 A jtfindnl(J jt, I n){A z;
  findnlz(n,z); R z;
 }
 
 // delete the locale numbered n, if it exists
void jterasenl(J jt, I n){
  if(jtfindnl(jt,n)){  // if locale exists
   I newblock=(I)&jt->numloctbl[n];
   // chain the new block off the old tail
   *jt->numlocdelqt=newblock;
   // set end-of-chain (loop to self) in the new block
   jt->numloctbl[n]=newblock;
   // set the new block as new tail
   jt->numlocdelqt=(I*)newblock;
   // add 1 to the size of the delq
   ++jt->numlocdelqn;
  }
 }
 
 // return list of active numbered locales, using namelist mask
 static A jtactivenl(J jt){A y;
  GATV0(y,INT,AN(JT(jt,stnum)),1); I *yv=IAV1(y);   // allocate place to hold numbers of active locales
  I nloc=0; DO(AN(JT(jt,stnum)), if(jtfindnl(jt,i)){yv[nloc]=i; ++nloc;})
  R every(take(sc(nloc),y),ds(CTHORN));  // ".&.> nloc{.y
 }
 
 // iterator support
#else
// Hashed version, without locale reuse
#if BW==64
#define HASHSLOT(x,tsize) (((UI)((UI4)(x)*(UI4)2654435761U)*(UI)(tsize))>>32)
#else
#define HASHSLOT(x,tsize) (((UI4)(x)*(UI4)2654435761U)%(UI4)(tsize))
#endif
// Initialize the numbered-locale system.  Called during initialization, so no need for ras()
// Runs in master thread
static A jtinitnl(J jt){A q;
 I s; FULLHASHSIZE(5*1,INTSIZE,0,0,s);  // at least 5 slots, so we always have at least 2 empties
 GATV0(q,INT,s,1); mvc(s*SZI,IAV1(q),MEMSET00LEN,MEMSET00);  // allocate hashtable and clear to 0.  Init no 
 JT(jt,stnum)=q;  // save address of block
 AS(JT(jt,stnum))[0]=0;  // set next number to allocate
 AM(JT(jt,stnum))=0;  // set number in use
 R q;  // return no error
}

// Install locale l in the numbered-locale table.  We have a write lock on stlock.  Return -1 if error (with the lock released), otherwise the locale number
static I jtinstallnl(J jt, A l){
 A nbuf=__atomic_load_n(&JT(jt,stnum),__ATOMIC_ACQUIRE);  // address of the hashtable
 while(2*AM(nbuf)>AN(nbuf)){  // resize if needed
  A obuf=nbuf;  //  address of incumbent block before resize
  if(unlikely(jtextendunderlock(jt,&JT(jt,stnum),&JT(jt,stlock),8+4+2+1)==0))R -1;  // this is the only error exit, with lock removed
  nbuf=__atomic_load_n(&JT(jt,stnum),__ATOMIC_ACQUIRE);  // address of the new hashtable
  // if AM(nbuf) is 0, we were the ones who extended the buffer.  We must rehash the old numbers into it.  In this case we know that
  // obuf has not been altered.
  if(likely(AM(nbuf)==0)){
   // rehash the old table into the new
   I i; for(i=0;i<AN(obuf);++i){
    A st; if(st=(A)IAV1(obuf)[i]){  // if there is a value hashed... (it is a SYMB type)
     I probe=HASHSLOT(LOCNUM(st),AN(nbuf));  // start of search.  Look backward, wrapping around, until we find an empty.  We never have duplicates
     NOUNROLL while(IAV1(nbuf)[probe]){if(unlikely(--probe<0))probe=AN(nbuf)-1;}  // find empty slot
     IAV1(nbuf)[probe]=(I)st;  // install in new hashtable
    }
   }
   AM(nbuf)=AM(obuf);  // indicate that the locales have been added
   mf(obuf);  // free the old buffer, which we are sure we own
  }
 }
 // hash the locale into the table, which we know has enough room
 I z=AS(nbuf)[0]++;   // get next locale# to allocate, and increment for next time
 I probe=HASHSLOT(z,AN(nbuf));  // start of search.  Look backward, wrapping around, until we find an empty.  We never have duplicates
 NOUNROLL while(IAV1(nbuf)[probe]){if(unlikely(--probe<0))probe=AN(nbuf)-1;}  // find empty slot
 IAV1(nbuf)[probe]=(I)l;  // put new locale in the empty slot
 ++AM(nbuf);  // increment number of locales outstanding
 ACINITZAP(l);  // protect new value in table
 R z;
}

// return the address of the locale block for number n, or 0 if not found
A jtfindnl(J jt, I n){A z=0;
 READLOCK(JT(jt,stlock))
 I probe=HASHSLOT(n,AN(JT(jt,stnum)));  // start of search.  Look backward, wrapping around, until we find match or an empty.
 NOUNROLL while(IAV1(JT(jt,stnum))[probe]){if(LOCNUM((A)IAV1(JT(jt,stnum))[probe])==n){z=(A)IAV1(JT(jt,stnum))[probe]; goto exit;} if(unlikely(--probe<0))probe=AN(JT(jt,stnum))-1;}  // return if locale match; wrap around at beginning of block
exit: ;
 READUNLOCK(JT(jt,stlock))
 R z;  // if no match, return failure
}

// delete the locale numbered n, if it exists
void jterasenl(J jt, I n){
 WRITELOCK(JT(jt,stlock))
 I probe=HASHSLOT(n,AN(JT(jt,stnum)));  // start of search.  Look backward, wrapping around, until we find a match or an empty.
 NOUNROLL while(IAV1(JT(jt,stnum))[probe]){if(LOCNUM((A)IAV1(JT(jt,stnum))[probe])==n)break; if(unlikely(--probe<0))probe=AN(JT(jt,stnum))-1;}  // wrap around at beginning of block
 // We have found the match, or are at an empty if no match.  Either way, mark the location as empty and scan forward to find the next empty,
 // moving back blocks that might have hashed into the newly vacated spot
 if(IAV1(JT(jt,stnum))[probe])--AM(JT(jt,stnum));  // if we found something to delete, decrement # locales outstanding
 NOUNROLL while(1){  // probe points to either the original deletion point or a value that was just copied to an earlier position.  Either way it gets deleted
  IAV1(JT(jt,stnum))[probe]=0;  // delete the now-invalid or -moved location
  I lastdel=probe;    // remember where the hole is
  I probehash;   // will hold original hash of probe
  NOUNROLL do{
   if(unlikely(--probe<0))probe=AN(JT(jt,stnum))-1;  // back up to next location to inspect
   if(!IAV1(JT(jt,stnum))[probe])goto exit;  // if we hit another hole, there can be no more values that need copying, we're done  *** RETURN POINT ***
   probehash=HASHSLOT(LOCNUM((A)IAV1(JT(jt,stnum))[probe]),AN(JT(jt,stnum)));  // see where the probed cell would like to hash
    // If we are not allowed to move the new probe into the hole, because its hash is after the probe position but before-or-equal the hole,
    // we leave it in place and continue looking at the next position.  This test must be performed cyclically, because the probe may have wrapped around 0
  }while((BETWEENO(probehash,probe,lastdel))||(probe>lastdel&&(probe<=probehash||probehash<lastdel)));  // first half is normal, second if probe wrapped around
  // here lastdel is the hole, and probe is a slot that hashed somewhere before lastdel.  We can safely move the probe to cover the hole.
  // This creates a new hole at probe, which we loop back to clear & then try to fill
  IAV1(JT(jt,stnum))[lastdel]=IAV1(JT(jt,stnum))[probe];  // move the hole forward
 }
exit: ;
 WRITEUNLOCK(JT(jt,stlock))
}

// return list of active numbered locales, using namelist mask
static A jtactivenl(J jt){A y;
 READLOCK(JT(jt,stlock))
 GATV0E(y,INT,AN(JT(jt,stnum)),1, {READUNLOCK(JT(jt,stlock));R0}); I *yv=IAV1(y);   // allocate place to hold numbers of active locales
 I nloc=0; DO(AN(JT(jt,stnum)), if(IAV1(JT(jt,stnum))[i]&&(LOCPATH((A)IAV1(JT(jt,stnum))[i]))){yv[nloc]=LOCNUM((A)IAV1(JT(jt,stnum))[i]); ++nloc;})
 READUNLOCK(JT(jt,stlock))
 R every(take(sc(nloc),y),ds(CTHORN));  // ":&.> nloc{.y
}

// iterator support.  countnl returns a number of iterations.  indexnl returns the A block (or 0) for 
I jtcountnl(J jt) { R AN(JT(jt,stnum)); }  // number of locales to reference by index
A jtindexnl(J jt,I n) {A z=(A)IAV1(JT(jt,stnum))[n]; R z&&LOCPATH(z)?z:0; }  // the locale address, or 0 if none

#endif

// Create symbol table: k is -1/0 for nonperm named/named, 1 for numbered, 2 for local; p is the exact number of hash entries desired (including SYMLINFOSIZE);
// n is length of name (or locale# to allocate, for numbered locales), u->name
// Result is SYMB type for the symbol table.  For global tables only, ras() has been executed
// on the result and on the name and path
// For named/numbered types, SYMLINFO (hash chain #0) is filled in to point to the name and path
//   the name is an A type holding an NM, which has hash filled in, and, for numbered locales, the bucketx filled in with the locale number
// For local symbol tables, hash chain 0 is repurposed to hold symbol-index info for x/y (filled in later)
// The SYMB table is always allocated with rank 0.  The stored rank is 1 for named locales, 0 for others, plus other flags
// For k=0, we have a write lock on stlock which we must hold throughout.
// For k=0 or 1, we have made sure there are 2-k symbols reserved (for the assignments we make).  Not required for k=2, which is not assigned
A jtstcreate(J jt,I1 k,I p,I n,C*u){A g,x,xx;L*v;
 // allocate the symbol table itself: we have to give exactly what the user asked for so that cloned tables will hash identically; but a minimum of 1 chain field so hashes can always run
 GATV0(g,SYMB,MAX(p,SYMLINFOSIZE+1),0); AFLAGORLOCAL(g,SYMB)  //  All SYMB tables are born recursive.
 // Allocate a symbol for the locale info, install in special hashchain 0.  Set flag;
 // (it is queried by 18!:_2)
 // The allocation clears all the hash chain bases, including the one used for SYMLINFO
 switch(k){
 case -1: case 0:  // named locale - we have a write lock on stloc->lock
  AR(g)=ARINVALID;  // until the table is all filled in, it is in an invalid state and cannot be inspected when freed
  v=symnew(&LXAV0(g)[SYMLINFO],0);    // put new block into locales table, allocate at head of chain without non-PERMANENT marking,  The symbol must be available
  v->flag|=LINFO;  // mark as not having a value
  RZ(x=nfs(n,u));  // this fills in the hash for the name
  // Install name and path.  Path is 'z'. correct for all but z locale itself, which is overwritten at initialization
  LOCNAME(g)=x; LOCPATH(g)=JT(jt,zpath);   // zpath is permanent.
  // Assign this name in the locales symbol table to point to the allocated SYMB block
  // This does ras() on g
  // Put the locale name into stloc.  We can't use symbis etc because we have to keep the continuous lock on stlock to prevent multiple assignment of a named locale
  // (and so we don't really need to ra/fa when adding/removing from stloc) 
  // We know the name is not in the table; we spin to the end of the chain on the theory that most-used locales will be loaded first
  UI4 hsh=NAV(x)->hash; L *sympv=SYMORIGIN;  // hash of name; origin of symbol tables
  LX *hv=LXAV0(JT(jt,stloc))+SYMHASH(hsh,AN(JT(jt,stloc))-SYMLINFOSIZE);  // get hashchain base in stloc
  LX tx=SYMNEXT(*hv); if(tx!=0)NOUNROLL while(SYMNEXT(sympv[tx].next)!=0)tx=SYMNEXT(sympv[tx].next);  // tx->last in chain, or 0 if chain empty
  v=symnew(hv,tx); v->name=x; v->val=g; ACINITZAP(g);  // install the new locale at end of chain; put name into block; save value; ZAP to match store of value
  LOCBLOOM(g)=0;  // Init Bloom filter to 'nothing assigned'
  ACINITZAP(x); ACINIT(x,ACUC2)  // now that we know we will succeed, transfer ownership to name to the locale and stloc, one each
  AR(g)=ARNAMED;   // set rank to indicate named locale
  LXAV0(g)[SYMLEXECCT]=((k+1)<<EXECCTPERMX)+EXECCTNOTDELD;  // mark permanent unless k is _1
  break;
 case 1:  // numbered locale - we have no lock
  AR(g)=ARINVALID;  // until the table is all filled in, it is in an invalid state and cannot be inspected when freed
  RZ(v=symnew(&LXAV0(g)[SYMLINFO],0));   // put new block into locales table, allocate at head of chain without non-PERMANENT marking
  v->flag|=LINFO;  // mark as not having a value (for diags.  value is used for locnum)
  // Put this locale into the in-use list at an empty location.  ras(g) at that time
  RZ(x=nfs(20,&CAV(ds(CALP))['a']))  // create the name block before lock.  The hash will be meaningless
  WRITELOCK(JT(jt,stlock)) RZ((n=jtinstallnl(jt, g))>=0);   // put the locale into the numbered list; exit if error (with lock removed); zap g
  I nmlen=sprintf(NAV(x)->s,FMTI,n); AN(x)=nmlen; NAV(x)->m=nmlen;  // install true locale number and length of name
  LOCNUMW(g)=(A)n; // save locale# in SYMLINFO
  LOCBLOOM(g)=0;  // Init Bloom filter to 'nothing assigned'.  Must be after installnl
  LOCPATH(g)=JT(jt,zpath);  // zpath is permanent, no ras needed  Must be after installnl
  WRITEUNLOCK(JT(jt,stlock))
  LOCNAME(g)=x;  // set name pointer in SYMLINFO
  ACINITZAP(x);   // now that we know we will succeed, transfer ownership to name to the locale
  AR(g)=0;   // set rank to indicate numbered locale
  LXAV0(g)[SYMLEXECCT]=EXECCTNOTDELD;  // numbered locales are nondeleted but not permanent
  break;
 case 2:  // local symbol table - we have no lock and we don't assign
  AR(g)=ARLOCALTABLE;  // flag this as a local table so the first hashchain is not freed
  // The first hashchain is not used as a symbol pointer - it holds xy bucket info
  // Bloom filter not used for local symbol tables (the field is a chain for the stack of active defs)
  // local symbol tables don't have execcts
  break;
 }
 R g;
}    /* create locale, named (0==k) or numbered (1==k) */

// initialization routine: INITZAP not required to protect blocks
B jtsymbinit(JS jjt){A q,zloc;JJ jt=MTHREAD(jjt);
 INITJT(jjt,locsize)[0]=3;  /* default hash table size for named    locales */
 INITJT(jjt,locsize)[1]=2;  /* default hash table size for numbered locales */
 RZ(jtsymext(jt));     /* initialize symbol pool                       */
 I p; FULLHASHSIZE(400,SYMBSIZE,1,SYMLINFOSIZE,p);
 GATV0(q,SYMB,p,0); AFLAGORLOCAL(q,SYMB) INITJT(jjt,stloc)=q;  // alloc space, clear hashchains.  No name/val for stloc.  All SYMBs are recursive (though this one, which will never be freed, needn't be)
 jtinitnl(jt);  // init numbered locales, using master thread to allocate
 // init z locale
 FULLHASHSIZE(1LL<<12,SYMBSIZE,1,SYMLINFOSIZE,p);  // about 2^13 chains
 SYMRESERVE(2) RZ(zloc=stcreate(0,p,1L,"z")); ACX(zloc);   // make the z locale permanent
 // create zpath, the default path to use for all other locales
 GAT0(q,BOX,2,1); AAV1(q)[0]=0; AAV1(q)[1]=zloc; ACX(q); JT(jt,zpath)=&AAV1(q)[1];   // install ending 0 & z locale; make the path permanent too .  In case we get reinitialized, we have to make sure zpath is set only once
 LOCPATH(zloc)=&AAV1(q)[0];   // make z locale have no path.  The path is already permanent
 // init the symbol tables for the master thread.  Worker threads must copy when they start execution
 // init base locale
 FULLHASHSIZE(1LL<<10,SYMBSIZE,1,SYMLINFOSIZE,p);  // about 2^11 chains
 SYMRESERVE(2) RZ(q=stcreate(0,p,sizeof(INITJT(jjt,baselocale)),INITJT(jjt,baselocale)));
 jt->global=q;  // init baselocale in master.  workers must init on each call
 // Allocate a symbol table with just 1 (empty) chain; then set length to 1 indicating 0 chains; make this the current local symbols, to use when no explicit def is running
 // NOTE: you must apply a name from a private locale ONLY to the locale it was created in, or to a global locale.  Private names contain bucket info & symbol pointers that would
 // cause errors if applied in another locale.  It is OK to apply a non-private name to any locale.
 A emptyloc; RZ(emptyloc=stcreate(2,0,0,0)); AKGST(emptyloc)=jt->global; AN(emptyloc)=SYMLINFOSIZE; AM(emptyloc)=(I)emptyloc; ACX(emptyloc); // close chain so u. at top level has no effect.  No Bloom filter since local table
 // To speed up the test for existence of local symbols, we adjust the position of emptyloc so that it is on an odd boundary of (two Is).  This has the salubrious side effect
 // of putting the oft-referenced parts (AN through the chains) into a single cacheline.  We know that AC is positioned at an offset of 3 words, and we know that the allocation has spare space at the end,
 // so we copy to move AC to the midpoint of the allocation (a new cacheline, in 64-bit)
 // We need a different empty locale for each thread, because the global symbol table is stored there.  Allocate at rank 1, and fill it with copies of emptyloc
 // Unfortunately the layout of the locale uses words 0 and 8, so we can't pack the block for the threads into adjacent cachelines.  Perhaps we should just have the thread
 // allocate an empty locale when it starts, but we have coded this and we will keep it.  Unallocated threrads will drop out of cache.
 GA0(q,INT,16*MAXTHREADS,1) INITJT(jjt,emptylocale)=(I(*)[MAXTHREADS][16])((I*)q+8-3);   //  this mangles the header; OK since the block will never be freed
 DONOUNROLL(MAXTHREADS, A ei=(A)&((I*)q)[16*i+8-3]; MC(ei,emptyloc,(8+3)*SZI); AM(ei)=(I)ei;)
 jt->locsyms=(A)(*INITJT(jjt,emptylocale))[0];  // init jt->locsyms for master thread to the emptylocale for the master thread.  jt->locsyms in other threads must be initialized for each user task
 R 1;
}


F1(jtlocsizeq){C*v; ASSERTMTV(w); v=JT(jt,locsize); R v2(v[0],v[1]);}
     /* 9!:38 default locale size query */

F1(jtlocsizes){I p,q,*v;
 ARGCHK1(w);
 ASSERT(1==AR(w),EVRANK);
 ASSERT(2==AN(w),EVLENGTH);
 RZ(w=vi(w)); v=AV(w); p=v[0]; q=v[1];
 ASSERT((p|q)>=0,EVDOMAIN);
 ASSERT(((p-14)&(q-14))<0,EVLIMIT);
 JT(jt,locsize)[0]=p;
 JT(jt,locsize)[1]=q;
 R mtm;
}    /* 9!:39 default locale size set */

// jtprobe, with readlock taken on stlock
static A jtprobestlock(J jt, C *u,UI4 h){F1PREFIP; READLOCK(JT(jt,stloc)->lock) A z=jtprobe(jtinplace,u,h,JT(jt,stloc)); READUNLOCK(JT(jt,stloc)->lock) R z;}

// find the symbol table for locale with name u which has length n and hash/number bucketx
// locale name is known to be valid
// n=0 means 'use base locale'
// n=-1 means 'numbered locale, don't bother checking digits'   u is invalid
// The slot we return might be a zombie, if LOCPATH is clear.  The caller must check.
A jtstfind(J jt,I n,C*u,I bucketx){
 if(unlikely(!n)){n=sizeof(JT(jt,baselocale)); u=JT(jt,baselocale);bucketx=JT(jt,baselocalehash);}
 if(n>0&&'9'<*u){  // named locale   > because *u is known to be non-empty
  ASSERT(n<256,EVLIMIT);
  A v=jtprobestlock((J)((I)jt+n),u,(UI4)bucketx);
  if(v)R v;   // if there is a symbol, return its value  scaf this fails if someone else is deleting this locale when we are referring to it through a locative
 }else{
  R findnl(bucketx);
 }
 R 0;  // not found
}


// Bring a destroyed locale back to life as if it were newly created: clear the chains, set the default path, clear the Bloom filter
// set permanent status as set in the cocreate request
#define REINITZOMBLOC(g,perm) mvc((AN(g)-SYMLINFOSIZE)*sizeof(LXAV0(g)[0]),LXAV0(g)+SYMLINFOSIZE,MEMSET00LEN,MEMSET00); LOCBLOOM(g)=0; LXAV0(g)[SYMLEXECCT]=(perm)?EXECCTNOTDELD+EXECCTPERM:EXECCTNOTDELD; LOCPATH(g)=JT(jt,zpath);
         // we should check whether the path in non0 but that would only matter if two threads created the locale simultaneously AND set a path, and the only loss would be that the path would leak
static F2(jtloccre);

// look up locale name, and create the locale if not found
// If a locale is returned, its path has been made nonnull
// bucketx is hash (for named locale) or number (for numeric locale)
// n=0 means 'use base locale'
// n=-1 means 'numbered locale, don't bother checking digits'   u is invalid
A jtstfindcre(J jt,I n,C*u,I bucketx){
 while(1){
  A v = stfind(n,u,bucketx);  // lookup.  NOTE another thread could delete the locale while we're looking at it - could always zombie it?
  if(likely(v!=0)){  // name found
   if(unlikely(LOCPATH(v)==0)){ra(v); WRITELOCK(JT(jt,stloc)->lock) REINITZOMBLOC(v,1) WRITEUNLOCK(JT(jt,stloc)->lock)}  // if the path is null, this is a zombie empty locale in the path of some other locale.  Bring it back to life
    // setting a path must be accompanied by raising the usecount, because a locale is liable to be erased when its path is nonnull and it must survive as a zombie then
   R v;  // return the locale found
  }
  // here the locale must be created (rare)
  if(n>=0&&'9'<*u){  // nonnumeric locale:
   RZ(jtloccre(jt,mark,boxW(str(n,u))));  // create it with name - we will loop back to look it up again
  }else{
   ASSERT(0,EVLOCALE); // illegal to create numeric locale explicitly
  }
 }
}

// b is flags: 1=check name for validity, 2=do not allow numeric locales (whether atomic or not)
static A jtvlocnl(J jt,I b,A w){A*wv,y;C*s;I i,m,n;
 ARGCHK1(w);
 if(((b-2) & (SGNIF(AT(w),INTX) | (SGNIF(AT(w),B01X) & (AR(w)-1))))<0)R w;  // integer list or scalar boolean is OK  C_LE
 n=AN(w);
 ASSERT(((n-1)|SGNIF(AT(w),BOXX))<0,EVDOMAIN);   // error if nonempty & nonboxed
 wv=AAV(w); 
 for(i=0;i<n;++i){
  y=C(wv[i]);  // pointer to box
  if(((b-2)&(AR(y)-1)&-(AT(y)&(INT|B01)))<0)continue;   // scalar numeric locale is ok
  m=AN(y); s=CAV(y);
  ASSERT(1>=AR(y),EVRANK);
  ASSERT(m!=0,EVLENGTH);
  ASSERT(LIT&AT(y),EVDOMAIN);
  ASSERT(((1-b) & (I)((UI)CAV(y)[0]-('9'+1)))>=0,EVDOMAIN);  // numeric locale not allowed except when called for in b
  if(b&1)ASSERTN(vlocnm(m,s),EVILNAME,nfs(m,s));
 }
 R w;
}    /* validate namelist of locale names  Returns list if all valid, else 0 for error */

// p points to string of length l.  Result stored in z, which must be a declared lvalue
// overflow is ignored, much as strtoI did (it clamped, which is similarly invalid).  We expect the user to have audited the length
#define strtoI10(p,l,z) {z=0; C* p_ = p; DQ(l, z=4*z+z; z=2*z + (*p_++-'0');)}
// subroutine version
I strtoI10s(I l,C* p) {I z; strtoI10(p,l,z); R z; }

F1(jtlocnc){A*wv,y,z;C c,*u;I i,m,n,*zv;
 RZ(vlocnl(0,w));
 n=AN(w); wv=AAV(w); 
 I zr=AR(w); GATV(z,INT,n,AR(w),AS(w)); zv=AVn(zr,z);
 if(!n)R z;  // if no input, return empty before handling numeric-atom case
 if(AT(w)&(INT|B01)){IAV(z)[0]=(y=findnl(BIV0(w)))&&LOCPATH(y)?1:-1; RETF(z);}  // if integer, must have been atomic or empty.  Handle the one value
 for(i=0;i<n;++i){
  y=C(wv[i]);
  if(!AR(y)&&AT(y)&((INT|B01))){  // atomic numeric locale
   zv[i]=findnl(BIV0(y))?1:-1;  // OK because the boxed value cannot be virtual, thus must have padding
  }else{A yy;
   // string locale, whether number or numeric
   m=AN(y); u=CAV(y); c=*u; 
   if(!vlocnm(m,u))zv[i]=-2;
   else if(c<='9') zv[i]=(y=findnl(strtoI10s(m,u)))&&LOCPATH(y)?1:-1;
   else            zv[i]=(yy=jtprobestlock((J)((I)jt+m),u,(UI4)nmhash(m,u)))&&LOCPATH(yy)?0:-1;
  }
 }
 RETF(z);
}    /* 18!:0 locale name class */

static A jtlocnlx(J jt,A a, A w, I zomb){A y,z=mtv;B*wv;I m=0;
 RZ(w=cvt(B01,w)); wv=BAV(w); DO(AN(w), m|=1+wv[i];);  // accumulate mask of requested types
 if(1&m)z=nlsym(a,JT(jt,stloc),zomb);  // named locales
 if(2&m){RZ(y=jtactivenl(jt)); z=over(y,z); }  // get list of active numbered locales
 R grade2(z,ope(z));
}

F1(jtlocnl1){A a; GAT0(a,B01,256,1) mvc(256L,CAV1(a),1,MEMSET01);  R locnlx(a,w,0);}
    /* 18!:1 locale name list */

// 18!:_3 locale name list, but including zombie locales
F1(jtlocnlz1){A a; GAT0(a,B01,256,1) mvc(256L,CAV1(a),1,MEMSET01);  R locnlx(a,w,1);}

// 18!:_4 locale header, including exec and del counts
F1(jtlochdr){
 ASSERT(AR(w)==0,EVRANK)
 RZ(vlocnl(0,w));
 A y;
 if(AT(w)&(INT|B01)){y=findnl(BIV0(w));  // if integer, look it up
 }else{
  w=AAV(w)[0];  // move to contents of box
  if(!AR(w)&&AT(w)&((INT|B01))){  // atomic numeric locale
   y=findnl(BIV0(w));  // OK because the boxed value cannot be virtual, thus must have padding
  }else{
   // string locale, whether number or numeric
   I m=AN(w); C *u=CAV(w); 
   ASSERT(vlocnm(m,u),EVILNAME);
   if(u[0]<='9') y=findnl(strtoI10s(m,u));
   else y=jtprobestlock((J)((I)jt+m),u,(UI4)nmhash(m,u));
  }
 }
 ASSERT(y!=0,EVLOCALE)  // fail if nonexistent
 R vec(INT,8,y);  // counts are in s[0]
}


F2(jtlocnl2){UC*u;
 ARGCHK2(a,w);
 ASSERT(LIT&AT(a),EVDOMAIN);
 A tmp; GAT0(tmp,B01,256,1) mvc(256L,CAV1(tmp),MEMSET00LEN,MEMSET00);
 u=UAV(a); DQ(AN(a),CAV1(tmp)[*u++]=1;);
 R locnlx(tmp,w,0); 
}    /* 18!:1 locale name list */

static A jtlocale(J jt,B b,A w){A g=0,*wv,y;
 if(((AR(w)-1) & -(AT(w)&(INT|B01)))<0){  // atomic integer is OK
  if(!((g=(b?jtstfindcre:jtstfind)(jt,-1,0,BIV0(w)))&&LOCPATH(g)))R 0;  // error if name not found/created or if name was found but deleted
 }else{
  RZ(vlocnl(1,w));
  wv=AAV(w); 
  DO(AN(w), y=AT(w)&BOX?C(wv[i]):sc(IAV(w)[i]); if(!((g=(b?jtstfindcre:jtstfind)(jt,AT(y)&(INT|B01)?-1:AN(y),CAV(y),AT(y)&(INT|B01)?BIV0(y):BUCKETXLOC(AN(y),CAV(y))))&&LOCPATH(g)))R 0;);
 }
 R g;
}    /* last locale (symbol table) from boxed locale names; 0 if none or error.  if b=1, create locale for each name */

// 18!:2 y  return boxed locale path.  y is locale name/number
DF1(jtlocpath1){AD * RESTRICT g; AD * RESTRICT z; F1RANK(0,jtlocpath1,self); RZ(vlocnl(1,w)); RZ(g=locale(1,C(w)));
 A *gp=LOCPATH(g);  // the path for the current locale.  It must be non0 normally, but if another deleted the locale while we are running it might be 0
 if(gp==0)RETF(mtv)   // if locale has been deleted, return something
 I npath; for(npath=0;*gp;--gp,++npath);  // npath is # names in path
 GATV0(z,BOX,npath,1); AFLAGINIT(z,BOX) A *zv=AAV1(z); gp+=npath;  // allocate result, point to input & output areas
 DO(npath, A t; RZ(t=sfn(0,LOCNAME(C(*gp)))); ACINITZAP(t); *zv++=t; --gp;)  // move strings except for the null terminator
 RETF(z); 
}
 // for paths, the shape holds the bucketx.  We must create a new copy that has the shape restored, and must incorporate it
     /* 18!:2  query locale path */

// null systemlock handler to wait for quiet system.  Used for changing locale path
static A jtnullsyslock(JTT* jt){R (A)1;}

DF2(jtlocpath2){A g,h; AD * RESTRICT x;
 F2RANK(1,0,jtlocpath2,self);
 ACVCACHECLEAR;  // changing any path invalidates all lookups
 RZ(g=locale(1,w));
 // The path is a recursive boxed list where each box is a SYMB type.  The usecount of each SYMB is incremented when it is added to the path.
 // When a locale is in a path the raised usecount prevents it from ever being deleted.  It has its Bloom filter zeroed so that it never searches for names
 // When all locales that have it in the path have been removed, the locale (including the name) will be deleted
 // The path is stored with one extra zero element at the end to allow for loop unrolling.
 // The path is allocated as a rank-1 list to keep it in a single cacheline
 GAT0(x,BOX,MAX(AN(a),1)+1,1); AFLAGINIT(x,BOX); A *xv=AAV1(x); // allocate enough locations, plus one.  Set as recursive.  xv->first slot
 *xv=0;  // install the terminating null in slot 0
 if(unlikely(AN(a)==0)){   // if path empty, leave it empty
 }else if(AN(a)==1){RZ(h=locale(1,a)); ra(h); *++xv=h;  // singleton, might be numeric atom - save it
 }else{A locatom;
  // more than one locale; must be list of boxes.  Go through the list, using a virtual block
  ASSERT(AT(a)&BOX,EVLOCALE);
  fauxblock(locfaux); fauxvirtual(locatom,locfaux,a,0,ACUC1) AK(locatom)+=AN(a)*SZI; AN(locatom)=1;  // create an faux atom, starting at beginning of a
  DO(AN(a), AK(locatom)-=SZI; RZ(h=locale(1,locatom)); if(likely(h!=g)){*++xv=h; ra(h);}) AN(x)=(xv-AAV1(x))+1;  // move locales for the names into the recursive path, but don't allow a locale in its own path
 }
 // xv points to end of path
 // We have the new path in x, and we can switch to it, but we have to call a system lock before we free the old path, to purge the old one from the system
 // This really sucks, but the alternative is to hold a lock on the path during the entire time the path is in use, which is worse.  better to
 // have a per-thread RFO flag indicating 'path in use', and wait here till all threads have been seen with that off
 // As a stopgap that will probably suffice forever, we avoid the system lock provided the new path merely extends the old from the beginning.  In that case,
 // we move the path pointer but we don't have to free anything, so we need no systemlock.  The test and exchange must be under a lock (which one isn't important,
 // because this is the only place that stores a non-PERMANENT path that might get freed) to avoid ABA trouble.  We still have to use exchange to set the path because
 // a deleting thread may be installing 0 or zpath
 A op=0; ACINITZAP(x);  // op is address of A for old path, if any.  Remove death warrant for new path, in case we store it
 WRITELOCK(JT(jt,locdellock))
 A *oldpath=__atomic_load_n(&LOCPATH(g),__ATOMIC_ACQUIRE);  // the path we are replacing
 if(oldpath==0){__atomic_store_n(&LOCPATH(g),xv,__ATOMIC_RELEASE); // if old path deleted, we can store new because no other path but 0 or zpath could be stored during our lock
 }else{
  // the path may be replaced while we are here, but only with 0 or zpath.  And in that case, the replacer will not free
  // or modify the old path without a systemlock.  We are free to extend the old path in place
  A *opv=oldpath; while(*opv)--opv; op=UNvoidAV1(opv);  // point opv to leading 0, and op to beginning of block
  if(ACISPERM(AC(op))){__atomic_store_n(&LOCPATH(g),xv,__ATOMIC_RELEASE); op=0;  // if path permanent, it's like 0 - ignore it
  }else{
   // we are replacing a non-permanent path.  See if we can extend the current path
   if(BETWEENO(AN(x),AN(op),allosize(op)>>LGSZI)){  // if the new path fits in the allocation...
    A *xv2=AAV1(x); DQ(AN(op)-1, if(*++opv!=*++xv2)goto noextend;)  // see if all the values in old path are the end of the new.  No need to check leading 0
    // extension is possible.  Move the rest of the new path to the old path (ra because recursive), update the old path, set new path pointer, free the no-longer-used new path
    // BUT: If the path has been replaced, we must not try to extend it, as it is about to be deleted.  In that case, we could install the new path, but we would have responsibility
    // for the path we replaced here; so we simply suppress changing the path, allowing the other change to have priority.  We use cas to avoid changing a changed path.
    // We need to update the path length so that all the newly-ra'd locales will be fa'd.
    DO(AN(x)-AN(op), A t=*++xv2; ra(t) *++opv=t;) __atomic_store_n(&AN(op),AN(x),__ATOMIC_RELEASE); __atomic_compare_exchange_n(&LOCPATH(g),&oldpath,opv,0,__ATOMIC_ACQ_REL,__ATOMIC_RELAXED);
    fa(x); op=0;  //op=0 to stop further update 
noextend: ;
   }
  }
 }
 // if op!=0, we have to install xv as the new path.  If the path has changed, we know it was changed to 0 or zpath, so we can ignore it; it is the other guy's responsibility to
 // free oldpath.  If the path hasn't changed, we have to free op.  This must be done under system lock, since we know op was not PERMANENT
 if(op!=0 && oldpath!=(A*)__atomic_exchange_n(&LOCPATH(g),xv,__ATOMIC_ACQ_REL))op=0;  // if path changed, suppress free below
 WRITEUNLOCK(JT(jt,locdellock))  // mustn't hold a lock when we call for systemlock
 if(op!=0){jtsystemlock(jt,LOCKPRIPATH,jtnullsyslock); fa(op)}
 R mtm;
}    /* 18!:2  set locale path */

// create named locale
// a is MARK (default size, permanent) or combined size/perm flag
static F2(jtloccre){A g,y,z=0;C*s;I n,p;A v;
 ARGCHK2(a,w);
 if(MARK&AT(a))p=JT(jt,locsize)[0]; else{RE(p=i0(a)); ASSERT(BETWEENC(p,-15,14),EVLIMIT);}
 y=C(AAV(w)[0]); n=AN(y); s=CAV(y); ASSERT(n<256,EVLIMIT);
 SYMRESERVE(2)  // make sure we have symbols to insert, for the locale itself
 A op=0;  // old path, if there is one
 WRITELOCK(JT(jt,locdellock)) WRITELOCK(JT(jt,stloc)->lock)  // take a write lock until we have installed the new locale if any.  No errors!  We need both locks, in this order (delete calls symfree, which takes locks in this order)
 if(v=jtprobe((J)((I)jt+n),s,(UI4)nmhash(n,s),JT(jt,stloc))){
  // named locale exists.  It may be zombie or not, but we have to keep using the same locale block, since it may be out there in paths
  g=v;
  A *gp=(A*)__atomic_exchange_n(&LOCPATH(g),0,__ATOMIC_ACQ_REL);  // pointer to path, and clear path to 0
  if(gp){
   // rare case of reinitializing a locale that has not been deleted.  This is allowed only if it has no symbols
   // verify locale is empty (if it is zombie, its hashchains are garbage - clear them)
   LX *u=SYMLINFOSIZE+LXAV0(g); DO(AN(g)-SYMLINFOSIZE, ASSERTGOTO(!u[i],EVLOCALE,exit););
   while(*gp)--gp; op=UNvoidAV1(gp);  // back up to the A block for the deleted path
  }else{
   ra(g);  // going from zombie to valid adds to the usecount
  }
  REINITZOMBLOC(g,p>=0);  // bring the dead locale back to life: clear chains, set path, clear Bloom filter
 }else{
  // new named locale needed
  I type=REPSGN(p);  // -1 if impermanent, 0 if permanent
  FULLHASHSIZE(1LL<<((REPSGN(p)^p)+5),SYMBSIZE,1,SYMLINFOSIZE,p);  // get table, size 2^p+6 minus a little
  if(unlikely(stcreate(type,p,n,s)==0))goto exit;   // create the locale, but if error, cause this routine to exit with failure
 }
 z=y;  // good return
exit:
 WRITEUNLOCK(JT(jt,locdellock)) WRITEUNLOCK(JT(jt,stloc)->lock)  // errors OK now
 if(unlikely(op!=0&&!ACISPERM(AC(op)))){jtsystemlock(jt,LOCKPRIPATH,jtnullsyslock); fa(op)}  // free old path after systemlock to ensure uses of path have been purged.  Mustn't hold lock
 R boxW(ca(z));  // result is boxed string of name - we copy it, perhaps not needed
}    /* create a locale named w with hash table size a */

static F1(jtloccrenum){C s[20];I k,p;A x;
 ARGCHK1(w);
 if(MARK&AT(w))p=JT(jt,locsize)[1]; else{RE(p=i0(w)); ASSERT(0<=p,EVDOMAIN); ASSERT(p<14,EVLIMIT);}
 FULLHASHSIZE(1LL<<(p+5),SYMBSIZE,1,SYMLINFOSIZE,p);  // get table, size 2^p+6 minus a little
 SYMRESERVE(1) RZ(x=stcreate(1,p,0,0L));  // make sure we have symbols to insert
 sprintf(s,FMTI,LOCNUM(x));   // extract locale# and convert to boxed string 
 R boxW(cstr(s));  // result is boxed string of name
}    /* create a numbered locale with hash table size n */

// 18!:3 create locale w, or numbered locale is w is empty
F1(jtloccre1){
 ARGCHK1(w);
 if(AN(w))R rank2ex0(mark,vlocnl(2+1,w),DUMMYSELF,jtloccre);  // if arg not empty, it is a list of locale names
 ASSERT(1==AR(w),EVRANK);
 R loccrenum(mark);
}    /* 18!:3  create locale */

F2(jtloccre2){
 ARGCHK2(a,w);
 if(AN(w))R rank2ex0(a,vlocnl(2+1,w),DUMMYSELF,jtloccre);  // if arg not empty, it is a list of locale names
 ASSERT(1==AR(w),EVRANK);
 R rank1ex0(a,DUMMYSELF,jtloccrenum);
}    /* 18!:3  create locale with specified hash table size */


// 18!:4 cocurrent/coclass.  Called only from unquote.  We return a flag requesting a change of locale
F1(jtlocswitch){A g;
 ARGCHK1(w);
 if(!(((AR(w)-1) & -(AT(w)&(INT|B01)))<0)){  // atomic integer/bool is OK as is
  // not a numeric atom.  perform boxxopen
  if(((AN(w)-1)|SGNIF(AT(w),BOXX))>=0)RZ(w=box(w));  // if not empty & not boxed, box it
  ASSERT(!AR(w),EVRANK);   // now always boxed: must be atom
 }
 RZ(g=locale(1,w));   // point to locale, if no error
 R (A)((I)g|1);  // set LSB as flag to unquote that we ran cocurrent
}
F1(jtlocname){A g=jt->global;
 ASSERTMTV(w);
 R boxW(sfn(0,LOCNAME(g)));
}    /* 18!:5  current locale name */

static SYMWALK(jtlocmap1,I,INT,18,3,1,
    {I t=AT(d->val);
     *zv++=i; 
     I zc; zc=(((1LL<<(ADVX-ADVX))|(2LL<<(CONJX-ADVX))|(3LL<<(VERBX-ADVX)))>>(CTTZ(((t&CONJ+ADV+VERB)|(1LL<<31))>>ADVX)))&3;   // ADVX, CONJx, VERBX, and the implied NOUNX=31 must all be >+ 2 bits apart
     zc=t==SYMB?6:zc; zc=t&(NOUN|VERB|ADV|CONJ|SYMB)?zc:-2;
     *zv++=zc;
     *zv++=(I)rifvs(sfn(SFNSIMPLEONLY,d->name));})  // this is going to be put into a box
     // produces a table of chain#,name class 0-3, 6 for symbol, _2 for other, pointer to name 

static F1(jtlocmaplocked){A g,q,x,y,*yv,z,*zv;I c=-1,d,j=0,m,*qv,*xv;
 ARGCHK1(w);
 ASSERT(!AR(w),EVRANK);
 RE(g=equ(w,zeroionei(0))?JT(jt,stloc):equ(w,zeroionei(1))?jt->locsyms:locale(0,w));
 ASSERT(g!=0,EVLOCALE);
 RZ(q=locmap1(g)); qv=AV(q);
 m=AS(q)[0];
 // split the q result between two boxes: first box is table of chain#,sym# in chain,class; second box is list of boxed names
 GATVR(x,INT,m*3,2,AS(q)); xv= AV2(x);
 GATV0(y,BOX,m,  1); yv=AAV1(y);
 DQ(m, *xv++=d=*qv++; *xv++=j=c==d?1+j:0; *xv++=*qv++; c=d; *yv++=incorp((A)*qv++););
 GAT0(z,BOX,2,1); zv=AAV1(z); zv[0]=incorp(x); zv[1]=incorp(y);
 R z;
}    /* 18!:_1 locale map */
F1(jtlocmap){READLOCK(JT(jt,stlock)) READLOCK(JT(jt,stloc)->lock) READLOCK(JT(jt,symlock)) A z=jtlocmaplocked(jt,w); READUNLOCK(JT(jt,stlock)) READUNLOCK(JT(jt,stloc)->lock) READUNLOCK(JT(jt,symlock)) R z;}

// recalculate Bloom filter in table w
SYMWALK(jtaccumbloom,B,B01,0,0,(LOCBLOOM(w)|=BLOOMMASK(NAV(d->name)->hash))&&0,;)

// 18!:6 reset Bloom filter.  Result=old filter.
F1(jtresetbloom){A g;
 ARGCHK1(w);
 ASSERT(!(((AR(w)-1) & -(AT(w)&(INT|B01)))<0),EVDOMAIN)   // always error (numbered locale)
 // not a numeric atom.  perform boxxopen
 if(((AN(w)-1)|SGNIF(AT(w),BOXX))>=0)RZ(w=box(w));  // if not empty & not boxed, box it
 ASSERT(!AR(w),EVRANK);   // now always boxed: must be atom
 RZ(g=locale(0,w));   // point to locale, if no error
 ASSERT(AR(g)&ARNAMED,EVDOMAIN)  // locale must be named
 I oldbloom=LOCBLOOM(g); LOCBLOOM(g)=0;  // get old value of Bloom filter, init new
 jtaccumbloom(jt,0,g);  // roll up all Bloom filters
 RETF(sc(oldbloom));  // return old Bloom
}

#if 0  // withdrawn
// 18!:7 make locale permanent
F1(jtsetpermanent){A g;
 ARGCHK1(w);
 ASSERT(!(((AR(w)-1) & -(AT(w)&(INT|B01)))<0),EVDOMAIN)   // always error (numbered locale)
 // not a numeric atom.  perform boxxopen
 if(((AN(w)-1)|SGNIF(AT(w),BOXX))>=0)RZ(w=box(w));  // if not empty & not boxed, box it
 ASSERT(!AR(w),EVRANK);   // now always boxed: must be atom
 RZ(g=locale(0,w));   // point to locale, if no error
 ASSERT(AR(g)&ARNAMED,EVDOMAIN)  // locale must be named
 LXAV0(g)[SYMLEXECCT]=EXECCTPERM+10000;  // mark the execct as never to be modified, with some slop in case there are execct deletions coming
 RETF(mtm);  // empty return
}
#endif



 SYMWALK(jtredefg,B,B01,100,1,1,RZ(redef((zv,mark),d->val)))
     /* check for redefinition (erasure) of entire symbol table. */

// 18!:55 destroy locale(s) from user's point of view.  This counts as one usecount; others are in execution and in paths.  When all go to 0, delete the locale
// Bivalent: if x is 271828, do the deletion even if on a permanent locale (for testcases only)
DF2(jtlocexmark){A g,*wv,y,z;B *zv;C*u;I i,m,n;
 ACVCACHECLEAR;  // destroying a locale invalidates all lookups
 if(unlikely(EPDYAD)){  // dyadic call
  I x; x=i0(a); if(jt->jerr){RESETERR; ASSERT(0,EVVALENCE)} ASSERT(x==271828,EVVALENCE)  // if not 271828, valence error
  // leave a non0 to indicate dyadic call
 }else{w=a; a=0;}  // a=0 normally
 RZ(vlocnl(1,w));
 if(ISDENSETYPE(AT(w),B01))RZ(w=cvt(INT,w));  // Since we have an array, we must convert b01 to INT
 n=AN(w); wv=AAV(w); 
 I zr=AR(w); GATV(z,B01,n,AR(w),AS(w)); zv=BAVn(zr,z);  // result area, all 1s
 // Do this in a critical region since others may be deleting as well.  Any lock will do.  We don't
 // use stloc->lock because most deletions are of numbered locales & we want to keep stloc available for named
 WRITELOCK(JT(jt,locdellock))
 for(i=0;i<n;++i){  // for each
  g=0;
  if(AT(w) & (INT)){zv[i]=1; g = findnl(IAV(w)[i]);  // integer, look up numbered locale
  }else{   // boxed...
   zv[i]=1; y=C(wv[i]);
   if(AT(y)&(INT|B01)){g = findnl(BIV0(y));  // boxed integer, look up numbered locale
   }else{
    m=AN(y); u=CAV(y);
    ASSERTSUFF(m<256,EVLIMIT,z=0; goto exitlock;);
    if('9'>=*u){g = findnl(strtoI10s(m,u));}  // boxed numeric string, look up numbered
    else {A v=jtprobestlock((J)((I)jt+m),u,(UI4)nmhash(m,u)); if(v)g=v;}  // g is locale block for named locale
   }
  }
  if(g){I k;  // if the specified locale exists in the system...
   if(a){ASSERTSUFF(*JT(jt,zpath)!=g,EVRO,z=0; goto exitlock;) LXAV0(g)[SYMLEXECCT]=0; locdestroy(g);  // forced delete, clear execct/del ct & do it (but error if deleting z)
    // ignore the deletion if the locale is execct-permanent.  The execct is unreliable then
   }else DELEXECCTIF(g)  // Delete if execct allows (never if permanent).  Say that the user doesn't want this locale any more.  Paths, execs, etc. still might.
  }
 }
exitlock:
 WRITEUNLOCK(JT(jt,locdellock))
 R z;
}

// destroy symbol table g, which must be named or numbered and must be 'out of execution' (i. e. execct=0)
// We cannot simply delete the symbol table, because it may be extant in paths.  So we empty the locale, and clear its path to 0
// to indicate that it is a zombie.  We reset the Bloom filter to make sure the path doesn't respond to any names.  We also reduce the
// usecount.  When the locale is no longer in any paths, it will be freed along with its name.
B jtlocdestroy(J jt,A g){
 // see if the locale has been deleted already.  Return fast if so
 A *path=(A*)__atomic_exchange_n(&LOCPATH(g),0,__ATOMIC_ACQ_REL);
 if(unlikely(path==0))R 1;  // already deleted - can't do it again
 // The path was nonnull, which means the usecount had 1 added correspondingly.  That means that freeing the path cannot make
 // the usecount of g go to 0.  (It couldn't anyway, because any locale that would be deleted by a fa() must have had its path cleared earlier)
 freesymb(jt,g);   // delete all the names.  Anything executing will have been fa()d
 while(*path)--path; fa(UNvoidAV1(path))   // delete the path too.  block is recursive; must fa() to free sublevels
 // Set path pointer to 0 (above) to indicate it has been emptied; clear Bloom filter.  Leave hashchains since the Bloom filter will ensure they are never used
 LOCBLOOM(g)=0;
 // lower the usecount.  The locale and the name will be freed when the usecount goes to 0
 fa(g);
 // The current implied locale can be deleted only after it leaves execution, i. e. after it returns to immex in all threads where it is current.
 // We are allowed to change it then because it is not stacked anywhere.  We change to z so that (1) we can be assured jt->global is always valid;
 // (2) the user doesn't have the disorienting experience of no locale and no path.
 if(unlikely(g==jt->global))SYMSETGLOBALS(jt->locsyms,*JT(jt,zpath));  // if we deleted the global locale, throw us into z locale (which is permanent)
 R 1;
}    /* destroy locale jt->callg[i] (marked earlier by 18!:55) */
