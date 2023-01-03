#!/bin/sh
#
# build linux/macOS on github actions
#
# argument is linux|darwin|raspberry|android|openbsd|freebsd
# openbsd is experimental

set -e
CC=${CC-clang}
USE_SLEEF=${USE_SLEEF-1}
export CC USE_SLEEF

if [ "$1" = "linux" ]; then
  ext="so"
  export _DEBUG=3
elif [ "$1" = "raspberry" ]; then
  ext="so"
elif [ "$1" = "darwin" ]; then
  ext="dylib"
  export _DEBUG=3
elif [ "$1" = "android" ]; then
  ext="so"
elif [ "$1" = "openbsd" ]; then
  ext="so"
elif [ "$1" = "freebsd" ]; then
  ext="so"
else
  echo "argument is linux|darwin|raspberry|android|openbsd|freebsd"
  exit 1
fi
if [ "`uname -m`" != "armv6l" ] && [ "`uname -m`" != "i386" ] && [ "`uname -m`" != "i686" ] ; then
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
if [ "$1" = "linux" ]; then
# cp mpir/linux/x86_64/libgmp.so j64
cp mpir/linux/x86_64/libgmpd.so j64/libgmp.so
elif [ "$1" = "raspberry" ]; then
if [ $m64 -eq 1 ]; then
cp mpir/linux/aarch64/libgmp.so j64
else
cp mpir/linux/arm/libgmp.so j32
fi
elif [ "$1" = "darwin" ]; then
cp mpir/apple/macos/libgmp.dylib j64
elif [ "$1" = "openbsd" ]; then
cp /usr/local/lib/libgmp.so.11.0 j64/libgmp.so
elif [ "$1" = "freebsd" ]; then
cp /usr/local/lib/libgmp.so.10 j64/libgmp.so
fi

cp version.txt jsrc/jversion.h
echo "#define jplatform \"$1\"" >> jsrc/jversion.h
echo "#define jlicense  \"commercial\"" >> jsrc/jversion.h
echo "#define jbuilder  \"www.jsoftware.com\"" >> jsrc/jversion.h

if [ "x$MAKEFLAGS" = x'' ] ; then
if [ "$1" = "linux" ] || [ "$1" = "raspberry" ] ; then
par=`nproc` 
elif [ "$1" = "darwin" ] || [ "$1" = "openbsd" ] || [ "$1" = "freebsd" ] || [ "$1" = "android" ] ; then
par=`sysctl -n hw.ncpu` 
else 
par=2
fi
export MAKEFLAGS=-j$par
fi
echo "MAKEFLAGS=$MAKEFLAGS"

if [ "$1" = "android" ]; then
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

# hostdefs netdefs
if [ "$1" = "openbsd" ] || [ "$1" = "freebsd" ] ; then
cd hostdefs
$CC hostdefs.c -o hostdefs && ./hostdefs
cd ../netdefs
$CC netdefs.c -o netdefs && ./netdefs
cd ..
fi

cd make2

if [ $m64 -eq 1 ]; then
if [ "$1" = "darwin" ]; then
./clean.sh
j64x=j64arm USE_PYXES=1 ./build_jconsole.sh
j64x=j64arm ./build_tsdll.sh
j64x=j64arm USE_PYXES=1 ./build_libj.sh
elif [ "$1" = "linux" ]; then
./clean.sh
j64x=j32 USE_PYXES=0 ./build_jconsole.sh
j64x=j32 ./build_tsdll.sh
j64x=j32 USE_PYXES=0 ./build_libj.sh
fi
./clean.sh
if ( [ "$1" = "openbsd" ] || [ "$1" = "freebsd" ] ) && ( [ "`uname -m`" = "aarch64" ] || [ "`uname -m`" = "arm64" ] ) ; then
 j64x=j64arm USE_PYXES=1 ./build_jconsole.sh
 j64x=j64arm ./build_tsdll.sh
 j64x=j64arm USE_PYXES=1 ./build_libj.sh
else
 j64x=j64 USE_PYXES=1 ./build_jconsole.sh
 j64x=j64 ./build_tsdll.sh
 j64x=j64 USE_PYXES=1 ./build_libj.sh
fi
else
j64x=j32 USE_PYXES=0 ./build_jconsole.sh
j64x=j32 ./build_tsdll.sh
j64x=j32 USE_PYXES=0 ./build_libj.sh
fi

if [ "$1" != "raspberry" ] && [ "$1" != "openbsd" ] && [ "$1" != "freebsd" ]; then
./clean.sh
j64x=j64avx USE_PYXES=1 ./build_libj.sh
./clean.sh
j64x=j64avx2 USE_PYXES=1 ./build_libj.sh
./clean.sh
j64x=j64avx512 USE_PYXES=1 ./build_libj.sh
fi

cd ..
if [ $m64 -eq 1 ]; then
if ( [ "$1" = "openbsd" ] || [ "$1" = "freebsd" ] ) && ( [ "`uname -m`" = "aarch64" ] || [ "`uname -m`" = "arm64" ] ) ; then
ls -l bin/$1/j64arm
cp bin/$1/j64arm/* j64
else
ls -l bin/$1/j64
cp bin/$1/j64/* j64
fi
else
ls -l bin/$1/j32
cp bin/$1/j32/* j32
fi
if [ "$1" = "darwin" ] && [ -f "bin/$1/j64arm/libj.$ext" ]; then
lipo bin/$1/j64/jconsole bin/$1/j64arm/jconsole -create -output j64/jconsole
lipo bin/$1/j64/libtsdll.$ext bin/$1/j64arm/libtsdll.$ext -create -output j64/libtsdll.$ext
lipo bin/$1/j64/libj.$ext bin/$1/j64arm/libj.$ext -create -output j64/libj.$ext
fi
if [ "$1" != "raspberry" ] && [ "$1" != "openbsd" ] && [ "$1" != "freebsd" ]; then
cp bin/$1/j64avx/libj.$ext j64/libjavx.$ext
cp bin/$1/j64avx2/libj.$ext j64/libjavx2.$ext
cp bin/$1/j64avx512/libj.$ext j64/libjavx512.$ext
fi

if [ $m64 -eq 1 ]; then
chmod 644 j64/*
chmod 755 j64/jconsole
ls -l j64
else
chmod 644 j32/*
chmod 755 j32/jconsole
ls -l j32
fi

if [ "$1" = "linux" ]; then
mkdir -p j32
cp bin/profile.ijs j32
cp bin/$1/j32/* j32
# cp mpir/linux/i386/libgmp.so j32
cp mpir/linux/i386/libgmpd.so j32/libgmp.so
chmod 644 j32/*
chmod 755 j32/jconsole
fi

if [ "$1" = "darwin" ]; then
cd j64
dsymutil jconsole 2> /dev/null || true
dsymutil libj.dylib 2> /dev/null || true
dsymutil libjavx.dylib 2> /dev/null || true
dsymutil libjavx2.dylib 2> /dev/null || true
dsymutil libjavx512.dylib 2> /dev/null || true
dsymutil libtsdll.dylib 2> /dev/null || true
cd ..
fi
