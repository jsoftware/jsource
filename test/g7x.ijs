prolog './g7x.ijs'
NB. 7!: -----------------------------------------------------------------

NB. Testing malloc/free; try  f 200  or  g 40000, etc.
delth =: {{ while. 1 T. '' do. 55 T. '' end. 1 }}  NB. delete all worker threads

NB. We use whatever threads the system defines

sp  =: 7!:0
spa =: 7!:8
space=:7!:2

pr =: [             NB. for silent iteration
NB. pr =: 1!:2&2    NB. to see each iteration

chk=: (<UNAME) e. 'FreeBSD';'OpenBSD'

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
  try. b=.%.a catch. b =. a =. =/~ i. # a end.
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

x =: spa ''
s =: h 3+?7
empty echo^:chk 'g7x a1'
NB. the following line hang on freebsd
1: 6!:3 ] 3.00  NB. must give the task a chance to be dequeued, 0.01 is insufficient
empty echo^:chk 'g7x a2'
y =: spa ''
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


NB. 7!:1 ----------------------------------------------------------------

1: (3!:3)&1  ^: _1 :: 1: ] 9
2e10 > {:7!:1''


NB. 7!:3 ----------------------------------------------------------------

t=:7!:3 ''
t -: <.t
0 < t
2 -: #$t
2 -: {:$t
t -: /:~t
*./~:{."1 t





epilog''

