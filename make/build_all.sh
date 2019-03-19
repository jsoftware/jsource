#!/bin/bash
# build all binaries
cd ~

$jmake/install.sh

$jmake/build_jconsole.sh j64
$jmake/build_libj.sh     j64
$jmake/build_tsdll.sh    j64



