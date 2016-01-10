NB. ?.y -----------------------------------------------------------------

seed =. 9!:0 ''
x=.30$1e6
a -: b [ a=.?.x [ b=.?.x
seed -: 9!:0 ''

seed =. 9!:0 ''
(s$?.1e6) -: ?."0 s$1e6 [ s=.5 6 7
seed -: 9!:0 ''

'domain error' -: ?. etx 0 _1                          
'domain error' -: ?. etx 'abc'                         
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

4!:55 ;:'a b s seed x'


