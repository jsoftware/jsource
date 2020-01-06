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
common="-Werror -fPIC -O2 -fwrapv -fno-strict-aliasing -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-clobbered -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-type-limits"
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
common="-Werror -fPIC -O2 -fwrapv -fno-strict-aliasing -Wextra -Wno-consumed -Wno-uninitialized -Wno-unused-parameter -Wno-sign-compare -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-unsequenced -Wno-string-plus-int -Wno-tautological-constant-out-of-range-compare"
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
darwin="-fPIC -O2 -fwrapv -fno-strict-aliasing -Wno-string-plus-int -Wno-empty-body -Wno-unsequenced -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-return-type -Wno-constant-logical-operand -Wno-comment -Wno-unsequenced -Wno-pass-failed"

case $jplatform\_$1 in

linux_j32)
if [ "$USE_LINENOISE" -ne "1" ] ; then
COMPILE="$common -m32 -DREADLINE"
LINK=" -m32 -ldl -o jconsole "
else
COMPILE="$common -m32 -DREADLINE -DUSE_LINENOISE"
LINK=" -m32 -ldl -o jconsole "
OBJSLN="linenoise.o"
fi
;;
linux_j64)
if [ "$USE_LINENOISE" -ne "1" ] ; then
COMPILE="$common -DREADLINE"
LINK=" -ldl -o jconsole "
else
COMPILE="$common -DREADLINE -DUSE_LINENOISE"
LINK=" -ldl -o jconsole "
OBJSLN="linenoise.o"
fi
;;
raspberry_j32)
if [ "$USE_LINENOISE" -ne "1" ] ; then
COMPILE="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DREADLINE -DRASPI"
LINK=" -ldl -o jconsole "
else
COMPILE="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DREADLINE -DUSE_LINENOISE -DRASPI"
LINK=" -ldl -o jconsole "
OBJSLN="linenoise.o"
fi
;;
raspberry_j64)
if [ "$USE_LINENOISE" -ne "1" ] ; then
COMPILE="$common -march=armv8-a+crc -DREADLINE -DRASPI"
LINK=" -ldl -o jconsole "
else
COMPILE="$common -march=armv8-a+crc -DREADLINE -DUSE_LINENOISE -DRASPI"
LINK=" -ldl -o jconsole "
OBJSLN="linenoise.o"
fi
;;
darwin_j32)
if [ "$USE_LINENOISE" -ne "1" ] ; then
COMPILE="$darwin -m32 -DREADLINE $macmin"
LINK=" -ldl -m32 $macmin -o jconsole "
else
COMPILE="$darwin -m32 -DREADLINE -DUSE_LINENOISE $macmin"
LINK=" -ldl -m32 $macmin -o jconsole "
OBJSLN="linenoise.o"
fi
;;
#-mmacosx-version-min=10.5
darwin_j64)
if [ "$USE_LINENOISE" -ne "1" ] ; then
COMPILE="$darwin -DREADLINE $macmin"
LINK=" -ldl $macmin -o jconsole "
else
COMPILE="$darwin -DREADLINE -DUSE_LINENOISE $macmin"
LINK=" -ldl $macmin -o jconsole "
OBJSLN="linenoise.o"
fi
;;
*)
echo no case for those parameters
exit
esac

OBJS="jconsole.o jeload.o ${OBJSLN}"
TARGET=jconsole
export OBJS COMPILE LINK TARGET
$jmake/domake.sh $1

