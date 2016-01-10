NB. [: ------------------------------------------------------------------

ss =: [: +/ *:
(+/*:x)  -: ss x=:  _40+?20$1000
(+/*:x)  -: ss x=:o._40+?20$1000
(+/x*:y) -: x ss y [ x=:?20$2 [ y=:?20$2

cc=: [:
f =: cc +/ *:
(+/*:x) -: f x=:_40+?20$1000
cc=: c1
c1=: c2
c2=: [:
(+/*:x) -: f x=:_40+?20$1000
cc =. %:
((%:x)+/*:x) -: f x

'domain error' -: [: etx 1 0 1
'domain error' -: [: etx 3 4$'chthonic'
'domain error' -: [: etx ?2 3 4$100000
'domain error' -: [: etx o.?2 4$100000
'domain error' -: [: etx r.o.?2 4$100000
'domain error' -: [: etx ;:'Cogito, ergo sum.'

'domain error' -: (<'boo hoo')   [: etx 1 0 1
'domain error' -: (?7$10000)     [: etx 'triskaidekaphobia'
'domain error' -: (a.{~?95$256)  [: etx ?2 3 4$100000
'domain error' -: (?17$2)        [: etx o.?2 4$100000
'domain error' -: 'eleemosynary' [: etx r.o.?2 4$100000
'domain error' -: ''             [: etx ;:'Cogito, ergo sum.'


4!:55 ;:'c1 c2 cc f ss t x y'


