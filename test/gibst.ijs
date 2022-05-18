prolog './gibst.ijs'
NB. i.!.0 and associates timing tests -----------------------------------

randuni''

L=: 1 : 0
:
 f=: x&u
 assert. THRESHOLD +. (threshold*{.t) <: -/}.t=: 10 timer 'f y','x u y',:'/:x'
 1
)

R=: 1 : 0
:
 f=: u&y
 assert. THRESHOLD +. (threshold*{.t) <: -/}.t=: 10 timer 'f x','x u y',:'/:y'
 1
)

NB. integer
x=: <"1 ] 23 2 ?@$ 100
y=: <"1 ] 3581 2 ?@$ 100

y i.!.0 L x
y i:!.0 L x
NB. x -.!.0 R y  NB. does not use prehasing as of 2005-11-16
x e.!.0 R y

x (e.!.0 i. 0:   ) R y
x (e.!.0 i. 1:   ) R y
x (e.!.0 i: 0:   ) R y
x (e.!.0 i: 1:   ) R y
x ([: + /  e.!.0 ) R y
x (   + /@(e.!.0)) R y
x ([: +./  e.!.0 ) R y
x (   +./@(e.!.0)) R y
x ([: *./  e.!.0 ) R y
x (   *./@(e.!.0)) R y
x ([: I.   e.!.0 ) R y
x (   I. @(e.!.0)) R y


NB. double
x=: <"1 ] 0.5+ 23 2 ?@$ 100
y=: <"1 ] 0.5+ 3581 2 ?@$ 100

y i.!.0 L x
y i:!.0 L x
NB. x -.!.0 R y  NB. does not use prehasing as of 2005-11-16
x e.!.0 R y

x (e.!.0 i. 0:   ) R y
x (e.!.0 i. 1:   ) R y
x (e.!.0 i: 0:   ) R y
x (e.!.0 i: 1:   ) R y
x ([: + /  e.!.0 ) R y
x (   + /@(e.!.0)) R y
x ([: +./  e.!.0 ) R y
x (   +./@(e.!.0)) R y
x ([: *./  e.!.0 ) R y
x (   *./@(e.!.0)) R y
x ([: I.   e.!.0 ) R y
x (   I. @(e.!.0)) R y


NB. symbol
x=: <"1 ] (_100{.sdot0){~ 23 2 ?@$ 100
y=: <"1 ] (_100{.sdot0){~ 3581 2 ?@$ 100

NB. !!! always failed
NB. y i.!.0 L x
NB. y i:!.0 L x
NB. x -.!.0 R y  NB. does not use prehasing as of 2005-11-16
NB. x e.!.0 R y

NB. x (e.!.0 i. 0:   ) R y
NB. x (e.!.0 i. 1:   ) R y
NB. x (e.!.0 i: 0:   ) R y
NB. x (e.!.0 i: 1:   ) R y
NB. x ([: + /  e.!.0 ) R y
NB. x (   + /@(e.!.0)) R y
NB. x ([: +./  e.!.0 ) R y
NB. x (   +./@(e.!.0)) R y
NB. x ([: *./  e.!.0 ) R y
NB. x (   *./@(e.!.0)) R y
NB. x ([: I.   e.!.0 ) R y
NB. x (   I. @(e.!.0)) R y


4!:55 ;:'adot1 adot2 sdot0 f L R t x y'
randfini''


epilog''

