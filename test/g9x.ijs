NB. 9!:0 and 9!:1 -------------------------------------------------------

rlq =: 9!:0
rls =: 9!:1

1 [ rls 7^5
(7^5) -: rlq ''
a =: ?$~100
b =: ?$~100
1 [ rls 7^5
(7^5) -: rlq ''
c =: ?$~100
-. a -: b
a -: c

'domain error' -: rls etx 3.5 
'domain error' -: rls etx 'a' 
'domain error' -: rls etx u:'a' 
'domain error' -: rls etx 10&u:'a' 
'domain error' -: rls etx <9  
'domain error' -: rls etx 3j4 


NB. 9!:2 and 9!:3 -------------------------------------------------------

dispq =: 9!:2
disps =: 9!:3

read  =: 1!:1
erase =: 1!:55
drop1 =: (>:@(i.&(10{a.)) }. ]) @ read
old   =: dispq ''
p     =: <'a1b2q0.x'
q     =: <'a1b2q1.x'

nub   =: (i.@# = i.~) # ]

t=: 0 : 0
disps 1
p 0!:100 'nub'
q 0!:100 '5!:1 <''nub'''
(drop1 p) -: drop1 q
erase p,q

disps 2
p 0!:100 'nub'
q 0!:100 '5!:2 <''nub'''
(drop1 p) -: drop1 q
erase p,q

disps 4
p 0!:100 'nub'
q 0!:100 '5!:4 <''nub'''
(drop1 p) -: drop1 q
erase p,q

disps 5
p 0!:100 'nub'
q 0!:100 '5!:5 <''nub'''
(drop1 p) -: drop1 q
erase p,q

disps old
(,old) -: dispq ''
)

'domain error' -: disps etx 'abc'            
'domain error' -: disps etx 1;2 3            
'domain error' -: disps etx 2 3j4            
'domain error' -: disps etx 2 3.4            
'domain error' -: disps etx 2 3 2            

'rank error'   -: disps etx 1 2$2 3          

'index error'  -: disps etx 2 _1             
'index error'  -: disps etx 2 7              
'index error'  -: disps etx 2 0              


NB. 9!:6 and 9!:7 -------------------------------------------------------

boxq =: 9 !: 6
boxs =: 9 !: 7

old =: boxq ''
(11={:$old) *. *./ old e. a.
'' -: boxs '0123456789?'
'0?29a96?8' -: ,":<'a'
'' -: boxs old

'domain error' -: boxs etx 9;'ab'   
'domain error' -: boxs etx 3j4      
'domain error' -: boxs etx i.11     
'domain error' -: boxs etx o.i.11   

'length error' -: boxs etx 10$' '   
'length error' -: boxs etx 12$' '  

'rank error'   -: boxs etx 2 3 11$'x'


NB. 9!:8 and 9!:9 -------------------------------------------------------

evmq =: 9!:8
evms =: 9!:9

t =: evmq ''
1  -: #$t
32 -: type t
*./ 1 = #&$&>t
*./ (t=a:) +. 2 = type&>t

t =: evmq ''

evms (<'bah humbug') (t i.<'domain error')}t
'bah humbug'   -: ^ etx 'abc'
evms t
'domain error' -: ^ etx 'abc'

'rank error'   -: evms etx 3 4$<'abc'  
'rank error'   -: evms etx <'abc'      
'rank error'   -: evms etx ($t)$<3 4$'a'

'length error' -: evms etx }.t        
'length error' -: evms etx t,<'abc'   

'domain error' -: evms etx ($t)$0 1   
'domain error' -: evms etx ($t)$'abc' 
'domain error' -: evms etx ($t)$2 3   
'domain error' -: evms etx ($t)$2.3   
'domain error' -: evms etx ($t)$2j3  
'domain error' -: evms etx ($t)$<0 1
'domain error' -: evms etx ($t)$<2 3
'domain error' -: evms etx ($t)$<2.3  
'domain error' -: evms etx ($t)$<2j3


NB. 9!:10 and 9!:11 -----------------------------------------------------

ppq =: 9!:10
pps =: 9!:11

a6 =: ":     o.1
a9 =: ":!.9  o.1
a12=: ":!.12 o.1

1 [ pps 9
a9  -: ":     o.1
a6  -: ":!.6  o.1
a9  -: ":!.9  o.1
a12 -: ":!.12 o.1

1 [ pps 12
a12 -: ":     o.1
a6  -: ":!.6  o.1
a9  -: ":!.9  o.1
a12 -: ":!.12 o.1

1 [ pps 6
a6  -: ":     o.1
a6  -: ":!.6  o.1
a9  -: ":!.9  o.1
a12 -: ":!.12 o.1

'domain error' -: pps etx '4'
'domain error' -: pps etx _4
'domain error' -: pps etx <7
'domain error' -: pps etx 3.4
'domain error' -: pps etx 3j4

'rank error'   -: pps etx 7 8

'limit error'  -: pps etx 30


NB. 9!:12 and 9!:14 -----------------------------------------------------

s=:9!:12 ''    NB. system identifier
(0=#$s), s e. _1 0 1 2 3 4 5 6 7

v=:9!:14 ''    NB. J version
(1=#$v), 0=v-.' /-:',Alphanum_j_


NB. 9!:16 and 9!:17 -----------------------------------------------------

'domain error' -: 9!:17 etx 'ab'
'domain error' -: 9!:17 etx 0 1.2
'domain error' -: 9!:17 etx 0j1 2

'length error' -: 9!:17 etx 0 1 2
'length error' -: 9!:17 etx i.0

'rank error'   -: 9!:17 etx ,:1 2
'rank error'   -: 9!:17 etx i.1 1


NB. 9!:18 and 9!:19 -----------------------------------------------------

(2^_44) -: 9!:18 ''
0 = 1 = 1+1e_13
9!:19 [1e_12 
1 = 1 = 1+1e_13
0 = 1 =!.(1e_14) 1+1e_13 
9!:19 [2^_44
0 = 1 = 1+1e_13

'domain error' -: 9!:19 etx 'a'
'domain error' -: 9!:19 etx <1e_12
'domain error' -: 9!:19 etx _1e_13
'domain error' -: 9!:19 etx 1e_8
'domain error' -: 9!:19 etx 14
'domain error' -: 9!:19 etx 14x

'rank error'   -: 9!:19 etx ,1e_14
'rank error'   -: 9!:19 etx 1 1 1$1e_14


NB. 9!:25 ---------------------------------------------------------------

'domain error' -: 9!:25 etx 'a'
'domain error' -: 9!:25 etx 2.5
'domain error' -: 9!:25 etx 2
'domain error' -: 9!:25 etx 2j4
'domain error' -: 9!:25 etx <2

'rank error'   -: 9!:25 etx 1 1 0


NB. 9!:26 and 9!:27 -----------------------------------------------------

'length error' -: 9!:26 etx i.4

'rank error'   -: 9!:26 etx 4
'rank error'   -: 9!:26 etx '4'
'rank error'   -: 9!:26 etx i.0 0

'domain error' -: 9!:27 etx i.12
'domain error' -: 9!:27 etx 3 4.5
'domain error' -: 9!:27 etx 3 4j5
'domain error' -: 9!:27 etx 3 4r5
'domain error' -: 9!:27 etx ;:'es chat o lo gy'

'rank error'   -: 9!:27 etx 3 4$'abc'


NB. 9!:28 and 9!:29 -----------------------------------------------------

'length error' -: 9!:28 etx i.4
'length error' -: 9!:28 etx 'abc'

'rank error'   -: 9!:28 etx 4
'rank error'   -: 9!:28 etx '4'
'rank error'   -: 9!:28 etx i.0 0

'domain error' -: 9!:29 etx 435
'domain error' -: 9!:29 etx 4.5
'domain error' -: 9!:29 etx 4j5
'domain error' -: 9!:29 etx 4r5
'domain error' -: 9!:29 etx <0

'rank error'   -: 9!:29 etx 0 1


NB. 9!:32 and 9!:33 -----------------------------------------------------

'domain error' -: 9!:33 etx _5
'domain error' -: 9!:33 etx 3j4
'domain error' -: 9!:33 etx 'a'
'domain error' -: 9!:33 etx <4.5

'limit error'  -: 9!:33 etx >IF64{1e9;1e16

'rank error'   -: 9!:33 etx 3 4.5


NB. 9!:38 and 9!:39 -----------------------------------------------------

t=: 9!:38 ''
(,2) -: $ t
4 = type t

9!:39 t+1
(t+1) -: 9!:38 ''
9!:39 t
t -: 9!:38 ''

'domain error' -: 9!:39 etx 2 3.4
'domain error' -: 9!:39 etx 2 3j4
'domain error' -: 9!:39 etx 2 3r4
'domain error' -: 9!:39 etx 2 3;4
'domain error' -: 9!:39 etx '23'
'domain error' -: 9!:39 etx u:'23'
'domain error' -: 9!:39 etx 10&u:'23'
'domain error' -: 9!:39 etx _1 3

'rank error'   -: 9!:39 etx 2
'rank error'   -: 9!:39 etx ,:3 2

'limit error'  -: 9!:39 etx 2 1000


NB. 9!:48 and 9!:49 -----------------------------------------------------

old=: 9!:48 ''
9!:49 ]0
'spelling error' -: ex 'x.=: 1'
'spelling error' -: ex 'y.=: 1'
'spelling error' -: ex 'm.=: 1'
'spelling error' -: ex 'n.=: 1'
'spelling error' -: ex 'u.=: +'
'spelling error' -: ex 'v.=: +'
9!:49 old


NB. 9!:50 and 9!:51 -----------------------------------------------------

NB. old=: 9!:50 ''
NB. 
NB. 'length error' -: 9!:50 etx i.4
NB. 
NB. 'rank error'   -: 9!:50 etx 4
NB. 'rank error'   -: 9!:50 etx '4'
NB. 'rank error'   -: 9!:50 etx i.0 0
NB. 
NB. 'domain error' -: 9!:51 etx i.12
NB. 'domain error' -: 9!:51 etx 3 4.5
NB. 'domain error' -: 9!:51 etx 3 4j5
NB. 'domain error' -: 9!:51 etx 3 4r5
NB. 'domain error' -: 9!:51 etx ;:'es chat o lo gy'
NB. 
NB. 'rank error'   -: 9!:51 etx 3 4$'abc'
NB. 
NB. 9!:51 old


4!:55 ;:'a a12 a6 a9 b boxq boxs c dispq disps '
4!:55 ;:'drop1 erase evmq evms nub old p ppq pps promptq '
4!:55 ;:'prompts q read rlq rls s t v '


