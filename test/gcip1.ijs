prolog './gcip1.ijs'
NB. +/ . * ------------------------------------------------------------------

NB. **************************************** matrix product **********************************
delth=: {{ while. 1 T. '' do. 55 T. '' end. 1 }}  NB. delete all worker threads
delth''  NB. make sure we start with an empty system

mm=: 4 : 0
f=. 9!:56'cblasfile'
dgemm=. (f,' dgemm_ + n *c *c *i *i *i *d *d *i *d *i *d *d *i')&cd
k=. ,{.$x
c=. (k,k)$17.2
_2 {:: dgemm (,'N');(,'N');k;k;k;(,2.5-1.5);y;k;x;k;(,1.5-1.5);c;k
)

t1=: 3 : 0
if. (1<1 { 8 T. '') do. {{0 T.0}}^:] 0 >. (1&T.'') -~ 4 <. 9!:56'cores' end.
echo 9!:14''
echo 'cpu ',(9!:56'cpu'),' cores ',": {. 8 T. ''
if. UNAME-:'Linux' do.
  echo 2!:0 ::(''"_) 'lscpu | grep "MHz"'
elseif. UNAME-:'Darwin' do.
  echo 2!:0 ::(''"_) 'sysctl -n machdep.cpu.brand_string'
elseif. UNAME-:'Win' do.
  echo spawn_jtask_ ::(''"_) 'wmic cpu get Name,CurrentClockSpeed'
end.
echo 'threads ', ": 1 T. ''
echo 'OMP_NUM_THREADS=', (''"_)^:(0&-:) (2!:5)'OMP_NUM_THREADS'
echo 'openmp ',":m0=. 9!:56'openmp'
echo 'cblas  ',":c0=. 9!:56'cblas'
echo 'gemm threshold ', ": (9!:58)"0 i.3

N=. IF64{2000 4000
'A B'=. 0?@$~2,,~N
echo '$A= ',":$A

_1 (9!:58)"0 i.3       NB.  +/ .*  never use blas
t1=. 6!:2'c1=. A+/ .*B'
echo ' never  blas ',' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9
0 (9!:58)"0 i.3        NB.  +/ .*  alwasy use blas
if. c0 do. 0(9!:56)'cblas' end.
if. m0 do. 0(9!:56)'openmp' end.
t1=. 6!:2'c2=. A+/ .*B'
echo 'always  blas ',' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9
assert. c1-:c2
if. m0 do.
  1(9!:56)'openmp'
  t1=. 6!:2'c2=. A+/ .*B'
  echo 'openmp  blas ',' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9
  assert. c1-:c2
end.
if. c0 do.
  1(9!:56)'cblas'
  t1=. 6!:2'c2=. A+/ .*B'
  echo 'always cblas ',' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9
  assert. c1-:c2
end.
if. (0;'') -.@e.~ <f=. 9!:56'cblasfile' do.
  t1=. 6!:2'c3=. A mm B'
  echo '      lapack ',' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9
  assert. c1-:c2
end.
)
t1''

delth''  NB. make sure we start with an empty system

4!:55 ;:'delth mm t1'

epilog''

