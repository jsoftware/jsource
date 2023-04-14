@rem build windows on github actions

xcopy /Y /I /S jlibrary\*
copy script\testga.ijs
copy script\ver.ijs

mkdir j32
copy bin\profile.ijs j32
copy pthreads4w\bin\pthreadVC3.dll j32
copy mpir\windows\win32\mpir.dll j32

copy version.txt jsrc\jversion.h
echo #define jplatform "windows" >> jsrc\jversion.h
echo #define jlicense  "commercial" >> jsrc\jversion.h
echo #define jbuilder  "www.jsoftware.com" >> jsrc\jversion.h

cd makemsvc\jconsole
@rem nmake -f makefile.win CC=clang-cl x64=1 USE_PYXES=1 clean
@rem nmake -f makefile.win CC=clang-cl x64=1 USE_PYXES=1
@rem IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
nmake -f makefile.win CC=clang-cl x64=0 USE_PYXES=1 clean
nmake -f makefile.win CC=clang-cl x64=0 USE_PYXES=1
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
cd ..
cd jdll
@rem nmake -f makefile.win CC=clang-cl x64=1 JAVX512=1 USE_OPENMP=0 USE_PYXES=1 clean
@rem nmake -f makefile.win CC=clang-cl x64=1 JAVX512=1 USE_OPENMP=0 USE_PYXES=1
@rem IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
@rem nmake -f makefile.win CC=clang-cl x64=1 JAVX512=0 JAVX2=1 USE_OPENMP=0 USE_PYXES=1 clean
@rem nmake -f makefile.win CC=clang-cl x64=1 JAVX512=0 JAVX2=1 USE_OPENMP=0 USE_PYXES=1
@rem IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
@rem nmake -f makefile.win CC=clang-cl x64=1 JAVX512=0 JAVX2=0 USE_OPENMP=0 USE_PYXES=1 clean
@rem nmake -f makefile.win CC=clang-cl x64=1 JAVX512=0 JAVX2=0 USE_OPENMP=0 USE_PYXES=1
@rem IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
nmake -f makefile.win CC=clang-cl x64=0 USE_OPENMP=0 USE_PYXES=1 clean
nmake -f makefile.win CC=clang-cl x64=0 USE_OPENMP=0 USE_PYXES=1
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
cd ..
cd tsdll
@rem nmake -f makefile.win CC=clang-cl x64=1 clean
@rem nmake -f makefile.win CC=clang-cl x64=1
@rem IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
nmake -f makefile.win CC=clang-cl x64=0 clean
nmake -f makefile.win CC=clang-cl x64=0
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
cd ..
cd jamalgam
@rem nmake -f makefile.win CC=clang-cl x64=1 JAVX512=0 JAVX2=0 USE_OPENMP=0 USE_PYXES=1 clean
@rem nmake -f makefile.win CC=clang-cl x64=1 JAVX512=0 JAVX2=0 USE_OPENMP=0 USE_PYXES=1
@rem IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
nmake -f makefile.win CC=clang-cl x64=0 USE_OPENMP=0 USE_PYXES=1 clean
nmake -f makefile.win CC=clang-cl x64=0 USE_OPENMP=0 USE_PYXES=1
IF %ERRORLEVEL% NEQ 0 EXIT /b %ERRORLEVEL%
cd ..

copy jconsole\jconsole.exe ..\j32
copy jdll\*.dll ..\j32
copy tsdll\tsdll.dll ..\j32
copy jamalgam\jamalgam.exe ..\j32
