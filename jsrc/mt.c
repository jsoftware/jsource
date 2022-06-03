// concurrency primitives, including mutexes
// a raft of reasons for this:
// - return EINTR when interrupted
// mutexes:
//  - fast timedlock and recursive mutexes on macos
//  - robust by default
//    - various edge cases like lock on one thread/release on another, or acquire a lock you already hold, are UB in posix!
//  - mutex requisition can be associated with a task, rather than a thread
// condvars:
//  - don't reacquire mutex on wake (_very_ slow)
//  - wake n on linux
// novel primitives, faster than would be possible with pthreads
//  - mutex tokens
//  - queue
//  - pyx

// mutexs are loosely modeled after ulrich drepper 'futexes are tricky'

#include"j.h"

struct jtimespec jtmtil(UI ns){
 struct jtimespec r=jmtclk();
 r.tv_sec+=ns/1000000000;r.tv_nsec+=ns%1000000000;
 if(r.tv_nsec>=1000000000){r.tv_sec++;r.tv_nsec-=1000000000;}
 R r;}
I jtmdif(struct jtimespec w){
 struct jtimespec t=jmtclk();
 if(t.tv_sec>w.tv_sec||t.tv_sec==w.tv_sec&&t.tv_nsec>=w.tv_nsec)R -1;
 R (w.tv_sec-t.tv_sec)*1000000000+w.tv_nsec-t.tv_nsec;}

#if defined(__APPLE__) || defined(__linux__)
enum{FREE=0,LOCK=1,WAIT=2};//values for mutex->v
//todo consider storing owner in the high bits of v.  apple pthreads does this.  But it means we can't use xadd to unlock.  On the other hand, apple is mostly arm now, which doesn't have xadd anyway.
//Also, I just realised you _can_ use xadd to unlock--subtract the top bits at the same time as the bottom one--it just adds a weird state where the low bit is 1, but the high bits don't denote any task--but that's ok

// can't use UL_UNFAIR_LOCK, as our ids are task ids, not thread ids/mach ports/whatever

// there is a flaw.  If t0 holds lock, t1 attempts to acquire it; when it eventually does, it will leave WAIT in v instead of LOCK, 

// todo what is ULF_WAIT_CANCEL_POINT?

void jtpthread_mutex_init(jtpthread_mutex_t *m,B recursive){*m=(jtpthread_mutex_t){.recursive=recursive};}
C jtpthread_mutex_lock(J jt,jtpthread_mutex_t *m,I self){
 if(uncommon(m->owner==self)){if(unlikely(!m->recursive))R EVCONCURRENCY; m->ct++;R 0;}
 UI4 e;if(likely((!(e=lda(&m->v)))&&((e=FREE),casa(&m->v,&e,LOCK))))goto success; //fast path.  test-and-test-and-set is from glibc, mildly optimises the case when many threads swarm a locked mutex.  Not sure if this is for the best, but after waffling for a bit I think it is
 if(e!=WAIT)e=xchga(&m->v,WAIT); //penalise the multi-waiters case, since it's slower anyway
 while(e!=FREE){
#if __linux__
  I i=_jfutex_waitn(&m->v,WAIT,(UI)-1);
  //bug? futex wait doesn't get interrupted by signals on linux if timeout is null
#else
  I i=jfutex_wait(&m->v,WAIT);
#endif
  if(uncommon(i<0)){
   if(i==-EINTR){if(JT(jt,adbreakr)[0])R EVATTN;}
   else if(i==-EOWNERDEAD)R EVCONCURRENCY;
   else if(i==-ENOMEM)R EVWSFULL;//lol
   else R EVFACE;}
  e=xchga(&m->v,WAIT);} //exit when e==FREE; i.e., _we_ successfully installed WAIT in place of FREE
success:m->ct+=m->recursive;m->owner=self;  R 0;}
I jtpthread_mutex_timedlock(J jt,jtpthread_mutex_t *m,UI ns,I self){
 if(uncommon(m->owner==self)){if(unlikely(!m->recursive))R EVCONCURRENCY; m->ct++;R 0;}
 UI4 e=0;if((e=lda(&m->v))!=FREE&&((e=FREE),casa(&m->v,&e,LOCK)))goto success;
 struct timespec tgt=jtmtil(ns);
 if(common(e!=WAIT)){e=xchga(&m->v,WAIT);if(e==FREE)goto success;} //penalise the multi-waiters case, since it's slower anyway
 while(1){
  I i=_jfutex_waitn(&m->v,WAIT,ns);
  if(uncommon(i==-ETIMEDOUT)); //don't penalise this case too harshly
  else if(unlikely(i<0)){
   if(i==-EINTR){if(JT(jt,adbreakr)[0])R EVATTN;}
   else if(i==-EOWNERDEAD)R EVCONCURRENCY;
   else if(i==-ENOMEM)R EVWSFULL;
   else R EVFACE;}
  e=xchga(&m->v,WAIT);
  if(e==FREE)goto success; //exit when e==FREE; i.e., _we_ successfully installed WAIT in place of FREE
  if(i==-ETIMEDOUT)R -1; //if the kernel says we timed out, trust it rather than doing another syscall to check the time
  if(-1==(ns=jtmdif(tgt)))R -1;} //update delta, abort if timed out
success:m->ct+=m->recursive;m->owner=self; R 0;}
I jtpthread_mutex_trylock(jtpthread_mutex_t *m,I self){
 if(uncommon(m->recursive)&&m->owner){if(m->owner!=self)R -1; m->ct++;R 0;}
 if(m->owner==self)R EVCONCURRENCY;
 if(casa(&m->v,&(UI4){FREE},LOCK)){m->ct+=m->recursive;m->owner=self;R 0;}   R -1;}
C jtpthread_mutex_unlock(jtpthread_mutex_t *m,I self){
 if(unlikely(m->owner!=self))R EVCONCURRENCY;
 if(uncommon(m->recursive)){if(--m->ct)R 0;} //need to be released more times on this thread
 m->owner=0;
 if(!casa(&m->v,&(UI4){LOCK},FREE)){sta(&m->v,FREE);jfutex_wake1(&m->v);}
 //below is what drepper does; I think the above is always faster, but it should definitely be faster without xadd
 //agner sez lock xadd has one cycle better latency vs lock cmpxchg on intel ... ??
 //if(adda(&m->v,-1)){sta(&m->v,FREE);jfutex_wake1(&m->v);}
 R 0;}
#endif //__APPLE__
