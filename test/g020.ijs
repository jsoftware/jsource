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
'domain error' -: > etx (<12),<<12  


NB. x>y -----------------------------------------------------------------

0 0 1 0 -: 0 0 1 1 > 0 1 0 1

(14  6#1 0) -: 1>1-10^-i.20
1 >!.0 [1-10^-i.15

'domain error' -: 'abc' > etx 3 4 5
'domain error' -: 'abc' >~etx 3 4 5      
'domain error' -: 'ab'  > etx 'cd'           
'domain error' -: 3     > etx <3 4        
'domain error' -: 3     >~etx <3 4   
'domain error' -: 3.4   > etx 1 2 3j4   
'domain error' -: 3.4   >~etx 1 2 3j4   
'domain error' -: 3j4   > etx 1 2 3j4    
'domain error' -: 'abc' > etx 1 2 3j4   
'domain error' -: 'abc' >~etx 1 2 3j4

4!:55 ;:'b crank crk cshape ctype f fill mrk msh mt '
4!:55 ;:'mtype ope rank t x '


