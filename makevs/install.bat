rem create new jbld folders for j32 and j64

cd %userprofile%

rmdir /S /Q jbld

mkdir jbld\j32
mkdir jbld\j64

xcopy /E /Y /Q git\jsource\jlibrary jbld\j32\
xcopy /E /Y /Q git\jsource\jlibrary jbld\j64\
    
