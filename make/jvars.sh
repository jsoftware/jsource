# source shell script (read with . jvars.sh) so stuff is easy to find

# edit following if your install is not standard 
jgit=~/gitdev/jsource # git jsource folder
jbld=~/jbld           # test libraries and binaries will be put here

# edit platform to build - linux darwin raspberry
jplatform=linux

# should not be necessary to edit after here
j32=$jbld/j32/bin/jconsole
j64=$jbld/j64/bin/jconsole
jmake=$jgit/make

export jgit jbld j32 j64 jmake

