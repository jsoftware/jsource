# this file will be overrided by caller's Application.mk if any
APP_OPTIM=release
# APP_OPTIM=debug
# APP_MODULES := jconsole-nopie libj libja jnative libtsdll netdefs-nopie hostdefs-nopie jamalgam-nopie
APP_MODULES := jconsole libj libja jnative libtsdll netdefs hostdefs jamalgam
# APP_MODULES := libjpcre libjpcre2 blas libf2c cholrl lapack
# use ndk r14 gcc-4.9 to build armeabi and armeabi-v7a
# APP_ABI := armeabi
APP_ABI := armeabi-v7a x86 x86_64 arm64-v8a
# APP_ABI := x86_64 arm64-v8a
# will be enable inside Jconsole.mk, it is easier to enable than disable
APP_PIE := false
# android 7.0
APP_PLATFORM := android-21   # android 5.0  first version supported pie executable only
# android 2.3.1
# APP_PLATFORM := android-9  # support nopie only
