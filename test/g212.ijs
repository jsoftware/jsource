prolog './g212.ijs'
NB. $: ------------------------------------------------------------------

(!i.10) -: 1:`(* $:@<:)@.*"0 i.10
12 -: 2&$: : * 6
(^5) -: 12 ^ : $:@- 7

f=: 0&$: : (4 : 'x+y')

x -: f x=: _50+?2 3 4$100
x -: f x=: j./_50+?2 3 4$100
(x+y) -: x f y=: j./_50+2 3 4$100

add=:(4 : 0)"0  NB. Verify that $: is pushed over parser call
x+y
)
p=:10&$: : (4 : 0) " 0
x add y
)
12 13 -: p 2 3

ln=: 40&$: : (4 : 0) " 0
 assert. 0<y
 m=. <.0.5+2^.y
 t=. (<:%>:) (x:!.0 y)%2x^m
 if. x<-:#":t do. t=. (1+x) round t end.
 ln2=. 2*+/1r3 (^%]) 1+2*i.>.0.5*(%3)^.0.5*0.1^x+>.10^.1>.m
 lnr=. 2*+/t   (^%]) 1+2*i.>.0.5*(|t)^.0.5*0.1^x
 lnr + m * ln2
)

0.69314718055994530942 1.09861228866810969140 -: ln 2 3

dv =: {{
    if. 1 < N=: N+1 do. >: ' ' end.
    x dv"0 y
}}
'domain error' -: 0 dv etx '.';'.' [ N =: 0  NB. Memory was corrupted because the error caused the recursive call not to free symbols
'domain error' -: 0 dv etx '.';'.' [ N =: 0
'domain error' -: 0 dv etx '.';'.' [ N =: 0


4!:55 ;:'add dv f ln N p x y'



epilog''

