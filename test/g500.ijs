NB. [ --------------------------------------------------------------------

xb =: ?2 3 4$2
xa =: 'Fourscore and seven years ago'
xi =: ?9 12$100000
xd =: o.?1 2 3$100000
xj =: j.&?~2 2 3 3$1000

xb -: xb [ xb
xb -: xb [ xa
xb -: xb [ xi
xb -: xb [ xd
xb -: xb [ xj

xa -: xa [ xb
xa -: xa [ xa
xa -: xa [ xi
xa -: xa [ xd
xa -: xa [ xj

xi -: xi [ xb
xi -: xi [ xa
xi -: xi [ xi
xi -: xi [ xd
xi -: xi [ xj

xd -: xd [ xb
xd -: xd [ xa
xd -: xd [ xi
xd -: xd [ xd
xd -: xd [ xj

xj -: xj [ xb
xj -: xj [ xa
xj -: xj [ xi
xj -: xj [ xd
xj -: xj [ xj

x=: i.1e4
y=: 2e4$'boustrophedonic chuffed'

(IF64{1000 2400) > 7!:2 'x[y'


4!:55 ;:'x xa xb xd xi xj y'


