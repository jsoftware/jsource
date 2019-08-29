#!/bin/bash
# build all binaries for j64 linux/darwin
cd ~

cat $jgit/jsrc/jversion.h

if [ ! -d jbld ]; then
 echo "~/jbld does not exist - running install.sh"
 $jmake/install.sh
fi

$jmake/clean.sh

$jmake/build_jconsole.sh j64
$jmake/build_tsdll.sh    j64
$jmake/build_libj.sh     j64
$jmake/build_libj.sh     j64avx
$jmake/build_libj.sh     j64avx2


