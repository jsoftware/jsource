NB. x #^:_1 y (expand) --------------------------------------------------

expand=: (* +/\ )@[ { -@>:@(+/)@[ {. +/@[ $ ]
exp   =: #^:_1

f=: 4 : 0
 b=. ?x$2
 x=. (?(+/b)$#y){y
 b (exp -: expand) x
)

(>:(i.10),?10$500) f"0 _ ] 0 1                  
(>:(i.10),?10$500) f"0 _ a.                 
(>:(i.10),?10$500) f"0 _         ?  25$1000
(>:(i.10),?10$500) f"0 _ o.      ?  25$1000
(>:(i.10),?10$500) f"0 _ j./     ?2 25$1000
(>:(i.10),?10$500) f"0 _ 'ab';<"0?  25$1000

g=: 4 : 0
 b=. ?x$2
 x=. a.{~?((+/b),y)$#a.
 b (exp -: expand) x
)

(>:(i.10),?10$500) g"0/>:i.20


NB. x #^:_1 y (expand), scalar y ----------------------------------------

b (exp -: expand) x=:     ?2     [ b=: ?(1+?1000)$2
b (exp -: expand) x=: a.{~?#a.   [ b=: ?(1+?1000)$2
b (exp -: expand) x=:     ?1e6   [ b=: ?(1+?1000)$2
b (exp -: expand) x=: o.  ?1e6   [ b=: ?(1+?1000)$2
b (exp -: expand) x=: j./ ?2$1e6 [ b=: ?(1+?1000)$2

b (exp -: expand) x=:     ?2     [ b=:  (1+?1000)$1
b (exp -: expand) x=: a.{~?#a.   [ b=:  (1+?1000)$1
b (exp -: expand) x=:     ?1e6   [ b=:  (1+?1000)$1
b (exp -: expand) x=: o.  ?1e6   [ b=:  (1+?1000)$1
b (exp -: expand) x=: j./ ?2$1e6 [ b=:  (1+?1000)$1

b (exp -: expand) x=:     ?2     [ b=:  (1+?1000)$0
b (exp -: expand) x=: a.{~?#a.   [ b=:  (1+?1000)$0
b (exp -: expand) x=:     ?1e6   [ b=:  (1+?1000)$0
b (exp -: expand) x=: o.  ?1e6   [ b=:  (1+?1000)$0
b (exp -: expand) x=: j./ ?2$1e6 [ b=:  (1+?1000)$0


'rank error' -: 0 #^:_1 etx 'a'
'rank error' -: 0 #^:_1 etx 'abc'
'rank error' -: 1 #^:_1 etx 'a'
'rank error' -: 1 #^:_1 etx 'abc'

'length error' -: 1 0 1 #^:_1 etx 2 3 4
'length error' -: 1 0 1 #^:_1 etx 'abc'
'length error' -: 1 0 1 #^:_1 etx i.5 3


4!:55 ;:'b exp expand f g x'


