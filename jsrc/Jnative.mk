
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := jnative
LOCAL_MODULE_FILENAME    := libjnative

ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
  LOCAL_CFLAGS := -fPIC -Os -fvisibility=hidden -fwrapv -Werror -fno-strict-aliasing -march=armv8-a -I../mpir/include
  LOCAL_ARM_NEON := true
endif
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
  LOCAL_CFLAGS := -DARMEABI_V7A -fPIC -Os -fvisibility=hidden -fwrapv -Werror -fno-strict-aliasing -mfloat-abi=softfp -mfpu=vfpv3-d16 -march=armv7-a -I../mpir/include
  LOCAL_ARM_MODE := arm
endif

LOCAL_SRC_FILES := andjnative.c jeload.c
LOCAL_LDLIBS := -ldl -llog
LOCAL_LDFLAGS += -Wl,-z,noexecstack

include $(BUILD_SHARED_LIBRARY)
