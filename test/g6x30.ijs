NB. 1:@:(9!:19)2^_44[(echo^:ECHOFILENAME) './g6x30.ijs'
NB. 6!:30 ----------------------------------------------------------------

tsl=: 6!:30
tsu=: 6!:31
tsp=: 6!:32

t=: tsl ''
(,6) = $t
(-: <.) 5{.t
*./ 0 <: t
1990 <: 0{t
*./ (1&<: *. <:&12 31) 1 2{t
24 60 60 *./ . > 3 4 5{t

t=: tsu ''
(,6) = $t
(-: <.) 5{.t
*./ 0 <: t
1990 <: 0{t
*./ (1&<: *. <:&12 31) 1 2{t
24 60 60 *./ . > 3 4 5{t

t=: tsl x=: '%Y-%m-%d %H:%M:%S'
($t) -: 19
t -:&(3!:0) x
(2{.t) -: '20'

t=: tsu x=: '%Y-%m-%d %H:%M:%S'
($t) -: 19
t -:&(3!:0) x
(2{.t) -: '20'

'domain error' -: tsl etx 3 4 5
'domain error' -: tsl etx <'%Y-%m-%d %H:%M:%S'
'domain error' -: tsu etx 3 4 5
'domain error' -: tsu etx <'%Y-%m-%d %H:%M:%S'

'rank error'   -: tsl etx ,: '%Y-%m-%d %H:%M:%S'
'rank error'   -: tsu etx ,: '%Y-%m-%d %H:%M:%S'

1 [ 6!:3 (55&< * 60&-) 5{tsl '' NB. delay til a new minute if close to it
(}.;'-'&,@}.@":&.>10000 100 100+3{.tsl '') -: tsl '%Y-%m-%d'
(tsl -: tsl@u:) '%Y-%m-%d'
(tsl -: tsl@u:) '%m/%d/%y %H:%M'
(tsl -: tsl@(10&u:)) '%Y-%m-%d'
(tsl -: tsl@(10&u:)) '%m/%d/%y %H:%M'
(":{.tsl '') -: tsl    '%Y'
(_2{.":{.tsl '') -: ts. '%y'

1 [ 6!:3 (55&< * 60&-) 5{tsu '' NB. delay til a new minute if close to it
(}.;'-'&,@}.@":&.>10000 100 100+3{.tsu '') -: tsu '%Y-%m-%d'
(tsu -: tsu@u:) '%Y-%m-%d'
(tsu -: tsu@u:) '%m/%d/%y %H:%M'
(tsu -: tsu@(10&u:)) '%Y-%m-%d'
(tsu -: tsu@(10&u:)) '%m/%d/%y %H:%M'
(":{.tsu '') -: tsu    '%Y'
(_2{.":{.tsu '') -: ts. '%y'

4!:55 ;:'t tsl tsu tsp x'
