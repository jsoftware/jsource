NB. 7!: -----------------------------------------------------------------

NB. Testing malloc/free; try  f 200  or  g 40000, etc.

sp  =: 7!:0
space=:7!:2

pr =: [             NB. for silent iteration
NB. pr =: 1!:2&2    NB. to see each iteration

f =: 3 : 0
 old=.sp ''
 whilst. y=.<:y do.
  pr y
 end.
 old,sp ''
)

g =: 3 : 0
 old=.sp ''
 whilst. y=.<:y do.
  t=.(?20000)$a.
  pr y
 end.
 old,sp ''
)

h =: 3 : 0
 old=.sp ''
 whilst. y=.<:y do.
  n=.>:?50
  a=. (n,n) ?@$ 5000
  b=.%.a
  d=.>./|,(=i.n)-a+/ . *b
  pr y,n,d
 end.
 old,sp ''
)

x=:20
y=:20
s=: 2 3

x =: sp ''
s =: f 30+?200
y =: sp ''
x -: y

x =: sp ''
s =: g 30+?200
y =: sp ''
x -: y

x =: sp ''
s =: h 3+?7
y =: sp ''
x -: y

t =: sp ''
0 = $$t
t = <.t
0<:t

0<space 't=:i.100'
t-:i.100
(IF64{1064 2500)>|(n*IF64{4 8)-space 'i.n' [ n=:1000
(IF64{1064 2500)>|(n*IF64{4 8)-space 'i.n' [ n=:2000

'domain error' -: 7!:2 etx 0 1
'domain error' -: 7!:2 etx 3 4 _5
'domain error' -: 7!:2 etx 3j4   
'domain error' -: 7!:2 etx 3;4 5


NB. 7!:3 ----------------------------------------------------------------

t=:7!:3 ''
t -: <.t
0 < t
2 -: #$t
2 -: {:$t
t -: /:~t
*./~:{."1 t


4!:55 ;:'f g h n old pr s sp space t x y '


