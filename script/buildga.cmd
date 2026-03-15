@rem build windows on github actions

set
clang-cl --version

@rem if $USE_EMU_AVX = 0 or $USE_PYXES = 0 skip build avx2 avx512

echo "%USE_EMU_AVX%"
echo "%USE_PYXES%"

@rem x64 x86 arm64
IF "%~1"=="x86" GOTO L0
IF "%~1"=="arm64" GOTO L0
IF "%~1" NEQ "x64" EXIT /b 1
:L0

xcopy /Y /I /S jlibrary\*
copy script\testga.ijs
copy script\ver.ijs

IF "%~1"=="x86" GOTO L01A
IF "%~1"=="arm64" GOTO L01B
IF "%~1" NEQ "x64" EXIT /b 1
mkdir j64
mkdir bin\windows\j64
mkdir bin\windows\j64avx2
mkdir bin\windows\j64avx512
copy bin\profile.ijs j64
copy pthreads4w\bin\pthreadVC3.dll j64
copy mpir\windows\x64\mpir.dll j64
copy openmp\obj\windows\libomp.dll j64
copy pcre2\windows\x64\jpcre2.dll tools\regex\.
curl --output-dir "j64" -O "https://www.jsoftware.com/download/lapackbin/libopenblas.dll"
GOTO L01C
:L01A
mkdir j32
mkdir bin\windows\j32
copy bin\profile.ijs j32
copy pthreads4w\bin\pthreadVC3-w32.dll j32\pthreadVC3.dll
copy mpir\windows\x86\mpir.dll j32\mpir32.dll
copy openmp\obj\windows\libomp32.dll j32\libomp32.dll
copy pcre2\windows\x86\jpcre2.dll tools\regex\jpcre2_32.dll
curl --output-dir "j32" -O "https://www.jsoftware.com/dowoad/lapackbin/libopenblas_32.dll"
GOTO L01C
:L01B
mkdir j64
mkdir bin\windows\j64arm
copy bin\profile.ijs j64
copy pthreads4w\bin\pthreadVC3-arm64.dll j64\pthreadVC3.dll
copy mpir\windows\arm64\mpir.dll j64
copy openmp\obj\windows\libomp.dll j64
copy pcre2\windows\arm64\jpcre2.dll tools\regex\jpcre2_arm64.dll
curl --output-dir "j64" -O "https://www.jsoftware.com/download/lapackbin/libopenblas_arm64.dll"
:L01C

copy version.txt jsrc\jversion.h
echo #define jplatform "windows" >> jsrc\jversion.h
echo #define jlicense "commercial" >> jsrc\jversion.h
echo #define jbuilder "www.jsoftware.com" >> jsrc\jversion.h

cd jsrc
set CC=clang-cl

IF "%~1"=="x86" GOTO L04A
IF "%~1"=="arm64" GOTO L04B
IF "%~1" NEQ "x64" EXIT /b 1
nmake -f ..\makemsvc\tsdll\makefile.win jplatform=windows j64x=j64 clean
nmake -f ..\makemsvc\tsdll\makefile.win jplatform=windows j64x=j64
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
GOTO L04C
:L04A
nmake -f ..\makemsvc\tsdll\makefile.win jplatform=windows j64x=j32 clean
nmake -f ..\makemsvc\tsdll\makefile.win jplatform=windows j64x=j32
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
GOTO L04C
:L04B
nmake -f ..\makemsvc\tsdll\makefile.win jplatform=windows j64x=j64arm clean
nmake -f ..\makemsvc\tsdll\makefile.win jplatform=windows j64x=j64arm
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
:L04C

IF "%~1"=="x86" GOTO L02A
IF "%~1"=="arm64" GOTO L02B
IF "%~1" NEQ "x64" EXIT /b 1
nmake -f ..\makemsvc\jconsole\makefile.win jplatform=windows j64x=j64 clean
nmake -f ..\makemsvc\jconsole\makefile.win jplatform=windows j64x=j64
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
GOTO L02C
:L02A
nmake -f ..\makemsvc\jconsole\makefile.win jplatform=windows j64x=j32 clean
nmake -f ..\makemsvc\jconsole\makefile.win jplatform=windows j64x=j32
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
GOTO L02C
:L02B
nmake -f ..\makemsvc\jconsole\makefile.win jplatform=windows j64x=j64arm clean
nmake -f ..\makemsvc\jconsole\makefile.win jplatform=windows j64x=j64arm
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
:L02C

IF "%~1"=="x86" GOTO L03A
IF "%~1"=="arm64" GOTO L03B
IF "%~1" NEQ "x64" EXIT /b 1
IF "%USE_EMU_AVX%"=="0" GOTO L03F
IF "%USE_PYXES%"=="0" GOTO L03F
nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j64avx512 clean
nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j64avx512
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j64avx2 clean
nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j64avx2
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
:L03F
nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j64 clean
nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j64
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
GOTO L03C
:L03A
nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j32 clean
nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j32
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
GOTO L03C
:L03B
nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j64arm NO_SHA_ASM=1 clean
nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j64arm NO_SHA_ASM=1
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
:L03C

GOTO L05C

IF "%~1"=="x86" GOTO L05A
IF "%~1"=="arm64" GOTO L05B
IF "%~1" NEQ "x64" EXIT /b 1
nmake -f ..\makemsvc\jamalgam\makefile.win jplatform=windows j64x=j64 clean
nmake -f ..\makemsvc\jamalgam\makefile.win jplatform=windows j64x=j64
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
GOTO L05C
:L05A
nmake -f ..\makemsvc\jamalgam\makefile.win jplatform=windows j64x=j32 jclean
nmake -f ..\makemsvc\jamalgam\makefile.win jplatform=windows j64x=j32
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
GOTO L05C
:L05B
nmake -f ..\makemsvc\jamalgam\makefile.win jplatform=windows j64x=j64arm NO_SHA_ASM=1 clean
nmake -f ..\makemsvc\jamalgam\makefile.win jplatform=windows j64x=j64arm NO_SHA_ASM=1
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
:L05C

cd ..
IF "%~1"=="x86" GOTO L06A
IF "%~1"=="arm64" GOTO L06B
IF "%~1" NEQ "x64" EXIT /b 1
copy bin\windows\j64\jconsole.exe j64
copy bin\windows\j64\*.dll j64
copy bin\windows\j64avx512\j.dll j64\javx512.dll
copy bin\windows\j64avx2\j.dll j64\javx2.dll
@rem copy bin\windows\j64\jamalgam.exe j64
GOTO L06C
:L06A
copy bin\windows\j32\jconsole.exe j32
copy bin\windows\j32\*.dll j32
@rem copy bin\windows\j32\jamalgam.exe j32
GOTO L06C
:L06B
copy bin\windows\j64arm\jconsole.exe j64
copy bin\windows\j64arm\*.dll j64
@rem copy bin\windows\j64arm\jamalgam.exe j64
:L06C
