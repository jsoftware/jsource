#!/bin/bash
# $1 is j32 or j64
cd ~

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
compiler=`$CC --version | head -n 1`
echo "CC=$CC"
echo "compiler=$compiler"

common=" -fPIC -O1 -Werror -Wextra -Wno-unused-parameter"

if [ -z "${compiler##*gcc*}" ]; then
# gcc
# alternatively, add comment /* fall through */
OVER_GCC_VER7=$(echo `$CC -dumpversion | cut -f1 -d.` \>= 7 | bc)
if [ $OVER_GCC_VER7 -eq 1 ] ; then
common="$common -Wno-implicit-fallthrough"
fi
OVER_GCC_VER8=$(echo `$CC -dumpversion | cut -f1 -d.` \>= 8 | bc)
if [ $OVER_GCC_VER8 -eq 1 ] ; then
common="$common -Wno-cast-function-type"
fi
fi

case $jplatform\_$1 in

linux_j32)
TARGET=libtsdll.so
COMPILE="$common -m32 "
LINK=" -shared -Wl,-soname,libtsdll.so  -m32 -o libtsdll.so "
;;
linux_j64)
TARGET=libtsdll.so
COMPILE="$common "
LINK=" -shared -Wl,-soname,libtsdll.so -o libtsdll.so "
;;
raspberry_j32)
TARGET=libtsdll.so
COMPILE="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp"
LINK=" -shared -Wl,-soname,libtsdll.so -o libtsdll.so "
;;
raspberry_j64)
TARGET=libtsdll.so
COMPILE="$common -march=armv8-a+crc "
LINK=" -shared -Wl,-soname,libtsdll.so -o libtsdll.so "
;;
darwin_j32)
TARGET=libtsdll.dylib
COMPILE="$common -m32 "
LINK=" -m32 -dynamiclib -o libtsdll.dylib "
;;
darwin_j64)
TARGET=libtsdll.dylib
COMPILE="$common "
LINK=" -dynamiclib -o libtsdll.dylib "
;;
*)
echo no case for those parameters
exit
esac

echo "COMPILE=$COMPILE"

OBJS="tsdll.o "
export OBJS COMPILE LINK TARGET
$jmake/domake.sh $1

