prolog './g530m0.ijs'
NB. This version has removed all uses of multibox m} and should be considered the main line.
NB. g530 has the old code for compatibility testing

NB. m}y and u}y ---------------------------------------------------------

randuni''

g =. e.&' '@{.} @ (,:&'_')
h =. e.&' ' {"0 1 ,"0&'_'

(g -: h) 'Cogito, ergo sum.'
(g -: h) u:'Cogito, ergo sum.'
(g -: h) 10&u:'Cogito, ergo sum.'
(g -: h) 2 3 4$'Now is the time, all good mean, to ergo sum.'
(g -: h) 2 3 4$u:'Now is the time, all good mean, to ergo sum.'
(g -: h) 2 3 4$10&u:'Now is the time, all good mean, to ergo sum.'

g =. e.&(s:@<"0 ' ')@{.} @ (,:&(s:@<"0 '_'))
h =. e.&(s:@<"0 ' ') {"0 1 ,"0&(s:@<"0 '_')
(g -: h) s:@<"0 'Cogito, ergo sum.'
(g -: h) 2 3 4$s:@<"0 'Now is the time, all good mean, to ergo sum.'

((<0 1)&|: -: i.@}.@$}) i.,~?20
((<0 1)&|: -: i.@}.@$}) i.,~?20

'domain error' -: ex '''a''} 2 3 4'
'domain error' -: ex '(<0)} 2 3 4 '
'domain error' -: ex '2.1 } 2 3 4 '
'domain error' -: ex '2j1 } 2 3 4 '

'index error'  -:  3       } etx 2 3 4
'index error'  -: _4       } etx 2 3 4
'index error'  -:  3 1 2 0 } etx 3 4$'a'
'index error'  -: _4 1 2 0 } etx 3 4$'a'

'rank error'   -: 2        } etx 3 4$'a'
'rank error'   -: (3 4$0 1)} etx 3 4$'a'

'length error' -: 2 3} etx i.4 3
'length error' -: 2 3} etx i.4 1

'index error' -: (,0)} etx i. 0 1
'index error' -: (,1)} etx i. 1 1
({. -: (,0)}) i. 1 1


NB. a=: c}x,y,... ,:z ---------------------------------------------------

f=: 3 : 0
 b=: ?5 7 11 13 17$2
 c=: ?($b)$3
 select. y 
  case. 'B' do.
   xx=: ?($b)$2
   yy=: ?($b)$2
   zz=: ?($b)$2
  case. 'I' do.
   xx=: _500+?($b)$1000
   yy=: _500+?($b)$1000
   zz=: _500+?($b)$1000
  case. 'D' do.
   xx=: o._500+?($b)$1000
   yy=: o._500+?($b)$1000
   zz=: o._500+?($b)$1000
  case. 'Z' do. 
   xx=: j./_500+?(2,$b)$1000
   yy=: j./_500+?(2,$b)$1000
   zz=: j./_500+?(2,$b)$1000
  case. 'C' do.
   xx=: a.{~?($b)$#a.
   yy=: a.{~?($b)$#a.
   zz=: a.{~?($b)$#a.
  case. 'W' do.
   xx=: adot1{~?($b)$#adot1
   yy=: adot1{~?($b)$#adot1
   zz=: adot1{~?($b)$#adot1
  case. 'U' do.
   xx=: adot2{~?($b)$#adot2
   yy=: adot2{~?($b)$#adot2
   zz=: adot2{~?($b)$#adot2
  case. 'S' do.
   xx=: sdot0{~?($b)$#sdot0
   yy=: sdot0{~?($b)$#sdot0
   zz=: sdot0{~?($b)$#sdot0
  end.
 i.0 0
)

g0=: 3 : 0  NB. basic identities, boolean selection
 f y
 if. y e. 'CWUS' do.
 q=: ($yy) $ (b0{,xx) (b0=. I.,b)},yy
 else.
 q=: (xx*b)+yy*-.b
 end.
 dd=: (b)}yy,:xx
 aa=: b}yy,:xx
 yy=: b}yy,:xx
 assert. q -: dd
 assert. q -: aa
 assert. q -: yy
 1
)

g1=: 3 : 0  NB. basic identities, integer selection
 f y
 q=: (xx*0=c)+(yy*1=c)+zz*2=c
 dd=: (c)}xx,yy,:zz
 aa=: c}xx,yy,:zz
 yy=: c}xx,yy,:zz
 assert. q -: dd
 assert. q -: aa
 assert. q -: yy
 1
)

g2=: 3 : 0  NB. force new copy, boolean selection
 f y
 if. y e. 'CWUS' do.
 q=: ($yy) $ (b0{,xx) (b0=. I.,b)},yy
 else.
 q=: (xx*b)+yy*-.b
 end.
 p=: yy
 yy=: b}yy,:xx
 assert. q -: yy
 if. y e. 'CWUS' do.
 assert. q -: ($yy) $ (b0{,xx) (b0=. I.,b)},yy
 else.
 assert. q -: (xx*b)+p*-.b
 end.
 assert. -. p -: yy
 1
)

g3=: 3 : 0  NB. force new copy, integer selection
 f y
 if. y e. 'CWUS' do.
 q=: c{"0 1 xx,"0 1 yy ,"0 zz
 else.
 q=: (xx*0=c)+(yy*1=c)+zz*2=c
 end.
 p=: yy
 yy=: c}xx,yy,:zz
 assert. q -: yy
 if. y e. 'CWUS' do.
 assert. q -: c{"0 1 xx,"0 1 p,"0 zz
 else.
 assert. q -: (xx*0=c)+(p*1=c)+zz*2=c
 end.
 assert. -. p -: yy
 1
)

g4=: 3 : 0  NB. in place, boolean selection
 f y
 if. y e. 'CWUS' do.
 q=: ($yy) $ (b0{,xx) (b0=. I.,b)},yy
 else.
 q=: (xx*b)+yy*-.b
 end.
 t=: 7!:2 'yy=: b}yy,:xx'
 assert. q -: yy
 assert. IGNOREIFFVI t<IF64{2000 4000
 1
)

g5=: 3 : 0  NB. integer selection
 f y
 q=: (xx*0=c)+(yy*1=c)+zz*2=c
 yy=: c}xx,yy,:zz
 assert. q -: yy
 d =: 1+c
 z1=: 0+zz
 assert. 'index error' -: ex 'zz=: d}xx,yy,:zz'
 assert. zz -: z1
 1
)

g8=: 3 : 0  NB. force idiom misidentification, boolean selection
 f y
 xx=: {.,xx
 if. y e. 'CWUS' do.
 q=: ($yy) $ ((#b0)#xx) (b0=. I.,b)},yy
 else.
 q=: (xx*b)+yy*-.b
 end.
 aa=: b}yy,:xx
 yy=: b}yy,:xx
 assert. q -: yy
 assert. q -: aa
 1
)

g9=: 3 : 0  NB. force idiom misidentification, integer selection
 f y
 xx=: {.,xx
 q=: (xx*0=c)+(yy*1=c)+zz*2=c
 aa=: c}xx,yy,:zz
 yy=: c}xx,yy,:zz
 assert. q -: yy
 assert. q -: aa
 1
)

g10=: 3 : 0  NB. force idiom misidentification, boolean selection
 f y
 xx=: 5&,
 a=: b}xx,:yy
 assert. a -: (5*-.b) + b*yy
 1
)

g11=: 3 : 0  NB. force idiom misidentification, integer selection
 f y
 xx=: 5&,@((2,$c)&$)
 a=: c}xx,yy,:zz
 assert. a -: (5*0=c) + (yy*1=c) + zz*2=c
 1
)

g0 "0 'BIDZCWUS'
g1 "0 'BIDZ'
g2 "0 'BIDZCWUS'
g3 "0 'BIDZCWUS'
g4 "0 'BIDZCWUS'
g5 "0 'BIDZ'
g8 "0 'BIDZCWUS'
g9 "0 'BIDZ'
g10"0 'BIDZ'
g11"0 'BIDZ'

NB. Verify no local-to-global aliasing
f10 =: 3 : 'a =. y} a,:b'
f11 =: 3 : 'a =: y} a,:b'
f12 =: 3 : ('a =. 2 3';'a =: y} a,:b')
f13 =: 3 : ('a =. 2 3';'a =. y} a,:b')
a =: 0 1 2 3
b =: 9 8 7 6
9 1 7 3 -: f10 1 0 1 0
0 1 2 3 -: a
9 8 7 6 -: b
9 1 7 3 -: f11 1 0 1 0
9 1 7 3 -: a
9 8 7 6 -: b
b =: 9 8
'domain error' -: f12 etx 0 1
2 8 -: f13 0 1

NB. This used to fail (bad name check)
c =: 1 2 3 4
b =: 0 0 1 1
c1 =: 1 2 3 4
d1 =: 9 8 7 6
c =: b} c1,:d1
1 2 3 4 -: c1
1 2 7 6 -: c

NB. Don't modify readonly
 c1 =: |. ] d1 =: i. # ] b =: 1 0 0 0 1 0 0 1 1 1
 d1 =: b } d1,:c1
 0 1 2 3 4 5 6 7 8 9 -: i. 10

y=: 1 2 3
b=: 0 1 2
4!:55 ;:'x'
'value error'  -: ex 'x=:b}x,:y'
x=: 10 20 30
'index error'  -: ex 'x=:b}x,:y'
b=: 0 1
'length error' -: ex 'x=:b}x,:y'
b=: 'abc'
'domain error' -: ex 'x=:b}x,:y'
y=: 'abc'
b=: 0 1 0
'domain error' -: ex 'x=:b}x,:y'


NB. x m}y and x u}y -----------------------------------------------------

ia =. 1 : 'u@(i.@$@])'
f  =. '_'&((' '&= # i.@#)@,@]})
h  =. e.&' ' {"0 1 ,"0&'_'

'Cogito,*ergo*sum.' -: '*' (' '&= # i.@#)@]} 'Cogito, ergo sum.'

(f -: h) 'Cogito, ergo sum.'
(f -: h) 2 3 4$'Now is the time, all good mean, to ergo sum.'

(u:'Cogito,*ergo*sum.') -: (u:'*') (' '&= # i.@#)@]} u:'Cogito, ergo sum.'

(f -: h) u:'Cogito, ergo sum.'
(f -: h) 2 3 4$u:'Now is the time, all good mean, to ergo sum.'

(10&u:'Cogito,*ergo*sum.') -: (10&u:'*') (' '&= # i.@#)@]} 10&u:'Cogito, ergo sum.'

(f -: h) 10&u:'Cogito, ergo sum.'
(f -: h) 2 3 4$10&u:'Now is the time, all good mean, to ergo sum.'

(s:@<"0 'Cogito,*ergo*sum.') -: (s:@<"0 '*') ((s:@<"0 ' ')&= # i.@#)@]} s:@<"0 'Cogito, ergo sum.'

f1  =. (s:@<"0 '_')&(((s:@<"0 ' ')&= # i.@#)@,@]})
h1  =. e.&(s:@<"0 ' ') {"0 1 ,"0&(s:@<"0 '_')

(f1 -: h1) s:@<"0 'Cogito, ergo sum.'
(f1 -: h1) 2 3 4$s:@<"0 'Now is the time, all good mean, to ergo sum.'

C =. 2 : 'u & ((#i.@#)@,@v@] })'

(f -: '_' C (' '&=)) 'Cogito, ergo sum.'
(f -: '_' C (' '&=)) 2 3 4$'Now is the time, all good mean, to ergo sum.'
a -: ' ' C('_'&=) '_' C(' '&=) a =. 2 3 4$'Now is the time, all good mean, '

(f -: '_' C (' '&=)) u:'Cogito, ergo sum.'
(f -: '_' C (' '&=)) 2 3 4$u:'Now is the time, all good mean, to ergo sum.'
a -: ' ' C('_'&=) '_' C(' '&=) a =. 2 3 4$u:'Now is the time, all good mean, '

(f -: '_' C (' '&=)) 10&u:'Cogito, ergo sum.'
(f -: '_' C (' '&=)) 2 3 4$10&u:'Now is the time, all good mean, to ergo sum.'
a -: ' ' C('_'&=) '_' C(' '&=) a =. 2 3 4$10&u:'Now is the time, all good mean, '

(f1 -: (s:@<"0 '_') C ((s:@<"0 ' ')&=)) s:@<"0 'Cogito, ergo sum.'
(f1 -: (s:@<"0 '_') C ((s:@<"0 ' ')&=)) 2 3 4$s:@<"0 'Now is the time, all good mean, to ergo sum.'
a -: (s:@<"0 ' ') C ((s:@<"0 '_')&=) (s:@<"0 '_') C((s:@<"0 ' ')&=) a =. 2 3 4$s:@<"0 'Now is the time, all good mean, '

*./ (=@i. -: 1&((<0 1)&|:ia})@($&0)@(,~))"0 ?5$10

'abcX' -: 'X' _1}'abcd'
'abcX' -: 'X' _1}u:'abcd'
'abcX' -: 'X' _1}10&u:'abcd'

1   -: type 'a' ''}0$0
2   -: type 'a' ''}''
131072-: type 'a' ''}u:''
262144-: type 'a' ''}10&u:''
65536 -: type (s:@<"0 'a') ''}s:''
4   -: type 'a' ''}i.0
8   -: type 'a' ''}0$3.5
16  -: type 'a' ''}0$3j5
32  -: type 'a' ''}0$<3 5
64  -: type 'a' ''}0$3x
128 -: type 'a' ''}0$3r5

123 -: 123 (0)}456
123 -: 123 a: }456

(3 4$123) -: 123 a:   }?3 4$1000
(3 4$123) -: 123 (<$0)}?3 4$1000
(3 4$123) -: 123 (<'')}?3 4$1000

y=: ?100 50 7$1e6
i=: ?50 2$100 50
x=: ?50 7$1e6
(x (i)} y) -: x (i,"1 0"1 i.7)}y

i=: ?11$100
j=: ?13$50
k=: ?17$7
x=: ?11 13 17$1e6
(x (<i;j;k)}y) -: x (>{i;j;k)}y

test=: 4 : 'y -: x (i.0)}y'
y=: 0 1 1;'235';2 3 5;2 3.5 6;2 3j5 6;(2;3;5);(u: 'abc');s: ' a b c'
(0$&.>y) test&>/ y

y=: 0 1 1;'235';2 3 5;2 3.5 6;2 3j5 6;(2;3;5);(10&u: 'abc');s: ' a b c'
(0$&.>y) test&>/ y

y -: (0$a:) ($0)} y=: 5 ?@$ 1e6
y -: ''     ($0)} y
y -: (i.0)  ($0)} y
y -: (0$0.5)($0)} y
y -: (0$0j5)($0)} y

NB. integrated rank support
'foabar' -: 'ab' (,(0 1) +/~ ,2) } 'foobar'

NB. Test cell sizes and types

NB. integer indexing
NB. y is list of atoms to choose from
NB. x is rank of x, length of item of m, rank of y
test =: 4 : 0"1
'rx lim ry'   =: x
NB. Select agreeing args
ty   =: y {~ (2 + ry ?@$ 7) ?@$ #y  NB. random draw of atoms of y, with random shape
tm   =: , (>:@:? ? ]) lim {. $ ty  NB. , is scaf   so is >: use ?@>:
selshape   =: (#tm) , lim }. $ty
tx   =: y {~ ((-rx) {. selshape) ?@$ #y
NB. Verify amend works
mody   =: tx tm} ty
assert. tx (] -: (($,)~ $)) tm { mody
assert. ty -: (tm { ty) tm} mody
NB. Verify works inplace too
mody   =: ] (tm { ty) tm} mody  NB. saved value
mody   =: tx tm} mody
assert. tx (] -: (($,)~ $)) tm { mody
assert. ty -: mody   =: (tm { ty) tm} mody
1
)

NB. Repeat for all ranks
test1 =: 3 : 0
NB. rank of y is 1-5
NB. length of item of m is <: rank y, constrained to 1 for now
NB. rank of x is <: (rank of y) - (length of item of m) + 1
args =. <"1 ,. 1 2 3 4 5  NB. ry
args =. (1 , ])&.> args  NB. add on lim
args =. ; ((,~"_ 0   i.@>:@>:@(-~/))&.>) args  NB. add on lim
args test y
)

test1 0 1
test1 'abcdefghijklmno'
test1 0.4 + i. 10
test1 5 j. i. 10
test1 i. 10x
test1 1r2 + i. 10x
test1 (1;'a';4x) , <"0 i. 7
test1 2 u: 'abcdefghijklmno'
test1 10 u: 'abcdefghijklmno'
test1 s: ' now is the time for all good men'

NB. boxed-boxed-list indexing
NB. y is list of atoms to choose from
NB. x is rank of x, number of leading axes to take in m, rank of y
test =: 4 : 0"1
'rx lm ry'   =: x
NB. Select agreeing args
ty   =: y {~ (2 + ry ?@$ 7) ?@$ #y  NB. random draw of atoms of y, with random shape
tm   =: < (?@:>: ?&.> ]) lim {. $ ty  NB. one box per axis, all boxed
selshape   =: (# S:0 tm) , lim }. $ty
tx   =: y {~ ((-rx) {. selshape) ?@$ #y
NB. Verify amend works
mody   =: tx tm} ty
assert. tx (] -: (($,)~ $)) tm { mody
assert. ty -: (tm { ty) tm} mody
NB. Verify works inplace too
mody   =: ] (tm { ty) tm} mody  NB. saved value
mody   =: tx tm} mody
assert. tx (] -: (($,)~ $)) tm { mody
assert. ty -: mody   =: (tm { ty) tm} mody
1
)

NB. Repeat for all ranks
test1 =: 3 : 0
NB. rank of y is 1-5
NB. length of item of m is <: rank y, constrained to 1 for now
NB. rank of x is <: (rank of y) - (length of item of m)
args =. <"1 ,. 1 2 3 4 5  NB. ry
args =. <"1 ; ((,~"_ 0   i.@>:@{:)&.>) args  NB. add on lim
args =. ; ((,~"_ 0   i.@>:@(-~/))&.>) args  NB. add on lim
args test y
)

2 1 2 test i. 10
test1 0 1
test1 'abcdefghijklmno'
test1 0.4 + i. 10
test1 5 j. i. 10
test1 i. 10x
test1 1r2 + i. 10x
test1 (1;'a';4x) , <"0 i. 7
test1 2 u: 'abcdefghijklmno'
test1 10 u: 'abcdefghijklmno'
test1 s: ' now is the time for all good men'

NB. complementary indexing and axes in full
NB. x has an atom for each leading axis of y to be fetched from:
NB. 0=atom 1=list 2=complementary 3=axis in full
NB. we calculate a selector for the given x.  Then we amend with it, comparing against an exploded version
NB. We do amends with each possible frame of replacement data
test1 =: {{
axes =. 0$a: [ exaxes =. 0$0  NB. axes, exploded axes
for_i. x do.
 ys =. i_index { $y  NB. length of selected axis
 select. i
 case. 0 do. exaxis =. axis =. ?ys
 case. 1 do. exaxis =. axis =. (?ys) ? ys
 case. 2 do. exaxis =. (i.ys) -. >axis =. < (?ys) ? ys
 case. 3 do. exaxis =. i. ys [ axis =. a:
 end.
 axes =. axes , <axis [ exaxes =. exaxes ,"1 0/ exaxis
end.
NB. We have the selectors.
selshape =. (}:$exaxes) , ({:$exaxes)}.$y  NB. shape of m{y
NB. obsolete ax   =: axes
NB. obsolete exax   =: exaxes
ip =. 1 |. y  NB. inplaceable version
for_i. i.#selshape do.
 repldata =. (,y) {~ (i}.selshape)?@$ */@$ y
NB. obsolete rd   =: repldata
NB. 'repldata__ axes__ exaxes__ y__'   =: repldata;axes;exaxes;<y
 assert. repldata ((<axes)} -: (<^:(1=#@$) exaxes)}) y
 NB. Also execute inplace, to exercise usecount management
 ip =. repldata (<axes)} ip
end.
1
}}"1 _
NB. x is number of leading axes to test
test2 =: {{
((#: i.@(*/)) x # 4) test1 y
1
}}"0 _
0 1 2 3 4 5 test2 i. 4 3 4 5 7 6 3
0 1 2 3 4 5 test2 0. + i. 4 3 4 5 7 6 4
0 1 2 3  test2 0x + i. 4 3 4 5 3
0 1 2 3  test2 1r2 + i. 4 3 4 5 3
0 1 2 3 test2 <"0 i. 4 3 4 5 2
0 1 2 3 4 5 test2 (4 3 4 5 7 6 1) $ a.
0 1 2 3 4 5 test2 (4 3 4 5 7 6 2) $ a.
0 1 2 3 4 5 test2 (4 3 4 5 7 6 3) $ a.
0 1 2 3 4 5 test2 (4 3 4 5 7 6 4) $ a.
0 1 2 3 4 5 test2 (4 3 4 5 7 6 5) $ a.

_10 -: (_10) (<0$4)} 4

NB. if m is numeric table it is treated as index lists
test1 =: 1 : 0
:
NB. x is x arg, m is m, y is y
val1 =. x m} y
mb =. <"1 m
rs =. (#@$y) - {:@$ m  NB. rank of selection
xb =. ($mb) ($,) <"rs x
for_l. xb ,.&, mb do.
 'xo mo' =. l
 y =. xo (<mo)} y
end.
val1 -: y
)
4 (5)test1 i. 20
4 (2 1$5)test1 i. 20
4 5 (2 1$5)test1 i. 20
4 5 (2 1$5 6)test1 i. 20
'length error' -: 4 5 (2 1$5 6)test1 etx i. 8 9
4 (1 2$5 6)test1 i. 8 9
'length error' -: 4 5 (1 2$5 6)test1 etx i. 8 9
(1$4) (1 2$5 6)test1 i. 8 9
'length error' -: 4 5 (3 2$1 2 3 4 5 6)test1 etx i. 8 9
4 5 6 (3 2$1 2 3 4 5 6)test1 i. 8 9
4 5 6 (3 2$_1 2 3 _4 _5 _6)test1 i. 8 9
4 5 6 1 2 (3 2$_1 2 3 _4 _5 _6)test1 i. 8 9 5
(i. 3 5) (3 2$_1 2 3 _4 _5 _6)test1 i. 8 9 5
'length error' -: (i. 1 5) (3 2$_1 2 3 _4 _5 _6)test1 etx i. 8 9 5
'length error' -: (i. 4 5) (3 2$_1 2 3 _4 _5 _6)test1 etx i. 8 9 5
'rank error' -: (i. 1 4 5) (3 2$_1 2 3 _4 _5 _6)test1 etx i. 8 9 5
4 (] -: (i. 0 1)}) i. 20
4 (] -: (i. 0 1 1)}) i. 20
'length error' -: 4 (i.0 2)} etx i. 20
'length error' -: (i. 1 5) (2 4$_1 2 3 _4 1 1 1 1)test1 etx i. 8 9 5
(100 ?@$ 100) (? 100 4 $ 6 7 8 9)test1 i. 6 7 8 9 


'index error' -:  0 (i. 6)} etx 0

(0 1 (2 3)} i. 6 2) -: (i. 2 2)  ({.@[)`({:@[)`]} i. 6 2  NB. Someday this might result in virtual m}

'domain error' -: ex '7  ''a''} 2 3 4'
'domain error' -: ex '7 (<2.3)} 2 3 4'
'domain error' -: ex '7 ( 2.1)} 2 3 4'
'domain error' -: ex '7 ( 2j1)} 2 3 4'

'domain error' -: 1     i.@#@]} etx 'abc'
'domain error' -: (<'a')i.@#@]} etx 'abc'
'domain error' -: 'a'   i.@#@]} etx 2 3 4
'domain error' -: (<12) i.@#@]} etx 2 3 4
'domain error' -: 1     i.@#@]} etx 2;3 4
'domain error' -: 'ab'  i.@#@]} etx 2;3 4

'domain error' -: 1     (0 1)} etx 'abc'
'domain error' -: (<'a')(0 1)} etx 'abc'
'domain error' -: 'a'   (0 1)} etx 2 3 4
'domain error' -: (<12) (0 1)} etx 2 3 4
'domain error' -: 1     (0 1)} etx 2;3 4
'domain error' -: 'ab'  (0 1)} etx 2;3 4

'index error'  -: 7 ( 3)} etx 2 3 4
'index error'  -: 7 (_4)} etx 2 3 4
'index error'  -: 7   3:} etx 2 3 4
'index error'  -: 7  _4:} etx 2 3 4

'length error' -: 2 3      i.@#@]} etx i.3
'length error' -: (i.2 3)  i.@$@]} etx i.4 3 2

'rank error'   -: (i.2 3 4) i.@#@]} etx i.3 4
'rank error'   -: (i.2 3 4) 0     } etx i.3 4


NB. } in place ----------------------------------------------------------

sp     =: 7!:2
b32    =: 100>#3!:1 i.8  NB. 1 if 32-bit; 0 if 64-bit

foo =. 3 : 0
 pqr=.xyz =. i.10000
 z=.''
 z=.z,sp 'xyz=. _123 (4)}xyz'   NB. create copy
 z=.z,sp 'xyz=. _456 (5)}xyz'   NB. in place
 z=.z,sp 'xyz=. _789 (6})xyz'   NB. in place
 z=.z,sp 'qqq=. _123 (7)}xyz'   NB. create copy
 z=.z,sp 'xyz=. 8 9 >:@[`[`]} xyz'  NB. in place
 z=.z,sp 'xyz=. xyz >:@]`]`[} 11 12'  NB. in place
 n =. 14 15
 z=.z,sp 'xyz=. n >:@(17 ,~ [)`(16 ,~ [)`]} xyz'  NB. in place but only the amend and ]
 assert. n -: 14 15
 z=.z,sp 'xyz=. 8 9 >:@[`[`(,~)} xyz'  NB. in place only if 9!:53]1
 z=.z,sp 'xyz=. xyz >:@]`]`,} 11 12'  NB. in place only if 9!:53]1
 assert. xyz -: 0 1 2 3 _123 _456 _789 7 9 10 10 12 13 13 15 16 18 17, (18}.(i.10000)) , 8 9 11 12
 assert. pqr -: i. 10000
 z
)

goo =. 3 : 0
 pqr=.xyz =. i.10000
 z=.''
 z=.z,sp 'xyz=. xyz 4}~ _123'   NB. create copy
 z=.z,sp 'xyz=. xyz 5}~ _456'   NB. in place
 z=.z,sp 'xyz=. xyz 6}~ _789'   NB. in place
 z=.z,sp 'qqq=. xyz 7}~ _123'   NB. create copy
 assert. xyz -: 0 1 2 3 _123 _456 _789,7}.i.10000
 assert. pqr -: i. 10000
 z
)

hoo =. 3 : 0
 pqr=.xyz =: i.10000
 z=:''
 z=:z,sp 'xyz=: _123 (4)}xyz'   NB. create copy
 z=:z,sp 'xyz=: _456 (5)}xyz'   NB. in place
 z=:z,sp 'xyz=: _789 (6})xyz'   NB. in place
 z=:z,sp 'qqq=: _123 (7)}xyz'   NB. create copy
 z=:z,sp 'xyz=: 8 9 >:@[`[`]} xyz'  NB. in place
 z=:z,sp 'xyz=: xyz >:@]`]`[} 11 12'  NB. in place
 n =: 14 15
 z=:z,sp 'xyz=: n >:@(17 ,~ [)`(16 ,~ [)`]} xyz'  NB. in place but only the amend
 assert. n -: 14 15
 z=:z,sp 'xyz=: 8 9 >:@[`[`(,~)} xyz'  NB. in place only if 9!:53]1
 z=:z,sp 'xyz=: xyz >:@]`]`,} 11 12'  NB. in place only if 9!:53]1
 z=:z,sp 'xyz=: 11 13 (>:@{)`[} xyz'  NB. in place, 2 gerunds
 assert. xyz -: 0 1 2 3 _123 _456 _789 7 9 10 10 13 13 14 15 16 18 17, (18}.(i.10000)) , 8 9 11 12
 assert. pqr -: i. 10000
 z
)

9!:53(0)
IGNOREIFFVI ((50000*2-b32) < 0 3 7 8 {t) , (7000*2-b32)> 1 2 4 5 6 { t=:foo 1
IGNOREIFFVI ((50000*2-b32) < 0 3{t) , (5000*2-b32)> 1 2 { t=:goo 1
IGNOREIFFVI ((50000*2-b32) < 0 3 7 8 {t) , (7000*2-b32)> 1 2 4 5 6 9 { t=:hoo 1
9!:53(1)
IGNOREIFFVI ((50000*2-b32) < 0 3 {t) , (7000*2-b32)> 1 2 4 5 6 7 8 { t=:foo 1
IGNOREIFFVI ((50000*2-b32) < 0 3{t) , (5000*2-b32)> 1 2 { t=:goo 1
IGNOREIFFVI ((50000*2-b32) < 0 3 {t) , (7000*2-b32)> 1 2 4 5 6 7 8 9 { t=:hoo 1
9!:53(2)   NB. Default

abc =. save =. i.10000
IGNOREIFFVI (68500*2-b32) > t1=.sp 'abc=. _123 (0)  }abc'  NB. create copy
IGNOREIFFVI ( 2900*2-b32) > t2=.sp 'abc=. _123 (1 2)}abc'  NB. in place
IGNOREIFFVI ( 2900*2-b32) > t2=.sp 'abc=. _123 (3 4})abc'  NB. in place

abc =. save =. i.10000
IGNOREIFFVI (68500*2-b32) > t1=.sp 'abc=. abc   0}~ _123'  NB. create copy
IGNOREIFFVI ( 2900*2-b32) > t2=.sp 'abc=. abc 1 2}~ _123'  NB. in place
IGNOREIFFVI ( 2900*2-b32) > t2=.sp 'abc=. abc 3 4}~ _123'  NB. in place

-. abc -: save
abc -: (5#_123),5}.i.10000
*./ 0 <: save
save -: i.10000

4!:55;:'ab abc x'

abc =: ?20 50 100$1e6
x   =: ?1e6
IGNOREIFFVI 10000 > sp 'abc=: x (<10;20;30)}abc'
x -: (<10;20;30){abc

y=: ?1e6
ab=: y (<10;20;30)}abc
x -: (<10;20;30){abc
y -: (<10;20;30){ab
*./, 1 (<10;20;30)} ab = abc

NB. Verify no local-to-global aliasing
1 [ 4!:55 <'a'
f10 =: 3 : 'a =. (0) 1} a'
f11 =: 3 : 'a =: (0) 1} a'
f12 =: 3 : ('a =. 2 3';'a =: (0) 1} a')
f13 =: 3 : ('a =. 2 3';'a =. (0) 1} a')
a =: i. 4
0 0 2 3 -: f10''
0 1 2 3 -: a
0 0 2 3 -: f11''
0 0 2 3 -: a
'domain error' -: f12 etx ''
2 0 -: f13''

NB. Boxed in place

a =: <"0 i. 100 1000
IGNOREIFFVI 500000 < 7!:2 '(<1) 0} a'
IGNOREIFFVI 30000 > 7!:2 'a =: (<1) 0} a'
b =: i. 100
IGNOREIFFVI 500000 < 7!:2 '(<b) 0} a'
IGNOREIFFVI 30000 > 7!:2 'a =: (<b) 0} a'
IGNOREIFFVI 500000 < 7!:2 '(<a) 0} a'   NB. Avoid loop
IGNOREIFFVI 500000 < 7!:2 '(<<<<a) 0} a'   NB. Avoid loop

size0 =: 7!:0''
a =: <"0 i. 100 1000
b =: <i. 500
size1 =: 7!:0''
IGNOREIFFVI 200000 > 7!:2 'a =: b (i. 10)} a'   NB. Write to many cells
IGNOREIFFVI (size1 - 10 * 1000 * IF64{4 8) > 7!:0''  NB. Verify values freed
4!:55 ;:'a b'
IGNOREIFFVI (size0 + 1000) > 7!:0''

size0 =: 7!:0''
a =: <"0 i. 100 1000
b =: <i. 500
size1 =: 7!:0''
IGNOREIFFVI 200000 > 7!:2 'a =: b (10$i. 2)} a'   NB. Rewrite cells
a -: b , b , 2 }. <"0 i. 100 1000
IGNOREIFFVI (size1 - 2 * 1000 * IF64{4 8) > 7!:0''  NB. Verify values freed
4!:55 ;:'a b'
IGNOREIFFVI 1  NB. execute a name to allow garbage collection to run
IGNOREIFFVI (size0 + 1000) > 7!:0''

NB. Verify memcpy routine by replacing one cell of an array
test =: 3 : 0"0
a =. (5,y) $ 'a'
((y#'*') (] , [ , ]) (2,y) $ 'a') -: (y#'*') 2} a 
)
test i. 517

NB. atom required now(,: 0 1 2 5) -: (,5) (,<0 3)} i. 1 4
'rank error' -: (,5) (<0 3)} etx i. 1 4
(0 1 2 0 4,:5 6 7 1 9) -:  (i. 2)    (<a:;3)} i. 2 5
'length error' -:  (i. 2)    (<a:;,3)} etx  i. 2 5
'index error' -: (i. 2 2)    (<<i. 2 2)} etx i. 2 11

NB. Noun & other components of AR
x =: 5
'domain error' -: ex '((5!:1<''x'')`[`])}'
'domain error' -: ex '([`(5!:1<''x'')`])}'
'domain error' -: ex '([`]`(5!:1<''x''))}'
x =: /
'domain error' -: ex '((5!:1<''x'')`[`])}'
'domain error' -: ex '([`(5!:1<''x'')`])}'
'domain error' -: ex '([`]`(5!:1<''x''))}'
x =: "
'domain error' -: ex '((5!:1<''x'')`[`])}'
'domain error' -: ex '([`(5!:1<''x'')`])}'
'domain error' -: ex '([`]`(5!:1<''x''))}'

NB. Verify values out of execution are still inplaceable
a =: i. 1e6
IGNOREIFFVI 10000 > 7!:2 'a =: (#a) 0} a'

NB. x -@{`[`]} y

test1 =: -@{`[`]} -: -"-@{`[`]}
a =: 0. + b =: i. 4 5
a: test1 a
a: test1 b
2 test1 a
3 test1 b
(<2;3 4) test1 a
(<2;3 4) test1 b
(<2 1;3) test1 a
(<2 1;3) test1 b
(<a:;3) test1 a
(<a:;3) test1 b
(<a:;3 1 4) test1 b
(<a:;<<3) test1 a
(<a:;<<3) test1 b
a =: 0. + b =: i. 4 5 6
(<a:;3) test1 a
(<a:;3) test1 b
 
a =: 0. + i. 100 100
10000 > 7!:2 'a =: 2 -@{`[`]} a'  NB. inplace

NB. x m}"n y with IRS

NB. y is (axis size profile);(surplus a frame);(common frame);(rank of ind);(# repeated axes of a);(cell rank);(type of data)
testm =: {{
'axisprof asf cf indr areps rcell dtype' =. y
ranshape =. axisprof I. 0 ?@$~ ]  NB. y is rank, result is shape using axis-length profile
aframe =. ranshape (asf>.0)   NB. neg means surplus is in w
wframe =. ranshape (0>.-asf)
commonframe =. ranshape cf
cellshape =. ranshape rcell  NB. shape of inner cell
indshape =. ranshape indr  NB. selectors lengths
wcellframe =. 1 >. ranshape ($indshape)   NB. frame of minor cells in w
ind =. indshape ?@$&.> wcellframe   NB. selector values for each leading axis of w-cell
selframe =. ((#ind) {. # S:0 ind)   NB. axes preserved by the selection
NB. Use simplest suitable form of ind, which is now a list of boxes
if. 1=#ind do.  NB. just one axis.
  ind =. ,>ind
  if. 1 = #ind do. if. (?2) do. ind =. {. ind [ selframe =. $0 end. end.   NB. Singleton.  Can be atom or list; if atom, lose the axis from the selection
  ind =. <@<^:(?2) ind   NB. Value can be bare or double-boxed
elseif. *./ (,1)&-:@$@> ind do.   NB. all singletons, make into list of successive axes
  selframe =. $0  NB. these axes are lost to the selection result  
  ind =. < ; ind   NB. Run the axes together in one box
else. ind =. <ind   NB. normal case, box the selectors
end.
c =: ind
ashapein =. areps }. selframe , cellshape  NB. a may be any length suffix of selection
ashape =. commonframe , aframe , ashapein
wshape =. commonframe , wframe , wcellframe , cellshape
d =: r =. (#ashapein) , (wcellframe +&# cellshape)
select. dtype
case. 1 do. a =: ashape ?@$ 2 [ b =: wshape ?@$ 2
case. 2 do. a =: ashape (] {~ (?@$ #)) a. [ b =: wshape (] {~ (?@$ #)) a.
case. 4 do. a =: ashape ?@$ 1e6 [ b =: wshape ?@$ 1e6
case. 8 do. a =: ashape ?@$ 0 [ b =: wshape ?@$ 0
case. 16 do. a =: 2 j. ashape ?@$ 0 [ b =: 3 j. wshape ?@$ 0
case. 64 do. a =: ashape ?@$ 1000000x [ b =: wshape ?@$ 1000000x
case. 128 do. a =: 1r7 * ashape ?@$ 1000000x [ b =: 1r7 * wshape ?@$ 1000000x
case. do. 13!:8 ] 2
end.
assert. a (ind}"r -: ind}"]"r) b
1
}}

NB. y is # tests;precision
testn =: {{
'ntests prec' =. y
for. i. ntests do.
  testm 0.02 0.1 0.5 0.9;(2-?4);(?4);(?4);(?2);(?2);prec
end.
}}"1
testn 10000 ,. 1 2 4 8 16 64 128  NB. Reduce count after burn-in

(,:~"2 i. 3 4 5) -:  (i. 3 4 5) (<a:,&<a:)}"2 i. 3 2 4 5  NB. verify last axis can't be removed

23334 -: +/ , 0 (<1;0;<<_1) } i. 2 2 65

NB. shaped boxed selectors
(2 6 2 3$0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 90 91 92 93 94 95 96 97 98 99 100 101 96 97 98 99 100 101 90 91 92 93 94 95 66 67 68 69 70 71) -: (90+i. 2 2 3) (<1;1 3,:4 2)} i. 2 6 2 3
(2 6 2 3$0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 90 91 92 48 49 50 93 94 95 54 55 56 93 94 95 60 61 62 90 91 92 66 67 68 69 70 71) -: (90+i. 2 3) (<1;(1 3,:4 2);1)} i. 2 6 2 3
(2 6 2 3$0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 90 48 49 50 51 52 91 54 55 56 57 58 91 60 61 62 63 64 90 66 67 68 69 70 71) -: (90+i. 2) (<1;(1 3,:4 2);1;2)} i. 2 6 2 3
(2 7 1 3$0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 900 901 902 900 901 902 903 904 905 903 904 905 900 901 902 36 37 38 903 904 905) -: (900+i. 2 1 3) (<1;3 2 $1 3  0 6 4 2)} i. 2 7 1 3
(7 2 3$0 1 2 900 901 902 6 7 8 900 901 902 12 13 14 903 904 905 18 19 20 903 904 905 24 25 26 900 901 902 30 31 32 33 34 35 36 37 38 903 904 905) -: (900+i. 2 3) (<(3 2 $1 3  0 6 4 2);1)} i. 7 2 3
(7 2 3$0 1 2 3 4 900 6 7 8 9 10 900 12 13 14 15 16 901 18 19 20 21 22 901 24 25 26 27 28 900 30 31 32 33 34 35 36 37 38 39 40 901) -: (900+i. 2) (<(3 2 $1 3  0 6 4 2);1;2)} i. 7 2 3

(2 6 2 3$0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900  900 900 900 900 900 900 900 66 67 68 69 70 71) -: (900) (<1;1 3,:4 2)} i. 2 6 2 3
(2 6 2 3$0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 900 900 900 48 49 50 900 900 900 54 55 56 900 900 900 60 61 62 900 900 900 66 67 68 69 70 71) -: (900) (<1;(1 3,:4 2);1)} i. 2 6 2 3
(2 6 2 3$0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 900 48 49 50 51 52 900 54 55 56 57 58 900 60 61 62 63 64 900 66 67 68 69 70 71) -: (900) (<1;(1 3,:4 2);1;2)} i. 2 6 2 3
(2 7 1 3$0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 900 900 900 900 900 900 900 900 900 900 900 900 900 900 900 36 37 38 900 900 900) -: (900) (<1;3 2 $1 3  0 6 4 2)} i. 2 7 1 3
(7 2 3$0 1 2 900 900 900 6 7 8 900 900 900 12 13 14 900 900 900 18 19 20 900 900 900 24 25 26 900 900 900 30 31 32 33 34 35 36 37 38 900 900 900) -: (900) (<(3 2 $1 3  0 6 4 2);1)} i. 7 2 3
(7 2 3$0 1 2 3 4 900 6 7 8 9 10 900 12 13 14 15 16 900 18 19 20 21 22 900 24 25 26 27 28 900 30 31 32 33 34 35 36 37 38 39 40 900) -: (900) (<(3 2 $1 3  0 6 4 2);1;2)} i. 7 2 3

(,1) -: $ (<i. 1 0) { 5
4 -: (,4) (<i. 1 0)} 5
4 -: (,4) (i. 1 0)} 5


'domain error' -: (<3) [`]`]} etx [`1:`]  NB. execute exception if gerund returns gerund

1:    99 (<1 2 3 ,:3 2 1)} i. 4 4 6   NB. more atoms than axes OK
'domain error' -: 99 (0;0 0)} etx i. 2 3

(0 2 3 ,: 0 3 2) -: (,: 1 2&(] ] (|.@:{)`[`]})) memu 0  2 3  NB. } not inplace when v2 returns old block
(1 2&(15!:19@((|.@:{)`[`]})) = 15!:19@]) memu 0  2 3  NB. } inplace
(7!:2 '1 [`0:`(+:@])} i. 100 100') < 10000 + 7!:2 '1 [`0:`]} i. 100 100'  NB. inplace when v2 returns new block

randfini''

epilog''

