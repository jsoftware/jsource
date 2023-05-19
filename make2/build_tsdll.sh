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

# gcc 5 vs 4 - killing off linux asm routines (overflow detection)
# new fast code uses builtins not available in gcc 4
# use -DC_NOMULTINTRINSIC to continue to use more standard c in version 4
# too early to move main linux release package to gcc 5

case "$jplatform64" in
	darwin/j64iphoneos)
	 CC="$(xcrun --sdk iphoneos --find clang)"
	 AR="$(xcrun --sdk iphoneos --find libtool)"
	 macmin="-isysroot $(xcrun --sdk iphoneos --show-sdk-path) -arch arm64";;
	darwin/j64iphonesimulator)
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
common="$OPENMP -fPIC $OPTLEVEL -fvisibility=hidden -fno-strict-aliasing  \
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

case $jplatform64 in

linux/j32) # linux x86
TARGET=libtsdll.so
# faster, but sse2 not available for 32-bit amd cpu
# sse does not support mfpmath=sse in 32-bit gcc
CFLAGS="$common -m32 -msse2 -mfpmath=sse "
# slower, use 387 fpu and truncate extra precision
# CFLAGS="$common -m32 -ffloat-store "
LDFLAGS=" -shared -Wl,-soname,libtsdll.so -m32 -lm -ldl "
;;

linux/j64*) # linux intel 64bit
TARGET=libtsdll.so
CFLAGS="$common "
LDFLAGS=" -shared -Wl,-soname,libtsdll.so -lm -ldl "
;;

raspberry/j32) # linux raspbian arm
TARGET=libtsdll.so
CFLAGS="$common -std=gnu99 -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DRASPI "
LDFLAGS=" -shared -Wl,-soname,libtsdll.so -lm -ldl "
;;

raspberry/j64) # linux arm64
TARGET=libtsdll.so
CFLAGS="$common -march=armv8-a+crc -DRASPI "
LDFLAGS=" -shared -Wl,-soname,libtsdll.so -lm -ldl "
;;

openbsd/j32) # openbsd x86
TARGET=libtsdll.so
CFLAGS="$common "
LDFLAGS=" -shared -Wl,-soname,libtsdll.so -lm "
;;

openbsd/j64arm) # openbsd arm64
TARGET=libtsdll.so
CFLAGS="$common -march=armv8-a+crc "
LDFLAGS=" -shared -Wl,-soname,libtsdll.so -lm "
;;

openbsd/j64*) # openbsd intel 64bit nonavx
TARGET=libtsdll.so
CFLAGS="$common "
LDFLAGS=" -shared -Wl,-soname,libtsdll.so -lm "
;;

freebsd/j32) # freebsd x86
TARGET=libtsdll.so
CFLAGS="$common "
LDFLAGS=" -shared -Wl,-soname,libtsdll.so -lm "
;;

freebsd/j64arm) # freebsd arm64
TARGET=libtsdll.so
CFLAGS="$common -march=armv8-a+crc "
LDFLAGS=" -shared -Wl,-soname,libtsdll.so -lm "
;;

freebsd/j64*) # freebsd intel 64bit nonavx
TARGET=libtsdll.so
CFLAGS="$common "
LDFLAGS=" -shared -Wl,-soname,libtsdll.so -lm "
;;

darwin/j32) # darwin x86
TARGET=libtsdll.dylib
CFLAGS="$common -m32 -msse2 -mfpmath=sse $macmin"
LDFLAGS=" -dynamiclib -install_name libtsdll.dylib -lm -ldl -m32 $macmin "
;;

darwin/j64arm) # darwin arm
TARGET=libtsdll.dylib
CFLAGS="$common $macmin -march=armv8-a+crc "
LDFLAGS=" -dynamiclib -install_name libtsdll.dylib -lm -ldl $macmin  "
;;

darwin/j64iphoneos) # iphone
TARGET_a=libtsdll.a
CFLAGS="$common $macmin -march=armv8-a+crc "
LDFLAGS=" -dynamiclib -install_name libtsdll.dylib -lm $macmin  "
LDFLAGS_a=" -static -o "
;;

darwin/j64iphonesimulator) # iphone simulator
TARGET_a=libtsdll.a
CFLAGS="$common $macmin "
LDFLAGS=" -dynamiclib -install_name libtsdll.dylib -lm $macmin "
LDFLAGS_a=" -static -o "
;;

darwin/j64*) # darwin intel 64bit
TARGET=libtsdll.dylib
CFLAGS="$common $macmin"
LDFLAGS=" -dynamiclib -install_name libtsdll.dylib -lm -ldl $macmin "
;;

*)
echo no case for those parameters
exit
esac

echo "CFLAGS=$CFLAGS"

mkdir -p ../bin/$jplatform64
mkdir -p obj/$jplatform64/
cp makefile-tsdll obj/$jplatform64/.
export CC AR CFLAGS LDFLAGS LDFLAGS_a LDFLAGS_b TARGET TARGET_a jplatform64
cd obj/$jplatform64/
if [ "x$MAKEFLAGS" = x'' ] ; then
 if [ `uname` = Linux ]; then par=`nproc`; else par=`sysctl -n hw.ncpu`; fi
 $make -j$par -f makefile-tsdll all
else
 $make -f makefile-tsdll all
fi
retval=$?
cd -
exit $retval
