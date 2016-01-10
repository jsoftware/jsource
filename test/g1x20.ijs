NB. 1!:20 ---------------------------------------------------------------

dir   =. 1!:0
write =. 1!:2
files =. 1!:20
open  =. 1!:21
close =. 1!:22
erase =. 1!:55

x=. 9!:12 ''
win =. x e. 2 6
mac =. x e. 3
unix=. x e. 5 7
pc  =. x e. 0 1 2 6

f =. <'foo.x'
g =. <'g'

(128$'a') write f
h =. open f
h -: open f
h -: open h
h -: open <h
h e. >{."1 files ''
erase h
(214$'triskaidekaphobic') write g
q =. files ''
h =. open <'non1such.x'
erase g,<'non1such.x'
q -: files ''

0~:open g
close g
q -: files ''
erase g

d =. ({. ~ 5: <. #) {."1 dir mac{'../js/*.*';'::js:'
h =. open (<(unix#''),(pc#'../js/'),mac#':temp:'),&.>d
h e. >{."1 files ''
q e. files ''
close h
q -: files ''

x -: open x=.''
x -: open x=.i.0 1 2
x -: open x=.0 1 2$a:

x -: close x=.''
x -: close x=.i.0 1 2
x -: close x=.0 1 2$a:

'domain error'      -: open  etx 'a'
'domain error'      -: open  etx 'abc'
'domain error'      -: open  etx 3.45
'domain error'      -: open  etx <3.45
'domain error'      -: open  etx 3j4
'domain error'      -: open  etx <3j4

'rank error'        -: open  etx <3 4$'a'
'rank error'        -: open  etx <21 31

'length error'      -: open  etx <''
'length error'      -: open  etx <i.0

'file number error' -: open  etx 0
'file number error' -: open  etx 1
'file number error' -: open  etx 2
'file number error' -: open  etx 3 4 5
'file number error' -: open  etx 0;1;0
'file number error' -: open  etx 3;4;5

t=.(pc#'does\not\exist\no.x'),(mac#':does:not:exist:no.x'),unix#'does/not/exist/no.x'
'file name error'   -: open  etx <t

'domain error'      -: close etx 'abc'
'domain error'      -: close etx 3.45
'domain error'      -: close etx <3.45
'domain error'      -: close etx 3j4 
'domain error'      -: close etx <3j4 

'rank error'        -: close etx <1 4$'a'
'rank error'        -: close etx <21 31

'length error'      -: close etx <''
'length error'      -: close etx <i.0

'file number error' -: close etx 0 
'file number error' -: close etx 1
'file number error' -: close etx 2
'file number error' -: close etx 0 1
'file number error' -: close etx i.2 4
'file number error' -: close etx 12345 12346
'file number error' -: close etx 0;1;0
'file number error' -: close etx 4;5;6

4!:55 ;:'close d dir erase f files g h mac open pc q t unix win write x'


