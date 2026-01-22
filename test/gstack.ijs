prolog './gstack.ijs'
NB. stack issues (function call limit) ----------------------------------

0!:_1`1:@.(2 -.@*@(17 b.) 9!:56'memaudit') '$'   NB. skip slow

NB. f=: 3 : 'f c=:>:y'
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
(+/i.1+c) -: (0:`(+ $:@<:)@.*) c=: 370  NB. 400 was failing on 32 bit linux running under gdb (debian bullseye)
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

NB. obsolete 'stack error' -: ex 'ff^:] gg&0~`hh'   NB. now not allowed
NB. obsolete 'stack error' -: ex '* ^:]  +&0@]`- '
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
f QKTEST{10000 1000
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

gerund =: ]F.:('@:' ,:@:<@:(,&<) ,~) (QKTEST{3e5 3e5) # < '+'

'stack error' -: ". etx 'gerund`:6' NB. JE crash.


NB. Check handling of deeply nested boxes.
NB. Nesting level.
N =: QKTEST{1e6 1e6 NB. Change to 1e1 to check that tests are correct.
NB. Utils.
arraytolist =: (0 $ 0)&(]F.:(,&<))
listtoarray =: [: 0&{::"1@}: >@{:^:(0 < #)^:a:
NB. 1) Check free.
(-: listtoarray@arraytolist) ?@$~ N
NB. 2) Check L. and free.
(N;'stack error') e.~ < L. etx arraytolist i. N
(0;'stack error') e.~ < N (> L.) etx arraytolist i. N
NB. 3) Cont'd.
((N+1);'stack error') e.~ < L. etx t =: (< arraytolist i. N) ({{ 3 ? N }} '')} <"0 i. N
4!:55 < 't'
NB. 4) Check L:, L. and free.
((N+10);'stack error') e.~ < L.@:({{ arraytolist i. 10 }}L:0) etx arraytolist i. N
NB. 5) Check S: and free.
((,. ,&0 >: i. N);'stack error') e.~ < >:S:0 etx arraytolist i. N
NB. 6) Display and free.
('out of memory';'stack error') e.~ < ": etx <^:N a:

'stack error' -: (<^:1e6 a:) = etx  <^:1e6 a:
'stack error' -: (<^:1e6 a:) ~: etx <^:1e6 a:
'stack error' -: (<^:1e6 a:) -: etx <^:1e6 a:
'stack error' -: /: etx (<^:1e6 ] 2) , <^:1e6 ] 1
'stack error' -: ~. etx (<^:1e6 ] 2) , <^:1e6 ] 1
b1 =: <^:1e6 ] 1
b2 =: <^:1e6 ] 2
a =: b1 , b2
NB. Tests.
'stack error' -: b1 = etx b2
'stack error' -: b1 ~: etx b2
'stack error' -: b1 -: etx b2
'stack error' -: b2 e. etx a
'stack error' -: a i. etx b2
'stack error' -: b2 E. etx a
'stack error' -: a -. etx b2
'stack error' -: a I. etx b2
'stack error' -: ~. etx a
'stack error' -: ~: etx a
'stack error' -: /: etx a
'stack error' -: \: etx a
'domain error' -: b1} etx 'abc'
'domain error' -: '*' b1} etx 'abc'
'stack error' -: a ]/. etx 1 2
'stack error' -: a ]/.. etx 1 2
'stack error' -: 1 2 /: etx a
'stack error' -: 1 2 \: etx a
'stack error' -: a i: etx b1
'stack error' -: <;.1 etx a  NB. u;.1 a
'stack error' -: <;._1 etx a NB. u;._1 a
'stack error' -: <;.2 etx a  NB. u;.2 a
'stack error' -: <;._2 etx a NB. u;._2 a
'stack error' -: b1 =!.0 etx b2

(;: '3 :') -.@-: (;: '3:')   NB. failed once

NB.$  end of skip

epilog''

