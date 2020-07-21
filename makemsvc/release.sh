#!/bin/sh

# copy windows binaries from makemsvc to jlibrary/bin

cd "`dirname "$0"`"

S=.
A=../jlibrary/bin
B=../jlibrary/bin32

cp $S/jconsole/jconsole.exe $A/.
cp $S/jconsole/jconsole32.exe $B/jconsole.exe

cp $S/jdll/javx.dll $A/javx.dll
cp $S/jdll/javx2.dll $A/javx2.dll
cp $S/jdll/j.dll $A/j-nonavx.dll
cp $S/jdll/javx2.dll $A/j.dll
cp $S/jdll/j32.dll $B/j.dll

cp $S/tsdll/tsdll.dll $A/.
cp $S/tsdll/tsdll32.dll $B/tsdll.dll
