@rem test windows on github actions

@rem if $USE_EMU_AVX = 0 or $USE_PYXES = 0 skip test avx2

echo "%USE_EMU_AVX%"
echo "%USE_PYXES%"

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
curl --output-dir "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/Llvm/x64/bin/" -o "liblldb.dll" "https://www.jsoftware.com/download/winlib/x64/liblldb.dll"
curl --output-dir "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/Llvm/x64/bin/" -o "lldb.exe" "https://www.jsoftware.com/download/winlib/x64/lldb.exe"
"C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\Llvm\x64\bin\lldb" --version
lldb --version

dir j64
IF "%USE_EMU_AVX%"=="0" GOTO L01F
IF "%USE_PYXES%"=="0" GOTO L01F
j64\jconsole -lib javx2.dll testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
:L01F
IF "%_DEBUG%"=="3" GOTO L01H
j64\jconsole -lib j.dll testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
GOTO L01C
:L01H
lldb -b -o run -k bt -k quit -- j64\jconsole.exe -lib j.dll testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
GOTO L01C
:L01A
dir j32
j32\jconsole testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
GOTO L01C
:L01B
dir jarm64
IF "%_DEBUG%"=="3" GOTO L01I
jarm64\jconsole testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
GOTO L01C
:L01I
lldb -b -o run -k bt -k quit -- jarm64\jconsole.exe -lib j.dll testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
:L01C
