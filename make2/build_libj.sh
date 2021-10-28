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
elif [ "`uname -m`" = "arm64" ] && [ -z "${jplatform##*darwin*}" ]; then
j64x="${j64x:=j64arm}"
else
j64x="${j64x:=j32}"
fi

# gcc 5 vs 4 - killing off linux asm routines (overflow detection)
# new fast code uses builtins not available in gcc 4
# use -DC_NOMULTINTRINSIC to continue to use more standard c in version 4
# too early to move main linux release package to gcc 5

if [ -z "${jplatform##*darwin*}" ]; then
if [ -z "${j64x##*j64arm*}" ]; then
macmin="-target arm64-apple-macos11 -mmacosx-version-min=11"
NO_SHA_ASM="${NO_SHA_ASM:=1}"
else
macmin="-target x86_64-apple-macos10.6 -mmacosx-version-min=10.6"
fi
fi

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
if [ -z "${j64x##*j64arm*}" ]; then
# assume libomp installed at /opt/homebrew/
OPENMP=" -Xpreprocessor -fopenmp -I/opt/homebrew/include "
LDOPENMP=" -L/opt/homebrew/lib -Wl,-rpath,/opt/homebrew/lib -lomp "
else
# assume libomp installed at /usr/local/
OPENMP=" -Xpreprocessor -fopenmp -I/usr/local/include "
LDOPENMP=" -L/usr/local/lib -Wl,-rpath,/usr/local/lib -lomp "
fi
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
common="$OPENMP -fPIC -O2 -fvisibility=hidden -fno-strict-aliasing -fno-stack-protector  \
 -Werror -Wextra -Wno-unknown-warning-option \
 -Wno-cast-function-type \
 -Wno-clobbered \
 -Wno-empty-body \
 -Wno-format-overflow \
 -Wno-implicit-fallthrough \
 -Wno-int-to-pointer-cast \
 -Wno-maybe-uninitialized \
 -Wno-missing-field-initializers \
 -Wno-parentheses \
 -Wno-pointer-sign \
 -Wno-shift-negative-value \
 -Wno-sign-compare \
 -Wno-type-limits \
 -Wno-uninitialized \
 -Wno-unused-parameter \
 -Wno-unused-value "

else
# clang
common="$OPENMP -fPIC -O2 -fvisibility=hidden -fno-strict-aliasing  \
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

USE_BOXEDSPARSE="${USE_BOXEDSPARSE:=0}"
if [ $USE_BOXEDSPARSE -eq 1 ] ; then
common="$common -DBOXEDSPARSE"
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

OBJS_BASE64=" \
  ../../../../base64/lib/arch/avx2/codec-avx2.o \
  ../../../../base64/lib/arch/generic/codec-generic.o \
  ../../../../base64/lib/arch/neon64/codec-neon64.o \
  ../../../../base64/lib/arch/ssse3/codec-ssse3.o \
  ../../../../base64/lib/arch/sse41/codec-sse41.o \
  ../../../../base64/lib/arch/sse42/codec-sse42.o \
  ../../../../base64/lib/arch/avx/codec-avx.o \
  ../../../../base64/lib/lib.o \
  ../../../../base64/lib/codec_choose.o \
  ../../../../base64/lib/tables/tables.o \
"

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
FLAGS_BASE64=""
;;

linux_j64) # linux intel 64bit nonavx
TARGET=libj.so
CFLAGS="$common -msse3 "
LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP $LDTHREAD"
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_LINUX}"
GASM_FLAGS=""
FLAGS_SLEEF=" -DENABLE_SSE2 "
FLAGS_BASE64=""
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
FLAGS_BASE64=" -DHAVE_SSSE3=1 -DHAVE_AVX=1 "
;;

linux_j64avx2) # linux intel 64bit avx2
TARGET=libj.so
CFLAGS="$common -DC_AVX=1 -DC_AVX2=1 "
LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP $LDTHREAD"
CFLAGS_SIMD=" -march=haswell -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt "
OBJS_FMA=" gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_LINUX}"
GASM_FLAGS=""
FLAGS_SLEEF=" -DENABLE_AVX2 "
FLAGS_BASE64=" -DHAVE_AVX2=1 "
;;

raspberry_j32) # linux raspbian arm
TARGET=libj.so
CFLAGS="$common -Wno-overflow -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DRASPI "
LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP $LDTHREAD"
SRC_ASM="${SRC_ASM_RASPI32}"
GASM_FLAGS=""
FLAGS_SLEEF=" -DENABLE_VECEXT "    # ENABLE_NEON32 single precision, useless
FLAGS_BASE64=""
;;

raspberry_j64) # linux arm64
TARGET=libj.so
CFLAGS="$common -march=armv8-a+crc -DRASPI -DC_CRC32C=1 "
LDFLAGS=" -shared -Wl,-soname,libj.so -lm -ldl $LDOPENMP $LDTHREAD"
OBJS_AESARM=" aes-arm.o "
SRC_ASM="${SRC_ASM_RASPI}"
GASM_FLAGS=""
FLAGS_SLEEF=" -DENABLE_ADVSIMD "
FLAGS_BASE64=" -DHAVE_NEON64=1 "
;;

darwin_j32) # darwin x86
TARGET=libj.dylib
CFLAGS="$common -m32 -msse2 -mfpmath=sse $macmin"
LDFLAGS=" -dynamiclib -lm -ldl $LDOPENMP $LDTHREAD -m32 $macmin"
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC32}"
GASM_FLAGS="-m32 $macmin"
FLAGS_SLEEF=" -DENABLE_SSE2 "
FLAGS_BASE64=""
;;

darwin_j64) # darwin intel 64bit nonavx
TARGET=libj.dylib
CFLAGS="$common $macmin -msse3 "
LDFLAGS=" -dynamiclib -lm -ldl $LDOPENMP $LDTHREAD $macmin"
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC}"
GASM_FLAGS="$macmin"
FLAGS_SLEEF=" -DENABLE_SSE2 "
FLAGS_BASE64=""
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
FLAGS_BASE64=" -DHAVE_SSSE3=1 -DHAVE_AVX=1 "
;;

darwin_j64avx2) # darwin intel 64bit
TARGET=libj.dylib
CFLAGS="$common $macmin -DC_AVX=1 -DC_AVX2=1 "
LDFLAGS=" -dynamiclib -lm -ldl $LDOPENMP $LDTHREAD $macmin"
CFLAGS_SIMD=" -march=haswell -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt "
OBJS_FMA=" gemm_int-fma.o "
OBJS_AESNI=" aes-ni.o "
SRC_ASM="${SRC_ASM_MAC}"
GASM_FLAGS="$macmin"
FLAGS_SLEEF=" -DENABLE_AVX2 "
FLAGS_BASE64=" -DHAVE_AVX2=1 "
;;

darwin_j64arm) # darwin arm
TARGET=libj.dylib
CFLAGS="$common $macmin $common -march=armv8-a+crc -DC_CRC32C=1 "
LDFLAGS=" -dynamiclib -lm -ldl $LDOPENMP $LDTHREAD $macmin"
OBJS_AESARM=" aes-arm.o "
SRC_ASM=""
GASM_FLAGS="$macmin"
FLAGS_SLEEF=" -DENABLE_ADVSIMD "
FLAGS_BASE64=" -DHAVE_NEON64=1 "
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
FLAGS_BASE64=""
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
FLAGS_BASE64=""
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
FLAGS_BASE64=" -DHAVE_SSSE3=1 -DHAVE_AVX=1 "
;;

windows_j64avx2) # windows intel 64bit avx
jolecom="${jolecom:=0}"
if [ $jolecom -eq 1 ] ; then
DOLECOM="-DOLECOM"
fi
TARGET=j.dll
CFLAGS="$common $DOLECOM -DC_AVX=1 -DC_AVX2=1 -D_FILE_OFFSET_BITS=64 -D_JDLL "
LDFLAGS=" -shared -Wl,--enable-stdcall-fixup -lm -static-libgcc -static-libstdc++ $LDOPENMP $LDTHREAD"
CFLAGS_SIMD=" -march=haswell -mavx2 -mfma -mbmi -mbmi2 -mlzcnt -mmovbe -mpopcnt "
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
FLAGS_BASE64=" -DHAVE_AVX2=1 "
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
fi
fi

echo "CFLAGS=$CFLAGS"

if [ ! -f ../jsrc/jversion.h ] ; then
  cp ../jsrc/jversion-x.h ../jsrc/jversion.h
fi

mkdir -p ../bin/$jplatform/$j64x
mkdir -p obj/$jplatform/$j64x/
cp makefile-libj obj/$jplatform/$j64x/.
export CFLAGS LDFLAGS TARGET CFLAGS_SIMD GASM_FLAGS FLAGS_SLEEF FLAGS_BASE64 DLLOBJS LIBJDEF LIBJRES OBJS_BASE64 OBJS_FMA OBJS_AESNI OBJS_AESARM OBJS_SLEEF OBJS_ASM SRC_ASM jplatform j64x
cd obj/$jplatform/$j64x/
make -f makefile-libj
cd -
