#!/bin/bash
# build all binaries
cd ~

# install.sh should only be done for clean install
# pacman may have update base library and addons
# $jmake/install.sh

$jmake/clean.sh

$jmake/build_jconsole.sh j64
$jmake/build_libj.sh     j64
$jmake/build_tsdll.sh    j64



