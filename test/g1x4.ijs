prolog './g1x4.ijs'
NB. 1!:4 ----------------------------------------------------------------

dir   =: 1!:0
size  =: 1!:4
open  =: 1!:21
close =: 1!:22

x=: 9!:12 ''
win =: x e. 2 6
mac =: x e. 3
unix=: x e. 5 7
pc  =: x e. 0 1 2 6

p =: >{:4!:3 ''
p =: < p ([ }.~ [: - |.@[ i. ]) (pc#'/'),(mac#':'),unix#'/'
d =: dir p,&.><'*.ijs'
(>2{"1 d) -: size p,&.>{."1 d

j =: (5<.#d)?#d
f =: p,&.>j{0{"1 d
s =: >j{2{"1 d
h =: open f
s = size f
s = size h
s = size ;/h
(s,s) = size f,<"0 h
close h

'domain error'      -: size etx 'a'
'domain error'      -: size etx 3.45
'domain error'      -: size etx 3j4
'domain error'      -: size etx <3.45
'domain error'      -: size etx <3j4

'rank error'        -: size etx <1 4$'asdf'
'rank error'        -: size etx <0 1 0
'rank error'        -: size etx <1 2 3 4

'length error'      -: size etx <''
'length error'      -: size etx <i.0

'file number error' -: size etx 0
'file number error' -: size etx 1 
'file number error' -: size etx 2 
'file number error' -: size etx 12311 12313
'file number error' -: size etx <0
'file number error' -: size etx <1 
'file number error' -: size etx <2 
'file number error' -: size etx <12311

f   =: mac{'no/such/dir/or/file';':no:such:dir:or:file'
'file name error'   -: size etx f
'file name error'   -: size etx <'noQsuch'

NB. Test 4!:6 and 4!:7
p =: 4!:3''
h =: 4!:6 'new string'  NB. If we rerun the test it might not be at the end
h>5
h<:#p
'new string' -: h {:: 4!:3''
0 -: 4!:6 (0) {:: p
1 -: 4!:6 (1) {:: p
1: 4!:7 (_1)
_1 -: 4!:7 (0) 
0 -: 4!:7 (<:#4!:3'') 
(<:#4!:3'') -: 4!:7 (0) 
'index error' -: 4!:7 etx _2
'index error' -: 4!:7 etx (#4!:3'')

4!:55 ;:'close d dir f h j mac open p pc s size unix win x ' 



epilog''

