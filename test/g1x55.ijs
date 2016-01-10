NB. 1!:55 ---------------------------------------------------------------

write =. 1!:2
files =. 1!:20
open  =. 1!:21
lock  =. 1!:31
erase =. 1!:55

f =. <'foo1x55.x'
g =. <'goo1x55.x'
q =. files ''

(128$'a') write f
erase f

(128$'a') write f
h =. open f
erase f
q -: files ''

(128$'a') write f
h =. open f
erase h
q -: files ''

(128$'a') write f
h =. open f
erase <h
q -: files ''

(128$'a') write f
(281$'a') write g
h =. open f
erase g,<h
q -: files ''

'domain error'      -: erase etx 'a'
'domain error'      -: erase etx 'abc'
'domain error'      -: erase etx 3.45
'domain error'      -: erase etx 3j4
'domain error'      -: erase etx <3.45
'domain error'      -: erase etx <3j4
'domain error'      -: erase etx <0;1;0

'file number error' -: erase etx 0
'file number error' -: erase etx 1
'file number error' -: erase etx 2
'file number error' -: erase etx <0
'file number error' -: erase etx <1
'file number error' -: erase etx <2
'file number error' -: erase etx 0;1;0
'file number error' -: erase etx 3;4;5
'file number error' -: erase etx 12345 12346

'rank error'        -: erase etx <3 4 5
'rank error'        -: erase etx <3 4$'a'

'length error'      -: erase etx <'' 
'length error'      -: erase etx <i.0

mac =. 3 = 9!:12 ''
f   =. mac{'no/such/dir/or/file';':no:such:dir:or:file'
(<erase etx <'NoSuch.943') e. 'file name error';'interface error'
(<erase etx f            ) e. 'file name error';'interface error' 

4!:55 ;:'erase f files g h lock mac open q write '


