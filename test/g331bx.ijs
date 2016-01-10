NB. x f;.n y for boxed x, n e. _2 _1 1 2 --------------------------------

stdx  =: #@$@] {. }.@(1&;)@[
index =: 1 : '({.@(1&{.);.m i.@#)@[ ` 0: @. (''''&-:@[ *. *@#@])'
size  =: 1 : '              #;.m~@[ ` ]  @. (''''&-:@[ *. *@#@])'
cut   =: 2 : '(stdx (n index&.> ,:&>&{ n size&.>) $@]) u;.0 ]'

f=: 1 : 0
  :
 assert. -. 0 e. $y
 c=: ; (i.1+r) <"1@comb&.>r=. #$y
 for_i. i.#c do.
  j=. >i{c
  t=. (j{x) j}r$a:
  assert. t (u;. 1 -: u cut  1) y 
  assert. t (u;._1 -: u cut _1) y
  assert. t (u;. 2 -: u cut  2) y
  assert. t (u;._2 -: u cut _2) y
 end.
 1
)

y=: i. 5 7
x=: 1 0 1 0 0; 1 0 0 0 1 0 1
x < f y
x ] f y
x=: ($y)$&.>0
x < f y
x ] f y

y=: ?(1+?3$25)$1000
x=: ?&.>($y)$&.>2
x < f y
x ] f y
x=: ($y)$&.>0
x < f y
x ] f y

g=: 1 : 0 
  :
 assert. 0 e. $y
 c=: ; (i.1+r) <"1@comb&.>r=. #$y
 for_i. i.#c do.
  j=. >i{c
  t=. (j{x) j}r$a:
  s=. i. >+/&.>t#~(t e.a:)*:*$y
  assert. s -: t u;. 1 y 
  assert. s -: t u;._1 y 
  assert. s -: t u;. 2 y 
  assert. s -: t u;._2 y 
 end.
 1
)

y=: 0 11 25$10
x=: ?&.>($y)$&.>2
x < g y
x ] g y

y=: 14 0 25$10
x=: ?&.>($y)$&.>2
x < g y
x ] g y

y=: 14 11 0$10
x=: ?&.>($y)$&.>2
x < g y
x ] g y

'domain error' -: (1 0 1;'abcd')    <;.1 etx i.3 4
'domain error' -: (1 0 1;2 3 4 1)   <;.1 etx i.3 4
'domain error' -: (1 0 1;<4$<1)     <;.1 etx i.3 4
'domain error' -: (1 0 1;2 3 4 1.2) <;.1 etx i.3 4
'domain error' -: (1 0 1;2 3 4 1j2) <;.1 etx i.3 4
'domain error' -: (1 0 1;1 0 1 1r2) <;.1 etx i.3 4

'length error' -: (1 0  ; 1 0 0 0)  <;.1 etx i.3 4
'length error' -: (1 0 1; 1 0 0  )  <;.1 etx i.3 4
'length error' -: (1 0 1; 1 0 0 0;0)<;.1 etx i.3 4

'rank error'   -: (1 0 1;,:1 0 0 0) <;.1 etx i.3 4


4!:55 ;:'c cut f g index size stdx x y'


