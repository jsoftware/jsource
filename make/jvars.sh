# source shell script (read with . jvars.sh) so stuff is easy to find

# edit following if your install is not standard 
jgit=~/git/jsource # git jsource folder
jbld=~/jbld        # test libraries and binaries will be put here

# edit platform to build - linux darwin raspberry
jplatform=linux

# should not be necessary to edit after here
tsu=$jgit/test/tsu.ijs
j32="$jbld/j32/bin/jconsole $tsu"
j64="$jbld/j64/bin/jconsole $tsu"
j64nonavx="$jbld/j64/bin/jconsole -lib libj-nonavx.so $tsu"
jmake=$jgit/make

export jgit jbld j32 j64 j64nonavx jmake

