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

NB. modular determinant
NB. modification of gaussian elimination from
NB. math/misc/linear.ijs

NB. failsafe case; never seen in practice
mdetx=: 4 : 'x|-/ . * x: y'

NB. main adverb
modular_det=: 1 : 0
mult=.* m. m
diff=. - m. m
div=.% m. m
A=. y
r =. #A
i=. 0
p=. 1
while. i<r-1 do.
  k=. 1 i.~ 1=m +. tc=.i}. i{"1 A   NB. need an invertible pivot
  if. r=i+k do.  NB. if so, no invertible pivot in lower column
     if. 0=#tc-.0 do. 0 return. NB. level 1: lower column all 0, det is 0
     end.
     p=. p mult mult/(<0 1)|:(i,i){.A    NB. reduce to the unprocessd block
     B=.(i,i)}.A
     if. 2=#B do.               NB. level 2: do 2 by 2 det
       'a b c d'=.,B
       p mult (a mult d)diff b mult c return.
     end.
     if. 1 e. m +. {.B do.      NB. level 3: look for pivot in  row
       p mult m modular_det_j_ |:B return.
     end.
     if. 1 e. t=.,m +. B do.   NB. level 4: look for a pivot anywhere (won't be in 0th row or col by above levels)
       'I J'=.($B)#: t i. 1
       B=.(<0,I)C. B
       B=.(<0,J) C."1 B
       p mult m modular_det_j_ B return.
     end.
     if. 1 < >./g=.+./B do.     NB. level 5: try to factor gcds out of columns
      p mult (mult/g) mult m modular_det_j_ B %"1 g return.
     end.
     if. 1 < >./g=.+./"1 B do.  NB. level 6: try to factor gcds out of rows
      p mult (mult/g) mult m modular_det_j_ B %"1 0 g return.
     end.
     if. 1 e. ,m +. S=.({. , }. diff"1 {.)B do.  NB. level 7: try row shear
     p mult m modular_det_j_ S  return.
     end.
     if. 1 e. ,m +. S=.({."1 ,. }."1 diff"1 0 {."1)B do.  NB. level 8: try column shear
     p mult m modular_det_j_ S  return.
     end.
     p mult m mdetx_j_ B     return.       NB. level 9: use extended
  end.
NB. end of dealing with no easy pivot
NB. this is where the main work is done and most of the above is usually skipped:
    if. k do.     NB. move a pivot from lower in column
      A=. (<i,i+k) C. A
      p=. p mult _1
    end.
NB. Time to partial pivot
    r1=.>:i
    col=.i{"1 A
    A=.(r1{.A),(r1}.A) diff (r1}.col) mult/ (i{A) div i{col
    i=. >:i
end.
p mult mult/(<0 1)|:A
)

Mdet=:1 : 0
(m modular_det_j_)"2 y
)
