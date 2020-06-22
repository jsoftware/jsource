0 : 0
VS2019 template manager

jconsole/jdll/tsdl template folders have templates for
.sln, .vcsproj, and .vcxproj.user files

git clone gets the template  folder files and they
need to be copied down a level (jconsolke/jdll/tsdll) to be by VS

the basejconsole/jdll/tsdll files are in gitignore so are not in repo

the template files have gitignore turned off so they are in repo

changes to the base files that should be in the
repo must be copied to template folder

template_status is currently only verb coded

routines to copy base files to template and template
to base files should probably be provided
but for now it must be done manually

windiff might help in looking at differences
)

echo 0 : 0
usage:
   template_status 'jconsole'
   template_status 'jdll'
   template_status 'tsdll'
)   

vspath=: jpath'git/jsource/makevs/'

template_status=: 3 : 0
'bad arg' assert (<y)e. 'jconsole';'jdll';'tsdll'
t=. '/*.sln';'/*.vcxproj';'/*vcxproj.user'
basep=. <vspath,y
base=. ;1 dir each basep,each t
base=. (>:;base i:each'/')}.each base

tmplp=. <vspath,y,'/template'
tmpl=. ;1 dir each tmplp,each t
tmpl=. (>:;tmpl i:each'/')}.each tmpl
  
a=. base-.tmpl
if. #a do.
 echo LF,'base files not in template folder'
 echo ' ',.>a
end.

a=. tmpl-.base
if. #a do.
 echo LF,'template files not in base folder'
 echo ' ',.>a
end.

inuse=. (;fexist basep,each,'/',each tmpl)#tmpl
a=. fread each basep,each'/',each inuse
b=. fread each tmplp,each'/',each inuse
d=. (a~:b)#inuse
if. #d do.
 echo LF,'template files that do not match base files'
 echo ' ',.>d
else.
 echo LF,'template files match base files'
end.
i.0 0
)

