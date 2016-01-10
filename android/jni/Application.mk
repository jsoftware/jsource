# this file will be overrided by caller's Application.mk if any
APP_OPTIM=release
APP_MODULES := libj jconsole jconsole-nopie libtsdll
APP_ABI := armeabi,armeabi-v7a,x86
# will be enable inside jconsole, it is easier to enable than disable
APP_PIE := false
