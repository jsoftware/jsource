#!/bin/bash

# run by build_jconsole and build_libj
# $1 is j32 or j64

echo $TARGET
echo $COMPILE
echo $LINK
mkdir -p $jbld/jout/$TARGET/$1/blis
cd $jbld/jout/$TARGET/$1

make -f $jmake/makefile

if [ $1 = "j64nonavx" ] ; then
 if [ $TARGET = "libj.dylib" ] ; then
  cp $TARGET $jbld/j64/bin/libj-nonavx.dylib
 else
  cp $TARGET $jbld/j64/bin/libj-nonavx.so
 fi
else
 cp $TARGET $jbld/$1/bin
fi
