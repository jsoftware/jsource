prolog './gtdot.ijs'
NB. T. t. ------------------------------------------------------------------

NB. **************************************** threads & tasks **********************************
NB. 64-bit only

chk=: (<UNAME) e. 'FreeBSD';'OpenBSD'

3 : 0''
if. IFWIN do.
 sleep=: usleep@>.@(1e6&*)
else.
 sleep=: 6!:3
end.
1
)

NB. wait until there are y waiting threads
wthr=: {{ while. y ~: {. 2 T.'' do. 6!:3]0.001 end. 1 }}
delth =: {{ while. 1 T. '' do. 55 T. '' end. 1 }}  NB. delete all worker threads
delth''  NB. make sure we start with an empty system

N=: <: 1 { 8 T. ''  NB. max # worker threads
N > 0
N1=: <.@%: N
N1 > 0
N >: N1*N1
N2=: ((QKTEST+.4>9!:56'cores'){32 16) <. <.@%: 20 * N+2
N2 > 0

NB. create all available threads
1: 0&T."1^:(0 < #) ''$~ (0 >. N-1 T. ''),0
N = 1 T.''
wthr N

NB. no more thread can be created
'limit error' -: 0&T. etx ''

('|ill-formed name'&([ -: #@[ {. ]) *. '(from pyx)'&([ -: -@#@[ {. ])) LF taketo  (>@[ 9!:59@0) :: ((13!:12)@(0$0) )  13!:8 t. 'worker' 4  NB. pyx error is so flagged
('|ill-formed name'&([ -: #@[ {. ]) *. '(from pyx)'&([ -: -@#@[ {. ])) LF taketo  (>@[ 9!:59@0) :: ((13!:12)@(0$0) )  13!:8 t. '' 4  NB. pyx error is so flagged

NB. delay
pyx=: (6!:3) t. (<'worker';0) "0 [ 1#~chk{(3*N), 8
1:&> pyx

pyx=: (6!:3) t. (<'worker';0) "0 [ 0.1#~chk{(30*N), 8
1:&> pyx

pyx=: (6!:3) t. (<'worker';1) "0 [ 1#~chk{(3*N), 8
1:&> pyx

pyx=: (6!:3) t. (<'worker';1) "0 [ 0.1#~chk{(30*N), 8
1:&> pyx

f=: 4 : 0
pyx=. g t. (<'worker';x) "0 i.y
while. do.
 r=. 4 T. pyx               NB. echo r for debug
 if. *./0>r do. break. end.
 6!:3[0.001
end.
if. -. r e. _1000 _1001 do. pyxA =: pyx [ rA =: r end.
assert. r e. _1000 _1001    NB. _1001 if run on the main thread
]&> pyx
)

f1=: 4 : 0
pyx=. x&g1@y t. (<'worker';x) "0 i.y
while. do.
 r=. 4 T. pyx               NB. echo r for debug
 if. *./0>r do. break. end.
 6!:3[0.001
end.
if. -. r e. _1000 _1001 do. pyxA =: pyx [ rA =: r end.
assert. r e. _1000 _1001    NB. _1001 if run on the main thread
]&> pyx
)

f2=: 4 : 0
pyx=. x&g1@y t. (<'worker';x) "0 i.y
pyx0=. g t. (<'worker';x) "0 i.y
while. do.
 r=. 4 T. pyx,pyx0               NB. echo r for debug
 if. *./0>r do. break. end.
 6!:3[0.001
end.
if. -. r e. _1000 _1001 do. pyxA =: pyx [ rA =: r end.
assert. r e. _1000 _1001    NB. _1001 if run on the main thread
(,]&> pyx), ]&>pyx0
)

g=: 3 : 0
sleep[0.001      NB. arbitrary delay
1
)

g1=: 4 : 0
for_i. i.y do.
 sleep[0.001     NB. arbitrary delay
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

allowlongjobs=: 0 [ QKTEST < 4<:{. 8 T.''       NB. some of these too long to run

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

NB. AMV
amv =: 16 T. 0  NB. AMV with value 0
0 = 17 T. amv,<1
1 = 17 T. amv,<2
3 = 17 T. amv,<1  NB. Now set to 4
4 = 18 T. amv,0;5  NB. failed write
'' -: 18 T. amv,0;4  NB. successful write - amv is now 0
'domain error' -: 16 T. etx 0.5
'domain error' -: 16 T. etx a:
'rank error' -: 16 T. etx 1 2
'domain error' -: 17 T. etx 0
'domain error' -: 17 T. etx 1.5
'rank error' -: 17 T. etx <amv
'length error' -: 17 T. etx amv,0;0
'rank error' -: 17 T. etx ,: amv,<0
'domain error' -: 17 T. etx 1.5;0
'domain error' -: 17 T. etx amv,<0.5
'rank error' -: 17 T. etx amv,<3 2
'domain error' -: 18 T. etx 0
'domain error' -: 18 T. etx 1.5
'rank error' -: 18 T. etx <amv
'length error' -: 18 T. etx amv,<0
'length error' -: 18 T. etx amv,0;3;4
'rank error' -: 18 T. etx ,: amv,0;2
'domain error' -: 18 T. etx 1.5;0;2
'domain error' -: 18 T. etx amv,0.5;2
'rank error' -: 18 T. etx amv,3 2;2
'domain error' -: 18 T. etx amv,2;0.5
'rank error' -: 18 T. etx amv,2;3 2

f =: {{ vec =. 0$0 for. i. y do. vec =. vec , 17 T. x,<1 end. vec }}  NB. x is amv, y is # times to reserve 1
wthr N
amv =: 16 T. 0  NB. AMV with value 0
(i. 30000) -: /:~ ; amv f t. ''"0 ] 3 $ 10000
f =: {{ vec =. 0$0  for. i. y do. exp =. 0 while. $exp do. exp =. 18 T. x,des;exp [ des =. >:exp end. vec =. vec,des end. vec }}  NB. x is amv, y is # times to reserve 1
wthr N
amv =: 16 T. 0  NB. AMV with value 0
(>: i. 3000) -: /:~ ; amv f t. ''"0 ] 3 $ 1000

NB. thread deletion
delth''
0 -: 1 T. ''
(8 {. i. 0 3) -: 2 T."0 i. 8 [ 6!:3 (0.2)  NB. empty
1 -: 0 T. 0  NB. 0
1 -: 1 T. ''
(8 {. ,: 1 0 1) -: 2 T."0 i. 8 [ 6!:3 (0.2)
2 -: 0 T. 1  NB. 0 1
'input interrupt' -: > etx 1!:1 t. '' ] 1   NB. (from pyx) is displayed also
2 -: 1 T. ''
(8 {. 2 # ,: 1 0 1) -: 2 T."0 i. 8 [ 6!:3 (0.2)
3 -: 0 T. 2  NB. 0 1 2
3 -: 1 T. ''
(8 {. 3 # ,: 1 0 1) -: 2 T."0 i. 8 [ 6!:3 (0.2)
4 -: 0 T. 1  NB. 0 1 2 1
4 -: 1 T. ''
(8 {. 1 0 1 , 2 0 2 ,: 1 0 1) -: 2 T."0 i. 8 [ 6!:3 (0.2)
5 -: 0 T. 3  NB. 0 1 2 1 3
5 -: 1 T. ''
(8 {. 1 2 1 1 */ 1 0 1) -: 2 T."0 i. 8 [ 6!:3 (0.2)
55 T. 1  NB. 0 1 2 x 3
4 -: 1 T. ''
(8 {. 1 1 1 1 */ 1 0 1) -: 2 T."0 i. 8 [ 6!:3 (0.2)
4 -: 0 T. 0  NB. 0 1 2 0 3
5 -: 1 T. ''
(8 {. 2 1 1 1 */ 1 0 1) -: 2 T."0 i. 8 [ 6!:3 (0.2)
55 T. 2  NB. 0 1 x 0 3
4 -: 1 T. ''
(8 {. 2 1 0 1 */ 1 0 1) -: 2 T."0 i. 8 [ 6!:3 (0.2)
55 T. 1  NB. 0 x x 0 3
3 -: 1 T. ''
(8 {. 2 0 0 1 */ 1 0 1) -: 2 T."0 i. 8 [ 6!:3 (0.2)
2 -: 0 T. 2  NB. 0 2 x 0 3
4 -: 1 T. ''
(8 {. 2 0 1 1 */ 1 0 1) -: 2 T."0 i. 8 [ 6!:3 (0.2)
3 -: 0 T. 3  NB. 0 2 3 0 3
5 -: 1 T. ''
(8 {.2 0 1 2  */ 1 0 1) -: 2 T."0 i. 8 [ 6!:3 (0.2)
55 T. ''  NB. 0 2 3 0 x
4 -: 1 T. ''
(8 {. 2 0 1 1 */ 1 0 1) -: 2 T."0 i. 8 [ 6!:3 (0.2)
55 T. ''  NB. 0 2 3 x x
3 -: 1 T. ''
(8 {. 1 0 1 1 */ 1 0 1) -: 2 T."0 i. 8 [ 6!:3 (0.2)
55 T. ''  NB. 0 2 x x x
2 -: 1 T. ''
(8 {. 1 0 1  */ 1 0 1) -: 2 T."0 i. 8 [ 6!:3 (0.2)
55 T. ''  NB. 0 x x x x
1 -: 1 T. ''
(8 {. 1 0 */ 1 0 1) -: 2 T."0 i. 8 [ 6!:3 (0.2)
55 T. ''  NB. x x x x x
0 -: 1 T. ''
(8 {. 0 0 */ 1 0 1) -: 2 T."0 i. 8 [ 6!:3 (0.2)

0 T. ''
f =: {{ 1 + y }}
{{)n
|domain error (from pyx)
|       >f t.'worker'y
emsg in pyx:
|domain error in f, executing dyad +
|y is character

}} -:&('|' taketo&.|. ]) {{
try.
> f t. 'worker' y
catch.
13!:12''
end.
}} ' '   NB. verify message from pyx is passed through

delth''




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



4!:55 ;:'allowlongjobs amv chk delth N N1 N2 f f1 f2 g g1 pyx sleep wthr'

epilog''

