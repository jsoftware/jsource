#!/bin/bash

# run by build_jconsole and build_libj
# $1 is j32 or j64

cd ~
. jvars.sh

echo $TARGET
echo $COMPILE
echo $LINK
mkdir -p $jbld/jout/$TARGET/$1/blis
cd $jbld/jout/$TARGET/$1

make -f $jmake/makefile

if [ $1 = "j64nonavx" ] ; then
 if [ $TARGET = "libj.dylib" ] ; then
  cp $TARGET $jbld/j64/bin/libjnonavx.dylib
 else
  cp $TARGET $jbld/j64/bin/libjnonavx.so
 fi
else
 cp $TARGET $jbld/$1/bin
fi
