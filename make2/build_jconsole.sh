#!/bin/sh
set -e

cd "$(dirname "$0")"
echo "entering `pwd`"

jplatform64=$(./jplatform64.sh)

if [ "" = "$CFLAGS" ]; then
 # OPTLEVEL will be merged back into CFLAGS, further down
	# OPTLEVEL is probably overly elaborate, but it works
 case "$_DEBUG" in
  3) OPTLEVEL=" -O2 -g "
   NASM_FLAGS="-g";;
  2) OPTLEVEL=" -O0 -ggdb "
   NASM_FLAGS="-g";;
  1) OPTLEVEL=" -O2 -g "
   NASM_FLAGS="-g"
   jplatform64=$(./jplatform64.sh)-debug;;
  *) OPTLEVEL=" -O2 ";;
 esac

fi
echo "jplatform64=$jplatform64"

USE_LINENOISE="${USE_LINENOISE:=1}"

# gcc 5 vs 4 - killing off linux asm routines (overflow detection)
# new fast code uses builtins not available in gcc 4
# use -DC_NOMULTINTRINSIC to continue to use more standard c in version 4
# too early to move main linux release package to gcc 5

case "$jplatform64" in
	darwin/j64iphoneos)
	 USE_OPENMP=0
	 CC="$(xcrun --sdk iphoneos --find clang)"
	 AR="$(xcrun --sdk iphoneos --find libtool)"
	 macmin="-isysroot $(xcrun --sdk iphoneos --show-sdk-path) -arch arm64";;
	darwin/j64iphonesimulator)
	 USE_OPENMP=0
	 CC="$(xcrun --sdk iphonesimulator --find clang)"
	 AR="$(xcrun --sdk iphonesimulator --find libtool)"
	 macmin="-isysroot $(xcrun --sdk iphonesimulator --show-sdk-path) -arch x86_64";;
	darwin/j64arm)
	 CC="$(xcrun --sdk macosx --find clang)"
	 AR="$(xcrun --sdk macosx --find libtool)"
	 macmin="-isysroot $(xcrun --sdk macosx --show-sdk-path) -arch arm64 -mmacosx-version-min=11";;
	darwin/*)
	 CC="$(xcrun --sdk macosx --find clang)"
	 AR="$(xcrun --sdk macosx --find libtool)"
	 macmin="-isysroot $(xcrun --sdk macosx --show-sdk-path) -arch x86_64 -mmacosx-version-min=10.6";;
	openbsd/*) make=gmake;;
	freebsd/*) make=gmake;;
esac
make="${make:=make}"

CC=${CC-$(which cc clang gcc 2>/dev/null | head -n1 | xargs basename)}
compiler=$(readlink -f $(which $CC) || which $CC)
echo "CC=$CC"
echo "compiler=$compiler"

if [ -z "${compiler##*gcc*}" ] || [ -z "${CC##*gcc*}" ]; then
# gcc
common="$OPENMP -fPIC $OPTLEVEL -fvisibility=hidden -fno-strict-aliasing -flax-vector-conversions \
 -Werror -Wextra -Wno-unknown-warning-option \
 -Wno-cast-function-type \
 -Wno-clobbered \
 -Wno-empty-body \
 -Wno-format-overflow \
 -Wno-implicit-fallthrough \
 -Wno-maybe-uninitialized \
 -Wno-missing-field-initializers \
 -Wno-parentheses \
 -Wno-pointer-sign \
 -Wno-shift-negative-value \
 -Wno-sign-compare \
 -Wno-type-limits \
 -Wno-uninitialized \
 -Wno-unused-parameter \
 -Wno-unused-value \
 $CFLAGS"

else
# clang
common="$OPENMP -fPIC $OPTLEVEL -fvisibility=hidden -fno-strict-aliasing \
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
 -Wno-string-plus-int \
 -Wno-unknown-pragmas \
 -Wno-unsequenced \
 -Wno-unused-function \
 -Wno-unused-parameter \
 -Wno-unused-value \
 -Wno-unused-variable \
 $CFLAGS"

fi

TARGET=jconsole

case "$jplatform64" in
 *32*) USE_EMU_AVX=0;;
  *) USE_EMU_AVX="${USE_EMU_AVX:=1}";;
esac
if [ $USE_EMU_AVX -eq 1 ] ; then
 common="$common -DEMU_AVX2=1"
fi

USE_PYXES="${USE_PYXES:=1}"
if [ $USE_PYXES -eq 1 ] ; then
common="$common -DPYXES=1"
else
common="$common -DPYXES=0"
fi

if [ "$USE_LINENOISE" -ne "1" ] ; then
common="$common -DREADLINE"
else
common="$common -DREADLINE -DUSE_LINENOISE"
OBJSLN="linenoise.o"
fi

if [ "${USE_GMP_H:=1}" -eq 1 ] ; then
 common="$common -I../../../../mpir/include"
fi

case $jplatform64 in

linux/j32)
CFLAGS="$common -m32 -msse2 -mfpmath=sse "
LDFLAGS=" -m32 -ldl $LDTHREAD "
;;
linux/j64*)
CFLAGS="$common"
LDFLAGS=" -ldl $LDTHREAD "
;;
raspberry/j32)
CFLAGS="$common -std=gnu99 -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DRASPI"
LDFLAGS=" -ldl $LDTHREAD "
;;
raspberry/j64)
CFLAGS="$common -march=armv8-a+crc -DRASPI"
LDFLAGS=" -ldl $LDTHREAD "
;;
openbsd/j32)
CFLAGS="$common -m32 -msse2 -mfpmath=sse "
LDFLAGS=" -m32 $LDTHREAD "
;;
openbsd/j64arm)
CFLAGS="$common -march=armv8-a+crc"
LDFLAGS=" $LDTHREAD "
;;
openbsd/j64*)
CFLAGS="$common"
LDFLAGS=" $LDTHREAD "
;;
freebsd/j32)
CFLAGS="$common -m32 -msse2 -mfpmath=sse "
LDFLAGS=" -m32 $LDTHREAD "
;;
freebsd/j64arm)
CFLAGS="$common -march=armv8-a+crc"
LDFLAGS=" $LDTHREAD "
;;
freebsd/j64*)
CFLAGS="$common"
LDFLAGS=" $LDTHREAD "
;;
darwin/j32)
CFLAGS="$common -m32 -msse2 -mfpmath=sse $macmin "
LDFLAGS=" -ldl $LDTHREAD -m32 $macmin "
;;
darwin/j64arm) # darwin arm
CFLAGS="$common $macmin -march=armv8-a+crc "
LDFLAGS=" -Wl,-stack_size,0xc00000 -ldl $LDTHREAD $macmin "
;;
darwin/j64iphoneos) # iphone
CFLAGS="$common $macmin -march=armv8-a+crc "
LDFLAGS=" -Wl,-stack_size,0xc00000 -ldl $LDTHREAD $macmin "
;;
darwin/j64iphoneimulator) # iphone simulator
CFLAGS="$common $macmin "
LDFLAGS=" -Wl,-stack_size,0xc00000 -ldl $LDTHREAD $macmin "
;;
darwin/j64*)
CFLAGS="$common $macmin "
LDFLAGS=" -ldl $LDTHREAD $macmin "
;;
windows/j32)
TARGET=jconsole.exe
CFLAGS="$common -m32 "
LDFLAGS=" -m32 -Wl,--stack=0xc00000,--subsystem,console -static-libgcc $LDTHREAD "
;;
windows/j64*)
TARGET=jconsole.exe
CFLAGS="$common"
LDFLAGS=" -Wl,--stack=0xc00000,--subsystem,console -static-libgcc $LDTHREAD "
;;
*)
echo no case for those parameters
exit
esac

echo "CFLAGS=$CFLAGS"

if [ ! -f ../jsrc/jversion.h ] ; then
  cp ../jsrc/jversion-x.h ../jsrc/jversion.h
fi

mkdir -p ../bin/$jplatform64
mkdir -p obj/$jplatform64
cp makefile-jconsole obj/$jplatform64/.
export CC AR CFLAGS LDFLAGS TARGET OBJSLN jplatform64
cd obj/$jplatform64/
if [ "x$MAKEFLAGS" = x'' ] ; then
 if [ `uname` = Linux ]; then par=`nproc`; else par=`sysctl -n hw.ncpu`; fi
 $make -j$par -f makefile-jconsole all
else
 $make -f makefile-jconsole all
fi
retval=$?
cd -
exit $retval
