prolog './gtdot3.ijs'
NB. T. t. ------------------------------------------------------------------
NB. mutex

NB. **************************************** threads & tasks **********************************
NB. j904 64-bit only

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
chk=. ('FreeBSD'-:UNAME) *. 1 e. 'avx' E. 9!:14''
try.
echo^:chk 'a1'
while. 2 > 1 T. '' do. 0 T. '' end.  NB. make sure we have 2 worker threads
echo^:chk 'a2'
catch.
assert. -. IF64  NB. threads should be supported on all 64-bit systems
end.
if. IF64 do.
echo^:chk 'a3'
assert. (<@i."0 i. 5) -: (i. t. ''"0 i. 5)
assert. 2 <: 1 T. ''
echo^:chk 'a4'
NB. verify that tasks go to different threads if possible.  We can ensure this only for as many threads as there are cores
setth nwthreads=. 1 >. <: 0 { 8 T. ''  NB. one worker thread per core, -1
echo^:chk 'a5'
wthr nwthreads
echo^:chk 'a6'
assert. GITHUBCI +. (>: i. nwthreads) *./@e. aaa__   =: > (3&T.@'')@(6!:3) t.'' "(0)  (0.1 #~ <:nwthreads), 0.6
echo^:chk 'a7'
wthr nwthreads
assert. GITHUBCI +. (>: i. nwthreads) *./@e. > (3&T.@'')@(6!:3) t.'' "(0)  (0.6 #~ <:nwthreads), 0.1
echo^:chk 'a8'
wthr nwthreads
assert. (ccc__   =: ((<_1000) #~ <: nwthreads),(>: i. nwthreads);_1001) e.~&> bbb__   =: 4 T. aaa__   =: (3&T.@'')@(6!:3) t.'' "(0) (0.3 #~ <: nwthreads), 2 1  NB. last thread should run in master; earlier ones complete first
echo^:chk 'a9'
wthr nwthreads
setth nwthreads=. N   NB. Back to testing many threads
echo^:chk 'a10'
assert. (nwthreads-1) = 1 T. '' [ 55 T. ''
assert. nwthreads = 1 T. '' [ 0 T. ''
echo^:chk 'a11'
NB. Verify forcetask arg
pyx =. 6!:3 t. ''"0 N # 1.0  NB. fill up with delaying threads
echo^:chk 'a12'
NB. Verify forcetask arg
t0 =. 6!:1''
assert. 1. = >pyx
echo^:chk 'a13'
NB. Verify forcetask arg
NB. assert. (t0 + 0.5) < 6!:1''  [ 'a1' NB. master should not wait
wthr nwthreads
NB. obsolete echo 't0';6!:1''
pyx =. 6!:3 t. ''"0 (>:N) # 1.0  NB. fill up with delaying threads
echo^:chk 'a14'
NB. obsolete echo 't1';6!:1''
t0 =. 6!:1''
assert. 1. = >pyx
echo^:chk 'a15'
NB. obsolete echo 't2';6!:1''
NB. assert. (t0 + 0.5) > 6!:1''  [ 'a2' NB. master should wait
wthr nwthreads
pyx =. 6!:3 t. 'worker'"0 (>:N) # 1.0  NB. fill up with delaying threads
echo^:chk 'a16'
t0 =. 6!:1''
assert. 1. = >pyx
NB. assert. (t0 + 0.5) < 6!:1''  [ 'a3' NB. master should not wait
echo^:chk 'a17'
wthr nwthreads
pyx =. 6!:3 t. (<'worker') "0 (>:N) # 1.0  NB. fill up with delaying threads
echo^:chk 'a18'
t0 =. 6!:1''
assert. 1. = >pyx
echo^:chk 'a19'
NB. assert. (t0 + 0.5) < 6!:1''  [ 'a4' NB. master should not wait
wthr nwthreads
pyx =. 6!:3 t. (<'worker';1) "0 (>:N) # 1.0  NB. fill up with delaying threads
echo^:chk 'a20'
t0 =. 6!:1''
assert. 1. = >pyx
NB. assert. (t0 + 0.5) < 6!:1''  [ 'a5' NB. master should not wait
wthr nwthreads
echo^:chk 'a21'
pyx =. 6!:3 t. (<'worker';0) "0 (>:N) # 1.0  NB. fill up with delaying threads
t0 =. 6!:1''
echo^:chk 'a22'
1. = >pyx
NB. assert. (t0 + 0.5) > 6!:1''  [ 'a6' NB. master should wait
NB. semaphore test
echo^:chk 'a23'
localpyx=.5 T. 5
echo^:chk 'a24'
sema t. '' localpyx;<1
echo^:chk 'a25'
'remotepyx remote_stuff'=.>localpyx
assert. remote_stuff=2
echo^:chk 'a26'
6 T. remotepyx,<(localpyx =. 5 T. 5);<3
echo^:chk 'a27'
'remotepyx remote_stuff'=.>localpyx
echo^:chk 'a28'
assert. remote_stuff=4
NB. mutex
mtx =. 10 T. 0
echo^:chk 'a29'
assert. 0 = >{{11 T. y;0}}t.''mtx
assert. 1 = 11 T. mtx;0
echo^:chk 'a30'
mtx =. 10 T. 0
assert. 0 = 11 T. mtx
echo^:chk 'a31'
'domain error' -: 11 T. etx >:&.> mtx
mtx =. 10 T. 0
echo^:chk 'a32'
tod =. 6!:1''
echo^:chk 'a32a'
assert. 0 = >{{11 T. y}}t.''mtx  NB. boxed mtx OK
echo^:chk 'a32b'
assert. 1 = 11 T. mtx;2.0
echo^:chk 'a32c'
assert. GITHUBCI +. (2.3 > dly) *. 2 <: dly =. tod-~6!:1''  NB. verify delay
echo^:chk 'a33'
tod =. 6!:1''
assert. 1 = 11 T. mtx;0.1
assert. GITHUBCI +. (0.3 > dly) *. 0.1 <: dly =. tod-~6!:1''  NB. verify delay
tod =. 6!:1''
echo^:chk 'a34'
assert. 1 = 11 T. mtx;0
assert. GITHUBCI +. (0.3 > dly) *. 0 <: dly =. tod-~6!:1''  NB. verify no delay
mtx=.10 T. 1  NB. recursive
echo^:chk 'a35'
assert. 0 = 11 T. mtx
assert. 0 = 11 T. mtx  NB. lock count=2
echo^:chk 'a36'
13 T. mtx
echo^:chk 'a37'
13 T. mtx
echo^:chk 'a38'
assert. '' -: $ mtx  NB. mtx looks atomic
assert. 1 = #mtx
echo^:chk 'a39'
assert. 'domain error' -: 11 T. etx mtx;'a'
assert. 'rank error' -: 11 T. etx mtx;i. 1 1
assert. 'length error' -: 11 T. etx mtx;1 2
assert. 'length error' -: 11 T. etx mtx;1;2
assert. 'rank error' -: 11 T. etx 2 2 $<mtx
echo^:chk 'a40'
end.
1
)
f ''

wthr N

4!:55 ;:'N N1 N2 delth f aaa__ bbb__ ccc__ sema setth wthr'

epilog''

