#!/bin/bash
#
# build linux/macOS on github actions
#
# argument is linux|darwin

if [ "$1" == "linux" ]; then
  ext="so"
else
  ext="dylib"
fi

cp -R jlibrary/* .
cp script/testga.ijs .
cp script/ver.ijs .

mkdir -p j64
cp bin/profile.ijs j64

cp version.txt jsrc/jversion.h
echo "#define jplatform \"$1\"" >> jsrc/jversion.h
echo "#define jlicense  \"commercial\"" >> jsrc/jversion.h
echo "#define jbuilder  \"www.jsoftware.com\"" >> jsrc/jversion.h

cd make2
./clean.sh

j64x=j64 ./build_jconsole.sh
j64x=j64 ./build_tsdll.sh
j64x=j64 ./build_libj.sh
./clean.sh
j64x=j64avx ./build_libj.sh
./clean.sh
j64x=j64avx2 ./build_libj.sh

cd ..
cp bin/$1/j64/* j64
cp bin/$1/j64avx/libj.$ext j64/libjavx.$ext
cp bin/$1/j64avx2/libj.$ext j64/libjavx2.$ext
chmod 644 j64/*
chmod 755 j64/jconsole
