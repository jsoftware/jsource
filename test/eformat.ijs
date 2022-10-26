0 : 0
To add formatting for a primitive:
1. Add a EFORMAT macro at the top of the primitive
2. Change ASSERT macros to ASSERTE as desired
3. For each ASSERTE, add primitive-specific code to eformat_j_ below
)

NB. y is AR of self
NB. x is the IRS n from "n, if any (63 63 if no IRS)
NB. Result is string form; if too long, truncate and append ...
eflinearself_j_ =: {{
'' linearself y
:
NB. create self as an entity; if entity has IRS, apply "n
self =. y 5!:0  NB. self as an entity
psself =. 4!:0 <'self'  NB. part of speech of self
if. (3=psself) *. 63 (+./@:~:) x do. self =. self f."(_1 x: <. (%63&~:) x) end.
lin =. 5!:5 <'self'
if. 17 < #lin do. lin =. '...' ,~ 17 {. lin end.
lin
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

NB. y is jerr;ranks;AR of failing self;AR of a;<AR of w
NB. If an arg is omitted its AR is empty
eformat_j_ =: {{
'e r selfar aar war' =. y
'ivr ovr' =. _2 |.\ r  NB. verb-ranks in case IRS is in use. ir=.[lr,] rr is the (positive) rank of self; or=.[lcr,] rcr is jt->ranks passed into self, i. e. from "n
self =. selfar 5!:0  NB. self as an entity
psself =. 4!:0 <'self'  NB. part of speech of self
if. #aar do. a =. aar 5!:0 end.  NB. a as an entity, if it exists
if. #war do. w =. war 5!:0 end.  NB. w as an entity, if it exists
emsg =. ''  NB. init no result
NB. Start parsing self
if. 2 = 3!:0 > selfar do. prim =. selfar [ args=.0$0  NB. primitive or single name: self-defining
else. prim =. {. > selfar [ args =. (0;1) {:: selfar  NB. entity, args if any
end.

NB. Go through a tree to find the message code to use
select. psself
case. 3 do.
  NB. verb. treats monad and dyad separately
  if. 0=#aar do.
    NB. Monads
  else.
    NB. Dyads
    select. prim
    case. ;:'=<<.<:>>.>:++.+:**.*:-%%:^^.~:|!o.&."b.' do.  NB. atomic dyads and u"v
      NB. Primitive atomic verb.  Check for agreement
      if. e=9 do. if. #emsg=.efckagree a;w;selfar;ivr;ovr do. emsg return. end. end.
    case. ;: '@&' do.  NB. conjunctions with inherited rank
      if. (e=9) *. -. +./ efarisnoun args do. if. #emsg=.efckagree a;w;selfar;ivr;ovr do. emsg return. end. end.  NB. check only if inherited rank
    case. ;:'I.' do.
      if. (e=9) do. emsg =. ((,.~ -&ivr) a ,&(#@$) w) efcarets a ;&$ w  return. end.
    end.
    
  end.
end.
''
}}


0 : 0
self=.+
eformat_j_ 9;0 0 63 63;(5!:1<'self');(5!:1<'a');<(5!:1<'w') [ a =. i. 2 3 4 [ w =. i. 3 3 4
eformat_j_ 9;0 0 1 63;(5!:1<'self');(5!:1<'a');<(5!:1<'w') [ a =. i. 2 3 4 [ w =. i. 3 3 4
eformat_j_ 9;0 0 1 2;(5!:1<'self');(5!:1<'a');<(5!:1<'w') [ a =. i. 2 3 4 [ w =. i. 3 3 4
)



