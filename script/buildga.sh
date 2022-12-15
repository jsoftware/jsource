#!/bin/bash
#
# build linux/macOS on github actions
#
# argument is linux|darwin|raspberry|android

set -e
CC=${CC-clang}
USE_SLEEF=${USE_SLEEF-1}
_DEBUG=3
export CC USE_SLEEF _DEBUG

if [ "$1" == "linux" ]; then
  ext="so"
elif [ "$1" == "raspberry" ]; then
  ext="so"
elif [ "$1" == "darwin" ]; then
  ext="dylib"
elif [ "$1" == "android" ]; then
  ext="so"
else
  echo "argument is linux|darwin|raspberry|android"
  exit 1
fi
if [ "`uname -m`" != "armv6l" ]; then
 m64=1
else
 m64=0
fi

cp -R jlibrary/* .
cp script/testga.ijs .
cp script/ver.ijs .

if [ $m64 -eq 1 ]; then
mkdir -p j64
cp bin/profile.ijs j64
else
mkdir -p j32
cp bin/profile.ijs j32
fi
if [ "$1" == "linux" ]; then
cp mpir/linux/x86_64/libgmp.so j64
elif [ "$1" == "raspberry" ]; then
if [ $m64 -eq 1 ]; then
cp mpir/linux/aarch64/libgmp.so j64
else
cp mpir/linux/arm/libgmp.so j32
fi
elif [ "$1" == "darwin" ]; then
cp mpir/apple/macos/libgmp.dylib j64
fi

cp version.txt jsrc/jversion.h
echo "#define jplatform \"$1\"" >> jsrc/jversion.h
echo "#define jlicense  \"commercial\"" >> jsrc/jversion.h
echo "#define jbuilder  \"www.jsoftware.com\"" >> jsrc/jversion.h

if [ "x$MAKEFLAGS" = x'' ] ; then
if [ "$1" == "linux" ] || [ "$1" == "raspberry" ] ; then par=`nproc`; else par=`sysctl -n hw.ncpu`; fi
export MAKEFLAGS=-j$par
fi
echo "MAKEFLAGS=$MAKEFLAGS"

if [ "$1" == "android" ]; then
export _DEBUG=0
cd android/jni
ln -sf ../../hostdefs .
ln -sf ../../jsrc .
ln -sf ../../netdefs .
cd ../..
cd android
ndk-build
cd ..
exit 0
fi

cd make2

if [ "$1" == "darwin" ]; then
./clean.sh
j64x=j64arm USE_PYXES=1 ./build_jconsole.sh
j64x=j64arm ./build_tsdll.sh
j64x=j64arm USE_PYXES=1 ./build_libj.sh
elif [ "$1" == "linux" ]; then
./clean.sh
j64x=j32 USE_PYXES=0 ./build_jconsole.sh
j64x=j32 ./build_tsdll.sh
j64x=j32 USE_PYXES=0 ./build_libj.sh
fi
if [ $m64 -eq 1 ]; then
./clean.sh
j64x=j64 USE_PYXES=1 ./build_jconsole.sh
j64x=j64 ./build_tsdll.sh
j64x=j64 USE_PYXES=1 ./build_libj.sh
else
j64x=j32 USE_PYXES=0 ./build_jconsole.sh
j64x=j32 ./build_tsdll.sh
j64x=j32 USE_PYXES=0 ./build_libj.sh
fi

cd ..
if [ $m64 -eq 1 ]; then
cp bin/$1/j64/* j64
else
cp bin/$1/j32/* j32
fi
if [ "$1" == "darwin" ] && [ -f "bin/$1/j64arm/libj.$ext" ]; then
lipo bin/$1/j64/jconsole bin/$1/j64arm/jconsole -create -output j64/jconsole
lipo bin/$1/j64/libtsdll.$ext bin/$1/j64arm/libtsdll.$ext -create -output j64/libtsdll.$ext
lipo bin/$1/j64/libj.$ext bin/$1/j64arm/libj.$ext -create -output j64/libj.$ext
fi
if [ $m64 -eq 1 ]; then
chmod 644 j64/*
chmod 755 j64/jconsole
else
chmod 644 j32/*
chmod 755 j32/jconsole
fi

