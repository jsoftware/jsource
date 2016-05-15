
git=..
jbld=.

mkdir -p $git/release/windows/j32
rm -f $git/release/windows/j32/jconsole.exe
rm -f $git/release/windows/j32/j.dll
rm -f $git/release/windows/j32/tsdll.dll

cp $jbld/jconsole/jconsole32.exe $git/release/windows/j32/jconsole.exe
cp $jbld/jdll/j32.dll            $git/release/windows/j32/j.dll
cp $jbld/tsdll/tsdll32.dll       $git/release/windows/j32/tsdll.dll

mkdir -p $git/release/windows/j64
rm -f $git/release/windows/j64/jconsole.exe
rm -f $git/release/windows/j64/j.dll
rm -f $git/release/windows/j64/tsdll.dll

cp $jbld/jconsole/jconsole.exe   $git/release/windows/j64/jconsole.exe
cp $jbld/jdll/j.dll              $git/release/windows/j64/j.dll
cp $jbld/tsdll/tsdll.dll         $git/release/windows/j64/tsdll.dll
