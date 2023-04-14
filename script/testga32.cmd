@rem test windows on github actions

systeminfo

dir j32

j32\jconsole -lib j.dll testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
@rem j32\jconsole -lib javx2.dll testga.ijs
@rem IF %ERRORLEVEL% NEQ 0 EXIT /b 1

