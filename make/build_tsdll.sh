#!/bin/bash
# $1 is j32 or j64

common=" -fPIC -O1 -Werror -Wextra -Wno-unused-parameter"

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

OBJS="tsdll.o "
export OBJS COMPILE LINK TARGET
$jmake/domake.sh $1

