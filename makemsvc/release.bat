
set jgit=..
set jbld=.

mkdir %jgit%\release\windows\j32
del   %jgit%\release\windows\j32\jconsole.exe
del   %jgit%\release\windows\j32\j.dll
del   %jgit%\release\windows\j32\tsdll.dll

copy  %jbld%\jconsole\jconsole32.exe %jgit%\release\windows\j32\jconsole.exe
copy  %jbld%\jdll\j32.dll            %jgit%\release\windows\j32\j.dll
copy  %jbld%\tssll\tsdll32.dll       %jgit%\release\windows\j32\tsdll.dll

mkdir %jgit%\release\windows\j64
del   %jgit%\release\windows\j64\jconsole.exe
del   %jgit%\release\windows\j64\j.dll
del   %jgit%\release\windows\j64\tsdll.dll

copy  %jbld%\jconsole\jconsole.exe   %jgit%\release\windows\j64\jconsole.exe
copy  %jbld%\jdll\j.dll              %jgit%\release\windows\j64\j.dll
copy  %jbld%\tssll\tsdll.dll         %jgit%\release\windows\j64\tsdll.dll
