#!/bin/sh
set -e

# respect individual jplatform and/or j64x if they are defined

cd "$(dirname "$0")"

USE_WASM="${USE_WASM:=0}"
if [ $USE_WASM -eq 1 ] ; then
  jplatform="${jplatform:=wasm}"
  j64x="${j64x:=j32}"
  echo "$jplatform/$j64x"
  exit 0
fi

if ( [ "`uname`" = "Linux" ] )  && ( [ "`uname -m`" = "armv6l" ] || [ "`uname -m`" = "aarch64" ]  || [ "`uname -m`" = "arm64" ] ); then
  jplatform="${jplatform:=raspberry}"
elif [ "`uname`" = "Darwin" ]; then
  jplatform="${jplatform:=darwin}"
elif [ "`uname`" = "OpenBSD" ]; then
  jplatform="${jplatform:=openbsd}"
elif [ "`uname`" = "FreeBSD" ]; then
  jplatform="${jplatform:=freebsd}"
else
  jplatform="${jplatform:=linux}"
fi

if [ "`uname -m`" = "x86_64" ] || [ "`uname -m`" = "amd64" ]; then
  j64x="${j64x:=j64avx2}"
elif [ "`uname -m`" = "aarch64" ] || [ "`uname -m`" = "arm64" ]; then
  if [ -z "${jplatform##*raspberry*}" ]; then
    j64x="${j64x:=j64}"
  else
    j64x="${j64x:=j64arm}"
  fi
elif [ "`uname -m`" = "armv6l" ]; then
  j64x="${j64x:=j32}"
else j64x="${j64x:=j32}"
fi

echo "$jplatform/$j64x"

