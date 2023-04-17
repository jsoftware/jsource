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
  A=. (i,j)m modular_pivot A
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
(r*1 _1){.m modular_gauss_jordan y,.=i.r=. #y
)

NB. #rows>:#cols; ambivalent
NB. Md is meant to be like %. mod m
Md=: 1 : 0
mp=. + m. m/ . (* m. m)
if. >/$y
do. yty=. (yt=. |:y)mp y
  (m Md0 yty)mp yt
else. m Md0 y
end.
:
mp=. + m. m/ . (* m. m)
(m Md y) mp x
)


try=: 0 : 0

-/ . * a=:?.3 3$5
a
]ai=: 17 Md a
ai + m. 17/ . (* m. 17) a
1 2 3 (17) Md a

NB. Non square case
$A=:a,1
17 Md A
17|(17 Md A)mp A
b=:1 2 3 4
s=:b 17 Md A
17| (B=:17|b-A mp s)  mp A
B

mp=.1 : '+ m. m/ . (* m. m)'
det=: 1 : '(- m. m)/ . (* m. m)'

NB. bigger tests
__ q: | -/ . * x: c=:?.30 30$5
M=.103*101
10 10{. ci=:M Md c
ci M mp c

__ q: | -/ . * (|: M mp ])x: c=:?.40 30$5
M=.103*101
10 10{. ci=:M Md c
ci M mp c

(i.40 2) M Md c

)