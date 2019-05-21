#!/bin/bash
# $1 is j32 or j64
cd ~

macmin="-mmacosx-version-min=10.6"
USE_LINENOISE="${USE_LINENOISE:=0}"

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

if [ -z "${compiler##*gcc*}" ]; then
# gcc
common=" -fPIC -O1 -fwrapv -fno-strict-aliasing -Wextra -Wno-maybe-uninitialized -Wno-unused-parameter -Wno-sign-compare -Wno-clobbered -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses"
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
common=" -Werror -fPIC -O1 -fwrapv -fno-strict-aliasing -Wextra -Wno-consumed -Wno-uninitialized -Wno-unused-parameter -Wno-sign-compare -Wno-empty-body -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-unsequenced -Wno-string-plus-int"
fi
darwin=" -fPIC -O1 -fwrapv -fno-strict-aliasing -Wno-string-plus-int -Wno-empty-body -Wno-unsequenced -Wno-unused-value -Wno-pointer-sign -Wno-parentheses -Wno-return-type -Wno-constant-logical-operand -Wno-comment -Wno-unsequenced"

case $jplatform\_$1 in

linux_j32)
if [ "$USE_LINENOISE" -ne "1" ] ; then
COMPILE="$common -m32 -DREADLINE"
LINK=" -l:libedit.so.2 -m32 -ldl -o jconsole "
else
COMPILE="$common -m32 -DREADLINE -DUSE_LINENOISE"
LINK=" -m32 -ldl -o jconsole "
OBJSLN="linenoise.o"
fi
;;
linux_j64)
if [ "$USE_LINENOISE" -ne "1" ] ; then
COMPILE="$common -DREADLINE"
LINK=" -ledit -ldl -o jconsole "
else
COMPILE="$common -DREADLINE -DUSE_LINENOISE"
LINK=" -ldl -o jconsole "
OBJSLN="linenoise.o"
fi
;;
raspberry_j32)
if [ "$USE_LINENOISE" -ne "1" ] ; then
COMPILE="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DREADLINE -DRASPI"
LINK=" -ledit -ldl -o jconsole "
else
COMPILE="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DREADLINE -DUSE_LINENOISE -DRASPI"
LINK=" -ldl -o jconsole "
OBJSLN="linenoise.o"
fi
;;
raspberry_j64)
if [ "$USE_LINENOISE" -ne "1" ] ; then
COMPILE="$common -march=armv8-a+crc -DREADLINE -DRASPI"
LINK=" -ledit -ldl -o jconsole "
else
COMPILE="$common -march=armv8-a+crc -DREADLINE -DUSE_LINENOISE -DRASPI"
LINK=" -ldl -o jconsole "
OBJSLN="linenoise.o"
fi
;;
darwin_j32)
COMPILE="$darwin -m32 -DREADLINE $macmin"
LINK=" -ledit -ldl -lncurses -m32 $macmin -o jconsole "
;;
#-mmacosx-version-min=10.5
darwin_j64)
COMPILE="$darwin -DREADLINE $macmin"
LINK=" -ledit -ldl -lncurses $macmin -o jconsole "
;;
*)
echo no case for those parameters
exit
esac

OBJS="jconsole.o jeload.o ${OBJSLN}"
TARGET=jconsole
export OBJS COMPILE LINK TARGET
$jmake/domake.sh $1

