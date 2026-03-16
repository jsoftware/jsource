rem copy windows binaries from bin to jlibrary\bin

set S=..\bin\windows
set A=..\jlibrary\bin
set B=..\jlibrary\bin32

copy %S%\j64\jconsole.exe %A%\.
copy %S%\j32\jconsole.exe %B%\.

copy %S%\j64avx2\j.dll %A%\javx2.dll
copy %S%\j64\j.dll %A%\.
copy %S%\j64\j.dll %A%\j-nonavx.dll
copy %S%\j32\j.dll %B%\.

copy %S%\j64\tsdll.dll %A%\.
copy %S%\j32\tsdll.dll %B%\.

copy %S%\..\..\mpir\windows\x64\mpir.dll %A%\.
copy %S%\..\..\pthreads4w\bin\pthreadVC3.dll %A%\.
copy %S%\..\..\pcre2\windows\x64\jpcre2.dll %A%\.
