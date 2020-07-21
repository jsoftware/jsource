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

else

cp ../bin/${jplatform}/j32/jconsole ../jlibrary/bin32/. || true
cp ../bin/${jplatform}/j64/jconsole ../jlibrary/bin/. || true
cp ../bin/${jplatform}/j64avx/jconsole ../jlibrary/bin/. || true
cp ../bin/${jplatform}/j64avx2/jconsole ../jlibrary/bin/. || true

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
