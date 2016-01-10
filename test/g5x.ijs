NB. 5!: -----------------------------------------------------------------

ar =. 5!:1
dr =. 5!:2
tr =. 5!:4
lr =. 5!:5
pr =. 5!:6

'domain error'    -: ar etx 0                           
'domain error'    -: dr etx 0                           
'domain error'    -: tr etx 0
'domain error'    -: lr etx 0                           
'domain error'    -: pr etx 0                           

'domain error'    -: ar etx 't'                         
'domain error'    -: dr etx 't'                         
'domain error'    -: tr etx 't'                         
'domain error'    -: lr etx 't'                          
'domain error'    -: pr etx 't'                          

'domain error'    -: ar etx 34                          
'domain error'    -: dr etx 34                          
'domain error'    -: tr etx 34                          
'domain error'    -: lr etx 34                          
'domain error'    -: pr etx 34                          

'domain error'    -: ar etx 3.4                         
'domain error'    -: dr etx 3.4                         
'domain error'    -: tr etx 3.4                         
'domain error'    -: lr etx 3.4                         
'domain error'    -: pr etx 3.4                         

'domain error'    -: ar etx 3j4                         
'domain error'    -: dr etx 3j4                         
'domain error'    -: tr etx 3j4                         
'domain error'    -: lr etx 3j4
'domain error'    -: pr etx 3j4

'ill-formed name' -: ar etx <''
'ill-formed name' -: dr etx <''
'ill-formed name' -: tr etx <''
'ill-formed name' -: lr etx <''
'ill-formed name' -: pr etx <''

'ill-formed name' -: ar etx <'-'
'ill-formed name' -: dr etx <'-'
'ill-formed name' -: tr etx <'-'
'ill-formed name' -: lr etx <'-'
'ill-formed name' -: pr etx <'-'

'ill-formed name' -: ar etx <'a b c'
'ill-formed name' -: dr etx <'a b c'
'ill-formed name' -: tr etx <'a b c'
'ill-formed name' -: lr etx <'a b c'
'ill-formed name' -: pr etx <'a b c'

'value error'     -: ar etx <'asdfasdf'
'value error'     -: dr etx <'asdfasdf'
'value error'     -: tr etx <'asdfasdf'
'value error'     -: lr etx <'asdfasdf'
'value error'     -: pr etx <'asdfasdf'

4!:55 ;:'ar dr pr lr tr'


