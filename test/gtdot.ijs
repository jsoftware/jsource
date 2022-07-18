prolog './gtdot.ijs'
NB. T. t. ------------------------------------------------------------------

NB. **************************************** threads & tasks **********************************
NB. j904 64-bit only

NB. wait until there are y waiting threads
wthr=: {{ while. y ~: {. 2 T.'' do. 6!:3]0.001 end. 1 }}
delth =: {{ while. 1 T. '' do. 55 T. '' end. 1 }}  NB. delete all worker threads
delth''  NB. make sure we start with an empty system

N=: <: 1 { 8 T. ''  NB. max # worker threads
N > 0
N1=: <.@%: N
N1 > 0
N >: N1*N1
N2=: 32 <. <.@%: 20 * N+2
N2 > 0

NB. create all available threads
1: 0&T."1^:(0 < #) ''$~ (0 >. N-1 T. ''),0
N = 1 T.''
wthr N

NB. no more thread can be created
'limit error' -: 0&T. etx ''

f=: 4 : 0
echo^:PRINTMSG 'test ',(":1),' f ',(":y),' start: ',":6!:0''
pyx=. g t. (<'worker';x) "0 i.y
while. do.
 r=. 4 T. pyx               NB. echo r for debug
 if. *./0>r do. break. end.
 6!:3[0.001
end.
if. -. r e. _1000 _1001 do. pyxA =: pyx [ rA =: r end.
assert. r e. _1000 _1001    NB. _1001 if run on the main thread
echo^:PRINTMSG 'test ',(":1),' f ',(":y),' finish: ',":6!:0''
]&> pyx
)

f1=: 4 : 0
if. 0 [ x*.GITHUBCI*.IFWIN do. y=. 10 <. y end.    NB. temp workaround
echo^:PRINTMSG 'test ',(":1),' f1 ',(":y),' start: ',":6!:0''
pyx=. x&g1@y t. (<'worker';x) "0 i.y
while. do.
 r=. 4 T. pyx               NB. echo r for debug
 if. *./0>r do. break. end.
 6!:3[0.001
end.
if. -. r e. _1000 _1001 do. pyxA =: pyx [ rA =: r end.
assert. r e. _1000 _1001    NB. _1001 if run on the main thread
echo^:PRINTMSG 'test ',(":1),' f1 ',(":y),' finish: ',":6!:0''
]&> pyx
)

f2=: 4 : 0
if. 0 [ x*.GITHUBCI*.IFWIN do. y=. 10 <. y end.    NB. temp workaround
echo^:PRINTMSG 'test ',(":1),' f2 ',(":y),' start: ',":6!:0''
pyx=. x&g1@y t. (<'worker';x) "0 i.y
pyx0=. g t. (<'worker';x) "0 i.y
while. do.
 r=. 4 T. pyx,pyx0               NB. echo r for debug
 if. *./0>r do. break. end.
 6!:3[0.001
end.
if. -. r e. _1000 _1001 do. pyxA =: pyx [ rA =: r end.
assert. r e. _1000 _1001    NB. _1001 if run on the main thread
echo^:PRINTMSG 'test ',(":1),' f2 ',(":y),' finish: ',":6!:0''
(,]&> pyx), ]&>pyx0
)

g=: 3 : 0
9!:1[(7^5)+3 T.''  NB. random per thread
6!:3[0.001*?0      NB. arbitrary delay
1
)

g1=: 4 : 0
9!:1[(7^5)-3 T.''  NB. random per thread
for_i. i.y do.
 6!:3[0.001*?0     NB. arbitrary delay
 pyx=. g t. (<'worker';x) "0 i.y
end.
]&> pyx
)

N = 1 T.''
wthr N

NB. number of tasks < number of worker threads

0 f N            NB. run in master thread

N = 1 T.''
wthr N

1 f N            NB. queued job

N = 1 T.''
wthr N

0 f1 N1          NB. run in master thread

N = 1 T.''
wthr N

1 f1 N1          NB. queued job

N = 1 T.''
wthr N

0 f2 N1          NB. run in master thread

N = 1 T.''
wthr N

1 f2 N1          NB. queued job

N = 1 T.''
wthr N

NB. number of tasks > number of worker threads

0 f 2*N          NB. run in master thread

N = 1 T.''
wthr N

1 f 2*N          NB. queued job

N = 1 T.''
wthr N

0 f1 N1          NB. run in master thread

N = 1 T.''
wthr N

1 f1 N1          NB. queued job

N = 1 T.''
wthr N

0 f2 N1          NB. run in master thread

N = 1 T.''
wthr N

1 f2 N1          NB. queued job

N = 1 T.''
wthr N

0 f1 N2          NB. run in master thread

N = 1 T.''
wthr N

1 f1 N2          NB. queued job stress test

N = 1 T.''
wthr N

1 f1 40          NB. queued job stress test

N = 1 T.''
wthr N

allowlongjobs =. 0 NB. some of these too long on Windows

1 1:`f1@.allowlongjobs 45          NB. queued job stress test

N = 1 T.''
wthr N

1 1:`f1@.allowlongjobs 50          NB. queued job stress test

N = 1 T.''
wthr N

1 1:`f1@.allowlongjobs 55          NB. queued job stress test

N = 1 T.''
wthr N

1 1:`f1@.allowlongjobs 60          NB. queued job stress test

N = 1 T.''
wthr N

0 f2 N2          NB. run in master thread

N = 1 T.''
wthr N

1 f2 N2          NB. queued job

N = 1 T.''
wthr N

'domain error' -: ". etx '0 t. ($0)'
'domain error' -: ". etx '+ t. 1.5'
'domain error' -: ". etx '+ t. ''abc'''
'domain error' -: ". etx '+ t. (<1.5)'
'domain error' -: ". etx '+ t. (''worker'';''abc'')'
'domain error' -: ". etx '+ t. (''worker'';''worker'')'
'domain error' -: ". etx '+ t. ((''worker'';0),&< (''worker'';0))'
'domain error' -: ". etx '+ t. (<''worker'';2)'
> ] t. (($0);'worker') 1

'limit error' -: 0 T. etx 8
'limit error' -: 2 T. etx 8
'limit error' -: ". etx '] t. 8'

4!:55 ;:'allowlongjobs delth N N1 N2 f f1 f2 g g1 wthr'

epilog''

