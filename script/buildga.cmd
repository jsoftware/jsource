@rem build windows on github actions

xcopy /I /S jlibrary\*
copy script\testga.ijs
copy script\ver.ijs

mkdir j64
copy bin\profile.ijs j64
copy pthreads4w\bin\pthreadVC3.dll j64

copy version.txt jsrc\jversion.h
echo #define jplatform "windows" >> jsrc\jversion.h
echo #define jlicense  "commercial" >> jsrc\jversion.h
echo #define jbuilder  "www.jsoftware.com" >> jsrc\jversion.h

cd makemsvc\jconsole
nmake -f makefile.win CC=clang-cl x64=1 USE_PYXES=1 clean
nmake -f makefile.win CC=clang-cl x64=1 USE_PYXES=1
cd ..
cd jdll
nmake -f makefile.win CC=clang-cl x64=1 JAVX2=1 USE_OPENMP=0 USE_PYXES=1 clean
nmake -f makefile.win CC=clang-cl x64=1 JAVX2=1 USE_OPENMP=0 USE_PYXES=1
nmake -f makefile.win CC=clang-cl x64=1 JAVX=1 USE_OPENMP=0 USE_PYXES=1 clean
nmake -f makefile.win CC=clang-cl x64=1 JAVX=1 USE_OPENMP=0 USE_PYXES=1
nmake -f makefile.win CC=clang-cl x64=1 USE_OPENMP=0 USE_PYXES=1 clean
nmake -f makefile.win CC=clang-cl x64=1 USE_OPENMP=0 USE_PYXES=1
cd ..
cd tsdll
nmake -f makefile.win CC=clang-cl x64=1 clean
nmake -f makefile.win CC=clang-cl x64=1
cd ..

copy jconsole\jconsole.exe ..\j64
copy jdll\*.dll ..\j64
copy tsdll\tsdll.dll ..\j64
