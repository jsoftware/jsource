1: 0 : 0
Rules for eformat_j_:

1. Do not assign y or any part of it to a global name.  The components of y are special headers
and their values are not protected by JE's usecount mechanism.  When eformat returns the values may
become invalid.

To guarantee a fresh copy (for debugging only) do something like
savy__ =: 3!:1 (3!:2) y

2. Remember that the noun arguments a, w, and ind may be very large.  (self may be large too but that
is less likely and we can truncate it if we need to)  Do not do anything that will require making a copy
of a/w/ind or creating a result of the same size.

These are NOT OK:
allint =. *./ (-: <.) , a  NB. <. a makes a copy of a
~. ind   NB. ~. makes a hashtable of size +:#ind
ind i. obinds   NB. (x i. y) makes a hashtable if y is a list
val =. ,a  NB. , result is virtual, but assigning it to a name realizes it

These are OK:
val =. a
val =. a ; w
(i. >./) , inds  NB. , is virtual and (i. >./) is backed by special code
10 {. a   NB. virtual
ind i. >./ obinds   NB. When x is a list and y is a scalar, no hashtable is created
)

(bx) =: >: i. #bx =. <@(,&'_j_');._2 (0 : 0)   NB. define error names
EVATTN
EVBREAK
EVDOMAIN
EVILNAME
EVILNUM
EVINDEX
EVFACE
EVINPRUPT
EVLENGTH
EVLIMIT
EVNONCE
EVASSERT
EVOPENQ
EVRANK
EVEXIT
EVSPELL
EVSTACK
EVSTOP
EVSYNTAX
EVSYSTEM
EVVALUE
EVWSFULL
EVCTRL
EVFACCESS
EVFNAME
EVFNUM
EVTIME
EVSECURE
EVSPARSE
EVLOCALE
EVRO
EVALLOC
EVNAN
EVNONNOUN
EVTHROW
EVFOLDLIMIT
EVVALENCE
EVINHOMO
EVINDEXDUP
EVEMPTYT
EVEMPTYDD
EVMISSINGGMP
EVSIDAMAGE
EVDEADLOCK
EVASSEMBLY
EVOFLO
EVTYPECHG
EVNORESULT
)

NB. x and y are strings to be joined.  We insert a space if not doing so would change the words
efjoinstgs_j_ =: ([,' ',]) [^:(-.@-:&(;: ::(a:"_))) ,
NB.x is (1 if all of main name always needed),(max # characters allowed),(par),(use gerund if possible); y is AR
NB. par is:0=no parens needed; 1=parens needed for train but not AC exec; parens needed for train and AC
NB. result is string to display, or ... if string too long
eflinAR_j_ =: {{
NB. parse the AR, recursively
if. (2{.x) -: 0 0 do. '...' return. end.  NB. If no room for formatting, stop looking
'frc max par ger' =. x
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
    if. 8192 < 7!:5<'aro1' do. '...' return. end.  NB. or anything big
    lin =. 5!:5<'aro1'  NB. value is small, take its linrep
    if. ger do. if. ';:'-:2{.lin do. lin =. (,'`'&,)&:>/ aro1 end. end.  NB. If we expect gerunds, convert word-list to gerund form
    if. max >: #lin do. (')' ,~ '('&,)^:((1<#;:lin)*.par~:0) lin return. end.  NB. return value if short enough; if multiword, parenthesize
    (_3 }. lin) , '...' return.
  case. ,&.>'234' do.  NB. hook/fork/train
    NB. these cases are not so important because they don't give verb-execution errors
    stgs=.0$a:  NB. list of strings
    for_i. aro1 do.
      stg =. 0.,~ ((1 1 p. i_index~:0) ,~ 0., 0. >. max%(#aro1)-i_index) eflinAR i  NB. collect strings for each AR; paren trains except the right
      max =. max - #stg [ stgs =. stgs , <stg  NB. don't allow total size to be exceeded
    end.
    NB. We have strings for each component.  If nothing has a display, return '...' unless this is top-level
    if. (frc=0) *. *./ stgs = <'...' do. '...' return. end.
    (')' ,~ '('&,)^:(par~:0) ; efjoinstgs&.>/ stgs return. 
  case. do.  NB. default: executed A/C
    ac =. ({.aro) 5!:0  NB. the actual ac
    posac =. 4!:0 <'ac'   NB. part of speech being executed here
    stgs =. <stg=. (0 >. (<:frc) , max , 1 0) eflinAR {. aro   NB. get (stg) for AC
    stgs =. stgs ,~ <stg =. (0 >. 0 , (({.aro) e. ;:'^:}@.') ,~ (posac{0 1 1 0) ,~  max =. max -#stg) eflinAR {. aro1
    if. 1 < #aro1 do. stgs =. stgs , <stg =. (0 >. 0 , 0 ,~ (posac{0 1 2 1) ,~  max =. max -#stg) eflinAR {: aro1 end.
    if. (frc=0) *. *./ stgs = <'...' do. '...' return. end.
    NB. string not too long; leave spaces where needed
    (')' ,~ '('&,)^:(par=2) ; efjoinstgs&.>/ stgs return. 
  end.
end.
}}


NB. y is AR of self
NB. x is the IRS n from "n, if any (63 63 if no IRS))
NB. Result is string form, limited to 30 characters
eflinearself_j_ =: {{
'' eflinearself y
:
NB. create self as an entity; if entity has IRS, apply "n
sstg =. 2 30 0 0 eflinAR y
if. 63 (+./@:~:) x do. sstg =. sstg , '"' , ": (_1 x: <. (%63&~:) x) end.
sstg
}}

NB. y is list of boxes.  Result is string with the words separated and commaed
efandlist_j_ =: {{
if. 2 < #y do. y =. (,&','&.>@}: , {:) y end.
if. 1 < #y do. y =. (}: , (<'and') , {:) y end.
;:^:_1 y
}}

NB. x is 2x2 positions of <> (x< x> ,: y< y>) ; y is ashape;wshape  result is frame message
efcarets_j_ =: {{
bshape =. (<"1 +:x) ;@:((;:'<>')"_`[`]})&.> (a: 0 _1} (<' ') (,@,. , [) (":&.>))&.> y
(0 {:: bshape) , ' and ' , (1 {:: bshape)  
}}

NB. y is a;w;ivr;ovr
NB. Create msg if frames of a & w do not agree
efckagree_j_ =: {{
'a w ivr ovr' =. y
awr =. a ,&(#@$) w  NB. awr is ranks of noun args
emsg=.''  NB. init no return
ir =. ivr <. or =. (ovr<0)} (awr<.ovr) ,: (0 >. awr+ovr)  NB. or= outer cell ranks, limited to arg rank and adjusted for negative rank; ir is rank at which verb is applied
if. or -: awr do. or =. ir end.  NB. If outer frame is empty, switch to inner frame so we just give a shapes message without <>
if. -. -:/ (<./ awr-or) {.&> a ;&$ w do.  NB. frames must share a common prefix
  NB. error in the outermost frame.  Don't highlight it, just say the shapes disagree
NB. obsolete   bktpos =. 0 ,. awr-or
  emsg =. 'shapes ' , (":$a) , ' and ' , (":$w) , ' do not conform'
elseif. -. -:/ (<./ or-ir) {.&> (-or) {.&.> a ;&$ w do.  NB. inner frames too, after discarding outer frame
  NB. error in the inside frame.  highlight it.
  bktpos =. (awr-or) ,. awr-ir  NB. positions of <> chars for a/w, showing the position to put <> before.  There can be no agreement error if a frame is empty, so only one char per position is possible
  emsg =. '<frames> do not conform in shapes ' , bktpos efcarets a ;&$ w
end.
NB. If we found an error, prepend the failing primitive
emsg
}}

NB. y is a list of 3!:0 results (0 for empty); result is list of the types represented, including a: to mean 'empty'
efhomo_j_ =: {{
types =. ~. 0 10 14 16 17 I. 0 1 4 8 16 64 128 1024 4096 8192 16384 2 131072 262144 2048 32 32768 65536 i. ,y
types { a:,;:'numeric character boxed symbol'
}}

efarisnoun_j_ =: (((<,'0')) = {.)@>  NB. predicate.  y is an AR
efarnounvalue_j_ =: 1&{::@>   NB. is is the AR of a noun.  result is the value

NB. x and y are shapes of atoms; return shape of cells of >x,&<y
shapeunion_j_ =: {{ x >.&((x >.&# y)&{.) y }}

NB. x is an array of selectors into an array of shape y
NB. result is either the shape the result would have, or rc;(y-axis#,is-excl,offi);<pathlist to offender
NB. where rc of 1 2 3 means the same as 9!:23
NB. _1 means an index referred to a non-existent axis (eg (<1 2 3){i.5 5)
NB. _2 means a singly-boxed index list had rank >1 (eg (<i.2 2){i.5 5)
NB. _3 means too much boxing
NB. _4 means too many exclusion lists
NB. (in these cases, axis# and offi should be ignored)
NB. pathlist is a;...;d, where the offender can be found as a{:: ... d{::x (TODO is there a nicer way to represent this?), except that ''{::<a is <a, not a
efindexaudit_j_ =: {{
 if. 0 (>:L.) x do. NB.simple array of integers
  rc=. (,x) 9!:23] 0;(-{.!.1 y),<:{.!.1 y NB. !.1 ensures correct behaviour for scalar
  if. 0-:0{::rc do. NB.no error
   ($x),}.y return.
  elseif. 1=0{::rc do. (0{rc),(<0 0 0),<,<0$0 return.
  else. (0{rc),(<0 0,(,x){~1{::rc),<,<,($x)#:1{::rc return. end. end.
 NB.(x;...){y.  Same result spec as efindexaudit
 auditindex =. {{
  NB.((<x);...){y.  y is dimension of the given axis.  Result spec is shape or rc,&<path
  auditselector =. {{
   if. 0 (>:L.) x do. (0&{ , }.&.>@(1&{) , 2&{)^:(0&(<L.)) x efindexaudit y return. end. NB.punt to ordinary audit for simple list of integers, and then strip out y-axis on error
   rs=. 0$0                               NB.result shape
   if. 0 ~: #$x do. _4;0 0;'' return. end.NB.too many exclusion lists
   x=. ,0{::,x
   if. 0 (<L.) x do. _3;1 0;($x)#:i return. end. NB.too much boxing!
   if. 0 (<L.) t=. x efindexaudit y do. (0&{ , (1 , {:)&.>@(1&{) , 2&{) t return. end. NB.oob excluded index
   y - #@~. x + y * x<0                    NB.overall length.  Need to regularise to correctly compute #excluded hyperrows(?), which needs linear space.  But { would have needed at least that much space anyway, and more besides, so it seems 
  }}
  if. 0 (>:L.) x do.                        NB.array of integers, each selecting 1 cell
   if. (2-m)<#@$x  do. _2;0 0 0;'' return. end.NB.rank too high (rank 2 allowed only for 1st box
   if. ({:$x) > #y do. _1;(0 0,{:$x);'' return. end.NB.index list too long
   'f cs'=. ({:$x)({.;}.)y                   NB.frame; cell shapesmoutput'`'
   offi=. /:~ ~. (x ;@:(<@I.@:>:"1) f) , (x ;@:(<@I.@:<"1) -f) NB.list of offending indices
   if. ''-:offi do.                       NB.everything in range
    (}:$x) , ({:$x) }. y return.
   else. 3;(({.offi) , 0 , ({.offi){x);({.offi) return. end. end.
  NB.boxed x; consider each element in turn
  if. 1<#@$x  do. _2;0 0 0;'' return. end.NB.rank too high
  if. x >&# y do. _1;(0 0,#x);'' return. end.NB.index list too long
  'f cs'=. (#x)({.;}.)y                   NB.frame; cell shape
  rs=. 0$0                                NB.result shape
  for_i. i.#x do.
   if. 0 (>:L.) t=. (i{::x) auditselector i{f do.
    rs=. rs , t
   else. (0{t) , (i&,&.>1{t) , (($x)#:i)&;&.> 2{t return. end. end.
  rs , cs
 }}
 rs=. 0$0                                 NB.result shape
 for_i. i.*/$x do.                        NB.boxed x; consider each index in turn
  if. 0 (>:L.) t=. (i{::,x) ((*i_index) auditindex) y do. NB.if no error, then pad out shape appropriately
   rs=. rs shapeunion t                   NB. and update result shape
  else.
   (2{.t) , (($x)#:i)&;&.>2{t return. end. end.NB.otherwise, add top-level path and return error desc
 ($x),rs
}}

NB. y is a result from efindexaudit; x is $y; m is (printed string for x;printed string for y)
effrommsg_j_ =: {{
 'xstg ystg'=. m
 'rc aeo path'=. y
 'axis excl off'=. aeo
 axismsg=. ''
 axislen=. {.!.1 x
 if. 1<#x do. axismsg=. 'axis ',' of ',~":axis
              axislen=. axis{x end.
 NB. generate a message like 'atom at position xx in exclusion list at position xx in ...
 NB. match successive elements of path with successive elements of parts, and remove the elements referring to empty paths
 parts=. ((1=#path) {:: ('index list';'index'));(excl {:: 'selector';'exclusion list');'atom'
 pathmsg=. path (] , (1<#x) # ' at position ' ,^:(*@#@]) ":@[)&.> (parts {.~ #path)
 pathmsg=. pathmsg #~ (0 1 0{.~#path) +. (0~:#)&>path NB.always print 'selector'/'exclusion list'; others are optional
 if. #pathmsg do. pathmsg=. >([ , ' in ' , ])&.>/ |.pathmsg else. pathmsg=. xstg end.
 emsg=. ''
 select. rc
 case. _4 do. emsg=. pathmsg , ' contains too many exclusion lists (must have rank 0)'
 case. _3 do. emsg=. pathmsg , ' is overly boxed'
 case. _2 do. emsg=. pathmsg , ' must have rank 1'
 case. _1 do. emsg=. pathmsg , ' is overlong; has length ' , (":off) , ' but rank of ' , ystg , ' is only ' , ":#x
 case.  1 do. emsg=. pathmsg , ' is not a number'
 case.  2 do. emsg=. pathmsg , ' is ' , (":off) , '; not an integer'
 case.  3 do. emsg=. pathmsg , ' is ' , (":off) , '; too long for ' , axismsg , 'y, whose length is only ' , ":axislen
 end.
 emsg
}}

NB. y is result from 9!:23, x is index arg to that 9!:23
NB. Result is message about the index if any, suitable by itself or if prefixed by ' [xy] has' 
efindexmsg_j_ =: {{
'rc il' =. y  NB. return code and index list of error
emsg =. ''
select. rc
case. 1 do. emsg =. 'nonnumeric type (' , (0{::efhomo 3!:0 x) , ')'
case. 2 do. emsg =. 'nonintegral value (' ,(": (<il) { x) , ')' ,  (' at position ' , ":)`(''"_)@.(1=*/@$x) il
case. 3 do. emsg =. 'invalid value (' ,(": (<il) { x) , ')' ,  (' at position ' , ":)`(''"_)@.(1=*/@$x) il
end.
emsg
}}

efdispnsp_j_ =: {{ (": , ' ' , (;:x) {::~ 1&=) y }}  NB. x is (plural;singular) y is count result is 'y word'

NB. [x and] y are args to numeric verb.  Result is string to display if type is nonnumeric
efcknumericargs_j_ =: {{
;:^:_1 ('numeric';a:) -.~ efhomo (*@(#@,) * 3!:0) y
:
xtype=. 'x is '&,&.> ('numeric';a:) -.~ efhomo (*@(#@,) * 3!:0) x [ ytype=. 'y is '&,&.> ('numeric';a:) -.~ efhomo (*@(#@,) * 3!:0) y
;:^:_1 xtype ([ , ((<'and') #~ *&#) , ]) ytype
}}

NB. y is polynomial in coeff, roots, or exponent form.  Result is msg, or '' if no error found
efauditpoly_j_ =: {{
if. 32 ~: 3!:0 y do.  NB. not boxed form - converting to roots
  if. e=EVDOMAIN do.
    if. #emsg=. efcknumericargs y  do. emsg return. end.
    if. </ |. 0 = y do. 'nonzero constant polynomial cannot be converted to roots' return. end.
  end.
else.  NB. boxed form
  if. (1<#y) +. (1>:#@$>{.y) do.  NB. coeff;roots
    if. e=EVDOMAIN do. if. (0<#@$y) *. 2~:#y do. 'boxed polynomial must be coeff;roots or <roots' end. end.
    if. #@$y do. 'coeff roots' =. y else. roots =. >{.ay[ coeffs =. 1 end.
    if. e=EVRANK do. if. #@$coeff do. 'coeff must be an atom' end. end.
    if. e=EVRANK do. if. 1<#@$roots do. 'roots must be have rank < 2' end. end.
    if. e=EVDOMAIN do. if. #emsg=. coeff efcknumericargs roots  do. 'coeff and roots must be numeric' return. end. end.  NB. complex case not decoded
  else.  NB. exponent form - a single box containing a two-row table
    exps =. >{.y  NB. the table
    if. e=EVRANK do. if. 2~:#@$exps do. 'exponent form must be a boxed table' end. end.
    if. e=EVLENGTH do. if. 2~:{:$exps do. 'exponent form must have 2 columns' end. end.
    if. e=EVDOMAIN do. if. #emsg=. efcknumericargs exps  do. 'exponent table must be numeric' return. end. end.  NB. complex case not decoded
    exps =. {:"1 exps  NB. extract the exponents
    if. e=EVDOMAIN do. if. #emsg=. 'exponent table has '&,^:(*@#) efindexmsg a 9!:23 (0;0) do. emsg return. end. end.  NB. nonnegative integral exponents
  end.
end.
''
}}
NB. y is a misspelt string
check_spelling_j_ =: {{
if. -. (*./@e.&(128{.a.)) y do. NB. any nonascii characters?
 i=. y I.@:-.@e. 128{.a.
 i=. ({.~ >:@(}. (=i.0:) >:@}:)) i NB. grab a consecutive leading sequence of illegal characters; usually, this will be utf8-encoded source, and the user will have a better idea of what we're talking about if we show them the character(s) from their source
 s=. 's' {.~ 1~:#i
 NB.can't pass through nonascii characters here; todo figure that out
 'invalid non-ascii character' , s , ('' [ ': ' , (quote i{y)) , ' (codepoint' , s , ' ' , (":3 u:i{y) , ') at offset ' , ":0{i return.
end.
NB. check for incorrectly inflected words
NB. I hope I've not missed any...
inflected=. ;:'=.=:<.<:>.>:+.+:*.*:-.-:%.%:^.^:$.$:~.~:|.|: . : :. :: ,.,:;.;:#.#:!.!:/./:\.\:[.[:].]:{.{:{::}.}:".":`:@.@:&.&.:&:?.a.a:A.b.C.e.E.f.H.i.i:I.j.L.L:M.o.p.p..p:q:r.s:S:t.T.u:u.v.x:F.F:F..F.:F:.F::assert.do.end.break.continue.else.elseif.for.if.return.select.case.fcase.throw.try.catch.catchd.catcht.while.whilst._9:_8:_7:_6:_5:_4:_3:_2:_1:_0:0:1:2:3:4:5:6:7:8:9:_:__:NB.'
initial_letters=. 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz'
digits=. '0123456789'
digitsm=. digits , '_'
continuation_letters=. initial_letters , digitsm
yw=. (#~ -.@e.&inflected) (#~ e.&'.:'@{:@>);:y NB.grab inflected words not in 'inflected'
for_wb. yw do.
 w=. >wb
 NB.need to check for x_: for_x. goto_x. label_x. NB.anything and numbers (eg 5.)
 if. 'NB.' -: 3{.w do. continue.
 elseif. '_:' -: _2{.w do. NB.self-effacing reference?
  if. (initial_letters e.~ {.w) *. continuation_letters *./@e.~ _2}.w do. continue. end.
 elseif. '.' -: {:w do.
  if. (digitsm e.~ {.w) *. digits *./@e.~ }.}:w do. continue. end. NB.number
  if. ('for_' -: 4{.w) +. ('goto_' -: 5{.w) +. ('label_' -: 6{.w) do. NB.control
   ww=. w }.~ >:w i.'_'
   if. (initial_letters e.~ {.ww) *. continuation_letters *./@e.~ }:ww do. continue. end. end. NB.name ok
 end.
 i=. w {.@I.@E. y
 NB.todo provide suggestions (cf '"elsif." is invalid; did you mean "elseif."?') based on hamming/levenshtein/whatever distance
 'invalid inflected word ' , (quote w) , ' at offset ' , ":i return.
end.
''
}}

NB. y is jerr;curname;jt->ranks;AR of failing self;a[;w][;m]
NB. if self is a verb, a/w/m are nouns; otherwise a/w are ARs
NB. result is string to replace the original emsg, or empty string to make no change
eformat_j_ =: {{
NB. extract internal state quickly, before anything disturbs it
fill =. 9!:22''  NB. This also clears jt->fill
'e curn ovr selfar a' =. 5{.y
'perr e' =. 0 256 #: e  NB. top bits are paren info
if. e=EVASSEMBLY do. y =. }:y [ ind=._1{::y   NB. assembly errors have a dope vector in y
elseif. ism=.ovr-:'' do. ovr=.63 63 [ y =. }:y [ ind=._1{::y  NB. ovr is the special value '' for m}.  Take the m arg in that case
end.
if. dyad =. 5<#y do. w =. 5{::y end.
NB. now a and possibly w are args 4&5.  If self is a verb these will be the arg value(s) and dyad will be the valence
NB. if the verb is m}, there will be an m argument
NB. if self is not a verb, a and w are ARs and dyad indicates a conjunction
NB. Start parsing self
if. selfar -: <,' ' do.
  NB. The special AR of SP is used to indicate errors found in preparsing.  a is the word list and w is the index of the failing word
  NB. We don't do any analysis yet, but we assign a special part of speech (5) to self
  psself =. 5 [ prim =. ''   NB. No primitive or sentence was executed
else.
  self =. selfar 5!:0  NB. self as an entity
  psself =. 4!:0 <'self'  NB. part of speech of self
  while. do.
    if. 2 = 3!:0 > selfar do. prim =. selfar [ args=.0$0 break.  NB. primitive or single name: self-defining
    else.
      prim =. {. > selfar [ args =. (0;1) {:: selfar  NB. entity, args if any
      if. prim ~: ;:'!.' do. break. end.
      NB. self is u!.n - discard the !.n
      selfar =. {. args
    end.
  end.
end.
NB. See if the executing value is m : .  If so, don't show the lines of the name
isexplicit =. 0 if.  prim -: {. ;:':' do. if. efarisnoun {.args do. isexplicit =. 1 end. end.

NB. for self-explanatory errors, keep the terse message.  Also for value error, which for argnames is detected without a self
if. e e. EVVALUE , EVILNAME , EVOPENQ , EVWSFULL do. '' return. end.

NB. Create the header lines: terse-error [in name] [executing fragment], and a version without the fragment
hdr1 =. ((<:e) {:: 9!:8'') , (' in '&,^:(*@#) curn) , LF
emsg =. ''  NB. init no formatted result

NB. Handle post-execution errors, which are not detected until after the sentence's final result has been found,
NB. so that by the time eformat is called the selfar has moved on from the error
select. e
case. EVNONNOUN do. emsg =. 'the sentence did not produce a noun result, usually because of a misspelled word or missing argument'
case. EVASSERT do. emsg =. 'the result was expected to be all 1'
end.
if. #emsg do. hdr1 , emsg return. end.  NB. pee

if. selfar -: {. ;:':' do. hdr =. (}:hdr1) , ', defining explicit entity' , LF
elseif. (e=EVSYNTAX) *. isexplicit do. hdr =. hdr1  NB. syntax error is not associated with any execution, unless in (". y) 
elseif. psself<5 do. hdr =. (}:hdr1) , ((((e=EVASSEMBLY){::', executing ';', assembling results for '),' ',~(0 2#.psself,dyad){::2 2 2 1 1#;:'noun adv conj monad dyad')&,^:(*@#) ovr eflinearself selfar) , LF  NB. finish header lines
else.  NB. error not associated with any execution.  a distinguishes types
  if. 32 = 3!:0 a do. hdr =. (}:hdr1) , ', before sentence execution' , LF  NB. error in enqueue
  elseif. 0 = #a do.  NB. error during parse
    if. e=EVSYNTAX do. hdr =. (}:hdr1) , ', sentence did not execute to a single result' , LF  NB. stack not empty at end
    else. hdr =. (}:hdr1) , ', error evaluating name' , LF
    end.
  else. hdr =. (}:hdr1) , ', unexecutable fragment (' , (;:^:_1 a{;:'noun adv conj verb') , ')' , LF
    if. *./ a=0 do. hdr =. hdr , 'to concatenate nouns use a verb such as ,' , LF end.
  end.
end.
NB. If there were unbalanced parentheses, add a second line describing them
if. perr do.
  'ppos pt' =. 0 2 #: perr   NB. paren pos left-or-equal error pos, parenpos right-or-equal error pos, paren type
  hdr =. hdr , 'unmatched ' , (pt{'()') , (ppos{::'';' to the left of';' to the right of';' at') , ' the word in error' , LF
end.

NB. Handle environment-dependent and non-execution errors
select. e
case. EVATTN do. emsg =. 'the executing entity was interrupted by user action'
case. EVBREAK do. emsg =. 'the executing sentence was aborted by user action'
case. EVFACE do. emsg =. ('11!:' -: 4 {. eflinearself selfar) {:: 'file system error';'host does not support wd'
case. EVLIMIT do. emsg =. 'a system limit was exceeded'
case. EVSTACK do.  emsg =. 'infinite recursion'
case. EVFACCESS do. emsg =. 'nonexistent file or missing permissions'
case. EVFNAME do. emsg =. 'nonexistent file or invalid filename '
case. EVFNUM do. emsg =. 'the specified file number is not open'
case. EVTIME do. emsg =. 'the execution time limit was exceeded'
case. EVRO do. emsg =. 'attempt to modify a read-only mapped file, or the xyz_index created by for_xyz. (perhaps with nested loops having the same index variable)'
case. EVCTRL do. emsg =. 'the line, with its number in its definition shown in brackets, has a mismatched control structure'
case. EVEMPTYT do. emsg =. 'no sentences following for. or select.'
case. EVEMPTYDD do. emsg =. 'unmatched {{ or }}'
case. EVILNUM do. emsg =. 'any word beginning with a digit or _ must be a valid number'
NB.todo also perform spellchecking for top-level forms in loaded files
case. EVSPELL do. if. selfar -: <'".' do. emsg =. check_spelling_j_ a
                  elseif. selfar -: <,':' do. emsg=. check_spelling_j_ w 5!:0 end.
                  if. 0-:#emsg do. emsg =. 'words with . or : inflections must be J primitive words' end.
case. EVMISSINGGMP do. emsg =. 'extended-precision library not found.  Run  install ''gmp''  or refer to your installation instructions'
case. EVDEADLOCK do. emsg =. 'this action would deadlock the system'
end.
if. #emsg do. hdr , emsg return. end.  NB. pee

NB. Take valence error without further ado
if. (e=EVVALENCE) do.
  if. selfar -: {. ;:'[:' do. hdr1 , '[: must be part of a capped fork' return.
  else.
    if.  isexplicit do. hdr1 , 'explicit definition has no ',(dyad{::'monad';'dyad'),'ic valence' return. end.  NB. could be {{ or m : and we can't distinguish
    hdr ,  ('verb has no ',(dyad{::'monad';'dyad'),'ic valence') return.
  end.
end.

NB. Assembly errors are not associated with any primitive, but to some combining operation.  Handle them
if. e=EVASSEMBLY do.
  'flen wreck oldt newt' =. 4 {. ind [ frame =. 4 }. ind
  if. flen do. hdr , 'First results were ' , (;efhomo oldt) , ', but result for cell ' , (('(' , ,&')')^:(flen>1) (": frame #: wreck)) , ' was ' , (;efhomo newt)
  else. hdr , 'results from the cells have incompatible types'  NB. 0-len frame means we couldn't localize
  end. return.
end.

NB. Further errors are related to details of primitive execution.

NB. in case of length error, check rank
if. dyad *. e=EVLENGTH do.
 if. #emsg=.efckagree a;w;(}. self b.0);ovr do. hdr,emsg return. end. end.

NB. Go through a tree to find the message code to use
select. psself
case. 3 do.
  NB. verb. treat monad and dyad separately
  if. dyad do.
    NB. Dyads
    select. prim
    fcase. ;:'o.' do.
      if. e=EVDOMAIN do. if. #emsg=. 'x has '&,^:(*@#) a efindexmsg a 9!:23 (0;_12 12) do. hdr,emsg return. end. end.
    case. ;:'=<<.<:>>.>:++.+:**.*:-%%:^^.~:|!"j.H.??.' do.  NB. atomic dyads and u"v
      NB. Primitive atomic verb.
      if. e=EVDOMAIN do.
        if. #emsg=. a efcknumericargs w do. hdr,emsg return. end.
        if. prim e. ;:'??.' do.
          if. #emsg=. w efindexmsg w 9!:23 (0;1) do. hdr,'y must be a positive integer' return. end.
          if. #emsg=. a efindexmsg a 9!:23 (0;0,<:w) do. hdr,'x must be an integer no greater than y' return. end.
        end.
      elseif. e=EVNAN do.
        if. prim -: <,'+' do. hdr,'x and y are infinities of opposite sign' return. end.
        if. prim -: <,'-' do. hdr,'x and y are infinities of like sign' return. end.
        if. prim -: <,'%' do. hdr,'x and y are both infinite' return. end.
        if. prim -: <,'|' do. hdr,'y is infinite' return. end. NB. (and x is nonzero)
        NB. etc. ...
      end.
    case. ;:'I.' do.
      if. e=EVRANK do. hdr,'the rank of y must be at least as large as the rank of an item of x' return. end.
      if. e=EVLENGTH do. hdr , '<search cells> do not conform in shapes ' , ((-/ 0 ,~ 0 >. <:@{.) (a ,&(#@$) w)) efcarets a ,&<&$ w return. end.
      if. e=EVINHOMO  do. if. 1 < #types =. a. -.~ a efhomo@:(,&(*@(#@,) * 3!:0)) w do. hdr,'arguments are incompatible: ' , efandlist types return. end. end.
    fcase. ;:',.' do.  NB. May have agreement error.  No IRS
      if. (e=EVLENGTH) do. emsg =. 'shapes ' , (":$a) , ' and ' , (":$w) , ' have different numbers of items' end.
    case. ;:',,:' do.  NB. only error is incompatible args, but could be with fill also
      if. (e e. EVDOMAIN , EVINHOMO) do.  NB. domain /inhomo
        if. 1 < #types =. a. -.~ a efhomo@:(,&(*@(#@,) * 3!:0)) w do. emsg =. 'arguments are incompatible: ' , efandlist types
        elseif. 1=#fill do.
          if. 1 < #types =. ~. types , efhomo 3!:0 fill do. emsg =. 'arguments and fill are incompatible: ' , efandlist types end.
        end.
      end.
    case. ;:'$' do.
      if. e=EVLENGTH do. emsg=.'extending an empty array requires fill'
      elseif. e=EVDOMAIN do. emsg=. 'x has '&,^:(*@#) a efindexmsg a 9!:23 (0;0)
      elseif. e=EVINHOMO do. emsg =. 'y argument and fill are incompatible: ' , efandlist w efhomo@:(,&(*@(#@,) * 3!:0)) fill
      end.
    case. ;:'|.' do.
      if. e=EVLENGTH do. emsg=.'x has ' , ('atoms atom' efdispnsp #a) , ' but y has only ' , ('axes axis' efdispnsp #@$w)
      elseif. e=EVDOMAIN do. emsg=. 'x has '&,^:(*@#) a efindexmsg a 9!:23 (0;0$0)
      elseif. e=EVINHOMO do. emsg =. 'y argument and fill are incompatible: ' , efandlist w efhomo@:(,&(*@(#@,) * 3!:0)) fill
      end.
    case. ;:'|:' do.
      if. e=EVLENGTH do. emsg=.'x has ' , ('atoms atom' efdispnsp #a) , ' but y has only ' , ('axes axis' efdispnsp #@$w)
      elseif. e=EVDOMAIN do. emsg=. 'x has '&,^:(*@#) a efindexmsg a 9!:23 (0;0$0)
      elseif. e=EVINDEXDUP do. emsg =. 'x contains a duplicate axis number'
      end.
    case. ;:';.' do.
      select. efarnounvalue 1{args
      case. 0 do.
        if. e=EVDOMAIN do. if. #emsg=. 'x has '&,^:(*@#) a efindexmsg ('' $ a) 9!:23 (2;0$0) do. hdr,emsg return. end. end.  NB. incorrect type of x
        if. e=EVLENGTH do.
          if. 2>#$a do. a =. 0,:a end.  NB. if a has rank <2, make it a table of start/end.
          if. 2 ~: _2 { $a do. hdr , 'The 2-cells of x must have 2 rows: offsets and lengths' return. end.
          if. (#$w) < {:!.1 $a do. hdr , 'The 2-cells of x have ' , ('columns column' efdispnsp {:!.1 $a) , ' but y has only ' , ('axes axis' efdispnsp #@$w) return. end.
        elseif. e=EVINDEX do.
          start =. (maxstart =. ({:$a) $ $w) (] + (* <&0))"1 {."2 a
          if. 1 e. epos =. , start ((< 0:) +. >"1) maxstart do.
            epos =. ($start) #: epos i. 1   NB. location of bad cell
            emsg=.'starting index out of bounds (value=', (":(<epos) { {."2 a) , ', axis len=' , (":({:epos){$w) , ')' , ' in '&,^:(*@#) ((' of ' ,~ 'axis ' , ":)`(''"_)@.(1>:{:$start) {: epos) , (('cell of x with path ' , ":)`('x'"_)@.(1>:*/}:$start) }: epos)
            hdr , emsg return.
          end.
        end.
      case. 1;2;_1;_2 do.
        if. e=EVDOMAIN do.
          if. ((32=3!:0) *. 0<#@,) a do.  NB. block-matrix case, ignored for the nonce 
          elseif. #emsg=. 'x has '&,^:(*@#) a efindexmsg a 9!:23 (0;00 1) do. hdr,emsg return.  NB. incorrect type of x
          end.
        elseif. e=EVLENGTH do.
          if. a ~:&# w do. emsg =. 'x has ' , (":#a) , ' items, y has ' , (":#w) end.
        end.
      case. 3;_3 do.
        if. e=EVDOMAIN do.
          if. #emsg=. 'x has '&,^:(*@#) a efindexmsg (a) 9!:23 (0;0$0) do. hdr,emsg return. end.  NB.nonintegral value
          if. 1<#$a do. if. #emsg=. 'x has '&,^:(*@#) a efindexmsg (1 {."2 a) 9!:23 (0;0) do. hdr,emsg return. end. end.  NB. first row if any must be nonnegative
        elseif. e=EVLENGTH do.
          if. (2=#$a) *. 2~:{:$a do. hdr , 'x must have exactly 2 rows: movement vector and shape' return. end.
          if. ({:!.1 $a) > #$w do. hdr , 'x has ' , ('columns column' efdispnsp {:!.1 $a) , ' but y has only ' , ('axes axis' efdispnsp #@$w) return. end.
        end.
      end.
    case. ;:'#' do.
      if. e=EVLENGTH do.
        NB. agreement error outside the item has already been handled
        if. ({:$a) ~: (-({:ovr)<.#$w) { $w do.   NB. agreement error inside the item
          if. 1>:#$a do. xmsg =. 'x is a list of ' , ('values value' efdispnsp {:$a) else. xmsg =. 'rows of x contain ' , ('values value' efdispnsp {:$a) , ' each' end.
          if. ({:ovr)>:#$w do. ymsg =. 'y has ' , ('items item' efdispnsp {.$w) else. ymsg =. 'cells of y contain ' , ('items item' efdispnsp ({:ovr){$w) , ' each' end.
          hdr , xmsg , ' but ' , ymsg return.
        end.
      elseif. e=EVDOMAIN do. emsg=. 'x has '&,^:(*@#) a efindexmsg a 9!:23 (1;0)
      elseif. e=EVINHOMO do. emsg =. 'arguments and fill are incompatible: ' , efandlist w efhomo@:(,&(*@(#@,) * 3!:0)) fill
      end.
    case. ;:'#.' do.
      if. e=EVLENGTH do.
        if. #emsg=.efckagree a;w;0 0;1 1 do. hdr,emsg return. end.  NB. agreement error outside the item
      elseif. e=EVDOMAIN do. if. #emsg=. a efcknumericargs w  do. hdr,emsg return. end.
      end.
    fcase. ;:'A.' do.
      if. e=EVINDEX do. hdr , 'the anagram number must be less than !#y' return. end.
    case. ;:'#:' do.
      if. e=EVDOMAIN do. if. #emsg=. a efcknumericargs w  do. hdr,emsg return. end.
      end.
    case. ;:'/./..' do.
      if. e=EVLENGTH do. emsg =. 'shapes ' , (":$a) , ' and ' , (":$w) , ' have different numbers of items' end.
    case. ;:'{' do.
      if. e e. EVINDEX,EVLENGTH,EVDOMAIN do.
        wcs=. ($w) {.~ -(0{ovr) <. #$w NB. w cell shape
        if. 0 (<L.) rc=. a efindexaudit wcs do.
          rc=. rc 1}~ < (0}~ (($w) -&# wcs) + 0&{) 1{::rc NB. add frame rank to y-axis#.  No }::; annoying
          emsg=. ($w) ('x';'y') effrommsg rc end. end.
    fcase. ;:'{.{:' do.
      if. e=EVINHOMO do. hdr ,  'y argument and fill are incompatible: ' , efandlist w efhomo@:(,&(*@(#@,) * 3!:0)) fill return. end.
    case. ;:'}.}:' do.
      if. e=EVLENGTH do. emsg=.'x has ' , ('atoms atom' efdispnsp #a) , ' but y has only ' , ('axes axis' efdispnsp #@$w)
      elseif. e=EVDOMAIN do. emsg=. 'x has '&,^:(*@#) a efindexmsg a 9!:23 (2;0$0)
      end.
    case. ;:'}' do.
      if. ism do.  NB. If we didn't capture ind, we can do nothing
        if. e=EVINHOMO do. if. 1 < #types =. a. -.~ a efhomo@:(,&(*@(#@,) * 3!:0)) w do. hdr,'arguments are incompatible: ' , efandlist types return. end. end.
        NB. get the shape of the selected region (or index error)
        selshape =. ''
        if. 32 ~: 3!:0 ind do.  NB. unboxed selectors
          select. #$ind
          case. 0;1 do.
            selshape =. <ind efindexaudit $w
          case. 2 do. NB. todo: scatter modify
            if. e=EVDOMAIN do. if. #emsg=. efcknumericargs ind  do. hdr,'m is ',emsg return. end.  end.
            if. e=EVLENGTH do. if. ({:$ind) > #@$w do. hdr,'the 1-cells of m have length ' , (":{:$ind) , ', but the rank of y is only ' , ":#@$w return. end. end.
            if. e e. EVDOMAIN,EVINDEX do. if. #emsg=. 'm has '&,^:(*@#) ind efindexmsg ind 9!:23 (0;'') do. hdr,emsg return. end. end.  NB. nonintegral index
            if. e=EVINDEX do.
              erow =. 1 i.~ ind +./@((< -) +. >:)"1 $w  NB. row containing error
              if. erow < #ind do.
                ecol =. 1 i.~ (erow { ind) ((< -) +. >:) $w  NB. column containing error
                hdr,'position (' , (":erow,ecol) , ') of m has the value ' , (":(<erow,ecol) { ind) , ', but the length of axis ' , (":ecol) , ' of y is only ' , (":ecol{$w) return.
              end.
            end.
            NB. selectors must be valid here
            selshape =. <(#ind) , (#@$ind) }. $w  NB. each row of ind selects a cell of w
          case. do. if. e e. EVRANK,EVLENGTH do. hdr,'rank of selector must be < 3' return. end.
          end.
        else.  NB. boxed selectors
          selshape =. <@(efindexaudit&($w))"0 ind   NB. one result per box
        end.
        if. e e. EVINDEX,EVLENGTH,EVDOMAIN do.  NB. index-type error -  see if any index box had an error
          if. 1 (< L.) selshape do.  NB. there is an index-type error
            errbox =.  1 i.~ 0&(<L.)@> ,selshape
            hdr,((1 < #@, selshape) # 'in box ' , (":errbox) , ' of m, ') , ($w) ('m';'y') effrommsg errbox {:: ,selshape return.
          end.
        end.
        cellshapes =. ,selshape  NB. all the selections, which are all boxed shapes of selected regions
        if. -. *./ (-:"_1 _  {.) cellshapes do. if. e=EVDOMAIN do. hdr,'the boxes of m must specify regions of the same shape' return. end. end.
        cellshapes =. ;`(# , >@{.)@.(1 < */@$) cellshapes  NB. shape of the selected region
        if. -. ($a) ([ -: -@#@[ {.!._1 ]) cellshapes do. if. e e. EVRANK`EVLENGTH do. hdr,'the shape of x (' , (":$a) ,') must be a suffix of the shape of the selection (' , (":cellshapes) , ')' return. end. end.
      end.
NB. ". domain
    case. ;:'b.' do.
      if. e=EVDOMAIN do.
        dom=.(16 <: efarnounvalue 0{args) {:: 0 1;0$0  NB. if m<16, domain is boolean, else integer
        if. #emsg=. 'x has '&,^:(*@#) a efindexmsg a 9!:23 (0;dom) do. hdr,emsg return. end.  NB. incorrect type of x or nonintegral
        if. #emsg=. 'y has '&,^:(*@#) w efindexmsg w 9!:23 (0;dom) do. hdr,emsg return. end.  NB. incorrect type of y or nonintegral
      end.
    case. ;:'C.' do.
      if. e=EVINDEXDUP do. hdr , ('a permutation in ' #~ 1<*/}:$a) , 'x contains a duplicate value' return. end.
      if. e e. EVDOMAIN,EVINDEX do.
        if. 32 ~: 3!:0 a do.
          if. #emsg=. 'x has '&,^:(*@#) a efindexmsg a 9!:23 (0;(_1&- , ]) #w) do. hdr,emsg return. end.  NB. direct form
        else.
          if. 1 e. , iserr =. (0~:0&{::)@> eall =. (9!:23&(0;(_1&- , ]) #w))&.> a do.
            enx =. ($ #: i.&1@,) iserr
            if. #emsg=. 'element (' , (":enx) , '} of x has ' , ((<enx){::a) efindexmsg ((<enx){::eall) do. hdr,emsg return. end.
          end.
        end.
      end.
    case. ;:'p.' do.
      if. 32~:3!:0 a do.  NB. unboxed polynomial
        if. e=EVDOMAIN do.
          if. #emsg=. efcknumericargs a do. hdr,'x is ' , emsg return. end.  NB. must be numeric
        end.
      else.  NB. boxed: mplr/roots or multinomial
        if. (e=EVDOMAIN) *. 0=*/@$ a  do. hdr,'polynomial may not be empty' return. end.  NB. must be nonempty
        if. (e=EVLENGTH) *. (2<#a) do. hdr,'polynomial may have coeff and roots, nothing more' return. end.  NB. must be nonempty
        'coeff roots' =. _2 {. 1;a  NB. Get (possibly defaulted) coeff, and roots
        if. e=EVDOMAIN do.
           if. #emsg=. efcknumericargs coeff do. hdr,'coeff is ' , emsg return. end.
           if. #emsg=. efcknumericargs roots do. hdr,'roots is ' , emsg return. end.
        end. 
        if. e=EVRANK do.
           if. '' -.@-: $coeff do. hdr, 'coeff must be an atom'  return. end.
           if. 2 < #@$ roots do. hdr, 'roots/multinomial must have rank < 3'  return. end.
        end.
        if. 2=#@$ roots do.
          if. (e=EVLENGTH) do.
            if. (2>{:$roots) do. hdr,'multinomial must have at leat 1 variable' return. end.  NB. must be nonempty
NB. must handle error in pdt here
          end.
        end.
      end.
      if. e=EVDOMAIN do. if. #emsg=. efcknumericargs w do. hdr,'y is ' , emsg return. end. end.  NB. must be numeric
    case. ;:'p..' do.
      if. #emsg=.efauditpoly w do. hdr,emsg return. end.
      if. e=EVDOMAIN do. if. #emsg=. efcknumericargs a do. hdr,'x is ' , emsg return. end. end.  NB. must be numeric
    case. ;:'p:' do.
      if. e=EVDOMAIN do.
        if. #emsg=. a efcknumericargs w  do. hdr,emsg return. end.
        if. '' -.@-: $a do. hdr , 'x must be an atom' return. end.
        if. -. a e. _4 _1 0 1 2 3 4 5 6 do.  hdr , 'x must select a valid function' return. end.
      end.
    case. ;:'q:\\.' do.
      if. e=EVDOMAIN do.
        if. #emsg=. a efcknumericargs w  do. hdr,emsg return. end.
        if. #emsg=. 'x has '&,^:(*@#) a efindexmsg a 9!:23 (0;0$0) do. hdr,emsg return. end.  NB.nonintegral value
      end.
    case. ;:'s:x:u:' do.
NB. most decoding omitted
      if. e=EVDOMAIN do. if. (~:''-:$a) +. a 9!:23 (0;0$0) do. hdr,'x must be an integer atom' return. end. end.
    case. ;:'T.' do.
      if. e=EVNONCE do. hdr,'your system does not support threading' return. end.
      if. e=EVDOMAIN do. if. (~:''-:$a) +. a 9!:23 (0;0$0) do. hdr,'x must be an integer atom' return. end. end.
      select. ''$a
      case. 4 do. if. e=EVDOMAIN do. hdr,'y must be boxes whose status is to be tested' return. end.
      case. 5 do.
        if. e=EVDOMAIN do. hdr,'y must be timeout value in seconds' return. end.
        if. e=EVLENGTH do. hdr,'y must be a numeric atom holding timeout value in seconds' return. end.
        if. e=EVLIMIT do. hdr,'timeout must not exceed 9e9 unless infinite' return. end.
      case. 6 do.
        if. e e. EVDOMAIN,EVRANK,EVLENGTH do. hdr,'y must be pyx;value' return. end.
        if. e=EVRO do. hdr,'the value of the pyx has already been set' return. end.
      case. 7 do.
        if. e e. EVDOMAIN,EVRANK,EVLENGTH do. hdr,'y must be pyx;atomic error code 1-255' return. end.
        if. e=EVRO do. hdr,'the value of the pyx has already been set' return. end.
      case. 2;15 do.
        if. e e. EVRANK,EVLENGTH do. hdr,'y must be an integer atom or empty' return. end.
        if. e=EVLIMIT do. hdr,'invalid thread number' return. end.
      case. 8;1;3;55 do.
        if. e e. EVRANK,EVLENGTH do. hdr,'y must be empty' return. end.
      case. 14 do.
        if. e e. EVRANK,EVLENGTH do. hdr,'y must be thread#,timeout' return. end.
        if. e=EVDOMAIN do.
          if. #emsg=. efcknumericargs w do. hdr,'y is ' , emsg return. end.   NB. must be numeric
          if. (~: <.) {.w do. hdr,'thread# is nonintegral' return. end.
          if. 0 > {:w do. hdr,'timeout is negative' return. end.
        end.
        if. e = EVLIMIT do. hdr,'thread# exceeds system limit' return. end.
      case. 0 do.
        if. e e. EVRANK,EVLENGTH do. hdr,'pool# must be an integer atom or empty' return. end.
        if. e=EVLIMIT do. hdr,'too many threads for pool or system' return. end.
        if. e=EVFACE do. hdr,'the OS refused to create a thread' return. end.
        if. e=EVSIDAMAGE do. hdr,'thread creation not allowed during debug suspension' return. end.
      case. 10;11;13;16;17;18 do.   NB. no decoding for these yet
      case. do. hdr,'unknown x value' return.
      end.
    case. ;:'Z:' do.
      if. e=EVSYNTAX do. hdr,'fold is not running' return. end.
    case. ;:'@.' do.
      if. ism do.  NB. the errors in @. must include the selectors
        if. e=EVRANK do. if. (#@$ind) > a >.&(#@$) w do.
          hdr,'the rank of the selectors (' , (":#$ind) , ') must not exceed the larger argument rank (' , (":#a) , ' and ' , (":#$w) , ')' return. end.
        end.
        if. e=EVDOMAIN do. if. #emsg=. efcknumericargs ind do. hdr,'selector is ' , emsg return. end. end.  NB. must be numeric
        if. e=EVLENGTH do.
          if. #emsg=.efckagree a;ind;0 0;63 63 do. hdr,'x''s and selectors'' ',emsg return. end.
          if. #emsg=.efckagree w;ind;0 0;63 63 do. hdr,'y''s and selectors'' ',emsg return. end.
        end.
        if. e=EVINDEX do.
          ngerunds=.# efarnounvalue {. args  NB. number of gerunds
          if. #emsg=. 'selector has '&,^:(*@#) ind efindexmsg ind 9!:23 (0;(- , <:) ngerunds) do. hdr,emsg return. end.
        end.
      end.
    end.
  else.

    NB. ******* Monads - but the argument is called a *******
    select. prim
    case. ;:'<.<:>.>:++:**:-%%:^^.|!j.H.??.' do.  NB. atomic dyads and u"v
      NB. Primitive atomic verb.  Check for agreement
      if. e=EVDOMAIN do.
        if. #emsg=. efcknumericargs a  do. hdr,'y is ',emsg return. end.
        if. prim e. ;:'??.' do.
          if. #emsg=. a efindexmsg a 9!:23 (0;1) do. hdr,'y must be a positive integer' return. end.
        end.
      elseif. e=EVNAN do.
        if. prim -: <,'!' do. hdr,'!__ is not well defined' return. end.
      end.
    case. ;:'>;{' do.  NB. only error is incompatible args, but could be with fill also
      if. (e e. EVDOMAIN , EVINHOMO) do.  NB. domain /inhomo
        if. 1 < #types =. a. -.~ efhomo (,&(*@(#@,) * 3!:0))@> a do. emsg =. 'contents are incompatible: ' , efandlist types
        elseif. 1=#fill do.
          if. 1 < #types =. ~. types , efhomo 3!:0 fill do. emsg =. 'contents and fill are incompatible: ' , efandlist types end.
        end.
      end.
    case. ;:'|.' do.
      if. e=EVINHOMO do. emsg =. 'argument and fill are incompatible: ' , efandlist w efhomo@:(,&(*@(#@,) * 3!:0)) fill end.
    case. ;:'#.#:' do.
      if. e=EVDOMAIN do. if. #emsg=. a efcknumericargs w  do. hdr,'y is ',emsg return. end. end.
    case. ;:'{.{:' do.
      if. e=EVINHOMO do. hdr ,  'y argument and fill are incompatible: ' , efandlist w efhomo@:(,&(*@(#@,) * 3!:0)) fill return. end.
NB. } xy homo ind domain (incl fill) and index x/ind agreement
    case. ;:'b.' do.
      if. e=EVDOMAIN do.
        dom=.(16 <: efarnounvalue 0{args) {:: 0 1;0$0  NB. if m<16, domain is boolean, else integer
        if. #emsg=. 'y has '&,^:(*@#) a efindexmsg a 9!:23 (0;dom) do. hdr,emsg return. end.  NB. incorrect type of x or nonintegral
      end.
NB. } x domain
    case. ;:'".' do.
      if. e=EVRANK do. if. 1<#@$a do. hdr,'y must be a list of characters' return. end. end.
      if. e=EVDOMAIN do.
        select. 3!:0 a
        case. 131072;262144 do. hdr,'y must be convertible to byte precision' return.
        case. 2 do.
        case. do. hdr,'y must be a list of characters' return.
        end.
      end.
    case. ;:'??.p:q:' do.
      if. e=EVDOMAIN do.
        if. #emsg=. a efcknumericargs w  do. hdr,'y is ',emsg return. end.
        if. #emsg=. a efindexmsg a 9!:23 (0;0) do. hdr,'y must be a nonnegative integer' return. end.
      end.
    case. ;:'A.C.' do.
      if. e=EVINDEXDUP do. hdr , ('a permutation in ' #~ 1<*/}:$a) , 'y contains a duplicate value' return. end.
      if. e e. EVDOMAIN,EVINDEX do.
        if. 32 ~: 3!:0 a do.
          if. #emsg=. efcknumericargs a  do. hdr,'y is ',emsg return. end.
          permord =. <. >./ a
          if. #emsg=. 'y has '&,^:(*@#) a efindexmsg a 9!:23 (0;(_1&- , ]) permord) do. hdr,emsg return. end.  NB. direct form
        else.
          if. a: +./@:~: ebox =. efcknumericargs&.> a do.
            epos =. ($ebox) #: a: i.&1@:~: ,ebox
            cmsg =. (' of ' ,~ 'box ' , ])^:(*@#) ":epos
            hdr,'contents of ' , cmsg , 'permutation y is ' , (<epos){:: ebox return.
          end.
          permord =. <. >./ >./@> , a
          if. 1 e. , iserr =. (0~:0&{::)@> eall =. (9!:23&(0;(_1&- , ]) permord))&.> a do.
            enx =. ($ #: i.&1@,) iserr
            if. #emsg=. 'cycle (' , (":enx) , '} of permutation y has ' , ((<enx){::a) efindexmsg ((<enx){::eall) do. hdr,emsg return. end.
          end.
        end.
      end.
    case. ;:'i.' do.
      if. e=EVDOMAIN do. if. #emsg=. 'y has '&,^:(*@#) a efindexmsg a 9!:23 (0;0) do. hdr,emsg return. end. end.
    case. ;:'i:' do.
      if. e=EVDOMAIN do.
        if. #emsg=. efcknumericargs a do. hdr,'y is ',emsg return. end.
        hdr,'number of steps must be a positive integer' return.
      end.
    case. ;:'I.' do.
      if. e=EVDOMAIN do. if. #emsg=. efcknumericargs a  do. hdr,'y is ',emsg return. end. end.  NB. complex case not decoded
    case. ;:'p.' do.
      if. #emsg=.efauditpoly a do. hdr,emsg return. end.
    case. ;:'u:' do.
      if. e=EVINDEX do. if. -. (3!:0 a) e. 2 131072 262144 do. if. #emsg=. a efindexmsg a 9!:23 (0;_65536 65535) do. hdr,emsg return. end. end. end.
    case. ;:'x:' do.
      if. e=EVDOMAIN do. if. #emsg=. efcknumericargs a  do. hdr,'y is ',emsg return. end. end.
    case. ;:'/' do.
      if. e=EVDOMAIN do. if. 0=#a do. hdr,'y is empty but the verb has no identity element' return. end. end.
    case. ;:'@.' do.
      if. ism do.  NB. if selectors given, the error is in the selection stage
        if. e=EVRANK do. if. (#@$ind) > >.&(#@$) a do.
          hdr,'the rank of the selectors (' , (":#$ind) , ') must not exceed the argument rank (' , (":#a) , ')' return. end.
        end.
        if. e=EVDOMAIN do. if. #emsg=. efcknumericargs ind do. hdr,'selector is ' , emsg return. end. end.  NB. must be numeric
        if. e=EVLENGTH do.
          if. #emsg=.efckagree a;ind;0 0;63 63 do. hdr,'y''s and selectors'' ',emsg return. end.
        end.
        if. e=EVINDEX do.
          ngerunds=.# efarnounvalue {. args  NB. number of gerunds
          if. #emsg=. 'selector has '&,^:(*@#) ind efindexmsg ind 9!:23 (0;(- , <:) ngerunds) do. hdr,emsg return. end.
        end.
      else.  NB. error following selection
        if. e=EVRANK do. hdr,'each gerund must return a result whose shape agrees with its inputs ' , emsg return. end.   NB. must be numeric
      end.
    end.
  end.
case. 2 do.
  NB. Executing modifiers.  select cases only.  a and w are ARs
  select. prim
  fcase. ;:'@:&:&.&.: :. F.F..F.:F:F:.F::' do.
    if. e=EVDOMAIN do. if. efarisnoun w do. hdr,'right argument of this modifier must be a verb' return. end. end.
  case. ;:'@ ::' do.
    if. e=EVDOMAIN do. if. efarisnoun a do. hdr,'left argument of this modifier must be a verb' return. end. end.
  case. ;:'L:' do.
    if. e=EVDOMAIN do. if. efarisnoun a do. hdr,'left argument of this modifier must be a verb' return. end. end.
    if. e=EVDOMAIN do.
      if. -. efarisnoun w do. hdr,'right argument of this modifier must be a noun' return. 
      elseif. #emsg=. a efindexmsg a 9!:23 (0;0$0) do. hdr,'level number ',emsg return.
      end.
    end.
  end.
end.
NB. not yet specifically diagnosed error
if. (0=#emsg) *. e=EVOFLO do. hdr , 'an operation on fixed-size integers does not fit in that size' return. end.
if. (0=#emsg) *. e=EVNAN do. hdr , 'you have calculated the equivalent of _-_ or _%_' return. end.
if. (0=#emsg) *. e=EVNONCE do. hdr , 'this computation is not yet supported' return. end.
if. (0=#emsg) *. e=EVTYPECHG do. hdr , 'a name changed part of speech since definition, usually because an undefined name was taken to be a verb' return. end.
if. (0=#emsg) *. e=EVNORESULT do. hdr , 'a fold did not execute anything that could contribute to the result' return. end.
(}:^:(0=#emsg) hdr) , emsg return.  NB. if we have a line, return it; otherwise remove LF from hdr to avoid empty line
}}




