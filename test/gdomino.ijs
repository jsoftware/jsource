prolog './gdomino.ijs'
NB. Tests for verbs defined in vd.c: %. 128!:0 128!:1

cblas =: 9!:56 'cblas'

NB. utils
X =: +/ .*
ct =: +@|: NB. conjugate transpose
nrm =: [: %: [: +/ [: , *:@| NB. Frobenius norm
lsq  =: {{ (x (X~ |:)&x: y) %. (X~ |:) x: y }} NB. exact least squares
mnrm =: {{ (|: x: y) X x (%. (X |:))&x: y }} NB. exact minimum norm
near =: {{ NB. check if same shape and values equal to 1e_10 relative
 if. -. x -:&$ y do. 0 return. end.
 if. 0 = #, y do. 1 return. end.
 (>./ , | x -&(x:^:_1) y) <: 1e_10 * 1 >. >./ , | x:^:_1 y
}}
eps =: 2^_52
tol =: {{ (9!:56 'cblas') { 1e8 1e3 }} NB. backward-error bound
ulps =: {{ (>./ , | (x:^:_1 x) - y) % eps * >./ , | x:^:_1 x }} NB. max error of float y vs exact x, in eps of the largest entry
rnd =: {{ NB. random array of type x and shape y
 select. x
  case. 'B01'  do. y ?@$ 2
  case. 'INT'  do. _9 + y ?@$ 19
  case. 'FL'   do. 0.5 - y ?@$ 0
  case. 'CMPX' do. (0.5 - ? y $ 0) j. 0.5 - ? y $ 0
  case. do. 13!:8 (3)
 end.
}}
mk =: {{ NB. random matrix of shape y with 2-norm condition number x
 'M N'=. y
 U =. > {. 128!:0 'FL' rnd M,N
 V =. > {. 128!:0 'FL' rnd N,N
 U X (x ^ - (i. N) % <: N) * |: V
}}
NB. check if QR
NB. y is Q;R of matrix x: Q orthonormal, QR = x, R upper with real diagonal > 0
isqr =: {{
 'Q R'=. y [ N=. {: $ x
 d =. (<0 1) |: R
 *./ (1e_13 > nrm (=i.N) - (ct Q) X Q) , (1e_13 > (nrm x - Q X R) % 1 >. nrm x) , (0 *./@:= , (>/~ i.N) * R) , (0 < 9 o. d) *. 0 = 11 o. d
}}


NB. data
M  =: 3 3 $ 4 1 2 1 5 1 2 0 6  NB. square, det 96
T  =: 4 2 $ 1 2 3 4 5 6 7 9  NB. tall, full column rank
D  =: 2 3 $ 1 2 3 4 5 7  NB. wide, full row rank
B  =: 3 3 $ 1 1 0 0 1 1 1 0 1  NB. boolean, det 2
M3 =: (,: |:) M  NB. two square cells
v3 =: 1 2 3


NB. tests
shapes =: {{
 assert. 0.25 -: %. 4
 assert. _ -: %. 0
 assert. (,0.25) near %. ,4
 assert. 0.12 0.16 near %. 3 4
 assert. 'domain error' -: %. etx 0 0
 assert. (1 1$0.25) near %. 1 1$4
 assert. (%. x: M) near %. M
 assert. (=i.3) near M X %. M
 assert. 2 4 -: $ %. T
 assert. ((=i.4) lsq T) near %. T
 assert. 3 2 -: $ %. D
 assert. (%. x: D) near %. D
 assert. (=i.2) near D X %. D
 assert. (|: %. |: D) near %. D
 assert. 'domain error' -: %. etx 2 3$1 2 3 2 4 6  NB. dependent rows
 assert. 'domain error' -: 1 10 %. etx 2 2 $ 1 1 10 10
 assert. 'domain error' -: %. etx 2 2$1 2 2 4 NB. singular
 assert. 'domain error' -: %. etx 2 2$0
 assert. 'domain error' -: %. etx 4 2$1 2 2 4 3 6 4 8 NB. rank-deficient tall
 assert. 'domain error' -: %. etx 3 2$1 0 2 0 3 0 NB. zero column
 assert. 2 3 3 -: $ %. M3
 assert. ((%. M) ,: %. |:M) near %. M3
 assert. 2 2 4 -: $ %. ,:~ T
 assert. 2 3 2 -: $ %. ,:~ D
 assert. (%. D) near 1 { %. D ,:~ +: D
 assert. 0 0 -: $ %. 0 0$0
 assert. 0 3 -: $ %. 3 0$0
 assert. (,0) -: $ %. 0$0
 assert. 3 0 -: $ %. 0 3$0
 assert. (,3) -: $ v3 %. M
 assert. (v3 %.&x: M) near v3 %. M
 assert. 3 2 -: $ (i.3 2) %. M
 assert. ((i.3 2) %.&x: M) near (i.3 2) %. M
 assert. 3 2 4 -: $ (i.3 2 4) %. M
 assert. (1 {"2 (i.3 2 4) %. M) near (1 {"2 (i.3 2 4)) %. M
 assert. (,3) -: $ 5 %. M
 assert. ((3$5) %. M) near 5 %. M
 assert. (=i.3) near M %. M
 Ut =. 3 3 $ 2 1 _1 0 3 2 0 0 4 NB. upper triangular
 assert. (v3 %.&x: Ut) near v3 %. Ut
 assert. (v3 %.&x: |: Ut) near v3 %. |: Ut NB. lower triangular
 assert. ((i.3 2) %.&x: Ut) near (i.3 2) %. Ut
 assert. ((i.3 2) %.&x: |: Ut) near (i.3 2) %. |: Ut
 assert. 0.5 0.5 0.375 -: 1 2 3 %. 3 3 $ 2 0 0 0 4 0 0 0 8
 Uc =. Ut j. 3 3 $ 1 1 1 0 1 1 0 0 1
 assert. v3 near Uc X v3 %. Uc
 assert. v3 near (|: Uc) X v3 %. |: Uc
 assert. 'domain error' -: v3 %. etx 3 3 $ 1 2 3 0 0 5 0 0 6 NB. 0 on the diagonal
 assert. 'domain error' -: v3 %. etx 3 3 $ 1 0 0 2 3 0 4 5 0
 assert. 3 2 4 -: $ (i.3 2 4) %. Ut
 assert. ((i.3 8) %.&x: Ut) near ,"2 (i.3 2 4) %. Ut
 assert. ((i.3 8) %.&x: |: Ut) near ,"2 (i.3 2 4) %. |: Ut
 assert. (3 2$1j2 3 4j_1 5 6 7) near Ut X (3 2$1j2 3 4j_1 5 6 7) %. Ut
 assert. (3 2$1j2 3 4j_1 5 6 7) near Uc X (3 2$1j2 3 4j_1 5 6 7) %. Uc
 assert. (3 2$1j2 3 4j_1 5 6 7) near (|: Uc) X (3 2$1j2 3 4j_1 5 6 7) %. |: Uc
 assert. (=i.3) near Ut %. Ut
 assert. (,2) -: $ 1 2 3 4 %. T
 assert. (1 2 3 4 lsq T) near 1 2 3 4 %. T
 assert. 2 3 -: $ (i.4 3) %. T
 assert. ((i.4 3) lsq T) near (i.4 3) %. T
 assert. (,2) -: $ 7 %. T
 assert. ((4$7) lsq T) near 7 %. T
 assert. (,3) -: $ 1 2 %. D
 assert. (1 2 mnrm D) near 1 2 %. D
 assert. 1 2 near D X 1 2 %. D
 assert. 3 4 -: $ (i.2 4) %. D
 assert. ((i.2 4) mnrm D) near (i.2 4) %. D
 assert. (,3) -: $ 7 %. D
 assert. ((2$7) mnrm D) near 7 %. D
 assert. '' -: $ v3 %. 4 5 6
 assert. (32r77) near v3 %. 4 5 6
 assert. (,2) -: $ (>:i.3 2) %. 4 5 6
 assert. '' -: $ 7 %. 4 5 6
 assert. (15r11) near 7 %. 4 5 6
 assert. (,2) near (,6) %. 1 1$3
 assert. (,2) near 6 %. 1 1$3
 assert. 2 -: 6 %. 3
 assert. (0.5 X 6 8) near 6 8 %. 2
 assert. 2 near (,6) %. ,3
 assert. 3 0 -: $ (3 0$0) %. M
 assert. (,0) -: $ (0$0) %. 0 0$0
 assert. (3$0) near (0$0) %. 0 3$0
 assert. (,0) -: $ (3$0) %. 3 0$0
 assert. 'length error' -: 1 2 %. etx M
 assert. 'length error' -: (2 2$1) %. etx M
 assert. 'length error' -: 1 2 %. etx T
 assert. 'length error' -: 1 2 3 %. etx D
 assert. 'length error' -: 1 2 %. etx 4 5 6
 assert. 'domain error' -: 1 2 %. etx 2 2$1 2 2 4
 assert. 'domain error' -: 1 2 3 4 %. etx 4 2$1 2 2 4 3 6 4 8
 assert. 'domain error' -: 1 2 %. etx 2 4$1 2 3 4 2 4 6 8
 assert. 'domain error' -: 'ab' %. etx 2 2$1 2 3 4
 assert. 2 3 -: $ v3 %. M3
 assert. (v3 %. |:M) near 1 { v3 %. M3
 assert. 2 3 -: $ (>:i.2 3) %."1 2 M3
 assert. (4 5 6 %. |:M) near 1 { (>:i.2 3) %."1 2 M3
 assert. 8 -: 3!:0 %. M
 assert. 8 -: 3!:0 %. B
 assert. 16 -: 3!:0 %. M j. 0
 assert. 16 -: 3!:0 ((v3 j. 1) %. M)
 assert. 128 -: 3!:0 %. x: M
 assert. 128 -: 3!:0 (v3 %.&x: M)
 assert. 128 -: 3!:0 (1 2 %.&x: D)
 assert. (%. x: B) near %. 1 = B
 ac =. 1 2 3 4 j. 4 3 2 1
 Dc =. D j. |."1 D
 'Mc Tc' =. (j. |.)&.> M ; T
 y =. (3 {. ac) %. Mc
 assert. 1e_12 > (nrm (3 {. ac) - Mc X y) % nrm y
 assert. 1e_12 > nrm (=i.3) - Mc X %. Mc
 y =. ac %. Tc
 assert. 1e_12 > (nrm (ct Tc) X ac - Tc X y) % nrm y
 assert. 1e_12 > nrm (=i.2) - (%. Tc) X Tc
 assert. 1e_12 > (nrm y - (%. Tc) X ac) % nrm y
 y =. (2 {. ac) %. Dc
 assert. 1e_12 > (nrm (2 {. ac) - Dc X y) % nrm y
 assert. 1e_12 > (nrm y - (ct Dc) X ((Dc X y) %. Dc X ct Dc)) % nrm y
 assert. 3 2 -: $ %. Dc
 assert. 1e_12 > nrm (=i.2) - Dc X %. Dc
 assert. 1e_12 > (nrm y - (%. Dc) X 2 {. ac) % nrm y
 assert. 1e_12 > nrm (%. Dc) - ct %. ct Dc
 assert. (%. x: M) -: x: %. M
 assert. (v3 %.&x: M) -: x: v3 %. M
 S =. 4 4 $ 2 1 0 0 1 2 1 0 0 1 2 1 0 0 1 2
 assert. (1 2 3 4 %. S) near 1 2 3 4 %. $. S
 1
}}

qrshapes =: {{
 'Q R'=. 128!:0 ] 2 2 $ 3 0 4 5
 assert. Q near 2 2 $ 0.6 _0.8 0.8 0.6
 assert. R near 2 2 $ 5 4 0 3
 'Q R'=. 128!:0 ] 3 1 $ 3 4 0
 assert. Q near 3 1 $ 0.6 0.8 0
 assert. R near 1 1 $ 5
 'Q R'=. 128!:0 ] 3 4
 assert. Q near 2 1 $ 0.6 0.8
 assert. R near 1 1 $ 5
 assert. (1 1;1 1) -: $&.> 128!:0 ] 5
 assert. (3 3;3 3) -: $&.> 128!:0 M
 assert. (4 2;2 2) -: $&.> 128!:0 T
 assert. M isqr 128!:0 M
 assert. T isqr 128!:0 T
 assert. (2 2 $ 3 3;3 3) -: $&.> 128!:0 M3
 assert. (|:M) isqr 1 { 128!:0 M3
 assert. (3 0;0 0) -: $&.> 128!:0 ] 3 0$0
 assert. (0 0;0 0) -: $&.> 128!:0 ] 0 0$0
 assert. (0 1;1 1) -: $&.> 128!:0 ] 0$0
 assert. (isqr 128!:0) M j. |. M
 assert. (isqr 128!:0) T j. |. T
 assert. 'length error' -: 128!:0 etx D NB. wide
 assert. 'domain error' -: 128!:0 etx 4 2$1 2 2 4 3 6 4 8 NB. rank-deficient
 assert. 'domain error' -: 128!:0 etx 3 2$1 0 2 0 3 0 NB. zero column
 assert. 'domain error' -: 128!:0 etx x: M
 assert. 'domain error' -: 128!:0 etx ] 2 2$'abcd'
 assert. 'nonce error'  -: 128!:0 etx $. M + 0.5
 assert. (2 2 $ 0.5 _0.125 0 0.25) near 128!:1 ] 2 2 $ 2 1 0 4
 'Q R' =. 128!:0 M
 assert. (=i.3) near R X 128!:1 R
 assert. (128!:1 ] 2 2$2 1 0 4) -: 128!:1 ] 6 c. 2 2$2 1 0 4
 assert. (128!:1 ] 2 2$2 1 0 4) -: 128!:1 ] 7 c. 2 2$2 1 0 4
 assert. 2 2 2 -: $ 128!:1 ] 2 2 2 $ 2 1 0 4
 assert. 0 0 -: $ 128!:1 ] 0 0$0
 assert. (2 2 $ 1 _1r2 0 1r4) -: 128!:1 ] 2 2 $ 1 2 0 4x
 Rr=. 3 3 $ 1r2 1 1 0 2 1 0 0 4
 assert. (=i.3) -: Rr X 128!:1 Rr
 Rx=. x: (>: 7 7 ?@$ 9) * <:/~ i.7
 assert. (=i.7) -: Rx X 128!:1 Rx
 assert. (128!:1 x:^:_1 Rx) near 128!:1 Rx
 assert. 'length error' -: 128!:1 etx ] 2 3$1
 assert. 'rank error' -: 128!:1 etx ] 3 4
 1
}}

NB. With LAPACK nearly singular matrices give domain error.
lapackonly =: {{
 assert. 'domain error' -: %. etx 2 2 $ 1 1 1 , 1+1e_14
 assert. 'domain error' -: 1 2 %. etx 2 2 $ 1 1 1 , 1+1e_14
 assert. 1e11 < >./ , | %. 2 2 $ 1 1 1 , 1+1e_12      NB. ill-conditioned
 assert. 'domain error' -: %. etx % 1 + +/~ i. 12     NB. Hilbert 12
 assert. 'domain error' -: %. etx 3 3 $ 1 1 0 1 0 1 1 0 1 NB. exactly singular boolean
 K =. (=i.60) - (<:/~ i.60) > =i.60
 assert. 'domain error' -: (i.60) %. etx K
 assert. 'domain error' -: (i.60) %. etx |: K
 1
}}

vals =: {{
 for_t. ;: 'B01 INT FL CMPX' do. t =. > t
  W =. t rnd 40 40 [ a =. t rnd 40 3
  r =. a %. W
  assert. 40 3 -: $ r
  assert. (tol'') > (nrm a - W X r) % eps * (nrm W) * nrm r
  Wi=. %. W
  assert. (tol'') > (nrm (=i.40) - W X Wi) % eps * (nrm W) * nrm Wi
  U =. (40 * =i.40) + W * <:/~ i.40
  for_V. U ; |: U do. V =. > V
   r =. a %. V
   assert. (tol'') > (nrm a - V X r) % eps * (nrm V) * nrm r
  end.
  W =. t rnd 120 40 [ a =. t rnd 120 3
  r =. a %. W
  assert. 40 3 -: $ r
  assert. (tol'') > (nrm (ct W) X a - W X r) % eps * ((*: nrm W) * nrm r) + (nrm W) * nrm a
  P =. %. W
  assert. 40 120 -: $ P
  assert. (tol'') > (nrm (=i.40) - P X W) % eps * (nrm P) * nrm W
  assert. (1e3 * tol'') > (nrm r - P X a) % eps * nrm r
  W =. t rnd 40 120 [ a =. t rnd 40 3
  r =. a %. W
  assert. 120 3 -: $ r
  assert. (tol'') > (nrm a - W X r) % eps * (nrm W) * nrm r
  s =. (W X r) %. W X ct W
  assert. (1e3 * tol'') > (nrm r - (ct W) X s) % eps * nrm r
  P =. %. W
  assert. 120 40 -: $ P
  assert. (tol'') > (nrm (=i.40) - W X P) % eps * (nrm W) * nrm P
  assert. (1e3 * tol'') > (nrm r - P X a) % eps * nrm r
  W =. t rnd 120 40
  'Q R' =. 128!:0 W
  assert. (120 40 ; 40 40) -: Q ;&$ R
  assert. (tol'') > (nrm (=i.40) - (ct Q) X Q) % eps
  assert. (tol'') > (nrm W - Q X R) % eps * nrm W
  assert. 0 *./@:= , (>/~ i.40) * R
  d=. (<0 1) |: R
  assert. *./ (0 < 9 o. d) *. 0 = 11 o. d
  Ri =. 128!:1 R
  assert. 0 *./@:= , (>/~ i.40) * Ri
  assert. 1e3 > (nrm (=i.40) - R X Ri) % eps * (nrm R) * nrm Ri
 end.
 1
}}

exact =: {{
 for_t. ;:'B01 INT' do. t =. > t
  W =. t rnd 8 8 [ a=. t rnd 8 2
  if. 0 ~: -/ .* x: W do. NB. skip singular
   assert. 4 > (%. x: W) ulps %. W
   assert. 4 > ((x: a) %. x: W) ulps a %. W
  end.
 end.
 W =. 3 3 $ 4 1 2 1 5 1 2 0 6
 assert. (%. x: W) -: x: %. W
 assert. (1 2 3 %.&x: W) -: x: 1 2 3 %. W
 W =. x: _9 + ? 8 8 $ 19 [ a =. x: _9 + ? 8 3 $ 19
 assert. (=i.8) -: W X %. W
 assert. a -: W X a %. W
 W =. x: _9 + ? 5 9 $ 19 [ a=. x: _9 + ? 5 $ 19
 r =. a %. W
 assert. a -: W X r
 assert. r -: (|: W) X a %. W X |: W
 assert. (=i.5) -: W X %. W
 assert. r -: (%. W) X a
 1
}}

NB. well-conditioned data
agree =: {{
 c =. 9!:56 'cblas'
 for_t. ;: 'FL CMPX INT' do. t =. > t
  S =. t rnd 30 30 [ U =. t rnd 90 30 [ V =. t rnd 30 90 [ a =. t rnd 30 2 [ b =. t rnd 90 2
  Tr =. (30 * =i.30) + S * <:/~ i.30
  r1 =. (%. S) ; (a %. S) ; (%. U) ; (b %. U) ; (a %. V) ; (%. V) ; (a %. Tr) ; (a %. |: Tr) ; 128!:0 U
  0 (9!:56) 'cblas'
  r0 =. (%. S) ; (a %. S) ; (%. U) ; (b %. U) ; (a %. V) ; (%. V) ; (a %. Tr) ; (a %. |: Tr) ; 128!:0 U
  c (9!:56) 'cblas'
  assert. r0 -:&($&.>) r1
  assert. *./ 1e_8 > r0 (nrm@:- % 1 >. nrm@])&> r1
 end.
 1
}}

inplace =: {{
 W =. 'FL' rnd 30 30 [ C =. 'CMPX' rnd 30 30 [ T =. 'FL' rnd 60 30 [ V =. 'CMPX' rnd 30 60 [ a =. 'FL' rnd 30
 Tc =. 'CMPX' rnd 60 30 [ b=. 'FL' rnd 60 [ ac=. 'CMPX' rnd 60 [ bc=. 'CMPX' rnd 30
 W0 =. 15!:15 W [ C0=. 15!:15 C [ T0=. 15!:15 T [ V0 =. 15!:15 V [ a0 =. 15!:15 a
 Tc0 =. 15!:15 Tc [ b0 =. 15!:15 b [ ac0 =. 15!:15 ac [ bc0 =. 15!:15 bc
 assert. W ~:&(15!:19) W0
 assert. W -: W0 [ %. W
 assert. W -: W0 [ a %. W
 assert. W -: W0 [ W %. W
 assert. W -: W0 [ (] %. ]) W
 assert. C -: C0 [ %. C
 assert. C -: C0 [ bc %. C
 assert. T -: T0 [ %. T
 assert. T -: T0 [ b %. T
 assert. T -: T0 [ (b,.b) %. T
 assert. Tc -: Tc0 [ ac %. Tc
 assert. V -: V0 [ %. V
 assert. V -: V0 [ bc %. V
 assert. (a -: a0) *. (b -: b0) *. (ac -: ac0) *. bc -: bc0
 Ut=. (30 * =i.30) + W * <:/~ i.30 [ Ut0=. (30 * =i.30) + W * <:/~ i.30
 assert. Ut -: Ut0 [ a %. Ut
 assert. 1e_10 > nrm (a %. Ut+0) - a %. Ut
 assert. 1e_10 > nrm (=i.30) - (] %. ]) Ut+0
 assert. W -: > 1 { (%. ; ]) W+0
 assert. W -: > 1 { ((] %. ]) ; ]) W+0
 assert. W -: > 1 { (%.@] ; ]) W+0
 assert. (tol'') > (nrm W - W X (] %. ]) W+0) % eps * (nrm W) * nrm (] %. ]) W+0
 assert. (2 2$1) -: (%. :: ]) 0 + 2 2$1
 assert. (2 2$1) -: 1 2 (%. :: ]) 0 + 2 2$1
 Vc =. 'CMPX' rnd 30 60
 assert. 1e_10 > nrm (ac %. Tc+0) - ac %. Tc
 assert. 1e_10 > nrm (bc %. Vc+0) - bc %. Vc
 assert. 1e_10 > nrm ((] %. ]) Tc+0) - Tc %. Tc
 assert. 1e_10 > nrm ((] %. ]) Vc+0) - Vc %. Vc
 assert. 1e_10 > nrm ((] %. ]) T+0) - T %. T
 1
}}

cells =: {{
 S =. 'FL' rnd 50 6 6 [ V =. 'FL' rnd 50 6 [ T =. 'CMPX' rnd 20 9 4
 assert. 1e_12 > >./ , | (%. S) - %.@> <"2 S
 assert. 1e_12 > >./ , | (V %."1 2 S) - (<"1 V) %.&> <"2 S
 assert. 1e_12 > >./ , | (%. T) - %.@> <"2 T
 1
}}

big =: {{
 W =. 'FL' rnd 300 300 [ a =. 'FL' rnd 300 4
 y =. a %. W
 assert. (tol'') > (nrm a - W X y) % eps * (nrm W) * nrm y
 W =. 'CMPX' rnd 1000 80 [ a =. 'CMPX' rnd 1000
 y =. a %. W
 assert. (tol'') > (nrm (ct W) X a - W X y) % eps * ((*: nrm W) * nrm y) + (nrm W) * nrm a
 W =. 'FL' rnd 80 1000 [ a =. 'FL' rnd 80 2
 y =. a %. W
 assert. (tol'') > (nrm a - W X y) % eps * (nrm W) * nrm y
 'Q R' =. 128!:0 W =. 'CMPX' rnd 500 100
 assert. (tol'') > (nrm (=i.100) - (ct Q) X Q) % eps
 assert. (tol'') > (nrm W - Q X R) % eps * nrm W
 1
}}

nans =: {{
 de =. 'domain error'&-:
 for_a. (2 2$0 1 2 _) ; (2 2$_. 1 2 3) ; (3 3$1 2 3 4 __ 6 7 8 10) ; (4 2$1 2 3 _ 5 6 7 8) ; (3 2$1 _ 0 1 0 1) ; (2 3$1 2 _. 4 5 6) ; (2 2$1j1 _ 2 3) ; (2 2$1 2 3 4j_.) ; (3 1$1 _ 2) do. a =. > a
  assert. de %. etx a
  assert. de ((#a)$1) %. etx a
  if. >:/ $ a do. assert. de 128!:0 etx a end.
 end.
 assert. de %. etx _ (<1 0 0)} 2 3 3 $ 1 2 3 4 5 6 7 8 10
 assert. de %. etx 1 1 $ _.
 assert. de 128!:0 etx 1 1 $ _
 assert. de %. etx 3 3$1e300 1e300 1 1e300 1 1e300 1 2 3
 assert. de %. etx 1 _.
 assert. _ -: %. 0
 assert. 0 -: %. _
 assert. 128!:5 %. _.
 1
}}

illcond =: {{
 for_k. 1e4 1e8 1e11 do.
  W =. k mk 30 30 [ xt =. 'FL' rnd 30 2
  r =. (W X xt) %. W
  assert. (1e3 * k * eps) > (nrm r - xt) % nrm xt
  W =. k mk 90 30
  P =. %. W
  assert. 1e3 > (nrm (=i.30) - P X W) % eps * k
  P =. %. |: W
  assert. 1e3 > (nrm (=i.30) - (|: W) X P) % eps * k
  'Q R' =. 128!:0 W
  assert. 1e3 > (nrm (=i.30) - (ct Q) X Q) % eps
 end.
 assert. 'domain error' -: %. etx 1e18 mk 30 30
 assert. 'domain error' -: (i.30) %. etx 1e18 mk 30 30
 assert. 'domain error' -: (i.90) %. etx 1e18 mk 90 30
 assert. 'domain error' -: (i.30) %. etx |: 1e18 mk 90 30
 assert. 'domain error' -: %. etx 1e18 mk 90 30
 assert. 'domain error' -: %. etx |: 1e18 mk 90 30
 1
}}

0 (9!:56) 'cblas'
shapes ''
qrshapes ''
vals ''
exact ''
inplace ''
cells ''
big ''
nans ''
cblas (9!:56) 'cblas'
shapes ''
qrshapes ''
vals ''
exact ''
inplace ''
cells ''
big ''
nans ''
agree ''
lapackonly^:cblas ''
illcond^:cblas ''

epilog''
