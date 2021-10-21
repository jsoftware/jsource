#!/bin/sh

# copy binaries in bin/ to jlibrary/bin

cd "`dirname "$0"`"

if [ "`uname -m`" = "armv6l" ] || [ "`uname -m`" = "aarch64" ] || [ "$RASPI" = 1 ]; then
jplatform="${jplatform:=raspberry}"
elif [ "`uname`" = "Darwin" ]; then
jplatform="${jplatform:=darwin}"
else
jplatform="${jplatform:=linux}"
fi

if [ "`uname -m`" = "armv6l" ] || [ "`uname -m`" = "aarch64" ] || [ "$RASPI" = 1 ]; then

cp ../bin/${jplatform}/j32/jconsole ../jlibrary/bin32/. || true
cp ../bin/${jplatform}/j64/jconsole ../jlibrary/bin/. || true

cp ../bin/${jplatform}/j32/libtsdll.so ../jlibrary/bin32/. || true
cp ../bin/${jplatform}/j64/libtsdll.so ../jlibrary/bin/. || true

cp ../bin/${jplatform}/j32/libj.so ../jlibrary/bin32/. || true
cp ../bin/${jplatform}/j64/libj.so ../jlibrary/bin/. || true

elif [ "`uname -m`" = "arm64" ]; then

# bin for arm64 binaries
# MUST rename/remove object first; overwrite cause cache error
if [ -f "../bin/${jplatform}/j64arm/jconsole" ]; then
mv ../jlibrary/bin/jconsole ../jlibrary/bin/jconsole.old || true
mv ../jlibrary/bin/libtsdll.dylib ../jlibrary/bin/libtsdll.dylib.old || true
mv ../jlibrary/bin/libj.dylib ../jlibrary/bin/libj.dylib.old || true
cp ../bin/${jplatform}/j64arm/jconsole ../jlibrary/bin/. || true
cp ../bin/${jplatform}/j64arm/libtsdll.dylib ../jlibrary/bin/. || true
cp ../bin/${jplatform}/j64arm/libj.dylib ../jlibrary/bin/. || true
fi

# bin32 for x86_64 binaries
if [ -f "../bin/${jplatform}/j64/jconsole" ]; then
mv ../jlibrary/bin32/jconsole ../jlibrary/bin32/jconsole.old || true
mv ../jlibrary/bin32/libtsdll.dylib ../jlibrary/bin32/libtsdll.dylib.old || true
mv ../jlibrary/bin32/libj.dylib ../jlibrary/bin32/libj.dylib.old || true
cp ../bin/${jplatform}/j64/jconsole ../jlibrary/bin32/. || true
cp ../bin/${jplatform}/j64/libtsdll.dylib ../jlibrary/bin32/. || true
cp ../bin/${jplatform}/j64/libj.dylib ../jlibrary/bin32/. || true
fi

else

cp ../bin/${jplatform}/j32/jconsole ../jlibrary/bin32/. || true
cp ../bin/${jplatform}/j64/jconsole ../jlibrary/bin/. || true
cp ../bin/${jplatform}/j64avx/jconsole ../jlibrary/bin/. || true
cp ../bin/${jplatform}/j64avx2/jconsole ../jlibrary/bin/. || true

if [ "`uname`" = "Darwin" ]; then

cp ../bin/${jplatform}/j32/libtsdll.dylib ../jlibrary/bin32/. || true
cp ../bin/${jplatform}/j64/libtsdll.dylib ../jlibrary/bin/. || true
cp ../bin/${jplatform}/j64avx/libtsdll.dylib ../jlibrary/bin/. || true
cp ../bin/${jplatform}/j64avx2/libtsdll.dylib ../jlibrary/bin/. || true

cp ../bin/${jplatform}/j32/libj.dylib ../jlibrary/bin32/. || true
cp ../bin/${jplatform}/j64/libj.dylib ../jlibrary/bin/libj-nonavx.dylib || true
cp ../bin/${jplatform}/j64avx/libj.dylib ../jlibrary/bin/libjavx.dylib || true
cp ../bin/${jplatform}/j64avx2/libj.dylib ../jlibrary/bin/libjavx2.dylib || true
cp ../bin/${jplatform}/j64avx2/libj.dylib ../jlibrary/bin/. || true

else

cp ../bin/${jplatform}/j32/libtsdll.so ../jlibrary/bin32/. || true
cp ../bin/${jplatform}/j64/libtsdll.so ../jlibrary/bin/. || true
cp ../bin/${jplatform}/j64avx/libtsdll.so ../jlibrary/bin/. || true
cp ../bin/${jplatform}/j64avx2/libtsdll.so ../jlibrary/bin/. || true

cp ../bin/${jplatform}/j32/libj.so ../jlibrary/bin32/. || true
cp ../bin/${jplatform}/j64/libj.so ../jlibrary/bin/libj-nonavx.so || true
cp ../bin/${jplatform}/j64avx/libj.so ../jlibrary/bin/libjavx.so || true
cp ../bin/${jplatform}/j64avx2/libj.so ../jlibrary/bin/libjavx2.so || true
cp ../bin/${jplatform}/j64avx2/libj.so ../jlibrary/bin/. || true

fi

fi
