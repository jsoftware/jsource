NB. byte parallel -------------------------------------------------------
NB. m-bytes-at-a-time operations on boolean functions

A=: 1 : 0
:
 g=: 4 : ('x ',(5!:5 <'u'),' y')
 xx=: x ?@$ 2
 yy=: (x,c=. y) ?@$ 2
 assert. (xx u yy) -: (c#"0 xx) u yy
 assert. (xx u yy) -: xx g"0 1 yy
 assert. (yy u xx) -: yy u c#"0 xx
 assert. (yy u xx) -: yy g"1 0 xx
 1
)

s=: 17
s =  A"1 0 >:i.25
s <  A"1 0 >:i.25
s <: A"1 0 >:i.25
s >  A"1 0 >:i.25
s >: A"1 0 >:i.25
s +. A"1 0 >:i.25
s +: A"1 0 >:i.25
s *. A"1 0 >:i.25
s *: A"1 0 >:i.25
s ~: A"1 0 >:i.25

I=: 1 : 0
:
 b=: (x,y) ?@$ 2
 assert. (u/   -: u/"1&.|:   ) b
 assert. (u/"2 -: u/"1&.|:"2 ) b
 assert. (u/"1 -: {.@(u/\.)"1) b
 assert. (u/"1 -: {:@(u/\ )"1) b
 1
)

s=: 5 11
s =  I"1 0 >:i.25
s <  I"1 0 >:i.25
s <: I"1 0 >:i.25
s >  I"1 0 >:i.25
s >: I"1 0 >:i.25
s +. I"1 0 >:i.25
s +: I"1 0 >:i.25
s *. I"1 0 >:i.25
s *: I"1 0 >:i.25
s ~: I"1 0 >:i.25

f=: 4 : 0
 b=: (x,y) ?@$ 2
 assert. (~:/"1 b) -: 2|+/"1 b
 assert. (= /"1 b) -: ~:/&.:-."1 b
 assert. (+ /"1 b) -: +/"1 b+0
 1
)

17  f"1 0 ]1  +i.25
2 5 f"1 0 ]1  +i.25
''  f"1 0 ]256+i.25

P=: 1 : 0
:
 b=: (x,y) ?@$ 2
 assert. (u/\   -: u/\"1&.|:  ) b
 assert. (u/\"2 -: u/\"1&.|:"2) b
 1
)

s=: 5 11
s =  P"1 0 >:i.17
s <  P"1 0 >:i.17
s <: P"1 0 >:i.17
s >  P"1 0 >:i.17
s >: P"1 0 >:i.17
s +. P"1 0 >:i.17
s +: P"1 0 >:i.17
s *. P"1 0 >:i.17
s *: P"1 0 >:i.17
s ~: P"1 0 >:i.17

S=: 1 : 0
:
 b=: (x,y) ?@$ 2
 assert. (u/\.   -: u/\."1&.|:  ) b
 assert. (u/\."2 -: u/\."1&.|:"2) b
 1
)

s=: 5 11
s =  S"1 0 >:i.17
s <  S"1 0 >:i.17
s <: S"1 0 >:i.17
s >  S"1 0 >:i.17
s >: S"1 0 >:i.17
s +. S"1 0 >:i.17
s +: S"1 0 >:i.17
s *. S"1 0 >:i.17
s *: S"1 0 >:i.17
s ~: S"1 0 >:i.17


4!:55 ;:'A b c f g I P S s xx yy'
 