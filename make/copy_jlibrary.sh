#!/bin/bash
# copy git jlibrary to jbld j32/j64

cd ~
if [ ! -f jvars.sh ]; then
 echo "~/jvars.sh must exist"
 exit 1
fi

cd ~
. jvars.sh

cp -r $jgit/jlibrary/* $jbld/j32
cp -r $jgit/jlibrary/* $jbld/j64
exit 0

