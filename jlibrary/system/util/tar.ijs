coclass'jtar'

tarhelp=: 0 : 0
tar utilities - implements simple tar subset

unix tar
 tar -x -f f.tar -C path
 tar -c -f f.tar -C path folder
 tar -t -f f.tar

J tar
 load'tar'
 tar 'x';'f.tar';path
 tar 'c';'f.tar';path;folder
 tar 't';'f.tar'

 if the tar file name ended with .tar.gz or .tgz, compression flag z is implied

for example,
 on computer
  $> tar -c -f ~/j8/temp/math.tar -C ~/j8/addons finance
  iTunes connect - move math.tar to J app folder

 on J
  tar 't';'finance.tar'
  tar 'x';'finance.tar';'~addons'

J gzip
       gzip '~temp/f.gz' - return data from f.gz
  data gzip '~/temp/f.gz' - write data to f.gz
)

NB. for windows, copy zlib1.dll from gtk binary to j.dll folder or windows/system32
libz=: IFUNIX{::'zlib1.dll';unxlib^:IFUNIX 'z'
cv=: IFWIN#'+'
null=: {.a.
chksum=: 148+i.8
folderx=: '000755 ',null,'000765 ',null,'000024 ',null
filex=: '000644 ',null,'000765 ',null,'000024 ',null

no_gzbuffer=: 0
NB. tar util

NB. =========================================================
getchk=: 3 : 0
d=. 512{.y
d=. ' ' chksum}d NB. checksum blanks
c=. +/a.i.d         NB. checksum
v=. _6{.(48+(11#8)#:c){a.
v,null,' '
)

NB. =========================================================
ftree=: 3 : 0
r=. ''
d=. 1!:0<'/*',~jpath y
m=. 'd'=>4{each 4{"1 d
t=. m#{."1 d
r=. r,(<y,'/'),each t
for_n. t do.
  r=. r,ftree y,'/',>n
end.
r
)
NB. tar main

NB. =========================================================
tar=: 3 : 0
t=. >{.y
f=. >1{y
if. ('.tar.gz'-:_7{.f)+.('.tgz'-:_4{.f) do. t=. (t-.'z'),'z' end.
z=. 'z'e.t
if. z do.
  assert. ('.tar.gz'-:_7{.f)+.('.tgz'-:_4{.f)
else.
  assert. '.tar'-:_4{.,f
end.
select. {.t-.'z'
case. 'x' do.
  assert. 3=#y['needs 3 paramters'
  assert. fexist f['file must exist'
  if. z do.
    f=. jpathsep^:IFWIN f
    fz=. (jpath '~temp'),((}.~ i:&'/') f),'.tar',":2!:6''
    ferase ::[ fz
    assert. 0=fexist fz
    (gzip f) fwrite fz
    y=. (<fz) 1}y
  end.
  r=. tarx }.y
  if. z do. ferase ::[ fz end.
  r
case. 'c' do.
  assert. 4=#y['needs 4 paramters'
  tarc }.y
  assert. fexist f['file must exist'
  if. z do. (fread f)gzip f end.
case. 't' do.
  assert. 2=#y['needs 2 paramters'
  assert. fexist f['file must exist'
  if. z do.
    f=. jpathsep^:IFWIN f
    fz=. (jpath '~temp'),((}.~ i:&'/') f),'.tar',":2!:6''
    ferase ::[ fz
    assert. 0=fexist fz
    (gzip f) fwrite fz
    y=. (<fz) 1}y
  end.
  r=. tart }.y
  if. z do. ferase ::[ fz end.
  r
case. do.
  assert. 0['unsupported feature'
end.
)

NB. =========================================================
NB. tarx (extract)
NB. tarx tar;path - write tar files to path
tarx=: 3 : 0
'file path'=. y
file=. jpathsep^:IFWIN file [ path=. jpathsep^:IFWIN path
mkdir_j_ jpath path
assert. 2=ftype path['path folder must exist'
d=. fread file
assert. _1-.@-:d['can not read file'
while. #d do.
  type=. 156{d
  if. type=null do.
    assert. 1024=+/null=1024{.d
    i.0 0 return.
  end.
  name=. (d i. null){.d
  assert. 100>#name
  f=. path,'/',name
  count=. 8#._48+a.i. ' '-.~ ({.~ i.&null) 12{.124}.d
  assert. (chksum{d) =&((0&".)@:({.~ i.&null)) (getchk d)
  select. type
  case. '5' do.
    d=. 512}.d
    mkdir_j_ jpath f
    assert. 2=ftype f
  case. '0' do.
    data=. count{.512}.d
    d=. (512*1+>.count%512)}.d
    assert. (#data)=data fwrite f
  case. do.
    assert. 'bad file'
  end.
end.
assert. 0['should never get here'
)

NB. =========================================================
NB. tart (test)
NB. tart file
tart=: 3 : 0
d=. fread ,>y
assert. _1-.@-:d['can not read file'
r=. ''
while. #d do.
  type=. 156{d
  if. type=null do.
    assert. 1024=+/null=1024{.d
    r return.
  end.
  name=. (d i. null){.d
  r=. r,name,LF
  count=. 8#._48+a.i. ' '-.~ ({.~ i.&null) 12{.124}.d
  assert. (chksum{d) =&((0&".)@:({.~ i.&null)) (getchk d)
  select. type
  case. '5' do.
    d=. 512}.d
  case. '0' do.
    d=. (512*1+>.count%512)}.d
  case. do.
    assert. 0['invalid file'
  end.
end.
assert. 0['should never get here'
)

NB. =========================================================
NB. tar create
tarc=: 3 : 0
'file path folder'=. y
file=. jpathsep^:IFWIN file [ path=. jpathsep^:IFWIN path [ folder=. jpathsep^:IFWIN folder
assert. 2=ftype path['path must exist'
folder=. ('/'={.folder)}.folder
'' fwrite file
path=. jpath path
base=. path,'/',folder
d=. {."1 dirtree base
files=. (>:#path)}.each d
folders=. (>:#path)}.each ftree path,'/',folder
for_f. folders do.
  f=. >f
  d=. 512#null
  d=. (100{.f,100#null) (i.100)}d
  d=. '5' 156}d
  d=. folderx (100+i.24)}d
  d=. (getchk d) chksum}d
  d fappend file
end.
for_f. files do.
  f=. >f
  d=. 512#null
  d=. (100{.f,100#null) (i.100)}d
  d=. '0' 156}d
  data=. fread path,'/',f
  c=. ' ',~(48+(11#8)#:#data){a.
  d=. c (124+i.12)}d
  d=. filex (100+i.24)}d
  d=. (getchk d) chksum}d
  d fappend file
  t=. 512*>.(#data)%512
  (t{.data)fappend file
end.
(1024#null) fappend file
i.0 0
)

NB. =========================================================
gzopen=: 3 : 0
'f m'=. y
f=. jpath jpathsep^:IFWIN f
f=. iospath^:IFIOS f
assert. 0~:h=. (libz,' gzopen >',cv,' x *c *c') cd f;,m
if. -.no_gzbuffer do.
  try. 0=(libz,' gzbuffer >',cv,' i x i') cd h;128*1024 catch. no_gzbuffer=: 1 end.  NB. not available in older version
end.
h
)

NB. =========================================================
gzclose=: (libz,' gzclose >',cv,' i x')&cd

NB. =========================================================
NB.      gzip '~temp/f.gz' - return data from f.gz
NB. data gzip '~/temp/f.gz' - write data to f.gz
gzip=: 3 : 0
r=. ''
d=. (128*1024)$' '
h=. gzopen y;'rb'
while. c=. (libz,' gzread >',cv,' i x *c i') cd h;d;#d do.
  assert. _1~:c
  r=. r,c{.d
end.
assert. 0=gzclose h
r
:
h=. gzopen y;'wb'
assert. (#x)=(libz,' gzwrite >',cv,' i x *c i') cd h;(,x);#x
assert. 0=gzclose h
i.0 0
)
NB. jtar zdefs

tarhelp_z_=: tarhelp_jtar_
tar_z_=: tar_jtar_
gzip_z_=: gzip_jtar_
