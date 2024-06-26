prolog './glapack.ijs'
NB. LAPACK --------------------------------------------------------------

require '~addons/math/lapack/lapack.ijs'
mp=: +/ .*
clean=: 1e_10&$: : (4 : 0)
if. L. y do.
  x clean each y
else.
  if. (3!:0 y) e. 16 16384 do.
    j./"1 y * x <: | y=. +.y
  else.
    y * x <: |y
  end.
end.
)
matchclean=: 0 *./ . = clean@,@:-
neareq=: (2^_16) > [ |@:% -
matchcleanf=: 0 *./ . neareq 5e_5&clean@,@:-
cleanf=: 5e_5&clean

test=: 3 : 0
 t=. dgeev_jlapack_ A=: y
 assert. (,3) -: $t
 assert. 32 = 3!:0 t
 assert. ($&.>t) -: 3$(,~n);,n=.#A
 'L e R' =: t
 assert. *./ 1 = +/ +/"1 *: +. L
 assert. *./ 1 = +/ +/"1 *: +. R
 assert. (A mp R)      matchcleanf e *"1 R
 assert. ((+|:L) mp A) matchcleanf e *"0 1 +|:L
 1
)


NB. LAPACK dgeev --------------------------------------------------------

require '~addons/math/lapack/dgeev.ijs'

test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 1
test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 2
test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 3
test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 4
test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 5
test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 6
test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 7
test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 8
test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 9
test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 10
test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 11

s0=: 7!:0 ''
s1=: 7!:0 ''
3 = # dgeev_jlapack_ 100 100 ?@$ 0
4!:55 ;:'LASTIN_jlapack_ LASTOUT_jlapack_'

s0=: 7!:0 ''
3 = # dgeev_jlapack_ 100 100 ?@$ 0
4!:55 ;:'LASTIN_jlapack_ LASTOUT_jlapack_'
s1=: 7!:0 ''
s0 -: s1

3 = # dgeev_jlapack_ A=: 50 50 ?.@$ 0
4!:55 ;:'LASTIN_jlapack_ LASTOUT_jlapack_ A'

s0=: 7!:0 ''
3 = # dgeev_jlapack_ A=: 50 50 ?.@$ 0
A -: 50 50 ?.@$ 0
4!:55 ;:'LASTIN_jlapack_ LASTOUT_jlapack_ A'
4!:55 ;:'A'
s1=: 7!:0 ''
s0 -: s1


NB. LAPACK zgeev --------------------------------------------------------

require '~addons/math/lapack/zgeev.ijs'

test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 1
test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 2
test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 3
test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 4
test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 5
test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 6
test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 7
test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 8
test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 9
test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 10
test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 11


4!:55 ;:'A e L mp n R s0 s1 test clean cleanf matchclean matchcleanf'



epilog''

