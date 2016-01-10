#!/bin/bash
# $1 is j32 or j64
cd ~
. jvars.sh

case $jplatform\_$1 in

linux_j32)
TARGET=libtsdll.so
COMPILE=" -fPIC -O1 -m32 -DC_64=0 "
LINK=" -shared -Wl,-soname,libtsdll.so  -m32 -o libtsdll.so "
;;
linux_j64)
TARGET=libtsdll.so
COMPILE=" -fPIC -O1 "
LINK=" -shared -Wl,-soname,libtsdll.so  -o libtsdll.so "
;;
raspberry_j32)
TARGET=libtsdll.so
COMPILE=" -fPIC -O1 -DC_64=0 "
LINK=" -shared -Wl,-soname,libtsdll.so -o libtsdll.so "
;;
darwin_j32)
TARGET=libtsdll.dylib
COMPILE=" -fPIC -O3 -m32 -DC_64=0 "
LINK=" -m32 -dynamiclib -o libtsdll.dylib "
;;
darwin_j64)

TARGET=libtsdll.dylib
COMPILE=" -fPIC -O3 "
LINK=" -dynamiclib -o libtsdll.dylib "
;;
*)
echo no case for those parameters
exit
esac

OBJS="tsdll.o "
export OBJS COMPILE LINK TARGET
$jmake/domake.sh $1

