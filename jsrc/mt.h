// mt.h: mutex, sync, timing related interfaces
// see mt.c

#if PYXES
struct jtimespec jtmtil(UI ns); //returns a time ns ns in the future
I jtmdif(struct jtimespec when); //returns the time in ns between now and when.  If when is not in the future, the result will be -1
//both of these are implemented in terms of mtclk and use its clock

__attribute__((cold)) C jfutex_wait(UI4 *p,UI4 v); //atomically, compare v to *p and go to sleep if they are equal.  Return error code
__attribute__((cold)) I jfutex_waitn(UI4 *p,UI4 v,UI ns); //ditto, but wake up after at most ns ns.  Result -1 means timeout definitely exceeded; other result is an error code
__attribute__((cold)) void jfutex_wake1(UI4 *p); //wake 1 thread waiting on p
__attribute__((cold)) void jfutex_wakea(UI4 *p); //wake all threads waiting on p

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

#if defined(__linux__)&&!defined(ANDROID)
#include <linux/futex.h>
#include <sys/syscall.h>
#elif defined(ANDORID)
#error no futex support for your platform
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

#elif defined(_WIN32)
// untested windows path; make henry test it when he gets back from vacation
// don't pollute everybody with windows.h.  win api is fairly basic anyway, so there is not much to take advantage of
#else
#error no futex support for your platform
#endif //_WIN32
#endif //PYXES
