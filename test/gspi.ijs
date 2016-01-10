NB. (dense vector) i. sparse , also i: ----------------------------------

df=: $.^:_1

f=: 4 : 0
 qs=: (3;x)$.(2;y)$.q
 assert. (scheck t) *. (p i. df qs) -: t=. p i. qs
 assert. (scheck t) *. (p i: df qs) -: t=. p i: qs
 1
)

q=: (?7 5$2) * ?7 5 8$50
c=: ; (i.1+r) <"1@comb&.>r=:#$q

0 99 f&>/ c [ p=:      ?47$50
0 99 f&>/ c [ p=:      ?47$2
0 99 f&>/ c [ p=: ]&.o.?47$50
0 99 f&>/ c [ p=: ]&.j.?47$50
0 99 f&>/ c [ p=: 'kakistocratic meconium'

p=: 1+?47$50
0 99 f&>/ c [ q=:       (?7 5 3$2) * ?7 5 3 8$2
0 99 f&>/ c [ q=:       (?7 5 3$2) * ?7 5 3 8$50
0 99 f&>/ c [ q=: ]&.o. (?7 5 3$2) * ?7 5 3 8$50
0 99 f&>/ c [ q=: ]&.j. (?7 5 3$2) * ?7 5 3 8$50

p=: (?47$2) * ?47$50
0 99 f&>/ c [ q=:       (?7 5 3$2) * ?7 5 3 8$2
0 99 f&>/ c [ q=:       (?7 5 3$2) * ?7 5 3 8$50
0 99 f&>/ c [ q=: ]&.o. (?7 5 3$2) * ?7 5 3 8$50
0 99 f&>/ c [ q=: ]&.j. (?7 5 3$2) * ?7 5 3 8$50


NB. (sparse vector) i. sparse , also i: ---------------------------------

f=: 4 : 0
 qs=: (3;x)$.(2;y)$.q
 assert. (scheck t) *. (ps i.&df qs) -: t=. ps i. qs
 assert. (scheck t) *. (ps i:&df qs) -: t=. ps i: qs
 1
)

q=: (?7 5$2) * ?7 5 8$50
c=: ; (i.1+r) <"1@comb&.>r=:#$q

0 99 f&>/ c [ ps=: $.      ?47$50
0 99 f&>/ c [ ps=: $.      ?47$2
0 99 f&>/ c [ ps=: $. ]&.o.?47$50
0 99 f&>/ c [ ps=: $. ]&.j.?47$50

f=: 4 : 0
 ps=: (3;pe)$.(2;x)$.p 
 qs=: (3;qe)$.(2;y)$.q
 assert. (scheck t) *. (ps i.&df qs) -: t=. ps i. qs
 assert. (scheck t) *. (ps i:&df qs) -: t=. ps i: qs
 1
)

p=: (?47$2) * ?47$50
('';0) f&>/ c [ pe=: 0  [ qe=: 0
('';0) f&>/ c [ pe=: 0  [ qe=: 99
('';0) f&>/ c [ pe=: 99 [ qe=: 0
('';0) f&>/ c [ pe=: 99 [ qe=: 99

p=: 1+?47$50
('';0) f&>/ c [ pe=: 0  [ qe=: 0
('';0) f&>/ c [ pe=: 0  [ qe=: 99
('';0) f&>/ c [ pe=: 99 [ qe=: 0
('';0) f&>/ c [ pe=: 99 [ qe=: 99

p=: (1+?47$50),0 0 0
('';0) f&>/ c [ pe=: 0  [ qe=: 0
('';0) f&>/ c [ pe=: 0  [ qe=: 99
('';0) f&>/ c [ pe=: 99 [ qe=: 0
('';0) f&>/ c [ pe=: 99 [ qe=: 99

p=: 0 0 0,(1+?47$50)
('';0) f&>/ c [ pe=: 0  [ qe=: 0
('';0) f&>/ c [ pe=: 0  [ qe=: 99
('';0) f&>/ c [ pe=: 99 [ qe=: 0
('';0) f&>/ c [ pe=: 99 [ qe=: 99

ps=: 0 (?5$10)}$. ?10$4
(i.~ps) -: i.~ df ps
(i:~ps) -: i:~ df ps

f=: 4 : 0
 ps=: $. x
 qs=: $. y
 assert. (ps i. qs) -: x i. y
 assert. (ps i: qs) -: x i: y
 1
)
0    f ?4 5$2
1    f ?4 5$10
(,0) f ?20$2
(,0) f ?20$10
(,1) f ?20$2
(,1) f ?20$10


NB. (sparse vector) i. dense , also i: ----------------------------------

f=: 4 : 0
 ps=: (3;x)$.(2;y)$.p 
 assert. ((df ps) i. q) -: ps i. q
 assert. ((df ps) i: q) -: ps i: q
 1
)

q=: (?7 5$2) * ?7 5 8$50

p=: (?47$2) * ?47$50
0  f 0
0  f ''
99 f 0
99 f ''

p=: 1+?47$50
0  f 0
0  f ''
99 f 0
99 f ''

((df ps) i. 0) -: ps i. 0 [ ps=:        $. (1+?47$50), 0 0 0 
((df ps) i. 0) -: ps i. 0 [ ps=:        $. (1+?47$50),~0 0 0 
((df ps) i. 0) -: ps i. 0 [ ps=: 0 (17)}$.  1+?47$50 

((df ps) i: 0) -: ps i: 0 [ ps=:        $. (1+?47$50), 0 0 0 
((df ps) i: 0) -: ps i: 0 [ ps=:        $. (1+?47$50),~0 0 0 
((df ps) i: 0) -: ps i: 0 [ ps=: 0 (17)}$.  1+?47$50 

p=: (?47$2) * ?47$50

0  f 0  [ q=: 0
0  f ''
99 f 0
99 f ''
0  f 0  [ q=: (?#p){p
0  f ''
99 f 0
99 f ''
0  f 0  [ q=: 99
0  f ''
99 f 0
99 f ''
0  f 0  [ q=: 123456
0  f ''
99 f 0
99 f ''

0  f 0  [ q=: s: ' foo upon thee'
0  f ''
99 f 0
99 f ''
0  f 0  [ q=: ' foo upon thee'
0  f ''
99 f 0
99 f ''
0  f 0  [ q=: ;:'Cogito, ergo sum.'
0  f ''
99 f 0
99 f ''

d=: df s=: 0(2)}$. i.5
(s i. 0) -: d i. 0
(s i: 0) -: d i: 0


NB. i. and i: on general dense or sparse arguments ----------------------

f=: 4 : 0
 (x=3$.t) *. (i:~ y) -: t=. i:~ $. y
)

d=: i.8 2
8 f d
7 f  8{.5{.d
2 f _8{.5{.d
7 f 0*d
0 f i.0 2

2 = 3 $. i:~       $.0 (2)}i.8 2
6 = 3 $. i:~ 0 (6)}$.0 (0)}i.8 2

f0=: 4 : 0
 ps=: (2;x)$.p
 qs=: (2;y)$.q
 assert. (p i. q) -: i=. ps i. qs
 assert. (p i: q) -: j=. ps i: qs
 if. #$i do. assert. (scheck i) *. (scheck j) end.
 assert. (scheck i) *. (p i. p) -: i=. ps i. ps
 assert. (scheck i) *. (p i: p) -: i=. ps i: ps
 1
)

f1=: 4 : 0
 ps=: (2;x)$.p
 assert. (p i. q) -: i=. ps i. q
 assert. (p i: q) -: j=. ps i: q
 if. #$i do. assert. (scheck i) *. (scheck j) end.
 1
)

f2=: 4 : 0
 qs=: (2;x)$.q
 assert. (p i. q) -: i=. p  i. qs
 assert. (p i: q) -: j=. p  i: qs
 if. #$i do. assert. (scheck i) *. (scheck j) end.
 1
)

f3=: 4 : 0
 ps=: (2;x)$.p
 qs=: (2;y)$.q
 assert. (p i.!.0 q) -: i=. ps i.!.0 qs
 assert. (p i:!.0 q) -: j=. ps i:!.0 qs
 if. #$i do. assert. (scheck i) *. (scheck j) end.
 assert. (scheck i) *. (p i.!.0 p) -: i=. ps i.!.0 ps
 assert. (scheck i) *. (p i:!.0 p) -: i=. ps i:!.0 ps
 1
)


p=: (?100 2$2) * ?100 2 3$10
q=: (?200$150){p,(?50 2$2) * ?50 2 3$10
c=: ; (i.1+r) <"1@comb&.>r=:#$p
d=: ; (i.1+s) <"1@comb&.>s=:#$q

c f0&>/d
c f1&>/0
0 f2&>/d
c f3&>/d

c f0&>/d [ p=: p*1.5-0.5 [ q=: q*1.5-0.5
c f1&>/0
0 f2&>/d
c f3&>/d

c f0&>/d [ p=: p*1j5-0j5 [ q=: q*1j5-0j5
c f1&>/0
0 f2&>/d
c f3&>/d

p=: (?100 2$2) * ?100 2 3$10
q=: (?150){p,(?50 2$2) * ?50 2 3$10
c=: ; (i.1+r) <"1@comb&.>r=:#$p
d=: ; (i.1+s) <"1@comb&.>s=:#$q

c f0&>/d
c f1&>/0
0 f2&>/d
c f3&>/d

c f0&>/d [ p=: p*1.5-0.5 [ q=: q*1.5-0.5
c f1&>/0
0 f2&>/d
c f3&>/d

c f0&>/d [ p=: p*1j5-0j5 [ q=: q*1j5-0j5
c f1&>/0
0 f2&>/d
c f3&>/d

d=: (i.6 2),.99
s=: 8 $. (3;99) $. $. d
s=: 11 (<2 2)}s
s=: 99 (<2 2)}s
d=: $.^:_1 s

(d i. d) -: s i. (3;99)$. $. d
(d i: d) -: s i: (3;99)$. $. d

s=: 0 (?5$10)}$.?10 3$4
(i.~s) -: i.~ df s
(i:~s) -: i:~ df s

s=: 0 (0 1 2)} $. 5 2?@$20
(df i.~s) -: i.~ df s
(df i:~s) -: i:~ df s

t=: 0 (2 3 4)} $. 5 2 ?@$ 20
(df i.~t) -: i.~ df t
(df i:~t) -: i:~ df t

d=: (?1009$2) * ?1009 2 3$10
c=: ; (i.1+r) <"1@comb&.>r=:#$d

f=: 3 : 'assert. (i.~ $.^:_1 s) -: i.~ s=: (b=:   ?((#d),2)$2) * (2;y)$.d'
f&> c

f=: 3 : 'assert. (i.~ $.^:_1 s) -: i.~ s=: (b=: 0=?((#d),2)$5) * (2;y)$.d'
f&> c

f=: 3 : 'assert. (i:~ $.^:_1 s) -: i:~ s=: (b=:   ?((#d),2)$2) * (2;y)$.d'
f&> c

f=: 3 : 'assert. (i:~ $.^:_1 s) -: i:~ s=: (b=: 0=?((#d),2)$5) * (2;y)$.d'
f&> c


4!:55 ;:'b c d df f f0 f1 f2 f3 f4 p pe ps q qe qs r s x'


