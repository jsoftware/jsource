NB. select. -------------------------------------------------------------

f0=: 3 : 0
 select. y 
  case. 1 do. i.1
  case. 2 do. i.2
  case. 3 do. i.3
  case. 4 do. i.4
 end.
)

(i. -: f0)"0 >:?4 5$4
(i.0 0) -: f0 0
(i.0 0) -: f0 5

f1=: 3 : 0
 t=. ''
 select. y 
  fcase. 1 do. t=.t,<i.1
  fcase. 2 do. t=.t,<i.2
  fcase. 3 do. t=.t,<i.3
  fcase. 4 do. t=.t,<i.4
 end.
)

(i.&.>1 2 3 4) -: f1 1
(i.&.>  2 3 4) -: f1 2
(i.&.>    3 4) -: f1 3
(i.&.>     ,4) -: f1 4

'' -: f1 0
'' -: f1 5

f2=: 3 : 0
 select. y 
  case. 1;2 do. i.1
  case. 3   do. i.2
  case. 4;5 do. i.3
  case. 6   do. i.4
 end.
)

(i.1) -: f2 1
(i.1) -: f2 2
(i.2) -: f2 3
(i.3) -: f2 4
(i.3) -: f2 5
(i.4) -: f2 6
(i.0 0) -: f2 0
(i.0 0) -: f2 7

f3=: 3 : 0
 select. y
  case. 'a' do. i.1
  case. 'b' do. i.2
  case.     do. i.3
 end.
)

(i.1) -: f3 'a'
(i.1) -: f3 'a';'b'
(i.2) -: f3 'b'
(i.3) -: f3 'c'
(i.3) -: f3 'x'

f4=: 4 : 0
 select. 
   if. -.x e. 1 2 3 4 do. x=.1 end.
   x
  case. 1 do. s=.0  for_i. i.y   do. s=.s+i end.
  case. 2 do. s=.1  for.   i.y,0 do. s=.+:s end.
  case. 3 do. s=.$0 for_j. i.y   do. s=.s,j end.
  case. 4 do. s=.0  while. 1<y   do. s=.>:s [ y=.-:y end.
 end.
)

(2&!       -: 1&f4)"0 x=:?4 5$100
(2&^       -: 2&f4)"0 x=:?4 5$20
(i.        -: 3&f4)"0 x=:?4 5$100
(>.@(2&^.) -: 4&f4)"0 x=:1+?4 5$1000
(1 f4 9)   -: 0 f4 9
(1 f4 9)   -: 5 f4 9

f5=: 3 : 0
 t=. ''
 select. y 
  case.  1 do. t=.t,<i.1
  fcase. 2 do. t=.t,<i.2
  case.  3 do. t=.t,<i.3
  fcase. 4 do. t=.t,<i.4
 end.
)

(i.&.>,1 ) -: f5 1
(i.&.>2 3) -: f5 2
(i.&.>,3 ) -: f5 3
(i.&.>,4 ) -: f5 4

'control error' -: ex '3 : ''select. do.        end.'' '
'control error' -: ex '3 : ''select. y          end.'' '
'control error' -: ex '3 : ''select. y case.  1 end.'' '
'control error' -: ex '3 : ''select. y fcase. 1 end.'' '

g1=: 3 : 0
 select. if. y do. y end.
  case. 1 do. i.1
  case. 2 do. i.2
 end.
)

(i.1) -: g1 1
(i.2) -: g1 2
'control error' -: g1 etx 0

g2=: 3 : 0
 select. foobar
  case. 1 do. i.1
  case. 2 do. i.2
 end.
)

'control error' -: g2 etx 3

g3=: 3 : 0
select. y
 case. 1        do. 'one'
 case. +        do. 'two'
 case. 3        do. 'three'
end.
)

'one'           -: g3 1

'control error' -: g3 etx 2
'control error' -: g3 etx 3
'control error' -: g3 etx 4

g4=: 3 : 0
select. y
 case. 1        do. 'one'
 case. undef123 do. 'two'
 case. 3        do. 'three'
end.
)

'one'           -: g4 1

'control error' -: g4 etx 2
'control error' -: g4 etx 3
'control error' -: g4 etx 4


4!:55 ;:'f0 f1 f2 f3 f4 f5 g1 g2 g3 g4 x'


