NB. %: ------------------------------------------------------------------

rat =: 128&=@type

(%: 2.5) -: %: 5r2
(%:_2.5) -: %:_5r2
(%: 25 ) -: %: 25r1

(%:_1  ) -: %: _1r1
(%:_0.5) -: %: _1r2

(3 %: 8) -: 3 %: 8r1

rat %: *: 7r2
7r2 -: %: *: 7r2
(%: 3.5) -: %: 7r2
(%:_3.5) -: %:_7r2



NB. %: ------------------------------------------------------------------

NB. Pell Equation 
NB. Integer solutions of 1=(x^2)-N*(y^2), N not a perfect square

NB. continued fraction expansion of %:N
NB. do computations in Z[%:N]

cfsqrt=: 3 : 0  
 N=. y
 p=. 0x
 q=. 1x
 r=. %:N
 assert. r~:<.r  NB. not a perfect square
 m0=. <.q%~p+r
 z=. $0
 while. 1 do.
  m=. <.q%~p+r
  t=. (m*q)-p
  q=. q%~N-*:p-m*q
  p=. t
  if. m=2*m0 do. x: z return. end.
  z=. z, m 
 end.
)

test=: 3 : 0
 N=: y   
 v=: cfsqrt N 
 'xx yy'=: 2 x: (+%)/v
 (xx^2) - N*yy^2
)

*./ 1 _1 e.~ t=. test"0 (-. <.&.:%:) i.100

*./ 1 _1 e.~ t=. test 1+*:1+?1000


4!:55 ;:'cfsqrt N rat t test v xx yy'


