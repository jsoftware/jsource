prolog './gcip.ijs'
NB. +/ . * ------------------------------------------------------------------

NB. **************************************** matrix product **********************************

N=: <: 9!:56'maxtasks'
1: 0&T.@''^:(N-1 T.'')''

X=: +/ . *
XT=: X t.''
a=: 1024 1024 ?@$ 0
b=: 1024 1024 ?@$ 0
c=: a +/@(*"1 _)t.'' b    NB.test against strawman approach
d=: a {{ (<x X y) , (x XT y) , (<x X y) }}t.'' b NB. create user task while queue has internal tasks, and vice versa
e=: a (XT , XT , XT , XT) b
*./c=>d
*./c=e

4!:55 ;:'N X XT a b c d e'

epilog''

