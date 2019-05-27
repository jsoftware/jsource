#!/bin/bash
# $1 is j32 or j64
cd ~
. jvars.sh

common=" -fPIC -O1 -Wextra "

case $jplatform\_$1 in

linux_j32)
TARGET=jconsole
COMPILE="$common -m32 -DREADLINE"
LINK=" -m32 -ldl -o jconsole "
;;
linux_j64)
TARGET=jconsole
COMPILE="$common -DREADLINE"
LINK=" -ldl -o jconsole "
;;
raspberry_j32)
TARGET=jconsole
COMPILE="$common -marm march=armv6 -mfloat-abi=hard -mfpu=vfp -DREADLINE -DRASPI"
LINK=" -ldl -o jconsole "
;;
raspberry_j64)
COMPILE="$common -march=armv8-a+crc -DREADLINE -DRASPI"
LINK=" -ldl -o jconsole "
;;
darwin_j32)
TARGET=jconsole
COMPILE="$common -m32 -DREADLINE -mmacosx-version-min=10.5"
LINK=" -ldl -m32 -mmacosx-version-min=10.5 -o jconsole "
;;
darwin_j64)
TARGET=jconsole
COMPILE="$common -DREADLINE -mmacosx-version-min=10.5"
LINK=" -ldl -mmacosx-version-min=10.5 -o jconsole "
;;
windows_j32)
TARGET=jconsole.exe
COMPILE="$common "
LINK=" -Wl,--stack=10000000 -static-libgcc -o jconsole.exe "
;;
windows_j64)
TARGET=jconsole.exe
COMPILE="$common "
LINK=" -Wl,--stack=10000000 -static-libgcc -o jconsole.exe "
;;
*)
echo no case for those parameters
exit
esac

OBJS="jconsole.o jeload.o "
export OBJS COMPILE LINK TARGET
$jmake/domake.sh $1

