1:@:(dbr bind Debug)@:(9!:19)2^_44[(prolog [ echo^:ECHOFILENAME) './gtdot.ijs'
NB. T. t. ------------------------------------------------------------------

NB. **************************************** threads & tasks **********************************
NB. j904 64-bit only

N0=: N=: <: 9!:56'maxtasks'
N > 0

NB. create all available threads
1: 0&T."1^:(0 < #) ''$~ (0 >. N-1 T. ''),0
N0 = 1 T.''
N0 = {. 2 T.''  NB. all should be waiting

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
(,]&> pyx), ]&>pyx0
)

g=: 3 : 0
9!:1[(7^5)+3 T.''  NB. random per thread
6!:3[0.3*?0            NB. arbitrary delay
1
)

g1=: 3 : 0
9!:1[(7^5)-3 T.''  NB. random per thread
for_i. i.%:N1 do.
 6!:3[0.3*?0
 pyx=. g t.'' "0 i.%:N1
end.
]&> pyx
)

f''

N0 = 1 T.''
N0 = {. 2 T.''  NB. all should be waiting

N1=:  <.&.%: N

f1^:(N1>0) ''

N0 = 1 T.''
N0 = {. 2 T.''  NB. all should be waiting

NB. run in master thread

N=: <. 1.5 * >: 9!:56'maxtasks'

f''

N0 = 1 T.''
N0 = {. 2 T.''  NB. all should be waiting

N=: 2 * >: 9!:56'maxtasks'
N1=:  <.&.%: N

f''

N0 = 1 T.''
N0 = {. 2 T.''  NB. all should be waiting

f1^:(N1>0) ''

N0 = 1 T.''
N0 = {. 2 T.''  NB. all should be waiting

N=: <: 9!:56'maxtasks'
N1=:  <.&.%: N

f2^:(N1>0) ''

N0 = 1 T.''
N0 = {. 2 T.''  NB. all should be waiting

NB. mutex

g =: 3 : 0  NB. semaphore task
'remotepyx remote_stuff'=.y
assert. remote_stuff=1
6 T. remotepyx,<(localpyx =. 5 T. 2);<2
'remotepyx remote_stuff'=.>localpyx
assert. remote_stuff=3
6 T. remotepyx,<(localpyx =. 5 T. 2);<4
4!:55 <'localpyx'
1
)

f =: 3 : 0
try.
while. 2 > 1 T. '' do. 0 T. '' end.  NB. make sure we have 2 worker threads
catch.
assert. -. IF64  NB. threads should be supported on all 64-bit systems
end.
if. IF64 do.
assert. (<@i."0 i. 5) -: (i. t. ''"0 i. 5)
assert. 2 <: 1 T. ''
nwthreads=. 1 T. ''
while. nwthreads ~: {. 2 T. '' do. end.  NB. wait till threads become ready
assert. (>: i. nwthreads) *./@e. > (3&T.@'')@(6!:3) t.'' "(0)  (nwthreads # 0.1), 0.3 
assert. (>: i. nwthreads) *./@e. > (3&T.@'')@(6!:3) t.'' "(0)  (nwthreads # 0.3), 0.1 
assert. (ccc__   =: ((<_1000) #~ <: nwthreads),(>: i. nwthreads);_1001) e.~&> bbb__   =: 4 T. aaa__   =: (3&T.@'')@(6!:3) t.'' "(0) (0.3 #~ <: nwthreads), 0.6 0.4  NB. last thread should run in master; earlier ones complete first
while. nwthreads ~: {. 2 T. '' do. end.  NB. wait till threads become ready
NB. Verify forcetask arg
pyx =. 6!:3 t. 0"0 N0 # 1.0  NB. fill up with delating threads
t0 =. 6!:1''
1. = >pyx
assert. (t0 + 0.5) < 6!:1''  NB. master should not wait
while. nwthreads ~: {. 2 T. '' do. end.  NB. wait till threads become ready
pyx =. 6!:3 t. 0"0 (>:N0) # 1.0  NB. fill up with delating threads
t0 =. 6!:1''
1. = >pyx
assert. (t0 + 0.5) > 6!:1''  NB. master should wait
while. nwthreads ~: {. 2 T. '' do. end.  NB. wait till threads become ready
pyx =. 6!:3 t. 1"0 (>:N0) # 1.0  NB. fill up with delating threads
t0 =. 6!:1''
1. = >pyx
assert. (t0 + 0.5) < 6!:1''  NB. master should not wait
NB. semaphore test
localpyx=.5 T. 5
g t. '' localpyx;<1
'remotepyx remote_stuff'=.>localpyx
assert. remote_stuff=2
6 T. remotepyx,<(localpyx =. 5 T. 5);<3
'remotepyx remote_stuff'=.>localpyx
assert. remote_stuff=4
NB. mutex
mtx =. 10 T. 0
0 = 11 T. mtx;0
1 = 11 T. mtx;0
13 T. mtx
0 = 11 T. mtx
'domain error' -: 11 T. etx >: mtx
'interface error' -: 14 T. etx mtx
13 T. mtx
14 T. mtx
mtx =. 10 T. 0
tod =. 6!:1''
0 = 11 T. <mtx  NB. boxed mtx OK
1 = 11 T. mtx;2.0
(2.005 > dly) *. 2 < dly =. tod-~6!:1''  NB. verify delay
tod =. 6!:1''
1 = 11 T. mtx;0.1
(0.105 > dly) *. 0.1 < dly =. tod-~6!:1''  NB. verify delay
tod =. 6!:1''
1 = 11 T. mtx;0
(0.005 > dly) *. 0 < dly =. tod-~6!:1''  NB. verify no delay
13 T. mtx
14 T. mtx
mtx=.10 T. 1  NB. recursive
0 = 11 T. mtx
0 = 11 T. mtx  NB. lock count=2
13 T. mtx
'interface error' -: 14 T. etx mtx
13 T. mtx
'' -: $ mtx  NB. mtx looks atomic
1 = #mtx
'domain error' -: 11 T. etx mtx;'a'
'rank error' -: 11 T. etx mtx;i. 1 1
'length error' -: 11 T. etx mtx;1 2
'length error' -: 11 T. etx mtx;1;2
'rank error' -: 11 T. etx 2 2 $<mtx
14 T. mtx
end.
1
)
f ''

4!:55 ;:'N N0 N1 f f1 f2 g g1 '

epilog''

