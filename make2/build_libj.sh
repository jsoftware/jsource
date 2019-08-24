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
common="$OPENMP -fPIC -O1 -fwrapv -fno-strict-aliasing -Wextra -Wno-maybe-uninitialized -Wno-unused-parameter -Wno-sign-compare -Wno-clobbered -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses"
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
# clang 3.5 .. 5.0
common="$OPENMP -Werror -fPIC -O1 -fwrapv -fno-strict-aliasing -Wextra -Wno-consumed -Wno-uninitialized -Wno-unused-parameter -Wno-sign-compare -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-unsequenced -Wno-string-plus-int -Wno-pass-failed"
fi
darwin="$OPENMP -fPIC -O1 -fwrapv -fno-strict-aliasing -Wno-string-plus-int -Wno-empty-body -Wno-unsequenced -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-return-type -Wno-constant-logical-operand -Wno-comment -Wno-unsequenced -Wno-pass-failed"

javx2="${javx2:=0}"

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

case $jplatform\_$j64x in

linux_j32) # linux x86
TARGET=libj.so
# faster, but sse2 not available for 32-bit amd cpu
# sse does not support mfpmath=sse in 32-bit gcc
CFLAGS="$common -m32 -msse2 -mfpmath=sse -DC_NOMULTINTRINSIC "
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
if [ "x$javx2" != x'1' ] ; then
CFLAGS_SIMD=" -mavx "
else
CFLAGS_SIMD=" -DC_AVX2=1 -mavx2 "
fi
OBJS_FMA=" gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_LINUX}"
GASM_FLAGS=""
;;

raspberry_j32) # linux raspbian arm
TARGET=libj.so
CFLAGS="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DRASPI -DC_NOMULTINTRINSIC "
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
CFLAGS="$darwin -m32 $macmin"
LDFLAGS=" -dynamiclib -lm -ldl $LDOPENMP -m32 $macmin"
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC32}"
GASM_FLAGS="-m32 $macmin"
;;

darwin_j64) # darwin intel 64bit nonavx
TARGET=libj.dylib
CFLAGS="$darwin $macmin"
LDFLAGS=" -dynamiclib -lm -ldl $LDOPENMP $macmin"
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC}"
GASM_FLAGS="$macmin"
;;

darwin_j64avx) # darwin intel 64bit
TARGET=libj.dylib
CFLAGS="$darwin $macmin -DC_AVX=1 "
LDFLAGS=" -dynamiclib -lm -ldl $LDOPENMP $macmin"
if [ "x$javx2" != x'1' ] ; then
CFLAGS_SIMD=" -mavx "
else
CFLAGS_SIMD=" -DC_AVX2=1 -mavx2 "
fi
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
CFLAGS="$common $DOLECOM -m32 -msse2 -mfpmath=sse -DC_NOMULTINTRINSIC -D_FILE_OFFSET_BITS=64 -D_JDLL "
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
if [ "x$javx2" != x'1' ] ; then
CFLAGS_SIMD=" -mavx "
else
CFLAGS_SIMD=" -DC_AVX2=1 -mavx2 "
fi
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
export CFLAGS LDFLAGS TARGET CFLAGS_SIMD GASM_FLAGS DLLOBJS LIBJDEF LIBJRES OBJS_FMA OBJS_AESNI OBJS_AESARM SRC_ASM jplatform j64x
cd obj/$jplatform/$j64x/
make -f makefile-libj
cd -
