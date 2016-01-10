NB. 6!:0 ----------------------------------------------------------------

ts=: 6!:0

t=: ts ''
(,6) = $t
(-: <.) 5{.t
*./ 0 <: t
1990 <: 0{t
*./ (1&<: *. <:&12 31) 1 2{t
24 60 60 *./ . > 3 4 5{t

t=: ts x=: 'YYYY-MM-DD hh:mm:ss.sss'
t -:&$ x
t -:&(3!:0) x
t -:&((-.x e. 'YMDhms')&#) x

'domain error' -: ts etx 3 4 5
'domain error' -: ts etx <'YYYY-MM-DD hh:mm:ss.sss'

'rank error'   -: ts etx ,: 'YYYY-MM-DD hh:mm:ss.sss'

1 [ 6!:3 (55&< * 60&-) 5{ts '' NB. delay til a new minute if close to it
(}.;'-'&,@}.@":&.>10000 100 100+3{.ts '') -: ts 'YYYY-MM-DD'
(ts -: ts@u:) 'YYYY-MM-DD'
(ts -: ts@u:) 'MM/DD/YY hh:mm'
(":{.ts '') -: ts    'YYYY'
(_2{.":{.ts '') -: ts 'YY'


4!:55 ;:'t ts x'
