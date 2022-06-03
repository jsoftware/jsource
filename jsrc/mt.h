// mt.h: mutex, sync, timing related interfaces
// see mt.c

#if PYXES
struct jtimespec jtmtil(UI ns); //returns a time ns ns in the future
I jtmdif(struct jtimespec when); //returns the time in ns between now and when.  If when is not in the future, the result will be -1
//both of these are implemented in terms of mtclk and use its clock

#if !defined(__APPLE__) && !defined(__linux__)
#include <pthread.h>
typedef pthread_mutex_t jtpthread_mutex_t;
static inline void jtpthread_mutex_init(jtpthread_mutex_t *m,B recursive){
 if(likely(!recursive)){pthread_mutex_init(m,0);}
 else{
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
  pthread_mutex_init(m,&attr);}}
static inline C jtpthread_mutex_lock(J jt,jtpthread_mutex_t *m,I self){
 I4 r=pthread_mutex_lock(m);
 if(likely(r==0))R 0;
 if(r==EDEADLK)R EVCONCURRENCY;
 R EVFACE;}
static inline I jtpthread_mutex_timedlock(J jt,jtpthread_mutex_t *m,UI ns,I self){
#if SY_WIN32
 struct jtimeval now;jgettimeofday(&now,0);
 struct timespec t;
 t.tv_sec=now.tv_sec+ns/1000000000;t.tv_nsec=1000*now.tv_usec+ns%1000000000;if(t.tv_nsec>=1000000000){t.tv_sec++;t.tv_nsec-=1000000000;}
#else
 struct timespec t;clock_gettime(CLOCK_REALTIME,&t);
 t.tv_sec+=ns/1000000000;t.tv_nsec+=ns%1000000000;if(t.tv_nsec>=1000000000){t.tv_sec++;t.tv_nsec-=1000000000;}
#endif
 I4 r=pthread_mutex_timedlock(m,&t);
 if(r==0)R 0;
 if(r==ETIMEDOUT)R -1;
 if(r==EDEADLK)R EVCONCURRENCY;
 R EVFACE;}
static inline I jtpthread_mutex_trylock(jtpthread_mutex_t *m,I self){
 I4 r=pthread_mutex_trylock(m);
 if(!r)R 0;
 if(r==EBUSY)R -1;
 if(r==EAGAIN)R EVLIMIT; //'max recursive locks exceeded'
 if(r==EDEADLK||r==EOWNERDEAD)R EVCONCURRENCY;
 R EVFACE;}
static inline C jtpthread_mutex_unlock(jtpthread_mutex_t *m,I self){
 I4 r=pthread_mutex_unlock(m);
 if(likely(!r))R 0;
 if(r==EPERM)R EVCONCURRENCY;
 R EVFACE;}
#else
typedef struct {
 B recursive;
 I owner; //user-provided; task id
 UI4 v;
 UI4 ct; //for recursive locks
}jtpthread_mutex_t;//todo should split into multiple cache lines?

void jtpthread_mutex_init(jtpthread_mutex_t*,B recursive);
C jtpthread_mutex_lock(J jt,jtpthread_mutex_t *m,I self);
I jtpthread_mutex_timedlock(J jt,jtpthread_mutex_t*,UI ns,I self); //absolute timers suck; correct the interface.  -1=failure; 0=success; positive=error
I jtpthread_mutex_trylock(jtpthread_mutex_t*,I self); //0=success -1=failure positive=error
C jtpthread_mutex_unlock(jtpthread_mutex_t*,I self); //0 or error code
//note: self must be non-zero

#if defined(__linux__)
#include <linux/futex.h>
#include <sys/syscall.h>
//glibc 'syscall': stupid errno
static inline void jfutex_wake1(UI4 *p){
 __asm__ volatile("syscall" :: "a" (SYS_futex), //eax: syscall#
                               "D" (p), //rdi: ptr
                               "S" (FUTEX_WAKE), //rsi: op
                               "d" (1));} //rdx: count
static inline void jfutex_wakea(UI4 *p){
 __asm__ volatile("syscall" :: "a" (SYS_futex), //eax: syscall#
                               "D" (p), //rdi: ptr
                               "S" (FUTEX_WAKE), //rsi: op
                               "d" (0xffffffff));} //rdx: count
static inline int jfutex_wait(UI4 *p,UI4 v){
 register struct timespec *pts asm("r10") = 0;
 int r;__asm__ volatile("syscall" : "=a"(r) //result in rax
                                  : "a" (SYS_futex), //eax: syscall#
                                    "D" (p), //rdi: ptr
                                    "S" (FUTEX_WAIT), //rsi: op
                                    "d" (v), //rdx: espected
                                    "r" (pts)); //r10: timeout (null=no timeout)
 R r;}
static inline int _jfutex_waitn(UI4 *p,UI4 v,UI ns){
 struct timespec ts={.tv_sec=ns/1000000000, .tv_nsec=ns%1000000000};
 register struct timespec *pts asm("r10") = &ts;
 int r;__asm__ volatile("syscall" : "=a"(r) //result in rax
                                  : "a" (SYS_futex), //eax: syscall#
                                    "D" (p), //rdi: ptr
                                    "S" (FUTEX_WAIT), //rsi: op
                                    "d" (v), //rdx: espected
                                    "r" (pts)); //r10: timeout (relative!)
 R r;}
#elif defined(__APPLE__)
// ulock (~futex) junk from xnu.  timeout=0 means wait forever
extern int __ulock_wait(uint32_t operation, void *addr, uint64_t value, uint32_t timeout);             // timeout in us
extern int __ulock_wait2(uint32_t operation, void *addr, uint64_t value, uint64_t timeout, uint64_t value2); // timeout in ns.  only available as of macos 11?
extern int __ulock_wake(uint32_t operation, void *addr, uint64_t wake_value);

// operation bits [7, 0] contain the operation code.
#define UL_COMPARE_AND_WAIT             1
#define UL_UNFAIR_LOCK                  2
#define UL_COMPARE_AND_WAIT_SHARED      3
#define UL_UNFAIR_LOCK64_SHARED         4
#define UL_COMPARE_AND_WAIT64           5
#define UL_COMPARE_AND_WAIT64_SHARED    6

// operation bits [15, 8] contain the flags for __ulock_wake
#define ULF_WAKE_ALL                    0x00000100 //wake all waiting threads (default is to just wake one)
#define ULF_WAKE_THREAD                 0x00000200 //thread id specified (how?) in wake_value
#define ULF_WAKE_ALLOW_NON_OWNER        0x00000400

// operation bits [23, 16] contain the flags for __ulock_wait
#define ULF_WAIT_WORKQ_DATA_CONTENTION  0x00010000 // The waiter is contending on this lock for synchronization around global data.  This causes the workqueue subsystem to not create new threads to offset for waiters on this lock.
#define ULF_WAIT_CANCEL_POINT           0x00020000 // This wait is a cancelation point.  [What does that mean?]
#define ULF_WAIT_ADAPTIVE_SPIN          0x00040000 // Use adaptive spinning when the thread that currently holds the unfair lock is on core.

// operation bits [31, 24] contain the generic flags
#define ULF_NO_ERRNO                    0x01000000 // I think this means return errors as negative codes instead of setting errno?

#define UL_OPCODE_MASK          0x000000FF
#define UL_FLAGS_MASK           0xFFFFFF00
#define ULF_GENERIC_MASK        0xFFFF0000

//positive (or just 1?) result from wait means someone else is waiting on this too?

static inline void jfutex_wake1(UI4 *p){__ulock_wake(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO,p,0);}
static inline void jfutex_wakea(UI4 *p){__ulock_wake(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO|ULF_WAKE_ALL,p,0);}
static inline int jfutex_wait(UI4 *p,UI4 v){R __ulock_wait(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO,p,v,0);}
#if __arm64__
// wait2 takes an ns timeout, but it's only available from macos 11 onward; coincidentally, arm macs only support macos 11+
// so we can count on having this
static inline int _jfutex_waitn(UI4 *p,UI4 v,UI ns){R __ulock_wait2(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO,p,v,ns,0);}
#else
// but for the x86 case, we keep compatibility with older macos.  Revisit in the future
// deal with >32 bits; 2^32us is just a little over an hour; just too close for comfort
static inline int _jfutex_waitn(UI4 *p,UI4 v,UI ns){
 UI us=ns/1000;
 while(us>0xfffffff){
  I4 r=__ulock_wait2(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO,p,v,0xffffffff,0);
  if(r!=-ETIMEDOUT)R r;
  us-=0xffffffff;}
 R __ulock_wait2(UL_COMPARE_AND_WAIT|ULF_NO_ERRNO,p,v,us,0);}
#endif
#elif defined(_WIN32)
// untested windows path; make henry test it when he gets back from vacation
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
static inline int jfutex_wait(UI4 *p,UI4 v){R WaitOnAddress(p,&v,4,INFINITE);} //todo return wrong
static inline int _jfutex_waitn(UI4 *p,UI4 v,UI ns){R WaitOnAddress(p,&v,4,ns/1000000);} //ditto
static inline void jfutex_wake1(UI4 *p){WakeByAddressSingle(p);}
static inline void jfutex_wakea(UI4 *p){WakeByAddressAll(p);}
#endif //_WIN32
#endif //__APPLE__ || __linux__
#endif //PYXES
