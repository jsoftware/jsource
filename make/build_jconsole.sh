#!/bin/bash
# $1 is j32 or j64
cd ~

common=" -fPIC -O1 -Wextra -Wno-unused-parameter "

macmin="-mmacosx-version-min=10.6"

case $jplatform\_$1 in

linux_j32)
COMPILE="$common -m32 -DREADLINE"
LINK=" -l:libedit.so.2 -m32 -ldl -o jconsole "
;;
linux_j64)
COMPILE="$common -DREADLINE"
LINK=" -ledit -ldl -o jconsole "
;;
raspberry_j32)
COMPILE="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DREADLINE -DRASPI"
LINK=" -ledit -ldl -o jconsole "
;;
raspberry_j64)
COMPILE="$common -march=armv8-a+crc -DREADLINE -DRASPI"
LINK=" -ledit -ldl -o jconsole "
;;
darwin_j32)
COMPILE="$common -m32 -DREADLINE $macmin"
LINK=" -ledit -ldl -lncurses -m32 $macmin -o jconsole "
;;
#-mmacosx-version-min=10.5
darwin_j64)
COMPILE="$common -DREADLINE $macmin"
LINK=" -ledit -ldl -lncurses $macmin -o jconsole "
;;
*)
echo no case for those parameters
exit
esac

OBJS="jconsole.o jeload.o "
TARGET=jconsole
export OBJS COMPILE LINK TARGET
$jmake/domake.sh $1

