
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := tsdll
LOCAL_MODULE_FILENAME    := libtsdll

ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
  LOCAL_CFLAGS := -fPIC -Os -fvisibility=hidden -fwrapv -Werror -fno-strict-aliasing -march=armv8-a
  LOCAL_ARM_NEON := true
endif
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
  LOCAL_CFLAGS := -fPIC -Os -fvisibility=hidden -fwrapv -Werror -fno-strict-aliasing -mfloat-abi=softfp -mfpu=vfpv3-d16 -march=armv7-a
  LOCAL_ARM_MODE := arm
endif

LOCAL_SRC_FILES :=  tsdll.c
LOCAL_LDLIBS := -lm
LOCAL_LDFLAGS += -Wl,-z,noexecstack

include $(BUILD_SHARED_LIBRARY)
