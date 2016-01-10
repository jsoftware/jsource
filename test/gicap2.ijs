NB. x I. y --------------------------------------------------------------

le=:      0 1 -: /:@,:
lt=: -: < 0 1 -: /:@,:
ge=:      0 1 -: \:@,:
gt=: -: < 0 1 -: \:@,:

test1=: 4 : 0
 xx=: /:~ y{~(353,x) ?@$ #y
 yy=:     y{~(419,x) ?@$ #y
 t=: |. xx
 i=: xx I. yy
 j=: t  I. yy
 assert. (yy le"_1 i{xx,{:xx) +. (i=#xx) *. yy gt"_1 _ {:xx 
 assert. (yy gt"_1 (i-1){xx ) +. 0=i                   
 assert. (yy ge"_1 j{t,{:t  ) +. (j=#t ) *. yy lt"_1 _ {:t 
 assert. (yy lt"_1 (j-1){t  ) +. 0=j                   
 1
)

test2=: 4 : 0
 xx=: /:~ x{~ 353 ?@$ #x
 yy=:     y{~ 419 ?@$ #y
 t=: |. xx
 i=: xx I. yy
 j=: t  I. yy
 assert. (yy le"_1 i{xx,{:xx) +. (i=#xx)*. yy gt"_1 _ {:xx
 assert. (yy gt"_1 (i-1){xx ) +. 0=i
 assert. (yy ge"_1 j{t,{:t  ) +. (j=#xx)*. yy lt"_1 _ {:t
 assert. (yy lt"_1 (j-1){t  ) +. 0=j
 1
)

''  test1 0 1
2 3 test1 0 1
''  test1 a.
2   test1 a.
4   test1 a.
8   test1 a.
11  test1 a.
''  test1 200 ?@$ 250 
5   test1 200 ?@$ 250 
''  test1 0.1 * 200 ?@$ 250
3   test1 0.1 * 200 ?@$ 250
''  test1 j./ 2 200 ?@$ 250
3   test1 j./ 2 200 ?@$ 250
''  test1 'paraskavedekatriaphobia';":&.> 200 ?@$ 250
3   test1 'paraskavedekatriaphobia';":&.> 200 ?@$ 250

0 1 test2       200 ?@$ 250
0 1 test2~      200 ?@$ 250
0 1 test2 0.1 * 200 ?@$ 250
0 1 test2~0.1 * 200 ?@$ 250
t   test2 0.5 * t=: 200 ?@$ 250
t   test2~0.5 * t
t   test2 x: t=: 200 ?@$ 250
t   test2~x: t
t   test2 1r2 * t=: 200 ?@$ 250
t   test2~1r2 * t
a.  test2 u: 1000 ?@$ 65536
a.  test2~u: 1000 ?@$ 65536

(20*0<x) -: (20$0) I. x=:    7 13 ?@$ 2
(20*0<x) -: (20$0) I. x=: _5+7 13 ?@$ 10
(20*1<x) -: (20$1) I. x=:    7 13 ?@$ 2
(20*1<x) -: (20$1) I. x=: _5+7 13 ?@$ 10

(1+10<x) -: (imin,10,imax) I. x=: _11+ 7 13 ?@$ 22
(1+10>x) -: (imax,10,imin) I. x=: _11+ 7 13 ?@$ 22

'domain error' -: 0 0 1 I. etx 'abcd'
'domain error' -: 0 0 1 I. etx~'abcd'
'domain error' -: 0 0 1 I. etx ;:'foo upon thee'
'domain error' -: 0 0 1 I. etx~;:'foo upon thee'
'domain error' -: 1 2 3 I. etx 'abcd'
'domain error' -: 1 2 3 I. etx~'abcd'
'domain error' -: 1 2 3 I. etx ;:'foo upon thee'
'domain error' -: 1 2 3 I. etx~;:'foo upon thee'


NB. x I. y mixed type ---------------------------------------------------

p=: +/\ 1+10 ?@$ 5
q=: _10+ 3 4 ?@$ 40

test3=: 3 : 0
 i=: p I. q
 assert. i -: j=: (p+y) I. q
 assert. i -: k=:  p    I. q+y
 1
)

test3 -~0.5
test3 -~0j5
test3 x: 0
test3 -~1r5


NB. x I. y model --------------------------------------------------------

bs=: 1 : 0
:
 i=. 0,_1+#x
 while. <:/i do. 
  k=. <.-:+/i 
  b=. u y ,: k{x
  i=. (i*-.b) + b*k+1 _1
 end.
 1+{:i
)

I=: 4 : 0
 r=. 0>._1+#$x
 assert. r<:#$y              NB. rank error
 assert. (}.$x) -: (-r){.$y  NB. length error
 assert. 1 [ x,y             NB. domain error
 ord=. /:`\: @. ({. /: 0 _1{ :: 0: x)
 x ord f. bs"(_,r) y
)

test4=: 1 : 0
 x=. 0 1 1 1 2 3 7 [ y
 assert. 0    -: x u _5
 assert. 0    -: x u _4.5
 assert. (#x) -: x u 99
 assert. 4    -: x u 2
 assert. 4    -: x u 1.5
 assert. (0 0,(#x),4 4) -: x u _5 _4.5 99 2 1.5
 assert. (i.~ -: u~) x
 assert. (i.~ -: u~) x=. /:~ ~. 100   ?@$ 1e6
 assert. (i.~ -: u~) x=. /:~ ~. 100 2 ?@$ 0
 assert. 0 0 0 -: '' I 'abc'
 assert. (i.~ -: u~) i.  6 3
 assert. (i.~ -: u~) i. _6 3
 1
)

I  test4 0
I. test4 0

test5=: 3 : 0
 xx=: 233 ?@$ 1500 
 yy=: 263 ?@$ 3000
 'domain error' -: xx I. etx yy
)

test5"0 i.5 20

((i.0) I. 1 2 3) -: (0$a.) I. 1 2 3
((i.0) I. 1 2 3) -: (0$a:) I. 1 2 3
(2 3 5 7 I. i.0) -: 2 3 5 7 I. 0$a.
(2 3 5 7 I. i.0) -: 2 3 5 7 I. 0$a:


4!:55 ;:'bs ge gt I i j k le lt p q t'
4!:55 ;:'test1 test2 test3 test4 test5'
4!:55 ;:'x xx y yy'


