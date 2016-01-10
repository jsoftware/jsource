NB. #;.n ----------------------------------------------------------------

f=: 3 : 0
 assert. (#;. 1 q) -: #;. 1 qs=: (2;y)$.q
 assert. (#;._1 q) -: #;._1 qs
 assert. (#;. 2 q) -: #;. 2 qs
 assert. (#;._2 q) -: #;._2 qs
 1
)

f&>'';0 [ q=:      ?1000$2
f&>'';0 [ q=:    0=?1000$10
f&>'';0 [ q=: 1, 0=?1000$10
f&>'';0 [ q=: 0, 0=?1000$10
f&>'';0 [ q=: 1,~0=?1000$10
f&>'';0 [ q=: 0,~0=?1000$10

j=: ?1000$2e9
s=: 1 j}1$.2e9;0;0
(#;.1~s) -: 2 -~/\(/:~j),#s


NB. f;.n ----------------------------------------------------------------

f=: 1 : 0
  :
 assert. (scheck t) *. (x u;. 1 y) -: t=: x u;. 1      $. y
 assert. (scheck t) *. (x u;. 1 y) -: t=: x u;. 1 (2;0)$. y
 assert. (scheck t) *. (x u;. 1 y) -: t=: x u;. 1 (2;1)$. y
 assert. (scheck t) *. (x u;._1 y) -: t=: x u;._1      $. y
 assert. (scheck t) *. (x u;._1 y) -: t=: x u;._1 (2;0)$. y
 assert. (scheck t) *. (x u;._1 y) -: t=: x u;._1 (2;1)$. y
 assert. (scheck t) *. (x u;. 2 y) -: t=: x u;. 2      $. y
 assert. (scheck t) *. (x u;. 2 y) -: t=: x u;. 2 (2;0)$. y
 assert. (scheck t) *. (x u;. 2 y) -: t=: x u;. 2 (2;1)$. y
 assert. (scheck t) *. (x u;._2 y) -: t=: x u;._2      $. y
 assert. (scheck t) *. (x u;._2 y) -: t=: x u;._2 (2;0)$. y
 assert. (scheck t) *. (x u;._2 y) -: t=: x u;._2 (2;1)$. y
 1
)

n=: 211
p +/ f q [ p=:   ?n$2 [ q=: (?n$2) * ?(n,3)$5
p ]  f q
p +/ f q [ p=: 0=?n$5 [ q=: (?n$2) * ?(n,3)$5
p ]  f q
p +/ f q [ p=: 0<?n$5 [ q=: (?n$2) * ?(n,3)$5
p ]  f q

classify=: 4 : 0
 (#x) }. (+/\i<#x) i}i=. /:x,y
)

cut1=: 1 : 0
  :
 i=. (4$.x) classify 4$.y
 s=. (i~:}:_1,i) u ;.1 (5$.y)
 ((0 e. i)}. s) (<:~.(0~:i)#i)}1 $. (+/x);0;-~{.s
)

i=: ?1000$2e9
j=: ?5000$2e9
x=: ?5000$2e9
p=: 1 i}1$.2e9;0;0
q=: x j}1$.2e9;0;2-2
(p +/;.1 q) -: p +/ cut1 q


4!:55 ;:'classify cut1 f i j n p q qs s t x'


