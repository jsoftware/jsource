coclass 'jpm'

SIZE=: 1e7
SCREENGLOBALS=: 0
unpack=: 6!:11
counter=: 6!:12
stats=: 6!:13
start=: 3 : 0
'' start y
:
reset''
x=. 2 {. x, (#x) }. 1 0
if. (0 < #y) *: 2 = 3!:0 y do.
  y=. ({. y, SIZE) $ ' '
end.
([ [: (6!:12) 1:) x 6!:10 y
)
stop=: 6!:10 bind ($0)
reset=: 3 : 0
4!:55 <'PMTESTDATA'
PM=: $0
PMREAD=: 0
PMENCODE=: PMDECODE=: PMSTATS=: $0
PMNDX=: PMLINES=: PMSPACE=: PMTIME=: $0
PMNAMES=: PMLOCALES=: ''
)

reset''
boxopencols=: ]`(<"1)@.(L. = 0:)
bracket=: ('['"_ , ": , '] '"_) each
dab=: -. & ' '
firstones=: > (0: , }:)
groupndx=: 4 : '<: (#x) }. (+/\r<#x) /: r=. /: x,y'
info=: mbinfo @ ('Performance Monitor'&;)
lastones=: > (}. , 0:)
maskdef=: [: * [: +/\ _1&= - 0: , }:@:(_2&=)
nolocale=: (i.&'_') {. ]
sort=: /:~ :/:
takeafter=: [: ] (#@[ + E. i. 1:) }. ]
taketo=: [: ] (E. i. 1:) {. ]
takewid=: ] ` ((WID&{.) , '...'"_) @. (WID"_ < #)
unwords=: ;: inverse
usage=: [: +/\ (- 0: , }:)
join=: ,.&.>/
colsum=: 4 : 0
nub=. ~. key=. x{"1 y
nub /:~ nub x}"_1 1 key +//. y
)
getmatchindex=: 4 : 0
bgn=. x
end=. y
level=. +/\ bgn - 0 , }: end
bpos=. I. bgn
blvl=. bpos { level
epos=. I. end
elvl=. epos { level
max=. 1 + #end
bndx=. max #. blvl ,. bpos
endx=. max #. elvl ,. epos
mtch=. bndx groupndx endx
bpos ; (/: mtch { /: bndx) { epos
)
getpm1=: 3 : 0
'nam loc'=. splitname y
nms=. _1 pick PM
nmx=. nms i. <nam
lcx=. nms i. <loc
msk=. ((0 pick PM) e. nmx) *. ((1 pick PM) e. lcx)
msk&# each }:PM
)
subpm=: 3 : 0
nms=. _1 pick PM
dat=. y&{ each }:PM
ind=. 0 pick dat
loc=. 1 pick dat
nub=. /:~ ~. ind,loc
nms=. nub { nms
loc=. nub i. loc
ind=. nub i. ind
dat=. (<ind) 0 } dat
dat=. (<loc) 1 } dat
dat , <nms
)
tominus=: 3 : 0
dat=. , y
($y) $ '-' (I. dat='_')} dat
)
ifmt=: 3 : 0
w=. 20
dat=. , y
neg=. 0 > dat
dat=. ": &.> <. | dat
msk=. (-w){.(|. w$1j1 1 1),3$1
exp=. #!.','~ ({.&msk)@-@#
dat=. exp &.> dat
dat=. (neg{'';'-'),&.>,dat
p=. - >./ # &> dat
p {. &> dat
)
ffmt=: 4 : 0
n=. ifmt 0 | y
if. x do.
  n ,. }."1 (j. x) ": ,. 1 || y
end.
)
SCALE=: 1 1

timeformat=: 6 & ffmt
TIMETEXT=: ' Time (seconds)'

spaceformat=: ifmt
SPACETEXT=: ' Space (bytes)'
getdetail=: 4 : 0

'type val'=. x
'name loc gvn'=. y

rep=. (val+1) linerep <name,'_',loc,'_'
if. 0 e. #rep do. ;~i.0 0 return. end.
ndx=. val + +/ PMENCODE * (PMNAMES i. <name),(PMLOCALES i. <loc),0
if. -. ndx e. PMNDX do. ;~i.0 0 return. end.

typename=. val pick 'monad';'dyad'
type getdetail1 typename;rep;ndx
)
getdetail1=: 4 : 0

'vid rep ind'=. y

if. -. ind e. PMNDX do.
  (i.0 4) ; '' return.
end.

dat=. x getdetail2 ind
lno=. > {."1 rep
if. lno -: _1 do.
  dat ; vid, ' ', 2 pick , rep return.
end.
replno=. _1 , > {."1 rep
repsno=. _1 ; 1 {"1 rep
reptxt=. 2 {"1 rep
len=. # &> repsno
stm=. {."1 dat
lns=. sort (len # replno) {~ (; repsno) i. stm
rep=. 3 {"1 dat
rep=. rep * stm e. {.&> repsno

val=. (1 2 {"1 dat),. rep
key=. ~. lns
val=. key ,. lns +/ /. val

val=. replno ,. }."1 (replno e. {."1 val) expand val
val=. val , 1e8, (1 2{+/val), (<0 3){ val

txt=. (bracket }.replno) ,each reptxt
txt=. vid ; txt , <'total ',vid

val ; <txt
)
getdetail2=: 4 : 0

ind=. y
msk=. PMNDX = ind

mbgn=. msk *. PMLINES = _1
mend=. msk *. PMLINES = _2
nbgn=. +/ mbgn
nend=. +/ mend

select. * nbgn - nend
case. 0 do.
  bnx=. mbgn i. 1
  enx=. >: mend i: 1
case. _1 do.
  bnx=. mbgn i. 1
  enx=. >: (+/\mend) i. nbgn
case. 1 do.
  bnx=. (+/\mbgn) i. nbgn - nend
  enx=. >: mend i: 1
end.

ndx=. PMNDX
lns=. PMLINES
val=. (x pick 'PMTIME';'PMSPACE')~
if. enx ~: #ndx do.
  ndx=. enx {. ndx
  lns=. enx {. lns
  val=. enx {. val
  mbgn=. enx {. mbgn
  mend=. enx {. mend
end.

if. bnx do.
  ndx=. bnx }. ndx
  lns=. bnx }. lns
  val=. bnx }. val
  mbgn=. bnx }. mbgn
  mend=. bnx }. mend
end.

msk=. 0 < mbgn usage mend

if. 0 e. msk do.
  ndx=. msk # ndx
  lns=. msk # lns
  val=. msk # val
  mbgn=. msk # mbgn
  mend=. msk # mend
end.

lvl=. mbgn usage mend
if. 1 +. 2 e. lvl do.
  rep=. getreps ind;ndx;lns;val
  her=. gethdetail ind;ndx;lns;val
  tot=. gettdetail ind;ndx;lns;val

  lns=. {."1 rep
  assert lns -: {."1 her
  assert lns -: {."1 tot
  res=. lns ,. ({:"1 tot) ,. ({:"1 her) ,. {:"1 rep
else.
  res=. getdetail3 ind;ndx;lns;val
end.
if. 1 ~: scale=. x { SCALE do.
  res=. res *"1 [ 1,scale,scale,1
end.

)
getdetail3=: 3 : 0
'ind ndx lns val'=. y
tmsk=. ndx = ind

tlns=. tmsk # lns
tval=. tmsk # val

msk=. tlns ~: _2
tall=. msk # (}.tval,0) - tval
tlns=. msk # tlns
f=. 0:`({: - {.) @. (0: < #)
toff=. msk # tmsk f;._1 val
ther=. tall - toff

0 colsum tlns,. tall,. ther ,. 1
)
gethdetail=: 3 : 0

'ind ndx lns val'=. y

all=. (}.val,0) - val
bgn=. lns = _1
end=. lns = _2
hit=. ind = ndx
msk=. 1 < bgn usage end
her=. 2 >: bgn usage end
bgn=. msk *. bgn
end=. msk *. end

'bdx edx'=. bgn getmatchindex end
clr=. <: bdx

hndx=. ind (I. her) } ndx
hlns=. her } lns ,: (#;.1 hit) # hit # lns
sel=. ind = clr { hndx
clr=. sel # clr
edx=. sel # edx
lns=. (clr{hlns) edx} lns

msk=. 1 edx} ndx = ind
msk=. msk *. lns ~: _2
0 colsum msk # lns ,. all
)
gettdetail=: 3 : 0

'ind ndx lns val'=. y

bgn=. lns = _1
end=. lns = _2
all=. (}.val,0) - val
j=. (ndx=ind) <;.1 lns
lns=. (# &> j) # {. &> j
lvl=. +/\ bgn - 0, }:end
msk=. 1 < lvl
bnb=. msk *. bgn
enb=. msk *. end
'bdx edx'=. bnb getmatchindex enb
clr=. <: bdx
lns=. (clr{lns) edx} lns

msk=. lns ~: _2
0 colsum msk # lns ,. all
)
getreps=: 3 : 0
'ind ndx lns val'=. y
msk=. (ndx=ind) *. lns ~: _2
0 colsum (msk # lns) ,. 1
)
maskit=: 4 : 0

bgn=. x
end=. y
sb=. +/\bgn
se=. +/\end
re=. +/\ inverse se + 0 <. <./\sb - se
mskbgn=. 0 < +/\ bgn - 0,}:re
sb=. +/\.bgn
se=. +/\.end
rb=. +/\. inverse sb + 0 <. <./\.se - sb
mskend=. 0 < +/\. end - }.rb,0

mskbgn *. mskend
)
f=. '_'"_ = {:
g=. ;&a:
h=. (i:&'_') ({.;}.@}.) ]
k=. [: h }:
splitname=: g`k @. f f.
fullname=: 3 : 0
'name loc'=. 2 {. boxopen y
if. '_' ~: {:name do. name,'_',loc,'_' else. name end.
)
getnames=: 4 : 0

'nms lcs j'=. |: PMDECODE #: x
xjp=. lcs ~: PMLOCALES i. <'jpm'

if. 0=#y do. xjp return. end.

y=. ;: y
sns=. splitname &> y
ndx=. y i. <,'~'
rin=. 1 getnames1 x;nms;lcs;<ndx {. sns
rot=. 0 getnames1 x;nms;lcs;<(ndx+1) }. sns

xjp *. rin > rot
)
getnames1=: 4 : 0

'ndx nms lcs sel'=. y

if. 0=#sel do. (#ndx)#x return. end.

nmx=. PMNAMES i. {."1 sel
lcx=. PMLOCALES i. {:"1 sel

msk=. (#ndx) # 0

len=. #. (0: < #) &> sel
if. 1 e. b=. len=1 do.
  msk=. msk +. lcs e. b#lcx
end.
if. 1 e. b=. len=2 do.
  msk=. msk +. nms e. b#nmx
end.
if. 1 e. b=. len=3 do.
  msk=. msk +. (lcs e. b#lcx) *. nms e. b#nmx
end.
)

getshortname=: 3 : 0
'name loc'=. 2 {. boxopen y
if. #loc=. loc -. ' ' do.
  ((name i.'_') {. name),'_',loc,'_'
else.
  name
end.
)
getnameloc=: 3 : 0

y=. dab y

if. 0 = #y do.
  '';'base';'' return.
end.

if. L. y do.
  select. #y
  case. 1 do.
    name=. dab > y
    loc=. 'base'
    given=. name
  case. 2 do.
    'name loc'=. dab each y
    if. #loc do.
      given=. name,'_',loc,'_'
    else.
      given=. name
      loc=. 'base'
    end.
  case. do.
    'invalid name' assert 0
  end.

else.

  given=. y
  'name loc'=. dab each splitname y
  if. 0=#loc do. loc=. 'base' end.

end.


name;loc;given
)
read=: 3 : 0
if. PMREAD do. 1 return. end.
if. 0 = +/ 6!:13'' do.
  smoutput 'There are no PM records'
  0 return.
end.

PMTIME=: 6!:11 ''
PMSTATS=: 6!:13 ''
6!:10 ''
PM=: PMTIME
locndx=. (1;0) {:: PMTIME
PMNAMES=: 6 pick PMTIME
PMLOCALES=: locndx }. PMNAMES
PMNAMES=: locndx {. PMNAMES
PMNDX=: > 3 {. PMTIME
ndx=. I. (1: e. '__'&E.) &> PMNAMES

if. #ndx do.
  nms=. (('__'&E. i. 1:) {. ]) each ndx { PMNAMES
  ndx merge nms
end.
ndx=. I. ('_'"_ = {:) &> PMNAMES

if. #ndx do.
  namx=. 0 { PMNDX
  locx=. 1 { PMNDX
  nms=. }: each ndx { PMNAMES
  ind=. i:&'_' &> nms
  loc=. (>: ind) }.each nms
  loc=. (<'base') (I. 0=# &> loc) } loc
  nms=. ind {.each nms
  lcs=. (namx i. ndx) { locx
  assert loc -: (lcs-locndx) { PMLOCALES

  ndx merge nms
end.
ind=. (0 { PMNDX) { PMNAMES i. PMNAMES
PMNDX=: ind 0 } PMNDX
PMLINES=: 3 pick PMTIME
PMSPACE=: 0, +/\ }: 4 pick PMTIME
PMTIME=: 5 pick PMTIME
PMDECODE=: 0,(#PMLOCALES),2
PMENCODE=: (2 * #PMLOCALES),2 1
PMNDX=: +/ PMENCODE * PMNDX - 0,locndx,1

PMREAD=: 1
)
merge=: 4 : 0
ndx=. x
nms=. y
namx=. 0 { PMNDX
locx=. 1 { PMNDX
nmx=. PMNAMES i. nms
msk=. nmx < #PMNAMES

if. 1 e. msk do.
  plc=. (namx i. msk#nmx) { locx
  rlc=. (namx i. msk#ndx) { locx
  b=. plc=rlc
  if. 1 e. b do.
    inx=. b # I. msk
    nwx=. ((inx{ndx), namx) i. namx
    new=. nwx { (inx{nmx), namx
    PMNDX=: new 0 } PMNDX
  end.
end.

PMNAMES=: nms ndx} PMNAMES
)
EXHDR=: '1234' ;&,&> ':'
linerep=: 4 : 0
dat=. x (5!:7) y
if. #dat do.
  stm=. ;{."1 dat
  lns=. {: &> 1 {"1 dat
  txt=. {:"1 dat
  lno=. <&> ~. lns
  sno=. lns </. stm
  ltx=. lns <@unwords/. txt
  lno ,. sno ,. ltx
else.
  lrep=. 5!:5 :: 0: y
  if. lrep -: 0 do. '' return. end.
  if. (x=1) *. '4 : 0' -: 5 {. lrep do. '' return. end.
  _1;_1;lrep
end.
)
showdetail=: 3 : 0
0 showdetail y
:
if. 0=#y do. return. end.
if. 0=read '' do. i.0 0 return. end.
if. 0={.PMSTATS do.
  'No detail PM records were recorded' return.
end.
y=. getnameloc y
}. ; LF ,&.> showdetail1&y each x
)
showdetail1=: 4 : 0
tit=. 'all';'here';'rep';{:y
top=. x pick TIMETEXT;SPACETEXT
'name loc given'=. y
res=. x showdetail2 y
if. 0 = L. res do. return. end.

'mdat mtxt ddat dtxt tdat ttxt'=. res

if. (#mdat) *. #ddat do.
  txt=. mtxt ,each ' ' ,each dtxt ,each ' ' ,each ttxt
else.
  txt=. mtxt ,each dtxt
end.

top, , LF ,. ": tit ,: txt
)
showdetail2=: 4 : 0
'name loc given'=. y

'mdat mrep'=. (x,0) getdetail y
'ddat drep'=. (x,1) getdetail y

if. 0 = (#mdat) + (#ddat) do.
  'not found: ',getshortname given return.
end.

if. 0 e. (#mdat), (#ddat) do.
  tdat=. 0, (1 2 { -: +/ mdat,ddat), {: +/ (1 {. mdat) , 1 {. ddat
else.
  tdat=. 0 ; 2 }. x gettotal fullname y
end.

trep=. <'total definition'
mtxt=. x showdetailfmt mdat;<mrep
dtxt=. x showdetailfmt ddat;<drep
ttxt=. x showdetailfmt tdat;<trep

mdat;mtxt;ddat;dtxt;tdat;<ttxt
)
showdetailfmt=: 4 : 0
'dat lns'=. y
if. 0 e. #dat do. 4 # <i.0 0 return. end.
if. x do. f=. spaceformat else. f=. timeformat end.
'all here rep'=. }. |: dat
lns=. > (#rep) {. boxxopen lns
(f all);(f here);(":,.rep); lns
)
detailoption=: 3 : '2 {. y , (#y) }. 0 0'
totaloption=: 3 : '3 {. y , (#y) }. 0 0 90'
showtotal=: 3 : 0
0 showtotal y
:
if. 0=read '' do. i. 0 0 return. end.
opt=. totaloption x
'x s p'=. opt
if. -. x e. 0 1 do.
  r=. 'first number in left argument should be either 0 (time)'
  r,' or 1 (space)' return.
end.
if. -. s e. 0 1 do.
  r=. 'second number in left argument should be either 0 (distinguish'
  r,' names by locale) or 1 (total names over locales)' return.
end.
if. s do.
  tit=. 'name (all locales)';'all';'here';'here%';'cum%';'rep'
else.
  tit=. 'name';'locale';'all';'here';'here%';'cum%';'rep'
end.
dat=. x showtotalfmt opt showtotal1 y
if. s do. dat=. (<<<1) { dat end.
dat=. > each dat
txt=. tit ,: dat
t=. x pick TIMETEXT;SPACETEXT
t, ,LF ,. ": txt
)
showtotal1=: 4 : 0
't s p'=. x

'nam loc all her rep'=. x gettotal y
if. 0 = #nam do. a: return. end.

j=. her % +/her
pct=. 0.1 * <. 0.5 + 1000 * j
cpt=. <. 0.5 + 100 * +/\ j
if. t do. f=. spaceformat else. f=. timeformat end.

nam=. nam, <'[total]'
loc=. loc, <''
all=. all
her=. her, +/her
pct=. pct,100
cpt=. cpt,100
rep=. rep

nam; loc; all; her; pct; cpt; rep
)
showtotalfmt=: 4 : 0
'nam loc all her pct cpt rep'=. y

if. x do. f=. spaceformat else. f=. timeformat end.

all=. f all
her=. f her
pct=. 1 ffmt pct
cpt=. ":,.cpt
rep=. ":,.rep

nam; loc; all; her; pct; cpt; rep
)
gettotal=: 4 : 0

if. 0=read'' do. a: return. end.

't s p'=. x
ndx=. PMNDX
lns=. PMLINES
if. t do. dat=. PMSPACE else. dat=. PMTIME end.
if. 0 e. lns do.
  msk=. lns < 0
  dat=. msk # dat
  ndx=. msk # ndx
  lns=. msk # lns
end.
msk=. (lns = _1) maskit lns = _2
if. 0 e. msk do.
  dat=. msk # dat
  ndx=. msk # ndx
  lns=. msk # lns
end.

if. 0 = #ndx do. a: return. end.
ndx=. ndx - 2 | ndx
nub=. ~. ndx

rms=. nub getnames y
req=. rms#nub
'all her rep'=. t gettotals ndx;lns;dat;nub;rms

'nx lx j'=. |: PMDECODE #: req
if. s do.
  'all her rep'=. |: nx +//. all,.her,.rep
  nx=. ~. nx
end.
ndx=. \: her ,. all
her=. ndx{her
all=. ndx{all
if. p < 100 do.
  len=. 1 + 1 i.~ (+/\her) >: (+/her) * p % 100
  curtailed=. len < #her
  if. curtailed do.
    curall=. +/ len }. all
    curher=. +/ len }. her
    her=. len {. her
    all=. len {. all
    ndx=. len {. ndx
  end.
else.
  curtailed=. 0
end.

rep=. ndx{rep
nam=. (ndx{nx) { PMNAMES

if. s do.
  loc=. (#nx) # a:
else.
  loc=. (ndx{lx) { PMLOCALES
end.

if. curtailed do.
  her=. her, curher
  loc=. loc, a:
  nam=. nam, <'[rest]'
end.

if. 1 ~: scale=. t { SCALE do.
  all=. scale * all
  her=. scale * her
end.
nam ; loc ; all ; her ; rep
)
gettotals=: 4 : 0

'ndx lns dat nub req'=. y

bgn=. lns = _1
end=. lns = _2
f=. 1: = [: +/\ (- (0: , 1: - }:))
msk=. f each ndx < /. bgn

ada=. ndx < /. dat * _1 ^ bgn

all=. msk +/@# &> ada
rep=. <. -: # &> ada
sbg=. }:bgn
snd=. sbg < }.end
dff=. (}. - }:) dat
str=. (bgn # ndx) ,. sbg # dff
edr=. ((0,snd) # ndx) ,. snd #dff
spc=. i.0 2
level=. +/\ bgn - end
ups=. (1 1 E. bgn) > 1 1 0 0 E. bgn
if. 1 e. ups do.

  ulvl=. ups # level
  upos=. I. ups
  spd=. (0 1 E. bgn) *. level > 0
  slvl=. spd # level
  spos=. I. spd

  if. #spos do.

    lmax=. 1 + ({:upos) >. {:spos

    uelp=. lmax #. ulvl ,. upos
    ind=. /: uelp
    uelp=. ind { uelp
    uvrb=. ind { ups # ndx

    selp=. lmax #. slvl ,. spos
    svrb=. (uelp groupndx selp) { uvrb
    sdff=. spos { dff
    spc=. svrb ,. sdff
  end.

end.
spcr=. i.0 2
if. +./mrec=. (_2=PMLINES)*.PMNDX=1|.PMNDX do.
  rdat=. >(>{.x){PMTIME;PMSPACE
  mdif=. mrec # rdat-1|.rdat
  spcr=. (mrec # PMNDX) ,.mdif
  spcr=. ((0{"1 spcr)e.ndx) # spcr
  spcr=. i.0 2
end.
sum=. str , edr , spc , spcr
her=. (nub i. {."1 sum) +/ /. {:"1 sum

if. x=0 do.
  assert *./ all >: her
end.

|: req # all ,. her ,. rep
)
getused=: 3 : 0
if. 0=read '' do. i. 0 0 return. end.
'nms lcs j'=. |: PMDECODE #: ~. PMNDX
xjp=. lcs ~: PMLOCALES i. <'jpm'
sort xjp # (nms { PMNAMES) ,. lcs { PMLOCALES
)
getnotused=: 3 : 0
loc=. 18!:1 [ 0 1
r=. i.0 2
for_lc. loc do.
  r=. r, (nl__lc 1 2 3) ,. lc
end.
sort r -. getused''
)
