prolog './g18x.ijs'
NB. 18!:30 y ------------------------------------------------------------

x=: i.12
sum=: +/
adv=: /

t=: 18!:30 <'base'
(,2) -: $t
32 -: type t
'x y' =: t
2 = #$x
3 = {:$x
4 -: type x
32 -: type y
1 = #$y
(;:'adv sum x') e. y
1 3 0 -: (y i. ;:'adv sum x'){ {:"1 x

4!:55 <'sum'
-. (<'sum') e. >1{18!:30 <'base'

'locale error' -: 18!:30 etx 100
'domain error' -: 18!:30 etx 2.5
'domain error' -: 18!:30 etx 2j5
'domain error' -: 18!:30 etx 2r5

'rank error'   -: 18!:30 etx ''
'rank error'   -: 18!:30 etx ;:'z base'

'locale error' -: 18!:30 etx <'nonexistentlocale'


NB. 18!:31 y ------------------------------------------------------------

pcheck=: 3 : 0
 if. 13!:17'' do. 1 return. end.   NB. parameters not applicable when dbr 1
 yy=: y
 assert. ((,4)-:$y) *. 32=type y
 'p a s f'=. y
 assert. 2=#$p
 assert. (2=#$p) *. 4 =type p                  NB. symbol pool
 assert. (1=#$a) *. 32=type a                  NB. object name
 assert. (1=#$s) *. 32=type s                  NB. locale name (or '**local**')
 assert. (#p) = (#a),#s
NB. p has: index,type,flag,sn,next,origin
NB. Flag is changed,cachable,LINFO,PERM,WASABANDONED,hasname,hasvalue

 i=. i.#p
 NB. b=. 0 0 -:"1 ]2 5{"1 p      NB. flag=0 & no prev pointer: empty symbol, on free list
 b=. 0 = (64+32) 17 b. 2{"1 p      NB. flag=0 (no name or value): empty symbol, on free list
 orph=. 64 = (64+32) 17 b. 2{"1 p      NB. flag=64 (no name, value): cached orphan value
NB. testing  scaforigin__   =: 5{"1 p
NB. testing  scafnfree__   =: f
 assert. {.b  NB. first symbol always free
 assert. 0=2 3{"1 b#p
 assert. (4{"1 b#p) e. (# i.@#) b
 assert. 0 e. 4{"1 b#p
 m=. >:>.2^.#b
 x=. ~. /:~ ,{~^:(i.m) b*4{"1 p                NB. transitive closure - chase the free chain
 assert. x *./@:e.  I. b  NB. was x -:  with multi chains, x is not all free eles

 oktypes =. <. 2 ^ 0 1 2 3 4 5 6 7 10 11 12 13 14 15 16 17 18 23 25 27 29   NB. Type 0 OK if permanent
 f =. 2{"1 p
 NB. pfst =. 2<:4|f                                   NB. symbol is in pfst, may have no value
 li=. 4<:8|f                                   NB. locale info
 perm=. 2<:4|f                                NB. permanent
 assert. i -: 0{"1 p                           NB. index
 assert. b +. li +. ((1{"1 p)e.oktypes) +. (perm *. 0 = 1{"1 p)          NB. internal type
 assert. li <: (s e.<'**local**')+.0 32 e.~ 1{"1 p   NB. search path of locales - 0 if local symbol table
 assert. 0<:f                                  NB. flag
 assert. b +. li +. (3{"1 p) e. _1,i.#4!:3 ''  NB. script index
 assert.      i e.~ next=. 4{"1 p              NB. next
 NB. assert. h +. i e.~ prev=. 5{"1 p              NB. prev
 NB. assert. b +. h +. (0=next) +. i = (next*-.h){prev,0
 NB. assert. b +. h +.             i = (prev*-.h){next,0

 assert. b +. orph +. li +. -. a e. a:
 assert. b +. orph +. li +. s e. '';'**local**';18!:1 i.2  NB. must allow no locale-name for local symbol tables
 assert. (18!:1 i.2) e. s
 1
)

pcheck 18!:31 ''

k=: 18!:3 ''
sum__k=: +/
sam__k=: 'United States of America'
junk_asdf_ =: 400$'foo'

pcheck 18!:31 ''

18!:55 k,<'asdf'

f=: 3 : 0
 a=. 12
 b=. o. y
 pcheck 18!:31 ''
)

f 1 2 3

(<'asdf') -: 18!:3 <'asdf'
pcheck 18!:31 ''
(<'asdf') -: 6 (18!:3) <'asdf'
pcheck 18!:31 ''
(<'asdf') -: 5 (18!:3) <'asdf'
pcheck 18!:31 ''
18!:55 <'asdf'


4!:55 ;:'a adv b f h i k li m oktypes p pcheck perm s sum t x y yy'



epilog''

