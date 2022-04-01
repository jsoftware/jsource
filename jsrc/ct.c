/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */

// Threads and Tasks
#include "j.h"

// burn some time, approximately n nanoseconds
NOINLINE I delay(I n){I johnson=0x1234; do{johnson ^= (johnson<<1) ^ johnson>>(BW-1);}while(--n); R johnson;}

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
 GA0(z,t,2*oldn,AR(*abuf)); ACINITZAP(z); // allocate a new block, bigger than the previous one; make it permanent.  This is the only error exit from this routine
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
  if(unlikely(t&BOX))mvc((AN(z)-2*oldn)*BOXSIZE,AAV(z)+2*oldn,1,MEMSET00);
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
// perform the action for state n.  In the leader, set ct/advance/act/wait  In others, wait/act/decr ct
#define DOINSTATE(l,n,expr) \
 {if(l){__atomic_store_n(&JT(jt,systemlocktct),nrunning-1,__ATOMIC_RELEASE); __atomic_store_n(&JT(jt,systemlock),(n),__ATOMIC_RELEASE);}else while(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)!=(n))delay(20);  \
  expr \
  if(l)while(__atomic_load_n(&JT(jt,systemlocktct),__ATOMIC_ACQUIRE)!=0)delay(20);else __atomic_fetch_sub(&JT(jt,systemlocktct),1,__ATOMIC_ACQ_REL); \
 }
 
// Take lock on the entire system, waiting till all threads acknowledge
// priority is the priority of the request.  lockedfunction is the function to call when the lock has been agreed.
// if multiple requesters ask for a lock, the function will be called in only one of them
// lockedfunction should return 0 for error, 1 for OK.  If it returns error, the error will be signaled to all tasks
// Result is 0 for error, otherwise OK
I jtsystemlock(J jt,I priority,I (*lockedfunction)()){
 // If the system is already in systemlock, the system is essentially single-threaded.  Just execute the user's function.
 // This would happen if a sentence executed in debug suspension needed symbols, or had an error
 if(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)>2){(*lockedfunction)(jt); RE(0); R 1;}
 // Process the request.  We don't know what the highest-priority request is until we have heard from all the
 // threads.  Thus, it is possible that our request will still be pending whe we finish.  In that case, loop till it is satisfied
 while(priority!=0){
  // If the systemlock is negative, the system is still finishing the previous lock.  Wait for it to clear
  while(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)<0)delay(100);
  S xxx=0; I leader=__atomic_compare_exchange_n(&JT(jt,systemlock), &xxx, (S)1, 0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED);  // go to state 1; set leader if we are the first to do so
  I nrunning=0; JTT *jjbase=JTTHREAD0(jt);  // #running threads, base of thread blocks
  // In the leader task only, go through all tasks, turning on the SYSLOCK task flag in each thread.  Count how many are running after the flag is set
  if(leader){DONOUNROLL(MAXTASKS, nrunning+=(__atomic_fetch_or(&jjbase[i].taskstate,TASKSTATELOCKACTIVE,__ATOMIC_ACQ_REL)>>TASKSTATERUNNINGX)&1;)}
  // state 2: lock requesters indicate request priority and we wait for all tasks to come to a stop
  C oldpriority; DOINSTATE(leader,2,oldpriority=__atomic_fetch_or(&JT(jt,adbreak)[1],priority,__ATOMIC_ACQ_REL);)  // remember pririty before we made or request
  // state 3: all threads get the final request priorities
  C finalpriority; DOINSTATE(leader,3,finalpriority=__atomic_load_n(&JT(jt,adbreak)[1],__ATOMIC_ACQUIRE);)
  I winningpri=LOWESTBIT(finalpriority); I executor=winningpri&priority&~oldpriority;  // priority to execute: were we the first to request it?
  // state 4: transfer nrunning to executor
  if(leader){__atomic_store_n(&JT(jt,systemlocktct),nrunning,__ATOMIC_RELEASE); __atomic_store_n(&JT(jt,systemlock),4,__ATOMIC_RELEASE);}
  if(executor){  // if we were the first to request the winning priority
   // This is the winning thread.  Perform the function and save the error status
   while(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)!=4)delay(20); nrunning=__atomic_load_n(&JT(jt,systemlocktct),__ATOMIC_ACQUIRE);  // pick up nrunning
   (*lockedfunction)(jt);
   __atomic_store_n(&((C*)&JT(jt,breakbytes))[1],jt->jerr,__ATOMIC_RELEASE);
  }
  // state 5: everybody gets the result of the operation
  C res; DOINSTATE(executor,5,res=__atomic_load_n(&((C*)&JT(jt,breakbytes))[1],__ATOMIC_ACQUIRE);)
  // Now wind down the lock.
  if(executor){
   __atomic_store_n(&((C*)&JT(jt,breakbytes))[1],0,__ATOMIC_RELEASE);  // clear the error flag from the interrupt request
   // remove the lock request from the break field
   __atomic_store_n(&JT(jt,adbreak)[1],0,__ATOMIC_RELEASE);
   // go through all threads, turning off SYSLOCK in each.  This allows other tasks to run and new tasks to start
   DO(MAXTASKS, __atomic_fetch_and(&jjbase[i].taskstate,~TASKSTATELOCKACTIVE,__ATOMIC_ACQ_REL);)
   // wait for the systemlocktct to go to 0 (all the running threads except us decrement it)
   while(__atomic_load_n(&JT(jt,systemlocktct),__ATOMIC_ACQUIRE)!=0)delay(20);
   // set the systemlock to 0, completing the operation
   __atomic_store_n(&JT(jt,systemlock),0,__ATOMIC_RELEASE);
  }else{
   // outside the executor, we wait for our tasklock to be removed
   while(__atomic_load_n(&jt->taskstate,__ATOMIC_ACQUIRE)&TASKSTATELOCKACTIVE)delay(20);
  }
  ASSERT(res==0,res)  // if there was an error, signal it in all threads
  priority&=~winningpri;  // if our request was serviced, remove it, regardless of who serviced it
 }
 R 1;  // no error
}

// Allow a lock to proceed.  Called by a running thread when it notices the broadcast system-lock request at its priority or higher
// priority is mask indicating the priorities for which this accept is valid
// Result is 0 if the lock processing failed
I jtsystemlockaccept(J jt, I priority){
 do{
  while(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)!=2)delay(20); __atomic_fetch_sub(&JT(jt,systemlock),1,__ATOMIC_ACQ_REL);  // state 2: requesters raise requests
  while(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)!=3)delay(20); C finalpriority=__atomic_load_n(&JT(jt,adbreak)[1],__ATOMIC_ACQUIRE); __atomic_fetch_sub(&JT(jt,systemlock),1,__ATOMIC_ACQ_REL);  // state 3: see what won
  while(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)!=4)delay(20); __atomic_fetch_sub(&JT(jt,systemlock),1,__ATOMIC_ACQ_REL);  // state 4: transfer to executor
  while(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)!=5)delay(20); C res=__atomic_load_n(&((C*)&JT(jt,breakbytes))[1],__ATOMIC_ACQUIRE); __atomic_fetch_sub(&JT(jt,systemlock),1,__ATOMIC_ACQ_REL);  // state 5: get result status
  while(__atomic_load_n(&jt->taskstate,__ATOMIC_ACQUIRE)&TASKSTATELOCKACTIVE)delay(20);
  ASSERT(res==0,res)  // if there was an error, signal it in all threads
  // loop back if there is another request that this can tolerate
  I winningpri=LOWESTBIT(finalpriority); finalpriority&=~winningpri; priority&=~finalpriority; // final<-remaining requests; leave priority as the ones we are OK with
 }while(priority);  // if our priority was SYM and the remaining request is DEBUG, we have to return to get to a DEBUG point
 // this thread is free to continue.  No lock request is possible until systemlock has been cleared
 R 1;
}

#if ARTIFPYX
// w is a block that will become the contents of a box.  Put it inside a pyx and return the address of the pyx.
// aflag is the boxing flag of the block the result is going to go into.  w has been prepared for that type
A jtartiffut(J jt,A w,I aflag){A z;
 GAT0(z,BOX,1,0); AFLAG(z)|=BOX; AAV0(z)[0]=w; AT(z)|=PYX; if(aflag&BOX){ra(z);}else{ACIPNO(z); ra(w);}  // just one of z/w must be adjusted to the recursion environment
 R z;
}
#endif
#if PYXES

// ****************************** waiting for values *******************************
typedef struct condmutex{
 pthread_cond_t cond;
 pthread_mutex_t mutex;
} WAITBLOK;

typedef struct pyxcondmutex{
 A pyxvalue;  // the A block of the pyx, when it is filled in.  It is 0 until then.
 S pyxorigthread;  // thread number that is working on this pyx, or _1 if the value is available
 C errcode;  // 0 if no error, or error code
 WAITBLOK pyxwb;  // sync info
} PYXBLOK;

static struct timespec maxwait={0,2000000};  // 2ms - maximum time to wait for a pyx.  After that, check to see if a system lock has been requested

// w is an A holding a pyx value.  Return its value when it has been resolved
A jtpyxval(J jt,A pyx){A res; C errcode;
 // read the pyx value.  Since the creating thread has a release barrier after creation and another after final resolution, we can be sure
 // that if we read nonzero the pyx has been resolved, even without an acquire barrier
 while((res=__atomic_load_n(&((PYXBLOK*)AAV0(pyx))->pyxvalue,__ATOMIC_ACQUIRE))==0&&(errcode=__atomic_load_n(&((PYXBLOK*)AAV0(pyx))->errcode,__ATOMIC_ACQUIRE))==0){  // repeat till defined, in case we get spurious wakeups
  // wait till the value is defined.  We have to make one last check inside the lock to make sure the value is still unresolved
  pthread_mutex_lock(&((PYXBLOK*)AAV0(pyx))->pyxwb.mutex);
  if((res=__atomic_load_n(&((PYXBLOK*)AAV0(pyx))->pyxvalue,__ATOMIC_ACQUIRE))==0&&(errcode=__atomic_load_n(&((PYXBLOK*)AAV0(pyx))->errcode,__ATOMIC_ACQUIRE))==0)
   pthread_cond_wait(&((PYXBLOK*)AAV0(pyx))->pyxwb.cond,&((PYXBLOK*)AAV0(pyx))->pyxwb.mutex);
  pthread_mutex_unlock(&((PYXBLOK*)AAV0(pyx))->pyxwb.mutex);
 }
 // res now contains the certified value of the pyx.
 if(likely(res!=0))R res;   // valid value, use it
 ASSERT(0,errcode)   // if error, return the error code
}

// ************************************* Locks **************************************
// take a readlock on *alock.  We come here only if a writelock was requested or running
void readlock(S *alock, S prev){
 // loop until we get the lock
 do{
  __atomic_fetch_sub(alock,1,__ATOMIC_ACQ_REL);  // rescind our read request.  The writer may hitch slightly when he sees our request, but we won't put it up more than once
  // spin until any write request has gone away
  I nspins=5000;  // good upper bound on the amount of time a write could reasonably take, in cycles
  while(prev<0){
   // we are delaying while a writer finishes.  Usually this will be fairly short, as controlled by nspins.  The danger is that the
   // writer will be preempted, leaving us in a tight spin.  If the spin counter goes to 0, we decide this must have happened, and we
   // do a low-power delay for a little while (method TBD)
   if(--nspins==0){nspins=5000; delay(5000);}
   delay(20);  // delay a little to reduce bus traffic while we wait for the writer to finish
   prev=__atomic_load_n(alock,__ATOMIC_ACQUIRE);
  }
  // try to reacquire the lock, loop if can't
 }while(__atomic_fetch_add(alock,1,__ATOMIC_ACQ_REL)<0);
}

// take a writelock on *alock.  We have turned on the write request; we come here only if the lock was in use.  The previous value was prev
void writelock(S *alock, S prev){
 // loop until we get the lock
 I nspins;
 while(1) {
 // if another writer has requested, they will win.  wait until they finish.  As above, back off if it looks like they were preempted
  nspins=prev&0x7fff?5000+1000:5000;  // max expected writer delay, plus reader delay if there are readers, in 20-ns units
  while(prev<0){
   if(--nspins==0){nspins=5000; delay(500000);}
   delay(20);  // delay a little to reduce bus traffic while we wait for the writer to finish
   prev=__atomic_load_n(alock,__ATOMIC_ACQUIRE);
  }
  // try to reacquire the writelock
  if(prev=__atomic_fetch_or(alock,(S)0x8000,__ATOMIC_ACQ_REL)>=0)break;  // put up our request; if no previous writer, it is valid, go wait for readers to finish
  //  here another writer beat us to the request.  Very rare.  Go back to wait
 }
 // We are the owner of the current write request.  When the reads finish, we have the lock
 nspins=1000;  // max expected reader delay, in 20-ns units.  They are all running in parallel
 while(prev&0x7fff){  // wait until reads complete
  if(--nspins==0){nspins=1000; delay(500000);}  // delay if a thread seems to have been preempted
  delay(20);  // delay a little to reduce bus traffic while we wait for the readers to finish
  prev=__atomic_load_n(alock,__ATOMIC_ACQUIRE);
 }
}

// *********************** task creation ********************************

// Processing loop for thread.  Create a wait block for the thread, and wait on it.  Each loop runs one user task
static void* jtthreadmain(void * arg){J jt=(J)arg; WAITBLOK wblok;
 // get/set stack limits
// not supported on Windows if(pthread_attr_getstackaddr(0,(void **)&jt->cstackinit)!=0)R 0;
 jt->cstackinit=(UI)&wblok;  // use a local as a surrogate for the stack pointer
 // allocate condition & mutex here in a struct & initialize
 pthread_cond_init(&wblok.cond,0); pthread_mutex_init(&wblok.mutex,0);
 A *old=jt->tnextpushp;  // we leave a clear stack when we go
 while(1){
  // put pointer to our cond/mutex into commregion so other tasks can see it
  // take our mutex
  // put our thread on the thread queue
  TASKAWAITBLOK(jt)=&wblok; J mjt=MTHREAD(JJTOJ(jt));
  WRITELOCK(mjt->tasklock);  jt->taskidleq=mjt->taskidleq; mjt->taskidleq=THREADID(jt); WRITEUNLOCK(mjt->tasklock);   // atomic install at head of chain
  // wait, then release mutex
  pthread_cond_wait(&wblok.cond,&wblok.mutex); pthread_mutex_unlock(&wblok.mutex);
  // extract task parameters: args & pyx block.  Their usecount was raised before we started.  The self here is for u t. v so that we can get to the v 
  A arg1=TASKCOMMREGION(jt)[0]; A arg2=TASKCOMMREGION(jt)[1]; A arg3=TASKCOMMREGION(jt)[2]; A pyx=TASKCOMMREGION(jt)[3];
  // initialize the non-parameter part of task block
  memset(&jt->uflags.us.uq,0,offsetof(JTT,ranks)-offsetof(JTT,uflags.us.uq));    // clear what should be cleared
  jt->iepdo=0; jt->xmode=0;  jt->recurstate=RECSTATEBUSY; RESETRANK; jt->locsyms=JT(jt,emptylocale); jt->currslistx=-1;  // init what needs initing.  Notably clear the local symbols
  // go to RUNNING state; but we are not allowed to change state if LOCKACTIVE has been set in our task.  In that
  // case someone has started a system lock and our running status has been captured
  S oldstate=0; while(!__atomic_compare_exchange_n(&jt->taskstate, &oldstate, TASKSTATERUNNING, 0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED)){if(oldstate&TASKSTATELOCKACTIVE)delay(100000); oldstate=0;}
  // run the task, raising & lowering the locale execct.  Bivalent
  I4 savcallstack = jt->callstacknext;   // starting callstack
  A startloc=jt->global;  // point to current global locale
  if(likely(startloc!=0))INCREXECCT(startloc);  // raise usecount of current locale to protect it while running
  I dyad=!(AT(arg2)&VERB); A self=dyad?arg3:arg2;  // the call is either noun self x or noun noun self.  See which set dyad flag and select self.
  // Get the arg2/arg3 to use for u .  These will be the self of u, possibly repeated if there is no a
  A uarg3=FAV(self)->fgh[0], uarg2=arg2; uarg2=dyad?uarg2:uarg3;  // get self, positioned after the last noun arg
  A z=(FAV(FAV(self)->fgh[0])->valencefns[dyad])(jt,arg1,uarg2,uarg3);  // execute the u in u t. v
  if(likely(startloc!=0))DECREXECCT(startloc);  // remove protection from executed locale.  This may result in its deletion
  jtstackepilog(jt, savcallstack); // handle any remnant on the call stack

  // put the result into the result block.  If there was an error, use the error code as the result.  But make sure the value is non0 so the pyx doesn't wait forever
  if(unlikely(z==0)){
   C errcode=jt->jerr; errcode=(errcode==0)?EVSYSTEM:errcode; __atomic_store_n(&((PYXBLOK*)AAV0(pyx))->errcode,errcode,__ATOMIC_RELEASE);  // copy failure code.  Must be non0 - if not that is itself an error
  }else{
   // result was good, use it
   rifv(z);  // realize virtual result before returning
   ra(z);  // since the pyx is recursive, we must ra the result we store into it  could zap
   __atomic_store_n(&((PYXBLOK*)AAV0(pyx))->pyxvalue,z,__ATOMIC_RELEASE);  // set result: value or error code
  }
  __atomic_store_n(&((PYXBLOK*)AAV0(pyx))->pyxorigthread,-1,__ATOMIC_RELEASE);  // set pyx no longer running
  // broadcast to wake up any tasks waiting for the result
  pthread_mutex_lock(&((PYXBLOK*)AAV0(pyx))->pyxwb.mutex); pthread_cond_broadcast(&((PYXBLOK*)AAV0(pyx))->pyxwb.cond); pthread_mutex_unlock(&((PYXBLOK*)AAV0(pyx))->pyxwb.mutex);
  // unprotect args
  fa(arg1); fa(arg2); if(dyad){fa(arg3);}
  // unprotect pyx
  fa(pyx);
  // go back to non-RUNNING state, but if SYSTEMLOCK has been started with us counted active go handle it
  oldstate=TASKSTATERUNNING; while(!__atomic_compare_exchange_n(&jt->taskstate, &oldstate, 0, 0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED)){if(oldstate&TASKSTATELOCKACTIVE)jtsystemlockaccept(jt,LOCKPRIDEBUG|LOCKPRISYM); oldstate=TASKSTATERUNNING;}
  jttpop(jt,old); // clear anything left on the stack after execution
  // loop back to wait for next task
 }
} 

// Create worker thread n, and call its threadmain to start it in wait state
static I jtthreadcreate(J jt,I n){
 pthread_attr_t attr;  // attributes for the task we will start
 // create thread
 pthread_attr_init(&attr);
 ASSERT(pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED)==0,EVFACE)  // default parms, except for DETACHED (which means we will never join() )
 size_t stksiz=CSTACKSIZE;
#if defined(__APPLE__)
 stksiz=pthread_get_stacksize_np(pthread_self());
#elif defined(__linux__) && defined(_GNU_SOURCE)
 pthread_attr_t tattr;  // attributes for the current task
 if(pthread_getattr_np(pthread_self(),&tattr)==0) if(pthread_attr_getstacksize(&tattr,&stksiz)!=0)stksiz=CSTACKSIZE;
#endif
 ASSERT(pthread_attr_setstacksize(&attr,stksiz)==0,EVFACE)    // request sufficient stack size
 ASSERT(pthread_create(&JTFORTHREAD(jt,n)->pthreadid,&attr,jtthreadmain,JTFORTHREAD(jt,n))==0,EVFACE)  // create the thread, save its threadid (by passing its jt into jtthreadmain)
 R 1;
}

// execute the user's task.  Result is a box or a pyx.  Bivalent
static A jttaskrun(J jt,A arg1, A arg2, A arg3){A pyx;
 ARGCHK2(arg1,arg2);  // the verb is not the issue
 I dyad=!(AT(arg2)&VERB); A self=dyad?arg3:arg2;  // the call is either noun self x or noun noun self.  See which set dyad flag and select self.
 // take a thread to run.  if none, just execute & return
 J mjt=MTHREAD(JJTOJ(jt));
 WRITELOCK(mjt->tasklock); J exjt=JTFORTHREAD(jt,mjt->taskidleq); mjt->taskidleq=exjt->taskidleq; WRITEUNLOCK(mjt->tasklock);   // exjt is thread to run; remove from idle q.  If no idle, exjt==mjt
 if(exjt==mjt){
  // there is no idle thread.  Just run the verb in this thread, creating a simple boxed result
  // Get the arg2/arg3 to use for u .  These will be the self of u, possibly repeated if there is no a
  A uarg3=FAV(self)->fgh[0], uarg2=arg2; uarg2=dyad?uarg2:uarg3;  // get self, positioned after the last noun arg
  pyx=(FAV(FAV(self)->fgh[0])->valencefns[dyad])(jt,arg1,uarg2,uarg3);  // execute the u in u t. v
 }else{
  // there is a thread.  start a task there
  // realize virtual arguments; raise the usecount of the arguments including self
  rifv(arg1); ra(arg1); rifv(arg2); ra(arg2); if(dyad){rifv(arg3); ra(arg3);}
  // allocate a result pyx.  Init value, cond, and mutex to idle
  GAT0(pyx,INT,((sizeof(PYXBLOK)+(SZI-1))>>LGSZI)+1,0); ACINITZAP(pyx); AAV0(pyx)[0]=0; // allocate the result pointer (1), and the cond/mutex for the pyx.  ra() the pyx, set to unresolved
  pthread_cond_init(&((PYXBLOK*)AAV0(pyx))->pyxwb.cond,0); pthread_mutex_init(&((PYXBLOK*)AAV0(pyx))->pyxwb.mutex,0);
  // Init the pyx to a recursive box, with raised usecount.  AN=1 always.  But set the value/errcode to NULL/no error and the thread# to the executing thread
  AT(pyx)=BOX+PYX; AFLAG(pyx)=BOX; ACINIT(pyx,ACUC2); AN(pyx)=1; ((PYXBLOK*)AAV0(pyx))->pyxvalue=0; ((PYXBLOK*)AAV0(pyx))->pyxorigthread=THREADID(exjt); ((PYXBLOK*)AAV0(pyx))->errcode=0; 
  // initialize the task parameters.  First, the ones inherited from this jt
  memcpy(exjt,jt,offsetof(JTT,uflags.us.uq));  // the inherited area
  // Now, the args and pyx
  TASKCOMMREGION(exjt)[0]=arg1; TASKCOMMREGION(exjt)[1]=arg2; TASKCOMMREGION(exjt)[2]=arg3; TASKCOMMREGION(exjt)[3]=pyx; 
  // Grab the thread's mutex and wake it up
  WAITBLOK *exwblok=TASKAWAITBLOK(exjt);  // get the address of the task's waitblok
  pthread_mutex_lock(&exwblok->mutex); pthread_cond_signal(&exwblok->cond); pthread_mutex_unlock(&exwblok->mutex);
 }
 R box(pyx);  // Create a RECURSIVE box holding the pyx or result value
}
#endif

// u t. n - start a task.  We just create a vrb to handle the arguments
F2(jttdot){
 ASSERTVN(a,w);
#if PYXES
 ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==0,EVLENGTH)  // only '' is allowed as an argument for now
 R fdef(0,CTDOT,VERB,jttaskrun,jttaskrun,a,w,0,VFLAGNONE,RMAX,RMAX,RMAX);
#else
 ASSERT(PYXES,EVNONCE)
#endif
}

// T. y - set debugging thread #
F1(jttcapdot1){ASSERT(0, EVNONCE)}
// x T. y - various thread and task operations
F2(jttcapdot2){A z;
 ARGCHK2(a,w)
#if PYXES
 I m; RE(m=i0(a))   // get the x argument, which must be an atom
 // process the requested function.  We test by hand because only a few could be called often
 if(likely(m==3)){
  // rattle the boxes of y and return status of each
  ASSERT(SGNIF(AT(w),BOXX)|(-AN(w))<0,EVDOMAIN)   // must be boxed or empty
  GAT(z,INT,AN(w),AR(w),AS(w)) I *zv=IAV(z); A *wv=AAV(w); // allocate result, zv->result area, wv->input boxes
  DONOUNROLL(AN(w), if(unlikely(!(AT(wv[i])&PYX)))zv[i]=-2; else zv[i]=((PYXBLOK*)AAV0(wv[i]))->pyxorigthread;)
 }else if(m==1){
  // return list of idle threads
  ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==0,EVLENGTH)  // only '' is allowed as an argument for now
  GAT0(z,INT,MAXTASKS,1) I *zv=IAV1(z);  // Don't allocate under lock, and list may change: so allocate max possible
  I threadct=0;  J mjt=MTHREAD(JJTOJ(jt)); J currjt=mjt;  // # threads, master thread, current thread
  WRITELOCK(mjt->tasklock);
  while(currjt->taskidleq){
zv[threadct++]=currjt->taskidleq;
 currjt=JTFORTHREAD(jt,currjt->taskidleq);
}
 WRITEUNLOCK(mjt->tasklock);   // copy idle threads to result.  The master can never be idle
  AN(z)=AS(z)[0]=threadct;  // install # idles found
 }else if(m==4){
  // return current thread #
  ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==0,EVLENGTH)  // only '' is allowed as an argument for now
  RZ(z=sc(THREADID(jt)))
 }else if(m==2){
  // return number of threads created
  ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==0,EVLENGTH)  // only '' is allowed as an argument for now
  RZ(z=sc(JT(jt,nwthreads)))
 }else if(m==0){
  // create a thread
  ASSERT(AR(w)==1,EVRANK) ASSERT(AN(w)==0,EVLENGTH)  // only '' is allowed as an argument for now
  // reserve a thread#, verify we have enough thread blocks for it
  I resthread=__atomic_add_fetch(&JT(jt,nwthreads),1,__ATOMIC_ACQ_REL);
  if(resthread>MAXTASKS-1){__atomic_store_n(&JT(jt,nwthreads),MAXTASKS-1,__ATOMIC_RELEASE); ASSERT(0,EVLIMIT);} //  this leaves the tiniest of timing windows, bfd
  // Try to allocate a thread in the OS and start it running
  I threadstatus=jtthreadcreate(jt,resthread);
  if(threadstatus==0){__atomic_add_fetch(&JT(jt,nwthreads),-1,__ATOMIC_ACQ_REL); z=0;  // if error, restore thread count; error signaled earlier
  }else{
   RZ(z=sc(resthread))  // thread# is result.  The thread installs itself into the idleq when it waits
  }
 }else ASSERT(0,EVDOMAIN)
 RETF(z);  // return thread#
#else
 ASSERT(PYXES,EVNONCE)
#endif
}
