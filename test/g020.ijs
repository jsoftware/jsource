prolog './g020.ijs'
NB. >y ------------------------------------------------------------------

rank   =. #@$

mt     =. 0&e.@$
mrk    =. >./@:(rank&>)
crk    =. mrk (-@[{.$&1@[,$@])&.> ]
crank  =. crk ($,)&.> ]
msh    =. >./@:($&>)
cshape =. <@msh {.&.> ]
mtype  =. >./@:(((3!:0)*-.@mt)&>)
fill   =. >@({&(' ';(<$0);0))@(2 32&i.)
ctype  =. (msh <@$ fill@mtype) [^:(mt@])&.> ]
ope    =. > @ cshape @ ctype @ crank

f =. > -: ope

f i.&.>i.7
f 1 2;i.2 3 4
f 'ab';2 3$'wxyz'
f (<<'Now'),<]&.>i.2 3

f '';3 4 5
f 'abc';''
f (<2;3);''

f '';'a';'b';'c'
f 'a';'b';$0
f 3;'';4

f '';($0);0$<''
f (<0 3 4)$&.>' ';(<$0);0

f =. {:@:>@($&0@[ ; ])
4 2 6 1 (f -: {.) i.1 2 4 1
b (f -: {.) i.>:?b=.>:?4$6
b (f -: {.) i.>:?b=.>:?4$6
b (f -: {.) i.>:?b=.>:?4$6

t -: ><"0 t=.?100$20
(4 6$'Cogito,     ergo  sum.  ') -: >;:'Cogito, ergo sum.'

2 1 1 1 -: $ >(i.0 0 0);4
2 1 1 2 -: $ >(i.0 0 2);4
2 1 1 2 -: $ >(i.0 0 2);i.0
2 1 1 3 -: $ >(i.0 0 2);i.3
2 1 0 3 -: $ >(i.0 0 2);i.0 3
2 0 0 3 -: $ >(i.0 0 2);i.0 0 3

x -: > x=: 0 0 $0
x -: > x=: 0 0 $3
x -: > x=: 0 0 $3.4
x -: > x=: 0 0 $3j4
x -: > x=: 0 0 $3x
x -: > x=: 0 0 $3r4
x -: > x=: 0 0 $'a'
x -: > x=: 0 0 $<34

'domain error' -: > etx (<'abc'),<12 
'domain error' -: > etx (<'abc'),<<12
'domain error' -: > etx (<u:'abc'),<12 
'domain error' -: > etx (<u:'abc'),<<12
'domain error' -: > etx (<10&u:'abc'),<12 
'domain error' -: > etx (<10&u:'abc'),<<12
'domain error' -: > etx (s:@<"0&.> <'abc'),<12 
'domain error' -: > etx (<"0@s: <'abc'),<12 
'domain error' -: > etx (s:@<"0&.> <'abc'),<<12
'domain error' -: > etx (<"0@s: <'abc'),<<12
'domain error' -: > etx (<12),<<12  

NB. >!.f y
(2 3 99,:4 5 6) -: >!.99 ] 2 3 ; 4 5 6
'domain error' -: >!.a: etx ] 2 3 ; 4 5 6
(2 3 8,:4 5 6) -: >!.a: ] 2 3 8; 4 5 6  NB.  no fill, no error
(2 3 99,:4 5 6) -: >!.99x ] 2 3 ; 4 5 6
64 -: 3!:0 >!.99x ] 2 3 ; 4 5 6

NB. x>y -----------------------------------------------------------------

0 0 1 0 -: 0 0 1 1 > 0 1 0 1

(14  6#1 0) -: 1>1-10^-i.20
1 >!.0 [1-10^-i.15

NB. symbol
*./, (s:@<"0 }.a.) > s:@<"0 }:a.

'domain error' -: 'abc' > etx 3 4 5
'domain error' -: 'abc' >~etx 3 4 5      
'domain error' -: 'ab'  > etx 'cd'           
'domain error' -: (u:'abc') > etx 3 4 5
'domain error' -: (u:'abc') >~etx 3 4 5      
'domain error' -: (u:'ab')  > etx 'cd'           
'domain error' -: (10&u:'abc') > etx 3 4 5
'domain error' -: (10&u:'abc') >~etx 3 4 5      
'domain error' -: (10&u:'ab')  > etx 'cd'           
'domain error' -: (s:@<"0 'abc') > etx 3 4 5
'domain error' -: (s:@<"0 'abc') >~etx 3 4 5      
'domain error' -: (s:@<"0 'ab')  > etx 'cd'           
'domain error' -: 3     > etx <3 4        
'domain error' -: 3     >~etx <3 4   
'domain error' -: 3.4   > etx 1 2 3j4   
'domain error' -: 3.4   >~etx 1 2 3j4   
'domain error' -: 3j4   > etx 1 2 3j4    
'domain error' -: 'abc' > etx 1 2 3j4   
'domain error' -: 'abc' >~etx 1 2 3j4
'domain error' -: (u:'abc') > etx 1 2 3j4   
'domain error' -: (u:'abc') >~etx 1 2 3j4
'domain error' -: (10&u:'abc') > etx 1 2 3j4   
'domain error' -: (10&u:'abc') >~etx 1 2 3j4
'domain error' -: (s:@<"0 'abc') > etx 1 2 3j4   
'domain error' -: (s:@<"0 'abc') >~etx 1 2 3j4




epilog''

