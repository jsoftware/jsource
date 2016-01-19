rem %1% is folder with jsource
rem copy sln and vcxproj files from .template files

set p="%1%\jsource\makevs\jconsole"
copy %p%\jconsole.sln.template     %p%\jconsole.sln
copy %p%\jconsole.vcxproj.template %p%\jconsole.vcxproj

set p="%1%\jsource\makevs\jdll"
copy %p%\jdll.sln.template     %p%\jdll.sln
copy %p%\jdll.vcxproj.template %p%\jdll.vcxproj

set p="%1%\jsource\makevs\tsdll"
copy %p%\tsdll.sln.template     %p%\tsdll.sln
copy %p%\tsdll.vcxproj.template %p%\tsdll.vcxproj

