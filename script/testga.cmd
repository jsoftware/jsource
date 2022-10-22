@rem test windows on github actions

dir j64

j64\jconsole -lib j.dll testga.ijs
IF %ERRORLEVEL% NEQ 0 EXIT /b 1
@rem j64\jconsole -lib javx.dll testga.ijs
@rem IF %ERRORLEVEL% NEQ 0 EXIT /b 1
@rem j64\jconsole -lib javx2.dll testga.ijs
@rem IF %ERRORLEVEL% NEQ 0 EXIT /b 1

