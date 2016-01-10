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
1 3 0 -: (y i. ;:'adv sum x'){{:"1 x

4!:55 <'sum'
-. (<'sum') e. >1{18!:30 <'base'

'domain error' -: 18!:30 etx 2
'domain error' -: 18!:30 etx 2.5
'domain error' -: 18!:30 etx 2j5
'domain error' -: 18!:30 etx 2r5

'rank error'   -: 18!:30 etx ''
'rank error'   -: 18!:30 etx ;:'z base'

'locale error' -: 18!:30 etx <'nonexistentlocale'


NB. 18!:31 y ------------------------------------------------------------

pcheck=: 3 : 0
 yy=: y
 assert. ((,3)-:$y) *. 32=type y
 'p a s'=: y
 assert. 2=#$p
 assert. (2=#$p) *. 4 =type p                  NB. symbol pool
 assert. (1=#$a) *. 32=type a                  NB. object name
 assert. (1=#$s) *. 32=type s                  NB. locale name (or '**local**')
 assert. (#p) = (#a),#s

 i=: i.#p
 b=: 0 0 -:"1 ]2 5{"1 p
 assert. {.b
 assert. 0=1 2 3 5{"1 b#p
 assert. (4{"1 b#p) e. (# i.@#) b
 assert. 0 e. 4{"1 b#p
 m=: >:>.2^.#b
 x=: ~. /:~ ,{~^:(i.m) b*4{"1 p                NB. transitive closure
 assert. x -: I. b

 f =: 2{"1 p
 h =: 2<:4|f                                   NB. head of linked list
 li=: 4<:8|f                                   NB. locale info
 assert. i -: 0{"1 p                           NB. index
 assert. b +. li +. 0<1{"1 p                   NB. internal type
 assert. li <: (s e.<'**local**')+.32=1{"1 p   NB. search path of locales 
 assert. 0<:f                                  NB. flag
 assert. b +. li +. (3{"1 p) e. _1,i.#4!:3 ''  NB. script index
 assert.      i e.~ next=. 4{"1 p              NB. next
 assert. h +. i e.~ prev=. 5{"1 p              NB. prev
 assert. b +. h +. (0=next) +. i = (next*-.h){prev,0
 assert. b +. h +.             i = (prev*-.h){next,0

 assert. b +. li +. -. a e. a:
 assert. b +. li +. s e. '**local**';18!:1 i.2
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


4!:55 ;:'a adv b f h i k li m p pcheck s sum t x y yy'


