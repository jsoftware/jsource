prolog './gcip.ijs'
NB. +/ . * ------------------------------------------------------------------

NB. **************************************** matrix product **********************************
delth =: {{ while. 1 T. '' do. 55 T. '' end. 1 }}  NB. delete all worker threads
delth''  NB. make sure we start with an empty system

{{
N=: (QKTEST{3 2) <. <: 1 { 8 T. ''  NB. max # worker threads, limited to 3
for. i. N do.

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

NB. Test small sizes, should break into blockedmmults
b=: 20 64 ?@$ 0 
{{ for_i. 20+i. 66 * >:N do.
  a=: (i,20) ?@$ 0
  assert.  (a +/@(*"1 _) b) -: a X b
end. 1 }} ''

a=: (QKTEST{::1024 1024;256 256) ?@$ 0
b=: (QKTEST{::1024 1024;256 256) ?@$ 0
c=: a +/@(*"1 _)t.'' b    NB.test against strawman approach
d=: a {{ (<x X y) , (x XT y) , (<x X y) }}t.'' b NB. create user task while queue has internal tasks, and vice versa
e=: a (XT , XT , XT , XT) b
assert. *./c=>d
assert. *./c=e

if. (1<{:8&T.'') *. N > 1 T. '' do. 0 T. '' end.  NB. Create another worker thread for next loop
end.
1
}} ''
delth''

4!:55 ;:'N X XT a b c d delth e n'

epilog''

