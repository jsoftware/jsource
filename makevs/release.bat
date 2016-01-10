
set jgit=%USERPROFILE%\gitdev\jsource
set jbld=%USERPROFILE%\jbld

mkdir %jgit%\release\windows\j32
del   %jgit%\release\windows\j32\jconsole.exe
del   %jgit%\release\windows\j32\j.dll

copy  %jbld%\j32\bin\jconsole.exe %jgit%\release\windows\j32\jconsole.exe
copy  %jbld%\j32\bin\j.dll        %jgit%\release\windows\j32\j.dll
copy  %jbld%\j32\bin\tsdll.dll    %jgit%\release\windows\j32\tsdll.dll

mkdir %jgit%\release\windows\j64
del   %jgit%\release\windows\j64\jconsole
del   %jgit%\jsource\release\windows\j64\j.dll

copy  %jbld%\j64\bin\jconsole.exe %jgit%\release\windows\j64\jconsole.exe
copy  %jbld%\j64\bin\j.dll        %jgit%\release\windows\j64\j.dll  
copy  %jbld%\j64\bin\tsdll.dll    %jgit%\release\windows\j64\tsdll.dll  
