NB. >:y -----------------------------------------------------------------

(>: -: 1&+) 1=?2 3 4$2
(>: -: 1&+) _1e9+?2 3 4$2e9
(>: -: 1&+) o._1e9+?2 3 4$2e9
(>: -: 1&+) j./?2 3 4$2e9

_1 0 1 2 3  -: >: _2 _1 0 1 2
2147483648  -: >: 2147483647
_2147483647 -: >:_2147483648

t -: [&.>: t=._1e9+?2 3 4$2e9

'domain error' -: >: etx 'abc'    
'domain error' -: >: etx <'abc'     


NB. x>:y ----------------------------------------------------------------

1 0 1 1 -: 0 0 1 1 >: 0 1 0 1

'domain error' -: 'abc' >: etx 3 4 5 
'domain error' -: 'abc' >:~etx 3 4 5  
'domain error' -: 3j4   >: etx 3 4 5  
'domain error' -: 3j4   >:~etx 3 4 5    
'domain error' -: (<34) >: etx 3 4 5      
'domain error' -: (<34) >:~etx 3 4 5  

'length error' -: 3 4     >: etx 5 6 7   
'length error' -: 3 4     >:~etx 5 6 7    
'length error' -: (i.3 4) >: etx i.5 4      
'length error' -: (i.3 4) >:~etx i.5 4  

4!:55 ;:'t'


