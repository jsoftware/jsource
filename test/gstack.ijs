1:@:(dbr bind Debug)@:(9!:19)2^_44[(prolog [ echo^:ECHOFILENAME) './gstack.ijs'
NB. stack issues (function call limit) ----------------------------------

0 0 $  0 : 0
The recursion limit is constrained by the stack size available to 
the J executable file. Crashes due to stack errors can be overcome 
by increasing the stack size. Under Windows, the stack size can be 
queried and set as follows:

   dumpbin /headers       c:\j601\j.exe
   editbin /stack:6000000 c:\j601\j.exe

The stack size is called "size of stack reserve" in the dumpbin output. 
The stack space can also be interrogated in J as follows:

   ss=: 256 #. a. i. +&3 2 1 0@[ { 1!:1@]
   368 ss <'c:\j601\j.exe'
5000000
   360 ss <'c:\j504\j.exe'
5000000

The right argument to ss is the name of the J executable file; the left 
argument is the location in the file of the 4-byte word that specifies 
the stack size.  (The location varies with the file, and the word is in 
reverse byte order for Windows.)
)
NB. f=: 3 : 'f c=:>:y'
'stack error' -: (f=: 3 : 'f y'         ) etx 0
'stack error' -: (f=: 3 : '0!:0 ''f 0''') etx 0
'stack error' -: (f=: 3 : 'f f. y'      ) etx 0
'stack error' -: (f=: 3 : '".''f y'''   ) etx 0
'stack error' -: (f=: 3 : '6!:2 ''f y''') etx 0
'stack error' -: (f=: 3 : '7!:2 ''f y''') etx 0

'stack error' -: (f=: f  + >:) etx 0
'stack error' -: (f=: >: + f ) etx 0

f5=: 0: :. (f5^:_1)
'stack error' -:   f5^:_1 etx 0
'stack error' -: 0 f5^:_1 etx 0
f6=: + :: f6
'stack error' -: ex '2 3 f6 4 5 6'

NB. ". t=: '".t [ c=:>:c' [ c=: 0
'stack error' -: ex '".t'    [ t=: '".t'
'stack error' -: ex '6!:2 t' [ t=: '6!:2 t'
'stack error' -: ex '7!:2 t' [ t=: '7!:2 t'
'stack error' -: ex '0!:0 t' [ t=: '0!:0 t'
(+/i.1+c) -: (0:`(+ $:@<:)@.*) c=: 400
'stack error' -: 0:`([: $: %)@.* etx 5

600 < c=: $:@>: :: <: 0
'stack error' -: ex '$:@>: 0'
'stack error' -: ex '$:@,@>: 0'
'stack error' -: ex '$:@,@,@>: 0'
'stack error' -: ex '$:@,@,@,@>: 0'
'stack error' -: ex '$:@,@,@,@,@>: 0'
'stack error' -: ex '$:@,@,@,@,@,@>: 0'
'stack error' -: ex '$:@,@,@,@,@,@,@>: 0'
'stack error' -: ex '$:@,@,@,@,@,@,@,@>: 0'
'stack error' -: ex '$:@,@,@,@,@,@,@,@,@>: 0'
'stack error' -: ex '$:@,@,@,@,@,@,@,@,@,@>: 0'
'stack error' -: ex '$:@,@,@,@,@,@,@,@,@,@,@>: 0'
'stack error' -: ex '($: + >:) 0'
'stack error' -: ex '(>: + $:) 0'

'stack error' -: $: etx 5
'stack error' -: 4 $: etx 5

'stack error' -: ex 'ff^:] gg&0~`hh'   NB. now not allowed
'stack error' -: ex '* ^:]  +&0~`- '
'stack error' -: ex '* ^:]  +^:]`- '
'stack error' -: ex 't}   t=: +^:]`-'
'stack error' -: ex 't`:0 t=: +^:]`-'

'stack error' -: ex '128!:2~ ''128!:2~'''
'stack error' -: ex 't 128!:2 ;~t' [ t=: '0&{:: ([ 128!:2 ;) 1&{::'
p=: 3 : 0
 m=.(3 #:@:+ [: i.@<:&.-: 2^#) y
 c=.m <@:p;.2"1 y
)
'stack error' -: ex 'p;:''a b c'''
f =: 3 : 0
for_loc. i. y do.
  coclass 'A',": loc
end.
coclass 'base'
for_loc. i. y do.
  coerase <'A',": loc
end.
)
f 10000
NB. test for self-replicating gerund
{{ J=. ((<@:((":0) ,&:< ]) , ])([.].))(`:6)
 CRASH=. 5!:1@<'J'
try.
(CRASH ((<@:((,'0') ,&:< ]) , ]) ([.].))) (`:6)
0
catch.
17=13!:11''  NB. should be stack error
end.
}} ''

NB. **************************************** threads & tasks **********************************
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
while. nwthreads ~: # 2 T. '' do. end.  NB. wait till threads become ready
assert. (>: i. nwthreads) *./@e. 2 T. ''
assert. (>: i. nwthreads) *./@e. > (3&T.@'')@(6!:3) t.'' "(0)  (nwthreads # 0.1), 0.3 
assert. (>: i. nwthreads) *./@e. > (3&T.@'')@(6!:3) t.'' "(0)  (nwthreads # 0.3), 0.1 
assert. (((<_1000) #~ <: nwthreads),(>: i. nwthreads);_1001) e.~&> 4 T. (3&T.@'')@(6!:3) t.'' "(0) (0.3 #~ <: nwthreads), 0.6 0.4
while. nwthreads ~: # 2 T. '' do. end.  NB. wait till threads become ready
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
0 = 12 T. mtx
1 = 12 T. mtx
13 T. mtx
0 = 11 T. mtx
'domain error' -: 12 T. etx >: mtx
'interface error' -: 14 T. etx mtx
13 T. mtx
14 T. mtx
mtx =. 10 T. 0
tod =. 6!:1''
0 = 11 T. mtx
1 = 11 T. mtx;2.0
0.45 < dly =. tod-~6!:1''  NB. verify delay
13 T. mtx
14 T. mtx
mtx=.10 T. 1  NB. recursive
0 = 11 T. mtx
0 = 11 T. mtx  NB. lock count=2
13 T. mtx
'interface error' -: 14 T. etx mtx
13 T. mtx
14 T. mtx
end.
1
)
f ''

4!:55 ;:'c f f5 f6 g t p'


