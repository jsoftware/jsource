prolog './gsp222.ijs'
NB. ~: monad ------------------------------------------------------------

f=: 3 : '(scheck t) *. (~:p) -: t=: ~:s=: (2;y)$.p'

c=: ; (i.1+r) <"1@comb&.>r=: 4
f&> c [ p=: (?200$100) { (?100 4 5$2) *    ?  100 4 5 3$2
f&> c [ p=: (?200$100) { (?100 4 5$2) *    ?  100 4 5 3$100
f&> c [ p=: (?100$ 50) { (? 50 4 5$2) * o. ?   50 4 5 3$100
f&> c [ p=: (?100$ 50) { (? 50 4 5$2) * j./?2  50 4 5 3$100

c=: ; (i.1+r) <"1@comb&.>r=: 3
f&> c [ p=:                          ?100 4 5$1000
f&> c [ p=:                   0 (0)} ?100 4 5$1000
f&> c [ p=:                   0 (9)} ?100 4 5$1000
f&> c [ p=:                   0    * ?100 4 5$1000
f&> c [ p=: (?200$100) {             ?100 4 5$1000
f&> c [ p=: (?200$100) {(?100 4$2) * ?100 4 5$1000

(~:$.p) -: ~:p=: i.0
(~:$.p) -: ~:p=: i.0 5
(~:$.p) -: ~:p=: i.5 0

p=: ?100$1e9
q=: (?100$20){?20 5$10
s=: q p}1 $. 1e9 5 ; 0 ; 2-2
t=: ~:s
scheck t
($t)   -: ,#s
(2$.t) -: ,0
(3$.t) -: 0
(4$.t) -: ,. /:~ ~. (s i.5$0) , (~:q/:p)#p/:p
(5$.t) -: (#4$.t)$1

s=: 4 (0)}1 $. 10 ; 0 ; 5
(~:s) -: ~:0$.s
scheck ~:s


NB. ~: dyad -------------------------------------------------------------

f=: 4 : '(p~:q) -: ((2;x)$.p) ~: (2;y)$.q'

p=: ?4 3 5 2$2
q=: ?4 3 5 2$2
c=: ; (i.1+r) <"1@comb&.>r=:#$p
f&>/~c

p=: ?4 3 5 2$4
q=: ?4 3 5 2$4
c=: ; (i.1+r) <"1@comb&.>r=:#$p
f&>/~c

p=: o.?4 3 5 2$4
q=: o.?4 3 5 2$4
c=: ; (i.1+r) <"1@comb&.>r=:#$p
f&>/~c

p=: j./?2 3 5 2$4
q=: j./?2 3 5 2$4
c=: ; (i.1+r) <"1@comb&.>r=:#$p
f&>/~c





epilog''

