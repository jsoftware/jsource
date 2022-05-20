prolog './g1x1.ijs'
NB. 1!:1 ----------------------------------------------------------------

dir   =: 1!:0
read  =: 1!:1
write =: 1!:2
size  =: 1!:4
open  =: 1!:21
close =: 1!:22
erase =: 1!:55

x=: 9!:12 ''
win =: x e. 2 6
mac =: x e. 3
unix=: x e. 5 7
pc  =: x e. 0 1 2 6

p =: >{:4!:3 ''
p =: < p ([ }.~ [: - |.@[ i. ]) (pc#'/'),(mac#':'),unix#'/'
f =: p,&.><'g100.ijs'
h =: open f

(read f) -: read h
(read f) -: read <h
(size h) -: # read h
close h


f =: <jpath '~temp/foogoo5.x'
t =: (?1000$#a.){a.
t write f
h =: open f

t -: read f
t -: read h

erase f

d =: dir p,&.><'*.ijs'
(>2{"1 d) -: #@read p,&.>{."1 d

x=:'1'#~1 j. 1 2 4 8 4 2 1 0
t=:read 1   NB. read from keyboard
1 1  1    1        1    1  1 1
t -: x
t=:read <1  NB. read from keyboard
1 1  1    1        1    1  1 1
t -: x

'domain error'      -: read etx 'a'
'domain error'      -: read etx 'abc'
'domain error'      -: read etx 3.45
'domain error'      -: read etx 3j4

'rank error'        -: read etx <0 1 0
'rank error'        -: read etx <3 4
'rank error'        -: read etx <1 3$'abc'

'length error'      -: read etx <''
'length error'      -: read etx <i.0

'file name error'   -: read etx <'1234skidoo'

'file number error' -: read etx 0
'file number error' -: read etx 2
'file number error' -: read etx 12345 12346
'file number error' -: read etx <0
'file number error' -: read etx <2
'file number error' -: read etx <12345

4!:55 ;:'close d dir erase f h mac open p pc read size t unix win write x'



epilog''

