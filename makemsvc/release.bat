rem copy windows binaries from makemsvc to jlibrary\bin

set S=.
set A=..\jlibrary\bin
set B=..\jlibrary\bin32

copy %S%\jconsole\jconsole.exe %A%\.
copy %S%\jconsole\jconsole32.exe %B%\jconsole.exe

copy %S%\jdll\javx.dll %A%\javx.dll
copy %S%\jdll\javx2.dll %A%\javx2.dll
copy %S%\jdll\j.dll %A%\j-nonavx.dll
copy %S%\jdll\javx2.dll %A%\j.dll
copy %S%\jdll\j32.dll %B%\j.dll

copy %S%\tsdll\tsdll.dll %A%\.
copy %S%\tsdll\tsdll32.dll %B%\tsdll.dll

copy %S%\mpir\windows\x64\mpir.dll %A%\.
copy %S%\pthreads4w\bin\pthreadVC3.dll %A%\.
copy %S%\pcre2\windows\x64\jpcre2.dll %A%\.
