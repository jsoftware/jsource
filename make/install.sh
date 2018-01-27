#!/bin/bash
# install jbld folders - new install or a reinstall

rm -f -r $jbld
mkdir $jbld
mkdir $jbld/j32
mkdir $jbld/j64
mkdir $jbld/jout
cp -r $jgit/jlibrary/* $jbld/j32
cp -r $jgit/jlibrary/* $jbld/j64

echo "install complete"
echo ""
echo "$jgit/license.txt"

cat $jgit/license.txt
exit 0

