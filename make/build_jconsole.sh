#!/bin/bash
# $1 is j32 or j64
cd ~
. jvars.sh

common=" -fPIC -O1 -Wextra "

case $jplatform\_$1 in

linux_j32)
COMPILE="$common -m32 -DREADLINE"
LINK=" $jgit/unix/libedit.a -m32 -ldl -l:libncursesw.so.5.9 -l:libtinfo.so.5.9 -o jconsole "
;;
linux_j64)
COMPILE="$common -DREADLINE"
LINK=" $jgit/unix/libedit64.a -ldl -lncursesw -o jconsole "
;;
raspberry_j32)
COMPILE="$common -DREADLINE -DRASPI"
LINK=" -ledit -ldl -o jconsole "
;;
darwin_j32)
COMPILE="$common -m32 -DREADLINE -mmacosx-version-min=10.5"
LINK=" -ledit -ldl -lncurses -m32 -mmacosx-version-min=10.5 -o jconsole "
;;
darwin_j64)
COMPILE="$common -DREADLINE -mmacosx-version-min=10.5"
LINK=" -ledit -ldl -lncurses -mmacosx-version-min=10.5 -o jconsole "
;;
*)
echo no case for those parameters
exit
esac

OBJS="jconsole.o jeload.o "
TARGET=jconsole
export OBJS COMPILE LINK TARGET
$jmake/domake.sh $1

