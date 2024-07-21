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

// todo better error-checking.  In particular:
// - Token should be a task id, not a thread it
// - Error for a task to exit without releasing all its mutexes (how should this be reported to openers of its pyx if the task was in error?)
// - Attempt to detect deadlock, perhaps in a debug mode of some sort (cf freebsd kernel WITNESS)

#include"j.h"
#if PYXES
#define YIELD sched_yield();  // if we are spinning on other threads, give them a chance to run in case they might be on this core
#else
#define YIELD ;   // if no other processes, no reason to delay
#endif

// timing shenanigans
struct jtimespec jtmtil(UI ns){ //returns the time ns ns in the future
 struct jtimespec r=jmtclk();
 r.tv_sec+=ns/1000000000ull;r.tv_nsec+=ns%1000000000ull;
 if(r.tv_nsec>=1000000000ll){r.tv_sec++;r.tv_nsec-=1000000000ll;}
 R r;}
I jtmdif(struct jtimespec w){ //returns the time in ns between the current time and w, or -1 if it is not in the future
 struct jtimespec t=jmtclk();
 if(t.tv_sec>w.tv_sec||t.tv_sec==w.tv_sec&&t.tv_nsec>=w.tv_nsec)R -1;
 R (w.tv_sec-t.tv_sec)*1000000000ull+w.tv_nsec-t.tv_nsec;}
struct jtimespec jtmftil(UI ns){
 struct jtimespec r=jmtfclk();
 r.tv_sec+=ns/1000000000ull;r.tv_nsec+=ns%1000000000ull;
 if(r.tv_nsec>=1000000000ll){r.tv_sec++;r.tv_nsec-=1000000000ll;}
 R r;}
I jtmfdif(struct jtimespec w){ //returns the time in ns between the current time and w, or -1 if it is not in the future
 struct jtimespec t=jmtfclk();
 if(t.tv_sec>w.tv_sec||t.tv_sec==w.tv_sec&&t.tv_nsec>=w.tv_nsec)R -1;
 R (w.tv_sec-t.tv_sec)*1000000000ull+w.tv_nsec-t.tv_nsec;}

#if PYXES
// implement jfutex_wait _waitn _wake1 _wakea; synchronisation primitives
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
 if(r==-EINTR||r==-EFAULT)R 0; //manifest EINTR as a spurious wake, triggering a look at adbreak, instead of returning EVATTN.  The caller will have to look at adbreak anyway, since the signal might have come right after waking up, also because attention interrupt might be disabled anyway...
                               //(also, there is a race, in the case when ^C is pressed right _before_ going to sleep.  Fixable; annoying; but the window is small and the user can just press ^C again so low-priority)
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
void jfutex_wake1(UI4 *p){syscall(SYS_futex,p,FUTEX_WAKE_PRIVATE,1);} //_PRIVATE means the address is not shared between multiple processes
void jfutex_waken(UI4 *p,UI4 n){syscall(SYS_futex,p,FUTEX_WAKE_PRIVATE,n);}
void jfutex_wakea(UI4 *p){syscall(SYS_futex,p,FUTEX_WAKE_PRIVATE,0x7fffffff);}
C jfutex_wait(UI4 *p,UI4 v){
 struct timespec *pts = 0;
 int r=syscall(SYS_futex,p,FUTEX_WAIT_PRIVATE,v,pts);
 if(r>=0)R 0;
 if(errno==EAGAIN||errno==EINTR)R 0;
 R EVFACE;}
I jfutex_waitn(UI4 *p,UI4 v,UI ns){
 struct timespec ts={.tv_sec=ns/1000000000, .tv_nsec=ns%1000000000}; //ts is relative (except for the cases in which it's absolute, but this is not one such)
 int r=syscall(SYS_futex,p,FUTEX_WAIT_PRIVATE,v,&ts);
 if(r>=0)R 0;
 if(errno==ETIMEDOUT)R -1;
 if(errno==EAGAIN||errno==EINTR)R 0;
 R EVFACE;}
#elif defined(_WIN32)
// defined in cd.c to avoid name collisions between j.h and windows.h
#elif defined(__FreeBSD__)
void jfutex_wake1(UI4 *p){_umtx_op(p,UMTX_OP_WAKE_PRIVATE,1,0,0);}
void jfutex_waken(UI4 *p,UI4 n){_umtx_op(p,UMTX_OP_WAKE_PRIVATE,n,0,0);}
void jfutex_wakea(UI4 *p){_umtx_op(p,UMTX_OP_WAKE_PRIVATE,INT_MAX,0,0);}
C jfutex_wait(UI4 *p,UI4 v){
 int r=_umtx_op(p,UMTX_OP_WAIT_UINT_PRIVATE,v,0,0);
 if (r==0)R 0;
 if (errno==EINTR)R 0;
 R EVFACE;}
I jfutex_waitn(UI4 *p,UI4 v,UI ns){
 struct timespec ts={.tv_sec=ns/1000000000, .tv_nsec=ns%1000000000};
 int r=_umtx_op(p,UMTX_OP_WAIT_UINT_PRIVATE,v,(void*)sizeof(struct timespec),&ts);
 if (r==0)R 0;
 if (errno==ETIMEDOUT)R -1;
 if (errno==EINTR)R 0;
 R EVFACE;}
#elif defined(__OpenBSD__)
// see comment in mt.h
void jfutex_wake1(UI4 *p){futex(p,FUTEX_WAKE_PRIVATE,1,0,0);}
void jfutex_waken(UI4 *p,UI4 n){futex(p,FUTEX_WAKE_PRIVATE,n,0,0);}
void jfutex_wakea(UI4 *p){futex(p,FUTEX_WAKE_PRIVATE,0x7fffffff,0,0);}
C jfutex_wait(UI4 *p,UI4 v){
 int r=futex(p,FUTEX_WAIT_PRIVATE,v,0,0);
 if(r>=0)R 0;
 if(errno==EAGAIN||errno==EINTR||errno==ECANCELED)R 0;
 R EVFACE;}
I jfutex_waitn(UI4 *p,UI4 v,UI ns){
 struct timespec ts={.tv_sec=ns/1000000000, .tv_nsec=ns%1000000000};
 int r=futex(p,FUTEX_WAIT_PRIVATE,v,&ts,0);
 if(r>=0)R 0;
 if(errno==ETIMEDOUT)R -1;
 if(errno==EAGAIN||errno==EINTR||errno==ECANCELED)R 0;
 R EVFACE;}
#endif

#if !defined(__linux__) && !defined(__FreeBSD__) && !defined(__OpenBSD__) //no native waken on other platforms
void jfutex_waken(UI4 *p,UI4 n){jfutex_wakea(p);} // TUNE: should DO(n,jfutex_wake1(p)) depending on n and the #threads waiting on p
#endif

//values for mutex->v.  The upper 16 bits are a wait counter; the state is the low 16 bits, as follows
//todo consider storing owner in the high bits of v
enum{
 FREE=0,  //a free mutex
 LOCK=1,  //a mutex which is held, and which no one is waiting on; uncontended mutexes will just move between FREE and LOCK
 WAIT=3}; //a mutex which is held, and which _may_ have threads waiting on it.  We can OR WAIT into a state to produce WAIT

// macos: can't use UL_UNFAIR_LOCK, as our ids are task ids, not thread ids/mach ports/whatever

// there is a flaw.  If t0 holds lock, t1 attempts to acquire it; when it eventually does, it will leave WAIT in v instead of LOCK, 

void jtpthread_mutex_init(jtpthread_mutex_t *m,B recursive){*m=(jtpthread_mutex_t){.recursive=recursive};}

C jtpthread_mutex_lock(J jt,jtpthread_mutex_t *m,I self){ //lock m; self is thread# requesting the lock
 I r;  // internal return code in case of error
 if(unlikely(!casa((US*)&m->v,&(US){FREE},LOCK))){    //fast and common path: attempt to install LOCK in place of FREE; if so, we have acquired the lock
  // The lock was in use.  We will (almost always) have to wait for it
  // it is barely possible that the lock had its state set to LOCK but the owner has not yet been filled in.  That is possible only if the lock
  // is owned by another thread, so it is safe even then for us to check whether this thread is the owner
  if(uncommon(m->owner==self)){if(unlikely(!m->recursive))R EVCONCURRENCY; m->ct++;R 0;} //handle deadlock and recursive cases
  sta(&jt->futexwt,&m->v); //ensure other threads know how to wake us up for systemlock
  // It is always safe to move the state of a lock to WAIT using xchg.  There are 2 cases:
  // (1) if the previous state was FREE, we now own the lock after the xchg...
  while(xchga((US*)&m->v,WAIT)!=FREE){ //exit when _we_ successfully installed WAIT in place of FREE
   // ... (2) the lock had an owner.  By moving state to WAIT, we guaranteed that the owner will wake us on freeing the lock
   //note that we must install WAIT in m->v even in the case when no one else is actually waiting, because we can't know if somebody else is waiting
   //ulrich drepper 'futexes are tricky' explains the issue with storing a waiter count in the value
   //a couple of alternatives suggest themselves: store up to k waiters (FREE/LOCK/WAIT is really 0/1/n; we could do eg 0/1/2/3/n); store the waiter count somehow outside of the value
   // Before waiting, handle system events if present
   UI4 waitval=lda(&m->v); C breakb;  // get the serial number before we check.  Must be atomic; this is supposed to synchronise with writes to the same location via futexwt by wakeall
   if(unlikely(BETWEENC(lda(&JT(jt,systemlock)),1,2))){jtsystemlockaccept(jt,LOCKALL);continue;}  // if system lock requested, accept it.  Then retry; a lot can happen during a systemlock, and we probably need to resample waitval anyway
   // the user may be requesting a BREAK interrupt for deadlock or other slow execution
   if(unlikely((breakb=lda(&JT(jt,adbreak)[0])))!=0){r=breakb==1?EVATTN:EVBREAK;goto fail;} // JBREAK: give up on the pyx and exit
   // Now wait for a change.  The futex_wait is atomic, and will wait only if the state is WAIT.  In that case,
   // the holder is guaranteed to perform a wake after freeing the lock.  If the state is not WAIT, something has happened already and we inspect it forthwith
#if __linux__
   I i=jfutex_waitn(&m->v,waitval|WAIT,(UI)-1);
   //kernel bug? futex wait doesn't get interrupted by signals on linux if timeout is null
#else
   I i=jfutex_wait(&m->v,waitval|WAIT);  // if we are out of WAIT state, or the serial number changed since we check for system lock, don't wait at all
#endif
   if(unlikely(i>0)){r=EVFACE; goto fail;} //handle error (unaligned unmapped interrupted...)
  }
  // come out of loop when we have the lock
  CLRFUTEXWT;
 }
 m->ct+=m->recursive;m->owner=self;R 0;  // install ownership info, good return
fail:CLRFUTEXWT; R r;}  // error return, with our internal errorcode


// return positive error code, 0 if got lock, -1 if lock timed out
I jtpthread_mutex_timedlock(J jt,jtpthread_mutex_t *m,UI ns,I self){ //lock m, with a timeout of ns ns.  Largely the same as lock
 I r;  // internal return code in case of error
 if(unlikely(!casa((US*)&m->v,&(US){FREE},LOCK))){    //fast and common path: attempt to install LOCK in place of FREE; if so, we have acquired the lock
  struct jtimespec tgt=jtmtil(ns);
  if(uncommon(m->owner==self)){if(unlikely(!m->recursive))R EVCONCURRENCY; m->ct++;R 0;} //handle deadlock and recursive cases
  sta(&jt->futexwt,&m->v); //ensure other threads know how to wake us up for systemlock
  while(xchga((US*)&m->v,WAIT)!=FREE){ //exit when _we_ successfully installed WAIT in place of FREE
   UI4 waitval=lda(&m->v); C breakb;  // get the serial number before we check.  Must be atomic; this is supposed to synchronise with writes to the same location via futexwt by wakeall
   if(unlikely(BETWEENC(lda(&JT(jt,systemlock)),1,2))){jtsystemlockaccept(jt,LOCKALL);goto retime;}
   if(unlikely((breakb=lda(&JT(jt,adbreak)[0])))!=0){r=breakb==1?EVATTN:EVBREAK;goto fail;} // JBREAK: give up on the pyx and exit
   I i=jfutex_waitn(&m->v,waitval|WAIT,ns);
   if(unlikely(i>0)){r=EVFACE; goto fail;} //handle error (unaligned unmapped interrupted...)
   if(i==-1){r=-1;goto fail;} //if the kernel says we timed out, trust it rather than doing another syscall to check the time
retime:
   if(-1ull==(ns=jtmdif(tgt))){r=-1;goto fail;} //update delta, abort if timed out
  }
  CLRFUTEXWT;  // remove wakeup to this thread
 }
 m->ct+=m->recursive;m->owner=self;R 0;  // install ownership info, good return
fail:CLRFUTEXWT; R r;}  // error return, with our internal errorcode or -1 if timeout

I jtpthread_mutex_trylock(jtpthread_mutex_t *m,I self){ //attempt to acquire m
 if(casa((US*)&m->v,&(US){FREE},LOCK)){m->ct+=m->recursive;m->owner=self;R 0;} //fastpath: attempt to acquire the lock; if free, take it
 // the lock was held.  owner might not be set yet, if the lock is held by another thread
 if(unlikely(m->owner==self)){if(!m->recursive)R EVCONCURRENCY; ++m->ct; R 0;}  // if we hold the lock already, that's error if nonrecursive lock; incr recursion count otherwise
 R -1;}   // if lock held elsewhere, return busy

C jtpthread_mutex_unlock(jtpthread_mutex_t *m,I self){ //release m
 if(unlikely(m->owner!=self))R EVCONCURRENCY; //error to release a lock you don't hold
 if(uncommon(m->recursive)){if(--m->ct)R 0;} //need to be released more times on this thread, so nothing more to do
 m->owner=0;  // clear owner before releasing the lock
 if(unlikely(xchga((US*)&m->v,FREE)==WAIT))jfutex_wake1(&m->v);  // move to FREE state; if state was WAIT, wake up a waiter
 R 0;}

// misc: sleep
// attempt to sleep for ns ns, with proper handling of systemlock and jbreak.  Returns error code
C jtjsleep(J jt,UI ns){
 C r=0,breakb;
 struct jtimespec tgt=jtmtil(ns);  // target time for end of wait
 UI4 ftx=0;
 sta(&jt->futexwt,&ftx);
 while(1){
  UI4 waitval=lda(&ftx);
  if(unlikely(BETWEENC(lda(&JT(jt,systemlock)),1,2))){jtsystemlockaccept(jt,LOCKALL);goto retime;} //systemlock can take a long time
  if(unlikely(lda(&JT(jt,adbreak)[0])>1)){r=EVBREAK;break;}   // ignore ATTN, but exit on BREAK
  I i=jfutex_waitn(&ftx,waitval|1,MIN(ns,1000000000));  // the 1 is PYXWAIT defined in ct.c - we wait for the state to move off WAIT state
   // ATTN doesn't wake us, so we have to sleep at most 1s at a time
  if(unlikely(i>0)){r=EVFACE;break;}
  if(i==-1){r=0;break;} //timed out
retime:
  if(-1ull==(ns=jtmdif(tgt))){r=0;break;}}  // recalculate time-to-target
 CLRFUTEXWT;
 R r;}
#endif //PYXES
