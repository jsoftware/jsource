prolog './g510.ijs'
NB. ] --------------------------------------------------------------------

xb =: ?2 3 4$2
xa =: 'Fourscore and seven years ago'
xw =: u:'Fourscore and seven years ago'
xu =: 10&u:'Fourscore and seven years ago'
xi =: ?9 12$100000
xd =: o.?1 2 3$100000
xj =: j.&?~2 2 3 3$1000

xb -: xb ] xb
xb -: xa ] xb
xb -: xw ] xb
xb -: xu ] xb
xb -: xi ] xb
xb -: xd ] xb
xb -: xj ] xb

xa -: xb ] xa
xa -: xa ] xa
xa -: xi ] xa
xa -: xd ] xa
xa -: xj ] xa

xw -: xb ] xw
xw -: xa ] xw
xw -: xu ] xw
xw -: xi ] xw
xw -: xd ] xw
xw -: xj ] xw

xu -: xb ] xu
xu -: xa ] xu
xu -: xw ] xu
xu -: xi ] xu
xu -: xd ] xu
xu -: xj ] xu

xi -: xb ] xi
xi -: xa ] xi
xi -: xw ] xi
xi -: xu ] xi
xi -: xi ] xi
xi -: xd ] xi
xi -: xj ] xi

xd -: xb ] xd
xd -: xa ] xd
xd -: xw ] xd
xd -: xu ] xd
xd -: xi ] xd
xd -: xd ] xd
xd -: xj ] xd

xj -: xb ] xj
xj -: xa ] xj
xj -: xw ] xj
xj -: xu ] xj
xj -: xi ] xj
xj -: xd ] xj
xj -: xj ] xj

x=: i.1e4
y=: 2e4$'boustrophedonic chuffed'

(IF64{2000 2400) > t=: 7!:2 'x]y'

y=: 2e4$u:'boustrophedonic chuffed'

(IF64{2000 2400) > t=: 7!:2 'x]y'

y=: 2e4$10&u:'boustrophedonic chuffed'

(IF64{2000 2400) > t=: 7!:2 'x]y'


NB. ]"r ------------------------------------------------------------------

dex =: 3 : ('y'; ':'; 'y')
lev =: 3 : ('y'; ':'; 'x')

'abc' (]" 1 -: dex" 1) ?2 3 4$100
4 5   (]"_1 -: dex"_1) ?2 3 4$100

f=: ]  ^:(2&|@])
g=: 0&+@]^:(2&|@])

(f/   -: g/  ) x=:?20$10
(f/\  -: g/\ ) x=:?20$10
(f/\. -: g/\.) x=:?20$10
(f"0  -: g"0 ) x=:?20$10
(f^:3 -: g^:3) x=:? 2$10

(i. 3) (]"0 -: dex"0) i. 3 3
(i. 3 3) (]"0 -: dex"0) i. 3
(i. 3 2 4) (]"1 -: dex"1) i. 3

(i. 3) (["0 -: lev"0) i. 3 3
(i. 3 3) (["0 -: lev"0) i. 3
(i. 3 2 4) (["1 -: lev"1) i. 3

'length error' -: 'abc' ]"_1 etx i.2 3
'length error' -: (u:'abc') ]"_1 etx i.2 3
'length error' -: (10&u:'abc') ]"_1 etx i.2 3

(i. 3 7 4 5) (]"2 -: dex"2) (i. 3 4 5)
(i. 3 7 4 5) (]"2 -: dex"2)~ (i. 3 4 5)





epilog''

