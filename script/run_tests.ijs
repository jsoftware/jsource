NB. script to run all tests for a machine
NB. rough version and not complete

NB. todo - macos and windows

output=: jpath'~temp/run_tests.txt'

NB.!while testing - 'RUN 3{.ddall' runit 'libj.so'
runit=: 4 : 0
output fappend~ LF,LF,'************************************************************  ',x,'  ',y,LF
t=. 'echo "load''git/jsource/test/tsu.ijs''\necho CMD" | jbld/j64/bin/jconsole -lib JX >>"OUT"'
t=. t rplc 'CMD';x;'JX';y;'OUT';hostpathsep output
spawn_jtask_ t
)

runall=: 3 : 0
ferase output
'RUN ddall' runit 'libj.so'
'RUN ddall' runit 'libjavx.so'
'RUN ddall' runit 'libjavx2.so'

'runpacman''''' runit 'libj.so'
'runpacman''''' runit 'libjavx.so'
'runpacman''''' runit 'libjavx2.so'

'runjd''''' runit 'libj.so'
'runjd''''' runit 'libjavx.so'
'runjd''''' runit 'libjavx2.so'

check''
)

NB. report important (error) lines in output
check=: 3 : 0
d=. <;._2 fread output
d=. d-.<'0 failed'
b=. 'g'=;{.each d
b=. b+.;+./each(<'failed') E.each d
b=. b+.;+./each(<'error')  E.each d
;(+./b){'all ok';'errors (fread output) !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!'
)