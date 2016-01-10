NB. while ---------------------------------------------------------------

fa =. 3 : 0
 :
 z=.1
 a=.x
 n=.y
 while. n do.
  if. 2|n do. z=.z*a end.
  a=.*:a
  n=.<.-:n
 end.
 z
 )

3 (^ = fa) 7
3    (^ = fa)"0 i.7
_3.1 (^ = fa)"0 ?5$100
0    (^ = fa)"0 ?5$50

fb =. 3 : 0
 z=.1
 while. y do.
  y=.<:y
  z =.+:z 
 end. 
)

(2&^ = fb)"0 x=.?10$25

fc =. 3 : 0
 z=.1
 while. 1 do.
  if. 0=y do. break. end.
  y=.<:y
  z =.+:z
 end.
)

(2&^ = fc)"0 i.8

fd =. 3 : 0
 z=.1
 while. 1 do.
  z=.+:z
  y=.<:y
  if. 0<y do. continue. end.
  break. 
 end.
 z
)

(2&^ = fd)"0 x=.>:?10$25

fe =. 3 : 0
 z=.1
 y=.>:y
 while. y=.<:y do.
  z=.+:z
 end.
 z
)

(2&^ = fe)"0 >:i.8

gcd =. 3 : 0    NB. (x+.y)=+/(x,y)*x gcd y
   :
 m=.x,1 0
 n=.y,0 1
 while. {.m do. n=.t [ m=.n-m*<.n %&{. t=.m end.
 }.n
)

(+./ -: [ +/ .* gcd/)"1 x=.?3 10 2$1000


NB. whilst --------------------------------------------------------------

ga =. 3 : 0
 :
 z=.1
 a=.x
 n=.y
 whilst. n do.
  if. 2|n do. z=.z*a end.
  a=.*:a
  n=.<.-:n
 end.
 z
)

3 (^ = ga) 7
*./ 3    (^ = ga)"0 x=.>:i.7
*./ _3.1 (^ = ga)"0 x=.>:?5$100
*./ 0    (^ = ga)"0 x=.>:?5$50

gb =. 3 : 0
 z=.1
 whilst. y do.
  y=.<:y
  z =.+:z 
 end. 
)

(2&^ = gb)"0 x=.>:?10$26

gc =. 3 : 0
 z=.1
 whilst. 1 do.
  if. 0=y do. break. end.
  y=.<:y
  z =.+:z
 end.
)

(2&^ = gc)"0 x=.>:?10$27

gd =. 3 : 0
 z=.1
 whilst. 1 do.
  z=.+:z
  y=.<:y
  if. 0<y do. continue. end.
  break. 
 end.
 z
)

(2&^ = gd)"0 x=.>:?10$27

ge =. 3 : 0
 whilst. z do.
  z=.y
  y=.<:y
 end.
 z
)

0 -: ge 0

4!:55 ;:'fa fb fc fd fe ga gb gc gcd gd ge x'


