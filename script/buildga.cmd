@rem build windows on github actions

xcopy /Y /I /S jlibrary\*
copy script\testga.ijs
copy script\ver.ijs

IF "%~1"=="x86" GOTO L01A
mkdir j64
copy bin\profile.ijs j64
copy pthreads4w\bin\pthreadVC3.dll j64
copy mpir\windows\x64\mpir.dll j64
copy pcre2\windows\x64\jpcre2.dll tools/regex/.
GOTO L01B
:L01A
mkdir j32
copy bin\profile.ijs j32
copy pthreads4w\bin\pthreadVC3-w32.dll j32\pthreadVC3.dll
copy mpir\windows\win32\mpir.dll j32
@rem copy pcre2\windows\win32\jpcre2.dll tools/regex/.
:L01B

copy version.txt jsrc\jversion.h
echo #define jplatform "windows" >> jsrc\jversion.h
echo #define jlicense  "commercial" >> jsrc\jversion.h
echo #define jbuilder  "www.jsoftware.com" >> jsrc\jversion.h

cd makemsvc\jconsole
IF "%~1"=="x86" GOTO L02A
nmake -f makefile.win CC=clang-cl x64=1 USE_PYXES=1 clean
nmake -f makefile.win CC=clang-cl x64=1 USE_PYXES=1
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
GOTO L02B
:L02A
nmake -f makefile.win CC=clang-cl x64=0 USE_PYXES=0 clean
nmake -f makefile.win CC=clang-cl x64=0 USE_PYXES=0
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
:L02B
cd ..
cd jdll
IF "%~1"=="x86" GOTO L03A
nmake -f makefile.win CC=clang-cl x64=1 JAVX512=1 USE_OPENMP=0 USE_PYXES=1 clean
nmake -f makefile.win CC=clang-cl x64=1 JAVX512=1 USE_OPENMP=0 USE_PYXES=1
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
nmake -f makefile.win CC=clang-cl x64=1 JAVX512=0 JAVX2=1 USE_OPENMP=0 USE_PYXES=1 clean
nmake -f makefile.win CC=clang-cl x64=1 JAVX512=0 JAVX2=1 USE_OPENMP=0 USE_PYXES=1
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
nmake -f makefile.win CC=clang-cl x64=1 JAVX512=0 JAVX2=0 USE_OPENMP=0 USE_PYXES=1 clean
nmake -f makefile.win CC=clang-cl x64=1 JAVX512=0 JAVX2=0 USE_OPENMP=0 USE_PYXES=1
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
GOTO L03B
:L03A
nmake -f makefile.win CC=clang-cl x64=0 USE_OPENMP=0 USE_PYXES=0 clean
nmake -f makefile.win CC=clang-cl x64=0 USE_OPENMP=0 USE_PYXES=0
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
:L03B
cd ..
cd tsdll
IF "%~1"=="x86" GOTO L04A
nmake -f makefile.win CC=clang-cl x64=1 clean
nmake -f makefile.win CC=clang-cl x64=1
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
GOTO L04B
:L04A
nmake -f makefile.win CC=clang-cl x64=0 clean
nmake -f makefile.win CC=clang-cl x64=0
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
:L04B
cd ..
cd jamalgam
IF "%~1"=="x86" GOTO L05A
nmake -f makefile.win CC=clang-cl x64=1 JAVX512=0 JAVX2=0 USE_OPENMP=0 USE_PYXES=1 clean
nmake -f makefile.win CC=clang-cl x64=1 JAVX512=0 JAVX2=0 USE_OPENMP=0 USE_PYXES=1
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
GOTO L05B
:L05A
nmake -f makefile.win CC=clang-cl x64=0 USE_OPENMP=0 USE_PYXES=0 clean
nmake -f makefile.win CC=clang-cl x64=0 USE_OPENMP=0 USE_PYXES=0
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
:L05B
cd ..

IF "%~1"=="x86" GOTO L06A
copy jconsole\jconsole.exe ..\j64
copy jdll\*.dll ..\j64
copy tsdll\tsdll.dll ..\j64
copy jamalgam\jamalgam.exe ..\j64
GOTO L06B
:L06A
copy jconsole\jconsole32.exe ..\j32\jconsole.exe
copy jdll\j32.dll ..\j32\j.dll
copy tsdll\tsdll32.dll ..\j32\tsdll.dll
copy jamalgam\jamalgam32.exe ..\j32\jamalgam.exe
:L06B
