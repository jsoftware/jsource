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

make -f $jmake/makefilewin

if [ $1 = "j64nonavx" ] ; then
 if [ $TARGET = "libj.dylib" ] ; then
  cp $TARGET $jbld/j64/bin/libj-nonavx.dylib
 else
 if [ $TARGET = "libj.so" ] ; then
  cp $TARGET $jbld/j64/bin/libj-nonavx.so
 else
  cp $TARGET $jbld/j64/bin/j-nonavx.dll
 fi
 fi
else
 cp $TARGET $jbld/$1/bin
fi
