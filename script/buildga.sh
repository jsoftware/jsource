#!/bin/bash
#
# build linux/macOS on github actions
#
# argument is linux|darwin

if [ "$1" == "linux" ]; then
  ext="so"
elif [ "$1" == "darwin" ]; then
  ext="dylib"
else
  echo "argument is linux|darwin"
  exit 1
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

j64x=j64 USE_PYXES=1 ./build_jconsole.sh
j64x=j64 ./build_tsdll.sh
j64x=j64 USE_PYXES=1 ./build_libj.sh
if [ "$1" == "darwin" ]; then
./clean.sh
j64x=j64arm USE_PYXES=1 ./build_jconsole.sh
j64x=j64arm ./build_tsdll.sh
j64x=j64arm USE_PYXES=1 ./build_libj.sh
fi
./clean.sh
j64x=j64avx USE_PYXES=1 ./build_libj.sh
./clean.sh
j64x=j64avx2 USE_PYXES=1 ./build_libj.sh

cd ..
cp bin/$1/j64/* j64
if [ "$1" == "darwin" ] && [ -f "bin/$1/j64arm/libj.$ext" ]; then
lipo bin/$1/j64/jconsole bin/$1/j64arm/jconsole -create -output j64/jconsole
lipo bin/$1/j64/libtsdll.$ext bin/$1/j64arm/libtsdll.$ext -create -output j64/libtsdll.$ext
lipo bin/$1/j64/libj.$ext bin/$1/j64arm/libj.$ext -create -output j64/libj.$ext
fi
cp bin/$1/j64avx/libj.$ext j64/libjavx.$ext
cp bin/$1/j64avx2/libj.$ext j64/libjavx2.$ext
chmod 644 j64/*
chmod 755 j64/jconsole
