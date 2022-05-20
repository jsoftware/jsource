prolog './gicap.ijs'
NB. I.y -----------------------------------------------------------------

ifb=: (# i.@#)
n=: 547

(I. -: ifb)  n$0
(I. -: ifb)  n$1
(I. -: ifb) ?n$2
(I. -: ifb) ?n$100
(I. -: ifb)@:(?@$&2)"0 i. 257

1 = {. I. b. 0

'domain error' -: I. etx 'abc'
'domain error' -: I. etx ;:'Cogito, ergo sum.'
'domain error' -: I. etx u:'abc'
'domain error' -: I. etx u:&.> ;:'Cogito, ergo sum.'
'domain error' -: I. etx 10&u:'abc'
'domain error' -: I. etx 10&u:&.> ;:'Cogito, ergo sum.'
'domain error' -: I. etx s:@<"0 'abc'
'domain error' -: I. etx s:@<"0&.> ;:'Cogito, ergo sum.'
'domain error' -: I. etx <"0@s: ;:'Cogito, ergo sum.'


NB. I.@~: y -------------------------------------------------------------

f=: ((= i.@#) # ]) @ (i.~)

test=: 3 : 0
 yy=: y
 assert. (f -: I.@~:) yy
 1
)

test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 0 1
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 'abcde'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: u:'abcde'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 10&u:'abcde'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: ?5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: ?5$2e9
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: o.?5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: j./?2 5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: (u:&.>) ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: (10&u:&.>) ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: s:@<"0&.> ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: <"0@s: ;:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: x: ?5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.5   [ t=: %/0 1+x:?2 5$20
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: u: ?5$65536
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: 10&u: RAND32 ?5$C4MAX
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s:' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s: u: 128+a.i. ' miasma chthonic chronic kakistocracy dado'
test@:(t"_ {~ 877&, ?@$ (#t)"_)"0 >:i.3 5 [ t=: s: 10&u: 65536+a.i. ' miasma chthonic chronic kakistocracy dado'


4!:55 ;:'f ifb n t test yy'



epilog''

