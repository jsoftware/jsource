#!/bin/sh
#
# build linux/macOS on github actions
#
# argument is linux|darwin|raspberry|android|openbsd|freebsd|wasm
# wasm is experimental

set -e
CC=${CC-clang}
USE_SLEEF=${USE_SLEEF-1}
export CC USE_SLEEF

if [ "$1" = "linux" ]; then
  ext="so"
  export _DEBUG=3
elif [ "$1" = "raspberry" ]; then
  ext="so"
  export _DEBUG=3
elif [ "$1" = "darwin" ]; then
  ext="dylib"
  export _DEBUG=3
elif [ "$1" = "android" ]; then
  ext="so"
elif [ "$1" = "openbsd" ]; then
  ext="so"
  export _DEBUG=3
elif [ "$1" = "freebsd" ]; then
  ext="so"
  export _DEBUG=3
elif [ "$1" = "wasm" ]; then
  ext=""
  export _DEBUG=0
else
  echo "argument is linux|darwin|raspberry|android|openbsd|freebsd|wasm"
  exit 1
fi
uname -a
uname -m
if [ "`uname -m`" != "armv6l" ] && [ "`uname -m`" != "i386" ] && [ "`uname -m`" != "i686" ] ; then
if [ "$1" = "wasm" ]; then
 m64=0
else
 m64=1
fi
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
cp pcre2/linux/x86_64/libjpcre2.so tools/regex/.
elif [ "$1" = "raspberry" ]; then
if [ $m64 -eq 1 ]; then
cp mpir/linux/aarch64/libgmp.so j64
cp pcre2/linux/aarch64/libjpcre2.so tools/regex/.
else
cp mpir/linux/arm/libgmp.so j32
cp pcre2/linux/arm/libjpcre2.so tools/regex/.
fi
elif [ "$1" = "darwin" ]; then
cp mpir/apple/macos/libgmp.dylib j64
cp pcre2/apple/macos/libjpcre2.dylib tools/regex/.
elif [ "$1" = "openbsd" ]; then
# cp /usr/local/lib/libgmp.so.11.0 j64/libgmp.so
if [ "`uname -m`" = "amd64" ] ; then
cp mpir/openbsd/x86_64/libgmp.so j64
cp pcre2/openbsd/x86_64/libjpcre2.so tools/regex/.
else
cp mpir/openbsd/aarch64/libgmp.so j64
cp pcre2/openbsd/aarch64/libjpcre2.so tools/regex/.
fi
elif [ "$1" = "freebsd" ]; then
# cp /usr/local/lib/libgmp.so.10 j64/libgmp.so
if [ "`uname -m`" = "amd64" ] ; then
cp mpir/freebsd/x86_64/libgmp.so j64
cp pcre2/freebsd/x86_64/libjpcre2.so tools/regex/.
else
cp mpir/freebsd/aarch64/libgmp.so j64
cp pcre2/freebsd/aarch64/libjpcre2.so tools/regex/.
fi
fi

cp version.txt jsrc/jversion.h
echo "#define jplatform \"$1\"" >> jsrc/jversion.h
echo "#define jlicense  \"commercial\"" >> jsrc/jversion.h
echo "#define jbuilder  \"www.jsoftware.com\"" >> jsrc/jversion.h

if [ "x$MAKEFLAGS" = x'' ] ; then
if [ "$1" = "wasm" ] ; then
par=2
elif [ "$1" = "linux" ] || [ "$1" = "raspberry" ] ; then
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
rm -f androidlibs.zip
# build binary for armeabi-v7a x86 x86_64 arm64-v8a
cd android
ndk-build
zip -r ../androidlibs.zip libs
cd ..
# build binary for armeabi
cd ~/
wget https://dl.google.com/android/repository/android-ndk-r16b-darwin-x86_64.zip
unzip android-ndk-r16b-darwin-x86_64.zip
cd -
cd android
sed -i "" -e "s/^APP_ABI/#   APP_ABI/g" jni/Application.mk
sed -i "" -e "s/^# APP_ABI := armeabi/APP_ABI := armeabi/g" jni/Application.mk
sed -i "" -e "s/android-16/android-9/g" jni/Application.mk
NDK_TOOLCHAIN_VERSION=4.9 ~/android-ndk-r16b/ndk-build
zip -r ../androidlibs.zip libs
cd ..
exit 0
fi

if [ "$1" = "wasm" ]; then
cd make2
./clean.sh
USE_WASM=1 jplatform=wasm j64x=j32 CC=emcc AR=emar ./build_jamalgam.sh
./clean.sh
USE_WASM=1 jplatform=wasm j64x=j32 CC=emcc AR=emar ./build_libj.sh
cd ..
cp bin/$1/j32/* j32
find j32 -type d -exec chmod 755 {} \;
find j32 -type f -exec chmod 644 {} \;
find j32 \( -name 'jconsole' -o -name 'jamalgam' \) -type f -exec chmod 755 {} \;
ls -l j32
exit 0
fi

# hostdefs netdefs
cd hostdefs
$CC hostdefs.c -o hostdefs && ./hostdefs
cd ../netdefs
$CC netdefs.c -o netdefs && ./netdefs
cd ..

cd make2

if [ $m64 -eq 1 ]; then
if [ "$1" = "darwin" ]; then
./clean.sh
j64x=j64arm USE_PYXES=1 ./build_jconsole.sh
j64x=j64arm ./build_tsdll.sh
j64x=j64arm USE_PYXES=1 ./build_libj.sh
j64x=j64arm USE_PYXES=1 ./build_jamalgam.sh
./clean.sh
j64x=j64iphoneos _DEBUG=0 USE_PYXES=1 ./build_jconsole.sh
j64x=j64iphoneos _DEBUG=0 ./build_tsdll.sh
j64x=j64iphoneos _DEBUG=0 USE_PYXES=1 ./build_libj.sh
./clean.sh
j64x=j64iphonesimulator _DEBUG=0 USE_PYXES=1 ./build_jconsole.sh
j64x=j64iphonesimulator _DEBUG=0 ./build_tsdll.sh
j64x=j64iphonesimulator _DEBUG=0 USE_PYXES=1 ./build_libj.sh
elif [ "$1" = "linux" ]; then
./clean.sh
j64x=j32 USE_PYXES=0 ./build_jconsole.sh
j64x=j32 ./build_tsdll.sh
j64x=j32 USE_PYXES=0 ./build_libj.sh
# j64x=j32 USE_PYXES=0 ./build_jamalgam.sh
fi
./clean.sh
if ( [ "$1" = "openbsd" ] || [ "$1" = "freebsd" ] ) && ( [ "`uname -m`" = "aarch64" ] || [ "`uname -m`" = "arm64" ] ) ; then
 j64x=j64arm USE_PYXES=1 ./build_jconsole.sh
 j64x=j64arm ./build_tsdll.sh
 j64x=j64arm USE_PYXES=1 ./build_libj.sh
# j64x=j64arm USE_PYXES=1 ./build_jamalgam.sh
else
 j64x=j64 USE_PYXES=1 ./build_jconsole.sh
 j64x=j64 ./build_tsdll.sh
 j64x=j64 USE_PYXES=1 ./build_libj.sh
 if [ "$1" != "openbsd" ] && [ "$1" != "freebsd" ] ; then
 j64x=j64 USE_PYXES=1 ./build_jamalgam.sh
 fi
fi
if [ "`uname -m`" = "x86_64" ] || [ "`uname -m`" = "amd64" ] ; then
./clean.sh
j64x=j64avx2 USE_PYXES=1 ./build_libj.sh
./clean.sh
j64x=j64avx512 USE_PYXES=1 ./build_libj.sh
fi
else
j64x=j32 USE_PYXES=0 ./build_jconsole.sh
j64x=j32 ./build_tsdll.sh
j64x=j32 USE_PYXES=0 ./build_libj.sh
# j64x=j32 USE_PYXES=0 ./build_jamalgam.sh
fi

cd -

if [ $m64 -eq 1 ]; then
if ( [ "$1" = "openbsd" ] || [ "$1" = "freebsd" ] ) && ( [ "`uname -m`" = "aarch64" ] || [ "`uname -m`" = "arm64" ] ) ; then
cp bin/$1/j64arm/* j64
else
cp bin/$1/j64/* j64
fi
else
cp bin/$1/j32/* j32
fi

if [ "$1" = "darwin" ] && [ -f "bin/$1/j64arm/libj.$ext" ]; then
lipo bin/$1/j64/jconsole bin/$1/j64arm/jconsole -create -output j64/jconsole
lipo bin/$1/j64/libtsdll.$ext bin/$1/j64arm/libtsdll.$ext -create -output j64/libtsdll.$ext
lipo bin/$1/j64/libj.$ext bin/$1/j64arm/libj.$ext -create -output j64/libj.$ext
lipo bin/$1/j64/jamalgam bin/$1/j64arm/jamalgam -create -output j64/jamalgam
fi

if [ -d "bin/$1/j64iphoneos" ]; then
mkdir -p j64/ios
cp -r bin/$1/j64iphoneos j64/ios/.
fi

if [ -d "bin/$1/j64iphonesimulator" ]; then
mkdir -p j64/ios
cp -r bin/$1/j64iphonesimulator j64/ios/.
fi

if [ -f bin/$1/j64avx2/libj.$ext ] ; then
cp bin/$1/j64avx2/libj.$ext j64/libjavx2.$ext
fi

if [ -f bin/$1/j64avx512/libj.$ext ] ; then
cp bin/$1/j64avx512/libj.$ext j64/libjavx512.$ext
fi

if [ "$1" = "linux" ]; then
mkdir -p j32
cp bin/profile.ijs j32
cp bin/$1/j32/* j32
# cp mpir/linux/i386/libgmp.so j32
cp mpir/linux/i386/libgmpd.so j32/libgmp.so
cp pcre2/linux/i386/libjpcre2.so tools/regex/libjpcre2_32.so
fi

if [ "$1" = "darwin" ] && [ -d j64 ] ; then
cd j64
dsymutil jconsole 2> /dev/null || true
dsymutil libj.dylib 2> /dev/null || true
dsymutil libjavx2.dylib 2> /dev/null || true
dsymutil libjavx512.dylib 2> /dev/null || true
dsymutil libtsdll.dylib 2> /dev/null || true
dsymutil jamalgam 2> /dev/null || true
cd ..
fi

if [ -d j64 ]; then
find j64 -type d -exec chmod 755 {} \;
find j64 -type f -exec chmod 644 {} \;
find j64 \( -name 'jconsole' -o -name 'jamalgam' \) -type f -exec chmod 755 {} \;
ls -l j64
fi

if [ -d j32 ]; then
find j32 -type d -exec chmod 755 {} \;
find j32 -type f -exec chmod 644 {} \;
find j32 \( -name 'jconsole' -o -name 'jamalgam' \) -type f -exec chmod 755 {} \;
ls -l j32
fi
