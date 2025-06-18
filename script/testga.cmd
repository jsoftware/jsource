@rem test windows on github actions

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
j64\jconsole -lib j.dll testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
j64\jconsole -lib javx2.dll testga.ijs
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
