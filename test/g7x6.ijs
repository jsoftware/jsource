prolog './g7x6.ijs'
NB. 7!:6 ----------------------------------------------------------------

(7!:6 <'') -: 7!:6 <'base'

spn=: 3 : '>.&.(2&^.) 64 >. ((>.&.(%&k) 4+4+k+k+4+3)+k*(7+1))+(k-1)+#y [ k=.IF64{4 8'   NB. space needed for a name
NB. >.&.(%&k) 4+4+k+k+4+1+1+1 sizeof(NM)     should be 8=4+4+k+k+4+1+1 offsetof(NM,s)   hash+bucket+bucketx+cachedref+symx+m+flag
NB. 7   header words
NB. 1   shape (no alignment needed)
NB. 1   trailing 0 pad - full word-1 because B01 ops need it, will round up
NB. #y  letters in the name

spl=: 4 : 0   NB. space needed for locale y with hash table size x
 z=. spn >y                      NB. locale name
 z=. z+(4)*2^6+x          NB. hash table
NB.  z=. z+7!:5 <'p' [ p=. 18!:2 y   NB. leave out the path since it's other locales
 z=. z+ (+/spn&> v) + +/ (IF64{16 24) + 7!:5 v=. ,&('_',(>y),'_')&.>(nl__y '')-.;:'x y'  NB. 24 is length of L block
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
18!:55 <'abcdefghijklmno'
(<'abcdefghijklmno') -: (h=:3) (18!:3) <'abcdefghijklmno'
foot_abcdefghijklmno_=: i.3 4
charboil_abcdefghijklmno_=: 123$'x'
jajabinks_abcdefghijklmno_ =: !100x
(p=: ;:'z base j') 18!:2 <'abcdefghijklmno'
NB. (7!:6 <'abc') -: ((spn 'abc')+(4*2^6+h)+sp <'p') + (+/spn&> nl_abc_ '') + +/ 24+sp_abc_ nl_abc_ ''
(7!:6 <'abcdefghijklmno') -: h spl <'abcdefghijklmno'
18!:55 <'abcdefghijklmno'


'locale error'    -: 7!:6 etx <'nonexistent'
'locale error'    -: 7!:6 etx <'123789456'

'domain error'    -: 7!:6 etx <i.4
'domain error'    -: 7!:6 etx <1 2.3 4
'domain error'    -: 7!:6 etx <1 2j3 4
'domain error'    -: 7!:6 etx <u: 'abc'
'domain error'    -: 7!:6 etx <10&u: 'abc'
'domain error'    -: 7!:6 etx <s: ' a b c'
'domain error'    -: 7!:6 etx <<'abc'
'domain error'    -: 7!:6 etx i.4
'domain error'    -: 7!:6 etx 1 2.3 4
'domain error'    -: 7!:6 etx 1 2j3 4
'domain error'    -: 7!:6 etx u:'abc'
'domain error'    -: 7!:6 etx 10&u:'abc'
'domain error'    -: 7!:6 etx s: ' a b c'

'rank error'      -: 7!:5 etx <,:'abc'

'ill-formed name' -: 7!:5 etx <'bad name'
'ill-formed name' -: 7!:5 etx <''

4!:55 ;:'h p sp_z_ spl spn'



epilog''

