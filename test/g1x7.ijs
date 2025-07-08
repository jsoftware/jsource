prolog './g1x7.ijs'
NB. 1!:7 ----------------------------------------------------------------

0 0$0 [ 'read only permissions leak to containing directory under wsl -- OS issue'

write =: 1!:2
mkdir =: 1!:5
perm  =: 1!:7
open  =: 1!:21
close =: 1!:22
erase =: 1!:55

unix  =: (9!:12 '') e. 5 7
nn    =: unix{3 9
wsl   =: 1 e. 'WSL' E. ":2!:0 ::0: 'cat /proc/version 2>/dev/null'
wslhack =: (nn$0 0,wsl) +. ]



test  =: 3 : ('((,nn)-:$y) *. *./y e."_1 nn$''rwx'',.''-''')

brandnew =. 'brandnew',(":2!:6''),'_',":3&T.''
erase ::1: ((brandnew,'/')&,)&.> {."1[ 1!:0<brandnew,'/*'
erase ::1: <brandnew
d =. <brandnew
mkdir d
test perm d
erase d

f =. <jpath ((UNAME-:'Android'){::'~temp/';'~bin/'),'foogoo5.x',(":2!:6''),'_',":3&T.''
'foo upon thee' write f
test perm f

(nn$'r--') perm f
wslhack (nn$'r--') = perm f
(nn$'rw-') perm f
wslhack (nn$'rw-') = perm f
h =. open f
wslhack (nn$'rw-') = perm h
close h

h =. open f
(perm h) -: perm f
(perm h) -: perm <h
close h

erase f


NB. 1!:7 ----------------------------------------------------------------

perm =: 1!:7

'domain error'      -: perm etx 'a'
'domain error'      -: perm etx 'abc'
'domain error'      -: perm etx 3.45
'domain error'      -: perm etx 3j4
'domain error'      -: perm etx <3.45
'domain error'      -: perm etx <3j4
'domain error'      -: perm etx <<'abc'

'rank error'        -: perm etx <0 1 0
'rank error'        -: perm etx <,4
'rank error'        -: perm etx <1 3$'abc'

'length error'      -: perm etx <''

'file name error'   -: perm etx <'1234skidoo'
'file name error'   -: perm etx <'does/not/exist'

'file number error' -: perm etx 0
'file number error' -: perm etx 1
'file number error' -: perm etx 2
'file number error' -: perm etx 12345 12346

'domain error'      -: 0 1 0 perm etx <jpath '~temp/foo.x'
'domain error'      -: 3 1   perm etx <jpath '~temp/foo.x'
'domain error'      -: 3.5   perm etx <jpath '~temp/foo.x'
'domain error'      -: 3j4   perm etx <jpath '~temp/foo.x'
'domain error'      -: (<'r')perm etx <jpath '~temp/foo.x'
'domain error'      -: (nn$'abc') perm etx <jpath '~temp/foo.x'
'domain error'      -: (nn$'a--') perm etx <jpath '~temp/foo.x'
'domain error'      -: (nn$'-a-') perm etx <jpath '~temp/foo.x'
'domain error'      -: (nn$'--a') perm etx <jpath '~temp/foo.x'

'length error'      -: 'wx'  perm etx <jpath '~temp/foo.x'




epilog''

