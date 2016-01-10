NB. 1!:3 ----------------------------------------------------------------

read   =. 1!:1
write  =. 1!:2
append =. 1!:3
erase  =. 1!:55

f =. <'foogQ0m1.x'
x =. (?1000$#a.){a.
x append f

x-:read f
x-:read f [ y append f [ x=.x,y=.a.{~?(>:?1000)$#a.
x-:read f [ y append f [ x=.x,y=.'j'
x-:read f [ y append f [ x=.x,y=.''

erase f

'domain error'      -: 'abcd'  append etx 'a'
'domain error'      -: 'abcd'  append etx 'abc'
'domain error'      -: 'abcd'  append etx 3.45
'domain error'      -: 'abcd'  append etx 3j4

'domain error'      -: 0 1 0   append etx <'asdf'
'domain error'      -: 3 4     append etx <'asdf'
'domain error'      -: 3.4 5.6 append etx <'asdf'
'domain error'      -: 3j4 0   append etx <'asdf'
'domain error'      -: (<'abc')append etx <'asdf'

'rank error'        -: 'abcd'  append etx <0 1 0
'rank error'        -: 'abcd'  append etx <3 4
'rank error'        -: 'abcd'  append etx <1 3$'abc'

'rank error'        -: (,.'ab')append etx <'asdf'

'length error'      -: 'abcd'  append etx <''
'length error'      -: 'abcd'  append etx <i.0

'file number error' -: 'abcd'  append etx 0
'file number error' -: 'abcd'  append etx 1
'file number error' -: 'abcd'  append etx 3
'file number error' -: 'abcd'  append etx 345
'file number error' -: 'abcd'  append etx <0
'file number error' -: 'abcd'  append etx <1
'file number error' -: 'abcd'  append etx <3
'file number error' -: 'abcd'  append etx <345

'domain error'      -: 3j4 0 append etx <'noQsuch'
'file name error'   -: read etx <'noQsuch'

mac =. 3 = 9!:12 ''
f   =. mac{'no/such/dir/or/file';':no:such:dir:or:file'
'file name error'   -: 'asdf' append etx f

4!:55 ;:'append erase f mac read write x y '


