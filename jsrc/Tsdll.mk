
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := tsdll
LOCAL_MODULE_FILENAME    := libtsdll

LOCAL_LDLIBS := -lc

ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
  LOCAL_CFLAGS := -fPIC -Os -Werror -fno-strict-aliasing -march=armv8-a
  LOCAL_ARM_MODE := arm
endif
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
  LOCAL_CFLAGS := -fPIC -Os -Werror -fno-strict-aliasing -mfloat-abi=softfp -mfpu=vfpv3-d16 -march=armv7-a
  LOCAL_ARM_MODE := arm
endif
ifeq ($(TARGET_ARCH_ABI),armeabi)
  LOCAL_CFLAGS := -fPIC -Os -Werror -fno-strict-aliasing -mfloat-abi=softfp -march=armv6
  LOCAL_ARM_MODE := arm
endif
ifeq ($(TARGET_ARCH),x86_64)
  LOCAL_CFLAGS := -fPIC -Os -Werror -fno-strict-aliasing -march=x86-64 -mtune=intel -msse4.2 -mpopcnt -fno-stack-protector
endif
ifeq ($(TARGET_ARCH),x86)
  LOCAL_CFLAGS := -fPIC -Os -Werror -fno-strict-aliasing -march=i686 -mtune=intel -mssse3 -mfpmath=sse -fno-stack-protector
endif

LOCAL_SRC_FILES :=  tsdll.c

include $(BUILD_SHARED_LIBRARY)
