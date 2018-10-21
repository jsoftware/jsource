#!/bin/bash
# $1 is j32 or j64
cd ~

common=" -fPIC -O1 -Werror -Wextra -Wno-unused-parameter"

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
COMPILE="$common -m32 -I$JAVA_HOME/include "
LINK=" -m32 -dynamiclib -o libjnative.dylib "
;;
darwin_j64)
TARGET=libjnative.dylib
COMPILE="$common -I$JAVA_HOME/include "
LINK=" -dynamiclib -o libjnative.dylib "
;;
*)
echo no case for those parameters
exit
esac

OBJS="andjnative.o jeload.o "
export OBJS COMPILE LINK TARGET
$jmake/domake.sh $1

