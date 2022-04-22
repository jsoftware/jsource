1:@:(dbr bind Debug)@:(9!:19)2^_44[(prolog [ echo^:ECHOFILENAME) './gtdot.ijs'

NB. j904 64-bit only
NB. T. t. ------------------------------------------------------------------

N0=: N=: <: 9!:56'maxtasks'
N > 0

NB. create all available threads
1: 0&T."1^:(0 < #) ''$~ (0 >. N-1 T. ''),0
N0 = 1 T.''
N0 = # 2 T.''

NB. no more thread can be created
'limit error' -: 0&T. etx ''

f=: 3 : 0
pyx=. g t.'' "0 i.N
while. do.
 r=. 4 T. pyx               NB. echo r for debug
 if. *./0>r do. break. end.
 6!:3[0.1
end.
assert. r e. _1000 _1001    NB. _1001 if run on the main thread
]&> pyx
)

f1=: 3 : 0
pyx=. g1 t.'' "0 i.%:N1
while. do.
 r=. 4 T. pyx               NB. echo r for debug
 if. *./0>r do. break. end.
 6!:3[0.1
end.
assert. r e. _1000 _1001    NB. _1001 if run on the main thread
]&> pyx
)

f2=: 3 : 0
pyx=. g1 t.'' "0 i.%:N1
pyx0=. g t.'' "0 i.%:N1
while. do.
 r=. 4 T. pyx,pyx0               NB. echo r for debug
 if. *./0>r do. break. end.
 6!:3[0.1
end.
assert. r e. _1000 _1001    NB. _1001 if run on the main thread
]&> pyx,pyx0
)

g=: 3 : 0
9!:1[(7^5)+3 T.''  NB. random per thread
6!:3[?0            NB. arbitrary delay
1
)

g1=: 3 : 0
9!:1[(7^5)-3 T.''  NB. random per thread
for_i. i.%:N1 do.
 6!:3[?0
 pyx=. g t.'' "0 i.%:N1
end.
]&> pyx
)

f''

N0 = 1 T.''
N0 = # 2 T.''

N1=:  <.&.%: N

f1^:(N1>0) ''

N0 = 1 T.''
N0 = # 2 T.''

NB. run in master thread

N=: <. 1.5 * >: 9!:56'maxtasks'

f''

N0 = 1 T.''
N0 = # 2 T.''

N=: 2 * >: 9!:56'maxtasks'
N1=:  <.&.%: N

f''

N0 = 1 T.''
N0 = # 2 T.''

f1^:(N1>0) ''

N0 = 1 T.''
N0 = # 2 T.''

N=: <: 9!:56'maxtasks'
N1=:  <.&.%: N

f2^:(N1>0) ''

N0 = 1 T.''
N0 = # 2 T.''

4!:55 ;:'N N0 N1 f f1 f2 g g1 '
