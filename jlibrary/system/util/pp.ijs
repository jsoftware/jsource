coclass 'jpp'
coinsert 'j'
j=. 'assert. break. case. catch. catchd. catcht. continue. do. else. elseif. end.'
CONTROLS=: ;: j,' fcase. for. if. return. select. try. while. whilst.'
CONTROLX=: 0 0 0 0 0 0 0 0 0 0 _1, 0 1 1 0 1 1 1 1
CONTROLN=: ;: 'case. catch. catchd. catcht. do. else. elseif. end. fcase.'
CONTROLB=: ;: 'for. if. select. try. while. whilst.'
CONTROLM=: CONTROLN -. <'end.'

FORMEND=: 'rem form end;';,')'
CONTS=: ';xywh';';cc';';cn'
adverbs=: '~ / \ /. \. } b. f. M. t. t:'

arguments=: 'm n u v x y'

t=. '= < <. <: > >. >: _: + +. +: * *. *: - -. -: % %. %: ^ ^.'
t=. t,' $ $. $: ~. ~: | |. |: , ,. ,: ; ;: # #. #: ! /: \: [ [: ]'
t=. t,' { {. {: {:: }. }: ". ": ? ?.'
t=. t,' A. c. C. e. E. i. i: I. j. L. o. p. p: q: r. s: u: x:'
t=. t,' _9: _8: _7: _6: _5: _4: _3: _2: _1:'
t=. t,' 0: 1: 2: 3: 4: 5: 6: 7: 8: 9:'
verbs=: t

t=. '^: . .. .: :  :. :: ;. !. !: " ` `: @ @. @: & &. &: &.:'
t=. t,' d. D. D: H. L: S: T.'
conjunctions=: t

t=. 'assert. break. case. catch. catchd. catcht. continue. do.'
t=. t,' else. elseif. end. fcase. for. goto. if. label.'
t=. t,' return. select. throw. trap. try. while. whilst.'
control_words=: t

nouns=: 'a. a:'

j=. ;: adverbs,' ',verbs,' ',conjunctions,' ',control_words,' ',nouns
SystemDefs=: j,;:'=. =:'
EXPDEFINE=: <@;: ;._2 (0 : 0)
1 : 0
2 : 0
3 : 0
4 : 0
1 define
2 define
3 define
4 define
adverb : 0
conjunction : 0
verb : 0
monad : 0
dyad : 0
adverb define
conjunction define
verb define
monad define
dyad define
)
NOUNDEFINE=: <@;: ;._2 '\' -.~ (0 : 0)
0 \: 0
noun \: 0
0 \define
noun \define
)
findfor=: 'for_'&-: @ (4&{.) *. ('.'&=) @ {:
info=: mbinfo @ ('Lint'&;)
lastones=: > (}. , 0:)
tolist=: }.@;@:(LF&,@,@":&.>)
findcontrols=: (1: e. (CONTS"_ (1: e. E.) &> <)) &>
firstones=: > (0 , }:)
maskselectside=: +./\ *. +./\.
notquotes=: (+: ~:/\)@(''''&=)
notcomments=: ([: +./\. (=&' ') +: [: +./\ 'NB.'&E. > ~:/\@(''''&=))
notqc=: (notquotes *. notcomments) f.
debq=: #~ (+. 1: |. (> </\))@(notquotes <: ' '&~:)
checkmulti=: 3 : 0
tok=. words @ (#~ notqc) each y
bgn=. masknoun1 &> tok
end=. tok=<;:')'
end=. 2 }. ; (1 0,bgn) < @ (</\) ;. 1 [ 0 1,end
if. (+/bgn) = +/end do.
  nounmask=. (+.~:/\) bgn +. end
  bgn=. nounmask < maskexp1 &> tok
  end=. nounmask < tok=<;:')'
  if. bgn pairup end do. 0 return. end.
end.
lvl=. (+/\bgn) - +/\end
if. 2 e. lvl do.
  lin=. (<:(bgn#lvl) i. 2) { I. bgn
  msg=. lin pick y
elseif. 1 = {:lvl do.
  lin=. 1 i.~ *./\.1 = lvl
  msg=. 'Definition not completed'
elseif. 1 do.
  lin=. lvl i. _1
  msg=. 'Unmatched closing paren'
end.
msg=. 'Could not match begin and end of multi-line definition:',LF,LF,msg
lin;msg
)
commentline=: 3 : 0
line=. y
ndx=. (y e. ' ',TAB) i. 0
if. -. 'NB. ' -: 4 {. ndx }. line do.
  line return.
end.
pre=. ndx {. line
len=. 57 - +/ 1 + 3 * pre = TAB
if. (,'=') -: ~. (4+ndx) }. line do.
  line=. pre,'NB. ',len#'='
elseif. (,'-') -: ~. (4+ndx) }. line do.
  line=. pre,'NB. ',len#'-'
end.
line
)
dellb=: #~ +./\ @: -. @ e.&(' ',TAB)
deltb=: #~ +./\.@: -. @ e.&(' ',TAB)
indent1=: 3 : 0
tok=. y
x=. I. findfor &> tok
tok=. (<'for.') x} tok
+/ (CONTROLX,0) {~ CONTROLS i. tok
)
maskexps=: 3 : 0
tok=. words @ (#~ notqc) each y
bgn=. maskexp1 &> tok
end=. tok=<;:')'
end=. 2 }. ; (1 0,bgn) < @ (</\) ;. 1 [ 0 1,end
~: /\. bgn +. end
)
maskexp1=: 3 : 0
1 e. EXPDEFINE 1&e.@E. &> <y
)
masknouns=: 3 : 0
tok=. words @ (#~ notqc) each y
bgn=. masknoun1 &> tok
if. -. 1 e. bgn do. return. end.
end=. tok = <;:')'
end=. 2 }. ; (1 0,bgn) < @ (</\) ;. 1 [ 0 1,end
~: /\. bgn +. end
)
masknoun1=: 3 : 0
if. 0=#y do. 0 return. end.
if. 1 e. NOUNDEFINE 1&e.@E. &> <,y do. 1 return. end.
if. (<'Note') ~: {.y do. 0 return. end.
if. (,<'Note') -: y do. 1 return. end.
if. -. (#y) e. 2 3 do. 0 return. end.
('NB.'-:3{.2 pick y,<'NB.') > (1{y) e. SystemDefs
)
maskselect=: 4 : 0
msk=. x
in=. y
ndx=. msk i. 1
if. ndx=#msk do. msk return. end.
in=. msk <;.1 in
(ndx#0) ,; maskselect1 each in
)

maskselect1=: 0 , [: *./\ }. >: {.
pairup=: 4 : 0
r=. +/\x - y
*./ (0={:r), r e. 0 1
)
remspaces=: 3 : 0
msk=. notcomments y
(debq msk#y), (-.msk)#y
)
spacing=: 3 : 0
in=. 0
bgn=. 0
msk=. notcomments y
txt=. msk#y
com=. (-.msk)#y

if. #txt do.
  tok=. words txt
  if. tok -: 0 do. return. end.
  if. #tok do.
    in=. indent1 tok
    bgn=. ({.tok) e. CONTROLN
    if. 1=#tok do.
      txt=. ;tok
    else.
      txt=. spacing1 dlb txt
    end.
  else.
    txt=. ''
  end.
else.
  com=. dlb com
end.

in;bgn;<<txt,com
)
words=: 7&u:&.>@:;:@(8&u:) :: 0:
f=. #~ (=&' ') *: 1: |. notquotes *. '=:'&E. +. '=.'&E.
noblankbefore=: f f. ^: _

f=. #~ 1: + [: j. (1: |. =&' ') < _1: |. notquotes *. '=:'&E. +. '=.'&E.
blankafter=: f f. ^: _
spacing1=: blankafter @ noblankbefore
pp=: 3 : 0
files=. boxxopen y
res=. (#files) $ 0
for_f. files do.
  s=. pp1 f
  if. s = _1 do. return. end.
  res=. s f_index } res
end.
res
)
pp1=: 3 : 0
old=. freads y
dat=. pplint old
if. 0 = #dat do. 0 return. end.
if. L. dat do.
  'lin msg'=. dat
  msg=. msg, ' in file:',LF,LF, > y
  (0 >. lin - 10) flopen >y
  if. lin do.
    pos=. 1 0 + (+/\LF = toJ old) i. lin + 0 1
    smsetselect pos
  end.
  info msg
  _1 return.
end.
if. dat -: old do.
  0
else.
  1 [ dat fwrites y
end.
)
pplint=: 3 : 0
dat=. ucp y

'fmt wid rms exp sel'=. Format_j_
if. wid=0 do. spc=. TAB else. spc=. wid#' ' end.
dat=. dat -. 26{a.
if. 0 = #dat do. return. end.
dat=. toJ dat
iftermLF=. LF = {:dat
dat=. <;._2 dat, iftermLF }. LF
dat=. deltb each dat
res=. checkmulti dat
if. L.res do. return. end.
nounx=. I. masknouns dat
nouns=. nounx { dat
dat=. a: nounx} dat
dat=. dellb each dat
if. rms do.
  dat=. remspaces each dat
end.
indat=. spacing each dat
if. (<0) e. indat do.
  lin=. indat i. <0
  txt=. lin pick dat
  cnt=. +/'''' = txt {.~ ('NB.' E. txt) i. 1
  if. 2 | cnt do.
    msg=. 'Mismatched quotes'
  else.
    msg=. 'Could not parse line'
  end.
  lin;msg
  return.
end.

'in begin dat'=. |: > indat
if. 0 ~: +/ in do.
  ins=. +/\ in
  if. _1 e. ins do.
    lin=. ins i. _1
    msg=. 'Unmatched end of control block'
  else.
    msk=. (dat = <,')') *. ins > 0
    if. 1 e. msk do.
      lin=. msk i. 1
      msg=. 'Unmatched start of control block'
    else.
      lin=. 0
      msg=. 'Mismatched control words'
    end.
  end.
  lin;msg return.
end.
res=. ppval dat
if. -. res -: 0 do. return. end.
if. -. fmt do. '' return. end.
in=. +/\ in
ins=. _1 |. in
ins=. 0 >. ins - begin
cmt=. 'NB.'&-: @ (3&{.)
ins=. ins * -. cmt &> dat
dat=. ins (([ # spc"_),]) each dat
if. sel do.
  msk=. (<'select.') = {. @ words &> dat
  msk=. msk maskselect in
  dat=. msk (([ # spc"_),]) each dat
end.
if. exp do.
  msk=. (dat=<,')') < maskexps dat
  dat=. msk (([ # spc"_),]) each dat
end.
dat=. commentline each dat
dat=. nouns nounx } dat
dat=. ; dat ,each LF
dat=. (- -.iftermLF) }. dat

utf8 dat
)
ppval=: 3 : 0
dat=. words each y
pos=. <: +/\ # &> dat
dat=. ; dat
bgn=. (dat e. CONTROLB) +. findfor &> dat
end=. dat = <'end.'
lvl=. +/\bgn-end
if. _1 e. lvl do.
  lin=. pos I. lvl i. _1
  lin;'Unmatched control end' return.
end.
if. -. 0 = {: lvl do.
  lin=. pos I. lvl i: 1
  lin;'Unmatched control begin' return.
end.
if. bgn = #dat do. 0 return. end.
while. max=. >./ lvl do.
  b=. max = lvl
  b1=. _1 |. b
  ndx=. (,1+{:) I. b > +./\ b1 > b
  res=. ppval1 ndx{dat
  if. res -: 0 do.
    dat=. (<'') ndx}dat
    lvl=. (max-1) ndx} lvl
  else.
    'hit msg'=. res
    lin=. pos I. hit + {. ndx
    lin;msg
    return.
  end.
end.
0
)
ppval1=: 3 : 0
dat=. y
select. > {. dat
case. 'if.' do.
  b=. 0 = +/ dat e. CONTROLM -. ;: 'else. elseif. do.'
  e0=. +/ dat = <'else.'
  e1=. +/ dat = <'elseif.'
  b=. b *. (2 > e0) *. 0 = e0 *. e1
  b=. b *. (+/ dat = <'do.') = 1 + e1
  if. e1 do.
    ix=. I. dat = <'elseif.'
    dx=. }. I. dat = <'do.'
    b=. b *. (#ix) = #dx
    if. b do.
      b=. b *. (i.@#-:/:) ,ix,.dx
    end.
  end.
case. 'select.' do.
  b=. 0 = +/ dat e. CONTROLM -. ;: 'case. fcase. do.'
  ix=. I. dat e. ;: 'case. fcase.'
  dx=. I. dat = <'do.'
  b=. b *. (#ix) = #dx
  if. b do.
    b=. b *. (i.@#-:/:) ,ix,.dx
  end.
case. 'try.' do.
  c=. ;: 'catch. catchd. catcht.'
  b=. (1 e. dat e. c) *. 0 = +/ dat e. CONTROLM -. c
case. 'while.';'whilst.' do.
  b=. (1 = +/ dat = <'do.') *. 0 = +/ dat e. CONTROLM -. <'do.'
case. do.
  b=. 1 = +/ dat = <'do.'
  b=. b *. 0 = +/ dat e. CONTROLM -. <'do.'
end.
if. b do. 0 return. end.
0;'Unmatched control words'
)
pplintqt=: 3 : 0
res=. pplint y
if. (0=#res) +. res-:y do. '' return. end.
if. 0=L.res do. '0',res return. end.
'line msg'=. res
'1',(":line),' ',msg
)
