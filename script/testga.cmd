@rem test windows on github actions

dir j64

j64\jconsole -lib j.dll testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
