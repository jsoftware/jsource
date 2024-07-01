prolog './gcip1.ijs'
NB. +/ . * ------------------------------------------------------------------

NB. **************************************** matrix product **********************************
delth =: {{ while. 1 T. '' do. 55 T. '' end. 1 }}  NB. delete all worker threads
delth''  NB. make sure we start with an empty system
empty {{0 T.0}}^:] 0 >. (1&T.'') -~  <: <./ 8&T.''

3 : 0''
if. 0=4!:0<'liblapack' do. '' return. end.
if. (<UNAME)e.'Linux';'FreeBSD';'OpenBSD' do.
  liblapack=: 'liblapack.so.3'
elseif. UNAME-:'Darwin' do.
    liblapack=: '/System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/vecLib'
    if. -.fexist liblapack do.
      liblapack=: '/System/Library/Frameworks/vecLib.framework/vecLib'
    end.
elseif. UNAME-:'Win' do.
 liblapack=: dquote jpath '~addons/math/lapack/jlapack',(IF64#'64'),'.dll'
elseif. do.
 'not supported' assert 0
end.
dgemm=: (liblapack,' dgemm_ n *c *c *i *i *i *d *d *i *d *i *d *d *i')&cd
EMPTY
)

mm=: 4 : 0
k=: ,{.$x
c=: (k,k)$17.2
 _2 {:: dgemm (,'N');(,'N');k;k;k;(,2.5-1.5);y;k;x;k;(,1.5-1.5);c;k 
)


N=: 2044 [ 4088
N=: IF64{20,N

'A B'=:0?@$~2,,~N
 
thr=: (9!:58)"0 i.3

NB. system default blas
t1=: 6!:2'c1=: A+/ .*B'
echo ' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9

0 (9!:58)"0 i.3        NB.  +/ .*  alwasy use blas
t1=: 6!:2'c2=: A+/ .*B'
echo ' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9

_1 (9!:58)"0 i.3       NB.  +/ .*  never use blas
t1=: 6!:2'c3=: A+/ .*B'
echo ' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9

empty thr (9!:58)"0 i.3

t1=: 6!:2'c4=: A mm B'
echo ' GFlop ',~ 0j3": 2*(N^3)%(t1)*1e9

echo (c1-:c2),(c1-:c3),(c1-:c4),(c2-:c3),(c2-:c4),(c3-:c4)

c1-:c2
c1-:c3
c1-:c4

delth''  NB. make sure we start with an empty system

4!:55 ;:'c1 c2 c3 c4 delth dgemm liblapack t1 thr'

epilog''

