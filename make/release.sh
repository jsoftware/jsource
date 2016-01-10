#!/bin/bash
# copy binaries to release folder
cd ~
. jvars.sh

mkdir -p $jgit/release/$jplatform/j32
rm    -f $jgit/release/$jplatform/j32/*
cp $jbld/j32/bin/jconsole       $jgit/release/$jplatform/j32/jconsole
cp $jbld/j32/bin/libj.so        $jgit/release/$jplatform/j32/libj.so
cp $jbld/j32/bin/libj.dylib     $jgit/release/$jplatform/j32/libj.dylib
cp $jbld/j32/bin/libtsdll.so    $jgit/release/$jplatform/j32/libtsdll.so
cp $jbld/j32/bin/libtsdll.dylib $jgit/release/$jplatform/j32/libtsdll.dylib


if [ $jplatform = "raspberry" ]; then
  exit 0
fi

mkdir -p $jgit/release/$jplatform/j64
rm    -f $jgit/release/$jplatform/j64/*
cp $jbld/j64/bin/jconsole       $jgit/release/$jplatform/j64/jconsole
cp $jbld/j64/bin/libj.so        $jgit/release/$jplatform/j64/libj.so
cp $jbld/j64/bin/libj.dylib     $jgit/release/$jplatform/j64/libj.dylib
cp $jbld/j64/bin/libtsdll.so    $jgit/release/$jplatform/j64/libtsdll.so
cp $jbld/j64/bin/libtsdll.dylib $jgit/release/$jplatform/j64/libtsdll.dylib

