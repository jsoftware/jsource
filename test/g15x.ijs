prolog './g15x.ijs'
NB. 15!: ----------------------------------------------------------------

NB. runs as noop as winapi.ijs is no longer supported - might be resurrected some day
pc=: 0 NB. (9!:12 '') e. 2 6  NB. Windows only

NB. a small memory leak is expected on the next line
2 = {:15!:1 ((15!:8) 10),0 5 4  NB. reference count

require'socket'
require 'dll'         NB. DLL utils


3 : 0''
if. pc do.
 require 'winapi'      NB. API utils
else.
 winset=:1:
end.
1
)

winset 'CREATE_NEW FILE_BEGIN FILE_CURRENT FILE_END GENERIC_READ GENERIC_WRITE'
winset 'OPEN_EXISTING'

cderx     =: 15!:11

Fclose    =: fclose    =: >@{.@('kernel32 CloseHandle i i'             &(15!:0))
Fdelete   =: fdelete   =: >@{.@('kernel32 DeleteFileA i *c'            &(15!:0))
Fdeletedir=: fdeletedir=: >@{.@('kernel32 RemoveDirectoryA i *c'       &(15!:0))
Fcreate   =:              >@{.@('kernel32 CreateFileA i *c i i * i i i'&(15!:0))
Fcreatedir=:              >@{.@('kernel32 CreateDirectoryA i *c *'     &(15!:0))
Fsize     =:              >@{.@('kernel32 GetFileSize i i *i'          &(15!:0))
Fwrite    =:              >@{.@('kernel32 WriteFile i i * i *i *'      &(15!:0))
Fsetptr   =:              >@{.@('kernel32 SetFilePointer i i i *i i'   &(15!:0))
Fcopyto   =:              >@{.@('kernel32 CopyFileA i *c *c i'         &(15!:0))
Fmoveto   =:              >@{.@('kernel32 MoveFileA i *c *c'           &(15!:0))
Fread     =:                    'kernel32 ReadFile i i * i *i *'       &(15!:0)

fcopyto   =: 4 : 'Fcopyto x,y,<0'
fmoveto   =: 4 : 'Fmoveto x,y'
fcreatedir=: 3 : 'Fcreatedir y,<<0'
fsize     =: 3 : 'Fsize y;<<0'
fsetptr   =: 4 : '>{.Fsetptr x;y;(<0);FILE_BEGIN'

fcreate=: 3 : 0   NB. fcreate name
 >{.Fcreate y,(GENERIC_READ+GENERIC_WRITE);0;(<0);CREATE_NEW   ;0;0
)
 
fopen  =: 3 : 0   NB. fopen name
 >{.Fcreate y,(GENERIC_READ+GENERIC_WRITE);0;(<0);OPEN_EXISTING;0;0
)
 
fwrite =: 4 : 0   NB. string fwrite handle
 Fwrite y;x;(#x);(,0);<<0
)

fread  =: 3 : 0   NB. fread handle
 y fsetptr 0
 n=. fsize y
 >2{Fread y;(n#' ');n;(,0);<<0
)

test=: 3 : 0   NB. windows only
if. pc do.
assert. 1 -: fcreatedir <jpath '~temp/testtemp'
assert. 0 -: fdelete <jpath '~temp/testtemp/non_existent_file'
assert. 2 -: >{.cderx ''

assert. _1 ~: h=: fcreate <jpath '~temp/testtemp/test.jnk'

s=: 'boustrophedonic paracletic kerygmatic'
assert. 1 -: s fwrite h

assert. (#s) -: fsize h
assert. s -: fread h

i=: ?#s
t=: 'professors in New England guard the glory that was Greece'
assert. i -: h fsetptr i

assert. 1 -: t fwrite h
assert. (i+#t) -: fsize h
assert. ((i{.s),t) -: fread h
assert. 1 -: fclose h

assert. 1 -: fcreatedir <jpath '~temp/testtemp/tempdir'

assert. (<jpath '~temp/testtemp/test.jnk') fcopyto <jpath '~temp/testtemp/test1.jnk'
assert. _1 ~: h=: fopen <jpath '~temp/testtemp/test1.jnk'
assert. ((i{.s),t) -: fread h
assert. 1 -: fclose h

assert. (<jpath '~temp/testtemp/test1.jnk') fmoveto <jpath '~temp/testtemp/tempdir/test2.jnk'
assert. _1 ~: h=: fopen <jpath '~temp/testtemp/tempdir/test2.jnk'
assert. ((i{.s),t) -: fread h
assert. 1 -: fclose h

assert. 1 -: fdelete <jpath '~temp/testtemp/test.jnk'

assert. 0 -: fdeletedir <jpath '~temp/testtemp/tempdir'
assert. (>{.cderx '') e. 5 145
assert. 1 -: fdelete <jpath '~temp/testtemp/tempdir/test2.jnk'
assert. 1 -: fdeletedir <jpath '~temp/testtemp/tempdir'
assert. 1 -: fdeletedir <jpath '~temp/testtemp'
end.
1
)

test ''

0 -: sdclose_jsocket_ >1{sdsocket_jsocket_''  NB. all systems

t=: 100 ?@$ 1e6
t -:      15!:1 (15!:14 <'t'),0,(*/$t),3!:0 t
(15!:14 <'t') = (15!:12 <'t') + 8*SZI
t=: 100 4 1?@$ 0
t -: ($t)$15!:1 (15!:14 <'t'),0,(*/$t),3!:0 t
(15!:14 <'t') = (15!:12 <'t') +10*SZI

'domain error' -: 15!:6  etx <'test'
'domain error' -: 15!:6  etx ;:'t test'
'domain error' -: 15!:6  etx <u:'test'
'domain error' -: 15!:6  etx u:&.> ;:'t test'
'domain error' -: 15!:6  etx <10&u:'test'
'domain error' -: 15!:6  etx 10&u:&.> ;:'t test'
'domain error' -: 15!:6  etx s:@<"0&.> <'test'
'domain error' -: 15!:6  etx <"0@s: <'test'
NB. ??? sometimes ill-formed name
NB. 'value error' -: 15!:6  etx s:@<"0&.> ;:'t test'
NB. 'value error' -: 15!:6  etx <"0@s: ;:'t test'
'domain error' -: 15!:14 etx <'test'
'domain error' -: 15!:14 etx ;:'t test'
'domain error' -: 15!:14 etx <u:'test'
'domain error' -: 15!:14 etx u:&.> ;:'t test'
'domain error' -: 15!:14 etx <10&u:'test'
'domain error' -: 15!:14 etx 10&u:&.> ;:'t test'
'domain error' -: 15!:14 etx s:@<"0&.> <'test'
'domain error' -: 15!:14 etx <"0@s: <'test'
NB. ??? sometimes ill-formed name
NB. 'value error' -: 15!:14 etx s:@<"0&.> ;:'t test'
NB. 'value error' -: 15!:14 etx <"0@s: ;:'t test'

'value error'  -: 15!:6  etx <'undefinedname'
'value error'  -: 15!:6  etx ;:'t undefinedname'
'value error'  -: 15!:14 etx <'undefinedname'
'value error'  -: 15!:14 etx ;:'t undefinedname'

NB. 0 ~: jt=: 15!:19 ''
NB. 0 = 15!:20 'nosuchlibrary'
NB. 0 ~: h=: 15!:20 >IFUNIX{ 'wsock32' ; unxlib 'c'
NB. 0 ~: h 15!:21 'gethostname'

4!:55 ;:'CREATE_NEW FILE_BEGIN FILE_CURRENT FILE_END GENERIC_READ '
4!:55 ;:'GENERIC_WRITE OPEN_EXISTING '
4!:55 ;:'Fclose Fcopyto Fcreate Fcreatedir Fdelete Fdeletedir Fmoveto Fread '
4!:55 ;:'Fsetptr Fsize Fwrite '
4!:55 ;:'cderx fclose fcopyto fcreate fcreatedir fdelete fdeletedir fmoveto '
4!:55 ;:'fopen fread fsetptr fsize fwrite '
4!:55 ;:'h i jt pc s sbp t test winset'



epilog''

