prolog './g128x.ijs'
NB. 128!:0 and 128!:1 ---------------------------------------------------

a  =. ?19 4$100
qr =. 128!:0 a
q =. >0{qr
r =. >1{qr

($q) -: $a
($r) -: 2$1{$a

i =. =i.#r
1e_13>>./|,(a+0=a)%~a-q+/ . *r
1e_14>>./|,i - (|:q) +/ . * q
*./,(|*r)<:<:/~i.#r

s =. 128!:1 r
($r) -: $s
1e_14>>./|,i-r +/ . * s

a  =. j.&?~ 9 5$100
qr =. 128!:0 a
q =. >0{qr
r =. >1{qr

($q) -: $a
($r) -: 2$1{$a

i =. =i.#r
1e_13>>./|,(a+0=a)%~a-q+/ . * r
1e_14>>./|,i - (+|:q) +/ . * q
*./,(|*r)<:<:/~i.#r

s =. 128!:1 r
($r) -: $s
1e_14>>./|,i-r +/ . * s

'domain error' -: ex '128!:88'

1e_12 > >./ ,| (%. - 128!:1) 3 3 $ _1 2 _4 0 5 _7 0 0 _7.
1e_12 > >./ ,| (%. - 128!:1) 3 3 $ _1 2 _4 0 5 _7 0 0 _7




'NaN error' -: 128!:1 etx 4 4 $ 0. 0 0 1

NB. 128!:2 --------------------------------------------------------------

(+/   x) -: '+/'   128!:2 x=: ?3 4 5$1e9
(+/"1 x) -: '+/"1' 128!:2 x

NB. 128!:10 -------------------------------------------------------------

{{
todiag =: ([`(,.~@i.@#@])`])}
lrtoa =: (((1. todiag *) +/ . * (* -.)) >/~@i.@#)
assert. (-: 1&{::@(128!:10)@:lrtoa) 3 3 $ 2 _1 _2 _2 4 _1 _2 _1 3
assert. (-: 1&{::@(128!:10)@:lrtoa) 4 4  $ 2 _1 _2 5  _2 4 _1 3  _2 _1 3 2  2 1 _2 3
NB. ((>./@:,@:-) 1&{::@(128!:10)@:lrtoa)@:>:@:i.@:(,~)"0 i. 20
t128 =. {{ lrin (>./@:,@:|@:-) out128 =: 1 {:: 128!:10 lrtoa lrin =: y }}
assert. 1e_15 > >./ t128@(1.&todiag)@:(0.01&*)@:(0 ?@$~ ,~)"0 i. 25
assert. 1e_14 > >./ t128@:(1.&todiag)@:(0.01&*)@:(0 ?@$~ ,~)"0 ] 500 + 5 ?@$ 25
NB. assert. 1e_14 > >/. ((>./@:,@:|@:-) 1&{::@(128!:10)@:lrtoa)@:(0 ?@$~ ,~)"0 ] 500 + i. 25
NB. sparse matrices
assert. 1e_15 > >./ t128@:((1. todiag (2#[) $ (0.01 * ?@$&0@])`((? *:)~)`(0. #~ *:@[)})   [: <. 0.01 * *:)"0 i. 25
assert. 1e_15 > >./ t128@:((1. todiag (2#[) $ (0.01 * ?@$&0@])`((? *:)~)`(0. #~ *:@[)})   [: <. 0.001 * *:)"0 ] 500 + 5 ?@$  25
ckmat =. {{ if. 1e_10 < err =. x (>./@:,@:|@:- 1&{::@(128!:10)) y do. 13!:8 (4) ] errx =: x [ erry =: y end. err }}
NB. assert. 1e_14 > >./ (ckmat lrtoa)@:((1. todiag (2#[) $ (0.01 * ?@$&0@])`((? *:)~)`(0. #~ *:@[)})   [: <. 0.01 * *:)"0 ] 500 + i. 50
NB. assert. 1e_14 > >./ (ckmat lrtoa)@:((1. todiag (2#[) $ (0.01 * ?@$&0@])`((? *:)~)`(0. #~ *:@[)})   [: <. 0.1 * *:)"0 ] 500 + i. 50
assert. 1e_10 > >./ (ckmat lrtoa)@:((1. todiag (2#[) $ (0.01 * ?@$&0@])`((? *:)~)`(0. #~ *:@[)})   [: <. 0.001 * *:)"0 ] 500 + 10 ?@$ 50

NB. permutation
for_dim. 2 + 20 ?@$ 40 do.
  'l r' =.  (10 * (2#dim) ?@$ 0) ((*-.) ,: *) t =. <:/~ i. dim
  l =. 1. todiag l * 0.5 > ($l) ?@$ 0
  a =. ({~ ?~@#) l +/ . * r
  'p lr' =. (1 0.01 1e_6 0) 128!:10 perma=: a
  assert. 1e_7 > >./ | , (p { a) - lrtoa lr
end.
1
}}^:IF64 1 NB. all J64 support EMU_AVX2 emulated fma

NB. LU rational
todiag =: ([`(,.~@i.@#@])`])}  NB. stuff x into diagonal of y
lrtoa =: (((1 todiag *) +/ . * (* -.)) >/~@i.@#)  NB. y is compressed Doolittle form, result is original a

t=: 3 : 0''
if. (-.IF64) +. GITHUBCI*.('ARM64'-.@-:2!:5'RUNNER_ARCH')*.'arm64'-:(9!:56'cpu') do.
  EMPTY return.
end.
c=. 9!:56'cblas'
for_i. i.15 do.
 echo 'i ',":i
 0(9!:56)'cblas'
 echo 'a1 '
 a1=. 128!:10 r=. (1000x ?@$~ ,~) i
 assert. r -: (0&{:: /:~ lrtoa@(1&{::)) a1                     NB. dev/lu rational
 b=. >./ | ,r - (0&{:: /:~ lrtoa@(1&{::)) _1&x: &.> a1  NB. dev/lu rational
 echo 'a2 ',":b
 assert. 1e_10 > b
 a2=. 128!:10 r1=. _1&x: r
 b=. >./ | ,r1 - (0&{:: /:~ lrtoa@(1&{::)) a2   NB. nocblas  double
 echo 'a3 ',":b
 assert. 1e_10 > b
 1(9!:56)'cblas'
 a3=. 128!:10 r1
 b=. >./ | ,r1 - (0&{:: /:~ lrtoa@(1&{::)) a3   NB. cblas  double
 echo 'a4 ',":b
 assert. 1e_10 > b
end.
c(9!:56)'cblas'
EMPTY
)

(-.IF64) +. (-: (0&{:: /:~ lrtoa@(1&{::))@(128!:10))@(1000x ?@$~ ,~)"0 i. 15

1: 0 : 0
sm =. ((1. todiag (2#[) $ (0.01 * ?@$&0@])`((? *:)~)`(0. #~ *:@[)})   [: <. 0.001 * *:) 1000
dm =.lrtoa@:(1.&todiag)@:(0.01&*)@:(0 ?@$~ ,~) 1000
)

NB. 128!:10 -------------------------------------------------------------
NB. lapack

t=: 3 : 0
N=. y
a=. (N,N) ?@$ 1000 1000
echo '$a ',":$a
t1=. 6!:2'c1=. 128!:10 a'
echo 'double  ',(' GFlop ',~ 0j3": (N^3)%(t1)*1e9),((N>:(9!:56'fma'){10,500)*.9!:56'cblas')#' cblas'
mk=. <:/~(i.N)
p=. 0{::c1 [ lu=. 1{::c1
u=. mk*lu [ l=. (=/~(i.N))+(-.mk)*lu
if. IF64 +. 9!:56'cblas' do.
  assert. 1e_4 > >./ | , a - p { l (+/ .*) u
end.

a=. a j. (N,N) ?@$ 1000 1000
t1=. 6!:2'c1=. 128!:10 a'
echo 'complex ',(' GFlop ',~ 0j3": 4*(N^3)%(t1)*1e9),((9!:56'cblas')#' cblas')
mk=. <:/~(i.N)
p=. 0{::c1 [ lu=. 1{::c1
u=. mk*lu [ l=. (=/~(i.N))+(-.mk)*lu
if. 9!:56'cblas' do.
  assert. 1e_4 > >./ | , a - p { l (+/ .*) u
end.
EMPTY
)

(3 : 0)''
if. GITHUBCI*.('ARM64'-.@-:2!:5'RUNNER_ARCH')*.'arm64'-:(9!:56'cpu') do.
  EMPTY return.
end.
echo 9!:14''
echo 'cpu ',(9!:56'cpu'),' cores ',": {. 8 T. ''
c=. 9!:56'cblas'
0(9!:56)'cblas'
t IF64{250 500
if. (9!:56)'fma' do. NB. otherwise too slow
  t IF64{500 1000
end.
1(9!:56)'cblas'
if. (9!:56)'cblas' do.
  t IF64{250 500
  t IF64{500 1000
end.
c(9!:56)'cblas'
EMPTY
)

4!:55 ;:'a i q qr r t todiag lrtoa lrin out128 perma s x'



epilog''

