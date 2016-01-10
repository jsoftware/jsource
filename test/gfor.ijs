NB. for. ----------------------------------------------------------------

f0=: 3 : 0
 s=. 0
 for.
  i. y
 do.
  s=.>:s
 end.
)

(f0 = ])"0 n=:?5 10$100

f1=: 3 : 0
 s=.0
 for_j.
  i.y
 do.
  s=.j+s
 end.
)

f1a=: 3 : 0
 s=.0
 for_j.
  i.y
 do.
  s=.j_index+s
 end.
)

(f1  = 2&!)"0 n=:?5 10$100
(f1a = 2&!)"0 n=:?5 10$100

f2=: 3 : 0
 s=.0
 for_j.
  i.y
 do.
  if. 2|j do. continue. end.
  s=.j+s
 end.
)

f2a=: 3 : 0
 s=.0
 for_j.
  i.y
 do.
  if. 2|j_index do. continue. end.
  s=.j_index+s
 end.
)

(f2  = 2&!@>.&.-:)"0 n=:?4 5$100
(f2a = 2&!@>.&.-:)"0 n=:?4 5$100

f3=: 3 : 0
 s=.0
 for_j.
  i.2e3
 do.
  if. s>:y do. j break. end.
  s=.j+s
 end.
)

1 0 2&-:@/:@(, 2&!@(_1 0&+)@f3)"0 n=:>:?4 5$1e6

f3a=: 3 : 0
 s=.0
 for_j.
  i.2e3
 do.
  if. s>:y do. j return. end.
  s=.j+s
 end.
)

1 0 2&-:@/:@(, 2&!@(_1 0&+)@f3a)"0 n=:>:?4 5$1e6

f4=: 3 : 0
 t=.4!:0 ;:'xyz xyz_index'
 for_xyz.
  1
 do.
  t=.t,4!:0 ;:'xyz xyz_index'
 end.
 t=.t,4!:0 ;:'xyz xyz_index'
)

_1 _1 0 0 _1 _1 -: f4 0

f5=: 3 : 0
 'm n'=. $y
 z=. (n,m)$_1
 for_i. i.m do.
  for_j. i.n do.
   z=. ((<i,j){y) (<j,i)}z
  end.
 end.
)

(|: -: f5) x=:? 7 11$1000
(|: -: f5) x=:?13  9$1000
(|: -: f5) x=:? 8  8$1000

f6=: 3 : 0
 s=. y
 z=. s$_1
 for_i. i.0{s do.
  for_j. i.1{s do.
   for_k. i.2{s do.
    for_e. i.3{s do.
     z=. (s#.i,j,k,e) (<i,j,k,e)}z
    end.
   end.
  end.
 end.
)

(i. -: f6) 3 3 3 3
(i. -: f6) 2 3 4 5

f7=: 4 : 0
 s=. 1
 for. i.y,0 do.
  s=. x*s
 end.
)

2 (^ -: f7) 13
3 (^ -: f7) 7

f8=: 4 : 0
 s=.0
 for_i.
  select. x
   case. 1 do. y
   case. 2 do. i.y
  end.
 do.
  s=.i+s
 end.
)

(+/x)           -: 1 f8 x=:?10$100
(2!n)           -: 2 f8 n=:?1000
'control error' -: 3 f8 etx 10

f9=: 3 : 0
 z=. ''
 for_i. y do.
  z=. z,>i
 end.
)

(; -: f9) (?1000$#x){x=: ;:'Cogito, ergo sum. 4 5 6 John Smith a b c d' 

f10=: 4 : 0
 best=. _999
 for_var. y do.
  if. x=var do.
   best=. var
  end.
 end.
 best
)

x -: f10"0 1~x=: ?100$1e6
_999 = 1e7 f10 x

f11=: 4 : 0
 best=. _999
 for_var. y do.
  if. x=var do.
   best=. var_index
  end.
 end.
 best
)

(i:~x) -: f11"0 1~x=: ?100$90
_999 = 1e7 f11 x

'ill-formed name' -: ex '3 : ''for_123. 4 5 6 do. 7 end.'' '
'ill-formed name' -: ex '3 : ''for_1ab. 4 5 6 do. 7 end.'' '
'ill-formed name' -: ex '3 : ''for__ab. 4 5 6 do. 7 end.'' '
'ill-formed name' -: ex '3 : ''for_.    4 5 6 do. 7 end.'' '

g0=: 3 : 'for. do. 7 end.'

'control error'  -: g0 etx 0

g1=: 3 : 0
 for.
  if. y do. 1 2 3 end.
 do.
  9
 end.
)

9 -: g1 1

'control error' -: g1 etx 0

'control error' -: ex '3 : ''for. 4 5 6 7 end.'' '
'control error' -: ex '3 : ''for. 4 5 6 7 do. '' '
'control error' -: ex '3 : ''for. 4 5 6 7 '' '
'control error' -: ex '3 : ''for. 4 5 6 7 for. 1 2 3 do. end.'' '


NB. for. ----------------------------------------------------------------

comp=: 4 : 0
 k=. (,.&.>-) i.#c=. 1,~y$0
 z=. ((x>:&*y),*x)$y 
 for. }. i.x do. z=. ; ((1+{:$z){.&.>k) +"1&.> (-c=.+/\.c){.&.><0,.z end.
)

f=: 4 : 0  NB. assertions on m comp n
 'm n'=. x
 c=. y
 assert. ($c) -: ((m>:&*n)*+/+/\.^:(m-1) 1,~n$0),m
 assert. ((#c)$n) -: +/"1 c
 assert. *./ 0 <: ,c
 assert. (i.#c) -: /:c
 1
)

(i.9) (, f comp)"0 (8)
0 (, f comp)"0 i.8

queens=: 3 : 0
 z=.i.n,*n=.y
 for. }.z do.
  b=. -. (i.n) e."1 ,. z +"1 _ ((-i.){:$z) */ _1 0 1
  z=. ((+/"1 b)#z),.(,b)#(*/$b)$i.n
 end.
)

f=: 4 : 0  NB. assertions on queens n
 n=. x
 q=. y
 assert. n={:$q
 assert. 2=#$q
 assert. (i.n) e."1 q
 assert. (/:q) -: i.#q
 x=. (i.n),."1 q
 y=. ((,.]) , (,.-)) 0 -.~ i:n
 assert. -. x (,/"3@:(+"1"1 2) e."2 [) y
 1
)

(f queens)"0 i.9
 

4!:55 ;:'comp f f0 f1 f1a f2 f2a f3 f3a f4 f5 f6 f7 f8 f9 f10 f11'
4!:55 ;:'g0 g1 n queens x '


