NB. 1!:2 ----------------------------------------------------------------

read  =. 1!:1
write =. 1!:2
erase =. 1!:55

f =. <'foogQ0m1.x'
x =. (?1000$#a.){a.
x write f

x-:read f
x-:read f [ (x=.a.{~?(>:?1000)$#a.) write f
x-:read f [ x=.,'j' [ 'j' write f
x-:read f [ (x=.'') write f

erase f

NB. 0 0$((0j7,1 j.0,~2^i.5)#'1') write 2  
NB. 1 1  1    1        1                1   The 1s should line up
  
read  =. 1!:1
write =. 1!:2

'domain error'      -: 'abcd'  write etx 'a'
'domain error'      -: 'abcd'  write etx 'abc'
'domain error'      -: 'abcd'  write etx 3.45
'domain error'      -: 'abcd'  write etx 3j4

'domain error'      -: 0 1 0   write etx <'asdf'
'domain error'      -: 3 4     write etx <'asdf'
'domain error'      -: 3.4 5.6 write etx <'asdf'
'domain error'      -: 3j4 0   write etx <'asdf'
'domain error'      -: (<'abc')write etx <'asdf'

'rank error'        -: 'abcd'  write etx <0 1 0
'rank error'        -: 'abcd'  write etx <3 4
'rank error'        -: 'abcd'  write etx <1 3$'abc'

'rank error'        -: (,.'ab')write etx <'asdf'

'length error'      -: 'abcd'  write etx <''
'length error'      -: 'abcd'  write etx <i.0

'file number error' -: 'abcd'  write etx 0
'file number error' -: 'abcd'  write etx 1
'file number error' -: 'abcd'  write etx 3
'file number error' -: 'abcd'  write etx 345
'file number error' -: 'abcd'  write etx <0
'file number error' -: 'abcd'  write etx <1
'file number error' -: 'abcd'  write etx <3
'file number error' -: 'abcd'  write etx <345

'domain error'      -: 3j4 0 write etx <'noQsuch'
'file name error'   -: read etx <'noQsuch'

x=. 9!:12 ''
win =. x e. 2 6
mac =. x e. 3
unix=. x e. 5 7
pc  =. x e. 0 1 2 6

f   =. mac{'no/such/dir/or/file';':no:such:dir:or:file'

'file name error'   -: 'asdf'  write etx f

4!:55 ;:'erase f mac pc read unix win write x'


