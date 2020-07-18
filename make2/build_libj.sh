#!/bin/sh

realpath()
{
 oldpath=`pwd`
 if ! cd $1 > /dev/null 2>&1; then
  cd ${1##*/} > /dev/null 2>&1
  echo $( pwd -P )/${1%/*}
 else
  pwd -P
 fi
 cd $oldpath > /dev/null 2>&1
}

cd "$(realpath "$0")"
echo "entering `pwd`"

if [ "`uname -m`" = "armv6l" ] || [ "`uname -m`" = "aarch64" ] || [ "$RASPI" = 1 ]; then
jplatform="${jplatform:=raspberry}"
elif [ "`uname`" = "Darwin" ]; then
jplatform="${jplatform:=darwin}"
else
jplatform="${jplatform:=linux}"
fi
if [ "`uname -m`" = "x86_64" ]; then
j64x="${j64x:=j64avx}"
elif [ "`uname -m`" = "aarch64" ]; then
j64x="${j64x:=j64}"
else
j64x="${j64x:=j32}"
fi

# gcc 5 vs 4 - killing off linux asm routines (overflow detection)
# new fast code uses builtins not available in gcc 4
# use -DC_NOMULTINTRINSIC to continue to use more standard c in version 4
# too early to move main linux release package to gcc 5

macmin="-mmacosx-version-min=10.6"

if [ "x$CC" = x'' ] ; then
if [ -f "/usr/bin/cc" ]; then
CC=cc
else
if [ -f "/usr/bin/clang" ]; then
CC=clang
else
CC=gcc
fi
fi
export CC
fi
# compiler=`$CC --version | head -n 1`
compiler=$(readlink -f $(command -v $CC) 2> /dev/null || echo $CC)
echo "CC=$CC"
echo "compiler=$compiler"

USE_OPENMP="${USE_OPENMP:=0}"
if [ $USE_OPENMP -eq 1 ] ; then
if [ -z "${jplatform##*darwin*}" ]; then
# assume libomp installed at /usr/local/opt/libomp
OPENMP=" -Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include "
LDOPENMP=" -L/usr/local/opt/libomp/lib -Wl,-rpath,/usr/local/opt/libomp/lib -lomp "
else
OPENMP=" -fopenmp "
LDOPENMP=" -fopenmp "
if [ -z "${compiler##*gcc*}" ] || [ -z "${CC##*gcc*}" ]; then
LDOPENMP32=" -l:libgomp.so.1 "    # gcc
else
if [ -f /etc/redhat-release ] ; then
LDOPENMP32=" -l:libomp.so "     # clang
else
LDOPENMP32=" -l:libomp.so.5 "     # clang
fi
fi
fi
fi

if [ -z "${compiler##*gcc*}" ] || [ -z "${CC##*gcc*}" ]; then
# gcc
common="$OPENMP -fPIC -O2 -fvisibility=hidden -fno-strict-aliasing  \
 -Werror -Wextra \
 -Wno-clobbered \
 -Wno-empty-body \
 -Wno-format-overflow \
 -Wno-parentheses \
 -Wno-pointer-sign \
 -Wno-sign-compare \
 -Wno-type-limits \
 -Wno-unused-parameter \
 -Wno-unused-value \
 -Wno-format-overflow"
GNUC_MAJOR=$(echo __GNUC__ | $CC -E -x c - | tail -n 1)
GNUC_MINOR=$(echo __GNUC_MINOR__ | $CC -E -x c - | tail -n 1)
if [ $GNUC_MAJOR -ge 5 ] ; then
common="$common -Wno-maybe-uninitialized"
else
common="$common -DC_NOMULTINTRINSIC -Wno-uninitialized"
fi
if [ $GNUC_MAJOR -ge 6 ] ; then
common="$common -Wno-shift-negative-value"
fi
# alternatively, add comment /* fall through */
if [ $GNUC_MAJOR -ge 7 ] ; then
common="$common -Wno-implicit-fallthrough"
fi
if [ $GNUC_MAJOR -ge 8 ] ; then
common="$common -Wno-cast-function-type"
fi
else
# clang
common="$OPENMP -fPIC -O2 -fvisibility=hidden -fno-strict-aliasing \
 -Werror -Wextra -Wno-unknown-warning-option \
 -Wsign-compare \
 -Wtautological-constant-out-of-range-compare \
 -Wuninitialized \
 -Wno-char-subscripts \
 -Wno-consumed \
 -Wno-delete-non-abstract-non-virtual-dtor \
 -Wno-empty-body \
 -Wno-implicit-float-conversion \
 -Wno-implicit-int-float-conversion \
 -Wno-int-in-bool-context \
 -Wno-missing-braces \
 -Wno-parentheses \
 -Wno-pass-failed \
 -Wno-pointer-sign \
 -Wno-sometimes-uninitialized \
 -Wno-string-plus-int \
 -Wno-unknown-pragmas \
 -Wno-unsequenced \
 -Wno-unused-function \
 -Wno-unused-parameter \
 -Wno-unused-value \
 -Wno-unused-variable "

fi

USE_SLEEF_SRC="${USE_SLEEF_SRC:=1}"
if [ -z "${j64x##*32*}" ] && [ -z "${jplatform##*raspberry*}" ]; then
# USE_SLEEF="${USE_SLEEF:=1}"
USE_SLEEF=0
else
USE_SLEEF="${USE_SLEEF:=1}"
fi
if [ $USE_SLEEF -eq 1 ] ; then
common="$common -DSLEEF=1"
else
USE_SLEEF_SRC=0
fi

if [ -z "${j64x##*32*}" ]; then
USE_EMU_AVX=0
else
USE_EMU_AVX="${USE_EMU_AVX:=1}"
fi
if [ $USE_EMU_AVX -eq 1 ] ; then
common="$common -DEMU_AVX=1"
fi

NO_SHA_ASM="${NO_SHA_ASM:=0}"

if [ $NO_SHA_ASM -ne 0 ] ; then

common="$common -DNO_SHA_ASM"

else

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

OBJS_ASM_WIN=" \
 ../../../../openssl-asm/keccak1600-x86_64-nasm.o \
 ../../../../openssl-asm/sha1-x86_64-nasm.o \
 ../../../../openssl-asm/sha256-x86_64-nasm.o \
 ../../../../openssl-asm/sha512-x86_64-nasm.o "

OBJS_ASM_WIN32=" \
 ../../../../openssl-asm/keccak1600-mmx-nasm.o \
 ../../../../openssl-asm/sha1-586-nasm.o \
 ../../../../openssl-asm/sha256-586-nasm.o \
 ../../../../openssl-asm/sha512-586-nasm.o "

fi

case $jplatform\_$j64x in

linux_j32) # linux x86
TARGET=libj.so
# faster, but sse2 not available for 32-bit amd cpu
# sse does not support mfpmath=sse in 32-bit gcc
CFLAGS="$common -m32 -msse2 -mfpmath=sse "
# slower, use 387 fpu and truncate extra precision
# CFLAGS="$common -m32 -ffloat-store "
LDFLAGS=" -shared -Wl,-soname,libj.so -m32 -lm -ldl $LDOPENMP32 $LDTHREAD"
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_LINUX32}"
GASM_FLAGS="-m32"
FLAGS_SLEEF=" -DENABLE_SSE2 "
LIBSLEEF=../../../../sleef/lib/linux32/libsleef.a
;;

linux_j64) # linux intel 64bit nonavx
TARGET=libj.so
CFLAGS="$common -msse3 "
LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP $LDTHREAD"
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_LINUX}"
GASM_FLAGS=""
FLAGS_SLEEF=" -DENABLE_SSE2 "
LIBSLEEF=../../../../sleef/lib/linux/libsleef.a
;;

linux_j64avx) # linux intel 64bit avx
TARGET=libj.so
CFLAGS="$common -DC_AVX=1 "
LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP $LDTHREAD"
CFLAGS_SIMD=" -mavx "
OBJS_FMA=" gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_LINUX}"
GASM_FLAGS=""
FLAGS_SLEEF=" -DENABLE_AVX "
LIBSLEEF=../../../../sleef/lib/linux/libsleef.a
;;

linux_j64avx2) # linux intel 64bit avx2
TARGET=libj.so
CFLAGS="$common -DC_AVX=1 -DC_AVX2=1 "
LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP $LDTHREAD"
CFLAGS_SIMD=" -mavx2 -mfma "
OBJS_FMA=" gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_LINUX}"
GASM_FLAGS=""
FLAGS_SLEEF=" -DENABLE_AVX2 "
LIBSLEEF=../../../../sleef/lib/linux/libsleef.a
;;

raspberry_j32) # linux raspbian arm
TARGET=libj.so
CFLAGS="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DRASPI "
LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP $LDTHREAD"
SRC_ASM="${SRC_ASM_RASPI32}"
GASM_FLAGS=""
FLAGS_SLEEF=" -DENABLE_VECEXT "    # ENABLE_NEON32 single precision, useless
LIBSLEEF=../../../../sleef/lib/raspberry32/libsleef.a
;;

raspberry_j64) # linux arm64
TARGET=libj.so
CFLAGS="$common -march=armv8-a+crc -DRASPI -DC_CRC32C=1 "
LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP $LDTHREAD"
OBJS_AESARM=" aes-arm.o "
SRC_ASM="${SRC_ASM_RASPI}"
GASM_FLAGS=""
FLAGS_SLEEF=" -DENABLE_ADVSIMD "
LIBSLEEF=../../../../sleef/lib/raspberry/libsleef.a
;;

darwin_j32) # darwin x86
TARGET=libj.dylib
CFLAGS="$common -m32 $macmin"
LDFLAGS=" -dynamiclib -lm -ldl $LDOPENMP $LDTHREAD -m32 $macmin"
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC32}"
GASM_FLAGS="-m32 $macmin"
FLAGS_SLEEF=" -DENABLE_SSE2 "
LIBSLEEF=../../../../sleef/lib/darwin32/libsleef.a
;;

darwin_j64) # darwin intel 64bit nonavx
TARGET=libj.dylib
CFLAGS="$common $macmin -msse3 "
LDFLAGS=" -dynamiclib -lm -ldl $LDOPENMP $LDTHREAD $macmin"
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC}"
GASM_FLAGS="$macmin"
FLAGS_SLEEF=" -DENABLE_SSE2 "
LIBSLEEF=../../../../sleef/lib/darwin/libsleef.a
;;

darwin_j64avx) # darwin intel 64bit
TARGET=libj.dylib
CFLAGS="$common $macmin -DC_AVX=1 "
LDFLAGS=" -dynamiclib -lm -ldl $LDOPENMP $LDTHREAD $macmin"
CFLAGS_SIMD=" -mavx "
OBJS_FMA=" gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC}"
GASM_FLAGS="$macmin"
FLAGS_SLEEF=" -DENABLE_AVX "
LIBSLEEF=../../../../sleef/lib/darwin/libsleef.a
;;

darwin_j64avx2) # darwin intel 64bit
TARGET=libj.dylib
CFLAGS="$common $macmin -DC_AVX=1 -DC_AVX2=1 "
LDFLAGS=" -dynamiclib -lm -ldl $LDOPENMP $LDTHREAD $macmin"
CFLAGS_SIMD=" -mavx2 -mfma "
OBJS_FMA=" gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC}"
GASM_FLAGS="$macmin"
FLAGS_SLEEF=" -DENABLE_AVX2 "
LIBSLEEF=../../../../sleef/lib/darwin/libsleef.a
;;

windows_j32) # windows x86
jolecom="${jolecom:=0}"
if [ $jolecom -eq 1 ] ; then
DOLECOM="-DOLECOM"
fi
TARGET=j.dll
# faster, but sse2 not available for 32-bit amd cpu
# sse does not support mfpmath=sse in 32-bit gcc
CFLAGS="$common $DOLECOM -m32 -msse2 -mfpmath=sse -D_FILE_OFFSET_BITS=64 -D_JDLL "
# slower, use 387 fpu and truncate extra precision
# CFLAGS="$common -m32 -ffloat-store "
LDFLAGS=" -shared -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ $LDOPENMP32 $LDTHREAD"
if [ $jolecom -eq 1 ] ; then
DLLOBJS=" jdll.o jdllcomx.o "
LIBJDEF=" ../../../../dllsrc/jdll.def "
else
DLLOBJS=" jdll.o "
LIBJDEF=" ../../../../dllsrc/jdll2.def "
fi
LIBJRES=" jdllres.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_WIN32}"
OBJS_ASM="${OBJS_ASM_WIN32}"
GASM_FLAGS=""
FLAGS_SLEEF=" -DENABLE_SSE2 "
LIBSLEEF=../../../../sleef/lib/win32/libsleef.a
;;

windows_j64) # windows intel 64bit nonavx
jolecom="${jolecom:=0}"
if [ $jolecom -eq 1 ] ; then
DOLECOM="-DOLECOM"
fi
TARGET=j.dll
CFLAGS="$common -msse3 $DOLECOM -D_FILE_OFFSET_BITS=64 -D_JDLL "
LDFLAGS=" -shared -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ $LDOPENMP $LDTHREAD"
if [ $jolecom -eq 1 ] ; then
DLLOBJS=" jdll.o jdllcomx.o "
LIBJDEF=" ../../../../dllsrc/jdll.def "
else
DLLOBJS=" jdll.o "
LIBJDEF=" ../../../../dllsrc/jdll2.def "
fi
LIBJRES=" jdllres.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_WIN}"
OBJS_ASM="${OBJS_ASM_WIN}"
GASM_FLAGS=""
FLAGS_SLEEF=" -DENABLE_SSE2 "
LIBSLEEF=../../../../sleef/lib/win/libsleef.a
;;

windows_j64avx) # windows intel 64bit avx
jolecom="${jolecom:=0}"
if [ $jolecom -eq 1 ] ; then
DOLECOM="-DOLECOM"
fi
TARGET=j.dll
CFLAGS="$common $DOLECOM -DC_AVX=1 -D_FILE_OFFSET_BITS=64 -D_JDLL "
LDFLAGS=" -shared -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ $LDOPENMP $LDTHREAD"
CFLAGS_SIMD=" -mavx "
if [ $jolecom -eq 1 ] ; then
DLLOBJS=" jdll.o jdllcomx.o "
LIBJDEF=" ../../../../dllsrc/jdll.def "
else
DLLOBJS=" jdll.o "
LIBJDEF=" ../../../../dllsrc/jdll2.def "
fi
LIBJRES=" jdllres.o "
OBJS_FMA=" gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_WIN}"
OBJS_ASM="${OBJS_ASM_WIN}"
GASM_FLAGS=""
FLAGS_SLEEF=" -DENABLE_AVX "
LIBSLEEF=../../../../sleef/lib/win/libsleef.a
;;

windows_j64avx2) # windows intel 64bit avx
jolecom="${jolecom:=0}"
if [ $jolecom -eq 1 ] ; then
DOLECOM="-DOLECOM"
fi
TARGET=j.dll
CFLAGS="$common $DOLECOM -DC_AVX=1 -DC_AVX2=1 -D_FILE_OFFSET_BITS=64 -D_JDLL "
LDFLAGS=" -shared -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ $LDOPENMP $LDTHREAD"
CFLAGS_SIMD=" -mavx2 -mfma "
if [ $jolecom -eq 1 ] ; then
DLLOBJS=" jdll.o jdllcomx.o "
LIBJDEF=" ../../../../dllsrc/jdll.def "
else
DLLOBJS=" jdll.o "
LIBJDEF=" ../../../../dllsrc/jdll2.def "
fi
LIBJRES=" jdllres.o "
OBJS_FMA=" gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_WIN}"
OBJS_ASM="${OBJS_ASM_WIN}"
GASM_FLAGS=""
FLAGS_SLEEF=" -DENABLE_AVX2 "
LIBSLEEF=../../../../sleef/lib/win/libsleef.a
;;

*)
echo no case for those parameters
exit
esac

if [ $USE_SLEEF -eq 1 ] ; then
if [ $USE_SLEEF_SRC -eq 1 ] ; then
OBJS_SLEEF=" \
 ../../../../sleef/src/common/arraymap.o \
 ../../../../sleef/src/common/common.o \
 ../../../../sleef/src/libm/rempitab.o \
 ../../../../sleef/src/libm/sleefsimddp.o \
 "
LIBSLEEF=
fi
else
LIBSLEEF=
fi

echo "CFLAGS=$CFLAGS"

if [ ! -f ../jsrc/jversion.h ] ; then
  cp ../jsrc/jversion-x.h ../jsrc/jversion.h
fi

mkdir -p ../bin/$jplatform/$j64x
mkdir -p obj/$jplatform/$j64x/
cp makefile-libj obj/$jplatform/$j64x/.
export CFLAGS LDFLAGS TARGET CFLAGS_SIMD GASM_FLAGS FLAGS_SLEEF DLLOBJS LIBJDEF LIBJRES LIBSLEEF OBJS_FMA OBJS_AESNI OBJS_AESARM OBJS_SLEEF OBJS_ASM SRC_ASM jplatform j64x
cd obj/$jplatform/$j64x/
make -f makefile-libj
cd -
