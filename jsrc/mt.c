// jtpthread_mutex*: mutex implementation for macos/ios/..
// loosly modeled after apple pthreads version 486.100.11 and ulrich drepper 'futexes are tricky', plus recursive mutexes
// needed for T. because the mainline version does not support pthread_mutex_timedlock; glibc/pthreads4w are ok
// _does not_ support condition variables (yet); so, use jtpthread_mutex_t if you need timedwait, but pthread_mutex_t if you need condition variables
// _not_ signal-safe

#include"j.h"

#ifdef __APPLE__
enum{FREE=0,LOCK=1,WAIT=2};//values for mutex->v
//todo consider storing owner in the high bits of v.  apple pthreads does this.  But it means we can't use xadd to unlock.  On the other hand, apple is mostly arm now, which doesn't have xadd anyway.
//Also, I just realised you _can_ use xadd to unlock--subtract the top bits at the same time as the bottom one--it just adds a weird state where the low bit is 1, but the high bits don't denote any task--but that's ok

// can't use UL_UNFAIR_LOCK, as our ids are task ids, not thread ids/mach ports/whatever

// there is a flaw.  If t0 holds lock, t1 attempts to acquire it; when it eventually does, it will leave WAIT in v instead of LOCK, 

// todo figure out ULF_WAIT_CANCEL_POINT (I think it allows implementing the desired behaviour for EVATTN)

void jtpthread_mutex_init(jtpthread_mutex_t *m,B recursive){*m=(jtpthread_mutex_t){.recursive=recursive};}
C jtpthread_mutex_lock(J jt,jtpthread_mutex_t *m,I self){
 if(uncommon(m->owner==self)){if(unlikely(!m->recursive))R EVCONCURRENCY; m->ct++;R 0;}
 UI4 e=0;if(!(e=lda(&m->v))&&((e=FREE),casa(&m->v,&e,LOCK))){m->ct+=m->recursive;m->owner=self;R 0;} //success
 if(common(e!=WAIT))e=xchga(&m->v,WAIT); //penalise the multi-waiters case, since it's slower anyway
 while(e!=FREE){
  I i=__ulock_wait(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO,&m->v,WAIT,0);
  if(unlikely(i<0)){
   if(i==-EINTR){if(JT(jt,adbreakr)[0])R EVATTN;}
   else if(i==-EOWNERDEAD)R EVCONCURRENCY;
   else if(i==-ENOMEM)R EVWSFULL;//lol
   else R EVFACE;}
  e=xchga(&m->v,WAIT);} //exit when e==FREE; i.e., _we_ successfully installed WAIT in place of FREE
 m->ct+=m->recursive;m->owner=self;  R 0;}
I jtpthread_mutex_timedlock(J jt,jtpthread_mutex_t *m,UI ns,I self){
 if(uncommon(m->owner==self)){if(unlikely(!m->recursive))R EVCONCURRENCY; m->ct++;R 0;}
 UI4 e=0;if((e=lda(&m->v))!=FREE&&((e=FREE),casa(&m->v,&e,LOCK))){m->ct+=m->recursive;m->owner=self;R 0;} //success
 struct timespec tgt,now;if(clock_gettime(CLOCK_MONOTONIC,&now))R EVFACE;
 tgt.tv_sec=now.tv_sec+ns/1000000000;tgt.tv_nsec=now.tv_nsec+ns%1000000000;if(tgt.tv_nsec>=1000000000){tgt.tv_nsec-=1000000000;tgt.tv_sec++;};
 if(common(e!=WAIT)){e=xchga(&m->v,WAIT);if(e==FREE)goto success;} //penalise the multi-waiters case, since it's slower anyway
 while(1){
#if __arm64__
  // wait2 takes an ns timeout, but it's only available from macos 11 onward; coincidentally, arm macs only support macos 11+
  // so we can count on having this
  I i=__ulock_wait2(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO,&m->v,WAIT,ns,0);
#else
  // but for the x86 case, we keep compatibility
  I i=__ulock_wait(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO,&m->v,WAIT,ns/1000);
#endif
  if(uncommon(i==-ETIMEDOUT)); //don't penalise this case too harshly
  else if(unlikely(i<0)){
   if(i==-EINTR){if(JT(jt,adbreakr)[0])R EVATTN;}
   else if(i==-EOWNERDEAD)R EVCONCURRENCY;
   else if(i==-ENOMEM)R EVWSFULL;
   else if(i==-ETIMEDOUT);
   else R EVFACE;}
  e=xchga(&m->v,WAIT);
  if(e==FREE)goto success; //exit when e==FREE; i.e., _we_ successfully installed WAIT in place of FREE
  if(i==-ETIMEDOUT)R -1; //if the kernel says we timed out, trust it rather than doing another syscall to check the time
  clock_gettime(CLOCK_MONOTONIC,&now);
  if(now.tv_sec>=tgt.tv_sec || now.tv_sec==tgt.tv_sec&&now.tv_nsec>=tgt.tv_nsec)R -1;//timed out
  ns=1000000000*(tgt.tv_sec-now.tv_sec);
  if(now.tv_nsec<=tgt.tv_nsec)ns+=tgt.tv_nsec-now.tv_nsec;
  else ns+=1000000000-(now.tv_nsec-tgt.tv_nsec);}
success:m->ct+=m->recursive;m->owner=self; R 0;}
I jtpthread_mutex_trylock(jtpthread_mutex_t *m,I self){
 if(uncommon(m->recursive)&&m->owner){if(m->owner!=self)R -1; m->ct++;R 0;}
 if(m->owner==self)R EVCONCURRENCY;
 if(casa(&m->v,&(UI4){FREE},LOCK)){m->ct+=m->recursive;m->owner=self;R 0;}   R -1;}
C jtpthread_mutex_unlock(jtpthread_mutex_t *m,I self){
 if(unlikely(m->owner!=self))R EVCONCURRENCY;
 if(uncommon(m->recursive)){if(--m->ct)R 0;} //need to be released more times on this thread
 m->owner=0;
 if(!casa(&m->v,&(UI4){LOCK},FREE)){sta(&m->v,FREE);__ulock_wake(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO,&m->v,0);}
 //below is what drepper does; I think the above is always faster, but it should definitely be faster without xadd
 //agner sez lock xadd has one cycle better latency vs lock cmpxchg on intel ... ??
 //if(adda(&m->v,-1)){sta(&m->v,FREE);__ulock_wake(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO,&m->v,0);}
 R 0;}
#endif //__APPLE__
