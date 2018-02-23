rem copy git jlibrary to jbld j32/j64

cd %userprofile%

rmdir /S /Q jbld

mkdir jbld\j32
mkdir jbld\j64

xcopy /E /Y /Q gitdev\jsource\jlibrary jbld\j32\
xcopy /E /Y /Q gitdev\jsource\jlibrary jbld\j64\
    
