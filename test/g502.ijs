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

'valence error' -: [: etx 1 0 1
'valence error' -: [: etx 3 4$'chthonic'
'valence error' -: [: etx 3 4$u:'chthonic'
'valence error' -: [: etx 3 4$10&u:'chthonic'
'valence error' -: [: etx 3 4$s:@<"0 'chthonic'
'valence error' -: [: etx ?2 3 4$100000
'valence error' -: [: etx o.?2 4$100000
'valence error' -: [: etx r.o.?2 4$100000
'valence error' -: [: etx ;:'Cogito, ergo sum.'
'valence error' -: [: etx u:&.> ;:'Cogito, ergo sum.'
'valence error' -: [: etx 10&u:&.> ;:'Cogito, ergo sum.'
'valence error' -: [: etx s:@<"0&.> ;:'Cogito, ergo sum.'
'valence error' -: [: etx <"0@s: ;:'Cogito, ergo sum.'

'valence error' -: (<'boo hoo')   [: etx 1 0 1
'valence error' -: (<u:'boo hoo')   [: etx 1 0 1
'valence error' -: (<10&u:'boo hoo')   [: etx 1 0 1
'valence error' -: (?7$10000)     [: etx 'triskaidekaphobia'
'valence error' -: (?7$10000)     [: etx u:'triskaidekaphobia'
'valence error' -: (?7$10000)     [: etx 10&u:'triskaidekaphobia'
'valence error' -: (?7$10000)     [: etx s:@<"0 'triskaidekaphobia'
'valence error' -: (a.{~?95$256)  [: etx ?2 3 4$100000
'valence error' -: (?17$2)        [: etx o.?2 4$100000
'valence error' -: 'eleemosynary' [: etx r.o.?2 4$100000
'valence error' -: ''             [: etx ;:'Cogito, ergo sum.'
'valence error' -: ''             [: etx u:&.> ;:'Cogito, ergo sum.'
'valence error' -: ''             [: etx 10&u:&.> ;:'Cogito, ergo sum.'
'valence error' -: ''             [: etx s:@<"0&.> ;:'Cogito, ergo sum.'
'valence error' -: ''             [: etx <"0@s: ;:'Cogito, ergo sum.'


4!:55 ;:'c1 c2 cc cc f ss t x y'



epilog''

