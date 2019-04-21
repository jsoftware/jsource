#!/bin/bash
# build all binaries

cd "$(dirname "$(readlink -f "$0" || realpath "$0")")"

find obj -name "*.o" -type f -delete

jplatform="${jplatform:=linux}"
j64="${j64:=j64}"

jplatform=$jplatform j64=$j64 ./build_jconsole.sh
jplatform=$jplatform j64=$j64 ./build_libj.sh
jplatform=$jplatform j64=$j64 ./build_tsdll.sh
