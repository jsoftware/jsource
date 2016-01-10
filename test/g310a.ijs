NB. : -------------------------------------------------------------------

jnc   =: 4!:0
verb  =: 3&= @ jnc
boxed =: 32&=  @ type
open  =: 32&~: @ type
inv   =: 1 : 'x^: _1'

pow=: 2 : 0
 assert. verb <'u'
 if.     verb <'v' do. u powv v 
 elseif. boxed n   do. u powg n
 elseif. 1         do. u pown n
 end.
)

powv=: 2 : 0
 u pown (v y) y 
  :
 x u pown (x v y) y 
)

pown =: 2 : 0
 if.     #$n do. u powg (>@{.`(>@{:))"1 n;"0<y
 elseif. 0>n do. u inv pown (-n) y
 elseif. 1 do.
  assert. n-:<.n
  z=.y [ t=.0-:y [ i=._1
  while. (t-:z)<n>i=.>:i do. z=.u t=.z end.
 end.
  :
 x&u pown n y
)

powg=: 2 : 0
 assert. ($n) -: ,2
 v0=. (0{n) `:6
 v1=. (1{n) `:6 
 u pown (v0 y) v1 y
  :
 assert. ($n) -:"1 ,. 2 3
 n=. _3}.'[';n
 v0=. (0{n) `:6
 v1=. (1{n) `:6
 v2=. (2{n) `:6
 (x v0 y) u pown (x v1 y) (x v2 y)
)

(>: pow n -: >: ^: n) 1 [ n=.?10
(>: pow n -: >: ^: n) 1 [ n=.?10$10
(>: pow n -: >: ^: n) 1 [ n=._5+?10$10
(>: pow n -: >: ^: n) 1 [ n=.i.9

((0&,+,&0) pow (i.9) -: (0&,+,&0)^:(i.9)) ,1

f=: (0&,+,&0) pow (i.9)
3 -: 4!:0 <'f'

(+/\ pow n -: +/\ ^:n) 7{.1 [  n=.i.9
(+/\ pow n -: +/\ ^:n) 7{.1 [  n=._7+i.15
g=: i.@>:@+:@# - #
(+/\ pow g -: +/\ ^:g) 7{.1 

(2&o. pow _ -: 2&o. ^:_) 1

f=:(#:1 3)&(+/ .*)
(f pow n -: f ^: n) 0 1 [ n=._5+i.11

(,~     pow n -: ,~    ^:n) 'x' [ n=.i.6
(,&'x'  pow n -: ,&'x' ^:n) ''  [ n=.i.6
(,&'xx' pow n -: ,&'xx'^:n) ''  [ n=.i.6


4!:55 ;:'boxed f g inv jnc n open pow powg pown powv verb' 


