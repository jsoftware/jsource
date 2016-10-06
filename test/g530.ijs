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
 assert. t<IF64{2000 4000
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

ia =. 1 : 'x@(i.@$@])'
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

C =. 2 : 'x & ((#i.@#)@,@y@] })'

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
(x (<"1 i)} y) -: x (<"1 i,"1 0"1 i.7)}y

i=: ?11$100
j=: ?13$50
k=: ?17$7
x=: ?11 13 17$1e6
(x (<i;j;k)}y) -: x ({i;j;k)}y

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
 assert. xyz -: 0 1 2 3 _123 _456 _789 7 9 10 10 12 13 13 15 16 18 17, (18}.(i.10000)) , 8 9 11 12
 assert. pqr -: i. 10000
 z
)

9!:53(0)
((50000*2-b32) < 0 3 7 8 {t) , (5000*2-b32)> 1 2 4 5 6 { t=:foo 1
((50000*2-b32) < 0 3{t) , (5000*2-b32)> 1 2 { t=:goo 1
((50000*2-b32) < 0 3 7 8 {t) , (5000*2-b32)> 1 2 4 5 6 { t=:hoo 1
9!:53(1)
((50000*2-b32) < 0 3 {t) , (5000*2-b32)> 1 2 4 5 6 7 8 { t=:foo 1
((50000*2-b32) < 0 3{t) , (5000*2-b32)> 1 2 { t=:goo 1
((50000*2-b32) < 0 3 {t) , (5000*2-b32)> 1 2 4 5 6 7 8 { t=:hoo 1
9!:53(2)   NB. Default

abc =. save =. i.10000
(68500*2-b32) > t1=.sp 'abc=. _123 (0)  }abc'  NB. create copy
( 2900*2-b32) > t2=.sp 'abc=. _123 (1 2)}abc'  NB. in place
( 2900*2-b32) > t2=.sp 'abc=. _123 (3 4})abc'  NB. in place

abc =. save =. i.10000
(68500*2-b32) > t1=.sp 'abc=. abc   0}~ _123'  NB. create copy
( 2900*2-b32) > t2=.sp 'abc=. abc 1 2}~ _123'  NB. in place
( 2900*2-b32) > t2=.sp 'abc=. abc 3 4}~ _123'  NB. in place

-. abc -: save
abc -: (5#_123),5}.i.10000
*./ 0 <: save
save -: i.10000

4!:55;:'ab abc x'

abc =: ?20 50 100$1e6
x   =: ?1e6
10000 > sp 'abc=: x (<10;20;30)}abc'
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


4!:55 ;:'a aa ab abc adot1 adot2 sdot0 b b32 C c c1 d d1 dd f f foo f1 '
4!:55 ;:'f10 f11 f12 f13'
4!:55 ;:'g g0 g1 g2 g3 g4 g5 g8 g9 g10 g11 goo '
4!:55 ;:'h h1 hoo i ia j k n p pqr q qqq save sp t t t0 t1 t2 test x xx xyz y yy z z1 zz '

