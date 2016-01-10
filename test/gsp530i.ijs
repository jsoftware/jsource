NB. } integer indices ----------------------------------------------------

d=: ?11 5 7 3 2$4

f=: 4 : '(x y}d) -: $.^:_1 x y}s'
h=: 4 : 'x f i=:g s=:(2;y)$.d'

g=: 3 : '(>:?5$5) ?@$ #s'
0  h&>c=: ; (i.1+r) <"1@comb&.>r=:#$d

g=: 3 : '(>:?3$2) ?@$ #s'
0  h&>c=: ; (i.1+r) <"1@comb&.>r=:#$d

g=: 3 : '(>:?5$5) ?@$ #s'
99 h&>c=: ; (i.1+r) <"1@comb&.>r=:#$d

g=: 3 : '(>:?3$2) ?@$ #s'
99 h&>c=: ; (i.1+r) <"1@comb&.>r=:#$d

d=: (?23 5$2) * ?23 5 7 3 2$4
c=: ; (i.1+r) <"1@comb&.>r=:#$d
f=: 3 : '(a i}d) -: $.^:_1 a i}s [ a=:h i=:g s=:(2;y)$.d'
g=: 3 : '(>:?4$4) ?@$ #d'

h=: 3 : '(}.$d) ?@$ 10'
] b=: f&>c

h=: 3 : '(($i),}.$d) ?@$ 10'
] b=: f&>c

g=: 3 : '?#d'          NB. scalar index 
h=: 3 : '?(}.$d)$5'    NB. dense replacement data
] b=: f&>c

g=: 3 : '?#d'          NB. scalar index
h=: 3 : '$.?(}.$d)$5'  NB. sparse replacement data
] b=: f&>c

s=: $. d=: 1 0 1 0 1 * 10*i.5 3
(1 2 3 (0)}d) -: 1 2 3 (0)}s
(1 2 3 (1)}d) -: 1 2 3 (1)}s
(1 2 3 (2)}d) -: 1 2 3 (2)}s
(1 2 3 (3)}d) -: 1 2 3 (3)}s
(1 2 3 (4)}d) -: 1 2 3 (4)}s


NB. amend in place -------------------------------------------------------

d=: (0=?37$3)*?37 1000$1000
s=: (2;0)$.d

i=: ,(?5$#i){i=: 4$.s
(IF64{9000 12000) > t=: 7!:2 's=: 0 i}s'
s -: d=: 0 i}d
scheck s

i=: ,(?5$#i){i=: 4$.s
x=: ?((#i),{:$s)$1000
(IF64{9000 12000) > t=: 7!:2 's=: x i}s'
s -: d=: x i}d
scheck s

i=: (?5$#i){i=:(i.37)-.,4$.s
x=: ?((#i),{:$s)$1000
s=: x i}s  NB. not in place
s -: d=: x i}d
scheck s


NB. amend in place with sparse replacement data --------------------------

d=: (0=?23456$10)*?23456 10$1000
s=: (2;0)$.d

i=: ,(?5$#i){i=: 4$.s
x=: $. (?($x)$2)*x=: ?($i{s)$1000
(IF64{40000 80000) > t=: 7!:2 's=: x i}s'
s -: d=: x i}d
scheck s

i=: (?5$#i){i=:(i.#s)-.,4$.s
x=: (?($x)$2)*x=: ?($i{s)$1000
s=: x i}s  NB. not in place
s -: d=: x i}d
scheck s


NB. sparse replacement data ----------------------------------------------

d=: (?23 5$2) * ?23 5 3 2 4$5
c=: ; (i.1+r) <"1@comb&.>r=:#$d

f=: 3 : '(a i}d) -: ($.a) i}s=:(2;y)$.d [ a=: g i'
g=: 3 : '?($y{d)$7'
m=: #d

] b=: f&>c [ i=: m-~? 3$+:m
] b=: f&>c [ i=: m-~? 6$+:m
] b=: f&>c [ i=: m-~?24$+:m
] b=: f&>c [ i=: m-~?48$+:m

s=: 1$.5 6;'';123
d=: 5 6$123
i=: ?2$5
x=: ?($i{d)$1000
s -: d
(x i}d) -: x i}s

'domain error' -: (3 4$'x') 2 0} etx $.i.2 3 4
'domain error' -: (3 4$<4 ) 2 0} etx $.i.2 3 4

'index error'  -: (i.2 3)   4 0} etx $.i.4 2 3
'index error'  -: (i.2 3)  _5 0} etx $.i.4 2 3

'length error' -: (i.2 3)   2 0} etx $.i.4 3 2


4!:55 ;:'a b c d f g h i m r s t x'


