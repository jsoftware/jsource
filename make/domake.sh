#!/bin/bash

# run by build_jconsole and build_libj and build_tsdll
# jconsole/tsdll $1 is j32 or j64
# libj           $1 is j32 or j64 or j64avx j64avx2

echo "domake: $1 $TARGET CC=$CC"

if [ $1 = "j32" ] ; then
 bits=j32
else
 bits=j64
fi

cd ~

bld=bld-

OUT=$jbld/jout/$1/bld-$TARGET

mkdir -p $OUT/blis
mkdir -p $OUT/txt
cd       $OUT

echo "        $OUT/txt/build.txt"
echo ""

make -f $jmake/makefile >$jbld/jout/$1/$bld$TARGET/txt/build.txt 2>&1

egrep -w 'warning|error|note' -B 2 $OUT/txt/build.txt

cp $TARGET $jbld/$bits/bin

