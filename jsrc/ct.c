/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */

#include "j.h"

#if ARTIFHIPREC
// w is a block that will become the contents of a box.  Put it inside a hiprec and return the address of the hiprec.
// aflag is the boxing flag of the block the result is going to go into.  w has been prepared for that type
A jtartiffut(J jt,A w,I aflag){A z;
 GAT0(z,BOX,1,0); AFLAG(z)|=BOX; AAV0(z)[0]=w; AT(z)|=HIPREC; if(aflag&BOX){ra(z);}else{ACIPNO(z); ra(w);}  // just one of z/w must be adjusted to the recursion environment
 R z;
}
#endif
#if HIPRECS
// x is a A holding a hiprecs value.  Return its value when it has been resolved
A jthipval(J jt,A w){
 // read the hiprecs value.  Since the creating thread has a release barrier after creation and another after final resolution, we can be sure
 // that if we read nonzero the hiprec has been resolved, even without an acquire barrier
 A res=AAV0(w)[0];  // fetch the possible value
 if(unlikely(res==0)){
  // here the hiprec was unresolved.  Wait for it
  SEGFAULT;  // scaf
 }
 // res now contains the certified value of the hiprec.
 ASSERT(((I)res&-256)!=0,(I)res)   // if error, return the error code
 R res;  // otherwise return the resolved value
}

// take a readlock on *alock.  We come here only if the lock was in use.  The previous value was prev
void readlock(S *alock, S prev){
 // loop until we get the lock
 while(1) {
  // spin until any write request has gone away
  I nspins=5000;  // good upper bound on the amount of time a write could reasonably take, in cycles
  while(prev<0){
   // we are delaying while a writer finishes.  Usually this will be fairly short, as controlled by nspins.  The danger is that the
   // writer will be preempted, leaving us in a tight spin.  If the spin counter goes to 0, we decide this must have happened, and we
   // do a low-power delay for a little while (method TBD)
   if(--nspins==0){nspins=5000; /* scaf delay(5000*100 nsec); */}
   prev=__atomic_load_n(alock,__ATOMIC_ACQUIRE);
  }
  // try to reacquire the lock
  if(__atomic_compare_exchange_n(alock, &prev, prev+1, 0, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))R;  // return if we get the read lock
  //  here somebody else came along while we were requesting.  very rare.  loop back to retry
 }
}

// take a writelock on *alock.  We come here only if the lock was in use.  The previous value was prev
void writelock(S *alock, S prev){
 // loop until we get the lock
 I nspins;
 while(1) {
 // if another writer has requested, they will win.  wait until they finish.  As above, back off if it looks like they were preempted
  nspins=prev&0x7fff?5000+1000:5000;  // max expected writer delay, plus reader delay if there are readers
  while(prev<0){
   if(--nspins==0){nspins=5000; /* scaf delay(5000*100 nsec); */}
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
  prev=__atomic_load_n(alock,__ATOMIC_ACQUIRE);
 }
}


#endif
// Extend a hashtable/data table under lock.  abuf is the pointer to the block to be extended (*abuf will hold the new block address).
// *alock is the lock to use.  We hold a writelock on *alock on entry, but we may relinquish inside this routine.
// On exit we hold the write lock UNLESS there was an error, in which case we return NOT holding the lock (to allow the caller to abort on error)
// ishash is 1 if *abuf is a hashtable.  In that case, fill it with -1.  Otherwise copy the old contents to the beginning of the resized table.
// result is 0 if we hit an error, otherwise the table has been resized, but not necessarily by us & it might not have enough space.
// The tables resized here are allocated with rank 0.  AN()/AS() (if present) gives the current allocation, and AM() gives the number of items actually in use
I jtextendunderlock(J jt, A *abuf, US *alock, I ishash){A z;
 I oldn=AN(*abuf);   // get the previous allocated size
 I t=AT(*abuf);  // get the type of the allocation
 WRITEUNLOCK(*alock);  // relinquish lock while we allocate the new area
 GA0(z,t,2*oldn,AR(*abuf)); ACINITZAP(z); // allocate a new block, bigger than the previous one; make it permanent.  This is the only error exit from this routine
 WRITELOCK(*alock);  // reacquire lock
 A obuf=*abuf;  // current buffer after reacquiring lock
 if(AN(obuf)==oldn){
  // normal case where no one else resized the block before we did.  Transfer the data and counts from the old block to the new
  I itemsize;
  I nvaliditems=AM(obuf);  // remember number of valid items in the old block
  I datasize=allosize(z);  // number of bytes in data area
  I alloatoms=datasize>>bplg(t);   // advance AN to max allocation - leaving no buffer at the end
  // if this allocation is a table, 
  if(AR(z)==2){itemsize=AS(z)[1]=AS(obuf)[1]; AS(z)[0]=alloatoms/itemsize; AN(z)=AS(z)[0]*itemsize;}else{itemsize=1; AN(z)=alloatoms;}
  if(ishash){
   // If the block is a hashtable, it will be rebuilt from scratch and we just initialize it to -1 pointers
   mvc(datasize,voidAV0(z),1,MEMSETFF);  // fill the entire table
   AM(z)=0;  // indicate the whole hash is invalid after resize
  }else{
   MC(voidAV0(z),voidAV0(obuf),itemsize*(nvaliditems<<bplg(t)));  // copy the valid data.  Rest can be left garbage
   AM(z)=nvaliditems;  // transfer the count of allocated atoms, now valid
  }
  // release the old block.  We assume that freeing a block will not permanently block this thread
  *abuf=z; fa(obuf);  // fa, not mf, because the block may be is use elsewhere, e. g. as a backer
 }else{
  // somebody else has already resized the block.  We just back off and hope they resized it enough.
  mf(z);  // discard the block we allocated
 }
 R 1;  // normal return
}


