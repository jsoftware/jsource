/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */

// Tasks
#include "j.h"

// burn some time
static NOINLINE I delay(I n){I johnson=0x1234; do{johnson ^= (johnson<<1) ^ johnson>>(BW-1);}while(--n); R johnson;}

#if ARTIFHIPREC
// w is a block that will become the contents of a box.  Put it inside a hiprec and return the address of the hiprec.
// aflag is the boxing flag of the block the result is going to go into.  w has been prepared for that type
A jtartiffut(J jt,A w,I aflag){A z;
 GAT0(z,BOX,1,0); AFLAG(z)|=BOX; AAV0(z)[0]=w; AT(z)|=HIPREC; if(aflag&BOX){ra(z);}else{ACIPNO(z); ra(w);}  // just one of z/w must be adjusted to the recursion environment
 R z;
}
#endif
#if HIPRECS

typedef struct condmutex{
 pthread_cond_t cond;
 pthread_mutex_t mutex;
} WAITBLOK;

// w is a A holding a hiprecs value.  Return its value when it has been resolved
A jthipval(J jt,A w){
 // read the hiprecs value.  Since the creating thread has a release barrier after creation and another after final resolution, we can be sure
 // that if we read nonzero the hiprec has been resolved, even without an acquire barrier
 A res=AAV0(w)[0];  // fetch the possible value
 while(res==0){  // repeat till defined, in case we get spurious wakeups
  // wait till the value is defined.  We have to make one last check inside the lock to make sure the value is still unresolved
  pthread_mutex_lock(&((WAITBLOK*)&AAV0(pyx)[1])->mutex);
  if((res=__atomic_load_n(&AAV0(w)[0],__ATOMIC_ACQUIRE)==0)pthread_cond_wait(&((WAITBLOK*)&AAV0(pyx)[1])->cond,&((WAITBLOK*)&AAV0(pyx)[1])->mutex);
  pthread_mutex_unlock(&((WAITBLOK*)&AAV0(pyx)[1])->mutex);
 }
 // res now contains the certified value of the hiprec.
 ASSERT(((I)res&-256)!=0,(I)res)   // if error, return the error code
 R res;  // otherwise return the resolved value
}

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
 }while(__atomic_fetch_add(alock,1,__ATOMIC_ACQ_REL)<0));
}

// take a writelock on *alock.  We have turned on the write request; we come here only if the lock was in use.  The previous value was prev
void writelock(S *alock, S prev){
 // loop until we get the lock
 I nspins;
 while(1) {
 // if another writer has requested, they will win.  wait until they finish.  As above, back off if it looks like they were preempted
  nspins=prev&0x7fff?5000+1000:5000;  // max expected writer delay, plus reader delay if there are readers
  while(prev<0){
   if(--nspins==0){nspins=5000; /* scaf delay(5000*100 nsec); */}
   delay(20);  // delay a little to reduce bus traffic while we wait for the writer to finish
   prev=__atomic_load_n(alock,__ATOMIC_ACQUIRE);
  }
  // try to reacquire the writelock
  if(prev=__atomic_fetch_or(alock,(S)0x8000,__ATOMIC_ACQ_REL)>=0)break;  // put up our request; if no previous writer, it is valid, go wait for readers to finish
  //  here another writer beat us to the request.  Very rare.  Go back to wait
 }
 // We are the owner of the current write request.  When the reads finish, we have the lock
 nspins=1000;  // max expected reader delay.  They are all running in parallel
 while(prev&0x7fff){  // wait until reads complete
  if(--nspins==0){nspins=1000; /* scaf delay(5000*100 nsec); */}  // delay if a thread seems to have been preempted
  delay(20);  // delay a little to reduce bus traffic while we wait for the readers to finish
  prev=__atomic_load_n(alock,__ATOMIC_ACQUIRE);
 }
}


#endif
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
// Take lock on the entire system, waiting till all threads acknowledge
// Result is 1 if we got the lock, 0 if somebody else did or there was an error (which is currently impossible)
I jtsystemlock(J jt){S xxx;
 // Acquire the systemlock.
 xxx=0;
 while(!__atomic_compare_exchange_n(&JT(jt,systemlock), &xxx, (S)1, 0, __ATOMIC_ACQ_REL, __ATOMIC_RELAXED)){
  // Lock is busy.  Two cases are possible: (1) someone else wants a new lock - let them have it; (2) the previous lock is finishing up - wait for it to run to completion
  if(xxx>=0){jtsystemlockaccept(jt); R 0;}   // someone else got it
  xxx=0;  // operation finishing - wait for it
  // should delay?
 }
 // We are the owner.  Go through all tasks, turning on the SYSLOCK task flag in each threads.  Count how many are running after the flag is set
 // if there is only 1 (us), leave the systemlock at 1 and return success
 I nrunning=0; JTT *jjbase=JTTHREAD0(jt);  // our thread#, #running threads, base of thread blocks
 DO(MAXTASKS, nrunning+=(__atomic_fetch_or(&jjbase[i].taskstate,TASKSTATELOCKACTIVE,__ATOMIC_ACQ_REL)&TASKSTATERUNNING)>>TASKSTATERUNNINGX;)
 if(nrunning==1)R 1;  // exit fast if we are the only task running
 // Set the number of other running tasks into the systemlock
 __atomic_store_n(&JT(jt,systemlock),nrunning-1,__ATOMIC_RELEASE);
 // Make the lock request in the break field
 __atomic_store_n(&JT(jt,adbreak)[1],1,__ATOMIC_RELEASE);
 // wait for the systemlock count to go to 0.  This indicates all threads have accepted the lock
 while(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)!=0);
 // store the number of running tasks (including us) in the systemlock.  This is a parm to unlock
 __atomic_store_n(&JT(jt,systemlock),nrunning,__ATOMIC_RELEASE);
 // return.  All threads are ready for the lock processing
 R 1;
}

// Release system lock previously acquired.  Called after all lock processing has finished.
// The systemlock indicates how many other threads are running
void jtsystemunlock(J jt){
 // turn on MSB of systemlock, indicating we are on the way out
 __atomic_fetch_or(&JT(jt,systemlock),0x8000,__ATOMIC_ACQ_REL);
 // remove the lock request from the break field
 __atomic_store_n(&JT(jt,adbreak)[1],0,__ATOMIC_RELEASE);
 // go through all threads, turning off SYSLOCK in each
 JTT *jjbase=JTTHREAD0(jt); DO(MAXTASKS, __atomic_fetch_or(&jjbase[i].taskstate,TASKSTATELOCKACTIVE,__ATOMIC_ACQ_REL);)
 // wait for the systemlock to go to 1 (all the running threads except us decrement it)
 while(__atomic_load_n(&JT(jt,systemlock),__ATOMIC_ACQUIRE)!=(S)0x8001);
 // set the systemlock to 0, completing the operation
 __atomic_store_n(&JT(jt,systemlock),0,__ATOMIC_RELEASE);
}

// Allow a lock to proceed.  Called by a running thread when it notices the system-lock request
void jtsystemlockaccept(J jt){
 // Decrement the systemlock
 __atomic_fetch_sub(&JT(jt,systemlock),1,__ATOMIC_ACQ_REL);
 // Wait until the break request has been removed
 while(__atomic_load_n(&JT(jt,adbreak)[1],__ATOMIC_ACQUIRE)!=0);
 // Decrement the systemlock
 __atomic_fetch_sub(&JT(jt,systemlock),1,__ATOMIC_ACQ_REL);
 // this thread is free to continue.  No lock request is possible until systemlock has been cleared
}

#if 0
// *********************** task creation ********************************
// Create worker thread n.
I jtthreadcreate(J jt,I n){
 pthread_attr_t attr;  // attributes for the task we will start
 // create thread
 pthread_attr_init(&attr);
 pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);  // default parms, except for DETACHED (which means we will never join() )
 ASSERT(pthread_create(&JTFORTHREAD(jt,n)->pthreadid,&attr,jtthreadmain,JTFORTHREAD(jt,n))==0,EVFACE);  // create the thread, saveits threadid
 R 1;
}

// Processing loop for thread.  Create a wait block for the thread, and wait on it.  Each loop runs one user task
void* jtthreadmain(J jt){
 // get/set stack limits
 if(pthread_attr_getstackaddr(0,(void **)&jt->cstackinit)!=0)R; if(pthread_attr_setstackaddr(0,CSTACKSIZE)!=0)R; 
 // allocate condition & mutex here in a struct & initialize
 WAITBLOK wblok; jt->waitblok=&wblok; pthread_cond_init(&wblok.cond,0); pthread_mutex_init(&wblok.mutex,0);)
 while(1){
  // put pointer to our cond/mutex into commregion so other tasks can see it
  // take our mutex
  // put our thread on the thread queue under lock
  TASKCOMMREGION(jt)[0]=(I)&wblok; pthread_mutex_lock(&wblok.mutex); WRITELOCK(MTHREAD(jt)->tasklock) jt->taskidleq=MTHREAD(jt)->taskidleq; MTHREAD(jt)->taskidleq=THREADID(jt); WRITEUNLOCK(MTHREAD(jt)->tasklock)
  // wait
  // release mutex
  pthread_cond_wait(&wblok.cond,&wblok.mutex); pthread_mutex_unlock(&wblok.mutex);
  // extract task parameters: args & result block
  // initialize the non-parameter part of task block
  // go to RUNNING state, but not if a SYSTEMLOCK is pending
  // run the task, raising & lowering the locale execct
  // put the result into the result block.  If there was an error, use the error code as the result.  But make sure the value is non0 so the pyx doesn't wait forever
  if(unlikely(z==0)){if(unlikely((z=(A)jt->jerr)==0)z=(A)EVSYSTEM;}
  __atomic_store_n(&AAV0(pyx)[0],z,__ATOMIC_RELEASE);
  // unprotect args
  // broadcast to wake up any tasks waiting for the result
  pthread_mutex_lock(&((WAITBLOK*)&AAV0(pyx)[1])->mutex); pthread_cond_broadcast(&((WAITBLOK*)&AAV0(pyx)[1])->cond); pthread_mutex_unlock(&((WAITBLOK*)&AAV0(pyx)[1])->mutex);
  // unprotect pyx
  // go back to non-RUNNING state, but not if SYSTEMLOCK is pending
 }
} 

// execute the user's task.  Result is a pyx.
A jttaskrun(J jt,A a, A w, A v){
 // take a thread to run.  if none, just execute & return
 J exjt=JTFORTHREAD(jt,threadno);
 WAITBLOK *exwblok=TASKCOMMREGION(exjt)[0];  // before we destroy it, save the address of the task's waitblok
 // realize virtual arguments
 // allocate a result block.  Init value, cond, and mutex to idle
 GAT0(pyx,INT,((sizeof(WAITBLOK)+(SZI-1))>>LGSZI)+1,0); ACINITZAP(pyx); AAV0(pyx)[0]=0; // allocate the result pointer (1), and the cond/mutex for the pyx.  ra() the pyx, set to unresolved
 pthread_cond_init(&((WAITBLOK*)&AAV0(pyx)[1])->cond,0); pthread_mutex_init(&((WAITBLOK*)&AAV0(pyx)[1])->mutex);
 // protect the arguments and result
 // initialize the task parameters
 // Grab the thread's mutex and wake it up
 pthread_mutex_lock(exwblok->mutex); pthread_cond_signal(exwblok->cond); pthread_mutex_unlock(exwblok->mutex);
 R pyx;
}
#endif
