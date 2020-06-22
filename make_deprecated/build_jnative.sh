#!/bin/bash
# $1 is j32 or j64
cd ~

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
compiler=`readlink -f $(command -v $CC)`
echo "CC=$CC"
echo "compiler=$compiler"

if [ $CC = "gcc" ] ; then
# gcc
common="-Werror -fPIC -O2 -fvisibility=hidden -fwrapv -fno-strict-aliasing -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-clobbered -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-type-limits"
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
common="-Werror -fPIC -O2 -fvisibility=hidden -fwrapv -fno-strict-aliasing -Wextra -Wno-consumed -Wuninitialized -Wno-unused-parameter -Wsign-compare -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wunsequenced -Wno-string-plus-int -Wtautological-constant-out-of-range-compare"
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

case $jplatform\_$1 in

linux_j32)
TARGET=libjnative.so
COMPILE="$common -m32 -I$JAVA_HOME/include -I$JAVA_HOME/include/linux "
LINK=" -shared -Wl,-soname,libjnative.so  -m32 -o libjnative.so "
;;
linux_j64)
TARGET=libjnative.so
COMPILE="$common -I$JAVA_HOME/include -I$JAVA_HOME/include/linux "
LINK=" -shared -Wl,-soname,libjnative.so -o libjnative.so "
;;
raspberry_j32)
TARGET=libjnative.so
COMPILE="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -I$JAVA_HOME/include -I$JAVA_HOME/include/linux "
LINK=" -shared -Wl,-soname,libjnative.so -o libjnative.so "
;;
raspberry_j64)
TARGET=libjnative.so
COMPILE="$common -march=armv8-a+crc -I$JAVA_HOME/include -I$JAVA_HOME/include/linux "
LINK=" -shared -Wl,-soname,libjnative.so -o libjnative.so "
;;
darwin_j32)
TARGET=libjnative.dylib
COMPILE="$common -m32 $macmin -I$JAVA_HOME/include -I$JAVA_HOME/include/darwin "
LINK=" -m32 $macmin -dynamiclib -o libjnative.dylib "
;;
darwin_j64)
TARGET=libjnative.dylib
COMPILE="$common $macmin -I$JAVA_HOME/include -I$JAVA_HOME/include/darwin "
LINK=" $macmin -dynamiclib -o libjnative.dylib "
;;
*)
echo no case for those parameters
exit
esac

OBJS="andjnative.o jeload.o "
export OBJS COMPILE LINK TARGET
$jmake/domake.sh $1

