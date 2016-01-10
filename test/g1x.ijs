NB. 1!: -----------------------------------------------------------------

read   =. 1!:1
write  =. 1!:2
append =. 1!:3
size   =. 1!:4
erase  =. 1!:55

mtm =. i. 0 0

t =. a.,":?~300
mtm -: t write <'junkfoo'
t =. read <'junkfoo'
1    -: #$t
2    -: type t
(#t) -: size <'junkfoo'

mtm -: (|.t) write <'oofknuj'
(#t) =  size 'junkfoo';'oofknuj'
x =. read <'oofknuj'
x -: |.t

mtm -: t append <'oofknuj'
(2*#t) = size <'oofknuj'
x =. read <'oofknuj'
x -: (|.t),t

erase 'junkfoo';'oofknuj'


NB. 1!: terminal input/output -------------------------------------------

in  =. 1!:1
out =. 1!:2&2

NB. 0 0$out 'this line should appear in the output'
t =. in 1
1   1  1 1
t -: '1   1  1 1'
t =. in 1
1
t -: ,'1'


NB. 1!: -----------------------------------------------------------------

read   =. 1!:1
write  =. 1!:2
append =. 1!:3
size   =. 1!:4
erase  =. 1!:55

'file name error'   -:       read   etx <'J9k8L7m6' 
'file name error'   -:       size   etx <'J9k8L7m6'
(<erase etx <'NoSuch.943') e. 'file name error';'interface error'

'domain error'      -:       read   etx 'ab'
'domain error'      -: 'abc' write  etx 'ab'
'domain error'      -: 'abc' append etx 'ab'
'domain error'      -:       size   etx 'ab'
'domain error'      -:       erase  etx 'ab'

'file number error' -:       read   etx 0
'file number error' -: 'abc' write  etx 0
'file number error' -: 'abc' append etx 0
'file number error' -:       size   etx 0
'file number error' -:       erase  etx 0

'file number error' -: 'abc' write  etx 1
'file number error' -: 'abc' append etx 1
'file number error' -:       size   etx 1
'file number error' -:       erase  etx 1

'file number error' -:       read   etx 2
'file number error' -:       size   etx 2
'file number error' -:       erase  etx 2

'file number error' -:       read   etx <2
'file number error' -: 'abc' write  etx <1
'file number error' -: 'abc' append etx <1
'file number error' -:       size   etx <1
'file number error' -:       erase  etx <1

'file number error' -:       read   etx 68
'file number error' -: 'abc' write  etx 68
'file number error' -: 'abc' append etx 68
'file number error' -:       size   etx 68
'file number error' -:       erase  etx 68

'file number error' -:       read   etx <68
'file number error' -: 'abc' write  etx <68
'file number error' -: 'abc' append etx <68
'file number error' -:       size   etx <68
'file number error' -:       erase  etx <68

'domain error'      -:       read   etx 3.5
'domain error'      -: 'abc' write  etx 3.5
'domain error'      -: 'abc' append etx 3.5
'domain error'      -:       size   etx 3.5
'domain error'      -:       erase  etx 3.5

'domain error'      -:       read   etx 3j5
'domain error'      -: 'abc' write  etx 3j5
'domain error'      -: 'abc' append etx 3j5
'domain error'      -:       size   etx 3j5
'domain error'      -:       erase  etx 3j5

'domain error'      -:       read   etx <3.5
'domain error'      -: 'abc' write  etx <3.5
'domain error'      -: 'abc' append etx <3.5
'domain error'      -:       size   etx <3.5
'domain error'      -:       erase  etx <3.5

'domain error'      -:       read   etx <3j5
'domain error'      -: 'abc' write  etx <3j5
'domain error'      -: 'abc' append etx <3j5
'domain error'      -:       size   etx <3j5
'domain error'      -:       erase  etx <3j5

'rank error'        -:       read   etx <3 4$'a'
'rank error'        -: 'abc' write  etx <3 4$'a'
'rank error'        -: 'abc' append etx <3 4$'a'
'rank error'        -:       size   etx <3 4$'a'
'rank error'        -:       erase  etx <3 4$'a'

'length error'      -:       read   etx <''
'length error'      -: 'abc' write  etx <''
'length error'      -: 'abc' append etx <''
'length error'      -:       size   etx <''
'length error'      -:       erase  etx <''

'domain error'      -: 1     write  etx <'abc'
'domain error'      -: 3     write  etx <'abc'
'domain error'      -: 3.4   write  etx <'abc'
'domain error'      -: 3j4   write  etx <'abc'
'domain error'      -: (<'a')write  etx <'abc'

'domain error'      -: 1 0 1 append etx <'abc'
'domain error'      -: 3 4   append etx <'abc'
'domain error'      -: 3.4 5 append etx <'abc'
'domain error'      -: 3j4   append etx <'abc'
'domain error'      -: (<'a')append etx <'abc'

'rank error'        -: (3 4$'a') write  etx <'abc'
'rank error'        -: (3 4$'a') append etx <'abc'


4!:55 ;:'append erase in mtm out read size t write x'


