#!/bin/sh
set -e
# build all binaries

cd "$(dirname "$0")"
echo "entering $(pwd)"

unameop=$(uname -o || uname -s)
eval "$(./jplatform64.sh)"

jplatform=${jplatform} j64x=${j64x} ./clean.sh "$1" || true
jplatform=${jplatform} j64x=${j64x} ./build_jconsole.sh
jplatform=${jplatform} j64x=${j64x} ./build_libj.sh
jplatform=${jplatform} j64x=${j64x} ./build_tsdll.sh
jplatform=${jplatform} j64x=${j64x} ./cpbin.sh
