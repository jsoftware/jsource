NB. .. and .: -----------------------------------------------------------

symm =: 2 : 'x -:@:+ x&y'
skew =: 2 : 'x -:@:- x&y'

sin  =: 1&o. 
cos  =: 2&o.
sinh =: 5&o.
cosh =: 6&o.

sin1  =: ^    .: - &. j. 
cos1  =: ^@j. .. -      
sinh1 =: ^    .: -     
cosh1 =: ^    .. -   

sin2  =: ^   skew- &. j.
cos2  =: ^@j.symm-
sinh2 =: ^   skew-
cosh2 =: ^   symm-

x=:(0.01*_700+?20$1400),0,*/\4$0j1

1e_12 > | (sin  - sin1 ) x
1e_12 > | (cos  - cos1 ) x
1e_12 > | (sinh - sinh1) x
1e_12 > | (cosh - cosh1) x

1e_12 > | (sin  - sin2 ) x
1e_12 > | (cos  - cos2 ) x
1e_12 > | (sinh - sinh2) x
1e_12 > | (cosh - cosh2) x


4!:55 ;:'cos cos1 cos2 cosh cosh1 cosh2 sin sin1 sin2 sinh '
4!:55 ;:'sinh1 sinh2 skew symm x '


