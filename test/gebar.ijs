prolog './gebar.ijs'
NB. x E.y ---------------------------------------------------------------

randuni''

1 0 1 0 0 -: 'co' E. 'cocoa'
1 1 1 1 0 -: 'aa' E. 5$'a'
1 0 0 0 0 -: E.~ 'abcde'
0 0 0 0 0 -: 'xy' E. 'asfdd'

1 0 1 0 0 -: (u:'co') E. u:'cocoa'
1 1 1 1 0 -: (u:'aa') E. 5$u:'a'
1 0 0 0 0 -: E.~ u:'abcde'
0 0 0 0 0 -: (u:'xy') E. u:'asfdd'

1 0 1 0 0 -: (10&u:'co') E. 10&u:'cocoa'
1 1 1 1 0 -: (10&u:'aa') E. 5$10&u:'a'
1 0 0 0 0 -: E.~ 10&u:'abcde'
0 0 0 0 0 -: (10&u:'xy') E. 10&u:'asfdd'

1 0 1 0 0 -: (s:@<"0 'co') E. s:@<"0 'cocoa'
1 1 1 1 0 -: (s:@<"0 'aa') E. 5$s:@<"0 'a'
1 0 0 0 0 -: E.~ s:@<"0 'abcde'
0 0 0 0 0 -: (s:@<"0 'xy') E. s:@<"0 'asfdd'

(($j)$0)  -: (a.{~j,j) E. a.{~j=:?(?100)$256
(($j)$0)  -: (a.{~j,j) E. a.{~j=:?(?100)$256
(($j)$0)  -: (a.{~j,j) E. a.{~j=:?(?100)$256
(($j)$0)  -: (a.{~j,j) E. a.{~j=:?(?100)$256
(($j)$0)  -: (a.{~j,j) E. a.{~j=:?(?100)$256

(($j)$0)  -: (adot1{~j,j) E. adot1{~j=:?(?100)$(#adot1)
(($j)$0)  -: (adot1{~j,j) E. adot1{~j=:?(?100)$(#adot1)
(($j)$0)  -: (adot1{~j,j) E. adot1{~j=:?(?100)$(#adot1)
(($j)$0)  -: (adot1{~j,j) E. adot1{~j=:?(?100)$(#adot1)
(($j)$0)  -: (adot1{~j,j) E. adot1{~j=:?(?100)$(#adot1)

(($j)$0)  -: (adot2{~j,j) E. adot2{~j=:?(?100)$(#adot2)
(($j)$0)  -: (adot2{~j,j) E. adot2{~j=:?(?100)$(#adot2)
(($j)$0)  -: (adot2{~j,j) E. adot2{~j=:?(?100)$(#adot2)
(($j)$0)  -: (adot2{~j,j) E. adot2{~j=:?(?100)$(#adot2)
(($j)$0)  -: (adot2{~j,j) E. adot2{~j=:?(?100)$(#adot2)

((m*n)$n{.1) *./ . <: s E. (m*n)$s=:a.{~?n$256 [ m=:?20 [ n=:?20
((m*n)$n{.1) *./ . <: s E. (m*n)$s=:a.{~?n$256 [ m=:?20 [ n=:?20
((m*n)$n{.1) *./ . <: s E. (m*n)$s=:a.{~?n$256 [ m=:?20 [ n=:?20
((m*n)$n{.1) *./ . <: s E. (m*n)$s=:a.{~?n$256 [ m=:?20 [ n=:?20
((m*n)$n{.1) *./ . <: s E. (m*n)$s=:a.{~?n$256 [ m=:?20 [ n=:?20

((m*n)$n{.1) *./ . <: s E. (m*n)$s=:adot1{~?n$(#adot1) [ m=:?20 [ n=:?20
((m*n)$n{.1) *./ . <: s E. (m*n)$s=:adot1{~?n$(#adot1) [ m=:?20 [ n=:?20
((m*n)$n{.1) *./ . <: s E. (m*n)$s=:adot1{~?n$(#adot1) [ m=:?20 [ n=:?20
((m*n)$n{.1) *./ . <: s E. (m*n)$s=:adot1{~?n$(#adot1) [ m=:?20 [ n=:?20
((m*n)$n{.1) *./ . <: s E. (m*n)$s=:adot1{~?n$(#adot1) [ m=:?20 [ n=:?20

((m*n)$n{.1) *./ . <: s E. (m*n)$s=:adot2{~?n$(#adot2) [ m=:?20 [ n=:?20
((m*n)$n{.1) *./ . <: s E. (m*n)$s=:adot2{~?n$(#adot2) [ m=:?20 [ n=:?20
((m*n)$n{.1) *./ . <: s E. (m*n)$s=:adot2{~?n$(#adot2) [ m=:?20 [ n=:?20
((m*n)$n{.1) *./ . <: s E. (m*n)$s=:adot2{~?n$(#adot2) [ m=:?20 [ n=:?20
((m*n)$n{.1) *./ . <: s E. (m*n)$s=:adot2{~?n$(#adot2) [ m=:?20 [ n=:?20

NB. literal
ebar =: 4 : 0
 assert. (0 e.$x) +. 2=type x
 assert. (0 e.$y) +. 2=type y
 assert. (1>:#$x)*.1>:#$y
 m=.#x
 n=.#y
 p=.(*m)+n-m
 v=.(a.i.y),99
 td1=.(m-i.m) (a.i.,x)}(#a.)$1+m
 z=.($y)$k=.0
 while. k<p do.
  i=.0 
  while. i<m do.
   if. (i{x)=(k+i){y do. i=.>:i else. break. end. 
  end.
  z=.(i=m) k}z
  k=.k+((k+m){v){td1
 end.
 z
)

a.    (ebar -: E.) a.
a.    (ebar -: E.) 'abc'
'abc' (ebar -: E.) a.
'ABC' (ebar -: E.) a.
''    (ebar -: E.) a.
a.    (ebar -: E.) ''
''    (ebar -: E.) ''
'a'   (ebar -: E.) a.
NB. a.    (ebar -: E.) 'a'
'a'   (ebar -: E.) 'a'
'a'   (ebar -: E.) 'b'
'aaa' (ebar -: E.) 50$'aaa'

NB. Verify long strings, and matches all over

NB. x is len of x, y is len of y
NB. install some random occurrences of x into y, then verify
f =. 4 : 0"0
ystg =. y (] {~ (?@# #)) 'abcdefghij'
xstg =. x (] {~ (?@# #)) 'abcdefgh'
randpos =. 2 ?@$ y - x
ystg =. (xstg,xstg) (, randpos +/ i.x)} ystg
assert. xstg (ebar -: E.) ystg [ xstgsav =: xstg [ ystgsav =: ystg
1
)
(30 + 1000 ?@$ 10) f 5000 + 1000 ?@$ 100

2 f 64 + 2000 ?@$ 10

NB. literal2
adot3=: /:~ (#adot1){.~.(u:'ABCabc'),adot1,u: (#adot1)?65536
ebar =: 4 : 0
 assert. (0 e.$x) +. 2 131072 e.~ type x
 assert. (0 e.$y) +. 2 131072 e.~ type y
 assert. (1>:#$x)*.1>:#$y
 m=.#x
 n=.#y
 p=.(*m)+n-m
 v=.(adot3 i.y),99
 td1=.(m-i.m) (adot3 i.,x)}(#adot3)$1+m
 z=.($y)$k=.0
 while. k<p do.
  i=.0 
  while. i<m do.
   if. (i{x)=(k+i){y do. i=.>:i else. break. end. 
  end.
  z=.(i=m) k}z
  k=.k+((k+m){v){td1
 end.
 z
)

adot3    (ebar -: E.) adot3
adot3    (ebar -: E.) u:'abc'
'abc'    (ebar -: E.) adot3
'ABC'    (ebar -: E.) adot3
''       (ebar -: E.) adot3
adot3    (ebar -: E.) u:''
''       (ebar -: E.) u:''
'a'      (ebar -: E.) adot3
NB. adot3    (ebar -: E.) u:'a'
'a'      (ebar -: E.) u:'a'
'a'      (ebar -: E.) u:'b'
'aaa'    (ebar -: E.) 50$u:'aaa'

NB. literal4
adot4=: /:~ (#adot2){.~.(u:'ABCabc'),adot2,10&u: RAND32 (#adot2)?C4MAX
ebar =: 4 : 0
 assert. (0 e.$x) +. 2 131072 262144 e.~ type x
 assert. (0 e.$y) +. 2 131072 262144 e.~ type y
 assert. (1>:#$x)*.1>:#$y
 m=.#x
 n=.#y
 p=.(*m)+n-m
 v=.(adot4 i.y),99
 td1=.(m-i.m) (adot4 i.,x)}(#adot4)$1+m
 z=.($y)$k=.0
 while. k<p do.
  i=.0 
  while. i<m do.
   if. (i{x)=(k+i){y do. i=.>:i else. break. end. 
  end.
  z=.(i=m) k}z
  k=.k+((k+m){v){td1
 end.
 z
)

adot4    (ebar -: E.) adot4
adot4    (ebar -: E.) 10&u:'abc'
'abc'    (ebar -: E.) adot4
'ABC'    (ebar -: E.) adot4
''       (ebar -: E.) adot4
adot4    (ebar -: E.) 10&u:''
''       (ebar -: E.) 10&u:''
'a'      (ebar -: E.) adot4
NB. adot4    (ebar -: E.) 10&u:'a'
'a'      (ebar -: E.) 10&u:'a'
'a'      (ebar -: E.) 10&u:'b'
'aaa'    (ebar -: E.) 50$10&u:'aaa'

NB. symbol5
adot5=: /:~ (#sdot0){.~.(s:' ABC abc a b c A B C'),sdot0,s:'zz',"1 ":i.100
ebar =: 4 : 0
 assert. (0 e.$x) +. 65536 e.~ type x
 assert. (0 e.$y) +. 65536 e.~ type y
 assert. (1>:#$x)*.1>:#$y
 m=.#x
 n=.#y
 p=.(*m)+n-m
 v=.(adot5 i.y),99
 td1=.(m-i.m) (adot5 i.,x)}(#adot5)$1+m
 z=.($y)$k=.0
 while. k<p do.
  i=.0 
  while. i<m do.
   if. (i{x)=(k+i){y do. i=.>:i else. break. end. 
  end.
  z=.(i=m) k}z
  k=.k+((k+m){v){td1
 end.
 z
)

adot5         (ebar -: E.) adot5
adot5         (ebar -: E.) s:' a b c'
(s:<'abc')    (ebar -: E.) adot5
(s:<'ABC')    (ebar -: E.) adot5
(s:'')        (ebar -: E.) adot5
adot5         (ebar -: E.) s:''
(s:'')        (ebar -: E.) s:''
(s:<'abc')    (ebar -: E.) adot5
NB. adot5         (ebar -: E.) s:<'abc'
(s:<'abc')    (ebar -: E.) s:<'abc'
(s:<'abc')    (ebar -: E.) s:<'b'
(s:<'abc')    (ebar -: E.) 50$s:<'abc'


x=: a.{~ ?31$#a.
y=: x (<"0(?40$y-&#x)+/i.#x)}y=: a.{~ ?9111$#a.
(x E. y) -: x E.&(a.&i.) y

x=: adot1{~ ?31$#adot1
y=: x (<"0(?40$y-&#x)+/i.#x)}y=: adot1{~ ?9111$#adot1
(x E. y) -: x E.&(adot1&i.) y

x=: adot2{~ ?31$#adot2
y=: x (<"0(?40$y-&#x)+/i.#x)}y=: adot2{~ ?9111$#adot2
(x E. y) -: x E.&(adot2&i.) y

x=: sdot0{~ ?31$#sdot0
y=: x (<"0(?40$y-&#x)+/i.#x)}y=: sdot0{~ ?9111$#sdot0
(x E. y) -: x E.&(sdot0&i.) y

x=: ?31$1000
i=: ?40$y-&#x
y=: x (<"0 i+/i.#x)}y=: ?9111$1000
((# i.@#)x E. y) e. i

(30$0) -: (<"0 ?35$10) E. <"0 ?30$10

1 1 1 -: 2.3 E. 2.3 2.3 2.3
1 1 1 -: 2j3 E. 2j3 2j3 2j3
1 1 0 -: 1 1 E.      1 1 1 + 1e_15
0 0 0 -: 1 1 E.!.0 ] 1 1 1 + 1e_15

(5$1) -: (i.0) E. i.5
(5$1) -: (i.0) E. 'abcde'
(5$1) -: (i.0) E. u:'abcde'
(5$1) -: (i.0) E. 10&u:'abcde'
(5$1) -: ''    E. i.5
(5$1) -: ''    E. 'abcde'
(5$1) -: ''    E. u:'abcde'
(5$1) -: ''    E. 10&u:'abcde'

NB. Application that looped forever trying to use E. on integers
G =:0 1 2 2 3 4 4 5 5 6 7 7 8 9 10 10 11 11 11 12 12 12
G =: G ,. 1 2 3 4 5 6 5 7 8 7 9 10 10 11 12 11 13 14 15 16 15 14
G =: <"1 G
joinroutes =: 4 : 0&.>
l =. >: (y i. {: x)
f =. 0:^:((#y)&=) (y i. {. x)
z =. (f {. y) , x , (l }. y)
y E. z
y
)
jtree =: ([: ~.@; joinroutes)/
12 14 -: jtree G

NB. implementation detail, ensure function pointer is 4 byte aligned, ok if no crash
1 = 0 0 +/@:E. 0 0

NB. ensure intermediate buffer doesn't fail the search
olim =: 9!:20''
9!:21 (16e6)
(0 $~ 8e6 % IF64 { 4 8) -: (0 3e6 * 2 - IF64)  E. i. 8e6 % IF64 { 4 8 [ 'If this fails, you need ''9!:21 olim'' to restore memory allocation size'
9!:21 olim

4!:55 ;:'adot1 adot2 sdot0 adot3 adot4 adot5 f g ebar i j m n s t x y G jtree joinroutes olim xstgsav ystgsav '
randfini''


epilog''

