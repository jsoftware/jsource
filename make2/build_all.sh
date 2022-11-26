#!/bin/sh
set -e
# build all binaries

cd "$(dirname "$0")"
echo "entering `pwd`"

./clean.sh

./build_jconsole.sh
./build_libj.sh
./build_tsdll.sh
