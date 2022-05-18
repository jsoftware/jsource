prolog './glapack.ijs'
NB. LAPACK --------------------------------------------------------------

NB. obsolete s48=: 9!:48 ''
NB. obsolete 9!:49 ]1

tol=: 2^_34

require '~addons/math/lapack/lapack.ijs'
mp=: +/ .*

test=: 3 : 0
 (2^_44) test y
   :
 t=. dgeev_jlapack_ A=: y
 assert. (,3) -: $t
 assert. 32 = 3!:0 t
 assert. ($&.>t) -: 3$(,~n);,n=.#A
 'L e R' =: t
 assert. *./ 1 = +/ +/"1 *: +. L
 assert. *./ 1 = +/ +/"1 *: +. R
 assert. (A mp R)      -:!.x e *"1 R
 assert. ((+|:L) mp A) -:!.x e *"0 1 +|:L
 1
)


NB. LAPACK dgeev --------------------------------------------------------

require '~addons/math/lapack/dgeev.ijs'

tol test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 1
tol test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 2
tol test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 3
tol test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 4
tol test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 5
tol test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 6
tol test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 7
tol test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 8
tol test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 9
tol test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 10
tol test 131072 %~ _5e6+?(n,n)$1e7 [ n=: 11

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

tol test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 1
tol test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 2
tol test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 3
tol test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 4
tol test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 5
tol test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 6
tol test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 7
tol test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 8
tol test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 9
tol test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 10
tol test j./ 131072 %~ _5e6+?(2,n,n)$1e7 [ n=: 11

NB. obsolete 9!:49 s48


4!:55 ;:'A e L mp n R s0 s1 s48 test tol'



epilog''

