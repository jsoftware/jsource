NB. x ?@$ y -------------------------------------------------------------

NRNG=: 5  NB. number of RNGs

NB. Ewart Shaw, Hypergeometric Functions and CDFs in J,
NB.  Vector 18.4, 2002 4.

NB. erf,            A&S table 7.1
NB. normal cdf,     A&S table 26.1
NB. chi-square cdf, A&S table 26.8

erf      =: 1 H. 1.5@*: * 2p_0.5&* % ^@:*:  NB. A&S 7.1.21
n01cdf   =: -: @ >: @ erf @ %&(%:2)         

gamma    =: ! & <:
ig0      =: 4 : '(1 H. (1+x) % x&((* ^) * (^ -)~)) y'
incgam   =: ig0 % gamma@[  NB. incomplete gamma
chisqcdf =: incgam&-:

NB. Stirling Numbers of the second kind.
NB. see Hui & Iverson, Representations of Recursion, APL95
NB. {n,r} is r{s2 n

s2=: 1: ` (i.@>: ((0,]) + [ * 0,~]) $:@<:) @. *

sp=: 7!:2


g0=: 2 3$(? @$)`(? @:$)`([: ?  $)`(? @#)`(? @:#)`([: ?  #)
g1=: 2 3$(?.@$)`(?.@:$)`([: ?. $)`(?.@#)`(?.@:#)`([: ?. #)
g =: g0,:g1

NB. test that special code is used by looking at space used

test=: 4 : 0
     j=. 9!:42 ''
     9!:43 x
 h=: ,g
 t=: 3 : '7!:5 <''y''' ?1e5$y
 for_i. i.#h do.
  f=: h@.i
  s=: sp '1e5 f y'
  assert. (IF64{0.01 0.015) > t%~|t-s 
 end.
     9!:43 j
 1
)

j=: 9!:42 ''
0 test :: 1: 2  NB. prime the pump
9!:43 j

(i.NRNG) test"0/ 2
(i.NRNG) test"0/ 1e9

test1=: 4 : 0
 h=: ,g
 for_i. i.#h do.
  f=: h@.i
  assert. (,x) -: $ x f y 
 end.
 1
)

1e4 test1 2
1e4 test1 1e9

test2=: 4 : 0
 h=: ,g
 t=: 3!:0 ?yy=: y
 for_i. i.#h do.
  f=: h@.i
  c=: x f etx y
  assert. t = 3!:0 c
  assert. (c-:<.c) *. (0<:c) *. c<y
 end.
 1
)

1483 test2"0 >:i.2 10
1483 test2"0] 2^i.10
1483 test2 1e9

test3=: 4 : 0  NB. tests on ?.
     j=. 9!:42 ''
     9!:43 ]1
 9!:1 ]7^5
 t=: x h@.0 y
 for_i. {: i. 2,-:#h do.
  f=: h@.i
  assert. t -: x f y 
 end.
     9!:43 j
 1
)

1229 test3"0 >:i.2 10
1229 test3"0 ] 2^i.10
1229 test3"0 ]10^i.10

test4=: 4 : 0
 h=: ,0{"2 g
 for_i. i.#h do. 
  f=: h@.i
  assert. x -: $x f y 
 end.
 1
)

1e8 100 0 2 test4"1 0 >: i.2 10
1e8 100 0 2 test4"1 0 ]2^i.10
13 97 7     test4 1000

test5=: 4 : 0
 h=: ,1{"2 g
 for_i. i.#h do. 
  f=: h@.i
  assert. (,+/x) -: $x f y 
 end.
 1
)

100 0 101 123 test5"1 0 >: i.2 10
100 0 101 123 test5"1 0 ] 2^i.10
97 13         test5 1e6

test6=: 4 : 0
 h=: ,0{"2 g
 for_i. i.#h do.
  f=: h@.i
  assert. 'limit error' -: x f etx y 
 end.
 1
)

j=: 9!:42 ''
(10 100 1000, <.imax%1234) test6 100
9!:43 j

test7=: 4 : 0
 h=: ,1{"2 g
 for_i. i.#h do. 
  f=: h@.i
  assert. 'limit error' -: x f etx y 
 end.
 1
)

j=: 9!:42 ''
(1 0 1 * imax - 1e6 0 2e6) test7 100
9!:43 j

test8=: 4 : 0
 h=: ,g
 for_i. i.#h do. 
  f=: h@.i
  assert. 'domain error' -: x f etx y 
 end.
 1
)

j=: 9!:42 ''
'a' test8 12
a:  test8 12
_35 test8 12
3.5 test8 12
3j5 test8 12
10  test8 'a'
10  test8 a:
10  test8 _35
10  test8 3.5
10  test8 3j5
9!:43 j

(2^_53) = +./ 5e3 ?@$ 0
1 = 5e3 ([: +./ ?@$)"0 ] 10^1 2 6 8 9
1 = 5e3 ([: +./ ?@$)"0 ]  2 ^1 3 5 7 8 13 23 30
1 = 5e3 ([: +./ ?@$)"0 ]  IF64#<.10^10 11 12 13 18
1 = 5e3 ([: +./ ?@$)"0 ]  IF64#<.2 ^32 33 34 43 47 53 62

mean=: +/ % #
var =: <:@# %~ +/@:*:@:(- mean)

test9=: 4 : 0  NB. mean test
     j=. 9!:42 ''
     9!:43 x
 yy=: y
 t=: 2e4 ?@$ yy
 d=: (%:var t) %~ | (mean t) - |-:<:yy
 p=: n01cdf d
 assert. p<0.99995
     9!:43 j
 1
)

(i.NRNG) test9"0/ 0
(i.NRNG) test9"0/ 10^1 2 6 8 9
(i.NRNG) test9"0/ 2 ^1 3 5 7 8 13 23 30
(i.NRNG) test9"0/ IF64#<.10^10 11 12 13 18
(i.NRNG) test9"0/ IF64#<.2 ^32 33 34 43 47 53 62

test10=: 3 : 0
     j=. 9!:42 ''
     9!:43 y
 xx=: 1e5 ?@$ 0
 assert. (0<xx)*.xx<1
 assert. (2^_53)=+./xx
     9!:43 j
 1
)

test10"0 i.NRNG


NB. tests from Knuth II, section 3.3.2 ----------------------------------

NB. equidistribution (frequency) test

testa=: 4 : 0  
     j=. 9!:42 ''
     9!:43 x
 n=: 20011             NB. number of trials
 yy=: y
 m=: 260<.yy+260*0=yy  NB. 1+ degrees of freedom
 e=: n%m               NB. expected count
 assert. 5 <: e
 if.     0=yy do. d=: <.m*   n ?@$ yy
 elseif. m<yy do. d=: <.m*y%~n ?@$ yy
 elseif. 1   do. d=: n ?@$ yy
 end.
 a=: <: #/.~ (i.m),d   NB. actual counts
 c=: e %~ +/ *: a - e  NB. chi-square statistic
 p=: (m-1) chisqcdf c
 assert. (0.00001<p)*.p<0.99999
     9!:43 j
 1
)

0 $ 0 : 0
(i.NRNG) testa"0/ 0
(i.NRNG) testa"0/ 6 10 100
(i.NRNG) testa"0/ 2^1 2 3 4 5 6 7 8
(i.NRNG) testa"0/ 2^28 30 , IF64#32 33 34 53 62
(i.NRNG) testa"0/10^7 8 9 , IF64#16 17 18
(i.NRNG) testa"0/13^6 7 8 , IF64#16 17
2        testa"0 x: 5 55
)

NB. serial test: chi-square test on successive k-tuples

testb=: 4 : 0  
     j=. 9!:42 ''
     9!:43 x
 k=: 2
 n=: ]&.(p:^:_1)k*1e4  NB. number of trials
 yy=: y
 m=: yy^k              NB. 1+ degrees of freedom
 e=: n%k*m             NB. expected count
 assert. 5 <: e
 d=: yy #. (-k) ]\ n ?@$ yy
 a=: <: #/.~ (i.m),d   NB. actual counts
 c=: e %~ +/ *: a - e  NB. chi-square statistic
 p=: (m-1) chisqcdf c
 assert. (0.0001<p)*.p<0.9999
     9!:43 j
 1
)

0 $ 0 : 0
(i.NRNG) testb"0/ 2 6 10
2        testb"0 x: 2 22
)

NB. gap test: chi-square test on successive k-tuples

testc=: 3 : 0
)

NB. poker (partition) test: chi-square test on # distinct values in groups of k

testd=: 4 : 0
     j=. 9!:42 ''
     9!:43 x
 'k d'=: y
 n=: ]&.(p:^:_1) 2e4
 r=: i.1+k
 x1=: #@~."1 (n,k) ?@$ d
 ea=: n*(d^k)%~(s2 k)**/\1,d-i.k
 m=: 1 i.~ 5<:+/\ ea
 e=: (m>.r) +//. ea
 assert. 5 <: e               NB. expected counts
 a=: <: #/.~ (m}.r),m>.x1     NB. actual counts
 c=: +/ e %~ *: a - e         NB. chi-square statistic
 p=: (#e) chisqcdf c
 assert. (0.0001<p)*.p<0.9999
     9!:43 j
 1
)

0 $ 0 : 0
(i.NRNG) testd"0 1/ _2]\ 5 10 5 13 7 23 7 29 13 52
)

NB. coupon collector's test: chi-square test on lengths of segments

couplen=: 4 : 0   NB. Ewart Shaw, 2005-09-08
 p=. <: 2^x                        NB. number of states
 s0=. p | (i.p) +."1/&.#: 2^i.x    NB. transition table
 s=. s0 ,"0 p{.2                   NB. output when in state 0
 ijr=. 1 0 , 2^{.y                 NB. process  {.y  'by hand'
 }: {:"1 (2;s;'';ijr) ;: y
)

teste=: 4 : 0
     j=. 9!:42 ''
     9!:43 x
 d=: y
 t=: 4*d
 r=: d+i.t-d
 xx=: (d*5e4) ?@$ d
 x1=: d couplen xx
 n=: #x1               
 e=: n*(((!d)%d^r)*(d-1)&{@s2"0 r-1),1-((!d)%d^t-1)*d{s2 t-1
 assert. 5 <: e               NB. expected counts
 a=: <: #/.~ (r,t),t<.x1      NB. actual counts
 c=: +/ e %~ *: a - e         NB. chi-square statistic
 p=: (#e) chisqcdf c
 assert. (0.0001<p)*.p<0.9999
     9!:43 j
 1
)

0 $ 0 : 0
(i.NRNG) teste"0/ 4 5 6 10
2 teste 10
)

NB. permutation test: chi-square test on orderings of successive k-tuples

testf=: 4 : 0
     j=. 9!:42 ''
     9!:43 x
 k=: y
 n=: k*1e3             NB. number of trials
 m=: !k                NB. 1+ degrees of freedom
 e=: n%k*m             NB. expected count
 assert. 5 <: e
 d=: A. /:"1 (-k) ]\ n ?@$ 0
 a=: <: #/.~ (i.m),d   NB. actual counts
 c=: e %~ +/ *: a - e  NB. chi-square statistic
 p=: (m-1) chisqcdf c
 assert. (0.0001<p)*.p<0.9999
     9!:43 j
 1
)

0 $ 0 : 0
(i.NRNG) testf"0/ 3 4 5
2 testf 5x
)

4!:55 ;:'a c chisqcdf couplen d e ea erf f g g0 g1 gamma h ig0 incgam'
4!:55 ;:'j k m mean n n01cdf NRNG p r s s2 sp t'
4!:55 ;:'test test1 test10 test2 test3 test4'
4!:55 ;:'test5 test6 test7 test8 test9'
4!:55 ;:'testa testb testc testd teste testf'
4!:55 ;:'var x xx x1 y yy'


