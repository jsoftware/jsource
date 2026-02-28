#!/bin/sh
set -e

# respect individual jplatform and/or j64x if they are defined

cd "$(dirname "$0")"

USE_WASM="${USE_WASM:=0}"
if [ $USE_WASM -eq 1 ]; then
 jplatform="${jplatform:=wasm}"
 j64x="${j64x:=j32}"
 echo "$jplatform/$j64x"
 exit 0
fi

if ([ "$(uname -s)" = "Linux" ]) && ([ "$(uname -m)" = "armv6l" ] || [ "$(uname -m)" = "aarch64" ] || [ "$(uname -m)" = "arm64" ]); then
 jplatform="${jplatform:=raspberry}"
elif [ "$(uname -s)" = "Darwin" ]; then
 jplatform="${jplatform:=darwin}"
elif [ "$(uname -s)" = "OpenBSD" ]; then
 jplatform="${jplatform:=openbsd}"
elif [ "$(uname -s)" = "FreeBSD" ]; then
 jplatform="${jplatform:=freebsd}"
elif [ "$(uname -s)" = "MINGW64" ] || [ "$(uname -s)" = "MINGW32" ] || [ "$(uname -s)" = "CYGWIN" ] || [ "$(uname -s)" = "MSYS" ]; then
 jplatform="${jplatform:=windows}"
else
 jplatform="${jplatform:=linux}"
fi

if [ "$(uname -m)" = "x86_64" ] || [ "$(uname -m)" = "amd64" ]; then
 j64x="${j64x:=j64avx2}"
elif [ "$(uname -m)" = "aarch64" ] || [ "$(uname -m)" = "arm64" ]; then
 64x="${j64x:=j64arm}"
elif [ "$(uname -m)" = "armv6l" ]; then
 j64x="${j64x:=j32}"
else
 j64x="${j64x:=j32}"
fi

echo "$jplatform/$j64x"
