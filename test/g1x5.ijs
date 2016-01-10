NB. 1!:5 ----------------------------------------------------------------

dir   =. 1!:0
read  =. 1!:1
write =. 1!:2
mkdir =. 1!:5
att   =. 1!:6
erase =. 1!:55

x=. 9!:12 ''
win =. x e. 2 6
mac =. x e. 3
unix=. x e. 5 7
pc  =. x e. 0 1 2 6

p=: (1!:43 ''),mac{'/:'

mkdir d=.<'brandnew'
". pc#'''----d-'' -: att d'

f=. p&,&.> mac{'brandnew/foo.x';':brandnew:foo.x'
0 0$(x=.a.{~?500$#a.) write f
x -: read f
(<'foo.x') -: _5{.&.> {.,dir p&,&.>mac{'brandnew/*.*';':brandnew'

erase f,d

'domain error'      -: mkdir etx 0
'domain error'      -: mkdir etx 2 
'domain error'      -: mkdir etx 'a'
'domain error'      -: mkdir etx 3.45
'domain error'      -: mkdir etx 3j4
'domain error'      -: mkdir etx <0 1 0
'domain error'      -: mkdir etx <1 2 3 4

'rank error'        -: mkdir etx <1 3$'abc'

badf =. e.&('interface error';'file name error')
badf <mkdir etx <''
badf <mkdir etx mac{'does/not/exist';':does:not:exist'
badf <mkdir etx <':bad:/bad/\bad\'

'asdf' write <'conflict'
bada =. e.&('interface error';'file access error')
bada <mkdir etx <'conflict'
erase <'conflict'

4!:55 ;:'att bada badf d dir erase f mac mkdir p pc '
4!:55 ;:'read unix win write x '


