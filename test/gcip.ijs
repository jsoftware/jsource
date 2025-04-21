prolog './gcip.ijs'
NB. +/ . * ------------------------------------------------------------------

NB. **************************************** matrix product **********************************
delth=: {{ while. 1 T. '' do. 55 T. '' end. 1 }}  NB. delete all worker threads
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

  thr=: (9!:58)"0 i.3

  'a b'=: (1000?@$~2,,~500)
  (a +/@(*"1 _) b) -: a X b
  0 (9!:58)"0 i.3        NB.  +/ .*  alwasy use blas
  (a +/@(*"1 _) b) -: a X b
  _1 (9!:58)"0 i.3       NB.  +/ .*  never use blas
  (a +/@(*"1 _) b) -: a X b

  empty thr (9!:58)"0 i.3

  'a b'=: (0?@$~2,,~500)
  (a +/@(*"1 _) b) -: a X b
  0 (9!:58)"0 i.3        NB.  +/ .*  alwasy use blas
  (a +/@(*"1 _) b) -: a X b
  _1 (9!:58)"0 i.3       NB.  +/ .*  never use blas
  (a +/@(*"1 _) b) -: a X b

  empty thr (9!:58)"0 i.3

  'a b'=: (0?@$~2,,~500) j. (0?@$~2,,~500)
  (a +/@(*"1 _) b) -: a X b
  0 (9!:58)"0 i.3        NB.  +/ .*  alwasy use blas
  (a +/@(*"1 _) b) -: a X b
  _1 (9!:58)"0 i.3       NB.  +/ .*  never use blas
  (a +/@(*"1 _) b) -: a X b

  empty thr (9!:58)"0 i.3

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

t1=: 3 : 0
{{0 T.0}}^:] 0 >. (1&T.'') -~ 4 <. <./ 8&T.''
echo 'threads ', ": 1 T. ''
echo 'OMP_NUM_THREADS=', (''"_)^:(0&-:) (2!:5)'OMP_NUM_THREADS'
echo 'openmp ',":9!:56'openmp'
echo 'cblas ',":c0=. 9!:56'cblas'
echo 'gemm threshold ', ": (9!:58)"0 i.3

N=. IF64{3000 4000
'A B'=. 0?@$~2,,~N

_1 (9!:58)"0 i.3       NB.  +/ .*  never use blas
t1=. 6!:2'c1=. A+/ .*B'
echo ' never blas ',' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9
0 (9!:58)"0 i.3        NB.  +/ .*  alwasy use blas
if. c0 do. 0(9!:56)'cblas' end.
t1=. 6!:2'c2=. A+/ .*B'
echo 'always blas ',' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9
assert. c1-:c2
if. c0 do.
  1(9!:56)'cblas'
  t1=. 6!:2'c2=. A+/ .*B'
  echo 'always cblas ',' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9
  assert. c1-:c2
end.
1
)
t1''

delth''

4!:55 ;:'N X XT a b c d delth e n t1 thr'

epilog''

