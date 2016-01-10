NB. 1!:12 ---------------------------------------------------------------

read   =. 1!:1
write  =. 1!:2
iread  =. 1!:11
iwrite =. 1!:12
open   =. 1!:21
erase  =. 1!:55

f =. <'foo1x12.x'
x =. (?1000$#a.){a.
x write f
h =. open f

'' iwrite h,?#x
x -: read h

y =. 'Cogito, ergo sum.'
y iwrite h,100
y -: iread h,100,#y

'plangent' iwrite f,<_1
'plangent' -: iread h,(_1+#x),8

erase h
  
f =. <'foo1x12.x'
'0123456789' write f
h =. open f

'domain error'      -: 'asdf' iwrite etx 'abc'
'domain error'      -: 'asdf' iwrite etx 3.4 5  
'domain error'      -: 'asdf' iwrite etx 3j4 2 3
'domain error'      -: 'asdf' iwrite etx f,<'ab'
'domain error'      -: 'asdf' iwrite etx f,<3.4 5
'domain error'      -: 'asdf' iwrite etx f,<3j4 5
'domain error'      -: 'asdf' iwrite etx f,<<3
'domain error'      -: 'asdf' iwrite etx h;'ab'
'domain error'      -: 'asdf' iwrite etx h;3.4 5
'domain error'      -: 'asdf' iwrite etx h;3j4 5

'domain error'      -: 0 1    iwrite etx h,1
'domain error'      -: 3 4 5  iwrite etx h,1
'domain error'      -: 3.4 5  iwrite etx h,1
'domain error'      -: 3j4 5  iwrite etx h,1
'domain error'      -: (<'ab')iwrite etx h,1
'domain error'      -: 0 1    iwrite etx h;1
'domain error'      -: 3 4 5  iwrite etx h;1
'domain error'      -: 3.4 5  iwrite etx h;1
'domain error'      -: 3j4 5  iwrite etx h;1
'domain error'      -: (<'ab')iwrite etx h;1
'domain error'      -: 0 1    iwrite etx f,<1
'domain error'      -: 3 4 5  iwrite etx f,<1
'domain error'      -: 3.4 5  iwrite etx f,<1
'domain error'      -: 3j4 5  iwrite etx f,<1
'domain error'      -: (<'ab')iwrite etx f,<1

'rank error'        -: 'asdf' iwrite etx 0 1 0;2
'rank error'        -: 'asdf' iwrite etx 3 4;2 
'rank error'        -: 'asdf' iwrite etx (1 3$'abc');2 3
'rank error'        -: 'asdf' iwrite etx f,<1 1$2
'rank error'        -: (,.a.) iwrite etx h,1
'rank error'        -: (,.a.) iwrite etx h;1
'rank error'        -: (,.a.) iwrite etx f,<1

'length error'      -: 'asdf' iwrite etx 'a'
'length error'      -: 'asdf' iwrite etx '';2 3
'length error'      -: 'asdf' iwrite etx (i.0);2 3
'length error'      -: 'asdf' iwrite etx h;i.0
'length error'      -: 'asdf' iwrite etx ,f
'length error'      -: 'asdf' iwrite etx ,h
'length error'      -: 'asdf' iwrite etx f,<2 3 4
'length error'      -: 'asdf' iwrite etx h,2 4
'length error'      -: 'asdf' iwrite etx h;2 4
'length error'      -: 'asdf' iwrite etx f,<2 4

'index error'       -: 'asdf' iwrite etx h,_11
'index error'       -: 'asdf' iwrite etx h;_11
'index error'       -: 'asdf' iwrite etx f,<_11

f =. (3=9!:12 ''){'no/such/dir/or/file';':no:such:dir:or:file'
'file name error'   -: 'asdf' iwrite etx f,<1 2

'file number error' -: 'asdf' iwrite etx 0 1 0
'file number error' -: 'asdf' iwrite etx 1 3
'file number error' -: 'asdf' iwrite etx 2 2 
'file number error' -: 'asdf' iwrite etx _2 2 
'file number error' -: 'asdf' iwrite etx _1234 2 
'file number error' -: 'asdf' iwrite etx 12345 12346
'file number error' -: 'asdf' iwrite etx 0;1 
'file number error' -: 'asdf' iwrite etx 1;1 
'file number error' -: 'asdf' iwrite etx 2;1
'file number error' -: 'asdf' iwrite etx _2; 2 
'file number error' -: 'asdf' iwrite etx _1234;2 
'file number error' -: 'asdf' iwrite etx 12345;2 3

erase h

4!:55 ;:'erase f h iread iwrite open read write x y '


