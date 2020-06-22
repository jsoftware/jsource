#!/bin/bash
. jvars.sh
mkdir -p $jbld/test
echo "echo '*************',LF,LF[echo RUN 3{.ddall" | $j64    >  $jbld/test/jtest.txt
echo "echo '*************',LF,LF[echo RUN 3{.ddall" | $j64avx >> $jbld/test/jtest.txt

echo "echo '*************',LF,LF[jdrun''" | $j64    >  $jbld/test/jdtest.txt
echo "echo '*************',LF,LF[jdrun''" | $j64avx >> $jbld/test/jdtest.txt

cat $jbld/test/jtest.txt
cat $jbld/test/jdtest.txt

