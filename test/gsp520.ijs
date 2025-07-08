prolog './gsp520.ijs'
NB. { -------------------------------------------------------------------

f=: 3 : '(i{d) -: $.^:_1 i{(2;y)$.s'
d=: ?6 5 4 3 2$100
s=: $.d
i=: ?2 4$6

f"1 ] 1 comb 5
f"1 ] 2 comb 5
f"1 ] 3 comb 5
f"1 ] 4 comb 5
f"1 ] 5 comb 5

r=: 3
f=: 3 : '(i{"r d) -: $.^:_1 i{"r (2;y)$.s'
d=: ?6 5 4 3 2$100
s=: $.d
i=: ?2 4 1$(-r){$s

f"1 ] 1 comb 5
f"1 ] 2 comb 5
f"1 ] 3 comb 5
f"1 ] 4 comb 5
f"1 ] 5 comb 5

r=: 4
f=: 3 : '(i{"r d) -: $.^:_1 i{"r (2;y)$.s'
d=: ?6 5 4 3 2$100
s=: $.d
i=: ?2 1 4$(-r){$s

f"1 ] 1 comb 5
f"1 ] 2 comb 5
f"1 ] 3 comb 5
f"1 ] 4 comb 5
f"1 ] 5 comb 5

d=: ? 11 7 8 3 2$4

f=: 4 : '(y{"(_,x) d) -: $.^:_1 y{"(_,x) s'
h=: 3 : '*./ ; g&.> >:i.#$s=:(2;y)$.d'

g=: 3 : 'y f"0 i.(-y){$d'
h&>c=: ; (i.1+r) <"1@comb&.>r=:#$d

g=: 3 : 'y f ?2 3 4$(-y){$d'
h&>c=: ; (i.1+r) <"1@comb&.>r=:#$d

d=: ? 11 7 3 2$4
f=: 3 : '(y{d) -: $.^:_1 y{s'
g=: 3 : '?&.> (>:@?&.>(?n$5)$&.>4)$&.>n{.s [ n=.1+?#s=. $y'
h=: 3 : 'f <i=:g s=:(2;y)$.d'
h&>c=: ; (i.1+r) <"1@comb&.>r=:#$d

s=: $. ?11 7 3 2$4
((<<"0 i){s) -: (<i){s [ i=: ?}:$s 

(0{x) -: 0{$.x=: 2 4 0 8 16 24
(2{x) -: 2{$.x
(0{x) -: 0{(2;'')$.x
(2{x) -: 2{(2;'')$.x
(i{x) -: (i=: <?$x){$.x=: ?2 3 4$3

d=: ?11 5 7 3 2$4
f=: 3 : '(y{d) -: y{s'
g=: 4 : '?((>:?(?5)$5),x)$x{.$y'
h=: 4 : 'f i=:<"1 x g s=:(2;y)$.d'
(>:i.r) h&>/ c=: ; (i.1+r) <"1@comb&.>r=:#$d

0 -: (<r$<0){$.(>:i.r)$0 [ r=: 1
0 -: (<r$<0){$.(>:i.r)$0 [ r=: 2
0 -: (<r$<0){$.(>:i.r)$0 [ r=: 3
0 -: (<r$<0){$.(>:i.r)$0 [ r=: 4
0 -: (<r$<0){$.(>:i.r)$0 [ r=: 5

d=: (?11 5 7$2)*?11 5 7 3 2$4
h=: 3 : 's -: i{s=:(2;y)$.d'
c=: ; (i.1+r) <"1@comb&.>r=:#$d

h&>c [ i=: <''
h&>c [ i=: <$0

NB. index from sparse y should result in the same error as index from dense array
spd=: {etx -: ({etx $.)
spde=: { :: ('error'"_) -: ({ :: ('error'"_))  NB. use when order of analysis causes different error

      'abc'   spd i.2 3
(u:   'abc')  spd i.2 3
(10&u:'abc')  spd i.2 3

(<2 3$<0) spd i.2 3
(<2;3;4)  spd i.5 6

 2 spd i.2 3
_3 spd i.2 3

(<'ab')       spd i.2 3
(<2;'a')      spde i.2 3
(<u:'ab')     spd i.2 3
(<2;u:'a')    spde i.2 3
(<10&u:'ab')  spd i.2 3
(<2;10&u:'a') spde i.2 3





epilog''

