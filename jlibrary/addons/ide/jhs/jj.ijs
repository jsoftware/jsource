NB. jj (j to jhs) client code

require'socket'
require'tar'

jjset=:    jjset_jj_
jjdo=:     jjdo_jj_
jjget=:    jjget_jj_
jjput=:    jjput_jj_

coclass'jj'

URL=: 'jjserver'
TAR=: '~temp/jtoj.tar'

posttemplate=: _2}.toCRLF 0 : 0
POST /<URL> HTTP/1.1 
Connection: Keep-Alive
Content-Length: <COUNT>

<DATA>
)

jjset=: 3 : 0
d=. <;._1 ' ',deb y
'argument not: ip:port pass'assert 2=#d
'SERVER PASS'=: d
i.0 0
)

jjdo=: 4 : 0
'jjset required to set ip:port pswd'assert 0=nc<'SERVER'
data=. PASS,({.a.),x,({.a.),3!:1 y
i=. SERVER i. ':'
port=. 0".}.i}.SERVER
server=. i{.SERVER
ip=. >2{sdgethostbyname_jsocket_ server

try.
 sk=. >0{sdcheck_jsocket_ sdsocket_jsocket_''
 sdcheck_jsocket_ sdconnect_jsocket_ sk;AF_INET_jsocket_;ip;port
 t=. posttemplate rplc '<URL>';URL;'<DATA>';data;'<COUNT>';":#data
 while. #t do. t=.(>sdcheck_jsocket_ t sdsend_jsocket_ sk,0)}.t end.
 h=. d=. ''
 cl=. 0
 while. (0=#h)+.cl>#d do. NB. read until we have header and all data
  z=. sdselect_jsocket_ sk;'';'';5000
  assert sk e.>1{z NB. timeout
  'c r'=. sdrecv_jsocket_ sk,10000,0
  assert 0=c

  if. (0=#r)*.cl=_ do. break. end. NB. no length and no more data

  d=. d,r
  if. 0=#h do. NB. get headers
   i=. (d E.~ CRLF,CRLF)i.1 NB. headers CRLF delimited with CRLF at end
   if. i<#d do. NB. have headers
    i=. 4+i
    h=. i{.d NB. headers
    d=. i}.d
    i=. ('Content-Length:'E. h)i.1

    if. i<#h do.
     t=. (15+i)}.h
     t=. (t i.CR){.t
     cl=. _1".t
     assert _1~:cl
    else.
     cl=. _
    end.
   end.
  end.
 end.
catch.
 shutdownJ_jsocket_ sk ; 2
 sdclose_jsocket_ ::0: sk
 (13!:12'') assert 0
end.
shutdownJ_jsocket_ sk ; 2
sdclose_jsocket_ ::0: sk

if. '|'={.d do. d assert 0 end. NB. error - plain text starting with |
3!:2 d
)

jjget=: 3 : 0
y jjget~ (y i:'/'){.y
:
target=. x [ source=. y
st=. 'ftype y'jjdo source
'source is not a file or folder'assert 0~:st
mkdir_j_ target
'target is not a folder'assert 2=ftype target
t=. target,(source i:'/')}.source
if. 1=st do.
 d=. 'fread y' jjdo source
 'write to target failed'assert (#d)=d fwrite t
else.
 'tar y'jjdo 'c';TAR;source;''
 d=. 'fread y' jjdo TAR
 d fwrite TAR
 tar 'x';TAR;t
end.
'OK'
)

jjput=: 3 : 0
y jjput~ (y i:'/'){.y
:
target=. x [ source=. y
st=. ftype source
'source is not a file or folder'assert 0~:st
'mkdir_j_ y' jjdo target
'target is not a folder'assert 2='ftype y'jjdo target
t=. target,(source i:'/')}.source
if. 1=st do.
 d=. fread source
 ('y fwrite ''',t,'''') jjdo d
else.
 tar 'c';TAR;source;''
 '(>{:y) fwrite {.y' jjdo TAR;fread TAR
 'tar y' jjdo 'x';TAR;t
end. 
'OK'
)

NB. simple test for jjput/jjget - run on same machine
test=: 3 : 0
mkdir_j_'~temp/jj'
ferase {."1 fdir'~temp/jjx*'
ferase {."1 fdir'~temp/jjy/*'
ferase {."1 fdir'~temp/jjz/*'
'abc'fwrite'~temp/jjx/a'
'~temp/jj/b'jjput'~temp/jjx/a'
'~temp/jj/c'jjget'~temp/jjx/b'
assert fexist (<'~temp/jjx/'),each ;:'a b c'

'~temp/jjy'jjput'~temp/jjx'
assert fexist (<'~temp/jjy/'),each ;:'a b c'
'~temp/jjz'jjget'~temp/jjy'
assert fexist (<'~temp/jjz/'),each ;:'a b c'
)

echo 0 : 0
         jjset 'ip:port pswd'
[target] jjget source
[target] jjput source
'...'    jjdo  y

source is .../FILE or .../FOLDER
to put in target folder

elided target is ... from source
)
