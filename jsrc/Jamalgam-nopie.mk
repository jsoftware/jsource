
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := jamalgam-nopie

# ndk r21 : OpenMP is now available as a dynamic library (and this is the new default behavior, so link with -static-openmp if you want to stick with the static runtime)
ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
  LOCAL_CFLAGS := -DC_CRC32C=1 -DEMU_AVX2=1 -DPYXES=1 -DSLEEF=1 -DSLEEFQUAD=1 -DENABLE_ADVSIMD -DHAVE_NEON64=1 -fPIC -Os -fvisibility=hidden -fwrapv -Werror -Wno-unknown-warning-option -Wno-string-plus-int -Wno-empty-body -Wno-parentheses -Wno-pointer-sign -Wno-pointer-to-int-cast -Wno-incompatible-function-pointer-types -Wno-logical-op-parentheses -Wno-unused-value -Wno-null-dereference -Wno-type-limits -Wno-pass-failed -D_FORTIFY_SOURCE=2 -Werror=fortify-source -fno-strict-aliasing -march=armv8-a+crc+crypto -Wno-sign-compare -Wno-deprecated-non-prototype -mno-outline-atomics -I../mpir/include
  LOCAL_ARM_NEON := true
  LOCAL_CFLAGS += -fopenmp
  LOCAL_LDFLAGS += -fopenmp -static-openmp
endif
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
  LOCAL_CFLAGS := -DSLEEF=0 -DSLEEFQUAD=1 -DHAVE_NEON32=1 -DARMEABI_V7A -fPIC -Os -fvisibility=hidden -fwrapv -Werror -Wno-unknown-warning-option -Wno-string-plus-int -Wno-empty-body -Wno-parentheses -Wno-pointer-sign -Wno-pointer-to-int-cast -Wno-incompatible-function-pointer-types -Wno-logical-op-parentheses -Wno-unused-value -Wno-null-dereference -Wno-type-limits -Wno-pass-failed -D_FORTIFY_SOURCE=2 -Werror=fortify-source -fno-strict-aliasing -mfloat-abi=softfp -march=armv7-a -Wno-sign-compare -Wno-deprecated-non-prototype -I../mpir/include
  LOCAL_ARM_MODE := arm
  LOCAL_ARM_NEON := true
  LOCAL_CFLAGS += -fopenmp
  LOCAL_LDFLAGS += -fopenmp -static-openmp
endif
ifeq ($(TARGET_ARCH_ABI),armeabi)
  LOCAL_CFLAGS := -std=gnu99 -DSLEEF=0 -DSLEEFQUAD=1 -fPIC -Os -fvisibility=hidden -fwrapv -Werror -Wno-unknown-warning-option -Wno-overflow -Wno-string-plus-int -Wno-empty-body -Wno-int-to-pointer-cast -Wno-parentheses -Wno-pointer-sign -Wno-pointer-to-int-cast -Wno-incompatible-function-pointer-types -Wno-logical-op-parentheses -Wno-unused-value -Wno-null-dereference -Wno-type-limits -Wno-return-local-addr -fno-strict-aliasing -mfloat-abi=softfp -march=armv5te -Wno-sign-compare -Wno-deprecated-non-prototype -I../mpir/include
  LOCAL_ARM_MODE := arm
  LOCAL_CFLAGS += -fopenmp
  LOCAL_LDFLAGS += -fopenmp
endif
ifeq ($(TARGET_ARCH),x86_64)
  LOCAL_CFLAGS := -DC_CRC32C=1 -DEMU_AVX2=1 -DPYXES=1 -DSLEEF=0 -DSLEEFQUAD=1 -DENABLE_SSE2 -DHAVE_SSSE3=1 -DHAVE_SSE42=1 -fPIC -Os -fvisibility=hidden -fwrapv -Werror -Wno-unknown-warning-option -Wno-string-plus-int -Wno-empty-body -Wno-parentheses -Wno-pointer-sign -Wno-pointer-to-int-cast -Wno-incompatible-function-pointer-types -Wno-logical-op-parentheses -Wno-unused-value -Wno-null-dereference -Wno-type-limits -Wno-pass-failed -D_FORTIFY_SOURCE=2 -Werror=fortify-source -fno-strict-aliasing -march=x86-64 -msse4.2 -mpopcnt -fno-stack-protector -Wno-sign-compare -Wno-deprecated-non-prototype -I../mpir/include
  LOCAL_CFLAGS += -fopenmp
  LOCAL_LDFLAGS += -fopenmp -static-openmp
endif
ifeq ($(TARGET_ARCH),x86)
  LOCAL_CFLAGS := -DSLEEF=1 -DSLEEFQUAD=1 -DENABLE_SSE2 -fPIC -Os -fvisibility=hidden -fwrapv -Werror -Wno-unknown-warning-option -Wno-string-plus-int -Wno-empty-body -Wno-parentheses -Wno-pointer-sign -Wno-pointer-to-int-cast -Wno-incompatible-function-pointer-types -Wno-logical-op-parentheses -Wno-unused-value -Wno-null-dereference -Wno-type-limits -Wno-pass-failed -D_FORTIFY_SOURCE=2 -Werror=fortify-source -fno-strict-aliasing -march=i686 -mssse3 -mfpmath=sse -fno-stack-protector -Wno-sign-compare -Wno-deprecated-non-prototype -I../mpir/include
  LOCAL_CFLAGS += -fopenmp
  LOCAL_LDFLAGS += -fopenmp -static-openmp
endif
LOCAL_LDLIBS := -ldl -llog
LOCAL_LDFLAGS += -Wl,-z,noexecstack
LOCAL_CFLAGS += -DJAMALGAM

# LOCAL_CFLAGS += -fPIE
# LOCAL_LDFLAGS += -fPIE -pie

LOCAL_SRC_FILES :=  jconsole.c jeload.c linenoise.c \
../libbacktrace/atomic.c \
../libbacktrace/backtrace.c \
../libbacktrace/dwarf.c \
../libbacktrace/fileline.c \
../libbacktrace/mmap.c \
../libbacktrace/mmapio.c \
../libbacktrace/posix.c \
../libbacktrace/print.c \
../libbacktrace/simple.c \
../libbacktrace/sort.c \
../libbacktrace/state.c \
../libbacktrace/elf.c \
 a.c ab.c aes-arm.c aes-c.c aes-sse2.c af.c ai.c am.c am1.c amn.c ao.c ap.c ar.c as.c au.c c.c ca.c cblas.c cc.c cd.c cf.c cg.c ch.c cip.c cl.c cp.c cpdtsp.c cpuinfo.c cr.c crs.c \
	ct.c cu.c cv.c cx.c d.c dc.c dss.c dstop.c dsusp.c dtoa.c f.c f2.c fbu.c gemm.c i.c io.c j.c jdlllic.c jgmpinit.c k.c m.c mbx.c mt.c p.c pv.c px.c r.c rl.c rt.c s.c sc.c sl.c \
	sn.c t.c u.c v.c v0.c v1.c v2.c va1.c va1ss.c va2.c va2s.c va2ss.c vamultsp.c vb.c vbang.c vbit.c vcant.c vchar.c vcat.c vcatsp.c vcomp.c vcompsc.c vd.c vdx.c ve.c \
	vf.c vfft.c vfrom.c vfromsp.c vg.c vgauss.c vgcomp.c vgranking.c vgsort.c vgsortiqavx512.c vgsp.c vi.c viavx.c viavx1-2.c viavx1-4.c viavx2.c viavx3.c viavx4.c viavx5.c viavx6.c viix.c visp.c vm.c vo.c vp.c vq.c vrand.c vrep.c vs.c vsb.c \
	vt.c vu.c vx.c vz.c w.c wc.c wn.c ws.c x.c x15.c xa.c xaes.c xb.c xc.c xcrc.c xd.c xdic.c xf.c xfmt.c xh.c xi.c xl.c xlp.c xo.c xs.c xsha.c xt.c xu.c crc32c.c str.c strptime.c \
	blis/gemm_c-ref.c blis/gemm_int-aarch64.c blis/gemm_int-avx.c blis/gemm_int-fma.c blis/gemm_int-sse2.c blis/gemm_vec-ref.c \
  openssl/sha/keccak1600.c openssl/sha/md4_dgst.c openssl/sha/md4_one.c openssl/sha/md5_dgst.c \
  openssl/sha/md5_one.c openssl/sha/openssl-util.c openssl/sha/sha1_one.c openssl/sha/sha256.c \
  openssl/sha/sha3.c openssl/sha/sha512.c \
  ../base64/lib/arch/avx2/codec-avx2.c \
  ../base64/lib/arch/avx512/codec-avx512.c \
  ../base64/lib/arch/generic/codec-generic.c \
  ../base64/lib/arch/neon32/codec-neon32.c \
  ../base64/lib/arch/neon64/codec-neon64.c \
  ../base64/lib/arch/ssse3/codec-ssse3.c \
  ../base64/lib/arch/sse41/codec-sse41.c \
  ../base64/lib/arch/sse42/codec-sse42.c \
  ../base64/lib/arch/avx/codec-avx.c \
  ../base64/lib/lib.c \
  ../base64/lib/codec_choose.c \
  ../base64/lib/tables/tables.c

ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
LOCAL_ASMFLAGS += -f elf64 -X gnu -D LINUX
LOCAL_SRC_FILES += \
  openssl/sha/asm/md5-aarch64-elf.S \
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
  openssl/sha/asm/md5-x86_64-elf.S \
  openssl/sha/asm/keccak1600-x86_64-elf.S \
  openssl/sha/asm/sha1-x86_64-elf.S \
  openssl/sha/asm/sha256-x86_64-elf.S \
  openssl/sha/asm/sha512-x86_64-elf.S
endif
ifeq ($(TARGET_ARCH),x86)
LOCAL_ASMFLAGS += -f elf32 -X gnu -D LINUX
LOCAL_SRC_FILES += \
  openssl/sha/asm/md5-586-android.S \
  openssl/sha/asm/keccak1600-mmx-android.S \
  openssl/sha/asm/sha1-586-android.S \
  openssl/sha/asm/sha256-586-android.S \
  openssl/sha/asm/sha512-586-android.S
endif

include $(BUILD_EXECUTABLE)
