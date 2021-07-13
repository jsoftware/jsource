@rem build windows on github actions

mkdir j64
copy script\ver.ijs j64
copy openmp\obj\windows\libomp.dll j64

copy version.txt jsrc\jversion.h
echo #define jplatform "windows" >> jsrc\jversion.h
echo #define jlicense  "commercial" >> jsrc\jversion.h
echo #define jbuilder  "www.jsoftware.com" >> jsrc\jversion.h

cd makemsvc\jconsole
nmake -f makefile.win CC=clang-cl x64=1 clean
nmake -f makefile.win CC=clang-cl x64=1
cd ..
cd jdll
nmake -f makefile.win CC=clang-cl x64=1 JAVX2=1 USE_OPENMP=1 clean
nmake -f makefile.win CC=clang-cl x64=1 JAVX2=1 USE_OPENMP=1
nmake -f makefile.win CC=clang-cl x64=1 JAVX=1 USE_OPENMP=1 clean
nmake -f makefile.win CC=clang-cl x64=1 JAVX=1 USE_OPENMP=1
nmake -f makefile.win CC=clang-cl x64=1 USE_OPENMP=1 clean
nmake -f makefile.win CC=clang-cl x64=1 USE_OPENMP=1
cd ..
cd tsdll
nmake -f makefile.win CC=clang-cl x64=1 clean
nmake -f makefile.win CC=clang-cl x64=1
cd ..

copy jconsole\jconsole.exe ..\j64
copy jdll\*.dll ..\j64
copy tsdll\tsdll.dll ..\j64
