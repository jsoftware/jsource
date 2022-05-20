prolog './g502.ijs'
NB. [: ------------------------------------------------------------------

ss =: [: +/ *:
(+/*:x)  -: ss x=:  _40+?20$1000
(+/*:x)  -: ss x=:o._40+?20$1000
(+/x*:y) -: x ss y [ x=:?20$2 [ y=:?20$2

cc=: [:
f =: cc +/ *:
(+/*:x) -: f x=:_40+?20$1000
NB. no longer honored cc=: c1
NB. no longer honored c1=: c2
NB. no longer honored c2=: [:
NB. no longer honored f =: cc +/ *:
NB. no longer honored (+/*:x) -: f x=:_40+?20$1000
NB. no longer honored cc =. %:
NB. no longer honored ((%:x)+/*:x) -: f x

'domain error' -: [: etx 1 0 1
'domain error' -: [: etx 3 4$'chthonic'
'domain error' -: [: etx 3 4$u:'chthonic'
'domain error' -: [: etx 3 4$10&u:'chthonic'
'domain error' -: [: etx 3 4$s:@<"0 'chthonic'
'domain error' -: [: etx ?2 3 4$100000
'domain error' -: [: etx o.?2 4$100000
'domain error' -: [: etx r.o.?2 4$100000
'domain error' -: [: etx ;:'Cogito, ergo sum.'
'domain error' -: [: etx u:&.> ;:'Cogito, ergo sum.'
'domain error' -: [: etx 10&u:&.> ;:'Cogito, ergo sum.'
'domain error' -: [: etx s:@<"0&.> ;:'Cogito, ergo sum.'
'domain error' -: [: etx <"0@s: ;:'Cogito, ergo sum.'

'domain error' -: (<'boo hoo')   [: etx 1 0 1
'domain error' -: (<u:'boo hoo')   [: etx 1 0 1
'domain error' -: (<10&u:'boo hoo')   [: etx 1 0 1
'domain error' -: (?7$10000)     [: etx 'triskaidekaphobia'
'domain error' -: (?7$10000)     [: etx u:'triskaidekaphobia'
'domain error' -: (?7$10000)     [: etx 10&u:'triskaidekaphobia'
'domain error' -: (?7$10000)     [: etx s:@<"0 'triskaidekaphobia'
'domain error' -: (a.{~?95$256)  [: etx ?2 3 4$100000
'domain error' -: (?17$2)        [: etx o.?2 4$100000
'domain error' -: 'eleemosynary' [: etx r.o.?2 4$100000
'domain error' -: ''             [: etx ;:'Cogito, ergo sum.'
'domain error' -: ''             [: etx u:&.> ;:'Cogito, ergo sum.'
'domain error' -: ''             [: etx 10&u:&.> ;:'Cogito, ergo sum.'
'domain error' -: ''             [: etx s:@<"0&.> ;:'Cogito, ergo sum.'
'domain error' -: ''             [: etx <"0@s: ;:'Cogito, ergo sum.'


4!:55 ;:'c1 c2 cc cc f ss t x y'



epilog''

