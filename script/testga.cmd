@rem test windows on github actions

systeminfo

IF "%~1"=="x86" GOTO L01A
dir j64
j64\jconsole -lib j.dll testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
j64\jconsole -lib javx2.dll testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
GOTO L01B
:L01A
dir j32
j32\jconsole testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
:L01B

