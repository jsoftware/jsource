@rem test windows on github actions

@echo on
@rem if $USE_EMU_AVX = 0 or $USE_PYXES = 0 skip test avx2

echo "%USE_EMU_AVX%"
echo "%USE_PYXES%"

set "A=jlibrary"
set "B=jlibrary\bin"
set "C=jlibrary\bin32"

echo %A%
echo %B%
echo %C%

@rem x64 x86 arm64
IF "%~1"=="x86" GOTO L0
IF "%~1"=="arm64" GOTO L0
IF "%~1" NEQ "x64" EXIT /b 1
:L0

systeminfo

IF "%~1"=="x86" GOTO L01A
IF "%~1"=="arm64" GOTO L01B
IF "%~1" NEQ "x64" EXIT /b 1
@rem copy "C:\Program Files\LLVM\bin\liblldb.dll" "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\Llvm\x64\bin\"
@rem curl --output-dir "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/Llvm/x64/bin/" -o "liblldb.dll" "https://www.jsoftware.com/download/winlib/x64/liblldb.dll"
@rem curl --output-dir "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/Llvm/x64/bin/" -o "lldb.exe" "https://www.jsoftware.com/download/winlib/x64/lldb.exe"
@rem "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\Llvm\x64\bin\lldb" --version
@rem lldb --version

dir %B%
IF "%USE_EMU_AVX%"=="0" GOTO L01F
IF "%USE_PYXES%"=="0" GOTO L01F
%B%\jconsole -lib javx2.dll script\testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
:L01F
IF "%_DEBUG%"=="3" GOTO L01H
%B%\jconsole -lib j.dll script\testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
GOTO L01C
:L01H
lldb -b -o run -k bt -k quit -- %B%\jconsole.exe -lib j.dll script\testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
GOTO L01C
:L01A
dir %C%
%C%\jconsole script\testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
GOTO L01C
:L01B
dir %B%
IF "%_DEBUG%"=="3" GOTO L01I
%B%\jconsole script\testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
GOTO L01C
:L01I
lldb -b -o run -k bt -k quit -- %B%\jconsole.exe -lib j.dll script\testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
:L01C
