#!/bin/bash
# $1 is j32 or j64
# jvars.sh exports CC as gcc or clang

cd ~

# gcc 5 vs 4 - killing off linux asm routines (overflow detection)
# new fast code uses builtins not available in gcc 4
# use -DC_NOMULTINTRINSIC to continue to use more standard c in version 4
# too early to move main linux release package to gcc 5

macmin="-mmacosx-version-min=10.6"

USE_OPENMP="${USE_OPENMP:=0}"
if [ $USE_OPENMP -eq 1 ] ; then
OPENMP=" -fopenmp "
LDOPENMP=" -fopenmp "
if [ -z "${$CCcc##*gcc*}" ] || [ -z "${CC##*gcc*}" ]; then
LDOPENMP32=" -l:libgomp.so.1 "    # gcc
else
LDOPENMP32=" -l:libomp.so.5 "     # clang
fi
fi

if [ $CC = "gcc" ] ; then
#gcc
common="$OPENMP -fPIC -O2 -fwrapv -fno-strict-aliasing -Wextra -Wno-maybe-uninitialized -Wno-unused-parameter -Wno-sign-compare -Wno-clobbered -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses"
OVER_GCC_VER6=$(echo `$CC -dumpversion | cut -f1 -d.` \>= 6 | bc)
if [ $OVER_GCC_VER6 -eq 1 ] ; then
common="$common -Wno-shift-negative-value"
else
common="$common -Wno-type-limits"
fi
# alternatively, add comment /* fall through */
OVER_GCC_VER7=$(echo `$CC -dumpversion | cut -f1 -d.` \>= 7 | bc)
if [ $OVER_GCC_VER7 -eq 1 ] ; then
common="$common -Wno-implicit-fallthrough"
fi
OVER_GCC_VER8=$(echo `$CC -dumpversion | cut -f1 -d.` \>= 8 | bc)
if [ $OVER_GCC_VER8 -eq 1 ] ; then
common="$common -Wno-cast-function-type"
fi
else
# clang 3.4
common="$OPENMP -Werror -fPIC -O2 -fwrapv -fno-strict-aliasing -Wextra -Wno-consumed -Wno-uninitialized -Wno-unused-parameter -Wno-sign-compare -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-unsequenced -Wno-string-plus-int -Wno-tautological-constant-out-of-range-compare"
# clang 3.8
CLANG_MAJOR=$(echo __clang_major__ | $CC -E -x c - | tail -n 1)
CLANG_MINOR=$(echo __clang_minor__ | $CC -E -x c - | tail -n 1)
if [ $CLANG_MAJOR -eq 3 ] && [ $CLANG_MINOR -ge 8 ] ; then
common="$common -Wno-pass-failed"
else
if [ $CLANG_MAJOR -ge 4 ] ; then
common="$common -Wno-pass-failed"
fi
fi
# clang 10
if [ $CLANG_MAJOR -ge 10 ] ; then
common="$common -Wno-implicit-int-float-conversion"
fi
fi
darwin="$OPENMP -fPIC -O2 -fwrapv -fno-strict-aliasing -Wno-string-plus-int -Wno-empty-body -Wno-unsequenced -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-return-type -Wno-constant-logical-operand -Wno-comment -Wno-unsequenced -Wno-pass-failed"

SRC_ASM_LINUX=" \
 keccak1600-x86_64-elf.o \
 sha1-x86_64-elf.o \
 sha256-x86_64-elf.o \
 sha512-x86_64-elf.o "

SRC_ASM_LINUX32=" \
 keccak1600-mmx-elf.o \
 sha1-586-elf.o \
 sha256-586-elf.o \
 sha512-586-elf.o "

SRC_ASM_RASPI=" \
 keccak1600-armv8-elf.o \
 sha1-armv8-elf.o \
 sha256-armv8-elf.o \
 sha512-armv8-elf.o "

SRC_ASM_RASPI32=" \
 keccak1600-armv4-elf.o \
 sha1-armv4-elf.o \
 sha256-armv4-elf.o \
 sha512-armv4-elf.o "

SRC_ASM_MAC=" \
 keccak1600-x86_64-macho.o \
 sha1-x86_64-macho.o \
 sha256-x86_64-macho.o \
 sha512-x86_64-macho.o "

SRC_ASM_MAC32=" \
 keccak1600-mmx-macho.o \
 sha1-586-macho.o \
 sha256-586-macho.o \
 sha512-586-macho.o "

case $jplatform\_$1 in

linux_j32) # linux x86
TARGET=libj.so
# faster, but sse2 not available for 32-bit amd cpu
# sse does not support mfpmath=sse in 32-bit gcc
COMPILE="$common -m32 -msse2 -mfpmath=sse -DC_NOMULTINTRINSIC "
# slower, use 387 fpu and truncate extra precision
# COMPILE="$common -m32 -ffloat-store "
LINK=" -shared -Wl,-soname,libj.so -m32 -lm -ldl $LDOPENMP32 -o libj.so "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_LINUX32}"
GASM_FLAGS="-m32"
;;

linux_j64) # linux intel 64bit nonavx
TARGET=libj.so
COMPILE="$common "
LINK=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP -o libj.so "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_LINUX}"
GASM_FLAGS=""
;;

linux_j64avx) # linux intel 64bit avx
TARGET=libjavx.so
COMPILE="$common -DC_AVX=1 "
LINK=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP -o libjavx.so "
CFLAGS_SIMD=" -mavx "
OBJS_FMA=" blis/gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_LINUX}"
GASM_FLAGS=""
;;

linux_j64avx2) # linux intel 64bit avx2
TARGET=libjavx2.so
COMPILE="$common -DC_AVX=1 -DC_AVX2=1"
LINK=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP -o libjavx2.so "
CFLAGS_SIMD=" -mavx2 -mfma "
OBJS_FMA=" blis/gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_LINUX}"
GASM_FLAGS=""
;;

raspberry_j32) # linux raspbian arm
TARGET=libj.so
COMPILE="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DRASPI -DC_NOMULTINTRINSIC "
LINK=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP -o libj.so "
SRC_ASM="${SRC_ASM_RASPI32}"
GASM_FLAGS=""
;;

raspberry_j64) # linux arm64
TARGET=libj.so
COMPILE="$common -march=armv8-a+crc -DRASPI -DC_CRC32C=1 "
LINK=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP -o libj.so "
OBJS_AESARM=" aes-arm.o "
SRC_ASM="${SRC_ASM_RASPI}"
GASM_FLAGS=""
;;

darwin_j32) # darwin x86
TARGET=libj.dylib
COMPILE="$darwin -m32 $macmin"
LINK=" -dynamiclib -lm -ldl $LDOPENMP -m32 $macmin -o libj.dylib"
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC32}"
GASM_FLAGS="-m32 $macmin"
;;

darwin_j64) # darwin intel 64bit nonavx
TARGET=libj.dylib
COMPILE="$darwin $macmin"
LINK=" -dynamiclib -lm -ldl $LDOPENMP $macmin -o libj.dylib"
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC}"
GASM_FLAGS="$macmin"
;;

darwin_j64avx) # darwin intel 64bit
TARGET=libjavx.dylib
COMPILE="$darwin $macmin -DC_AVX=1"
LINK=" -dynamiclib -lm -ldl $LDOPENMP $macmin -o libjavx.dylib"
CFLAGS_SIMD=" -mavx "
OBJS_FMA=" blis/gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC}"
GASM_FLAGS="$macmin"
;;

darwin_j64avx2) # darwin intel 64bit
TARGET=libjavx2.dylib
COMPILE="$darwin $macmin -DC_AVX=1 -DC_AVX2=1"
LINK=" -dynamiclib -lm -ldl $LDOPENMP $macmin -o libjavx2.dylib"
CFLAGS_SIMD=" -mavx2 -mfma "
OBJS_FMA=" blis/gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC}"
GASM_FLAGS="$macmin"
;;

*)
echo no case for those parameters
exit
esac

OBJS="\
 a.o \
 ab.o \
 aes-c.o \
 aes-sse2.o \
 af.o \
 ai.o \
 am.o \
 am1.o \
 amn.o \
 ao.o \
 ap.o \
 ar.o \
 as.o \
 au.o \
 blis/gemm_c-ref.o \
 blis/gemm_int-aarch64.o \
 blis/gemm_int-avx.o \
 blis/gemm_int-sse2.o \
 blis/gemm_vec-ref.o \
 c.o \
 ca.o \
 cc.o \
 cd.o \
 cf.o \
 cg.o \
 ch.o \
 cip.o \
 cl.o \
 cp.o \
 cpdtsp.o \
 cpuinfo.o \
 cr.o \
 crs.o \
 ct.o \
 cu.o \
 cv.o \
 cx.o \
 d.o \
 dc.o \
 dss.o \
 dstop.o \
 dsusp.o \
 dtoa.o \
 f.o \
 f2.o \
 fbu.o \
 gemm.o \
 i.o \
 io.o \
 j.o \
 jdlllic.o \
 k.o \
 m.o \
 mbx.o \
 p.o \
 pv.o \
 px.o \
 r.o \
 rl.o \
 rt.o \
 s.o \
 sc.o \
 sl.o \
 sn.o \
 t.o \
 u.o \
 v.o \
 v0.o \
 v1.o \
 v2.o \
 va1.o \
 va1ss.o \
 va2.o \
 va2s.o \
 va2ss.o \
 vamultsp.o \
 vb.o \
 vbang.o \
 vbit.o \
 vcant.o \
 vchar.o \
 vcat.o \
 vcatsp.o \
 vcomp.o \
 vcompsc.o \
 vd.o \
 vdx.o \
 ve.o \
 vf.o \
 vfft.o \
 vfrom.o \
 vfromsp.o \
 vg.o \
 vgauss.o \
 vgcomp.o \
 vgranking.o \
 vgsort.o \
 vgsp.o \
 vi.o \
 viavx.o \
 viix.o \
 visp.o \
 vm.o \
 vo.o \
 vp.o \
 vq.o \
 vrand.o \
 vrep.o \
 vs.o \
 vsb.o \
 vt.o \
 vu.o \
 vx.o \
 vz.o \
 w.o \
 wc.o \
 wn.o \
 ws.o \
 x.o \
 x15.o \
 xa.o \
 xaes.o \
 xb.o \
 xc.o \
 xcrc.o \
 xd.o \
 xf.o \
 xfmt.o \
 xh.o \
 xi.o \
 xl.o \
 xo.o \
 xs.o \
 xsha.o \
 xt.o \
 xu.o \
 keccak1600.o \
 md4_dgst.o \
 md4_one.o \
 md5_dgst.o \
 md5_one.o \
 openssl-util.o \
 sha1_one.o \
 sha256.o \
 sha3.o \
 sha512.o "

export OBJS OBJS_FMA OBJS_AESNI OBJS_AESARM SRC_ASM GASM_FLAGS COMPILE CFLAGS_SIMD LINK TARGET
$jmake/domake.sh $1

