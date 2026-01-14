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
if. GITHUBCI*.('ARM64'-.@-:2!:5'RUNNER_ARCH')*.'arm64'-:(9!:56'cpu') do.
  '' return.  NB. no real hardware fma
end.
if. (1<1 { 8 T. '') do. {{0 T.0}}^:] 0 >. (1&T.'') -~ 4 <. 9!:56'cores' end.
techo^:PRINTMSG 9!:14''
techo^:PRINTMSG 'cpu ',(9!:56'cpu'),' cores ',": {. 8 T. ''
if. UNAME-:'Darwin' do.
  techo^:PRINTMSG 2!:0 ::(''"_) 'sysctl -n machdep.cpu.brand_string'
elseif. UNAME-:'Win' do.
  techo^:PRINTMSG spawn_jtask_ ::(''"_) 'wmic cpu get Name,CurrentClockSpeed'
elseif. do.
  techo^:PRINTMSG 2!:0 ::(''"_) 'lscpu | grep "MHz"'
end.
techo^:PRINTMSG 'threads ', ": 1 T. ''
techo^:PRINTMSG 'OMP_NUM_THREADS=', (''"_)^:(0&-:) (2!:5)'OMP_NUM_THREADS'
techo^:PRINTMSG 'openmp ',":m0=. 9!:56'openmp'
techo^:PRINTMSG 'cblas  ',":c0=. 9!:56'cblas'
techo^:PRINTMSG 'cblasfile ',":9!:56'cblasfile'
techo^:PRINTMSG 'gemm threshold ', ": (9!:58)"0 i.3

N=. IF64{QKTEST{::2000 4000;500 800
'A B'=. 0?@$~2,,~N
techo^:PRINTMSG '$A= ',":$A

_1 (9!:58)"0 i.3       NB.  +/ .*  never use blas
t1=. 6!:2'c1=. A+/ .*B'
techo^:PRINTMSG ' never  blas ',' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9
0 (9!:58)"0 i.3        NB.  +/ .*  alwasy use blas
if. c0 do. 0(9!:56)'cblas' end.
if. m0 do. 0(9!:56)'openmp' end.
t1=. 6!:2'c2=. A+/ .*B'
techo^:PRINTMSG 'always  blas ',' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9
assert. c1-:c2
if. m0 do.
  1(9!:56)'openmp'
  t1=. 6!:2'c2=. A+/ .*B'
  techo^:PRINTMSG 'openmp  blas ',' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9
  assert. c1-:c2
end.
if. c0 do.
  1(9!:56)'cblas'
  t1=. 6!:2'c2=. A+/ .*B'
  techo^:PRINTMSG 'always cblas ',' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9
  assert. c1-:c2
end.
if. (0;'') -.@e.~ <f=. 9!:56'cblasfile' do.
  t1=. 6!:2'c3=. A mm B'
  techo^:PRINTMSG '      lapack ',' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9
  assert. c1-:c2
end.
)
t1''

delth''  NB. make sure we start with an empty system


epilog''

