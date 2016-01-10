#!/bin/bash
# build all binaries
cd ~
. jvars.sh

$jmake/install.sh

$jmake/build_jconsole.sh j32
$jmake/build_libj.sh     j32
$jmake/build_tsdll.sh    j32

if [ $jplatform = "raspberry" ]; then
  exit 0
fi  

$jmake/build_jconsole.sh j64
$jmake/build_libj.sh     j64
$jmake/build_tsdll.sh    j64

