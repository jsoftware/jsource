prolog './g641.ijs'
NB. ?.y -----------------------------------------------------------------

seed =. 9!:0 ''
x=.30$1e6
a -: b [ a=.?.x [ b=.?.x
seed -: 9!:0 ''
x=.30$1000000x
a -: b [ a=.?.x [ b=.?.x

seed =. 9!:0 ''
(s$?.1e6) -: ?."0 s$1e6 [ s=.5 6 7
seed -: 9!:0 ''

NB.  0 1 0 6 7 4 1 52 162 406 942 1905 -: ?. 2x ^ i. 12 NB. tentative

9!:43 ] 2
'domain error' -: ?. etx _1
2 -: 9!:42 ''
'domain error' -: 5 ?. etx _1
2 -: 9!:42 ''
1: ?. 10$10
2 -: 9!:42 ''

'domain error' -: ?. etx 0 _1                          
'domain error' -: ?. etx 'abc'                         
'domain error' -: ?. etx u:'abc'                         
'domain error' -: ?. etx 10&u:'abc'                         
'domain error' -: ?. etx 2 3 _4                        
'domain error' -: ?. etx 2 3.4 5                       
'domain error' -: ?. etx 2 3j4 5                       
'domain error' -: ?. etx 2 3;4 5                       

'domain error' -: ?.@(*/) etx 'abc'                       


NB. x?.y ----------------------------------------------------------------

seed=. 9!:0 ''
a -: b [ a=.?.~100 [ b=.?.~100
seed -: 9!:0 ''

seed=. 9!:0 ''
((s,16)$16?.17) -: 16 ?. s$17 [ s=.3 4
((s,17)$17?.17) -: 17 ?. s$17 [ s=.3 4
seed -: 9!:0 ''

'domain error' -: 2 ?. etx 1                           
'domain error' -: 4 ?. etx 0                           
'domain error' -: 4 ?. etx 3                           
'domain error' -: 3 ?. etx 'abc'                       
'domain error' -: 3 ?.~etx 'abc'                       
'domain error' -: 1 ?. etx 2 3 _4                      
'domain error' -: 5 ?.~etx 2 3 _4                      
'domain error' -: 2 ?. etx 2 3.4 5                     
'domain error' -: 5 ?.~etx 2 3.4 5                     
'domain error' -: 2 ?. etx 2 3j4 5                     
'domain error' -: 5 ?.~etx 2 3j4 5                     
'domain error' -: 2 ?. etx 2 3;4 5                     
'domain error' -: 2 ?.~etx 2 3;4 5

'domain error' -: ?.~@(*/) etx 'abc'

'length error' -: 2 3 ?. etx 4 5 6

270869761 1880044261 -: q: :: q: 509247139646491621x   NB. Twice to elude nonce error

4!:55 ;:'a b s seed x'



epilog''

