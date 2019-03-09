#!/bin/bash
# $1 is j32 or j64
cd ~

common=" -fPIC -O1 -Wextra -Wno-unused-parameter "

macmin="-mmacosx-version-min=10.6"

case $jplatform\_$1 in

linux_j32)
if [ -z "$USE_LINENOISE" ] ; then
COMPILE="$common -m32 -DREADLINE"
LINK=" -l:libedit.so.2 -m32 -ldl -o jconsole "
else
COMPILE="$common -m32 -DREADLINE -DUSE_LINENOISE"
LINK=" -m32 -ldl -o jconsole "
OBJSLN="linenoise.o"
fi
;;
linux_j64)
if [ -z "$USE_LINENOISE" ] ; then
COMPILE="$common -DREADLINE"
LINK=" -ledit -ldl -o jconsole "
else
COMPILE="$common -DREADLINE -DUSE_LINENOISE"
LINK=" -ldl -o jconsole "
OBJSLN="linenoise.o"
fi
;;
raspberry_j32)
if [ -z "$USE_LINENOISE" ] ; then
COMPILE="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DREADLINE -DRASPI"
LINK=" -ledit -ldl -o jconsole "
else
COMPILE="$common -marm -march=armv6 -mfloat-abi=hard -mfpu=vfp -DREADLINE -DUSE_LINENOISE -DRASPI"
LINK=" -ldl -o jconsole "
OBJSLN="linenoise.o"
fi
;;
raspberry_j64)
if [ -z "$USE_LINENOISE" ] ; then
COMPILE="$common -march=armv8-a+crc -DREADLINE -DRASPI"
LINK=" -ledit -ldl -o jconsole "
else
COMPILE="$common -march=armv8-a+crc -DREADLINE -DUSE_LINENOISE -DRASPI"
LINK=" -ldl -o jconsole "
OBJSLN="linenoise.o"
fi
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

OBJS="jconsole.o jeload.o ${OBJSLN}"
TARGET=jconsole
export OBJS COMPILE LINK TARGET
$jmake/domake.sh $1

