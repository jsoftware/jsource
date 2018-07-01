#!/bin/bash

# run by build_jconsole and build_libj
# $1 is j32 or j64

cd ~

mkdir -p $jbld/jout/$TARGET/$1/blis
cd $jbld/jout/$TARGET/$1

targ=$1
avx=""
if [ $1 = "j64nonavx" ] ; then
 targ=j64
 avx=-nonavx
fi 

echo "building  $jbld/$targ/bin/$TARGET $avx"
echo "output in $jbld/$targ/bin/build_$TARGET$avx.txt"
make -f $jmake/makefile >$jbld/$targ/bin/build_$TARGET$avx.txt 2>&1
echo `egrep -w 'warning|error|note' $jbld/$targ/bin/build_$TARGET$avx.txt`

if [ $1 = "j64nonavx" ] ; then
 if [ $TARGET = "libj.dylib" ] ; then
  cp $TARGET $jbld/j64/bin/libj-nonavx.dylib
 else
  cp $TARGET $jbld/j64/bin/libj-nonavx.so
 fi
else
 cp $TARGET $jbld/$1/bin
fi
