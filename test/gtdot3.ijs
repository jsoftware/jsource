prolog './gtdot3.ijs'
NB. T. t. ------------------------------------------------------------------
NB. mutex

NB. **************************************** threads & tasks **********************************
NB. 64-bit only

NB. wait until there are y free threads
wthr=: {{ while. y ~: {. 2 T.'' do. 6!:3]0.001 end. 1 }}

NB. delete all worker threads
delth =: {{ while. 1 T. '' do. 55 T. '' end. 1 }}
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
6 T. remotepyx,<(localpyx =. 5 T. 2);<2
'remotepyx remote_stuff'=.>localpyx
assert. remote_stuff=3
6 T. remotepyx,<(localpyx =. 5 T. 2);<4
4!:55 <'localpyx'
1
)

f =: 3 : 0
chk=. ('FreeBSD'-:UNAME)
try.
while. 2 > 1 T. '' do. 0 T. '' end.  NB. make sure we have 2 worker threads
catch.
assert. -. (1<{:8&T.'')  NB. threads should be supported on all 64-bit systems
end.
if. (1<{:8&T.'') do.
assert. (<@i."0 i. 5) -: (i. t. ''"0 i. 5)
assert. 2 <: 1 T. ''
NB. verify that tasks go to different threads if possible.  We can ensure this only for as many threads as there are cores
setth nwthreads=. 1 >. <: 0 { 8 T. ''  NB. one worker thread per core, -1
wthr nwthreads
assert. GITHUBCI +. (>: i. nwthreads) *./@e. aaa__   =: > (3&T.@'')@(6!:3) t.'' "(0)  (0.1 #~ <:nwthreads), 0.6
wthr nwthreads
assert. GITHUBCI +. (>: i. nwthreads) *./@e. > (3&T.@'')@(6!:3) t.'' "(0)  (0.6 #~ <:nwthreads), 0.1
wthr nwthreads
assert. (ccc__   =: ((<_1000) #~ <: nwthreads),(>: i. nwthreads);_1001) e.~&> bbb__   =: 4 T. aaa__   =: (3&T.@'')@(6!:3) t.'' "(0) (0.3 #~ <: nwthreads), 2 1  NB. last thread should run in master; earlier ones complete first
wthr nwthreads
setth nwthreads=. N   NB. Back to testing many threads
assert. (nwthreads-1) = 1 T. '' [ 55 T. ''
assert. nwthreads = 1 T. '' [ 0 T. ''
NB. Verify forcetask arg
pyx =. 6!:3 t. ''"0 N # 1.0  NB. fill up with delaying threads
NB. Verify forcetask arg
t0 =. 6!:1''
assert. 1. = >pyx
techo^:chk 'gtdot3 a13'
NB. Verify forcetask arg
NB. assert. (t0 + 0.5) < 6!:1''  [ 'a1' NB. master should not wait
techo^:chk 'gtdot3 a13a'
wthr nwthreads
techo^:chk 'gtdot3 a13b'
pyx =. 6!:3 t. ''"0 (>:N) # 1.0  NB. fill up with delaying threads
techo^:chk 'gtdot3 a13c'
t0 =. 6!:1''
assert. 1. = >pyx
NB. assert. (t0 + 0.5) > 6!:1''  [ 'a2' NB. master should wait
wthr nwthreads
pyx =. 6!:3 t. 'worker'"0 (>:N) # 1.0  NB. fill up with delaying threads
techo^:chk 'gtdot3 a13d'
t0 =. 6!:1''
assert. 1. = >pyx
NB. assert. (t0 + 0.5) < 6!:1''  [ 'a3' NB. master should not wait
wthr nwthreads
pyx =. 6!:3 t. (<'worker') "0 (>:N) # 1.0  NB. fill up with delaying threads
techo^:chk 'gtdot3 a13e'
t0 =. 6!:1''
assert. 1. = >pyx
NB. assert. (t0 + 0.5) < 6!:1''  [ 'a4' NB. master should not wait
wthr nwthreads
pyx =. 6!:3 t. (<'worker';1) "0 (>:N) # 1.0  NB. fill up with delaying threads
techo^:chk 'gtdot3 a13f'
t0 =. 6!:1''
assert. 1. = >pyx
NB. assert. (t0 + 0.5) < 6!:1''  [ 'a5' NB. master should not wait
wthr nwthreads
pyx =. 6!:3 t. (<'worker';0) "0 (>:N) # 1.0  NB. fill up with delaying threads
techo^:chk 'gtdot3 a13g'
t0 =. 6!:1''
1. = >pyx
NB. assert. (t0 + 0.5) > 6!:1''  [ 'a6' NB. master should wait
NB. semaphore test
localpyx=.5 T. 5
sema t. '' localpyx;<1
'remotepyx remote_stuff'=.>localpyx
assert. remote_stuff=2
6 T. remotepyx,<(localpyx =. 5 T. 5);<3
'remotepyx remote_stuff'=.>localpyx
assert. remote_stuff=4
NB. mutex
mtx =. 10 T. 0
assert. 0 = >{{11 T. y;0}}t.''mtx
assert. 1 = 11 T. mtx;0
mtx =. 10 T. 0
assert. 0 = 11 T. mtx
'domain error' -: 11 T. etx >:&.> mtx
mtx =. 10 T. 0
techo^:chk 'gtdot3 a32'
tod =. 6!:1''
techo^:chk 'gtdot3 a32a'
assert. 0 = >{{11 T. y}}t.''mtx  NB. boxed mtx OK
techo^:chk 'gtdot3 a32b'
NB. the following line hang on freebsd
assert. 1 = 11 T. mtx;2.0
techo^:chk 'gtdot3 a32c'
assert. GITHUBCI +. (2.3 > dly) *. 2 <: dly =. tod-~6!:1''  NB. verify delay
techo^:chk 'gtdot3 a32d'
tod =. 6!:1''
assert. 1 = 11 T. mtx;0.1
assert. GITHUBCI +. (0.3 > dly) *. 0.1 <: dly =. tod-~6!:1''  NB. verify delay
tod =. 6!:1''
assert. 1 = 11 T. mtx;0
assert. GITHUBCI +. (0.3 > dly) *. 0 <: dly =. tod-~6!:1''  NB. verify no delay
mtx=.10 T. 1  NB. recursive
assert. 0 = 11 T. mtx
assert. 0 = 11 T. mtx  NB. lock count=2
13 T. mtx
13 T. mtx
assert. '' -: $ mtx  NB. mtx looks atomic
assert. 1 = #mtx
assert. 'domain error' -: 11 T. etx mtx;'a'
assert. 'rank error' -: 11 T. etx mtx;i. 1 1
assert. 'length error' -: 11 T. etx mtx;1 2
assert. 'length error' -: 11 T. etx mtx;1;2
assert. 'rank error' -: 11 T. etx 2 2 $<mtx
end.
1
)
f ''

wthr N

4!:55 ;:'N N1 N2 delth f aaa__ bbb__ ccc__ sema setth wthr'

epilog''

