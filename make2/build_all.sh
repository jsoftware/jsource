#!/bin/sh
set -e
# build all binaries

cd "$(dirname "$0")"
echo "entering `pwd`"

if [ "$1" != "noclean" ] ; then
./clean.sh || true
fi
./build_jconsole.sh
./build_libj.sh
./build_tsdll.sh
