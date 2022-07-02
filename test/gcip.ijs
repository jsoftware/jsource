prolog './gcip.ijs'
NB. +/ . * ------------------------------------------------------------------

NB. **************************************** matrix product **********************************

N=: 3 <. <: 1 { 8 T. ''  NB. max # worker threads
1: 0&T.@''^:(0 >. N-1 T.'')''  NB. create tasks if needed
1: 55&T.@''^:(0 >. N-~1 T.'')''  NB. Destroy tasks as needed
N = 1 T. ''

X=: +/ . *
XT=: X t.''

NB. Test blocking near the 384-line boundary
b=: 20 64 ?@$ 0 
{{ for_i. (384*>:N) + i: 80*>:N do.
  a=: (i,20) ?@$ 0
  assert.  (a +/@(*"1 _) b) -: a X b
end. 1 }} ''

NB. Test blocking near the 384-line boundary
b=: 200 64 ?@$ 0 
{{ for_i. 50 + i. 100*>:N do.
  a=: (i,200) ?@$ 0
  assert.  (a +/@(*"1 _) b) -: a X b
end. 1 }} ''

a=: 1024 1024 ?@$ 0
b=: 1024 1024 ?@$ 0
c=: a +/@(*"1 _)t.'' b    NB.test against strawman approach
d=: a {{ (<x X y) , (x XT y) , (<x X y) }}t.'' b NB. create user task while queue has internal tasks, and vice versa
e=: a (XT , XT , XT , XT) b
*./c=>d
*./c=e

4!:55 ;:'N X XT a b c d e n'

epilog''

