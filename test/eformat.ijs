NB. x is (1 if all of main name always needed),(max # characters allowed),(parenthesize w if compound); y is AR
NB. result is string to display, or ... if string too long
eflinAR_j_ =: {{
NB. parse the AR, recursively
if. y -: 0 0 do. '...' return. end.  NB. If no room for formatting, stop looking
'frc max par' =. x
aro =. >y
if. 2 = 3!:0 aro do.   NB. primitive or named entity
  if. (*frc) +. max >: #aro do. aro return. end.  NB. return value if short enough or we want all of it
  (_3 }. aro) , '...' return.
else.
  NB. not prim/name.  Look for other types: noun or modifier execution
  aro1 =. 1{::aro  NB. value of noun, or other ARs
  select. {. aro
  case. ;:'0' do.  NB. noun
    if. 1 < #@$ aro1 do. '...' return. end.  NB. don't try to format ranks>1
    if. 30 < #aro1  do. '...' return. end.  NB. or too many atoms
    if. (3!:0 aro1) e. 32 64 128 do. '...' return. end.  NB. or boxed/extended
    lin =. 5!:5<'aro1'  NB. value is small, take its linrep
    if. max >: #lin do. lin return. end.  NB. return value if short enough
    (_3 }. lin) , '...' return.
  case. ,&.>'234' do.  NB. hook/fork/train
    NB. these cases are not so important because they don't give verb-execution errors
    stgs=.0$a:  NB. list of strings
    for_i. aro1 do.
      stg =. (0 ,~ 0., 0. >. max%(#aro1)-i_index) eflinAR i  NB. collect strings for each AR
      max =. max - #stg [ stgs =. stgs , <stg  NB. don't allow total size to be exceeded
    end.
    NB. We have strings for each component.  If nothing has a display, return '...' unless this is top-level
    if. (frc=0) *. *./ stgs = <'...' do. '...' return. end.
    (')' ,~ '('&,)^:par ;:^:_1 stgs return. 
  case. do.  NB. default: executed A/C
    stgs =. <stg=. (0 >. (<:frc) , max , 1) eflinAR {. aro   NB. get (stg) for AC
    stgs =. stgs ,~ <stg =. (0 >. 0 , 0 ,~  max =. max -#stg) eflinAR {. aro1
    if. 1 < #aro1 do. stgs =. stgs , <stg =. (0 >. 0 , 1 ,~  max =. max -#stg) eflinAR {: aro1 end.
    if. (frc=0) *. *./ stgs = <'...' do. '...' return. end.
    (')' ,~ '('&,)^:par ; stgs return. 
  end.
end.
}}


NB. y is AR of self
NB. x is the IRS n from "n, if any (63 63 if no IRS)
NB. Result is string form, limited to 30 characters
eflinearself_j_ =: {{
'' eflinearself y
:
NB. create self as an entity; if entity has IRS, apply "n
sstg =. 2 30 0 eflinAR y
if. 63 (+./@:~:) x do. sstg =. sstg , '"' , ": (_1 x: <. (%63&~:) x) end.
sstg
}}

NB. x is 2x2 positions of <>; y is ashape;w shape  result is frame message
efcarets_j_ =: {{
bshape =. (<"1 +:x) ;@:((;:'<>')"_`[`]})&.> (a: 0 _1} (<' ') (,@,. , [) (":&.>))&.> y
' : <frames> do not conform in shapes ' , (0 {:: bshape) , ' and ' , (1 {:: bshape)  
}}

NB. y is a;w;selfar;ivr;ovr
NB. Create msg if frames of a & w do not agree
efckagree_j_ =: {{
'a w selfar ivr ovr' =. y
self =. selfar 5!:0  NB. self as an entity
awr =. a ,&(#@$) w  NB. awr is ranks of noun args
emsg=.''  NB. init no return
ir =. ivr <. or =. (ovr<0)} (awr<.ovr) ,: (0 >. awr+ovr)  NB. or= outer cell ranks, limited to arg rank and adjusted for negative rank; ir is rank at which verb is applied
if. or -: awr do. or =. ir end.  NB. If outer frame is empty, switch to inner frame so we just give a shapes message without <>
if. -. -:/ (<./ awr-or) {.&> a ;&$ w do.  NB. frames must share a common prefix
  NB. error in the outermost frame.  Don't highlight it, just say the shapes disagree
NB. obsolete   bktpos =. 0 ,. awr-or
  emsg =. ' : shapes ' , (":$a) , ' and ' , (":$w) , ' do not conform'
elseif. -. -:/ (<./ or-ir) {.&> (-or) {.&.> a ;&$ w do.  NB. inner frames too, after discarding outer frame
  NB. error in the inside frame.  highlight it.
  bktpos =. (awr-or) ,. awr-ir  NB. positions of <> chars for a/w, showing the position to put <> before.  There can be no agreement error if a frame is empty, so only one char per position is possible
  emsg =. bktpos efcarets a ;&$ w
end.
NB. If we found an error, prepend the failing primitive
if. #emsg do.
  if. #selfmsg =. ovr eflinearself selfar do. emsg =. emsg ,~ selfmsg ,~ ', in ' end.
end.
emsg
}}

efarisnoun_j_ =: (((<,'0')) = {.)@>

NB. y is jerr;ranks;fill (i.0 0 if no fill);AR of failing self;a[;w][;m]
NB. if self is a verb, a/w/m are nouns; otherwise a/w are ARs
eformat_j_ =: {{
'e ovr selfar a' =. 4{.y

self =. selfar 5!:0  NB. self as an entity
psself =. 4!:0 <'self'  NB. part of speech of self
if. ism=.ovr-:'' do. ovr=.63 63 [ y =. }:y [ ind=._1{::y end.  NB. orv is the special value '' for m}.  Take the m arg in that case
if. dyad =. 4<#y do. w =. 4{::y end.
NB. now a and possibly w are args 5&6.  If self is a verb these will be the arg value(s) and dyad will be the valence
NB. if the verb is m}, there will be an m argument
NB. if self is not a verb, a and w are ARs and dyad indicates a conjunction
emsg =. ''  NB. init no result
NB. Start parsing self
if. 2 = 3!:0 > selfar do. prim =. selfar [ args=.0$0  NB. primitive or single name: self-defining
else. prim =. {. > selfar [ args =. (0;1) {:: selfar  NB. entity, args if any
end.

NB. Go through a tree to find the message code to use
select. psself
case. 3 do.
  NB. verb. treat monad and dyad separately
  if. dyad do.
    NB. Dyads
    ivr =. }. self b. 0  NB. dyad ranks of the verb
    select. prim
    case. ;:'=<<.<:>>.>:++.+:**.*:-%%:^^.~:|!o.&."b.' do.  NB. atomic dyads and u"v
      NB. Primitive atomic verb.  Check for agreement
      if. e=9 do. if. #emsg=.efckagree a;w;selfar;ivr;ovr do. emsg return. end. end.
    case. ;: '@&' do.  NB. conjunctions with inherited rank
      if. (e=9) *. -. +./ efarisnoun args do. if. #emsg=.efckagree a;w;selfar;ivr;ovr do. emsg return. end. end.  NB. check only if inherited rank
    case. ;:'I.' do.
      if. (e=9) do. emsg =. ((,.~ -&ivr) a ,&(#@$) w) efcarets a ;&$ w  return. end.
    end.
  else.
    NB. Monads
  end.
end.
''
}}


1: 0 : 0
self=.+
eformat_j_ 9;63 63;(5!:1<'self');a;<w [ a =. i. 2 3 4 [ w =. i. 3 3 4
eformat_j_ 9;1 63;(5!:1<'self');a;<w [ a =. i. 2 3 4 [ w =. i. 3 3 4
eformat_j_ 9;1 2;(5!:1<'self');a;<w [ a =. i. 2 3 4 [ w =. i. 3 3 4
eformat_j_ 9;1 2;(5!:1<'self');a;<w [ a =. i. 1 2 [ w =. i. 1 2 4   NB. no error
eformat_j_ 9;1 1;(5!:1<'self');a;<w [ a =. i. 1 2 [ w =. i. 1 2 4
eflinearself_j_ 5!:1<'self'
self=.+&5
eflinearself_j_ 5!:1<'self'
self=.+&(i. 2 2)
eflinearself_j_ 5!:1<'self'
self=.+&1x
eflinearself_j_ 5!:1<'self'
self=.(+/ # *)
eflinearself_j_ 5!:1<'self'
self=.tolower
eflinearself_j_ 5!:1<'self'
self=.(tolower@tolower tolower tolower@tolower)
eflinearself_j_ 5!:1<'self'
)



