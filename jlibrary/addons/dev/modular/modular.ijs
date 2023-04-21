NB. Modulo matrix divide
NB. Modular gauss jordan is based upon the
NB. math/misc/linear.ijs
NB. modified from math/misc/matutil
NB. m Md is meant to be like %. mod m
cocurrent 'j'  NB. Define these names in j locale
modular_gauss_jordan=: 1 : 0
A=. y
'r c'=. $A
rws=. i.r
i=. j=. 0
while. (i<r) *. j<c do.
  k=. 1 i.~ 1=m +. i}. j{"1 A   NB. need an invertible pivot
  if. k do.
    if. r=i+k do. 'singular matrix' 13!:8 (3) end.
    A=. (<i,i+k) C. A
  end.
  A=. (i,j)m modular_pivot_j_ A
  i=. >:i
  j=. >:j
end.
A
)

modular_pivot=: 1 : 0
:
'r c'=. x
col=. c{"1 y
y - m. m (col-r=i.#y) * m. m/ (r{y) % m. m r{col
)

NB. modular inverse square monad
Md0=: 1 : 0
(r*1 _1){.m modular_gauss_jordan_j_ y,.=i.r=. #y
)

NB. #rows>:#cols; ambivalent
NB. Md is meant to be like %. mod m
Mdrank=: 1 : 0
mp=. + m. m/ . (* m. m)
if. </$y do.  'singular matrix' 13!:8 (3) end.
if. >/$y
do. yty=. (yt=. |:y)mp y
  (m Md0_j_ yty)mp yt
else. m Md0_j_ y
end.
:
mp=. + m. m/ . (* m. m)
(m Md_j_ y) mp x
)
Md=:1 : 0  NB. The result of (n Md) must be bivalent inside JE.  Explicit definitions are.
(m Mdrank_j_)"2 _ 2 y
:
x (m Mdrank_j_)"2 _ 2 y
)

try=: 0 : 0
ts=:6!:2 ,7!:2@]

mmp=:1 : '+ m. m/ . (* m. m)'

NB. tests
__ q: | -/ . * x: c=:?.30 30$5
M=:103*101
10 10{. ci=:%. m. M c
5 5{.ci M mmp c

__ q: | -/ . * (|: M mmp ])x: c2=:?.40 30$5
5 5{. c2i=:%. m. M c2
5 5{.c2i M mmp c2
6!:2 '(i.40 2) %. m. M c2'
NB. errors:
%. m. M d2=:?.30 40$5
%. m. M i. 3 3

)
