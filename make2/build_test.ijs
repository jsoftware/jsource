man=: 0 : 0
manage building/testing J releases/betas from J
uses make2 sh script to build binaries
all binaries are copied to git/jlibrary/bin with qualified names (e.g. libjavx2.so)


!!! this J script is NOT ignored by gitignore - changes will be pushed

   build'jconsole'
   build'libtsdll'
   build'libj'    NB. 'libjavx' 'libjavx2'
   build_all''    NB. build all
   
   run'libj'      NB. 'libjavx' 'libjavx2'
   runtests'libj' NB. 'libjavx' 'libjavx2'
   runtests_all'' NB. tests on all
)   

load'~addons/misc/miscutils/utils.ijs'

build_test=: 3 : '' NB. for whichscript

pmake2=: (t i:'/'){.t=. ;whichscript'build_test' NB. path to make2 folder
ptemp=:  jpath'~temp/jbuild'

mkdir_j_ jpath ptemp

stdout=: ptemp,'/stdout.txt'
stderr=: ptemp,'/stderr.txt'
psh=:    ptemp,'/build.sh'
clean=: './clean.sh' NB. set clean=: '' to avoid compiles

3 : 0'' 
if. UNAME-:'Linux' do.
 platform=: 'linux'
 compiler=: 'clang-8'
 suffix=: '.so'
 terminal=: 'x-terminal-emulator -e '
else.
 platform=: 'darwin'
 compiler=: 'clang'
 suffix=: '.dylib'
 terminal=: ''
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
runpacman''
runjd''
exit''
)

(t,rt) fwrite ptemp,'/runtests.ijs'

shtemplate=: 0 : 0
target=../jlibrary/bin/TARGETSUFFIX
echo $target
cd MAKE2
jplatform=PLATFORM
CC=COMPILER
export CC
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
target=../jlibrary/bin/TARGET
echo $target
cd MAKE2
jplatform=PLATFORM
CC=COMPILER
export CC
./clean.sh
rm -f $target
)

shjconsole=: (0 : 0 rplc 'COMMON';shcommon) rplc 'TARGET';'jconsole';same
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
build each 'jconsole';'libtsdll';'libj';'libjavx';'libjavx2'
)

NB. run libj or libjavx or libjavx2
run=: 3 : 0
t=. terminal,pmake2,'/../jlibrary/bin/jconsole -lib ',y,suffix,' ',ptemp,'/run.ijs'
fork_jtask_ t
)

runmac=: 3 : 0
t=. terminal,pmake2,'/../jlibrary/bin/jconsole -lib ',y,suffix,' ',ptemp,'/run.ijs'
fork_jtask_ t
)


runtests=: 3 : 0
t=. pmake2,'/../jlibrary/bin/jconsole -lib ',y,suffix,' ',ptemp,'/runddall.ijs'
spawn_jtask_ t
)

runtests_all=: 3 : 0
t=. LF,LF,80#'*'
echo runtests'libj'
echo t
echo runtests'libjavx'
echo t
echo runtests'libjavx2'
i.0 0
)