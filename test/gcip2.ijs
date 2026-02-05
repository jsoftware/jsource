prolog './gcip2.ijs'
NB. +/ . * ------------------------------------------------------------------

NB. **************************************** matrix product **********************************
delth''  NB. make sure we start with an empty system

thr=: (9!:58)"0 i.3
{{
N=: (QKTEST{3 2) <. <: 1 { 8 T. ''  NB. max # worker threads, limited to 3
for. i. N do.

  X=: +/ . *
  XT=: X t.''

  empty thr (9!:58)"0 i.3
  techo^:PRINTMSG 'Test blas integer'
  'a b'=: (1000?@$~2,,~QKTEST{500 100)
  (a +/@(*"1 _) b) -: a X b
  0 (9!:58)"0 i.3        NB.  +/ .*  alwasy use blas
  (a +/@(*"1 _) b) -: a X b
 (_1 1000{~(9!:56'pyxes')<IF64) (9!:58)"0 i.3       NB.  +/ .*  never use blas if pyxes
  (a +/@(*"1 _) b) -: a X b

  empty thr (9!:58)"0 i.3
  techo^:PRINTMSG 'Test blas floating'
  'a b'=: (0?@$~2,,~QKTEST{500 100)
  (a +/@(*"1 _) b) -: a X b
  0 (9!:58)"0 i.3        NB.  +/ .*  alwasy use blas
  (a +/@(*"1 _) b) -: a X b
  (_1 1000{~(9!:56'pyxes')<IF64) (9!:58)"0 i.3       NB.  +/ .*  never use blas if pyxes
  (a +/@(*"1 _) b) -: a X b

  empty thr (9!:58)"0 i.3
  techo^:PRINTMSG 'Test blas complex'
  'a b'=: (0?@$~2,,~QKTEST{500 100) j. (0?@$~2,,~QKTEST{500 100)
  (a +/@(*"1 _) b) -: a X b
  0 (9!:58)"0 i.3        NB.  +/ .*  alwasy use blas
  (a +/@(*"1 _) b) -: a X b
  (_1 1000{~(9!:56'pyxes')<IF64) (9!:58)"0 i.3       NB.  +/ .*  never use blas if pyxes
  (a +/@(*"1 _) b) -: a X b

  if. (9!:56'PYXES') do.
  if. (1<{:8&T.'') *. N > 1 T. '' do. 0 T. '' end.  NB. Create another worker thread for next loop
  end.
end.
1
}} ''
delth''
empty thr (9!:58)"0 i.3

epilog''

