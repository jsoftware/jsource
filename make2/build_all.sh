#!/bin/sh
# build all binaries

cd "$(dirname "$(readlink -f "$0" || realpath "$0")")"

./clean.sh

if [ "`uname -m`" = "armv6l" ] || [ "`uname -m`" = "aarch64" ] || [ "$RASPI" = 1 ]; then
platform=raspberry
elif [ "`uname`" = "Darwin" ]; then
platform=darwin
else
platform=linux
fi

jplatform="${jplatform:=$platform}"
if [ "`uname -m`" = "x86_64" ] || [ "`uname -m`" = "aarch64" ]; then
cpu=j64
else
cpu=j32
fi

jplatform="${jplatform:=$platform}"
j64x="${j64x:=$cpu}"

jplatform=$jplatform j64x=$j64x ./build_jconsole.sh
jplatform=$jplatform j64x=$j64x ./build_libj.sh
jplatform=$jplatform j64x=$j64x ./build_tsdll.sh
