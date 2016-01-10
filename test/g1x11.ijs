NB. 1!:11 ---------------------------------------------------------------

write =. 1!:2
iread =. 1!:11
open  =. 1!:21
close =. 1!:22
erase =. 1!:55

intv  =. [ {~ ([: ({. + i.@{:) ]) 

f =. <'foo1x11.x'
t =. a.{~?1000$#a.
t write f
(t intv i) -: iread f,<i=.?100 900
h =. open f
(t intv i) -: iread f,<i=.?100 900
(t intv i) -: iread h, i=.?200 800
(t intv i) -: iread h; i=.?300 700
(t intv i) -: iread h, i=.(?1000),0

(i}.t) -: iread h, i=.?1000
(i}.t) -: iread h; i=.?1000
(i}.t) -: iread f,<i=.?1000

(i{.t) -: iread h, i=.->:?1000
(i{.t) -: iread h; i=.->:?1000
(i{.t) -: iread f,<i=.->:?1000

erase h


f =. <'foo1x11.x'
'0123456789' write f
h =. open f

'domain error'      -: iread etx 'abc'
'domain error'      -: iread etx 3.4 5  
'domain error'      -: iread etx 3j4 2 3
'domain error'      -: iread etx f,<'ab'
'domain error'      -: iread etx f,<3.4 5
'domain error'      -: iread etx f,<3j4	5
'domain error'      -: iread etx f,<3;4
'domain error'      -: iread etx h;'ab'
'domain error'      -: iread etx h;3.4 5
'domain error'      -: iread etx h;3j4 5
'domain error'      -: iread etx h;<3;4

'rank error'        -: iread etx 0 1 0;2 3
'rank error'        -: iread etx 3 4;2 3
'rank error'        -: iread etx (1 3$'abc');2 3
'rank error'        -: iread etx f,<i.1 2

'length error'      -: iread etx 'a'
'length error'      -: iread etx '';2 3
'length error'      -: iread etx (i.0);2 3
'length error'      -: iread etx h;i.0
'length error'      -: iread etx ,f
'length error'      -: iread etx ,h
'length error'      -: iread etx f,<2 3 4

'index error'       -: iread etx h,10
'index error'       -: iread etx h,9 2
'index error'       -: iread etx h,_11
'index error'       -: iread etx h,_1 2
'index error'       -: iread etx h;10
'index error'       -: iread etx h;9 2
'index error'       -: iread etx h;_11
'index error'       -: iread etx h;_1 2
'index error'       -: iread etx f,<10
'index error'       -: iread etx f,<9 2
'index error'       -: iread etx f,<_11
'index error'       -: iread etx f,<_1 2

f =. (3=9!:12 ''){'no/such/dir/or/file';':no:such:dir:or:file'
'file name error'   -: iread etx f,<1 2
'file name error'   -: iread etx 'noQsuch';0

'file number error' -: iread etx 0 1 0
'file number error' -: iread etx 1 3
'file number error' -: iread etx 2 2 
'file number error' -: iread etx _2 2 
'file number error' -: iread etx _1234 2 
'file number error' -: iread etx 12345 12346
'file number error' -: iread etx 0;1 
'file number error' -: iread etx 1;1 
'file number error' -: iread etx 2;1
'file number error' -: iread etx _2; 2 
'file number error' -: iread etx _1234;2 
'file number error' -: iread etx 12345;2 3

erase h

4!:55 ;:'close erase f h i intv iread open t write'


