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

if [ -z "${compiler##*gcc*}" ] || [ -z "${CC##*gcc*}" ]; then
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
COMPILE="$darwin -m32 $macmin -I$JAVA_HOME/include -I$JAVA_HOME/include/darwin "
LINK=" -m32 $macmin -dynamiclib -o libjnative.dylib "
;;
darwin_j64)
TARGET=libjnative.dylib
COMPILE="$darwin $macmin -I$JAVA_HOME/include -I$JAVA_HOME/include/darwin "
LINK=" $macmin -dynamiclib -o libjnative.dylib "
;;
*)
echo no case for those parameters
exit
esac

OBJS="andjnative.o jeload.o "
export OBJS COMPILE LINK TARGET
$jmake/domake.sh $1

