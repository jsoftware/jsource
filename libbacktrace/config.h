/* config.h.in.  Generated from configure.ac by autoheader.  */

/* ELF size: 32 or 64 */
#ifdef __LP64__
#define BACKTRACE_ELF_SIZE 64
#else
#define BACKTRACE_ELF_SIZE 32
#endif

/* Define to 1 if you have the __atomic functions */
#define HAVE_ATOMIC_FUNCTIONS 1

/* Define to 1 if you have the declaration of `getpagesize', and to 0 if you
   don't. */
//#undef HAVE_DECL_GETPAGESIZE

/* Define if dl_iterate_phdr is available.  Should be true for all ELF platforms, but apparently not on older android versions.
 * See https://reviews.llvm.org/D39468 for a potential workaround if this causes breakage */
#ifdef ANDROID
#if defined(__arm__)
#if __ANDROID_API__ >= 21
#define HAVE_DL_ITERATE_PHDR 1
#endif /* __ANDROID_API__ >= 21 */
#else
#define HAVE_DL_ITERATE_PHDR 1
#endif
#elif !defined(_WIN32) && !defined(__APPLE__)
#define HAVE_DL_ITERATE_PHDR 1
#endif

/* Define to 1 if you have the fcntl function */
#define HAVE_FCNTL 1

/* Define if getexecname is available. Todo do this more robustly */
//#define HAVE_GETEXECNAME 1

/* Define if _Unwind_GetIPInfo is available. */
//#undef HAVE_GETIPINFO

#ifdef __APPLE__
/* Define to 1 if you have the <mach-o/dyld.h> header file. */
#define HAVE_MACH_O_DYLD_H 1
#endif

/* Define to 1 if you have KERN_PROCARGS and KERN_PROC_PATHNAME in
   <sys/sysctl.h>. */
//#define HAVE_KERN_PROC_ARGS 1
/* Define to 1 if you have KERN_PROC and KERN_PROC_PATHNAME in <sys/sysctl.h>.
   */
//#define HAVE_KERN_PROC 1

// not actually used?
/* Define if -llzma is available. */
//#undef HAVE_LIBLZMA
/* Define if -lz is available. */
//#undef HAVE_ZLIB

#define _GNU_SOURCE

/* Define to 1 if you have the <link.h> header file. */
#define HAVE_LINK_H 1

/* Define to 1 if you have the <sys/link.h> header file. */
//#undef HAVE_SYS_LINK_H

/* Define to 1 if you have the `lstat' function. */
#define HAVE_LSTAT 1

/* Define to 1 if you have the `readlink' function. */
#define HAVE_READLINK 1

/* Define to 1 if you have the __sync functions */
#define HAVE_SYNC_FUNCTIONS 1

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif
