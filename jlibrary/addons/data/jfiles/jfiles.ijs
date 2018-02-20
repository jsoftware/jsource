cocurrent 'jfiles'
V64L=: 1718235186
V32L=: 1718235187
V32B=: 1718235188
V64B=: 1718235189

M32=: 4294967296

B4=: 4 # 256
B8=: 8 # 256
j_blk=: [: <. >.&.(2&^.)@#
j_box=: <^:(L. = 0:)
j_ischar=: 3!:0 e. 2 131072 262144"_
j_nos=: [: ; [: {."1 (1!:20)
j_read=: 3!:2 @ (1!:11)
j_rep=: 3!:1
j_take0=: {.!.({.a.)
j_writerep=: j_rep&.>@[ j_write ]
j_vcur=: 3 : '(2 * IF64) }. V32L,V32B,V64B'
j_compress=: 3 : 0
f=. /:~ y
bgn=. {."1 f
end=. bgn + {:"1 f
min=. 0, }: end
bgn=. bgn >. min
b=. bgn ~: min
if. 0 e. b do.
  f=. (b#bgn) ,. b +/;.1 (end-bgn)
end.
f /: {:"1 f
)
j_dir=: 3 : 0
hdr=. 1!:11 y;0 512
try.
  <. each 7 {. 3!:2 :: 0: hdr
catch.
  <. each 7 {. (3!:2) 256 {. hdr
end.
)
j_ext=: 3 : 0
y=. jpathsep 7 u: y
y=. y,'.ijf' #~ (0<#y) > '.' e. (-.+./\.y='/')#y
8 u: y
)
j_getver=: 3 : 0
h=. y
'v s c l d f q'=. j_dir h
if. v >: V64L do. v return. end.
if. c = 0 do. V32L return. end.
free=. j_read h;f
dir=. j_readdir V32B,h,c,d
if. 1 e. l < {."1 dir do. V32L return. end.
r=. /:~ f,d,dir,free
p=. {.{.r
y=. p,+/"1 r
(y -: ({."1 r),l) { V32L,V32B
)
j_name=: 3 : 0
y=. >y
if. j_ischar y do.
  j_ext y
end.
)
j_open=: 3 : 0
y=. >y
if. 0 e. #y do. _1 return. end.
if. j_ischar y do.
  y=. j_ext y
  j=. 1!:11 :: _1: y;0 0
  if. _1 -: j do. return. end.
  n=. ;{."1 [ 1!:20''
  h=. 1!:21 <y
  h,-.h e. n
else.
  y,0
end.
)
j_readcpt=: 4 : 0
'v h d'=. x
select. v
case. V64B do.
  <. B8 #. _8 [\"1 a. i. 1!:11 h,.(d+y*16),.16
case. V32L do.
  <. B4 #. _4 |.\"1 a. i. 1!:11 h,.(d+y*8),.8
case. V32B do.
  <. B4 #. _4 [\"1 a. i. 1!:11 h,.(d+y*8),.8
case. V64L do.
  <. B8 #. _8 |.\"1 a. i. 1!:11 h,.(d+y*16),.16
end.
)
j_readdir=: 3 : 0
'v h c d e'=. y
select. v
case. V64B do.
  _2[\ <. B8 #. _8 [\ a. i. 1!:11 h,d,c*16
case. V32L do.
  _2[\ <. B4 #. _4 |.\ a. i. 1!:11 h,d,c*8
case. V32B do.
  _2[\ <. B4 #. _4 [\ a. i. 1!:11 h,d,c*8
case. V64L do.
  _2[\ <. B8 #. _8 |.\ a. i. 1!:11 h,d,c*16
end.
)
j_upver=: 3 : 0
h=. y
'v s c l d f q'=. j_dir h
if. v e. j_vcur '' do. return. end.
v=. j_getver h
dir=. j_readdir v,h,c,d
if. IF64 +. v = V64L do. v=. V64B end.
dir=. , v j_tochar dir
dir=. (512 >. #dir) j_take0 dir
d=. 512 >. d
free=. j_read h;f
free=. j_compress d,f,free
'l free new'=. (<dir) j_write l;free;h
'l f'=. j_writefree l;free;h
d=. ,new
q=. >:{.q,0
res=. v;s;c;l;d;f;q
(512 j_take0 j_rep res) 1!:12 h;0
res
)
j_tochar=: 4 : 0
select. x
case. V64B do.
  ,"2 a. {~ B8 #: y
case. V32L do.
  ,"2 |."1 a. {~ B4 #: y
case. V32B do.
  ,"2 a. {~ B4 #: y
case. V64L do.
  ,"2 |."1 a. {~ B8 #: y
end.
)
j_write=: 4 : 0
'l f h'=. y
x=. ,x
d=. i.0 2
while. #x do.
  blk=. j_blk dat=. >{.x
  x=. }.x
  if.
    (#f) > ndx=. 1 i.~ blk <: {:"1 f
  do.
    'p s'=. ndx{f
    d=. d,p,blk
    if. blk < s do.
      f=. ((p+blk),s-blk) ndx } f
    else.
      f=. (ndx{.f),(>:ndx)}.f
    end.
    dat 1!:12 h;p
  else.
    d=. d,l,blk
    l=. l+blk
    (blk{.dat) 1!:3 h
  end.
end.
l;f;d
)
j_writefree=: 3 : 0
'l f h'=. y
blk=. 256 >. j_blk dat=. j_rep f
if.
  (#f) > ndx=. 1 i.~ blk <: {:"1 f
do.
  'p s'=. ndx{f
  d=. p,blk
  if. blk < s do.
    f=. ((p+blk),s-blk) ndx } f
  else.
    f=. (ndx{.f),(>:ndx)}.f
  end.
  (j_rep f) 1!:12 h;p
else.
  d=. l,blk
  l=. l+blk
  (blk{.dat) 1!:3 h
end.
l;d
)
jdup=: 3 : 0
'' jdup y
:

fl=. j_name x
'y sel'=. 2{.boxopen y
y=. j_name y

max=. 100000

if. own=. 0=#fl do.
  fl=. ((+./\.y='/')#y),'eraseme.pls'
end.

'h o'=. j_open y
if. h=_1 do. return. end.

if. #sel do.
  c=. #sel
  c jcreate fl
  hx=. jopen fl
  whilst. #sel=. }.sel do.
    (jread h;{.sel) jappend hx
  end.

else.
  'v s c l d f q'=. j_dir h
  if. v < V64L do.
    v=. j_getver h
  end.
  w=. (v e. V64B,V64L) { 8 16
  c jcreate fl
  hx=. jopen fl
  pos=. 0
  blk=. 1000
  read=. j_read@(h&;)
  dir=. j_readdir v,h,c,d
  while. #dir do.
    sel=. (blk <. #dir) {. dir
    dir=. blk }. dir
    while. #sel do.
      siz=. {:"1 sel
      len=. 1>.(max<+/\siz)i.1
      (read &.> <"1 len{.sel) jappend hx
      sel=. len}.sel
    end.
  end.

end.

jclose hx
if. o do. jclose h end.

if. own do.
  pos=. 0
  siz=. 1!:4 <fl
  '' 1!:2 <y
  while. siz>pos do.
    len=. max<.siz-pos
    (1!:11 fl;pos,len) 1!:3 <y
    pos=. pos+max
  end.
  1!:55 <fl
end.

j_validate fl
c
)
jappend=: 4 : 0
'h o'=. j_open y
if. h=_1 do. return. end.
'v s c l d f q'=. j_dir h
if. -. v e. j_vcur '' do.
  'v s c l d f q'=. j_upver h
end.
w=. (v = V64B) { 8 16
free=. j_read h;f
dat=. j_box x
if. 1<#$dat do.
  if. o do. jclose h end.
  _3 return.
end.
res=. c+ i.#dat
'l free new'=. dat j_writerep l;free;h
dat=. , v j_tochar new
'b e'=. d
if. e>w*{:res do.
  dat 1!:12 h;b+w*c
  (j_rep free) 1!:12 h;{.f
else.
  dat=. (1!:11 h;b,w*c),dat
  free=. j_compress d,f,free
  'l free new'=. (<dat) j_write l;free;h
  'l f'=. j_writefree l;free;h
  d=. ,new
end.
c=. >:{:res
q=. >:{.q,0
(j_rep v;s;c;l;d;f;q) 1!:12 h;0
if. o do. jclose h end.
j_validate y
res
)
jclose=: 1!:22 :: _4:
jcreate=: 3 : 0
0 jcreate y
:
q=. s=. c=. 0
d=. 1024,<. >.&.(2&^.) 512>.8*x
f=. (+/d),256
l=. +/f
v=. IF64 { V32L,V64B
hdr=. ({.f){.j_rep v;s;c;l;d;f;q;s
hdr=. l{.hdr,j_rep i.0 2
y=. <j_name y
1!:55 :: 0: y
f=. (1: [ 1!:2) :: 0:
hdr f y
)
jerase=: 1!:55 @ < @: j_name
jopen=: 3 : 0
y=. >y
if. 0 e. #y do. _1 return. end.
if. j_ischar y do.
  y=. <j_ext y
  1!:21 :: _1: y
end.
)
jread=: 3 : 0
'y x'=. y
'h o'=. j_open y
if. h=_1 do. return. end.
'v s c l d f q'=. j_dir h
if. v < V64L do.
  v=. j_getver h
end.
w=. (v e. V64B,V64L) { 8 16
cpt=. <.,x
if. (0=#cpt) +. 1 e. (cpt<0) +. cpt>:c do.
  if. o do. jclose h end.
  _2 return.
end.
dir=. (v,h,{.d) j_readcpt cpt
dat=. j_read &.> <"1 h,.dir
if. o do. jclose h end.
($x)$dat
)
jreplace=: 4 : 0
't z'=. y
'h o'=. j_open t
if. h=_1 do. return. end.
'v s c l d f q'=. j_dir h
if. -. v e. j_vcur '' do.
  'v s c l d f q'=. j_upver h
end.
w=. (v = V64B) { 8 16
free=. j_read h;f
cpt=. <.,z
if. 0=#cpt do. return. end.
if. 1 e. (cpt<0) +. cpt>:c
do.
  if. o do. jclose h end.
  _2 return.
end.
dat=. j_box x
if. 1<#$dat do.
  if. o do. jclose h end.
  _3 return.
end.
dat=. (#cpt)$dat
dir=. (v,h,{.d) j_readcpt cpt
free=. j_compress dir,f,free
'l free new'=. dat j_writerep l;free;h
new=. <"1 v j_tochar new
new (1!:12 h&;) &.> ({.d)+cpt*w
'l f'=. j_writefree l;free;h
q=. >:{.q,0
(j_rep v;s;c;l;d;f;q) 1!:12 h;0
if. o do. jclose h end.
j_validate >{.y
cpt
)
jseqno=: 3 : 0
'h o'=. j_open y
if. h=_1 do. return. end.
q=. >6{j_dir h
if. o do. jclose h end.
q
)
jsize=: 3 : 0
'h o'=. j_open y
if. h=_1 do. return. end.
'v s c l d f q'=. j_dir h
free=. j_read h;f
if. o do. jclose h end.
s,c,l,{:+/free
)
j_valcheck=: 3 : 0
'h o'=. j_open y
if. h=_1 do. 1 return. end.
'v s c l d f q'=. j_dir h
free=. j_read h;f
dir=. j_readdir v,h,c,d
r=. /:~ f,d,dir,free
p=. {.{.r
s=. p e. 256 512 1024
y=. p,+/"1 r
if. o do. jclose h end.
s *. y -: ({."1 r),l
)
j_validate=: 3 : 0
13!:8 (-.j_valcheck y) # 12
)

j_validate=: ]
keycreate=: 3 : 0
r=. jcreate y
if. r <: 0 do. return. end.
((1 7#'keyed file 2';'unused'),'';<'';i.0) jappend y
1
)
keydir=: 3 : '>{.>jread y;9'
keydrop=: 3 : 0
'f k'=. y
'h o'=. j_open f
if. h = _1 do. return. end.
'd c'=. >jread h;9
k=. ,each boxopen k
if. 0 e. k e. d do.
  if. o do. jclose h end.
  _4 return.
end.
b=. d e. k
'' jreplace h;b#c
b=. -. b
d=. b#d
c=. b#c
if. #e=. >jread h;8 do.
  if. 0 < L. e do. (<b#e) jreplace h;8 end.
end.
(<d;<c) jreplace h;9
if. o do. jclose h end.
1
)
keyerase=: jerase
keyread=: 3 : 0
'f k'=. key_right y
'h o'=. j_open f
if. h = _1 do. return. end.
'd c'=. >jread h;9
if. 0=#k do.
  r=. jread h;c
else.
  k=. ,each boxopen k
  if. 0 e. k e. d do.
    if. o do. jclose h end.
    _4 return.
  end.
  x=. d i. k
  r=. jread h; x { c
end.
if. o do. jclose h end.
r
)
keyreadx=: 3 : 0
'f k'=. key_right y
'h o'=. j_open f
if. h = _1 do. return. end.
r=. >jread h;8
'd c'=. >jread h;9
if. 0 = L.r do. r=. (#c) # a: end.
if. #k do.
  k=. ,each boxopen k
  if. 0 e. k e. d do.
    if. o do. jclose h end.
    _4 return.
  end.
  x=. d i. k
  r=. x { r
end.
if. o do. jclose h end.
r
)
keywrite=: 4 : 0
dat=. boxopen x
'f k'=. key_right y
'h o'=. j_open f
if. h = _1 do. return. end.
'd c'=. >jread h;9
if. 0=#k do.
  dat jreplace h;c
else.
  k=. ,each boxopen k
  x=. d i. k
  m=. x < #d
  if. 0 e. m do.
    new=. +/ m=0
    c=. c, key_new new;h;c
    d=. d, (m=0) # k
    x=. d i. k
    if. 0 < L. e=. >jread h;8 do.
      e=. e, new # a:
      (<e) jreplace h;8
    end.
    (<d;<c) jreplace h;9
  end.
  dat jreplace h; x { c
end.
if. o do. jclose h end.
empty''
)
keywritex=: 4 : 0
dat=. boxopen x
'f k'=. key_right y
'h o'=. j_open f
if. h = _1 do. return. end.
'd c'=. >jread h;9
if. 0=#k do.
  if. -. (#dat) e. 1,#c do.
    if. o do. jclose h end.
    _3 return.
  end.
  (<dat) jreplace h;8
else.
  e=. >jread h;8
  if. 0 = L. e do. e=. (#c) # a: end.
  k=. ,each boxopen k
  x=. d i. k
  m=. x < #d
  if. 0 e. m do.
    new=. +/ m=0
    c=. c, key_new new;h;c
    d=. d, (m=0) # k
    e=. e, new # a:
    x=. d i. k
    (<d;<c) jreplace h;9
  end.
  (<dat x} e) jreplace h;8
end.
if. o do. jclose h end.
empty''
)
key_new=: 3 : 0
'n h c'=. y
f=. (10 }. i. 1{ jsize h) -. c
f=. (n <. #f) {. f
if. n > #f do.
  f=. f, ((n - #f)#a:) jappend h
end.
)
key_right=: 3 : 0
if. 1=#y=. boxopen y do. y , a: else. y end.
)
jappend_z_=: jappend_jfiles_
jcreate_z_=: jcreate_jfiles_
jdup_z_=: jdup_jfiles_
jerase_z_=: jerase_jfiles_
jread_z_=: jread_jfiles_
jreplace_z_=: jreplace_jfiles_
jsize_z_=: jsize_jfiles_

keycreate_z_=: keycreate_jfiles_
keydir_z_=: keydir_jfiles_
keydrop_z_=: keydrop_jfiles_
keyerase_z_=: keyerase_jfiles_
keyread_z_=: keyread_jfiles_
keyreadx_z_=: keyreadx_jfiles_
keywrite_z_=: keywrite_jfiles_
keywritex_z_=: keywritex_jfiles_
