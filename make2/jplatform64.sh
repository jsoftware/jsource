#!/bin/sh
set -e

# respect individual jplatform and/or j64x if they are defined

cd "$(dirname "$0")"

if [ "`uname`" != "Darwin" ] && ( [ "`uname -m`" = "armv6l" ] || [ "`uname -m`" = "aarch64" ] ); then
  jplatform="${jplatform:=raspberry}"
elif [ "`uname`" = "Darwin" ]; then
  jplatform="${jplatform:=darwin}"
elif [ "`uname`" = "OpenBSD" ]; then
  jplatform="${jplatform:=openbsd}"
else
  jplatform="${jplatform:=linux}"
fi

if [ "`uname -m`" = "arm64" ] && [ -z "${jplatform##*darwin*}" ]; then
  j64x="${j64x:=j64arm}"
elif [ "`uname -m`" = "x86_64" ]; then
  j64x="${j64x:=j64avx}"
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

