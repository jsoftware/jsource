prolog './gsp101.ijs'
NB. +. ------------------------------------------------------------------

f=: 3 : '(+.q) -: +.(2;y)$.q'

q=: ?2 4 7 5 3$5
c=: ; (i.1+r) <"1@comb&.>r=:#$q
f&> c

q=: j./ ?2 2 4 7 5 3$5
c=: ; (i.1+r) <"1@comb&.>r=:#$q
f&> c

(+.3j3+i.2 3)                -: +.     3j3+$.i.2 3

'non-unique sparse elements' -: +. etx 3j4+$.i.2 3

f=: 4 : '(p+.q) -: ((2;x)$.p) +. (2;y)$.q'
g=: 3 : '(a+.q) -: a          +. (2;y)$.q'

p=: (?2 4 7$2) * ?2 4 7 3$2
q=: (?2 4 7$2) * ?2 4 7 3$2
a=: ?2
c=: ; (i.1+r) <"1@comb&.>r=:#$p
f&>/~c
g&>c

p=: (?4 5$2) * ?4 5 3$4e6
q=: (?4 5$2) * ?4 5 3$4e6
a=: ?1e6
c=: ; (i.1+r) <"1@comb&.>r=:#$p
f&>/~c
g&>c

p=: (?4 5$2) * o.?4 5 3$4
q=: (?4 5$2) * o.?4 5 3$4
a=: o.?16
c=: ; (i.1+r) <"1@comb&.>r=:#$p
f&>/~c
g&>c

p=: (?4 5$2) * j./?2 4 5 3$4
q=: (?4 5$2) * j./?2 4 5 3$4
a=: j./?26 26
c=: ; (i.1+r) <"1@comb&.>r=:#$p
f&>/~c
g&>c




epilog''

