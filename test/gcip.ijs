prolog './gcip.ijs'
NB. +/ . * ------------------------------------------------------------------

NB. **************************************** matrix product **********************************
delth=: {{ while. 1 T. '' do. 55 T. '' end. 1 }}  NB. delete all worker threads
delth''  NB. make sure we start with an empty system

thr=: (9!:58)"0 i.3
{{
N=: (QKTEST{3 2) <. <: 1 { 8 T. ''  NB. max # worker threads, limited to 3
for. i. N do.

  X=: +/ . *
  XT=: X t.''

  _1 (9!:58)"0 i.3       NB.  +/ .*  never use blas

  echo 'Test blocking near the 384-line boundary'
NB. Test blocking near the 384-line boundary
  b=: 20 64 ?@$ 0
  {{ for_i. (384*>:N) + i: 80*>:N do.
    a=: (i,20) ?@$ 0
    assert. (a +/@(*"1 _) b) -: a X b
  end. 1 }} ''

NB. Test blocking near the 384-line boundary
  b=: 200 64 ?@$ 0
  {{ for_i. 50 + i. 100*>:N do.
    a=: (i,200) ?@$ 0
    assert. (a +/@(*"1 _) b) -: a X b
  end. 1 }} ''

NB. Test small sizes, should break into blockedmmults
  b=: 20 64 ?@$ 0
  {{ for_i. 20+i. 66 * >:N do.
    a=: (i,20) ?@$ 0
    assert. (a +/@(*"1 _) b) -: a X b
  end. 1 }} ''

  empty thr (9!:58)"0 i.3
  echo 'Test blas integer'
  'a b'=: (1000?@$~2,,~QKTEST{500 100)
  (a +/@(*"1 _) b) -: a X b
  0 (9!:58)"0 i.3        NB.  +/ .*  alwasy use blas
  (a +/@(*"1 _) b) -: a X b
  _1 (9!:58)"0 i.3       NB.  +/ .*  never use blas
  (a +/@(*"1 _) b) -: a X b

  empty thr (9!:58)"0 i.3
  echo 'Test blas floating'
  'a b'=: (0?@$~2,,~QKTEST{500 100)
  (a +/@(*"1 _) b) -: a X b
  0 (9!:58)"0 i.3        NB.  +/ .*  alwasy use blas
  (a +/@(*"1 _) b) -: a X b
  _1 (9!:58)"0 i.3       NB.  +/ .*  never use blas
  (a +/@(*"1 _) b) -: a X b

  empty thr (9!:58)"0 i.3
  echo 'Test blas complex'
  'a b'=: (0?@$~2,,~QKTEST{500 100) j. (0?@$~2,,~QKTEST{500 100)
  (a +/@(*"1 _) b) -: a X b
  0 (9!:58)"0 i.3        NB.  +/ .*  alwasy use blas
  (a +/@(*"1 _) b) -: a X b
  _1 (9!:58)"0 i.3       NB.  +/ .*  never use blas
  (a +/@(*"1 _) b) -: a X b

  _1 (9!:58)"0 i.3       NB.  +/ .*  never use blas

  echo 'Test multithreading'
  a=: (QKTEST{::1024 1024;128 128) ?@$ 0
  b=: (QKTEST{::1024 1024;128 128) ?@$ 0
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
empty thr (9!:58)"0 i.3


epilog''

