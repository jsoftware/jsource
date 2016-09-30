NB. base class for ctag

coclass 'ctag'

initdone=: 0
tagtags=: tagfiles=: taglines=: ''
Tags=: ''               NB. default Tags files
absolutepath=: 1        NB. 0 - do not resolve absolute path
NB. ctag definition

create=: destroy=: [:    NB. abstract class


NB. cleartags
NB. =========================================================
cleartags=: 3 : 0
initdone=: 0
tagtags=: tagfiles=: taglines=: ''
EMPTY
)

NB. loadtags
NB. =========================================================
loadtags=: 3 : 0
if. #y do.
  parsetags"0 jpath&.> boxopen y
else.
  if. Tags do. parsetags"0 jpath&.> boxopen Tags end.
end.
initdone=: 1
EMPTY
)

NB. =========================================================
NB. parsetags  parse tags as segmented string for memory efficiency
parsetags=: 3 : 0
y=. '/' (I.'\'=y)}y=. >y
a=. freads y
if. 4>#a do. EMPTY return. end.
tag=. ; ,&({.a.)&.>@{.@(<;._2)@(,&TAB);._2 a
if. 0=#tag do. EMPTY return. end.
if. 0=#tagtags do. tagtags=: tagfiles=: taglines=: ,{.a. end.
tagtags=: tagtags, tag
if. absolutepath *. y-.@-:pf=. ({.~ i:&'/')y do.
  pf=. pf,'/'
  tagfiles=: tagfiles, b=. jpathsep ; ,&({.a.)@(pf&,)&.>@(1&{)@(<;._2)@(,&TAB);._2 a
else.
  tagfiles=: tagfiles, b=. jpathsep ; ,&({.a.)&.>@(1&{)@(<;._2)@(,&TAB);._2 a
end.
taglines=: taglines, ; <@(,&({.a.))@parseline@;@:(2&}.)@(<;.2)@(,&TAB);._2 a
EMPTY
)

NB. =========================================================
parseline=: 3 : 0
if. '/' = {.y do.
  if. #r=. I. ('/;"',TAB) E. y do.
    z=. }.({.r){.y
  else.
    z=. ,'1'
  end.
elseif. '0123456789' e.~ {.y do.
  if. #r=. I. (';"',TAB) E. y do.
    z=. ({.r){.y
  else.
    z=. ,'1'
  end.
elseif. do.
  z=. ,'1'
end.
z
)

NB. find tags

NB. =========================================================
NB. y string or boxed list of tags
NB. x (default 0)  if x=1 then include partial matches
tagtag=: 0&$: : (4 : 0)
if. 0=#y=. boxxopen y do. 0$0 return. end.
if. 0=initdone do. loadtags'' end.
if. 0=#tagtags do. 0$0 return. end.
res=. 0$0
for_t. y do.
  if. 1 -.@e. r=. (({.a.),(>t),(0=x)#{.a.) E. tagtags do. continue. end.
  res=. res, (I.tagtags={.a.) i. I.r
end.
~.res
)

NB. tag name from index
tagtagname=: 3 : 0
'a b'=. (y+i.2){r=. I.tagtags={.a.
(}.a+i.b-a){tagtags
)

NB. tag file from index
tagfile=: 3 : 0
'a b'=. (y+i.2){r=. I.tagfiles={.a.
if. 1 e. msk=. '/./' E. r=. (}.a+i.b-a){tagfiles do.
  r=. r #~ -. +./ 0 _1 |."0 1 msk          NB.  simplify /./ to /
end.
r
)

NB. tag line from index
tagline=: 3 : 0
'a b'=. (y+i.2){r=. I.taglines={.a.
(}.a+i.b-a){taglines
)

NB. tagopen  open in external editor
NB. x (default 0)  index of occurrence as in tagselect
NB. =========================================================
tagopen=: 0&$: : (4 : 0)
if. 0=#n=. tagtag y do. EMPTY return. end.
file=. tagfile idx=. x{n
if. -.fexist <file do.
  smoutput 'not found: ',file
  EMPTY return.
end.
xedit_j_ file ; {.file tagss tagline idx
EMPTY
)

NB. tagselect  list matching tags
NB. x (default 0)  if x=1 then include partial matches
NB. =========================================================
tagselect=: 0&$: : (4 : 0)
if. x do. y=. tagcp y end.
if. 0=#n=. tagtag y do. EMPTY return. end.
file=. <@tagfile"0 n
line=. <@('     '&,)@tagline"0 n
> line (>:2*i.#file)}2#(<"1 ":,.i.#file) ,&.> (<'  ') ,&.> file
)

NB. tagcp   tag completion
NB. return boxed list of partial matches
NB. =========================================================
tagcp=: 3 : 0
if. 0=#n=. 1 tagtag y do. 0$<'' return. end.
/:~ ~. <@tagtagname"0 n
)

NB. user friendly helper verbs

NB. jump to tag directly for only one candidate,
NB. otherwise display all candidates and wait for user input
NB. x (default 0)  if x=1 then include partial matches
NB. =========================================================
ta=: 0&$: : (4 : 0)
if. x do. y=. tagcp y end.
if. #a=. tagselect y do.
  if. 3>#a do.
    tagopen ::0: y
  else.
    smoutput a
    smoutput 'Type number and <Enter> (empty cancels):'
    if. '' -.@-: b=. 0". 1!:1]1 do.
      ({.b) tagopen ::0: y
    end.
  end.
end.
EMPTY
)
NB. do string search
NB. =========================================================
NB. return 0-base line number or 0$0 if nothing done
tagss=: 4 : 0
what=. >y [ f=. boxopen x
ind=. 0$0
if. 0=#what do. ind return. end.
if. '0123456789' e. ~{.what do. <: 0". what return. end.  NB. line number

NB. flag to enable UTF-8 support
rxflag=. RX_OPTIONS_UTF8_jregex_
RX_OPTIONS_UTF8_jregex_=: 1

if. 0=tagss_init what do. ind [ RX_OPTIONS_UTF8_jregex_=: rxflag return. end.

termLF=. , ((0 < #) # LF -. {:)
tagmatches=. {.@{."2 @ rxmatches_jregex_
groupndx=. [: <: I. + e.~

txt=. freads f
if. -. txt -: _1 do.
  ndx=. TAGCOMP tagmatches txt
  if. #ndx do.
    ind=. ~. (0,}:I. txt = LF) groupndx ndx
  end.
end.
rxfree_jregex_ :: 0: TAGCOMP
ind [ RX_OPTIONS_UTF8_jregex_=: rxflag
)

NB. =========================================================
tagss_init=: 3 : 0
NB. no magic except anchors, but \ is \\ in tags file for vi compatibility
anchor1=. '^'={.y [ anchor2=. '$'={:y
y=. ('\/';'/') stringreplace ('\\';'\') stringreplace (anchor1#'^'), '\Q', ((-anchor2)}.anchor1}.y), '\E', (anchor2#'$')
TAGCOMP=: rxcomp_jregex_ :: _1: y
if. TAGCOMP -: _1 do.
  rxfree_jregex_ :: 0: TAGCOMP
  0
else.
  1
end.
)
