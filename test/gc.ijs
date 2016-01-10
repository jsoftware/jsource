NB. C.y -----------------------------------------------------------------

boxed =: 32&=@type
ord   =: >:@(>./)
pind  =: ]`]`+@.(*@])"0
pfill =: [ ((i.@[-.]) , ]) pind

ac    =: (, i. ]) { 1&|.@[ , ]
dfc   =: >@(ac&.>/)@(pind&.> , <@i.@[)
bc    =: <@((i.>./) |. ])@~.
cfd   =: ~.@(/: {.&>)@:(bc"1)@|:@({/\)@(,~@[ $ pfill)
cdot1 =: (ord cfd ])`(ord@; dfc ])@.boxed " 1

isperm  =: 3 : '(/:~y)-:i.#y' " 1
iscyc   =: 3 : '(isperm ;y)*.(({.&>y)-:>./&>y)*.(i.#y)-:/:{.&>y' " 1

(C.4 5 2 1 0 3) -: (,2);4 0;5 3 1
(C.2;4 0;5 3 1) -: 4 5 2 1 0 3
'C.' -: C. b. _1

p =: 12?15$12
^/@:(iscyc@C.) p
p -: C.&C. p
c =: C.p
c -: C.C.c
^/@:(C. -: cdot1) p

(C.k   ) -: (,&.>i._1{k),<k      [ k =: 1 0+?100
(C.k,_2) -: (,&.>i. <:k),<k-0 1  [ k =: 2+?100

'domain error' -: C. etx 'abcde'
'domain error' -: C. etx 3j4 5
'domain error' -: C. etx 3 4;'abc'


NB. C.!.2 y -------------------------------------------------------------

parity=: 3 : 0 " 1
 n=. #q=. y
 p=. 0
 for_i. i. n do.
  if. i~:j=. q i. i do.
   q=. (i,i{q) (i,j)}q
   p=. -. p
  end.
 end.
 _1^p
)

(parity -: C.!.2) (i.!n) A. i.n=:3
(parity -: C.!.2) (i.!n) A. i.n=:4
(parity -: C.!.2) (i.!n) A. i.n=:5

0 1 _1 -: C.!.2 ]2 , (i.8) ,: 1 0 2 3 4 5 6 7

CT=: 3 : '(C.!.2 p) (<"1 p)}1$.$~y [ p=. (i.!y) A. i.y'  NB. complete tensor

(CT 3) -: C.!.2&> {i.&.> $~ 3
(CT 4) -: C.!.2&> {i.&.> $~ 4

'domain error' -: ex 'C.!.3    '
'domain error' -: ex 'C.!.''a'''
'domain error' -: ex 'C.!.(<2) '

'rank error'   -: ex 'C.!.2 2  '


NB. x C.y ---------------------------------------------------------------

boxed =: 32&=@type
pind  =: ]`]`+@.(*@])"0
pfill =: [ ((i.@[-.]) , ]) pind

ac    =: (, i. ]) { 1&|.@[ , ]
dfc   =: >@(ac&.>/)@(pind&.> , <@i.@[)
cdot2 =: ((#@] pfill`dfc@.(boxed@]) [) { ]) " 1 _

cyc   =: (1&<@#@> # ])@C.

p (C. -: cdot2) x [ p=:?~#x=:1=?200 2$2
p (C. -: cdot2) x [ p=:?~#x=:(?200$#a.){a.
p (C. -: cdot2) x [ p=:?~#x=:?200 2$2
p (C. -: cdot2) x [ p=:?~#x=:o.?200 2$2
p (C. -: cdot2) x [ p=:?~#x=:r.?200$2002
p (C. -: cdot2) x [ p=:?~#x=:(?200$#t){t=:;:'#a.){a.now is the 1 2 3'

(cyc p) (C. -: cdot2) x [ p=:?~#x=:1=?200 2$2
(cyc p) (C. -: cdot2) x [ p=:?~#x=:(?200$#a.){a.
(cyc p) (C. -: cdot2) x [ p=:?~#x=:?200 2$2
(cyc p) (C. -: cdot2) x [ p=:?~#x=:o.?200 2$2
(cyc p) (C. -: cdot2) x [ p=:?~#x=:r.?200$2002
(cyc p) (C. -: cdot2) x [ p=:?~#x=:(?200$#t){t=:;:'#a.){a.now is the 1 2 3'

1 2 3 0 4 -: (2 1;3 0 1)C.i.5

(p C. x) -: (p=:?~200){x=:1=?200 2$2
(p C. x) -: (p=:?~200){x=:(?200$#a.){a.
(p C. x) -: (p=:?~200){x=:?200 2$2000
(p C. x) -: (p=:?~200){x=:o.?200 2$2000
(p C. x) -: (p=:?~200){x=:^0j1*?200$200
(p C. x) -: (p=:?~200){x=:(?200$#x){x=:;:'#a.){a.now is the 1 2 3'

((cyc p) C. x) -: (p=:?~400){x=:1=?400 2$2
((cyc p) C. x) -: (p=:?~1200){x=:(?1200$#a.){a.
((cyc p) C. x) -: (p=:?~200){x=:?200 2$2000
((cyc p) C. x) -: (p=:?~200){x=:o.?200 2$2000
((cyc p) C. x) -: (p=:?~200){x=:^0j1*?200$200
((cyc p) C. x) -: (p=:?~200){x=:(?200$#x){x=:;:'#a.){a.now is the 1 2 3'

'index error' -: 0 1 24     C. etx i.4
'index error' -: 0 1 _24    C. etx i.4

'domain error' -: 3 4j5     C. etx i.4
'domain error' -: 'abc'     C. etx i.4
'domain error' -: (3 4;'a') C. etx i.4


NB. C. ------------------------------------------------------------------

pow   =: i.@#@[ C.~ (#&>@C.@[|]) # C.@[ NB. y-th power of permutation x
ord   =: *./@(#&>"_)@C.                 NB. the order of permutation y

assert=: 0!:2@":^:-.                    NB. assert y    signal error if 0=y

g0    =: , ,. =@i.@2:                   NB. initial state for GCD
it    =: {: ,: {. - {: * <.@%&{./       NB. iterative step for GCD
gcd   =: (}.@{.)@(it^:(*@{.@{:)^:_)@g0  NB. GCD as a linear combination

ab    =: |.@(gcd/ * [ % +./)@(,&{.)     NB. coeff. for Chinese Remainder
cr    =: [: |/\ *.&{. , ,&{: +/ .* ab   NB. Chinese Remainder

wh    =: <:@#@[ - { i. {:@[             NB. oc wh q     index in opened cycle
chk0  =: [: assert { -: wh |. [         NB. oc chk0 q   1 iff q is legal
where =: wh [ chk0                      NB. oc where q  wh with validation
mr    =: #&>@[ ,. (>@[ where ])"0 1     NB. c mr q      moduli and residues
chk1  =: ] [ [: assert {:@[ -: {.@[|]   NB. check Chinese Remainder
log   =: (|: chk1 {:@(cr/)) @ (C.@[mr]) NB. q -: p pow p log q

p=:?~300
(i.#p)  -: p pow 0
p       -: p pow 1
(p{p)   -: p pow 2
(p{p{p) -: p pow 3
(/:p)   -: p pow <:ord p
(i.#p)  -: p pow ord p

p1 =: {^:(]`(i.@#@[))   NB. much slower than pow
p (pow -: p1) k=:?10000
p (pow -: p1) k=:?10000

0 $ 0 : 0
n=:ord p
(n-1) = p log /:p
0 = p log i.#p
1 = p log p
2 = p log p{p
3 = p log p{p{p
3 = p log p pow 3
k = p log p pow k=:?n
k = p log"1 p pow"1 0 k=:?2 10$n
)


4!:55 ;:'ab ac assert bc boxed c cdot1 cdot2 cfd chk0 '
4!:55 ;:'chk1 cr CT cyc dfc f g g0 gcd iscyc '
4!:55 ;:'isperm it k log mr n ord p p1 parity pfill '
4!:55 ;:'pind pow t wh where x '


