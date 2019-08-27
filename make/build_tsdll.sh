#!/bin/bash
# $1 is j32 or j64
# jvars.sh exports CC as gcc or clang

cd ~

macmin="-mmacosx-version-min=10.6"

if [ $CC = "gcc" ] ; then
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
TARGET=libtsdll.so
COMPILE="$common -m32 "
LINK=" -shared -Wl,-soname,libtsdll.so  -m32 -o libtsdll.so -lm "
;;
linux_j64)
TARGET=libtsdll.so
COMPILE="$common "
LINK=" -shared -Wl,-soname,libtsdll.so -o libtsdll.so -lm "
;;
raspberry_j32)
TARGET=libtsdll.so
COMPILE="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp"
LINK=" -shared -Wl,-soname,libtsdll.so -o libtsdll.so -lm "
;;
raspberry_j64)
TARGET=libtsdll.so
COMPILE="$common -march=armv8-a+crc "
LINK=" -shared -Wl,-soname,libtsdll.so -o libtsdll.so -lm "
;;
darwin_j32)
TARGET=libtsdll.dylib
COMPILE="$darwin -m32 $macmin"
LINK=" -m32 $macmin -dynamiclib -o libtsdll.dylib -lm "
;;
darwin_j64)
TARGET=libtsdll.dylib
COMPILE="$common $macmin"
LINK=" $macmin -dynamiclib -o libtsdll.dylib -lm "
;;
*)
echo no case for those parameters
exit
esac

OBJS="tsdll.o "
export OBJS COMPILE LINK TARGET
$jmake/domake.sh $1

