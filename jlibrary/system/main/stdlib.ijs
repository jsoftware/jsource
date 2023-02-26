18!:4 <'z'
3 : 0 ''

JLIB=: '9.4.16'

notdef=. 0: ~: 4!:0 @ <
hostpathsep=: ('/\'{~6=9!:12'')&(I. @ (e.&'/\')@] })
jpathsep=: '/'&(('\' I.@:= ])})
winpathsep=: '\'&(('/' I.@:= ])})
PATHJSEP_j_=: '/'
IFDEF=: 3 : '0=4!:0<''DEF'',y,''_z_'''
IF64=: 16={:$3!:3[2
IFBE=: 'a'~:{.2 (3!:4) a.i.'a'
'IFUNIX IFWIN IFWINCE'=: 5 6 7 = 9!:12''
IFJHS=: 0
IFWINE=: (0 ~: 'ntdll wine_get_version >+ x'&(15!:0)) ::(0:@(15!:10))`0:@.IFUNIX ''
if. notdef 'IFIOS' do.
  IFIOS=: 0
end.
if. notdef 'IFJA' do.
  IFJA=: 0
end.
if. notdef 'IFJNET' do.
  IFJNET=: 0
end.
if. notdef 'BINPATH' do.
  BINPATH=: '/j/bin'
end.
if. notdef 'UNAME' do.
  if. IFUNIX do.
    if. -.IFIOS do.
      UNAME=: (2!:0 'uname')-.10{a.
    else.
      UNAME=: 'Darwin'
    end.
  elseif. do.
    UNAME=: 'Win'
  end.
end.
if. notdef 'LIBFILE' do.
  LIBFILE=: BINPATH,'/',IFUNIX{::'j.dll';(UNAME-:'Darwin'){::'libj.so';'libj.dylib'
end.
if. notdef 'FHS' do.
  FHS=: IFUNIX>'/'e.LIBFILE
end.
if. notdef 'RUNJSCRIPT' do.
  RUNJSCRIPT=: 0
end.
if. notdef 'IFRASPI' do.
  if. ((<UNAME)e.'Linux';'OpenBSD';'FreeBSD') do.
    IFRASPI=: (<9!:56'cpu') e. 'arm';'arm64'
  else.
    IFRASPI=: 0
  end.
end.
if. IF64 +. IFIOS do.
  IFWOW64=: 0
else.
  if. IFUNIX do.
    IFWOW64=: '64'-:_2{.(2!:0 (UNAME-:'Android'){::'uname -m';'getprop ro.product.cpu.abi')-.10{a.
  else.
    IFWOW64=: 'AMD64'-:2!:5'PROCESSOR_ARCHITEW6432'
  end.
end.
if. notdef 'IFQT' do.
  IFQT=: 0
  libjqt=: IFUNIX{::'jqt.dll';'libjqt',(UNAME-:'Darwin'){::'.so';'.dylib'
  if. 0 ~: 1!:4 :: 0: < ((BINPATH,'/')&,) libjqt do.
    libjqt=: ((BINPATH,'/')&,) libjqt
  end.
end.
if. UNAME-:'Android' do.
  AndroidLibPath=: '/lib',~ ({.~i:&'/')^:2 BINPATH
end.

assert. IFQT *: IFJA
)
jcwdpath=: jpathsep@(1!:43@(0&$),])@((*@# # '/'"_),])
jsystemdefs=: 3 : 0
xuname=. UNAME
if. 0=4!:0 <f=. y,'_',(tolower xuname),(IF64#'_64'),'_j_' do.
  0!:100 toHOST f~
else.
  0!:0 <jpath '~system/defs/',y,'_',(tolower xuname),(IF64#'_64'),'.ijs'
end.
)
18!:4 <'j'

setjdefs=: 4 : 'if. _1=4!:0 y do. (>y)=: x end.'

1 setjdefs"_ 0 <;._2 [ 0 : 0
RGBSEQ
)

(0 2$<'') setjdefs"_ 0 <;._2 [ 0 : 0
SystemFolders
UserFolders
)

3 : 0''
if. (<'bin') -.@e. {."1 SystemFolders do.
  SystemFolders=: SystemFolders, 'bin';BINPATH
end.

if. (<'home') -.@e. {."1 SystemFolders do.
  if. 'Win'-:UNAME do. t=. 2!:5'USERPROFILE'
  elseif. 'Android'-:UNAME do. t=. '/sdcard'
  elseif. ((<UNAME)e.'Linux';'OpenBSD';'FreeBSD';'Darwin') do. t=. (0-:t){::'';~t=. 2!:5'HOME'
  elseif. do. t=. ''
  end.
  if. (''-:t)+.((,'/')-:t)+.('/root'-:t)+.('/usr/'-:5{.t) do.
    t=. '/tmp/',":2!:6''
    1!:5 ::] <t
  end.
  SystemFolders=: SystemFolders, 'home';t
end.

if. (<'temp') -.@e. {."1 SystemFolders do.
  if. 'Win'-:UNAME do. 1!:5 ::] <t=. (2!:5'USERPROFILE'),'\Temp'
  elseif. 'Android'-:UNAME do. t=. '/sdcard'
  elseif. ((<UNAME)e.'Linux';'OpenBSD';'FreeBSD';'Darwin') do. 1!:5 ::] <t=. '/tmp/',":2!:6''
  elseif. do. t=. ''
  end.
  SystemFolders=: SystemFolders, 'temp';t
end.
''
)

4!:55 <'setjdefs'

18!:4 <'z'
18!:4 <'z'
UNXLIB=: ([: <;._1 ' ',]);._2 (0 : 0)
libc.so.6 libc.so.7 libc.so.7 libc.so libc.dylib libc.so
libz.so.1 libz.so.7 libz.so.6 libz.so libz.dylib libz.so
libsqlite3.so.0 libsqlite3.so.0 libsqlite3.so.0 libsqlite.so libsqlite3.dylib libsqlite3.so
libxml2.so.2 libxml2.so.18.0 libxml2.so.2 libxml2.so libxml2.dylib libxml2.so
libpcre2-8.so.0 libpcre2-8.so.0.6 libpcre2-8.so.0 libpcre2-8.so libpcre2-8.dylib libpcre2-8.so
)
3 : 0^:((<UNAME)e.'Linux';'OpenBSD';'FreeBSD')''
b=. (<UNAME)i.~'Linux';'OpenBSD';'FreeBSD'
a=. 2!:0 ::(''"_) b{::'/sbin/ldconfig -p';'/sbin/ldconfig -r';'/sbin/ldconfig -r'
if. #a1=. I. '/libc.so.' E. a do.
  UNXLIB=: (<({.~i.&(10{a.))}.a}.~{.a1) (<0,b)}UNXLIB
end.
if. #a1=. I. '/libz.so.' E. a do.
  UNXLIB=: (<({.~i.&(10{a.))}.a}.~{.a1) (<1,b)}UNXLIB
end.
if. #a1=. I. '/libsqlite3.so.' E. a do.
  UNXLIB=: (<({.~i.&(10{a.))}.a}.~{.a1) (<2,b)}UNXLIB
end.
if. #a1=. I. '/libxml2.so.' E. a do.
  UNXLIB=: (<({.~i.&(10{a.))}.a}.~{.a1) (<3,b)}UNXLIB
end.
if. #a1=. I. '/libpcre2-8.so.' E. a do.
  UNXLIB=: (<({.~i.&(10{a.))}.a}.~{.a1) (<4,b)}UNXLIB
end.
''
)
unxlib=: 3 : 0
r=. (;: 'c z sqlite3 libxml2 pcre2') i. <,y
c=. (;: 'Linux OpenBSD FreeBSD Android Darwin') i. <UNAME_z_
(<r,c) {:: UNXLIB_z_
)
18!:4 <'z'
anddf=: 4 : '''libj.so android_download_file > i *c *c'' 15!:0 x;y'
andunzip=: 3 : 0
'' andunzip y
:
'libj.so java_unzip_file > i *c *c' 15!:0 y;x
)
android_exec_am=: 3 : 0
'intent uri mimetype flags'=. 4{.y
user=. (UserNumber_ja_"_)^:(0=4!:0<'UserNumber_ja_') (_1)
2!:0 ::0: utf8 'am start ',((user>:0)#'--user ', ":user),' -a ', intent, ' -d ', (dquote uri), ((*#mimetype)#' -t ', mimetype), ((0~:flags)#' -f ', ":flags)
i.0 0
)

android_exec_host=: 2!:1@(3&{.)`android_exec_am@.(0=4!:0<'AndroidPackage')
android_getdisplaymetrics=: 3 : 0
dm=. 0 2 320 1280 2 720 243.247 244.273
if. 18<:APILEVEL_ja_ do.
  if. 0=4!:0<'android_getdisplaymetrics_memo_ja_' do.
    dm=. android_getdisplaymetrics_memo_ja_
  else.
    try.
      densityDpi=. 0&". ' '-.~ (}.~ i:&' ') LF-.~ 2!:0 'wm density'
      ('widthPixels heightPixels')=: 0&". ;._1 'x', ' '-.~ (}.~ i:&' ') LF-.~ 2!:0 'wm size'
      density=. (0.5*heightPixels>480) + (0.5*heightPixels>320) + densityDpi% 160
      dm=. 1 2 3 4 5 (density, densityDpi, heightPixels, density, widthPixels)}dm
    catch. end.
    android_getdisplaymetrics_memo_ja_=: dm
  end.
end.
'DM_density_ja_ DM_densityDpi_ja_ DM_scaledDensity_ja_'=: 1 2 4{dm
dm
)
18!:4 <'z'
'TAB LF FF CR DEL EAV'=: 9 10 12 13 127 255{a.
LF2=: LF,LF
CRLF=: CR,LF
EMPTY=: 0 0$0
Debug=: 0
'noun adverb conjunction verb monad dyad'=: 0 1 2 3 3 4
setalpha=: 16bff&$: : (4 : 0)
((_32&(34 b.))^:IF64 _8 (32 b.) x)&(23 b.) 16bffffff (17 b.) y
)
getalpha=: 16bff (17 b.) _24&(34 b.)
abspath=: 3 : 0
if. (1 e. '://'&E.) y=. ,jpathsep y do. y return. end.
if. IFWIN do.
  assert. 0<rc=. >@{. cdrc=. 'kernel32 GetFullPathNameW   i *w i *w *w'&cd (uucp y);((#;])1024$u:' '),<<0
  y=. jpathsep utf8 rc{.3{::cdrc
elseif. ('/' ~: {.) y do.
  y=. iospath^:IFIOS (1!:43'') , '/' , utf8 y
end.
y
)
apply=: 128!:2
assert=: 0 0 $ 13!:8^:((0 e. ])`(12"_))
bind=: 2 : 'u@(v"_)'
boxopen=: <^:(0&(>: L.))
boxxopen=: <^:((> L.)~ *@#)
bx=: I.
clear=: 3 : 0
". 'do_',(' '-.~y),'_ '' (#~ -.@(4!:55)) (4!:1) 0 1 2 3'''
)
cutLF=: 3 : 'if. 0 (<L.) y do. y else. a: -.~ <;._2 y,LF end.'
cutopen=: 3 : 0
y cutopen~ (' ',LF) {~ LF e. ,y
:
if. 0 (<L.) y do. y return. end.
if. 1 < #$y do. <"_1 y return. end.
(<'') -.~ (y e.x) <;._2 y=. y,1{.x
)
datatype=: 3 : 0
n=. 1 2 4 8 16 32 64 128 1024 2048 4096 8192 16384 32768 65536 131072 262144
t=. '/boolean/literal/integer/floating/complex/boxed/extended/rational'
t=. t,'/sparse boolean/sparse literal/sparse integer/sparse floating'
t=. t,'/sparse complex/sparse boxed/symbol/unicode/unicode4'
(n i. 3!:0 y) pick <;._1 t
)
def=: :
define=: : 0
H=. '0123456789ABCDEF'
h=. '0123456789abcdef'
dfh=: (16 #. 16 | (H,h) i. ]) :.hfd
hfd=: (h {~ 16 #.^:_1 ]) :.dfh
4!:55 'H';'h'
do=: ".
drop=: }.
each=: &.>
empty=: EMPTY"_
erase=: [: 4!:55 ;: ::]
every=: &>
evtloop=: EMPTY"_
exit=: 2!:55
expand=: #^:_1
file2url=: 3 : 0
if. (1 e. '://'&E.) ,y do. y return. end.
y=. (' ';'%20') stringreplace abspath y -. '"'
if. IFWIN do.
  if. '//'-:2{.y do.
    'file:',y
  else.
    'file:///',y
  end.
else.
  'file://',y
end.
)
fixdotdot=: 3 : 0
while. 1 e. r=. '../' E. y do.
  y=. ((2+p)}.y),~ ({.~ i:&'/') }: (p=. {.I.r){.y
end.
y
)
fliprgb=: 3 : 0
s=. $y
d=. ((#y),4)$2 (3!:4) y=. <.,y
d=. 2 1 0 3{"1 d
s$_2(3!:4),d
)
getargs=: 3 : 0
ARGV getargs y
:
argb=. (]`(([: < 1: {. }.) , [: < 2: }. ])@.('-'"_ = {.))&.> x
parm=. 32 = ;(3!:0)&.> argb
((-. parm)#argb);(>parm#argb);(". (0 = isatty 0)#'stdin ''''')
)
getenv=: 2!:5
inv=: inverse=: ^:_1
iospath=: 3 : 0
if. IFIOS *. ('/j'-:2{.y) do. y=. y,~ '/Documents',~ 2!:5 'HOME' end.
y
)
3 : 0''
if. IFUNIX do.
  isatty=: ((unxlib 'c'),' isatty > i i') & (15!:0)
else.
  isatty=: 2: = ('kernel32 GetFileType > i x' & (15!:0)) @ ('kernel32 GetStdHandle > x i'& (15!:0)) @ - @ (10&+)
end.
''
)
isutf8=: 0:`(1:@(7&u:) :: 0:)@.(2=3!:0)
isutf16=: 0:`(1:@(8&u:) :: 0:)@.(131072=3!:0)
items=: "_1
fetch=: {::
leaf=: L:0
list=: 3 : 0
w=. {.wcsize''
w list y
:
if. 0=#y do. i.0 0 return. end.
if. 2>#$y=. >y do.
  d=. (' ',LF) {~ LF e. y=. toJ ": y
  y=. [;._2 y, d #~ d ~: {: y
end.
y=. y-. ' '{.~ c=. {:$ y=. (": y),.' '
(- 1>. <. x % c) ;\ <"1 y
)
nameclass=: nc=: 4!:0
namelist=: 4!:1
nl=: 3 : 0
'' nl y
:
if. 0 e. #y do. y=. 0 1 2 3 end.

if. 1 4 8 e.~ 3!:0 y do.
  nms=. (4!:1 y) -. ;: 'x y x. y.'
else.
  nms=. cutopen_z_ y
end.

if. 0 e. #nms do. return. end.

if. #t=. x -. ' ' do.
  'n s'=. '~*' e. t
  t=. t -. '~*'
  b=. t&E. &> nms
  if. s do. b=. +./"1 b
  else. b=. {."1 b end.
  nms=. nms #~ n ~: b
end.
)
names=: list_z_ @ nl
Note=: 3 : '0 0 $ 0 : 0' : [
on=: @:
pick=: >@{
rows=: "1
script=: [: 3 : '0!:0 y [ 4!:55<''y''' jpath_z_ &.: >
scriptd=: [: 3 : '0!:1 y [ 4!:55<''y''' jpath_z_ &.: >
stdout=: 1!:2&4
stderr=: 1!:2&5
stdin=: 1!:1@3: :. stdout
sign=: *
sminfo=: 3 : 0
if. IFJHS do. smoutput >{:boxopen y
elseif. IFQT do. wdinfo_jqtide_ y
elseif. IFJA do. wdinfo_ja_ y
elseif. IFJNET do. wdinfo_jnet_ y
elseif. (0-:11!:0 ::0:'qwd') < 3=4!:0<'wdinfo' do. wdinfo y
elseif. do. smoutput >{:boxopen y end.
)
echo=: 0 0 $ 1!:2&2
smoutput=: 0 0 $ 1!:2&2
tmoutput=: 0 0 $ 1!:2&4
sort=: /:~ : /:
split=: {. ,&< }.
table=: 1 : 0~
:
(((#~LF-.@e.])5!:5<'u');,.y),.({.;}.)":x,y u/x
)
take=: {.
timespacex=: 6!:2 , 7!:2@]
timex=: 6!:2
tolist=: }.@;@:(LF&,@,@":&.>)
tolower=: 3 : 0`((((97+i.26){a.)(65+i.26)}a.) {~ a. i. ])@.(2 = 3!:0)
x=. I. 26 > n=. ((65+i.26){a.) i. t=. ,y
($y) $ ((x{n) { (97+i.26){a.) x}t
)

toupper=: 3 : 0`((((65+i.26){a.)(97+i.26)}a.) {~ a. i. ])@.(2 = 3!:0)
x=. I. 26 > n=. ((97+i.26){a.) i. t=. ,y
($y) $ ((x{n) { (65+i.26){a.) x}t
)

3 : 0''
try.
  tolower=: 0&(3!:12)
  toupper=: 1&(3!:12)
catch. end.
''
)
t=. <;._1 '/invalid name/not defined/noun/adverb/conjunction/verb/unknown'
type=: {&t@(2&+)@(4!:0)&boxopen
ucp=: 7&u:
ucpcount=: # @ (7&u:)
3 : 0''
if. IFUNIX do.
  usleep=: 3 : ('''',(unxlib 'c'),' usleep > i i''&(15!:0) >.y')
else.
  usleep=: 3 : '0: ''kernel32 Sleep > n i''&(15!:0) >.y % 1000'
end.
EMPTY
)
utf8=: 8&u:
uucp=: u:@(7&u:)
toJ=: (LF I.@(CR=])}]) @: (#~ -.@(CRLF&E.@,))
toCRLF=: 2&}. @: ; @: (((CR&,)&.>)@<;.1@(LF&,)@toJ)

3 : 0''
if. 5=9!:12'' do.
  toHOST=: ]
else.
  toHOST=: toCRLF
end.
1
)
18!:4 <'z'
coclass=: 18!:4 @ boxxopen
cocreate=: 18!:3
cocurrent=: 18!:4 @ boxxopen
codestroy=: coerase @ coname
coerase=: 18!:55
cofullname=: 3 : 0
y=. ,> y
if. #y do.
  if. ('_' = {: y) +: 1 e. '__' E. y do.
    y,'_',(>18!:5''),'_'
  end.
end.
)
coinsert=: 3 : 0
n=. ;: :: ] y
p=. ; (, 18!:2) @ < each n
p=. ~. (18!:2 coname''), p
(p /: p = <,'z') 18!:2 coname''
)
coname=: 18!:5
conames=: list_z_ @ conl
conew=: 3 : 0
c=. <y
obj=. cocreate''
coinsert__obj c
COCREATOR__obj=: coname''
obj
:
w=. conew y
create__w x
w
)
conl=: 18!:1 @ (, 0 1"_ #~ # = 0:)
copath=: 18!:2 & boxxopen
coreset=: 3 : 0
0 0$coerase conl 1
)
cocurrent 'z'
cofind=: 3 : 0
r=. (<,>y) (4 : 'try. x e. nl__y $0 catch. 0 end.'"0 # ]) 18!:1]0 1
if. 0=#r do. i.0 2 end.
)
cofindv=: 3 : 0
lcs=. cofind y
if. #lcs do.
  lcs ,. ". each (<y,'_') ,each lcs ,each '_'
end.
)
coinfo=: 3 : 0
ref=. boxxopen y
if. 0 e. $ref do. i.0 4 return. end.
if. 0=4!:0 <'COCREATOR__ref'
do. c=. COCREATOR__ref else. c=. a: end.
(conouns ref),ref,c,< ;:inverse copath ref
)
conouns=: 3 : 0 "0
n=. nl 0
t=. n#~ (<y)-:&> ".each n
< ;: inverse t
)
conounsx=: 3 : 0
r=. ''
if. #y do.
  s=. #y=. boxxopen y
  loc=. conl 0
  for_i. loc do. r=. r,conouns__i y end.
  r=. (r~:a:) # (y$~#r),.r,.s#loc
end.
/:~~.r
)
copathnl=: 3 : 0
'' copathnl y
:
r=. ''
t=. (coname''),copath coname''
for_i. t -. <,'z' do.
  r=. r,x nl__i y
end.
/:~~.r
)
copathnlx=: 3 : 0
'' copathnlx y
:
r=. ''
t=. (coname''),copath coname''
for_i. t=. t -. <,'z' do.
  r=. r,<x nl__i y
end.
n=. ~.;r
n,.|:( n&e. &> r) #each t
)
costate=: 3 : 0
r=. ,: ;:'refs id creator path'
if. #n=. conl 1 do. r,coinfo &> n /: 0 ".&> n end.
)
cocurrent 'z'
cd=: 15!:0
memr=: 15!:1
memw=: 15!:2
mema=: 15!:3
memf=: 15!:4
memu=: '' 1 : 'try. 15!:15 m catch. a: { ] return. end. 15!:15'
cdf=: 15!:5
cder=: 15!:10
cderx=: 15!:11
symget=: 15!:6
symset=: 15!:7
symdat=: 15!:14
cdcb=: 15!:13
JB01=: 1
JCHAR=: 2
JSTR=: _1,JCHAR
JINT=: 4
JPTR=: JINT
JFL=: 8
JCMPX=: 16
JBOXED=: 32
JSB=: 65536
JCHAR2=: 131072
JSTR2=: _1,JCHAR2
JCHAR4=: 262144
JSTR4=: _1,JCHAR4
JTYPES=: JB01,JCHAR,JINT,JPTR,JFL,JCMPX,JBOXED,JSB,JCHAR2,JCHAR4
JSIZES=: >IF64{1 1 4 4 8 16 4 4 2 4;1 1 8 8 8 16 8 8 2 4
SZI=: IF64{4 8
ic=: 3!:4
fc=: 3!:5
endian=: |.^:('a'={.2 ic a.i.'a')
Endian=: |.^:('a'~:{.2 ic a.i.'a')
AND=: $:/ : (17 b.)
OR=: $:/ : (23 b.)
XOR=: $:/ : (22 b.)
cocurrent 'z'
calendar=: 3 : 0
0 calendar y
:
a=. ((j<100)*(-100&|){.6!:0'')+j=. {.y
b=. (a-x)+-/<.4 100 400%~<:a
r=. 28+3,(~:/0=4 100 400|a),10$5$3 2
r=. (-7|b+0,+/\}:r)|."0 1 r(]&:>:*"1>/)i.42
m=. (<:}.y),i.12*1=#y
h=. 'JanFebMarAprMayJunJulAugSepOctNovDec'
h=. ((x*3)|.' Su Mo Tu We Th Fr Sa'),:"1~_3(_12&{.)\h
<"2 m{h,"2[12 6 21 ($,) r{' ',3":1+i.31 1
)
getdate=: 3 : 0
0 getdate y
:
r=. ''
opt=. x
chr=. [: -. [: *./ e.&'0123456789 '
dat=. ' ' (I. y e.',-/:') } y

if. chr dat do.
  opt=. 0
  dat=. a: -.~ <;._1 ' ',dat
  if. 1=#dat do. r return. end.
  typ=. chr &> dat
  dat=. (2{.typ{dat),{:dat
  mth=. 3{.>1{dat
  uc=. 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
  lc=. 'abcdefghijklmnopqrstuvwxyz'
  mth=. (lc,a.) {~ mth i.~ uc,a.
  mos=. _3[\'janfebmaraprmayjunjulaugsepoctnovdec'
  mth=. <": >:mos i. mth
  dat=. ;' ',each mth 1 } dat
end.

dat=. ". :: (''"_) dat
if. 0 e. #dat do. return. end.

if. 3 ~: #dat do. r return. end.

if. 31 < {.dat do. 'y m d'=. dat
else. ((opt|.'d m '),' y')=. dat
end.

if. y<100 do.
  y=. y + (-100&|) {. 6!:0''
end.

(#~ valdate) y,m,d
)
isotimestamp=: 3 : 0
r=. }: $y
t=. _6 [\ , 6 {."1 y
d=. '--b:' 4 7 10 13 }"1 [ 4 3 3 3 3 ": <. 5{."1 <.t
d=. d ,. ':' 0 }"1 [ 7j3 ": ,. {:"1 t
c=. {: $d
d=. ,d
d=. '0' (I. d=' ')} d
d=. ' ' (I. d='b')} d
(r,c) $ d
)
todate=: 3 : 0
0 todate y
:
s=. $y
y=. s$ 0 (I. (,y) e. _ __)},y
a=. 657377.75 +, y=. <. p=. y
d=. <. a - 36524.25 * c=. <. a % 36524.25
d=. <.1.75 + d - 365.25 * y=. <. (d+0.75) % 365.25
r=. (1+12|m+2) ,: <. 0.41+d-30.6* m=. <. (d-0.59) % 30.6
if. 1<x do.
  h=. <. t=. 24*(1&|) ,p
  mm=. <. t=. 60*t-h
  ss=. 60*t-mm
  r=. s $ |: ((c*100)+y+m >: 10) ,r , |: h ,. mm,. ss
else.
  r=. s $ |: ((c*100)+y+m >: 10) ,r
end.
if. 1=x do. r=. 100 #. r end.
r
)
todayno=: 3 : 0
0 todayno y
:
a=. y
if. 1=x do. a=. 0 100 100 #: a end.
a=. ((*/r=. }: $a) , {:$a) $,a
if. 1<x do.
  'y m d h mm s'=. <"_1 |: a
else.
  'y m d'=. <"_1 |: a
end.
y=. 0 100 #: y - m <: 2
n=. +/ |: <. 36524.25 365.25 *"1 y
n=. n + <. 0.41 + 0 30.6 #. (12 | m-3),"0 d
if. 1<x do.
  n=. n + (24 60 60#.h,.mm,.s)%24*3600
end.
0 >. r $ n - 657378
)
tsdiff=: 4 : 0
r=. -/"2 d=. _6 (_3&([\)) \ ,x,"1 y
if. #i=. i#i.#i=. 0 > 2{"1 r do.
  j=. (-/0=4 100 400 |/ (<i;1;0){d)* 2=m=. (<i;1;1){d
  j=. _1,.j + m{0 31 28 31 30 31 30 31 31 30 31 30 31
  n=. <i;1 2
  r=. (j + n{r) n } r
end.
r +/ . % 1 12 365
)
tsrep=: 3 : 0
0 tsrep y
:
if. x do.
  r=. $y
  'w n t'=. |: 0 86400 1000 #: ,y
  w=. w + 657377.75
  d=. <. w - 36524.25 * c=. <. w % 36524.25
  d=. <.1.75 + d - 365.25 * w=. <. (d+0.75) % 365.25
  s=. (1+12|m+2) ,: <. 0.41+d-30.6* m=. <. (d-0.59) % 30.6
  s=. |: ((c*100)+w+m >: 10) ,s
  r $ s,. (_3{. &> t%1000) +"1 [ 0 60 60 #: n
else.
  a=. ((*/r=. }: $y) , {:$y) $, y
  'w m d'=. <"_1 |: 3{."1 a
  w=. 0 100 #: w - m <: 2
  n=. +/ |: <. 36524.25 365.25 *"1 w
  n=. n + <. 0.41 + 0 30.6 #. (12 | m-3),"0 d
  s=. 3600000 60000 1000 +/ .*"1 [ 3}."1 a
  r $ s+86400000 * n - 657378
end.
)
timestamp=: 3 : 0
if. 0 = #y do. w=. 6!:0'' else. w=. y end.
r=. }: $ w
t=. 2 1 0 3 4 5 {"1 [ _6 [\ , 6 {."1 <. w
d=. '+++::' 2 6 11 14 17 }"1 [ 2 4 5 3 3 3 ": t
mth=. _3[\'   JanFebMarAprMayJunJulAugSepOctNovDec'
d=. ,((1 {"1 t) { mth) 3 4 5 }"1 d
d=. '0' (I. d=' ') } d
d=. ' ' (I. d='+') } d
(r,20) $ d
)
tstamp=: timestamp
valdate=: 3 : 0
s=. }:$y
'w m d'=. t=. |:((*/s),3)$,y
b=. *./(t=<.t),(_1 0 0<t),12>:m
day=. (13|m){0 31 28 31 30 31 30 31 31 30 31 30 31
day=. day+(m=2)*-/0=4 100 400|/w
s$b*d<:day
)
weekday=: 7 | 3 + todayno
weeknumber=: 3 : 0
yr=. {.y
sd=. 1 ((i.~weekday){]) ((<:yr),.12,.29+i.3),yr,.1,.1+i.4
wk=. >.7%~>: y -&todayno sd
if. wk >weeksinyear yr do.
  (>:yr),1
elseif. wk=0 do.
  (,weeksinyear)<:yr
elseif. do.
  yr,wk
end.
)
weeksinyear=: 3 : '52+ +./"1 [ 4=weekday(1 1,:12 31),"0 1/~ y'
cocurrent 'z'
dbr=: 13!:0
dbs=: 13!:1
dbsq=: 13!:2
dbss=: 13!:3
dbrun=: 13!:4
dbnxt=: 13!:5
dbret=: 13!:6
dbjmp=: 13!:7
dbsig=: 13!:8
dbrr=: 13!:9
dbrrx=: 13!:10
dberr=: 13!:11
dberm=: 13!:12
dbstk=: 13!:13
dblxq=: 13!:14
dblxs=: 13!:15
dbtrace=: 13!:16
dbq=: 13!:17
dbst=: 13!:18
dbcut=: 13!:19
dbover=: 13!:20
dbinto=: 13!:21
dbout=: 13!:22
dbctx=: 10 10&$: : (4 : 0)
if. -.13!:17'' do. 0 0$'' return. end.
try.
  'before after'=. 2{. <. , x, 10 10
catch.
  'before after'=. 10 10
end.
if. 0= #d=. 13!:13'' do. 0 0$'' return. end.
if. '*' -.@e. sus=. >{:"1 d do. 0 0$'' return. end.
'name ln nc def src'=. 0 2 3 4 5{(sus i. '*'){d
dyad=. {: ':'&e.;._2 ] 13!:12''
if. (_2{.def) -: LF,')' do.
  def=. }.def [ def0=. {.def=. }:<;._2 def,LF
else.
  def=. ,<def [ def0=: ''
end.
if. def e.~ <,':' do.
  if. dyad do.
    def=. def}.~ >: def i. <,':'
  else.
    def=. def{.~ def i. <,':'
  end.
end.
min=. 0>.ln-before [ max=. (<:#def)<.ln+after
ctx=. ((,.ln=range){' >'),"1 '[',"1 (":,.range) ,"1 ('] ') ,"1 >def{~range=. min + i. >:max-min
; ,&LF&.> (<'@@ ', name, '[', (dyad#':'), (":ln) ,'] *', (nc{' acv'),' @@ ', src), def0, <@dtb "1 ctx
)
dbg=: 3 : 0
if. -.IFQT do.
  13!:0 y return.
end.
if. y do.
  if. _1 = 4!:0 <'jdb_open_jdebug_' do.
    0!:0 <jpath '~addons/ide/qt/debugs.ijs'
  end.
  jdb_open_jdebug_''
  13!:0 [ 1
else.
  jdb_close_jdebug_ :: ] ''
  13!:15 ''
  13!:0 [ 0
end.
)
dblocals=: _1&$: : (4 : 0)
stk=. }. 13!:13''
if. 0=#y do. y=. a: else. y=. (y e. i.#stk) # y end.
loc=. (<y ; 0 7) { stk
if. x-: 0 do.
  {."1 &.>@:{:"1 loc
elseif. -. x-:_1 do.
  t=. ;: ::] x
  f=. ({."1 e. t"_) # ]
  ({."1 loc) ,. f &.> {:"1 loc
end.
)
dbstack=: 3 : 0
hdr=. ;:'name en ln nc args locals susp'
stk=. }. 13!:13''
if. #y do.
  if. 2=3!:0 y do.
    stk=. stk #~ (<y)={."1 stk
  else.
    stk=. ((#stk)<.,y){.stk
  end.
end.
stk=. 1 1 1 1 0 0 1 1 1 #"1 stk
stk=. hdr, ": &.> stk
wds=. ({:@$@":@,.)"1 |: stk
len=. 20 >.<.-:({.wcsize'') - +/8, 4 {. wds
tc=. (len+1)&<.@$ {.!.'.' ({.~ len&<.@$)
tc@": each stk
)
dbstop=: 3 : 0
if. 0 e. #y -. ' ' do. 13!:3'' return. end.
t=. 13!:2''
if. #t do. t=. <;._2 t, ';' -. {:t end.
t=. ~. t, (;: ^: (L.=0:) y) ,&.> <' *:*'
13!:3 ; t ,&.> ';'
)
dbstops=: 3 : 0
13!:3 ; (;: ^: (L.=0:) y) ,&.> <' *:*;'
)
dbstopme=: 3 : 0
if. y do.
  if. 0 e. $c=. }. 13!:13'' do. return. end.
  c=. (> {. {. c), ' *:*'
  t=. 13!:2''
  if. #t do. t=. <;._2 t, ';' -. {:t end.
  t=. ~. t, <c
  13!:3 }: ; t ,&.> ';'
end.
)
dbstopnext=: 3 : 0
if. y do.
  if. 0 e. $c=. }. 13!:13'' do. return. end.
  'd n a'=. 0 2 6 { {. c
  c=. d,' ',(':'#~2=#a),":n+1
  t=. 13!:2''
  if. #t do. t=. <;._2 t, ';' -. {:t end.
  t=. ~. t, <c
  13!:3 }: ; t ,&.> ';'
end.
)
dbview=: 3 : 0
if. -.IFQT do. return. end.
if. _1 = 4!:0 <'jdbview_jdbview_' do.
  'require'~'~addons/ide/qt/dbview.ijs'
end.
jdbview_jdbview_ }. 13!:13''
)
dbhelp=: 0 : 0
The call stack (dbstk'') is a 9-column boxed matrix:
  0  name
  1  error number, or 0 if this call has no error.
  2  line number
  3  name class
  4  definition
  5  source script
  6  argument list
  7  locals
  8  suspense

f is the name of a verb
      dbss 'f 0'   monadic line 0
      dbss 'f :2'  dyadic line 2
      dbss 'f *:*' all lines

dbr     13!:0  reset, set suspension mode (0=disable, 1=enable)
dbs     13!:1  display stack
dbsq    13!:2  stop query
dbss    13!:3  stop set
dbrun   13!:4  run again (from current stop)
dbnxt   13!:5  run next (skip line and run)
dbret   13!:6  exit and return argument
dbjmp   13!:7  jump to line number
dbsig   13!:8  signal error
dbrr    13!:9  re-run with specified arguments
dbrrx   13!:10 re-run with specified executed arguments
dberr   13!:11 last error number
dberm   13!:12 last error message
dbstk   13!:13 call stack
dblxq   13!:14 latent expression query
dblxs   13!:15 latent expression set
dbtrace 13!:16 trace control
dbq     13!:17 queries suspension mode (set by dbr)
dbst    13!:18 returns stack text
(these 4 verbs are subject to change without notice)
dbcut   13!:19 cut back
dbover  13!:20 step over (13!:20'' or moveline 13!:20'')
dbinto  13!:21 step into ...
dbout   13!:22 step out  ...
(utilities)
dbctx       display context
dbg         turn debug window on/off
dblocals    display local names on stack
dbstack     display stack
dbstop      add stop definitions
dbstops     set all stop definitions
dbstopme    stop current definition
dbstopnext  stop current definition at next line
dbview      (GUI only) view stack
)
cocurrent 'z'
dir=: 3 : 0
'n' dir y
:
ps=. '/'
y=. jpath y,(0=#y)#'*'
y=. y,((':',ps) e.~ {:y)#'*'
if. 0=#dr=. 1!:0 y do. empty'' return. end.
fls=. 'd' ~: 4{"1>4{"1 dr
if. (1=#dr) *. 0={.fls do.
  r=. x dir y,ps,'*'
  if. #r do. r return. end.
end.
if. fmt=. 2=3!:0 x do. opt=. 2 1
else. opt=. 2{.x end.
if. 0={:opt do. fls=. 1#~#dr=. fls#dr end.
if. 0=#dr do. empty'' return. end.
nms=. {."1 dr
nms=. nms ,&.> fls{ps;''
ndx=. /: (":,.fls),.>nms
if. 0=opt do.
  list >ndx{nms
elseif. 1=opt do.
  path=. (+./\.y=ps)#y
  path&,&.>ndx{nms
elseif. fmt<2=opt do.
  ndx{nms,.}."1 dr
elseif. fmt do.
  'nms ts size'=. |:3{."1 dr
  ds=. '   <dir>    ' ((-.fls)#i.#fls) } 12 ":,.size
  mth=. _3[\'   JanFebMarAprMayJunJulAugSepOctNovDec'
  f=. > @ ([: _2&{. [: '0'&, ": )&.>
  'y m d h n s'=. f&> ,<"1 |: 100|ts
  m=. (1{"1 ts){mth
  time=. d,.'-',.m,.'-',.y,.' ',.h,.':',.n,.':',.s
  dat=. (>nms),.ds,.' ',.time
  dat /: fls,. /:/: >(3|'dns'i.x){ts;nms;size
elseif. 1 do.
  'invalid left argument'
end.
)
dircompare=: 3 : 0
0 dircompare y
:
if. 0=#y do.
  '''long dirtree timestamps'' dircompare dir1;dir2'
  return.
end.

opt=. 3 {. x
res=. opt dircompares y
if. 0 = L. res do. return. end.

ps=. '/'
'a b c'=. res

'x y'=. jpath each cutopen y
x=. x, ps #~ (*#x) *. ps~:_1{.x
y=. y, ps #~ (*#y) *. ps~:_1{.y

r=. 'comparing  ',x,'  and  ',y,LF

if. #a do.
  r=. r,LF,'not in  ',y,':',LF,,(list a),.LF
end.

if. #b do.
  r=. r,LF,'not in  ',x,':',LF,,(list b),.LF
end.

if. +/ # &> c do.
  'cf cd'=. c
  r=. r,LF,'not same in both:',LF,,(list cf),.LF
  if. {.opt do.
    r=. r,LF,;(,&(LF2)) &.> cd
  end.

end.

if. 0=+/# S:0 res do. r=. r,'no difference',LF end.

}:r
)
dircompares=: 3 : 0
0 dircompares y
:
ps=. '/'
opt=. 3{. x
'x y'=. jpath each cutopen y
x=. x, ps #~ (*#x) *. ps~:_1{.x
y=. y, ps #~ (*#y) *. ps~:_1{.y

if. 1{opt do.
  dx=. dirtree x [ dy=. dirtree y
else.
  dx=. 2 0 dir x [ dy=. 2 0 dir y
end.

if. dx -: dy do. 'no difference' return. end.
if. 0 e. #dx do. 'first directory is empty' return. end.
if. 0 e. #dy do. 'second directory is empty' return. end.

f=. #~ [: +./\. =&ps
sx=. f x
sy=. f y
fx=. {."1 dx
fy=. {."1 dy

if. 1{opt do.
  fx=. (#sx)}.&.>fx
  fy=. (#sy)}.&.>fy
  dx=. fx 0 }"0 1 dx
  dy=. fy 0 }"0 1 dy
end.

r=. <fx -. fy
r=. r , <fy -. fx

dx=. (fx e. fy)#dx
dy=. (fy e. fx)#dy

if. #j=. dx -. dy do.
  j=. {."1 j
  cmp=. <@fcompare"1 (sx&,&.>j),.sy&,&.>j

  if. 0=2{opt do.
    f=. 'no difference'&-: @ (_13&{.)
    msk=. -. f &> cmp
    j=. msk#j
    cmp=. msk#cmp
  end.

  r=. r,< j;<cmp
else.
  r=. r,a:
end.

r
)
dirfind=: 4 : 0
f=. [: 1&e. x&E.
g=. #~ [: -. [: +./\. =&'/'
d=. {."1 dirtree y
m=. f@g &> d
if. 1 e. m do. ; (m # d) ,each LF else. 0 0$'' end.
)
dirpath=: 3 : 0
0 dirpath y
:
r=. ''
t=. jpath y
ps=. '/'
if. #t do. t=. t, ps -. {:t end.
dirs=. <t
ifdir=. 'd'&= @ (4&{"1) @ > @ (4&{"1)
subdir=. ifdir # ]
while. #dirs do.
  fpath=. (>{.dirs) &,
  dirs=. }.dirs
  dat=. 1!:0 fpath '*'
  if. #dat do.
    dat=. subdir dat
    if. #dat do.
      r=. r, fpath each /:~ {."1 dat
      dirs=. (fpath @ (,&ps) each {."1 dat),dirs
    end.
  end.
end.
if. x do.
  f=. 1!:0 @ (,&(ps,'*'))
  g=. 0:`(0: e. ifdir)
  h=. g @. (*@#) @ f
  r=. r #~ h &> r
end.
if. #t do. r=. r,<}:t end.
/:~ r
)
dirss=: 4 : 0
if. (2=#x) *. 1=L. x do.
  x dirssrplc y return.
end.
sub=. ' '&(I.@(e.&(TAB,CRLF))@]})
fls=. {."1 dirtree y
if. 0 e. #fls do. 'not found: ',x return. end.
fnd=. ''
while. #fls do.
  dat=. 1!:1 <fl=. >{.fls
  fls=. }.fls
  ndx=. I. x E. dat
  if. rws=. #ndx do.
    dat=. (20$' '),dat,30$' '
    dat=. (rws,50)$sub(,ndx+/i.50){dat
    fnd=. fnd,LF2,fl,' (',(":#ndx),')'
    fnd=. fnd,,LF,.dat
  end.
end.
if. #fnd do. 2}.fnd else. 'not found: ',x end.
)
dirssrplc=: 4 : 0
fls=. {."1 dirtree y
if. 0 e. #fls do.
  'no files found' return.
end.
r=. (x&fssrplc) each fls
b=. r ~: <'no match found'
j=. >b # fls , each ': '&, each r
}: , j ,. LF
)
dirtree=: 3 : 0
0 dirtree y
:
if. 0=4!:0 <'DirTreeX_j_' do.
  ex=. cutopen DirTreeX_j_
else.
  ex=. ''
end.
r=. i.0 3
ps=. '/'
y=. jpath y
y=. y #~ (+./\ *. +./\.) y~:' '
y=. y,(0=#y)#'*'
if. ps={:y do. y=. y,'*' end.
if. -. '*' e. y do.
  if. 1 = #j=. 1!:0 y do.
    select. 'hd' = 1 4 { >4{,j
    case. 0 1 do. x dirtree y,ps,'*' return.
    case. 1 1 do. i.0 3 return.
    end.
  end.
end.
ts=. 100"_ #. 6: {. 0: >. <. - # {. 1980"_
'path ext'=. (b#y);(-.b=. +./\.y=ps)#y
if. #dl=. 1!:0 y do.
  att=. > 4{"1 dl
  fl=. (('h' ~: 1{"1 att) *. 'd' ~: 4{"1 att)#dl
  if. #fl do.
    r=. r,(path&,&.>{."1 fl),.1 2{"1 fl
  end.
end.
if. #dl=. 1!:0 path,'*' do.
  att=. > 4{"1 dl
  dr=. {."1 (('h' ~: 1{"1 att) *. 'd' = 4{"1 att) # dl
  dr=. dr -. ex
  if. #dr do.
    r=. r,;x&dirtree@(path&,@,&(ps,ext)) &.> dr
  end.
end.
r #~ (ts x) <: ts &> 1{"1 r
)
dirused=: [: (# , +/ @ ; @ (2: {"1 ])) 0&dirtree
cocurrent 'z'

fexists=: #~ fexist
f2utf8=: ]
fboxname=: <@(fixdotdot^:IFIOS)@jpath_j_@(8 u: >) ::]
fappend=: 4 : 0
(,x) (#@[ [ 1!:3) :: _1: fboxname y
)
fappends=: 4 : 0
(fputs x) (#@[ [ 1!:3) :: _1: fboxname y
)
fapplylines=: 1 : 0
0 u fapplylines y
:
y=. > fboxname y
s=. 1!:4 <y
if. s = _1 do. return. end.
p=. 0
dat=. ''
while. p < s do.
  b=. 1e6 <. s-p
  dat=. dat, 1!:11 y;p,b
  p=. p + b
  if. p = s do.
    len=. #dat=. dat, LF -. {:dat
  elseif. (#dat) < len=. 1 + dat i:LF do.
    'file not in LF-delimited lines' 13!:8[3
  end.
  if. x do.
    u ;.2 len {. dat
  else.
    u ;._2 CR -.~ len {. dat
  end.
  dat=. len }. dat
end.
)
fcopynew=: 4 : 0
dat=. fread each boxopen y
if. (<_1) e. dat do. _1 return. end.
dat=. ; dat
if. dat -: fread :: 0: x do. 0,#dat else.
  if. _1=dat fwrite x do. _1 else. 1,#dat end.
end.
)
fdir=: 1!:0@fboxname
ferase=: (1!:55 :: _1:) @ (fboxname &>) @ boxopen
fexist=: (1:@(1!:4) :: 0:) @ (fboxname &>) @ boxopen
fgets=: 3 : 0
y=. (-(26{a.)={:y) }. y
if. 0=#y do. '' return. end.
y,LF -. {:y=. toJ y
)
fmakex=: (] 1!:7~ 'x' 2 5 8} 1!:7) @ fboxname
fpathcreate=: 3 : 0
if. 0=#y do. 1 return. end.
p=. (,'/'-.{:) jpathsep y
if. # 1!:0 }: p do. 1 return. end.
for_n. I. p='/' do. 1!:5 :: 0: < n{.p end.
)
fpathname=: +./\.@:=&'/' (# ; -.@[ # ]) ]
fread=: 3 : 0
if. 1 = #y=. boxopen y do.
  1!:1 :: _1: fboxname y
else.
  1!:11 :: _1: (fboxname {.y),{:y
end.
:
x freads y
)
freadblock=: 3 : 0
1e6 freadblock y
:
'f p'=. y
f=. > fboxname f
s=. 1!:4 <f
if. s = _1 do. return. end.
if. (s = 0) +. p >: s do. '';p return. end.
if. x < s-p do.
  dat=. 1!:11 f;p,x
  len=. 1 + dat i: LF
  if. len > #dat do.
    'file not in LF-delimited lines' 13!:8[3
  end.
  p=. p + len
  dat=. len {. dat
else.
  dat=. 1!:11 f;p,s-p
  dat=. dat, LF -. {: dat
  p=. s
end.
(toJ dat);p
)
freadr=: 3 : 0
'f s'=. 2{.boxopen y
f=. fboxname f
max=. 1!:4 :: _1: f
if. max -: _1 do. return. end.
pos=. 0
step=. 10000
whilst. blk = cls
do.
  blk=. step<.max-pos
  if. 0=blk do. 'file not organized in records' return. end.
  dat=. 1!:11 f,<pos,blk
  cls=. <./dat i.CRLF
  pos=. pos+step
end.
len=. cls+pos-step
dat=. 1!:11 f,<len,2<.max-len
dlm=. +/CRLF e. dat
wid=. len+dlm
s=. wid*s,0 #~ 0=#s
dat=. 1!:11 f,<s
dat=. (-wid)[\dat
(-dlm)}."1 dat
)
freads=: 3 : 0
'' freads y
:
dat=. fread y
if. (dat -: _1) +. 0=#dat do. return. end.
dat=. fgets dat
if. 'b'e.x do. dat=. <;._2 dat
elseif. 'm'e.x do. dat=. ];._2 dat
end.
)
frename=: 4 : 0
x=. > fboxname x
y=. > fboxname y
if. x -: y do. 1 return. end.
if. IFUNIX do.
  0=((unxlib 'c'),' rename > i *c *c') 15!:0 y;x
else.
  'kernel32 MoveFileW > i *w *w' 15!:0 (uucp y);uucp x
end.
)
freplace=: 4 : 0
y=. boxopen y
dat=. ,x
f=. #@[ [ 1!:12
dat f :: _1: (fboxname {.y),{:y
)
fsize=: (1!:4 :: _1:) @ (fboxname &>) @ boxopen
fss=: 4 : 0
y=. fboxname y
size=. 1!:4 :: _1: y
if. size -: _1 do. return. end.
blk=. (#x) >. 100000 <. size
r=. i.pos=. 0
while. pos < size do.
  dat=. 1!:11 y,<pos,blk <. size-pos
  r=. r,pos+I. x E. dat
  pos=. pos+blk+1-#x
end.
r
)
fssrplc=: fstringreplace
fstamp=: (1: >@{ , @ (1!:0) @ fboxname) :: _1:
fputs=: 3 : 0
dat=. ":y
if. 0 e. $dat do.
  ''
else.
  if. 1>:#$dat do.
    toHOST dat,(-.({:dat) e. CRLF) # LF
  else.
    ,dat,"1 toHOST LF
  end.
end.
)
ftype=: 3 : 0
(1:@(1!:4) :: 0:)^:IFWIN < f=. }: ^: ('/' = {:) , > fboxname y
d=. 1!:0 f
if. #d do.
  >: 'd' = 4 { > 4 { ,d
else.
  0
end.
)
fview=: 3 : 0
if. 3 ~: nc <'textview_z_' do.
  sminfo 'textview not available.' return.
end.
txt=. freads y
if. txt -: _1 do.
  sminfo 'file not found: ',,>y return.
end.
textview txt
)
fwrite=: 4 : 0
(,x) (#@[ [ 1!:2) :: _1: fboxname y
)
fwritenew=: 4 : 0
dat=. ,x
if. dat -: fread y do. 0 return. end.
dat fwrite y
)
fwrites=: 4 : 0
(fputs x) (#@[ [ 1!:2) :: _1: fboxname y
)
cocurrent 'z'
install=: 3 : 0
require 'pacman'
do_install_jpacman_ y
)
getqtbin=: 3 : 0
if. (<UNAME) -.@e. 'Linux';'OpenBSD';'FreeBSD';'Darwin';'Win' do. return. end.
if. IFQT do.
  smoutput 'must run from jconsole' return.
end.
require 'pacman'
do_getqtbin_jpacman_ y
)
cocurrent 'z'
cuts=: 2 : 0
if. n=1 do. [: u (#@[ + E. i. 1:) {. ]
elseif. n=_1 do. [: u (E. i. 1:) {. ]
elseif. n= 2 do. [: u (E. i. 1:) }. ]
elseif. 1 do. [: u (#@[ + E. i. 1:) }. ]
end.
)
cut=: ' '&$: :([: -.&a: <;._2@,~)
deb=: #~ (+. 1: |. (> </\))@(' '&~:)
debc=: #~"1 [: (+. (1: |. (> </\))) ' '&(+./ .~:)
delstring=: 4 : ';(x E.r) <@((#x)&}.) ;.1 r=. x,y'
detab=: ' ' I.@(=&TAB@])} ]
3 : 0''
try.
dlb=: 1&(128!:11)
dltb=: 2&(128!:11)
dtb=: 0&(128!:11)
catch.
dlb=: }.~ =&' ' i. 0:
dltb=: #~ [: (+./\ *. +./\.) ' '&~:
dtb=: #~ [: +./\. ' '&~:
end.
''
)
joinstring=: ''&$: : (#@[ }. <@[ ;@,. ])
ljust=: (|.~ +/@(*./\)@(' '&=))"1
rjust=: (|.~ -@(+/)@(*./\.)@(' '&=))"1
ss=: I. @ E.
dropto=: ] cuts 2
dropafter=: ] cuts 1
taketo=: ] cuts _1
takeafter=: ] cuts _2
charsub=: 4 : 0
'f t'=. |: _2 ]\ x
l=. f i."1 0 y
x=. l { t,'?'
c=. l = #f
c } x ,: y
)
chopstring=: 3 : 0
(' ';'"') chopstring y
:
dat=. y
'fd sd'=. 2{. boxopen x
assert. 1 = #fd
if. #sd do.
  sd=. ~.sd
  if. 1 < #sd do.
    s=. {. '|`' -. fd
    dat=. dat charsub~ ,sd,.s
    sd=. s
  end.
  dat=. dat,fd
  b=. dat = fd
  c=. dat = {.sd
  d=. ~:/\ c
  fmsk=. b > d
  smsk=. (> (0 , }:)) c
  smsk=. -. smsk +. c *. 1|.fmsk
  y=. smsk#y,fd
  fmsk=. 0:^:(,@1: -: ]) smsk#fmsk
  fmsk <;._2 y
else.
  <;._2 dat,fd
end.
)
dltbs=: LF&$: : (4 : 0)
txt=. ({.x), y
a=. txt ~: ' '
b=. (a # txt) e. x
c=. b +. }. b, 1
d=. ~: /\ a #^:_1 c ~: }: 0, c
}. (a >: d) # txt
)
dquote=: ('"'&,@(,&'"'))@ (#~ >:@(=&'"'))
dquotex=: 3 : 0
s=. y#~ >: m=. (=&'"') y
p=. (i.#y)#~>: m
('"'&,@(,&'"')) '\' (p i.(I.m))}s
)
dtbs=: 3 : 0
CRLF dtbs y
:
txt=. y , {.x
blk=. txt ~: ' '
ndx=. +/\ blk
b=. blk < }. (txt e. x), 0
msk=. blk >: ndx e. b # ndx
}: msk # txt
)
rplc=: stringreplace~
rplci=: stringreplacei~
fstringreplace=: 4 : 0
nf=. 'no match found'
y=. fboxname y
try. size=. 1!:4 y catch. nf return. end.
if. size=0 do. nf return. end.
old=. freads y
new=. x stringreplace old
if. old -: new do. nf return. end.
new fwrites y
cnt=. +/ (0 pick x) E. old
(":cnt),' replacement',((1~:cnt)#'s'),' made'
)
quote=: (''''&,@(,&''''))@ (#~ >:@(=&''''))
nos=. i.@#@] e. #@[ ({~^:a:&0@(,&_1)@(]I.+) { _1,~]) I.@E.
splitnostring=: #@[ }.each [ (nos f. <;.1 ]) ,
splitstring=: #@[ }.each [ (E. <;.1 ]) ,
stringreplace=: 4 : 0

txt=. ,y
t=. _2 [\ ,x
old=. {."1 t
new=. {:"1 t
oldlen=. # &> old
newlen=. # &> new

if. *./ 1 = oldlen do.

  hit=. (;old) i. txt
  ndx=. I. hit < #old

  if. 0 e. $ndx do. txt return. end.

  cnt=. 1
  exp=. hit { newlen,1
  hnx=. ndx { hit
  bgn=. ndx + +/\ 0, (}: hnx) { newlen - 1

else.

  hit=. old I. @ E. each <txt
  cnt=. # &> hit

  if. 0 = +/ cnt do. txt return. end.

  bgn=. set=. ''

  pick=. > @ {
  diff=. }. - }:

  for_i. I. 0 < cnt do.
    ln=. i pick oldlen
    cx=. (i pick hit) -. set, ,bgn -/ i.ln
    while. 0 e. b=. 1, <:/\ ln <: diff cx do. cx=. b#cx end.
    hit=. (<cx) i} hit
    bgn=. bgn, cx
    set=. set, ,cx +/ i.ln
  end.

  cnt=. # &> hit
  msk=. 0 < cnt
  exp=. (#txt) $ 1
  del=. newlen - oldlen

  if. #add=. I. msk *. del > 0 do.
    exp=. (>: (add{cnt) # add{del) (;add{hit) } exp
  end.

  if. #sub=. I. msk *. del < 0 do.
    sbx=. ; (;sub{hit) + each (sub{cnt) # i. each sub{del
    exp=. 0 sbx } exp
  end.

  hit=. ; hit
  ind=. /: (#hit) $ 1 2 3
  hnx=. (/: ind { hit) { ind
  bgn=. (hnx { hit) + +/\ 0, }: hnx { cnt # del

end.

ind=. ; bgn + each hnx { cnt # i.each newlen
rep=. ; hnx { cnt # new
rep ind} exp # txt
)
stringreplacei=: 4 : 0

txt=. ,y
t=. _2 [\ ,x
old=. {."1 t
new=. {:"1 t
oldlen=. # &> old
newlen=. # &> new

if. *./ 1 = oldlen do.

  hit=. (;old) i.&tolower txt
  ndx=. I. hit < #old

  if. 0 e. $ndx do. txt return. end.

  cnt=. 1
  exp=. hit { newlen,1
  hnx=. ndx { hit
  bgn=. ndx + +/\ 0, (}: hnx) { newlen - 1

else.

  hit=. old (I. @ E.)&tolower each <txt
  cnt=. # &> hit

  if. 0 = +/ cnt do. txt return. end.

  bgn=. set=. ''

  pick=. > @ {
  diff=. }. - }:

  for_i. I. 0 < cnt do.
    ln=. i pick oldlen
    cx=. (i pick hit) -. set, ,bgn -/ i.ln
    while. 0 e. b=. 1, <:/\ ln <: diff cx do. cx=. b#cx end.
    hit=. (<cx) i} hit
    bgn=. bgn, cx
    set=. set, ,cx +/ i.ln
  end.

  cnt=. # &> hit
  msk=. 0 < cnt
  exp=. (#txt) $ 1
  del=. newlen - oldlen

  if. #add=. I. msk *. del > 0 do.
    exp=. (>: (add{cnt) # add{del) (;add{hit) } exp
  end.

  if. #sub=. I. msk *. del < 0 do.
    sbx=. ; (;sub{hit) + each (sub{cnt) # i. each sub{del
    exp=. 0 sbx } exp
  end.

  hit=. ; hit
  ind=. /: (#hit) $ 1 2 3
  hnx=. (/: ind { hit) { ind
  bgn=. (hnx { hit) + +/\ 0, }: hnx { cnt # del

end.

ind=. ; bgn + each hnx { cnt # i.each newlen
rep=. ; hnx { cnt # new
rep ind} exp # txt
)
undquote=: (#~ -.@('""'&E.))@}:@}.^:(('"' = {.) *. '"' = {:)
cutpara=: 3 : 0
txt=. topara y
txt=. txt,LF -. {:txt
b=. (}.b,0) < b=. txt=LF
b <;._2 txt
)
foldtext=: 4 : 0
if. 0 e. $y do. '' return. end.
y=. ; x&foldpara each cutpara y
y }.~ - (LF ~: |.y) i. 1
)
foldpara=: 4 : 0
if. 0=#y do. LF return. end.
r=. ''
x1=. >: x
txt=. y
while.
  ind=. ' ' i.~ |. x1{.txt
  s=. txt {.~ ndx=. x1 - >: x1 | ind
  s=. (+./\.s ~: ' ') # s
  r=. r, s, LF
  #txt=. (ndx + ind<x1) }. txt
do. end.
r
)
topara=: 3 : 0
if. 0=#y do. '' return. end.
b=. y=LF
c=. b +. y=' '
b=. b > (1,}:b) +. }.c,0
' ' (I. b) } y
)
18!:4 <'z'
3 : 0''
if. IFIOS do.
  r=. 'Engine: ',9!:14''
  r=. r,LF,'Library: ',JLIB
  r=. r,LF,'J/iOS Version: ',VERSION
  r=. r,LF,'Platform: ',UNAME,' ',IF64 pick '32';'64'
  r=. r,LF,'InstallPath: ', (2!:5'HOME'), '/Documents/j'
  JVERSION=: toJ r
end.
EMPTY
)
NB. break
NB.%break.ijs - break utilities
NB.-This script defines break utilities and is included in the J standard library.
NB.-Definitions are loaded into the z locale.
NB.-
NB.-`setbreak 'default'` is done by profile for Jqt. JHS and jconsole can use ctrl+c. 
NB.-
NB.-setbreak creates file `~break/Pid.Class` and writes 0 to the first byte.
NB.-
NB.-Pid is the process id and Class is normally 'default'.
NB.-
NB.-setbreak calls 9!:47 with this file.
NB.-
NB.-9!:47 maps the first byte of file, and JE tests this byte for break requests.
NB.-
NB.-Another task writes 1 or 2 to the file for attention/break.
NB.-
NB.-9!:46 returns the filename.
NB.-
NB.-`break 'abc'` sets break for JEs with class abc.
NB.-
NB.-JEs with the same class all get the break. A non-default class protects JE from the default break.
NB.-
NB.- A new setbreak replaces the old.
NB.-
NB.-`break 0'` shows breakhelp

cocurrent'z'

NB. =========================================================
NB.*break v break J execution
NB. y is class to signal - '' treated as 'default'
break=: 3 : 0
if. y-:0 do. breakhelp_j_ return. end.
breakclean_j_''
p=. jpath'~break/'
fs=. ((<p),each{."1[1!:0 p,'*')-.<9!:46''
pc=. (>:;fs i:each'/')}.each fs
i=. ;pc i.each'.'
pids=. _1".each i{.each pc
classes=. (>:i)}.each pc
if. y-:1 do. /:~(>":each pids),.>' ',each classes return. end.
'no task to break'assert #fs
if. (0=#y)+.2=3!:0 y do.
  b=. classes= (0=#y){y;'default'
  'bad class'assert +/b
  fs=. (<p),each (":each b#pids),each '.',each b#classes
else.
  i=. pids i.<y
  'bad pid'assert i~:#pids
  fs=. <p,(":;i{pids),'.',;i{classes
end.
for_f. fs do.
  v=. 2<.>:a.i.1!:11 f,<0 1
  (v{a.) 1!:12 f,<0
end.
i.0 0
)

NB. =========================================================
NB.*setbreak v set break file
NB.-Set break
NB. y is class
NB. Creates unique file ~break/Pid.Class
setbreak=: 3 : 0
if. (-.IFQT)*.y-:'default' do. i.0 0 return. end. NB. only for qt and not default
try.
  assert #y
  q=. jpath '~break/'
  1!:5 ::] <q
  f=. q,(":2!:6''),'.',y
  ({.a.) 1!:12 f;0
  9!:47 f
  breakclean_j_''
  f
catch. 13!:12'' end.
)

NB. =========================================================
NB.*breakclean v erase orphan break files
breakclean_j_=: 3 : 0
q=. jpath '~break/'
fs=. ((<q),each{."1[1!:0 q,'*')-.9!:46''
if. UNAME-:'Win' do.
  ferase fs NB. windows erase has not effect while file is in use
else.
  d=. dltb each}.<;._2 spawn_jtask_'ps -e'
  allpids=. ;0".each (d i.each ' '){.each d
  pc=. (>:;fs i:each'/')}.each fs
  pids=. ;_1".each (;pc i.each'.'){.each pc
  ferase (-.pids e. allpids)#fs
end.
)

NB. =========================================================
NB.*breakhelp n break help
breakhelp_j_=: 0 : 0
   break 0     NB. help
   break 1     NB. list other ~break pids and classes
   break ''    NB. break to all default class tasks
   break '...' NB. break to all ... class tasks
   break pid   NB. break to that pid

1st break stops execution at line start
2nd break stops execution mid-line, 6!:3 , socket select

profile does setbreak'default' for Jqt
profile does not do it for jconsole or JHS (use ctrl+c)

   setbreak'abc' NB. set break file for this pid and class abc

https://code.jsoftware.com/wiki/Standard_Library/break
)

cocurrent <'j'
Alpha=: a. {~ , (a.i.'Aa') +/ i.26
Num=: a. {~ (a.i.'0') + i.10
AlphaNum=: Alpha,Num
Boxes=: ((16+i.11) { a.),:'+++++++++|-'
ScriptExt=: '.ijs'
ProjExt=: '.jproj'

extnone=: {.~ i:&'.'
extproj=: , (ProjExt #~ '.'&e. < 0 < #)
extsrc=: , ('.ijs' #~ '.'&e. < 0 < #)

addfname=: , ('/' ~: {:) # i:&'/' }. ]
boxdraw=: 3 : '9!:7 y { Boxes'
hostcmd=: [: 2!:0 '(' , ,&' || true)'
fpath=: [: }: +./\.@:=&'/' # ]
isURL=: 1 e. '://'&E.
maxrecent=: 3 : '(RecentMax <. #r) {. r=. ~.y'
pack=: [: (,. ".&.>) ;: ::]
pdef=: 3 : '0 0$({."1 y)=: {:"1 y'
seldir=: #~ '-d'&-:"1 @ (1 4&{"1) @ > @ (4&{"1)
spath=: #~ [: *./\. '/'&~:
termLF=: , (0 < #) # LF -. {:
termsep=: , (0 < #) # '/' -. {:
remsep=: }.~ [: - '/' = {:

path2proj=: ,'/',ProjExt ,~ spath
win2lower=: 3 : 0
if. 0=#CasePaths_j_ do. tolower y return. end.
p=. jpathsep y
n=. 1 + p i. ':'
d=. n {. p
if. (<d) e. CasePaths_j_ do. y return. end.
b=. n }. p
p=. d,(('/'~:{.b)#'/'), b,'/'
p=. (1 + p i: '/') {. p
p=. (I.p='/') {.each <p
if. 1 e. p e. CasePaths_j_ do. y else. tolower y end.
)
3 : 0''
if. UNAME-:'Darwin' do.
  filecase=: tolower
  isroot=: '/' = {.
elseif. IFUNIX do.
  filecase=: ]
  isroot=: '/' = {.
elseif. do.
  filecase=: win2lower
  isroot=: ('\\' -: 2&{.) +. ('//' -: 2&{.) +. (':' = {.@}.)
end.
0
)
dirtreex=: 3 : 0
'' dirtreex y
:
y=. jpath y
p=. (+./\. y = '/') # y
d=. 1!:0 y,('/' = {:y) # '*'
if. 0 = #d do. '' return. end.
a=. > 4 {"1 d
m=. 'd' = 4 {"1 a
f=. (<p) ,each {."1 d
if. 1 e. m do.
  f=. f, ; dirtreex each (m#f) ,each <'/','*'
end.
if. #x do.
  f #~ (1 e. x E. ])&> f
end.
)
getfolderdefs=: 3 : 0
p=. (, '/' , ProjExt ,~ spath) each subdirtree y
t=. p #~ #@(1!:0)&> p
t;<fpath each (1+#y) }. each (-#ProjExt) }. each t
)
isconfigfile=: 3 : 0
'p f'=. fpathname y
x=. f i: '.'
(p -: jpath '~config/') *. '.cfg'-:x}.f
)
isdir=: 3 : 0
d=. 1!:0 fboxname y
if. 1 ~: #d do. 0 return. end.
'd' = 4 { 4 pick ,d
)
isfile=: 3 : 0
d=. 1!:0 fboxname y
if. 1 ~: #d do. 0 return. end.
'd' ~: 4 { 4 pick ,d
)
istempname=: 3 : 0
x=. y i: '.'
*./ ('.ijs'-:x}.y),(x{.y) e. Num
)
istempscript=: 3 : 0
'p f'=. fpathname y
(p -: jpath '~temp/') *. istempname f
)
jshowconsole=: 3 : 0
if. -.IFWIN do. 'only supported in windows' return. end.
t=. {.>'kernel32.dll GetConsoleWindow x'cd''
'user32.dll ShowWindow n x i'cd t;(0-:y){5 0
i.0 0
)
mkdir=: 3 : 0
a=. termsep jpath y
if. #1!:0 }:a do. 1 return. end.
for_n. I. a='/' do.
  1!:5 :: 0: < n{.a
end.
)
newtempscript=: 3 : 0
x=. ScriptExt
p=. jpath '~temp/'
d=. 1!:0 p,'*',x
a=. (-#x) }. each {."1 d
a=. a #~ (*./ .e.&'0123456789') &> a
a=. 0, {.@:(0&".) &> a
p, x ,~ ": {. (i. >: #a) -. a
)
nounrep=: 2 }. [: ; [: nounrep1 each ;:
nounrep1=: LF2 , ] , '=: ' , [: nounrep2 ".
nounrep2=: 3 : 0
if. 0 = #y do. '''''' return. end.
select. 3!:0 y
fcase. 32 do.
  y=. ; y ,each LF
case. 2 do.
  if. LF e. y do.
    y=. y, LF -. {:y
    '0 : 0', LF, ; <;.2 y,')'
  else.
    quote y
  end.
case. do.
  ": y
end.
)
octal=: 3 : 0
t=. ,y
if. LF e. t do.
  t=. octal each <;._2 t,LF
  }: ; t ,each LF return.
end.
u=. isutf8 t
x=. a. i. t
m=. (x e. 9 13) < x < 32
if. u > 1 e. m do. t return. end.
n=. I. m=. m +. u < x > 126
s=. '\',.}.1 ": 8 (#.^:_1) 255,n{x
s ((n+3*i.#n)+/i.4)} (>:3*m)#t
)
revinfo=: 3 : 0
v=. 9!:14''
if. '.' e. (v i. '/') {. v do.
  res=. 8 {. <;._1 '/',v
  a=. 0 pick res
  ndx=. a i. '-'
  beta=. {. 0 ". (ndx+5) }. a
  vno=. 100 #. (0 ".&> <;._1 '.' 0} ndx {. a), beta
  vno;res
else.
  res=. 9 {. <;._1 '/',v
  'a b'=. 0 3 { res
  res=. (<<<0 3) {res
  res=. (('www.jsoftware.com' -: 3 pick res){'na';'GPL3') 2} res
  'm n'=. ": each ver=. 0 100 #: 0 ". }. a
  num=. _97 + a.i. {:b
  if. 'r' = {. b do.
    rev=. (num+1),0
    vst=. 'j',m,'.',n,'.',":num+1
  else.
    rev=. 0,num
    vst=. 'j',m,'.',n,'.0-beta',":num
  end.
  (100 #.ver,rev);vst;res
end.
)
rmdir=: 3 : 0
r=. 1;'not a directory: ',":y
if. 0=#y do. r return. end.
d=. 1!:0 y
if. 1 ~: #d do. r return. end.
if. 'd' ~: 4 { 4 pick {. d do. r return. end.
if. IFWIN do.
  shell_jtask_ 'rmdir "',y,'" /S /Q'
else.
  hostcmd_j_ 'rm -rf ',((UNAME-:'Linux')#'--preserve-root '),y
end.
(#1!:0 y);''
)
runimmx0_j_=: 3 : 0
IMMX_j_=: utf8 y
9!:27 '0!:100 IMMX_j_'
9!:29 [ 1
)
runimmx1_j_=: 3 : 0
IMMX_j_=: utf8 y
9!:27 '0!:101 IMMX_j_'
9!:29 [ 1
)
scripts=: 3 : 0
if. 0=#y do.
  list 0{"1 Public
elseif. 'v'e.y do.
  dir=. Public
  a=. >0{"1 dir
  b=. >1{"1 dir
  a /:~ a,.' ',.b
elseif. 1 do.
  'invalid argument to scripts: ',,":y
end.
)
setfolder=: 3 : 0
if. 0=#y do.
  Folder=: FolderTree=: FolderIds=: '' return.
end.
assert. (<y) e. {."1 UserFolders
Folder=: y
'FolderTree FolderIds'=: getfolderdefs jpath '~',y
if. 3=nc <'snapshot_tree_jp_' do.
  snapshot_tree_jp_ FolderTree
end.
EMPTY
)
subdirtree=: 3 : 0
if. 0=#1!:0 y do. '' return. end.
r=. ''
dir=. <y,'/'
while. #dir do.
  fpath=. (>{.dir) &,
  dir=. }.dir
  dat=. seldir 1!:0 fpath '*'
  if. #dat do.
    dat=. fpath each {."1 dat
    r=. r,dat
    dir=. (dat ,each '/'),dir
  end.
end.
sort filecase each r
)
unixshell=: 3 : 0
f=. jpath '~temp/shell.sh'
t=. jpath '~temp/shell.txt'
e=. jpath '~temp/shell.err'
('#!/bin/sh',LF,y,LF) fwrite f
'rwx------' 1!:7 <f
hostcmd '"',f,'" > "',t,'" 2> "',e,'"'
r=. (fread t);fread e
ferase f;t;e
r
)
unixshellx=: 3 : 0
'res err'=. unixshell y
if. #err do.
  smoutput 'Shell command error: ',LF,LF,err
end.
res
)
browseref=: 3 : 0
htmlhelp 'dictionary/',y
)
dquote=: 3 : 0
if. '"' = {.y do. y else. '"',y,'"' end.
)
browse=: 3 : 0
if. -. isURL cmd=. dltb y do.
  if. -.fexist cmd do. EMPTY return. end.
end.
if. IFJHS do.
  redirecturl_jijxm_=: file2url cmd
  EMPTY return.
elseif. IFIOS do.
  jh '<a href="',(file2url cmd),'"</a>'
  EMPTY return.
end.
browser=. Browser_j_
select. UNAME
case. 'Win' do.
  ShellExecute=. 'shell32 ShellExecuteW > i x *w *w *w *w i'&cd
  SW_SHOWNORMAL=. 1
  NULL=. <0
  if. 0 = #browser do.
    r=. ShellExecute 0;(uucp 'open');(uucp winpathsep cmd);NULL;NULL;SW_SHOWNORMAL
  else.
    r=. ShellExecute 0;(uucp 'open');(uucp winpathsep browser);(uucp dquote winpathsep cmd);NULL;SW_SHOWNORMAL
  end.
  if. r<33 do. sminfo 'browse error:',browser,' ',cmd,LF2,1{::cderx'' end.
case. 'Android' do.
  android_exec_host 'android.intent.action.VIEW';(utf8 ('file://'&,)@abspath^:(-.@isURL) cmd);'text/html';16b0004000
case. do.
  if. 0 = #browser do.
    browser=. dfltbrowser''
  end.
  browser=. dquote (browser;Browser_nox_j_){::~ nox=. ((<UNAME)e.'Linux';'OpenBSD';'FreeBSD') *. (0;'') e.~ <2!:5 'DISPLAY'
  cmd=. browser,' ',dquote cmd
  try.
    2!:1 cmd, (0=nox)#' >/dev/null 2>&1 &'
  catch.
    msg=. 'Could not run the browser with the command:',LF2
    msg=. msg, cmd,LF2
    if. IFQT do.
      msg=. msg, 'You can change the browser definition in Edit|Configure|Base',LF2
    end.
    sminfo 'Run Browser';msg
  end.
end.
EMPTY
)
dfltbrowser=: verb define
select. UNAME
case. 'Android' do. ''
case. 'Win' do. ''
case. 'Darwin' do. 'open'
case. do.
  try.
    2!:0'which x-www-browser 2>/dev/null'
    'x-www-browser' return. catch. end.
  try.
    2!:0'which google-chrome 2>/dev/null'
    'google-chrome' return. catch. end.
  try.
    2!:0'which chromium 2>/dev/null'
    'chromium' return. catch. end.
  try.
    2!:0'which chromium-browser 2>/dev/null'
    'chromium-browser' return. catch. end.
  try.
    2!:0'which firefox 2>/dev/null'
    'firefox' return. catch. end.
  try.
    2!:0'which konqueror 2>/dev/null'
    'konqueror' return. catch. end.
  try.
    2!:0'which opera 2>/dev/null'
    'opera' return. catch. end.
  '' return.
end.
)
viewpdf=: 3 : 0
if. -. isURL cmd=. dltb y do.
  if. -.fexist cmd do. EMPTY return. end.
end.
if. IFJHS do.
  redirecturl_jijxm_=: file2url cmd
  EMPTY return.
elseif. IFIOS do.
  jh '<a href="',(file2url cmd),'"</a>'
  EMPTY return.
end.
PDFReader=. PDFReader_j_
select. UNAME
case. 'Win' do.
  ShellExecute=. 'shell32 ShellExecuteW > i x *w *w *w *w i'&cd
  SW_SHOWNORMAL=. 1
  NULL=. <0
  if. 0 = #PDFReader do.
    r=. ShellExecute 0;(uucp 'open');(uucp winpathsep cmd);NULL;NULL;SW_SHOWNORMAL
  else.
    r=. ShellExecute 0;(uucp 'open');(uucp winpathsep PDFReader);(uucp dquote winpathsep cmd);NULL;SW_SHOWNORMAL
  end.
  if. r<33 do. sminfo 'view pdf error:',PDFReader,' ',cmd,LF2,1{::cderx'' end.
case. 'Android' do.
  android_exec_host 'android.intent.action.VIEW';(utf8 ('file://'&,)@abspath^:(-.@isURL) cmd);'application/pdf';0
case. do.
  PDFReader=. dquote PDFReader
  cmd=. PDFReader,' ',(dquote cmd),' &'
  try.
    2!:1 cmd
  catch.
    msg=. 'Could not run the PDFReader with the command:',LF2
    msg=. msg, cmd,LF2
    if. IFQT do.
      msg=. msg, 'You can change the PDFReader definition in Edit|Configure|Base',LF2
    end.
    sminfo 'Run PDFReader';msg
  end.
end.
EMPTY
)

linux_pdfreader=: <;._2]0 :0
xdg-open
evince
kpdf
xpdf
okular
acroread
)
dfltpdfreader=: verb define
select. UNAME
case. 'Win' do. ''
case. 'Android' do. ''
case. 'Darwin' do. 'open'
case. do.
  nox=. ((<UNAME)e.'Linux';'OpenBSD';'FreeBSD') *. (0;'') e.~ <2!:5 'DISPLAY'
  if. (((<UNAME)e.'Linux';'OpenBSD';'FreeBSD') > nox) *. ''-: te=. nox{::PDFReader_j_;PDFReader_nox_j_ do.
    for_t. linux_pdfreader do.
      try. 2!:0'which ',(>t),' 2>/dev/null'
        te=. >t
        break.
      catch. end.
    end.
  end.
  te
end.
)
Folder=: ''
FolderTree=: FolderIds=: 0

Cwh=: 79 24
jpath=: 3 : 0
nam=. jpathsep y
if. '~' ~: {. nam do. return. end.
fld=. SystemFolders, UserFolders
ind=. nam i. '/'
tag=. }. ind {. nam
if. 0=#tag do.
  tag=. 'home'
  nam=. '~home',}.nam
  ind=. nam i. '/'
end.
if. tag -: 'addons' do.
  bal=. 8 }. nam
end.
par=. '.' = {. tag
if. par do.
  len=. ('.' = tag) i. 0
  tag=. len }. tag
end.
ndx=. ({."1 fld) i. <tag
if. ndx < # fld do.
  bal=. ind }. nam
  pfx=. 1 pick ndx { fld
  if. '~' = {.pfx do.
    pfx=. jpath pfx
  end.
  if. par do.
    pfx=. ((#pfx) | (+/\. pfx='/') i: len) {. pfx
  end.
  nam=. pfx,bal
end.
nam
)
tofoldername=: 3 : 0
if. 0=#y do. '' return. end.
folders=. UserFolders,SystemFolders
pds=. {."1 folders
pps=. termsep each {:"1 folders
ndx=. \: # &> pps
pds=. ndx{pds
pps=. ndx{pps
res=. filecase each boxxopen y
len=. # &> pps
for_i. i.#res do.
  nam=. i pick res
  if. '~' = {. nam,'~' do. continue. end.
  msk=. pps = len {. each <nam,'/'
  if. 1 e. msk do.
    ndx=. ((i. >./) msk # len) { I. msk
    nam=. ('~', > ndx { pds),(<: ndx { len) }. nam
    res=. (<nam) i } res
  end.
end.
pps=. }: each pps
ndx=. 1 + pps i: &> '/'
msk=. ndx < len
pps=. msk # ndx {.each pps
pds=. msk # pds
len=. # &> pps
for_i. i.#res do.
  nam=. i pick res
  if. '~' = {. nam,'~' do. continue. end.
  if. '/' = {. nam do. continue. end.
  msk=. pps = len {. each <nam
  if. 1 e. msk do.
    ndx=. ((i. >./) msk # len) { I. msk
    nam=. ('~.', > ndx { pds),(<: ndx { len) }. nam
    res=. (<nam) i } res
  end.
end.
if. L. y do. res else. >res end.
)
Loaded=: ''
Public=: i. 0 2
UserFolders=: i. 0 2
getignore=: 3 : 0
r=. ' colib compare convert coutil dates dir dll files libpath strings text gl2 graphics/gl2'
Ignore=: <;._1 r
)

getignore''
buildpublic=: 3 : 0
dat=. deb toJ y
dat=. a: -.~ <;._2 dat, LF
ndx=. dat i. &> ' '
short=. ndx {.each dat
long=. ndx }. each dat
long=. extsrc@jpathsep@deb each long
msk=. (<'system','/') = 7 {. each long
long=. (msk{'';'~') ,each long
msk=. (i. ~.) {."1 Public=: Public,~ short,.long
Public=: sort msk{Public
empty''
)
cutnames=: 3 : 0
if. LF e. y do.
  txt=. y, LF
  nms=. (txt = LF) <;._2 txt
else.
  txt=. y, ' '
  msk=. txt = '"'
  com=. (txt = ' ') > ~: /\ msk
  msk=. (msk *. ~:/\msk) < msk <: 1 |. msk
  nms=. (msk # com) <;._2 msk # txt
end.
nms -. a:
)
3 : 0''
if. 0=9!:24'' do.
  exist=: fexist
else.
  exist=: 0:
end.
1
)
fullname=: 3 : 0
p=. '/'
d=. jpath y
if. </ d i. ':',p do.
elseif. (2{.d) -: 2#p do.
elseif. p ~: 1{.d do.
  jcwdpath d
elseif. IFWIN do.
  (2{.jcwdpath''),d
end.
)
getscripts=: 3 : 0
if. 0=#y do. '' return. end.
if. 0=L.y do.
  if. isfile y do.
    fullname each fboxname y return.
  end.
  y=. cutnames y
end.
y=. y -. Ignore
if. 0=#y do. '' return. end.
ndx=. ({."1 Public) i. y
ind=. I. ndx < # Public
y=. ((ind { ndx) { 1 {"1 Public) ind } y
ind=. (I.-.isroot&>y) -. ind
if. #ind do.
  bal=. jpath each ind { y
  msk=. (isfile &> bal) +. '/'={:&> bal
  y=. (msk#bal) (msk#ind) } y
  ind=. (-.msk)#ind
  if. #ind do.
    bal=. (-.msk)#bal
    msk=. -. '.'&e.@(}.~i:&'/') &> bal
    msk=. msk *. *./@:((a.{~, 65 97 +/i.26)e.~])@:({.~i.&'/') &> bal
    msk=. msk > isroot &> bal
    cnt=. ('/' +/ .= ]) &> bal
    ndx=. I. msk *. cnt=1
    bal=. (addfname each ndx { bal) ndx } bal
    ndx=. I. msk *. cnt > 0
    bal=. (<jpath '~addons/') ,each (ndx{bal) ,each <'.ijs'
    y=. bal (ndx{ind) } y
  end.
end.
fullname each y
)
getpath=: ([: +./\. =&'/') # ]
xedit=: 0&$: : (4 : 0)
'file row'=. 2{.(boxopen y),<0
file=. dltb file
if. -.fexist file do. EMPTY return. end.
if. IFJHS do.
  xmr ::0: file
  EMPTY return.
end.
if. UNAME-:'Android' do.
  if. IFJA do.
    android_exec_host 'android.intent.action.VIEW';(utf8 ('file://'&,)@abspath^:(-.@isURL) file);'text/plain';0
  elseif. 1=ftype '/system/bin/vi' do.
    2!:1 '/system/bin/vi', ' ', (dquote >@fboxname file)
  elseif. 1=ftype '/system/xbin/vi' do.
    2!:1 '/system/xbin/vi', ' ', (dquote >@fboxname file)
  elseif. #Editor_j_ do.
    2!:1 Editor_j_, ' ', (dquote >@fboxname file)
  end.
  EMPTY return.
end.
editor=. (Editor_j_;Editor_nox_j_){::~ nox=. ((<UNAME)e.'Linux';'OpenBSD';'FreeBSD') *. (0;'') e.~ <2!:5 'DISPLAY'
if. 0=#editor do. EMPTY return. end.
nox=. ((<UNAME)e.'Linux';'OpenBSD';'FreeBSD') *. (0;'') e.~ <2!:5 'DISPLAY'
if. ((<UNAME)e.'Linux';'OpenBSD';'FreeBSD')>nox do.
  if. 1 e. r=. 'term' E. editor do.
    if. '-e ' -: 3{. editor=. dlb (}.~ i.&' ') ({.I.r)}.editor do.
      editor=. TermEmu, (('gnome-terminal'-:TermEmu){::' -e ';' -- '), dlb 3}.editor
    else.
      editor=. TermEmu, ' ', editor
    end.
  else.
    editor=. TermEmu, (('gnome-terminal'-:TermEmu){::' -e ';' -- '), editor
  end.
end.
if. 1 e. '%f' E. editor do.
  cmd=. editor stringreplace~ '%f';(dquote >@fboxname file);'%l';(":>:row)
else.
  cmd=. editor, ' ', (dquote >@fboxname file)
end.
try.
  if. IFUNIX do.
    if. x do.
      2!:1 cmd
    else.
      2!:1 cmd, (0=nox)#' 1>/dev/null &'
    end.
  else.
    (x{0 _1) fork_jtask_ cmd
  end.
catch.
  msg=. '|Could not run the editor:',cmd,LF
  msg=. msg,'|You can change the Editor definition in Edit|Configure|Base'
  smoutput msg
end.
EMPTY
)

linux_terminal=: <;._2]0 :0
x-terminal-emulator
gnome-terminal
mate-terminal
konsole
urxvt
rxvt
lxterminal
xfce4-terminal
eterm
terminator
terminology
st
xterm
)
dflttermemu=: verb define
nox=. ((<UNAME)e.'Linux';'OpenBSD';'FreeBSD') *. (0;'') e.~ <2!:5 'DISPLAY'
if. (((<UNAME)e.'Linux';'OpenBSD';'FreeBSD') > nox) *. ''-: te=. nox{::TermEmu_j_;TermEmu_nox_j_ do.
  for_t. linux_terminal do.
    try. 2!:0'which ',(>t),' 2>/dev/null'
      te=. >t
      break.
    catch. end.
  end.
end.
te
)
viewimage=: 3 : 0
if. -. isURL cmd=. dltb y do.
  if. -.fexist cmd do. EMPTY return. end.
end.
if. IFJHS do.
  redirecturl_jijxm_=: file2url cmd
  EMPTY return.
elseif. IFIOS do.
  jh '<a href="',(file2url cmd),'"</a>'
  EMPTY return.
end.
nox=. ((<UNAME)e.'Linux';'OpenBSD';'FreeBSD') *. (0;'') e.~ <2!:5 'DISPLAY'
ImageViewer=. nox{::ImageViewer_j_;ImageViewer_nox_j_
select. UNAME
case. 'Win' do.
  ShellExecute=. 'shell32 ShellExecuteW > i x *w *w *w *w i'&cd
  SW_SHOWNORMAL=. 1
  NULL=. <0
  r=. ShellExecute 0;NULL;(uucp winpathsep cmd);NULL;NULL;SW_SHOWNORMAL
  if. r<33 do. sminfo 'view image error: ',cmd,LF2,1{::cderx'' end.
case. 'Android' do.
  android_exec_host 'android.intent.action.VIEW';(utf8 ('file://'&,)@abspath^:(-.@isURL) cmd);'image/*';0
case. do.
  if. 0 = #ImageViewer do.
    ImageViewer=. dfltimageviewer''
  end.
  if. 0 = #ImageViewer do.
    browser=. Browser_j_
    if. 0 = #browser do.
      browser=. dfltbrowser''
    end.
    browser=. dquote (browser;Browser_nox_j_){::~ nox=. ((<UNAME)e.'Linux';'OpenBSD';'FreeBSD') *. (0;'') e.~ <2!:5 'DISPLAY'
  else.
    browser=. dquote ImageViewer
  end.
  cmd=. browser,' ',dquote cmd
  try.
    2!:1 cmd, (0=nox)#' >/dev/null 2>&1 &'
  catch.
    msg=. 'Could not run the image viewer with the command:',LF2
    msg=. msg, cmd,LF2
    if. IFQT do.
      msg=. msg, 'You can change the imageviewer definition in Edit|Configure|Base',LF2
    end.
    sminfo 'Run image viewer';msg
  end.
end.
EMPTY
)
dfltimageviewer=: verb define
select. UNAME
case. 'Win' do. ''
case. 'Android' do. ''
case. 'Darwin' do. 'open'
case. do.
  try.
    2!:0'which xdg-open 2>/dev/null'
    'xdg-open' return. catch. end.
  try.
    2!:0'which eog 2>/dev/null'
    'eog' return. catch. end.
  '' return.
end.
)
cocurrent 'z'
jpath=: jpath_j_
load=: 3 : 0
0 load y
:
fls=. getscripts_j_ y
fn=. ('script',x#'d')~
for_fl. fls do.
  if. Displayload_j_ do. smoutput > fl end.
  if. -. fexist fl do.
    smoutput 'not found: ',>fl
  end.
  fn fl
  Loaded_j_=: ~. Loaded_j_,fl
end.
empty''
)
loadd=: 1&load
require=: 3 : 0
fls=. Loaded_j_ -.~ getscripts_j_ y
if. # fls do. load fls else. empty'' end.
)
scripts=: scripts_j_
show=: 3 : 0
y=. y,(0=#y)#0 1 2 3
if. (3!:0 y) e. 2 32 do. y=. cutopen y
else. y=. (4!:1 y) -. (,'y');,'y.' end.
wid=. {.wcsize''
sub=. '.'&(I. @(e.&(9 10 12 13 127 254 255{a.))@]})
den=: {{ try. 0 $.y [ 3 $. y catch. y end. }}
j=. '((1<#$t)#(":$t),''$''),":,t'
j=. 'if. L. t=. den ".y do. 5!:5 <y return. end.';j
j=. 'if. 0~:4!:0 <y do. 5!:5 <y return. end.';j
a=. (,&'=: ',sub @ (3 : j)) each y
}: ; ((wid <. #&> a) {.each a) ,each LF
)
xedit=: xedit_j_
wcsize=: 3 : 0
if. (IFQT+.IFJNET+.IFJHS+.IFIOS+.UNAME-:'Android') < IFUNIX do.
  |.@".@(-.&LF)@(2!:0) :: (Cwh_j_"_) '/bin/stty size 2>/dev/null'
else.
  (Cwh_j_"_)`((0 ". wd) :: (Cwh_j_"_))@.IFQT 'sm get termcwh'
end.
)
coclass 'jcompare'

MAXPFX=: 100
MAXLCS=: *: MAXPFX
cin=: e. ,
fmt0=: 'p<0 [>q<] >' & (8!:0)
fmt1=: 'p<1 [>q<] >' & (8!:0)
lcs=: * * 1 + >./\@:(_1&|.)@:(>./\"1@:(_1&|."1))
mindx=: }.@{.@/:~@(+/"1 ,. ])
remltws=: 3 : 0
y=. y, LF
dat=. <;._2 y
msk=. CR = {: &> dat
dat=. (-msk) }. each dat
fn=. #~ ([: (+./\ *. +./\.) -.@(e.&(' ',TAB)))
dat=. fn each dat
dat=. dat ,each msk{'';CR
}: ; dat ,each LF
)
comp=: 4 : 0

sep=. ((LF cin x) +. LF cin y) { CRLF
if. 2=#$x do. x=. <@dtb"1 x
else. x=. <;._2 x,sep -. {:x end.
if. 2=#$y do. y=. <@dtb"1 y
else. y=. <;._2 y,sep -. {:y end.
if. x -: y do. 'no difference' return. end.
XY=: x,y
AX=: X=: XY i. x
AY=: Y=: XY i. y
NX=: i.#x
NY=: i.#y
SX=: SY=: ''
while. compend'' do. complcs'' end.
sx=. /:~ SX
sy=. /:~ SY
x=. (fmt0 sx) ,each (sx { AX) { XY
y=. (fmt1 sy) ,each (sy { AY) { XY
r=. (x,y) /: (sx,.0),sy,.1
}: ; r ,each LF
)
fcomp=: 4 : 0
'p j n'=. 3 {. x
'ifws ifsep'=. 2 2 #: j
'x y'=. _2 {. ,&p each cutopen y
if. L. n do.
  'nx ny'=. n
else.
  nx=. x [ ny=. y
end.
f=. 1!:1 :: _1:
tx=. f x=. fboxname x
if. tx -: _1 do. 'unable to read ',nx return. end.
ty=. f y=. fboxname y
if. ty -: _1 do. 'unable to read ',ny return. end.
tx=. f2utf8 tx
ty=. f2utf8 ty
if. ifsep do.
  tx=. toJ tx
  ty=. toJ ty
end.
if. ifws do.
  tx=. remltws tx
  ty=. remltws ty
end.
f=. _3&{.@('0'&,@(":@]))
mth=. _3[\'   JanFebMarAprMayJunJulAugSepOctNovDec'
'a m d h n s'=. 6{.1 pick dx=. ,1!:0 x
fx=. (4":d),' ',(m{mth),'  ::' 0 3 6 9};f &.> 100|a,h,n,s
'a m d h n s'=. 6{.1 pick dy=. ,1!:0 y
fy=. (4":d),' ',(m{mth),'  ::' 0 3 6 9};f &.> 100|a,h,n,s
'nx ny'=. <"1>nx;ny
r=. 'comparing:',LF
r=. r,nx,fx,'  ',(":2 pick dx),LF
r=. r,ny,fy,'  ',(":2 pick dy),LF
r,tx compare ty
)
compare=: 4 : 0
if. x -: y do. 'no difference' return. end.
if. 0=#x do. 'empty left argument' return. end.
if. 0=#y do. 'empty right argument' return. end.
a=. conew 'jcompare'
r=. x comp__a y
coerase a
r
)
fcompare=: 3 : 0
('';0) fcomp y
:
(x;0) fcomp y
)
fcompares=: 3 : 0
('';1) fcomp y
:
(x;1) fcomp y
)
compend=: 3 : 0
old=. 0 0
len=. (#X),#Y

while. -. len -: old do.
  old=. len
  t=. <./len
  m=. 0 i.~ (t {. X) = t {. Y
  X=: m }. X
  Y=: m }. Y
  t=. m - t
  n=. - +/ *./\. (t {. X) = t {. Y
  X=: n }. X
  Y=: n }. Y
  NX=: m }. n }. NX
  NY=: m }. n }. NY
  m=. X e. Y
  if. 0 e. m do.
    SX=: SX,(-.m)#NX
    X=: m # X
    NX=: m # NX
  end.
  m=. Y e. X
  if. 0 e. m do.
    SY=: SY,(-.m)#NY
    Y=: m # Y
    NY=: m # NY
  end.
  len=. (#X),#Y
end.
if. -. 0 e. len do. 1 return. end.
SX=: SX,NX
SY=: SY,NY
0
)
complcs=: 3 : 0
lx=. #X
ly=. #Y
if. MAXLCS < lx * ly do.
  select. MAXPFX < lx,ly
  case. 0 1 do.
    ly=. <. MAXLCS % lx
  case. 1 0 do.
    lx=. <. MAXLCS % ly
  case. do.
    lx=. ly=. MAXPFX
  end.
end.
a=. lx {. X
b=. ly {. Y
m=. ((b =/ a),.0),0
cm=. lcs ^:_ m
len=. >./ ,cm
rc=. 1 + mindx ($cm) #: I. len = ,cm
cm=. , rc {. cm
msk=. (1+i.len) =/ cm
ndx=. <@I."1 msk
pos=. ; (<rc) #: each ndx
pos=. (+/"1 pos),.pos
pos=. ((# &> ndx) # i.len),.pos
pos=. /:~ pos
'ib ia'=. |: 2 }."1 (~:{."1 pos)#pos
n=. 1 + {: ia
SX=: SX,(<<<ia) { n {. NX
X=: n }. X
NX=: n }. NX
n=. 1 + {: ib
SY=: SY,(<<<ib) { n {. NY
Y=: n }. Y
NY=: n }. NY
0
)
compare_z_=: compare_jcompare_
fcompare_z_=: fcompare_jcompare_
fcompares_z_=: fcompares_jcompare_
cocurrent <'base'