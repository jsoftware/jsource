prolog './gcip1.ijs'
NB. +/ . * ------------------------------------------------------------------

NB. **************************************** matrix product **********************************
delth =: {{ while. 1 T. '' do. 55 T. '' end. 1 }}  NB. delete all worker threads
delth''  NB. make sure we start with an empty system

t1=: 3 : 0
if. (1<1 { 8 T. '') do. {{0 T.0}}^:] 0 >. (1&T.'') -~ 4 <. 9!:56'cores' end.
echo 9!:14''
echo 'threads ', ": 1 T. ''
echo 'OMP_NUM_THREADS=', (''"_)^:(0&-:) (2!:5)'OMP_NUM_THREADS'
echo 'openmp ',":9!:56'openmp'
echo 'cblas  ',":c0=. 9!:56'cblas'
echo 'gemm threshold ', ": (9!:58)"0 i.3 

N=. 2044 [ 4088
N=. IF64{20,N
'A B'=. 0?@$~2,,~N

_1 (9!:58)"0 i.3       NB.  +/ .*  never use blas
t1=. 6!:2'c1=. A+/ .*B'
echo ' never  blas ',' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9
0 (9!:58)"0 i.3        NB.  +/ .*  alwasy use blas
if. c0 do. 0(9!:56)'cblas' end.
t1=. 6!:2'c2=. A+/ .*B'
echo 'always  blas ',' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9
assert. c1-:c2
if. c0 do.
1(9!:56)'cblas'
t1=. 6!:2'c2=. A+/ .*B'
echo 'always cblas ',' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9
assert. c1-:c2
end.
)
t1''

delth''  NB. make sure we start with an empty system

4!:55 ;:'delth t1'

epilog''

