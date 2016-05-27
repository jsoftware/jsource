NB. x E.y ---------------------------------------------------------------

1 0 1 0 0 -: 'co' E. 'cocoa'
1 1 1 1 0 -: 'aa' E. 5$'a'
1 0 0 0 0 -: E.~ 'abcde'
0 0 0 0 0 -: 'xy' E. 'asfdd'

(($j)$0)  -: (a.{~j,j) E. a.{~j=.?(?100)$256
(($j)$0)  -: (a.{~j,j) E. a.{~j=.?(?100)$256
(($j)$0)  -: (a.{~j,j) E. a.{~j=.?(?100)$256
(($j)$0)  -: (a.{~j,j) E. a.{~j=.?(?100)$256
(($j)$0)  -: (a.{~j,j) E. a.{~j=.?(?100)$256

((m*n)$n{.1) *./ . <: s E. (m*n)$s=.a.{~?n$256 [ m=.?20 [ n=.?20
((m*n)$n{.1) *./ . <: s E. (m*n)$s=.a.{~?n$256 [ m=.?20 [ n=.?20
((m*n)$n{.1) *./ . <: s E. (m*n)$s=.a.{~?n$256 [ m=.?20 [ n=.?20
((m*n)$n{.1) *./ . <: s E. (m*n)$s=.a.{~?n$256 [ m=.?20 [ n=.?20
((m*n)$n{.1) *./ . <: s E. (m*n)$s=.a.{~?n$256 [ m=.?20 [ n=.?20

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

x=: a.{~ ?31$#a.
y=: x ((?40$y-&#x)+/i.#x)}y=: a.{~ ?9111$#a.
(x E. y) -: x E.&(a.&i.) y

x=: ?31$1000
i=: ?40$y-&#x
y=: x (i+/i.#x)}y=: ?9111$1000
((# i.@#)x E. y) e. i

(30$0) -: (<"0 ?35$10) E. <"0 ?30$10

1 1 1 -: 2.3 E. 2.3 2.3 2.3
1 1 1 -: 2j3 E. 2j3 2j3 2j3
1 1 0 -: 1 1 E.      1 1 1 + 1e_15
0 0 0 -: 1 1 E.!.0 ] 1 1 1 + 1e_15

(5$1) -: (i.0) E. i.5
(5$1) -: (i.0) E. 'abcde'
(5$1) -: ''    E. i.5
(5$1) -: ''    E. 'abcde'

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

NB. ensure intermediate buffer doesn't fail the search
olim =: 9!:20''
9!:21 (16e6)
(0 $~ 8e6 % IF64 { 4 8) -: (0 3e6 * 2 - IF64)  E. i. 8e6 % IF64 { 4 8 [ 'If this fails, you need ''9!:21 olim'' to restore memory allocation size'
9!:21 olim

4!:55 ;:'g ebar i j m n s t x y G jtree joinroutes olim '


