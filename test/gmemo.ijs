prolog './gmemo.ijs'
NB. M. ------------------------------------------------------------------

fib=: 3 : 0 M.
 if. 1>:y do. y else. (fib y-1)+fib y-2 end.
)

fibr=: 3 : 0
 if. 1>:y do. y else. (fibr y-1)+fibr y-2 end.
)

(fib -: fibr)"0 i.15

NB. pn implements recurrence relation by Euler, equation 11 in
NB. http://mathworld.wolfram.com/PartitionFunctionP.html

rec=: 3 : 0
 _1>.y--:k*"1 ] _1 1+/3*k=. 1+i.1+<.%:y*0.6666666
)

pn=: 3 : 0 M.
 if. 0>:y do. 0=y else. -/+/pn"0 rec y end.
)

pnx=: 3 : 0
 if. 0>:y do. 0=y else. -/+/pnx"0 rec y end.
)

(pn -: pnx)"0 i.15

combr=: 4 : 0 M.  NB. All size x combinations of i.y
 if. (x>:y)+.0=x do. i.(x<:y),x else. (0,.x combr&.<: y),1+x combr y-1 end.
)

(comb -: combr)/"1 (<:/"1 t)#t=. ,/>{;~i.9

params =: ( <:@:i.@:(] - <:@:[))
coeffs =: (|.@:>:@:i.@(] - <:@:[))
f =: 4 : 0 M.
if. y < x do. 1
else. >: +/ ((x coeffs y) * x f"0 (x params y))
end.
)
1053389 -: 50 f 168
1148904 -: 10 f 57

r =: 1: M.
hwmk =: 7!:0''
r"0 i. 10000
4!:55<'r'
(hwmk + 2500) > 7!:0''
 
4!:55 ;:'coeffs combr f fib fibr hwmk params pn pnx rec t'



epilog''

