NB. >.y -----------------------------------------------------------------

ir   =. 3!:1

p -: >.p=.?3 4$100
4 =  3!:0 >.p

p =. o.?3 4$100
q =. >.p
q -: >.q
($q) -: $p
(p<:q)*.q<:1+p

p =. 1e20*o.?3 4$100
q =. >.p
q -: >.q
($q) -: $p
(p<:q)*.q<:1+p

_ __ -: >. _ __
(ir -: ir@:>.) _ __ _.

'domain error' -: >. etx 'abc'        
'domain error' -: >. etx <'abc'   


NB. x>.y ----------------------------------------------------------------

5  -: 3 >. 5
_3 -: _3 >. _5

(  i.2 3 3 4)       -: (   i.2 3) >.   i.2 3 3 4
(o.i.2 3 3 4)       -: ( o.i.2 3) >. o.i.2 3 3 4
((<3 4)$&>-  i.2 3) -: (-  i.2 3) >.-  i.2 3 3 4
((<3 4)$&>-o.i.2 3) -: (-o.i.2 3) >.-o.i.2 3 3 4

(3 >. i.10)   -: (0{3 5j7) >. i.10
((i.10) >. 3) -: (i.10) >. 0{3 5j6

'domain error' -: 'abc' >. etx 3 4 5      
'domain error' -: 'abc' >.~etx 3 4 5  
'domain error' -: 3j4   >. etx 3 4 5    
'domain error' -: 3j4   >.~etx 3 4 5        
'domain error' -: (<34) >. etx 3 4 5       
'domain error' -: (<34) >.~etx 3 4 5       

'length error' -: 3 4 >. etx 5 6 7            
'length error' -: 3 4 >.~etx 5 6 7        
'length error' -: (i.3 4) >. etx i.5 4    
'length error' -: (i.3 4) >.~etx i.5 4

4!:55 ;:'ir p q'


