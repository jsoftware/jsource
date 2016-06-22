coclass'jfif'
coinsert'jhs'

HBS=: 0 : 0
jhma''
jhjmlink''
jhmz''
'find'   jhb'Find'
'what'   jhtext '';20
'where'  jhtext '';50
'context'jhselect(<;._2 FIFCONTEXT);1;0
'matchcase' jhcheckbox'case';0
'subfolders'jhcheckbox'sub';0
'nameonly'  jhcheckbox'lines';0
jhbr
jhresize''
'area'   jhdiv''
)

NB. 'wherex'  jhselect JHSFOLDERS;1;JHSFOLDERS i. <FIFDIR
NB. 'type'   jhselect JHSFILTERS;1;JHSFILTERS i. <FIFTYPE

NB. regex option not supported in UI, but could be

NB. state init
crx=: 3 : 0
TABNDX=: 1
getfoldernames''
FIFFOLDERS=: UserFolders_j_,SystemFolders_j_
fif_rundef''
FIFINFO=: ''
NB. JHSFILTERS=: {."1 FIFFILTERS
NB. JHSFOLDERS=: {."1 FIFFOLDERS
)

create=: 3 : 0
crx''
'jfif'jhr''
)

jev_get=: create

ev_find_click=: 3 : 0
t=. <;._2 getv'jdata'
'FIFWHAT FIFCONTEXTNDX FIFTYPE FIFDIR FIFCASE FIFSUBDIR FIFREGEX FIFNAMEONLY'=: t
if. '/'={:FIFDIR do. FIFDIR=: }:FIFDIR end.
if. -.'*'e.FIFDIR do. FIFDIR=: FIFDIR,'/*.ijs' end. 
i=. FIFDIR i:'/'
FIFTYPE=: (>:i)}.FIFDIR
FIFDIR=: i{.FIFDIR
FIFCONTEXTNDX=: ".FIFCONTEXTNDX
FIFCASE=: ".FIFCASE
FIFSUBDIR=: ".FIFSUBDIR
FIFREGEX=: ".FIFREGEX
FIFNAMEONLY=: ".FIFNAMEONLY
FIFINFO=: ''
JHSFOUNDFILES=: ''
fiff_find_button''
jhrajax FIFINFO,>FIFNAMEONLY{JHSFOUNDFILES;FIFFOUND
)

CSS=: 0 : 0
*{font-family:"courier new","courier","monospace";}
)

JS=: 0 : 0
function ev_body_load()
{
 setlast("what");
 setlast("where");
 
 jform.context.selectedIndex= getls("context");
 jform.matchcase.checked=  getlstf("matchcase");
 jform.subfolders.checked= getlstf("subfolders");
 jform.nameonly.checked=   getlstf("nameonly");
 
 jbyid("what").focus();
 jresize();
}

function ev_what_enter(){jscdo("find");}
function ev_where_enter(){jscdo("find");}
function ev_find_click_ajax(ts){jbyid("area").innerHTML=ts[0];}


function ev_find_click()
{
 adrecall("what",jform.what.value,"0");
 adrecall("where",jform.where.value,"0");
 setls("context",   jform.context.selectedIndex);
 setls("matchcase", jform.matchcase.checked);
 setls("subfolders",jform.subfolders.checked);
 setls("nameonly",  jform.nameonly.checked);
 var t=jform.what.value+JASEP;
 t+=jform.context.selectedIndex+JASEP;
 t+=0+JASEP; // type is part of where - jform.type.value+JASEP;
 t+=jform.where.value+JASEP;
 t+=(jform.matchcase.checked?1:0)+JASEP;
 t+=(jform.subfolders.checked?1:0)+JASEP;
 t+=0+JASEP; // regex not supported by ui
 t+=(jform.nameonly.checked?1:0)+JASEP;
 jdoajax([],t);
}
)

NB. ffss version for use from jijs
NB. dat ffssijs what;contextindex;case;regex
ffssijs=: 4 : 0
save=. FIFWHAT;FIFCONTEXTNDX;FIFCASE
'FIFWHAT FIFCONTEXTNDX FIFCASE'=: y
dat=. toLF x
fl=.  'ijs'
fls=. ''

RX=: FIFREGEX    

if. FIFCASE=0 do.
  what=. tolower FIFWHAT
else.
  what=. FIFWHAT
end.

if. 0=ffssinit what do. '' return. end.

  dat=. termLF dat
  if. FIFCASE do. txt=. dat else. txt=. tolower dat end.
  if. RX do.
    select. FIFCONTEXTNDX
    case. 7 do.
      m=. ; quotemask each <;.2 txt
      txt=. m } DEL,:txt
    case. 8 do.
      m=. ; quotemaskx each <;.2 txt
      txt=. m } txt ,: DEL
    end.
    ndx=. FIFCOMP ffmatches txt
    if. FIFCONTEXTNDX e. 2 3 4 do.
      ind=. FIFCOMS ffmatches txt
      if. #ind do.
        ndx=. /:~ ndx,ffmatchass txt;ind;what
      end.
    end.
    if. (FIFCONTEXTNDX=6) *. *#ndx do.
      prevnl=. ndx (i:&1@:>: { ])"0 1 (0) , >: I. FIFFRET = dat
      prevnb=. ndx (i:&1@:>: { ])"0 1 (_1) , I. 'NB.' E. dat
      ndx=. ndx #~ prevnl > prevnb
    end.
  else.
    ndx=. I. what E. txt
  end.
  if. rws=. #ndx do.
    ind=. (0,}:I. txt=LF) groupndx ndx
    b=. ~: ind
    lns=. b#ind
  else.
    lns=. ''
  end.

if. RX do. rxfree_jregex_ FIFCOMP,FIFCOMS end.

'FIFWHAT FIFCONTEXTNDX FIFCASE'=: save
rws;lns
)


NB. hacked version of j602 fif
NB. defines to let jhs load fif code
IFWINNT=: 0
IFWIN=: 0
PATHSEP=: '/'
PATHSEP_j_=: '/'
SYSTEMFOLDERS=: SystemFolders_j_
USERFOLDERS=: UserFolders_j_

NB. see override hacks at end

termLF=: , (0: < #) # LF"_ -. _1&{.
addfret=: , PATHSEP"_ -. [: {: PATHSEP"_ , ]
bcfind=: #@[ (| - =) i. &: (filecase each)
bcfind0=: {."1 @ [ bcfind ]
cutdeb=: [: -.&a: <@deb;._2@termLF
deb=: #~ (+. 1: |. (> </\))@(' '&~:)
delfret=: }: , {: -. PATHSEP"_
drophead=: ] }.~ #@[ * [ -: &: filecase #@[ {. ]
extijp=: 3 : 0
y, ((0 < #y) > '.ijp' -: _4 {. y) # '.ijp'
)
extijs=: 3 : 0
y, ((0 < #y) > ('.ijl';'.ijs') e.~ < _4 {. y) # '.ijs'
)
extnone=: 3 : 0
(- 4 * (<_4 {. y) e. '.ijp';'.ijs') }. y
)
fexist=: 1:@(1!:4)@boxopen :: 0:
fexists=: #~ fexist&>
find=: #@[ (| - =) i.
find0=: {."1 @ [ bcfind ]
fixrelative1=: 3 : 0
fn=. y
if. 0 = #fn do. return. end.
if. '~' = {. fn do. return. end.
if. PATHSEP = {. fn do. return. end.
if. IFWIN *. ':' = 1 { fn,':' do. return. end.
tofoldername1 fullname fn
)
fromfoldername=: 3 : 0
nms=. boxxopen y
ind=. I. '~' = ({. @ (,&' ')) &> nms
new=. }. each ind { nms
ndx=. <./ @ (i.&'/\') &> new
tag=. ndx {. each new
par=. '.' = {. &> tag
len=. ('.'&= i. 0:) &> tag
tag=. len }. each tag
inx=. PPIDS i. tag
msk=. inx < # PPIDS
hdr=. (msk # inx) { PPATHS
if. 1 e. msk # par do.
  sel=. I. msk # par
  shd=. sel { hdr
  f=. #@] | ([: +/\. ] e. '/\'"_) i: [
  top=. (sel { len) f each shd
  shd=. top {. each shd
  hdr=. shd sel } hdr
end.
new=. hdr ,each msk # ndx }. each new
nms=. new (msk # ind) } nms
jpath each nms
)
fullproject=: fullname @ extijp
fullscript=: (0: < #) # fullname @ extijs
getdepth=: 3 : 0
+/ PID = PATHSEP
)
getfoldernames=: 3 : 0
folders=. (SYSTEMFOLDERS,.<0),USERFOLDERS
folders=. folders \: # &> {:"1 folders
PPIDS=: {."1 folders
PPATHS=: filecase each 1 {"1 folders
PSUBS=: ;2 {"1 folders
)
getlibfiles=: 3 : 0
f=. (0{"1 PUBLIC),y
t=. (1{"1 PUBLIC),y
t {~ f i. y
)
getpidpath=: 3 : 0
pid=. y
ind=. pid i. PATHSEP
ndx=. ({."1 USERFOLDERS) bcfind <ind {. pid
if. ndx ~: _1 do.
  t=. (> 1 { ndx{USERFOLDERS),ind }. pid
else.
  t=. pid
end.
fullname addfret t
)
pidname=: 3 : 0
y=. PIDPATH drophead extnone y
x=. <;.1 PATHSEP,y
if. =/ _2 {. x do. }. ; }: x else. y end.
)
pr1=: (fixrelative1 @ extijs each) @: cutdeb
pr2=: cutdeb
projcfgread=: 3 : 0
PMTABS=: 0 2
PMSNAPS=: 0
PMSNAPX=: ''
PROJECTLOOKIN=: ''
PROJECTRECENT=: ''
try. 0!:0 <PROJECTCFG catch. end.
XPROJECTRECENT=: fexists fullproject each cutopen PROJECTRECENT
XPROJECTLOOKIN=: fullname PROJECTLOOKIN
)
projgetfiles=: 3 : 0
p=. addfret y
files=. projtree p
sort filecase each files
)
projsplit=: 3 : 0
if. 0 < L. y do. y return. end.
if. 0=#y do. '';'' return. end.
dat=. <;._2 y
g=. i.&' '
h=. g (toblank@{. ; deb@}.) ]
h &> dat
)
projtree=: 3 : 0
r=. ''
t=. y
ps=. PATHSEP_j_
if. #t do. t=. t, ps -. {:t end.
dirs=. <t
while. #dirs do.
  fpath=. (>{.dirs) &,
  dirs=. }. dirs
  dat=. 1!:0 fpath '*'
  if. #dat do.
    drs=. seldir dat
    if. #drs do.
      dirs=. (fpath @ (,&ps) each {."1 drs),dirs
    end.
    r=. r, fpath each selproj dat
  end.
end.
sort filecase each r
)
seldir=: #~ '-d'&-:"1 @ (1 4&{"1) @ > @ (4&{"1)
selproj=: (#~ ([:(<'.ijp')&=_4&{.&.>)) @: ({."1)
tofoldername1=: 3 : 0
nam=. y
if. '~' = {. nam,'~' do. return. end.

nam=. filecase nam

msk=. -. PSUBS
pps=. msk # PPATHS ,each PATHSEP
pds=. msk # PPIDS
len=. # &> pps
msk=. pps = len {. each <nam
if. 1 e. msk do.
  ndx=. ((i. >./) msk # len) { I. msk
  '~', (> ndx { pds),(<: ndx { len) }. nam return.
end.

if. -. ifuserfolder PID do. nam return. end.
pid=. PIDPATH
msk=. pid = (#pid) {. nam
ndx=. msk i. 0
if. ndx <: 1 do. nam return. end.
if. 1 >: +/ PATHSEP = ndx {. pid do. nam return. end.

cnt=. (+/ PATHSEP = ndx }. pid) - getdepth''
if. cnt < 1 do. nam return. end.

'~', (cnt # '.'), PID, (<:ndx) }. nam
)

3 : 0''
if. IFUNIX do.
  filecase=: [
else.
  filecase=: tolower
end.
0
)
finfo=: 3 : 'sminfo FIFTITLE;y'
fquery=: 4 : 'x wdquery FIFTITLE;y'
groupndx=: [: <: I. + e.~
isempty=: 0: e. $
subs=. 2 : 'x I. @(e.&y)@]} ]'
toblank=: ' ' subs '_'
tohyphen=: '_' subs ' '
subtcc=: ' '&(I.@(e.&(TAB,CRLF))@]})
termdelLF=: }.~ [: - 0: i.~ LF&= @ |.
todelim=: ; @: ((DEL&,) @ (,&DEL) @ , @ ": &.>)
tolist=: }. @ ; @: (LF&, each)
toLF=: (10{a.)&(I. @(e.&(13{a.))@]})
words=: ;: :: ((}: , }:&.>@{:)@;:@(,&''''))

ffmatches=: {.@{."2 @ rxmatches_jregex_
fifsbs=: #~ -.@('\\'&E.)
fifplain=: ;@(,~&.> e.&'[](){}$^.*+?|\' #&.> (<PATHSEP_j_)"_)

fullname=: fullname_j_ f.
termdelLF=: }.~ [: - 0: i.~ LF&= @ |.

wdifopen=: boxopen e. <;._2 @ wdqp
3 : 0''
if. IFUNIX do.
  filecase=: [
else.
  filecase=: tolower
end.
0
)
fcut=: 3 : 0
txt=. y,';'
txt=. (txt e. ',;') <;._2 txt
txt=. deb each txt
txt -. a:
)
fiflower=: 3 : 0
if. IFWIN do.
  PIDPATH=: tolower PIDPATH
  PJFILES=: tolower each PJFILES
  PROJECTFILE=: tolower PROJECTFILE
end.
)
fopenfile=: 4 : 0
if. #FIFOPENFILE do.
  fork_jtask_ '"',FIFOPENFILE,' ',y,'"'
else.
  'row hit'=. x
  try.
    smopen y
  catch. finfo 'Unable to open: ',y return.
  end.
  off=. +/ CR = hit {. fread y
  smscroll row
  smsetselect 2 # 0 >. hit - off
  smfocus ''
end.
)
quotemask=: 3 : 0
m=. y = ''''
m=. m +. ~:/\. m
c=. +./\ m < 'NB.' E. y
m=. m > (0,}:m) ~: }.m,0
c < (}:m),0
)
quotemaskx=: 3 : 0
m=. y = ''''
m=. m +. ~:/\. m
c=. +./\ m < 'NB.' E. y
c < (}:m),0
)
timestamp=: 3 : 0
if. 0 = #y do. y=. 6!:0'' else. y=. y end.
r=. }: $ y
t=. 2 1 0 3 4 5 {"1 [ _6 [\ , 6 {."1 <. y
d=. '+++::' 2 6 11 14 17 }"1 [ 2 4 5 3 3 3 ": t
mth=. _3[\'   JanFebMarAprMayJunJulAugSepOctNovDec'
d=. ,((1 {"1 t) { mth) 3 4 5 }"1 d
d=. '0' (I. d=' ') } d
d=. ' ' (I. d='+') } d
(r,20) $ d
)
truncate=: {. , '...'"_

FIFCASE=: 1
FIFCONTEXTNDX=: 0
FIFFILES=: ''
FIFDIR=: '~temp'
FIFFOUND=: ''
FIFFRET=: LF
FIFHELP=: 'Dictionary'
FIFMSK=: ''
FIFNAMEONLY=: 1
FIFREGEX=: 0
FIFSHOW=: 0           
FIFSUBDIR=: 1
FIFTYPE=: '*.ijs'          
FIFFILTERS=: 0 2$''
FIFWHAT=: ''

FIFXHELP=: '' NB. not used in JHS
FIFMAXSIZE=: IFWINNT { 30000 200000
FIFMAXLINE=: 256

FIFTITLE=: 'Find in Files'
FIFCFGFILE=: jpath '~config\ffconfig.ijs'
FIFHTMFILE=: jpath '~temp\fif.htm'

FIFPATHSEP=: PATHSEP_j_
termSEP=: , (0: < #) # FIFPATHSEP"_ -. _1&{.
termdelSEP=: }.~ [: - 0: i.~ FIFPATHSEP&= @ |.

PROJECTCFG=: jpath '~config\projects.ijs'
fifputconfig=: 3 : 0
dat=. 'NB. Find in Files configuration',LF,LF
dat=. dat,'FIFFILTERS' fifjoin FIFFILTERS
dat=. dat,LF,'FIFOPENFILE=: ''',(":FIFOPENFILE),''''
dat=. toHOST dat
dat 1!:2 <FIFCFGFILE
)
fifjoin=: 4 : 0
if. 0 = L. y do.
  dat=. y
else.
  dat=. }. , LF ,. (>tohyphen each {."1 y) ,. ' ',. > {:"1 y
end.
dat=. x,'=: 0 : 0',LF,(termLF dat),')',LF
)
fifsplit=: 3 : 0
if. 0 < L. y do. y return. end.
dat=. <;._2 y
g=. i.&' '
h=. g (toblank@{. ; deb@}.) ]
h &> dat
)
fifgetfilters=: 3 : 0
FIFFILTERS=: fifsplit FIFFILTERS
)


FIFFILTERS=: 0 : 0
Scripts     *.ijs
Labs        *.ijt
Text        *.txt
Plaintext   *.ijs, *.ijt, *.txt
HTML        *.htm, *.html
All         *
)

FIFOPENFILE=: ''

WPINSERT=: 0 : 0
pc wpinsert closeok;pn "Insert";
xywh 5 6 67 51;cc g0 groupbox;cn "Select";
xywh 9 17 58 11;cc inchar radiobutton;cn "&Character";
xywh 9 29 58 11;cc instr radiobutton group;cn "&String";
xywh 9 41 58 11;cc injstr radiobutton group;cn "&J String";
xywh 78 9 110 140;cc inlist listbox ws_vscroll rightmove bottommove;
xywh 195 11 40 12;cc insert button leftmove rightmove;cn "&Insert";
xywh 195 26 40 12;cc cancel button leftmove rightmove;cn "Close";
xywh 195 136 40 12;cc undo button leftmove topmove rightmove bottommove;cn "&Undo";
pas 4 2;pcenter;
rem form end;
)
wpinsert_run=: 3 : 0
if. wdifopen 'wpinsert' do. wd 'psel wpinsert' return. end.
WPINSERTNDX=: 0
WPUNDO=: ''
pos=. wd 'psel fif;qformx'
wd WPINSERT
wdcenter pos
wd 'setenable undo 0'
wpshowlist''
wd 'pshow;'
)
wpshowlist=: 3 : 0
wd 'set inlist *',tolist WPINSERTNDX pick FIFINSERT
wd 'setselect inlist -1'
)
wpinsertone=: 3 : 0
if. 0=#ndx=. ". inlist_select do.
  finfo 'Nothing to insert' return.
end.
ins=. ndx pick WPINSERTNDX pick FIFINSTXT
dat=. wd 'setenable undo;psel fif;qd'

select. TABNDX
case. 0 do. j=. 'pwhat_select';'pwhat'
case. 1 do. j=. 'what_select';'what'
case. 2 do. j=. 'hwhat_select';'hwhat'
end.

j=. j wdget dat
'sel what'=. WPUNDO=: j

c=. {: 0, ". sel
wd 'setreplace what ',j=. DEL,(ins-.'Q'),DEL
wd 'setreplace hwhat ',j
wd 'setselect what ',k=. ":2#c+ins i.'Q'
wd 'setselect hwhat ',k
wd 'set regex 1;set case 0;set hregex 1;set hcase 0'
if. NOPROJ=0 do.
  wd 'setreplace pwhat ',j
  wd 'setselect pwhat ',k
  wd 'set pregex 1;set pcase 0'
end.
wd 'psel wpinsert'
)
wpinsert_undo_button=: 3 : 0
if. WPUNDO -: '' do.
  finfo 'Nothing to do' return.
end.
'sel what'=. WPUNDO
wd 'setenable undo 0;psel fif;'
wd 'set what *',what
wd 'setselect what ',sel
if. NOPROJ=0 do.
  wd 'set pwhat *',what
  wd 'setselect pwhat ',sel
end.
)
wpinsert_inlist_button=: wpinsertone
wpinsert_insert_button=: wpinsertone
wpinbutton=: 3 : 0
if. y ~: WPINSERTNDX do.
  WPINSERTNDX=: y
  wpshowlist''
end.
)
wpinsert_close=: wd bind 'pclose;psel fif'
wpinsert_cancel_button=: wpinsert_cancel=: wpinsert_close
wpinsert_inchar_button=: wpinbutton bind 0
wpinsert_instr_button=: wpinbutton bind 1
wpinsert_injstr_button=: wpinbutton bind 2
f=. < @: (<;._1;._2)
j=. f (0 : 0)
@Any character@.Q
@Blank character@ Q
@Whitespace character@[[:blank:]]Q
@Control character@[[:cntrl:]]Q
@Printable character@[[:print:]]Q
@Alphanumeric@[[:alnum:]]Q
@Alphabetic@[[:alpha:]]Q
@Digit@[[:digit:]]Q
@Hex digit@[[:xdigit:]]Q
@Lowercase letter@[[:lower:]]Q
@Uppercase letter@[[:upper:]]Q
@Start of line@^Q
@End of line@$Q
)
j=. j, f (0 : 0)
@Subexpression@(Q)
@Character in set@[Q]
@Characters not in set@[^Q]
@Or@|Q
@Any of@*Q
@Some of@+Q
@Optional@?Q
@Interval@{Q,}
@Any characters (0 or more)@.*Q
@Some characters (1 or more)@.+Q
@Any whitespace (0 or more)@[[:blank:]]*Q
@Some whitespace (1 or more)@[[:blank:]]+Q
)
j=. j, f (0 : 0)
@Character item@ *'(''|[^'])'Q
@Character constant@ *'(''|[^'])*'Q
@Numeric item@ *[[:digit:]_][[:alnum:]_.]*Q
@Numeric constant@ *[[:digit:]_][[:alnum:] _.]*Q
@Number first digit@[[:digit:]_]Q
@Number non-first digit@[[:alnum:]_.]Q
@Name@ *[[:alpha:]][[:alnum:]_]*Q
@Name trailer@[[:alnum:]_]*Q
@Name first character@[[:alpha:]]Q
@Name non-first character@[[:alnum:]_]Q
@Assignment@ *=[.:]Q
@In parentheses@ *\(Q\)
@Comment@ *NB\..*Q
@Optional comment@ *(NB\..*)?$Q
@Start of J line@^[[:blank:]]*Q
@End of J line@ *(NB\..*)?$Q
@Character not in name or number@(^|[^[:alnum:]_.]|$)Q
)

FIFINSERT=: {."1 each j
FIFINSTXT=: {:"1 each j
t=: wpinsert_run

ffssinit=: 3 : 0
if. (FIFCONTEXTNDX=0) > RX do. 1 return. end.

if. RX do.
  p=. y
else.
  RX=: 1
  p=. fifplain y
end.

s=. ''
nna=. '(^|[^[:alnum:]_])'
nnz=. '($|[^[:alnum:]_.:])'
ass=. '[[:space:]]*='
select. FIFCONTEXTNDX
case. 1 do. p=. nna,p,nnz              
case. 2 do. p=. nna,p,s=. ass,'[.:]'   
case. 3 do. p=. nna,p,s=. ass,':'      
case. 4 do. p=. nna,p,s=. ass,'\.'     
case. 5 do. p=. 'NB\..*',p             
end.

FIFCOMP=: rxcomp_jregex_ :: _1: p
FIFCOMS=: rxcomp_jregex_ :: _1: '''',s

if. _1 e. FIFCOMP,FIFCOMS do.
  finfo 'Unable to compile regular expression'
  rxfree_jregex_ FIFCOMP,FIFCOMS
  0
else.
  1
end.
)
ffss=: 3 : 0
JHSFOUNDFILES=: ''
RX=: FIFREGEX    

if. FIFCASE=0 do.
  what=. tolower FIFWHAT
else.
  what=. FIFWHAT
end.
fls=. ffgetfiles''
if. 0 e. #fls do. '' return. end.

if. 0=ffssinit what do. '' return. end.

fnd=. ''
lns=. ''
msk=. ''
hit=. ''

dr=. fls

read=. (1!:1 :: _1:) @ <
if. FIFFRET=CR do.
  read=. toLF @ (read f.)
end.

while. #dr do.
  dat=. read fl=. >{.dr
  dr=. }.dr
  if. dat -: _1 do. msk=. msk,0 continue. end.
  dat=. termLF dat
  if. FIFCASE do. txt=. dat else. txt=. tolower dat end.
  if. RX do.
    select. FIFCONTEXTNDX
    case. 7 do.
      m=. ; quotemask each <;.2 txt
      txt=. m } DEL,:txt
    case. 8 do.
      m=. ; quotemaskx each <;.2 txt
      txt=. m } txt ,: DEL
    end.
    ndx=. FIFCOMP ffmatches txt
    if. FIFCONTEXTNDX e. 2 3 4 do.
      ind=. FIFCOMS ffmatches txt
      if. #ind do.
        ndx=. /:~ ndx,ffmatchass txt;ind;what
      end.
    end.
    if. (FIFCONTEXTNDX=6) *. *#ndx do.
      prevnl=. ndx (i:&1@:>: { ])"0 1 (0) , >: I. FIFFRET = dat
      prevnb=. ndx (i:&1@:>: { ])"0 1 (_1) , I. 'NB.' E. dat
      ndx=. ndx #~ prevnl > prevnb
    end.
  else.
    ndx=. I. what E. txt
  end.
  if. rws=. #ndx do.
    msk=. msk,1
    t=. jhsfixfl fl
    fnd=. fnd,t,' (',(":#ndx),')'
    JHSFOUNDFILES=: JHSFOUNDFILES,t
    txt=. dat
    ind=. (0,}:I. txt=LF) groupndx ndx
    b=. ~: ind
    ind=. b#ind
    hit=. hit, <b#ndx
    lns=. lns, <ind
    txt=. ; ind { <;.2 txt
    fnd=. fnd,jhsfixtxt txt
  else.
    msk=. msk,0
  end.
end.

if. RX do. rxfree_jregex_ FIFCOMP,FIFCOMS end.
NB. JHS fnd=. termdelLF toJ }. fnd
fnd;lns;hit;msk;<fls
)
ffmatchass=: 3 : 0
'txt ind p'=. y
r=. ''
p=. ' ',p,' '
for_i. |. ind do.
  txt=. i {. txt
  ndx=. txt i: ''''
  fdx=. txt i: FIFFRET
  if. ndx > fdx do.
    blk=. ' ',((ndx+1) }. txt),' '
    r=. r, (1 e. p E. blk) # i
  end.
end.
|.r
)

ffextmask=: 4 : 0
f=. j_tolower_j_ @ (-@(i.&'.')@|. {. ])
x #~ (f each x) e. f each y
)
ffgetfiles=: 3 : 0

if. 0=4!:0 <'DIRTREEX_j_' do.
  ex=. boxxopen DIRTREEX_j_
else.
  ex=. ''
end.

select. TABNDX
case. 0 do.
  j=. XOTHERFILES,XFILES,getlibfiles XLIBS
  jpath each /:~ ~. j -. a: return.
case. 1 do.
  types=. ffgettypes FIFTYPE
  dirs=. FIFFOLDERS ffgetdirs FIFDIR
  excl=. ''
case. 2 do.
  types=. '*.htm';'*.html'
  dirs=. FIFHELPS ffgetdirs FIFHELP
  excl=. FIFXHELP
end.

if. 0 = #dirs do.
  finfo 'Folder not found' return.
end.
r=. ''
dirs=. fullname each dirs

while. #dirs do.
  fpath=. (>{.dirs) &,
  dirs=. }.dirs
  dat=. a: -.~ 1!:0 each fpath each types

  if. #dat do.
    dat=. {."1 ;dat
    r=. r, fpath each /:~ dat
  end.

  if. FIFSUBDIR do.
    if. #j=. 1!:0 fpath '*' do.
      if. #j=. ({."1 ffgetsubdir j) -. ex do.
        dirs=. ((fpath @ (,&FIFPATHSEP)) each j),dirs
        dirs=. dirs -. excl
      end.
    end.
  end.
end.

r=. ( ;@:((# {. 1:)&.>) <;.1 filecase@;) r

if. 0 = #r do.
  finfo 'No files found'
else.
  ~. jpath each r
end.
)
ffgettypes=: 3 : 0
ndx=. ({."1 FIFFILTERS) bcfind <y
if. ndx ~: _1 do.
  fcut > {: ndx{FIFFILTERS
else.
  if. 1 e. ' ,:' e. y do.
    sel=. fcut y
    ndx=. ({."1 FIFFILTERS) bcfind sel
    ind=. I. ndx >: 0
    ((<(ind{ndx);1) { FIFFILTERS) ind } sel
  else.
    <y
  end.
end.
)
ffgetdirs=: 4 : 0
ndx=. ({."1 x) bcfind <y
if. ndx ~: _1 do.
  t=. > {: ndx{x
else.
  t=. y
end.
termSEP each fcut t
)
ffgetsubdir=: #~ '-d'&-:"1 @ (1 4&{"1) @ > @ (4&{"1)

WPTYPE=: 0 : 0
pc wptype closeok;pn "File Types";
xywh 1 6 175 94;cc g0 groupbox rightmove bottommove;cn "";
xywh 10 13 62 9;cc label static;cn "Description";
xywh 57 13 62 9;cc label static;cn "Type Filters";
xywh 6 22 165 62;cc types listbox ws_hscroll ws_vscroll rightmove bottommove;
xywh 10 85 38 11;cc up button topmove bottommove;cn "Move &Up";
xywh 51 85 38 11;cc down button topmove bottommove;cn "Move Do&wn";
xywh 133 85 38 11;cc delete button topmove bottommove;cn "&Delete";
xywh 1 103 175 51;cc g0 groupbox topmove rightmove bottommove;cn "";
xywh 9 111 36 9;cc label static topmove bottommove;cn "Description:";
xywh 42 110 129 11;cc desc edit ws_border es_autohscroll topmove rightmove bottommove;
xywh 9 123 36 9;cc label static topmove bottommove;cn "Type Filters:";
xywh 42 122 129 11;cc filter edit ws_border es_autohscroll topmove rightmove bottommove;
xywh 10 139 38 11;cc addtop button topmove bottommove;cn "Add at &top";
xywh 51 139 38 11;cc addend button topmove bottommove;cn "Add at &end";
xywh 133 139 38 11;cc replace button topmove bottommove;cn "&Replace";
xywh 181 10 40 12;cc ok button leftmove rightmove;cn "OK";
xywh 181 25 40 12;cc cancel button leftmove rightmove;cn "Cancel";
pas 4 4;pcenter;
rem form end;
)

wptype_run=: 3 : 0

if. wdifopen 'wptype' do. wd 'psel wptype' return. end.

FIFTYPE=: etype
XFIFFILTERS=: FIFFILTERS

pos=. wd 'psel fif;qformx'
wd WPTYPE
wdcenter pos

wd 'settabstops types 60'
XNDX=: ({."1 XFIFFILTERS) bcfind <FIFTYPE
wptypeshow''
wd 'pshow;'
)
wptypeshow=: 3 : 0
if. #XFIFFILTERS do.
  j=. }. , LF ,. '"' ,. (>{."1 XFIFFILTERS) ,. TAB ,"1 (>{:"1 XFIFFILTERS) ,. '"'
  wd 'set types ', j
  wd 'setselect types ',": XNDX
  wptypeshowdetails''
end.
)

wptypeshowdetails=: 3 : 0
if. 0 <: XNDX do.
  'j k'=. XNDX { XFIFFILTERS
  wd 'set desc *',j
  wd 'set filter *',k
end.
)
wptype_cancel_button=: 3 : 0
wd :: [ 'pclose;'
wd 'psel fif'
)

wptype_cancel=: wptype_cancel_button
wptype_up_button=: 3 : 0
if. #ndx=. ". types_select do.
  ind=. ndx, ndx1=. 0 >. ndx-1
  XFIFFILTERS=: (|.ind{XFIFFILTERS) ind} XFIFFILTERS
  XNDX=: ndx1
  wptypeshow''
end.
)
wptype_down_button=: 3 : 0
if. #ndx=. ". types_select do.
  ind=. ndx, ndx1=. (<:#XFIFFILTERS) <. ndx+1
  XFIFFILTERS=: (|.ind{XFIFFILTERS) ind} XFIFFILTERS
  XNDX=: ndx1
  wptypeshow''
end.
)
wptype_delete_button=: 3 : 0
if. #ndx=. ". types_select do.
  msg=. 'OK to delete: ',>{.ndx{XFIFFILTERS
  if. 0=2 fquery msg do.
    XFIFFILTERS=: XFIFFILTERS #~ -. (i.#XFIFFILTERS) e. ndx
    XNDX=: (<:#XFIFFILTERS) <. ndx
    wptypeshow''
  end.
end.
)
wptype_types_select=: 3 : 0
XNDX=: ". types_select
wptypeshowdetails''
)
wptype_addtop_button=: 3 : 0
if. wptype_addcheck'' do.
  XFIFFILTERS=: ~. (desc;filter),XFIFFILTERS
  XNDX=: 0
  wptypeshow''
end.
)
wptype_addend_button=: 3 : 0
if. wptype_addcheck'' do.
  XFIFFILTERS=: ~. XFIFFILTERS, desc;filter
  XNDX=: ({."1 XFIFFILTERS) i. <desc
  wptypeshow''
end.
)
wptype_replace_button=: 3 : 0
if. wptype_addcheck'' do.
  if. #ndx=. ". types_select do.
    XFIFFILTERS=: (desc;filter) ndx} XFIFFILTERS
    XNDX=: ndx
    wptypeshow''
  end.
end.
)
wptype_addcheck=: 3 : 0
r=. 0
if. 0=#desc do.
  finfo 'Enter the description'
elseif. 20<#desc do.
  finfo 'Descriptions may not exceed 20 characters'
elseif. 0=#filter do.
  finfo 'Enter the type filters'
elseif. 1 do. r=. 1
end.
r
)
wptypeclose=: 3 : 0
if. #ndx=. ". types_select do.
  FIFTYPE=: 0 pick ndx{FIFFILTERS
end.
wd 'psel fif'
wd 'set etype ',todelim {."1 FIFFILTERS
wd 'setselect etype ',": ({."1 FIFFILTERS) i. <FIFTYPE
wd 'psel wptype;pclose;psel fif'
)
wptypesavenew=: 3 : 0
FIFFILTERS=: XFIFFILTERS
fifputconfig''
if. -. (<FIFTYPE) e. {."1 FIFFILTERS do.
  FIFTYPE=: 0 pick {. FIFFILTERS
end.
)
wptype_close=: 3 : 0
if. -. FIFFILTERS -: XFIFFILTERS do.
  if. 0=2 fquery 'OK to save file filters' do.
    wptypesavenew''
  end.
end.
wptypeclose''
)

wptype_ok_button=: wptypeclose @ wptypesavenew

TABGROUPS=: ;:'fifp fiff fifh'
PTOP=: 0
TABNDX=: _1   
FIFCONTEXT=: 0 : 0
any
name only
assigned
assigned globally
assigned locally
in comment text
outside comment text
in quoted string
outside quoted string
)
FIF=: 0 : 0
pc fif;
menupop "&Options";
menu print "&Print" "" "" "";
menusep ;
menu clip "&Clip" "" "" "";
menusep ;
menu refresh "&Refresh Project" "" "" "";
menusep ;
menu help "&Help" "" "" "";
menusep ;
menu exit "E&xit" "" "" "";
menupopz;
xywh 2 2 255 91;cc tabs tab rightmove;
xywh 229 2 28 11;cc ptop checkbox leftmove rightmove;cn "&Top";
xywh 0 94 259 80;cc found editm ws_hscroll ws_vscroll rightmove bottommove;
pas 0 0;pcenter;
rem form end;
)
fif_run=: 3 : 0
getfoldernames''
FIFFOLDERS=: 2 {."1 USERFOLDERS
if. wdifopen 'fif' do.
  wd 'psel fif'
  id=. TABNDX pick 'pwhat';'what';'hwhat'
  FIFWHAT=: id wdget wdqd''
  wd 'pshow;pactive'
  fifselwhat''
  return.
end.
font=. FIXFONT
fif_rundef''

wd FIF
wd 'pn *',FIFTITLE
if. NOPROJ do.
  wd 'set tabs "Folders" "Help"'
  wd 'creategroup tabs'
  wd FIFF
  wd FIFH
else.
  wd 'set tabs "Project" "Folders" "Help"'
  wd 'creategroup tabs'
  wd FIFP
  wd FIFF
  wd FIFH
end.
wd 'creategroup'
wd 'setfont found ',font

if. TABNDX = _1 do. TABNDX=: NOPROJ end.
fifsettab TABNDX

fifshow''
fifselwhat''
if. IFUNIX do. wd'setshow ptop 0' end.
wd 'set ptop ',":PTOP
wpset_j_ 'fif'
wd 'pshow;ptop ',":PTOP
)
fif_rundef=: 3 : 0
0!:0 :: ] <FIFCFGFILE
fifgetfilters''
NB. JHS FIFTYPE=: 0 pick {.FIFFILTERS,a:
NB. JHS FIFDIR=: 0 pick {.FIFFOLDERS,a:
FIFHELP=: 'Dictionary' 
fifgetproj''
NOPROJ=: 0=#PROJECTFILE
)
fifselwhat=: 3 : 0
what=. TABNDX pick 'pwhat';'what';'hwhat'
wd 'setselect ',what,' 0 ',":#FIFWHAT
wd 'setfocus ',what
)
fifread=: 3 : 0
select. TABNDX
case. 0 do.
  FIFWHAT=: pwhat
  FIFCONTEXTNDX=: ". pecontext_select
  FIFCASE=: ". pcase
  FIFNAMEONLY=: ". pnameonly
  FIFREGEX=: ". pregex
case. 1 do.
  FIFWHAT=: what
  FIFTYPE=: etype
  FIFDIR=: edir
  FIFCONTEXTNDX=: ". econtext_select
  FIFCASE=: ". case
  FIFNAMEONLY=: ". nameonly
  FIFREGEX=: ". regex
  FIFSUBDIR=: ". subdir
case. 2 do.
  FIFWHAT=: hwhat
  FIFCONTEXTNDX=: ". hcontext_select
  FIFHELP=: hlook
  FIFCASE=: ". hcase
  FIFREGEX=: ". hregex
end.
)
fifshow=: 3 : 0
select. TABNDX
case. 0 do.
  wd 'set pwhat *',FIFWHAT
  wd 'set pecontext *',FIFCONTEXT
  wd 'setselect pecontext ',":FIFCONTEXTNDX
  wd 'set pcase ',":FIFCASE
  wd 'set pnameonly ',":FIFNAMEONLY
  wd 'set pregex ',":FIFREGEX
  fifpshow''
  wd 'setfocus pwhat'
case. 1 do.
  wd 'set what *',FIFWHAT
  wd 'set econtext *',FIFCONTEXT
  wd 'setselect econtext ',":FIFCONTEXTNDX
  wd 'set case ',":FIFCASE
  wd 'set nameonly ',":FIFNAMEONLY
  wd 'set regex ',":FIFREGEX
  wd 'set subdir ',":FIFSUBDIR
  fiffshow''
  wd 'setfocus what'
case. 2 do.
  wd 'set hwhat *',FIFWHAT
  wd 'set hcontext *',FIFCONTEXT
  wd 'setselect hcontext ',":FIFCONTEXTNDX
  wd 'set hcase ',":FIFCASE
  wd 'set hregex ',":FIFREGEX
  wd 'set hsubdir ',":FIFSUBDIR
  fifhshow''
  wd 'setfocus hwhat'
end.
)
fif_tabs_button=: 3 : 0
fifsettab NOPROJ + ".tabs_select
)
fifsettab=: 3 : 0
if. TABNDX ~: y do.
  fifread''
  TABNDX=: y
end.
sel=. TABNDX = i. 3
if. NOPROJ=0 do.
  wd 'setshow ',(0 pick TABGROUPS),' ', ": 0 pick sel
end.
wd 'setshow ',(1 pick TABGROUPS),' ', ": 1 pick sel
wd 'setshow ',(2 pick TABGROUPS),' ', ": 2 pick sel
wd 'setselect tabs ',":TABNDX-NOPROJ
fifshow''
)
fifshowfind=: 3 : 0
if. 0=#FIFFOUND do. return. end.
if. FIFNAMEONLY do.
  if. 0=#FIFFOUNDFILES do.
    j=. 0, +/\ 2 + }: #&> FIFLINES
    FIFFOUNDFILES=: }: ; j {<;.2 FIFFOUND,LF
  end.
  wd 'set found *',FIFFOUNDFILES
else.
  if. FIFMAXMSG < FIFMAXSIZE < #FIFFOUND do.
    msg=. 'Found text is too large to display in full.'
    msg=. msg,LF,LF,'Save full text to file temp\fif.txt?'
    if. 0=2 fquery msg do.
      FIFFOUND 1!:2 <jpath '~temp\fif.txt'
    end.
    FIFFOUND=: (FIFMAXSIZE{.FIFFOUND),'...'
  end.
  FIFMAXMSG=: 1
  wd 'set found *',FIFFOUND
end.
)
fif_close=: 3 : 0
f=. wd :: ]
f 'psel wpinsert;pclose'
f 'psel wptype;pclose'
try.
  wd 'psel fif'
  wpsave_j_ 'fif'
  wd 'pclose'
catch. end.
)
fif_find_button=: 3 : 0
tag=. TABNDX { 'pfh'
('fif',tag,'_find_button')~''
)
fif_ptop_button=: 3 : 0
PTOP=: ".ptop
wd 'ptop ',ptop
)
fif_cancel=: fif_cancel_button=: fif_close
fif_exit_button=: fif_close
fif_insert_button=: wpinsert_run
fif_enter=: fif_what_button=: fif_find_button
fif_edir_button=: fif_etype_button=: fif_econtext_button=: fif_find_button
fif_clip_button=: 3 : 'wdclipwrite found'
fif_help_button=: 3 : 0
htmlhelp_j_ 'user\find_in_files.htm'
)
fif_open_button=: 3 : 0
if. 0=#found do. return. end.
pos=. {.".found_select
if. FIFNAMEONLY do.
  row=. hit=. 0
  blk=. (00,I. FIFFOUNDFILES=LF) groupndx pos
else.
  line=. (00,I. FIFFOUND=LF) groupndx pos
  dat=. <;._2 FIFFOUND,LF
  datx=. 0, 1+I. dat=<''
  blk=. datx groupndx line
  rws=. blk pick FIFLINES
  hit=. blk pick FIFHITS
  ind=. line-1+blk{datx
  if. ind < 0 do.
    row=. hit=. 0
  else.
    ind=. ind <. <:#rws
    row=. ind { rws
    hit=. ind { hit
  end.
end.
file=. blk pick FIFMSK # FIFFILES
(row,hit) fopenfile file
)
fif_print_button=: 3 : 0
if. 0=#found do. finfo 'Nothing to print'
else. prints_j_ found end.
)
fif_refresh_button=: 3 : 0
fif_rundef''
fifshow''
)
fif=: fif_run

FIFF=: 0 : 0
pc fiff;
xywh 3 6 37 11;cc s0 static ss_right;cn "Fi&nd what:";
xywh 43 5 150 12;cc what edit ws_border rightmove;
xywh 3 20 37 11;cc s1 static ss_right;cn "In &context:";
xywh 43 19 93 200;cc econtext combolist cbs_autohscroll rightmove;
xywh 3 34 37 11;cc s2 static ss_right;cn "File ty&pe:";
xywh 43 33 93 200;cc etype combodrop ws_vscroll cbs_autohscroll rightmove;
xywh 3 48 37 11;cc s3 static ss_right;cn "In fo&lders:";
xywh 43 47 93 200;cc edir combodrop ws_vscroll cbs_autohscroll rightmove;
xywh 138 34 11 10;cc selecttype button leftmove rightmove;cn ">>";
xywh 153 22 44 11;cc regex checkbox leftmove rightmove;cn "&Regex";
xywh 153 35 76 11;cc case checkbox leftmove rightmove;cn "&Match case";
xywh 153 49 94 11;cc subdir checkbox leftmove rightmove;cn "&Search sub&directories";
xywh 210 4 40 12;cc insert button leftmove rightmove;cn "&Insert...";
xywh 210 17 40 12;cc find button bs_defpushbutton leftmove rightmove;cn "&Find";
xywh 4 63 102 12;cc nameonly checkbox;cn "File n&ames only";
xywh 169 62 40 12;cc open button leftmove rightmove;cn "&Open";
xywh 210 62 40 12;cc cancel button leftmove rightmove;cn "Cancel";
pas 0 0;
rem form end;
)
fiff_cancel_button=: fif_cancel_button
fiff_open_button=: fif_open_button
fiff_insert_button=: fif_insert_button

fiff_edir_button=: fiff_etype_button=: fiff_econtext_button=: fiff_find_button
fiff_enter=: fiff_what_button=: fiff_find_button
fiffshow=: 3 : 0
wd 'set etype ',todelim j=. {."1 FIFFILTERS
wd 'setselect etype ',": j i. <FIFTYPE
wd 'set edir ', todelim j=. {."1 FIFFOLDERS
if. (#j) > ndx=. j i. <FIFDIR do.
  wd 'setselect edir ',": ndx
end.
)
fiff_find_button=: 3 : 0
fifread''
if. #FIFWHAT do.
  dat=. ffss''
  if. #dat do.
    if. # 0 pick dat do.
      'FIFFOUND FIFLINES FIFHITS FIFMSK FIFFILES'=: dat
      FIFFOUNDFILES=: ''
      FIFMAXMSG=: 0
      if. FIFTYPE -: 'HTML' do.
        fifshowhtml''
      else.
        fifshowfind''
      end.
    elseif. 0 e. # 2 pick dat do.
      finfo 'No match found'
    elseif. 1 do.
      finfo 'No files found'
    end.
  else.
    finfo 'No match found'
  end.
else.
 finfo 'Nothing to find'
end.
)
fiff_nameonly_button=: 3 : 0
FIFNAMEONLY=: ". nameonly
fifshowfind ''
)
fiff_open_button=: 3 : 0
if. FIFTYPE -: 'HTML' do.
  fifh_open_button ''
else.
  fif_open_button ''
end.
)
fiff_selecttype_button=: wptype_run
FIFHELPS=: '' NB. not suppported in JHS
FIFH=: 0 : 0
pc fifh;
xywh 3 7 37 11;cc s0 static ss_right;cn "Fi&nd what:";
xywh 43 5 150 12;cc hwhat edit ws_border rightmove;
xywh 3 20 37 11;cc s1 static ss_right;cn "In &context:";
xywh 43 19 93 200;cc hcontext combolist cbs_autohscroll rightmove;
xywh 3 34 37 11;cc s3 static ss_right;cn "Look &In:";
xywh 43 33 93 200;cc hlook combolist ws_vscroll rightmove;
xywh 153 22 44 11;cc hregex checkbox leftmove rightmove;cn "&Regex";
xywh 153 35 68 11;cc hcase checkbox leftmove rightmove;cn "&Match case";
xywh 153 49 95 11;cc hsubdir checkbox leftmove rightmove;cn "&Search sub&directories";
xywh 210 4 40 12;cc insert button leftmove rightmove;cn "&Insert...";
xywh 210 17 40 12;cc find button bs_defpushbutton leftmove rightmove;cn "&Find";
xywh 169 62 40 12;cc open button leftmove rightmove;cn "&Open";
xywh 210 62 40 12;cc cancel button leftmove rightmove;cn "Cancel";
pas 0 0;pcenter;
rem form end;
)
fifh_cancel_button=: fif_cancel_button
fifh_insert_button=: fif_insert_button
fifh_open_button=: fif_open_button
fifh_enter=: fifh_hwhat_button=: fifh_find_button
fifh_hcontext_button=: fifh_find_button
fifh_find_button=: 3 : 0
fifread''
if. #FIFWHAT do.
  dat=. ffss''
  if. #dat do.
    if. # 0 pick dat do.
      'FIFFOUND FIFLINES FIFHITS FIFMSK FIFFILES'=: dat
      FIFFOUNDFILES=: ''
      FIFMAXMSG=: 0
      fifshowhtml''
    elseif. 0 e. # 2 pick dat do.
      finfo 'No match found'
    elseif. 1 do.
      finfo 'No files found'
    end.
  else.
    finfo 'No match found'
  end.
end.
)
fifh_open_button=: 3 : 0
require '~system\extras\util\browser.ijs'
launch_jbrowser_ FIFHTMFILE
)
fifhshow=: 3 : 0
wd 'set hlook ', todelim j=. {."1 FIFHELPS
wd 'setselect hlook ',": j i. <FIFHELP
)

FIFPNMS=: ; < @ (' '&,) ;._2 (0 : 0)
PID
PIDPATH
PIDTREE
PJFILES
PROJECTFILE
XFILES
XLIBS
XOTHERFILES
XPROJECTRECENT
PIDPATH
)

(FIFPNMS)=: a:
FIFP=: 0 : 0
pc fifp;
xywh 3 6 37 11;cc s0 static ss_right;cn "Fi&nd what:";
xywh 43 5 150 12;cc pwhat edit ws_border rightmove;
xywh 3 20 37 11;cc s1 static ss_right;cn "In &context:";
xywh 43 19 93 200;cc pecontext combolist cbs_autohscroll rightmove;
xywh 3 34 37 11;cc s2 static ss_right;cn "&Project:";
xywh 43 33 93 200;cc eproj combolist ws_vscroll rightmove;
xywh 3 47 37 11;cc s3 static ss_right;cn "Look &In:";
xywh 43 47 93 200;cc elook combolist ws_vscroll rightmove;
xywh 153 22 44 11;cc pregex checkbox leftmove rightmove;cn "&Regex";
xywh 153 35 64 11;cc pcase checkbox leftmove rightmove;cn "&Match case";
xywh 210 4 40 12;cc insert button leftmove rightmove;cn "&Insert...";
xywh 210 17 40 12;cc find button bs_defpushbutton leftmove rightmove;cn "&Find";
xywh 4 63 103 12;cc pnameonly checkbox;cn "File n&ames only";
xywh 169 62 40 12;cc open button leftmove rightmove;cn "&Open";
xywh 210 62 40 12;cc cancel button leftmove rightmove;cn "Cancel";
pas 2 0;pcenter;
rem form end;
)
fifp_cancel_button=: fif_cancel_button
fifp_open_button=: fif_open_button
fifp_insert_button=: fif_insert_button
fifp_open_button=: fif_open_button

fifp_pecontext_button=: fifp_eproj_button=: fifp_elook_button=: fifp_find_button
fifp_enter=: fifp_pwhat_button=: fifp_find_button
fifp_find_button=: 3 : 0
fifread''
if. #FIFWHAT do.
  dat=. ffss''
  if. #dat do.
    if. # 0 pick dat do.
      'FIFFOUND FIFLINES FIFHITS FIFMSK FIFFILES'=: dat
      FIFFOUNDFILES=: ''
      FIFMAXMSG=: 0
      fifshowfind''
    elseif. 0 e. # 2 pick dat do.
      finfo 'No match found'
    elseif. 1 do.
      finfo 'No files found'
    end.
  else.
    finfo 'No match found'
  end.
end.
)
fifgetproj=: 3 : 0

if. (<'jproject') e. 18!:1 [ 0 do.
  if. 0=4!:0 <'PJFILES_jproject_' do.
    (FIFPNMS)=: a:
    try.
      (FIFPNMS)=: do_jproject_ '<@". ;._1 ''',FIFPNMS,''''
    catch. end.
    fiflower''
    return.
  end.
end.

PROJECTRECENT=: ''
projcfgread''
PROJECTFILE=: jpath 0 pick XPROJECTRECENT,a:

fiflower''

pn=. 0 pick 0 { FIFFOLDERS,a:
r=. 0
if. #PROJECTFILE do. r=. fifpreadtry PROJECTFILE end.
if. r do.
  m=. [ -: #@[ {. ]
  p=. fullname@addfret each {:"1 FIFFOLDERS
  x=. \: # &> p
  p=. x { p
  if. IFWIN do. p=. tolower each p end.
  b=. p m &> <PROJECTFILE
  if. -. 1 e. b do.
  else.
    ndx=. b i. 1
    pn=. 0 pick ndx { x { FIFFOLDERS
  end.
end.
fifppath pn

)
fifpshow=: 3 : 0
if. 0=#PROJECTFILE do.
  wd 'set eproj *'
  wd 'set elook *'
else.
  wd 'set eproj *', tolist pidname each PJFILES
  wd 'setselect eproj ',": PJFILES bcfind <PROJECTFILE
  wd 'set elook *', tolist PIDTREE
  wd 'setselect elook ',": PIDTREE find <PID
end.
)
fifp_eproj_select=: 3 : 0
if. #eproj_select do.
  j=. (".eproj_select) pick PJFILES
  if. j -: PROJECTFILE do. return. end.
  fifpreadtry PROJECTFILE=: jpath j
end.
)
fifp_pnameonly_button=: 3 : 0
FIFNAMEONLY=: ". pnameonly
fifshowfind ''
)
fifpreadtry=: 3 : 0
try.
  fifpread y
  1
catch.
  PROJECTFILE=: ''
  0
end.
)
fifp_elook_select=: 3 : 0
if. elook -: PID do. return. end.
fifppath elook
PROJECTFILE=: jpath 0 pick PJFILES,a:
try. fifpread PROJECTFILE catch. end.
if. #PJFILES do.
  wd 'set eproj *', tolist pidname each PJFILES
  wd 'setselect eproj 0'
else.
  wd 'set eproj *'
end.
)
fifpread=: 3 : 0
'PRIMARYFILES DEVFILES PRIMARYLIBS DEVLIBS OTHERFILES'=. a:
'XFILES XLIBS XOTHERFILES'=: a:
dat=. 1!:1 <y
ndx=. 7 + {. I. 'coclass' E. dat
dat=. ndx }. dat
ndx=. I. '=:' E. dat
dat=. '.' (ndx+1)} dat
0!:100 dat
XFILES=: pr1 PRIMARYFILES,DEVFILES
XLIBS=: pr2 PRIMARYLIBS,DEVLIBS
XOTHERFILES=: pr1 OTHERFILES
)
fifppath=: 3 : 0
PID=: y
PIDPATH=: getpidpath PID
PJFILES=: fullproject each projgetfiles PIDPATH
fiflower''
)

HTMHEADER=: 0 : 0
<html>
<head>
<title>Find in Files</title>
</head>
<body>

)
HTMHELP=: (0 : 0) rplc LF;' '
Search results have been written to file fif.htm in
the temp directory. Click Open to view this file
in your browser, or refresh the page if it is already open.
)
htmfooter=: 3 : 0
t=. LF,'<!--Find in Files ',(timestamp''),'-->'
t=. t,LF,'</body>',LF,'</html>'
)
htmheader=: 3 : 0
t=. HTMHEADER
t=. t, 'Searched for: <code>',y,'</code>',LF
)
htmlcode=: '<code>'&, @ (,&'</code>')
htmlpre=: '<pre>'&, @ (,&'</pre>')
htmlref=: 3 : 0
'name jump'=. 2 $ boxopen y
jump=. (<: jump i. '(') {. jump
jump=. '/' (I. jump=PATHSEP_j_) } jump
jump=. 'file://',jump
'<a href="' , jump , '" target="',TARGET,'">' , name , '</a><br>'
)
htmlstriptags=: 3 : 0
dat=. y
sum=. +/\ ('<' = dat) - '>' = dat
if. *./ sum e. 0 1 do.
  ndx=. dat i. '<'
  hdr=. ndx {. dat
  dat=. ndx }. dat
  if. #dat do.
    dat=. <;._1 dat
    ndx=. 1 + dat i. &> '>'
    dat=. ndx }.each dat
  end.
  hdr, ;dat
else.
  dat
end.
)
fifshowhtml=: 3 : 0
if. 0=#FIFFOUND do. return. end.
if. 0=#FIFFOUNDFILES do.
  j=. 0, +/\ 2 + }: #&> FIFLINES
  FIFFOUNDFILES=: }: ; j {<;.2 FIFFOUND,LF
end.
wd 'set found *',FIFFOUNDFILES

ndx=. 0, +/\ 2 + }: #&> FIFLINES
txt=. <;._2 FIFFOUND,LF
ind=. (i.#txt) -. ndx
ref=. htmlref each ndx { txt
bal=. htmlstriptags each ind { txt
len=. # &> bal
msk=. FIFMAXLINE < len
if. 1 e. msk do.
  inx=. I. msk
  bal=. (FIFMAXLINE truncate each inx { bal) inx } bal
  len=. len <. FIFMAXLINE  
end.
cnt=. +/ len, # &> txt
if. FIFMAXSIZE < cnt do.
  txt=. '<p>Found text is too large - listing file names only.<p>'
  txt=. txt, ; tolist ref
else.
  ref=. '<p>'&, each ref
  txt=. ref ndx } txt
  rep=. tohtml tolist len {.each bal
  rep=. (<;._2 rep,LF) ,each (<'<br>')
  txt=. rep ind } txt
  txt=. ; tolist txt
end.
hdr=. htmheader FIFWHAT
ftr=. htmfooter''
txt=. hdr, txt, ftr
txt fwrites FIFHTMFILE
finfo HTMHELP
)
tohtml=: 3 : 0
txt=. ,y
txt=. txt rplc '&';'&amp;'
txt=. txt rplc '''';'&apos;'
txt=. txt rplc '"';'&quot;'
txt=. txt rplc '<';'&lt;'
txt=. txt rplc '>';'&gt;'
)

NB. hacked overrides to let jhs run fif

fifread=: 3 : 0
''
)

fifshowfind=: 3 : 0
NB. FIFFOUNDFILES=: ''
NB. if. 0=#FIFFOUND do. return. end.
NB. j=. 0, +/\ 2 + }: #&> FIFLINES
NB. FIFFOUNDFILES=: }: ; j {<;.2 FIFFOUND,LF
''
)

finfo=: 3 : 0
FIFINFO=: y
FIFFOUND=: ''
FIFFOUNDFILES=: ''
JHSFOUND=: ''
)

FIFCONTEXT=: 0 : 0
any
name
=: =.
=:
=.
)

jhsfixfl=: 3 : 0
'<br><a href="jijs?mid=open&path=',(jurlencode y),'" target="',TARGET,'">',y,'</a>'
)

jhsfixtxt=: 3 : 0
'<br>',jhfroma y
)

findhelp=: 0 : 0
   find 'what' ; 'where/*.ijs'
   find 'what' ; 'where/*.ijs' ; 'name cs'
options: name assign =: =. cs(case sensitive) nr(no recursion) nl(no lines)
)

find=: 3 : 0
if. 0=L.y do. findhelp return. end.
crx''
'FIFWHAT FIFDIR'=: 2{.y
'FIFCASE FIFSUBDIR FIFNAMEONLY FIFREGX'=: 0 1 1 0
p=. deb >2{y,<''
c=. 0
if. 0~:#p do.
 for_n. <;._1 ' ',p do.
  select. >n
  case. 'name'   do. c=. 1
  case. 'assign' do. c=. 2
  case. '=:'     do. c=. 3
  case. '=.'     do. c=. 4
  case. 'case'   do. FIFCASE=: 1
  case. 'nr'     do. FIFSUBDIR=: 0
  case. 'nl'     do. FIFNAMEONLY=: 0
  case.          do. assert 0['bad option'
  end.
 end.
end. 
FIFCONTEXTNDX=: c
i=. FIFDIR i:'/'
FIFTYPE=: (>:i)}.FIFDIR
FIFDIR=: i{.FIFDIR
JHSFOUNDFILES=: ''
fiff_find_button''
jhtml  '<div contenteditable="false">',(>FIFNAMEONLY{JHSFOUNDFILES;FIFFOUND),'</div'
)
