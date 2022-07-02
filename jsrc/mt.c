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

// timing shenanigans
struct jtimespec jtmtil(UI ns){
 struct jtimespec r=jmtclk();
 r.tv_sec+=ns/1000000000ull;r.tv_nsec+=ns%1000000000ull;
 if(r.tv_nsec>=1000000000ll){r.tv_sec++;r.tv_nsec-=1000000000ll;}
 R r;}
I jtmdif(struct jtimespec w){
 struct jtimespec t=jmtclk();
 if(t.tv_sec>w.tv_sec||t.tv_sec==w.tv_sec&&t.tv_nsec>=w.tv_nsec)R -1;
 R (w.tv_sec-t.tv_sec)*1000000000ull+w.tv_nsec-t.tv_nsec;}

#if PYXES
#if defined(__APPLE__)
void jfutex_wake1(UI4 *p){__ulock_wake(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO,p,0);}
void jfutex_wakea(UI4 *p){__ulock_wake(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO|ULF_WAKE_ALL,p,0);}
C jfutex_wait(UI4 *p,UI4 v){
 I r=__ulock_wait(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO,p,v,0);
 if(r>=0)R 0;
 if(r==-EINTR||r==-EFAULT)R 0; //EFAULT means the address needed to be paged in, not that it wasn't mapped?
 R EVFACE;} //should never happen?
#if defined(__arm64__)||defined(__aarch64__)
// wait2 takes an ns timeout, but it's only available from macos 11 onward; coincidentally, arm macs only support macos 11+
// so we can count on having this
I jfutex_waitn(UI4 *p,UI4 v,UI ns){
 I r=__ulock_wait2(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO,p,v,ns,0);
 if(r>=0)R 0;
 if(r==-ETIMEDOUT)R -1;
 if(r==-EINTR||r==-EFAULT)R 0;
 if(r==-ENOMEM)R EVWSFULL;//lol
 R EVFACE;}
#else
// but for the x86 case, we keep compatibility with older macos.  Revisit in the future
// deal with >32 bits; 2^32us is just a little over an hour; just too close for comfort
I jfutex_waitn(UI4 *p,UI4 v,UI ns){I r;
 UI us=ns/1000;
 while(us>0xfffffff){
  r=__ulock_wait(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO,p,v,0xffffffff);
  if(r!=-ETIMEDOUT)goto out;
  us-=0xffffffff;}
 r=__ulock_wait(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO,p,v,us);
out:
 if(r>=0)R 0;
 if(r==-ETIMEDOUT)R -1;
 if(r==-EINTR||r==-EFAULT)R 0;
 if(r==-ENOMEM)R EVWSFULL;
 R EVFACE;}
#endif
#elif defined(__linux__)
#include<unistd.h>
#if defined(__NR_futex) && !defined(SYS_futex) //android seemingly defines the former, but not the latter
#define SYS_futex __NR_futex
#endif
void jfutex_wake1(UI4 *p){syscall(SYS_futex,p,FUTEX_WAKE_PRIVATE,1);}
void jfutex_wakea(UI4 *p){syscall(SYS_futex,p,FUTEX_WAKE_PRIVATE,0xffffffff);}
C jfutex_wait(UI4 *p,UI4 v){
 struct timespec *pts = 0;
 int r=syscall(SYS_futex,p,FUTEX_WAIT_PRIVATE,v,pts);
 if(r>=0)R 0;
 if(errno==EAGAIN||errno==EINTR)R 0;
 R EVFACE;}
I jfutex_waitn(UI4 *p,UI4 v,UI ns){
 struct timespec ts={.tv_sec=ns/1000000000, .tv_nsec=ns%1000000000};
 int r=syscall(SYS_futex,p,FUTEX_WAIT_PRIVATE,v,&ts);
 if(r>=0)R 0;
 if(errno==ETIMEDOUT)R -1;
 if(errno==EAGAIN||errno==EINTR)R 0;
 R EVFACE;}
#elif defined(_WIN32)
// defined in cd.c to avoid name collisions between j.h and windows.h
#endif

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
 I r;
 sta(&jt->futexwt,&m->v);
 while(e!=FREE){
  S attn=lda((S*)&JT(jt,adbreakr)[0]);
  if(attn>>8)jtsystemlockaccept(jt,LOCKPRISYM+LOCKPRIPATH+LOCKPRIDEBUG);
  if(attn&0xff){r=attn&0xff;goto fail;}
#if __linux__
  I i=jfutex_waitn(&m->v,WAIT,(UI)-1);
  //bug? futex wait doesn't get interrupted by signals on linux if timeout is null
#else
  I i=jfutex_wait(&m->v,WAIT);
#endif
  if(i>0){r=i;goto fail;}
  e=xchga(&m->v,WAIT);} //exit when e==FREE; i.e., _we_ successfully installed WAIT in place of FREE
success:sta(&jt->futexwt,0);m->ct+=m->recursive;m->owner=self;R 0;
fail:sta(&jt->futexwt,0);R r;}
I jtpthread_mutex_timedlock(J jt,jtpthread_mutex_t *m,UI ns,I self){
 if(uncommon(m->owner==self)){if(unlikely(!m->recursive))R EVCONCURRENCY; m->ct++;R 0;}
 UI4 e=0;if((e=lda(&m->v))!=FREE&&((e=FREE),casa(&m->v,&e,LOCK)))goto success;
 struct jtimespec tgt=jtmtil(ns);
 if(common(e!=WAIT)){e=xchga(&m->v,WAIT);if(e==FREE)goto success;} //penalise the multi-waiters case, since it's slower anyway
 I r;
 sta(&jt->futexwt,&m->v);
 while(1){
  S attn=lda((S*)&JT(jt,adbreakr)[0]);
  if(attn>>8)jtsystemlockaccept(jt,LOCKPRISYM+LOCKPRIPATH+LOCKPRIDEBUG);
  if(attn&0xff){r=attn&0xff;goto fail;}
  I i=jfutex_waitn(&m->v,WAIT,ns);
  if(unlikely(i>0)){r=i;goto fail;}
  e=xchga(&m->v,WAIT);
  if(e==FREE)goto success; //exit when e==FREE; i.e., _we_ successfully installed WAIT in place of FREE
  if(i==-1){r=-1;goto fail;} //if the kernel says we timed out, trust it rather than doing another syscall to check the time
  if(-1ull==(ns=jtmdif(tgt))){r=-1;goto fail;}} //update delta, abort if timed out
success:sta(&jt->futexwt,0);m->ct+=m->recursive;m->owner=self; R 0;
fail:sta(&jt->futexwt,0);R r;}
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
 //(probably that's only in the uncontended case)
 //if(adda(&m->v,-1)){sta(&m->v,FREE);jfutex_wake1(&m->v);}
 R 0;}
#endif //PYXES
