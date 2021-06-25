#!/bin/bash
#
# build linux/macOS on github actions
#
# argument is linux|darwin

if [ $1 == "linux" ]; then
  ext="so"
  s() { sed -i "$@"; }
else
  ext="dylib"
  s() { sed -i "" "$@"; }
fi

cp script/jversion.h jsource/jsrc
s "s/windows/$1/" jsource/jsrc/jversion.h
cat jsource/jsrc/jversion.h
cd jsource/make2
./clean.sh

j64x=j64 ./build_jconsole.sh
j64x=j64 ./build_tsdll.sh
j64x=j64 ./build_libj.sh
./clean.sh
j64x=j64avx ./build_libj.sh
./clean.sh
j64x=j64avx2 ./build_libj.sh

cd ../..
D=j64
mkdir -p $D
cp jsource/bin/$1/j64/* $D
cp jsource/bin/$1/j64avx/libj.$ext $D/libjavx.$ext
cp jsource/bin/$1/j64avx2/libj.$ext $D/libjavx2.$ext
cp script/ver.ijs $D
chmod 644 $D/*
chmod 755 $D/jconsole
