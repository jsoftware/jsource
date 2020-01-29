
LOCAL_PATH := $(call my-dir)

LOCAL_MODULE    := j
LOCAL_MODULE_FILENAME    := libj

ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
  LOCAL_CFLAGS := -DUSE_THREAD -DC_CRC32C=1 -fPIC -Os -fvisibility=hidden -fwrapv -Werror -Wno-string-plus-int -Wno-empty-body -Wno-parentheses -Wno-pointer-sign -Wno-logical-op-parentheses -Wno-unused-value -Wno-null-dereference -Wno-type-limits -Wno-pass-failed -D_FORTIFY_SOURCE=2 -Werror=fortify-source -fno-strict-aliasing -march=armv8-a+crc+crypto
  LOCAL_ARM_NEON := true
endif
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
  LOCAL_CFLAGS := -DUSE_THREAD -fPIC -Os -fvisibility=hidden -fwrapv -Werror -Wno-string-plus-int -Wno-empty-body -Wno-parentheses -Wno-pointer-sign -Wno-logical-op-parentheses -Wno-unused-value -Wno-null-dereference -Wno-type-limits -Wno-pass-failed -D_FORTIFY_SOURCE=2 -Werror=fortify-source -fno-strict-aliasing -mfloat-abi=softfp -march=armv7-a
  LOCAL_ARM_MODE := arm
  LOCAL_ARM_NEON := true
endif
ifeq ($(TARGET_ARCH_ABI),armeabi)
  LOCAL_CFLAGS := -DUSE_THREAD -fPIC -Os -fvisibility=hidden -fwrapv -Werror -Wno-string-plus-int -Wno-empty-body -Wno-parentheses -Wno-pointer-sign -Wno-logical-op-parentheses -Wno-unused-value -Wno-null-dereference -Wno-type-limits -fno-strict-aliasing -mfloat-abi=softfp -march=armv5te
  LOCAL_ARM_MODE := arm
endif
ifeq ($(TARGET_ARCH),x86_64)
  LOCAL_CFLAGS := -DUSE_THREAD -DC_CRC32C=1 -fPIC -Os -fvisibility=hidden -fwrapv -Werror -Wno-string-plus-int -Wno-empty-body -Wno-parentheses -Wno-pointer-sign -Wno-logical-op-parentheses -Wno-unused-value -Wno-null-dereference -Wno-type-limits -Wno-pass-failed -D_FORTIFY_SOURCE=2 -Werror=fortify-source -fno-strict-aliasing -march=x86-64 -mtune=intel -msse4.2 -mpopcnt -fno-stack-protector
endif
ifeq ($(TARGET_ARCH),x86)
  LOCAL_CFLAGS := -DUSE_THREAD -fPIC -Os -fvisibility=hidden -fwrapv -Werror -Wno-string-plus-int -Wno-empty-body -Wno-parentheses -Wno-pointer-sign -Wno-logical-op-parentheses -Wno-unused-value -Wno-null-dereference -Wno-type-limits -Wno-pass-failed -D_FORTIFY_SOURCE=2 -Werror=fortify-source -fno-strict-aliasing -march=i686 -mtune=intel -mssse3 -mfpmath=sse -fno-stack-protector
endif
LOCAL_LDLIBS := -ldl -llog
LOCAL_CFLAGS += -fopenmp
LOCAL_LDFLAGS += -fopenmp

LOCAL_SRC_FILES := a.c ab.c aes-c.c aes-arm.c aes-sse2.c af.c ai.c am.c am1.c amn.c ao.c ap.c ar.c as.c au.c c.c ca.c cc.c cd.c cf.c cg.c ch.c cip.c cl.c cp.c cpdtsp.c cpuinfo.c cr.c crs.c \
	ct.c cu.c cv.c cx.c d.c dc.c dss.c dstop.c dsusp.c dtoa.c f.c f2.c fbu.c gemm.c i.c io.c j.c jdlllic.c k.c m.c mbx.c p.c pv.c px.c r.c rl.c rt.c s.c sc.c sl.c \
	sn.c t.c u.c v.c v0.c v1.c v2.c va1.c va1ss.c va2.c va2s.c va2ss.c vamultsp.c vb.c vbang.c vbit.c vcant.c vchar.c vcat.c vcatsp.c vcomp.c vcompsc.c vd.c vdx.c ve.c \
	vf.c vfft.c vfrom.c  vfromsp.c vg.c vgauss.c vgcomp.c vgranking.c vgsort.c vgsp.c vi.c viavx.c viix.c visp.c vm.c vo.c vp.c vq.c vrand.c vrep.c vs.c vsb.c \
	vt.c vu.c vx.c vz.c w.c wc.c wn.c ws.c x.c x15.c xa.c xaes.c xb.c xc.c xcrc.c xd.c xf.c xfmt.c xh.c xi.c xl.c xo.c xs.c xsha.c xt.c xu.c \
	blis/gemm_c-ref.c blis/gemm_int-aarch64.c blis/gemm_int-avx.c blis/gemm_int-fma.c blis/gemm_int-sse2.c blis/gemm_vec-ref.c \
  openssl/sha/keccak1600.c openssl/sha/md4_dgst.c openssl/sha/md4_one.c openssl/sha/md5_dgst.c \
  openssl/sha/md5_one.c openssl/sha/openssl-util.c openssl/sha/sha1_one.c openssl/sha/sha256.c \
  openssl/sha/sha3.c openssl/sha/sha512.c

ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
LOCAL_ASMFLAGS += -f elf64 -X gnu -D LINUX
LOCAL_SRC_FILES += \
  openssl/sha/asm/keccak1600-armv8-elf.S \
  openssl/sha/asm/sha1-armv8-elf.S \
  openssl/sha/asm/sha256-armv8-elf.S \
  openssl/sha/asm/sha512-armv8-elf.S
endif
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_ASMFLAGS += -f elf32 -X gnu -D LINUX
LOCAL_SRC_FILES += \
  openssl/sha/asm/keccak1600-armv4-elf.S \
  openssl/sha/asm/sha1-armv4-elf.S \
  openssl/sha/asm/sha256-armv4-elf.S \
  openssl/sha/asm/sha512-armv4-elf.S
endif
ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_ASMFLAGS += -f elf32 -X gnu -D LINUX
LOCAL_SRC_FILES += \
  openssl/sha/asm/keccak1600-armv4-elf.S \
  openssl/sha/asm/sha1-armv4-elf.S \
  openssl/sha/asm/sha256-armv4-elf.S \
  openssl/sha/asm/sha512-armv4-elf.S
endif

ifeq ($(TARGET_ARCH),x86_64)
LOCAL_ASMFLAGS += -f elf64 -X gnu -D LINUX
LOCAL_SRC_FILES += \
  openssl/sha/asm/keccak1600-x86_64-elf.S \
  openssl/sha/asm/sha1-x86_64-elf.S \
  openssl/sha/asm/sha256-x86_64-elf.S \
  openssl/sha/asm/sha512-x86_64-elf.S
endif
ifeq ($(TARGET_ARCH),x86)
LOCAL_ASMFLAGS += -f elf32 -X gnu -D LINUX
LOCAL_SRC_FILES += \
  openssl/sha/asm/keccak1600-mmx-elf.S \
  openssl/sha/asm/sha1-586-elf.S \
  openssl/sha/asm/sha256-586-elf.S \
  openssl/sha/asm/sha512-586-elf.S
endif

include $(BUILD_SHARED_LIBRARY)
