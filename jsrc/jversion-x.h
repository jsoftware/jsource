#define jversion   "9.4.0-betaX"
#ifdef _WIN32
#define jplatform "windows"  // windows/linux/darwin/raspberry/android/...
#elif defined(RASPI)
#define jplatform "raspberry"
#elif defined(ANDROID)
#define jplatform "android"
#elif defined(__MACH__)
#define jplatform "darwin"
#elif defined(__linux__)
#define jplatform "linux"
#elif defined(__FreeBSD__)
#define jplatform "freebsd"
#elif defined(__OpenBSD__)
#define jplatform "openbsd"
#else
#define jplatform "unknown"
#endif
#define jlicense   "GPL3"
#define jbuilder   "unknown"  // website or email
