#if defined(__APPLE__)

// kern_return_t thread_policy_get(thread_inspect_t thread, thread_policy_flavor_t flavor, thread_policy_t policy_info, mach_msg_type_number_t *policy_infoCnt, boolean_t *get_default);
// kern_return_t thread_policy_set(thread_act_t     thread, thread_policy_flavor_t flavor, thread_policy_t policy_info, mach_msg_type_number_t policy_infoCnt);

#include "thread.h"

int pthread_getaffinity_np(pthread_t thread, size_t cpu_size, cpu_set_t *cpu_set)
{
  thread_port_t mach_thread = pthread_mach_thread_np(thread);
  thread_affinity_policy_data_t policy = { 0 };
  mach_msg_type_number_t infoCnt = THREAD_AFFINITY_POLICY_COUNT;
  boolean_t get_default = 1;
  if (KERN_SUCCESS == thread_policy_get(mach_thread, THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, &infoCnt, &get_default)) {
    cpu_set->count = (1 << *(integer_t*)&policy);
    return 0;
  } else return 1;
}

int pthread_setaffinity_np(pthread_t thread, size_t cpu_size, cpu_set_t *cpu_set)
{
  thread_port_t mach_thread;
  size_t core = 0;
  for (core = 0; core < 8 * cpu_size; core++) {
    if (CPU_ISSET(core, cpu_set)) break;
  }
  thread_affinity_policy_data_t policy = { core };
  mach_thread = pthread_mach_thread_np(thread);
  return KERN_SUCCESS != thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, THREAD_AFFINITY_POLICY_COUNT);
}

#endif
