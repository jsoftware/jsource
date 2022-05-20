prolog './g421.ijs'
NB. f/. f\  f\. models --------------------------------------------------

randuni''

en     =: #@]
em     =: (en >.@% -@[)`(en 0&>.@>:@- [) @. (0&<:@[)
kay    =: en`em @. (0&<@[)
omask  =: (em,en) $ ($&0@|@[ , $&1@kay)

base   =: 1&>.@-@[ * i.@em
iind   =: base ,. |@[ <. en - base
seg    =: ((+i.)/@[ { ])"1 _

infix  =: 1 : '(iind  u@seg ])"0 _'
outfix =: 1 : '(omask u@#   ])"0 _'
prefix =: 1 : '>:@,.@i.@# u@{. ]'
suffix =: 1 : ',.@i.@#    u@}. ]'

key    =: 1 : '=@[ u@# ]'
key0   =: 1 : '=!.0@[ u@# ]'

osub   =: >@]`(>@[ >@:{ ]) @. (*@#@])
oind   =: (+/&i./ </.&, i.)@(2&{.)@(,&1 1)@$
ob     =: 1 : 'oind u@osub"0 1 ,@(<"_2)'

bs     =: 1 : '(u prefix) : (u infix )'
bsd    =: 1 : '(u suffix) : (u outfix)'
sd     =: 1 : '(u ob    ) : (u key   )'


NB.  f/.y ---------------------------------------------------------------

NB. Sequential search
(4 3$0 1 5 2 6 0 3 7 0 4 8 0) -: 1 1 2 3 4 1 2 3 4 ]/. i. 9
(4 3$0 1 5 2 6 0 3 7 0 4 8 0) -: 1 1 2 3 4 1 2 3 4. ]/. i. 9
(4 3$0 1 5 2 6 0 3 7 0 4 8 0) -: 1 1 2 3 4 1 2 3 4. ]/.!.0 i. 9
(4 3$0 1 5 2 6 0 3 7 0 4 8 0) -: 'aabcdabcd' ]/.!.0 i. 9
(4 3$0 1 5 2 6 0 3 7 0 4 8 0) -: (u: 'aabcdabcd') ]/.!.0 i. 9
(4 3$0 1 5 2 6 0 3 7 0 4 8 0) -: (10 u: 'aabcdabcd') ]/.!.0 i. 9
(4 3$0 1 5 2 6 0 3 7 0 4 8 0) -: (o. 1 1 2 3 4 1 2 3 4.) ]/.!.0 i. 9
(4 3$0 1 5 2 6 0 3 7 0 4 8 0) -: (o. 1 1 2 3 4 1 2 3 4.) ]/. i. 9
(4 3$0 1 5 2 6 0 3 7 0 4 8 0) -: (<"0 'aabcdabcd') ]/.!.0 i. 9
(4 3$0 1 5 2 6 0 3 7 0 4 8 0) -: (1 1 2 3 4 1 2 3 4x) ]/. i. 9
(4 3$0 1 5 2 6 0 3 7 0 4 8 0) -: (1 1 2 3 4r3 1 2 3 4r3) ]/. i. 9
(~. =&# </.~) 1 + 1e_14 * i. 10
(~.!.0 =&# </.!.0~) 1 + 1e_14 * i. 10
(~. =&# </.~) 1 + 1e_14 * i. 15
(~.!.0 =&# </.!.0~) 1 + 1e_14 * i. 15




NB. Boolean
a=:1=?10 5$2
(</.   -: < ob) a
(</.   -: < ob) ,a
(]/.   -: ] ob) a
(+.//. -: +./ ob) a
(</.   -: < ob) 1
(</.   -: < ob) 0$1

NB. literal
a=:a.{~32+?10 5 3$95
(</. -: < ob) a
(</. -: < ob) ,a
(]/. -: ] ob) a
(</. -: < ob) 'a'
(</. -: < ob) ''

NB. literal2
a=:adot1{~32+?10 5 3$95
(</. -: < ob) a
(</. -: < ob) ,a
(]/. -: ] ob) a
(</. -: < ob) u:'a'
(</. -: < ob) u:''

NB. literal4
a=:adot2{~32+?10 5 3$95
(</. -: < ob) a
(</. -: < ob) ,a
(]/. -: ] ob) a
(</. -: < ob) 10&u:'a'
(</. -: < ob) 10&u:''

NB. symbol
a=:sdot0{~32+?10 5 3$95
(</. -: < ob) a
(</. -: < ob) ,a
(]/. -: ] ob) a
(</. -: < ob) s:@<"0 'a'
(</. -: < ob) s:''

NB. integer
a=:?3 4 5 6$100
(</.  -: < ob) a
(</.  -: < ob) ,a
(]/.  -: ] ob) a
(+//. -: +/ ob) a
(</.  -: < ob) i.0 7
(</.  -: < ob) i.7 0 4
(</.  -: < ob) i.0 0 4 5
(</.  -: < ob) 34
(</.  -: < ob) 0$34

NB. floating point
a=:o._40+?10 5$100
(</.  -: < ob) a
(</.  -: < ob) ,a
(]/.  -: ] ob) a
1e_12 > | (+//. - +/ ob) a  NB. roundoff
(</.  -: < ob) _3.454
(</.  -: < ob) 0$_3.4

NB. complex
a=:^0j0.01*_400+?10 5$1000
(</.  -: < ob) a
(</.  -: < ob) ,a
(]/.  -: ] ob) a
(+//. -: +/ ob) a
(</.  -: < ob) _3j454
(</.  -: < ob) 0$_3j4

NB. boxed
t=:(1=?70$3)<;.1 ?70$100
a=:t{~?10 3$#t
(</. -: < ob) a
(</. -: < ob) ,a
(]/. -: ] ob) a
(</. -: < ob) <i.3 4
(</. -: < ob) 0$<'_3j4'

NB. 4 7 -: $ i.@7:/. i. 2 3    withdrawn
NB. 4 7 -: $ i.@7:/. i. 2 3 4
NB. 3 7 -: $ i.@7:/. i. 1 3
NB. 3 7 -: $ i.@7:/. i. 1 3 4
NB. 2 7 -: $ i.@7:/. i. 0 3
NB. 2 7 -: $ i.@7:/. i. 0 3 4
NB. 1 7 -: $ i.@7:/. i. 2 0
NB. 1 7 -: $ i.@7:/. i. 2 0 4
NB. 0 7 -: $ i.@7:/. i. 1 0
NB. 0 7 -: $ i.@7:/. i. 1 0 4
NB. 0 7 -: $ i.@7:/. i. 0 0
NB. 0 7 -: $ i.@7:/. i. 0 0 4

NB.  f/.y convolution ---------------------------------------------------

pru=: [: ^ 0j2p1&%               NB. principal n-th root of unity

conv=: +//.@(*/)                 NB. convolution

conv1=: 4 : 0
 n=. #x
 A=. (pru 2*n)^*/~i.2*n          NB. (%.A) = (2*n)%~r^-*/~i.2*n  
 }: x *&.(A&(+/ .*))&((2*n)&{.) y
)

pconv=: (| +/~@i.)@#@[ +//.&, */ NB. positive wrapped convolution

pconv1=: 4 : 0
 n=. #x
 A=. (*:pru 2*n)^*/~i.n          NB. (%.A) = n%~r^-*/~i.n
 x *&.(A&(+/ .*)) y
)

s=: ?10$100
t=: ?10$100
eq=: 1e_8&> @: (>./) @: | @: -
s ( conv eq  conv1) t
s (pconv eq pconv1) t


NB.  x f/. y ------------------------------------------------------------

NB. Long subarrays
a=: (2^_12) * _4e5+10000?@$1e6
k (</.   -: < key) a    [ k=:?(#a)$400
k (]/.   -: ] key) a    [ k=:?(#a)$400
k (<@:+:/.  -: <@:+: key) a     [ k=:?(#a)$400
k ((- -)/.  -: (- -) key) a     [ k=:?(#a)$400
k (+.//. -: +./ key) a  [ k=:?(#a)$400

a=: (2^_12) * _4e5+10000 2?@$1e6
k (</.   -: < key) a    [ k=:?(#a)$400
k (]/.   -: ] key) a    [ k=:?(#a)$400
k (<@:+:/.  -: <@:+: key) a     [ k=:?(#a)$400
k ((- -)/.  -: (- -) key) a     [ k=:?(#a)$400
k (+.//. -: +./ key) a  [ k=:?(#a)$400

a=: (2^_12) * _4e5+10000 5?@$1e6
k (</.   -: < key) a    [ k=:?(#a)$400
k (]/.   -: ] key) a    [ k=:?(#a)$400
k (<@:+:/.  -: <@:+: key) a     [ k=:?(#a)$400
k ((- -)/.  -: (- -) key) a     [ k=:?(#a)$400
k (+.//. -: +./ key) a  [ k=:?(#a)$400

NB. Integer lists that must not inplace
a=: ?1000$110
k (%/.   -: % key) a    [ k=:?(#a)$5
k (</.   -: < key) a    [ k=:?(#a)$5
k (]/.   -: ] key) a    [ k=:?(#a)$5
k (<@:+:/.  -: <@:+: key) a     [ k=:?(#a)$5
k ((- -)/.  -: (- -) key) a     [ k=:?(#a)$5
k (+.//. -: +./ key) a  [ k=:?(#a)$5

NB. Boolean
a=:1=?11 5$2
k (</.   -: < key) a    [ k=:?11$4
k (]/.   -: ] key) a    [ k=:?11$4
k (<@:+:/.  -: <@:+: key) a     [ k=:?11$4
k ((- -)/.  -: (- -) key) a     [ k=:?11$4
k (+.//. -: +./ key) a  [ k=:?11$4

NB. literal
a=:a.{~32+?11$95
k (</. -: < key) a      [ k=:?11$4
k (]/. -: ] key) a      [ k=:?11$4

NB. literal2
a=:adot1{~32+?11$95
k (</. -: < key) a      [ k=:?11$4
k (]/. -: ] key) a      [ k=:?11$4

NB. literal4
a=:adot2{~32+?11$95
k (</. -: < key) a      [ k=:?11$4
k (]/. -: ] key) a      [ k=:?11$4

NB. symbol
a=:sdot0{~32+?11$95
k (</. -: < key) a      [ k=:?11$4
k (]/. -: ] key) a      [ k=:?11$4

NB. integer
a=:?11 5$110
k (</.  -: < key) a     [ k=:?11$4
k (]/.  -: ] key) a     [ k=:?11$4
k (<@:+:/.  -: <@:+: key) a     [ k=:?11$4
k ((- -)/.  -: (- -) key) a     [ k=:?11$4
k (+//. -: +/ key) a    [ k=:?11$4

NB. floating point
a=: (2^_12) * _4e5+?11$1e6
k (</.  -: < key) a     [ k=:?11$4
k (]/.  -: ] key) a     [ k=:?11$4
k (<@:+:/.  -: <@:+: key) a     [ k=:?11$4
k ((- -)/.  -: (- -) key) a     [ k=:?11$4
k (+//. -: +/ key) a    [ k=:?11$4

NB. complex
a=: j./_4e5+?2 11 5$1e6
k (</.  -: < key) a     [ k=:?11$4
k (]/.  -: ] key) a     [ k=:?11$4
k (<@:+:/.  -: <@:+: key) a     [ k=:?11$4
k ((- -)/.  -: (- -) key) a     [ k=:?11$4
k (+//. -: +/ key) a    [ k=:?11$4

NB. boxed
a=:x{~?11 3$#x=:(1=?70$3)<;.1 ?70$110
k (</. -: < key) a      [ k=:?11$4
k (]/. -: ] key) a      [ k=:?11$4

NB. Check different types of key-searches
NB. Intolerant
a=:?120 5$110
k (</.  -: < key) a     [ k=:(#a)?@$1e8
k (</.  -: < key) a     [ k=:a. {~ ((#a),2)?@$#a.
k (</.  -: < key) a     [ k=:a. {~ ((#a),3)?@$#a.
k (</.  -: < key) a     [ k=:a. {~ ((#a),4)?@$#a.
k (</.  -: < key) a     [ k=:a. {~ ((#a),5)?@$#a.
k (</.  -: < key) a     [ k=:4 u: (#a)?@$65536
k (</.  -: < key) a     [ k=:10 u: (#a)?@$1e6
k (</.!.0  -: < key0) a [ k=:0.5+(#a)?@$1e8
k (</.!.0  -: < key0) a [ k=:j.~ (#a)?@$1e8
NB. Tolerant
k (</.  -: < key) a     [ k=:(#a)?@$0
k (</.  -: < key) a     [ k=:j.~ (#a)?@$0
NB. Small-range
k (</.  -: < key) a     [ k=:(#a)?@$100
k (</.  -: < key) a     [ k=:_1000 + (#a)?@$100
k (</.  -: < key) a     [ k=:1000 + (#a)?@$100
NB. Boxed
k (</.  -: < key) a     [ k=:('abc' ;~ ])&.> 1000 + (#a)?@$100
NB. Sequential
a=:?10 5$110
k (</.  -: < key) a     [ k=:(#a)?@$100
NB. Arrays
a=:?10 5$110
k (</.  -: < key) a     [ k=:((#a),2)?@$100
k (</.  -: < key) a     [ k=:((#a),2)?@$2
k (</.  -: < key) a     [ k=: 'ab' {~ ((#a),2)?@$2
k (</.  -: < key) a     [ k=:a. {~ ((#a),2)?@$100
NB. small arguments and x with rank > 1
(,3) -: (3 2$1) +//. i.3    NB. don't crash

NB. Intolerant
a=:?120 5$110
k (<@]/.  -: <@] key) a     [ k=:(#a)?@$1e8
k (<@]/.  -: <@] key) a     [ k=:a. {~ ((#a),2)?@$#a.
k (<@]/.  -: <@] key) a     [ k=:a. {~ ((#a),3)?@$#a.
k (<@]/.  -: <@] key) a     [ k=:a. {~ ((#a),4)?@$#a.
k (<@]/.  -: <@] key) a     [ k=:a. {~ ((#a),5)?@$#a.
k (<@]/.  -: <@] key) a     [ k=:4 u: (#a)?@$65536
k (<@]/.  -: <@] key) a     [ k=:10 u: (#a)?@$1e6
k (<@]/.!.0  -: <@] key0) a [ k=:0.5+(#a)?@$1e8
k (<@]/.!.0  -: <@] key0) a [ k=:j.~ (#a)?@$1e8
NB. Tolerant
k (<@]/.  -: <@] key) a     [ k=:(#a)?@$0
k (<@]/.  -: <@] key) a     [ k=:j.~ (#a)?@$0
NB. Small-range
k (<@]/.  -: <@] key) a     [ k=:(#a)?@$100
k (<@]/.  -: <@] key) a     [ k=:_1000 + (#a)?@$100
k (<@]/.  -: <@] key) a     [ k=:1000 + (#a)?@$100
NB. Boxed
k (<@]/.  -: <@] key) a     [ k=:('abc' ;~ ])&.> 1000 + (#a)?@$100
NB. Sequential
a=:?10 5$110
k (<@]/.  -: <@] key) a     [ k=:(#a)?@$100
NB. Arrays
a=:?10 5$110
k (<@]/.  -: <@] key) a     [ k=:((#a),2)?@$100
k (<@]/.  -: <@] key) a     [ k=:((#a),2)?@$2
k (<@]/.  -: <@] key) a     [ k=: 'ab' {~ ((#a),2)?@$2
k (<@]/.  -: <@] key) a     [ k=:a. {~ ((#a),2)?@$100

NB. small arguments and x with rank > 1
(,3) -: (3 2$1) +//. i.3    NB. don't crash

NB. Verify inplacing
NB. We can't reliably check the sizes because the call to i. allocates memory; also there are differences dep. type of result of u/.
NB. IF64 = '(0: + 0:/.~) +: a' (> 10000&+)&(7!:2) '0:/.~ +: a' [ a =: 10000 ?@$ 400   NB. The right side inplaces (64-bit only)
NB. '(0: + 0:/.~) +: a' (> 10000&+)&(7!:2) '0:/.~ +: a' [ a =: 10000 2 ?@$ 400   NB. The right side inplaces
NB. '(0: + 0:/.~) 2 #"1 a' (> 10000&+)&(7!:2) '0:/.~ 2 #"1 a' [ a =: 10 u: 10000 1 ?@$ 400   NB. The right side inplaces
NB. -. '(0: + 0:/.~) 1 #"1 a' (> 10000&+)&(7!:2) '0:/.~ 1 #"1 a' [ a =: 10 u: 10000 1 ?@$ 400   NB. Does not inplace
NB. -. '(0: + 0:/.~) 2 # a' (> 10000&+)&(7!:2) '0:/.~ 2 # a' [ a =: 10 u: 5000 ?@$ 400   NB. Does not inplace
NB. 

NB. Check that w can be reused if direct inplaceable, cells more than 4 bytes, non-virtual.  Just make sure incorrect types don't fail
yx =. 500 ((* #) $ ]) 1 2 3 4 5
(yx </. ({. , }.) yy) -: ((i. 5) +/ (5 * i. 500)) <@:{"1 _ yy =. 2500 $ 0 0 1 1 1 1
(yx </. ({. , }.) yy) -: ((i. 5) +/ (5 * i. 500)) <@:{"1 _ yy =. 2500 $ 'abcdef'
(yx </. ({. , }.) yy) -: ((i. 5) +/ (5 * i. 500)) <@:{"1 _ yy =. 2500 2 $ 'abcdef'
(yx </. ({. , }.) yy) -: ((i. 5) +/ (5 * i. 500)) <@:{"1 _ yy =. 2500 3 $ 'abcdef'
(yx </. ({. , }.) yy) -: ((i. 5) +/ (5 * i. 500)) <@:{"1 _ yy =. 2500 4 $ 'abcdef'
(yx </. ({. , }.) yy) -: ((i. 5) +/ (5 * i. 500)) <@:{"1 _ yy =. 2500 5 $ 'abcdef'
(yx </. ({. , }.) yy) -: ((i. 5) +/ (5 * i. 500)) <@:{"1 _ yy =. 2500 $ 3 1 4 1 5 9
(yx </. ({. , }.) yy) -: ((i. 5) +/ (5 * i. 500)) <@:{"1 _ yy =. 2500 $ 3 1 4 1 5 9.2
(yx </. ({. , }.) yy) -: ((i. 5) +/ (5 * i. 500)) <@:{"1 _ yy =. 2500 $ 1 j. i. 6
(yx </. ({. , }.) yy) -: ((i. 5) +/ (5 * i. 500)) <@:{"1 _ yy =. 2500 $ <"0 ] 3 1 4 1 5 9
(yx </. ({. , }.) yy) -: ((i. 5) +/ (5 * i. 500)) <@:{"1 _ yy =. 2500 $ 3 1 4 1 5 9x
(yx </. ({. , }.) yy) -: ((i. 5) +/ (5 * i. 500)) <@:{"1 _ yy =. 2500 $ 3 1 4 1 5 9r2
(yx </. ({. , }.) yy) -: ((i. 5) +/ (5 * i. 500)) <@:{"1 _ yy =. 2500 $ s: ' 3 1 4 1 5 9'
(yx </. ({. , }.) yy) -: ((i. 5) +/ (5 * i. 500)) <@:{"1 _ yy =. 2500 $ 7 u: 3 1 4 1 5 9
(yx </. ({. , }.) yy) -: ((i. 5) +/ (5 * i. 500)) <@:{"1 _ yy =. 2500 $ 9 u: 3 1 4 1 5 9

''        -: '' </. ''
''        -: '' </. i.0 4 5
''        -: (i.0 4 5) </. ''
''        -: (u:'') </. u:''
''        -: (u:'') </. i.0 4 5
''        -: (i.0 4 5) </. u:''
''        -: (10&u:'') </. 10&u:''
''        -: (10&u:'') </. i.0 4 5
''        -: (i.0 4 5) </. 10&u:''
''        -: (s:'') </. s:''
''        -: (s:'') </. i.0 4 5
''        -: (i.0 4 5) </. s:''
(,<,5)    -: 4 </. 5
(,<i.1 9) -: 4 </. i.1 9
(,<,4)    -: (i.1 9) </. 4
(,<,4)    -: (i.1 0) </. 4
(,<x )    -: (i.(#x),0) </. x=:'abcdefghij'
(,<x )    -: (i.(#x),0) </. x=:u:'abcdefghij'
(,<x )    -: (i.(#x),0) </. x=:10&u:'abcdefghij'
(,<x )    -: (i.(#x),0) </. x=:s:@<"0 'abcdefghij'


'length error' -: 'abc'  </. etx i.4
'length error' -: (u:'abc')  </. etx i.4
'length error' -: (10&u:'abc')  </. etx i.4
'length error' -: (s:@<"0 'abc')  </. etx i.4
'length error' -: 'abcd' </. etx i.3
'length error' -: (u:'abcd') </. etx i.3
'length error' -: (10&u:'abcd') </. etx i.3
'length error' -: (s:@<"0 'abcd') </. etx i.3
'length error' -: ''     </. etx i.4
'length error' -: 4      </. etx i.4
'length error' -: 'abcd' </. etx 4
'length error' -: (u:'abcd') </. etx 4
'length error' -: (10&u:'abcd') </. etx 4
'length error' -: (s:@<"0 'abcd') </. etx 4

(,<,1) -: 1 1 1 1 (<@~.)/.!.0 (1) + 1e_15 * i. 4
(4#<,1) -: (<@~.)/.!.0~ (1) + 1e_15 * i. 4
(,<,1) -: 1 1 1 1 (<@~.)/. (1) + 1e_15 * i. 4


NB. x f/.y on empty x ---------------------------------------------------

test=: 2 : 0
 n=: p: ?200
 xx=: n ?@$ >.-:n
 assert. xx             (u f./. -: v/.) i.n,0
 assert. ((n,0)$0     ) (u f./. -: v/.) i.n,0
 assert. ((n,0)$0     ) (u f./. -: v/.) i.n
 assert. ((n,0)$''    ) (u f./. -: v/.) i.n
 assert. ((n,0)$u: 0  ) (u f./. -: v/.) i.n
 assert. ((n,0)$10&u: 0) (u f./. -: v/.) i.n
 assert. ((n,0)$2     ) (u f./. -: v/.) i.n
 assert. ((n,0)$2.5   ) (u f./. -: v/.) i.n
 assert. ((n,0)$2j5   ) (u f./. -: v/.) i.n
 assert. ((n,0)$2x    ) (u f./. -: v/.) i.n
 assert. ((n,0)$2r5   ) (u f./. -: v/.) i.n
 assert. ((n,0)$a:    ) (u f./. -: v/.) i.n
 assert. ((n,0)$s:<'x') (u f./. -: v/.) i.n
 assert. ((n,0)$s:<u: 128+a.i. 'x') (u f./. -: v/.) i.n
 assert. ((n,0)$s:<10&u: 65536+a.i. 'x') (u f./. -: v/.) i.n
 1
)

<       test (3 : '<y')
#       test (3 : '#y')
+ /     test (3 : '+ /y')
>./     test (3 : '>./y')
<./     test (3 : '<./y')
({.,# ) test (3 : '({.,# )y')
(# ,{.) test (3 : '(# ,{.)y')

NB. Type of empty result
4 -: 3!:0 #/.~ ''
4 -: 3!:0 #/.~ 0$1x  NB. Should be 64
4 -: 3!:0 #"#/.~ ''
4 -: 3!:0 #"#/.~ 0$a:
64 -: 3!:0 #"#/.~ 0$1x
32 -: 3!:0 </.~ 0$a:
32 -: 3!:0 </.~ 0$''
32 -: 3!:0 </.~ 0$1x
8 -: 3!:0 ^./.~ 0$a:
8 -: 3!:0 ^./.~ 0$''
8 -: 3!:0 ^./.~ 0$1x
1 -: 3!:0 ] 5 0:\ 'abc'
2 -: 3!:0 ] 5 ]\ 'abc'

NB. x </. y
NB. Tested as part of ob above
NB. Check for memory loss
a =: 7!:0''
b =: 7!:0''
a =: 7!:0 ''
1: a. </. i. 256
b =: 7!:0''
b <: 512 + a  NB. Don't know where the 256 goes on assignment
1: </.~ i. 256
b =: 7!:0''
b <: 512 + a
c =: a. </. i. 256
4!:55<'c'
b =: 7!:0''
b <: 512 + a
c =: </.~ i. 256
4!:55<'c'
b =: 7!:0''
b <: 512 + a

4!:55 ;:'a adot1 adot2 sdot0 b base bs bsd conv conv1 em en eq iind infix k '
4!:55 ;:'kay key key0 n ob oind omask osub outfix pconv pconv1 prefix pru x yx yy'
4!:55 ;:'s sd seg suffix t test xx ' 
randfini''


epilog''

