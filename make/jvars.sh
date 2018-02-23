# source shell script (read with . jvars.sh) so stuff is easy to find

# edit following if your install is not standard 
cd ..
jgit=$(pwd)
cd make
jbld=$jgit/build

# edit platform to build - linux darwin raspberry
jplatform=linux

# should not be necessary to edit after here
j32=$jbld/j32/bin/jconsole
j64=$jbld/j64/bin/jconsole
j64nonavx="$jbld/j64/bin/jconsole -lib libjnonavx.so"
jmake=$jgit/make

export jgit jbld j32 j64 j64avx jmake

