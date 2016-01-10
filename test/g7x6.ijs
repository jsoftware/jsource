NB. 7!:6 ----------------------------------------------------------------

(7!:6 <'') -: 7!:6 <'base'

spn=: 3 : '>.&.(2&^.) k*2+7+1+4+1+>.(#y)%k=.IF64{4 8'   NB. space needed for a name
NB. 2   MS struct
NB. 7   header words
NB. 1   shape
NB. 4   NM struct
NB. 1   trailing 0 pad
NB. #y  letters in the name

spl=: 4 : 0   NB. space needed for locale y with hash table size x
 z=. spn >y                      NB. locale name
 z=. z+(IF64{4 8)*2^6+x          NB. hash table
 z=. z+7!:5 <'p' [ p=. 18!:2 y   NB. path
 z=. z+ (+/spn&> v) + +/ (IF64{24 48) + 7!:5 v=. ,&('_',(>y),'_')&.>(nl__y '')-.;:'x y'
)

sp_z_=: 7!:5

18!:55 <'abc'
(<'abc') -: (h=:3) (18!:3) <'abc'
foot_abc_=: i.3 4
charboil_abc_=: 123$'x'
jajabinks_abc_ =: !100x
(p=: ;:'z base j') 18!:2 <'abc'
NB. (7!:6 <'abc') -: ((spn 'abc')+(4*2^6+h)+sp <'p') + (+/spn&> nl_abc_ '') + +/ 24+sp_abc_ nl_abc_ ''
(7!:6 <'abc') -: h spl <'abc'
18!:55 <'abc'

'locale error'    -: 7!:6 etx <'nonexistent'
'locale error'    -: 7!:6 etx <'123789456'

'domain error'    -: 7!:6 etx <i.4
'domain error'    -: 7!:6 etx <1 2.3 4
'domain error'    -: 7!:6 etx <1 2j3 4
'domain error'    -: 7!:6 etx <u: 'abc'
'domain error'    -: 7!:6 etx <s: ' bc'
'domain error'    -: 7!:6 etx <<'abc'
'domain error'    -: 7!:6 etx i.4
'domain error'    -: 7!:6 etx 1 2.3 4
'domain error'    -: 7!:6 etx 1 2j3 4
'domain error'    -: 7!:6 etx u: 'abc'
'domain error'    -: 7!:6 etx s: ' bc'

'rank error'      -: 7!:5 etx <,:'abc'

'ill-formed name' -: 7!:5 etx <'bad name'
'ill-formed name' -: 7!:5 etx <''


4!:55 ;:'h p sp_z_ spl spn'


