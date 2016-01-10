#!/bin/bash

# run by build_jconsole and build_libj
# $1 is j32 or j64

cd ~
. jvars.sh

echo $TARGET
echo $COMPILE
echo $LINK
mkdir -p $jbld/jout/$TARGET/$1
cd $jbld/jout/$TARGET/$1

make -f $jmake/makefile
cp $TARGET $jbld/$1/bin
