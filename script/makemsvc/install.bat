rem %1% is folder with jsource

rem create jbld folders for j32 and j64

rmdir /S /Q jbld

mkdir jbld\j32
mkdir jbld\j64

xcopy /E /Y /Q %1%\jsource\jlibrary jbld\j32\
xcopy /E /Y /Q %1%\jsource\jlibrary jbld\j64\
