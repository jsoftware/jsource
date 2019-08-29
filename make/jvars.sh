#!/bin/bash
# source shell script (read with . jvars.sh) so stuff is easy to find

# edit following if your install is not standard 
jgit=~/git/jsource # git jsource folder
jbld=~/jbld        # test libraries and binaries will be put here

# platform and shared library suffix
jplatform=`uname|tr '[:upper:]' '[:lower:]'`
jsuffix=so
if [ $jplatform = "darwin" ] ; then jsuffix=dylib ; fi

CC=clang # compiler

# should not be necessary to edit after here
tsu=$jgit/test/tsu.ijs
j32="$jbld/j32/bin/jconsole $tsu"
j64="$jbld/j64/bin/jconsole $tsu"
j64avx="$jbld/j64/bin/jconsole -lib libjavx.$jsuffix $tsu"
j64avx2="$jbld/j64/bin/jconsole -lib libjavx2.$jsuffix $tsu"
jmake=$jgit/make

export jgit jbld jplatform j32 j64 j64avx j64avx2 jmake CC
