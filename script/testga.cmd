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
dir j64
echo "A1"
IF "%USE_EMU_AVX%"=="0" GOTO L01F
echo "A2"
IF "%USE_PYXES%"=="0" GOTO L01F
echo "A3"
j64\jconsole -lib javx2.dll testga.ijs
echo "A4"
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
:L01F
echo "A5"
j64\jconsole -lib j.dll testga.ijs
echo "A6"
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
GOTO L01C
:L01A
dir j32
j32\jconsole testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
GOTO L01C
:L01B
dir jarm64
jarm64\jconsole testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
:L01C
