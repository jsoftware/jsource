0 : 0
VS13 template manager

jconsole/jdll/tsdl folders have templates for
.sln, .vcsproj, and .vcxproj.user files

git clone gets the template files and they
need to be copied to the files used by VS

the base files are in gitignore so are not in repo

changes to the base files that should be in the
repo must be copied to .template
and then must be copied to the base files to be used

template_status is currently only verb coded

routines to copy base files to template and template
to base files should probably be provided
but for now it must be done manually
)

vspath=: jpath'git/jsource/makevs/'

template_status=: 3 : 0
for_n. 'jdll';'jconsole';'tsdll' do.
 n=. ;n
 for_t. 'sln';'vcxproj';'vcxproj.user' do.
  n=. ;n
  a=. n,'/',n,'.',;t
  f=. vspath,a
  
  if. -.fexist f,'.template' do.
   echo 'missing  ',a,' ***************'
   continue.
  end.
  
  if. -.fexist f do.
    echo 'copied   ',a
    (fread f,'.template')fwrite f
  else.
   if. (fread f)-:fread f,'.template' do.
    echo 'same    ',a
   else.
    echo 'differs ',a,' ***********'
   end.
  end.
 end.
 echo' '
end.
)

