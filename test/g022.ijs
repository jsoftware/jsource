prolog './g022.ijs'
NB. >:y -----------------------------------------------------------------

1 2 -: >:@| 0 1   NB. smoke test, 32-bit arm crash on unaligned access of 8-byte double

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
'domain error' -: >: etx u:'abc'    
'domain error' -: >: etx <u:'abc'     
'domain error' -: >: etx 10&u:'abc'    
'domain error' -: >: etx <10&u:'abc'     


NB. x>:y ----------------------------------------------------------------

1 0 1 1 -: 0 0 1 1 >: 0 1 0 1

NB. symbol
1 1 0 1 1 0 -: (s:@<"0 'cbaCBA') >: s:@<"0 'abcABC'

'domain error' -: 'abc' >: etx 3 4 5 
'domain error' -: 'abc' >:~etx 3 4 5  
'domain error' -: (u:'abc') >: etx 3 4 5 
'domain error' -: (u:'abc') >:~etx 3 4 5  
'domain error' -: (10&u:'abc') >: etx 3 4 5 
'domain error' -: (10&u:'abc') >:~etx 3 4 5  
'domain error' -: 3j4   >: etx 3 4 5  
'domain error' -: 3j4   >:~etx 3 4 5    
'domain error' -: (<34) >: etx 3 4 5      
'domain error' -: (<34) >:~etx 3 4 5  

'length error' -: 3 4     >: etx 5 6 7   
'length error' -: 3 4     >:~etx 5 6 7    
'length error' -: (i.3 4) >: etx i.5 4      
'length error' -: (i.3 4) >:~etx i.5 4  

NB. Verify all comparisons work for arguments of any length, including exact comparisons
t1 =. 1&<.&.(0.05&+)&.> <@((?@$)&0)"1 (3+i.3) ,"0/ 1+i.50
t2 =. 1&<.&.(0.05&+)&.> <@((?@$)&0)"1 $@> t1
f =. 1 : 0
:
assert. x (u"+"+ -: u)&> y
assert. ({."1&.> x) (u"+"+ -: u)&> y
assert. x (u"+"+ -: u)&> ({."1&.> y)
assert. x ((u f.)!.0"+"+ -: (u f.)!.0)&> y
assert. ({."1&.> x) ((u f.)!.0"+"+ -: (u f.)!.0)&> y
assert. x ((u f.)!.0"+"+ -: (u f.)!.0)&> ({."1&.> y)
)
t1 >: f t2
t1 > f t2
t1 <: f t2
t1 < f t2
t1 = f t2
t1 ~: f t2

t1 =. 1e4&<.&.(500&+)&.> <@((?@$)&1e4)"1 (3+i.3) ,"0/ 1+i.50
t2 =. 1e4&<.&.(500&+)&.> <@((?@$)&1e4)"1 $@> t1
f =. 1 : 0
:
assert. x (u"+"+ -: u)&> y
assert. ({."1&.> x) (u"+"+ -: u)&> y
assert. x (u"+"+ -: u)&> ({."1&.> y)
)
t1 >: f t2
t1 > f t2
t1 <: f t2
t1 < f t2
t1 = f t2
t1 ~: f t2

4!:55 ;:'t t1 t2 f'



epilog''

