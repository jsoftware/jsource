prolog './g9x40.ijs'
NB. 9!:40 and 9!:41 -----------------------------------------------------

ws=: 9!:40 ''
1 -: type ws
0 = $#ws

'rank error'   -: 9!:40 etx 0
'rank error'   -: 9!:40 etx 1

'length error' -: 9!:40 etx 1 2
'length error' -: 9!:40 etx 'abc'
'length error' -: 9!:40 etx u:'abc'
'length error' -: 9!:40 etx 10&u:'abc'

'rank error'   -: 9!:41 etx ,0
'rank error'   -: 9!:41 etx ,1

lf=: 10{a.
nn=: <@((,'0')&;)

9!:41 ]1

f=: 3 : t=: 0 : 0
 assert. 0<:y   NB. non-negative
 %: y
)

(5!:1 <'f') -: < (,':');<(nn 3),nn ];._2 t
(5!:2 <'f') -: 3;(,':');];._2 t
(5!:5 <'f') -: '3 : 0',lf,t,')'

f=: 3 : t=: 0 : 0
  :
 assert. 0<:y   NB. non-negative
 %: y
)

x=: ':',(t i. lf)}.t

(5!:1 <'f') -: < (,':');<(nn 3),nn ];._2 x
(5!:2 <'f') -: 3;(,':');];._2 x
(5!:5 <'f') -: '3 : 0',lf,x,')'

f=: 3 : t=: 0 : 0
 assert. 0<:y   NB. non-negative
 %: y           NB. monadic definition
  :
 x %: y         NB. dyadic definition
)

x=: (i{.t),(2+i)}.t [ i=: ('  :' E. t)i.1

(5!:1 <'f') -: < (,':');<(nn 3),nn ];._2 x
(5!:2 <'f') -: 3;(,':');];._2 x
(5!:5 <'f') -: '3 : 0',lf,x,')'

f=: 4 : t=: 0 : 0
 assert. 0< x   NB. positive
 assert. 0<:y   NB. non-negative
 x %: y         NB. dyadic only definition
)

(5!:1 <'f') -: < (,':');<(nn 4),nn ];._2 t
(5!:2 <'f') -: 4;(,':');];._2 t
(5!:5 <'f') -: '4 : 0',lf,t,')'

f=: 1 : t=: 0 : 0
 assert. 2|2{6!:0 ''   NB. only on odd numbered days
 u/
)

(5!:1 <'f') -: < (,':');<(nn 1),nn ];._2 t
(5!:2 <'f') -: 1;(,':');];._2 t
(5!:5 <'f') -: '1 : 0',lf,t,')'

f=: 1 : t=: 0 : 0
  :
 assert. 2|2{6!:0 ''   NB. only on odd numbered days
 x u/ y                NB. dyadic only
)

x=: ':',(t i. lf)}.t

(5!:1 <'f') -: < (,':');<(nn 1),nn ];._2 x
(5!:2 <'f') -: 1;(,':');];._2 x
(5!:5 <'f') -: '1 : 0',lf,x,')'

f=: 1 : t=: 0 : 0
 assert. 2|2{6!:0 ''   NB. only on odd numbered days
 u/ y                  NB. monadic only
   :
)

(5!:1 <'f') -: < (,':');<(nn 1),nn ];._2 t
(5!:2 <'f') -: 1;(,':');];._2 t
(5!:5 <'f') -: '1 : 0',lf,t,')'

f=: 1 : t=: 0 : 0
 assert. 1=2|2{6!:0 ''   NB. only on odd numbered days
 u/ y                    NB. monadic defn
   :
 assert. 0=2|2{6!:0 ''   NB. only on even numbered days
 x u/ y                  NB. dyadic defn
)

x=: (i{.t),(3+i)}.t [ i=: ('   :' E. t)i.1

(5!:1 <'f') -: < (,':');<(nn 1),nn ];._2 x
(5!:2 <'f') -: 1;(,':');];._2 x
(5!:5 <'f') -: '1 : 0',lf,x,')'

f=: 2 : t=: 0 : 0
 assert. 1=2|2{6!:0 ''   NB. only on odd numbered days
 u^:n
)

(5!:1 <'f') -: < (,':');<(nn 2),nn ];._2 t
(5!:2 <'f') -: 2;(,':');];._2 t
(5!:5 <'f') -: '2 : 0',lf,t,')'

f=: 2 : t=: 0 : 0
  :
 assert. 1=2|2{6!:0 ''   NB. only on odd numbered days
 x u^:n y                NB. dyadic only
)

x=: ':',(t i. lf)}.t

(5!:1 <'f') -: < (,':');<(nn 2),nn ];._2 x
(5!:2 <'f') -: 2;(,':');];._2 x
(5!:5 <'f') -: '2 : 0',lf,x,')'

f=: 2 : t=: 0 : 0
 assert. 1=2|2{6!:0 ''   NB. only on odd numbered days
 u^:n y                  NB. monadic only
   :
)

(5!:1 <'f') -: < (,':');<(nn 2),nn ];._2 t
(5!:2 <'f') -: 2;(,':');];._2 t
(5!:5 <'f') -: '2 : 0',lf,t,')'

f=: 2 : t=: 0 : 0
 assert. 1=2|2{6!:0 ''   NB. only on odd numbered days
 u^:n y                  NB. monadic defn
   :
 assert. 0=2|2{6!:0 ''   NB. only on even numbered days
 x u^:n y                NB. dyadic defn
)

x=: (i{.t),(3+i)}.t [ i=: ('   :' E. t)i.1

(5!:1 <'f') -: < (,':');<(nn 2),nn ];._2 x
(5!:2 <'f') -: 2;(,':');];._2 x
(5!:5 <'f') -: '2 : 0',lf,x,')'

9!:41 ws


4!:55 ;:'f i lf nn t x ws'



epilog''

