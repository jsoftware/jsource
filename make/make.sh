#

export jplatform=linux

cd ..
export jgit=$(pwd)
cd make
export jbld=$jgit/build

export j32=$jbld/j32/bin/jconsole
export j64=$jbld/j64/bin/jconsole
export j64avx="$jbld/j64/bin/jconsole -lib libjavx.so"
export jmake=$jgit/make

$jmake/install.sh
$jmake/clean.sh

$jmake/build_all.sh
$jmake/release.sh
