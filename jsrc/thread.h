#if defined(__APPLE__)

#include <unistd.h>
#include <pthread.h>
#include <mach/thread_act.h>
#include <mach/thread_policy.h>

// These two functions are declared in mach/thread_policy.h, but are commented out.
#if 0
kern_return_t	thread_policy_set(
					thread_t			thread,
					thread_policy_flavor_t		flavor,
					thread_policy_t			policy_info,
					mach_msg_type_number_t		count);

kern_return_t	thread_policy_get(
					thread_t			thread,
					thread_policy_flavor_t		flavor,
					thread_policy_t			policy_info,
					mach_msg_type_number_t		*count,
					boolean_t			*get_default);
#endif

#define CPU_SETSIZE 64

typedef struct cpu_set {
  uint64_t    count;
} cpu_set_t;

static inline void CPU_ZERO(cpu_set_t *cs) { cs->count = 0; }

static inline void CPU_SET(int num, cpu_set_t *cs) { cs->count |= (1 << num); }

static inline int CPU_ISSET(int num, cpu_set_t *cs) { return (cs->count & (1 << num)); }

int pthread_getaffinity_np(pthread_t thread, size_t cpu_size, cpu_set_t *cpu_set);
int pthread_setaffinity_np(pthread_t thread, size_t cpu_size, cpu_set_t *cpu_set);

#endif
