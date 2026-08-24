prolog './gtdot3.ijs'
NB. T. t. ------------------------------------------------------------------
NB. mutex

0!:_1`1:@.(9!:56'pyxes') '?'   NB. skip if not pyxes

NB. **************************************** threads & tasks **********************************
NB. pyxes only

NB. wait until there are y free threads
wthr=: {{ while. y ~: {. 2 T.'' do. 6!:3]0.001 end. 1 }}

NB. delete all worker threads
delth''  NB. make sure we start with an empty system

NB. leave us with y worker threads
setth =: {{ while. t =. * y-1 T. '' do. '' T.~ t{0 0 55 end. 1 }} 

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

NB. mutex

sema =: 3 : 0  NB. semaphore task
'remotepyx remote_stuff'=.y
assert. remote_stuff=1
empty 6 T. remotepyx,<(localpyx =. 5 T. 2);<2
'remotepyx remote_stuff'=.>localpyx
assert. remote_stuff=3
6 T. remotepyx,<(localpyx =. 5 T. 2);<4
4!:55 <'localpyx'
1
)

f =: 3 : 0''
try.
while. 2 > 1 T. '' do. 0 T. '' end.  NB. make sure we have 2 worker threads
catch.
assert. -. (1<{:8&T.'')  NB. threads should be supported on all 64-bit systems
end.
1
)

(<@i."0 i. 5) -: (i. t. ''"0 i. 5)
2 <: 1 T. ''

granularity=: 2e_13
NB. verify that tasks go to different threads if possible.  We can ensure this only for as many threads as there are cores
setth nwthreads=: 1 >. <: 0 { 8 T. ''  NB. one worker thread per core, -1
wthr nwthreads
(>: i. nwthreads) *./@e. > ({.@(3&T.)@'')@(6!:3) t.'' "(0)  (0.1 #~ <:nwthreads), 0.6
wthr nwthreads
(>: i. nwthreads) *./@e. > ({.@(3&T.)@'')@(6!:3) t.'' "(0)  (0.6 #~ <:nwthreads), 0.1
wthr nwthreads
(*9!:56'AUDITEXECRESULTS') +. (((<_1000) #~ <: nwthreads),(>: i. nwthreads);_1001) e.~&> 4 T. ({.@(3&T.)@'')@(6!:3) t.'' "(0) (0.3 #~ <: nwthreads), 2 1  NB. last thread should run in master; earlier ones complete first
wthr nwthreads
setth nwthreads=: N   NB. Back to testing many threads
(nwthreads-1) = 1 T. '' [ 55 T. ''
nwthreads = 1 T. '' [ 0 T. ''
NB. Verify forcetask arg
pyx =: 6!:3 t. ''"0 N # 1.0  NB. fill up with delaying threads
NB. Verify forcetask arg
t0 =: 6!:1''
1. = >pyx
NB. Verify forcetask arg
NB. assert. (t0 + 0.5) < 6!:1''  [ 'a1' NB. master should not wait
wthr nwthreads
pyx =: 6!:3 t. ''"0 (>:N) # 1.0  NB. fill up with delaying threads
t0 =: 6!:1''
1. = >pyx
NB. assert. (t0 + 0.5) > 6!:1''  [ 'a2' NB. master should wait
wthr nwthreads
pyx =: 6!:3 t. 'worker'"0 (>:N) # 1.0  NB. fill up with delaying threads
t0 =: 6!:1''
1. = >pyx
NB. assert. (t0 + 0.5) < 6!:1''  [ 'a3' NB. master should not wait
wthr nwthreads
pyx =: 6!:3 t. (<'worker') "0 (>:N) # 1.0  NB. fill up with delaying threads
t0 =: 6!:1''
1. = >pyx
NB. assert. (t0 + 0.5) < 6!:1''  [ 'a4' NB. master should not wait
wthr nwthreads
pyx =: 6!:3 t. (<'worker';1) "0 (>:N) # 1.0  NB. fill up with delaying threads
t0 =: 6!:1''
1. = >pyx
NB. assert. (t0 + 0.5) < 6!:1''  [ 'a5' NB. master should not wait
wthr nwthreads
pyx =: 6!:3 t. (<'worker';0) "0 (>:N) # 1.0  NB. fill up with delaying threads
t0 =: 6!:1''
1. = >pyx
NB. assert. (t0 + 0.5) > 6!:1''  [ 'a6' NB. master should wait
NB. semaphore test
localpyx=:5 T. 5
empty sema t. '' localpyx;<1  NB. Must not display result of sema
'remotepyx remote_stuff'=:>localpyx
remote_stuff=2
6 T. remotepyx,<(localpyx =: 5 T. 5);<3
'remotepyx remote_stuff'=:>localpyx
remote_stuff=4
NB. mutex
mtx =: 10 T. 0
0 = >{{11 T. y;0}}t.''mtx
1 = 11 T. mtx;0
mtx =: 10 T. 0
0 = 11 T. mtx
'domain error' -: 11 T. etx >:&.> mtx
mtx =: 10 T. 0
tod =: 6!:1''
0 = >{{11 T. y}}t.''mtx  NB. boxed mtx OK
NB. the following line hang on freebsd
1 = 11 T. mtx;2.0
echo (2.3 > dly) , (2-granularity) , dly =: tod-~6!:1''  NB. verify delay
(2.3 > dly) *. (2-granularity) <: dly =: tod-~6!:1''  NB. verify delay
tod =: 6!:1''
1 = 11 T. mtx;0.1
echo (0.3 > dly) , (0.1-granularity) , dly =: tod-~6!:1''  NB. verify delay
(0.3 > dly) *. (0.1-granularity) <: dly =: tod-~6!:1''  NB. verify delay
tod =: 6!:1''
1 = 11 T. mtx;0
echo (0.3 > dly) , 0 , dly =: tod-~6!:1''  NB. verify no delay
(0.3 > dly) *. 0 <: dly =: tod-~6!:1''  NB. verify no delay
mtx=:10 T. 1  NB. recursive
0 = 11 T. mtx
0 = 11 T. mtx  NB. lock count=2
empty 13 T. mtx
empty 13 T. mtx
'' -: $ mtx  NB. mtx looks atomic
1 = #mtx
'domain error' -: 11 T. etx mtx;'a'
'rank error' -: 11 T. etx mtx;i. 1 1
'length error' -: 11 T. etx mtx;1 2
'length error' -: 11 T. etx mtx;1;2
'rank error' -: 11 T. etx 2 2 $<mtx

wthr N

f1=: 3 : 0
11 T. mx
20 T. ''
13 T. mx
1
)

f=: 3 : 0''
{{0 T.0}}^:] 0 >. (1&T.'') -~ <: {: 8 T. ''
mx=: 10 T. 0
for_i. i.10 do.
  1:&> (f1 t. '')"0 i. QKTEST{1e5 1e3
end.
''
)

wthr N

NB.?  end of skip if not pyxes

epilog''

