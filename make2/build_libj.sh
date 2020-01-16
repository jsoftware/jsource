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
OPENMP=" -fopenmp "
LDOPENMP=" -fopenmp "
if [ -z "${compiler##*gcc*}" ] || [ -z "${CC##*gcc*}" ]; then
LDOPENMP32=" -l:libgomp.so.1 "    # gcc
else
LDOPENMP32=" -l:libomp.so.5 "     # clang
fi
fi

if [ -z "${compiler##*gcc*}" ] || [ -z "${CC##*gcc*}" ]; then
# gcc
common="$OPENMP -Werror -fPIC -O2 -fwrapv -fno-strict-aliasing -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-clobbered -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-type-limits"
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
common="$common -Wno-implicit-float-conversion"
fi
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
LDFLAGS=" -shared -Wl,-soname,libj.so -m32 -lm -ldl $LDOPENMP32"
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_LINUX32}"
GASM_FLAGS="-m32"
;;

linux_j64) # linux intel 64bit nonavx
TARGET=libj.so
CFLAGS="$common "
LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP"
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_LINUX}"
GASM_FLAGS=""
;;

linux_j64avx) # linux intel 64bit avx
TARGET=libj.so
CFLAGS="$common -DC_AVX=1 "
LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP"
CFLAGS_SIMD=" -mavx "
OBJS_FMA=" gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_LINUX}"
GASM_FLAGS=""
;;

linux_j64avx2) # linux intel 64bit avx2
TARGET=libj.so
CFLAGS="$common -DC_AVX=1 -DC_AVX2=1 "
LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP"
CFLAGS_SIMD=" -mavx2 -mfma "
OBJS_FMA=" gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_LINUX}"
GASM_FLAGS=""
;;

raspberry_j32) # linux raspbian arm
TARGET=libj.so
CFLAGS="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DRASPI "
LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP"
SRC_ASM="${SRC_ASM_RASPI32}"
GASM_FLAGS=""
;;

raspberry_j64) # linux arm64
TARGET=libj.so
CFLAGS="$common -march=armv8-a+crc -DRASPI -DC_CRC32C=1 "
LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP"
OBJS_AESARM=" aes-arm.o "
SRC_ASM="${SRC_ASM_RASPI}"
GASM_FLAGS=""
;;

darwin_j32) # darwin x86
TARGET=libj.dylib
CFLAGS="$common -m32 $macmin"
LDFLAGS=" -dynamiclib -lm -ldl $LDOPENMP -m32 $macmin"
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC32}"
GASM_FLAGS="-m32 $macmin"
;;

darwin_j64) # darwin intel 64bit nonavx
TARGET=libj.dylib
CFLAGS="$common $macmin"
LDFLAGS=" -dynamiclib -lm -ldl $LDOPENMP $macmin"
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC}"
GASM_FLAGS="$macmin"
;;

darwin_j64avx) # darwin intel 64bit
TARGET=libj.dylib
CFLAGS="$common $macmin -DC_AVX=1 "
LDFLAGS=" -dynamiclib -lm -ldl $LDOPENMP $macmin"
CFLAGS_SIMD=" -mavx "
OBJS_FMA=" gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC}"
GASM_FLAGS="$macmin"
;;

darwin_j64avx2) # darwin intel 64bit
TARGET=libj.dylib
CFLAGS="$common $macmin -DC_AVX=1 -DC_AVX2=1 "
LDFLAGS=" -dynamiclib -lm -ldl $LDOPENMP $macmin"
CFLAGS_SIMD=" -mavx2 -mfma "
OBJS_FMA=" gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC}"
GASM_FLAGS="$macmin"
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
LDFLAGS=" -shared -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ $LDOPENMP32 "
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
;;

windows_j64) # windows intel 64bit nonavx
jolecom="${jolecom:=0}"
if [ $jolecom -eq 1 ] ; then
DOLECOM="-DOLECOM"
fi
TARGET=j.dll
CFLAGS="$common $DOLECOM -D_FILE_OFFSET_BITS=64 -D_JDLL "
LDFLAGS=" -shared -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ $LDOPENMP "
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
;;

windows_j64avx) # windows intel 64bit avx
jolecom="${jolecom:=0}"
if [ $jolecom -eq 1 ] ; then
DOLECOM="-DOLECOM"
fi
TARGET=j.dll
CFLAGS="$common $DOLECOM -DC_AVX=1 -D_FILE_OFFSET_BITS=64 -D_JDLL "
LDFLAGS=" -shared -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ $LDOPENMP "
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
;;

windows_j64avx2) # windows intel 64bit avx
jolecom="${jolecom:=0}"
if [ $jolecom -eq 1 ] ; then
DOLECOM="-DOLECOM"
fi
TARGET=j.dll
CFLAGS="$common $DOLECOM -DC_AVX=1 -DC_AVX2=1 -D_FILE_OFFSET_BITS=64 -D_JDLL "
LDFLAGS=" -shared -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ $LDOPENMP "
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
;;

*)
echo no case for those parameters
exit
esac

echo "CFLAGS=$CFLAGS"

if [ ! -f ../jsrc/jversion.h ] ; then
  cp ../jsrc/jversion-x.h ../jsrc/jversion.h
fi

mkdir -p ../bin/$jplatform/$j64x
mkdir -p obj/$jplatform/$j64x/
cp makefile-libj obj/$jplatform/$j64x/.
export CFLAGS LDFLAGS TARGET CFLAGS_SIMD GASM_FLAGS DLLOBJS LIBJDEF LIBJRES OBJS_FMA OBJS_AESNI OBJS_AESARM OBJS_ASM SRC_ASM jplatform j64x
cd obj/$jplatform/$j64x/
make -f makefile-libj
cd -
