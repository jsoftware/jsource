NB. i.!.0 and associates timing tests -----------------------------------

L=: 1 : 0
:
 f=: x&u
 assert. (threshold*{.t) <: -/}.t=: 10 timer 'f y','x u y',:'/:x'
 1
)

R=: 1 : 0
:
 f=: u&y
 assert. (threshold*{.t) <: -/}.t=: 10 timer 'f x','x u y',:'/:y'
 1
)

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


4!:55 ;:'f L R t x y'


