NB. rational numbers +. -------------------------------------------------

gcd=: 4 : 0 " 0
 x=. |x
 y=. |y
 while. r=. y|x do.
  x=. y
  y=. r
 end.
)

x=: %/ 0 1x + ?2 100$2e9
y=: %/ 0 1x + ?2 100$2e9

x (+. -: gcd) y
0 (+. -: gcd) y
x (+. -: gcd) 0
x (+. -: gcd) x

x=: (+%)/\90$1x
(i{x) (+. -: gcd) j{x [ i=: ?10$#x [ j=: ?10$#x


4!:55 ;:'gcd i j x y'


