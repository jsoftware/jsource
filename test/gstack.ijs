prolog './gstack.ijs'
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

4!:55 ;:'c f f5 f6 t p'



epilog''

