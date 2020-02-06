#!/bin/bash
# $1 is j32 or j64
# jvars.sh exports CC as gcc or clang

cd ~

macmin="-mmacosx-version-min=10.6"
USE_LINENOISE="${USE_LINENOISE:=1}"

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

if [ "$USE_LINENOISE" -ne "1" ] ; then
common="$common -DREADLINE"
else
common="$common -DREADLINE -DUSE_LINENOISE"
OBJSLN="linenoise.o"
fi

case $jplatform\_$1 in

linux_j32)
COMPILE="$common -m32"
LINK=" -m32 -ldl -o jconsole "
;;
linux_j64)
COMPILE="$common"
LINK=" -ldl -o jconsole "
;;
raspberry_j32)
COMPILE="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DRASPI"
LINK=" -ldl -o jconsole "
;;
raspberry_j64)
COMPILE="$common -march=armv8-a+crc -DRASPI"
LINK=" -ldl -o jconsole "
;;
darwin_j32)
COMPILE="$common -m32 $macmin"
LINK=" -ldl -m32 $macmin -o jconsole "
;;
#-mmacosx-version-min=10.5
darwin_j64)
COMPILE="$common $macmin"
LINK=" -ldl $macmin -o jconsole "
;;
*)
echo no case for those parameters
exit
esac

OBJS="jconsole.o jeload.o ${OBJSLN}"
TARGET=jconsole
export OBJS COMPILE LINK TARGET
$jmake/domake.sh $1

