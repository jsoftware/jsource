#ifndef J_JNI_INTERFACE
#define J_JNI_INTERFACE

#ifdef ANDROID
#define LOCALOGTAG "libj"
#include <android/log.h>
#define LOGD(msg) __android_log_write(ANDROID_LOG_DEBUG,LOCALOGTAG,msg)
#define LOGFD(...) __android_log_print(ANDROID_LOG_DEBUG,LOCALOGTAG,__VA_ARGS__)
// #define LOGD(msg)
// #define LOGFD(...)

#else
#include <stdio.h>
#define LOGD(msg) printf("%s: %s\n",LOCALOGTAG,msg)

#define LOGFD(str,...) printf(LOCALOGTAG " " str,__VA_ARGS__)

#endif
void _stdcall logcat(char *msg);

#endif // J_JNI_INTERFACE
