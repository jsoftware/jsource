prolog './ge.ijs'

startnames =: nl''

NB. e.y -----------------------------------------------------------------

y =: 'abc';'dc';'a'
(e.y) -: 1 1 1 0 1 1,0 0 1 1 1 0,:1 0 0 0 0 1

x =: 1 0 0 1 0 1 (<;.1) 3#"1,.'abcdca'
(e.x) -: e.y

y =: u:&.>'abc';'dc';'a'
(e.y) -: 1 1 1 0 1 1,0 0 1 1 1 0,:1 0 0 0 0 1

x =: 1 0 0 1 0 1 (<;.1) 3#"1,.u:'abcdca'
(e.x) -: e.y

y =: 10&u:&.>'abc';'dc';'a'
(e.y) -: 1 1 1 0 1 1,0 0 1 1 1 0,:1 0 0 0 0 1

x =: 1 0 0 1 0 1 (<;.1) 3#"1,.10&u:'abcdca'
(e.x) -: e.y

y =: s:@<"0&.> 'abc';'dc';'a'
(e.y) -: 1 1 1 0 1 1,0 0 1 1 1 0,:1 0 0 0 0 1

x =: 1 0 0 1 0 1 (<;.1) 3#"1,.s:@<"0 'abcdca'
(e.x) -: e.y

(e.'') -: i.0 0
(e.u:'') -: i.0 0
(e.10&u:'') -: i.0 0
(e.s:'') -: i.0 0

NB. Similar to e.'' are cases of fills with a rank loop that alters boxing
0 1 -: $ i.@(3!:0)@> 0$a:
0 1 -: $ i.@(3!:0)&> 0$a:
0 1 -: $ i.@(3!:0)@>"(0) 0$a:
0 1 -: $ i.@(3!:0)&>"(0) 0$a:
0 1 -: $ i.@(3!:0)@>"(1) 0$a:
0 1 -: $ i.@(3!:0)&>"(1) 0$a:
0 32 -: $ i.@(3!:0)"(0) 0$a:
0 4 -: $ i.@(3!:0)"(0) 0$4

0 1 -: $ (0$a:) i.@(3!:0)@,&> (0$a:)
0 4 -: $ (0$a:) i.@(3!:0)@,&> (0$4)
0 1 -: $ (0$a:) i.@(3!:0)@[&> (0$4)
0 4 -: $ (0$a:) i.@(3!:0)@]&> (0$4)

1 1 -: $ ]@>"0 _~ ,1
1 1 -: $ ]@>"0 _ 0~ ,1
(,1) -: $ ]@>"_ 0 0~ ,1

'domain error' -: e. etx 1 2;'abc'  
'domain error' -: e. etx 1 2;<<'abc' 
'domain error' -: e. etx 'ab';<<'a' 
'domain error' -: e. etx 1 2;u:'abc'  
'domain error' -: e. etx 1 2;<<u:'abc' 
'domain error' -: e. etx 'ab';<<u:'a' 
'domain error' -: e. etx 1 2;10&u:'abc'  
'domain error' -: e. etx 1 2;<<10&u:'abc' 
'domain error' -: e. etx 'ab';<<10&u:'a' 


NB. x e.y ---------------------------------------------------------------

1 0 0 -: 'foo' e.'f'
0 0 0 -: 'foo' e.'x'
0 0 0 -: 'foo' e.4
1 0 0 -: (u:'foo') e.u:'f'
0 0 0 -: (u:'foo') e.u:'x'
0 0 0 -: (u:'foo') e.4
1 0 0 -: (10&u:'foo') e.10&u:'f'
0 0 0 -: (10&u:'foo') e.10&u:'x'
0 0 0 -: (10&u:'foo') e.4
1 0 0 -: (s:@<"0 'foo') e.s:@<"0 'f'
0 0 0 -: (s:@<"0 'foo') e.s:@<"0 'x'
0 0 0 -: (s:@<"0 'foo') e.4
1 0 0 -: 1 0 0 e.1
1 0 0 -: 2 1 4 e.2
1 0 0 -: (o.3 1 2)e.o.3

0 0     -: (i.2 3) e. 4 3$'a'
0 0     -: (i.2 3) e. 4 3$u:'a'
0 0     -: (i.2 3) e. 4 3$10&u:'a'
0 0     -: (i.2 3) e. 4 3$s:@<"0 'a'
0 0 0   -: (i.3 2 4) e. 1 2 4$<'a'
0 0 0   -: (i.3 2 4) e. 1 2 4$<u:'a'
0 0 0   -: (i.3 2 4) e. 1 2 4$<10&u:'a'
0 0 0   -: (i.3 2 4) e. 1 2 4$s:@<"0&.> <'a'
0 0 0   -: (i.3 2 4) e. 1 2 4$<"0@s: <'a'
(3 2$0) -: (3 2 4$'a') e. i.2 4
(3 2$0) -: (3 2 4$u:'a') e. i.2 4
(3 2$0) -: (3 2 4$10&u:'a') e. i.2 4

(<<2) e. 2#<<2  NB. succeeds if J doesn't crash

nan=: e.!.0&_.

0 0 0 0 -: nan 0 1 2  3
0 0 0 0 -: nan 0 1 2  3
(0 0 1 0,:0 0 0 0) e.~ nan 0 1 _. 3  NB. crc32 hash ?

test=: 3 : 0
 yy=: y
 i=: ?101$#yy
 b=: yy e. i{yy
 assert. 1=type b
 assert. 1=#$b
 assert. b -:&# yy
 assert. b >: 1 i}(#yy)$0
 1
)

test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 0 1
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 'abcde'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: u:'abcde'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 10&u:'abcde'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s:@<"0 'abcde'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: ?5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: ?5$2e9
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: o.?5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: j./?2 5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: (u:&.>) ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: (10&u:&.>) ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: s:@<"0&.> ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: <"0@s: ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: x: ?5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: %/0 1+x:?2 5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: u: ?5$65536
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 10&u: RAND32 ?5$C4MAX
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s: u: 128+a.i. ' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s: 10&u: 65536+a.i. ' miasma chthonic chronic kakistocracy dado'

0 0 -: (i.2 3) e. i.2 4
0 0 -: (i.2 3) e. 4 5$'a'
0 0 -: (i.2 3) e. 4 5$u:'a'
0 0 -: (i.2 3) e. 4 5$10&u:'a'
0 0 -: (i.2 3) e. 4 5$s:@<"0 'a'

0   -: 3 e. i.4 3

NB. Test reversed hashes, including bit and packed-bit mode
g =: 4 : 0
xx =: x
yy =: y
assert. ((yy&i.) xx) -: yy i. xx
assert. ((#yy) ~: yy i. xx) -: xx e. yy
assert. ((e.&yy) xx) -: xx e. yy
1
)
NB. Test on small-range integer
g&>/~ (-&.>   1000 ?@$~ #) ?@(# 1.5&*)&.> 1000 10000 100000,QKTEST{::1000000 10000000;100000 100000
NB. Test on hashing
g&>/~ (-&.>   1000 ?@$~ #) ?@(,&2 $ ])&.> 1000 10000 100000,QKTEST{1000000 100000

NB. Repeat with rank _1
g =: 4 : 0
xx =: x
yy =: y
assert. (yy i."i."_1 xx) -: yy i."_1 xx
assert. ((1 { $yy) ~: yy i."_1 xx) -: xx e."_1 yy
assert. (xx e."e."_1 yy) -: xx e."_1 yy
1
)
g&>/~ (-&.>   1000 ?@$~ #) ?@(3&, $ ])&.> 1000 10000 100000,QKTEST{1000000 100000
g&>/~ (-&.>   1000 ?@$~ #) ?@((3 , ,&2) $ ])&.> 1000 10000 100000,QKTEST{1000000 100000

NB. x e."r y ------------------------------------------------------------

g=: 4 : 'x e. y'

x=:?2 3 4 5$5
x (g"0 0 -: e."0 0) y=:?2 3 4 5$5
x (g"1 0 -: e."1 0) y=:?2 3 4  $5
x (g"2 0 -: e."2 0) y=:?2 3    $5 
x (g"3 0 -: e."3 0) y=:?2      $5
x (g"4 0 -: e."4 0) y=:?        5
x (g"0 1 -: e."0 1) y=:?13     $5
x (g"1 1 -: e."1 1) y=:?2 3 4 7$5
x (g"2 1 -: e."2 1) y=:?2 3   2$5
x (g"3 1 -: e."3 1) y=:?2     7$5
x (g"4 1 -: e."4 1) y=:?      9$5
x (g"1 2 -: e."1 2) y=:?2 3 4 2 5$5
x (g"2 2 -: e."2 2) y=:?2 3   1 5$5
x (g"3 2 -: e."3 2) y=:?2     4 5$5
x (g"4 2 -: e."4 2) y=:?3    17 5$5
x (g"2 3 -: e."2 3) y=:?2 3 7 4 5$5
x (g"3 3 -: e."3 3) y=:?2   7 4 5$5
x (g"4 3 -: e."4 3) y=:?11  7 4 5$5

(i.0 3 4) (g"1 -: e."1) i.0 3 7

(4!:55) ('x';'y'), (nl'') -. startnames



epilog''

