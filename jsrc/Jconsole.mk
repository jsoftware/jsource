
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := jconsole

LOCAL_LDLIBS := -ldl -llog

ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
  LOCAL_CFLAGS := -fPIC -Os -fvisibility=hidden -fwrapv -Werror -fno-strict-aliasing -DREADLINE -DUSE_LINENOISE -DC_CRC32C=1 -DEMU_AVX=1 -DPYXES=1 -march=armv8-a
  LOCAL_ARM_NEON := true
endif
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
  LOCAL_CFLAGS := -fPIC -Os -fvisibility=hidden -fwrapv -Werror -fno-strict-aliasing -DREADLINE -DUSE_LINENOISE -mfloat-abi=softfp -mfpu=vfpv3-d16 -march=armv7-a
  LOCAL_ARM_MODE := arm
endif
ifeq ($(TARGET_ARCH_ABI),armeabi)
  LOCAL_CFLAGS := -fPIC -Os -fvisibility=hidden -fwrapv -Werror -fno-strict-aliasing -DREADLINE -DUSE_LINENOISE -mfloat-abi=softfp -mfpu=vfp -march=armv5te
  LOCAL_ARM_MODE := thumb
endif
ifeq ($(TARGET_ARCH),x86_64)
  LOCAL_CFLAGS := -fPIC -Os -fvisibility=hidden -fwrapv -Werror -fno-strict-aliasing -DREADLINE -DUSE_LINENOISE -DC_CRC32C=1 -DEMU_AVX=1 -DPYXES=1 -march=x86-64 -msse4.2 -mpopcnt -fno-stack-protector
endif
ifeq ($(TARGET_ARCH),x86)
  LOCAL_CFLAGS := -fPIC -Os -fvisibility=hidden -fwrapv -Werror -fno-strict-aliasing -DREADLINE -DUSE_LINENOISE -march=i686 -mssse3 -mfpmath=sse -fno-stack-protector
endif

LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie

LOCAL_SRC_FILES :=  jconsole.c jeload.c linenoise.c

include $(BUILD_EXECUTABLE)

