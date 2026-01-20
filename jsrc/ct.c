/* Copyright (c) 1990-2026, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Threads and Tasks                                                       */

#define _GNU_SOURCE
#include "j.h"

// burn some time, approximately n nanoseconds
NOINLINE I johnson(I n){I johnson=0x1234; if(n<0)R n; do{johnson ^= (johnson<<1) ^ johnson>>(BW-1);}while(--n); R johnson&-256;}  // return low byte 0
#if PYXES
void delay(I n){if(uncommon(n>36))DONOUNROLL((n-7)/36,_mm_pause();)else johnson(n);}
#else
#define delay(n)
#endif
//36ns TUNE; ~60clk on zen, ~160clk on intel; consider adding more general uarch tuning capabilities (eg for cache size)
//7ns mispredict penalty (15-20clk) + mul latency (3clk)

// Extend a hashtable/data table under lock.  abuf is the pointer to the block to be extended (*abuf will hold the new block address).
// *alock is the lock to use.  We hold a writelock on *alock on entry, but we may relinquish inside this routine.
// On exit we hold the write lock UNLESS there was an error, in which case we return NOT holding the lock (to allow the caller to abort on error)
// flags&1 is 1 if *abuf is a hashtable.  In that case, fill it with -1 and set AM to 0 to indicate a rehash is needed.  Otherwise copy the old contents to the beginning of the resized table.
// flags&2 is set to suppress freeing the old block after setting the new one
// flags&4 is set when flags&1 is set to indicate the hash table should fill with 0 rather than -1 (for address rather than index hashes)
// flags&8 is  set if the block has rank 1 and needs to keep AS[0] unchanged so it can be used in J verbs
// result is 0 if we hit an error, otherwise the table has been resized, but not necessarily by us & it might not have enough space.
// The tables resized here are allocated with any rank.  AN()/AS() (if present) gives the current allocation, and AM() gives the number of items actually in use
// When a table is resized, it is mf()'d without recurring to contents.  This means it must not be in use otherwise, for example as a result or a backer
I jtextendunderlock(J jt, A *abuf, US *alock, I flags){A z;
 I oldn=AN(*abuf);   // get the previous allocated size
 I t=AT(*abuf);  // get the type of the allocation
 WRITEUNLOCK(*alock);  // relinquish lock while we allocate the new area
 GA0(z,t,2*oldn,AR(*abuf)); ACINITUNPUSH(z); // allocate a new block, bigger than the previous one; make it permanent.  This is the only error exit from this routine
 WRITELOCK(*alock);  // reacquire lock
 A obuf=*abuf;  // current buffer after reacquiring lock
 AFLAGINIT(z,AFLAG(obuf));  // preserve the recursive status of the block since we are transferring contents
 if(AN(obuf)==oldn){
  // normal case where no one else resized the block before we did.  Transfer the data and counts from the old block to the new
  I itemsize;
  I nvaliditems=AM(obuf);  // remember number of valid items in the old block
  I datasize=allosize(z);  // number of bytes in data area
  I alloatoms=datasize>>bplg(t);   // advance AN to max allocation - leaving no buffer at the end
  // if this allocation is a table, fill in AN and AS[0] (otherwise AN is all we need)
  // if there are elements of the shape beyond AS[0], they must be parameters or item shape and they are just copied.  If rank=1, AS[0] is also such a parameter
  if(AR(z)>1){itemsize=AN(obuf)/AS(obuf)[0]; AS(z)[0]=alloatoms/itemsize; AN(z)=AS(z)[0]*itemsize; DONOUNROLL(AR(z)-1, AS(z)[i+1]=AS(obuf)[i+1];)
  }else{itemsize=1; AN(z)=alloatoms; AS(z)[0]=flags&8?AS(obuf)[0]:alloatoms;}  // if rank=1, AS[0] may be a user field.  If rank=0 it will be overwritten
  // if the type is boxed, and we extended the allocation, we had better clear the added atoms in case we ever free the block elsewhere
  if(unlikely(t&BOX))mvc((AN(z)-2*oldn)*BOXSIZE,AAV(z)+2*oldn,MEMSET00LEN,MEMSET00);
  if(flags&1){
   // If the block is a hashtable, it will be rebuilt from scratch and we just initialize it to -1 pointers
   mvc(datasize,voidAV(z),1,(flags&4)?MEMSET00:MEMSETFF);  // fill the entire table
   AM(z)=0;  // indicate the whole hash is invalid after resize
  }else{
   MC(voidAV(z),voidAV(obuf),itemsize*(nvaliditems<<bplg(t)));  // copy the valid data.  Rest can be left garbage
   AM(z)=nvaliditems;  // transfer the count of allocated atoms, now valid
  }
  // release the old block.  We assume that freeing a block will not permanently block this thread
  __atomic_store_n(abuf,z,__ATOMIC_RELEASE); if(!(flags&2))mf(obuf);  // mf, not fa, so we don't recur to recursible children that have been copied and are still valid.   The free is optional
 }else{
  // somebody else has already resized the block.  We just back off and hope they resized it enough.
  mf(z);  // discard the block we allocated
 }
 R 1;  // normal return
}

// ************************ system lock **********************************
#define POLLDELAY delay(20);

// perform the action for state n.  In the leader, set ct/advance to state n/action/wait, running wtact  In others, wait for state n/action/decr ct.  When we finish all actors have performed the action for state n, and state is n
#define DOINSTATE(l,n,expr,wtact) \
 {if(l){__atomic_store_n(&JT(jt,systemlocktct),nrunning-1,__ATOMIC_RELEASE); __atomic_store_n(&JT(jt,systemlock),(n),__ATOMIC_RELEASE);}else while(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)!=(n)){YIELD}  \
  expr \
  if(l)while(__atomic_load_n(&JT(jt,systemlocktct),__ATOMIC_ACQUIRE)!=0){wtact YIELD} else __atomic_fetch_sub(&JT(jt,systemlocktct),1,__ATOMIC_ACQ_REL); \
 }



// Similar function, for systemlockaccept
#define DOINSTATEA(n,expr) {while(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)!=(n))YIELD expr __atomic_fetch_sub(&JT(jt,systemlocktct),1,__ATOMIC_ACQ_REL);}
// wake all threads currently waiting on a futex.  wakea can do extraneous work; mac/linux have a way to wake just one thread.  But we want to wake up everybody anyway, so it makes not much difference
// we increment the store counter in the futex before we wake, in case a thread has sampled the system info but not yet waited
#if PYXES
// So that we wake each thread only once, we clear the futexwt when we wakeup.  wta is a pointer to the state of the waiting pyx, used as the futex
static void wakeall(J jt){UI4 *wta;
 DONOUNROLL(NALLTHREADS(jt),
   __atomic_fetch_or(&JTTHREAD0(jt)[i].taskstate,TASKSTATEFUTEXWAKE,__ATOMIC_ACQ_REL);
   if((wta=lda(&JTTHREAD0(jt)[i].futexwt))!=0){sta(&JTTHREAD0(jt)[i].futexwt,0); aadd(wta,0x100); jfutex_wakea(wta);}
   __atomic_fetch_and(&JTTHREAD0(jt)[i].taskstate,~TASKSTATEFUTEXWAKE,__ATOMIC_ACQ_REL);)
}
#else
static void wakeall(J jt){}
#endif

// Take lock on the entire system, waiting till all threads acknowledge
// priority is the priority of the request.  lockedfunction is the function to call when the lock has been agreed.
// if multiple requesters ask for a lock of the same priority, the function will be called in only one of them
// lockedfunction should return 0 for error, otherwise the value to use.  The return to the caller depends on jerr and
// whether the thread ran the function: in the thread that ran the function, value/error from lockedfunction is passed through;
// in other threads, 1 is returned always with no error signaled
A jtsystemlock(J jt,I priority,A (*lockedfunction)(J)){A z;
 // If the system is already in systemlock, the system is essentially single-threaded.  Just execute the user's function.
 // This would happen if a sentence executed in debug suspension needed symbols, or had an error
 if(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)>2){R (*lockedfunction)(jt);}
 // Process the request.  We don't know what the highest-priority request is until we have heard from all the
 // threads.  Thus, it is possible that our request will still be pending whe we finish.  In that case, loop till it is satisfied
 while(priority!=0){
  // go to state 1; set leader if we are the first to do so
  I leader=__atomic_compare_exchange_n(&JT(jt,systemlock), &(S){0}, (S)1, 0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED);
  I nrunning=0; JTT *jjbase=JTTHREAD0(jt);  // #running threads, base of thread blocks
  // In the leader task only, go through all tasks (including master), turning on the SYSLOCK task flag in each thread.  Count how many are running after the flag is set
  // Also, wake up all tasks that are in a loop that needs interrupting on system action.  Those loops will honor it when we are in state 1/2
  // We take a lock on the thread info to ensure a thread is not added while we are counting
  I nlocked;  // in the leader and executor, the number of threads that were found when we started
  if(leader){WRITELOCK(JT(jt,flock)) DONOUNROLL(nlocked=NALLTHREADS(jt), nrunning+=(__atomic_fetch_or(&jjbase[i].taskstate,TASKSTATELOCKACTIVE,__ATOMIC_ACQ_REL)>>TASKSTATERUNNINGX)&1;) WRITEUNLOCK(JT(jt,flock))}
  // state 2: lock requesters indicate request priority and we wait for all tasks to come to a stop.  We wake all threads that are waiting on pyx/mutex, which is harder
  // than it sounds: we don't know immediately whether a thread has gone to wait, because there may be delay in our seeing the futexwt.  We repeatedly wake up the waiting threads until
  // all the active ones have entered systemlock
  C oldpriority; DOINSTATE(leader,2,oldpriority=__atomic_fetch_or(&JT(jt,adbreak)[1],priority,__ATOMIC_ACQ_REL);,wakeall(jt);)  // remember priority before we made our request
  // state 3: all threads get the final request priorities
  C finalpriority; DOINSTATE(leader,3,finalpriority=__atomic_load_n(&JT(jt,adbreak)[1],__ATOMIC_ACQUIRE);,)
  I winningpri=LOWESTBIT(finalpriority); I executor=winningpri&priority&~oldpriority;  // priority to execute: were we the first to request it?
  // state 4: transfer nrunning/nlocked to executor and run the locked function.  Other tasks must do nothing and wait for state 5.
  if(leader){__atomic_store_n(&JT(jt,systemlocktct),nrunning,__ATOMIC_RELEASE); __atomic_store_n(&JT(jt,systemlockthreadct),nlocked,__ATOMIC_RELEASE); __atomic_store_n(&JT(jt,systemlock),4,__ATOMIC_RELEASE);}  // leader advances to state 4
  if(executor){  // if we were the first to request the winning priority
   // This is the winning thread.  Perform the function and save the error status
   while(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)!=4)YIELD nrunning=__atomic_load_n(&JT(jt,systemlocktct),__ATOMIC_ACQUIRE); nlocked=__atomic_load_n(&JT(jt,systemlockthreadct),__ATOMIC_ACQUIRE);  // executor waits for state 4 and picks up nrunning/nlocked.
   // remove the lock request from the break field so that it doesn't cause the function to think a lock is requested
   __atomic_store_n(&JT(jt,adbreak)[1],0,__ATOMIC_RELEASE);
   z=(*lockedfunction)(jt);  // perform the locked function
   __atomic_store_n(&((C*)&JT(jt,breakbytes))[1],jt->jerr,__ATOMIC_RELEASE);  // make the error status available to all threads
  }
  // state 5: everybody gets the result of the operation
  DOINSTATE(executor,5,__atomic_load_n(&((C*)&JT(jt,breakbytes))[1],__ATOMIC_ACQUIRE);,)
  // Now wind down the lock.  systemlocktct is known to be 0.  Turn off all the LOCK bits and then set state to 0.  Other tasks will
  // wait for state to move off 5.  There is no guarantee they will see state 0 of the next systemlock, but state cannot advance beyond 1 until they have finished this one.
  // There is also no guarantee they will see their LOCK removed
  if(executor){
   __atomic_store_n(&((C*)&JT(jt,breakbytes))[1],0,__ATOMIC_RELEASE);  // clear the error flag from the interrupt request
   // go through all threads, turning off SYSLOCK in each.  This allows other tasks to run and new tasks to start.
   // threads may have been marked for deletion, or even deleted, but we must turn off the lock bit in every thread where we set it
   DO(nlocked, __atomic_fetch_and(&jjbase[i].taskstate,~TASKSTATELOCKACTIVE,__ATOMIC_ACQ_REL);)
   // set the systemlock to 0, completing the operation
   __atomic_store_n(&JT(jt,systemlock),0,__ATOMIC_RELEASE);
  }else{
   // outside the executor, we wait for state to move off 5
   while(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)==5)YIELD
   z=(A)1;  // just use a value of 1 to indicate we were not the executing thread.  The real z goes to the executor
   // NOTE that a non-executor thread here does not use the error status
  }
  priority&=~winningpri;  // if our request was serviced, remove it, regardless of who serviced it
 }
 // here a request was processed at the level we requested.  We have the value to return; error
 // is available but has been signaled to the executor only
 R z;  // return to requester
}

// Allow a system lock to proceed.  Called by a running thread when it notices the broadcast system-lock request at its priority or higher
// priority is mask indicating the priorities for which this accept is valid.  smaller bit-values have higher priority
// Result is 0 if the lock processing failed
I jtsystemlockaccept(J jt, I priority){
 do{C finalpriority; C res;
  DOINSTATEA(2,)  // state 2: requests at different priorities
  DOINSTATEA(3,finalpriority=__atomic_load_n(&JT(jt,adbreak)[1],__ATOMIC_ACQUIRE);)  // state 3: get winning priority
  // state 4: transfer nrunning to executor and run the function.  Other threads wait for the result
  DOINSTATEA(5,res=__atomic_load_n(&((C*)&JT(jt,breakbytes))[1],__ATOMIC_ACQUIRE);)// state 5: everybody gets the result of the operation
  while(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)==5)YIELD  // wait for state to move off 5, indicating completion
  ASSERT(res==0,res)  // if there was an error, signal it in all acceptor threads
  // this thread is free to continue.  systemlock has been cleared
  // loop back if there is another request that this can tolerate
  I winningpri=LOWESTBIT(finalpriority); finalpriority&=~winningpri; priority&=finalpriority; // final<-remaining requests; leave priority as the ones we are OK with
 }while(priority);  // if our priority was SYM-only and the remaining request is DEBUG, we have to return to get to a DEBUG point
 R 1;
}

// ****************************** waiting for values *******************************

typedef struct pyxcondmutex{
 // The PYXBLOK is the first thing is a BOX block with AN=1.  Thus the first field in the PYXBLOK
 // must be the value, which will be freed when the PYXBLOK is freed
 A pyxvalue;  // the A block of the pyx/emsg, when it is filled in.  It is 0 until then.
 float pyxmaxwt;  // max time to wait for this pyx in seconds
 S pyxorigthread;  // thread number that is working on this pyx, or _1 if the value is available
 C errcode;  // 0 if no error, or error code
#if PYXES
 UI4 seqstate;//one of the below pyx states
#endif
} PYXBLOK;
enum{  // pyx state is low byte of seqstate.  High 3 bytes are the wakeup sequence number
 PYXEMPTY=0, //the pyx is not filled in, and no one is waiting
 PYXWAIT=3,  //at least 1 thread is waiting, and the pyx is not filled in.  We can OR WAIT into pyx state to move to WAIT state
 PYXFULL=1}; //the pyx is filled in
#if PYXES

// Install a value/errcode into a (recursive) pyx, and broadcast to anyone waiting on it.
// If the value has been previously installed (invalid, and possible only with user pyxes), return abort code 0, otherwise 1
// if errcode!=0, change z to be an A block for the error message in this thread
static I jtsetpyxval(J jt, A pyx, A z, C errcode){I res=1;
 S prevthread=__atomic_exchange_n(&((PYXBLOK*)AAV0(pyx))->pyxorigthread,-1,__ATOMIC_ACQ_REL);  // set pyx no longer running
 if(unlikely(prevthread<0))R 0;  // the pyx is read-only once written
 ASSERTSYS(((UI)z|(UI)errcode)!=0,"error pyx with no errorcode");
 if(unlikely(z==0)){
  // error.  Remember the entire message that has been formatted into etx, and pass that string as the result value.  The error code will indicate the semantics
  __atomic_store_n(&((PYXBLOK*)AAV0(pyx))->errcode,errcode,__ATOMIC_RELEASE);  // copy failure code.  Must be non0 - if not that is itself an error above
  z=str(jt->etxn,jt->etxinfo->etx);   // previous error text
 }
 razap(z);    // since the pyx is recursive, we must ra the result/emsg we store into it.  We transfer ownership to the pyx
 __atomic_store_n(&((PYXBLOK*)AAV0(pyx))->pyxvalue,z,__ATOMIC_RELEASE);  // set result/emsg value
 // broadcast to wake up any tasks waiting for the result
 if(PYXWAIT==xchga((C*)&((PYXBLOK*)AAV0(pyx))->seqstate,PYXFULL))jfutex_wakea(&((PYXBLOK*)AAV0(pyx))->seqstate);
R 1;
}

// Allocate a pyx, marked as owned by (thread).  The tstack may have been hijacked (OK since we do just the 1 GA).  Set usecount to noninplaceable since it might be in a box at birth
static A jtcreatepyx(J jt, I thread,D timeout){A pyx;
 // Allocate.  Init value, cond, and mutex to idle
 GAT0(pyx,INT,((sizeof(PYXBLOK)+(SZI-1))>>LGSZI)+1,0); AAV0(pyx)[0]=0; // allocate the result pointer (1), and the cond/mutex for the pyx.
 ((PYXBLOK*)AAV0(pyx))->seqstate=PYXEMPTY;
 // Init the pyx to a recursive box, with raised usecount.  AN=1 always.  Set the value/errcode to NULL/no error and the thread# to the executing thread
 AT(pyx)=BOX+PYX; AFLAG(pyx)=BOX; ACINIT(pyx,ACUC1); AN(pyx)=1; ((PYXBLOK*)AAV0(pyx))->pyxvalue=0; ((PYXBLOK*)AAV0(pyx))->pyxorigthread=thread; ((PYXBLOK*)AAV0(pyx))->errcode=0;  ((PYXBLOK*)AAV0(pyx))->pyxmaxwt=timeout;
 // The pyx's usecount must be raised before it is sent to a task, because there are nominally 2 owners: one for the task thread and one for the current thread, which has something - either a tpop for the pyx
 // or an enclosing box - that protects it until it is put into its box.  When the pyx is filled in the owner will remove the protection (by fa() for either the pyx or the enclosing box)
 R pyx;
}

// w is an A holding a pyx value.  Return its value when it has been resolved, or 0 if error, with error code set.
// EVTIME if timeout
A jtpyxval(J jt,A pyx){UI4 state;PYXBLOK *blok=(PYXBLOK*)AAV0(pyx);
 if(PYXFULL==(state=lda((C*)&blok->seqstate)))goto done; // if pyx already full, return result
 {C dummy=PYXEMPTY;casa(state!=PYXEMPTY?&dummy:(C*)&blok->seqstate,&dummy,PYXWAIT);}  // if pyx is EMPTY, move it to WAIT.  Avoid excess contention on hot pyxes
 UI ns=({D mwt=blok->pyxmaxwt;mwt==inf?IMAX:(I)(mwt*1e9);}); // figure out how long to wait
 struct jtimespec end=jtmtil(ns); // get the time when we have to give up on this pyx
 I err;
 while(1){ // repeat till state goes to FULL
  // The wait may time out because of a pending system action (BREAK or system lock).  If so, we accept it now...
  UI4 state=lda(&blok->seqstate); C breakb;  // get store sequence # before we check for system event
  if(PYXFULL==((C)state))break; // if pyx was filled, exit and return its value
  // if someone is starting a system lock, we must go accept it.  To avoid reads from other cores, we look at our taskstate, where the lock requester
  // tell us that a lock is running.  In normal use we will be the owner of this cacheline already.  It is not vital to get this right the first time, because the
  // lock code will wake us 
  if(lda(&jt->taskstate)&TASKSTATELOCKACTIVE){
   if(unlikely(JT(jt,systemlock)>2)){err=EVDEADLOCK; goto fail;}  // if in suspension, the pyx cannot be filled if we block, since the thread is paused
   jtsystemlockaccept(jt,LOCKALL);  // process systemlock and keep waiting.
  }
  // the user may be requesting a BREAK interrupt for deadlock or other slow execution
  if(unlikely(lda(&JT(jt,adbreak)[0])>1)){err=EVBREAK;goto fail;} // JBREAK: give up on the pyx and exit
  if(uncommon(-1ull==(ns=jtmdif(end)))){ //update time-until-timeout.  If the time has expired...
   if(unlikely(inf==blok->pyxmaxwt))ns=IMAX;  // if time wrapped around, reset to infinite
   else{err=EVTIME;goto fail;}} // otherwise, timeout, fail the pyx and exit
  sta(&jt->futexwt,&blok->seqstate); // make sure systemlock knows how to wake us up.  It will clear this field after the wakeup, so we get only one wakeup per systemlock
  I wr=jfutex_waitn(&blok->seqstate,state|PYXWAIT,MIN(ns,1000000000));if(unlikely(wr>0)){err=EVFACE;goto fail;} // wait on futex.  If new event# or state has moved off of WAIT, don't wait
   // because ATTN doesn't wake tasks, we have to check for JBREAK every so often
 }
 // We have the value; but if someone has started a system lock, they may erroneously try to wake up our futex, which is no longer needed.  We clear the futex pointer,
 // and then wait until system lock is over
 CLRFUTEXWT;   // clear the futex wait till no one is using it
done:  // pyx has been filled in.  jt->futexwt must be 0
 if(likely(blok->errcode==0))R blok->pyxvalue; // valid value, use it
 ASSERTPYX(blok->errcode,blok->pyxvalue); // error: signal the error in the reader, noting that it was externally generated
fail:
 CLRFUTEXWT;ASSERT(0,err);}
// w is an A holding a pyx value.  Return its value when it has been resolved, otherwise 0
// EVTIME if timeout
A jtpyxvalid(J jt,A pyx){R ((PYXBLOK*)AAV0(pyx))->pyxvalue;}

// ************************************* Locks **************************************
// take a readlock on *alock.  We come here only if a writelock was requested or running.  We have incremented the readlock
void readlock(S *alock, S prev){
 // loop until we get the lock
 do{
  __atomic_fetch_sub(alock,1,__ATOMIC_ACQ_REL);  // rescind our read request.  The writer may hitch slightly when he sees our request, but we won't put it up more than once
  // spin until any write request has gone away
  I nspins=50;  // good upper bound on the amount of time a write could reasonably take, in poll delays
  while(prev&-WLOCKBIT){
   // we are delaying while a writer finishes.  Usually this will be fairly short, as controlled by nspins.  The danger is that the
   // writer will be preempted, leaving us in a tight spin.  If the spin counter goes to 0, we decide this must have happened, and we
   // do a low-power delay for a little while (method TBD)
   if(--nspins==0){nspins=50; YIELD}
   POLLDELAY  // delay a little to reduce bus traffic while we wait for the writer to finish
   prev=__atomic_load_n(alock,__ATOMIC_ACQUIRE);
  }
  // try to reacquire the lock, loop if can't
 }while((prev=__atomic_fetch_add(alock,1,__ATOMIC_ACQ_REL)&(S)-WLOCKBIT)!=0);
}

// take a writelock on *alock.  We have turned on the write request; we come here only if the lock was in use.  The previous value was prev
void writelock(S *alock, S prev){
 // loop until we get the lock
 I nspins;
 while(prev&(S)-WLOCKBIT) {
 // Another writer requested before us.  They win.  wait until they finish.  As above, back off if it looks like they were preempted
  nspins=prev&(WLOCKBIT-1)?50+10:50;  // max expected writer delay, plus reader delay if there are readers, in 20-ns units
  while(prev&(S)-WLOCKBIT){
   if(--nspins==0){nspins=50; YIELD}
   POLLDELAY  // delay a little to reduce bus traffic while we wait for the writer to finish
   prev=__atomic_load_n(alock,__ATOMIC_ACQUIRE);  // loop without RFO cycle till the other writer goes away
  }
  // try to reacquire the writelock.  When the holder releases it, all requests are cleared
#if WLOCKBIT==0x8000
  prev=__atomic_fetch_or(alock,(S)WLOCKBIT,__ATOMIC_ACQ_REL);
#else
  prev=__atomic_fetch_add(alock,WLOCKBIT,__ATOMIC_ACQ_REL);
#endif
  // that repeats our request
 }
 // We are the owner of the current write request.  When the reads finish, we have the lock
 nspins=20;  // max expected reader delay, in poll delays.  They are all running in parallel
 while(prev&(WLOCKBIT-1)){  // wait until reads complete
  if(--nspins==0){nspins=20; YIELD}  // delay if a thread seems to have been preempted
  POLLDELAY  // delay a little to reduce bus traffic while we wait for the readers to finish
  prev=__atomic_load_n(alock,__ATOMIC_ACQUIRE);  // no need to RFO: once readers finish we have it
 }
 // Readers have finished; we get the lock.  The read count may go non0 while we run, but we won't look
}

// *********************** task creation ********************************

// The RUNNING flag must not be changed while a system lock is in progress, because the lock owner knows how many active tasks there are
// set running, returning 1 if it wasn't set already
I jtsettaskrunning(J jt){C oldstate;
 if(unlikely(jt->taskstate&TASKSTATERUNNING))R 0;   // if for some reason we are called with the bit already set, keep it set and indicate it wasn't us that set it
 // go to RUNNING state; but we are not allowed to change state if LOCKACTIVE has been set in our task.  In that
 // case someone has started a system lock and our running status has been captured.  LOCKACTIVE is set in state 1 and removed in state 5.  We must
 // first wait for the lock to clear and then wait to get out of state 5 (so that we don't do a systemlock request and think we are single-threaded)
 while(oldstate=jt->taskstate&~TASKSTATELOCKACTIVE, !__atomic_compare_exchange_n(&jt->taskstate, &oldstate, oldstate|TASKSTATERUNNING, 0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED)){
  if(unlikely(oldstate&TASKSTATERUNNING))R 0;   // if for some reason we are called with the bit already set, keep it set and indicate it wasn't us that set it
  if(unlikely(oldstate&TASKSTATELOCKACTIVE)){YIELD delay(1000);}
 }
 while(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)==5)YIELD   // active set; wait till system lock is idle before allowing continuation
 R 1;
}
void jtclrtaskrunning(J jt){C oldstate;
 if(unlikely(!(jt->taskstate&TASKSTATERUNNING)))R;   // if for some reason we are called with the bit already off, keep it off
 // go back to non-RUNNING state, but if SYSTEMLOCK has been started with us counted active go handle it
 while(oldstate=jt->taskstate&~TASKSTATELOCKACTIVE, !__atomic_compare_exchange_n(&jt->taskstate, &oldstate, oldstate&~TASKSTATERUNNING, 0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED)){
  if(likely(oldstate&TASKSTATELOCKACTIVE)){jtsystemlockaccept(jt,LOCKALL);}else{YIELD delay(1000);}
 }
}

// block describing a waiting/executing job/task
// this is aligned to a cacheline boundary, but to shorten the block the pyx of a user job is pointed to by the AM field of the
// enclosing AD and the global symbols are pointed to by AK.  AR is set to 0 if the system must kick threads after removing this job
typedef struct jobstruct {
 JOB *next; // points to the block containing the next job
 I4 n;  // number of tasks in this job.  If <=0, this is a user job; if 0x8..0, this is user job with locales;
 US initthread;  // thread# of the thread that started this job
 // 2 bytes free
 UI ns_mask;  // (normal) number of tasks already started for this job; (user job with locales) mask of threads that have already processed this job
 union {
  struct {
   A args[3];  // w,u,u if monad; a,w,u if dyad
   I inherited[(offsetof(JTT,uflags.init0area)+SZI-1)>>LGSZI]; // inherited sections of JT, plus a bit.
   //  AM(job) is *pyx, AK is jt->global.  For user jobs with locales, AM is *pyx array, AN is mask of enabled threads, AK is *locale#s A, which must be freed
  } user;
  struct uiint {
   unsigned char (*f)(J jt,void *ctx,UI4 i);  // function to do 1 internal task. C is the error code, 0=OK. i is the task# within this job
   void *ctx;   // info needed by the task
   UI4 nf;  // number of tasks finished
   C err; // if nonzero, error returned from f.  Because tasks run in parallel, multiple errors may be generated; we discard all but the first
  } internal;
 };
} JOB;

// we use the 6 LSBs of jobq->ht[0] as the lock, so that when we get the lock we also have the job pointer.  The job is always on a cacheline boundary
// We take JOBLOCK before taking the mutex, always.  By measurement (20220516 SkylakeX, 4 cores) the job lock keeps contention low until the tasks are < 400ns
// long, while using the mutex gives out at < 1000ns
_Static_assert(MAXTHREADSINPOOL<64,"JOBLOCK fails if > 63 threads in pool");
#define JOBLOCK(jobq) ({I z; if(unlikely(((z=__atomic_fetch_add((I*)&jobq->ht[0],1,__ATOMIC_ACQ_REL))&(ABDY-1))!=0))z=joblock(jobq); (JOB*)z; })
#define JOBUNLOCK(jobq,oldh) __atomic_store_n(&jobq->ht[0],oldh,__ATOMIC_RELEASE);
static NOINLINE I joblock(JOBQ *jobq){I z;
 // loop until we get the lock
 do{
  I nspins=10;  // good upper bound on the amount of time a lock could reasonably take, in poll delays.  It's the time of an uncontended lock of the mutex
  // we are delaying while a writer finishes.  Usually this will be fairly short, as controlled by nspins.  The danger is that the
  // writer will be preempted, leaving us in a tight spin.  If the spin counter goes to 0, we decide this must have happened, and we
  // do a low-power delay for a little while (method TBD)
  do{if(--nspins==0){nspins=50; YIELD} POLLDELAY}while((__atomic_load_n((I*)&jobq->ht[0],__ATOMIC_ACQUIRE)&(ABDY-1))!=0);  // loop till lock released
  // try to reacquire the lock, loop if can't
 }while(((z=__atomic_fetch_add((I*)&jobq->ht[0],1,__ATOMIC_ACQ_REL))&(ABDY-1))!=0);
 R z;
}
// It would be possible to save a little time in going from active to waiting in all threads, by having a 'conditional lock' that
// would not wait if the lock is held but empty.  Each thread would see the empty and process it simultaneously on the way to waiting.
// (futexval would have to be sampled before the read of the jobq)
// This would still require an RFO cycle to read the lock, and it would require that jobq->waiters and job->internal.nf be modified with an atomic instruction since it
// would not be fully under lock when incremented.  Because this would add an RFO for waiters to the wakeup sequence, where we worry about the thundering herd,
// we avoid it.  If waiters is eliminated we should revisit.

// Processing loop for thread.  Grab jobs from the global queue, and execute them
static void *jtthreadmain(void *arg){J jt=arg;I dummy;
 // One-time initialization
 A *old=jt->tnextpushp;  // we leave a clear stack when we go
 // get/set stack limits
 // not supported on Windows if(pthread_attr_getstackaddr(0,(void **)&jt->cstackinit)!=0)R 0;
 __atomic_store_n(&jt->cstackinit,(UI)&dummy,__ATOMIC_RELEASE);  // use a local as a surrogate for the stack pointer
 __atomic_store_n(&jt->cstackmin,jt->cstackinit-(CSTACKSIZE-CSTACKRESERVE),__ATOMIC_RELEASE);  // use a local as a surrogate for the stack pointer
 // Note: we use cstackmin as an indication that this thread is ready to use.
 JOBQ *jobq=&(*JT(jt,jobqueues))[jt->threadpoolno];   // The jobq block for the threadpool we are in - never changes

 // loop forever executing tasks.  First time through, the thread-creation code holds the job lock until the initialization finishes
nexttask: ; 
  I ndxinthreadpool=jt->ndxinthreadpool;   // we load this before we get the lock, 
  JOB *job=JOBLOCK(jobq);  // pointer to next job entry, simultaneously locking
  
nexttasklocked: ;  // come here if already holding the lock, and job is set.  ndxinthreadpool is also set
  UI4 futexval;
  if(unlikely(job==0)){  // not really unlikely, but if there's not one we can be as slow as we like
   // No job to run.  Wait for one.  While we're waiting, do a garbage-collection if one is needed.  It could be signaled by a different thread
   JOBUNLOCK(jobq,0);
   jtrepatrecv(jt); // Reclaim any of our own memory from others; unconditionally because there's nothing better to do
   if(jt->uflags.spfreeneeded&SPFREEGC)spfree();  // Collect garbage if there is enough to check
   job=JOBLOCK(jobq);
   if(likely(job==0)){
    // Still no job.  As far as tasks are concerned, we are now waiting.  But don't do an OS wait till we have lingered
    ++jobq->waiters;  // indicate we are waiting, while under lock.  Central needs to know there is an idle thread to prevent the master from taking a task.  We are in a non-OS wait state here
    UI4 warmendns=jobq->keepwarmns;   // user's keepwarm delay
    do{   // loop till we get something to process
     if(warmendns!=0){struct jtimespec endtime=jtmftil(warmendns);  // time when our keepwarm expires
      // the user wants us to linger before performing a wait.  We will spin here in the hope that a job arrives
      JOBUNLOCK(jobq,job);
      while(1){
       if(jtmfdif(endtime)<0)break;  // if time expired, exit loop.  Would prefer to deal with ns only rather than timespec, but these are the primitives we have
#define THREADSPERPAUSE 4  // We want to reduce the bus load when all threads are lingering.  With PAUSE at 140 cycles, one read per 35 cycles seems negligible
       I threadct=jobq->nthreads; do{_mm_pause();}while((threadct-=THREADSPERPAUSE)>0);  // pause long enough for every thread to poll without bus overload: pausetime * #threads / THREADSPERPAUSE
       if(__atomic_load_n((I*)&jobq->ht[0],__ATOMIC_ACQUIRE)!=0)break;  // if a job shows up, exit and try to acquire it
      }
      if((job=JOBLOCK(jobq))!=0)goto jobfound;   // reestablish lock, checking in case a job has arrived.  If it has, exit to process
     }
     // still have the lock, but there was nothing on the queue.  Wait.
     if(unlikely(jt->taskstate&TASKSTATETERMINATE)){--jobq->waiters; goto terminate;  // if central has requested this thread to terminate, do so when the queue goes empty.   This counts as work
      // no fallthrough, so we can put in a label for an immediate wait
waitforkick: ;  // come here with ndxinthreadpool set to wait for a new addition.  ndxinthreadpool must be set, and we must have the lock.
      ++jobq->waiters;  // this entry sleeps without linger, because we know the queue has something we can't process.  Mark us as waiting
     }
     futexval=__atomic_load_n(&jobq->futex,__ATOMIC_ACQUIRE);  // get current value to wait on, before we check for work.  It is updated under lock when a job is added or if threads are kicked with 15 T.
                              // we set the value before lingering so that if we are kicked while lingering the wait will fail and we will come back to linger again
     JOBUNLOCK(jobq,job);
     jfutex_wait(&jobq->futex,futexval);  // wait (unless a new job has been added).  When we come out, there may or may not be a task to process (usually there is)
     // NOTE: when we come out of the wait, waiters has not been decremented; thus it may show non0 when no one is waiting
     // we could check to see if there is a job before doing the RFO; that would reduce contention after a kick but it would increase delay
     // for real jobs.  Since the threads are idle anyway during a kick, we accept the contention
     job=JOBLOCK(jobq);  // take a conditional lock to reduce bus traffic when there is no work (as after a kick)
    }while(job==0); // wait till we get a job to run; exit holding the job lock
jobfound:;  // come here or fall through when we got a job while we were waiting.  We must remove waiting status
    --jobq->waiters;   // waiting no longer
   }
  }
  // We have the job lock, and a job to run, in (job).  It is possible that all the other threads - the thundering herd - woke up too.  We need to do our business with the job block and jobq ASAP.
  UI jobnsmask=job->ns_mask; JOB *jobnext=job->next; UI jobn=(UI)(I)job->n;   // fetch what we know we will need.  jobns-1 is the piece we are taking here
  unsigned char (*f)(J jt,void *ctx,UI4 i)=job->internal.f; void *ctx=job->internal.ctx; C err=job->internal.err;  // also fetch what an internal job will need
   // The compiler defers these reads but the read delay is so long that they will get executed early anyway - poor instruction model?
  // increment the # starts; if that equals or exceeds the # of tasks, dequeue the job.  But if user job with locales, add to the mask, not the job count
  UI locbit=SGNTO0(jobn)<<ndxinthreadpool;
  // if this thread has already processed this block, it means either (1) the thread was not enabled for the block (2) this thread finished the block before another thread started it.
  // In this case we don't want to spin on this job, so we sleep till the next one.  If there is another block queued after this one, we clear AR in this job to indicate a request
  // for a kick when this job is removed from the queue
  if(unlikely(locbit&jobnsmask)){if(unlikely(jobnext!=0))AR(UNvoidAV1(job))=0; goto waitforkick;}  // req kick (by writing 0) only if there is another job.  Start waiting
 locbit=(I)jobn<0?locbit:1; job->ns_mask=jobnsmask+=locbit;   // increment task counter for next owner
  JOB **writeptr=&jobq->ht[1]; writeptr=jobnext!=0?(JOB**)&jt->shapesink[0]:writeptr; writeptr=jobnsmask<jobn?(JOB**)&jt->shapesink[0]:writeptr; jobnext=jobnsmask<jobn?job:jobnext;  // calc head & tail ptrs
      // if there are more jobs (jobnext!=0) OR more tasks in the current job (jobns<jobn), divert write of tail; otherwise write the empty-queue value into tail.  If job finishing, set new headptr in jobnext
      // If this is a user job without locales, ns_mask is garbage but n=0, so jobns<jobn will never be true (because the vbls are unsigned).
  *writeptr=(JOB *)writeptr; JOBUNLOCK(jobq,jobnext);  // Do the writes.  tailptr write, if not diverted, sets tail->itself.  The write of the headptr releases the lock.
  // We have now dequeued the job if it has all started, extracted what an internal job needs to run, and released the lock.  Let the thundering herd come and fight over the job lock
  
  // lock released - now process the job
  if((I)jobn>0){
   // internal job.  We first have to handle the special case of jobns>n.  This indicates that the job has been entirely started (possibly not finished), but
   // we couldn't free the job block earlier because it might have been in the middle of the job list (in this case it would have been finished in the originating thread).  We can free it now, then look for the next job.
   // Note that if the job is not finished it will still be protected by the originator until all tasks have finished
   if((unlikely(jobnsmask>jobn))){fa(UNvoidAV1(job)); goto nexttask;}
   if(likely(!err)){   //  If an error has been signaled, skip over it and immediately mark it finished
    if(unlikely((err=f(jt,ctx,jobnsmask-1))!=0))__atomic_compare_exchange_n(&job->internal.err,&(C){0},err,0,__ATOMIC_ACQ_REL,__ATOMIC_RELAXED);  // keep the first error for use by later blocks
   }
   // This block is done.  Since we will need the lock when we go to look for work, we take it now.
   tpop(old);  // release any resources used by internal job
   ndxinthreadpool=jt->ndxinthreadpool; JOB *nextjob=JOBLOCK(jobq);  // pointer to next job entry, simultaneously locking
   __atomic_fetch_add(&job->internal.nf,1,__ATOMIC_ACQ_REL);    // account that this task has finished; must do atomic to ensure handshake with end-of-job code
   job=nextjob;  // set up for loop
   if(unlikely(jt->taskstate&TASKSTATETERMINATE))goto terminate;  // if central has requested this state to terminate, do so
   goto nexttasklocked; // loop for work, holding the lock
  }else{
   // user job, which may or may not specify locales.  If there are locales, each will revisit the job block & perhaps we should move the first cacheline to a quiet spot.  For the time
   // being we will copy out of the block quickly.  If no locales, there is no thundering herd, and we can read from the job block undisturbed (it has been dequeued)
   // set up jt state here only; for internal tasks, such setup is not needed
   ((I*)jt)[0]=job->user.inherited[0]; ((I*)jt)[1]=job->user.inherited[1]&(((I)1<<((offsetof(JTT,uflags.init0area)-SZI)*BB))-1);  // copy inherited part, zero the following
   A arg1=job->user.args[0],arg2=job->user.args[1],self=job->user.args[2];  // arg2=self if monad
   I initthread=job->initthread;  // extract thread# of thread that created the job
   jtsettaskrunning(jt);  // go to RUNNING state, perhaps after waiting for system lock to finish
  // extract the address of the pyx and globals area from the job.  For a job with locales we have to get the value from the vectors
   A pyx=UNvoidAV1(job)->mback.jobpyx; A startloc=UNvoidAV1(job)->kchain.global; // extract the pyx and globals pointer from the job
   if((I)jobn<0){  // user job with locales
    // this is a job with locales.  If we removed the job from the queue (mask=n), AND AR was 0 indicating that a thread is waiting for a kick, deliver the kick
   if(unlikely(((jobn^jobnsmask)|AR(UNvoidAV1(job)))==0)){JOB *job=JOBLOCK(jobq); ++jobq->futex; JOBUNLOCK(jobq,job); jfutex_wakea(&jobq->futex);}  // wakeall sequence: lock the pool; advance futex value; unlock; kick.  No new work added
    // pyx points to a vector of pyxes, startloc to a vector of locale#s.  Extract the pyx and global for this thread
    I vecx=__builtin_popcountll((UI)(AN(UNvoidAV1(job))<<(BW-jt->ndxinthreadpool)));  // AN is mask of threads.  Convert thread index to index into vectors, by counting the 1s after discarding higher bits in mask
    pyx=AAV1(pyx)[vecx]; startloc=jtfindnl(jt,IAV(startloc)[vecx]); ASSERTGOTO(startloc!=0,EVLOCALE,fail)  // get pyx (AAV1 ok) and locale#; convert locale# to globals address
   }
   ((PYXBLOK*)AAV0(pyx))->pyxorigthread=THREADID(jt);  // install the running thread# into the pyx
   jt->locsyms=(A)(*JT(jt,emptylocale))[THREADID(jt)]; SYMSETGLOBALS(jt->locsyms,startloc); RESETRANK; jt->currslistx=-1; jt->recurstate=RECSTATERUNNING;  // init what needs initing.  Notably clear the local symbols
  // run the task, putting the starting locale into execution by raising & lowering the locale execct.  Bivalent
   jt->uflags.bstkreqd=1; INCREXECCTIF(startloc);  // start new exec chain; raise execcount of current locale to protect it while running
  jt->parserstackframe.sf=self;  // each thread starts a new recursion point
   // Get the arg2/arg3 to use for u .  These will be the self of u, possibly repeated if there is no a
  A uself=FAV(self)->fgh[0], uarg2=arg2!=self?arg2:uself;  // get self, positioned after the last noun arg
   A *old=jt->tnextpushp;  // we leave a clear stack when we go
   // ***** this is where the user task is executed *******
   A z=(FAV(uself)->valencefns[arg2!=self])(jt,arg1,uarg2,uself);  // execute the u in u t. v
   // ***** return from user task and look for next one *****
   DECREXECCTIF(jt->global);  // remove exec-protection from finishing exec chain.  This may result in its deletion
   // put the result into the result block.  If there was an error, use the error code as the result, but make sure the errorcode is non0 so the pyx doesn't wait forever
   C errcode=0;
   if(unlikely(z==0)){fail: z=0; errcode=jt->jerr; errcode=(errcode==0)?EVSYSTEM:errcode;}else{realizeifvirtualERR(z,goto fail;);}  // realize virtual result before returning it
   jtsetpyxval(jt,pyx,z,errcode);  // report the value and wake up waiting tasks.  Cannot fail.  This protects the arguments in the pyx
   // If all threads have started a locales job, we could have freed the job block before the task call, thus saving a register.  But the values pointed to by the job mustn't
   // be freed until the last thread has finished.  That means we need something to count finishes, necessarily under lock.  The simplest solution is to use the usecount in the job
   // as the finish-counter.  We increment it for the number of threads to run and free, decrementing it after eack task finishes.  When the count goes to 0, we then free everything that was protected
   // We can't use the locales list or the pyxes for this counter, because they are in the wild as a user value.
   I taskended=0;  // 1 if this completion means the user task finished
   if(faprobe(UNvoidAV1(job))<2){  // if time to free...
    // we are the last (or only) thread using this job block.  Unprotect everything it protects, then free the job
   // remove the ra() for the args that was issued to protect the args over the lifetime of this thread.  If the fa() results in freeing a virtual block,
    // we must also fa the backer.  This is different from the case of virtual args to explicit defns: there we know that the virtual arg is on the stack in the caller,
    // and will be freed from the stack, and thus that there is no chance that a virtual will be freed.  Here the caller has continued, and there may be nothing but this
    // virtual to hold the backer.  So, unlike in all other fa()s, we fa the backer if the virtual is freed.
   fa(UNvoidAV1(job)->mback.jobpyx) fa(UNvoidAV1(job)->kchain.global) faafterrav(arg1); if(arg2!=self)faafterrav(arg2); fanamedacv(self);  // unprotect args only after result has been safely installed
    mf(UNvoidAV1(job));  // free the job, which is never a recursive block
    taskended=1;  // if we free, we finished the task
   }
   jtrepatsend(jt); // send our freed blocks back to where they were allocated.  That will include any args just freed
   __atomic_store_n(&JTFORTHREAD(jt,(US)initthread)->uflags.sprepatneeded,1,__ATOMIC_RELEASE);  // signal the originator to repat the freed blocks.  We force this now in case some were virtual and have large backers.  The repat may be delayed a while.
   jtclrtaskrunning(jt);  // clear RUNNING state, possibly after finishing system locks (which is why we wait till the value has been signaled)
   tpop(old); // clear anything left on the stack after execution, including z
   RESETERR  // we had to keep the error till now; remove it for next task
   ndxinthreadpool=jt->ndxinthreadpool; job=JOBLOCK(jobq);  // pointer to next job entry, simultaneously locking
   jobq->nuunfin-=taskended; // mark in the jobq that we have finished the job we were working on, if we are the last thread to work on it
   if(unlikely(jt->taskstate&TASKSTATETERMINATE))goto terminate;  // if central has requested this state to terminate, do so
   goto nexttasklocked;  // loop for next task
  }
 // end of loop forever
terminate:   // termination request.  We hold the job lock, and 'job' has the value to write back to it.  We are not marked as waiting
 __atomic_fetch_and(&jt->taskstate,~(TASKSTATEACTIVE|TASKSTATETERMINATE),__ATOMIC_ACQ_REL);  // go inactive, and ack the terminate request
 JOBUNLOCK(jobq,job); 
 jtrepatsend(jt); // release any memory belonging to other threads
 R 0;  // return to OS, closing the thread
}

// Create worker thread n, and call its threadmain to start it in wait state.  *threadattrs has initial attributes, which we modify
static I jtthreadcreate(J jt,I n,pthread_attr_t *threadattrs){
 // create thread
 ASSERT(pthread_attr_setdetachstate(threadattrs,PTHREAD_CREATE_DETACHED)==0,EVFACE);
 size_t stksiz=CSTACKSIZE;
#if defined(__APPLE__)
 stksiz=pthread_get_stacksize_np(pthread_self());
#elif defined(__linux__) && defined(_GNU_SOURCE)
 pthread_attr_t tattr;  // attributes for the current task
 if(pthread_getattr_np(pthread_self(),&tattr)==0)if(pthread_attr_getstacksize(&tattr,&stksiz)!=0)stksiz=CSTACKSIZE;
#endif
 ASSERT(pthread_attr_setstacksize(threadattrs,stksiz)==0,EVFACE)    // request sufficient stack size
 JTFORTHREAD(jt,n)->cstackmin=0;  // clear any old stackarea; we wait for thread to fill in the stack
 ASSERT(pthread_create(&(pthread_t){0},threadattrs,jtthreadmain,JTFORTHREAD(jt,n))==0,EVFACE)  // create the thread, save its threadid (by passing its jt into jtthreadmain)
pid_t p;
 // since the user may try to use the thread right away, delay until it is available for use.  We use cstackmin as a 99.999% proxy for 'ready'
 while(__atomic_load_n(&JTFORTHREAD(jt,n)->cstackmin,__ATOMIC_ACQUIRE)==0){delay(10000); YIELD}  // task startup takes a while
 R 1;
}

// execute the user's task.  Result is an ordinary noun or a pyx.  Bivalent (a,w,self) or (w,self,self) called from unquote or parse
static A jttaskrun(J jtfg,A arg1, A arg2, A self){F12JT;
 ARGCHK2(arg1,arg2);  // the verb is not the issue.
 A jobA;GAT0(jobA,INT,(sizeof(JOB)+SZI-1)>>LGSZI,1); ACINITUNPUSH(jobA);  // protect the job till it is finished
 JOB *job=(JOB*)AAV1(jobA);  // The job starts on the second cacheline of the A block.  When we free the job we will have to back up to the A block
I taskflags=FAV(self)->localuse.lu1.forcetask, localesx=FAV(self)->localuse.lu1.forcetask>>16;  // flags: 0-7=pool, 8=workeronly, 9=mask0 set 10=locales given  16+=localesx (-1 if no locales)
 JOBQ *jobq=&(*JT(jt,jobqueues))[FAV(self)->localuse.lu1.forcetask&0xff];  // bits 0-7 = threadpool number to use, point to jobq info for selected thread
 // create the pyx/global info: if no locales, one pyx and the current jt->global; if locales, a list of pyxes and the user's list of locales.  We store these in the job.  They are what we ra and fa
 // Also fill in n, ns_mask
 if(localesx<0){   // locales given?
  // no locales
  RZ(jobA->mback.jobpyx=jtcreatepyx(jt,-2,inf)) jobA->kchain.global=jt->global; job->n=0;  // pyx is secreted in header  threadid is not known yet
 }else{A p;
  // there are locales.
  A masklocs=AAV(AAV(FAV(self)->fgh[1])[localesx])[1],maska=AAV(masklocs)[0],locsa=AAV(masklocs)[1];  // point to mask/loc parameters
  ASSERTGOTO(!ACISPERM(AC(locsa)),EVRO,errfajobA)   // surely the locales block is not permanent, but check to be safe
  UI mask=BIV0(maska); I nlocs=AN(locsa);  // extract mask value, number of locales
  ASSERTGOTO((mask>>jobq->nthreads)<=1,EVDEADLOCK,errfajobA)  // exit if the mask calls for threads that are not active.  Mask bit 0 is for current thread.  If user deletes a thread while this job is running, we may deadlock
  taskflags|=(2+(mask&1))<<9;  // set flags indicating whether mask bit0 is set (=this enqueueing thread must run a locale), and whether locales given
  jobA->kchain.global=locsa; AN(jobA)=mask; job->n=-1; job->ns_mask=1|~mask; AC(jobA)=nlocs-(mask&1);  // fill in job info: locale list, threadmask, locales type, mask of threads NOT to use.  Usecount of job is #worker threads, to count down to find completion of last thread
  GATV0E(jobA->mback.jobpyx,BOX,nlocs,1,goto errfajobA;) AFLAGINIT(jobA->mback.jobpyx,BOX)  // allocate recursive list of pyxes which will be our result
  A *psv=jt->tnextpushp; jt->tnextpushp=AAV1(jobA->mback.jobpyx); DO(nlocs, if((p=jtcreatepyx(jt,-2,inf))==0)break;) jt->tnextpushp=psv;   // hijack tstack (see vo.c for discussion), allocate pyxes.  They have usecount 1, protected by the enclosing box.
  RZGOTO(p,errfajobA)   // exit if failure allocating a pyx
 }
// extract parms given to t.: threadpool number, worker-only flag
UI forcetask=REPSGN((taskflags&0x500)-1);  // 0 if the user wants to force this job to queue (worker or locales specified), ~0 otherwise
 if((((I)(forcetask&lda(&jobq->nuunfin))-jobq->nthreads)&(lda(&JT(jt,systemlock))-3))<0){  // more workers than unfinished jobs (ignoring # unfinished if forcetask was requested) - fast look
    // in suspension (systemlock state>2) we do not start any task anywhere
  // We will probably start the job, failing to do so only if another thread beats us to it.  Do anything costly before taking the lock.  And, once we start the job the JOB block is unreliable, so take out anything needed from there
  // we would like to avoid realizing virtual arguments, so that the copy will be done into the core that needs the data.  However, if we leave the block as virtual,
  // we will fa() it at end-of-job and that will fail if it is recursive.  We meanly want to avoid checking VIRTUAL on every fa.  So, we realize a virtual only if it is recursive,
  // or if it is UNINCORPABLE (in which case we only need to clone the nonrecursive block).  After that, ra() the arguments to protect them until the task completes.
  // It would be nice to be able to free the virtual before the task completes, but we don't have a way to (we could realize/fa in the worker, but why?).  The virtual backer will be tied up during the task, but we
  // won't have to copy the data here and then transfer it in the task
  ASSERTGOTO(ISDENSE(AT(arg1)),EVNONCE,errfajobA) // Don't allow sparse args since we can't box them; copy/realize as needed
  if(unlikely(AFLAG(arg1)&AFANCHORED)){RZGOTO(arg1=ca(arg1),errfajobA) ACINITZAP(arg1);}else{if(AFLAG(arg1)&AFVIRTUAL){if(AT(arg1)&TRAVERSIBLE)RZGOTO(arg1=realize(arg1),errfajobA) else if(AFLAG(arg1)&AFUNINCORPABLE)RZGOTO(arg1=clonevirtual(arg1),errfajobA)} ra(arg1);}
  if(arg2!=self){ASSERTGOTO(ISDENSE(AT(arg2)),EVNONCE,errfaarg1)  // repeat for dyad
   if(unlikely(AFLAG(arg2)&AFANCHORED)){RZGOTO(arg2=ca(arg2),errfaarg1) ACINITZAP(arg2);}else{if(AFLAG(arg2)&AFVIRTUAL){if(AT(arg2)&TRAVERSIBLE)RZGOTO(arg2=realize(arg2),errfaarg1) else if(AFLAG(arg2)&AFUNINCORPABLE)RZGOTO(arg2=clonevirtual(arg2),errfaarg1)} ra(arg2);}
  }
  ra(self);  // self can't be virtual - simply raise it
  job->initthread=THREADID(jt);  // indicate this is a user job.  ns is immaterial since it will always trigger a deq.  Install initing thread# for repatriation
  job->user.args[0]=arg1;job->user.args[1]=arg2;job->user.args[2]=self;memcpy(job->user.inherited,jt,sizeof(job->user.inherited));  // Move in all parms for starting the job.  A little overcopy OK on inherited.  arg2 has not changed for monads, =self
  // protect anything that we have to ensure stays valid for the verb.  This is the pyx or list thereof, and the locales or list thereof.  Also extract them from the evanescent job block
  A ppyx=jobA->mback.jobpyx, ploc=jobA->kchain.global;  // values that need protecting/extracting
  raincr(ploc);   // protect globals: known nonrecursive, and were allocated here or came from boxes or jt->global, thus positive usecount; and not ACPERMANENT
  JOB *oldjob=JOBLOCK(jobq);  // pointer to next job entry, simultaneously locking
  if(likely((UI)jobq->nthreads>(forcetask&jobq->nuunfin))){  // recheck after lock
   // We know there is a thread that can take the user task (possibly after finishing internal tasks), or the user insists on queueing.  Queue the task
   ACINIT(ppyx,ACUC2)  // we have committed to the job; protect the pyx/pyxlist then
   ++jobq->nuunfin;  // add the new user job to the unfinished count
   _Static_assert(offsetof(JOB,next)==offsetof(JOBQ,ht[0]),"JOB and JOBQ need identical prefixes");  // we pun &JOBQ->ht[1] as a JOB, when the q is empty
   job->next=0; jobq->ht[1]->next=job; jobq->ht[1]=job;  // clear chain in job; point the last job to it, and the tail ptr.  If queue is empty these both store to tailptr
   oldjob=(oldjob==0)?job:oldjob;   //  Keep old head unless it was empty
   ++jobq->futex;  // while under lock, advance futex value to indicate that we have added a job
   JOBUNLOCK(jobq,oldjob);  // set head pointer, which unlocks.
   (taskflags&0x400?jfutex_wakea:jfutex_wake1)(&jobq->futex);  // wake 1 waiting thread, if there is one; for locales type, wake them all
   if(taskflags&0x200){
    // bit 0 of the mask was set, indicating that this thread should pitch in with locale 0.  Run as if in a thread, and report any error in the pyx.  This is copied from threadmain
    // We start in the console locale for this thread, with globals as called for by the task.  When we finish we must restore local & both global pointers to the current
    // state of this thread
    A pyx=AAV1(ppyx)[0], startloc=jtfindnl(jt,IAV(ploc)[0]); ASSERTGOTO(startloc!=0,EVLOCALE,fail)  // get pyx (AAV1 ok) and locale#; convert locale# to globals address
    ((PYXBLOK*)AAV0(pyx))->pyxorigthread=THREADID(jt);  // install the running thread# into the pyx
    A savlocsyms=jt->locsyms,savglobal=jt->global;jt->locsyms=(A)(*JT(jt,emptylocale))[THREADID(jt)]; SYMSETGLOBALS(jt->locsyms,startloc); RESETRANK;  // init what needs initing.  Notably clear the local symbols
   INCREXECCTIF(startloc);  // start new exec chain; raise execcount of current locale to protect it while running
    jt->parserstackframe.sf=self;  // each thread starts a new recursion point
    A uself=FAV(self)->fgh[0], uarg2=arg2!=self?arg2:uself;  // get self, positioned after the last noun arg
   A z=(FAV(uself)->valencefns[arg2!=self])(jt,arg1,uarg2,uself);  // execute the u in u t. v
    DECREXECCTIF(jt->global); SYMRESTORELOCALGLOBALS(savlocsyms,savglobal);  // remove exec-protection from finishing exec chain.  This may result in its deletion.  Reset symbol tables too (note there may be no locals & we have changed only globals)
    C errcode=0;
    if(unlikely(z==0)){fail: z=0; errcode=jt->jerr; errcode=(errcode==0)?EVSYSTEM:errcode;}else{realizeifvirtualERR(z,goto fail;);}  // realize virtual result before returning it
    jtsetpyxval(jt,pyx,z,errcode);  // install completion info into pyx 0
   }
   R ppyx;

 // No thread for the job.  Run it here
  }else{JOBUNLOCK(jobq,oldjob); fa(ploc)}  // return lock if there is no task to take the job, and undo the increment of the locale.  ppyx was not raised
 fa(arg1); if(arg2!=self)fa(arg2); fanamedacv(self); // free these now to match ra before test.  We don't need faafterrav because any virtual cannot have usecount=1 since we ra()'d it
 }
 mf(jobA); //  mf to free jobA, whose usecount we might have raised.  pyx and locs if any are on tstack
 ASSERT(!(taskflags&0x400),EVUNTIMELY)   // if we wanted a locales job but didn't get one, we must have been in system lock (i. e. debug suspension).  Fail then
A uself=FAV(self)->fgh[0], uarg2=arg2!=self?arg2:uself;  // args to u, with monad flagging
 // u always starts a recursion point, whether in a new task or not
 A s=jt->parserstackframe.sf; jt->parserstackframe.sf=self; A z=(FAV(uself)->valencefns[arg2!=self])(jt,arg1,uarg2,uself); jt->parserstackframe.sf=s;
 R z;
 // error exits, to free things we raised
errfaarg1: fa(arg1);
errfajobA: mf(jobA);
 R 0;
}


//todo: don't wake everybody up if the job only has fewer tasks than there are threads. futex_wake can do it
// execute an internal job made up of n tasks (but if suspension is active, force n=1).  f is the function to run, end is the function to call at end, ctx is parms to pass to each task
// poolno is the threadpool to use.  Tasks are run on this thread and the worker threads
// Result is 0 for OK, else jerr.h error code
C jtjobrun(J jt,unsigned char(*f)(J,void*,UI4),void *ctx,UI4 n,I poolno){JOBQ *jobq=&(*JT(jt,jobqueues))[poolno];
 A jobA;GAT0(jobA,INT,(sizeof(JOB)+SZI-1)>>LGSZI,1); ACINITUNPUSH(jobA);  // we could allocate this (aligned) on the stack, since we wait here for all tasks to finish.  Must never really free!
 JOB *job=(JOB*)AAV1(jobA); job->n=n; job->ns_mask=1;  job->initthread=THREADID(jt); job->internal.f=f; job->internal.ctx=ctx; job->internal.nf=0; job->internal.err=0;  // by hand: allocation is short.  ns=1 because we take the first task in this thread
 I lastqueuedtask=-1;  // if nonneg, the task# of the last task (i. e. n-1).  If this task is taken here we have to leave it in the queue
 if(likely(((lda(&JT(jt,systemlock))-3)&-(I)jobq->nthreads&(1-(I)n))<0)){  // we will take the first task; wake threads only if there are other blocks, and worker threads
      // we don't start tasks during suspension (lock state>2), because if the user changed the debug thread to a running task's there would be chaos
  JOB *oldjob=JOBLOCK(jobq);  // lock jobq before mutex
  _Static_assert(offsetof(JOB,next)==offsetof(JOBQ,ht[0]),"JOB and JOBQ need identical prefixes");  // we pun the JOBQ as a JOB, when the q is empty
  // When we finish all tasks, we will have a problem.  The job block is on the jobq somewhere, but not necessarily at the top.
  // If not at the top, we can't dequeue the job or free it.  What we do is leave it on the jobq, with ns=n.  When the job is next taken for a task
  // (possibly immediately), that condition will cause the job to be dequeued, and then (as a special case) fa()d.  Since we might still
  // be processing the job, we ra the job now to protect it.  It will be freed at the later of (coming to the top of the job list) and
  // (all tasks finished and waited for here).  We fa explicitly rather than calling tpop
  // NOTE that this is problematic during debug suspension, which is a systemlock.  During the systemlock no thread will start a new task, which means that
  // all jobs will be processed single-threaded even though they are on the queue: everything will be run here by the originator.
// we had a crash here: while waiting for it to return, isolate the assignments
  job->next=0;
 jobq->ht[1]->next=job;
 jobq->ht[1]=job;  // clear chain in job; point the last job to it, and the tail ptr.  If queue is empty these both store to tailptr
  oldjob=(oldjob==0)?job:oldjob;   //  Keep old head unless it was empty
  ++jobq->futex;  // while under lock, advance futex value to indicate that we have added a job
  I nwaiters=jobq->waiters;  // see if there are waiting threads
  JOBUNLOCK(jobq,oldjob);  // set head pointer, which unlocks.
  if(nwaiters!=0)jfutex_wakea(&jobq->futex);  // if there are waiting threads, wake them up.  We test in case there are no worker threads.  Not really necessary to read under lock, but it silences asan
  lastqueuedtask=n-1;  // if we take this task here, it is special
   // todo scaf: would be nice to wake only as many as we have work for
 }
 // We have started all the threads, but we pitch in and and process tasks ourselves, starting with task 0
 // In our job setup we have accounted for the fact that we are taking the first task, so that we need nothing more from the job block to start running the first task
 A *old=jt->tnextpushp;  // we leave a clear stack when we go
 UI4 i=0; C err=0;  // the number of the block we are working on, and the current error status
 while(1){  // at top of loop mutex is free, i is the task# to take, err is error status so far
  // run the user's function on one thread.  If there are errors, we skip after the first.  Return from thread is 0 if OK, else error code
  if(!err){   //  If an error has been signaled, skip over it and immediately mark it finished
   if(unlikely((err=f(jt,ctx,i))!=0))__atomic_compare_exchange_n(&job->internal.err,&(C){0},err,0,__ATOMIC_ACQ_REL,__ATOMIC_RELAXED);  // keep the first error for use by later blocks
  }
  tpop(old);  // free anything allocated within the task
  JOB *oldjob=JOBLOCK(jobq);  // pointer to next job entry, simultaneously locking
  ++job->internal.nf;  // we have finished a block - account for it
  i=job->ns_mask; err=job->internal.err;  // account for the work unit we are taking, fetch current composite error status
  // whether we started threads or not, there is work to do.  We will pitch in and work, but only on our job
  if(unlikely(i==lastqueuedtask)){
   // We are taking the last task.
   if(job==oldjob&&job->next==0){
    // special case where the job we are working on is the only job in the queue.  This is the usual case, but its importance is in debug suspension, where as noted
    // above all jobs are queued but run only in this thread.  We mustn't wait for the other threads to see the job because they aren't looking during the
    // suspension.  If we leave the job it will stay on the queue till the end of suspension.  So, we dequeue it.
    jobq->ht[1]=(JOB *)&jobq->ht[1];   // the queue is going empty.  In that condition tail points to itself
    oldjob=0;  // when we release the lock, this will make the queue empty
   }else ACINIT(jobA,ACUC2) // if we are starting the last task when there are threads, the threads will not free the block until it gets to the top with job->ns_mask==n.  ra() to account for that.  We ra() cheaply since AC is still in init state
  }
  job->ns_mask=i+(i<n);  // we're taking the block if it's not past the end - account for it
  JOBUNLOCK(jobq,oldjob)
  if(i>=n)break;  //  if all tasks have already started, stop looking for one.  Leave i==n so that a thread will fa()
 }
 // There are no more tasks to start.  Wait for all to finish.
 // The threads and us acquire job->internal.nf to ensure all writes have been seen.  For this reason the call and the threads do not need atomic ops when accessing the ctx block
 while(__atomic_load_n(&job->internal.nf,__ATOMIC_ACQUIRE)<n){_mm_pause(); YIELD}  // scaf  should we have a mutex & wait for a wakeup from the finisher?
 C r=__atomic_load_n(&job->internal.err,__ATOMIC_ACQUIRE); fa(jobA); R r;  // extract return code from the job, then free the job and return the error code
 // job may still be in the job list - if so it will be fa()d when it reaches the top
}

// 13!:_7 run a null job with tasks.  w is #spins per task, # tasks
static C nulljohnson(J jt,void *ctx,UI4 i){R johnson(*(I*)ctx);}  // delay a bit
F1(jtnulljob){F12IP;
  ASSERT(AR(w)==1,EVRANK); ASSERT(AN(w)==2,EVLENGTH); if(!(AT(w)&INT))RZ(w=cvt(INT,w));
  I nspins=IAV(w)[0], ntasks=IAV(w)[1];  // extract parms
  I ctx=nspins;
  jtjobrun(jt,&nulljohnson,&ctx,ntasks,0);  // always use threadpool 0
  R mtm;
}

#else
static A jttaskrun(J jt,A arg1, A arg2, A arg3){A pyx;
 ARGCHK2(arg1,arg2);  // the verb is not the issue
 I dyad=!(AT(arg2)&VERB); A self=dyad?arg3:arg2;  // the call is either noun self x or noun noun self.  See which set dyad flag and select self.
 A uarg3=FAV(self)->fgh[0], uarg2=dyad?arg2:uarg3;   // get self, positioned after the last noun arg
 // u always starts a recursion point, whether in a new task or not
 A s=jt->parserstackframe.sf; jt->parserstackframe.sf=self; pyx=(FAV(FAV(self)->fgh[0])->valencefns[dyad])(jt,arg1,uarg2,uarg3); jt->parserstackframe.sf=s;
 R pyx;
}
static I jtthreadcreate(J jt,I n,void *threadattrs){ASSERT(0,EVFACE)}
C jtjobrun(J jt,unsigned char(*f)(J,void*,UI4),void *ctx,UI4 n,I poolno){
 DO(n,C c=f(jt,ctx,i);if(c)R c;);
 R 0;}

#endif

// u t. n - start a task.  We just create a verb to handle the arguments, performing <@u
// n is [importantoptions, all numeric or boxed numeric] [; k [;v] ]...
// importantoptions are pool#
// k is 'worker', value=0/1(default)
//      'locales', value=mask;locale#s  no default
F2(jttdot){F12IP;
 ASSERTVN(a,w);
 ASSERT(AR(w)<=1,EVRANK) // arg must be atom or list
 I nolocal=-1;  // establish unset values for options
 I localex=-1;  // this will hold the index of the box containing the locales keyword
 I poolno=0;  // default to using threadpool 0
 A afixed=0;  // the fixed-format args if any
 // parse the options
 // Go through each box, analyzing.  If we hit leading fixed-format options, remember where and skip for later
 DO(AN(w),
  A akw; A aval;  // A block for keyword, A block for value
  if(AT(w)&BOX){
   A boxl1=C(AAV(w)[i]);  // contents of top-level box to examine
   if(AN(boxl1)==0){ASSERT(i==0,EVDOMAIN) afixed=boxl1; continue;}   // empty box, construe as fixed parms
   if(AT(boxl1)&NUMERIC){ASSERT(i==0,EVDOMAIN) afixed=boxl1; continue;}   // numeric contents, construe as fixed parms
   if(AT(boxl1)&BOX){   // boxed contents, must be key [; value]
    A boxl2=C(AAV(boxl1)[0]);   // w is (<((<boxl2), ...))
    if(AN(boxl2)==0){ASSERT(i==0,EVDOMAIN) afixed=boxl2; continue;}  // w is < ''
    if(AT(boxl2)&NUMERIC){ASSERT(i==0,EVDOMAIN) afixed=boxl2; continue;}   // w is < poolno
    akw=boxl2;   // the keyword
    if(!(AT(akw)&LIT))RZ(akw=cvt(LIT,akw))  // keyword must be literal type
    ASSERT(AR(boxl1)<2,EVRANK) ASSERT(AN(boxl1)<=2,EVLENGTH) aval=AN(boxl1)==1?0:C(AAV(boxl1)[1]);  // arg has only 0-1 value; get value if any
   }else{
    akw=boxl1;  // the keyword
    if(!(AT(akw)&LIT))RZ(akw=cvt(LIT,akw))  // keyword must be literal type
    aval=0;
   }
  }else if(AT(w)&NUMERIC){afixed=w; break;  // numeric arg must be fixed-format
  }else{akw=w; if(!(AT(akw)&LIT))RZ(akw=cvt(LIT,akw)) aval=0;  // string arg is a valueless keyword
  }
  // we have the keyword/value; examine them one by one
  if(strncmp(CAV(akw),"worker",AN(akw))==0){
   ASSERT(nolocal<0,EVDOMAIN)  // can't set same parm twice
   aval=aval==0?num(1):aval;  // if value omitted, assume 1
   RE(nolocal=b0(aval))   // extract binary value
  }else if(strncmp(CAV(akw),"locales",AN(akw))==0){
   ASSERT(localex<0,EVDOMAIN)  // can't set same parm twice
   ASSERT(aval!=0,EVDOMAIN)  // value must be given
   ASSERT(AT(aval)&BOX,EVDOMAIN) ASSERT(AR(aval)==1,EVRANK) ASSERT(AN(aval)==2,EVLENGTH) // value must be 2-box list
   A boxx=C(AAV(aval)[0]); ASSERT(AT(boxx)&INT+B01,EVDOMAIN) ASSERT(AR(boxx)==0,EVRANK) I mask=BIV0(boxx);  // mask must be bool/int
   boxx=C(AAV(aval)[1]); ASSERT(AT(boxx)&INT,EVDOMAIN) ASSERT(AR(boxx)<=1,EVRANK) ASSERT(AN(boxx)==__builtin_popcountll(mask),EVDEADLOCK)  // must have 1 locale per 1-bit in mask
   I *bv=IAV(boxx); DO(AN(boxx), ASSERT(bv[i]>=0,EVLOCALE))  // verify locale #s nonnegative
   ASSERT((mask&~1)!=0,EVDEADLOCK)  // must have at least 1 locale outside of the master
   localex=i;  // remember the index we found the keyword at
  }else{ASSERT(0,EVDOMAIN)}   // error if keyword is unknown
  if(!(AT(w)&BOX))break;  // unboxed must be a single value
 )
 // if there is a fixed-format area, analyze it
 if(afixed){
  ASSERT(AR(afixed)<2&&AN(afixed)<=1,EVDOMAIN)  // fixed area is [threadpoolno]
  if(likely(AN(afixed)>=1)){
   RZ(afixed=vi(afixed)) poolno=IAV(afixed)[0]; ASSERT(BETWEENO(poolno,0,MAXTHREADPOOLS),EVLIMIT)  // verify value in bounds
  }
 }
 // set defaults for omitted parms
 nolocal=nolocal<0?0:nolocal;  // nolocal defaults to 0 (no mask given)
 // parms read, install them into the block for t. verb
 A z; RZ(z=fdef(0,CTDOT,VERB,jttaskrun,jttaskrun,a,w,0,FAV(a)->flag&VNONAME+VNOSELF,RMAX,RMAX,RMAX))  // NOSELF because t. does $: stacking
 FAV(z)->localuse.lu1.forcetask=poolno+(nolocal<<8)+(localex<<16);  // save the t. options for execution.  Bits 0-7=poolno, 8=worker only, 16+=box# in w of the locales keyword (-1 if none) 
 if(localex<0)z=atco(ds(CBOX),z);  // non-locales call returns a single pyx (to match local execution) and it must be boxed.  use <@: to get BOXATOP flags.  locales call must always run in threads
 RETF(z)  
}

// credentials.  These are installed into AM of a synco to indicate the type of a synco
#define CREDMUTEX (I)0x582a9524c923485f
#define CREDAMV (I)0x86a7210d76e0295f

// x T. y - various thread and task operations
F2(jttcapdot2){F12IP;A z;
 ARGCHK2(a,w)
 I m; RE(m=i0(a))   // get the x argument, which must be an atom
 switch(m){
 case 4: { // rattle the boxes of y and return status of each
  ASSERT((SGNIF(AT(w),BOXX)|(AN(w)-1))<0,EVDOMAIN)   // must be boxed or empty
  I zr=AR(w); GATV(z,FL,AN(w),AR(w),AS(w)) D *zv=DAVn(zr,z); A *wv=AAV(w); // allocate result, zv->result area, wv->input boxes
  DONOUNROLL(AN(w), if(unlikely(!(AT(wv[i])&PYX)))zv[i]=-1001;  // not pyx: _1001
                    else if(((PYXBLOK*)AAV0(wv[i]))->pyxorigthread>=0)zv[i]=((PYXBLOK*)AAV0(wv[i]))->pyxorigthread;  // running pyx: the running thread
                    else if(((PYXBLOK*)AAV0(wv[i]))->pyxorigthread==-2)zv[i]=inf; // not yet started; thread not yet known: _
                    else if(((PYXBLOK*)AAV0(wv[i]))->errcode>0)zv[i]=-((PYXBLOK*)AAV0(wv[i]))->errcode;  // finished with error: -error code
                    else zv[i]=-1000;  // finished with no error: _1000
  )
  break;}
 case 5: { // create a user pyx.  y is the timeout in seconds
#if PYXES
  ASSERT(AN(w)==1,EVLENGTH) w=ccvt(FL,w,0); D atimeout=*DAV(w); // get the timeout value
  if(atimeout==0.)atimeout=inf; ASSERT(atimeout==inf||atimeout<=9e9,EVLIMIT); // 9e9 is approx 63 bits of ns.  This leaves ~300y; should be ok
 RZ(z=jtcreatepyx(jt,THREADID(jt),atimeout));  // create the recursive pyx, owned by this thread
  AC(z)=ACUC2; R box(z); // ra the pyx to protect it until it is filled in, return the boxed pyx
#else
ASSERT(0,EVNONCE)
#endif
  break;}
 case 6: { // set value of pyx.  y is pyx;value
#if PYXES
  ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==2,EVLENGTH)  // must be pyx and value
  A pyx=AAV(w)[0], val=C(AAV(w)[1]);  // get the components to store
  ASSERT((AT(pyx)&BOX+PYX)==BOX+PYX,EVDOMAIN)
  ASSERT(jtsetpyxval(jt,pyx,val,0)!=0,EVRO) fa(pyx)  // install value.  Will fail if previously set.  The pyx is protected until it gets a value; then it returns to ownership of its caller
  z=mtm;  // good quiet value
#else
ASSERT(0,EVNONCE)
#endif
  break;}
 case 7: { // signal error in pyx
#if PYXES
  // set value of pyx.  y is pyx;value
  ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==2,EVLENGTH)  // must be pyx and value
  A pyx=AAV(w)[0], val=C(AAV(w)[1]);  // get the components to store
  ASSERT((AT(pyx)&BOX+PYX)==BOX+PYX,EVDOMAIN) I err=i0(val); ASSERT(BETWEENC(err,1,255),EVDOMAIN)  // get the error number
  ASSERT(jtsetpyxval(jt,pyx,0,err)!=0,EVRO) fa(pyx)  // install error value.  Will fail if previously set.  The pyx is protected until it gets a value; then it returns to ownership of its caller
  z=mtm;  // good quiet value
#else
ASSERT(0,EVNONCE)
#endif
  break;}
 case 2:  { // threadpool info: (count of idle threads),(count of unfinished user tasks),(#threads in pool)
#if PYXES
  I poolno=0;  // default to threadpool 0
  if(AN(w)){   // arg is [threadpool #]
   ASSERT(AR(w)<=1,EVRANK) ASSERT(AN(w)<=1,EVLENGTH)  // must be singleton
   RZ(w=vi(w)) poolno=IAV(w)[0]; ASSERT(BETWEENO(poolno,0,MAXTHREADPOOLS),EVLIMIT)  // extract threadpool# and audit it
  }
  JOBQ *jobq=&(*JT(jt,jobqueues))[poolno];
  GAT0(z,INT,3,1)  // allocate result
  JOB *oldjob=JOBLOCK(jobq);  // lock the jobq to present a consistent picture
  IAV1(z)[0]=jobq->waiters, IAV1(z)[1]=jobq->nuunfin, IAV1(z)[2]=jobq->nthreads;  // don't allocate under lock
  JOBUNLOCK(jobq,oldjob)
#else
ASSERT(0,EVNONCE)
#endif
  break;}
 case 22:  { // get/set coremask for executing thread
#if PYXES && SUPPORT_AFFINITY && !defined(__FreeBSD__)
  A athread, acoremask=0;
  if(AT(w)&BOX){  // not naked thread#
   ASSERT(AR(w)<2,EVRANK) ASSERT(BETWEENC(AN(w),1,2),EVLENGTH)  // must be singleton or 2-box list
   athread=C(AAV(w)[0]); if(AN(w)==2)acoremask=C(AAV(w)[1]);   // get thread#, coremask args
  }else athread=w;  // if bare thread#, take it
  if(!supportaffinity)ASSERT(0,EVNONCE)
  I threadno; RE(threadno=i0(athread)) ASSERT(threadno==0,EVNONCE)  // get thread#, which must be 0
  if(acoremask){ASSERT(AR(acoremask)<=1,EVRANK) ASSERT(AN(acoremask)!=0,EVLENGTH) if(unlikely(AT(acoremask)&FL+B01))acoremask=cvt(INT,acoremask); ASSERT(AT(acoremask)&INT+LIT,EVDOMAIN)} // verify coremask valid if given
  pthread_attr_t tattr; cpu_set_t cpuset; size_t cpusetsize=sizeof(cpu_set_t); // attributes for the current task
#if defined(ANDROID)
  ASSERT(sched_getaffinity(pthread_gettid_np(pthread_self()), cpusetsize,&cpuset)==0,EVFACE)  // fetch current affinity for return
#elif defined(__APPLE__)
  ASSERT(pthread_getaffinity_np(pthread_self(),cpusetsize,&cpuset)==0,EVFACE)  // fetch current affinity for return
#elif defined(__linux__) && defined(_GNU_SOURCE)
  ASSERT(pthread_getattr_np(pthread_self(),&tattr)==0,EVFACE) ASSERT(pthread_attr_getaffinity_np(&tattr,cpusetsize,&cpuset)==0,EVFACE)  // fetch current affinity for return
#else
  ASSERT(pthread_getaffinity_np(pthread_self(),cpusetsize,&cpuset)==0,EVFACE)  // fetch current affinity for return
#endif
#if defined(ANDROID)
  if(acoremask)ASSERT(sched_getaffinity(pthread_gettid_np(pthread_self()),AN(acoremask)<<bplg(AT(acoremask)),(cpu_set_t*)IAV(acoremask))==0,EVFACE)  // set new coremask if requested
#else
  if(acoremask)ASSERT(pthread_setaffinity_np(pthread_self(),AN(acoremask)<<bplg(AT(acoremask)),(cpu_set_t*)IAV(acoremask))==0,EVFACE)  // set new coremask if requested
#endif
  z=vec(INT,sizeof(cpu_set_t)>>LGSZI,(void *)&cpuset);  // convert old value to A return result
#else
  ASSERT(0,EVNONCE)
#endif
  break;}
 case 8:  { // system info: (number of cores),(max number of threads including master)
  ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==0,EVLENGTH)  // only '' is allowed as an argument for now
  z=v2(numberOfCores,MAXTHREADS);
  break;}
 case 14:  { // threadpool keepwarm (in sec): set to y, return previous value
#if PYXES
#define MAXLINGER 0.1  //  maximum time we will allow for lingering
  ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==2,EVLENGTH)  // arg is threadpool# keepwarm
  if(AT(w)!=FL)RZ(w=ccvt(FL,w,0));  // make arg float type
  D dpoolno=DAV(w)[0]; I poolno=(I)dpoolno; ASSERT((D)poolno==dpoolno,EVDOMAIN) ASSERT(BETWEENO(poolno,0,MAXTHREADPOOLS),EVLIMIT)  // extract threadpool# and audit it
  JOBQ *jobq=&(*JT(jt,jobqueues))[poolno];
  D oldval=jobq->keepwarmns*1e-9;
  D kwtime=DAV(w)[1]; ASSERT(kwtime>=0,EVDOMAIN); if(unlikely(kwtime>MAXLINGER))kwtime=MAXLINGER; I kwtimens=(I)(kwtime*1000000000);  // limit time and convert to ns
  jobq->keepwarmns=kwtimens;  // store new value
  z=scf(oldval);  // return old value
#else
ASSERT(0,EVNONCE)
#endif
  break;}
 case 15:  { // kick a threadpool
#if PYXES
  I poolno=0;  // default to threadpool 0
  if(AN(w)){   // arg is [threadpool #]
   ASSERT(AR(w)<=1,EVRANK) ASSERT(AN(w)<=1,EVLENGTH)  // must be singleton
   RZ(w=vi(w)) poolno=IAV(w)[0]; ASSERT(BETWEENO(poolno,0,MAXTHREADPOOLS),EVLIMIT)  // extract threadpool# and audit it
  }
  JOBQ *jobq=&(*JT(jt,jobqueues))[poolno];
  // wakeall sequence: lock the pool; while under lock, advance futex value to cause a wakeup, so that if a waiter finishes its keepwarm it will start another one; unlock; kick.  No new work added
  JOB *job=JOBLOCK(jobq); ++jobq->futex; JOBUNLOCK(jobq,job); jfutex_wakea(&jobq->futex);
  z=mtm;
#else
ASSERT(0,EVNONCE)
#endif
  break;}
 case 3: { // return current thread #,threadpool,threadpool index
  ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==0,EVLENGTH)  // only '' is allowed as an argument for now
  RZ(z=vec(INT,3,((I[3]){THREADID(jt),jt->threadpoolno,jt->ndxinthreadpool})))
  break;}
 case 1: { // return net number of threads created, not counting terminated & inactive; i. e. #active threads in the long run
  ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==0,EVLENGTH)  // only '' is allowed as an argument for now
  WRITELOCK(JT(jt,flock))  // nwthreads is protected by flock
  I nact=0; DONOUNROLL(JT(jt,wthreadhwmk), nact+=(__atomic_load_n(&(JTFORTHREAD(jt,THREADIDFORWORKER(i)))->taskstate,__ATOMIC_ACQUIRE)&TASKSTATETERMINATE+TASKSTATEACTIVE)==TASKSTATEACTIVE;)
  WRITEUNLOCK(JT(jt,flock))  // nwthreads is protected by flock
  RZ(z=sc(nact))
  break;}
 case 0:  { // create a thread and start it.  positional arg is threadpool#, keyword is coremask (integer/char mask value required)
#if PYXES
  // extract args
  ASSERT(AR(w)<=1,EVRANK) // arg must be atom or list
  A coremask=0;  // establish unset values for options
  I poolno=0;  // default to using threadpool 0
  A afixed=0;  // the fixed-format args if any
  // parse the options
  // Go through each box, analyzing.  If we hit leading fixed-format options, remember where and skip for later
  DO(AN(w),
   A akw; A aval;  // A block for keyword, A block for value
   if(AT(w)&BOX){
    A boxl1=C(AAV(w)[i]);  // contents of first-level box to examine
    if(AN(boxl1)==0){ASSERT(i==0,EVDOMAIN) afixed=boxl1; continue;}  // first box empty; call it fixed options
    if(AT(boxl1)&NUMERIC){ASSERT(i==0,EVDOMAIN) afixed=boxl1; continue;}  // first box numeric, call it fixed options
    if(AT(boxl1)&BOX){   // box contains boxes
     A boxl2=C(AAV(boxl1)[0]);   // w is (<((<boxl2), ...))
     if(AN(boxl2)==0){ASSERT(i==0,EVDOMAIN) afixed=boxl2; continue;}   // first keyword empty or numeric, treat as fixed option
     if(AT(boxl2)&NUMERIC){ASSERT(i==0,EVDOMAIN) afixed=boxl2; continue;}
     akw=boxl2;   // the keyword
     if(!(AT(akw)&LIT))RZ(akw=cvt(LIT,akw))  // keyword must be literal type
     ASSERT(AR(boxl1)<2,EVRANK) ASSERT(AN(boxl1)<=2,EVLENGTH) aval=AN(boxl2)==1?0:C(AAV(boxl1)[1]);  // arg has only 0-1 value; get value if any
    }else{
     akw=boxl1;  // the keyword
     if(!(AT(akw)&LIT))RZ(akw=cvt(LIT,akw))  // keyword must be literal type
     aval=0;
    }
   }else if(AT(w)&NUMERIC){afixed=w; break;  // numeric arg must be fixed-format
   }else{akw=w; if(!(AT(akw)&LIT))RZ(akw=cvt(LIT,akw)) aval=0;  // string arg is a valueless keyword
   }
   // we have the keyword/value; examine them one by one
   if(strncmp(CAV(akw),"coremask",AN(akw))==0){
    ASSERT(coremask==0,EVDOMAIN)  // can't set same parm twice
    ASSERT(aval!=0,EVDOMAIN) ASSERT(AR(aval)<=1,EVRANK) ASSERT(AN(aval)!=0,EVLENGTH) if(unlikely(AT(aval)&FL+B01))aval=cvt(INT,aval); ASSERT(AT(aval)&INT+LIT,EVDOMAIN)  // cannot omit mask, which must be nonempty rank<2 IN T or LIT
    coremask=aval;  // if value OK, keep it.  We will send it to pthread_create
   }else{ASSERT(0,EVDOMAIN)}   // error if keyword is unknown
   if(!(AT(w)&BOX))break;  // unboxed must be a single value
  )
  // if there is a fixed-format area, analyze it
  if(afixed){
   ASSERT(AR(afixed)<2&&AN(afixed)<=1,EVDOMAIN)  // fixed area is [threadpoolno]
   if(likely(AN(afixed)>=1)){
    RZ(afixed=vi(afixed)) poolno=IAV(afixed)[0]; ASSERT(BETWEENO(poolno,0,MAXTHREADPOOLS),EVLIMIT)  // verify value in bounds
   }
  }


  // input parms have been read.  Allocate the thread
  // create the attributes for the thread
  pthread_attr_t threadattrs; ASSERT(pthread_attr_init(&threadattrs)==0,EVFACE)
#if SUPPORT_AFFINITY && !defined(__FreeBSD__)
#if defined(ANDROID)
  if(coremask!=0)ASSERT(sched_setaffinity(pthread_gettid_np(pthread_self()),AN(coremask)<<bplg(AT(coremask)),(cpu_set_t*)IAV(coremask))==0,EVFACE)
#elif defined(__APPLE__)
  if(coremask!=0)ASSERT(pthread_setaffinity_np(pthread_self(),AN(coremask)<<bplg(AT(coremask)),(cpu_set_t*)IAV(coremask))==0,EVFACE)
#else
  if(coremask!=0)ASSERT(pthread_attr_setaffinity_np(&threadattrs,AN(coremask)<<bplg(AT(coremask)),(cpu_set_t*)IAV(coremask))==0,EVFACE)
#endif
#else
  ASSERT(coremask==0,EVNONCE)  // if affinity not supported, fail a request for it
#endif
  // We must not increase the # running tasks while suspension is running.  If we do, we have no way to tell the task that a system lock is active, and we also have
  // no way to prevent the created task from starting up running, thus violating the lock rules.
  ASSERT(lda(&JT(jt,systemlock))<=2,EVSIDAMAGE)  // if the lock has already started, this must be an execution from debug suspension.  Fail it
  // if the threadslot we will use is being terminated, we have to wait for termination to finish, so that we can restart it with the correct threadpool
  // We also have to ensure that the thread data is initialized and the virtual memory for it is mapped
  // Finally, we have to ensure that we don't add a thread during systemlock, from the time the threads are counted until the end.  If systemlock has started at all, we defer to it
  I resthread;  // thread# we will be allocating
  // ***** start of lock on thread info
  while(1){
   WRITELOCK(JT(jt,flock))  // nwthreads is protected by flock
   if(unlikely(lda(&JT(jt,systemlock))>0)){WRITEUNLOCK(JT(jt,flock)) jtsystemlockaccept(jt,LOCKALL); continue;}  // allow syslock if requested
   // Scan threads backwards to find an empty slot (i. e. !ACTIVE, or terminating); convert worker# to thread#
   for(resthread=0;resthread<JT(jt,wthreadhwmk);++resthread)if((__atomic_load_n(&(JTFORTHREAD(jt,THREADIDFORWORKER(resthread)))->taskstate,__ATOMIC_ACQUIRE)&TASKSTATETERMINATE+TASKSTATEACTIVE)!=TASKSTATEACTIVE)break;
   resthread=THREADIDFORWORKER(resthread); // If no empty slot, use next slot; convert worker# to thread#
   ASSERTSUFF(resthread<MAXTHREADS,EVLIMIT,WRITEUNLOCK(JT(jt,flock)); R 0;); //  error if new 0-origin thread# exceeds limit
   if(!jvmcommit(JTFORTHREAD(jt,resthread),sizeof(JTT))){ // attempt to commit thread data (in case it's not already committed); if failed, then bail
    WRITEUNLOCK(JT(jt,flock));
    ASSERT(0,EVWSFULL);}
   if(unlikely(!jtjinitt(JTFORTHREAD(jt,resthread)))){WRITEUNLOCK(JT(jt,flock)); R 0;} // initialise thread-local state
   if(!(lda(&JTFORTHREAD(jt,resthread)->taskstate)&TASKSTATETERMINATE))break;  // normal case: not terminating, continue holding lock
   WRITEUNLOCK(JT(jt,flock))  // release lock for next poll
   YIELD  // let other threads run while we wait for the on-deck thread to terminate
  }
  // we have a lock on the overall thread info; and resthread, the slot we want to fill, is idle.  keep the lock while we fill it.  systemlock will not count threads until we have finished adding and starting the new one
  JOBQ *jobq=&(*JT(jt,jobqueues))[poolno];
  ASSERTSUFF(jobq->nthreads<MAXTHREADSINPOOL,EVLIMIT,WRITEUNLOCK(JT(jt,flock)); R 0;); //  error if threadpool limit exceeded.  OK to CHECK outside of job lock
  // We also have to lock the threadpool before changing nthreads, because jobq->nthreads is used to decide whether to start a job
  JOB *job=JOBLOCK(jobq);  // must modify thread info under lock on the threadpool
  C origstate=__atomic_fetch_or(&JTFORTHREAD(jt,resthread)->taskstate,TASKSTATEACTIVE,__ATOMIC_ACQ_REL);  // put into ACTIVE state
  JTFORTHREAD(jt,resthread)->threadpoolno=poolno;  // install threadpool number
  JTFORTHREAD(jt,resthread)->ndxinthreadpool=jobq->nthreads+1;  // install 1-origin ndx within pool.  Always ascending in the threads, since we delete only from the end
  // Try to allocate a thread in the OS and start it running.  We hold locks while this is happening, so thread startup must be lock-free
  if(jtthreadcreate(jt,resthread,&threadattrs)){   // start thread.  thread started normally?
   if(WORKERIDFORTHREAD(resthread)>=JT(jt,wthreadhwmk))JT(jt,wthreadhwmk)=WORKERIDFORTHREAD(resthread+1);   // if adding a new thread, increment hwmk
   ++jobq->nthreads;  // incr # threads in pool
  }else resthread=0;  // if error, mark invalid thread#; error signaled earlier
  JOBUNLOCK(jobq,job);  // We don't add a job - just unlock
  WRITEUNLOCK(JT(jt,flock))  // release lock on global thread data
  pthread_attr_destroy(&threadattrs);  // release attr block
  // ***** end of lock on thread info
  jvmwire(JTFORTHREAD(jt,resthread),sizeof(JTT)); // try to wire thread data.  Do this outside of the lock, since failure is not catastrophic.  Also don't check for error before doing the wiring; if there was an error, resthread=0, so we just harmlessly wire thread 0's data again.
  z=resthread?sc(resthread):0;  // if no error, return thread# started
#else
  ASSERT(0,EVLIMIT)
#endif
  break;}
 case 10: {  // create a mutex.  w indicates recursive status
#if PYXES
  I recur; RE(recur=i0(w)) ASSERT((recur&~1)==0,EVDOMAIN)  // recur must be 0 or 1
  A zz;GAT0(zz,INT,(sizeof(jtpthread_mutex_t)+SZI-1)>>LGSZI,0); ACINITUNPUSH(zz); AN(zz)=1; AM(zz)=CREDMUTEX;  // allocate mutex, make it immortal and atomic, install credential
  jtpthread_mutex_init((jtpthread_mutex_t*)IAV0(zz),recur);
  z=box(zz);  // protect in a box in case the mutex is copied
#else
  ASSERT(0,EVNONCE)
#endif
  break;}
 case 11: {  // lock mutex.  w is mutex[;timeout] timeout of 0=trylock
#if PYXES
  ASSERT(AT(w)&BOX,EVDOMAIN) ASSERT(AR(w)<=1,EVRANK) ASSERT(AN(w)<=2,EVLENGTH)
  A mutex=AN(w)==1?w:C(AAV(w)[0]); D timeout=inf; I lockfail=0;
  if(AN(w)==2){A tob=C(AAV(w)[1]); ASSERT(AR(tob)<=1,EVRANK) ASSERT(AN(tob)==1,EVLENGTH) if(!(AT(tob)&FL))RZ(tob=ccvt(FL,tob,0)) timeout=DAV(tob)[0];}  // pull out timeout
  ASSERT(AT(mutex)&BOX,EVDOMAIN) ASSERT(AR(mutex)==0,EVRANK) mutex=AAV(mutex)[0];  // mutex is a box protecting the actual mutex: open it
  ASSERT(AT(mutex)&INT,EVDOMAIN); ASSERT(AM(mutex)==CREDMUTEX,EVDOMAIN);  // verify valid mutex
  if(timeout==inf){  // is there a max timeout?
   C c=jtpthread_mutex_lock(jt,(jtpthread_mutex_t*)IAV0(mutex),1+THREADID(jt));ASSERT(!c,c); //1+ is to ensure nonzero id.  TODO id should be unique per-task, not just per-thread
  }else if(timeout==0.0){
   I lockrc=jtpthread_mutex_trylock((jtpthread_mutex_t*)IAV0(mutex),1+THREADID(jt));
   ASSERT(lockrc<=0,lockrc);  // positive is a hard failure
   lockfail=lockrc==-1;  // -1 is a soft failure
  }else{
   ASSERT(timeout<=9e9,EVLIMIT); // 9e9 is approx 63 bits of ns.  This leaves ~300y; should be ok
   I lockrc=jtpthread_mutex_timedlock(jt,(jtpthread_mutex_t*)IAV0(mutex),1e9*timeout,1+THREADID(jt));
   ASSERT(lockrc<=0,lockrc);  // positive is a hard failure
   lockfail=lockrc==-1;  // -1 is a soft failure
  }
  z=num(lockfail);
#else
  ASSERT(0,EVNONCE)
#endif
  break;}
 case 13: {  // unlock mutex.  w is mutex
#if PYXES
  A mutex=w;
  ASSERT(AT(mutex)&BOX,EVDOMAIN) ASSERT(AR(mutex)==0,EVRANK) mutex=AAV(mutex)[0];  // mutex is a box protecting the actual mutex: open it
  ASSERT(AT(mutex)&INT,EVDOMAIN); ASSERT(AM(mutex)==CREDMUTEX,EVDOMAIN);  // verify valid mutex
  C c=jtpthread_mutex_unlock((jtpthread_mutex_t*)IAV0(mutex),1+THREADID(jt));
  ASSERT(!c,c);
  z=mtm;
#else
  ASSERT(0,EVNONCE)
#endif
  break;}
 case 16: {  // create an AMV.  w is initial value
#if PYXES
  I initval; RE(initval=i0(w))  // recur must be integer
  A zz; GAT0(zz,INT,1,0);  ACINITUNPUSH(zz); AN(zz)=1; AM(zz)=CREDAMV; IAV0(zz)[0]=initval;  // AMV is a boxed integer atom.  The boxing is needed to protect the value from being virtualized and then realized in a different place
  z=box(zz); 
#else
  ASSERT(0,EVNONCE)
#endif
  break;}
 case 17: {  // atomic add to atom, returning previous value
#if PYXES
  ASSERT(AT(w)&BOX,EVDOMAIN) ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==2,EVLENGTH)  // w is 2 boxes
  A amv=AAV(w)[0]; ASSERT(AT(amv)&INT,EVDOMAIN) ASSERT(AN(amv)==1,EVLENGTH) ASSERT(AM(amv)==CREDAMV,EVDOMAIN)  // amv is singleton integer - we don't verify anything else
  I imod; RE(imod=i0(AAV(w)[1]))  // addend must be integer
  z=sc(__atomic_fetch_add(&IAV(amv)[0],imod,__ATOMIC_ACQ_REL));  // do the add, return unincremented value
#else
  ASSERT(0,EVNONCE)
#endif
  break;}
 case 18: {  // compare-and-swap on atom
#if PYXES
  ASSERT(AT(w)&BOX,EVDOMAIN) ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==3,EVLENGTH)  // w is 2 boxes
  A amv=AAV(w)[0]; ASSERT(AT(amv)&INT,EVDOMAIN) ASSERT(AN(amv)==1,EVLENGTH) ASSERT(AM(amv)==CREDAMV,EVDOMAIN)  // amv is singleton integer - we don't verify anything else
  I desired; RE(desired=i0(AAV(w)[1]))  // replacement value
  I expected; RE(expected=i0(AAV(w)[2]))  // expected value
  z=mtv; if(!__atomic_compare_exchange_n(&IAV(amv)[0], &expected, desired, 0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED))z=sc(expected);  // if no match for expected, return new value; otherwise modify & return ''
#else
  ASSERT(0,EVNONCE)
#endif
  break;}
 case 20: {  // yield
#if PYXES
  ASSERTMTV(w);
  YIELD
  z=mtv;
#else
  ASSERT(0,EVNONCE)
#endif
  break;}
 case 55: {  // destroy thread.  w is threadpool to destroy from, or '' for highest-numbered thread
  // A thread is set to ACTIVE state when it is created.  It sets itself !ACTIVE when it returns.
  // ACTIVE is changed only under the job lock.
  // To kill a thread, we set TERMINATE which causes the thread to return when it notices.
  // JT(jt,nwthreads) is the number of threads we have in the long run.  Terminated threads that are
  // still running are not counted in nwthreads.
  // If we create a thread and the new thread is still ACTIVE, we simply remove TERMINATE.  Thus,
  // TERMINATE is changed only under the job lock.
  // We return from the terminate request with the thread possibly still running a task.  After all, we could be terminating ourselves!
  // If we are told to kill a thread that is not ACTIVE or is already terminating, we take no action
  // Result is # of terminated thread, 0 if none
#if PYXES
  I threadpool;   // the threadpool to look in, if any
  if(AR(w)!=0){ASSERTMTV(w); threadpool=-1;}else{RE(threadpool=i0(w)); ASSERT(BETWEENO(threadpool,0,MAXTHREADPOOLS),EVLIMIT) }
  I resthread;  //  the thread# we will delete
  JOBQ *jobq;  // JOBQ for the thread
  JOB *job;  // the first job on the JOBQ
  // Acquire locks, and make sure that we don't delete the last thread in a pool that has jobs pending.  Wait till the jobs finish
  // NOTE that it is OK to reduce nthreads even if a systemlock is running, because systemlock will always unlock any thread it locked initially.  The blocks are never unmapped
  // ***** start of lock
  while(1){
   WRITELOCK(JT(jt,flock))  // nwthreads is protected by flock
   for(resthread=JT(jt,wthreadhwmk)-1;resthread>=0;--resthread){
    if((__atomic_load_n(&JTFORTHREAD(jt,THREADIDFORWORKER(resthread))->taskstate,__ATOMIC_ACQUIRE)&TASKSTATETERMINATE+TASKSTATEACTIVE)==TASKSTATEACTIVE){  // look for active & !terminating
     if(threadpool<0||JTFORTHREAD(jt,THREADIDFORWORKER(resthread))->threadpoolno==threadpool)break;  // if no threadpool given, or pool of thread matches the one requested 
    }
   }
   resthread=THREADIDFORWORKER(resthread);  // convert worker# to thread#
   ASSERTSUFF(resthread>=1,EVLIMIT,WRITEUNLOCK(JT(jt,flock)); R 0;); //  error if no thread to delete
   jobq=&(*JT(jt,jobqueues))[JTFORTHREAD(jt,resthread)->threadpoolno];
   job=JOBLOCK(jobq);  // must change status under lock for the threadpool
   if(job==0||jobq->nthreads>1)break;  // normal continuation: not last thread in a busy pool.  Wait for that
   JOBUNLOCK(jobq,job);  // We don't add a job - we just kick all the threads
   WRITEUNLOCK(JT(jt,flock))  // nwthreads is protected by flock
   if(unlikely(lda(&JT(jt,adbreak)[1]))!=0){jtsystemlockaccept(jt,LOCKALL);}else{YIELD}  // allow syslock if requested; otherwise let other threads run
  }
  // Now we have locks on the flock and the JOBQ
  // Mark the thread for deletion, wake up all the threads
  C oldstate=__atomic_fetch_or(&JTFORTHREAD(jt,resthread)->taskstate,TASKSTATETERMINATE,__ATOMIC_ACQ_REL);  // request term.  Low bits of flag are used outside of lock
  if(unlikely(!(oldstate&TASKSTATEACTIVE))){__atomic_fetch_and(&JTFORTHREAD(jt,resthread)->taskstate,~TASKSTATETERMINATE,__ATOMIC_ACQ_REL); goto notermlocks;}  // if thread no longer active, abort releasing locks.  should not occur
  --jobq->nthreads;  // remove thread from count in threadpool
  ++jobq->futex;  // while under lock, advance futex value to indicate that we have added work: not a job, but the thread
  JOBUNLOCK(jobq,job);  // We don't add a job - we just kick all the threads
  WRITEUNLOCK(JT(jt,flock))
  // ***** end of lock
  jfutex_wakea(&jobq->futex);  // wake em all up
  z=num(1);  // indicate we terminated a thread
  break;
 notermlocks:
  JOBUNLOCK(jobq,job);  // We don't add a job - we just kick all the threads
  WRITEUNLOCK(JT(jt,flock))
  z=num(0);  // indicate we didn't terminate
  break;
#else
  ASSERT(0,EVNONCE)
#endif
 }
 default: ASSERT(0,EVDOMAIN) break;
 }
 RETF(z);  // return thread#
}
