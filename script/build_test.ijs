man=: 0 : 0
manage building/testing J releases/betas from J

   git_status''           NB. report git status and jversion
   build_for'J904-beta-e' NB. set build globals and jversion.h
   
tags
 $ cd git/jsource
 $ git checkout master
 $ git pull
 
 $ cd ~
 $ git/jsource/script/tag.sh j904-beta-?
 $ cd git/jsource
 $ git checkout tags/j904-beta-x



build uses make2 for linux/macos
JE binaries are copied to git/jlibrary/bin with qualified names (e.g. libjavx2.so)
   build'jconsole'
   build'libtsdll'
   build'libj'       NB. 'libjavx' 'libjavx2' - with current jvserion.h
   build_all''       NB. build all - with current jversion.h
   
   get_jversion''
   set_jversion''    NB. version and type set by build_for
   build_for'J904-beta-f'

windows builds done with vs2019

fork - linux/macos/windows   
   run'libj'      NB. 'libjavx' 'libjavx2'
   
spawn - linux/macos/windows   
   runit LIB       ;FILE
   runit 'libj'    ;'rundd.ijs'
   runit 'libjavx2';'rundd.ijs'
   
   runit 'libj'    ;'runpm.ijs' NB. pacman
   runit 'libj'    ;'runjd.ijs' NB. jd
   
   runit_all''   NB. ddall/pacman/jd on all - written to file
   
*** publish release/beta/maintenace
   git branch -a  -list branches
   git checkout branch
   
   build_for 'J904-beta-f'
   

)   

load'~addons/misc/miscutils/utils.ijs'

pgit=:      'git/jsource'
pversionh=: pgit,'/jsrc/jversion.h'

jversion_template=: 0 : 0
#define jversion  "XXX"
#define jplatform "PLATFORM"
#define jtype     "TYPE"
#define jlicense  "commercial"
#define jbuilder  "www.jsoftware.com"
)

build_for=: 3 : 0
'invalid'assert 'J-'=0 4{y
v=: 3{.}.y
t=: 5}.y
'bad jversion'assert +/902 903 904=0".v
'bad jtype'assert (('beta-'-:5{.t)*.6=#t)+.('release-'-:8{.t)*.9=#t
version=: v
type=: t
platform=: ;(('Win';'Linux';'Darwin')i.<UNAME){'windows';'linux';'darwin'
t=. jversion_template rplc 'XXX';v;'PLATFORM';platform;'TYPE';type
t fwrite pversionh
git_status''
)

git_status=: 3 : 0
echo shell_jtask_'cd git/jsource ; git status'
echo LF,get_jversion''
)

git_log=: 3 : 0
shell_jtask_ 'cd git/jsource ; git log --pretty=format:"%h%x09%an%x09%ad%x09%s" >~/git.log'
2000{.fread 'git.log'
)

build_test=: 3 : '' NB. for whichscript

pmake2=: pgit,'/make2'
ptemp=:  jpath'~temp/jbuild'

mkdir_j_ jpath ptemp

stdout=: ptemp,'/stdout.txt'
stderr=: ptemp,'/stderr.txt'
psh=:    ptemp,'/build.sh'
clean=: './clean.sh' NB. set clean=: '' to avoid compiles

3 : 0'' 
if. UNAME-:'Linux' do.
 platform=: 'linux'
 compiler=: 'clang'
 suffix=: '.so'
 terminal=: 'x-terminal-emulator -e '
else.
 platform=: 'darwin'
 compiler=: 'clang'
 suffix=: '.dylib'
 terminal=: 'open '
end.
)

'' fwrite psh
spawn_jtask_ 'chmod +x ',psh NB. ensure build.sh is +x

t=. 'load''',pmake2,'/../test/tsu.ijs'''
t fwrite ptemp,'/run.ijs' NB. script to load tsu.ijs

rt=. 0 : 0
RUN ddall
echo LF,'failed: ',":#RBAD''
echo RBAD''
exit''
)
(t,LF,rt) fwrite ptemp,'/rundd.ijs'

rt=. 0 : 0
runpacman''
exit''
)
(t,LF,rt) fwrite ptemp,'/runpm.ijs'

rt=. 0 : 0
runjd''
exit''
)
(t,LF,rt) fwrite ptemp,'/runjd.ijs'

shtemplate=: 0 : 0
target=../jlibrary/bin/TARGETSUFFIX
echo $target
cd MAKE2
jplatform=PLATFORM
CC=COMPILER
export CC
USE_SLEEF=1
export USE_SLEEF
USE_PYXES=1
export USE_PYXES
CLEAN
j64x=j64TYPE ./build_libj.sh  
cp ../bin/$jplatform/j64TYPE/libjSUFFIX $target
echo done
)

same=: 'SUFFIX';suffix;'PLATFORM';platform;'MAKE2';pmake2;'CLEAN';clean;'COMPILER';compiler
shlibj    =: shtemplate rplc 'TARGET';'libj'    ;'TYPE';''    ;same
shlibjavx =: shtemplate rplc 'TARGET';'libjavx' ;'TYPE';'avx' ;same
shlibjavx2=: shtemplate rplc 'TARGET';'libjavx2';'TYPE';'avx2';same

shcommon=: }:0 : 0 rplc 'MAKE2';pmake2
target=../jlibrary/bin/TARGETSUFFIX
echo $target
cd MAKE2
jplatform=PLATFORM
CC=COMPILER
export CC
./clean.sh
rm -f $target
)

shjconsole=: (0 : 0 rplc 'COMMON';shcommon) rplc 'TARGET';'jconsole';'SUFFIX';'';'PLATFORM';platform;'MAKE2';pmake2;'CLEAN';clean;'COMPILER';compiler
COMMON
j64x=j64 ./build_jconsole.sh  
cp ../bin/$jplatform/j64/jconsole $target 
echo done
)

shlibtsdll=: (0 : 0 rplc 'COMMON';shcommon) rplc 'TARGET';'libtsdll';same
COMMON
j64x=j64 ./build_tsdll.sh  
cp ../bin/$jplatform/j64/libtsdllSUFFIX $target
echo done
)

get_jversion=: 3 : 0
fread pversionh
)

build=: 3 : 0
suf=. suffix
select. y
case. 'jconsole' do.
 suf=. ''
 shjconsole fwrite psh
case. 'libtsdll' do.
 shlibtsdll fwrite psh
case. 'libj' do.
 shlibj fwrite psh
case. 'libjavx' do.
 shlibjavx fwrite psh
case. 'libjavx2' do.
 shlibjavx2 fwrite psh
case.            do.
 'unknown build' assert 0
end.
spawn_jtask_   (ptemp,'/build.sh'),' 1> ',stdout,' 2> ',stderr
echo fread stderr
'build failed'assert 0=#fread stderr
'target not created' assert fexist pmake2,'/../jlibrary/bin/',y,suf
)

build_all=: 3 : 0
'do not run in JHS'assert -.IFJHS
git_status''
build each 'jconsole';'libtsdll';'libj';'libjavx';'libjavx2'
)

NB. run libj or libjavx or libjavx2
run=: 3 : 0
if. UNAME-:'Linux' do.
 t=. terminal,pmake2,'/../jlibrary/bin/jconsole -lib ',y,suffix,' ',ptemp,'/run.ijs'
 fork_jtask_ t
else.
 pmac=. ptemp,'/macrun.command'
 (macrun rplc 'LIB';y)fwrite pmac
 spawn_jtask_ 'chmod +x ',pmac
 fork_jtask_'open ',pmac
end.
)

runit=: 3 : 0
'LIB FILE'=: y
t=. pmake2,'/../jlibrary/bin/jconsole -lib LIBSUFFIX ',ptemp,'/FILE'
t=. t rplc 'LIB';LIB;'SUFFIX';suffix;'FILE';FILE
a=. LF,~(80#'*'),;' ',~each y
LF,a,spawn_jtask_ t 
)

rundd=: 3 : 0
t=. pmake2,'/../jlibrary/bin/jconsole -lib ',y,'SUFFIX ',ptemp,'/runddall.ijs'
spawn_jtask_ t
)

report=: ptemp,'/report.txt'

runit_all=: 3 : 0
report fwrite~ ''
report fappend~runit'libj'    ;'rundd.ijs'
report fappend~runit'libjavx' ;'rundd.ijs'
report fappend~runit'libjavx2';'rundd.ijs'

report fappend~runit'libj'    ;'runpm.ijs'
report fappend~runit'libjavx' ;'runpm.ijs'
report fappend~runit'libjavx2';'runpm.ijs'

report fappend~runit'libj'    ;'runjd.ijs'
report fappend~runit'libjavx' ;'runjd.ijs'
report fappend~runit'libjavx2';'runjd.ijs'

echo 'fread report'
check_report''
)


check_report=: 3 : 0
r=. <;._2 fread report
t=. 'failed: 0'
if. 3~:+/;(<t)=(#t){.each r do. echo 'RUN failures' end.

t=. 'Done.'
if. 3~:+/;(<t)=(#t){.each r do. echo 'pacman failures' end.

t=. '0 failed'
if. 3~:+/;(<t)=(#t){.each r do.
 echo 'jd failures'
 t=. 'loadd'
 echo >' ',each~.((<t)=(#t){.each r)#r
end. 
i.0 0
)

macrun=: 0 : 0
#!/bin/sh
/users/eric/git/jsource/jlibrary/bin/jconsole  -lib LIB.dylib git/jsource/test/tsu.ijs
)
