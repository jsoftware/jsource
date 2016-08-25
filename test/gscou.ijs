NB. s: unicode -----------------------------------------------------

c=: ' a b cd chthonic boustrophedonic octothorpe'
t=: s: c
s=: s: u: c
s -: t
s -:&(6&s:) t
(1 s: s) -: ; '`'&,   &.><;._1 c
(2 s: s) -: ; ,&(0{a.)&.><;._1 c
(3 s: s) -: (<;._1 c) ,&> ((>./ - ])#;.1 c)$&.>0{a.
(4 s: s) -: (<;._1 c) ,&> ((>./ - ])#;.1 c)$&.>' '
(5 s: s) -:  <;._1 c

d=: ' triskaidekaphobia paronomasia tasis litotes metonymic'
t=: s,s: d
(1 s: t) -: ; '`'&,   &.><;._1 c,d
(2 s: t) -: ; ,&(0{a.)&.><;._1 c,d
(3 s: t) -: (<;._1 c,d) ,&> ((>./ - ])#;.1 c,d)$&.>0{a.
(4 s: t) -: (<;._1 c,d) ,&> ((>./ - ])#;.1 c,d)$&.>' '
(5 s: t) -:  <;._1 c,d

i=: ?~#t
(1 s: i{t) -: ; i{'`'&,   &.><;._1 c,d
(2 s: i{t) -: ; i{,&(0{a.)&.><;._1 c,d
(3 s: i{t) -: i{(<;._1 c,d) ,&> ((>./ - ])#;.1 c,d)$&.>0{a.
(4 s: i{t) -: i{(<;._1 c,d) ,&> ((>./ - ])#;.1 c,d)$&.>' '
(5 s: i{t) -: i{ <;._1 c,d

0 s: 11
10 s: 0 s: 10
0 s: 11

c=: ' a b cd chthonic boustrophedonic octothorpe'
t=: s: c
s=: s: 10&u: c
s -: t
s -:&(6&s:) t
(1 s: s) -: ; '`'&,   &.><;._1 c
(2 s: s) -: ; ,&(0{a.)&.><;._1 c
(3 s: s) -: (<;._1 c) ,&> ((>./ - ])#;.1 c)$&.>0{a.
(4 s: s) -: (<;._1 c) ,&> ((>./ - ])#;.1 c)$&.>' '
(5 s: s) -:  <;._1 c

d=: ' triskaidekaphobia paronomasia tasis litotes metonymic'
t=: s,s: d
(1 s: t) -: ; '`'&,   &.><;._1 c,d
(2 s: t) -: ; ,&(0{a.)&.><;._1 c,d
(3 s: t) -: (<;._1 c,d) ,&> ((>./ - ])#;.1 c,d)$&.>0{a.
(4 s: t) -: (<;._1 c,d) ,&> ((>./ - ])#;.1 c,d)$&.>' '
(5 s: t) -:  <;._1 c,d

i=: ?~#t
(1 s: i{t) -: ; i{'`'&,   &.><;._1 c,d
(2 s: i{t) -: ; i{,&(0{a.)&.><;._1 c,d
(3 s: i{t) -: i{(<;._1 c,d) ,&> ((>./ - ])#;.1 c,d)$&.>0{a.
(4 s: i{t) -: i{(<;._1 c,d) ,&> ((>./ - ])#;.1 c,d)$&.>' '
(5 s: i{t) -: i{ <;._1 c,d

0 s: 11
10 s: 0 s: 10
0 s: 11

c=: ' a b cd chthonic boustrophedonic octothorpe'
t=: s: u: c
s=: s: 10&u: c
s -: t
s -:&(6&s:) t
(1 s: s) -: ; '`'&,   &.><;._1 c
(2 s: s) -: ; ,&(0{a.)&.><;._1 c
(3 s: s) -: (<;._1 c) ,&> ((>./ - ])#;.1 c)$&.>0{a.
(4 s: s) -: (<;._1 c) ,&> ((>./ - ])#;.1 c)$&.>' '
(5 s: s) -:  <;._1 c

d=: ' triskaidekaphobia paronomasia tasis litotes metonymic'
t=: s,s: d
(1 s: t) -: ; '`'&,   &.><;._1 c,d
(2 s: t) -: ; ,&(0{a.)&.><;._1 c,d
(3 s: t) -: (<;._1 c,d) ,&> ((>./ - ])#;.1 c,d)$&.>0{a.
(4 s: t) -: (<;._1 c,d) ,&> ((>./ - ])#;.1 c,d)$&.>' '
(5 s: t) -:  <;._1 c,d

i=: ?~#t
(1 s: i{t) -: ; i{'`'&,   &.><;._1 c,d
(2 s: i{t) -: ; i{,&(0{a.)&.><;._1 c,d
(3 s: i{t) -: i{(<;._1 c,d) ,&> ((>./ - ])#;.1 c,d)$&.>0{a.
(4 s: i{t) -: i{(<;._1 c,d) ,&> ((>./ - ])#;.1 c,d)$&.>' '
(5 s: i{t) -: i{ <;._1 c,d

0 s: 11
10 s: 0 s: 10
0 s: 11

c=: ' zeugma deesis acyron meiosis bdelygma chiasmus'
s=: s: u: c
t=: s: c
s -: t
s -:&(6&s:) t
(1 s: s) -: ; '`'&,   &.><;._1 c
(2 s: s) -: ; ,&(0{a.)&.><;._1 c
(3 s: s) -: (<;._1 c) ,&> ((>./ - ])#;.1 c)$&.>0{a.
(4 s: s) -: (<;._1 c) ,&> ((>./ - ])#;.1 c)$&.>' '
(5 s: s) -:  <;._1 c

s -:    s: ];._1 u: c
s -:    s: <;._1 u: c
s -: _1 s:    u: c
s -: _2 s: 1|.u: c
s -: _3 s: (<;._1 u: c) ,&> ((>./ - ])#;._1 c)$&.>u: 0
s -: _4 s: ];._1 u: c
s -: _5 s: <;._1 u: c

1 [ s: <u: 'force even alignment ',":?1e9
1 [ s: <u: 'force padding ',":?1e9

0 s: 11
10 s: 0 s: 10
0 s: 11

c=: ' zeugma deesis acyron meiosis bdelygma chiasmus'
s=: s: 10&u: c
t=: s: c
s -: t
s -:&(6&s:) t
(1 s: s) -: ; '`'&,   &.><;._1 c
(2 s: s) -: ; ,&(0{a.)&.><;._1 c
(3 s: s) -: (<;._1 c) ,&> ((>./ - ])#;.1 c)$&.>0{a.
(4 s: s) -: (<;._1 c) ,&> ((>./ - ])#;.1 c)$&.>' '
(5 s: s) -:  <;._1 c

s -:    s: ];._1 (10&u:) c
s -:    s: <;._1 (10&u:) c
s -: _1 s:    10&u: c
s -: _2 s: 1|.10&u: c
s -: _3 s: (<;._1 (10&u:) c) ,&> ((>./ - ])#;._1 c)$&.>(10&u:) 0
s -: _4 s: ];._1 (10&u:) c
s -: _5 s: <;._1 (10&u:) c

1 [ s: <10&u: 'force even alignment ',":?1e9
1 [ s: <10&u: 'force padding ',":?1e9

c=: ' zeugma deesis acyron meiosis bdelygma chiasmus'
s=: s: 10&u: c
t=: s: u: c
s -: t
s -:&(6&s:) t
(1 s: s) -: ; '`'&,   &.><;._1 c
(2 s: s) -: ; ,&(0{a.)&.><;._1 c
(3 s: s) -: (<;._1 c) ,&> ((>./ - ])#;.1 c)$&.>0{a.
(4 s: s) -: (<;._1 c) ,&> ((>./ - ])#;.1 c)$&.>' '
(5 s: s) -:  <;._1 c

NB. sbprobe
c=: s: <"0 a.
256 = #~.c
s=: s: <"0 [10&u: (i.512),65536+i.512
1024 = #~.s
t=: s: <"0 u:i.512
512 = #~.t

0 s: 11

4!:55 ;:'c d i s t u'


