rem %1 is path to source library
rem ....\copy_library_from_current j64-904

cd /users/eric/gitdev/jsource/jlibrary

rmdir /S /Q addons
rmdir /S /Q bin
rmdir /S /Q system

mkdir addons
mkdir addons\ide
mkdir addons\ide\jhs
mkdir addons\data
mkdir addons\data\jmf

xcopy %userprofile%\%1\addons\ide\jhs  addons\ide\jhs  /S /Q
xcopy %userprofile%\%1\addons\data\jmf addons\data\jmf /S /Q

mkdir bin
copy %userprofile%\%1\bin\profile.ijs bin\profile.ijs

mkdir system
xcopy %userprofile%\%1\system  system  /S /Q

cd %userprofile%
