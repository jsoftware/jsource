
# LOCAL_DISABLE_FATAL_LINKER_WARNINGS := true

LOCAL_PATH:=(call my-dir)

include jni/jsrc/Android.mk
include jni/jsrc/Jnative.mk
include jni/jsrc/Jconsole.mk
include jni/jsrc/Jconsole-nopie.mk
include jni/jsrc/Tsdll.mk
include jni/pcre/Android.mk
include jni/pcre2/Android.mk
include jni/hostdefs/Hostdefs.mk
include jni/hostdefs/Hostdefs-nopie.mk
include jni/netdefs/Netdefs.mk
include jni/netdefs/Netdefs-nopie.mk
# include jni/clapack/Android.mk
